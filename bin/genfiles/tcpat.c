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
struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 103
typedef struct _tuple2*Cyc_Absyn_qvar_t;typedef struct _tuple2*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple2*Cyc_Absyn_typedef_name_t;
typedef struct _tuple2*Cyc_Absyn_typedef_name_opt_t;
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 318
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 336
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 349
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 369
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 374
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 430 "absyn.h"
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
# 432
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 445
typedef void*Cyc_Absyn_funcparams_t;
# 448
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 512
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 538
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 545
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 563
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 726 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 919
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 927
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 929
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 933
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 939
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 941
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
# 947
void*Cyc_Absyn_compress_kb(void*);
# 952
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 954
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 957
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 959
extern void*Cyc_Absyn_sint_typ;
# 961
void*Cyc_Absyn_float_typ(int);
# 963
extern void*Cyc_Absyn_empty_effect;
# 1016
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1063
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1067
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
# 1070
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1085
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
# 1177
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
# 92
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);
# 124 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 141
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 181
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,void*r,int resetable,int opened);
# 183
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
# 235 "tcutil.h"
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 238
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr err_msg);
# 253
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 278
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 286
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 291
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 306
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 325
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 370
int Cyc_Tcutil_is_array(void*t);
# 374
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
void Cyc_Tcpat_print_decision_tree(void*);
# 120
int Cyc_Tcpat_has_vars(struct Cyc_Core_Opt*pat_vars);struct _tuple14{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple14 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 52 "tcpat.cyc"
static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Pat*p){
void*_tmp0=p->r;void*_tmp1=_tmp0;struct Cyc_Absyn_Exp*_tmp32;struct Cyc_Absyn_Aggrdecl**_tmp31;struct Cyc_List_List*_tmp30;struct Cyc_List_List**_tmp2F;struct Cyc_List_List*_tmp2E;int _tmp2D;struct Cyc_List_List*_tmp2C;struct Cyc_List_List*_tmp2B;int _tmp2A;switch(*((int*)_tmp1)){case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1 == 0){_LL1: _tmp2C=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f2;_tmp2B=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f3;_tmp2A=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f4;_LL2:
# 59
 if(topt == 0)
({void*_tmp2=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp3="cannot determine pattern type";_tag_dyneither(_tmp3,sizeof(char),30);}),_tag_dyneither(_tmp2,sizeof(void*),0));});{
void*t=Cyc_Tcutil_compress(*((void**)_check_null(topt)));
{void*_tmp4=t;struct Cyc_Absyn_AggrInfo _tmpB;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4)->tag == 11){_LL10: _tmpB=((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4)->f1;_LL11:
# 64
 p->r=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5[0]=({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp6;_tmp6.tag=7;_tmp6.f1=({struct Cyc_Absyn_AggrInfo*_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7[0]=_tmpB;_tmp7;});_tmp6.f2=_tmp2C;_tmp6.f3=_tmp2B;_tmp6.f4=_tmp2A;_tmp6;});_tmp5;});
Cyc_Tcpat_resolve_pat(te,topt,p);
goto _LLF;}else{_LL12: _LL13:
# 68
({struct Cyc_String_pa_PrintArg_struct _tmpA;_tmpA.tag=0;_tmpA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));({void*_tmp8[1]={& _tmpA};Cyc_Tcutil_terr(p->loc,({const char*_tmp9="pattern expects aggregate type instead of %s";_tag_dyneither(_tmp9,sizeof(char),45);}),_tag_dyneither(_tmp8,sizeof(void*),1));});});
goto _LLF;}_LLF:;}
# 72
return;};}else{if(((((struct Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->aggr_info).KnownAggr).tag == 2){_LL3: _tmp31=(((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->aggr_info).KnownAggr).val;_tmp30=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->targs;_tmp2F=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f2;_tmp2E=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f3;_tmp2D=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f4;_LL4: {
# 75
struct Cyc_Absyn_Aggrdecl*_tmpC=*_tmp31;
if(_tmpC->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmpF;_tmpF.tag=0;_tmpF.f1=(struct _dyneither_ptr)(
_tmpC->kind == Cyc_Absyn_StructA?({const char*_tmp10="struct";_tag_dyneither(_tmp10,sizeof(char),7);}):({const char*_tmp11="union";_tag_dyneither(_tmp11,sizeof(char),6);}));({void*_tmpD[1]={& _tmpF};Cyc_Tcutil_terr(p->loc,({const char*_tmpE="can't destructure an abstract %s";_tag_dyneither(_tmpE,sizeof(char),33);}),_tag_dyneither(_tmpD,sizeof(void*),1));});});
p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;}{
# 82
int more_exists=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpC->impl))->exist_vars)- ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*_tmp2F);
if(more_exists < 0){
({void*_tmp12=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp13="too many existentially bound type variables in pattern";_tag_dyneither(_tmp13,sizeof(char),55);}),_tag_dyneither(_tmp12,sizeof(void*),0));});{
struct Cyc_List_List**_tmp14=_tmp2F;
{int n=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpC->impl))->exist_vars);for(0;n != 0;-- n){
_tmp14=&((struct Cyc_List_List*)_check_null(*_tmp14))->tl;}}
*_tmp14=0;};}else{
if(more_exists > 0){
# 91
struct Cyc_List_List*_tmp15=0;
for(0;more_exists != 0;-- more_exists){
_tmp15=({struct Cyc_List_List*_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16->hd=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17[0]=({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp18;_tmp18.tag=1;_tmp18.f1=0;_tmp18;});_tmp17;}));_tmp16->tl=_tmp15;_tmp16;});}
*_tmp2F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp2F,_tmp15);}}
# 96
return;};}}else{_LLB: _LLC:
# 109
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp27=_cycalloc(sizeof(*_tmp27));_tmp27[0]=({struct Cyc_Core_Impossible_exn_struct _tmp28;_tmp28.tag=Cyc_Core_Impossible;_tmp28.f1=({const char*_tmp29="resolve_pat unknownAggr";_tag_dyneither(_tmp29,sizeof(char),24);});_tmp28;});_tmp27;}));}}case 17: _LL5: _tmp32=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1)->f1;_LL6:
# 98
 Cyc_Tcexp_tcExp(te,0,_tmp32);
if(!Cyc_Tcutil_is_const_exp(_tmp32)){
({void*_tmp19=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp1A="non-constant expression in case pattern";_tag_dyneither(_tmp1A,sizeof(char),40);}),_tag_dyneither(_tmp19,sizeof(void*),0));});
p->r=(void*)& Cyc_Absyn_Wild_p_val;}{
# 103
struct _tuple14 _tmp1B=Cyc_Evexp_eval_const_uint_exp(_tmp32);struct _tuple14 _tmp1C=_tmp1B;unsigned int _tmp20;int _tmp1F;_LL15: _tmp20=_tmp1C.f1;_tmp1F=_tmp1C.f2;_LL16:;
p->r=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp1D=_cycalloc_atomic(sizeof(*_tmp1D));_tmp1D[0]=({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmp1E;_tmp1E.tag=10;_tmp1E.f1=Cyc_Absyn_None;_tmp1E.f2=(int)_tmp20;_tmp1E;});_tmp1D;});
return;};case 15: _LL7: _LL8:
# 107
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp21=_cycalloc(sizeof(*_tmp21));_tmp21[0]=({struct Cyc_Core_Impossible_exn_struct _tmp22;_tmp22.tag=Cyc_Core_Impossible;_tmp22.f1=({const char*_tmp23="resolve_pat UnknownId_p";_tag_dyneither(_tmp23,sizeof(char),24);});_tmp22;});_tmp21;}));case 16: _LL9: _LLA:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp24=_cycalloc(sizeof(*_tmp24));_tmp24[0]=({struct Cyc_Core_Impossible_exn_struct _tmp25;_tmp25.tag=Cyc_Core_Impossible;_tmp25.f1=({const char*_tmp26="resolve_pat UnknownCall_p";_tag_dyneither(_tmp26,sizeof(char),26);});_tmp25;});_tmp24;}));default: _LLD: _LLE:
# 111
 return;}_LL0:;}
# 115
static struct _dyneither_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){
return(*vd->name).f2;}
# 118
static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**topt){
if(topt != 0)
return*topt;
return Cyc_Absyn_new_evar(& Cyc_Tcutil_mko,({struct Cyc_Core_Opt*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->v=s;_tmp33;}));}
# 123
static void*Cyc_Tcpat_num_type(void**topt,void*numt){
# 127
if(topt != 0  && Cyc_Tcutil_coerceable(*topt))
return*topt;
# 130
{void*_tmp34=Cyc_Tcutil_compress(numt);void*_tmp35=_tmp34;switch(*((int*)_tmp35)){case 13: _LL18: _LL19:
 goto _LL1B;case 14: _LL1A: _LL1B:
 if(topt != 0)return*topt;goto _LL17;default: _LL1C: _LL1D:
 goto _LL17;}_LL17:;}
# 135
return numt;}struct _tuple15{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 138
static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl**vd,struct Cyc_Absyn_Exp*e,struct Cyc_List_List**v_result_ptr,void*t){
# 140
if(vd != 0){
(*vd)->type=t;
(*vd)->tq=Cyc_Absyn_empty_tqual(0);}
# 144
*v_result_ptr=({struct Cyc_List_List*_tmp36=_cycalloc(sizeof(*_tmp36));_tmp36->hd=({struct _tuple15*_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37->f1=vd;_tmp37->f2=e;_tmp37;});_tmp36->tl=*v_result_ptr;_tmp36;});}
# 146
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,struct Cyc_Tcpat_TcPatResult res2){
# 148
struct Cyc_Tcpat_TcPatResult _tmp38=res1;struct _tuple1*_tmp42;struct Cyc_List_List*_tmp41;_LL1F: _tmp42=_tmp38.tvars_and_bounds_opt;_tmp41=_tmp38.patvars;_LL20:;{
struct Cyc_Tcpat_TcPatResult _tmp39=res2;struct _tuple1*_tmp40;struct Cyc_List_List*_tmp3F;_LL22: _tmp40=_tmp39.tvars_and_bounds_opt;_tmp3F=_tmp39.patvars;_LL23:;
if(_tmp42 != 0  || _tmp40 != 0){
if(_tmp42 == 0)
_tmp42=({struct _tuple1*_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A->f1=0;_tmp3A->f2=0;_tmp3A;});
if(_tmp40 == 0)
_tmp40=({struct _tuple1*_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B->f1=0;_tmp3B->f2=0;_tmp3B;});
return({struct Cyc_Tcpat_TcPatResult _tmp3C;_tmp3C.tvars_and_bounds_opt=({struct _tuple1*_tmp3D=_cycalloc(sizeof(*_tmp3D));_tmp3D->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp42).f1,(*_tmp40).f1);_tmp3D->f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp42).f2,(*_tmp40).f2);_tmp3D;});_tmp3C.patvars=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp41,_tmp3F);_tmp3C;});}
# 159
return({struct Cyc_Tcpat_TcPatResult _tmp3E;_tmp3E.tvars_and_bounds_opt=0;_tmp3E.patvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp41,_tmp3F);_tmp3E;});};}
# 162
static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(unsigned int loc){
return({struct Cyc_Absyn_Pat*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->loc=loc;_tmp43->topt=0;_tmp43->r=(void*)& Cyc_Absyn_Wild_p_val;_tmp43;});}
# 167
static void*Cyc_Tcpat_pat_promote_array(struct Cyc_Tcenv_Tenv*te,void*t,void*rgn_opt){
return Cyc_Tcutil_is_array(t)?
Cyc_Tcutil_promote_array(t,rgn_opt == 0?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,({struct Cyc_Core_Opt*_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44->v=Cyc_Tcenv_lookup_type_vars(te);_tmp44;})): rgn_opt,0): t;}struct _tuple16{struct Cyc_Absyn_Tvar*f1;int f2;};
# 174
static struct _tuple16*Cyc_Tcpat_add_false(struct Cyc_Absyn_Tvar*tv){
return({struct _tuple16*_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45->f1=tv;_tmp45->f2=0;_tmp45;});}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Pat*f2;};
# 178
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void*rgn_pat,int allow_ref_pat,struct Cyc_Absyn_Exp*access_exp){
# 181
Cyc_Tcpat_resolve_pat(te,topt,p);{
void*t;
struct Cyc_Tcpat_TcPatResult res=({struct Cyc_Tcpat_TcPatResult _tmp11D;_tmp11D.tvars_and_bounds_opt=0;_tmp11D.patvars=0;_tmp11D;});
# 186
{void*_tmp46=p->r;void*_tmp47=_tmp46;struct Cyc_Absyn_Datatypedecl*_tmp11C;struct Cyc_Absyn_Datatypefield*_tmp11B;struct Cyc_List_List**_tmp11A;int _tmp119;struct Cyc_Absyn_Aggrdecl*_tmp118;struct Cyc_List_List**_tmp117;struct Cyc_List_List*_tmp116;struct Cyc_List_List**_tmp115;int _tmp114;struct Cyc_List_List**_tmp113;int _tmp112;struct Cyc_Absyn_Pat*_tmp111;void*_tmp110;struct Cyc_Absyn_Enumdecl*_tmp10F;int _tmp10E;struct Cyc_Absyn_Tvar*_tmp10D;struct Cyc_Absyn_Vardecl*_tmp10C;struct Cyc_Absyn_Vardecl*_tmp10B;struct Cyc_Absyn_Pat*_tmp10A;struct Cyc_Absyn_Tvar*_tmp109;struct Cyc_Absyn_Vardecl*_tmp108;struct Cyc_Absyn_Vardecl*_tmp107;struct Cyc_Absyn_Pat*_tmp106;switch(*((int*)_tmp47)){case 0: _LL25: _LL26:
# 189
 if(topt != 0)
t=*topt;else{
# 192
t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(te),topt);}
goto _LL24;case 1: _LL27: _tmp107=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp47)->f1;_tmp106=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp47)->f2;_LL28: {
# 196
struct _tuple2*_tmp48=_tmp107->name;struct _tuple2*_tmp49=_tmp48;struct _dyneither_ptr _tmp56;_LL52: _tmp56=*_tmp49->f2;_LL53:;
if(Cyc_strcmp((struct _dyneither_ptr)_tmp56,({const char*_tmp4A="true";_tag_dyneither(_tmp4A,sizeof(char),5);}))== 0  || Cyc_strcmp((struct _dyneither_ptr)_tmp56,({const char*_tmp4B="false";_tag_dyneither(_tmp4B,sizeof(char),6);}))== 0)
({struct Cyc_String_pa_PrintArg_struct _tmp4E;_tmp4E.tag=0;_tmp4E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp56);({void*_tmp4C[1]={& _tmp4E};Cyc_Tcutil_warn(p->loc,({const char*_tmp4D="you probably do not want to use %s as a local variable...";_tag_dyneither(_tmp4D,sizeof(char),58);}),_tag_dyneither(_tmp4C,sizeof(void*),1));});});
# 200
res=Cyc_Tcpat_tcPatRec(te,_tmp106,topt,rgn_pat,allow_ref_pat,access_exp);
t=(void*)_check_null(_tmp106->topt);
# 204
{void*_tmp4F=Cyc_Tcutil_compress(t);void*_tmp50=_tmp4F;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp50)->tag == 8){_LL55: _LL56:
# 206
 if(rgn_pat == 0  || !allow_ref_pat)
({void*_tmp51=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp52="array reference would point into unknown/unallowed region";_tag_dyneither(_tmp52,sizeof(char),58);}),_tag_dyneither(_tmp51,sizeof(void*),0));});
goto _LL54;}else{_LL57: _LL58:
# 210
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tmk))
({void*_tmp53=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp54="pattern would point to an abstract member";_tag_dyneither(_tmp54,sizeof(char),42);}),_tag_dyneither(_tmp53,sizeof(void*),0));});
goto _LL54;}_LL54:;}
# 214
Cyc_Tcpat_set_vd(({struct Cyc_Absyn_Vardecl**_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55[0]=_tmp107;_tmp55;}),access_exp,& res.patvars,Cyc_Tcpat_pat_promote_array(te,t,rgn_pat));
goto _LL24;}case 2: _LL29: _tmp109=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp47)->f1;_tmp108=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp47)->f2;_LL2A: {
# 217
struct _RegionHandle _tmp57=_new_region("r2");struct _RegionHandle*r2=& _tmp57;_push_region(r2);{
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r2,p->loc,te,({struct Cyc_Absyn_Tvar*_tmp5B[1];_tmp5B[0]=_tmp109;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(r2,_tag_dyneither(_tmp5B,sizeof(struct Cyc_Absyn_Tvar*),1));}));
if(res.tvars_and_bounds_opt == 0)
res.tvars_and_bounds_opt=({struct _tuple1*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58->f1=0;_tmp58->f2=0;_tmp58;});
(*res.tvars_and_bounds_opt).f1=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f1,({struct Cyc_List_List*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59->hd=({struct _tuple16*_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A->f1=_tmp109;_tmp5A->f2=1;_tmp5A;});_tmp59->tl=0;_tmp59;}));
Cyc_Tcutil_check_type(p->loc,te2,Cyc_Tcenv_lookup_type_vars(te2),& Cyc_Tcutil_tmk,1,0,_tmp108->type);}
# 227
if(topt != 0)t=*topt;else{
t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(te),topt);}
{void*_tmp5C=Cyc_Tcutil_compress(t);void*_tmp5D=_tmp5C;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5D)->tag == 8){_LL5A: _LL5B:
# 231
 if(rgn_pat == 0  || !allow_ref_pat)
({void*_tmp5E=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp5F="array reference would point into unknown/unallowed region";_tag_dyneither(_tmp5F,sizeof(char),58);}),_tag_dyneither(_tmp5E,sizeof(void*),0));});
goto _LL59;}else{_LL5C: _LL5D:
# 235
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tmk))
({void*_tmp60=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp61="pattern would point to an abstract member";_tag_dyneither(_tmp61,sizeof(char),42);}),_tag_dyneither(_tmp60,sizeof(void*),0));});
goto _LL59;}_LL59:;}
# 239
Cyc_Tcpat_set_vd(({struct Cyc_Absyn_Vardecl**_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62[0]=_tmp108;_tmp62;}),access_exp,& res.patvars,_tmp108->type);
_npop_handler(0);goto _LL24;
# 217
;_pop_region(r2);}case 3: _LL2B: _tmp10B=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp47)->f1;_tmp10A=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp47)->f2;_LL2C:
# 243
 res=Cyc_Tcpat_tcPatRec(te,_tmp10A,topt,rgn_pat,allow_ref_pat,access_exp);
t=(void*)_check_null(_tmp10A->topt);
if(!allow_ref_pat  || rgn_pat == 0){
({void*_tmp63=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp64="* pattern would point into an unknown/unallowed region";_tag_dyneither(_tmp64,sizeof(char),55);}),_tag_dyneither(_tmp63,sizeof(void*),0));});
goto _LL24;}else{
# 250
struct _RegionHandle _tmp65=_new_region("rgn");struct _RegionHandle*rgn=& _tmp65;_push_region(rgn);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))
({void*_tmp66=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp67="* pattern cannot take the address of an alias-free path";_tag_dyneither(_tmp67,sizeof(char),56);}),_tag_dyneither(_tmp66,sizeof(void*),0));});
# 251
;_pop_region(rgn);}{
# 256
struct Cyc_Absyn_Exp*new_access_exp=0;
void*t2=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp6A;_tmp6A.tag=5;_tmp6A.f1=({struct Cyc_Absyn_PtrInfo _tmp6B;_tmp6B.elt_typ=t;_tmp6B.elt_tq=Cyc_Absyn_empty_tqual(0);_tmp6B.ptr_atts=({(_tmp6B.ptr_atts).rgn=rgn_pat;(_tmp6B.ptr_atts).nullable=Cyc_Absyn_false_conref;(_tmp6B.ptr_atts).bounds=
# 259
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp6B.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp6B.ptr_atts).ptrloc=0;_tmp6B.ptr_atts;});_tmp6B;});_tmp6A;});_tmp69;});
# 261
if((unsigned int)access_exp){
new_access_exp=Cyc_Absyn_address_exp(access_exp,0);
new_access_exp->topt=t2;}
# 265
Cyc_Tcpat_set_vd(({struct Cyc_Absyn_Vardecl**_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68[0]=_tmp10B;_tmp68;}),new_access_exp,& res.patvars,t2);
goto _LL24;};case 4: _LL2D: _tmp10D=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp47)->f1;_tmp10C=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp47)->f2;_LL2E:
# 271
 Cyc_Tcpat_set_vd(({struct Cyc_Absyn_Vardecl**_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C[0]=_tmp10C;_tmp6C;}),access_exp,& res.patvars,_tmp10C->type);{
# 275
struct _RegionHandle _tmp6D=_new_region("r2");struct _RegionHandle*r2=& _tmp6D;_push_region(r2);
Cyc_Tcenv_add_type_vars(r2,p->loc,te,({struct Cyc_Absyn_Tvar*_tmp6E[1];_tmp6E[0]=_tmp10D;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp6E,sizeof(struct Cyc_Absyn_Tvar*),1));}));
# 278
if(res.tvars_and_bounds_opt == 0)
res.tvars_and_bounds_opt=({struct _tuple1*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->f1=0;_tmp6F->f2=0;_tmp6F;});
(*res.tvars_and_bounds_opt).f1=({struct Cyc_List_List*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->hd=({struct _tuple16*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->f1=_tmp10D;_tmp71->f2=0;_tmp71;});_tmp70->tl=(*res.tvars_and_bounds_opt).f1;_tmp70;});
# 282
t=Cyc_Absyn_uint_typ;
_npop_handler(0);goto _LL24;
# 275
;_pop_region(r2);};case 10: switch(((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp47)->f1){case Cyc_Absyn_Unsigned: _LL2F: _LL30:
# 285
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_typ);goto _LL24;case Cyc_Absyn_None: _LL31: _LL32:
 goto _LL34;default: _LL33: _LL34:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_typ);goto _LL24;}case 11: _LL35: _LL36:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_typ);goto _LL24;case 12: _LL37: _tmp10E=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp47)->f2;_LL38:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_float_typ(_tmp10E));goto _LL24;case 13: _LL39: _tmp10F=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp47)->f1;_LL3A:
