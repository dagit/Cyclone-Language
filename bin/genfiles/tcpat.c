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
# 73 "absyn.h"
typedef struct Cyc_Relations_Reln*Cyc_Relations_reln_t;
typedef struct Cyc_List_List*Cyc_Relations_relns_t;
# 79
typedef void*Cyc_Tcpat_decision_opt_t;
# 87
typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_t;
typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 99
typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 103
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 106
typedef struct _tuple2*Cyc_Absyn_qvar_t;typedef struct _tuple2*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple2*Cyc_Absyn_typedef_name_t;
typedef struct _tuple2*Cyc_Absyn_typedef_name_opt_t;
# 111
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
# 162
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int real_const;unsigned int loc;};
# 183
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz  = 4};
# 188
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0,Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top  = 2};
# 195
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_EffKind  = 4,Cyc_Absyn_IntKind  = 5};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 215
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None  = 2};
# 217
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0,Cyc_Absyn_UnionA  = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};
# 226
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 321
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 339
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 352
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 372
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 377
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 433 "absyn.h"
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
# 435
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 448
typedef void*Cyc_Absyn_funcparams_t;
# 451
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 515
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 541
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 548
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 566
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 729 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 922
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 930
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 932
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 936
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 942
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 944
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
# 950
void*Cyc_Absyn_compress_kb(void*);
# 955
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 957
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 960
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 962
extern void*Cyc_Absyn_sint_typ;
# 964
void*Cyc_Absyn_float_typ(int);
# 966
extern void*Cyc_Absyn_empty_effect;
# 1019
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1066
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1070
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
# 1073
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1088
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
# 1111
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1120
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);
# 1180
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
# 114 "tcenv.h"
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 124
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
struct _tuple14 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 52 "tcpat.cyc"
static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Pat*p){
void*_tmp0=p->r;void*_tmp1=_tmp0;struct Cyc_Absyn_Exp*_tmp6E;enum Cyc_Absyn_AggrKind _tmp6D;struct _tuple2*_tmp6C;struct Cyc_List_List*_tmp6B;struct Cyc_List_List**_tmp6A;struct Cyc_List_List*_tmp69;int _tmp68;struct Cyc_List_List*_tmp67;struct Cyc_List_List*_tmp66;int _tmp65;struct _tuple2*_tmp64;struct Cyc_List_List*_tmp63;int _tmp62;struct _tuple2*_tmp61;switch(*((int*)_tmp1)){case 15: _LL1: _tmp61=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1)->f1;_LL2:
# 55
{struct _handler_cons _tmp2;_push_handler(& _tmp2);{int _tmp4=0;if(setjmp(_tmp2.handler))_tmp4=1;if(!_tmp4){
{struct _RegionHandle _tmp5=_new_region("r");struct _RegionHandle*r=& _tmp5;_push_region(r);
{void*_tmp6=Cyc_Tcenv_lookup_ordinary(r,te,p->loc,_tmp61,0);void*_tmp7=_tmp6;void*_tmp15;struct Cyc_Absyn_Enumfield*_tmp14;struct Cyc_Absyn_Enumdecl*_tmp13;struct Cyc_Absyn_Enumfield*_tmp12;struct Cyc_Absyn_Datatypedecl*_tmp11;struct Cyc_Absyn_Datatypefield*_tmp10;switch(*((int*)_tmp7)){case 1: _LLE: _LLF:
# 59
 p->r=(void*)& Cyc_Absyn_Wild_p_val;
({void*_tmp8=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp9="struct tag used without arguments in pattern";_tag_dyneither(_tmp9,sizeof(char),45);}),_tag_dyneither(_tmp8,sizeof(void*),0));});
_npop_handler(1);return;case 2: _LL10: _tmp11=((struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp7)->f1;_tmp10=((struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp7)->f2;_LL11:
# 63
 p->r=(void*)({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA[0]=({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmpB;_tmpB.tag=8;_tmpB.f1=_tmp11;_tmpB.f2=_tmp10;_tmpB.f3=0;_tmpB.f4=0;_tmpB;});_tmpA;});_npop_handler(1);return;case 3: _LL12: _tmp13=((struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp7)->f1;_tmp12=((struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp7)->f2;_LL13:
# 65
 p->r=(void*)({struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmpC=_cycalloc(sizeof(*_tmpC));_tmpC[0]=({struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct _tmpD;_tmpD.tag=13;_tmpD.f1=_tmp13;_tmpD.f2=_tmp12;_tmpD;});_tmpC;});
_npop_handler(1);return;case 4: _LL14: _tmp15=(void*)((struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp7)->f1;_tmp14=((struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp7)->f2;_LL15:
# 68
 p->r=(void*)({struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmpE=_cycalloc(sizeof(*_tmpE));_tmpE[0]=({struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct _tmpF;_tmpF.tag=14;_tmpF.f1=_tmp15;_tmpF.f2=_tmp14;_tmpF;});_tmpE;});
_npop_handler(1);return;default: _LL16: _LL17:
 goto _LLD;}_LLD:;}
# 57
;_pop_region(r);}
# 56
;_pop_handler();}else{void*_tmp3=(void*)_exn_thrown;void*_tmp16=_tmp3;void*_tmp17;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp16)->tag == Cyc_Dict_Absent){_LL19: _LL1A:
# 73
 goto _LL18;}else{_LL1B: _tmp17=_tmp16;_LL1C:(int)_rethrow(_tmp17);}_LL18:;}};}{
# 75
union Cyc_Absyn_Nmspace _tmp18=(*_tmp61).f1;union Cyc_Absyn_Nmspace _tmp19=_tmp18;switch((_tmp19.Rel_n).tag){case 4: _LL1E: _LL1F:
 goto _LL21;case 1: if((_tmp19.Rel_n).val == 0){_LL20: _LL21:
# 78
(*_tmp61).f1=Cyc_Absyn_Loc_n;
p->r=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp1A=_cycalloc(sizeof(*_tmp1A));_tmp1A[0]=({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp1B;_tmp1B.tag=1;_tmp1B.f1=Cyc_Absyn_new_vardecl(_tmp61,(void*)& Cyc_Absyn_VoidType_val,0);_tmp1B.f2=
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0);_tmp1B;});_tmp1A;});
return;}else{goto _LL22;}default: _LL22: _LL23:
({void*_tmp1C=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp1D="qualified variable in pattern";_tag_dyneither(_tmp1D,sizeof(char),30);}),_tag_dyneither(_tmp1C,sizeof(void*),0));});return;}_LL1D:;};case 16: _LL3: _tmp64=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1)->f1;_tmp63=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1)->f2;_tmp62=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1)->f3;_LL4:
# 85
{struct _handler_cons _tmp1E;_push_handler(& _tmp1E);{int _tmp20=0;if(setjmp(_tmp1E.handler))_tmp20=1;if(!_tmp20){
{struct _RegionHandle _tmp21=_new_region("r");struct _RegionHandle*r=& _tmp21;_push_region(r);
{void*_tmp22=Cyc_Tcenv_lookup_ordinary(r,te,p->loc,_tmp64,0);void*_tmp23=_tmp22;struct Cyc_Absyn_Datatypedecl*_tmp31;struct Cyc_Absyn_Datatypefield*_tmp30;struct Cyc_Absyn_Aggrdecl*_tmp2F;switch(*((int*)_tmp23)){case 1: _LL25: _tmp2F=((struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmp23)->f1;_LL26: {
# 89
struct Cyc_List_List*_tmp24=0;
for(0;_tmp63 != 0;_tmp63=_tmp63->tl){
_tmp24=({struct Cyc_List_List*_tmp25=_cycalloc(sizeof(*_tmp25));_tmp25->hd=({struct _tuple15*_tmp26=_cycalloc(sizeof(*_tmp26));_tmp26->f1=0;_tmp26->f2=(struct Cyc_Absyn_Pat*)_tmp63->hd;_tmp26;});_tmp25->tl=_tmp24;_tmp25;});}
p->r=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp27=_cycalloc(sizeof(*_tmp27));_tmp27[0]=({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp28;_tmp28.tag=7;_tmp28.f1=({struct Cyc_Absyn_AggrInfo*_tmp29=_cycalloc(sizeof(*_tmp29));_tmp29->aggr_info=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A[0]=_tmp2F;_tmp2A;}));_tmp29->targs=0;_tmp29;});_tmp28.f2=0;_tmp28.f3=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp24);_tmp28.f4=_tmp62;_tmp28;});_tmp27;});
_npop_handler(1);return;}case 2: _LL27: _tmp31=((struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp23)->f1;_tmp30=((struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp23)->f2;_LL28:
 p->r=(void*)({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B[0]=({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp2C;_tmp2C.tag=8;_tmp2C.f1=_tmp31;_tmp2C.f2=_tmp30;_tmp2C.f3=_tmp63;_tmp2C.f4=_tmp62;_tmp2C;});_tmp2B;});_npop_handler(1);return;case 3: _LL29: _LL2A:
 goto _LL2C;case 4: _LL2B: _LL2C:
# 98
({void*_tmp2D=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp2E="enum tag used with arguments in pattern";_tag_dyneither(_tmp2E,sizeof(char),40);}),_tag_dyneither(_tmp2D,sizeof(void*),0));});
p->r=(void*)& Cyc_Absyn_Wild_p_val;
_npop_handler(1);return;default: _LL2D: _LL2E:
 goto _LL24;}_LL24:;}
# 87
;_pop_region(r);}
# 86
;_pop_handler();}else{void*_tmp1F=(void*)_exn_thrown;void*_tmp32=_tmp1F;void*_tmp33;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp32)->tag == Cyc_Dict_Absent){_LL30: _LL31:
# 103
 goto _LL2F;}else{_LL32: _tmp33=_tmp32;_LL33:(int)_rethrow(_tmp33);}_LL2F:;}};}
({struct Cyc_String_pa_PrintArg_struct _tmp36;_tmp36.tag=0;_tmp36.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp64));({void*_tmp34[1]={& _tmp36};Cyc_Tcutil_terr(p->loc,({const char*_tmp35="%s is not a constructor in pattern";_tag_dyneither(_tmp35,sizeof(char),35);}),_tag_dyneither(_tmp34,sizeof(void*),1));});});
p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1 == 0){_LL5: _tmp67=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f2;_tmp66=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f3;_tmp65=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f4;_LL6:
# 109
 if(topt == 0)
({void*_tmp37=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp38="cannot determine pattern type";_tag_dyneither(_tmp38,sizeof(char),30);}),_tag_dyneither(_tmp37,sizeof(void*),0));});{
void*t=Cyc_Tcutil_compress(*((void**)_check_null(topt)));
{void*_tmp39=t;struct Cyc_Absyn_AggrInfo _tmp40;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp39)->tag == 11){_LL35: _tmp40=((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp39)->f1;_LL36:
# 114
 p->r=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A[0]=({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp3B;_tmp3B.tag=7;_tmp3B.f1=({struct Cyc_Absyn_AggrInfo*_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C[0]=_tmp40;_tmp3C;});_tmp3B.f2=_tmp67;_tmp3B.f3=_tmp66;_tmp3B.f4=_tmp65;_tmp3B;});_tmp3A;});
Cyc_Tcpat_resolve_pat(te,topt,p);
goto _LL34;}else{_LL37: _LL38:
# 118
({struct Cyc_String_pa_PrintArg_struct _tmp3F;_tmp3F.tag=0;_tmp3F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));({void*_tmp3D[1]={& _tmp3F};Cyc_Tcutil_terr(p->loc,({const char*_tmp3E="pattern expects aggregate type instead of %s";_tag_dyneither(_tmp3E,sizeof(char),45);}),_tag_dyneither(_tmp3D,sizeof(void*),1));});});
goto _LL34;}_LL34:;}
# 122
return;};}else{if(((((struct Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->aggr_info).UnknownAggr).tag == 1){_LL7: _tmp6D=((((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->aggr_info).UnknownAggr).val).f1;_tmp6C=((((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->aggr_info).UnknownAggr).val).f2;_tmp6B=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->targs;_tmp6A=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f2;_tmp69=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f3;_tmp68=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f4;_LL8:
# 125
{struct _handler_cons _tmp41;_push_handler(& _tmp41);{int _tmp43=0;if(setjmp(_tmp41.handler))_tmp43=1;if(!_tmp43){
{struct Cyc_Absyn_Aggrdecl**_tmp44=Cyc_Tcenv_lookup_aggrdecl(te,p->loc,_tmp6C);
struct Cyc_Absyn_Aggrdecl*_tmp45=*_tmp44;
if(_tmp45->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp48;_tmp48.tag=0;_tmp48.f1=(struct _dyneither_ptr)(
_tmp45->kind == Cyc_Absyn_StructA?({const char*_tmp49="struct";_tag_dyneither(_tmp49,sizeof(char),7);}):({const char*_tmp4A="union";_tag_dyneither(_tmp4A,sizeof(char),6);}));({void*_tmp46[1]={& _tmp48};Cyc_Tcutil_terr(p->loc,({const char*_tmp47="can't destructure an abstract %s";_tag_dyneither(_tmp47,sizeof(char),33);}),_tag_dyneither(_tmp46,sizeof(void*),1));});});
p->r=(void*)& Cyc_Absyn_Wild_p_val;
_npop_handler(0);return;}{
# 134
int more_exists=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp45->impl))->exist_vars)- ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*_tmp6A);
if(more_exists < 0){
({void*_tmp4B=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp4C="too many existentially bound type variables in pattern";_tag_dyneither(_tmp4C,sizeof(char),55);}),_tag_dyneither(_tmp4B,sizeof(void*),0));});{
struct Cyc_List_List**_tmp4D=_tmp6A;
{int n=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp45->impl))->exist_vars);for(0;n != 0;-- n){
_tmp4D=&((struct Cyc_List_List*)_check_null(*_tmp4D))->tl;}}
*_tmp4D=0;};}else{
if(more_exists > 0){
# 143
struct Cyc_List_List*_tmp4E=0;
for(0;more_exists != 0;-- more_exists){
_tmp4E=({struct Cyc_List_List*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F->hd=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50[0]=({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp51;_tmp51.tag=1;_tmp51.f1=0;_tmp51;});_tmp50;}));_tmp4F->tl=_tmp4E;_tmp4F;});}
*_tmp6A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp6A,_tmp4E);}}
# 148
p->r=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52[0]=({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp53;_tmp53.tag=7;_tmp53.f1=({struct Cyc_Absyn_AggrInfo*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->aggr_info=Cyc_Absyn_KnownAggr(_tmp44);_tmp54->targs=_tmp6B;_tmp54;});_tmp53.f2=*_tmp6A;_tmp53.f3=_tmp69;_tmp53.f4=_tmp68;_tmp53;});_tmp52;});};}
# 126
;_pop_handler();}else{void*_tmp42=(void*)_exn_thrown;void*_tmp55=_tmp42;void*_tmp58;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp55)->tag == Cyc_Dict_Absent){_LL3A: _LL3B:
# 151
({void*_tmp56=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp57="Non-aggregate name has designator patterns";_tag_dyneither(_tmp57,sizeof(char),43);}),_tag_dyneither(_tmp56,sizeof(void*),0));});
p->r=(void*)& Cyc_Absyn_Wild_p_val;
goto _LL39;}else{_LL3C: _tmp58=_tmp55;_LL3D:(int)_rethrow(_tmp58);}_LL39:;}};}
# 155
return;}else{goto _LLB;}}case 17: _LL9: _tmp6E=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1)->f1;_LLA:
# 157
 Cyc_Tcexp_tcExp(te,0,_tmp6E);
if(!Cyc_Tcutil_is_const_exp(_tmp6E)){
({void*_tmp59=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp5A="non-constant expression in case pattern";_tag_dyneither(_tmp5A,sizeof(char),40);}),_tag_dyneither(_tmp59,sizeof(void*),0));});
p->r=(void*)& Cyc_Absyn_Wild_p_val;}{
# 162
struct _tuple14 _tmp5B=Cyc_Evexp_eval_const_uint_exp(_tmp6E);struct _tuple14 _tmp5C=_tmp5B;unsigned int _tmp60;int _tmp5F;_LL3F: _tmp60=_tmp5C.f1;_tmp5F=_tmp5C.f2;_LL40:;
p->r=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp5D=_cycalloc_atomic(sizeof(*_tmp5D));_tmp5D[0]=({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmp5E;_tmp5E.tag=10;_tmp5E.f1=Cyc_Absyn_None;_tmp5E.f2=(int)_tmp60;_tmp5E;});_tmp5D;});
return;};default: _LLB: _LLC:
 return;}_LL0:;}
# 169
static struct _dyneither_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){
return(*vd->name).f2;}
# 172
static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**topt){
if(topt != 0)
return*topt;
return Cyc_Absyn_new_evar(& Cyc_Tcutil_mko,({struct Cyc_Core_Opt*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->v=s;_tmp6F;}));}
# 177
static void*Cyc_Tcpat_num_type(void**topt,void*numt){
# 181
if(topt != 0  && Cyc_Tcutil_coerceable(*topt))
return*topt;
# 184
{void*_tmp70=Cyc_Tcutil_compress(numt);void*_tmp71=_tmp70;switch(*((int*)_tmp71)){case 13: _LL42: _LL43:
 goto _LL45;case 14: _LL44: _LL45:
 if(topt != 0)return*topt;goto _LL41;default: _LL46: _LL47:
 goto _LL41;}_LL41:;}
# 189
return numt;}struct _tuple16{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 192
static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl**vd,struct Cyc_Absyn_Exp*e,struct Cyc_List_List**v_result_ptr,void*t){
# 194
if(vd != 0){
(*vd)->type=t;
(*vd)->tq=Cyc_Absyn_empty_tqual(0);}
# 198
*v_result_ptr=({struct Cyc_List_List*_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72->hd=({struct _tuple16*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->f1=vd;_tmp73->f2=e;_tmp73;});_tmp72->tl=*v_result_ptr;_tmp72;});}
# 200
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,struct Cyc_Tcpat_TcPatResult res2){
# 202
struct Cyc_Tcpat_TcPatResult _tmp74=res1;struct _tuple1*_tmp7E;struct Cyc_List_List*_tmp7D;_LL49: _tmp7E=_tmp74.tvars_and_bounds_opt;_tmp7D=_tmp74.patvars;_LL4A:;{
struct Cyc_Tcpat_TcPatResult _tmp75=res2;struct _tuple1*_tmp7C;struct Cyc_List_List*_tmp7B;_LL4C: _tmp7C=_tmp75.tvars_and_bounds_opt;_tmp7B=_tmp75.patvars;_LL4D:;
if(_tmp7E != 0  || _tmp7C != 0){
if(_tmp7E == 0)
_tmp7E=({struct _tuple1*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->f1=0;_tmp76->f2=0;_tmp76;});
if(_tmp7C == 0)
_tmp7C=({struct _tuple1*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->f1=0;_tmp77->f2=0;_tmp77;});
return({struct Cyc_Tcpat_TcPatResult _tmp78;_tmp78.tvars_and_bounds_opt=({struct _tuple1*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp7E).f1,(*_tmp7C).f1);_tmp79->f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp7E).f2,(*_tmp7C).f2);_tmp79;});_tmp78.patvars=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp7D,_tmp7B);_tmp78;});}
# 213
return({struct Cyc_Tcpat_TcPatResult _tmp7A;_tmp7A.tvars_and_bounds_opt=0;_tmp7A.patvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp7D,_tmp7B);_tmp7A;});};}
# 216
static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(unsigned int loc){
return({struct Cyc_Absyn_Pat*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->loc=loc;_tmp7F->topt=0;_tmp7F->r=(void*)& Cyc_Absyn_Wild_p_val;_tmp7F;});}
# 221
static void*Cyc_Tcpat_pat_promote_array(struct Cyc_Tcenv_Tenv*te,void*t,void*rgn_opt){
return Cyc_Tcutil_is_array(t)?
Cyc_Tcutil_promote_array(t,rgn_opt == 0?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,({struct Cyc_Core_Opt*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->v=Cyc_Tcenv_lookup_type_vars(te);_tmp80;})): rgn_opt,0): t;}struct _tuple17{struct Cyc_Absyn_Tvar*f1;int f2;};
# 228
static struct _tuple17*Cyc_Tcpat_add_false(struct Cyc_Absyn_Tvar*tv){
return({struct _tuple17*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->f1=tv;_tmp81->f2=0;_tmp81;});}struct _tuple18{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Pat*f2;};
# 232
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void*rgn_pat,int allow_ref_pat,struct Cyc_Absyn_Exp*access_exp){
# 235
Cyc_Tcpat_resolve_pat(te,topt,p);{
void*t;
struct Cyc_Tcpat_TcPatResult res=({struct Cyc_Tcpat_TcPatResult _tmp159;_tmp159.tvars_and_bounds_opt=0;_tmp159.patvars=0;_tmp159;});
# 240
{void*_tmp82=p->r;void*_tmp83=_tmp82;struct Cyc_Absyn_Datatypedecl*_tmp158;struct Cyc_Absyn_Datatypefield*_tmp157;struct Cyc_List_List**_tmp156;int _tmp155;struct Cyc_Absyn_Aggrdecl*_tmp154;struct Cyc_List_List**_tmp153;struct Cyc_List_List*_tmp152;struct Cyc_List_List**_tmp151;int _tmp150;struct Cyc_List_List**_tmp14F;int _tmp14E;struct Cyc_Absyn_Pat*_tmp14D;void*_tmp14C;struct Cyc_Absyn_Enumdecl*_tmp14B;int _tmp14A;struct Cyc_Absyn_Tvar*_tmp149;struct Cyc_Absyn_Vardecl*_tmp148;struct Cyc_Absyn_Vardecl*_tmp147;struct Cyc_Absyn_Pat*_tmp146;struct Cyc_Absyn_Tvar*_tmp145;struct Cyc_Absyn_Vardecl*_tmp144;struct Cyc_Absyn_Vardecl*_tmp143;struct Cyc_Absyn_Pat*_tmp142;switch(*((int*)_tmp83)){case 0: _LL4F: _LL50:
# 243
 if(topt != 0)
t=*topt;else{
# 246
t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(te),topt);}
goto _LL4E;case 1: _LL51: _tmp143=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp83)->f1;_tmp142=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp83)->f2;_LL52: {
# 250
struct _tuple2*_tmp84=_tmp143->name;struct _tuple2*_tmp85=_tmp84;struct _dyneither_ptr _tmp92;_LL7C: _tmp92=*_tmp85->f2;_LL7D:;
if(Cyc_strcmp((struct _dyneither_ptr)_tmp92,({const char*_tmp86="true";_tag_dyneither(_tmp86,sizeof(char),5);}))== 0  || Cyc_strcmp((struct _dyneither_ptr)_tmp92,({const char*_tmp87="false";_tag_dyneither(_tmp87,sizeof(char),6);}))== 0)
({struct Cyc_String_pa_PrintArg_struct _tmp8A;_tmp8A.tag=0;_tmp8A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp92);({void*_tmp88[1]={& _tmp8A};Cyc_Tcutil_warn(p->loc,({const char*_tmp89="you probably do not want to use %s as a local variable...";_tag_dyneither(_tmp89,sizeof(char),58);}),_tag_dyneither(_tmp88,sizeof(void*),1));});});
# 254
res=Cyc_Tcpat_tcPatRec(te,_tmp142,topt,rgn_pat,allow_ref_pat,access_exp);
t=(void*)_check_null(_tmp142->topt);
# 258
{void*_tmp8B=Cyc_Tcutil_compress(t);void*_tmp8C=_tmp8B;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8C)->tag == 8){_LL7F: _LL80:
# 260
 if(rgn_pat == 0  || !allow_ref_pat)
({void*_tmp8D=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp8E="array reference would point into unknown/unallowed region";_tag_dyneither(_tmp8E,sizeof(char),58);}),_tag_dyneither(_tmp8D,sizeof(void*),0));});
goto _LL7E;}else{_LL81: _LL82:
# 264
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tmk))
({void*_tmp8F=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp90="pattern would point to an abstract member";_tag_dyneither(_tmp90,sizeof(char),42);}),_tag_dyneither(_tmp8F,sizeof(void*),0));});
goto _LL7E;}_LL7E:;}
# 268
Cyc_Tcpat_set_vd(({struct Cyc_Absyn_Vardecl**_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91[0]=_tmp143;_tmp91;}),access_exp,& res.patvars,Cyc_Tcpat_pat_promote_array(te,t,rgn_pat));
goto _LL4E;}case 2: _LL53: _tmp145=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp83)->f1;_tmp144=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp83)->f2;_LL54: {
# 271
struct _RegionHandle _tmp93=_new_region("r2");struct _RegionHandle*r2=& _tmp93;_push_region(r2);{
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r2,p->loc,te,({struct Cyc_Absyn_Tvar*_tmp97[1];_tmp97[0]=_tmp145;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(r2,_tag_dyneither(_tmp97,sizeof(struct Cyc_Absyn_Tvar*),1));}));
if(res.tvars_and_bounds_opt == 0)
res.tvars_and_bounds_opt=({struct _tuple1*_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94->f1=0;_tmp94->f2=0;_tmp94;});
(*res.tvars_and_bounds_opt).f1=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f1,({struct Cyc_List_List*_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95->hd=({struct _tuple17*_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96->f1=_tmp145;_tmp96->f2=1;_tmp96;});_tmp95->tl=0;_tmp95;}));
Cyc_Tcutil_check_type(p->loc,te2,Cyc_Tcenv_lookup_type_vars(te2),& Cyc_Tcutil_tmk,1,0,_tmp144->type);}
# 281
if(topt != 0)t=*topt;else{
t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(te),topt);}
{void*_tmp98=Cyc_Tcutil_compress(t);void*_tmp99=_tmp98;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp99)->tag == 8){_LL84: _LL85:
# 285
 if(rgn_pat == 0  || !allow_ref_pat)
({void*_tmp9A=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp9B="array reference would point into unknown/unallowed region";_tag_dyneither(_tmp9B,sizeof(char),58);}),_tag_dyneither(_tmp9A,sizeof(void*),0));});
goto _LL83;}else{_LL86: _LL87:
# 289
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tmk))
({void*_tmp9C=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp9D="pattern would point to an abstract member";_tag_dyneither(_tmp9D,sizeof(char),42);}),_tag_dyneither(_tmp9C,sizeof(void*),0));});
goto _LL83;}_LL83:;}
# 293
Cyc_Tcpat_set_vd(({struct Cyc_Absyn_Vardecl**_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E[0]=_tmp144;_tmp9E;}),access_exp,& res.patvars,_tmp144->type);
_npop_handler(0);goto _LL4E;
# 271
;_pop_region(r2);}case 3: _LL55: _tmp147=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp83)->f1;_tmp146=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp83)->f2;_LL56:
# 297
 res=Cyc_Tcpat_tcPatRec(te,_tmp146,topt,rgn_pat,allow_ref_pat,access_exp);
