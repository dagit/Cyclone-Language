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
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
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
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 364
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 385
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 390
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 444 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 460
typedef void*Cyc_Absyn_funcparams_t;
# 463
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 525
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 551
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 558
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 576
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 740 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 942
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 944
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 958
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 967
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 986
void*Cyc_Absyn_exn_typ();
# 1008
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1024
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1038
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple2*,unsigned int);
# 1099
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
# 1112
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1123
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1128
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1132
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);
# 1184
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
# 1208
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
# 373
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
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
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_List_List*patvars);
# 62
void Cyc_Tcpat_check_switch_exhaustive(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*p);
void Cyc_Tcpat_check_catch_overlap(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
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
void*_tmp0=e->r;void*_tmp1=_tmp0;struct Cyc_Absyn_Exp*_tmp3;struct Cyc_Absyn_Exp*_tmp5;_LL1: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp2=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1;if(_tmp2->tag != 4)goto _LL3;else{_tmp3=_tmp2->f1;if(_tmp2->f2 != Cyc_Absyn_PostInc)goto _LL3;}}_LL2:
# 59
{struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmp182;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp181;e->r=(void*)((_tmp181=_cycalloc(sizeof(*_tmp181)),((_tmp181[0]=((_tmp182.tag=4,((_tmp182.f1=_tmp3,((_tmp182.f2=Cyc_Absyn_PreInc,_tmp182)))))),_tmp181))));}goto _LL0;_LL3: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp4=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1;if(_tmp4->tag != 4)goto _LL5;else{_tmp5=_tmp4->f1;if(_tmp4->f2 != Cyc_Absyn_PostDec)goto _LL5;}}_LL4:
# 61
{struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmp185;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp184;e->r=(void*)((_tmp184=_cycalloc(sizeof(*_tmp184)),((_tmp184[0]=((_tmp185.tag=4,((_tmp185.f1=_tmp5,((_tmp185.f2=Cyc_Absyn_PreDec,_tmp185)))))),_tmp184))));}goto _LL0;_LL5:;_LL6:
 goto _LL0;_LL0:;}
# 66
static void Cyc_Tcstmt_decorate_stmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s){
Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));
s->try_depth=Cyc_Tcenv_get_try_depth(te);}struct _tuple15{struct Cyc_Absyn_Tvar*f1;int f2;};
# 72
static void Cyc_Tcstmt_pattern_synth(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcpat_TcPatResult pat_res,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*where_opt,int new_block){
# 74
struct _tuple1*_tmpB;struct Cyc_List_List*_tmpC;struct Cyc_Tcpat_TcPatResult _tmpA=pat_res;_tmpB=_tmpA.tvars_and_bounds_opt;_tmpC=_tmpA.patvars;{
struct _tuple1 _tmpD=((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmpC);struct Cyc_List_List*_tmpF;struct _tuple1 _tmpE=_tmpD;_tmpF=_tmpE.f1;{
struct Cyc_List_List*_tmp10=_tmpB == 0?0:((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst,(*_tmpB).f1);
struct Cyc_List_List*_tmp11=_tmpB == 0?0:(*_tmpB).f2;
struct Cyc_List_List*_tmp12=_tmpB == 0?0:((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst,((struct Cyc_List_List*(*)(int(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_filter)((int(*)(struct _tuple15*))Cyc_Core_snd,(*_tmpB).f1));
struct _RegionHandle _tmp13=_new_region("r");struct _RegionHandle*r=& _tmp13;_push_region(r);{
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r,loc,te,_tmp10);
for(0;_tmp12 != 0;_tmp12=_tmp12->tl){
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp188;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp187;te2=Cyc_Tcenv_add_region(r,te2,(void*)((_tmp187=_cycalloc(sizeof(*_tmp187)),((_tmp187[0]=((_tmp188.tag=2,((_tmp188.f1=(struct Cyc_Absyn_Tvar*)_tmp12->hd,_tmp188)))),_tmp187)))),0,1);}
te2=Cyc_Tcenv_new_outlives_constraints(r,te2,_tmp11,loc);
if(new_block)
te2=Cyc_Tcenv_new_block(r,loc,te2);{
void*_tmp16=Cyc_Tcenv_curr_rgn(te2);
{struct Cyc_List_List*_tmp17=_tmpF;for(0;_tmp17 != 0;_tmp17=_tmp17->tl){
if((struct Cyc_Absyn_Vardecl**)_tmp17->hd != 0){
te2=Cyc_Tcenv_add_pat_var(r,loc,te2,*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)_tmp17->hd)));
(*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)_tmp17->hd)))->rgn=_tmp16;}}}
# 93
if(where_opt != 0){
const char*_tmp189;Cyc_Tcexp_tcTest(te2,where_opt,((_tmp189="switch clause guard",_tag_dyneither(_tmp189,sizeof(char),20))));}
# 96
if(_tmpF != 0)
te2=Cyc_Tcenv_set_encloser(r,te2,s);
Cyc_Tcstmt_tcStmt(te2,s,0);};}
# 101
if(_tmpF != 0)
Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));
# 79
;_pop_region(r);};};}
# 105
static struct Cyc_List_List*Cyc_Tcstmt_cmap_c(struct _RegionHandle*r,void*(*f)(void*,void*),void*env,const struct Cyc_Tcenv_CList*const x){
# 107
if(x == (const struct Cyc_Tcenv_CList*)0)return 0;else{
struct Cyc_List_List*_tmp18A;return(_tmp18A=_region_malloc(r,sizeof(*_tmp18A)),((_tmp18A->hd=f(env,x->hd),((_tmp18A->tl=Cyc_Tcstmt_cmap_c(r,f,env,x->tl),_tmp18A)))));}}
# 113
static void Cyc_Tcstmt_tcStmtRefine(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,void*r1,void*r2,int new_block){
# 116
struct _RegionHandle _tmp1A=_new_region("r");struct _RegionHandle*r=& _tmp1A;_push_region(r);
{struct _tuple13*tk=0;
struct Cyc_Tcenv_Tenv*_tmp1B=Cyc_Tcenv_add_region_equality(r,te,r1,r2,& tk,s0->loc);
Cyc_Tcstmt_tcStmt(_tmp1B,s0,new_block);
if(tk != 0)
((*((struct _tuple13*)_check_null(tk))).f1)->kind=(*((struct _tuple13*)_check_null(tk))).f2;}
# 117
;_pop_region(r);}
# 125
static int Cyc_Tcstmt_stmt_temp_var_counter=0;
static struct _tuple2*Cyc_Tcstmt_stmt_temp_var(){
int _tmp1C=Cyc_Tcstmt_stmt_temp_var_counter ++;
struct _dyneither_ptr*_tmp197;const char*_tmp196;void*_tmp195[1];struct Cyc_Int_pa_PrintArg_struct _tmp194;struct _tuple2*_tmp193;struct _tuple2*res=(_tmp193=_cycalloc(sizeof(*_tmp193)),((_tmp193->f1=Cyc_Absyn_Loc_n,((_tmp193->f2=((_tmp197=_cycalloc(sizeof(*_tmp197)),((_tmp197[0]=(struct _dyneither_ptr)((_tmp194.tag=1,((_tmp194.f1=(unsigned int)_tmp1C,((_tmp195[0]=& _tmp194,Cyc_aprintf(((_tmp196="_stmttmp%X",_tag_dyneither(_tmp196,sizeof(char),11))),_tag_dyneither(_tmp195,sizeof(void*),1)))))))),_tmp197)))),_tmp193)))));
return res;}
# 139 "tcstmt.cyc"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,int new_block){
# 141
Cyc_Tcstmt_decorate_stmt(te,s0);{
# 143
void*_tmp22=s0->r;void*_tmp23=_tmp22;struct Cyc_Absyn_Exp*_tmp26;struct Cyc_Absyn_Stmt*_tmp28;struct Cyc_Absyn_Stmt*_tmp29;struct Cyc_Absyn_Exp*_tmp2B;struct Cyc_Absyn_Exp*_tmp2D;struct Cyc_Absyn_Stmt*_tmp2E;struct Cyc_Absyn_Stmt*_tmp2F;struct Cyc_Absyn_Exp*_tmp31;struct Cyc_Absyn_Stmt*_tmp32;struct Cyc_Absyn_Stmt*_tmp33;struct Cyc_Absyn_Exp*_tmp35;struct Cyc_Absyn_Exp*_tmp36;struct Cyc_Absyn_Stmt*_tmp37;struct Cyc_Absyn_Exp*_tmp38;struct Cyc_Absyn_Stmt*_tmp39;struct Cyc_Absyn_Stmt*_tmp3A;struct Cyc_Absyn_Stmt*_tmp3C;struct Cyc_Absyn_Exp*_tmp3D;struct Cyc_Absyn_Stmt*_tmp3E;struct Cyc_Absyn_Stmt**_tmp40;struct Cyc_Absyn_Stmt**_tmp42;struct _dyneither_ptr*_tmp44;struct Cyc_Absyn_Stmt**_tmp45;struct Cyc_List_List*_tmp47;struct Cyc_Absyn_Switch_clause***_tmp48;struct _dyneither_ptr*_tmp4A;struct Cyc_Absyn_Stmt*_tmp4B;struct Cyc_Absyn_Exp**_tmp4D;struct Cyc_List_List*_tmp4E;struct Cyc_Absyn_Stmt*_tmp50;struct Cyc_List_List*_tmp51;struct Cyc_Absyn_Decl*_tmp53;struct Cyc_Absyn_Stmt*_tmp54;struct Cyc_Absyn_Exp*_tmp56;_LL8: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp24=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp23;if(_tmp24->tag != 0)goto _LLA;}_LL9:
# 145
 return;_LLA: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp25=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp23;if(_tmp25->tag != 1)goto _LLC;else{_tmp26=_tmp25->f1;}}_LLB:
# 148
 Cyc_Tcexp_tcExp(te,0,_tmp26);
if(!Cyc_Tcexp_in_stmt_exp)
Cyc_Tcstmt_simplify_unused_result_exp(_tmp26);
return;_LLC: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp27=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp23;if(_tmp27->tag != 2)goto _LLE;else{_tmp28=_tmp27->f1;_tmp29=_tmp27->f2;}}_LLD: {
# 154
struct _RegionHandle _tmp57=_new_region("r");struct _RegionHandle*r=& _tmp57;_push_region(r);{
struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct _tmp19A;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp199;struct Cyc_Tcenv_Tenv*_tmp58=Cyc_Tcenv_set_next(r,te,(void*)((_tmp199=_cycalloc(sizeof(*_tmp199)),((_tmp199[0]=((_tmp19A.tag=3,((_tmp19A.f1=_tmp29,_tmp19A)))),_tmp199)))));
Cyc_Tcstmt_tcStmt(_tmp58,_tmp28,1);}
# 158
Cyc_Tcstmt_tcStmt(te,_tmp29,1);
_npop_handler(0);return;
# 154
;_pop_region(r);}_LLE: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp2A=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp23;if(_tmp2A->tag != 3)goto _LL10;else{_tmp2B=_tmp2A->f1;}}_LLF: {
# 179 "tcstmt.cyc"
void*t=Cyc_Tcenv_return_typ(te);
if(_tmp2B == 0){
void*_tmp5B=Cyc_Tcutil_compress(t);void*_tmp5C=_tmp5B;_LL2B: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp5D=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp5C;if(_tmp5D->tag != 0)goto _LL2D;}_LL2C:
 goto _LL2A;_LL2D: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5E=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5C;if(_tmp5E->tag != 7)goto _LL2F;}_LL2E:
 goto _LL30;_LL2F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C;if(_tmp5F->tag != 6)goto _LL31;}_LL30:
# 185
{const char*_tmp19E;void*_tmp19D[1];struct Cyc_String_pa_PrintArg_struct _tmp19C;(_tmp19C.tag=0,((_tmp19C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp19D[0]=& _tmp19C,Cyc_Tcutil_warn(s0->loc,((_tmp19E="should return a value of type %s",_tag_dyneither(_tmp19E,sizeof(char),33))),_tag_dyneither(_tmp19D,sizeof(void*),1)))))));}
goto _LL2A;_LL31:;_LL32:
# 188
{const char*_tmp1A2;void*_tmp1A1[1];struct Cyc_String_pa_PrintArg_struct _tmp1A0;(_tmp1A0.tag=0,((_tmp1A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1A1[0]=& _tmp1A0,Cyc_Tcutil_terr(s0->loc,((_tmp1A2="must return a value of type %s",_tag_dyneither(_tmp1A2,sizeof(char),31))),_tag_dyneither(_tmp1A1,sizeof(void*),1)))))));}
goto _LL2A;_LL2A:;}else{
# 193
int bogus=0;
struct Cyc_Absyn_Exp*e=_tmp2B;
Cyc_Tcexp_tcExp(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
{const char*_tmp1A7;void*_tmp1A6[2];struct Cyc_String_pa_PrintArg_struct _tmp1A5;struct Cyc_String_pa_PrintArg_struct _tmp1A4;(_tmp1A4.tag=0,((_tmp1A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp1A5.tag=0,((_tmp1A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp1A6[0]=& _tmp1A5,((_tmp1A6[1]=& _tmp1A4,Cyc_Tcutil_terr(s0->loc,((_tmp1A7="returns value of type %s but requires %s",_tag_dyneither(_tmp1A7,sizeof(char),41))),_tag_dyneither(_tmp1A6,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{
# 201
struct _RegionHandle _tmp6A=_new_region("temp");struct _RegionHandle*temp=& _tmp6A;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){
const char*_tmp1AA;void*_tmp1A9;(_tmp1A9=0,Cyc_Tcutil_terr(e->loc,((_tmp1AA="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp1AA,sizeof(char),49))),_tag_dyneither(_tmp1A9,sizeof(void*),0)));}
# 202
;_pop_region(temp);};}
# 206
return;}_LL10: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp2C=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp23;if(_tmp2C->tag != 4)goto _LL12;else{_tmp2D=_tmp2C->f1;_tmp2E=_tmp2C->f2;_tmp2F=_tmp2C->f3;}}_LL11: {
# 209
const char*_tmp1AB;struct Cyc_Tcexp_TestEnv _tmp6D=Cyc_Tcexp_tcTest(te,_tmp2D,((_tmp1AB="if statement",_tag_dyneither(_tmp1AB,sizeof(char),13))));struct _tuple0*_tmp6F;int _tmp70;struct Cyc_Tcexp_TestEnv _tmp6E=_tmp6D;_tmp6F=_tmp6E.eq;_tmp70=_tmp6E.isTrue;
{struct _tuple0*_tmp71=_tmp6F;void*_tmp72;void*_tmp73;_LL34: if(_tmp71 == 0)goto _LL36;_tmp72=_tmp71->f1;_tmp73=_tmp71->f2;_LL35:
# 212
 if(_tmp70){
Cyc_Tcstmt_tcStmtRefine(te,_tmp2E,_tmp72,_tmp73,1);
Cyc_Tcstmt_tcStmt(te,_tmp2F,1);}else{
# 217
Cyc_Tcstmt_tcStmt(te,_tmp2E,1);
Cyc_Tcstmt_tcStmtRefine(te,_tmp2F,_tmp72,_tmp73,1);}
# 220
goto _LL33;_LL36:;_LL37:
# 222
 Cyc_Tcstmt_tcStmt(te,_tmp2E,1);
Cyc_Tcstmt_tcStmt(te,_tmp2F,1);
goto _LL33;_LL33:;}
# 226
return;}_LL12: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp30=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp23;if(_tmp30->tag != 5)goto _LL14;else{_tmp31=(_tmp30->f1).f1;_tmp32=(_tmp30->f1).f2;_tmp33=_tmp30->f2;}}_LL13:
# 229
 Cyc_Tcstmt_decorate_stmt(te,_tmp32);
{const char*_tmp1AC;Cyc_Tcexp_tcTest(te,_tmp31,((_tmp1AC="while loop",_tag_dyneither(_tmp1AC,sizeof(char),11))));}{
struct _RegionHandle _tmp76=_new_region("r");struct _RegionHandle*r=& _tmp76;_push_region(r);
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_in_loop(r,te,_tmp32),_tmp33,1);
# 234
_npop_handler(0);return;
# 231
;_pop_region(r);};_LL14: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp34=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp23;if(_tmp34->tag != 9)goto _LL16;else{_tmp35=_tmp34->f1;_tmp36=(_tmp34->f2).f1;_tmp37=(_tmp34->f2).f2;_tmp38=(_tmp34->f3).f1;_tmp39=(_tmp34->f3).f2;_tmp3A=_tmp34->f4;}}_LL15:
# 237
 Cyc_Tcstmt_decorate_stmt(te,_tmp37);
Cyc_Tcstmt_decorate_stmt(te,_tmp39);
Cyc_Tcexp_tcExp(te,0,_tmp35);
{const char*_tmp1AD;Cyc_Tcexp_tcTest(te,_tmp36,((_tmp1AD="for loop",_tag_dyneither(_tmp1AD,sizeof(char),9))));}{
struct _RegionHandle _tmp78=_new_region("r");struct _RegionHandle*r=& _tmp78;_push_region(r);{
struct Cyc_Tcenv_Tenv*_tmp79=Cyc_Tcenv_set_in_loop(r,te,_tmp39);
Cyc_Tcstmt_tcStmt(_tmp79,_tmp3A,1);
Cyc_Tcexp_tcExp(_tmp79,0,_tmp38);
Cyc_Tcstmt_simplify_unused_result_exp(_tmp38);}
# 247
_npop_handler(0);return;
# 241
;_pop_region(r);};_LL16: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp3B=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp23;if(_tmp3B->tag != 14)goto _LL18;else{_tmp3C=_tmp3B->f1;_tmp3D=(_tmp3B->f2).f1;_tmp3E=(_tmp3B->f2).f2;}}_LL17:
# 250
 Cyc_Tcstmt_decorate_stmt(te,_tmp3E);{
struct _RegionHandle _tmp7A=_new_region("r");struct _RegionHandle*r=& _tmp7A;_push_region(r);
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_in_loop(r,te,_tmp3E),_tmp3C,1);
# 254
{const char*_tmp1AE;Cyc_Tcexp_tcTest(te,_tmp3D,((_tmp1AE="do loop",_tag_dyneither(_tmp1AE,sizeof(char),8))));}
_npop_handler(0);return;
# 251
;_pop_region(r);};_LL18: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp3F=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp23;if(_tmp3F->tag != 6)goto _LL1A;else{_tmp40=(struct Cyc_Absyn_Stmt**)& _tmp3F->f1;}}_LL19:
# 259
 Cyc_Tcenv_process_break(te,s0,_tmp40);return;_LL1A: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp41=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp23;if(_tmp41->tag != 7)goto _LL1C;else{_tmp42=(struct Cyc_Absyn_Stmt**)& _tmp41->f1;}}_LL1B:
 Cyc_Tcenv_process_continue(te,s0,_tmp42);return;_LL1C: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp43=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp23;if(_tmp43->tag != 8)goto _LL1E;else{_tmp44=_tmp43->f1;_tmp45=(struct Cyc_Absyn_Stmt**)& _tmp43->f2;}}_LL1D:
 Cyc_Tcenv_process_goto(te,s0,_tmp44,_tmp45);return;_LL1E: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp46=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp23;if(_tmp46->tag != 11)goto _LL20;else{_tmp47=_tmp46->f1;_tmp48=(struct Cyc_Absyn_Switch_clause***)& _tmp46->f2;}}_LL1F: {
# 264
const struct _tuple12*_tmp7C=Cyc_Tcenv_process_fallthru(te,s0,_tmp48);
if(_tmp7C == (const struct _tuple12*)0){
{const char*_tmp1B1;void*_tmp1B0;(_tmp1B0=0,Cyc_Tcutil_terr(s0->loc,((_tmp1B1="fallthru not in a non-last case",_tag_dyneither(_tmp1B1,sizeof(char),32))),_tag_dyneither(_tmp1B0,sizeof(void*),0)));}
return;}{
# 269
struct Cyc_List_List*_tmp7F=(*_tmp7C).f2;
const struct Cyc_Tcenv_CList*_tmp80=(*_tmp7C).f3;
struct Cyc_List_List*instantiation=
((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_make_inst_var,Cyc_Tcenv_lookup_type_vars(te),_tmp7F);
struct Cyc_List_List*_tmp81=((struct Cyc_List_List*(*)(struct _RegionHandle*r,void*(*f)(struct Cyc_List_List*,void*),struct Cyc_List_List*env,const struct Cyc_Tcenv_CList*x))Cyc_Tcstmt_cmap_c)(Cyc_Core_heap_region,Cyc_Tcutil_substitute,instantiation,_tmp80);
for(0;_tmp81 != 0  && _tmp47 != 0;(_tmp81=_tmp81->tl,_tmp47=_tmp47->tl)){
# 276
int bogus=0;
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp47->hd);
if(!Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)_tmp47->hd,(void*)_tmp81->hd,& bogus)){
{const char*_tmp1B6;void*_tmp1B5[2];struct Cyc_String_pa_PrintArg_struct _tmp1B4;struct Cyc_String_pa_PrintArg_struct _tmp1B3;(_tmp1B3.tag=0,((_tmp1B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 281
Cyc_Absynpp_typ2string((void*)_tmp81->hd)),((_tmp1B4.tag=0,((_tmp1B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp47->hd)->topt))),((_tmp1B5[0]=& _tmp1B4,((_tmp1B5[1]=& _tmp1B3,Cyc_Tcutil_terr(s0->loc,((_tmp1B6="fallthru argument has type %s but pattern variable has type %s",_tag_dyneither(_tmp1B6,sizeof(char),63))),_tag_dyneither(_tmp1B5,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{
# 285
struct _RegionHandle _tmp86=_new_region("temp");struct _RegionHandle*temp=& _tmp86;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(void*)_tmp81->hd) && !
Cyc_Tcutil_is_noalias_path(temp,(struct Cyc_Absyn_Exp*)_tmp47->hd)){
const char*_tmp1B9;void*_tmp1B8;(_tmp1B8=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp47->hd)->loc,((_tmp1B9="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp1B9,sizeof(char),49))),_tag_dyneither(_tmp1B8,sizeof(void*),0)));}
# 286
;_pop_region(temp);};}
# 291
if(_tmp47 != 0){
const char*_tmp1BC;void*_tmp1BB;(_tmp1BB=0,Cyc_Tcutil_terr(s0->loc,((_tmp1BC="too many arguments in explicit fallthru",_tag_dyneither(_tmp1BC,sizeof(char),40))),_tag_dyneither(_tmp1BB,sizeof(void*),0)));}
if(_tmp81 != 0){
const char*_tmp1BF;void*_tmp1BE;(_tmp1BE=0,Cyc_Tcutil_terr(s0->loc,((_tmp1BF="too few arguments in explicit fallthru",_tag_dyneither(_tmp1BF,sizeof(char),39))),_tag_dyneither(_tmp1BE,sizeof(void*),0)));}
return;};}_LL20: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp49=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp23;if(_tmp49->tag != 13)goto _LL22;else{_tmp4A=_tmp49->f1;_tmp4B=_tmp49->f2;}}_LL21: {
# 303
struct _RegionHandle _tmp8D=_new_region("r");struct _RegionHandle*r=& _tmp8D;_push_region(r);{
struct _dyneither_ptr*_tmp1CC;const char*_tmp1CB;void*_tmp1CA[1];struct Cyc_String_pa_PrintArg_struct _tmp1C9;struct Cyc_Absyn_Tvar*_tmp1C8;Cyc_Tcstmt_tcStmt(Cyc_Tcenv_new_named_block(r,s0->loc,Cyc_Tcenv_add_label(te,_tmp4A,_tmp4B),(
(_tmp1C8=_cycalloc(sizeof(*_tmp1C8)),((_tmp1C8->name=((_tmp1CC=_cycalloc(sizeof(*_tmp1CC)),((_tmp1CC[0]=(struct _dyneither_ptr)((_tmp1C9.tag=0,((_tmp1C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4A),((_tmp1CA[0]=& _tmp1C9,Cyc_aprintf(((_tmp1CB="`%s",_tag_dyneither(_tmp1CB,sizeof(char),4))),_tag_dyneither(_tmp1CA,sizeof(void*),1)))))))),_tmp1CC)))),((_tmp1C8->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp1C8->kind=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmp1C8))))))))),_tmp4B,0);}
# 309
_npop_handler(0);return;
# 303
;_pop_region(r);}_LL22: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp4C=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp23;if(_tmp4C->tag != 10)goto _LL24;else{_tmp4D=(struct Cyc_Absyn_Exp**)& _tmp4C->f1;_tmp4E=_tmp4C->f2;}}_LL23: {
# 315
struct Cyc_Absyn_Exp*_tmp93=*_tmp4D;
if(!Cyc_Tcutil_is_var_exp(_tmp93)){
struct _tuple2*_tmp94=Cyc_Tcstmt_stmt_temp_var();
struct Cyc_Absyn_Stmt*_tmp95=Cyc_Absyn_new_stmt(s0->r,s0->loc);
struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp1CF;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp1CE;struct Cyc_Absyn_Decl*_tmp96=Cyc_Absyn_let_decl(Cyc_Absyn_new_pat((void*)((_tmp1CE=_cycalloc(sizeof(*_tmp1CE)),((_tmp1CE[0]=((_tmp1CF.tag=1,((_tmp1CF.f1=Cyc_Absyn_new_vardecl(_tmp94,
Cyc_Absyn_new_evar(0,0),0),((_tmp1CF.f2=
# 322
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,_tmp93->loc),_tmp1CF)))))),_tmp1CE)))),_tmp93->loc),_tmp93,s0->loc);
# 325
struct Cyc_Absyn_Stmt*_tmp97=Cyc_Absyn_decl_stmt(_tmp96,_tmp95,s0->loc);
# 327
s0->r=_tmp97->r;
*_tmp4D=Cyc_Absyn_var_exp(_tmp94,_tmp93->loc);
Cyc_Tcstmt_tcStmt(te,s0,new_block);
return;}
# 333
Cyc_Tcexp_tcExp(te,0,_tmp93);{
void*_tmp9A=(void*)_check_null(_tmp93->topt);
# 338
struct _RegionHandle _tmp9B=_new_region("r");struct _RegionHandle*r=& _tmp9B;_push_region(r);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmp9A) && !Cyc_Tcutil_is_noalias_path(r,_tmp93)){
const char*_tmp1D2;void*_tmp1D1;(_tmp1D1=0,Cyc_Tcutil_terr(_tmp93->loc,((_tmp1D2="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp1D2,sizeof(char),49))),_tag_dyneither(_tmp1D1,sizeof(void*),0)));}{
struct Cyc_Tcenv_Tenv*_tmp9E=Cyc_Tcenv_set_in_switch(r,te);
_tmp9E=Cyc_Tcenv_clear_fallthru(r,_tmp9E);{
struct Cyc_List_List*scs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp4E);for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Pat*_tmp9F=((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern;
struct Cyc_Tcpat_TcPatResult _tmpA0=Cyc_Tcpat_tcPat(_tmp9E,_tmp9F,& _tmp9A,_tmp93);
struct Cyc_List_List*_tmpA1=_tmpA0.tvars_and_bounds_opt == 0?0:
((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst,(*_tmpA0.tvars_and_bounds_opt).f1);
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp9F->topt),_tmp9A)){
{const char*_tmp1D7;void*_tmp1D6[2];struct Cyc_String_pa_PrintArg_struct _tmp1D5;struct Cyc_String_pa_PrintArg_struct _tmp1D4;(_tmp1D4.tag=0,((_tmp1D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmp9F->topt))),((_tmp1D5.tag=0,((_tmp1D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp9A)),((_tmp1D6[0]=& _tmp1D5,((_tmp1D6[1]=& _tmp1D4,Cyc_Tcutil_terr(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,((_tmp1D7="switch on type %s, but case expects type %s",_tag_dyneither(_tmp1D7,sizeof(char),44))),_tag_dyneither(_tmp1D6,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}else{
# 354
Cyc_Tcpat_check_pat_regions(_tmp9E,_tmp9F,_tmpA0.patvars);}
{struct Cyc_Core_Opt*_tmp1D8;((struct Cyc_Absyn_Switch_clause*)scs->hd)->pat_vars=((_tmp1D8=_cycalloc(sizeof(*_tmp1D8)),((_tmp1D8->v=_tmpA0.patvars,_tmp1D8))));}
# 357
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,_tmp9E,_tmpA0,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body,((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause,1);
# 359
if(_tmpA0.tvars_and_bounds_opt != 0  && (*_tmpA0.tvars_and_bounds_opt).f2 != 0)
# 361
_tmp9E=Cyc_Tcenv_clear_fallthru(r,_tmp9E);else{
# 363
struct Cyc_List_List*_tmpA7=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmpA0.patvars)).f1);
_tmpA7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpA7);
_tmp9E=Cyc_Tcenv_set_fallthru(r,_tmp9E,_tmpA1,_tmpA7,(struct Cyc_Absyn_Switch_clause*)scs->hd);}}};};
# 369
Cyc_Tcpat_check_switch_exhaustive(s0->loc,te,_tmp4E);
_npop_handler(0);return;
# 338
;_pop_region(r);};}_LL24: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp4F=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp23;if(_tmp4F->tag != 15)goto _LL26;else{_tmp50=_tmp4F->f1;_tmp51=_tmp4F->f2;}}_LL25:
# 377
{struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp1DB;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp1DA;_tmp50->r=(Cyc_Absyn_new_stmt((void*)((_tmp1DA=_cycalloc(sizeof(*_tmp1DA)),((_tmp1DA[0]=((_tmp1DB.tag=2,((_tmp1DB.f1=Cyc_Absyn_new_stmt(_tmp50->r,_tmp50->loc),((_tmp1DB.f2=Cyc_Absyn_skip_stmt(_tmp50->loc),_tmp1DB)))))),_tmp1DA)))),_tmp50->loc))->r;}{
# 381
struct _RegionHandle _tmpAA=_new_region("r");struct _RegionHandle*r=& _tmpAA;_push_region(r);
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(r,Cyc_Tcenv_enter_try(r,te),s0),_tmp50,1);{
# 385
struct _RegionHandle _tmpAB=_new_region("r2");struct _RegionHandle*r2=& _tmpAB;_push_region(r2);{
# 387
struct Cyc_Tcenv_Tenv*_tmpAC=Cyc_Tcenv_set_in_switch(r2,te);
_tmpAC=Cyc_Tcenv_clear_fallthru(r2,_tmpAC);{
struct Cyc_List_List*_tmpAD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp51);for(0;_tmpAD != 0;_tmpAD=_tmpAD->tl){
struct Cyc_Absyn_Pat*_tmpAE=((struct Cyc_Absyn_Switch_clause*)_tmpAD->hd)->pattern;
void*_tmpAF=Cyc_Absyn_exn_typ();
struct Cyc_Tcpat_TcPatResult _tmpB0=Cyc_Tcpat_tcPat(_tmpAC,_tmpAE,& _tmpAF,0);
# 395
struct Cyc_List_List*_tmpB1=_tmpB0.tvars_and_bounds_opt == 0?0:
((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst,(*_tmpB0.tvars_and_bounds_opt).f1);
if(!Cyc_Tcutil_unify((void*)_check_null(_tmpAE->topt),_tmpAF)){
const char*_tmp1DF;void*_tmp1DE[1];struct Cyc_String_pa_PrintArg_struct _tmp1DD;(_tmp1DD.tag=0,((_tmp1DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmpAE->topt))),((_tmp1DE[0]=& _tmp1DD,Cyc_Tcutil_terr(((struct Cyc_Absyn_Switch_clause*)_tmpAD->hd)->loc,((_tmp1DF="expected datatype exn@ but found %s",_tag_dyneither(_tmp1DF,sizeof(char),36))),_tag_dyneither(_tmp1DE,sizeof(void*),1)))))));}else{
# 401
Cyc_Tcpat_check_pat_regions(_tmpAC,_tmpAE,_tmpB0.patvars);}
{struct Cyc_Core_Opt*_tmp1E0;((struct Cyc_Absyn_Switch_clause*)_tmpAD->hd)->pat_vars=((_tmp1E0=_cycalloc(sizeof(*_tmp1E0)),((_tmp1E0->v=_tmpB0.patvars,_tmp1E0))));}
# 404
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)_tmpAD->hd)->loc,_tmpAC,_tmpB0,((struct Cyc_Absyn_Switch_clause*)_tmpAD->hd)->body,((struct Cyc_Absyn_Switch_clause*)_tmpAD->hd)->where_clause,1);
# 406
if(_tmpB0.tvars_and_bounds_opt != 0  && (*_tmpB0.tvars_and_bounds_opt).f2 != 0)
# 408
_tmpAC=Cyc_Tcenv_clear_fallthru(r2,_tmpAC);else{
# 410
struct Cyc_List_List*_tmpB6=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmpB0.patvars)).f1);
_tmpB6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpB6);
_tmpAC=Cyc_Tcenv_set_fallthru(r2,_tmpAC,_tmpB1,_tmpB6,(struct Cyc_Absyn_Switch_clause*)_tmpAD->hd);}}};}
# 416
Cyc_Tcpat_check_catch_overlap(s0->loc,te,_tmp51);
_npop_handler(1);return;
# 385
;_pop_region(r2);};
# 381
;_pop_region(r);};_LL26: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp52=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp23;if(_tmp52->tag != 12)goto _LL28;else{_tmp53=_tmp52->f1;_tmp54=_tmp52->f2;}}_LL27: {
# 420
struct _dyneither_ptr unimp_msg_part;
struct _RegionHandle _tmpB7=_new_region("decl_rgn");struct _RegionHandle*decl_rgn=& _tmpB7;_push_region(decl_rgn);{
struct Cyc_Tcenv_Tenv*te2;
if(new_block)
te2=Cyc_Tcenv_new_block(decl_rgn,s0->loc,te);else{
# 426
te2=Cyc_Tcenv_copy_tenv(decl_rgn,te);}{
struct Cyc_Tcenv_Tenv*_tmpB8=te2;
void*_tmpB9=_tmp53->r;void*_tmpBA=_tmpB9;struct Cyc_Absyn_Vardecl*_tmpBC;struct Cyc_Absyn_Pat*_tmpBE;struct Cyc_Core_Opt**_tmpBF;struct Cyc_Absyn_Exp**_tmpC0;struct Cyc_List_List*_tmpC2;struct Cyc_Absyn_Tvar*_tmpC4;struct Cyc_Absyn_Vardecl*_tmpC5;int _tmpC6;struct Cyc_Absyn_Exp*_tmpC7;struct Cyc_Absyn_Fndecl*_tmpC9;struct _dyneither_ptr*_tmpCB;struct Cyc_List_List*_tmpCC;struct _tuple2*_tmpCE;struct Cyc_List_List*_tmpCF;_LL39: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpBB=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpBA;if(_tmpBB->tag != 0)goto _LL3B;else{_tmpBC=_tmpBB->f1;}}_LL3A: {
# 430
enum Cyc_Absyn_Scope _tmpD9;union Cyc_Absyn_Nmspace _tmpDA;struct _dyneither_ptr*_tmpDB;struct Cyc_Absyn_Tqual _tmpDC;void*_tmpDD;struct Cyc_Absyn_Exp*_tmpDE;void**_tmpDF;struct Cyc_List_List*_tmpE0;struct Cyc_Absyn_Vardecl*_tmpD8=_tmpBC;_tmpD9=_tmpD8->sc;_tmpDA=(_tmpD8->name)->f1;_tmpDB=(_tmpD8->name)->f2;_tmpDC=_tmpD8->tq;_tmpDD=_tmpD8->type;_tmpDE=_tmpD8->initializer;_tmpDF=(void**)& _tmpD8->rgn;_tmpE0=_tmpD8->attributes;{
void*_tmpE1=Cyc_Tcenv_curr_rgn(_tmpB8);
int is_local;
switch(_tmpD9){case Cyc_Absyn_Static: _LL57:
 goto _LL58;case Cyc_Absyn_Extern: _LL58:
 goto _LL59;case Cyc_Absyn_ExternC: _LL59:
# 437
 _tmpBC->escapes=1;
is_local=0;break;case Cyc_Absyn_Abstract: _LL5A:
# 440
{const char*_tmp1E3;void*_tmp1E2;(_tmp1E2=0,Cyc_Tcutil_terr(_tmp53->loc,((_tmp1E3="bad abstract scope for local variable",_tag_dyneither(_tmp1E3,sizeof(char),38))),_tag_dyneither(_tmp1E2,sizeof(void*),0)));}
goto _LL5B;case Cyc_Absyn_Register: _LL5B:
 goto _LL5C;case Cyc_Absyn_Public: _LL5C:
 is_local=1;break;}
# 446
*_tmpDF=is_local?_tmpE1:(void*)& Cyc_Absyn_HeapRgn_val;
{union Cyc_Absyn_Nmspace _tmpE4=_tmpDA;_LL5F: if((_tmpE4.Loc_n).tag != 4)goto _LL61;_LL60:
 goto _LL5E;_LL61: if((_tmpE4.Rel_n).tag != 1)goto _LL63;if((struct Cyc_List_List*)(_tmpE4.Rel_n).val != 0)goto _LL63;_LL62:
# 450
(*_tmpBC->name).f1=Cyc_Absyn_Loc_n;
goto _LL5E;_LL63: if((_tmpE4.C_n).tag != 3)goto _LL65;_LL64:
# 454
 goto _LL66;_LL65: if((_tmpE4.Abs_n).tag != 2)goto _LL67;_LL66:
 goto _LL68;_LL67:;_LL68:
# 457
{const char*_tmp1E6;void*_tmp1E5;(_tmp1E5=0,Cyc_Tcutil_terr(_tmp53->loc,((_tmp1E6="cannot declare a qualified local variable",_tag_dyneither(_tmp1E6,sizeof(char),42))),_tag_dyneither(_tmp1E5,sizeof(void*),0)));}
(*_tmpBC->name).f1=Cyc_Absyn_Loc_n;
goto _LL5E;_LL5E:;}
# 462
{void*_tmpE7=Cyc_Tcutil_compress(_tmpDD);void*_tmpE8=_tmpE7;void*_tmpEB;struct Cyc_Absyn_Tqual _tmpEC;union Cyc_Absyn_Constraint*_tmpED;unsigned int _tmpEE;_LL6A:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE9=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE8;if(_tmpE9->tag != 9)goto _LL6C;}if(!is_local)goto _LL6C;_LL6B:
# 465
 _tmpBC->escapes=1;
_tmpD9=Cyc_Absyn_Extern;
is_local=0;
goto _LL69;_LL6C:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpEA=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE8;if(_tmpEA->tag != 8)goto _LL6E;else{_tmpEB=(_tmpEA->f1).elt_type;_tmpEC=(_tmpEA->f1).tq;if((_tmpEA->f1).num_elts != 0)goto _LL6E;_tmpED=(_tmpEA->f1).zero_term;_tmpEE=(_tmpEA->f1).zt_loc;}}if(!(_tmpBC->initializer != 0))goto _LL6E;_LL6D:
# 470
{void*_tmpEF=((struct Cyc_Absyn_Exp*)_check_null(_tmpBC->initializer))->r;void*_tmpF0=_tmpEF;struct _dyneither_ptr _tmpF2;struct _dyneither_ptr _tmpF4;struct Cyc_Absyn_Exp*_tmpF6;struct Cyc_Absyn_Exp*_tmpF8;struct Cyc_List_List*_tmpFA;struct Cyc_List_List*_tmpFC;_LL71: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpF1=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF0;if(_tmpF1->tag != 0)goto _LL73;else{if(((_tmpF1->f1).String_c).tag != 8)goto _LL73;_tmpF2=(struct _dyneither_ptr)((_tmpF1->f1).String_c).val;}}_LL72:
# 472
 _tmpDD=(_tmpBC->type=Cyc_Absyn_array_typ(_tmpEB,_tmpEC,Cyc_Absyn_uint_exp(_get_dyneither_size(_tmpF2,sizeof(char)),0),_tmpED,_tmpEE));
goto _LL70;_LL73: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpF3=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF0;if(_tmpF3->tag != 0)goto _LL75;else{if(((_tmpF3->f1).Wstring_c).tag != 9)goto _LL75;_tmpF4=(struct _dyneither_ptr)((_tmpF3->f1).Wstring_c).val;}}_LL74:
# 475
 _tmpDD=(_tmpBC->type=Cyc_Absyn_array_typ(_tmpEB,_tmpEC,Cyc_Absyn_uint_exp(1,0),_tmpED,_tmpEE));
goto _LL70;_LL75: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpF5=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpF0;if(_tmpF5->tag != 26)goto _LL77;else{_tmpF6=_tmpF5->f2;}}_LL76:
 _tmpF8=_tmpF6;goto _LL78;_LL77: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpF7=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpF0;if(_tmpF7->tag != 27)goto _LL79;else{_tmpF8=_tmpF7->f1;}}_LL78:
# 480
 _tmpDD=(_tmpBC->type=Cyc_Absyn_array_typ(_tmpEB,_tmpEC,_tmpF8,_tmpED,_tmpEE));
goto _LL70;_LL79: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpF9=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpF0;if(_tmpF9->tag != 35)goto _LL7B;else{_tmpFA=_tmpF9->f2;}}_LL7A:
 _tmpFC=_tmpFA;goto _LL7C;_LL7B: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpFB=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpF0;if(_tmpFB->tag != 25)goto _LL7D;else{_tmpFC=_tmpFB->f1;}}_LL7C:
# 484
 _tmpDD=(_tmpBC->type=Cyc_Absyn_array_typ(_tmpEB,_tmpEC,Cyc_Absyn_uint_exp((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpFC),0),_tmpED,_tmpEE));
# 486
goto _LL70;_LL7D:;_LL7E:
 goto _LL70;_LL70:;}
# 489
goto _LL69;_LL6E:;_LL6F:
 goto _LL69;_LL69:;}{
# 496
struct Cyc_List_List*_tmpFD=!is_local?0: Cyc_Tcenv_lookup_type_vars(_tmpB8);
int _tmpFE=!is_local?0: 1;
Cyc_Tcutil_check_type(s0->loc,_tmpB8,_tmpFD,& Cyc_Tcutil_tmk,_tmpFE,1,_tmpDD);
(_tmpBC->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(s0->loc,(_tmpBC->tq).print_const,_tmpDD);{
# 501
struct Cyc_Tcenv_Tenv*_tmpFF=Cyc_Tcenv_add_local_var(decl_rgn,_tmp54->loc,_tmpB8,_tmpBC);
if(_tmpD9 == Cyc_Absyn_Extern  || _tmpD9 == Cyc_Absyn_ExternC){
const char*_tmp1E9;void*_tmp1E8;(_tmp1E8=0,Cyc_Tcutil_terr(_tmp53->loc,((_tmp1E9="extern declarations are not yet supported within functions",_tag_dyneither(_tmp1E9,sizeof(char),59))),_tag_dyneither(_tmp1E8,sizeof(void*),0)));}
if(_tmpDE != 0){
Cyc_Tcexp_tcExpInitializer(_tmpFF,& _tmpDD,_tmpDE);
# 507
if(!is_local  && !Cyc_Tcutil_is_const_exp(_tmpDE)){
const char*_tmp1EC;void*_tmp1EB;(_tmp1EB=0,Cyc_Tcutil_terr(_tmp53->loc,((_tmp1EC="initializer for static variable needs to be a constant expression",_tag_dyneither(_tmp1EC,sizeof(char),66))),_tag_dyneither(_tmp1EB,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_assign(_tmpFF,_tmpDE,_tmpDD)){
struct _dyneither_ptr _tmp104=*_tmpDB;
struct _dyneither_ptr _tmp105=Cyc_Absynpp_typ2string(_tmpDD);
struct _dyneither_ptr _tmp106=Cyc_Absynpp_typ2string((void*)_check_null(_tmpDE->topt));
if(((_get_dyneither_size(_tmp104,sizeof(char))+ _get_dyneither_size(_tmp105,sizeof(char)))+ _get_dyneither_size(_tmp106,sizeof(char)))+ 50 < 80){
const char*_tmp1F2;void*_tmp1F1[3];struct Cyc_String_pa_PrintArg_struct _tmp1F0;struct Cyc_String_pa_PrintArg_struct _tmp1EF;struct Cyc_String_pa_PrintArg_struct _tmp1EE;(_tmp1EE.tag=0,((_tmp1EE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp106),((_tmp1EF.tag=0,((_tmp1EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp105),((_tmp1F0.tag=0,((_tmp1F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp104),((_tmp1F1[0]=& _tmp1F0,((_tmp1F1[1]=& _tmp1EF,((_tmp1F1[2]=& _tmp1EE,Cyc_Tcutil_terr(_tmp53->loc,((_tmp1F2="%s was declared with type %s but initialized with type %s.",_tag_dyneither(_tmp1F2,sizeof(char),59))),_tag_dyneither(_tmp1F1,sizeof(void*),3)))))))))))))))))));}else{
# 516
if((_get_dyneither_size(_tmp104,sizeof(char))+ _get_dyneither_size(_tmp105,sizeof(char)))+ 25 < 80  && 
_get_dyneither_size(_tmp106,sizeof(char))+ 25 < 80){
const char*_tmp1F8;void*_tmp1F7[3];struct Cyc_String_pa_PrintArg_struct _tmp1F6;struct Cyc_String_pa_PrintArg_struct _tmp1F5;struct Cyc_String_pa_PrintArg_struct _tmp1F4;(_tmp1F4.tag=0,((_tmp1F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp106),((_tmp1F5.tag=0,((_tmp1F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp105),((_tmp1F6.tag=0,((_tmp1F6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp104),((_tmp1F7[0]=& _tmp1F6,((_tmp1F7[1]=& _tmp1F5,((_tmp1F7[2]=& _tmp1F4,Cyc_Tcutil_terr(_tmp53->loc,((_tmp1F8="%s was declared with type %s\n but initialized with type %s.",_tag_dyneither(_tmp1F8,sizeof(char),60))),_tag_dyneither(_tmp1F7,sizeof(void*),3)))))))))))))))))));}else{
# 521
const char*_tmp1FE;void*_tmp1FD[3];struct Cyc_String_pa_PrintArg_struct _tmp1FC;struct Cyc_String_pa_PrintArg_struct _tmp1FB;struct Cyc_String_pa_PrintArg_struct _tmp1FA;(_tmp1FA.tag=0,((_tmp1FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp106),((_tmp1FB.tag=0,((_tmp1FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp105),((_tmp1FC.tag=0,((_tmp1FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp104),((_tmp1FD[0]=& _tmp1FC,((_tmp1FD[1]=& _tmp1FB,((_tmp1FD[2]=& _tmp1FA,Cyc_Tcutil_terr(_tmp53->loc,((_tmp1FE="%s declared with type \n%s\n but initialized with type \n%s.",_tag_dyneither(_tmp1FE,sizeof(char),58))),_tag_dyneither(_tmp1FD,sizeof(void*),3)))))))))))))))))));}}
# 523
Cyc_Tcutil_unify(_tmpDD,(void*)_check_null(_tmpDE->topt));
Cyc_Tcutil_explain_failure();}}
# 527
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(decl_rgn,_tmpFF,s0),_tmp54,0);
_npop_handler(0);return;};};};}_LL3B: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmpBD=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpBA;if(_tmpBD->tag != 2)goto _LL3D;else{_tmpBE=_tmpBD->f1;_tmpBF=(struct Cyc_Core_Opt**)& _tmpBD->f2;_tmpC0=(struct Cyc_Absyn_Exp**)& _tmpBD->f3;}}_LL3C: {
# 535
struct Cyc_Absyn_Exp*_tmp116=*_tmpC0;
{void*_tmp117=_tmpBE->r;void*_tmp118=_tmp117;_LL80: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp119=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp118;if(_tmp119->tag != 1)goto _LL82;}_LL81:
 goto _LL83;_LL82: {struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp11A=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp118;if(_tmp11A->tag != 2)goto _LL84;}_LL83:
 goto _LL85;_LL84: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp11B=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp118;if(_tmp11B->tag != 15)goto _LL86;}_LL85:
 goto _LL7F;_LL86:;_LL87:
# 541
 if(!Cyc_Tcutil_is_var_exp(_tmp116)){
struct _tuple2*_tmp11C=Cyc_Tcstmt_stmt_temp_var();
struct Cyc_Absyn_Stmt*_tmp11D=Cyc_Absyn_new_stmt(s0->r,s0->loc);
struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp201;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp200;struct Cyc_Absyn_Decl*_tmp11E=Cyc_Absyn_let_decl(Cyc_Absyn_new_pat((void*)((_tmp200=_cycalloc(sizeof(*_tmp200)),((_tmp200[0]=((_tmp201.tag=1,((_tmp201.f1=Cyc_Absyn_new_vardecl(_tmp11C,
Cyc_Absyn_new_evar(0,0),0),((_tmp201.f2=
# 547
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,_tmp116->loc),_tmp201)))))),_tmp200)))),_tmp116->loc),_tmp116,s0->loc);
# 550
struct Cyc_Absyn_Stmt*_tmp11F=Cyc_Absyn_decl_stmt(_tmp11E,_tmp11D,s0->loc);
# 552
s0->r=_tmp11F->r;
*_tmpC0=Cyc_Absyn_var_exp(_tmp11C,_tmp116->loc);
Cyc_Tcstmt_tcStmt(_tmpB8,s0,new_block);
_npop_handler(0);return;}_LL7F:;}
# 558
Cyc_Tcexp_tcExpInitializer(_tmpB8,0,_tmp116);{
# 560
void*pat_type=(void*)_check_null(_tmp116->topt);
# 562
struct Cyc_Tcpat_TcPatResult _tmp122=Cyc_Tcpat_tcPat(_tmpB8,_tmpBE,& pat_type,_tmp116);
{struct Cyc_Core_Opt*_tmp202;*_tmpBF=((_tmp202=_cycalloc(sizeof(*_tmp202)),((_tmp202->v=_tmp122.patvars,_tmp202))));}
if(!Cyc_Tcutil_unify((void*)_check_null(_tmpBE->topt),(void*)_check_null(_tmp116->topt)) && !
Cyc_Tcutil_coerce_assign(_tmpB8,_tmp116,(void*)_check_null(_tmpBE->topt))){
{const char*_tmp207;void*_tmp206[2];struct Cyc_String_pa_PrintArg_struct _tmp205;struct Cyc_String_pa_PrintArg_struct _tmp204;(_tmp204.tag=0,((_tmp204.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmp116->topt))),((_tmp205.tag=0,((_tmp205.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmpBE->topt))),((_tmp206[0]=& _tmp205,((_tmp206[1]=& _tmp204,Cyc_Tcutil_terr(_tmp53->loc,((_tmp207="pattern type %s does not match definition type %s",_tag_dyneither(_tmp207,sizeof(char),50))),_tag_dyneither(_tmp206,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unify((void*)_check_null(_tmpBE->topt),(void*)_check_null(_tmp116->topt));
Cyc_Tcutil_explain_failure();}else{
# 572
Cyc_Tcpat_check_pat_regions(_tmpB8,_tmpBE,_tmp122.patvars);}
Cyc_Tcpat_check_let_pat_exhaustive(_tmpBE->loc,_tmpB8,_tmpBE);
Cyc_Tcstmt_pattern_synth(s0->loc,_tmpB8,_tmp122,_tmp54,0,0);
_npop_handler(0);return;};}_LL3D: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmpC1=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmpBA;if(_tmpC1->tag != 3)goto _LL3F;else{_tmpC2=_tmpC1->f1;}}_LL3E: {
# 578
void*_tmp128=Cyc_Tcenv_curr_rgn(_tmpB8);
struct Cyc_Tcenv_Tenv*_tmp129=_tmpB8;
for(0;_tmpC2 != 0;_tmpC2=_tmpC2->tl){
struct Cyc_Absyn_Vardecl*_tmp12A=(struct Cyc_Absyn_Vardecl*)_tmpC2->hd;
union Cyc_Absyn_Nmspace _tmp12C;void*_tmp12D;void**_tmp12E;struct Cyc_Absyn_Vardecl*_tmp12B=_tmp12A;_tmp12C=(_tmp12B->name)->f1;_tmp12D=_tmp12B->type;_tmp12E=(void**)& _tmp12B->rgn;
*_tmp12E=_tmp128;
{union Cyc_Absyn_Nmspace _tmp12F=_tmp12C;struct Cyc_List_List*_tmp130;_LL89: if((_tmp12F.Loc_n).tag != 4)goto _LL8B;_LL8A:
 goto _LL88;_LL8B: if((_tmp12F.Rel_n).tag != 1)goto _LL8D;if((struct Cyc_List_List*)(_tmp12F.Rel_n).val != 0)goto _LL8D;_LL8C:
# 587
(*_tmp12A->name).f1=Cyc_Absyn_Loc_n;
goto _LL88;_LL8D: if((_tmp12F.Abs_n).tag != 2)goto _LL8F;_tmp130=(struct Cyc_List_List*)(_tmp12F.Abs_n).val;_LL8E: {
const char*_tmp20A;void*_tmp209;(int)_throw(((_tmp209=0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp20A="tcstmt: Abs_n declaration",_tag_dyneither(_tmp20A,sizeof(char),26))),_tag_dyneither(_tmp209,sizeof(void*),0)))));}_LL8F:;_LL90:
# 593
{const char*_tmp20D;void*_tmp20C;(_tmp20C=0,Cyc_Tcutil_terr(_tmp53->loc,((_tmp20D="cannot declare a qualified local variable",_tag_dyneither(_tmp20D,sizeof(char),42))),_tag_dyneither(_tmp20C,sizeof(void*),0)));}
goto _LL88;_LL88:;}
# 596
Cyc_Tcutil_check_type(s0->loc,_tmp129,Cyc_Tcenv_lookup_type_vars(_tmp129),& Cyc_Tcutil_tmk,1,1,_tmp12D);
_tmp129=Cyc_Tcenv_add_local_var(decl_rgn,_tmp54->loc,_tmp129,_tmp12A);}
# 599
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(decl_rgn,_tmp129,s0),_tmp54,0);
_npop_handler(0);return;}_LL3F: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmpC3=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpBA;if(_tmpC3->tag != 4)goto _LL41;else{_tmpC4=_tmpC3->f1;_tmpC5=_tmpC3->f2;_tmpC6=_tmpC3->f3;_tmpC7=_tmpC3->f4;}}_LL40:
# 605
{struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp210;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp20F;_tmp54->r=(Cyc_Absyn_new_stmt((void*)((_tmp20F=_cycalloc(sizeof(*_tmp20F)),((_tmp20F[0]=((_tmp210.tag=2,((_tmp210.f1=Cyc_Absyn_new_stmt(_tmp54->r,_tmp54->loc),((_tmp210.f2=Cyc_Absyn_skip_stmt(_tmp54->loc),_tmp210)))))),_tmp20F)))),_tmp54->loc))->r;}{
# 607
struct Cyc_Tcenv_Tenv*_tmp137=_tmpB8;
void**_tmp139;void**_tmp13A;struct Cyc_Absyn_Vardecl*_tmp138=_tmpC5;_tmp139=(void**)& _tmp138->type;_tmp13A=(void**)& _tmp138->rgn;{
void*_tmp13B=Cyc_Tcenv_curr_rgn(_tmpB8);
*_tmp13A=_tmp13B;{
void*rgn_typ;
if((unsigned int)_tmpC7){
# 615
union Cyc_Absyn_Nmspace _tmp216;struct _dyneither_ptr*_tmp215[1];struct _tuple2*_tmp214;struct _tuple2*drname=(_tmp214=_cycalloc(sizeof(*_tmp214)),((_tmp214->f1=(((_tmp216.Abs_n).val=((_tmp215[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"Core",sizeof(char),5),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp215,sizeof(struct _dyneither_ptr*),1)))),(((_tmp216.Abs_n).tag=2,_tmp216)))),((_tmp214->f2=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"DynamicRegion",sizeof(char),14),_tmp214)))));
struct Cyc_Core_Opt*_tmp217;void*_tmp13C=Cyc_Absyn_new_evar(((_tmp217=_cycalloc(sizeof(*_tmp217)),((_tmp217->v=& Cyc_Tcutil_trk,_tmp217)))),0);
{struct Cyc_Core_Opt*_tmp218;rgn_typ=Cyc_Absyn_new_evar(((_tmp218=_cycalloc(sizeof(*_tmp218)),((_tmp218->v=& Cyc_Tcutil_rk,_tmp218)))),0);}{
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp222;void*_tmp221[1];struct Cyc_Absyn_AggrInfo _tmp220;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp21F;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp13E=
(_tmp21F=_cycalloc(sizeof(*_tmp21F)),((_tmp21F[0]=((_tmp222.tag=11,((_tmp222.f1=((_tmp220.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,drname,0),((_tmp220.targs=(
(_tmp221[0]=rgn_typ,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp221,sizeof(void*),1)))),_tmp220)))),_tmp222)))),_tmp21F)));
void*_tmp13F=Cyc_Absyn_at_typ((void*)_tmp13E,_tmp13C,
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);
Cyc_Tcutil_check_type(s0->loc,_tmpB8,Cyc_Tcenv_lookup_type_vars(_tmpB8),& Cyc_Tcutil_tmk,1,0,_tmp13F);{
void*_tmp140=Cyc_Tcexp_tcExp(_tmpB8,& _tmp13F,_tmpC7);
if(!Cyc_Tcutil_unify(_tmp13F,_tmp140) && !
Cyc_Tcutil_coerce_assign(_tmpB8,_tmpC7,_tmp13F)){
const char*_tmp227;void*_tmp226[2];struct Cyc_String_pa_PrintArg_struct _tmp225;struct Cyc_String_pa_PrintArg_struct _tmp224;(_tmp224.tag=0,((_tmp224.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp140)),((_tmp225.tag=0,((_tmp225.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp13F)),((_tmp226[0]=& _tmp225,((_tmp226[1]=& _tmp224,Cyc_Tcutil_terr(s0->loc,((_tmp227="expected %s but found %s",_tag_dyneither(_tmp227,sizeof(char),25))),_tag_dyneither(_tmp226,sizeof(void*),2)))))))))))));}
if(!Cyc_Tcutil_unify(_tmp13C,(void*)& Cyc_Absyn_UniqueRgn_val) && !
Cyc_Tcutil_unify(_tmp13C,(void*)& Cyc_Absyn_RefCntRgn_val)){
const char*_tmp22A;void*_tmp229;(_tmp229=0,Cyc_Tcutil_terr(s0->loc,((_tmp22A="open is only allowed on unique or reference-counted keys",_tag_dyneither(_tmp22A,sizeof(char),57))),_tag_dyneither(_tmp229,sizeof(void*),0)));}{
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp22D;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp22C;*_tmp139=(void*)((_tmp22C=_cycalloc(sizeof(*_tmp22C)),((_tmp22C[0]=((_tmp22D.tag=15,((_tmp22D.f1=rgn_typ,_tmp22D)))),_tmp22C))));};};};}else{
# 635
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp230;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp22F;rgn_typ=(void*)((_tmp22F=_cycalloc(sizeof(*_tmp22F)),((_tmp22F[0]=((_tmp230.tag=2,((_tmp230.f1=_tmpC4,_tmp230)))),_tmp22F))));}{
struct Cyc_List_List*_tmp231;_tmp137=Cyc_Tcenv_add_type_vars(decl_rgn,s0->loc,_tmp137,((_tmp231=_cycalloc(sizeof(*_tmp231)),((_tmp231->hd=_tmpC4,((_tmp231->tl=0,_tmp231)))))));};}
# 638
_tmp137=Cyc_Tcenv_add_region(decl_rgn,_tmp137,rgn_typ,_tmpC6,1);
Cyc_Tcutil_check_type(s0->loc,_tmpB8,Cyc_Tcenv_lookup_type_vars(_tmp137),& Cyc_Tcutil_bk,1,0,*_tmp139);
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp234;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp233;if(!Cyc_Tcutil_unify(*_tmp139,(void*)((_tmp233=_cycalloc(sizeof(*_tmp233)),((_tmp233[0]=((_tmp234.tag=15,((_tmp234.f1=rgn_typ,_tmp234)))),_tmp233)))))){
const char*_tmp237;void*_tmp236;(_tmp236=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp237="region stmt: type of region handle is wrong!",_tag_dyneither(_tmp237,sizeof(char),45))),_tag_dyneither(_tmp236,sizeof(void*),0)));}}
# 644
if(!Cyc_Absyn_no_regions)
_tmp137=Cyc_Tcenv_enter_try(decl_rgn,_tmp137);
# 647
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(decl_rgn,Cyc_Tcenv_add_local_var(decl_rgn,_tmp54->loc,_tmp137,_tmpC5),s0),_tmp54,0);
_npop_handler(0);return;};};};_LL41: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpC8=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpBA;if(_tmpC8->tag != 1)goto _LL43;else{_tmpC9=_tmpC8->f1;}}_LL42: {
# 652
void*_tmp15A=Cyc_Tcenv_curr_rgn(_tmpB8);
if(_tmpC9->sc != Cyc_Absyn_Public){
const char*_tmp23A;void*_tmp239;(_tmp239=0,Cyc_Tcutil_terr(_tmp53->loc,((_tmp23A="bad storage class for inner function",_tag_dyneither(_tmp23A,sizeof(char),37))),_tag_dyneither(_tmp239,sizeof(void*),0)));}
# 656
{union Cyc_Absyn_Nmspace _tmp15D=(*_tmpC9->name).f1;union Cyc_Absyn_Nmspace _tmp15E=_tmp15D;struct Cyc_List_List*_tmp15F;_LL92: if((_tmp15E.Rel_n).tag != 1)goto _LL94;if((struct Cyc_List_List*)(_tmp15E.Rel_n).val != 0)goto _LL94;_LL93:
 goto _LL91;_LL94: if((_tmp15E.Abs_n).tag != 2)goto _LL96;_tmp15F=(struct Cyc_List_List*)(_tmp15E.Abs_n).val;_LL95: {
const char*_tmp23D;void*_tmp23C;(_tmp23C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp23D="tc: Abs_n in tcStmt var decl",_tag_dyneither(_tmp23D,sizeof(char),29))),_tag_dyneither(_tmp23C,sizeof(void*),0)));}_LL96:;_LL97: {
const char*_tmp240;void*_tmp23F;(_tmp23F=0,Cyc_Tcutil_terr(_tmp53->loc,((_tmp240="explicit namespace not allowed on inner function declaration",_tag_dyneither(_tmp240,sizeof(char),61))),_tag_dyneither(_tmp23F,sizeof(void*),0)));}_LL91:;}
# 661
(*_tmpC9->name).f1=Cyc_Absyn_Loc_n;
Cyc_Tcutil_check_fndecl_valid_type(_tmp53->loc,_tmpB8,_tmpC9);{
void*t=Cyc_Tcutil_fndecl2typ(_tmpC9);
_tmpC9->attributes=Cyc_Tcutil_transfer_fn_type_atts(t,_tmpC9->attributes);
# 666
{struct Cyc_List_List*atts=_tmpC9->attributes;for(0;(unsigned int)atts;atts=atts->tl){
void*_tmp164=(void*)atts->hd;void*_tmp165=_tmp164;_LL99: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp166=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp165;if(_tmp166->tag != 7)goto _LL9B;}_LL9A:
 goto _LL9C;_LL9B: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp167=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp165;if(_tmp167->tag != 6)goto _LL9D;}_LL9C:
{const char*_tmp244;void*_tmp243[1];struct Cyc_String_pa_PrintArg_struct _tmp242;(_tmp242.tag=0,((_tmp242.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp243[0]=& _tmp242,Cyc_Tcutil_terr(_tmp53->loc,((_tmp244="bad attribute %s for function",_tag_dyneither(_tmp244,sizeof(char),30))),_tag_dyneither(_tmp243,sizeof(void*),1)))))));}
goto _LL98;_LL9D:;_LL9E:
 goto _LL98;_LL98:;}}{
# 674
struct Cyc_Absyn_Vardecl*_tmp245;struct Cyc_Absyn_Vardecl*vd=(_tmp245=_cycalloc(sizeof(*_tmp245)),((_tmp245->sc=_tmpC9->sc,((_tmp245->name=_tmpC9->name,((_tmp245->tq=Cyc_Absyn_const_tqual(0),((_tmp245->type=
Cyc_Absyn_at_typ(t,_tmp15A,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),((_tmp245->initializer=0,((_tmp245->rgn=_tmp15A,((_tmp245->attributes=0,((_tmp245->escapes=0,_tmp245)))))))))))))))));
# 678
_tmpC9->fn_vardecl=vd;
_tmpB8=Cyc_Tcenv_add_local_var(decl_rgn,_tmp53->loc,_tmpB8,vd);{
struct Cyc_Tcenv_Fenv*old_fenv=(struct Cyc_Tcenv_Fenv*)_check_null(_tmpB8->le);
_tmpB8->le=Cyc_Tcenv_nested_fenv(_tmp53->loc,old_fenv,_tmpC9);
Cyc_Tcstmt_tcStmt(_tmpB8,_tmpC9->body,0);
Cyc_Tcenv_check_delayed_effects(_tmpB8);
Cyc_Tcenv_check_delayed_constraints(_tmpB8);
if(!Cyc_Tcenv_all_labels_resolved(_tmpB8)){
const char*_tmp248;void*_tmp247;(_tmp247=0,Cyc_Tcutil_terr(_tmp53->loc,((_tmp248="function has goto statements to undefined labels",_tag_dyneither(_tmp248,sizeof(char),49))),_tag_dyneither(_tmp247,sizeof(void*),0)));}
_tmpB8->le=old_fenv;
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(decl_rgn,_tmpB8,s0),_tmp54,0);
_npop_handler(0);return;};};};}_LL43: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpCA=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpBA;if(_tmpCA->tag != 9)goto _LL45;else{_tmpCB=_tmpCA->f1;_tmpCC=_tmpCA->f2;}}_LL44:
{const char*_tmp249;unimp_msg_part=((_tmp249="namespace",_tag_dyneither(_tmp249,sizeof(char),10)));}goto _LL38;_LL45: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmpCD=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpBA;if(_tmpCD->tag != 10)goto _LL47;else{_tmpCE=_tmpCD->f1;_tmpCF=_tmpCD->f2;}}_LL46:
{const char*_tmp24A;unimp_msg_part=((_tmp24A="using",_tag_dyneither(_tmp24A,sizeof(char),6)));}goto _LL38;_LL47: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD0=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpBA;if(_tmpD0->tag != 5)goto _LL49;}_LL48:
{const char*_tmp24B;unimp_msg_part=((_tmp24B="type",_tag_dyneither(_tmp24B,sizeof(char),5)));}goto _LL38;_LL49: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmpD1=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmpBA;if(_tmpD1->tag != 6)goto _LL4B;}_LL4A:
{const char*_tmp24C;unimp_msg_part=((_tmp24C="datatype",_tag_dyneither(_tmp24C,sizeof(char),9)));}goto _LL38;_LL4B: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmpD2=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpBA;if(_tmpD2->tag != 7)goto _LL4D;}_LL4C:
{const char*_tmp24D;unimp_msg_part=((_tmp24D="enum",_tag_dyneither(_tmp24D,sizeof(char),5)));}goto _LL38;_LL4D: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmpD3=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpBA;if(_tmpD3->tag != 8)goto _LL4F;}_LL4E:
{const char*_tmp24E;unimp_msg_part=((_tmp24E="typedef",_tag_dyneither(_tmp24E,sizeof(char),8)));}goto _LL38;_LL4F: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmpD4=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmpBA;if(_tmpD4->tag != 11)goto _LL51;}_LL50:
{const char*_tmp24F;unimp_msg_part=((_tmp24F="extern \"C\"",_tag_dyneither(_tmp24F,sizeof(char),11)));}goto _LL38;_LL51: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpD5=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmpBA;if(_tmpD5->tag != 12)goto _LL53;}_LL52:
# 698
{const char*_tmp250;unimp_msg_part=((_tmp250="extern \"C include\"",_tag_dyneither(_tmp250,sizeof(char),19)));}goto _LL38;_LL53: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmpD6=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmpBA;if(_tmpD6->tag != 13)goto _LL55;}_LL54:
{const char*_tmp251;unimp_msg_part=((_tmp251="__cyclone_port_on__",_tag_dyneither(_tmp251,sizeof(char),20)));}goto _LL38;_LL55: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmpD7=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmpBA;if(_tmpD7->tag != 14)goto _LL38;}_LL56:
{const char*_tmp252;unimp_msg_part=((_tmp252="__cyclone_port_off__",_tag_dyneither(_tmp252,sizeof(char),21)));}goto _LL38;_LL38:;};}{
# 703
const char*_tmp256;void*_tmp255[1];struct Cyc_String_pa_PrintArg_struct _tmp254;(int)_throw(((_tmp254.tag=0,((_tmp254.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)unimp_msg_part),((_tmp255[0]=& _tmp254,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp256="tcStmt: nested %s declarations unimplemented",_tag_dyneither(_tmp256,sizeof(char),45))),_tag_dyneither(_tmp255,sizeof(void*),1)))))))));};
# 421
;_pop_region(decl_rgn);}_LL28: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp55=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp23;if(_tmp55->tag != 16)goto _LL7;else{_tmp56=_tmp55->f1;}}_LL29: {
# 707
void*rgn_type=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp259;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp258;void*etype=(void*)((_tmp258=_cycalloc(sizeof(*_tmp258)),((_tmp258[0]=((_tmp259.tag=15,((_tmp259.f1=rgn_type,_tmp259)))),_tmp258))));
if(!Cyc_Tcutil_unify(Cyc_Tcexp_tcExp(te,& etype,_tmp56),etype)){
const char*_tmp25D;void*_tmp25C[1];struct Cyc_String_pa_PrintArg_struct _tmp25B;(_tmp25B.tag=0,((_tmp25B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp56->topt))),((_tmp25C[0]=& _tmp25B,Cyc_Tcutil_terr(_tmp56->loc,((_tmp25D="expecting region_t but found %s",_tag_dyneither(_tmp25D,sizeof(char),32))),_tag_dyneither(_tmp25C,sizeof(void*),1)))))));}
# 712
Cyc_Tcenv_check_rgn_resetable(te,s0->loc,rgn_type);
return;}_LL7:;};}