# 291
 t=Cyc_Tcpat_num_type(topt,(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72[0]=({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp73;_tmp73.tag=13;_tmp73.f1=_tmp10F->name;_tmp73.f2=_tmp10F;_tmp73;});_tmp72;}));
goto _LL24;case 14: _LL3B: _tmp110=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp47)->f1;_LL3C:
 t=Cyc_Tcpat_num_type(topt,_tmp110);goto _LL24;case 9: _LL3D: _LL3E:
# 295
 if(topt != 0){
void*_tmp74=Cyc_Tcutil_compress(*topt);void*_tmp75=_tmp74;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp75)->tag == 5){_LL5F: _LL60:
 t=*topt;goto tcpat_end;}else{_LL61: _LL62:
 goto _LL5E;}_LL5E:;}{
# 300
struct Cyc_Core_Opt*_tmp76=Cyc_Tcenv_lookup_opt_type_vars(te);
t=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp78;_tmp78.tag=5;_tmp78.f1=({struct Cyc_Absyn_PtrInfo _tmp79;_tmp79.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp76);_tmp79.elt_tq=
Cyc_Absyn_empty_tqual(0);_tmp79.ptr_atts=({(_tmp79.ptr_atts).rgn=
Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp76);(_tmp79.ptr_atts).nullable=Cyc_Absyn_true_conref;(_tmp79.ptr_atts).bounds=
# 305
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp79.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp79.ptr_atts).ptrloc=0;_tmp79.ptr_atts;});_tmp79;});_tmp78;});_tmp77;});
# 308
goto _LL24;};case 6: _LL3F: _tmp111=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp47)->f1;_LL40: {
# 313
void*inner_typ=(void*)& Cyc_Absyn_VoidType_val;
void**_tmp7A=0;
int elt_const=0;
if(topt != 0){
void*_tmp7B=Cyc_Tcutil_compress(*topt);void*_tmp7C=_tmp7B;void*_tmp7E;struct Cyc_Absyn_Tqual _tmp7D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7C)->tag == 5){_LL64: _tmp7E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7C)->f1).elt_typ;_tmp7D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7C)->f1).elt_tq;_LL65:
# 319
 inner_typ=_tmp7E;
_tmp7A=& inner_typ;
elt_const=_tmp7D.real_const;
goto _LL63;}else{_LL66: _LL67:
 goto _LL63;}_LL63:;}{
# 328
void*ptr_rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp)new_access_exp=Cyc_Absyn_deref_exp(access_exp,0);
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,_tmp111,_tmp7A,ptr_rgn,1,new_access_exp));
# 337
{void*_tmp7F=Cyc_Tcutil_compress((void*)_check_null(_tmp111->topt));void*_tmp80=_tmp7F;struct Cyc_Absyn_Datatypedecl*_tmp8F;struct Cyc_Absyn_Datatypefield*_tmp8E;struct Cyc_List_List*_tmp8D;if(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp80)->tag == 4){if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp80)->f1).field_info).KnownDatatypefield).tag == 2){_LL69: _tmp8F=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp80)->f1).field_info).KnownDatatypefield).val).f1;_tmp8E=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp80)->f1).field_info).KnownDatatypefield).val).f2;_tmp8D=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp80)->f1).targs;_LL6A:
# 341
{void*_tmp81=Cyc_Tcutil_compress(inner_typ);void*_tmp82=_tmp81;if(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp82)->tag == 4){_LL6E: _LL6F:
# 343
 goto DONT_PROMOTE;}else{_LL70: _LL71:
 goto _LL6D;}_LL6D:;}{
# 347
void*new_type=(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86[0]=({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp87;_tmp87.tag=3;_tmp87.f1=({struct Cyc_Absyn_DatatypeInfo _tmp88;_tmp88.datatype_info=Cyc_Absyn_KnownDatatype(({struct Cyc_Absyn_Datatypedecl**_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89[0]=_tmp8F;_tmp89;}));_tmp88.targs=_tmp8D;_tmp88;});_tmp87;});_tmp86;});
# 350
_tmp111->topt=new_type;
t=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp84;_tmp84.tag=5;_tmp84.f1=({struct Cyc_Absyn_PtrInfo _tmp85;_tmp85.elt_typ=new_type;_tmp85.elt_tq=
elt_const?Cyc_Absyn_const_tqual(0):
 Cyc_Absyn_empty_tqual(0);_tmp85.ptr_atts=({(_tmp85.ptr_atts).rgn=ptr_rgn;(_tmp85.ptr_atts).nullable=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp85.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref;(_tmp85.ptr_atts).zero_term=Cyc_Absyn_false_conref;(_tmp85.ptr_atts).ptrloc=0;_tmp85.ptr_atts;});_tmp85;});_tmp84;});_tmp83;});
# 357
goto _LL68;};}else{goto _LL6B;}}else{_LL6B: _LL6C:
# 359
 DONT_PROMOTE:
 t=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp8B;_tmp8B.tag=5;_tmp8B.f1=({struct Cyc_Absyn_PtrInfo _tmp8C;_tmp8C.elt_typ=(void*)_check_null(_tmp111->topt);_tmp8C.elt_tq=
elt_const?Cyc_Absyn_const_tqual(0):
 Cyc_Absyn_empty_tqual(0);_tmp8C.ptr_atts=({(_tmp8C.ptr_atts).rgn=ptr_rgn;(_tmp8C.ptr_atts).nullable=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp8C.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp8C.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp8C.ptr_atts).ptrloc=0;_tmp8C.ptr_atts;});_tmp8C;});_tmp8B;});_tmp8A;});}_LL68:;}
# 367
if((unsigned int)new_access_exp)new_access_exp->topt=_tmp111->topt;
goto _LL24;};}case 5: _LL41: _tmp113=(struct Cyc_List_List**)&((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp47)->f1;_tmp112=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp47)->f2;_LL42: {
# 371
struct Cyc_List_List*_tmp90=*_tmp113;
struct Cyc_List_List*pat_ts=0;
struct Cyc_List_List*topt_ts=0;
if(topt != 0){
void*_tmp91=Cyc_Tcutil_compress(*topt);void*_tmp92=_tmp91;struct Cyc_List_List*_tmp98;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp92)->tag == 10){_LL73: _tmp98=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp92)->f1;_LL74:
# 377
 topt_ts=_tmp98;
if(_tmp112){
# 380
int _tmp93=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp90);
int _tmp94=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp98);
if(_tmp93 < _tmp94){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmp94 - _tmp93;++ i){
wild_ps=({struct Cyc_List_List*_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95->hd=Cyc_Tcpat_wild_pat(p->loc);_tmp95->tl=wild_ps;_tmp95;});}}
*_tmp113=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp90,wild_ps);
_tmp90=*_tmp113;}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp90)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp98))
({void*_tmp96=0;Cyc_Tcutil_warn(p->loc,({const char*_tmp97="unnecessary ... in tuple pattern";_tag_dyneither(_tmp97,sizeof(char),33);}),_tag_dyneither(_tmp96,sizeof(void*),0));});}}
# 392
goto _LL72;}else{_LL75: _LL76:
# 394
 goto _LL72;}_LL72:;}else{
# 396
if(_tmp112)
({void*_tmp99=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp9A="cannot determine missing fields for ... in tuple pattern";_tag_dyneither(_tmp9A,sizeof(char),57);}),_tag_dyneither(_tmp99,sizeof(void*),0));});}
{int i=0;for(0;_tmp90 != 0;(_tmp90=_tmp90->tl,i ++)){
void**_tmp9B=0;
if(topt_ts != 0){
_tmp9B=&(*((struct _tuple17*)topt_ts->hd)).f2;
topt_ts=topt_ts->tl;}{
# 404
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp)
new_access_exp=Cyc_Absyn_subscript_exp(access_exp,
Cyc_Absyn_const_exp(({union Cyc_Absyn_Cnst _tmp9C;(_tmp9C.Int_c).val=({struct _tuple7 _tmp9D;_tmp9D.f1=Cyc_Absyn_Unsigned;_tmp9D.f2=i;_tmp9D;});(_tmp9C.Int_c).tag=5;_tmp9C;}),0),0);
# 409
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)_tmp90->hd,_tmp9B,rgn_pat,allow_ref_pat,new_access_exp));
# 412
if((unsigned int)new_access_exp)new_access_exp->topt=((struct Cyc_Absyn_Pat*)_tmp90->hd)->topt;
pat_ts=({struct Cyc_List_List*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->hd=({struct _tuple17*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F->f1=Cyc_Absyn_empty_tqual(0);_tmp9F->f2=(void*)_check_null(((struct Cyc_Absyn_Pat*)_tmp90->hd)->topt);_tmp9F;});_tmp9E->tl=pat_ts;_tmp9E;});};}}
# 415
t=(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0[0]=({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpA1;_tmpA1.tag=10;_tmpA1.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(pat_ts);_tmpA1;});_tmpA0;});
goto _LL24;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp47)->f1 != 0){if(((((struct Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp47)->f1)->aggr_info).KnownAggr).tag == 2){_LL43: _tmp118=*(((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp47)->f1)->aggr_info).KnownAggr).val;_tmp117=(struct Cyc_List_List**)&(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp47)->f1)->targs;_tmp116=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp47)->f2;_tmp115=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp47)->f3;_tmp114=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp47)->f4;_LL44: {
# 419
struct Cyc_List_List*_tmpA2=*_tmp115;
struct _dyneither_ptr aggr_str=_tmp118->kind == Cyc_Absyn_StructA?({const char*_tmpE7="struct";_tag_dyneither(_tmpE7,sizeof(char),7);}):({const char*_tmpE8="union";_tag_dyneither(_tmpE8,sizeof(char),6);});
if(_tmp118->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmpA5;_tmpA5.tag=0;_tmpA5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);({void*_tmpA3[1]={& _tmpA5};Cyc_Tcutil_terr(p->loc,({const char*_tmpA4="can't destructure an abstract %s";_tag_dyneither(_tmpA4,sizeof(char),33);}),_tag_dyneither(_tmpA3,sizeof(void*),1));});});
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));
goto _LL24;}
# 428
if(_tmp118->kind == Cyc_Absyn_UnionA  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp118->impl))->tagged)
allow_ref_pat=0;
if(_tmp116 != 0){
# 434
if(topt == 0  || Cyc_Tcutil_typ_kind(*topt)!= & Cyc_Tcutil_ak)
allow_ref_pat=0;}{
# 437
struct _RegionHandle _tmpA6=_new_region("rgn");struct _RegionHandle*rgn=& _tmpA6;_push_region(rgn);{
# 439
struct Cyc_List_List*_tmpA7=0;
struct Cyc_List_List*outlives_constraints=0;
struct Cyc_List_List*_tmpA8=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp118->impl))->exist_vars;
{struct Cyc_List_List*t=_tmp116;for(0;t != 0;t=t->tl){
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;
struct Cyc_Absyn_Tvar*uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmpA8))->hd;
_tmpA8=_tmpA8->tl;{
void*_tmpA9=Cyc_Absyn_compress_kb(tv->kind);
void*_tmpAA=Cyc_Absyn_compress_kb(uv->kind);
int error=0;
struct Cyc_Absyn_Kind*k2;
{void*_tmpAB=_tmpAA;struct Cyc_Absyn_Kind*_tmpAF;struct Cyc_Absyn_Kind*_tmpAE;switch(*((int*)_tmpAB)){case 2: _LL78: _tmpAE=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpAB)->f2;_LL79:
 _tmpAF=_tmpAE;goto _LL7B;case 0: _LL7A: _tmpAF=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpAB)->f1;_LL7B:
 k2=_tmpAF;goto _LL77;default: _LL7C: _LL7D:
({void*_tmpAC=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpAD="unconstrained existential type variable in aggregate";_tag_dyneither(_tmpAD,sizeof(char),53);}),_tag_dyneither(_tmpAC,sizeof(void*),0));});}_LL77:;}
# 455
{void*_tmpB0=_tmpA9;struct Cyc_Core_Opt**_tmpB5;struct Cyc_Core_Opt**_tmpB4;struct Cyc_Absyn_Kind*_tmpB3;struct Cyc_Absyn_Kind*_tmpB2;switch(*((int*)_tmpB0)){case 0: _LL7F: _tmpB2=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpB0)->f1;_LL80:
# 458
 if(!Cyc_Tcutil_kind_leq(k2,_tmpB2))
error=1;
goto _LL7E;case 2: _LL81: _tmpB4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB0)->f1;_tmpB3=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB0)->f2;_LL82:
 _tmpB5=_tmpB4;goto _LL84;default: _LL83: _tmpB5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpB0)->f1;_LL84:
*_tmpB5=({struct Cyc_Core_Opt*_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1->v=_tmpAA;_tmpB1;});goto _LL7E;}_LL7E:;}
# 464
if(error)
({struct Cyc_String_pa_PrintArg_struct _tmpBA;_tmpBA.tag=0;_tmpBA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 468
Cyc_Absynpp_kind2string(k2));({struct Cyc_String_pa_PrintArg_struct _tmpB9;_tmpB9.tag=0;_tmpB9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(_tmpA9));({struct Cyc_String_pa_PrintArg_struct _tmpB8;_tmpB8.tag=0;_tmpB8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name);({void*_tmpB6[3]={& _tmpB8,& _tmpB9,& _tmpBA};Cyc_Tcutil_terr(p->loc,({const char*_tmpB7="type variable %s has kind %s but must have at least kind %s";_tag_dyneither(_tmpB7,sizeof(char),60);}),_tag_dyneither(_tmpB6,sizeof(void*),3));});});});});{
# 470
void*vartype=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpC0=_cycalloc(sizeof(*_tmpC0));_tmpC0[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpC1;_tmpC1.tag=2;_tmpC1.f1=tv;_tmpC1;});_tmpC0;});
_tmpA7=({struct Cyc_List_List*_tmpBB=_region_malloc(rgn,sizeof(*_tmpBB));_tmpBB->hd=vartype;_tmpBB->tl=_tmpA7;_tmpBB;});
# 474
if(k2->kind == Cyc_Absyn_RgnKind){
if(k2->aliasqual == Cyc_Absyn_Aliasable)
outlives_constraints=({struct Cyc_List_List*_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC->hd=({struct _tuple0*_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD->f1=Cyc_Absyn_empty_effect;_tmpBD->f2=vartype;_tmpBD;});_tmpBC->tl=outlives_constraints;_tmpBC;});else{
# 479
({void*_tmpBE=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpBF="opened existential had unique or top region kind";_tag_dyneither(_tmpBF,sizeof(char),49);}),_tag_dyneither(_tmpBE,sizeof(void*),0));});}}};};}}
# 483
_tmpA7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpA7);{
# 485
struct _RegionHandle _tmpC2=_new_region("r2");struct _RegionHandle*r2=& _tmpC2;_push_region(r2);
{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r2,p->loc,te,_tmp116);
# 488
struct Cyc_List_List*_tmpC3=Cyc_Tcenv_lookup_type_vars(te2);
struct _tuple12 _tmpC4=({struct _tuple12 _tmpE6;_tmpE6.f1=_tmpC3;_tmpE6.f2=rgn;_tmpE6;});
struct Cyc_List_List*_tmpC5=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmpC4,_tmp118->tvs);
struct Cyc_List_List*_tmpC6=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp118->impl))->exist_vars,_tmpA7);
struct Cyc_List_List*_tmpC7=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmpC5);
struct Cyc_List_List*_tmpC8=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmpC6);
struct Cyc_List_List*_tmpC9=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmpC5,_tmpC6);
# 496
if(_tmp116 != 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp118->impl))->rgn_po != 0){
if(res.tvars_and_bounds_opt == 0)
res.tvars_and_bounds_opt=({struct _tuple1*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->f1=0;_tmpCA->f2=0;_tmpCA;});
(*res.tvars_and_bounds_opt).f1=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f1,((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_add_false,_tmp116));
(*res.tvars_and_bounds_opt).f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,outlives_constraints);{
struct Cyc_List_List*_tmpCB=0;
{struct Cyc_List_List*_tmpCC=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp118->impl))->rgn_po;for(0;_tmpCC != 0;_tmpCC=_tmpCC->tl){
_tmpCB=({struct Cyc_List_List*_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD->hd=({struct _tuple0*_tmpCE=_cycalloc(sizeof(*_tmpCE));_tmpCE->f1=Cyc_Tcutil_rsubstitute(rgn,_tmpC9,(*((struct _tuple0*)_tmpCC->hd)).f1);_tmpCE->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmpC9,(*((struct _tuple0*)_tmpCC->hd)).f2);_tmpCE;});_tmpCD->tl=_tmpCB;_tmpCD;});}}
# 508
_tmpCB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpCB);
(*res.tvars_and_bounds_opt).f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,_tmpCB);};}
# 513
*_tmp117=_tmpC7;
t=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpD0;_tmpD0.tag=11;_tmpD0.f1=({struct Cyc_Absyn_AggrInfo _tmpD1;_tmpD1.aggr_info=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2[0]=_tmp118;_tmpD2;}));_tmpD1.targs=*_tmp117;_tmpD1;});_tmpD0;});_tmpCF;});
if(_tmp114  && _tmp118->kind == Cyc_Absyn_UnionA)
({void*_tmpD3=0;Cyc_Tcutil_warn(p->loc,({const char*_tmpD4="`...' pattern not allowed in union pattern";_tag_dyneither(_tmpD4,sizeof(char),43);}),_tag_dyneither(_tmpD3,sizeof(void*),0));});else{
if(_tmp114){
# 519
int _tmpD5=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpA2);
int _tmpD6=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp118->impl))->fields);
if(_tmpD5 < _tmpD6){
struct Cyc_List_List*wild_dps=0;
{int i=0;for(0;i < _tmpD6 - _tmpD5;++ i){
wild_dps=({struct Cyc_List_List*_tmpD7=_cycalloc(sizeof(*_tmpD7));_tmpD7->hd=({struct _tuple18*_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8->f1=0;_tmpD8->f2=Cyc_Tcpat_wild_pat(p->loc);_tmpD8;});_tmpD7->tl=wild_dps;_tmpD7;});}}
*_tmp115=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpA2,wild_dps);
_tmpA2=*_tmp115;}else{
if(_tmpD5 == _tmpD6)
({void*_tmpD9=0;Cyc_Tcutil_warn(p->loc,({const char*_tmpDA="unnecessary ... in struct pattern";_tag_dyneither(_tmpDA,sizeof(char),34);}),_tag_dyneither(_tmpD9,sizeof(void*),0));});}}}{
# 530
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,p->loc,_tmpA2,_tmp118->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp118->impl))->fields);
for(0;fields != 0;fields=fields->tl){
struct _tuple19*_tmpDB=(struct _tuple19*)fields->hd;struct _tuple19*_tmpDC=_tmpDB;struct Cyc_Absyn_Aggrfield*_tmpE5;struct Cyc_Absyn_Pat*_tmpE4;_LL86: _tmpE5=_tmpDC->f1;_tmpE4=_tmpDC->f2;_LL87:;{
void*_tmpDD=Cyc_Tcutil_rsubstitute(rgn,_tmpC9,_tmpE5->type);
# 536
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp)
new_access_exp=Cyc_Absyn_aggrmember_exp(access_exp,_tmpE5->name,0);
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te2,_tmpE4,& _tmpDD,rgn_pat,allow_ref_pat,new_access_exp));
# 544
if(!Cyc_Tcutil_unify((void*)_check_null(_tmpE4->topt),_tmpDD))
({struct Cyc_String_pa_PrintArg_struct _tmpE3;_tmpE3.tag=0;_tmpE3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 547
Cyc_Absynpp_typ2string((void*)_check_null(_tmpE4->topt)));({struct Cyc_String_pa_PrintArg_struct _tmpE2;_tmpE2.tag=0;_tmpE2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 546
Cyc_Absynpp_typ2string(_tmpDD));({struct Cyc_String_pa_PrintArg_struct _tmpE1;_tmpE1.tag=0;_tmpE1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);({struct Cyc_String_pa_PrintArg_struct _tmpE0;_tmpE0.tag=0;_tmpE0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpE5->name);({void*_tmpDE[4]={& _tmpE0,& _tmpE1,& _tmpE2,& _tmpE3};Cyc_Tcutil_terr(p->loc,({const char*_tmpDF="field %s of %s pattern expects type %s != %s";_tag_dyneither(_tmpDF,sizeof(char),45);}),_tag_dyneither(_tmpDE,sizeof(void*),4));});});});});});
# 548
if((unsigned int)new_access_exp)new_access_exp->topt=_tmpE4->topt;};}};}
# 486
;_pop_region(r2);};}
# 552
_npop_handler(0);goto _LL24;
# 437
;_pop_region(rgn);};}}else{_LL49: _LL4A:
# 608
 goto _LL4C;}}else{_LL47: _LL48:
# 607
 goto _LL4A;}case 8: _LL45: _tmp11C=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp47)->f1;_tmp11B=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp47)->f2;_tmp11A=(struct Cyc_List_List**)&((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp47)->f3;_tmp119=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp47)->f4;_LL46: {
# 555
struct Cyc_List_List*_tmpE9=*_tmp11A;
struct _RegionHandle _tmpEA=_new_region("rgn");struct _RegionHandle*rgn=& _tmpEA;_push_region(rgn);{
struct Cyc_List_List*tqts=_tmp11B->typs;
# 559
struct Cyc_List_List*_tmpEB=Cyc_Tcenv_lookup_type_vars(te);
struct _tuple12 _tmpEC=({struct _tuple12 _tmp105;_tmp105.f1=_tmpEB;_tmp105.f2=rgn;_tmp105;});
struct Cyc_List_List*_tmpED=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmpEC,_tmp11C->tvs);
struct Cyc_List_List*_tmpEE=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmpED);
t=(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpEF=_cycalloc(sizeof(*_tmpEF));_tmpEF[0]=({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpF0;_tmpF0.tag=4;_tmpF0.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmpF1;_tmpF1.field_info=Cyc_Absyn_KnownDatatypefield(_tmp11C,_tmp11B);_tmpF1.targs=_tmpEE;_tmpF1;});_tmpF0;});_tmpEF;});
# 565
if(_tmp119){
# 567
int _tmpF2=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpE9);
int _tmpF3=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tqts);
if(_tmpF2 < _tmpF3){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmpF3 - _tmpF2;++ i){
wild_ps=({struct Cyc_List_List*_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4->hd=Cyc_Tcpat_wild_pat(p->loc);_tmpF4->tl=wild_ps;_tmpF4;});}}
*_tmp11A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpE9,wild_ps);
_tmpE9=*_tmp11A;}else{
if(_tmpF2 == _tmpF3)
({struct Cyc_String_pa_PrintArg_struct _tmpF7;_tmpF7.tag=0;_tmpF7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp11C->name));({void*_tmpF5[1]={& _tmpF7};Cyc_Tcutil_warn(p->loc,({const char*_tmpF6="unnecessary ... in datatype field %s";_tag_dyneither(_tmpF6,sizeof(char),37);}),_tag_dyneither(_tmpF5,sizeof(void*),1));});});}}
# 579
for(0;_tmpE9 != 0  && tqts != 0;(_tmpE9=_tmpE9->tl,tqts=tqts->tl)){
struct Cyc_Absyn_Pat*_tmpF8=(struct Cyc_Absyn_Pat*)_tmpE9->hd;
# 583
void*_tmpF9=Cyc_Tcutil_rsubstitute(rgn,_tmpED,(*((struct _tuple17*)tqts->hd)).f2);
# 586
if((unsigned int)access_exp)
Cyc_Tcpat_set_vd(0,access_exp,& res.patvars,Cyc_Absyn_char_typ);
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,_tmpF8,& _tmpF9,rgn_pat,allow_ref_pat,0));
# 593
if(!Cyc_Tcutil_unify((void*)_check_null(_tmpF8->topt),_tmpF9))
({struct Cyc_String_pa_PrintArg_struct _tmpFE;_tmpFE.tag=0;_tmpFE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 596
Cyc_Absynpp_typ2string((void*)_check_null(_tmpF8->topt)));({struct Cyc_String_pa_PrintArg_struct _tmpFD;_tmpFD.tag=0;_tmpFD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 595
Cyc_Absynpp_typ2string(_tmpF9));({struct Cyc_String_pa_PrintArg_struct _tmpFC;_tmpFC.tag=0;_tmpFC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp11B->name));({void*_tmpFA[3]={& _tmpFC,& _tmpFD,& _tmpFE};Cyc_Tcutil_terr(_tmpF8->loc,({const char*_tmpFB="%s expects argument type %s, not %s";_tag_dyneither(_tmpFB,sizeof(char),36);}),_tag_dyneither(_tmpFA,sizeof(void*),3));});});});});}
# 598
if(_tmpE9 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp101;_tmp101.tag=0;_tmp101.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp11B->name));({void*_tmpFF[1]={& _tmp101};Cyc_Tcutil_terr(p->loc,({const char*_tmp100="too many arguments for datatype constructor %s";_tag_dyneither(_tmp100,sizeof(char),47);}),_tag_dyneither(_tmpFF,sizeof(void*),1));});});
if(tqts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp104;_tmp104.tag=0;_tmp104.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp11B->name));({void*_tmp102[1]={& _tmp104};Cyc_Tcutil_terr(p->loc,({const char*_tmp103="too few arguments for datatype constructor %s";_tag_dyneither(_tmp103,sizeof(char),46);}),_tag_dyneither(_tmp102,sizeof(void*),1));});});}
# 605
_npop_handler(0);goto _LL24;
# 556
;_pop_region(rgn);}case 15: _LL4B: _LL4C:
# 609
 goto _LL4E;case 17: _LL4D: _LL4E:
 goto _LL50;default: _LL4F: _LL50:
# 612
 t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));goto _LL24;}_LL24:;}
# 614
tcpat_end:
 p->topt=t;
return res;};}
# 619
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,struct Cyc_Absyn_Exp*pat_var_exp){
# 621
struct Cyc_Tcpat_TcPatResult _tmp11E=Cyc_Tcpat_tcPatRec(te,p,topt,0,0,pat_var_exp);
# 623
struct _RegionHandle _tmp11F=_new_region("r");struct _RegionHandle*r=& _tmp11F;_push_region(r);{
struct _tuple1 _tmp120=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(r,r,_tmp11E.patvars);struct _tuple1 _tmp121=_tmp120;struct Cyc_List_List*_tmp129;_LL89: _tmp129=_tmp121.f1;_LL8A:;{
struct Cyc_List_List*_tmp122=0;
{struct Cyc_List_List*x=_tmp129;for(0;x != 0;x=x->tl){
if((struct Cyc_Absyn_Vardecl**)x->hd != 0)_tmp122=({struct Cyc_List_List*_tmp123=_region_malloc(r,sizeof(*_tmp123));_tmp123->hd=*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)x->hd));_tmp123->tl=_tmp122;_tmp123;});}}
Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Tcpat_get_name,_tmp122),p->loc,({const char*_tmp124="pattern contains a repeated variable";_tag_dyneither(_tmp124,sizeof(char),37);}));{
# 633
struct Cyc_List_List*x=_tmp11E.patvars;for(0;x != 0;x=x->tl){
struct _tuple15*_tmp125=(struct _tuple15*)x->hd;struct _tuple15*_tmp126=_tmp125;struct Cyc_Absyn_Vardecl**_tmp128;struct Cyc_Absyn_Exp**_tmp127;_LL8C: _tmp128=_tmp126->f1;_tmp127=(struct Cyc_Absyn_Exp**)& _tmp126->f2;_LL8D:;
if(*_tmp127 != 0  && *_tmp127 != pat_var_exp)
*_tmp127=Cyc_Tcutil_deep_copy_exp(1,(struct Cyc_Absyn_Exp*)_check_null(*_tmp127));}};};}{
# 639
struct Cyc_Tcpat_TcPatResult _tmp12A=_tmp11E;_npop_handler(0);return _tmp12A;};
# 623
;_pop_region(r);}
# 645
static int Cyc_Tcpat_try_alias_coerce(struct Cyc_Tcenv_Tenv*tenv,void*old_type,void*new_type,struct Cyc_Absyn_Exp*initializer,struct Cyc_List_List*assump){
# 648
struct _tuple0 _tmp12B=({struct _tuple0 _tmp133;_tmp133.f1=Cyc_Tcutil_compress(old_type);_tmp133.f2=Cyc_Tcutil_compress(new_type);_tmp133;});struct _tuple0 _tmp12C=_tmp12B;struct Cyc_Absyn_PtrInfo _tmp132;struct Cyc_Absyn_PtrInfo _tmp131;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12C.f1)->tag == 5){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12C.f2)->tag == 5){_LL8F: _tmp132=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12C.f1)->f1;_tmp131=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12C.f2)->f1;_LL90: {
# 650
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp12D=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp12E=_cycalloc(sizeof(*_tmp12E));_tmp12E[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp12F;_tmp12F.tag=5;_tmp12F.f1=({struct Cyc_Absyn_PtrInfo _tmp130;_tmp130.elt_typ=_tmp132.elt_typ;_tmp130.elt_tq=_tmp131.elt_tq;_tmp130.ptr_atts=({(_tmp130.ptr_atts).rgn=(_tmp132.ptr_atts).rgn;(_tmp130.ptr_atts).nullable=(_tmp131.ptr_atts).nullable;(_tmp130.ptr_atts).bounds=(_tmp131.ptr_atts).bounds;(_tmp130.ptr_atts).zero_term=(_tmp131.ptr_atts).zero_term;(_tmp130.ptr_atts).ptrloc=(_tmp132.ptr_atts).ptrloc;_tmp130.ptr_atts;});_tmp130;});_tmp12F;});_tmp12E;});
# 656
return Cyc_Tcutil_subtype(tenv,assump,(void*)_tmp12D,new_type) && 
Cyc_Tcutil_coerce_assign(tenv,initializer,(void*)_tmp12D);}}else{goto _LL91;}}else{_LL91: _LL92:
 return 0;}_LL8E:;}
# 665
static void Cyc_Tcpat_check_alias_coercion(struct Cyc_Tcenv_Tenv*tenv,unsigned int loc,void*old_type,struct Cyc_Absyn_Tvar*tv,void*new_type,struct Cyc_Absyn_Exp*initializer){
# 668
struct Cyc_List_List*assump=({struct Cyc_List_List*_tmp13F=_cycalloc(sizeof(*_tmp13F));_tmp13F->hd=({struct _tuple0*_tmp140=_cycalloc(sizeof(*_tmp140));_tmp140->f1=(void*)& Cyc_Absyn_UniqueRgn_val;_tmp140->f2=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp141=_cycalloc(sizeof(*_tmp141));_tmp141[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp142;_tmp142.tag=2;_tmp142.f1=tv;_tmp142;});_tmp141;});_tmp140;});_tmp13F->tl=0;_tmp13F;});
if(Cyc_Tcutil_subtype(tenv,assump,old_type,new_type)){
# 686 "tcpat.cyc"
struct _tuple0 _tmp134=({struct _tuple0 _tmp13A;_tmp13A.f1=Cyc_Tcutil_compress(old_type);_tmp13A.f2=Cyc_Tcutil_compress(new_type);_tmp13A;});struct _tuple0 _tmp135=_tmp134;struct Cyc_Absyn_PtrInfo _tmp139;struct Cyc_Absyn_PtrInfo _tmp138;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp135.f1)->tag == 5){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp135.f2)->tag == 5){_LL94: _tmp139=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp135.f1)->f1;_tmp138=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp135.f2)->f1;_LL95:
 goto _LL93;}else{goto _LL96;}}else{_LL96: _LL97:
({void*_tmp136=0;Cyc_Tcutil_terr(loc,({const char*_tmp137="alias requires pointer type";_tag_dyneither(_tmp137,sizeof(char),28);}),_tag_dyneither(_tmp136,sizeof(void*),0));});goto _LL93;}_LL93:;}else{
# 691
({struct Cyc_String_pa_PrintArg_struct _tmp13E;_tmp13E.tag=0;_tmp13E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(new_type));({struct Cyc_String_pa_PrintArg_struct _tmp13D;_tmp13D.tag=0;_tmp13D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(old_type));({void*_tmp13B[2]={& _tmp13D,& _tmp13E};Cyc_Tcutil_terr(loc,({const char*_tmp13C="cannot alias value of type %s to type %s";_tag_dyneither(_tmp13C,sizeof(char),41);}),_tag_dyneither(_tmp13B,sizeof(void*),2));});});});}}
# 698
void Cyc_Tcpat_check_pat_regions_rec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,int did_noalias_deref,struct Cyc_List_List*patvars){
# 702
void*_tmp143=p->r;void*_tmp144=_tmp143;struct Cyc_Absyn_Tvar*_tmp16E;struct Cyc_Absyn_Vardecl*_tmp16D;struct Cyc_Absyn_Vardecl*_tmp16C;struct Cyc_Absyn_Pat*_tmp16B;struct Cyc_Absyn_Vardecl*_tmp16A;struct Cyc_Absyn_Pat*_tmp169;struct Cyc_List_List*_tmp168;struct Cyc_List_List*_tmp167;struct Cyc_Absyn_AggrInfo*_tmp166;struct Cyc_List_List*_tmp165;struct Cyc_List_List*_tmp164;struct Cyc_Absyn_Pat*_tmp163;switch(*((int*)_tmp144)){case 6: _LL99: _tmp163=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp144)->f1;_LL9A: {
# 704
void*_tmp145=(void*)_check_null(p->topt);void*_tmp146=_tmp145;void*_tmp149;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp146)->tag == 5){_LLAA: _tmp149=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp146)->f1).ptr_atts).rgn;_LLAB:
# 706
 Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp149);
Cyc_Tcpat_check_pat_regions_rec(te,_tmp163,Cyc_Tcutil_is_noalias_region(_tmp149,0),patvars);
return;}else{_LLAC: _LLAD:
({void*_tmp147=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp148="check_pat_regions: bad pointer type";_tag_dyneither(_tmp148,sizeof(char),36);}),_tag_dyneither(_tmp147,sizeof(void*),0));});}_LLA9:;}case 7: _LL9B: _tmp166=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp144)->f1;_tmp165=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp144)->f2;_tmp164=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp144)->f3;_LL9C:
# 712
 for(0;_tmp164 != 0;_tmp164=_tmp164->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(*((struct _tuple18*)_tmp164->hd)).f2,did_noalias_deref,patvars);}
return;case 8: _LL9D: _tmp167=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp144)->f3;_LL9E:
# 716
 did_noalias_deref=0;_tmp168=_tmp167;goto _LLA0;case 5: _LL9F: _tmp168=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp144)->f1;_LLA0:
# 718
 for(0;_tmp168 != 0;_tmp168=_tmp168->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(struct Cyc_Absyn_Pat*)_tmp168->hd,did_noalias_deref,patvars);}
return;case 3: _LLA1: _tmp16A=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp144)->f1;_tmp169=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp144)->f2;_LLA2:
# 722
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple15*_tmp14A=(struct _tuple15*)x->hd;struct _tuple15*_tmp14B=_tmp14A;struct Cyc_Absyn_Vardecl**_tmp154;struct Cyc_Absyn_Exp*_tmp153;_LLAF: _tmp154=_tmp14B->f1;_tmp153=_tmp14B->f2;_LLB0:;
# 728
if((_tmp154 != 0  && *_tmp154 == _tmp16A) && _tmp153 != 0){
{void*_tmp14C=_tmp153->r;void*_tmp14D=_tmp14C;struct Cyc_Absyn_Exp*_tmp152;if(((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp14D)->tag == 14){_LLB2: _tmp152=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp14D)->f1;_LLB3:
# 731
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr(Cyc_Tcenv_get_fnrgn(te),(void*)_check_null(_tmp152->topt)))
# 733
({void*_tmp14E=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp14F="reference pattern not allowed on alias-free pointers";_tag_dyneither(_tmp14F,sizeof(char),53);}),_tag_dyneither(_tmp14E,sizeof(void*),0));});
goto _LLB1;}else{_LLB4: _LLB5:
# 736
({void*_tmp150=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp151="check_pat_regions: bad reference access expression";_tag_dyneither(_tmp151,sizeof(char),51);}),_tag_dyneither(_tmp150,sizeof(void*),0));});}_LLB1:;}
# 738
break;}}}
# 741
Cyc_Tcpat_check_pat_regions_rec(te,_tmp169,did_noalias_deref,patvars);
return;case 1: _LLA3: _tmp16C=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp144)->f1;_tmp16B=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp144)->f2;_LLA4:
# 744
{void*_tmp155=p->topt;void*_tmp156=_tmp155;if(_tmp156 != 0){if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp156)->tag == 8){_LLB7: _LLB8:
# 746
 if(did_noalias_deref){
({void*_tmp157=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp158="pattern to array would create alias of no-alias pointer";_tag_dyneither(_tmp158,sizeof(char),56);}),_tag_dyneither(_tmp157,sizeof(void*),0));});
return;}
# 750
goto _LLB6;}else{goto _LLB9;}}else{_LLB9: _LLBA:
 goto _LLB6;}_LLB6:;}
# 753
Cyc_Tcpat_check_pat_regions_rec(te,_tmp16B,did_noalias_deref,patvars);
return;case 2: _LLA5: _tmp16E=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp144)->f1;_tmp16D=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp144)->f2;_LLA6:
# 756
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple15*_tmp159=(struct _tuple15*)x->hd;struct _tuple15*_tmp15A=_tmp159;struct Cyc_Absyn_Vardecl**_tmp162;struct Cyc_Absyn_Exp*_tmp161;_LLBC: _tmp162=_tmp15A->f1;_tmp161=_tmp15A->f2;_LLBD:;
# 760
if(_tmp162 != 0  && *_tmp162 == _tmp16D){
if(_tmp161 == 0)
({void*_tmp15B=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp15C="cannot alias pattern expression in datatype";_tag_dyneither(_tmp15C,sizeof(char),44);}),_tag_dyneither(_tmp15B,sizeof(void*),0));});else{
# 764
struct _RegionHandle _tmp15D=_new_region("r");struct _RegionHandle*r=& _tmp15D;_push_region(r);
{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r,p->loc,te,({struct Cyc_Absyn_Tvar*_tmp160[1];_tmp160[0]=_tmp16E;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(r,_tag_dyneither(_tmp160,sizeof(struct Cyc_Absyn_Tvar*),1));}));
te2=Cyc_Tcenv_add_region(r,te2,(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp15E=_cycalloc(sizeof(*_tmp15E));_tmp15E[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp15F;_tmp15F.tag=2;_tmp15F.f1=_tmp16E;_tmp15F;});_tmp15E;}),0,1);
# 768
Cyc_Tcpat_check_alias_coercion(te2,p->loc,(void*)_check_null(_tmp161->topt),_tmp16E,_tmp16D->type,_tmp161);}
# 765
;_pop_region(r);}
# 772
break;}}}
# 775
goto _LL98;default: _LLA7: _LLA8:
 return;}_LL98:;}