t=(void*)_check_null(_tmp146->topt);
if(!allow_ref_pat  || rgn_pat == 0){
({void*_tmp9F=0;Cyc_Tcutil_terr(p->loc,({const char*_tmpA0="* pattern would point into an unknown/unallowed region";_tag_dyneither(_tmpA0,sizeof(char),55);}),_tag_dyneither(_tmp9F,sizeof(void*),0));});
goto _LL4E;}else{
# 304
struct _RegionHandle _tmpA1=_new_region("rgn");struct _RegionHandle*rgn=& _tmpA1;_push_region(rgn);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))
({void*_tmpA2=0;Cyc_Tcutil_terr(p->loc,({const char*_tmpA3="* pattern cannot take the address of an alias-free path";_tag_dyneither(_tmpA3,sizeof(char),56);}),_tag_dyneither(_tmpA2,sizeof(void*),0));});
# 305
;_pop_region(rgn);}{
# 310
struct Cyc_Absyn_Exp*new_access_exp=0;
void*t2=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpA6;_tmpA6.tag=5;_tmpA6.f1=({struct Cyc_Absyn_PtrInfo _tmpA7;_tmpA7.elt_typ=t;_tmpA7.elt_tq=Cyc_Absyn_empty_tqual(0);_tmpA7.ptr_atts=({(_tmpA7.ptr_atts).rgn=rgn_pat;(_tmpA7.ptr_atts).nullable=Cyc_Absyn_false_conref;(_tmpA7.ptr_atts).bounds=
# 313
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmpA7.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmpA7.ptr_atts).ptrloc=0;_tmpA7.ptr_atts;});_tmpA7;});_tmpA6;});_tmpA5;});
# 315
if((unsigned int)access_exp){
new_access_exp=Cyc_Absyn_address_exp(access_exp,0);
new_access_exp->topt=t2;}
# 319
Cyc_Tcpat_set_vd(({struct Cyc_Absyn_Vardecl**_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4[0]=_tmp147;_tmpA4;}),new_access_exp,& res.patvars,t2);
goto _LL4E;};case 4: _LL57: _tmp149=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp83)->f1;_tmp148=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp83)->f2;_LL58:
# 325
 Cyc_Tcpat_set_vd(({struct Cyc_Absyn_Vardecl**_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8[0]=_tmp148;_tmpA8;}),access_exp,& res.patvars,_tmp148->type);{
# 329
struct _RegionHandle _tmpA9=_new_region("r2");struct _RegionHandle*r2=& _tmpA9;_push_region(r2);
Cyc_Tcenv_add_type_vars(r2,p->loc,te,({struct Cyc_Absyn_Tvar*_tmpAA[1];_tmpAA[0]=_tmp149;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAA,sizeof(struct Cyc_Absyn_Tvar*),1));}));
# 332
if(res.tvars_and_bounds_opt == 0)
res.tvars_and_bounds_opt=({struct _tuple1*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB->f1=0;_tmpAB->f2=0;_tmpAB;});
(*res.tvars_and_bounds_opt).f1=({struct Cyc_List_List*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->hd=({struct _tuple17*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->f1=_tmp149;_tmpAD->f2=0;_tmpAD;});_tmpAC->tl=(*res.tvars_and_bounds_opt).f1;_tmpAC;});
# 336
t=Cyc_Absyn_uint_typ;
_npop_handler(0);goto _LL4E;
# 329
;_pop_region(r2);};case 10: switch(((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp83)->f1){case Cyc_Absyn_Unsigned: _LL59: _LL5A:
# 339
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_typ);goto _LL4E;case Cyc_Absyn_None: _LL5B: _LL5C:
 goto _LL5E;default: _LL5D: _LL5E:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_typ);goto _LL4E;}case 11: _LL5F: _LL60:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_typ);goto _LL4E;case 12: _LL61: _tmp14A=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp83)->f2;_LL62:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_float_typ(_tmp14A));goto _LL4E;case 13: _LL63: _tmp14B=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp83)->f1;_LL64:
# 345
 t=Cyc_Tcpat_num_type(topt,(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE[0]=({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpAF;_tmpAF.tag=13;_tmpAF.f1=_tmp14B->name;_tmpAF.f2=_tmp14B;_tmpAF;});_tmpAE;}));
goto _LL4E;case 14: _LL65: _tmp14C=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp83)->f1;_LL66:
 t=Cyc_Tcpat_num_type(topt,_tmp14C);goto _LL4E;case 9: _LL67: _LL68:
# 349
 if(topt != 0){
void*_tmpB0=Cyc_Tcutil_compress(*topt);void*_tmpB1=_tmpB0;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB1)->tag == 5){_LL89: _LL8A:
 t=*topt;goto tcpat_end;}else{_LL8B: _LL8C:
 goto _LL88;}_LL88:;}{
# 354
struct Cyc_Core_Opt*_tmpB2=Cyc_Tcenv_lookup_opt_type_vars(te);
t=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpB4;_tmpB4.tag=5;_tmpB4.f1=({struct Cyc_Absyn_PtrInfo _tmpB5;_tmpB5.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmpB2);_tmpB5.elt_tq=
Cyc_Absyn_empty_tqual(0);_tmpB5.ptr_atts=({(_tmpB5.ptr_atts).rgn=
Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmpB2);(_tmpB5.ptr_atts).nullable=Cyc_Absyn_true_conref;(_tmpB5.ptr_atts).bounds=
# 359
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmpB5.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmpB5.ptr_atts).ptrloc=0;_tmpB5.ptr_atts;});_tmpB5;});_tmpB4;});_tmpB3;});
# 362
goto _LL4E;};case 6: _LL69: _tmp14D=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp83)->f1;_LL6A: {
# 367
void*inner_typ=(void*)& Cyc_Absyn_VoidType_val;
void**_tmpB6=0;
int elt_const=0;
if(topt != 0){
void*_tmpB7=Cyc_Tcutil_compress(*topt);void*_tmpB8=_tmpB7;void*_tmpBA;struct Cyc_Absyn_Tqual _tmpB9;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB8)->tag == 5){_LL8E: _tmpBA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB8)->f1).elt_typ;_tmpB9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB8)->f1).elt_tq;_LL8F:
# 373
 inner_typ=_tmpBA;
_tmpB6=& inner_typ;
elt_const=_tmpB9.real_const;
goto _LL8D;}else{_LL90: _LL91:
 goto _LL8D;}_LL8D:;}{
# 382
void*ptr_rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp)new_access_exp=Cyc_Absyn_deref_exp(access_exp,0);
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,_tmp14D,_tmpB6,ptr_rgn,1,new_access_exp));
# 391
{void*_tmpBB=Cyc_Tcutil_compress((void*)_check_null(_tmp14D->topt));void*_tmpBC=_tmpBB;struct Cyc_Absyn_Datatypedecl*_tmpCB;struct Cyc_Absyn_Datatypefield*_tmpCA;struct Cyc_List_List*_tmpC9;if(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpBC)->tag == 4){if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpBC)->f1).field_info).KnownDatatypefield).tag == 2){_LL93: _tmpCB=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpBC)->f1).field_info).KnownDatatypefield).val).f1;_tmpCA=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpBC)->f1).field_info).KnownDatatypefield).val).f2;_tmpC9=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpBC)->f1).targs;_LL94:
# 395
{void*_tmpBD=Cyc_Tcutil_compress(inner_typ);void*_tmpBE=_tmpBD;if(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpBE)->tag == 4){_LL98: _LL99:
# 397
 goto DONT_PROMOTE;}else{_LL9A: _LL9B:
 goto _LL97;}_LL97:;}{
# 401
void*new_type=(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2[0]=({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpC3;_tmpC3.tag=3;_tmpC3.f1=({struct Cyc_Absyn_DatatypeInfo _tmpC4;_tmpC4.datatype_info=Cyc_Absyn_KnownDatatype(({struct Cyc_Absyn_Datatypedecl**_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5[0]=_tmpCB;_tmpC5;}));_tmpC4.targs=_tmpC9;_tmpC4;});_tmpC3;});_tmpC2;});
# 404
_tmp14D->topt=new_type;
t=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpC0;_tmpC0.tag=5;_tmpC0.f1=({struct Cyc_Absyn_PtrInfo _tmpC1;_tmpC1.elt_typ=new_type;_tmpC1.elt_tq=
elt_const?Cyc_Absyn_const_tqual(0):
 Cyc_Absyn_empty_tqual(0);_tmpC1.ptr_atts=({(_tmpC1.ptr_atts).rgn=ptr_rgn;(_tmpC1.ptr_atts).nullable=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmpC1.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref;(_tmpC1.ptr_atts).zero_term=Cyc_Absyn_false_conref;(_tmpC1.ptr_atts).ptrloc=0;_tmpC1.ptr_atts;});_tmpC1;});_tmpC0;});_tmpBF;});
# 411
goto _LL92;};}else{goto _LL95;}}else{_LL95: _LL96:
# 413
 DONT_PROMOTE:
 t=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpC7;_tmpC7.tag=5;_tmpC7.f1=({struct Cyc_Absyn_PtrInfo _tmpC8;_tmpC8.elt_typ=(void*)_check_null(_tmp14D->topt);_tmpC8.elt_tq=
elt_const?Cyc_Absyn_const_tqual(0):
 Cyc_Absyn_empty_tqual(0);_tmpC8.ptr_atts=({(_tmpC8.ptr_atts).rgn=ptr_rgn;(_tmpC8.ptr_atts).nullable=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmpC8.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmpC8.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmpC8.ptr_atts).ptrloc=0;_tmpC8.ptr_atts;});_tmpC8;});_tmpC7;});_tmpC6;});}_LL92:;}
# 421
if((unsigned int)new_access_exp)new_access_exp->topt=_tmp14D->topt;
goto _LL4E;};}case 5: _LL6B: _tmp14F=(struct Cyc_List_List**)&((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp83)->f1;_tmp14E=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp83)->f2;_LL6C: {
# 425
struct Cyc_List_List*_tmpCC=*_tmp14F;
struct Cyc_List_List*pat_ts=0;
struct Cyc_List_List*topt_ts=0;
if(topt != 0){
void*_tmpCD=Cyc_Tcutil_compress(*topt);void*_tmpCE=_tmpCD;struct Cyc_List_List*_tmpD4;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpCE)->tag == 10){_LL9D: _tmpD4=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpCE)->f1;_LL9E:
# 431
 topt_ts=_tmpD4;
if(_tmp14E){
# 434
int _tmpCF=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpCC);
int _tmpD0=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpD4);
if(_tmpCF < _tmpD0){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmpD0 - _tmpCF;++ i){
wild_ps=({struct Cyc_List_List*_tmpD1=_cycalloc(sizeof(*_tmpD1));_tmpD1->hd=Cyc_Tcpat_wild_pat(p->loc);_tmpD1->tl=wild_ps;_tmpD1;});}}
*_tmp14F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpCC,wild_ps);
_tmpCC=*_tmp14F;}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpCC)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpD4))
({void*_tmpD2=0;Cyc_Tcutil_warn(p->loc,({const char*_tmpD3="unnecessary ... in tuple pattern";_tag_dyneither(_tmpD3,sizeof(char),33);}),_tag_dyneither(_tmpD2,sizeof(void*),0));});}}
# 446
goto _LL9C;}else{_LL9F: _LLA0:
# 448
 goto _LL9C;}_LL9C:;}else{
# 450
if(_tmp14E)
({void*_tmpD5=0;Cyc_Tcutil_terr(p->loc,({const char*_tmpD6="cannot determine missing fields for ... in tuple pattern";_tag_dyneither(_tmpD6,sizeof(char),57);}),_tag_dyneither(_tmpD5,sizeof(void*),0));});}
{int i=0;for(0;_tmpCC != 0;(_tmpCC=_tmpCC->tl,i ++)){
void**_tmpD7=0;
if(topt_ts != 0){
_tmpD7=&(*((struct _tuple18*)topt_ts->hd)).f2;
topt_ts=topt_ts->tl;}{
# 458
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp)
new_access_exp=Cyc_Absyn_subscript_exp(access_exp,
Cyc_Absyn_const_exp(({union Cyc_Absyn_Cnst _tmpD8;(_tmpD8.Int_c).val=({struct _tuple7 _tmpD9;_tmpD9.f1=Cyc_Absyn_Unsigned;_tmpD9.f2=i;_tmpD9;});(_tmpD8.Int_c).tag=5;_tmpD8;}),0),0);
# 463
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)_tmpCC->hd,_tmpD7,rgn_pat,allow_ref_pat,new_access_exp));
# 466
if((unsigned int)new_access_exp)new_access_exp->topt=((struct Cyc_Absyn_Pat*)_tmpCC->hd)->topt;
pat_ts=({struct Cyc_List_List*_tmpDA=_cycalloc(sizeof(*_tmpDA));_tmpDA->hd=({struct _tuple18*_tmpDB=_cycalloc(sizeof(*_tmpDB));_tmpDB->f1=Cyc_Absyn_empty_tqual(0);_tmpDB->f2=(void*)_check_null(((struct Cyc_Absyn_Pat*)_tmpCC->hd)->topt);_tmpDB;});_tmpDA->tl=pat_ts;_tmpDA;});};}}
# 469
t=(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC[0]=({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpDD;_tmpDD.tag=10;_tmpDD.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(pat_ts);_tmpDD;});_tmpDC;});
goto _LL4E;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp83)->f1 != 0){if(((((struct Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp83)->f1)->aggr_info).KnownAggr).tag == 2){_LL6D: _tmp154=*(((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp83)->f1)->aggr_info).KnownAggr).val;_tmp153=(struct Cyc_List_List**)&(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp83)->f1)->targs;_tmp152=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp83)->f2;_tmp151=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp83)->f3;_tmp150=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp83)->f4;_LL6E: {
# 473
struct Cyc_List_List*_tmpDE=*_tmp151;
struct _dyneither_ptr aggr_str=_tmp154->kind == Cyc_Absyn_StructA?({const char*_tmp123="struct";_tag_dyneither(_tmp123,sizeof(char),7);}):({const char*_tmp124="union";_tag_dyneither(_tmp124,sizeof(char),6);});
if(_tmp154->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmpE1;_tmpE1.tag=0;_tmpE1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);({void*_tmpDF[1]={& _tmpE1};Cyc_Tcutil_terr(p->loc,({const char*_tmpE0="can't destructure an abstract %s";_tag_dyneither(_tmpE0,sizeof(char),33);}),_tag_dyneither(_tmpDF,sizeof(void*),1));});});
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));
goto _LL4E;}
# 482
if(_tmp154->kind == Cyc_Absyn_UnionA  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp154->impl))->tagged)
allow_ref_pat=0;
if(_tmp152 != 0){
# 488
if(topt == 0  || Cyc_Tcutil_typ_kind(*topt)!= & Cyc_Tcutil_ak)
allow_ref_pat=0;}{
# 491
struct _RegionHandle _tmpE2=_new_region("rgn");struct _RegionHandle*rgn=& _tmpE2;_push_region(rgn);{
# 493
struct Cyc_List_List*_tmpE3=0;
struct Cyc_List_List*outlives_constraints=0;
struct Cyc_List_List*_tmpE4=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp154->impl))->exist_vars;
{struct Cyc_List_List*t=_tmp152;for(0;t != 0;t=t->tl){
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;
struct Cyc_Absyn_Tvar*uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmpE4))->hd;
_tmpE4=_tmpE4->tl;{
void*_tmpE5=Cyc_Absyn_compress_kb(tv->kind);
void*_tmpE6=Cyc_Absyn_compress_kb(uv->kind);
int error=0;
struct Cyc_Absyn_Kind*k2;
{void*_tmpE7=_tmpE6;struct Cyc_Absyn_Kind*_tmpEB;struct Cyc_Absyn_Kind*_tmpEA;switch(*((int*)_tmpE7)){case 2: _LLA2: _tmpEA=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpE7)->f2;_LLA3:
 _tmpEB=_tmpEA;goto _LLA5;case 0: _LLA4: _tmpEB=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpE7)->f1;_LLA5:
 k2=_tmpEB;goto _LLA1;default: _LLA6: _LLA7:
({void*_tmpE8=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpE9="unconstrained existential type variable in aggregate";_tag_dyneither(_tmpE9,sizeof(char),53);}),_tag_dyneither(_tmpE8,sizeof(void*),0));});}_LLA1:;}
# 509
{void*_tmpEC=_tmpE5;struct Cyc_Core_Opt**_tmpF1;struct Cyc_Core_Opt**_tmpF0;struct Cyc_Absyn_Kind*_tmpEF;struct Cyc_Absyn_Kind*_tmpEE;switch(*((int*)_tmpEC)){case 0: _LLA9: _tmpEE=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpEC)->f1;_LLAA:
# 512
 if(!Cyc_Tcutil_kind_leq(k2,_tmpEE))
error=1;
goto _LLA8;case 2: _LLAB: _tmpF0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpEC)->f1;_tmpEF=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpEC)->f2;_LLAC:
 _tmpF1=_tmpF0;goto _LLAE;default: _LLAD: _tmpF1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpEC)->f1;_LLAE:
*_tmpF1=({struct Cyc_Core_Opt*_tmpED=_cycalloc(sizeof(*_tmpED));_tmpED->v=_tmpE6;_tmpED;});goto _LLA8;}_LLA8:;}
# 518
if(error)
({struct Cyc_String_pa_PrintArg_struct _tmpF6;_tmpF6.tag=0;_tmpF6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 522
Cyc_Absynpp_kind2string(k2));({struct Cyc_String_pa_PrintArg_struct _tmpF5;_tmpF5.tag=0;_tmpF5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(_tmpE5));({struct Cyc_String_pa_PrintArg_struct _tmpF4;_tmpF4.tag=0;_tmpF4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name);({void*_tmpF2[3]={& _tmpF4,& _tmpF5,& _tmpF6};Cyc_Tcutil_terr(p->loc,({const char*_tmpF3="type variable %s has kind %s but must have at least kind %s";_tag_dyneither(_tmpF3,sizeof(char),60);}),_tag_dyneither(_tmpF2,sizeof(void*),3));});});});});{
# 524
void*vartype=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpFC=_cycalloc(sizeof(*_tmpFC));_tmpFC[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpFD;_tmpFD.tag=2;_tmpFD.f1=tv;_tmpFD;});_tmpFC;});
_tmpE3=({struct Cyc_List_List*_tmpF7=_region_malloc(rgn,sizeof(*_tmpF7));_tmpF7->hd=vartype;_tmpF7->tl=_tmpE3;_tmpF7;});
# 528
if(k2->kind == Cyc_Absyn_RgnKind){
if(k2->aliasqual == Cyc_Absyn_Aliasable)
outlives_constraints=({struct Cyc_List_List*_tmpF8=_cycalloc(sizeof(*_tmpF8));_tmpF8->hd=({struct _tuple0*_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9->f1=Cyc_Absyn_empty_effect;_tmpF9->f2=vartype;_tmpF9;});_tmpF8->tl=outlives_constraints;_tmpF8;});else{
# 533
({void*_tmpFA=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpFB="opened existential had unique or top region kind";_tag_dyneither(_tmpFB,sizeof(char),49);}),_tag_dyneither(_tmpFA,sizeof(void*),0));});}}};};}}
# 537
_tmpE3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpE3);{
# 539
struct _RegionHandle _tmpFE=_new_region("r2");struct _RegionHandle*r2=& _tmpFE;_push_region(r2);
{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r2,p->loc,te,_tmp152);
# 542
struct Cyc_List_List*_tmpFF=Cyc_Tcenv_lookup_type_vars(te2);
struct _tuple12 _tmp100=({struct _tuple12 _tmp122;_tmp122.f1=_tmpFF;_tmp122.f2=rgn;_tmp122;});
struct Cyc_List_List*_tmp101=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp100,_tmp154->tvs);
struct Cyc_List_List*_tmp102=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp154->impl))->exist_vars,_tmpE3);
struct Cyc_List_List*_tmp103=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp101);
struct Cyc_List_List*_tmp104=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp102);
struct Cyc_List_List*_tmp105=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmp101,_tmp102);
# 550
if(_tmp152 != 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp154->impl))->rgn_po != 0){
if(res.tvars_and_bounds_opt == 0)
res.tvars_and_bounds_opt=({struct _tuple1*_tmp106=_cycalloc(sizeof(*_tmp106));_tmp106->f1=0;_tmp106->f2=0;_tmp106;});
(*res.tvars_and_bounds_opt).f1=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f1,((struct Cyc_List_List*(*)(struct _tuple17*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_add_false,_tmp152));
(*res.tvars_and_bounds_opt).f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,outlives_constraints);{
struct Cyc_List_List*_tmp107=0;
{struct Cyc_List_List*_tmp108=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp154->impl))->rgn_po;for(0;_tmp108 != 0;_tmp108=_tmp108->tl){
_tmp107=({struct Cyc_List_List*_tmp109=_cycalloc(sizeof(*_tmp109));_tmp109->hd=({struct _tuple0*_tmp10A=_cycalloc(sizeof(*_tmp10A));_tmp10A->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp105,(*((struct _tuple0*)_tmp108->hd)).f1);_tmp10A->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmp105,(*((struct _tuple0*)_tmp108->hd)).f2);_tmp10A;});_tmp109->tl=_tmp107;_tmp109;});}}
# 562
_tmp107=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp107);
(*res.tvars_and_bounds_opt).f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,_tmp107);};}
# 567
*_tmp153=_tmp103;
t=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp10B=_cycalloc(sizeof(*_tmp10B));_tmp10B[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp10C;_tmp10C.tag=11;_tmp10C.f1=({struct Cyc_Absyn_AggrInfo _tmp10D;_tmp10D.aggr_info=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp10E=_cycalloc(sizeof(*_tmp10E));_tmp10E[0]=_tmp154;_tmp10E;}));_tmp10D.targs=*_tmp153;_tmp10D;});_tmp10C;});_tmp10B;});
if(_tmp150  && _tmp154->kind == Cyc_Absyn_UnionA)
({void*_tmp10F=0;Cyc_Tcutil_warn(p->loc,({const char*_tmp110="`...' pattern not allowed in union pattern";_tag_dyneither(_tmp110,sizeof(char),43);}),_tag_dyneither(_tmp10F,sizeof(void*),0));});else{
if(_tmp150){
# 573
int _tmp111=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpDE);
int _tmp112=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp154->impl))->fields);
if(_tmp111 < _tmp112){
struct Cyc_List_List*wild_dps=0;
{int i=0;for(0;i < _tmp112 - _tmp111;++ i){
wild_dps=({struct Cyc_List_List*_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113->hd=({struct _tuple15*_tmp114=_cycalloc(sizeof(*_tmp114));_tmp114->f1=0;_tmp114->f2=Cyc_Tcpat_wild_pat(p->loc);_tmp114;});_tmp113->tl=wild_dps;_tmp113;});}}
*_tmp151=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpDE,wild_dps);
_tmpDE=*_tmp151;}else{
if(_tmp111 == _tmp112)
({void*_tmp115=0;Cyc_Tcutil_warn(p->loc,({const char*_tmp116="unnecessary ... in struct pattern";_tag_dyneither(_tmp116,sizeof(char),34);}),_tag_dyneither(_tmp115,sizeof(void*),0));});}}}{
# 584
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,p->loc,_tmpDE,_tmp154->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp154->impl))->fields);
for(0;fields != 0;fields=fields->tl){
struct _tuple19*_tmp117=(struct _tuple19*)fields->hd;struct _tuple19*_tmp118=_tmp117;struct Cyc_Absyn_Aggrfield*_tmp121;struct Cyc_Absyn_Pat*_tmp120;_LLB0: _tmp121=_tmp118->f1;_tmp120=_tmp118->f2;_LLB1:;{
void*_tmp119=Cyc_Tcutil_rsubstitute(rgn,_tmp105,_tmp121->type);
# 590
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp)
new_access_exp=Cyc_Absyn_aggrmember_exp(access_exp,_tmp121->name,0);
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te2,_tmp120,& _tmp119,rgn_pat,allow_ref_pat,new_access_exp));
# 598
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp120->topt),_tmp119))
({struct Cyc_String_pa_PrintArg_struct _tmp11F;_tmp11F.tag=0;_tmp11F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 601
Cyc_Absynpp_typ2string((void*)_check_null(_tmp120->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp11E;_tmp11E.tag=0;_tmp11E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 600
Cyc_Absynpp_typ2string(_tmp119));({struct Cyc_String_pa_PrintArg_struct _tmp11D;_tmp11D.tag=0;_tmp11D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);({struct Cyc_String_pa_PrintArg_struct _tmp11C;_tmp11C.tag=0;_tmp11C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp121->name);({void*_tmp11A[4]={& _tmp11C,& _tmp11D,& _tmp11E,& _tmp11F};Cyc_Tcutil_terr(p->loc,({const char*_tmp11B="field %s of %s pattern expects type %s != %s";_tag_dyneither(_tmp11B,sizeof(char),45);}),_tag_dyneither(_tmp11A,sizeof(void*),4));});});});});});
# 602
if((unsigned int)new_access_exp)new_access_exp->topt=_tmp120->topt;};}};}
# 540
;_pop_region(r2);};}
# 606
_npop_handler(0);goto _LL4E;
# 491
;_pop_region(rgn);};}}else{_LL73: _LL74:
# 662
 goto _LL76;}}else{_LL71: _LL72:
# 661
 goto _LL74;}case 8: _LL6F: _tmp158=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp83)->f1;_tmp157=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp83)->f2;_tmp156=(struct Cyc_List_List**)&((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp83)->f3;_tmp155=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp83)->f4;_LL70: {
# 609
struct Cyc_List_List*_tmp125=*_tmp156;
struct _RegionHandle _tmp126=_new_region("rgn");struct _RegionHandle*rgn=& _tmp126;_push_region(rgn);{
struct Cyc_List_List*tqts=_tmp157->typs;
# 613
struct Cyc_List_List*_tmp127=Cyc_Tcenv_lookup_type_vars(te);
struct _tuple12 _tmp128=({struct _tuple12 _tmp141;_tmp141.f1=_tmp127;_tmp141.f2=rgn;_tmp141;});
struct Cyc_List_List*_tmp129=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp128,_tmp158->tvs);
struct Cyc_List_List*_tmp12A=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp129);
t=(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp12B=_cycalloc(sizeof(*_tmp12B));_tmp12B[0]=({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp12C;_tmp12C.tag=4;_tmp12C.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmp12D;_tmp12D.field_info=Cyc_Absyn_KnownDatatypefield(_tmp158,_tmp157);_tmp12D.targs=_tmp12A;_tmp12D;});_tmp12C;});_tmp12B;});
# 619
if(_tmp155){
# 621
int _tmp12E=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp125);
int _tmp12F=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tqts);
if(_tmp12E < _tmp12F){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmp12F - _tmp12E;++ i){
wild_ps=({struct Cyc_List_List*_tmp130=_cycalloc(sizeof(*_tmp130));_tmp130->hd=Cyc_Tcpat_wild_pat(p->loc);_tmp130->tl=wild_ps;_tmp130;});}}
*_tmp156=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp125,wild_ps);
_tmp125=*_tmp156;}else{
if(_tmp12E == _tmp12F)
({struct Cyc_String_pa_PrintArg_struct _tmp133;_tmp133.tag=0;_tmp133.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp158->name));({void*_tmp131[1]={& _tmp133};Cyc_Tcutil_warn(p->loc,({const char*_tmp132="unnecessary ... in datatype field %s";_tag_dyneither(_tmp132,sizeof(char),37);}),_tag_dyneither(_tmp131,sizeof(void*),1));});});}}
# 633
for(0;_tmp125 != 0  && tqts != 0;(_tmp125=_tmp125->tl,tqts=tqts->tl)){
struct Cyc_Absyn_Pat*_tmp134=(struct Cyc_Absyn_Pat*)_tmp125->hd;
# 637
void*_tmp135=Cyc_Tcutil_rsubstitute(rgn,_tmp129,(*((struct _tuple18*)tqts->hd)).f2);
# 640
if((unsigned int)access_exp)
Cyc_Tcpat_set_vd(0,access_exp,& res.patvars,Cyc_Absyn_char_typ);
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,_tmp134,& _tmp135,rgn_pat,allow_ref_pat,0));
# 647
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp134->topt),_tmp135))
({struct Cyc_String_pa_PrintArg_struct _tmp13A;_tmp13A.tag=0;_tmp13A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 650
Cyc_Absynpp_typ2string((void*)_check_null(_tmp134->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp139;_tmp139.tag=0;_tmp139.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 649
Cyc_Absynpp_typ2string(_tmp135));({struct Cyc_String_pa_PrintArg_struct _tmp138;_tmp138.tag=0;_tmp138.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp157->name));({void*_tmp136[3]={& _tmp138,& _tmp139,& _tmp13A};Cyc_Tcutil_terr(_tmp134->loc,({const char*_tmp137="%s expects argument type %s, not %s";_tag_dyneither(_tmp137,sizeof(char),36);}),_tag_dyneither(_tmp136,sizeof(void*),3));});});});});}
# 652
if(_tmp125 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp13D;_tmp13D.tag=0;_tmp13D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp157->name));({void*_tmp13B[1]={& _tmp13D};Cyc_Tcutil_terr(p->loc,({const char*_tmp13C="too many arguments for datatype constructor %s";_tag_dyneither(_tmp13C,sizeof(char),47);}),_tag_dyneither(_tmp13B,sizeof(void*),1));});});
if(tqts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp140;_tmp140.tag=0;_tmp140.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp157->name));({void*_tmp13E[1]={& _tmp140};Cyc_Tcutil_terr(p->loc,({const char*_tmp13F="too few arguments for datatype constructor %s";_tag_dyneither(_tmp13F,sizeof(char),46);}),_tag_dyneither(_tmp13E,sizeof(void*),1));});});}
# 659
_npop_handler(0);goto _LL4E;
# 610
;_pop_region(rgn);}case 15: _LL75: _LL76:
# 663
 goto _LL78;case 17: _LL77: _LL78:
 goto _LL7A;default: _LL79: _LL7A:
# 666
 t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));goto _LL4E;}_LL4E:;}
# 668
tcpat_end:
 p->topt=t;
return res;};}
# 673
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,struct Cyc_Absyn_Exp*pat_var_exp){
# 675
struct Cyc_Tcpat_TcPatResult _tmp15A=Cyc_Tcpat_tcPatRec(te,p,topt,0,0,pat_var_exp);
# 677
struct _RegionHandle _tmp15B=_new_region("r");struct _RegionHandle*r=& _tmp15B;_push_region(r);{
struct _tuple1 _tmp15C=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(r,r,_tmp15A.patvars);struct _tuple1 _tmp15D=_tmp15C;struct Cyc_List_List*_tmp165;_LLB3: _tmp165=_tmp15D.f1;_LLB4:;{
struct Cyc_List_List*_tmp15E=0;
{struct Cyc_List_List*x=_tmp165;for(0;x != 0;x=x->tl){
if((struct Cyc_Absyn_Vardecl**)x->hd != 0)_tmp15E=({struct Cyc_List_List*_tmp15F=_region_malloc(r,sizeof(*_tmp15F));_tmp15F->hd=*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)x->hd));_tmp15F->tl=_tmp15E;_tmp15F;});}}
Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Tcpat_get_name,_tmp15E),p->loc,({const char*_tmp160="pattern contains a repeated variable";_tag_dyneither(_tmp160,sizeof(char),37);}));{
# 687
struct Cyc_List_List*x=_tmp15A.patvars;for(0;x != 0;x=x->tl){
struct _tuple16*_tmp161=(struct _tuple16*)x->hd;struct _tuple16*_tmp162=_tmp161;struct Cyc_Absyn_Vardecl**_tmp164;struct Cyc_Absyn_Exp**_tmp163;_LLB6: _tmp164=_tmp162->f1;_tmp163=(struct Cyc_Absyn_Exp**)& _tmp162->f2;_LLB7:;
if(*_tmp163 != 0  && *_tmp163 != pat_var_exp)
*_tmp163=Cyc_Tcutil_deep_copy_exp(1,(struct Cyc_Absyn_Exp*)_check_null(*_tmp163));}};};}{
# 693
struct Cyc_Tcpat_TcPatResult _tmp166=_tmp15A;_npop_handler(0);return _tmp166;};
# 677
;_pop_region(r);}
# 699
static int Cyc_Tcpat_try_alias_coerce(struct Cyc_Tcenv_Tenv*tenv,void*old_type,void*new_type,struct Cyc_Absyn_Exp*initializer,struct Cyc_List_List*assump){
# 702
struct _tuple0 _tmp167=({struct _tuple0 _tmp16F;_tmp16F.f1=Cyc_Tcutil_compress(old_type);_tmp16F.f2=Cyc_Tcutil_compress(new_type);_tmp16F;});struct _tuple0 _tmp168=_tmp167;struct Cyc_Absyn_PtrInfo _tmp16E;struct Cyc_Absyn_PtrInfo _tmp16D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp168.f1)->tag == 5){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp168.f2)->tag == 5){_LLB9: _tmp16E=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp168.f1)->f1;_tmp16D=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp168.f2)->f1;_LLBA: {
# 704
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp169=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp16A=_cycalloc(sizeof(*_tmp16A));_tmp16A[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp16B;_tmp16B.tag=5;_tmp16B.f1=({struct Cyc_Absyn_PtrInfo _tmp16C;_tmp16C.elt_typ=_tmp16E.elt_typ;_tmp16C.elt_tq=_tmp16D.elt_tq;_tmp16C.ptr_atts=({(_tmp16C.ptr_atts).rgn=(_tmp16E.ptr_atts).rgn;(_tmp16C.ptr_atts).nullable=(_tmp16D.ptr_atts).nullable;(_tmp16C.ptr_atts).bounds=(_tmp16D.ptr_atts).bounds;(_tmp16C.ptr_atts).zero_term=(_tmp16D.ptr_atts).zero_term;(_tmp16C.ptr_atts).ptrloc=(_tmp16E.ptr_atts).ptrloc;_tmp16C.ptr_atts;});_tmp16C;});_tmp16B;});_tmp16A;});
# 710
return Cyc_Tcutil_subtype(tenv,assump,(void*)_tmp169,new_type) && 
Cyc_Tcutil_coerce_assign(tenv,initializer,(void*)_tmp169);}}else{goto _LLBB;}}else{_LLBB: _LLBC:
 return 0;}_LLB8:;}
# 719
static void Cyc_Tcpat_check_alias_coercion(struct Cyc_Tcenv_Tenv*tenv,unsigned int loc,void*old_type,struct Cyc_Absyn_Tvar*tv,void*new_type,struct Cyc_Absyn_Exp*initializer){
# 722
struct Cyc_List_List*assump=({struct Cyc_List_List*_tmp17B=_cycalloc(sizeof(*_tmp17B));_tmp17B->hd=({struct _tuple0*_tmp17C=_cycalloc(sizeof(*_tmp17C));_tmp17C->f1=(void*)& Cyc_Absyn_UniqueRgn_val;_tmp17C->f2=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp17D=_cycalloc(sizeof(*_tmp17D));_tmp17D[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp17E;_tmp17E.tag=2;_tmp17E.f1=tv;_tmp17E;});_tmp17D;});_tmp17C;});_tmp17B->tl=0;_tmp17B;});
if(Cyc_Tcutil_subtype(tenv,assump,old_type,new_type)){
# 740 "tcpat.cyc"
struct _tuple0 _tmp170=({struct _tuple0 _tmp176;_tmp176.f1=Cyc_Tcutil_compress(old_type);_tmp176.f2=Cyc_Tcutil_compress(new_type);_tmp176;});struct _tuple0 _tmp171=_tmp170;struct Cyc_Absyn_PtrInfo _tmp175;struct Cyc_Absyn_PtrInfo _tmp174;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp171.f1)->tag == 5){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp171.f2)->tag == 5){_LLBE: _tmp175=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp171.f1)->f1;_tmp174=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp171.f2)->f1;_LLBF:
 goto _LLBD;}else{goto _LLC0;}}else{_LLC0: _LLC1:
({void*_tmp172=0;Cyc_Tcutil_terr(loc,({const char*_tmp173="alias requires pointer type";_tag_dyneither(_tmp173,sizeof(char),28);}),_tag_dyneither(_tmp172,sizeof(void*),0));});goto _LLBD;}_LLBD:;}else{
# 745
({struct Cyc_String_pa_PrintArg_struct _tmp17A;_tmp17A.tag=0;_tmp17A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(new_type));({struct Cyc_String_pa_PrintArg_struct _tmp179;_tmp179.tag=0;_tmp179.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(old_type));({void*_tmp177[2]={& _tmp179,& _tmp17A};Cyc_Tcutil_terr(loc,({const char*_tmp178="cannot alias value of type %s to type %s";_tag_dyneither(_tmp178,sizeof(char),41);}),_tag_dyneither(_tmp177,sizeof(void*),2));});});});}}
# 752
void Cyc_Tcpat_check_pat_regions_rec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,int did_noalias_deref,struct Cyc_List_List*patvars){
# 756
void*_tmp17F=p->r;void*_tmp180=_tmp17F;struct Cyc_Absyn_Tvar*_tmp1AA;struct Cyc_Absyn_Vardecl*_tmp1A9;struct Cyc_Absyn_Vardecl*_tmp1A8;struct Cyc_Absyn_Pat*_tmp1A7;struct Cyc_Absyn_Vardecl*_tmp1A6;struct Cyc_Absyn_Pat*_tmp1A5;struct Cyc_List_List*_tmp1A4;struct Cyc_List_List*_tmp1A3;struct Cyc_Absyn_AggrInfo*_tmp1A2;struct Cyc_List_List*_tmp1A1;struct Cyc_List_List*_tmp1A0;struct Cyc_Absyn_Pat*_tmp19F;switch(*((int*)_tmp180)){case 6: _LLC3: _tmp19F=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp180)->f1;_LLC4: {
# 758
void*_tmp181=(void*)_check_null(p->topt);void*_tmp182=_tmp181;void*_tmp185;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp182)->tag == 5){_LLD4: _tmp185=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp182)->f1).ptr_atts).rgn;_LLD5:
# 760
 Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp185);
