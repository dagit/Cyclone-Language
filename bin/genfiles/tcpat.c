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
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 175
struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*,struct Cyc_List_List*x);
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
# 381
struct Cyc_List_List*Cyc_List_rtabulate_c(struct _RegionHandle*r,int n,void*(*f)(void*,int),void*env);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 69
struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 105
typedef struct _tuple2*Cyc_Absyn_qvar_t;typedef struct _tuple2*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple2*Cyc_Absyn_typedef_name_t;
typedef struct _tuple2*Cyc_Absyn_typedef_name_opt_t;
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{int tag;};
# 286
typedef void*Cyc_Absyn_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 334
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 352
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 365
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 385
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 390
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 443 "absyn.h"
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
# 445
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 458
typedef void*Cyc_Absyn_funcparams_t;
# 461
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 522
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 548
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 555
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 573
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 737 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 935
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 937
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 941
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 947
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 950
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
# 955
void*Cyc_Absyn_compress_kb(void*);
# 960
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 962
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 965
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 967
extern void*Cyc_Absyn_sint_typ;
# 969
void*Cyc_Absyn_float_typ(int);
# 971
extern void*Cyc_Absyn_empty_effect;
# 1024
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1071
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1075
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
# 1078
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1116
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1125
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_PP_Ppstate;
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
# 72
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);struct Cyc_RgnOrder_RgnPO;
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
typedef void*Cyc_Tcenv_jumpee_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};
# 100
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 104
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);
# 128 "tcenv.h"
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 137
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 154
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 211
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,void*r,int resetable,int opened);
# 213
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
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
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
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_List_List*patvars);
# 62
void Cyc_Tcpat_check_switch_exhaustive(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*p);
void Cyc_Tcpat_check_catch_overlap(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);struct _tuple14{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple14 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 52 "tcpat.cyc"
static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Pat*p){
void*_tmp0=p->r;void*_tmp1=_tmp0;struct _tuple2*_tmp3;struct _tuple2*_tmp5;struct Cyc_List_List*_tmp6;int _tmp7;struct Cyc_List_List*_tmp9;struct Cyc_List_List*_tmpA;int _tmpB;enum Cyc_Absyn_AggrKind _tmpD;struct _tuple2*_tmpE;struct Cyc_List_List*_tmpF;struct Cyc_List_List**_tmp10;struct Cyc_List_List*_tmp11;int _tmp12;struct Cyc_Absyn_Exp*_tmp14;_LL1: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp2=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1;if(_tmp2->tag != 15)goto _LL3;else{_tmp3=_tmp2->f1;}}_LL2:
# 55
{struct _handler_cons _tmp15;_push_handler(& _tmp15);{int _tmp17=0;if(setjmp(_tmp15.handler))_tmp17=1;if(!_tmp17){
{struct _RegionHandle _tmp18=_new_region("r");struct _RegionHandle*r=& _tmp18;_push_region(r);
{void*_tmp19=Cyc_Tcenv_lookup_ordinary(r,te,p->loc,_tmp3,0);void*_tmp1A=_tmp19;struct Cyc_Absyn_Datatypedecl*_tmp1D;struct Cyc_Absyn_Datatypefield*_tmp1E;struct Cyc_Absyn_Enumdecl*_tmp20;struct Cyc_Absyn_Enumfield*_tmp21;void*_tmp23;struct Cyc_Absyn_Enumfield*_tmp24;_LLE: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp1B=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmp1A;if(_tmp1B->tag != 1)goto _LL10;}_LLF:
# 59
 p->r=(void*)& Cyc_Absyn_Wild_p_val;
{const char*_tmp3E9;void*_tmp3E8;(_tmp3E8=0,Cyc_Tcutil_terr(p->loc,((_tmp3E9="struct tag used without arguments in pattern",_tag_dyneither(_tmp3E9,sizeof(char),45))),_tag_dyneither(_tmp3E8,sizeof(void*),0)));}
_npop_handler(1);return;_LL10: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp1C=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp1A;if(_tmp1C->tag != 2)goto _LL12;else{_tmp1D=_tmp1C->f1;_tmp1E=_tmp1C->f2;}}_LL11:
# 63
{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp3EC;struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp3EB;p->r=(void*)((_tmp3EB=_cycalloc(sizeof(*_tmp3EB)),((_tmp3EB[0]=((_tmp3EC.tag=8,((_tmp3EC.f1=_tmp1D,((_tmp3EC.f2=_tmp1E,((_tmp3EC.f3=0,((_tmp3EC.f4=0,_tmp3EC)))))))))),_tmp3EB))));}_npop_handler(1);return;_LL12: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp1F=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp1A;if(_tmp1F->tag != 3)goto _LL14;else{_tmp20=_tmp1F->f1;_tmp21=_tmp1F->f2;}}_LL13:
# 65
{struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct _tmp3EF;struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp3EE;p->r=(void*)((_tmp3EE=_cycalloc(sizeof(*_tmp3EE)),((_tmp3EE[0]=((_tmp3EF.tag=13,((_tmp3EF.f1=_tmp20,((_tmp3EF.f2=_tmp21,_tmp3EF)))))),_tmp3EE))));}
_npop_handler(1);return;_LL14: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp22=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp1A;if(_tmp22->tag != 4)goto _LL16;else{_tmp23=(void*)_tmp22->f1;_tmp24=_tmp22->f2;}}_LL15:
# 68
{struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct _tmp3F2;struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp3F1;p->r=(void*)((_tmp3F1=_cycalloc(sizeof(*_tmp3F1)),((_tmp3F1[0]=((_tmp3F2.tag=14,((_tmp3F2.f1=_tmp23,((_tmp3F2.f2=_tmp24,_tmp3F2)))))),_tmp3F1))));}
_npop_handler(1);return;_LL16: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp25=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp1A;if(_tmp25->tag != 0)goto _LLD;}_LL17:
 goto _LLD;_LLD:;}
# 57
;_pop_region(r);}
# 56
;_pop_handler();}else{void*_tmp16=(void*)_exn_thrown;void*_tmp2F=_tmp16;void*_tmp31;_LL19: {struct Cyc_Dict_Absent_exn_struct*_tmp30=(struct Cyc_Dict_Absent_exn_struct*)_tmp2F;if(_tmp30->tag != Cyc_Dict_Absent)goto _LL1B;}_LL1A:
# 73
 goto _LL18;_LL1B: _tmp31=_tmp2F;_LL1C:(void)_rethrow(_tmp31);_LL18:;}};}{
# 75
union Cyc_Absyn_Nmspace _tmp32=(*_tmp3).f1;union Cyc_Absyn_Nmspace _tmp33=_tmp32;_LL1E: if((_tmp33.Loc_n).tag != 4)goto _LL20;_LL1F:
 goto _LL21;_LL20: if((_tmp33.Rel_n).tag != 1)goto _LL22;if((struct Cyc_List_List*)(_tmp33.Rel_n).val != 0)goto _LL22;_LL21:
# 78
(*_tmp3).f1=Cyc_Absyn_Loc_n;
{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp3F5;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp3F4;p->r=(void*)((_tmp3F4=_cycalloc(sizeof(*_tmp3F4)),((_tmp3F4[0]=((_tmp3F5.tag=1,((_tmp3F5.f1=Cyc_Absyn_new_vardecl(_tmp3,(void*)& Cyc_Absyn_VoidType_val,0),((_tmp3F5.f2=
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0),_tmp3F5)))))),_tmp3F4))));}
return;_LL22:;_LL23:
{const char*_tmp3F8;void*_tmp3F7;(_tmp3F7=0,Cyc_Tcutil_terr(p->loc,((_tmp3F8="qualified variable in pattern",_tag_dyneither(_tmp3F8,sizeof(char),30))),_tag_dyneither(_tmp3F7,sizeof(void*),0)));}return;_LL1D:;};_LL3: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp4=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1;if(_tmp4->tag != 16)goto _LL5;else{_tmp5=_tmp4->f1;_tmp6=_tmp4->f2;_tmp7=_tmp4->f3;}}_LL4:
# 85
{struct _handler_cons _tmp38;_push_handler(& _tmp38);{int _tmp3A=0;if(setjmp(_tmp38.handler))_tmp3A=1;if(!_tmp3A){
{struct _RegionHandle _tmp3B=_new_region("r");struct _RegionHandle*r=& _tmp3B;_push_region(r);
{void*_tmp3C=Cyc_Tcenv_lookup_ordinary(r,te,p->loc,_tmp5,0);void*_tmp3D=_tmp3C;struct Cyc_Absyn_Aggrdecl*_tmp3F;struct Cyc_Absyn_Datatypedecl*_tmp41;struct Cyc_Absyn_Datatypefield*_tmp42;_LL25: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp3E=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmp3D;if(_tmp3E->tag != 1)goto _LL27;else{_tmp3F=_tmp3E->f1;}}_LL26: {
# 89
struct Cyc_List_List*_tmp46=0;
for(0;_tmp6 != 0;_tmp6=_tmp6->tl){
struct _tuple15*_tmp3FB;struct Cyc_List_List*_tmp3FA;_tmp46=((_tmp3FA=_cycalloc(sizeof(*_tmp3FA)),((_tmp3FA->hd=((_tmp3FB=_cycalloc(sizeof(*_tmp3FB)),((_tmp3FB->f1=0,((_tmp3FB->f2=(struct Cyc_Absyn_Pat*)_tmp6->hd,_tmp3FB)))))),((_tmp3FA->tl=_tmp46,_tmp3FA))))));}
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp405;struct Cyc_Absyn_Aggrdecl**_tmp404;struct Cyc_Absyn_AggrInfo*_tmp403;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp402;p->r=(void*)((_tmp402=_cycalloc(sizeof(*_tmp402)),((_tmp402[0]=((_tmp405.tag=7,((_tmp405.f1=((_tmp403=_cycalloc(sizeof(*_tmp403)),((_tmp403->aggr_info=Cyc_Absyn_KnownAggr(((_tmp404=_cycalloc(sizeof(*_tmp404)),((_tmp404[0]=_tmp3F,_tmp404))))),((_tmp403->targs=0,_tmp403)))))),((_tmp405.f2=0,((_tmp405.f3=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp46),((_tmp405.f4=_tmp7,_tmp405)))))))))),_tmp402))));}
_npop_handler(1);return;}_LL27: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp40=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp3D;if(_tmp40->tag != 2)goto _LL29;else{_tmp41=_tmp40->f1;_tmp42=_tmp40->f2;}}_LL28:
{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp408;struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp407;p->r=(void*)((_tmp407=_cycalloc(sizeof(*_tmp407)),((_tmp407[0]=((_tmp408.tag=8,((_tmp408.f1=_tmp41,((_tmp408.f2=_tmp42,((_tmp408.f3=_tmp6,((_tmp408.f4=_tmp7,_tmp408)))))))))),_tmp407))));}_npop_handler(1);return;_LL29: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp43=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp3D;if(_tmp43->tag != 3)goto _LL2B;}_LL2A:
 goto _LL2C;_LL2B: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp44=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp3D;if(_tmp44->tag != 4)goto _LL2D;}_LL2C:
# 98
{const char*_tmp40B;void*_tmp40A;(_tmp40A=0,Cyc_Tcutil_terr(p->loc,((_tmp40B="enum tag used with arguments in pattern",_tag_dyneither(_tmp40B,sizeof(char),40))),_tag_dyneither(_tmp40A,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
_npop_handler(1);return;_LL2D: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp45=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp3D;if(_tmp45->tag != 0)goto _LL24;}_LL2E:
 goto _LL24;_LL24:;}
# 87
;_pop_region(r);}
# 86
;_pop_handler();}else{void*_tmp39=(void*)_exn_thrown;void*_tmp52=_tmp39;void*_tmp54;_LL30: {struct Cyc_Dict_Absent_exn_struct*_tmp53=(struct Cyc_Dict_Absent_exn_struct*)_tmp52;if(_tmp53->tag != Cyc_Dict_Absent)goto _LL32;}_LL31:
# 103
 goto _LL2F;_LL32: _tmp54=_tmp52;_LL33:(void)_rethrow(_tmp54);_LL2F:;}};}
{const char*_tmp40F;void*_tmp40E[1];struct Cyc_String_pa_PrintArg_struct _tmp40D;(_tmp40D.tag=0,((_tmp40D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp5)),((_tmp40E[0]=& _tmp40D,Cyc_Tcutil_terr(p->loc,((_tmp40F="%s is not a constructor in pattern",_tag_dyneither(_tmp40F,sizeof(char),35))),_tag_dyneither(_tmp40E,sizeof(void*),1)))))));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;_LL5: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp8=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1;if(_tmp8->tag != 7)goto _LL7;else{if(_tmp8->f1 != 0)goto _LL7;_tmp9=_tmp8->f2;_tmpA=_tmp8->f3;_tmpB=_tmp8->f4;}}_LL6:
# 109
 if(topt == 0){
const char*_tmp412;void*_tmp411;(_tmp411=0,Cyc_Tcutil_terr(p->loc,((_tmp412="cannot determine pattern type",_tag_dyneither(_tmp412,sizeof(char),30))),_tag_dyneither(_tmp411,sizeof(void*),0)));}{
void*t=Cyc_Tcutil_compress(*((void**)_check_null(topt)));
{void*_tmp5A=t;struct Cyc_Absyn_AggrInfo _tmp5C;_LL35: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp5B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5A;if(_tmp5B->tag != 11)goto _LL37;else{_tmp5C=_tmp5B->f1;}}_LL36:
# 114
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp418;struct Cyc_Absyn_AggrInfo*_tmp417;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp416;p->r=(void*)((_tmp416=_cycalloc(sizeof(*_tmp416)),((_tmp416[0]=((_tmp418.tag=7,((_tmp418.f1=((_tmp417=_cycalloc(sizeof(*_tmp417)),((_tmp417[0]=_tmp5C,_tmp417)))),((_tmp418.f2=_tmp9,((_tmp418.f3=_tmpA,((_tmp418.f4=_tmpB,_tmp418)))))))))),_tmp416))));}
Cyc_Tcpat_resolve_pat(te,topt,p);
goto _LL34;_LL37:;_LL38:
# 118
{const char*_tmp41C;void*_tmp41B[1];struct Cyc_String_pa_PrintArg_struct _tmp41A;(_tmp41A.tag=0,((_tmp41A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp41B[0]=& _tmp41A,Cyc_Tcutil_terr(p->loc,((_tmp41C="pattern expects aggregate type instead of %s",_tag_dyneither(_tmp41C,sizeof(char),45))),_tag_dyneither(_tmp41B,sizeof(void*),1)))))));}
# 121
goto _LL34;_LL34:;}
# 123
return;};_LL7: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpC=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1;if(_tmpC->tag != 7)goto _LL9;else{if(_tmpC->f1 == 0)goto _LL9;if((((_tmpC->f1)->aggr_info).UnknownAggr).tag != 1)goto _LL9;_tmpD=((struct _tuple4)(((_tmpC->f1)->aggr_info).UnknownAggr).val).f1;_tmpE=((struct _tuple4)(((_tmpC->f1)->aggr_info).UnknownAggr).val).f2;_tmpF=(_tmpC->f1)->targs;_tmp10=(struct Cyc_List_List**)& _tmpC->f2;_tmp11=_tmpC->f3;_tmp12=_tmpC->f4;}}_LL8:
# 126
{struct _handler_cons _tmp63;_push_handler(& _tmp63);{int _tmp65=0;if(setjmp(_tmp63.handler))_tmp65=1;if(!_tmp65){
{struct Cyc_Absyn_Aggrdecl**_tmp66=Cyc_Tcenv_lookup_aggrdecl(te,p->loc,_tmpE);
struct Cyc_Absyn_Aggrdecl*_tmp67=*_tmp66;
if(_tmp67->impl == 0){
{const char*_tmp424;void*_tmp423[1];const char*_tmp422;const char*_tmp421;struct Cyc_String_pa_PrintArg_struct _tmp420;(_tmp420.tag=0,((_tmp420.f1=(struct _dyneither_ptr)(
_tmp67->kind == Cyc_Absyn_StructA?(_tmp421="struct",_tag_dyneither(_tmp421,sizeof(char),7)):((_tmp422="union",_tag_dyneither(_tmp422,sizeof(char),6)))),((_tmp423[0]=& _tmp420,Cyc_Tcutil_terr(p->loc,((_tmp424="can't destructure an abstract %s",_tag_dyneither(_tmp424,sizeof(char),33))),_tag_dyneither(_tmp423,sizeof(void*),1)))))));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
_npop_handler(0);return;}{
# 135
int more_exists=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp67->impl))->exist_vars)- ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*_tmp10);
if(more_exists < 0){
{const char*_tmp427;void*_tmp426;(_tmp426=0,Cyc_Tcutil_terr(p->loc,((_tmp427="too many existentially bound type variables in pattern",_tag_dyneither(_tmp427,sizeof(char),55))),_tag_dyneither(_tmp426,sizeof(void*),0)));}{
struct Cyc_List_List**_tmp6F=_tmp10;
{int n=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp67->impl))->exist_vars);for(0;n != 0;-- n){
_tmp6F=&((struct Cyc_List_List*)_check_null(*_tmp6F))->tl;}}
*_tmp6F=0;};}else{
if(more_exists > 0){
# 144
struct Cyc_List_List*_tmp70=0;
for(0;more_exists != 0;-- more_exists){
struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp42D;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp42C;struct Cyc_List_List*_tmp42B;_tmp70=((_tmp42B=_cycalloc(sizeof(*_tmp42B)),((_tmp42B->hd=Cyc_Tcutil_new_tvar((void*)((_tmp42D=_cycalloc(sizeof(*_tmp42D)),((_tmp42D[0]=((_tmp42C.tag=1,((_tmp42C.f1=0,_tmp42C)))),_tmp42D))))),((_tmp42B->tl=_tmp70,_tmp42B))))));}
*_tmp10=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp10,_tmp70);}}{
# 149
struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp433;struct Cyc_Absyn_AggrInfo*_tmp432;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp431;p->r=(void*)((_tmp431=_cycalloc(sizeof(*_tmp431)),((_tmp431[0]=((_tmp433.tag=7,((_tmp433.f1=((_tmp432=_cycalloc(sizeof(*_tmp432)),((_tmp432->aggr_info=Cyc_Absyn_KnownAggr(_tmp66),((_tmp432->targs=_tmpF,_tmp432)))))),((_tmp433.f2=*_tmp10,((_tmp433.f3=_tmp11,((_tmp433.f4=_tmp12,_tmp433)))))))))),_tmp431))));};};}
# 127
;_pop_handler();}else{void*_tmp64=(void*)_exn_thrown;void*_tmp78=_tmp64;void*_tmp7A;_LL3A: {struct Cyc_Dict_Absent_exn_struct*_tmp79=(struct Cyc_Dict_Absent_exn_struct*)_tmp78;if(_tmp79->tag != Cyc_Dict_Absent)goto _LL3C;}_LL3B:
# 152
{const char*_tmp436;void*_tmp435;(_tmp435=0,Cyc_Tcutil_terr(p->loc,((_tmp436="Non-aggregate name has designator patterns",_tag_dyneither(_tmp436,sizeof(char),43))),_tag_dyneither(_tmp435,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
goto _LL39;_LL3C: _tmp7A=_tmp78;_LL3D:(void)_rethrow(_tmp7A);_LL39:;}};}
# 156
return;_LL9: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp13=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1;if(_tmp13->tag != 17)goto _LLB;else{_tmp14=_tmp13->f1;}}_LLA:
# 158
 Cyc_Tcexp_tcExp(te,0,_tmp14);
if(!Cyc_Tcutil_is_const_exp(_tmp14)){
{const char*_tmp439;void*_tmp438;(_tmp438=0,Cyc_Tcutil_terr(p->loc,((_tmp439="non-constant expression in case pattern",_tag_dyneither(_tmp439,sizeof(char),40))),_tag_dyneither(_tmp438,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;}{
# 163
struct _tuple14 _tmp7F=Cyc_Evexp_eval_const_uint_exp(_tmp14);unsigned int _tmp81;int _tmp82;struct _tuple14 _tmp80=_tmp7F;_tmp81=_tmp80.f1;_tmp82=_tmp80.f2;
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmp43C;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp43B;p->r=(void*)((_tmp43B=_cycalloc_atomic(sizeof(*_tmp43B)),((_tmp43B[0]=((_tmp43C.tag=10,((_tmp43C.f1=Cyc_Absyn_None,((_tmp43C.f2=(int)_tmp81,_tmp43C)))))),_tmp43B))));}
return;};_LLB:;_LLC:
 return;_LL0:;}
# 170
static struct _dyneither_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){
return(*vd->name).f2;}
# 173
static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**topt){
if(topt != 0)
return*topt;{
struct Cyc_Core_Opt*_tmp43D;return Cyc_Absyn_new_evar(& Cyc_Tcutil_mko,((_tmp43D=_cycalloc(sizeof(*_tmp43D)),((_tmp43D->v=s,_tmp43D)))));};}
# 178
static void*Cyc_Tcpat_num_type(void**topt,void*numt){
# 182
if(topt != 0  && Cyc_Tcutil_coerceable(*topt))
return*topt;
# 185
{void*_tmp86=Cyc_Tcutil_compress(numt);void*_tmp87=_tmp86;_LL3F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp88=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp87;if(_tmp88->tag != 13)goto _LL41;}_LL40:
 goto _LL42;_LL41: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp89=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp87;if(_tmp89->tag != 14)goto _LL43;}_LL42:
 if(topt != 0)return*topt;goto _LL3E;_LL43:;_LL44:
 goto _LL3E;_LL3E:;}
