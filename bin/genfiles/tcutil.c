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
# 88
int Cyc_fflush(struct Cyc___cycFILE*);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;
# 157 "cycboot.h"
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);
# 232 "cycboot.h"
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;};
# 110 "core.h"
void*Cyc_Core_fst(struct _tuple0*);
# 121
int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
# 70
struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 94
struct Cyc_List_List*Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);
# 133
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 161
struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 205
struct Cyc_List_List*Cyc_List_rflatten(struct _RegionHandle*,struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 261
int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 270
struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 303
struct _tuple1 Cyc_List_rsplit(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);
# 322
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 336
void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);
# 383
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;
# 41
struct _dyneither_ptr Cyc_Position_string_of_segment(unsigned int);
struct Cyc_List_List*Cyc_Position_strings_of_segments(struct Cyc_List_List*);struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;
# 56
struct Cyc_Position_Error*Cyc_Position_mk_err_elab(unsigned int,struct _dyneither_ptr);extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
# 62
extern int Cyc_Position_num_errors;
extern int Cyc_Position_max_errors;
void Cyc_Position_post_error(struct Cyc_Position_Error*);struct Cyc_Relations_Reln;
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 252
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{int tag;};
# 287
typedef void*Cyc_Absyn_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 334
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 353
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 365
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 386
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 391
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 446 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 462
typedef void*Cyc_Absyn_funcparams_t;
# 465
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 529
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 544
extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;
# 555
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 562
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 580
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 936 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 938
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 946
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 949
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x);
# 953
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 956
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 958
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
# 964
void*Cyc_Absyn_compress_kb(void*);
# 969
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 974
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 976
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;
# 980
extern void*Cyc_Absyn_empty_effect;
# 990
extern struct _tuple2*Cyc_Absyn_datatype_print_arg_qvar;
extern struct _tuple2*Cyc_Absyn_datatype_scanf_arg_qvar;
# 999
extern void*Cyc_Absyn_bounds_one;
# 1004
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 1030
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1040
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1047
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned int);
# 1049
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
# 1052
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1062
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1064
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned int);
# 1069
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1075
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_rethrow_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1087
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
# 1093
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1133
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);
# 1180
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1182
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1184
struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1186
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
# 1188
int Cyc_Absyn_fntype_att(void*a);struct _tuple13{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;};
# 1192
struct _tuple13 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1194
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 56 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 58
extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 65
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 67
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*);
# 70
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 72
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct Cyc_RgnOrder_RgnPO;
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
# 109 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(struct _RegionHandle*,void*ret_type,struct Cyc_List_List*args);
# 114
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 118
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 121
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 124
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 138
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
# 141
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
# 179
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 191
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*r1,void*r2);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);struct _tuple14{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple14 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 45
int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 44
void Cyc_Tcutil_flush_warnings();extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 53
extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;
# 58
void*Cyc_Tcutil_copy_type(void*t);
# 61
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 64
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 68
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
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
int Cyc_Tcutil_is_arithmetic_type(void*);
# 83
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 85
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 87
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);struct _tuple15{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 91
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 93
extern int Cyc_Tcutil_warn_alias_coerce;
# 96
extern int Cyc_Tcutil_warn_region_coerce;
# 99
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_function_type(void*t);
int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_boxed(void*t);
int Cyc_Tcutil_is_dyneither_ptr(void*t);
void*Cyc_Tcutil_pointer_elt_type(void*t);
void*Cyc_Tcutil_pointer_region(void*t);
# 110
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 117
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 122
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 127
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
extern struct Cyc_Core_Opt Cyc_Tcutil_eko;
# 134
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
extern struct Cyc_Core_Opt Cyc_Tcutil_tbko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 139
extern struct Cyc_Core_Opt Cyc_Tcutil_urko;
extern struct Cyc_Core_Opt Cyc_Tcutil_uako;
extern struct Cyc_Core_Opt Cyc_Tcutil_ubko;
extern struct Cyc_Core_Opt Cyc_Tcutil_umko;
# 144
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
int Cyc_Tcutil_unify_kindbound(void*,void*);struct _tuple16{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 148
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb);
# 153
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
# 155
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 159
void Cyc_Tcutil_explain_failure();
# 161
int Cyc_Tcutil_unify(void*,void*);
# 163
int Cyc_Tcutil_typecmp(void*,void*);
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 166
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 171
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*);
# 175
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_List_List*fs);
# 179
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 182
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 186
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 188
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 192
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);struct _tuple17{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*,struct Cyc_Absyn_Tvar*);
# 198
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);
# 225 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 227
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 235
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 238
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr err_msg);
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 245
void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);
# 247
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
# 249
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 251
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
# 253
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 259
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 261
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 263
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 267
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type);
# 273
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 278
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 281
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 286
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 291
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);struct _tuple18{int f1;void*f2;};
# 295
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 303
void*Cyc_Tcutil_normalize_effect(void*e);
# 306
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 308
int Cyc_Tcutil_new_tvar_id();
# 310
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 313
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 315
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
# 318
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 322
int Cyc_Tcutil_bits_only(void*t);
# 325
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 328
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e);
# 331
void*Cyc_Tcutil_snd_tqt(struct _tuple12*);
# 335
int Cyc_Tcutil_supports_default(void*);
# 339
int Cyc_Tcutil_admits_zero(void*t);
# 342
int Cyc_Tcutil_is_noreturn(void*);
# 346
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 350
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 354
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 357
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 360
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 364
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 367
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 370
int Cyc_Tcutil_is_array(void*t);
# 374
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 377
int Cyc_Tcutil_zeroable_type(void*t);
# 32 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};
# 39
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);
# 43 "tc.h"
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Aggrdecl*);
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Enumdecl*);
# 25 "cyclone.h"
extern int Cyc_Cyclone_tovc_r;
# 27
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 40 "relations.h"
typedef union Cyc_Relations_RelnOp Cyc_Relations_reln_op_t;
# 42
union Cyc_Relations_RelnOp Cyc_Relations_RParam(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RParamNumelts(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RReturn();
# 51
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0,Cyc_Relations_Rneq  = 1,Cyc_Relations_Rlte  = 2,Cyc_Relations_Rlt  = 3};
typedef enum Cyc_Relations_Relation Cyc_Relations_relation_t;struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 71
struct Cyc_Relations_Reln*Cyc_Relations_negate(struct _RegionHandle*,struct Cyc_Relations_Reln*);
# 85
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);
# 103
struct Cyc_List_List*Cyc_Relations_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);
# 122
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);char Cyc_Tcutil_Unify[6]="Unify";struct Cyc_Tcutil_Unify_exn_struct{char*tag;};
# 47 "tcutil.cyc"
struct Cyc_Tcutil_Unify_exn_struct Cyc_Tcutil_Unify_val={Cyc_Tcutil_Unify};
# 49
void Cyc_Tcutil_unify_it(void*t1,void*t2);
# 53
int Cyc_Tcutil_warn_region_coerce=0;
# 56
void*Cyc_Tcutil_t1_failure=(void*)& Cyc_Absyn_VoidType_val;
int Cyc_Tcutil_tq1_const=0;
void*Cyc_Tcutil_t2_failure=(void*)& Cyc_Absyn_VoidType_val;
int Cyc_Tcutil_tq2_const=0;
# 61
struct _dyneither_ptr Cyc_Tcutil_failure_reason={(void*)0,(void*)0,(void*)(0 + 0)};
# 65
void Cyc_Tcutil_explain_failure(){
if(Cyc_Position_num_errors >= Cyc_Position_max_errors)return;
Cyc_fflush(Cyc_stderr);
# 70
if(Cyc_strcmp(({const char*_tmp1="(qualifiers don't match)";_tag_dyneither(_tmp1,sizeof(char),25);}),(struct _dyneither_ptr)Cyc_Tcutil_failure_reason)== 0){
({struct Cyc_String_pa_PrintArg_struct _tmp4;_tmp4.tag=0;_tmp4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);({void*_tmp2[1]={& _tmp4};Cyc_fprintf(Cyc_stderr,({const char*_tmp3="  %s\n";_tag_dyneither(_tmp3,sizeof(char),6);}),_tag_dyneither(_tmp2,sizeof(void*),1));});});
return;}
# 75
if(Cyc_strcmp(({const char*_tmp5="(function effects do not match)";_tag_dyneither(_tmp5,sizeof(char),32);}),(struct _dyneither_ptr)Cyc_Tcutil_failure_reason)== 0){
struct Cyc_Absynpp_Params _tmp6=Cyc_Absynpp_tc_params_r;
_tmp6.print_all_effects=1;
Cyc_Absynpp_set_params(& _tmp6);}{
# 80
struct _dyneither_ptr s1=Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure);
struct _dyneither_ptr s2=Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure);
int pos=2;
({struct Cyc_String_pa_PrintArg_struct _tmp9;_tmp9.tag=0;_tmp9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);({void*_tmp7[1]={& _tmp9};Cyc_fprintf(Cyc_stderr,({const char*_tmp8="  %s";_tag_dyneither(_tmp8,sizeof(char),5);}),_tag_dyneither(_tmp7,sizeof(void*),1));});});
pos +=_get_dyneither_size(s1,sizeof(char));
if(pos + 5 >= 80){
({void*_tmpA=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpB="\n\t";_tag_dyneither(_tmpB,sizeof(char),3);}),_tag_dyneither(_tmpA,sizeof(void*),0));});
pos=8;}else{
# 89
({void*_tmpC=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpD=" ";_tag_dyneither(_tmpD,sizeof(char),2);}),_tag_dyneither(_tmpC,sizeof(void*),0));});
++ pos;}
# 92
({void*_tmpE=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpF="and ";_tag_dyneither(_tmpF,sizeof(char),5);}),_tag_dyneither(_tmpE,sizeof(void*),0));});
pos +=4;
if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){
({void*_tmp10=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp11="\n\t";_tag_dyneither(_tmp11,sizeof(char),3);}),_tag_dyneither(_tmp10,sizeof(void*),0));});
pos=8;}
# 98
({struct Cyc_String_pa_PrintArg_struct _tmp14;_tmp14.tag=0;_tmp14.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);({void*_tmp12[1]={& _tmp14};Cyc_fprintf(Cyc_stderr,({const char*_tmp13="%s ";_tag_dyneither(_tmp13,sizeof(char),4);}),_tag_dyneither(_tmp12,sizeof(void*),1));});});
pos +=_get_dyneither_size(s2,sizeof(char))+ 1;
if(pos + 17 >= 80){
({void*_tmp15=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp16="\n\t";_tag_dyneither(_tmp16,sizeof(char),3);}),_tag_dyneither(_tmp15,sizeof(void*),0));});
pos=8;}
# 104
({void*_tmp17=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp18="are not compatible. ";_tag_dyneither(_tmp18,sizeof(char),21);}),_tag_dyneither(_tmp17,sizeof(void*),0));});
pos +=17;
if((char*)Cyc_Tcutil_failure_reason.curr != (char*)(_tag_dyneither(0,0,0)).curr){
if(pos + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason)>= 80)
({void*_tmp19=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1A="\n\t";_tag_dyneither(_tmp1A,sizeof(char),3);}),_tag_dyneither(_tmp19,sizeof(void*),0));});
# 110
({struct Cyc_String_pa_PrintArg_struct _tmp1D;_tmp1D.tag=0;_tmp1D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);({void*_tmp1B[1]={& _tmp1D};Cyc_fprintf(Cyc_stderr,({const char*_tmp1C="%s";_tag_dyneither(_tmp1C,sizeof(char),3);}),_tag_dyneither(_tmp1B,sizeof(void*),1));});});}
# 112
({void*_tmp1E=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1F="\n";_tag_dyneither(_tmp1F,sizeof(char),2);}),_tag_dyneither(_tmp1E,sizeof(void*),0));});
Cyc_fflush(Cyc_stderr);};}
# 116
void Cyc_Tcutil_terr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 119
Cyc_Position_post_error(Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}
# 122
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 125
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
({struct Cyc_String_pa_PrintArg_struct _tmp22;_tmp22.tag=0;_tmp22.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg);({void*_tmp20[1]={& _tmp22};Cyc_fprintf(Cyc_stderr,({const char*_tmp21="Compiler Error (Tcutil::impos): %s\n";_tag_dyneither(_tmp21,sizeof(char),36);}),_tag_dyneither(_tmp20,sizeof(void*),1));});});
Cyc_fflush(Cyc_stderr);
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp23=_cycalloc(sizeof(*_tmp23));_tmp23[0]=({struct Cyc_Core_Impossible_exn_struct _tmp24;_tmp24.tag=Cyc_Core_Impossible;_tmp24.f1=msg;_tmp24;});_tmp23;}));}char Cyc_Tcutil_AbortTypeCheckingFunction[26]="AbortTypeCheckingFunction";
# 133
static struct _dyneither_ptr Cyc_Tcutil_tvar2string(struct Cyc_Absyn_Tvar*tv){
return*tv->name;}
# 137
void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){
for(0;tvs != 0;tvs=tvs->tl){
({struct Cyc_String_pa_PrintArg_struct _tmp28;_tmp28.tag=0;_tmp28.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind));({struct Cyc_String_pa_PrintArg_struct _tmp27;_tmp27.tag=0;_tmp27.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)tvs->hd));({void*_tmp25[2]={& _tmp27,& _tmp28};Cyc_fprintf(Cyc_stderr,({const char*_tmp26="%s::%s ";_tag_dyneither(_tmp26,sizeof(char),8);}),_tag_dyneither(_tmp25,sizeof(void*),2));});});});}
({void*_tmp29=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2A="\n";_tag_dyneither(_tmp2A,sizeof(char),2);}),_tag_dyneither(_tmp29,sizeof(void*),0));});Cyc_fflush(Cyc_stderr);}
# 144
static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;
# 149
void Cyc_Tcutil_warn(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 152
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
Cyc_Tcutil_warning_segs=({struct Cyc_List_List*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B->hd=(void*)sg;_tmp2B->tl=Cyc_Tcutil_warning_segs;_tmp2B;});
Cyc_Tcutil_warning_msgs=({struct Cyc_List_List*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C->hd=({struct _dyneither_ptr*_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D[0]=msg;_tmp2D;});_tmp2C->tl=Cyc_Tcutil_warning_msgs;_tmp2C;});}
# 156
void Cyc_Tcutil_flush_warnings(){
if(Cyc_Tcutil_warning_segs == 0)
return;
({void*_tmp2E=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2F="***Warnings***\n";_tag_dyneither(_tmp2F,sizeof(char),16);}),_tag_dyneither(_tmp2E,sizeof(void*),0));});{
struct Cyc_List_List*_tmp30=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);
Cyc_Tcutil_warning_segs=0;
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);
while(Cyc_Tcutil_warning_msgs != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp34;_tmp34.tag=0;_tmp34.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd));({struct Cyc_String_pa_PrintArg_struct _tmp33;_tmp33.tag=0;_tmp33.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp30))->hd));({void*_tmp31[2]={& _tmp33,& _tmp34};Cyc_fprintf(Cyc_stderr,({const char*_tmp32="%s: %s\n";_tag_dyneither(_tmp32,sizeof(char),8);}),_tag_dyneither(_tmp31,sizeof(void*),2));});});});
_tmp30=_tmp30->tl;
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}
# 168
({void*_tmp35=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp36="**************\n";_tag_dyneither(_tmp36,sizeof(char),16);}),_tag_dyneither(_tmp35,sizeof(void*),0));});
Cyc_fflush(Cyc_stderr);};}
# 173
struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set=0;
# 177
static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
return tv1->identity - tv2->identity;}
# 182
void*Cyc_Tcutil_compress(void*t){
void*_tmp37=t;void*_tmp38;struct Cyc_Absyn_Exp*_tmp39;struct Cyc_Absyn_Exp*_tmp3A;void**_tmp3B;void**_tmp3C;switch(*((int*)_tmp37)){case 1: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp37)->f2 == 0){_LL1: _LL2:
 goto _LL4;}else{_LL7: _tmp3C=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp37)->f2;_LL8: {
# 193
void*ta=(void*)_check_null(*_tmp3C);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp3C=t2;
return t2;}}case 17: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp37)->f4 == 0){_LL3: _LL4:
# 185
 return t;}else{_LL5: _tmp3B=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp37)->f4;_LL6: {
# 187
void*ta=(void*)_check_null(*_tmp3B);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp3B=t2;
return t2;}}case 18: _LL9: _tmp3A=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp37)->f1;_LLA:
# 199
 Cyc_Evexp_eval_const_uint_exp(_tmp3A);{
void*_tmp3D=_tmp3A->r;void*_tmp3E=_tmp3D;void*_tmp3F;if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp3E)->tag == 38){_LL12: _tmp3F=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp3E)->f1;_LL13:
 return Cyc_Tcutil_compress(_tmp3F);}else{_LL14: _LL15:
 return t;}_LL11:;};case 27: _LLB: _tmp39=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp37)->f1;_LLC: {
# 205
void*_tmp40=_tmp39->topt;
if(_tmp40 != 0)return _tmp40;else{
return t;}}case 26: if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp37)->f2 != 0){_LLD: _tmp38=*((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp37)->f2;_LLE:
# 209
 return Cyc_Tcutil_compress(_tmp38);}else{goto _LLF;}default: _LLF: _LL10:
 return t;}_LL0:;}
# 217
void*Cyc_Tcutil_copy_type(void*t);
static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){
return((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);}
# 221
static union Cyc_Absyn_Constraint*Cyc_Tcutil_copy_conref(union Cyc_Absyn_Constraint*cptr){
union Cyc_Absyn_Constraint*_tmp41=cptr;union Cyc_Absyn_Constraint*_tmp42;void*_tmp43;switch((((union Cyc_Absyn_Constraint*)_tmp41)->Eq_constr).tag){case 3: _LL17: _LL18:
 return Cyc_Absyn_empty_conref();case 1: _LL19: _tmp43=(void*)(_tmp41->Eq_constr).val;_LL1A:
 return Cyc_Absyn_new_conref(_tmp43);default: _LL1B: _tmp42=(_tmp41->Forward_constr).val;_LL1C:
 return Cyc_Tcutil_copy_conref(_tmp42);}_LL16:;}
# 228
static void*Cyc_Tcutil_copy_kindbound(void*kb){
void*_tmp44=Cyc_Absyn_compress_kb(kb);void*_tmp45=_tmp44;struct Cyc_Absyn_Kind*_tmp46;switch(*((int*)_tmp45)){case 1: _LL1E: _LL1F:
 return(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp47=_cycalloc(sizeof(*_tmp47));_tmp47[0]=({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp48;_tmp48.tag=1;_tmp48.f1=0;_tmp48;});_tmp47;});case 2: _LL20: _tmp46=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp45)->f2;_LL21:
 return(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp4A;_tmp4A.tag=2;_tmp4A.f1=0;_tmp4A.f2=_tmp46;_tmp4A;});_tmp49;});default: _LL22: _LL23:
 return kb;}_LL1D:;}
# 235
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 237
return({struct Cyc_Absyn_Tvar*_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B->name=tv->name;_tmp4B->identity=- 1;_tmp4B->kind=Cyc_Tcutil_copy_kindbound(tv->kind);_tmp4B;});}
# 239
static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
# 241
struct _tuple10*_tmp4C=arg;struct _dyneither_ptr*_tmp4D;struct Cyc_Absyn_Tqual _tmp4E;void*_tmp4F;_LL25: _tmp4D=_tmp4C->f1;_tmp4E=_tmp4C->f2;_tmp4F=_tmp4C->f3;_LL26:;
return({struct _tuple10*_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50->f1=_tmp4D;_tmp50->f2=_tmp4E;_tmp50->f3=Cyc_Tcutil_copy_type(_tmp4F);_tmp50;});}
# 244
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
struct _tuple12*_tmp51=arg;struct Cyc_Absyn_Tqual _tmp52;void*_tmp53;_LL28: _tmp52=_tmp51->f1;_tmp53=_tmp51->f2;_LL29:;
return({struct _tuple12*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->f1=_tmp52;_tmp54->f2=Cyc_Tcutil_copy_type(_tmp53);_tmp54;});}
# 248
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*);
# 250
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
return({struct Cyc_Absyn_Aggrfield*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55->name=f->name;_tmp55->tq=f->tq;_tmp55->type=Cyc_Tcutil_copy_type(f->type);_tmp55->width=f->width;_tmp55->attributes=f->attributes;_tmp55->requires_clause=
Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause);_tmp55;});}
# 254
static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
struct _tuple0*_tmp56=x;void*_tmp57;void*_tmp58;_LL2B: _tmp57=_tmp56->f1;_tmp58=_tmp56->f2;_LL2C:;
return({struct _tuple0*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59->f1=Cyc_Tcutil_copy_type(_tmp57);_tmp59->f2=Cyc_Tcutil_copy_type(_tmp58);_tmp59;});}
# 258
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){
return({struct Cyc_Absyn_Enumfield*_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A->name=f->name;_tmp5A->tag=f->tag;_tmp5A->loc=f->loc;_tmp5A;});}
# 261
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp5B=_cycalloc(sizeof(*_tmp5B));_tmp5B[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp5C;_tmp5C.tag=2;_tmp5C.f1=Cyc_Tcutil_copy_tvar(t);_tmp5C;});_tmp5B;});}
# 265
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp5D=Cyc_Tcutil_compress(t);void*_tmp5E=_tmp5D;struct Cyc_Absyn_Datatypedecl*_tmp5F;struct Cyc_Absyn_Enumdecl*_tmp60;struct Cyc_Absyn_Aggrdecl*_tmp61;void*_tmp62;struct Cyc_List_List*_tmp63;void*_tmp64;struct _tuple2*_tmp65;struct Cyc_List_List*_tmp66;struct Cyc_Absyn_Typedefdecl*_tmp67;void*_tmp68;void*_tmp69;void*_tmp6A;struct Cyc_Absyn_Exp*_tmp6B;struct Cyc_Absyn_Exp*_tmp6C;void*_tmp6D;struct Cyc_List_List*_tmp6E;struct _tuple2*_tmp6F;struct Cyc_Absyn_Enumdecl*_tmp70;enum Cyc_Absyn_AggrKind _tmp71;struct Cyc_List_List*_tmp72;struct Cyc_Absyn_Aggrdecl**_tmp73;struct Cyc_List_List*_tmp74;enum Cyc_Absyn_AggrKind _tmp75;struct _tuple2*_tmp76;struct Cyc_Core_Opt*_tmp77;struct Cyc_List_List*_tmp78;struct Cyc_List_List*_tmp79;struct Cyc_List_List*_tmp7A;void*_tmp7B;struct Cyc_Absyn_Tqual _tmp7C;void*_tmp7D;struct Cyc_List_List*_tmp7E;int _tmp7F;struct Cyc_Absyn_VarargInfo*_tmp80;struct Cyc_List_List*_tmp81;struct Cyc_List_List*_tmp82;struct Cyc_Absyn_Exp*_tmp83;struct Cyc_List_List*_tmp84;struct Cyc_Absyn_Exp*_tmp85;struct Cyc_List_List*_tmp86;void*_tmp87;struct Cyc_Absyn_Tqual _tmp88;struct Cyc_Absyn_Exp*_tmp89;union Cyc_Absyn_Constraint*_tmp8A;unsigned int _tmp8B;void*_tmp8C;struct Cyc_Absyn_Tqual _tmp8D;void*_tmp8E;union Cyc_Absyn_Constraint*_tmp8F;union Cyc_Absyn_Constraint*_tmp90;union Cyc_Absyn_Constraint*_tmp91;struct Cyc_Absyn_PtrLoc*_tmp92;union Cyc_Absyn_DatatypeFieldInfoU _tmp93;struct Cyc_List_List*_tmp94;union Cyc_Absyn_DatatypeInfoU _tmp95;struct Cyc_List_List*_tmp96;struct Cyc_Absyn_Tvar*_tmp97;switch(*((int*)_tmp5E)){case 0: _LL2E: _LL2F:
 goto _LL31;case 1: _LL30: _LL31:
 return t;case 2: _LL32: _tmp97=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp5E)->f1;_LL33:
 return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp99;_tmp99.tag=2;_tmp99.f1=Cyc_Tcutil_copy_tvar(_tmp97);_tmp99;});_tmp98;});case 3: _LL34: _tmp95=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5E)->f1).datatype_info;_tmp96=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5E)->f1).targs;_LL35:
# 271
 return(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A[0]=({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp9B;_tmp9B.tag=3;_tmp9B.f1=({struct Cyc_Absyn_DatatypeInfo _tmp9C;_tmp9C.datatype_info=_tmp95;_tmp9C.targs=Cyc_Tcutil_copy_types(_tmp96);_tmp9C;});_tmp9B;});_tmp9A;});case 4: _LL36: _tmp93=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp5E)->f1).field_info;_tmp94=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp5E)->f1).targs;_LL37:
# 273
 return(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D[0]=({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp9E;_tmp9E.tag=4;_tmp9E.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmp9F;_tmp9F.field_info=_tmp93;_tmp9F.targs=Cyc_Tcutil_copy_types(_tmp94);_tmp9F;});_tmp9E;});_tmp9D;});case 5: _LL38: _tmp8C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).elt_typ;_tmp8D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).elt_tq;_tmp8E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).ptr_atts).rgn;_tmp8F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).ptr_atts).nullable;_tmp90=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).ptr_atts).bounds;_tmp91=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).ptr_atts).zero_term;_tmp92=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).ptr_atts).ptrloc;_LL39: {
# 275
void*_tmpA0=Cyc_Tcutil_copy_type(_tmp8C);
void*_tmpA1=Cyc_Tcutil_copy_type(_tmp8E);
union Cyc_Absyn_Constraint*_tmpA2=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp8F);
struct Cyc_Absyn_Tqual _tmpA3=_tmp8D;
# 280
union Cyc_Absyn_Constraint*_tmpA4=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp90);
union Cyc_Absyn_Constraint*_tmpA5=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp91);
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpA7;_tmpA7.tag=5;_tmpA7.f1=({struct Cyc_Absyn_PtrInfo _tmpA8;_tmpA8.elt_typ=_tmpA0;_tmpA8.elt_tq=_tmpA3;_tmpA8.ptr_atts=({(_tmpA8.ptr_atts).rgn=_tmpA1;(_tmpA8.ptr_atts).nullable=_tmpA2;(_tmpA8.ptr_atts).bounds=_tmpA4;(_tmpA8.ptr_atts).zero_term=_tmpA5;(_tmpA8.ptr_atts).ptrloc=_tmp92;_tmpA8.ptr_atts;});_tmpA8;});_tmpA7;});_tmpA6;});}case 6: _LL3A: _LL3B:
 goto _LL3D;case 7: _LL3C: _LL3D:
 return t;case 8: _LL3E: _tmp87=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E)->f1).elt_type;_tmp88=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E)->f1).tq;_tmp89=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E)->f1).num_elts;_tmp8A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E)->f1).zero_term;_tmp8B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E)->f1).zt_loc;_LL3F: {
# 286
struct Cyc_Absyn_Exp*eopt2=Cyc_Tcutil_deep_copy_exp_opt(1,_tmp89);
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpAA;_tmpAA.tag=8;_tmpAA.f1=({struct Cyc_Absyn_ArrayInfo _tmpAB;_tmpAB.elt_type=Cyc_Tcutil_copy_type(_tmp87);_tmpAB.tq=_tmp88;_tmpAB.num_elts=eopt2;_tmpAB.zero_term=
((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp8A);_tmpAB.zt_loc=_tmp8B;_tmpAB;});_tmpAA;});_tmpA9;});}case 9: _LL40: _tmp7A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).tvars;_tmp7B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).effect;_tmp7C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).ret_tqual;_tmp7D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).ret_typ;_tmp7E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).args;_tmp7F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).c_varargs;_tmp80=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).cyc_varargs;_tmp81=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).rgn_po;_tmp82=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).attributes;_tmp83=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).requires_clause;_tmp84=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).requires_relns;_tmp85=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).ensures_clause;_tmp86=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).ensures_relns;_LL41: {
# 290
struct Cyc_List_List*_tmpAC=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp7A);
void*effopt2=_tmp7B == 0?0: Cyc_Tcutil_copy_type(_tmp7B);
void*_tmpAD=Cyc_Tcutil_copy_type(_tmp7D);
struct Cyc_List_List*_tmpAE=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp7E);
int _tmpAF=_tmp7F;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(_tmp80 != 0){
struct Cyc_Absyn_VarargInfo*cv=_tmp80;
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0->name=cv->name;_tmpB0->tq=cv->tq;_tmpB0->type=Cyc_Tcutil_copy_type(cv->type);_tmpB0->inject=cv->inject;_tmpB0;});}{
# 301
struct Cyc_List_List*_tmpB1=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp81);
struct Cyc_List_List*_tmpB2=_tmp82;
struct Cyc_Absyn_Exp*_tmpB3=Cyc_Tcutil_deep_copy_exp_opt(1,_tmp83);
struct Cyc_List_List*_tmpB4=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp84);
struct Cyc_Absyn_Exp*_tmpB5=Cyc_Tcutil_deep_copy_exp_opt(1,_tmp85);
struct Cyc_List_List*_tmpB6=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp86);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpB8;_tmpB8.tag=9;_tmpB8.f1=({struct Cyc_Absyn_FnInfo _tmpB9;_tmpB9.tvars=_tmpAC;_tmpB9.effect=effopt2;_tmpB9.ret_tqual=_tmp7C;_tmpB9.ret_typ=_tmpAD;_tmpB9.args=_tmpAE;_tmpB9.c_varargs=_tmpAF;_tmpB9.cyc_varargs=cyc_varargs2;_tmpB9.rgn_po=_tmpB1;_tmpB9.attributes=_tmpB2;_tmpB9.requires_clause=_tmpB3;_tmpB9.requires_relns=_tmpB4;_tmpB9.ensures_clause=_tmpB5;_tmpB9.ensures_relns=_tmpB6;_tmpB9;});_tmpB8;});_tmpB7;});};}case 10: _LL42: _tmp79=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5E)->f1;_LL43:
# 310
 return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA[0]=({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpBB;_tmpBB.tag=10;_tmpBB.f1=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp79);_tmpBB;});_tmpBA;});case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5E)->f1).aggr_info).UnknownAggr).tag == 1){_LL44: _tmp75=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5E)->f1).aggr_info).UnknownAggr).val).f1;_tmp76=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5E)->f1).aggr_info).UnknownAggr).val).f2;_tmp77=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5E)->f1).aggr_info).UnknownAggr).val).f3;_tmp78=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5E)->f1).targs;_LL45:
# 312
 return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpBD;_tmpBD.tag=11;_tmpBD.f1=({struct Cyc_Absyn_AggrInfo _tmpBE;_tmpBE.aggr_info=Cyc_Absyn_UnknownAggr(_tmp75,_tmp76,_tmp77);_tmpBE.targs=Cyc_Tcutil_copy_types(_tmp78);_tmpBE;});_tmpBD;});_tmpBC;});}else{_LL46: _tmp73=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5E)->f1).aggr_info).KnownAggr).val;_tmp74=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5E)->f1).targs;_LL47:
# 314
 return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpC0;_tmpC0.tag=11;_tmpC0.f1=({struct Cyc_Absyn_AggrInfo _tmpC1;_tmpC1.aggr_info=Cyc_Absyn_KnownAggr(_tmp73);_tmpC1.targs=Cyc_Tcutil_copy_types(_tmp74);_tmpC1;});_tmpC0;});_tmpBF;});}case 12: _LL48: _tmp71=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5E)->f1;_tmp72=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5E)->f2;_LL49:
 return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2[0]=({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpC3;_tmpC3.tag=12;_tmpC3.f1=_tmp71;_tmpC3.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmp72);_tmpC3;});_tmpC2;});case 13: _LL4A: _tmp6F=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5E)->f1;_tmp70=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5E)->f2;_LL4B:
 return(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4[0]=({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpC5;_tmpC5.tag=13;_tmpC5.f1=_tmp6F;_tmpC5.f2=_tmp70;_tmpC5;});_tmpC4;});case 14: _LL4C: _tmp6E=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp5E)->f1;_LL4D:
 return(void*)({struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6[0]=({struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpC7;_tmpC7.tag=14;_tmpC7.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmp6E);_tmpC7;});_tmpC6;});case 19: _LL4E: _tmp6D=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5E)->f1;_LL4F:
 return(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpC8=_cycalloc(sizeof(*_tmpC8));_tmpC8[0]=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpC9;_tmpC9.tag=19;_tmpC9.f1=Cyc_Tcutil_copy_type(_tmp6D);_tmpC9;});_tmpC8;});case 18: _LL50: _tmp6C=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp5E)->f1;_LL51:
# 321
 return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpCB;_tmpCB.tag=18;_tmpCB.f1=_tmp6C;_tmpCB;});_tmpCA;});case 27: _LL52: _tmp6B=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp5E)->f1;_LL53:
# 324
 return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC[0]=({struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmpCD;_tmpCD.tag=27;_tmpCD.f1=_tmp6B;_tmpCD;});_tmpCC;});case 15: _LL54: _tmp6A=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5E)->f1;_LL55:
 return(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpCE=_cycalloc(sizeof(*_tmpCE));_tmpCE[0]=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpCF;_tmpCF.tag=15;_tmpCF.f1=Cyc_Tcutil_copy_type(_tmp6A);_tmpCF;});_tmpCE;});case 16: _LL56: _tmp68=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp5E)->f1;_tmp69=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp5E)->f2;_LL57:
 return(void*)({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0[0]=({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpD1;_tmpD1.tag=16;_tmpD1.f1=Cyc_Tcutil_copy_type(_tmp68);_tmpD1.f2=Cyc_Tcutil_copy_type(_tmp69);_tmpD1;});_tmpD0;});case 17: _LL58: _tmp65=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5E)->f1;_tmp66=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5E)->f2;_tmp67=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5E)->f3;_LL59:
# 328
 return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2[0]=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpD3;_tmpD3.tag=17;_tmpD3.f1=_tmp65;_tmpD3.f2=Cyc_Tcutil_copy_types(_tmp66);_tmpD3.f3=_tmp67;_tmpD3.f4=0;_tmpD3;});_tmpD2;});case 21: _LL5A: _LL5B:
 goto _LL5D;case 22: _LL5C: _LL5D:
 goto _LL5F;case 28: _LL5E: _LL5F:
 goto _LL61;case 20: _LL60: _LL61:
 return t;case 23: _LL62: _tmp64=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp5E)->f1;_LL63:
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpD5;_tmpD5.tag=23;_tmpD5.f1=Cyc_Tcutil_copy_type(_tmp64);_tmpD5;});_tmpD4;});case 24: _LL64: _tmp63=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp5E)->f1;_LL65:
 return(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD6=_cycalloc(sizeof(*_tmpD6));_tmpD6[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD7;_tmpD7.tag=24;_tmpD7.f1=Cyc_Tcutil_copy_types(_tmp63);_tmpD7;});_tmpD6;});case 25: _LL66: _tmp62=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp5E)->f1;_LL67:
 return(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpD9;_tmpD9.tag=25;_tmpD9.f1=Cyc_Tcutil_copy_type(_tmp62);_tmpD9;});_tmpD8;});default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5E)->f1)->r)){case 0: _LL68: _tmp61=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5E)->f1)->r)->f1;_LL69: {
# 338
struct Cyc_List_List*_tmpDA=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmp61->tvs);
return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpDB=_cycalloc(sizeof(*_tmpDB));_tmpDB[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpDC;_tmpDC.tag=11;_tmpDC.f1=({struct Cyc_Absyn_AggrInfo _tmpDD;_tmpDD.aggr_info=Cyc_Absyn_UnknownAggr(_tmp61->kind,_tmp61->name,0);_tmpDD.targs=_tmpDA;_tmpDD;});_tmpDC;});_tmpDB;});}case 1: _LL6A: _tmp60=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5E)->f1)->r)->f1;_LL6B:
# 341
 return(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE[0]=({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpDF;_tmpDF.tag=13;_tmpDF.f1=_tmp60->name;_tmpDF.f2=0;_tmpDF;});_tmpDE;});default: _LL6C: _tmp5F=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5E)->f1)->r)->f1;_LL6D: {
# 343
struct Cyc_List_List*_tmpE0=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmp5F->tvs);
return(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1[0]=({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpE2;_tmpE2.tag=3;_tmpE2.f1=({struct Cyc_Absyn_DatatypeInfo _tmpE3;_tmpE3.datatype_info=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmpE4;_tmpE4.name=_tmp5F->name;_tmpE4.is_extensible=0;_tmpE4;}));_tmpE3.targs=_tmpE0;_tmpE3;});_tmpE2;});_tmpE1;});}}}_LL2D:;}
# 358 "tcutil.cyc"
static void*Cyc_Tcutil_copy_designator(int pt,void*d){
void*_tmpE5=d;struct _dyneither_ptr*_tmpE6;struct Cyc_Absyn_Exp*_tmpE7;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpE5)->tag == 0){_LL6F: _tmpE7=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpE5)->f1;_LL70:
 return(void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpE8=_cycalloc(sizeof(*_tmpE8));_tmpE8[0]=({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpE9;_tmpE9.tag=0;_tmpE9.f1=Cyc_Tcutil_deep_copy_exp(pt,_tmpE7);_tmpE9;});_tmpE8;});}else{_LL71: _tmpE6=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpE5)->f1;_LL72:
 return d;}_LL6E:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 364
static struct _tuple19*Cyc_Tcutil_copy_eds(int pt,struct _tuple19*e){
# 366
return({struct _tuple19*_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA->f1=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,pt,(e[0]).f1);_tmpEA->f2=Cyc_Tcutil_deep_copy_exp(pt,(e[0]).f2);_tmpEA;});}
# 369
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*new_e;
int _tmpEB=preserve_types;
{void*_tmpEC=e->r;void*_tmpED=_tmpEC;int _tmpEE;struct _dyneither_ptr _tmpEF;void*_tmpF0;struct Cyc_Absyn_Exp*_tmpF1;struct _dyneither_ptr*_tmpF2;struct Cyc_Core_Opt*_tmpF3;struct Cyc_List_List*_tmpF4;struct Cyc_Absyn_Exp*_tmpF5;struct Cyc_Absyn_Exp*_tmpF6;int _tmpF7;struct Cyc_Absyn_Exp*_tmpF8;void**_tmpF9;struct Cyc_Absyn_Exp*_tmpFA;int _tmpFB;int _tmpFC;struct _tuple2*_tmpFD;void*_tmpFE;struct Cyc_Absyn_Enumfield*_tmpFF;struct _tuple2*_tmp100;struct Cyc_Absyn_Enumdecl*_tmp101;struct Cyc_Absyn_Enumfield*_tmp102;struct Cyc_List_List*_tmp103;struct Cyc_Absyn_Datatypedecl*_tmp104;struct Cyc_Absyn_Datatypefield*_tmp105;void*_tmp106;struct Cyc_List_List*_tmp107;struct _tuple2*_tmp108;struct Cyc_List_List*_tmp109;struct Cyc_List_List*_tmp10A;struct Cyc_Absyn_Aggrdecl*_tmp10B;struct Cyc_Absyn_Exp*_tmp10C;void*_tmp10D;int _tmp10E;struct Cyc_Absyn_Vardecl*_tmp10F;struct Cyc_Absyn_Exp*_tmp110;struct Cyc_Absyn_Exp*_tmp111;int _tmp112;struct Cyc_List_List*_tmp113;struct _dyneither_ptr*_tmp114;struct Cyc_Absyn_Tqual _tmp115;void*_tmp116;struct Cyc_List_List*_tmp117;struct Cyc_List_List*_tmp118;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp11B;struct _dyneither_ptr*_tmp11C;int _tmp11D;int _tmp11E;struct Cyc_Absyn_Exp*_tmp11F;struct _dyneither_ptr*_tmp120;int _tmp121;int _tmp122;struct Cyc_Absyn_Exp*_tmp123;void*_tmp124;struct Cyc_List_List*_tmp125;struct Cyc_Absyn_Exp*_tmp126;void*_tmp127;struct Cyc_Absyn_Exp*_tmp128;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_Absyn_Exp*_tmp12A;void*_tmp12B;struct Cyc_Absyn_Exp*_tmp12C;int _tmp12D;enum Cyc_Absyn_Coercion _tmp12E;struct Cyc_Absyn_Exp*_tmp12F;struct Cyc_List_List*_tmp130;struct Cyc_Absyn_Exp*_tmp131;struct Cyc_Absyn_Exp*_tmp132;int _tmp133;struct Cyc_Absyn_Exp*_tmp134;struct Cyc_List_List*_tmp135;struct Cyc_Absyn_VarargCallInfo*_tmp136;int _tmp137;struct Cyc_Absyn_Exp*_tmp138;struct Cyc_Absyn_Exp*_tmp139;struct Cyc_Absyn_Exp*_tmp13A;struct Cyc_Absyn_Exp*_tmp13B;struct Cyc_Absyn_Exp*_tmp13C;struct Cyc_Absyn_Exp*_tmp13D;struct Cyc_Absyn_Exp*_tmp13E;struct Cyc_Absyn_Exp*_tmp13F;struct Cyc_Absyn_Exp*_tmp140;struct Cyc_Absyn_Exp*_tmp141;enum Cyc_Absyn_Incrementor _tmp142;struct Cyc_Absyn_Exp*_tmp143;struct Cyc_Core_Opt*_tmp144;struct Cyc_Absyn_Exp*_tmp145;enum Cyc_Absyn_Primop _tmp146;struct Cyc_List_List*_tmp147;void*_tmp148;union Cyc_Absyn_Cnst _tmp149;switch(*((int*)_tmpED)){case 0: _LL74: _tmp149=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LL75:
 new_e=Cyc_Absyn_const_exp(_tmp149,e->loc);goto _LL73;case 1: _LL76: _tmp148=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LL77:
 new_e=Cyc_Absyn_varb_exp(_tmp148,e->loc);goto _LL73;case 2: _LL78: _tmp146=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp147=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL79:
 new_e=Cyc_Absyn_primop_exp(_tmp146,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpEB,_tmp147),e->loc);goto _LL73;case 3: _LL7A: _tmp143=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp144=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp145=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_LL7B:
# 377
 new_e=Cyc_Absyn_assignop_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp143),(unsigned int)_tmp144?({struct Cyc_Core_Opt*_tmp14A=_cycalloc_atomic(sizeof(*_tmp14A));_tmp14A->v=(void*)_tmp144->v;_tmp14A;}): 0,Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp145),e->loc);
goto _LL73;case 4: _LL7C: _tmp141=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp142=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL7D:
 new_e=Cyc_Absyn_increment_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp141),_tmp142,e->loc);goto _LL73;case 5: _LL7E: _tmp13E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp13F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp140=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_LL7F:
# 381
 new_e=Cyc_Absyn_conditional_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp13E),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp13F),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp140),e->loc);goto _LL73;case 6: _LL80: _tmp13C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp13D=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL81:
 new_e=Cyc_Absyn_and_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp13C),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp13D),e->loc);goto _LL73;case 7: _LL82: _tmp13A=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp13B=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL83:
 new_e=Cyc_Absyn_or_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp13A),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp13B),e->loc);goto _LL73;case 8: _LL84: _tmp138=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp139=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL85:
 new_e=Cyc_Absyn_seq_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp138),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp139),e->loc);goto _LL73;case 9: _LL86: _tmp134=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp135=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp136=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_tmp137=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpED)->f4;_LL87:
# 386
{struct Cyc_Absyn_VarargCallInfo*_tmp14B=_tmp136;int _tmp14C;struct Cyc_List_List*_tmp14D;struct Cyc_Absyn_VarargInfo*_tmp14E;if(_tmp14B != 0){_LLC5: _tmp14C=_tmp14B->num_varargs;_tmp14D=_tmp14B->injectors;_tmp14E=_tmp14B->vai;_LLC6: {
# 388
struct Cyc_Absyn_VarargInfo*_tmp14F=_tmp14E;struct _dyneither_ptr*_tmp150;struct Cyc_Absyn_Tqual _tmp151;void*_tmp152;int _tmp153;_LLCA: _tmp150=_tmp14F->name;_tmp151=_tmp14F->tq;_tmp152=_tmp14F->type;_tmp153=_tmp14F->inject;_LLCB:;
new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp154=_cycalloc(sizeof(*_tmp154));_tmp154[0]=({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp155;_tmp155.tag=9;_tmp155.f1=
Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp134);_tmp155.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpEB,_tmp135);_tmp155.f3=({struct Cyc_Absyn_VarargCallInfo*_tmp156=_cycalloc(sizeof(*_tmp156));_tmp156->num_varargs=_tmp14C;_tmp156->injectors=_tmp14D;_tmp156->vai=({struct Cyc_Absyn_VarargInfo*_tmp157=_cycalloc(sizeof(*_tmp157));_tmp157->name=_tmp150;_tmp157->tq=_tmp151;_tmp157->type=
# 392
Cyc_Tcutil_copy_type(_tmp152);_tmp157->inject=_tmp153;_tmp157;});_tmp156;});_tmp155.f4=_tmp137;_tmp155;});_tmp154;}),e->loc);
# 394
goto _LLC4;}}else{_LLC7: _LLC8:
# 396
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp158=_cycalloc(sizeof(*_tmp158));_tmp158[0]=({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp159;_tmp159.tag=9;_tmp159.f1=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp134);_tmp159.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpEB,_tmp135);_tmp159.f3=_tmp136;_tmp159.f4=_tmp137;_tmp159;});_tmp158;}),e->loc);
goto _LLC4;}_LLC4:;}
# 399
goto _LL73;case 10: _LL88: _tmp132=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp133=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL89:
# 401
 new_e=_tmp133?Cyc_Absyn_rethrow_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp132),e->loc): Cyc_Absyn_throw_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp132),e->loc);
goto _LL73;case 11: _LL8A: _tmp131=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LL8B:
 new_e=Cyc_Absyn_noinstantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp131),e->loc);
goto _LL73;case 12: _LL8C: _tmp12F=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp130=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL8D:
# 406
 new_e=Cyc_Absyn_instantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp12F),((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp130),e->loc);
goto _LL73;case 13: _LL8E: _tmp12B=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp12C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp12D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_tmp12E=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpED)->f4;_LL8F:
# 409
 new_e=Cyc_Absyn_cast_exp(Cyc_Tcutil_copy_type(_tmp12B),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp12C),_tmp12D,_tmp12E,e->loc);goto _LL73;case 14: _LL90: _tmp12A=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LL91:
 new_e=Cyc_Absyn_address_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp12A),e->loc);goto _LL73;case 15: _LL92: _tmp128=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp129=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL93: {
# 412
struct Cyc_Absyn_Exp*eo1=_tmp128;if(_tmp128 != 0)eo1=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp128);
new_e=Cyc_Absyn_New_exp(eo1,Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp129),e->loc);
goto _LL73;}case 16: _LL94: _tmp127=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LL95:
 new_e=Cyc_Absyn_sizeoftyp_exp(Cyc_Tcutil_copy_type(_tmp127),e->loc);
goto _LL73;case 17: _LL96: _tmp126=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LL97:
 new_e=Cyc_Absyn_sizeofexp_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp126),e->loc);goto _LL73;case 18: _LL98: _tmp124=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp125=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL99:
# 419
 new_e=Cyc_Absyn_offsetof_exp(Cyc_Tcutil_copy_type(_tmp124),_tmp125,e->loc);goto _LL73;case 19: _LL9A: _tmp123=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LL9B:
 new_e=Cyc_Absyn_deref_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp123),e->loc);goto _LL73;case 20: _LL9C: _tmp11F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp120=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp121=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_tmp122=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpED)->f4;_LL9D:
# 422
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp15A=_cycalloc(sizeof(*_tmp15A));_tmp15A[0]=({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmp15B;_tmp15B.tag=20;_tmp15B.f1=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp11F);_tmp15B.f2=_tmp120;_tmp15B.f3=_tmp121;_tmp15B.f4=_tmp122;_tmp15B;});_tmp15A;}),e->loc);goto _LL73;case 21: _LL9E: _tmp11B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp11C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp11D=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_tmp11E=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpED)->f4;_LL9F:
# 424
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp15C=_cycalloc(sizeof(*_tmp15C));_tmp15C[0]=({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmp15D;_tmp15D.tag=21;_tmp15D.f1=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp11B);_tmp15D.f2=_tmp11C;_tmp15D.f3=_tmp11D;_tmp15D.f4=_tmp11E;_tmp15D;});_tmp15C;}),e->loc);goto _LL73;case 22: _LLA0: _tmp119=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp11A=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LLA1:
 new_e=Cyc_Absyn_subscript_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp119),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp11A),e->loc);
goto _LL73;case 23: _LLA2: _tmp118=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LLA3:
 new_e=Cyc_Absyn_tuple_exp(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpEB,_tmp118),e->loc);goto _LL73;case 24: _LLA4: _tmp114=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpED)->f1)->f1;_tmp115=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpED)->f1)->f2;_tmp116=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpED)->f1)->f3;_tmp117=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LLA5: {
# 429
struct _dyneither_ptr*vopt1=_tmp114;
if(_tmp114 != 0)vopt1=_tmp114;
new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp15E=_cycalloc(sizeof(*_tmp15E));_tmp15E[0]=({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmp15F;_tmp15F.tag=24;_tmp15F.f1=({struct _tuple10*_tmp160=_cycalloc(sizeof(*_tmp160));_tmp160->f1=vopt1;_tmp160->f2=_tmp115;_tmp160->f3=Cyc_Tcutil_copy_type(_tmp116);_tmp160;});_tmp15F.f2=
((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpEB,_tmp117);_tmp15F;});_tmp15E;}),e->loc);
goto _LL73;}case 25: _LLA6: _tmp113=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LLA7:
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp161=_cycalloc(sizeof(*_tmp161));_tmp161[0]=({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp162;_tmp162.tag=25;_tmp162.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpEB,_tmp113);_tmp162;});_tmp161;}),e->loc);
goto _LL73;case 26: _LLA8: _tmp10F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp110=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp111=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_tmp112=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpED)->f4;_LLA9:
# 437
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp163=_cycalloc(sizeof(*_tmp163));_tmp163[0]=({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmp164;_tmp164.tag=26;_tmp164.f1=_tmp10F;_tmp164.f2=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp110);_tmp164.f3=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp111);_tmp164.f4=_tmp112;_tmp164;});_tmp163;}),e->loc);
goto _LL73;case 27: _LLAA: _tmp10C=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp10D=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp10E=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_LLAB:
# 440
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp165=_cycalloc(sizeof(*_tmp165));_tmp165[0]=({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmp166;_tmp166.tag=27;_tmp166.f1=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp10C);_tmp166.f2=Cyc_Tcutil_copy_type(_tmp10D);_tmp166.f3=_tmp10E;_tmp166;});_tmp165;}),_tmp10C->loc);
# 442
goto _LL73;case 28: _LLAC: _tmp108=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp109=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp10A=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_tmp10B=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpED)->f4;_LLAD:
# 444
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp167=_cycalloc(sizeof(*_tmp167));_tmp167[0]=({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp168;_tmp168.tag=28;_tmp168.f1=_tmp108;_tmp168.f2=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp109);_tmp168.f3=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpEB,_tmp10A);_tmp168.f4=_tmp10B;_tmp168;});_tmp167;}),e->loc);
# 446
goto _LL73;case 29: _LLAE: _tmp106=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp107=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LLAF:
# 448
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp169=_cycalloc(sizeof(*_tmp169));_tmp169[0]=({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmp16A;_tmp16A.tag=29;_tmp16A.f1=Cyc_Tcutil_copy_type(_tmp106);_tmp16A.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpEB,_tmp107);_tmp16A;});_tmp169;}),e->loc);
goto _LL73;case 30: _LLB0: _tmp103=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp104=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp105=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_LLB1:
# 451
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp16B=_cycalloc(sizeof(*_tmp16B));_tmp16B[0]=({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp16C;_tmp16C.tag=30;_tmp16C.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpEB,_tmp103);_tmp16C.f2=_tmp104;_tmp16C.f3=_tmp105;_tmp16C;});_tmp16B;}),e->loc);
goto _LL73;case 31: _LLB2: _tmp100=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp101=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp102=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_LLB3:
 new_e=e;goto _LL73;case 32: _LLB4: _tmpFD=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmpFE=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmpFF=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_LLB5:
# 455
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp16D=_cycalloc(sizeof(*_tmp16D));_tmp16D[0]=({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp16E;_tmp16E.tag=32;_tmp16E.f1=_tmpFD;_tmp16E.f2=Cyc_Tcutil_copy_type(_tmpFE);_tmp16E.f3=_tmpFF;_tmp16E;});_tmp16D;}),e->loc);
goto _LL73;case 33: _LLB6: _tmpF7=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpED)->f1).is_calloc;_tmpF8=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpED)->f1).rgn;_tmpF9=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpED)->f1).elt_type;_tmpFA=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpED)->f1).num_elts;_tmpFB=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpED)->f1).fat_result;_tmpFC=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpED)->f1).inline_call;_LLB7: {
# 458
struct Cyc_Absyn_Exp*_tmp16F=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmpF8;if(_tmpF8 != 0)r1=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmpF8);{
void**t1=_tmpF9;if(_tmpF9 != 0)t1=({void**_tmp170=_cycalloc(sizeof(*_tmp170));_tmp170[0]=Cyc_Tcutil_copy_type(*_tmpF9);_tmp170;});
_tmp16F->r=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp171=_cycalloc(sizeof(*_tmp171));_tmp171[0]=({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmp172;_tmp172.tag=33;_tmp172.f1=({struct Cyc_Absyn_MallocInfo _tmp173;_tmp173.is_calloc=_tmpF7;_tmp173.rgn=r1;_tmp173.elt_type=t1;_tmp173.num_elts=_tmpFA;_tmp173.fat_result=_tmpFB;_tmp173.inline_call=_tmpFC;_tmp173;});_tmp172;});_tmp171;});
new_e=_tmp16F;
goto _LL73;};}case 34: _LLB8: _tmpF5=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmpF6=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LLB9:
 new_e=Cyc_Absyn_swap_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmpF5),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmpF6),e->loc);goto _LL73;case 35: _LLBA: _tmpF3=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmpF4=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LLBB: {
# 466
struct Cyc_Core_Opt*nopt1=_tmpF3;
if(_tmpF3 != 0)nopt1=({struct Cyc_Core_Opt*_tmp174=_cycalloc(sizeof(*_tmp174));_tmp174->v=(struct _tuple2*)_tmpF3->v;_tmp174;});
new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp175=_cycalloc(sizeof(*_tmp175));_tmp175[0]=({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp176;_tmp176.tag=35;_tmp176.f1=nopt1;_tmp176.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpEB,_tmpF4);_tmp176;});_tmp175;}),e->loc);
goto _LL73;}case 36: _LLBC: _LLBD:
# 471
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp177=_cycalloc(sizeof(*_tmp177));_tmp177[0]=({struct Cyc_Core_Failure_exn_struct _tmp178;_tmp178.tag=Cyc_Core_Failure;_tmp178.f1=({const char*_tmp179="deep_copy: statement expressions unsupported";_tag_dyneither(_tmp179,sizeof(char),45);});_tmp178;});_tmp177;}));case 37: _LLBE: _tmpF1=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmpF2=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LLBF:
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp17A=_cycalloc(sizeof(*_tmp17A));_tmp17A[0]=({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmp17B;_tmp17B.tag=37;_tmp17B.f1=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmpF1);_tmp17B.f2=_tmpF2;_tmp17B;});_tmp17A;}),e->loc);
goto _LL73;case 38: _LLC0: _tmpF0=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LLC1:
 new_e=Cyc_Absyn_valueof_exp(Cyc_Tcutil_copy_type(_tmpF0),e->loc);
goto _LL73;default: _LLC2: _tmpEE=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmpEF=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LLC3:
 new_e=Cyc_Absyn_asm_exp(_tmpEE,_tmpEF,e->loc);goto _LL73;}_LL73:;}
# 479
if(preserve_types)new_e->topt=e->topt;
return new_e;}
# 483
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*e){
if(e == 0)return 0;else{
return Cyc_Tcutil_deep_copy_exp(preserve_types,e);}}struct _tuple20{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple21{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 496 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
struct Cyc_Absyn_Kind*_tmp17C=ka1;enum Cyc_Absyn_KindQual _tmp17D;enum Cyc_Absyn_AliasQual _tmp17E;_LLCD: _tmp17D=_tmp17C->kind;_tmp17E=_tmp17C->aliasqual;_LLCE:;{
struct Cyc_Absyn_Kind*_tmp17F=ka2;enum Cyc_Absyn_KindQual _tmp180;enum Cyc_Absyn_AliasQual _tmp181;_LLD0: _tmp180=_tmp17F->kind;_tmp181=_tmp17F->aliasqual;_LLD1:;
# 500
if(_tmp17D != _tmp180){
struct _tuple20 _tmp182=({struct _tuple20 _tmp184;_tmp184.f1=_tmp17D;_tmp184.f2=_tmp180;_tmp184;});struct _tuple20 _tmp183=_tmp182;switch(_tmp183.f1){case Cyc_Absyn_BoxKind: switch(_tmp183.f2){case Cyc_Absyn_MemKind: _LLD3: _LLD4:
 goto _LLD6;case Cyc_Absyn_AnyKind: _LLD5: _LLD6:
 goto _LLD8;default: goto _LLD9;}case Cyc_Absyn_MemKind: if(_tmp183.f2 == Cyc_Absyn_AnyKind){_LLD7: _LLD8:
 goto _LLD2;}else{goto _LLD9;}default: _LLD9: _LLDA:
 return 0;}_LLD2:;}
# 509
if(_tmp17E != _tmp181){
struct _tuple21 _tmp185=({struct _tuple21 _tmp187;_tmp187.f1=_tmp17E;_tmp187.f2=_tmp181;_tmp187;});struct _tuple21 _tmp186=_tmp185;switch(_tmp186.f1){case Cyc_Absyn_Aliasable: if(_tmp186.f2 == Cyc_Absyn_Top){_LLDC: _LLDD:
 goto _LLDF;}else{goto _LLE0;}case Cyc_Absyn_Unique: if(_tmp186.f2 == Cyc_Absyn_Top){_LLDE: _LLDF:
 return 1;}else{goto _LLE0;}default: _LLE0: _LLE1:
 return 0;}_LLDB:;}
# 516
return 1;};}
# 519
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp188=Cyc_Absyn_compress_kb(tv->kind);void*_tmp189=_tmp188;struct Cyc_Absyn_Kind*_tmp18A;struct Cyc_Absyn_Kind*_tmp18B;switch(*((int*)_tmp189)){case 0: _LLE3: _tmp18B=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp189)->f1;_LLE4:
 return _tmp18B;case 2: _LLE5: _tmp18A=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp189)->f2;_LLE6:
 return _tmp18A;default: _LLE7: _LLE8:
# 524
 tv->kind=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp18C=_cycalloc(sizeof(*_tmp18C));_tmp18C[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp18D;_tmp18D.tag=2;_tmp18D.f1=0;_tmp18D.f2=def;_tmp18D;});_tmp18C;});
return def;}_LLE2:;}
# 529
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
struct _tuple0 _tmp18E=({struct _tuple0 _tmp1A5;_tmp1A5.f1=Cyc_Absyn_compress_kb(kb1);_tmp1A5.f2=Cyc_Absyn_compress_kb(kb2);_tmp1A5;});struct _tuple0 _tmp18F=_tmp18E;struct Cyc_Core_Opt**_tmp190;void*_tmp191;void*_tmp192;struct Cyc_Core_Opt**_tmp193;struct Cyc_Core_Opt**_tmp194;struct Cyc_Absyn_Kind*_tmp195;struct Cyc_Core_Opt**_tmp196;struct Cyc_Absyn_Kind*_tmp197;struct Cyc_Core_Opt**_tmp198;struct Cyc_Absyn_Kind*_tmp199;struct Cyc_Absyn_Kind*_tmp19A;struct Cyc_Absyn_Kind*_tmp19B;struct Cyc_Core_Opt**_tmp19C;struct Cyc_Absyn_Kind*_tmp19D;struct Cyc_Absyn_Kind*_tmp19E;struct Cyc_Absyn_Kind*_tmp19F;switch(*((int*)_tmp18F.f1)){case 0: switch(*((int*)_tmp18F.f2)){case 0: _LLEA: _tmp19E=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp18F.f1)->f1;_tmp19F=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp18F.f2)->f1;_LLEB:
 return _tmp19E == _tmp19F;case 2: _LLEE: _tmp19B=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp18F.f1)->f1;_tmp19C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp18F.f2)->f1;_tmp19D=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp18F.f2)->f2;_LLEF:
# 538
 if(Cyc_Tcutil_kind_leq(_tmp19B,_tmp19D)){
*_tmp19C=({struct Cyc_Core_Opt*_tmp1A1=_cycalloc(sizeof(*_tmp1A1));_tmp1A1->v=kb1;_tmp1A1;});
return 1;}else{
return 0;}default: goto _LLF4;}case 2: switch(*((int*)_tmp18F.f2)){case 0: _LLEC: _tmp198=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp18F.f1)->f1;_tmp199=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp18F.f1)->f2;_tmp19A=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp18F.f2)->f1;_LLED:
# 533
 if(Cyc_Tcutil_kind_leq(_tmp19A,_tmp199)){
*_tmp198=({struct Cyc_Core_Opt*_tmp1A0=_cycalloc(sizeof(*_tmp1A0));_tmp1A0->v=kb2;_tmp1A0;});
return 1;}else{
return 0;}case 2: _LLF0: _tmp194=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp18F.f1)->f1;_tmp195=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp18F.f1)->f2;_tmp196=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp18F.f2)->f1;_tmp197=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp18F.f2)->f2;_LLF1:
# 543
 if(Cyc_Tcutil_kind_leq(_tmp195,_tmp197)){
*_tmp196=({struct Cyc_Core_Opt*_tmp1A2=_cycalloc(sizeof(*_tmp1A2));_tmp1A2->v=kb1;_tmp1A2;});
return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp197,_tmp195)){
*_tmp194=({struct Cyc_Core_Opt*_tmp1A3=_cycalloc(sizeof(*_tmp1A3));_tmp1A3->v=kb2;_tmp1A3;});
return 1;}else{
return 0;}}default: _LLF4: _tmp192=_tmp18F.f1;_tmp193=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp18F.f2)->f1;_LLF5:
# 552
*_tmp193=({struct Cyc_Core_Opt*_tmp1A4=_cycalloc(sizeof(*_tmp1A4));_tmp1A4->v=_tmp192;_tmp1A4;});
return 1;}default: _LLF2: _tmp190=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp18F.f1)->f1;_tmp191=_tmp18F.f2;_LLF3:
# 550
 _tmp192=_tmp191;_tmp193=_tmp190;goto _LLF5;}_LLE9:;}
# 557
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp1A6=Cyc_Tcutil_compress(t);void*_tmp1A7=_tmp1A6;struct Cyc_Absyn_Tvar*_tmp1A8;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A7)->tag == 2){_LLF7: _tmp1A8=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A7)->f1;_LLF8: {
# 560
void*_tmp1A9=_tmp1A8->kind;
_tmp1A8->kind=kb;
return({struct _tuple16 _tmp1AA;_tmp1AA.f1=_tmp1A8;_tmp1AA.f2=_tmp1A9;_tmp1AA;});}}else{_LLF9: _LLFA:
# 564
({struct Cyc_String_pa_PrintArg_struct _tmp1AD;_tmp1AD.tag=0;_tmp1AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp1AB[1]={& _tmp1AD};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1AC="swap_kind: cannot update the kind of %s";_tag_dyneither(_tmp1AC,sizeof(char),40);}),_tag_dyneither(_tmp1AB,sizeof(void*),1));});});}_LLF6:;}
# 570
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_typ,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 572
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_typ_kind(field_typ);
if(ts != 0  && (k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak)){
# 576
struct _RegionHandle _tmp1AE=_new_region("temp");struct _RegionHandle*temp=& _tmp1AE;_push_region(temp);
{struct Cyc_List_List*_tmp1AF=0;
# 579
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp1B0=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp1B1=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;
struct Cyc_Absyn_Kind*_tmp1B2=Cyc_Tcutil_tvar_kind(_tmp1B0,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp1B3=_tmp1B2;switch(((struct Cyc_Absyn_Kind*)_tmp1B3)->kind){case Cyc_Absyn_IntKind: _LLFC: _LLFD:
 goto _LLFF;case Cyc_Absyn_AnyKind: _LLFE: _LLFF:
# 585
 _tmp1AF=({struct Cyc_List_List*_tmp1B4=_region_malloc(temp,sizeof(*_tmp1B4));_tmp1B4->hd=({struct _tuple16*_tmp1B5=_region_malloc(temp,sizeof(*_tmp1B5));_tmp1B5->f1=_tmp1B0;_tmp1B5->f2=_tmp1B1;_tmp1B5;});_tmp1B4->tl=_tmp1AF;_tmp1B4;});goto _LLFB;default: _LL100: _LL101:
 goto _LLFB;}_LLFB:;}
# 589
if(_tmp1AF != 0){
field_typ=Cyc_Tcutil_rsubstitute(temp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1AF),field_typ);
k=Cyc_Tcutil_typ_kind(field_typ);}}
# 577
;_pop_region(temp);}
# 594
return k;}
# 601
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t){
void*_tmp1B6=Cyc_Tcutil_compress(t);void*_tmp1B7=_tmp1B6;struct Cyc_Absyn_Typedefdecl*_tmp1B8;struct Cyc_Absyn_Exp*_tmp1B9;struct Cyc_Absyn_Kind*_tmp1BA;struct Cyc_Absyn_PtrInfo _tmp1BB;enum Cyc_Absyn_AggrKind _tmp1BC;struct Cyc_List_List*_tmp1BD;struct Cyc_Absyn_AggrdeclImpl*_tmp1BE;int _tmp1BF;struct Cyc_List_List*_tmp1C0;struct Cyc_Absyn_Datatypedecl*_tmp1C1;struct Cyc_Absyn_Datatypefield*_tmp1C2;enum Cyc_Absyn_Size_of _tmp1C3;struct Cyc_Absyn_Tvar*_tmp1C4;struct Cyc_Core_Opt*_tmp1C5;switch(*((int*)_tmp1B7)){case 1: _LL103: _tmp1C5=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B7)->f1;_LL104:
 return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp1C5))->v;case 2: _LL105: _tmp1C4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1B7)->f1;_LL106:
 return Cyc_Tcutil_tvar_kind(_tmp1C4,& Cyc_Tcutil_bk);case 0: _LL107: _LL108:
 return& Cyc_Tcutil_mk;case 6: _LL109: _tmp1C3=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1B7)->f2;_LL10A:
# 607
 return(_tmp1C3 == Cyc_Absyn_Int_sz  || _tmp1C3 == Cyc_Absyn_Long_sz)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;case 7: _LL10B: _LL10C:
# 609
 return& Cyc_Tcutil_mk;case 9: _LL10D: _LL10E:
 return& Cyc_Tcutil_ak;case 16: _LL10F: _LL110:
 goto _LL112;case 13: _LL111: _LL112:
 goto _LL114;case 14: _LL113: _LL114:
 goto _LL116;case 15: _LL115: _LL116:
 return& Cyc_Tcutil_bk;case 21: _LL117: _LL118:
 return& Cyc_Tcutil_urk;case 20: _LL119: _LL11A:
 return& Cyc_Tcutil_rk;case 22: _LL11B: _LL11C:
 return& Cyc_Tcutil_trk;case 3: _LL11D: _LL11E:
# 620
 return& Cyc_Tcutil_ak;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1B7)->f1).field_info).KnownDatatypefield).tag == 2){_LL11F: _tmp1C1=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1B7)->f1).field_info).KnownDatatypefield).val).f1;_tmp1C2=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1B7)->f1).field_info).KnownDatatypefield).val).f2;_LL120:
# 622
 return& Cyc_Tcutil_mk;}else{_LL121: _LL122:
# 624
({void*_tmp1C6=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1C7="typ_kind: Unresolved DatatypeFieldType";_tag_dyneither(_tmp1C7,sizeof(char),39);}),_tag_dyneither(_tmp1C6,sizeof(void*),0));});}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B7)->f1).aggr_info).UnknownAggr).tag == 1){_LL123: _LL124:
# 627
 return& Cyc_Tcutil_ak;}else{_LL125: _tmp1BC=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B7)->f1).aggr_info).KnownAggr).val)->kind;_tmp1BD=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B7)->f1).aggr_info).KnownAggr).val)->tvs;_tmp1BE=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B7)->f1).aggr_info).KnownAggr).val)->impl;_tmp1BF=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B7)->f1).aggr_info).KnownAggr).val)->expected_mem_kind;_tmp1C0=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B7)->f1).targs;_LL126:
# 629
 if(_tmp1BE == 0){
if(_tmp1BF)
return& Cyc_Tcutil_mk;else{
# 633
return& Cyc_Tcutil_ak;}}{
# 635
struct Cyc_List_List*_tmp1C8=_tmp1BE->fields;
if(_tmp1C8 == 0)return& Cyc_Tcutil_mk;
# 638
if(_tmp1BC == Cyc_Absyn_StructA){
for(0;_tmp1C8->tl != 0;_tmp1C8=_tmp1C8->tl){;}{
void*_tmp1C9=((struct Cyc_Absyn_Aggrfield*)_tmp1C8->hd)->type;
struct Cyc_Absyn_Kind*_tmp1CA=Cyc_Tcutil_field_kind(_tmp1C9,_tmp1C0,_tmp1BD);
if(_tmp1CA == & Cyc_Tcutil_ak  || _tmp1CA == & Cyc_Tcutil_tak)return _tmp1CA;};}else{
# 646
for(0;_tmp1C8 != 0;_tmp1C8=_tmp1C8->tl){
void*_tmp1CB=((struct Cyc_Absyn_Aggrfield*)_tmp1C8->hd)->type;
struct Cyc_Absyn_Kind*_tmp1CC=Cyc_Tcutil_field_kind(_tmp1CB,_tmp1C0,_tmp1BD);
if(_tmp1CC == & Cyc_Tcutil_ak  || _tmp1CC == & Cyc_Tcutil_tak)return _tmp1CC;}}
# 652
return& Cyc_Tcutil_mk;};}case 12: _LL127: _LL128:
 return& Cyc_Tcutil_mk;case 5: _LL129: _tmp1BB=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B7)->f1;_LL12A: {
# 655
void*_tmp1CD=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,(_tmp1BB.ptr_atts).bounds);void*_tmp1CE=_tmp1CD;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1CE)->tag == 0){_LL146: _LL147: {
# 657
enum Cyc_Absyn_AliasQual _tmp1CF=(Cyc_Tcutil_typ_kind((_tmp1BB.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp1D0=_tmp1CF;switch(_tmp1D0){case Cyc_Absyn_Aliasable: _LL14B: _LL14C:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL14D: _LL14E:
 return& Cyc_Tcutil_umk;default: _LL14F: _LL150:
 return& Cyc_Tcutil_tmk;}_LL14A:;}}else{_LL148: _LL149: {
# 663
enum Cyc_Absyn_AliasQual _tmp1D1=(Cyc_Tcutil_typ_kind((_tmp1BB.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp1D2=_tmp1D1;switch(_tmp1D2){case Cyc_Absyn_Aliasable: _LL152: _LL153:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL154: _LL155:
 return& Cyc_Tcutil_ubk;default: _LL156: _LL157:
 return& Cyc_Tcutil_tbk;}_LL151:;}}_LL145:;}case 18: _LL12B: _LL12C:
# 669
 return& Cyc_Tcutil_ik;case 28: _LL12D: _tmp1BA=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp1B7)->f2;_LL12E:
 return _tmp1BA;case 27: _LL12F: _LL130:
# 674
 return& Cyc_Tcutil_ak;case 19: _LL131: _LL132:
 return& Cyc_Tcutil_bk;case 8: _LL133: _tmp1B9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1B7)->f1).num_elts;_LL134:
# 677
 if(_tmp1B9 == 0  || Cyc_Tcutil_is_const_exp(_tmp1B9))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;case 10: _LL135: _LL136:
 return& Cyc_Tcutil_mk;case 17: _LL137: _tmp1B8=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1B7)->f3;_LL138:
# 681
 if(_tmp1B8 == 0  || _tmp1B8->kind == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1D5;_tmp1D5.tag=0;_tmp1D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp1D3[1]={& _tmp1D5};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1D4="typ_kind: typedef found: %s";_tag_dyneither(_tmp1D4,sizeof(char),28);}),_tag_dyneither(_tmp1D3,sizeof(void*),1));});});
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp1B8->kind))->v;case 26: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1B7)->f1)->r)){case 0: _LL139: _LL13A:
 return& Cyc_Tcutil_ak;case 1: _LL13B: _LL13C:
 return& Cyc_Tcutil_bk;default: _LL13D: _LL13E:
 return& Cyc_Tcutil_ak;}case 23: _LL13F: _LL140:
 goto _LL142;case 24: _LL141: _LL142:
 goto _LL144;default: _LL143: _LL144:
 return& Cyc_Tcutil_ek;}_LL102:;}
# 693
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2  || k1->kind == k2->kind  && k1->aliasqual == k2->aliasqual;}
# 698
int Cyc_Tcutil_unify(void*t1,void*t2){
struct _handler_cons _tmp1D6;_push_handler(& _tmp1D6);{int _tmp1D8=0;if(setjmp(_tmp1D6.handler))_tmp1D8=1;if(!_tmp1D8){
Cyc_Tcutil_unify_it(t1,t2);{
int _tmp1D9=1;_npop_handler(0);return _tmp1D9;};
# 700
;_pop_handler();}else{void*_tmp1D7=(void*)_exn_thrown;void*_tmp1DA=_tmp1D7;void*_tmp1DB;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp1DA)->tag == Cyc_Tcutil_Unify){_LL159: _LL15A:
# 702
 return 0;}else{_LL15B: _tmp1DB=_tmp1DA;_LL15C:(int)_rethrow(_tmp1DB);}_LL158:;}};}
# 707
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
t=Cyc_Tcutil_compress(t);{
void*_tmp1DC=t;struct Cyc_List_List*_tmp1DD;void*_tmp1DE;void*_tmp1DF;void*_tmp1E0;void*_tmp1E1;struct Cyc_List_List*_tmp1E2;struct Cyc_List_List*_tmp1E3;struct Cyc_List_List*_tmp1E4;struct Cyc_List_List*_tmp1E5;struct Cyc_List_List*_tmp1E6;struct Cyc_List_List*_tmp1E7;struct Cyc_List_List*_tmp1E8;void*_tmp1E9;struct Cyc_Absyn_Tqual _tmp1EA;void*_tmp1EB;struct Cyc_List_List*_tmp1EC;int _tmp1ED;struct Cyc_Absyn_VarargInfo*_tmp1EE;struct Cyc_List_List*_tmp1EF;struct Cyc_List_List*_tmp1F0;struct Cyc_Absyn_Exp*_tmp1F1;struct Cyc_List_List*_tmp1F2;struct Cyc_Absyn_Exp*_tmp1F3;struct Cyc_List_List*_tmp1F4;void*_tmp1F5;struct Cyc_Absyn_PtrInfo _tmp1F6;void*_tmp1F7;struct Cyc_Core_Opt**_tmp1F8;struct Cyc_Absyn_Tvar*_tmp1F9;switch(*((int*)_tmp1DC)){case 2: _LL15E: _tmp1F9=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1DC)->f1;_LL15F:
# 712
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp1F9)){
Cyc_Tcutil_failure_reason=({const char*_tmp1FA="(type variable would escape scope)";_tag_dyneither(_tmp1FA,sizeof(char),35);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 716
goto _LL15D;case 1: _LL160: _tmp1F7=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1DC)->f2;_tmp1F8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1DC)->f4;_LL161:
# 718
 if(t == evar){
Cyc_Tcutil_failure_reason=({const char*_tmp1FB="(occurs check)";_tag_dyneither(_tmp1FB,sizeof(char),15);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{
# 722
if(_tmp1F7 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp1F7);else{
# 725
int problem=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp1F8))->v;for(0;s != 0;s=s->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}
# 732
if(problem){
struct Cyc_List_List*_tmp1FC=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp1F8))->v;for(0;s != 0;s=s->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd))
_tmp1FC=({struct Cyc_List_List*_tmp1FD=_cycalloc(sizeof(*_tmp1FD));_tmp1FD->hd=(struct Cyc_Absyn_Tvar*)s->hd;_tmp1FD->tl=_tmp1FC;_tmp1FD;});}}
# 738
*_tmp1F8=({struct Cyc_Core_Opt*_tmp1FE=_cycalloc(sizeof(*_tmp1FE));_tmp1FE->v=_tmp1FC;_tmp1FE;});}}}
# 741
goto _LL15D;case 5: _LL162: _tmp1F6=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1DC)->f1;_LL163:
# 744
 Cyc_Tcutil_occurs(evar,r,env,_tmp1F6.elt_typ);
Cyc_Tcutil_occurs(evar,r,env,(_tmp1F6.ptr_atts).rgn);
goto _LL15D;case 8: _LL164: _tmp1F5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1DC)->f1).elt_type;_LL165:
# 748
 Cyc_Tcutil_occurs(evar,r,env,_tmp1F5);goto _LL15D;case 9: _LL166: _tmp1E8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DC)->f1).tvars;_tmp1E9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DC)->f1).effect;_tmp1EA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DC)->f1).ret_tqual;_tmp1EB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DC)->f1).ret_typ;_tmp1EC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DC)->f1).args;_tmp1ED=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DC)->f1).c_varargs;_tmp1EE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DC)->f1).cyc_varargs;_tmp1EF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DC)->f1).rgn_po;_tmp1F0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DC)->f1).attributes;_tmp1F1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DC)->f1).requires_clause;_tmp1F2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DC)->f1).requires_relns;_tmp1F3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DC)->f1).ensures_clause;_tmp1F4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DC)->f1).ensures_relns;_LL167:
# 751
 env=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp1E8,env);
if(_tmp1E9 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp1E9);
Cyc_Tcutil_occurs(evar,r,env,_tmp1EB);
for(0;_tmp1EC != 0;_tmp1EC=_tmp1EC->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple10*)_tmp1EC->hd)).f3);}
if(_tmp1EE != 0)
Cyc_Tcutil_occurs(evar,r,env,_tmp1EE->type);
for(0;_tmp1EF != 0;_tmp1EF=_tmp1EF->tl){
struct _tuple0*_tmp1FF=(struct _tuple0*)_tmp1EF->hd;struct _tuple0*_tmp200=_tmp1FF;void*_tmp201;void*_tmp202;_LL181: _tmp201=_tmp200->f1;_tmp202=_tmp200->f2;_LL182:;
Cyc_Tcutil_occurs(evar,r,env,_tmp201);
Cyc_Tcutil_occurs(evar,r,env,_tmp202);}
# 763
goto _LL15D;case 10: _LL168: _tmp1E7=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1DC)->f1;_LL169:
# 765
 for(0;_tmp1E7 != 0;_tmp1E7=_tmp1E7->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple12*)_tmp1E7->hd)).f2);}
goto _LL15D;case 3: _LL16A: _tmp1E6=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1DC)->f1).targs;_LL16B:
# 769
 Cyc_Tcutil_occurslist(evar,r,env,_tmp1E6);goto _LL15D;case 17: _LL16C: _tmp1E5=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1DC)->f2;_LL16D:
 _tmp1E4=_tmp1E5;goto _LL16F;case 4: _LL16E: _tmp1E4=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1DC)->f1).targs;_LL16F:
 _tmp1E3=_tmp1E4;goto _LL171;case 11: _LL170: _tmp1E3=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1DC)->f1).targs;_LL171:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp1E3);goto _LL15D;case 12: _LL172: _tmp1E2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1DC)->f2;_LL173:
# 775
 for(0;_tmp1E2 != 0;_tmp1E2=_tmp1E2->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp1E2->hd)->type);}
goto _LL15D;case 19: _LL174: _tmp1E1=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp1DC)->f1;_LL175:
 _tmp1E0=_tmp1E1;goto _LL177;case 23: _LL176: _tmp1E0=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp1DC)->f1;_LL177:
 _tmp1DF=_tmp1E0;goto _LL179;case 15: _LL178: _tmp1DF=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1DC)->f1;_LL179:
 _tmp1DE=_tmp1DF;goto _LL17B;case 25: _LL17A: _tmp1DE=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp1DC)->f1;_LL17B:
 Cyc_Tcutil_occurs(evar,r,env,_tmp1DE);goto _LL15D;case 24: _LL17C: _tmp1DD=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp1DC)->f1;_LL17D:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp1DD);goto _LL15D;default: _LL17E: _LL17F:
# 785
 goto _LL15D;}_LL15D:;};}
# 788
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
# 790
for(0;ts != 0;ts=ts->tl){
Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}
# 795
static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){
Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0  || t2 != 0)
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 803
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){
if(tq1.print_const  && !tq1.real_const)
({void*_tmp203=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp204="tq1 real_const not set.";_tag_dyneither(_tmp204,sizeof(char),24);}),_tag_dyneither(_tmp203,sizeof(void*),0));});
if(tq2.print_const  && !tq2.real_const)
({void*_tmp205=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp206="tq2 real_const not set.";_tag_dyneither(_tmp206,sizeof(char),24);}),_tag_dyneither(_tmp205,sizeof(void*),0));});
# 809
if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
# 812
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;
Cyc_Tcutil_failure_reason=({const char*_tmp207="(qualifiers don't match)";_tag_dyneither(_tmp207,sizeof(char),25);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 820
Cyc_Tcutil_tq1_const=0;
Cyc_Tcutil_tq2_const=0;}
# 824
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
return(tq1.real_const == tq2.real_const  && tq1.q_volatile == tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}
# 831
static void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason){
# 833
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return;{
union Cyc_Absyn_Constraint*_tmp208=x;void*_tmp209;switch((((union Cyc_Absyn_Constraint*)_tmp208)->Eq_constr).tag){case 3: _LL184: _LL185:
# 838
*x=({union Cyc_Absyn_Constraint _tmp20A;(_tmp20A.Forward_constr).val=y;(_tmp20A.Forward_constr).tag=2;_tmp20A;});return;case 1: _LL186: _tmp209=(void*)(_tmp208->Eq_constr).val;_LL187: {
# 840
union Cyc_Absyn_Constraint*_tmp20B=y;void*_tmp20C;switch((((union Cyc_Absyn_Constraint*)_tmp20B)->Eq_constr).tag){case 3: _LL18B: _LL18C:
*y=*x;return;case 1: _LL18D: _tmp20C=(void*)(_tmp20B->Eq_constr).val;_LL18E:
# 843
 if(cmp(_tmp209,_tmp20C)!= 0){
Cyc_Tcutil_failure_reason=reason;
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 847
return;default: _LL18F: _LL190:
({void*_tmp20D=0;Cyc_Tcutil_impos(({const char*_tmp20E="unify_conref: forward after compress(2)";_tag_dyneither(_tmp20E,sizeof(char),40);}),_tag_dyneither(_tmp20D,sizeof(void*),0));});}_LL18A:;}default: _LL188: _LL189:
# 850
({void*_tmp20F=0;Cyc_Tcutil_impos(({const char*_tmp210="unify_conref: forward after compress";_tag_dyneither(_tmp210,sizeof(char),37);}),_tag_dyneither(_tmp20F,sizeof(void*),0));});}_LL183:;};}
# 854
static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
struct _handler_cons _tmp211;_push_handler(& _tmp211);{int _tmp213=0;if(setjmp(_tmp211.handler))_tmp213=1;if(!_tmp213){
Cyc_Tcutil_unify_it_conrefs(cmp,x,y,_tag_dyneither(0,0,0));{
int _tmp214=1;_npop_handler(0);return _tmp214;};
# 856
;_pop_handler();}else{void*_tmp212=(void*)_exn_thrown;void*_tmp215=_tmp212;void*_tmp216;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp215)->tag == Cyc_Tcutil_Unify){_LL192: _LL193:
# 858
 return 0;}else{_LL194: _tmp216=_tmp215;_LL195:(int)_rethrow(_tmp216);}_LL191:;}};}
# 861
static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmp217=({struct _tuple0 _tmp21B;_tmp21B.f1=b1;_tmp21B.f2=b2;_tmp21B;});struct _tuple0 _tmp218=_tmp217;struct Cyc_Absyn_Exp*_tmp219;struct Cyc_Absyn_Exp*_tmp21A;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp218.f1)->tag == 0){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp218.f2)->tag == 0){_LL197: _LL198:
 return 0;}else{_LL199: _LL19A:
 return - 1;}}else{if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp218.f2)->tag == 0){_LL19B: _LL19C:
 return 1;}else{_LL19D: _tmp219=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp218.f1)->f1;_tmp21A=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp218.f2)->f1;_LL19E:
# 867
 return Cyc_Evexp_const_exp_cmp(_tmp219,_tmp21A);}}_LL196:;}
# 871
static void Cyc_Tcutil_unify_cmp_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
{struct _tuple0 _tmp21C=({struct _tuple0 _tmp22A;_tmp22A.f1=e1->r;_tmp22A.f2=e2->r;_tmp22A;});struct _tuple0 _tmp21D=_tmp21C;void*_tmp21E;void*_tmp21F;struct Cyc_Absyn_Exp*_tmp220;struct Cyc_Absyn_Exp*_tmp221;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp21D.f1)->tag == 13){_LL1A0: _tmp221=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp21D.f1)->f2;_LL1A1:
# 877
 Cyc_Tcutil_unify_cmp_exp(_tmp221,e2);
return;}else{if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp21D.f2)->tag == 13){_LL1A2: _tmp220=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp21D.f2)->f2;_LL1A3:
# 880
 Cyc_Tcutil_unify_cmp_exp(e1,_tmp220);
return;}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp21D.f1)->tag == 38){_LL1A4: _tmp21F=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp21D.f1)->f1;_LL1A5:
# 883
{void*_tmp222=Cyc_Tcutil_compress(_tmp21F);void*_tmp223=_tmp222;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp223)->tag == 1){_LL1AB: _LL1AC:
# 885
 Cyc_Tcutil_unify_it(_tmp21F,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp224=_cycalloc(sizeof(*_tmp224));_tmp224[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp225;_tmp225.tag=18;_tmp225.f1=e2;_tmp225;});_tmp224;}));
return;}else{_LL1AD: _LL1AE:
# 888
 goto _LL1AA;}_LL1AA:;}
# 890
goto _LL19F;}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp21D.f2)->tag == 38){_LL1A6: _tmp21E=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp21D.f2)->f1;_LL1A7:
# 892
{void*_tmp226=Cyc_Tcutil_compress(_tmp21E);void*_tmp227=_tmp226;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp227)->tag == 1){_LL1B0: _LL1B1:
# 894
 Cyc_Tcutil_unify_it(_tmp21E,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp228=_cycalloc(sizeof(*_tmp228));_tmp228[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp229;_tmp229.tag=18;_tmp229.f1=e1;_tmp229;});_tmp228;}));
return;}else{_LL1B2: _LL1B3:
# 897
 goto _LL1AF;}_LL1AF:;}
# 899
goto _LL19F;}else{_LL1A8: _LL1A9:
 goto _LL19F;}}}}_LL19F:;}
# 902
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 905
static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){
struct _tuple0 _tmp22B=({struct _tuple0 _tmp235;_tmp235.f1=b1;_tmp235.f2=b2;_tmp235;});struct _tuple0 _tmp22C=_tmp22B;struct Cyc_Absyn_Exp*_tmp22D;struct Cyc_Absyn_Exp*_tmp22E;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp22C.f1)->tag == 0){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp22C.f2)->tag == 0){_LL1B5: _LL1B6:
 return 0;}else{_LL1B7: _LL1B8:
 return - 1;}}else{if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp22C.f2)->tag == 0){_LL1B9: _LL1BA:
 return 1;}else{_LL1BB: _tmp22D=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp22C.f1)->f1;_tmp22E=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp22C.f2)->f1;_LL1BC: {
# 911
int r=Cyc_Evexp_const_exp_cmp(_tmp22D,_tmp22E);
if(r == 0)return 0;{
struct _handler_cons _tmp22F;_push_handler(& _tmp22F);{int _tmp231=0;if(setjmp(_tmp22F.handler))_tmp231=1;if(!_tmp231){
Cyc_Tcutil_unify_cmp_exp(_tmp22D,_tmp22E);{
int _tmp232=0;_npop_handler(0);return _tmp232;};
# 914
;_pop_handler();}else{void*_tmp230=(void*)_exn_thrown;void*_tmp233=_tmp230;void*_tmp234;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp233)->tag == Cyc_Tcutil_Unify){_LL1BE: _LL1BF:
# 917
 return r;}else{_LL1C0: _tmp234=_tmp233;_LL1C1:(int)_rethrow(_tmp234);}_LL1BD:;}};};}}}_LL1B4:;}
# 922
static int Cyc_Tcutil_attribute_case_number(void*att){
void*_tmp236=att;switch(*((int*)_tmp236)){case 0: _LL1C3: _LL1C4:
 return 0;case 1: _LL1C5: _LL1C6:
 return 1;case 2: _LL1C7: _LL1C8:
 return 2;case 3: _LL1C9: _LL1CA:
 return 3;case 4: _LL1CB: _LL1CC:
 return 4;case 5: _LL1CD: _LL1CE:
 return 5;case 6: _LL1CF: _LL1D0:
 return 6;case 7: _LL1D1: _LL1D2:
 return 7;case 8: _LL1D3: _LL1D4:
 return 8;case 9: _LL1D5: _LL1D6:
 return 9;case 10: _LL1D7: _LL1D8:
 return 10;case 11: _LL1D9: _LL1DA:
 return 11;case 12: _LL1DB: _LL1DC:
 return 12;case 13: _LL1DD: _LL1DE:
 return 13;case 14: _LL1DF: _LL1E0:
 return 14;case 15: _LL1E1: _LL1E2:
 return 15;case 16: _LL1E3: _LL1E4:
 return 16;case 17: _LL1E5: _LL1E6:
 return 17;case 18: _LL1E7: _LL1E8:
 return 18;case 19: _LL1E9: _LL1EA:
 return 19;case 20: _LL1EB: _LL1EC:
 return 20;default: _LL1ED: _LL1EE:
 return 21;}_LL1C2:;}
# 949
static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple0 _tmp237=({struct _tuple0 _tmp249;_tmp249.f1=att1;_tmp249.f2=att2;_tmp249;});struct _tuple0 _tmp238=_tmp237;enum Cyc_Absyn_Format_Type _tmp239;int _tmp23A;int _tmp23B;enum Cyc_Absyn_Format_Type _tmp23C;int _tmp23D;int _tmp23E;struct _dyneither_ptr _tmp23F;struct _dyneither_ptr _tmp240;struct Cyc_Absyn_Exp*_tmp241;struct Cyc_Absyn_Exp*_tmp242;int _tmp243;int _tmp244;int _tmp245;int _tmp246;switch(*((int*)_tmp238.f1)){case 0: if(((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp238.f2)->tag == 0){_LL1F0: _tmp245=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp238.f1)->f1;_tmp246=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp238.f2)->f1;_LL1F1:
 _tmp243=_tmp245;_tmp244=_tmp246;goto _LL1F3;}else{goto _LL1FA;}case 20: if(((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp238.f2)->tag == 20){_LL1F2: _tmp243=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp238.f1)->f1;_tmp244=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp238.f2)->f1;_LL1F3:
# 953
 return Cyc_Core_intcmp(_tmp243,_tmp244);}else{goto _LL1FA;}case 6: if(((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp238.f2)->tag == 6){_LL1F4: _tmp241=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp238.f1)->f1;_tmp242=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp238.f2)->f1;_LL1F5:
# 955
 if(_tmp241 == _tmp242)return 0;
if(_tmp241 == 0)return - 1;
if(_tmp242 == 0)return 1;
return Cyc_Evexp_const_exp_cmp(_tmp241,_tmp242);}else{goto _LL1FA;}case 8: if(((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp238.f2)->tag == 8){_LL1F6: _tmp23F=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp238.f1)->f1;_tmp240=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp238.f2)->f1;_LL1F7:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp23F,(struct _dyneither_ptr)_tmp240);}else{goto _LL1FA;}case 19: if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp238.f2)->tag == 19){_LL1F8: _tmp239=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp238.f1)->f1;_tmp23A=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp238.f1)->f2;_tmp23B=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp238.f1)->f3;_tmp23C=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp238.f2)->f1;_tmp23D=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp238.f2)->f2;_tmp23E=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp238.f2)->f3;_LL1F9: {
# 961
int _tmp247=Cyc_Core_intcmp((int)((unsigned int)_tmp239),(int)((unsigned int)_tmp23C));
if(_tmp247 != 0)return _tmp247;{
int _tmp248=Cyc_Core_intcmp(_tmp23A,_tmp23D);
if(_tmp248 != 0)return _tmp248;
return Cyc_Core_intcmp(_tmp23B,_tmp23E);};}}else{goto _LL1FA;}default: _LL1FA: _LL1FB:
# 967
 return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(att2));}_LL1EF:;}
# 971
static int Cyc_Tcutil_equal_att(void*a1,void*a2){
return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}
# 975
int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}
return 1;}
# 984
static void*Cyc_Tcutil_rgns_of(void*t);
# 986
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 990
static struct _tuple16*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_tmp24A=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp24B=_tmp24A;switch(((struct Cyc_Absyn_Kind*)_tmp24B)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp24B)->aliasqual){case Cyc_Absyn_Unique: _LL1FD: _LL1FE:
 t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL1FC;case Cyc_Absyn_Aliasable: _LL1FF: _LL200:
 t=(void*)& Cyc_Absyn_HeapRgn_val;goto _LL1FC;default: goto _LL205;}case Cyc_Absyn_EffKind: _LL201: _LL202:
 t=Cyc_Absyn_empty_effect;goto _LL1FC;case Cyc_Absyn_IntKind: _LL203: _LL204:
 t=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp24C=_cycalloc(sizeof(*_tmp24C));_tmp24C[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp24D;_tmp24D.tag=18;_tmp24D.f1=Cyc_Absyn_uint_exp(0,0);_tmp24D;});_tmp24C;});goto _LL1FC;default: _LL205: _LL206:
 t=Cyc_Absyn_sint_typ;goto _LL1FC;}_LL1FC:;}
# 999
return({struct _tuple16*_tmp24E=_cycalloc(sizeof(*_tmp24E));_tmp24E->f1=tv;_tmp24E->f2=t;_tmp24E;});}
# 1006
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp24F=Cyc_Tcutil_compress(t);void*_tmp250=_tmp24F;struct Cyc_List_List*_tmp251;void*_tmp252;struct Cyc_List_List*_tmp253;void*_tmp254;struct Cyc_Absyn_Tqual _tmp255;void*_tmp256;struct Cyc_List_List*_tmp257;struct Cyc_Absyn_VarargInfo*_tmp258;struct Cyc_List_List*_tmp259;struct Cyc_List_List*_tmp25A;struct Cyc_List_List*_tmp25B;struct Cyc_List_List*_tmp25C;struct Cyc_List_List*_tmp25D;void*_tmp25E;void*_tmp25F;void*_tmp260;struct Cyc_List_List*_tmp261;void*_tmp262;void*_tmp263;void*_tmp264;switch(*((int*)_tmp250)){case 0: _LL208: _LL209:
 goto _LL20B;case 7: _LL20A: _LL20B:
 goto _LL20D;case 13: _LL20C: _LL20D:
 goto _LL20F;case 14: _LL20E: _LL20F:
 goto _LL211;case 18: _LL210: _LL211:
 goto _LL213;case 27: _LL212: _LL213:
 goto _LL215;case 28: _LL214: _LL215:
 goto _LL217;case 6: _LL216: _LL217:
 return Cyc_Absyn_empty_effect;case 1: _LL218: _LL219:
 goto _LL21B;case 2: _LL21A: _LL21B: {
# 1018
struct Cyc_Absyn_Kind*_tmp265=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp266=_tmp265;switch(((struct Cyc_Absyn_Kind*)_tmp266)->kind){case Cyc_Absyn_RgnKind: _LL243: _LL244:
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp267=_cycalloc(sizeof(*_tmp267));_tmp267[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp268;_tmp268.tag=23;_tmp268.f1=t;_tmp268;});_tmp267;});case Cyc_Absyn_EffKind: _LL245: _LL246:
 return t;case Cyc_Absyn_IntKind: _LL247: _LL248:
 return Cyc_Absyn_empty_effect;default: _LL249: _LL24A:
 return(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp269=_cycalloc(sizeof(*_tmp269));_tmp269[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp26A;_tmp26A.tag=25;_tmp26A.f1=t;_tmp26A;});_tmp269;});}_LL242:;}case 15: _LL21C: _tmp264=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp250)->f1;_LL21D:
# 1024
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp26B=_cycalloc(sizeof(*_tmp26B));_tmp26B[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp26C;_tmp26C.tag=23;_tmp26C.f1=_tmp264;_tmp26C;});_tmp26B;});case 16: _LL21E: _tmp262=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp250)->f1;_tmp263=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp250)->f2;_LL21F:
# 1028
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp26D=_cycalloc(sizeof(*_tmp26D));_tmp26D[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp26E;_tmp26E.tag=23;_tmp26E.f1=_tmp263;_tmp26E;});_tmp26D;});case 3: _LL220: _tmp261=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp250)->f1).targs;_LL221: {
# 1030
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp261);
return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp26F=_cycalloc(sizeof(*_tmp26F));_tmp26F[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp270;_tmp270.tag=24;_tmp270.f1=ts;_tmp270;});_tmp26F;}));}case 5: _LL222: _tmp25F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp250)->f1).elt_typ;_tmp260=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp250)->f1).ptr_atts).rgn;_LL223:
# 1034
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp271=_cycalloc(sizeof(*_tmp271));_tmp271[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp272;_tmp272.tag=24;_tmp272.f1=({void*_tmp273[2];_tmp273[1]=Cyc_Tcutil_rgns_of(_tmp25F);_tmp273[0]=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp274=_cycalloc(sizeof(*_tmp274));_tmp274[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp275;_tmp275.tag=23;_tmp275.f1=_tmp260;_tmp275;});_tmp274;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp273,sizeof(void*),2));});_tmp272;});_tmp271;}));case 8: _LL224: _tmp25E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp250)->f1).elt_type;_LL225:
# 1036
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp25E));case 10: _LL226: _tmp25D=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp250)->f1;_LL227: {
# 1038
struct Cyc_List_List*_tmp276=0;
for(0;_tmp25D != 0;_tmp25D=_tmp25D->tl){
_tmp276=({struct Cyc_List_List*_tmp277=_cycalloc(sizeof(*_tmp277));_tmp277->hd=(*((struct _tuple12*)_tmp25D->hd)).f2;_tmp277->tl=_tmp276;_tmp277;});}
_tmp25C=_tmp276;goto _LL229;}case 4: _LL228: _tmp25C=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp250)->f1).targs;_LL229:
 _tmp25B=_tmp25C;goto _LL22B;case 11: _LL22A: _tmp25B=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp250)->f1).targs;_LL22B:
# 1044
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp278=_cycalloc(sizeof(*_tmp278));_tmp278[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp279;_tmp279.tag=24;_tmp279.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp25B);_tmp279;});_tmp278;}));case 12: _LL22C: _tmp25A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp250)->f2;_LL22D:
# 1046
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp27A=_cycalloc(sizeof(*_tmp27A));_tmp27A[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp27B;_tmp27B.tag=24;_tmp27B.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp25A);_tmp27B;});_tmp27A;}));case 19: _LL22E: _LL22F:
 return Cyc_Absyn_empty_effect;case 9: _LL230: _tmp253=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp250)->f1).tvars;_tmp254=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp250)->f1).effect;_tmp255=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp250)->f1).ret_tqual;_tmp256=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp250)->f1).ret_typ;_tmp257=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp250)->f1).args;_tmp258=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp250)->f1).cyc_varargs;_tmp259=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp250)->f1).rgn_po;_LL231: {
# 1056
void*_tmp27C=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp253),(void*)_check_null(_tmp254));
return Cyc_Tcutil_normalize_effect(_tmp27C);}case 21: _LL232: _LL233:
 goto _LL235;case 22: _LL234: _LL235:
 goto _LL237;case 20: _LL236: _LL237:
 return Cyc_Absyn_empty_effect;case 23: _LL238: _LL239:
 goto _LL23B;case 24: _LL23A: _LL23B:
 return t;case 25: _LL23C: _tmp252=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp250)->f1;_LL23D:
 return Cyc_Tcutil_rgns_of(_tmp252);case 17: _LL23E: _tmp251=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp250)->f2;_LL23F:
# 1065
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp27D=_cycalloc(sizeof(*_tmp27D));_tmp27D[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp27E;_tmp27E.tag=24;_tmp27E.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp251);_tmp27E;});_tmp27D;}));default: _LL240: _LL241:
({void*_tmp27F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp280="typedecl in rgns_of";_tag_dyneither(_tmp280,sizeof(char),20);}),_tag_dyneither(_tmp27F,sizeof(void*),0));});}_LL207:;}
# 1073
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp281=e;void*_tmp282;struct Cyc_List_List**_tmp283;switch(*((int*)_tmp281)){case 24: _LL24C: _tmp283=(struct Cyc_List_List**)&((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp281)->f1;_LL24D: {
# 1077
int redo_join=0;
{struct Cyc_List_List*effs=*_tmp283;for(0;effs != 0;effs=effs->tl){
void*_tmp284=(void*)effs->hd;
effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp284));{
void*_tmp285=(void*)effs->hd;void*_tmp286=_tmp285;switch(*((int*)_tmp286)){case 24: _LL253: _LL254:
 goto _LL256;case 23: switch(*((int*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp286)->f1)){case 20: _LL255: _LL256:
 redo_join=1;goto _LL252;case 22: _LL257: _LL258:
 redo_join=1;goto _LL252;case 21: _LL259: _LL25A:
 redo_join=1;goto _LL252;default: goto _LL25B;}default: _LL25B: _LL25C:
 goto _LL252;}_LL252:;};}}
# 1089
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*_tmp283;for(0;effs != 0;effs=effs->tl){
void*_tmp287=Cyc_Tcutil_compress((void*)effs->hd);void*_tmp288=_tmp287;void*_tmp289;struct Cyc_List_List*_tmp28A;switch(*((int*)_tmp288)){case 24: _LL25E: _tmp28A=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp288)->f1;_LL25F:
# 1094
 effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp28A,effects);
goto _LL25D;case 23: switch(*((int*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp288)->f1)){case 20: _LL260: _LL261:
 goto _LL25D;case 22: _LL262: _LL263:
 goto _LL25D;case 21: _LL264: _LL265:
 goto _LL25D;default: goto _LL266;}default: _LL266: _tmp289=_tmp288;_LL267:
 effects=({struct Cyc_List_List*_tmp28B=_cycalloc(sizeof(*_tmp28B));_tmp28B->hd=_tmp289;_tmp28B->tl=effects;_tmp28B;});goto _LL25D;}_LL25D:;}}
# 1102
*_tmp283=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
return e;};}case 25: _LL24E: _tmp282=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp281)->f1;_LL24F: {
# 1105
void*_tmp28C=Cyc_Tcutil_compress(_tmp282);void*_tmp28D=_tmp28C;switch(*((int*)_tmp28D)){case 1: _LL269: _LL26A:
 goto _LL26C;case 2: _LL26B: _LL26C:
 return e;default: _LL26D: _LL26E:
 return Cyc_Tcutil_rgns_of(_tmp282);}_LL268:;}default: _LL250: _LL251:
# 1110
 return e;}_LL24B:;};}
# 1115
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp28E=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp28F=_cycalloc(sizeof(*_tmp28F));_tmp28F[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp290;_tmp290.tag=9;_tmp290.f1=({struct Cyc_Absyn_FnInfo _tmp291;_tmp291.tvars=0;_tmp291.effect=eff;_tmp291.ret_tqual=
Cyc_Absyn_empty_tqual(0);_tmp291.ret_typ=(void*)& Cyc_Absyn_VoidType_val;_tmp291.args=0;_tmp291.c_varargs=0;_tmp291.cyc_varargs=0;_tmp291.rgn_po=0;_tmp291.attributes=0;_tmp291.requires_clause=0;_tmp291.requires_relns=0;_tmp291.ensures_clause=0;_tmp291.ensures_relns=0;_tmp291;});_tmp290;});_tmp28F;});
# 1125
return Cyc_Absyn_atb_typ((void*)_tmp28E,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}
# 1132
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);
if((r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (void*)& Cyc_Absyn_UniqueRgn_val) || r == (void*)& Cyc_Absyn_RefCntRgn_val)
return 1;{
void*_tmp292=Cyc_Tcutil_compress(e);void*_tmp293=_tmp292;struct Cyc_Core_Opt*_tmp294;void**_tmp295;struct Cyc_Core_Opt*_tmp296;void*_tmp297;struct Cyc_List_List*_tmp298;void*_tmp299;switch(*((int*)_tmp293)){case 23: _LL270: _tmp299=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp293)->f1;_LL271:
# 1139
 if(constrain)return Cyc_Tcutil_unify(r,_tmp299);
_tmp299=Cyc_Tcutil_compress(_tmp299);
if(r == _tmp299)return 1;{
struct _tuple0 _tmp29A=({struct _tuple0 _tmp29E;_tmp29E.f1=r;_tmp29E.f2=_tmp299;_tmp29E;});struct _tuple0 _tmp29B=_tmp29A;struct Cyc_Absyn_Tvar*_tmp29C;struct Cyc_Absyn_Tvar*_tmp29D;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp29B.f1)->tag == 2){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp29B.f2)->tag == 2){_LL27B: _tmp29C=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp29B.f1)->f1;_tmp29D=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp29B.f2)->f1;_LL27C:
 return Cyc_Absyn_tvar_cmp(_tmp29C,_tmp29D)== 0;}else{goto _LL27D;}}else{_LL27D: _LL27E:
 return 0;}_LL27A:;};case 24: _LL272: _tmp298=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp293)->f1;_LL273:
# 1147
 for(0;_tmp298 != 0;_tmp298=_tmp298->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp298->hd))return 1;}
return 0;case 25: _LL274: _tmp297=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp293)->f1;_LL275: {
# 1151
void*_tmp29F=Cyc_Tcutil_rgns_of(_tmp297);void*_tmp2A0=_tmp29F;void*_tmp2A1;void*_tmp2A2;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2A0)->tag == 25){_LL280: _tmp2A2=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2A0)->f1;_LL281:
# 1153
 if(!constrain)return 0;{
void*_tmp2A3=Cyc_Tcutil_compress(_tmp2A2);void*_tmp2A4=_tmp2A3;struct Cyc_Core_Opt*_tmp2A5;void**_tmp2A6;struct Cyc_Core_Opt*_tmp2A7;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A4)->tag == 1){_LL285: _tmp2A5=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A4)->f1;_tmp2A6=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A4)->f2;_tmp2A7=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A4)->f4;_LL286: {
# 1158
void*_tmp2A8=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2A7);
# 1161
Cyc_Tcutil_occurs(_tmp2A8,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2A7))->v,r);{
void*_tmp2A9=Cyc_Tcutil_dummy_fntype((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2AA=_cycalloc(sizeof(*_tmp2AA));_tmp2AA[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2AB;_tmp2AB.tag=24;_tmp2AB.f1=({void*_tmp2AC[2];_tmp2AC[1]=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp2AD=_cycalloc(sizeof(*_tmp2AD));_tmp2AD[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp2AE;_tmp2AE.tag=23;_tmp2AE.f1=r;_tmp2AE;});_tmp2AD;});_tmp2AC[0]=_tmp2A8;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2AC,sizeof(void*),2));});_tmp2AB;});_tmp2AA;}));
*_tmp2A6=_tmp2A9;
return 1;};}}else{_LL287: _LL288:
 return 0;}_LL284:;};}else{_LL282: _tmp2A1=_tmp2A0;_LL283:
# 1167
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp2A1);}_LL27F:;}case 1: _LL276: _tmp294=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp293)->f1;_tmp295=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp293)->f2;_tmp296=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp293)->f4;_LL277:
# 1170
 if(_tmp294 == 0  || ((struct Cyc_Absyn_Kind*)_tmp294->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp2AF=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2B0="effect evar has wrong kind";_tag_dyneither(_tmp2B0,sizeof(char),27);}),_tag_dyneither(_tmp2AF,sizeof(void*),0));});
if(!constrain)return 0;{
# 1175
void*_tmp2B1=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp296);
# 1178
Cyc_Tcutil_occurs(_tmp2B1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp296))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2B2=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2B3=_cycalloc(sizeof(*_tmp2B3));_tmp2B3[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2B4;_tmp2B4.tag=24;_tmp2B4.f1=({struct Cyc_List_List*_tmp2B5=_cycalloc(sizeof(*_tmp2B5));_tmp2B5->hd=_tmp2B1;_tmp2B5->tl=({struct Cyc_List_List*_tmp2B6=_cycalloc(sizeof(*_tmp2B6));_tmp2B6->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp2B7=_cycalloc(sizeof(*_tmp2B7));_tmp2B7[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp2B8;_tmp2B8.tag=23;_tmp2B8.f1=r;_tmp2B8;});_tmp2B7;});_tmp2B6->tl=0;_tmp2B6;});_tmp2B5;});_tmp2B4;});_tmp2B3;});
*_tmp295=(void*)_tmp2B2;
return 1;};};default: _LL278: _LL279:
 return 0;}_LL26F:;};}
# 1189
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Tcutil_compress(t);{
void*_tmp2B9=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_tmp2BA=_tmp2B9;struct Cyc_Core_Opt*_tmp2BB;void**_tmp2BC;struct Cyc_Core_Opt*_tmp2BD;void*_tmp2BE;struct Cyc_List_List*_tmp2BF;switch(*((int*)_tmp2BA)){case 23: _LL28A: _LL28B:
 return 0;case 24: _LL28C: _tmp2BF=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2BA)->f1;_LL28D:
# 1194
 for(0;_tmp2BF != 0;_tmp2BF=_tmp2BF->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp2BF->hd))
return 1;}
return 0;case 25: _LL28E: _tmp2BE=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2BA)->f1;_LL28F:
# 1199
 _tmp2BE=Cyc_Tcutil_compress(_tmp2BE);
if(t == _tmp2BE)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp2BE);{
void*_tmp2C0=Cyc_Tcutil_rgns_of(t);void*_tmp2C1=_tmp2C0;void*_tmp2C2;void*_tmp2C3;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2C1)->tag == 25){_LL295: _tmp2C3=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2C1)->f1;_LL296: {
# 1204
struct _tuple0 _tmp2C4=({struct _tuple0 _tmp2C8;_tmp2C8.f1=Cyc_Tcutil_compress(_tmp2C3);_tmp2C8.f2=_tmp2BE;_tmp2C8;});struct _tuple0 _tmp2C5=_tmp2C4;struct Cyc_Absyn_Tvar*_tmp2C6;struct Cyc_Absyn_Tvar*_tmp2C7;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2C5.f1)->tag == 2){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2C5.f2)->tag == 2){_LL29A: _tmp2C6=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2C5.f1)->f1;_tmp2C7=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2C5.f2)->f1;_LL29B:
 return Cyc_Tcutil_unify(t,_tmp2BE);}else{goto _LL29C;}}else{_LL29C: _LL29D:
 return _tmp2C3 == _tmp2BE;}_LL299:;}}else{_LL297: _tmp2C2=_tmp2C1;_LL298:
# 1208
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp2C2);}_LL294:;};case 1: _LL290: _tmp2BB=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2BA)->f1;_tmp2BC=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2BA)->f2;_tmp2BD=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2BA)->f4;_LL291:
# 1211
 if(_tmp2BB == 0  || ((struct Cyc_Absyn_Kind*)_tmp2BB->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp2C9=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2CA="effect evar has wrong kind";_tag_dyneither(_tmp2CA,sizeof(char),27);}),_tag_dyneither(_tmp2C9,sizeof(void*),0));});
if(!may_constrain_evars)return 0;{
# 1216
void*_tmp2CB=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2BD);
# 1219
Cyc_Tcutil_occurs(_tmp2CB,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2BD))->v,t);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2CC=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2CD=_cycalloc(sizeof(*_tmp2CD));_tmp2CD[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2CE;_tmp2CE.tag=24;_tmp2CE.f1=({struct Cyc_List_List*_tmp2CF=_cycalloc(sizeof(*_tmp2CF));_tmp2CF->hd=_tmp2CB;_tmp2CF->tl=({struct Cyc_List_List*_tmp2D0=_cycalloc(sizeof(*_tmp2D0));_tmp2D0->hd=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp2D1=_cycalloc(sizeof(*_tmp2D1));_tmp2D1[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp2D2;_tmp2D2.tag=25;_tmp2D2.f1=t;_tmp2D2;});_tmp2D1;});_tmp2D0->tl=0;_tmp2D0;});_tmp2CF;});_tmp2CE;});_tmp2CD;});
*_tmp2BC=(void*)_tmp2CC;
return 1;};};default: _LL292: _LL293:
 return 0;}_LL289:;};}
# 1230
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp2D3=e;struct Cyc_Core_Opt*_tmp2D4;void**_tmp2D5;struct Cyc_Core_Opt*_tmp2D6;void*_tmp2D7;struct Cyc_List_List*_tmp2D8;struct Cyc_Absyn_Tvar*_tmp2D9;switch(*((int*)_tmp2D3)){case 2: _LL29F: _tmp2D9=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2D3)->f1;_LL2A0:
 return Cyc_Absyn_tvar_cmp(v,_tmp2D9)== 0;case 24: _LL2A1: _tmp2D8=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2D3)->f1;_LL2A2:
# 1235
 for(0;_tmp2D8 != 0;_tmp2D8=_tmp2D8->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)_tmp2D8->hd))
return 1;}
return 0;case 25: _LL2A3: _tmp2D7=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2D3)->f1;_LL2A4: {
# 1240
void*_tmp2DA=Cyc_Tcutil_rgns_of(_tmp2D7);void*_tmp2DB=_tmp2DA;void*_tmp2DC;void*_tmp2DD;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2DB)->tag == 25){_LL2AA: _tmp2DD=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2DB)->f1;_LL2AB:
# 1242
 if(!may_constrain_evars)return 0;{
void*_tmp2DE=Cyc_Tcutil_compress(_tmp2DD);void*_tmp2DF=_tmp2DE;struct Cyc_Core_Opt*_tmp2E0;void**_tmp2E1;struct Cyc_Core_Opt*_tmp2E2;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2DF)->tag == 1){_LL2AF: _tmp2E0=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2DF)->f1;_tmp2E1=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2DF)->f2;_tmp2E2=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2DF)->f4;_LL2B0: {
# 1248
void*_tmp2E3=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2E2);
# 1250
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2E2))->v,v))return 0;{
void*_tmp2E4=Cyc_Tcutil_dummy_fntype((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2E5=_cycalloc(sizeof(*_tmp2E5));_tmp2E5[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2E6;_tmp2E6.tag=24;_tmp2E6.f1=({void*_tmp2E7[2];_tmp2E7[1]=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2E8=_cycalloc(sizeof(*_tmp2E8));_tmp2E8[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp2E9;_tmp2E9.tag=2;_tmp2E9.f1=v;_tmp2E9;});_tmp2E8;});_tmp2E7[0]=_tmp2E3;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2E7,sizeof(void*),2));});_tmp2E6;});_tmp2E5;}));
*_tmp2E1=_tmp2E4;
return 1;};}}else{_LL2B1: _LL2B2:
 return 0;}_LL2AE:;};}else{_LL2AC: _tmp2DC=_tmp2DB;_LL2AD:
# 1256
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp2DC);}_LL2A9:;}case 1: _LL2A5: _tmp2D4=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2D3)->f1;_tmp2D5=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2D3)->f2;_tmp2D6=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2D3)->f4;_LL2A6:
# 1259
 if(_tmp2D4 == 0  || ((struct Cyc_Absyn_Kind*)_tmp2D4->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp2EA=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2EB="effect evar has wrong kind";_tag_dyneither(_tmp2EB,sizeof(char),27);}),_tag_dyneither(_tmp2EA,sizeof(void*),0));});{
# 1263
void*_tmp2EC=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2D6);
# 1265
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2D6))->v,v))
return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2ED=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2EE=_cycalloc(sizeof(*_tmp2EE));_tmp2EE[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2EF;_tmp2EF.tag=24;_tmp2EF.f1=({struct Cyc_List_List*_tmp2F0=_cycalloc(sizeof(*_tmp2F0));_tmp2F0->hd=_tmp2EC;_tmp2F0->tl=({struct Cyc_List_List*_tmp2F1=_cycalloc(sizeof(*_tmp2F1));_tmp2F1->hd=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2F2=_cycalloc(sizeof(*_tmp2F2));_tmp2F2[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp2F3;_tmp2F3.tag=2;_tmp2F3.f1=v;_tmp2F3;});_tmp2F2;});_tmp2F1->tl=0;_tmp2F1;});_tmp2F0;});_tmp2EF;});_tmp2EE;});
*_tmp2D5=(void*)_tmp2ED;
return 1;};};default: _LL2A7: _LL2A8:
 return 0;}_LL29E:;};}
# 1275
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp2F4=e;void*_tmp2F5;struct Cyc_List_List*_tmp2F6;switch(*((int*)_tmp2F4)){case 24: _LL2B4: _tmp2F6=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2F4)->f1;_LL2B5:
# 1279
 for(0;_tmp2F6 != 0;_tmp2F6=_tmp2F6->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp2F6->hd))
return 1;}
return 0;case 25: _LL2B6: _tmp2F5=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2F4)->f1;_LL2B7: {
# 1284
void*_tmp2F7=Cyc_Tcutil_rgns_of(_tmp2F5);void*_tmp2F8=_tmp2F7;void*_tmp2F9;void*_tmp2FA;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2F8)->tag == 25){_LL2BD: _tmp2FA=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2F8)->f1;_LL2BE:
 return 0;}else{_LL2BF: _tmp2F9=_tmp2F8;_LL2C0:
 return Cyc_Tcutil_evar_in_effect(evar,_tmp2F9);}_LL2BC:;}case 1: _LL2B8: _LL2B9:
# 1288
 return evar == e;default: _LL2BA: _LL2BB:
 return 0;}_LL2B3:;};}
# 1302 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1307
void*_tmp2FB=Cyc_Tcutil_compress(e1);void*_tmp2FC=_tmp2FB;void**_tmp2FD;struct Cyc_Core_Opt*_tmp2FE;void*_tmp2FF;struct Cyc_Absyn_Tvar*_tmp300;void*_tmp301;struct Cyc_List_List*_tmp302;switch(*((int*)_tmp2FC)){case 24: _LL2C2: _tmp302=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2FC)->f1;_LL2C3:
# 1309
 for(0;_tmp302 != 0;_tmp302=_tmp302->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp302->hd,e2))
return 0;}
return 1;case 23: _LL2C4: _tmp301=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp2FC)->f1;_LL2C5:
# 1320
 return Cyc_Tcutil_region_in_effect(may_constrain_evars,_tmp301,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp301,(void*)& Cyc_Absyn_HeapRgn_val);case 2: _LL2C6: _tmp300=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2FC)->f1;_LL2C7:
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp300,e2);case 25: _LL2C8: _tmp2FF=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2FC)->f1;_LL2C9: {
# 1324
void*_tmp303=Cyc_Tcutil_rgns_of(_tmp2FF);void*_tmp304=_tmp303;void*_tmp305;void*_tmp306;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp304)->tag == 25){_LL2CF: _tmp306=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp304)->f1;_LL2D0:
# 1329
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp306,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp306,Cyc_Absyn_sint_typ);}else{_LL2D1: _tmp305=_tmp304;_LL2D2:
 return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp305,e2);}_LL2CE:;}case 1: _LL2CA: _tmp2FD=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2FC)->f2;_tmp2FE=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2FC)->f4;_LL2CB:
# 1334
 if(!Cyc_Tcutil_evar_in_effect(e1,e2)){
# 1338
*_tmp2FD=Cyc_Absyn_empty_effect;
# 1341
return 1;}else{
# 1343
return 0;}default: _LL2CC: _LL2CD:
({struct Cyc_String_pa_PrintArg_struct _tmp309;_tmp309.tag=0;_tmp309.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1));({void*_tmp307[1]={& _tmp309};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp308="subset_effect: bad effect: %s";_tag_dyneither(_tmp308,sizeof(char),30);}),_tag_dyneither(_tmp307,sizeof(void*),1));});});}_LL2C1:;}
# 1359 "tcutil.cyc"
static int Cyc_Tcutil_unify_effect(void*e1,void*e2){
e1=Cyc_Tcutil_normalize_effect(e1);
e2=Cyc_Tcutil_normalize_effect(e2);
if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;
if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;
return 0;}
# 1375
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1377
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple0*_tmp30A=(struct _tuple0*)r1->hd;struct _tuple0*_tmp30B=_tmp30A;void*_tmp30C;void*_tmp30D;_LL2D4: _tmp30C=_tmp30B->f1;_tmp30D=_tmp30B->f2;_LL2D5:;{
int found=_tmp30C == (void*)& Cyc_Absyn_HeapRgn_val;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
struct _tuple0*_tmp30E=(struct _tuple0*)r2->hd;struct _tuple0*_tmp30F=_tmp30E;void*_tmp310;void*_tmp311;_LL2D7: _tmp310=_tmp30F->f1;_tmp311=_tmp30F->f2;_LL2D8:;
if(Cyc_Tcutil_unify(_tmp30C,_tmp310) && Cyc_Tcutil_unify(_tmp30D,_tmp311)){
found=1;
break;}}}
# 1387
if(!found)return 0;};}}
# 1389
return 1;}
# 1392
typedef struct Cyc_List_List*Cyc_Tcutil_relns_t;
# 1396
static int Cyc_Tcutil_check_logical_implication(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
for(0;r2 != 0;r2=r2->tl){
struct Cyc_Relations_Reln*_tmp312=Cyc_Relations_negate(Cyc_Core_heap_region,(struct Cyc_Relations_Reln*)r2->hd);
struct Cyc_List_List*_tmp313=({struct Cyc_List_List*_tmp314=_cycalloc(sizeof(*_tmp314));_tmp314->hd=_tmp312;_tmp314->tl=r1;_tmp314;});
if(Cyc_Relations_consistent_relations(_tmp313))return 0;}
# 1402
return 1;}
# 1407
static int Cyc_Tcutil_check_logical_equivalence(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
if(r1 == r2)return 1;
return Cyc_Tcutil_check_logical_implication(r1,r2) && Cyc_Tcutil_check_logical_implication(r2,r1);}
# 1413
static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1415
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple22{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*f2;};
# 1419
void Cyc_Tcutil_unify_it(void*t1,void*t2){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=_tag_dyneither(0,0,0);
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return;
{void*_tmp315=t1;struct Cyc_Core_Opt*_tmp316;void**_tmp317;struct Cyc_Core_Opt*_tmp318;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp315)->tag == 1){_LL2DA: _tmp316=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp315)->f1;_tmp317=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp315)->f2;_tmp318=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp315)->f4;_LL2DB:
# 1430
 Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp318))->v,t2);{
struct Cyc_Absyn_Kind*_tmp319=Cyc_Tcutil_typ_kind(t2);
# 1435
if(Cyc_Tcutil_kind_leq(_tmp319,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp316))->v)){
*_tmp317=t2;
return;}else{
# 1439
{void*_tmp31A=t2;struct Cyc_Absyn_PtrInfo _tmp31B;void**_tmp31C;struct Cyc_Core_Opt*_tmp31D;switch(*((int*)_tmp31A)){case 1: _LL2DF: _tmp31C=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp31A)->f2;_tmp31D=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp31A)->f4;_LL2E0: {
# 1442
struct Cyc_List_List*_tmp31E=(struct Cyc_List_List*)_tmp318->v;
{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp31D))->v;for(0;s2 != 0;s2=s2->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp31E,(struct Cyc_Absyn_Tvar*)s2->hd)){
Cyc_Tcutil_failure_reason=({const char*_tmp31F="(type variable would escape scope)";_tag_dyneither(_tmp31F,sizeof(char),35);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1449
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp316->v,_tmp319)){
*_tmp31C=t1;return;}
# 1452
Cyc_Tcutil_failure_reason=({const char*_tmp320="(kinds are incompatible)";_tag_dyneither(_tmp320,sizeof(char),25);});
goto _LL2DE;}case 5: _LL2E1: _tmp31B=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp31A)->f1;if(((struct Cyc_Absyn_Kind*)_tmp316->v)->kind == Cyc_Absyn_BoxKind){_LL2E2: {
# 1458
union Cyc_Absyn_Constraint*_tmp321=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((_tmp31B.ptr_atts).bounds);
{union Cyc_Absyn_Constraint*_tmp322=_tmp321;if((((union Cyc_Absyn_Constraint*)_tmp322)->No_constr).tag == 3){_LL2E6: _LL2E7:
# 1462
({struct _union_Constraint_Eq_constr*_tmp323=& _tmp321->Eq_constr;_tmp323->tag=1;_tmp323->val=Cyc_Absyn_bounds_one;});
*_tmp317=t2;
return;}else{_LL2E8: _LL2E9:
 goto _LL2E5;}_LL2E5:;}
# 1467
goto _LL2DE;}}else{goto _LL2E3;}default: _LL2E3: _LL2E4:
 goto _LL2DE;}_LL2DE:;}
# 1470
Cyc_Tcutil_failure_reason=({const char*_tmp324="(kinds are incompatible)";_tag_dyneither(_tmp324,sizeof(char),25);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};}else{_LL2DC: _LL2DD:
# 1473
 goto _LL2D9;}_LL2D9:;}
# 1478
{struct _tuple0 _tmp325=({struct _tuple0 _tmp3EA;_tmp3EA.f1=t2;_tmp3EA.f2=t1;_tmp3EA;});struct _tuple0 _tmp326=_tmp325;void*_tmp327;void*_tmp328;void*_tmp329;void*_tmp32A;void*_tmp32B;void*_tmp32C;struct Cyc_List_List*_tmp32D;struct Cyc_Absyn_Typedefdecl*_tmp32E;struct Cyc_List_List*_tmp32F;struct Cyc_Absyn_Typedefdecl*_tmp330;enum Cyc_Absyn_AggrKind _tmp331;struct Cyc_List_List*_tmp332;enum Cyc_Absyn_AggrKind _tmp333;struct Cyc_List_List*_tmp334;struct Cyc_List_List*_tmp335;struct Cyc_List_List*_tmp336;struct Cyc_List_List*_tmp337;void*_tmp338;struct Cyc_Absyn_Tqual _tmp339;void*_tmp33A;struct Cyc_List_List*_tmp33B;int _tmp33C;struct Cyc_Absyn_VarargInfo*_tmp33D;struct Cyc_List_List*_tmp33E;struct Cyc_List_List*_tmp33F;struct Cyc_Absyn_Exp*_tmp340;struct Cyc_List_List*_tmp341;struct Cyc_Absyn_Exp*_tmp342;struct Cyc_List_List*_tmp343;struct Cyc_List_List*_tmp344;void*_tmp345;struct Cyc_Absyn_Tqual _tmp346;void*_tmp347;struct Cyc_List_List*_tmp348;int _tmp349;struct Cyc_Absyn_VarargInfo*_tmp34A;struct Cyc_List_List*_tmp34B;struct Cyc_List_List*_tmp34C;struct Cyc_Absyn_Exp*_tmp34D;struct Cyc_List_List*_tmp34E;struct Cyc_Absyn_Exp*_tmp34F;struct Cyc_List_List*_tmp350;void*_tmp351;struct Cyc_Absyn_Tqual _tmp352;struct Cyc_Absyn_Exp*_tmp353;union Cyc_Absyn_Constraint*_tmp354;void*_tmp355;struct Cyc_Absyn_Tqual _tmp356;struct Cyc_Absyn_Exp*_tmp357;union Cyc_Absyn_Constraint*_tmp358;struct Cyc_Absyn_Exp*_tmp359;struct Cyc_Absyn_Exp*_tmp35A;void*_tmp35B;void*_tmp35C;int _tmp35D;int _tmp35E;enum Cyc_Absyn_Sign _tmp35F;enum Cyc_Absyn_Size_of _tmp360;enum Cyc_Absyn_Sign _tmp361;enum Cyc_Absyn_Size_of _tmp362;void*_tmp363;struct Cyc_Absyn_Tqual _tmp364;void*_tmp365;union Cyc_Absyn_Constraint*_tmp366;union Cyc_Absyn_Constraint*_tmp367;union Cyc_Absyn_Constraint*_tmp368;void*_tmp369;struct Cyc_Absyn_Tqual _tmp36A;void*_tmp36B;union Cyc_Absyn_Constraint*_tmp36C;union Cyc_Absyn_Constraint*_tmp36D;union Cyc_Absyn_Constraint*_tmp36E;struct Cyc_Absyn_Datatypedecl*_tmp36F;struct Cyc_Absyn_Datatypefield*_tmp370;struct Cyc_List_List*_tmp371;struct Cyc_Absyn_Datatypedecl*_tmp372;struct Cyc_Absyn_Datatypefield*_tmp373;struct Cyc_List_List*_tmp374;struct Cyc_Absyn_Datatypedecl*_tmp375;struct Cyc_List_List*_tmp376;struct Cyc_Absyn_Datatypedecl*_tmp377;struct Cyc_List_List*_tmp378;struct Cyc_List_List*_tmp379;struct Cyc_List_List*_tmp37A;struct _tuple2*_tmp37B;struct _tuple2*_tmp37C;union Cyc_Absyn_AggrInfoU _tmp37D;struct Cyc_List_List*_tmp37E;union Cyc_Absyn_AggrInfoU _tmp37F;struct Cyc_List_List*_tmp380;struct Cyc_Absyn_Tvar*_tmp381;struct Cyc_Absyn_Tvar*_tmp382;switch(*((int*)_tmp326.f1)){case 1: _LL2EB: _LL2EC:
# 1481
 Cyc_Tcutil_unify_it(t2,t1);
return;case 0: switch(*((int*)_tmp326.f2)){case 0: _LL2ED: _LL2EE:
# 1484
 return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 2: switch(*((int*)_tmp326.f2)){case 2: _LL2EF: _tmp381=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp326.f1)->f1;_tmp382=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp326.f2)->f1;_LL2F0: {
# 1487
struct _dyneither_ptr*_tmp383=_tmp381->name;
struct _dyneither_ptr*_tmp384=_tmp382->name;
# 1490
int _tmp385=_tmp381->identity;
int _tmp386=_tmp382->identity;
if(_tmp386 == _tmp385)return;
Cyc_Tcutil_failure_reason=({const char*_tmp387="(variable types are not the same)";_tag_dyneither(_tmp387,sizeof(char),34);});
goto _LL2EA;}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 11: switch(*((int*)_tmp326.f2)){case 11: _LL2F1: _tmp37D=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp326.f1)->f1).aggr_info;_tmp37E=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp326.f1)->f1).targs;_tmp37F=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp326.f2)->f1).aggr_info;_tmp380=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp326.f2)->f1).targs;_LL2F2: {
# 1497
struct _tuple13 _tmp388=Cyc_Absyn_aggr_kinded_name(_tmp37F);struct _tuple13 _tmp389=_tmp388;enum Cyc_Absyn_AggrKind _tmp38A;struct _tuple2*_tmp38B;_LL328: _tmp38A=_tmp389.f1;_tmp38B=_tmp389.f2;_LL329:;{
struct _tuple13 _tmp38C=Cyc_Absyn_aggr_kinded_name(_tmp37D);struct _tuple13 _tmp38D=_tmp38C;enum Cyc_Absyn_AggrKind _tmp38E;struct _tuple2*_tmp38F;_LL32B: _tmp38E=_tmp38D.f1;_tmp38F=_tmp38D.f2;_LL32C:;
if(_tmp38A != _tmp38E){Cyc_Tcutil_failure_reason=({const char*_tmp390="(struct and union type)";_tag_dyneither(_tmp390,sizeof(char),24);});goto _LL2EA;}
if(Cyc_Absyn_qvar_cmp(_tmp38B,_tmp38F)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp391="(different type name)";_tag_dyneither(_tmp391,sizeof(char),22);});goto _LL2EA;}
Cyc_Tcutil_unify_list(_tmp380,_tmp37E);
return;};}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 13: switch(*((int*)_tmp326.f2)){case 13: _LL2F3: _tmp37B=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp326.f1)->f1;_tmp37C=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp326.f2)->f1;_LL2F4:
# 1506
 if(Cyc_Absyn_qvar_cmp(_tmp37B,_tmp37C)== 0)return;
Cyc_Tcutil_failure_reason=({const char*_tmp392="(different enum types)";_tag_dyneither(_tmp392,sizeof(char),23);});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 14: switch(*((int*)_tmp326.f2)){case 14: _LL2F5: _tmp379=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp326.f1)->f1;_tmp37A=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp326.f2)->f1;_LL2F6: {
# 1511
int bad=0;
for(0;_tmp379 != 0  && _tmp37A != 0;(_tmp379=_tmp379->tl,_tmp37A=_tmp37A->tl)){
struct Cyc_Absyn_Enumfield*_tmp393=(struct Cyc_Absyn_Enumfield*)_tmp379->hd;
struct Cyc_Absyn_Enumfield*_tmp394=(struct Cyc_Absyn_Enumfield*)_tmp37A->hd;
if(Cyc_Absyn_qvar_cmp(_tmp393->name,_tmp394->name)!= 0){
Cyc_Tcutil_failure_reason=({const char*_tmp395="(different names for enum fields)";_tag_dyneither(_tmp395,sizeof(char),34);});
bad=1;
break;}
# 1520
if(_tmp393->tag == _tmp394->tag)continue;
if(_tmp393->tag == 0  || _tmp394->tag == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp396="(different tag values for enum fields)";_tag_dyneither(_tmp396,sizeof(char),39);});
bad=1;
break;}
# 1526
if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp393->tag),(struct Cyc_Absyn_Exp*)_check_null(_tmp394->tag))){
Cyc_Tcutil_failure_reason=({const char*_tmp397="(different tag values for enum fields)";_tag_dyneither(_tmp397,sizeof(char),39);});
bad=1;
break;}}
# 1532
if(bad)goto _LL2EA;
if(_tmp379 == 0  && _tmp37A == 0)return;
Cyc_Tcutil_failure_reason=({const char*_tmp398="(different number of fields for enums)";_tag_dyneither(_tmp398,sizeof(char),39);});
goto _LL2EA;}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp326.f1)->f1).datatype_info).KnownDatatype).tag == 2)switch(*((int*)_tmp326.f2)){case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp326.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL2F7: _tmp375=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp326.f1)->f1).datatype_info).KnownDatatype).val;_tmp376=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp326.f1)->f1).targs;_tmp377=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp326.f2)->f1).datatype_info).KnownDatatype).val;_tmp378=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp326.f2)->f1).targs;_LL2F8:
# 1539
 if(_tmp375 == _tmp377  || Cyc_Absyn_qvar_cmp(_tmp375->name,_tmp377->name)== 0){
Cyc_Tcutil_unify_list(_tmp378,_tmp376);
return;}
# 1543
goto _LL2EA;}else{goto _LL325;}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}else{switch(*((int*)_tmp326.f2)){case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}}case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp326.f1)->f1).field_info).KnownDatatypefield).tag == 2)switch(*((int*)_tmp326.f2)){case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp326.f2)->f1).field_info).KnownDatatypefield).tag == 2){_LL2F9: _tmp36F=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp326.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp370=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp326.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp371=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp326.f1)->f1).targs;_tmp372=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp326.f2)->f1).field_info).KnownDatatypefield).val).f1;_tmp373=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp326.f2)->f1).field_info).KnownDatatypefield).val).f2;_tmp374=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp326.f2)->f1).targs;_LL2FA:
# 1547
 if((_tmp36F == _tmp372  || Cyc_Absyn_qvar_cmp(_tmp36F->name,_tmp372->name)== 0) && (
_tmp370 == _tmp373  || Cyc_Absyn_qvar_cmp(_tmp370->name,_tmp373->name)== 0)){
Cyc_Tcutil_unify_list(_tmp374,_tmp371);
return;}
# 1552
Cyc_Tcutil_failure_reason=({const char*_tmp399="(different datatype field types)";_tag_dyneither(_tmp399,sizeof(char),33);});
goto _LL2EA;}else{goto _LL325;}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}else{switch(*((int*)_tmp326.f2)){case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}}case 5: switch(*((int*)_tmp326.f2)){case 5: _LL2FB: _tmp363=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326.f1)->f1).elt_typ;_tmp364=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326.f1)->f1).elt_tq;_tmp365=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326.f1)->f1).ptr_atts).rgn;_tmp366=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326.f1)->f1).ptr_atts).nullable;_tmp367=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326.f1)->f1).ptr_atts).bounds;_tmp368=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326.f1)->f1).ptr_atts).zero_term;_tmp369=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326.f2)->f1).elt_typ;_tmp36A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326.f2)->f1).elt_tq;_tmp36B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326.f2)->f1).ptr_atts).rgn;_tmp36C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326.f2)->f1).ptr_atts).nullable;_tmp36D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326.f2)->f1).ptr_atts).bounds;_tmp36E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326.f2)->f1).ptr_atts).zero_term;_LL2FC:
# 1557
 Cyc_Tcutil_unify_it(_tmp369,_tmp363);
Cyc_Tcutil_unify_it(_tmp365,_tmp36B);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp36E,_tmp368,({const char*_tmp39A="(not both zero terminated)";_tag_dyneither(_tmp39A,sizeof(char),27);}));
Cyc_Tcutil_unify_tqual(_tmp36A,_tmp369,_tmp364,_tmp363);
((void(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp36D,_tmp367,({const char*_tmp39B="(different pointer bounds)";_tag_dyneither(_tmp39B,sizeof(char),27);}));
# 1565
{void*_tmp39C=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp36D);void*_tmp39D=_tmp39C;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp39D)->tag == 0){_LL32E: _LL32F:
 return;}else{_LL330: _LL331:
 goto _LL32D;}_LL32D:;}
# 1569
((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp36C,_tmp366,({const char*_tmp39E="(incompatible pointer types)";_tag_dyneither(_tmp39E,sizeof(char),29);}));
return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 6: switch(*((int*)_tmp326.f2)){case 6: _LL2FD: _tmp35F=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp326.f1)->f1;_tmp360=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp326.f1)->f2;_tmp361=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp326.f2)->f1;_tmp362=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp326.f2)->f2;_LL2FE:
# 1574
 if(_tmp361 == _tmp35F  && ((_tmp362 == _tmp360  || _tmp362 == Cyc_Absyn_Int_sz  && _tmp360 == Cyc_Absyn_Long_sz) || 
_tmp362 == Cyc_Absyn_Long_sz  && _tmp360 == Cyc_Absyn_Int_sz))return;
Cyc_Tcutil_failure_reason=({const char*_tmp39F="(different integral types)";_tag_dyneither(_tmp39F,sizeof(char),27);});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 7: switch(*((int*)_tmp326.f2)){case 7: _LL2FF: _tmp35D=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp326.f1)->f1;_tmp35E=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp326.f2)->f1;_LL300:
# 1580
 if(_tmp35E == 0  && _tmp35D == 0)return;else{
if(_tmp35E == 1  && _tmp35D == 1)return;else{
# 1583
if(((_tmp35E != 0  && _tmp35E != 1) && _tmp35D != 0) && _tmp35D != 1)return;}}
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 19: switch(*((int*)_tmp326.f2)){case 19: _LL301: _tmp35B=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp326.f1)->f1;_tmp35C=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp326.f2)->f1;_LL302:
# 1586
 Cyc_Tcutil_unify_it(_tmp35B,_tmp35C);return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 18: switch(*((int*)_tmp326.f2)){case 18: _LL303: _tmp359=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp326.f1)->f1;_tmp35A=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp326.f2)->f1;_LL304:
# 1589
 if(!Cyc_Evexp_same_const_exp(_tmp359,_tmp35A)){
Cyc_Tcutil_failure_reason=({const char*_tmp3A0="(cannot prove expressions are the same)";_tag_dyneither(_tmp3A0,sizeof(char),40);});
goto _LL2EA;}
# 1593
return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 8: switch(*((int*)_tmp326.f2)){case 8: _LL305: _tmp351=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp326.f1)->f1).elt_type;_tmp352=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp326.f1)->f1).tq;_tmp353=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp326.f1)->f1).num_elts;_tmp354=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp326.f1)->f1).zero_term;_tmp355=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp326.f2)->f1).elt_type;_tmp356=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp326.f2)->f1).tq;_tmp357=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp326.f2)->f1).num_elts;_tmp358=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp326.f2)->f1).zero_term;_LL306:
# 1597
 Cyc_Tcutil_unify_it(_tmp355,_tmp351);
Cyc_Tcutil_unify_tqual(_tmp356,_tmp355,_tmp352,_tmp351);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp354,_tmp358,({const char*_tmp3A1="(not both zero terminated)";_tag_dyneither(_tmp3A1,sizeof(char),27);}));
if(_tmp353 == _tmp357)return;
if(_tmp353 == 0  || _tmp357 == 0)goto _LL2EA;
if(Cyc_Evexp_same_const_exp(_tmp353,_tmp357))
return;
Cyc_Tcutil_failure_reason=({const char*_tmp3A2="(different array sizes)";_tag_dyneither(_tmp3A2,sizeof(char),24);});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 9: switch(*((int*)_tmp326.f2)){case 9: _LL307: _tmp337=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f1)->f1).tvars;_tmp338=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f1)->f1).effect;_tmp339=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f1)->f1).ret_tqual;_tmp33A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f1)->f1).ret_typ;_tmp33B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f1)->f1).args;_tmp33C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f1)->f1).c_varargs;_tmp33D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f1)->f1).cyc_varargs;_tmp33E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f1)->f1).rgn_po;_tmp33F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f1)->f1).attributes;_tmp340=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f1)->f1).requires_clause;_tmp341=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f1)->f1).requires_relns;_tmp342=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f1)->f1).ensures_clause;_tmp343=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f1)->f1).ensures_relns;_tmp344=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f2)->f1).tvars;_tmp345=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f2)->f1).effect;_tmp346=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f2)->f1).ret_tqual;_tmp347=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f2)->f1).ret_typ;_tmp348=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f2)->f1).args;_tmp349=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f2)->f1).c_varargs;_tmp34A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f2)->f1).cyc_varargs;_tmp34B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f2)->f1).rgn_po;_tmp34C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f2)->f1).attributes;_tmp34D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f2)->f1).requires_clause;_tmp34E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f2)->f1).requires_relns;_tmp34F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f2)->f1).ensures_clause;_tmp350=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f2)->f1).ensures_relns;_LL308: {
# 1611
int done=0;
{struct _RegionHandle _tmp3A3=_new_region("rgn");struct _RegionHandle*rgn=& _tmp3A3;_push_region(rgn);
{struct Cyc_List_List*inst=0;
while(_tmp344 != 0){
if(_tmp337 == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp3A4="(second function type has too few type variables)";_tag_dyneither(_tmp3A4,sizeof(char),50);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1619
void*_tmp3A5=((struct Cyc_Absyn_Tvar*)_tmp344->hd)->kind;
void*_tmp3A6=((struct Cyc_Absyn_Tvar*)_tmp337->hd)->kind;
if(!Cyc_Tcutil_unify_kindbound(_tmp3A5,_tmp3A6)){
Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3AB;_tmp3AB.tag=0;_tmp3AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1625
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp337->hd,& Cyc_Tcutil_bk)));({struct Cyc_String_pa_PrintArg_struct _tmp3AA;_tmp3AA.tag=0;_tmp3AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1624
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp344->hd,& Cyc_Tcutil_bk)));({struct Cyc_String_pa_PrintArg_struct _tmp3A9;_tmp3A9.tag=0;_tmp3A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1623
Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)_tmp344->hd));({void*_tmp3A7[3]={& _tmp3A9,& _tmp3AA,& _tmp3AB};Cyc_aprintf(({const char*_tmp3A8="(type var %s has different kinds %s and %s)";_tag_dyneither(_tmp3A8,sizeof(char),44);}),_tag_dyneither(_tmp3A7,sizeof(void*),3));});});});});
# 1626
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1628
inst=({struct Cyc_List_List*_tmp3AC=_region_malloc(rgn,sizeof(*_tmp3AC));_tmp3AC->hd=({struct _tuple16*_tmp3AD=_region_malloc(rgn,sizeof(*_tmp3AD));_tmp3AD->f1=(struct Cyc_Absyn_Tvar*)_tmp337->hd;_tmp3AD->f2=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3AE=_cycalloc(sizeof(*_tmp3AE));_tmp3AE[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp3AF;_tmp3AF.tag=2;_tmp3AF.f1=(struct Cyc_Absyn_Tvar*)_tmp344->hd;_tmp3AF;});_tmp3AE;});_tmp3AD;});_tmp3AC->tl=inst;_tmp3AC;});
_tmp344=_tmp344->tl;
_tmp337=_tmp337->tl;};}
# 1632
if(_tmp337 != 0){
Cyc_Tcutil_failure_reason=({const char*_tmp3B0="(second function type has too many type variables)";_tag_dyneither(_tmp3B0,sizeof(char),51);});
_npop_handler(0);goto _LL2EA;}
# 1636
if(inst != 0){
Cyc_Tcutil_unify_it((void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp3B1=_cycalloc(sizeof(*_tmp3B1));_tmp3B1[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp3B2;_tmp3B2.tag=9;_tmp3B2.f1=({struct Cyc_Absyn_FnInfo _tmp3B3;_tmp3B3.tvars=0;_tmp3B3.effect=_tmp345;_tmp3B3.ret_tqual=_tmp346;_tmp3B3.ret_typ=_tmp347;_tmp3B3.args=_tmp348;_tmp3B3.c_varargs=_tmp349;_tmp3B3.cyc_varargs=_tmp34A;_tmp3B3.rgn_po=_tmp34B;_tmp3B3.attributes=_tmp34C;_tmp3B3.requires_clause=_tmp340;_tmp3B3.requires_relns=_tmp341;_tmp3B3.ensures_clause=_tmp342;_tmp3B3.ensures_relns=_tmp343;_tmp3B3;});_tmp3B2;});_tmp3B1;}),
# 1640
Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp3B4=_cycalloc(sizeof(*_tmp3B4));_tmp3B4[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp3B5;_tmp3B5.tag=9;_tmp3B5.f1=({struct Cyc_Absyn_FnInfo _tmp3B6;_tmp3B6.tvars=0;_tmp3B6.effect=_tmp338;_tmp3B6.ret_tqual=_tmp339;_tmp3B6.ret_typ=_tmp33A;_tmp3B6.args=_tmp33B;_tmp3B6.c_varargs=_tmp33C;_tmp3B6.cyc_varargs=_tmp33D;_tmp3B6.rgn_po=_tmp33E;_tmp3B6.attributes=_tmp33F;_tmp3B6.requires_clause=_tmp34D;_tmp3B6.requires_relns=_tmp34E;_tmp3B6.ensures_clause=_tmp34F;_tmp3B6.ensures_relns=_tmp350;_tmp3B6;});_tmp3B5;});_tmp3B4;})));
# 1645
done=1;}}
# 1613
;_pop_region(rgn);}
# 1648
if(done)
return;
Cyc_Tcutil_unify_it(_tmp347,_tmp33A);
Cyc_Tcutil_unify_tqual(_tmp346,_tmp347,_tmp339,_tmp33A);
for(0;_tmp348 != 0  && _tmp33B != 0;(_tmp348=_tmp348->tl,_tmp33B=_tmp33B->tl)){
struct _tuple10 _tmp3B7=*((struct _tuple10*)_tmp348->hd);struct _tuple10 _tmp3B8=_tmp3B7;struct Cyc_Absyn_Tqual _tmp3B9;void*_tmp3BA;_LL333: _tmp3B9=_tmp3B8.f2;_tmp3BA=_tmp3B8.f3;_LL334:;{
struct _tuple10 _tmp3BB=*((struct _tuple10*)_tmp33B->hd);struct _tuple10 _tmp3BC=_tmp3BB;struct Cyc_Absyn_Tqual _tmp3BD;void*_tmp3BE;_LL336: _tmp3BD=_tmp3BC.f2;_tmp3BE=_tmp3BC.f3;_LL337:;
Cyc_Tcutil_unify_it(_tmp3BA,_tmp3BE);
Cyc_Tcutil_unify_tqual(_tmp3B9,_tmp3BA,_tmp3BD,_tmp3BE);};}
# 1658
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(_tmp348 != 0  || _tmp33B != 0){
Cyc_Tcutil_failure_reason=({const char*_tmp3BF="(function types have different number of arguments)";_tag_dyneither(_tmp3BF,sizeof(char),52);});
goto _LL2EA;}
# 1664
if(_tmp349 != _tmp33C){
Cyc_Tcutil_failure_reason=({const char*_tmp3C0="(only one function type takes C varargs)";_tag_dyneither(_tmp3C0,sizeof(char),41);});
goto _LL2EA;}{
# 1669
int bad_cyc_vararg=0;
{struct _tuple22 _tmp3C1=({struct _tuple22 _tmp3CD;_tmp3CD.f1=_tmp34A;_tmp3CD.f2=_tmp33D;_tmp3CD;});struct _tuple22 _tmp3C2=_tmp3C1;struct _dyneither_ptr*_tmp3C3;struct Cyc_Absyn_Tqual _tmp3C4;void*_tmp3C5;int _tmp3C6;struct _dyneither_ptr*_tmp3C7;struct Cyc_Absyn_Tqual _tmp3C8;void*_tmp3C9;int _tmp3CA;if(_tmp3C2.f1 == 0){if(_tmp3C2.f2 == 0){_LL339: _LL33A:
 goto _LL338;}else{_LL33B: _LL33C:
 goto _LL33E;}}else{if(_tmp3C2.f2 == 0){_LL33D: _LL33E:
# 1674
 bad_cyc_vararg=1;
Cyc_Tcutil_failure_reason=({const char*_tmp3CB="(only one function type takes varargs)";_tag_dyneither(_tmp3CB,sizeof(char),39);});
goto _LL338;}else{_LL33F: _tmp3C3=(_tmp3C2.f1)->name;_tmp3C4=(_tmp3C2.f1)->tq;_tmp3C5=(_tmp3C2.f1)->type;_tmp3C6=(_tmp3C2.f1)->inject;_tmp3C7=(_tmp3C2.f2)->name;_tmp3C8=(_tmp3C2.f2)->tq;_tmp3C9=(_tmp3C2.f2)->type;_tmp3CA=(_tmp3C2.f2)->inject;_LL340:
# 1678
 Cyc_Tcutil_unify_it(_tmp3C5,_tmp3C9);
Cyc_Tcutil_unify_tqual(_tmp3C4,_tmp3C5,_tmp3C8,_tmp3C9);
if(_tmp3C6 != _tmp3CA){
bad_cyc_vararg=1;
Cyc_Tcutil_failure_reason=({const char*_tmp3CC="(only one function type injects varargs)";_tag_dyneither(_tmp3CC,sizeof(char),41);});}
# 1684
goto _LL338;}}_LL338:;}
# 1686
if(bad_cyc_vararg)goto _LL2EA;{
# 1689
int bad_effect=0;
{struct _tuple0 _tmp3CE=({struct _tuple0 _tmp3D0;_tmp3D0.f1=_tmp345;_tmp3D0.f2=_tmp338;_tmp3D0;});struct _tuple0 _tmp3CF=_tmp3CE;if(_tmp3CF.f1 == 0){if(_tmp3CF.f2 == 0){_LL342: _LL343:
 goto _LL341;}else{_LL344: _LL345:
 goto _LL347;}}else{if(_tmp3CF.f2 == 0){_LL346: _LL347:
 bad_effect=1;goto _LL341;}else{_LL348: _LL349:
 bad_effect=!Cyc_Tcutil_unify_effect((void*)_check_null(_tmp345),(void*)_check_null(_tmp338));goto _LL341;}}_LL341:;}
# 1696
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(bad_effect){
Cyc_Tcutil_failure_reason=({const char*_tmp3D1="(function effects do not match)";_tag_dyneither(_tmp3D1,sizeof(char),32);});
goto _LL2EA;}
# 1702
if(!Cyc_Tcutil_same_atts(_tmp33F,_tmp34C)){
Cyc_Tcutil_failure_reason=({const char*_tmp3D2="(function types have different attributes)";_tag_dyneither(_tmp3D2,sizeof(char),43);});
goto _LL2EA;}
# 1706
if(!Cyc_Tcutil_same_rgn_po(_tmp33E,_tmp34B)){
Cyc_Tcutil_failure_reason=({const char*_tmp3D3="(function types have different region lifetime orderings)";_tag_dyneither(_tmp3D3,sizeof(char),58);});
goto _LL2EA;}
# 1710
if(!Cyc_Tcutil_check_logical_equivalence(_tmp341,_tmp34E)){
Cyc_Tcutil_failure_reason=({const char*_tmp3D4="(@requires clauses not equivalent)";_tag_dyneither(_tmp3D4,sizeof(char),35);});
goto _LL2EA;}
# 1714
if(!Cyc_Tcutil_check_logical_equivalence(_tmp343,_tmp350)){
Cyc_Tcutil_failure_reason=({const char*_tmp3D5="(@ensures clauses not equivalent)";_tag_dyneither(_tmp3D5,sizeof(char),34);});
goto _LL2EA;}
# 1718
return;};};}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 10: switch(*((int*)_tmp326.f2)){case 10: _LL309: _tmp335=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp326.f1)->f1;_tmp336=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp326.f2)->f1;_LL30A:
# 1721
 for(0;_tmp336 != 0  && _tmp335 != 0;(_tmp336=_tmp336->tl,_tmp335=_tmp335->tl)){
struct _tuple12 _tmp3D6=*((struct _tuple12*)_tmp336->hd);struct _tuple12 _tmp3D7=_tmp3D6;struct Cyc_Absyn_Tqual _tmp3D8;void*_tmp3D9;_LL34B: _tmp3D8=_tmp3D7.f1;_tmp3D9=_tmp3D7.f2;_LL34C:;{
struct _tuple12 _tmp3DA=*((struct _tuple12*)_tmp335->hd);struct _tuple12 _tmp3DB=_tmp3DA;struct Cyc_Absyn_Tqual _tmp3DC;void*_tmp3DD;_LL34E: _tmp3DC=_tmp3DB.f1;_tmp3DD=_tmp3DB.f2;_LL34F:;
Cyc_Tcutil_unify_it(_tmp3D9,_tmp3DD);
Cyc_Tcutil_unify_tqual(_tmp3D8,_tmp3D9,_tmp3DC,_tmp3DD);};}
# 1727
if(_tmp336 == 0  && _tmp335 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp3DE="(tuple types have different numbers of components)";_tag_dyneither(_tmp3DE,sizeof(char),51);});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 12: switch(*((int*)_tmp326.f2)){case 12: _LL30B: _tmp331=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp326.f1)->f1;_tmp332=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp326.f1)->f2;_tmp333=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp326.f2)->f1;_tmp334=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp326.f2)->f2;_LL30C:
# 1734
 if(_tmp333 != _tmp331){Cyc_Tcutil_failure_reason=({const char*_tmp3DF="(struct and union type)";_tag_dyneither(_tmp3DF,sizeof(char),24);});goto _LL2EA;}
for(0;_tmp334 != 0  && _tmp332 != 0;(_tmp334=_tmp334->tl,_tmp332=_tmp332->tl)){
struct Cyc_Absyn_Aggrfield*_tmp3E0=(struct Cyc_Absyn_Aggrfield*)_tmp334->hd;
struct Cyc_Absyn_Aggrfield*_tmp3E1=(struct Cyc_Absyn_Aggrfield*)_tmp332->hd;
if(Cyc_strptrcmp(_tmp3E0->name,_tmp3E1->name)!= 0){
Cyc_Tcutil_failure_reason=({const char*_tmp3E2="(different member names)";_tag_dyneither(_tmp3E2,sizeof(char),25);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1742
Cyc_Tcutil_unify_it(_tmp3E0->type,_tmp3E1->type);
Cyc_Tcutil_unify_tqual(_tmp3E0->tq,_tmp3E0->type,_tmp3E1->tq,_tmp3E1->type);
if(!Cyc_Tcutil_same_atts(_tmp3E0->attributes,_tmp3E1->attributes)){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp3E3="(different attributes on member)";_tag_dyneither(_tmp3E3,sizeof(char),33);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1750
if((_tmp3E0->width != 0  && _tmp3E1->width == 0  || 
_tmp3E1->width != 0  && _tmp3E0->width == 0) || 
(_tmp3E0->width != 0  && _tmp3E1->width != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp3E0->width),(struct Cyc_Absyn_Exp*)_check_null(_tmp3E1->width))){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp3E4="(different bitfield widths on member)";_tag_dyneither(_tmp3E4,sizeof(char),38);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1759
if((_tmp3E0->requires_clause != 0  && _tmp3E1->requires_clause == 0  || 
_tmp3E0->requires_clause == 0  && _tmp3E1->requires_clause != 0) || 
(_tmp3E0->requires_clause == 0  && _tmp3E1->requires_clause != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp3E0->requires_clause),(struct Cyc_Absyn_Exp*)_check_null(_tmp3E1->requires_clause))){
# 1764
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp3E5="(different @requires clauses on member)";_tag_dyneither(_tmp3E5,sizeof(char),40);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1770
if(_tmp334 == 0  && _tmp332 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp3E6="(different number of members)";_tag_dyneither(_tmp3E6,sizeof(char),30);});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 17: switch(*((int*)_tmp326.f2)){case 17: _LL30D: _tmp32D=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp326.f1)->f2;_tmp32E=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp326.f1)->f3;_tmp32F=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp326.f2)->f2;_tmp330=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp326.f2)->f3;_LL30E:
# 1776
 if(_tmp32E != _tmp330){
Cyc_Tcutil_failure_reason=({const char*_tmp3E7="(different abstract typedefs)";_tag_dyneither(_tmp3E7,sizeof(char),30);});
goto _LL2EA;}
# 1780
Cyc_Tcutil_failure_reason=({const char*_tmp3E8="(type parameters to typedef differ)";_tag_dyneither(_tmp3E8,sizeof(char),36);});
Cyc_Tcutil_unify_list(_tmp32D,_tmp32F);
return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 20: switch(*((int*)_tmp326.f2)){case 20: _LL30F: _LL310:
 return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 21: switch(*((int*)_tmp326.f2)){case 21: _LL311: _LL312:
 return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 22: switch(*((int*)_tmp326.f2)){case 22: _LL313: _LL314:
 return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 15: switch(*((int*)_tmp326.f2)){case 15: _LL315: _tmp32B=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp326.f1)->f1;_tmp32C=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp326.f2)->f1;_LL316:
# 1787
 Cyc_Tcutil_unify_it(_tmp32B,_tmp32C);
return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 16: switch(*((int*)_tmp326.f2)){case 16: _LL317: _tmp327=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp326.f1)->f1;_tmp328=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp326.f1)->f2;_tmp329=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp326.f2)->f1;_tmp32A=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp326.f2)->f2;_LL318:
# 1790
 Cyc_Tcutil_unify_it(_tmp327,_tmp329);
Cyc_Tcutil_unify_it(_tmp328,_tmp32A);
return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 24: _LL319: _LL31A:
 goto _LL31C;default: if(((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp326.f2)->tag == 24){_LL31B: _LL31C:
 goto _LL31E;}else{switch(*((int*)_tmp326.f1)){case 23: _LL31D: _LL31E:
 goto _LL320;case 25: _LL31F: _LL320:
 goto _LL322;default: switch(*((int*)_tmp326.f2)){case 25: _LL321: _LL322:
 goto _LL324;case 23: _LL323: _LL324:
# 1799
 if(Cyc_Tcutil_unify_effect(t1,t2))return;
Cyc_Tcutil_failure_reason=({const char*_tmp3E9="(effects don't unify)";_tag_dyneither(_tmp3E9,sizeof(char),22);});
goto _LL2EA;default: _LL325: _LL326:
 goto _LL2EA;}}}}_LL2EA:;}
# 1804
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1807
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0  && a2 != 0)return - 1;
if(a1 != 0  && a2 == 0)return 1;
return cmp((void*)_check_null(a1),(void*)_check_null(a2));}
# 1814
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp3EB=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
int _tmp3EC=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp3EB,_tmp3EC);}
# 1821
static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return 0;{
union Cyc_Absyn_Constraint*_tmp3ED=x;void*_tmp3EE;switch((((union Cyc_Absyn_Constraint*)_tmp3ED)->Eq_constr).tag){case 3: _LL351: _LL352:
 return - 1;case 1: _LL353: _tmp3EE=(void*)(_tmp3ED->Eq_constr).val;_LL354: {
# 1828
union Cyc_Absyn_Constraint*_tmp3EF=y;void*_tmp3F0;switch((((union Cyc_Absyn_Constraint*)_tmp3EF)->Eq_constr).tag){case 3: _LL358: _LL359:
 return 1;case 1: _LL35A: _tmp3F0=(void*)(_tmp3EF->Eq_constr).val;_LL35B:
 return cmp(_tmp3EE,_tmp3F0);default: _LL35C: _LL35D:
({void*_tmp3F1=0;Cyc_Tcutil_impos(({const char*_tmp3F2="unify_conref: forward after compress(2)";_tag_dyneither(_tmp3F2,sizeof(char),40);}),_tag_dyneither(_tmp3F1,sizeof(void*),0));});}_LL357:;}default: _LL355: _LL356:
# 1833
({void*_tmp3F3=0;Cyc_Tcutil_impos(({const char*_tmp3F4="unify_conref: forward after compress";_tag_dyneither(_tmp3F4,sizeof(char),37);}),_tag_dyneither(_tmp3F3,sizeof(void*),0));});}_LL350:;};}
# 1837
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct _tuple12*_tmp3F5=tqt1;struct Cyc_Absyn_Tqual _tmp3F6;void*_tmp3F7;_LL35F: _tmp3F6=_tmp3F5->f1;_tmp3F7=_tmp3F5->f2;_LL360:;{
struct _tuple12*_tmp3F8=tqt2;struct Cyc_Absyn_Tqual _tmp3F9;void*_tmp3FA;_LL362: _tmp3F9=_tmp3F8->f1;_tmp3FA=_tmp3F8->f2;_LL363:;{
int _tmp3FB=Cyc_Tcutil_tqual_cmp(_tmp3F6,_tmp3F9);
if(_tmp3FB != 0)return _tmp3FB;
return Cyc_Tcutil_typecmp(_tmp3F7,_tmp3FA);};};}
# 1845
static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp3FC=Cyc_strptrcmp(f1->name,f2->name);
if(_tmp3FC != 0)return _tmp3FC;{
int _tmp3FD=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);
if(_tmp3FD != 0)return _tmp3FD;{
int _tmp3FE=Cyc_Tcutil_typecmp(f1->type,f2->type);
if(_tmp3FE != 0)return _tmp3FE;{
int _tmp3FF=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);
if(_tmp3FF != 0)return _tmp3FF;
_tmp3FF=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);
if(_tmp3FF != 0)return _tmp3FF;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->requires_clause,f2->requires_clause);};};};}
# 1859
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp400=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp400 != 0)return _tmp400;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}
# 1865
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp401=t;switch(*((int*)_tmp401)){case 0: _LL365: _LL366:
 return 0;case 1: _LL367: _LL368:
 return 1;case 2: _LL369: _LL36A:
 return 2;case 3: _LL36B: _LL36C:
 return 3;case 4: _LL36D: _LL36E:
 return 4;case 5: _LL36F: _LL370:
 return 5;case 6: _LL371: _LL372:
 return 6;case 7: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp401)->f1 == 0){_LL373: _LL374:
 return 7;}else{_LL375: _LL376:
 return 8;}case 8: _LL377: _LL378:
 return 9;case 9: _LL379: _LL37A:
 return 10;case 10: _LL37B: _LL37C:
 return 11;case 11: _LL37D: _LL37E:
 return 12;case 12: _LL37F: _LL380:
 return 14;case 13: _LL381: _LL382:
 return 16;case 14: _LL383: _LL384:
 return 17;case 15: _LL385: _LL386:
 return 18;case 17: _LL387: _LL388:
 return 19;case 21: _LL389: _LL38A:
 return 20;case 20: _LL38B: _LL38C:
 return 21;case 23: _LL38D: _LL38E:
 return 22;case 24: _LL38F: _LL390:
 return 23;case 25: _LL391: _LL392:
 return 24;case 22: _LL393: _LL394:
 return 25;case 19: _LL395: _LL396:
 return 26;case 16: _LL397: _LL398:
 return 27;case 18: _LL399: _LL39A:
 return 28;case 26: _LL39B: _LL39C:
 return 29;case 27: _LL39D: _LL39E:
 return 30;default: _LL39F: _LL3A0:
 return 31;}_LL364:;}
# 1902
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return 0;{
int _tmp402=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));
if(_tmp402 != 0)
return _tmp402;{
# 1911
struct _tuple0 _tmp403=({struct _tuple0 _tmp466;_tmp466.f1=t2;_tmp466.f2=t1;_tmp466;});struct _tuple0 _tmp404=_tmp403;struct _dyneither_ptr _tmp405;struct _dyneither_ptr _tmp406;struct Cyc_Absyn_Exp*_tmp407;struct Cyc_Absyn_Exp*_tmp408;struct Cyc_Absyn_Exp*_tmp409;struct Cyc_Absyn_Exp*_tmp40A;void*_tmp40B;void*_tmp40C;void*_tmp40D;void*_tmp40E;void*_tmp40F;void*_tmp410;void*_tmp411;void*_tmp412;enum Cyc_Absyn_AggrKind _tmp413;struct Cyc_List_List*_tmp414;enum Cyc_Absyn_AggrKind _tmp415;struct Cyc_List_List*_tmp416;struct Cyc_List_List*_tmp417;struct Cyc_List_List*_tmp418;void*_tmp419;struct Cyc_Absyn_Tqual _tmp41A;struct Cyc_Absyn_Exp*_tmp41B;union Cyc_Absyn_Constraint*_tmp41C;void*_tmp41D;struct Cyc_Absyn_Tqual _tmp41E;struct Cyc_Absyn_Exp*_tmp41F;union Cyc_Absyn_Constraint*_tmp420;int _tmp421;int _tmp422;enum Cyc_Absyn_Sign _tmp423;enum Cyc_Absyn_Size_of _tmp424;enum Cyc_Absyn_Sign _tmp425;enum Cyc_Absyn_Size_of _tmp426;void*_tmp427;struct Cyc_Absyn_Tqual _tmp428;void*_tmp429;union Cyc_Absyn_Constraint*_tmp42A;union Cyc_Absyn_Constraint*_tmp42B;union Cyc_Absyn_Constraint*_tmp42C;void*_tmp42D;struct Cyc_Absyn_Tqual _tmp42E;void*_tmp42F;union Cyc_Absyn_Constraint*_tmp430;union Cyc_Absyn_Constraint*_tmp431;union Cyc_Absyn_Constraint*_tmp432;struct Cyc_Absyn_Datatypedecl*_tmp433;struct Cyc_Absyn_Datatypefield*_tmp434;struct Cyc_List_List*_tmp435;struct Cyc_Absyn_Datatypedecl*_tmp436;struct Cyc_Absyn_Datatypefield*_tmp437;struct Cyc_List_List*_tmp438;struct Cyc_Absyn_Datatypedecl*_tmp439;struct Cyc_List_List*_tmp43A;struct Cyc_Absyn_Datatypedecl*_tmp43B;struct Cyc_List_List*_tmp43C;struct Cyc_List_List*_tmp43D;struct Cyc_List_List*_tmp43E;struct _tuple2*_tmp43F;struct _tuple2*_tmp440;union Cyc_Absyn_AggrInfoU _tmp441;struct Cyc_List_List*_tmp442;union Cyc_Absyn_AggrInfoU _tmp443;struct Cyc_List_List*_tmp444;struct Cyc_Absyn_Tvar*_tmp445;struct Cyc_Absyn_Tvar*_tmp446;switch(*((int*)_tmp404.f1)){case 1: switch(*((int*)_tmp404.f2)){case 1: _LL3A2: _LL3A3:
# 1913
({void*_tmp447=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp448="typecmp: can only compare closed types";_tag_dyneither(_tmp448,sizeof(char),39);}),_tag_dyneither(_tmp447,sizeof(void*),0));});case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 2: switch(*((int*)_tmp404.f2)){case 2: _LL3A4: _tmp445=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp404.f1)->f1;_tmp446=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp404.f2)->f1;_LL3A5:
# 1917
 return Cyc_Core_intcmp(_tmp446->identity,_tmp445->identity);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 11: switch(*((int*)_tmp404.f2)){case 11: _LL3A6: _tmp441=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp404.f1)->f1).aggr_info;_tmp442=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp404.f1)->f1).targs;_tmp443=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp404.f2)->f1).aggr_info;_tmp444=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp404.f2)->f1).targs;_LL3A7: {
# 1921
struct _tuple13 _tmp449=Cyc_Absyn_aggr_kinded_name(_tmp441);struct _tuple13 _tmp44A=_tmp449;struct _tuple2*_tmp44B;_LL3D9: _tmp44B=_tmp44A.f2;_LL3DA:;{
struct _tuple13 _tmp44C=Cyc_Absyn_aggr_kinded_name(_tmp443);struct _tuple13 _tmp44D=_tmp44C;struct _tuple2*_tmp44E;_LL3DC: _tmp44E=_tmp44D.f2;_LL3DD:;{
int _tmp44F=Cyc_Absyn_qvar_cmp(_tmp44B,_tmp44E);
if(_tmp44F != 0)return _tmp44F;else{
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp442,_tmp444);}};};}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 13: switch(*((int*)_tmp404.f2)){case 13: _LL3A8: _tmp43F=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp404.f1)->f1;_tmp440=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp404.f2)->f1;_LL3A9:
# 1928
 return Cyc_Absyn_qvar_cmp(_tmp43F,_tmp440);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 14: switch(*((int*)_tmp404.f2)){case 14: _LL3AA: _tmp43D=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp404.f1)->f1;_tmp43E=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp404.f2)->f1;_LL3AB:
# 1931
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp43D,_tmp43E);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp404.f1)->f1).datatype_info).KnownDatatype).tag == 2)switch(*((int*)_tmp404.f2)){case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp404.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL3AC: _tmp439=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp404.f1)->f1).datatype_info).KnownDatatype).val;_tmp43A=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp404.f1)->f1).targs;_tmp43B=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp404.f2)->f1).datatype_info).KnownDatatype).val;_tmp43C=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp404.f2)->f1).targs;_LL3AD:
# 1935
 if(_tmp43B == _tmp439)return 0;{
int _tmp450=Cyc_Absyn_qvar_cmp(_tmp43B->name,_tmp439->name);
if(_tmp450 != 0)return _tmp450;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp43C,_tmp43A);};}else{goto _LL3D6;}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}else{switch(*((int*)_tmp404.f2)){case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}}case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp404.f1)->f1).field_info).KnownDatatypefield).tag == 2)switch(*((int*)_tmp404.f2)){case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp404.f2)->f1).field_info).KnownDatatypefield).tag == 2){_LL3AE: _tmp433=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp404.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp434=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp404.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp435=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp404.f1)->f1).targs;_tmp436=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp404.f2)->f1).field_info).KnownDatatypefield).val).f1;_tmp437=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp404.f2)->f1).field_info).KnownDatatypefield).val).f2;_tmp438=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp404.f2)->f1).targs;_LL3AF:
# 1942
 if(_tmp436 == _tmp433)return 0;{
int _tmp451=Cyc_Absyn_qvar_cmp(_tmp433->name,_tmp436->name);
if(_tmp451 != 0)return _tmp451;{
int _tmp452=Cyc_Absyn_qvar_cmp(_tmp434->name,_tmp437->name);
if(_tmp452 != 0)return _tmp452;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp438,_tmp435);};};}else{goto _LL3D6;}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}else{switch(*((int*)_tmp404.f2)){case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}}case 5: switch(*((int*)_tmp404.f2)){case 5: _LL3B0: _tmp427=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp404.f1)->f1).elt_typ;_tmp428=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp404.f1)->f1).elt_tq;_tmp429=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp404.f1)->f1).ptr_atts).rgn;_tmp42A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp404.f1)->f1).ptr_atts).nullable;_tmp42B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp404.f1)->f1).ptr_atts).bounds;_tmp42C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp404.f1)->f1).ptr_atts).zero_term;_tmp42D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp404.f2)->f1).elt_typ;_tmp42E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp404.f2)->f1).elt_tq;_tmp42F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp404.f2)->f1).ptr_atts).rgn;_tmp430=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp404.f2)->f1).ptr_atts).nullable;_tmp431=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp404.f2)->f1).ptr_atts).bounds;_tmp432=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp404.f2)->f1).ptr_atts).zero_term;_LL3B1: {
# 1951
int _tmp453=Cyc_Tcutil_typecmp(_tmp42D,_tmp427);
if(_tmp453 != 0)return _tmp453;{
int _tmp454=Cyc_Tcutil_typecmp(_tmp42F,_tmp429);
if(_tmp454 != 0)return _tmp454;{
int _tmp455=Cyc_Tcutil_tqual_cmp(_tmp42E,_tmp428);
if(_tmp455 != 0)return _tmp455;{
int _tmp456=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp431,_tmp42B);
if(_tmp456 != 0)return _tmp456;{
int _tmp457=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp432,_tmp42C);
if(_tmp457 != 0)return _tmp457;
{void*_tmp458=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp431);void*_tmp459=_tmp458;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp459)->tag == 0){_LL3DF: _LL3E0:
 return 0;}else{_LL3E1: _LL3E2:
 goto _LL3DE;}_LL3DE:;}
# 1965
return((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp430,_tmp42A);};};};};}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 6: switch(*((int*)_tmp404.f2)){case 6: _LL3B2: _tmp423=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp404.f1)->f1;_tmp424=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp404.f1)->f2;_tmp425=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp404.f2)->f1;_tmp426=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp404.f2)->f2;_LL3B3:
# 1968
 if(_tmp425 != _tmp423)return Cyc_Core_intcmp((int)((unsigned int)_tmp425),(int)((unsigned int)_tmp423));
if(_tmp426 != _tmp424)return Cyc_Core_intcmp((int)((unsigned int)_tmp426),(int)((unsigned int)_tmp424));
return 0;case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 7: switch(*((int*)_tmp404.f2)){case 7: _LL3B4: _tmp421=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp404.f1)->f1;_tmp422=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp404.f2)->f1;_LL3B5:
# 1973
 if(_tmp421 == _tmp422)return 0;else{
if(_tmp422 == 0)return - 1;else{
if(_tmp422 == 1  && _tmp421 == 0)return - 1;else{
return 1;}}}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 8: switch(*((int*)_tmp404.f2)){case 8: _LL3B6: _tmp419=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp404.f1)->f1).elt_type;_tmp41A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp404.f1)->f1).tq;_tmp41B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp404.f1)->f1).num_elts;_tmp41C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp404.f1)->f1).zero_term;_tmp41D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp404.f2)->f1).elt_type;_tmp41E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp404.f2)->f1).tq;_tmp41F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp404.f2)->f1).num_elts;_tmp420=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp404.f2)->f1).zero_term;_LL3B7: {
# 1980
int _tmp45A=Cyc_Tcutil_tqual_cmp(_tmp41E,_tmp41A);
if(_tmp45A != 0)return _tmp45A;{
int _tmp45B=Cyc_Tcutil_typecmp(_tmp41D,_tmp419);
if(_tmp45B != 0)return _tmp45B;{
int _tmp45C=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp41C,_tmp420);
if(_tmp45C != 0)return _tmp45C;
if(_tmp41B == _tmp41F)return 0;
if(_tmp41B == 0  || _tmp41F == 0)
({void*_tmp45D=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp45E="missing expression in array index";_tag_dyneither(_tmp45E,sizeof(char),34);}),_tag_dyneither(_tmp45D,sizeof(void*),0));});
# 1990
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp41B,_tmp41F);};};}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 9: switch(*((int*)_tmp404.f2)){case 9: _LL3B8: _LL3B9:
# 1993
({void*_tmp45F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp460="typecmp: function types not handled";_tag_dyneither(_tmp460,sizeof(char),36);}),_tag_dyneither(_tmp45F,sizeof(void*),0));});case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 10: switch(*((int*)_tmp404.f2)){case 10: _LL3BA: _tmp417=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp404.f1)->f1;_tmp418=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp404.f2)->f1;_LL3BB:
# 1996
 return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp418,_tmp417);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 12: switch(*((int*)_tmp404.f2)){case 12: _LL3BC: _tmp413=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp404.f1)->f1;_tmp414=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp404.f1)->f2;_tmp415=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp404.f2)->f1;_tmp416=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp404.f2)->f2;_LL3BD:
# 1999
 if(_tmp415 != _tmp413){
if(_tmp415 == Cyc_Absyn_StructA)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp416,_tmp414);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 15: switch(*((int*)_tmp404.f2)){case 15: _LL3BE: _tmp411=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp404.f1)->f1;_tmp412=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp404.f2)->f1;_LL3BF:
# 2004
 return Cyc_Tcutil_typecmp(_tmp411,_tmp412);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 16: switch(*((int*)_tmp404.f2)){case 16: _LL3C0: _tmp40D=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp404.f1)->f1;_tmp40E=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp404.f1)->f2;_tmp40F=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp404.f2)->f1;_tmp410=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp404.f2)->f2;_LL3C1: {
# 2006
int _tmp461=Cyc_Tcutil_typecmp(_tmp40D,_tmp40F);
if(_tmp461 != 0)return _tmp461;else{
return Cyc_Tcutil_typecmp(_tmp40E,_tmp410);}}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 19: switch(*((int*)_tmp404.f2)){case 19: _LL3C2: _tmp40B=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp404.f1)->f1;_tmp40C=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp404.f2)->f1;_LL3C3:
 return Cyc_Tcutil_typecmp(_tmp40B,_tmp40C);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 18: switch(*((int*)_tmp404.f2)){case 18: _LL3C4: _tmp409=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp404.f1)->f1;_tmp40A=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp404.f2)->f1;_LL3C5:
 _tmp407=_tmp409;_tmp408=_tmp40A;goto _LL3C7;case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 27: switch(*((int*)_tmp404.f2)){case 27: _LL3C6: _tmp407=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp404.f1)->f1;_tmp408=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp404.f2)->f1;_LL3C7:
# 2012
 return Cyc_Evexp_const_exp_cmp(_tmp407,_tmp408);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 28: switch(*((int*)_tmp404.f2)){case 28: _LL3C8: _tmp405=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp404.f1)->f1;_tmp406=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp404.f2)->f1;_LL3C9:
# 2014
 return Cyc_strcmp((struct _dyneither_ptr)_tmp405,(struct _dyneither_ptr)_tmp406);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 24: _LL3CA: _LL3CB:
 goto _LL3CD;default: if(((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp404.f2)->tag == 24){_LL3CC: _LL3CD:
 goto _LL3CF;}else{switch(*((int*)_tmp404.f1)){case 23: _LL3CE: _LL3CF:
 goto _LL3D1;case 25: _LL3D0: _LL3D1:
 goto _LL3D3;default: switch(*((int*)_tmp404.f2)){case 25: _LL3D2: _LL3D3:
 goto _LL3D5;case 23: _LL3D4: _LL3D5:
({void*_tmp462=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp463="typecmp: effects not handled";_tag_dyneither(_tmp463,sizeof(char),29);}),_tag_dyneither(_tmp462,sizeof(void*),0));});default: _LL3D6: _LL3D7:
({void*_tmp464=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp465="Unmatched case in typecmp";_tag_dyneither(_tmp465,sizeof(char),26);}),_tag_dyneither(_tmp464,sizeof(void*),0));});}}}}_LL3A1:;};};}
# 2025
int Cyc_Tcutil_is_arithmetic_type(void*t){
void*_tmp467=Cyc_Tcutil_compress(t);void*_tmp468=_tmp467;switch(*((int*)_tmp468)){case 6: _LL3E4: _LL3E5:
 goto _LL3E7;case 7: _LL3E6: _LL3E7:
 goto _LL3E9;case 13: _LL3E8: _LL3E9:
 goto _LL3EB;case 14: _LL3EA: _LL3EB:
 return 1;default: _LL3EC: _LL3ED:
 return 0;}_LL3E3:;}
# 2037
int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp469=({struct _tuple0 _tmp46D;_tmp46D.f1=t1;_tmp46D.f2=t2;_tmp46D;});struct _tuple0 _tmp46A=_tmp469;int _tmp46B;int _tmp46C;switch(*((int*)_tmp46A.f1)){case 7: switch(*((int*)_tmp46A.f2)){case 7: _LL3EF: _tmp46B=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp46A.f1)->f1;_tmp46C=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp46A.f2)->f1;_LL3F0:
# 2042
 return _tmp46C == 0  && _tmp46B != 0  || (_tmp46C == 1  && _tmp46B != 0) && _tmp46B != 1;case 6: _LL3F1: _LL3F2:
 goto _LL3F4;case 19: _LL3F3: _LL3F4:
 return 1;default: goto _LL40F;}case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46A.f1)->f2){case Cyc_Absyn_LongLong_sz: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46A.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46A.f2)->f2 == Cyc_Absyn_LongLong_sz){_LL3F5: _LL3F6:
 return 0;}else{goto _LL3F7;}}else{_LL3F7: _LL3F8:
 return 1;}case Cyc_Absyn_Long_sz: switch(*((int*)_tmp46A.f2)){case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46A.f2)->f2){case Cyc_Absyn_Int_sz: _LL3F9: _LL3FA:
# 2049
 goto _LL3FC;case Cyc_Absyn_Short_sz: _LL401: _LL402:
# 2054
 goto _LL404;case Cyc_Absyn_Char_sz: _LL407: _LL408:
# 2057
 goto _LL40A;default: goto _LL40F;}case 7: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp46A.f2)->f1 == 0){_LL3FD: _LL3FE:
# 2052
 goto _LL400;}else{goto _LL40F;}default: goto _LL40F;}case Cyc_Absyn_Int_sz: switch(*((int*)_tmp46A.f2)){case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46A.f2)->f2){case Cyc_Absyn_Long_sz: _LL3FB: _LL3FC:
# 2050
 return 0;case Cyc_Absyn_Short_sz: _LL403: _LL404:
# 2055
 goto _LL406;case Cyc_Absyn_Char_sz: _LL409: _LL40A:
# 2058
 goto _LL40C;default: goto _LL40F;}case 7: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp46A.f2)->f1 == 0){_LL3FF: _LL400:
# 2053
 goto _LL402;}else{goto _LL40F;}default: goto _LL40F;}case Cyc_Absyn_Short_sz: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46A.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46A.f2)->f2 == Cyc_Absyn_Char_sz){_LL40B: _LL40C:
# 2059
 goto _LL40E;}else{goto _LL40F;}}else{goto _LL40F;}default: goto _LL40F;}case 19: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46A.f2)->tag == 6)switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46A.f2)->f2){case Cyc_Absyn_Short_sz: _LL405: _LL406:
# 2056
 goto _LL408;case Cyc_Absyn_Char_sz: _LL40D: _LL40E:
# 2060
 return 1;default: goto _LL40F;}else{goto _LL40F;}default: _LL40F: _LL410:
# 2062
 return 0;}_LL3EE:;};}
# 2068
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
# 2071
struct _RegionHandle _tmp46E=_new_region("r");struct _RegionHandle*r=& _tmp46E;_push_region(r);{
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0  || 
Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
max_arith_type=({struct Cyc_Core_Opt*_tmp46F=_region_malloc(r,sizeof(*_tmp46F));_tmp46F->v=t1;_tmp46F;});}}}
# 2081
if(max_arith_type != 0){
if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v)){
int _tmp470=0;_npop_handler(0);return _tmp470;}}}
# 2086
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){
({struct Cyc_String_pa_PrintArg_struct _tmp474;_tmp474.tag=0;_tmp474.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp473;_tmp473.tag=0;_tmp473.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp471[2]={& _tmp473,& _tmp474};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)el->hd)->loc,({const char*_tmp472="type mismatch: expecting %s but found %s";_tag_dyneither(_tmp472,sizeof(char),41);}),_tag_dyneither(_tmp471,sizeof(void*),2));});});});{
int _tmp475=0;_npop_handler(0);return _tmp475;};}}}{
# 2092
int _tmp476=1;_npop_handler(0);return _tmp476;};
# 2071
;_pop_region(r);}
# 2097
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_typ(te,e)){
void*_tmp477=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp478=_tmp477;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp478)->tag == 5){_LL412: _LL413:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);goto _LL411;}else{_LL414: _LL415:
 return 0;}_LL411:;}
# 2103
return 1;}
# 2106
int Cyc_Tcutil_is_integral_type(void*t){
void*_tmp479=Cyc_Tcutil_compress(t);void*_tmp47A=_tmp479;switch(*((int*)_tmp47A)){case 6: _LL417: _LL418:
 goto _LL41A;case 19: _LL419: _LL41A:
 goto _LL41C;case 13: _LL41B: _LL41C:
 goto _LL41E;case 14: _LL41D: _LL41E:
 return 1;default: _LL41F: _LL420:
 return 0;}_LL416:;}
# 2117
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
return 1;
# 2121
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
({void*_tmp47B=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp47C="integral size mismatch; conversion supplied";_tag_dyneither(_tmp47C,sizeof(char),44);}),_tag_dyneither(_tmp47B,sizeof(void*),0));});
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 2127
return 0;}
# 2131
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ))
return 1;
# 2135
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_sint_typ))
({void*_tmp47D=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp47E="integral size mismatch; conversion supplied";_tag_dyneither(_tmp47E,sizeof(char),44);}),_tag_dyneither(_tmp47D,sizeof(void*),0));});
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 2141
return 0;}
# 2146
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);
# 2154
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
# 2156
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp47F=({struct _tuple0 _tmp498;_tmp498.f1=t1;_tmp498.f2=t2;_tmp498;});struct _tuple0 _tmp480=_tmp47F;void*_tmp481;struct Cyc_Absyn_Tqual _tmp482;struct Cyc_Absyn_Exp*_tmp483;union Cyc_Absyn_Constraint*_tmp484;void*_tmp485;struct Cyc_Absyn_Tqual _tmp486;struct Cyc_Absyn_Exp*_tmp487;union Cyc_Absyn_Constraint*_tmp488;struct Cyc_Absyn_PtrInfo _tmp489;struct Cyc_Absyn_PtrInfo _tmp48A;switch(*((int*)_tmp480.f1)){case 5: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp480.f2)->tag == 5){_LL422: _tmp489=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp480.f1)->f1;_tmp48A=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp480.f2)->f1;_LL423: {
# 2160
int okay=1;
# 2162
if(!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp489.ptr_atts).nullable,(_tmp48A.ptr_atts).nullable))
# 2165
okay=!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp489.ptr_atts).nullable);
# 2167
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp489.ptr_atts).bounds,(_tmp48A.ptr_atts).bounds)){
# 2170
struct _tuple0 _tmp48B=({struct _tuple0 _tmp492;_tmp492.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp489.ptr_atts).bounds);_tmp492.f2=
((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp48A.ptr_atts).bounds);_tmp492;});
# 2170
struct _tuple0 _tmp48C=_tmp48B;struct Cyc_Absyn_Exp*_tmp48D;struct Cyc_Absyn_Exp*_tmp48E;struct Cyc_Absyn_Exp*_tmp48F;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp48C.f1)->tag == 1){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp48C.f2)->tag == 0){_LL42B: _LL42C:
# 2172
 goto _LL42E;}else{_LL42F: _tmp48E=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp48C.f1)->f1;_tmp48F=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp48C.f2)->f1;_LL430:
# 2175
 okay=okay  && Cyc_Evexp_lte_const_exp(_tmp48F,_tmp48E);
# 2179
if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp48A.ptr_atts).zero_term))
({void*_tmp490=0;Cyc_Tcutil_warn(loc,({const char*_tmp491="implicit cast to shorter array";_tag_dyneither(_tmp491,sizeof(char),31);}),_tag_dyneither(_tmp490,sizeof(void*),0));});
goto _LL42A;}}else{if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp48C.f2)->tag == 0){_LL42D: _LL42E:
# 2173
 okay=1;goto _LL42A;}else{_LL431: _tmp48D=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp48C.f2)->f1;_LL432:
# 2184
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp489.ptr_atts).zero_term) && 
Cyc_Tcutil_is_bound_one((_tmp48A.ptr_atts).bounds))
goto _LL42A;
okay=0;
goto _LL42A;}}_LL42A:;}
# 2193
okay=okay  && (!(_tmp489.elt_tq).real_const  || (_tmp48A.elt_tq).real_const);
# 2196
if(!Cyc_Tcutil_unify((_tmp489.ptr_atts).rgn,(_tmp48A.ptr_atts).rgn)){
if(Cyc_Tcenv_region_outlives(te,(_tmp489.ptr_atts).rgn,(_tmp48A.ptr_atts).rgn)){
if(Cyc_Tcutil_warn_region_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp496;_tmp496.tag=0;_tmp496.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2201
Cyc_Absynpp_typ2string((_tmp48A.ptr_atts).rgn));({struct Cyc_String_pa_PrintArg_struct _tmp495;_tmp495.tag=0;_tmp495.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2200
Cyc_Absynpp_typ2string((_tmp489.ptr_atts).rgn));({void*_tmp493[2]={& _tmp495,& _tmp496};Cyc_Tcutil_warn(loc,({const char*_tmp494="implicit cast form region %s to region %s";_tag_dyneither(_tmp494,sizeof(char),42);}),_tag_dyneither(_tmp493,sizeof(void*),2));});});});}else{
# 2202
okay=0;}}
# 2206
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp489.ptr_atts).zero_term,(_tmp48A.ptr_atts).zero_term) || 
# 2209
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,(_tmp489.ptr_atts).zero_term) && (_tmp48A.elt_tq).real_const);{
# 2217
int _tmp497=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp48A.ptr_atts).bounds,Cyc_Absyn_bounds_one_conref) && !
# 2220
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp48A.ptr_atts).zero_term);
# 2224
okay=okay  && (Cyc_Tcutil_unify(_tmp489.elt_typ,_tmp48A.elt_typ) || 
(_tmp497  && ((_tmp48A.elt_tq).real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp48A.elt_typ)))) && Cyc_Tcutil_ptrsubtype(te,0,_tmp489.elt_typ,_tmp48A.elt_typ));
# 2227
return okay;};}}else{goto _LL428;}case 8: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp480.f2)->tag == 8){_LL424: _tmp481=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp480.f1)->f1).elt_type;_tmp482=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp480.f1)->f1).tq;_tmp483=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp480.f1)->f1).num_elts;_tmp484=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp480.f1)->f1).zero_term;_tmp485=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp480.f2)->f1).elt_type;_tmp486=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp480.f2)->f1).tq;_tmp487=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp480.f2)->f1).num_elts;_tmp488=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp480.f2)->f1).zero_term;_LL425: {
# 2231
int okay;
# 2233
okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp484,_tmp488) && (
(_tmp483 != 0  && _tmp487 != 0) && Cyc_Evexp_same_const_exp(_tmp483,_tmp487));
# 2236
return(okay  && Cyc_Tcutil_unify(_tmp481,_tmp485)) && (!_tmp482.real_const  || _tmp486.real_const);}}else{goto _LL428;}case 19: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp480.f2)->tag == 6){_LL426: _LL427:
# 2238
 return 0;}else{goto _LL428;}default: _LL428: _LL429:
# 2240
 return Cyc_Tcutil_unify(t1,t2);}_LL421:;};}
# 2244
int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp499=Cyc_Tcutil_compress(t);void*_tmp49A=_tmp499;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49A)->tag == 5){_LL434: _LL435:
 return 1;}else{_LL436: _LL437:
 return 0;}_LL433:;}
# 2250
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp49B=Cyc_Tcutil_compress(t);void*_tmp49C=_tmp49B;void*_tmp49D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49C)->tag == 5){_LL439: _tmp49D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49C)->f1).elt_typ;_LL43A:
 return _tmp49D;}else{_LL43B: _LL43C:
({void*_tmp49E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp49F="pointer_elt_type";_tag_dyneither(_tmp49F,sizeof(char),17);}),_tag_dyneither(_tmp49E,sizeof(void*),0));});}_LL438:;}
# 2256
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp4A0=Cyc_Tcutil_compress(t);void*_tmp4A1=_tmp4A0;struct Cyc_Absyn_PtrAtts*_tmp4A2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A1)->tag == 5){_LL43E: _tmp4A2=(struct Cyc_Absyn_PtrAtts*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A1)->f1).ptr_atts;_LL43F:
 return _tmp4A2->rgn;}else{_LL440: _LL441:
({void*_tmp4A3=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4A4="pointer_elt_type";_tag_dyneither(_tmp4A4,sizeof(char),17);}),_tag_dyneither(_tmp4A3,sizeof(void*),0));});}_LL43D:;}
# 2263
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp4A5=Cyc_Tcutil_compress(t);void*_tmp4A6=_tmp4A5;void*_tmp4A7;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A6)->tag == 5){_LL443: _tmp4A7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A6)->f1).ptr_atts).rgn;_LL444:
# 2266
*rgn=_tmp4A7;
return 1;}else{_LL445: _LL446:
 return 0;}_LL442:;}
# 2273
int Cyc_Tcutil_is_boxed(void*t){
return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;}
# 2277
int Cyc_Tcutil_is_dyneither_ptr(void*t){
void*_tmp4A8=Cyc_Tcutil_compress(t);void*_tmp4A9=_tmp4A8;union Cyc_Absyn_Constraint*_tmp4AA;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A9)->tag == 5){_LL448: _tmp4AA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A9)->f1).ptr_atts).bounds;_LL449:
# 2280
 return((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp4AA)== (void*)& Cyc_Absyn_DynEither_b_val;}else{_LL44A: _LL44B:
 return 0;}_LL447:;}
# 2286
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp4AB=e->r;void*_tmp4AC=_tmp4AB;void*_tmp4AD;struct Cyc_Absyn_Exp*_tmp4AE;struct _dyneither_ptr _tmp4AF;switch(*((int*)_tmp4AC)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4AC)->f1).Wchar_c).tag){case 5: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4AC)->f1).Int_c).val).f2 == 0){_LL44D: _LL44E:
 goto _LL450;}else{goto _LL459;}case 2: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4AC)->f1).Char_c).val).f2 == 0){_LL44F: _LL450:
 goto _LL452;}else{goto _LL459;}case 4: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4AC)->f1).Short_c).val).f2 == 0){_LL451: _LL452:
 goto _LL454;}else{goto _LL459;}case 6: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4AC)->f1).LongLong_c).val).f2 == 0){_LL453: _LL454:
# 2292
 return 1;}else{goto _LL459;}case 3: _LL455: _tmp4AF=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4AC)->f1).Wchar_c).val;_LL456: {
# 2294
unsigned long _tmp4B0=Cyc_strlen((struct _dyneither_ptr)_tmp4AF);
int i=0;
if(_tmp4B0 >= 2  && *((const char*)_check_dyneither_subscript(_tmp4AF,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp4AF,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmp4AF,sizeof(char),1))== 'x'  && _tmp4B0 >= 3) && *((const char*)_check_dyneither_subscript(_tmp4AF,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp4B0;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp4AF,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 2304
return 0;}}default: goto _LL459;}case 13: _LL457: _tmp4AD=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4AC)->f1;_tmp4AE=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4AC)->f2;_LL458:
 return Cyc_Tcutil_is_zero(_tmp4AE) && Cyc_Tcutil_admits_zero(_tmp4AD);default: _LL459: _LL45A:
 return 0;}_LL44C:;}
# 2310
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
# 2317
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 2322
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 2327
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
# 2334
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 2339
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 2344
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp4B1=ka;enum Cyc_Absyn_KindQual _tmp4B2;enum Cyc_Absyn_AliasQual _tmp4B3;_LL45C: _tmp4B2=_tmp4B1->kind;_tmp4B3=_tmp4B1->aliasqual;_LL45D:;
{enum Cyc_Absyn_AliasQual _tmp4B4=_tmp4B3;switch(_tmp4B4){case Cyc_Absyn_Aliasable: _LL45F: _LL460: {
# 2348
enum Cyc_Absyn_KindQual _tmp4B5=_tmp4B2;switch(_tmp4B5){case Cyc_Absyn_AnyKind: _LL466: _LL467:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LL468: _LL469:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL46A: _LL46B:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL46C: _LL46D:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL46E: _LL46F:
 return& Cyc_Tcutil_eko;default: _LL470: _LL471:
 return& Cyc_Tcutil_iko;}_LL465:;}case Cyc_Absyn_Unique: _LL461: _LL462:
# 2357
{enum Cyc_Absyn_KindQual _tmp4B6=_tmp4B2;switch(_tmp4B6){case Cyc_Absyn_AnyKind: _LL473: _LL474:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL475: _LL476:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL477: _LL478:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL479: _LL47A:
 return& Cyc_Tcutil_urko;default: _LL47B: _LL47C:
 goto _LL472;}_LL472:;}
# 2364
goto _LL45E;default: _LL463: _LL464:
# 2366
{enum Cyc_Absyn_KindQual _tmp4B7=_tmp4B2;switch(_tmp4B7){case Cyc_Absyn_AnyKind: _LL47E: _LL47F:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL480: _LL481:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL482: _LL483:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL484: _LL485:
 return& Cyc_Tcutil_trko;default: _LL486: _LL487:
 goto _LL47D;}_LL47D:;}
# 2373
goto _LL45E;}_LL45E:;}
# 2375
({struct Cyc_String_pa_PrintArg_struct _tmp4BA;_tmp4BA.tag=0;_tmp4BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka));({void*_tmp4B8[1]={& _tmp4BA};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4B9="kind_to_opt: bad kind %s\n";_tag_dyneither(_tmp4B9,sizeof(char),26);}),_tag_dyneither(_tmp4B8,sizeof(void*),1));});});}
# 2378
static void**Cyc_Tcutil_kind_to_b(struct Cyc_Absyn_Kind*ka){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ab_v={0,& Cyc_Tcutil_ak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct mb_v={0,& Cyc_Tcutil_mk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct bb_v={0,& Cyc_Tcutil_bk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rb_v={0,& Cyc_Tcutil_rk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct eb_v={0,& Cyc_Tcutil_ek};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ib_v={0,& Cyc_Tcutil_ik};
# 2386
static void*ab=(void*)& ab_v;
static void*mb=(void*)& mb_v;
static void*bb=(void*)& bb_v;
static void*rb=(void*)& rb_v;
static void*eb=(void*)& eb_v;
static void*ib=(void*)& ib_v;
# 2393
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tab_v={0,& Cyc_Tcutil_tak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tmb_v={0,& Cyc_Tcutil_tmk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tbb_v={0,& Cyc_Tcutil_tbk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct trb_v={0,& Cyc_Tcutil_trk};
# 2398
static void*tab=(void*)& tab_v;
static void*tmb=(void*)& tmb_v;
static void*tbb=(void*)& tbb_v;
static void*trb=(void*)& trb_v;
# 2403
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct uab_v={0,& Cyc_Tcutil_uak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct umb_v={0,& Cyc_Tcutil_umk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ubb_v={0,& Cyc_Tcutil_ubk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct urb_v={0,& Cyc_Tcutil_urk};
# 2408
static void*uab=(void*)& uab_v;
static void*umb=(void*)& umb_v;
static void*ubb=(void*)& ubb_v;
static void*urb=(void*)& urb_v;
# 2413
struct Cyc_Absyn_Kind*_tmp4BB=ka;enum Cyc_Absyn_KindQual _tmp4BC;enum Cyc_Absyn_AliasQual _tmp4BD;_LL489: _tmp4BC=_tmp4BB->kind;_tmp4BD=_tmp4BB->aliasqual;_LL48A:;
{enum Cyc_Absyn_AliasQual _tmp4BE=_tmp4BD;switch(_tmp4BE){case Cyc_Absyn_Aliasable: _LL48C: _LL48D: {
# 2416
enum Cyc_Absyn_KindQual _tmp4BF=_tmp4BC;switch(_tmp4BF){case Cyc_Absyn_AnyKind: _LL493: _LL494:
 return& ab;case Cyc_Absyn_MemKind: _LL495: _LL496:
 return& mb;case Cyc_Absyn_BoxKind: _LL497: _LL498:
 return& bb;case Cyc_Absyn_RgnKind: _LL499: _LL49A:
 return& rb;case Cyc_Absyn_EffKind: _LL49B: _LL49C:
 return& eb;default: _LL49D: _LL49E:
 return& ib;}_LL492:;}case Cyc_Absyn_Unique: _LL48E: _LL48F:
# 2425
{enum Cyc_Absyn_KindQual _tmp4C0=_tmp4BC;switch(_tmp4C0){case Cyc_Absyn_AnyKind: _LL4A0: _LL4A1:
 return& uab;case Cyc_Absyn_MemKind: _LL4A2: _LL4A3:
 return& umb;case Cyc_Absyn_BoxKind: _LL4A4: _LL4A5:
 return& ubb;case Cyc_Absyn_RgnKind: _LL4A6: _LL4A7:
 return& urb;default: _LL4A8: _LL4A9:
 goto _LL49F;}_LL49F:;}
# 2432
goto _LL48B;default: _LL490: _LL491:
# 2434
{enum Cyc_Absyn_KindQual _tmp4C1=_tmp4BC;switch(_tmp4C1){case Cyc_Absyn_AnyKind: _LL4AB: _LL4AC:
 return& tab;case Cyc_Absyn_MemKind: _LL4AD: _LL4AE:
 return& tmb;case Cyc_Absyn_BoxKind: _LL4AF: _LL4B0:
 return& tbb;case Cyc_Absyn_RgnKind: _LL4B1: _LL4B2:
 return& trb;default: _LL4B3: _LL4B4:
 goto _LL4AA;}_LL4AA:;}
# 2441
goto _LL48B;}_LL48B:;}
# 2443
({struct Cyc_String_pa_PrintArg_struct _tmp4C4;_tmp4C4.tag=0;_tmp4C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka));({void*_tmp4C2[1]={& _tmp4C4};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4C3="kind_to_b: bad kind %s\n";_tag_dyneither(_tmp4C3,sizeof(char),24);}),_tag_dyneither(_tmp4C2,sizeof(void*),1));});});}
# 2446
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return*Cyc_Tcutil_kind_to_b(k);}
# 2449
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
# 2451
return({struct Cyc_Core_Opt*_tmp4D3=_cycalloc(sizeof(*_tmp4D3));_tmp4D3->v=Cyc_Tcutil_kind_to_bound(k);_tmp4D3;});}
# 2456
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){
e1->r=(void*)({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4D4=_cycalloc(sizeof(*_tmp4D4));_tmp4D4[0]=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp4D5;_tmp4D5.tag=0;_tmp4D5.f1=Cyc_Absyn_Null_c;_tmp4D5;});_tmp4D4;});{
struct Cyc_Core_Opt*_tmp4D6=Cyc_Tcenv_lookup_opt_type_vars(te);
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4D7=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4DC=_cycalloc(sizeof(*_tmp4DC));_tmp4DC[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp4DD;_tmp4DD.tag=5;_tmp4DD.f1=({struct Cyc_Absyn_PtrInfo _tmp4DE;_tmp4DE.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp4D6);_tmp4DE.elt_tq=
Cyc_Absyn_empty_tqual(0);_tmp4DE.ptr_atts=({(_tmp4DE.ptr_atts).rgn=
Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp4D6);(_tmp4DE.ptr_atts).nullable=Cyc_Absyn_true_conref;(_tmp4DE.ptr_atts).bounds=
# 2464
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp4DE.ptr_atts).zero_term=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp4DE.ptr_atts).ptrloc=0;_tmp4DE.ptr_atts;});_tmp4DE;});_tmp4DD;});_tmp4DC;});
e1->topt=(void*)_tmp4D7;{
int bogus=0;
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
if(bogus != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp4DB;_tmp4DB.tag=0;_tmp4DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(e1->loc));({struct Cyc_String_pa_PrintArg_struct _tmp4DA;_tmp4DA.tag=0;_tmp4DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));({void*_tmp4D8[2]={& _tmp4DA,& _tmp4DB};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4D9="zero_to_null resulted in an alias coercion on %s at %s\n";_tag_dyneither(_tmp4D9,sizeof(char),56);}),_tag_dyneither(_tmp4D8,sizeof(void*),2));});});});
return retv;};};}
# 2474
return 0;}
# 2477
struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
enum Cyc_Absyn_Coercion _tmp4DF=c;switch(_tmp4DF){case Cyc_Absyn_Unknown_coercion: _LL4B6: _LL4B7:
 return({const char*_tmp4E0="unknown";_tag_dyneither(_tmp4E0,sizeof(char),8);});case Cyc_Absyn_No_coercion: _LL4B8: _LL4B9:
 return({const char*_tmp4E1="no coercion";_tag_dyneither(_tmp4E1,sizeof(char),12);});case Cyc_Absyn_NonNull_to_Null: _LL4BA: _LL4BB:
 return({const char*_tmp4E2="null check";_tag_dyneither(_tmp4E2,sizeof(char),11);});default: _LL4BC: _LL4BD:
 return({const char*_tmp4E3="other coercion";_tag_dyneither(_tmp4E3,sizeof(char),15);});}_LL4B5:;}
# 2486
int Cyc_Tcutil_warn_alias_coerce=0;
# 2492
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0,& Cyc_Tcutil_rk};
# 2496
static int counter=0;
struct _tuple2*v=({struct _tuple2*_tmp4F7=_cycalloc(sizeof(*_tmp4F7));_tmp4F7->f1=Cyc_Absyn_Loc_n;_tmp4F7->f2=({struct _dyneither_ptr*_tmp4F8=_cycalloc(sizeof(*_tmp4F8));_tmp4F8[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp4FB;_tmp4FB.tag=1;_tmp4FB.f1=(unsigned long)counter ++;({void*_tmp4F9[1]={& _tmp4FB};Cyc_aprintf(({const char*_tmp4FA="__aliasvar%d";_tag_dyneither(_tmp4FA,sizeof(char),13);}),_tag_dyneither(_tmp4F9,sizeof(void*),1));});});_tmp4F8;});_tmp4F7;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(v,e_typ,e);
struct Cyc_Absyn_Exp*ve=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4F5=_cycalloc(sizeof(*_tmp4F5));_tmp4F5[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp4F6;_tmp4F6.tag=4;_tmp4F6.f1=vd;_tmp4F6;});_tmp4F5;}),e->loc);
# 2505
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 2507
{void*_tmp4E4=Cyc_Tcutil_compress(e_typ);void*_tmp4E5=_tmp4E4;void*_tmp4E6;struct Cyc_Absyn_Tqual _tmp4E7;void*_tmp4E8;union Cyc_Absyn_Constraint*_tmp4E9;union Cyc_Absyn_Constraint*_tmp4EA;union Cyc_Absyn_Constraint*_tmp4EB;struct Cyc_Absyn_PtrLoc*_tmp4EC;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E5)->tag == 5){_LL4BF: _tmp4E6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E5)->f1).elt_typ;_tmp4E7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E5)->f1).elt_tq;_tmp4E8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E5)->f1).ptr_atts).rgn;_tmp4E9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E5)->f1).ptr_atts).nullable;_tmp4EA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E5)->f1).ptr_atts).bounds;_tmp4EB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E5)->f1).ptr_atts).zero_term;_tmp4EC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E5)->f1).ptr_atts).ptrloc;_LL4C0:
# 2509
{void*_tmp4ED=Cyc_Tcutil_compress(_tmp4E8);void*_tmp4EE=_tmp4ED;void**_tmp4EF;struct Cyc_Core_Opt*_tmp4F0;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4EE)->tag == 1){_LL4C4: _tmp4EF=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4EE)->f2;_tmp4F0=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4EE)->f4;_LL4C5: {
# 2511
struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4F1=({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4F2=_cycalloc(sizeof(*_tmp4F2));_tmp4F2[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4F3;_tmp4F3.tag=2;_tmp4F3.f1=tv;_tmp4F3;});_tmp4F2;});
*_tmp4EF=(void*)_tmp4F1;
goto _LL4C3;}}else{_LL4C6: _LL4C7:
 goto _LL4C3;}_LL4C3:;}
# 2516
goto _LL4BE;}else{_LL4C1: _LL4C2:
 goto _LL4BE;}_LL4BE:;}
# 2520
e->topt=0;
vd->initializer=0;{
# 2524
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 2526
return({struct _tuple15 _tmp4F4;_tmp4F4.f1=d;_tmp4F4.f2=ve;_tmp4F4;});};}
# 2531
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ,void*wants_typ,unsigned int loc){
# 2534
struct _RegionHandle _tmp4FD=_new_region("r");struct _RegionHandle*r=& _tmp4FD;_push_region(r);
if((Cyc_Tcutil_is_noalias_path(r,e) && Cyc_Tcutil_is_noalias_pointer(e_typ,0)) && 
Cyc_Tcutil_is_pointer_type(e_typ)){
# 2539
void*_tmp4FE=Cyc_Tcutil_compress(wants_typ);void*_tmp4FF=_tmp4FE;void*_tmp500;switch(*((int*)_tmp4FF)){case 5: _LL4C9: _tmp500=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FF)->f1).ptr_atts).rgn;_LL4CA: {
# 2541
void*_tmp501=Cyc_Tcutil_compress(_tmp500);void*_tmp502=_tmp501;if(((struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp502)->tag == 20){_LL4D0: _LL4D1: {
int _tmp503=0;_npop_handler(0);return _tmp503;}}else{_LL4D2: _LL4D3: {
# 2544
struct Cyc_Absyn_Kind*_tmp504=Cyc_Tcutil_typ_kind(_tmp500);
int _tmp505=_tmp504->kind == Cyc_Absyn_RgnKind  && _tmp504->aliasqual == Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp505;}}_LL4CF:;}case 17: _LL4CB: _LL4CC:
# 2548
({struct Cyc_String_pa_PrintArg_struct _tmp509;_tmp509.tag=0;_tmp509.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(loc));({struct Cyc_String_pa_PrintArg_struct _tmp508;_tmp508.tag=0;_tmp508.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_compress(wants_typ)));({void*_tmp506[2]={& _tmp508,& _tmp509};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp507="got typedef %s in can_insert_alias at %s\n";_tag_dyneither(_tmp507,sizeof(char),42);}),_tag_dyneither(_tmp506,sizeof(void*),2));});});});default: _LL4CD: _LL4CE: {
int _tmp50A=0;_npop_handler(0);return _tmp50A;}}_LL4C8:;}{
# 2553
int _tmp50B=0;_npop_handler(0);return _tmp50B;};
# 2535
;_pop_region(r);}
# 2557
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 2562
if(Cyc_Tcutil_unify(t1,t2))return 1;
# 2564
if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
# 2566
if(Cyc_Tcutil_will_lose_precision(t1,t2))
({struct Cyc_String_pa_PrintArg_struct _tmp50F;_tmp50F.tag=0;_tmp50F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_PrintArg_struct _tmp50E;_tmp50E.tag=0;_tmp50E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));({void*_tmp50C[2]={& _tmp50E,& _tmp50F};Cyc_Tcutil_warn(e->loc,({const char*_tmp50D="integral size mismatch; %s -> %s conversion supplied";_tag_dyneither(_tmp50D,sizeof(char),53);}),_tag_dyneither(_tmp50C,sizeof(void*),2));});});});
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
return 1;}else{
# 2575
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp514;_tmp514.tag=0;_tmp514.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_PrintArg_struct _tmp513;_tmp513.tag=0;_tmp513.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));({struct Cyc_String_pa_PrintArg_struct _tmp512;_tmp512.tag=0;_tmp512.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp510[3]={& _tmp512,& _tmp513,& _tmp514};Cyc_Tcutil_warn(e->loc,({const char*_tmp511="implicit alias coercion for %s:%s to %s";_tag_dyneither(_tmp511,sizeof(char),40);}),_tag_dyneither(_tmp510,sizeof(void*),3));});});});});
*alias_coercion=1;}
# 2582
if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);
return 1;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e))
return 1;else{
if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= Cyc_Absyn_Unknown_coercion){
# 2590
if(c != Cyc_Absyn_No_coercion)Cyc_Tcutil_unchecked_cast(te,e,t2,c);
if(c != Cyc_Absyn_NonNull_to_Null)
({struct Cyc_String_pa_PrintArg_struct _tmp518;_tmp518.tag=0;_tmp518.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_PrintArg_struct _tmp517;_tmp517.tag=0;_tmp517.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));({void*_tmp515[2]={& _tmp517,& _tmp518};Cyc_Tcutil_warn(e->loc,({const char*_tmp516="implicit cast from %s to %s";_tag_dyneither(_tmp516,sizeof(char),28);}),_tag_dyneither(_tmp515,sizeof(void*),2));});});});
return 1;}else{
# 2596
return 0;}}}}}
# 2603
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){
# 2606
int bogus=0;
return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}
# 2610
int Cyc_Tcutil_coerceable(void*t){
void*_tmp519=Cyc_Tcutil_compress(t);void*_tmp51A=_tmp519;switch(*((int*)_tmp51A)){case 6: _LL4D5: _LL4D6:
 goto _LL4D8;case 7: _LL4D7: _LL4D8:
 return 1;default: _LL4D9: _LL4DA:
 return 0;}_LL4D4:;}
# 2628 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 2632
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ_f(struct _tuple23*env,struct Cyc_Absyn_Aggrfield*x){
# 2635
struct _tuple23 _tmp51B=*env;struct _tuple23 _tmp51C=_tmp51B;struct Cyc_List_List*_tmp51D;struct _RegionHandle*_tmp51E;struct Cyc_Tcenv_Tenv*_tmp51F;int _tmp520;_LL4DC: _tmp51D=_tmp51C.f1;_tmp51E=_tmp51C.f2;_tmp51F=_tmp51C.f3;_tmp520=_tmp51C.f4;_LL4DD:;{
# 2637
void*_tmp521=_tmp51D == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp51E,_tmp51D,x->type);
struct Cyc_List_List*_tmp522=Cyc_Tcutil_flatten_typ(_tmp51E,_tmp520,_tmp51F,_tmp521);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp522)== 1)
return({struct Cyc_List_List*_tmp523=_region_malloc(_tmp51E,sizeof(*_tmp523));_tmp523->hd=({struct _tuple12*_tmp524=_region_malloc(_tmp51E,sizeof(*_tmp524));_tmp524->f1=x->tq;_tmp524->f2=_tmp521;_tmp524;});_tmp523->tl=0;_tmp523;});else{
return _tmp522;}};}struct _tuple24{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2643
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple24*env,struct _tuple12*x){
# 2645
struct _tuple24 _tmp525=*env;struct _tuple24 _tmp526=_tmp525;struct _RegionHandle*_tmp527;struct Cyc_Tcenv_Tenv*_tmp528;int _tmp529;_LL4DF: _tmp527=_tmp526.f1;_tmp528=_tmp526.f2;_tmp529=_tmp526.f3;_LL4E0:;{
struct _tuple12 _tmp52A=*x;struct _tuple12 _tmp52B=_tmp52A;struct Cyc_Absyn_Tqual _tmp52C;void*_tmp52D;_LL4E2: _tmp52C=_tmp52B.f1;_tmp52D=_tmp52B.f2;_LL4E3:;{
struct Cyc_List_List*_tmp52E=Cyc_Tcutil_flatten_typ(_tmp527,_tmp529,_tmp528,_tmp52D);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp52E)== 1)
return({struct Cyc_List_List*_tmp52F=_region_malloc(_tmp527,sizeof(*_tmp52F));_tmp52F->hd=({struct _tuple12*_tmp530=_region_malloc(_tmp527,sizeof(*_tmp530));_tmp530->f1=_tmp52C;_tmp530->f2=_tmp52D;_tmp530;});_tmp52F->tl=0;_tmp52F;});else{
return _tmp52E;}};};}
# 2652
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2656
if(flatten){
t1=Cyc_Tcutil_compress(t1);{
void*_tmp531=t1;struct Cyc_List_List*_tmp532;struct Cyc_Absyn_Aggrdecl*_tmp533;struct Cyc_List_List*_tmp534;struct Cyc_List_List*_tmp535;switch(*((int*)_tmp531)){case 0: _LL4E5: _LL4E6:
 return 0;case 10: _LL4E7: _tmp535=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp531)->f1;_LL4E8: {
# 2661
struct _tuple24 _tmp536=({struct _tuple24 _tmp53F;_tmp53F.f1=r;_tmp53F.f2=te;_tmp53F.f3=flatten;_tmp53F;});
# 2663
struct Cyc_List_List*_tmp537=_tmp535;struct _tuple12*_tmp538;struct Cyc_List_List*_tmp539;if(_tmp537 == 0){_LL4F0: _LL4F1:
 return 0;}else{_LL4F2: _tmp538=(struct _tuple12*)_tmp537->hd;_tmp539=_tmp537->tl;_LL4F3: {
# 2666
struct Cyc_List_List*_tmp53A=Cyc_Tcutil_rcopy_tqt(& _tmp536,_tmp538);
_tmp536.f3=0;{
struct Cyc_List_List*_tmp53B=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple12*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp536,_tmp535);
struct Cyc_List_List*_tmp53C=({struct Cyc_List_List*_tmp53E=_region_malloc(r,sizeof(*_tmp53E));_tmp53E->hd=_tmp53A;_tmp53E->tl=_tmp53B;_tmp53E;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,({struct Cyc_List_List*_tmp53D=_region_malloc(r,sizeof(*_tmp53D));_tmp53D->hd=_tmp53A;_tmp53D->tl=_tmp53B;_tmp53D;}));};}}_LL4EF:;}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp531)->f1).aggr_info).KnownAggr).tag == 2){_LL4E9: _tmp533=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp531)->f1).aggr_info).KnownAggr).val;_tmp534=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp531)->f1).targs;_LL4EA:
# 2674
 if(((_tmp533->kind == Cyc_Absyn_UnionA  || _tmp533->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp533->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp533->impl))->rgn_po != 0)
# 2676
return({struct Cyc_List_List*_tmp540=_region_malloc(r,sizeof(*_tmp540));_tmp540->hd=({struct _tuple12*_tmp541=_region_malloc(r,sizeof(*_tmp541));_tmp541->f1=Cyc_Absyn_empty_tqual(0);_tmp541->f2=t1;_tmp541;});_tmp540->tl=0;_tmp540;});{
struct Cyc_List_List*_tmp542=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp533->tvs,_tmp534);
struct _tuple23 env=({struct _tuple23 _tmp54B;_tmp54B.f1=_tmp542;_tmp54B.f2=r;_tmp54B.f3=te;_tmp54B.f4=flatten;_tmp54B;});
struct Cyc_List_List*_tmp543=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp533->impl))->fields;struct Cyc_List_List*_tmp544=_tmp543;struct Cyc_Absyn_Aggrfield*_tmp545;struct Cyc_List_List*_tmp546;if(_tmp544 == 0){_LL4F5: _LL4F6:
 return 0;}else{_LL4F7: _tmp545=(struct Cyc_Absyn_Aggrfield*)_tmp544->hd;_tmp546=_tmp544->tl;_LL4F8: {
# 2682
struct Cyc_List_List*_tmp547=Cyc_Tcutil_flatten_typ_f(& env,_tmp545);
env.f4=0;{
struct Cyc_List_List*_tmp548=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp546);
struct Cyc_List_List*_tmp549=({struct Cyc_List_List*_tmp54A=_region_malloc(r,sizeof(*_tmp54A));_tmp54A->hd=_tmp547;_tmp54A->tl=_tmp548;_tmp54A;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp549);};}}_LL4F4:;};}else{goto _LL4ED;}case 12: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp531)->f1 == Cyc_Absyn_StructA){_LL4EB: _tmp532=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp531)->f2;_LL4EC: {
# 2689
struct _tuple23 env=({struct _tuple23 _tmp553;_tmp553.f1=0;_tmp553.f2=r;_tmp553.f3=te;_tmp553.f4=flatten;_tmp553;});
struct Cyc_List_List*_tmp54C=_tmp532;struct Cyc_Absyn_Aggrfield*_tmp54D;struct Cyc_List_List*_tmp54E;if(_tmp54C == 0){_LL4FA: _LL4FB:
 return 0;}else{_LL4FC: _tmp54D=(struct Cyc_Absyn_Aggrfield*)_tmp54C->hd;_tmp54E=_tmp54C->tl;_LL4FD: {
# 2693
struct Cyc_List_List*_tmp54F=Cyc_Tcutil_flatten_typ_f(& env,_tmp54D);
env.f4=0;{
struct Cyc_List_List*_tmp550=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp54E);
struct Cyc_List_List*_tmp551=({struct Cyc_List_List*_tmp552=_region_malloc(r,sizeof(*_tmp552));_tmp552->hd=_tmp54F;_tmp552->tl=_tmp550;_tmp552;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp551);};}}_LL4F9:;}}else{goto _LL4ED;}default: _LL4ED: _LL4EE:
# 2699
 goto _LL4E4;}_LL4E4:;};}
# 2702
return({struct Cyc_List_List*_tmp554=_region_malloc(r,sizeof(*_tmp554));_tmp554->hd=({struct _tuple12*_tmp555=_region_malloc(r,sizeof(*_tmp555));_tmp555->f1=Cyc_Absyn_empty_tqual(0);_tmp555->f2=t1;_tmp555;});_tmp554->tl=0;_tmp554;});}
# 2706
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp556=(void*)t->hd;void*_tmp557=_tmp556;switch(*((int*)_tmp557)){case 23: _LL4FF: _LL500:
 goto _LL502;case 4: _LL501: _LL502:
 goto _LL504;case 20: _LL503: _LL504:
# 2712
 continue;default: _LL505: _LL506:
# 2714
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;}_LL4FE:;}}
# 2717
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}
# 2720
return 1;}
# 2723
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp558=({struct _tuple0 _tmp55C;_tmp55C.f1=Cyc_Tcutil_compress(t1);_tmp55C.f2=Cyc_Tcutil_compress(t2);_tmp55C;});struct _tuple0 _tmp559=_tmp558;enum Cyc_Absyn_Size_of _tmp55A;enum Cyc_Absyn_Size_of _tmp55B;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp559.f1)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp559.f2)->tag == 6){_LL508: _tmp55A=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp559.f1)->f2;_tmp55B=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp559.f2)->f2;_LL509:
# 2726
 return(_tmp55A == _tmp55B  || _tmp55A == Cyc_Absyn_Int_sz  && _tmp55B == Cyc_Absyn_Long_sz) || 
_tmp55A == Cyc_Absyn_Long_sz  && _tmp55B == Cyc_Absyn_Int_sz;}else{goto _LL50A;}}else{_LL50A: _LL50B:
 return 0;}_LL507:;}
# 2734
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2737
if(Cyc_Tcutil_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}
# 2742
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp55D=({struct _tuple0 _tmp593;_tmp593.f1=t1;_tmp593.f2=t2;_tmp593;});struct _tuple0 _tmp55E=_tmp55D;struct Cyc_Absyn_FnInfo _tmp55F;struct Cyc_Absyn_FnInfo _tmp560;struct Cyc_Absyn_Datatypedecl*_tmp561;struct Cyc_Absyn_Datatypefield*_tmp562;struct Cyc_List_List*_tmp563;struct Cyc_Absyn_Datatypedecl*_tmp564;struct Cyc_List_List*_tmp565;void*_tmp566;struct Cyc_Absyn_Tqual _tmp567;void*_tmp568;union Cyc_Absyn_Constraint*_tmp569;union Cyc_Absyn_Constraint*_tmp56A;union Cyc_Absyn_Constraint*_tmp56B;void*_tmp56C;struct Cyc_Absyn_Tqual _tmp56D;void*_tmp56E;union Cyc_Absyn_Constraint*_tmp56F;union Cyc_Absyn_Constraint*_tmp570;union Cyc_Absyn_Constraint*_tmp571;switch(*((int*)_tmp55E.f1)){case 5: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55E.f2)->tag == 5){_LL50D: _tmp566=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55E.f1)->f1).elt_typ;_tmp567=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55E.f1)->f1).elt_tq;_tmp568=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55E.f1)->f1).ptr_atts).rgn;_tmp569=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55E.f1)->f1).ptr_atts).nullable;_tmp56A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55E.f1)->f1).ptr_atts).bounds;_tmp56B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55E.f1)->f1).ptr_atts).zero_term;_tmp56C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55E.f2)->f1).elt_typ;_tmp56D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55E.f2)->f1).elt_tq;_tmp56E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55E.f2)->f1).ptr_atts).rgn;_tmp56F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55E.f2)->f1).ptr_atts).nullable;_tmp570=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55E.f2)->f1).ptr_atts).bounds;_tmp571=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55E.f2)->f1).ptr_atts).zero_term;_LL50E:
# 2750
 if(_tmp567.real_const  && !_tmp56D.real_const)
return 0;
# 2753
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp569,_tmp56F) && 
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp569)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp56F))
return 0;
# 2757
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp56B,_tmp571) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp56B)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp571))
return 0;
# 2761
if((!Cyc_Tcutil_unify(_tmp568,_tmp56E) && !Cyc_Tcenv_region_outlives(te,_tmp568,_tmp56E)) && !
Cyc_Tcutil_subtype(te,assume,_tmp568,_tmp56E))
return 0;
# 2765
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp56A,_tmp570)){
struct _tuple0 _tmp572=({struct _tuple0 _tmp576;_tmp576.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp56A);_tmp576.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp570);_tmp576;});struct _tuple0 _tmp573=_tmp572;struct Cyc_Absyn_Exp*_tmp574;struct Cyc_Absyn_Exp*_tmp575;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp573.f1)->tag == 0){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp573.f2)->tag == 0){_LL516: _LL517:
 goto _LL515;}else{goto _LL51A;}}else{if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp573.f2)->tag == 1){_LL518: _tmp574=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp573.f1)->f1;_tmp575=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp573.f2)->f1;_LL519:
# 2770
 if(!Cyc_Evexp_lte_const_exp(_tmp575,_tmp574))
return 0;
goto _LL515;}else{_LL51A: _LL51B:
 return 0;}}_LL515:;}
# 2778
if(!_tmp56D.real_const  && _tmp567.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp56C)))
return 0;}{
# 2784
int _tmp577=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp570,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp571);
# 2790
return(_tmp577  && Cyc_Tcutil_ptrsubtype(te,({struct Cyc_List_List*_tmp578=_cycalloc(sizeof(*_tmp578));_tmp578->hd=({struct _tuple0*_tmp579=_cycalloc(sizeof(*_tmp579));_tmp579->f1=t1;_tmp579->f2=t2;_tmp579;});_tmp578->tl=assume;_tmp578;}),_tmp566,_tmp56C) || Cyc_Tcutil_unify(_tmp566,_tmp56C)) || Cyc_Tcutil_isomorphic(_tmp566,_tmp56C);};}else{goto _LL513;}case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp55E.f1)->f1).field_info).KnownDatatypefield).tag == 2){if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp55E.f2)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp55E.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL50F: _tmp561=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp55E.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp562=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp55E.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp563=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp55E.f1)->f1).targs;_tmp564=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp55E.f2)->f1).datatype_info).KnownDatatype).val;_tmp565=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp55E.f2)->f1).targs;_LL510:
# 2796
 if(_tmp561 != _tmp564  && Cyc_Absyn_qvar_cmp(_tmp561->name,_tmp564->name)!= 0)return 0;
# 2798
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp563)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp565))return 0;
for(0;_tmp563 != 0;(_tmp563=_tmp563->tl,_tmp565=_tmp565->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp563->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp565))->hd))return 0;}
return 1;}else{goto _LL513;}}else{goto _LL513;}}else{goto _LL513;}case 9: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp55E.f2)->tag == 9){_LL511: _tmp55F=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp55E.f1)->f1;_tmp560=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp55E.f2)->f1;_LL512:
# 2805
 if(_tmp55F.tvars != 0  || _tmp560.tvars != 0){
struct Cyc_List_List*_tmp57A=_tmp55F.tvars;
struct Cyc_List_List*_tmp57B=_tmp560.tvars;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp57A)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp57B))return 0;{
struct _RegionHandle _tmp57C=_new_region("r");struct _RegionHandle*r=& _tmp57C;_push_region(r);
{struct Cyc_List_List*inst=0;
while(_tmp57A != 0){
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp57A->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp57B))->hd)->kind)){int _tmp57D=0;_npop_handler(0);return _tmp57D;}
inst=({struct Cyc_List_List*_tmp57E=_region_malloc(r,sizeof(*_tmp57E));_tmp57E->hd=({struct _tuple16*_tmp57F=_region_malloc(r,sizeof(*_tmp57F));_tmp57F->f1=(struct Cyc_Absyn_Tvar*)_tmp57B->hd;_tmp57F->f2=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp580=_cycalloc(sizeof(*_tmp580));_tmp580[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp581;_tmp581.tag=2;_tmp581.f1=(struct Cyc_Absyn_Tvar*)_tmp57A->hd;_tmp581;});_tmp580;});_tmp57F;});_tmp57E->tl=inst;_tmp57E;});
_tmp57A=_tmp57A->tl;
_tmp57B=_tmp57B->tl;}
# 2817
if(inst != 0){
_tmp55F.tvars=0;
_tmp560.tvars=0;{
int _tmp586=Cyc_Tcutil_subtype(te,assume,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp582=_cycalloc(sizeof(*_tmp582));_tmp582[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp583;_tmp583.tag=9;_tmp583.f1=_tmp55F;_tmp583;});_tmp582;}),(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp584=_cycalloc(sizeof(*_tmp584));_tmp584[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp585;_tmp585.tag=9;_tmp585.f1=_tmp560;_tmp585;});_tmp584;}));_npop_handler(0);return _tmp586;};}}
# 2810
;_pop_region(r);};}
# 2825
if(!Cyc_Tcutil_subtype(te,assume,_tmp55F.ret_typ,_tmp560.ret_typ))return 0;{
struct Cyc_List_List*_tmp587=_tmp55F.args;
struct Cyc_List_List*_tmp588=_tmp560.args;
# 2830
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp587)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp588))return 0;
# 2832
for(0;_tmp587 != 0;(_tmp587=_tmp587->tl,_tmp588=_tmp588->tl)){
struct _tuple10 _tmp589=*((struct _tuple10*)_tmp587->hd);struct _tuple10 _tmp58A=_tmp589;struct Cyc_Absyn_Tqual _tmp58B;void*_tmp58C;_LL51D: _tmp58B=_tmp58A.f2;_tmp58C=_tmp58A.f3;_LL51E:;{
struct _tuple10 _tmp58D=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp588))->hd);struct _tuple10 _tmp58E=_tmp58D;struct Cyc_Absyn_Tqual _tmp58F;void*_tmp590;_LL520: _tmp58F=_tmp58E.f2;_tmp590=_tmp58E.f3;_LL521:;
# 2836
if(_tmp58F.real_const  && !_tmp58B.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp590,_tmp58C))
return 0;};}
# 2840
if(_tmp55F.c_varargs != _tmp560.c_varargs)return 0;
if(_tmp55F.cyc_varargs != 0  && _tmp560.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp591=*_tmp55F.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp592=*_tmp560.cyc_varargs;
# 2845
if((_tmp592.tq).real_const  && !(_tmp591.tq).real_const  || !
Cyc_Tcutil_subtype(te,assume,_tmp592.type,_tmp591.type))
return 0;}else{
if(_tmp55F.cyc_varargs != 0  || _tmp560.cyc_varargs != 0)return 0;}
# 2850
if(!Cyc_Tcutil_subset_effect(1,(void*)_check_null(_tmp55F.effect),(void*)_check_null(_tmp560.effect)))return 0;
# 2852
if(!Cyc_Tcutil_sub_rgnpo(_tmp55F.rgn_po,_tmp560.rgn_po))return 0;
# 2854
if(!Cyc_Tcutil_sub_attributes(_tmp55F.attributes,_tmp560.attributes))return 0;
# 2856
if(!Cyc_Tcutil_check_logical_implication(_tmp560.requires_relns,_tmp55F.requires_relns))
return 0;
# 2859
if(!Cyc_Tcutil_check_logical_implication(_tmp55F.ensures_relns,_tmp560.ensures_relns))
return 0;
# 2862
return 1;};}else{goto _LL513;}default: _LL513: _LL514:
 return 0;}_LL50C:;};}
# 2874 "tcutil.cyc"
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2876
struct _RegionHandle _tmp594=_new_region("temp");struct _RegionHandle*temp=& _tmp594;_push_region(temp);
{struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(temp,1,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(temp,1,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0){int _tmp595=0;_npop_handler(0);return _tmp595;}{
struct _tuple12*_tmp596=(struct _tuple12*)tqs1->hd;struct _tuple12*_tmp597=_tmp596;struct Cyc_Absyn_Tqual _tmp598;void*_tmp599;_LL523: _tmp598=_tmp597->f1;_tmp599=_tmp597->f2;_LL524:;{
struct _tuple12*_tmp59A=(struct _tuple12*)tqs2->hd;struct _tuple12*_tmp59B=_tmp59A;struct Cyc_Absyn_Tqual _tmp59C;void*_tmp59D;_LL526: _tmp59C=_tmp59B->f1;_tmp59D=_tmp59B->f2;_LL527:;
# 2884
if(_tmp598.real_const  && !_tmp59C.real_const){int _tmp59E=0;_npop_handler(0);return _tmp59E;}
# 2886
if((_tmp59C.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp59D))) && 
Cyc_Tcutil_subtype(te,assume,_tmp599,_tmp59D))
# 2889
continue;
# 2891
if(Cyc_Tcutil_unify(_tmp599,_tmp59D))
# 2893
continue;
# 2895
if(Cyc_Tcutil_isomorphic(_tmp599,_tmp59D))
# 2897
continue;{
# 2900
int _tmp59F=0;_npop_handler(0);return _tmp59F;};};};}{
# 2902
int _tmp5A0=1;_npop_handler(0);return _tmp5A0;};}
# 2877
;_pop_region(temp);}
# 2906
static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp5A1=Cyc_Tcutil_compress(t);void*_tmp5A2=_tmp5A1;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A2)->f2 == Cyc_Absyn_Char_sz){_LL529: _LL52A:
 return 1;}else{goto _LL52B;}}else{_LL52B: _LL52C:
 return 0;}_LL528:;}
# 2915
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
# 2921
if(t2 == (void*)& Cyc_Absyn_VoidType_val)
return Cyc_Absyn_No_coercion;
{void*_tmp5A3=t2;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A3)->tag == 6)switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A3)->f2){case Cyc_Absyn_Int_sz: _LL52E: _LL52F:
# 2925
 goto _LL531;case Cyc_Absyn_Long_sz: _LL530: _LL531:
# 2927
 if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL52D;default: goto _LL532;}else{_LL532: _LL533:
 goto _LL52D;}_LL52D:;}{
# 2931
void*_tmp5A4=t1;void*_tmp5A5;struct Cyc_Absyn_Enumdecl*_tmp5A6;void*_tmp5A7;struct Cyc_Absyn_Tqual _tmp5A8;struct Cyc_Absyn_Exp*_tmp5A9;union Cyc_Absyn_Constraint*_tmp5AA;void*_tmp5AB;struct Cyc_Absyn_Tqual _tmp5AC;void*_tmp5AD;union Cyc_Absyn_Constraint*_tmp5AE;union Cyc_Absyn_Constraint*_tmp5AF;union Cyc_Absyn_Constraint*_tmp5B0;switch(*((int*)_tmp5A4)){case 5: _LL535: _tmp5AB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A4)->f1).elt_typ;_tmp5AC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A4)->f1).elt_tq;_tmp5AD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A4)->f1).ptr_atts).rgn;_tmp5AE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A4)->f1).ptr_atts).nullable;_tmp5AF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A4)->f1).ptr_atts).bounds;_tmp5B0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A4)->f1).ptr_atts).zero_term;_LL536:
# 2939
{void*_tmp5B1=t2;void*_tmp5B2;struct Cyc_Absyn_Tqual _tmp5B3;void*_tmp5B4;union Cyc_Absyn_Constraint*_tmp5B5;union Cyc_Absyn_Constraint*_tmp5B6;union Cyc_Absyn_Constraint*_tmp5B7;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B1)->tag == 5){_LL544: _tmp5B2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B1)->f1).elt_typ;_tmp5B3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B1)->f1).elt_tq;_tmp5B4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B1)->f1).ptr_atts).rgn;_tmp5B5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B1)->f1).ptr_atts).nullable;_tmp5B6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B1)->f1).ptr_atts).bounds;_tmp5B7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B1)->f1).ptr_atts).zero_term;_LL545: {
# 2943
enum Cyc_Absyn_Coercion coercion=Cyc_Absyn_Other_coercion;
struct Cyc_List_List*_tmp5B8=({struct Cyc_List_List*_tmp5C2=_cycalloc(sizeof(*_tmp5C2));_tmp5C2->hd=({struct _tuple0*_tmp5C3=_cycalloc(sizeof(*_tmp5C3));_tmp5C3->f1=t1;_tmp5C3->f2=t2;_tmp5C3;});_tmp5C2->tl=0;_tmp5C2;});
int _tmp5B9=_tmp5B3.real_const  || !_tmp5AC.real_const;
# 2957 "tcutil.cyc"
int _tmp5BA=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp5B6,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp5B7);
# 2961
int _tmp5BB=_tmp5B9  && (
((_tmp5BA  && Cyc_Tcutil_ptrsubtype(te,_tmp5B8,_tmp5AB,_tmp5B2) || 
Cyc_Tcutil_unify(_tmp5AB,_tmp5B2)) || Cyc_Tcutil_isomorphic(_tmp5AB,_tmp5B2)) || Cyc_Tcutil_unify(_tmp5B2,(void*)& Cyc_Absyn_VoidType_val));
# 2965
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp5B0,_tmp5B7) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp5B7);
# 2969
int _tmp5BC=_tmp5BB?0:((Cyc_Tcutil_bits_only(_tmp5AB) && Cyc_Tcutil_is_char_type(_tmp5B2)) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5B7)) && (
_tmp5B3.real_const  || !_tmp5AC.real_const);
int bounds_ok=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp5AF,_tmp5B6);
if(!bounds_ok  && !_tmp5BC){
struct _tuple0 _tmp5BD=({struct _tuple0 _tmp5C1;_tmp5C1.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp5AF);_tmp5C1.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp5B6);_tmp5C1;});struct _tuple0 _tmp5BE=_tmp5BD;struct Cyc_Absyn_Exp*_tmp5BF;struct Cyc_Absyn_Exp*_tmp5C0;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5BE.f1)->tag == 1){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5BE.f2)->tag == 1){_LL549: _tmp5BF=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5BE.f1)->f1;_tmp5C0=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5BE.f2)->f1;_LL54A:
# 2979
 if(Cyc_Evexp_lte_const_exp(_tmp5C0,_tmp5BF))
bounds_ok=1;
goto _LL548;}else{goto _LL54B;}}else{_LL54B: _LL54C:
# 2984
 bounds_ok=1;goto _LL548;}_LL548:;}
# 2986
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5AE) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5B5))
coercion=Cyc_Absyn_NonNull_to_Null;
# 2991
if(((bounds_ok  && zeroterm_ok) && (_tmp5BB  || _tmp5BC)) && (
Cyc_Tcutil_unify(_tmp5AD,_tmp5B4) || Cyc_Tcenv_region_outlives(te,_tmp5AD,_tmp5B4)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};}}else{_LL546: _LL547:
 goto _LL543;}_LL543:;}
# 2997
return Cyc_Absyn_Unknown_coercion;case 8: _LL537: _tmp5A7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A4)->f1).elt_type;_tmp5A8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A4)->f1).tq;_tmp5A9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A4)->f1).num_elts;_tmp5AA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A4)->f1).zero_term;_LL538:
# 2999
{void*_tmp5C4=t2;void*_tmp5C5;struct Cyc_Absyn_Tqual _tmp5C6;struct Cyc_Absyn_Exp*_tmp5C7;union Cyc_Absyn_Constraint*_tmp5C8;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5C4)->tag == 8){_LL54E: _tmp5C5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5C4)->f1).elt_type;_tmp5C6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5C4)->f1).tq;_tmp5C7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5C4)->f1).num_elts;_tmp5C8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5C4)->f1).zero_term;_LL54F: {
# 3001
int okay;
okay=
(((_tmp5A9 != 0  && _tmp5C7 != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp5AA,_tmp5C8)) && 
Cyc_Evexp_lte_const_exp(_tmp5C7,_tmp5A9)) && 
Cyc_Evexp_lte_const_exp(_tmp5A9,_tmp5C7);
return
# 3008
(okay  && Cyc_Tcutil_unify(_tmp5A7,_tmp5C5)) && (!_tmp5A8.real_const  || _tmp5C6.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}}else{_LL550: _LL551:
# 3010
 return Cyc_Absyn_Unknown_coercion;}_LL54D:;}
# 3012
return Cyc_Absyn_Unknown_coercion;case 13: _LL539: _tmp5A6=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5A4)->f2;_LL53A:
# 3016
{void*_tmp5C9=t2;struct Cyc_Absyn_Enumdecl*_tmp5CA;if(((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5C9)->tag == 13){_LL553: _tmp5CA=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5C9)->f2;_LL554:
# 3018
 if((_tmp5A6->fields != 0  && _tmp5CA->fields != 0) && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5A6->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5CA->fields))->v))
return Cyc_Absyn_Other_coercion;
goto _LL552;}else{_LL555: _LL556:
 goto _LL552;}_LL552:;}
# 3024
goto _LL53C;case 6: _LL53B: _LL53C:
 goto _LL53E;case 7: _LL53D: _LL53E:
 return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;case 15: _LL53F: _tmp5A5=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5A4)->f1;_LL540:
# 3029
{void*_tmp5CB=t2;void*_tmp5CC;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5CB)->tag == 15){_LL558: _tmp5CC=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5CB)->f1;_LL559:
# 3031
 if(Cyc_Tcenv_region_outlives(te,_tmp5A5,_tmp5CC))return Cyc_Absyn_No_coercion;
goto _LL557;}else{_LL55A: _LL55B:
 goto _LL557;}_LL557:;}
# 3035
return Cyc_Absyn_Unknown_coercion;default: _LL541: _LL542:
 return Cyc_Absyn_Unknown_coercion;}_LL534:;};}
# 3041
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),t)){
struct Cyc_Absyn_Exp*_tmp5CD=Cyc_Absyn_copy_exp(e);
e->r=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp5CE=_cycalloc(sizeof(*_tmp5CE));_tmp5CE[0]=({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp5CF;_tmp5CF.tag=13;_tmp5CF.f1=t;_tmp5CF.f2=_tmp5CD;_tmp5CF.f3=0;_tmp5CF.f4=c;_tmp5CF;});_tmp5CE;});
e->topt=t;}}
# 3049
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp5D0=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp5D1=_tmp5D0;switch(*((int*)_tmp5D1)){case 6: _LL55D: _LL55E:
 goto _LL560;case 13: _LL55F: _LL560:
 goto _LL562;case 14: _LL561: _LL562:
 goto _LL564;case 19: _LL563: _LL564:
 return 1;case 1: _LL565: _LL566:
 return Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ);default: _LL567: _LL568:
 return 0;}_LL55C:;}
# 3060
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp5D2=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp5D3=_tmp5D2;if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D3)->tag == 7){_LL56A: _LL56B:
 return 1;}else{_LL56C: _LL56D:
 return 0;}_LL569:;};}
# 3069
int Cyc_Tcutil_is_function_type(void*t){
void*_tmp5D4=Cyc_Tcutil_compress(t);void*_tmp5D5=_tmp5D4;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5D5)->tag == 9){_LL56F: _LL570:
 return 1;}else{_LL571: _LL572:
 return 0;}_LL56E:;}
# 3076
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
struct _tuple0 _tmp5D6=({struct _tuple0 _tmp5DA;_tmp5DA.f1=t1;_tmp5DA.f2=t2;_tmp5DA;});struct _tuple0 _tmp5D7=_tmp5D6;int _tmp5D8;int _tmp5D9;if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D7.f1)->tag == 7){if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D7.f2)->tag == 7){_LL574: _tmp5D8=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D7.f1)->f1;_tmp5D9=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D7.f2)->f1;_LL575:
# 3079
 if(_tmp5D8 != 0  && _tmp5D8 != 1)return t1;else{
if(_tmp5D9 != 0  && _tmp5D9 != 1)return t2;else{
if(_tmp5D8 >= _tmp5D9)return t1;else{
return t2;}}}}else{_LL576: _LL577:
 return t1;}}else{if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D7.f2)->tag == 7){_LL578: _LL579:
 return t2;}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D7.f1)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D7.f1)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D7.f1)->f2 == Cyc_Absyn_LongLong_sz){_LL57A: _LL57B:
 goto _LL57D;}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D7.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D7.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D7.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL57C;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D7.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL582;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D7.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL584;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D7.f1)->f2 == Cyc_Absyn_Int_sz)goto _LL586;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D7.f2)->f2 == Cyc_Absyn_Int_sz)goto _LL588;else{goto _LL58E;}}}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D7.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL580;else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D7.f1)->f2){case Cyc_Absyn_Long_sz: goto _LL582;case Cyc_Absyn_Int_sz: goto _LL586;default: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D7.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL58C;else{goto _LL58E;}}}}}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D7.f1)->f2){case Cyc_Absyn_Long_sz: _LL582: _LL583:
# 3089
 goto _LL585;case Cyc_Absyn_Int_sz: _LL586: _LL587:
# 3092
 goto _LL589;default: goto _LL58E;}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D7.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D7.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D7.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL57C;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D7.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL57E;else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D7.f2)->f2){case Cyc_Absyn_Long_sz: goto _LL584;case Cyc_Absyn_Int_sz: goto _LL588;default: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D7.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL58A;else{goto _LL58E;}}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D7.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL57E;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D7.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL580;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D7.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL58A;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D7.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL58C;else{goto _LL58E;}}}}}}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D7.f1)->f2){case Cyc_Absyn_LongLong_sz: _LL57E: _LL57F:
# 3087
 goto _LL581;case Cyc_Absyn_Long_sz: _LL58A: _LL58B:
# 3094
 goto _LL58D;default: goto _LL58E;}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D7.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D7.f2)->f1 == Cyc_Absyn_Unsigned)switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D7.f2)->f2){case Cyc_Absyn_LongLong_sz: _LL57C: _LL57D:
# 3086
 return Cyc_Absyn_ulonglong_typ;case Cyc_Absyn_Long_sz: _LL584: _LL585:
# 3090
 return Cyc_Absyn_ulong_typ;case Cyc_Absyn_Int_sz: _LL588: _LL589:
# 3093
 return Cyc_Absyn_uint_typ;default: goto _LL58E;}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D7.f2)->f2){case Cyc_Absyn_LongLong_sz: _LL580: _LL581:
# 3088
 return Cyc_Absyn_slonglong_typ;case Cyc_Absyn_Long_sz: _LL58C: _LL58D:
# 3095
 return Cyc_Absyn_slong_typ;default: goto _LL58E;}}}else{_LL58E: _LL58F:
 return Cyc_Absyn_sint_typ;}}}}_LL573:;}
# 3107 "tcutil.cyc"
static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){
c1=Cyc_Absyn_compress_kb(c1);
c2=Cyc_Absyn_compress_kb(c2);
if(c1 == c2)return 1;{
struct _tuple0 _tmp5DB=({struct _tuple0 _tmp5F1;_tmp5F1.f1=c1;_tmp5F1.f2=c2;_tmp5F1;});struct _tuple0 _tmp5DC=_tmp5DB;struct Cyc_Core_Opt**_tmp5DD;struct Cyc_Absyn_Kind*_tmp5DE;struct Cyc_Core_Opt**_tmp5DF;struct Cyc_Absyn_Kind*_tmp5E0;struct Cyc_Core_Opt**_tmp5E1;struct Cyc_Absyn_Kind*_tmp5E2;struct Cyc_Absyn_Kind*_tmp5E3;struct Cyc_Core_Opt**_tmp5E4;struct Cyc_Core_Opt**_tmp5E5;struct Cyc_Absyn_Kind*_tmp5E6;struct Cyc_Core_Opt**_tmp5E7;struct Cyc_Absyn_Kind*_tmp5E8;struct Cyc_Absyn_Kind*_tmp5E9;struct Cyc_Absyn_Kind*_tmp5EA;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5DC.f1)->tag == 0)switch(*((int*)_tmp5DC.f2)){case 0: _LL591: _tmp5E9=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5DC.f1)->f1;_tmp5EA=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5DC.f2)->f1;_LL592:
 return _tmp5E9 == _tmp5EA;case 1: goto _LL593;default: _LL599: _tmp5E6=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5DC.f1)->f1;_tmp5E7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5DC.f2)->f1;_tmp5E8=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5DC.f2)->f2;_LL59A:
# 3120
 if(Cyc_Tcutil_kind_leq(_tmp5E6,_tmp5E8)){
*_tmp5E7=({struct Cyc_Core_Opt*_tmp5EE=_cycalloc(sizeof(*_tmp5EE));_tmp5EE->v=c1;_tmp5EE;});return 1;}else{
return 0;}}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5DC.f2)->tag == 1){_LL593: _tmp5E5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5DC.f2)->f1;_LL594:
# 3113
*_tmp5E5=({struct Cyc_Core_Opt*_tmp5EB=_cycalloc(sizeof(*_tmp5EB));_tmp5EB->v=c1;_tmp5EB;});return 1;}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5DC.f1)->tag == 1){_LL595: _tmp5E4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5DC.f1)->f1;_LL596:
*_tmp5E4=({struct Cyc_Core_Opt*_tmp5EC=_cycalloc(sizeof(*_tmp5EC));_tmp5EC->v=c2;_tmp5EC;});return 1;}else{if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5DC.f2)->tag == 0){_LL597: _tmp5E1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5DC.f1)->f1;_tmp5E2=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5DC.f1)->f2;_tmp5E3=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5DC.f2)->f1;_LL598:
# 3116
 if(Cyc_Tcutil_kind_leq(_tmp5E3,_tmp5E2)){
*_tmp5E1=({struct Cyc_Core_Opt*_tmp5ED=_cycalloc(sizeof(*_tmp5ED));_tmp5ED->v=c2;_tmp5ED;});return 1;}else{
return 0;}}else{_LL59B: _tmp5DD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5DC.f1)->f1;_tmp5DE=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5DC.f1)->f2;_tmp5DF=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5DC.f2)->f1;_tmp5E0=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5DC.f2)->f2;_LL59C:
# 3124
 if(Cyc_Tcutil_kind_leq(_tmp5DE,_tmp5E0)){
*_tmp5DF=({struct Cyc_Core_Opt*_tmp5EF=_cycalloc(sizeof(*_tmp5EF));_tmp5EF->v=c1;_tmp5EF;});return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp5E0,_tmp5DE)){
*_tmp5DD=({struct Cyc_Core_Opt*_tmp5F0=_cycalloc(sizeof(*_tmp5F0));_tmp5F0->v=c2;_tmp5F0;});return 1;}else{
return 0;}}}}}}_LL590:;};}
# 3133
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 3138
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp5F6;_tmp5F6.tag=1;_tmp5F6.f1=(unsigned long)i;({void*_tmp5F4[1]={& _tmp5F6};Cyc_aprintf(({const char*_tmp5F5="#%d";_tag_dyneither(_tmp5F5,sizeof(char),4);}),_tag_dyneither(_tmp5F4,sizeof(void*),1));});});
return({struct Cyc_Absyn_Tvar*_tmp5F2=_cycalloc(sizeof(*_tmp5F2));_tmp5F2->name=({struct _dyneither_ptr*_tmp5F3=_cycalloc(sizeof(struct _dyneither_ptr)* 1);_tmp5F3[0]=(struct _dyneither_ptr)s;_tmp5F3;});_tmp5F2->identity=- 1;_tmp5F2->kind=k;_tmp5F2;});}
# 3145
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _dyneither_ptr _tmp5F7=*t->name;
return*((const char*)_check_dyneither_subscript(_tmp5F7,sizeof(char),0))== '#';}
# 3150
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
({struct Cyc_String_pa_PrintArg_struct _tmp5FA;_tmp5FA.tag=0;_tmp5FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name);({void*_tmp5F8[1]={& _tmp5FA};Cyc_printf(({const char*_tmp5F9="%s";_tag_dyneither(_tmp5F9,sizeof(char),3);}),_tag_dyneither(_tmp5F8,sizeof(void*),1));});});
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
struct _dyneither_ptr _tmp5FB=Cyc_strconcat(({const char*_tmp600="`";_tag_dyneither(_tmp600,sizeof(char),2);}),(struct _dyneither_ptr)*t->name);
({struct _dyneither_ptr _tmp5FC=_dyneither_ptr_plus(_tmp5FB,sizeof(char),1);char _tmp5FD=*((char*)_check_dyneither_subscript(_tmp5FC,sizeof(char),0));char _tmp5FE='t';if(_get_dyneither_size(_tmp5FC,sizeof(char))== 1  && (_tmp5FD == '\000'  && _tmp5FE != '\000'))_throw_arraybounds();*((char*)_tmp5FC.curr)=_tmp5FE;});
t->name=({struct _dyneither_ptr*_tmp5FF=_cycalloc(sizeof(struct _dyneither_ptr)* 1);_tmp5FF[0]=(struct _dyneither_ptr)_tmp5FB;_tmp5FF;});};}
# 3159
static struct _tuple10*Cyc_Tcutil_fndecl2typ_f(struct _tuple10*x){
# 3161
return({struct _tuple10*_tmp601=_cycalloc(sizeof(*_tmp601));_tmp601->f1=(*x).f1;_tmp601->f2=(*x).f2;_tmp601->f3=(*x).f3;_tmp601;});}
# 3164
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_typ == 0){
# 3171
struct Cyc_List_List*_tmp602=0;
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd))
_tmp602=({struct Cyc_List_List*_tmp603=_cycalloc(sizeof(*_tmp603));_tmp603->hd=(void*)atts->hd;_tmp603->tl=_tmp602;_tmp603;});}}
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp604=_cycalloc(sizeof(*_tmp604));_tmp604[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp605;_tmp605.tag=9;_tmp605.f1=({struct Cyc_Absyn_FnInfo _tmp606;_tmp606.tvars=fd->tvs;_tmp606.effect=fd->effect;_tmp606.ret_tqual=fd->ret_tqual;_tmp606.ret_typ=fd->ret_type;_tmp606.args=
((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args);_tmp606.c_varargs=fd->c_varargs;_tmp606.cyc_varargs=fd->cyc_varargs;_tmp606.rgn_po=fd->rgn_po;_tmp606.attributes=_tmp602;_tmp606.requires_clause=fd->requires_clause;_tmp606.requires_relns=fd->requires_relns;_tmp606.ensures_clause=fd->ensures_clause;_tmp606.ensures_relns=fd->ensures_relns;_tmp606;});_tmp605;});_tmp604;});}
# 3184
return(void*)_check_null(fd->cached_typ);}
# 3190
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 3192
union Cyc_Relations_RelnOp _tmp607=*rop;union Cyc_Relations_RelnOp _tmp608=_tmp607;struct Cyc_Absyn_Vardecl*_tmp609;struct Cyc_Absyn_Vardecl*_tmp60A;switch((_tmp608.RNumelts).tag){case 2: _LL59E: _tmp60A=(_tmp608.RVar).val;_LL59F: {
# 3194
struct _tuple2 _tmp60B=*_tmp60A->name;struct _tuple2 _tmp60C=_tmp60B;union Cyc_Absyn_Nmspace _tmp60D;struct _dyneither_ptr*_tmp60E;_LL5A5: _tmp60D=_tmp60C.f1;_tmp60E=_tmp60C.f2;_LL5A6:;
if(!((int)((_tmp60D.Loc_n).tag == 4)))goto _LL59D;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp60E,({const char*_tmp60F="return_value";_tag_dyneither(_tmp60F,sizeof(char),13);}))== 0){
*rop=Cyc_Relations_RReturn();
goto _LL59D;}{
# 3200
unsigned int c=0;
{struct Cyc_List_List*_tmp610=args;for(0;_tmp610 != 0;(_tmp610=_tmp610->tl,c ++)){
struct _tuple10*_tmp611=(struct _tuple10*)_tmp610->hd;struct _tuple10*_tmp612=_tmp611;struct _dyneither_ptr*_tmp613;_LL5A8: _tmp613=_tmp612->f1;_LL5A9:;
if(_tmp613 != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp60E,(struct _dyneither_ptr)*_tmp613)== 0){
*rop=Cyc_Relations_RParam(c);
break;}}}}
# 3210
goto _LL59D;};}case 3: _LL5A0: _tmp609=(_tmp608.RNumelts).val;_LL5A1: {
# 3212
struct _tuple2 _tmp614=*_tmp609->name;struct _tuple2 _tmp615=_tmp614;union Cyc_Absyn_Nmspace _tmp616;struct _dyneither_ptr*_tmp617;_LL5AB: _tmp616=_tmp615.f1;_tmp617=_tmp615.f2;_LL5AC:;
if(!((int)((_tmp616.Loc_n).tag == 4)))goto _LL59D;{
unsigned int c=0;
{struct Cyc_List_List*_tmp618=args;for(0;_tmp618 != 0;(_tmp618=_tmp618->tl,c ++)){
struct _tuple10*_tmp619=(struct _tuple10*)_tmp618->hd;struct _tuple10*_tmp61A=_tmp619;struct _dyneither_ptr*_tmp61B;_LL5AE: _tmp61B=_tmp61A->f1;_LL5AF:;
if(_tmp61B != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp617,(struct _dyneither_ptr)*_tmp61B)== 0){
*rop=Cyc_Relations_RParamNumelts(c);
break;}}}}
# 3224
goto _LL59D;};}default: _LL5A2: _LL5A3:
 goto _LL59D;}_LL59D:;}
# 3229
static void Cyc_Tcutil_replace_rops(struct Cyc_List_List*args,struct Cyc_Relations_Reln*r){
# 3231
Cyc_Tcutil_replace_rop(args,& r->rop1);
Cyc_Tcutil_replace_rop(args,& r->rop2);}
# 3235
static struct Cyc_List_List*Cyc_Tcutil_extract_relns(struct Cyc_List_List*args,struct Cyc_Absyn_Exp*e){
# 3237
if(e == 0)return 0;{
struct Cyc_List_List*_tmp61C=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,_tmp61C);
return _tmp61C;};}struct _tuple25{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 3244
static void*Cyc_Tcutil_fst_fdarg(struct _tuple25*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct _tuple12*_tmp61D=pr;struct Cyc_Absyn_Tqual _tmp61E;void*_tmp61F;_LL5B1: _tmp61E=_tmp61D->f1;_tmp61F=_tmp61D->f2;_LL5B2:;
if(_tmp61F == t)return pr;else{
return({struct _tuple12*_tmp620=_cycalloc(sizeof(*_tmp620));_tmp620->f1=_tmp61E;_tmp620->f2=t;_tmp620;});}}struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 3251
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
# 3253
return({struct _tuple27*_tmp621=_region_malloc(rgn,sizeof(*_tmp621));_tmp621->f1=({struct _tuple26*_tmp622=_region_malloc(rgn,sizeof(*_tmp622));_tmp622->f1=(*y).f1;_tmp622->f2=(*y).f2;_tmp622;});_tmp621->f2=(*y).f3;_tmp621;});}
# 3255
static struct _tuple10*Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){
# 3257
struct _tuple10 _tmp623=*orig_arg;struct _tuple10 _tmp624=_tmp623;struct _dyneither_ptr*_tmp625;struct Cyc_Absyn_Tqual _tmp626;void*_tmp627;_LL5B4: _tmp625=_tmp624.f1;_tmp626=_tmp624.f2;_tmp627=_tmp624.f3;_LL5B5:;
if(t == _tmp627)return orig_arg;
return({struct _tuple10*_tmp628=_cycalloc(sizeof(*_tmp628));_tmp628->f1=_tmp625;_tmp628->f2=_tmp626;_tmp628->f3=t;_tmp628;});}
# 3261
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){
return f->type;}
# 3264
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 3269
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 3271
return({struct Cyc_Absyn_Exp*_tmp629=_cycalloc(sizeof(*_tmp629));_tmp629->topt=old->topt;_tmp629->r=r;_tmp629->loc=old->loc;_tmp629->annot=old->annot;_tmp629;});}
# 3276
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp62A=e->r;void*_tmp62B=_tmp62A;void*_tmp62C;void*_tmp62D;struct Cyc_List_List*_tmp62E;struct Cyc_Absyn_Exp*_tmp62F;void*_tmp630;void*_tmp631;struct Cyc_Absyn_Exp*_tmp632;int _tmp633;enum Cyc_Absyn_Coercion _tmp634;struct Cyc_Absyn_Exp*_tmp635;struct Cyc_Absyn_Exp*_tmp636;struct Cyc_Absyn_Exp*_tmp637;struct Cyc_Absyn_Exp*_tmp638;struct Cyc_Absyn_Exp*_tmp639;struct Cyc_Absyn_Exp*_tmp63A;struct Cyc_Absyn_Exp*_tmp63B;struct Cyc_Absyn_Exp*_tmp63C;struct Cyc_Absyn_Exp*_tmp63D;enum Cyc_Absyn_Primop _tmp63E;struct Cyc_List_List*_tmp63F;switch(*((int*)_tmp62B)){case 0: _LL5B7: _LL5B8:
 goto _LL5BA;case 31: _LL5B9: _LL5BA:
 goto _LL5BC;case 32: _LL5BB: _LL5BC:
 goto _LL5BE;case 1: _LL5BD: _LL5BE:
 return e;case 2: _LL5BF: _tmp63E=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp62B)->f1;_tmp63F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp62B)->f2;_LL5C0:
# 3284
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp63F)== 1){
struct Cyc_Absyn_Exp*_tmp640=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp63F))->hd;
struct Cyc_Absyn_Exp*_tmp641=Cyc_Tcutil_rsubsexp(r,inst,_tmp640);
if(_tmp641 == _tmp640)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp642=_cycalloc(sizeof(*_tmp642));_tmp642[0]=({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp643;_tmp643.tag=2;_tmp643.f1=_tmp63E;_tmp643.f2=({struct Cyc_Absyn_Exp*_tmp644[1];_tmp644[0]=_tmp641;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp644,sizeof(struct Cyc_Absyn_Exp*),1));});_tmp643;});_tmp642;}));}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp63F)== 2){
struct Cyc_Absyn_Exp*_tmp645=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp63F))->hd;
struct Cyc_Absyn_Exp*_tmp646=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp63F->tl))->hd;
struct Cyc_Absyn_Exp*_tmp647=Cyc_Tcutil_rsubsexp(r,inst,_tmp645);
struct Cyc_Absyn_Exp*_tmp648=Cyc_Tcutil_rsubsexp(r,inst,_tmp646);
if(_tmp647 == _tmp645  && _tmp648 == _tmp646)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp649=_cycalloc(sizeof(*_tmp649));_tmp649[0]=({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp64A;_tmp64A.tag=2;_tmp64A.f1=_tmp63E;_tmp64A.f2=({struct Cyc_Absyn_Exp*_tmp64B[2];_tmp64B[1]=_tmp648;_tmp64B[0]=_tmp647;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp64B,sizeof(struct Cyc_Absyn_Exp*),2));});_tmp64A;});_tmp649;}));}else{
return({void*_tmp64C=0;((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp64D="primop does not have 1 or 2 args!";_tag_dyneither(_tmp64D,sizeof(char),34);}),_tag_dyneither(_tmp64C,sizeof(void*),0));});}}case 5: _LL5C1: _tmp63B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp62B)->f1;_tmp63C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp62B)->f2;_tmp63D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp62B)->f3;_LL5C2: {
# 3298
struct Cyc_Absyn_Exp*_tmp64E=Cyc_Tcutil_rsubsexp(r,inst,_tmp63B);
struct Cyc_Absyn_Exp*_tmp64F=Cyc_Tcutil_rsubsexp(r,inst,_tmp63C);
struct Cyc_Absyn_Exp*_tmp650=Cyc_Tcutil_rsubsexp(r,inst,_tmp63D);
if((_tmp64E == _tmp63B  && _tmp64F == _tmp63C) && _tmp650 == _tmp63D)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp651=_cycalloc(sizeof(*_tmp651));_tmp651[0]=({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp652;_tmp652.tag=5;_tmp652.f1=_tmp64E;_tmp652.f2=_tmp64F;_tmp652.f3=_tmp650;_tmp652;});_tmp651;}));}case 6: _LL5C3: _tmp639=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp62B)->f1;_tmp63A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp62B)->f2;_LL5C4: {
# 3304
struct Cyc_Absyn_Exp*_tmp653=Cyc_Tcutil_rsubsexp(r,inst,_tmp639);
struct Cyc_Absyn_Exp*_tmp654=Cyc_Tcutil_rsubsexp(r,inst,_tmp63A);
if(_tmp653 == _tmp639  && _tmp654 == _tmp63A)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp655=_cycalloc(sizeof(*_tmp655));_tmp655[0]=({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmp656;_tmp656.tag=6;_tmp656.f1=_tmp653;_tmp656.f2=_tmp654;_tmp656;});_tmp655;}));}case 7: _LL5C5: _tmp637=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp62B)->f1;_tmp638=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp62B)->f2;_LL5C6: {
# 3309
struct Cyc_Absyn_Exp*_tmp657=Cyc_Tcutil_rsubsexp(r,inst,_tmp637);
struct Cyc_Absyn_Exp*_tmp658=Cyc_Tcutil_rsubsexp(r,inst,_tmp638);
if(_tmp657 == _tmp637  && _tmp658 == _tmp638)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp659=_cycalloc(sizeof(*_tmp659));_tmp659[0]=({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmp65A;_tmp65A.tag=7;_tmp65A.f1=_tmp657;_tmp65A.f2=_tmp658;_tmp65A;});_tmp659;}));}case 8: _LL5C7: _tmp635=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp62B)->f1;_tmp636=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp62B)->f2;_LL5C8: {
# 3314
struct Cyc_Absyn_Exp*_tmp65B=Cyc_Tcutil_rsubsexp(r,inst,_tmp635);
struct Cyc_Absyn_Exp*_tmp65C=Cyc_Tcutil_rsubsexp(r,inst,_tmp636);
if(_tmp65B == _tmp635  && _tmp65C == _tmp636)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp65D=_cycalloc(sizeof(*_tmp65D));_tmp65D[0]=({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmp65E;_tmp65E.tag=8;_tmp65E.f1=_tmp65B;_tmp65E.f2=_tmp65C;_tmp65E;});_tmp65D;}));}case 13: _LL5C9: _tmp631=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp62B)->f1;_tmp632=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp62B)->f2;_tmp633=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp62B)->f3;_tmp634=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp62B)->f4;_LL5CA: {
# 3319
struct Cyc_Absyn_Exp*_tmp65F=Cyc_Tcutil_rsubsexp(r,inst,_tmp632);
void*_tmp660=Cyc_Tcutil_rsubstitute(r,inst,_tmp631);
if(_tmp65F == _tmp632  && _tmp660 == _tmp631)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp661=_cycalloc(sizeof(*_tmp661));_tmp661[0]=({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp662;_tmp662.tag=13;_tmp662.f1=_tmp660;_tmp662.f2=_tmp65F;_tmp662.f3=_tmp633;_tmp662.f4=_tmp634;_tmp662;});_tmp661;}));}case 16: _LL5CB: _tmp630=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp62B)->f1;_LL5CC: {
# 3324
void*_tmp663=Cyc_Tcutil_rsubstitute(r,inst,_tmp630);
if(_tmp663 == _tmp630)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp664=_cycalloc(sizeof(*_tmp664));_tmp664[0]=({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp665;_tmp665.tag=16;_tmp665.f1=_tmp663;_tmp665;});_tmp664;}));}case 17: _LL5CD: _tmp62F=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp62B)->f1;_LL5CE: {
# 3328
struct Cyc_Absyn_Exp*_tmp666=Cyc_Tcutil_rsubsexp(r,inst,_tmp62F);
if(_tmp666 == _tmp62F)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp667=_cycalloc(sizeof(*_tmp667));_tmp667[0]=({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmp668;_tmp668.tag=17;_tmp668.f1=_tmp666;_tmp668;});_tmp667;}));}case 18: _LL5CF: _tmp62D=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp62B)->f1;_tmp62E=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp62B)->f2;_LL5D0: {
# 3332
void*_tmp669=Cyc_Tcutil_rsubstitute(r,inst,_tmp62D);
if(_tmp669 == _tmp62D)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp66A=_cycalloc(sizeof(*_tmp66A));_tmp66A[0]=({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp66B;_tmp66B.tag=18;_tmp66B.f1=_tmp669;_tmp66B.f2=_tmp62E;_tmp66B;});_tmp66A;}));}case 38: _LL5D1: _tmp62C=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp62B)->f1;_LL5D2: {
# 3336
void*_tmp66C=Cyc_Tcutil_rsubstitute(r,inst,_tmp62C);
if(_tmp66C == _tmp62C)return e;{
# 3339
void*_tmp66D=Cyc_Tcutil_compress(_tmp66C);void*_tmp66E=_tmp66D;struct Cyc_Absyn_Exp*_tmp66F;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp66E)->tag == 18){_LL5D6: _tmp66F=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp66E)->f1;_LL5D7:
 return _tmp66F;}else{_LL5D8: _LL5D9:
# 3342
 return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp670=_cycalloc(sizeof(*_tmp670));_tmp670[0]=({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp671;_tmp671.tag=38;_tmp671.f1=_tmp66C;_tmp671;});_tmp670;}));}_LL5D5:;};}default: _LL5D3: _LL5D4:
# 3345
 return({void*_tmp672=0;((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp673="non-type-level-expression in Tcutil::rsubsexp";_tag_dyneither(_tmp673,sizeof(char),46);}),_tag_dyneither(_tmp672,sizeof(void*),0));});}_LL5B6:;}
# 3349
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 3352
if(e == 0)return 0;else{
return Cyc_Tcutil_rsubsexp(r,inst,e);}}
# 3356
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 3359
void*_tmp674=f->type;
struct Cyc_Absyn_Exp*_tmp675=f->requires_clause;
void*_tmp676=Cyc_Tcutil_rsubstitute(r,inst,_tmp674);
struct Cyc_Absyn_Exp*_tmp677=Cyc_Tcutil_rsubs_exp_opt(r,inst,_tmp675);
if(_tmp674 == _tmp676  && _tmp675 == _tmp677)return f;else{
return({struct Cyc_Absyn_Aggrfield*_tmp678=_cycalloc(sizeof(*_tmp678));_tmp678->name=f->name;_tmp678->tq=f->tq;_tmp678->type=_tmp676;_tmp678->width=f->width;_tmp678->attributes=f->attributes;_tmp678->requires_clause=_tmp677;_tmp678;});}}
# 3369
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 3372
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp679=(struct Cyc_Absyn_Aggrfield*)fs->hd;
struct Cyc_List_List*_tmp67A=fs->tl;
struct Cyc_Absyn_Aggrfield*_tmp67B=Cyc_Tcutil_subst_aggrfield(r,inst,_tmp679);
struct Cyc_List_List*_tmp67C=Cyc_Tcutil_subst_aggrfields(r,inst,_tmp67A);
if(_tmp67B == _tmp679  && _tmp67C == _tmp67A)return fs;
# 3379
return({struct Cyc_List_List*_tmp67D=_cycalloc(sizeof(*_tmp67D));_tmp67D->hd=_tmp67B;_tmp67D->tl=_tmp67C;_tmp67D;});};}
# 3382
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 3385
struct _tuple1 _tmp67E=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct _tuple1 _tmp67F=_tmp67E;struct Cyc_List_List*_tmp680;struct Cyc_List_List*_tmp681;_LL5DB: _tmp680=_tmp67F.f1;_tmp681=_tmp67F.f2;_LL5DC:;{
struct Cyc_List_List*_tmp682=Cyc_Tcutil_substs(rgn,inst,_tmp680);
struct Cyc_List_List*_tmp683=Cyc_Tcutil_substs(rgn,inst,_tmp681);
if(_tmp682 == _tmp680  && _tmp683 == _tmp681)
return rgn_po;else{
# 3391
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp682,_tmp683);}};}
# 3394
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3397
void*_tmp684=Cyc_Tcutil_compress(t);void*_tmp685=_tmp684;struct Cyc_List_List*_tmp686;void*_tmp687;void*_tmp688;struct Cyc_Absyn_Exp*_tmp689;struct Cyc_Absyn_Exp*_tmp68A;void*_tmp68B;void*_tmp68C;void*_tmp68D;void*_tmp68E;void*_tmp68F;enum Cyc_Absyn_AggrKind _tmp690;struct Cyc_List_List*_tmp691;struct Cyc_List_List*_tmp692;struct Cyc_List_List*_tmp693;void*_tmp694;struct Cyc_Absyn_Tqual _tmp695;void*_tmp696;struct Cyc_List_List*_tmp697;int _tmp698;struct Cyc_Absyn_VarargInfo*_tmp699;struct Cyc_List_List*_tmp69A;struct Cyc_List_List*_tmp69B;struct Cyc_Absyn_Exp*_tmp69C;struct Cyc_Absyn_Exp*_tmp69D;void*_tmp69E;struct Cyc_Absyn_Tqual _tmp69F;void*_tmp6A0;union Cyc_Absyn_Constraint*_tmp6A1;union Cyc_Absyn_Constraint*_tmp6A2;union Cyc_Absyn_Constraint*_tmp6A3;void*_tmp6A4;struct Cyc_Absyn_Tqual _tmp6A5;struct Cyc_Absyn_Exp*_tmp6A6;union Cyc_Absyn_Constraint*_tmp6A7;unsigned int _tmp6A8;struct _tuple2*_tmp6A9;struct Cyc_List_List*_tmp6AA;struct Cyc_Absyn_Typedefdecl*_tmp6AB;void*_tmp6AC;union Cyc_Absyn_DatatypeFieldInfoU _tmp6AD;struct Cyc_List_List*_tmp6AE;union Cyc_Absyn_DatatypeInfoU _tmp6AF;struct Cyc_List_List*_tmp6B0;union Cyc_Absyn_AggrInfoU _tmp6B1;struct Cyc_List_List*_tmp6B2;struct Cyc_Absyn_Tvar*_tmp6B3;switch(*((int*)_tmp685)){case 2: _LL5DE: _tmp6B3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp685)->f1;_LL5DF: {
# 3400
struct _handler_cons _tmp6B4;_push_handler(& _tmp6B4);{int _tmp6B6=0;if(setjmp(_tmp6B4.handler))_tmp6B6=1;if(!_tmp6B6){{void*_tmp6B7=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp6B3);_npop_handler(0);return _tmp6B7;};_pop_handler();}else{void*_tmp6B5=(void*)_exn_thrown;void*_tmp6B8=_tmp6B5;void*_tmp6B9;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp6B8)->tag == Cyc_Core_Not_found){_LL619: _LL61A:
 return t;}else{_LL61B: _tmp6B9=_tmp6B8;_LL61C:(int)_rethrow(_tmp6B9);}_LL618:;}};}case 11: _LL5E0: _tmp6B1=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp685)->f1).aggr_info;_tmp6B2=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp685)->f1).targs;_LL5E1: {
# 3403
struct Cyc_List_List*_tmp6BA=Cyc_Tcutil_substs(rgn,inst,_tmp6B2);
return _tmp6BA == _tmp6B2?t:(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp6BB=_cycalloc(sizeof(*_tmp6BB));_tmp6BB[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp6BC;_tmp6BC.tag=11;_tmp6BC.f1=({struct Cyc_Absyn_AggrInfo _tmp6BD;_tmp6BD.aggr_info=_tmp6B1;_tmp6BD.targs=_tmp6BA;_tmp6BD;});_tmp6BC;});_tmp6BB;});}case 3: _LL5E2: _tmp6AF=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp685)->f1).datatype_info;_tmp6B0=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp685)->f1).targs;_LL5E3: {
# 3406
struct Cyc_List_List*_tmp6BE=Cyc_Tcutil_substs(rgn,inst,_tmp6B0);
return _tmp6BE == _tmp6B0?t:(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp6BF=_cycalloc(sizeof(*_tmp6BF));_tmp6BF[0]=({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp6C0;_tmp6C0.tag=3;_tmp6C0.f1=({struct Cyc_Absyn_DatatypeInfo _tmp6C1;_tmp6C1.datatype_info=_tmp6AF;_tmp6C1.targs=_tmp6BE;_tmp6C1;});_tmp6C0;});_tmp6BF;});}case 4: _LL5E4: _tmp6AD=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp685)->f1).field_info;_tmp6AE=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp685)->f1).targs;_LL5E5: {
# 3410
struct Cyc_List_List*_tmp6C2=Cyc_Tcutil_substs(rgn,inst,_tmp6AE);
return _tmp6C2 == _tmp6AE?t:(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp6C3=_cycalloc(sizeof(*_tmp6C3));_tmp6C3[0]=({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp6C4;_tmp6C4.tag=4;_tmp6C4.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmp6C5;_tmp6C5.field_info=_tmp6AD;_tmp6C5.targs=_tmp6C2;_tmp6C5;});_tmp6C4;});_tmp6C3;});}case 17: _LL5E6: _tmp6A9=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp685)->f1;_tmp6AA=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp685)->f2;_tmp6AB=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp685)->f3;_tmp6AC=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp685)->f4;_LL5E7: {
# 3414
struct Cyc_List_List*_tmp6C6=Cyc_Tcutil_substs(rgn,inst,_tmp6AA);
return _tmp6C6 == _tmp6AA?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp6C7=_cycalloc(sizeof(*_tmp6C7));_tmp6C7[0]=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp6C8;_tmp6C8.tag=17;_tmp6C8.f1=_tmp6A9;_tmp6C8.f2=_tmp6C6;_tmp6C8.f3=_tmp6AB;_tmp6C8.f4=_tmp6AC;_tmp6C8;});_tmp6C7;});}case 8: _LL5E8: _tmp6A4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp685)->f1).elt_type;_tmp6A5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp685)->f1).tq;_tmp6A6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp685)->f1).num_elts;_tmp6A7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp685)->f1).zero_term;_tmp6A8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp685)->f1).zt_loc;_LL5E9: {
# 3417
void*_tmp6C9=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6A4);
struct Cyc_Absyn_Exp*_tmp6CA=_tmp6A6 == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,_tmp6A6);
return _tmp6C9 == _tmp6A4  && _tmp6CA == _tmp6A6?t:(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp6CB=_cycalloc(sizeof(*_tmp6CB));_tmp6CB[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp6CC;_tmp6CC.tag=8;_tmp6CC.f1=({struct Cyc_Absyn_ArrayInfo _tmp6CD;_tmp6CD.elt_type=_tmp6C9;_tmp6CD.tq=_tmp6A5;_tmp6CD.num_elts=_tmp6CA;_tmp6CD.zero_term=_tmp6A7;_tmp6CD.zt_loc=_tmp6A8;_tmp6CD;});_tmp6CC;});_tmp6CB;});}case 5: _LL5EA: _tmp69E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp685)->f1).elt_typ;_tmp69F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp685)->f1).elt_tq;_tmp6A0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp685)->f1).ptr_atts).rgn;_tmp6A1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp685)->f1).ptr_atts).nullable;_tmp6A2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp685)->f1).ptr_atts).bounds;_tmp6A3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp685)->f1).ptr_atts).zero_term;_LL5EB: {
# 3422
void*_tmp6CE=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp69E);
void*_tmp6CF=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6A0);
union Cyc_Absyn_Constraint*_tmp6D0=_tmp6A2;
{void*_tmp6D1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp6A2);void*_tmp6D2=_tmp6D1;struct Cyc_Absyn_Exp*_tmp6D3;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6D2)->tag == 1){_LL61E: _tmp6D3=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6D2)->f1;_LL61F: {
# 3427
struct Cyc_Absyn_Exp*_tmp6D4=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp6D3);
if(_tmp6D4 != _tmp6D3)
_tmp6D0=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6D5=_cycalloc(sizeof(*_tmp6D5));_tmp6D5[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp6D6;_tmp6D6.tag=1;_tmp6D6.f1=_tmp6D4;_tmp6D6;});_tmp6D5;}));
goto _LL61D;}}else{_LL620: _LL621:
 goto _LL61D;}_LL61D:;}
# 3433
if((_tmp6CE == _tmp69E  && _tmp6CF == _tmp6A0) && _tmp6D0 == _tmp6A2)
return t;
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6D7=_cycalloc(sizeof(*_tmp6D7));_tmp6D7[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp6D8;_tmp6D8.tag=5;_tmp6D8.f1=({struct Cyc_Absyn_PtrInfo _tmp6D9;_tmp6D9.elt_typ=_tmp6CE;_tmp6D9.elt_tq=_tmp69F;_tmp6D9.ptr_atts=({(_tmp6D9.ptr_atts).rgn=_tmp6CF;(_tmp6D9.ptr_atts).nullable=_tmp6A1;(_tmp6D9.ptr_atts).bounds=_tmp6D0;(_tmp6D9.ptr_atts).zero_term=_tmp6A3;(_tmp6D9.ptr_atts).ptrloc=0;_tmp6D9.ptr_atts;});_tmp6D9;});_tmp6D8;});_tmp6D7;});}case 9: _LL5EC: _tmp693=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp685)->f1).tvars;_tmp694=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp685)->f1).effect;_tmp695=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp685)->f1).ret_tqual;_tmp696=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp685)->f1).ret_typ;_tmp697=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp685)->f1).args;_tmp698=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp685)->f1).c_varargs;_tmp699=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp685)->f1).cyc_varargs;_tmp69A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp685)->f1).rgn_po;_tmp69B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp685)->f1).attributes;_tmp69C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp685)->f1).requires_clause;_tmp69D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp685)->f1).ensures_clause;_LL5ED:
# 3439
{struct Cyc_List_List*_tmp6DA=_tmp693;for(0;_tmp6DA != 0;_tmp6DA=_tmp6DA->tl){
inst=({struct Cyc_List_List*_tmp6DB=_region_malloc(rgn,sizeof(*_tmp6DB));_tmp6DB->hd=({struct _tuple16*_tmp6DC=_region_malloc(rgn,sizeof(*_tmp6DC));_tmp6DC->f1=(struct Cyc_Absyn_Tvar*)_tmp6DA->hd;_tmp6DC->f2=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp6DD=_cycalloc(sizeof(*_tmp6DD));_tmp6DD[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp6DE;_tmp6DE.tag=2;_tmp6DE.f1=(struct Cyc_Absyn_Tvar*)_tmp6DA->hd;_tmp6DE;});_tmp6DD;});_tmp6DC;});_tmp6DB->tl=inst;_tmp6DB;});}}{
struct _tuple1 _tmp6DF=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp697));
# 3441
struct _tuple1 _tmp6E0=_tmp6DF;struct Cyc_List_List*_tmp6E1;struct Cyc_List_List*_tmp6E2;_LL623: _tmp6E1=_tmp6E0.f1;_tmp6E2=_tmp6E0.f2;_LL624:;{
# 3443
struct Cyc_List_List*_tmp6E3=_tmp697;
struct Cyc_List_List*_tmp6E4=Cyc_Tcutil_substs(rgn,inst,_tmp6E2);
if(_tmp6E4 != _tmp6E2)
_tmp6E3=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmp697,_tmp6E4);{
void*eff2;
if(_tmp694 == 0)
eff2=0;else{
# 3451
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp694);
if(new_eff == _tmp694)
eff2=_tmp694;else{
# 3455
eff2=new_eff;}}{
# 3457
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(_tmp699 == 0)
cyc_varargs2=0;else{
# 3461
struct Cyc_Absyn_VarargInfo _tmp6E5=*_tmp699;struct Cyc_Absyn_VarargInfo _tmp6E6=_tmp6E5;struct _dyneither_ptr*_tmp6E7;struct Cyc_Absyn_Tqual _tmp6E8;void*_tmp6E9;int _tmp6EA;_LL626: _tmp6E7=_tmp6E6.name;_tmp6E8=_tmp6E6.tq;_tmp6E9=_tmp6E6.type;_tmp6EA=_tmp6E6.inject;_LL627:;{
void*_tmp6EB=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6E9);
if(_tmp6EB == _tmp6E9)cyc_varargs2=_tmp699;else{
# 3465
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp6EC=_cycalloc(sizeof(*_tmp6EC));_tmp6EC->name=_tmp6E7;_tmp6EC->tq=_tmp6E8;_tmp6EC->type=_tmp6EB;_tmp6EC->inject=_tmp6EA;_tmp6EC;});}};}{
# 3467
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp69A);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp69C);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp69D);
struct Cyc_List_List*_tmp6ED=Cyc_Tcutil_extract_relns(_tmp6E3,req2);
struct Cyc_List_List*_tmp6EE=Cyc_Tcutil_extract_relns(_tmp6E3,ens2);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp6EF=_cycalloc(sizeof(*_tmp6EF));_tmp6EF[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp6F0;_tmp6F0.tag=9;_tmp6F0.f1=({struct Cyc_Absyn_FnInfo _tmp6F1;_tmp6F1.tvars=_tmp693;_tmp6F1.effect=eff2;_tmp6F1.ret_tqual=_tmp695;_tmp6F1.ret_typ=
Cyc_Tcutil_rsubstitute(rgn,inst,_tmp696);_tmp6F1.args=_tmp6E3;_tmp6F1.c_varargs=_tmp698;_tmp6F1.cyc_varargs=cyc_varargs2;_tmp6F1.rgn_po=rgn_po2;_tmp6F1.attributes=_tmp69B;_tmp6F1.requires_clause=req2;_tmp6F1.requires_relns=_tmp6ED;_tmp6F1.ensures_clause=ens2;_tmp6F1.ensures_relns=_tmp6EE;_tmp6F1;});_tmp6F0;});_tmp6EF;});};};};};};case 10: _LL5EE: _tmp692=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp685)->f1;_LL5EF: {
# 3477
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp6F2=_tmp692;for(0;_tmp6F2 != 0;_tmp6F2=_tmp6F2->tl){
void*_tmp6F3=(*((struct _tuple12*)_tmp6F2->hd)).f2;
void*_tmp6F4=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6F3);
if(_tmp6F3 != _tmp6F4)
change=1;
# 3485
ts2=({struct Cyc_List_List*_tmp6F5=_region_malloc(rgn,sizeof(*_tmp6F5));_tmp6F5->hd=_tmp6F4;_tmp6F5->tl=ts2;_tmp6F5;});}}
# 3487
if(!change)
return t;{
struct Cyc_List_List*_tmp6F6=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp692,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6F7=_cycalloc(sizeof(*_tmp6F7));_tmp6F7[0]=({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp6F8;_tmp6F8.tag=10;_tmp6F8.f1=_tmp6F6;_tmp6F8;});_tmp6F7;});};}case 12: _LL5F0: _tmp690=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp685)->f1;_tmp691=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp685)->f2;_LL5F1: {
# 3492
struct Cyc_List_List*_tmp6F9=Cyc_Tcutil_subst_aggrfields(rgn,inst,_tmp691);
if(_tmp691 == _tmp6F9)return t;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp6FA=_cycalloc(sizeof(*_tmp6FA));_tmp6FA[0]=({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp6FB;_tmp6FB.tag=12;_tmp6FB.f1=_tmp690;_tmp6FB.f2=_tmp6F9;_tmp6FB;});_tmp6FA;});}case 1: _LL5F2: _tmp68F=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp685)->f2;_LL5F3:
# 3496
 if(_tmp68F != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp68F);else{
return t;}case 15: _LL5F4: _tmp68E=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp685)->f1;_LL5F5: {
# 3499
void*_tmp6FC=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp68E);
return _tmp6FC == _tmp68E?t:(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp6FD=_cycalloc(sizeof(*_tmp6FD));_tmp6FD[0]=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp6FE;_tmp6FE.tag=15;_tmp6FE.f1=_tmp6FC;_tmp6FE;});_tmp6FD;});}case 16: _LL5F6: _tmp68C=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp685)->f1;_tmp68D=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp685)->f2;_LL5F7: {
# 3502
void*_tmp6FF=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp68C);
void*_tmp700=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp68D);
return _tmp6FF == _tmp68C  && _tmp700 == _tmp68D?t:(void*)({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp701=_cycalloc(sizeof(*_tmp701));_tmp701[0]=({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp702;_tmp702.tag=16;_tmp702.f1=_tmp6FF;_tmp702.f2=_tmp700;_tmp702;});_tmp701;});}case 19: _LL5F8: _tmp68B=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp685)->f1;_LL5F9: {
# 3506
void*_tmp703=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp68B);
return _tmp703 == _tmp68B?t:(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp704=_cycalloc(sizeof(*_tmp704));_tmp704[0]=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp705;_tmp705.tag=19;_tmp705.f1=_tmp703;_tmp705;});_tmp704;});}case 18: _LL5FA: _tmp68A=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp685)->f1;_LL5FB: {
# 3509
struct Cyc_Absyn_Exp*_tmp706=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp68A);
return _tmp706 == _tmp68A?t:(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp707=_cycalloc(sizeof(*_tmp707));_tmp707[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp708;_tmp708.tag=18;_tmp708.f1=_tmp706;_tmp708;});_tmp707;});}case 27: _LL5FC: _tmp689=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp685)->f1;_LL5FD: {
# 3512
struct Cyc_Absyn_Exp*_tmp709=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp689);
return _tmp709 == _tmp689?t:(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp70A=_cycalloc(sizeof(*_tmp70A));_tmp70A[0]=({struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmp70B;_tmp70B.tag=27;_tmp70B.f1=_tmp709;_tmp70B;});_tmp70A;});}case 13: _LL5FE: _LL5FF:
 goto _LL601;case 14: _LL600: _LL601:
 goto _LL603;case 0: _LL602: _LL603:
 goto _LL605;case 6: _LL604: _LL605:
 goto _LL607;case 7: _LL606: _LL607:
 goto _LL609;case 22: _LL608: _LL609:
 goto _LL60B;case 21: _LL60A: _LL60B:
 goto _LL60D;case 28: _LL60C: _LL60D:
 goto _LL60F;case 20: _LL60E: _LL60F:
 return t;case 25: _LL610: _tmp688=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp685)->f1;_LL611: {
# 3524
void*_tmp70C=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp688);
return _tmp70C == _tmp688?t:(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp70D=_cycalloc(sizeof(*_tmp70D));_tmp70D[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp70E;_tmp70E.tag=25;_tmp70E.f1=_tmp70C;_tmp70E;});_tmp70D;});}case 23: _LL612: _tmp687=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp685)->f1;_LL613: {
# 3527
void*_tmp70F=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp687);
return _tmp70F == _tmp687?t:(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp710=_cycalloc(sizeof(*_tmp710));_tmp710[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp711;_tmp711.tag=23;_tmp711.f1=_tmp70F;_tmp711;});_tmp710;});}case 24: _LL614: _tmp686=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp685)->f1;_LL615: {
# 3530
struct Cyc_List_List*_tmp712=Cyc_Tcutil_substs(rgn,inst,_tmp686);
return _tmp712 == _tmp686?t:(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp713=_cycalloc(sizeof(*_tmp713));_tmp713[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp714;_tmp714.tag=24;_tmp714.f1=_tmp712;_tmp714;});_tmp713;});}default: _LL616: _LL617:
({void*_tmp715=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp716="found typedecltype in rsubs";_tag_dyneither(_tmp716,sizeof(char),28);}),_tag_dyneither(_tmp715,sizeof(void*),0));});}_LL5DD:;}
# 3536
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 3539
if(ts == 0)
return 0;{
void*_tmp717=(void*)ts->hd;
struct Cyc_List_List*_tmp718=ts->tl;
void*_tmp719=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp717);
struct Cyc_List_List*_tmp71A=Cyc_Tcutil_substs(rgn,inst,_tmp718);
if(_tmp717 == _tmp719  && _tmp718 == _tmp71A)
return ts;
return({struct Cyc_List_List*_tmp71B=_cycalloc(sizeof(*_tmp71B));_tmp71B->hd=_tmp719;_tmp71B->tl=_tmp71A;_tmp71B;});};}
# 3550
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 3557
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp71C=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple16*_tmp71D=_cycalloc(sizeof(*_tmp71D));_tmp71D->f1=tv;_tmp71D->f2=Cyc_Absyn_new_evar(_tmp71C,({struct Cyc_Core_Opt*_tmp71E=_cycalloc(sizeof(*_tmp71E));_tmp71E->v=s;_tmp71E;}));_tmp71D;});}
# 3562
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*env,struct Cyc_Absyn_Tvar*tv){
# 3564
struct _tuple17*_tmp71F=env;struct Cyc_List_List*_tmp720;struct _RegionHandle*_tmp721;_LL629: _tmp720=_tmp71F->f1;_tmp721=_tmp71F->f2;_LL62A:;{
struct Cyc_Core_Opt*_tmp722=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple16*_tmp723=_region_malloc(_tmp721,sizeof(*_tmp723));_tmp723->f1=tv;_tmp723->f2=Cyc_Absyn_new_evar(_tmp722,({struct Cyc_Core_Opt*_tmp724=_cycalloc(sizeof(*_tmp724));_tmp724->v=_tmp720;_tmp724;}));_tmp723;});};}
# 3574
static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(unsigned int loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3578
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2))
({struct Cyc_String_pa_PrintArg_struct _tmp729;_tmp729.tag=0;_tmp729.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(k2));({struct Cyc_String_pa_PrintArg_struct _tmp728;_tmp728.tag=0;_tmp728.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1));({struct Cyc_String_pa_PrintArg_struct _tmp727;_tmp727.tag=0;_tmp727.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name);({void*_tmp725[3]={& _tmp727,& _tmp728,& _tmp729};Cyc_Tcutil_terr(loc,({const char*_tmp726="type variable %s is used with inconsistent kinds %s and %s";_tag_dyneither(_tmp726,sizeof(char),59);}),_tag_dyneither(_tmp725,sizeof(void*),3));});});});});
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)
({void*_tmp72A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp72B="same type variable has different identity!";_tag_dyneither(_tmp72B,sizeof(char),43);}),_tag_dyneither(_tmp72A,sizeof(void*),0));});}
return tvs;}}}
# 3592
tv->identity=Cyc_Tcutil_new_tvar_id();
return({struct Cyc_List_List*_tmp72C=_cycalloc(sizeof(*_tmp72C));_tmp72C->hd=tv;_tmp72C->tl=tvs;_tmp72C;});}
# 3598
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3600
if(tv->identity == - 1)
({void*_tmp72D=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp72E="fast_add_free_tvar: bad identity in tv";_tag_dyneither(_tmp72E,sizeof(char),39);}),_tag_dyneither(_tmp72D,sizeof(void*),0));});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3604
struct Cyc_Absyn_Tvar*_tmp72F=(struct Cyc_Absyn_Tvar*)tvs2->hd;
if(_tmp72F->identity == - 1)
({void*_tmp730=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp731="fast_add_free_tvar: bad identity in tvs2";_tag_dyneither(_tmp731,sizeof(char),41);}),_tag_dyneither(_tmp730,sizeof(void*),0));});
if(_tmp72F->identity == tv->identity)
return tvs;}}
# 3611
return({struct Cyc_List_List*_tmp732=_cycalloc(sizeof(*_tmp732));_tmp732->hd=tv;_tmp732->tl=tvs;_tmp732;});}struct _tuple28{struct Cyc_Absyn_Tvar*f1;int f2;};
# 3617
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 3622
if(tv->identity == - 1)
({void*_tmp733=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp734="fast_add_free_tvar_bool: bad identity in tv";_tag_dyneither(_tmp734,sizeof(char),44);}),_tag_dyneither(_tmp733,sizeof(void*),0));});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3626
struct _tuple28*_tmp735=(struct _tuple28*)tvs2->hd;struct _tuple28*_tmp736=_tmp735;struct Cyc_Absyn_Tvar*_tmp737;int*_tmp738;_LL62C: _tmp737=_tmp736->f1;_tmp738=(int*)& _tmp736->f2;_LL62D:;
if(_tmp737->identity == - 1)
({void*_tmp739=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp73A="fast_add_free_tvar_bool: bad identity in tvs2";_tag_dyneither(_tmp73A,sizeof(char),46);}),_tag_dyneither(_tmp739,sizeof(void*),0));});
if(_tmp737->identity == tv->identity){
*_tmp738=*_tmp738  || b;
return tvs;}}}
# 3634
return({struct Cyc_List_List*_tmp73B=_region_malloc(r,sizeof(*_tmp73B));_tmp73B->hd=({struct _tuple28*_tmp73C=_region_malloc(r,sizeof(*_tmp73C));_tmp73C->f1=tv;_tmp73C->f2=b;_tmp73C;});_tmp73B->tl=tvs;_tmp73B;});}
# 3638
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3640
if(tv->identity == - 1)
({struct Cyc_String_pa_PrintArg_struct _tmp73F;_tmp73F.tag=0;_tmp73F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(tv));({void*_tmp73D[1]={& _tmp73F};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp73E="bound tvar id for %s is NULL";_tag_dyneither(_tmp73E,sizeof(char),29);}),_tag_dyneither(_tmp73D,sizeof(void*),1));});});
return({struct Cyc_List_List*_tmp740=_cycalloc(sizeof(*_tmp740));_tmp740->hd=tv;_tmp740->tl=tvs;_tmp740;});}struct _tuple29{void*f1;int f2;};
# 3649
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 3652
void*_tmp741=Cyc_Tcutil_compress(e);void*_tmp742=_tmp741;int _tmp743;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp742)->tag == 1){_LL62F: _tmp743=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp742)->f3;_LL630:
# 3654
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple29*_tmp744=(struct _tuple29*)es2->hd;struct _tuple29*_tmp745=_tmp744;void*_tmp746;int*_tmp747;_LL634: _tmp746=_tmp745->f1;_tmp747=(int*)& _tmp745->f2;_LL635:;{
void*_tmp748=Cyc_Tcutil_compress(_tmp746);void*_tmp749=_tmp748;int _tmp74A;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp749)->tag == 1){_LL637: _tmp74A=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp749)->f3;_LL638:
# 3658
 if(_tmp743 == _tmp74A){
if(b != *_tmp747)*_tmp747=1;
return es;}
# 3662
goto _LL636;}else{_LL639: _LL63A:
 goto _LL636;}_LL636:;};}}
# 3666
return({struct Cyc_List_List*_tmp74B=_region_malloc(r,sizeof(*_tmp74B));_tmp74B->hd=({struct _tuple29*_tmp74C=_region_malloc(r,sizeof(*_tmp74C));_tmp74C->f1=e;_tmp74C->f2=b;_tmp74C;});_tmp74B->tl=es;_tmp74B;});}else{_LL631: _LL632:
 return es;}_LL62E:;}
# 3671
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3674
struct Cyc_List_List*r=0;
for(0;tvs != 0;tvs=tvs->tl){
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3683
if(!present)r=({struct Cyc_List_List*_tmp74D=_region_malloc(rgn,sizeof(*_tmp74D));_tmp74D->hd=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp74D->tl=r;_tmp74D;});}
# 3685
r=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);
return r;}
# 3690
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3694
struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp74E=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp74F=_tmp74E;struct Cyc_Absyn_Tvar*_tmp750;int _tmp751;_LL63C: _tmp750=_tmp74F.f1;_tmp751=_tmp74F.f2;_LL63D:;{
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp750->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3704
if(!present)res=({struct Cyc_List_List*_tmp752=_region_malloc(r,sizeof(*_tmp752));_tmp752->hd=(struct _tuple28*)tvs->hd;_tmp752->tl=res;_tmp752;});};}
# 3706
res=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);
return res;}
# 3710
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){
# 3712
if(width != 0){
unsigned int w=0;
if(!Cyc_Tcutil_is_const_exp(width))
({struct Cyc_String_pa_PrintArg_struct _tmp755;_tmp755.tag=0;_tmp755.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);({void*_tmp753[1]={& _tmp755};Cyc_Tcutil_terr(loc,({const char*_tmp754="bitfield %s does not have constant width";_tag_dyneither(_tmp754,sizeof(char),41);}),_tag_dyneither(_tmp753,sizeof(void*),1));});});else{
# 3717
struct _tuple14 _tmp756=Cyc_Evexp_eval_const_uint_exp(width);struct _tuple14 _tmp757=_tmp756;unsigned int _tmp758;int _tmp759;_LL63F: _tmp758=_tmp757.f1;_tmp759=_tmp757.f2;_LL640:;
if(!_tmp759)
({void*_tmp75A=0;Cyc_Tcutil_terr(loc,({const char*_tmp75B="bitfield width cannot use sizeof or offsetof";_tag_dyneither(_tmp75B,sizeof(char),45);}),_tag_dyneither(_tmp75A,sizeof(void*),0));});
w=_tmp758;}{
# 3722
void*_tmp75C=Cyc_Tcutil_compress(field_typ);void*_tmp75D=_tmp75C;enum Cyc_Absyn_Size_of _tmp75E;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp75D)->tag == 6){_LL642: _tmp75E=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp75D)->f2;_LL643:
# 3725
{enum Cyc_Absyn_Size_of _tmp75F=_tmp75E;switch(_tmp75F){case Cyc_Absyn_Char_sz: _LL647: _LL648:
 if(w > 8)({void*_tmp760=0;Cyc_Tcutil_terr(loc,({const char*_tmp761="bitfield larger than type";_tag_dyneither(_tmp761,sizeof(char),26);}),_tag_dyneither(_tmp760,sizeof(void*),0));});goto _LL646;case Cyc_Absyn_Short_sz: _LL649: _LL64A:
 if(w > 16)({void*_tmp762=0;Cyc_Tcutil_terr(loc,({const char*_tmp763="bitfield larger than type";_tag_dyneither(_tmp763,sizeof(char),26);}),_tag_dyneither(_tmp762,sizeof(void*),0));});goto _LL646;case Cyc_Absyn_Long_sz: _LL64B: _LL64C:
 goto _LL64E;case Cyc_Absyn_Int_sz: _LL64D: _LL64E:
# 3730
 if(w > 32)({void*_tmp764=0;Cyc_Tcutil_terr(loc,({const char*_tmp765="bitfield larger than type";_tag_dyneither(_tmp765,sizeof(char),26);}),_tag_dyneither(_tmp764,sizeof(void*),0));});goto _LL646;default: _LL64F: _LL650:
# 3732
 if(w > 64)({void*_tmp766=0;Cyc_Tcutil_terr(loc,({const char*_tmp767="bitfield larger than type";_tag_dyneither(_tmp767,sizeof(char),26);}),_tag_dyneither(_tmp766,sizeof(void*),0));});goto _LL646;}_LL646:;}
# 3734
goto _LL641;}else{_LL644: _LL645:
# 3736
({struct Cyc_String_pa_PrintArg_struct _tmp76B;_tmp76B.tag=0;_tmp76B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(field_typ));({struct Cyc_String_pa_PrintArg_struct _tmp76A;_tmp76A.tag=0;_tmp76A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);({void*_tmp768[2]={& _tmp76A,& _tmp76B};Cyc_Tcutil_terr(loc,({const char*_tmp769="bitfield %s must have integral type but has type %s";_tag_dyneither(_tmp769,sizeof(char),52);}),_tag_dyneither(_tmp768,sizeof(void*),2));});});});
goto _LL641;}_LL641:;};}}
# 3743
static void Cyc_Tcutil_check_field_atts(unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp76C=(void*)atts->hd;void*_tmp76D=_tmp76C;switch(*((int*)_tmp76D)){case 7: _LL652: _LL653:
 continue;case 6: _LL654: _LL655:
 continue;default: _LL656: _LL657:
({struct Cyc_String_pa_PrintArg_struct _tmp771;_tmp771.tag=0;_tmp771.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);({struct Cyc_String_pa_PrintArg_struct _tmp770;_tmp770.tag=0;_tmp770.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd));({void*_tmp76E[2]={& _tmp770,& _tmp771};Cyc_Tcutil_terr(loc,({const char*_tmp76F="bad attribute %s on member %s";_tag_dyneither(_tmp76F,sizeof(char),30);}),_tag_dyneither(_tmp76E,sizeof(void*),2));});});});}_LL651:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
# 3767
typedef struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_cvtenv_t;
# 3771
int Cyc_Tcutil_extract_const_from_typedef(unsigned int loc,int declared_const,void*t){
void*_tmp772=t;struct Cyc_Absyn_Typedefdecl*_tmp773;void*_tmp774;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp772)->tag == 17){_LL659: _tmp773=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp772)->f3;_tmp774=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp772)->f4;_LL65A:
# 3774
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp773))->tq).real_const  || (_tmp773->tq).print_const){
if(declared_const)({void*_tmp775=0;Cyc_Tcutil_warn(loc,({const char*_tmp776="extra const";_tag_dyneither(_tmp776,sizeof(char),12);}),_tag_dyneither(_tmp775,sizeof(void*),0));});
return 1;}
# 3779
if((unsigned int)_tmp774)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp774);else{
return declared_const;}}else{_LL65B: _LL65C:
 return declared_const;}_LL658:;}
# 3786
static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td != 0){
if(td->defn != 0){
void*_tmp777=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_tmp778=_tmp777;void*_tmp779;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp778)->tag == 5){_LL65E: _tmp779=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp778)->f1).ptr_atts).rgn;_LL65F:
# 3791
{void*_tmp77A=Cyc_Tcutil_compress(_tmp779);void*_tmp77B=_tmp77A;struct Cyc_Absyn_Tvar*_tmp77C;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp77B)->tag == 2){_LL663: _tmp77C=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp77B)->f1;_LL664:
# 3793
 return Cyc_Absyn_tvar_cmp(tvar,_tmp77C)== 0;}else{_LL665: _LL666:
 goto _LL662;}_LL662:;}
# 3796
goto _LL65D;}else{_LL660: _LL661:
 goto _LL65D;}_LL65D:;}}else{
# 3802
return 1;}
return 0;}
# 3806
static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 3809
void*_tmp77D=Cyc_Absyn_compress_kb(tvar->kind);void*_tmp77E=_tmp77D;switch(*((int*)_tmp77E)){case 2: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp77E)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp77E)->f2)->aliasqual == Cyc_Absyn_Top){_LL668: _LL669:
 goto _LL66B;}else{goto _LL66C;}}else{goto _LL66C;}case 0: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp77E)->f1)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp77E)->f1)->aliasqual == Cyc_Absyn_Top){_LL66A: _LL66B:
# 3818
 if(((expected_kind->kind == Cyc_Absyn_BoxKind  || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind) && 
# 3821
Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){
if(expected_kind->aliasqual == Cyc_Absyn_Aliasable)
return& Cyc_Tcutil_rk;else{
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}
# 3827
return& Cyc_Tcutil_trk;}else{goto _LL66C;}}else{goto _LL66C;}default: _LL66C: _LL66D:
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);}_LL667:;}
# 3833
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);struct _tuple30{struct Cyc_Tcutil_CVTEnv f1;struct Cyc_List_List*f2;};
# 3837
static struct _tuple30 Cyc_Tcutil_check_clause(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct _dyneither_ptr clause_name,struct Cyc_Absyn_Exp*clause){
# 3840
struct Cyc_List_List*relns=0;
if(clause != 0){
Cyc_Tcexp_tcExp(te,0,clause);
if(!Cyc_Tcutil_is_integral(clause))
({struct Cyc_String_pa_PrintArg_struct _tmp782;_tmp782.tag=0;_tmp782.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(clause->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp781;_tmp781.tag=0;_tmp781.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name);({void*_tmp77F[2]={& _tmp781,& _tmp782};Cyc_Tcutil_terr(loc,({const char*_tmp780="%s clause has type %s instead of integral type";_tag_dyneither(_tmp780,sizeof(char),47);}),_tag_dyneither(_tmp77F,sizeof(void*),2));});});});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(clause,te,cvtenv);
relns=Cyc_Relations_exp2relns(Cyc_Core_heap_region,clause);
if(!Cyc_Relations_consistent_relations(relns))
({struct Cyc_String_pa_PrintArg_struct _tmp786;_tmp786.tag=0;_tmp786.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(clause));({struct Cyc_String_pa_PrintArg_struct _tmp785;_tmp785.tag=0;_tmp785.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name);({void*_tmp783[2]={& _tmp785,& _tmp786};Cyc_Tcutil_terr(clause->loc,({const char*_tmp784="%s clause '%s' may be unsatisfiable";_tag_dyneither(_tmp784,sizeof(char),36);}),_tag_dyneither(_tmp783,sizeof(void*),2));});});});}
# 3852
return({struct _tuple30 _tmp787;_tmp787.f1=cvtenv;_tmp787.f2=relns;_tmp787;});}struct _tuple31{enum Cyc_Absyn_Format_Type f1;void*f2;};
# 3884 "tcutil.cyc"
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect,int allow_abs_aggr){
# 3892
{void*_tmp788=Cyc_Tcutil_compress(t);void*_tmp789=_tmp788;struct Cyc_List_List*_tmp78A;void*_tmp78B;void*_tmp78C;void*_tmp78D;void*_tmp78E;void*_tmp78F;struct _tuple2*_tmp790;struct Cyc_List_List**_tmp791;struct Cyc_Absyn_Typedefdecl**_tmp792;void**_tmp793;union Cyc_Absyn_AggrInfoU*_tmp794;struct Cyc_List_List**_tmp795;enum Cyc_Absyn_AggrKind _tmp796;struct Cyc_List_List*_tmp797;struct Cyc_List_List*_tmp798;struct Cyc_List_List**_tmp799;void**_tmp79A;struct Cyc_Absyn_Tqual*_tmp79B;void*_tmp79C;struct Cyc_List_List*_tmp79D;int _tmp79E;struct Cyc_Absyn_VarargInfo*_tmp79F;struct Cyc_List_List*_tmp7A0;struct Cyc_List_List*_tmp7A1;struct Cyc_Absyn_Exp*_tmp7A2;struct Cyc_List_List**_tmp7A3;struct Cyc_Absyn_Exp*_tmp7A4;struct Cyc_List_List**_tmp7A5;void*_tmp7A6;struct Cyc_Absyn_Tqual*_tmp7A7;struct Cyc_Absyn_Exp**_tmp7A8;union Cyc_Absyn_Constraint*_tmp7A9;unsigned int _tmp7AA;struct Cyc_Absyn_Exp*_tmp7AB;struct Cyc_Absyn_Exp*_tmp7AC;void*_tmp7AD;void*_tmp7AE;struct Cyc_Absyn_Tqual*_tmp7AF;void*_tmp7B0;union Cyc_Absyn_Constraint*_tmp7B1;union Cyc_Absyn_Constraint*_tmp7B2;union Cyc_Absyn_Constraint*_tmp7B3;union Cyc_Absyn_DatatypeFieldInfoU*_tmp7B4;struct Cyc_List_List*_tmp7B5;union Cyc_Absyn_DatatypeInfoU*_tmp7B6;struct Cyc_List_List**_tmp7B7;struct _tuple2*_tmp7B8;struct Cyc_Absyn_Enumdecl**_tmp7B9;struct Cyc_List_List*_tmp7BA;void*_tmp7BB;void***_tmp7BC;struct Cyc_Absyn_Tvar*_tmp7BD;struct Cyc_Core_Opt**_tmp7BE;void**_tmp7BF;switch(*((int*)_tmp789)){case 0: _LL66F: _LL670:
 goto _LL66E;case 1: _LL671: _tmp7BE=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp789)->f1;_tmp7BF=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp789)->f2;_LL672:
# 3896
 if(*_tmp7BE == 0  || 
Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp7BE))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp7BE))->v,expected_kind))
*_tmp7BE=Cyc_Tcutil_kind_to_opt(expected_kind);
if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){
# 3901
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
*_tmp7BF=(void*)& Cyc_Absyn_UniqueRgn_val;else{
# 3904
*_tmp7BF=(void*)& Cyc_Absyn_HeapRgn_val;}}else{
if(cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar*_tmp7C0=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp7C3=_cycalloc(sizeof(*_tmp7C3));_tmp7C3[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp7C4;_tmp7C4.tag=2;_tmp7C4.f1=0;_tmp7C4.f2=expected_kind;_tmp7C4;});_tmp7C3;}));
*_tmp7BF=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp7C1=_cycalloc(sizeof(*_tmp7C1));_tmp7C1[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp7C2;_tmp7C2.tag=2;_tmp7C2.f1=_tmp7C0;_tmp7C2;});_tmp7C1;});
_tmp7BD=_tmp7C0;goto _LL674;}else{
# 3910
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}
# 3912
goto _LL66E;case 2: _LL673: _tmp7BD=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp789)->f1;_LL674:
# 3914
{void*_tmp7C5=Cyc_Absyn_compress_kb(_tmp7BD->kind);void*_tmp7C6=_tmp7C5;struct Cyc_Core_Opt**_tmp7C7;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7C6)->tag == 1){_LL6AA: _tmp7C7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7C6)->f1;_LL6AB:
# 3916
*_tmp7C7=({struct Cyc_Core_Opt*_tmp7C8=_cycalloc(sizeof(*_tmp7C8));_tmp7C8->v=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp7C9=_cycalloc(sizeof(*_tmp7C9));_tmp7C9[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp7CA;_tmp7CA.tag=2;_tmp7CA.f1=0;_tmp7CA.f2=expected_kind;_tmp7CA;});_tmp7C9;});_tmp7C8;});goto _LL6A9;}else{_LL6AC: _LL6AD:
 goto _LL6A9;}_LL6A9:;}
# 3921
cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp7BD);
# 3924
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp7BD,put_in_effect);
# 3926
{void*_tmp7CB=Cyc_Absyn_compress_kb(_tmp7BD->kind);void*_tmp7CC=_tmp7CB;struct Cyc_Core_Opt**_tmp7CD;struct Cyc_Absyn_Kind*_tmp7CE;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7CC)->tag == 2){_LL6AF: _tmp7CD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7CC)->f1;_tmp7CE=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7CC)->f2;_LL6B0:
# 3928
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmp7CE))
*_tmp7CD=({struct Cyc_Core_Opt*_tmp7CF=_cycalloc(sizeof(*_tmp7CF));_tmp7CF->v=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp7D0=_cycalloc(sizeof(*_tmp7D0));_tmp7D0[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp7D1;_tmp7D1.tag=2;_tmp7D1.f1=0;_tmp7D1.f2=expected_kind;_tmp7D1;});_tmp7D0;});_tmp7CF;});
goto _LL6AE;}else{_LL6B1: _LL6B2:
 goto _LL6AE;}_LL6AE:;}
# 3933
goto _LL66E;case 26: _LL675: _tmp7BB=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp789)->f1)->r;_tmp7BC=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp789)->f2;_LL676: {
# 3939
void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmp7D2=_tmp7BB;struct Cyc_Absyn_Datatypedecl*_tmp7D3;struct Cyc_Absyn_Enumdecl*_tmp7D4;struct Cyc_Absyn_Aggrdecl*_tmp7D5;switch(*((int*)_tmp7D2)){case 0: _LL6B4: _tmp7D5=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp7D2)->f1;_LL6B5:
# 3942
 if(te->in_extern_c_include)
_tmp7D5->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp7D5);goto _LL6B3;case 1: _LL6B6: _tmp7D4=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp7D2)->f1;_LL6B7:
# 3946
 if(te->in_extern_c_include)
_tmp7D4->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp7D4);goto _LL6B3;default: _LL6B8: _tmp7D3=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp7D2)->f1;_LL6B9:
# 3950
 Cyc_Tc_tcDatatypedecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp7D3);goto _LL6B3;}_LL6B3:;}
# 3952
*_tmp7BC=({void**_tmp7D6=_cycalloc(sizeof(*_tmp7D6));_tmp7D6[0]=new_t;_tmp7D6;});
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect,allow_abs_aggr);}case 14: _LL677: _tmp7BA=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp789)->f1;_LL678: {
# 3958
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct _RegionHandle _tmp7D7=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp7D7;_push_region(uprev_rgn);{
struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;
for(0;_tmp7BA != 0;_tmp7BA=_tmp7BA->tl){
struct Cyc_Absyn_Enumfield*_tmp7D8=(struct Cyc_Absyn_Enumfield*)_tmp7BA->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp7D8->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp7DB;_tmp7DB.tag=0;_tmp7DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp7D8->name).f2);({void*_tmp7D9[1]={& _tmp7DB};Cyc_Tcutil_terr(_tmp7D8->loc,({const char*_tmp7DA="duplicate enum field name %s";_tag_dyneither(_tmp7DA,sizeof(char),29);}),_tag_dyneither(_tmp7D9,sizeof(void*),1));});});else{
# 3967
prev_fields=({struct Cyc_List_List*_tmp7DC=_region_malloc(uprev_rgn,sizeof(*_tmp7DC));_tmp7DC->hd=(*_tmp7D8->name).f2;_tmp7DC->tl=prev_fields;_tmp7DC;});}
# 3969
if(_tmp7D8->tag == 0)
_tmp7D8->tag=Cyc_Absyn_uint_exp(tag_count,_tmp7D8->loc);else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp7D8->tag)))
({struct Cyc_String_pa_PrintArg_struct _tmp7DF;_tmp7DF.tag=0;_tmp7DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp7D8->name).f2);({void*_tmp7DD[1]={& _tmp7DF};Cyc_Tcutil_terr(loc,({const char*_tmp7DE="enum field %s: expression is not constant";_tag_dyneither(_tmp7DE,sizeof(char),42);}),_tag_dyneither(_tmp7DD,sizeof(void*),1));});});}{
# 3974
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp7D8->tag))).f1;
tag_count=t1 + 1;
{union Cyc_Absyn_Nmspace _tmp7E0=(*_tmp7D8->name).f1;union Cyc_Absyn_Nmspace _tmp7E1=_tmp7E0;if((_tmp7E1.Rel_n).tag == 1){_LL6BB: _LL6BC:
# 3978
(*_tmp7D8->name).f1=Cyc_Absyn_Abs_n(te->ns,0);goto _LL6BA;}else{_LL6BD: _LL6BE:
 goto _LL6BA;}_LL6BA:;}
# 3981
ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple29*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp7D8->name).f2,({struct _tuple29*_tmp7E2=_cycalloc(sizeof(*_tmp7E2));_tmp7E2->f1=(void*)({struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp7E3=_cycalloc(sizeof(*_tmp7E3));_tmp7E3[0]=({struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct _tmp7E4;_tmp7E4.tag=4;_tmp7E4.f1=t;_tmp7E4.f2=_tmp7D8;_tmp7E4;});_tmp7E3;});_tmp7E2->f2=1;_tmp7E2;}));};}}
# 3985
_npop_handler(0);goto _LL66E;
# 3959
;_pop_region(uprev_rgn);}case 13: _LL679: _tmp7B8=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp789)->f1;_tmp7B9=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp789)->f2;_LL67A:
# 3987
 if(*_tmp7B9 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp7B9))->fields == 0){
struct _handler_cons _tmp7E5;_push_handler(& _tmp7E5);{int _tmp7E7=0;if(setjmp(_tmp7E5.handler))_tmp7E7=1;if(!_tmp7E7){
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp7B8);
*_tmp7B9=*ed;}
# 3989
;_pop_handler();}else{void*_tmp7E6=(void*)_exn_thrown;void*_tmp7E8=_tmp7E6;void*_tmp7E9;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp7E8)->tag == Cyc_Dict_Absent){_LL6C0: _LL6C1: {
# 3993
struct Cyc_Tcenv_Genv*_tmp7EA=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp7EB=({struct Cyc_Absyn_Enumdecl*_tmp7EC=_cycalloc(sizeof(*_tmp7EC));_tmp7EC->sc=Cyc_Absyn_Extern;_tmp7EC->name=_tmp7B8;_tmp7EC->fields=0;_tmp7EC;});
Cyc_Tc_tcEnumdecl(te,_tmp7EA,loc,_tmp7EB);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp7B8);
*_tmp7B9=*ed;
goto _LL6BF;};}}else{_LL6C2: _tmp7E9=_tmp7E8;_LL6C3:(int)_rethrow(_tmp7E9);}_LL6BF:;}};}{
# 4002
struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp7B9);
# 4004
*_tmp7B8=(ed->name)[0];
goto _LL66E;};case 3: _LL67B: _tmp7B6=(union Cyc_Absyn_DatatypeInfoU*)&(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp789)->f1).datatype_info;_tmp7B7=(struct Cyc_List_List**)&(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp789)->f1).targs;_LL67C: {
# 4007
struct Cyc_List_List*_tmp7ED=*_tmp7B7;
{union Cyc_Absyn_DatatypeInfoU _tmp7EE=*_tmp7B6;union Cyc_Absyn_DatatypeInfoU _tmp7EF=_tmp7EE;struct Cyc_Absyn_Datatypedecl*_tmp7F0;struct _tuple2*_tmp7F1;int _tmp7F2;if((_tmp7EF.UnknownDatatype).tag == 1){_LL6C5: _tmp7F1=((_tmp7EF.UnknownDatatype).val).name;_tmp7F2=((_tmp7EF.UnknownDatatype).val).is_extensible;_LL6C6: {
# 4010
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmp7F3;_push_handler(& _tmp7F3);{int _tmp7F5=0;if(setjmp(_tmp7F3.handler))_tmp7F5=1;if(!_tmp7F5){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp7F1);;_pop_handler();}else{void*_tmp7F4=(void*)_exn_thrown;void*_tmp7F6=_tmp7F4;void*_tmp7F7;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp7F6)->tag == Cyc_Dict_Absent){_LL6CA: _LL6CB: {
# 4014
struct Cyc_Tcenv_Genv*_tmp7F8=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Datatypedecl*_tmp7F9=({struct Cyc_Absyn_Datatypedecl*_tmp7FD=_cycalloc(sizeof(*_tmp7FD));_tmp7FD->sc=Cyc_Absyn_Extern;_tmp7FD->name=_tmp7F1;_tmp7FD->tvs=0;_tmp7FD->fields=0;_tmp7FD->is_extensible=_tmp7F2;_tmp7FD;});
Cyc_Tc_tcDatatypedecl(te,_tmp7F8,loc,_tmp7F9);
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp7F1);
# 4019
if(_tmp7ED != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp7FC;_tmp7FC.tag=0;_tmp7FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7F1));({void*_tmp7FA[1]={& _tmp7FC};Cyc_Tcutil_terr(loc,({const char*_tmp7FB="declare parameterized datatype %s before using";_tag_dyneither(_tmp7FB,sizeof(char),47);}),_tag_dyneither(_tmp7FA,sizeof(void*),1));});});
return cvtenv;}
# 4024
goto _LL6C9;}}else{_LL6CC: _tmp7F7=_tmp7F6;_LL6CD:(int)_rethrow(_tmp7F7);}_LL6C9:;}};}
# 4030
if(_tmp7F2  && !(*tudp)->is_extensible)
({struct Cyc_String_pa_PrintArg_struct _tmp800;_tmp800.tag=0;_tmp800.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7F1));({void*_tmp7FE[1]={& _tmp800};Cyc_Tcutil_terr(loc,({const char*_tmp7FF="datatype %s was not declared @extensible";_tag_dyneither(_tmp7FF,sizeof(char),41);}),_tag_dyneither(_tmp7FE,sizeof(void*),1));});});
*_tmp7B6=Cyc_Absyn_KnownDatatype(tudp);
_tmp7F0=*tudp;goto _LL6C8;}}else{_LL6C7: _tmp7F0=*(_tmp7EF.KnownDatatype).val;_LL6C8: {
# 4037
struct Cyc_List_List*tvs=_tmp7F0->tvs;
for(0;_tmp7ED != 0  && tvs != 0;(_tmp7ED=_tmp7ED->tl,tvs=tvs->tl)){
void*t=(void*)_tmp7ED->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 4043
{struct _tuple0 _tmp801=({struct _tuple0 _tmp804;_tmp804.f1=Cyc_Absyn_compress_kb(tv->kind);_tmp804.f2=t;_tmp804;});struct _tuple0 _tmp802=_tmp801;struct Cyc_Absyn_Tvar*_tmp803;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp802.f1)->tag == 1){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp802.f2)->tag == 2){_LL6CF: _tmp803=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp802.f2)->f1;_LL6D0:
# 4045
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp803);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp803,1);
continue;}else{goto _LL6D1;}}else{_LL6D1: _LL6D2:
 goto _LL6CE;}_LL6CE:;}{
# 4050
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 4054
if(_tmp7ED != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp807;_tmp807.tag=0;_tmp807.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7F0->name));({void*_tmp805[1]={& _tmp807};Cyc_Tcutil_terr(loc,({const char*_tmp806="too many type arguments for datatype %s";_tag_dyneither(_tmp806,sizeof(char),40);}),_tag_dyneither(_tmp805,sizeof(void*),1));});});
if(tvs != 0){
# 4059
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp808=_cycalloc(sizeof(*_tmp808));_tmp808->hd=e;_tmp808->tl=hidden_ts;_tmp808;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1,allow_abs_aggr);}
# 4066
*_tmp7B7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp7B7,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4068
goto _LL6C4;}}_LL6C4:;}
# 4070
goto _LL66E;}case 4: _LL67D: _tmp7B4=(union Cyc_Absyn_DatatypeFieldInfoU*)&(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp789)->f1).field_info;_tmp7B5=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp789)->f1).targs;_LL67E:
# 4073
{union Cyc_Absyn_DatatypeFieldInfoU _tmp809=*_tmp7B4;union Cyc_Absyn_DatatypeFieldInfoU _tmp80A=_tmp809;struct Cyc_Absyn_Datatypedecl*_tmp80B;struct Cyc_Absyn_Datatypefield*_tmp80C;struct _tuple2*_tmp80D;struct _tuple2*_tmp80E;int _tmp80F;if((_tmp80A.UnknownDatatypefield).tag == 1){_LL6D4: _tmp80D=((_tmp80A.UnknownDatatypefield).val).datatype_name;_tmp80E=((_tmp80A.UnknownDatatypefield).val).field_name;_tmp80F=((_tmp80A.UnknownDatatypefield).val).is_extensible;_LL6D5: {
# 4075
struct Cyc_Absyn_Datatypedecl*tud;
struct Cyc_Absyn_Datatypefield*tuf;
{struct _handler_cons _tmp810;_push_handler(& _tmp810);{int _tmp812=0;if(setjmp(_tmp810.handler))_tmp812=1;if(!_tmp812){*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp80D);;_pop_handler();}else{void*_tmp811=(void*)_exn_thrown;void*_tmp813=_tmp811;void*_tmp814;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp813)->tag == Cyc_Dict_Absent){_LL6D9: _LL6DA:
# 4079
({struct Cyc_String_pa_PrintArg_struct _tmp817;_tmp817.tag=0;_tmp817.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp80D));({void*_tmp815[1]={& _tmp817};Cyc_Tcutil_terr(loc,({const char*_tmp816="unbound datatype %s";_tag_dyneither(_tmp816,sizeof(char),20);}),_tag_dyneither(_tmp815,sizeof(void*),1));});});
return cvtenv;}else{_LL6DB: _tmp814=_tmp813;_LL6DC:(int)_rethrow(_tmp814);}_LL6D8:;}};}
# 4082
{struct _handler_cons _tmp818;_push_handler(& _tmp818);{int _tmp81A=0;if(setjmp(_tmp818.handler))_tmp81A=1;if(!_tmp81A){
{struct _RegionHandle _tmp81B=_new_region("r");struct _RegionHandle*r=& _tmp81B;_push_region(r);
{void*_tmp81C=Cyc_Tcenv_lookup_ordinary(r,te,loc,_tmp80E,0);void*_tmp81D=_tmp81C;struct Cyc_Absyn_Datatypedecl*_tmp81E;struct Cyc_Absyn_Datatypefield*_tmp81F;if(((struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp81D)->tag == 2){_LL6DE: _tmp81E=((struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp81D)->f1;_tmp81F=((struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp81D)->f2;_LL6DF:
# 4086
 tuf=_tmp81F;
tud=_tmp81E;
if(_tmp80F  && !tud->is_extensible)
({struct Cyc_String_pa_PrintArg_struct _tmp822;_tmp822.tag=0;_tmp822.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp80D));({void*_tmp820[1]={& _tmp822};Cyc_Tcutil_terr(loc,({const char*_tmp821="datatype %s was not declared @extensible";_tag_dyneither(_tmp821,sizeof(char),41);}),_tag_dyneither(_tmp820,sizeof(void*),1));});});
goto _LL6DD;}else{_LL6E0: _LL6E1:
({struct Cyc_String_pa_PrintArg_struct _tmp826;_tmp826.tag=0;_tmp826.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp80D));({struct Cyc_String_pa_PrintArg_struct _tmp825;_tmp825.tag=0;_tmp825.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp80E));({void*_tmp823[2]={& _tmp825,& _tmp826};Cyc_Tcutil_terr(loc,({const char*_tmp824="unbound field %s in type datatype %s";_tag_dyneither(_tmp824,sizeof(char),37);}),_tag_dyneither(_tmp823,sizeof(void*),2));});});});{
struct Cyc_Tcutil_CVTEnv _tmp827=cvtenv;_npop_handler(1);return _tmp827;};}_LL6DD:;}
# 4084
;_pop_region(r);}
# 4083
;_pop_handler();}else{void*_tmp819=(void*)_exn_thrown;void*_tmp828=_tmp819;void*_tmp829;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp828)->tag == Cyc_Dict_Absent){_LL6E3: _LL6E4:
# 4099
({struct Cyc_String_pa_PrintArg_struct _tmp82D;_tmp82D.tag=0;_tmp82D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp80D));({struct Cyc_String_pa_PrintArg_struct _tmp82C;_tmp82C.tag=0;_tmp82C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp80E));({void*_tmp82A[2]={& _tmp82C,& _tmp82D};Cyc_Tcutil_terr(loc,({const char*_tmp82B="unbound field %s in type datatype %s";_tag_dyneither(_tmp82B,sizeof(char),37);}),_tag_dyneither(_tmp82A,sizeof(void*),2));});});});
return cvtenv;}else{_LL6E5: _tmp829=_tmp828;_LL6E6:(int)_rethrow(_tmp829);}_LL6E2:;}};}
# 4103
*_tmp7B4=Cyc_Absyn_KnownDatatypefield(tud,tuf);
_tmp80B=tud;_tmp80C=tuf;goto _LL6D7;}}else{_LL6D6: _tmp80B=((_tmp80A.KnownDatatypefield).val).f1;_tmp80C=((_tmp80A.KnownDatatypefield).val).f2;_LL6D7: {
# 4107
struct Cyc_List_List*tvs=_tmp80B->tvs;
for(0;_tmp7B5 != 0  && tvs != 0;(_tmp7B5=_tmp7B5->tl,tvs=tvs->tl)){
void*t=(void*)_tmp7B5->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 4113
{struct _tuple0 _tmp82E=({struct _tuple0 _tmp831;_tmp831.f1=Cyc_Absyn_compress_kb(tv->kind);_tmp831.f2=t;_tmp831;});struct _tuple0 _tmp82F=_tmp82E;struct Cyc_Absyn_Tvar*_tmp830;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp82F.f1)->tag == 1){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp82F.f2)->tag == 2){_LL6E8: _tmp830=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp82F.f2)->f1;_LL6E9:
# 4115
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp830);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp830,1);
continue;}else{goto _LL6EA;}}else{_LL6EA: _LL6EB:
 goto _LL6E7;}_LL6E7:;}{
# 4120
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 4124
if(_tmp7B5 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp835;_tmp835.tag=0;_tmp835.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp80C->name));({struct Cyc_String_pa_PrintArg_struct _tmp834;_tmp834.tag=0;_tmp834.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp80B->name));({void*_tmp832[2]={& _tmp834,& _tmp835};Cyc_Tcutil_terr(loc,({const char*_tmp833="too many type arguments for datatype %s.%s";_tag_dyneither(_tmp833,sizeof(char),43);}),_tag_dyneither(_tmp832,sizeof(void*),2));});});});
if(tvs != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp839;_tmp839.tag=0;_tmp839.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp80C->name));({struct Cyc_String_pa_PrintArg_struct _tmp838;_tmp838.tag=0;_tmp838.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp80B->name));({void*_tmp836[2]={& _tmp838,& _tmp839};Cyc_Tcutil_terr(loc,({const char*_tmp837="too few type arguments for datatype %s.%s";_tag_dyneither(_tmp837,sizeof(char),42);}),_tag_dyneither(_tmp836,sizeof(void*),2));});});});
goto _LL6D3;}}_LL6D3:;}
# 4132
goto _LL66E;case 5: _LL67F: _tmp7AE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp789)->f1).elt_typ;_tmp7AF=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp789)->f1).elt_tq;_tmp7B0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp789)->f1).ptr_atts).rgn;_tmp7B1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp789)->f1).ptr_atts).nullable;_tmp7B2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp789)->f1).ptr_atts).bounds;_tmp7B3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp789)->f1).ptr_atts).zero_term;_LL680: {
# 4135
int is_zero_terminated;
# 4137
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp7AE,1,1);
_tmp7AF->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp7AF->print_const,_tmp7AE);{
struct Cyc_Absyn_Kind*k;
{enum Cyc_Absyn_AliasQual _tmp83A=expected_kind->aliasqual;enum Cyc_Absyn_AliasQual _tmp83B=_tmp83A;switch(_tmp83B){case Cyc_Absyn_Aliasable: _LL6ED: _LL6EE:
 k=& Cyc_Tcutil_rk;goto _LL6EC;case Cyc_Absyn_Unique: _LL6EF: _LL6F0:
 k=& Cyc_Tcutil_urk;goto _LL6EC;default: _LL6F1: _LL6F2:
 k=& Cyc_Tcutil_trk;goto _LL6EC;}_LL6EC:;}
# 4145
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmp7B0,1,1);
{union Cyc_Absyn_Constraint*_tmp83C=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp7B3);union Cyc_Absyn_Constraint*_tmp83D=_tmp83C;int _tmp83E;switch((((union Cyc_Absyn_Constraint*)_tmp83D)->Eq_constr).tag){case 3: _LL6F4: _LL6F5:
# 4150
{void*_tmp83F=Cyc_Tcutil_compress(_tmp7AE);void*_tmp840=_tmp83F;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp840)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp840)->f2 == Cyc_Absyn_Char_sz){_LL6FB: _LL6FC:
# 4152
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp7B3,Cyc_Absyn_true_conref);
is_zero_terminated=1;
goto _LL6FA;}else{goto _LL6FD;}}else{_LL6FD: _LL6FE:
# 4156
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp7B3,Cyc_Absyn_false_conref);
is_zero_terminated=0;
goto _LL6FA;}_LL6FA:;}
# 4160
goto _LL6F3;case 1: _LL6F6: _tmp83E=(int)(_tmp83D->Eq_constr).val;if(_tmp83E){_LL6F7:
# 4163
 if(!Cyc_Tcutil_admits_zero(_tmp7AE))
({struct Cyc_String_pa_PrintArg_struct _tmp843;_tmp843.tag=0;_tmp843.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp7AE));({void*_tmp841[1]={& _tmp843};Cyc_Tcutil_terr(loc,({const char*_tmp842="cannot have a pointer to zero-terminated %s elements";_tag_dyneither(_tmp842,sizeof(char),53);}),_tag_dyneither(_tmp841,sizeof(void*),1));});});
is_zero_terminated=1;
goto _LL6F3;}else{goto _LL6F8;}default: _LL6F8: _LL6F9:
# 4169
 is_zero_terminated=0;
goto _LL6F3;}_LL6F3:;}
# 4173
{void*_tmp844=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp7B2);void*_tmp845=_tmp844;struct Cyc_Absyn_Exp*_tmp846;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp845)->tag == 0){_LL700: _LL701:
 goto _LL6FF;}else{_LL702: _tmp846=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp845)->f1;_LL703: {
# 4176
struct _RegionHandle _tmp847=_new_region("temp");struct _RegionHandle*temp=& _tmp847;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp848=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp848,0,_tmp846);}
# 4180
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp846,te,cvtenv);
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp846))
({void*_tmp849=0;Cyc_Tcutil_terr(loc,({const char*_tmp84A="pointer bounds expression is not an unsigned int";_tag_dyneither(_tmp84A,sizeof(char),49);}),_tag_dyneither(_tmp849,sizeof(void*),0));});{
struct _tuple14 _tmp84B=Cyc_Evexp_eval_const_uint_exp(_tmp846);struct _tuple14 _tmp84C=_tmp84B;unsigned int _tmp84D;int _tmp84E;_LL705: _tmp84D=_tmp84C.f1;_tmp84E=_tmp84C.f2;_LL706:;
if(is_zero_terminated  && (!_tmp84E  || _tmp84D < 1))
({void*_tmp84F=0;Cyc_Tcutil_terr(loc,({const char*_tmp850="zero-terminated pointer cannot point to empty sequence";_tag_dyneither(_tmp850,sizeof(char),55);}),_tag_dyneither(_tmp84F,sizeof(void*),0));});
_npop_handler(0);goto _LL6FF;};
# 4176
;_pop_region(temp);}}_LL6FF:;}
# 4188
goto _LL66E;};}case 19: _LL681: _tmp7AD=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp789)->f1;_LL682:
# 4190
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp7AD,1,1);goto _LL66E;case 28: _LL683: _LL684:
 goto _LL66E;case 18: _LL685: _tmp7AC=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp789)->f1;_LL686: {
# 4196
struct _RegionHandle _tmp851=_new_region("temp");struct _RegionHandle*temp=& _tmp851;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp852=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp852,0,_tmp7AC);}
# 4200
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp7AC))
({void*_tmp853=0;Cyc_Tcutil_terr(loc,({const char*_tmp854="valueof_t requires an int expression";_tag_dyneither(_tmp854,sizeof(char),37);}),_tag_dyneither(_tmp853,sizeof(void*),0));});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7AC,te,cvtenv);
_npop_handler(0);goto _LL66E;
# 4196
;_pop_region(temp);}case 27: _LL687: _tmp7AB=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp789)->f1;_LL688: {
# 4208
struct _RegionHandle _tmp855=_new_region("temp");struct _RegionHandle*temp=& _tmp855;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp856=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp856,0,_tmp7AB);}
# 4212
_npop_handler(0);goto _LL66E;
# 4208
;_pop_region(temp);}case 6: _LL689: _LL68A:
# 4213
 goto _LL68C;case 7: _LL68B: _LL68C:
 goto _LL66E;case 8: _LL68D: _tmp7A6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp789)->f1).elt_type;_tmp7A7=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp789)->f1).tq;_tmp7A8=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp789)->f1).num_elts;_tmp7A9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp789)->f1).zero_term;_tmp7AA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp789)->f1).zt_loc;_LL68E: {
# 4218
struct Cyc_Absyn_Exp*_tmp857=*_tmp7A8;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp7A6,1,allow_abs_aggr);
_tmp7A7->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp7A7->print_const,_tmp7A6);{
int is_zero_terminated;
{union Cyc_Absyn_Constraint*_tmp858=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp7A9);union Cyc_Absyn_Constraint*_tmp859=_tmp858;int _tmp85A;switch((((union Cyc_Absyn_Constraint*)_tmp859)->Eq_constr).tag){case 3: _LL708: _LL709:
# 4225
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp7A9,Cyc_Absyn_false_conref);
is_zero_terminated=0;
# 4240 "tcutil.cyc"
goto _LL707;case 1: _LL70A: _tmp85A=(int)(_tmp859->Eq_constr).val;if(_tmp85A){_LL70B:
# 4243
 if(!Cyc_Tcutil_admits_zero(_tmp7A6))
({struct Cyc_String_pa_PrintArg_struct _tmp85D;_tmp85D.tag=0;_tmp85D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp7A6));({void*_tmp85B[1]={& _tmp85D};Cyc_Tcutil_terr(loc,({const char*_tmp85C="cannot have a zero-terminated array of %s elements";_tag_dyneither(_tmp85C,sizeof(char),51);}),_tag_dyneither(_tmp85B,sizeof(void*),1));});});
is_zero_terminated=1;
goto _LL707;}else{goto _LL70C;}default: _LL70C: _LL70D:
# 4249
 is_zero_terminated=0;
goto _LL707;}_LL707:;}
# 4254
if(_tmp857 == 0){
# 4256
if(is_zero_terminated)
# 4258
*_tmp7A8=(_tmp857=Cyc_Absyn_uint_exp(1,0));else{
# 4261
({void*_tmp85E=0;Cyc_Tcutil_warn(loc,({const char*_tmp85F="array bound defaults to 1 here";_tag_dyneither(_tmp85F,sizeof(char),31);}),_tag_dyneither(_tmp85E,sizeof(void*),0));});
*_tmp7A8=(_tmp857=Cyc_Absyn_uint_exp(1,0));}}{
# 4265
struct _RegionHandle _tmp860=_new_region("temp");struct _RegionHandle*temp=& _tmp860;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp861=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp861,0,_tmp857);}
# 4269
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp857))
({void*_tmp862=0;Cyc_Tcutil_terr(loc,({const char*_tmp863="array bounds expression is not an unsigned int";_tag_dyneither(_tmp863,sizeof(char),47);}),_tag_dyneither(_tmp862,sizeof(void*),0));});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp857,te,cvtenv);{
# 4276
struct _tuple14 _tmp864=Cyc_Evexp_eval_const_uint_exp(_tmp857);struct _tuple14 _tmp865=_tmp864;unsigned int _tmp866;int _tmp867;_LL70F: _tmp866=_tmp865.f1;_tmp867=_tmp865.f2;_LL710:;
# 4278
if((is_zero_terminated  && _tmp867) && _tmp866 < 1)
({void*_tmp868=0;Cyc_Tcutil_warn(loc,({const char*_tmp869="zero terminated array cannot have zero elements";_tag_dyneither(_tmp869,sizeof(char),48);}),_tag_dyneither(_tmp868,sizeof(void*),0));});
# 4281
if((_tmp867  && _tmp866 < 1) && Cyc_Cyclone_tovc_r){
({void*_tmp86A=0;Cyc_Tcutil_warn(loc,({const char*_tmp86B="arrays with 0 elements are not supported except with gcc -- changing to 1.";_tag_dyneither(_tmp86B,sizeof(char),75);}),_tag_dyneither(_tmp86A,sizeof(void*),0));});
*_tmp7A8=Cyc_Absyn_uint_exp(1,0);}
# 4285
_npop_handler(0);goto _LL66E;};
# 4265
;_pop_region(temp);};};}case 9: _LL68F: _tmp799=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp789)->f1).tvars;_tmp79A=(void**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp789)->f1).effect;_tmp79B=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp789)->f1).ret_tqual;_tmp79C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp789)->f1).ret_typ;_tmp79D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp789)->f1).args;_tmp79E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp789)->f1).c_varargs;_tmp79F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp789)->f1).cyc_varargs;_tmp7A0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp789)->f1).rgn_po;_tmp7A1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp789)->f1).attributes;_tmp7A2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp789)->f1).requires_clause;_tmp7A3=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp789)->f1).requires_relns;_tmp7A4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp789)->f1).ensures_clause;_tmp7A5=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp789)->f1).ensures_relns;_LL690: {
# 4292
int num_convs=0;
int seen_cdecl=0;
int seen_stdcall=0;
int seen_fastcall=0;
int seen_format=0;
enum Cyc_Absyn_Format_Type ft=Cyc_Absyn_Printf_ft;
int fmt_desc_arg=-1;
int fmt_arg_start=-1;
for(0;_tmp7A1 != 0;_tmp7A1=_tmp7A1->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp7A1->hd))
({struct Cyc_String_pa_PrintArg_struct _tmp86E;_tmp86E.tag=0;_tmp86E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp7A1->hd));({void*_tmp86C[1]={& _tmp86E};Cyc_Tcutil_terr(loc,({const char*_tmp86D="bad function type attribute %s";_tag_dyneither(_tmp86D,sizeof(char),31);}),_tag_dyneither(_tmp86C,sizeof(void*),1));});});{
void*_tmp86F=(void*)_tmp7A1->hd;void*_tmp870=_tmp86F;enum Cyc_Absyn_Format_Type _tmp871;int _tmp872;int _tmp873;switch(*((int*)_tmp870)){case 1: _LL712: _LL713:
# 4305
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL711;case 2: _LL714: _LL715:
# 4307
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL711;case 3: _LL716: _LL717:
# 4309
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL711;case 19: _LL718: _tmp871=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp870)->f1;_tmp872=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp870)->f2;_tmp873=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp870)->f3;_LL719:
# 4311
 if(!seen_format){
seen_format=1;
ft=_tmp871;
fmt_desc_arg=_tmp872;
fmt_arg_start=_tmp873;}else{
# 4317
({void*_tmp874=0;Cyc_Tcutil_terr(loc,({const char*_tmp875="function can't have multiple format attributes";_tag_dyneither(_tmp875,sizeof(char),47);}),_tag_dyneither(_tmp874,sizeof(void*),0));});}
goto _LL711;default: _LL71A: _LL71B:
 goto _LL711;}_LL711:;};}
# 4322
if(num_convs > 1)
({void*_tmp876=0;Cyc_Tcutil_terr(loc,({const char*_tmp877="function can't have multiple calling conventions";_tag_dyneither(_tmp877,sizeof(char),49);}),_tag_dyneither(_tmp876,sizeof(void*),0));});
# 4327
Cyc_Tcutil_check_unique_tvars(loc,*_tmp799);
{struct Cyc_List_List*b=*_tmp799;for(0;b != 0;b=b->tl){
((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{
void*_tmp878=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp879=_tmp878;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp879)->tag == 0){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp879)->f1)->kind == Cyc_Absyn_MemKind){_LL71D: _LL71E:
# 4333
({struct Cyc_String_pa_PrintArg_struct _tmp87C;_tmp87C.tag=0;_tmp87C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name);({void*_tmp87A[1]={& _tmp87C};Cyc_Tcutil_terr(loc,({const char*_tmp87B="function attempts to abstract Mem type variable %s";_tag_dyneither(_tmp87B,sizeof(char),51);}),_tag_dyneither(_tmp87A,sizeof(void*),1));});});
goto _LL71C;}else{goto _LL71F;}}else{_LL71F: _LL720:
 goto _LL71C;}_LL71C:;};}}{
# 4341
struct _RegionHandle _tmp87D=_new_region("newr");struct _RegionHandle*newr=& _tmp87D;_push_region(newr);{
struct Cyc_Tcutil_CVTEnv _tmp87E=({struct Cyc_Tcutil_CVTEnv _tmp911;_tmp911.r=newr;_tmp911.kind_env=cvtenv.kind_env;_tmp911.free_vars=0;_tmp911.free_evars=0;_tmp911.generalize_evars=cvtenv.generalize_evars;_tmp911.fn_result=1;_tmp911;});
# 4347
_tmp87E=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp87E,& Cyc_Tcutil_tmk,_tmp79C,1,1);
_tmp79B->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp79B->print_const,_tmp79C);
_tmp87E.fn_result=0;
{struct Cyc_List_List*a=_tmp79D;for(0;a != 0;a=a->tl){
struct _tuple10*_tmp87F=(struct _tuple10*)a->hd;
void*_tmp880=(*_tmp87F).f3;
_tmp87E=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp87E,& Cyc_Tcutil_tmk,_tmp880,1,1);{
int _tmp881=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp87F).f2).print_const,_tmp880);
((*_tmp87F).f2).real_const=_tmp881;
# 4358
if(Cyc_Tcutil_is_array(_tmp880)){
# 4361
void*_tmp882=Cyc_Absyn_new_evar(0,0);
_tmp87E=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp87E,& Cyc_Tcutil_rk,_tmp882,1,1);
(*_tmp87F).f3=Cyc_Tcutil_promote_array(_tmp880,_tmp882,0);}};}}
# 4368
if(_tmp79F != 0){
if(_tmp79E)({void*_tmp883=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp884="both c_vararg and cyc_vararg";_tag_dyneither(_tmp884,sizeof(char),29);}),_tag_dyneither(_tmp883,sizeof(void*),0));});{
struct Cyc_Absyn_VarargInfo _tmp885=*_tmp79F;struct Cyc_Absyn_VarargInfo _tmp886=_tmp885;void*_tmp887;int _tmp888;_LL722: _tmp887=_tmp886.type;_tmp888=_tmp886.inject;_LL723:;
_tmp87E=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp87E,& Cyc_Tcutil_tmk,_tmp887,1,1);
(_tmp79F->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmp79F->tq).print_const,_tmp887);
# 4374
if(_tmp888){
void*_tmp889=Cyc_Tcutil_compress(_tmp887);void*_tmp88A=_tmp889;void*_tmp88B;union Cyc_Absyn_Constraint*_tmp88C;union Cyc_Absyn_Constraint*_tmp88D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp88A)->tag == 5){_LL725: _tmp88B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp88A)->f1).elt_typ;_tmp88C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp88A)->f1).ptr_atts).bounds;_tmp88D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp88A)->f1).ptr_atts).zero_term;_LL726:
# 4377
{void*_tmp88E=Cyc_Tcutil_compress(_tmp88B);void*_tmp88F=_tmp88E;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp88F)->tag == 3){_LL72A: _LL72B:
# 4379
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp88D))
({void*_tmp890=0;Cyc_Tcutil_terr(loc,({const char*_tmp891="can't inject into a zeroterm pointer";_tag_dyneither(_tmp891,sizeof(char),37);}),_tag_dyneither(_tmp890,sizeof(void*),0));});
{void*_tmp892=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp88C);void*_tmp893=_tmp892;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp893)->tag == 0){_LL72F: _LL730:
# 4383
({void*_tmp894=0;Cyc_Tcutil_terr(loc,({const char*_tmp895="can't inject into a fat pointer to datatype";_tag_dyneither(_tmp895,sizeof(char),44);}),_tag_dyneither(_tmp894,sizeof(void*),0));});
goto _LL72E;}else{_LL731: _LL732:
 goto _LL72E;}_LL72E:;}
# 4387
goto _LL729;}else{_LL72C: _LL72D:
({void*_tmp896=0;Cyc_Tcutil_terr(loc,({const char*_tmp897="can't inject a non-datatype type";_tag_dyneither(_tmp897,sizeof(char),33);}),_tag_dyneither(_tmp896,sizeof(void*),0));});goto _LL729;}_LL729:;}
# 4390
goto _LL724;}else{_LL727: _LL728:
({void*_tmp898=0;Cyc_Tcutil_terr(loc,({const char*_tmp899="expecting a datatype pointer type";_tag_dyneither(_tmp899,sizeof(char),34);}),_tag_dyneither(_tmp898,sizeof(void*),0));});goto _LL724;}_LL724:;}};}
# 4396
if(seen_format){
int _tmp89A=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp79D);
if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmp89A) || fmt_arg_start < 0) || 
# 4400
(_tmp79F == 0  && !_tmp79E) && fmt_arg_start != 0) || 
(_tmp79F != 0  || _tmp79E) && fmt_arg_start != _tmp89A + 1)
# 4403
({void*_tmp89B=0;Cyc_Tcutil_terr(loc,({const char*_tmp89C="bad format descriptor";_tag_dyneither(_tmp89C,sizeof(char),22);}),_tag_dyneither(_tmp89B,sizeof(void*),0));});else{
# 4406
struct _tuple10 _tmp89D=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp79D,fmt_desc_arg - 1);struct _tuple10 _tmp89E=_tmp89D;void*_tmp89F;_LL734: _tmp89F=_tmp89E.f3;_LL735:;
# 4408
{void*_tmp8A0=Cyc_Tcutil_compress(_tmp89F);void*_tmp8A1=_tmp8A0;void*_tmp8A2;union Cyc_Absyn_Constraint*_tmp8A3;union Cyc_Absyn_Constraint*_tmp8A4;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8A1)->tag == 5){_LL737: _tmp8A2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8A1)->f1).elt_typ;_tmp8A3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8A1)->f1).ptr_atts).bounds;_tmp8A4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8A1)->f1).ptr_atts).zero_term;_LL738:
# 4411
{struct _tuple0 _tmp8A5=({struct _tuple0 _tmp8AD;_tmp8AD.f1=Cyc_Tcutil_compress(_tmp8A2);_tmp8AD.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp8A3);_tmp8AD;});struct _tuple0 _tmp8A6=_tmp8A5;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp8A6.f1)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp8A6.f1)->f2 == Cyc_Absyn_Char_sz){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp8A6.f2)->tag == 0){_LL73C: _LL73D:
# 4413
 if(_tmp79E)
({void*_tmp8A7=0;Cyc_Tcutil_terr(loc,({const char*_tmp8A8="format descriptor is not a char * type";_tag_dyneither(_tmp8A8,sizeof(char),39);}),_tag_dyneither(_tmp8A7,sizeof(void*),0));});
goto _LL73B;}else{_LL73E: _LL73F:
# 4417
 if(!_tmp79E)
({void*_tmp8A9=0;Cyc_Tcutil_terr(loc,({const char*_tmp8AA="format descriptor is not a char ? type";_tag_dyneither(_tmp8AA,sizeof(char),39);}),_tag_dyneither(_tmp8A9,sizeof(void*),0));});
goto _LL73B;}}else{goto _LL740;}}else{_LL740: _LL741:
# 4421
({void*_tmp8AB=0;Cyc_Tcutil_terr(loc,({const char*_tmp8AC="format descriptor is not a string type";_tag_dyneither(_tmp8AC,sizeof(char),39);}),_tag_dyneither(_tmp8AB,sizeof(void*),0));});
goto _LL73B;}_LL73B:;}
# 4424
goto _LL736;}else{_LL739: _LL73A:
({void*_tmp8AE=0;Cyc_Tcutil_terr(loc,({const char*_tmp8AF="format descriptor is not a string type";_tag_dyneither(_tmp8AF,sizeof(char),39);}),_tag_dyneither(_tmp8AE,sizeof(void*),0));});goto _LL736;}_LL736:;}
# 4427
if(fmt_arg_start != 0  && !_tmp79E){
# 4431
int problem;
{struct _tuple31 _tmp8B0=({struct _tuple31 _tmp8B4;_tmp8B4.f1=ft;_tmp8B4.f2=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp79F))->type));_tmp8B4;});struct _tuple31 _tmp8B1=_tmp8B0;struct Cyc_Absyn_Datatypedecl*_tmp8B2;struct Cyc_Absyn_Datatypedecl*_tmp8B3;if(_tmp8B1.f1 == Cyc_Absyn_Printf_ft){if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp8B1.f2)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp8B1.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL743: _tmp8B3=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp8B1.f2)->f1).datatype_info).KnownDatatype).val;_LL744:
# 4434
 problem=Cyc_Absyn_qvar_cmp(_tmp8B3->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL742;}else{goto _LL747;}}else{goto _LL747;}}else{if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp8B1.f2)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp8B1.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL745: _tmp8B2=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp8B1.f2)->f1).datatype_info).KnownDatatype).val;_LL746:
# 4436
 problem=Cyc_Absyn_qvar_cmp(_tmp8B2->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL742;}else{goto _LL747;}}else{_LL747: _LL748:
# 4438
 problem=1;goto _LL742;}}_LL742:;}
# 4440
if(problem)
({void*_tmp8B5=0;Cyc_Tcutil_terr(loc,({const char*_tmp8B6="format attribute and vararg types don't match";_tag_dyneither(_tmp8B6,sizeof(char),46);}),_tag_dyneither(_tmp8B5,sizeof(void*),0));});}}}
# 4448
{struct Cyc_List_List*rpo=_tmp7A0;for(0;rpo != 0;rpo=rpo->tl){
struct _tuple0*_tmp8B7=(struct _tuple0*)rpo->hd;struct _tuple0*_tmp8B8=_tmp8B7;void*_tmp8B9;void*_tmp8BA;_LL74A: _tmp8B9=_tmp8B8->f1;_tmp8BA=_tmp8B8->f2;_LL74B:;
_tmp87E=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp87E,& Cyc_Tcutil_ek,_tmp8B9,1,1);
_tmp87E=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp87E,& Cyc_Tcutil_trk,_tmp8BA,1,1);}}{
# 4456
struct Cyc_Tcenv_Fenv*_tmp8BB=Cyc_Tcenv_bogus_fenv(newr,_tmp79C,_tmp79D);
struct Cyc_Tcenv_Tenv*_tmp8BC=({struct Cyc_Tcenv_Tenv*_tmp910=_region_malloc(newr,sizeof(*_tmp910));_tmp910->ns=te->ns;_tmp910->ae=te->ae;_tmp910->le=_tmp8BB;_tmp910->allow_valueof=1;_tmp910->in_extern_c_include=te->in_extern_c_include;_tmp910;});
struct _tuple30 _tmp8BD=Cyc_Tcutil_check_clause(loc,_tmp8BC,_tmp87E,({const char*_tmp90F="@requires";_tag_dyneither(_tmp90F,sizeof(char),10);}),_tmp7A2);struct _tuple30 _tmp8BE=_tmp8BD;struct Cyc_Tcutil_CVTEnv _tmp8BF;struct Cyc_List_List*_tmp8C0;_LL74D: _tmp8BF=_tmp8BE.f1;_tmp8C0=_tmp8BE.f2;_LL74E:;
_tmp87E=_tmp8BF;
*_tmp7A3=_tmp8C0;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp79D,_tmp8C0);{
# 4463
struct _dyneither_ptr*_tmp8C1=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"return_value",sizeof(char),13);
struct Cyc_Absyn_Vardecl*_tmp8C2=({struct Cyc_Absyn_Vardecl*_tmp90C=_cycalloc(sizeof(*_tmp90C));_tmp90C->sc=Cyc_Absyn_Public;_tmp90C->name=({struct _tuple2*_tmp90D=_cycalloc(sizeof(*_tmp90D));_tmp90D->f1=Cyc_Absyn_Loc_n;_tmp90D->f2=_tmp8C1;_tmp90D;});_tmp90C->tq=
Cyc_Absyn_empty_tqual(0);_tmp90C->type=_tmp79C;_tmp90C->initializer=0;_tmp90C->rgn=
Cyc_Tcenv_curr_rgn(_tmp8BC);_tmp90C->attributes=0;_tmp90C->escapes=0;_tmp90C;});
# 4468
_tmp8BC=Cyc_Tcenv_add_local_var(newr,0,_tmp8BC,_tmp8C2);{
struct _tuple30 _tmp8C3=Cyc_Tcutil_check_clause(loc,_tmp8BC,_tmp87E,({const char*_tmp90B="@ensures";_tag_dyneither(_tmp90B,sizeof(char),9);}),_tmp7A4);struct _tuple30 _tmp8C4=_tmp8C3;struct Cyc_Tcutil_CVTEnv _tmp8C5;struct Cyc_List_List*_tmp8C6;_LL750: _tmp8C5=_tmp8C4.f1;_tmp8C6=_tmp8C4.f2;_LL751:;
_tmp87E=_tmp8C5;
*_tmp7A5=_tmp8C6;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp79D,_tmp8C6);
# 4474
if(*_tmp79A != 0)
_tmp87E=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp87E,& Cyc_Tcutil_ek,(void*)_check_null(*_tmp79A),1,1);else{
# 4477
struct Cyc_List_List*effect=0;
# 4482
{struct Cyc_List_List*tvs=_tmp87E.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp8C7=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp8C8=_tmp8C7;struct Cyc_Absyn_Tvar*_tmp8C9;int _tmp8CA;_LL753: _tmp8C9=_tmp8C8.f1;_tmp8CA=_tmp8C8.f2;_LL754:;
if(!_tmp8CA)continue;{
void*_tmp8CB=Cyc_Absyn_compress_kb(_tmp8C9->kind);void*_tmp8CC=_tmp8CB;struct Cyc_Core_Opt**_tmp8CD;struct Cyc_Absyn_Kind*_tmp8CE;struct Cyc_Core_Opt**_tmp8CF;struct Cyc_Core_Opt**_tmp8D0;struct Cyc_Absyn_Kind*_tmp8D1;switch(*((int*)_tmp8CC)){case 2: _LL756: _tmp8D0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8CC)->f1;_tmp8D1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8CC)->f2;if(_tmp8D1->kind == Cyc_Absyn_RgnKind){_LL757:
# 4488
 if(_tmp8D1->aliasqual == Cyc_Absyn_Top){
*_tmp8D0=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);_tmp8CE=_tmp8D1;goto _LL759;}
# 4491
*_tmp8D0=Cyc_Tcutil_kind_to_bound_opt(_tmp8D1);_tmp8CE=_tmp8D1;goto _LL759;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8CC)->f2)->kind){case Cyc_Absyn_IntKind: _LL75A: _LL75B:
# 4494
 goto _LL75D;case Cyc_Absyn_EffKind: _LL75E: _tmp8CF=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8CC)->f1;_LL75F:
# 4497
*_tmp8CF=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);goto _LL761;default: goto _LL764;}}case 0: _LL758: _tmp8CE=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp8CC)->f1;if(_tmp8CE->kind == Cyc_Absyn_RgnKind){_LL759:
# 4493
 effect=({struct Cyc_List_List*_tmp8D2=_cycalloc(sizeof(*_tmp8D2));_tmp8D2->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp8D3=_cycalloc(sizeof(*_tmp8D3));_tmp8D3[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp8D4;_tmp8D4.tag=23;_tmp8D4.f1=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8D5=_cycalloc(sizeof(*_tmp8D5));_tmp8D5[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8D6;_tmp8D6.tag=2;_tmp8D6.f1=_tmp8C9;_tmp8D6;});_tmp8D5;});_tmp8D4;});_tmp8D3;});_tmp8D2->tl=effect;_tmp8D2;});goto _LL755;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp8CC)->f1)->kind){case Cyc_Absyn_IntKind: _LL75C: _LL75D:
# 4495
 goto _LL755;case Cyc_Absyn_EffKind: _LL760: _LL761:
# 4499
 effect=({struct Cyc_List_List*_tmp8D7=_cycalloc(sizeof(*_tmp8D7));_tmp8D7->hd=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8D8=_cycalloc(sizeof(*_tmp8D8));_tmp8D8[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8D9;_tmp8D9.tag=2;_tmp8D9.f1=_tmp8C9;_tmp8D9;});_tmp8D8;});_tmp8D7->tl=effect;_tmp8D7;});goto _LL755;default: _LL764: _LL765:
# 4504
 effect=({struct Cyc_List_List*_tmp8DD=_cycalloc(sizeof(*_tmp8DD));_tmp8DD->hd=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp8DE=_cycalloc(sizeof(*_tmp8DE));_tmp8DE[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp8DF;_tmp8DF.tag=25;_tmp8DF.f1=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8E0=_cycalloc(sizeof(*_tmp8E0));_tmp8E0[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8E1;_tmp8E1.tag=2;_tmp8E1.f1=_tmp8C9;_tmp8E1;});_tmp8E0;});_tmp8DF;});_tmp8DE;});_tmp8DD->tl=effect;_tmp8DD;});goto _LL755;}}default: _LL762: _tmp8CD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp8CC)->f1;_LL763:
# 4501
*_tmp8CD=({struct Cyc_Core_Opt*_tmp8DA=_cycalloc(sizeof(*_tmp8DA));_tmp8DA->v=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8DB=_cycalloc(sizeof(*_tmp8DB));_tmp8DB[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp8DC;_tmp8DC.tag=2;_tmp8DC.f1=0;_tmp8DC.f2=& Cyc_Tcutil_ak;_tmp8DC;});_tmp8DB;});_tmp8DA;});goto _LL765;}_LL755:;};}}
# 4508
{struct Cyc_List_List*ts=_tmp87E.free_evars;for(0;ts != 0;ts=ts->tl){
struct _tuple29 _tmp8E2=*((struct _tuple29*)ts->hd);struct _tuple29 _tmp8E3=_tmp8E2;void*_tmp8E4;int _tmp8E5;_LL767: _tmp8E4=_tmp8E3.f1;_tmp8E5=_tmp8E3.f2;_LL768:;
if(!_tmp8E5)continue;{
struct Cyc_Absyn_Kind*_tmp8E6=Cyc_Tcutil_typ_kind(_tmp8E4);struct Cyc_Absyn_Kind*_tmp8E7=_tmp8E6;switch(((struct Cyc_Absyn_Kind*)_tmp8E7)->kind){case Cyc_Absyn_RgnKind: _LL76A: _LL76B:
# 4513
 effect=({struct Cyc_List_List*_tmp8E8=_cycalloc(sizeof(*_tmp8E8));_tmp8E8->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp8E9=_cycalloc(sizeof(*_tmp8E9));_tmp8E9[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp8EA;_tmp8EA.tag=23;_tmp8EA.f1=_tmp8E4;_tmp8EA;});_tmp8E9;});_tmp8E8->tl=effect;_tmp8E8;});goto _LL769;case Cyc_Absyn_EffKind: _LL76C: _LL76D:
# 4515
 effect=({struct Cyc_List_List*_tmp8EB=_cycalloc(sizeof(*_tmp8EB));_tmp8EB->hd=_tmp8E4;_tmp8EB->tl=effect;_tmp8EB;});goto _LL769;case Cyc_Absyn_IntKind: _LL76E: _LL76F:
 goto _LL769;default: _LL770: _LL771:
# 4518
 effect=({struct Cyc_List_List*_tmp8EC=_cycalloc(sizeof(*_tmp8EC));_tmp8EC->hd=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp8ED=_cycalloc(sizeof(*_tmp8ED));_tmp8ED[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp8EE;_tmp8EE.tag=25;_tmp8EE.f1=_tmp8E4;_tmp8EE;});_tmp8ED;});_tmp8EC->tl=effect;_tmp8EC;});goto _LL769;}_LL769:;};}}
# 4521
*_tmp79A=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp8EF=_cycalloc(sizeof(*_tmp8EF));_tmp8EF[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp8F0;_tmp8F0.tag=24;_tmp8F0.f1=effect;_tmp8F0;});_tmp8EF;});}
# 4527
if(*_tmp799 != 0){
struct Cyc_List_List*bs=*_tmp799;for(0;bs != 0;bs=bs->tl){
void*_tmp8F1=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);void*_tmp8F2=_tmp8F1;struct Cyc_Core_Opt**_tmp8F3;struct Cyc_Absyn_Kind*_tmp8F4;struct Cyc_Core_Opt**_tmp8F5;struct Cyc_Core_Opt**_tmp8F6;struct Cyc_Core_Opt**_tmp8F7;struct Cyc_Core_Opt**_tmp8F8;struct Cyc_Core_Opt**_tmp8F9;struct Cyc_Core_Opt**_tmp8FA;struct Cyc_Core_Opt**_tmp8FB;struct Cyc_Core_Opt**_tmp8FC;struct Cyc_Core_Opt**_tmp8FD;switch(*((int*)_tmp8F2)){case 1: _LL773: _tmp8FD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp8F2)->f1;_LL774:
# 4531
({struct Cyc_String_pa_PrintArg_struct _tmp900;_tmp900.tag=0;_tmp900.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name);({void*_tmp8FE[1]={& _tmp900};Cyc_Tcutil_warn(loc,({const char*_tmp8FF="Type variable %s unconstrained, assuming boxed";_tag_dyneither(_tmp8FF,sizeof(char),47);}),_tag_dyneither(_tmp8FE,sizeof(void*),1));});});
# 4533
_tmp8FC=_tmp8FD;goto _LL776;case 2: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F2)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F2)->f2)->aliasqual == Cyc_Absyn_Top){_LL775: _tmp8FC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F2)->f1;_LL776:
 _tmp8FB=_tmp8FC;goto _LL778;}else{goto _LL785;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F2)->f2)->aliasqual){case Cyc_Absyn_Top: _LL777: _tmp8FB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F2)->f1;_LL778:
 _tmp8FA=_tmp8FB;goto _LL77A;case Cyc_Absyn_Aliasable: _LL779: _tmp8FA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F2)->f1;_LL77A:
 _tmp8F8=_tmp8FA;goto _LL77C;default: _LL77F: _tmp8F9=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F2)->f1;_LL780:
# 4540
 _tmp8F6=_tmp8F9;goto _LL782;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F2)->f2)->aliasqual){case Cyc_Absyn_Top: _LL77B: _tmp8F8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F2)->f1;_LL77C:
# 4537
 _tmp8F7=_tmp8F8;goto _LL77E;case Cyc_Absyn_Aliasable: _LL77D: _tmp8F7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F2)->f1;_LL77E:
# 4539
*_tmp8F7=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL772;default: _LL781: _tmp8F6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F2)->f1;_LL782:
# 4542
*_tmp8F6=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL772;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F2)->f2)->aliasqual == Cyc_Absyn_Top){_LL783: _tmp8F5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F2)->f1;_LL784:
# 4544
*_tmp8F5=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL772;}else{goto _LL785;}default: _LL785: _tmp8F3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F2)->f1;_tmp8F4=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F2)->f2;_LL786:
# 4546
*_tmp8F3=Cyc_Tcutil_kind_to_bound_opt(_tmp8F4);goto _LL772;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp8F2)->f1)->kind == Cyc_Absyn_MemKind){_LL787: _LL788:
# 4548
({void*_tmp901=0;Cyc_Tcutil_terr(loc,({const char*_tmp902="functions can't abstract M types";_tag_dyneither(_tmp902,sizeof(char),33);}),_tag_dyneither(_tmp901,sizeof(void*),0));});goto _LL772;}else{_LL789: _LL78A:
 goto _LL772;}}_LL772:;}}
# 4553
cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,_tmp87E.kind_env,*_tmp799);
_tmp87E.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(_tmp87E.r,_tmp87E.free_vars,*_tmp799);
# 4556
{struct Cyc_List_List*tvs=_tmp87E.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp903=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp904=_tmp903;struct Cyc_Absyn_Tvar*_tmp905;int _tmp906;_LL78C: _tmp905=_tmp904.f1;_tmp906=_tmp904.f2;_LL78D:;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp905,_tmp906);}}{
# 4561
struct Cyc_List_List*evs=_tmp87E.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmp907=*((struct _tuple29*)evs->hd);struct _tuple29 _tmp908=_tmp907;void*_tmp909;int _tmp90A;_LL78F: _tmp909=_tmp908.f1;_tmp90A=_tmp908.f2;_LL790:;
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmp909,_tmp90A);}};};};};}
# 4566
_npop_handler(0);goto _LL66E;
# 4341
;_pop_region(newr);};}case 10: _LL691: _tmp798=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp789)->f1;_LL692:
# 4569
 for(0;_tmp798 != 0;_tmp798=_tmp798->tl){
struct _tuple12*_tmp912=(struct _tuple12*)_tmp798->hd;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmp912).f2,1,0);
((*_tmp912).f1).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp912).f1).print_const,(*_tmp912).f2);}
# 4575
goto _LL66E;case 12: _LL693: _tmp796=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp789)->f1;_tmp797=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp789)->f2;_LL694: {
# 4579
struct _RegionHandle _tmp913=_new_region("aprev_rgn");struct _RegionHandle*aprev_rgn=& _tmp913;_push_region(aprev_rgn);{
struct Cyc_List_List*prev_fields=0;
for(0;_tmp797 != 0;_tmp797=_tmp797->tl){
struct Cyc_Absyn_Aggrfield*_tmp914=(struct Cyc_Absyn_Aggrfield*)_tmp797->hd;struct Cyc_Absyn_Aggrfield*_tmp915=_tmp914;struct _dyneither_ptr*_tmp916;struct Cyc_Absyn_Tqual*_tmp917;void*_tmp918;struct Cyc_Absyn_Exp*_tmp919;struct Cyc_List_List*_tmp91A;struct Cyc_Absyn_Exp*_tmp91B;_LL792: _tmp916=_tmp915->name;_tmp917=(struct Cyc_Absyn_Tqual*)& _tmp915->tq;_tmp918=_tmp915->type;_tmp919=_tmp915->width;_tmp91A=_tmp915->attributes;_tmp91B=_tmp915->requires_clause;_LL793:;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp916))
({struct Cyc_String_pa_PrintArg_struct _tmp91E;_tmp91E.tag=0;_tmp91E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp916);({void*_tmp91C[1]={& _tmp91E};Cyc_Tcutil_terr(loc,({const char*_tmp91D="duplicate field %s";_tag_dyneither(_tmp91D,sizeof(char),19);}),_tag_dyneither(_tmp91C,sizeof(void*),1));});});
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp916,({const char*_tmp91F="";_tag_dyneither(_tmp91F,sizeof(char),1);}))!= 0)
prev_fields=({struct Cyc_List_List*_tmp920=_region_malloc(aprev_rgn,sizeof(*_tmp920));_tmp920->hd=_tmp916;_tmp920->tl=prev_fields;_tmp920;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp918,1,0);
_tmp917->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp917->print_const,_tmp918);
# 4593
Cyc_Tcutil_check_bitfield(loc,te,_tmp918,_tmp919,_tmp916);
Cyc_Tcutil_check_field_atts(loc,_tmp916,_tmp91A);
if(_tmp91B != 0){
# 4597
if(_tmp796 != Cyc_Absyn_UnionA)
({void*_tmp921=0;Cyc_Tcutil_terr(loc,({const char*_tmp922="@requires clause is only allowed on union members";_tag_dyneither(_tmp922,sizeof(char),50);}),_tag_dyneither(_tmp921,sizeof(void*),0));});{
struct _RegionHandle _tmp923=_new_region("temp");struct _RegionHandle*temp=& _tmp923;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp924=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp924,0,_tmp91B);}
# 4603
if(!Cyc_Tcutil_is_integral(_tmp91B))
({struct Cyc_String_pa_PrintArg_struct _tmp927;_tmp927.tag=0;_tmp927.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmp91B->topt)));({void*_tmp925[1]={& _tmp927};Cyc_Tcutil_terr(loc,({const char*_tmp926="@requires clause has type %s instead of integral type";_tag_dyneither(_tmp926,sizeof(char),54);}),_tag_dyneither(_tmp925,sizeof(void*),1));});});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp91B,te,cvtenv);
# 4599
;_pop_region(temp);};}}}
# 4610
_npop_handler(0);goto _LL66E;
# 4579
;_pop_region(aprev_rgn);}case 11: _LL695: _tmp794=(union Cyc_Absyn_AggrInfoU*)&(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp789)->f1).aggr_info;_tmp795=(struct Cyc_List_List**)&(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp789)->f1).targs;_LL696:
# 4613
{union Cyc_Absyn_AggrInfoU _tmp928=*_tmp794;union Cyc_Absyn_AggrInfoU _tmp929=_tmp928;struct Cyc_Absyn_Aggrdecl*_tmp92A;enum Cyc_Absyn_AggrKind _tmp92B;struct _tuple2*_tmp92C;struct Cyc_Core_Opt*_tmp92D;if((_tmp929.UnknownAggr).tag == 1){_LL795: _tmp92B=((_tmp929.UnknownAggr).val).f1;_tmp92C=((_tmp929.UnknownAggr).val).f2;_tmp92D=((_tmp929.UnknownAggr).val).f3;_LL796: {
# 4615
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmp92E;_push_handler(& _tmp92E);{int _tmp930=0;if(setjmp(_tmp92E.handler))_tmp930=1;if(!_tmp930){
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp92C);{
struct Cyc_Absyn_Aggrdecl*_tmp931=*adp;
if(_tmp931->kind != _tmp92B){
if(_tmp931->kind == Cyc_Absyn_StructA)
({struct Cyc_String_pa_PrintArg_struct _tmp935;_tmp935.tag=0;_tmp935.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp92C));({struct Cyc_String_pa_PrintArg_struct _tmp934;_tmp934.tag=0;_tmp934.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4621
Cyc_Absynpp_qvar2string(_tmp92C));({void*_tmp932[2]={& _tmp934,& _tmp935};Cyc_Tcutil_terr(loc,({const char*_tmp933="expecting struct %s instead of union %s";_tag_dyneither(_tmp933,sizeof(char),40);}),_tag_dyneither(_tmp932,sizeof(void*),2));});});});else{
# 4624
({struct Cyc_String_pa_PrintArg_struct _tmp939;_tmp939.tag=0;_tmp939.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp92C));({struct Cyc_String_pa_PrintArg_struct _tmp938;_tmp938.tag=0;_tmp938.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4624
Cyc_Absynpp_qvar2string(_tmp92C));({void*_tmp936[2]={& _tmp938,& _tmp939};Cyc_Tcutil_terr(loc,({const char*_tmp937="expecting union %s instead of struct %s";_tag_dyneither(_tmp937,sizeof(char),40);}),_tag_dyneither(_tmp936,sizeof(void*),2));});});});}}
# 4627
if((unsigned int)_tmp92D  && (int)_tmp92D->v){
if(!((unsigned int)_tmp931->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp931->impl))->tagged)
({struct Cyc_String_pa_PrintArg_struct _tmp93C;_tmp93C.tag=0;_tmp93C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp92C));({void*_tmp93A[1]={& _tmp93C};Cyc_Tcutil_terr(loc,({const char*_tmp93B="@tagged qualfiers don't agree on union %s";_tag_dyneither(_tmp93B,sizeof(char),42);}),_tag_dyneither(_tmp93A,sizeof(void*),1));});});}
# 4633
*_tmp794=Cyc_Absyn_KnownAggr(adp);};
# 4617
;_pop_handler();}else{void*_tmp92F=(void*)_exn_thrown;void*_tmp93D=_tmp92F;void*_tmp93E;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp93D)->tag == Cyc_Dict_Absent){_LL79A: _LL79B: {
# 4637
struct Cyc_Tcenv_Genv*_tmp93F=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp940=({struct Cyc_Absyn_Aggrdecl*_tmp944=_cycalloc(sizeof(*_tmp944));_tmp944->kind=_tmp92B;_tmp944->sc=Cyc_Absyn_Extern;_tmp944->name=_tmp92C;_tmp944->tvs=0;_tmp944->impl=0;_tmp944->attributes=0;_tmp944->expected_mem_kind=0;_tmp944;});
Cyc_Tc_tcAggrdecl(te,_tmp93F,loc,_tmp940);
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp92C);
*_tmp794=Cyc_Absyn_KnownAggr(adp);
# 4643
if(*_tmp795 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp943;_tmp943.tag=0;_tmp943.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp92C));({void*_tmp941[1]={& _tmp943};Cyc_Tcutil_terr(loc,({const char*_tmp942="declare parameterized type %s before using";_tag_dyneither(_tmp942,sizeof(char),43);}),_tag_dyneither(_tmp941,sizeof(void*),1));});});
return cvtenv;}
# 4648
goto _LL799;}}else{_LL79C: _tmp93E=_tmp93D;_LL79D:(int)_rethrow(_tmp93E);}_LL799:;}};}
# 4650
_tmp92A=*adp;goto _LL798;}}else{_LL797: _tmp92A=*(_tmp929.KnownAggr).val;_LL798: {
# 4652
struct Cyc_List_List*tvs=_tmp92A->tvs;
struct Cyc_List_List*ts=*_tmp795;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Tvar*_tmp945=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp946=(void*)ts->hd;
# 4660
{struct _tuple0 _tmp947=({struct _tuple0 _tmp94A;_tmp94A.f1=Cyc_Absyn_compress_kb(_tmp945->kind);_tmp94A.f2=_tmp946;_tmp94A;});struct _tuple0 _tmp948=_tmp947;struct Cyc_Absyn_Tvar*_tmp949;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp948.f1)->tag == 1){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp948.f2)->tag == 2){_LL79F: _tmp949=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp948.f2)->f1;_LL7A0:
# 4662
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp949);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp949,1);
continue;}else{goto _LL7A1;}}else{_LL7A1: _LL7A2:
 goto _LL79E;}_LL79E:;}{
# 4667
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 4671
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp94D;_tmp94D.tag=0;_tmp94D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp92A->name));({void*_tmp94B[1]={& _tmp94D};Cyc_Tcutil_terr(loc,({const char*_tmp94C="too many parameters for type %s";_tag_dyneither(_tmp94C,sizeof(char),32);}),_tag_dyneither(_tmp94B,sizeof(void*),1));});});
if(tvs != 0){
# 4675
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp94E=_cycalloc(sizeof(*_tmp94E));_tmp94E->hd=e;_tmp94E->tl=hidden_ts;_tmp94E;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);}
# 4682
*_tmp795=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp795,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4684
if((allow_abs_aggr  && _tmp92A->impl == 0) && !
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,expected_kind))
# 4689
_tmp92A->expected_mem_kind=1;}}_LL794:;}
# 4692
goto _LL66E;case 17: _LL697: _tmp790=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp789)->f1;_tmp791=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp789)->f2;_tmp792=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp789)->f3;_tmp793=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp789)->f4;_LL698: {
# 4695
struct Cyc_List_List*targs=*_tmp791;
# 4697
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmp94F;_push_handler(& _tmp94F);{int _tmp951=0;if(setjmp(_tmp94F.handler))_tmp951=1;if(!_tmp951){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp790);;_pop_handler();}else{void*_tmp950=(void*)_exn_thrown;void*_tmp952=_tmp950;void*_tmp953;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp952)->tag == Cyc_Dict_Absent){_LL7A4: _LL7A5:
# 4700
({struct Cyc_String_pa_PrintArg_struct _tmp956;_tmp956.tag=0;_tmp956.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp790));({void*_tmp954[1]={& _tmp956};Cyc_Tcutil_terr(loc,({const char*_tmp955="unbound typedef name %s";_tag_dyneither(_tmp955,sizeof(char),24);}),_tag_dyneither(_tmp954,sizeof(void*),1));});});
return cvtenv;}else{_LL7A6: _tmp953=_tmp952;_LL7A7:(int)_rethrow(_tmp953);}_LL7A3:;}};}
# 4703
*_tmp792=td;
# 4705
_tmp790[0]=(td->name)[0];{
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;
struct _RegionHandle _tmp957=_new_region("temp");struct _RegionHandle*temp=& _tmp957;_push_region(temp);{
struct Cyc_List_List*inst=0;
# 4711
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,td);
# 4716
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
inst=({struct Cyc_List_List*_tmp958=_region_malloc(temp,sizeof(*_tmp958));_tmp958->hd=({struct _tuple16*_tmp959=_region_malloc(temp,sizeof(*_tmp959));_tmp959->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp959->f2=(void*)ts->hd;_tmp959;});_tmp958->tl=inst;_tmp958;});}
# 4720
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp95C;_tmp95C.tag=0;_tmp95C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp790));({void*_tmp95A[1]={& _tmp95C};Cyc_Tcutil_terr(loc,({const char*_tmp95B="too many parameters for typedef %s";_tag_dyneither(_tmp95B,sizeof(char),35);}),_tag_dyneither(_tmp95A,sizeof(void*),1));});});
if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;
# 4725
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp95D=_cycalloc(sizeof(*_tmp95D));_tmp95D->hd=e;_tmp95D->tl=hidden_ts;_tmp95D;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);
inst=({struct Cyc_List_List*_tmp95E=_cycalloc(sizeof(*_tmp95E));_tmp95E->hd=({struct _tuple16*_tmp95F=_cycalloc(sizeof(*_tmp95F));_tmp95F->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp95F->f2=e;_tmp95F;});_tmp95E->tl=inst;_tmp95E;});}
# 4733
*_tmp791=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4735
if(td->defn != 0){
void*new_typ=
inst == 0?(void*)_check_null(td->defn):
 Cyc_Tcutil_rsubstitute(temp,inst,(void*)_check_null(td->defn));
*_tmp793=new_typ;}}
# 4742
_npop_handler(0);goto _LL66E;
# 4708
;_pop_region(temp);};}case 22: _LL699: _LL69A:
# 4743
 goto _LL69C;case 21: _LL69B: _LL69C:
 goto _LL69E;case 20: _LL69D: _LL69E:
 goto _LL66E;case 15: _LL69F: _tmp78F=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp789)->f1;_LL6A0:
# 4747
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp78F,1,allow_abs_aggr);goto _LL66E;case 16: _LL6A1: _tmp78D=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp789)->f1;_tmp78E=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp789)->f2;_LL6A2:
# 4750
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp78D,0,0);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp78E,1,0);
goto _LL66E;case 23: _LL6A3: _tmp78C=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp789)->f1;_LL6A4:
# 4754
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp78C,1,1);goto _LL66E;case 25: _LL6A5: _tmp78B=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp789)->f1;_LL6A6:
# 4756
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp78B,1,1);goto _LL66E;default: _LL6A7: _tmp78A=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp789)->f1;_LL6A8:
# 4758
 for(0;_tmp78A != 0;_tmp78A=_tmp78A->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmp78A->hd,1,1);}
goto _LL66E;}_LL66E:;}
# 4762
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind))
({struct Cyc_String_pa_PrintArg_struct _tmp964;_tmp964.tag=0;_tmp964.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(expected_kind));({struct Cyc_String_pa_PrintArg_struct _tmp963;_tmp963.tag=0;_tmp963.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t)));({struct Cyc_String_pa_PrintArg_struct _tmp962;_tmp962.tag=0;_tmp962.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp960[3]={& _tmp962,& _tmp963,& _tmp964};Cyc_Tcutil_terr(loc,({const char*_tmp961="type %s has kind %s but as used here needs kind %s";_tag_dyneither(_tmp961,sizeof(char),51);}),_tag_dyneither(_tmp960,sizeof(void*),3));});});});});
# 4766
return cvtenv;}
# 4774
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){
# 4776
{void*_tmp965=e->r;void*_tmp966=_tmp965;struct Cyc_Absyn_Exp*_tmp967;void*_tmp968;void*_tmp969;void*_tmp96A;void*_tmp96B;struct Cyc_Absyn_Exp*_tmp96C;struct Cyc_Absyn_Exp*_tmp96D;struct Cyc_Absyn_Exp*_tmp96E;struct Cyc_Absyn_Exp*_tmp96F;struct Cyc_Absyn_Exp*_tmp970;struct Cyc_Absyn_Exp*_tmp971;struct Cyc_Absyn_Exp*_tmp972;struct Cyc_Absyn_Exp*_tmp973;struct Cyc_Absyn_Exp*_tmp974;struct Cyc_Absyn_Exp*_tmp975;struct Cyc_List_List*_tmp976;switch(*((int*)_tmp966)){case 0: _LL7A9: _LL7AA:
 goto _LL7AC;case 31: _LL7AB: _LL7AC:
 goto _LL7AE;case 32: _LL7AD: _LL7AE:
 goto _LL7B0;case 1: _LL7AF: _LL7B0:
 goto _LL7A8;case 2: _LL7B1: _tmp976=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp966)->f2;_LL7B2:
# 4782
 for(0;_tmp976 != 0;_tmp976=_tmp976->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmp976->hd,te,cvtenv);}
goto _LL7A8;case 5: _LL7B3: _tmp973=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp966)->f1;_tmp974=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp966)->f2;_tmp975=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp966)->f3;_LL7B4:
# 4786
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp973,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp974,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp975,te,cvtenv);
goto _LL7A8;case 6: _LL7B5: _tmp971=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp966)->f1;_tmp972=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp966)->f2;_LL7B6:
 _tmp96F=_tmp971;_tmp970=_tmp972;goto _LL7B8;case 7: _LL7B7: _tmp96F=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp966)->f1;_tmp970=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp966)->f2;_LL7B8:
 _tmp96D=_tmp96F;_tmp96E=_tmp970;goto _LL7BA;case 8: _LL7B9: _tmp96D=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp966)->f1;_tmp96E=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp966)->f2;_LL7BA:
# 4793
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp96D,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp96E,te,cvtenv);
goto _LL7A8;case 13: _LL7BB: _tmp96B=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp966)->f1;_tmp96C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp966)->f2;_LL7BC:
# 4797
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp96C,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp96B,1,0);
goto _LL7A8;case 18: _LL7BD: _tmp96A=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp966)->f1;_LL7BE:
 _tmp969=_tmp96A;goto _LL7C0;case 16: _LL7BF: _tmp969=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp966)->f1;_LL7C0:
# 4802
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp969,1,0);
goto _LL7A8;case 38: _LL7C1: _tmp968=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp966)->f1;_LL7C2:
# 4805
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp968,1,0);
goto _LL7A8;case 17: _LL7C3: _tmp967=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp966)->f1;_LL7C4:
# 4808
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp967,te,cvtenv);
goto _LL7A8;default: _LL7C5: _LL7C6:
# 4811
({void*_tmp977=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp978="non-type-level-expression in Tcutil::i_check_valid_type_level_exp";_tag_dyneither(_tmp978,sizeof(char),66);}),_tag_dyneither(_tmp977,sizeof(void*),0));});}_LL7A8:;}
# 4813
return cvtenv;}
# 4816
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,int allow_abs_aggr,void*t){
# 4822
struct Cyc_List_List*_tmp979=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1,allow_abs_aggr);
# 4825
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
struct _tuple28 _tmp97A=*((struct _tuple28*)vs->hd);struct _tuple28 _tmp97B=_tmp97A;struct Cyc_Absyn_Tvar*_tmp97C;_LL7C8: _tmp97C=_tmp97B.f1;_LL7C9:;
cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(_tmp979,_tmp97C);}}
# 4833
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmp97D=*((struct _tuple29*)evs->hd);struct _tuple29 _tmp97E=_tmp97D;void*_tmp97F;_LL7CB: _tmp97F=_tmp97E.f1;_LL7CC:;{
void*_tmp980=Cyc_Tcutil_compress(_tmp97F);void*_tmp981=_tmp980;struct Cyc_Core_Opt**_tmp982;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp981)->tag == 1){_LL7CE: _tmp982=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp981)->f4;_LL7CF:
# 4837
 if(*_tmp982 == 0)
*_tmp982=({struct Cyc_Core_Opt*_tmp983=_cycalloc(sizeof(*_tmp983));_tmp983->v=_tmp979;_tmp983;});else{
# 4841
struct Cyc_List_List*_tmp984=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp982))->v;
struct Cyc_List_List*_tmp985=0;
for(0;_tmp984 != 0;_tmp984=_tmp984->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp979,(struct Cyc_Absyn_Tvar*)_tmp984->hd))
_tmp985=({struct Cyc_List_List*_tmp986=_cycalloc(sizeof(*_tmp986));_tmp986->hd=(struct Cyc_Absyn_Tvar*)_tmp984->hd;_tmp986->tl=_tmp985;_tmp986;});}
*_tmp982=({struct Cyc_Core_Opt*_tmp987=_cycalloc(sizeof(*_tmp987));_tmp987->v=_tmp985;_tmp987;});}
# 4848
goto _LL7CD;}else{_LL7D0: _LL7D1:
 goto _LL7CD;}_LL7CD:;};}}
# 4852
return cvt;}
# 4858
void Cyc_Tcutil_check_free_evars(struct Cyc_List_List*free_evars,void*in_t,unsigned int loc){
# 4860
for(0;free_evars != 0;free_evars=free_evars->tl){
void*e=(void*)free_evars->hd;
struct Cyc_Absyn_Kind*_tmp988=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind*_tmp989=_tmp988;switch(((struct Cyc_Absyn_Kind*)_tmp989)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp989)->aliasqual){case Cyc_Absyn_Unique: _LL7D3: _LL7D4:
# 4864
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val))
({void*_tmp98A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp98B="can't unify evar with unique region!";_tag_dyneither(_tmp98B,sizeof(char),37);}),_tag_dyneither(_tmp98A,sizeof(void*),0));});
goto _LL7D2;case Cyc_Absyn_Aliasable: _LL7D5: _LL7D6:
 goto _LL7D8;default: _LL7D7: _LL7D8:
# 4869
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val))({void*_tmp98C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp98D="can't unify evar with heap!";_tag_dyneither(_tmp98D,sizeof(char),28);}),_tag_dyneither(_tmp98C,sizeof(void*),0));});
goto _LL7D2;}case Cyc_Absyn_EffKind: _LL7D9: _LL7DA:
# 4872
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({void*_tmp98E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp98F="can't unify evar with {}!";_tag_dyneither(_tmp98F,sizeof(char),26);}),_tag_dyneither(_tmp98E,sizeof(void*),0));});
goto _LL7D2;default: _LL7DB: _LL7DC:
# 4875
({struct Cyc_String_pa_PrintArg_struct _tmp993;_tmp993.tag=0;_tmp993.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(in_t));({struct Cyc_String_pa_PrintArg_struct _tmp992;_tmp992.tag=0;_tmp992.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e));({void*_tmp990[2]={& _tmp992,& _tmp993};Cyc_Tcutil_terr(loc,({const char*_tmp991="hidden type variable %s isn't abstracted in type %s";_tag_dyneither(_tmp991,sizeof(char),52);}),_tag_dyneither(_tmp990,sizeof(void*),2));});});});
goto _LL7D2;}_LL7D2:;}}
# 4887
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_List_List*_tmp994=Cyc_Tcenv_lookup_type_vars(te);
struct Cyc_Absyn_Kind*expected_kind=generalize_evars?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
struct _RegionHandle _tmp995=_new_region("temp");struct _RegionHandle*temp=& _tmp995;_push_region(temp);
{struct Cyc_Tcutil_CVTEnv _tmp996=Cyc_Tcutil_check_valid_type(loc,te,({struct Cyc_Tcutil_CVTEnv _tmp9B4;_tmp9B4.r=temp;_tmp9B4.kind_env=_tmp994;_tmp9B4.free_vars=0;_tmp9B4.free_evars=0;_tmp9B4.generalize_evars=generalize_evars;_tmp9B4.fn_result=0;_tmp9B4;}),expected_kind,1,t);
# 4896
struct Cyc_List_List*_tmp997=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmp996.free_vars);
struct Cyc_List_List*_tmp998=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmp996.free_evars);
# 4900
if(_tmp994 != 0){
struct Cyc_List_List*_tmp999=0;
{struct Cyc_List_List*_tmp99A=_tmp997;for(0;(unsigned int)_tmp99A;_tmp99A=_tmp99A->tl){
struct Cyc_Absyn_Tvar*_tmp99B=(struct Cyc_Absyn_Tvar*)_tmp99A->hd;
int found=0;
{struct Cyc_List_List*_tmp99C=_tmp994;for(0;(unsigned int)_tmp99C;_tmp99C=_tmp99C->tl){
if(Cyc_Absyn_tvar_cmp(_tmp99B,(struct Cyc_Absyn_Tvar*)_tmp99C->hd)== 0){found=1;break;}}}
if(!found)
_tmp999=({struct Cyc_List_List*_tmp99D=_region_malloc(temp,sizeof(*_tmp99D));_tmp99D->hd=(struct Cyc_Absyn_Tvar*)_tmp99A->hd;_tmp99D->tl=_tmp999;_tmp99D;});}}
# 4910
_tmp997=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp999);}
# 4916
{struct Cyc_List_List*x=_tmp997;for(0;x != 0;x=x->tl){
void*_tmp99E=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);void*_tmp99F=_tmp99E;enum Cyc_Absyn_AliasQual _tmp9A0;struct Cyc_Core_Opt**_tmp9A1;struct Cyc_Absyn_Kind*_tmp9A2;struct Cyc_Core_Opt**_tmp9A3;struct Cyc_Core_Opt**_tmp9A4;struct Cyc_Core_Opt**_tmp9A5;struct Cyc_Core_Opt**_tmp9A6;struct Cyc_Core_Opt**_tmp9A7;struct Cyc_Core_Opt**_tmp9A8;switch(*((int*)_tmp99F)){case 1: _LL7DE: _tmp9A8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp99F)->f1;_LL7DF:
 _tmp9A7=_tmp9A8;goto _LL7E1;case 2: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99F)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99F)->f2)->aliasqual == Cyc_Absyn_Top){_LL7E0: _tmp9A7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99F)->f1;_LL7E1:
 _tmp9A6=_tmp9A7;goto _LL7E3;}else{goto _LL7EA;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99F)->f2)->aliasqual){case Cyc_Absyn_Top: _LL7E2: _tmp9A6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99F)->f1;_LL7E3:
 _tmp9A5=_tmp9A6;goto _LL7E5;case Cyc_Absyn_Aliasable: _LL7E4: _tmp9A5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99F)->f1;_LL7E5:
# 4922
*_tmp9A5=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL7DD;default: _LL7E6: _tmp9A4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99F)->f1;_LL7E7:
# 4924
*_tmp9A4=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL7DD;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99F)->f2)->aliasqual == Cyc_Absyn_Top){_LL7E8: _tmp9A3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99F)->f1;_LL7E9:
# 4926
*_tmp9A3=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL7DD;}else{goto _LL7EA;}default: _LL7EA: _tmp9A1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99F)->f1;_tmp9A2=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99F)->f2;_LL7EB:
# 4928
*_tmp9A1=Cyc_Tcutil_kind_to_bound_opt(_tmp9A2);goto _LL7DD;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp99F)->f1)->kind == Cyc_Absyn_MemKind){_LL7EC: _tmp9A0=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp99F)->f1)->aliasqual;_LL7ED:
# 4930
({struct Cyc_String_pa_PrintArg_struct _tmp9AC;_tmp9AC.tag=0;_tmp9AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp9AD=_cycalloc_atomic(sizeof(*_tmp9AD));_tmp9AD->kind=Cyc_Absyn_MemKind;_tmp9AD->aliasqual=_tmp9A0;_tmp9AD;})));({struct Cyc_String_pa_PrintArg_struct _tmp9AB;_tmp9AB.tag=0;_tmp9AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd));({void*_tmp9A9[2]={& _tmp9AB,& _tmp9AC};Cyc_Tcutil_terr(loc,({const char*_tmp9AA="type variable %s cannot have kind %s";_tag_dyneither(_tmp9AA,sizeof(char),37);}),_tag_dyneither(_tmp9A9,sizeof(void*),2));});});});
goto _LL7DD;}else{_LL7EE: _LL7EF:
 goto _LL7DD;}}_LL7DD:;}}
# 4937
if(_tmp997 != 0  || _tmp998 != 0){
{void*_tmp9AE=Cyc_Tcutil_compress(t);void*_tmp9AF=_tmp9AE;struct Cyc_List_List**_tmp9B0;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9AF)->tag == 9){_LL7F1: _tmp9B0=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9AF)->f1).tvars;_LL7F2:
# 4940
 if(*_tmp9B0 == 0){
# 4942
*_tmp9B0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmp997);
_tmp997=0;}
# 4945
goto _LL7F0;}else{_LL7F3: _LL7F4:
 goto _LL7F0;}_LL7F0:;}
# 4948
if(_tmp997 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp9B3;_tmp9B3.tag=0;_tmp9B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmp997->hd)->name);({void*_tmp9B1[1]={& _tmp9B3};Cyc_Tcutil_terr(loc,({const char*_tmp9B2="unbound type variable %s";_tag_dyneither(_tmp9B2,sizeof(char),25);}),_tag_dyneither(_tmp9B1,sizeof(void*),1));});});
Cyc_Tcutil_check_free_evars(_tmp998,t,loc);}}
# 4892
;_pop_region(temp);}
# 4959
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 4962
Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);
{void*_tmp9B5=Cyc_Tcutil_compress(t);void*_tmp9B6=_tmp9B5;struct Cyc_List_List*_tmp9B7;void*_tmp9B8;struct Cyc_Absyn_Tqual _tmp9B9;void*_tmp9BA;struct Cyc_List_List*_tmp9BB;struct Cyc_Absyn_Exp*_tmp9BC;struct Cyc_List_List*_tmp9BD;struct Cyc_Absyn_Exp*_tmp9BE;struct Cyc_List_List*_tmp9BF;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9B6)->tag == 9){_LL7F6: _tmp9B7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9B6)->f1).tvars;_tmp9B8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9B6)->f1).effect;_tmp9B9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9B6)->f1).ret_tqual;_tmp9BA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9B6)->f1).ret_typ;_tmp9BB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9B6)->f1).args;_tmp9BC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9B6)->f1).requires_clause;_tmp9BD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9B6)->f1).requires_relns;_tmp9BE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9B6)->f1).ensures_clause;_tmp9BF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9B6)->f1).ensures_relns;_LL7F7:
# 4965
 fd->tvs=_tmp9B7;
fd->effect=_tmp9B8;
{struct Cyc_List_List*_tmp9C0=fd->args;for(0;_tmp9C0 != 0;(_tmp9C0=_tmp9C0->tl,_tmp9BB=_tmp9BB->tl)){
# 4969
(*((struct _tuple10*)_tmp9C0->hd)).f2=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp9BB))->hd)).f2;
(*((struct _tuple10*)_tmp9C0->hd)).f3=(*((struct _tuple10*)_tmp9BB->hd)).f3;}}
# 4972
fd->ret_tqual=_tmp9B9;
fd->ret_type=_tmp9BA;
(fd->ret_tqual).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmp9BA);
fd->requires_clause=_tmp9BC;
fd->requires_relns=_tmp9BD;
fd->ensures_clause=_tmp9BE;
fd->ensures_relns=_tmp9BF;
goto _LL7F5;}else{_LL7F8: _LL7F9:
({void*_tmp9C1=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp9C2="check_fndecl_valid_type: not a FnType";_tag_dyneither(_tmp9C2,sizeof(char),38);}),_tag_dyneither(_tmp9C1,sizeof(void*),0));});}_LL7F5:;}{
# 4983
struct _RegionHandle _tmp9C3=_new_region("r");struct _RegionHandle*r=& _tmp9C3;_push_region(r);
Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _dyneither_ptr*(*)(struct _tuple10*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,({const char*_tmp9C4="function declaration has repeated parameter";_tag_dyneither(_tmp9C4,sizeof(char),44);}));
# 4988
fd->cached_typ=t;
# 4983
;_pop_region(r);};}
# 4993
void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,int allow_abs_aggr,void*t){
# 4999
struct _RegionHandle _tmp9C5=_new_region("r");struct _RegionHandle*r=& _tmp9C5;_push_region(r);
{struct Cyc_Tcutil_CVTEnv _tmp9C6=Cyc_Tcutil_check_valid_type(loc,te,({struct Cyc_Tcutil_CVTEnv _tmp9CE;_tmp9CE.r=r;_tmp9CE.kind_env=bound_tvars;_tmp9CE.free_vars=0;_tmp9CE.free_evars=0;_tmp9CE.generalize_evars=0;_tmp9CE.fn_result=0;_tmp9CE;}),expected_kind,allow_abs_aggr,t);
# 5003
struct Cyc_List_List*_tmp9C7=Cyc_Tcutil_remove_bound_tvars(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmp9C6.free_vars),bound_tvars);
# 5005
struct Cyc_List_List*_tmp9C8=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmp9C6.free_evars);
{struct Cyc_List_List*fs=_tmp9C7;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmp9C9=((struct Cyc_Absyn_Tvar*)fs->hd)->name;
({struct Cyc_String_pa_PrintArg_struct _tmp9CD;_tmp9CD.tag=0;_tmp9CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({struct Cyc_String_pa_PrintArg_struct _tmp9CC;_tmp9CC.tag=0;_tmp9CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp9C9);({void*_tmp9CA[2]={& _tmp9CC,& _tmp9CD};Cyc_Tcutil_terr(loc,({const char*_tmp9CB="unbound type variable %s in type %s";_tag_dyneither(_tmp9CB,sizeof(char),36);}),_tag_dyneither(_tmp9CA,sizeof(void*),2));});});});}}
# 5010
if(!allow_evars)
Cyc_Tcutil_check_free_evars(_tmp9C8,t,loc);}
# 5000
;_pop_region(r);}
# 5015
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
tv->identity=Cyc_Tcutil_new_tvar_id();}
# 5020
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 5026
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){
# 5029
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0)
({struct Cyc_String_pa_PrintArg_struct _tmp9D2;_tmp9D2.tag=0;_tmp9D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string(vs->hd));({struct Cyc_String_pa_PrintArg_struct _tmp9D1;_tmp9D1.tag=0;_tmp9D1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg);({void*_tmp9CF[2]={& _tmp9D1,& _tmp9D2};Cyc_Tcutil_terr(loc,({const char*_tmp9D0="%s: %s";_tag_dyneither(_tmp9D0,sizeof(char),7);}),_tag_dyneither(_tmp9CF,sizeof(void*),2));});});});}}}
# 5035
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s){
return*s;}
# 5039
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr msg){
((void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 5043
void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,({const char*_tmp9D3="duplicate type variable";_tag_dyneither(_tmp9D3,sizeof(char),24);}));}struct _tuple32{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple33{struct Cyc_List_List*f1;void*f2;};struct _tuple34{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 5057 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 5062
struct _RegionHandle _tmp9D4=_new_region("temp");struct _RegionHandle*temp=& _tmp9D4;_push_region(temp);
# 5066
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
if(Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,({const char*_tmp9D5="";_tag_dyneither(_tmp9D5,sizeof(char),1);}))!= 0)
fields=({struct Cyc_List_List*_tmp9D6=_region_malloc(temp,sizeof(*_tmp9D6));_tmp9D6->hd=({struct _tuple32*_tmp9D7=_region_malloc(temp,sizeof(*_tmp9D7));_tmp9D7->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd;_tmp9D7->f2=0;_tmp9D7;});_tmp9D6->tl=fields;_tmp9D6;});}}
# 5071
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 5073
struct _dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?({const char*_tmp9FD="struct";_tag_dyneither(_tmp9FD,sizeof(char),7);}):({const char*_tmp9FE="union";_tag_dyneither(_tmp9FE,sizeof(char),6);});
# 5076
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple33*_tmp9D8=(struct _tuple33*)des->hd;struct _tuple33*_tmp9D9=_tmp9D8;struct Cyc_List_List*_tmp9DA;void*_tmp9DB;_LL7FB: _tmp9DA=_tmp9D9->f1;_tmp9DB=_tmp9D9->f2;_LL7FC:;
if(_tmp9DA == 0){
# 5081
struct Cyc_List_List*_tmp9DC=fields;
for(0;_tmp9DC != 0;_tmp9DC=_tmp9DC->tl){
if(!(*((struct _tuple32*)_tmp9DC->hd)).f2){
(*((struct _tuple32*)_tmp9DC->hd)).f2=1;
(*((struct _tuple33*)des->hd)).f1=({struct Cyc_List_List*_tmp9DD=_cycalloc(sizeof(*_tmp9DD));_tmp9DD->hd=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp9DE=_cycalloc(sizeof(*_tmp9DE));_tmp9DE[0]=({struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp9DF;_tmp9DF.tag=1;_tmp9DF.f1=((*((struct _tuple32*)_tmp9DC->hd)).f1)->name;_tmp9DF;});_tmp9DE;});_tmp9DD->tl=0;_tmp9DD;});
ans=({struct Cyc_List_List*_tmp9E0=_region_malloc(rgn,sizeof(*_tmp9E0));_tmp9E0->hd=({struct _tuple34*_tmp9E1=_region_malloc(rgn,sizeof(*_tmp9E1));_tmp9E1->f1=(*((struct _tuple32*)_tmp9DC->hd)).f1;_tmp9E1->f2=_tmp9DB;_tmp9E1;});_tmp9E0->tl=ans;_tmp9E0;});
break;}}
# 5089
if(_tmp9DC == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp9E4;_tmp9E4.tag=0;_tmp9E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);({void*_tmp9E2[1]={& _tmp9E4};Cyc_Tcutil_terr(loc,({const char*_tmp9E3="too many arguments to %s";_tag_dyneither(_tmp9E3,sizeof(char),25);}),_tag_dyneither(_tmp9E2,sizeof(void*),1));});});}else{
if(_tmp9DA->tl != 0)
# 5093
({void*_tmp9E5=0;Cyc_Tcutil_terr(loc,({const char*_tmp9E6="multiple designators are not yet supported";_tag_dyneither(_tmp9E6,sizeof(char),43);}),_tag_dyneither(_tmp9E5,sizeof(void*),0));});else{
# 5096
void*_tmp9E7=(void*)_tmp9DA->hd;void*_tmp9E8=_tmp9E7;struct _dyneither_ptr*_tmp9E9;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp9E8)->tag == 0){_LL7FE: _LL7FF:
# 5098
({struct Cyc_String_pa_PrintArg_struct _tmp9EC;_tmp9EC.tag=0;_tmp9EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);({void*_tmp9EA[1]={& _tmp9EC};Cyc_Tcutil_terr(loc,({const char*_tmp9EB="array designator used in argument to %s";_tag_dyneither(_tmp9EB,sizeof(char),40);}),_tag_dyneither(_tmp9EA,sizeof(void*),1));});});
goto _LL7FD;}else{_LL800: _tmp9E9=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp9E8)->f1;_LL801: {
# 5101
struct Cyc_List_List*_tmp9ED=fields;
for(0;_tmp9ED != 0;_tmp9ED=_tmp9ED->tl){
if(Cyc_strptrcmp(_tmp9E9,((*((struct _tuple32*)_tmp9ED->hd)).f1)->name)== 0){
if((*((struct _tuple32*)_tmp9ED->hd)).f2)
({struct Cyc_String_pa_PrintArg_struct _tmp9F0;_tmp9F0.tag=0;_tmp9F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp9E9);({void*_tmp9EE[1]={& _tmp9F0};Cyc_Tcutil_terr(loc,({const char*_tmp9EF="member %s has already been used as an argument";_tag_dyneither(_tmp9EF,sizeof(char),47);}),_tag_dyneither(_tmp9EE,sizeof(void*),1));});});
(*((struct _tuple32*)_tmp9ED->hd)).f2=1;
ans=({struct Cyc_List_List*_tmp9F1=_region_malloc(rgn,sizeof(*_tmp9F1));_tmp9F1->hd=({struct _tuple34*_tmp9F2=_region_malloc(rgn,sizeof(*_tmp9F2));_tmp9F2->f1=(*((struct _tuple32*)_tmp9ED->hd)).f1;_tmp9F2->f2=_tmp9DB;_tmp9F2;});_tmp9F1->tl=ans;_tmp9F1;});
break;}}
# 5110
if(_tmp9ED == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp9F5;_tmp9F5.tag=0;_tmp9F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp9E9);({void*_tmp9F3[1]={& _tmp9F5};Cyc_Tcutil_terr(loc,({const char*_tmp9F4="bad field designator %s";_tag_dyneither(_tmp9F4,sizeof(char),24);}),_tag_dyneither(_tmp9F3,sizeof(void*),1));});});
goto _LL7FD;}}_LL7FD:;}}}
# 5115
if(aggr_kind == Cyc_Absyn_StructA)
# 5117
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple32*)fields->hd)).f2){
({void*_tmp9F6=0;Cyc_Tcutil_terr(loc,({const char*_tmp9F7="too few arguments to struct";_tag_dyneither(_tmp9F7,sizeof(char),28);}),_tag_dyneither(_tmp9F6,sizeof(void*),0));});
break;}}else{
# 5124
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple32*)fields->hd)).f2){
if(found)({void*_tmp9F8=0;Cyc_Tcutil_terr(loc,({const char*_tmp9F9="only one member of a union is allowed";_tag_dyneither(_tmp9F9,sizeof(char),38);}),_tag_dyneither(_tmp9F8,sizeof(void*),0));});
found=1;}}
# 5131
if(!found)({void*_tmp9FA=0;Cyc_Tcutil_terr(loc,({const char*_tmp9FB="missing member for union";_tag_dyneither(_tmp9FB,sizeof(char),25);}),_tag_dyneither(_tmp9FA,sizeof(void*),0));});}{
# 5134
struct Cyc_List_List*_tmp9FC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0);return _tmp9FC;};};}
# 5066
;_pop_region(temp);}
# 5137
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmp9FF=Cyc_Tcutil_compress(t);void*_tmpA00=_tmp9FF;void*_tmpA01;union Cyc_Absyn_Constraint*_tmpA02;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA00)->tag == 5){_LL803: _tmpA01=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA00)->f1).elt_typ;_tmpA02=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA00)->f1).ptr_atts).bounds;_LL804: {
# 5140
void*_tmpA03=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpA02);void*_tmpA04=_tmpA03;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpA04)->tag == 0){_LL808: _LL809:
# 5142
*elt_typ_dest=_tmpA01;
return 1;}else{_LL80A: _LL80B:
 return 0;}_LL807:;}}else{_LL805: _LL806:
# 5146
 return 0;}_LL802:;}
# 5150
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmpA05=Cyc_Tcutil_compress(t);void*_tmpA06=_tmpA05;void*_tmpA07;union Cyc_Absyn_Constraint*_tmpA08;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA06)->tag == 5){_LL80D: _tmpA07=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA06)->f1).elt_typ;_tmpA08=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA06)->f1).ptr_atts).zero_term;_LL80E:
# 5153
*elt_typ_dest=_tmpA07;
return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpA08);}else{_LL80F: _LL810:
 return 0;}_LL80C:;}
# 5161
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type){
# 5163
void*_tmpA09=Cyc_Tcutil_compress(t);void*_tmpA0A=_tmpA09;void*_tmpA0B;struct Cyc_Absyn_Tqual _tmpA0C;struct Cyc_Absyn_Exp*_tmpA0D;union Cyc_Absyn_Constraint*_tmpA0E;void*_tmpA0F;union Cyc_Absyn_Constraint*_tmpA10;union Cyc_Absyn_Constraint*_tmpA11;switch(*((int*)_tmpA0A)){case 5: _LL812: _tmpA0F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA0A)->f1).elt_typ;_tmpA10=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA0A)->f1).ptr_atts).bounds;_tmpA11=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA0A)->f1).ptr_atts).zero_term;_LL813:
# 5165
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpA11)){
*ptr_type=t;
*elt_type=_tmpA0F;
{void*_tmpA12=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmpA10);void*_tmpA13=_tmpA12;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpA13)->tag == 0){_LL819: _LL81A:
*is_dyneither=1;goto _LL818;}else{_LL81B: _LL81C:
*is_dyneither=0;goto _LL818;}_LL818:;}
# 5172
return 1;}else{
return 0;}case 8: _LL814: _tmpA0B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA0A)->f1).elt_type;_tmpA0C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA0A)->f1).tq;_tmpA0D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA0A)->f1).num_elts;_tmpA0E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA0A)->f1).zero_term;_LL815:
# 5175
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpA0E)){
*elt_type=_tmpA0B;
*is_dyneither=0;
*ptr_type=Cyc_Tcutil_promote_array(t,(void*)& Cyc_Absyn_HeapRgn_val,0);
return 1;}else{
return 0;}default: _LL816: _LL817:
 return 0;}_LL811:;}
# 5188
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){
# 5190
void*_tmpA14=e1->r;void*_tmpA15=_tmpA14;struct Cyc_Absyn_Exp*_tmpA16;struct Cyc_Absyn_Exp*_tmpA17;struct Cyc_Absyn_Exp*_tmpA18;struct Cyc_Absyn_Exp*_tmpA19;struct Cyc_Absyn_Exp*_tmpA1A;struct Cyc_Absyn_Exp*_tmpA1B;switch(*((int*)_tmpA15)){case 13: _LL81E: _LL81F:
# 5192
({struct Cyc_String_pa_PrintArg_struct _tmpA1E;_tmpA1E.tag=0;_tmpA1E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));({void*_tmpA1C[1]={& _tmpA1E};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA1D="we have a cast in a lhs:  %s";_tag_dyneither(_tmpA1D,sizeof(char),29);}),_tag_dyneither(_tmpA1C,sizeof(void*),1));});});case 19: _LL820: _tmpA1B=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpA15)->f1;_LL821:
 _tmpA1A=_tmpA1B;goto _LL823;case 22: _LL822: _tmpA1A=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA15)->f1;_LL823:
# 5195
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpA1A->topt),ptr_type,is_dyneither,elt_type);case 21: _LL824: _tmpA19=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA15)->f1;_LL825:
 _tmpA18=_tmpA19;goto _LL827;case 20: _LL826: _tmpA18=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA15)->f1;_LL827:
# 5199
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpA18->topt),ptr_type,is_dyneither,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmpA21;_tmpA21.tag=0;_tmpA21.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1));({void*_tmpA1F[1]={& _tmpA21};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA20="found zero pointer aggregate member assignment: %s";_tag_dyneither(_tmpA20,sizeof(char),51);}),_tag_dyneither(_tmpA1F,sizeof(void*),1));});});
return 0;case 12: _LL828: _tmpA17=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpA15)->f1;_LL829:
 _tmpA16=_tmpA17;goto _LL82B;case 11: _LL82A: _tmpA16=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpA15)->f1;_LL82B:
# 5205
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpA16->topt),ptr_type,is_dyneither,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmpA24;_tmpA24.tag=0;_tmpA24.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1));({void*_tmpA22[1]={& _tmpA24};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA23="found zero pointer instantiate/noinstantiate: %s";_tag_dyneither(_tmpA23,sizeof(char),49);}),_tag_dyneither(_tmpA22,sizeof(void*),1));});});
return 0;case 1: _LL82C: _LL82D:
 return 0;default: _LL82E: _LL82F:
({struct Cyc_String_pa_PrintArg_struct _tmpA27;_tmpA27.tag=0;_tmpA27.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));({void*_tmpA25[1]={& _tmpA27};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA26="found bad lhs in is_zero_ptr_deref: %s";_tag_dyneither(_tmpA26,sizeof(char),39);}),_tag_dyneither(_tmpA25,sizeof(void*),1));});});}_LL81D:;}
# 5214
int Cyc_Tcutil_is_tagged_pointer_typ(void*t){
void*ignore=(void*)& Cyc_Absyn_VoidType_val;
return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
# 5225
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
# 5228
void*_tmpA28=Cyc_Tcutil_compress(r);void*_tmpA29=_tmpA28;struct Cyc_Absyn_Tvar*_tmpA2A;enum Cyc_Absyn_KindQual _tmpA2B;enum Cyc_Absyn_AliasQual _tmpA2C;switch(*((int*)_tmpA29)){case 22: _LL831: _LL832:
 return !must_be_unique;case 21: _LL833: _LL834:
 return 1;case 17: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpA29)->f3 != 0){if(((struct Cyc_Absyn_Typedefdecl*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpA29)->f3)->kind != 0){if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpA29)->f4 == 0){_LL835: _tmpA2B=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpA29)->f3)->kind)->v)->kind;_tmpA2C=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpA29)->f3)->kind)->v)->aliasqual;_LL836:
# 5232
 return _tmpA2B == Cyc_Absyn_RgnKind  && (_tmpA2C == Cyc_Absyn_Unique  || _tmpA2C == Cyc_Absyn_Top  && !must_be_unique);}else{goto _LL839;}}else{goto _LL839;}}else{goto _LL839;}case 2: _LL837: _tmpA2A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA29)->f1;_LL838: {
# 5236
struct Cyc_Absyn_Kind*_tmpA2D=Cyc_Tcutil_tvar_kind(_tmpA2A,& Cyc_Tcutil_rk);struct Cyc_Absyn_Kind*_tmpA2E=_tmpA2D;enum Cyc_Absyn_KindQual _tmpA2F;enum Cyc_Absyn_AliasQual _tmpA30;_LL83C: _tmpA2F=_tmpA2E->kind;_tmpA30=_tmpA2E->aliasqual;_LL83D:;
if(_tmpA2F == Cyc_Absyn_RgnKind  && (_tmpA30 == Cyc_Absyn_Unique  || _tmpA30 == Cyc_Absyn_Top  && !must_be_unique)){
void*_tmpA31=Cyc_Absyn_compress_kb(_tmpA2A->kind);void*_tmpA32=_tmpA31;struct Cyc_Core_Opt**_tmpA33;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA32)->tag == 2){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA32)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA32)->f2)->aliasqual == Cyc_Absyn_Top){_LL83F: _tmpA33=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA32)->f1;_LL840:
# 5240
*_tmpA33=({struct Cyc_Core_Opt*_tmpA34=_cycalloc(sizeof(*_tmpA34));_tmpA34->v=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA35=_cycalloc(sizeof(*_tmpA35));_tmpA35[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpA36;_tmpA36.tag=2;_tmpA36.f1=0;_tmpA36.f2=& Cyc_Tcutil_rk;_tmpA36;});_tmpA35;});_tmpA34;});
return 0;}else{goto _LL841;}}else{goto _LL841;}}else{_LL841: _LL842:
 return 1;}_LL83E:;}
# 5245
return 0;}default: _LL839: _LL83A:
 return 0;}_LL830:;}
# 5252
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmpA37=Cyc_Tcutil_compress(t);void*_tmpA38=_tmpA37;struct Cyc_Absyn_Tvar*_tmpA39;void*_tmpA3A;switch(*((int*)_tmpA38)){case 5: _LL844: _tmpA3A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA38)->f1).ptr_atts).rgn;_LL845:
# 5255
 return Cyc_Tcutil_is_noalias_region(_tmpA3A,must_be_unique);case 2: _LL846: _tmpA39=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA38)->f1;_LL847: {
# 5257
struct Cyc_Absyn_Kind*_tmpA3B=Cyc_Tcutil_tvar_kind(_tmpA39,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmpA3C=_tmpA3B;enum Cyc_Absyn_KindQual _tmpA3D;enum Cyc_Absyn_AliasQual _tmpA3E;_LL84B: _tmpA3D=_tmpA3C->kind;_tmpA3E=_tmpA3C->aliasqual;_LL84C:;{
enum Cyc_Absyn_KindQual _tmpA3F=_tmpA3D;switch(_tmpA3F){case Cyc_Absyn_BoxKind: _LL84E: _LL84F:
 goto _LL851;case Cyc_Absyn_AnyKind: _LL850: _LL851: goto _LL853;case Cyc_Absyn_MemKind: _LL852: _LL853:
 if(_tmpA3E == Cyc_Absyn_Unique  || _tmpA3E == Cyc_Absyn_Top  && !must_be_unique){
void*_tmpA40=Cyc_Absyn_compress_kb(_tmpA39->kind);void*_tmpA41=_tmpA40;struct Cyc_Core_Opt**_tmpA42;enum Cyc_Absyn_KindQual _tmpA43;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA41)->tag == 2){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA41)->f2)->aliasqual == Cyc_Absyn_Top){_LL857: _tmpA42=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA41)->f1;_tmpA43=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA41)->f2)->kind;_LL858:
# 5263
*_tmpA42=({struct Cyc_Core_Opt*_tmpA44=_cycalloc(sizeof(*_tmpA44));_tmpA44->v=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA45=_cycalloc(sizeof(*_tmpA45));_tmpA45[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpA46;_tmpA46.tag=2;_tmpA46.f1=0;_tmpA46.f2=({struct Cyc_Absyn_Kind*_tmpA47=_cycalloc_atomic(sizeof(*_tmpA47));_tmpA47->kind=_tmpA43;_tmpA47->aliasqual=Cyc_Absyn_Aliasable;_tmpA47;});_tmpA46;});_tmpA45;});_tmpA44;});
return 0;}else{goto _LL859;}}else{_LL859: _LL85A:
# 5267
 return 1;}_LL856:;}
# 5270
return 0;default: _LL854: _LL855:
 return 0;}_LL84D:;};}default: _LL848: _LL849:
# 5273
 return 0;}_LL843:;}
# 5276
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*rgn,void*t){
void*_tmpA48=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(_tmpA48,0))return 1;{
void*_tmpA49=_tmpA48;union Cyc_Absyn_DatatypeFieldInfoU _tmpA4A;struct Cyc_List_List*_tmpA4B;union Cyc_Absyn_DatatypeInfoU _tmpA4C;struct Cyc_List_List*_tmpA4D;struct Cyc_List_List*_tmpA4E;struct Cyc_Absyn_Aggrdecl**_tmpA4F;struct Cyc_List_List*_tmpA50;struct Cyc_List_List*_tmpA51;switch(*((int*)_tmpA49)){case 10: _LL85C: _tmpA51=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA49)->f1;_LL85D:
# 5281
 while(_tmpA51 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(*((struct _tuple12*)_tmpA51->hd)).f2))return 1;
_tmpA51=_tmpA51->tl;}
# 5285
return 0;case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA49)->f1).aggr_info).KnownAggr).tag == 2){_LL85E: _tmpA4F=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA49)->f1).aggr_info).KnownAggr).val;_tmpA50=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA49)->f1).targs;_LL85F:
# 5287
 if((*_tmpA4F)->impl == 0)return 0;else{
# 5289
struct Cyc_List_List*_tmpA52=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,(*_tmpA4F)->tvs,_tmpA50);
struct Cyc_List_List*_tmpA53=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpA4F)->impl))->fields;
void*t;
while(_tmpA53 != 0){
t=_tmpA52 == 0?((struct Cyc_Absyn_Aggrfield*)_tmpA53->hd)->type: Cyc_Tcutil_rsubstitute(rgn,_tmpA52,((struct Cyc_Absyn_Aggrfield*)_tmpA53->hd)->type);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpA53=_tmpA53->tl;}
# 5297
return 0;}}else{_LL862: _LL863:
# 5307
 return 0;}case 12: _LL860: _tmpA4E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA49)->f2;_LL861:
# 5300
 while(_tmpA4E != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,((struct Cyc_Absyn_Aggrfield*)_tmpA4E->hd)->type))return 1;
_tmpA4E=_tmpA4E->tl;}
# 5304
return 0;case 3: _LL864: _tmpA4C=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA49)->f1).datatype_info;_tmpA4D=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA49)->f1).targs;_LL865: {
# 5309
union Cyc_Absyn_DatatypeInfoU _tmpA54=_tmpA4C;struct Cyc_List_List*_tmpA55;struct Cyc_Core_Opt*_tmpA56;struct _tuple2*_tmpA57;int _tmpA58;if((_tmpA54.UnknownDatatype).tag == 1){_LL86B: _tmpA57=((_tmpA54.UnknownDatatype).val).name;_tmpA58=((_tmpA54.UnknownDatatype).val).is_extensible;_LL86C:
# 5312
 return 0;}else{_LL86D: _tmpA55=(*(_tmpA54.KnownDatatype).val)->tvs;_tmpA56=(*(_tmpA54.KnownDatatype).val)->fields;_LL86E:
# 5315
 return 0;}_LL86A:;}case 4: _LL866: _tmpA4A=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpA49)->f1).field_info;_tmpA4B=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpA49)->f1).targs;_LL867: {
# 5318
union Cyc_Absyn_DatatypeFieldInfoU _tmpA59=_tmpA4A;struct Cyc_Absyn_Datatypedecl*_tmpA5A;struct Cyc_Absyn_Datatypefield*_tmpA5B;if((_tmpA59.UnknownDatatypefield).tag == 1){_LL870: _LL871:
# 5321
 return 0;}else{_LL872: _tmpA5A=((_tmpA59.KnownDatatypefield).val).f1;_tmpA5B=((_tmpA59.KnownDatatypefield).val).f2;_LL873: {
# 5323
struct Cyc_List_List*_tmpA5C=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpA5A->tvs,_tmpA4B);
struct Cyc_List_List*_tmpA5D=_tmpA5B->typs;
while(_tmpA5D != 0){
_tmpA48=_tmpA5C == 0?(*((struct _tuple12*)_tmpA5D->hd)).f2: Cyc_Tcutil_rsubstitute(rgn,_tmpA5C,(*((struct _tuple12*)_tmpA5D->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,_tmpA48))return 1;
_tmpA5D=_tmpA5D->tl;}
# 5330
return 0;}}_LL86F:;}default: _LL868: _LL869:
# 5332
 return 0;}_LL85B:;};}
# 5336
static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,struct _dyneither_ptr*f){
void*_tmpA5E=Cyc_Tcutil_compress(t);void*_tmpA5F=_tmpA5E;struct Cyc_List_List*_tmpA60;struct Cyc_Absyn_Aggrdecl*_tmpA61;struct Cyc_List_List*_tmpA62;switch(*((int*)_tmpA5F)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA5F)->f1).aggr_info).KnownAggr).tag == 2){_LL875: _tmpA61=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA5F)->f1).aggr_info).KnownAggr).val;_tmpA62=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA5F)->f1).targs;_LL876:
# 5339
 _tmpA60=_tmpA61->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpA61->impl))->fields;goto _LL878;}else{goto _LL879;}case 12: _LL877: _tmpA60=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA5F)->f2;_LL878: {
# 5341
struct Cyc_Absyn_Aggrfield*_tmpA63=Cyc_Absyn_lookup_field(_tmpA60,f);
{struct Cyc_Absyn_Aggrfield*_tmpA64=_tmpA63;void*_tmpA65;if(_tmpA64 == 0){_LL87C: _LL87D:
({void*_tmpA66=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA67="is_noalias_field: missing field";_tag_dyneither(_tmpA67,sizeof(char),32);}),_tag_dyneither(_tmpA66,sizeof(void*),0));});}else{_LL87E: _tmpA65=_tmpA64->type;_LL87F:
# 5345
 return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpA65);}_LL87B:;}
# 5347
return 0;}default: _LL879: _LL87A:
# 5349
({struct Cyc_String_pa_PrintArg_struct _tmpA6A;_tmpA6A.tag=0;_tmpA6A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));({void*_tmpA68[1]={& _tmpA6A};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA69="is_noalias_field: invalid type |%s|";_tag_dyneither(_tmpA69,sizeof(char),36);}),_tag_dyneither(_tmpA68,sizeof(void*),1));});});}_LL874:;}
# 5357
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
void*_tmpA6B=e->r;void*_tmpA6C=_tmpA6B;struct Cyc_Absyn_Stmt*_tmpA6D;struct Cyc_Absyn_Exp*_tmpA6E;struct Cyc_Absyn_Exp*_tmpA6F;struct Cyc_Absyn_Exp*_tmpA70;struct Cyc_Absyn_Exp*_tmpA71;struct Cyc_Absyn_Exp*_tmpA72;struct Cyc_Absyn_Exp*_tmpA73;struct Cyc_Absyn_Exp*_tmpA74;struct _dyneither_ptr*_tmpA75;struct Cyc_Absyn_Exp*_tmpA76;struct Cyc_Absyn_Exp*_tmpA77;switch(*((int*)_tmpA6C)){case 1: if(((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA6C)->f1)->tag == 1){_LL881: _LL882:
 return 0;}else{goto _LL893;}case 21: _LL883: _tmpA77=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA6C)->f1;_LL884:
 _tmpA76=_tmpA77;goto _LL886;case 19: _LL885: _tmpA76=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpA6C)->f1;_LL886:
# 5362
 return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmpA76->topt),1) && 
Cyc_Tcutil_is_noalias_path(r,_tmpA76);case 20: _LL887: _tmpA74=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA6C)->f1;_tmpA75=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA6C)->f2;_LL888:
 return Cyc_Tcutil_is_noalias_path(r,_tmpA74);case 22: _LL889: _tmpA72=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA6C)->f1;_tmpA73=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA6C)->f2;_LL88A: {
# 5366
void*_tmpA78=Cyc_Tcutil_compress((void*)_check_null(_tmpA72->topt));void*_tmpA79=_tmpA78;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA79)->tag == 10){_LL896: _LL897:
 return Cyc_Tcutil_is_noalias_path(r,_tmpA72);}else{_LL898: _LL899:
 return 0;}_LL895:;}case 5: _LL88B: _tmpA70=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA6C)->f2;_tmpA71=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA6C)->f3;_LL88C:
# 5371
 return Cyc_Tcutil_is_noalias_path(r,_tmpA70) && Cyc_Tcutil_is_noalias_path(r,_tmpA71);case 8: _LL88D: _tmpA6F=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpA6C)->f2;_LL88E:
 _tmpA6E=_tmpA6F;goto _LL890;case 13: _LL88F: _tmpA6E=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA6C)->f2;_LL890:
 return Cyc_Tcutil_is_noalias_path(r,_tmpA6E);case 36: _LL891: _tmpA6D=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpA6C)->f1;_LL892:
# 5375
 while(1){
void*_tmpA7A=_tmpA6D->r;void*_tmpA7B=_tmpA7A;struct Cyc_Absyn_Exp*_tmpA7C;struct Cyc_Absyn_Decl*_tmpA7D;struct Cyc_Absyn_Stmt*_tmpA7E;struct Cyc_Absyn_Stmt*_tmpA7F;struct Cyc_Absyn_Stmt*_tmpA80;switch(*((int*)_tmpA7B)){case 2: _LL89B: _tmpA7F=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpA7B)->f1;_tmpA80=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpA7B)->f2;_LL89C:
 _tmpA6D=_tmpA80;goto _LL89A;case 12: _LL89D: _tmpA7D=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpA7B)->f1;_tmpA7E=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpA7B)->f2;_LL89E:
 _tmpA6D=_tmpA7E;goto _LL89A;case 1: _LL89F: _tmpA7C=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpA7B)->f1;_LL8A0:
 return Cyc_Tcutil_is_noalias_path(r,_tmpA7C);default: _LL8A1: _LL8A2:
({void*_tmpA81=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA82="is_noalias_stmt_exp: ill-formed StmtExp";_tag_dyneither(_tmpA82,sizeof(char),40);}),_tag_dyneither(_tmpA81,sizeof(void*),0));});}_LL89A:;}default: _LL893: _LL894:
# 5383
 return 1;}_LL880:;}
# 5400 "tcutil.cyc"
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5402
struct _tuple18 bogus_ans=({struct _tuple18 _tmpACA;_tmpACA.f1=0;_tmpACA.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpACA;});
void*_tmpA83=e->r;void*_tmpA84=_tmpA83;struct Cyc_Absyn_Exp*_tmpA85;struct Cyc_Absyn_Exp*_tmpA86;struct Cyc_Absyn_Exp*_tmpA87;struct Cyc_Absyn_Exp*_tmpA88;struct _dyneither_ptr*_tmpA89;int _tmpA8A;struct Cyc_Absyn_Exp*_tmpA8B;struct _dyneither_ptr*_tmpA8C;int _tmpA8D;void*_tmpA8E;switch(*((int*)_tmpA84)){case 1: _LL8A4: _tmpA8E=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA84)->f1;_LL8A5: {
# 5406
void*_tmpA8F=_tmpA8E;struct Cyc_Absyn_Vardecl*_tmpA90;struct Cyc_Absyn_Vardecl*_tmpA91;struct Cyc_Absyn_Vardecl*_tmpA92;struct Cyc_Absyn_Vardecl*_tmpA93;switch(*((int*)_tmpA8F)){case 0: _LL8B1: _LL8B2:
 goto _LL8B4;case 2: _LL8B3: _LL8B4:
 return bogus_ans;case 1: _LL8B5: _tmpA93=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpA8F)->f1;_LL8B6: {
# 5410
void*_tmpA94=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpA95=_tmpA94;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA95)->tag == 8){_LL8BE: _LL8BF:
# 5412
 return({struct _tuple18 _tmpA96;_tmpA96.f1=1;_tmpA96.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpA96;});}else{_LL8C0: _LL8C1:
 return({struct _tuple18 _tmpA97;_tmpA97.f1=(_tmpA93->tq).real_const;_tmpA97.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpA97;});}_LL8BD:;}case 4: _LL8B7: _tmpA92=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpA8F)->f1;_LL8B8: {
# 5416
void*_tmpA98=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpA99=_tmpA98;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA99)->tag == 8){_LL8C3: _LL8C4:
 return({struct _tuple18 _tmpA9A;_tmpA9A.f1=1;_tmpA9A.f2=(void*)_check_null(_tmpA92->rgn);_tmpA9A;});}else{_LL8C5: _LL8C6:
# 5419
 _tmpA92->escapes=1;
return({struct _tuple18 _tmpA9B;_tmpA9B.f1=(_tmpA92->tq).real_const;_tmpA9B.f2=(void*)_check_null(_tmpA92->rgn);_tmpA9B;});}_LL8C2:;}case 5: _LL8B9: _tmpA91=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpA8F)->f1;_LL8BA:
# 5422
 _tmpA90=_tmpA91;goto _LL8BC;default: _LL8BB: _tmpA90=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpA8F)->f1;_LL8BC:
# 5424
 _tmpA90->escapes=1;
return({struct _tuple18 _tmpA9C;_tmpA9C.f1=(_tmpA90->tq).real_const;_tmpA9C.f2=(void*)_check_null(_tmpA90->rgn);_tmpA9C;});}_LL8B0:;}case 20: _LL8A6: _tmpA8B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA84)->f1;_tmpA8C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA84)->f2;_tmpA8D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA84)->f3;_LL8A7:
# 5429
 if(_tmpA8D)return bogus_ans;{
# 5432
void*_tmpA9D=Cyc_Tcutil_compress((void*)_check_null(_tmpA8B->topt));void*_tmpA9E=_tmpA9D;struct Cyc_Absyn_Aggrdecl*_tmpA9F;struct Cyc_List_List*_tmpAA0;switch(*((int*)_tmpA9E)){case 12: _LL8C8: _tmpAA0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA9E)->f2;_LL8C9: {
# 5434
struct Cyc_Absyn_Aggrfield*_tmpAA1=Cyc_Absyn_lookup_field(_tmpAA0,_tmpA8C);
if(_tmpAA1 != 0  && _tmpAA1->width == 0){
struct _tuple18 _tmpAA2=Cyc_Tcutil_addressof_props(te,_tmpA8B);struct _tuple18 _tmpAA3=_tmpAA2;int _tmpAA4;void*_tmpAA5;_LL8CF: _tmpAA4=_tmpAA3.f1;_tmpAA5=_tmpAA3.f2;_LL8D0:;
return({struct _tuple18 _tmpAA6;_tmpAA6.f1=(_tmpAA1->tq).real_const  || _tmpAA4;_tmpAA6.f2=_tmpAA5;_tmpAA6;});}
# 5439
return bogus_ans;}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA9E)->f1).aggr_info).KnownAggr).tag == 2){_LL8CA: _tmpA9F=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA9E)->f1).aggr_info).KnownAggr).val;_LL8CB: {
# 5441
struct Cyc_Absyn_Aggrfield*_tmpAA7=Cyc_Absyn_lookup_decl_field(_tmpA9F,_tmpA8C);
if(_tmpAA7 != 0  && _tmpAA7->width == 0){
struct _tuple18 _tmpAA8=Cyc_Tcutil_addressof_props(te,_tmpA8B);struct _tuple18 _tmpAA9=_tmpAA8;int _tmpAAA;void*_tmpAAB;_LL8D2: _tmpAAA=_tmpAA9.f1;_tmpAAB=_tmpAA9.f2;_LL8D3:;
return({struct _tuple18 _tmpAAC;_tmpAAC.f1=(_tmpAA7->tq).real_const  || _tmpAAA;_tmpAAC.f2=_tmpAAB;_tmpAAC;});}
# 5446
return bogus_ans;}}else{goto _LL8CC;}default: _LL8CC: _LL8CD:
 return bogus_ans;}_LL8C7:;};case 21: _LL8A8: _tmpA88=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA84)->f1;_tmpA89=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA84)->f2;_tmpA8A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA84)->f3;_LL8A9:
# 5451
 if(_tmpA8A)return bogus_ans;{
# 5455
void*_tmpAAD=Cyc_Tcutil_compress((void*)_check_null(_tmpA88->topt));void*_tmpAAE=_tmpAAD;void*_tmpAAF;void*_tmpAB0;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAAE)->tag == 5){_LL8D5: _tmpAAF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAAE)->f1).elt_typ;_tmpAB0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAAE)->f1).ptr_atts).rgn;_LL8D6: {
# 5457
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmpAB1=Cyc_Tcutil_compress(_tmpAAF);void*_tmpAB2=_tmpAB1;struct Cyc_Absyn_Aggrdecl*_tmpAB3;struct Cyc_List_List*_tmpAB4;switch(*((int*)_tmpAB2)){case 12: _LL8DA: _tmpAB4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpAB2)->f2;_LL8DB:
# 5460
 finfo=Cyc_Absyn_lookup_field(_tmpAB4,_tmpA89);goto _LL8D9;case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAB2)->f1).aggr_info).KnownAggr).tag == 2){_LL8DC: _tmpAB3=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAB2)->f1).aggr_info).KnownAggr).val;_LL8DD:
# 5462
 finfo=Cyc_Absyn_lookup_decl_field(_tmpAB3,_tmpA89);goto _LL8D9;}else{goto _LL8DE;}default: _LL8DE: _LL8DF:
 return bogus_ans;}_LL8D9:;}
# 5465
if(finfo != 0  && finfo->width == 0)
return({struct _tuple18 _tmpAB5;_tmpAB5.f1=(finfo->tq).real_const;_tmpAB5.f2=_tmpAB0;_tmpAB5;});
return bogus_ans;}}else{_LL8D7: _LL8D8:
 return bogus_ans;}_LL8D4:;};case 19: _LL8AA: _tmpA87=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpA84)->f1;_LL8AB: {
# 5472
void*_tmpAB6=Cyc_Tcutil_compress((void*)_check_null(_tmpA87->topt));void*_tmpAB7=_tmpAB6;struct Cyc_Absyn_Tqual _tmpAB8;void*_tmpAB9;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAB7)->tag == 5){_LL8E1: _tmpAB8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAB7)->f1).elt_tq;_tmpAB9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAB7)->f1).ptr_atts).rgn;_LL8E2:
# 5474
 return({struct _tuple18 _tmpABA;_tmpABA.f1=_tmpAB8.real_const;_tmpABA.f2=_tmpAB9;_tmpABA;});}else{_LL8E3: _LL8E4:
 return bogus_ans;}_LL8E0:;}case 22: _LL8AC: _tmpA85=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA84)->f1;_tmpA86=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA84)->f2;_LL8AD: {
# 5480
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmpA85->topt));
void*_tmpABB=t;struct Cyc_Absyn_Tqual _tmpABC;struct Cyc_Absyn_Tqual _tmpABD;void*_tmpABE;struct Cyc_List_List*_tmpABF;switch(*((int*)_tmpABB)){case 10: _LL8E6: _tmpABF=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpABB)->f1;_LL8E7: {
# 5484
struct _tuple14 _tmpAC0=Cyc_Evexp_eval_const_uint_exp(_tmpA86);struct _tuple14 _tmpAC1=_tmpAC0;unsigned int _tmpAC2;int _tmpAC3;_LL8EF: _tmpAC2=_tmpAC1.f1;_tmpAC3=_tmpAC1.f2;_LL8F0:;
if(!_tmpAC3)
return bogus_ans;{
struct _tuple12*_tmpAC4=Cyc_Absyn_lookup_tuple_field(_tmpABF,(int)_tmpAC2);
if(_tmpAC4 != 0)
return({struct _tuple18 _tmpAC5;_tmpAC5.f1=((*_tmpAC4).f1).real_const;_tmpAC5.f2=(Cyc_Tcutil_addressof_props(te,_tmpA85)).f2;_tmpAC5;});
return bogus_ans;};}case 5: _LL8E8: _tmpABD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpABB)->f1).elt_tq;_tmpABE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpABB)->f1).ptr_atts).rgn;_LL8E9:
# 5492
 return({struct _tuple18 _tmpAC6;_tmpAC6.f1=_tmpABD.real_const;_tmpAC6.f2=_tmpABE;_tmpAC6;});case 8: _LL8EA: _tmpABC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpABB)->f1).tq;_LL8EB:
# 5498
 return({struct _tuple18 _tmpAC7;_tmpAC7.f1=_tmpABC.real_const;_tmpAC7.f2=(Cyc_Tcutil_addressof_props(te,_tmpA85)).f2;_tmpAC7;});default: _LL8EC: _LL8ED:
 return bogus_ans;}_LL8E5:;}default: _LL8AE: _LL8AF:
# 5502
({void*_tmpAC8=0;Cyc_Tcutil_terr(e->loc,({const char*_tmpAC9="unary & applied to non-lvalue";_tag_dyneither(_tmpAC9,sizeof(char),30);}),_tag_dyneither(_tmpAC8,sizeof(void*),0));});
return bogus_ans;}_LL8A3:;}
# 5509
void Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,union Cyc_Absyn_Constraint*b){
b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(b);{
void*_tmpACB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);void*_tmpACC=_tmpACB;struct Cyc_Absyn_Exp*_tmpACD;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpACC)->tag == 0){_LL8F2: _LL8F3:
 return;}else{_LL8F4: _tmpACD=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpACC)->f1;_LL8F5: {
# 5514
struct _tuple14 _tmpACE=Cyc_Evexp_eval_const_uint_exp(_tmpACD);struct _tuple14 _tmpACF=_tmpACE;unsigned int _tmpAD0;int _tmpAD1;_LL8F7: _tmpAD0=_tmpACF.f1;_tmpAD1=_tmpACF.f2;_LL8F8:;
if(_tmpAD1  && _tmpAD0 <= i)
({struct Cyc_Int_pa_PrintArg_struct _tmpAD5;_tmpAD5.tag=1;_tmpAD5.f1=(unsigned long)((int)i);({struct Cyc_Int_pa_PrintArg_struct _tmpAD4;_tmpAD4.tag=1;_tmpAD4.f1=(unsigned long)((int)_tmpAD0);({void*_tmpAD2[2]={& _tmpAD4,& _tmpAD5};Cyc_Tcutil_terr(loc,({const char*_tmpAD3="dereference is out of bounds: %d <= %d";_tag_dyneither(_tmpAD3,sizeof(char),39);}),_tag_dyneither(_tmpAD2,sizeof(void*),2));});});});
return;}}_LL8F1:;};}
# 5521
void Cyc_Tcutil_check_nonzero_bound(unsigned int loc,union Cyc_Absyn_Constraint*b){
Cyc_Tcutil_check_bound(loc,0,b);}
# 5525
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b){
void*_tmpAD6=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);void*_tmpAD7=_tmpAD6;struct Cyc_Absyn_Exp*_tmpAD8;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpAD7)->tag == 1){_LL8FA: _tmpAD8=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpAD7)->f1;_LL8FB: {
# 5528
struct _tuple14 _tmpAD9=Cyc_Evexp_eval_const_uint_exp(_tmpAD8);struct _tuple14 _tmpADA=_tmpAD9;unsigned int _tmpADB;int _tmpADC;_LL8FF: _tmpADB=_tmpADA.f1;_tmpADC=_tmpADA.f2;_LL900:;
return _tmpADC  && _tmpADB == 1;}}else{_LL8FC: _LL8FD:
 return 0;}_LL8F9:;}
# 5534
int Cyc_Tcutil_bits_only(void*t){
void*_tmpADD=Cyc_Tcutil_compress(t);void*_tmpADE=_tmpADD;struct Cyc_List_List*_tmpADF;struct Cyc_Absyn_Aggrdecl*_tmpAE0;struct Cyc_List_List*_tmpAE1;struct Cyc_List_List*_tmpAE2;void*_tmpAE3;union Cyc_Absyn_Constraint*_tmpAE4;switch(*((int*)_tmpADE)){case 0: _LL902: _LL903:
 goto _LL905;case 6: _LL904: _LL905:
 goto _LL907;case 7: _LL906: _LL907:
 return 1;case 13: _LL908: _LL909:
 goto _LL90B;case 14: _LL90A: _LL90B:
 return 0;case 8: _LL90C: _tmpAE3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpADE)->f1).elt_type;_tmpAE4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpADE)->f1).zero_term;_LL90D:
# 5544
 return !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpAE4) && Cyc_Tcutil_bits_only(_tmpAE3);case 10: _LL90E: _tmpAE2=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpADE)->f1;_LL90F:
# 5546
 for(0;_tmpAE2 != 0;_tmpAE2=_tmpAE2->tl){
if(!Cyc_Tcutil_bits_only((*((struct _tuple12*)_tmpAE2->hd)).f2))return 0;}
return 1;case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpADE)->f1).aggr_info).UnknownAggr).tag == 1){_LL910: _LL911:
 return 0;}else{_LL912: _tmpAE0=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpADE)->f1).aggr_info).KnownAggr).val;_tmpAE1=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpADE)->f1).targs;_LL913:
# 5551
 if(_tmpAE0->impl == 0)
return 0;{
int okay=1;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAE0->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
if(okay)return 1;{
struct _RegionHandle _tmpAE5=_new_region("rgn");struct _RegionHandle*rgn=& _tmpAE5;_push_region(rgn);
{struct Cyc_List_List*_tmpAE6=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpAE0->tvs,_tmpAE1);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAE0->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpAE6,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpAE7=0;_npop_handler(0);return _tmpAE7;}}}{
int _tmpAE8=1;_npop_handler(0);return _tmpAE8;};}
# 5558
;_pop_region(rgn);};};}case 12: _LL914: _tmpADF=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpADE)->f2;_LL915:
# 5563
 for(0;_tmpADF != 0;_tmpADF=_tmpADF->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)_tmpADF->hd)->type))return 0;}
return 1;default: _LL916: _LL917:
 return 0;}_LL901:;}
# 5571
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e){
while(1){
void*_tmpAE9=e->r;void*_tmpAEA=_tmpAE9;struct Cyc_Absyn_Exp*_tmpAEB;struct Cyc_Absyn_Exp*_tmpAEC;switch(*((int*)_tmpAEA)){case 1: _LL919: _LL91A:
 return 1;case 11: _LL91B: _tmpAEC=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpAEA)->f1;_LL91C:
 _tmpAEB=_tmpAEC;goto _LL91E;case 12: _LL91D: _tmpAEB=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpAEA)->f1;_LL91E:
 e=_tmpAEB;continue;default: _LL91F: _LL920:
# 5578
 return 0;}_LL918:;}}
# 5588
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmpAED=e->r;void*_tmpAEE=_tmpAED;struct Cyc_List_List*_tmpAEF;struct Cyc_List_List*_tmpAF0;enum Cyc_Absyn_Primop _tmpAF1;struct Cyc_List_List*_tmpAF2;struct Cyc_List_List*_tmpAF3;struct Cyc_List_List*_tmpAF4;struct Cyc_List_List*_tmpAF5;struct Cyc_Absyn_Exp*_tmpAF6;struct Cyc_Absyn_Exp*_tmpAF7;struct Cyc_Absyn_Exp*_tmpAF8;struct Cyc_Absyn_Exp*_tmpAF9;void*_tmpAFA;struct Cyc_Absyn_Exp*_tmpAFB;void*_tmpAFC;struct Cyc_Absyn_Exp*_tmpAFD;struct Cyc_Absyn_Exp*_tmpAFE;struct Cyc_Absyn_Exp*_tmpAFF;struct Cyc_Absyn_Exp*_tmpB00;struct Cyc_Absyn_Exp*_tmpB01;struct Cyc_Absyn_Exp*_tmpB02;struct Cyc_Absyn_Exp*_tmpB03;struct Cyc_Absyn_Exp*_tmpB04;void*_tmpB05;switch(*((int*)_tmpAEE)){case 0: _LL922: _LL923:
 goto _LL925;case 16: _LL924: _LL925:
 goto _LL927;case 17: _LL926: _LL927:
 goto _LL929;case 18: _LL928: _LL929:
 goto _LL92B;case 31: _LL92A: _LL92B:
 goto _LL92D;case 32: _LL92C: _LL92D:
 return 1;case 1: _LL92E: _tmpB05=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpAEE)->f1;_LL92F: {
# 5599
void*_tmpB06=_tmpB05;struct Cyc_Absyn_Vardecl*_tmpB07;struct Cyc_Absyn_Vardecl*_tmpB08;switch(*((int*)_tmpB06)){case 2: _LL951: _LL952:
 return 1;case 1: _LL953: _tmpB08=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpB06)->f1;_LL954: {
# 5602
void*_tmpB09=Cyc_Tcutil_compress(_tmpB08->type);void*_tmpB0A=_tmpB09;switch(*((int*)_tmpB0A)){case 8: _LL95C: _LL95D:
 goto _LL95F;case 9: _LL95E: _LL95F:
 return 1;default: _LL960: _LL961:
 return var_okay;}_LL95B:;}case 4: _LL955: _tmpB07=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpB06)->f1;_LL956:
# 5609
 if(_tmpB07->sc == Cyc_Absyn_Static){
void*_tmpB0B=Cyc_Tcutil_compress(_tmpB07->type);void*_tmpB0C=_tmpB0B;switch(*((int*)_tmpB0C)){case 8: _LL963: _LL964:
 goto _LL966;case 9: _LL965: _LL966:
 return 1;default: _LL967: _LL968:
 return var_okay;}_LL962:;}else{
# 5616
return var_okay;}case 0: _LL957: _LL958:
 return 0;default: _LL959: _LL95A:
 return var_okay;}_LL950:;}case 5: _LL930: _tmpB02=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAEE)->f1;_tmpB03=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAEE)->f2;_tmpB04=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAEE)->f3;_LL931:
# 5622
 return(Cyc_Tcutil_cnst_exp(0,_tmpB02) && 
Cyc_Tcutil_cnst_exp(0,_tmpB03)) && 
Cyc_Tcutil_cnst_exp(0,_tmpB04);case 8: _LL932: _tmpB00=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpAEE)->f1;_tmpB01=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpAEE)->f2;_LL933:
# 5626
 return Cyc_Tcutil_cnst_exp(0,_tmpB00) && Cyc_Tcutil_cnst_exp(0,_tmpB01);case 11: _LL934: _tmpAFF=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpAEE)->f1;_LL935:
 _tmpAFE=_tmpAFF;goto _LL937;case 12: _LL936: _tmpAFE=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpAEE)->f1;_LL937:
# 5629
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpAFE);case 13: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAEE)->f4 == Cyc_Absyn_No_coercion){_LL938: _tmpAFC=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAEE)->f1;_tmpAFD=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAEE)->f2;_LL939:
# 5631
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpAFD);}else{_LL93A: _tmpAFA=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAEE)->f1;_tmpAFB=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAEE)->f2;_LL93B:
# 5634
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpAFB);}case 14: _LL93C: _tmpAF9=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpAEE)->f1;_LL93D:
# 5636
 return Cyc_Tcutil_cnst_exp(1,_tmpAF9);case 26: _LL93E: _tmpAF7=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpAEE)->f2;_tmpAF8=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpAEE)->f3;_LL93F:
# 5638
 return Cyc_Tcutil_cnst_exp(0,_tmpAF7) && Cyc_Tcutil_cnst_exp(0,_tmpAF8);case 27: _LL940: _tmpAF6=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpAEE)->f1;_LL941:
# 5640
 return Cyc_Tcutil_cnst_exp(0,_tmpAF6);case 25: _LL942: _tmpAF5=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpAEE)->f1;_LL943:
 _tmpAF4=_tmpAF5;goto _LL945;case 29: _LL944: _tmpAF4=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpAEE)->f2;_LL945:
 _tmpAF3=_tmpAF4;goto _LL947;case 28: _LL946: _tmpAF3=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpAEE)->f3;_LL947:
# 5644
 for(0;_tmpAF3 != 0;_tmpAF3=_tmpAF3->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple19*)_tmpAF3->hd)).f2))
return 0;}
return 1;case 2: _LL948: _tmpAF1=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpAEE)->f1;_tmpAF2=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpAEE)->f2;_LL949:
# 5649
 _tmpAF0=_tmpAF2;goto _LL94B;case 23: _LL94A: _tmpAF0=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpAEE)->f1;_LL94B:
 _tmpAEF=_tmpAF0;goto _LL94D;case 30: _LL94C: _tmpAEF=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpAEE)->f1;_LL94D:
# 5652
 for(0;_tmpAEF != 0;_tmpAEF=_tmpAEF->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmpAEF->hd))
return 0;}
return 1;default: _LL94E: _LL94F:
 return 0;}_LL921:;}
# 5660
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 5664
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);
# 5666
int Cyc_Tcutil_supports_default(void*t){
void*_tmpB0D=Cyc_Tcutil_compress(t);void*_tmpB0E=_tmpB0D;struct Cyc_List_List*_tmpB0F;union Cyc_Absyn_AggrInfoU _tmpB10;struct Cyc_List_List*_tmpB11;struct Cyc_List_List*_tmpB12;void*_tmpB13;union Cyc_Absyn_Constraint*_tmpB14;union Cyc_Absyn_Constraint*_tmpB15;switch(*((int*)_tmpB0E)){case 0: _LL96A: _LL96B:
 goto _LL96D;case 6: _LL96C: _LL96D:
 goto _LL96F;case 7: _LL96E: _LL96F:
 return 1;case 5: _LL970: _tmpB14=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB0E)->f1).ptr_atts).nullable;_tmpB15=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB0E)->f1).ptr_atts).bounds;_LL971: {
# 5673
void*_tmpB16=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpB15);void*_tmpB17=_tmpB16;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpB17)->tag == 0){_LL981: _LL982:
 return 1;}else{_LL983: _LL984:
# 5676
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmpB14);}_LL980:;}case 8: _LL972: _tmpB13=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB0E)->f1).elt_type;_LL973:
# 5679
 return Cyc_Tcutil_supports_default(_tmpB13);case 10: _LL974: _tmpB12=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpB0E)->f1;_LL975:
# 5681
 for(0;_tmpB12 != 0;_tmpB12=_tmpB12->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple12*)_tmpB12->hd)).f2))return 0;}
return 1;case 11: _LL976: _tmpB10=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpB0E)->f1).aggr_info;_tmpB11=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpB0E)->f1).targs;_LL977: {
# 5685
struct Cyc_Absyn_Aggrdecl*_tmpB18=Cyc_Absyn_get_known_aggrdecl(_tmpB10);
if(_tmpB18->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB18->impl))->exist_vars != 0)return 0;
return Cyc_Tcutil_fields_support_default(_tmpB18->tvs,_tmpB11,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB18->impl))->fields);}case 12: _LL978: _tmpB0F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpB0E)->f2;_LL979:
 return Cyc_Tcutil_fields_support_default(0,0,_tmpB0F);case 14: _LL97A: _LL97B:
# 5691
 goto _LL97D;case 13: _LL97C: _LL97D:
 return 1;default: _LL97E: _LL97F:
# 5694
 return 0;}_LL969:;}
# 5699
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t){
void*_tmpB19=t;struct Cyc_Absyn_Typedefdecl*_tmpB1A;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpB19)->tag == 17){_LL986: _tmpB1A=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpB19)->f3;_LL987:
# 5702
 if(_tmpB1A != 0){
struct Cyc_Absyn_Tqual _tmpB1B=_tmpB1A->tq;
if(((_tmpB1B.print_const  || _tmpB1B.q_volatile) || _tmpB1B.q_restrict) || _tmpB1B.real_const)
# 5707
({struct Cyc_String_pa_PrintArg_struct _tmpB1E;_tmpB1E.tag=0;_tmpB1E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmpB1C[1]={& _tmpB1E};Cyc_Tcutil_warn(loc,({const char*_tmpB1D="qualifier within typedef type %s is ignored";_tag_dyneither(_tmpB1D,sizeof(char),44);}),_tag_dyneither(_tmpB1C,sizeof(void*),1));});});}
# 5710
goto _LL985;}else{_LL988: _LL989:
 goto _LL985;}_LL985:;}
# 5715
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 5717
struct _RegionHandle _tmpB1F=_new_region("rgn");struct _RegionHandle*rgn=& _tmpB1F;_push_region(rgn);
{struct Cyc_List_List*_tmpB20=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_supports_default(t)){int _tmpB21=1;_npop_handler(0);return _tmpB21;}
t=Cyc_Tcutil_rsubstitute(rgn,_tmpB20,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpB22=0;_npop_handler(0);return _tmpB22;}}{
# 5725
int _tmpB23=1;_npop_handler(0);return _tmpB23;};}
# 5718
;_pop_region(rgn);}
# 5731
int Cyc_Tcutil_admits_zero(void*t){
void*_tmpB24=Cyc_Tcutil_compress(t);void*_tmpB25=_tmpB24;union Cyc_Absyn_Constraint*_tmpB26;union Cyc_Absyn_Constraint*_tmpB27;switch(*((int*)_tmpB25)){case 6: _LL98B: _LL98C:
 goto _LL98E;case 7: _LL98D: _LL98E:
 return 1;case 5: _LL98F: _tmpB26=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB25)->f1).ptr_atts).nullable;_tmpB27=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB25)->f1).ptr_atts).bounds;_LL990: {
# 5736
void*_tmpB28=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpB27);void*_tmpB29=_tmpB28;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpB29)->tag == 0){_LL994: _LL995:
# 5740
 return 0;}else{_LL996: _LL997:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpB26);}_LL993:;}default: _LL991: _LL992:
# 5743
 return 0;}_LL98A:;}
# 5747
int Cyc_Tcutil_is_noreturn(void*t){
{void*_tmpB2A=Cyc_Tcutil_compress(t);void*_tmpB2B=_tmpB2A;struct Cyc_List_List*_tmpB2C;void*_tmpB2D;switch(*((int*)_tmpB2B)){case 5: _LL999: _tmpB2D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB2B)->f1).elt_typ;_LL99A:
 return Cyc_Tcutil_is_noreturn(_tmpB2D);case 9: _LL99B: _tmpB2C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB2B)->f1).attributes;_LL99C:
# 5751
 for(0;_tmpB2C != 0;_tmpB2C=_tmpB2C->tl){
void*_tmpB2E=(void*)_tmpB2C->hd;void*_tmpB2F=_tmpB2E;if(((struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmpB2F)->tag == 4){_LL9A0: _LL9A1:
 return 1;}else{_LL9A2: _LL9A3:
 continue;}_LL99F:;}
# 5756
goto _LL998;default: _LL99D: _LL99E:
 goto _LL998;}_LL998:;}
# 5759
return 0;}
# 5764
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmpB30=Cyc_Tcutil_compress(t);void*_tmpB31=_tmpB30;struct Cyc_List_List**_tmpB32;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB31)->tag == 9){_LL9A5: _tmpB32=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB31)->f1).attributes;_LL9A6: {
# 5767
struct Cyc_List_List*_tmpB33=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Tcutil_attribute_cmp,*_tmpB32,(void*)atts->hd))
*_tmpB32=({struct Cyc_List_List*_tmpB34=_cycalloc(sizeof(*_tmpB34));_tmpB34->hd=(void*)atts->hd;_tmpB34->tl=*_tmpB32;_tmpB34;});}else{
# 5774
_tmpB33=({struct Cyc_List_List*_tmpB35=_cycalloc(sizeof(*_tmpB35));_tmpB35->hd=(void*)atts->hd;_tmpB35->tl=_tmpB33;_tmpB35;});}}
return _tmpB33;}}else{_LL9A7: _LL9A8:
({void*_tmpB36=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpB37="transfer_fn_type_atts";_tag_dyneither(_tmpB37,sizeof(char),22);}),_tag_dyneither(_tmpB36,sizeof(void*),0));});}_LL9A4:;}
# 5781
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
struct Cyc_Absyn_Exp*bound=0;
{void*_tmpB38=Cyc_Tcutil_compress(t);void*_tmpB39=_tmpB38;struct Cyc_Absyn_Exp*_tmpB3A;struct Cyc_Absyn_PtrInfo*_tmpB3B;switch(*((int*)_tmpB39)){case 5: _LL9AA: _tmpB3B=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB39)->f1;_LL9AB:
# 5785
{void*_tmpB3C=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)((_tmpB3B->ptr_atts).bounds);void*_tmpB3D=_tmpB3C;struct Cyc_Absyn_Exp*_tmpB3E;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpB3D)->tag == 1){_LL9B1: _tmpB3E=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpB3D)->f1;_LL9B2:
 bound=_tmpB3E;goto _LL9B0;}else{_LL9B3: _LL9B4:
 goto _LL9B0;}_LL9B0:;}
# 5789
goto _LL9A9;case 8: _LL9AC: _tmpB3A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB39)->f1).num_elts;_LL9AD:
# 5791
 bound=_tmpB3A;
goto _LL9A9;default: _LL9AE: _LL9AF:
 goto _LL9A9;}_LL9A9:;}
# 5795
return bound;}
# 5800
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
{void*_tmpB3F=b;struct Cyc_Absyn_Vardecl*_tmpB40;struct Cyc_Absyn_Vardecl*_tmpB41;struct Cyc_Absyn_Vardecl*_tmpB42;struct Cyc_Absyn_Vardecl*_tmpB43;switch(*((int*)_tmpB3F)){case 5: _LL9B6: _tmpB43=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpB3F)->f1;_LL9B7:
 _tmpB42=_tmpB43;goto _LL9B9;case 4: _LL9B8: _tmpB42=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpB3F)->f1;_LL9B9:
 _tmpB41=_tmpB42;goto _LL9BB;case 3: _LL9BA: _tmpB41=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpB3F)->f1;_LL9BB:
 _tmpB40=_tmpB41;goto _LL9BD;case 1: _LL9BC: _tmpB40=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpB3F)->f1;_LL9BD:
# 5806
 if(!_tmpB40->escapes)return _tmpB40;
goto _LL9B5;default: _LL9BE: _LL9BF:
 goto _LL9B5;}_LL9B5:;}
# 5810
return 0;}
# 5814
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmpB44=0;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0)_tmpB44=({struct Cyc_List_List*_tmpB45=_cycalloc(sizeof(*_tmpB45));_tmpB45->hd=*((void**)_check_null((void**)x->hd));_tmpB45->tl=_tmpB44;_tmpB45;});}}
return _tmpB44;}
# 5821
int Cyc_Tcutil_is_array(void*t){
# 5823
void*_tmpB46=Cyc_Tcutil_compress(t);void*_tmpB47=_tmpB46;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB47)->tag == 8){_LL9C1: _LL9C2:
# 5825
 return 1;}else{_LL9C3: _LL9C4:
 return 0;}_LL9C0:;}
# 5830
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
# 5832
void*_tmpB48=Cyc_Tcutil_compress(t);void*_tmpB49=_tmpB48;void*_tmpB4A;struct Cyc_Absyn_Tqual _tmpB4B;struct Cyc_Absyn_Exp*_tmpB4C;union Cyc_Absyn_Constraint*_tmpB4D;unsigned int _tmpB4E;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB49)->tag == 8){_LL9C6: _tmpB4A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB49)->f1).elt_type;_tmpB4B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB49)->f1).tq;_tmpB4C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB49)->f1).num_elts;_tmpB4D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB49)->f1).zero_term;_tmpB4E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB49)->f1).zt_loc;_LL9C7: {
# 5834
void*b;
if(_tmpB4C == 0)
b=(void*)& Cyc_Absyn_DynEither_b_val;else{
# 5838
struct Cyc_Absyn_Exp*bnd=_tmpB4C;
if(convert_tag){
if(bnd->topt == 0)
({void*_tmpB4F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpB50="cannot convert tag without type!";_tag_dyneither(_tmpB50,sizeof(char),33);}),_tag_dyneither(_tmpB4F,sizeof(void*),0));});{
void*_tmpB51=Cyc_Tcutil_compress((void*)_check_null(bnd->topt));void*_tmpB52=_tmpB51;void*_tmpB53;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpB52)->tag == 19){_LL9CB: _tmpB53=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpB52)->f1;_LL9CC:
# 5844
 b=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpB54=_cycalloc(sizeof(*_tmpB54));_tmpB54[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpB55;_tmpB55.tag=1;_tmpB55.f1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmpB53,0),0,Cyc_Absyn_No_coercion,0);_tmpB55;});_tmpB54;});
goto _LL9CA;}else{_LL9CD: _LL9CE:
# 5847
 if(Cyc_Tcutil_is_const_exp(bnd))
b=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpB56=_cycalloc(sizeof(*_tmpB56));_tmpB56[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpB57;_tmpB57.tag=1;_tmpB57.f1=bnd;_tmpB57;});_tmpB56;});else{
# 5850
b=(void*)& Cyc_Absyn_DynEither_b_val;}}_LL9CA:;};}else{
# 5854
b=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpB58=_cycalloc(sizeof(*_tmpB58));_tmpB58[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpB59;_tmpB59.tag=1;_tmpB59.f1=bnd;_tmpB59;});_tmpB58;});}}
# 5856
return Cyc_Absyn_atb_typ(_tmpB4A,rgn,_tmpB4B,b,_tmpB4D);}}else{_LL9C8: _LL9C9:
# 5859
 return t;}_LL9C5:;}
# 5864
int Cyc_Tcutil_zeroable_type(void*t){
void*_tmpB5A=Cyc_Tcutil_compress(t);void*_tmpB5B=_tmpB5A;struct Cyc_List_List*_tmpB5C;union Cyc_Absyn_AggrInfoU _tmpB5D;struct Cyc_List_List*_tmpB5E;struct Cyc_List_List*_tmpB5F;void*_tmpB60;union Cyc_Absyn_Constraint*_tmpB61;switch(*((int*)_tmpB5B)){case 0: _LL9D0: _LL9D1:
 return 1;case 1: _LL9D2: _LL9D3:
 goto _LL9D5;case 2: _LL9D4: _LL9D5:
 goto _LL9D7;case 3: _LL9D6: _LL9D7:
 goto _LL9D9;case 4: _LL9D8: _LL9D9:
 return 0;case 5: _LL9DA: _tmpB61=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB5B)->f1).ptr_atts).nullable;_LL9DB:
# 5872
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmpB61);case 6: _LL9DC: _LL9DD:
 goto _LL9DF;case 7: _LL9DE: _LL9DF:
 return 1;case 8: _LL9E0: _tmpB60=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB5B)->f1).elt_type;_LL9E1:
 return Cyc_Tcutil_zeroable_type(_tmpB60);case 9: _LL9E2: _LL9E3:
 return 0;case 10: _LL9E4: _tmpB5F=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpB5B)->f1;_LL9E5:
# 5878
 for(0;(unsigned int)_tmpB5F;_tmpB5F=_tmpB5F->tl){
if(!Cyc_Tcutil_zeroable_type((*((struct _tuple12*)_tmpB5F->hd)).f2))return 0;}
return 1;case 11: _LL9E6: _tmpB5D=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpB5B)->f1).aggr_info;_tmpB5E=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpB5B)->f1).targs;_LL9E7: {
# 5882
struct Cyc_Absyn_Aggrdecl*_tmpB62=Cyc_Absyn_get_known_aggrdecl(_tmpB5D);
if(_tmpB62->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB62->impl))->exist_vars != 0)
return 0;{
struct _RegionHandle _tmpB63=_new_region("r");struct _RegionHandle*r=& _tmpB63;_push_region(r);
{struct Cyc_List_List*_tmpB64=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmpB62->tvs,_tmpB5E);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB62->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmpB64,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpB65=0;_npop_handler(0);return _tmpB65;}}}{
int _tmpB66=1;_npop_handler(0);return _tmpB66;};}
# 5886
;_pop_region(r);};}case 13: _LL9E8: _LL9E9:
# 5890
 return 1;case 19: _LL9EA: _LL9EB:
 return 1;case 12: _LL9EC: _tmpB5C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpB5B)->f2;_LL9ED:
# 5893
 for(0;_tmpB5C != 0;_tmpB5C=_tmpB5C->tl){
if(!Cyc_Tcutil_zeroable_type(((struct Cyc_Absyn_Aggrfield*)_tmpB5C->hd)->type))return 0;}
return 1;case 14: _LL9EE: _LL9EF:
 return 1;case 17: _LL9F0: _LL9F1:
 return 0;case 16: _LL9F2: _LL9F3:
 return 0;case 15: _LL9F4: _LL9F5:
 return 0;case 28: _LL9F6: _LL9F7:
 return 0;case 26: _LL9F8: _LL9F9:
# 5902
 goto _LL9FB;case 27: _LL9FA: _LL9FB:
 goto _LL9FD;case 18: _LL9FC: _LL9FD:
 goto _LL9FF;case 20: _LL9FE: _LL9FF:
 goto _LLA01;case 21: _LLA00: _LLA01:
 goto _LLA03;case 22: _LLA02: _LLA03:
 goto _LLA05;case 23: _LLA04: _LLA05:
 goto _LLA07;case 24: _LLA06: _LLA07:
 goto _LLA09;default: _LLA08: _LLA09:
({struct Cyc_String_pa_PrintArg_struct _tmpB69;_tmpB69.tag=0;_tmpB69.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmpB67[1]={& _tmpB69};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpB68="bad type `%s' in zeroable type";_tag_dyneither(_tmpB68,sizeof(char),31);}),_tag_dyneither(_tmpB67,sizeof(void*),1));});});}_LL9CF:;}