Cyc_Tcpat_check_pat_regions_rec(te,_tmp19F,Cyc_Tcutil_is_noalias_region(_tmp185,0),patvars);
return;}else{_LLD6: _LLD7:
({void*_tmp183=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp184="check_pat_regions: bad pointer type";_tag_dyneither(_tmp184,sizeof(char),36);}),_tag_dyneither(_tmp183,sizeof(void*),0));});}_LLD3:;}case 7: _LLC5: _tmp1A2=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp180)->f1;_tmp1A1=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp180)->f2;_tmp1A0=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp180)->f3;_LLC6:
# 766
 for(0;_tmp1A0 != 0;_tmp1A0=_tmp1A0->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(*((struct _tuple15*)_tmp1A0->hd)).f2,did_noalias_deref,patvars);}
return;case 8: _LLC7: _tmp1A3=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp180)->f3;_LLC8:
# 770
 did_noalias_deref=0;_tmp1A4=_tmp1A3;goto _LLCA;case 5: _LLC9: _tmp1A4=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp180)->f1;_LLCA:
# 772
 for(0;_tmp1A4 != 0;_tmp1A4=_tmp1A4->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(struct Cyc_Absyn_Pat*)_tmp1A4->hd,did_noalias_deref,patvars);}
return;case 3: _LLCB: _tmp1A6=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp180)->f1;_tmp1A5=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp180)->f2;_LLCC:
# 776
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple16*_tmp186=(struct _tuple16*)x->hd;struct _tuple16*_tmp187=_tmp186;struct Cyc_Absyn_Vardecl**_tmp190;struct Cyc_Absyn_Exp*_tmp18F;_LLD9: _tmp190=_tmp187->f1;_tmp18F=_tmp187->f2;_LLDA:;
# 782
if((_tmp190 != 0  && *_tmp190 == _tmp1A6) && _tmp18F != 0){
{void*_tmp188=_tmp18F->r;void*_tmp189=_tmp188;struct Cyc_Absyn_Exp*_tmp18E;if(((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp189)->tag == 14){_LLDC: _tmp18E=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp189)->f1;_LLDD:
# 785
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr(Cyc_Tcenv_get_fnrgn(te),(void*)_check_null(_tmp18E->topt)))
# 787
({void*_tmp18A=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp18B="reference pattern not allowed on alias-free pointers";_tag_dyneither(_tmp18B,sizeof(char),53);}),_tag_dyneither(_tmp18A,sizeof(void*),0));});
goto _LLDB;}else{_LLDE: _LLDF:
# 790
({void*_tmp18C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp18D="check_pat_regions: bad reference access expression";_tag_dyneither(_tmp18D,sizeof(char),51);}),_tag_dyneither(_tmp18C,sizeof(void*),0));});}_LLDB:;}
# 792
break;}}}
# 795
Cyc_Tcpat_check_pat_regions_rec(te,_tmp1A5,did_noalias_deref,patvars);
return;case 1: _LLCD: _tmp1A8=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp180)->f1;_tmp1A7=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp180)->f2;_LLCE:
# 798
{void*_tmp191=p->topt;void*_tmp192=_tmp191;if(_tmp192 != 0){if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp192)->tag == 8){_LLE1: _LLE2:
# 800
 if(did_noalias_deref){
({void*_tmp193=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp194="pattern to array would create alias of no-alias pointer";_tag_dyneither(_tmp194,sizeof(char),56);}),_tag_dyneither(_tmp193,sizeof(void*),0));});
return;}
# 804
goto _LLE0;}else{goto _LLE3;}}else{_LLE3: _LLE4:
 goto _LLE0;}_LLE0:;}
# 807
Cyc_Tcpat_check_pat_regions_rec(te,_tmp1A7,did_noalias_deref,patvars);
return;case 2: _LLCF: _tmp1AA=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp180)->f1;_tmp1A9=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp180)->f2;_LLD0:
# 810
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple16*_tmp195=(struct _tuple16*)x->hd;struct _tuple16*_tmp196=_tmp195;struct Cyc_Absyn_Vardecl**_tmp19E;struct Cyc_Absyn_Exp*_tmp19D;_LLE6: _tmp19E=_tmp196->f1;_tmp19D=_tmp196->f2;_LLE7:;
# 814
if(_tmp19E != 0  && *_tmp19E == _tmp1A9){
if(_tmp19D == 0)
({void*_tmp197=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp198="cannot alias pattern expression in datatype";_tag_dyneither(_tmp198,sizeof(char),44);}),_tag_dyneither(_tmp197,sizeof(void*),0));});else{
# 818
struct _RegionHandle _tmp199=_new_region("r");struct _RegionHandle*r=& _tmp199;_push_region(r);
{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r,p->loc,te,({struct Cyc_Absyn_Tvar*_tmp19C[1];_tmp19C[0]=_tmp1AA;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(r,_tag_dyneither(_tmp19C,sizeof(struct Cyc_Absyn_Tvar*),1));}));
te2=Cyc_Tcenv_add_region(r,te2,(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp19A=_cycalloc(sizeof(*_tmp19A));_tmp19A[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp19B;_tmp19B.tag=2;_tmp19B.f1=_tmp1AA;_tmp19B;});_tmp19A;}),0,1);
# 822
Cyc_Tcpat_check_alias_coercion(te2,p->loc,(void*)_check_null(_tmp19D->topt),_tmp1AA,_tmp1A9->type,_tmp19D);}
# 819
;_pop_region(r);}
# 826
break;}}}
# 829
goto _LLC2;default: _LLD1: _LLD2:
 return;}_LLC2:;}
# 845 "tcpat.cyc"
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_List_List*patvars){
# 847
Cyc_Tcpat_check_pat_regions_rec(te,p,0,patvars);{
struct _RegionHandle _tmp1AB=_new_region("r");struct _RegionHandle*r=& _tmp1AB;_push_region(r);
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple16*_tmp1AC=(struct _tuple16*)x->hd;struct _tuple16*_tmp1AD=_tmp1AC;struct Cyc_Absyn_Vardecl**_tmp1B7;struct Cyc_Absyn_Exp*_tmp1B6;_LLE9: _tmp1B7=_tmp1AD->f1;_tmp1B6=_tmp1AD->f2;_LLEA:;
if(_tmp1B6 != 0){
struct Cyc_Absyn_Exp*_tmp1AE=_tmp1B6;
# 855
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)_check_null(_tmp1AE->topt)) && !
Cyc_Tcutil_is_noalias_path(r,_tmp1AE))
# 858
({struct Cyc_String_pa_PrintArg_struct _tmp1B1;_tmp1B1.tag=0;_tmp1B1.f1=(struct _dyneither_ptr)(
# 860
_tmp1B7 != 0?(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1B4;_tmp1B4.tag=0;_tmp1B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 862
Cyc_Absynpp_qvar2string((*_tmp1B7)->name));({void*_tmp1B2[1]={& _tmp1B4};Cyc_aprintf(({const char*_tmp1B3="for variable %s";_tag_dyneither(_tmp1B3,sizeof(char),16);}),_tag_dyneither(_tmp1B2,sizeof(void*),1));});}):({const char*_tmp1B5="";_tag_dyneither(_tmp1B5,sizeof(char),1);}));({void*_tmp1AF[1]={& _tmp1B1};Cyc_Tcutil_terr(p->loc,({const char*_tmp1B0="pattern %s dereferences a alias-free pointer from a non-unique path";_tag_dyneither(_tmp1B0,sizeof(char),68);}),_tag_dyneither(_tmp1AF,sizeof(void*),1));});});}}}
# 849
;_pop_region(r);};}
# 917 "tcpat.cyc"
struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct Cyc_Tcpat_EqNull_val={1};
struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct Cyc_Tcpat_NeqNull_val={2};
# 927
struct Cyc_Tcpat_Dummy_Tcpat_Access_struct Cyc_Tcpat_Dummy_val={0};
struct Cyc_Tcpat_Deref_Tcpat_Access_struct Cyc_Tcpat_Deref_val={1};union Cyc_Tcpat_PatOrWhere;struct Cyc_Tcpat_PathNode;struct _union_Name_value_Name_v{int tag;struct _dyneither_ptr val;};struct _union_Name_value_Int_v{int tag;int val;};union Cyc_Tcpat_Name_value{struct _union_Name_value_Name_v Name_v;struct _union_Name_value_Int_v Int_v;};
# 940
typedef union Cyc_Tcpat_Name_value Cyc_Tcpat_name_value_t;
union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct _dyneither_ptr s){return({union Cyc_Tcpat_Name_value _tmp1BC;(_tmp1BC.Name_v).val=s;(_tmp1BC.Name_v).tag=1;_tmp1BC;});}
union Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i){return({union Cyc_Tcpat_Name_value _tmp1BD;(_tmp1BD.Int_v).val=i;(_tmp1BD.Int_v).tag=2;_tmp1BD;});}struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value name;int arity;int*span;union Cyc_Tcpat_PatOrWhere orig_pat;};
# 950
typedef struct Cyc_Tcpat_Con_s*Cyc_Tcpat_con_t;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct{int tag;};struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 955
typedef void*Cyc_Tcpat_simple_pat_t;
# 958
static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){
union Cyc_Tcpat_Name_value _tmp1BE=c1->name;union Cyc_Tcpat_Name_value _tmp1BF=_tmp1BE;int _tmp1C7;struct _dyneither_ptr _tmp1C6;if((_tmp1BF.Name_v).tag == 1){_LLEC: _tmp1C6=(_tmp1BF.Name_v).val;_LLED: {
# 961
union Cyc_Tcpat_Name_value _tmp1C0=c2->name;union Cyc_Tcpat_Name_value _tmp1C1=_tmp1C0;struct _dyneither_ptr _tmp1C2;if((_tmp1C1.Name_v).tag == 1){_LLF1: _tmp1C2=(_tmp1C1.Name_v).val;_LLF2:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp1C6,(struct _dyneither_ptr)_tmp1C2);}else{_LLF3: _LLF4:
 return - 1;}_LLF0:;}}else{_LLEE: _tmp1C7=(_tmp1BF.Int_v).val;_LLEF: {
# 966
union Cyc_Tcpat_Name_value _tmp1C3=c2->name;union Cyc_Tcpat_Name_value _tmp1C4=_tmp1C3;int _tmp1C5;if((_tmp1C4.Name_v).tag == 1){_LLF6: _LLF7:
 return 1;}else{_LLF8: _tmp1C5=(_tmp1C4.Int_v).val;_LLF9:
 return _tmp1C7 - _tmp1C5;}_LLF5:;}}_LLEB:;}
# 974
static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(){
return((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(Cyc_Core_heap_region,Cyc_Tcpat_compare_con);}
# 978
static int Cyc_Tcpat_one_opt=1;
static int Cyc_Tcpat_two_opt=2;
static int Cyc_Tcpat_twofiftysix_opt=256;
# 982
static unsigned int Cyc_Tcpat_datatype_tag_number(struct Cyc_Absyn_Datatypedecl*td,struct _tuple2*name){
unsigned int ans=0;
struct Cyc_List_List*_tmp1C8=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp1C8))->hd)->name)!= 0){
++ ans;
_tmp1C8=_tmp1C8->tl;}
# 989
return ans;}
# 992
static int Cyc_Tcpat_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp1C9=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp1C9 != 0;_tmp1C9=_tmp1C9->tl){
struct Cyc_Absyn_Aggrfield*_tmp1CA=(struct Cyc_Absyn_Aggrfield*)_tmp1C9->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp1CA->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}
# 999
({void*_tmp1CB=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1CE;_tmp1CE.tag=0;_tmp1CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp1CC[1]={& _tmp1CE};Cyc_aprintf(({const char*_tmp1CD="get_member_offset %s failed";_tag_dyneither(_tmp1CD,sizeof(char),28);}),_tag_dyneither(_tmp1CC,sizeof(void*),1));});}),_tag_dyneither(_tmp1CB,sizeof(void*),0));});}
# 1002
static void*Cyc_Tcpat_get_pat_test(union Cyc_Tcpat_PatOrWhere pw){
union Cyc_Tcpat_PatOrWhere _tmp1CF=pw;struct Cyc_Absyn_Pat*_tmp208;struct Cyc_Absyn_Exp*_tmp207;if((_tmp1CF.where_clause).tag == 2){_LLFB: _tmp207=(_tmp1CF.where_clause).val;_LLFC:
 return(void*)({struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*_tmp1D0=_cycalloc(sizeof(*_tmp1D0));_tmp1D0[0]=({struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct _tmp1D1;_tmp1D1.tag=0;_tmp1D1.f1=_tmp207;_tmp1D1;});_tmp1D0;});}else{_LLFD: _tmp208=(_tmp1CF.pattern).val;_LLFE: {
# 1006
void*_tmp1D2=_tmp208->r;void*_tmp1D3=_tmp1D2;union Cyc_Absyn_AggrInfoU _tmp206;struct Cyc_List_List*_tmp205;struct Cyc_Absyn_Datatypedecl*_tmp204;struct Cyc_Absyn_Datatypefield*_tmp203;void*_tmp202;struct Cyc_Absyn_Enumfield*_tmp201;struct Cyc_Absyn_Enumdecl*_tmp200;struct Cyc_Absyn_Enumfield*_tmp1FF;struct _dyneither_ptr _tmp1FE;int _tmp1FD;char _tmp1FC;enum Cyc_Absyn_Sign _tmp1FB;int _tmp1FA;struct Cyc_Absyn_Pat*_tmp1F9;struct Cyc_Absyn_Pat*_tmp1F8;switch(*((int*)_tmp1D3)){case 1: _LL100: _tmp1F8=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1D3)->f2;_LL101:
 _tmp1F9=_tmp1F8;goto _LL103;case 3: _LL102: _tmp1F9=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1D3)->f2;_LL103:
 return Cyc_Tcpat_get_pat_test(({union Cyc_Tcpat_PatOrWhere _tmp1D4;(_tmp1D4.pattern).val=_tmp1F9;(_tmp1D4.pattern).tag=1;_tmp1D4;}));case 9: _LL104: _LL105:
 return(void*)& Cyc_Tcpat_EqNull_val;case 10: _LL106: _tmp1FB=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1D3)->f1;_tmp1FA=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1D3)->f2;_LL107:
 return(void*)({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_tmp1D5=_cycalloc_atomic(sizeof(*_tmp1D5));_tmp1D5[0]=({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct _tmp1D6;_tmp1D6.tag=6;_tmp1D6.f1=(unsigned int)_tmp1FA;_tmp1D6;});_tmp1D5;});case 11: _LL108: _tmp1FC=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1D3)->f1;_LL109:
 return(void*)({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_tmp1D7=_cycalloc_atomic(sizeof(*_tmp1D7));_tmp1D7[0]=({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct _tmp1D8;_tmp1D8.tag=6;_tmp1D8.f1=(unsigned int)_tmp1FC;_tmp1D8;});_tmp1D7;});case 12: _LL10A: _tmp1FE=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1D3)->f1;_tmp1FD=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1D3)->f2;_LL10B:
 return(void*)({struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*_tmp1D9=_cycalloc(sizeof(*_tmp1D9));_tmp1D9[0]=({struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct _tmp1DA;_tmp1DA.tag=5;_tmp1DA.f1=_tmp1FE;_tmp1DA.f2=_tmp1FD;_tmp1DA;});_tmp1D9;});case 13: _LL10C: _tmp200=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1D3)->f1;_tmp1FF=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1D3)->f2;_LL10D:
 return(void*)({struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*_tmp1DB=_cycalloc(sizeof(*_tmp1DB));_tmp1DB[0]=({struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct _tmp1DC;_tmp1DC.tag=3;_tmp1DC.f1=_tmp200;_tmp1DC.f2=_tmp1FF;_tmp1DC;});_tmp1DB;});case 14: _LL10E: _tmp202=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1D3)->f1;_tmp201=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1D3)->f2;_LL10F:
 return(void*)({struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*_tmp1DD=_cycalloc(sizeof(*_tmp1DD));_tmp1DD[0]=({struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct _tmp1DE;_tmp1DE.tag=4;_tmp1DE.f1=_tmp202;_tmp1DE.f2=_tmp201;_tmp1DE;});_tmp1DD;});case 6: _LL110: _LL111:
# 1016
{void*_tmp1DF=Cyc_Tcutil_compress((void*)_check_null(_tmp208->topt));void*_tmp1E0=_tmp1DF;union Cyc_Absyn_Constraint*_tmp1E1;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E0)->tag == 5){_LL119: _tmp1E1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E0)->f1).ptr_atts).nullable;_LL11A:
# 1018
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp1E1))
return(void*)& Cyc_Tcpat_NeqNull_val;
goto _LL118;}else{_LL11B: _LL11C:
 goto _LL118;}_LL118:;}
# 1023
({void*_tmp1E2=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1E3="non-null pointer type or non-pointer type in pointer pattern";_tag_dyneither(_tmp1E3,sizeof(char),61);}),_tag_dyneither(_tmp1E2,sizeof(void*),0));});case 8: _LL112: _tmp204=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1D3)->f1;_tmp203=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1D3)->f2;_LL113:
# 1025
 if(_tmp204->is_extensible)