# 791 "tcpat.cyc"
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_List_List*patvars){
# 793
Cyc_Tcpat_check_pat_regions_rec(te,p,0,patvars);{
struct _RegionHandle _tmp16F=_new_region("r");struct _RegionHandle*r=& _tmp16F;_push_region(r);
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple15*_tmp170=(struct _tuple15*)x->hd;struct _tuple15*_tmp171=_tmp170;struct Cyc_Absyn_Vardecl**_tmp17B;struct Cyc_Absyn_Exp*_tmp17A;_LLBF: _tmp17B=_tmp171->f1;_tmp17A=_tmp171->f2;_LLC0:;
if(_tmp17A != 0){
struct Cyc_Absyn_Exp*_tmp172=_tmp17A;
# 801
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)_check_null(_tmp172->topt)) && !
Cyc_Tcutil_is_noalias_path(r,_tmp172))
# 804
({struct Cyc_String_pa_PrintArg_struct _tmp175;_tmp175.tag=0;_tmp175.f1=(struct _dyneither_ptr)(
# 806
_tmp17B != 0?(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp178;_tmp178.tag=0;_tmp178.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 808
Cyc_Absynpp_qvar2string((*_tmp17B)->name));({void*_tmp176[1]={& _tmp178};Cyc_aprintf(({const char*_tmp177="for variable %s";_tag_dyneither(_tmp177,sizeof(char),16);}),_tag_dyneither(_tmp176,sizeof(void*),1));});}):({const char*_tmp179="";_tag_dyneither(_tmp179,sizeof(char),1);}));({void*_tmp173[1]={& _tmp175};Cyc_Tcutil_terr(p->loc,({const char*_tmp174="pattern %s dereferences a alias-free pointer from a non-unique path";_tag_dyneither(_tmp174,sizeof(char),68);}),_tag_dyneither(_tmp173,sizeof(void*),1));});});}}}
# 795
;_pop_region(r);};}
# 863 "tcpat.cyc"
struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct Cyc_Tcpat_EqNull_val={1};
struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct Cyc_Tcpat_NeqNull_val={2};
# 873
struct Cyc_Tcpat_Dummy_Tcpat_Access_struct Cyc_Tcpat_Dummy_val={0};
struct Cyc_Tcpat_Deref_Tcpat_Access_struct Cyc_Tcpat_Deref_val={1};union Cyc_Tcpat_PatOrWhere;struct Cyc_Tcpat_PathNode;struct _union_Name_value_Name_v{int tag;struct _dyneither_ptr val;};struct _union_Name_value_Int_v{int tag;int val;};union Cyc_Tcpat_Name_value{struct _union_Name_value_Name_v Name_v;struct _union_Name_value_Int_v Int_v;};
# 886
typedef union Cyc_Tcpat_Name_value Cyc_Tcpat_name_value_t;
union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct _dyneither_ptr s){return({union Cyc_Tcpat_Name_value _tmp180;(_tmp180.Name_v).val=s;(_tmp180.Name_v).tag=1;_tmp180;});}
union Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i){return({union Cyc_Tcpat_Name_value _tmp181;(_tmp181.Int_v).val=i;(_tmp181.Int_v).tag=2;_tmp181;});}struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value name;int arity;int*span;union Cyc_Tcpat_PatOrWhere orig_pat;};
# 896
typedef struct Cyc_Tcpat_Con_s*Cyc_Tcpat_con_t;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct{int tag;};struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 901
typedef void*Cyc_Tcpat_simple_pat_t;
# 904
static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){
union Cyc_Tcpat_Name_value _tmp182=c1->name;union Cyc_Tcpat_Name_value _tmp183=_tmp182;int _tmp18B;struct _dyneither_ptr _tmp18A;if((_tmp183.Name_v).tag == 1){_LLC2: _tmp18A=(_tmp183.Name_v).val;_LLC3: {
# 907
union Cyc_Tcpat_Name_value _tmp184=c2->name;union Cyc_Tcpat_Name_value _tmp185=_tmp184;struct _dyneither_ptr _tmp186;if((_tmp185.Name_v).tag == 1){_LLC7: _tmp186=(_tmp185.Name_v).val;_LLC8:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp18A,(struct _dyneither_ptr)_tmp186);}else{_LLC9: _LLCA:
 return - 1;}_LLC6:;}}else{_LLC4: _tmp18B=(_tmp183.Int_v).val;_LLC5: {
# 912
union Cyc_Tcpat_Name_value _tmp187=c2->name;union Cyc_Tcpat_Name_value _tmp188=_tmp187;int _tmp189;if((_tmp188.Name_v).tag == 1){_LLCC: _LLCD:
 return 1;}else{_LLCE: _tmp189=(_tmp188.Int_v).val;_LLCF:
 return _tmp18B - _tmp189;}_LLCB:;}}_LLC1:;}
# 920
static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(){
return((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(Cyc_Core_heap_region,Cyc_Tcpat_compare_con);}
# 924
static int Cyc_Tcpat_one_opt=1;
static int Cyc_Tcpat_two_opt=2;
static int Cyc_Tcpat_twofiftysix_opt=256;
# 928
static unsigned int Cyc_Tcpat_datatype_tag_number(struct Cyc_Absyn_Datatypedecl*td,struct _tuple2*name){
unsigned int ans=0;
struct Cyc_List_List*_tmp18C=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp18C))->hd)->name)!= 0){
++ ans;
_tmp18C=_tmp18C->tl;}
# 935
return ans;}
# 938
static int Cyc_Tcpat_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp18D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp18D != 0;_tmp18D=_tmp18D->tl){
struct Cyc_Absyn_Aggrfield*_tmp18E=(struct Cyc_Absyn_Aggrfield*)_tmp18D->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp18E->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}
# 945
({void*_tmp18F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp192;_tmp192.tag=0;_tmp192.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp190[1]={& _tmp192};Cyc_aprintf(({const char*_tmp191="get_member_offset %s failed";_tag_dyneither(_tmp191,sizeof(char),28);}),_tag_dyneither(_tmp190,sizeof(void*),1));});}),_tag_dyneither(_tmp18F,sizeof(void*),0));});}
# 948
static void*Cyc_Tcpat_get_pat_test(union Cyc_Tcpat_PatOrWhere pw){
union Cyc_Tcpat_PatOrWhere _tmp193=pw;struct Cyc_Absyn_Pat*_tmp1CC;struct Cyc_Absyn_Exp*_tmp1CB;if((_tmp193.where_clause).tag == 2){_LLD1: _tmp1CB=(_tmp193.where_clause).val;_LLD2:
 return(void*)({struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*_tmp194=_cycalloc(sizeof(*_tmp194));_tmp194[0]=({struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct _tmp195;_tmp195.tag=0;_tmp195.f1=_tmp1CB;_tmp195;});_tmp194;});}else{_LLD3: _tmp1CC=(_tmp193.pattern).val;_LLD4: {
# 952
void*_tmp196=_tmp1CC->r;void*_tmp197=_tmp196;union Cyc_Absyn_AggrInfoU _tmp1CA;struct Cyc_List_List*_tmp1C9;struct Cyc_Absyn_Datatypedecl*_tmp1C8;struct Cyc_Absyn_Datatypefield*_tmp1C7;void*_tmp1C6;struct Cyc_Absyn_Enumfield*_tmp1C5;struct Cyc_Absyn_Enumdecl*_tmp1C4;struct Cyc_Absyn_Enumfield*_tmp1C3;struct _dyneither_ptr _tmp1C2;int _tmp1C1;char _tmp1C0;enum Cyc_Absyn_Sign _tmp1BF;int _tmp1BE;struct Cyc_Absyn_Pat*_tmp1BD;struct Cyc_Absyn_Pat*_tmp1BC;switch(*((int*)_tmp197)){case 1: _LLD6: _tmp1BC=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp197)->f2;_LLD7:
 _tmp1BD=_tmp1BC;goto _LLD9;case 3: _LLD8: _tmp1BD=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp197)->f2;_LLD9:
 return Cyc_Tcpat_get_pat_test(({union Cyc_Tcpat_PatOrWhere _tmp198;(_tmp198.pattern).val=_tmp1BD;(_tmp198.pattern).tag=1;_tmp198;}));case 9: _LLDA: _LLDB:
 return(void*)& Cyc_Tcpat_EqNull_val;case 10: _LLDC: _tmp1BF=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp197)->f1;_tmp1BE=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp197)->f2;_LLDD:
 return(void*)({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_tmp199=_cycalloc_atomic(sizeof(*_tmp199));_tmp199[0]=({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct _tmp19A;_tmp19A.tag=6;_tmp19A.f1=(unsigned int)_tmp1BE;_tmp19A;});_tmp199;});case 11: _LLDE: _tmp1C0=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp197)->f1;_LLDF:
 return(void*)({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_tmp19B=_cycalloc_atomic(sizeof(*_tmp19B));_tmp19B[0]=({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct _tmp19C;_tmp19C.tag=6;_tmp19C.f1=(unsigned int)_tmp1C0;_tmp19C;});_tmp19B;});case 12: _LLE0: _tmp1C2=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp197)->f1;_tmp1C1=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp197)->f2;_LLE1:
 return(void*)({struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*_tmp19D=_cycalloc(sizeof(*_tmp19D));_tmp19D[0]=({struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct _tmp19E;_tmp19E.tag=5;_tmp19E.f1=_tmp1C2;_tmp19E.f2=_tmp1C1;_tmp19E;});_tmp19D;});case 13: _LLE2: _tmp1C4=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp197)->f1;_tmp1C3=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp197)->f2;_LLE3:
 return(void*)({struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*_tmp19F=_cycalloc(sizeof(*_tmp19F));_tmp19F[0]=({struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct _tmp1A0;_tmp1A0.tag=3;_tmp1A0.f1=_tmp1C4;_tmp1A0.f2=_tmp1C3;_tmp1A0;});_tmp19F;});case 14: _LLE4: _tmp1C6=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp197)->f1;_tmp1C5=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp197)->f2;_LLE5:
 return(void*)({struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*_tmp1A1=_cycalloc(sizeof(*_tmp1A1));_tmp1A1[0]=({struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct _tmp1A2;_tmp1A2.tag=4;_tmp1A2.f1=_tmp1C6;_tmp1A2.f2=_tmp1C5;_tmp1A2;});_tmp1A1;});case 6: _LLE6: _LLE7:
# 962
{void*_tmp1A3=Cyc_Tcutil_compress((void*)_check_null(_tmp1CC->topt));void*_tmp1A4=_tmp1A3;union Cyc_Absyn_Constraint*_tmp1A5;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A4)->tag == 5){_LLEF: _tmp1A5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A4)->f1).ptr_atts).nullable;_LLF0:
# 964
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp1A5))
return(void*)& Cyc_Tcpat_NeqNull_val;
goto _LLEE;}else{_LLF1: _LLF2:
 goto _LLEE;}_LLEE:;}
# 969
({void*_tmp1A6=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1A7="non-null pointer type or non-pointer type in pointer pattern";_tag_dyneither(_tmp1A7,sizeof(char),61);}),_tag_dyneither(_tmp1A6,sizeof(void*),0));});case 8: _LLE8: _tmp1C8=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp197)->f1;_tmp1C7=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp197)->f2;_LLE9:
# 971
 if(_tmp1C8->is_extensible)
return(void*)({struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*_tmp1A8=_cycalloc(sizeof(*_tmp1A8));_tmp1A8[0]=({struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct _tmp1A9;_tmp1A9.tag=9;_tmp1A9.f1=_tmp1C8;_tmp1A9.f2=_tmp1C7;_tmp1A9;});_tmp1A8;});else{
# 974
return(void*)({struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*_tmp1AA=_cycalloc(sizeof(*_tmp1AA));_tmp1AA[0]=({struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct _tmp1AB;_tmp1AB.tag=7;_tmp1AB.f1=(int)Cyc_Tcpat_datatype_tag_number(_tmp1C8,_tmp1C7->name);_tmp1AB.f2=_tmp1C8;_tmp1AB.f3=_tmp1C7;_tmp1AB;});_tmp1AA;});}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp197)->f1 != 0){_LLEA: _tmp1CA=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp197)->f1)->aggr_info;_tmp1C9=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp197)->f3;_LLEB: {
# 976
struct Cyc_Absyn_Aggrdecl*_tmp1AC=Cyc_Absyn_get_known_aggrdecl(_tmp1CA);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1AC->impl))->tagged){
struct _tuple18*_tmp1AD=(struct _tuple18*)((struct Cyc_List_List*)_check_null(_tmp1C9))->hd;struct _tuple18*_tmp1AE=_tmp1AD;struct Cyc_List_List*_tmp1B7;struct Cyc_Absyn_Pat*_tmp1B6;_LLF4: _tmp1B7=_tmp1AE->f1;_tmp1B6=_tmp1AE->f2;_LLF5:;{
struct _dyneither_ptr*f;
{void*_tmp1AF=(void*)((struct Cyc_List_List*)_check_null(_tmp1B7))->hd;void*_tmp1B0=_tmp1AF;struct _dyneither_ptr*_tmp1B3;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp1B0)->tag == 1){_LLF7: _tmp1B3=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp1B0)->f1;_LLF8:
 f=_tmp1B3;goto _LLF6;}else{_LLF9: _LLFA:
({void*_tmp1B1=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1B2="no field name in tagged union pattern";_tag_dyneither(_tmp1B2,sizeof(char),38);}),_tag_dyneither(_tmp1B1,sizeof(void*),0));});}_LLF6:;}
# 984
return(void*)({struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*_tmp1B4=_cycalloc(sizeof(*_tmp1B4));_tmp1B4[0]=({struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct _tmp1B5;_tmp1B5.tag=8;_tmp1B5.f1=f;_tmp1B5.f2=Cyc_Tcpat_get_member_offset(_tmp1AC,f);_tmp1B5;});_tmp1B4;});};}else{
# 986
({void*_tmp1B8=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1B9="non-tagged aggregate in pattern test";_tag_dyneither(_tmp1B9,sizeof(char),37);}),_tag_dyneither(_tmp1B8,sizeof(void*),0));});}}}else{goto _LLEC;}default: _LLEC: _LLED:
({void*_tmp1BA=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1BB="non-test pattern in pattern test";_tag_dyneither(_tmp1BB,sizeof(char),33);}),_tag_dyneither(_tmp1BA,sizeof(void*),0));});}_LLD5:;}}_LLD0:;}
# 992
static union Cyc_Tcpat_PatOrWhere Cyc_Tcpat_pw(struct Cyc_Absyn_Pat*p){
return({union Cyc_Tcpat_PatOrWhere _tmp1CD;(_tmp1CD.pattern).val=p;(_tmp1CD.pattern).tag=1;_tmp1CD;});}
# 996
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp1CE=_cycalloc(sizeof(*_tmp1CE));_tmp1CE->name=Cyc_Tcpat_Name_v(({const char*_tmp1CF="NULL";_tag_dyneither(_tmp1CF,sizeof(char),5);}));_tmp1CE->arity=0;_tmp1CE->span=& Cyc_Tcpat_two_opt;_tmp1CE->orig_pat=Cyc_Tcpat_pw(p);_tmp1CE;});}
# 999
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp1D0=_cycalloc(sizeof(*_tmp1D0));_tmp1D0->name=Cyc_Tcpat_Name_v(({const char*_tmp1D1="&";_tag_dyneither(_tmp1D1,sizeof(char),2);}));_tmp1D0->arity=1;_tmp1D0->span=& Cyc_Tcpat_two_opt;_tmp1D0->orig_pat=Cyc_Tcpat_pw(p);_tmp1D0;});}
# 1002
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));_tmp1D2->name=Cyc_Tcpat_Name_v(({const char*_tmp1D3="&";_tag_dyneither(_tmp1D3,sizeof(char),2);}));_tmp1D2->arity=1;_tmp1D2->span=& Cyc_Tcpat_one_opt;_tmp1D2->orig_pat=Cyc_Tcpat_pw(p);_tmp1D2;});}
# 1005
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(int i,union Cyc_Tcpat_PatOrWhere p){
return({struct Cyc_Tcpat_Con_s*_tmp1D4=_cycalloc(sizeof(*_tmp1D4));_tmp1D4->name=Cyc_Tcpat_Int_v(i);_tmp1D4->arity=0;_tmp1D4->span=0;_tmp1D4->orig_pat=p;_tmp1D4;});}
# 1008
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp1D5=_cycalloc(sizeof(*_tmp1D5));_tmp1D5->name=Cyc_Tcpat_Name_v(f);_tmp1D5->arity=0;_tmp1D5->span=0;_tmp1D5->orig_pat=Cyc_Tcpat_pw(p);_tmp1D5;});}
# 1011
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(char c,struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp1D6=_cycalloc(sizeof(*_tmp1D6));_tmp1D6->name=Cyc_Tcpat_Int_v((int)c);_tmp1D6->arity=0;_tmp1D6->span=& Cyc_Tcpat_twofiftysix_opt;_tmp1D6->orig_pat=Cyc_Tcpat_pw(p);_tmp1D6;});}
# 1014
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(int i,union Cyc_Tcpat_PatOrWhere p){
return({struct Cyc_Tcpat_Con_s*_tmp1D7=_cycalloc(sizeof(*_tmp1D7));_tmp1D7->name=Cyc_Tcpat_Name_v(({const char*_tmp1D8="$";_tag_dyneither(_tmp1D8,sizeof(char),2);}));_tmp1D7->arity=i;_tmp1D7->span=& Cyc_Tcpat_one_opt;_tmp1D7->orig_pat=p;_tmp1D7;});}
# 1019
static void*Cyc_Tcpat_null_pat(struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1D9=_cycalloc(sizeof(*_tmp1D9));_tmp1D9[0]=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp1DA;_tmp1DA.tag=1;_tmp1DA.f1=Cyc_Tcpat_null_con(p);_tmp1DA.f2=0;_tmp1DA;});_tmp1D9;});}
# 1022
static void*Cyc_Tcpat_int_pat(int i,union Cyc_Tcpat_PatOrWhere p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1DB=_cycalloc(sizeof(*_tmp1DB));_tmp1DB[0]=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp1DC;_tmp1DC.tag=1;_tmp1DC.f1=Cyc_Tcpat_int_con(i,p);_tmp1DC.f2=0;_tmp1DC;});_tmp1DB;});}
# 1025
static void*Cyc_Tcpat_char_pat(char c,struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1DD=_cycalloc(sizeof(*_tmp1DD));_tmp1DD[0]=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp1DE;_tmp1DE.tag=1;_tmp1DE.f1=Cyc_Tcpat_char_con(c,p);_tmp1DE.f2=0;_tmp1DE;});_tmp1DD;});}
# 1028
static void*Cyc_Tcpat_float_pat(struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1DF=_cycalloc(sizeof(*_tmp1DF));_tmp1DF[0]=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp1E0;_tmp1E0.tag=1;_tmp1E0.f1=Cyc_Tcpat_float_con(f,p);_tmp1E0.f2=0;_tmp1E0;});_tmp1DF;});}
# 1031
static void*Cyc_Tcpat_null_ptr_pat(void*p,struct Cyc_Absyn_Pat*p0){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1E1=_cycalloc(sizeof(*_tmp1E1));_tmp1E1[0]=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp1E2;_tmp1E2.tag=1;_tmp1E2.f1=Cyc_Tcpat_null_ptr_con(p0);_tmp1E2.f2=({struct Cyc_List_List*_tmp1E3=_cycalloc(sizeof(*_tmp1E3));_tmp1E3->hd=p;_tmp1E3->tl=0;_tmp1E3;});_tmp1E2;});_tmp1E1;});}
# 1034
static void*Cyc_Tcpat_ptr_pat(void*p,struct Cyc_Absyn_Pat*p0){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1E4=_cycalloc(sizeof(*_tmp1E4));_tmp1E4[0]=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp1E5;_tmp1E5.tag=1;_tmp1E5.f1=Cyc_Tcpat_ptr_con(p0);_tmp1E5.f2=({struct Cyc_List_List*_tmp1E6=_cycalloc(sizeof(*_tmp1E6));_tmp1E6->hd=p;_tmp1E6->tl=0;_tmp1E6;});_tmp1E5;});_tmp1E4;});}
# 1037
static void*Cyc_Tcpat_tuple_pat(struct Cyc_List_List*ss,union Cyc_Tcpat_PatOrWhere p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1E7=_cycalloc(sizeof(*_tmp1E7));_tmp1E7[0]=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp1E8;_tmp1E8.tag=1;_tmp1E8.f1=Cyc_Tcpat_tuple_con(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ss),p);_tmp1E8.f2=ss;_tmp1E8;});_tmp1E7;});}
# 1040
static void*Cyc_Tcpat_con_pat(struct _dyneither_ptr con_name,int*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*p){
# 1042
struct Cyc_Tcpat_Con_s*c=({struct Cyc_Tcpat_Con_s*_tmp1EB=_cycalloc(sizeof(*_tmp1EB));_tmp1EB->name=Cyc_Tcpat_Name_v(con_name);_tmp1EB->arity=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ps);_tmp1EB->span=span;_tmp1EB->orig_pat=Cyc_Tcpat_pw(p);_tmp1EB;});
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1E9=_cycalloc(sizeof(*_tmp1E9));_tmp1E9[0]=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp1EA;_tmp1EA.tag=1;_tmp1EA.f1=c;_tmp1EA.f2=ps;_tmp1EA;});_tmp1E9;});}
# 1047
static void*Cyc_Tcpat_compile_pat(struct Cyc_Absyn_Pat*p){
void*s;
{void*_tmp1EC=p->r;void*_tmp1ED=_tmp1EC;void*_tmp230;struct Cyc_Absyn_Enumfield*_tmp22F;struct Cyc_Absyn_Enumdecl*_tmp22E;struct Cyc_Absyn_Enumfield*_tmp22D;struct Cyc_Absyn_Aggrdecl*_tmp22C;struct Cyc_List_List*_tmp22B;struct Cyc_List_List*_tmp22A;struct Cyc_Absyn_Datatypedecl*_tmp229;struct Cyc_Absyn_Datatypefield*_tmp228;struct Cyc_List_List*_tmp227;struct Cyc_Absyn_Pat*_tmp226;struct Cyc_Absyn_Pat*_tmp225;struct Cyc_Absyn_Pat*_tmp224;struct _dyneither_ptr _tmp223;char _tmp222;enum Cyc_Absyn_Sign _tmp221;int _tmp220;switch(*((int*)_tmp1ED)){case 0: _LLFC: _LLFD:
 goto _LLFF;case 2: _LLFE: _LLFF:
 goto _LL101;case 4: _LL100: _LL101:
 s=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp1EE=_cycalloc_atomic(sizeof(*_tmp1EE));_tmp1EE[0]=({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp1EF;_tmp1EF.tag=0;_tmp1EF;});_tmp1EE;});goto _LLFB;case 9: _LL102: _LL103:
 s=Cyc_Tcpat_null_pat(p);goto _LLFB;case 10: _LL104: _tmp221=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1ED)->f1;_tmp220=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1ED)->f2;_LL105:
 s=Cyc_Tcpat_int_pat(_tmp220,Cyc_Tcpat_pw(p));goto _LLFB;case 11: _LL106: _tmp222=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1ED)->f1;_LL107:
 s=Cyc_Tcpat_char_pat(_tmp222,p);goto _LLFB;case 12: _LL108: _tmp223=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1ED)->f1;_LL109:
 s=Cyc_Tcpat_float_pat(_tmp223,p);goto _LLFB;case 1: _LL10A: _tmp224=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1ED)->f2;_LL10B:
 s=Cyc_Tcpat_compile_pat(_tmp224);goto _LLFB;case 3: _LL10C: _tmp225=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1ED)->f2;_LL10D:
 s=Cyc_Tcpat_compile_pat(_tmp225);goto _LLFB;case 6: _LL10E: _tmp226=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1ED)->f1;_LL10F:
# 1060
{void*_tmp1F0=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp1F1=_tmp1F0;union Cyc_Absyn_Constraint*_tmp1F8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F1)->tag == 5){_LL123: _tmp1F8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F1)->f1).ptr_atts).nullable;_LL124: {
# 1062
int is_nullable=0;
int still_working=1;
while(still_working){
union Cyc_Absyn_Constraint*_tmp1F2=_tmp1F8;int _tmp1F5;union Cyc_Absyn_Constraint*_tmp1F4;switch((((union Cyc_Absyn_Constraint*)_tmp1F2)->No_constr).tag){case 2: _LL128: _tmp1F4=(_tmp1F2->Forward_constr).val;_LL129:
# 1067
*_tmp1F8=*_tmp1F4;
continue;case 3: _LL12A: _LL12B:
# 1070
({struct _union_Constraint_Eq_constr*_tmp1F3=& _tmp1F8->Eq_constr;_tmp1F3->tag=1;_tmp1F3->val=0;});
is_nullable=0;
still_working=0;
goto _LL127;default: _LL12C: _tmp1F5=(int)(_tmp1F2->Eq_constr).val;_LL12D:
# 1075
 is_nullable=_tmp1F5;
still_working=0;
goto _LL127;}_LL127:;}{
# 1080
void*ss=Cyc_Tcpat_compile_pat(_tmp226);
if(is_nullable)
s=Cyc_Tcpat_null_ptr_pat(ss,p);else{
# 1084
s=Cyc_Tcpat_ptr_pat(ss,p);}
goto _LL122;};}}else{_LL125: _LL126:
({void*_tmp1F6=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1F7="expecting pointertype for pattern!";_tag_dyneither(_tmp1F7,sizeof(char),35);}),_tag_dyneither(_tmp1F6,sizeof(void*),0));});}_LL122:;}
# 1088
goto _LLFB;case 8: _LL110: _tmp229=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1ED)->f1;_tmp228=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1ED)->f2;_tmp227=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1ED)->f3;_LL111: {
# 1090
int*span;
{void*_tmp1F9=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp1FA=_tmp1F9;switch(*((int*)_tmp1FA)){case 3: _LL12F: _LL130:
# 1093
 if(_tmp229->is_extensible)
span=0;else{
# 1096
span=({int*_tmp1FB=_cycalloc_atomic(sizeof(*_tmp1FB));_tmp1FB[0]=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp229->fields))->v);_tmp1FB;});}
goto _LL12E;case 4: _LL131: _LL132:
 span=& Cyc_Tcpat_one_opt;goto _LL12E;default: _LL133: _LL134:
 span=({void*_tmp1FC=0;((int*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1FD="void datatype pattern has bad type";_tag_dyneither(_tmp1FD,sizeof(char),35);}),_tag_dyneither(_tmp1FC,sizeof(void*),0));});goto _LL12E;}_LL12E:;}
# 1101
s=Cyc_Tcpat_con_pat(*(*_tmp228->name).f2,span,((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp227),p);
goto _LLFB;}case 5: _LL112: _tmp22A=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1ED)->f1;_LL113:
# 1105
 s=Cyc_Tcpat_tuple_pat(((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp22A),Cyc_Tcpat_pw(p));goto _LLFB;case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1ED)->f1 != 0){if(((((struct Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1ED)->f1)->aggr_info).KnownAggr).tag == 2){_LL114: _tmp22C=*(((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1ED)->f1)->aggr_info).KnownAggr).val;_tmp22B=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1ED)->f3;_LL115:
# 1110
 if(_tmp22C->kind == Cyc_Absyn_StructA){
struct Cyc_List_List*ps=0;
{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp22C->impl))->fields;for(0;fields != 0;fields=fields->tl){
# 1114
int found=Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,({const char*_tmp209="";_tag_dyneither(_tmp209,sizeof(char),1);}))== 0;
{struct Cyc_List_List*dlps0=_tmp22B;for(0;!found  && dlps0 != 0;dlps0=dlps0->tl){
struct _tuple18*_tmp1FE=(struct _tuple18*)dlps0->hd;struct _tuple18*_tmp1FF=_tmp1FE;struct Cyc_List_List*_tmp206;struct Cyc_Absyn_Pat*_tmp205;_LL136: _tmp206=_tmp1FF->f1;_tmp205=_tmp1FF->f2;_LL137:;{
struct Cyc_List_List*_tmp200=_tmp206;struct _dyneither_ptr*_tmp204;if(_tmp200 != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)_tmp200)->hd)->tag == 1){if(((struct Cyc_List_List*)_tmp200)->tl == 0){_LL139: _tmp204=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp200->hd)->f1;_LL13A:
# 1119
 if(Cyc_strptrcmp(_tmp204,((struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){
ps=({struct Cyc_List_List*_tmp201=_cycalloc(sizeof(*_tmp201));_tmp201->hd=Cyc_Tcpat_compile_pat(_tmp205);_tmp201->tl=ps;_tmp201;});
found=1;}
# 1123
goto _LL138;}else{goto _LL13B;}}else{goto _LL13B;}}else{_LL13B: _LL13C:
({void*_tmp202=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp203="bad designator(s)";_tag_dyneither(_tmp203,sizeof(char),18);}),_tag_dyneither(_tmp202,sizeof(void*),0));});}_LL138:;};}}
# 1127
if(!found)
({void*_tmp207=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp208="bad designator";_tag_dyneither(_tmp208,sizeof(char),15);}),_tag_dyneither(_tmp207,sizeof(void*),0));});}}
# 1130
s=Cyc_Tcpat_tuple_pat(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ps),Cyc_Tcpat_pw(p));}else{
# 1133
if(!((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp22C->impl))->tagged)
({void*_tmp20A=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp20B="patterns on untagged unions not yet supported.";_tag_dyneither(_tmp20B,sizeof(char),47);}),_tag_dyneither(_tmp20A,sizeof(void*),0));});{
int*span=({int*_tmp212=_cycalloc_atomic(sizeof(*_tmp212));_tmp212[0]=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp22C->impl))->fields);_tmp212;});
struct Cyc_List_List*_tmp20C=_tmp22B;struct _dyneither_ptr*_tmp211;struct Cyc_Absyn_Pat*_tmp210;if(_tmp20C != 0){if(((struct _tuple18*)((struct Cyc_List_List*)_tmp20C)->hd)->f1 != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)((struct _tuple18*)((struct Cyc_List_List*)_tmp20C)->hd)->f1)->hd)->tag == 1){if(((struct Cyc_List_List*)((struct _tuple18*)((struct Cyc_List_List*)_tmp20C)->hd)->f1)->tl == 0){if(((struct Cyc_List_List*)_tmp20C)->tl == 0){_LL13E: _tmp211=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)(((struct _tuple18*)_tmp20C->hd)->f1)->hd)->f1;_tmp210=((struct _tuple18*)_tmp20C->hd)->f2;_LL13F:
# 1138
 s=Cyc_Tcpat_con_pat(*_tmp211,span,({struct Cyc_List_List*_tmp20D=_cycalloc(sizeof(*_tmp20D));_tmp20D->hd=Cyc_Tcpat_compile_pat(_tmp210);_tmp20D->tl=0;_tmp20D;}),p);
goto _LL13D;}else{goto _LL140;}}else{goto _LL140;}}else{goto _LL140;}}else{goto _LL140;}}else{_LL140: _LL141:
({void*_tmp20E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp20F="bad union pattern";_tag_dyneither(_tmp20F,sizeof(char),18);}),_tag_dyneither(_tmp20E,sizeof(void*),0));});}_LL13D:;};}
# 1143
goto _LLFB;}else{goto _LL11E;}}else{_LL11E: _LL11F:
# 1185
 goto _LL121;}case 13: _LL116: _tmp22E=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1ED)->f1;_tmp22D=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1ED)->f2;_LL117:
# 1148
{void*_tmp213=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp214=_tmp213;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp214)->tag == 6){_LL143: _LL144:
# 1152
 s=Cyc_Tcpat_con_pat(*(*_tmp22D->name).f2,0,0,p);
goto _LL142;}else{_LL145: _LL146: {
# 1155
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp22E->fields))->v);
s=Cyc_Tcpat_con_pat(*(*_tmp22D->name).f2,({int*_tmp215=_cycalloc_atomic(sizeof(*_tmp215));_tmp215[0]=span;_tmp215;}),0,p);
goto _LL142;}}_LL142:;}
# 1159
goto _LLFB;case 14: _LL118: _tmp230=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1ED)->f1;_tmp22F=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1ED)->f2;_LL119: {
# 1164
struct Cyc_List_List*fields;
{void*_tmp216=Cyc_Tcutil_compress(_tmp230);void*_tmp217=_tmp216;struct Cyc_List_List*_tmp21A;if(((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp217)->tag == 14){_LL148: _tmp21A=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp217)->f1;_LL149:
 fields=_tmp21A;goto _LL147;}else{_LL14A: _LL14B:
({void*_tmp218=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp219="bad type in AnonEnum_p";_tag_dyneither(_tmp219,sizeof(char),23);}),_tag_dyneither(_tmp218,sizeof(void*),0));});}_LL147:;}
# 1171
{void*_tmp21B=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp21C=_tmp21B;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp21C)->tag == 6){_LL14D: _LL14E:
# 1175
 s=Cyc_Tcpat_con_pat(*(*_tmp22F->name).f2,0,0,p);
goto _LL14C;}else{_LL14F: _LL150: {
# 1178
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);
s=Cyc_Tcpat_con_pat(*(*_tmp22F->name).f2,({int*_tmp21D=_cycalloc_atomic(sizeof(*_tmp21D));_tmp21D[0]=span;_tmp21D;}),0,p);
goto _LL14C;}}_LL14C:;}
# 1182
goto _LLFB;}case 15: _LL11A: _LL11B:
 goto _LL11D;case 16: _LL11C: _LL11D:
 goto _LL11F;default: _LL120: _LL121:
# 1186
 s=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp21E=_cycalloc_atomic(sizeof(*_tmp21E));_tmp21E[0]=({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp21F;_tmp21F.tag=0;_tmp21F;});_tmp21E;});}_LLFB:;}
# 1188
return s;}
# 1196
typedef struct Cyc_List_List*Cyc_Tcpat_match_t;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct{int tag;struct Cyc_Set_Set*f1;};
# 1210
typedef struct Cyc_List_List*Cyc_Tcpat_context_t;
# 1217
static int Cyc_Tcpat_same_access(void*a1,void*a2){
struct _tuple0 _tmp231=({struct _tuple0 _tmp23D;_tmp23D.f1=a1;_tmp23D.f2=a2;_tmp23D;});struct _tuple0 _tmp232=_tmp231;int _tmp23C;struct _dyneither_ptr*_tmp23B;int _tmp23A;struct _dyneither_ptr*_tmp239;struct Cyc_Absyn_Datatypefield*_tmp238;unsigned int _tmp237;struct Cyc_Absyn_Datatypefield*_tmp236;unsigned int _tmp235;unsigned int _tmp234;unsigned int _tmp233;switch(*((int*)_tmp232.f1)){case 0: if(((struct Cyc_Tcpat_Dummy_Tcpat_Access_struct*)_tmp232.f2)->tag == 0){_LL152: _LL153:
 return 1;}else{goto _LL15C;}case 1: if(((struct Cyc_Tcpat_Deref_Tcpat_Access_struct*)_tmp232.f2)->tag == 1){_LL154: _LL155:
 return 1;}else{goto _LL15C;}case 2: if(((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp232.f2)->tag == 2){_LL156: _tmp234=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp232.f1)->f1;_tmp233=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp232.f2)->f1;_LL157:
 return _tmp234 == _tmp233;}else{goto _LL15C;}case 3: if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp232.f2)->tag == 3){_LL158: _tmp238=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp232.f1)->f2;_tmp237=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp232.f1)->f3;_tmp236=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp232.f2)->f2;_tmp235=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp232.f2)->f3;_LL159:
# 1223
 return _tmp238 == _tmp236  && _tmp237 == _tmp235;}else{goto _LL15C;}default: if(((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp232.f2)->tag == 4){_LL15A: _tmp23C=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp232.f1)->f1;_tmp23B=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp232.f1)->f2;_tmp23A=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp232.f2)->f1;_tmp239=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp232.f2)->f2;_LL15B:
# 1225
 return _tmp23C == _tmp23A  && Cyc_strptrcmp(_tmp23B,_tmp239)== 0;}else{_LL15C: _LL15D:
 return 0;}}_LL151:;}
# 1230
static int Cyc_Tcpat_same_path(struct Cyc_List_List*p1,struct Cyc_List_List*p2){
while(p1 != 0  && p2 != 0){
if(!Cyc_Tcpat_same_access(((struct Cyc_Tcpat_PathNode*)p1->hd)->access,((struct Cyc_Tcpat_PathNode*)p2->hd)->access))return 0;
p1=p1->tl;
p2=p2->tl;}
# 1236
if(p1 != p2)return 0;
return 1;}
# 1240
static void*Cyc_Tcpat_ifeq(struct Cyc_List_List*access,struct Cyc_Tcpat_Con_s*con,void*d1,void*d2){
void*_tmp23E=Cyc_Tcpat_get_pat_test(con->orig_pat);
{void*_tmp23F=d2;struct Cyc_List_List*_tmp246;struct Cyc_List_List*_tmp245;void*_tmp244;if(((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp23F)->tag == 2){_LL15F: _tmp246=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp23F)->f1;_tmp245=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp23F)->f2;_tmp244=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp23F)->f3;_LL160:
# 1244
 if(Cyc_Tcpat_same_path(access,_tmp246) && (int)(((con->orig_pat).pattern).tag == 1))
return(void*)({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_tmp240=_cycalloc(sizeof(*_tmp240));_tmp240[0]=({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct _tmp241;_tmp241.tag=2;_tmp241.f1=_tmp246;_tmp241.f2=({struct Cyc_List_List*_tmp242=_cycalloc(sizeof(*_tmp242));_tmp242->hd=({struct _tuple0*_tmp243=_cycalloc(sizeof(*_tmp243));_tmp243->f1=_tmp23E;_tmp243->f2=d1;_tmp243;});_tmp242->tl=_tmp245;_tmp242;});_tmp241.f3=_tmp244;_tmp241;});_tmp240;});else{
# 1248
goto _LL15E;}}else{_LL161: _LL162:
 goto _LL15E;}_LL15E:;}
# 1251
return(void*)({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_tmp247=_cycalloc(sizeof(*_tmp247));_tmp247[0]=({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct _tmp248;_tmp248.tag=2;_tmp248.f1=access;_tmp248.f2=({struct Cyc_List_List*_tmp249=_cycalloc(sizeof(*_tmp249));_tmp249->hd=({struct _tuple0*_tmp24A=_cycalloc(sizeof(*_tmp24A));_tmp24A->f1=_tmp23E;_tmp24A->f2=d1;_tmp24A;});_tmp249->tl=0;_tmp249;});_tmp248.f3=d2;_tmp248;});_tmp247;});}struct _tuple20{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 1257
typedef struct _tuple20*Cyc_Tcpat_work_stack_frame_t;
# 1260
typedef struct Cyc_List_List*Cyc_Tcpat_work_stack_t;
# 1262
enum Cyc_Tcpat_Answer{Cyc_Tcpat_Yes  = 0,Cyc_Tcpat_No  = 1,Cyc_Tcpat_Maybe  = 2};
typedef enum Cyc_Tcpat_Answer Cyc_Tcpat_answer_t;
# 1265
static void Cyc_Tcpat_print_tab(int i){
for(0;i != 0;-- i){
({void*_tmp24B=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp24C=" ";_tag_dyneither(_tmp24C,sizeof(char),2);}),_tag_dyneither(_tmp24B,sizeof(void*),0));});}}
# 1271
static void Cyc_Tcpat_print_con(struct Cyc_Tcpat_Con_s*c){
union Cyc_Tcpat_Name_value _tmp24D=c->name;
union Cyc_Tcpat_Name_value _tmp24E=_tmp24D;int _tmp256;struct _dyneither_ptr _tmp255;if((_tmp24E.Name_v).tag == 1){_LL164: _tmp255=(_tmp24E.Name_v).val;_LL165:
({struct Cyc_String_pa_PrintArg_struct _tmp251;_tmp251.tag=0;_tmp251.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp255);({void*_tmp24F[1]={& _tmp251};Cyc_fprintf(Cyc_stderr,({const char*_tmp250="%s";_tag_dyneither(_tmp250,sizeof(char),3);}),_tag_dyneither(_tmp24F,sizeof(void*),1));});});goto _LL163;}else{_LL166: _tmp256=(_tmp24E.Int_v).val;_LL167:
({struct Cyc_Int_pa_PrintArg_struct _tmp254;_tmp254.tag=1;_tmp254.f1=(unsigned long)_tmp256;({void*_tmp252[1]={& _tmp254};Cyc_fprintf(Cyc_stderr,({const char*_tmp253="%d";_tag_dyneither(_tmp253,sizeof(char),3);}),_tag_dyneither(_tmp252,sizeof(void*),1));});});goto _LL163;}_LL163:;}
# 1279
static void Cyc_Tcpat_print_access(void*a){
void*_tmp257=a;int _tmp26D;struct _dyneither_ptr*_tmp26C;struct Cyc_Absyn_Datatypefield*_tmp26B;unsigned int _tmp26A;unsigned int _tmp269;switch(*((int*)_tmp257)){case 0: _LL169: _LL16A:
({void*_tmp258=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp259="DUMMY";_tag_dyneither(_tmp259,sizeof(char),6);}),_tag_dyneither(_tmp258,sizeof(void*),0));});goto _LL168;case 1: _LL16B: _LL16C:
({void*_tmp25A=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp25B="*";_tag_dyneither(_tmp25B,sizeof(char),2);}),_tag_dyneither(_tmp25A,sizeof(void*),0));});goto _LL168;case 2: _LL16D: _tmp269=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp257)->f1;_LL16E:
({struct Cyc_Int_pa_PrintArg_struct _tmp25E;_tmp25E.tag=1;_tmp25E.f1=(unsigned long)((int)_tmp269);({void*_tmp25C[1]={& _tmp25E};Cyc_fprintf(Cyc_stderr,({const char*_tmp25D="[%d]";_tag_dyneither(_tmp25D,sizeof(char),5);}),_tag_dyneither(_tmp25C,sizeof(void*),1));});});goto _LL168;case 3: _LL16F: _tmp26B=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp257)->f2;_tmp26A=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp257)->f3;_LL170:
# 1285
({struct Cyc_Int_pa_PrintArg_struct _tmp262;_tmp262.tag=1;_tmp262.f1=(unsigned long)((int)_tmp26A);({struct Cyc_String_pa_PrintArg_struct _tmp261;_tmp261.tag=0;_tmp261.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp26B->name));({void*_tmp25F[2]={& _tmp261,& _tmp262};Cyc_fprintf(Cyc_stderr,({const char*_tmp260="%s[%d]";_tag_dyneither(_tmp260,sizeof(char),7);}),_tag_dyneither(_tmp25F,sizeof(void*),2));});});});goto _LL168;default: _LL171: _tmp26D=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp257)->f1;_tmp26C=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp257)->f2;_LL172:
# 1287
 if(_tmp26D)
