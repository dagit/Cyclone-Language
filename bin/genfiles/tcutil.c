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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 251
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{int tag;};
# 286
typedef void*Cyc_Absyn_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 333
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 352
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 364
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 385
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 390
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 445 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 461
typedef void*Cyc_Absyn_funcparams_t;
# 464
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 528
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 543
extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;
# 554
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 561
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 579
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 938 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 940
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 948
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 951
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x);
# 955
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 958
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 960
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
# 966
void*Cyc_Absyn_compress_kb(void*);
# 971
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 976
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 978
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;
# 982
extern void*Cyc_Absyn_empty_effect;
# 992
extern struct _tuple2*Cyc_Absyn_datatype_print_arg_qvar;
extern struct _tuple2*Cyc_Absyn_datatype_scanf_arg_qvar;
# 1001
extern void*Cyc_Absyn_bounds_one;
# 1006
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 1032
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1042
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1049
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple2*,void*,unsigned int);
# 1051
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
# 1054
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1064
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1066
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned int);
# 1071
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1077
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
# 1089
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
# 1095
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1135
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);
# 1182
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1184
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1186
struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1188
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
# 1190
int Cyc_Absyn_fntype_att(void*a);struct _tuple13{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;};
# 1194
struct _tuple13 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1196
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
# 124 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(struct _RegionHandle*,void*ret_type,struct Cyc_List_List*args);
# 129
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 133
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 136
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 139
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 153
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
# 156
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
# 211
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 223
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
# 202
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);
# 228 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 230
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 238
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 241
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr err_msg);
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 248
void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);
# 250
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
# 252
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 254
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
# 256
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 262
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 264
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 266
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 270
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type);
# 276
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 281
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 284
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 289
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 294
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);struct _tuple18{int f1;void*f2;};
# 298
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 306
void*Cyc_Tcutil_normalize_effect(void*e);
# 309
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 311
int Cyc_Tcutil_new_tvar_id();
# 313
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 316
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 318
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
# 321
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 325
int Cyc_Tcutil_bits_only(void*t);
# 328
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 331
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e);
# 334
void*Cyc_Tcutil_snd_tqt(struct _tuple12*);
# 338
int Cyc_Tcutil_supports_default(void*);
# 342
int Cyc_Tcutil_admits_zero(void*t);
# 345
int Cyc_Tcutil_is_noreturn(void*);
# 349
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 353
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 357
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 360
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 363
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 367
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 370
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 373
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 380
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
{const char*_tmpEE2;if(Cyc_strcmp(((_tmpEE2="(qualifiers don't match)",_tag_dyneither(_tmpEE2,sizeof(char),25))),(struct _dyneither_ptr)Cyc_Tcutil_failure_reason)== 0){
{const char*_tmpEE6;void*_tmpEE5[1];struct Cyc_String_pa_PrintArg_struct _tmpEE4;(_tmpEE4.tag=0,((_tmpEE4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((_tmpEE5[0]=& _tmpEE4,Cyc_fprintf(Cyc_stderr,((_tmpEE6="  %s\n",_tag_dyneither(_tmpEE6,sizeof(char),6))),_tag_dyneither(_tmpEE5,sizeof(void*),1)))))));}
return;}}
# 75
{const char*_tmpEE7;if(Cyc_strcmp(((_tmpEE7="(function effects do not match)",_tag_dyneither(_tmpEE7,sizeof(char),32))),(struct _dyneither_ptr)Cyc_Tcutil_failure_reason)== 0){
struct Cyc_Absynpp_Params _tmp6=Cyc_Absynpp_tc_params_r;
_tmp6.print_all_effects=1;
Cyc_Absynpp_set_params(& _tmp6);}}{
# 80
struct _dyneither_ptr s1=Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure);
struct _dyneither_ptr s2=Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure);
int pos=2;
{const char*_tmpEEB;void*_tmpEEA[1];struct Cyc_String_pa_PrintArg_struct _tmpEE9;(_tmpEE9.tag=0,((_tmpEE9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmpEEA[0]=& _tmpEE9,Cyc_fprintf(Cyc_stderr,((_tmpEEB="  %s",_tag_dyneither(_tmpEEB,sizeof(char),5))),_tag_dyneither(_tmpEEA,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s1,sizeof(char));
if(pos + 5 >= 80){
{const char*_tmpEEE;void*_tmpEED;(_tmpEED=0,Cyc_fprintf(Cyc_stderr,((_tmpEEE="\n\t",_tag_dyneither(_tmpEEE,sizeof(char),3))),_tag_dyneither(_tmpEED,sizeof(void*),0)));}
pos=8;}else{
# 89
{const char*_tmpEF1;void*_tmpEF0;(_tmpEF0=0,Cyc_fprintf(Cyc_stderr,((_tmpEF1=" ",_tag_dyneither(_tmpEF1,sizeof(char),2))),_tag_dyneither(_tmpEF0,sizeof(void*),0)));}
++ pos;}
# 92
{const char*_tmpEF4;void*_tmpEF3;(_tmpEF3=0,Cyc_fprintf(Cyc_stderr,((_tmpEF4="and ",_tag_dyneither(_tmpEF4,sizeof(char),5))),_tag_dyneither(_tmpEF3,sizeof(void*),0)));}
pos +=4;
if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){
{const char*_tmpEF7;void*_tmpEF6;(_tmpEF6=0,Cyc_fprintf(Cyc_stderr,((_tmpEF7="\n\t",_tag_dyneither(_tmpEF7,sizeof(char),3))),_tag_dyneither(_tmpEF6,sizeof(void*),0)));}
pos=8;}
# 98
{const char*_tmpEFB;void*_tmpEFA[1];struct Cyc_String_pa_PrintArg_struct _tmpEF9;(_tmpEF9.tag=0,((_tmpEF9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmpEFA[0]=& _tmpEF9,Cyc_fprintf(Cyc_stderr,((_tmpEFB="%s ",_tag_dyneither(_tmpEFB,sizeof(char),4))),_tag_dyneither(_tmpEFA,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s2,sizeof(char))+ 1;
if(pos + 17 >= 80){
{const char*_tmpEFE;void*_tmpEFD;(_tmpEFD=0,Cyc_fprintf(Cyc_stderr,((_tmpEFE="\n\t",_tag_dyneither(_tmpEFE,sizeof(char),3))),_tag_dyneither(_tmpEFD,sizeof(void*),0)));}
pos=8;}
# 104
{const char*_tmpF01;void*_tmpF00;(_tmpF00=0,Cyc_fprintf(Cyc_stderr,((_tmpF01="are not compatible. ",_tag_dyneither(_tmpF01,sizeof(char),21))),_tag_dyneither(_tmpF00,sizeof(void*),0)));}
pos +=17;
if((char*)Cyc_Tcutil_failure_reason.curr != (char*)(_tag_dyneither(0,0,0)).curr){
if(pos + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason)>= 80){
const char*_tmpF04;void*_tmpF03;(_tmpF03=0,Cyc_fprintf(Cyc_stderr,((_tmpF04="\n\t",_tag_dyneither(_tmpF04,sizeof(char),3))),_tag_dyneither(_tmpF03,sizeof(void*),0)));}{
# 110
const char*_tmpF08;void*_tmpF07[1];struct Cyc_String_pa_PrintArg_struct _tmpF06;(_tmpF06.tag=0,((_tmpF06.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((_tmpF07[0]=& _tmpF06,Cyc_fprintf(Cyc_stderr,((_tmpF08="%s",_tag_dyneither(_tmpF08,sizeof(char),3))),_tag_dyneither(_tmpF07,sizeof(void*),1)))))));};}
# 112
{const char*_tmpF0B;void*_tmpF0A;(_tmpF0A=0,Cyc_fprintf(Cyc_stderr,((_tmpF0B="\n",_tag_dyneither(_tmpF0B,sizeof(char),2))),_tag_dyneither(_tmpF0A,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);};}
# 116
void Cyc_Tcutil_terr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 119
Cyc_Position_post_error(Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}
# 122
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 125
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
{const char*_tmpF0F;void*_tmpF0E[1];struct Cyc_String_pa_PrintArg_struct _tmpF0D;(_tmpF0D.tag=0,((_tmpF0D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmpF0E[0]=& _tmpF0D,Cyc_fprintf(Cyc_stderr,((_tmpF0F="Compiler Error (Tcutil::impos): %s\n",_tag_dyneither(_tmpF0F,sizeof(char),36))),_tag_dyneither(_tmpF0E,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Core_Impossible_exn_struct _tmpF12;struct Cyc_Core_Impossible_exn_struct*_tmpF11;(int)_throw((void*)((_tmpF11=_cycalloc(sizeof(*_tmpF11)),((_tmpF11[0]=((_tmpF12.tag=Cyc_Core_Impossible,((_tmpF12.f1=msg,_tmpF12)))),_tmpF11)))));};}char Cyc_Tcutil_AbortTypeCheckingFunction[26]="AbortTypeCheckingFunction";
# 133
static struct _dyneither_ptr Cyc_Tcutil_tvar2string(struct Cyc_Absyn_Tvar*tv){
return*tv->name;}
# 137
void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){
for(0;tvs != 0;tvs=tvs->tl){
const char*_tmpF17;void*_tmpF16[2];struct Cyc_String_pa_PrintArg_struct _tmpF15;struct Cyc_String_pa_PrintArg_struct _tmpF14;(_tmpF14.tag=0,((_tmpF14.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((_tmpF15.tag=0,((_tmpF15.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)tvs->hd)),((_tmpF16[0]=& _tmpF15,((_tmpF16[1]=& _tmpF14,Cyc_fprintf(Cyc_stderr,((_tmpF17="%s::%s ",_tag_dyneither(_tmpF17,sizeof(char),8))),_tag_dyneither(_tmpF16,sizeof(void*),2)))))))))))));}
{const char*_tmpF1A;void*_tmpF19;(_tmpF19=0,Cyc_fprintf(Cyc_stderr,((_tmpF1A="\n",_tag_dyneither(_tmpF1A,sizeof(char),2))),_tag_dyneither(_tmpF19,sizeof(void*),0)));}Cyc_fflush(Cyc_stderr);}
# 144
static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;
# 149
void Cyc_Tcutil_warn(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 152
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
{struct Cyc_List_List*_tmpF1B;Cyc_Tcutil_warning_segs=((_tmpF1B=_cycalloc(sizeof(*_tmpF1B)),((_tmpF1B->hd=(void*)sg,((_tmpF1B->tl=Cyc_Tcutil_warning_segs,_tmpF1B))))));}{
struct _dyneither_ptr*_tmpF1E;struct Cyc_List_List*_tmpF1D;Cyc_Tcutil_warning_msgs=((_tmpF1D=_cycalloc(sizeof(*_tmpF1D)),((_tmpF1D->hd=((_tmpF1E=_cycalloc(sizeof(*_tmpF1E)),((_tmpF1E[0]=msg,_tmpF1E)))),((_tmpF1D->tl=Cyc_Tcutil_warning_msgs,_tmpF1D))))));};}
# 156
void Cyc_Tcutil_flush_warnings(){
if(Cyc_Tcutil_warning_segs == 0)
return;
{const char*_tmpF21;void*_tmpF20;(_tmpF20=0,Cyc_fprintf(Cyc_stderr,((_tmpF21="***Warnings***\n",_tag_dyneither(_tmpF21,sizeof(char),16))),_tag_dyneither(_tmpF20,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp30=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);
Cyc_Tcutil_warning_segs=0;
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);
while(Cyc_Tcutil_warning_msgs != 0){
{const char*_tmpF26;void*_tmpF25[2];struct Cyc_String_pa_PrintArg_struct _tmpF24;struct Cyc_String_pa_PrintArg_struct _tmpF23;(_tmpF23.tag=0,((_tmpF23.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd)),((_tmpF24.tag=0,((_tmpF24.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp30))->hd)),((_tmpF25[0]=& _tmpF24,((_tmpF25[1]=& _tmpF23,Cyc_fprintf(Cyc_stderr,((_tmpF26="%s: %s\n",_tag_dyneither(_tmpF26,sizeof(char),8))),_tag_dyneither(_tmpF25,sizeof(void*),2)))))))))))));}
_tmp30=_tmp30->tl;
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}
# 168
{const char*_tmpF29;void*_tmpF28;(_tmpF28=0,Cyc_fprintf(Cyc_stderr,((_tmpF29="**************\n",_tag_dyneither(_tmpF29,sizeof(char),16))),_tag_dyneither(_tmpF28,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);};}
# 173
struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set=0;
# 177
static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
return tv1->identity - tv2->identity;}
# 182
void*Cyc_Tcutil_compress(void*t){
void*_tmp37=t;void**_tmp3B;void**_tmp3D;struct Cyc_Absyn_Exp*_tmp3F;struct Cyc_Absyn_Exp*_tmp41;void*_tmp43;_LL1: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp38=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp37;if(_tmp38->tag != 1)goto _LL3;else{if((void*)_tmp38->f2 != 0)goto _LL3;}}_LL2:
 goto _LL4;_LL3: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp39=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp37;if(_tmp39->tag != 17)goto _LL5;else{if((void*)_tmp39->f4 != 0)goto _LL5;}}_LL4:
 return t;_LL5: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp3A=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp37;if(_tmp3A->tag != 17)goto _LL7;else{_tmp3B=(void**)((void**)& _tmp3A->f4);}}_LL6: {
# 187
void*ta=(void*)_check_null(*_tmp3B);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp3B=t2;
return t2;}_LL7: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp37;if(_tmp3C->tag != 1)goto _LL9;else{_tmp3D=(void**)((void**)& _tmp3C->f2);}}_LL8: {
# 193
void*ta=(void*)_check_null(*_tmp3D);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp3D=t2;
return t2;}_LL9: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp3E=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp37;if(_tmp3E->tag != 18)goto _LLB;else{_tmp3F=_tmp3E->f1;}}_LLA:
# 199
 Cyc_Evexp_eval_const_uint_exp(_tmp3F);{
void*_tmp44=_tmp3F->r;void*_tmp45=_tmp44;void*_tmp47;_LL12: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp46=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp45;if(_tmp46->tag != 38)goto _LL14;else{_tmp47=(void*)_tmp46->f1;}}_LL13:
 return Cyc_Tcutil_compress(_tmp47);_LL14:;_LL15:
 return t;_LL11:;};_LLB: {struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp40=(struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp37;if(_tmp40->tag != 27)goto _LLD;else{_tmp41=_tmp40->f1;}}_LLC: {
# 205
void*_tmp48=_tmp41->topt;
if(_tmp48 != 0)return _tmp48;else{
return t;}}_LLD: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp42=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp37;if(_tmp42->tag != 26)goto _LLF;else{if(_tmp42->f2 == 0)goto _LLF;_tmp43=*_tmp42->f2;}}_LLE:
# 209
 return Cyc_Tcutil_compress(_tmp43);_LLF:;_LL10:
 return t;_LL0:;}
# 217
void*Cyc_Tcutil_copy_type(void*t);
static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){
return((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);}
# 221
static union Cyc_Absyn_Constraint*Cyc_Tcutil_copy_conref(union Cyc_Absyn_Constraint*cptr){
union Cyc_Absyn_Constraint*_tmp49=cptr;void*_tmp4A;union Cyc_Absyn_Constraint*_tmp4B;_LL17: if((_tmp49->No_constr).tag != 3)goto _LL19;_LL18:
 return Cyc_Absyn_empty_conref();_LL19: if((_tmp49->Eq_constr).tag != 1)goto _LL1B;_tmp4A=(void*)(_tmp49->Eq_constr).val;_LL1A:
 return Cyc_Absyn_new_conref(_tmp4A);_LL1B: if((_tmp49->Forward_constr).tag != 2)goto _LL16;_tmp4B=(union Cyc_Absyn_Constraint*)(_tmp49->Forward_constr).val;_LL1C:
 return Cyc_Tcutil_copy_conref(_tmp4B);_LL16:;}
# 228
static void*Cyc_Tcutil_copy_kindbound(void*kb){
void*_tmp4C=Cyc_Absyn_compress_kb(kb);void*_tmp4D=_tmp4C;struct Cyc_Absyn_Kind*_tmp50;_LL1E: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp4E=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp4D;if(_tmp4E->tag != 1)goto _LL20;}_LL1F: {
struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpF2C;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpF2B;return(void*)((_tmpF2B=_cycalloc(sizeof(*_tmpF2B)),((_tmpF2B[0]=((_tmpF2C.tag=1,((_tmpF2C.f1=0,_tmpF2C)))),_tmpF2B))));}_LL20: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp4F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4D;if(_tmp4F->tag != 2)goto _LL22;else{_tmp50=_tmp4F->f2;}}_LL21: {
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpF2F;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpF2E;return(void*)((_tmpF2E=_cycalloc(sizeof(*_tmpF2E)),((_tmpF2E[0]=((_tmpF2F.tag=2,((_tmpF2F.f1=0,((_tmpF2F.f2=_tmp50,_tmpF2F)))))),_tmpF2E))));}_LL22:;_LL23:
 return kb;_LL1D:;}
# 235
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 237
struct Cyc_Absyn_Tvar*_tmpF30;return(_tmpF30=_cycalloc(sizeof(*_tmpF30)),((_tmpF30->name=tv->name,((_tmpF30->identity=- 1,((_tmpF30->kind=Cyc_Tcutil_copy_kindbound(tv->kind),_tmpF30)))))));}
# 239
static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
# 241
struct _dyneither_ptr*_tmp57;struct Cyc_Absyn_Tqual _tmp58;void*_tmp59;struct _tuple10*_tmp56=arg;_tmp57=_tmp56->f1;_tmp58=_tmp56->f2;_tmp59=_tmp56->f3;{
struct _tuple10*_tmpF31;return(_tmpF31=_cycalloc(sizeof(*_tmpF31)),((_tmpF31->f1=_tmp57,((_tmpF31->f2=_tmp58,((_tmpF31->f3=Cyc_Tcutil_copy_type(_tmp59),_tmpF31)))))));};}
# 244
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
struct Cyc_Absyn_Tqual _tmp5C;void*_tmp5D;struct _tuple12*_tmp5B=arg;_tmp5C=_tmp5B->f1;_tmp5D=_tmp5B->f2;{
struct _tuple12*_tmpF32;return(_tmpF32=_cycalloc(sizeof(*_tmpF32)),((_tmpF32->f1=_tmp5C,((_tmpF32->f2=Cyc_Tcutil_copy_type(_tmp5D),_tmpF32)))));};}
# 248
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*);
# 250
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_Absyn_Aggrfield*_tmpF33;return(_tmpF33=_cycalloc(sizeof(*_tmpF33)),((_tmpF33->name=f->name,((_tmpF33->tq=f->tq,((_tmpF33->type=Cyc_Tcutil_copy_type(f->type),((_tmpF33->width=f->width,((_tmpF33->attributes=f->attributes,((_tmpF33->requires_clause=
Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause),_tmpF33)))))))))))));}
# 254
static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
void*_tmp61;void*_tmp62;struct _tuple0*_tmp60=x;_tmp61=_tmp60->f1;_tmp62=_tmp60->f2;{
struct _tuple0*_tmpF34;return(_tmpF34=_cycalloc(sizeof(*_tmpF34)),((_tmpF34->f1=Cyc_Tcutil_copy_type(_tmp61),((_tmpF34->f2=Cyc_Tcutil_copy_type(_tmp62),_tmpF34)))));};}
# 258
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){
struct Cyc_Absyn_Enumfield*_tmpF35;return(_tmpF35=_cycalloc(sizeof(*_tmpF35)),((_tmpF35->name=f->name,((_tmpF35->tag=f->tag,((_tmpF35->loc=f->loc,_tmpF35)))))));}
# 261
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpF38;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpF37;return(void*)((_tmpF37=_cycalloc(sizeof(*_tmpF37)),((_tmpF37[0]=((_tmpF38.tag=2,((_tmpF38.f1=Cyc_Tcutil_copy_tvar(t),_tmpF38)))),_tmpF37))));}
# 265
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp67=Cyc_Tcutil_compress(t);void*_tmp68=_tmp67;struct Cyc_Absyn_Tvar*_tmp6C;union Cyc_Absyn_DatatypeInfoU _tmp6E;struct Cyc_List_List*_tmp6F;union Cyc_Absyn_DatatypeFieldInfoU _tmp71;struct Cyc_List_List*_tmp72;void*_tmp74;struct Cyc_Absyn_Tqual _tmp75;void*_tmp76;union Cyc_Absyn_Constraint*_tmp77;union Cyc_Absyn_Constraint*_tmp78;union Cyc_Absyn_Constraint*_tmp79;struct Cyc_Absyn_PtrLoc*_tmp7A;void*_tmp7E;struct Cyc_Absyn_Tqual _tmp7F;struct Cyc_Absyn_Exp*_tmp80;union Cyc_Absyn_Constraint*_tmp81;unsigned int _tmp82;struct Cyc_List_List*_tmp84;void*_tmp85;struct Cyc_Absyn_Tqual _tmp86;void*_tmp87;struct Cyc_List_List*_tmp88;int _tmp89;struct Cyc_Absyn_VarargInfo*_tmp8A;struct Cyc_List_List*_tmp8B;struct Cyc_List_List*_tmp8C;struct Cyc_Absyn_Exp*_tmp8D;struct Cyc_List_List*_tmp8E;struct Cyc_Absyn_Exp*_tmp8F;struct Cyc_List_List*_tmp90;struct Cyc_List_List*_tmp92;enum Cyc_Absyn_AggrKind _tmp94;struct _tuple2*_tmp95;struct Cyc_Core_Opt*_tmp96;struct Cyc_List_List*_tmp97;struct Cyc_Absyn_Aggrdecl**_tmp99;struct Cyc_List_List*_tmp9A;enum Cyc_Absyn_AggrKind _tmp9C;struct Cyc_List_List*_tmp9D;struct _tuple2*_tmp9F;struct Cyc_Absyn_Enumdecl*_tmpA0;struct Cyc_List_List*_tmpA2;void*_tmpA4;struct Cyc_Absyn_Exp*_tmpA6;struct Cyc_Absyn_Exp*_tmpA8;void*_tmpAA;void*_tmpAC;void*_tmpAD;struct _tuple2*_tmpAF;struct Cyc_List_List*_tmpB0;struct Cyc_Absyn_Typedefdecl*_tmpB1;void*_tmpB7;struct Cyc_List_List*_tmpB9;void*_tmpBB;struct Cyc_Absyn_Aggrdecl*_tmpBE;struct Cyc_Absyn_Enumdecl*_tmpC1;struct Cyc_Absyn_Datatypedecl*_tmpC4;_LL25: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp69=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp68;if(_tmp69->tag != 0)goto _LL27;}_LL26:
 goto _LL28;_LL27: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp6A=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp68;if(_tmp6A->tag != 1)goto _LL29;}_LL28:
 return t;_LL29: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp6B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp68;if(_tmp6B->tag != 2)goto _LL2B;else{_tmp6C=_tmp6B->f1;}}_LL2A: {
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpF3B;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpF3A;return(void*)((_tmpF3A=_cycalloc(sizeof(*_tmpF3A)),((_tmpF3A[0]=((_tmpF3B.tag=2,((_tmpF3B.f1=Cyc_Tcutil_copy_tvar(_tmp6C),_tmpF3B)))),_tmpF3A))));}_LL2B: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp6D=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp68;if(_tmp6D->tag != 3)goto _LL2D;else{_tmp6E=(_tmp6D->f1).datatype_info;_tmp6F=(_tmp6D->f1).targs;}}_LL2C: {
# 271
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpF41;struct Cyc_Absyn_DatatypeInfo _tmpF40;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpF3F;return(void*)((_tmpF3F=_cycalloc(sizeof(*_tmpF3F)),((_tmpF3F[0]=((_tmpF41.tag=3,((_tmpF41.f1=((_tmpF40.datatype_info=_tmp6E,((_tmpF40.targs=Cyc_Tcutil_copy_types(_tmp6F),_tmpF40)))),_tmpF41)))),_tmpF3F))));}_LL2D: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp70=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp68;if(_tmp70->tag != 4)goto _LL2F;else{_tmp71=(_tmp70->f1).field_info;_tmp72=(_tmp70->f1).targs;}}_LL2E: {
# 273
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpF47;struct Cyc_Absyn_DatatypeFieldInfo _tmpF46;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpF45;return(void*)((_tmpF45=_cycalloc(sizeof(*_tmpF45)),((_tmpF45[0]=((_tmpF47.tag=4,((_tmpF47.f1=((_tmpF46.field_info=_tmp71,((_tmpF46.targs=Cyc_Tcutil_copy_types(_tmp72),_tmpF46)))),_tmpF47)))),_tmpF45))));}_LL2F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp73=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp68;if(_tmp73->tag != 5)goto _LL31;else{_tmp74=(_tmp73->f1).elt_typ;_tmp75=(_tmp73->f1).elt_tq;_tmp76=((_tmp73->f1).ptr_atts).rgn;_tmp77=((_tmp73->f1).ptr_atts).nullable;_tmp78=((_tmp73->f1).ptr_atts).bounds;_tmp79=((_tmp73->f1).ptr_atts).zero_term;_tmp7A=((_tmp73->f1).ptr_atts).ptrloc;}}_LL30: {
# 275
void*_tmpCD=Cyc_Tcutil_copy_type(_tmp74);
void*_tmpCE=Cyc_Tcutil_copy_type(_tmp76);
union Cyc_Absyn_Constraint*_tmpCF=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp77);
struct Cyc_Absyn_Tqual _tmpD0=_tmp75;
# 280
union Cyc_Absyn_Constraint*_tmpD1=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp78);
union Cyc_Absyn_Constraint*_tmpD2=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp79);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpF4D;struct Cyc_Absyn_PtrInfo _tmpF4C;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpF4B;return(void*)((_tmpF4B=_cycalloc(sizeof(*_tmpF4B)),((_tmpF4B[0]=((_tmpF4D.tag=5,((_tmpF4D.f1=((_tmpF4C.elt_typ=_tmpCD,((_tmpF4C.elt_tq=_tmpD0,((_tmpF4C.ptr_atts=(((_tmpF4C.ptr_atts).rgn=_tmpCE,(((_tmpF4C.ptr_atts).nullable=_tmpCF,(((_tmpF4C.ptr_atts).bounds=_tmpD1,(((_tmpF4C.ptr_atts).zero_term=_tmpD2,(((_tmpF4C.ptr_atts).ptrloc=_tmp7A,_tmpF4C.ptr_atts)))))))))),_tmpF4C)))))),_tmpF4D)))),_tmpF4B))));}_LL31: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp68;if(_tmp7B->tag != 6)goto _LL33;}_LL32:
 goto _LL34;_LL33: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp68;if(_tmp7C->tag != 7)goto _LL35;}_LL34:
 return t;_LL35: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp7D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp68;if(_tmp7D->tag != 8)goto _LL37;else{_tmp7E=(_tmp7D->f1).elt_type;_tmp7F=(_tmp7D->f1).tq;_tmp80=(_tmp7D->f1).num_elts;_tmp81=(_tmp7D->f1).zero_term;_tmp82=(_tmp7D->f1).zt_loc;}}_LL36: {
# 286
struct Cyc_Absyn_Exp*eopt2=Cyc_Tcutil_deep_copy_exp_opt(1,_tmp80);
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpF53;struct Cyc_Absyn_ArrayInfo _tmpF52;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpF51;return(void*)((_tmpF51=_cycalloc(sizeof(*_tmpF51)),((_tmpF51[0]=((_tmpF53.tag=8,((_tmpF53.f1=((_tmpF52.elt_type=Cyc_Tcutil_copy_type(_tmp7E),((_tmpF52.tq=_tmp7F,((_tmpF52.num_elts=eopt2,((_tmpF52.zero_term=
((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp81),((_tmpF52.zt_loc=_tmp82,_tmpF52)))))))))),_tmpF53)))),_tmpF51))));}_LL37: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp83=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68;if(_tmp83->tag != 9)goto _LL39;else{_tmp84=(_tmp83->f1).tvars;_tmp85=(_tmp83->f1).effect;_tmp86=(_tmp83->f1).ret_tqual;_tmp87=(_tmp83->f1).ret_typ;_tmp88=(_tmp83->f1).args;_tmp89=(_tmp83->f1).c_varargs;_tmp8A=(_tmp83->f1).cyc_varargs;_tmp8B=(_tmp83->f1).rgn_po;_tmp8C=(_tmp83->f1).attributes;_tmp8D=(_tmp83->f1).requires_clause;_tmp8E=(_tmp83->f1).requires_relns;_tmp8F=(_tmp83->f1).ensures_clause;_tmp90=(_tmp83->f1).ensures_relns;}}_LL38: {
# 290
struct Cyc_List_List*_tmpD9=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp84);
void*effopt2=_tmp85 == 0?0: Cyc_Tcutil_copy_type(_tmp85);
void*_tmpDA=Cyc_Tcutil_copy_type(_tmp87);
struct Cyc_List_List*_tmpDB=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp88);
int _tmpDC=_tmp89;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(_tmp8A != 0){
struct Cyc_Absyn_VarargInfo*cv=_tmp8A;
struct Cyc_Absyn_VarargInfo*_tmpF54;cyc_varargs2=((_tmpF54=_cycalloc(sizeof(*_tmpF54)),((_tmpF54->name=cv->name,((_tmpF54->tq=cv->tq,((_tmpF54->type=Cyc_Tcutil_copy_type(cv->type),((_tmpF54->inject=cv->inject,_tmpF54))))))))));}{
# 301
struct Cyc_List_List*_tmpDE=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp8B);
struct Cyc_List_List*_tmpDF=_tmp8C;
struct Cyc_Absyn_Exp*_tmpE0=Cyc_Tcutil_deep_copy_exp_opt(1,_tmp8D);
struct Cyc_List_List*_tmpE1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp8E);
struct Cyc_Absyn_Exp*_tmpE2=Cyc_Tcutil_deep_copy_exp_opt(1,_tmp8F);
struct Cyc_List_List*_tmpE3=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp90);
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpF5A;struct Cyc_Absyn_FnInfo _tmpF59;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpF58;return(void*)((_tmpF58=_cycalloc(sizeof(*_tmpF58)),((_tmpF58[0]=((_tmpF5A.tag=9,((_tmpF5A.f1=((_tmpF59.tvars=_tmpD9,((_tmpF59.effect=effopt2,((_tmpF59.ret_tqual=_tmp86,((_tmpF59.ret_typ=_tmpDA,((_tmpF59.args=_tmpDB,((_tmpF59.c_varargs=_tmpDC,((_tmpF59.cyc_varargs=cyc_varargs2,((_tmpF59.rgn_po=_tmpDE,((_tmpF59.attributes=_tmpDF,((_tmpF59.requires_clause=_tmpE0,((_tmpF59.requires_relns=_tmpE1,((_tmpF59.ensures_clause=_tmpE2,((_tmpF59.ensures_relns=_tmpE3,_tmpF59)))))))))))))))))))))))))),_tmpF5A)))),_tmpF58))));};}_LL39: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp91=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp68;if(_tmp91->tag != 10)goto _LL3B;else{_tmp92=_tmp91->f1;}}_LL3A: {
# 310
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpF5D;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpF5C;return(void*)((_tmpF5C=_cycalloc(sizeof(*_tmpF5C)),((_tmpF5C[0]=((_tmpF5D.tag=10,((_tmpF5D.f1=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp92),_tmpF5D)))),_tmpF5C))));}_LL3B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp93=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp68;if(_tmp93->tag != 11)goto _LL3D;else{if((((_tmp93->f1).aggr_info).UnknownAggr).tag != 1)goto _LL3D;_tmp94=((struct _tuple4)(((_tmp93->f1).aggr_info).UnknownAggr).val).f1;_tmp95=((struct _tuple4)(((_tmp93->f1).aggr_info).UnknownAggr).val).f2;_tmp96=((struct _tuple4)(((_tmp93->f1).aggr_info).UnknownAggr).val).f3;_tmp97=(_tmp93->f1).targs;}}_LL3C: {
# 312
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpF63;struct Cyc_Absyn_AggrInfo _tmpF62;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF61;return(void*)((_tmpF61=_cycalloc(sizeof(*_tmpF61)),((_tmpF61[0]=((_tmpF63.tag=11,((_tmpF63.f1=((_tmpF62.aggr_info=Cyc_Absyn_UnknownAggr(_tmp94,_tmp95,_tmp96),((_tmpF62.targs=Cyc_Tcutil_copy_types(_tmp97),_tmpF62)))),_tmpF63)))),_tmpF61))));}_LL3D: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp98=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp68;if(_tmp98->tag != 11)goto _LL3F;else{if((((_tmp98->f1).aggr_info).KnownAggr).tag != 2)goto _LL3F;_tmp99=(struct Cyc_Absyn_Aggrdecl**)(((_tmp98->f1).aggr_info).KnownAggr).val;_tmp9A=(_tmp98->f1).targs;}}_LL3E: {
# 314
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpF69;struct Cyc_Absyn_AggrInfo _tmpF68;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF67;return(void*)((_tmpF67=_cycalloc(sizeof(*_tmpF67)),((_tmpF67[0]=((_tmpF69.tag=11,((_tmpF69.f1=((_tmpF68.aggr_info=Cyc_Absyn_KnownAggr(_tmp99),((_tmpF68.targs=Cyc_Tcutil_copy_types(_tmp9A),_tmpF68)))),_tmpF69)))),_tmpF67))));}_LL3F: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp9B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp68;if(_tmp9B->tag != 12)goto _LL41;else{_tmp9C=_tmp9B->f1;_tmp9D=_tmp9B->f2;}}_LL40: {
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpF6C;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpF6B;return(void*)((_tmpF6B=_cycalloc(sizeof(*_tmpF6B)),((_tmpF6B[0]=((_tmpF6C.tag=12,((_tmpF6C.f1=_tmp9C,((_tmpF6C.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmp9D),_tmpF6C)))))),_tmpF6B))));}_LL41: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp9E=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp68;if(_tmp9E->tag != 13)goto _LL43;else{_tmp9F=_tmp9E->f1;_tmpA0=_tmp9E->f2;}}_LL42: {
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpF6F;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpF6E;return(void*)((_tmpF6E=_cycalloc(sizeof(*_tmpF6E)),((_tmpF6E[0]=((_tmpF6F.tag=13,((_tmpF6F.f1=_tmp9F,((_tmpF6F.f2=_tmpA0,_tmpF6F)))))),_tmpF6E))));}_LL43: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpA1=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp68;if(_tmpA1->tag != 14)goto _LL45;else{_tmpA2=_tmpA1->f1;}}_LL44: {
struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpF72;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpF71;return(void*)((_tmpF71=_cycalloc(sizeof(*_tmpF71)),((_tmpF71[0]=((_tmpF72.tag=14,((_tmpF72.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmpA2),_tmpF72)))),_tmpF71))));}_LL45: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpA3=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp68;if(_tmpA3->tag != 19)goto _LL47;else{_tmpA4=(void*)_tmpA3->f1;}}_LL46: {
struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpF75;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpF74;return(void*)((_tmpF74=_cycalloc(sizeof(*_tmpF74)),((_tmpF74[0]=((_tmpF75.tag=19,((_tmpF75.f1=Cyc_Tcutil_copy_type(_tmpA4),_tmpF75)))),_tmpF74))));}_LL47: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpA5=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp68;if(_tmpA5->tag != 18)goto _LL49;else{_tmpA6=_tmpA5->f1;}}_LL48: {
# 321
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpF78;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpF77;return(void*)((_tmpF77=_cycalloc(sizeof(*_tmpF77)),((_tmpF77[0]=((_tmpF78.tag=18,((_tmpF78.f1=_tmpA6,_tmpF78)))),_tmpF77))));}_LL49: {struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmpA7=(struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp68;if(_tmpA7->tag != 27)goto _LL4B;else{_tmpA8=_tmpA7->f1;}}_LL4A: {
# 324
struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmpF7B;struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmpF7A;return(void*)((_tmpF7A=_cycalloc(sizeof(*_tmpF7A)),((_tmpF7A[0]=((_tmpF7B.tag=27,((_tmpF7B.f1=_tmpA8,_tmpF7B)))),_tmpF7A))));}_LL4B: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA9=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp68;if(_tmpA9->tag != 15)goto _LL4D;else{_tmpAA=(void*)_tmpA9->f1;}}_LL4C: {
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpF7E;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpF7D;return(void*)((_tmpF7D=_cycalloc(sizeof(*_tmpF7D)),((_tmpF7D[0]=((_tmpF7E.tag=15,((_tmpF7E.f1=Cyc_Tcutil_copy_type(_tmpAA),_tmpF7E)))),_tmpF7D))));}_LL4D: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpAB=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp68;if(_tmpAB->tag != 16)goto _LL4F;else{_tmpAC=(void*)_tmpAB->f1;_tmpAD=(void*)_tmpAB->f2;}}_LL4E: {
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpF81;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpF80;return(void*)((_tmpF80=_cycalloc(sizeof(*_tmpF80)),((_tmpF80[0]=((_tmpF81.tag=16,((_tmpF81.f1=Cyc_Tcutil_copy_type(_tmpAC),((_tmpF81.f2=Cyc_Tcutil_copy_type(_tmpAD),_tmpF81)))))),_tmpF80))));}_LL4F: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpAE=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp68;if(_tmpAE->tag != 17)goto _LL51;else{_tmpAF=_tmpAE->f1;_tmpB0=_tmpAE->f2;_tmpB1=_tmpAE->f3;}}_LL50: {
# 328
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpF84;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpF83;return(void*)((_tmpF83=_cycalloc(sizeof(*_tmpF83)),((_tmpF83[0]=((_tmpF84.tag=17,((_tmpF84.f1=_tmpAF,((_tmpF84.f2=Cyc_Tcutil_copy_types(_tmpB0),((_tmpF84.f3=_tmpB1,((_tmpF84.f4=0,_tmpF84)))))))))),_tmpF83))));}_LL51: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpB2=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp68;if(_tmpB2->tag != 21)goto _LL53;}_LL52:
 goto _LL54;_LL53: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpB3=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp68;if(_tmpB3->tag != 22)goto _LL55;}_LL54:
 goto _LL56;_LL55: {struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*_tmpB4=(struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp68;if(_tmpB4->tag != 28)goto _LL57;}_LL56:
 goto _LL58;_LL57: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmpB5=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp68;if(_tmpB5->tag != 20)goto _LL59;}_LL58:
 return t;_LL59: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpB6=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp68;if(_tmpB6->tag != 23)goto _LL5B;else{_tmpB7=(void*)_tmpB6->f1;}}_LL5A: {
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpF87;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpF86;return(void*)((_tmpF86=_cycalloc(sizeof(*_tmpF86)),((_tmpF86[0]=((_tmpF87.tag=23,((_tmpF87.f1=Cyc_Tcutil_copy_type(_tmpB7),_tmpF87)))),_tmpF86))));}_LL5B: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpB8=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp68;if(_tmpB8->tag != 24)goto _LL5D;else{_tmpB9=_tmpB8->f1;}}_LL5C: {
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF8A;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF89;return(void*)((_tmpF89=_cycalloc(sizeof(*_tmpF89)),((_tmpF89[0]=((_tmpF8A.tag=24,((_tmpF8A.f1=Cyc_Tcutil_copy_types(_tmpB9),_tmpF8A)))),_tmpF89))));}_LL5D: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpBA=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp68;if(_tmpBA->tag != 25)goto _LL5F;else{_tmpBB=(void*)_tmpBA->f1;}}_LL5E: {
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpF8D;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpF8C;return(void*)((_tmpF8C=_cycalloc(sizeof(*_tmpF8C)),((_tmpF8C[0]=((_tmpF8D.tag=25,((_tmpF8D.f1=Cyc_Tcutil_copy_type(_tmpBB),_tmpF8D)))),_tmpF8C))));}_LL5F: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpBC=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp68;if(_tmpBC->tag != 26)goto _LL61;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmpBD=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmpBC->f1)->r;if(_tmpBD->tag != 0)goto _LL61;else{_tmpBE=_tmpBD->f1;}}}_LL60: {
# 338
struct Cyc_List_List*_tmp107=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpBE->tvs);
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpF93;struct Cyc_Absyn_AggrInfo _tmpF92;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF91;return(void*)((_tmpF91=_cycalloc(sizeof(*_tmpF91)),((_tmpF91[0]=((_tmpF93.tag=11,((_tmpF93.f1=((_tmpF92.aggr_info=Cyc_Absyn_UnknownAggr(_tmpBE->kind,_tmpBE->name,0),((_tmpF92.targs=_tmp107,_tmpF92)))),_tmpF93)))),_tmpF91))));}_LL61: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpBF=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp68;if(_tmpBF->tag != 26)goto _LL63;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpC0=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmpBF->f1)->r;if(_tmpC0->tag != 1)goto _LL63;else{_tmpC1=_tmpC0->f1;}}}_LL62: {
# 341
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpF96;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpF95;return(void*)((_tmpF95=_cycalloc(sizeof(*_tmpF95)),((_tmpF95[0]=((_tmpF96.tag=13,((_tmpF96.f1=_tmpC1->name,((_tmpF96.f2=0,_tmpF96)))))),_tmpF95))));}_LL63: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpC2=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp68;if(_tmpC2->tag != 26)goto _LL24;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmpC3=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmpC2->f1)->r;if(_tmpC3->tag != 2)goto _LL24;else{_tmpC4=_tmpC3->f1;}}}_LL64: {
# 343
struct Cyc_List_List*_tmp10D=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpC4->tvs);
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpFA0;struct Cyc_Absyn_UnknownDatatypeInfo _tmpF9F;struct Cyc_Absyn_DatatypeInfo _tmpF9E;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpF9D;return(void*)((_tmpF9D=_cycalloc(sizeof(*_tmpF9D)),((_tmpF9D[0]=((_tmpFA0.tag=3,((_tmpFA0.f1=((_tmpF9E.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpF9F.name=_tmpC4->name,((_tmpF9F.is_extensible=0,_tmpF9F))))),((_tmpF9E.targs=_tmp10D,_tmpF9E)))),_tmpFA0)))),_tmpF9D))));}_LL24:;}
# 358 "tcutil.cyc"
static void*Cyc_Tcutil_copy_designator(int pt,void*d){
void*_tmp112=d;struct Cyc_Absyn_Exp*_tmp114;struct _dyneither_ptr*_tmp116;_LL66: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp113=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp112;if(_tmp113->tag != 0)goto _LL68;else{_tmp114=_tmp113->f1;}}_LL67: {
struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpFA3;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpFA2;return(void*)((_tmpFA2=_cycalloc(sizeof(*_tmpFA2)),((_tmpFA2[0]=((_tmpFA3.tag=0,((_tmpFA3.f1=Cyc_Tcutil_deep_copy_exp(pt,_tmp114),_tmpFA3)))),_tmpFA2))));}_LL68: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp115=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp112;if(_tmp115->tag != 1)goto _LL65;else{_tmp116=_tmp115->f1;}}_LL69:
 return d;_LL65:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 364
static struct _tuple19*Cyc_Tcutil_copy_eds(int pt,struct _tuple19*e){
# 366
struct _tuple19*_tmpFA4;return(_tmpFA4=_cycalloc(sizeof(*_tmpFA4)),((_tmpFA4->f1=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,pt,(e[0]).f1),((_tmpFA4->f2=Cyc_Tcutil_deep_copy_exp(pt,(e[0]).f2),_tmpFA4)))));}
# 369
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*new_e;
int _tmp11A=preserve_types;
{void*_tmp11B=e->r;void*_tmp11C=_tmp11B;union Cyc_Absyn_Cnst _tmp11E;struct _tuple2*_tmp120;void*_tmp121;enum Cyc_Absyn_Primop _tmp123;struct Cyc_List_List*_tmp124;struct Cyc_Absyn_Exp*_tmp126;struct Cyc_Core_Opt*_tmp127;struct Cyc_Absyn_Exp*_tmp128;struct Cyc_Absyn_Exp*_tmp12A;enum Cyc_Absyn_Incrementor _tmp12B;struct Cyc_Absyn_Exp*_tmp12D;struct Cyc_Absyn_Exp*_tmp12E;struct Cyc_Absyn_Exp*_tmp12F;struct Cyc_Absyn_Exp*_tmp131;struct Cyc_Absyn_Exp*_tmp132;struct Cyc_Absyn_Exp*_tmp134;struct Cyc_Absyn_Exp*_tmp135;struct Cyc_Absyn_Exp*_tmp137;struct Cyc_Absyn_Exp*_tmp138;struct Cyc_Absyn_Exp*_tmp13A;struct Cyc_List_List*_tmp13B;struct Cyc_Absyn_VarargCallInfo*_tmp13C;int _tmp13D;struct Cyc_Absyn_Exp*_tmp13F;int _tmp140;struct Cyc_Absyn_Exp*_tmp142;struct Cyc_Absyn_Exp*_tmp144;struct Cyc_List_List*_tmp145;void*_tmp147;struct Cyc_Absyn_Exp*_tmp148;int _tmp149;enum Cyc_Absyn_Coercion _tmp14A;struct Cyc_Absyn_Exp*_tmp14C;struct Cyc_Absyn_Exp*_tmp14E;struct Cyc_Absyn_Exp*_tmp14F;void*_tmp151;struct Cyc_Absyn_Exp*_tmp153;void*_tmp155;struct Cyc_List_List*_tmp156;struct Cyc_Absyn_Exp*_tmp158;struct Cyc_Absyn_Exp*_tmp15A;struct _dyneither_ptr*_tmp15B;int _tmp15C;int _tmp15D;struct Cyc_Absyn_Exp*_tmp15F;struct _dyneither_ptr*_tmp160;int _tmp161;int _tmp162;struct Cyc_Absyn_Exp*_tmp164;struct Cyc_Absyn_Exp*_tmp165;struct Cyc_List_List*_tmp167;struct _dyneither_ptr*_tmp169;struct Cyc_Absyn_Tqual _tmp16A;void*_tmp16B;struct Cyc_List_List*_tmp16C;struct Cyc_List_List*_tmp16E;struct Cyc_Absyn_Vardecl*_tmp170;struct Cyc_Absyn_Exp*_tmp171;struct Cyc_Absyn_Exp*_tmp172;int _tmp173;struct Cyc_Absyn_Exp*_tmp175;void*_tmp176;int _tmp177;struct _tuple2*_tmp179;struct Cyc_List_List*_tmp17A;struct Cyc_List_List*_tmp17B;struct Cyc_Absyn_Aggrdecl*_tmp17C;void*_tmp17E;struct Cyc_List_List*_tmp17F;struct Cyc_List_List*_tmp181;struct Cyc_Absyn_Datatypedecl*_tmp182;struct Cyc_Absyn_Datatypefield*_tmp183;struct _tuple2*_tmp185;struct Cyc_Absyn_Enumdecl*_tmp186;struct Cyc_Absyn_Enumfield*_tmp187;struct _tuple2*_tmp189;void*_tmp18A;struct Cyc_Absyn_Enumfield*_tmp18B;int _tmp18D;struct Cyc_Absyn_Exp*_tmp18E;void**_tmp18F;struct Cyc_Absyn_Exp*_tmp190;int _tmp191;int _tmp192;struct Cyc_Absyn_Exp*_tmp194;struct Cyc_Absyn_Exp*_tmp195;struct Cyc_Core_Opt*_tmp197;struct Cyc_List_List*_tmp198;struct Cyc_Absyn_Exp*_tmp19B;struct _dyneither_ptr*_tmp19C;void*_tmp19E;int _tmp1A0;struct _dyneither_ptr _tmp1A1;_LL6B: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp11D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp11D->tag != 0)goto _LL6D;else{_tmp11E=_tmp11D->f1;}}_LL6C:
 new_e=Cyc_Absyn_const_exp(_tmp11E,e->loc);goto _LL6A;_LL6D: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp11F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp11F->tag != 1)goto _LL6F;else{_tmp120=_tmp11F->f1;_tmp121=(void*)_tmp11F->f2;}}_LL6E:
 new_e=Cyc_Absyn_varb_exp(_tmp120,_tmp121,e->loc);goto _LL6A;_LL6F: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp122=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp122->tag != 2)goto _LL71;else{_tmp123=_tmp122->f1;_tmp124=_tmp122->f2;}}_LL70:
 new_e=Cyc_Absyn_primop_exp(_tmp123,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp11A,_tmp124),e->loc);goto _LL6A;_LL71: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp125=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp125->tag != 3)goto _LL73;else{_tmp126=_tmp125->f1;_tmp127=_tmp125->f2;_tmp128=_tmp125->f3;}}_LL72:
# 377
{struct Cyc_Core_Opt*_tmpFA5;new_e=Cyc_Absyn_assignop_exp(Cyc_Tcutil_deep_copy_exp(_tmp11A,_tmp126),(unsigned int)_tmp127?(_tmpFA5=_cycalloc_atomic(sizeof(*_tmpFA5)),((_tmpFA5->v=(void*)((enum Cyc_Absyn_Primop)_tmp127->v),_tmpFA5))): 0,Cyc_Tcutil_deep_copy_exp(_tmp11A,_tmp128),e->loc);}
goto _LL6A;_LL73: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp129=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp129->tag != 4)goto _LL75;else{_tmp12A=_tmp129->f1;_tmp12B=_tmp129->f2;}}_LL74:
 new_e=Cyc_Absyn_increment_exp(Cyc_Tcutil_deep_copy_exp(_tmp11A,_tmp12A),_tmp12B,e->loc);goto _LL6A;_LL75: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp12C=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp12C->tag != 5)goto _LL77;else{_tmp12D=_tmp12C->f1;_tmp12E=_tmp12C->f2;_tmp12F=_tmp12C->f3;}}_LL76:
# 381
 new_e=Cyc_Absyn_conditional_exp(Cyc_Tcutil_deep_copy_exp(_tmp11A,_tmp12D),Cyc_Tcutil_deep_copy_exp(_tmp11A,_tmp12E),Cyc_Tcutil_deep_copy_exp(_tmp11A,_tmp12F),e->loc);goto _LL6A;_LL77: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp130=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp130->tag != 6)goto _LL79;else{_tmp131=_tmp130->f1;_tmp132=_tmp130->f2;}}_LL78:
 new_e=Cyc_Absyn_and_exp(Cyc_Tcutil_deep_copy_exp(_tmp11A,_tmp131),Cyc_Tcutil_deep_copy_exp(_tmp11A,_tmp132),e->loc);goto _LL6A;_LL79: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp133=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp133->tag != 7)goto _LL7B;else{_tmp134=_tmp133->f1;_tmp135=_tmp133->f2;}}_LL7A:
 new_e=Cyc_Absyn_or_exp(Cyc_Tcutil_deep_copy_exp(_tmp11A,_tmp134),Cyc_Tcutil_deep_copy_exp(_tmp11A,_tmp135),e->loc);goto _LL6A;_LL7B: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp136=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp136->tag != 8)goto _LL7D;else{_tmp137=_tmp136->f1;_tmp138=_tmp136->f2;}}_LL7C:
 new_e=Cyc_Absyn_seq_exp(Cyc_Tcutil_deep_copy_exp(_tmp11A,_tmp137),Cyc_Tcutil_deep_copy_exp(_tmp11A,_tmp138),e->loc);goto _LL6A;_LL7D: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp139=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp139->tag != 9)goto _LL7F;else{_tmp13A=_tmp139->f1;_tmp13B=_tmp139->f2;_tmp13C=_tmp139->f3;_tmp13D=_tmp139->f4;}}_LL7E:
# 386
{struct Cyc_Absyn_VarargCallInfo*_tmp1A3=_tmp13C;int _tmp1A4;struct Cyc_List_List*_tmp1A5;struct Cyc_Absyn_VarargInfo*_tmp1A6;_LLBC: if(_tmp1A3 == 0)goto _LLBE;_tmp1A4=_tmp1A3->num_varargs;_tmp1A5=_tmp1A3->injectors;_tmp1A6=_tmp1A3->vai;_LLBD: {
# 388
struct _dyneither_ptr*_tmp1A8;struct Cyc_Absyn_Tqual _tmp1A9;void*_tmp1AA;int _tmp1AB;struct Cyc_Absyn_VarargInfo*_tmp1A7=_tmp1A6;_tmp1A8=_tmp1A7->name;_tmp1A9=_tmp1A7->tq;_tmp1AA=_tmp1A7->type;_tmp1AB=_tmp1A7->inject;
{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpFAF;struct Cyc_Absyn_VarargInfo*_tmpFAE;struct Cyc_Absyn_VarargCallInfo*_tmpFAD;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpFAC;new_e=Cyc_Absyn_new_exp((void*)((_tmpFAC=_cycalloc(sizeof(*_tmpFAC)),((_tmpFAC[0]=((_tmpFAF.tag=9,((_tmpFAF.f1=
Cyc_Tcutil_deep_copy_exp(_tmp11A,_tmp13A),((_tmpFAF.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp11A,_tmp13B),((_tmpFAF.f3=(
(_tmpFAD=_cycalloc(sizeof(*_tmpFAD)),((_tmpFAD->num_varargs=_tmp1A4,((_tmpFAD->injectors=_tmp1A5,((_tmpFAD->vai=(
(_tmpFAE=_cycalloc(sizeof(*_tmpFAE)),((_tmpFAE->name=_tmp1A8,((_tmpFAE->tq=_tmp1A9,((_tmpFAE->type=Cyc_Tcutil_copy_type(_tmp1AA),((_tmpFAE->inject=_tmp1AB,_tmpFAE)))))))))),_tmpFAD)))))))),((_tmpFAF.f4=_tmp13D,_tmpFAF)))))))))),_tmpFAC)))),e->loc);}
# 394
goto _LLBB;}_LLBE:;_LLBF:
# 396
{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpFB2;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpFB1;new_e=Cyc_Absyn_new_exp((void*)((_tmpFB1=_cycalloc(sizeof(*_tmpFB1)),((_tmpFB1[0]=((_tmpFB2.tag=9,((_tmpFB2.f1=Cyc_Tcutil_deep_copy_exp(_tmp11A,_tmp13A),((_tmpFB2.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp11A,_tmp13B),((_tmpFB2.f3=_tmp13C,((_tmpFB2.f4=_tmp13D,_tmpFB2)))))))))),_tmpFB1)))),e->loc);}
goto _LLBB;_LLBB:;}
# 399
goto _LL6A;_LL7F: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp13E=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp13E->tag != 10)goto _LL81;else{_tmp13F=_tmp13E->f1;_tmp140=_tmp13E->f2;}}_LL80:
# 401
 new_e=_tmp140?Cyc_Absyn_rethrow_exp(Cyc_Tcutil_deep_copy_exp(_tmp11A,_tmp13F),e->loc): Cyc_Absyn_throw_exp(Cyc_Tcutil_deep_copy_exp(_tmp11A,_tmp13F),e->loc);
goto _LL6A;_LL81: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp141=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp141->tag != 11)goto _LL83;else{_tmp142=_tmp141->f1;}}_LL82:
 new_e=Cyc_Absyn_noinstantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmp11A,_tmp142),e->loc);
goto _LL6A;_LL83: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp143=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp143->tag != 12)goto _LL85;else{_tmp144=_tmp143->f1;_tmp145=_tmp143->f2;}}_LL84:
# 406
 new_e=Cyc_Absyn_instantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmp11A,_tmp144),((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp145),e->loc);
goto _LL6A;_LL85: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp146=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp146->tag != 13)goto _LL87;else{_tmp147=(void*)_tmp146->f1;_tmp148=_tmp146->f2;_tmp149=_tmp146->f3;_tmp14A=_tmp146->f4;}}_LL86:
# 409
 new_e=Cyc_Absyn_cast_exp(Cyc_Tcutil_copy_type(_tmp147),Cyc_Tcutil_deep_copy_exp(_tmp11A,_tmp148),_tmp149,_tmp14A,e->loc);goto _LL6A;_LL87: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp14B=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp14B->tag != 14)goto _LL89;else{_tmp14C=_tmp14B->f1;}}_LL88:
 new_e=Cyc_Absyn_address_exp(Cyc_Tcutil_deep_copy_exp(_tmp11A,_tmp14C),e->loc);goto _LL6A;_LL89: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp14D=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp14D->tag != 15)goto _LL8B;else{_tmp14E=_tmp14D->f1;_tmp14F=_tmp14D->f2;}}_LL8A: {
# 412
struct Cyc_Absyn_Exp*eo1=_tmp14E;if(_tmp14E != 0)eo1=Cyc_Tcutil_deep_copy_exp(_tmp11A,_tmp14E);
new_e=Cyc_Absyn_New_exp(eo1,Cyc_Tcutil_deep_copy_exp(_tmp11A,_tmp14F),e->loc);
goto _LL6A;}_LL8B: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp150=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp150->tag != 16)goto _LL8D;else{_tmp151=(void*)_tmp150->f1;}}_LL8C:
 new_e=Cyc_Absyn_sizeoftyp_exp(Cyc_Tcutil_copy_type(_tmp151),e->loc);
goto _LL6A;_LL8D: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp152=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp152->tag != 17)goto _LL8F;else{_tmp153=_tmp152->f1;}}_LL8E:
 new_e=Cyc_Absyn_sizeofexp_exp(Cyc_Tcutil_deep_copy_exp(_tmp11A,_tmp153),e->loc);goto _LL6A;_LL8F: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp154=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp154->tag != 18)goto _LL91;else{_tmp155=(void*)_tmp154->f1;_tmp156=_tmp154->f2;}}_LL90:
# 419
 new_e=Cyc_Absyn_offsetof_exp(Cyc_Tcutil_copy_type(_tmp155),_tmp156,e->loc);goto _LL6A;_LL91: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp157=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp157->tag != 19)goto _LL93;else{_tmp158=_tmp157->f1;}}_LL92:
 new_e=Cyc_Absyn_deref_exp(Cyc_Tcutil_deep_copy_exp(_tmp11A,_tmp158),e->loc);goto _LL6A;_LL93: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp159=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp159->tag != 20)goto _LL95;else{_tmp15A=_tmp159->f1;_tmp15B=_tmp159->f2;_tmp15C=_tmp159->f3;_tmp15D=_tmp159->f4;}}_LL94:
# 422
{struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpFB5;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpFB4;new_e=Cyc_Absyn_new_exp((void*)((_tmpFB4=_cycalloc(sizeof(*_tmpFB4)),((_tmpFB4[0]=((_tmpFB5.tag=20,((_tmpFB5.f1=Cyc_Tcutil_deep_copy_exp(_tmp11A,_tmp15A),((_tmpFB5.f2=_tmp15B,((_tmpFB5.f3=_tmp15C,((_tmpFB5.f4=_tmp15D,_tmpFB5)))))))))),_tmpFB4)))),e->loc);}goto _LL6A;_LL95: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp15E=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp15E->tag != 21)goto _LL97;else{_tmp15F=_tmp15E->f1;_tmp160=_tmp15E->f2;_tmp161=_tmp15E->f3;_tmp162=_tmp15E->f4;}}_LL96:
# 424
{struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpFB8;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpFB7;new_e=Cyc_Absyn_new_exp((void*)((_tmpFB7=_cycalloc(sizeof(*_tmpFB7)),((_tmpFB7[0]=((_tmpFB8.tag=21,((_tmpFB8.f1=Cyc_Tcutil_deep_copy_exp(_tmp11A,_tmp15F),((_tmpFB8.f2=_tmp160,((_tmpFB8.f3=_tmp161,((_tmpFB8.f4=_tmp162,_tmpFB8)))))))))),_tmpFB7)))),e->loc);}goto _LL6A;_LL97: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp163=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp163->tag != 22)goto _LL99;else{_tmp164=_tmp163->f1;_tmp165=_tmp163->f2;}}_LL98:
 new_e=Cyc_Absyn_subscript_exp(Cyc_Tcutil_deep_copy_exp(_tmp11A,_tmp164),Cyc_Tcutil_deep_copy_exp(_tmp11A,_tmp165),e->loc);
goto _LL6A;_LL99: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp166=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp166->tag != 23)goto _LL9B;else{_tmp167=_tmp166->f1;}}_LL9A:
 new_e=Cyc_Absyn_tuple_exp(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp11A,_tmp167),e->loc);goto _LL6A;_LL9B: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp168=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp168->tag != 24)goto _LL9D;else{_tmp169=(_tmp168->f1)->f1;_tmp16A=(_tmp168->f1)->f2;_tmp16B=(_tmp168->f1)->f3;_tmp16C=_tmp168->f2;}}_LL9C: {
# 429
struct _dyneither_ptr*vopt1=_tmp169;
if(_tmp169 != 0)vopt1=_tmp169;
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpFBE;struct _tuple10*_tmpFBD;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpFBC;new_e=Cyc_Absyn_new_exp((void*)((_tmpFBC=_cycalloc(sizeof(*_tmpFBC)),((_tmpFBC[0]=((_tmpFBE.tag=24,((_tmpFBE.f1=((_tmpFBD=_cycalloc(sizeof(*_tmpFBD)),((_tmpFBD->f1=vopt1,((_tmpFBD->f2=_tmp16A,((_tmpFBD->f3=Cyc_Tcutil_copy_type(_tmp16B),_tmpFBD)))))))),((_tmpFBE.f2=
((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp11A,_tmp16C),_tmpFBE)))))),_tmpFBC)))),e->loc);}
goto _LL6A;}_LL9D: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp16D=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp16D->tag != 25)goto _LL9F;else{_tmp16E=_tmp16D->f1;}}_LL9E:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmpFC1;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpFC0;new_e=Cyc_Absyn_new_exp((void*)((_tmpFC0=_cycalloc(sizeof(*_tmpFC0)),((_tmpFC0[0]=((_tmpFC1.tag=25,((_tmpFC1.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp11A,_tmp16E),_tmpFC1)))),_tmpFC0)))),e->loc);}
goto _LL6A;_LL9F: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp16F=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp16F->tag != 26)goto _LLA1;else{_tmp170=_tmp16F->f1;_tmp171=_tmp16F->f2;_tmp172=_tmp16F->f3;_tmp173=_tmp16F->f4;}}_LLA0:
# 437
{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpFC4;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpFC3;new_e=Cyc_Absyn_new_exp((void*)((_tmpFC3=_cycalloc(sizeof(*_tmpFC3)),((_tmpFC3[0]=((_tmpFC4.tag=26,((_tmpFC4.f1=_tmp170,((_tmpFC4.f2=Cyc_Tcutil_deep_copy_exp(_tmp11A,_tmp171),((_tmpFC4.f3=Cyc_Tcutil_deep_copy_exp(_tmp11A,_tmp172),((_tmpFC4.f4=_tmp173,_tmpFC4)))))))))),_tmpFC3)))),e->loc);}
goto _LL6A;_LLA1: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp174=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp174->tag != 27)goto _LLA3;else{_tmp175=_tmp174->f1;_tmp176=(void*)_tmp174->f2;_tmp177=_tmp174->f3;}}_LLA2:
# 440
{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpFC7;struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpFC6;new_e=Cyc_Absyn_new_exp((void*)((_tmpFC6=_cycalloc(sizeof(*_tmpFC6)),((_tmpFC6[0]=((_tmpFC7.tag=27,((_tmpFC7.f1=Cyc_Tcutil_deep_copy_exp(_tmp11A,_tmp175),((_tmpFC7.f2=Cyc_Tcutil_copy_type(_tmp176),((_tmpFC7.f3=_tmp177,_tmpFC7)))))))),_tmpFC6)))),_tmp175->loc);}
# 442
goto _LL6A;_LLA3: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp178=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp178->tag != 28)goto _LLA5;else{_tmp179=_tmp178->f1;_tmp17A=_tmp178->f2;_tmp17B=_tmp178->f3;_tmp17C=_tmp178->f4;}}_LLA4:
# 444
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpFCA;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpFC9;new_e=Cyc_Absyn_new_exp((void*)((_tmpFC9=_cycalloc(sizeof(*_tmpFC9)),((_tmpFC9[0]=((_tmpFCA.tag=28,((_tmpFCA.f1=_tmp179,((_tmpFCA.f2=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp17A),((_tmpFCA.f3=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp11A,_tmp17B),((_tmpFCA.f4=_tmp17C,_tmpFCA)))))))))),_tmpFC9)))),e->loc);}
# 446
goto _LL6A;_LLA5: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp17D=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp17D->tag != 29)goto _LLA7;else{_tmp17E=(void*)_tmp17D->f1;_tmp17F=_tmp17D->f2;}}_LLA6:
# 448
{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmpFCD;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpFCC;new_e=Cyc_Absyn_new_exp((void*)((_tmpFCC=_cycalloc(sizeof(*_tmpFCC)),((_tmpFCC[0]=((_tmpFCD.tag=29,((_tmpFCD.f1=Cyc_Tcutil_copy_type(_tmp17E),((_tmpFCD.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp11A,_tmp17F),_tmpFCD)))))),_tmpFCC)))),e->loc);}
goto _LL6A;_LLA7: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp180=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp180->tag != 30)goto _LLA9;else{_tmp181=_tmp180->f1;_tmp182=_tmp180->f2;_tmp183=_tmp180->f3;}}_LLA8:
# 451
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmpFD0;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpFCF;new_e=Cyc_Absyn_new_exp((void*)((_tmpFCF=_cycalloc(sizeof(*_tmpFCF)),((_tmpFCF[0]=((_tmpFD0.tag=30,((_tmpFD0.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp11A,_tmp181),((_tmpFD0.f2=_tmp182,((_tmpFD0.f3=_tmp183,_tmpFD0)))))))),_tmpFCF)))),e->loc);}
goto _LL6A;_LLA9: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp184=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp184->tag != 31)goto _LLAB;else{_tmp185=_tmp184->f1;_tmp186=_tmp184->f2;_tmp187=_tmp184->f3;}}_LLAA:
 new_e=e;goto _LL6A;_LLAB: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp188=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp188->tag != 32)goto _LLAD;else{_tmp189=_tmp188->f1;_tmp18A=(void*)_tmp188->f2;_tmp18B=_tmp188->f3;}}_LLAC:
# 455
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpFD3;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpFD2;new_e=Cyc_Absyn_new_exp((void*)((_tmpFD2=_cycalloc(sizeof(*_tmpFD2)),((_tmpFD2[0]=((_tmpFD3.tag=32,((_tmpFD3.f1=_tmp189,((_tmpFD3.f2=Cyc_Tcutil_copy_type(_tmp18A),((_tmpFD3.f3=_tmp18B,_tmpFD3)))))))),_tmpFD2)))),e->loc);}
goto _LL6A;_LLAD: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp18C=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp18C->tag != 33)goto _LLAF;else{_tmp18D=(_tmp18C->f1).is_calloc;_tmp18E=(_tmp18C->f1).rgn;_tmp18F=(_tmp18C->f1).elt_type;_tmp190=(_tmp18C->f1).num_elts;_tmp191=(_tmp18C->f1).fat_result;_tmp192=(_tmp18C->f1).inline_call;}}_LLAE: {
# 458
struct Cyc_Absyn_Exp*_tmp1C7=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp18E;if(_tmp18E != 0)r1=Cyc_Tcutil_deep_copy_exp(_tmp11A,_tmp18E);{
void**t1=_tmp18F;if(_tmp18F != 0){void**_tmpFD4;t1=((_tmpFD4=_cycalloc(sizeof(*_tmpFD4)),((_tmpFD4[0]=Cyc_Tcutil_copy_type(*_tmp18F),_tmpFD4))));}
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpFDA;struct Cyc_Absyn_MallocInfo _tmpFD9;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpFD8;_tmp1C7->r=(void*)((_tmpFD8=_cycalloc(sizeof(*_tmpFD8)),((_tmpFD8[0]=((_tmpFDA.tag=33,((_tmpFDA.f1=((_tmpFD9.is_calloc=_tmp18D,((_tmpFD9.rgn=r1,((_tmpFD9.elt_type=t1,((_tmpFD9.num_elts=_tmp190,((_tmpFD9.fat_result=_tmp191,((_tmpFD9.inline_call=_tmp192,_tmpFD9)))))))))))),_tmpFDA)))),_tmpFD8))));}
new_e=_tmp1C7;
goto _LL6A;};}_LLAF: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp193=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp193->tag != 34)goto _LLB1;else{_tmp194=_tmp193->f1;_tmp195=_tmp193->f2;}}_LLB0:
 new_e=Cyc_Absyn_swap_exp(Cyc_Tcutil_deep_copy_exp(_tmp11A,_tmp194),Cyc_Tcutil_deep_copy_exp(_tmp11A,_tmp195),e->loc);goto _LL6A;_LLB1: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp196=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp196->tag != 35)goto _LLB3;else{_tmp197=_tmp196->f1;_tmp198=_tmp196->f2;}}_LLB2: {
# 466
struct Cyc_Core_Opt*nopt1=_tmp197;
if(_tmp197 != 0){struct Cyc_Core_Opt*_tmpFDB;nopt1=((_tmpFDB=_cycalloc(sizeof(*_tmpFDB)),((_tmpFDB->v=(struct _tuple2*)_tmp197->v,_tmpFDB))));}
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpFDE;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpFDD;new_e=Cyc_Absyn_new_exp((void*)((_tmpFDD=_cycalloc(sizeof(*_tmpFDD)),((_tmpFDD[0]=((_tmpFDE.tag=35,((_tmpFDE.f1=nopt1,((_tmpFDE.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp11A,_tmp198),_tmpFDE)))))),_tmpFDD)))),e->loc);}
goto _LL6A;}_LLB3: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp199=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp199->tag != 36)goto _LLB5;}_LLB4: {
# 471
struct Cyc_Core_Failure_exn_struct _tmpFE4;const char*_tmpFE3;struct Cyc_Core_Failure_exn_struct*_tmpFE2;(int)_throw((void*)((_tmpFE2=_cycalloc(sizeof(*_tmpFE2)),((_tmpFE2[0]=((_tmpFE4.tag=Cyc_Core_Failure,((_tmpFE4.f1=((_tmpFE3="deep_copy: statement expressions unsupported",_tag_dyneither(_tmpFE3,sizeof(char),45))),_tmpFE4)))),_tmpFE2)))));}_LLB5: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp19A=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp19A->tag != 37)goto _LLB7;else{_tmp19B=_tmp19A->f1;_tmp19C=_tmp19A->f2;}}_LLB6:
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpFE7;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpFE6;new_e=Cyc_Absyn_new_exp((void*)((_tmpFE6=_cycalloc(sizeof(*_tmpFE6)),((_tmpFE6[0]=((_tmpFE7.tag=37,((_tmpFE7.f1=Cyc_Tcutil_deep_copy_exp(_tmp11A,_tmp19B),((_tmpFE7.f2=_tmp19C,_tmpFE7)))))),_tmpFE6)))),e->loc);}
goto _LL6A;_LLB7: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp19D=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp19D->tag != 38)goto _LLB9;else{_tmp19E=(void*)_tmp19D->f1;}}_LLB8:
 new_e=Cyc_Absyn_valueof_exp(Cyc_Tcutil_copy_type(_tmp19E),e->loc);
goto _LL6A;_LLB9: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp19F=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp11C;if(_tmp19F->tag != 39)goto _LL6A;else{_tmp1A0=_tmp19F->f1;_tmp1A1=_tmp19F->f2;}}_LLBA:
 new_e=Cyc_Absyn_asm_exp(_tmp1A0,_tmp1A1,e->loc);goto _LL6A;_LL6A:;}
# 479
if(preserve_types)new_e->topt=e->topt;
return new_e;}
# 483
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*e){
if(e == 0)return 0;else{
return Cyc_Tcutil_deep_copy_exp(preserve_types,e);}}struct _tuple20{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple21{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 496 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
enum Cyc_Absyn_KindQual _tmp1D5;enum Cyc_Absyn_AliasQual _tmp1D6;struct Cyc_Absyn_Kind*_tmp1D4=ka1;_tmp1D5=_tmp1D4->kind;_tmp1D6=_tmp1D4->aliasqual;{
enum Cyc_Absyn_KindQual _tmp1D8;enum Cyc_Absyn_AliasQual _tmp1D9;struct Cyc_Absyn_Kind*_tmp1D7=ka2;_tmp1D8=_tmp1D7->kind;_tmp1D9=_tmp1D7->aliasqual;
# 500
if(_tmp1D5 != _tmp1D8){
struct _tuple20 _tmpFE8;struct _tuple20 _tmp1DA=(_tmpFE8.f1=_tmp1D5,((_tmpFE8.f2=_tmp1D8,_tmpFE8)));struct _tuple20 _tmp1DB=_tmp1DA;_LLC1: if(_tmp1DB.f1 != Cyc_Absyn_BoxKind)goto _LLC3;if(_tmp1DB.f2 != Cyc_Absyn_MemKind)goto _LLC3;_LLC2:
 goto _LLC4;_LLC3: if(_tmp1DB.f1 != Cyc_Absyn_BoxKind)goto _LLC5;if(_tmp1DB.f2 != Cyc_Absyn_AnyKind)goto _LLC5;_LLC4:
 goto _LLC6;_LLC5: if(_tmp1DB.f1 != Cyc_Absyn_MemKind)goto _LLC7;if(_tmp1DB.f2 != Cyc_Absyn_AnyKind)goto _LLC7;_LLC6:
 goto _LLC0;_LLC7:;_LLC8:
 return 0;_LLC0:;}
# 509
if(_tmp1D6 != _tmp1D9){
struct _tuple21 _tmpFE9;struct _tuple21 _tmp1DD=(_tmpFE9.f1=_tmp1D6,((_tmpFE9.f2=_tmp1D9,_tmpFE9)));struct _tuple21 _tmp1DE=_tmp1DD;_LLCA: if(_tmp1DE.f1 != Cyc_Absyn_Aliasable)goto _LLCC;if(_tmp1DE.f2 != Cyc_Absyn_Top)goto _LLCC;_LLCB:
 goto _LLCD;_LLCC: if(_tmp1DE.f1 != Cyc_Absyn_Unique)goto _LLCE;if(_tmp1DE.f2 != Cyc_Absyn_Top)goto _LLCE;_LLCD:
 return 1;_LLCE:;_LLCF:
 return 0;_LLC9:;}
# 516
return 1;};}
# 519
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp1E0=Cyc_Absyn_compress_kb(tv->kind);void*_tmp1E1=_tmp1E0;struct Cyc_Absyn_Kind*_tmp1E3;struct Cyc_Absyn_Kind*_tmp1E5;_LLD1: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1E2=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1E1;if(_tmp1E2->tag != 0)goto _LLD3;else{_tmp1E3=_tmp1E2->f1;}}_LLD2:
 return _tmp1E3;_LLD3: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1E4=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1E1;if(_tmp1E4->tag != 2)goto _LLD5;else{_tmp1E5=_tmp1E4->f2;}}_LLD4:
 return _tmp1E5;_LLD5:;_LLD6:
# 524
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpFEC;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpFEB;tv->kind=(void*)((_tmpFEB=_cycalloc(sizeof(*_tmpFEB)),((_tmpFEB[0]=((_tmpFEC.tag=2,((_tmpFEC.f1=0,((_tmpFEC.f2=def,_tmpFEC)))))),_tmpFEB))));}
return def;_LLD0:;}
# 529
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
struct _tuple0 _tmpFED;struct _tuple0 _tmp1E8=(_tmpFED.f1=Cyc_Absyn_compress_kb(kb1),((_tmpFED.f2=Cyc_Absyn_compress_kb(kb2),_tmpFED)));struct _tuple0 _tmp1E9=_tmp1E8;struct Cyc_Absyn_Kind*_tmp1EB;struct Cyc_Absyn_Kind*_tmp1ED;struct Cyc_Core_Opt**_tmp1EF;struct Cyc_Absyn_Kind*_tmp1F0;struct Cyc_Absyn_Kind*_tmp1F2;struct Cyc_Absyn_Kind*_tmp1F4;struct Cyc_Core_Opt**_tmp1F6;struct Cyc_Absyn_Kind*_tmp1F7;struct Cyc_Core_Opt**_tmp1F9;struct Cyc_Absyn_Kind*_tmp1FA;struct Cyc_Core_Opt**_tmp1FC;struct Cyc_Absyn_Kind*_tmp1FD;struct Cyc_Core_Opt**_tmp1FF;void*_tmp200;void*_tmp201;struct Cyc_Core_Opt**_tmp203;_LLD8:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1EA=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1E9.f1;if(_tmp1EA->tag != 0)goto _LLDA;else{_tmp1EB=_tmp1EA->f1;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1EC=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1E9.f2;if(_tmp1EC->tag != 0)goto _LLDA;else{_tmp1ED=_tmp1EC->f1;}};_LLD9:
 return _tmp1EB == _tmp1ED;_LLDA:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1EE=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1E9.f1;if(_tmp1EE->tag != 2)goto _LLDC;else{_tmp1EF=(struct Cyc_Core_Opt**)& _tmp1EE->f1;_tmp1F0=_tmp1EE->f2;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1F1=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1E9.f2;if(_tmp1F1->tag != 0)goto _LLDC;else{_tmp1F2=_tmp1F1->f1;}};_LLDB:
# 533
 if(Cyc_Tcutil_kind_leq(_tmp1F2,_tmp1F0)){
{struct Cyc_Core_Opt*_tmpFEE;*_tmp1EF=((_tmpFEE=_cycalloc(sizeof(*_tmpFEE)),((_tmpFEE->v=kb2,_tmpFEE))));}
return 1;}else{
return 0;}_LLDC:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1F3=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1E9.f1;if(_tmp1F3->tag != 0)goto _LLDE;else{_tmp1F4=_tmp1F3->f1;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1F5=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1E9.f2;if(_tmp1F5->tag != 2)goto _LLDE;else{_tmp1F6=(struct Cyc_Core_Opt**)& _tmp1F5->f1;_tmp1F7=_tmp1F5->f2;}};_LLDD:
# 538
 if(Cyc_Tcutil_kind_leq(_tmp1F4,_tmp1F7)){
{struct Cyc_Core_Opt*_tmpFEF;*_tmp1F6=((_tmpFEF=_cycalloc(sizeof(*_tmpFEF)),((_tmpFEF->v=kb1,_tmpFEF))));}
return 1;}else{
return 0;}_LLDE:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1F8=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1E9.f1;if(_tmp1F8->tag != 2)goto _LLE0;else{_tmp1F9=(struct Cyc_Core_Opt**)& _tmp1F8->f1;_tmp1FA=_tmp1F8->f2;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1FB=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1E9.f2;if(_tmp1FB->tag != 2)goto _LLE0;else{_tmp1FC=(struct Cyc_Core_Opt**)& _tmp1FB->f1;_tmp1FD=_tmp1FB->f2;}};_LLDF:
# 543
 if(Cyc_Tcutil_kind_leq(_tmp1FA,_tmp1FD)){
{struct Cyc_Core_Opt*_tmpFF0;*_tmp1FC=((_tmpFF0=_cycalloc(sizeof(*_tmpFF0)),((_tmpFF0->v=kb1,_tmpFF0))));}
return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp1FD,_tmp1FA)){
{struct Cyc_Core_Opt*_tmpFF1;*_tmp1F9=((_tmpFF1=_cycalloc(sizeof(*_tmpFF1)),((_tmpFF1->v=kb2,_tmpFF1))));}
return 1;}else{
return 0;}}_LLE0:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp1FE=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1E9.f1;if(_tmp1FE->tag != 1)goto _LLE2;else{_tmp1FF=(struct Cyc_Core_Opt**)& _tmp1FE->f1;}}_tmp200=_tmp1E9.f2;_LLE1:
 _tmp201=_tmp200;_tmp203=_tmp1FF;goto _LLE3;_LLE2: _tmp201=_tmp1E9.f1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp202=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1E9.f2;if(_tmp202->tag != 1)goto _LLD7;else{_tmp203=(struct Cyc_Core_Opt**)& _tmp202->f1;}};_LLE3:
# 552
{struct Cyc_Core_Opt*_tmpFF2;*_tmp203=((_tmpFF2=_cycalloc(sizeof(*_tmpFF2)),((_tmpFF2->v=_tmp201,_tmpFF2))));}
return 1;_LLD7:;}
# 557
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp20A=Cyc_Tcutil_compress(t);void*_tmp20B=_tmp20A;struct Cyc_Absyn_Tvar*_tmp20D;_LLE5: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp20C=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp20B;if(_tmp20C->tag != 2)goto _LLE7;else{_tmp20D=_tmp20C->f1;}}_LLE6: {
# 560
void*_tmp20E=_tmp20D->kind;
_tmp20D->kind=kb;{
struct _tuple16 _tmpFF3;return(_tmpFF3.f1=_tmp20D,((_tmpFF3.f2=_tmp20E,_tmpFF3)));};}_LLE7:;_LLE8: {
# 564
const char*_tmpFF7;void*_tmpFF6[1];struct Cyc_String_pa_PrintArg_struct _tmpFF5;(_tmpFF5.tag=0,((_tmpFF5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpFF6[0]=& _tmpFF5,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFF7="swap_kind: cannot update the kind of %s",_tag_dyneither(_tmpFF7,sizeof(char),40))),_tag_dyneither(_tmpFF6,sizeof(void*),1)))))));}_LLE4:;}
# 570
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_typ,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 572
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_typ_kind(field_typ);
if(ts != 0  && (k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak)){
# 576
struct _RegionHandle _tmp213=_new_region("temp");struct _RegionHandle*temp=& _tmp213;_push_region(temp);
{struct Cyc_List_List*_tmp214=0;
# 579
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp215=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp216=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;
struct Cyc_Absyn_Kind*_tmp217=Cyc_Tcutil_tvar_kind(_tmp215,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp218=_tmp217;_LLEA: if(_tmp218->kind != Cyc_Absyn_IntKind)goto _LLEC;_LLEB:
 goto _LLED;_LLEC: if(_tmp218->kind != Cyc_Absyn_AnyKind)goto _LLEE;_LLED:
# 585
{struct _tuple16*_tmpFFA;struct Cyc_List_List*_tmpFF9;_tmp214=((_tmpFF9=_region_malloc(temp,sizeof(*_tmpFF9)),((_tmpFF9->hd=((_tmpFFA=_region_malloc(temp,sizeof(*_tmpFFA)),((_tmpFFA->f1=_tmp215,((_tmpFFA->f2=_tmp216,_tmpFFA)))))),((_tmpFF9->tl=_tmp214,_tmpFF9))))));}goto _LLE9;_LLEE:;_LLEF:
 goto _LLE9;_LLE9:;}
# 589
if(_tmp214 != 0){
field_typ=Cyc_Tcutil_rsubstitute(temp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp214),field_typ);
k=Cyc_Tcutil_typ_kind(field_typ);}}
# 577
;_pop_region(temp);}
# 594
return k;}
# 601
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t){
void*_tmp21B=Cyc_Tcutil_compress(t);void*_tmp21C=_tmp21B;struct Cyc_Core_Opt*_tmp21E;struct Cyc_Absyn_Tvar*_tmp220;enum Cyc_Absyn_Size_of _tmp223;struct Cyc_Absyn_Datatypedecl*_tmp22F;struct Cyc_Absyn_Datatypefield*_tmp230;enum Cyc_Absyn_AggrKind _tmp234;struct Cyc_List_List*_tmp235;struct Cyc_Absyn_AggrdeclImpl*_tmp236;int _tmp237;struct Cyc_List_List*_tmp238;struct Cyc_Absyn_PtrInfo _tmp23B;struct Cyc_Absyn_Kind*_tmp23E;struct Cyc_Absyn_Exp*_tmp242;struct Cyc_Absyn_Typedefdecl*_tmp245;_LLF1: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp21D=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp21C;if(_tmp21D->tag != 1)goto _LLF3;else{_tmp21E=_tmp21D->f1;}}_LLF2:
 return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp21E))->v;_LLF3: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp21F=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp21C;if(_tmp21F->tag != 2)goto _LLF5;else{_tmp220=_tmp21F->f1;}}_LLF4:
 return Cyc_Tcutil_tvar_kind(_tmp220,& Cyc_Tcutil_bk);_LLF5: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp221=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp21C;if(_tmp221->tag != 0)goto _LLF7;}_LLF6:
 return& Cyc_Tcutil_mk;_LLF7: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp222=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp21C;if(_tmp222->tag != 6)goto _LLF9;else{_tmp223=_tmp222->f2;}}_LLF8:
# 607
 return(_tmp223 == Cyc_Absyn_Int_sz  || _tmp223 == Cyc_Absyn_Long_sz)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;_LLF9: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp224=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp21C;if(_tmp224->tag != 7)goto _LLFB;}_LLFA:
# 609
 return& Cyc_Tcutil_mk;_LLFB: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp225=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp21C;if(_tmp225->tag != 9)goto _LLFD;}_LLFC:
 return& Cyc_Tcutil_ak;_LLFD: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp226=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp21C;if(_tmp226->tag != 16)goto _LLFF;}_LLFE:
 goto _LL100;_LLFF: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp227=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp21C;if(_tmp227->tag != 13)goto _LL101;}_LL100:
 goto _LL102;_LL101: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp228=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp21C;if(_tmp228->tag != 14)goto _LL103;}_LL102:
 goto _LL104;_LL103: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp229=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp21C;if(_tmp229->tag != 15)goto _LL105;}_LL104:
 return& Cyc_Tcutil_bk;_LL105: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp22A=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp21C;if(_tmp22A->tag != 21)goto _LL107;}_LL106:
 return& Cyc_Tcutil_urk;_LL107: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp22B=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp21C;if(_tmp22B->tag != 20)goto _LL109;}_LL108:
 return& Cyc_Tcutil_rk;_LL109: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp22C=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp21C;if(_tmp22C->tag != 22)goto _LL10B;}_LL10A:
 return& Cyc_Tcutil_trk;_LL10B: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp22D=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp21C;if(_tmp22D->tag != 3)goto _LL10D;}_LL10C:
# 620
 return& Cyc_Tcutil_ak;_LL10D: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp22E=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp21C;if(_tmp22E->tag != 4)goto _LL10F;else{if((((_tmp22E->f1).field_info).KnownDatatypefield).tag != 2)goto _LL10F;_tmp22F=((struct _tuple3)(((_tmp22E->f1).field_info).KnownDatatypefield).val).f1;_tmp230=((struct _tuple3)(((_tmp22E->f1).field_info).KnownDatatypefield).val).f2;}}_LL10E:
# 622
 return& Cyc_Tcutil_mk;_LL10F: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp231=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp21C;if(_tmp231->tag != 4)goto _LL111;else{if((((_tmp231->f1).field_info).UnknownDatatypefield).tag != 1)goto _LL111;}}_LL110: {
# 624
const char*_tmpFFD;void*_tmpFFC;(_tmpFFC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFFD="typ_kind: Unresolved DatatypeFieldType",_tag_dyneither(_tmpFFD,sizeof(char),39))),_tag_dyneither(_tmpFFC,sizeof(void*),0)));}_LL111: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp232=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp21C;if(_tmp232->tag != 11)goto _LL113;else{if((((_tmp232->f1).aggr_info).UnknownAggr).tag != 1)goto _LL113;}}_LL112:
# 627
 return& Cyc_Tcutil_ak;_LL113: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp233=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp21C;if(_tmp233->tag != 11)goto _LL115;else{if((((_tmp233->f1).aggr_info).KnownAggr).tag != 2)goto _LL115;_tmp234=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp233->f1).aggr_info).KnownAggr).val))->kind;_tmp235=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp233->f1).aggr_info).KnownAggr).val))->tvs;_tmp236=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp233->f1).aggr_info).KnownAggr).val))->impl;_tmp237=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp233->f1).aggr_info).KnownAggr).val))->expected_mem_kind;_tmp238=(_tmp233->f1).targs;}}_LL114:
# 629
 if(_tmp236 == 0){
if(_tmp237)
return& Cyc_Tcutil_mk;else{
# 633
return& Cyc_Tcutil_ak;}}{
# 635
struct Cyc_List_List*_tmp251=_tmp236->fields;
if(_tmp251 == 0)return& Cyc_Tcutil_mk;
# 638
if(_tmp234 == Cyc_Absyn_StructA){
for(0;_tmp251->tl != 0;_tmp251=_tmp251->tl){;}{
void*_tmp252=((struct Cyc_Absyn_Aggrfield*)_tmp251->hd)->type;
struct Cyc_Absyn_Kind*_tmp253=Cyc_Tcutil_field_kind(_tmp252,_tmp238,_tmp235);
if(_tmp253 == & Cyc_Tcutil_ak  || _tmp253 == & Cyc_Tcutil_tak)return _tmp253;};}else{
# 646
for(0;_tmp251 != 0;_tmp251=_tmp251->tl){
void*_tmp254=((struct Cyc_Absyn_Aggrfield*)_tmp251->hd)->type;
struct Cyc_Absyn_Kind*_tmp255=Cyc_Tcutil_field_kind(_tmp254,_tmp238,_tmp235);
if(_tmp255 == & Cyc_Tcutil_ak  || _tmp255 == & Cyc_Tcutil_tak)return _tmp255;}}
# 652
return& Cyc_Tcutil_mk;};_LL115: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp239=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp21C;if(_tmp239->tag != 12)goto _LL117;}_LL116:
 return& Cyc_Tcutil_mk;_LL117: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp23A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21C;if(_tmp23A->tag != 5)goto _LL119;else{_tmp23B=_tmp23A->f1;}}_LL118: {
# 655
void*_tmp256=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,(_tmp23B.ptr_atts).bounds);void*_tmp257=_tmp256;_LL134: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp258=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp257;if(_tmp258->tag != 0)goto _LL136;}_LL135: {
# 657
enum Cyc_Absyn_AliasQual _tmp25A=(Cyc_Tcutil_typ_kind((_tmp23B.ptr_atts).rgn))->aliasqual;switch(_tmp25A){case Cyc_Absyn_Aliasable: _LL138:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL139:
 return& Cyc_Tcutil_umk;case Cyc_Absyn_Top: _LL13A:
 return& Cyc_Tcutil_tmk;}}_LL136: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp259=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp257;if(_tmp259->tag != 1)goto _LL133;}_LL137: {
# 663
enum Cyc_Absyn_AliasQual _tmp25B=(Cyc_Tcutil_typ_kind((_tmp23B.ptr_atts).rgn))->aliasqual;switch(_tmp25B){case Cyc_Absyn_Aliasable: _LL13C:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL13D:
 return& Cyc_Tcutil_ubk;case Cyc_Absyn_Top: _LL13E:
 return& Cyc_Tcutil_tbk;}}_LL133:;}_LL119: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp23C=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp21C;if(_tmp23C->tag != 18)goto _LL11B;}_LL11A:
# 669
 return& Cyc_Tcutil_ik;_LL11B: {struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*_tmp23D=(struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp21C;if(_tmp23D->tag != 28)goto _LL11D;else{_tmp23E=_tmp23D->f2;}}_LL11C:
 return _tmp23E;_LL11D: {struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp23F=(struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp21C;if(_tmp23F->tag != 27)goto _LL11F;}_LL11E:
# 674
 return& Cyc_Tcutil_ak;_LL11F: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp240=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp21C;if(_tmp240->tag != 19)goto _LL121;}_LL120:
 return& Cyc_Tcutil_bk;_LL121: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp241=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp21C;if(_tmp241->tag != 8)goto _LL123;else{_tmp242=(_tmp241->f1).num_elts;}}_LL122:
# 677
 if(_tmp242 == 0  || Cyc_Tcutil_is_const_exp(_tmp242))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;_LL123: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp243=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp21C;if(_tmp243->tag != 10)goto _LL125;}_LL124:
 return& Cyc_Tcutil_mk;_LL125: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp244=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp21C;if(_tmp244->tag != 17)goto _LL127;else{_tmp245=_tmp244->f3;}}_LL126:
# 681
 if(_tmp245 == 0  || _tmp245->kind == 0){
const char*_tmp1001;void*_tmp1000[1];struct Cyc_String_pa_PrintArg_struct _tmpFFF;(_tmpFFF.tag=0,((_tmpFFF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1000[0]=& _tmpFFF,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1001="typ_kind: typedef found: %s",_tag_dyneither(_tmp1001,sizeof(char),28))),_tag_dyneither(_tmp1000,sizeof(void*),1)))))));}
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp245->kind))->v;_LL127: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp246=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp21C;if(_tmp246->tag != 26)goto _LL129;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp247=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp246->f1)->r;if(_tmp247->tag != 0)goto _LL129;}}_LL128:
 return& Cyc_Tcutil_ak;_LL129: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp248=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp21C;if(_tmp248->tag != 26)goto _LL12B;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp249=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp248->f1)->r;if(_tmp249->tag != 1)goto _LL12B;}}_LL12A:
 return& Cyc_Tcutil_bk;_LL12B: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp24A=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp21C;if(_tmp24A->tag != 26)goto _LL12D;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp24B=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp24A->f1)->r;if(_tmp24B->tag != 2)goto _LL12D;}}_LL12C:
 return& Cyc_Tcutil_ak;_LL12D: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp24C=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp21C;if(_tmp24C->tag != 23)goto _LL12F;}_LL12E:
 goto _LL130;_LL12F: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp24D=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp21C;if(_tmp24D->tag != 24)goto _LL131;}_LL130:
 goto _LL132;_LL131: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp24E=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp21C;if(_tmp24E->tag != 25)goto _LLF0;}_LL132:
 return& Cyc_Tcutil_ek;_LLF0:;}
# 693
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2  || k1->kind == k2->kind  && k1->aliasqual == k2->aliasqual;}
# 698
int Cyc_Tcutil_unify(void*t1,void*t2){
struct _handler_cons _tmp25F;_push_handler(& _tmp25F);{int _tmp261=0;if(setjmp(_tmp25F.handler))_tmp261=1;if(!_tmp261){
Cyc_Tcutil_unify_it(t1,t2);{
int _tmp262=1;_npop_handler(0);return _tmp262;};
# 700
;_pop_handler();}else{void*_tmp260=(void*)_exn_thrown;void*_tmp264=_tmp260;void*_tmp266;_LL141: {struct Cyc_Tcutil_Unify_exn_struct*_tmp265=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp264;if(_tmp265->tag != Cyc_Tcutil_Unify)goto _LL143;}_LL142:
# 702
 return 0;_LL143: _tmp266=_tmp264;_LL144:(void)_rethrow(_tmp266);_LL140:;}};}
# 707
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
t=Cyc_Tcutil_compress(t);{
void*_tmp267=t;struct Cyc_Absyn_Tvar*_tmp269;void*_tmp26B;struct Cyc_Core_Opt**_tmp26C;struct Cyc_Absyn_PtrInfo _tmp26E;void*_tmp270;struct Cyc_List_List*_tmp272;void*_tmp273;struct Cyc_Absyn_Tqual _tmp274;void*_tmp275;struct Cyc_List_List*_tmp276;int _tmp277;struct Cyc_Absyn_VarargInfo*_tmp278;struct Cyc_List_List*_tmp279;struct Cyc_List_List*_tmp27A;struct Cyc_Absyn_Exp*_tmp27B;struct Cyc_List_List*_tmp27C;struct Cyc_Absyn_Exp*_tmp27D;struct Cyc_List_List*_tmp27E;struct Cyc_List_List*_tmp280;struct Cyc_List_List*_tmp282;struct Cyc_List_List*_tmp284;struct Cyc_List_List*_tmp286;struct Cyc_List_List*_tmp288;struct Cyc_List_List*_tmp28A;void*_tmp28C;void*_tmp28E;void*_tmp290;void*_tmp292;struct Cyc_List_List*_tmp294;_LL146: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp268=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp267;if(_tmp268->tag != 2)goto _LL148;else{_tmp269=_tmp268->f1;}}_LL147:
# 712
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp269)){
{const char*_tmp1002;Cyc_Tcutil_failure_reason=((_tmp1002="(type variable would escape scope)",_tag_dyneither(_tmp1002,sizeof(char),35)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 716
goto _LL145;_LL148: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp26A=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp267;if(_tmp26A->tag != 1)goto _LL14A;else{_tmp26B=(void*)_tmp26A->f2;_tmp26C=(struct Cyc_Core_Opt**)& _tmp26A->f4;}}_LL149:
# 718
 if(t == evar){
{const char*_tmp1003;Cyc_Tcutil_failure_reason=((_tmp1003="(occurs check)",_tag_dyneither(_tmp1003,sizeof(char),15)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{
# 722
if(_tmp26B != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp26B);else{
# 725
int problem=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp26C))->v;for(0;s != 0;s=s->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}
# 732
if(problem){
struct Cyc_List_List*_tmp297=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp26C))->v;for(0;s != 0;s=s->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
struct Cyc_List_List*_tmp1004;_tmp297=((_tmp1004=_cycalloc(sizeof(*_tmp1004)),((_tmp1004->hd=(struct Cyc_Absyn_Tvar*)s->hd,((_tmp1004->tl=_tmp297,_tmp1004))))));}}}{
# 738
struct Cyc_Core_Opt*_tmp1005;*_tmp26C=((_tmp1005=_cycalloc(sizeof(*_tmp1005)),((_tmp1005->v=_tmp297,_tmp1005))));};}}}
# 741
goto _LL145;_LL14A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp26D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp267;if(_tmp26D->tag != 5)goto _LL14C;else{_tmp26E=_tmp26D->f1;}}_LL14B:
# 744
 Cyc_Tcutil_occurs(evar,r,env,_tmp26E.elt_typ);
Cyc_Tcutil_occurs(evar,r,env,(_tmp26E.ptr_atts).rgn);
goto _LL145;_LL14C: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp26F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp267;if(_tmp26F->tag != 8)goto _LL14E;else{_tmp270=(_tmp26F->f1).elt_type;}}_LL14D:
# 748
 Cyc_Tcutil_occurs(evar,r,env,_tmp270);goto _LL145;_LL14E: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp271=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp267;if(_tmp271->tag != 9)goto _LL150;else{_tmp272=(_tmp271->f1).tvars;_tmp273=(_tmp271->f1).effect;_tmp274=(_tmp271->f1).ret_tqual;_tmp275=(_tmp271->f1).ret_typ;_tmp276=(_tmp271->f1).args;_tmp277=(_tmp271->f1).c_varargs;_tmp278=(_tmp271->f1).cyc_varargs;_tmp279=(_tmp271->f1).rgn_po;_tmp27A=(_tmp271->f1).attributes;_tmp27B=(_tmp271->f1).requires_clause;_tmp27C=(_tmp271->f1).requires_relns;_tmp27D=(_tmp271->f1).ensures_clause;_tmp27E=(_tmp271->f1).ensures_relns;}}_LL14F:
# 751
 env=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp272,env);
if(_tmp273 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp273);
Cyc_Tcutil_occurs(evar,r,env,_tmp275);
for(0;_tmp276 != 0;_tmp276=_tmp276->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple10*)_tmp276->hd)).f3);}
if(_tmp278 != 0)
Cyc_Tcutil_occurs(evar,r,env,_tmp278->type);
for(0;_tmp279 != 0;_tmp279=_tmp279->tl){
struct _tuple0*_tmp29A=(struct _tuple0*)_tmp279->hd;void*_tmp29C;void*_tmp29D;struct _tuple0*_tmp29B=_tmp29A;_tmp29C=_tmp29B->f1;_tmp29D=_tmp29B->f2;
Cyc_Tcutil_occurs(evar,r,env,_tmp29C);
Cyc_Tcutil_occurs(evar,r,env,_tmp29D);}
# 763
goto _LL145;_LL150: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp27F=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp267;if(_tmp27F->tag != 10)goto _LL152;else{_tmp280=_tmp27F->f1;}}_LL151:
# 765
 for(0;_tmp280 != 0;_tmp280=_tmp280->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple12*)_tmp280->hd)).f2);}
goto _LL145;_LL152: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp281=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp267;if(_tmp281->tag != 3)goto _LL154;else{_tmp282=(_tmp281->f1).targs;}}_LL153:
# 769
 Cyc_Tcutil_occurslist(evar,r,env,_tmp282);goto _LL145;_LL154: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp283=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp267;if(_tmp283->tag != 17)goto _LL156;else{_tmp284=_tmp283->f2;}}_LL155:
 _tmp286=_tmp284;goto _LL157;_LL156: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp285=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp267;if(_tmp285->tag != 4)goto _LL158;else{_tmp286=(_tmp285->f1).targs;}}_LL157:
 _tmp288=_tmp286;goto _LL159;_LL158: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp287=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp267;if(_tmp287->tag != 11)goto _LL15A;else{_tmp288=(_tmp287->f1).targs;}}_LL159:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp288);goto _LL145;_LL15A: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp289=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp267;if(_tmp289->tag != 12)goto _LL15C;else{_tmp28A=_tmp289->f2;}}_LL15B:
# 775
 for(0;_tmp28A != 0;_tmp28A=_tmp28A->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp28A->hd)->type);}
goto _LL145;_LL15C: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp28B=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp267;if(_tmp28B->tag != 19)goto _LL15E;else{_tmp28C=(void*)_tmp28B->f1;}}_LL15D:
 _tmp28E=_tmp28C;goto _LL15F;_LL15E: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp28D=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp267;if(_tmp28D->tag != 23)goto _LL160;else{_tmp28E=(void*)_tmp28D->f1;}}_LL15F:
 _tmp290=_tmp28E;goto _LL161;_LL160: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp28F=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp267;if(_tmp28F->tag != 15)goto _LL162;else{_tmp290=(void*)_tmp28F->f1;}}_LL161:
 _tmp292=_tmp290;goto _LL163;_LL162: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp291=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp267;if(_tmp291->tag != 25)goto _LL164;else{_tmp292=(void*)_tmp291->f1;}}_LL163:
 Cyc_Tcutil_occurs(evar,r,env,_tmp292);goto _LL145;_LL164: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp293=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp267;if(_tmp293->tag != 24)goto _LL166;else{_tmp294=_tmp293->f1;}}_LL165:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp294);goto _LL145;_LL166:;_LL167:
# 785
 goto _LL145;_LL145:;};}
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
if(tq1.print_const  && !tq1.real_const){
const char*_tmp1008;void*_tmp1007;(_tmp1007=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1008="tq1 real_const not set.",_tag_dyneither(_tmp1008,sizeof(char),24))),_tag_dyneither(_tmp1007,sizeof(void*),0)));}
if(tq2.print_const  && !tq2.real_const){
const char*_tmp100B;void*_tmp100A;(_tmp100A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp100B="tq2 real_const not set.",_tag_dyneither(_tmp100B,sizeof(char),24))),_tag_dyneither(_tmp100A,sizeof(void*),0)));}
# 809
if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
# 812
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;
{const char*_tmp100C;Cyc_Tcutil_failure_reason=((_tmp100C="(qualifiers don't match)",_tag_dyneither(_tmp100C,sizeof(char),25)));}
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
union Cyc_Absyn_Constraint*_tmp2A3=x;void*_tmp2A4;_LL169: if((_tmp2A3->No_constr).tag != 3)goto _LL16B;_LL16A:
# 838
{union Cyc_Absyn_Constraint _tmp100D;*x=(((_tmp100D.Forward_constr).val=y,(((_tmp100D.Forward_constr).tag=2,_tmp100D))));}return;_LL16B: if((_tmp2A3->Eq_constr).tag != 1)goto _LL16D;_tmp2A4=(void*)(_tmp2A3->Eq_constr).val;_LL16C: {
# 840
union Cyc_Absyn_Constraint*_tmp2A6=y;void*_tmp2A7;_LL170: if((_tmp2A6->No_constr).tag != 3)goto _LL172;_LL171:
*y=*x;return;_LL172: if((_tmp2A6->Eq_constr).tag != 1)goto _LL174;_tmp2A7=(void*)(_tmp2A6->Eq_constr).val;_LL173:
# 843
 if(cmp(_tmp2A4,_tmp2A7)!= 0){
Cyc_Tcutil_failure_reason=reason;
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 847
return;_LL174: if((_tmp2A6->Forward_constr).tag != 2)goto _LL16F;_LL175: {
const char*_tmp1010;void*_tmp100F;(_tmp100F=0,Cyc_Tcutil_impos(((_tmp1010="unify_conref: forward after compress(2)",_tag_dyneither(_tmp1010,sizeof(char),40))),_tag_dyneither(_tmp100F,sizeof(void*),0)));}_LL16F:;}_LL16D: if((_tmp2A3->Forward_constr).tag != 2)goto _LL168;_LL16E: {
# 850
const char*_tmp1013;void*_tmp1012;(_tmp1012=0,Cyc_Tcutil_impos(((_tmp1013="unify_conref: forward after compress",_tag_dyneither(_tmp1013,sizeof(char),37))),_tag_dyneither(_tmp1012,sizeof(void*),0)));}_LL168:;};}
# 854
static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
struct _handler_cons _tmp2AC;_push_handler(& _tmp2AC);{int _tmp2AE=0;if(setjmp(_tmp2AC.handler))_tmp2AE=1;if(!_tmp2AE){
Cyc_Tcutil_unify_it_conrefs(cmp,x,y,_tag_dyneither(0,0,0));{
int _tmp2AF=1;_npop_handler(0);return _tmp2AF;};
# 856
;_pop_handler();}else{void*_tmp2AD=(void*)_exn_thrown;void*_tmp2B1=_tmp2AD;void*_tmp2B3;_LL177: {struct Cyc_Tcutil_Unify_exn_struct*_tmp2B2=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp2B1;if(_tmp2B2->tag != Cyc_Tcutil_Unify)goto _LL179;}_LL178:
# 858
 return 0;_LL179: _tmp2B3=_tmp2B1;_LL17A:(void)_rethrow(_tmp2B3);_LL176:;}};}
# 861
static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmp1014;struct _tuple0 _tmp2B4=(_tmp1014.f1=b1,((_tmp1014.f2=b2,_tmp1014)));struct _tuple0 _tmp2B5=_tmp2B4;struct Cyc_Absyn_Exp*_tmp2BB;struct Cyc_Absyn_Exp*_tmp2BD;_LL17C:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2B6=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2B5.f1;if(_tmp2B6->tag != 0)goto _LL17E;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2B7=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2B5.f2;if(_tmp2B7->tag != 0)goto _LL17E;};_LL17D:
 return 0;_LL17E: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2B8=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2B5.f1;if(_tmp2B8->tag != 0)goto _LL180;}_LL17F:
 return - 1;_LL180: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2B9=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2B5.f2;if(_tmp2B9->tag != 0)goto _LL182;}_LL181:
 return 1;_LL182:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2BA=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2B5.f1;if(_tmp2BA->tag != 1)goto _LL17B;else{_tmp2BB=_tmp2BA->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2BC=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2B5.f2;if(_tmp2BC->tag != 1)goto _LL17B;else{_tmp2BD=_tmp2BC->f1;}};_LL183:
# 867
 return Cyc_Evexp_const_exp_cmp(_tmp2BB,_tmp2BD);_LL17B:;}
# 871
static void Cyc_Tcutil_unify_cmp_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
{struct _tuple0 _tmp1015;struct _tuple0 _tmp2BF=(_tmp1015.f1=e1->r,((_tmp1015.f2=e2->r,_tmp1015)));struct _tuple0 _tmp2C0=_tmp2BF;struct Cyc_Absyn_Exp*_tmp2C2;struct Cyc_Absyn_Exp*_tmp2C4;void*_tmp2C6;void*_tmp2C8;_LL185: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp2C1=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2C0.f1;if(_tmp2C1->tag != 13)goto _LL187;else{_tmp2C2=_tmp2C1->f2;}}_LL186:
# 877
 Cyc_Tcutil_unify_cmp_exp(_tmp2C2,e2);
return;_LL187: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp2C3=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2C0.f2;if(_tmp2C3->tag != 13)goto _LL189;else{_tmp2C4=_tmp2C3->f2;}}_LL188:
# 880
 Cyc_Tcutil_unify_cmp_exp(e1,_tmp2C4);
return;_LL189: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp2C5=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp2C0.f1;if(_tmp2C5->tag != 38)goto _LL18B;else{_tmp2C6=(void*)_tmp2C5->f1;}}_LL18A:
# 883
{void*_tmp2C9=Cyc_Tcutil_compress(_tmp2C6);void*_tmp2CA=_tmp2C9;_LL190: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp2CB=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2CA;if(_tmp2CB->tag != 1)goto _LL192;}_LL191:
# 885
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp1018;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1017;Cyc_Tcutil_unify_it(_tmp2C6,(void*)((_tmp1017=_cycalloc(sizeof(*_tmp1017)),((_tmp1017[0]=((_tmp1018.tag=18,((_tmp1018.f1=e2,_tmp1018)))),_tmp1017)))));}
return;_LL192:;_LL193:
# 888
 goto _LL18F;_LL18F:;}
# 890
goto _LL184;_LL18B: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp2C7=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp2C0.f2;if(_tmp2C7->tag != 38)goto _LL18D;else{_tmp2C8=(void*)_tmp2C7->f1;}}_LL18C:
# 892
{void*_tmp2CE=Cyc_Tcutil_compress(_tmp2C8);void*_tmp2CF=_tmp2CE;_LL195: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp2D0=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2CF;if(_tmp2D0->tag != 1)goto _LL197;}_LL196:
# 894
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp101B;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp101A;Cyc_Tcutil_unify_it(_tmp2C8,(void*)((_tmp101A=_cycalloc(sizeof(*_tmp101A)),((_tmp101A[0]=((_tmp101B.tag=18,((_tmp101B.f1=e1,_tmp101B)))),_tmp101A)))));}
return;_LL197:;_LL198:
# 897
 goto _LL194;_LL194:;}
# 899
goto _LL184;_LL18D:;_LL18E:
 goto _LL184;_LL184:;}
# 902
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 905
static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){
struct _tuple0 _tmp101C;struct _tuple0 _tmp2D4=(_tmp101C.f1=b1,((_tmp101C.f2=b2,_tmp101C)));struct _tuple0 _tmp2D5=_tmp2D4;struct Cyc_Absyn_Exp*_tmp2DB;struct Cyc_Absyn_Exp*_tmp2DD;_LL19A:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2D6=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2D5.f1;if(_tmp2D6->tag != 0)goto _LL19C;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2D7=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2D5.f2;if(_tmp2D7->tag != 0)goto _LL19C;};_LL19B:
 return 0;_LL19C: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2D8=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2D5.f1;if(_tmp2D8->tag != 0)goto _LL19E;}_LL19D:
 return - 1;_LL19E: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2D9=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2D5.f2;if(_tmp2D9->tag != 0)goto _LL1A0;}_LL19F:
 return 1;_LL1A0:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2DA=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2D5.f1;if(_tmp2DA->tag != 1)goto _LL199;else{_tmp2DB=_tmp2DA->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2DC=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2D5.f2;if(_tmp2DC->tag != 1)goto _LL199;else{_tmp2DD=_tmp2DC->f1;}};_LL1A1: {
# 911
int r=Cyc_Evexp_const_exp_cmp(_tmp2DB,_tmp2DD);
if(r == 0)return 0;{
struct _handler_cons _tmp2DE;_push_handler(& _tmp2DE);{int _tmp2E0=0;if(setjmp(_tmp2DE.handler))_tmp2E0=1;if(!_tmp2E0){
Cyc_Tcutil_unify_cmp_exp(_tmp2DB,_tmp2DD);{
int _tmp2E1=0;_npop_handler(0);return _tmp2E1;};
# 914
;_pop_handler();}else{void*_tmp2DF=(void*)_exn_thrown;void*_tmp2E3=_tmp2DF;void*_tmp2E5;_LL1A3: {struct Cyc_Tcutil_Unify_exn_struct*_tmp2E4=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp2E3;if(_tmp2E4->tag != Cyc_Tcutil_Unify)goto _LL1A5;}_LL1A4:
# 917
 return r;_LL1A5: _tmp2E5=_tmp2E3;_LL1A6:(void)_rethrow(_tmp2E5);_LL1A2:;}};};}_LL199:;}
# 922
static int Cyc_Tcutil_attribute_case_number(void*att){
void*_tmp2E7=att;_LL1A8: {struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2E8=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2E7;if(_tmp2E8->tag != 0)goto _LL1AA;}_LL1A9:
 return 0;_LL1AA: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp2E9=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp2E7;if(_tmp2E9->tag != 1)goto _LL1AC;}_LL1AB:
 return 1;_LL1AC: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp2EA=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp2E7;if(_tmp2EA->tag != 2)goto _LL1AE;}_LL1AD:
 return 2;_LL1AE: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp2EB=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp2E7;if(_tmp2EB->tag != 3)goto _LL1B0;}_LL1AF:
 return 3;_LL1B0: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp2EC=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp2E7;if(_tmp2EC->tag != 4)goto _LL1B2;}_LL1B1:
 return 4;_LL1B2: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp2ED=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp2E7;if(_tmp2ED->tag != 5)goto _LL1B4;}_LL1B3:
 return 5;_LL1B4: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2EE=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2E7;if(_tmp2EE->tag != 6)goto _LL1B6;}_LL1B5:
 return 6;_LL1B6: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp2EF=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp2E7;if(_tmp2EF->tag != 7)goto _LL1B8;}_LL1B7:
 return 7;_LL1B8: {struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2F0=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2E7;if(_tmp2F0->tag != 8)goto _LL1BA;}_LL1B9:
 return 8;_LL1BA: {struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*_tmp2F1=(struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*)_tmp2E7;if(_tmp2F1->tag != 9)goto _LL1BC;}_LL1BB:
 return 9;_LL1BC: {struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*_tmp2F2=(struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*)_tmp2E7;if(_tmp2F2->tag != 10)goto _LL1BE;}_LL1BD:
 return 10;_LL1BE: {struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*_tmp2F3=(struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*)_tmp2E7;if(_tmp2F3->tag != 11)goto _LL1C0;}_LL1BF:
 return 11;_LL1C0: {struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*_tmp2F4=(struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*)_tmp2E7;if(_tmp2F4->tag != 12)goto _LL1C2;}_LL1C1:
 return 12;_LL1C2: {struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*_tmp2F5=(struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*)_tmp2E7;if(_tmp2F5->tag != 13)goto _LL1C4;}_LL1C3:
 return 13;_LL1C4: {struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*_tmp2F6=(struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*)_tmp2E7;if(_tmp2F6->tag != 14)goto _LL1C6;}_LL1C5:
 return 14;_LL1C6: {struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*_tmp2F7=(struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*)_tmp2E7;if(_tmp2F7->tag != 15)goto _LL1C8;}_LL1C7:
 return 15;_LL1C8: {struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*_tmp2F8=(struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*)_tmp2E7;if(_tmp2F8->tag != 16)goto _LL1CA;}_LL1C9:
 return 16;_LL1CA: {struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*_tmp2F9=(struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*)_tmp2E7;if(_tmp2F9->tag != 17)goto _LL1CC;}_LL1CB:
 return 17;_LL1CC: {struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*_tmp2FA=(struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*)_tmp2E7;if(_tmp2FA->tag != 18)goto _LL1CE;}_LL1CD:
 return 18;_LL1CE: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp2FB=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2E7;if(_tmp2FB->tag != 19)goto _LL1D0;}_LL1CF:
 return 19;_LL1D0: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2FC=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2E7;if(_tmp2FC->tag != 20)goto _LL1D2;}_LL1D1:
 return 20;_LL1D2:;_LL1D3:
 return 21;_LL1A7:;}
# 949
static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple0 _tmp101D;struct _tuple0 _tmp2FD=(_tmp101D.f1=att1,((_tmp101D.f2=att2,_tmp101D)));struct _tuple0 _tmp2FE=_tmp2FD;int _tmp300;int _tmp302;int _tmp304;int _tmp306;struct Cyc_Absyn_Exp*_tmp308;struct Cyc_Absyn_Exp*_tmp30A;struct _dyneither_ptr _tmp30C;struct _dyneither_ptr _tmp30E;enum Cyc_Absyn_Format_Type _tmp310;int _tmp311;int _tmp312;enum Cyc_Absyn_Format_Type _tmp314;int _tmp315;int _tmp316;_LL1D5:{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2FF=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2FE.f1;if(_tmp2FF->tag != 0)goto _LL1D7;else{_tmp300=_tmp2FF->f1;}}{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp301=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2FE.f2;if(_tmp301->tag != 0)goto _LL1D7;else{_tmp302=_tmp301->f1;}};_LL1D6:
 _tmp304=_tmp300;_tmp306=_tmp302;goto _LL1D8;_LL1D7:{struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp303=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2FE.f1;if(_tmp303->tag != 20)goto _LL1D9;else{_tmp304=_tmp303->f1;}}{struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp305=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2FE.f2;if(_tmp305->tag != 20)goto _LL1D9;else{_tmp306=_tmp305->f1;}};_LL1D8:
# 953
 return Cyc_Core_intcmp(_tmp304,_tmp306);_LL1D9:{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp307=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2FE.f1;if(_tmp307->tag != 6)goto _LL1DB;else{_tmp308=_tmp307->f1;}}{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp309=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2FE.f2;if(_tmp309->tag != 6)goto _LL1DB;else{_tmp30A=_tmp309->f1;}};_LL1DA:
# 955
 if(_tmp308 == _tmp30A)return 0;
if(_tmp308 == 0)return - 1;
if(_tmp30A == 0)return 1;
return Cyc_Evexp_const_exp_cmp(_tmp308,_tmp30A);_LL1DB:{struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp30B=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2FE.f1;if(_tmp30B->tag != 8)goto _LL1DD;else{_tmp30C=_tmp30B->f1;}}{struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp30D=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2FE.f2;if(_tmp30D->tag != 8)goto _LL1DD;else{_tmp30E=_tmp30D->f1;}};_LL1DC:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp30C,(struct _dyneither_ptr)_tmp30E);_LL1DD:{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp30F=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2FE.f1;if(_tmp30F->tag != 19)goto _LL1DF;else{_tmp310=_tmp30F->f1;_tmp311=_tmp30F->f2;_tmp312=_tmp30F->f3;}}{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp313=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2FE.f2;if(_tmp313->tag != 19)goto _LL1DF;else{_tmp314=_tmp313->f1;_tmp315=_tmp313->f2;_tmp316=_tmp313->f3;}};_LL1DE: {
# 961
int _tmp317=Cyc_Core_intcmp((int)((unsigned int)_tmp310),(int)((unsigned int)_tmp314));
if(_tmp317 != 0)return _tmp317;{
int _tmp318=Cyc_Core_intcmp(_tmp311,_tmp315);
if(_tmp318 != 0)return _tmp318;
return Cyc_Core_intcmp(_tmp312,_tmp316);};}_LL1DF:;_LL1E0:
# 967
 return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(att2));_LL1D4:;}
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
{struct Cyc_Absyn_Kind*_tmp31A=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp31B=_tmp31A;_LL1E2: if(_tmp31B->kind != Cyc_Absyn_RgnKind)goto _LL1E4;if(_tmp31B->aliasqual != Cyc_Absyn_Unique)goto _LL1E4;_LL1E3:
 t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL1E1;_LL1E4: if(_tmp31B->kind != Cyc_Absyn_RgnKind)goto _LL1E6;if(_tmp31B->aliasqual != Cyc_Absyn_Aliasable)goto _LL1E6;_LL1E5:
 t=(void*)& Cyc_Absyn_HeapRgn_val;goto _LL1E1;_LL1E6: if(_tmp31B->kind != Cyc_Absyn_EffKind)goto _LL1E8;_LL1E7:
 t=Cyc_Absyn_empty_effect;goto _LL1E1;_LL1E8: if(_tmp31B->kind != Cyc_Absyn_IntKind)goto _LL1EA;_LL1E9:
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp1020;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp101F;t=(void*)((_tmp101F=_cycalloc(sizeof(*_tmp101F)),((_tmp101F[0]=((_tmp1020.tag=18,((_tmp1020.f1=Cyc_Absyn_uint_exp(0,0),_tmp1020)))),_tmp101F))));}goto _LL1E1;_LL1EA:;_LL1EB:
 t=Cyc_Absyn_sint_typ;goto _LL1E1;_LL1E1:;}{
# 999
struct _tuple16*_tmp1021;return(_tmp1021=_cycalloc(sizeof(*_tmp1021)),((_tmp1021->f1=tv,((_tmp1021->f2=t,_tmp1021)))));};}
# 1006
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp31F=Cyc_Tcutil_compress(t);void*_tmp320=_tmp31F;void*_tmp32C;void*_tmp32E;void*_tmp32F;struct Cyc_List_List*_tmp331;void*_tmp333;void*_tmp334;void*_tmp336;struct Cyc_List_List*_tmp338;struct Cyc_List_List*_tmp33A;struct Cyc_List_List*_tmp33C;struct Cyc_List_List*_tmp33E;struct Cyc_List_List*_tmp341;void*_tmp342;struct Cyc_Absyn_Tqual _tmp343;void*_tmp344;struct Cyc_List_List*_tmp345;struct Cyc_Absyn_VarargInfo*_tmp346;struct Cyc_List_List*_tmp347;void*_tmp34E;struct Cyc_List_List*_tmp350;_LL1ED: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp321=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp320;if(_tmp321->tag != 0)goto _LL1EF;}_LL1EE:
 goto _LL1F0;_LL1EF: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp322=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp320;if(_tmp322->tag != 7)goto _LL1F1;}_LL1F0:
 goto _LL1F2;_LL1F1: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp323=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp320;if(_tmp323->tag != 13)goto _LL1F3;}_LL1F2:
 goto _LL1F4;_LL1F3: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp324=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp320;if(_tmp324->tag != 14)goto _LL1F5;}_LL1F4:
 goto _LL1F6;_LL1F5: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp325=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp320;if(_tmp325->tag != 18)goto _LL1F7;}_LL1F6:
 goto _LL1F8;_LL1F7: {struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp326=(struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp320;if(_tmp326->tag != 27)goto _LL1F9;}_LL1F8:
 goto _LL1FA;_LL1F9: {struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*_tmp327=(struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp320;if(_tmp327->tag != 28)goto _LL1FB;}_LL1FA:
 goto _LL1FC;_LL1FB: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp328=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp320;if(_tmp328->tag != 6)goto _LL1FD;}_LL1FC:
 return Cyc_Absyn_empty_effect;_LL1FD: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp329=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp320;if(_tmp329->tag != 1)goto _LL1FF;}_LL1FE:
 goto _LL200;_LL1FF: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp32A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp320;if(_tmp32A->tag != 2)goto _LL201;}_LL200: {
# 1018
struct Cyc_Absyn_Kind*_tmp352=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp353=_tmp352;_LL228: if(_tmp353->kind != Cyc_Absyn_RgnKind)goto _LL22A;_LL229: {
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1024;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1023;return(void*)((_tmp1023=_cycalloc(sizeof(*_tmp1023)),((_tmp1023[0]=((_tmp1024.tag=23,((_tmp1024.f1=t,_tmp1024)))),_tmp1023))));}_LL22A: if(_tmp353->kind != Cyc_Absyn_EffKind)goto _LL22C;_LL22B:
 return t;_LL22C: if(_tmp353->kind != Cyc_Absyn_IntKind)goto _LL22E;_LL22D:
 return Cyc_Absyn_empty_effect;_LL22E:;_LL22F: {
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp1027;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1026;return(void*)((_tmp1026=_cycalloc(sizeof(*_tmp1026)),((_tmp1026[0]=((_tmp1027.tag=25,((_tmp1027.f1=t,_tmp1027)))),_tmp1026))));}_LL227:;}_LL201: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp32B=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp320;if(_tmp32B->tag != 15)goto _LL203;else{_tmp32C=(void*)_tmp32B->f1;}}_LL202: {
# 1024
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp102A;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1029;return(void*)((_tmp1029=_cycalloc(sizeof(*_tmp1029)),((_tmp1029[0]=((_tmp102A.tag=23,((_tmp102A.f1=_tmp32C,_tmp102A)))),_tmp1029))));}_LL203: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp32D=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp320;if(_tmp32D->tag != 16)goto _LL205;else{_tmp32E=(void*)_tmp32D->f1;_tmp32F=(void*)_tmp32D->f2;}}_LL204: {
# 1028
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp102D;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp102C;return(void*)((_tmp102C=_cycalloc(sizeof(*_tmp102C)),((_tmp102C[0]=((_tmp102D.tag=23,((_tmp102D.f1=_tmp32F,_tmp102D)))),_tmp102C))));}_LL205: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp330=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp320;if(_tmp330->tag != 3)goto _LL207;else{_tmp331=(_tmp330->f1).targs;}}_LL206: {
# 1030
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp331);
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1030;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp102F;return Cyc_Tcutil_normalize_effect((void*)((_tmp102F=_cycalloc(sizeof(*_tmp102F)),((_tmp102F[0]=((_tmp1030.tag=24,((_tmp1030.f1=ts,_tmp1030)))),_tmp102F)))));}_LL207: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp332=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp320;if(_tmp332->tag != 5)goto _LL209;else{_tmp333=(_tmp332->f1).elt_typ;_tmp334=((_tmp332->f1).ptr_atts).rgn;}}_LL208: {
# 1034
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp103F;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp103E;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp103D;void*_tmp103C[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp103B;return Cyc_Tcutil_normalize_effect((void*)((_tmp103B=_cycalloc(sizeof(*_tmp103B)),((_tmp103B[0]=((_tmp103F.tag=24,((_tmp103F.f1=((_tmp103C[1]=Cyc_Tcutil_rgns_of(_tmp333),((_tmp103C[0]=(void*)((_tmp103E=_cycalloc(sizeof(*_tmp103E)),((_tmp103E[0]=((_tmp103D.tag=23,((_tmp103D.f1=_tmp334,_tmp103D)))),_tmp103E)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp103C,sizeof(void*),2)))))),_tmp103F)))),_tmp103B)))));}_LL209: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp335=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp320;if(_tmp335->tag != 8)goto _LL20B;else{_tmp336=(_tmp335->f1).elt_type;}}_LL20A:
# 1036
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp336));_LL20B: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp337=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp320;if(_tmp337->tag != 10)goto _LL20D;else{_tmp338=_tmp337->f1;}}_LL20C: {
# 1038
struct Cyc_List_List*_tmp363=0;
for(0;_tmp338 != 0;_tmp338=_tmp338->tl){
struct Cyc_List_List*_tmp1040;_tmp363=((_tmp1040=_cycalloc(sizeof(*_tmp1040)),((_tmp1040->hd=(*((struct _tuple12*)_tmp338->hd)).f2,((_tmp1040->tl=_tmp363,_tmp1040))))));}
_tmp33A=_tmp363;goto _LL20E;}_LL20D: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp339=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp320;if(_tmp339->tag != 4)goto _LL20F;else{_tmp33A=(_tmp339->f1).targs;}}_LL20E:
 _tmp33C=_tmp33A;goto _LL210;_LL20F: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp33B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp320;if(_tmp33B->tag != 11)goto _LL211;else{_tmp33C=(_tmp33B->f1).targs;}}_LL210: {
# 1044
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1043;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1042;return Cyc_Tcutil_normalize_effect((void*)((_tmp1042=_cycalloc(sizeof(*_tmp1042)),((_tmp1042[0]=((_tmp1043.tag=24,((_tmp1043.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp33C),_tmp1043)))),_tmp1042)))));}_LL211: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp33D=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp320;if(_tmp33D->tag != 12)goto _LL213;else{_tmp33E=_tmp33D->f2;}}_LL212: {
# 1046
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1046;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1045;return Cyc_Tcutil_normalize_effect((void*)((_tmp1045=_cycalloc(sizeof(*_tmp1045)),((_tmp1045[0]=((_tmp1046.tag=24,((_tmp1046.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp33E),_tmp1046)))),_tmp1045)))));}_LL213: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp33F=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp320;if(_tmp33F->tag != 19)goto _LL215;}_LL214:
 return Cyc_Absyn_empty_effect;_LL215: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp340=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp320;if(_tmp340->tag != 9)goto _LL217;else{_tmp341=(_tmp340->f1).tvars;_tmp342=(_tmp340->f1).effect;_tmp343=(_tmp340->f1).ret_tqual;_tmp344=(_tmp340->f1).ret_typ;_tmp345=(_tmp340->f1).args;_tmp346=(_tmp340->f1).cyc_varargs;_tmp347=(_tmp340->f1).rgn_po;}}_LL216: {
# 1056
void*_tmp369=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp341),(void*)_check_null(_tmp342));
return Cyc_Tcutil_normalize_effect(_tmp369);}_LL217: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp348=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp320;if(_tmp348->tag != 21)goto _LL219;}_LL218:
 goto _LL21A;_LL219: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp349=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp320;if(_tmp349->tag != 22)goto _LL21B;}_LL21A:
 goto _LL21C;_LL21B: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp34A=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp320;if(_tmp34A->tag != 20)goto _LL21D;}_LL21C:
 return Cyc_Absyn_empty_effect;_LL21D: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp34B=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp320;if(_tmp34B->tag != 23)goto _LL21F;}_LL21E:
 goto _LL220;_LL21F: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp34C=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp320;if(_tmp34C->tag != 24)goto _LL221;}_LL220:
 return t;_LL221: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp34D=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp320;if(_tmp34D->tag != 25)goto _LL223;else{_tmp34E=(void*)_tmp34D->f1;}}_LL222:
 return Cyc_Tcutil_rgns_of(_tmp34E);_LL223: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp34F=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp320;if(_tmp34F->tag != 17)goto _LL225;else{_tmp350=_tmp34F->f2;}}_LL224: {
# 1065
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1049;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1048;return Cyc_Tcutil_normalize_effect((void*)((_tmp1048=_cycalloc(sizeof(*_tmp1048)),((_tmp1048[0]=((_tmp1049.tag=24,((_tmp1049.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp350),_tmp1049)))),_tmp1048)))));}_LL225: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp351=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp320;if(_tmp351->tag != 26)goto _LL1EC;}_LL226: {
const char*_tmp104C;void*_tmp104B;(_tmp104B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp104C="typedecl in rgns_of",_tag_dyneither(_tmp104C,sizeof(char),20))),_tag_dyneither(_tmp104B,sizeof(void*),0)));}_LL1EC:;}
# 1073
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp36E=e;struct Cyc_List_List**_tmp370;void*_tmp372;_LL231: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp36F=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp36E;if(_tmp36F->tag != 24)goto _LL233;else{_tmp370=(struct Cyc_List_List**)& _tmp36F->f1;}}_LL232: {
# 1077
int redo_join=0;
{struct Cyc_List_List*effs=*_tmp370;for(0;effs != 0;effs=effs->tl){
void*_tmp373=(void*)effs->hd;
effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp373));{
void*_tmp374=(void*)effs->hd;void*_tmp375=_tmp374;_LL238: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp376=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp375;if(_tmp376->tag != 24)goto _LL23A;}_LL239:
 goto _LL23B;_LL23A: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp377=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp375;if(_tmp377->tag != 23)goto _LL23C;else{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp378=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)((void*)_tmp377->f1);if(_tmp378->tag != 20)goto _LL23C;}}_LL23B:
 redo_join=1;goto _LL237;_LL23C: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp379=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp375;if(_tmp379->tag != 23)goto _LL23E;else{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp37A=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)((void*)_tmp379->f1);if(_tmp37A->tag != 22)goto _LL23E;}}_LL23D:
 redo_join=1;goto _LL237;_LL23E: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp37B=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp375;if(_tmp37B->tag != 23)goto _LL240;else{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp37C=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)((void*)_tmp37B->f1);if(_tmp37C->tag != 21)goto _LL240;}}_LL23F:
 redo_join=1;goto _LL237;_LL240:;_LL241:
 goto _LL237;_LL237:;};}}
# 1089
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*_tmp370;for(0;effs != 0;effs=effs->tl){
void*_tmp37D=Cyc_Tcutil_compress((void*)effs->hd);void*_tmp37E=_tmp37D;struct Cyc_List_List*_tmp380;void*_tmp387;_LL243: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp37F=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp37E;if(_tmp37F->tag != 24)goto _LL245;else{_tmp380=_tmp37F->f1;}}_LL244:
# 1094
 effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp380,effects);
goto _LL242;_LL245: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp381=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp37E;if(_tmp381->tag != 23)goto _LL247;else{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp382=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)((void*)_tmp381->f1);if(_tmp382->tag != 20)goto _LL247;}}_LL246:
 goto _LL242;_LL247: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp383=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp37E;if(_tmp383->tag != 23)goto _LL249;else{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp384=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)((void*)_tmp383->f1);if(_tmp384->tag != 22)goto _LL249;}}_LL248:
 goto _LL242;_LL249: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp385=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp37E;if(_tmp385->tag != 23)goto _LL24B;else{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp386=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)((void*)_tmp385->f1);if(_tmp386->tag != 21)goto _LL24B;}}_LL24A:
 goto _LL242;_LL24B: _tmp387=_tmp37E;_LL24C:
{struct Cyc_List_List*_tmp104D;effects=((_tmp104D=_cycalloc(sizeof(*_tmp104D)),((_tmp104D->hd=_tmp387,((_tmp104D->tl=effects,_tmp104D))))));}goto _LL242;_LL242:;}}
# 1102
*_tmp370=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
return e;};}_LL233: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp371=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp36E;if(_tmp371->tag != 25)goto _LL235;else{_tmp372=(void*)_tmp371->f1;}}_LL234: {
# 1105
void*_tmp389=Cyc_Tcutil_compress(_tmp372);void*_tmp38A=_tmp389;_LL24E: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp38B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp38A;if(_tmp38B->tag != 1)goto _LL250;}_LL24F:
 goto _LL251;_LL250: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp38C=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp38A;if(_tmp38C->tag != 2)goto _LL252;}_LL251:
 return e;_LL252:;_LL253:
 return Cyc_Tcutil_rgns_of(_tmp372);_LL24D:;}_LL235:;_LL236:
# 1110
 return e;_LL230:;};}
# 1115
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1053;struct Cyc_Absyn_FnInfo _tmp1052;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1051;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp38D=(_tmp1051=_cycalloc(sizeof(*_tmp1051)),((_tmp1051[0]=((_tmp1053.tag=9,((_tmp1053.f1=((_tmp1052.tvars=0,((_tmp1052.effect=eff,((_tmp1052.ret_tqual=
Cyc_Absyn_empty_tqual(0),((_tmp1052.ret_typ=(void*)& Cyc_Absyn_VoidType_val,((_tmp1052.args=0,((_tmp1052.c_varargs=0,((_tmp1052.cyc_varargs=0,((_tmp1052.rgn_po=0,((_tmp1052.attributes=0,((_tmp1052.requires_clause=0,((_tmp1052.requires_relns=0,((_tmp1052.ensures_clause=0,((_tmp1052.ensures_relns=0,_tmp1052)))))))))))))))))))))))))),_tmp1053)))),_tmp1051)));
# 1125
return Cyc_Absyn_atb_typ((void*)_tmp38D,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}
# 1132
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);
if((r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (void*)& Cyc_Absyn_UniqueRgn_val) || r == (void*)& Cyc_Absyn_RefCntRgn_val)
return 1;{
void*_tmp391=Cyc_Tcutil_compress(e);void*_tmp392=_tmp391;void*_tmp394;struct Cyc_List_List*_tmp396;void*_tmp398;struct Cyc_Core_Opt*_tmp39A;void**_tmp39B;struct Cyc_Core_Opt*_tmp39C;_LL255: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp393=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp392;if(_tmp393->tag != 23)goto _LL257;else{_tmp394=(void*)_tmp393->f1;}}_LL256:
# 1139
 if(constrain)return Cyc_Tcutil_unify(r,_tmp394);
_tmp394=Cyc_Tcutil_compress(_tmp394);
if(r == _tmp394)return 1;{
struct _tuple0 _tmp1054;struct _tuple0 _tmp39D=(_tmp1054.f1=r,((_tmp1054.f2=_tmp394,_tmp1054)));struct _tuple0 _tmp39E=_tmp39D;struct Cyc_Absyn_Tvar*_tmp3A0;struct Cyc_Absyn_Tvar*_tmp3A2;_LL260:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp39F=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp39E.f1;if(_tmp39F->tag != 2)goto _LL262;else{_tmp3A0=_tmp39F->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3A1=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp39E.f2;if(_tmp3A1->tag != 2)goto _LL262;else{_tmp3A2=_tmp3A1->f1;}};_LL261:
 return Cyc_Absyn_tvar_cmp(_tmp3A0,_tmp3A2)== 0;_LL262:;_LL263:
 return 0;_LL25F:;};_LL257: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp395=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp392;if(_tmp395->tag != 24)goto _LL259;else{_tmp396=_tmp395->f1;}}_LL258:
# 1147
 for(0;_tmp396 != 0;_tmp396=_tmp396->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp396->hd))return 1;}
return 0;_LL259: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp397=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp392;if(_tmp397->tag != 25)goto _LL25B;else{_tmp398=(void*)_tmp397->f1;}}_LL25A: {
# 1151
void*_tmp3A4=Cyc_Tcutil_rgns_of(_tmp398);void*_tmp3A5=_tmp3A4;void*_tmp3A7;void*_tmp3A8;_LL265: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3A6=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3A5;if(_tmp3A6->tag != 25)goto _LL267;else{_tmp3A7=(void*)_tmp3A6->f1;}}_LL266:
# 1153
 if(!constrain)return 0;{
void*_tmp3A9=Cyc_Tcutil_compress(_tmp3A7);void*_tmp3AA=_tmp3A9;struct Cyc_Core_Opt*_tmp3AC;void**_tmp3AD;struct Cyc_Core_Opt*_tmp3AE;_LL26A: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3AB=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3AA;if(_tmp3AB->tag != 1)goto _LL26C;else{_tmp3AC=_tmp3AB->f1;_tmp3AD=(void**)((void**)& _tmp3AB->f2);_tmp3AE=_tmp3AB->f4;}}_LL26B: {
# 1158
void*_tmp3AF=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp3AE);
# 1161
Cyc_Tcutil_occurs(_tmp3AF,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3AE))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1063;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1062;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1061;void*_tmp1060[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp105F;void*_tmp3B0=Cyc_Tcutil_dummy_fntype((void*)((_tmp105F=_cycalloc(sizeof(*_tmp105F)),((_tmp105F[0]=((_tmp1063.tag=24,((_tmp1063.f1=((_tmp1060[1]=(void*)((_tmp1062=_cycalloc(sizeof(*_tmp1062)),((_tmp1062[0]=((_tmp1061.tag=23,((_tmp1061.f1=r,_tmp1061)))),_tmp1062)))),((_tmp1060[0]=_tmp3AF,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1060,sizeof(void*),2)))))),_tmp1063)))),_tmp105F)))));
*_tmp3AD=_tmp3B0;
return 1;};}_LL26C:;_LL26D:
 return 0;_LL269:;};_LL267: _tmp3A8=_tmp3A5;_LL268:
# 1167
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp3A8);_LL264:;}_LL25B: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp399=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp392;if(_tmp399->tag != 1)goto _LL25D;else{_tmp39A=_tmp399->f1;_tmp39B=(void**)((void**)& _tmp399->f2);_tmp39C=_tmp399->f4;}}_LL25C:
# 1170
 if(_tmp39A == 0  || ((struct Cyc_Absyn_Kind*)_tmp39A->v)->kind != Cyc_Absyn_EffKind){
const char*_tmp1066;void*_tmp1065;(_tmp1065=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1066="effect evar has wrong kind",_tag_dyneither(_tmp1066,sizeof(char),27))),_tag_dyneither(_tmp1065,sizeof(void*),0)));}
if(!constrain)return 0;{
# 1175
void*_tmp3B8=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp39C);
# 1178
Cyc_Tcutil_occurs(_tmp3B8,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp39C))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp107B;struct Cyc_List_List*_tmp107A;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1079;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1078;struct Cyc_List_List*_tmp1077;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1076;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3B9=(_tmp1076=_cycalloc(sizeof(*_tmp1076)),((_tmp1076[0]=((_tmp107B.tag=24,((_tmp107B.f1=((_tmp1077=_cycalloc(sizeof(*_tmp1077)),((_tmp1077->hd=_tmp3B8,((_tmp1077->tl=((_tmp107A=_cycalloc(sizeof(*_tmp107A)),((_tmp107A->hd=(void*)((_tmp1078=_cycalloc(sizeof(*_tmp1078)),((_tmp1078[0]=((_tmp1079.tag=23,((_tmp1079.f1=r,_tmp1079)))),_tmp1078)))),((_tmp107A->tl=0,_tmp107A)))))),_tmp1077)))))),_tmp107B)))),_tmp1076)));
*_tmp39B=(void*)_tmp3B9;
return 1;};};_LL25D:;_LL25E:
 return 0;_LL254:;};}
# 1189
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Tcutil_compress(t);{
void*_tmp3C0=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_tmp3C1=_tmp3C0;struct Cyc_List_List*_tmp3C4;void*_tmp3C6;struct Cyc_Core_Opt*_tmp3C8;void**_tmp3C9;struct Cyc_Core_Opt*_tmp3CA;_LL26F: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3C2=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3C1;if(_tmp3C2->tag != 23)goto _LL271;}_LL270:
 return 0;_LL271: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3C3=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3C1;if(_tmp3C3->tag != 24)goto _LL273;else{_tmp3C4=_tmp3C3->f1;}}_LL272:
# 1194
 for(0;_tmp3C4 != 0;_tmp3C4=_tmp3C4->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp3C4->hd))
return 1;}
return 0;_LL273: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3C5=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3C1;if(_tmp3C5->tag != 25)goto _LL275;else{_tmp3C6=(void*)_tmp3C5->f1;}}_LL274:
# 1199
 _tmp3C6=Cyc_Tcutil_compress(_tmp3C6);
if(t == _tmp3C6)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp3C6);{
void*_tmp3CB=Cyc_Tcutil_rgns_of(t);void*_tmp3CC=_tmp3CB;void*_tmp3CE;void*_tmp3CF;_LL27A: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3CD=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3CC;if(_tmp3CD->tag != 25)goto _LL27C;else{_tmp3CE=(void*)_tmp3CD->f1;}}_LL27B: {
# 1204
struct _tuple0 _tmp107C;struct _tuple0 _tmp3D0=(_tmp107C.f1=Cyc_Tcutil_compress(_tmp3CE),((_tmp107C.f2=_tmp3C6,_tmp107C)));struct _tuple0 _tmp3D1=_tmp3D0;struct Cyc_Absyn_Tvar*_tmp3D3;struct Cyc_Absyn_Tvar*_tmp3D5;_LL27F:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3D2=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3D1.f1;if(_tmp3D2->tag != 2)goto _LL281;else{_tmp3D3=_tmp3D2->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3D4=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3D1.f2;if(_tmp3D4->tag != 2)goto _LL281;else{_tmp3D5=_tmp3D4->f1;}};_LL280:
 return Cyc_Tcutil_unify(t,_tmp3C6);_LL281:;_LL282:
 return _tmp3CE == _tmp3C6;_LL27E:;}_LL27C: _tmp3CF=_tmp3CC;_LL27D:
# 1208
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp3CF);_LL279:;};_LL275: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3C7=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3C1;if(_tmp3C7->tag != 1)goto _LL277;else{_tmp3C8=_tmp3C7->f1;_tmp3C9=(void**)((void**)& _tmp3C7->f2);_tmp3CA=_tmp3C7->f4;}}_LL276:
# 1211
 if(_tmp3C8 == 0  || ((struct Cyc_Absyn_Kind*)_tmp3C8->v)->kind != Cyc_Absyn_EffKind){
const char*_tmp107F;void*_tmp107E;(_tmp107E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp107F="effect evar has wrong kind",_tag_dyneither(_tmp107F,sizeof(char),27))),_tag_dyneither(_tmp107E,sizeof(void*),0)));}
if(!may_constrain_evars)return 0;{
# 1216
void*_tmp3D9=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp3CA);
# 1219
Cyc_Tcutil_occurs(_tmp3D9,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3CA))->v,t);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1094;struct Cyc_List_List*_tmp1093;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp1092;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1091;struct Cyc_List_List*_tmp1090;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp108F;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3DA=(_tmp108F=_cycalloc(sizeof(*_tmp108F)),((_tmp108F[0]=((_tmp1094.tag=24,((_tmp1094.f1=((_tmp1090=_cycalloc(sizeof(*_tmp1090)),((_tmp1090->hd=_tmp3D9,((_tmp1090->tl=((_tmp1093=_cycalloc(sizeof(*_tmp1093)),((_tmp1093->hd=(void*)((_tmp1091=_cycalloc(sizeof(*_tmp1091)),((_tmp1091[0]=((_tmp1092.tag=25,((_tmp1092.f1=t,_tmp1092)))),_tmp1091)))),((_tmp1093->tl=0,_tmp1093)))))),_tmp1090)))))),_tmp1094)))),_tmp108F)));
*_tmp3C9=(void*)_tmp3DA;
return 1;};};_LL277:;_LL278:
 return 0;_LL26E:;};}
# 1230
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp3E1=e;struct Cyc_Absyn_Tvar*_tmp3E3;struct Cyc_List_List*_tmp3E5;void*_tmp3E7;struct Cyc_Core_Opt*_tmp3E9;void**_tmp3EA;struct Cyc_Core_Opt*_tmp3EB;_LL284: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3E2=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3E1;if(_tmp3E2->tag != 2)goto _LL286;else{_tmp3E3=_tmp3E2->f1;}}_LL285:
 return Cyc_Absyn_tvar_cmp(v,_tmp3E3)== 0;_LL286: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3E4=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3E1;if(_tmp3E4->tag != 24)goto _LL288;else{_tmp3E5=_tmp3E4->f1;}}_LL287:
# 1235
 for(0;_tmp3E5 != 0;_tmp3E5=_tmp3E5->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)_tmp3E5->hd))
return 1;}
return 0;_LL288: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3E6=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3E1;if(_tmp3E6->tag != 25)goto _LL28A;else{_tmp3E7=(void*)_tmp3E6->f1;}}_LL289: {
# 1240
void*_tmp3EC=Cyc_Tcutil_rgns_of(_tmp3E7);void*_tmp3ED=_tmp3EC;void*_tmp3EF;void*_tmp3F0;_LL28F: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3EE=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3ED;if(_tmp3EE->tag != 25)goto _LL291;else{_tmp3EF=(void*)_tmp3EE->f1;}}_LL290:
# 1242
 if(!may_constrain_evars)return 0;{
void*_tmp3F1=Cyc_Tcutil_compress(_tmp3EF);void*_tmp3F2=_tmp3F1;struct Cyc_Core_Opt*_tmp3F4;void**_tmp3F5;struct Cyc_Core_Opt*_tmp3F6;_LL294: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3F3=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3F2;if(_tmp3F3->tag != 1)goto _LL296;else{_tmp3F4=_tmp3F3->f1;_tmp3F5=(void**)((void**)& _tmp3F3->f2);_tmp3F6=_tmp3F3->f4;}}_LL295: {
# 1248
void*_tmp3F7=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp3F6);
# 1250
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3F6))->v,v))return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp10A3;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp10A2;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp10A1;void*_tmp10A0[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp109F;void*_tmp3F8=Cyc_Tcutil_dummy_fntype((void*)((_tmp109F=_cycalloc(sizeof(*_tmp109F)),((_tmp109F[0]=((_tmp10A3.tag=24,((_tmp10A3.f1=((_tmp10A0[1]=(void*)((_tmp10A2=_cycalloc(sizeof(*_tmp10A2)),((_tmp10A2[0]=((_tmp10A1.tag=2,((_tmp10A1.f1=v,_tmp10A1)))),_tmp10A2)))),((_tmp10A0[0]=_tmp3F7,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp10A0,sizeof(void*),2)))))),_tmp10A3)))),_tmp109F)))));
*_tmp3F5=_tmp3F8;
return 1;};}_LL296:;_LL297:
 return 0;_LL293:;};_LL291: _tmp3F0=_tmp3ED;_LL292:
# 1256
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp3F0);_LL28E:;}_LL28A: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3E8=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3E1;if(_tmp3E8->tag != 1)goto _LL28C;else{_tmp3E9=_tmp3E8->f1;_tmp3EA=(void**)((void**)& _tmp3E8->f2);_tmp3EB=_tmp3E8->f4;}}_LL28B:
# 1259
 if(_tmp3E9 == 0  || ((struct Cyc_Absyn_Kind*)_tmp3E9->v)->kind != Cyc_Absyn_EffKind){
const char*_tmp10A6;void*_tmp10A5;(_tmp10A5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10A6="effect evar has wrong kind",_tag_dyneither(_tmp10A6,sizeof(char),27))),_tag_dyneither(_tmp10A5,sizeof(void*),0)));}{
# 1263
void*_tmp400=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp3EB);
# 1265
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3EB))->v,v))
return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp10BB;struct Cyc_List_List*_tmp10BA;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp10B9;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp10B8;struct Cyc_List_List*_tmp10B7;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp10B6;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp401=(_tmp10B6=_cycalloc(sizeof(*_tmp10B6)),((_tmp10B6[0]=((_tmp10BB.tag=24,((_tmp10BB.f1=((_tmp10B7=_cycalloc(sizeof(*_tmp10B7)),((_tmp10B7->hd=_tmp400,((_tmp10B7->tl=((_tmp10BA=_cycalloc(sizeof(*_tmp10BA)),((_tmp10BA->hd=(void*)((_tmp10B8=_cycalloc(sizeof(*_tmp10B8)),((_tmp10B8[0]=((_tmp10B9.tag=2,((_tmp10B9.f1=v,_tmp10B9)))),_tmp10B8)))),((_tmp10BA->tl=0,_tmp10BA)))))),_tmp10B7)))))),_tmp10BB)))),_tmp10B6)));
*_tmp3EA=(void*)_tmp401;
return 1;};};_LL28C:;_LL28D:
 return 0;_LL283:;};}
# 1275
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp408=e;struct Cyc_List_List*_tmp40A;void*_tmp40C;_LL299: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp409=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp408;if(_tmp409->tag != 24)goto _LL29B;else{_tmp40A=_tmp409->f1;}}_LL29A:
# 1279
 for(0;_tmp40A != 0;_tmp40A=_tmp40A->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp40A->hd))
return 1;}
return 0;_LL29B: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp40B=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp408;if(_tmp40B->tag != 25)goto _LL29D;else{_tmp40C=(void*)_tmp40B->f1;}}_LL29C: {
# 1284
void*_tmp40E=Cyc_Tcutil_rgns_of(_tmp40C);void*_tmp40F=_tmp40E;void*_tmp411;void*_tmp412;_LL2A2: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp410=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp40F;if(_tmp410->tag != 25)goto _LL2A4;else{_tmp411=(void*)_tmp410->f1;}}_LL2A3:
 return 0;_LL2A4: _tmp412=_tmp40F;_LL2A5:
 return Cyc_Tcutil_evar_in_effect(evar,_tmp412);_LL2A1:;}_LL29D: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp40D=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp408;if(_tmp40D->tag != 1)goto _LL29F;}_LL29E:
# 1288
 return evar == e;_LL29F:;_LL2A0:
 return 0;_LL298:;};}
# 1302 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1307
void*_tmp413=Cyc_Tcutil_compress(e1);void*_tmp414=_tmp413;struct Cyc_List_List*_tmp416;void*_tmp418;struct Cyc_Absyn_Tvar*_tmp41A;void*_tmp41C;void**_tmp41E;struct Cyc_Core_Opt*_tmp41F;_LL2A7: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp415=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp414;if(_tmp415->tag != 24)goto _LL2A9;else{_tmp416=_tmp415->f1;}}_LL2A8:
# 1309
 for(0;_tmp416 != 0;_tmp416=_tmp416->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp416->hd,e2))
return 0;}
return 1;_LL2A9: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp417=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp414;if(_tmp417->tag != 23)goto _LL2AB;else{_tmp418=(void*)_tmp417->f1;}}_LL2AA:
# 1320
 return Cyc_Tcutil_region_in_effect(may_constrain_evars,_tmp418,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp418,(void*)& Cyc_Absyn_HeapRgn_val);_LL2AB: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp419=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp414;if(_tmp419->tag != 2)goto _LL2AD;else{_tmp41A=_tmp419->f1;}}_LL2AC:
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp41A,e2);_LL2AD: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp41B=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp414;if(_tmp41B->tag != 25)goto _LL2AF;else{_tmp41C=(void*)_tmp41B->f1;}}_LL2AE: {
# 1324
void*_tmp420=Cyc_Tcutil_rgns_of(_tmp41C);void*_tmp421=_tmp420;void*_tmp423;void*_tmp424;_LL2B4: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp422=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp421;if(_tmp422->tag != 25)goto _LL2B6;else{_tmp423=(void*)_tmp422->f1;}}_LL2B5:
# 1329
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp423,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp423,Cyc_Absyn_sint_typ);_LL2B6: _tmp424=_tmp421;_LL2B7:
 return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp424,e2);_LL2B3:;}_LL2AF: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp41D=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp414;if(_tmp41D->tag != 1)goto _LL2B1;else{_tmp41E=(void**)((void**)& _tmp41D->f2);_tmp41F=_tmp41D->f4;}}_LL2B0:
# 1334
 if(!Cyc_Tcutil_evar_in_effect(e1,e2)){
# 1338
*_tmp41E=Cyc_Absyn_empty_effect;
# 1341
return 1;}else{
# 1343
return 0;}_LL2B1:;_LL2B2: {
const char*_tmp10BF;void*_tmp10BE[1];struct Cyc_String_pa_PrintArg_struct _tmp10BD;(_tmp10BD.tag=0,((_tmp10BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1)),((_tmp10BE[0]=& _tmp10BD,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10BF="subset_effect: bad effect: %s",_tag_dyneither(_tmp10BF,sizeof(char),30))),_tag_dyneither(_tmp10BE,sizeof(void*),1)))))));}_LL2A6:;}
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
struct _tuple0*_tmp428=(struct _tuple0*)r1->hd;void*_tmp42A;void*_tmp42B;struct _tuple0*_tmp429=_tmp428;_tmp42A=_tmp429->f1;_tmp42B=_tmp429->f2;{
int found=_tmp42A == (void*)& Cyc_Absyn_HeapRgn_val;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
struct _tuple0*_tmp42C=(struct _tuple0*)r2->hd;void*_tmp42E;void*_tmp42F;struct _tuple0*_tmp42D=_tmp42C;_tmp42E=_tmp42D->f1;_tmp42F=_tmp42D->f2;
if(Cyc_Tcutil_unify(_tmp42A,_tmp42E) && Cyc_Tcutil_unify(_tmp42B,_tmp42F)){
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
struct Cyc_Relations_Reln*_tmp430=Cyc_Relations_negate(Cyc_Core_heap_region,(struct Cyc_Relations_Reln*)r2->hd);
struct Cyc_List_List*_tmp10C0;struct Cyc_List_List*_tmp431=(_tmp10C0=_cycalloc(sizeof(*_tmp10C0)),((_tmp10C0->hd=_tmp430,((_tmp10C0->tl=r1,_tmp10C0)))));
if(Cyc_Relations_consistent_relations(_tmp431))return 0;}
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
{void*_tmp433=t1;struct Cyc_Core_Opt*_tmp435;void**_tmp436;struct Cyc_Core_Opt*_tmp437;_LL2B9: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp434=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp433;if(_tmp434->tag != 1)goto _LL2BB;else{_tmp435=_tmp434->f1;_tmp436=(void**)((void**)& _tmp434->f2);_tmp437=_tmp434->f4;}}_LL2BA:
# 1430
 Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp437))->v,t2);{
struct Cyc_Absyn_Kind*_tmp438=Cyc_Tcutil_typ_kind(t2);
# 1435
if(Cyc_Tcutil_kind_leq(_tmp438,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp435))->v)){
*_tmp436=t2;
return;}else{
# 1439
{void*_tmp439=t2;void**_tmp43B;struct Cyc_Core_Opt*_tmp43C;struct Cyc_Absyn_PtrInfo _tmp43E;_LL2BE: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp43A=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp439;if(_tmp43A->tag != 1)goto _LL2C0;else{_tmp43B=(void**)((void**)& _tmp43A->f2);_tmp43C=_tmp43A->f4;}}_LL2BF: {
# 1442
struct Cyc_List_List*_tmp43F=(struct Cyc_List_List*)_tmp437->v;
{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp43C))->v;for(0;s2 != 0;s2=s2->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp43F,(struct Cyc_Absyn_Tvar*)s2->hd)){
{const char*_tmp10C1;Cyc_Tcutil_failure_reason=((_tmp10C1="(type variable would escape scope)",_tag_dyneither(_tmp10C1,sizeof(char),35)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1449
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp435->v,_tmp438)){
*_tmp43B=t1;return;}
# 1452
{const char*_tmp10C2;Cyc_Tcutil_failure_reason=((_tmp10C2="(kinds are incompatible)",_tag_dyneither(_tmp10C2,sizeof(char),25)));}
goto _LL2BD;}_LL2C0:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp43D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp439;if(_tmp43D->tag != 5)goto _LL2C2;else{_tmp43E=_tmp43D->f1;}}if(!(((struct Cyc_Absyn_Kind*)_tmp435->v)->kind == Cyc_Absyn_BoxKind))goto _LL2C2;_LL2C1: {
# 1458
union Cyc_Absyn_Constraint*_tmp442=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((_tmp43E.ptr_atts).bounds);
{union Cyc_Absyn_Constraint*_tmp443=_tmp442;_LL2C5: if((_tmp443->No_constr).tag != 3)goto _LL2C7;_LL2C6:
# 1462
{struct _union_Constraint_Eq_constr*_tmp10C3;(_tmp10C3=& _tmp442->Eq_constr,((_tmp10C3->tag=1,_tmp10C3->val=Cyc_Absyn_bounds_one)));}
*_tmp436=t2;
return;_LL2C7:;_LL2C8:
 goto _LL2C4;_LL2C4:;}
# 1467
goto _LL2BD;}_LL2C2:;_LL2C3:
 goto _LL2BD;_LL2BD:;}
# 1470
{const char*_tmp10C4;Cyc_Tcutil_failure_reason=((_tmp10C4="(kinds are incompatible)",_tag_dyneither(_tmp10C4,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};_LL2BB:;_LL2BC:
# 1473
 goto _LL2B8;_LL2B8:;}
# 1478
{struct _tuple0 _tmp10C5;struct _tuple0 _tmp446=(_tmp10C5.f1=t2,((_tmp10C5.f2=t1,_tmp10C5)));struct _tuple0 _tmp447=_tmp446;struct Cyc_Absyn_Tvar*_tmp44C;struct Cyc_Absyn_Tvar*_tmp44E;union Cyc_Absyn_AggrInfoU _tmp450;struct Cyc_List_List*_tmp451;union Cyc_Absyn_AggrInfoU _tmp453;struct Cyc_List_List*_tmp454;struct _tuple2*_tmp456;struct _tuple2*_tmp458;struct Cyc_List_List*_tmp45A;struct Cyc_List_List*_tmp45C;struct Cyc_Absyn_Datatypedecl*_tmp45E;struct Cyc_List_List*_tmp45F;struct Cyc_Absyn_Datatypedecl*_tmp461;struct Cyc_List_List*_tmp462;struct Cyc_Absyn_Datatypedecl*_tmp464;struct Cyc_Absyn_Datatypefield*_tmp465;struct Cyc_List_List*_tmp466;struct Cyc_Absyn_Datatypedecl*_tmp468;struct Cyc_Absyn_Datatypefield*_tmp469;struct Cyc_List_List*_tmp46A;void*_tmp46C;struct Cyc_Absyn_Tqual _tmp46D;void*_tmp46E;union Cyc_Absyn_Constraint*_tmp46F;union Cyc_Absyn_Constraint*_tmp470;union Cyc_Absyn_Constraint*_tmp471;void*_tmp473;struct Cyc_Absyn_Tqual _tmp474;void*_tmp475;union Cyc_Absyn_Constraint*_tmp476;union Cyc_Absyn_Constraint*_tmp477;union Cyc_Absyn_Constraint*_tmp478;enum Cyc_Absyn_Sign _tmp47A;enum Cyc_Absyn_Size_of _tmp47B;enum Cyc_Absyn_Sign _tmp47D;enum Cyc_Absyn_Size_of _tmp47E;int _tmp480;int _tmp482;void*_tmp484;void*_tmp486;struct Cyc_Absyn_Exp*_tmp488;struct Cyc_Absyn_Exp*_tmp48A;void*_tmp48C;struct Cyc_Absyn_Tqual _tmp48D;struct Cyc_Absyn_Exp*_tmp48E;union Cyc_Absyn_Constraint*_tmp48F;void*_tmp491;struct Cyc_Absyn_Tqual _tmp492;struct Cyc_Absyn_Exp*_tmp493;union Cyc_Absyn_Constraint*_tmp494;struct Cyc_List_List*_tmp496;void*_tmp497;struct Cyc_Absyn_Tqual _tmp498;void*_tmp499;struct Cyc_List_List*_tmp49A;int _tmp49B;struct Cyc_Absyn_VarargInfo*_tmp49C;struct Cyc_List_List*_tmp49D;struct Cyc_List_List*_tmp49E;struct Cyc_Absyn_Exp*_tmp49F;struct Cyc_List_List*_tmp4A0;struct Cyc_Absyn_Exp*_tmp4A1;struct Cyc_List_List*_tmp4A2;struct Cyc_List_List*_tmp4A4;void*_tmp4A5;struct Cyc_Absyn_Tqual _tmp4A6;void*_tmp4A7;struct Cyc_List_List*_tmp4A8;int _tmp4A9;struct Cyc_Absyn_VarargInfo*_tmp4AA;struct Cyc_List_List*_tmp4AB;struct Cyc_List_List*_tmp4AC;struct Cyc_Absyn_Exp*_tmp4AD;struct Cyc_List_List*_tmp4AE;struct Cyc_Absyn_Exp*_tmp4AF;struct Cyc_List_List*_tmp4B0;struct Cyc_List_List*_tmp4B2;struct Cyc_List_List*_tmp4B4;enum Cyc_Absyn_AggrKind _tmp4B6;struct Cyc_List_List*_tmp4B7;enum Cyc_Absyn_AggrKind _tmp4B9;struct Cyc_List_List*_tmp4BA;struct Cyc_List_List*_tmp4BC;struct Cyc_Absyn_Typedefdecl*_tmp4BD;struct Cyc_List_List*_tmp4BF;struct Cyc_Absyn_Typedefdecl*_tmp4C0;void*_tmp4C8;void*_tmp4CA;void*_tmp4CC;void*_tmp4CD;void*_tmp4CF;void*_tmp4D0;_LL2CA: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp448=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp447.f1;if(_tmp448->tag != 1)goto _LL2CC;}_LL2CB:
# 1481
 Cyc_Tcutil_unify_it(t2,t1);
return;_LL2CC:{struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp449=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp447.f1;if(_tmp449->tag != 0)goto _LL2CE;}{struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp44A=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp447.f2;if(_tmp44A->tag != 0)goto _LL2CE;};_LL2CD:
# 1484
 return;_LL2CE:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp44B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp447.f1;if(_tmp44B->tag != 2)goto _LL2D0;else{_tmp44C=_tmp44B->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp44D=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp447.f2;if(_tmp44D->tag != 2)goto _LL2D0;else{_tmp44E=_tmp44D->f1;}};_LL2CF: {
# 1487
struct _dyneither_ptr*_tmp4D7=_tmp44C->name;
struct _dyneither_ptr*_tmp4D8=_tmp44E->name;
# 1490
int _tmp4D9=_tmp44C->identity;
int _tmp4DA=_tmp44E->identity;
if(_tmp4DA == _tmp4D9)return;
{const char*_tmp10C6;Cyc_Tcutil_failure_reason=((_tmp10C6="(variable types are not the same)",_tag_dyneither(_tmp10C6,sizeof(char),34)));}
goto _LL2C9;}_LL2D0:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp44F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp447.f1;if(_tmp44F->tag != 11)goto _LL2D2;else{_tmp450=(_tmp44F->f1).aggr_info;_tmp451=(_tmp44F->f1).targs;}}{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp452=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp447.f2;if(_tmp452->tag != 11)goto _LL2D2;else{_tmp453=(_tmp452->f1).aggr_info;_tmp454=(_tmp452->f1).targs;}};_LL2D1: {
# 1497
struct _tuple13 _tmp4DC=Cyc_Absyn_aggr_kinded_name(_tmp453);enum Cyc_Absyn_AggrKind _tmp4DE;struct _tuple2*_tmp4DF;struct _tuple13 _tmp4DD=_tmp4DC;_tmp4DE=_tmp4DD.f1;_tmp4DF=_tmp4DD.f2;{
struct _tuple13 _tmp4E0=Cyc_Absyn_aggr_kinded_name(_tmp450);enum Cyc_Absyn_AggrKind _tmp4E2;struct _tuple2*_tmp4E3;struct _tuple13 _tmp4E1=_tmp4E0;_tmp4E2=_tmp4E1.f1;_tmp4E3=_tmp4E1.f2;
if(_tmp4DE != _tmp4E2){{const char*_tmp10C7;Cyc_Tcutil_failure_reason=((_tmp10C7="(struct and union type)",_tag_dyneither(_tmp10C7,sizeof(char),24)));}goto _LL2C9;}
if(Cyc_Absyn_qvar_cmp(_tmp4DF,_tmp4E3)!= 0){{const char*_tmp10C8;Cyc_Tcutil_failure_reason=((_tmp10C8="(different type name)",_tag_dyneither(_tmp10C8,sizeof(char),22)));}goto _LL2C9;}
Cyc_Tcutil_unify_list(_tmp454,_tmp451);
return;};}_LL2D2:{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp455=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp447.f1;if(_tmp455->tag != 13)goto _LL2D4;else{_tmp456=_tmp455->f1;}}{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp457=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp447.f2;if(_tmp457->tag != 13)goto _LL2D4;else{_tmp458=_tmp457->f1;}};_LL2D3:
# 1506
 if(Cyc_Absyn_qvar_cmp(_tmp456,_tmp458)== 0)return;
{const char*_tmp10C9;Cyc_Tcutil_failure_reason=((_tmp10C9="(different enum types)",_tag_dyneither(_tmp10C9,sizeof(char),23)));}
goto _LL2C9;_LL2D4:{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp459=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp447.f1;if(_tmp459->tag != 14)goto _LL2D6;else{_tmp45A=_tmp459->f1;}}{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp45B=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp447.f2;if(_tmp45B->tag != 14)goto _LL2D6;else{_tmp45C=_tmp45B->f1;}};_LL2D5: {
# 1511
int bad=0;
for(0;_tmp45A != 0  && _tmp45C != 0;(_tmp45A=_tmp45A->tl,_tmp45C=_tmp45C->tl)){
struct Cyc_Absyn_Enumfield*_tmp4E7=(struct Cyc_Absyn_Enumfield*)_tmp45A->hd;
struct Cyc_Absyn_Enumfield*_tmp4E8=(struct Cyc_Absyn_Enumfield*)_tmp45C->hd;
if(Cyc_Absyn_qvar_cmp(_tmp4E7->name,_tmp4E8->name)!= 0){
{const char*_tmp10CA;Cyc_Tcutil_failure_reason=((_tmp10CA="(different names for enum fields)",_tag_dyneither(_tmp10CA,sizeof(char),34)));}
bad=1;
break;}
# 1520
if(_tmp4E7->tag == _tmp4E8->tag)continue;
if(_tmp4E7->tag == 0  || _tmp4E8->tag == 0){
{const char*_tmp10CB;Cyc_Tcutil_failure_reason=((_tmp10CB="(different tag values for enum fields)",_tag_dyneither(_tmp10CB,sizeof(char),39)));}
bad=1;
break;}
# 1526
if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp4E7->tag),(struct Cyc_Absyn_Exp*)_check_null(_tmp4E8->tag))){
{const char*_tmp10CC;Cyc_Tcutil_failure_reason=((_tmp10CC="(different tag values for enum fields)",_tag_dyneither(_tmp10CC,sizeof(char),39)));}
bad=1;
break;}}
# 1532
if(bad)goto _LL2C9;
if(_tmp45A == 0  && _tmp45C == 0)return;
{const char*_tmp10CD;Cyc_Tcutil_failure_reason=((_tmp10CD="(different number of fields for enums)",_tag_dyneither(_tmp10CD,sizeof(char),39)));}
goto _LL2C9;}_LL2D6:{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp45D=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp447.f1;if(_tmp45D->tag != 3)goto _LL2D8;else{if((((_tmp45D->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2D8;_tmp45E=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp45D->f1).datatype_info).KnownDatatype).val);_tmp45F=(_tmp45D->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp460=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp447.f2;if(_tmp460->tag != 3)goto _LL2D8;else{if((((_tmp460->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2D8;_tmp461=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp460->f1).datatype_info).KnownDatatype).val);_tmp462=(_tmp460->f1).targs;}};_LL2D7:
# 1539
 if(_tmp45E == _tmp461  || Cyc_Absyn_qvar_cmp(_tmp45E->name,_tmp461->name)== 0){
Cyc_Tcutil_unify_list(_tmp462,_tmp45F);
return;}
# 1543
goto _LL2C9;_LL2D8:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp463=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp447.f1;if(_tmp463->tag != 4)goto _LL2DA;else{if((((_tmp463->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2DA;_tmp464=((struct _tuple3)(((_tmp463->f1).field_info).KnownDatatypefield).val).f1;_tmp465=((struct _tuple3)(((_tmp463->f1).field_info).KnownDatatypefield).val).f2;_tmp466=(_tmp463->f1).targs;}}{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp467=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp447.f2;if(_tmp467->tag != 4)goto _LL2DA;else{if((((_tmp467->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2DA;_tmp468=((struct _tuple3)(((_tmp467->f1).field_info).KnownDatatypefield).val).f1;_tmp469=((struct _tuple3)(((_tmp467->f1).field_info).KnownDatatypefield).val).f2;_tmp46A=(_tmp467->f1).targs;}};_LL2D9:
# 1547
 if((_tmp464 == _tmp468  || Cyc_Absyn_qvar_cmp(_tmp464->name,_tmp468->name)== 0) && (
_tmp465 == _tmp469  || Cyc_Absyn_qvar_cmp(_tmp465->name,_tmp469->name)== 0)){
Cyc_Tcutil_unify_list(_tmp46A,_tmp466);
return;}
# 1552
{const char*_tmp10CE;Cyc_Tcutil_failure_reason=((_tmp10CE="(different datatype field types)",_tag_dyneither(_tmp10CE,sizeof(char),33)));}
goto _LL2C9;_LL2DA:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp46B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp447.f1;if(_tmp46B->tag != 5)goto _LL2DC;else{_tmp46C=(_tmp46B->f1).elt_typ;_tmp46D=(_tmp46B->f1).elt_tq;_tmp46E=((_tmp46B->f1).ptr_atts).rgn;_tmp46F=((_tmp46B->f1).ptr_atts).nullable;_tmp470=((_tmp46B->f1).ptr_atts).bounds;_tmp471=((_tmp46B->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp472=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp447.f2;if(_tmp472->tag != 5)goto _LL2DC;else{_tmp473=(_tmp472->f1).elt_typ;_tmp474=(_tmp472->f1).elt_tq;_tmp475=((_tmp472->f1).ptr_atts).rgn;_tmp476=((_tmp472->f1).ptr_atts).nullable;_tmp477=((_tmp472->f1).ptr_atts).bounds;_tmp478=((_tmp472->f1).ptr_atts).zero_term;}};_LL2DB:
# 1557
 Cyc_Tcutil_unify_it(_tmp473,_tmp46C);
Cyc_Tcutil_unify_it(_tmp46E,_tmp475);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp10CF;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp478,_tmp471,((_tmp10CF="(not both zero terminated)",_tag_dyneither(_tmp10CF,sizeof(char),27))));}
Cyc_Tcutil_unify_tqual(_tmp474,_tmp473,_tmp46D,_tmp46C);
{const char*_tmp10D0;((void(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp477,_tmp470,((_tmp10D0="(different pointer bounds)",_tag_dyneither(_tmp10D0,sizeof(char),27))));}
# 1565
{void*_tmp4F0=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp477);void*_tmp4F1=_tmp4F0;_LL307: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp4F2=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp4F1;if(_tmp4F2->tag != 0)goto _LL309;}_LL308:
 return;_LL309:;_LL30A:
 goto _LL306;_LL306:;}
# 1569
{const char*_tmp10D1;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp476,_tmp46F,((_tmp10D1="(incompatible pointer types)",_tag_dyneither(_tmp10D1,sizeof(char),29))));}
return;_LL2DC:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp479=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp447.f1;if(_tmp479->tag != 6)goto _LL2DE;else{_tmp47A=_tmp479->f1;_tmp47B=_tmp479->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp47C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp447.f2;if(_tmp47C->tag != 6)goto _LL2DE;else{_tmp47D=_tmp47C->f1;_tmp47E=_tmp47C->f2;}};_LL2DD:
# 1574
 if(_tmp47D == _tmp47A  && ((_tmp47E == _tmp47B  || _tmp47E == Cyc_Absyn_Int_sz  && _tmp47B == Cyc_Absyn_Long_sz) || 
_tmp47E == Cyc_Absyn_Long_sz  && _tmp47B == Cyc_Absyn_Int_sz))return;
{const char*_tmp10D2;Cyc_Tcutil_failure_reason=((_tmp10D2="(different integral types)",_tag_dyneither(_tmp10D2,sizeof(char),27)));}
goto _LL2C9;_LL2DE:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp47F=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp447.f1;if(_tmp47F->tag != 7)goto _LL2E0;else{_tmp480=_tmp47F->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp481=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp447.f2;if(_tmp481->tag != 7)goto _LL2E0;else{_tmp482=_tmp481->f1;}};_LL2DF:
# 1580
 if(_tmp482 == 0  && _tmp480 == 0)return;else{
if(_tmp482 == 1  && _tmp480 == 1)return;else{
# 1583
if(((_tmp482 != 0  && _tmp482 != 1) && _tmp480 != 0) && _tmp480 != 1)return;}}
goto _LL2C9;_LL2E0:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp483=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp447.f1;if(_tmp483->tag != 19)goto _LL2E2;else{_tmp484=(void*)_tmp483->f1;}}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp485=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp447.f2;if(_tmp485->tag != 19)goto _LL2E2;else{_tmp486=(void*)_tmp485->f1;}};_LL2E1:
# 1586
 Cyc_Tcutil_unify_it(_tmp484,_tmp486);return;_LL2E2:{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp487=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp447.f1;if(_tmp487->tag != 18)goto _LL2E4;else{_tmp488=_tmp487->f1;}}{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp489=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp447.f2;if(_tmp489->tag != 18)goto _LL2E4;else{_tmp48A=_tmp489->f1;}};_LL2E3:
# 1589
 if(!Cyc_Evexp_same_const_exp(_tmp488,_tmp48A)){
{const char*_tmp10D3;Cyc_Tcutil_failure_reason=((_tmp10D3="(cannot prove expressions are the same)",_tag_dyneither(_tmp10D3,sizeof(char),40)));}
goto _LL2C9;}
# 1593
return;_LL2E4:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp48B=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp447.f1;if(_tmp48B->tag != 8)goto _LL2E6;else{_tmp48C=(_tmp48B->f1).elt_type;_tmp48D=(_tmp48B->f1).tq;_tmp48E=(_tmp48B->f1).num_elts;_tmp48F=(_tmp48B->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp490=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp447.f2;if(_tmp490->tag != 8)goto _LL2E6;else{_tmp491=(_tmp490->f1).elt_type;_tmp492=(_tmp490->f1).tq;_tmp493=(_tmp490->f1).num_elts;_tmp494=(_tmp490->f1).zero_term;}};_LL2E5:
# 1597
 Cyc_Tcutil_unify_it(_tmp491,_tmp48C);
Cyc_Tcutil_unify_tqual(_tmp492,_tmp491,_tmp48D,_tmp48C);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp10D4;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp48F,_tmp494,((_tmp10D4="(not both zero terminated)",_tag_dyneither(_tmp10D4,sizeof(char),27))));}
if(_tmp48E == _tmp493)return;
if(_tmp48E == 0  || _tmp493 == 0)goto _LL2C9;
if(Cyc_Evexp_same_const_exp(_tmp48E,_tmp493))
return;
{const char*_tmp10D5;Cyc_Tcutil_failure_reason=((_tmp10D5="(different array sizes)",_tag_dyneither(_tmp10D5,sizeof(char),24)));}
goto _LL2C9;_LL2E6:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp495=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp447.f1;if(_tmp495->tag != 9)goto _LL2E8;else{_tmp496=(_tmp495->f1).tvars;_tmp497=(_tmp495->f1).effect;_tmp498=(_tmp495->f1).ret_tqual;_tmp499=(_tmp495->f1).ret_typ;_tmp49A=(_tmp495->f1).args;_tmp49B=(_tmp495->f1).c_varargs;_tmp49C=(_tmp495->f1).cyc_varargs;_tmp49D=(_tmp495->f1).rgn_po;_tmp49E=(_tmp495->f1).attributes;_tmp49F=(_tmp495->f1).requires_clause;_tmp4A0=(_tmp495->f1).requires_relns;_tmp4A1=(_tmp495->f1).ensures_clause;_tmp4A2=(_tmp495->f1).ensures_relns;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4A3=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp447.f2;if(_tmp4A3->tag != 9)goto _LL2E8;else{_tmp4A4=(_tmp4A3->f1).tvars;_tmp4A5=(_tmp4A3->f1).effect;_tmp4A6=(_tmp4A3->f1).ret_tqual;_tmp4A7=(_tmp4A3->f1).ret_typ;_tmp4A8=(_tmp4A3->f1).args;_tmp4A9=(_tmp4A3->f1).c_varargs;_tmp4AA=(_tmp4A3->f1).cyc_varargs;_tmp4AB=(_tmp4A3->f1).rgn_po;_tmp4AC=(_tmp4A3->f1).attributes;_tmp4AD=(_tmp4A3->f1).requires_clause;_tmp4AE=(_tmp4A3->f1).requires_relns;_tmp4AF=(_tmp4A3->f1).ensures_clause;_tmp4B0=(_tmp4A3->f1).ensures_relns;}};_LL2E7: {
# 1611
int done=0;
struct _RegionHandle _tmp4F8=_new_region("rgn");struct _RegionHandle*rgn=& _tmp4F8;_push_region(rgn);{
struct Cyc_List_List*inst=0;
while(_tmp4A4 != 0){
if(_tmp496 == 0){
{const char*_tmp10D6;Cyc_Tcutil_failure_reason=((_tmp10D6="(second function type has too few type variables)",_tag_dyneither(_tmp10D6,sizeof(char),50)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1619
void*_tmp4FA=((struct Cyc_Absyn_Tvar*)_tmp4A4->hd)->kind;
void*_tmp4FB=((struct Cyc_Absyn_Tvar*)_tmp496->hd)->kind;
if(!Cyc_Tcutil_unify_kindbound(_tmp4FA,_tmp4FB)){
{const char*_tmp10DC;void*_tmp10DB[3];struct Cyc_String_pa_PrintArg_struct _tmp10DA;struct Cyc_String_pa_PrintArg_struct _tmp10D9;struct Cyc_String_pa_PrintArg_struct _tmp10D8;Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)((_tmp10D8.tag=0,((_tmp10D8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1625
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp496->hd,& Cyc_Tcutil_bk))),((_tmp10D9.tag=0,((_tmp10D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1624
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp4A4->hd,& Cyc_Tcutil_bk))),((_tmp10DA.tag=0,((_tmp10DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1623
Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)_tmp4A4->hd)),((_tmp10DB[0]=& _tmp10DA,((_tmp10DB[1]=& _tmp10D9,((_tmp10DB[2]=& _tmp10D8,Cyc_aprintf(((_tmp10DC="(type var %s has different kinds %s and %s)",_tag_dyneither(_tmp10DC,sizeof(char),44))),_tag_dyneither(_tmp10DB,sizeof(void*),3))))))))))))))))))));}
# 1626
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1628
{struct _tuple16*_tmp10E6;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp10E5;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp10E4;struct Cyc_List_List*_tmp10E3;inst=((_tmp10E3=_region_malloc(rgn,sizeof(*_tmp10E3)),((_tmp10E3->hd=((_tmp10E6=_region_malloc(rgn,sizeof(*_tmp10E6)),((_tmp10E6->f1=(struct Cyc_Absyn_Tvar*)_tmp496->hd,((_tmp10E6->f2=(void*)((_tmp10E4=_cycalloc(sizeof(*_tmp10E4)),((_tmp10E4[0]=((_tmp10E5.tag=2,((_tmp10E5.f1=(struct Cyc_Absyn_Tvar*)_tmp4A4->hd,_tmp10E5)))),_tmp10E4)))),_tmp10E6)))))),((_tmp10E3->tl=inst,_tmp10E3))))));}
_tmp4A4=_tmp4A4->tl;
_tmp496=_tmp496->tl;};}
# 1632
if(_tmp496 != 0){
{const char*_tmp10E7;Cyc_Tcutil_failure_reason=((_tmp10E7="(second function type has too many type variables)",_tag_dyneither(_tmp10E7,sizeof(char),51)));}
_npop_handler(0);goto _LL2C9;}
# 1636
if(inst != 0){
{struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp10F3;struct Cyc_Absyn_FnInfo _tmp10F2;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp10F1;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp10ED;struct Cyc_Absyn_FnInfo _tmp10EC;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp10EB;Cyc_Tcutil_unify_it((void*)((_tmp10EB=_cycalloc(sizeof(*_tmp10EB)),((_tmp10EB[0]=((_tmp10ED.tag=9,((_tmp10ED.f1=((_tmp10EC.tvars=0,((_tmp10EC.effect=_tmp4A5,((_tmp10EC.ret_tqual=_tmp4A6,((_tmp10EC.ret_typ=_tmp4A7,((_tmp10EC.args=_tmp4A8,((_tmp10EC.c_varargs=_tmp4A9,((_tmp10EC.cyc_varargs=_tmp4AA,((_tmp10EC.rgn_po=_tmp4AB,((_tmp10EC.attributes=_tmp4AC,((_tmp10EC.requires_clause=_tmp49F,((_tmp10EC.requires_relns=_tmp4A0,((_tmp10EC.ensures_clause=_tmp4A1,((_tmp10EC.ensures_relns=_tmp4A2,_tmp10EC)))))))))))))))))))))))))),_tmp10ED)))),_tmp10EB)))),
# 1640
Cyc_Tcutil_rsubstitute(rgn,inst,(void*)(
(_tmp10F1=_cycalloc(sizeof(*_tmp10F1)),((_tmp10F1[0]=((_tmp10F3.tag=9,((_tmp10F3.f1=((_tmp10F2.tvars=0,((_tmp10F2.effect=_tmp497,((_tmp10F2.ret_tqual=_tmp498,((_tmp10F2.ret_typ=_tmp499,((_tmp10F2.args=_tmp49A,((_tmp10F2.c_varargs=_tmp49B,((_tmp10F2.cyc_varargs=_tmp49C,((_tmp10F2.rgn_po=_tmp49D,((_tmp10F2.attributes=_tmp49E,((_tmp10F2.requires_clause=_tmp4AD,((_tmp10F2.requires_relns=_tmp4AE,((_tmp10F2.ensures_clause=_tmp4AF,((_tmp10F2.ensures_relns=_tmp4B0,_tmp10F2)))))))))))))))))))))))))),_tmp10F3)))),_tmp10F1))))));}
# 1645
done=1;}}
# 1648
if(done){
_npop_handler(0);return;}
Cyc_Tcutil_unify_it(_tmp4A7,_tmp499);
Cyc_Tcutil_unify_tqual(_tmp4A6,_tmp4A7,_tmp498,_tmp499);
for(0;_tmp4A8 != 0  && _tmp49A != 0;(_tmp4A8=_tmp4A8->tl,_tmp49A=_tmp49A->tl)){
struct _tuple10 _tmp50C=*((struct _tuple10*)_tmp4A8->hd);struct Cyc_Absyn_Tqual _tmp50E;void*_tmp50F;struct _tuple10 _tmp50D=_tmp50C;_tmp50E=_tmp50D.f2;_tmp50F=_tmp50D.f3;{
struct _tuple10 _tmp510=*((struct _tuple10*)_tmp49A->hd);struct Cyc_Absyn_Tqual _tmp512;void*_tmp513;struct _tuple10 _tmp511=_tmp510;_tmp512=_tmp511.f2;_tmp513=_tmp511.f3;
Cyc_Tcutil_unify_it(_tmp50F,_tmp513);
Cyc_Tcutil_unify_tqual(_tmp50E,_tmp50F,_tmp512,_tmp513);};}
# 1658
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(_tmp4A8 != 0  || _tmp49A != 0){
{const char*_tmp10F4;Cyc_Tcutil_failure_reason=((_tmp10F4="(function types have different number of arguments)",_tag_dyneither(_tmp10F4,sizeof(char),52)));}
_npop_handler(0);goto _LL2C9;}
# 1664
if(_tmp4A9 != _tmp49B){
{const char*_tmp10F5;Cyc_Tcutil_failure_reason=((_tmp10F5="(only one function type takes C varargs)",_tag_dyneither(_tmp10F5,sizeof(char),41)));}
_npop_handler(0);goto _LL2C9;}{
# 1669
int bad_cyc_vararg=0;
{struct _tuple22 _tmp10F6;struct _tuple22 _tmp516=(_tmp10F6.f1=_tmp4AA,((_tmp10F6.f2=_tmp49C,_tmp10F6)));struct _tuple22 _tmp517=_tmp516;struct _dyneither_ptr*_tmp518;struct Cyc_Absyn_Tqual _tmp519;void*_tmp51A;int _tmp51B;struct _dyneither_ptr*_tmp51C;struct Cyc_Absyn_Tqual _tmp51D;void*_tmp51E;int _tmp51F;_LL30C: if(_tmp517.f1 != 0)goto _LL30E;if(_tmp517.f2 != 0)goto _LL30E;_LL30D:
 goto _LL30B;_LL30E: if(_tmp517.f1 != 0)goto _LL310;_LL30F:
 goto _LL311;_LL310: if(_tmp517.f2 != 0)goto _LL312;_LL311:
# 1674
 bad_cyc_vararg=1;
{const char*_tmp10F7;Cyc_Tcutil_failure_reason=((_tmp10F7="(only one function type takes varargs)",_tag_dyneither(_tmp10F7,sizeof(char),39)));}
goto _LL30B;_LL312: if(_tmp517.f1 == 0)goto _LL30B;_tmp518=(_tmp517.f1)->name;_tmp519=(_tmp517.f1)->tq;_tmp51A=(_tmp517.f1)->type;_tmp51B=(_tmp517.f1)->inject;if(_tmp517.f2 == 0)goto _LL30B;_tmp51C=(_tmp517.f2)->name;_tmp51D=(_tmp517.f2)->tq;_tmp51E=(_tmp517.f2)->type;_tmp51F=(_tmp517.f2)->inject;_LL313:
# 1678
 Cyc_Tcutil_unify_it(_tmp51A,_tmp51E);
Cyc_Tcutil_unify_tqual(_tmp519,_tmp51A,_tmp51D,_tmp51E);
if(_tmp51B != _tmp51F){
bad_cyc_vararg=1;{
const char*_tmp10F8;Cyc_Tcutil_failure_reason=((_tmp10F8="(only one function type injects varargs)",_tag_dyneither(_tmp10F8,sizeof(char),41)));};}
# 1684
goto _LL30B;_LL30B:;}
# 1686
if(bad_cyc_vararg){_npop_handler(0);goto _LL2C9;}{
# 1689
int bad_effect=0;
{struct _tuple0 _tmp10F9;struct _tuple0 _tmp523=(_tmp10F9.f1=_tmp4A5,((_tmp10F9.f2=_tmp497,_tmp10F9)));struct _tuple0 _tmp524=_tmp523;_LL315: if(_tmp524.f1 != 0)goto _LL317;if(_tmp524.f2 != 0)goto _LL317;_LL316:
 goto _LL314;_LL317: if(_tmp524.f1 != 0)goto _LL319;_LL318:
 goto _LL31A;_LL319: if(_tmp524.f2 != 0)goto _LL31B;_LL31A:
 bad_effect=1;goto _LL314;_LL31B:;_LL31C:
 bad_effect=!Cyc_Tcutil_unify_effect((void*)_check_null(_tmp4A5),(void*)_check_null(_tmp497));goto _LL314;_LL314:;}
# 1696
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(bad_effect){
{const char*_tmp10FA;Cyc_Tcutil_failure_reason=((_tmp10FA="(function effects do not match)",_tag_dyneither(_tmp10FA,sizeof(char),32)));}
_npop_handler(0);goto _LL2C9;}
# 1702
if(!Cyc_Tcutil_same_atts(_tmp49E,_tmp4AC)){
{const char*_tmp10FB;Cyc_Tcutil_failure_reason=((_tmp10FB="(function types have different attributes)",_tag_dyneither(_tmp10FB,sizeof(char),43)));}
_npop_handler(0);goto _LL2C9;}
# 1706
if(!Cyc_Tcutil_same_rgn_po(_tmp49D,_tmp4AB)){
{const char*_tmp10FC;Cyc_Tcutil_failure_reason=((_tmp10FC="(function types have different region lifetime orderings)",_tag_dyneither(_tmp10FC,sizeof(char),58)));}
_npop_handler(0);goto _LL2C9;}
# 1710
if(!Cyc_Tcutil_check_logical_equivalence(_tmp4A0,_tmp4AE)){
{const char*_tmp10FD;Cyc_Tcutil_failure_reason=((_tmp10FD="(@requires clauses not equivalent)",_tag_dyneither(_tmp10FD,sizeof(char),35)));}
_npop_handler(0);goto _LL2C9;}
# 1714
if(!Cyc_Tcutil_check_logical_equivalence(_tmp4A2,_tmp4B0)){
{const char*_tmp10FE;Cyc_Tcutil_failure_reason=((_tmp10FE="(@ensures clauses not equivalent)",_tag_dyneither(_tmp10FE,sizeof(char),34)));}
_npop_handler(0);goto _LL2C9;}
# 1718
_npop_handler(0);return;};};
# 1612
;_pop_region(rgn);}_LL2E8:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp4B1=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp447.f1;if(_tmp4B1->tag != 10)goto _LL2EA;else{_tmp4B2=_tmp4B1->f1;}}{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp4B3=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp447.f2;if(_tmp4B3->tag != 10)goto _LL2EA;else{_tmp4B4=_tmp4B3->f1;}};_LL2E9:
# 1721
 for(0;_tmp4B4 != 0  && _tmp4B2 != 0;(_tmp4B4=_tmp4B4->tl,_tmp4B2=_tmp4B2->tl)){
struct _tuple12 _tmp52B=*((struct _tuple12*)_tmp4B4->hd);struct Cyc_Absyn_Tqual _tmp52D;void*_tmp52E;struct _tuple12 _tmp52C=_tmp52B;_tmp52D=_tmp52C.f1;_tmp52E=_tmp52C.f2;{
struct _tuple12 _tmp52F=*((struct _tuple12*)_tmp4B2->hd);struct Cyc_Absyn_Tqual _tmp531;void*_tmp532;struct _tuple12 _tmp530=_tmp52F;_tmp531=_tmp530.f1;_tmp532=_tmp530.f2;
Cyc_Tcutil_unify_it(_tmp52E,_tmp532);
Cyc_Tcutil_unify_tqual(_tmp52D,_tmp52E,_tmp531,_tmp532);};}
# 1727
if(_tmp4B4 == 0  && _tmp4B2 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp10FF;Cyc_Tcutil_failure_reason=((_tmp10FF="(tuple types have different numbers of components)",_tag_dyneither(_tmp10FF,sizeof(char),51)));}
goto _LL2C9;_LL2EA:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp4B5=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp447.f1;if(_tmp4B5->tag != 12)goto _LL2EC;else{_tmp4B6=_tmp4B5->f1;_tmp4B7=_tmp4B5->f2;}}{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp4B8=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp447.f2;if(_tmp4B8->tag != 12)goto _LL2EC;else{_tmp4B9=_tmp4B8->f1;_tmp4BA=_tmp4B8->f2;}};_LL2EB:
# 1734
 if(_tmp4B9 != _tmp4B6){{const char*_tmp1100;Cyc_Tcutil_failure_reason=((_tmp1100="(struct and union type)",_tag_dyneither(_tmp1100,sizeof(char),24)));}goto _LL2C9;}
for(0;_tmp4BA != 0  && _tmp4B7 != 0;(_tmp4BA=_tmp4BA->tl,_tmp4B7=_tmp4B7->tl)){
struct Cyc_Absyn_Aggrfield*_tmp535=(struct Cyc_Absyn_Aggrfield*)_tmp4BA->hd;
struct Cyc_Absyn_Aggrfield*_tmp536=(struct Cyc_Absyn_Aggrfield*)_tmp4B7->hd;
if(Cyc_strptrcmp(_tmp535->name,_tmp536->name)!= 0){
{const char*_tmp1101;Cyc_Tcutil_failure_reason=((_tmp1101="(different member names)",_tag_dyneither(_tmp1101,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1742
Cyc_Tcutil_unify_it(_tmp535->type,_tmp536->type);
Cyc_Tcutil_unify_tqual(_tmp535->tq,_tmp535->type,_tmp536->tq,_tmp536->type);
if(!Cyc_Tcutil_same_atts(_tmp535->attributes,_tmp536->attributes)){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp1102;Cyc_Tcutil_failure_reason=((_tmp1102="(different attributes on member)",_tag_dyneither(_tmp1102,sizeof(char),33)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1750
if((_tmp535->width != 0  && _tmp536->width == 0  || 
_tmp536->width != 0  && _tmp535->width == 0) || 
(_tmp535->width != 0  && _tmp536->width != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp535->width),(struct Cyc_Absyn_Exp*)_check_null(_tmp536->width))){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp1103;Cyc_Tcutil_failure_reason=((_tmp1103="(different bitfield widths on member)",_tag_dyneither(_tmp1103,sizeof(char),38)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1759
if((_tmp535->requires_clause != 0  && _tmp536->requires_clause == 0  || 
_tmp535->requires_clause == 0  && _tmp536->requires_clause != 0) || 
(_tmp535->requires_clause == 0  && _tmp536->requires_clause != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp535->requires_clause),(struct Cyc_Absyn_Exp*)_check_null(_tmp536->requires_clause))){
# 1764
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp1104;Cyc_Tcutil_failure_reason=((_tmp1104="(different @requires clauses on member)",_tag_dyneither(_tmp1104,sizeof(char),40)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1770
if(_tmp4BA == 0  && _tmp4B7 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp1105;Cyc_Tcutil_failure_reason=((_tmp1105="(different number of members)",_tag_dyneither(_tmp1105,sizeof(char),30)));}
goto _LL2C9;_LL2EC:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp4BB=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp447.f1;if(_tmp4BB->tag != 17)goto _LL2EE;else{_tmp4BC=_tmp4BB->f2;_tmp4BD=_tmp4BB->f3;}}{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp4BE=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp447.f2;if(_tmp4BE->tag != 17)goto _LL2EE;else{_tmp4BF=_tmp4BE->f2;_tmp4C0=_tmp4BE->f3;}};_LL2ED:
# 1776
 if(_tmp4BD != _tmp4C0){
{const char*_tmp1106;Cyc_Tcutil_failure_reason=((_tmp1106="(different abstract typedefs)",_tag_dyneither(_tmp1106,sizeof(char),30)));}
goto _LL2C9;}
# 1780
{const char*_tmp1107;Cyc_Tcutil_failure_reason=((_tmp1107="(type parameters to typedef differ)",_tag_dyneither(_tmp1107,sizeof(char),36)));}
Cyc_Tcutil_unify_list(_tmp4BC,_tmp4BF);
return;_LL2EE:{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp4C1=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp447.f1;if(_tmp4C1->tag != 20)goto _LL2F0;}{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp4C2=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp447.f2;if(_tmp4C2->tag != 20)goto _LL2F0;};_LL2EF:
 return;_LL2F0:{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp4C3=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp447.f1;if(_tmp4C3->tag != 21)goto _LL2F2;}{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp4C4=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp447.f2;if(_tmp4C4->tag != 21)goto _LL2F2;};_LL2F1:
 return;_LL2F2:{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp4C5=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp447.f1;if(_tmp4C5->tag != 22)goto _LL2F4;}{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp4C6=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp447.f2;if(_tmp4C6->tag != 22)goto _LL2F4;};_LL2F3:
 return;_LL2F4:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4C7=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp447.f1;if(_tmp4C7->tag != 15)goto _LL2F6;else{_tmp4C8=(void*)_tmp4C7->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4C9=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp447.f2;if(_tmp4C9->tag != 15)goto _LL2F6;else{_tmp4CA=(void*)_tmp4C9->f1;}};_LL2F5:
# 1787
 Cyc_Tcutil_unify_it(_tmp4C8,_tmp4CA);
return;_LL2F6:{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp4CB=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp447.f1;if(_tmp4CB->tag != 16)goto _LL2F8;else{_tmp4CC=(void*)_tmp4CB->f1;_tmp4CD=(void*)_tmp4CB->f2;}}{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp4CE=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp447.f2;if(_tmp4CE->tag != 16)goto _LL2F8;else{_tmp4CF=(void*)_tmp4CE->f1;_tmp4D0=(void*)_tmp4CE->f2;}};_LL2F7:
# 1790
 Cyc_Tcutil_unify_it(_tmp4CC,_tmp4CF);
Cyc_Tcutil_unify_it(_tmp4CD,_tmp4D0);
return;_LL2F8: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp4D1=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp447.f1;if(_tmp4D1->tag != 24)goto _LL2FA;}_LL2F9:
 goto _LL2FB;_LL2FA: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp4D2=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp447.f2;if(_tmp4D2->tag != 24)goto _LL2FC;}_LL2FB:
 goto _LL2FD;_LL2FC: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4D3=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp447.f1;if(_tmp4D3->tag != 23)goto _LL2FE;}_LL2FD:
 goto _LL2FF;_LL2FE: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp4D4=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp447.f1;if(_tmp4D4->tag != 25)goto _LL300;}_LL2FF:
 goto _LL301;_LL300: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp4D5=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp447.f2;if(_tmp4D5->tag != 25)goto _LL302;}_LL301:
 goto _LL303;_LL302: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4D6=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp447.f2;if(_tmp4D6->tag != 23)goto _LL304;}_LL303:
# 1799
 if(Cyc_Tcutil_unify_effect(t1,t2))return;
{const char*_tmp1108;Cyc_Tcutil_failure_reason=((_tmp1108="(effects don't unify)",_tag_dyneither(_tmp1108,sizeof(char),22)));}
goto _LL2C9;_LL304:;_LL305:
 goto _LL2C9;_LL2C9:;}
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
int _tmp540=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
int _tmp541=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp540,_tmp541);}
# 1821
static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return 0;{
union Cyc_Absyn_Constraint*_tmp542=x;void*_tmp543;_LL31E: if((_tmp542->No_constr).tag != 3)goto _LL320;_LL31F:
 return - 1;_LL320: if((_tmp542->Eq_constr).tag != 1)goto _LL322;_tmp543=(void*)(_tmp542->Eq_constr).val;_LL321: {
# 1828
union Cyc_Absyn_Constraint*_tmp544=y;void*_tmp545;_LL325: if((_tmp544->No_constr).tag != 3)goto _LL327;_LL326:
 return 1;_LL327: if((_tmp544->Eq_constr).tag != 1)goto _LL329;_tmp545=(void*)(_tmp544->Eq_constr).val;_LL328:
 return cmp(_tmp543,_tmp545);_LL329: if((_tmp544->Forward_constr).tag != 2)goto _LL324;_LL32A: {
const char*_tmp110B;void*_tmp110A;(_tmp110A=0,Cyc_Tcutil_impos(((_tmp110B="unify_conref: forward after compress(2)",_tag_dyneither(_tmp110B,sizeof(char),40))),_tag_dyneither(_tmp110A,sizeof(void*),0)));}_LL324:;}_LL322: if((_tmp542->Forward_constr).tag != 2)goto _LL31D;_LL323: {
# 1833
const char*_tmp110E;void*_tmp110D;(_tmp110D=0,Cyc_Tcutil_impos(((_tmp110E="unify_conref: forward after compress",_tag_dyneither(_tmp110E,sizeof(char),37))),_tag_dyneither(_tmp110D,sizeof(void*),0)));}_LL31D:;};}
# 1837
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct Cyc_Absyn_Tqual _tmp54B;void*_tmp54C;struct _tuple12*_tmp54A=tqt1;_tmp54B=_tmp54A->f1;_tmp54C=_tmp54A->f2;{
struct Cyc_Absyn_Tqual _tmp54E;void*_tmp54F;struct _tuple12*_tmp54D=tqt2;_tmp54E=_tmp54D->f1;_tmp54F=_tmp54D->f2;{
int _tmp550=Cyc_Tcutil_tqual_cmp(_tmp54B,_tmp54E);
if(_tmp550 != 0)return _tmp550;
return Cyc_Tcutil_typecmp(_tmp54C,_tmp54F);};};}
# 1845
static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp551=Cyc_strptrcmp(f1->name,f2->name);
if(_tmp551 != 0)return _tmp551;{
int _tmp552=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);
if(_tmp552 != 0)return _tmp552;{
int _tmp553=Cyc_Tcutil_typecmp(f1->type,f2->type);
if(_tmp553 != 0)return _tmp553;{
int _tmp554=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);
if(_tmp554 != 0)return _tmp554;
_tmp554=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);
if(_tmp554 != 0)return _tmp554;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->requires_clause,f2->requires_clause);};};};}
# 1859
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp555=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp555 != 0)return _tmp555;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}
# 1865
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp556=t;_LL32C: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp557=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp556;if(_tmp557->tag != 0)goto _LL32E;}_LL32D:
 return 0;_LL32E: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp558=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp556;if(_tmp558->tag != 1)goto _LL330;}_LL32F:
 return 1;_LL330: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp559=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp556;if(_tmp559->tag != 2)goto _LL332;}_LL331:
 return 2;_LL332: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp55A=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp556;if(_tmp55A->tag != 3)goto _LL334;}_LL333:
 return 3;_LL334: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp55B=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp556;if(_tmp55B->tag != 4)goto _LL336;}_LL335:
 return 4;_LL336: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp55C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp556;if(_tmp55C->tag != 5)goto _LL338;}_LL337:
 return 5;_LL338: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp55D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp556;if(_tmp55D->tag != 6)goto _LL33A;}_LL339:
 return 6;_LL33A: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp55E=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp556;if(_tmp55E->tag != 7)goto _LL33C;else{if(_tmp55E->f1 != 0)goto _LL33C;}}_LL33B:
 return 7;_LL33C: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp55F=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp556;if(_tmp55F->tag != 7)goto _LL33E;}_LL33D:
 return 8;_LL33E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp560=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp556;if(_tmp560->tag != 8)goto _LL340;}_LL33F:
 return 9;_LL340: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp561=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp556;if(_tmp561->tag != 9)goto _LL342;}_LL341:
 return 10;_LL342: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp562=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp556;if(_tmp562->tag != 10)goto _LL344;}_LL343:
 return 11;_LL344: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp563=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp556;if(_tmp563->tag != 11)goto _LL346;}_LL345:
 return 12;_LL346: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp564=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp556;if(_tmp564->tag != 12)goto _LL348;}_LL347:
 return 14;_LL348: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp565=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp556;if(_tmp565->tag != 13)goto _LL34A;}_LL349:
 return 16;_LL34A: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp566=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp556;if(_tmp566->tag != 14)goto _LL34C;}_LL34B:
 return 17;_LL34C: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp567=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp556;if(_tmp567->tag != 15)goto _LL34E;}_LL34D:
 return 18;_LL34E: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp568=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp556;if(_tmp568->tag != 17)goto _LL350;}_LL34F:
 return 19;_LL350: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp569=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp556;if(_tmp569->tag != 21)goto _LL352;}_LL351:
 return 20;_LL352: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp56A=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp556;if(_tmp56A->tag != 20)goto _LL354;}_LL353:
 return 21;_LL354: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp56B=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp556;if(_tmp56B->tag != 23)goto _LL356;}_LL355:
 return 22;_LL356: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp56C=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp556;if(_tmp56C->tag != 24)goto _LL358;}_LL357:
 return 23;_LL358: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp56D=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp556;if(_tmp56D->tag != 25)goto _LL35A;}_LL359:
 return 24;_LL35A: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp56E=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp556;if(_tmp56E->tag != 22)goto _LL35C;}_LL35B:
 return 25;_LL35C: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp56F=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp556;if(_tmp56F->tag != 19)goto _LL35E;}_LL35D:
 return 26;_LL35E: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp570=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp556;if(_tmp570->tag != 16)goto _LL360;}_LL35F:
 return 27;_LL360: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp571=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp556;if(_tmp571->tag != 18)goto _LL362;}_LL361:
 return 28;_LL362: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp572=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp556;if(_tmp572->tag != 26)goto _LL364;}_LL363:
 return 29;_LL364: {struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp573=(struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp556;if(_tmp573->tag != 27)goto _LL366;}_LL365:
 return 30;_LL366: {struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*_tmp574=(struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp556;if(_tmp574->tag != 28)goto _LL32B;}_LL367:
 return 31;_LL32B:;}
# 1902
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return 0;{
int _tmp575=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));
if(_tmp575 != 0)
return _tmp575;{
# 1911
struct _tuple0 _tmp110F;struct _tuple0 _tmp576=(_tmp110F.f1=t2,((_tmp110F.f2=t1,_tmp110F)));struct _tuple0 _tmp577=_tmp576;struct Cyc_Absyn_Tvar*_tmp57B;struct Cyc_Absyn_Tvar*_tmp57D;union Cyc_Absyn_AggrInfoU _tmp57F;struct Cyc_List_List*_tmp580;union Cyc_Absyn_AggrInfoU _tmp582;struct Cyc_List_List*_tmp583;struct _tuple2*_tmp585;struct _tuple2*_tmp587;struct Cyc_List_List*_tmp589;struct Cyc_List_List*_tmp58B;struct Cyc_Absyn_Datatypedecl*_tmp58D;struct Cyc_List_List*_tmp58E;struct Cyc_Absyn_Datatypedecl*_tmp590;struct Cyc_List_List*_tmp591;struct Cyc_Absyn_Datatypedecl*_tmp593;struct Cyc_Absyn_Datatypefield*_tmp594;struct Cyc_List_List*_tmp595;struct Cyc_Absyn_Datatypedecl*_tmp597;struct Cyc_Absyn_Datatypefield*_tmp598;struct Cyc_List_List*_tmp599;void*_tmp59B;struct Cyc_Absyn_Tqual _tmp59C;void*_tmp59D;union Cyc_Absyn_Constraint*_tmp59E;union Cyc_Absyn_Constraint*_tmp59F;union Cyc_Absyn_Constraint*_tmp5A0;void*_tmp5A2;struct Cyc_Absyn_Tqual _tmp5A3;void*_tmp5A4;union Cyc_Absyn_Constraint*_tmp5A5;union Cyc_Absyn_Constraint*_tmp5A6;union Cyc_Absyn_Constraint*_tmp5A7;enum Cyc_Absyn_Sign _tmp5A9;enum Cyc_Absyn_Size_of _tmp5AA;enum Cyc_Absyn_Sign _tmp5AC;enum Cyc_Absyn_Size_of _tmp5AD;int _tmp5AF;int _tmp5B1;void*_tmp5B3;struct Cyc_Absyn_Tqual _tmp5B4;struct Cyc_Absyn_Exp*_tmp5B5;union Cyc_Absyn_Constraint*_tmp5B6;void*_tmp5B8;struct Cyc_Absyn_Tqual _tmp5B9;struct Cyc_Absyn_Exp*_tmp5BA;union Cyc_Absyn_Constraint*_tmp5BB;struct Cyc_List_List*_tmp5BF;struct Cyc_List_List*_tmp5C1;enum Cyc_Absyn_AggrKind _tmp5C3;struct Cyc_List_List*_tmp5C4;enum Cyc_Absyn_AggrKind _tmp5C6;struct Cyc_List_List*_tmp5C7;void*_tmp5C9;void*_tmp5CB;void*_tmp5CD;void*_tmp5CE;void*_tmp5D0;void*_tmp5D1;void*_tmp5D3;void*_tmp5D5;struct Cyc_Absyn_Exp*_tmp5D7;struct Cyc_Absyn_Exp*_tmp5D9;struct Cyc_Absyn_Exp*_tmp5DB;struct Cyc_Absyn_Exp*_tmp5DD;struct _dyneither_ptr _tmp5DF;struct _dyneither_ptr _tmp5E1;_LL369:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp578=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp577.f1;if(_tmp578->tag != 1)goto _LL36B;}{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp579=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp577.f2;if(_tmp579->tag != 1)goto _LL36B;};_LL36A: {
# 1913
const char*_tmp1112;void*_tmp1111;(_tmp1111=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1112="typecmp: can only compare closed types",_tag_dyneither(_tmp1112,sizeof(char),39))),_tag_dyneither(_tmp1111,sizeof(void*),0)));}_LL36B:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp57A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp577.f1;if(_tmp57A->tag != 2)goto _LL36D;else{_tmp57B=_tmp57A->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp57C=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp577.f2;if(_tmp57C->tag != 2)goto _LL36D;else{_tmp57D=_tmp57C->f1;}};_LL36C:
# 1917
 return Cyc_Core_intcmp(_tmp57D->identity,_tmp57B->identity);_LL36D:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp57E=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp577.f1;if(_tmp57E->tag != 11)goto _LL36F;else{_tmp57F=(_tmp57E->f1).aggr_info;_tmp580=(_tmp57E->f1).targs;}}{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp581=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp577.f2;if(_tmp581->tag != 11)goto _LL36F;else{_tmp582=(_tmp581->f1).aggr_info;_tmp583=(_tmp581->f1).targs;}};_LL36E: {
# 1921
struct _tuple13 _tmp5EA=Cyc_Absyn_aggr_kinded_name(_tmp57F);struct _tuple2*_tmp5EC;struct _tuple13 _tmp5EB=_tmp5EA;_tmp5EC=_tmp5EB.f2;{
struct _tuple13 _tmp5ED=Cyc_Absyn_aggr_kinded_name(_tmp582);struct _tuple2*_tmp5EF;struct _tuple13 _tmp5EE=_tmp5ED;_tmp5EF=_tmp5EE.f2;{
int _tmp5F0=Cyc_Absyn_qvar_cmp(_tmp5EC,_tmp5EF);
if(_tmp5F0 != 0)return _tmp5F0;else{
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp580,_tmp583);}};};}_LL36F:{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp584=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp577.f1;if(_tmp584->tag != 13)goto _LL371;else{_tmp585=_tmp584->f1;}}{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp586=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp577.f2;if(_tmp586->tag != 13)goto _LL371;else{_tmp587=_tmp586->f1;}};_LL370:
# 1928
 return Cyc_Absyn_qvar_cmp(_tmp585,_tmp587);_LL371:{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp588=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp577.f1;if(_tmp588->tag != 14)goto _LL373;else{_tmp589=_tmp588->f1;}}{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp58A=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp577.f2;if(_tmp58A->tag != 14)goto _LL373;else{_tmp58B=_tmp58A->f1;}};_LL372:
# 1931
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp589,_tmp58B);_LL373:{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp58C=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp577.f1;if(_tmp58C->tag != 3)goto _LL375;else{if((((_tmp58C->f1).datatype_info).KnownDatatype).tag != 2)goto _LL375;_tmp58D=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp58C->f1).datatype_info).KnownDatatype).val);_tmp58E=(_tmp58C->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp58F=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp577.f2;if(_tmp58F->tag != 3)goto _LL375;else{if((((_tmp58F->f1).datatype_info).KnownDatatype).tag != 2)goto _LL375;_tmp590=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp58F->f1).datatype_info).KnownDatatype).val);_tmp591=(_tmp58F->f1).targs;}};_LL374:
# 1935
 if(_tmp590 == _tmp58D)return 0;{
int _tmp5F1=Cyc_Absyn_qvar_cmp(_tmp590->name,_tmp58D->name);
if(_tmp5F1 != 0)return _tmp5F1;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp591,_tmp58E);};_LL375:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp592=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp577.f1;if(_tmp592->tag != 4)goto _LL377;else{if((((_tmp592->f1).field_info).KnownDatatypefield).tag != 2)goto _LL377;_tmp593=((struct _tuple3)(((_tmp592->f1).field_info).KnownDatatypefield).val).f1;_tmp594=((struct _tuple3)(((_tmp592->f1).field_info).KnownDatatypefield).val).f2;_tmp595=(_tmp592->f1).targs;}}{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp596=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp577.f2;if(_tmp596->tag != 4)goto _LL377;else{if((((_tmp596->f1).field_info).KnownDatatypefield).tag != 2)goto _LL377;_tmp597=((struct _tuple3)(((_tmp596->f1).field_info).KnownDatatypefield).val).f1;_tmp598=((struct _tuple3)(((_tmp596->f1).field_info).KnownDatatypefield).val).f2;_tmp599=(_tmp596->f1).targs;}};_LL376:
# 1942
 if(_tmp597 == _tmp593)return 0;{
int _tmp5F2=Cyc_Absyn_qvar_cmp(_tmp593->name,_tmp597->name);
if(_tmp5F2 != 0)return _tmp5F2;{
int _tmp5F3=Cyc_Absyn_qvar_cmp(_tmp594->name,_tmp598->name);
if(_tmp5F3 != 0)return _tmp5F3;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp599,_tmp595);};};_LL377:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp59A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp577.f1;if(_tmp59A->tag != 5)goto _LL379;else{_tmp59B=(_tmp59A->f1).elt_typ;_tmp59C=(_tmp59A->f1).elt_tq;_tmp59D=((_tmp59A->f1).ptr_atts).rgn;_tmp59E=((_tmp59A->f1).ptr_atts).nullable;_tmp59F=((_tmp59A->f1).ptr_atts).bounds;_tmp5A0=((_tmp59A->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5A1=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp577.f2;if(_tmp5A1->tag != 5)goto _LL379;else{_tmp5A2=(_tmp5A1->f1).elt_typ;_tmp5A3=(_tmp5A1->f1).elt_tq;_tmp5A4=((_tmp5A1->f1).ptr_atts).rgn;_tmp5A5=((_tmp5A1->f1).ptr_atts).nullable;_tmp5A6=((_tmp5A1->f1).ptr_atts).bounds;_tmp5A7=((_tmp5A1->f1).ptr_atts).zero_term;}};_LL378: {
# 1951
int _tmp5F4=Cyc_Tcutil_typecmp(_tmp5A2,_tmp59B);
if(_tmp5F4 != 0)return _tmp5F4;{
int _tmp5F5=Cyc_Tcutil_typecmp(_tmp5A4,_tmp59D);
if(_tmp5F5 != 0)return _tmp5F5;{
int _tmp5F6=Cyc_Tcutil_tqual_cmp(_tmp5A3,_tmp59C);
if(_tmp5F6 != 0)return _tmp5F6;{
int _tmp5F7=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp5A6,_tmp59F);
if(_tmp5F7 != 0)return _tmp5F7;{
int _tmp5F8=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp5A7,_tmp5A0);
if(_tmp5F8 != 0)return _tmp5F8;
{void*_tmp5F9=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5A6);void*_tmp5FA=_tmp5F9;_LL3A0: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5FB=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5FA;if(_tmp5FB->tag != 0)goto _LL3A2;}_LL3A1:
 return 0;_LL3A2:;_LL3A3:
 goto _LL39F;_LL39F:;}
# 1965
return((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp5A5,_tmp59E);};};};};}_LL379:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp577.f1;if(_tmp5A8->tag != 6)goto _LL37B;else{_tmp5A9=_tmp5A8->f1;_tmp5AA=_tmp5A8->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp577.f2;if(_tmp5AB->tag != 6)goto _LL37B;else{_tmp5AC=_tmp5AB->f1;_tmp5AD=_tmp5AB->f2;}};_LL37A:
# 1968
 if(_tmp5AC != _tmp5A9)return Cyc_Core_intcmp((int)((unsigned int)_tmp5AC),(int)((unsigned int)_tmp5A9));
if(_tmp5AD != _tmp5AA)return Cyc_Core_intcmp((int)((unsigned int)_tmp5AD),(int)((unsigned int)_tmp5AA));
return 0;_LL37B:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5AE=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp577.f1;if(_tmp5AE->tag != 7)goto _LL37D;else{_tmp5AF=_tmp5AE->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5B0=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp577.f2;if(_tmp5B0->tag != 7)goto _LL37D;else{_tmp5B1=_tmp5B0->f1;}};_LL37C:
# 1973
 if(_tmp5AF == _tmp5B1)return 0;else{
if(_tmp5B1 == 0)return - 1;else{
if(_tmp5B1 == 1  && _tmp5AF == 0)return - 1;else{
return 1;}}}_LL37D:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5B2=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp577.f1;if(_tmp5B2->tag != 8)goto _LL37F;else{_tmp5B3=(_tmp5B2->f1).elt_type;_tmp5B4=(_tmp5B2->f1).tq;_tmp5B5=(_tmp5B2->f1).num_elts;_tmp5B6=(_tmp5B2->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5B7=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp577.f2;if(_tmp5B7->tag != 8)goto _LL37F;else{_tmp5B8=(_tmp5B7->f1).elt_type;_tmp5B9=(_tmp5B7->f1).tq;_tmp5BA=(_tmp5B7->f1).num_elts;_tmp5BB=(_tmp5B7->f1).zero_term;}};_LL37E: {
# 1980
int _tmp5FC=Cyc_Tcutil_tqual_cmp(_tmp5B9,_tmp5B4);
if(_tmp5FC != 0)return _tmp5FC;{
int _tmp5FD=Cyc_Tcutil_typecmp(_tmp5B8,_tmp5B3);
if(_tmp5FD != 0)return _tmp5FD;{
int _tmp5FE=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp5B6,_tmp5BB);
if(_tmp5FE != 0)return _tmp5FE;
if(_tmp5B5 == _tmp5BA)return 0;
if(_tmp5B5 == 0  || _tmp5BA == 0){
const char*_tmp1115;void*_tmp1114;(_tmp1114=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1115="missing expression in array index",_tag_dyneither(_tmp1115,sizeof(char),34))),_tag_dyneither(_tmp1114,sizeof(void*),0)));}
# 1990
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp5B5,_tmp5BA);};};}_LL37F:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp5BC=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp577.f1;if(_tmp5BC->tag != 9)goto _LL381;}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp5BD=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp577.f2;if(_tmp5BD->tag != 9)goto _LL381;};_LL380: {
# 1993
const char*_tmp1118;void*_tmp1117;(_tmp1117=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1118="typecmp: function types not handled",_tag_dyneither(_tmp1118,sizeof(char),36))),_tag_dyneither(_tmp1117,sizeof(void*),0)));}_LL381:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp5BE=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp577.f1;if(_tmp5BE->tag != 10)goto _LL383;else{_tmp5BF=_tmp5BE->f1;}}{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp5C0=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp577.f2;if(_tmp5C0->tag != 10)goto _LL383;else{_tmp5C1=_tmp5C0->f1;}};_LL382:
# 1996
 return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp5C1,_tmp5BF);_LL383:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp5C2=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp577.f1;if(_tmp5C2->tag != 12)goto _LL385;else{_tmp5C3=_tmp5C2->f1;_tmp5C4=_tmp5C2->f2;}}{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp5C5=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp577.f2;if(_tmp5C5->tag != 12)goto _LL385;else{_tmp5C6=_tmp5C5->f1;_tmp5C7=_tmp5C5->f2;}};_LL384:
# 1999
 if(_tmp5C6 != _tmp5C3){
if(_tmp5C6 == Cyc_Absyn_StructA)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp5C7,_tmp5C4);_LL385:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp5C8=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp577.f1;if(_tmp5C8->tag != 15)goto _LL387;else{_tmp5C9=(void*)_tmp5C8->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp5CA=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp577.f2;if(_tmp5CA->tag != 15)goto _LL387;else{_tmp5CB=(void*)_tmp5CA->f1;}};_LL386:
# 2004
 return Cyc_Tcutil_typecmp(_tmp5C9,_tmp5CB);_LL387:{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp5CC=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp577.f1;if(_tmp5CC->tag != 16)goto _LL389;else{_tmp5CD=(void*)_tmp5CC->f1;_tmp5CE=(void*)_tmp5CC->f2;}}{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp5CF=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp577.f2;if(_tmp5CF->tag != 16)goto _LL389;else{_tmp5D0=(void*)_tmp5CF->f1;_tmp5D1=(void*)_tmp5CF->f2;}};_LL388: {
# 2006
int _tmp603=Cyc_Tcutil_typecmp(_tmp5CD,_tmp5D0);
if(_tmp603 != 0)return _tmp603;else{
return Cyc_Tcutil_typecmp(_tmp5CE,_tmp5D1);}}_LL389:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5D2=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp577.f1;if(_tmp5D2->tag != 19)goto _LL38B;else{_tmp5D3=(void*)_tmp5D2->f1;}}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5D4=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp577.f2;if(_tmp5D4->tag != 19)goto _LL38B;else{_tmp5D5=(void*)_tmp5D4->f1;}};_LL38A:
 return Cyc_Tcutil_typecmp(_tmp5D3,_tmp5D5);_LL38B:{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp5D6=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp577.f1;if(_tmp5D6->tag != 18)goto _LL38D;else{_tmp5D7=_tmp5D6->f1;}}{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp5D8=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp577.f2;if(_tmp5D8->tag != 18)goto _LL38D;else{_tmp5D9=_tmp5D8->f1;}};_LL38C:
 _tmp5DB=_tmp5D7;_tmp5DD=_tmp5D9;goto _LL38E;_LL38D:{struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp5DA=(struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp577.f1;if(_tmp5DA->tag != 27)goto _LL38F;else{_tmp5DB=_tmp5DA->f1;}}{struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp5DC=(struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp577.f2;if(_tmp5DC->tag != 27)goto _LL38F;else{_tmp5DD=_tmp5DC->f1;}};_LL38E:
# 2012
 return Cyc_Evexp_const_exp_cmp(_tmp5DB,_tmp5DD);_LL38F:{struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*_tmp5DE=(struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp577.f1;if(_tmp5DE->tag != 28)goto _LL391;else{_tmp5DF=_tmp5DE->f1;}}{struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*_tmp5E0=(struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp577.f2;if(_tmp5E0->tag != 28)goto _LL391;else{_tmp5E1=_tmp5E0->f1;}};_LL390:
# 2014
 return Cyc_strcmp((struct _dyneither_ptr)_tmp5DF,(struct _dyneither_ptr)_tmp5E1);_LL391: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp5E2=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp577.f1;if(_tmp5E2->tag != 24)goto _LL393;}_LL392:
 goto _LL394;_LL393: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp5E3=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp577.f2;if(_tmp5E3->tag != 24)goto _LL395;}_LL394:
 goto _LL396;_LL395: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp5E4=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp577.f1;if(_tmp5E4->tag != 23)goto _LL397;}_LL396:
 goto _LL398;_LL397: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp5E5=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp577.f1;if(_tmp5E5->tag != 25)goto _LL399;}_LL398:
 goto _LL39A;_LL399: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp5E6=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp577.f2;if(_tmp5E6->tag != 25)goto _LL39B;}_LL39A:
 goto _LL39C;_LL39B: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp5E7=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp577.f2;if(_tmp5E7->tag != 23)goto _LL39D;}_LL39C: {
const char*_tmp111B;void*_tmp111A;(_tmp111A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp111B="typecmp: effects not handled",_tag_dyneither(_tmp111B,sizeof(char),29))),_tag_dyneither(_tmp111A,sizeof(void*),0)));}_LL39D:;_LL39E: {
const char*_tmp111E;void*_tmp111D;(_tmp111D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp111E="Unmatched case in typecmp",_tag_dyneither(_tmp111E,sizeof(char),26))),_tag_dyneither(_tmp111D,sizeof(void*),0)));}_LL368:;};};}
# 2025
int Cyc_Tcutil_is_arithmetic_type(void*t){
void*_tmp609=Cyc_Tcutil_compress(t);void*_tmp60A=_tmp609;_LL3A5: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp60B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp60A;if(_tmp60B->tag != 6)goto _LL3A7;}_LL3A6:
 goto _LL3A8;_LL3A7: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp60C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp60A;if(_tmp60C->tag != 7)goto _LL3A9;}_LL3A8:
 goto _LL3AA;_LL3A9: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp60D=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp60A;if(_tmp60D->tag != 13)goto _LL3AB;}_LL3AA:
 goto _LL3AC;_LL3AB: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp60E=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp60A;if(_tmp60E->tag != 14)goto _LL3AD;}_LL3AC:
 return 1;_LL3AD:;_LL3AE:
 return 0;_LL3A4:;}
# 2037
int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp111F;struct _tuple0 _tmp60F=(_tmp111F.f1=t1,((_tmp111F.f2=t2,_tmp111F)));struct _tuple0 _tmp610=_tmp60F;int _tmp612;int _tmp614;_LL3B0:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp611=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp610.f1;if(_tmp611->tag != 7)goto _LL3B2;else{_tmp612=_tmp611->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp613=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp610.f2;if(_tmp613->tag != 7)goto _LL3B2;else{_tmp614=_tmp613->f1;}};_LL3B1:
# 2042
 return _tmp614 == 0  && _tmp612 != 0  || (_tmp614 == 1  && _tmp612 != 0) && _tmp612 != 1;_LL3B2:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp615=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp610.f1;if(_tmp615->tag != 7)goto _LL3B4;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp616=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp610.f2;if(_tmp616->tag != 6)goto _LL3B4;};_LL3B3:
 goto _LL3B5;_LL3B4:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp617=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp610.f1;if(_tmp617->tag != 7)goto _LL3B6;}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp618=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp610.f2;if(_tmp618->tag != 19)goto _LL3B6;};_LL3B5:
 return 1;_LL3B6:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp619=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp610.f1;if(_tmp619->tag != 6)goto _LL3B8;else{if(_tmp619->f2 != Cyc_Absyn_LongLong_sz)goto _LL3B8;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp61A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp610.f2;if(_tmp61A->tag != 6)goto _LL3B8;else{if(_tmp61A->f2 != Cyc_Absyn_LongLong_sz)goto _LL3B8;}};_LL3B7:
 return 0;_LL3B8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp61B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp610.f1;if(_tmp61B->tag != 6)goto _LL3BA;else{if(_tmp61B->f2 != Cyc_Absyn_LongLong_sz)goto _LL3BA;}}_LL3B9:
 return 1;_LL3BA:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp61C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp610.f1;if(_tmp61C->tag != 6)goto _LL3BC;else{if(_tmp61C->f2 != Cyc_Absyn_Long_sz)goto _LL3BC;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp61D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp610.f2;if(_tmp61D->tag != 6)goto _LL3BC;else{if(_tmp61D->f2 != Cyc_Absyn_Int_sz)goto _LL3BC;}};_LL3BB:
# 2049
 goto _LL3BD;_LL3BC:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp61E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp610.f1;if(_tmp61E->tag != 6)goto _LL3BE;else{if(_tmp61E->f2 != Cyc_Absyn_Int_sz)goto _LL3BE;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp61F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp610.f2;if(_tmp61F->tag != 6)goto _LL3BE;else{if(_tmp61F->f2 != Cyc_Absyn_Long_sz)goto _LL3BE;}};_LL3BD:
 return 0;_LL3BE:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp620=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp610.f1;if(_tmp620->tag != 6)goto _LL3C0;else{if(_tmp620->f2 != Cyc_Absyn_Long_sz)goto _LL3C0;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp621=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp610.f2;if(_tmp621->tag != 7)goto _LL3C0;else{if(_tmp621->f1 != 0)goto _LL3C0;}};_LL3BF:
# 2052
 goto _LL3C1;_LL3C0:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp622=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp610.f1;if(_tmp622->tag != 6)goto _LL3C2;else{if(_tmp622->f2 != Cyc_Absyn_Int_sz)goto _LL3C2;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp623=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp610.f2;if(_tmp623->tag != 7)goto _LL3C2;else{if(_tmp623->f1 != 0)goto _LL3C2;}};_LL3C1:
 goto _LL3C3;_LL3C2:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp624=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp610.f1;if(_tmp624->tag != 6)goto _LL3C4;else{if(_tmp624->f2 != Cyc_Absyn_Long_sz)goto _LL3C4;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp625=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp610.f2;if(_tmp625->tag != 6)goto _LL3C4;else{if(_tmp625->f2 != Cyc_Absyn_Short_sz)goto _LL3C4;}};_LL3C3:
 goto _LL3C5;_LL3C4:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp626=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp610.f1;if(_tmp626->tag != 6)goto _LL3C6;else{if(_tmp626->f2 != Cyc_Absyn_Int_sz)goto _LL3C6;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp627=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp610.f2;if(_tmp627->tag != 6)goto _LL3C6;else{if(_tmp627->f2 != Cyc_Absyn_Short_sz)goto _LL3C6;}};_LL3C5:
 goto _LL3C7;_LL3C6:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp628=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp610.f1;if(_tmp628->tag != 19)goto _LL3C8;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp629=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp610.f2;if(_tmp629->tag != 6)goto _LL3C8;else{if(_tmp629->f2 != Cyc_Absyn_Short_sz)goto _LL3C8;}};_LL3C7:
 goto _LL3C9;_LL3C8:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp62A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp610.f1;if(_tmp62A->tag != 6)goto _LL3CA;else{if(_tmp62A->f2 != Cyc_Absyn_Long_sz)goto _LL3CA;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp62B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp610.f2;if(_tmp62B->tag != 6)goto _LL3CA;else{if(_tmp62B->f2 != Cyc_Absyn_Char_sz)goto _LL3CA;}};_LL3C9:
 goto _LL3CB;_LL3CA:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp62C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp610.f1;if(_tmp62C->tag != 6)goto _LL3CC;else{if(_tmp62C->f2 != Cyc_Absyn_Int_sz)goto _LL3CC;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp62D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp610.f2;if(_tmp62D->tag != 6)goto _LL3CC;else{if(_tmp62D->f2 != Cyc_Absyn_Char_sz)goto _LL3CC;}};_LL3CB:
 goto _LL3CD;_LL3CC:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp62E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp610.f1;if(_tmp62E->tag != 6)goto _LL3CE;else{if(_tmp62E->f2 != Cyc_Absyn_Short_sz)goto _LL3CE;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp62F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp610.f2;if(_tmp62F->tag != 6)goto _LL3CE;else{if(_tmp62F->f2 != Cyc_Absyn_Char_sz)goto _LL3CE;}};_LL3CD:
 goto _LL3CF;_LL3CE:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp630=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp610.f1;if(_tmp630->tag != 19)goto _LL3D0;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp631=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp610.f2;if(_tmp631->tag != 6)goto _LL3D0;else{if(_tmp631->f2 != Cyc_Absyn_Char_sz)goto _LL3D0;}};_LL3CF:
 return 1;_LL3D0:;_LL3D1:
# 2062
 return 0;_LL3AF:;};}
# 2068
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
# 2071
struct _RegionHandle _tmp633=_new_region("r");struct _RegionHandle*r=& _tmp633;_push_region(r);{
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0  || 
Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v)){
struct Cyc_Core_Opt*_tmp1120;max_arith_type=((_tmp1120=_region_malloc(r,sizeof(*_tmp1120)),((_tmp1120->v=t1,_tmp1120))));}}}}
# 2081
if(max_arith_type != 0){
if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v)){
int _tmp635=0;_npop_handler(0);return _tmp635;}}}
# 2086
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){
{const char*_tmp1125;void*_tmp1124[2];struct Cyc_String_pa_PrintArg_struct _tmp1123;struct Cyc_String_pa_PrintArg_struct _tmp1122;(_tmp1122.tag=0,((_tmp1122.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))),((_tmp1123.tag=0,((_tmp1123.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1124[0]=& _tmp1123,((_tmp1124[1]=& _tmp1122,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)el->hd)->loc,((_tmp1125="type mismatch: expecting %s but found %s",_tag_dyneither(_tmp1125,sizeof(char),41))),_tag_dyneither(_tmp1124,sizeof(void*),2)))))))))))));}{
int _tmp63A=0;_npop_handler(0);return _tmp63A;};}}}{
# 2092
int _tmp63B=1;_npop_handler(0);return _tmp63B;};
# 2071
;_pop_region(r);}
# 2097
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_typ(te,e)){
void*_tmp63C=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp63D=_tmp63C;_LL3D3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp63E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp63D;if(_tmp63E->tag != 5)goto _LL3D5;}_LL3D4:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);goto _LL3D2;_LL3D5:;_LL3D6:
 return 0;_LL3D2:;}
# 2103
return 1;}
# 2106
int Cyc_Tcutil_is_integral_type(void*t){
void*_tmp63F=Cyc_Tcutil_compress(t);void*_tmp640=_tmp63F;_LL3D8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp641=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp640;if(_tmp641->tag != 6)goto _LL3DA;}_LL3D9:
 goto _LL3DB;_LL3DA: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp642=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp640;if(_tmp642->tag != 19)goto _LL3DC;}_LL3DB:
 goto _LL3DD;_LL3DC: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp643=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp640;if(_tmp643->tag != 13)goto _LL3DE;}_LL3DD:
 goto _LL3DF;_LL3DE: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp644=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp640;if(_tmp644->tag != 14)goto _LL3E0;}_LL3DF:
 return 1;_LL3E0:;_LL3E1:
 return 0;_LL3D7:;}
# 2117
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
return 1;
# 2121
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_typ)){
const char*_tmp1128;void*_tmp1127;(_tmp1127=0,Cyc_Tcutil_warn(e->loc,((_tmp1128="integral size mismatch; conversion supplied",_tag_dyneither(_tmp1128,sizeof(char),44))),_tag_dyneither(_tmp1127,sizeof(void*),0)));}
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
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_sint_typ)){
const char*_tmp112B;void*_tmp112A;(_tmp112A=0,Cyc_Tcutil_warn(e->loc,((_tmp112B="integral size mismatch; conversion supplied",_tag_dyneither(_tmp112B,sizeof(char),44))),_tag_dyneither(_tmp112A,sizeof(void*),0)));}
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
struct _tuple0 _tmp112C;struct _tuple0 _tmp649=(_tmp112C.f1=t1,((_tmp112C.f2=t2,_tmp112C)));struct _tuple0 _tmp64A=_tmp649;struct Cyc_Absyn_PtrInfo _tmp64C;struct Cyc_Absyn_PtrInfo _tmp64E;void*_tmp650;struct Cyc_Absyn_Tqual _tmp651;struct Cyc_Absyn_Exp*_tmp652;union Cyc_Absyn_Constraint*_tmp653;void*_tmp655;struct Cyc_Absyn_Tqual _tmp656;struct Cyc_Absyn_Exp*_tmp657;union Cyc_Absyn_Constraint*_tmp658;_LL3E3:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp64B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp64A.f1;if(_tmp64B->tag != 5)goto _LL3E5;else{_tmp64C=_tmp64B->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp64D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp64A.f2;if(_tmp64D->tag != 5)goto _LL3E5;else{_tmp64E=_tmp64D->f1;}};_LL3E4: {
# 2160
int okay=1;
# 2162
if(!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp64C.ptr_atts).nullable,(_tmp64E.ptr_atts).nullable))
# 2165
okay=!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp64C.ptr_atts).nullable);
# 2167
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp64C.ptr_atts).bounds,(_tmp64E.ptr_atts).bounds)){
# 2170
struct _tuple0 _tmp112D;struct _tuple0 _tmp65B=(_tmp112D.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp64C.ptr_atts).bounds),((_tmp112D.f2=
((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp64E.ptr_atts).bounds),_tmp112D)));
# 2170
struct _tuple0 _tmp65C=_tmp65B;struct Cyc_Absyn_Exp*_tmp662;struct Cyc_Absyn_Exp*_tmp664;struct Cyc_Absyn_Exp*_tmp667;_LL3EC:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp65D=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp65C.f1;if(_tmp65D->tag != 1)goto _LL3EE;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp65E=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp65C.f2;if(_tmp65E->tag != 0)goto _LL3EE;};_LL3ED:
# 2172
 goto _LL3EF;_LL3EE:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp65F=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp65C.f1;if(_tmp65F->tag != 0)goto _LL3F0;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp660=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp65C.f2;if(_tmp660->tag != 0)goto _LL3F0;};_LL3EF:
 okay=1;goto _LL3EB;_LL3F0:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp661=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp65C.f1;if(_tmp661->tag != 1)goto _LL3F2;else{_tmp662=_tmp661->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp663=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp65C.f2;if(_tmp663->tag != 1)goto _LL3F2;else{_tmp664=_tmp663->f1;}};_LL3F1:
# 2175
 okay=okay  && Cyc_Evexp_lte_const_exp(_tmp664,_tmp662);
# 2179
if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp64E.ptr_atts).zero_term)){
const char*_tmp1130;void*_tmp112F;(_tmp112F=0,Cyc_Tcutil_warn(loc,((_tmp1130="implicit cast to shorter array",_tag_dyneither(_tmp1130,sizeof(char),31))),_tag_dyneither(_tmp112F,sizeof(void*),0)));}
goto _LL3EB;_LL3F2:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp665=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp65C.f1;if(_tmp665->tag != 0)goto _LL3EB;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp666=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp65C.f2;if(_tmp666->tag != 1)goto _LL3EB;else{_tmp667=_tmp666->f1;}};_LL3F3:
# 2184
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp64C.ptr_atts).zero_term) && 
Cyc_Tcutil_is_bound_one((_tmp64E.ptr_atts).bounds))
goto _LL3EB;
okay=0;
goto _LL3EB;_LL3EB:;}
# 2193
okay=okay  && (!(_tmp64C.elt_tq).real_const  || (_tmp64E.elt_tq).real_const);
# 2196
if(!Cyc_Tcutil_unify((_tmp64C.ptr_atts).rgn,(_tmp64E.ptr_atts).rgn)){
if(Cyc_Tcenv_region_outlives(te,(_tmp64C.ptr_atts).rgn,(_tmp64E.ptr_atts).rgn)){
if(Cyc_Tcutil_warn_region_coerce){
const char*_tmp1135;void*_tmp1134[2];struct Cyc_String_pa_PrintArg_struct _tmp1133;struct Cyc_String_pa_PrintArg_struct _tmp1132;(_tmp1132.tag=0,((_tmp1132.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2201
Cyc_Absynpp_typ2string((_tmp64E.ptr_atts).rgn)),((_tmp1133.tag=0,((_tmp1133.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2200
Cyc_Absynpp_typ2string((_tmp64C.ptr_atts).rgn)),((_tmp1134[0]=& _tmp1133,((_tmp1134[1]=& _tmp1132,Cyc_Tcutil_warn(loc,((_tmp1135="implicit cast form region %s to region %s",_tag_dyneither(_tmp1135,sizeof(char),42))),_tag_dyneither(_tmp1134,sizeof(void*),2)))))))))))));}}else{
# 2202
okay=0;}}
# 2206
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp64C.ptr_atts).zero_term,(_tmp64E.ptr_atts).zero_term) || 
# 2209
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,(_tmp64C.ptr_atts).zero_term) && (_tmp64E.elt_tq).real_const);{
# 2217
int _tmp66F=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp64E.ptr_atts).bounds,Cyc_Absyn_bounds_one_conref) && !
# 2220
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp64E.ptr_atts).zero_term);
# 2224
okay=okay  && (Cyc_Tcutil_unify(_tmp64C.elt_typ,_tmp64E.elt_typ) || 
(_tmp66F  && ((_tmp64E.elt_tq).real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp64E.elt_typ)))) && Cyc_Tcutil_ptrsubtype(te,0,_tmp64C.elt_typ,_tmp64E.elt_typ));
# 2227
return okay;};}_LL3E5:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp64F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp64A.f1;if(_tmp64F->tag != 8)goto _LL3E7;else{_tmp650=(_tmp64F->f1).elt_type;_tmp651=(_tmp64F->f1).tq;_tmp652=(_tmp64F->f1).num_elts;_tmp653=(_tmp64F->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp654=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp64A.f2;if(_tmp654->tag != 8)goto _LL3E7;else{_tmp655=(_tmp654->f1).elt_type;_tmp656=(_tmp654->f1).tq;_tmp657=(_tmp654->f1).num_elts;_tmp658=(_tmp654->f1).zero_term;}};_LL3E6: {
# 2231
int okay;
# 2233
okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp653,_tmp658) && (
(_tmp652 != 0  && _tmp657 != 0) && Cyc_Evexp_same_const_exp(_tmp652,_tmp657));
# 2236
return(okay  && Cyc_Tcutil_unify(_tmp650,_tmp655)) && (!_tmp651.real_const  || _tmp656.real_const);}_LL3E7:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp659=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp64A.f1;if(_tmp659->tag != 19)goto _LL3E9;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp65A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp64A.f2;if(_tmp65A->tag != 6)goto _LL3E9;};_LL3E8:
# 2238
 return 0;_LL3E9:;_LL3EA:
# 2240
 return Cyc_Tcutil_unify(t1,t2);_LL3E2:;};}
# 2244
int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp671=Cyc_Tcutil_compress(t);void*_tmp672=_tmp671;_LL3F5: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp673=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp672;if(_tmp673->tag != 5)goto _LL3F7;}_LL3F6:
 return 1;_LL3F7:;_LL3F8:
 return 0;_LL3F4:;}
# 2250
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp674=Cyc_Tcutil_compress(t);void*_tmp675=_tmp674;void*_tmp677;_LL3FA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp676=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp675;if(_tmp676->tag != 5)goto _LL3FC;else{_tmp677=(_tmp676->f1).elt_typ;}}_LL3FB:
 return _tmp677;_LL3FC:;_LL3FD: {
const char*_tmp1138;void*_tmp1137;(_tmp1137=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1138="pointer_elt_type",_tag_dyneither(_tmp1138,sizeof(char),17))),_tag_dyneither(_tmp1137,sizeof(void*),0)));}_LL3F9:;}
# 2256
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp67A=Cyc_Tcutil_compress(t);void*_tmp67B=_tmp67A;struct Cyc_Absyn_PtrAtts*_tmp67D;_LL3FF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp67C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp67B;if(_tmp67C->tag != 5)goto _LL401;else{_tmp67D=(struct Cyc_Absyn_PtrAtts*)&(_tmp67C->f1).ptr_atts;}}_LL400:
 return _tmp67D->rgn;_LL401:;_LL402: {
const char*_tmp113B;void*_tmp113A;(_tmp113A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp113B="pointer_elt_type",_tag_dyneither(_tmp113B,sizeof(char),17))),_tag_dyneither(_tmp113A,sizeof(void*),0)));}_LL3FE:;}
# 2263
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp680=Cyc_Tcutil_compress(t);void*_tmp681=_tmp680;void*_tmp683;_LL404: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp682=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp681;if(_tmp682->tag != 5)goto _LL406;else{_tmp683=((_tmp682->f1).ptr_atts).rgn;}}_LL405:
# 2266
*rgn=_tmp683;
return 1;_LL406:;_LL407:
 return 0;_LL403:;}
# 2273
int Cyc_Tcutil_is_boxed(void*t){
return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;}
# 2277
int Cyc_Tcutil_is_dyneither_ptr(void*t){
void*_tmp684=Cyc_Tcutil_compress(t);void*_tmp685=_tmp684;union Cyc_Absyn_Constraint*_tmp687;_LL409: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp686=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp685;if(_tmp686->tag != 5)goto _LL40B;else{_tmp687=((_tmp686->f1).ptr_atts).bounds;}}_LL40A:
# 2280
 return((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp687)== (void*)& Cyc_Absyn_DynEither_b_val;_LL40B:;_LL40C:
 return 0;_LL408:;}
# 2286
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp688=e->r;void*_tmp689=_tmp688;struct _dyneither_ptr _tmp68F;void*_tmp691;struct Cyc_Absyn_Exp*_tmp692;_LL40E: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp68A=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp689;if(_tmp68A->tag != 0)goto _LL410;else{if(((_tmp68A->f1).Int_c).tag != 5)goto _LL410;if(((struct _tuple7)((_tmp68A->f1).Int_c).val).f2 != 0)goto _LL410;}}_LL40F:
 goto _LL411;_LL410: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp68B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp689;if(_tmp68B->tag != 0)goto _LL412;else{if(((_tmp68B->f1).Char_c).tag != 2)goto _LL412;if(((struct _tuple5)((_tmp68B->f1).Char_c).val).f2 != 0)goto _LL412;}}_LL411:
 goto _LL413;_LL412: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp68C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp689;if(_tmp68C->tag != 0)goto _LL414;else{if(((_tmp68C->f1).Short_c).tag != 4)goto _LL414;if(((struct _tuple6)((_tmp68C->f1).Short_c).val).f2 != 0)goto _LL414;}}_LL413:
 goto _LL415;_LL414: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp68D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp689;if(_tmp68D->tag != 0)goto _LL416;else{if(((_tmp68D->f1).LongLong_c).tag != 6)goto _LL416;if(((struct _tuple8)((_tmp68D->f1).LongLong_c).val).f2 != 0)goto _LL416;}}_LL415:
# 2292
 return 1;_LL416: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp68E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp689;if(_tmp68E->tag != 0)goto _LL418;else{if(((_tmp68E->f1).Wchar_c).tag != 3)goto _LL418;_tmp68F=(struct _dyneither_ptr)((_tmp68E->f1).Wchar_c).val;}}_LL417: {
# 2294
unsigned long _tmp693=Cyc_strlen((struct _dyneither_ptr)_tmp68F);
int i=0;
if(_tmp693 >= 2  && *((const char*)_check_dyneither_subscript(_tmp68F,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp68F,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmp68F,sizeof(char),1))== 'x'  && _tmp693 >= 3) && *((const char*)_check_dyneither_subscript(_tmp68F,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp693;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp68F,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 2304
return 0;}}_LL418: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp690=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp689;if(_tmp690->tag != 13)goto _LL41A;else{_tmp691=(void*)_tmp690->f1;_tmp692=_tmp690->f2;}}_LL419:
 return Cyc_Tcutil_is_zero(_tmp692) && Cyc_Tcutil_admits_zero(_tmp691);_LL41A:;_LL41B:
 return 0;_LL40D:;}
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
enum Cyc_Absyn_KindQual _tmp695;enum Cyc_Absyn_AliasQual _tmp696;struct Cyc_Absyn_Kind*_tmp694=ka;_tmp695=_tmp694->kind;_tmp696=_tmp694->aliasqual;
switch(_tmp696){case Cyc_Absyn_Aliasable: _LL41C:
# 2348
 switch(_tmp695){case Cyc_Absyn_AnyKind: _LL41E:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LL41F:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL420:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL421:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL422:
 return& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL423:
 return& Cyc_Tcutil_iko;}case Cyc_Absyn_Unique: _LL41D:
# 2357
 switch(_tmp695){case Cyc_Absyn_AnyKind: _LL426:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL427:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL428:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL429:
 return& Cyc_Tcutil_urko;default: _LL42A:
 break;}
# 2364
break;case Cyc_Absyn_Top: _LL425:
# 2366
 switch(_tmp695){case Cyc_Absyn_AnyKind: _LL42D:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL42E:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL42F:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL430:
 return& Cyc_Tcutil_trko;default: _LL431:
 break;}
# 2373
break;}{
# 2375
const char*_tmp113F;void*_tmp113E[1];struct Cyc_String_pa_PrintArg_struct _tmp113D;(_tmp113D.tag=0,((_tmp113D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmp113E[0]=& _tmp113D,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp113F="kind_to_opt: bad kind %s\n",_tag_dyneither(_tmp113F,sizeof(char),26))),_tag_dyneither(_tmp113E,sizeof(void*),1)))))));};}
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
enum Cyc_Absyn_KindQual _tmp69B;enum Cyc_Absyn_AliasQual _tmp69C;struct Cyc_Absyn_Kind*_tmp69A=ka;_tmp69B=_tmp69A->kind;_tmp69C=_tmp69A->aliasqual;
switch(_tmp69C){case Cyc_Absyn_Aliasable: _LL433:
# 2416
 switch(_tmp69B){case Cyc_Absyn_AnyKind: _LL435:
 return& ab;case Cyc_Absyn_MemKind: _LL436:
 return& mb;case Cyc_Absyn_BoxKind: _LL437:
 return& bb;case Cyc_Absyn_RgnKind: _LL438:
 return& rb;case Cyc_Absyn_EffKind: _LL439:
 return& eb;case Cyc_Absyn_IntKind: _LL43A:
 return& ib;}case Cyc_Absyn_Unique: _LL434:
# 2425
 switch(_tmp69B){case Cyc_Absyn_AnyKind: _LL43D:
 return& uab;case Cyc_Absyn_MemKind: _LL43E:
 return& umb;case Cyc_Absyn_BoxKind: _LL43F:
 return& ubb;case Cyc_Absyn_RgnKind: _LL440:
 return& urb;default: _LL441:
 break;}
# 2432
break;case Cyc_Absyn_Top: _LL43C:
# 2434
 switch(_tmp69B){case Cyc_Absyn_AnyKind: _LL444:
 return& tab;case Cyc_Absyn_MemKind: _LL445:
 return& tmb;case Cyc_Absyn_BoxKind: _LL446:
 return& tbb;case Cyc_Absyn_RgnKind: _LL447:
 return& trb;default: _LL448:
 break;}
# 2441
break;}{
# 2443
const char*_tmp1143;void*_tmp1142[1];struct Cyc_String_pa_PrintArg_struct _tmp1141;(_tmp1141.tag=0,((_tmp1141.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmp1142[0]=& _tmp1141,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1143="kind_to_b: bad kind %s\n",_tag_dyneither(_tmp1143,sizeof(char),24))),_tag_dyneither(_tmp1142,sizeof(void*),1)))))));};}
# 2446
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return*Cyc_Tcutil_kind_to_b(k);}
# 2449
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
# 2451
struct Cyc_Core_Opt*_tmp1144;return(_tmp1144=_cycalloc(sizeof(*_tmp1144)),((_tmp1144->v=Cyc_Tcutil_kind_to_bound(k),_tmp1144)));}
# 2456
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){
{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp1147;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1146;e1->r=(void*)((_tmp1146=_cycalloc(sizeof(*_tmp1146)),((_tmp1146[0]=((_tmp1147.tag=0,((_tmp1147.f1=Cyc_Absyn_Null_c,_tmp1147)))),_tmp1146))));}{
struct Cyc_Core_Opt*_tmp6B1=Cyc_Tcenv_lookup_opt_type_vars(te);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp114D;struct Cyc_Absyn_PtrInfo _tmp114C;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp114B;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6B2=(_tmp114B=_cycalloc(sizeof(*_tmp114B)),((_tmp114B[0]=((_tmp114D.tag=5,((_tmp114D.f1=((_tmp114C.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp6B1),((_tmp114C.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp114C.ptr_atts=(
((_tmp114C.ptr_atts).rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp6B1),(((_tmp114C.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp114C.ptr_atts).bounds=
# 2464
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp114C.ptr_atts).zero_term=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp114C.ptr_atts).ptrloc=0,_tmp114C.ptr_atts)))))))))),_tmp114C)))))),_tmp114D)))),_tmp114B)));
e1->topt=(void*)_tmp6B2;{
int bogus=0;
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
if(bogus != 0){
const char*_tmp1152;void*_tmp1151[2];struct Cyc_String_pa_PrintArg_struct _tmp1150;struct Cyc_String_pa_PrintArg_struct _tmp114F;(_tmp114F.tag=0,((_tmp114F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(e1->loc)),((_tmp1150.tag=0,((_tmp1150.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp1151[0]=& _tmp1150,((_tmp1151[1]=& _tmp114F,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1152="zero_to_null resulted in an alias coercion on %s at %s\n",_tag_dyneither(_tmp1152,sizeof(char),56))),_tag_dyneither(_tmp1151,sizeof(void*),2)))))))))))));}
return retv;};};}
# 2474
return 0;}
# 2477
struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
switch(c){case Cyc_Absyn_Unknown_coercion: _LL44A: {
const char*_tmp1153;return(_tmp1153="unknown",_tag_dyneither(_tmp1153,sizeof(char),8));}case Cyc_Absyn_No_coercion: _LL44B: {
const char*_tmp1154;return(_tmp1154="no coercion",_tag_dyneither(_tmp1154,sizeof(char),12));}case Cyc_Absyn_NonNull_to_Null: _LL44C: {
const char*_tmp1155;return(_tmp1155="null check",_tag_dyneither(_tmp1155,sizeof(char),11));}case Cyc_Absyn_Other_coercion: _LL44D: {
const char*_tmp1156;return(_tmp1156="other coercion",_tag_dyneither(_tmp1156,sizeof(char),15));}}}
# 2486
int Cyc_Tcutil_warn_alias_coerce=0;
# 2492
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0,& Cyc_Tcutil_rk};
# 2496
static int counter=0;
struct _dyneither_ptr*_tmp1163;const char*_tmp1162;void*_tmp1161[1];struct Cyc_Int_pa_PrintArg_struct _tmp1160;struct _tuple2*_tmp115F;struct _tuple2*v=(_tmp115F=_cycalloc(sizeof(*_tmp115F)),((_tmp115F->f1=Cyc_Absyn_Loc_n,((_tmp115F->f2=((_tmp1163=_cycalloc(sizeof(*_tmp1163)),((_tmp1163[0]=(struct _dyneither_ptr)((_tmp1160.tag=1,((_tmp1160.f1=(unsigned long)counter ++,((_tmp1161[0]=& _tmp1160,Cyc_aprintf(((_tmp1162="__aliasvar%d",_tag_dyneither(_tmp1162,sizeof(char),13))),_tag_dyneither(_tmp1161,sizeof(void*),1)))))))),_tmp1163)))),_tmp115F)))));
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(v,e_typ,e);
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp1166;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp1165;struct Cyc_Absyn_Exp*ve=Cyc_Absyn_varb_exp(v,(void*)((_tmp1165=_cycalloc(sizeof(*_tmp1165)),((_tmp1165[0]=((_tmp1166.tag=4,((_tmp1166.f1=vd,_tmp1166)))),_tmp1165)))),e->loc);
# 2505
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 2507
{void*_tmp6BE=Cyc_Tcutil_compress(e_typ);void*_tmp6BF=_tmp6BE;void*_tmp6C1;struct Cyc_Absyn_Tqual _tmp6C2;void*_tmp6C3;union Cyc_Absyn_Constraint*_tmp6C4;union Cyc_Absyn_Constraint*_tmp6C5;union Cyc_Absyn_Constraint*_tmp6C6;struct Cyc_Absyn_PtrLoc*_tmp6C7;_LL450: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6C0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6BF;if(_tmp6C0->tag != 5)goto _LL452;else{_tmp6C1=(_tmp6C0->f1).elt_typ;_tmp6C2=(_tmp6C0->f1).elt_tq;_tmp6C3=((_tmp6C0->f1).ptr_atts).rgn;_tmp6C4=((_tmp6C0->f1).ptr_atts).nullable;_tmp6C5=((_tmp6C0->f1).ptr_atts).bounds;_tmp6C6=((_tmp6C0->f1).ptr_atts).zero_term;_tmp6C7=((_tmp6C0->f1).ptr_atts).ptrloc;}}_LL451:
# 2509
{void*_tmp6C8=Cyc_Tcutil_compress(_tmp6C3);void*_tmp6C9=_tmp6C8;void**_tmp6CB;struct Cyc_Core_Opt*_tmp6CC;_LL455: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp6CA=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp6C9;if(_tmp6CA->tag != 1)goto _LL457;else{_tmp6CB=(void**)((void**)& _tmp6CA->f2);_tmp6CC=_tmp6CA->f4;}}_LL456: {
# 2511
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1169;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1168;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp6CD=(_tmp1168=_cycalloc(sizeof(*_tmp1168)),((_tmp1168[0]=((_tmp1169.tag=2,((_tmp1169.f1=tv,_tmp1169)))),_tmp1168)));
*_tmp6CB=(void*)_tmp6CD;
goto _LL454;}_LL457:;_LL458:
 goto _LL454;_LL454:;}
# 2516
goto _LL44F;_LL452:;_LL453:
 goto _LL44F;_LL44F:;}
# 2520
e->topt=0;
vd->initializer=0;{
# 2524
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 2526
struct _tuple15 _tmp116A;return(_tmp116A.f1=d,((_tmp116A.f2=ve,_tmp116A)));};}
# 2531
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ,void*wants_typ,unsigned int loc){
# 2534
struct _RegionHandle _tmp6D9=_new_region("r");struct _RegionHandle*r=& _tmp6D9;_push_region(r);
if((Cyc_Tcutil_is_noalias_path(r,e) && Cyc_Tcutil_is_noalias_pointer(e_typ,0)) && 
Cyc_Tcutil_is_pointer_type(e_typ)){
# 2539
void*_tmp6DA=Cyc_Tcutil_compress(wants_typ);void*_tmp6DB=_tmp6DA;void*_tmp6DD;_LL45A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6DC=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6DB;if(_tmp6DC->tag != 5)goto _LL45C;else{_tmp6DD=((_tmp6DC->f1).ptr_atts).rgn;}}_LL45B: {
# 2541
void*_tmp6DF=Cyc_Tcutil_compress(_tmp6DD);void*_tmp6E0=_tmp6DF;_LL461: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp6E1=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp6E0;if(_tmp6E1->tag != 20)goto _LL463;}_LL462: {
int _tmp6E2=0;_npop_handler(0);return _tmp6E2;}_LL463:;_LL464: {
# 2544
struct Cyc_Absyn_Kind*_tmp6E3=Cyc_Tcutil_typ_kind(_tmp6DD);
int _tmp6E4=_tmp6E3->kind == Cyc_Absyn_RgnKind  && _tmp6E3->aliasqual == Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp6E4;}_LL460:;}_LL45C: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp6DE=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6DB;if(_tmp6DE->tag != 17)goto _LL45E;}_LL45D: {
# 2548
const char*_tmp116F;void*_tmp116E[2];struct Cyc_String_pa_PrintArg_struct _tmp116D;struct Cyc_String_pa_PrintArg_struct _tmp116C;(_tmp116C.tag=0,((_tmp116C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(loc)),((_tmp116D.tag=0,((_tmp116D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_compress(wants_typ))),((_tmp116E[0]=& _tmp116D,((_tmp116E[1]=& _tmp116C,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp116F="got typedef %s in can_insert_alias at %s\n",_tag_dyneither(_tmp116F,sizeof(char),42))),_tag_dyneither(_tmp116E,sizeof(void*),2)))))))))))));}_LL45E:;_LL45F: {
int _tmp6E9=0;_npop_handler(0);return _tmp6E9;}_LL459:;}{
# 2553
int _tmp6EA=0;_npop_handler(0);return _tmp6EA;};
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
if(Cyc_Tcutil_will_lose_precision(t1,t2)){
const char*_tmp1174;void*_tmp1173[2];struct Cyc_String_pa_PrintArg_struct _tmp1172;struct Cyc_String_pa_PrintArg_struct _tmp1171;(_tmp1171.tag=0,((_tmp1171.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp1172.tag=0,((_tmp1172.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp1173[0]=& _tmp1172,((_tmp1173[1]=& _tmp1171,Cyc_Tcutil_warn(e->loc,((_tmp1174="integral size mismatch; %s -> %s conversion supplied",_tag_dyneither(_tmp1174,sizeof(char),53))),_tag_dyneither(_tmp1173,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
return 1;}else{
# 2575
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce){
const char*_tmp117A;void*_tmp1179[3];struct Cyc_String_pa_PrintArg_struct _tmp1178;struct Cyc_String_pa_PrintArg_struct _tmp1177;struct Cyc_String_pa_PrintArg_struct _tmp1176;(_tmp1176.tag=0,((_tmp1176.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp1177.tag=0,((_tmp1177.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp1178.tag=0,((_tmp1178.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp1179[0]=& _tmp1178,((_tmp1179[1]=& _tmp1177,((_tmp1179[2]=& _tmp1176,Cyc_Tcutil_warn(e->loc,((_tmp117A="implicit alias coercion for %s:%s to %s",_tag_dyneither(_tmp117A,sizeof(char),40))),_tag_dyneither(_tmp1179,sizeof(void*),3)))))))))))))))))));}
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
if(c != Cyc_Absyn_NonNull_to_Null){
const char*_tmp117F;void*_tmp117E[2];struct Cyc_String_pa_PrintArg_struct _tmp117D;struct Cyc_String_pa_PrintArg_struct _tmp117C;(_tmp117C.tag=0,((_tmp117C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp117D.tag=0,((_tmp117D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp117E[0]=& _tmp117D,((_tmp117E[1]=& _tmp117C,Cyc_Tcutil_warn(e->loc,((_tmp117F="implicit cast from %s to %s",_tag_dyneither(_tmp117F,sizeof(char),28))),_tag_dyneither(_tmp117E,sizeof(void*),2)))))))))))));}
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
void*_tmp6F8=Cyc_Tcutil_compress(t);void*_tmp6F9=_tmp6F8;_LL466: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6FA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6F9;if(_tmp6FA->tag != 6)goto _LL468;}_LL467:
 goto _LL469;_LL468: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp6FB=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp6F9;if(_tmp6FB->tag != 7)goto _LL46A;}_LL469:
 return 1;_LL46A:;_LL46B:
 return 0;_LL465:;}
# 2628 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 2632
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ_f(struct _tuple23*env,struct Cyc_Absyn_Aggrfield*x){
# 2635
struct _tuple23 _tmp6FC=*env;struct Cyc_List_List*_tmp6FE;struct _RegionHandle*_tmp6FF;struct Cyc_Tcenv_Tenv*_tmp700;int _tmp701;struct _tuple23 _tmp6FD=_tmp6FC;_tmp6FE=_tmp6FD.f1;_tmp6FF=_tmp6FD.f2;_tmp700=_tmp6FD.f3;_tmp701=_tmp6FD.f4;{
# 2637
void*_tmp702=_tmp6FE == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp6FF,_tmp6FE,x->type);
struct Cyc_List_List*_tmp703=Cyc_Tcutil_flatten_typ(_tmp6FF,_tmp701,_tmp700,_tmp702);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp703)== 1){
struct _tuple12*_tmp1182;struct Cyc_List_List*_tmp1181;return(_tmp1181=_region_malloc(_tmp6FF,sizeof(*_tmp1181)),((_tmp1181->hd=((_tmp1182=_region_malloc(_tmp6FF,sizeof(*_tmp1182)),((_tmp1182->f1=x->tq,((_tmp1182->f2=_tmp702,_tmp1182)))))),((_tmp1181->tl=0,_tmp1181)))));}else{
return _tmp703;}};}struct _tuple24{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2643
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple24*env,struct _tuple12*x){
# 2645
struct _tuple24 _tmp706=*env;struct _RegionHandle*_tmp708;struct Cyc_Tcenv_Tenv*_tmp709;int _tmp70A;struct _tuple24 _tmp707=_tmp706;_tmp708=_tmp707.f1;_tmp709=_tmp707.f2;_tmp70A=_tmp707.f3;{
struct _tuple12 _tmp70B=*x;struct Cyc_Absyn_Tqual _tmp70D;void*_tmp70E;struct _tuple12 _tmp70C=_tmp70B;_tmp70D=_tmp70C.f1;_tmp70E=_tmp70C.f2;{
struct Cyc_List_List*_tmp70F=Cyc_Tcutil_flatten_typ(_tmp708,_tmp70A,_tmp709,_tmp70E);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp70F)== 1){
struct _tuple12*_tmp1185;struct Cyc_List_List*_tmp1184;return(_tmp1184=_region_malloc(_tmp708,sizeof(*_tmp1184)),((_tmp1184->hd=((_tmp1185=_region_malloc(_tmp708,sizeof(*_tmp1185)),((_tmp1185->f1=_tmp70D,((_tmp1185->f2=_tmp70E,_tmp1185)))))),((_tmp1184->tl=0,_tmp1184)))));}else{
return _tmp70F;}};};}
# 2652
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2656
if(flatten){
t1=Cyc_Tcutil_compress(t1);{
void*_tmp712=t1;struct Cyc_List_List*_tmp715;struct Cyc_Absyn_Aggrdecl*_tmp717;struct Cyc_List_List*_tmp718;struct Cyc_List_List*_tmp71A;_LL46D: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp713=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp712;if(_tmp713->tag != 0)goto _LL46F;}_LL46E:
 return 0;_LL46F: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp714=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp712;if(_tmp714->tag != 10)goto _LL471;else{_tmp715=_tmp714->f1;}}_LL470: {
# 2661
struct _tuple24 _tmp1186;struct _tuple24 _tmp71B=(_tmp1186.f1=r,((_tmp1186.f2=te,((_tmp1186.f3=flatten,_tmp1186)))));
# 2663
struct Cyc_List_List*_tmp71C=_tmp715;struct _tuple12*_tmp71D;struct Cyc_List_List*_tmp71E;_LL478: if(_tmp71C != 0)goto _LL47A;_LL479:
 return 0;_LL47A: if(_tmp71C == 0)goto _LL477;_tmp71D=(struct _tuple12*)_tmp71C->hd;_tmp71E=_tmp71C->tl;_LL47B: {
# 2666
struct Cyc_List_List*_tmp71F=Cyc_Tcutil_rcopy_tqt(& _tmp71B,_tmp71D);
_tmp71B.f3=0;{
struct Cyc_List_List*_tmp720=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple12*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp71B,_tmp715);
struct Cyc_List_List*_tmp1187;struct Cyc_List_List*_tmp721=(_tmp1187=_region_malloc(r,sizeof(*_tmp1187)),((_tmp1187->hd=_tmp71F,((_tmp1187->tl=_tmp720,_tmp1187)))));
struct Cyc_List_List*_tmp1188;return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,((_tmp1188=_region_malloc(r,sizeof(*_tmp1188)),((_tmp1188->hd=_tmp71F,((_tmp1188->tl=_tmp720,_tmp1188)))))));};}_LL477:;}_LL471: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp716=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp712;if(_tmp716->tag != 11)goto _LL473;else{if((((_tmp716->f1).aggr_info).KnownAggr).tag != 2)goto _LL473;_tmp717=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp716->f1).aggr_info).KnownAggr).val);_tmp718=(_tmp716->f1).targs;}}_LL472:
# 2674
 if(((_tmp717->kind == Cyc_Absyn_UnionA  || _tmp717->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp717->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp717->impl))->rgn_po != 0){
# 2676
struct _tuple12*_tmp118B;struct Cyc_List_List*_tmp118A;return(_tmp118A=_region_malloc(r,sizeof(*_tmp118A)),((_tmp118A->hd=((_tmp118B=_region_malloc(r,sizeof(*_tmp118B)),((_tmp118B->f1=Cyc_Absyn_empty_tqual(0),((_tmp118B->f2=t1,_tmp118B)))))),((_tmp118A->tl=0,_tmp118A)))));}{
struct Cyc_List_List*_tmp727=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp717->tvs,_tmp718);
struct _tuple23 _tmp118C;struct _tuple23 env=(_tmp118C.f1=_tmp727,((_tmp118C.f2=r,((_tmp118C.f3=te,((_tmp118C.f4=flatten,_tmp118C)))))));
struct Cyc_List_List*_tmp728=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp717->impl))->fields;struct Cyc_List_List*_tmp729=_tmp728;struct Cyc_Absyn_Aggrfield*_tmp72A;struct Cyc_List_List*_tmp72B;_LL47D: if(_tmp729 != 0)goto _LL47F;_LL47E:
 return 0;_LL47F: if(_tmp729 == 0)goto _LL47C;_tmp72A=(struct Cyc_Absyn_Aggrfield*)_tmp729->hd;_tmp72B=_tmp729->tl;_LL480: {
# 2682
struct Cyc_List_List*_tmp72C=Cyc_Tcutil_flatten_typ_f(& env,_tmp72A);
env.f4=0;{
struct Cyc_List_List*_tmp72D=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp72B);
struct Cyc_List_List*_tmp118D;struct Cyc_List_List*_tmp72E=(_tmp118D=_region_malloc(r,sizeof(*_tmp118D)),((_tmp118D->hd=_tmp72C,((_tmp118D->tl=_tmp72D,_tmp118D)))));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp72E);};}_LL47C:;};_LL473: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp719=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp712;if(_tmp719->tag != 12)goto _LL475;else{if(_tmp719->f1 != Cyc_Absyn_StructA)goto _LL475;_tmp71A=_tmp719->f2;}}_LL474: {
# 2689
struct _tuple23 _tmp118E;struct _tuple23 env=(_tmp118E.f1=0,((_tmp118E.f2=r,((_tmp118E.f3=te,((_tmp118E.f4=flatten,_tmp118E)))))));
struct Cyc_List_List*_tmp731=_tmp71A;struct Cyc_Absyn_Aggrfield*_tmp732;struct Cyc_List_List*_tmp733;_LL482: if(_tmp731 != 0)goto _LL484;_LL483:
 return 0;_LL484: if(_tmp731 == 0)goto _LL481;_tmp732=(struct Cyc_Absyn_Aggrfield*)_tmp731->hd;_tmp733=_tmp731->tl;_LL485: {
# 2693
struct Cyc_List_List*_tmp734=Cyc_Tcutil_flatten_typ_f(& env,_tmp732);
env.f4=0;{
struct Cyc_List_List*_tmp735=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp733);
struct Cyc_List_List*_tmp118F;struct Cyc_List_List*_tmp736=(_tmp118F=_region_malloc(r,sizeof(*_tmp118F)),((_tmp118F->hd=_tmp734,((_tmp118F->tl=_tmp735,_tmp118F)))));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp736);};}_LL481:;}_LL475:;_LL476:
# 2699
 goto _LL46C;_LL46C:;};}{
# 2702
struct _tuple12*_tmp1192;struct Cyc_List_List*_tmp1191;return(_tmp1191=_region_malloc(r,sizeof(*_tmp1191)),((_tmp1191->hd=((_tmp1192=_region_malloc(r,sizeof(*_tmp1192)),((_tmp1192->f1=Cyc_Absyn_empty_tqual(0),((_tmp1192->f2=t1,_tmp1192)))))),((_tmp1191->tl=0,_tmp1191)))));};}
# 2706
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp73B=(void*)t->hd;void*_tmp73C=_tmp73B;_LL487: {struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*_tmp73D=(struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*)_tmp73C;if(_tmp73D->tag != 23)goto _LL489;}_LL488:
 goto _LL48A;_LL489: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp73E=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp73C;if(_tmp73E->tag != 4)goto _LL48B;}_LL48A:
 goto _LL48C;_LL48B: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp73F=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp73C;if(_tmp73F->tag != 20)goto _LL48D;}_LL48C:
# 2712
 continue;_LL48D:;_LL48E:
# 2714
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;_LL486:;}}
# 2717
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}
# 2720
return 1;}
# 2723
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp1193;struct _tuple0 _tmp740=(_tmp1193.f1=Cyc_Tcutil_compress(t1),((_tmp1193.f2=Cyc_Tcutil_compress(t2),_tmp1193)));struct _tuple0 _tmp741=_tmp740;enum Cyc_Absyn_Size_of _tmp743;enum Cyc_Absyn_Size_of _tmp745;_LL490:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp742=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp741.f1;if(_tmp742->tag != 6)goto _LL492;else{_tmp743=_tmp742->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp744=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp741.f2;if(_tmp744->tag != 6)goto _LL492;else{_tmp745=_tmp744->f2;}};_LL491:
# 2726
 return(_tmp743 == _tmp745  || _tmp743 == Cyc_Absyn_Int_sz  && _tmp745 == Cyc_Absyn_Long_sz) || 
_tmp743 == Cyc_Absyn_Long_sz  && _tmp745 == Cyc_Absyn_Int_sz;_LL492:;_LL493:
 return 0;_LL48F:;}
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
struct _tuple0 _tmp1194;struct _tuple0 _tmp747=(_tmp1194.f1=t1,((_tmp1194.f2=t2,_tmp1194)));struct _tuple0 _tmp748=_tmp747;void*_tmp74A;struct Cyc_Absyn_Tqual _tmp74B;void*_tmp74C;union Cyc_Absyn_Constraint*_tmp74D;union Cyc_Absyn_Constraint*_tmp74E;union Cyc_Absyn_Constraint*_tmp74F;void*_tmp751;struct Cyc_Absyn_Tqual _tmp752;void*_tmp753;union Cyc_Absyn_Constraint*_tmp754;union Cyc_Absyn_Constraint*_tmp755;union Cyc_Absyn_Constraint*_tmp756;struct Cyc_Absyn_Datatypedecl*_tmp758;struct Cyc_Absyn_Datatypefield*_tmp759;struct Cyc_List_List*_tmp75A;struct Cyc_Absyn_Datatypedecl*_tmp75C;struct Cyc_List_List*_tmp75D;struct Cyc_Absyn_FnInfo _tmp75F;struct Cyc_Absyn_FnInfo _tmp761;_LL495:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp749=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp748.f1;if(_tmp749->tag != 5)goto _LL497;else{_tmp74A=(_tmp749->f1).elt_typ;_tmp74B=(_tmp749->f1).elt_tq;_tmp74C=((_tmp749->f1).ptr_atts).rgn;_tmp74D=((_tmp749->f1).ptr_atts).nullable;_tmp74E=((_tmp749->f1).ptr_atts).bounds;_tmp74F=((_tmp749->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp750=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp748.f2;if(_tmp750->tag != 5)goto _LL497;else{_tmp751=(_tmp750->f1).elt_typ;_tmp752=(_tmp750->f1).elt_tq;_tmp753=((_tmp750->f1).ptr_atts).rgn;_tmp754=((_tmp750->f1).ptr_atts).nullable;_tmp755=((_tmp750->f1).ptr_atts).bounds;_tmp756=((_tmp750->f1).ptr_atts).zero_term;}};_LL496:
# 2750
 if(_tmp74B.real_const  && !_tmp752.real_const)
return 0;
# 2753
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp74D,_tmp754) && 
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp74D)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp754))
return 0;
# 2757
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp74F,_tmp756) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp74F)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp756))
return 0;
# 2761
if((!Cyc_Tcutil_unify(_tmp74C,_tmp753) && !Cyc_Tcenv_region_outlives(te,_tmp74C,_tmp753)) && !
Cyc_Tcutil_subtype(te,assume,_tmp74C,_tmp753))
return 0;
# 2765
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp74E,_tmp755)){
struct _tuple0 _tmp1195;struct _tuple0 _tmp762=(_tmp1195.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp74E),((_tmp1195.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp755),_tmp1195)));struct _tuple0 _tmp763=_tmp762;struct Cyc_Absyn_Exp*_tmp767;struct Cyc_Absyn_Exp*_tmp769;_LL49E:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp764=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp763.f1;if(_tmp764->tag != 0)goto _LL4A0;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp765=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp763.f2;if(_tmp765->tag != 0)goto _LL4A0;};_LL49F:
 goto _LL49D;_LL4A0:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp766=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp763.f1;if(_tmp766->tag != 1)goto _LL4A2;else{_tmp767=_tmp766->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp768=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp763.f2;if(_tmp768->tag != 1)goto _LL4A2;else{_tmp769=_tmp768->f1;}};_LL4A1:
# 2770
 if(!Cyc_Evexp_lte_const_exp(_tmp769,_tmp767))
return 0;
goto _LL49D;_LL4A2:;_LL4A3:
 return 0;_LL49D:;}
# 2778
if(!_tmp752.real_const  && _tmp74B.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp751)))
return 0;}{
# 2784
int _tmp76B=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp755,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp756);
# 2790
struct _tuple0*_tmp1198;struct Cyc_List_List*_tmp1197;return(_tmp76B  && Cyc_Tcutil_ptrsubtype(te,((_tmp1197=_cycalloc(sizeof(*_tmp1197)),((_tmp1197->hd=((_tmp1198=_cycalloc(sizeof(*_tmp1198)),((_tmp1198->f1=t1,((_tmp1198->f2=t2,_tmp1198)))))),((_tmp1197->tl=assume,_tmp1197)))))),_tmp74A,_tmp751) || Cyc_Tcutil_unify(_tmp74A,_tmp751)) || Cyc_Tcutil_isomorphic(_tmp74A,_tmp751);};_LL497:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp757=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp748.f1;if(_tmp757->tag != 4)goto _LL499;else{if((((_tmp757->f1).field_info).KnownDatatypefield).tag != 2)goto _LL499;_tmp758=((struct _tuple3)(((_tmp757->f1).field_info).KnownDatatypefield).val).f1;_tmp759=((struct _tuple3)(((_tmp757->f1).field_info).KnownDatatypefield).val).f2;_tmp75A=(_tmp757->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp75B=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp748.f2;if(_tmp75B->tag != 3)goto _LL499;else{if((((_tmp75B->f1).datatype_info).KnownDatatype).tag != 2)goto _LL499;_tmp75C=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp75B->f1).datatype_info).KnownDatatype).val);_tmp75D=(_tmp75B->f1).targs;}};_LL498:
# 2796
 if(_tmp758 != _tmp75C  && Cyc_Absyn_qvar_cmp(_tmp758->name,_tmp75C->name)!= 0)return 0;
# 2798
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp75A)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp75D))return 0;
for(0;_tmp75A != 0;(_tmp75A=_tmp75A->tl,_tmp75D=_tmp75D->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp75A->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp75D))->hd))return 0;}
return 1;_LL499:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp75E=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp748.f1;if(_tmp75E->tag != 9)goto _LL49B;else{_tmp75F=_tmp75E->f1;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp760=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp748.f2;if(_tmp760->tag != 9)goto _LL49B;else{_tmp761=_tmp760->f1;}};_LL49A:
# 2805
 if(_tmp75F.tvars != 0  || _tmp761.tvars != 0){
struct Cyc_List_List*_tmp76E=_tmp75F.tvars;
struct Cyc_List_List*_tmp76F=_tmp761.tvars;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp76E)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp76F))return 0;{
struct _RegionHandle _tmp770=_new_region("r");struct _RegionHandle*r=& _tmp770;_push_region(r);
{struct Cyc_List_List*inst=0;
while(_tmp76E != 0){
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp76E->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp76F))->hd)->kind)){int _tmp771=0;_npop_handler(0);return _tmp771;}
{struct _tuple16*_tmp11A2;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp11A1;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp11A0;struct Cyc_List_List*_tmp119F;inst=((_tmp119F=_region_malloc(r,sizeof(*_tmp119F)),((_tmp119F->hd=((_tmp11A2=_region_malloc(r,sizeof(*_tmp11A2)),((_tmp11A2->f1=(struct Cyc_Absyn_Tvar*)_tmp76F->hd,((_tmp11A2->f2=(void*)((_tmp11A0=_cycalloc(sizeof(*_tmp11A0)),((_tmp11A0[0]=((_tmp11A1.tag=2,((_tmp11A1.f1=(struct Cyc_Absyn_Tvar*)_tmp76E->hd,_tmp11A1)))),_tmp11A0)))),_tmp11A2)))))),((_tmp119F->tl=inst,_tmp119F))))));}
_tmp76E=_tmp76E->tl;
_tmp76F=_tmp76F->tl;}
# 2817
if(inst != 0){
_tmp75F.tvars=0;
_tmp761.tvars=0;{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp11A8;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp11A7;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp11A5;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp11A4;int _tmp77A=Cyc_Tcutil_subtype(te,assume,(void*)((_tmp11A4=_cycalloc(sizeof(*_tmp11A4)),((_tmp11A4[0]=((_tmp11A5.tag=9,((_tmp11A5.f1=_tmp75F,_tmp11A5)))),_tmp11A4)))),(void*)((_tmp11A7=_cycalloc(sizeof(*_tmp11A7)),((_tmp11A7[0]=((_tmp11A8.tag=9,((_tmp11A8.f1=_tmp761,_tmp11A8)))),_tmp11A7)))));_npop_handler(0);return _tmp77A;};}}
# 2810
;_pop_region(r);};}
# 2825
if(!Cyc_Tcutil_subtype(te,assume,_tmp75F.ret_typ,_tmp761.ret_typ))return 0;{
struct Cyc_List_List*_tmp77B=_tmp75F.args;
struct Cyc_List_List*_tmp77C=_tmp761.args;
# 2830
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp77B)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp77C))return 0;
# 2832
for(0;_tmp77B != 0;(_tmp77B=_tmp77B->tl,_tmp77C=_tmp77C->tl)){
struct _tuple10 _tmp77D=*((struct _tuple10*)_tmp77B->hd);struct Cyc_Absyn_Tqual _tmp77F;void*_tmp780;struct _tuple10 _tmp77E=_tmp77D;_tmp77F=_tmp77E.f2;_tmp780=_tmp77E.f3;{
struct _tuple10 _tmp781=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp77C))->hd);struct Cyc_Absyn_Tqual _tmp783;void*_tmp784;struct _tuple10 _tmp782=_tmp781;_tmp783=_tmp782.f2;_tmp784=_tmp782.f3;
# 2836
if(_tmp783.real_const  && !_tmp77F.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp784,_tmp780))
return 0;};}
# 2840
if(_tmp75F.c_varargs != _tmp761.c_varargs)return 0;
if(_tmp75F.cyc_varargs != 0  && _tmp761.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp785=*_tmp75F.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp786=*_tmp761.cyc_varargs;
# 2845
if((_tmp786.tq).real_const  && !(_tmp785.tq).real_const  || !
Cyc_Tcutil_subtype(te,assume,_tmp786.type,_tmp785.type))
return 0;}else{
if(_tmp75F.cyc_varargs != 0  || _tmp761.cyc_varargs != 0)return 0;}
# 2850
if(!Cyc_Tcutil_subset_effect(1,(void*)_check_null(_tmp75F.effect),(void*)_check_null(_tmp761.effect)))return 0;
# 2852
if(!Cyc_Tcutil_sub_rgnpo(_tmp75F.rgn_po,_tmp761.rgn_po))return 0;
# 2854
if(!Cyc_Tcutil_sub_attributes(_tmp75F.attributes,_tmp761.attributes))return 0;
# 2856
if(!Cyc_Tcutil_check_logical_implication(_tmp761.requires_relns,_tmp75F.requires_relns))
return 0;
# 2859
if(!Cyc_Tcutil_check_logical_implication(_tmp75F.ensures_relns,_tmp761.ensures_relns))
return 0;
# 2862
return 1;};_LL49B:;_LL49C:
 return 0;_LL494:;};}
# 2874 "tcutil.cyc"
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2876
struct _RegionHandle _tmp788=_new_region("temp");struct _RegionHandle*temp=& _tmp788;_push_region(temp);
{struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(temp,1,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(temp,1,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0){int _tmp789=0;_npop_handler(0);return _tmp789;}{
struct _tuple12*_tmp78A=(struct _tuple12*)tqs1->hd;struct Cyc_Absyn_Tqual _tmp78C;void*_tmp78D;struct _tuple12*_tmp78B=_tmp78A;_tmp78C=_tmp78B->f1;_tmp78D=_tmp78B->f2;{
struct _tuple12*_tmp78E=(struct _tuple12*)tqs2->hd;struct Cyc_Absyn_Tqual _tmp790;void*_tmp791;struct _tuple12*_tmp78F=_tmp78E;_tmp790=_tmp78F->f1;_tmp791=_tmp78F->f2;
# 2884
if(_tmp78C.real_const  && !_tmp790.real_const){int _tmp792=0;_npop_handler(0);return _tmp792;}
# 2886
if((_tmp790.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp791))) && 
Cyc_Tcutil_subtype(te,assume,_tmp78D,_tmp791))
# 2889
continue;
# 2891
if(Cyc_Tcutil_unify(_tmp78D,_tmp791))
# 2893
continue;
# 2895
if(Cyc_Tcutil_isomorphic(_tmp78D,_tmp791))
# 2897
continue;{
# 2900
int _tmp793=0;_npop_handler(0);return _tmp793;};};};}{
# 2902
int _tmp794=1;_npop_handler(0);return _tmp794;};}
# 2877
;_pop_region(temp);}
# 2906
static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp795=Cyc_Tcutil_compress(t);void*_tmp796=_tmp795;_LL4A5: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp797=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp796;if(_tmp797->tag != 6)goto _LL4A7;else{if(_tmp797->f2 != Cyc_Absyn_Char_sz)goto _LL4A7;}}_LL4A6:
 return 1;_LL4A7:;_LL4A8:
 return 0;_LL4A4:;}
# 2915
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
# 2921
if(t2 == (void*)& Cyc_Absyn_VoidType_val)
return Cyc_Absyn_No_coercion;
{void*_tmp798=t2;_LL4AA: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp799=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp798;if(_tmp799->tag != 6)goto _LL4AC;else{if(_tmp799->f2 != Cyc_Absyn_Int_sz)goto _LL4AC;}}_LL4AB:
# 2925
 goto _LL4AD;_LL4AC: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp79A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp798;if(_tmp79A->tag != 6)goto _LL4AE;else{if(_tmp79A->f2 != Cyc_Absyn_Long_sz)goto _LL4AE;}}_LL4AD:
# 2927
 if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL4A9;_LL4AE:;_LL4AF:
 goto _LL4A9;_LL4A9:;}{
# 2931
void*_tmp79B=t1;void*_tmp79D;struct Cyc_Absyn_Tqual _tmp79E;void*_tmp79F;union Cyc_Absyn_Constraint*_tmp7A0;union Cyc_Absyn_Constraint*_tmp7A1;union Cyc_Absyn_Constraint*_tmp7A2;void*_tmp7A4;struct Cyc_Absyn_Tqual _tmp7A5;struct Cyc_Absyn_Exp*_tmp7A6;union Cyc_Absyn_Constraint*_tmp7A7;struct Cyc_Absyn_Enumdecl*_tmp7A9;void*_tmp7AD;_LL4B1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp79C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp79B;if(_tmp79C->tag != 5)goto _LL4B3;else{_tmp79D=(_tmp79C->f1).elt_typ;_tmp79E=(_tmp79C->f1).elt_tq;_tmp79F=((_tmp79C->f1).ptr_atts).rgn;_tmp7A0=((_tmp79C->f1).ptr_atts).nullable;_tmp7A1=((_tmp79C->f1).ptr_atts).bounds;_tmp7A2=((_tmp79C->f1).ptr_atts).zero_term;}}_LL4B2:
# 2939
{void*_tmp7AE=t2;void*_tmp7B0;struct Cyc_Absyn_Tqual _tmp7B1;void*_tmp7B2;union Cyc_Absyn_Constraint*_tmp7B3;union Cyc_Absyn_Constraint*_tmp7B4;union Cyc_Absyn_Constraint*_tmp7B5;_LL4C0: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp7AF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7AE;if(_tmp7AF->tag != 5)goto _LL4C2;else{_tmp7B0=(_tmp7AF->f1).elt_typ;_tmp7B1=(_tmp7AF->f1).elt_tq;_tmp7B2=((_tmp7AF->f1).ptr_atts).rgn;_tmp7B3=((_tmp7AF->f1).ptr_atts).nullable;_tmp7B4=((_tmp7AF->f1).ptr_atts).bounds;_tmp7B5=((_tmp7AF->f1).ptr_atts).zero_term;}}_LL4C1: {
# 2943
enum Cyc_Absyn_Coercion coercion=Cyc_Absyn_Other_coercion;
struct _tuple0*_tmp11AB;struct Cyc_List_List*_tmp11AA;struct Cyc_List_List*_tmp7B6=(_tmp11AA=_cycalloc(sizeof(*_tmp11AA)),((_tmp11AA->hd=((_tmp11AB=_cycalloc(sizeof(*_tmp11AB)),((_tmp11AB->f1=t1,((_tmp11AB->f2=t2,_tmp11AB)))))),((_tmp11AA->tl=0,_tmp11AA)))));
int _tmp7B7=_tmp7B1.real_const  || !_tmp79E.real_const;
# 2957 "tcutil.cyc"
int _tmp7B8=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp7B4,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp7B5);
# 2961
int _tmp7B9=_tmp7B7  && (
((_tmp7B8  && Cyc_Tcutil_ptrsubtype(te,_tmp7B6,_tmp79D,_tmp7B0) || 
Cyc_Tcutil_unify(_tmp79D,_tmp7B0)) || Cyc_Tcutil_isomorphic(_tmp79D,_tmp7B0)) || Cyc_Tcutil_unify(_tmp7B0,(void*)& Cyc_Absyn_VoidType_val));
# 2965
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp7A2,_tmp7B5) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp7B5);
# 2969
int _tmp7BA=_tmp7B9?0:((Cyc_Tcutil_bits_only(_tmp79D) && Cyc_Tcutil_is_char_type(_tmp7B0)) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp7B5)) && (
_tmp7B1.real_const  || !_tmp79E.real_const);
int bounds_ok=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp7A1,_tmp7B4);
if(!bounds_ok  && !_tmp7BA){
struct _tuple0 _tmp11AC;struct _tuple0 _tmp7BB=(_tmp11AC.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp7A1),((_tmp11AC.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp7B4),_tmp11AC)));struct _tuple0 _tmp7BC=_tmp7BB;struct Cyc_Absyn_Exp*_tmp7BE;struct Cyc_Absyn_Exp*_tmp7C0;_LL4C5:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp7BD=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp7BC.f1;if(_tmp7BD->tag != 1)goto _LL4C7;else{_tmp7BE=_tmp7BD->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp7BF=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp7BC.f2;if(_tmp7BF->tag != 1)goto _LL4C7;else{_tmp7C0=_tmp7BF->f1;}};_LL4C6:
# 2979
 if(Cyc_Evexp_lte_const_exp(_tmp7C0,_tmp7BE))
bounds_ok=1;
goto _LL4C4;_LL4C7:;_LL4C8:
# 2984
 bounds_ok=1;goto _LL4C4;_LL4C4:;}
# 2986
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp7A0) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp7B3))
coercion=Cyc_Absyn_NonNull_to_Null;
# 2991
if(((bounds_ok  && zeroterm_ok) && (_tmp7B9  || _tmp7BA)) && (
Cyc_Tcutil_unify(_tmp79F,_tmp7B2) || Cyc_Tcenv_region_outlives(te,_tmp79F,_tmp7B2)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};}_LL4C2:;_LL4C3:
 goto _LL4BF;_LL4BF:;}
# 2997
return Cyc_Absyn_Unknown_coercion;_LL4B3: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp7A3=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp79B;if(_tmp7A3->tag != 8)goto _LL4B5;else{_tmp7A4=(_tmp7A3->f1).elt_type;_tmp7A5=(_tmp7A3->f1).tq;_tmp7A6=(_tmp7A3->f1).num_elts;_tmp7A7=(_tmp7A3->f1).zero_term;}}_LL4B4:
# 2999
{void*_tmp7C4=t2;void*_tmp7C6;struct Cyc_Absyn_Tqual _tmp7C7;struct Cyc_Absyn_Exp*_tmp7C8;union Cyc_Absyn_Constraint*_tmp7C9;_LL4CA: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp7C5=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp7C4;if(_tmp7C5->tag != 8)goto _LL4CC;else{_tmp7C6=(_tmp7C5->f1).elt_type;_tmp7C7=(_tmp7C5->f1).tq;_tmp7C8=(_tmp7C5->f1).num_elts;_tmp7C9=(_tmp7C5->f1).zero_term;}}_LL4CB: {
# 3001
int okay;
okay=
(((_tmp7A6 != 0  && _tmp7C8 != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp7A7,_tmp7C9)) && 
Cyc_Evexp_lte_const_exp(_tmp7C8,_tmp7A6)) && 
Cyc_Evexp_lte_const_exp(_tmp7A6,_tmp7C8);
return
# 3008
(okay  && Cyc_Tcutil_unify(_tmp7A4,_tmp7C6)) && (!_tmp7A5.real_const  || _tmp7C7.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}_LL4CC:;_LL4CD:
# 3010
 return Cyc_Absyn_Unknown_coercion;_LL4C9:;}
# 3012
return Cyc_Absyn_Unknown_coercion;_LL4B5: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp7A8=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp79B;if(_tmp7A8->tag != 13)goto _LL4B7;else{_tmp7A9=_tmp7A8->f2;}}_LL4B6:
# 3016
{void*_tmp7CA=t2;struct Cyc_Absyn_Enumdecl*_tmp7CC;_LL4CF: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp7CB=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp7CA;if(_tmp7CB->tag != 13)goto _LL4D1;else{_tmp7CC=_tmp7CB->f2;}}_LL4D0:
# 3018
 if((_tmp7A9->fields != 0  && _tmp7CC->fields != 0) && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp7A9->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp7CC->fields))->v))
return Cyc_Absyn_Other_coercion;
goto _LL4CE;_LL4D1:;_LL4D2:
 goto _LL4CE;_LL4CE:;}
# 3024
goto _LL4B8;_LL4B7: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7AA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp79B;if(_tmp7AA->tag != 6)goto _LL4B9;}_LL4B8:
 goto _LL4BA;_LL4B9: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7AB=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp79B;if(_tmp7AB->tag != 7)goto _LL4BB;}_LL4BA:
 return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;_LL4BB: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp7AC=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp79B;if(_tmp7AC->tag != 15)goto _LL4BD;else{_tmp7AD=(void*)_tmp7AC->f1;}}_LL4BC:
# 3029
{void*_tmp7CD=t2;void*_tmp7CF;_LL4D4: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp7CE=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp7CD;if(_tmp7CE->tag != 15)goto _LL4D6;else{_tmp7CF=(void*)_tmp7CE->f1;}}_LL4D5:
# 3031
 if(Cyc_Tcenv_region_outlives(te,_tmp7AD,_tmp7CF))return Cyc_Absyn_No_coercion;
goto _LL4D3;_LL4D6:;_LL4D7:
 goto _LL4D3;_LL4D3:;}
# 3035
return Cyc_Absyn_Unknown_coercion;_LL4BD:;_LL4BE:
 return Cyc_Absyn_Unknown_coercion;_LL4B0:;};}
# 3041
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),t)){
struct Cyc_Absyn_Exp*_tmp7D0=Cyc_Absyn_copy_exp(e);
{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp11AF;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp11AE;e->r=(void*)((_tmp11AE=_cycalloc(sizeof(*_tmp11AE)),((_tmp11AE[0]=((_tmp11AF.tag=13,((_tmp11AF.f1=t,((_tmp11AF.f2=_tmp7D0,((_tmp11AF.f3=0,((_tmp11AF.f4=c,_tmp11AF)))))))))),_tmp11AE))));}
e->topt=t;}}
# 3049
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp7D3=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp7D4=_tmp7D3;_LL4D9: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7D5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7D4;if(_tmp7D5->tag != 6)goto _LL4DB;}_LL4DA:
 goto _LL4DC;_LL4DB: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp7D6=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp7D4;if(_tmp7D6->tag != 13)goto _LL4DD;}_LL4DC:
 goto _LL4DE;_LL4DD: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp7D7=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp7D4;if(_tmp7D7->tag != 14)goto _LL4DF;}_LL4DE:
 goto _LL4E0;_LL4DF: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp7D8=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp7D4;if(_tmp7D8->tag != 19)goto _LL4E1;}_LL4E0:
 return 1;_LL4E1: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp7D9=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp7D4;if(_tmp7D9->tag != 1)goto _LL4E3;}_LL4E2:
 return Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ);_LL4E3:;_LL4E4:
 return 0;_LL4D8:;}
# 3060
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp7DA=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp7DB=_tmp7DA;_LL4E6: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7DC=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp7DB;if(_tmp7DC->tag != 7)goto _LL4E8;}_LL4E7:
 return 1;_LL4E8:;_LL4E9:
 return 0;_LL4E5:;};}
# 3069
int Cyc_Tcutil_is_function_type(void*t){
void*_tmp7DD=Cyc_Tcutil_compress(t);void*_tmp7DE=_tmp7DD;_LL4EB: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp7DF=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp7DE;if(_tmp7DF->tag != 9)goto _LL4ED;}_LL4EC:
 return 1;_LL4ED:;_LL4EE:
 return 0;_LL4EA:;}
# 3076
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
struct _tuple0 _tmp11B0;struct _tuple0 _tmp7E0=(_tmp11B0.f1=t1,((_tmp11B0.f2=t2,_tmp11B0)));struct _tuple0 _tmp7E1=_tmp7E0;int _tmp7E3;int _tmp7E5;_LL4F0:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7E2=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp7E1.f1;if(_tmp7E2->tag != 7)goto _LL4F2;else{_tmp7E3=_tmp7E2->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7E4=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp7E1.f2;if(_tmp7E4->tag != 7)goto _LL4F2;else{_tmp7E5=_tmp7E4->f1;}};_LL4F1:
# 3079
 if(_tmp7E3 != 0  && _tmp7E3 != 1)return t1;else{
if(_tmp7E5 != 0  && _tmp7E5 != 1)return t2;else{
if(_tmp7E3 >= _tmp7E5)return t1;else{
return t2;}}}_LL4F2: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7E6=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp7E1.f1;if(_tmp7E6->tag != 7)goto _LL4F4;}_LL4F3:
 return t1;_LL4F4: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7E7=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp7E1.f2;if(_tmp7E7->tag != 7)goto _LL4F6;}_LL4F5:
 return t2;_LL4F6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7E8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7E1.f1;if(_tmp7E8->tag != 6)goto _LL4F8;else{if(_tmp7E8->f1 != Cyc_Absyn_Unsigned)goto _LL4F8;if(_tmp7E8->f2 != Cyc_Absyn_LongLong_sz)goto _LL4F8;}}_LL4F7:
 goto _LL4F9;_LL4F8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7E9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7E1.f2;if(_tmp7E9->tag != 6)goto _LL4FA;else{if(_tmp7E9->f1 != Cyc_Absyn_Unsigned)goto _LL4FA;if(_tmp7E9->f2 != Cyc_Absyn_LongLong_sz)goto _LL4FA;}}_LL4F9:
 return Cyc_Absyn_ulonglong_typ;_LL4FA: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7EA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7E1.f1;if(_tmp7EA->tag != 6)goto _LL4FC;else{if(_tmp7EA->f2 != Cyc_Absyn_LongLong_sz)goto _LL4FC;}}_LL4FB:
 goto _LL4FD;_LL4FC: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7EB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7E1.f2;if(_tmp7EB->tag != 6)goto _LL4FE;else{if(_tmp7EB->f2 != Cyc_Absyn_LongLong_sz)goto _LL4FE;}}_LL4FD:
 return Cyc_Absyn_slonglong_typ;_LL4FE: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7EC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7E1.f1;if(_tmp7EC->tag != 6)goto _LL500;else{if(_tmp7EC->f1 != Cyc_Absyn_Unsigned)goto _LL500;if(_tmp7EC->f2 != Cyc_Absyn_Long_sz)goto _LL500;}}_LL4FF:
 goto _LL501;_LL500: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7ED=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7E1.f2;if(_tmp7ED->tag != 6)goto _LL502;else{if(_tmp7ED->f1 != Cyc_Absyn_Unsigned)goto _LL502;if(_tmp7ED->f2 != Cyc_Absyn_Long_sz)goto _LL502;}}_LL501:
 return Cyc_Absyn_ulong_typ;_LL502: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7EE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7E1.f1;if(_tmp7EE->tag != 6)goto _LL504;else{if(_tmp7EE->f1 != Cyc_Absyn_Unsigned)goto _LL504;if(_tmp7EE->f2 != Cyc_Absyn_Int_sz)goto _LL504;}}_LL503:
# 3092
 goto _LL505;_LL504: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7EF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7E1.f2;if(_tmp7EF->tag != 6)goto _LL506;else{if(_tmp7EF->f1 != Cyc_Absyn_Unsigned)goto _LL506;if(_tmp7EF->f2 != Cyc_Absyn_Int_sz)goto _LL506;}}_LL505:
 return Cyc_Absyn_uint_typ;_LL506: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7F0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7E1.f1;if(_tmp7F0->tag != 6)goto _LL508;else{if(_tmp7F0->f2 != Cyc_Absyn_Long_sz)goto _LL508;}}_LL507:
 goto _LL509;_LL508: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7F1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7E1.f2;if(_tmp7F1->tag != 6)goto _LL50A;else{if(_tmp7F1->f2 != Cyc_Absyn_Long_sz)goto _LL50A;}}_LL509:
 return Cyc_Absyn_slong_typ;_LL50A:;_LL50B:
 return Cyc_Absyn_sint_typ;_LL4EF:;}
# 3101
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_tmp7F3=e->r;void*_tmp7F4=_tmp7F3;_LL50D: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp7F5=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp7F4;if(_tmp7F5->tag != 3)goto _LL50F;else{if(_tmp7F5->f2 != 0)goto _LL50F;}}_LL50E:
{const char*_tmp11B3;void*_tmp11B2;(_tmp11B2=0,Cyc_Tcutil_warn(e->loc,((_tmp11B3="assignment in test",_tag_dyneither(_tmp11B3,sizeof(char),19))),_tag_dyneither(_tmp11B2,sizeof(void*),0)));}goto _LL50C;_LL50F:;_LL510:
 goto _LL50C;_LL50C:;}
# 3115 "tcutil.cyc"
static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){
c1=Cyc_Absyn_compress_kb(c1);
c2=Cyc_Absyn_compress_kb(c2);
if(c1 == c2)return 1;{
struct _tuple0 _tmp11B4;struct _tuple0 _tmp7F8=(_tmp11B4.f1=c1,((_tmp11B4.f2=c2,_tmp11B4)));struct _tuple0 _tmp7F9=_tmp7F8;struct Cyc_Absyn_Kind*_tmp7FB;struct Cyc_Absyn_Kind*_tmp7FD;struct Cyc_Core_Opt**_tmp7FF;struct Cyc_Core_Opt**_tmp801;struct Cyc_Core_Opt**_tmp803;struct Cyc_Absyn_Kind*_tmp804;struct Cyc_Absyn_Kind*_tmp806;struct Cyc_Absyn_Kind*_tmp808;struct Cyc_Core_Opt**_tmp80A;struct Cyc_Absyn_Kind*_tmp80B;struct Cyc_Core_Opt**_tmp80D;struct Cyc_Absyn_Kind*_tmp80E;struct Cyc_Core_Opt**_tmp810;struct Cyc_Absyn_Kind*_tmp811;_LL512:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp7FA=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7F9.f1;if(_tmp7FA->tag != 0)goto _LL514;else{_tmp7FB=_tmp7FA->f1;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp7FC=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7F9.f2;if(_tmp7FC->tag != 0)goto _LL514;else{_tmp7FD=_tmp7FC->f1;}};_LL513:
 return _tmp7FB == _tmp7FD;_LL514: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp7FE=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7F9.f2;if(_tmp7FE->tag != 1)goto _LL516;else{_tmp7FF=(struct Cyc_Core_Opt**)& _tmp7FE->f1;}}_LL515:
{struct Cyc_Core_Opt*_tmp11B5;*_tmp7FF=((_tmp11B5=_cycalloc(sizeof(*_tmp11B5)),((_tmp11B5->v=c1,_tmp11B5))));}return 1;_LL516: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp800=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7F9.f1;if(_tmp800->tag != 1)goto _LL518;else{_tmp801=(struct Cyc_Core_Opt**)& _tmp800->f1;}}_LL517:
{struct Cyc_Core_Opt*_tmp11B6;*_tmp801=((_tmp11B6=_cycalloc(sizeof(*_tmp11B6)),((_tmp11B6->v=c2,_tmp11B6))));}return 1;_LL518:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp802=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7F9.f1;if(_tmp802->tag != 2)goto _LL51A;else{_tmp803=(struct Cyc_Core_Opt**)& _tmp802->f1;_tmp804=_tmp802->f2;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp805=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7F9.f2;if(_tmp805->tag != 0)goto _LL51A;else{_tmp806=_tmp805->f1;}};_LL519:
# 3124
 if(Cyc_Tcutil_kind_leq(_tmp806,_tmp804)){
{struct Cyc_Core_Opt*_tmp11B7;*_tmp803=((_tmp11B7=_cycalloc(sizeof(*_tmp11B7)),((_tmp11B7->v=c2,_tmp11B7))));}return 1;}else{
return 0;}_LL51A:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp807=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7F9.f1;if(_tmp807->tag != 0)goto _LL51C;else{_tmp808=_tmp807->f1;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp809=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7F9.f2;if(_tmp809->tag != 2)goto _LL51C;else{_tmp80A=(struct Cyc_Core_Opt**)& _tmp809->f1;_tmp80B=_tmp809->f2;}};_LL51B:
# 3128
 if(Cyc_Tcutil_kind_leq(_tmp808,_tmp80B)){
{struct Cyc_Core_Opt*_tmp11B8;*_tmp80A=((_tmp11B8=_cycalloc(sizeof(*_tmp11B8)),((_tmp11B8->v=c1,_tmp11B8))));}return 1;}else{
return 0;}_LL51C:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp80C=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7F9.f1;if(_tmp80C->tag != 2)goto _LL511;else{_tmp80D=(struct Cyc_Core_Opt**)& _tmp80C->f1;_tmp80E=_tmp80C->f2;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp80F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7F9.f2;if(_tmp80F->tag != 2)goto _LL511;else{_tmp810=(struct Cyc_Core_Opt**)& _tmp80F->f1;_tmp811=_tmp80F->f2;}};_LL51D:
# 3132
 if(Cyc_Tcutil_kind_leq(_tmp80E,_tmp811)){
{struct Cyc_Core_Opt*_tmp11B9;*_tmp810=((_tmp11B9=_cycalloc(sizeof(*_tmp11B9)),((_tmp11B9->v=c1,_tmp11B9))));}return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp811,_tmp80E)){
{struct Cyc_Core_Opt*_tmp11BA;*_tmp80D=((_tmp11BA=_cycalloc(sizeof(*_tmp11BA)),((_tmp11BA->v=c2,_tmp11BA))));}return 1;}else{
return 0;}}_LL511:;};}
# 3141
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 3146
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
const char*_tmp11BE;void*_tmp11BD[1];struct Cyc_Int_pa_PrintArg_struct _tmp11BC;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmp11BC.tag=1,((_tmp11BC.f1=(unsigned long)i,((_tmp11BD[0]=& _tmp11BC,Cyc_aprintf(((_tmp11BE="#%d",_tag_dyneither(_tmp11BE,sizeof(char),4))),_tag_dyneither(_tmp11BD,sizeof(void*),1))))))));
struct _dyneither_ptr*_tmp11C1;struct Cyc_Absyn_Tvar*_tmp11C0;return(_tmp11C0=_cycalloc(sizeof(*_tmp11C0)),((_tmp11C0->name=((_tmp11C1=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp11C1[0]=(struct _dyneither_ptr)s,_tmp11C1)))),((_tmp11C0->identity=- 1,((_tmp11C0->kind=k,_tmp11C0)))))));}
# 3153
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _dyneither_ptr _tmp81E=*t->name;
return*((const char*)_check_dyneither_subscript(_tmp81E,sizeof(char),0))== '#';}
# 3158
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
{const char*_tmp11C5;void*_tmp11C4[1];struct Cyc_String_pa_PrintArg_struct _tmp11C3;(_tmp11C3.tag=0,((_tmp11C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name),((_tmp11C4[0]=& _tmp11C3,Cyc_printf(((_tmp11C5="%s",_tag_dyneither(_tmp11C5,sizeof(char),3))),_tag_dyneither(_tmp11C4,sizeof(void*),1)))))));}
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
const char*_tmp11C6;struct _dyneither_ptr _tmp822=Cyc_strconcat(((_tmp11C6="`",_tag_dyneither(_tmp11C6,sizeof(char),2))),(struct _dyneither_ptr)*t->name);
{char _tmp11C9;char _tmp11C8;struct _dyneither_ptr _tmp11C7;(_tmp11C7=_dyneither_ptr_plus(_tmp822,sizeof(char),1),((_tmp11C8=*((char*)_check_dyneither_subscript(_tmp11C7,sizeof(char),0)),((_tmp11C9='t',((_get_dyneither_size(_tmp11C7,sizeof(char))== 1  && (_tmp11C8 == '\000'  && _tmp11C9 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp11C7.curr)=_tmp11C9)))))));}{
struct _dyneither_ptr*_tmp11CA;t->name=((_tmp11CA=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp11CA[0]=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp822),_tmp11CA))));};};}
# 3167
static struct _tuple10*Cyc_Tcutil_fndecl2typ_f(struct _tuple10*x){
# 3169
struct _tuple10*_tmp11CB;return(_tmp11CB=_cycalloc(sizeof(*_tmp11CB)),((_tmp11CB->f1=(*x).f1,((_tmp11CB->f2=(*x).f2,((_tmp11CB->f3=(*x).f3,_tmp11CB)))))));}
# 3172
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_typ == 0){
# 3179
struct Cyc_List_List*_tmp829=0;
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
struct Cyc_List_List*_tmp11CC;_tmp829=((_tmp11CC=_cycalloc(sizeof(*_tmp11CC)),((_tmp11CC->hd=(void*)atts->hd,((_tmp11CC->tl=_tmp829,_tmp11CC))))));}}}{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp11D2;struct Cyc_Absyn_FnInfo _tmp11D1;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp11D0;return(void*)((_tmp11D0=_cycalloc(sizeof(*_tmp11D0)),((_tmp11D0[0]=((_tmp11D2.tag=9,((_tmp11D2.f1=((_tmp11D1.tvars=fd->tvs,((_tmp11D1.effect=fd->effect,((_tmp11D1.ret_tqual=fd->ret_tqual,((_tmp11D1.ret_typ=fd->ret_type,((_tmp11D1.args=
((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args),((_tmp11D1.c_varargs=fd->c_varargs,((_tmp11D1.cyc_varargs=fd->cyc_varargs,((_tmp11D1.rgn_po=fd->rgn_po,((_tmp11D1.attributes=_tmp829,((_tmp11D1.requires_clause=fd->requires_clause,((_tmp11D1.requires_relns=fd->requires_relns,((_tmp11D1.ensures_clause=fd->ensures_clause,((_tmp11D1.ensures_relns=fd->ensures_relns,_tmp11D1)))))))))))))))))))))))))),_tmp11D2)))),_tmp11D0))));};}
# 3192
return(void*)_check_null(fd->cached_typ);}
# 3198
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 3200
union Cyc_Relations_RelnOp _tmp82E=*rop;union Cyc_Relations_RelnOp _tmp82F=_tmp82E;struct Cyc_Absyn_Vardecl*_tmp830;struct Cyc_Absyn_Vardecl*_tmp831;_LL51F: if((_tmp82F.RVar).tag != 2)goto _LL521;_tmp830=(struct Cyc_Absyn_Vardecl*)(_tmp82F.RVar).val;_LL520: {
# 3202
struct _tuple2 _tmp832=*_tmp830->name;union Cyc_Absyn_Nmspace _tmp834;struct _dyneither_ptr*_tmp835;struct _tuple2 _tmp833=_tmp832;_tmp834=_tmp833.f1;_tmp835=_tmp833.f2;
if(!((int)((_tmp834.Loc_n).tag == 4)))goto _LL51E;
{const char*_tmp11D3;if(Cyc_strcmp((struct _dyneither_ptr)*_tmp835,((_tmp11D3="return_value",_tag_dyneither(_tmp11D3,sizeof(char),13))))== 0){
*rop=Cyc_Relations_RReturn();
goto _LL51E;}}{
# 3208
unsigned int c=0;
{struct Cyc_List_List*_tmp837=args;for(0;_tmp837 != 0;(_tmp837=_tmp837->tl,c ++)){
struct _tuple10*_tmp838=(struct _tuple10*)_tmp837->hd;struct _dyneither_ptr*_tmp83A;struct _tuple10*_tmp839=_tmp838;_tmp83A=_tmp839->f1;
if(_tmp83A != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp835,(struct _dyneither_ptr)*_tmp83A)== 0){
*rop=Cyc_Relations_RParam(c);
break;}}}}
# 3218
goto _LL51E;};}_LL521: if((_tmp82F.RNumelts).tag != 3)goto _LL523;_tmp831=(struct Cyc_Absyn_Vardecl*)(_tmp82F.RNumelts).val;_LL522: {
# 3220
struct _tuple2 _tmp83B=*_tmp831->name;union Cyc_Absyn_Nmspace _tmp83D;struct _dyneither_ptr*_tmp83E;struct _tuple2 _tmp83C=_tmp83B;_tmp83D=_tmp83C.f1;_tmp83E=_tmp83C.f2;
if(!((int)((_tmp83D.Loc_n).tag == 4)))goto _LL51E;{
unsigned int c=0;
{struct Cyc_List_List*_tmp83F=args;for(0;_tmp83F != 0;(_tmp83F=_tmp83F->tl,c ++)){
struct _tuple10*_tmp840=(struct _tuple10*)_tmp83F->hd;struct _dyneither_ptr*_tmp842;struct _tuple10*_tmp841=_tmp840;_tmp842=_tmp841->f1;
if(_tmp842 != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp83E,(struct _dyneither_ptr)*_tmp842)== 0){
*rop=Cyc_Relations_RParamNumelts(c);
break;}}}}
# 3232
goto _LL51E;};}_LL523:;_LL524:
 goto _LL51E;_LL51E:;}
# 3237
static void Cyc_Tcutil_replace_rops(struct Cyc_List_List*args,struct Cyc_Relations_Reln*r){
# 3239
Cyc_Tcutil_replace_rop(args,& r->rop1);
Cyc_Tcutil_replace_rop(args,& r->rop2);}
# 3243
static struct Cyc_List_List*Cyc_Tcutil_extract_relns(struct Cyc_List_List*args,struct Cyc_Absyn_Exp*e){
# 3245
if(e == 0)return 0;{
struct Cyc_List_List*_tmp843=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,_tmp843);
return _tmp843;};}struct _tuple25{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 3252
static void*Cyc_Tcutil_fst_fdarg(struct _tuple25*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct Cyc_Absyn_Tqual _tmp845;void*_tmp846;struct _tuple12*_tmp844=pr;_tmp845=_tmp844->f1;_tmp846=_tmp844->f2;
if(_tmp846 == t)return pr;else{
struct _tuple12*_tmp11D4;return(_tmp11D4=_cycalloc(sizeof(*_tmp11D4)),((_tmp11D4->f1=_tmp845,((_tmp11D4->f2=t,_tmp11D4)))));}}struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 3259
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
# 3261
struct _tuple26*_tmp11D7;struct _tuple27*_tmp11D6;return(_tmp11D6=_region_malloc(rgn,sizeof(*_tmp11D6)),((_tmp11D6->f1=((_tmp11D7=_region_malloc(rgn,sizeof(*_tmp11D7)),((_tmp11D7->f1=(*y).f1,((_tmp11D7->f2=(*y).f2,_tmp11D7)))))),((_tmp11D6->f2=(*y).f3,_tmp11D6)))));}
# 3263
static struct _tuple10*Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){
# 3265
struct _tuple10 _tmp84A=*orig_arg;struct _dyneither_ptr*_tmp84C;struct Cyc_Absyn_Tqual _tmp84D;void*_tmp84E;struct _tuple10 _tmp84B=_tmp84A;_tmp84C=_tmp84B.f1;_tmp84D=_tmp84B.f2;_tmp84E=_tmp84B.f3;
if(t == _tmp84E)return orig_arg;{
struct _tuple10*_tmp11D8;return(_tmp11D8=_cycalloc(sizeof(*_tmp11D8)),((_tmp11D8->f1=_tmp84C,((_tmp11D8->f2=_tmp84D,((_tmp11D8->f3=t,_tmp11D8)))))));};}
# 3269
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){
return f->type;}
# 3272
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 3277
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 3279
struct Cyc_Absyn_Exp*_tmp11D9;return(_tmp11D9=_cycalloc(sizeof(*_tmp11D9)),((_tmp11D9->topt=old->topt,((_tmp11D9->r=r,((_tmp11D9->loc=old->loc,((_tmp11D9->annot=old->annot,_tmp11D9)))))))));}
# 3284
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp851=e->r;void*_tmp852=_tmp851;enum Cyc_Absyn_Primop _tmp858;struct Cyc_List_List*_tmp859;struct Cyc_Absyn_Exp*_tmp85B;struct Cyc_Absyn_Exp*_tmp85C;struct Cyc_Absyn_Exp*_tmp85D;struct Cyc_Absyn_Exp*_tmp85F;struct Cyc_Absyn_Exp*_tmp860;struct Cyc_Absyn_Exp*_tmp862;struct Cyc_Absyn_Exp*_tmp863;struct Cyc_Absyn_Exp*_tmp865;struct Cyc_Absyn_Exp*_tmp866;void*_tmp868;struct Cyc_Absyn_Exp*_tmp869;int _tmp86A;enum Cyc_Absyn_Coercion _tmp86B;void*_tmp86D;struct Cyc_Absyn_Exp*_tmp86F;void*_tmp871;struct Cyc_List_List*_tmp872;void*_tmp874;_LL526: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp853=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp852;if(_tmp853->tag != 0)goto _LL528;}_LL527:
 goto _LL529;_LL528: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp854=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp852;if(_tmp854->tag != 31)goto _LL52A;}_LL529:
 goto _LL52B;_LL52A: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp855=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp852;if(_tmp855->tag != 32)goto _LL52C;}_LL52B:
 goto _LL52D;_LL52C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp856=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp852;if(_tmp856->tag != 1)goto _LL52E;}_LL52D:
 return e;_LL52E: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp857=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp852;if(_tmp857->tag != 2)goto _LL530;else{_tmp858=_tmp857->f1;_tmp859=_tmp857->f2;}}_LL52F:
# 3292
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp859)== 1){
struct Cyc_Absyn_Exp*_tmp875=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp859))->hd;
struct Cyc_Absyn_Exp*_tmp876=Cyc_Tcutil_rsubsexp(r,inst,_tmp875);
if(_tmp876 == _tmp875)return e;{
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp11DF;struct Cyc_Absyn_Exp*_tmp11DE[1];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp11DD;return Cyc_Tcutil_copye(e,(void*)((_tmp11DD=_cycalloc(sizeof(*_tmp11DD)),((_tmp11DD[0]=((_tmp11DF.tag=2,((_tmp11DF.f1=_tmp858,((_tmp11DF.f2=((_tmp11DE[0]=_tmp876,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp11DE,sizeof(struct Cyc_Absyn_Exp*),1)))),_tmp11DF)))))),_tmp11DD)))));};}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp859)== 2){
struct Cyc_Absyn_Exp*_tmp87A=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp859))->hd;
struct Cyc_Absyn_Exp*_tmp87B=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp859->tl))->hd;
struct Cyc_Absyn_Exp*_tmp87C=Cyc_Tcutil_rsubsexp(r,inst,_tmp87A);
struct Cyc_Absyn_Exp*_tmp87D=Cyc_Tcutil_rsubsexp(r,inst,_tmp87B);
if(_tmp87C == _tmp87A  && _tmp87D == _tmp87B)return e;{
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp11E5;struct Cyc_Absyn_Exp*_tmp11E4[2];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp11E3;return Cyc_Tcutil_copye(e,(void*)((_tmp11E3=_cycalloc(sizeof(*_tmp11E3)),((_tmp11E3[0]=((_tmp11E5.tag=2,((_tmp11E5.f1=_tmp858,((_tmp11E5.f2=((_tmp11E4[1]=_tmp87D,((_tmp11E4[0]=_tmp87C,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp11E4,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmp11E5)))))),_tmp11E3)))));};}else{
const char*_tmp11E8;void*_tmp11E7;return(_tmp11E7=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11E8="primop does not have 1 or 2 args!",_tag_dyneither(_tmp11E8,sizeof(char),34))),_tag_dyneither(_tmp11E7,sizeof(void*),0)));}}_LL530: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp85A=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp852;if(_tmp85A->tag != 5)goto _LL532;else{_tmp85B=_tmp85A->f1;_tmp85C=_tmp85A->f2;_tmp85D=_tmp85A->f3;}}_LL531: {
# 3306
struct Cyc_Absyn_Exp*_tmp883=Cyc_Tcutil_rsubsexp(r,inst,_tmp85B);
struct Cyc_Absyn_Exp*_tmp884=Cyc_Tcutil_rsubsexp(r,inst,_tmp85C);
struct Cyc_Absyn_Exp*_tmp885=Cyc_Tcutil_rsubsexp(r,inst,_tmp85D);
if((_tmp883 == _tmp85B  && _tmp884 == _tmp85C) && _tmp885 == _tmp85D)return e;{
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp11EB;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp11EA;return Cyc_Tcutil_copye(e,(void*)((_tmp11EA=_cycalloc(sizeof(*_tmp11EA)),((_tmp11EA[0]=((_tmp11EB.tag=5,((_tmp11EB.f1=_tmp883,((_tmp11EB.f2=_tmp884,((_tmp11EB.f3=_tmp885,_tmp11EB)))))))),_tmp11EA)))));};}_LL532: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp85E=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp852;if(_tmp85E->tag != 6)goto _LL534;else{_tmp85F=_tmp85E->f1;_tmp860=_tmp85E->f2;}}_LL533: {
# 3312
struct Cyc_Absyn_Exp*_tmp888=Cyc_Tcutil_rsubsexp(r,inst,_tmp85F);
struct Cyc_Absyn_Exp*_tmp889=Cyc_Tcutil_rsubsexp(r,inst,_tmp860);
if(_tmp888 == _tmp85F  && _tmp889 == _tmp860)return e;{
struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmp11EE;struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp11ED;return Cyc_Tcutil_copye(e,(void*)((_tmp11ED=_cycalloc(sizeof(*_tmp11ED)),((_tmp11ED[0]=((_tmp11EE.tag=6,((_tmp11EE.f1=_tmp888,((_tmp11EE.f2=_tmp889,_tmp11EE)))))),_tmp11ED)))));};}_LL534: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp861=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp852;if(_tmp861->tag != 7)goto _LL536;else{_tmp862=_tmp861->f1;_tmp863=_tmp861->f2;}}_LL535: {
# 3317
struct Cyc_Absyn_Exp*_tmp88C=Cyc_Tcutil_rsubsexp(r,inst,_tmp862);
struct Cyc_Absyn_Exp*_tmp88D=Cyc_Tcutil_rsubsexp(r,inst,_tmp863);
if(_tmp88C == _tmp862  && _tmp88D == _tmp863)return e;{
struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmp11F1;struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp11F0;return Cyc_Tcutil_copye(e,(void*)((_tmp11F0=_cycalloc(sizeof(*_tmp11F0)),((_tmp11F0[0]=((_tmp11F1.tag=7,((_tmp11F1.f1=_tmp88C,((_tmp11F1.f2=_tmp88D,_tmp11F1)))))),_tmp11F0)))));};}_LL536: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp864=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp852;if(_tmp864->tag != 8)goto _LL538;else{_tmp865=_tmp864->f1;_tmp866=_tmp864->f2;}}_LL537: {
# 3322
struct Cyc_Absyn_Exp*_tmp890=Cyc_Tcutil_rsubsexp(r,inst,_tmp865);
struct Cyc_Absyn_Exp*_tmp891=Cyc_Tcutil_rsubsexp(r,inst,_tmp866);
if(_tmp890 == _tmp865  && _tmp891 == _tmp866)return e;{
struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmp11F4;struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp11F3;return Cyc_Tcutil_copye(e,(void*)((_tmp11F3=_cycalloc(sizeof(*_tmp11F3)),((_tmp11F3[0]=((_tmp11F4.tag=8,((_tmp11F4.f1=_tmp890,((_tmp11F4.f2=_tmp891,_tmp11F4)))))),_tmp11F3)))));};}_LL538: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp867=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp852;if(_tmp867->tag != 13)goto _LL53A;else{_tmp868=(void*)_tmp867->f1;_tmp869=_tmp867->f2;_tmp86A=_tmp867->f3;_tmp86B=_tmp867->f4;}}_LL539: {
# 3327
struct Cyc_Absyn_Exp*_tmp894=Cyc_Tcutil_rsubsexp(r,inst,_tmp869);
void*_tmp895=Cyc_Tcutil_rsubstitute(r,inst,_tmp868);
if(_tmp894 == _tmp869  && _tmp895 == _tmp868)return e;{
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp11F7;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp11F6;return Cyc_Tcutil_copye(e,(void*)((_tmp11F6=_cycalloc(sizeof(*_tmp11F6)),((_tmp11F6[0]=((_tmp11F7.tag=13,((_tmp11F7.f1=_tmp895,((_tmp11F7.f2=_tmp894,((_tmp11F7.f3=_tmp86A,((_tmp11F7.f4=_tmp86B,_tmp11F7)))))))))),_tmp11F6)))));};}_LL53A: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp86C=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp852;if(_tmp86C->tag != 16)goto _LL53C;else{_tmp86D=(void*)_tmp86C->f1;}}_LL53B: {
# 3332
void*_tmp898=Cyc_Tcutil_rsubstitute(r,inst,_tmp86D);
if(_tmp898 == _tmp86D)return e;{
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp11FA;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp11F9;return Cyc_Tcutil_copye(e,(void*)((_tmp11F9=_cycalloc(sizeof(*_tmp11F9)),((_tmp11F9[0]=((_tmp11FA.tag=16,((_tmp11FA.f1=_tmp898,_tmp11FA)))),_tmp11F9)))));};}_LL53C: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp86E=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp852;if(_tmp86E->tag != 17)goto _LL53E;else{_tmp86F=_tmp86E->f1;}}_LL53D: {
# 3336
struct Cyc_Absyn_Exp*_tmp89B=Cyc_Tcutil_rsubsexp(r,inst,_tmp86F);
if(_tmp89B == _tmp86F)return e;{
struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmp11FD;struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp11FC;return Cyc_Tcutil_copye(e,(void*)((_tmp11FC=_cycalloc(sizeof(*_tmp11FC)),((_tmp11FC[0]=((_tmp11FD.tag=17,((_tmp11FD.f1=_tmp89B,_tmp11FD)))),_tmp11FC)))));};}_LL53E: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp870=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp852;if(_tmp870->tag != 18)goto _LL540;else{_tmp871=(void*)_tmp870->f1;_tmp872=_tmp870->f2;}}_LL53F: {
# 3340
void*_tmp89E=Cyc_Tcutil_rsubstitute(r,inst,_tmp871);
if(_tmp89E == _tmp871)return e;{
struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp1200;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp11FF;return Cyc_Tcutil_copye(e,(void*)((_tmp11FF=_cycalloc(sizeof(*_tmp11FF)),((_tmp11FF[0]=((_tmp1200.tag=18,((_tmp1200.f1=_tmp89E,((_tmp1200.f2=_tmp872,_tmp1200)))))),_tmp11FF)))));};}_LL540: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp873=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp852;if(_tmp873->tag != 38)goto _LL542;else{_tmp874=(void*)_tmp873->f1;}}_LL541: {
# 3344
void*_tmp8A1=Cyc_Tcutil_rsubstitute(r,inst,_tmp874);
if(_tmp8A1 == _tmp874)return e;{
# 3347
void*_tmp8A2=Cyc_Tcutil_compress(_tmp8A1);void*_tmp8A3=_tmp8A2;struct Cyc_Absyn_Exp*_tmp8A5;_LL545: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp8A4=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp8A3;if(_tmp8A4->tag != 18)goto _LL547;else{_tmp8A5=_tmp8A4->f1;}}_LL546:
 return _tmp8A5;_LL547:;_LL548: {
# 3350
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp1203;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp1202;return Cyc_Tcutil_copye(e,(void*)((_tmp1202=_cycalloc(sizeof(*_tmp1202)),((_tmp1202[0]=((_tmp1203.tag=38,((_tmp1203.f1=_tmp8A1,_tmp1203)))),_tmp1202)))));}_LL544:;};}_LL542:;_LL543: {
# 3353
const char*_tmp1206;void*_tmp1205;return(_tmp1205=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1206="non-type-level-expression in Tcutil::rsubsexp",_tag_dyneither(_tmp1206,sizeof(char),46))),_tag_dyneither(_tmp1205,sizeof(void*),0)));}_LL525:;}
# 3357
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 3360
if(e == 0)return 0;else{
return Cyc_Tcutil_rsubsexp(r,inst,e);}}
# 3364
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 3367
void*_tmp8AA=f->type;
struct Cyc_Absyn_Exp*_tmp8AB=f->requires_clause;
void*_tmp8AC=Cyc_Tcutil_rsubstitute(r,inst,_tmp8AA);
struct Cyc_Absyn_Exp*_tmp8AD=Cyc_Tcutil_rsubs_exp_opt(r,inst,_tmp8AB);
if(_tmp8AA == _tmp8AC  && _tmp8AB == _tmp8AD)return f;else{
struct Cyc_Absyn_Aggrfield*_tmp1207;return(_tmp1207=_cycalloc(sizeof(*_tmp1207)),((_tmp1207->name=f->name,((_tmp1207->tq=f->tq,((_tmp1207->type=_tmp8AC,((_tmp1207->width=f->width,((_tmp1207->attributes=f->attributes,((_tmp1207->requires_clause=_tmp8AD,_tmp1207)))))))))))));}}
# 3377
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 3380
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp8AF=(struct Cyc_Absyn_Aggrfield*)fs->hd;
struct Cyc_List_List*_tmp8B0=fs->tl;
struct Cyc_Absyn_Aggrfield*_tmp8B1=Cyc_Tcutil_subst_aggrfield(r,inst,_tmp8AF);
struct Cyc_List_List*_tmp8B2=Cyc_Tcutil_subst_aggrfields(r,inst,_tmp8B0);
if(_tmp8B1 == _tmp8AF  && _tmp8B2 == _tmp8B0)return fs;{
# 3387
struct Cyc_List_List*_tmp1208;return(_tmp1208=_cycalloc(sizeof(*_tmp1208)),((_tmp1208->hd=_tmp8B1,((_tmp1208->tl=_tmp8B2,_tmp1208)))));};};}
# 3390
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 3393
struct _tuple1 _tmp8B4=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct Cyc_List_List*_tmp8B6;struct Cyc_List_List*_tmp8B7;struct _tuple1 _tmp8B5=_tmp8B4;_tmp8B6=_tmp8B5.f1;_tmp8B7=_tmp8B5.f2;{
struct Cyc_List_List*_tmp8B8=Cyc_Tcutil_substs(rgn,inst,_tmp8B6);
struct Cyc_List_List*_tmp8B9=Cyc_Tcutil_substs(rgn,inst,_tmp8B7);
if(_tmp8B8 == _tmp8B6  && _tmp8B9 == _tmp8B7)
return rgn_po;else{
# 3399
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp8B8,_tmp8B9);}};}
# 3402
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3405
void*_tmp8BA=Cyc_Tcutil_compress(t);void*_tmp8BB=_tmp8BA;struct Cyc_Absyn_Tvar*_tmp8BD;union Cyc_Absyn_AggrInfoU _tmp8BF;struct Cyc_List_List*_tmp8C0;union Cyc_Absyn_DatatypeInfoU _tmp8C2;struct Cyc_List_List*_tmp8C3;union Cyc_Absyn_DatatypeFieldInfoU _tmp8C5;struct Cyc_List_List*_tmp8C6;struct _tuple2*_tmp8C8;struct Cyc_List_List*_tmp8C9;struct Cyc_Absyn_Typedefdecl*_tmp8CA;void*_tmp8CB;void*_tmp8CD;struct Cyc_Absyn_Tqual _tmp8CE;struct Cyc_Absyn_Exp*_tmp8CF;union Cyc_Absyn_Constraint*_tmp8D0;unsigned int _tmp8D1;void*_tmp8D3;struct Cyc_Absyn_Tqual _tmp8D4;void*_tmp8D5;union Cyc_Absyn_Constraint*_tmp8D6;union Cyc_Absyn_Constraint*_tmp8D7;union Cyc_Absyn_Constraint*_tmp8D8;struct Cyc_List_List*_tmp8DA;void*_tmp8DB;struct Cyc_Absyn_Tqual _tmp8DC;void*_tmp8DD;struct Cyc_List_List*_tmp8DE;int _tmp8DF;struct Cyc_Absyn_VarargInfo*_tmp8E0;struct Cyc_List_List*_tmp8E1;struct Cyc_List_List*_tmp8E2;struct Cyc_Absyn_Exp*_tmp8E3;struct Cyc_Absyn_Exp*_tmp8E4;struct Cyc_List_List*_tmp8E6;enum Cyc_Absyn_AggrKind _tmp8E8;struct Cyc_List_List*_tmp8E9;void*_tmp8EB;void*_tmp8ED;void*_tmp8EF;void*_tmp8F0;void*_tmp8F2;struct Cyc_Absyn_Exp*_tmp8F4;struct Cyc_Absyn_Exp*_tmp8F6;void*_tmp901;void*_tmp903;struct Cyc_List_List*_tmp905;_LL54A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8BC=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp8BB;if(_tmp8BC->tag != 2)goto _LL54C;else{_tmp8BD=_tmp8BC->f1;}}_LL54B: {
# 3408
struct _handler_cons _tmp907;_push_handler(& _tmp907);{int _tmp909=0;if(setjmp(_tmp907.handler))_tmp909=1;if(!_tmp909){{void*_tmp90A=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp8BD);_npop_handler(0);return _tmp90A;};_pop_handler();}else{void*_tmp908=(void*)_exn_thrown;void*_tmp90C=_tmp908;void*_tmp90E;_LL585: {struct Cyc_Core_Not_found_exn_struct*_tmp90D=(struct Cyc_Core_Not_found_exn_struct*)_tmp90C;if(_tmp90D->tag != Cyc_Core_Not_found)goto _LL587;}_LL586:
 return t;_LL587: _tmp90E=_tmp90C;_LL588:(void)_rethrow(_tmp90E);_LL584:;}};}_LL54C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp8BE=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp8BB;if(_tmp8BE->tag != 11)goto _LL54E;else{_tmp8BF=(_tmp8BE->f1).aggr_info;_tmp8C0=(_tmp8BE->f1).targs;}}_LL54D: {
# 3411
struct Cyc_List_List*_tmp90F=Cyc_Tcutil_substs(rgn,inst,_tmp8C0);
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp120E;struct Cyc_Absyn_AggrInfo _tmp120D;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp120C;return _tmp90F == _tmp8C0?t:(void*)((_tmp120C=_cycalloc(sizeof(*_tmp120C)),((_tmp120C[0]=((_tmp120E.tag=11,((_tmp120E.f1=((_tmp120D.aggr_info=_tmp8BF,((_tmp120D.targs=_tmp90F,_tmp120D)))),_tmp120E)))),_tmp120C))));}_LL54E: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp8C1=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp8BB;if(_tmp8C1->tag != 3)goto _LL550;else{_tmp8C2=(_tmp8C1->f1).datatype_info;_tmp8C3=(_tmp8C1->f1).targs;}}_LL54F: {
# 3414
struct Cyc_List_List*_tmp913=Cyc_Tcutil_substs(rgn,inst,_tmp8C3);
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp1214;struct Cyc_Absyn_DatatypeInfo _tmp1213;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp1212;return _tmp913 == _tmp8C3?t:(void*)(
(_tmp1212=_cycalloc(sizeof(*_tmp1212)),((_tmp1212[0]=((_tmp1214.tag=3,((_tmp1214.f1=((_tmp1213.datatype_info=_tmp8C2,((_tmp1213.targs=_tmp913,_tmp1213)))),_tmp1214)))),_tmp1212))));}_LL550: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp8C4=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp8BB;if(_tmp8C4->tag != 4)goto _LL552;else{_tmp8C5=(_tmp8C4->f1).field_info;_tmp8C6=(_tmp8C4->f1).targs;}}_LL551: {
# 3418
struct Cyc_List_List*_tmp917=Cyc_Tcutil_substs(rgn,inst,_tmp8C6);
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp121A;struct Cyc_Absyn_DatatypeFieldInfo _tmp1219;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp1218;return _tmp917 == _tmp8C6?t:(void*)(
(_tmp1218=_cycalloc(sizeof(*_tmp1218)),((_tmp1218[0]=((_tmp121A.tag=4,((_tmp121A.f1=((_tmp1219.field_info=_tmp8C5,((_tmp1219.targs=_tmp917,_tmp1219)))),_tmp121A)))),_tmp1218))));}_LL552: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp8C7=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp8BB;if(_tmp8C7->tag != 17)goto _LL554;else{_tmp8C8=_tmp8C7->f1;_tmp8C9=_tmp8C7->f2;_tmp8CA=_tmp8C7->f3;_tmp8CB=(void*)_tmp8C7->f4;}}_LL553: {
# 3422
struct Cyc_List_List*_tmp91B=Cyc_Tcutil_substs(rgn,inst,_tmp8C9);
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp121D;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp121C;return _tmp91B == _tmp8C9?t:(void*)((_tmp121C=_cycalloc(sizeof(*_tmp121C)),((_tmp121C[0]=((_tmp121D.tag=17,((_tmp121D.f1=_tmp8C8,((_tmp121D.f2=_tmp91B,((_tmp121D.f3=_tmp8CA,((_tmp121D.f4=_tmp8CB,_tmp121D)))))))))),_tmp121C))));}_LL554: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8CC=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8BB;if(_tmp8CC->tag != 8)goto _LL556;else{_tmp8CD=(_tmp8CC->f1).elt_type;_tmp8CE=(_tmp8CC->f1).tq;_tmp8CF=(_tmp8CC->f1).num_elts;_tmp8D0=(_tmp8CC->f1).zero_term;_tmp8D1=(_tmp8CC->f1).zt_loc;}}_LL555: {
# 3425
void*_tmp91E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8CD);
struct Cyc_Absyn_Exp*_tmp91F=_tmp8CF == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,_tmp8CF);
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp1223;struct Cyc_Absyn_ArrayInfo _tmp1222;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1221;return _tmp91E == _tmp8CD  && _tmp91F == _tmp8CF?t:(void*)(
(_tmp1221=_cycalloc(sizeof(*_tmp1221)),((_tmp1221[0]=((_tmp1223.tag=8,((_tmp1223.f1=((_tmp1222.elt_type=_tmp91E,((_tmp1222.tq=_tmp8CE,((_tmp1222.num_elts=_tmp91F,((_tmp1222.zero_term=_tmp8D0,((_tmp1222.zt_loc=_tmp8D1,_tmp1222)))))))))),_tmp1223)))),_tmp1221))));}_LL556: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8D2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8BB;if(_tmp8D2->tag != 5)goto _LL558;else{_tmp8D3=(_tmp8D2->f1).elt_typ;_tmp8D4=(_tmp8D2->f1).elt_tq;_tmp8D5=((_tmp8D2->f1).ptr_atts).rgn;_tmp8D6=((_tmp8D2->f1).ptr_atts).nullable;_tmp8D7=((_tmp8D2->f1).ptr_atts).bounds;_tmp8D8=((_tmp8D2->f1).ptr_atts).zero_term;}}_LL557: {
# 3430
void*_tmp923=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8D3);
void*_tmp924=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8D5);
union Cyc_Absyn_Constraint*_tmp925=_tmp8D7;
{void*_tmp926=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp8D7);void*_tmp927=_tmp926;struct Cyc_Absyn_Exp*_tmp929;_LL58A: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp928=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp927;if(_tmp928->tag != 1)goto _LL58C;else{_tmp929=_tmp928->f1;}}_LL58B: {
# 3435
struct Cyc_Absyn_Exp*_tmp92A=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp929);
if(_tmp92A != _tmp929){
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp1226;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp1225;_tmp925=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp1225=_cycalloc(sizeof(*_tmp1225)),((_tmp1225[0]=((_tmp1226.tag=1,((_tmp1226.f1=_tmp92A,_tmp1226)))),_tmp1225)))));}
goto _LL589;}_LL58C:;_LL58D:
 goto _LL589;_LL589:;}
# 3441
if((_tmp923 == _tmp8D3  && _tmp924 == _tmp8D5) && _tmp925 == _tmp8D7)
return t;{
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp122C;struct Cyc_Absyn_PtrInfo _tmp122B;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp122A;return(void*)((_tmp122A=_cycalloc(sizeof(*_tmp122A)),((_tmp122A[0]=((_tmp122C.tag=5,((_tmp122C.f1=((_tmp122B.elt_typ=_tmp923,((_tmp122B.elt_tq=_tmp8D4,((_tmp122B.ptr_atts=(((_tmp122B.ptr_atts).rgn=_tmp924,(((_tmp122B.ptr_atts).nullable=_tmp8D6,(((_tmp122B.ptr_atts).bounds=_tmp925,(((_tmp122B.ptr_atts).zero_term=_tmp8D8,(((_tmp122B.ptr_atts).ptrloc=0,_tmp122B.ptr_atts)))))))))),_tmp122B)))))),_tmp122C)))),_tmp122A))));};}_LL558: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp8D9=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp8BB;if(_tmp8D9->tag != 9)goto _LL55A;else{_tmp8DA=(_tmp8D9->f1).tvars;_tmp8DB=(_tmp8D9->f1).effect;_tmp8DC=(_tmp8D9->f1).ret_tqual;_tmp8DD=(_tmp8D9->f1).ret_typ;_tmp8DE=(_tmp8D9->f1).args;_tmp8DF=(_tmp8D9->f1).c_varargs;_tmp8E0=(_tmp8D9->f1).cyc_varargs;_tmp8E1=(_tmp8D9->f1).rgn_po;_tmp8E2=(_tmp8D9->f1).attributes;_tmp8E3=(_tmp8D9->f1).requires_clause;_tmp8E4=(_tmp8D9->f1).ensures_clause;}}_LL559:
# 3447
{struct Cyc_List_List*_tmp930=_tmp8DA;for(0;_tmp930 != 0;_tmp930=_tmp930->tl){
struct _tuple16*_tmp1236;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1235;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1234;struct Cyc_List_List*_tmp1233;inst=((_tmp1233=_region_malloc(rgn,sizeof(*_tmp1233)),((_tmp1233->hd=((_tmp1236=_region_malloc(rgn,sizeof(*_tmp1236)),((_tmp1236->f1=(struct Cyc_Absyn_Tvar*)_tmp930->hd,((_tmp1236->f2=(void*)((_tmp1234=_cycalloc(sizeof(*_tmp1234)),((_tmp1234[0]=((_tmp1235.tag=2,((_tmp1235.f1=(struct Cyc_Absyn_Tvar*)_tmp930->hd,_tmp1235)))),_tmp1234)))),_tmp1236)))))),((_tmp1233->tl=inst,_tmp1233))))));}}{
struct _tuple1 _tmp935=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp8DE));
# 3449
struct Cyc_List_List*_tmp937;struct Cyc_List_List*_tmp938;struct _tuple1 _tmp936=_tmp935;_tmp937=_tmp936.f1;_tmp938=_tmp936.f2;{
# 3451
struct Cyc_List_List*_tmp939=_tmp8DE;
struct Cyc_List_List*_tmp93A=Cyc_Tcutil_substs(rgn,inst,_tmp938);
if(_tmp93A != _tmp938)
_tmp939=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmp8DE,_tmp93A);{
void*eff2;
if(_tmp8DB == 0)
eff2=0;else{
# 3459
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8DB);
if(new_eff == _tmp8DB)
eff2=_tmp8DB;else{
# 3463
eff2=new_eff;}}{
# 3465
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(_tmp8E0 == 0)
cyc_varargs2=0;else{
# 3469
struct Cyc_Absyn_VarargInfo _tmp93B=*_tmp8E0;struct _dyneither_ptr*_tmp93D;struct Cyc_Absyn_Tqual _tmp93E;void*_tmp93F;int _tmp940;struct Cyc_Absyn_VarargInfo _tmp93C=_tmp93B;_tmp93D=_tmp93C.name;_tmp93E=_tmp93C.tq;_tmp93F=_tmp93C.type;_tmp940=_tmp93C.inject;{
void*_tmp941=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp93F);
if(_tmp941 == _tmp93F)cyc_varargs2=_tmp8E0;else{
# 3473
struct Cyc_Absyn_VarargInfo*_tmp1237;cyc_varargs2=((_tmp1237=_cycalloc(sizeof(*_tmp1237)),((_tmp1237->name=_tmp93D,((_tmp1237->tq=_tmp93E,((_tmp1237->type=_tmp941,((_tmp1237->inject=_tmp940,_tmp1237))))))))));}};}{
# 3475
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp8E1);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp8E3);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp8E4);
struct Cyc_List_List*_tmp943=Cyc_Tcutil_extract_relns(_tmp939,req2);
struct Cyc_List_List*_tmp944=Cyc_Tcutil_extract_relns(_tmp939,ens2);
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp123D;struct Cyc_Absyn_FnInfo _tmp123C;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp123B;return(void*)(
(_tmp123B=_cycalloc(sizeof(*_tmp123B)),((_tmp123B[0]=((_tmp123D.tag=9,((_tmp123D.f1=((_tmp123C.tvars=_tmp8DA,((_tmp123C.effect=eff2,((_tmp123C.ret_tqual=_tmp8DC,((_tmp123C.ret_typ=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8DD),((_tmp123C.args=_tmp939,((_tmp123C.c_varargs=_tmp8DF,((_tmp123C.cyc_varargs=cyc_varargs2,((_tmp123C.rgn_po=rgn_po2,((_tmp123C.attributes=_tmp8E2,((_tmp123C.requires_clause=req2,((_tmp123C.requires_relns=_tmp943,((_tmp123C.ensures_clause=ens2,((_tmp123C.ensures_relns=_tmp944,_tmp123C)))))))))))))))))))))))))),_tmp123D)))),_tmp123B))));};};};};};_LL55A: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp8E5=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp8BB;if(_tmp8E5->tag != 10)goto _LL55C;else{_tmp8E6=_tmp8E5->f1;}}_LL55B: {
# 3485
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp948=_tmp8E6;for(0;_tmp948 != 0;_tmp948=_tmp948->tl){
void*_tmp949=(*((struct _tuple12*)_tmp948->hd)).f2;
void*_tmp94A=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp949);
if(_tmp949 != _tmp94A)
change=1;{
# 3493
struct Cyc_List_List*_tmp123E;ts2=((_tmp123E=_region_malloc(rgn,sizeof(*_tmp123E)),((_tmp123E->hd=_tmp94A,((_tmp123E->tl=ts2,_tmp123E))))));};}}
# 3495
if(!change)
return t;{
struct Cyc_List_List*_tmp94C=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp8E6,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp1241;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp1240;return(void*)((_tmp1240=_cycalloc(sizeof(*_tmp1240)),((_tmp1240[0]=((_tmp1241.tag=10,((_tmp1241.f1=_tmp94C,_tmp1241)))),_tmp1240))));};}_LL55C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp8E7=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp8BB;if(_tmp8E7->tag != 12)goto _LL55E;else{_tmp8E8=_tmp8E7->f1;_tmp8E9=_tmp8E7->f2;}}_LL55D: {
# 3500
struct Cyc_List_List*_tmp94F=Cyc_Tcutil_subst_aggrfields(rgn,inst,_tmp8E9);
if(_tmp8E9 == _tmp94F)return t;{
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp1244;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1243;return(void*)((_tmp1243=_cycalloc(sizeof(*_tmp1243)),((_tmp1243[0]=((_tmp1244.tag=12,((_tmp1244.f1=_tmp8E8,((_tmp1244.f2=_tmp94F,_tmp1244)))))),_tmp1243))));};}_LL55E: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp8EA=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp8BB;if(_tmp8EA->tag != 1)goto _LL560;else{_tmp8EB=(void*)_tmp8EA->f2;}}_LL55F:
# 3504
 if(_tmp8EB != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8EB);else{
return t;}_LL560: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp8EC=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp8BB;if(_tmp8EC->tag != 15)goto _LL562;else{_tmp8ED=(void*)_tmp8EC->f1;}}_LL561: {
# 3507
void*_tmp952=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8ED);
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp1247;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp1246;return _tmp952 == _tmp8ED?t:(void*)((_tmp1246=_cycalloc(sizeof(*_tmp1246)),((_tmp1246[0]=((_tmp1247.tag=15,((_tmp1247.f1=_tmp952,_tmp1247)))),_tmp1246))));}_LL562: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp8EE=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp8BB;if(_tmp8EE->tag != 16)goto _LL564;else{_tmp8EF=(void*)_tmp8EE->f1;_tmp8F0=(void*)_tmp8EE->f2;}}_LL563: {
# 3510
void*_tmp955=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8EF);
void*_tmp956=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8F0);
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp124A;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp1249;return _tmp955 == _tmp8EF  && _tmp956 == _tmp8F0?t:(void*)((_tmp1249=_cycalloc(sizeof(*_tmp1249)),((_tmp1249[0]=((_tmp124A.tag=16,((_tmp124A.f1=_tmp955,((_tmp124A.f2=_tmp956,_tmp124A)))))),_tmp1249))));}_LL564: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp8F1=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp8BB;if(_tmp8F1->tag != 19)goto _LL566;else{_tmp8F2=(void*)_tmp8F1->f1;}}_LL565: {
# 3514
void*_tmp959=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8F2);
struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp124D;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp124C;return _tmp959 == _tmp8F2?t:(void*)((_tmp124C=_cycalloc(sizeof(*_tmp124C)),((_tmp124C[0]=((_tmp124D.tag=19,((_tmp124D.f1=_tmp959,_tmp124D)))),_tmp124C))));}_LL566: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp8F3=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp8BB;if(_tmp8F3->tag != 18)goto _LL568;else{_tmp8F4=_tmp8F3->f1;}}_LL567: {
# 3517
struct Cyc_Absyn_Exp*_tmp95C=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp8F4);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp1250;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp124F;return _tmp95C == _tmp8F4?t:(void*)((_tmp124F=_cycalloc(sizeof(*_tmp124F)),((_tmp124F[0]=((_tmp1250.tag=18,((_tmp1250.f1=_tmp95C,_tmp1250)))),_tmp124F))));}_LL568: {struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp8F5=(struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp8BB;if(_tmp8F5->tag != 27)goto _LL56A;else{_tmp8F6=_tmp8F5->f1;}}_LL569: {
# 3520
struct Cyc_Absyn_Exp*_tmp95F=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp8F6);
struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmp1253;struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp1252;return _tmp95F == _tmp8F6?t:(void*)((_tmp1252=_cycalloc(sizeof(*_tmp1252)),((_tmp1252[0]=((_tmp1253.tag=27,((_tmp1253.f1=_tmp95F,_tmp1253)))),_tmp1252))));}_LL56A: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp8F7=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp8BB;if(_tmp8F7->tag != 13)goto _LL56C;}_LL56B:
 goto _LL56D;_LL56C: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp8F8=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp8BB;if(_tmp8F8->tag != 14)goto _LL56E;}_LL56D:
 goto _LL56F;_LL56E: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp8F9=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp8BB;if(_tmp8F9->tag != 0)goto _LL570;}_LL56F:
 goto _LL571;_LL570: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8FA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp8BB;if(_tmp8FA->tag != 6)goto _LL572;}_LL571:
 goto _LL573;_LL572: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp8FB=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp8BB;if(_tmp8FB->tag != 7)goto _LL574;}_LL573:
 goto _LL575;_LL574: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp8FC=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp8BB;if(_tmp8FC->tag != 22)goto _LL576;}_LL575:
 goto _LL577;_LL576: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp8FD=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp8BB;if(_tmp8FD->tag != 21)goto _LL578;}_LL577:
 goto _LL579;_LL578: {struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*_tmp8FE=(struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp8BB;if(_tmp8FE->tag != 28)goto _LL57A;}_LL579:
 goto _LL57B;_LL57A: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp8FF=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp8BB;if(_tmp8FF->tag != 20)goto _LL57C;}_LL57B:
 return t;_LL57C: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp900=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp8BB;if(_tmp900->tag != 25)goto _LL57E;else{_tmp901=(void*)_tmp900->f1;}}_LL57D: {
# 3532
void*_tmp962=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp901);
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp1256;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1255;return _tmp962 == _tmp901?t:(void*)((_tmp1255=_cycalloc(sizeof(*_tmp1255)),((_tmp1255[0]=((_tmp1256.tag=25,((_tmp1256.f1=_tmp962,_tmp1256)))),_tmp1255))));}_LL57E: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp902=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp8BB;if(_tmp902->tag != 23)goto _LL580;else{_tmp903=(void*)_tmp902->f1;}}_LL57F: {
# 3535
void*_tmp965=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp903);
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1259;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1258;return _tmp965 == _tmp903?t:(void*)((_tmp1258=_cycalloc(sizeof(*_tmp1258)),((_tmp1258[0]=((_tmp1259.tag=23,((_tmp1259.f1=_tmp965,_tmp1259)))),_tmp1258))));}_LL580: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp904=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp8BB;if(_tmp904->tag != 24)goto _LL582;else{_tmp905=_tmp904->f1;}}_LL581: {
# 3538
struct Cyc_List_List*_tmp968=Cyc_Tcutil_substs(rgn,inst,_tmp905);
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp125C;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp125B;return _tmp968 == _tmp905?t:(void*)((_tmp125B=_cycalloc(sizeof(*_tmp125B)),((_tmp125B[0]=((_tmp125C.tag=24,((_tmp125C.f1=_tmp968,_tmp125C)))),_tmp125B))));}_LL582: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp906=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp8BB;if(_tmp906->tag != 26)goto _LL549;}_LL583: {
const char*_tmp125F;void*_tmp125E;(_tmp125E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp125F="found typedecltype in rsubs",_tag_dyneither(_tmp125F,sizeof(char),28))),_tag_dyneither(_tmp125E,sizeof(void*),0)));}_LL549:;}
# 3544
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 3547
if(ts == 0)
return 0;{
void*_tmp96D=(void*)ts->hd;
struct Cyc_List_List*_tmp96E=ts->tl;
void*_tmp96F=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp96D);
struct Cyc_List_List*_tmp970=Cyc_Tcutil_substs(rgn,inst,_tmp96E);
if(_tmp96D == _tmp96F  && _tmp96E == _tmp970)
return ts;{
struct Cyc_List_List*_tmp1260;return(_tmp1260=_cycalloc(sizeof(*_tmp1260)),((_tmp1260->hd=_tmp96F,((_tmp1260->tl=_tmp970,_tmp1260)))));};};}
# 3558
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 3565
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp972=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
struct Cyc_Core_Opt*_tmp1263;struct _tuple16*_tmp1262;return(_tmp1262=_cycalloc(sizeof(*_tmp1262)),((_tmp1262->f1=tv,((_tmp1262->f2=Cyc_Absyn_new_evar(_tmp972,((_tmp1263=_cycalloc(sizeof(*_tmp1263)),((_tmp1263->v=s,_tmp1263))))),_tmp1262)))));}
# 3570
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*env,struct Cyc_Absyn_Tvar*tv){
# 3572
struct Cyc_List_List*_tmp976;struct _RegionHandle*_tmp977;struct _tuple17*_tmp975=env;_tmp976=_tmp975->f1;_tmp977=_tmp975->f2;{
struct Cyc_Core_Opt*_tmp978=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
struct Cyc_Core_Opt*_tmp1266;struct _tuple16*_tmp1265;return(_tmp1265=_region_malloc(_tmp977,sizeof(*_tmp1265)),((_tmp1265->f1=tv,((_tmp1265->f2=Cyc_Absyn_new_evar(_tmp978,((_tmp1266=_cycalloc(sizeof(*_tmp1266)),((_tmp1266->v=_tmp976,_tmp1266))))),_tmp1265)))));};}
# 3582
static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(unsigned int loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3586
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2)){
const char*_tmp126C;void*_tmp126B[3];struct Cyc_String_pa_PrintArg_struct _tmp126A;struct Cyc_String_pa_PrintArg_struct _tmp1269;struct Cyc_String_pa_PrintArg_struct _tmp1268;(_tmp1268.tag=0,((_tmp1268.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(k2)),((_tmp1269.tag=0,((_tmp1269.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1)),((_tmp126A.tag=0,((_tmp126A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmp126B[0]=& _tmp126A,((_tmp126B[1]=& _tmp1269,((_tmp126B[2]=& _tmp1268,Cyc_Tcutil_terr(loc,((_tmp126C="type variable %s is used with inconsistent kinds %s and %s",_tag_dyneither(_tmp126C,sizeof(char),59))),_tag_dyneither(_tmp126B,sizeof(void*),3)))))))))))))))))));}
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){
const char*_tmp126F;void*_tmp126E;(_tmp126E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp126F="same type variable has different identity!",_tag_dyneither(_tmp126F,sizeof(char),43))),_tag_dyneither(_tmp126E,sizeof(void*),0)));}}
return tvs;}}}
# 3600
tv->identity=Cyc_Tcutil_new_tvar_id();{
struct Cyc_List_List*_tmp1270;return(_tmp1270=_cycalloc(sizeof(*_tmp1270)),((_tmp1270->hd=tv,((_tmp1270->tl=tvs,_tmp1270)))));};}
# 3606
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3608
if(tv->identity == - 1){
const char*_tmp1273;void*_tmp1272;(_tmp1272=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1273="fast_add_free_tvar: bad identity in tv",_tag_dyneither(_tmp1273,sizeof(char),39))),_tag_dyneither(_tmp1272,sizeof(void*),0)));}
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3612
struct Cyc_Absyn_Tvar*_tmp985=(struct Cyc_Absyn_Tvar*)tvs2->hd;
if(_tmp985->identity == - 1){
const char*_tmp1276;void*_tmp1275;(_tmp1275=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1276="fast_add_free_tvar: bad identity in tvs2",_tag_dyneither(_tmp1276,sizeof(char),41))),_tag_dyneither(_tmp1275,sizeof(void*),0)));}
if(_tmp985->identity == tv->identity)
return tvs;}}{
# 3619
struct Cyc_List_List*_tmp1277;return(_tmp1277=_cycalloc(sizeof(*_tmp1277)),((_tmp1277->hd=tv,((_tmp1277->tl=tvs,_tmp1277)))));};}struct _tuple28{struct Cyc_Absyn_Tvar*f1;int f2;};
# 3625
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 3630
if(tv->identity == - 1){
const char*_tmp127A;void*_tmp1279;(_tmp1279=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp127A="fast_add_free_tvar_bool: bad identity in tv",_tag_dyneither(_tmp127A,sizeof(char),44))),_tag_dyneither(_tmp1279,sizeof(void*),0)));}
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3634
struct _tuple28*_tmp98B=(struct _tuple28*)tvs2->hd;struct Cyc_Absyn_Tvar*_tmp98D;int*_tmp98E;struct _tuple28*_tmp98C=_tmp98B;_tmp98D=_tmp98C->f1;_tmp98E=(int*)& _tmp98C->f2;
if(_tmp98D->identity == - 1){
const char*_tmp127D;void*_tmp127C;(_tmp127C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp127D="fast_add_free_tvar_bool: bad identity in tvs2",_tag_dyneither(_tmp127D,sizeof(char),46))),_tag_dyneither(_tmp127C,sizeof(void*),0)));}
if(_tmp98D->identity == tv->identity){
*_tmp98E=*_tmp98E  || b;
return tvs;}}}{
# 3642
struct _tuple28*_tmp1280;struct Cyc_List_List*_tmp127F;return(_tmp127F=_region_malloc(r,sizeof(*_tmp127F)),((_tmp127F->hd=((_tmp1280=_region_malloc(r,sizeof(*_tmp1280)),((_tmp1280->f1=tv,((_tmp1280->f2=b,_tmp1280)))))),((_tmp127F->tl=tvs,_tmp127F)))));};}
# 3646
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3648
if(tv->identity == - 1){
const char*_tmp1284;void*_tmp1283[1];struct Cyc_String_pa_PrintArg_struct _tmp1282;(_tmp1282.tag=0,((_tmp1282.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(tv)),((_tmp1283[0]=& _tmp1282,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1284="bound tvar id for %s is NULL",_tag_dyneither(_tmp1284,sizeof(char),29))),_tag_dyneither(_tmp1283,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp1285;return(_tmp1285=_cycalloc(sizeof(*_tmp1285)),((_tmp1285->hd=tv,((_tmp1285->tl=tvs,_tmp1285)))));};}struct _tuple29{void*f1;int f2;};
# 3657
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 3660
void*_tmp997=Cyc_Tcutil_compress(e);void*_tmp998=_tmp997;int _tmp99A;_LL58F: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp999=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp998;if(_tmp999->tag != 1)goto _LL591;else{_tmp99A=_tmp999->f3;}}_LL590:
# 3662
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple29*_tmp99B=(struct _tuple29*)es2->hd;void*_tmp99D;int*_tmp99E;struct _tuple29*_tmp99C=_tmp99B;_tmp99D=_tmp99C->f1;_tmp99E=(int*)& _tmp99C->f2;{
void*_tmp99F=Cyc_Tcutil_compress(_tmp99D);void*_tmp9A0=_tmp99F;int _tmp9A2;_LL594: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp9A1=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp9A0;if(_tmp9A1->tag != 1)goto _LL596;else{_tmp9A2=_tmp9A1->f3;}}_LL595:
# 3666
 if(_tmp99A == _tmp9A2){
if(b != *_tmp99E)*_tmp99E=1;
return es;}
# 3670
goto _LL593;_LL596:;_LL597:
 goto _LL593;_LL593:;};}}{
# 3674
struct _tuple29*_tmp1288;struct Cyc_List_List*_tmp1287;return(_tmp1287=_region_malloc(r,sizeof(*_tmp1287)),((_tmp1287->hd=((_tmp1288=_region_malloc(r,sizeof(*_tmp1288)),((_tmp1288->f1=e,((_tmp1288->f2=b,_tmp1288)))))),((_tmp1287->tl=es,_tmp1287)))));};_LL591:;_LL592:
 return es;_LL58E:;}
# 3679
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3682
struct Cyc_List_List*r=0;
for(0;tvs != 0;tvs=tvs->tl){
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3691
if(!present){struct Cyc_List_List*_tmp1289;r=((_tmp1289=_region_malloc(rgn,sizeof(*_tmp1289)),((_tmp1289->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1289->tl=r,_tmp1289))))));}}
# 3693
r=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);
return r;}
# 3698
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3702
struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp9A6=*((struct _tuple28*)tvs->hd);struct Cyc_Absyn_Tvar*_tmp9A8;int _tmp9A9;struct _tuple28 _tmp9A7=_tmp9A6;_tmp9A8=_tmp9A7.f1;_tmp9A9=_tmp9A7.f2;{
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp9A8->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3712
if(!present){struct Cyc_List_List*_tmp128A;res=((_tmp128A=_region_malloc(r,sizeof(*_tmp128A)),((_tmp128A->hd=(struct _tuple28*)tvs->hd,((_tmp128A->tl=res,_tmp128A))))));}};}
# 3714
res=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);
return res;}
# 3718
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){
# 3720
if(width != 0){
unsigned int w=0;
if(!Cyc_Tcutil_is_const_exp(width)){
const char*_tmp128E;void*_tmp128D[1];struct Cyc_String_pa_PrintArg_struct _tmp128C;(_tmp128C.tag=0,((_tmp128C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp128D[0]=& _tmp128C,Cyc_Tcutil_terr(loc,((_tmp128E="bitfield %s does not have constant width",_tag_dyneither(_tmp128E,sizeof(char),41))),_tag_dyneither(_tmp128D,sizeof(void*),1)))))));}else{
# 3725
struct _tuple14 _tmp9AE=Cyc_Evexp_eval_const_uint_exp(width);unsigned int _tmp9B0;int _tmp9B1;struct _tuple14 _tmp9AF=_tmp9AE;_tmp9B0=_tmp9AF.f1;_tmp9B1=_tmp9AF.f2;
if(!_tmp9B1){
const char*_tmp1291;void*_tmp1290;(_tmp1290=0,Cyc_Tcutil_terr(loc,((_tmp1291="bitfield width cannot use sizeof or offsetof",_tag_dyneither(_tmp1291,sizeof(char),45))),_tag_dyneither(_tmp1290,sizeof(void*),0)));}
w=_tmp9B0;}{
# 3730
void*_tmp9B4=Cyc_Tcutil_compress(field_typ);void*_tmp9B5=_tmp9B4;enum Cyc_Absyn_Size_of _tmp9B7;_LL599: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp9B6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp9B5;if(_tmp9B6->tag != 6)goto _LL59B;else{_tmp9B7=_tmp9B6->f2;}}_LL59A:
# 3733
 switch(_tmp9B7){case Cyc_Absyn_Char_sz: _LL59D:
 if(w > 8){const char*_tmp1294;void*_tmp1293;(_tmp1293=0,Cyc_Tcutil_terr(loc,((_tmp1294="bitfield larger than type",_tag_dyneither(_tmp1294,sizeof(char),26))),_tag_dyneither(_tmp1293,sizeof(void*),0)));}break;case Cyc_Absyn_Short_sz: _LL59E:
 if(w > 16){const char*_tmp1297;void*_tmp1296;(_tmp1296=0,Cyc_Tcutil_terr(loc,((_tmp1297="bitfield larger than type",_tag_dyneither(_tmp1297,sizeof(char),26))),_tag_dyneither(_tmp1296,sizeof(void*),0)));}break;case Cyc_Absyn_Long_sz: _LL59F:
 goto _LL5A0;case Cyc_Absyn_Int_sz: _LL5A0:
# 3738
 if(w > 32){const char*_tmp129A;void*_tmp1299;(_tmp1299=0,Cyc_Tcutil_terr(loc,((_tmp129A="bitfield larger than type",_tag_dyneither(_tmp129A,sizeof(char),26))),_tag_dyneither(_tmp1299,sizeof(void*),0)));}break;case Cyc_Absyn_LongLong_sz: _LL5A1:
# 3740
 if(w > 64){const char*_tmp129D;void*_tmp129C;(_tmp129C=0,Cyc_Tcutil_terr(loc,((_tmp129D="bitfield larger than type",_tag_dyneither(_tmp129D,sizeof(char),26))),_tag_dyneither(_tmp129C,sizeof(void*),0)));}break;}
# 3742
goto _LL598;_LL59B:;_LL59C:
# 3744
{const char*_tmp12A2;void*_tmp12A1[2];struct Cyc_String_pa_PrintArg_struct _tmp12A0;struct Cyc_String_pa_PrintArg_struct _tmp129F;(_tmp129F.tag=0,((_tmp129F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(field_typ)),((_tmp12A0.tag=0,((_tmp12A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp12A1[0]=& _tmp12A0,((_tmp12A1[1]=& _tmp129F,Cyc_Tcutil_terr(loc,((_tmp12A2="bitfield %s must have integral type but has type %s",_tag_dyneither(_tmp12A2,sizeof(char),52))),_tag_dyneither(_tmp12A1,sizeof(void*),2)))))))))))));}
goto _LL598;_LL598:;};}}
# 3751
static void Cyc_Tcutil_check_field_atts(unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp9C4=(void*)atts->hd;void*_tmp9C5=_tmp9C4;_LL5A4: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp9C6=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp9C5;if(_tmp9C6->tag != 7)goto _LL5A6;}_LL5A5:
 continue;_LL5A6: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp9C7=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp9C5;if(_tmp9C7->tag != 6)goto _LL5A8;}_LL5A7:
 continue;_LL5A8:;_LL5A9: {
const char*_tmp12A7;void*_tmp12A6[2];struct Cyc_String_pa_PrintArg_struct _tmp12A5;struct Cyc_String_pa_PrintArg_struct _tmp12A4;(_tmp12A4.tag=0,((_tmp12A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp12A5.tag=0,((_tmp12A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp12A6[0]=& _tmp12A5,((_tmp12A6[1]=& _tmp12A4,Cyc_Tcutil_terr(loc,((_tmp12A7="bad attribute %s on member %s",_tag_dyneither(_tmp12A7,sizeof(char),30))),_tag_dyneither(_tmp12A6,sizeof(void*),2)))))))))))));}_LL5A3:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
# 3775
typedef struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_cvtenv_t;
# 3779
int Cyc_Tcutil_extract_const_from_typedef(unsigned int loc,int declared_const,void*t){
void*_tmp9CC=t;struct Cyc_Absyn_Typedefdecl*_tmp9CE;void*_tmp9CF;_LL5AB: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp9CD=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9CC;if(_tmp9CD->tag != 17)goto _LL5AD;else{_tmp9CE=_tmp9CD->f3;_tmp9CF=(void*)_tmp9CD->f4;}}_LL5AC:
# 3782
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp9CE))->tq).real_const  || (_tmp9CE->tq).print_const){
if(declared_const){const char*_tmp12AA;void*_tmp12A9;(_tmp12A9=0,Cyc_Tcutil_warn(loc,((_tmp12AA="extra const",_tag_dyneither(_tmp12AA,sizeof(char),12))),_tag_dyneither(_tmp12A9,sizeof(void*),0)));}
return 1;}
# 3787
if((unsigned int)_tmp9CF)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp9CF);else{
return declared_const;}_LL5AD:;_LL5AE:
 return declared_const;_LL5AA:;}
# 3794
static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td != 0){
if(td->defn != 0){
void*_tmp9D2=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_tmp9D3=_tmp9D2;void*_tmp9D5;_LL5B0: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9D4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9D3;if(_tmp9D4->tag != 5)goto _LL5B2;else{_tmp9D5=((_tmp9D4->f1).ptr_atts).rgn;}}_LL5B1:
# 3799
{void*_tmp9D6=Cyc_Tcutil_compress(_tmp9D5);void*_tmp9D7=_tmp9D6;struct Cyc_Absyn_Tvar*_tmp9D9;_LL5B5: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp9D8=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9D7;if(_tmp9D8->tag != 2)goto _LL5B7;else{_tmp9D9=_tmp9D8->f1;}}_LL5B6:
# 3801
 return Cyc_Absyn_tvar_cmp(tvar,_tmp9D9)== 0;_LL5B7:;_LL5B8:
 goto _LL5B4;_LL5B4:;}
# 3804
goto _LL5AF;_LL5B2:;_LL5B3:
 goto _LL5AF;_LL5AF:;}}else{
# 3810
return 1;}
return 0;}
# 3814
static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 3817
void*_tmp9DA=Cyc_Absyn_compress_kb(tvar->kind);void*_tmp9DB=_tmp9DA;_LL5BA: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9DC=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9DB;if(_tmp9DC->tag != 2)goto _LL5BC;else{if((_tmp9DC->f2)->kind != Cyc_Absyn_RgnKind)goto _LL5BC;if((_tmp9DC->f2)->aliasqual != Cyc_Absyn_Top)goto _LL5BC;}}_LL5BB:
 goto _LL5BD;_LL5BC: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp9DD=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp9DB;if(_tmp9DD->tag != 0)goto _LL5BE;else{if((_tmp9DD->f1)->kind != Cyc_Absyn_RgnKind)goto _LL5BE;if((_tmp9DD->f1)->aliasqual != Cyc_Absyn_Top)goto _LL5BE;}}_LL5BD:
# 3826
 if(((expected_kind->kind == Cyc_Absyn_BoxKind  || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind) && 
# 3829
Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){
if(expected_kind->aliasqual == Cyc_Absyn_Aliasable)
return& Cyc_Tcutil_rk;else{
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}
# 3835
return& Cyc_Tcutil_trk;_LL5BE:;_LL5BF:
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);_LL5B9:;}
# 3841
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);struct _tuple30{struct Cyc_Tcutil_CVTEnv f1;struct Cyc_List_List*f2;};
# 3845
static struct _tuple30 Cyc_Tcutil_check_clause(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct _dyneither_ptr clause_name,struct Cyc_Absyn_Exp*clause){
# 3848
struct Cyc_List_List*relns=0;
if(clause != 0){
Cyc_Tcexp_tcExp(te,0,clause);
if(!Cyc_Tcutil_is_integral(clause)){
const char*_tmp12AF;void*_tmp12AE[2];struct Cyc_String_pa_PrintArg_struct _tmp12AD;struct Cyc_String_pa_PrintArg_struct _tmp12AC;(_tmp12AC.tag=0,((_tmp12AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(clause->topt))),((_tmp12AD.tag=0,((_tmp12AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name),((_tmp12AE[0]=& _tmp12AD,((_tmp12AE[1]=& _tmp12AC,Cyc_Tcutil_terr(loc,((_tmp12AF="%s clause has type %s instead of integral type",_tag_dyneither(_tmp12AF,sizeof(char),47))),_tag_dyneither(_tmp12AE,sizeof(void*),2)))))))))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(clause,te,cvtenv);
relns=Cyc_Relations_exp2relns(Cyc_Core_heap_region,clause);
if(!Cyc_Relations_consistent_relations(relns)){
const char*_tmp12B4;void*_tmp12B3[2];struct Cyc_String_pa_PrintArg_struct _tmp12B2;struct Cyc_String_pa_PrintArg_struct _tmp12B1;(_tmp12B1.tag=0,((_tmp12B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(clause)),((_tmp12B2.tag=0,((_tmp12B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name),((_tmp12B3[0]=& _tmp12B2,((_tmp12B3[1]=& _tmp12B1,Cyc_Tcutil_terr(clause->loc,((_tmp12B4="%s clause '%s' may be unsatisfiable",_tag_dyneither(_tmp12B4,sizeof(char),36))),_tag_dyneither(_tmp12B3,sizeof(void*),2)))))))))))));}}{
# 3860
struct _tuple30 _tmp12B5;return(_tmp12B5.f1=cvtenv,((_tmp12B5.f2=relns,_tmp12B5)));};}struct _tuple31{enum Cyc_Absyn_Format_Type f1;void*f2;};
# 3892 "tcutil.cyc"
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect,int allow_abs_aggr){
# 3900
{void*_tmp9E7=Cyc_Tcutil_compress(t);void*_tmp9E8=_tmp9E7;struct Cyc_Core_Opt**_tmp9EB;void**_tmp9EC;struct Cyc_Absyn_Tvar*_tmp9EE;void*_tmp9F0;void***_tmp9F1;struct Cyc_List_List*_tmp9F3;struct _tuple2*_tmp9F5;struct Cyc_Absyn_Enumdecl**_tmp9F6;union Cyc_Absyn_DatatypeInfoU*_tmp9F8;struct Cyc_List_List**_tmp9F9;union Cyc_Absyn_DatatypeFieldInfoU*_tmp9FB;struct Cyc_List_List*_tmp9FC;void*_tmp9FE;struct Cyc_Absyn_Tqual*_tmp9FF;void*_tmpA00;union Cyc_Absyn_Constraint*_tmpA01;union Cyc_Absyn_Constraint*_tmpA02;union Cyc_Absyn_Constraint*_tmpA03;void*_tmpA05;struct Cyc_Absyn_Exp*_tmpA08;struct Cyc_Absyn_Exp*_tmpA0A;void*_tmpA0E;struct Cyc_Absyn_Tqual*_tmpA0F;struct Cyc_Absyn_Exp**_tmpA10;union Cyc_Absyn_Constraint*_tmpA11;unsigned int _tmpA12;struct Cyc_List_List**_tmpA14;void**_tmpA15;struct Cyc_Absyn_Tqual*_tmpA16;void*_tmpA17;struct Cyc_List_List*_tmpA18;int _tmpA19;struct Cyc_Absyn_VarargInfo*_tmpA1A;struct Cyc_List_List*_tmpA1B;struct Cyc_List_List*_tmpA1C;struct Cyc_Absyn_Exp*_tmpA1D;struct Cyc_List_List**_tmpA1E;struct Cyc_Absyn_Exp*_tmpA1F;struct Cyc_List_List**_tmpA20;struct Cyc_List_List*_tmpA22;enum Cyc_Absyn_AggrKind _tmpA24;struct Cyc_List_List*_tmpA25;union Cyc_Absyn_AggrInfoU*_tmpA27;struct Cyc_List_List**_tmpA28;struct _tuple2*_tmpA2A;struct Cyc_List_List**_tmpA2B;struct Cyc_Absyn_Typedefdecl**_tmpA2C;void**_tmpA2D;void*_tmpA32;void*_tmpA34;void*_tmpA35;void*_tmpA37;void*_tmpA39;struct Cyc_List_List*_tmpA3B;_LL5C1: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp9E9=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp9E8;if(_tmp9E9->tag != 0)goto _LL5C3;}_LL5C2:
 goto _LL5C0;_LL5C3: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp9EA=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp9E8;if(_tmp9EA->tag != 1)goto _LL5C5;else{_tmp9EB=(struct Cyc_Core_Opt**)& _tmp9EA->f1;_tmp9EC=(void**)((void**)& _tmp9EA->f2);}}_LL5C4:
# 3904
 if(*_tmp9EB == 0  || 
Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp9EB))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp9EB))->v,expected_kind))
*_tmp9EB=Cyc_Tcutil_kind_to_opt(expected_kind);
if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){
# 3909
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
*_tmp9EC=(void*)& Cyc_Absyn_UniqueRgn_val;else{
# 3912
*_tmp9EC=(void*)& Cyc_Absyn_HeapRgn_val;}}else{
if(cvtenv.generalize_evars){
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp12B8;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp12B7;struct Cyc_Absyn_Tvar*_tmpA3C=Cyc_Tcutil_new_tvar((void*)((_tmp12B7=_cycalloc(sizeof(*_tmp12B7)),((_tmp12B7[0]=((_tmp12B8.tag=2,((_tmp12B8.f1=0,((_tmp12B8.f2=expected_kind,_tmp12B8)))))),_tmp12B7)))));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp12BB;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp12BA;*_tmp9EC=(void*)((_tmp12BA=_cycalloc(sizeof(*_tmp12BA)),((_tmp12BA[0]=((_tmp12BB.tag=2,((_tmp12BB.f1=_tmpA3C,_tmp12BB)))),_tmp12BA))));}
_tmp9EE=_tmpA3C;goto _LL5C6;}else{
# 3918
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}
# 3920
goto _LL5C0;_LL5C5: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp9ED=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9E8;if(_tmp9ED->tag != 2)goto _LL5C7;else{_tmp9EE=_tmp9ED->f1;}}_LL5C6:
# 3922
{void*_tmpA41=Cyc_Absyn_compress_kb(_tmp9EE->kind);void*_tmpA42=_tmpA41;struct Cyc_Core_Opt**_tmpA44;_LL5FC: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA43=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA42;if(_tmpA43->tag != 1)goto _LL5FE;else{_tmpA44=(struct Cyc_Core_Opt**)& _tmpA43->f1;}}_LL5FD:
# 3924
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp12C1;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp12C0;struct Cyc_Core_Opt*_tmp12BF;*_tmpA44=((_tmp12BF=_cycalloc(sizeof(*_tmp12BF)),((_tmp12BF->v=(void*)((_tmp12C1=_cycalloc(sizeof(*_tmp12C1)),((_tmp12C1[0]=((_tmp12C0.tag=2,((_tmp12C0.f1=0,((_tmp12C0.f2=expected_kind,_tmp12C0)))))),_tmp12C1)))),_tmp12BF))));}goto _LL5FB;_LL5FE:;_LL5FF:
 goto _LL5FB;_LL5FB:;}
# 3929
cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp9EE);
# 3932
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp9EE,put_in_effect);
# 3934
{void*_tmpA48=Cyc_Absyn_compress_kb(_tmp9EE->kind);void*_tmpA49=_tmpA48;struct Cyc_Core_Opt**_tmpA4B;struct Cyc_Absyn_Kind*_tmpA4C;_LL601: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA4A=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA49;if(_tmpA4A->tag != 2)goto _LL603;else{_tmpA4B=(struct Cyc_Core_Opt**)& _tmpA4A->f1;_tmpA4C=_tmpA4A->f2;}}_LL602:
# 3936
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmpA4C)){
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp12C7;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp12C6;struct Cyc_Core_Opt*_tmp12C5;*_tmpA4B=((_tmp12C5=_cycalloc(sizeof(*_tmp12C5)),((_tmp12C5->v=(void*)((_tmp12C7=_cycalloc(sizeof(*_tmp12C7)),((_tmp12C7[0]=((_tmp12C6.tag=2,((_tmp12C6.f1=0,((_tmp12C6.f2=expected_kind,_tmp12C6)))))),_tmp12C7)))),_tmp12C5))));}
goto _LL600;_LL603:;_LL604:
 goto _LL600;_LL600:;}
# 3941
goto _LL5C0;_LL5C7: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp9EF=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp9E8;if(_tmp9EF->tag != 26)goto _LL5C9;else{_tmp9F0=(_tmp9EF->f1)->r;_tmp9F1=(void***)& _tmp9EF->f2;}}_LL5C8: {
# 3947
void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmpA50=_tmp9F0;struct Cyc_Absyn_Aggrdecl*_tmpA52;struct Cyc_Absyn_Enumdecl*_tmpA54;struct Cyc_Absyn_Datatypedecl*_tmpA56;_LL606: {struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmpA51=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmpA50;if(_tmpA51->tag != 0)goto _LL608;else{_tmpA52=_tmpA51->f1;}}_LL607:
# 3950
 if(te->in_extern_c_include)
_tmpA52->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmpA52);goto _LL605;_LL608: {struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpA53=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmpA50;if(_tmpA53->tag != 1)goto _LL60A;else{_tmpA54=_tmpA53->f1;}}_LL609:
# 3954
 if(te->in_extern_c_include)
_tmpA54->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmpA54);goto _LL605;_LL60A: {struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmpA55=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmpA50;if(_tmpA55->tag != 2)goto _LL605;else{_tmpA56=_tmpA55->f1;}}_LL60B:
# 3958
 Cyc_Tc_tcDatatypedecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmpA56);goto _LL605;_LL605:;}
# 3960
{void**_tmp12C8;*_tmp9F1=((_tmp12C8=_cycalloc(sizeof(*_tmp12C8)),((_tmp12C8[0]=new_t,_tmp12C8))));}
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect,allow_abs_aggr);}_LL5C9: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp9F2=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp9E8;if(_tmp9F2->tag != 14)goto _LL5CB;else{_tmp9F3=_tmp9F2->f1;}}_LL5CA: {
# 3966
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct _RegionHandle _tmpA58=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmpA58;_push_region(uprev_rgn);{
struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;
for(0;_tmp9F3 != 0;_tmp9F3=_tmp9F3->tl){
struct Cyc_Absyn_Enumfield*_tmpA59=(struct Cyc_Absyn_Enumfield*)_tmp9F3->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmpA59->name).f2)){
const char*_tmp12CC;void*_tmp12CB[1];struct Cyc_String_pa_PrintArg_struct _tmp12CA;(_tmp12CA.tag=0,((_tmp12CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpA59->name).f2),((_tmp12CB[0]=& _tmp12CA,Cyc_Tcutil_terr(_tmpA59->loc,((_tmp12CC="duplicate enum field name %s",_tag_dyneither(_tmp12CC,sizeof(char),29))),_tag_dyneither(_tmp12CB,sizeof(void*),1)))))));}else{
# 3975
struct Cyc_List_List*_tmp12CD;prev_fields=((_tmp12CD=_region_malloc(uprev_rgn,sizeof(*_tmp12CD)),((_tmp12CD->hd=(*_tmpA59->name).f2,((_tmp12CD->tl=prev_fields,_tmp12CD))))));}
# 3977
if(_tmpA59->tag == 0)
_tmpA59->tag=Cyc_Absyn_uint_exp(tag_count,_tmpA59->loc);else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmpA59->tag))){
const char*_tmp12D1;void*_tmp12D0[1];struct Cyc_String_pa_PrintArg_struct _tmp12CF;(_tmp12CF.tag=0,((_tmp12CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpA59->name).f2),((_tmp12D0[0]=& _tmp12CF,Cyc_Tcutil_terr(loc,((_tmp12D1="enum field %s: expression is not constant",_tag_dyneither(_tmp12D1,sizeof(char),42))),_tag_dyneither(_tmp12D0,sizeof(void*),1)))))));}}{
# 3982
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmpA59->tag))).f1;
tag_count=t1 + 1;
{union Cyc_Absyn_Nmspace _tmpA61=(*_tmpA59->name).f1;union Cyc_Absyn_Nmspace _tmpA62=_tmpA61;_LL60D: if((_tmpA62.Rel_n).tag != 1)goto _LL60F;_LL60E:
# 3986
(*_tmpA59->name).f1=Cyc_Absyn_Abs_n(te->ns,0);goto _LL60C;_LL60F:;_LL610:
 goto _LL60C;_LL60C:;}{
# 3989
struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp12D7;struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct _tmp12D6;struct _tuple29*_tmp12D5;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple29*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmpA59->name).f2,(
(_tmp12D5=_cycalloc(sizeof(*_tmp12D5)),((_tmp12D5->f1=(void*)((_tmp12D7=_cycalloc(sizeof(*_tmp12D7)),((_tmp12D7[0]=((_tmp12D6.tag=4,((_tmp12D6.f1=t,((_tmp12D6.f2=_tmpA59,_tmp12D6)))))),_tmp12D7)))),((_tmp12D5->f2=1,_tmp12D5)))))));};};}}
# 3993
_npop_handler(0);goto _LL5C0;
# 3967
;_pop_region(uprev_rgn);}_LL5CB: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp9F4=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp9E8;if(_tmp9F4->tag != 13)goto _LL5CD;else{_tmp9F5=_tmp9F4->f1;_tmp9F6=(struct Cyc_Absyn_Enumdecl**)& _tmp9F4->f2;}}_LL5CC:
# 3995
 if(*_tmp9F6 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp9F6))->fields == 0){
struct _handler_cons _tmpA66;_push_handler(& _tmpA66);{int _tmpA68=0;if(setjmp(_tmpA66.handler))_tmpA68=1;if(!_tmpA68){
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp9F5);
*_tmp9F6=*ed;}
# 3997
;_pop_handler();}else{void*_tmpA67=(void*)_exn_thrown;void*_tmpA6A=_tmpA67;void*_tmpA6C;_LL612: {struct Cyc_Dict_Absent_exn_struct*_tmpA6B=(struct Cyc_Dict_Absent_exn_struct*)_tmpA6A;if(_tmpA6B->tag != Cyc_Dict_Absent)goto _LL614;}_LL613: {
# 4001
struct Cyc_Tcenv_Genv*_tmpA6D=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp12D8;struct Cyc_Absyn_Enumdecl*_tmpA6E=(_tmp12D8=_cycalloc(sizeof(*_tmp12D8)),((_tmp12D8->sc=Cyc_Absyn_Extern,((_tmp12D8->name=_tmp9F5,((_tmp12D8->fields=0,_tmp12D8)))))));
Cyc_Tc_tcEnumdecl(te,_tmpA6D,loc,_tmpA6E);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp9F5);
*_tmp9F6=*ed;
goto _LL611;};}_LL614: _tmpA6C=_tmpA6A;_LL615:(void)_rethrow(_tmpA6C);_LL611:;}};}{
# 4010
struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp9F6);
# 4012
*_tmp9F5=(ed->name)[0];
goto _LL5C0;};_LL5CD: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp9F7=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp9E8;if(_tmp9F7->tag != 3)goto _LL5CF;else{_tmp9F8=(union Cyc_Absyn_DatatypeInfoU*)&(_tmp9F7->f1).datatype_info;_tmp9F9=(struct Cyc_List_List**)&(_tmp9F7->f1).targs;}}_LL5CE: {
# 4015
struct Cyc_List_List*_tmpA70=*_tmp9F9;
{union Cyc_Absyn_DatatypeInfoU _tmpA71=*_tmp9F8;union Cyc_Absyn_DatatypeInfoU _tmpA72=_tmpA71;struct _tuple2*_tmpA73;int _tmpA74;struct Cyc_Absyn_Datatypedecl*_tmpA75;_LL617: if((_tmpA72.UnknownDatatype).tag != 1)goto _LL619;_tmpA73=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpA72.UnknownDatatype).val).name;_tmpA74=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpA72.UnknownDatatype).val).is_extensible;_LL618: {
# 4018
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmpA76;_push_handler(& _tmpA76);{int _tmpA78=0;if(setjmp(_tmpA76.handler))_tmpA78=1;if(!_tmpA78){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpA73);;_pop_handler();}else{void*_tmpA77=(void*)_exn_thrown;void*_tmpA7A=_tmpA77;void*_tmpA7C;_LL61C: {struct Cyc_Dict_Absent_exn_struct*_tmpA7B=(struct Cyc_Dict_Absent_exn_struct*)_tmpA7A;if(_tmpA7B->tag != Cyc_Dict_Absent)goto _LL61E;}_LL61D: {
# 4022
struct Cyc_Tcenv_Genv*_tmpA7D=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Datatypedecl*_tmp12D9;struct Cyc_Absyn_Datatypedecl*_tmpA7E=(_tmp12D9=_cycalloc(sizeof(*_tmp12D9)),((_tmp12D9->sc=Cyc_Absyn_Extern,((_tmp12D9->name=_tmpA73,((_tmp12D9->tvs=0,((_tmp12D9->fields=0,((_tmp12D9->is_extensible=_tmpA74,_tmp12D9)))))))))));
Cyc_Tc_tcDatatypedecl(te,_tmpA7D,loc,_tmpA7E);
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpA73);
# 4027
if(_tmpA70 != 0){
{const char*_tmp12DD;void*_tmp12DC[1];struct Cyc_String_pa_PrintArg_struct _tmp12DB;(_tmp12DB.tag=0,((_tmp12DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA73)),((_tmp12DC[0]=& _tmp12DB,Cyc_Tcutil_terr(loc,((_tmp12DD="declare parameterized datatype %s before using",_tag_dyneither(_tmp12DD,sizeof(char),47))),_tag_dyneither(_tmp12DC,sizeof(void*),1)))))));}
return cvtenv;}
# 4032
goto _LL61B;}_LL61E: _tmpA7C=_tmpA7A;_LL61F:(void)_rethrow(_tmpA7C);_LL61B:;}};}
# 4038
if(_tmpA74  && !(*tudp)->is_extensible){
const char*_tmp12E1;void*_tmp12E0[1];struct Cyc_String_pa_PrintArg_struct _tmp12DF;(_tmp12DF.tag=0,((_tmp12DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA73)),((_tmp12E0[0]=& _tmp12DF,Cyc_Tcutil_terr(loc,((_tmp12E1="datatype %s was not declared @extensible",_tag_dyneither(_tmp12E1,sizeof(char),41))),_tag_dyneither(_tmp12E0,sizeof(void*),1)))))));}
*_tmp9F8=Cyc_Absyn_KnownDatatype(tudp);
_tmpA75=*tudp;goto _LL61A;}_LL619: if((_tmpA72.KnownDatatype).tag != 2)goto _LL616;_tmpA75=*((struct Cyc_Absyn_Datatypedecl**)(_tmpA72.KnownDatatype).val);_LL61A: {
# 4045
struct Cyc_List_List*tvs=_tmpA75->tvs;
for(0;_tmpA70 != 0  && tvs != 0;(_tmpA70=_tmpA70->tl,tvs=tvs->tl)){
void*t=(void*)_tmpA70->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 4051
{struct _tuple0 _tmp12E2;struct _tuple0 _tmpA86=(_tmp12E2.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp12E2.f2=t,_tmp12E2)));struct _tuple0 _tmpA87=_tmpA86;struct Cyc_Absyn_Tvar*_tmpA8A;_LL621:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA88=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA87.f1;if(_tmpA88->tag != 1)goto _LL623;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA89=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA87.f2;if(_tmpA89->tag != 2)goto _LL623;else{_tmpA8A=_tmpA89->f1;}};_LL622:
# 4053
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpA8A);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpA8A,1);
continue;_LL623:;_LL624:
 goto _LL620;_LL620:;}{
# 4058
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 4062
if(_tmpA70 != 0){
const char*_tmp12E6;void*_tmp12E5[1];struct Cyc_String_pa_PrintArg_struct _tmp12E4;(_tmp12E4.tag=0,((_tmp12E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA75->name)),((_tmp12E5[0]=& _tmp12E4,Cyc_Tcutil_terr(loc,((_tmp12E6="too many type arguments for datatype %s",_tag_dyneither(_tmp12E6,sizeof(char),40))),_tag_dyneither(_tmp12E5,sizeof(void*),1)))))));}
if(tvs != 0){
# 4067
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp12E7;hidden_ts=((_tmp12E7=_cycalloc(sizeof(*_tmp12E7)),((_tmp12E7->hd=e,((_tmp12E7->tl=hidden_ts,_tmp12E7))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1,allow_abs_aggr);}
# 4074
*_tmp9F9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp9F9,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4076
goto _LL616;}_LL616:;}
# 4078
goto _LL5C0;}_LL5CF: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp9FA=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp9E8;if(_tmp9FA->tag != 4)goto _LL5D1;else{_tmp9FB=(union Cyc_Absyn_DatatypeFieldInfoU*)&(_tmp9FA->f1).field_info;_tmp9FC=(_tmp9FA->f1).targs;}}_LL5D0:
# 4081
{union Cyc_Absyn_DatatypeFieldInfoU _tmpA90=*_tmp9FB;union Cyc_Absyn_DatatypeFieldInfoU _tmpA91=_tmpA90;struct _tuple2*_tmpA92;struct _tuple2*_tmpA93;int _tmpA94;struct Cyc_Absyn_Datatypedecl*_tmpA95;struct Cyc_Absyn_Datatypefield*_tmpA96;_LL626: if((_tmpA91.UnknownDatatypefield).tag != 1)goto _LL628;_tmpA92=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpA91.UnknownDatatypefield).val).datatype_name;_tmpA93=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpA91.UnknownDatatypefield).val).field_name;_tmpA94=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpA91.UnknownDatatypefield).val).is_extensible;_LL627: {
# 4083
struct Cyc_Absyn_Datatypedecl*tud;
struct Cyc_Absyn_Datatypefield*tuf;
{struct _handler_cons _tmpA97;_push_handler(& _tmpA97);{int _tmpA99=0;if(setjmp(_tmpA97.handler))_tmpA99=1;if(!_tmpA99){*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpA92);;_pop_handler();}else{void*_tmpA98=(void*)_exn_thrown;void*_tmpA9B=_tmpA98;void*_tmpA9D;_LL62B: {struct Cyc_Dict_Absent_exn_struct*_tmpA9C=(struct Cyc_Dict_Absent_exn_struct*)_tmpA9B;if(_tmpA9C->tag != Cyc_Dict_Absent)goto _LL62D;}_LL62C:
# 4087
{const char*_tmp12EB;void*_tmp12EA[1];struct Cyc_String_pa_PrintArg_struct _tmp12E9;(_tmp12E9.tag=0,((_tmp12E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA92)),((_tmp12EA[0]=& _tmp12E9,Cyc_Tcutil_terr(loc,((_tmp12EB="unbound datatype %s",_tag_dyneither(_tmp12EB,sizeof(char),20))),_tag_dyneither(_tmp12EA,sizeof(void*),1)))))));}
return cvtenv;_LL62D: _tmpA9D=_tmpA9B;_LL62E:(void)_rethrow(_tmpA9D);_LL62A:;}};}
# 4090
{struct _handler_cons _tmpAA1;_push_handler(& _tmpAA1);{int _tmpAA3=0;if(setjmp(_tmpAA1.handler))_tmpAA3=1;if(!_tmpAA3){
{struct _RegionHandle _tmpAA4=_new_region("r");struct _RegionHandle*r=& _tmpAA4;_push_region(r);
{void*_tmpAA5=Cyc_Tcenv_lookup_ordinary(r,te,loc,_tmpA93,0);void*_tmpAA6=_tmpAA5;struct Cyc_Absyn_Datatypedecl*_tmpAA8;struct Cyc_Absyn_Datatypefield*_tmpAA9;_LL630: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpAA7=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpAA6;if(_tmpAA7->tag != 2)goto _LL632;else{_tmpAA8=_tmpAA7->f1;_tmpAA9=_tmpAA7->f2;}}_LL631:
# 4094
 tuf=_tmpAA9;
tud=_tmpAA8;
if(_tmpA94  && !tud->is_extensible){
const char*_tmp12EF;void*_tmp12EE[1];struct Cyc_String_pa_PrintArg_struct _tmp12ED;(_tmp12ED.tag=0,((_tmp12ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA92)),((_tmp12EE[0]=& _tmp12ED,Cyc_Tcutil_terr(loc,((_tmp12EF="datatype %s was not declared @extensible",_tag_dyneither(_tmp12EF,sizeof(char),41))),_tag_dyneither(_tmp12EE,sizeof(void*),1)))))));}
goto _LL62F;_LL632:;_LL633:
{const char*_tmp12F4;void*_tmp12F3[2];struct Cyc_String_pa_PrintArg_struct _tmp12F2;struct Cyc_String_pa_PrintArg_struct _tmp12F1;(_tmp12F1.tag=0,((_tmp12F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA92)),((_tmp12F2.tag=0,((_tmp12F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA93)),((_tmp12F3[0]=& _tmp12F2,((_tmp12F3[1]=& _tmp12F1,Cyc_Tcutil_terr(loc,((_tmp12F4="unbound field %s in type datatype %s",_tag_dyneither(_tmp12F4,sizeof(char),37))),_tag_dyneither(_tmp12F3,sizeof(void*),2)))))))))))));}{
struct Cyc_Tcutil_CVTEnv _tmpAB1=cvtenv;_npop_handler(1);return _tmpAB1;};_LL62F:;}
# 4092
;_pop_region(r);}
# 4091
;_pop_handler();}else{void*_tmpAA2=(void*)_exn_thrown;void*_tmpAB3=_tmpAA2;void*_tmpAB5;_LL635: {struct Cyc_Dict_Absent_exn_struct*_tmpAB4=(struct Cyc_Dict_Absent_exn_struct*)_tmpAB3;if(_tmpAB4->tag != Cyc_Dict_Absent)goto _LL637;}_LL636:
# 4107
{const char*_tmp12F9;void*_tmp12F8[2];struct Cyc_String_pa_PrintArg_struct _tmp12F7;struct Cyc_String_pa_PrintArg_struct _tmp12F6;(_tmp12F6.tag=0,((_tmp12F6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA92)),((_tmp12F7.tag=0,((_tmp12F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA93)),((_tmp12F8[0]=& _tmp12F7,((_tmp12F8[1]=& _tmp12F6,Cyc_Tcutil_terr(loc,((_tmp12F9="unbound field %s in type datatype %s",_tag_dyneither(_tmp12F9,sizeof(char),37))),_tag_dyneither(_tmp12F8,sizeof(void*),2)))))))))))));}
return cvtenv;_LL637: _tmpAB5=_tmpAB3;_LL638:(void)_rethrow(_tmpAB5);_LL634:;}};}
# 4111
*_tmp9FB=Cyc_Absyn_KnownDatatypefield(tud,tuf);
_tmpA95=tud;_tmpA96=tuf;goto _LL629;}_LL628: if((_tmpA91.KnownDatatypefield).tag != 2)goto _LL625;_tmpA95=((struct _tuple3)(_tmpA91.KnownDatatypefield).val).f1;_tmpA96=((struct _tuple3)(_tmpA91.KnownDatatypefield).val).f2;_LL629: {
# 4115
struct Cyc_List_List*tvs=_tmpA95->tvs;
for(0;_tmp9FC != 0  && tvs != 0;(_tmp9FC=_tmp9FC->tl,tvs=tvs->tl)){
void*t=(void*)_tmp9FC->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 4121
{struct _tuple0 _tmp12FA;struct _tuple0 _tmpABA=(_tmp12FA.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp12FA.f2=t,_tmp12FA)));struct _tuple0 _tmpABB=_tmpABA;struct Cyc_Absyn_Tvar*_tmpABE;_LL63A:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpABC=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpABB.f1;if(_tmpABC->tag != 1)goto _LL63C;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpABD=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpABB.f2;if(_tmpABD->tag != 2)goto _LL63C;else{_tmpABE=_tmpABD->f1;}};_LL63B:
# 4123
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpABE);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpABE,1);
continue;_LL63C:;_LL63D:
 goto _LL639;_LL639:;}{
# 4128
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 4132
if(_tmp9FC != 0){
const char*_tmp12FF;void*_tmp12FE[2];struct Cyc_String_pa_PrintArg_struct _tmp12FD;struct Cyc_String_pa_PrintArg_struct _tmp12FC;(_tmp12FC.tag=0,((_tmp12FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA96->name)),((_tmp12FD.tag=0,((_tmp12FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA95->name)),((_tmp12FE[0]=& _tmp12FD,((_tmp12FE[1]=& _tmp12FC,Cyc_Tcutil_terr(loc,((_tmp12FF="too many type arguments for datatype %s.%s",_tag_dyneither(_tmp12FF,sizeof(char),43))),_tag_dyneither(_tmp12FE,sizeof(void*),2)))))))))))));}
if(tvs != 0){
const char*_tmp1304;void*_tmp1303[2];struct Cyc_String_pa_PrintArg_struct _tmp1302;struct Cyc_String_pa_PrintArg_struct _tmp1301;(_tmp1301.tag=0,((_tmp1301.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA96->name)),((_tmp1302.tag=0,((_tmp1302.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA95->name)),((_tmp1303[0]=& _tmp1302,((_tmp1303[1]=& _tmp1301,Cyc_Tcutil_terr(loc,((_tmp1304="too few type arguments for datatype %s.%s",_tag_dyneither(_tmp1304,sizeof(char),42))),_tag_dyneither(_tmp1303,sizeof(void*),2)))))))))))));}
goto _LL625;}_LL625:;}
# 4140
goto _LL5C0;_LL5D1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9FD=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9E8;if(_tmp9FD->tag != 5)goto _LL5D3;else{_tmp9FE=(_tmp9FD->f1).elt_typ;_tmp9FF=(struct Cyc_Absyn_Tqual*)&(_tmp9FD->f1).elt_tq;_tmpA00=((_tmp9FD->f1).ptr_atts).rgn;_tmpA01=((_tmp9FD->f1).ptr_atts).nullable;_tmpA02=((_tmp9FD->f1).ptr_atts).bounds;_tmpA03=((_tmp9FD->f1).ptr_atts).zero_term;}}_LL5D2: {
# 4143
int is_zero_terminated;
# 4145
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp9FE,1,1);
_tmp9FF->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp9FF->print_const,_tmp9FE);{
struct Cyc_Absyn_Kind*k;
{enum Cyc_Absyn_AliasQual _tmpAC8=expected_kind->aliasqual;switch(_tmpAC8){case Cyc_Absyn_Aliasable: _LL63E:
 k=& Cyc_Tcutil_rk;break;case Cyc_Absyn_Unique: _LL63F:
 k=& Cyc_Tcutil_urk;break;case Cyc_Absyn_Top: _LL640:
 k=& Cyc_Tcutil_trk;break;}}
# 4153
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmpA00,1,1);
{union Cyc_Absyn_Constraint*_tmpAC9=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmpA03);union Cyc_Absyn_Constraint*_tmpACA=_tmpAC9;_LL643: if((_tmpACA->No_constr).tag != 3)goto _LL645;_LL644:
# 4158
{void*_tmpACB=Cyc_Tcutil_compress(_tmp9FE);void*_tmpACC=_tmpACB;_LL64A: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpACD=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpACC;if(_tmpACD->tag != 6)goto _LL64C;else{if(_tmpACD->f2 != Cyc_Absyn_Char_sz)goto _LL64C;}}_LL64B:
# 4160
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpA03,Cyc_Absyn_true_conref);
is_zero_terminated=1;
goto _LL649;_LL64C:;_LL64D:
# 4164
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpA03,Cyc_Absyn_false_conref);
is_zero_terminated=0;
goto _LL649;_LL649:;}
# 4168
goto _LL642;_LL645: if((_tmpACA->Eq_constr).tag != 1)goto _LL647;if((int)(_tmpACA->Eq_constr).val != 1)goto _LL647;_LL646:
# 4171
 if(!Cyc_Tcutil_admits_zero(_tmp9FE)){
const char*_tmp1308;void*_tmp1307[1];struct Cyc_String_pa_PrintArg_struct _tmp1306;(_tmp1306.tag=0,((_tmp1306.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp9FE)),((_tmp1307[0]=& _tmp1306,Cyc_Tcutil_terr(loc,((_tmp1308="cannot have a pointer to zero-terminated %s elements",_tag_dyneither(_tmp1308,sizeof(char),53))),_tag_dyneither(_tmp1307,sizeof(void*),1)))))));}
is_zero_terminated=1;
goto _LL642;_LL647:;_LL648:
# 4177
 is_zero_terminated=0;
goto _LL642;_LL642:;}
# 4181
{void*_tmpAD1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmpA02);void*_tmpAD2=_tmpAD1;struct Cyc_Absyn_Exp*_tmpAD5;_LL64F: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpAD3=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpAD2;if(_tmpAD3->tag != 0)goto _LL651;}_LL650:
 goto _LL64E;_LL651: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpAD4=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpAD2;if(_tmpAD4->tag != 1)goto _LL64E;else{_tmpAD5=_tmpAD4->f1;}}_LL652: {
# 4184
struct _RegionHandle _tmpAD6=_new_region("temp");struct _RegionHandle*temp=& _tmpAD6;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmpAD7=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmpAD7,0,_tmpAD5);}
# 4188
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpAD5,te,cvtenv);
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpAD5)){
const char*_tmp130B;void*_tmp130A;(_tmp130A=0,Cyc_Tcutil_terr(loc,((_tmp130B="pointer bounds expression is not an unsigned int",_tag_dyneither(_tmp130B,sizeof(char),49))),_tag_dyneither(_tmp130A,sizeof(void*),0)));}{
struct _tuple14 _tmpADA=Cyc_Evexp_eval_const_uint_exp(_tmpAD5);unsigned int _tmpADC;int _tmpADD;struct _tuple14 _tmpADB=_tmpADA;_tmpADC=_tmpADB.f1;_tmpADD=_tmpADB.f2;
if(is_zero_terminated  && (!_tmpADD  || _tmpADC < 1)){
const char*_tmp130E;void*_tmp130D;(_tmp130D=0,Cyc_Tcutil_terr(loc,((_tmp130E="zero-terminated pointer cannot point to empty sequence",_tag_dyneither(_tmp130E,sizeof(char),55))),_tag_dyneither(_tmp130D,sizeof(void*),0)));}
_npop_handler(0);goto _LL64E;};
# 4184
;_pop_region(temp);}_LL64E:;}
# 4196
goto _LL5C0;};}_LL5D3: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpA04=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp9E8;if(_tmpA04->tag != 19)goto _LL5D5;else{_tmpA05=(void*)_tmpA04->f1;}}_LL5D4:
# 4198
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpA05,1,1);goto _LL5C0;_LL5D5: {struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*_tmpA06=(struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp9E8;if(_tmpA06->tag != 28)goto _LL5D7;}_LL5D6:
 goto _LL5C0;_LL5D7: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpA07=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp9E8;if(_tmpA07->tag != 18)goto _LL5D9;else{_tmpA08=_tmpA07->f1;}}_LL5D8: {
# 4204
struct _RegionHandle _tmpAE0=_new_region("temp");struct _RegionHandle*temp=& _tmpAE0;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmpAE1=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmpAE1,0,_tmpA08);}
# 4208
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpA08)){
const char*_tmp1311;void*_tmp1310;(_tmp1310=0,Cyc_Tcutil_terr(loc,((_tmp1311="valueof_t requires an int expression",_tag_dyneither(_tmp1311,sizeof(char),37))),_tag_dyneither(_tmp1310,sizeof(void*),0)));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpA08,te,cvtenv);
_npop_handler(0);goto _LL5C0;
# 4204
;_pop_region(temp);}_LL5D9: {struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmpA09=(struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp9E8;if(_tmpA09->tag != 27)goto _LL5DB;else{_tmpA0A=_tmpA09->f1;}}_LL5DA: {
# 4216
struct _RegionHandle _tmpAE4=_new_region("temp");struct _RegionHandle*temp=& _tmpAE4;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmpAE5=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmpAE5,0,_tmpA0A);}
# 4220
_npop_handler(0);goto _LL5C0;
# 4216
;_pop_region(temp);}_LL5DB: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpA0B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp9E8;if(_tmpA0B->tag != 6)goto _LL5DD;}_LL5DC:
# 4221
 goto _LL5DE;_LL5DD: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpA0C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp9E8;if(_tmpA0C->tag != 7)goto _LL5DF;}_LL5DE:
 goto _LL5C0;_LL5DF: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpA0D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9E8;if(_tmpA0D->tag != 8)goto _LL5E1;else{_tmpA0E=(_tmpA0D->f1).elt_type;_tmpA0F=(struct Cyc_Absyn_Tqual*)&(_tmpA0D->f1).tq;_tmpA10=(struct Cyc_Absyn_Exp**)&(_tmpA0D->f1).num_elts;_tmpA11=(_tmpA0D->f1).zero_term;_tmpA12=(_tmpA0D->f1).zt_loc;}}_LL5E0: {
# 4226
struct Cyc_Absyn_Exp*_tmpAE6=*_tmpA10;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpA0E,1,allow_abs_aggr);
_tmpA0F->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpA0F->print_const,_tmpA0E);{
int is_zero_terminated;
{union Cyc_Absyn_Constraint*_tmpAE7=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmpA11);union Cyc_Absyn_Constraint*_tmpAE8=_tmpAE7;_LL654: if((_tmpAE8->No_constr).tag != 3)goto _LL656;_LL655:
# 4233
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpA11,Cyc_Absyn_false_conref);
is_zero_terminated=0;
# 4248 "tcutil.cyc"
goto _LL653;_LL656: if((_tmpAE8->Eq_constr).tag != 1)goto _LL658;if((int)(_tmpAE8->Eq_constr).val != 1)goto _LL658;_LL657:
# 4251
 if(!Cyc_Tcutil_admits_zero(_tmpA0E)){
const char*_tmp1315;void*_tmp1314[1];struct Cyc_String_pa_PrintArg_struct _tmp1313;(_tmp1313.tag=0,((_tmp1313.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmpA0E)),((_tmp1314[0]=& _tmp1313,Cyc_Tcutil_terr(loc,((_tmp1315="cannot have a zero-terminated array of %s elements",_tag_dyneither(_tmp1315,sizeof(char),51))),_tag_dyneither(_tmp1314,sizeof(void*),1)))))));}
is_zero_terminated=1;
goto _LL653;_LL658:;_LL659:
# 4257
 is_zero_terminated=0;
goto _LL653;_LL653:;}
# 4262
if(_tmpAE6 == 0){
# 4264
if(is_zero_terminated)
# 4266
*_tmpA10=(_tmpAE6=Cyc_Absyn_uint_exp(1,0));else{
# 4269
{const char*_tmp1318;void*_tmp1317;(_tmp1317=0,Cyc_Tcutil_warn(loc,((_tmp1318="array bound defaults to 1 here",_tag_dyneither(_tmp1318,sizeof(char),31))),_tag_dyneither(_tmp1317,sizeof(void*),0)));}
*_tmpA10=(_tmpAE6=Cyc_Absyn_uint_exp(1,0));}}{
# 4273
struct _RegionHandle _tmpAEE=_new_region("temp");struct _RegionHandle*temp=& _tmpAEE;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmpAEF=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmpAEF,0,_tmpAE6);}
# 4277
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpAE6)){
const char*_tmp131B;void*_tmp131A;(_tmp131A=0,Cyc_Tcutil_terr(loc,((_tmp131B="array bounds expression is not an unsigned int",_tag_dyneither(_tmp131B,sizeof(char),47))),_tag_dyneither(_tmp131A,sizeof(void*),0)));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpAE6,te,cvtenv);{
# 4284
struct _tuple14 _tmpAF2=Cyc_Evexp_eval_const_uint_exp(_tmpAE6);unsigned int _tmpAF4;int _tmpAF5;struct _tuple14 _tmpAF3=_tmpAF2;_tmpAF4=_tmpAF3.f1;_tmpAF5=_tmpAF3.f2;
# 4286
if((is_zero_terminated  && _tmpAF5) && _tmpAF4 < 1){
const char*_tmp131E;void*_tmp131D;(_tmp131D=0,Cyc_Tcutil_warn(loc,((_tmp131E="zero terminated array cannot have zero elements",_tag_dyneither(_tmp131E,sizeof(char),48))),_tag_dyneither(_tmp131D,sizeof(void*),0)));}
# 4289
if((_tmpAF5  && _tmpAF4 < 1) && Cyc_Cyclone_tovc_r){
{const char*_tmp1321;void*_tmp1320;(_tmp1320=0,Cyc_Tcutil_warn(loc,((_tmp1321="arrays with 0 elements are not supported except with gcc -- changing to 1.",_tag_dyneither(_tmp1321,sizeof(char),75))),_tag_dyneither(_tmp1320,sizeof(void*),0)));}
*_tmpA10=Cyc_Absyn_uint_exp(1,0);}
# 4293
_npop_handler(0);goto _LL5C0;};
# 4273
;_pop_region(temp);};};}_LL5E1: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpA13=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9E8;if(_tmpA13->tag != 9)goto _LL5E3;else{_tmpA14=(struct Cyc_List_List**)&(_tmpA13->f1).tvars;_tmpA15=(void**)&(_tmpA13->f1).effect;_tmpA16=(struct Cyc_Absyn_Tqual*)&(_tmpA13->f1).ret_tqual;_tmpA17=(_tmpA13->f1).ret_typ;_tmpA18=(_tmpA13->f1).args;_tmpA19=(_tmpA13->f1).c_varargs;_tmpA1A=(_tmpA13->f1).cyc_varargs;_tmpA1B=(_tmpA13->f1).rgn_po;_tmpA1C=(_tmpA13->f1).attributes;_tmpA1D=(_tmpA13->f1).requires_clause;_tmpA1E=(struct Cyc_List_List**)&(_tmpA13->f1).requires_relns;_tmpA1F=(_tmpA13->f1).ensures_clause;_tmpA20=(struct Cyc_List_List**)&(_tmpA13->f1).ensures_relns;}}_LL5E2: {
# 4300
int num_convs=0;
int seen_cdecl=0;
int seen_stdcall=0;
int seen_fastcall=0;
int seen_format=0;
enum Cyc_Absyn_Format_Type ft=Cyc_Absyn_Printf_ft;
int fmt_desc_arg=-1;
int fmt_arg_start=-1;
for(0;_tmpA1C != 0;_tmpA1C=_tmpA1C->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmpA1C->hd)){
const char*_tmp1325;void*_tmp1324[1];struct Cyc_String_pa_PrintArg_struct _tmp1323;(_tmp1323.tag=0,((_tmp1323.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmpA1C->hd)),((_tmp1324[0]=& _tmp1323,Cyc_Tcutil_terr(loc,((_tmp1325="bad function type attribute %s",_tag_dyneither(_tmp1325,sizeof(char),31))),_tag_dyneither(_tmp1324,sizeof(void*),1)))))));}{
void*_tmpAFD=(void*)_tmpA1C->hd;void*_tmpAFE=_tmpAFD;enum Cyc_Absyn_Format_Type _tmpB03;int _tmpB04;int _tmpB05;_LL65B: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpAFF=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpAFE;if(_tmpAFF->tag != 1)goto _LL65D;}_LL65C:
# 4313
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL65A;_LL65D: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpB00=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpAFE;if(_tmpB00->tag != 2)goto _LL65F;}_LL65E:
# 4315
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL65A;_LL65F: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpB01=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpAFE;if(_tmpB01->tag != 3)goto _LL661;}_LL660:
# 4317
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL65A;_LL661: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpB02=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpAFE;if(_tmpB02->tag != 19)goto _LL663;else{_tmpB03=_tmpB02->f1;_tmpB04=_tmpB02->f2;_tmpB05=_tmpB02->f3;}}_LL662:
# 4319
 if(!seen_format){
seen_format=1;
ft=_tmpB03;
fmt_desc_arg=_tmpB04;
fmt_arg_start=_tmpB05;}else{
# 4325
const char*_tmp1328;void*_tmp1327;(_tmp1327=0,Cyc_Tcutil_terr(loc,((_tmp1328="function can't have multiple format attributes",_tag_dyneither(_tmp1328,sizeof(char),47))),_tag_dyneither(_tmp1327,sizeof(void*),0)));}
goto _LL65A;_LL663:;_LL664:
 goto _LL65A;_LL65A:;};}
# 4330
if(num_convs > 1){
const char*_tmp132B;void*_tmp132A;(_tmp132A=0,Cyc_Tcutil_terr(loc,((_tmp132B="function can't have multiple calling conventions",_tag_dyneither(_tmp132B,sizeof(char),49))),_tag_dyneither(_tmp132A,sizeof(void*),0)));}
# 4335
Cyc_Tcutil_check_unique_tvars(loc,*_tmpA14);
{struct Cyc_List_List*b=*_tmpA14;for(0;b != 0;b=b->tl){
((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{
void*_tmpB0A=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmpB0B=_tmpB0A;_LL666: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpB0C=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpB0B;if(_tmpB0C->tag != 0)goto _LL668;else{if((_tmpB0C->f1)->kind != Cyc_Absyn_MemKind)goto _LL668;}}_LL667:
# 4341
{const char*_tmp132F;void*_tmp132E[1];struct Cyc_String_pa_PrintArg_struct _tmp132D;(_tmp132D.tag=0,((_tmp132D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name),((_tmp132E[0]=& _tmp132D,Cyc_Tcutil_terr(loc,((_tmp132F="function attempts to abstract Mem type variable %s",_tag_dyneither(_tmp132F,sizeof(char),51))),_tag_dyneither(_tmp132E,sizeof(void*),1)))))));}
goto _LL665;_LL668:;_LL669:
 goto _LL665;_LL665:;};}}{
# 4349
struct _RegionHandle _tmpB10=_new_region("newr");struct _RegionHandle*newr=& _tmpB10;_push_region(newr);{
struct Cyc_Tcutil_CVTEnv _tmp1330;struct Cyc_Tcutil_CVTEnv _tmpB11=
(_tmp1330.r=newr,((_tmp1330.kind_env=cvtenv.kind_env,((_tmp1330.free_vars=0,((_tmp1330.free_evars=0,((_tmp1330.generalize_evars=cvtenv.generalize_evars,((_tmp1330.fn_result=1,_tmp1330)))))))))));
# 4355
_tmpB11=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpB11,& Cyc_Tcutil_tmk,_tmpA17,1,1);
_tmpA16->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpA16->print_const,_tmpA17);
_tmpB11.fn_result=0;
{struct Cyc_List_List*a=_tmpA18;for(0;a != 0;a=a->tl){
struct _tuple10*_tmpB12=(struct _tuple10*)a->hd;
void*_tmpB13=(*_tmpB12).f3;
_tmpB11=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpB11,& Cyc_Tcutil_tmk,_tmpB13,1,1);{
int _tmpB14=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpB12).f2).print_const,_tmpB13);
((*_tmpB12).f2).real_const=_tmpB14;
# 4366
if(Cyc_Tcutil_is_array(_tmpB13)){
# 4369
void*_tmpB15=Cyc_Absyn_new_evar(0,0);
_tmpB11=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpB11,& Cyc_Tcutil_rk,_tmpB15,1,1);
(*_tmpB12).f3=Cyc_Tcutil_promote_array(_tmpB13,_tmpB15,0);}};}}
# 4376
if(_tmpA1A != 0){
if(_tmpA19){const char*_tmp1333;void*_tmp1332;(_tmp1332=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1333="both c_vararg and cyc_vararg",_tag_dyneither(_tmp1333,sizeof(char),29))),_tag_dyneither(_tmp1332,sizeof(void*),0)));}{
struct Cyc_Absyn_VarargInfo _tmpB18=*_tmpA1A;void*_tmpB1A;int _tmpB1B;struct Cyc_Absyn_VarargInfo _tmpB19=_tmpB18;_tmpB1A=_tmpB19.type;_tmpB1B=_tmpB19.inject;
_tmpB11=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpB11,& Cyc_Tcutil_tmk,_tmpB1A,1,1);
(_tmpA1A->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmpA1A->tq).print_const,_tmpB1A);
# 4382
if(_tmpB1B){
void*_tmpB1C=Cyc_Tcutil_compress(_tmpB1A);void*_tmpB1D=_tmpB1C;void*_tmpB1F;union Cyc_Absyn_Constraint*_tmpB20;union Cyc_Absyn_Constraint*_tmpB21;_LL66B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpB1E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB1D;if(_tmpB1E->tag != 5)goto _LL66D;else{_tmpB1F=(_tmpB1E->f1).elt_typ;_tmpB20=((_tmpB1E->f1).ptr_atts).bounds;_tmpB21=((_tmpB1E->f1).ptr_atts).zero_term;}}_LL66C:
# 4385
{void*_tmpB22=Cyc_Tcutil_compress(_tmpB1F);void*_tmpB23=_tmpB22;_LL670: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpB24=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpB23;if(_tmpB24->tag != 3)goto _LL672;}_LL671:
# 4387
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmpB21)){
const char*_tmp1336;void*_tmp1335;(_tmp1335=0,Cyc_Tcutil_terr(loc,((_tmp1336="can't inject into a zeroterm pointer",_tag_dyneither(_tmp1336,sizeof(char),37))),_tag_dyneither(_tmp1335,sizeof(void*),0)));}
{void*_tmpB27=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmpB20);void*_tmpB28=_tmpB27;_LL675: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpB29=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpB28;if(_tmpB29->tag != 0)goto _LL677;}_LL676:
# 4391
{const char*_tmp1339;void*_tmp1338;(_tmp1338=0,Cyc_Tcutil_terr(loc,((_tmp1339="can't inject into a fat pointer to datatype",_tag_dyneither(_tmp1339,sizeof(char),44))),_tag_dyneither(_tmp1338,sizeof(void*),0)));}
goto _LL674;_LL677:;_LL678:
 goto _LL674;_LL674:;}
# 4395
goto _LL66F;_LL672:;_LL673:
{const char*_tmp133C;void*_tmp133B;(_tmp133B=0,Cyc_Tcutil_terr(loc,((_tmp133C="can't inject a non-datatype type",_tag_dyneither(_tmp133C,sizeof(char),33))),_tag_dyneither(_tmp133B,sizeof(void*),0)));}goto _LL66F;_LL66F:;}
# 4398
goto _LL66A;_LL66D:;_LL66E:
{const char*_tmp133F;void*_tmp133E;(_tmp133E=0,Cyc_Tcutil_terr(loc,((_tmp133F="expecting a datatype pointer type",_tag_dyneither(_tmp133F,sizeof(char),34))),_tag_dyneither(_tmp133E,sizeof(void*),0)));}goto _LL66A;_LL66A:;}};}
# 4404
if(seen_format){
int _tmpB30=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpA18);
if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmpB30) || fmt_arg_start < 0) || 
# 4408
(_tmpA1A == 0  && !_tmpA19) && fmt_arg_start != 0) || 
(_tmpA1A != 0  || _tmpA19) && fmt_arg_start != _tmpB30 + 1){
# 4411
const char*_tmp1342;void*_tmp1341;(_tmp1341=0,Cyc_Tcutil_terr(loc,((_tmp1342="bad format descriptor",_tag_dyneither(_tmp1342,sizeof(char),22))),_tag_dyneither(_tmp1341,sizeof(void*),0)));}else{
# 4414
struct _tuple10 _tmpB33=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmpA18,fmt_desc_arg - 1);void*_tmpB35;struct _tuple10 _tmpB34=_tmpB33;_tmpB35=_tmpB34.f3;
# 4416
{void*_tmpB36=Cyc_Tcutil_compress(_tmpB35);void*_tmpB37=_tmpB36;void*_tmpB39;union Cyc_Absyn_Constraint*_tmpB3A;union Cyc_Absyn_Constraint*_tmpB3B;_LL67A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpB38=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB37;if(_tmpB38->tag != 5)goto _LL67C;else{_tmpB39=(_tmpB38->f1).elt_typ;_tmpB3A=((_tmpB38->f1).ptr_atts).bounds;_tmpB3B=((_tmpB38->f1).ptr_atts).zero_term;}}_LL67B:
# 4419
{struct _tuple0 _tmp1343;struct _tuple0 _tmpB3C=(_tmp1343.f1=Cyc_Tcutil_compress(_tmpB39),((_tmp1343.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpB3A),_tmp1343)));struct _tuple0 _tmpB3D=_tmpB3C;_LL67F:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpB3E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpB3D.f1;if(_tmpB3E->tag != 6)goto _LL681;else{if(_tmpB3E->f2 != Cyc_Absyn_Char_sz)goto _LL681;}}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpB3F=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpB3D.f2;if(_tmpB3F->tag != 0)goto _LL681;};_LL680:
# 4421
 if(_tmpA19){
const char*_tmp1346;void*_tmp1345;(_tmp1345=0,Cyc_Tcutil_terr(loc,((_tmp1346="format descriptor is not a char * type",_tag_dyneither(_tmp1346,sizeof(char),39))),_tag_dyneither(_tmp1345,sizeof(void*),0)));}
goto _LL67E;_LL681: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpB40=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpB3D.f1;if(_tmpB40->tag != 6)goto _LL683;else{if(_tmpB40->f2 != Cyc_Absyn_Char_sz)goto _LL683;}}_LL682:
# 4425
 if(!_tmpA19){
const char*_tmp1349;void*_tmp1348;(_tmp1348=0,Cyc_Tcutil_terr(loc,((_tmp1349="format descriptor is not a char ? type",_tag_dyneither(_tmp1349,sizeof(char),39))),_tag_dyneither(_tmp1348,sizeof(void*),0)));}
goto _LL67E;_LL683:;_LL684:
# 4429
{const char*_tmp134C;void*_tmp134B;(_tmp134B=0,Cyc_Tcutil_terr(loc,((_tmp134C="format descriptor is not a string type",_tag_dyneither(_tmp134C,sizeof(char),39))),_tag_dyneither(_tmp134B,sizeof(void*),0)));}
goto _LL67E;_LL67E:;}
# 4432
goto _LL679;_LL67C:;_LL67D:
{const char*_tmp134F;void*_tmp134E;(_tmp134E=0,Cyc_Tcutil_terr(loc,((_tmp134F="format descriptor is not a string type",_tag_dyneither(_tmp134F,sizeof(char),39))),_tag_dyneither(_tmp134E,sizeof(void*),0)));}goto _LL679;_LL679:;}
# 4435
if(fmt_arg_start != 0  && !_tmpA19){
# 4439
int problem;
{struct _tuple31 _tmp1350;struct _tuple31 _tmpB4A=(_tmp1350.f1=ft,((_tmp1350.f2=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmpA1A))->type)),_tmp1350)));struct _tuple31 _tmpB4B=_tmpB4A;struct Cyc_Absyn_Datatypedecl*_tmpB4D;struct Cyc_Absyn_Datatypedecl*_tmpB4F;_LL686: if(_tmpB4B.f1 != Cyc_Absyn_Printf_ft)goto _LL688;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpB4C=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpB4B.f2;if(_tmpB4C->tag != 3)goto _LL688;else{if((((_tmpB4C->f1).datatype_info).KnownDatatype).tag != 2)goto _LL688;_tmpB4D=*((struct Cyc_Absyn_Datatypedecl**)(((_tmpB4C->f1).datatype_info).KnownDatatype).val);}};_LL687:
# 4442
 problem=Cyc_Absyn_qvar_cmp(_tmpB4D->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL685;_LL688: if(_tmpB4B.f1 != Cyc_Absyn_Scanf_ft)goto _LL68A;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpB4E=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpB4B.f2;if(_tmpB4E->tag != 3)goto _LL68A;else{if((((_tmpB4E->f1).datatype_info).KnownDatatype).tag != 2)goto _LL68A;_tmpB4F=*((struct Cyc_Absyn_Datatypedecl**)(((_tmpB4E->f1).datatype_info).KnownDatatype).val);}};_LL689:
# 4444
 problem=Cyc_Absyn_qvar_cmp(_tmpB4F->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL685;_LL68A:;_LL68B:
# 4446
 problem=1;goto _LL685;_LL685:;}
# 4448
if(problem){
const char*_tmp1353;void*_tmp1352;(_tmp1352=0,Cyc_Tcutil_terr(loc,((_tmp1353="format attribute and vararg types don't match",_tag_dyneither(_tmp1353,sizeof(char),46))),_tag_dyneither(_tmp1352,sizeof(void*),0)));}}}}
# 4456
{struct Cyc_List_List*rpo=_tmpA1B;for(0;rpo != 0;rpo=rpo->tl){
struct _tuple0*_tmpB53=(struct _tuple0*)rpo->hd;void*_tmpB55;void*_tmpB56;struct _tuple0*_tmpB54=_tmpB53;_tmpB55=_tmpB54->f1;_tmpB56=_tmpB54->f2;
_tmpB11=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpB11,& Cyc_Tcutil_ek,_tmpB55,1,1);
_tmpB11=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpB11,& Cyc_Tcutil_trk,_tmpB56,1,1);}}{
# 4464
struct Cyc_Tcenv_Fenv*_tmpB57=Cyc_Tcenv_bogus_fenv(newr,_tmpA17,_tmpA18);
struct Cyc_Tcenv_Tenv*_tmp1354;struct Cyc_Tcenv_Tenv*_tmpB58=(_tmp1354=_region_malloc(newr,sizeof(*_tmp1354)),((_tmp1354->ns=te->ns,((_tmp1354->ae=te->ae,((_tmp1354->le=_tmpB57,((_tmp1354->allow_valueof=1,((_tmp1354->in_extern_c_include=te->in_extern_c_include,_tmp1354)))))))))));
const char*_tmp1355;struct _tuple30 _tmpB59=Cyc_Tcutil_check_clause(loc,_tmpB58,_tmpB11,((_tmp1355="@requires",_tag_dyneither(_tmp1355,sizeof(char),10))),_tmpA1D);struct Cyc_Tcutil_CVTEnv _tmpB5B;struct Cyc_List_List*_tmpB5C;struct _tuple30 _tmpB5A=_tmpB59;_tmpB5B=_tmpB5A.f1;_tmpB5C=_tmpB5A.f2;
_tmpB11=_tmpB5B;
*_tmpA1E=_tmpB5C;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmpA18,_tmpB5C);{
# 4471
struct _dyneither_ptr*_tmpB5D=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"return_value",sizeof(char),13);
struct _tuple2*_tmp1358;struct Cyc_Absyn_Vardecl*_tmp1357;struct Cyc_Absyn_Vardecl*_tmpB5E=(_tmp1357=_cycalloc(sizeof(*_tmp1357)),((_tmp1357->sc=Cyc_Absyn_Public,((_tmp1357->name=((_tmp1358=_cycalloc(sizeof(*_tmp1358)),((_tmp1358->f1=Cyc_Absyn_Loc_n,((_tmp1358->f2=_tmpB5D,_tmp1358)))))),((_tmp1357->tq=
Cyc_Absyn_empty_tqual(0),((_tmp1357->type=_tmpA17,((_tmp1357->initializer=0,((_tmp1357->rgn=
Cyc_Tcenv_curr_rgn(_tmpB58),((_tmp1357->attributes=0,((_tmp1357->escapes=0,_tmp1357)))))))))))))))));
# 4476
_tmpB58=Cyc_Tcenv_add_local_var(newr,0,_tmpB58,_tmpB5E);{
const char*_tmp1359;struct _tuple30 _tmpB5F=Cyc_Tcutil_check_clause(loc,_tmpB58,_tmpB11,((_tmp1359="@ensures",_tag_dyneither(_tmp1359,sizeof(char),9))),_tmpA1F);struct Cyc_Tcutil_CVTEnv _tmpB61;struct Cyc_List_List*_tmpB62;struct _tuple30 _tmpB60=_tmpB5F;_tmpB61=_tmpB60.f1;_tmpB62=_tmpB60.f2;
_tmpB11=_tmpB61;
*_tmpA20=_tmpB62;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmpA18,_tmpB62);
# 4482
if(*_tmpA15 != 0)
_tmpB11=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpB11,& Cyc_Tcutil_ek,(void*)_check_null(*_tmpA15),1,1);else{
# 4485
struct Cyc_List_List*effect=0;
# 4490
{struct Cyc_List_List*tvs=_tmpB11.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmpB63=*((struct _tuple28*)tvs->hd);struct Cyc_Absyn_Tvar*_tmpB65;int _tmpB66;struct _tuple28 _tmpB64=_tmpB63;_tmpB65=_tmpB64.f1;_tmpB66=_tmpB64.f2;
if(!_tmpB66)continue;{
void*_tmpB67=Cyc_Absyn_compress_kb(_tmpB65->kind);void*_tmpB68=_tmpB67;struct Cyc_Core_Opt**_tmpB6A;struct Cyc_Absyn_Kind*_tmpB6B;struct Cyc_Absyn_Kind*_tmpB6D;struct Cyc_Core_Opt**_tmpB71;struct Cyc_Core_Opt**_tmpB74;_LL68D:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB69=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB68;if(_tmpB69->tag != 2)goto _LL68F;else{_tmpB6A=(struct Cyc_Core_Opt**)& _tmpB69->f1;_tmpB6B=_tmpB69->f2;}}if(!(_tmpB6B->kind == Cyc_Absyn_RgnKind))goto _LL68F;_LL68E:
# 4496
 if(_tmpB6B->aliasqual == Cyc_Absyn_Top){
*_tmpB6A=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);_tmpB6D=_tmpB6B;goto _LL690;}
# 4499
*_tmpB6A=Cyc_Tcutil_kind_to_bound_opt(_tmpB6B);_tmpB6D=_tmpB6B;goto _LL690;_LL68F:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpB6C=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpB68;if(_tmpB6C->tag != 0)goto _LL691;else{_tmpB6D=_tmpB6C->f1;}}if(!(_tmpB6D->kind == Cyc_Absyn_RgnKind))goto _LL691;_LL690:
# 4501
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1368;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1367;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1366;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1365;struct Cyc_List_List*_tmp1364;effect=((_tmp1364=_cycalloc(sizeof(*_tmp1364)),((_tmp1364->hd=(void*)((_tmp1368=_cycalloc(sizeof(*_tmp1368)),((_tmp1368[0]=((_tmp1365.tag=23,((_tmp1365.f1=(void*)((_tmp1367=_cycalloc(sizeof(*_tmp1367)),((_tmp1367[0]=((_tmp1366.tag=2,((_tmp1366.f1=_tmpB65,_tmp1366)))),_tmp1367)))),_tmp1365)))),_tmp1368)))),((_tmp1364->tl=effect,_tmp1364))))));}goto _LL68C;_LL691: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB6E=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB68;if(_tmpB6E->tag != 2)goto _LL693;else{if((_tmpB6E->f2)->kind != Cyc_Absyn_IntKind)goto _LL693;}}_LL692:
 goto _LL694;_LL693: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpB6F=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpB68;if(_tmpB6F->tag != 0)goto _LL695;else{if((_tmpB6F->f1)->kind != Cyc_Absyn_IntKind)goto _LL695;}}_LL694:
 goto _LL68C;_LL695: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB70=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB68;if(_tmpB70->tag != 2)goto _LL697;else{_tmpB71=(struct Cyc_Core_Opt**)& _tmpB70->f1;if((_tmpB70->f2)->kind != Cyc_Absyn_EffKind)goto _LL697;}}_LL696:
# 4505
*_tmpB71=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);goto _LL698;_LL697: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpB72=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpB68;if(_tmpB72->tag != 0)goto _LL699;else{if((_tmpB72->f1)->kind != Cyc_Absyn_EffKind)goto _LL699;}}_LL698:
# 4507
{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp136E;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp136D;struct Cyc_List_List*_tmp136C;effect=((_tmp136C=_cycalloc(sizeof(*_tmp136C)),((_tmp136C->hd=(void*)((_tmp136E=_cycalloc(sizeof(*_tmp136E)),((_tmp136E[0]=((_tmp136D.tag=2,((_tmp136D.f1=_tmpB65,_tmp136D)))),_tmp136E)))),((_tmp136C->tl=effect,_tmp136C))))));}goto _LL68C;_LL699: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpB73=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpB68;if(_tmpB73->tag != 1)goto _LL69B;else{_tmpB74=(struct Cyc_Core_Opt**)& _tmpB73->f1;}}_LL69A:
# 4509
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1374;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1373;struct Cyc_Core_Opt*_tmp1372;*_tmpB74=((_tmp1372=_cycalloc(sizeof(*_tmp1372)),((_tmp1372->v=(void*)((_tmp1374=_cycalloc(sizeof(*_tmp1374)),((_tmp1374[0]=((_tmp1373.tag=2,((_tmp1373.f1=0,((_tmp1373.f2=& Cyc_Tcutil_ak,_tmp1373)))))),_tmp1374)))),_tmp1372))));}goto _LL69C;_LL69B:;_LL69C:
# 4512
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1383;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1382;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1381;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp1380;struct Cyc_List_List*_tmp137F;effect=((_tmp137F=_cycalloc(sizeof(*_tmp137F)),((_tmp137F->hd=(void*)((_tmp1383=_cycalloc(sizeof(*_tmp1383)),((_tmp1383[0]=((_tmp1380.tag=25,((_tmp1380.f1=(void*)((_tmp1382=_cycalloc(sizeof(*_tmp1382)),((_tmp1382[0]=((_tmp1381.tag=2,((_tmp1381.f1=_tmpB65,_tmp1381)))),_tmp1382)))),_tmp1380)))),_tmp1383)))),((_tmp137F->tl=effect,_tmp137F))))));}goto _LL68C;_LL68C:;};}}
# 4516
{struct Cyc_List_List*ts=_tmpB11.free_evars;for(0;ts != 0;ts=ts->tl){
struct _tuple29 _tmpB85=*((struct _tuple29*)ts->hd);void*_tmpB87;int _tmpB88;struct _tuple29 _tmpB86=_tmpB85;_tmpB87=_tmpB86.f1;_tmpB88=_tmpB86.f2;
if(!_tmpB88)continue;{
struct Cyc_Absyn_Kind*_tmpB89=Cyc_Tcutil_typ_kind(_tmpB87);struct Cyc_Absyn_Kind*_tmpB8A=_tmpB89;_LL69E: if(_tmpB8A->kind != Cyc_Absyn_RgnKind)goto _LL6A0;_LL69F:
# 4521
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1389;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1388;struct Cyc_List_List*_tmp1387;effect=((_tmp1387=_cycalloc(sizeof(*_tmp1387)),((_tmp1387->hd=(void*)((_tmp1389=_cycalloc(sizeof(*_tmp1389)),((_tmp1389[0]=((_tmp1388.tag=23,((_tmp1388.f1=_tmpB87,_tmp1388)))),_tmp1389)))),((_tmp1387->tl=effect,_tmp1387))))));}goto _LL69D;_LL6A0: if(_tmpB8A->kind != Cyc_Absyn_EffKind)goto _LL6A2;_LL6A1:
# 4523
{struct Cyc_List_List*_tmp138A;effect=((_tmp138A=_cycalloc(sizeof(*_tmp138A)),((_tmp138A->hd=_tmpB87,((_tmp138A->tl=effect,_tmp138A))))));}goto _LL69D;_LL6A2: if(_tmpB8A->kind != Cyc_Absyn_IntKind)goto _LL6A4;_LL6A3:
 goto _LL69D;_LL6A4:;_LL6A5:
# 4526
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1390;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp138F;struct Cyc_List_List*_tmp138E;effect=((_tmp138E=_cycalloc(sizeof(*_tmp138E)),((_tmp138E->hd=(void*)((_tmp1390=_cycalloc(sizeof(*_tmp1390)),((_tmp1390[0]=((_tmp138F.tag=25,((_tmp138F.f1=_tmpB87,_tmp138F)))),_tmp1390)))),((_tmp138E->tl=effect,_tmp138E))))));}goto _LL69D;_LL69D:;};}}{
# 4529
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1393;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1392;*_tmpA15=(void*)((_tmp1392=_cycalloc(sizeof(*_tmp1392)),((_tmp1392[0]=((_tmp1393.tag=24,((_tmp1393.f1=effect,_tmp1393)))),_tmp1392))));};}
# 4535
if(*_tmpA14 != 0){
struct Cyc_List_List*bs=*_tmpA14;for(0;bs != 0;bs=bs->tl){
void*_tmpB94=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);void*_tmpB95=_tmpB94;struct Cyc_Core_Opt**_tmpB97;struct Cyc_Core_Opt**_tmpB99;struct Cyc_Core_Opt**_tmpB9B;struct Cyc_Core_Opt**_tmpB9D;struct Cyc_Core_Opt**_tmpB9F;struct Cyc_Core_Opt**_tmpBA1;struct Cyc_Core_Opt**_tmpBA3;struct Cyc_Core_Opt**_tmpBA5;struct Cyc_Core_Opt**_tmpBA7;struct Cyc_Core_Opt**_tmpBA9;struct Cyc_Absyn_Kind*_tmpBAA;_LL6A7: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpB96=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpB95;if(_tmpB96->tag != 1)goto _LL6A9;else{_tmpB97=(struct Cyc_Core_Opt**)& _tmpB96->f1;}}_LL6A8:
# 4539
{const char*_tmp1397;void*_tmp1396[1];struct Cyc_String_pa_PrintArg_struct _tmp1395;(_tmp1395.tag=0,((_tmp1395.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((_tmp1396[0]=& _tmp1395,Cyc_Tcutil_warn(loc,((_tmp1397="Type variable %s unconstrained, assuming boxed",_tag_dyneither(_tmp1397,sizeof(char),47))),_tag_dyneither(_tmp1396,sizeof(void*),1)))))));}
# 4541
_tmpB99=_tmpB97;goto _LL6AA;_LL6A9: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB98=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB95;if(_tmpB98->tag != 2)goto _LL6AB;else{_tmpB99=(struct Cyc_Core_Opt**)& _tmpB98->f1;if((_tmpB98->f2)->kind != Cyc_Absyn_BoxKind)goto _LL6AB;if((_tmpB98->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6AB;}}_LL6AA:
 _tmpB9B=_tmpB99;goto _LL6AC;_LL6AB: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB9A=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB95;if(_tmpB9A->tag != 2)goto _LL6AD;else{_tmpB9B=(struct Cyc_Core_Opt**)& _tmpB9A->f1;if((_tmpB9A->f2)->kind != Cyc_Absyn_MemKind)goto _LL6AD;if((_tmpB9A->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6AD;}}_LL6AC:
 _tmpB9D=_tmpB9B;goto _LL6AE;_LL6AD: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB9C=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB95;if(_tmpB9C->tag != 2)goto _LL6AF;else{_tmpB9D=(struct Cyc_Core_Opt**)& _tmpB9C->f1;if((_tmpB9C->f2)->kind != Cyc_Absyn_MemKind)goto _LL6AF;if((_tmpB9C->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL6AF;}}_LL6AE:
 _tmpB9F=_tmpB9D;goto _LL6B0;_LL6AF: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB9E=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB95;if(_tmpB9E->tag != 2)goto _LL6B1;else{_tmpB9F=(struct Cyc_Core_Opt**)& _tmpB9E->f1;if((_tmpB9E->f2)->kind != Cyc_Absyn_AnyKind)goto _LL6B1;if((_tmpB9E->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6B1;}}_LL6B0:
 _tmpBA1=_tmpB9F;goto _LL6B2;_LL6B1: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpBA0=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB95;if(_tmpBA0->tag != 2)goto _LL6B3;else{_tmpBA1=(struct Cyc_Core_Opt**)& _tmpBA0->f1;if((_tmpBA0->f2)->kind != Cyc_Absyn_AnyKind)goto _LL6B3;if((_tmpBA0->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL6B3;}}_LL6B2:
# 4547
*_tmpBA1=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL6A6;_LL6B3: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpBA2=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB95;if(_tmpBA2->tag != 2)goto _LL6B5;else{_tmpBA3=(struct Cyc_Core_Opt**)& _tmpBA2->f1;if((_tmpBA2->f2)->kind != Cyc_Absyn_MemKind)goto _LL6B5;if((_tmpBA2->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL6B5;}}_LL6B4:
 _tmpBA5=_tmpBA3;goto _LL6B6;_LL6B5: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpBA4=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB95;if(_tmpBA4->tag != 2)goto _LL6B7;else{_tmpBA5=(struct Cyc_Core_Opt**)& _tmpBA4->f1;if((_tmpBA4->f2)->kind != Cyc_Absyn_AnyKind)goto _LL6B7;if((_tmpBA4->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL6B7;}}_LL6B6:
# 4550
*_tmpBA5=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL6A6;_LL6B7: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpBA6=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB95;if(_tmpBA6->tag != 2)goto _LL6B9;else{_tmpBA7=(struct Cyc_Core_Opt**)& _tmpBA6->f1;if((_tmpBA6->f2)->kind != Cyc_Absyn_RgnKind)goto _LL6B9;if((_tmpBA6->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6B9;}}_LL6B8:
# 4552
*_tmpBA7=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL6A6;_LL6B9: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpBA8=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB95;if(_tmpBA8->tag != 2)goto _LL6BB;else{_tmpBA9=(struct Cyc_Core_Opt**)& _tmpBA8->f1;_tmpBAA=_tmpBA8->f2;}}_LL6BA:
# 4554
*_tmpBA9=Cyc_Tcutil_kind_to_bound_opt(_tmpBAA);goto _LL6A6;_LL6BB: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpBAB=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpB95;if(_tmpBAB->tag != 0)goto _LL6BD;else{if((_tmpBAB->f1)->kind != Cyc_Absyn_MemKind)goto _LL6BD;}}_LL6BC:
# 4556
{const char*_tmp139A;void*_tmp1399;(_tmp1399=0,Cyc_Tcutil_terr(loc,((_tmp139A="functions can't abstract M types",_tag_dyneither(_tmp139A,sizeof(char),33))),_tag_dyneither(_tmp1399,sizeof(void*),0)));}goto _LL6A6;_LL6BD:;_LL6BE:
 goto _LL6A6;_LL6A6:;}}
# 4561
cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,_tmpB11.kind_env,*_tmpA14);
_tmpB11.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(_tmpB11.r,_tmpB11.free_vars,*_tmpA14);
# 4564
{struct Cyc_List_List*tvs=_tmpB11.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmpBB1=*((struct _tuple28*)tvs->hd);struct Cyc_Absyn_Tvar*_tmpBB3;int _tmpBB4;struct _tuple28 _tmpBB2=_tmpBB1;_tmpBB3=_tmpBB2.f1;_tmpBB4=_tmpBB2.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpBB3,_tmpBB4);}}{
# 4569
struct Cyc_List_List*evs=_tmpB11.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmpBB5=*((struct _tuple29*)evs->hd);void*_tmpBB7;int _tmpBB8;struct _tuple29 _tmpBB6=_tmpBB5;_tmpBB7=_tmpBB6.f1;_tmpBB8=_tmpBB6.f2;
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmpBB7,_tmpBB8);}};};};};}
# 4574
_npop_handler(0);goto _LL5C0;
# 4349
;_pop_region(newr);};}_LL5E3: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpA21=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp9E8;if(_tmpA21->tag != 10)goto _LL5E5;else{_tmpA22=_tmpA21->f1;}}_LL5E4:
# 4577
 for(0;_tmpA22 != 0;_tmpA22=_tmpA22->tl){
struct _tuple12*_tmpBC0=(struct _tuple12*)_tmpA22->hd;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmpBC0).f2,1,0);
((*_tmpBC0).f1).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpBC0).f1).print_const,(*_tmpBC0).f2);}
# 4583
goto _LL5C0;_LL5E5: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpA23=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp9E8;if(_tmpA23->tag != 12)goto _LL5E7;else{_tmpA24=_tmpA23->f1;_tmpA25=_tmpA23->f2;}}_LL5E6: {
# 4587
struct _RegionHandle _tmpBC1=_new_region("aprev_rgn");struct _RegionHandle*aprev_rgn=& _tmpBC1;_push_region(aprev_rgn);{
struct Cyc_List_List*prev_fields=0;
for(0;_tmpA25 != 0;_tmpA25=_tmpA25->tl){
struct Cyc_Absyn_Aggrfield*_tmpBC2=(struct Cyc_Absyn_Aggrfield*)_tmpA25->hd;struct _dyneither_ptr*_tmpBC4;struct Cyc_Absyn_Tqual*_tmpBC5;void*_tmpBC6;struct Cyc_Absyn_Exp*_tmpBC7;struct Cyc_List_List*_tmpBC8;struct Cyc_Absyn_Exp*_tmpBC9;struct Cyc_Absyn_Aggrfield*_tmpBC3=_tmpBC2;_tmpBC4=_tmpBC3->name;_tmpBC5=(struct Cyc_Absyn_Tqual*)& _tmpBC3->tq;_tmpBC6=_tmpBC3->type;_tmpBC7=_tmpBC3->width;_tmpBC8=_tmpBC3->attributes;_tmpBC9=_tmpBC3->requires_clause;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpBC4)){
const char*_tmp139E;void*_tmp139D[1];struct Cyc_String_pa_PrintArg_struct _tmp139C;(_tmp139C.tag=0,((_tmp139C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpBC4),((_tmp139D[0]=& _tmp139C,Cyc_Tcutil_terr(loc,((_tmp139E="duplicate field %s",_tag_dyneither(_tmp139E,sizeof(char),19))),_tag_dyneither(_tmp139D,sizeof(void*),1)))))));}
{const char*_tmp139F;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpBC4,((_tmp139F="",_tag_dyneither(_tmp139F,sizeof(char),1))))!= 0){
struct Cyc_List_List*_tmp13A0;prev_fields=((_tmp13A0=_region_malloc(aprev_rgn,sizeof(*_tmp13A0)),((_tmp13A0->hd=_tmpBC4,((_tmp13A0->tl=prev_fields,_tmp13A0))))));}}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpBC6,1,0);
_tmpBC5->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpBC5->print_const,_tmpBC6);
# 4601
Cyc_Tcutil_check_bitfield(loc,te,_tmpBC6,_tmpBC7,_tmpBC4);
Cyc_Tcutil_check_field_atts(loc,_tmpBC4,_tmpBC8);
if(_tmpBC9 != 0){
# 4605
if(_tmpA24 != Cyc_Absyn_UnionA){
const char*_tmp13A3;void*_tmp13A2;(_tmp13A2=0,Cyc_Tcutil_terr(loc,((_tmp13A3="@requires clause is only allowed on union members",_tag_dyneither(_tmp13A3,sizeof(char),50))),_tag_dyneither(_tmp13A2,sizeof(void*),0)));}{
struct _RegionHandle _tmpBD1=_new_region("temp");struct _RegionHandle*temp=& _tmpBD1;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmpBD2=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmpBD2,0,_tmpBC9);}
# 4611
if(!Cyc_Tcutil_is_integral(_tmpBC9)){
const char*_tmp13A7;void*_tmp13A6[1];struct Cyc_String_pa_PrintArg_struct _tmp13A5;(_tmp13A5.tag=0,((_tmp13A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmpBC9->topt))),((_tmp13A6[0]=& _tmp13A5,Cyc_Tcutil_terr(loc,((_tmp13A7="@requires clause has type %s instead of integral type",_tag_dyneither(_tmp13A7,sizeof(char),54))),_tag_dyneither(_tmp13A6,sizeof(void*),1)))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpBC9,te,cvtenv);
# 4607
;_pop_region(temp);};}}}
# 4618
_npop_handler(0);goto _LL5C0;
# 4587
;_pop_region(aprev_rgn);}_LL5E7: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpA26=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp9E8;if(_tmpA26->tag != 11)goto _LL5E9;else{_tmpA27=(union Cyc_Absyn_AggrInfoU*)&(_tmpA26->f1).aggr_info;_tmpA28=(struct Cyc_List_List**)&(_tmpA26->f1).targs;}}_LL5E8:
# 4621
{union Cyc_Absyn_AggrInfoU _tmpBD6=*_tmpA27;union Cyc_Absyn_AggrInfoU _tmpBD7=_tmpBD6;enum Cyc_Absyn_AggrKind _tmpBD8;struct _tuple2*_tmpBD9;struct Cyc_Core_Opt*_tmpBDA;struct Cyc_Absyn_Aggrdecl*_tmpBDB;_LL6C0: if((_tmpBD7.UnknownAggr).tag != 1)goto _LL6C2;_tmpBD8=((struct _tuple4)(_tmpBD7.UnknownAggr).val).f1;_tmpBD9=((struct _tuple4)(_tmpBD7.UnknownAggr).val).f2;_tmpBDA=((struct _tuple4)(_tmpBD7.UnknownAggr).val).f3;_LL6C1: {
# 4623
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmpBDC;_push_handler(& _tmpBDC);{int _tmpBDE=0;if(setjmp(_tmpBDC.handler))_tmpBDE=1;if(!_tmpBDE){
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmpBD9);{
struct Cyc_Absyn_Aggrdecl*_tmpBDF=*adp;
if(_tmpBDF->kind != _tmpBD8){
if(_tmpBDF->kind == Cyc_Absyn_StructA){
const char*_tmp13AC;void*_tmp13AB[2];struct Cyc_String_pa_PrintArg_struct _tmp13AA;struct Cyc_String_pa_PrintArg_struct _tmp13A9;(_tmp13A9.tag=0,((_tmp13A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpBD9)),((_tmp13AA.tag=0,((_tmp13AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4629
Cyc_Absynpp_qvar2string(_tmpBD9)),((_tmp13AB[0]=& _tmp13AA,((_tmp13AB[1]=& _tmp13A9,Cyc_Tcutil_terr(loc,((_tmp13AC="expecting struct %s instead of union %s",_tag_dyneither(_tmp13AC,sizeof(char),40))),_tag_dyneither(_tmp13AB,sizeof(void*),2)))))))))))));}else{
# 4632
const char*_tmp13B1;void*_tmp13B0[2];struct Cyc_String_pa_PrintArg_struct _tmp13AF;struct Cyc_String_pa_PrintArg_struct _tmp13AE;(_tmp13AE.tag=0,((_tmp13AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpBD9)),((_tmp13AF.tag=0,((_tmp13AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4632
Cyc_Absynpp_qvar2string(_tmpBD9)),((_tmp13B0[0]=& _tmp13AF,((_tmp13B0[1]=& _tmp13AE,Cyc_Tcutil_terr(loc,((_tmp13B1="expecting union %s instead of struct %s",_tag_dyneither(_tmp13B1,sizeof(char),40))),_tag_dyneither(_tmp13B0,sizeof(void*),2)))))))))))));}}
# 4635
if((unsigned int)_tmpBDA  && (int)_tmpBDA->v){
if(!((unsigned int)_tmpBDF->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpBDF->impl))->tagged){
const char*_tmp13B5;void*_tmp13B4[1];struct Cyc_String_pa_PrintArg_struct _tmp13B3;(_tmp13B3.tag=0,((_tmp13B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpBD9)),((_tmp13B4[0]=& _tmp13B3,Cyc_Tcutil_terr(loc,((_tmp13B5="@tagged qualfiers don't agree on union %s",_tag_dyneither(_tmp13B5,sizeof(char),42))),_tag_dyneither(_tmp13B4,sizeof(void*),1)))))));}}
# 4641
*_tmpA27=Cyc_Absyn_KnownAggr(adp);};
# 4625
;_pop_handler();}else{void*_tmpBDD=(void*)_exn_thrown;void*_tmpBEC=_tmpBDD;void*_tmpBEE;_LL6C5: {struct Cyc_Dict_Absent_exn_struct*_tmpBED=(struct Cyc_Dict_Absent_exn_struct*)_tmpBEC;if(_tmpBED->tag != Cyc_Dict_Absent)goto _LL6C7;}_LL6C6: {
# 4645
struct Cyc_Tcenv_Genv*_tmpBEF=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp13B6;struct Cyc_Absyn_Aggrdecl*_tmpBF0=(_tmp13B6=_cycalloc(sizeof(*_tmp13B6)),((_tmp13B6->kind=_tmpBD8,((_tmp13B6->sc=Cyc_Absyn_Extern,((_tmp13B6->name=_tmpBD9,((_tmp13B6->tvs=0,((_tmp13B6->impl=0,((_tmp13B6->attributes=0,((_tmp13B6->expected_mem_kind=0,_tmp13B6)))))))))))))));
Cyc_Tc_tcAggrdecl(te,_tmpBEF,loc,_tmpBF0);
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmpBD9);
*_tmpA27=Cyc_Absyn_KnownAggr(adp);
# 4651
if(*_tmpA28 != 0){
{const char*_tmp13BA;void*_tmp13B9[1];struct Cyc_String_pa_PrintArg_struct _tmp13B8;(_tmp13B8.tag=0,((_tmp13B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpBD9)),((_tmp13B9[0]=& _tmp13B8,Cyc_Tcutil_terr(loc,((_tmp13BA="declare parameterized type %s before using",_tag_dyneither(_tmp13BA,sizeof(char),43))),_tag_dyneither(_tmp13B9,sizeof(void*),1)))))));}
return cvtenv;}
# 4656
goto _LL6C4;}_LL6C7: _tmpBEE=_tmpBEC;_LL6C8:(void)_rethrow(_tmpBEE);_LL6C4:;}};}
# 4658
_tmpBDB=*adp;goto _LL6C3;}_LL6C2: if((_tmpBD7.KnownAggr).tag != 2)goto _LL6BF;_tmpBDB=*((struct Cyc_Absyn_Aggrdecl**)(_tmpBD7.KnownAggr).val);_LL6C3: {
# 4660
struct Cyc_List_List*tvs=_tmpBDB->tvs;
struct Cyc_List_List*ts=*_tmpA28;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Tvar*_tmpBF5=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmpBF6=(void*)ts->hd;
# 4668
{struct _tuple0 _tmp13BB;struct _tuple0 _tmpBF7=(_tmp13BB.f1=Cyc_Absyn_compress_kb(_tmpBF5->kind),((_tmp13BB.f2=_tmpBF6,_tmp13BB)));struct _tuple0 _tmpBF8=_tmpBF7;struct Cyc_Absyn_Tvar*_tmpBFB;_LL6CA:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpBF9=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpBF8.f1;if(_tmpBF9->tag != 1)goto _LL6CC;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpBFA=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpBF8.f2;if(_tmpBFA->tag != 2)goto _LL6CC;else{_tmpBFB=_tmpBFA->f1;}};_LL6CB:
# 4670
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpBFB);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpBFB,1);
continue;_LL6CC:;_LL6CD:
 goto _LL6C9;_LL6C9:;}{
# 4675
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 4679
if(ts != 0){
const char*_tmp13BF;void*_tmp13BE[1];struct Cyc_String_pa_PrintArg_struct _tmp13BD;(_tmp13BD.tag=0,((_tmp13BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBDB->name)),((_tmp13BE[0]=& _tmp13BD,Cyc_Tcutil_terr(loc,((_tmp13BF="too many parameters for type %s",_tag_dyneither(_tmp13BF,sizeof(char),32))),_tag_dyneither(_tmp13BE,sizeof(void*),1)))))));}
if(tvs != 0){
# 4683
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp13C0;hidden_ts=((_tmp13C0=_cycalloc(sizeof(*_tmp13C0)),((_tmp13C0->hd=e,((_tmp13C0->tl=hidden_ts,_tmp13C0))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);}
# 4690
*_tmpA28=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpA28,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4692
if((allow_abs_aggr  && _tmpBDB->impl == 0) && !
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,expected_kind))
# 4697
_tmpBDB->expected_mem_kind=1;}_LL6BF:;}
# 4700
goto _LL5C0;_LL5E9: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA29=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9E8;if(_tmpA29->tag != 17)goto _LL5EB;else{_tmpA2A=_tmpA29->f1;_tmpA2B=(struct Cyc_List_List**)& _tmpA29->f2;_tmpA2C=(struct Cyc_Absyn_Typedefdecl**)& _tmpA29->f3;_tmpA2D=(void**)((void**)& _tmpA29->f4);}}_LL5EA: {
# 4703
struct Cyc_List_List*targs=*_tmpA2B;
# 4705
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmpC01;_push_handler(& _tmpC01);{int _tmpC03=0;if(setjmp(_tmpC01.handler))_tmpC03=1;if(!_tmpC03){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmpA2A);;_pop_handler();}else{void*_tmpC02=(void*)_exn_thrown;void*_tmpC05=_tmpC02;void*_tmpC07;_LL6CF: {struct Cyc_Dict_Absent_exn_struct*_tmpC06=(struct Cyc_Dict_Absent_exn_struct*)_tmpC05;if(_tmpC06->tag != Cyc_Dict_Absent)goto _LL6D1;}_LL6D0:
# 4708
{const char*_tmp13C4;void*_tmp13C3[1];struct Cyc_String_pa_PrintArg_struct _tmp13C2;(_tmp13C2.tag=0,((_tmp13C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA2A)),((_tmp13C3[0]=& _tmp13C2,Cyc_Tcutil_terr(loc,((_tmp13C4="unbound typedef name %s",_tag_dyneither(_tmp13C4,sizeof(char),24))),_tag_dyneither(_tmp13C3,sizeof(void*),1)))))));}
return cvtenv;_LL6D1: _tmpC07=_tmpC05;_LL6D2:(void)_rethrow(_tmpC07);_LL6CE:;}};}
# 4711
*_tmpA2C=td;
# 4713
_tmpA2A[0]=(td->name)[0];{
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;
struct _RegionHandle _tmpC0B=_new_region("temp");struct _RegionHandle*temp=& _tmpC0B;_push_region(temp);{
struct Cyc_List_List*inst=0;
# 4719
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,td);
# 4724
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{
struct _tuple16*_tmp13C7;struct Cyc_List_List*_tmp13C6;inst=((_tmp13C6=_region_malloc(temp,sizeof(*_tmp13C6)),((_tmp13C6->hd=((_tmp13C7=_region_malloc(temp,sizeof(*_tmp13C7)),((_tmp13C7->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp13C7->f2=(void*)ts->hd,_tmp13C7)))))),((_tmp13C6->tl=inst,_tmp13C6))))));};}
# 4728
if(ts != 0){
const char*_tmp13CB;void*_tmp13CA[1];struct Cyc_String_pa_PrintArg_struct _tmp13C9;(_tmp13C9.tag=0,((_tmp13C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA2A)),((_tmp13CA[0]=& _tmp13C9,Cyc_Tcutil_terr(loc,((_tmp13CB="too many parameters for typedef %s",_tag_dyneither(_tmp13CB,sizeof(char),35))),_tag_dyneither(_tmp13CA,sizeof(void*),1)))))));}
if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;
# 4733
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp13CC;hidden_ts=((_tmp13CC=_cycalloc(sizeof(*_tmp13CC)),((_tmp13CC->hd=e,((_tmp13CC->tl=hidden_ts,_tmp13CC))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);{
struct _tuple16*_tmp13CF;struct Cyc_List_List*_tmp13CE;inst=((_tmp13CE=_cycalloc(sizeof(*_tmp13CE)),((_tmp13CE->hd=((_tmp13CF=_cycalloc(sizeof(*_tmp13CF)),((_tmp13CF->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp13CF->f2=e,_tmp13CF)))))),((_tmp13CE->tl=inst,_tmp13CE))))));};}
# 4741
*_tmpA2B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4743
if(td->defn != 0){
void*new_typ=
inst == 0?(void*)_check_null(td->defn):
 Cyc_Tcutil_rsubstitute(temp,inst,(void*)_check_null(td->defn));
*_tmpA2D=new_typ;}}
# 4750
_npop_handler(0);goto _LL5C0;
# 4716
;_pop_region(temp);};}_LL5EB: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpA2E=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp9E8;if(_tmpA2E->tag != 22)goto _LL5ED;}_LL5EC:
# 4751
 goto _LL5EE;_LL5ED: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpA2F=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp9E8;if(_tmpA2F->tag != 21)goto _LL5EF;}_LL5EE:
 goto _LL5F0;_LL5EF: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmpA30=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp9E8;if(_tmpA30->tag != 20)goto _LL5F1;}_LL5F0:
 goto _LL5C0;_LL5F1: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA31=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp9E8;if(_tmpA31->tag != 15)goto _LL5F3;else{_tmpA32=(void*)_tmpA31->f1;}}_LL5F2:
# 4755
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmpA32,1,allow_abs_aggr);goto _LL5C0;_LL5F3: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpA33=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp9E8;if(_tmpA33->tag != 16)goto _LL5F5;else{_tmpA34=(void*)_tmpA33->f1;_tmpA35=(void*)_tmpA33->f2;}}_LL5F4:
# 4758
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmpA34,0,0);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmpA35,1,0);
goto _LL5C0;_LL5F5: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpA36=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp9E8;if(_tmpA36->tag != 23)goto _LL5F7;else{_tmpA37=(void*)_tmpA36->f1;}}_LL5F6:
# 4762
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmpA37,1,1);goto _LL5C0;_LL5F7: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpA38=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp9E8;if(_tmpA38->tag != 25)goto _LL5F9;else{_tmpA39=(void*)_tmpA38->f1;}}_LL5F8:
# 4764
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpA39,1,1);goto _LL5C0;_LL5F9: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpA3A=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp9E8;if(_tmpA3A->tag != 24)goto _LL5C0;else{_tmpA3B=_tmpA3A->f1;}}_LL5FA:
# 4766
 for(0;_tmpA3B != 0;_tmpA3B=_tmpA3B->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmpA3B->hd,1,1);}
goto _LL5C0;_LL5C0:;}
# 4770
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind)){
const char*_tmp13D5;void*_tmp13D4[3];struct Cyc_String_pa_PrintArg_struct _tmp13D3;struct Cyc_String_pa_PrintArg_struct _tmp13D2;struct Cyc_String_pa_PrintArg_struct _tmp13D1;(_tmp13D1.tag=0,((_tmp13D1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(expected_kind)),((_tmp13D2.tag=0,((_tmp13D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmp13D3.tag=0,((_tmp13D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp13D4[0]=& _tmp13D3,((_tmp13D4[1]=& _tmp13D2,((_tmp13D4[2]=& _tmp13D1,Cyc_Tcutil_terr(loc,((_tmp13D5="type %s has kind %s but as used here needs kind %s",_tag_dyneither(_tmp13D5,sizeof(char),51))),_tag_dyneither(_tmp13D4,sizeof(void*),3)))))))))))))))))));}
# 4774
return cvtenv;}
# 4782
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){
# 4784
{void*_tmpC19=e->r;void*_tmpC1A=_tmpC19;struct Cyc_List_List*_tmpC20;struct Cyc_Absyn_Exp*_tmpC22;struct Cyc_Absyn_Exp*_tmpC23;struct Cyc_Absyn_Exp*_tmpC24;struct Cyc_Absyn_Exp*_tmpC26;struct Cyc_Absyn_Exp*_tmpC27;struct Cyc_Absyn_Exp*_tmpC29;struct Cyc_Absyn_Exp*_tmpC2A;struct Cyc_Absyn_Exp*_tmpC2C;struct Cyc_Absyn_Exp*_tmpC2D;void*_tmpC2F;struct Cyc_Absyn_Exp*_tmpC30;void*_tmpC32;void*_tmpC34;void*_tmpC36;struct Cyc_Absyn_Exp*_tmpC38;_LL6D4: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpC1B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpC1A;if(_tmpC1B->tag != 0)goto _LL6D6;}_LL6D5:
 goto _LL6D7;_LL6D6: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpC1C=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpC1A;if(_tmpC1C->tag != 31)goto _LL6D8;}_LL6D7:
 goto _LL6D9;_LL6D8: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpC1D=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpC1A;if(_tmpC1D->tag != 32)goto _LL6DA;}_LL6D9:
 goto _LL6DB;_LL6DA: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpC1E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC1A;if(_tmpC1E->tag != 1)goto _LL6DC;}_LL6DB:
 goto _LL6D3;_LL6DC: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpC1F=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC1A;if(_tmpC1F->tag != 2)goto _LL6DE;else{_tmpC20=_tmpC1F->f2;}}_LL6DD:
# 4790
 for(0;_tmpC20 != 0;_tmpC20=_tmpC20->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmpC20->hd,te,cvtenv);}
goto _LL6D3;_LL6DE: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpC21=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpC1A;if(_tmpC21->tag != 5)goto _LL6E0;else{_tmpC22=_tmpC21->f1;_tmpC23=_tmpC21->f2;_tmpC24=_tmpC21->f3;}}_LL6DF:
# 4794
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC22,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC23,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC24,te,cvtenv);
goto _LL6D3;_LL6E0: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpC25=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpC1A;if(_tmpC25->tag != 6)goto _LL6E2;else{_tmpC26=_tmpC25->f1;_tmpC27=_tmpC25->f2;}}_LL6E1:
 _tmpC29=_tmpC26;_tmpC2A=_tmpC27;goto _LL6E3;_LL6E2: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmpC28=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpC1A;if(_tmpC28->tag != 7)goto _LL6E4;else{_tmpC29=_tmpC28->f1;_tmpC2A=_tmpC28->f2;}}_LL6E3:
 _tmpC2C=_tmpC29;_tmpC2D=_tmpC2A;goto _LL6E5;_LL6E4: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpC2B=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpC1A;if(_tmpC2B->tag != 8)goto _LL6E6;else{_tmpC2C=_tmpC2B->f1;_tmpC2D=_tmpC2B->f2;}}_LL6E5:
# 4801
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC2C,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC2D,te,cvtenv);
goto _LL6D3;_LL6E6: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpC2E=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpC1A;if(_tmpC2E->tag != 13)goto _LL6E8;else{_tmpC2F=(void*)_tmpC2E->f1;_tmpC30=_tmpC2E->f2;}}_LL6E7:
# 4805
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC30,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpC2F,1,0);
goto _LL6D3;_LL6E8: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpC31=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpC1A;if(_tmpC31->tag != 18)goto _LL6EA;else{_tmpC32=(void*)_tmpC31->f1;}}_LL6E9:
 _tmpC34=_tmpC32;goto _LL6EB;_LL6EA: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpC33=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpC1A;if(_tmpC33->tag != 16)goto _LL6EC;else{_tmpC34=(void*)_tmpC33->f1;}}_LL6EB:
# 4810
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpC34,1,0);
goto _LL6D3;_LL6EC: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmpC35=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpC1A;if(_tmpC35->tag != 38)goto _LL6EE;else{_tmpC36=(void*)_tmpC35->f1;}}_LL6ED:
# 4813
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpC36,1,0);
goto _LL6D3;_LL6EE: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpC37=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpC1A;if(_tmpC37->tag != 17)goto _LL6F0;else{_tmpC38=_tmpC37->f1;}}_LL6EF:
# 4816
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC38,te,cvtenv);
goto _LL6D3;_LL6F0:;_LL6F1: {
# 4819
const char*_tmp13D8;void*_tmp13D7;(_tmp13D7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13D8="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",_tag_dyneither(_tmp13D8,sizeof(char),66))),_tag_dyneither(_tmp13D7,sizeof(void*),0)));}_LL6D3:;}
# 4821
return cvtenv;}
# 4824
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,int allow_abs_aggr,void*t){
# 4830
struct Cyc_List_List*_tmpC3B=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1,allow_abs_aggr);
# 4833
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
struct _tuple28 _tmpC3C=*((struct _tuple28*)vs->hd);struct Cyc_Absyn_Tvar*_tmpC3E;struct _tuple28 _tmpC3D=_tmpC3C;_tmpC3E=_tmpC3D.f1;
cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(_tmpC3B,_tmpC3E);}}
# 4841
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmpC3F=*((struct _tuple29*)evs->hd);void*_tmpC41;struct _tuple29 _tmpC40=_tmpC3F;_tmpC41=_tmpC40.f1;{
void*_tmpC42=Cyc_Tcutil_compress(_tmpC41);void*_tmpC43=_tmpC42;struct Cyc_Core_Opt**_tmpC45;_LL6F3: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpC44=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpC43;if(_tmpC44->tag != 1)goto _LL6F5;else{_tmpC45=(struct Cyc_Core_Opt**)& _tmpC44->f4;}}_LL6F4:
# 4845
 if(*_tmpC45 == 0){
struct Cyc_Core_Opt*_tmp13D9;*_tmpC45=((_tmp13D9=_cycalloc(sizeof(*_tmp13D9)),((_tmp13D9->v=_tmpC3B,_tmp13D9))));}else{
# 4849
struct Cyc_List_List*_tmpC47=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmpC45))->v;
struct Cyc_List_List*_tmpC48=0;
for(0;_tmpC47 != 0;_tmpC47=_tmpC47->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmpC3B,(struct Cyc_Absyn_Tvar*)_tmpC47->hd)){
struct Cyc_List_List*_tmp13DA;_tmpC48=((_tmp13DA=_cycalloc(sizeof(*_tmp13DA)),((_tmp13DA->hd=(struct Cyc_Absyn_Tvar*)_tmpC47->hd,((_tmp13DA->tl=_tmpC48,_tmp13DA))))));}}{
struct Cyc_Core_Opt*_tmp13DB;*_tmpC45=((_tmp13DB=_cycalloc(sizeof(*_tmp13DB)),((_tmp13DB->v=_tmpC48,_tmp13DB))));};}
# 4856
goto _LL6F2;_LL6F5:;_LL6F6:
 goto _LL6F2;_LL6F2:;};}}
# 4860
return cvt;}
# 4866
void Cyc_Tcutil_check_free_evars(struct Cyc_List_List*free_evars,void*in_t,unsigned int loc){
# 4868
for(0;free_evars != 0;free_evars=free_evars->tl){
void*e=(void*)free_evars->hd;
struct Cyc_Absyn_Kind*_tmpC4B=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind*_tmpC4C=_tmpC4B;_LL6F8: if(_tmpC4C->kind != Cyc_Absyn_RgnKind)goto _LL6FA;if(_tmpC4C->aliasqual != Cyc_Absyn_Unique)goto _LL6FA;_LL6F9:
# 4872
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp13DE;void*_tmp13DD;(_tmp13DD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13DE="can't unify evar with unique region!",_tag_dyneither(_tmp13DE,sizeof(char),37))),_tag_dyneither(_tmp13DD,sizeof(void*),0)));}
goto _LL6F7;_LL6FA: if(_tmpC4C->kind != Cyc_Absyn_RgnKind)goto _LL6FC;if(_tmpC4C->aliasqual != Cyc_Absyn_Aliasable)goto _LL6FC;_LL6FB:
 goto _LL6FD;_LL6FC: if(_tmpC4C->kind != Cyc_Absyn_RgnKind)goto _LL6FE;if(_tmpC4C->aliasqual != Cyc_Absyn_Top)goto _LL6FE;_LL6FD:
# 4877
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp13E1;void*_tmp13E0;(_tmp13E0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13E1="can't unify evar with heap!",_tag_dyneither(_tmp13E1,sizeof(char),28))),_tag_dyneither(_tmp13E0,sizeof(void*),0)));}
goto _LL6F7;_LL6FE: if(_tmpC4C->kind != Cyc_Absyn_EffKind)goto _LL700;_LL6FF:
# 4880
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*_tmp13E4;void*_tmp13E3;(_tmp13E3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13E4="can't unify evar with {}!",_tag_dyneither(_tmp13E4,sizeof(char),26))),_tag_dyneither(_tmp13E3,sizeof(void*),0)));}
goto _LL6F7;_LL700:;_LL701:
# 4883
{const char*_tmp13E9;void*_tmp13E8[2];struct Cyc_String_pa_PrintArg_struct _tmp13E7;struct Cyc_String_pa_PrintArg_struct _tmp13E6;(_tmp13E6.tag=0,((_tmp13E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(in_t)),((_tmp13E7.tag=0,((_tmp13E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e)),((_tmp13E8[0]=& _tmp13E7,((_tmp13E8[1]=& _tmp13E6,Cyc_Tcutil_terr(loc,((_tmp13E9="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(_tmp13E9,sizeof(char),52))),_tag_dyneither(_tmp13E8,sizeof(void*),2)))))))))))));}
goto _LL6F7;_LL6F7:;}}
# 4895
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_List_List*_tmpC57=Cyc_Tcenv_lookup_type_vars(te);
struct Cyc_Absyn_Kind*expected_kind=generalize_evars?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
struct _RegionHandle _tmpC58=_new_region("temp");struct _RegionHandle*temp=& _tmpC58;_push_region(temp);
{struct Cyc_Tcutil_CVTEnv _tmp13EA;struct Cyc_Tcutil_CVTEnv _tmpC59=Cyc_Tcutil_check_valid_type(loc,te,(
(_tmp13EA.r=temp,((_tmp13EA.kind_env=_tmpC57,((_tmp13EA.free_vars=0,((_tmp13EA.free_evars=0,((_tmp13EA.generalize_evars=generalize_evars,((_tmp13EA.fn_result=0,_tmp13EA)))))))))))),expected_kind,1,t);
# 4904
struct Cyc_List_List*_tmpC5A=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmpC59.free_vars);
struct Cyc_List_List*_tmpC5B=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmpC59.free_evars);
# 4908
if(_tmpC57 != 0){
struct Cyc_List_List*_tmpC5C=0;
{struct Cyc_List_List*_tmpC5D=_tmpC5A;for(0;(unsigned int)_tmpC5D;_tmpC5D=_tmpC5D->tl){
struct Cyc_Absyn_Tvar*_tmpC5E=(struct Cyc_Absyn_Tvar*)_tmpC5D->hd;
int found=0;
{struct Cyc_List_List*_tmpC5F=_tmpC57;for(0;(unsigned int)_tmpC5F;_tmpC5F=_tmpC5F->tl){
if(Cyc_Absyn_tvar_cmp(_tmpC5E,(struct Cyc_Absyn_Tvar*)_tmpC5F->hd)== 0){found=1;break;}}}
if(!found){
struct Cyc_List_List*_tmp13EB;_tmpC5C=((_tmp13EB=_region_malloc(temp,sizeof(*_tmp13EB)),((_tmp13EB->hd=(struct Cyc_Absyn_Tvar*)_tmpC5D->hd,((_tmp13EB->tl=_tmpC5C,_tmp13EB))))));}}}
# 4918
_tmpC5A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpC5C);}
# 4924
{struct Cyc_List_List*x=_tmpC5A;for(0;x != 0;x=x->tl){
void*_tmpC61=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);void*_tmpC62=_tmpC61;struct Cyc_Core_Opt**_tmpC64;struct Cyc_Core_Opt**_tmpC66;struct Cyc_Core_Opt**_tmpC68;struct Cyc_Core_Opt**_tmpC6A;struct Cyc_Core_Opt**_tmpC6C;struct Cyc_Core_Opt**_tmpC6E;struct Cyc_Core_Opt**_tmpC70;struct Cyc_Absyn_Kind*_tmpC71;enum Cyc_Absyn_AliasQual _tmpC73;_LL703: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpC63=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpC62;if(_tmpC63->tag != 1)goto _LL705;else{_tmpC64=(struct Cyc_Core_Opt**)& _tmpC63->f1;}}_LL704:
 _tmpC66=_tmpC64;goto _LL706;_LL705: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC65=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC62;if(_tmpC65->tag != 2)goto _LL707;else{_tmpC66=(struct Cyc_Core_Opt**)& _tmpC65->f1;if((_tmpC65->f2)->kind != Cyc_Absyn_BoxKind)goto _LL707;if((_tmpC65->f2)->aliasqual != Cyc_Absyn_Top)goto _LL707;}}_LL706:
 _tmpC68=_tmpC66;goto _LL708;_LL707: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC67=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC62;if(_tmpC67->tag != 2)goto _LL709;else{_tmpC68=(struct Cyc_Core_Opt**)& _tmpC67->f1;if((_tmpC67->f2)->kind != Cyc_Absyn_MemKind)goto _LL709;if((_tmpC67->f2)->aliasqual != Cyc_Absyn_Top)goto _LL709;}}_LL708:
 _tmpC6A=_tmpC68;goto _LL70A;_LL709: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC69=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC62;if(_tmpC69->tag != 2)goto _LL70B;else{_tmpC6A=(struct Cyc_Core_Opt**)& _tmpC69->f1;if((_tmpC69->f2)->kind != Cyc_Absyn_MemKind)goto _LL70B;if((_tmpC69->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL70B;}}_LL70A:
# 4930
*_tmpC6A=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL702;_LL70B: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC6B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC62;if(_tmpC6B->tag != 2)goto _LL70D;else{_tmpC6C=(struct Cyc_Core_Opt**)& _tmpC6B->f1;if((_tmpC6B->f2)->kind != Cyc_Absyn_MemKind)goto _LL70D;if((_tmpC6B->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL70D;}}_LL70C:
# 4932
*_tmpC6C=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL702;_LL70D: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC6D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC62;if(_tmpC6D->tag != 2)goto _LL70F;else{_tmpC6E=(struct Cyc_Core_Opt**)& _tmpC6D->f1;if((_tmpC6D->f2)->kind != Cyc_Absyn_RgnKind)goto _LL70F;if((_tmpC6D->f2)->aliasqual != Cyc_Absyn_Top)goto _LL70F;}}_LL70E:
# 4934
*_tmpC6E=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL702;_LL70F: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC6F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC62;if(_tmpC6F->tag != 2)goto _LL711;else{_tmpC70=(struct Cyc_Core_Opt**)& _tmpC6F->f1;_tmpC71=_tmpC6F->f2;}}_LL710:
# 4936
*_tmpC70=Cyc_Tcutil_kind_to_bound_opt(_tmpC71);goto _LL702;_LL711: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpC72=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpC62;if(_tmpC72->tag != 0)goto _LL713;else{if((_tmpC72->f1)->kind != Cyc_Absyn_MemKind)goto _LL713;_tmpC73=(_tmpC72->f1)->aliasqual;}}_LL712:
# 4938
{const char*_tmp13F2;void*_tmp13F1[2];struct Cyc_String_pa_PrintArg_struct _tmp13F0;struct Cyc_Absyn_Kind*_tmp13EF;struct Cyc_String_pa_PrintArg_struct _tmp13EE;(_tmp13EE.tag=0,((_tmp13EE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(((_tmp13EF=_cycalloc_atomic(sizeof(*_tmp13EF)),((_tmp13EF->kind=Cyc_Absyn_MemKind,((_tmp13EF->aliasqual=_tmpC73,_tmp13EF)))))))),((_tmp13F0.tag=0,((_tmp13F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd)),((_tmp13F1[0]=& _tmp13F0,((_tmp13F1[1]=& _tmp13EE,Cyc_Tcutil_terr(loc,((_tmp13F2="type variable %s cannot have kind %s",_tag_dyneither(_tmp13F2,sizeof(char),37))),_tag_dyneither(_tmp13F1,sizeof(void*),2)))))))))))));}
goto _LL702;_LL713:;_LL714:
 goto _LL702;_LL702:;}}
# 4945
if(_tmpC5A != 0  || _tmpC5B != 0){
{void*_tmpC79=Cyc_Tcutil_compress(t);void*_tmpC7A=_tmpC79;struct Cyc_List_List**_tmpC7C;_LL716: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpC7B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC7A;if(_tmpC7B->tag != 9)goto _LL718;else{_tmpC7C=(struct Cyc_List_List**)&(_tmpC7B->f1).tvars;}}_LL717:
# 4948
 if(*_tmpC7C == 0){
# 4950
*_tmpC7C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmpC5A);
_tmpC5A=0;}
# 4953
goto _LL715;_LL718:;_LL719:
 goto _LL715;_LL715:;}
# 4956
if(_tmpC5A != 0){
const char*_tmp13F6;void*_tmp13F5[1];struct Cyc_String_pa_PrintArg_struct _tmp13F4;(_tmp13F4.tag=0,((_tmp13F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmpC5A->hd)->name),((_tmp13F5[0]=& _tmp13F4,Cyc_Tcutil_terr(loc,((_tmp13F6="unbound type variable %s",_tag_dyneither(_tmp13F6,sizeof(char),25))),_tag_dyneither(_tmp13F5,sizeof(void*),1)))))));}
Cyc_Tcutil_check_free_evars(_tmpC5B,t,loc);}}
# 4900
;_pop_region(temp);}
# 4967
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 4970
Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);
{void*_tmpC81=Cyc_Tcutil_compress(t);void*_tmpC82=_tmpC81;struct Cyc_List_List*_tmpC84;void*_tmpC85;struct Cyc_Absyn_Tqual _tmpC86;void*_tmpC87;struct Cyc_List_List*_tmpC88;struct Cyc_Absyn_Exp*_tmpC89;struct Cyc_List_List*_tmpC8A;struct Cyc_Absyn_Exp*_tmpC8B;struct Cyc_List_List*_tmpC8C;_LL71B: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpC83=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC82;if(_tmpC83->tag != 9)goto _LL71D;else{_tmpC84=(_tmpC83->f1).tvars;_tmpC85=(_tmpC83->f1).effect;_tmpC86=(_tmpC83->f1).ret_tqual;_tmpC87=(_tmpC83->f1).ret_typ;_tmpC88=(_tmpC83->f1).args;_tmpC89=(_tmpC83->f1).requires_clause;_tmpC8A=(_tmpC83->f1).requires_relns;_tmpC8B=(_tmpC83->f1).ensures_clause;_tmpC8C=(_tmpC83->f1).ensures_relns;}}_LL71C:
# 4973
 fd->tvs=_tmpC84;
fd->effect=_tmpC85;
{struct Cyc_List_List*_tmpC8D=fd->args;for(0;_tmpC8D != 0;(_tmpC8D=_tmpC8D->tl,_tmpC88=_tmpC88->tl)){
# 4977
(*((struct _tuple10*)_tmpC8D->hd)).f2=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmpC88))->hd)).f2;
(*((struct _tuple10*)_tmpC8D->hd)).f3=(*((struct _tuple10*)_tmpC88->hd)).f3;}}
# 4980
fd->ret_tqual=_tmpC86;
fd->ret_type=_tmpC87;
(fd->ret_tqual).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmpC87);
fd->requires_clause=_tmpC89;
fd->requires_relns=_tmpC8A;
fd->ensures_clause=_tmpC8B;
fd->ensures_relns=_tmpC8C;
goto _LL71A;_LL71D:;_LL71E: {
const char*_tmp13F9;void*_tmp13F8;(_tmp13F8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13F9="check_fndecl_valid_type: not a FnType",_tag_dyneither(_tmp13F9,sizeof(char),38))),_tag_dyneither(_tmp13F8,sizeof(void*),0)));}_LL71A:;}{
# 4991
struct _RegionHandle _tmpC90=_new_region("r");struct _RegionHandle*r=& _tmpC90;_push_region(r);{
const char*_tmp13FA;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _dyneither_ptr*(*)(struct _tuple10*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,(
(_tmp13FA="function declaration has repeated parameter",_tag_dyneither(_tmp13FA,sizeof(char),44))));}
# 4996
fd->cached_typ=t;
# 4991
;_pop_region(r);};}
# 5001
void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,int allow_abs_aggr,void*t){
# 5007
struct _RegionHandle _tmpC92=_new_region("r");struct _RegionHandle*r=& _tmpC92;_push_region(r);
{struct Cyc_Tcutil_CVTEnv _tmp13FB;struct Cyc_Tcutil_CVTEnv _tmpC93=Cyc_Tcutil_check_valid_type(loc,te,(
(_tmp13FB.r=r,((_tmp13FB.kind_env=bound_tvars,((_tmp13FB.free_vars=0,((_tmp13FB.free_evars=0,((_tmp13FB.generalize_evars=0,((_tmp13FB.fn_result=0,_tmp13FB)))))))))))),expected_kind,allow_abs_aggr,t);
# 5011
struct Cyc_List_List*_tmpC94=Cyc_Tcutil_remove_bound_tvars(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmpC93.free_vars),bound_tvars);
# 5013
struct Cyc_List_List*_tmpC95=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmpC93.free_evars);
{struct Cyc_List_List*fs=_tmpC94;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmpC96=((struct Cyc_Absyn_Tvar*)fs->hd)->name;
const char*_tmp1400;void*_tmp13FF[2];struct Cyc_String_pa_PrintArg_struct _tmp13FE;struct Cyc_String_pa_PrintArg_struct _tmp13FD;(_tmp13FD.tag=0,((_tmp13FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp13FE.tag=0,((_tmp13FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpC96),((_tmp13FF[0]=& _tmp13FE,((_tmp13FF[1]=& _tmp13FD,Cyc_Tcutil_terr(loc,((_tmp1400="unbound type variable %s in type %s",_tag_dyneither(_tmp1400,sizeof(char),36))),_tag_dyneither(_tmp13FF,sizeof(void*),2)))))))))))));}}
# 5018
if(!allow_evars)
Cyc_Tcutil_check_free_evars(_tmpC95,t,loc);}
# 5008
;_pop_region(r);}
# 5023
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
tv->identity=Cyc_Tcutil_new_tvar_id();}
# 5028
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 5034
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){
# 5037
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0){
const char*_tmp1405;void*_tmp1404[2];struct Cyc_String_pa_PrintArg_struct _tmp1403;struct Cyc_String_pa_PrintArg_struct _tmp1402;(_tmp1402.tag=0,((_tmp1402.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string(vs->hd)),((_tmp1403.tag=0,((_tmp1403.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp1404[0]=& _tmp1403,((_tmp1404[1]=& _tmp1402,Cyc_Tcutil_terr(loc,((_tmp1405="%s: %s",_tag_dyneither(_tmp1405,sizeof(char),7))),_tag_dyneither(_tmp1404,sizeof(void*),2)))))))))))));}}}}
# 5043
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s){
return*s;}
# 5047
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr msg){
((void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 5051
void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
const char*_tmp1406;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,((_tmp1406="duplicate type variable",_tag_dyneither(_tmp1406,sizeof(char),24))));}struct _tuple32{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple33{struct Cyc_List_List*f1;void*f2;};struct _tuple34{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 5065 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 5070
struct _RegionHandle _tmpCA1=_new_region("temp");struct _RegionHandle*temp=& _tmpCA1;_push_region(temp);
# 5074
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
const char*_tmp1407;if(Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,((_tmp1407="",_tag_dyneither(_tmp1407,sizeof(char),1))))!= 0){
struct _tuple32*_tmp140A;struct Cyc_List_List*_tmp1409;fields=((_tmp1409=_region_malloc(temp,sizeof(*_tmp1409)),((_tmp1409->hd=((_tmp140A=_region_malloc(temp,sizeof(*_tmp140A)),((_tmp140A->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,((_tmp140A->f2=0,_tmp140A)))))),((_tmp1409->tl=fields,_tmp1409))))));}}}
# 5079
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 5081
const char*_tmp140C;const char*_tmp140B;struct _dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?(_tmp140C="struct",_tag_dyneither(_tmp140C,sizeof(char),7)):((_tmp140B="union",_tag_dyneither(_tmp140B,sizeof(char),6)));
# 5084
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple33*_tmpCA5=(struct _tuple33*)des->hd;struct Cyc_List_List*_tmpCA7;void*_tmpCA8;struct _tuple33*_tmpCA6=_tmpCA5;_tmpCA7=_tmpCA6->f1;_tmpCA8=_tmpCA6->f2;
if(_tmpCA7 == 0){
# 5089
struct Cyc_List_List*_tmpCA9=fields;
for(0;_tmpCA9 != 0;_tmpCA9=_tmpCA9->tl){
if(!(*((struct _tuple32*)_tmpCA9->hd)).f2){
(*((struct _tuple32*)_tmpCA9->hd)).f2=1;
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp1412;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp1411;struct Cyc_List_List*_tmp1410;(*((struct _tuple33*)des->hd)).f1=((_tmp1410=_cycalloc(sizeof(*_tmp1410)),((_tmp1410->hd=(void*)((_tmp1412=_cycalloc(sizeof(*_tmp1412)),((_tmp1412[0]=((_tmp1411.tag=1,((_tmp1411.f1=((*((struct _tuple32*)_tmpCA9->hd)).f1)->name,_tmp1411)))),_tmp1412)))),((_tmp1410->tl=0,_tmp1410))))));}
{struct _tuple34*_tmp1415;struct Cyc_List_List*_tmp1414;ans=((_tmp1414=_region_malloc(rgn,sizeof(*_tmp1414)),((_tmp1414->hd=((_tmp1415=_region_malloc(rgn,sizeof(*_tmp1415)),((_tmp1415->f1=(*((struct _tuple32*)_tmpCA9->hd)).f1,((_tmp1415->f2=_tmpCA8,_tmp1415)))))),((_tmp1414->tl=ans,_tmp1414))))));}
break;}}
# 5097
if(_tmpCA9 == 0){
const char*_tmp1419;void*_tmp1418[1];struct Cyc_String_pa_PrintArg_struct _tmp1417;(_tmp1417.tag=0,((_tmp1417.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp1418[0]=& _tmp1417,Cyc_Tcutil_terr(loc,((_tmp1419="too many arguments to %s",_tag_dyneither(_tmp1419,sizeof(char),25))),_tag_dyneither(_tmp1418,sizeof(void*),1)))))));}}else{
if(_tmpCA7->tl != 0){
# 5101
const char*_tmp141C;void*_tmp141B;(_tmp141B=0,Cyc_Tcutil_terr(loc,((_tmp141C="multiple designators are not yet supported",_tag_dyneither(_tmp141C,sizeof(char),43))),_tag_dyneither(_tmp141B,sizeof(void*),0)));}else{
# 5104
void*_tmpCB4=(void*)_tmpCA7->hd;void*_tmpCB5=_tmpCB4;struct _dyneither_ptr*_tmpCB8;_LL720: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpCB6=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpCB5;if(_tmpCB6->tag != 0)goto _LL722;}_LL721:
# 5106
{const char*_tmp1420;void*_tmp141F[1];struct Cyc_String_pa_PrintArg_struct _tmp141E;(_tmp141E.tag=0,((_tmp141E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp141F[0]=& _tmp141E,Cyc_Tcutil_terr(loc,((_tmp1420="array designator used in argument to %s",_tag_dyneither(_tmp1420,sizeof(char),40))),_tag_dyneither(_tmp141F,sizeof(void*),1)))))));}
goto _LL71F;_LL722: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpCB7=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpCB5;if(_tmpCB7->tag != 1)goto _LL71F;else{_tmpCB8=_tmpCB7->f1;}}_LL723: {
# 5109
struct Cyc_List_List*_tmpCBC=fields;
for(0;_tmpCBC != 0;_tmpCBC=_tmpCBC->tl){
if(Cyc_strptrcmp(_tmpCB8,((*((struct _tuple32*)_tmpCBC->hd)).f1)->name)== 0){
if((*((struct _tuple32*)_tmpCBC->hd)).f2){
const char*_tmp1424;void*_tmp1423[1];struct Cyc_String_pa_PrintArg_struct _tmp1422;(_tmp1422.tag=0,((_tmp1422.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpCB8),((_tmp1423[0]=& _tmp1422,Cyc_Tcutil_terr(loc,((_tmp1424="member %s has already been used as an argument",_tag_dyneither(_tmp1424,sizeof(char),47))),_tag_dyneither(_tmp1423,sizeof(void*),1)))))));}
(*((struct _tuple32*)_tmpCBC->hd)).f2=1;
{struct _tuple34*_tmp1427;struct Cyc_List_List*_tmp1426;ans=((_tmp1426=_region_malloc(rgn,sizeof(*_tmp1426)),((_tmp1426->hd=((_tmp1427=_region_malloc(rgn,sizeof(*_tmp1427)),((_tmp1427->f1=(*((struct _tuple32*)_tmpCBC->hd)).f1,((_tmp1427->f2=_tmpCA8,_tmp1427)))))),((_tmp1426->tl=ans,_tmp1426))))));}
break;}}
# 5118
if(_tmpCBC == 0){
const char*_tmp142B;void*_tmp142A[1];struct Cyc_String_pa_PrintArg_struct _tmp1429;(_tmp1429.tag=0,((_tmp1429.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpCB8),((_tmp142A[0]=& _tmp1429,Cyc_Tcutil_terr(loc,((_tmp142B="bad field designator %s",_tag_dyneither(_tmp142B,sizeof(char),24))),_tag_dyneither(_tmp142A,sizeof(void*),1)))))));}
goto _LL71F;}_LL71F:;}}}
# 5123
if(aggr_kind == Cyc_Absyn_StructA)
# 5125
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple32*)fields->hd)).f2){
{const char*_tmp142E;void*_tmp142D;(_tmp142D=0,Cyc_Tcutil_terr(loc,((_tmp142E="too few arguments to struct",_tag_dyneither(_tmp142E,sizeof(char),28))),_tag_dyneither(_tmp142D,sizeof(void*),0)));}
break;}}else{
# 5132
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple32*)fields->hd)).f2){
if(found){const char*_tmp1431;void*_tmp1430;(_tmp1430=0,Cyc_Tcutil_terr(loc,((_tmp1431="only one member of a union is allowed",_tag_dyneither(_tmp1431,sizeof(char),38))),_tag_dyneither(_tmp1430,sizeof(void*),0)));}
found=1;}}
# 5139
if(!found){const char*_tmp1434;void*_tmp1433;(_tmp1433=0,Cyc_Tcutil_terr(loc,((_tmp1434="missing member for union",_tag_dyneither(_tmp1434,sizeof(char),25))),_tag_dyneither(_tmp1433,sizeof(void*),0)));}}{
# 5142
struct Cyc_List_List*_tmpCCB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0);return _tmpCCB;};};}
# 5074
;_pop_region(temp);}
# 5145
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmpCCE=Cyc_Tcutil_compress(t);void*_tmpCCF=_tmpCCE;void*_tmpCD1;union Cyc_Absyn_Constraint*_tmpCD2;_LL725: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCD0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCCF;if(_tmpCD0->tag != 5)goto _LL727;else{_tmpCD1=(_tmpCD0->f1).elt_typ;_tmpCD2=((_tmpCD0->f1).ptr_atts).bounds;}}_LL726: {
# 5148
void*_tmpCD3=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpCD2);void*_tmpCD4=_tmpCD3;_LL72A: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpCD5=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpCD4;if(_tmpCD5->tag != 0)goto _LL72C;}_LL72B:
# 5150
*elt_typ_dest=_tmpCD1;
return 1;_LL72C:;_LL72D:
 return 0;_LL729:;}_LL727:;_LL728:
# 5154
 return 0;_LL724:;}
# 5158
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmpCD6=Cyc_Tcutil_compress(t);void*_tmpCD7=_tmpCD6;void*_tmpCD9;union Cyc_Absyn_Constraint*_tmpCDA;_LL72F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCD8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCD7;if(_tmpCD8->tag != 5)goto _LL731;else{_tmpCD9=(_tmpCD8->f1).elt_typ;_tmpCDA=((_tmpCD8->f1).ptr_atts).zero_term;}}_LL730:
# 5161
*elt_typ_dest=_tmpCD9;
return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpCDA);_LL731:;_LL732:
 return 0;_LL72E:;}
# 5169
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type){
# 5171
void*_tmpCDB=Cyc_Tcutil_compress(t);void*_tmpCDC=_tmpCDB;void*_tmpCDE;union Cyc_Absyn_Constraint*_tmpCDF;union Cyc_Absyn_Constraint*_tmpCE0;void*_tmpCE2;struct Cyc_Absyn_Tqual _tmpCE3;struct Cyc_Absyn_Exp*_tmpCE4;union Cyc_Absyn_Constraint*_tmpCE5;_LL734: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCDD=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCDC;if(_tmpCDD->tag != 5)goto _LL736;else{_tmpCDE=(_tmpCDD->f1).elt_typ;_tmpCDF=((_tmpCDD->f1).ptr_atts).bounds;_tmpCE0=((_tmpCDD->f1).ptr_atts).zero_term;}}_LL735:
# 5173
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpCE0)){
*ptr_type=t;
*elt_type=_tmpCDE;
{void*_tmpCE6=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmpCDF);void*_tmpCE7=_tmpCE6;_LL73B: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpCE8=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpCE7;if(_tmpCE8->tag != 0)goto _LL73D;}_LL73C:
*is_dyneither=1;goto _LL73A;_LL73D:;_LL73E:
*is_dyneither=0;goto _LL73A;_LL73A:;}
# 5180
return 1;}else{
return 0;}_LL736: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCE1=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpCDC;if(_tmpCE1->tag != 8)goto _LL738;else{_tmpCE2=(_tmpCE1->f1).elt_type;_tmpCE3=(_tmpCE1->f1).tq;_tmpCE4=(_tmpCE1->f1).num_elts;_tmpCE5=(_tmpCE1->f1).zero_term;}}_LL737:
# 5183
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpCE5)){
*elt_type=_tmpCE2;
*is_dyneither=0;
*ptr_type=Cyc_Tcutil_promote_array(t,(void*)& Cyc_Absyn_HeapRgn_val,0);
return 1;}else{
return 0;}_LL738:;_LL739:
 return 0;_LL733:;}
# 5196
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){
# 5198
void*_tmpCE9=e1->r;void*_tmpCEA=_tmpCE9;struct Cyc_Absyn_Exp*_tmpCED;struct Cyc_Absyn_Exp*_tmpCEF;struct Cyc_Absyn_Exp*_tmpCF1;struct Cyc_Absyn_Exp*_tmpCF3;struct Cyc_Absyn_Exp*_tmpCF5;struct Cyc_Absyn_Exp*_tmpCF7;_LL740: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpCEB=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpCEA;if(_tmpCEB->tag != 13)goto _LL742;}_LL741: {
# 5200
const char*_tmp1438;void*_tmp1437[1];struct Cyc_String_pa_PrintArg_struct _tmp1436;(_tmp1436.tag=0,((_tmp1436.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp1437[0]=& _tmp1436,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1438="we have a cast in a lhs:  %s",_tag_dyneither(_tmp1438,sizeof(char),29))),_tag_dyneither(_tmp1437,sizeof(void*),1)))))));}_LL742: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpCEC=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpCEA;if(_tmpCEC->tag != 19)goto _LL744;else{_tmpCED=_tmpCEC->f1;}}_LL743:
 _tmpCEF=_tmpCED;goto _LL745;_LL744: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpCEE=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpCEA;if(_tmpCEE->tag != 22)goto _LL746;else{_tmpCEF=_tmpCEE->f1;}}_LL745:
# 5203
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpCEF->topt),ptr_type,is_dyneither,elt_type);_LL746: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpCF0=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpCEA;if(_tmpCF0->tag != 21)goto _LL748;else{_tmpCF1=_tmpCF0->f1;}}_LL747:
 _tmpCF3=_tmpCF1;goto _LL749;_LL748: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpCF2=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpCEA;if(_tmpCF2->tag != 20)goto _LL74A;else{_tmpCF3=_tmpCF2->f1;}}_LL749:
# 5207
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpCF3->topt),ptr_type,is_dyneither,elt_type)){
const char*_tmp143C;void*_tmp143B[1];struct Cyc_String_pa_PrintArg_struct _tmp143A;(_tmp143A.tag=0,((_tmp143A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1)),((_tmp143B[0]=& _tmp143A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp143C="found zero pointer aggregate member assignment: %s",_tag_dyneither(_tmp143C,sizeof(char),51))),_tag_dyneither(_tmp143B,sizeof(void*),1)))))));}
return 0;_LL74A: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpCF4=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpCEA;if(_tmpCF4->tag != 12)goto _LL74C;else{_tmpCF5=_tmpCF4->f1;}}_LL74B:
 _tmpCF7=_tmpCF5;goto _LL74D;_LL74C: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpCF6=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpCEA;if(_tmpCF6->tag != 11)goto _LL74E;else{_tmpCF7=_tmpCF6->f1;}}_LL74D:
# 5213
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpCF7->topt),ptr_type,is_dyneither,elt_type)){
const char*_tmp1440;void*_tmp143F[1];struct Cyc_String_pa_PrintArg_struct _tmp143E;(_tmp143E.tag=0,((_tmp143E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1)),((_tmp143F[0]=& _tmp143E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1440="found zero pointer instantiate/noinstantiate: %s",_tag_dyneither(_tmp1440,sizeof(char),49))),_tag_dyneither(_tmp143F,sizeof(void*),1)))))));}
return 0;_LL74E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpCF8=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpCEA;if(_tmpCF8->tag != 1)goto _LL750;}_LL74F:
 return 0;_LL750:;_LL751: {
# 5219
const char*_tmp1444;void*_tmp1443[1];struct Cyc_String_pa_PrintArg_struct _tmp1442;(_tmp1442.tag=0,((_tmp1442.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp1443[0]=& _tmp1442,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1444="found bad lhs in is_zero_ptr_deref: %s",_tag_dyneither(_tmp1444,sizeof(char),39))),_tag_dyneither(_tmp1443,sizeof(void*),1)))))));}_LL73F:;}
# 5223
int Cyc_Tcutil_is_tagged_pointer_typ(void*t){
void*ignore=(void*)& Cyc_Absyn_VoidType_val;
return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
# 5234
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
void*_tmpD05=Cyc_Tcutil_compress(r);void*_tmpD06=_tmpD05;struct Cyc_Absyn_Tvar*_tmpD0A;_LL753: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpD07=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmpD06;if(_tmpD07->tag != 22)goto _LL755;}_LL754:
 return !must_be_unique;_LL755: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpD08=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmpD06;if(_tmpD08->tag != 21)goto _LL757;}_LL756:
 return 1;_LL757: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpD09=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpD06;if(_tmpD09->tag != 2)goto _LL759;else{_tmpD0A=_tmpD09->f1;}}_LL758: {
# 5239
struct Cyc_Absyn_Kind*_tmpD0B=Cyc_Tcutil_tvar_kind(_tmpD0A,& Cyc_Tcutil_rk);enum Cyc_Absyn_KindQual _tmpD0D;enum Cyc_Absyn_AliasQual _tmpD0E;struct Cyc_Absyn_Kind*_tmpD0C=_tmpD0B;_tmpD0D=_tmpD0C->kind;_tmpD0E=_tmpD0C->aliasqual;
if(_tmpD0D == Cyc_Absyn_RgnKind  && (_tmpD0E == Cyc_Absyn_Unique  || _tmpD0E == Cyc_Absyn_Top  && !must_be_unique)){
void*_tmpD0F=Cyc_Absyn_compress_kb(_tmpD0A->kind);void*_tmpD10=_tmpD0F;struct Cyc_Core_Opt**_tmpD12;_LL75C: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpD11=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpD10;if(_tmpD11->tag != 2)goto _LL75E;else{_tmpD12=(struct Cyc_Core_Opt**)& _tmpD11->f1;if((_tmpD11->f2)->kind != Cyc_Absyn_RgnKind)goto _LL75E;if((_tmpD11->f2)->aliasqual != Cyc_Absyn_Top)goto _LL75E;}}_LL75D:
# 5243
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp144A;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1449;struct Cyc_Core_Opt*_tmp1448;*_tmpD12=((_tmp1448=_cycalloc(sizeof(*_tmp1448)),((_tmp1448->v=(void*)((_tmp144A=_cycalloc(sizeof(*_tmp144A)),((_tmp144A[0]=((_tmp1449.tag=2,((_tmp1449.f1=0,((_tmp1449.f2=& Cyc_Tcutil_rk,_tmp1449)))))),_tmp144A)))),_tmp1448))));}
return 0;_LL75E:;_LL75F:
 return 1;_LL75B:;}
# 5248
return 0;}_LL759:;_LL75A:
 return 0;_LL752:;}
# 5255
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmpD16=Cyc_Tcutil_compress(t);void*_tmpD17=_tmpD16;void*_tmpD19;struct Cyc_Absyn_Tvar*_tmpD1B;_LL761: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD18=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD17;if(_tmpD18->tag != 5)goto _LL763;else{_tmpD19=((_tmpD18->f1).ptr_atts).rgn;}}_LL762:
# 5258
 return Cyc_Tcutil_is_noalias_region(_tmpD19,must_be_unique);_LL763: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpD1A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpD17;if(_tmpD1A->tag != 2)goto _LL765;else{_tmpD1B=_tmpD1A->f1;}}_LL764: {
# 5260
struct Cyc_Absyn_Kind*_tmpD1C=Cyc_Tcutil_tvar_kind(_tmpD1B,& Cyc_Tcutil_bk);enum Cyc_Absyn_KindQual _tmpD1E;enum Cyc_Absyn_AliasQual _tmpD1F;struct Cyc_Absyn_Kind*_tmpD1D=_tmpD1C;_tmpD1E=_tmpD1D->kind;_tmpD1F=_tmpD1D->aliasqual;
switch(_tmpD1E){case Cyc_Absyn_BoxKind: _LL767:
 goto _LL768;case Cyc_Absyn_AnyKind: _LL768: goto _LL769;case Cyc_Absyn_MemKind: _LL769:
 if(_tmpD1F == Cyc_Absyn_Unique  || _tmpD1F == Cyc_Absyn_Top  && !must_be_unique){
void*_tmpD20=Cyc_Absyn_compress_kb(_tmpD1B->kind);void*_tmpD21=_tmpD20;struct Cyc_Core_Opt**_tmpD23;enum Cyc_Absyn_KindQual _tmpD24;_LL76C: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpD22=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpD21;if(_tmpD22->tag != 2)goto _LL76E;else{_tmpD23=(struct Cyc_Core_Opt**)& _tmpD22->f1;_tmpD24=(_tmpD22->f2)->kind;if((_tmpD22->f2)->aliasqual != Cyc_Absyn_Top)goto _LL76E;}}_LL76D:
# 5266
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1454;struct Cyc_Absyn_Kind*_tmp1453;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1452;struct Cyc_Core_Opt*_tmp1451;*_tmpD23=((_tmp1451=_cycalloc(sizeof(*_tmp1451)),((_tmp1451->v=(void*)((_tmp1454=_cycalloc(sizeof(*_tmp1454)),((_tmp1454[0]=((_tmp1452.tag=2,((_tmp1452.f1=0,((_tmp1452.f2=((_tmp1453=_cycalloc_atomic(sizeof(*_tmp1453)),((_tmp1453->kind=_tmpD24,((_tmp1453->aliasqual=Cyc_Absyn_Aliasable,_tmp1453)))))),_tmp1452)))))),_tmp1454)))),_tmp1451))));}
return 0;_LL76E:;_LL76F:
# 5270
 return 1;_LL76B:;}
# 5273
return 0;default: _LL76A:
 return 0;}}_LL765:;_LL766:
# 5276
 return 0;_LL760:;}
# 5279
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*rgn,void*t){
void*_tmpD29=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(_tmpD29,0))return 1;{
void*_tmpD2A=_tmpD29;struct Cyc_List_List*_tmpD2C;struct Cyc_Absyn_Aggrdecl**_tmpD2E;struct Cyc_List_List*_tmpD2F;struct Cyc_List_List*_tmpD31;union Cyc_Absyn_DatatypeInfoU _tmpD34;struct Cyc_List_List*_tmpD35;union Cyc_Absyn_DatatypeFieldInfoU _tmpD37;struct Cyc_List_List*_tmpD38;_LL772: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpD2B=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpD2A;if(_tmpD2B->tag != 10)goto _LL774;else{_tmpD2C=_tmpD2B->f1;}}_LL773:
# 5284
 while(_tmpD2C != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(*((struct _tuple12*)_tmpD2C->hd)).f2))return 1;
_tmpD2C=_tmpD2C->tl;}
# 5288
return 0;_LL774: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD2D=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpD2A;if(_tmpD2D->tag != 11)goto _LL776;else{if((((_tmpD2D->f1).aggr_info).KnownAggr).tag != 2)goto _LL776;_tmpD2E=(struct Cyc_Absyn_Aggrdecl**)(((_tmpD2D->f1).aggr_info).KnownAggr).val;_tmpD2F=(_tmpD2D->f1).targs;}}_LL775:
# 5290
 if((*_tmpD2E)->impl == 0)return 0;else{
# 5292
struct Cyc_List_List*_tmpD39=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,(*_tmpD2E)->tvs,_tmpD2F);
struct Cyc_List_List*_tmpD3A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpD2E)->impl))->fields;
void*t;
while(_tmpD3A != 0){
t=_tmpD39 == 0?((struct Cyc_Absyn_Aggrfield*)_tmpD3A->hd)->type: Cyc_Tcutil_rsubstitute(rgn,_tmpD39,((struct Cyc_Absyn_Aggrfield*)_tmpD3A->hd)->type);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpD3A=_tmpD3A->tl;}
# 5300
return 0;}_LL776: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpD30=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpD2A;if(_tmpD30->tag != 12)goto _LL778;else{_tmpD31=_tmpD30->f2;}}_LL777:
# 5303
 while(_tmpD31 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,((struct Cyc_Absyn_Aggrfield*)_tmpD31->hd)->type))return 1;
_tmpD31=_tmpD31->tl;}
# 5307
return 0;_LL778: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD32=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpD2A;if(_tmpD32->tag != 11)goto _LL77A;else{if((((_tmpD32->f1).aggr_info).UnknownAggr).tag != 1)goto _LL77A;}}_LL779:
# 5310
 return 0;_LL77A: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpD33=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpD2A;if(_tmpD33->tag != 3)goto _LL77C;else{_tmpD34=(_tmpD33->f1).datatype_info;_tmpD35=(_tmpD33->f1).targs;}}_LL77B: {
# 5312
union Cyc_Absyn_DatatypeInfoU _tmpD3B=_tmpD34;struct _tuple2*_tmpD3C;int _tmpD3D;struct Cyc_List_List*_tmpD3E;struct Cyc_Core_Opt*_tmpD3F;_LL781: if((_tmpD3B.UnknownDatatype).tag != 1)goto _LL783;_tmpD3C=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpD3B.UnknownDatatype).val).name;_tmpD3D=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpD3B.UnknownDatatype).val).is_extensible;_LL782:
# 5315
 return 0;_LL783: if((_tmpD3B.KnownDatatype).tag != 2)goto _LL780;_tmpD3E=(*((struct Cyc_Absyn_Datatypedecl**)(_tmpD3B.KnownDatatype).val))->tvs;_tmpD3F=(*((struct Cyc_Absyn_Datatypedecl**)(_tmpD3B.KnownDatatype).val))->fields;_LL784:
# 5318
 return 0;_LL780:;}_LL77C: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpD36=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpD2A;if(_tmpD36->tag != 4)goto _LL77E;else{_tmpD37=(_tmpD36->f1).field_info;_tmpD38=(_tmpD36->f1).targs;}}_LL77D: {
# 5321
union Cyc_Absyn_DatatypeFieldInfoU _tmpD40=_tmpD37;struct Cyc_Absyn_Datatypedecl*_tmpD41;struct Cyc_Absyn_Datatypefield*_tmpD42;_LL786: if((_tmpD40.UnknownDatatypefield).tag != 1)goto _LL788;_LL787:
# 5324
 return 0;_LL788: if((_tmpD40.KnownDatatypefield).tag != 2)goto _LL785;_tmpD41=((struct _tuple3)(_tmpD40.KnownDatatypefield).val).f1;_tmpD42=((struct _tuple3)(_tmpD40.KnownDatatypefield).val).f2;_LL789: {
# 5326
struct Cyc_List_List*_tmpD43=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpD41->tvs,_tmpD38);
struct Cyc_List_List*_tmpD44=_tmpD42->typs;
while(_tmpD44 != 0){
_tmpD29=_tmpD43 == 0?(*((struct _tuple12*)_tmpD44->hd)).f2: Cyc_Tcutil_rsubstitute(rgn,_tmpD43,(*((struct _tuple12*)_tmpD44->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,_tmpD29))return 1;
_tmpD44=_tmpD44->tl;}
# 5333
return 0;}_LL785:;}_LL77E:;_LL77F:
# 5335
 return 0;_LL771:;};}
# 5339
static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,struct _dyneither_ptr*f){
void*_tmpD45=Cyc_Tcutil_compress(t);void*_tmpD46=_tmpD45;struct Cyc_Absyn_Aggrdecl*_tmpD48;struct Cyc_List_List*_tmpD49;struct Cyc_List_List*_tmpD4B;_LL78B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD47=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpD46;if(_tmpD47->tag != 11)goto _LL78D;else{if((((_tmpD47->f1).aggr_info).KnownAggr).tag != 2)goto _LL78D;_tmpD48=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpD47->f1).aggr_info).KnownAggr).val);_tmpD49=(_tmpD47->f1).targs;}}_LL78C:
# 5342
 _tmpD4B=_tmpD48->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD48->impl))->fields;goto _LL78E;_LL78D: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpD4A=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpD46;if(_tmpD4A->tag != 12)goto _LL78F;else{_tmpD4B=_tmpD4A->f2;}}_LL78E: {
# 5344
struct Cyc_Absyn_Aggrfield*_tmpD4C=Cyc_Absyn_lookup_field(_tmpD4B,f);
{struct Cyc_Absyn_Aggrfield*_tmpD4D=_tmpD4C;void*_tmpD4E;_LL792: if(_tmpD4D != 0)goto _LL794;_LL793: {
const char*_tmp1457;void*_tmp1456;(_tmp1456=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1457="is_noalias_field: missing field",_tag_dyneither(_tmp1457,sizeof(char),32))),_tag_dyneither(_tmp1456,sizeof(void*),0)));}_LL794: if(_tmpD4D == 0)goto _LL791;_tmpD4E=_tmpD4D->type;_LL795:
# 5348
 return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpD4E);_LL791:;}
# 5350
return 0;}_LL78F:;_LL790: {
# 5352
const char*_tmp145B;void*_tmp145A[1];struct Cyc_String_pa_PrintArg_struct _tmp1459;(_tmp1459.tag=0,((_tmp1459.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp145A[0]=& _tmp1459,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp145B="is_noalias_field: invalid type |%s|",_tag_dyneither(_tmp145B,sizeof(char),36))),_tag_dyneither(_tmp145A,sizeof(void*),1)))))));}_LL78A:;}
# 5360
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
void*_tmpD54=e->r;void*_tmpD55=_tmpD54;struct Cyc_Absyn_Exp*_tmpD59;struct Cyc_Absyn_Exp*_tmpD5B;struct Cyc_Absyn_Exp*_tmpD5D;struct _dyneither_ptr*_tmpD5E;struct Cyc_Absyn_Exp*_tmpD60;struct Cyc_Absyn_Exp*_tmpD61;struct Cyc_Absyn_Exp*_tmpD63;struct Cyc_Absyn_Exp*_tmpD64;struct Cyc_Absyn_Exp*_tmpD66;struct Cyc_Absyn_Exp*_tmpD68;struct Cyc_Absyn_Stmt*_tmpD6A;_LL797: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpD56=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD55;if(_tmpD56->tag != 1)goto _LL799;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpD57=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmpD56->f2);if(_tmpD57->tag != 1)goto _LL799;}}_LL798:
 return 0;_LL799: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpD58=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpD55;if(_tmpD58->tag != 21)goto _LL79B;else{_tmpD59=_tmpD58->f1;}}_LL79A:
 _tmpD5B=_tmpD59;goto _LL79C;_LL79B: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpD5A=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpD55;if(_tmpD5A->tag != 19)goto _LL79D;else{_tmpD5B=_tmpD5A->f1;}}_LL79C:
# 5365
 return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmpD5B->topt),1) && 
Cyc_Tcutil_is_noalias_path(r,_tmpD5B);_LL79D: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpD5C=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpD55;if(_tmpD5C->tag != 20)goto _LL79F;else{_tmpD5D=_tmpD5C->f1;_tmpD5E=_tmpD5C->f2;}}_LL79E:
 return Cyc_Tcutil_is_noalias_path(r,_tmpD5D);_LL79F: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpD5F=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpD55;if(_tmpD5F->tag != 22)goto _LL7A1;else{_tmpD60=_tmpD5F->f1;_tmpD61=_tmpD5F->f2;}}_LL7A0: {
# 5369
void*_tmpD6B=Cyc_Tcutil_compress((void*)_check_null(_tmpD60->topt));void*_tmpD6C=_tmpD6B;_LL7AC: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpD6D=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpD6C;if(_tmpD6D->tag != 10)goto _LL7AE;}_LL7AD:
 return Cyc_Tcutil_is_noalias_path(r,_tmpD60);_LL7AE:;_LL7AF:
 return 0;_LL7AB:;}_LL7A1: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpD62=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD55;if(_tmpD62->tag != 5)goto _LL7A3;else{_tmpD63=_tmpD62->f2;_tmpD64=_tmpD62->f3;}}_LL7A2:
# 5374
 return Cyc_Tcutil_is_noalias_path(r,_tmpD63) && Cyc_Tcutil_is_noalias_path(r,_tmpD64);_LL7A3: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpD65=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpD55;if(_tmpD65->tag != 8)goto _LL7A5;else{_tmpD66=_tmpD65->f2;}}_LL7A4:
 _tmpD68=_tmpD66;goto _LL7A6;_LL7A5: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpD67=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD55;if(_tmpD67->tag != 13)goto _LL7A7;else{_tmpD68=_tmpD67->f2;}}_LL7A6:
 return Cyc_Tcutil_is_noalias_path(r,_tmpD68);_LL7A7: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmpD69=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpD55;if(_tmpD69->tag != 36)goto _LL7A9;else{_tmpD6A=_tmpD69->f1;}}_LL7A8:
# 5378
 while(1){
void*_tmpD6E=_tmpD6A->r;void*_tmpD6F=_tmpD6E;struct Cyc_Absyn_Stmt*_tmpD71;struct Cyc_Absyn_Stmt*_tmpD72;struct Cyc_Absyn_Decl*_tmpD74;struct Cyc_Absyn_Stmt*_tmpD75;struct Cyc_Absyn_Exp*_tmpD77;_LL7B1: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpD70=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpD6F;if(_tmpD70->tag != 2)goto _LL7B3;else{_tmpD71=_tmpD70->f1;_tmpD72=_tmpD70->f2;}}_LL7B2:
 _tmpD6A=_tmpD72;goto _LL7B0;_LL7B3: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpD73=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpD6F;if(_tmpD73->tag != 12)goto _LL7B5;else{_tmpD74=_tmpD73->f1;_tmpD75=_tmpD73->f2;}}_LL7B4:
 _tmpD6A=_tmpD75;goto _LL7B0;_LL7B5: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpD76=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpD6F;if(_tmpD76->tag != 1)goto _LL7B7;else{_tmpD77=_tmpD76->f1;}}_LL7B6:
 return Cyc_Tcutil_is_noalias_path(r,_tmpD77);_LL7B7:;_LL7B8: {
const char*_tmp145E;void*_tmp145D;(_tmp145D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp145E="is_noalias_stmt_exp: ill-formed StmtExp",_tag_dyneither(_tmp145E,sizeof(char),40))),_tag_dyneither(_tmp145D,sizeof(void*),0)));}_LL7B0:;}_LL7A9:;_LL7AA:
# 5386
 return 1;_LL796:;}
# 5403 "tcutil.cyc"
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5405
struct _tuple18 _tmp145F;struct _tuple18 bogus_ans=(_tmp145F.f1=0,((_tmp145F.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp145F)));
void*_tmpD7A=e->r;void*_tmpD7B=_tmpD7A;struct _tuple2*_tmpD7D;void*_tmpD7E;struct Cyc_Absyn_Exp*_tmpD80;struct _dyneither_ptr*_tmpD81;int _tmpD82;struct Cyc_Absyn_Exp*_tmpD84;struct _dyneither_ptr*_tmpD85;int _tmpD86;struct Cyc_Absyn_Exp*_tmpD88;struct Cyc_Absyn_Exp*_tmpD8A;struct Cyc_Absyn_Exp*_tmpD8B;_LL7BA: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpD7C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD7B;if(_tmpD7C->tag != 1)goto _LL7BC;else{_tmpD7D=_tmpD7C->f1;_tmpD7E=(void*)_tmpD7C->f2;}}_LL7BB: {
# 5409
void*_tmpD8C=_tmpD7E;struct Cyc_Absyn_Vardecl*_tmpD90;struct Cyc_Absyn_Vardecl*_tmpD92;struct Cyc_Absyn_Vardecl*_tmpD94;struct Cyc_Absyn_Vardecl*_tmpD96;_LL7C7: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpD8D=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpD8C;if(_tmpD8D->tag != 0)goto _LL7C9;}_LL7C8:
 goto _LL7CA;_LL7C9: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpD8E=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpD8C;if(_tmpD8E->tag != 2)goto _LL7CB;}_LL7CA:
 return bogus_ans;_LL7CB: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpD8F=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpD8C;if(_tmpD8F->tag != 1)goto _LL7CD;else{_tmpD90=_tmpD8F->f1;}}_LL7CC: {
# 5413
void*_tmpD97=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpD98=_tmpD97;_LL7D4: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD99=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD98;if(_tmpD99->tag != 8)goto _LL7D6;}_LL7D5: {
# 5415
struct _tuple18 _tmp1460;return(_tmp1460.f1=1,((_tmp1460.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp1460)));}_LL7D6:;_LL7D7: {
struct _tuple18 _tmp1461;return(_tmp1461.f1=(_tmpD90->tq).real_const,((_tmp1461.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp1461)));}_LL7D3:;}_LL7CD: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD91=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpD8C;if(_tmpD91->tag != 4)goto _LL7CF;else{_tmpD92=_tmpD91->f1;}}_LL7CE: {
# 5419
void*_tmpD9C=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpD9D=_tmpD9C;_LL7D9: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD9E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD9D;if(_tmpD9E->tag != 8)goto _LL7DB;}_LL7DA: {
struct _tuple18 _tmp1462;return(_tmp1462.f1=1,((_tmp1462.f2=(void*)_check_null(_tmpD92->rgn),_tmp1462)));}_LL7DB:;_LL7DC:
# 5422
 _tmpD92->escapes=1;{
struct _tuple18 _tmp1463;return(_tmp1463.f1=(_tmpD92->tq).real_const,((_tmp1463.f2=(void*)_check_null(_tmpD92->rgn),_tmp1463)));};_LL7D8:;}_LL7CF: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpD93=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpD8C;if(_tmpD93->tag != 5)goto _LL7D1;else{_tmpD94=_tmpD93->f1;}}_LL7D0:
# 5425
 _tmpD96=_tmpD94;goto _LL7D2;_LL7D1: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpD95=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpD8C;if(_tmpD95->tag != 3)goto _LL7C6;else{_tmpD96=_tmpD95->f1;}}_LL7D2:
# 5427
 _tmpD96->escapes=1;{
struct _tuple18 _tmp1464;return(_tmp1464.f1=(_tmpD96->tq).real_const,((_tmp1464.f2=(void*)_check_null(_tmpD96->rgn),_tmp1464)));};_LL7C6:;}_LL7BC: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpD7F=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpD7B;if(_tmpD7F->tag != 20)goto _LL7BE;else{_tmpD80=_tmpD7F->f1;_tmpD81=_tmpD7F->f2;_tmpD82=_tmpD7F->f3;}}_LL7BD:
# 5432
 if(_tmpD82)return bogus_ans;{
# 5435
void*_tmpDA2=Cyc_Tcutil_compress((void*)_check_null(_tmpD80->topt));void*_tmpDA3=_tmpDA2;struct Cyc_List_List*_tmpDA5;struct Cyc_Absyn_Aggrdecl*_tmpDA7;_LL7DE: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpDA4=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpDA3;if(_tmpDA4->tag != 12)goto _LL7E0;else{_tmpDA5=_tmpDA4->f2;}}_LL7DF: {
# 5437
struct Cyc_Absyn_Aggrfield*_tmpDA8=Cyc_Absyn_lookup_field(_tmpDA5,_tmpD81);
if(_tmpDA8 != 0  && _tmpDA8->width == 0){
struct _tuple18 _tmpDA9=Cyc_Tcutil_addressof_props(te,_tmpD80);int _tmpDAB;void*_tmpDAC;struct _tuple18 _tmpDAA=_tmpDA9;_tmpDAB=_tmpDAA.f1;_tmpDAC=_tmpDAA.f2;{
struct _tuple18 _tmp1465;return(_tmp1465.f1=(_tmpDA8->tq).real_const  || _tmpDAB,((_tmp1465.f2=_tmpDAC,_tmp1465)));};}
# 5442
return bogus_ans;}_LL7E0: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpDA6=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpDA3;if(_tmpDA6->tag != 11)goto _LL7E2;else{if((((_tmpDA6->f1).aggr_info).KnownAggr).tag != 2)goto _LL7E2;_tmpDA7=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpDA6->f1).aggr_info).KnownAggr).val);}}_LL7E1: {
# 5444
struct Cyc_Absyn_Aggrfield*_tmpDAE=Cyc_Absyn_lookup_decl_field(_tmpDA7,_tmpD81);
if(_tmpDAE != 0  && _tmpDAE->width == 0){
struct _tuple18 _tmpDAF=Cyc_Tcutil_addressof_props(te,_tmpD80);int _tmpDB1;void*_tmpDB2;struct _tuple18 _tmpDB0=_tmpDAF;_tmpDB1=_tmpDB0.f1;_tmpDB2=_tmpDB0.f2;{
struct _tuple18 _tmp1466;return(_tmp1466.f1=(_tmpDAE->tq).real_const  || _tmpDB1,((_tmp1466.f2=_tmpDB2,_tmp1466)));};}
# 5449
return bogus_ans;}_LL7E2:;_LL7E3:
 return bogus_ans;_LL7DD:;};_LL7BE: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpD83=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpD7B;if(_tmpD83->tag != 21)goto _LL7C0;else{_tmpD84=_tmpD83->f1;_tmpD85=_tmpD83->f2;_tmpD86=_tmpD83->f3;}}_LL7BF:
# 5454
 if(_tmpD86)return bogus_ans;{
# 5458
void*_tmpDB4=Cyc_Tcutil_compress((void*)_check_null(_tmpD84->topt));void*_tmpDB5=_tmpDB4;void*_tmpDB7;void*_tmpDB8;_LL7E5: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpDB6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpDB5;if(_tmpDB6->tag != 5)goto _LL7E7;else{_tmpDB7=(_tmpDB6->f1).elt_typ;_tmpDB8=((_tmpDB6->f1).ptr_atts).rgn;}}_LL7E6: {
# 5460
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmpDB9=Cyc_Tcutil_compress(_tmpDB7);void*_tmpDBA=_tmpDB9;struct Cyc_List_List*_tmpDBC;struct Cyc_Absyn_Aggrdecl*_tmpDBE;_LL7EA: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpDBB=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpDBA;if(_tmpDBB->tag != 12)goto _LL7EC;else{_tmpDBC=_tmpDBB->f2;}}_LL7EB:
# 5463
 finfo=Cyc_Absyn_lookup_field(_tmpDBC,_tmpD85);goto _LL7E9;_LL7EC: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpDBD=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpDBA;if(_tmpDBD->tag != 11)goto _LL7EE;else{if((((_tmpDBD->f1).aggr_info).KnownAggr).tag != 2)goto _LL7EE;_tmpDBE=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpDBD->f1).aggr_info).KnownAggr).val);}}_LL7ED:
# 5465
 finfo=Cyc_Absyn_lookup_decl_field(_tmpDBE,_tmpD85);goto _LL7E9;_LL7EE:;_LL7EF:
 return bogus_ans;_LL7E9:;}
# 5468
if(finfo != 0  && finfo->width == 0){
struct _tuple18 _tmp1467;return(_tmp1467.f1=(finfo->tq).real_const,((_tmp1467.f2=_tmpDB8,_tmp1467)));}
return bogus_ans;}_LL7E7:;_LL7E8:
 return bogus_ans;_LL7E4:;};_LL7C0: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpD87=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpD7B;if(_tmpD87->tag != 19)goto _LL7C2;else{_tmpD88=_tmpD87->f1;}}_LL7C1: {
# 5475
void*_tmpDC0=Cyc_Tcutil_compress((void*)_check_null(_tmpD88->topt));void*_tmpDC1=_tmpDC0;struct Cyc_Absyn_Tqual _tmpDC3;void*_tmpDC4;_LL7F1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpDC2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpDC1;if(_tmpDC2->tag != 5)goto _LL7F3;else{_tmpDC3=(_tmpDC2->f1).elt_tq;_tmpDC4=((_tmpDC2->f1).ptr_atts).rgn;}}_LL7F2: {
# 5477
struct _tuple18 _tmp1468;return(_tmp1468.f1=_tmpDC3.real_const,((_tmp1468.f2=_tmpDC4,_tmp1468)));}_LL7F3:;_LL7F4:
 return bogus_ans;_LL7F0:;}_LL7C2: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpD89=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpD7B;if(_tmpD89->tag != 22)goto _LL7C4;else{_tmpD8A=_tmpD89->f1;_tmpD8B=_tmpD89->f2;}}_LL7C3: {
# 5483
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmpD8A->topt));
void*_tmpDC6=t;struct Cyc_List_List*_tmpDC8;struct Cyc_Absyn_Tqual _tmpDCA;void*_tmpDCB;struct Cyc_Absyn_Tqual _tmpDCD;_LL7F6: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpDC7=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpDC6;if(_tmpDC7->tag != 10)goto _LL7F8;else{_tmpDC8=_tmpDC7->f1;}}_LL7F7: {
# 5487
struct _tuple14 _tmpDCE=Cyc_Evexp_eval_const_uint_exp(_tmpD8B);unsigned int _tmpDD0;int _tmpDD1;struct _tuple14 _tmpDCF=_tmpDCE;_tmpDD0=_tmpDCF.f1;_tmpDD1=_tmpDCF.f2;
if(!_tmpDD1)
return bogus_ans;{
struct _tuple12*_tmpDD2=Cyc_Absyn_lookup_tuple_field(_tmpDC8,(int)_tmpDD0);
if(_tmpDD2 != 0){
struct _tuple18 _tmp1469;return(_tmp1469.f1=((*_tmpDD2).f1).real_const,((_tmp1469.f2=(Cyc_Tcutil_addressof_props(te,_tmpD8A)).f2,_tmp1469)));}
return bogus_ans;};}_LL7F8: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpDC9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpDC6;if(_tmpDC9->tag != 5)goto _LL7FA;else{_tmpDCA=(_tmpDC9->f1).elt_tq;_tmpDCB=((_tmpDC9->f1).ptr_atts).rgn;}}_LL7F9: {
# 5495
struct _tuple18 _tmp146A;return(_tmp146A.f1=_tmpDCA.real_const,((_tmp146A.f2=_tmpDCB,_tmp146A)));}_LL7FA: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpDCC=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpDC6;if(_tmpDCC->tag != 8)goto _LL7FC;else{_tmpDCD=(_tmpDCC->f1).tq;}}_LL7FB: {
# 5501
struct _tuple18 _tmp146B;return(_tmp146B.f1=_tmpDCD.real_const,((_tmp146B.f2=(Cyc_Tcutil_addressof_props(te,_tmpD8A)).f2,_tmp146B)));}_LL7FC:;_LL7FD:
 return bogus_ans;_LL7F5:;}_LL7C4:;_LL7C5:
# 5505
{const char*_tmp146E;void*_tmp146D;(_tmp146D=0,Cyc_Tcutil_terr(e->loc,((_tmp146E="unary & applied to non-lvalue",_tag_dyneither(_tmp146E,sizeof(char),30))),_tag_dyneither(_tmp146D,sizeof(void*),0)));}
return bogus_ans;_LL7B9:;}
# 5512
void Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,union Cyc_Absyn_Constraint*b){
b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(b);{
void*_tmpDD9=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);void*_tmpDDA=_tmpDD9;struct Cyc_Absyn_Exp*_tmpDDD;_LL7FF: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpDDB=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpDDA;if(_tmpDDB->tag != 0)goto _LL801;}_LL800:
 return;_LL801: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpDDC=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpDDA;if(_tmpDDC->tag != 1)goto _LL7FE;else{_tmpDDD=_tmpDDC->f1;}}_LL802: {
# 5517
struct _tuple14 _tmpDDE=Cyc_Evexp_eval_const_uint_exp(_tmpDDD);unsigned int _tmpDE0;int _tmpDE1;struct _tuple14 _tmpDDF=_tmpDDE;_tmpDE0=_tmpDDF.f1;_tmpDE1=_tmpDDF.f2;
if(_tmpDE1  && _tmpDE0 <= i){
const char*_tmp1473;void*_tmp1472[2];struct Cyc_Int_pa_PrintArg_struct _tmp1471;struct Cyc_Int_pa_PrintArg_struct _tmp1470;(_tmp1470.tag=1,((_tmp1470.f1=(unsigned long)((int)i),((_tmp1471.tag=1,((_tmp1471.f1=(unsigned long)((int)_tmpDE0),((_tmp1472[0]=& _tmp1471,((_tmp1472[1]=& _tmp1470,Cyc_Tcutil_terr(loc,((_tmp1473="dereference is out of bounds: %d <= %d",_tag_dyneither(_tmp1473,sizeof(char),39))),_tag_dyneither(_tmp1472,sizeof(void*),2)))))))))))));}
return;}_LL7FE:;};}
# 5524
void Cyc_Tcutil_check_nonzero_bound(unsigned int loc,union Cyc_Absyn_Constraint*b){
Cyc_Tcutil_check_bound(loc,0,b);}
# 5528
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b){
void*_tmpDE6=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);void*_tmpDE7=_tmpDE6;struct Cyc_Absyn_Exp*_tmpDE9;_LL804: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpDE8=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpDE7;if(_tmpDE8->tag != 1)goto _LL806;else{_tmpDE9=_tmpDE8->f1;}}_LL805: {
# 5531
struct _tuple14 _tmpDEA=Cyc_Evexp_eval_const_uint_exp(_tmpDE9);unsigned int _tmpDEC;int _tmpDED;struct _tuple14 _tmpDEB=_tmpDEA;_tmpDEC=_tmpDEB.f1;_tmpDED=_tmpDEB.f2;
return _tmpDED  && _tmpDEC == 1;}_LL806:;_LL807:
 return 0;_LL803:;}
# 5537
int Cyc_Tcutil_bits_only(void*t){
void*_tmpDEE=Cyc_Tcutil_compress(t);void*_tmpDEF=_tmpDEE;void*_tmpDF6;union Cyc_Absyn_Constraint*_tmpDF7;struct Cyc_List_List*_tmpDF9;struct Cyc_Absyn_Aggrdecl*_tmpDFC;struct Cyc_List_List*_tmpDFD;struct Cyc_List_List*_tmpDFF;_LL809: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpDF0=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpDEF;if(_tmpDF0->tag != 0)goto _LL80B;}_LL80A:
 goto _LL80C;_LL80B: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpDF1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpDEF;if(_tmpDF1->tag != 6)goto _LL80D;}_LL80C:
 goto _LL80E;_LL80D: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpDF2=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpDEF;if(_tmpDF2->tag != 7)goto _LL80F;}_LL80E:
 return 1;_LL80F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpDF3=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpDEF;if(_tmpDF3->tag != 13)goto _LL811;}_LL810:
 goto _LL812;_LL811: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpDF4=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpDEF;if(_tmpDF4->tag != 14)goto _LL813;}_LL812:
 return 0;_LL813: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpDF5=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpDEF;if(_tmpDF5->tag != 8)goto _LL815;else{_tmpDF6=(_tmpDF5->f1).elt_type;_tmpDF7=(_tmpDF5->f1).zero_term;}}_LL814:
# 5547
 return !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpDF7) && Cyc_Tcutil_bits_only(_tmpDF6);_LL815: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpDF8=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpDEF;if(_tmpDF8->tag != 10)goto _LL817;else{_tmpDF9=_tmpDF8->f1;}}_LL816:
# 5549
 for(0;_tmpDF9 != 0;_tmpDF9=_tmpDF9->tl){
if(!Cyc_Tcutil_bits_only((*((struct _tuple12*)_tmpDF9->hd)).f2))return 0;}
return 1;_LL817: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpDFA=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpDEF;if(_tmpDFA->tag != 11)goto _LL819;else{if((((_tmpDFA->f1).aggr_info).UnknownAggr).tag != 1)goto _LL819;}}_LL818:
 return 0;_LL819: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpDFB=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpDEF;if(_tmpDFB->tag != 11)goto _LL81B;else{if((((_tmpDFB->f1).aggr_info).KnownAggr).tag != 2)goto _LL81B;_tmpDFC=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpDFB->f1).aggr_info).KnownAggr).val);_tmpDFD=(_tmpDFB->f1).targs;}}_LL81A:
# 5554
 if(_tmpDFC->impl == 0)
return 0;{
int okay=1;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpDFC->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
if(okay)return 1;{
struct _RegionHandle _tmpE00=_new_region("rgn");struct _RegionHandle*rgn=& _tmpE00;_push_region(rgn);
{struct Cyc_List_List*_tmpE01=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpDFC->tvs,_tmpDFD);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpDFC->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpE01,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpE02=0;_npop_handler(0);return _tmpE02;}}}{
int _tmpE03=1;_npop_handler(0);return _tmpE03;};}
# 5561
;_pop_region(rgn);};};_LL81B: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpDFE=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpDEF;if(_tmpDFE->tag != 12)goto _LL81D;else{_tmpDFF=_tmpDFE->f2;}}_LL81C:
# 5567
 for(0;_tmpDFF != 0;_tmpDFF=_tmpDFF->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)_tmpDFF->hd)->type))return 0;}
return 1;_LL81D:;_LL81E:
 return 0;_LL808:;}
# 5575
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e){
while(1){
void*_tmpE04=e->r;void*_tmpE05=_tmpE04;struct Cyc_Absyn_Exp*_tmpE08;struct Cyc_Absyn_Exp*_tmpE0A;_LL820: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpE06=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpE05;if(_tmpE06->tag != 1)goto _LL822;}_LL821:
 return 1;_LL822: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpE07=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpE05;if(_tmpE07->tag != 11)goto _LL824;else{_tmpE08=_tmpE07->f1;}}_LL823:
 _tmpE0A=_tmpE08;goto _LL825;_LL824: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpE09=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpE05;if(_tmpE09->tag != 12)goto _LL826;else{_tmpE0A=_tmpE09->f1;}}_LL825:
 e=_tmpE0A;continue;_LL826:;_LL827:
# 5582
 return 0;_LL81F:;}}
# 5592
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmpE0B=e->r;void*_tmpE0C=_tmpE0B;struct _tuple2*_tmpE14;void*_tmpE15;struct Cyc_Absyn_Exp*_tmpE17;struct Cyc_Absyn_Exp*_tmpE18;struct Cyc_Absyn_Exp*_tmpE19;struct Cyc_Absyn_Exp*_tmpE1B;struct Cyc_Absyn_Exp*_tmpE1C;struct Cyc_Absyn_Exp*_tmpE1E;struct Cyc_Absyn_Exp*_tmpE20;void*_tmpE22;struct Cyc_Absyn_Exp*_tmpE23;void*_tmpE25;struct Cyc_Absyn_Exp*_tmpE26;struct Cyc_Absyn_Exp*_tmpE28;struct Cyc_Absyn_Exp*_tmpE2A;struct Cyc_Absyn_Exp*_tmpE2B;struct Cyc_Absyn_Exp*_tmpE2D;struct Cyc_List_List*_tmpE2F;struct Cyc_List_List*_tmpE31;struct Cyc_List_List*_tmpE33;enum Cyc_Absyn_Primop _tmpE35;struct Cyc_List_List*_tmpE36;struct Cyc_List_List*_tmpE38;struct Cyc_List_List*_tmpE3A;_LL829: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE0D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpE0C;if(_tmpE0D->tag != 0)goto _LL82B;}_LL82A:
 goto _LL82C;_LL82B: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpE0E=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpE0C;if(_tmpE0E->tag != 16)goto _LL82D;}_LL82C:
 goto _LL82E;_LL82D: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpE0F=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpE0C;if(_tmpE0F->tag != 17)goto _LL82F;}_LL82E:
 goto _LL830;_LL82F: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpE10=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpE0C;if(_tmpE10->tag != 18)goto _LL831;}_LL830:
 goto _LL832;_LL831: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpE11=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpE0C;if(_tmpE11->tag != 31)goto _LL833;}_LL832:
 goto _LL834;_LL833: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpE12=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpE0C;if(_tmpE12->tag != 32)goto _LL835;}_LL834:
 return 1;_LL835: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpE13=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpE0C;if(_tmpE13->tag != 1)goto _LL837;else{_tmpE14=_tmpE13->f1;_tmpE15=(void*)_tmpE13->f2;}}_LL836: {
# 5603
void*_tmpE3B=_tmpE15;struct Cyc_Absyn_Vardecl*_tmpE3E;struct Cyc_Absyn_Vardecl*_tmpE40;_LL858: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpE3C=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpE3B;if(_tmpE3C->tag != 2)goto _LL85A;}_LL859:
 return 1;_LL85A: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpE3D=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpE3B;if(_tmpE3D->tag != 1)goto _LL85C;else{_tmpE3E=_tmpE3D->f1;}}_LL85B: {
# 5606
void*_tmpE42=Cyc_Tcutil_compress(_tmpE3E->type);void*_tmpE43=_tmpE42;_LL863: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpE44=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE43;if(_tmpE44->tag != 8)goto _LL865;}_LL864:
 goto _LL866;_LL865: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE45=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE43;if(_tmpE45->tag != 9)goto _LL867;}_LL866:
 return 1;_LL867:;_LL868:
 return var_okay;_LL862:;}_LL85C: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpE3F=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpE3B;if(_tmpE3F->tag != 4)goto _LL85E;else{_tmpE40=_tmpE3F->f1;}}_LL85D:
# 5613
 if(_tmpE40->sc == Cyc_Absyn_Static){
void*_tmpE46=Cyc_Tcutil_compress(_tmpE40->type);void*_tmpE47=_tmpE46;_LL86A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpE48=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE47;if(_tmpE48->tag != 8)goto _LL86C;}_LL86B:
 goto _LL86D;_LL86C: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE49=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE47;if(_tmpE49->tag != 9)goto _LL86E;}_LL86D:
 return 1;_LL86E:;_LL86F:
 return var_okay;_LL869:;}else{
# 5620
return var_okay;}_LL85E: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpE41=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpE3B;if(_tmpE41->tag != 0)goto _LL860;}_LL85F:
 return 0;_LL860:;_LL861:
 return var_okay;_LL857:;}_LL837: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpE16=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpE0C;if(_tmpE16->tag != 5)goto _LL839;else{_tmpE17=_tmpE16->f1;_tmpE18=_tmpE16->f2;_tmpE19=_tmpE16->f3;}}_LL838:
# 5626
 return(Cyc_Tcutil_cnst_exp(0,_tmpE17) && 
Cyc_Tcutil_cnst_exp(0,_tmpE18)) && 
Cyc_Tcutil_cnst_exp(0,_tmpE19);_LL839: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpE1A=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpE0C;if(_tmpE1A->tag != 8)goto _LL83B;else{_tmpE1B=_tmpE1A->f1;_tmpE1C=_tmpE1A->f2;}}_LL83A:
# 5630
 return Cyc_Tcutil_cnst_exp(0,_tmpE1B) && Cyc_Tcutil_cnst_exp(0,_tmpE1C);_LL83B: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpE1D=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpE0C;if(_tmpE1D->tag != 11)goto _LL83D;else{_tmpE1E=_tmpE1D->f1;}}_LL83C:
 _tmpE20=_tmpE1E;goto _LL83E;_LL83D: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpE1F=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpE0C;if(_tmpE1F->tag != 12)goto _LL83F;else{_tmpE20=_tmpE1F->f1;}}_LL83E:
# 5633
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpE20);_LL83F: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpE21=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpE0C;if(_tmpE21->tag != 13)goto _LL841;else{_tmpE22=(void*)_tmpE21->f1;_tmpE23=_tmpE21->f2;if(_tmpE21->f4 != Cyc_Absyn_No_coercion)goto _LL841;}}_LL840:
# 5635
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpE23);_LL841: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpE24=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpE0C;if(_tmpE24->tag != 13)goto _LL843;else{_tmpE25=(void*)_tmpE24->f1;_tmpE26=_tmpE24->f2;}}_LL842:
# 5638
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpE26);_LL843: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmpE27=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpE0C;if(_tmpE27->tag != 14)goto _LL845;else{_tmpE28=_tmpE27->f1;}}_LL844:
# 5640
 return Cyc_Tcutil_cnst_exp(1,_tmpE28);_LL845: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpE29=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpE0C;if(_tmpE29->tag != 26)goto _LL847;else{_tmpE2A=_tmpE29->f2;_tmpE2B=_tmpE29->f3;}}_LL846:
# 5642
 return Cyc_Tcutil_cnst_exp(0,_tmpE2A) && Cyc_Tcutil_cnst_exp(0,_tmpE2B);_LL847: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpE2C=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpE0C;if(_tmpE2C->tag != 27)goto _LL849;else{_tmpE2D=_tmpE2C->f1;}}_LL848:
# 5644
 return Cyc_Tcutil_cnst_exp(0,_tmpE2D);_LL849: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpE2E=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpE0C;if(_tmpE2E->tag != 25)goto _LL84B;else{_tmpE2F=_tmpE2E->f1;}}_LL84A:
 _tmpE31=_tmpE2F;goto _LL84C;_LL84B: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpE30=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpE0C;if(_tmpE30->tag != 29)goto _LL84D;else{_tmpE31=_tmpE30->f2;}}_LL84C:
 _tmpE33=_tmpE31;goto _LL84E;_LL84D: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpE32=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpE0C;if(_tmpE32->tag != 28)goto _LL84F;else{_tmpE33=_tmpE32->f3;}}_LL84E:
# 5648
 for(0;_tmpE33 != 0;_tmpE33=_tmpE33->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple19*)_tmpE33->hd)).f2))
return 0;}
return 1;_LL84F: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpE34=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpE0C;if(_tmpE34->tag != 2)goto _LL851;else{_tmpE35=_tmpE34->f1;_tmpE36=_tmpE34->f2;}}_LL850:
# 5653
 _tmpE38=_tmpE36;goto _LL852;_LL851: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmpE37=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpE0C;if(_tmpE37->tag != 23)goto _LL853;else{_tmpE38=_tmpE37->f1;}}_LL852:
 _tmpE3A=_tmpE38;goto _LL854;_LL853: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpE39=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpE0C;if(_tmpE39->tag != 30)goto _LL855;else{_tmpE3A=_tmpE39->f1;}}_LL854:
# 5656
 for(0;_tmpE3A != 0;_tmpE3A=_tmpE3A->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmpE3A->hd))
return 0;}
return 1;_LL855:;_LL856:
 return 0;_LL828:;}
# 5664
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 5668
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);
# 5670
int Cyc_Tcutil_supports_default(void*t){
void*_tmpE4A=Cyc_Tcutil_compress(t);void*_tmpE4B=_tmpE4A;union Cyc_Absyn_Constraint*_tmpE50;union Cyc_Absyn_Constraint*_tmpE51;void*_tmpE53;struct Cyc_List_List*_tmpE55;union Cyc_Absyn_AggrInfoU _tmpE57;struct Cyc_List_List*_tmpE58;struct Cyc_List_List*_tmpE5A;_LL871: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpE4C=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpE4B;if(_tmpE4C->tag != 0)goto _LL873;}_LL872:
 goto _LL874;_LL873: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpE4D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpE4B;if(_tmpE4D->tag != 6)goto _LL875;}_LL874:
 goto _LL876;_LL875: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpE4E=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpE4B;if(_tmpE4E->tag != 7)goto _LL877;}_LL876:
 return 1;_LL877: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE4F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE4B;if(_tmpE4F->tag != 5)goto _LL879;else{_tmpE50=((_tmpE4F->f1).ptr_atts).nullable;_tmpE51=((_tmpE4F->f1).ptr_atts).bounds;}}_LL878: {
# 5677
void*_tmpE5D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpE51);void*_tmpE5E=_tmpE5D;_LL888: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpE5F=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpE5E;if(_tmpE5F->tag != 0)goto _LL88A;}_LL889:
 return 1;_LL88A:;_LL88B:
# 5680
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmpE50);_LL887:;}_LL879: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpE52=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE4B;if(_tmpE52->tag != 8)goto _LL87B;else{_tmpE53=(_tmpE52->f1).elt_type;}}_LL87A:
# 5683
 return Cyc_Tcutil_supports_default(_tmpE53);_LL87B: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpE54=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpE4B;if(_tmpE54->tag != 10)goto _LL87D;else{_tmpE55=_tmpE54->f1;}}_LL87C:
# 5685
 for(0;_tmpE55 != 0;_tmpE55=_tmpE55->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple12*)_tmpE55->hd)).f2))return 0;}
return 1;_LL87D: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpE56=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpE4B;if(_tmpE56->tag != 11)goto _LL87F;else{_tmpE57=(_tmpE56->f1).aggr_info;_tmpE58=(_tmpE56->f1).targs;}}_LL87E: {
# 5689
struct Cyc_Absyn_Aggrdecl*_tmpE60=Cyc_Absyn_get_known_aggrdecl(_tmpE57);
if(_tmpE60->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpE60->impl))->exist_vars != 0)return 0;
return Cyc_Tcutil_fields_support_default(_tmpE60->tvs,_tmpE58,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpE60->impl))->fields);}_LL87F: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpE59=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpE4B;if(_tmpE59->tag != 12)goto _LL881;else{_tmpE5A=_tmpE59->f2;}}_LL880:
 return Cyc_Tcutil_fields_support_default(0,0,_tmpE5A);_LL881: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpE5B=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpE4B;if(_tmpE5B->tag != 14)goto _LL883;}_LL882:
# 5695
 goto _LL884;_LL883: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpE5C=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpE4B;if(_tmpE5C->tag != 13)goto _LL885;}_LL884:
 return 1;_LL885:;_LL886:
# 5698
 return 0;_LL870:;}
# 5703
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t){
void*_tmpE61=t;struct Cyc_Absyn_Typedefdecl*_tmpE63;_LL88D: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpE62=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpE61;if(_tmpE62->tag != 17)goto _LL88F;else{_tmpE63=_tmpE62->f3;}}_LL88E:
# 5706
 if(_tmpE63 != 0){
struct Cyc_Absyn_Tqual _tmpE64=_tmpE63->tq;
if(((_tmpE64.print_const  || _tmpE64.q_volatile) || _tmpE64.q_restrict) || _tmpE64.real_const){
# 5711
const char*_tmp1477;void*_tmp1476[1];struct Cyc_String_pa_PrintArg_struct _tmp1475;(_tmp1475.tag=0,((_tmp1475.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1476[0]=& _tmp1475,Cyc_Tcutil_warn(loc,((_tmp1477="qualifier within typedef type %s is ignored",_tag_dyneither(_tmp1477,sizeof(char),44))),_tag_dyneither(_tmp1476,sizeof(void*),1)))))));}}
# 5714
goto _LL88C;_LL88F:;_LL890:
 goto _LL88C;_LL88C:;}
# 5719
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 5721
struct _RegionHandle _tmpE68=_new_region("rgn");struct _RegionHandle*rgn=& _tmpE68;_push_region(rgn);{
struct Cyc_List_List*_tmpE69=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_supports_default(t)){int _tmpE6A=1;_npop_handler(0);return _tmpE6A;}
t=Cyc_Tcutil_rsubstitute(rgn,_tmpE69,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpE6B=0;_npop_handler(0);return _tmpE6B;}}}{
# 5730
int _tmpE6C=1;_npop_handler(0);return _tmpE6C;};
# 5721
;_pop_region(rgn);}
# 5736
int Cyc_Tcutil_admits_zero(void*t){
void*_tmpE6D=Cyc_Tcutil_compress(t);void*_tmpE6E=_tmpE6D;union Cyc_Absyn_Constraint*_tmpE72;union Cyc_Absyn_Constraint*_tmpE73;_LL892: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpE6F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpE6E;if(_tmpE6F->tag != 6)goto _LL894;}_LL893:
 goto _LL895;_LL894: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpE70=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpE6E;if(_tmpE70->tag != 7)goto _LL896;}_LL895:
 return 1;_LL896: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE71=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE6E;if(_tmpE71->tag != 5)goto _LL898;else{_tmpE72=((_tmpE71->f1).ptr_atts).nullable;_tmpE73=((_tmpE71->f1).ptr_atts).bounds;}}_LL897: {
# 5741
void*_tmpE74=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpE73);void*_tmpE75=_tmpE74;_LL89B: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpE76=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpE75;if(_tmpE76->tag != 0)goto _LL89D;}_LL89C:
# 5745
 return 0;_LL89D:;_LL89E:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpE72);_LL89A:;}_LL898:;_LL899:
# 5748
 return 0;_LL891:;}
# 5752
int Cyc_Tcutil_is_noreturn(void*t){
{void*_tmpE77=Cyc_Tcutil_compress(t);void*_tmpE78=_tmpE77;void*_tmpE7A;struct Cyc_List_List*_tmpE7C;_LL8A0: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE79=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE78;if(_tmpE79->tag != 5)goto _LL8A2;else{_tmpE7A=(_tmpE79->f1).elt_typ;}}_LL8A1:
 return Cyc_Tcutil_is_noreturn(_tmpE7A);_LL8A2: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE7B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE78;if(_tmpE7B->tag != 9)goto _LL8A4;else{_tmpE7C=(_tmpE7B->f1).attributes;}}_LL8A3:
# 5756
 for(0;_tmpE7C != 0;_tmpE7C=_tmpE7C->tl){
void*_tmpE7D=(void*)_tmpE7C->hd;void*_tmpE7E=_tmpE7D;_LL8A7: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmpE7F=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmpE7E;if(_tmpE7F->tag != 4)goto _LL8A9;}_LL8A8:
 return 1;_LL8A9:;_LL8AA:
 continue;_LL8A6:;}
# 5761
goto _LL89F;_LL8A4:;_LL8A5:
 goto _LL89F;_LL89F:;}
# 5764
return 0;}
# 5769
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmpE80=Cyc_Tcutil_compress(t);void*_tmpE81=_tmpE80;struct Cyc_List_List**_tmpE83;_LL8AC: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE82=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE81;if(_tmpE82->tag != 9)goto _LL8AE;else{_tmpE83=(struct Cyc_List_List**)&(_tmpE82->f1).attributes;}}_LL8AD: {
# 5772
struct Cyc_List_List*_tmpE84=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Tcutil_attribute_cmp,*_tmpE83,(void*)atts->hd)){
struct Cyc_List_List*_tmp1478;*_tmpE83=((_tmp1478=_cycalloc(sizeof(*_tmp1478)),((_tmp1478->hd=(void*)atts->hd,((_tmp1478->tl=*_tmpE83,_tmp1478))))));}}else{
# 5779
struct Cyc_List_List*_tmp1479;_tmpE84=((_tmp1479=_cycalloc(sizeof(*_tmp1479)),((_tmp1479->hd=(void*)atts->hd,((_tmp1479->tl=_tmpE84,_tmp1479))))));}}
return _tmpE84;}_LL8AE:;_LL8AF: {
const char*_tmp147C;void*_tmp147B;(_tmp147B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp147C="transfer_fn_type_atts",_tag_dyneither(_tmp147C,sizeof(char),22))),_tag_dyneither(_tmp147B,sizeof(void*),0)));}_LL8AB:;}
# 5786
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
struct Cyc_Absyn_Exp*bound=0;
{void*_tmpE89=Cyc_Tcutil_compress(t);void*_tmpE8A=_tmpE89;struct Cyc_Absyn_PtrInfo*_tmpE8C;struct Cyc_Absyn_Exp*_tmpE8E;_LL8B1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE8B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE8A;if(_tmpE8B->tag != 5)goto _LL8B3;else{_tmpE8C=(struct Cyc_Absyn_PtrInfo*)& _tmpE8B->f1;}}_LL8B2:
# 5790
{void*_tmpE8F=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)((_tmpE8C->ptr_atts).bounds);void*_tmpE90=_tmpE8F;struct Cyc_Absyn_Exp*_tmpE92;_LL8B8: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpE91=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpE90;if(_tmpE91->tag != 1)goto _LL8BA;else{_tmpE92=_tmpE91->f1;}}_LL8B9:
 bound=_tmpE92;goto _LL8B7;_LL8BA:;_LL8BB:
 goto _LL8B7;_LL8B7:;}
# 5794
goto _LL8B0;_LL8B3: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpE8D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE8A;if(_tmpE8D->tag != 8)goto _LL8B5;else{_tmpE8E=(_tmpE8D->f1).num_elts;}}_LL8B4:
# 5796
 bound=_tmpE8E;
goto _LL8B0;_LL8B5:;_LL8B6:
 goto _LL8B0;_LL8B0:;}
# 5800
return bound;}
# 5805
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
{void*_tmpE93=b;struct Cyc_Absyn_Vardecl*_tmpE95;struct Cyc_Absyn_Vardecl*_tmpE97;struct Cyc_Absyn_Vardecl*_tmpE99;struct Cyc_Absyn_Vardecl*_tmpE9B;_LL8BD: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpE94=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpE93;if(_tmpE94->tag != 5)goto _LL8BF;else{_tmpE95=_tmpE94->f1;}}_LL8BE:
 _tmpE97=_tmpE95;goto _LL8C0;_LL8BF: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpE96=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpE93;if(_tmpE96->tag != 4)goto _LL8C1;else{_tmpE97=_tmpE96->f1;}}_LL8C0:
 _tmpE99=_tmpE97;goto _LL8C2;_LL8C1: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpE98=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpE93;if(_tmpE98->tag != 3)goto _LL8C3;else{_tmpE99=_tmpE98->f1;}}_LL8C2:
 _tmpE9B=_tmpE99;goto _LL8C4;_LL8C3: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpE9A=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpE93;if(_tmpE9A->tag != 1)goto _LL8C5;else{_tmpE9B=_tmpE9A->f1;}}_LL8C4:
# 5811
 if(!_tmpE9B->escapes)return _tmpE9B;
goto _LL8BC;_LL8C5:;_LL8C6:
 goto _LL8BC;_LL8BC:;}
# 5815
return 0;}
# 5819
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmpE9C=0;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0){struct Cyc_List_List*_tmp147D;_tmpE9C=((_tmp147D=_cycalloc(sizeof(*_tmp147D)),((_tmp147D->hd=*((void**)_check_null((void**)x->hd)),((_tmp147D->tl=_tmpE9C,_tmp147D))))));}}}
return _tmpE9C;}
# 5826
int Cyc_Tcutil_is_array(void*t){
# 5828
void*_tmpE9E=Cyc_Tcutil_compress(t);void*_tmpE9F=_tmpE9E;_LL8C8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpEA0=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE9F;if(_tmpEA0->tag != 8)goto _LL8CA;}_LL8C9:
# 5830
 return 1;_LL8CA:;_LL8CB:
 return 0;_LL8C7:;}
# 5835
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
# 5837
void*_tmpEA1=Cyc_Tcutil_compress(t);void*_tmpEA2=_tmpEA1;void*_tmpEA4;struct Cyc_Absyn_Tqual _tmpEA5;struct Cyc_Absyn_Exp*_tmpEA6;union Cyc_Absyn_Constraint*_tmpEA7;unsigned int _tmpEA8;_LL8CD: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpEA3=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpEA2;if(_tmpEA3->tag != 8)goto _LL8CF;else{_tmpEA4=(_tmpEA3->f1).elt_type;_tmpEA5=(_tmpEA3->f1).tq;_tmpEA6=(_tmpEA3->f1).num_elts;_tmpEA7=(_tmpEA3->f1).zero_term;_tmpEA8=(_tmpEA3->f1).zt_loc;}}_LL8CE: {
# 5839
void*b;
if(_tmpEA6 == 0)
b=(void*)& Cyc_Absyn_DynEither_b_val;else{
# 5843
struct Cyc_Absyn_Exp*bnd=_tmpEA6;
if(convert_tag){
if(bnd->topt == 0){
const char*_tmp1480;void*_tmp147F;(_tmp147F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1480="cannot convert tag without type!",_tag_dyneither(_tmp1480,sizeof(char),33))),_tag_dyneither(_tmp147F,sizeof(void*),0)));}{
void*_tmpEAB=Cyc_Tcutil_compress((void*)_check_null(bnd->topt));void*_tmpEAC=_tmpEAB;void*_tmpEAE;_LL8D2: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpEAD=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpEAC;if(_tmpEAD->tag != 19)goto _LL8D4;else{_tmpEAE=(void*)_tmpEAD->f1;}}_LL8D3:
# 5849
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp1483;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp1482;b=(void*)((_tmp1482=_cycalloc(sizeof(*_tmp1482)),((_tmp1482[0]=((_tmp1483.tag=1,((_tmp1483.f1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmpEAE,0),0,Cyc_Absyn_No_coercion,0),_tmp1483)))),_tmp1482))));}
goto _LL8D1;_LL8D4:;_LL8D5:
# 5852
 if(Cyc_Tcutil_is_const_exp(bnd)){
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp1486;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp1485;b=(void*)((_tmp1485=_cycalloc(sizeof(*_tmp1485)),((_tmp1485[0]=((_tmp1486.tag=1,((_tmp1486.f1=bnd,_tmp1486)))),_tmp1485))));}else{
# 5855
b=(void*)& Cyc_Absyn_DynEither_b_val;}_LL8D1:;};}else{
# 5859
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp1489;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp1488;b=(void*)((_tmp1488=_cycalloc(sizeof(*_tmp1488)),((_tmp1488[0]=((_tmp1489.tag=1,((_tmp1489.f1=bnd,_tmp1489)))),_tmp1488))));}}
# 5861
return Cyc_Absyn_atb_typ(_tmpEA4,rgn,_tmpEA5,b,_tmpEA7);}_LL8CF:;_LL8D0:
# 5864
 return t;_LL8CC:;}
# 5869
int Cyc_Tcutil_zeroable_type(void*t){
void*_tmpEB5=Cyc_Tcutil_compress(t);void*_tmpEB6=_tmpEB5;union Cyc_Absyn_Constraint*_tmpEBD;void*_tmpEC1;struct Cyc_List_List*_tmpEC4;union Cyc_Absyn_AggrInfoU _tmpEC6;struct Cyc_List_List*_tmpEC7;struct Cyc_List_List*_tmpECB;_LL8D7: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpEB7=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpEB6;if(_tmpEB7->tag != 0)goto _LL8D9;}_LL8D8:
 return 1;_LL8D9: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpEB8=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpEB6;if(_tmpEB8->tag != 1)goto _LL8DB;}_LL8DA:
 goto _LL8DC;_LL8DB: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpEB9=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpEB6;if(_tmpEB9->tag != 2)goto _LL8DD;}_LL8DC:
 goto _LL8DE;_LL8DD: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpEBA=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpEB6;if(_tmpEBA->tag != 3)goto _LL8DF;}_LL8DE:
 goto _LL8E0;_LL8DF: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpEBB=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpEB6;if(_tmpEBB->tag != 4)goto _LL8E1;}_LL8E0:
 return 0;_LL8E1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpEBC=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpEB6;if(_tmpEBC->tag != 5)goto _LL8E3;else{_tmpEBD=((_tmpEBC->f1).ptr_atts).nullable;}}_LL8E2:
# 5877
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmpEBD);_LL8E3: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpEBE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpEB6;if(_tmpEBE->tag != 6)goto _LL8E5;}_LL8E4:
 goto _LL8E6;_LL8E5: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpEBF=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpEB6;if(_tmpEBF->tag != 7)goto _LL8E7;}_LL8E6:
 return 1;_LL8E7: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpEC0=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpEB6;if(_tmpEC0->tag != 8)goto _LL8E9;else{_tmpEC1=(_tmpEC0->f1).elt_type;}}_LL8E8:
 return Cyc_Tcutil_zeroable_type(_tmpEC1);_LL8E9: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpEC2=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpEB6;if(_tmpEC2->tag != 9)goto _LL8EB;}_LL8EA:
 return 0;_LL8EB: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpEC3=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpEB6;if(_tmpEC3->tag != 10)goto _LL8ED;else{_tmpEC4=_tmpEC3->f1;}}_LL8EC:
# 5883
 for(0;(unsigned int)_tmpEC4;_tmpEC4=_tmpEC4->tl){
if(!Cyc_Tcutil_zeroable_type((*((struct _tuple12*)_tmpEC4->hd)).f2))return 0;}
return 1;_LL8ED: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpEC5=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpEB6;if(_tmpEC5->tag != 11)goto _LL8EF;else{_tmpEC6=(_tmpEC5->f1).aggr_info;_tmpEC7=(_tmpEC5->f1).targs;}}_LL8EE: {
# 5887
struct Cyc_Absyn_Aggrdecl*_tmpEDA=Cyc_Absyn_get_known_aggrdecl(_tmpEC6);
if(_tmpEDA->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpEDA->impl))->exist_vars != 0)
return 0;{
struct _RegionHandle _tmpEDB=_new_region("r");struct _RegionHandle*r=& _tmpEDB;_push_region(r);
{struct Cyc_List_List*_tmpEDC=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmpEDA->tvs,_tmpEC7);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpEDA->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmpEDC,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpEDD=0;_npop_handler(0);return _tmpEDD;}}}{
int _tmpEDE=1;_npop_handler(0);return _tmpEDE;};}
# 5891
;_pop_region(r);};}_LL8EF: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpEC8=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpEB6;if(_tmpEC8->tag != 13)goto _LL8F1;}_LL8F0:
# 5896
 return 1;_LL8F1: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpEC9=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpEB6;if(_tmpEC9->tag != 19)goto _LL8F3;}_LL8F2:
 return 1;_LL8F3: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpECA=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpEB6;if(_tmpECA->tag != 12)goto _LL8F5;else{_tmpECB=_tmpECA->f2;}}_LL8F4:
# 5899
 for(0;_tmpECB != 0;_tmpECB=_tmpECB->tl){
if(!Cyc_Tcutil_zeroable_type(((struct Cyc_Absyn_Aggrfield*)_tmpECB->hd)->type))return 0;}
return 1;_LL8F5: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpECC=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpEB6;if(_tmpECC->tag != 14)goto _LL8F7;}_LL8F6:
 return 1;_LL8F7: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpECD=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpEB6;if(_tmpECD->tag != 17)goto _LL8F9;}_LL8F8:
 return 0;_LL8F9: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpECE=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmpEB6;if(_tmpECE->tag != 16)goto _LL8FB;}_LL8FA:
 return 0;_LL8FB: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpECF=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpEB6;if(_tmpECF->tag != 15)goto _LL8FD;}_LL8FC:
 return 0;_LL8FD: {struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*_tmpED0=(struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmpEB6;if(_tmpED0->tag != 28)goto _LL8FF;}_LL8FE:
 return 0;_LL8FF: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpED1=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmpEB6;if(_tmpED1->tag != 26)goto _LL901;}_LL900:
# 5908
 goto _LL902;_LL901: {struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmpED2=(struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmpEB6;if(_tmpED2->tag != 27)goto _LL903;}_LL902:
 goto _LL904;_LL903: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpED3=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmpEB6;if(_tmpED3->tag != 18)goto _LL905;}_LL904:
 goto _LL906;_LL905: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmpED4=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmpEB6;if(_tmpED4->tag != 20)goto _LL907;}_LL906:
 goto _LL908;_LL907: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpED5=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmpEB6;if(_tmpED5->tag != 21)goto _LL909;}_LL908:
 goto _LL90A;_LL909: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpED6=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmpEB6;if(_tmpED6->tag != 22)goto _LL90B;}_LL90A:
 goto _LL90C;_LL90B: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpED7=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmpEB6;if(_tmpED7->tag != 23)goto _LL90D;}_LL90C:
 goto _LL90E;_LL90D: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpED8=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmpEB6;if(_tmpED8->tag != 24)goto _LL90F;}_LL90E:
 goto _LL910;_LL90F: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpED9=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmpEB6;if(_tmpED9->tag != 25)goto _LL8D6;}_LL910: {
const char*_tmp148D;void*_tmp148C[1];struct Cyc_String_pa_PrintArg_struct _tmp148B;(_tmp148B.tag=0,((_tmp148B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp148C[0]=& _tmp148B,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp148D="bad type `%s' in zeroable type",_tag_dyneither(_tmp148D,sizeof(char),31))),_tag_dyneither(_tmp148C,sizeof(void*),1)))))));}_LL8D6:;}