return(void*)({struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*_tmp1E4=_cycalloc(sizeof(*_tmp1E4));_tmp1E4[0]=({struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct _tmp1E5;_tmp1E5.tag=9;_tmp1E5.f1=_tmp204;_tmp1E5.f2=_tmp203;_tmp1E5;});_tmp1E4;});else{
# 1028
return(void*)({struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*_tmp1E6=_cycalloc(sizeof(*_tmp1E6));_tmp1E6[0]=({struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct _tmp1E7;_tmp1E7.tag=7;_tmp1E7.f1=(int)Cyc_Tcpat_datatype_tag_number(_tmp204,_tmp203->name);_tmp1E7.f2=_tmp204;_tmp1E7.f3=_tmp203;_tmp1E7;});_tmp1E6;});}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D3)->f1 != 0){_LL114: _tmp206=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D3)->f1)->aggr_info;_tmp205=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D3)->f3;_LL115: {
# 1030
struct Cyc_Absyn_Aggrdecl*_tmp1E8=Cyc_Absyn_get_known_aggrdecl(_tmp206);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1E8->impl))->tagged){
struct _tuple15*_tmp1E9=(struct _tuple15*)((struct Cyc_List_List*)_check_null(_tmp205))->hd;struct _tuple15*_tmp1EA=_tmp1E9;struct Cyc_List_List*_tmp1F3;struct Cyc_Absyn_Pat*_tmp1F2;_LL11E: _tmp1F3=_tmp1EA->f1;_tmp1F2=_tmp1EA->f2;_LL11F:;{
struct _dyneither_ptr*f;
{void*_tmp1EB=(void*)((struct Cyc_List_List*)_check_null(_tmp1F3))->hd;void*_tmp1EC=_tmp1EB;struct _dyneither_ptr*_tmp1EF;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp1EC)->tag == 1){_LL121: _tmp1EF=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp1EC)->f1;_LL122:
 f=_tmp1EF;goto _LL120;}else{_LL123: _LL124:
({void*_tmp1ED=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1EE="no field name in tagged union pattern";_tag_dyneither(_tmp1EE,sizeof(char),38);}),_tag_dyneither(_tmp1ED,sizeof(void*),0));});}_LL120:;}
# 1038
return(void*)({struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*_tmp1F0=_cycalloc(sizeof(*_tmp1F0));_tmp1F0[0]=({struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct _tmp1F1;_tmp1F1.tag=8;_tmp1F1.f1=f;_tmp1F1.f2=Cyc_Tcpat_get_member_offset(_tmp1E8,f);_tmp1F1;});_tmp1F0;});};}else{
# 1040
({void*_tmp1F4=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1F5="non-tagged aggregate in pattern test";_tag_dyneither(_tmp1F5,sizeof(char),37);}),_tag_dyneither(_tmp1F4,sizeof(void*),0));});}}}else{goto _LL116;}default: _LL116: _LL117:
({void*_tmp1F6=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1F7="non-test pattern in pattern test";_tag_dyneither(_tmp1F7,sizeof(char),33);}),_tag_dyneither(_tmp1F6,sizeof(void*),0));});}_LLFF:;}}_LLFA:;}
# 1046
static union Cyc_Tcpat_PatOrWhere Cyc_Tcpat_pw(struct Cyc_Absyn_Pat*p){
return({union Cyc_Tcpat_PatOrWhere _tmp209;(_tmp209.pattern).val=p;(_tmp209.pattern).tag=1;_tmp209;});}
# 1050
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp20A=_cycalloc(sizeof(*_tmp20A));_tmp20A->name=Cyc_Tcpat_Name_v(({const char*_tmp20B="NULL";_tag_dyneither(_tmp20B,sizeof(char),5);}));_tmp20A->arity=0;_tmp20A->span=& Cyc_Tcpat_two_opt;_tmp20A->orig_pat=Cyc_Tcpat_pw(p);_tmp20A;});}
# 1053
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp20C=_cycalloc(sizeof(*_tmp20C));_tmp20C->name=Cyc_Tcpat_Name_v(({const char*_tmp20D="&";_tag_dyneither(_tmp20D,sizeof(char),2);}));_tmp20C->arity=1;_tmp20C->span=& Cyc_Tcpat_two_opt;_tmp20C->orig_pat=Cyc_Tcpat_pw(p);_tmp20C;});}
# 1056
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp20E=_cycalloc(sizeof(*_tmp20E));_tmp20E->name=Cyc_Tcpat_Name_v(({const char*_tmp20F="&";_tag_dyneither(_tmp20F,sizeof(char),2);}));_tmp20E->arity=1;_tmp20E->span=& Cyc_Tcpat_one_opt;_tmp20E->orig_pat=Cyc_Tcpat_pw(p);_tmp20E;});}
# 1059
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(int i,union Cyc_Tcpat_PatOrWhere p){
return({struct Cyc_Tcpat_Con_s*_tmp210=_cycalloc(sizeof(*_tmp210));_tmp210->name=Cyc_Tcpat_Int_v(i);_tmp210->arity=0;_tmp210->span=0;_tmp210->orig_pat=p;_tmp210;});}
# 1062
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp211=_cycalloc(sizeof(*_tmp211));_tmp211->name=Cyc_Tcpat_Name_v(f);_tmp211->arity=0;_tmp211->span=0;_tmp211->orig_pat=Cyc_Tcpat_pw(p);_tmp211;});}
# 1065
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(char c,struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp212=_cycalloc(sizeof(*_tmp212));_tmp212->name=Cyc_Tcpat_Int_v((int)c);_tmp212->arity=0;_tmp212->span=& Cyc_Tcpat_twofiftysix_opt;_tmp212->orig_pat=Cyc_Tcpat_pw(p);_tmp212;});}
# 1068
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(int i,union Cyc_Tcpat_PatOrWhere p){
return({struct Cyc_Tcpat_Con_s*_tmp213=_cycalloc(sizeof(*_tmp213));_tmp213->name=Cyc_Tcpat_Name_v(({const char*_tmp214="$";_tag_dyneither(_tmp214,sizeof(char),2);}));_tmp213->arity=i;_tmp213->span=& Cyc_Tcpat_one_opt;_tmp213->orig_pat=p;_tmp213;});}
# 1073
static void*Cyc_Tcpat_null_pat(struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp215=_cycalloc(sizeof(*_tmp215));_tmp215[0]=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp216;_tmp216.tag=1;_tmp216.f1=Cyc_Tcpat_null_con(p);_tmp216.f2=0;_tmp216;});_tmp215;});}
# 1076
static void*Cyc_Tcpat_int_pat(int i,union Cyc_Tcpat_PatOrWhere p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp217=_cycalloc(sizeof(*_tmp217));_tmp217[0]=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp218;_tmp218.tag=1;_tmp218.f1=Cyc_Tcpat_int_con(i,p);_tmp218.f2=0;_tmp218;});_tmp217;});}
# 1079
static void*Cyc_Tcpat_char_pat(char c,struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp219=_cycalloc(sizeof(*_tmp219));_tmp219[0]=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp21A;_tmp21A.tag=1;_tmp21A.f1=Cyc_Tcpat_char_con(c,p);_tmp21A.f2=0;_tmp21A;});_tmp219;});}
# 1082
static void*Cyc_Tcpat_float_pat(struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp21B=_cycalloc(sizeof(*_tmp21B));_tmp21B[0]=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp21C;_tmp21C.tag=1;_tmp21C.f1=Cyc_Tcpat_float_con(f,p);_tmp21C.f2=0;_tmp21C;});_tmp21B;});}
# 1085
static void*Cyc_Tcpat_null_ptr_pat(void*p,struct Cyc_Absyn_Pat*p0){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp21D=_cycalloc(sizeof(*_tmp21D));_tmp21D[0]=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp21E;_tmp21E.tag=1;_tmp21E.f1=Cyc_Tcpat_null_ptr_con(p0);_tmp21E.f2=({struct Cyc_List_List*_tmp21F=_cycalloc(sizeof(*_tmp21F));_tmp21F->hd=p;_tmp21F->tl=0;_tmp21F;});_tmp21E;});_tmp21D;});}
# 1088
static void*Cyc_Tcpat_ptr_pat(void*p,struct Cyc_Absyn_Pat*p0){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp220=_cycalloc(sizeof(*_tmp220));_tmp220[0]=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp221;_tmp221.tag=1;_tmp221.f1=Cyc_Tcpat_ptr_con(p0);_tmp221.f2=({struct Cyc_List_List*_tmp222=_cycalloc(sizeof(*_tmp222));_tmp222->hd=p;_tmp222->tl=0;_tmp222;});_tmp221;});_tmp220;});}
# 1091
static void*Cyc_Tcpat_tuple_pat(struct Cyc_List_List*ss,union Cyc_Tcpat_PatOrWhere p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp223=_cycalloc(sizeof(*_tmp223));_tmp223[0]=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp224;_tmp224.tag=1;_tmp224.f1=Cyc_Tcpat_tuple_con(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ss),p);_tmp224.f2=ss;_tmp224;});_tmp223;});}
# 1094
static void*Cyc_Tcpat_con_pat(struct _dyneither_ptr con_name,int*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*p){
# 1096
struct Cyc_Tcpat_Con_s*c=({struct Cyc_Tcpat_Con_s*_tmp227=_cycalloc(sizeof(*_tmp227));_tmp227->name=Cyc_Tcpat_Name_v(con_name);_tmp227->arity=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ps);_tmp227->span=span;_tmp227->orig_pat=Cyc_Tcpat_pw(p);_tmp227;});
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp225=_cycalloc(sizeof(*_tmp225));_tmp225[0]=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp226;_tmp226.tag=1;_tmp226.f1=c;_tmp226.f2=ps;_tmp226;});_tmp225;});}
# 1101
static void*Cyc_Tcpat_compile_pat(struct Cyc_Absyn_Pat*p){
void*s;
{void*_tmp228=p->r;void*_tmp229=_tmp228;void*_tmp26C;struct Cyc_Absyn_Enumfield*_tmp26B;struct Cyc_Absyn_Enumdecl*_tmp26A;struct Cyc_Absyn_Enumfield*_tmp269;struct Cyc_Absyn_Aggrdecl*_tmp268;struct Cyc_List_List*_tmp267;struct Cyc_List_List*_tmp266;struct Cyc_Absyn_Datatypedecl*_tmp265;struct Cyc_Absyn_Datatypefield*_tmp264;struct Cyc_List_List*_tmp263;struct Cyc_Absyn_Pat*_tmp262;struct Cyc_Absyn_Pat*_tmp261;struct Cyc_Absyn_Pat*_tmp260;struct _dyneither_ptr _tmp25F;char _tmp25E;enum Cyc_Absyn_Sign _tmp25D;int _tmp25C;switch(*((int*)_tmp229)){case 0: _LL126: _LL127:
 goto _LL129;case 2: _LL128: _LL129:
 goto _LL12B;case 4: _LL12A: _LL12B:
 s=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp22A=_cycalloc_atomic(sizeof(*_tmp22A));_tmp22A[0]=({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp22B;_tmp22B.tag=0;_tmp22B;});_tmp22A;});goto _LL125;case 9: _LL12C: _LL12D:
 s=Cyc_Tcpat_null_pat(p);goto _LL125;case 10: _LL12E: _tmp25D=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp229)->f1;_tmp25C=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp229)->f2;_LL12F:
 s=Cyc_Tcpat_int_pat(_tmp25C,Cyc_Tcpat_pw(p));goto _LL125;case 11: _LL130: _tmp25E=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp229)->f1;_LL131:
 s=Cyc_Tcpat_char_pat(_tmp25E,p);goto _LL125;case 12: _LL132: _tmp25F=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp229)->f1;_LL133:
 s=Cyc_Tcpat_float_pat(_tmp25F,p);goto _LL125;case 1: _LL134: _tmp260=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp229)->f2;_LL135:
 s=Cyc_Tcpat_compile_pat(_tmp260);goto _LL125;case 3: _LL136: _tmp261=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp229)->f2;_LL137:
 s=Cyc_Tcpat_compile_pat(_tmp261);goto _LL125;case 6: _LL138: _tmp262=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp229)->f1;_LL139:
# 1114
{void*_tmp22C=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp22D=_tmp22C;union Cyc_Absyn_Constraint*_tmp234;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22D)->tag == 5){_LL14D: _tmp234=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22D)->f1).ptr_atts).nullable;_LL14E: {
# 1116
int is_nullable=0;
int still_working=1;
while(still_working){
union Cyc_Absyn_Constraint*_tmp22E=_tmp234;int _tmp231;union Cyc_Absyn_Constraint*_tmp230;switch((((union Cyc_Absyn_Constraint*)_tmp22E)->No_constr).tag){case 2: _LL152: _tmp230=(_tmp22E->Forward_constr).val;_LL153:
# 1121
*_tmp234=*_tmp230;
continue;case 3: _LL154: _LL155:
# 1124
({struct _union_Constraint_Eq_constr*_tmp22F=& _tmp234->Eq_constr;_tmp22F->tag=1;_tmp22F->val=0;});
is_nullable=0;
still_working=0;
goto _LL151;default: _LL156: _tmp231=(int)(_tmp22E->Eq_constr).val;_LL157:
# 1129
 is_nullable=_tmp231;
still_working=0;
goto _LL151;}_LL151:;}{
# 1134
void*ss=Cyc_Tcpat_compile_pat(_tmp262);
if(is_nullable)
s=Cyc_Tcpat_null_ptr_pat(ss,p);else{
# 1138
s=Cyc_Tcpat_ptr_pat(ss,p);}
goto _LL14C;};}}else{_LL14F: _LL150:
({void*_tmp232=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp233="expecting pointertype for pattern!";_tag_dyneither(_tmp233,sizeof(char),35);}),_tag_dyneither(_tmp232,sizeof(void*),0));});}_LL14C:;}
# 1142
goto _LL125;case 8: _LL13A: _tmp265=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp229)->f1;_tmp264=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp229)->f2;_tmp263=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp229)->f3;_LL13B: {
# 1144
int*span;
{void*_tmp235=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp236=_tmp235;switch(*((int*)_tmp236)){case 3: _LL159: _LL15A:
# 1147
 if(_tmp265->is_extensible)
span=0;else{
# 1150
span=({int*_tmp237=_cycalloc_atomic(sizeof(*_tmp237));_tmp237[0]=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp265->fields))->v);_tmp237;});}
goto _LL158;case 4: _LL15B: _LL15C:
 span=& Cyc_Tcpat_one_opt;goto _LL158;default: _LL15D: _LL15E:
 span=({void*_tmp238=0;((int*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp239="void datatype pattern has bad type";_tag_dyneither(_tmp239,sizeof(char),35);}),_tag_dyneither(_tmp238,sizeof(void*),0));});goto _LL158;}_LL158:;}
# 1155
s=Cyc_Tcpat_con_pat(*(*_tmp264->name).f2,span,((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp263),p);
goto _LL125;}case 5: _LL13C: _tmp266=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp229)->f1;_LL13D:
# 1159
 s=Cyc_Tcpat_tuple_pat(((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp266),Cyc_Tcpat_pw(p));goto _LL125;case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp229)->f1 != 0){if(((((struct Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp229)->f1)->aggr_info).KnownAggr).tag == 2){_LL13E: _tmp268=*(((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp229)->f1)->aggr_info).KnownAggr).val;_tmp267=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp229)->f3;_LL13F:
# 1164
 if(_tmp268->kind == Cyc_Absyn_StructA){
struct Cyc_List_List*ps=0;
{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp268->impl))->fields;for(0;fields != 0;fields=fields->tl){
# 1168
int found=Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,({const char*_tmp245="";_tag_dyneither(_tmp245,sizeof(char),1);}))== 0;
{struct Cyc_List_List*dlps0=_tmp267;for(0;!found  && dlps0 != 0;dlps0=dlps0->tl){
struct _tuple15*_tmp23A=(struct _tuple15*)dlps0->hd;struct _tuple15*_tmp23B=_tmp23A;struct Cyc_List_List*_tmp242;struct Cyc_Absyn_Pat*_tmp241;_LL160: _tmp242=_tmp23B->f1;_tmp241=_tmp23B->f2;_LL161:;{
struct Cyc_List_List*_tmp23C=_tmp242;struct _dyneither_ptr*_tmp240;if(_tmp23C != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)_tmp23C)->hd)->tag == 1){if(((struct Cyc_List_List*)_tmp23C)->tl == 0){_LL163: _tmp240=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp23C->hd)->f1;_LL164:
# 1173
 if(Cyc_strptrcmp(_tmp240,((struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){
ps=({struct Cyc_List_List*_tmp23D=_cycalloc(sizeof(*_tmp23D));_tmp23D->hd=Cyc_Tcpat_compile_pat(_tmp241);_tmp23D->tl=ps;_tmp23D;});
found=1;}
# 1177
goto _LL162;}else{goto _LL165;}}else{goto _LL165;}}else{_LL165: _LL166:
({void*_tmp23E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp23F="bad designator(s)";_tag_dyneither(_tmp23F,sizeof(char),18);}),_tag_dyneither(_tmp23E,sizeof(void*),0));});}_LL162:;};}}
# 1181
if(!found)
({void*_tmp243=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp244="bad designator";_tag_dyneither(_tmp244,sizeof(char),15);}),_tag_dyneither(_tmp243,sizeof(void*),0));});}}
# 1184
s=Cyc_Tcpat_tuple_pat(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ps),Cyc_Tcpat_pw(p));}else{
# 1187
if(!((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp268->impl))->tagged)
({void*_tmp246=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp247="patterns on untagged unions not yet supported.";_tag_dyneither(_tmp247,sizeof(char),47);}),_tag_dyneither(_tmp246,sizeof(void*),0));});{
int*span=({int*_tmp24E=_cycalloc_atomic(sizeof(*_tmp24E));_tmp24E[0]=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp268->impl))->fields);_tmp24E;});
struct Cyc_List_List*_tmp248=_tmp267;struct _dyneither_ptr*_tmp24D;struct Cyc_Absyn_Pat*_tmp24C;if(_tmp248 != 0){if(((struct _tuple15*)((struct Cyc_List_List*)_tmp248)->hd)->f1 != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)((struct _tuple15*)((struct Cyc_List_List*)_tmp248)->hd)->f1)->hd)->tag == 1){if(((struct Cyc_List_List*)((struct _tuple15*)((struct Cyc_List_List*)_tmp248)->hd)->f1)->tl == 0){if(((struct Cyc_List_List*)_tmp248)->tl == 0){_LL168: _tmp24D=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)(((struct _tuple15*)_tmp248->hd)->f1)->hd)->f1;_tmp24C=((struct _tuple15*)_tmp248->hd)->f2;_LL169:
# 1192
 s=Cyc_Tcpat_con_pat(*_tmp24D,span,({struct Cyc_List_List*_tmp249=_cycalloc(sizeof(*_tmp249));_tmp249->hd=Cyc_Tcpat_compile_pat(_tmp24C);_tmp249->tl=0;_tmp249;}),p);
goto _LL167;}else{goto _LL16A;}}else{goto _LL16A;}}else{goto _LL16A;}}else{goto _LL16A;}}else{_LL16A: _LL16B:
({void*_tmp24A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp24B="bad union pattern";_tag_dyneither(_tmp24B,sizeof(char),18);}),_tag_dyneither(_tmp24A,sizeof(void*),0));});}_LL167:;};}
# 1197
goto _LL125;}else{goto _LL148;}}else{_LL148: _LL149:
# 1239
 goto _LL14B;}case 13: _LL140: _tmp26A=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp229)->f1;_tmp269=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp229)->f2;_LL141:
# 1202
{void*_tmp24F=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp250=_tmp24F;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp250)->tag == 6){_LL16D: _LL16E:
# 1206
 s=Cyc_Tcpat_con_pat(*(*_tmp269->name).f2,0,0,p);
goto _LL16C;}else{_LL16F: _LL170: {
# 1209
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp26A->fields))->v);
s=Cyc_Tcpat_con_pat(*(*_tmp269->name).f2,({int*_tmp251=_cycalloc_atomic(sizeof(*_tmp251));_tmp251[0]=span;_tmp251;}),0,p);
goto _LL16C;}}_LL16C:;}
# 1213
goto _LL125;case 14: _LL142: _tmp26C=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp229)->f1;_tmp26B=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp229)->f2;_LL143: {
# 1218
struct Cyc_List_List*fields;
{void*_tmp252=Cyc_Tcutil_compress(_tmp26C);void*_tmp253=_tmp252;struct Cyc_List_List*_tmp256;if(((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp253)->tag == 14){_LL172: _tmp256=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp253)->f1;_LL173:
 fields=_tmp256;goto _LL171;}else{_LL174: _LL175:
({void*_tmp254=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp255="bad type in AnonEnum_p";_tag_dyneither(_tmp255,sizeof(char),23);}),_tag_dyneither(_tmp254,sizeof(void*),0));});}_LL171:;}
# 1225
{void*_tmp257=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp258=_tmp257;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp258)->tag == 6){_LL177: _LL178:
# 1229
 s=Cyc_Tcpat_con_pat(*(*_tmp26B->name).f2,0,0,p);
goto _LL176;}else{_LL179: _LL17A: {
# 1232
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);
s=Cyc_Tcpat_con_pat(*(*_tmp26B->name).f2,({int*_tmp259=_cycalloc_atomic(sizeof(*_tmp259));_tmp259[0]=span;_tmp259;}),0,p);
goto _LL176;}}_LL176:;}
# 1236
goto _LL125;}case 15: _LL144: _LL145:
 goto _LL147;case 16: _LL146: _LL147:
 goto _LL149;default: _LL14A: _LL14B:
# 1240
 s=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp25A=_cycalloc_atomic(sizeof(*_tmp25A));_tmp25A[0]=({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp25B;_tmp25B.tag=0;_tmp25B;});_tmp25A;});}_LL125:;}
# 1242
return s;}
# 1250
typedef struct Cyc_List_List*Cyc_Tcpat_match_t;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct{int tag;struct Cyc_Set_Set*f1;};
# 1264
typedef struct Cyc_List_List*Cyc_Tcpat_context_t;
# 1271
static int Cyc_Tcpat_same_access(void*a1,void*a2){
struct _tuple0 _tmp26D=({struct _tuple0 _tmp279;_tmp279.f1=a1;_tmp279.f2=a2;_tmp279;});struct _tuple0 _tmp26E=_tmp26D;int _tmp278;struct _dyneither_ptr*_tmp277;int _tmp276;struct _dyneither_ptr*_tmp275;struct Cyc_Absyn_Datatypefield*_tmp274;unsigned int _tmp273;struct Cyc_Absyn_Datatypefield*_tmp272;unsigned int _tmp271;unsigned int _tmp270;unsigned int _tmp26F;switch(*((int*)_tmp26E.f1)){case 0: if(((struct Cyc_Tcpat_Dummy_Tcpat_Access_struct*)_tmp26E.f2)->tag == 0){_LL17C: _LL17D:
 return 1;}else{goto _LL186;}case 1: if(((struct Cyc_Tcpat_Deref_Tcpat_Access_struct*)_tmp26E.f2)->tag == 1){_LL17E: _LL17F:
 return 1;}else{goto _LL186;}case 2: if(((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp26E.f2)->tag == 2){_LL180: _tmp270=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp26E.f1)->f1;_tmp26F=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp26E.f2)->f1;_LL181:
 return _tmp270 == _tmp26F;}else{goto _LL186;}case 3: if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp26E.f2)->tag == 3){_LL182: _tmp274=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp26E.f1)->f2;_tmp273=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp26E.f1)->f3;_tmp272=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp26E.f2)->f2;_tmp271=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp26E.f2)->f3;_LL183:
# 1277
 return _tmp274 == _tmp272  && _tmp273 == _tmp271;}else{goto _LL186;}default: if(((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp26E.f2)->tag == 4){_LL184: _tmp278=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp26E.f1)->f1;_tmp277=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp26E.f1)->f2;_tmp276=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp26E.f2)->f1;_tmp275=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp26E.f2)->f2;_LL185:
# 1279
 return _tmp278 == _tmp276  && Cyc_strptrcmp(_tmp277,_tmp275)== 0;}else{_LL186: _LL187:
 return 0;}}_LL17B:;}
# 1284
static int Cyc_Tcpat_same_path(struct Cyc_List_List*p1,struct Cyc_List_List*p2){
while(p1 != 0  && p2 != 0){
if(!Cyc_Tcpat_same_access(((struct Cyc_Tcpat_PathNode*)p1->hd)->access,((struct Cyc_Tcpat_PathNode*)p2->hd)->access))return 0;
p1=p1->tl;
p2=p2->tl;}
# 1290
if(p1 != p2)return 0;
return 1;}
# 1294
static void*Cyc_Tcpat_ifeq(struct Cyc_List_List*access,struct Cyc_Tcpat_Con_s*con,void*d1,void*d2){
void*_tmp27A=Cyc_Tcpat_get_pat_test(con->orig_pat);
{void*_tmp27B=d2;struct Cyc_List_List*_tmp282;struct Cyc_List_List*_tmp281;void*_tmp280;if(((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp27B)->tag == 2){_LL189: _tmp282=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp27B)->f1;_tmp281=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp27B)->f2;_tmp280=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp27B)->f3;_LL18A:
# 1298
 if(Cyc_Tcpat_same_path(access,_tmp282) && (int)(((con->orig_pat).pattern).tag == 1))
return(void*)({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_tmp27C=_cycalloc(sizeof(*_tmp27C));_tmp27C[0]=({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct _tmp27D;_tmp27D.tag=2;_tmp27D.f1=_tmp282;_tmp27D.f2=({struct Cyc_List_List*_tmp27E=_cycalloc(sizeof(*_tmp27E));_tmp27E->hd=({struct _tuple0*_tmp27F=_cycalloc(sizeof(*_tmp27F));_tmp27F->f1=_tmp27A;_tmp27F->f2=d1;_tmp27F;});_tmp27E->tl=_tmp281;_tmp27E;});_tmp27D.f3=_tmp280;_tmp27D;});_tmp27C;});else{
# 1302
goto _LL188;}}else{_LL18B: _LL18C:
 goto _LL188;}_LL188:;}