({struct Cyc_String_pa_PrintArg_struct _tmp265;_tmp265.tag=0;_tmp265.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp26C);({void*_tmp263[1]={& _tmp265};Cyc_fprintf(Cyc_stderr,({const char*_tmp264=".tagunion.%s";_tag_dyneither(_tmp264,sizeof(char),13);}),_tag_dyneither(_tmp263,sizeof(void*),1));});});else{
# 1290
({struct Cyc_String_pa_PrintArg_struct _tmp268;_tmp268.tag=0;_tmp268.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp26C);({void*_tmp266[1]={& _tmp268};Cyc_fprintf(Cyc_stderr,({const char*_tmp267=".%s";_tag_dyneither(_tmp267,sizeof(char),4);}),_tag_dyneither(_tmp266,sizeof(void*),1));});});}
goto _LL168;}_LL168:;}
# 1295
static void Cyc_Tcpat_print_pat_test(void*p){
void*_tmp26E=p;struct Cyc_Absyn_Datatypefield*_tmp293;struct _dyneither_ptr*_tmp292;int _tmp291;int _tmp290;unsigned int _tmp28F;struct _dyneither_ptr _tmp28E;struct Cyc_Absyn_Enumfield*_tmp28D;struct Cyc_Absyn_Enumfield*_tmp28C;struct Cyc_Absyn_Exp*_tmp28B;switch(*((int*)_tmp26E)){case 0: if(((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp26E)->f1 == 0){_LL174: _LL175:
({void*_tmp26F=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp270="where(NULL)";_tag_dyneither(_tmp270,sizeof(char),12);}),_tag_dyneither(_tmp26F,sizeof(void*),0));});goto _LL173;}else{_LL176: _tmp28B=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp26E)->f1;_LL177:
({struct Cyc_String_pa_PrintArg_struct _tmp273;_tmp273.tag=0;_tmp273.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string((struct Cyc_Absyn_Exp*)_check_null(_tmp28B)));({void*_tmp271[1]={& _tmp273};Cyc_fprintf(Cyc_stderr,({const char*_tmp272="where(%s)";_tag_dyneither(_tmp272,sizeof(char),10);}),_tag_dyneither(_tmp271,sizeof(void*),1));});});goto _LL173;}case 1: _LL178: _LL179:
({void*_tmp274=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp275="NULL";_tag_dyneither(_tmp275,sizeof(char),5);}),_tag_dyneither(_tmp274,sizeof(void*),0));});goto _LL173;case 2: _LL17A: _LL17B:
({void*_tmp276=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp277="NOT-NULL:";_tag_dyneither(_tmp277,sizeof(char),10);}),_tag_dyneither(_tmp276,sizeof(void*),0));});goto _LL173;case 4: _LL17C: _tmp28C=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp26E)->f2;_LL17D:
 _tmp28D=_tmp28C;goto _LL17F;case 3: _LL17E: _tmp28D=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp26E)->f2;_LL17F:
({struct Cyc_String_pa_PrintArg_struct _tmp27A;_tmp27A.tag=0;_tmp27A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp28D->name));({void*_tmp278[1]={& _tmp27A};Cyc_fprintf(Cyc_stderr,({const char*_tmp279="%s";_tag_dyneither(_tmp279,sizeof(char),3);}),_tag_dyneither(_tmp278,sizeof(void*),1));});});goto _LL173;case 5: _LL180: _tmp28E=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp26E)->f1;_LL181:
({struct Cyc_String_pa_PrintArg_struct _tmp27D;_tmp27D.tag=0;_tmp27D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp28E);({void*_tmp27B[1]={& _tmp27D};Cyc_fprintf(Cyc_stderr,({const char*_tmp27C="%s";_tag_dyneither(_tmp27C,sizeof(char),3);}),_tag_dyneither(_tmp27B,sizeof(void*),1));});});goto _LL173;case 6: _LL182: _tmp28F=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp26E)->f1;_LL183:
({struct Cyc_Int_pa_PrintArg_struct _tmp280;_tmp280.tag=1;_tmp280.f1=(unsigned long)((int)_tmp28F);({void*_tmp27E[1]={& _tmp280};Cyc_fprintf(Cyc_stderr,({const char*_tmp27F="%d";_tag_dyneither(_tmp27F,sizeof(char),3);}),_tag_dyneither(_tmp27E,sizeof(void*),1));});});goto _LL173;case 7: _LL184: _tmp290=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp26E)->f1;_LL185:
({struct Cyc_Int_pa_PrintArg_struct _tmp283;_tmp283.tag=1;_tmp283.f1=(unsigned long)_tmp290;({void*_tmp281[1]={& _tmp283};Cyc_fprintf(Cyc_stderr,({const char*_tmp282="datatypetag(%d)";_tag_dyneither(_tmp282,sizeof(char),16);}),_tag_dyneither(_tmp281,sizeof(void*),1));});});goto _LL173;case 8: _LL186: _tmp292=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp26E)->f1;_tmp291=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp26E)->f2;_LL187:
({struct Cyc_Int_pa_PrintArg_struct _tmp287;_tmp287.tag=1;_tmp287.f1=(unsigned long)_tmp291;({struct Cyc_String_pa_PrintArg_struct _tmp286;_tmp286.tag=0;_tmp286.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp292);({void*_tmp284[2]={& _tmp286,& _tmp287};Cyc_fprintf(Cyc_stderr,({const char*_tmp285="uniontag[%s](%d)";_tag_dyneither(_tmp285,sizeof(char),17);}),_tag_dyneither(_tmp284,sizeof(void*),2));});});});goto _LL173;default: _LL188: _tmp293=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp26E)->f2;_LL189:
# 1308
({struct Cyc_String_pa_PrintArg_struct _tmp28A;_tmp28A.tag=0;_tmp28A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp293->name));({void*_tmp288[1]={& _tmp28A};Cyc_fprintf(Cyc_stderr,({const char*_tmp289="datatypefield(%s)";_tag_dyneither(_tmp289,sizeof(char),18);}),_tag_dyneither(_tmp288,sizeof(void*),1));});});}_LL173:;}
# 1312
static void Cyc_Tcpat_print_rhs(struct Cyc_Tcpat_Rhs*r){
({struct Cyc_String_pa_PrintArg_struct _tmp296;_tmp296.tag=0;_tmp296.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(r->rhs));({void*_tmp294[1]={& _tmp296};Cyc_fprintf(Cyc_stderr,({const char*_tmp295="%s";_tag_dyneither(_tmp295,sizeof(char),3);}),_tag_dyneither(_tmp294,sizeof(void*),1));});});}
# 1316
static void Cyc_Tcpat_print_dec_tree(void*d,int tab){
void*_tmp297=d;struct Cyc_List_List*_tmp2B3;struct Cyc_List_List*_tmp2B2;void*_tmp2B1;struct Cyc_Tcpat_Rhs*_tmp2B0;switch(*((int*)_tmp297)){case 1: _LL18B: _tmp2B0=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp297)->f1;_LL18C:
# 1319
 Cyc_Tcpat_print_tab(tab);
({void*_tmp298=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp299="Success(";_tag_dyneither(_tmp299,sizeof(char),9);}),_tag_dyneither(_tmp298,sizeof(void*),0));});Cyc_Tcpat_print_rhs(_tmp2B0);({void*_tmp29A=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp29B=")\n";_tag_dyneither(_tmp29B,sizeof(char),3);}),_tag_dyneither(_tmp29A,sizeof(void*),0));});
goto _LL18A;case 0: _LL18D: _LL18E:
({void*_tmp29C=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp29D="Failure\n";_tag_dyneither(_tmp29D,sizeof(char),9);}),_tag_dyneither(_tmp29C,sizeof(void*),0));});goto _LL18A;default: _LL18F: _tmp2B3=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp297)->f1;_tmp2B2=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp297)->f2;_tmp2B1=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp297)->f3;_LL190:
# 1324
 Cyc_Tcpat_print_tab(tab);
({void*_tmp29E=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp29F="Switch[";_tag_dyneither(_tmp29F,sizeof(char),8);}),_tag_dyneither(_tmp29E,sizeof(void*),0));});
for(0;_tmp2B3 != 0;_tmp2B3=_tmp2B3->tl){
Cyc_Tcpat_print_access(((struct Cyc_Tcpat_PathNode*)_tmp2B3->hd)->access);
if(_tmp2B3->tl != 0)({void*_tmp2A0=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2A1=",";_tag_dyneither(_tmp2A1,sizeof(char),2);}),_tag_dyneither(_tmp2A0,sizeof(void*),0));});}
# 1330
({void*_tmp2A2=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2A3="] {\n";_tag_dyneither(_tmp2A3,sizeof(char),5);}),_tag_dyneither(_tmp2A2,sizeof(void*),0));});
for(0;_tmp2B2 != 0;_tmp2B2=_tmp2B2->tl){
struct _tuple0 _tmp2A4=*((struct _tuple0*)_tmp2B2->hd);struct _tuple0 _tmp2A5=_tmp2A4;void*_tmp2AB;void*_tmp2AA;_LL192: _tmp2AB=_tmp2A5.f1;_tmp2AA=_tmp2A5.f2;_LL193:;
Cyc_Tcpat_print_tab(tab);
({void*_tmp2A6=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2A7="case ";_tag_dyneither(_tmp2A7,sizeof(char),6);}),_tag_dyneither(_tmp2A6,sizeof(void*),0));});
Cyc_Tcpat_print_pat_test(_tmp2AB);
({void*_tmp2A8=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2A9=":\n";_tag_dyneither(_tmp2A9,sizeof(char),3);}),_tag_dyneither(_tmp2A8,sizeof(void*),0));});
Cyc_Tcpat_print_dec_tree(_tmp2AA,tab + 7);}
# 1339
Cyc_Tcpat_print_tab(tab);
({void*_tmp2AC=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2AD="default:\n";_tag_dyneither(_tmp2AD,sizeof(char),10);}),_tag_dyneither(_tmp2AC,sizeof(void*),0));});
Cyc_Tcpat_print_dec_tree(_tmp2B1,tab + 7);
Cyc_Tcpat_print_tab(tab);
({void*_tmp2AE=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2AF="}\n";_tag_dyneither(_tmp2AF,sizeof(char),3);}),_tag_dyneither(_tmp2AE,sizeof(void*),0));});}_LL18A:;}
# 1347
void Cyc_Tcpat_print_decision_tree(void*d){
Cyc_Tcpat_print_dec_tree(d,0);}
# 1355
static void*Cyc_Tcpat_add_neg(void*td,struct Cyc_Tcpat_Con_s*c){
void*_tmp2B4=td;struct Cyc_Set_Set*_tmp2B9;if(((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2B4)->tag == 1){_LL195: _tmp2B9=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2B4)->f1;_LL196:
# 1365
 return(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2B5=_cycalloc(sizeof(*_tmp2B5));_tmp2B5[0]=({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp2B6;_tmp2B6.tag=1;_tmp2B6.f1=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_insert)(_tmp2B9,c);_tmp2B6;});_tmp2B5;});}else{_LL197: _LL198:
({void*_tmp2B7=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2B8="add_neg called when td is Positive";_tag_dyneither(_tmp2B8,sizeof(char),35);}),_tag_dyneither(_tmp2B7,sizeof(void*),0));});}_LL194:;}
# 1372
static enum Cyc_Tcpat_Answer Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*td){
void*_tmp2BA=td;struct Cyc_Set_Set*_tmp2BC;struct Cyc_Tcpat_Con_s*_tmp2BB;if(((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2BA)->tag == 0){_LL19A: _tmp2BB=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2BA)->f1;_LL19B:
# 1376
 if(Cyc_Tcpat_compare_con(c,_tmp2BB)== 0)return Cyc_Tcpat_Yes;else{
return Cyc_Tcpat_No;}}else{_LL19C: _tmp2BC=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2BA)->f1;_LL19D:
# 1380
 if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2BC,c))return Cyc_Tcpat_No;else{
# 1383
if(c->span != 0  && *((int*)_check_null(c->span))== ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp2BC)+ 1)
return Cyc_Tcpat_Yes;else{
# 1386
return Cyc_Tcpat_Maybe;}}}_LL199:;}struct _tuple21{struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 1394
static struct Cyc_List_List*Cyc_Tcpat_augment(struct Cyc_List_List*ctxt,void*dsc){
struct Cyc_List_List*_tmp2BD=ctxt;struct Cyc_Tcpat_Con_s*_tmp2C3;struct Cyc_List_List*_tmp2C2;struct Cyc_List_List*_tmp2C1;if(_tmp2BD == 0){_LL19F: _LL1A0:
 return 0;}else{_LL1A1: _tmp2C3=((struct _tuple21*)_tmp2BD->hd)->f1;_tmp2C2=((struct _tuple21*)_tmp2BD->hd)->f2;_tmp2C1=_tmp2BD->tl;_LL1A2:
# 1398
 return({struct Cyc_List_List*_tmp2BE=_cycalloc(sizeof(*_tmp2BE));_tmp2BE->hd=({struct _tuple21*_tmp2BF=_cycalloc(sizeof(*_tmp2BF));_tmp2BF->f1=_tmp2C3;_tmp2BF->f2=({struct Cyc_List_List*_tmp2C0=_cycalloc(sizeof(*_tmp2C0));_tmp2C0->hd=dsc;_tmp2C0->tl=_tmp2C2;_tmp2C0;});_tmp2BF;});_tmp2BE->tl=_tmp2C1;_tmp2BE;});}_LL19E:;}
# 1406
static struct Cyc_List_List*Cyc_Tcpat_norm_context(struct Cyc_List_List*ctxt){
struct Cyc_List_List*_tmp2C4=ctxt;struct Cyc_Tcpat_Con_s*_tmp2CB;struct Cyc_List_List*_tmp2CA;struct Cyc_List_List*_tmp2C9;if(_tmp2C4 == 0){_LL1A4: _LL1A5:
({void*_tmp2C5=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2C6="norm_context: empty context";_tag_dyneither(_tmp2C6,sizeof(char),28);}),_tag_dyneither(_tmp2C5,sizeof(void*),0));});}else{_LL1A6: _tmp2CB=((struct _tuple21*)_tmp2C4->hd)->f1;_tmp2CA=((struct _tuple21*)_tmp2C4->hd)->f2;_tmp2C9=_tmp2C4->tl;_LL1A7:
# 1410
 return Cyc_Tcpat_augment(_tmp2C9,(void*)({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2C7=_cycalloc(sizeof(*_tmp2C7));_tmp2C7[0]=({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp2C8;_tmp2C8.tag=0;_tmp2C8.f1=_tmp2CB;_tmp2C8.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp2CA);_tmp2C8;});_tmp2C7;}));}_LL1A3:;}
# 1419
static void*Cyc_Tcpat_build_desc(struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
# 1421
struct _tuple1 _tmp2CC=({struct _tuple1 _tmp2D9;_tmp2D9.f1=ctxt;_tmp2D9.f2=work;_tmp2D9;});struct _tuple1 _tmp2CD=_tmp2CC;struct Cyc_Tcpat_Con_s*_tmp2D8;struct Cyc_List_List*_tmp2D7;struct Cyc_List_List*_tmp2D6;struct Cyc_List_List*_tmp2D5;struct Cyc_List_List*_tmp2D4;if(_tmp2CD.f1 == 0){if(_tmp2CD.f2 == 0){_LL1A9: _LL1AA:
 return dsc;}else{_LL1AB: _LL1AC:
 goto _LL1AE;}}else{if(_tmp2CD.f2 == 0){_LL1AD: _LL1AE:
({void*_tmp2CE=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2CF="build_desc: ctxt and work don't match";_tag_dyneither(_tmp2CF,sizeof(char),38);}),_tag_dyneither(_tmp2CE,sizeof(void*),0));});}else{_LL1AF: _tmp2D8=((struct _tuple21*)(_tmp2CD.f1)->hd)->f1;_tmp2D7=((struct _tuple21*)(_tmp2CD.f1)->hd)->f2;_tmp2D6=(_tmp2CD.f1)->tl;_tmp2D5=((struct _tuple20*)(_tmp2CD.f2)->hd)->f3;_tmp2D4=(_tmp2CD.f2)->tl;_LL1B0: {
# 1426
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2D0=({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2D1=_cycalloc(sizeof(*_tmp2D1));_tmp2D1[0]=({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp2D2;_tmp2D2.tag=0;_tmp2D2.f1=_tmp2D8;_tmp2D2.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp2D7),({struct Cyc_List_List*_tmp2D3=_cycalloc(sizeof(*_tmp2D3));_tmp2D3->hd=dsc;_tmp2D3->tl=_tmp2D5;_tmp2D3;}));_tmp2D2;});_tmp2D1;});
return Cyc_Tcpat_build_desc(_tmp2D6,(void*)_tmp2D0,_tmp2D4);}}}_LL1A8:;}
# 1431
static void*Cyc_Tcpat_match(void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules);struct _tuple22{void*f1;struct Cyc_Tcpat_Rhs*f2;};
# 1438
static void*Cyc_Tcpat_or_match(void*dsc,struct Cyc_List_List*allmrules){
struct Cyc_List_List*_tmp2DA=allmrules;void*_tmp2DF;struct Cyc_Tcpat_Rhs*_tmp2DE;struct Cyc_List_List*_tmp2DD;if(_tmp2DA == 0){_LL1B2: _LL1B3:
 return(void*)({struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp2DB=_cycalloc(sizeof(*_tmp2DB));_tmp2DB[0]=({struct Cyc_Tcpat_Failure_Tcpat_Decision_struct _tmp2DC;_tmp2DC.tag=0;_tmp2DC.f1=dsc;_tmp2DC;});_tmp2DB;});}else{_LL1B4: _tmp2DF=((struct _tuple22*)_tmp2DA->hd)->f1;_tmp2DE=((struct _tuple22*)_tmp2DA->hd)->f2;_tmp2DD=_tmp2DA->tl;_LL1B5:
# 1442
 return Cyc_Tcpat_match(_tmp2DF,0,dsc,0,0,_tmp2DE,_tmp2DD);}_LL1B1:;}