# 190
return numt;}struct _tuple16{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 193
static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl**vd,struct Cyc_Absyn_Exp*e,struct Cyc_List_List**v_result_ptr,void*t){
# 195
if(vd != 0){
(*vd)->type=t;
(*vd)->tq=Cyc_Absyn_empty_tqual(0);}{
# 199
struct _tuple16*_tmp440;struct Cyc_List_List*_tmp43F;*v_result_ptr=((_tmp43F=_cycalloc(sizeof(*_tmp43F)),((_tmp43F->hd=((_tmp440=_cycalloc(sizeof(*_tmp440)),((_tmp440->f1=vd,((_tmp440->f2=e,_tmp440)))))),((_tmp43F->tl=*v_result_ptr,_tmp43F))))));};}
# 201
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,struct Cyc_Tcpat_TcPatResult res2){
# 203
struct _tuple1*_tmp8D;struct Cyc_List_List*_tmp8E;struct Cyc_Tcpat_TcPatResult _tmp8C=res1;_tmp8D=_tmp8C.tvars_and_bounds_opt;_tmp8E=_tmp8C.patvars;{
struct _tuple1*_tmp90;struct Cyc_List_List*_tmp91;struct Cyc_Tcpat_TcPatResult _tmp8F=res2;_tmp90=_tmp8F.tvars_and_bounds_opt;_tmp91=_tmp8F.patvars;
if(_tmp8D != 0  || _tmp90 != 0){
if(_tmp8D == 0){
struct _tuple1*_tmp441;_tmp8D=((_tmp441=_cycalloc(sizeof(*_tmp441)),((_tmp441->f1=0,((_tmp441->f2=0,_tmp441))))));}
if(_tmp90 == 0){
struct _tuple1*_tmp442;_tmp90=((_tmp442=_cycalloc(sizeof(*_tmp442)),((_tmp442->f1=0,((_tmp442->f2=0,_tmp442))))));}{
struct _tuple1*_tmp445;struct Cyc_Tcpat_TcPatResult _tmp444;return(_tmp444.tvars_and_bounds_opt=((_tmp445=_cycalloc(sizeof(*_tmp445)),((_tmp445->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp8D).f1,(*_tmp90).f1),((_tmp445->f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp8D).f2,(*_tmp90).f2),_tmp445)))))),((_tmp444.patvars=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp8E,_tmp91),_tmp444)));};}{
# 214
struct Cyc_Tcpat_TcPatResult _tmp446;return(_tmp446.tvars_and_bounds_opt=0,((_tmp446.patvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp8E,_tmp91),_tmp446)));};};}
# 217
static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(unsigned int loc){
struct Cyc_Absyn_Pat*_tmp447;return(_tmp447=_cycalloc(sizeof(*_tmp447)),((_tmp447->loc=loc,((_tmp447->topt=0,((_tmp447->r=(void*)& Cyc_Absyn_Wild_p_val,_tmp447)))))));}
# 222
static void*Cyc_Tcpat_pat_promote_array(struct Cyc_Tcenv_Tenv*te,void*t,void*rgn_opt){
struct Cyc_Core_Opt*_tmp448;return Cyc_Tcutil_is_array(t)?
Cyc_Tcutil_promote_array(t,rgn_opt == 0?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,((_tmp448=_cycalloc(sizeof(*_tmp448)),((_tmp448->v=Cyc_Tcenv_lookup_type_vars(te),_tmp448))))): rgn_opt,0): t;}struct _tuple17{struct Cyc_Absyn_Tvar*f1;int f2;};
# 229
static struct _tuple17*Cyc_Tcpat_add_false(struct Cyc_Absyn_Tvar*tv){
struct _tuple17*_tmp449;return(_tmp449=_cycalloc(sizeof(*_tmp449)),((_tmp449->f1=tv,((_tmp449->f2=0,_tmp449)))));}struct _tuple18{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Pat*f2;};
# 233
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void*rgn_pat,int allow_ref_pat,struct Cyc_Absyn_Exp*access_exp){
# 236
Cyc_Tcpat_resolve_pat(te,topt,p);{
void*t;
struct Cyc_Tcpat_TcPatResult _tmp44A;struct Cyc_Tcpat_TcPatResult res=(_tmp44A.tvars_and_bounds_opt=0,((_tmp44A.patvars=0,_tmp44A)));
# 241
{void*_tmp9A=p->r;void*_tmp9B=_tmp9A;struct Cyc_Absyn_Vardecl*_tmp9E;struct Cyc_Absyn_Pat*_tmp9F;struct Cyc_Absyn_Tvar*_tmpA1;struct Cyc_Absyn_Vardecl*_tmpA2;struct Cyc_Absyn_Vardecl*_tmpA4;struct Cyc_Absyn_Pat*_tmpA5;struct Cyc_Absyn_Tvar*_tmpA7;struct Cyc_Absyn_Vardecl*_tmpA8;int _tmpAE;struct Cyc_Absyn_Enumdecl*_tmpB0;void*_tmpB2;struct Cyc_Absyn_Pat*_tmpB5;struct Cyc_List_List**_tmpB7;int _tmpB8;struct Cyc_Absyn_Aggrdecl*_tmpBA;struct Cyc_List_List**_tmpBB;struct Cyc_List_List*_tmpBC;struct Cyc_List_List**_tmpBD;int _tmpBE;struct Cyc_Absyn_Datatypedecl*_tmpC0;struct Cyc_Absyn_Datatypefield*_tmpC1;struct Cyc_List_List**_tmpC2;int _tmpC3;_LL46: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp9C=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmp9C->tag != 0)goto _LL48;}_LL47:
# 244
 if(topt != 0)
t=*topt;else{
# 247
t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(te),topt);}
goto _LL45;_LL48: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp9D=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmp9D->tag != 1)goto _LL4A;else{_tmp9E=_tmp9D->f1;_tmp9F=_tmp9D->f2;}}_LL49: {
# 251
struct _tuple2*_tmpC9=_tmp9E->name;struct _dyneither_ptr _tmpCB;struct _tuple2*_tmpCA=_tmpC9;_tmpCB=*_tmpCA->f2;
{const char*_tmp44C;const char*_tmp44B;if(Cyc_strcmp((struct _dyneither_ptr)_tmpCB,((_tmp44C="true",_tag_dyneither(_tmp44C,sizeof(char),5))))== 0  || Cyc_strcmp((struct _dyneither_ptr)_tmpCB,((_tmp44B="false",_tag_dyneither(_tmp44B,sizeof(char),6))))== 0){
const char*_tmp450;void*_tmp44F[1];struct Cyc_String_pa_PrintArg_struct _tmp44E;(_tmp44E.tag=0,((_tmp44E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpCB),((_tmp44F[0]=& _tmp44E,Cyc_Tcutil_warn(p->loc,((_tmp450="you probably do not want to use %s as a local variable...",_tag_dyneither(_tmp450,sizeof(char),58))),_tag_dyneither(_tmp44F,sizeof(void*),1)))))));}}
# 255
res=Cyc_Tcpat_tcPatRec(te,_tmp9F,topt,rgn_pat,allow_ref_pat,access_exp);
t=(void*)_check_null(_tmp9F->topt);
# 259
{void*_tmpD1=Cyc_Tcutil_compress(t);void*_tmpD2=_tmpD1;_LL73: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD3=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD2;if(_tmpD3->tag != 8)goto _LL75;}_LL74:
# 261
 if(rgn_pat == 0  || !allow_ref_pat){
const char*_tmp453;void*_tmp452;(_tmp452=0,Cyc_Tcutil_terr(p->loc,((_tmp453="array reference would point into unknown/unallowed region",_tag_dyneither(_tmp453,sizeof(char),58))),_tag_dyneither(_tmp452,sizeof(void*),0)));}
goto _LL72;_LL75:;_LL76:
# 265
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tmk)){
const char*_tmp456;void*_tmp455;(_tmp455=0,Cyc_Tcutil_terr(p->loc,((_tmp456="pattern would point to an abstract member",_tag_dyneither(_tmp456,sizeof(char),42))),_tag_dyneither(_tmp455,sizeof(void*),0)));}
goto _LL72;_LL72:;}
# 269
{struct Cyc_Absyn_Vardecl**_tmp457;Cyc_Tcpat_set_vd(((_tmp457=_cycalloc(sizeof(*_tmp457)),((_tmp457[0]=_tmp9E,_tmp457)))),access_exp,& res.patvars,Cyc_Tcpat_pat_promote_array(te,t,rgn_pat));}
goto _LL45;}_LL4A: {struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmpA0=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpA0->tag != 2)goto _LL4C;else{_tmpA1=_tmpA0->f1;_tmpA2=_tmpA0->f2;}}_LL4B: {
# 272
struct _RegionHandle _tmpD9=_new_region("r2");struct _RegionHandle*r2=& _tmpD9;_push_region(r2);{
struct Cyc_Absyn_Tvar*_tmp458[1];struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r2,p->loc,te,((_tmp458[0]=_tmpA1,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(r2,_tag_dyneither(_tmp458,sizeof(struct Cyc_Absyn_Tvar*),1)))));
if(res.tvars_and_bounds_opt == 0){
struct _tuple1*_tmp459;res.tvars_and_bounds_opt=((_tmp459=_cycalloc(sizeof(*_tmp459)),((_tmp459->f1=0,((_tmp459->f2=0,_tmp459))))));}
{struct _tuple17*_tmp45C;struct Cyc_List_List*_tmp45B;(*res.tvars_and_bounds_opt).f1=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f1,((_tmp45B=_cycalloc(sizeof(*_tmp45B)),((_tmp45B->hd=((_tmp45C=_cycalloc(sizeof(*_tmp45C)),((_tmp45C->f1=_tmpA1,((_tmp45C->f2=1,_tmp45C)))))),((_tmp45B->tl=0,_tmp45B)))))));}
Cyc_Tcutil_check_type(p->loc,te2,Cyc_Tcenv_lookup_type_vars(te2),& Cyc_Tcutil_tmk,1,_tmpA2->type);}
# 282
if(topt != 0)t=*topt;else{
t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(te),topt);}
{void*_tmpDE=Cyc_Tcutil_compress(t);void*_tmpDF=_tmpDE;_LL78: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpE0=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpDF;if(_tmpE0->tag != 8)goto _LL7A;}_LL79:
# 286
 if(rgn_pat == 0  || !allow_ref_pat){
const char*_tmp45F;void*_tmp45E;(_tmp45E=0,Cyc_Tcutil_terr(p->loc,((_tmp45F="array reference would point into unknown/unallowed region",_tag_dyneither(_tmp45F,sizeof(char),58))),_tag_dyneither(_tmp45E,sizeof(void*),0)));}
goto _LL77;_LL7A:;_LL7B:
# 290
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tmk)){
const char*_tmp462;void*_tmp461;(_tmp461=0,Cyc_Tcutil_terr(p->loc,((_tmp462="pattern would point to an abstract member",_tag_dyneither(_tmp462,sizeof(char),42))),_tag_dyneither(_tmp461,sizeof(void*),0)));}
goto _LL77;_LL77:;}
# 294
{struct Cyc_Absyn_Vardecl**_tmp463;Cyc_Tcpat_set_vd(((_tmp463=_cycalloc(sizeof(*_tmp463)),((_tmp463[0]=_tmpA2,_tmp463)))),access_exp,& res.patvars,_tmpA2->type);}
_npop_handler(0);goto _LL45;
# 272
;_pop_region(r2);}_LL4C: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpA3=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpA3->tag != 3)goto _LL4E;else{_tmpA4=_tmpA3->f1;_tmpA5=_tmpA3->f2;}}_LL4D:
# 298
 res=Cyc_Tcpat_tcPatRec(te,_tmpA5,topt,rgn_pat,allow_ref_pat,access_exp);
t=(void*)_check_null(_tmpA5->topt);
if(!allow_ref_pat  || rgn_pat == 0){
{const char*_tmp466;void*_tmp465;(_tmp465=0,Cyc_Tcutil_terr(p->loc,((_tmp466="* pattern would point into an unknown/unallowed region",_tag_dyneither(_tmp466,sizeof(char),55))),_tag_dyneither(_tmp465,sizeof(void*),0)));}
goto _LL45;}else{
# 305
struct _RegionHandle _tmpE8=_new_region("rgn");struct _RegionHandle*rgn=& _tmpE8;_push_region(rgn);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t)){
const char*_tmp469;void*_tmp468;(_tmp468=0,Cyc_Tcutil_terr(p->loc,((_tmp469="* pattern cannot take the address of an alias-free path",_tag_dyneither(_tmp469,sizeof(char),56))),_tag_dyneither(_tmp468,sizeof(void*),0)));}
# 306
;_pop_region(rgn);}{
# 311
struct Cyc_Absyn_Exp*new_access_exp=0;
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp46F;struct Cyc_Absyn_PtrInfo _tmp46E;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp46D;void*t2=(void*)((_tmp46D=_cycalloc(sizeof(*_tmp46D)),((_tmp46D[0]=((_tmp46F.tag=5,((_tmp46F.f1=((_tmp46E.elt_typ=t,((_tmp46E.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp46E.ptr_atts=(
((_tmp46E.ptr_atts).rgn=rgn_pat,(((_tmp46E.ptr_atts).nullable=Cyc_Absyn_false_conref,(((_tmp46E.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp46E.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp46E.ptr_atts).ptrloc=0,_tmp46E.ptr_atts)))))))))),_tmp46E)))))),_tmp46F)))),_tmp46D))));
# 316
if((unsigned int)access_exp){
new_access_exp=Cyc_Absyn_address_exp(access_exp,0);
new_access_exp->topt=t2;}
# 320
{struct Cyc_Absyn_Vardecl**_tmp470;Cyc_Tcpat_set_vd(((_tmp470=_cycalloc(sizeof(*_tmp470)),((_tmp470[0]=_tmpA4,_tmp470)))),new_access_exp,& res.patvars,t2);}
goto _LL45;};_LL4E: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpA6=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpA6->tag != 4)goto _LL50;else{_tmpA7=_tmpA6->f1;_tmpA8=_tmpA6->f2;}}_LL4F:
# 326
{struct Cyc_Absyn_Vardecl**_tmp471;Cyc_Tcpat_set_vd(((_tmp471=_cycalloc(sizeof(*_tmp471)),((_tmp471[0]=_tmpA8,_tmp471)))),access_exp,& res.patvars,_tmpA8->type);}{
# 330
struct _RegionHandle _tmpF0=_new_region("r2");struct _RegionHandle*r2=& _tmpF0;_push_region(r2);{
struct Cyc_Absyn_Tvar*_tmp472[1];Cyc_Tcenv_add_type_vars(r2,p->loc,te,((_tmp472[0]=_tmpA7,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp472,sizeof(struct Cyc_Absyn_Tvar*),1)))));}
# 333
if(res.tvars_and_bounds_opt == 0){
struct _tuple1*_tmp473;res.tvars_and_bounds_opt=((_tmp473=_cycalloc(sizeof(*_tmp473)),((_tmp473->f1=0,((_tmp473->f2=0,_tmp473))))));}
{struct _tuple17*_tmp476;struct Cyc_List_List*_tmp475;(*res.tvars_and_bounds_opt).f1=(
(_tmp475=_cycalloc(sizeof(*_tmp475)),((_tmp475->hd=((_tmp476=_cycalloc(sizeof(*_tmp476)),((_tmp476->f1=_tmpA7,((_tmp476->f2=0,_tmp476)))))),((_tmp475->tl=(*res.tvars_and_bounds_opt).f1,_tmp475))))));}
t=Cyc_Absyn_uint_typ;
_npop_handler(0);goto _LL45;
# 330
;_pop_region(r2);};_LL50: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpA9=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpA9->tag != 10)goto _LL52;else{if(_tmpA9->f1 != Cyc_Absyn_Unsigned)goto _LL52;}}_LL51:
# 340
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_typ);goto _LL45;_LL52: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpAA=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpAA->tag != 10)goto _LL54;else{if(_tmpAA->f1 != Cyc_Absyn_None)goto _LL54;}}_LL53:
 goto _LL55;_LL54: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpAB=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpAB->tag != 10)goto _LL56;else{if(_tmpAB->f1 != Cyc_Absyn_Signed)goto _LL56;}}_LL55:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_typ);goto _LL45;_LL56: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmpAC=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpAC->tag != 11)goto _LL58;}_LL57:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_typ);goto _LL45;_LL58: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmpAD=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpAD->tag != 12)goto _LL5A;else{_tmpAE=_tmpAD->f2;}}_LL59:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_float_typ(_tmpAE));goto _LL45;_LL5A: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmpAF=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpAF->tag != 13)goto _LL5C;else{_tmpB0=_tmpAF->f1;}}_LL5B:
# 346
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp479;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp478;t=Cyc_Tcpat_num_type(topt,(void*)((_tmp478=_cycalloc(sizeof(*_tmp478)),((_tmp478[0]=((_tmp479.tag=13,((_tmp479.f1=_tmpB0->name,((_tmp479.f2=_tmpB0,_tmp479)))))),_tmp478)))));}
goto _LL45;_LL5C: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmpB1=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpB1->tag != 14)goto _LL5E;else{_tmpB2=(void*)_tmpB1->f1;}}_LL5D:
 t=Cyc_Tcpat_num_type(topt,_tmpB2);goto _LL45;_LL5E: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmpB3=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpB3->tag != 9)goto _LL60;}_LL5F:
# 350
 if(topt != 0){
void*_tmpF7=Cyc_Tcutil_compress(*topt);void*_tmpF8=_tmpF7;_LL7D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpF9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF8;if(_tmpF9->tag != 5)goto _LL7F;}_LL7E:
 t=*topt;goto tcpat_end;_LL7F:;_LL80:
 goto _LL7C;_LL7C:;}{
# 355
struct Cyc_Core_Opt*_tmpFA=Cyc_Tcenv_lookup_opt_type_vars(te);
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp47F;struct Cyc_Absyn_PtrInfo _tmp47E;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp47D;t=(void*)((_tmp47D=_cycalloc(sizeof(*_tmp47D)),((_tmp47D[0]=((_tmp47F.tag=5,((_tmp47F.f1=((_tmp47E.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmpFA),((_tmp47E.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp47E.ptr_atts=(
((_tmp47E.ptr_atts).rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmpFA),(((_tmp47E.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp47E.ptr_atts).bounds=
# 360
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp47E.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp47E.ptr_atts).ptrloc=0,_tmp47E.ptr_atts)))))))))),_tmp47E)))))),_tmp47F)))),_tmp47D))));}
# 363
goto _LL45;};_LL60: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpB4=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpB4->tag != 6)goto _LL62;else{_tmpB5=_tmpB4->f1;}}_LL61: {
# 368
void*inner_typ=(void*)& Cyc_Absyn_VoidType_val;
void**_tmpFE=0;
int elt_const=0;
if(topt != 0){
void*_tmpFF=Cyc_Tcutil_compress(*topt);void*_tmp100=_tmpFF;void*_tmp102;struct Cyc_Absyn_Tqual _tmp103;_LL82: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp101=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp100;if(_tmp101->tag != 5)goto _LL84;else{_tmp102=(_tmp101->f1).elt_typ;_tmp103=(_tmp101->f1).elt_tq;}}_LL83:
# 374
 inner_typ=_tmp102;
_tmpFE=& inner_typ;
elt_const=_tmp103.real_const;
goto _LL81;_LL84:;_LL85:
 goto _LL81;_LL81:;}{
# 383
void*ptr_rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp)new_access_exp=Cyc_Absyn_deref_exp(access_exp,0);
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,_tmpB5,_tmpFE,ptr_rgn,1,new_access_exp));
# 392
{void*_tmp104=Cyc_Tcutil_compress((void*)_check_null(_tmpB5->topt));void*_tmp105=_tmp104;struct Cyc_Absyn_Datatypedecl*_tmp107;struct Cyc_Absyn_Datatypefield*_tmp108;struct Cyc_List_List*_tmp109;_LL87: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp106=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp105;if(_tmp106->tag != 4)goto _LL89;else{if((((_tmp106->f1).field_info).KnownDatatypefield).tag != 2)goto _LL89;_tmp107=((struct _tuple3)(((_tmp106->f1).field_info).KnownDatatypefield).val).f1;_tmp108=((struct _tuple3)(((_tmp106->f1).field_info).KnownDatatypefield).val).f2;_tmp109=(_tmp106->f1).targs;}}_LL88:
# 396
{void*_tmp10A=Cyc_Tcutil_compress(inner_typ);void*_tmp10B=_tmp10A;_LL8C: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp10C=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp10B;if(_tmp10C->tag != 4)goto _LL8E;}_LL8D:
# 398
 goto DONT_PROMOTE;_LL8E:;_LL8F:
 goto _LL8B;_LL8B:;}{
# 402
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp489;struct Cyc_Absyn_Datatypedecl**_tmp488;struct Cyc_Absyn_DatatypeInfo _tmp487;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp486;void*new_type=(void*)((_tmp486=_cycalloc(sizeof(*_tmp486)),((_tmp486[0]=((_tmp489.tag=3,((_tmp489.f1=((_tmp487.datatype_info=Cyc_Absyn_KnownDatatype(((_tmp488=_cycalloc(sizeof(*_tmp488)),((_tmp488[0]=_tmp107,_tmp488))))),((_tmp487.targs=_tmp109,_tmp487)))),_tmp489)))),_tmp486))));
# 405
_tmpB5->topt=new_type;
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp48F;struct Cyc_Absyn_PtrInfo _tmp48E;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp48D;t=(void*)((_tmp48D=_cycalloc(sizeof(*_tmp48D)),((_tmp48D[0]=((_tmp48F.tag=5,((_tmp48F.f1=((_tmp48E.elt_typ=new_type,((_tmp48E.elt_tq=
elt_const?Cyc_Absyn_const_tqual(0):
 Cyc_Absyn_empty_tqual(0),((_tmp48E.ptr_atts=(
((_tmp48E.ptr_atts).rgn=ptr_rgn,(((_tmp48E.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp48E.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref,(((_tmp48E.ptr_atts).zero_term=Cyc_Absyn_false_conref,(((_tmp48E.ptr_atts).ptrloc=0,_tmp48E.ptr_atts)))))))))),_tmp48E)))))),_tmp48F)))),_tmp48D))));}
# 412
goto _LL86;};_LL89:;_LL8A:
# 414
 DONT_PROMOTE: {
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp495;struct Cyc_Absyn_PtrInfo _tmp494;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp493;t=(void*)((_tmp493=_cycalloc(sizeof(*_tmp493)),((_tmp493[0]=((_tmp495.tag=5,((_tmp495.f1=((_tmp494.elt_typ=(void*)_check_null(_tmpB5->topt),((_tmp494.elt_tq=
elt_const?Cyc_Absyn_const_tqual(0):
 Cyc_Absyn_empty_tqual(0),((_tmp494.ptr_atts=(
((_tmp494.ptr_atts).rgn=ptr_rgn,(((_tmp494.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp494.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp494.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp494.ptr_atts).ptrloc=0,_tmp494.ptr_atts)))))))))),_tmp494)))))),_tmp495)))),_tmp493))));}_LL86:;}
# 422
if((unsigned int)new_access_exp)new_access_exp->topt=_tmpB5->topt;
goto _LL45;};}_LL62: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmpB6=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpB6->tag != 5)goto _LL64;else{_tmpB7=(struct Cyc_List_List**)& _tmpB6->f1;_tmpB8=_tmpB6->f2;}}_LL63: {
# 426
struct Cyc_List_List*_tmp117=*_tmpB7;
struct Cyc_List_List*pat_ts=0;
struct Cyc_List_List*topt_ts=0;
if(topt != 0){
void*_tmp118=Cyc_Tcutil_compress(*topt);void*_tmp119=_tmp118;struct Cyc_List_List*_tmp11B;_LL91: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp11A=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp119;if(_tmp11A->tag != 10)goto _LL93;else{_tmp11B=_tmp11A->f1;}}_LL92:
# 432
 topt_ts=_tmp11B;
if(_tmpB8){
# 435
int _tmp11C=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp117);
int _tmp11D=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp11B);
if(_tmp11C < _tmp11D){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmp11D - _tmp11C;++ i){
struct Cyc_List_List*_tmp496;wild_ps=((_tmp496=_cycalloc(sizeof(*_tmp496)),((_tmp496->hd=Cyc_Tcpat_wild_pat(p->loc),((_tmp496->tl=wild_ps,_tmp496))))));}}
*_tmpB7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp117,wild_ps);
_tmp117=*_tmpB7;}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp117)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp11B)){
const char*_tmp499;void*_tmp498;(_tmp498=0,Cyc_Tcutil_warn(p->loc,((_tmp499="unnecessary ... in tuple pattern",_tag_dyneither(_tmp499,sizeof(char),33))),_tag_dyneither(_tmp498,sizeof(void*),0)));}}}
# 447
goto _LL90;_LL93:;_LL94:
# 449
 goto _LL90;_LL90:;}else{
# 451
if(_tmpB8){
const char*_tmp49C;void*_tmp49B;(_tmp49B=0,Cyc_Tcutil_terr(p->loc,((_tmp49C="cannot determine missing fields for ... in tuple pattern",_tag_dyneither(_tmp49C,sizeof(char),57))),_tag_dyneither(_tmp49B,sizeof(void*),0)));}}
{int i=0;for(0;_tmp117 != 0;(_tmp117=_tmp117->tl,i ++)){
void**_tmp123=0;
if(topt_ts != 0){
_tmp123=&(*((struct _tuple18*)topt_ts->hd)).f2;
topt_ts=topt_ts->tl;}{
# 459
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp){
struct _tuple7 _tmp49F;union Cyc_Absyn_Cnst _tmp49E;new_access_exp=Cyc_Absyn_subscript_exp(access_exp,
Cyc_Absyn_const_exp((((_tmp49E.Int_c).val=((_tmp49F.f1=Cyc_Absyn_Unsigned,((_tmp49F.f2=i,_tmp49F)))),(((_tmp49E.Int_c).tag=5,_tmp49E)))),0),0);}
# 464
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)_tmp117->hd,_tmp123,rgn_pat,allow_ref_pat,new_access_exp));
# 467
if((unsigned int)new_access_exp)new_access_exp->topt=((struct Cyc_Absyn_Pat*)_tmp117->hd)->topt;{
struct _tuple18*_tmp4A2;struct Cyc_List_List*_tmp4A1;pat_ts=((_tmp4A1=_cycalloc(sizeof(*_tmp4A1)),((_tmp4A1->hd=((_tmp4A2=_cycalloc(sizeof(*_tmp4A2)),((_tmp4A2->f1=Cyc_Absyn_empty_tqual(0),((_tmp4A2->f2=(void*)_check_null(((struct Cyc_Absyn_Pat*)_tmp117->hd)->topt),_tmp4A2)))))),((_tmp4A1->tl=pat_ts,_tmp4A1))))));};};}}
# 470
{struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp4A5;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp4A4;t=(void*)((_tmp4A4=_cycalloc(sizeof(*_tmp4A4)),((_tmp4A4[0]=((_tmp4A5.tag=10,((_tmp4A5.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(pat_ts),_tmp4A5)))),_tmp4A4))));}
goto _LL45;}_LL64: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpB9=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpB9->tag != 7)goto _LL66;else{if(_tmpB9->f1 == 0)goto _LL66;if((((_tmpB9->f1)->aggr_info).KnownAggr).tag != 2)goto _LL66;_tmpBA=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpB9->f1)->aggr_info).KnownAggr).val);_tmpBB=(struct Cyc_List_List**)&(_tmpB9->f1)->targs;_tmpBC=_tmpB9->f2;_tmpBD=(struct Cyc_List_List**)& _tmpB9->f3;_tmpBE=_tmpB9->f4;}}_LL65: {
# 474
struct Cyc_List_List*_tmp12A=*_tmpBD;
const char*_tmp4A7;const char*_tmp4A6;struct _dyneither_ptr aggr_str=_tmpBA->kind == Cyc_Absyn_StructA?(_tmp4A7="struct",_tag_dyneither(_tmp4A7,sizeof(char),7)):((_tmp4A6="union",_tag_dyneither(_tmp4A6,sizeof(char),6)));
if(_tmpBA->impl == 0){
{const char*_tmp4AB;void*_tmp4AA[1];struct Cyc_String_pa_PrintArg_struct _tmp4A9;(_tmp4A9.tag=0,((_tmp4A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp4AA[0]=& _tmp4A9,Cyc_Tcutil_terr(p->loc,((_tmp4AB="can't destructure an abstract %s",_tag_dyneither(_tmp4AB,sizeof(char),33))),_tag_dyneither(_tmp4AA,sizeof(void*),1)))))));}
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));
goto _LL45;}
# 483
if(_tmpBA->kind == Cyc_Absyn_UnionA  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpBA->impl))->tagged)
allow_ref_pat=0;
if(_tmpBC != 0){
# 489
if(topt == 0  || Cyc_Tcutil_typ_kind(*topt)!= & Cyc_Tcutil_ak)
allow_ref_pat=0;}{
# 492
struct _RegionHandle _tmp12E=_new_region("rgn");struct _RegionHandle*rgn=& _tmp12E;_push_region(rgn);{
# 494
struct Cyc_List_List*_tmp12F=0;
struct Cyc_List_List*outlives_constraints=0;
struct Cyc_List_List*_tmp130=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpBA->impl))->exist_vars;
{struct Cyc_List_List*t=_tmpBC;for(0;t != 0;t=t->tl){
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;
struct Cyc_Absyn_Tvar*uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp130))->hd;
_tmp130=_tmp130->tl;{
void*_tmp131=Cyc_Absyn_compress_kb(tv->kind);
void*_tmp132=Cyc_Absyn_compress_kb(uv->kind);
int error=0;
struct Cyc_Absyn_Kind*k2;
{void*_tmp133=_tmp132;struct Cyc_Absyn_Kind*_tmp135;struct Cyc_Absyn_Kind*_tmp137;_LL96: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp134=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp133;if(_tmp134->tag != 2)goto _LL98;else{_tmp135=_tmp134->f2;}}_LL97:
 _tmp137=_tmp135;goto _LL99;_LL98: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp136=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp133;if(_tmp136->tag != 0)goto _LL9A;else{_tmp137=_tmp136->f1;}}_LL99:
 k2=_tmp137;goto _LL95;_LL9A:;_LL9B: {
const char*_tmp4AE;void*_tmp4AD;(_tmp4AD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4AE="unconstrained existential type variable in aggregate",_tag_dyneither(_tmp4AE,sizeof(char),53))),_tag_dyneither(_tmp4AD,sizeof(void*),0)));}_LL95:;}
# 510
{void*_tmp13A=_tmp131;struct Cyc_Absyn_Kind*_tmp13C;struct Cyc_Core_Opt**_tmp13E;struct Cyc_Absyn_Kind*_tmp13F;struct Cyc_Core_Opt**_tmp141;_LL9D: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp13B=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp13A;if(_tmp13B->tag != 0)goto _LL9F;else{_tmp13C=_tmp13B->f1;}}_LL9E:
# 513
 if(!Cyc_Tcutil_kind_leq(k2,_tmp13C))
error=1;
goto _LL9C;_LL9F: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp13D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp13A;if(_tmp13D->tag != 2)goto _LLA1;else{_tmp13E=(struct Cyc_Core_Opt**)& _tmp13D->f1;_tmp13F=_tmp13D->f2;}}_LLA0:
 _tmp141=_tmp13E;goto _LLA2;_LLA1: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp140=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp13A;if(_tmp140->tag != 1)goto _LL9C;else{_tmp141=(struct Cyc_Core_Opt**)& _tmp140->f1;}}_LLA2:
{struct Cyc_Core_Opt*_tmp4AF;*_tmp141=((_tmp4AF=_cycalloc(sizeof(*_tmp4AF)),((_tmp4AF->v=_tmp132,_tmp4AF))));}goto _LL9C;_LL9C:;}
# 519
if(error){
const char*_tmp4B5;void*_tmp4B4[3];struct Cyc_String_pa_PrintArg_struct _tmp4B3;struct Cyc_String_pa_PrintArg_struct _tmp4B2;struct Cyc_String_pa_PrintArg_struct _tmp4B1;(_tmp4B1.tag=0,((_tmp4B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 523
Cyc_Absynpp_kind2string(k2)),((_tmp4B2.tag=0,((_tmp4B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(_tmp131)),((_tmp4B3.tag=0,((_tmp4B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmp4B4[0]=& _tmp4B3,((_tmp4B4[1]=& _tmp4B2,((_tmp4B4[2]=& _tmp4B1,Cyc_Tcutil_terr(p->loc,((_tmp4B5="type variable %s has kind %s but must have at least kind %s",_tag_dyneither(_tmp4B5,sizeof(char),60))),_tag_dyneither(_tmp4B4,sizeof(void*),3)))))))))))))))))));}{
# 525
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4B8;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4B7;void*vartype=(void*)((_tmp4B7=_cycalloc(sizeof(*_tmp4B7)),((_tmp4B7[0]=((_tmp4B8.tag=2,((_tmp4B8.f1=tv,_tmp4B8)))),_tmp4B7))));
{struct Cyc_List_List*_tmp4B9;_tmp12F=((_tmp4B9=_region_malloc(rgn,sizeof(*_tmp4B9)),((_tmp4B9->hd=vartype,((_tmp4B9->tl=_tmp12F,_tmp4B9))))));}
# 529
if(k2->kind == Cyc_Absyn_RgnKind){
if(k2->aliasqual == Cyc_Absyn_Aliasable){
struct _tuple0*_tmp4BC;struct Cyc_List_List*_tmp4BB;outlives_constraints=(
(_tmp4BB=_cycalloc(sizeof(*_tmp4BB)),((_tmp4BB->hd=((_tmp4BC=_cycalloc(sizeof(*_tmp4BC)),((_tmp4BC->f1=Cyc_Absyn_empty_effect,((_tmp4BC->f2=vartype,_tmp4BC)))))),((_tmp4BB->tl=outlives_constraints,_tmp4BB))))));}else{
# 534
const char*_tmp4BF;void*_tmp4BE;(_tmp4BE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4BF="opened existential had unique or top region kind",_tag_dyneither(_tmp4BF,sizeof(char),49))),_tag_dyneither(_tmp4BE,sizeof(void*),0)));}}};};}}
# 538
_tmp12F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp12F);{
# 540
struct _RegionHandle _tmp14F=_new_region("r2");struct _RegionHandle*r2=& _tmp14F;_push_region(r2);
{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r2,p->loc,te,_tmpBC);
# 543
struct Cyc_List_List*_tmp150=Cyc_Tcenv_lookup_type_vars(te2);
struct _tuple12 _tmp4C0;struct _tuple12 _tmp151=(_tmp4C0.f1=_tmp150,((_tmp4C0.f2=rgn,_tmp4C0)));
struct Cyc_List_List*_tmp152=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp151,_tmpBA->tvs);
struct Cyc_List_List*_tmp153=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpBA->impl))->exist_vars,_tmp12F);
struct Cyc_List_List*_tmp154=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp152);
struct Cyc_List_List*_tmp155=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp153);
struct Cyc_List_List*_tmp156=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmp152,_tmp153);
# 551
if(_tmpBC != 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpBA->impl))->rgn_po != 0){
if(res.tvars_and_bounds_opt == 0){
struct _tuple1*_tmp4C1;res.tvars_and_bounds_opt=((_tmp4C1=_cycalloc(sizeof(*_tmp4C1)),((_tmp4C1->f1=0,((_tmp4C1->f2=0,_tmp4C1))))));}
(*res.tvars_and_bounds_opt).f1=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f1,((struct Cyc_List_List*(*)(struct _tuple17*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_add_false,_tmpBC));
(*res.tvars_and_bounds_opt).f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,outlives_constraints);{
struct Cyc_List_List*_tmp158=0;
{struct Cyc_List_List*_tmp159=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpBA->impl))->rgn_po;for(0;_tmp159 != 0;_tmp159=_tmp159->tl){
struct _tuple0*_tmp4C4;struct Cyc_List_List*_tmp4C3;_tmp158=((_tmp4C3=_cycalloc(sizeof(*_tmp4C3)),((_tmp4C3->hd=((_tmp4C4=_cycalloc(sizeof(*_tmp4C4)),((_tmp4C4->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp156,(*((struct _tuple0*)_tmp159->hd)).f1),((_tmp4C4->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmp156,(*((struct _tuple0*)_tmp159->hd)).f2),_tmp4C4)))))),((_tmp4C3->tl=_tmp158,_tmp4C3))))));}}
# 563
_tmp158=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp158);
(*res.tvars_and_bounds_opt).f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,_tmp158);};}
# 568
*_tmpBB=_tmp154;
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp4CE;struct Cyc_Absyn_Aggrdecl**_tmp4CD;struct Cyc_Absyn_AggrInfo _tmp4CC;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4CB;t=(void*)((_tmp4CB=_cycalloc(sizeof(*_tmp4CB)),((_tmp4CB[0]=((_tmp4CE.tag=11,((_tmp4CE.f1=((_tmp4CC.aggr_info=Cyc_Absyn_KnownAggr(((_tmp4CD=_cycalloc(sizeof(*_tmp4CD)),((_tmp4CD[0]=_tmpBA,_tmp4CD))))),((_tmp4CC.targs=*_tmpBB,_tmp4CC)))),_tmp4CE)))),_tmp4CB))));}
if(_tmpBE  && _tmpBA->kind == Cyc_Absyn_UnionA){
const char*_tmp4D1;void*_tmp4D0;(_tmp4D0=0,Cyc_Tcutil_warn(p->loc,((_tmp4D1="`...' pattern not allowed in union pattern",_tag_dyneither(_tmp4D1,sizeof(char),43))),_tag_dyneither(_tmp4D0,sizeof(void*),0)));}else{
if(_tmpBE){
# 574
int _tmp162=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp12A);
int _tmp163=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpBA->impl))->fields);
if(_tmp162 < _tmp163){
struct Cyc_List_List*wild_dps=0;
{int i=0;for(0;i < _tmp163 - _tmp162;++ i){
struct _tuple15*_tmp4D4;struct Cyc_List_List*_tmp4D3;wild_dps=((_tmp4D3=_cycalloc(sizeof(*_tmp4D3)),((_tmp4D3->hd=((_tmp4D4=_cycalloc(sizeof(*_tmp4D4)),((_tmp4D4->f1=0,((_tmp4D4->f2=Cyc_Tcpat_wild_pat(p->loc),_tmp4D4)))))),((_tmp4D3->tl=wild_dps,_tmp4D3))))));}}
*_tmpBD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp12A,wild_dps);
_tmp12A=*_tmpBD;}else{
if(_tmp162 == _tmp163){
const char*_tmp4D7;void*_tmp4D6;(_tmp4D6=0,Cyc_Tcutil_warn(p->loc,((_tmp4D7="unnecessary ... in struct pattern",_tag_dyneither(_tmp4D7,sizeof(char),34))),_tag_dyneither(_tmp4D6,sizeof(void*),0)));}}}}{
# 585
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,p->loc,_tmp12A,_tmpBA->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpBA->impl))->fields);
for(0;fields != 0;fields=fields->tl){
struct _tuple19*_tmp168=(struct _tuple19*)fields->hd;struct Cyc_Absyn_Aggrfield*_tmp16A;struct Cyc_Absyn_Pat*_tmp16B;struct _tuple19*_tmp169=_tmp168;_tmp16A=_tmp169->f1;_tmp16B=_tmp169->f2;{
void*_tmp16C=Cyc_Tcutil_rsubstitute(rgn,_tmp156,_tmp16A->type);
# 591
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp)
new_access_exp=Cyc_Absyn_aggrmember_exp(access_exp,_tmp16A->name,0);
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te2,_tmp16B,& _tmp16C,rgn_pat,allow_ref_pat,new_access_exp));
# 599
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp16B->topt),_tmp16C)){
const char*_tmp4DE;void*_tmp4DD[4];struct Cyc_String_pa_PrintArg_struct _tmp4DC;struct Cyc_String_pa_PrintArg_struct _tmp4DB;struct Cyc_String_pa_PrintArg_struct _tmp4DA;struct Cyc_String_pa_PrintArg_struct _tmp4D9;(_tmp4D9.tag=0,((_tmp4D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 602
Cyc_Absynpp_typ2string((void*)_check_null(_tmp16B->topt))),((_tmp4DA.tag=0,((_tmp4DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 601
Cyc_Absynpp_typ2string(_tmp16C)),((_tmp4DB.tag=0,((_tmp4DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp4DC.tag=0,((_tmp4DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp16A->name),((_tmp4DD[0]=& _tmp4DC,((_tmp4DD[1]=& _tmp4DB,((_tmp4DD[2]=& _tmp4DA,((_tmp4DD[3]=& _tmp4D9,Cyc_Tcutil_terr(p->loc,((_tmp4DE="field %s of %s pattern expects type %s != %s",_tag_dyneither(_tmp4DE,sizeof(char),45))),_tag_dyneither(_tmp4DD,sizeof(void*),4)))))))))))))))))))))))));}
# 603
if((unsigned int)new_access_exp)new_access_exp->topt=_tmp16B->topt;};}};}
# 541
;_pop_region(r2);};}
# 607
_npop_handler(0);goto _LL45;
# 492
;_pop_region(rgn);};}_LL66: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmpBF=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpBF->tag != 8)goto _LL68;else{_tmpC0=_tmpBF->f1;_tmpC1=_tmpBF->f2;_tmpC2=(struct Cyc_List_List**)& _tmpBF->f3;_tmpC3=_tmpBF->f4;}}_LL67: {
# 610
struct Cyc_List_List*_tmp176=*_tmpC2;
struct _RegionHandle _tmp177=_new_region("rgn");struct _RegionHandle*rgn=& _tmp177;_push_region(rgn);{
struct Cyc_List_List*tqts=_tmpC1->typs;
# 614
struct Cyc_List_List*_tmp178=Cyc_Tcenv_lookup_type_vars(te);
struct _tuple12 _tmp4DF;struct _tuple12 _tmp179=(_tmp4DF.f1=_tmp178,((_tmp4DF.f2=rgn,_tmp4DF)));
struct Cyc_List_List*_tmp17A=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp179,_tmpC0->tvs);
struct Cyc_List_List*_tmp17B=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp17A);
{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp4E5;struct Cyc_Absyn_DatatypeFieldInfo _tmp4E4;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp4E3;t=(void*)((_tmp4E3=_cycalloc(sizeof(*_tmp4E3)),((_tmp4E3[0]=((_tmp4E5.tag=4,((_tmp4E5.f1=((_tmp4E4.field_info=Cyc_Absyn_KnownDatatypefield(_tmpC0,_tmpC1),((_tmp4E4.targs=_tmp17B,_tmp4E4)))),_tmp4E5)))),_tmp4E3))));}
# 620
if(_tmpC3){
# 622
int _tmp17F=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp176);
int _tmp180=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tqts);
if(_tmp17F < _tmp180){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmp180 - _tmp17F;++ i){
struct Cyc_List_List*_tmp4E6;wild_ps=((_tmp4E6=_cycalloc(sizeof(*_tmp4E6)),((_tmp4E6->hd=Cyc_Tcpat_wild_pat(p->loc),((_tmp4E6->tl=wild_ps,_tmp4E6))))));}}
*_tmpC2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp176,wild_ps);
_tmp176=*_tmpC2;}else{
if(_tmp17F == _tmp180){
const char*_tmp4EA;void*_tmp4E9[1];struct Cyc_String_pa_PrintArg_struct _tmp4E8;(_tmp4E8.tag=0,((_tmp4E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpC0->name)),((_tmp4E9[0]=& _tmp4E8,Cyc_Tcutil_warn(p->loc,((_tmp4EA="unnecessary ... in datatype field %s",_tag_dyneither(_tmp4EA,sizeof(char),37))),_tag_dyneither(_tmp4E9,sizeof(void*),1)))))));}}}
# 634
for(0;_tmp176 != 0  && tqts != 0;(_tmp176=_tmp176->tl,tqts=tqts->tl)){
struct Cyc_Absyn_Pat*_tmp185=(struct Cyc_Absyn_Pat*)_tmp176->hd;
# 638
void*_tmp186=Cyc_Tcutil_rsubstitute(rgn,_tmp17A,(*((struct _tuple18*)tqts->hd)).f2);
# 641
if((unsigned int)access_exp)
Cyc_Tcpat_set_vd(0,access_exp,& res.patvars,Cyc_Absyn_char_typ);
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,_tmp185,& _tmp186,rgn_pat,allow_ref_pat,0));
# 648
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp185->topt),_tmp186)){
const char*_tmp4F0;void*_tmp4EF[3];struct Cyc_String_pa_PrintArg_struct _tmp4EE;struct Cyc_String_pa_PrintArg_struct _tmp4ED;struct Cyc_String_pa_PrintArg_struct _tmp4EC;(_tmp4EC.tag=0,((_tmp4EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 651
Cyc_Absynpp_typ2string((void*)_check_null(_tmp185->topt))),((_tmp4ED.tag=0,((_tmp4ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 650
Cyc_Absynpp_typ2string(_tmp186)),((_tmp4EE.tag=0,((_tmp4EE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpC1->name)),((_tmp4EF[0]=& _tmp4EE,((_tmp4EF[1]=& _tmp4ED,((_tmp4EF[2]=& _tmp4EC,Cyc_Tcutil_terr(_tmp185->loc,((_tmp4F0="%s expects argument type %s, not %s",_tag_dyneither(_tmp4F0,sizeof(char),36))),_tag_dyneither(_tmp4EF,sizeof(void*),3)))))))))))))))))));}}
# 653
if(_tmp176 != 0){
const char*_tmp4F4;void*_tmp4F3[1];struct Cyc_String_pa_PrintArg_struct _tmp4F2;(_tmp4F2.tag=0,((_tmp4F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpC1->name)),((_tmp4F3[0]=& _tmp4F2,Cyc_Tcutil_terr(p->loc,((_tmp4F4="too many arguments for datatype constructor %s",_tag_dyneither(_tmp4F4,sizeof(char),47))),_tag_dyneither(_tmp4F3,sizeof(void*),1)))))));}
if(tqts != 0){
const char*_tmp4F8;void*_tmp4F7[1];struct Cyc_String_pa_PrintArg_struct _tmp4F6;(_tmp4F6.tag=0,((_tmp4F6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpC1->name)),((_tmp4F7[0]=& _tmp4F6,Cyc_Tcutil_terr(p->loc,((_tmp4F8="too few arguments for datatype constructor %s",_tag_dyneither(_tmp4F8,sizeof(char),46))),_tag_dyneither(_tmp4F7,sizeof(void*),1)))))));}}
# 660
_npop_handler(0);goto _LL45;
# 611
;_pop_region(rgn);}_LL68: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpC4=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpC4->tag != 7)goto _LL6A;else{if(_tmpC4->f1 != 0)goto _LL6A;}}_LL69:
# 662
 goto _LL6B;_LL6A: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpC5=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpC5->tag != 7)goto _LL6C;else{if(_tmpC5->f1 == 0)goto _LL6C;if((((_tmpC5->f1)->aggr_info).UnknownAggr).tag != 1)goto _LL6C;}}_LL6B:
 goto _LL6D;_LL6C: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmpC6=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpC6->tag != 15)goto _LL6E;}_LL6D:
 goto _LL6F;_LL6E: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmpC7=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpC7->tag != 17)goto _LL70;}_LL6F:
 goto _LL71;_LL70: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmpC8=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpC8->tag != 16)goto _LL45;}_LL71:
# 667
 t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));goto _LL45;_LL45:;}
# 669
tcpat_end:
 p->topt=t;
return res;};}
# 674
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,struct Cyc_Absyn_Exp*pat_var_exp){
# 676
struct Cyc_Tcpat_TcPatResult _tmp194=Cyc_Tcpat_tcPatRec(te,p,topt,0,0,pat_var_exp);
# 678
struct _RegionHandle _tmp195=_new_region("r");struct _RegionHandle*r=& _tmp195;_push_region(r);{
struct _tuple1 _tmp196=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(r,r,_tmp194.patvars);struct Cyc_List_List*_tmp198;struct _tuple1 _tmp197=_tmp196;_tmp198=_tmp197.f1;{
struct Cyc_List_List*_tmp199=0;
{struct Cyc_List_List*x=_tmp198;for(0;x != 0;x=x->tl){
if((struct Cyc_Absyn_Vardecl**)x->hd != 0){struct Cyc_List_List*_tmp4F9;_tmp199=((_tmp4F9=_region_malloc(r,sizeof(*_tmp4F9)),((_tmp4F9->hd=*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)x->hd)),((_tmp4F9->tl=_tmp199,_tmp4F9))))));}}}
{const char*_tmp4FA;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Tcpat_get_name,_tmp199),p->loc,(
(_tmp4FA="pattern contains a repeated variable",_tag_dyneither(_tmp4FA,sizeof(char),37))));}{
# 688
struct Cyc_List_List*x=_tmp194.patvars;for(0;x != 0;x=x->tl){
struct _tuple16*_tmp19C=(struct _tuple16*)x->hd;struct Cyc_Absyn_Vardecl**_tmp19E;struct Cyc_Absyn_Exp**_tmp19F;struct _tuple16*_tmp19D=_tmp19C;_tmp19E=_tmp19D->f1;_tmp19F=(struct Cyc_Absyn_Exp**)& _tmp19D->f2;
if(*_tmp19F != 0  && *_tmp19F != pat_var_exp)
*_tmp19F=Cyc_Tcutil_deep_copy_exp(1,(struct Cyc_Absyn_Exp*)_check_null(*_tmp19F));}};};}{
# 694
struct Cyc_Tcpat_TcPatResult _tmp1A0=_tmp194;_npop_handler(0);return _tmp1A0;};
# 678
;_pop_region(r);}
# 700
static int Cyc_Tcpat_try_alias_coerce(struct Cyc_Tcenv_Tenv*tenv,void*old_type,void*new_type,struct Cyc_Absyn_Exp*initializer,struct Cyc_List_List*assump){
# 703
struct _tuple0 _tmp4FB;struct _tuple0 _tmp1A1=(_tmp4FB.f1=Cyc_Tcutil_compress(old_type),((_tmp4FB.f2=Cyc_Tcutil_compress(new_type),_tmp4FB)));struct _tuple0 _tmp1A2=_tmp1A1;struct Cyc_Absyn_PtrInfo _tmp1A4;struct Cyc_Absyn_PtrInfo _tmp1A6;_LLA4:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1A3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A2.f1;if(_tmp1A3->tag != 5)goto _LLA6;else{_tmp1A4=_tmp1A3->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1A5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A2.f2;if(_tmp1A5->tag != 5)goto _LLA6;else{_tmp1A6=_tmp1A5->f1;}};_LLA5: {
# 705
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp501;struct Cyc_Absyn_PtrInfo _tmp500;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4FF;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1A7=(_tmp4FF=_cycalloc(sizeof(*_tmp4FF)),((_tmp4FF[0]=((_tmp501.tag=5,((_tmp501.f1=((_tmp500.elt_typ=_tmp1A4.elt_typ,((_tmp500.elt_tq=_tmp1A6.elt_tq,((_tmp500.ptr_atts=(
((_tmp500.ptr_atts).rgn=(_tmp1A4.ptr_atts).rgn,(((_tmp500.ptr_atts).nullable=(_tmp1A6.ptr_atts).nullable,(((_tmp500.ptr_atts).bounds=(_tmp1A6.ptr_atts).bounds,(((_tmp500.ptr_atts).zero_term=(_tmp1A6.ptr_atts).zero_term,(((_tmp500.ptr_atts).ptrloc=(_tmp1A4.ptr_atts).ptrloc,_tmp500.ptr_atts)))))))))),_tmp500)))))),_tmp501)))),_tmp4FF)));
# 711
return Cyc_Tcutil_subtype(tenv,assump,(void*)_tmp1A7,new_type) && 
Cyc_Tcutil_coerce_assign(tenv,initializer,(void*)_tmp1A7);}_LLA6:;_LLA7:
 return 0;_LLA3:;}
# 720
static void Cyc_Tcpat_check_alias_coercion(struct Cyc_Tcenv_Tenv*tenv,unsigned int loc,void*old_type,struct Cyc_Absyn_Tvar*tv,void*new_type,struct Cyc_Absyn_Exp*initializer){
# 723
struct _tuple0*_tmp50B;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp50A;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp509;struct Cyc_List_List*_tmp508;struct Cyc_List_List*assump=(_tmp508=_cycalloc(sizeof(*_tmp508)),((_tmp508->hd=((_tmp50B=_cycalloc(sizeof(*_tmp50B)),((_tmp50B->f1=(void*)& Cyc_Absyn_UniqueRgn_val,((_tmp50B->f2=(void*)((_tmp509=_cycalloc(sizeof(*_tmp509)),((_tmp509[0]=((_tmp50A.tag=2,((_tmp50A.f1=tv,_tmp50A)))),_tmp509)))),_tmp50B)))))),((_tmp508->tl=0,_tmp508)))));
if(Cyc_Tcutil_subtype(tenv,assump,old_type,new_type)){
# 741 "tcpat.cyc"
struct _tuple0 _tmp50C;struct _tuple0 _tmp1AC=(_tmp50C.f1=Cyc_Tcutil_compress(old_type),((_tmp50C.f2=Cyc_Tcutil_compress(new_type),_tmp50C)));struct _tuple0 _tmp1AD=_tmp1AC;struct Cyc_Absyn_PtrInfo _tmp1AF;struct Cyc_Absyn_PtrInfo _tmp1B1;_LLA9:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1AE=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AD.f1;if(_tmp1AE->tag != 5)goto _LLAB;else{_tmp1AF=_tmp1AE->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1B0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AD.f2;if(_tmp1B0->tag != 5)goto _LLAB;else{_tmp1B1=_tmp1B0->f1;}};_LLAA:
 goto _LLA8;_LLAB:;_LLAC:
{const char*_tmp50F;void*_tmp50E;(_tmp50E=0,Cyc_Tcutil_terr(loc,((_tmp50F="alias requires pointer type",_tag_dyneither(_tmp50F,sizeof(char),28))),_tag_dyneither(_tmp50E,sizeof(void*),0)));}goto _LLA8;_LLA8:;}else{
# 746
const char*_tmp514;void*_tmp513[2];struct Cyc_String_pa_PrintArg_struct _tmp512;struct Cyc_String_pa_PrintArg_struct _tmp511;(_tmp511.tag=0,((_tmp511.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(new_type)),((_tmp512.tag=0,((_tmp512.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(old_type)),((_tmp513[0]=& _tmp512,((_tmp513[1]=& _tmp511,Cyc_Tcutil_terr(loc,((_tmp514="cannot alias value of type %s to type %s",_tag_dyneither(_tmp514,sizeof(char),41))),_tag_dyneither(_tmp513,sizeof(void*),2)))))))))))));}}
# 753
void Cyc_Tcpat_check_pat_regions_rec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,int did_noalias_deref,struct Cyc_List_List*patvars){
# 757
void*_tmp1BD=p->r;void*_tmp1BE=_tmp1BD;struct Cyc_Absyn_Pat*_tmp1C0;struct Cyc_Absyn_AggrInfo*_tmp1C2;struct Cyc_List_List*_tmp1C3;struct Cyc_List_List*_tmp1C4;struct Cyc_List_List*_tmp1C6;struct Cyc_List_List*_tmp1C8;struct Cyc_Absyn_Vardecl*_tmp1CA;struct Cyc_Absyn_Pat*_tmp1CB;struct Cyc_Absyn_Vardecl*_tmp1CD;struct Cyc_Absyn_Pat*_tmp1CE;struct Cyc_Absyn_Tvar*_tmp1D0;struct Cyc_Absyn_Vardecl*_tmp1D1;_LLAE: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp1BF=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1BE;if(_tmp1BF->tag != 6)goto _LLB0;else{_tmp1C0=_tmp1BF->f1;}}_LLAF: {
# 759
void*_tmp1D2=(void*)_check_null(p->topt);void*_tmp1D3=_tmp1D2;void*_tmp1D5;_LLBF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1D4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D3;if(_tmp1D4->tag != 5)goto _LLC1;else{_tmp1D5=((_tmp1D4->f1).ptr_atts).rgn;}}_LLC0:
# 761
 Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp1D5);
Cyc_Tcpat_check_pat_regions_rec(te,_tmp1C0,Cyc_Tcutil_is_noalias_region(_tmp1D5,0),patvars);
return;_LLC1:;_LLC2: {
const char*_tmp517;void*_tmp516;(_tmp516=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp517="check_pat_regions: bad pointer type",_tag_dyneither(_tmp517,sizeof(char),36))),_tag_dyneither(_tmp516,sizeof(void*),0)));}_LLBE:;}_LLB0: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp1C1=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1BE;if(_tmp1C1->tag != 7)goto _LLB2;else{_tmp1C2=_tmp1C1->f1;_tmp1C3=_tmp1C1->f2;_tmp1C4=_tmp1C1->f3;}}_LLB1:
# 767
 for(0;_tmp1C4 != 0;_tmp1C4=_tmp1C4->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(*((struct _tuple15*)_tmp1C4->hd)).f2,did_noalias_deref,patvars);}
return;_LLB2: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp1C5=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1BE;if(_tmp1C5->tag != 8)goto _LLB4;else{_tmp1C6=_tmp1C5->f3;}}_LLB3:
# 771
 did_noalias_deref=0;_tmp1C8=_tmp1C6;goto _LLB5;_LLB4: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp1C7=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1BE;if(_tmp1C7->tag != 5)goto _LLB6;else{_tmp1C8=_tmp1C7->f1;}}_LLB5:
# 773
 for(0;_tmp1C8 != 0;_tmp1C8=_tmp1C8->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(struct Cyc_Absyn_Pat*)_tmp1C8->hd,did_noalias_deref,patvars);}
return;_LLB6: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp1C9=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1BE;if(_tmp1C9->tag != 3)goto _LLB8;else{_tmp1CA=_tmp1C9->f1;_tmp1CB=_tmp1C9->f2;}}_LLB7:
# 777
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple16*_tmp1D8=(struct _tuple16*)x->hd;struct Cyc_Absyn_Vardecl**_tmp1DA;struct Cyc_Absyn_Exp*_tmp1DB;struct _tuple16*_tmp1D9=_tmp1D8;_tmp1DA=_tmp1D9->f1;_tmp1DB=_tmp1D9->f2;
# 783
if((_tmp1DA != 0  && *_tmp1DA == _tmp1CA) && _tmp1DB != 0){
{void*_tmp1DC=_tmp1DB->r;void*_tmp1DD=_tmp1DC;struct Cyc_Absyn_Exp*_tmp1DF;_LLC4: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp1DE=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp1DD;if(_tmp1DE->tag != 14)goto _LLC6;else{_tmp1DF=_tmp1DE->f1;}}_LLC5:
# 786
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr(Cyc_Tcenv_get_fnrgn(te),(void*)_check_null(_tmp1DF->topt))){
# 788
const char*_tmp51A;void*_tmp519;(_tmp519=0,Cyc_Tcutil_terr(p->loc,((_tmp51A="reference pattern not allowed on alias-free pointers",_tag_dyneither(_tmp51A,sizeof(char),53))),_tag_dyneither(_tmp519,sizeof(void*),0)));}
goto _LLC3;_LLC6:;_LLC7: {
# 791
const char*_tmp51D;void*_tmp51C;(_tmp51C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp51D="check_pat_regions: bad reference access expression",_tag_dyneither(_tmp51D,sizeof(char),51))),_tag_dyneither(_tmp51C,sizeof(void*),0)));}_LLC3:;}
# 793
break;}}}
# 796
Cyc_Tcpat_check_pat_regions_rec(te,_tmp1CB,did_noalias_deref,patvars);
return;_LLB8: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp1CC=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1BE;if(_tmp1CC->tag != 1)goto _LLBA;else{_tmp1CD=_tmp1CC->f1;_tmp1CE=_tmp1CC->f2;}}_LLB9:
# 799
{void*_tmp1E4=p->topt;void*_tmp1E5=_tmp1E4;_LLC9: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1E6=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1E5;if(_tmp1E6 == 0)goto _LLCB;else{if(_tmp1E6->tag != 8)goto _LLCB;}}_LLCA:
# 801
 if(did_noalias_deref){
{const char*_tmp520;void*_tmp51F;(_tmp51F=0,Cyc_Tcutil_terr(p->loc,((_tmp520="pattern to array would create alias of no-alias pointer",_tag_dyneither(_tmp520,sizeof(char),56))),_tag_dyneither(_tmp51F,sizeof(void*),0)));}
return;}
# 805
goto _LLC8;_LLCB:;_LLCC:
 goto _LLC8;_LLC8:;}
# 808
Cyc_Tcpat_check_pat_regions_rec(te,_tmp1CE,did_noalias_deref,patvars);
return;_LLBA: {struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp1CF=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp1BE;if(_tmp1CF->tag != 2)goto _LLBC;else{_tmp1D0=_tmp1CF->f1;_tmp1D1=_tmp1CF->f2;}}_LLBB:
# 811
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple16*_tmp1E9=(struct _tuple16*)x->hd;struct Cyc_Absyn_Vardecl**_tmp1EB;struct Cyc_Absyn_Exp*_tmp1EC;struct _tuple16*_tmp1EA=_tmp1E9;_tmp1EB=_tmp1EA->f1;_tmp1EC=_tmp1EA->f2;
# 815
if(_tmp1EB != 0  && *_tmp1EB == _tmp1D1){
if(_tmp1EC == 0){
const char*_tmp523;void*_tmp522;(_tmp522=0,Cyc_Tcutil_terr(p->loc,((_tmp523="cannot alias pattern expression in datatype",_tag_dyneither(_tmp523,sizeof(char),44))),_tag_dyneither(_tmp522,sizeof(void*),0)));}else{
# 819
struct _RegionHandle _tmp1EF=_new_region("r");struct _RegionHandle*r=& _tmp1EF;_push_region(r);
{struct Cyc_Absyn_Tvar*_tmp524[1];struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r,p->loc,te,((_tmp524[0]=_tmp1D0,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(r,_tag_dyneither(_tmp524,sizeof(struct Cyc_Absyn_Tvar*),1)))));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp527;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp526;te2=Cyc_Tcenv_add_region(r,te2,(void*)((_tmp526=_cycalloc(sizeof(*_tmp526)),((_tmp526[0]=((_tmp527.tag=2,((_tmp527.f1=_tmp1D0,_tmp527)))),_tmp526)))),0,1);}
# 823
Cyc_Tcpat_check_alias_coercion(te2,p->loc,(void*)_check_null(_tmp1EC->topt),_tmp1D0,_tmp1D1->type,_tmp1EC);}
# 820
;_pop_region(r);}
# 827
break;}}}
# 830
goto _LLAD;_LLBC:;_LLBD:
 return;_LLAD:;}