# 1305
return(void*)({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_tmp283=_cycalloc(sizeof(*_tmp283));_tmp283[0]=({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct _tmp284;_tmp284.tag=2;_tmp284.f1=access;_tmp284.f2=({struct Cyc_List_List*_tmp285=_cycalloc(sizeof(*_tmp285));_tmp285->hd=({struct _tuple0*_tmp286=_cycalloc(sizeof(*_tmp286));_tmp286->f1=_tmp27A;_tmp286->f2=d1;_tmp286;});_tmp285->tl=0;_tmp285;});_tmp284.f3=d2;_tmp284;});_tmp283;});}struct _tuple20{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 1311
typedef struct _tuple20*Cyc_Tcpat_work_stack_frame_t;
# 1314
typedef struct Cyc_List_List*Cyc_Tcpat_work_stack_t;
# 1316
enum Cyc_Tcpat_Answer{Cyc_Tcpat_Yes  = 0,Cyc_Tcpat_No  = 1,Cyc_Tcpat_Maybe  = 2};
typedef enum Cyc_Tcpat_Answer Cyc_Tcpat_answer_t;
# 1319
static void Cyc_Tcpat_print_tab(int i){
for(0;i != 0;-- i){
({void*_tmp287=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp288=" ";_tag_dyneither(_tmp288,sizeof(char),2);}),_tag_dyneither(_tmp287,sizeof(void*),0));});}}
# 1325
static void Cyc_Tcpat_print_con(struct Cyc_Tcpat_Con_s*c){
union Cyc_Tcpat_Name_value _tmp289=c->name;
union Cyc_Tcpat_Name_value _tmp28A=_tmp289;int _tmp292;struct _dyneither_ptr _tmp291;if((_tmp28A.Name_v).tag == 1){_LL18E: _tmp291=(_tmp28A.Name_v).val;_LL18F:
({struct Cyc_String_pa_PrintArg_struct _tmp28D;_tmp28D.tag=0;_tmp28D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp291);({void*_tmp28B[1]={& _tmp28D};Cyc_fprintf(Cyc_stderr,({const char*_tmp28C="%s";_tag_dyneither(_tmp28C,sizeof(char),3);}),_tag_dyneither(_tmp28B,sizeof(void*),1));});});goto _LL18D;}else{_LL190: _tmp292=(_tmp28A.Int_v).val;_LL191:
({struct Cyc_Int_pa_PrintArg_struct _tmp290;_tmp290.tag=1;_tmp290.f1=(unsigned long)_tmp292;({void*_tmp28E[1]={& _tmp290};Cyc_fprintf(Cyc_stderr,({const char*_tmp28F="%d";_tag_dyneither(_tmp28F,sizeof(char),3);}),_tag_dyneither(_tmp28E,sizeof(void*),1));});});goto _LL18D;}_LL18D:;}
# 1333
static void Cyc_Tcpat_print_access(void*a){
void*_tmp293=a;int _tmp2A9;struct _dyneither_ptr*_tmp2A8;struct Cyc_Absyn_Datatypefield*_tmp2A7;unsigned int _tmp2A6;unsigned int _tmp2A5;switch(*((int*)_tmp293)){case 0: _LL193: _LL194:
({void*_tmp294=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp295="DUMMY";_tag_dyneither(_tmp295,sizeof(char),6);}),_tag_dyneither(_tmp294,sizeof(void*),0));});goto _LL192;case 1: _LL195: _LL196:
({void*_tmp296=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp297="*";_tag_dyneither(_tmp297,sizeof(char),2);}),_tag_dyneither(_tmp296,sizeof(void*),0));});goto _LL192;case 2: _LL197: _tmp2A5=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp293)->f1;_LL198:
({struct Cyc_Int_pa_PrintArg_struct _tmp29A;_tmp29A.tag=1;_tmp29A.f1=(unsigned long)((int)_tmp2A5);({void*_tmp298[1]={& _tmp29A};Cyc_fprintf(Cyc_stderr,({const char*_tmp299="[%d]";_tag_dyneither(_tmp299,sizeof(char),5);}),_tag_dyneither(_tmp298,sizeof(void*),1));});});goto _LL192;case 3: _LL199: _tmp2A7=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp293)->f2;_tmp2A6=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp293)->f3;_LL19A:
# 1339
({struct Cyc_Int_pa_PrintArg_struct _tmp29E;_tmp29E.tag=1;_tmp29E.f1=(unsigned long)((int)_tmp2A6);({struct Cyc_String_pa_PrintArg_struct _tmp29D;_tmp29D.tag=0;_tmp29D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2A7->name));({void*_tmp29B[2]={& _tmp29D,& _tmp29E};Cyc_fprintf(Cyc_stderr,({const char*_tmp29C="%s[%d]";_tag_dyneither(_tmp29C,sizeof(char),7);}),_tag_dyneither(_tmp29B,sizeof(void*),2));});});});goto _LL192;default: _LL19B: _tmp2A9=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp293)->f1;_tmp2A8=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp293)->f2;_LL19C:
# 1341
 if(_tmp2A9)
({struct Cyc_String_pa_PrintArg_struct _tmp2A1;_tmp2A1.tag=0;_tmp2A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2A8);({void*_tmp29F[1]={& _tmp2A1};Cyc_fprintf(Cyc_stderr,({const char*_tmp2A0=".tagunion.%s";_tag_dyneither(_tmp2A0,sizeof(char),13);}),_tag_dyneither(_tmp29F,sizeof(void*),1));});});else{
# 1344
({struct Cyc_String_pa_PrintArg_struct _tmp2A4;_tmp2A4.tag=0;_tmp2A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2A8);({void*_tmp2A2[1]={& _tmp2A4};Cyc_fprintf(Cyc_stderr,({const char*_tmp2A3=".%s";_tag_dyneither(_tmp2A3,sizeof(char),4);}),_tag_dyneither(_tmp2A2,sizeof(void*),1));});});}
goto _LL192;}_LL192:;}
# 1349
static void Cyc_Tcpat_print_pat_test(void*p){
void*_tmp2AA=p;struct Cyc_Absyn_Datatypefield*_tmp2CF;struct _dyneither_ptr*_tmp2CE;int _tmp2CD;int _tmp2CC;unsigned int _tmp2CB;struct _dyneither_ptr _tmp2CA;struct Cyc_Absyn_Enumfield*_tmp2C9;struct Cyc_Absyn_Enumfield*_tmp2C8;struct Cyc_Absyn_Exp*_tmp2C7;switch(*((int*)_tmp2AA)){case 0: if(((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp2AA)->f1 == 0){_LL19E: _LL19F:
({void*_tmp2AB=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2AC="where(NULL)";_tag_dyneither(_tmp2AC,sizeof(char),12);}),_tag_dyneither(_tmp2AB,sizeof(void*),0));});goto _LL19D;}else{_LL1A0: _tmp2C7=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp2AA)->f1;_LL1A1:
({struct Cyc_String_pa_PrintArg_struct _tmp2AF;_tmp2AF.tag=0;_tmp2AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string((struct Cyc_Absyn_Exp*)_check_null(_tmp2C7)));({void*_tmp2AD[1]={& _tmp2AF};Cyc_fprintf(Cyc_stderr,({const char*_tmp2AE="where(%s)";_tag_dyneither(_tmp2AE,sizeof(char),10);}),_tag_dyneither(_tmp2AD,sizeof(void*),1));});});goto _LL19D;}case 1: _LL1A2: _LL1A3:
({void*_tmp2B0=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2B1="NULL";_tag_dyneither(_tmp2B1,sizeof(char),5);}),_tag_dyneither(_tmp2B0,sizeof(void*),0));});goto _LL19D;case 2: _LL1A4: _LL1A5:
({void*_tmp2B2=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2B3="NOT-NULL:";_tag_dyneither(_tmp2B3,sizeof(char),10);}),_tag_dyneither(_tmp2B2,sizeof(void*),0));});goto _LL19D;case 4: _LL1A6: _tmp2C8=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp2AA)->f2;_LL1A7:
 _tmp2C9=_tmp2C8;goto _LL1A9;case 3: _LL1A8: _tmp2C9=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp2AA)->f2;_LL1A9:
({struct Cyc_String_pa_PrintArg_struct _tmp2B6;_tmp2B6.tag=0;_tmp2B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2C9->name));({void*_tmp2B4[1]={& _tmp2B6};Cyc_fprintf(Cyc_stderr,({const char*_tmp2B5="%s";_tag_dyneither(_tmp2B5,sizeof(char),3);}),_tag_dyneither(_tmp2B4,sizeof(void*),1));});});goto _LL19D;case 5: _LL1AA: _tmp2CA=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp2AA)->f1;_LL1AB:
({struct Cyc_String_pa_PrintArg_struct _tmp2B9;_tmp2B9.tag=0;_tmp2B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2CA);({void*_tmp2B7[1]={& _tmp2B9};Cyc_fprintf(Cyc_stderr,({const char*_tmp2B8="%s";_tag_dyneither(_tmp2B8,sizeof(char),3);}),_tag_dyneither(_tmp2B7,sizeof(void*),1));});});goto _LL19D;case 6: _LL1AC: _tmp2CB=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp2AA)->f1;_LL1AD:
({struct Cyc_Int_pa_PrintArg_struct _tmp2BC;_tmp2BC.tag=1;_tmp2BC.f1=(unsigned long)((int)_tmp2CB);({void*_tmp2BA[1]={& _tmp2BC};Cyc_fprintf(Cyc_stderr,({const char*_tmp2BB="%d";_tag_dyneither(_tmp2BB,sizeof(char),3);}),_tag_dyneither(_tmp2BA,sizeof(void*),1));});});goto _LL19D;case 7: _LL1AE: _tmp2CC=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp2AA)->f1;_LL1AF:
({struct Cyc_Int_pa_PrintArg_struct _tmp2BF;_tmp2BF.tag=1;_tmp2BF.f1=(unsigned long)_tmp2CC;({void*_tmp2BD[1]={& _tmp2BF};Cyc_fprintf(Cyc_stderr,({const char*_tmp2BE="datatypetag(%d)";_tag_dyneither(_tmp2BE,sizeof(char),16);}),_tag_dyneither(_tmp2BD,sizeof(void*),1));});});goto _LL19D;case 8: _LL1B0: _tmp2CE=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp2AA)->f1;_tmp2CD=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp2AA)->f2;_LL1B1:
({struct Cyc_Int_pa_PrintArg_struct _tmp2C3;_tmp2C3.tag=1;_tmp2C3.f1=(unsigned long)_tmp2CD;({struct Cyc_String_pa_PrintArg_struct _tmp2C2;_tmp2C2.tag=0;_tmp2C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2CE);({void*_tmp2C0[2]={& _tmp2C2,& _tmp2C3};Cyc_fprintf(Cyc_stderr,({const char*_tmp2C1="uniontag[%s](%d)";_tag_dyneither(_tmp2C1,sizeof(char),17);}),_tag_dyneither(_tmp2C0,sizeof(void*),2));});});});goto _LL19D;default: _LL1B2: _tmp2CF=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp2AA)->f2;_LL1B3:
# 1362
({struct Cyc_String_pa_PrintArg_struct _tmp2C6;_tmp2C6.tag=0;_tmp2C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2CF->name));({void*_tmp2C4[1]={& _tmp2C6};Cyc_fprintf(Cyc_stderr,({const char*_tmp2C5="datatypefield(%s)";_tag_dyneither(_tmp2C5,sizeof(char),18);}),_tag_dyneither(_tmp2C4,sizeof(void*),1));});});}_LL19D:;}
# 1366
static void Cyc_Tcpat_print_rhs(struct Cyc_Tcpat_Rhs*r){
({struct Cyc_String_pa_PrintArg_struct _tmp2D2;_tmp2D2.tag=0;_tmp2D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(r->rhs));({void*_tmp2D0[1]={& _tmp2D2};Cyc_fprintf(Cyc_stderr,({const char*_tmp2D1="%s";_tag_dyneither(_tmp2D1,sizeof(char),3);}),_tag_dyneither(_tmp2D0,sizeof(void*),1));});});}
# 1370
static void Cyc_Tcpat_print_dec_tree(void*d,int tab){
void*_tmp2D3=d;struct Cyc_List_List*_tmp2EF;struct Cyc_List_List*_tmp2EE;void*_tmp2ED;struct Cyc_Tcpat_Rhs*_tmp2EC;switch(*((int*)_tmp2D3)){case 1: _LL1B5: _tmp2EC=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp2D3)->f1;_LL1B6:
# 1373
 Cyc_Tcpat_print_tab(tab);
({void*_tmp2D4=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2D5="Success(";_tag_dyneither(_tmp2D5,sizeof(char),9);}),_tag_dyneither(_tmp2D4,sizeof(void*),0));});Cyc_Tcpat_print_rhs(_tmp2EC);({void*_tmp2D6=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2D7=")\n";_tag_dyneither(_tmp2D7,sizeof(char),3);}),_tag_dyneither(_tmp2D6,sizeof(void*),0));});
goto _LL1B4;case 0: _LL1B7: _LL1B8:
({void*_tmp2D8=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2D9="Failure\n";_tag_dyneither(_tmp2D9,sizeof(char),9);}),_tag_dyneither(_tmp2D8,sizeof(void*),0));});goto _LL1B4;default: _LL1B9: _tmp2EF=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp2D3)->f1;_tmp2EE=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp2D3)->f2;_tmp2ED=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp2D3)->f3;_LL1BA:
# 1378
 Cyc_Tcpat_print_tab(tab);
({void*_tmp2DA=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2DB="Switch[";_tag_dyneither(_tmp2DB,sizeof(char),8);}),_tag_dyneither(_tmp2DA,sizeof(void*),0));});
for(0;_tmp2EF != 0;_tmp2EF=_tmp2EF->tl){
Cyc_Tcpat_print_access(((struct Cyc_Tcpat_PathNode*)_tmp2EF->hd)->access);
if(_tmp2EF->tl != 0)({void*_tmp2DC=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2DD=",";_tag_dyneither(_tmp2DD,sizeof(char),2);}),_tag_dyneither(_tmp2DC,sizeof(void*),0));});}
# 1384
({void*_tmp2DE=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2DF="] {\n";_tag_dyneither(_tmp2DF,sizeof(char),5);}),_tag_dyneither(_tmp2DE,sizeof(void*),0));});
for(0;_tmp2EE != 0;_tmp2EE=_tmp2EE->tl){
struct _tuple0 _tmp2E0=*((struct _tuple0*)_tmp2EE->hd);struct _tuple0 _tmp2E1=_tmp2E0;void*_tmp2E7;void*_tmp2E6;_LL1BC: _tmp2E7=_tmp2E1.f1;_tmp2E6=_tmp2E1.f2;_LL1BD:;
Cyc_Tcpat_print_tab(tab);
({void*_tmp2E2=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2E3="case ";_tag_dyneither(_tmp2E3,sizeof(char),6);}),_tag_dyneither(_tmp2E2,sizeof(void*),0));});
Cyc_Tcpat_print_pat_test(_tmp2E7);
({void*_tmp2E4=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2E5=":\n";_tag_dyneither(_tmp2E5,sizeof(char),3);}),_tag_dyneither(_tmp2E4,sizeof(void*),0));});
Cyc_Tcpat_print_dec_tree(_tmp2E6,tab + 7);}
# 1393
Cyc_Tcpat_print_tab(tab);
({void*_tmp2E8=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2E9="default:\n";_tag_dyneither(_tmp2E9,sizeof(char),10);}),_tag_dyneither(_tmp2E8,sizeof(void*),0));});
Cyc_Tcpat_print_dec_tree(_tmp2ED,tab + 7);
Cyc_Tcpat_print_tab(tab);
({void*_tmp2EA=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2EB="}\n";_tag_dyneither(_tmp2EB,sizeof(char),3);}),_tag_dyneither(_tmp2EA,sizeof(void*),0));});}_LL1B4:;}
# 1401
void Cyc_Tcpat_print_decision_tree(void*d){
Cyc_Tcpat_print_dec_tree(d,0);}
# 1409
static void*Cyc_Tcpat_add_neg(void*td,struct Cyc_Tcpat_Con_s*c){
void*_tmp2F0=td;struct Cyc_Set_Set*_tmp2F5;if(((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2F0)->tag == 1){_LL1BF: _tmp2F5=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2F0)->f1;_LL1C0:
# 1419
 return(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2F1=_cycalloc(sizeof(*_tmp2F1));_tmp2F1[0]=({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp2F2;_tmp2F2.tag=1;_tmp2F2.f1=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_insert)(_tmp2F5,c);_tmp2F2;});_tmp2F1;});}else{_LL1C1: _LL1C2:
({void*_tmp2F3=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2F4="add_neg called when td is Positive";_tag_dyneither(_tmp2F4,sizeof(char),35);}),_tag_dyneither(_tmp2F3,sizeof(void*),0));});}_LL1BE:;}
# 1426
static enum Cyc_Tcpat_Answer Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*td){
void*_tmp2F6=td;struct Cyc_Set_Set*_tmp2F8;struct Cyc_Tcpat_Con_s*_tmp2F7;if(((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2F6)->tag == 0){_LL1C4: _tmp2F7=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2F6)->f1;_LL1C5:
# 1430
 if(Cyc_Tcpat_compare_con(c,_tmp2F7)== 0)return Cyc_Tcpat_Yes;else{
return Cyc_Tcpat_No;}}else{_LL1C6: _tmp2F8=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2F6)->f1;_LL1C7:
# 1434
 if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2F8,c))return Cyc_Tcpat_No;else{
# 1437
if(c->span != 0  && *((int*)_check_null(c->span))== ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp2F8)+ 1)
return Cyc_Tcpat_Yes;else{
# 1440
return Cyc_Tcpat_Maybe;}}}_LL1C3:;}struct _tuple21{struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 1448
static struct Cyc_List_List*Cyc_Tcpat_augment(struct Cyc_List_List*ctxt,void*dsc){
struct Cyc_List_List*_tmp2F9=ctxt;struct Cyc_Tcpat_Con_s*_tmp2FF;struct Cyc_List_List*_tmp2FE;struct Cyc_List_List*_tmp2FD;if(_tmp2F9 == 0){_LL1C9: _LL1CA:
 return 0;}else{_LL1CB: _tmp2FF=((struct _tuple21*)_tmp2F9->hd)->f1;_tmp2FE=((struct _tuple21*)_tmp2F9->hd)->f2;_tmp2FD=_tmp2F9->tl;_LL1CC:
# 1452
 return({struct Cyc_List_List*_tmp2FA=_cycalloc(sizeof(*_tmp2FA));_tmp2FA->hd=({struct _tuple21*_tmp2FB=_cycalloc(sizeof(*_tmp2FB));_tmp2FB->f1=_tmp2FF;_tmp2FB->f2=({struct Cyc_List_List*_tmp2FC=_cycalloc(sizeof(*_tmp2FC));_tmp2FC->hd=dsc;_tmp2FC->tl=_tmp2FE;_tmp2FC;});_tmp2FB;});_tmp2FA->tl=_tmp2FD;_tmp2FA;});}_LL1C8:;}
# 1460
static struct Cyc_List_List*Cyc_Tcpat_norm_context(struct Cyc_List_List*ctxt){
struct Cyc_List_List*_tmp300=ctxt;struct Cyc_Tcpat_Con_s*_tmp307;struct Cyc_List_List*_tmp306;struct Cyc_List_List*_tmp305;if(_tmp300 == 0){_LL1CE: _LL1CF:
({void*_tmp301=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp302="norm_context: empty context";_tag_dyneither(_tmp302,sizeof(char),28);}),_tag_dyneither(_tmp301,sizeof(void*),0));});}else{_LL1D0: _tmp307=((struct _tuple21*)_tmp300->hd)->f1;_tmp306=((struct _tuple21*)_tmp300->hd)->f2;_tmp305=_tmp300->tl;_LL1D1:
# 1464
 return Cyc_Tcpat_augment(_tmp305,(void*)({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp303=_cycalloc(sizeof(*_tmp303));_tmp303[0]=({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp304;_tmp304.tag=0;_tmp304.f1=_tmp307;_tmp304.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp306);_tmp304;});_tmp303;}));}_LL1CD:;}
# 1473
static void*Cyc_Tcpat_build_desc(struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
# 1475
struct _tuple1 _tmp308=({struct _tuple1 _tmp315;_tmp315.f1=ctxt;_tmp315.f2=work;_tmp315;});struct _tuple1 _tmp309=_tmp308;struct Cyc_Tcpat_Con_s*_tmp314;struct Cyc_List_List*_tmp313;struct Cyc_List_List*_tmp312;struct Cyc_List_List*_tmp311;struct Cyc_List_List*_tmp310;if(_tmp309.f1 == 0){if(_tmp309.f2 == 0){_LL1D3: _LL1D4:
 return dsc;}else{_LL1D5: _LL1D6:
 goto _LL1D8;}}else{if(_tmp309.f2 == 0){_LL1D7: _LL1D8:
({void*_tmp30A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp30B="build_desc: ctxt and work don't match";_tag_dyneither(_tmp30B,sizeof(char),38);}),_tag_dyneither(_tmp30A,sizeof(void*),0));});}else{_LL1D9: _tmp314=((struct _tuple21*)(_tmp309.f1)->hd)->f1;_tmp313=((struct _tuple21*)(_tmp309.f1)->hd)->f2;_tmp312=(_tmp309.f1)->tl;_tmp311=((struct _tuple20*)(_tmp309.f2)->hd)->f3;_tmp310=(_tmp309.f2)->tl;_LL1DA: {
# 1480
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp30C=({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp30D=_cycalloc(sizeof(*_tmp30D));_tmp30D[0]=({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp30E;_tmp30E.tag=0;_tmp30E.f1=_tmp314;_tmp30E.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp313),({struct Cyc_List_List*_tmp30F=_cycalloc(sizeof(*_tmp30F));_tmp30F->hd=dsc;_tmp30F->tl=_tmp311;_tmp30F;}));_tmp30E;});_tmp30D;});
return Cyc_Tcpat_build_desc(_tmp312,(void*)_tmp30C,_tmp310);}}}_LL1D2:;}
# 1485
static void*Cyc_Tcpat_match(void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules);struct _tuple22{void*f1;struct Cyc_Tcpat_Rhs*f2;};
# 1492
static void*Cyc_Tcpat_or_match(void*dsc,struct Cyc_List_List*allmrules){
struct Cyc_List_List*_tmp316=allmrules;void*_tmp31B;struct Cyc_Tcpat_Rhs*_tmp31A;struct Cyc_List_List*_tmp319;if(_tmp316 == 0){_LL1DC: _LL1DD:
 return(void*)({struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp317=_cycalloc(sizeof(*_tmp317));_tmp317[0]=({struct Cyc_Tcpat_Failure_Tcpat_Decision_struct _tmp318;_tmp318.tag=0;_tmp318.f1=dsc;_tmp318;});_tmp317;});}else{_LL1DE: _tmp31B=((struct _tuple22*)_tmp316->hd)->f1;_tmp31A=((struct _tuple22*)_tmp316->hd)->f2;_tmp319=_tmp316->tl;_LL1DF:
# 1496
 return Cyc_Tcpat_match(_tmp31B,0,dsc,0,0,_tmp31A,_tmp319);}_LL1DB:;}
