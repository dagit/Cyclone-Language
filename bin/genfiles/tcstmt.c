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
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;};
# 110 "core.h"
void*Cyc_Core_fst(struct _tuple0*);
# 113
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
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple1 Cyc_List_split(struct Cyc_List_List*x);
# 391
struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
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
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 369
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 390
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 395
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 450 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
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
# 952
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 954
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 968
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 977
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 996
void*Cyc_Absyn_exn_typ();
# 1018
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1034
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1038
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1048
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1054
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple2*,unsigned int);
# 1109
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1122
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1133
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1138
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1142
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);
# 1194
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
# 1218
extern int Cyc_Absyn_no_regions;struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 65 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 122 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*new_fn);
# 139
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 150
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);
# 152
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
# 156
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 160
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause);
# 167
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,void*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*te);
# 189
void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);
void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);
void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt**);struct _tuple12{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;const struct Cyc_Tcenv_CList*f3;};
# 193
const struct _tuple12*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Switch_clause***);
# 197
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);
# 200
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt*);
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*);
# 203
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*name);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc);struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple13**oldtv,unsigned int loc);
# 211
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 213
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,void*r,int resetable,int opened);
# 217
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 229
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 71
void*Cyc_Tcutil_compress(void*t);
# 73
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
# 110
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 112
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 117
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
# 120
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 127
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
# 145
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 159
void Cyc_Tcutil_explain_failure();
# 161
int Cyc_Tcutil_unify(void*,void*);
# 164
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 188
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 192
struct _tuple13*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);
# 230 "tcutil.h"
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 238
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 289
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 294
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 311
int Cyc_Tcutil_new_tvar_id();
# 328
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 331
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e);
# 349
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 353
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 370
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 32 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};
# 39
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);
# 42
extern int Cyc_Tcexp_in_stmt_exp;struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};
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
void Cyc_Tcpat_check_catch_overlap(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,void**);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
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
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple14{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple14 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 144 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 163
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 235 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 32 "new_control_flow.h"
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);
# 27 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int new_block);
# 56 "tcstmt.cyc"
static void Cyc_Tcstmt_simplify_unused_result_exp(struct Cyc_Absyn_Exp*e){
void*_tmp0=e->r;void*_tmp1=_tmp0;struct Cyc_Absyn_Exp*_tmp2;struct Cyc_Absyn_Exp*_tmp3;if(((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->tag == 4)switch(((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f2){case Cyc_Absyn_PostInc: _LL1: _tmp3=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL2:
# 59
{struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmp15F;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp15E;e->r=(void*)((_tmp15E=_cycalloc(sizeof(*_tmp15E)),((_tmp15E[0]=((_tmp15F.tag=4,((_tmp15F.f1=_tmp3,((_tmp15F.f2=Cyc_Absyn_PreInc,_tmp15F)))))),_tmp15E))));}goto _LL0;case Cyc_Absyn_PostDec: _LL3: _tmp2=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL4:
# 61
{struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmp162;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp161;e->r=(void*)((_tmp161=_cycalloc(sizeof(*_tmp161)),((_tmp161[0]=((_tmp162.tag=4,((_tmp162.f1=_tmp2,((_tmp162.f2=Cyc_Absyn_PreDec,_tmp162)))))),_tmp161))));}goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
 goto _LL0;}_LL0:;}
# 66
static void Cyc_Tcstmt_decorate_stmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s){
Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));
s->try_depth=Cyc_Tcenv_get_try_depth(te);}struct _tuple15{struct Cyc_Absyn_Tvar*f1;int f2;};
# 72
static void Cyc_Tcstmt_pattern_synth(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcpat_TcPatResult pat_res,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*where_opt,int new_block){
# 74
struct Cyc_Tcpat_TcPatResult _tmp8=pat_res;struct _tuple1*_tmp9;struct Cyc_List_List*_tmpA;_LL8: _tmp9=_tmp8.tvars_and_bounds_opt;_tmpA=_tmp8.patvars;_LL9: {
struct _tuple1 _tmpB=((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmpA);struct _tuple1 _tmpC=_tmpB;struct Cyc_List_List*_tmpD;_LLD: _tmpD=_tmpC.f1;_LLE: {
struct Cyc_List_List*_tmpE=_tmp9 == 0?0:((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst,(*_tmp9).f1);
struct Cyc_List_List*_tmpF=_tmp9 == 0?0:(*_tmp9).f2;
struct Cyc_List_List*_tmp10=_tmp9 == 0?0:((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst,((struct Cyc_List_List*(*)(int(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_filter)((int(*)(struct _tuple15*))Cyc_Core_snd,(*_tmp9).f1));
struct _RegionHandle _tmp11=_new_region("r");struct _RegionHandle*r=& _tmp11;_push_region(r);{
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r,loc,te,_tmpE);
for(0;_tmp10 != 0;_tmp10=_tmp10->tl){
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp165;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp164;te2=Cyc_Tcenv_add_region(r,te2,(void*)((_tmp164=_cycalloc(sizeof(*_tmp164)),((_tmp164[0]=((_tmp165.tag=2,((_tmp165.f1=(struct Cyc_Absyn_Tvar*)_tmp10->hd,_tmp165)))),_tmp164)))),0,1);}
te2=Cyc_Tcenv_new_outlives_constraints(r,te2,_tmpF,loc);
if(new_block)
te2=Cyc_Tcenv_new_block(r,loc,te2);{
void*_tmp14=Cyc_Tcenv_curr_rgn(te2);
{struct Cyc_List_List*_tmp15=_tmpD;for(0;_tmp15 != 0;_tmp15=_tmp15->tl){
if((struct Cyc_Absyn_Vardecl**)_tmp15->hd != 0){
te2=Cyc_Tcenv_add_pat_var(r,loc,te2,*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)_tmp15->hd)));
(*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)_tmp15->hd)))->rgn=_tmp14;}}}
# 93
if(where_opt != 0){
const char*_tmp166;Cyc_Tcexp_tcTest(te2,where_opt,((_tmp166="switch clause guard",_tag_dyneither(_tmp166,sizeof(char),20))));}
# 96
if(_tmpD != 0)
te2=Cyc_Tcenv_set_encloser(r,te2,s);
Cyc_Tcstmt_tcStmt(te2,s,0);};}
# 101
if(_tmpD != 0)
Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));
# 79
;_pop_region(r);}_LLC:;}_LL7:;}
# 105
static struct Cyc_List_List*Cyc_Tcstmt_cmap_c(struct _RegionHandle*r,void*(*f)(void*,void*),void*env,const struct Cyc_Tcenv_CList*const x){
# 107
if(x == (const struct Cyc_Tcenv_CList*)0)return 0;else{
struct Cyc_List_List*_tmp167;return(_tmp167=_region_malloc(r,sizeof(*_tmp167)),((_tmp167->hd=f(env,x->hd),((_tmp167->tl=Cyc_Tcstmt_cmap_c(r,f,env,x->tl),_tmp167)))));}}
# 113
static void Cyc_Tcstmt_tcStmtRefine(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,void*r1,void*r2,int new_block){
# 116
struct _RegionHandle _tmp18=_new_region("r");struct _RegionHandle*r=& _tmp18;_push_region(r);
{struct _tuple13*tk=0;
struct Cyc_Tcenv_Tenv*_tmp19=Cyc_Tcenv_add_region_equality(r,te,r1,r2,& tk,s0->loc);
Cyc_Tcstmt_tcStmt(_tmp19,s0,new_block);
if(tk != 0)
((*((struct _tuple13*)_check_null(tk))).f1)->kind=(*((struct _tuple13*)_check_null(tk))).f2;}
# 117
;_pop_region(r);}
# 125
static int Cyc_Tcstmt_stmt_temp_var_counter=0;
static struct _tuple2*Cyc_Tcstmt_stmt_temp_var(){
int _tmp1A=Cyc_Tcstmt_stmt_temp_var_counter ++;
struct _dyneither_ptr*_tmp174;const char*_tmp173;void*_tmp172[1];struct Cyc_Int_pa_PrintArg_struct _tmp171;struct _tuple2*_tmp170;struct _tuple2*res=(_tmp170=_cycalloc(sizeof(*_tmp170)),((_tmp170->f1=Cyc_Absyn_Loc_n,((_tmp170->f2=((_tmp174=_cycalloc(sizeof(*_tmp174)),((_tmp174[0]=(struct _dyneither_ptr)((_tmp171.tag=1,((_tmp171.f1=(unsigned int)_tmp1A,((_tmp172[0]=& _tmp171,Cyc_aprintf(((_tmp173="_stmttmp%X",_tag_dyneither(_tmp173,sizeof(char),11))),_tag_dyneither(_tmp172,sizeof(void*),1)))))))),_tmp174)))),_tmp170)))));
return res;}
# 139 "tcstmt.cyc"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,int new_block){
# 141
Cyc_Tcstmt_decorate_stmt(te,s0);{
# 143
void*_tmp20=s0->r;void*_tmp21=_tmp20;struct Cyc_Absyn_Exp*_tmp22;struct Cyc_Absyn_Decl*_tmp23;struct Cyc_Absyn_Stmt*_tmp24;struct Cyc_Absyn_Stmt*_tmp25;struct Cyc_List_List**_tmp26;void**_tmp27;struct Cyc_Absyn_Exp**_tmp28;struct Cyc_List_List*_tmp29;void**_tmp2A;struct _dyneither_ptr*_tmp2B;struct Cyc_Absyn_Stmt*_tmp2C;struct Cyc_List_List*_tmp2D;struct Cyc_Absyn_Switch_clause***_tmp2E;struct _dyneither_ptr*_tmp2F;struct Cyc_Absyn_Stmt**_tmp30;struct Cyc_Absyn_Stmt**_tmp31;struct Cyc_Absyn_Stmt**_tmp32;struct Cyc_Absyn_Stmt*_tmp33;struct Cyc_Absyn_Exp*_tmp34;struct Cyc_Absyn_Stmt*_tmp35;struct Cyc_Absyn_Exp*_tmp36;struct Cyc_Absyn_Exp*_tmp37;struct Cyc_Absyn_Stmt*_tmp38;struct Cyc_Absyn_Exp*_tmp39;struct Cyc_Absyn_Stmt*_tmp3A;struct Cyc_Absyn_Stmt*_tmp3B;struct Cyc_Absyn_Exp*_tmp3C;struct Cyc_Absyn_Stmt*_tmp3D;struct Cyc_Absyn_Stmt*_tmp3E;struct Cyc_Absyn_Exp*_tmp3F;struct Cyc_Absyn_Stmt*_tmp40;struct Cyc_Absyn_Stmt*_tmp41;struct Cyc_Absyn_Exp*_tmp42;struct Cyc_Absyn_Stmt*_tmp43;struct Cyc_Absyn_Stmt*_tmp44;struct Cyc_Absyn_Exp*_tmp45;switch(*((int*)_tmp21)){case 0: _LL12: _LL13:
# 145
 return;case 1: _LL14: _tmp45=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp21)->f1;_LL15:
# 148
 Cyc_Tcexp_tcExp(te,0,_tmp45);
if(!Cyc_Tcexp_in_stmt_exp)
Cyc_Tcstmt_simplify_unused_result_exp(_tmp45);
return;case 2: _LL16: _tmp43=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp21)->f1;_tmp44=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp21)->f2;_LL17: {
# 154
struct _RegionHandle _tmp46=_new_region("r");struct _RegionHandle*r=& _tmp46;_push_region(r);{
struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct _tmp177;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp176;struct Cyc_Tcenv_Tenv*_tmp47=Cyc_Tcenv_set_next(r,te,(void*)((_tmp176=_cycalloc(sizeof(*_tmp176)),((_tmp176[0]=((_tmp177.tag=3,((_tmp177.f1=_tmp44,_tmp177)))),_tmp176)))));
Cyc_Tcstmt_tcStmt(_tmp47,_tmp43,1);}
# 158
Cyc_Tcstmt_tcStmt(te,_tmp44,1);
_npop_handler(0);return;
# 154
;_pop_region(r);}case 3: _LL18: _tmp42=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp21)->f1;_LL19: {
# 179 "tcstmt.cyc"
void*t=Cyc_Tcenv_return_typ(te);
if(_tmp42 == 0){
void*_tmp4A=Cyc_Tcutil_compress(t);void*_tmp4B=_tmp4A;switch(*((int*)_tmp4B)){case 0: _LL35: _LL36:
 goto _LL34;case 7: _LL37: _LL38:
 goto _LL3A;case 6: _LL39: _LL3A:
# 185
{const char*_tmp17B;void*_tmp17A[1];struct Cyc_String_pa_PrintArg_struct _tmp179;(_tmp179.tag=0,((_tmp179.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp17A[0]=& _tmp179,Cyc_Tcutil_warn(s0->loc,((_tmp17B="should return a value of type %s",_tag_dyneither(_tmp17B,sizeof(char),33))),_tag_dyneither(_tmp17A,sizeof(void*),1)))))));}
goto _LL34;default: _LL3B: _LL3C:
# 188
{const char*_tmp17F;void*_tmp17E[1];struct Cyc_String_pa_PrintArg_struct _tmp17D;(_tmp17D.tag=0,((_tmp17D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp17E[0]=& _tmp17D,Cyc_Tcutil_terr(s0->loc,((_tmp17F="must return a value of type %s",_tag_dyneither(_tmp17F,sizeof(char),31))),_tag_dyneither(_tmp17E,sizeof(void*),1)))))));}
goto _LL34;}_LL34:;}else{
# 193
int bogus=0;
struct Cyc_Absyn_Exp*e=_tmp42;
Cyc_Tcexp_tcExp(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
{const char*_tmp184;void*_tmp183[2];struct Cyc_String_pa_PrintArg_struct _tmp182;struct Cyc_String_pa_PrintArg_struct _tmp181;(_tmp181.tag=0,((_tmp181.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp182.tag=0,((_tmp182.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp183[0]=& _tmp182,((_tmp183[1]=& _tmp181,Cyc_Tcutil_terr(s0->loc,((_tmp184="returns value of type %s but requires %s",_tag_dyneither(_tmp184,sizeof(char),41))),_tag_dyneither(_tmp183,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{
# 201
struct _RegionHandle _tmp56=_new_region("temp");struct _RegionHandle*temp=& _tmp56;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){
const char*_tmp187;void*_tmp186;(_tmp186=0,Cyc_Tcutil_terr(e->loc,((_tmp187="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp187,sizeof(char),49))),_tag_dyneither(_tmp186,sizeof(void*),0)));}
# 202
;_pop_region(temp);};}
# 206
return;}case 4: _LL1A: _tmp3F=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp21)->f1;_tmp40=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp21)->f2;_tmp41=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp21)->f3;_LL1B: {
# 209
const char*_tmp188;struct Cyc_Tcexp_TestEnv _tmp59=Cyc_Tcexp_tcTest(te,_tmp3F,((_tmp188="if statement",_tag_dyneither(_tmp188,sizeof(char),13))));struct Cyc_Tcexp_TestEnv _tmp5A=_tmp59;struct _tuple0*_tmp5B;int _tmp5C;_LL3E: _tmp5B=_tmp5A.eq;_tmp5C=_tmp5A.isTrue;_LL3F:
{struct _tuple0*_tmp5D=_tmp5B;void*_tmp5E;void*_tmp5F;if(_tmp5D != 0){_LL43: _tmp5E=_tmp5D->f1;_tmp5F=_tmp5D->f2;_LL44:
# 212
 if(_tmp5C){
Cyc_Tcstmt_tcStmtRefine(te,_tmp40,_tmp5E,_tmp5F,1);
Cyc_Tcstmt_tcStmt(te,_tmp41,1);}else{
# 217
Cyc_Tcstmt_tcStmt(te,_tmp40,1);
Cyc_Tcstmt_tcStmtRefine(te,_tmp41,_tmp5E,_tmp5F,1);}
# 220
goto _LL42;}else{_LL45: _LL46:
# 222
 Cyc_Tcstmt_tcStmt(te,_tmp40,1);
Cyc_Tcstmt_tcStmt(te,_tmp41,1);
goto _LL42;}_LL42:;}
# 226
return;_LL3D:;}case 5: _LL1C: _tmp3C=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp21)->f1).f1;_tmp3D=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp21)->f1).f2;_tmp3E=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp21)->f2;_LL1D:
# 229
 Cyc_Tcstmt_decorate_stmt(te,_tmp3D);
{const char*_tmp189;Cyc_Tcexp_tcTest(te,_tmp3C,((_tmp189="while loop",_tag_dyneither(_tmp189,sizeof(char),11))));}{
struct _RegionHandle _tmp62=_new_region("r");struct _RegionHandle*r=& _tmp62;_push_region(r);
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_in_loop(r,te,_tmp3D),_tmp3E,1);
# 234
_npop_handler(0);return;
# 231
;_pop_region(r);};case 9: _LL1E: _tmp36=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp21)->f1;_tmp37=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp21)->f2).f1;_tmp38=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp21)->f2).f2;_tmp39=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp21)->f3).f1;_tmp3A=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp21)->f3).f2;_tmp3B=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp21)->f4;_LL1F:
# 237
 Cyc_Tcstmt_decorate_stmt(te,_tmp38);