# 846 "tcpat.cyc"
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_List_List*patvars){
# 848
Cyc_Tcpat_check_pat_regions_rec(te,p,0,patvars);{
struct _RegionHandle _tmp1F3=_new_region("r");struct _RegionHandle*r=& _tmp1F3;_push_region(r);
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple16*_tmp1F4=(struct _tuple16*)x->hd;struct Cyc_Absyn_Vardecl**_tmp1F6;struct Cyc_Absyn_Exp*_tmp1F7;struct _tuple16*_tmp1F5=_tmp1F4;_tmp1F6=_tmp1F5->f1;_tmp1F7=_tmp1F5->f2;
if(_tmp1F7 != 0){
struct Cyc_Absyn_Exp*_tmp1F8=_tmp1F7;
# 856
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)_check_null(_tmp1F8->topt)) && !
Cyc_Tcutil_is_noalias_path(r,_tmp1F8)){
# 859
const char*_tmp534;void*_tmp533[1];const char*_tmp532;struct Cyc_String_pa_PrintArg_struct _tmp531;void*_tmp530[1];const char*_tmp52F;struct Cyc_String_pa_PrintArg_struct _tmp52E;(_tmp52E.tag=0,((_tmp52E.f1=(struct _dyneither_ptr)(
# 861
_tmp1F6 != 0?(struct _dyneither_ptr)(
(_tmp531.tag=0,((_tmp531.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string((*_tmp1F6)->name)),((_tmp530[0]=& _tmp531,Cyc_aprintf(((_tmp52F="for variable %s",_tag_dyneither(_tmp52F,sizeof(char),16))),_tag_dyneither(_tmp530,sizeof(void*),1)))))))):(
(_tmp532="",_tag_dyneither(_tmp532,sizeof(char),1)))),((_tmp533[0]=& _tmp52E,Cyc_Tcutil_terr(p->loc,((_tmp534="pattern %s dereferences a alias-free pointer from a non-unique path",_tag_dyneither(_tmp534,sizeof(char),68))),_tag_dyneither(_tmp533,sizeof(void*),1)))))));}}}}
# 850
;_pop_region(r);};}struct _union_Name_value_Name_v{int tag;struct _dyneither_ptr val;};struct _union_Name_value_Int_v{int tag;int val;};union Cyc_Tcpat_Name_value{struct _union_Name_value_Name_v Name_v;struct _union_Name_value_Int_v Int_v;};
# 907 "tcpat.cyc"
typedef union Cyc_Tcpat_Name_value Cyc_Tcpat_name_value_t;
union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct _dyneither_ptr s){union Cyc_Tcpat_Name_value _tmp535;return((_tmp535.Name_v).val=s,(((_tmp535.Name_v).tag=1,_tmp535)));}
union Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i){union Cyc_Tcpat_Name_value _tmp536;return((_tmp536.Int_v).val=i,(((_tmp536.Int_v).tag=2,_tmp536)));}struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value name;int arity;int*span;struct Cyc_Absyn_Pat*orig_pat;};
# 917
typedef struct Cyc_Tcpat_Con_s*Cyc_Tcpat_con_t;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct{int tag;};struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 922
typedef void*Cyc_Tcpat_simple_pat_t;
# 925
static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){
union Cyc_Tcpat_Name_value _tmp202=c1->name;union Cyc_Tcpat_Name_value _tmp203=_tmp202;struct _dyneither_ptr _tmp204;int _tmp205;_LLCE: if((_tmp203.Name_v).tag != 1)goto _LLD0;_tmp204=(struct _dyneither_ptr)(_tmp203.Name_v).val;_LLCF: {
# 928
union Cyc_Tcpat_Name_value _tmp206=c2->name;union Cyc_Tcpat_Name_value _tmp207=_tmp206;struct _dyneither_ptr _tmp208;_LLD3: if((_tmp207.Name_v).tag != 1)goto _LLD5;_tmp208=(struct _dyneither_ptr)(_tmp207.Name_v).val;_LLD4:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp204,(struct _dyneither_ptr)_tmp208);_LLD5: if((_tmp207.Int_v).tag != 2)goto _LLD2;_LLD6:
 return - 1;_LLD2:;}_LLD0: if((_tmp203.Int_v).tag != 2)goto _LLCD;_tmp205=(int)(_tmp203.Int_v).val;_LLD1: {
# 933
union Cyc_Tcpat_Name_value _tmp209=c2->name;union Cyc_Tcpat_Name_value _tmp20A=_tmp209;int _tmp20B;_LLD8: if((_tmp20A.Name_v).tag != 1)goto _LLDA;_LLD9:
 return 1;_LLDA: if((_tmp20A.Int_v).tag != 2)goto _LLD7;_tmp20B=(int)(_tmp20A.Int_v).val;_LLDB:
 return _tmp205 - _tmp20B;_LLD7:;}_LLCD:;}
# 941
static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(struct _RegionHandle*r){
return((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(r,Cyc_Tcpat_compare_con);}
# 945
static int Cyc_Tcpat_one_opt=1;
static int Cyc_Tcpat_two_opt=2;
static int Cyc_Tcpat_twofiftysix_opt=256;
# 949
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
const char*_tmp539;struct Cyc_Tcpat_Con_s*_tmp538;return(_tmp538=_region_malloc(r,sizeof(*_tmp538)),((_tmp538->name=Cyc_Tcpat_Name_v(((_tmp539="NULL",_tag_dyneither(_tmp539,sizeof(char),5)))),((_tmp538->arity=0,((_tmp538->span=& Cyc_Tcpat_two_opt,((_tmp538->orig_pat=p,_tmp538)))))))));}
# 952
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
const char*_tmp53C;struct Cyc_Tcpat_Con_s*_tmp53B;return(_tmp53B=_region_malloc(r,sizeof(*_tmp53B)),((_tmp53B->name=Cyc_Tcpat_Name_v(((_tmp53C="&",_tag_dyneither(_tmp53C,sizeof(char),2)))),((_tmp53B->arity=1,((_tmp53B->span=& Cyc_Tcpat_two_opt,((_tmp53B->orig_pat=p,_tmp53B)))))))));}
# 955
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
const char*_tmp53F;struct Cyc_Tcpat_Con_s*_tmp53E;return(_tmp53E=_region_malloc(r,sizeof(*_tmp53E)),((_tmp53E->name=Cyc_Tcpat_Name_v(((_tmp53F="&",_tag_dyneither(_tmp53F,sizeof(char),2)))),((_tmp53E->arity=1,((_tmp53E->span=& Cyc_Tcpat_one_opt,((_tmp53E->orig_pat=p,_tmp53E)))))))));}
# 958
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_tmp540;return(_tmp540=_region_malloc(r,sizeof(*_tmp540)),((_tmp540->name=Cyc_Tcpat_Int_v(i),((_tmp540->arity=0,((_tmp540->span=0,((_tmp540->orig_pat=p,_tmp540)))))))));}
# 961
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_tmp541;return(_tmp541=_region_malloc(r,sizeof(*_tmp541)),((_tmp541->name=Cyc_Tcpat_Name_v(f),((_tmp541->arity=0,((_tmp541->span=0,((_tmp541->orig_pat=p,_tmp541)))))))));}
# 964
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_tmp542;return(_tmp542=_region_malloc(r,sizeof(*_tmp542)),((_tmp542->name=Cyc_Tcpat_Int_v((int)c),((_tmp542->arity=0,((_tmp542->span=& Cyc_Tcpat_twofiftysix_opt,((_tmp542->orig_pat=p,_tmp542)))))))));}
# 967
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){
const char*_tmp545;struct Cyc_Tcpat_Con_s*_tmp544;return(_tmp544=_region_malloc(r,sizeof(*_tmp544)),((_tmp544->name=Cyc_Tcpat_Name_v(((_tmp545="$",_tag_dyneither(_tmp545,sizeof(char),2)))),((_tmp544->arity=i,((_tmp544->span=& Cyc_Tcpat_one_opt,((_tmp544->orig_pat=p,_tmp544)))))))));}
# 972
static void*Cyc_Tcpat_null_pat(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp548;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp547;return(void*)((_tmp547=_region_malloc(r,sizeof(*_tmp547)),((_tmp547[0]=((_tmp548.tag=1,((_tmp548.f1=Cyc_Tcpat_null_con(r,p),((_tmp548.f2=0,_tmp548)))))),_tmp547))));}
# 975
static void*Cyc_Tcpat_int_pat(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp54B;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp54A;return(void*)((_tmp54A=_region_malloc(r,sizeof(*_tmp54A)),((_tmp54A[0]=((_tmp54B.tag=1,((_tmp54B.f1=Cyc_Tcpat_int_con(r,i,p),((_tmp54B.f2=0,_tmp54B)))))),_tmp54A))));}
# 978
static void*Cyc_Tcpat_char_pat(struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp54E;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp54D;return(void*)((_tmp54D=_region_malloc(r,sizeof(*_tmp54D)),((_tmp54D[0]=((_tmp54E.tag=1,((_tmp54E.f1=Cyc_Tcpat_char_con(r,c,p),((_tmp54E.f2=0,_tmp54E)))))),_tmp54D))));}
# 981
static void*Cyc_Tcpat_float_pat(struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp551;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp550;return(void*)((_tmp550=_region_malloc(r,sizeof(*_tmp550)),((_tmp550[0]=((_tmp551.tag=1,((_tmp551.f1=Cyc_Tcpat_float_con(r,f,p),((_tmp551.f2=0,_tmp551)))))),_tmp550))));}
# 984
static void*Cyc_Tcpat_null_ptr_pat(struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp557;struct Cyc_List_List*_tmp556;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp555;return(void*)((_tmp555=_region_malloc(r,sizeof(*_tmp555)),((_tmp555[0]=((_tmp557.tag=1,((_tmp557.f1=Cyc_Tcpat_null_ptr_con(r,p0),((_tmp557.f2=((_tmp556=_region_malloc(r,sizeof(*_tmp556)),((_tmp556->hd=p,((_tmp556->tl=0,_tmp556)))))),_tmp557)))))),_tmp555))));}
# 987
static void*Cyc_Tcpat_ptr_pat(struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp55D;struct Cyc_List_List*_tmp55C;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp55B;return(void*)((_tmp55B=_region_malloc(r,sizeof(*_tmp55B)),((_tmp55B[0]=((_tmp55D.tag=1,((_tmp55D.f1=Cyc_Tcpat_ptr_con(r,p0),((_tmp55D.f2=((_tmp55C=_region_malloc(r,sizeof(*_tmp55C)),((_tmp55C->hd=p,((_tmp55C->tl=0,_tmp55C)))))),_tmp55D)))))),_tmp55B))));}
# 990
static void*Cyc_Tcpat_tuple_pat(struct _RegionHandle*r,struct Cyc_List_List*ss,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp560;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp55F;return(void*)((_tmp55F=_region_malloc(r,sizeof(*_tmp55F)),((_tmp55F[0]=((_tmp560.tag=1,((_tmp560.f1=Cyc_Tcpat_tuple_con(r,((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ss),p),((_tmp560.f2=ss,_tmp560)))))),_tmp55F))));}
# 993
static void*Cyc_Tcpat_con_pat(struct _RegionHandle*r,struct _dyneither_ptr con_name,int*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*p){
# 995
struct Cyc_Tcpat_Con_s*_tmp561;struct Cyc_Tcpat_Con_s*c=(_tmp561=_region_malloc(r,sizeof(*_tmp561)),((_tmp561->name=Cyc_Tcpat_Name_v(con_name),((_tmp561->arity=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ps),((_tmp561->span=span,((_tmp561->orig_pat=p,_tmp561)))))))));
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp564;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp563;return(void*)((_tmp563=_region_malloc(r,sizeof(*_tmp563)),((_tmp563[0]=((_tmp564.tag=1,((_tmp564.f1=c,((_tmp564.f2=ps,_tmp564)))))),_tmp563))));}
# 1000
static void*Cyc_Tcpat_compile_pat(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
void*s;
{void*_tmp22A=p->r;void*_tmp22B=_tmp22A;enum Cyc_Absyn_Sign _tmp231;int _tmp232;char _tmp234;struct _dyneither_ptr _tmp236;struct Cyc_Absyn_Pat*_tmp238;struct Cyc_Absyn_Pat*_tmp23A;struct Cyc_Absyn_Pat*_tmp23C;struct Cyc_Absyn_Datatypedecl*_tmp23E;struct Cyc_Absyn_Datatypefield*_tmp23F;struct Cyc_List_List*_tmp240;struct Cyc_List_List*_tmp242;struct Cyc_Absyn_Aggrdecl*_tmp244;struct Cyc_List_List*_tmp245;struct Cyc_Absyn_Enumdecl*_tmp247;struct Cyc_Absyn_Enumfield*_tmp248;void*_tmp24A;struct Cyc_Absyn_Enumfield*_tmp24B;_LLDD: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp22C=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp22B;if(_tmp22C->tag != 0)goto _LLDF;}_LLDE:
 goto _LLE0;_LLDF: {struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp22D=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp22B;if(_tmp22D->tag != 2)goto _LLE1;}_LLE0:
 goto _LLE2;_LLE1: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp22E=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp22B;if(_tmp22E->tag != 4)goto _LLE3;}_LLE2:
{struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp567;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp566;s=(void*)((_tmp566=_region_malloc(r,sizeof(*_tmp566)),((_tmp566[0]=((_tmp567.tag=0,_tmp567)),_tmp566))));}goto _LLDC;_LLE3: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp22F=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp22B;if(_tmp22F->tag != 9)goto _LLE5;}_LLE4:
 s=Cyc_Tcpat_null_pat(r,p);goto _LLDC;_LLE5: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp230=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp22B;if(_tmp230->tag != 10)goto _LLE7;else{_tmp231=_tmp230->f1;_tmp232=_tmp230->f2;}}_LLE6:
 s=Cyc_Tcpat_int_pat(r,_tmp232,p);goto _LLDC;_LLE7: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp233=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp22B;if(_tmp233->tag != 11)goto _LLE9;else{_tmp234=_tmp233->f1;}}_LLE8:
 s=Cyc_Tcpat_char_pat(r,_tmp234,p);goto _LLDC;_LLE9: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp235=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp22B;if(_tmp235->tag != 12)goto _LLEB;else{_tmp236=_tmp235->f1;}}_LLEA:
 s=Cyc_Tcpat_float_pat(r,_tmp236,p);goto _LLDC;_LLEB: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp237=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp22B;if(_tmp237->tag != 1)goto _LLED;else{_tmp238=_tmp237->f2;}}_LLEC:
 s=Cyc_Tcpat_compile_pat(r,_tmp238);goto _LLDC;_LLED: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp239=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp22B;if(_tmp239->tag != 3)goto _LLEF;else{_tmp23A=_tmp239->f2;}}_LLEE:
 s=Cyc_Tcpat_compile_pat(r,_tmp23A);goto _LLDC;_LLEF: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp23B=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp22B;if(_tmp23B->tag != 6)goto _LLF1;else{_tmp23C=_tmp23B->f1;}}_LLF0:
# 1013
{void*_tmp252=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp253=_tmp252;union Cyc_Absyn_Constraint*_tmp255;_LL104: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp254=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp253;if(_tmp254->tag != 5)goto _LL106;else{_tmp255=((_tmp254->f1).ptr_atts).nullable;}}_LL105: {
# 1015
int is_nullable=0;
int still_working=1;
while(still_working){
union Cyc_Absyn_Constraint*_tmp256=_tmp255;union Cyc_Absyn_Constraint*_tmp257;int _tmp258;_LL109: if((_tmp256->Forward_constr).tag != 2)goto _LL10B;_tmp257=(union Cyc_Absyn_Constraint*)(_tmp256->Forward_constr).val;_LL10A:
# 1020
*_tmp255=*_tmp257;
continue;_LL10B: if((_tmp256->No_constr).tag != 3)goto _LL10D;_LL10C:
# 1023
{struct _union_Constraint_Eq_constr*_tmp568;(_tmp568=& _tmp255->Eq_constr,((_tmp568->tag=1,_tmp568->val=0)));}
is_nullable=0;
still_working=0;
goto _LL108;_LL10D: if((_tmp256->Eq_constr).tag != 1)goto _LL108;_tmp258=(int)(_tmp256->Eq_constr).val;_LL10E:
# 1028
 is_nullable=_tmp258;
still_working=0;
goto _LL108;_LL108:;}{
# 1033
void*ss=Cyc_Tcpat_compile_pat(r,_tmp23C);
if(is_nullable)
s=Cyc_Tcpat_null_ptr_pat(r,ss,p);else{
# 1037
s=Cyc_Tcpat_ptr_pat(r,ss,p);}
goto _LL103;};}_LL106:;_LL107: {
const char*_tmp56B;void*_tmp56A;(_tmp56A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp56B="expecting pointertype for pattern!",_tag_dyneither(_tmp56B,sizeof(char),35))),_tag_dyneither(_tmp56A,sizeof(void*),0)));}_LL103:;}
# 1041
goto _LLDC;_LLF1: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp23D=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp22B;if(_tmp23D->tag != 8)goto _LLF3;else{_tmp23E=_tmp23D->f1;_tmp23F=_tmp23D->f2;_tmp240=_tmp23D->f3;}}_LLF2: {
# 1043
int*span;
{void*_tmp25C=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp25D=_tmp25C;_LL110: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp25E=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp25D;if(_tmp25E->tag != 3)goto _LL112;}_LL111:
# 1046
 if(_tmp23E->is_extensible)
span=0;else{
# 1049
int*_tmp56C;span=((_tmp56C=_region_malloc(r,sizeof(*_tmp56C)),((_tmp56C[0]=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp23E->fields))->v),_tmp56C))));}
goto _LL10F;_LL112: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp25F=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp25D;if(_tmp25F->tag != 4)goto _LL114;}_LL113:
 span=& Cyc_Tcpat_one_opt;goto _LL10F;_LL114:;_LL115:
{const char*_tmp56F;void*_tmp56E;span=((_tmp56E=0,((int*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp56F="void datatype pattern has bad type",_tag_dyneither(_tmp56F,sizeof(char),35))),_tag_dyneither(_tmp56E,sizeof(void*),0))));}goto _LL10F;_LL10F:;}
# 1054
s=Cyc_Tcpat_con_pat(r,*(*_tmp23F->name).f2,span,((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp240),p);
goto _LLDC;}_LLF3: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp241=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp22B;if(_tmp241->tag != 5)goto _LLF5;else{_tmp242=_tmp241->f1;}}_LLF4:
# 1058
 s=Cyc_Tcpat_tuple_pat(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp242),p);goto _LLDC;_LLF5: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp243=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp22B;if(_tmp243->tag != 7)goto _LLF7;else{if(_tmp243->f1 == 0)goto _LLF7;if((((_tmp243->f1)->aggr_info).KnownAggr).tag != 2)goto _LLF7;_tmp244=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp243->f1)->aggr_info).KnownAggr).val);_tmp245=_tmp243->f3;}}_LLF6:
# 1063
 if(_tmp244->kind == Cyc_Absyn_StructA){
struct Cyc_List_List*ps=0;
{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp244->impl))->fields;for(0;fields != 0;fields=fields->tl){
# 1067
const char*_tmp570;int found=Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,((_tmp570="",_tag_dyneither(_tmp570,sizeof(char),1))))== 0;
{struct Cyc_List_List*dlps0=_tmp245;for(0;!found  && dlps0 != 0;dlps0=dlps0->tl){
struct _tuple15*_tmp263=(struct _tuple15*)dlps0->hd;struct Cyc_List_List*_tmp265;struct Cyc_Absyn_Pat*_tmp266;struct _tuple15*_tmp264=_tmp263;_tmp265=_tmp264->f1;_tmp266=_tmp264->f2;{
struct Cyc_List_List*_tmp267=_tmp265;struct _dyneither_ptr*_tmp269;_LL117: if(_tmp267 == 0)goto _LL119;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp268=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((void*)_tmp267->hd);if(_tmp268->tag != 1)goto _LL119;else{_tmp269=_tmp268->f1;}}if(_tmp267->tl != 0)goto _LL119;_LL118:
# 1072
 if(Cyc_strptrcmp(_tmp269,((struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){
{struct Cyc_List_List*_tmp571;ps=((_tmp571=_region_malloc(r,sizeof(*_tmp571)),((_tmp571->hd=Cyc_Tcpat_compile_pat(r,_tmp266),((_tmp571->tl=ps,_tmp571))))));}
found=1;}
# 1076
goto _LL116;_LL119:;_LL11A: {
const char*_tmp574;void*_tmp573;(_tmp573=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp574="bad designator(s)",_tag_dyneither(_tmp574,sizeof(char),18))),_tag_dyneither(_tmp573,sizeof(void*),0)));}_LL116:;};}}
# 1080
if(!found){
const char*_tmp577;void*_tmp576;(_tmp576=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp577="bad designator",_tag_dyneither(_tmp577,sizeof(char),15))),_tag_dyneither(_tmp576,sizeof(void*),0)));}}}
# 1083
s=Cyc_Tcpat_tuple_pat(r,ps,p);}else{
# 1086
if(!((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp244->impl))->tagged){
const char*_tmp57A;void*_tmp579;(_tmp579=0,Cyc_Tcutil_terr(p->loc,((_tmp57A="patterns on untagged unions not yet supported.",_tag_dyneither(_tmp57A,sizeof(char),47))),_tag_dyneither(_tmp579,sizeof(void*),0)));}{
int*_tmp57B;int*span=(_tmp57B=_region_malloc(r,sizeof(*_tmp57B)),((_tmp57B[0]=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp244->impl))->fields),_tmp57B)));
struct Cyc_List_List*_tmp272=_tmp245;struct _dyneither_ptr*_tmp274;struct Cyc_Absyn_Pat*_tmp275;_LL11C: if(_tmp272 == 0)goto _LL11E;if(((struct _tuple15*)_tmp272->hd)->f1 == 0)goto _LL11E;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp273=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((void*)(((struct _tuple15*)_tmp272->hd)->f1)->hd);if(_tmp273->tag != 1)goto _LL11E;else{_tmp274=_tmp273->f1;}}if((((struct _tuple15*)_tmp272->hd)->f1)->tl != 0)goto _LL11E;_tmp275=((struct _tuple15*)_tmp272->hd)->f2;if(_tmp272->tl != 0)goto _LL11E;_LL11D:
# 1091
{struct Cyc_List_List*_tmp57C;s=Cyc_Tcpat_con_pat(r,*_tmp274,span,((_tmp57C=_region_malloc(r,sizeof(*_tmp57C)),((_tmp57C->hd=Cyc_Tcpat_compile_pat(r,_tmp275),((_tmp57C->tl=0,_tmp57C)))))),p);}
goto _LL11B;_LL11E:;_LL11F: {
const char*_tmp57F;void*_tmp57E;(_tmp57E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp57F="bad union pattern",_tag_dyneither(_tmp57F,sizeof(char),18))),_tag_dyneither(_tmp57E,sizeof(void*),0)));}_LL11B:;};}
# 1096
goto _LLDC;_LLF7: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp246=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp22B;if(_tmp246->tag != 13)goto _LLF9;else{_tmp247=_tmp246->f1;_tmp248=_tmp246->f2;}}_LLF8:
# 1101
{void*_tmp27A=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp27B=_tmp27A;_LL121: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp27C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp27B;if(_tmp27C->tag != 6)goto _LL123;}_LL122:
# 1105
 s=Cyc_Tcpat_con_pat(r,*(*_tmp248->name).f2,0,0,p);