# 1501
static void*Cyc_Tcpat_match_compile(struct Cyc_List_List*allmrules){
return Cyc_Tcpat_or_match((void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp31C=_cycalloc(sizeof(*_tmp31C));_tmp31C[0]=({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp31D;_tmp31D.tag=1;_tmp31D.f1=Cyc_Tcpat_empty_con_set();_tmp31D;});_tmp31C;}),allmrules);}
# 1508
static void*Cyc_Tcpat_and_match(struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules){
# 1511
struct Cyc_List_List*_tmp31E=work;struct Cyc_List_List*_tmp336;struct Cyc_List_List*_tmp335;struct Cyc_List_List*_tmp334;struct Cyc_List_List*_tmp333;struct Cyc_List_List*_tmp332;if(_tmp31E == 0){_LL1E1: _LL1E2:
 return(void*)({struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp31F=_cycalloc(sizeof(*_tmp31F));_tmp31F[0]=({struct Cyc_Tcpat_Success_Tcpat_Decision_struct _tmp320;_tmp320.tag=1;_tmp320.f1=right_hand_side;_tmp320;});_tmp31F;});}else{if(((struct _tuple20*)((struct Cyc_List_List*)_tmp31E)->hd)->f1 == 0){if(((struct _tuple20*)((struct Cyc_List_List*)_tmp31E)->hd)->f2 == 0){if(((struct _tuple20*)((struct Cyc_List_List*)_tmp31E)->hd)->f3 == 0){_LL1E3: _tmp332=_tmp31E->tl;_LL1E4:
# 1514
 return Cyc_Tcpat_and_match(Cyc_Tcpat_norm_context(ctx),_tmp332,right_hand_side,rules);}else{goto _LL1E5;}}else{goto _LL1E5;}}else{_LL1E5: _tmp336=((struct _tuple20*)_tmp31E->hd)->f1;_tmp335=((struct _tuple20*)_tmp31E->hd)->f2;_tmp334=((struct _tuple20*)_tmp31E->hd)->f3;_tmp333=_tmp31E->tl;_LL1E6:
# 1516
 if((_tmp336 == 0  || _tmp335 == 0) || _tmp334 == 0)
({void*_tmp321=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp322="tcpat:and_match: malformed work frame";_tag_dyneither(_tmp322,sizeof(char),38);}),_tag_dyneither(_tmp321,sizeof(void*),0));});{
struct Cyc_List_List*_tmp323=_tmp336;struct Cyc_List_List*_tmp324=_tmp323;void*_tmp331;struct Cyc_List_List*_tmp330;_LL1E8: _tmp331=(void*)_tmp324->hd;_tmp330=_tmp324->tl;_LL1E9:;{
struct Cyc_List_List*_tmp325=_tmp335;struct Cyc_List_List*_tmp326=_tmp325;struct Cyc_List_List*_tmp32F;struct Cyc_List_List*_tmp32E;_LL1EB: _tmp32F=(struct Cyc_List_List*)_tmp326->hd;_tmp32E=_tmp326->tl;_LL1EC:;{
struct Cyc_List_List*_tmp327=_tmp334;struct Cyc_List_List*_tmp328=_tmp327;void*_tmp32D;struct Cyc_List_List*_tmp32C;_LL1EE: _tmp32D=(void*)_tmp328->hd;_tmp32C=_tmp328->tl;_LL1EF:;{
struct _tuple20*_tmp329=({struct _tuple20*_tmp32B=_cycalloc(sizeof(*_tmp32B));_tmp32B->f1=_tmp330;_tmp32B->f2=_tmp32E;_tmp32B->f3=_tmp32C;_tmp32B;});
return Cyc_Tcpat_match(_tmp331,_tmp32F,_tmp32D,ctx,({struct Cyc_List_List*_tmp32A=_cycalloc(sizeof(*_tmp32A));_tmp32A->hd=_tmp329;_tmp32A->tl=_tmp333;_tmp32A;}),right_hand_side,rules);};};};};}}_LL1E0:;}
# 1527
static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct Cyc_Tcpat_Con_s*pcon,void*dsc){
void*_tmp337=dsc;struct Cyc_List_List*_tmp33D;struct Cyc_Set_Set*_tmp33C;if(((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp337)->tag == 1){_LL1F1: _tmp33C=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp337)->f1;_LL1F2: {
# 1533
void*any=(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp33A=_cycalloc(sizeof(*_tmp33A));_tmp33A[0]=({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp33B;_tmp33B.tag=1;_tmp33B.f1=Cyc_Tcpat_empty_con_set();_tmp33B;});_tmp33A;});
struct Cyc_List_List*_tmp338=0;
{int i=0;for(0;i < pcon->arity;++ i){
_tmp338=({struct Cyc_List_List*_tmp339=_cycalloc(sizeof(*_tmp339));_tmp339->hd=any;_tmp339->tl=_tmp338;_tmp339;});}}
return _tmp338;}}else{_LL1F3: _tmp33D=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp337)->f2;_LL1F4:
 return _tmp33D;}_LL1F0:;}
# 1542
static void*Cyc_Tcpat_get_access(union Cyc_Tcpat_PatOrWhere pw,int i){
union Cyc_Tcpat_PatOrWhere _tmp33E=pw;struct Cyc_Absyn_Pat*_tmp35F;if((_tmp33E.where_clause).tag == 2){_LL1F6: _LL1F7:
 return(void*)& Cyc_Tcpat_Dummy_val;}else{_LL1F8: _tmp35F=(_tmp33E.pattern).val;_LL1F9: {
# 1546
void*_tmp33F=_tmp35F->r;void*_tmp340=_tmp33F;union Cyc_Absyn_AggrInfoU _tmp35E;struct Cyc_List_List*_tmp35D;struct Cyc_Absyn_Datatypedecl*_tmp35C;struct Cyc_Absyn_Datatypefield*_tmp35B;switch(*((int*)_tmp340)){case 6: _LL1FB: _LL1FC:
# 1548
 if(i != 0)
({void*_tmp341=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp344;_tmp344.tag=1;_tmp344.f1=(unsigned long)i;({void*_tmp342[1]={& _tmp344};Cyc_aprintf(({const char*_tmp343="get_access on pointer pattern with offset %d\n";_tag_dyneither(_tmp343,sizeof(char),46);}),_tag_dyneither(_tmp342,sizeof(void*),1));});}),_tag_dyneither(_tmp341,sizeof(void*),0));});
return(void*)& Cyc_Tcpat_Deref_val;case 5: _LL1FD: _LL1FE:
 return(void*)({struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*_tmp345=_cycalloc_atomic(sizeof(*_tmp345));_tmp345[0]=({struct Cyc_Tcpat_TupleField_Tcpat_Access_struct _tmp346;_tmp346.tag=2;_tmp346.f1=(unsigned int)i;_tmp346;});_tmp345;});case 8: _LL1FF: _tmp35C=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp340)->f1;_tmp35B=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp340)->f2;_LL200:
 return(void*)({struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*_tmp347=_cycalloc(sizeof(*_tmp347));_tmp347[0]=({struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct _tmp348;_tmp348.tag=3;_tmp348.f1=_tmp35C;_tmp348.f2=_tmp35B;_tmp348.f3=(unsigned int)i;_tmp348;});_tmp347;});case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp340)->f1 != 0){_LL201: _tmp35E=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp340)->f1)->aggr_info;_tmp35D=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp340)->f3;_LL202: {
# 1554
struct Cyc_Absyn_Aggrdecl*_tmp349=Cyc_Absyn_get_known_aggrdecl(_tmp35E);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp349->impl))->tagged){
struct Cyc_List_List*_tmp34A=(*((struct _tuple15*)((struct Cyc_List_List*)_check_null(_tmp35D))->hd)).f1;struct Cyc_List_List*_tmp34B=_tmp34A;struct _dyneither_ptr*_tmp352;if(_tmp34B != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)_tmp34B)->hd)->tag == 1){if(((struct Cyc_List_List*)_tmp34B)->tl == 0){_LL206: _tmp352=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp34B->hd)->f1;_LL207:
# 1558
 return(void*)({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_tmp34C=_cycalloc(sizeof(*_tmp34C));_tmp34C[0]=({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct _tmp34D;_tmp34D.tag=4;_tmp34D.f1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp349->impl))->tagged;_tmp34D.f2=_tmp352;_tmp34D;});_tmp34C;});}else{goto _LL208;}}else{goto _LL208;}}else{_LL208: _LL209:
({void*_tmp34E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp351;_tmp351.tag=0;_tmp351.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_pat2string(_tmp35F));({void*_tmp34F[1]={& _tmp351};Cyc_aprintf(({const char*_tmp350="get_access on bad aggr pattern: %s";_tag_dyneither(_tmp350,sizeof(char),35);}),_tag_dyneither(_tmp34F,sizeof(void*),1));});}),_tag_dyneither(_tmp34E,sizeof(void*),0));});}_LL205:;}{
# 1562
struct Cyc_List_List*_tmp353=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp349->impl))->fields;
int _tmp354=i;
for(0;i != 0;-- i){_tmp353=((struct Cyc_List_List*)_check_null(_tmp353))->tl;}
return(void*)({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_tmp355=_cycalloc(sizeof(*_tmp355));_tmp355[0]=({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct _tmp356;_tmp356.tag=4;_tmp356.f1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp349->impl))->tagged;_tmp356.f2=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp353))->hd)->name;_tmp356;});_tmp355;});};}}else{goto _LL203;}default: _LL203: _LL204:
({void*_tmp357=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp35A;_tmp35A.tag=0;_tmp35A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_pat2string(_tmp35F));({void*_tmp358[1]={& _tmp35A};Cyc_aprintf(({const char*_tmp359="get_access on bad pattern: %s";_tag_dyneither(_tmp359,sizeof(char),30);}),_tag_dyneither(_tmp358,sizeof(void*),1));});}),_tag_dyneither(_tmp357,sizeof(void*),0));});}_LL1FA:;}}_LL1F5:;}struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*f2;};
# 1572
static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple23*env,int i){
struct _tuple23*_tmp360=env;struct Cyc_List_List*_tmp364;struct Cyc_Tcpat_Con_s*_tmp363;_LL20B: _tmp364=_tmp360->f1;_tmp363=_tmp360->f2;_LL20C:;{
void*acc=Cyc_Tcpat_get_access(_tmp363->orig_pat,i);
return({struct Cyc_List_List*_tmp361=_cycalloc(sizeof(*_tmp361));_tmp361->hd=({struct Cyc_Tcpat_PathNode*_tmp362=_cycalloc(sizeof(*_tmp362));_tmp362->orig_pat=_tmp363->orig_pat;_tmp362->access=acc;_tmp362;});_tmp361->tl=_tmp364;_tmp361;});};}
# 1577
static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){
struct _tuple23 _tmp365=({struct _tuple23 _tmp366;_tmp366.f1=obj;_tmp366.f2=pcon;_tmp366;});
return((struct Cyc_List_List*(*)(int n,struct Cyc_List_List*(*f)(struct _tuple23*,int),struct _tuple23*env))Cyc_List_tabulate_c)(pcon->arity,Cyc_Tcpat_getoarg,& _tmp365);}
# 1585
static void*Cyc_Tcpat_match(void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules){
# 1589
void*_tmp367=p;struct Cyc_Tcpat_Con_s*_tmp375;struct Cyc_List_List*_tmp374;if(((struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*)_tmp367)->tag == 0){_LL20E: _LL20F:
# 1591
 return Cyc_Tcpat_and_match(Cyc_Tcpat_augment(ctx,dsc),work,right_hand_side,rules);}else{_LL210: _tmp375=((struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp367)->f1;_tmp374=((struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp367)->f2;_LL211: {
# 1593
enum Cyc_Tcpat_Answer _tmp368=Cyc_Tcpat_static_match(_tmp375,dsc);enum Cyc_Tcpat_Answer _tmp369=_tmp368;switch(_tmp369){case Cyc_Tcpat_Yes: _LL213: _LL214: {
# 1595
struct Cyc_List_List*ctx2=({struct Cyc_List_List*_tmp36C=_cycalloc(sizeof(*_tmp36C));_tmp36C->hd=({struct _tuple21*_tmp36D=_cycalloc(sizeof(*_tmp36D));_tmp36D->f1=_tmp375;_tmp36D->f2=0;_tmp36D;});_tmp36C->tl=ctx;_tmp36C;});
struct _tuple20*work_frame=({struct _tuple20*_tmp36B=_cycalloc(sizeof(*_tmp36B));_tmp36B->f1=_tmp374;_tmp36B->f2=Cyc_Tcpat_getoargs(_tmp375,obj);_tmp36B->f3=
Cyc_Tcpat_getdargs(_tmp375,dsc);_tmp36B;});
struct Cyc_List_List*work2=({struct Cyc_List_List*_tmp36A=_cycalloc(sizeof(*_tmp36A));_tmp36A->hd=work_frame;_tmp36A->tl=work;_tmp36A;});
return Cyc_Tcpat_and_match(ctx2,work2,right_hand_side,rules);}case Cyc_Tcpat_No: _LL215: _LL216:
# 1601
 return Cyc_Tcpat_or_match(Cyc_Tcpat_build_desc(ctx,dsc,work),rules);default: _LL217: _LL218: {
# 1603
struct Cyc_List_List*ctx2=({struct Cyc_List_List*_tmp372=_cycalloc(sizeof(*_tmp372));_tmp372->hd=({struct _tuple21*_tmp373=_cycalloc(sizeof(*_tmp373));_tmp373->f1=_tmp375;_tmp373->f2=0;_tmp373;});_tmp372->tl=ctx;_tmp372;});
struct _tuple20*work_frame=({struct _tuple20*_tmp371=_cycalloc(sizeof(*_tmp371));_tmp371->f1=_tmp374;_tmp371->f2=Cyc_Tcpat_getoargs(_tmp375,obj);_tmp371->f3=
Cyc_Tcpat_getdargs(_tmp375,dsc);_tmp371;});
struct Cyc_List_List*work2=({struct Cyc_List_List*_tmp370=_cycalloc(sizeof(*_tmp370));_tmp370->hd=work_frame;_tmp370->tl=work;_tmp370;});
void*_tmp36E=Cyc_Tcpat_and_match(ctx2,work2,right_hand_side,rules);
void*_tmp36F=Cyc_Tcpat_or_match(Cyc_Tcpat_build_desc(ctx,Cyc_Tcpat_add_neg(dsc,_tmp375),work),rules);
# 1610
return Cyc_Tcpat_ifeq(obj,_tmp375,_tmp36E,_tmp36F);}}_LL212:;}}_LL20D:;}
# 1620
static void Cyc_Tcpat_check_exhaust_overlap(void*d,void(*not_exhaust)(void*,void*),void*env1,void(*rhs_appears)(void*,struct Cyc_Tcpat_Rhs*),void*env2,int*exhaust_done){
# 1625
void*_tmp376=d;struct Cyc_List_List*_tmp37D;void*_tmp37C;struct Cyc_Tcpat_Rhs*_tmp37B;void*_tmp37A;switch(*((int*)_tmp376)){case 0: _LL21A: _tmp37A=(void*)((struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp376)->f1;_LL21B:
# 1627
 if(!(*exhaust_done)){not_exhaust(env1,_tmp37A);*exhaust_done=1;}
goto _LL219;case 1: _LL21C: _tmp37B=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp376)->f1;_LL21D:
 rhs_appears(env2,_tmp37B);goto _LL219;default: _LL21E: _tmp37D=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp376)->f2;_tmp37C=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp376)->f3;_LL21F:
# 1631
 for(0;_tmp37D != 0;_tmp37D=_tmp37D->tl){
struct _tuple0 _tmp377=*((struct _tuple0*)_tmp37D->hd);struct _tuple0 _tmp378=_tmp377;void*_tmp379;_LL221: _tmp379=_tmp378.f2;_LL222:;
Cyc_Tcpat_check_exhaust_overlap(_tmp379,not_exhaust,env1,rhs_appears,env2,exhaust_done);}
# 1636
Cyc_Tcpat_check_exhaust_overlap(_tmp37C,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1638
goto _LL219;}_LL219:;}
# 1646
static struct _tuple22*Cyc_Tcpat_get_match(int*ctr,struct Cyc_Absyn_Switch_clause*swc){
# 1648
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp386=_cycalloc(sizeof(*_tmp386));_tmp386->used=0;_tmp386->pat_loc=(swc->pattern)->loc;_tmp386->rhs=swc->body;_tmp386;});
void*sp;
if(swc->where_clause != 0){
union Cyc_Tcpat_PatOrWhere _tmp37E=({union Cyc_Tcpat_PatOrWhere _tmp380;(_tmp380.where_clause).val=swc->where_clause;(_tmp380.where_clause).tag=2;_tmp380;});
sp=Cyc_Tcpat_tuple_pat(({void*_tmp37F[2];_tmp37F[1]=Cyc_Tcpat_int_pat(*ctr,_tmp37E);_tmp37F[0]=sp0;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp37F,sizeof(void*),2));}),_tmp37E);
*ctr=*ctr + 1;}else{
# 1656
sp=Cyc_Tcpat_tuple_pat(({void*_tmp381[2];_tmp381[1]=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp382=_cycalloc_atomic(sizeof(*_tmp382));_tmp382[0]=({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp383;_tmp383.tag=0;_tmp383;});_tmp382;});_tmp381[0]=sp0;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp381,sizeof(void*),2));}),({union Cyc_Tcpat_PatOrWhere _tmp384;(_tmp384.where_clause).val=0;(_tmp384.where_clause).tag=2;_tmp384;}));}
return({struct _tuple22*_tmp385=_cycalloc(sizeof(*_tmp385));_tmp385->f1=sp;_tmp385->f2=rhs;_tmp385;});}char Cyc_Tcpat_Desc2string[12]="Desc2string";struct Cyc_Tcpat_Desc2string_exn_struct{char*tag;};
# 1662
struct Cyc_Tcpat_Desc2string_exn_struct Cyc_Tcpat_Desc2string_val={Cyc_Tcpat_Desc2string};
# 1664
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*);
static struct _dyneither_ptr Cyc_Tcpat_desc2string(void*d){
void*_tmp388=d;struct Cyc_Set_Set*_tmp3E9;struct Cyc_Tcpat_Con_s*_tmp3E8;struct Cyc_List_List*_tmp3E7;if(((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp388)->tag == 0){_LL224: _tmp3E8=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp388)->f1;_tmp3E7=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp388)->f2;_LL225: {
# 1668
union Cyc_Tcpat_Name_value _tmp389=_tmp3E8->name;
struct Cyc_Absyn_Pat*p;
{union Cyc_Tcpat_PatOrWhere _tmp38A=_tmp3E8->orig_pat;union Cyc_Tcpat_PatOrWhere _tmp38B=_tmp38A;struct Cyc_Absyn_Pat*_tmp38C;if((_tmp38B.where_clause).tag == 2){_LL229: _LL22A:
 return Cyc_Tcpat_desc2string((void*)((struct Cyc_List_List*)_check_null(_tmp3E7))->hd);}else{_LL22B: _tmp38C=(_tmp38B.pattern).val;_LL22C:
 p=_tmp38C;goto _LL228;}_LL228:;}{
# 1674
void*_tmp38D=p->r;void*_tmp38E=_tmp38D;struct Cyc_Absyn_Exp*_tmp3C3;struct Cyc_Absyn_Enumfield*_tmp3C2;struct Cyc_Absyn_Enumfield*_tmp3C1;struct _dyneither_ptr _tmp3C0;int _tmp3BF;char _tmp3BE;int _tmp3BD;struct Cyc_Absyn_Datatypefield*_tmp3BC;struct Cyc_Absyn_Aggrdecl*_tmp3BB;struct Cyc_List_List*_tmp3BA;struct Cyc_Absyn_Tvar*_tmp3B9;struct Cyc_Absyn_Vardecl*_tmp3B8;struct Cyc_Absyn_Vardecl*_tmp3B7;struct Cyc_Absyn_Vardecl*_tmp3B6;switch(*((int*)_tmp38E)){case 0: _LL22E: _LL22F:
 return({const char*_tmp38F="_";_tag_dyneither(_tmp38F,sizeof(char),2);});case 1: _LL230: _tmp3B6=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp38E)->f1;_LL231:
 return Cyc_Absynpp_qvar2string(_tmp3B6->name);case 3: _LL232: _tmp3B7=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp38E)->f1;_LL233:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp392;_tmp392.tag=0;_tmp392.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp3B7->name));({void*_tmp390[1]={& _tmp392};Cyc_aprintf(({const char*_tmp391="*%s";_tag_dyneither(_tmp391,sizeof(char),4);}),_tag_dyneither(_tmp390,sizeof(void*),1));});});case 4: _LL234: _tmp3B9=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp38E)->f1;_tmp3B8=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp38E)->f2;_LL235:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp396;_tmp396.tag=0;_tmp396.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3B9->name);({struct Cyc_String_pa_PrintArg_struct _tmp395;_tmp395.tag=0;_tmp395.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp3B8->name));({void*_tmp393[2]={& _tmp395,& _tmp396};Cyc_aprintf(({const char*_tmp394="%s<`%s>";_tag_dyneither(_tmp394,sizeof(char),8);}),_tag_dyneither(_tmp393,sizeof(void*),2));});});});case 5: _LL236: _LL237:
# 1680
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp399;_tmp399.tag=0;_tmp399.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(_tmp3E7));({void*_tmp397[1]={& _tmp399};Cyc_aprintf(({const char*_tmp398="$(%s)";_tag_dyneither(_tmp398,sizeof(char),6);}),_tag_dyneither(_tmp397,sizeof(void*),1));});});case 6: _LL238: _LL239:
# 1682
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp39C;_tmp39C.tag=0;_tmp39C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(_tmp3E7));({void*_tmp39A[1]={& _tmp39C};Cyc_aprintf(({const char*_tmp39B="&%s";_tag_dyneither(_tmp39B,sizeof(char),4);}),_tag_dyneither(_tmp39A,sizeof(void*),1));});});case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp38E)->f1 != 0){if(((((struct Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp38E)->f1)->aggr_info).KnownAggr).tag == 2){_LL23A: _tmp3BB=*(((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp38E)->f1)->aggr_info).KnownAggr).val;_tmp3BA=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp38E)->f3;_LL23B:
# 1684
 if(_tmp3BB->kind == Cyc_Absyn_UnionA){
struct Cyc_List_List*_tmp39D=_tmp3BA;struct _dyneither_ptr*_tmp3A3;if(_tmp39D != 0){if(((struct _tuple15*)((struct Cyc_List_List*)_tmp39D)->hd)->f1 != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)((struct _tuple15*)((struct Cyc_List_List*)_tmp39D)->hd)->f1)->hd)->tag == 1){_LL24F: _tmp3A3=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)(((struct _tuple15*)_tmp39D->hd)->f1)->hd)->f1;_LL250:
# 1687
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3A2;_tmp3A2.tag=0;_tmp3A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Tcpat_descs2string(_tmp3E7));({struct Cyc_String_pa_PrintArg_struct _tmp3A1;_tmp3A1.tag=0;_tmp3A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3A3);({struct Cyc_String_pa_PrintArg_struct _tmp3A0;_tmp3A0.tag=0;_tmp3A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1687
Cyc_Absynpp_qvar2string(_tmp3BB->name));({void*_tmp39E[3]={& _tmp3A0,& _tmp3A1,& _tmp3A2};Cyc_aprintf(({const char*_tmp39F="%s{.%s = %s}";_tag_dyneither(_tmp39F,sizeof(char),13);}),_tag_dyneither(_tmp39E,sizeof(void*),3));});});});});}else{goto _LL251;}}else{goto _LL251;}}else{_LL251: _LL252:
# 1689
 goto _LL24E;}_LL24E:;}
# 1692
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3A7;_tmp3A7.tag=0;_tmp3A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(_tmp3E7));({struct Cyc_String_pa_PrintArg_struct _tmp3A6;_tmp3A6.tag=0;_tmp3A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp3BB->name));({void*_tmp3A4[2]={& _tmp3A6,& _tmp3A7};Cyc_aprintf(({const char*_tmp3A5="%s{%s}";_tag_dyneither(_tmp3A5,sizeof(char),7);}),_tag_dyneither(_tmp3A4,sizeof(void*),2));});});});}else{goto _LL24C;}}else{goto _LL24C;}case 8: _LL23C: _tmp3BC=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp38E)->f2;_LL23D:
# 1694
 if(_tmp3E7 == 0)
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3AA;_tmp3AA.tag=0;_tmp3AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp3BC->name));({void*_tmp3A8[1]={& _tmp3AA};Cyc_aprintf(({const char*_tmp3A9="%s";_tag_dyneither(_tmp3A9,sizeof(char),3);}),_tag_dyneither(_tmp3A8,sizeof(void*),1));});});else{
# 1697
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3AE;_tmp3AE.tag=0;_tmp3AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(_tmp3E7));({struct Cyc_String_pa_PrintArg_struct _tmp3AD;_tmp3AD.tag=0;_tmp3AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp3BC->name));({void*_tmp3AB[2]={& _tmp3AD,& _tmp3AE};Cyc_aprintf(({const char*_tmp3AC="%s(%s)";_tag_dyneither(_tmp3AC,sizeof(char),7);}),_tag_dyneither(_tmp3AB,sizeof(void*),2));});});});}case 9: _LL23E: _LL23F:
 return({const char*_tmp3AF="NULL";_tag_dyneither(_tmp3AF,sizeof(char),5);});case 10: _LL240: _tmp3BD=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp38E)->f2;_LL241:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3B2;_tmp3B2.tag=1;_tmp3B2.f1=(unsigned long)_tmp3BD;({void*_tmp3B0[1]={& _tmp3B2};Cyc_aprintf(({const char*_tmp3B1="%d";_tag_dyneither(_tmp3B1,sizeof(char),3);}),_tag_dyneither(_tmp3B0,sizeof(void*),1));});});case 11: _LL242: _tmp3BE=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp38E)->f1;_LL243:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3B5;_tmp3B5.tag=1;_tmp3B5.f1=(unsigned long)((int)_tmp3BE);({void*_tmp3B3[1]={& _tmp3B5};Cyc_aprintf(({const char*_tmp3B4="%d";_tag_dyneither(_tmp3B4,sizeof(char),3);}),_tag_dyneither(_tmp3B3,sizeof(void*),1));});});case 12: _LL244: _tmp3C0=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp38E)->f1;_tmp3BF=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp38E)->f2;_LL245:
 return _tmp3C0;case 13: _LL246: _tmp3C1=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp38E)->f2;_LL247:
 _tmp3C2=_tmp3C1;goto _LL249;case 14: _LL248: _tmp3C2=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp38E)->f2;_LL249:
 return Cyc_Absynpp_qvar2string(_tmp3C2->name);case 17: _LL24A: _tmp3C3=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp38E)->f1;_LL24B:
 return Cyc_Absynpp_exp2string(_tmp3C3);default: _LL24C: _LL24D:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LL22D:;};}}else{_LL226: _tmp3E9=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp388)->f1;_LL227:
# 1708
 if(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_is_empty)(_tmp3E9))return({const char*_tmp3C4="_";_tag_dyneither(_tmp3C4,sizeof(char),2);});{
struct Cyc_Tcpat_Con_s*_tmp3C5=((struct Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp3E9);
# 1712
if((int)(((_tmp3C5->orig_pat).where_clause).tag == 2))(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_Absyn_Pat*_tmp3C6=({union Cyc_Tcpat_PatOrWhere _tmp3E6=_tmp3C5->orig_pat;if((_tmp3E6.pattern).tag != 1)_throw_match();(_tmp3E6.pattern).val;});
void*_tmp3C7=Cyc_Tcutil_compress((void*)_check_null(_tmp3C6->topt));void*_tmp3C8=_tmp3C7;struct Cyc_Absyn_Aggrdecl*_tmp3E5;struct Cyc_Absyn_Datatypedecl*_tmp3E4;struct Cyc_Absyn_PtrAtts _tmp3E3;switch(*((int*)_tmp3C8)){case 6: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3C8)->f2 == Cyc_Absyn_Char_sz){_LL254: _LL255:
# 1717
{int i=0;for(0;i < 256;++ i){
struct Cyc_Tcpat_Con_s*_tmp3C9=Cyc_Tcpat_char_con((char)i,_tmp3C6);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp3E9,_tmp3C9))
return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3CC;_tmp3CC.tag=1;_tmp3CC.f1=(unsigned long)i;({void*_tmp3CA[1]={& _tmp3CC};Cyc_aprintf(({const char*_tmp3CB="%d";_tag_dyneither(_tmp3CB,sizeof(char),3);}),_tag_dyneither(_tmp3CA,sizeof(void*),1));});});}}
# 1722
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}else{_LL256: _LL257:
# 1725
{unsigned int i=0;for(0;i < -1;++ i){
struct Cyc_Tcpat_Con_s*_tmp3CD=Cyc_Tcpat_int_con((int)i,_tmp3C5->orig_pat);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp3E9,_tmp3CD))
return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3D0;_tmp3D0.tag=1;_tmp3D0.f1=(unsigned long)((int)i);({void*_tmp3CE[1]={& _tmp3D0};Cyc_aprintf(({const char*_tmp3CF="%d";_tag_dyneither(_tmp3CF,sizeof(char),3);}),_tag_dyneither(_tmp3CE,sizeof(void*),1));});});}}
# 1730
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}case 5: _LL258: _tmp3E3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3C8)->f1).ptr_atts;_LL259: {
# 1732
union Cyc_Absyn_Constraint*_tmp3D1=_tmp3E3.nullable;
int is_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3D1);
if(is_nullable){
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp3E9,Cyc_Tcpat_null_con(_tmp3C6)))
return({const char*_tmp3D2="NULL";_tag_dyneither(_tmp3D2,sizeof(char),5);});}
# 1738
return({const char*_tmp3D3="&_";_tag_dyneither(_tmp3D3,sizeof(char),3);});}case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3C8)->f1).datatype_info).KnownDatatype).tag == 2){_LL25A: _tmp3E4=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3C8)->f1).datatype_info).KnownDatatype).val;_LL25B:
# 1740
 if(_tmp3E4->is_extensible)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_List_List*_tmp3D4=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3E4->fields))->v;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp3D4);
for(0;(unsigned int)_tmp3D4;_tmp3D4=_tmp3D4->tl){
struct _dyneither_ptr n=*(*((struct Cyc_Absyn_Datatypefield*)_tmp3D4->hd)->name).f2;
struct Cyc_List_List*_tmp3D5=((struct Cyc_Absyn_Datatypefield*)_tmp3D4->hd)->typs;
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp3E9,({struct Cyc_Tcpat_Con_s*_tmp3D6=_cycalloc(sizeof(*_tmp3D6));_tmp3D6->name=Cyc_Tcpat_Name_v(n);_tmp3D6->arity=0;_tmp3D6->span=0;_tmp3D6->orig_pat=_tmp3C5->orig_pat;_tmp3D6;}))){
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp3D5)== 0)
return n;else{
# 1750
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3D9;_tmp3D9.tag=0;_tmp3D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n);({void*_tmp3D7[1]={& _tmp3D9};Cyc_aprintf(({const char*_tmp3D8="%s(...)";_tag_dyneither(_tmp3D8,sizeof(char),8);}),_tag_dyneither(_tmp3D7,sizeof(void*),1));});});}}}
# 1753
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);};}else{goto _LL25E;}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3C8)->f1).aggr_info).KnownAggr).tag == 2){_LL25C: _tmp3E5=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3C8)->f1).aggr_info).KnownAggr).val;if(_tmp3E5->kind == Cyc_Absyn_UnionA){_LL25D: {
# 1755
struct Cyc_List_List*_tmp3DA=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3E5->impl))->fields;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp3DA);
struct _tuple2*_tmp3DB=_tmp3E5->name;struct _tuple2*_tmp3DC=_tmp3DB;struct _dyneither_ptr _tmp3E2;_LL261: _tmp3E2=*_tmp3DC->f2;_LL262:;
for(0;(unsigned int)_tmp3DA;_tmp3DA=_tmp3DA->tl){
struct _dyneither_ptr n=*((struct Cyc_Absyn_Aggrfield*)_tmp3DA->hd)->name;
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp3E9,({struct Cyc_Tcpat_Con_s*_tmp3DD=_cycalloc(sizeof(*_tmp3DD));_tmp3DD->name=Cyc_Tcpat_Name_v(n);_tmp3DD->arity=0;_tmp3DD->span=0;_tmp3DD->orig_pat=_tmp3C5->orig_pat;_tmp3DD;})))
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3E1;_tmp3E1.tag=0;_tmp3E1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n);({struct Cyc_String_pa_PrintArg_struct _tmp3E0;_tmp3E0.tag=0;_tmp3E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3E2);({void*_tmp3DE[2]={& _tmp3E0,& _tmp3E1};Cyc_aprintf(({const char*_tmp3DF="%s{.%s = _}";_tag_dyneither(_tmp3DF,sizeof(char),12);}),_tag_dyneither(_tmp3DE,sizeof(void*),2));});});});}
# 1763
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}}else{goto _LL25E;}}else{goto _LL25E;}default: _LL25E: _LL25F:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LL253:;};};}_LL223:;}
# 1768
static struct _dyneither_ptr*Cyc_Tcpat_desc2stringptr(void*d){
return({struct _dyneither_ptr*_tmp3EA=_cycalloc(sizeof(*_tmp3EA));_tmp3EA[0]=Cyc_Tcpat_desc2string(d);_tmp3EA;});}
# 1771
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*ds){
struct Cyc_List_List*_tmp3EB=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_desc2stringptr,ds);
struct _dyneither_ptr*comma=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),",",sizeof(char),2);
{struct Cyc_List_List*_tmp3EC=_tmp3EB;for(0;_tmp3EC != 0;_tmp3EC=((struct Cyc_List_List*)_check_null(_tmp3EC))->tl){
if(_tmp3EC->tl != 0){
_tmp3EC->tl=({struct Cyc_List_List*_tmp3ED=_cycalloc(sizeof(*_tmp3ED));_tmp3ED->hd=comma;_tmp3ED->tl=_tmp3EC->tl;_tmp3ED;});
_tmp3EC=_tmp3EC->tl;}}}
# 1780
return(struct _dyneither_ptr)Cyc_strconcat_l(_tmp3EB);}
# 1783
static void Cyc_Tcpat_not_exhaust_err(unsigned int loc,void*desc){
struct _handler_cons _tmp3EF;_push_handler(& _tmp3EF);{int _tmp3F1=0;if(setjmp(_tmp3EF.handler))_tmp3F1=1;if(!_tmp3F1){
{struct _dyneither_ptr _tmp3F2=Cyc_Tcpat_desc2string(desc);
({struct Cyc_String_pa_PrintArg_struct _tmp3F5;_tmp3F5.tag=0;_tmp3F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3F2);({void*_tmp3F3[1]={& _tmp3F5};Cyc_Tcutil_terr(loc,({const char*_tmp3F4="patterns may not be exhaustive.\n\tmissing case for %s";_tag_dyneither(_tmp3F4,sizeof(char),53);}),_tag_dyneither(_tmp3F3,sizeof(void*),1));});});}
# 1785
;_pop_handler();}else{void*_tmp3F0=(void*)_exn_thrown;void*_tmp3F6=_tmp3F0;void*_tmp3F9;if(((struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp3F6)->tag == Cyc_Tcpat_Desc2string){_LL264: _LL265:
# 1789
({void*_tmp3F7=0;Cyc_Tcutil_terr(loc,({const char*_tmp3F8="patterns may not be exhaustive.";_tag_dyneither(_tmp3F8,sizeof(char),32);}),_tag_dyneither(_tmp3F7,sizeof(void*),0));});
goto _LL263;}else{_LL266: _tmp3F9=_tmp3F6;_LL267:(int)_rethrow(_tmp3F9);}_LL263:;}};}
# 1793
static void Cyc_Tcpat_rule_occurs(int dummy,struct Cyc_Tcpat_Rhs*rhs){
rhs->used=1;}
# 1797
void Cyc_Tcpat_check_switch_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs,void**dopt){
# 1803
int _tmp3FA=0;
int*_tmp3FB=& _tmp3FA;
struct Cyc_List_List*_tmp3FC=((struct Cyc_List_List*(*)(struct _tuple22*(*f)(int*,struct Cyc_Absyn_Switch_clause*),int*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcpat_get_match,_tmp3FB,swcs);
void*_tmp3FD=Cyc_Tcpat_match_compile(_tmp3FC);
*dopt=_tmp3FD;{
# 1809
int _tmp3FE=0;
((void(*)(void*d,void(*not_exhaust)(unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct Cyc_Tcpat_Rhs*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp3FD,Cyc_Tcpat_not_exhaust_err,loc,Cyc_Tcpat_rule_occurs,0,& _tmp3FE);
# 1812
for(0;_tmp3FC != 0;_tmp3FC=_tmp3FC->tl){
struct _tuple22*_tmp3FF=(struct _tuple22*)_tmp3FC->hd;struct _tuple22*_tmp400=_tmp3FF;int _tmp404;unsigned int _tmp403;_LL269: _tmp404=(_tmp400->f2)->used;_tmp403=(_tmp400->f2)->pat_loc;_LL26A:;
if(!_tmp404){
({void*_tmp401=0;Cyc_Tcutil_terr(_tmp403,({const char*_tmp402="redundant pattern (check for misspelled constructors in earlier patterns)";_tag_dyneither(_tmp402,sizeof(char),74);}),_tag_dyneither(_tmp401,sizeof(void*),0));});
# 1817
break;}}};}
# 1826
static void Cyc_Tcpat_not_exhaust_warn(struct _tuple14*pr,void*desc){
(*pr).f2=0;{
struct _handler_cons _tmp405;_push_handler(& _tmp405);{int _tmp407=0;if(setjmp(_tmp405.handler))_tmp407=1;if(!_tmp407){
{struct _dyneither_ptr _tmp408=Cyc_Tcpat_desc2string(desc);
({struct Cyc_String_pa_PrintArg_struct _tmp40B;_tmp40B.tag=0;_tmp40B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp408);({void*_tmp409[1]={& _tmp40B};Cyc_Tcutil_warn((*pr).f1,({const char*_tmp40A="pattern not exhaustive.\n\tmissing case for %s";_tag_dyneither(_tmp40A,sizeof(char),45);}),_tag_dyneither(_tmp409,sizeof(void*),1));});});}
# 1829
;_pop_handler();}else{void*_tmp406=(void*)_exn_thrown;void*_tmp40C=_tmp406;void*_tmp40F;if(((struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp40C)->tag == Cyc_Tcpat_Desc2string){_LL26C: _LL26D:
# 1833
({void*_tmp40D=0;Cyc_Tcutil_warn((*pr).f1,({const char*_tmp40E="pattern not exhaustive.";_tag_dyneither(_tmp40E,sizeof(char),24);}),_tag_dyneither(_tmp40D,sizeof(void*),0));});
goto _LL26B;}else{_LL26E: _tmp40F=_tmp40C;_LL26F:(int)_rethrow(_tmp40F);}_LL26B:;}};};}
# 1837
static void Cyc_Tcpat_dummy_fn(int i,struct Cyc_Tcpat_Rhs*rhs){
return;}
# 1841
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**dopt){
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp417=_cycalloc(sizeof(*_tmp417));_tmp417->used=0;_tmp417->pat_loc=p->loc;_tmp417->rhs=Cyc_Absyn_skip_stmt(0);_tmp417;});
struct Cyc_List_List*_tmp410=({struct Cyc_List_List*_tmp415=_cycalloc(sizeof(*_tmp415));_tmp415->hd=({struct _tuple22*_tmp416=_cycalloc(sizeof(*_tmp416));_tmp416->f1=Cyc_Tcpat_compile_pat(p);_tmp416->f2=rhs;_tmp416;});_tmp415->tl=0;_tmp415;});
void*_tmp411=Cyc_Tcpat_match_compile(_tmp410);
struct _tuple14 _tmp412=({struct _tuple14 _tmp414;_tmp414.f1=loc;_tmp414.f2=1;_tmp414;});
int _tmp413=0;
((void(*)(void*d,void(*not_exhaust)(struct _tuple14*,void*),struct _tuple14*env1,void(*rhs_appears)(int,struct Cyc_Tcpat_Rhs*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp411,Cyc_Tcpat_not_exhaust_warn,& _tmp412,Cyc_Tcpat_dummy_fn,0,& _tmp413);
# 1849
*dopt=_tmp411;
return _tmp412.f2;}
# 1857
static struct _tuple22*Cyc_Tcpat_get_match2(struct Cyc_Absyn_Switch_clause*swc){
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp419=_cycalloc(sizeof(*_tmp419));_tmp419->used=0;_tmp419->pat_loc=(swc->pattern)->loc;_tmp419->rhs=swc->body;_tmp419;});
return({struct _tuple22*_tmp418=_cycalloc(sizeof(*_tmp418));_tmp418->f1=sp0;_tmp418->f2=rhs;_tmp418;});}
# 1862
static void Cyc_Tcpat_not_exhaust_err2(unsigned int loc,void*d){;}
# 1865
void Cyc_Tcpat_check_catch_overlap(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs,void**dopt){
# 1869
struct Cyc_List_List*_tmp41A=((struct Cyc_List_List*(*)(struct _tuple22*(*f)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_get_match2,swcs);
void*_tmp41B=Cyc_Tcpat_match_compile(_tmp41A);
*dopt=_tmp41B;{
int _tmp41C=0;
((void(*)(void*d,void(*not_exhaust)(unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct Cyc_Tcpat_Rhs*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp41B,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,0,& _tmp41C);
# 1875
for(0;_tmp41A != 0;_tmp41A=_tmp41A->tl){
# 1877
if(_tmp41A->tl == 0)break;{
struct _tuple22*_tmp41D=(struct _tuple22*)_tmp41A->hd;struct _tuple22*_tmp41E=_tmp41D;int _tmp422;unsigned int _tmp421;_LL271: _tmp422=(_tmp41E->f2)->used;_tmp421=(_tmp41E->f2)->pat_loc;_LL272:;
if(!_tmp422){
({void*_tmp41F=0;Cyc_Tcutil_terr(_tmp421,({const char*_tmp420="redundant pattern (check for misspelled constructors in earlier patterns)";_tag_dyneither(_tmp420,sizeof(char),74);}),_tag_dyneither(_tmp41F,sizeof(void*),0));});
break;}};}};}
# 1886
int Cyc_Tcpat_has_vars(struct Cyc_Core_Opt*pat_vars){
{struct Cyc_List_List*_tmp423=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(pat_vars))->v;for(0;_tmp423 != 0;_tmp423=_tmp423->tl){
if((*((struct _tuple16*)_tmp423->hd)).f1 != 0)
return 1;}}
return 0;}