Cyc_Tcstmt_decorate_stmt(te,_tmp3A);
Cyc_Tcexp_tcExp(te,0,_tmp36);
{const char*_tmp18A;Cyc_Tcexp_tcTest(te,_tmp37,((_tmp18A="for loop",_tag_dyneither(_tmp18A,sizeof(char),9))));}{
struct _RegionHandle _tmp64=_new_region("r");struct _RegionHandle*r=& _tmp64;_push_region(r);{
struct Cyc_Tcenv_Tenv*_tmp65=Cyc_Tcenv_set_in_loop(r,te,_tmp3A);
Cyc_Tcstmt_tcStmt(_tmp65,_tmp3B,1);
Cyc_Tcexp_tcExp(_tmp65,0,_tmp39);
Cyc_Tcstmt_simplify_unused_result_exp(_tmp39);}
# 247
_npop_handler(0);return;
# 241
;_pop_region(r);};case 14: _LL20: _tmp33=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp21)->f1;_tmp34=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp21)->f2).f1;_tmp35=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp21)->f2).f2;_LL21:
# 250
 Cyc_Tcstmt_decorate_stmt(te,_tmp35);{
struct _RegionHandle _tmp66=_new_region("r");struct _RegionHandle*r=& _tmp66;_push_region(r);
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_in_loop(r,te,_tmp35),_tmp33,1);
# 254
{const char*_tmp18B;Cyc_Tcexp_tcTest(te,_tmp34,((_tmp18B="do loop",_tag_dyneither(_tmp18B,sizeof(char),8))));}
_npop_handler(0);return;
# 251
;_pop_region(r);};case 6: _LL22: _tmp32=(struct Cyc_Absyn_Stmt**)&((struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp21)->f1;_LL23:
# 259
 Cyc_Tcenv_process_break(te,s0,_tmp32);return;case 7: _LL24: _tmp31=(struct Cyc_Absyn_Stmt**)&((struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp21)->f1;_LL25:
 Cyc_Tcenv_process_continue(te,s0,_tmp31);return;case 8: _LL26: _tmp2F=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp21)->f1;_tmp30=(struct Cyc_Absyn_Stmt**)&((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp21)->f2;_LL27:
 Cyc_Tcenv_process_goto(te,s0,_tmp2F,_tmp30);return;case 11: _LL28: _tmp2D=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp21)->f1;_tmp2E=(struct Cyc_Absyn_Switch_clause***)&((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp21)->f2;_LL29: {
# 264
const struct _tuple12*_tmp68=Cyc_Tcenv_process_fallthru(te,s0,_tmp2E);
if(_tmp68 == (const struct _tuple12*)0){
{const char*_tmp18E;void*_tmp18D;(_tmp18D=0,Cyc_Tcutil_terr(s0->loc,((_tmp18E="fallthru not in a non-last case",_tag_dyneither(_tmp18E,sizeof(char),32))),_tag_dyneither(_tmp18D,sizeof(void*),0)));}
return;}{
# 269
struct Cyc_List_List*_tmp6B=(*_tmp68).f2;
const struct Cyc_Tcenv_CList*_tmp6C=(*_tmp68).f3;
struct Cyc_List_List*instantiation=
((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_make_inst_var,Cyc_Tcenv_lookup_type_vars(te),_tmp6B);
struct Cyc_List_List*_tmp6D=((struct Cyc_List_List*(*)(struct _RegionHandle*r,void*(*f)(struct Cyc_List_List*,void*),struct Cyc_List_List*env,const struct Cyc_Tcenv_CList*x))Cyc_Tcstmt_cmap_c)(Cyc_Core_heap_region,Cyc_Tcutil_substitute,instantiation,_tmp6C);
for(0;_tmp6D != 0  && _tmp2D != 0;(_tmp6D=_tmp6D->tl,_tmp2D=_tmp2D->tl)){
# 276
int bogus=0;
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp2D->hd);
if(!Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)_tmp2D->hd,(void*)_tmp6D->hd,& bogus)){
{const char*_tmp193;void*_tmp192[2];struct Cyc_String_pa_PrintArg_struct _tmp191;struct Cyc_String_pa_PrintArg_struct _tmp190;(_tmp190.tag=0,((_tmp190.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 281
Cyc_Absynpp_typ2string((void*)_tmp6D->hd)),((_tmp191.tag=0,((_tmp191.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp2D->hd)->topt))),((_tmp192[0]=& _tmp191,((_tmp192[1]=& _tmp190,Cyc_Tcutil_terr(s0->loc,((_tmp193="fallthru argument has type %s but pattern variable has type %s",_tag_dyneither(_tmp193,sizeof(char),63))),_tag_dyneither(_tmp192,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{
# 285
struct _RegionHandle _tmp72=_new_region("temp");struct _RegionHandle*temp=& _tmp72;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(void*)_tmp6D->hd) && !
Cyc_Tcutil_is_noalias_path(temp,(struct Cyc_Absyn_Exp*)_tmp2D->hd)){
const char*_tmp196;void*_tmp195;(_tmp195=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp2D->hd)->loc,((_tmp196="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp196,sizeof(char),49))),_tag_dyneither(_tmp195,sizeof(void*),0)));}
# 286
;_pop_region(temp);};}
# 291
if(_tmp2D != 0){
const char*_tmp199;void*_tmp198;(_tmp198=0,Cyc_Tcutil_terr(s0->loc,((_tmp199="too many arguments in explicit fallthru",_tag_dyneither(_tmp199,sizeof(char),40))),_tag_dyneither(_tmp198,sizeof(void*),0)));}
if(_tmp6D != 0){
const char*_tmp19C;void*_tmp19B;(_tmp19B=0,Cyc_Tcutil_terr(s0->loc,((_tmp19C="too few arguments in explicit fallthru",_tag_dyneither(_tmp19C,sizeof(char),39))),_tag_dyneither(_tmp19B,sizeof(void*),0)));}
return;};}case 13: _LL2A: _tmp2B=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp21)->f1;_tmp2C=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp21)->f2;_LL2B: {
# 303
struct _RegionHandle _tmp79=_new_region("r");struct _RegionHandle*r=& _tmp79;_push_region(r);{
struct _dyneither_ptr*_tmp1A9;const char*_tmp1A8;void*_tmp1A7[1];struct Cyc_String_pa_PrintArg_struct _tmp1A6;struct Cyc_Absyn_Tvar*_tmp1A5;Cyc_Tcstmt_tcStmt(Cyc_Tcenv_new_named_block(r,s0->loc,Cyc_Tcenv_add_label(te,_tmp2B,_tmp2C),(
(_tmp1A5=_cycalloc(sizeof(*_tmp1A5)),((_tmp1A5->name=((_tmp1A9=_cycalloc(sizeof(*_tmp1A9)),((_tmp1A9[0]=(struct _dyneither_ptr)((_tmp1A6.tag=0,((_tmp1A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2B),((_tmp1A7[0]=& _tmp1A6,Cyc_aprintf(((_tmp1A8="`%s",_tag_dyneither(_tmp1A8,sizeof(char),4))),_tag_dyneither(_tmp1A7,sizeof(void*),1)))))))),_tmp1A9)))),((_tmp1A5->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp1A5->kind=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmp1A5))))))))),_tmp2C,0);}
# 309
_npop_handler(0);return;
# 303
;_pop_region(r);}case 10: _LL2C: _tmp28=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp21)->f1;_tmp29=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp21)->f2;_tmp2A=(void**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp21)->f3;_LL2D: {
# 315
struct Cyc_Absyn_Exp*_tmp7F=*_tmp28;
if(!Cyc_Tcutil_is_var_exp(_tmp7F)){
struct _tuple2*_tmp80=Cyc_Tcstmt_stmt_temp_var();
struct Cyc_Absyn_Stmt*_tmp81=Cyc_Absyn_new_stmt(s0->r,s0->loc);
struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp1AC;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp1AB;struct Cyc_Absyn_Decl*_tmp82=Cyc_Absyn_let_decl(Cyc_Absyn_new_pat((void*)((_tmp1AB=_cycalloc(sizeof(*_tmp1AB)),((_tmp1AB[0]=((_tmp1AC.tag=1,((_tmp1AC.f1=Cyc_Absyn_new_vardecl(_tmp80,
Cyc_Absyn_new_evar(0,0),0),((_tmp1AC.f2=
# 322
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,_tmp7F->loc),_tmp1AC)))))),_tmp1AB)))),_tmp7F->loc),_tmp7F,s0->loc);
# 325
struct Cyc_Absyn_Stmt*_tmp83=Cyc_Absyn_decl_stmt(_tmp82,_tmp81,s0->loc);
# 327
s0->r=_tmp83->r;
*_tmp28=Cyc_Absyn_var_exp(_tmp80,_tmp7F->loc);
Cyc_Tcstmt_tcStmt(te,s0,new_block);
return;}
# 333
Cyc_Tcexp_tcExp(te,0,_tmp7F);{
void*_tmp86=(void*)_check_null(_tmp7F->topt);
# 338
struct _RegionHandle _tmp87=_new_region("r");struct _RegionHandle*r=& _tmp87;_push_region(r);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmp86) && !Cyc_Tcutil_is_noalias_path(r,_tmp7F)){
const char*_tmp1AF;void*_tmp1AE;(_tmp1AE=0,Cyc_Tcutil_terr(_tmp7F->loc,((_tmp1AF="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp1AF,sizeof(char),49))),_tag_dyneither(_tmp1AE,sizeof(void*),0)));}{
struct Cyc_Tcenv_Tenv*_tmp8A=Cyc_Tcenv_set_in_switch(r,te);
_tmp8A=Cyc_Tcenv_clear_fallthru(r,_tmp8A);{
struct Cyc_List_List*scs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp29);for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Pat*_tmp8B=((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern;
struct Cyc_Tcpat_TcPatResult _tmp8C=Cyc_Tcpat_tcPat(_tmp8A,_tmp8B,& _tmp86,_tmp7F);
struct Cyc_List_List*_tmp8D=_tmp8C.tvars_and_bounds_opt == 0?0:
((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst,(*_tmp8C.tvars_and_bounds_opt).f1);
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp8B->topt),_tmp86)){
{const char*_tmp1B4;void*_tmp1B3[2];struct Cyc_String_pa_PrintArg_struct _tmp1B2;struct Cyc_String_pa_PrintArg_struct _tmp1B1;(_tmp1B1.tag=0,((_tmp1B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmp8B->topt))),((_tmp1B2.tag=0,((_tmp1B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp86)),((_tmp1B3[0]=& _tmp1B2,((_tmp1B3[1]=& _tmp1B1,Cyc_Tcutil_terr(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,((_tmp1B4="switch on type %s, but case expects type %s",_tag_dyneither(_tmp1B4,sizeof(char),44))),_tag_dyneither(_tmp1B3,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}else{
# 354
Cyc_Tcpat_check_pat_regions(_tmp8A,_tmp8B,_tmp8C.patvars);}
{struct Cyc_Core_Opt*_tmp1B5;((struct Cyc_Absyn_Switch_clause*)scs->hd)->pat_vars=((_tmp1B5=_cycalloc(sizeof(*_tmp1B5)),((_tmp1B5->v=_tmp8C.patvars,_tmp1B5))));}
# 357
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,_tmp8A,_tmp8C,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body,((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause,1);
# 359
if(_tmp8C.tvars_and_bounds_opt != 0  && (*_tmp8C.tvars_and_bounds_opt).f2 != 0)
# 361
_tmp8A=Cyc_Tcenv_clear_fallthru(r,_tmp8A);else{
# 363
struct Cyc_List_List*_tmp93=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp8C.patvars)).f1);
_tmp93=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp93);
_tmp8A=Cyc_Tcenv_set_fallthru(r,_tmp8A,_tmp8D,_tmp93,(struct Cyc_Absyn_Switch_clause*)scs->hd);}}};};
# 369
Cyc_Tcpat_check_switch_exhaustive(s0->loc,te,_tmp29,_tmp2A);
_npop_handler(0);return;
# 338
;_pop_region(r);};}case 15: _LL2E: _tmp25=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp21)->f1;_tmp26=(struct Cyc_List_List**)&((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp21)->f2;_tmp27=(void**)&((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp21)->f3;_LL2F:
# 377
{struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp1B8;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp1B7;_tmp25->r=(Cyc_Absyn_new_stmt((void*)((_tmp1B7=_cycalloc(sizeof(*_tmp1B7)),((_tmp1B7[0]=((_tmp1B8.tag=2,((_tmp1B8.f1=Cyc_Absyn_new_stmt(_tmp25->r,_tmp25->loc),((_tmp1B8.f2=Cyc_Absyn_skip_stmt(_tmp25->loc),_tmp1B8)))))),_tmp1B7)))),_tmp25->loc))->r;}{
# 381
struct _RegionHandle _tmp96=_new_region("r");struct _RegionHandle*r=& _tmp96;_push_region(r);
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(r,Cyc_Tcenv_enter_try(r,te),s0),_tmp25,1);{
# 386
struct _tuple2*_tmp1B9;struct _tuple2*def_v=(_tmp1B9=_cycalloc(sizeof(*_tmp1B9)),((_tmp1B9->f1=Cyc_Absyn_Loc_n,((_tmp1B9->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"exn",sizeof(char),4),_tmp1B9)))));
struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp1BC;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp1BB;struct Cyc_Absyn_Pat*_tmp97=Cyc_Absyn_new_pat((void*)((_tmp1BB=_cycalloc(sizeof(*_tmp1BB)),((_tmp1BB[0]=((_tmp1BC.tag=1,((_tmp1BC.f1=Cyc_Absyn_new_vardecl(def_v,(void*)& Cyc_Absyn_VoidType_val,0),((_tmp1BC.f2=
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0),_tmp1BC)))))),_tmp1BB)))),0);
struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct _tmp1BF;struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp1BE;struct Cyc_Absyn_Stmt*_tmp98=Cyc_Absyn_exp_stmt(Cyc_Absyn_new_exp((void*)((_tmp1BE=_cycalloc(sizeof(*_tmp1BE)),((_tmp1BE[0]=((_tmp1BF.tag=10,((_tmp1BF.f1=Cyc_Absyn_var_exp(def_v,0),((_tmp1BF.f2=1,_tmp1BF)))))),_tmp1BE)))),0),0);
struct Cyc_Absyn_Switch_clause*_tmp1C0;struct Cyc_Absyn_Switch_clause*_tmp99=(_tmp1C0=_cycalloc(sizeof(*_tmp1C0)),((_tmp1C0->pattern=_tmp97,((_tmp1C0->pat_vars=0,((_tmp1C0->where_clause=0,((_tmp1C0->body=_tmp98,((_tmp1C0->loc=0,_tmp1C0)))))))))));
{struct Cyc_Absyn_Switch_clause*_tmp1C1[1];*_tmp26=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(*_tmp26,((_tmp1C1[0]=_tmp99,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C1,sizeof(struct Cyc_Absyn_Switch_clause*),1)))));}{
# 393
struct _RegionHandle _tmp9B=_new_region("r2");struct _RegionHandle*r2=& _tmp9B;_push_region(r2);{
# 395
struct Cyc_Tcenv_Tenv*_tmp9C=Cyc_Tcenv_set_in_switch(r2,te);
_tmp9C=Cyc_Tcenv_clear_fallthru(r2,_tmp9C);{
struct Cyc_List_List*_tmp9D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(*_tmp26);for(0;_tmp9D != 0;_tmp9D=_tmp9D->tl){
struct Cyc_Absyn_Pat*_tmp9E=((struct Cyc_Absyn_Switch_clause*)_tmp9D->hd)->pattern;
void*_tmp9F=Cyc_Absyn_exn_typ();
struct Cyc_Tcpat_TcPatResult _tmpA0=Cyc_Tcpat_tcPat(_tmp9C,_tmp9E,& _tmp9F,0);
# 403
struct Cyc_List_List*_tmpA1=_tmpA0.tvars_and_bounds_opt == 0?0:
((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst,(*_tmpA0.tvars_and_bounds_opt).f1);
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp9E->topt),_tmp9F)){
const char*_tmp1C5;void*_tmp1C4[1];struct Cyc_String_pa_PrintArg_struct _tmp1C3;(_tmp1C3.tag=0,((_tmp1C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmp9E->topt))),((_tmp1C4[0]=& _tmp1C3,Cyc_Tcutil_terr(((struct Cyc_Absyn_Switch_clause*)_tmp9D->hd)->loc,((_tmp1C5="expected datatype exn@ but found %s",_tag_dyneither(_tmp1C5,sizeof(char),36))),_tag_dyneither(_tmp1C4,sizeof(void*),1)))))));}else{
# 409
Cyc_Tcpat_check_pat_regions(_tmp9C,_tmp9E,_tmpA0.patvars);}
{struct Cyc_Core_Opt*_tmp1C6;((struct Cyc_Absyn_Switch_clause*)_tmp9D->hd)->pat_vars=((_tmp1C6=_cycalloc(sizeof(*_tmp1C6)),((_tmp1C6->v=_tmpA0.patvars,_tmp1C6))));}
# 412
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)_tmp9D->hd)->loc,_tmp9C,_tmpA0,((struct Cyc_Absyn_Switch_clause*)_tmp9D->hd)->body,((struct Cyc_Absyn_Switch_clause*)_tmp9D->hd)->where_clause,1);
# 414
if(_tmpA0.tvars_and_bounds_opt != 0  && (*_tmpA0.tvars_and_bounds_opt).f2 != 0)
# 416
_tmp9C=Cyc_Tcenv_clear_fallthru(r2,_tmp9C);else{
# 418
struct Cyc_List_List*_tmpA6=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmpA0.patvars)).f1);
_tmpA6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpA6);
_tmp9C=Cyc_Tcenv_set_fallthru(r2,_tmp9C,_tmpA1,_tmpA6,(struct Cyc_Absyn_Switch_clause*)_tmp9D->hd);}}};}
# 424
Cyc_Tcpat_check_catch_overlap(s0->loc,te,*_tmp26,_tmp27);
_npop_handler(1);return;
# 393
;_pop_region(r2);};};
# 381
;_pop_region(r);};case 12: _LL30: _tmp23=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp21)->f1;_tmp24=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp21)->f2;_LL31: {
# 428
struct _dyneither_ptr unimp_msg_part;
struct _RegionHandle _tmpAE=_new_region("decl_rgn");struct _RegionHandle*decl_rgn=& _tmpAE;_push_region(decl_rgn);{
struct Cyc_Tcenv_Tenv*te2;
if(new_block)
te2=Cyc_Tcenv_new_block(decl_rgn,s0->loc,te);else{
# 434
te2=Cyc_Tcenv_copy_tenv(decl_rgn,te);}{
struct Cyc_Tcenv_Tenv*_tmpAF=te2;
void*_tmpB0=_tmp23->r;void*_tmpB1=_tmpB0;struct _tuple2*_tmpB2;struct Cyc_List_List*_tmpB3;struct _dyneither_ptr*_tmpB4;struct Cyc_List_List*_tmpB5;struct Cyc_Absyn_Fndecl*_tmpB6;struct Cyc_Absyn_Tvar*_tmpB7;struct Cyc_Absyn_Vardecl*_tmpB8;int _tmpB9;struct Cyc_Absyn_Exp*_tmpBA;struct Cyc_List_List*_tmpBB;struct Cyc_Absyn_Pat*_tmpBC;struct Cyc_Core_Opt**_tmpBD;struct Cyc_Absyn_Exp**_tmpBE;void**_tmpBF;struct Cyc_Absyn_Vardecl*_tmpC0;switch(*((int*)_tmpB1)){case 0: _LL48: _tmpC0=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpB1)->f1;_LL49: {
# 438
struct Cyc_Absyn_Vardecl*_tmpC1=_tmpC0;enum Cyc_Absyn_Scope _tmpC2;union Cyc_Absyn_Nmspace _tmpC3;struct _dyneither_ptr*_tmpC4;struct Cyc_Absyn_Tqual _tmpC5;void*_tmpC6;struct Cyc_Absyn_Exp*_tmpC7;void**_tmpC8;struct Cyc_List_List*_tmpC9;_LL67: _tmpC2=_tmpC1->sc;_tmpC3=(_tmpC1->name)->f1;_tmpC4=(_tmpC1->name)->f2;_tmpC5=_tmpC1->tq;_tmpC6=_tmpC1->type;_tmpC7=_tmpC1->initializer;_tmpC8=(void**)& _tmpC1->rgn;_tmpC9=_tmpC1->attributes;_LL68: {
void*_tmpCA=Cyc_Tcenv_curr_rgn(_tmpAF);
int is_local;
{enum Cyc_Absyn_Scope _tmpCB=_tmpC2;switch(_tmpCB){case Cyc_Absyn_Static: _LL6C: _LL6D:
 goto _LL6F;case Cyc_Absyn_Extern: _LL6E: _LL6F:
 goto _LL71;case Cyc_Absyn_ExternC: _LL70: _LL71:
# 445
 _tmpC0->escapes=1;
is_local=0;goto _LL6B;case Cyc_Absyn_Abstract: _LL72: _LL73:
# 448
{const char*_tmp1C9;void*_tmp1C8;(_tmp1C8=0,Cyc_Tcutil_terr(_tmp23->loc,((_tmp1C9="bad abstract scope for local variable",_tag_dyneither(_tmp1C9,sizeof(char),38))),_tag_dyneither(_tmp1C8,sizeof(void*),0)));}
goto _LL75;case Cyc_Absyn_Register: _LL74: _LL75:
 goto _LL77;default: _LL76: _LL77:
 is_local=1;goto _LL6B;}_LL6B:;}
# 454
*_tmpC8=is_local?_tmpCA:(void*)& Cyc_Absyn_HeapRgn_val;
{union Cyc_Absyn_Nmspace _tmpCE=_tmpC3;switch((_tmpCE.C_n).tag){case 4: _LL79: _LL7A:
 goto _LL78;case 1: if((_tmpCE.Rel_n).val == 0){_LL7B: _LL7C:
# 458
(*_tmpC0->name).f1=Cyc_Absyn_Loc_n;
goto _LL78;}else{_LL81: _LL82:
# 465
{const char*_tmp1CC;void*_tmp1CB;(_tmp1CB=0,Cyc_Tcutil_terr(_tmp23->loc,((_tmp1CC="cannot declare a qualified local variable",_tag_dyneither(_tmp1CC,sizeof(char),42))),_tag_dyneither(_tmp1CB,sizeof(void*),0)));}
(*_tmpC0->name).f1=Cyc_Absyn_Loc_n;
goto _LL78;}case 3: _LL7D: _LL7E:
# 462
 goto _LL80;default: _LL7F: _LL80:
 goto _LL82;}_LL78:;}
# 470
{void*_tmpD1=Cyc_Tcutil_compress(_tmpC6);void*_tmpD2=_tmpD1;void*_tmpD3;struct Cyc_Absyn_Tqual _tmpD4;union Cyc_Absyn_Constraint*_tmpD5;unsigned int _tmpD6;switch(*((int*)_tmpD2)){case 9: _LL84: if(is_local){_LL85:
# 473
 _tmpC0->escapes=1;
_tmpC2=Cyc_Absyn_Extern;
is_local=0;
goto _LL83;}else{goto _LL88;}case 8: if((((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD2)->f1).num_elts == 0){_LL86: _tmpD3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD2)->f1).elt_type;_tmpD4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD2)->f1).tq;_tmpD5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD2)->f1).zero_term;_tmpD6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD2)->f1).zt_loc;if(_tmpC0->initializer != 0){_LL87:
# 478
{void*_tmpD7=((struct Cyc_Absyn_Exp*)_check_null(_tmpC0->initializer))->r;void*_tmpD8=_tmpD7;struct Cyc_List_List*_tmpD9;struct Cyc_List_List*_tmpDA;struct Cyc_Absyn_Exp*_tmpDB;struct Cyc_Absyn_Exp*_tmpDC;struct _dyneither_ptr _tmpDD;struct _dyneither_ptr _tmpDE;switch(*((int*)_tmpD8)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).Wstring_c).tag){case 8: _LL8B: _tmpDE=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).String_c).val;_LL8C:
# 480
 _tmpC6=(_tmpC0->type=Cyc_Absyn_array_typ(_tmpD3,_tmpD4,Cyc_Absyn_uint_exp(_get_dyneither_size(_tmpDE,sizeof(char)),0),_tmpD5,_tmpD6));