goto _LL120;_LL123:;_LL124: {
# 1108
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp247->fields))->v);
{int*_tmp580;s=Cyc_Tcpat_con_pat(r,*(*_tmp248->name).f2,((_tmp580=_region_malloc(r,sizeof(*_tmp580)),((_tmp580[0]=span,_tmp580)))),0,p);}
goto _LL120;}_LL120:;}
# 1112
goto _LLDC;_LLF9: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp249=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp22B;if(_tmp249->tag != 14)goto _LLFB;else{_tmp24A=(void*)_tmp249->f1;_tmp24B=_tmp249->f2;}}_LLFA: {
# 1117
struct Cyc_List_List*fields;
{void*_tmp27E=Cyc_Tcutil_compress(_tmp24A);void*_tmp27F=_tmp27E;struct Cyc_List_List*_tmp281;_LL126: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp280=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp27F;if(_tmp280->tag != 14)goto _LL128;else{_tmp281=_tmp280->f1;}}_LL127:
 fields=_tmp281;goto _LL125;_LL128:;_LL129: {
const char*_tmp583;void*_tmp582;(_tmp582=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp583="bad type in AnonEnum_p",_tag_dyneither(_tmp583,sizeof(char),23))),_tag_dyneither(_tmp582,sizeof(void*),0)));}_LL125:;}
# 1124
{void*_tmp284=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp285=_tmp284;_LL12B: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp286=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp285;if(_tmp286->tag != 6)goto _LL12D;}_LL12C:
# 1128
 s=Cyc_Tcpat_con_pat(r,*(*_tmp24B->name).f2,0,0,p);
goto _LL12A;_LL12D:;_LL12E: {
# 1131
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);
{int*_tmp584;s=Cyc_Tcpat_con_pat(r,*(*_tmp24B->name).f2,((_tmp584=_region_malloc(r,sizeof(*_tmp584)),((_tmp584[0]=span,_tmp584)))),0,p);}
goto _LL12A;}_LL12A:;}
# 1135
goto _LLDC;}_LLFB: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp24C=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp22B;if(_tmp24C->tag != 15)goto _LLFD;}_LLFC:
 goto _LLFE;_LLFD: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp24D=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp22B;if(_tmp24D->tag != 16)goto _LLFF;}_LLFE:
 goto _LL100;_LLFF: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp24E=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp22B;if(_tmp24E->tag != 7)goto _LL101;}_LL100:
 goto _LL102;_LL101: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp24F=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp22B;if(_tmp24F->tag != 17)goto _LLDC;}_LL102: {
struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp587;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp586;s=(void*)((_tmp586=_region_malloc(r,sizeof(*_tmp586)),((_tmp586[0]=((_tmp587.tag=0,_tmp587)),_tmp586))));}_LLDC:;}
# 1141
return s;}
# 1149
typedef struct Cyc_List_List*Cyc_Tcpat_match_t;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct{int tag;struct Cyc_Set_Set*f1;};
# 1160
typedef void*Cyc_Tcpat_term_desc_t;
# 1164
typedef struct Cyc_List_List*Cyc_Tcpat_context_t;
# 1171
typedef struct Cyc_List_List*Cyc_Tcpat_access_t;struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*f2;void*f3;void*f4;};
# 1180
typedef void*Cyc_Tcpat_decision_t;struct _tuple20{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 1185
typedef struct _tuple20*Cyc_Tcpat_work_stack_frame_t;
# 1188
typedef struct Cyc_List_List*Cyc_Tcpat_work_stack_t;
# 1190
enum Cyc_Tcpat_Answer{Cyc_Tcpat_Yes  = 0,Cyc_Tcpat_No  = 1,Cyc_Tcpat_Maybe  = 2};
typedef enum Cyc_Tcpat_Answer Cyc_Tcpat_answer_t;
# 1193
static void Cyc_Tcpat_print_tab(int i){
for(0;i != 0;-- i){
const char*_tmp58A;void*_tmp589;(_tmp589=0,Cyc_fprintf(Cyc_stderr,((_tmp58A=" ",_tag_dyneither(_tmp58A,sizeof(char),2))),_tag_dyneither(_tmp589,sizeof(void*),0)));}}
# 1199
static void Cyc_Tcpat_print_con(struct Cyc_Tcpat_Con_s*c){
union Cyc_Tcpat_Name_value _tmp28C=c->name;
union Cyc_Tcpat_Name_value _tmp28D=_tmp28C;struct _dyneither_ptr _tmp28E;int _tmp28F;_LL130: if((_tmp28D.Name_v).tag != 1)goto _LL132;_tmp28E=(struct _dyneither_ptr)(_tmp28D.Name_v).val;_LL131:
{const char*_tmp58E;void*_tmp58D[1];struct Cyc_String_pa_PrintArg_struct _tmp58C;(_tmp58C.tag=0,((_tmp58C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp28E),((_tmp58D[0]=& _tmp58C,Cyc_fprintf(Cyc_stderr,((_tmp58E="%s",_tag_dyneither(_tmp58E,sizeof(char),3))),_tag_dyneither(_tmp58D,sizeof(void*),1)))))));}goto _LL12F;_LL132: if((_tmp28D.Int_v).tag != 2)goto _LL12F;_tmp28F=(int)(_tmp28D.Int_v).val;_LL133:
{const char*_tmp592;void*_tmp591[1];struct Cyc_Int_pa_PrintArg_struct _tmp590;(_tmp590.tag=1,((_tmp590.f1=(unsigned long)_tmp28F,((_tmp591[0]=& _tmp590,Cyc_fprintf(Cyc_stderr,((_tmp592="%d",_tag_dyneither(_tmp592,sizeof(char),3))),_tag_dyneither(_tmp591,sizeof(void*),1)))))));}goto _LL12F;_LL12F:;}
# 1207
static void Cyc_Tcpat_print_dec_tree(void*d,void(*print_rhs)(void*),int tab){
void*_tmp296=d;void*_tmp298;struct Cyc_List_List*_tmp29B;struct Cyc_Tcpat_Con_s*_tmp29C;void*_tmp29D;void*_tmp29E;_LL135: {struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp297=(struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp296;if(_tmp297->tag != 1)goto _LL137;else{_tmp298=(void*)_tmp297->f1;}}_LL136:
# 1210
 Cyc_Tcpat_print_tab(tab);
{const char*_tmp595;void*_tmp594;(_tmp594=0,Cyc_fprintf(Cyc_stderr,((_tmp595="Success(",_tag_dyneither(_tmp595,sizeof(char),9))),_tag_dyneither(_tmp594,sizeof(void*),0)));}print_rhs(_tmp298);{const char*_tmp598;void*_tmp597;(_tmp597=0,Cyc_fprintf(Cyc_stderr,((_tmp598=")\n",_tag_dyneither(_tmp598,sizeof(char),3))),_tag_dyneither(_tmp597,sizeof(void*),0)));}
goto _LL134;_LL137: {struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp299=(struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp296;if(_tmp299->tag != 0)goto _LL139;}_LL138:
{const char*_tmp59B;void*_tmp59A;(_tmp59A=0,Cyc_fprintf(Cyc_stderr,((_tmp59B="Failure\n",_tag_dyneither(_tmp59B,sizeof(char),9))),_tag_dyneither(_tmp59A,sizeof(void*),0)));}goto _LL134;_LL139: {struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*_tmp29A=(struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*)_tmp296;if(_tmp29A->tag != 2)goto _LL134;else{_tmp29B=_tmp29A->f1;_tmp29C=_tmp29A->f2;_tmp29D=(void*)_tmp29A->f3;_tmp29E=(void*)_tmp29A->f4;}}_LL13A:
# 1215
 Cyc_Tcpat_print_tab(tab);
{const char*_tmp59E;void*_tmp59D;(_tmp59D=0,Cyc_fprintf(Cyc_stderr,((_tmp59E="Access[",_tag_dyneither(_tmp59E,sizeof(char),8))),_tag_dyneither(_tmp59D,sizeof(void*),0)));}
for(0;_tmp29B != 0;_tmp29B=_tmp29B->tl){
{const char*_tmp5A2;void*_tmp5A1[1];struct Cyc_Int_pa_PrintArg_struct _tmp5A0;(_tmp5A0.tag=1,((_tmp5A0.f1=(unsigned long)((int)_tmp29B->hd),((_tmp5A1[0]=& _tmp5A0,Cyc_fprintf(Cyc_stderr,((_tmp5A2="%d",_tag_dyneither(_tmp5A2,sizeof(char),3))),_tag_dyneither(_tmp5A1,sizeof(void*),1)))))));}
if(_tmp29B->tl != 0){const char*_tmp5A5;void*_tmp5A4;(_tmp5A4=0,Cyc_fprintf(Cyc_stderr,((_tmp5A5=",",_tag_dyneither(_tmp5A5,sizeof(char),2))),_tag_dyneither(_tmp5A4,sizeof(void*),0)));}}
# 1221
{const char*_tmp5A8;void*_tmp5A7;(_tmp5A7=0,Cyc_fprintf(Cyc_stderr,((_tmp5A8="],",_tag_dyneither(_tmp5A8,sizeof(char),3))),_tag_dyneither(_tmp5A7,sizeof(void*),0)));}
Cyc_Tcpat_print_con(_tmp29C);
{const char*_tmp5AB;void*_tmp5AA;(_tmp5AA=0,Cyc_fprintf(Cyc_stderr,((_tmp5AB=",\n",_tag_dyneither(_tmp5AB,sizeof(char),3))),_tag_dyneither(_tmp5AA,sizeof(void*),0)));}
Cyc_Tcpat_print_dec_tree(_tmp29D,print_rhs,tab + 7);
Cyc_Tcpat_print_dec_tree(_tmp29E,print_rhs,tab + 7);_LL134:;}struct _tuple21{int f1;unsigned int f2;};
# 1229
static void Cyc_Tcpat_print_swrhs(struct _tuple21*x){
return;}
# 1237
static void*Cyc_Tcpat_add_neg(struct _RegionHandle*r,void*td,struct Cyc_Tcpat_Con_s*c){
void*_tmp2B0=td;struct Cyc_Set_Set*_tmp2B2;_LL13C: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2B1=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2B0;if(_tmp2B1->tag != 1)goto _LL13E;else{_tmp2B2=_tmp2B1->f1;}}_LL13D: {
# 1247
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp5AE;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp5AD;return(void*)((_tmp5AD=_region_malloc(r,sizeof(*_tmp5AD)),((_tmp5AD[0]=((_tmp5AE.tag=1,((_tmp5AE.f1=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_rinsert)(r,_tmp2B2,c),_tmp5AE)))),_tmp5AD))));}_LL13E: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2B3=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2B0;if(_tmp2B3->tag != 0)goto _LL13B;}_LL13F: {
const char*_tmp5B1;void*_tmp5B0;(_tmp5B0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5B1="add_neg called when td is Positive",_tag_dyneither(_tmp5B1,sizeof(char),35))),_tag_dyneither(_tmp5B0,sizeof(void*),0)));}_LL13B:;}
# 1254
static enum Cyc_Tcpat_Answer Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*td){
void*_tmp2B8=td;struct Cyc_Tcpat_Con_s*_tmp2BA;struct Cyc_Set_Set*_tmp2BC;_LL141: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2B9=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2B8;if(_tmp2B9->tag != 0)goto _LL143;else{_tmp2BA=_tmp2B9->f1;}}_LL142:
# 1258
 if(Cyc_Tcpat_compare_con(c,_tmp2BA)== 0)return Cyc_Tcpat_Yes;else{
return Cyc_Tcpat_No;}_LL143: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2BB=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2B8;if(_tmp2BB->tag != 1)goto _LL140;else{_tmp2BC=_tmp2BB->f1;}}_LL144:
# 1262
 if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2BC,c))return Cyc_Tcpat_No;else{
# 1265
if(c->span != 0  && *((int*)_check_null(c->span))== ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp2BC)+ 1)
return Cyc_Tcpat_Yes;else{
# 1268
return Cyc_Tcpat_Maybe;}}_LL140:;}struct _tuple22{struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 1276
static struct Cyc_List_List*Cyc_Tcpat_augment(struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc){
# 1278
struct Cyc_List_List*_tmp2BD=ctxt;struct Cyc_Tcpat_Con_s*_tmp2BE;struct Cyc_List_List*_tmp2BF;struct Cyc_List_List*_tmp2C0;_LL146: if(_tmp2BD != 0)goto _LL148;_LL147:
 return 0;_LL148: if(_tmp2BD == 0)goto _LL145;_tmp2BE=((struct _tuple22*)_tmp2BD->hd)->f1;_tmp2BF=((struct _tuple22*)_tmp2BD->hd)->f2;_tmp2C0=_tmp2BD->tl;_LL149: {
# 1281
struct _tuple22*_tmp5B7;struct Cyc_List_List*_tmp5B6;struct Cyc_List_List*_tmp5B5;return(_tmp5B5=_region_malloc(r,sizeof(*_tmp5B5)),((_tmp5B5->hd=(
(_tmp5B7=_region_malloc(r,sizeof(*_tmp5B7)),((_tmp5B7->f1=_tmp2BE,((_tmp5B7->f2=((_tmp5B6=_region_malloc(r,sizeof(*_tmp5B6)),((_tmp5B6->hd=dsc,((_tmp5B6->tl=_tmp2BF,_tmp5B6)))))),_tmp5B7)))))),((_tmp5B5->tl=_tmp2C0,_tmp5B5)))));}_LL145:;}
# 1289
static struct Cyc_List_List*Cyc_Tcpat_norm_context(struct _RegionHandle*r,struct Cyc_List_List*ctxt){
struct Cyc_List_List*_tmp2C4=ctxt;struct Cyc_Tcpat_Con_s*_tmp2C5;struct Cyc_List_List*_tmp2C6;struct Cyc_List_List*_tmp2C7;_LL14B: if(_tmp2C4 != 0)goto _LL14D;_LL14C: {
const char*_tmp5BA;void*_tmp5B9;(_tmp5B9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5BA="norm_context: empty context",_tag_dyneither(_tmp5BA,sizeof(char),28))),_tag_dyneither(_tmp5B9,sizeof(void*),0)));}_LL14D: if(_tmp2C4 == 0)goto _LL14A;_tmp2C5=((struct _tuple22*)_tmp2C4->hd)->f1;_tmp2C6=((struct _tuple22*)_tmp2C4->hd)->f2;_tmp2C7=_tmp2C4->tl;_LL14E: {
# 1293
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp5BD;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp5BC;return Cyc_Tcpat_augment(r,_tmp2C7,(void*)((_tmp5BC=_region_malloc(r,sizeof(*_tmp5BC)),((_tmp5BC[0]=((_tmp5BD.tag=0,((_tmp5BD.f1=_tmp2C5,((_tmp5BD.f2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,_tmp2C6),_tmp5BD)))))),_tmp5BC)))));}_LL14A:;}
# 1302
static void*Cyc_Tcpat_build_desc(struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
# 1304
struct _tuple1 _tmp5BE;struct _tuple1 _tmp2CC=(_tmp5BE.f1=ctxt,((_tmp5BE.f2=work,_tmp5BE)));struct _tuple1 _tmp2CD=_tmp2CC;struct Cyc_Tcpat_Con_s*_tmp2CE;struct Cyc_List_List*_tmp2CF;struct Cyc_List_List*_tmp2D0;struct Cyc_List_List*_tmp2D1;struct Cyc_List_List*_tmp2D2;_LL150: if(_tmp2CD.f1 != 0)goto _LL152;if(_tmp2CD.f2 != 0)goto _LL152;_LL151:
 return dsc;_LL152: if(_tmp2CD.f1 != 0)goto _LL154;_LL153:
 goto _LL155;_LL154: if(_tmp2CD.f2 != 0)goto _LL156;_LL155: {
const char*_tmp5C1;void*_tmp5C0;(_tmp5C0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5C1="build_desc: ctxt and work don't match",_tag_dyneither(_tmp5C1,sizeof(char),38))),_tag_dyneither(_tmp5C0,sizeof(void*),0)));}_LL156: if(_tmp2CD.f1 == 0)goto _LL14F;_tmp2CE=((struct _tuple22*)(_tmp2CD.f1)->hd)->f1;_tmp2CF=((struct _tuple22*)(_tmp2CD.f1)->hd)->f2;_tmp2D0=(_tmp2CD.f1)->tl;if(_tmp2CD.f2 == 0)goto _LL14F;_tmp2D1=((struct _tuple20*)(_tmp2CD.f2)->hd)->f3;_tmp2D2=(_tmp2CD.f2)->tl;_LL157: {
# 1309
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp5C7;struct Cyc_List_List*_tmp5C6;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp5C5;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2D5=(_tmp5C5=_region_malloc(r,sizeof(*_tmp5C5)),((_tmp5C5[0]=((_tmp5C7.tag=0,((_tmp5C7.f1=_tmp2CE,((_tmp5C7.f2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,_tmp2CF),(
(_tmp5C6=_region_malloc(r,sizeof(*_tmp5C6)),((_tmp5C6->hd=dsc,((_tmp5C6->tl=_tmp2D1,_tmp5C6))))))),_tmp5C7)))))),_tmp5C5)));
return Cyc_Tcpat_build_desc(r,_tmp2D0,(void*)_tmp2D5,_tmp2D2);}_LL14F:;}
# 1315
static void*Cyc_Tcpat_match(struct _RegionHandle*r,void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules);
# 1322
static void*Cyc_Tcpat_or_match(struct _RegionHandle*r,void*dsc,struct Cyc_List_List*allmrules){
# 1324
struct Cyc_List_List*_tmp2DA=allmrules;void*_tmp2DB;void*_tmp2DC;struct Cyc_List_List*_tmp2DD;_LL159: if(_tmp2DA != 0)goto _LL15B;_LL15A: {
struct Cyc_Tcpat_Failure_Tcpat_Decision_struct _tmp5CA;struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp5C9;return(void*)((_tmp5C9=_cycalloc(sizeof(*_tmp5C9)),((_tmp5C9[0]=((_tmp5CA.tag=0,((_tmp5CA.f1=dsc,_tmp5CA)))),_tmp5C9))));}_LL15B: if(_tmp2DA == 0)goto _LL158;_tmp2DB=((struct _tuple0*)_tmp2DA->hd)->f1;_tmp2DC=((struct _tuple0*)_tmp2DA->hd)->f2;_tmp2DD=_tmp2DA->tl;_LL15C:
# 1327
 return Cyc_Tcpat_match(r,_tmp2DB,0,dsc,0,0,_tmp2DC,_tmp2DD);_LL158:;}