# 1447
static void*Cyc_Tcpat_match_compile(struct Cyc_List_List*allmrules){
return Cyc_Tcpat_or_match((void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2E0=_cycalloc(sizeof(*_tmp2E0));_tmp2E0[0]=({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp2E1;_tmp2E1.tag=1;_tmp2E1.f1=Cyc_Tcpat_empty_con_set();_tmp2E1;});_tmp2E0;}),allmrules);}
# 1454
static void*Cyc_Tcpat_and_match(struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules){
# 1457
struct Cyc_List_List*_tmp2E2=work;struct Cyc_List_List*_tmp2FA;struct Cyc_List_List*_tmp2F9;struct Cyc_List_List*_tmp2F8;struct Cyc_List_List*_tmp2F7;struct Cyc_List_List*_tmp2F6;if(_tmp2E2 == 0){_LL1B7: _LL1B8:
 return(void*)({struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp2E3=_cycalloc(sizeof(*_tmp2E3));_tmp2E3[0]=({struct Cyc_Tcpat_Success_Tcpat_Decision_struct _tmp2E4;_tmp2E4.tag=1;_tmp2E4.f1=right_hand_side;_tmp2E4;});_tmp2E3;});}else{if(((struct _tuple20*)((struct Cyc_List_List*)_tmp2E2)->hd)->f1 == 0){if(((struct _tuple20*)((struct Cyc_List_List*)_tmp2E2)->hd)->f2 == 0){if(((struct _tuple20*)((struct Cyc_List_List*)_tmp2E2)->hd)->f3 == 0){_LL1B9: _tmp2F6=_tmp2E2->tl;_LL1BA:
# 1460
 return Cyc_Tcpat_and_match(Cyc_Tcpat_norm_context(ctx),_tmp2F6,right_hand_side,rules);}else{goto _LL1BB;}}else{goto _LL1BB;}}else{_LL1BB: _tmp2FA=((struct _tuple20*)_tmp2E2->hd)->f1;_tmp2F9=((struct _tuple20*)_tmp2E2->hd)->f2;_tmp2F8=((struct _tuple20*)_tmp2E2->hd)->f3;_tmp2F7=_tmp2E2->tl;_LL1BC:
# 1462
 if((_tmp2FA == 0  || _tmp2F9 == 0) || _tmp2F8 == 0)
({void*_tmp2E5=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2E6="tcpat:and_match: malformed work frame";_tag_dyneither(_tmp2E6,sizeof(char),38);}),_tag_dyneither(_tmp2E5,sizeof(void*),0));});{
struct Cyc_List_List*_tmp2E7=_tmp2FA;struct Cyc_List_List*_tmp2E8=_tmp2E7;void*_tmp2F5;struct Cyc_List_List*_tmp2F4;_LL1BE: _tmp2F5=(void*)_tmp2E8->hd;_tmp2F4=_tmp2E8->tl;_LL1BF:;{
struct Cyc_List_List*_tmp2E9=_tmp2F9;struct Cyc_List_List*_tmp2EA=_tmp2E9;struct Cyc_List_List*_tmp2F3;struct Cyc_List_List*_tmp2F2;_LL1C1: _tmp2F3=(struct Cyc_List_List*)_tmp2EA->hd;_tmp2F2=_tmp2EA->tl;_LL1C2:;{
struct Cyc_List_List*_tmp2EB=_tmp2F8;struct Cyc_List_List*_tmp2EC=_tmp2EB;void*_tmp2F1;struct Cyc_List_List*_tmp2F0;_LL1C4: _tmp2F1=(void*)_tmp2EC->hd;_tmp2F0=_tmp2EC->tl;_LL1C5:;{
struct _tuple20*_tmp2ED=({struct _tuple20*_tmp2EF=_cycalloc(sizeof(*_tmp2EF));_tmp2EF->f1=_tmp2F4;_tmp2EF->f2=_tmp2F2;_tmp2EF->f3=_tmp2F0;_tmp2EF;});
return Cyc_Tcpat_match(_tmp2F5,_tmp2F3,_tmp2F1,ctx,({struct Cyc_List_List*_tmp2EE=_cycalloc(sizeof(*_tmp2EE));_tmp2EE->hd=_tmp2ED;_tmp2EE->tl=_tmp2F7;_tmp2EE;}),right_hand_side,rules);};};};};}}_LL1B6:;}
# 1473
static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct Cyc_Tcpat_Con_s*pcon,void*dsc){
void*_tmp2FB=dsc;struct Cyc_List_List*_tmp301;struct Cyc_Set_Set*_tmp300;if(((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2FB)->tag == 1){_LL1C7: _tmp300=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2FB)->f1;_LL1C8: {
# 1479
void*any=(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2FE=_cycalloc(sizeof(*_tmp2FE));_tmp2FE[0]=({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp2FF;_tmp2FF.tag=1;_tmp2FF.f1=Cyc_Tcpat_empty_con_set();_tmp2FF;});_tmp2FE;});
struct Cyc_List_List*_tmp2FC=0;
{int i=0;for(0;i < pcon->arity;++ i){
_tmp2FC=({struct Cyc_List_List*_tmp2FD=_cycalloc(sizeof(*_tmp2FD));_tmp2FD->hd=any;_tmp2FD->tl=_tmp2FC;_tmp2FD;});}}
return _tmp2FC;}}else{_LL1C9: _tmp301=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2FB)->f2;_LL1CA:
 return _tmp301;}_LL1C6:;}
# 1488
static void*Cyc_Tcpat_get_access(union Cyc_Tcpat_PatOrWhere pw,int i){
union Cyc_Tcpat_PatOrWhere _tmp302=pw;struct Cyc_Absyn_Pat*_tmp323;if((_tmp302.where_clause).tag == 2){_LL1CC: _LL1CD:
 return(void*)& Cyc_Tcpat_Dummy_val;}else{_LL1CE: _tmp323=(_tmp302.pattern).val;_LL1CF: {
# 1492
void*_tmp303=_tmp323->r;void*_tmp304=_tmp303;union Cyc_Absyn_AggrInfoU _tmp322;struct Cyc_List_List*_tmp321;struct Cyc_Absyn_Datatypedecl*_tmp320;struct Cyc_Absyn_Datatypefield*_tmp31F;switch(*((int*)_tmp304)){case 6: _LL1D1: _LL1D2:
# 1494
 if(i != 0)
({void*_tmp305=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp308;_tmp308.tag=1;_tmp308.f1=(unsigned long)i;({void*_tmp306[1]={& _tmp308};Cyc_aprintf(({const char*_tmp307="get_access on pointer pattern with offset %d\n";_tag_dyneither(_tmp307,sizeof(char),46);}),_tag_dyneither(_tmp306,sizeof(void*),1));});}),_tag_dyneither(_tmp305,sizeof(void*),0));});
return(void*)& Cyc_Tcpat_Deref_val;case 5: _LL1D3: _LL1D4:
 return(void*)({struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*_tmp309=_cycalloc_atomic(sizeof(*_tmp309));_tmp309[0]=({struct Cyc_Tcpat_TupleField_Tcpat_Access_struct _tmp30A;_tmp30A.tag=2;_tmp30A.f1=(unsigned int)i;_tmp30A;});_tmp309;});case 8: _LL1D5: _tmp320=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp304)->f1;_tmp31F=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp304)->f2;_LL1D6:
 return(void*)({struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*_tmp30B=_cycalloc(sizeof(*_tmp30B));_tmp30B[0]=({struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct _tmp30C;_tmp30C.tag=3;_tmp30C.f1=_tmp320;_tmp30C.f2=_tmp31F;_tmp30C.f3=(unsigned int)i;_tmp30C;});_tmp30B;});case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp304)->f1 != 0){_LL1D7: _tmp322=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp304)->f1)->aggr_info;_tmp321=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp304)->f3;_LL1D8: {
# 1500
struct Cyc_Absyn_Aggrdecl*_tmp30D=Cyc_Absyn_get_known_aggrdecl(_tmp322);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp30D->impl))->tagged){
struct Cyc_List_List*_tmp30E=(*((struct _tuple18*)((struct Cyc_List_List*)_check_null(_tmp321))->hd)).f1;struct Cyc_List_List*_tmp30F=_tmp30E;struct _dyneither_ptr*_tmp316;if(_tmp30F != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)_tmp30F)->hd)->tag == 1){if(((struct Cyc_List_List*)_tmp30F)->tl == 0){_LL1DC: _tmp316=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp30F->hd)->f1;_LL1DD:
# 1504
 return(void*)({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_tmp310=_cycalloc(sizeof(*_tmp310));_tmp310[0]=({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct _tmp311;_tmp311.tag=4;_tmp311.f1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp30D->impl))->tagged;_tmp311.f2=_tmp316;_tmp311;});_tmp310;});}else{goto _LL1DE;}}else{goto _LL1DE;}}else{_LL1DE: _LL1DF:
({void*_tmp312=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp315;_tmp315.tag=0;_tmp315.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_pat2string(_tmp323));({void*_tmp313[1]={& _tmp315};Cyc_aprintf(({const char*_tmp314="get_access on bad aggr pattern: %s";_tag_dyneither(_tmp314,sizeof(char),35);}),_tag_dyneither(_tmp313,sizeof(void*),1));});}),_tag_dyneither(_tmp312,sizeof(void*),0));});}_LL1DB:;}{
# 1508
struct Cyc_List_List*_tmp317=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp30D->impl))->fields;
int _tmp318=i;
for(0;i != 0;-- i){_tmp317=((struct Cyc_List_List*)_check_null(_tmp317))->tl;}
return(void*)({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_tmp319=_cycalloc(sizeof(*_tmp319));_tmp319[0]=({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct _tmp31A;_tmp31A.tag=4;_tmp31A.f1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp30D->impl))->tagged;_tmp31A.f2=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp317))->hd)->name;_tmp31A;});_tmp319;});};}}else{goto _LL1D9;}default: _LL1D9: _LL1DA:
({void*_tmp31B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp31E;_tmp31E.tag=0;_tmp31E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_pat2string(_tmp323));({void*_tmp31C[1]={& _tmp31E};Cyc_aprintf(({const char*_tmp31D="get_access on bad pattern: %s";_tag_dyneither(_tmp31D,sizeof(char),30);}),_tag_dyneither(_tmp31C,sizeof(void*),1));});}),_tag_dyneither(_tmp31B,sizeof(void*),0));});}_LL1D0:;}}_LL1CB:;}struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*f2;};
# 1518
static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple23*env,int i){
struct _tuple23*_tmp324=env;struct Cyc_List_List*_tmp328;struct Cyc_Tcpat_Con_s*_tmp327;_LL1E1: _tmp328=_tmp324->f1;_tmp327=_tmp324->f2;_LL1E2:;{
void*acc=Cyc_Tcpat_get_access(_tmp327->orig_pat,i);
return({struct Cyc_List_List*_tmp325=_cycalloc(sizeof(*_tmp325));_tmp325->hd=({struct Cyc_Tcpat_PathNode*_tmp326=_cycalloc(sizeof(*_tmp326));_tmp326->orig_pat=_tmp327->orig_pat;_tmp326->access=acc;_tmp326;});_tmp325->tl=_tmp328;_tmp325;});};}
# 1523
static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){
struct _tuple23 _tmp329=({struct _tuple23 _tmp32A;_tmp32A.f1=obj;_tmp32A.f2=pcon;_tmp32A;});
return((struct Cyc_List_List*(*)(int n,struct Cyc_List_List*(*f)(struct _tuple23*,int),struct _tuple23*env))Cyc_List_tabulate_c)(pcon->arity,Cyc_Tcpat_getoarg,& _tmp329);}
# 1531
static void*Cyc_Tcpat_match(void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules){
# 1535
void*_tmp32B=p;struct Cyc_Tcpat_Con_s*_tmp339;struct Cyc_List_List*_tmp338;if(((struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*)_tmp32B)->tag == 0){_LL1E4: _LL1E5:
# 1537
 return Cyc_Tcpat_and_match(Cyc_Tcpat_augment(ctx,dsc),work,right_hand_side,rules);}else{_LL1E6: _tmp339=((struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp32B)->f1;_tmp338=((struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp32B)->f2;_LL1E7: {
# 1539
enum Cyc_Tcpat_Answer _tmp32C=Cyc_Tcpat_static_match(_tmp339,dsc);enum Cyc_Tcpat_Answer _tmp32D=_tmp32C;switch(_tmp32D){case Cyc_Tcpat_Yes: _LL1E9: _LL1EA: {
# 1541
struct Cyc_List_List*ctx2=({struct Cyc_List_List*_tmp330=_cycalloc(sizeof(*_tmp330));_tmp330->hd=({struct _tuple21*_tmp331=_cycalloc(sizeof(*_tmp331));_tmp331->f1=_tmp339;_tmp331->f2=0;_tmp331;});_tmp330->tl=ctx;_tmp330;});
struct _tuple20*work_frame=({struct _tuple20*_tmp32F=_cycalloc(sizeof(*_tmp32F));_tmp32F->f1=_tmp338;_tmp32F->f2=Cyc_Tcpat_getoargs(_tmp339,obj);_tmp32F->f3=
Cyc_Tcpat_getdargs(_tmp339,dsc);_tmp32F;});
struct Cyc_List_List*work2=({struct Cyc_List_List*_tmp32E=_cycalloc(sizeof(*_tmp32E));_tmp32E->hd=work_frame;_tmp32E->tl=work;_tmp32E;});
return Cyc_Tcpat_and_match(ctx2,work2,right_hand_side,rules);}case Cyc_Tcpat_No: _LL1EB: _LL1EC:
# 1547
 return Cyc_Tcpat_or_match(Cyc_Tcpat_build_desc(ctx,dsc,work),rules);default: _LL1ED: _LL1EE: {
# 1549
struct Cyc_List_List*ctx2=({struct Cyc_List_List*_tmp336=_cycalloc(sizeof(*_tmp336));_tmp336->hd=({struct _tuple21*_tmp337=_cycalloc(sizeof(*_tmp337));_tmp337->f1=_tmp339;_tmp337->f2=0;_tmp337;});_tmp336->tl=ctx;_tmp336;});
struct _tuple20*work_frame=({struct _tuple20*_tmp335=_cycalloc(sizeof(*_tmp335));_tmp335->f1=_tmp338;_tmp335->f2=Cyc_Tcpat_getoargs(_tmp339,obj);_tmp335->f3=
Cyc_Tcpat_getdargs(_tmp339,dsc);_tmp335;});
struct Cyc_List_List*work2=({struct Cyc_List_List*_tmp334=_cycalloc(sizeof(*_tmp334));_tmp334->hd=work_frame;_tmp334->tl=work;_tmp334;});
void*_tmp332=Cyc_Tcpat_and_match(ctx2,work2,right_hand_side,rules);
void*_tmp333=Cyc_Tcpat_or_match(Cyc_Tcpat_build_desc(ctx,Cyc_Tcpat_add_neg(dsc,_tmp339),work),rules);
# 1556
return Cyc_Tcpat_ifeq(obj,_tmp339,_tmp332,_tmp333);}}_LL1E8:;}}_LL1E3:;}
# 1566
static void Cyc_Tcpat_check_exhaust_overlap(void*d,void(*not_exhaust)(void*,void*),void*env1,void(*rhs_appears)(void*,struct Cyc_Tcpat_Rhs*),void*env2,int*exhaust_done){
# 1571
void*_tmp33A=d;struct Cyc_List_List*_tmp341;void*_tmp340;struct Cyc_Tcpat_Rhs*_tmp33F;void*_tmp33E;switch(*((int*)_tmp33A)){case 0: _LL1F0: _tmp33E=(void*)((struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp33A)->f1;_LL1F1:
# 1573
 if(!(*exhaust_done)){not_exhaust(env1,_tmp33E);*exhaust_done=1;}
goto _LL1EF;case 1: _LL1F2: _tmp33F=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp33A)->f1;_LL1F3:
 rhs_appears(env2,_tmp33F);goto _LL1EF;default: _LL1F4: _tmp341=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp33A)->f2;_tmp340=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp33A)->f3;_LL1F5:
# 1577
 for(0;_tmp341 != 0;_tmp341=_tmp341->tl){
struct _tuple0 _tmp33B=*((struct _tuple0*)_tmp341->hd);struct _tuple0 _tmp33C=_tmp33B;void*_tmp33D;_LL1F7: _tmp33D=_tmp33C.f2;_LL1F8:;
Cyc_Tcpat_check_exhaust_overlap(_tmp33D,not_exhaust,env1,rhs_appears,env2,exhaust_done);}
# 1582
Cyc_Tcpat_check_exhaust_overlap(_tmp340,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1584
goto _LL1EF;}_LL1EF:;}
# 1592
static struct _tuple22*Cyc_Tcpat_get_match(int*ctr,struct Cyc_Absyn_Switch_clause*swc){
# 1594
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp34A=_cycalloc(sizeof(*_tmp34A));_tmp34A->used=0;_tmp34A->pat_loc=(swc->pattern)->loc;_tmp34A->rhs=swc->body;_tmp34A;});
void*sp;
if(swc->where_clause != 0){
union Cyc_Tcpat_PatOrWhere _tmp342=({union Cyc_Tcpat_PatOrWhere _tmp344;(_tmp344.where_clause).val=swc->where_clause;(_tmp344.where_clause).tag=2;_tmp344;});
sp=Cyc_Tcpat_tuple_pat(({void*_tmp343[2];_tmp343[1]=Cyc_Tcpat_int_pat(*ctr,_tmp342);_tmp343[0]=sp0;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp343,sizeof(void*),2));}),_tmp342);
*ctr=*ctr + 1;}else{
# 1602
sp=Cyc_Tcpat_tuple_pat(({void*_tmp345[2];_tmp345[1]=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp346=_cycalloc_atomic(sizeof(*_tmp346));_tmp346[0]=({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp347;_tmp347.tag=0;_tmp347;});_tmp346;});_tmp345[0]=sp0;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp345,sizeof(void*),2));}),({union Cyc_Tcpat_PatOrWhere _tmp348;(_tmp348.where_clause).val=0;(_tmp348.where_clause).tag=2;_tmp348;}));}
return({struct _tuple22*_tmp349=_cycalloc(sizeof(*_tmp349));_tmp349->f1=sp;_tmp349->f2=rhs;_tmp349;});}char Cyc_Tcpat_Desc2string[12]="Desc2string";struct Cyc_Tcpat_Desc2string_exn_struct{char*tag;};
# 1608
struct Cyc_Tcpat_Desc2string_exn_struct Cyc_Tcpat_Desc2string_val={Cyc_Tcpat_Desc2string};
# 1610
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*);
static struct _dyneither_ptr Cyc_Tcpat_desc2string(void*d){
void*_tmp34C=d;struct Cyc_Set_Set*_tmp3AD;struct Cyc_Tcpat_Con_s*_tmp3AC;struct Cyc_List_List*_tmp3AB;if(((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp34C)->tag == 0){_LL1FA: _tmp3AC=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp34C)->f1;_tmp3AB=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp34C)->f2;_LL1FB: {
# 1614
union Cyc_Tcpat_Name_value _tmp34D=_tmp3AC->name;
struct Cyc_Absyn_Pat*p;
{union Cyc_Tcpat_PatOrWhere _tmp34E=_tmp3AC->orig_pat;union Cyc_Tcpat_PatOrWhere _tmp34F=_tmp34E;struct Cyc_Absyn_Pat*_tmp350;if((_tmp34F.where_clause).tag == 2){_LL1FF: _LL200:
 return Cyc_Tcpat_desc2string((void*)((struct Cyc_List_List*)_check_null(_tmp3AB))->hd);}else{_LL201: _tmp350=(_tmp34F.pattern).val;_LL202:
 p=_tmp350;goto _LL1FE;}_LL1FE:;}{
# 1620
void*_tmp351=p->r;void*_tmp352=_tmp351;struct Cyc_Absyn_Exp*_tmp387;struct Cyc_Absyn_Enumfield*_tmp386;struct Cyc_Absyn_Enumfield*_tmp385;struct _dyneither_ptr _tmp384;int _tmp383;char _tmp382;int _tmp381;struct Cyc_Absyn_Datatypefield*_tmp380;struct Cyc_Absyn_Aggrdecl*_tmp37F;struct Cyc_List_List*_tmp37E;struct Cyc_Absyn_Tvar*_tmp37D;struct Cyc_Absyn_Vardecl*_tmp37C;struct Cyc_Absyn_Vardecl*_tmp37B;struct Cyc_Absyn_Vardecl*_tmp37A;switch(*((int*)_tmp352)){case 0: _LL204: _LL205:
 return({const char*_tmp353="_";_tag_dyneither(_tmp353,sizeof(char),2);});case 1: _LL206: _tmp37A=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp352)->f1;_LL207:
 return Cyc_Absynpp_qvar2string(_tmp37A->name);case 3: _LL208: _tmp37B=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp352)->f1;_LL209:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp356;_tmp356.tag=0;_tmp356.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp37B->name));({void*_tmp354[1]={& _tmp356};Cyc_aprintf(({const char*_tmp355="*%s";_tag_dyneither(_tmp355,sizeof(char),4);}),_tag_dyneither(_tmp354,sizeof(void*),1));});});case 4: _LL20A: _tmp37D=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp352)->f1;_tmp37C=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp352)->f2;_LL20B:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp35A;_tmp35A.tag=0;_tmp35A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp37D->name);({struct Cyc_String_pa_PrintArg_struct _tmp359;_tmp359.tag=0;_tmp359.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp37C->name));({void*_tmp357[2]={& _tmp359,& _tmp35A};Cyc_aprintf(({const char*_tmp358="%s<`%s>";_tag_dyneither(_tmp358,sizeof(char),8);}),_tag_dyneither(_tmp357,sizeof(void*),2));});});});case 5: _LL20C: _LL20D:
# 1626
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp35D;_tmp35D.tag=0;_tmp35D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(_tmp3AB));({void*_tmp35B[1]={& _tmp35D};Cyc_aprintf(({const char*_tmp35C="$(%s)";_tag_dyneither(_tmp35C,sizeof(char),6);}),_tag_dyneither(_tmp35B,sizeof(void*),1));});});case 6: _LL20E: _LL20F:
# 1628
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp360;_tmp360.tag=0;_tmp360.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(_tmp3AB));({void*_tmp35E[1]={& _tmp360};Cyc_aprintf(({const char*_tmp35F="&%s";_tag_dyneither(_tmp35F,sizeof(char),4);}),_tag_dyneither(_tmp35E,sizeof(void*),1));});});case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp352)->f1 != 0){if(((((struct Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp352)->f1)->aggr_info).KnownAggr).tag == 2){_LL210: _tmp37F=*(((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp352)->f1)->aggr_info).KnownAggr).val;_tmp37E=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp352)->f3;_LL211:
# 1630
 if(_tmp37F->kind == Cyc_Absyn_UnionA){
struct Cyc_List_List*_tmp361=_tmp37E;struct _dyneither_ptr*_tmp367;if(_tmp361 != 0){if(((struct _tuple18*)((struct Cyc_List_List*)_tmp361)->hd)->f1 != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)((struct _tuple18*)((struct Cyc_List_List*)_tmp361)->hd)->f1)->hd)->tag == 1){_LL225: _tmp367=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)(((struct _tuple18*)_tmp361->hd)->f1)->hd)->f1;_LL226:
# 1633
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp366;_tmp366.tag=0;_tmp366.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Tcpat_descs2string(_tmp3AB));({struct Cyc_String_pa_PrintArg_struct _tmp365;_tmp365.tag=0;_tmp365.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp367);({struct Cyc_String_pa_PrintArg_struct _tmp364;_tmp364.tag=0;_tmp364.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1633
Cyc_Absynpp_qvar2string(_tmp37F->name));({void*_tmp362[3]={& _tmp364,& _tmp365,& _tmp366};Cyc_aprintf(({const char*_tmp363="%s{.%s = %s}";_tag_dyneither(_tmp363,sizeof(char),13);}),_tag_dyneither(_tmp362,sizeof(void*),3));});});});});}else{goto _LL227;}}else{goto _LL227;}}else{_LL227: _LL228:
# 1635
 goto _LL224;}_LL224:;}
# 1638
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp36B;_tmp36B.tag=0;_tmp36B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(_tmp3AB));({struct Cyc_String_pa_PrintArg_struct _tmp36A;_tmp36A.tag=0;_tmp36A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp37F->name));({void*_tmp368[2]={& _tmp36A,& _tmp36B};Cyc_aprintf(({const char*_tmp369="%s{%s}";_tag_dyneither(_tmp369,sizeof(char),7);}),_tag_dyneither(_tmp368,sizeof(void*),2));});});});}else{goto _LL222;}}else{goto _LL222;}case 8: _LL212: _tmp380=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp352)->f2;_LL213:
# 1640
 if(_tmp3AB == 0)
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp36E;_tmp36E.tag=0;_tmp36E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp380->name));({void*_tmp36C[1]={& _tmp36E};Cyc_aprintf(({const char*_tmp36D="%s";_tag_dyneither(_tmp36D,sizeof(char),3);}),_tag_dyneither(_tmp36C,sizeof(void*),1));});});else{
# 1643
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp372;_tmp372.tag=0;_tmp372.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(_tmp3AB));({struct Cyc_String_pa_PrintArg_struct _tmp371;_tmp371.tag=0;_tmp371.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp380->name));({void*_tmp36F[2]={& _tmp371,& _tmp372};Cyc_aprintf(({const char*_tmp370="%s(%s)";_tag_dyneither(_tmp370,sizeof(char),7);}),_tag_dyneither(_tmp36F,sizeof(void*),2));});});});}case 9: _LL214: _LL215:
 return({const char*_tmp373="NULL";_tag_dyneither(_tmp373,sizeof(char),5);});case 10: _LL216: _tmp381=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp352)->f2;_LL217:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp376;_tmp376.tag=1;_tmp376.f1=(unsigned long)_tmp381;({void*_tmp374[1]={& _tmp376};Cyc_aprintf(({const char*_tmp375="%d";_tag_dyneither(_tmp375,sizeof(char),3);}),_tag_dyneither(_tmp374,sizeof(void*),1));});});case 11: _LL218: _tmp382=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp352)->f1;_LL219:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp379;_tmp379.tag=1;_tmp379.f1=(unsigned long)((int)_tmp382);({void*_tmp377[1]={& _tmp379};Cyc_aprintf(({const char*_tmp378="%d";_tag_dyneither(_tmp378,sizeof(char),3);}),_tag_dyneither(_tmp377,sizeof(void*),1));});});case 12: _LL21A: _tmp384=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp352)->f1;_tmp383=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp352)->f2;_LL21B:
 return _tmp384;case 13: _LL21C: _tmp385=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp352)->f2;_LL21D:
 _tmp386=_tmp385;goto _LL21F;case 14: _LL21E: _tmp386=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp352)->f2;_LL21F:
 return Cyc_Absynpp_qvar2string(_tmp386->name);case 17: _LL220: _tmp387=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp352)->f1;_LL221:
 return Cyc_Absynpp_exp2string(_tmp387);default: _LL222: _LL223:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LL203:;};}}else{_LL1FC: _tmp3AD=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp34C)->f1;_LL1FD:
# 1654
 if(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_is_empty)(_tmp3AD))return({const char*_tmp388="_";_tag_dyneither(_tmp388,sizeof(char),2);});{
struct Cyc_Tcpat_Con_s*_tmp389=((struct Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp3AD);
# 1658
if((int)(((_tmp389->orig_pat).where_clause).tag == 2))(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_Absyn_Pat*_tmp38A=({union Cyc_Tcpat_PatOrWhere _tmp3AA=_tmp389->orig_pat;if((_tmp3AA.pattern).tag != 1)_throw_match();(_tmp3AA.pattern).val;});
void*_tmp38B=Cyc_Tcutil_compress((void*)_check_null(_tmp38A->topt));void*_tmp38C=_tmp38B;struct Cyc_Absyn_Aggrdecl*_tmp3A9;struct Cyc_Absyn_Datatypedecl*_tmp3A8;struct Cyc_Absyn_PtrAtts _tmp3A7;switch(*((int*)_tmp38C)){case 6: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp38C)->f2 == Cyc_Absyn_Char_sz){_LL22A: _LL22B:
# 1663
{int i=0;for(0;i < 256;++ i){
struct Cyc_Tcpat_Con_s*_tmp38D=Cyc_Tcpat_char_con((char)i,_tmp38A);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp3AD,_tmp38D))
return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp390;_tmp390.tag=1;_tmp390.f1=(unsigned long)i;({void*_tmp38E[1]={& _tmp390};Cyc_aprintf(({const char*_tmp38F="%d";_tag_dyneither(_tmp38F,sizeof(char),3);}),_tag_dyneither(_tmp38E,sizeof(void*),1));});});}}
# 1668
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}else{_LL22C: _LL22D:
# 1671
{unsigned int i=0;for(0;i < -1;++ i){
struct Cyc_Tcpat_Con_s*_tmp391=Cyc_Tcpat_int_con((int)i,_tmp389->orig_pat);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp3AD,_tmp391))
return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp394;_tmp394.tag=1;_tmp394.f1=(unsigned long)((int)i);({void*_tmp392[1]={& _tmp394};Cyc_aprintf(({const char*_tmp393="%d";_tag_dyneither(_tmp393,sizeof(char),3);}),_tag_dyneither(_tmp392,sizeof(void*),1));});});}}
# 1676
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}case 5: _LL22E: _tmp3A7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38C)->f1).ptr_atts;_LL22F: {
# 1678
union Cyc_Absyn_Constraint*_tmp395=_tmp3A7.nullable;
int is_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp395);
if(is_nullable){
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp3AD,Cyc_Tcpat_null_con(_tmp38A)))
return({const char*_tmp396="NULL";_tag_dyneither(_tmp396,sizeof(char),5);});}
# 1684
return({const char*_tmp397="&_";_tag_dyneither(_tmp397,sizeof(char),3);});}case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp38C)->f1).datatype_info).KnownDatatype).tag == 2){_LL230: _tmp3A8=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp38C)->f1).datatype_info).KnownDatatype).val;_LL231:
# 1686
 if(_tmp3A8->is_extensible)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_List_List*_tmp398=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3A8->fields))->v;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp398);
for(0;(unsigned int)_tmp398;_tmp398=_tmp398->tl){
struct _dyneither_ptr n=*(*((struct Cyc_Absyn_Datatypefield*)_tmp398->hd)->name).f2;
struct Cyc_List_List*_tmp399=((struct Cyc_Absyn_Datatypefield*)_tmp398->hd)->typs;
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp3AD,({struct Cyc_Tcpat_Con_s*_tmp39A=_cycalloc(sizeof(*_tmp39A));_tmp39A->name=Cyc_Tcpat_Name_v(n);_tmp39A->arity=0;_tmp39A->span=0;_tmp39A->orig_pat=_tmp389->orig_pat;_tmp39A;}))){
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp399)== 0)
return n;else{
# 1696
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp39D;_tmp39D.tag=0;_tmp39D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n);({void*_tmp39B[1]={& _tmp39D};Cyc_aprintf(({const char*_tmp39C="%s(...)";_tag_dyneither(_tmp39C,sizeof(char),8);}),_tag_dyneither(_tmp39B,sizeof(void*),1));});});}}}
# 1699
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);};}else{goto _LL234;}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp38C)->f1).aggr_info).KnownAggr).tag == 2){_LL232: _tmp3A9=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp38C)->f1).aggr_info).KnownAggr).val;if(_tmp3A9->kind == Cyc_Absyn_UnionA){_LL233: {
# 1701
struct Cyc_List_List*_tmp39E=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3A9->impl))->fields;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp39E);
struct _tuple2*_tmp39F=_tmp3A9->name;struct _tuple2*_tmp3A0=_tmp39F;struct _dyneither_ptr _tmp3A6;_LL237: _tmp3A6=*_tmp3A0->f2;_LL238:;
for(0;(unsigned int)_tmp39E;_tmp39E=_tmp39E->tl){
struct _dyneither_ptr n=*((struct Cyc_Absyn_Aggrfield*)_tmp39E->hd)->name;
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp3AD,({struct Cyc_Tcpat_Con_s*_tmp3A1=_cycalloc(sizeof(*_tmp3A1));_tmp3A1->name=Cyc_Tcpat_Name_v(n);_tmp3A1->arity=0;_tmp3A1->span=0;_tmp3A1->orig_pat=_tmp389->orig_pat;_tmp3A1;})))
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3A5;_tmp3A5.tag=0;_tmp3A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n);({struct Cyc_String_pa_PrintArg_struct _tmp3A4;_tmp3A4.tag=0;_tmp3A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3A6);({void*_tmp3A2[2]={& _tmp3A4,& _tmp3A5};Cyc_aprintf(({const char*_tmp3A3="%s{.%s = _}";_tag_dyneither(_tmp3A3,sizeof(char),12);}),_tag_dyneither(_tmp3A2,sizeof(void*),2));});});});}
# 1709
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}}else{goto _LL234;}}else{goto _LL234;}default: _LL234: _LL235:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LL229:;};};}_LL1F9:;}
# 1714
static struct _dyneither_ptr*Cyc_Tcpat_desc2stringptr(void*d){
return({struct _dyneither_ptr*_tmp3AE=_cycalloc(sizeof(*_tmp3AE));_tmp3AE[0]=Cyc_Tcpat_desc2string(d);_tmp3AE;});}
# 1717
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*ds){
struct Cyc_List_List*_tmp3AF=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_desc2stringptr,ds);
struct _dyneither_ptr*comma=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),",",sizeof(char),2);
{struct Cyc_List_List*_tmp3B0=_tmp3AF;for(0;_tmp3B0 != 0;_tmp3B0=((struct Cyc_List_List*)_check_null(_tmp3B0))->tl){
if(_tmp3B0->tl != 0){
_tmp3B0->tl=({struct Cyc_List_List*_tmp3B1=_cycalloc(sizeof(*_tmp3B1));_tmp3B1->hd=comma;_tmp3B1->tl=_tmp3B0->tl;_tmp3B1;});
_tmp3B0=_tmp3B0->tl;}}}
# 1726
return(struct _dyneither_ptr)Cyc_strconcat_l(_tmp3AF);}
# 1729
static void Cyc_Tcpat_not_exhaust_err(unsigned int loc,void*desc){
struct _handler_cons _tmp3B3;_push_handler(& _tmp3B3);{int _tmp3B5=0;if(setjmp(_tmp3B3.handler))_tmp3B5=1;if(!_tmp3B5){
{struct _dyneither_ptr _tmp3B6=Cyc_Tcpat_desc2string(desc);
({struct Cyc_String_pa_PrintArg_struct _tmp3B9;_tmp3B9.tag=0;_tmp3B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3B6);({void*_tmp3B7[1]={& _tmp3B9};Cyc_Tcutil_terr(loc,({const char*_tmp3B8="patterns may not be exhaustive.\n\tmissing case for %s";_tag_dyneither(_tmp3B8,sizeof(char),53);}),_tag_dyneither(_tmp3B7,sizeof(void*),1));});});}
# 1731
;_pop_handler();}else{void*_tmp3B4=(void*)_exn_thrown;void*_tmp3BA=_tmp3B4;void*_tmp3BD;if(((struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp3BA)->tag == Cyc_Tcpat_Desc2string){_LL23A: _LL23B:
# 1735
({void*_tmp3BB=0;Cyc_Tcutil_terr(loc,({const char*_tmp3BC="patterns may not be exhaustive.";_tag_dyneither(_tmp3BC,sizeof(char),32);}),_tag_dyneither(_tmp3BB,sizeof(void*),0));});
goto _LL239;}else{_LL23C: _tmp3BD=_tmp3BA;_LL23D:(int)_rethrow(_tmp3BD);}_LL239:;}};}
# 1739
static void Cyc_Tcpat_rule_occurs(int dummy,struct Cyc_Tcpat_Rhs*rhs){
rhs->used=1;}
# 1743
void Cyc_Tcpat_check_switch_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs,void**dopt){
# 1749
int _tmp3BE=0;
int*_tmp3BF=& _tmp3BE;
struct Cyc_List_List*_tmp3C0=((struct Cyc_List_List*(*)(struct _tuple22*(*f)(int*,struct Cyc_Absyn_Switch_clause*),int*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcpat_get_match,_tmp3BF,swcs);
void*_tmp3C1=Cyc_Tcpat_match_compile(_tmp3C0);
*dopt=_tmp3C1;{
# 1755
int _tmp3C2=0;
((void(*)(void*d,void(*not_exhaust)(unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct Cyc_Tcpat_Rhs*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp3C1,Cyc_Tcpat_not_exhaust_err,loc,Cyc_Tcpat_rule_occurs,0,& _tmp3C2);
# 1758
for(0;_tmp3C0 != 0;_tmp3C0=_tmp3C0->tl){
struct _tuple22*_tmp3C3=(struct _tuple22*)_tmp3C0->hd;struct _tuple22*_tmp3C4=_tmp3C3;int _tmp3C8;unsigned int _tmp3C7;_LL23F: _tmp3C8=(_tmp3C4->f2)->used;_tmp3C7=(_tmp3C4->f2)->pat_loc;_LL240:;
if(!_tmp3C8){
({void*_tmp3C5=0;Cyc_Tcutil_terr(_tmp3C7,({const char*_tmp3C6="redundant pattern (check for misspelled constructors in earlier patterns)";_tag_dyneither(_tmp3C6,sizeof(char),74);}),_tag_dyneither(_tmp3C5,sizeof(void*),0));});
# 1763
break;}}};}
# 1772
static void Cyc_Tcpat_not_exhaust_warn(struct _tuple14*pr,void*desc){
(*pr).f2=0;{
struct _handler_cons _tmp3C9;_push_handler(& _tmp3C9);{int _tmp3CB=0;if(setjmp(_tmp3C9.handler))_tmp3CB=1;if(!_tmp3CB){
{struct _dyneither_ptr _tmp3CC=Cyc_Tcpat_desc2string(desc);
({struct Cyc_String_pa_PrintArg_struct _tmp3CF;_tmp3CF.tag=0;_tmp3CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3CC);({void*_tmp3CD[1]={& _tmp3CF};Cyc_Tcutil_warn((*pr).f1,({const char*_tmp3CE="pattern not exhaustive.\n\tmissing case for %s";_tag_dyneither(_tmp3CE,sizeof(char),45);}),_tag_dyneither(_tmp3CD,sizeof(void*),1));});});}
# 1775
;_pop_handler();}else{void*_tmp3CA=(void*)_exn_thrown;void*_tmp3D0=_tmp3CA;void*_tmp3D3;if(((struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp3D0)->tag == Cyc_Tcpat_Desc2string){_LL242: _LL243:
# 1779
({void*_tmp3D1=0;Cyc_Tcutil_warn((*pr).f1,({const char*_tmp3D2="pattern not exhaustive.";_tag_dyneither(_tmp3D2,sizeof(char),24);}),_tag_dyneither(_tmp3D1,sizeof(void*),0));});
goto _LL241;}else{_LL244: _tmp3D3=_tmp3D0;_LL245:(int)_rethrow(_tmp3D3);}_LL241:;}};};}
# 1783
static void Cyc_Tcpat_dummy_fn(int i,struct Cyc_Tcpat_Rhs*rhs){
return;}
# 1787
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**dopt){
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp3DB=_cycalloc(sizeof(*_tmp3DB));_tmp3DB->used=0;_tmp3DB->pat_loc=p->loc;_tmp3DB->rhs=Cyc_Absyn_skip_stmt(0);_tmp3DB;});
struct Cyc_List_List*_tmp3D4=({struct Cyc_List_List*_tmp3D9=_cycalloc(sizeof(*_tmp3D9));_tmp3D9->hd=({struct _tuple22*_tmp3DA=_cycalloc(sizeof(*_tmp3DA));_tmp3DA->f1=Cyc_Tcpat_compile_pat(p);_tmp3DA->f2=rhs;_tmp3DA;});_tmp3D9->tl=0;_tmp3D9;});
void*_tmp3D5=Cyc_Tcpat_match_compile(_tmp3D4);
struct _tuple14 _tmp3D6=({struct _tuple14 _tmp3D8;_tmp3D8.f1=loc;_tmp3D8.f2=1;_tmp3D8;});
int _tmp3D7=0;
((void(*)(void*d,void(*not_exhaust)(struct _tuple14*,void*),struct _tuple14*env1,void(*rhs_appears)(int,struct Cyc_Tcpat_Rhs*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp3D5,Cyc_Tcpat_not_exhaust_warn,& _tmp3D6,Cyc_Tcpat_dummy_fn,0,& _tmp3D7);
# 1795
*dopt=_tmp3D5;
return _tmp3D6.f2;}
# 1803
static struct _tuple22*Cyc_Tcpat_get_match2(struct Cyc_Absyn_Switch_clause*swc){
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp3DD=_cycalloc(sizeof(*_tmp3DD));_tmp3DD->used=0;_tmp3DD->pat_loc=(swc->pattern)->loc;_tmp3DD->rhs=swc->body;_tmp3DD;});
return({struct _tuple22*_tmp3DC=_cycalloc(sizeof(*_tmp3DC));_tmp3DC->f1=sp0;_tmp3DC->f2=rhs;_tmp3DC;});}
# 1808
static void Cyc_Tcpat_not_exhaust_err2(unsigned int loc,void*d){;}
# 1811
void Cyc_Tcpat_check_catch_overlap(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs,void**dopt){
# 1815
struct Cyc_List_List*_tmp3DE=((struct Cyc_List_List*(*)(struct _tuple22*(*f)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_get_match2,swcs);
void*_tmp3DF=Cyc_Tcpat_match_compile(_tmp3DE);
*dopt=_tmp3DF;{
int _tmp3E0=0;
((void(*)(void*d,void(*not_exhaust)(unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct Cyc_Tcpat_Rhs*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp3DF,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,0,& _tmp3E0);
# 1821
for(0;_tmp3DE != 0;_tmp3DE=_tmp3DE->tl){
# 1823
if(_tmp3DE->tl == 0)break;{
struct _tuple22*_tmp3E1=(struct _tuple22*)_tmp3DE->hd;struct _tuple22*_tmp3E2=_tmp3E1;int _tmp3E6;unsigned int _tmp3E5;_LL247: _tmp3E6=(_tmp3E2->f2)->used;_tmp3E5=(_tmp3E2->f2)->pat_loc;_LL248:;
if(!_tmp3E6){
({void*_tmp3E3=0;Cyc_Tcutil_terr(_tmp3E5,({const char*_tmp3E4="redundant pattern (check for misspelled constructors in earlier patterns)";_tag_dyneither(_tmp3E4,sizeof(char),74);}),_tag_dyneither(_tmp3E3,sizeof(void*),0));});
break;}};}};}
# 1832
int Cyc_Tcpat_has_vars(struct Cyc_Core_Opt*pat_vars){
{struct Cyc_List_List*_tmp3E7=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(pat_vars))->v;for(0;_tmp3E7 != 0;_tmp3E7=_tmp3E7->tl){
if((*((struct _tuple15*)_tmp3E7->hd)).f1 != 0)
return 1;}}
return 0;}