goto _LL8A;case 9: _LL8D: _tmpDD=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).Wstring_c).val;_LL8E:
# 483
 _tmpC6=(_tmpC0->type=Cyc_Absyn_array_typ(_tmpD3,_tmpD4,Cyc_Absyn_uint_exp(1,0),_tmpD5,_tmpD6));
goto _LL8A;default: goto _LL97;}case 26: _LL8F: _tmpDC=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL90:
 _tmpDB=_tmpDC;goto _LL92;case 27: _LL91: _tmpDB=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL92:
# 488
 _tmpC6=(_tmpC0->type=Cyc_Absyn_array_typ(_tmpD3,_tmpD4,_tmpDB,_tmpD5,_tmpD6));
goto _LL8A;case 35: _LL93: _tmpDA=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL94:
 _tmpD9=_tmpDA;goto _LL96;case 25: _LL95: _tmpD9=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL96:
# 492
 _tmpC6=(_tmpC0->type=Cyc_Absyn_array_typ(_tmpD3,_tmpD4,Cyc_Absyn_uint_exp((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpD9),0),_tmpD5,_tmpD6));
# 494
goto _LL8A;default: _LL97: _LL98:
 goto _LL8A;}_LL8A:;}
# 497
goto _LL83;}else{goto _LL88;}}else{goto _LL88;}default: _LL88: _LL89:
 goto _LL83;}_LL83:;}{
# 504
struct Cyc_List_List*_tmpDF=!is_local?0: Cyc_Tcenv_lookup_type_vars(_tmpAF);
int _tmpE0=!is_local?0: 1;
Cyc_Tcutil_check_type(s0->loc,_tmpAF,_tmpDF,& Cyc_Tcutil_tmk,_tmpE0,1,_tmpC6);
(_tmpC0->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(s0->loc,(_tmpC0->tq).print_const,_tmpC6);{
# 509
struct Cyc_Tcenv_Tenv*_tmpE1=Cyc_Tcenv_add_local_var(decl_rgn,_tmp24->loc,_tmpAF,_tmpC0);
if(_tmpC2 == Cyc_Absyn_Extern  || _tmpC2 == Cyc_Absyn_ExternC){
const char*_tmp1CF;void*_tmp1CE;(_tmp1CE=0,Cyc_Tcutil_terr(_tmp23->loc,((_tmp1CF="extern declarations are not yet supported within functions",_tag_dyneither(_tmp1CF,sizeof(char),59))),_tag_dyneither(_tmp1CE,sizeof(void*),0)));}
if(_tmpC7 != 0){
Cyc_Tcexp_tcExpInitializer(_tmpE1,& _tmpC6,_tmpC7);
# 515
if(!is_local  && !Cyc_Tcutil_is_const_exp(_tmpC7)){
const char*_tmp1D2;void*_tmp1D1;(_tmp1D1=0,Cyc_Tcutil_terr(_tmp23->loc,((_tmp1D2="initializer for static variable needs to be a constant expression",_tag_dyneither(_tmp1D2,sizeof(char),66))),_tag_dyneither(_tmp1D1,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_assign(_tmpE1,_tmpC7,_tmpC6)){
struct _dyneither_ptr _tmpE6=*_tmpC4;
struct _dyneither_ptr _tmpE7=Cyc_Absynpp_typ2string(_tmpC6);
struct _dyneither_ptr _tmpE8=Cyc_Absynpp_typ2string((void*)_check_null(_tmpC7->topt));
if(((_get_dyneither_size(_tmpE6,sizeof(char))+ _get_dyneither_size(_tmpE7,sizeof(char)))+ _get_dyneither_size(_tmpE8,sizeof(char)))+ 50 < 80){
const char*_tmp1D8;void*_tmp1D7[3];struct Cyc_String_pa_PrintArg_struct _tmp1D6;struct Cyc_String_pa_PrintArg_struct _tmp1D5;struct Cyc_String_pa_PrintArg_struct _tmp1D4;(_tmp1D4.tag=0,((_tmp1D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpE8),((_tmp1D5.tag=0,((_tmp1D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpE7),((_tmp1D6.tag=0,((_tmp1D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpE6),((_tmp1D7[0]=& _tmp1D6,((_tmp1D7[1]=& _tmp1D5,((_tmp1D7[2]=& _tmp1D4,Cyc_Tcutil_terr(_tmp23->loc,((_tmp1D8="%s was declared with type %s but initialized with type %s.",_tag_dyneither(_tmp1D8,sizeof(char),59))),_tag_dyneither(_tmp1D7,sizeof(void*),3)))))))))))))))))));}else{
# 524
if((_get_dyneither_size(_tmpE6,sizeof(char))+ _get_dyneither_size(_tmpE7,sizeof(char)))+ 25 < 80  && 
_get_dyneither_size(_tmpE8,sizeof(char))+ 25 < 80){
const char*_tmp1DE;void*_tmp1DD[3];struct Cyc_String_pa_PrintArg_struct _tmp1DC;struct Cyc_String_pa_PrintArg_struct _tmp1DB;struct Cyc_String_pa_PrintArg_struct _tmp1DA;(_tmp1DA.tag=0,((_tmp1DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpE8),((_tmp1DB.tag=0,((_tmp1DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpE7),((_tmp1DC.tag=0,((_tmp1DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpE6),((_tmp1DD[0]=& _tmp1DC,((_tmp1DD[1]=& _tmp1DB,((_tmp1DD[2]=& _tmp1DA,Cyc_Tcutil_terr(_tmp23->loc,((_tmp1DE="%s was declared with type %s\n but initialized with type %s.",_tag_dyneither(_tmp1DE,sizeof(char),60))),_tag_dyneither(_tmp1DD,sizeof(void*),3)))))))))))))))))));}else{
# 529
const char*_tmp1E4;void*_tmp1E3[3];struct Cyc_String_pa_PrintArg_struct _tmp1E2;struct Cyc_String_pa_PrintArg_struct _tmp1E1;struct Cyc_String_pa_PrintArg_struct _tmp1E0;(_tmp1E0.tag=0,((_tmp1E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpE8),((_tmp1E1.tag=0,((_tmp1E1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpE7),((_tmp1E2.tag=0,((_tmp1E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpE6),((_tmp1E3[0]=& _tmp1E2,((_tmp1E3[1]=& _tmp1E1,((_tmp1E3[2]=& _tmp1E0,Cyc_Tcutil_terr(_tmp23->loc,((_tmp1E4="%s declared with type \n%s\n but initialized with type \n%s.",_tag_dyneither(_tmp1E4,sizeof(char),58))),_tag_dyneither(_tmp1E3,sizeof(void*),3)))))))))))))))))));}}
# 531
Cyc_Tcutil_unify(_tmpC6,(void*)_check_null(_tmpC7->topt));
Cyc_Tcutil_explain_failure();}}
# 535
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(decl_rgn,_tmpE1,s0),_tmp24,0);
_npop_handler(0);return;};};}_LL66:;}case 2: _LL4A: _tmpBC=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpB1)->f1;_tmpBD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpB1)->f2;_tmpBE=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpB1)->f3;_tmpBF=(void**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpB1)->f4;_LL4B: {
# 543
struct Cyc_Absyn_Exp*_tmpF8=*_tmpBE;
{void*_tmpF9=_tmpBC->r;void*_tmpFA=_tmpF9;switch(*((int*)_tmpFA)){case 1: _LL9A: _LL9B:
 goto _LL9D;case 2: _LL9C: _LL9D:
 goto _LL9F;case 15: _LL9E: _LL9F:
 goto _LL99;default: _LLA0: _LLA1:
# 549
 if(!Cyc_Tcutil_is_var_exp(_tmpF8)){
struct _tuple2*_tmpFB=Cyc_Tcstmt_stmt_temp_var();
struct Cyc_Absyn_Stmt*_tmpFC=Cyc_Absyn_new_stmt(s0->r,s0->loc);
struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp1E7;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp1E6;struct Cyc_Absyn_Decl*_tmpFD=Cyc_Absyn_let_decl(Cyc_Absyn_new_pat((void*)((_tmp1E6=_cycalloc(sizeof(*_tmp1E6)),((_tmp1E6[0]=((_tmp1E7.tag=1,((_tmp1E7.f1=Cyc_Absyn_new_vardecl(_tmpFB,
Cyc_Absyn_new_evar(0,0),0),((_tmp1E7.f2=
# 555
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,_tmpF8->loc),_tmp1E7)))))),_tmp1E6)))),_tmpF8->loc),_tmpF8,s0->loc);
# 558
struct Cyc_Absyn_Stmt*_tmpFE=Cyc_Absyn_decl_stmt(_tmpFD,_tmpFC,s0->loc);
# 560
s0->r=_tmpFE->r;
*_tmpBE=Cyc_Absyn_var_exp(_tmpFB,_tmpF8->loc);
Cyc_Tcstmt_tcStmt(_tmpAF,s0,new_block);
_npop_handler(0);return;}}_LL99:;}
# 566
Cyc_Tcexp_tcExpInitializer(_tmpAF,0,_tmpF8);{
# 568
void*pat_type=(void*)_check_null(_tmpF8->topt);
# 570
struct Cyc_Tcpat_TcPatResult _tmp101=Cyc_Tcpat_tcPat(_tmpAF,_tmpBC,& pat_type,_tmpF8);
{struct Cyc_Core_Opt*_tmp1E8;*_tmpBD=((_tmp1E8=_cycalloc(sizeof(*_tmp1E8)),((_tmp1E8->v=_tmp101.patvars,_tmp1E8))));}
if(!Cyc_Tcutil_unify((void*)_check_null(_tmpBC->topt),(void*)_check_null(_tmpF8->topt)) && !
Cyc_Tcutil_coerce_assign(_tmpAF,_tmpF8,(void*)_check_null(_tmpBC->topt))){
{const char*_tmp1ED;void*_tmp1EC[2];struct Cyc_String_pa_PrintArg_struct _tmp1EB;struct Cyc_String_pa_PrintArg_struct _tmp1EA;(_tmp1EA.tag=0,((_tmp1EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmpF8->topt))),((_tmp1EB.tag=0,((_tmp1EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmpBC->topt))),((_tmp1EC[0]=& _tmp1EB,((_tmp1EC[1]=& _tmp1EA,Cyc_Tcutil_terr(_tmp23->loc,((_tmp1ED="pattern type %s does not match definition type %s",_tag_dyneither(_tmp1ED,sizeof(char),50))),_tag_dyneither(_tmp1EC,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unify((void*)_check_null(_tmpBC->topt),(void*)_check_null(_tmpF8->topt));
Cyc_Tcutil_explain_failure();}else{
# 580
Cyc_Tcpat_check_pat_regions(_tmpAF,_tmpBC,_tmp101.patvars);}
Cyc_Tcpat_check_let_pat_exhaustive(_tmpBC->loc,_tmpAF,_tmpBC,_tmpBF);
Cyc_Tcstmt_pattern_synth(s0->loc,_tmpAF,_tmp101,_tmp24,0,0);
_npop_handler(0);return;};}case 3: _LL4C: _tmpBB=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmpB1)->f1;_LL4D: {
# 586
void*_tmp107=Cyc_Tcenv_curr_rgn(_tmpAF);
struct Cyc_Tcenv_Tenv*_tmp108=_tmpAF;
for(0;_tmpBB != 0;_tmpBB=_tmpBB->tl){
struct Cyc_Absyn_Vardecl*_tmp109=(struct Cyc_Absyn_Vardecl*)_tmpBB->hd;
struct Cyc_Absyn_Vardecl*_tmp10A=_tmp109;union Cyc_Absyn_Nmspace _tmp10B;void*_tmp10C;void**_tmp10D;_LLA3: _tmp10B=(_tmp10A->name)->f1;_tmp10C=_tmp10A->type;_tmp10D=(void**)& _tmp10A->rgn;_LLA4:
*_tmp10D=_tmp107;
{union Cyc_Absyn_Nmspace _tmp10E=_tmp10B;struct Cyc_List_List*_tmp10F;switch((_tmp10E.Abs_n).tag){case 4: _LLA8: _LLA9:
 goto _LLA7;case 1: if((_tmp10E.Rel_n).val == 0){_LLAA: _LLAB:
# 595
(*_tmp109->name).f1=Cyc_Absyn_Loc_n;
goto _LLA7;}else{goto _LLAE;}case 2: _LLAC: _tmp10F=(_tmp10E.Abs_n).val;_LLAD: {
const char*_tmp1F0;void*_tmp1EF;(int)_throw(((_tmp1EF=0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1F0="tcstmt: Abs_n declaration",_tag_dyneither(_tmp1F0,sizeof(char),26))),_tag_dyneither(_tmp1EF,sizeof(void*),0)))));}default: _LLAE: _LLAF:
# 601
{const char*_tmp1F3;void*_tmp1F2;(_tmp1F2=0,Cyc_Tcutil_terr(_tmp23->loc,((_tmp1F3="cannot declare a qualified local variable",_tag_dyneither(_tmp1F3,sizeof(char),42))),_tag_dyneither(_tmp1F2,sizeof(void*),0)));}
goto _LLA7;}_LLA7:;}
# 604
Cyc_Tcutil_check_type(s0->loc,_tmp108,Cyc_Tcenv_lookup_type_vars(_tmp108),& Cyc_Tcutil_tmk,1,1,_tmp10C);
_tmp108=Cyc_Tcenv_add_local_var(decl_rgn,_tmp24->loc,_tmp108,_tmp109);_LLA2:;}
# 607
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(decl_rgn,_tmp108,s0),_tmp24,0);
_npop_handler(0);return;}case 4: _LL4E: _tmpB7=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpB1)->f1;_tmpB8=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpB1)->f2;_tmpB9=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpB1)->f3;_tmpBA=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpB1)->f4;_LL4F:
# 613
{struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp1F6;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp1F5;_tmp24->r=(Cyc_Absyn_new_stmt((void*)((_tmp1F5=_cycalloc(sizeof(*_tmp1F5)),((_tmp1F5[0]=((_tmp1F6.tag=2,((_tmp1F6.f1=Cyc_Absyn_new_stmt(_tmp24->r,_tmp24->loc),((_tmp1F6.f2=Cyc_Absyn_skip_stmt(_tmp24->loc),_tmp1F6)))))),_tmp1F5)))),_tmp24->loc))->r;}{
# 615
struct Cyc_Tcenv_Tenv*_tmp116=_tmpAF;
struct Cyc_Absyn_Vardecl*_tmp117=_tmpB8;void**_tmp118;void**_tmp119;_LLB1: _tmp118=(void**)& _tmp117->type;_tmp119=(void**)& _tmp117->rgn;_LLB2: {
void*_tmp11A=Cyc_Tcenv_curr_rgn(_tmpAF);
*_tmp119=_tmp11A;{
void*rgn_typ;
if((unsigned int)_tmpBA){
# 623
union Cyc_Absyn_Nmspace _tmp1FC;struct _dyneither_ptr*_tmp1FB[1];struct _tuple2*_tmp1FA;struct _tuple2*drname=(_tmp1FA=_cycalloc(sizeof(*_tmp1FA)),((_tmp1FA->f1=(((_tmp1FC.Abs_n).val=((_tmp1FB[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"Core",sizeof(char),5),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1FB,sizeof(struct _dyneither_ptr*),1)))),(((_tmp1FC.Abs_n).tag=2,_tmp1FC)))),((_tmp1FA->f2=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"DynamicRegion",sizeof(char),14),_tmp1FA)))));
struct Cyc_Core_Opt*_tmp1FD;void*_tmp11B=Cyc_Absyn_new_evar(((_tmp1FD=_cycalloc(sizeof(*_tmp1FD)),((_tmp1FD->v=& Cyc_Tcutil_trk,_tmp1FD)))),0);
{struct Cyc_Core_Opt*_tmp1FE;rgn_typ=Cyc_Absyn_new_evar(((_tmp1FE=_cycalloc(sizeof(*_tmp1FE)),((_tmp1FE->v=& Cyc_Tcutil_rk,_tmp1FE)))),0);}{
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp208;void*_tmp207[1];struct Cyc_Absyn_AggrInfo _tmp206;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp205;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp11D=
(_tmp205=_cycalloc(sizeof(*_tmp205)),((_tmp205[0]=((_tmp208.tag=11,((_tmp208.f1=((_tmp206.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,drname,0),((_tmp206.targs=(
(_tmp207[0]=rgn_typ,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp207,sizeof(void*),1)))),_tmp206)))),_tmp208)))),_tmp205)));
void*_tmp11E=Cyc_Absyn_at_typ((void*)_tmp11D,_tmp11B,
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);
Cyc_Tcutil_check_type(s0->loc,_tmpAF,Cyc_Tcenv_lookup_type_vars(_tmpAF),& Cyc_Tcutil_tmk,1,0,_tmp11E);{
void*_tmp11F=Cyc_Tcexp_tcExp(_tmpAF,& _tmp11E,_tmpBA);
if(!Cyc_Tcutil_unify(_tmp11E,_tmp11F) && !
Cyc_Tcutil_coerce_assign(_tmpAF,_tmpBA,_tmp11E)){
const char*_tmp20D;void*_tmp20C[2];struct Cyc_String_pa_PrintArg_struct _tmp20B;struct Cyc_String_pa_PrintArg_struct _tmp20A;(_tmp20A.tag=0,((_tmp20A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp11F)),((_tmp20B.tag=0,((_tmp20B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp11E)),((_tmp20C[0]=& _tmp20B,((_tmp20C[1]=& _tmp20A,Cyc_Tcutil_terr(s0->loc,((_tmp20D="expected %s but found %s",_tag_dyneither(_tmp20D,sizeof(char),25))),_tag_dyneither(_tmp20C,sizeof(void*),2)))))))))))));}
if(!Cyc_Tcutil_unify(_tmp11B,(void*)& Cyc_Absyn_UniqueRgn_val) && !
Cyc_Tcutil_unify(_tmp11B,(void*)& Cyc_Absyn_RefCntRgn_val)){
const char*_tmp210;void*_tmp20F;(_tmp20F=0,Cyc_Tcutil_terr(s0->loc,((_tmp210="open is only allowed on unique or reference-counted keys",_tag_dyneither(_tmp210,sizeof(char),57))),_tag_dyneither(_tmp20F,sizeof(void*),0)));}{
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp213;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp212;*_tmp118=(void*)((_tmp212=_cycalloc(sizeof(*_tmp212)),((_tmp212[0]=((_tmp213.tag=15,((_tmp213.f1=rgn_typ,_tmp213)))),_tmp212))));};};};}else{
# 643
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp216;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp215;rgn_typ=(void*)((_tmp215=_cycalloc(sizeof(*_tmp215)),((_tmp215[0]=((_tmp216.tag=2,((_tmp216.f1=_tmpB7,_tmp216)))),_tmp215))));}{
struct Cyc_List_List*_tmp217;_tmp116=Cyc_Tcenv_add_type_vars(decl_rgn,s0->loc,_tmp116,((_tmp217=_cycalloc(sizeof(*_tmp217)),((_tmp217->hd=_tmpB7,((_tmp217->tl=0,_tmp217)))))));};}
# 646
_tmp116=Cyc_Tcenv_add_region(decl_rgn,_tmp116,rgn_typ,_tmpB9,1);
Cyc_Tcutil_check_type(s0->loc,_tmpAF,Cyc_Tcenv_lookup_type_vars(_tmp116),& Cyc_Tcutil_bk,1,0,*_tmp118);
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp21A;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp219;if(!Cyc_Tcutil_unify(*_tmp118,(void*)((_tmp219=_cycalloc(sizeof(*_tmp219)),((_tmp219[0]=((_tmp21A.tag=15,((_tmp21A.f1=rgn_typ,_tmp21A)))),_tmp219)))))){
const char*_tmp21D;void*_tmp21C;(_tmp21C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp21D="region stmt: type of region handle is wrong!",_tag_dyneither(_tmp21D,sizeof(char),45))),_tag_dyneither(_tmp21C,sizeof(void*),0)));}}
# 652
if(!Cyc_Absyn_no_regions)
_tmp116=Cyc_Tcenv_enter_try(decl_rgn,_tmp116);
# 655
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(decl_rgn,Cyc_Tcenv_add_local_var(decl_rgn,_tmp24->loc,_tmp116,_tmpB8),s0),_tmp24,0);
_npop_handler(0);return;};}_LLB0:;};case 1: _LL50: _tmpB6=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpB1)->f1;_LL51: {
# 660
void*_tmp139=Cyc_Tcenv_curr_rgn(_tmpAF);
if(_tmpB6->sc != Cyc_Absyn_Public){
const char*_tmp220;void*_tmp21F;(_tmp21F=0,Cyc_Tcutil_terr(_tmp23->loc,((_tmp220="bad storage class for inner function",_tag_dyneither(_tmp220,sizeof(char),37))),_tag_dyneither(_tmp21F,sizeof(void*),0)));}
# 664
{union Cyc_Absyn_Nmspace _tmp13C=(*_tmpB6->name).f1;union Cyc_Absyn_Nmspace _tmp13D=_tmp13C;struct Cyc_List_List*_tmp13E;switch((_tmp13D.Abs_n).tag){case 1: if((_tmp13D.Rel_n).val == 0){_LLB6: _LLB7:
 goto _LLB5;}else{goto _LLBA;}case 2: _LLB8: _tmp13E=(_tmp13D.Abs_n).val;_LLB9: {
const char*_tmp223;void*_tmp222;(_tmp222=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp223="tc: Abs_n in tcStmt var decl",_tag_dyneither(_tmp223,sizeof(char),29))),_tag_dyneither(_tmp222,sizeof(void*),0)));}default: _LLBA: _LLBB: {
const char*_tmp226;void*_tmp225;(_tmp225=0,Cyc_Tcutil_terr(_tmp23->loc,((_tmp226="explicit namespace not allowed on inner function declaration",_tag_dyneither(_tmp226,sizeof(char),61))),_tag_dyneither(_tmp225,sizeof(void*),0)));}}_LLB5:;}
# 669
(*_tmpB6->name).f1=Cyc_Absyn_Loc_n;
Cyc_Tcutil_check_fndecl_valid_type(_tmp23->loc,_tmpAF,_tmpB6);{
void*t=Cyc_Tcutil_fndecl2typ(_tmpB6);
_tmpB6->attributes=Cyc_Tcutil_transfer_fn_type_atts(t,_tmpB6->attributes);
# 674
{struct Cyc_List_List*atts=_tmpB6->attributes;for(0;(unsigned int)atts;atts=atts->tl){
void*_tmp143=(void*)atts->hd;void*_tmp144=_tmp143;switch(*((int*)_tmp144)){case 7: _LLBD: _LLBE:
 goto _LLC0;case 6: _LLBF: _LLC0:
{const char*_tmp22A;void*_tmp229[1];struct Cyc_String_pa_PrintArg_struct _tmp228;(_tmp228.tag=0,((_tmp228.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp229[0]=& _tmp228,Cyc_Tcutil_terr(_tmp23->loc,((_tmp22A="bad attribute %s for function",_tag_dyneither(_tmp22A,sizeof(char),30))),_tag_dyneither(_tmp229,sizeof(void*),1)))))));}
goto _LLBC;default: _LLC1: _LLC2:
 goto _LLBC;}_LLBC:;}}{
# 682
struct Cyc_Absyn_Vardecl*_tmp22B;struct Cyc_Absyn_Vardecl*vd=(_tmp22B=_cycalloc(sizeof(*_tmp22B)),((_tmp22B->sc=_tmpB6->sc,((_tmp22B->name=_tmpB6->name,((_tmp22B->tq=Cyc_Absyn_const_tqual(0),((_tmp22B->type=
Cyc_Absyn_at_typ(t,_tmp139,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),((_tmp22B->initializer=0,((_tmp22B->rgn=_tmp139,((_tmp22B->attributes=0,((_tmp22B->escapes=0,_tmp22B)))))))))))))))));
# 686
_tmpB6->fn_vardecl=vd;
_tmpAF=Cyc_Tcenv_add_local_var(decl_rgn,_tmp23->loc,_tmpAF,vd);{
struct Cyc_Tcenv_Fenv*old_fenv=(struct Cyc_Tcenv_Fenv*)_check_null(_tmpAF->le);
_tmpAF->le=Cyc_Tcenv_nested_fenv(_tmp23->loc,old_fenv,_tmpB6);
Cyc_Tcstmt_tcStmt(_tmpAF,_tmpB6->body,0);
Cyc_Tcenv_check_delayed_effects(_tmpAF);
Cyc_Tcenv_check_delayed_constraints(_tmpAF);
if(!Cyc_Tcenv_all_labels_resolved(_tmpAF)){
const char*_tmp22E;void*_tmp22D;(_tmp22D=0,Cyc_Tcutil_terr(_tmp23->loc,((_tmp22E="function has goto statements to undefined labels",_tag_dyneither(_tmp22E,sizeof(char),49))),_tag_dyneither(_tmp22D,sizeof(void*),0)));}
_tmpAF->le=old_fenv;
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(decl_rgn,_tmpAF,s0),_tmp24,0);
_npop_handler(0);return;};};};}case 9: _LL52: _tmpB4=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpB1)->f1;_tmpB5=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpB1)->f2;_LL53:
{const char*_tmp22F;unimp_msg_part=((_tmp22F="namespace",_tag_dyneither(_tmp22F,sizeof(char),10)));}goto _LL47;case 10: _LL54: _tmpB2=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpB1)->f1;_tmpB3=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpB1)->f2;_LL55:
{const char*_tmp230;unimp_msg_part=((_tmp230="using",_tag_dyneither(_tmp230,sizeof(char),6)));}goto _LL47;case 5: _LL56: _LL57:
{const char*_tmp231;unimp_msg_part=((_tmp231="type",_tag_dyneither(_tmp231,sizeof(char),5)));}goto _LL47;case 6: _LL58: _LL59:
{const char*_tmp232;unimp_msg_part=((_tmp232="datatype",_tag_dyneither(_tmp232,sizeof(char),9)));}goto _LL47;case 7: _LL5A: _LL5B:
{const char*_tmp233;unimp_msg_part=((_tmp233="enum",_tag_dyneither(_tmp233,sizeof(char),5)));}goto _LL47;case 8: _LL5C: _LL5D:
{const char*_tmp234;unimp_msg_part=((_tmp234="typedef",_tag_dyneither(_tmp234,sizeof(char),8)));}goto _LL47;case 11: _LL5E: _LL5F:
{const char*_tmp235;unimp_msg_part=((_tmp235="extern \"C\"",_tag_dyneither(_tmp235,sizeof(char),11)));}goto _LL47;case 12: _LL60: _LL61:
# 706
{const char*_tmp236;unimp_msg_part=((_tmp236="extern \"C include\"",_tag_dyneither(_tmp236,sizeof(char),19)));}goto _LL47;case 13: _LL62: _LL63:
{const char*_tmp237;unimp_msg_part=((_tmp237="__cyclone_port_on__",_tag_dyneither(_tmp237,sizeof(char),20)));}goto _LL47;default: _LL64: _LL65:
{const char*_tmp238;unimp_msg_part=((_tmp238="__cyclone_port_off__",_tag_dyneither(_tmp238,sizeof(char),21)));}goto _LL47;}_LL47:;};}{
# 711
const char*_tmp23C;void*_tmp23B[1];struct Cyc_String_pa_PrintArg_struct _tmp23A;(int)_throw(((_tmp23A.tag=0,((_tmp23A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)unimp_msg_part),((_tmp23B[0]=& _tmp23A,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp23C="tcStmt: nested %s declarations unimplemented",_tag_dyneither(_tmp23C,sizeof(char),45))),_tag_dyneither(_tmp23B,sizeof(void*),1)))))))));};
# 429
;_pop_region(decl_rgn);}default: _LL32: _tmp22=((struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp21)->f1;_LL33: {
# 715
void*rgn_type=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp23F;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp23E;void*etype=(void*)((_tmp23E=_cycalloc(sizeof(*_tmp23E)),((_tmp23E[0]=((_tmp23F.tag=15,((_tmp23F.f1=rgn_type,_tmp23F)))),_tmp23E))));
if(!Cyc_Tcutil_unify(Cyc_Tcexp_tcExp(te,& etype,_tmp22),etype)){
const char*_tmp243;void*_tmp242[1];struct Cyc_String_pa_PrintArg_struct _tmp241;(_tmp241.tag=0,((_tmp241.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp22->topt))),((_tmp242[0]=& _tmp241,Cyc_Tcutil_terr(_tmp22->loc,((_tmp243="expecting region_t but found %s",_tag_dyneither(_tmp243,sizeof(char),32))),_tag_dyneither(_tmp242,sizeof(void*),1)))))));}
# 720
Cyc_Tcenv_check_rgn_resetable(te,s0->loc,rgn_type);
return;}}_LL11:;};}