# 1332
static void*Cyc_Tcpat_match_compile(struct _RegionHandle*r,struct Cyc_List_List*allmrules){
# 1334
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp5CD;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp5CC;return Cyc_Tcpat_or_match(r,(void*)((_tmp5CC=_region_malloc(r,sizeof(*_tmp5CC)),((_tmp5CC[0]=((_tmp5CD.tag=1,((_tmp5CD.f1=Cyc_Tcpat_empty_con_set(r),_tmp5CD)))),_tmp5CC)))),allmrules);}
# 1340
static void*Cyc_Tcpat_and_match(struct _RegionHandle*r,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules){
# 1344
struct Cyc_List_List*_tmp2E2=work;struct Cyc_List_List*_tmp2E3;struct Cyc_List_List*_tmp2E4;struct Cyc_List_List*_tmp2E5;struct Cyc_List_List*_tmp2E6;struct Cyc_List_List*_tmp2E7;_LL15E: if(_tmp2E2 != 0)goto _LL160;_LL15F: {
struct Cyc_Tcpat_Success_Tcpat_Decision_struct _tmp5D0;struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp5CF;return(void*)((_tmp5CF=_region_malloc(r,sizeof(*_tmp5CF)),((_tmp5CF[0]=((_tmp5D0.tag=1,((_tmp5D0.f1=right_hand_side,_tmp5D0)))),_tmp5CF))));}_LL160: if(_tmp2E2 == 0)goto _LL162;if(((struct _tuple20*)_tmp2E2->hd)->f1 != 0)goto _LL162;if(((struct _tuple20*)_tmp2E2->hd)->f2 != 0)goto _LL162;if(((struct _tuple20*)_tmp2E2->hd)->f3 != 0)goto _LL162;_tmp2E3=_tmp2E2->tl;_LL161:
# 1347
 return Cyc_Tcpat_and_match(r,Cyc_Tcpat_norm_context(r,ctx),_tmp2E3,right_hand_side,rules);_LL162: if(_tmp2E2 == 0)goto _LL15D;_tmp2E4=((struct _tuple20*)_tmp2E2->hd)->f1;_tmp2E5=((struct _tuple20*)_tmp2E2->hd)->f2;_tmp2E6=((struct _tuple20*)_tmp2E2->hd)->f3;_tmp2E7=_tmp2E2->tl;_LL163:
# 1349
 if((_tmp2E4 == 0  || _tmp2E5 == 0) || _tmp2E6 == 0){
const char*_tmp5D3;void*_tmp5D2;(_tmp5D2=0,Cyc_Tcutil_impos(((_tmp5D3="tcpat:and_match: malformed work frame",_tag_dyneither(_tmp5D3,sizeof(char),38))),_tag_dyneither(_tmp5D2,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp2EC=_tmp2E4;void*_tmp2EE;struct Cyc_List_List*_tmp2EF;struct Cyc_List_List*_tmp2ED=_tmp2EC;_tmp2EE=(void*)_tmp2ED->hd;_tmp2EF=_tmp2ED->tl;{
struct Cyc_List_List*_tmp2F0=_tmp2E5;struct Cyc_List_List*_tmp2F2;struct Cyc_List_List*_tmp2F3;struct Cyc_List_List*_tmp2F1=_tmp2F0;_tmp2F2=(struct Cyc_List_List*)_tmp2F1->hd;_tmp2F3=_tmp2F1->tl;{
struct Cyc_List_List*_tmp2F4=_tmp2E6;void*_tmp2F6;struct Cyc_List_List*_tmp2F7;struct Cyc_List_List*_tmp2F5=_tmp2F4;_tmp2F6=(void*)_tmp2F5->hd;_tmp2F7=_tmp2F5->tl;{
struct _tuple20*_tmp5D4;struct _tuple20*_tmp2F8=(_tmp5D4=_region_malloc(r,sizeof(*_tmp5D4)),((_tmp5D4->f1=_tmp2EF,((_tmp5D4->f2=_tmp2F3,((_tmp5D4->f3=_tmp2F7,_tmp5D4)))))));
struct Cyc_List_List*_tmp5D5;return Cyc_Tcpat_match(r,_tmp2EE,_tmp2F2,_tmp2F6,ctx,((_tmp5D5=_region_malloc(r,sizeof(*_tmp5D5)),((_tmp5D5->hd=_tmp2F8,((_tmp5D5->tl=_tmp2E7,_tmp5D5)))))),right_hand_side,rules);};};};};_LL15D:;}
# 1361
static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,void*dsc){
# 1363
void*_tmp2FB=dsc;struct Cyc_Set_Set*_tmp2FD;struct Cyc_List_List*_tmp2FF;_LL165: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2FC=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2FB;if(_tmp2FC->tag != 1)goto _LL167;else{_tmp2FD=_tmp2FC->f1;}}_LL166: {
# 1368
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp5D8;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp5D7;void*any=(void*)((_tmp5D7=_region_malloc(r,sizeof(*_tmp5D7)),((_tmp5D7[0]=((_tmp5D8.tag=1,((_tmp5D8.f1=Cyc_Tcpat_empty_con_set(r),_tmp5D8)))),_tmp5D7))));
struct Cyc_List_List*_tmp300=0;
{int i=0;for(0;i < pcon->arity;++ i){
struct Cyc_List_List*_tmp5D9;_tmp300=((_tmp5D9=_region_malloc(r,sizeof(*_tmp5D9)),((_tmp5D9->hd=any,((_tmp5D9->tl=_tmp300,_tmp5D9))))));}}
return _tmp300;}_LL167: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2FE=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2FB;if(_tmp2FE->tag != 0)goto _LL164;else{_tmp2FF=_tmp2FE->f2;}}_LL168:
 return _tmp2FF;_LL164:;}struct _tuple23{struct _RegionHandle*f1;struct Cyc_List_List*f2;};
# 1378
static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple23*env,int i){
struct _RegionHandle*_tmp305;struct Cyc_List_List*_tmp306;struct _tuple23*_tmp304=env;_tmp305=_tmp304->f1;_tmp306=_tmp304->f2;{
struct Cyc_List_List*_tmp5DA;return(_tmp5DA=_region_malloc(_tmp305,sizeof(*_tmp5DA)),((_tmp5DA->hd=(void*)(i + 1),((_tmp5DA->tl=_tmp306,_tmp5DA)))));};}
# 1382
static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){
# 1384
struct _tuple23 _tmp5DB;struct _tuple23 _tmp308=(_tmp5DB.f1=r,((_tmp5DB.f2=obj,_tmp5DB)));
return((struct Cyc_List_List*(*)(struct _RegionHandle*r,int n,struct Cyc_List_List*(*f)(struct _tuple23*,int),struct _tuple23*env))Cyc_List_rtabulate_c)(r,pcon->arity,Cyc_Tcpat_getoarg,& _tmp308);}
# 1391
static void*Cyc_Tcpat_match(struct _RegionHandle*r,void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules){
# 1395
void*_tmp30A=p;struct Cyc_Tcpat_Con_s*_tmp30D;struct Cyc_List_List*_tmp30E;_LL16A: {struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp30B=(struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*)_tmp30A;if(_tmp30B->tag != 0)goto _LL16C;}_LL16B:
# 1397
 return Cyc_Tcpat_and_match(r,Cyc_Tcpat_augment(r,ctx,dsc),work,right_hand_side,rules);_LL16C: {struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp30C=(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp30A;if(_tmp30C->tag != 1)goto _LL169;else{_tmp30D=_tmp30C->f1;_tmp30E=_tmp30C->f2;}}_LL16D: {
# 1399
enum Cyc_Tcpat_Answer _tmp30F=Cyc_Tcpat_static_match(_tmp30D,dsc);switch(_tmp30F){case Cyc_Tcpat_Yes: _LL16E: {
# 1401
struct _tuple22*_tmp5DE;struct Cyc_List_List*_tmp5DD;struct Cyc_List_List*ctx2=(_tmp5DD=_region_malloc(r,sizeof(*_tmp5DD)),((_tmp5DD->hd=((_tmp5DE=_region_malloc(r,sizeof(*_tmp5DE)),((_tmp5DE->f1=_tmp30D,((_tmp5DE->f2=0,_tmp5DE)))))),((_tmp5DD->tl=ctx,_tmp5DD)))));
struct _tuple20*_tmp5DF;struct _tuple20*work_frame=(_tmp5DF=_region_malloc(r,sizeof(*_tmp5DF)),((_tmp5DF->f1=_tmp30E,((_tmp5DF->f2=Cyc_Tcpat_getoargs(r,_tmp30D,obj),((_tmp5DF->f3=
Cyc_Tcpat_getdargs(r,_tmp30D,dsc),_tmp5DF)))))));
struct Cyc_List_List*_tmp5E0;struct Cyc_List_List*work2=(_tmp5E0=_region_malloc(r,sizeof(*_tmp5E0)),((_tmp5E0->hd=work_frame,((_tmp5E0->tl=work,_tmp5E0)))));
return Cyc_Tcpat_and_match(r,ctx2,work2,right_hand_side,rules);}case Cyc_Tcpat_No: _LL16F:
# 1407
 return Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,dsc,work),rules);case Cyc_Tcpat_Maybe: _LL170: {
# 1409
struct _tuple22*_tmp5E3;struct Cyc_List_List*_tmp5E2;struct Cyc_List_List*ctx2=(_tmp5E2=_region_malloc(r,sizeof(*_tmp5E2)),((_tmp5E2->hd=((_tmp5E3=_region_malloc(r,sizeof(*_tmp5E3)),((_tmp5E3->f1=_tmp30D,((_tmp5E3->f2=0,_tmp5E3)))))),((_tmp5E2->tl=ctx,_tmp5E2)))));
struct _tuple20*_tmp5E4;struct _tuple20*work_frame=(_tmp5E4=_region_malloc(r,sizeof(*_tmp5E4)),((_tmp5E4->f1=_tmp30E,((_tmp5E4->f2=Cyc_Tcpat_getoargs(r,_tmp30D,obj),((_tmp5E4->f3=
Cyc_Tcpat_getdargs(r,_tmp30D,dsc),_tmp5E4)))))));
struct Cyc_List_List*_tmp5E5;struct Cyc_List_List*work2=(_tmp5E5=_region_malloc(r,sizeof(*_tmp5E5)),((_tmp5E5->hd=work_frame,((_tmp5E5->tl=work,_tmp5E5)))));
void*_tmp314=Cyc_Tcpat_and_match(r,ctx2,work2,right_hand_side,rules);
void*_tmp315=Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,Cyc_Tcpat_add_neg(r,dsc,_tmp30D),work),rules);
# 1416
struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct _tmp5E8;struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*_tmp5E7;return(void*)((_tmp5E7=_region_malloc(r,sizeof(*_tmp5E7)),((_tmp5E7[0]=((_tmp5E8.tag=2,((_tmp5E8.f1=obj,((_tmp5E8.f2=_tmp30D,((_tmp5E8.f3=_tmp314,((_tmp5E8.f4=_tmp315,_tmp5E8)))))))))),_tmp5E7))));}}}_LL169:;}
# 1426
static void Cyc_Tcpat_check_exhaust_overlap(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,void*,void*),void*env1,void(*rhs_appears)(void*,void*),void*env2,int*exhaust_done){
# 1432
void*_tmp31C=d;void*_tmp31E;void*_tmp320;void*_tmp322;void*_tmp323;_LL173: {struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp31D=(struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp31C;if(_tmp31D->tag != 0)goto _LL175;else{_tmp31E=(void*)_tmp31D->f1;}}_LL174:
# 1434
 if(!(*exhaust_done)){not_exhaust(r,env1,_tmp31E);*exhaust_done=1;}
goto _LL172;_LL175: {struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp31F=(struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp31C;if(_tmp31F->tag != 1)goto _LL177;else{_tmp320=(void*)_tmp31F->f1;}}_LL176:
 rhs_appears(env2,_tmp320);goto _LL172;_LL177: {struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*_tmp321=(struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*)_tmp31C;if(_tmp321->tag != 2)goto _LL172;else{_tmp322=(void*)_tmp321->f3;_tmp323=(void*)_tmp321->f4;}}_LL178:
# 1438
 Cyc_Tcpat_check_exhaust_overlap(r,_tmp322,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1440
Cyc_Tcpat_check_exhaust_overlap(r,_tmp323,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1442
goto _LL172;_LL172:;}struct _tuple24{void*f1;struct _tuple21*f2;};struct _tuple25{struct _RegionHandle*f1;int*f2;};
# 1450
static struct _tuple24*Cyc_Tcpat_get_match(struct _tuple25*env,struct Cyc_Absyn_Switch_clause*swc){
# 1453
struct _RegionHandle*_tmp325;int*_tmp326;struct _tuple25*_tmp324=env;_tmp325=_tmp324->f1;_tmp326=_tmp324->f2;{
void*sp0=Cyc_Tcpat_compile_pat(_tmp325,swc->pattern);
struct _tuple21*_tmp5E9;struct _tuple21*rhs=(_tmp5E9=_region_malloc(_tmp325,sizeof(*_tmp5E9)),((_tmp5E9->f1=0,((_tmp5E9->f2=(swc->pattern)->loc,_tmp5E9)))));
void*sp;
if(swc->where_clause != 0){
{struct Cyc_List_List*_tmp5EC;struct Cyc_List_List*_tmp5EB;sp=Cyc_Tcpat_tuple_pat(_tmp325,((_tmp5EB=_region_malloc(_tmp325,sizeof(*_tmp5EB)),((_tmp5EB->hd=sp0,((_tmp5EB->tl=(
(_tmp5EC=_region_malloc(_tmp325,sizeof(*_tmp5EC)),((_tmp5EC->hd=Cyc_Tcpat_int_pat(_tmp325,*_tmp326,0),((_tmp5EC->tl=0,_tmp5EC)))))),_tmp5EB)))))),0);}
*_tmp326=*_tmp326 + 1;}else{
# 1462
struct Cyc_List_List*_tmp5F6;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp5F5;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp5F4;struct Cyc_List_List*_tmp5F3;sp=Cyc_Tcpat_tuple_pat(_tmp325,((_tmp5F3=_region_malloc(_tmp325,sizeof(*_tmp5F3)),((_tmp5F3->hd=sp0,((_tmp5F3->tl=((_tmp5F6=_region_malloc(_tmp325,sizeof(*_tmp5F6)),((_tmp5F6->hd=(void*)((_tmp5F4=_region_malloc(_tmp325,sizeof(*_tmp5F4)),((_tmp5F4[0]=((_tmp5F5.tag=0,_tmp5F5)),_tmp5F4)))),((_tmp5F6->tl=0,_tmp5F6)))))),_tmp5F3)))))),0);}{
struct _tuple24*_tmp5F7;return(_tmp5F7=_region_malloc(_tmp325,sizeof(*_tmp5F7)),((_tmp5F7->f1=sp,((_tmp5F7->f2=rhs,_tmp5F7)))));};};}char Cyc_Tcpat_Desc2string[12]="Desc2string";struct Cyc_Tcpat_Desc2string_exn_struct{char*tag;};
# 1468
struct Cyc_Tcpat_Desc2string_exn_struct Cyc_Tcpat_Desc2string_val={Cyc_Tcpat_Desc2string};
# 1470
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,struct Cyc_List_List*);
static struct _dyneither_ptr Cyc_Tcpat_desc2string(struct _RegionHandle*r,void*d){
void*_tmp330=d;struct Cyc_Tcpat_Con_s*_tmp332;struct Cyc_List_List*_tmp333;struct Cyc_Set_Set*_tmp335;_LL17A: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp331=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp330;if(_tmp331->tag != 0)goto _LL17C;else{_tmp332=_tmp331->f1;_tmp333=_tmp331->f2;}}_LL17B: {
# 1474
union Cyc_Tcpat_Name_value _tmp336=_tmp332->name;
struct Cyc_Absyn_Pat*_tmp337=_tmp332->orig_pat;
if(_tmp337 == 0)
return Cyc_Tcpat_desc2string(r,(void*)((struct Cyc_List_List*)_check_null(_tmp333))->hd);{
# 1479
void*_tmp338=_tmp337->r;void*_tmp339=_tmp338;struct Cyc_Absyn_Vardecl*_tmp33C;struct Cyc_Absyn_Vardecl*_tmp33E;struct Cyc_Absyn_Tvar*_tmp340;struct Cyc_Absyn_Vardecl*_tmp341;struct Cyc_Absyn_Aggrdecl*_tmp345;struct Cyc_List_List*_tmp346;struct Cyc_Absyn_Datatypefield*_tmp348;int _tmp34B;char _tmp34D;struct _dyneither_ptr _tmp34F;int _tmp350;struct Cyc_Absyn_Enumfield*_tmp352;struct Cyc_Absyn_Enumfield*_tmp354;struct Cyc_Absyn_Exp*_tmp356;_LL17F: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp33A=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp339;if(_tmp33A->tag != 0)goto _LL181;}_LL180: {
const char*_tmp5F8;return(_tmp5F8="_",_tag_dyneither(_tmp5F8,sizeof(char),2));}_LL181: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp33B=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp339;if(_tmp33B->tag != 1)goto _LL183;else{_tmp33C=_tmp33B->f1;}}_LL182:
 return Cyc_Absynpp_qvar2string(_tmp33C->name);_LL183: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp33D=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp339;if(_tmp33D->tag != 3)goto _LL185;else{_tmp33E=_tmp33D->f1;}}_LL184: {
const char*_tmp5FC;void*_tmp5FB[1];struct Cyc_String_pa_PrintArg_struct _tmp5FA;return(struct _dyneither_ptr)((_tmp5FA.tag=0,((_tmp5FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp33E->name)),((_tmp5FB[0]=& _tmp5FA,Cyc_aprintf(((_tmp5FC="*%s",_tag_dyneither(_tmp5FC,sizeof(char),4))),_tag_dyneither(_tmp5FB,sizeof(void*),1))))))));}_LL185: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp33F=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp339;if(_tmp33F->tag != 4)goto _LL187;else{_tmp340=_tmp33F->f1;_tmp341=_tmp33F->f2;}}_LL186: {
const char*_tmp601;void*_tmp600[2];struct Cyc_String_pa_PrintArg_struct _tmp5FF;struct Cyc_String_pa_PrintArg_struct _tmp5FE;return(struct _dyneither_ptr)((_tmp5FE.tag=0,((_tmp5FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp340->name),((_tmp5FF.tag=0,((_tmp5FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp341->name)),((_tmp600[0]=& _tmp5FF,((_tmp600[1]=& _tmp5FE,Cyc_aprintf(((_tmp601="%s<`%s>",_tag_dyneither(_tmp601,sizeof(char),8))),_tag_dyneither(_tmp600,sizeof(void*),2))))))))))))));}_LL187: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp342=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp339;if(_tmp342->tag != 5)goto _LL189;}_LL188: {
# 1485
const char*_tmp605;void*_tmp604[1];struct Cyc_String_pa_PrintArg_struct _tmp603;return(struct _dyneither_ptr)((_tmp603.tag=0,((_tmp603.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp333)),((_tmp604[0]=& _tmp603,Cyc_aprintf(((_tmp605="$(%s)",_tag_dyneither(_tmp605,sizeof(char),6))),_tag_dyneither(_tmp604,sizeof(void*),1))))))));}_LL189: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp343=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp339;if(_tmp343->tag != 6)goto _LL18B;}_LL18A: {
# 1487
const char*_tmp609;void*_tmp608[1];struct Cyc_String_pa_PrintArg_struct _tmp607;return(struct _dyneither_ptr)((_tmp607.tag=0,((_tmp607.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp333)),((_tmp608[0]=& _tmp607,Cyc_aprintf(((_tmp609="&%s",_tag_dyneither(_tmp609,sizeof(char),4))),_tag_dyneither(_tmp608,sizeof(void*),1))))))));}_LL18B: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp344=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp339;if(_tmp344->tag != 7)goto _LL18D;else{if(_tmp344->f1 == 0)goto _LL18D;if((((_tmp344->f1)->aggr_info).KnownAggr).tag != 2)goto _LL18D;_tmp345=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp344->f1)->aggr_info).KnownAggr).val);_tmp346=_tmp344->f3;}}_LL18C:
# 1489
 if(_tmp345->kind == Cyc_Absyn_UnionA){
struct Cyc_List_List*_tmp365=_tmp346;struct _dyneither_ptr*_tmp367;_LL1A0: if(_tmp365 == 0)goto _LL1A2;if(((struct _tuple15*)_tmp365->hd)->f1 == 0)goto _LL1A2;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp366=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((void*)(((struct _tuple15*)_tmp365->hd)->f1)->hd);if(_tmp366->tag != 1)goto _LL1A2;else{_tmp367=_tmp366->f1;}};_LL1A1: {
# 1492
const char*_tmp60F;void*_tmp60E[3];struct Cyc_String_pa_PrintArg_struct _tmp60D;struct Cyc_String_pa_PrintArg_struct _tmp60C;struct Cyc_String_pa_PrintArg_struct _tmp60B;return(struct _dyneither_ptr)((_tmp60B.tag=0,((_tmp60B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Tcpat_descs2string(r,_tmp333)),((_tmp60C.tag=0,((_tmp60C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp367),((_tmp60D.tag=0,((_tmp60D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1492
Cyc_Absynpp_qvar2string(_tmp345->name)),((_tmp60E[0]=& _tmp60D,((_tmp60E[1]=& _tmp60C,((_tmp60E[2]=& _tmp60B,Cyc_aprintf(((_tmp60F="%s{.%s = %s}",_tag_dyneither(_tmp60F,sizeof(char),13))),_tag_dyneither(_tmp60E,sizeof(void*),3))))))))))))))))))));}_LL1A2:;_LL1A3:
# 1494
 goto _LL19F;_LL19F:;}{
# 1497
const char*_tmp614;void*_tmp613[2];struct Cyc_String_pa_PrintArg_struct _tmp612;struct Cyc_String_pa_PrintArg_struct _tmp611;return(struct _dyneither_ptr)((_tmp611.tag=0,((_tmp611.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp333)),((_tmp612.tag=0,((_tmp612.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp345->name)),((_tmp613[0]=& _tmp612,((_tmp613[1]=& _tmp611,Cyc_aprintf(((_tmp614="%s{%s}",_tag_dyneither(_tmp614,sizeof(char),7))),_tag_dyneither(_tmp613,sizeof(void*),2))))))))))))));};_LL18D: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp347=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp339;if(_tmp347->tag != 8)goto _LL18F;else{_tmp348=_tmp347->f2;}}_LL18E:
# 1499
 if(_tmp333 == 0){
const char*_tmp618;void*_tmp617[1];struct Cyc_String_pa_PrintArg_struct _tmp616;return(struct _dyneither_ptr)((_tmp616.tag=0,((_tmp616.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp348->name)),((_tmp617[0]=& _tmp616,Cyc_aprintf(((_tmp618="%s",_tag_dyneither(_tmp618,sizeof(char),3))),_tag_dyneither(_tmp617,sizeof(void*),1))))))));}else{
# 1502
const char*_tmp61D;void*_tmp61C[2];struct Cyc_String_pa_PrintArg_struct _tmp61B;struct Cyc_String_pa_PrintArg_struct _tmp61A;return(struct _dyneither_ptr)((_tmp61A.tag=0,((_tmp61A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp333)),((_tmp61B.tag=0,((_tmp61B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp348->name)),((_tmp61C[0]=& _tmp61B,((_tmp61C[1]=& _tmp61A,Cyc_aprintf(((_tmp61D="%s(%s)",_tag_dyneither(_tmp61D,sizeof(char),7))),_tag_dyneither(_tmp61C,sizeof(void*),2))))))))))))));}_LL18F: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp349=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp339;if(_tmp349->tag != 9)goto _LL191;}_LL190: {
const char*_tmp61E;return(_tmp61E="NULL",_tag_dyneither(_tmp61E,sizeof(char),5));}_LL191: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp34A=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp339;if(_tmp34A->tag != 10)goto _LL193;else{_tmp34B=_tmp34A->f2;}}_LL192: {
const char*_tmp622;void*_tmp621[1];struct Cyc_Int_pa_PrintArg_struct _tmp620;return(struct _dyneither_ptr)((_tmp620.tag=1,((_tmp620.f1=(unsigned long)_tmp34B,((_tmp621[0]=& _tmp620,Cyc_aprintf(((_tmp622="%d",_tag_dyneither(_tmp622,sizeof(char),3))),_tag_dyneither(_tmp621,sizeof(void*),1))))))));}_LL193: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp34C=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp339;if(_tmp34C->tag != 11)goto _LL195;else{_tmp34D=_tmp34C->f1;}}_LL194: {
const char*_tmp626;void*_tmp625[1];struct Cyc_Int_pa_PrintArg_struct _tmp624;return(struct _dyneither_ptr)((_tmp624.tag=1,((_tmp624.f1=(unsigned long)((int)_tmp34D),((_tmp625[0]=& _tmp624,Cyc_aprintf(((_tmp626="%d",_tag_dyneither(_tmp626,sizeof(char),3))),_tag_dyneither(_tmp625,sizeof(void*),1))))))));}_LL195: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp34E=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp339;if(_tmp34E->tag != 12)goto _LL197;else{_tmp34F=_tmp34E->f1;_tmp350=_tmp34E->f2;}}_LL196:
 return _tmp34F;_LL197: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp351=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp339;if(_tmp351->tag != 13)goto _LL199;else{_tmp352=_tmp351->f2;}}_LL198:
 _tmp354=_tmp352;goto _LL19A;_LL199: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp353=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp339;if(_tmp353->tag != 14)goto _LL19B;else{_tmp354=_tmp353->f2;}}_LL19A:
 return Cyc_Absynpp_qvar2string(_tmp354->name);_LL19B: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp355=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp339;if(_tmp355->tag != 17)goto _LL19D;else{_tmp356=_tmp355->f1;}}_LL19C:
 return Cyc_Absynpp_exp2string(_tmp356);_LL19D:;_LL19E:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL17E:;};}_LL17C: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp334=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp330;if(_tmp334->tag != 1)goto _LL179;else{_tmp335=_tmp334->f1;}}_LL17D:
# 1513
 if(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_is_empty)(_tmp335)){const char*_tmp627;return(_tmp627="_",_tag_dyneither(_tmp627,sizeof(char),2));}{
struct Cyc_Tcpat_Con_s*_tmp380=((struct Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp335);
struct Cyc_Absyn_Pat*_tmp381=_tmp380->orig_pat;
# 1518
if(_tmp381 == 0)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
void*_tmp382=Cyc_Tcutil_compress((void*)_check_null(_tmp381->topt));void*_tmp383=_tmp382;struct Cyc_Absyn_PtrAtts _tmp387;struct Cyc_Absyn_Datatypedecl*_tmp389;struct Cyc_Absyn_Aggrdecl*_tmp38B;_LL1A5: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp384=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp383;if(_tmp384->tag != 6)goto _LL1A7;else{if(_tmp384->f2 != Cyc_Absyn_Char_sz)goto _LL1A7;}}_LL1A6:
# 1522
{int i=0;for(0;i < 256;++ i){
struct Cyc_Tcpat_Con_s*_tmp38C=Cyc_Tcpat_char_con(r,(char)i,_tmp381);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp335,_tmp38C)){
const char*_tmp62B;void*_tmp62A[1];struct Cyc_Int_pa_PrintArg_struct _tmp629;return(struct _dyneither_ptr)((_tmp629.tag=1,((_tmp629.f1=(unsigned long)i,((_tmp62A[0]=& _tmp629,Cyc_aprintf(((_tmp62B="%d",_tag_dyneither(_tmp62B,sizeof(char),3))),_tag_dyneither(_tmp62A,sizeof(void*),1))))))));}}}
# 1527
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL1A7: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp385=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp383;if(_tmp385->tag != 6)goto _LL1A9;}_LL1A8:
# 1530
{unsigned int i=0;for(0;i < -1;++ i){
struct Cyc_Tcpat_Con_s*_tmp390=Cyc_Tcpat_int_con(r,(int)i,_tmp381);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp335,_tmp390)){
const char*_tmp62F;void*_tmp62E[1];struct Cyc_Int_pa_PrintArg_struct _tmp62D;return(struct _dyneither_ptr)((_tmp62D.tag=1,((_tmp62D.f1=(unsigned long)((int)i),((_tmp62E[0]=& _tmp62D,Cyc_aprintf(((_tmp62F="%d",_tag_dyneither(_tmp62F,sizeof(char),3))),_tag_dyneither(_tmp62E,sizeof(void*),1))))))));}}}
# 1535
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL1A9: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp386=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp383;if(_tmp386->tag != 5)goto _LL1AB;else{_tmp387=(_tmp386->f1).ptr_atts;}}_LL1AA: {
# 1537
union Cyc_Absyn_Constraint*_tmp394=_tmp387.nullable;
int is_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp394);
if(is_nullable){
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp335,Cyc_Tcpat_null_con(r,_tmp381))){
const char*_tmp630;return(_tmp630="NULL",_tag_dyneither(_tmp630,sizeof(char),5));}}{
# 1543
const char*_tmp631;return(_tmp631="&_",_tag_dyneither(_tmp631,sizeof(char),3));};}_LL1AB: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp388=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp383;if(_tmp388->tag != 3)goto _LL1AD;else{if((((_tmp388->f1).datatype_info).KnownDatatype).tag != 2)goto _LL1AD;_tmp389=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp388->f1).datatype_info).KnownDatatype).val);}}_LL1AC:
# 1545
 if(_tmp389->is_extensible)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_List_List*_tmp397=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp389->fields))->v;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp397);
for(0;(unsigned int)_tmp397;_tmp397=_tmp397->tl){
struct _dyneither_ptr n=*(*((struct Cyc_Absyn_Datatypefield*)_tmp397->hd)->name).f2;
struct Cyc_List_List*_tmp398=((struct Cyc_Absyn_Datatypefield*)_tmp397->hd)->typs;
struct Cyc_Tcpat_Con_s*_tmp632;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp335,((_tmp632=_cycalloc(sizeof(*_tmp632)),((_tmp632->name=Cyc_Tcpat_Name_v(n),((_tmp632->arity=0,((_tmp632->span=0,((_tmp632->orig_pat=_tmp381,_tmp632)))))))))))){
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp398)== 0)
return n;else{
# 1555
const char*_tmp636;void*_tmp635[1];struct Cyc_String_pa_PrintArg_struct _tmp634;return(struct _dyneither_ptr)((_tmp634.tag=0,((_tmp634.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n),((_tmp635[0]=& _tmp634,Cyc_aprintf(((_tmp636="%s(...)",_tag_dyneither(_tmp636,sizeof(char),8))),_tag_dyneither(_tmp635,sizeof(void*),1))))))));}}}
# 1558
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);};_LL1AD:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp38A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp383;if(_tmp38A->tag != 11)goto _LL1AF;else{if((((_tmp38A->f1).aggr_info).KnownAggr).tag != 2)goto _LL1AF;_tmp38B=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp38A->f1).aggr_info).KnownAggr).val);}}if(!(_tmp38B->kind == Cyc_Absyn_UnionA))goto _LL1AF;_LL1AE: {
# 1560
struct Cyc_List_List*_tmp39D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp38B->impl))->fields;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp39D);
struct _tuple2*_tmp39E=_tmp38B->name;struct _dyneither_ptr _tmp3A0;struct _tuple2*_tmp39F=_tmp39E;_tmp3A0=*_tmp39F->f2;
for(0;(unsigned int)_tmp39D;_tmp39D=_tmp39D->tl){
struct _dyneither_ptr n=*((struct Cyc_Absyn_Aggrfield*)_tmp39D->hd)->name;
struct Cyc_Tcpat_Con_s*_tmp637;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp335,((_tmp637=_cycalloc(sizeof(*_tmp637)),((_tmp637->name=Cyc_Tcpat_Name_v(n),((_tmp637->arity=0,((_tmp637->span=0,((_tmp637->orig_pat=_tmp381,_tmp637)))))))))))){
const char*_tmp63C;void*_tmp63B[2];struct Cyc_String_pa_PrintArg_struct _tmp63A;struct Cyc_String_pa_PrintArg_struct _tmp639;return(struct _dyneither_ptr)((_tmp639.tag=0,((_tmp639.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n),((_tmp63A.tag=0,((_tmp63A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3A0),((_tmp63B[0]=& _tmp63A,((_tmp63B[1]=& _tmp639,Cyc_aprintf(((_tmp63C="%s{.%s = _}",_tag_dyneither(_tmp63C,sizeof(char),12))),_tag_dyneither(_tmp63B,sizeof(void*),2))))))))))))));}}
# 1568
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LL1AF:;_LL1B0:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL1A4:;};};_LL179:;}
# 1573
static struct _dyneither_ptr*Cyc_Tcpat_desc2stringptr(struct _RegionHandle*r,void*d){
struct _dyneither_ptr*_tmp63D;return(_tmp63D=_cycalloc(sizeof(*_tmp63D)),((_tmp63D[0]=Cyc_Tcpat_desc2string(r,d),_tmp63D)));}
# 1576
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,struct Cyc_List_List*ds){
struct Cyc_List_List*_tmp3A7=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _RegionHandle*,void*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_desc2stringptr,r,ds);
struct _dyneither_ptr*comma=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),",",sizeof(char),2);
{struct Cyc_List_List*_tmp3A8=_tmp3A7;for(0;_tmp3A8 != 0;_tmp3A8=((struct Cyc_List_List*)_check_null(_tmp3A8))->tl){
if(_tmp3A8->tl != 0){
{struct Cyc_List_List*_tmp63E;_tmp3A8->tl=((_tmp63E=_cycalloc(sizeof(*_tmp63E)),((_tmp63E->hd=comma,((_tmp63E->tl=_tmp3A8->tl,_tmp63E))))));}
_tmp3A8=_tmp3A8->tl;}}}
# 1585
return(struct _dyneither_ptr)Cyc_strconcat_l(_tmp3A7);}
# 1588
static void Cyc_Tcpat_not_exhaust_err(struct _RegionHandle*r,unsigned int loc,void*desc){
struct _handler_cons _tmp3AB;_push_handler(& _tmp3AB);{int _tmp3AD=0;if(setjmp(_tmp3AB.handler))_tmp3AD=1;if(!_tmp3AD){
{struct _dyneither_ptr _tmp3AE=Cyc_Tcpat_desc2string(r,desc);
const char*_tmp642;void*_tmp641[1];struct Cyc_String_pa_PrintArg_struct _tmp640;(_tmp640.tag=0,((_tmp640.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3AE),((_tmp641[0]=& _tmp640,Cyc_Tcutil_terr(loc,((_tmp642="patterns may not be exhaustive.\n\tmissing case for %s",_tag_dyneither(_tmp642,sizeof(char),53))),_tag_dyneither(_tmp641,sizeof(void*),1)))))));}
# 1590
;_pop_handler();}else{void*_tmp3AC=(void*)_exn_thrown;void*_tmp3B3=_tmp3AC;void*_tmp3B5;_LL1B2: {struct Cyc_Tcpat_Desc2string_exn_struct*_tmp3B4=(struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp3B3;if(_tmp3B4->tag != Cyc_Tcpat_Desc2string)goto _LL1B4;}_LL1B3:
# 1594
{const char*_tmp645;void*_tmp644;(_tmp644=0,Cyc_Tcutil_terr(loc,((_tmp645="patterns may not be exhaustive.",_tag_dyneither(_tmp645,sizeof(char),32))),_tag_dyneither(_tmp644,sizeof(void*),0)));}
goto _LL1B1;_LL1B4: _tmp3B5=_tmp3B3;_LL1B5:(void)_rethrow(_tmp3B5);_LL1B1:;}};}
# 1598
static void Cyc_Tcpat_rule_occurs(int dummy,struct _tuple21*rhs){
(*rhs).f1=1;}
# 1602
void Cyc_Tcpat_check_switch_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){
# 1607
struct _RegionHandle _tmp3B8=_new_region("r");struct _RegionHandle*r=& _tmp3B8;_push_region(r);
{int _tmp3B9=0;
struct _tuple25 _tmp646;struct _tuple25 _tmp3BA=(_tmp646.f1=r,((_tmp646.f2=& _tmp3B9,_tmp646)));
struct Cyc_List_List*_tmp3BB=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple24*(*f)(struct _tuple25*,struct Cyc_Absyn_Switch_clause*),struct _tuple25*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match,& _tmp3BA,swcs);
void*_tmp3BC=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,_tmp3BB);
int _tmp3BD=0;
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct _tuple21*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp3BC,Cyc_Tcpat_not_exhaust_err,loc,Cyc_Tcpat_rule_occurs,0,& _tmp3BD);
# 1615
for(0;_tmp3BB != 0;_tmp3BB=_tmp3BB->tl){
struct _tuple24*_tmp3BE=(struct _tuple24*)_tmp3BB->hd;int _tmp3C0;unsigned int _tmp3C1;struct _tuple24*_tmp3BF=_tmp3BE;_tmp3C0=(_tmp3BF->f2)->f1;_tmp3C1=(_tmp3BF->f2)->f2;
if(!_tmp3C0){
{const char*_tmp649;void*_tmp648;(_tmp648=0,Cyc_Tcutil_terr(_tmp3C1,((_tmp649="redundant pattern (check for misspelled constructors in earlier patterns)",_tag_dyneither(_tmp649,sizeof(char),74))),_tag_dyneither(_tmp648,sizeof(void*),0)));}
# 1620
break;}}}
# 1608
;_pop_region(r);}
# 1630
static void Cyc_Tcpat_not_exhaust_warn(struct _RegionHandle*r,struct _tuple14*pr,void*desc){
(*pr).f2=0;{
struct _handler_cons _tmp3C5;_push_handler(& _tmp3C5);{int _tmp3C7=0;if(setjmp(_tmp3C5.handler))_tmp3C7=1;if(!_tmp3C7){
{struct _dyneither_ptr _tmp3C8=Cyc_Tcpat_desc2string(r,desc);
const char*_tmp64D;void*_tmp64C[1];struct Cyc_String_pa_PrintArg_struct _tmp64B;(_tmp64B.tag=0,((_tmp64B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3C8),((_tmp64C[0]=& _tmp64B,Cyc_Tcutil_warn((*pr).f1,((_tmp64D="pattern not exhaustive.\n\tmissing case for %s",_tag_dyneither(_tmp64D,sizeof(char),45))),_tag_dyneither(_tmp64C,sizeof(void*),1)))))));}
# 1633
;_pop_handler();}else{void*_tmp3C6=(void*)_exn_thrown;void*_tmp3CD=_tmp3C6;void*_tmp3CF;_LL1B7: {struct Cyc_Tcpat_Desc2string_exn_struct*_tmp3CE=(struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp3CD;if(_tmp3CE->tag != Cyc_Tcpat_Desc2string)goto _LL1B9;}_LL1B8:
# 1637
{const char*_tmp650;void*_tmp64F;(_tmp64F=0,Cyc_Tcutil_warn((*pr).f1,((_tmp650="pattern not exhaustive.",_tag_dyneither(_tmp650,sizeof(char),24))),_tag_dyneither(_tmp64F,sizeof(void*),0)));}
goto _LL1B6;_LL1B9: _tmp3CF=_tmp3CD;_LL1BA:(void)_rethrow(_tmp3CF);_LL1B6:;}};};}
# 1641
static void Cyc_Tcpat_dummy_fn(int i,int j){
return;}struct _tuple26{void*f1;int f2;};
# 1644
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){
struct _RegionHandle _tmp3D2=_new_region("r");struct _RegionHandle*r=& _tmp3D2;_push_region(r);
{struct _tuple26*_tmp653;struct Cyc_List_List*_tmp652;struct Cyc_List_List*_tmp3D3=(_tmp652=_region_malloc(r,sizeof(*_tmp652)),((_tmp652->hd=((_tmp653=_region_malloc(r,sizeof(*_tmp653)),((_tmp653->f1=Cyc_Tcpat_compile_pat(r,p),((_tmp653->f2=0,_tmp653)))))),((_tmp652->tl=0,_tmp652)))));
void*_tmp3D4=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,_tmp3D3);
struct _tuple14 _tmp654;struct _tuple14 _tmp3D5=(_tmp654.f1=loc,((_tmp654.f2=1,_tmp654)));
int _tmp3D6=0;
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,struct _tuple14*,void*),struct _tuple14*env1,void(*rhs_appears)(int,int),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp3D4,Cyc_Tcpat_not_exhaust_warn,& _tmp3D5,Cyc_Tcpat_dummy_fn,0,& _tmp3D6);{
# 1652
int _tmp3D7=_tmp3D5.f2;_npop_handler(0);return _tmp3D7;};}
# 1646
;_pop_region(r);}
# 1660
static struct _tuple24*Cyc_Tcpat_get_match2(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*swc){
# 1662
void*sp0=Cyc_Tcpat_compile_pat(r,swc->pattern);
struct _tuple21*_tmp655;struct _tuple21*rhs=(_tmp655=_region_malloc(r,sizeof(*_tmp655)),((_tmp655->f1=0,((_tmp655->f2=(swc->pattern)->loc,_tmp655)))));
struct _tuple24*_tmp656;return(_tmp656=_region_malloc(r,sizeof(*_tmp656)),((_tmp656->f1=sp0,((_tmp656->f2=rhs,_tmp656)))));}
# 1666
static void Cyc_Tcpat_not_exhaust_err2(struct _RegionHandle*r,unsigned int loc,void*d){;}
# 1669
void Cyc_Tcpat_check_catch_overlap(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){
# 1671
struct _RegionHandle _tmp3DD=_new_region("r");struct _RegionHandle*r=& _tmp3DD;_push_region(r);
{struct Cyc_List_List*_tmp3DE=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple24*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match2,r,swcs);
void*_tmp3DF=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,_tmp3DE);
int _tmp3E0=0;
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct _tuple21*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp3DF,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,0,& _tmp3E0);
# 1677
for(0;_tmp3DE != 0;_tmp3DE=_tmp3DE->tl){
struct _tuple24*_tmp3E1=(struct _tuple24*)_tmp3DE->hd;int _tmp3E3;unsigned int _tmp3E4;struct _tuple24*_tmp3E2=_tmp3E1;_tmp3E3=(_tmp3E2->f2)->f1;_tmp3E4=(_tmp3E2->f2)->f2;
if(!_tmp3E3){
{const char*_tmp659;void*_tmp658;(_tmp658=0,Cyc_Tcutil_terr(_tmp3E4,((_tmp659="redundant pattern (check for misspelled constructors in earlier patterns)",_tag_dyneither(_tmp659,sizeof(char),74))),_tag_dyneither(_tmp658,sizeof(void*),0)));}
break;}}}
# 1672
;_pop_region(r);}
