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
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 444 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 460
typedef void*Cyc_Absyn_funcparams_t;
# 463
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 527
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 542
extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;
# 553
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 560
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 578
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
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
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple2*,void*,unsigned int);
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
{const char*_tmpEA9;if(Cyc_strcmp(((_tmpEA9="(qualifiers don't match)",_tag_dyneither(_tmpEA9,sizeof(char),25))),(struct _dyneither_ptr)Cyc_Tcutil_failure_reason)== 0){
{const char*_tmpEAD;void*_tmpEAC[1];struct Cyc_String_pa_PrintArg_struct _tmpEAB;(_tmpEAB.tag=0,((_tmpEAB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((_tmpEAC[0]=& _tmpEAB,Cyc_fprintf(Cyc_stderr,((_tmpEAD="  %s\n",_tag_dyneither(_tmpEAD,sizeof(char),6))),_tag_dyneither(_tmpEAC,sizeof(void*),1)))))));}
return;}}
# 75
{const char*_tmpEAE;if(Cyc_strcmp(((_tmpEAE="(function effects do not match)",_tag_dyneither(_tmpEAE,sizeof(char),32))),(struct _dyneither_ptr)Cyc_Tcutil_failure_reason)== 0){
struct Cyc_Absynpp_Params _tmp6=Cyc_Absynpp_tc_params_r;
_tmp6.print_all_effects=1;
Cyc_Absynpp_set_params(& _tmp6);}}{
# 80
struct _dyneither_ptr s1=Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure);
struct _dyneither_ptr s2=Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure);
int pos=2;
{const char*_tmpEB2;void*_tmpEB1[1];struct Cyc_String_pa_PrintArg_struct _tmpEB0;(_tmpEB0.tag=0,((_tmpEB0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmpEB1[0]=& _tmpEB0,Cyc_fprintf(Cyc_stderr,((_tmpEB2="  %s",_tag_dyneither(_tmpEB2,sizeof(char),5))),_tag_dyneither(_tmpEB1,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s1,sizeof(char));
if(pos + 5 >= 80){
{const char*_tmpEB5;void*_tmpEB4;(_tmpEB4=0,Cyc_fprintf(Cyc_stderr,((_tmpEB5="\n\t",_tag_dyneither(_tmpEB5,sizeof(char),3))),_tag_dyneither(_tmpEB4,sizeof(void*),0)));}
pos=8;}else{
# 89
{const char*_tmpEB8;void*_tmpEB7;(_tmpEB7=0,Cyc_fprintf(Cyc_stderr,((_tmpEB8=" ",_tag_dyneither(_tmpEB8,sizeof(char),2))),_tag_dyneither(_tmpEB7,sizeof(void*),0)));}
++ pos;}
# 92
{const char*_tmpEBB;void*_tmpEBA;(_tmpEBA=0,Cyc_fprintf(Cyc_stderr,((_tmpEBB="and ",_tag_dyneither(_tmpEBB,sizeof(char),5))),_tag_dyneither(_tmpEBA,sizeof(void*),0)));}
pos +=4;
if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){
{const char*_tmpEBE;void*_tmpEBD;(_tmpEBD=0,Cyc_fprintf(Cyc_stderr,((_tmpEBE="\n\t",_tag_dyneither(_tmpEBE,sizeof(char),3))),_tag_dyneither(_tmpEBD,sizeof(void*),0)));}
pos=8;}
# 98
{const char*_tmpEC2;void*_tmpEC1[1];struct Cyc_String_pa_PrintArg_struct _tmpEC0;(_tmpEC0.tag=0,((_tmpEC0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmpEC1[0]=& _tmpEC0,Cyc_fprintf(Cyc_stderr,((_tmpEC2="%s ",_tag_dyneither(_tmpEC2,sizeof(char),4))),_tag_dyneither(_tmpEC1,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s2,sizeof(char))+ 1;
if(pos + 17 >= 80){
{const char*_tmpEC5;void*_tmpEC4;(_tmpEC4=0,Cyc_fprintf(Cyc_stderr,((_tmpEC5="\n\t",_tag_dyneither(_tmpEC5,sizeof(char),3))),_tag_dyneither(_tmpEC4,sizeof(void*),0)));}
pos=8;}
# 104
{const char*_tmpEC8;void*_tmpEC7;(_tmpEC7=0,Cyc_fprintf(Cyc_stderr,((_tmpEC8="are not compatible. ",_tag_dyneither(_tmpEC8,sizeof(char),21))),_tag_dyneither(_tmpEC7,sizeof(void*),0)));}
pos +=17;
if((char*)Cyc_Tcutil_failure_reason.curr != (char*)(_tag_dyneither(0,0,0)).curr){
if(pos + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason)>= 80){
const char*_tmpECB;void*_tmpECA;(_tmpECA=0,Cyc_fprintf(Cyc_stderr,((_tmpECB="\n\t",_tag_dyneither(_tmpECB,sizeof(char),3))),_tag_dyneither(_tmpECA,sizeof(void*),0)));}{
# 110
const char*_tmpECF;void*_tmpECE[1];struct Cyc_String_pa_PrintArg_struct _tmpECD;(_tmpECD.tag=0,((_tmpECD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((_tmpECE[0]=& _tmpECD,Cyc_fprintf(Cyc_stderr,((_tmpECF="%s",_tag_dyneither(_tmpECF,sizeof(char),3))),_tag_dyneither(_tmpECE,sizeof(void*),1)))))));};}
# 112
{const char*_tmpED2;void*_tmpED1;(_tmpED1=0,Cyc_fprintf(Cyc_stderr,((_tmpED2="\n",_tag_dyneither(_tmpED2,sizeof(char),2))),_tag_dyneither(_tmpED1,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);};}
# 116
void Cyc_Tcutil_terr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 119
Cyc_Position_post_error(Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}
# 122
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 125
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
{const char*_tmpED6;void*_tmpED5[1];struct Cyc_String_pa_PrintArg_struct _tmpED4;(_tmpED4.tag=0,((_tmpED4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmpED5[0]=& _tmpED4,Cyc_fprintf(Cyc_stderr,((_tmpED6="Compiler Error (Tcutil::impos): %s\n",_tag_dyneither(_tmpED6,sizeof(char),36))),_tag_dyneither(_tmpED5,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Core_Impossible_exn_struct _tmpED9;struct Cyc_Core_Impossible_exn_struct*_tmpED8;(int)_throw((void*)((_tmpED8=_cycalloc(sizeof(*_tmpED8)),((_tmpED8[0]=((_tmpED9.tag=Cyc_Core_Impossible,((_tmpED9.f1=msg,_tmpED9)))),_tmpED8)))));};}char Cyc_Tcutil_AbortTypeCheckingFunction[26]="AbortTypeCheckingFunction";
# 133
static struct _dyneither_ptr Cyc_Tcutil_tvar2string(struct Cyc_Absyn_Tvar*tv){
return*tv->name;}
# 137
void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){
for(0;tvs != 0;tvs=tvs->tl){
const char*_tmpEDE;void*_tmpEDD[2];struct Cyc_String_pa_PrintArg_struct _tmpEDC;struct Cyc_String_pa_PrintArg_struct _tmpEDB;(_tmpEDB.tag=0,((_tmpEDB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((_tmpEDC.tag=0,((_tmpEDC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)tvs->hd)),((_tmpEDD[0]=& _tmpEDC,((_tmpEDD[1]=& _tmpEDB,Cyc_fprintf(Cyc_stderr,((_tmpEDE="%s::%s ",_tag_dyneither(_tmpEDE,sizeof(char),8))),_tag_dyneither(_tmpEDD,sizeof(void*),2)))))))))))));}
{const char*_tmpEE1;void*_tmpEE0;(_tmpEE0=0,Cyc_fprintf(Cyc_stderr,((_tmpEE1="\n",_tag_dyneither(_tmpEE1,sizeof(char),2))),_tag_dyneither(_tmpEE0,sizeof(void*),0)));}Cyc_fflush(Cyc_stderr);}
# 144
static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;
# 149
void Cyc_Tcutil_warn(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 152
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
{struct Cyc_List_List*_tmpEE2;Cyc_Tcutil_warning_segs=((_tmpEE2=_cycalloc(sizeof(*_tmpEE2)),((_tmpEE2->hd=(void*)sg,((_tmpEE2->tl=Cyc_Tcutil_warning_segs,_tmpEE2))))));}{
struct _dyneither_ptr*_tmpEE5;struct Cyc_List_List*_tmpEE4;Cyc_Tcutil_warning_msgs=((_tmpEE4=_cycalloc(sizeof(*_tmpEE4)),((_tmpEE4->hd=((_tmpEE5=_cycalloc(sizeof(*_tmpEE5)),((_tmpEE5[0]=msg,_tmpEE5)))),((_tmpEE4->tl=Cyc_Tcutil_warning_msgs,_tmpEE4))))));};}
# 156
void Cyc_Tcutil_flush_warnings(){
if(Cyc_Tcutil_warning_segs == 0)
return;
{const char*_tmpEE8;void*_tmpEE7;(_tmpEE7=0,Cyc_fprintf(Cyc_stderr,((_tmpEE8="***Warnings***\n",_tag_dyneither(_tmpEE8,sizeof(char),16))),_tag_dyneither(_tmpEE7,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp30=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);
Cyc_Tcutil_warning_segs=0;
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);
while(Cyc_Tcutil_warning_msgs != 0){
{const char*_tmpEED;void*_tmpEEC[2];struct Cyc_String_pa_PrintArg_struct _tmpEEB;struct Cyc_String_pa_PrintArg_struct _tmpEEA;(_tmpEEA.tag=0,((_tmpEEA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd)),((_tmpEEB.tag=0,((_tmpEEB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp30))->hd)),((_tmpEEC[0]=& _tmpEEB,((_tmpEEC[1]=& _tmpEEA,Cyc_fprintf(Cyc_stderr,((_tmpEED="%s: %s\n",_tag_dyneither(_tmpEED,sizeof(char),8))),_tag_dyneither(_tmpEEC,sizeof(void*),2)))))))))))));}
_tmp30=_tmp30->tl;
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}
# 168
{const char*_tmpEF0;void*_tmpEEF;(_tmpEEF=0,Cyc_fprintf(Cyc_stderr,((_tmpEF0="**************\n",_tag_dyneither(_tmpEF0,sizeof(char),16))),_tag_dyneither(_tmpEEF,sizeof(void*),0)));}
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
struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpEF3;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpEF2;return(void*)((_tmpEF2=_cycalloc(sizeof(*_tmpEF2)),((_tmpEF2[0]=((_tmpEF3.tag=1,((_tmpEF3.f1=0,_tmpEF3)))),_tmpEF2))));}_LL20: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp4F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4D;if(_tmp4F->tag != 2)goto _LL22;else{_tmp50=_tmp4F->f2;}}_LL21: {
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpEF6;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpEF5;return(void*)((_tmpEF5=_cycalloc(sizeof(*_tmpEF5)),((_tmpEF5[0]=((_tmpEF6.tag=2,((_tmpEF6.f1=0,((_tmpEF6.f2=_tmp50,_tmpEF6)))))),_tmpEF5))));}_LL22:;_LL23:
 return kb;_LL1D:;}
# 235
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 237
struct Cyc_Absyn_Tvar*_tmpEF7;return(_tmpEF7=_cycalloc(sizeof(*_tmpEF7)),((_tmpEF7->name=tv->name,((_tmpEF7->identity=- 1,((_tmpEF7->kind=Cyc_Tcutil_copy_kindbound(tv->kind),_tmpEF7)))))));}
# 239
static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
# 241
struct _dyneither_ptr*_tmp57;struct Cyc_Absyn_Tqual _tmp58;void*_tmp59;struct _tuple10*_tmp56=arg;_tmp57=_tmp56->f1;_tmp58=_tmp56->f2;_tmp59=_tmp56->f3;{
struct _tuple10*_tmpEF8;return(_tmpEF8=_cycalloc(sizeof(*_tmpEF8)),((_tmpEF8->f1=_tmp57,((_tmpEF8->f2=_tmp58,((_tmpEF8->f3=Cyc_Tcutil_copy_type(_tmp59),_tmpEF8)))))));};}
# 244
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
struct Cyc_Absyn_Tqual _tmp5C;void*_tmp5D;struct _tuple12*_tmp5B=arg;_tmp5C=_tmp5B->f1;_tmp5D=_tmp5B->f2;{
struct _tuple12*_tmpEF9;return(_tmpEF9=_cycalloc(sizeof(*_tmpEF9)),((_tmpEF9->f1=_tmp5C,((_tmpEF9->f2=Cyc_Tcutil_copy_type(_tmp5D),_tmpEF9)))));};}
# 248
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*);
# 250
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_Absyn_Aggrfield*_tmpEFA;return(_tmpEFA=_cycalloc(sizeof(*_tmpEFA)),((_tmpEFA->name=f->name,((_tmpEFA->tq=f->tq,((_tmpEFA->type=Cyc_Tcutil_copy_type(f->type),((_tmpEFA->width=f->width,((_tmpEFA->attributes=f->attributes,((_tmpEFA->requires_clause=
Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause),_tmpEFA)))))))))))));}
# 254
static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
void*_tmp61;void*_tmp62;struct _tuple0*_tmp60=x;_tmp61=_tmp60->f1;_tmp62=_tmp60->f2;{
struct _tuple0*_tmpEFB;return(_tmpEFB=_cycalloc(sizeof(*_tmpEFB)),((_tmpEFB->f1=Cyc_Tcutil_copy_type(_tmp61),((_tmpEFB->f2=Cyc_Tcutil_copy_type(_tmp62),_tmpEFB)))));};}
# 258
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){
struct Cyc_Absyn_Enumfield*_tmpEFC;return(_tmpEFC=_cycalloc(sizeof(*_tmpEFC)),((_tmpEFC->name=f->name,((_tmpEFC->tag=f->tag,((_tmpEFC->loc=f->loc,_tmpEFC)))))));}
# 261
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpEFF;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpEFE;return(void*)((_tmpEFE=_cycalloc(sizeof(*_tmpEFE)),((_tmpEFE[0]=((_tmpEFF.tag=2,((_tmpEFF.f1=Cyc_Tcutil_copy_tvar(t),_tmpEFF)))),_tmpEFE))));}
# 265
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp67=Cyc_Tcutil_compress(t);void*_tmp68=_tmp67;struct Cyc_Absyn_Tvar*_tmp6C;union Cyc_Absyn_DatatypeInfoU _tmp6E;struct Cyc_List_List*_tmp6F;union Cyc_Absyn_DatatypeFieldInfoU _tmp71;struct Cyc_List_List*_tmp72;void*_tmp74;struct Cyc_Absyn_Tqual _tmp75;void*_tmp76;union Cyc_Absyn_Constraint*_tmp77;union Cyc_Absyn_Constraint*_tmp78;union Cyc_Absyn_Constraint*_tmp79;struct Cyc_Absyn_PtrLoc*_tmp7A;void*_tmp7E;struct Cyc_Absyn_Tqual _tmp7F;struct Cyc_Absyn_Exp*_tmp80;union Cyc_Absyn_Constraint*_tmp81;unsigned int _tmp82;struct Cyc_List_List*_tmp84;void*_tmp85;struct Cyc_Absyn_Tqual _tmp86;void*_tmp87;struct Cyc_List_List*_tmp88;int _tmp89;struct Cyc_Absyn_VarargInfo*_tmp8A;struct Cyc_List_List*_tmp8B;struct Cyc_List_List*_tmp8C;struct Cyc_Absyn_Exp*_tmp8D;struct Cyc_List_List*_tmp8E;struct Cyc_Absyn_Exp*_tmp8F;struct Cyc_List_List*_tmp90;struct Cyc_List_List*_tmp92;enum Cyc_Absyn_AggrKind _tmp94;struct _tuple2*_tmp95;struct Cyc_Core_Opt*_tmp96;struct Cyc_List_List*_tmp97;struct Cyc_Absyn_Aggrdecl**_tmp99;struct Cyc_List_List*_tmp9A;enum Cyc_Absyn_AggrKind _tmp9C;struct Cyc_List_List*_tmp9D;struct _tuple2*_tmp9F;struct Cyc_Absyn_Enumdecl*_tmpA0;struct Cyc_List_List*_tmpA2;void*_tmpA4;struct Cyc_Absyn_Exp*_tmpA6;struct Cyc_Absyn_Exp*_tmpA8;void*_tmpAA;void*_tmpAC;void*_tmpAD;struct _tuple2*_tmpAF;struct Cyc_List_List*_tmpB0;struct Cyc_Absyn_Typedefdecl*_tmpB1;void*_tmpB6;struct Cyc_List_List*_tmpB8;void*_tmpBA;struct Cyc_Absyn_Aggrdecl*_tmpBD;struct Cyc_Absyn_Enumdecl*_tmpC0;struct Cyc_Absyn_Datatypedecl*_tmpC3;_LL25: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp69=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp68;if(_tmp69->tag != 0)goto _LL27;}_LL26:
 goto _LL28;_LL27: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp6A=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp68;if(_tmp6A->tag != 1)goto _LL29;}_LL28:
 return t;_LL29: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp6B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp68;if(_tmp6B->tag != 2)goto _LL2B;else{_tmp6C=_tmp6B->f1;}}_LL2A: {
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpF02;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpF01;return(void*)((_tmpF01=_cycalloc(sizeof(*_tmpF01)),((_tmpF01[0]=((_tmpF02.tag=2,((_tmpF02.f1=Cyc_Tcutil_copy_tvar(_tmp6C),_tmpF02)))),_tmpF01))));}_LL2B: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp6D=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp68;if(_tmp6D->tag != 3)goto _LL2D;else{_tmp6E=(_tmp6D->f1).datatype_info;_tmp6F=(_tmp6D->f1).targs;}}_LL2C: {
# 271
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpF08;struct Cyc_Absyn_DatatypeInfo _tmpF07;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpF06;return(void*)((_tmpF06=_cycalloc(sizeof(*_tmpF06)),((_tmpF06[0]=((_tmpF08.tag=3,((_tmpF08.f1=((_tmpF07.datatype_info=_tmp6E,((_tmpF07.targs=Cyc_Tcutil_copy_types(_tmp6F),_tmpF07)))),_tmpF08)))),_tmpF06))));}_LL2D: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp70=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp68;if(_tmp70->tag != 4)goto _LL2F;else{_tmp71=(_tmp70->f1).field_info;_tmp72=(_tmp70->f1).targs;}}_LL2E: {
# 273
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpF0E;struct Cyc_Absyn_DatatypeFieldInfo _tmpF0D;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpF0C;return(void*)((_tmpF0C=_cycalloc(sizeof(*_tmpF0C)),((_tmpF0C[0]=((_tmpF0E.tag=4,((_tmpF0E.f1=((_tmpF0D.field_info=_tmp71,((_tmpF0D.targs=Cyc_Tcutil_copy_types(_tmp72),_tmpF0D)))),_tmpF0E)))),_tmpF0C))));}_LL2F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp73=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp68;if(_tmp73->tag != 5)goto _LL31;else{_tmp74=(_tmp73->f1).elt_typ;_tmp75=(_tmp73->f1).elt_tq;_tmp76=((_tmp73->f1).ptr_atts).rgn;_tmp77=((_tmp73->f1).ptr_atts).nullable;_tmp78=((_tmp73->f1).ptr_atts).bounds;_tmp79=((_tmp73->f1).ptr_atts).zero_term;_tmp7A=((_tmp73->f1).ptr_atts).ptrloc;}}_LL30: {
# 275
void*_tmpCC=Cyc_Tcutil_copy_type(_tmp74);
void*_tmpCD=Cyc_Tcutil_copy_type(_tmp76);
union Cyc_Absyn_Constraint*_tmpCE=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp77);
struct Cyc_Absyn_Tqual _tmpCF=_tmp75;
# 280
union Cyc_Absyn_Constraint*_tmpD0=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp78);
union Cyc_Absyn_Constraint*_tmpD1=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp79);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpF14;struct Cyc_Absyn_PtrInfo _tmpF13;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpF12;return(void*)((_tmpF12=_cycalloc(sizeof(*_tmpF12)),((_tmpF12[0]=((_tmpF14.tag=5,((_tmpF14.f1=((_tmpF13.elt_typ=_tmpCC,((_tmpF13.elt_tq=_tmpCF,((_tmpF13.ptr_atts=(((_tmpF13.ptr_atts).rgn=_tmpCD,(((_tmpF13.ptr_atts).nullable=_tmpCE,(((_tmpF13.ptr_atts).bounds=_tmpD0,(((_tmpF13.ptr_atts).zero_term=_tmpD1,(((_tmpF13.ptr_atts).ptrloc=_tmp7A,_tmpF13.ptr_atts)))))))))),_tmpF13)))))),_tmpF14)))),_tmpF12))));}_LL31: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp68;if(_tmp7B->tag != 6)goto _LL33;}_LL32:
 goto _LL34;_LL33: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp68;if(_tmp7C->tag != 7)goto _LL35;}_LL34:
 return t;_LL35: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp7D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp68;if(_tmp7D->tag != 8)goto _LL37;else{_tmp7E=(_tmp7D->f1).elt_type;_tmp7F=(_tmp7D->f1).tq;_tmp80=(_tmp7D->f1).num_elts;_tmp81=(_tmp7D->f1).zero_term;_tmp82=(_tmp7D->f1).zt_loc;}}_LL36: {
# 286
struct Cyc_Absyn_Exp*eopt2=Cyc_Tcutil_deep_copy_exp_opt(1,_tmp80);
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpF1A;struct Cyc_Absyn_ArrayInfo _tmpF19;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpF18;return(void*)((_tmpF18=_cycalloc(sizeof(*_tmpF18)),((_tmpF18[0]=((_tmpF1A.tag=8,((_tmpF1A.f1=((_tmpF19.elt_type=Cyc_Tcutil_copy_type(_tmp7E),((_tmpF19.tq=_tmp7F,((_tmpF19.num_elts=eopt2,((_tmpF19.zero_term=
((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp81),((_tmpF19.zt_loc=_tmp82,_tmpF19)))))))))),_tmpF1A)))),_tmpF18))));}_LL37: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp83=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68;if(_tmp83->tag != 9)goto _LL39;else{_tmp84=(_tmp83->f1).tvars;_tmp85=(_tmp83->f1).effect;_tmp86=(_tmp83->f1).ret_tqual;_tmp87=(_tmp83->f1).ret_typ;_tmp88=(_tmp83->f1).args;_tmp89=(_tmp83->f1).c_varargs;_tmp8A=(_tmp83->f1).cyc_varargs;_tmp8B=(_tmp83->f1).rgn_po;_tmp8C=(_tmp83->f1).attributes;_tmp8D=(_tmp83->f1).requires_clause;_tmp8E=(_tmp83->f1).requires_relns;_tmp8F=(_tmp83->f1).ensures_clause;_tmp90=(_tmp83->f1).ensures_relns;}}_LL38: {
# 290
struct Cyc_List_List*_tmpD8=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp84);
void*effopt2=_tmp85 == 0?0: Cyc_Tcutil_copy_type(_tmp85);
void*_tmpD9=Cyc_Tcutil_copy_type(_tmp87);
struct Cyc_List_List*_tmpDA=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp88);
int _tmpDB=_tmp89;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(_tmp8A != 0){
struct Cyc_Absyn_VarargInfo*cv=_tmp8A;
struct Cyc_Absyn_VarargInfo*_tmpF1B;cyc_varargs2=((_tmpF1B=_cycalloc(sizeof(*_tmpF1B)),((_tmpF1B->name=cv->name,((_tmpF1B->tq=cv->tq,((_tmpF1B->type=Cyc_Tcutil_copy_type(cv->type),((_tmpF1B->inject=cv->inject,_tmpF1B))))))))));}{
# 301
struct Cyc_List_List*_tmpDD=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp8B);
struct Cyc_List_List*_tmpDE=_tmp8C;
struct Cyc_Absyn_Exp*_tmpDF=Cyc_Tcutil_deep_copy_exp_opt(1,_tmp8D);
struct Cyc_List_List*_tmpE0=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp8E);
struct Cyc_Absyn_Exp*_tmpE1=Cyc_Tcutil_deep_copy_exp_opt(1,_tmp8F);
struct Cyc_List_List*_tmpE2=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp90);
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpF21;struct Cyc_Absyn_FnInfo _tmpF20;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpF1F;return(void*)((_tmpF1F=_cycalloc(sizeof(*_tmpF1F)),((_tmpF1F[0]=((_tmpF21.tag=9,((_tmpF21.f1=((_tmpF20.tvars=_tmpD8,((_tmpF20.effect=effopt2,((_tmpF20.ret_tqual=_tmp86,((_tmpF20.ret_typ=_tmpD9,((_tmpF20.args=_tmpDA,((_tmpF20.c_varargs=_tmpDB,((_tmpF20.cyc_varargs=cyc_varargs2,((_tmpF20.rgn_po=_tmpDD,((_tmpF20.attributes=_tmpDE,((_tmpF20.requires_clause=_tmpDF,((_tmpF20.requires_relns=_tmpE0,((_tmpF20.ensures_clause=_tmpE1,((_tmpF20.ensures_relns=_tmpE2,_tmpF20)))))))))))))))))))))))))),_tmpF21)))),_tmpF1F))));};}_LL39: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp91=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp68;if(_tmp91->tag != 10)goto _LL3B;else{_tmp92=_tmp91->f1;}}_LL3A: {
# 310
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpF24;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpF23;return(void*)((_tmpF23=_cycalloc(sizeof(*_tmpF23)),((_tmpF23[0]=((_tmpF24.tag=10,((_tmpF24.f1=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp92),_tmpF24)))),_tmpF23))));}_LL3B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp93=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp68;if(_tmp93->tag != 11)goto _LL3D;else{if((((_tmp93->f1).aggr_info).UnknownAggr).tag != 1)goto _LL3D;_tmp94=((struct _tuple4)(((_tmp93->f1).aggr_info).UnknownAggr).val).f1;_tmp95=((struct _tuple4)(((_tmp93->f1).aggr_info).UnknownAggr).val).f2;_tmp96=((struct _tuple4)(((_tmp93->f1).aggr_info).UnknownAggr).val).f3;_tmp97=(_tmp93->f1).targs;}}_LL3C: {
# 312
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpF2A;struct Cyc_Absyn_AggrInfo _tmpF29;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF28;return(void*)((_tmpF28=_cycalloc(sizeof(*_tmpF28)),((_tmpF28[0]=((_tmpF2A.tag=11,((_tmpF2A.f1=((_tmpF29.aggr_info=Cyc_Absyn_UnknownAggr(_tmp94,_tmp95,_tmp96),((_tmpF29.targs=Cyc_Tcutil_copy_types(_tmp97),_tmpF29)))),_tmpF2A)))),_tmpF28))));}_LL3D: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp98=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp68;if(_tmp98->tag != 11)goto _LL3F;else{if((((_tmp98->f1).aggr_info).KnownAggr).tag != 2)goto _LL3F;_tmp99=(struct Cyc_Absyn_Aggrdecl**)(((_tmp98->f1).aggr_info).KnownAggr).val;_tmp9A=(_tmp98->f1).targs;}}_LL3E: {
# 314
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpF30;struct Cyc_Absyn_AggrInfo _tmpF2F;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF2E;return(void*)((_tmpF2E=_cycalloc(sizeof(*_tmpF2E)),((_tmpF2E[0]=((_tmpF30.tag=11,((_tmpF30.f1=((_tmpF2F.aggr_info=Cyc_Absyn_KnownAggr(_tmp99),((_tmpF2F.targs=Cyc_Tcutil_copy_types(_tmp9A),_tmpF2F)))),_tmpF30)))),_tmpF2E))));}_LL3F: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp9B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp68;if(_tmp9B->tag != 12)goto _LL41;else{_tmp9C=_tmp9B->f1;_tmp9D=_tmp9B->f2;}}_LL40: {
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpF33;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpF32;return(void*)((_tmpF32=_cycalloc(sizeof(*_tmpF32)),((_tmpF32[0]=((_tmpF33.tag=12,((_tmpF33.f1=_tmp9C,((_tmpF33.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmp9D),_tmpF33)))))),_tmpF32))));}_LL41: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp9E=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp68;if(_tmp9E->tag != 13)goto _LL43;else{_tmp9F=_tmp9E->f1;_tmpA0=_tmp9E->f2;}}_LL42: {
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpF36;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpF35;return(void*)((_tmpF35=_cycalloc(sizeof(*_tmpF35)),((_tmpF35[0]=((_tmpF36.tag=13,((_tmpF36.f1=_tmp9F,((_tmpF36.f2=_tmpA0,_tmpF36)))))),_tmpF35))));}_LL43: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpA1=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp68;if(_tmpA1->tag != 14)goto _LL45;else{_tmpA2=_tmpA1->f1;}}_LL44: {
struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpF39;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpF38;return(void*)((_tmpF38=_cycalloc(sizeof(*_tmpF38)),((_tmpF38[0]=((_tmpF39.tag=14,((_tmpF39.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmpA2),_tmpF39)))),_tmpF38))));}_LL45: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpA3=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp68;if(_tmpA3->tag != 19)goto _LL47;else{_tmpA4=(void*)_tmpA3->f1;}}_LL46: {
struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpF3C;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpF3B;return(void*)((_tmpF3B=_cycalloc(sizeof(*_tmpF3B)),((_tmpF3B[0]=((_tmpF3C.tag=19,((_tmpF3C.f1=Cyc_Tcutil_copy_type(_tmpA4),_tmpF3C)))),_tmpF3B))));}_LL47: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpA5=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp68;if(_tmpA5->tag != 18)goto _LL49;else{_tmpA6=_tmpA5->f1;}}_LL48: {
# 321
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpF3F;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpF3E;return(void*)((_tmpF3E=_cycalloc(sizeof(*_tmpF3E)),((_tmpF3E[0]=((_tmpF3F.tag=18,((_tmpF3F.f1=_tmpA6,_tmpF3F)))),_tmpF3E))));}_LL49: {struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmpA7=(struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp68;if(_tmpA7->tag != 27)goto _LL4B;else{_tmpA8=_tmpA7->f1;}}_LL4A: {
# 324
struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmpF42;struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmpF41;return(void*)((_tmpF41=_cycalloc(sizeof(*_tmpF41)),((_tmpF41[0]=((_tmpF42.tag=27,((_tmpF42.f1=_tmpA8,_tmpF42)))),_tmpF41))));}_LL4B: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA9=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp68;if(_tmpA9->tag != 15)goto _LL4D;else{_tmpAA=(void*)_tmpA9->f1;}}_LL4C: {
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpF45;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpF44;return(void*)((_tmpF44=_cycalloc(sizeof(*_tmpF44)),((_tmpF44[0]=((_tmpF45.tag=15,((_tmpF45.f1=Cyc_Tcutil_copy_type(_tmpAA),_tmpF45)))),_tmpF44))));}_LL4D: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpAB=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp68;if(_tmpAB->tag != 16)goto _LL4F;else{_tmpAC=(void*)_tmpAB->f1;_tmpAD=(void*)_tmpAB->f2;}}_LL4E: {
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpF48;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpF47;return(void*)((_tmpF47=_cycalloc(sizeof(*_tmpF47)),((_tmpF47[0]=((_tmpF48.tag=16,((_tmpF48.f1=Cyc_Tcutil_copy_type(_tmpAC),((_tmpF48.f2=Cyc_Tcutil_copy_type(_tmpAD),_tmpF48)))))),_tmpF47))));}_LL4F: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpAE=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp68;if(_tmpAE->tag != 17)goto _LL51;else{_tmpAF=_tmpAE->f1;_tmpB0=_tmpAE->f2;_tmpB1=_tmpAE->f3;}}_LL50: {
# 328
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpF4B;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpF4A;return(void*)((_tmpF4A=_cycalloc(sizeof(*_tmpF4A)),((_tmpF4A[0]=((_tmpF4B.tag=17,((_tmpF4B.f1=_tmpAF,((_tmpF4B.f2=Cyc_Tcutil_copy_types(_tmpB0),((_tmpF4B.f3=_tmpB1,((_tmpF4B.f4=0,_tmpF4B)))))))))),_tmpF4A))));}_LL51: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpB2=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp68;if(_tmpB2->tag != 21)goto _LL53;}_LL52:
 goto _LL54;_LL53: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpB3=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp68;if(_tmpB3->tag != 22)goto _LL55;}_LL54:
 goto _LL56;_LL55: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmpB4=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp68;if(_tmpB4->tag != 20)goto _LL57;}_LL56:
 return t;_LL57: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpB5=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp68;if(_tmpB5->tag != 23)goto _LL59;else{_tmpB6=(void*)_tmpB5->f1;}}_LL58: {
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpF4E;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpF4D;return(void*)((_tmpF4D=_cycalloc(sizeof(*_tmpF4D)),((_tmpF4D[0]=((_tmpF4E.tag=23,((_tmpF4E.f1=Cyc_Tcutil_copy_type(_tmpB6),_tmpF4E)))),_tmpF4D))));}_LL59: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpB7=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp68;if(_tmpB7->tag != 24)goto _LL5B;else{_tmpB8=_tmpB7->f1;}}_LL5A: {
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF51;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF50;return(void*)((_tmpF50=_cycalloc(sizeof(*_tmpF50)),((_tmpF50[0]=((_tmpF51.tag=24,((_tmpF51.f1=Cyc_Tcutil_copy_types(_tmpB8),_tmpF51)))),_tmpF50))));}_LL5B: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpB9=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp68;if(_tmpB9->tag != 25)goto _LL5D;else{_tmpBA=(void*)_tmpB9->f1;}}_LL5C: {
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpF54;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpF53;return(void*)((_tmpF53=_cycalloc(sizeof(*_tmpF53)),((_tmpF53[0]=((_tmpF54.tag=25,((_tmpF54.f1=Cyc_Tcutil_copy_type(_tmpBA),_tmpF54)))),_tmpF53))));}_LL5D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpBB=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp68;if(_tmpBB->tag != 26)goto _LL5F;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmpBC=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmpBB->f1)->r;if(_tmpBC->tag != 0)goto _LL5F;else{_tmpBD=_tmpBC->f1;}}}_LL5E: {
# 337
struct Cyc_List_List*_tmp106=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpBD->tvs);
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpF5A;struct Cyc_Absyn_AggrInfo _tmpF59;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF58;return(void*)((_tmpF58=_cycalloc(sizeof(*_tmpF58)),((_tmpF58[0]=((_tmpF5A.tag=11,((_tmpF5A.f1=((_tmpF59.aggr_info=Cyc_Absyn_UnknownAggr(_tmpBD->kind,_tmpBD->name,0),((_tmpF59.targs=_tmp106,_tmpF59)))),_tmpF5A)))),_tmpF58))));}_LL5F: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpBE=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp68;if(_tmpBE->tag != 26)goto _LL61;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpBF=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmpBE->f1)->r;if(_tmpBF->tag != 1)goto _LL61;else{_tmpC0=_tmpBF->f1;}}}_LL60: {
# 340
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpF5D;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpF5C;return(void*)((_tmpF5C=_cycalloc(sizeof(*_tmpF5C)),((_tmpF5C[0]=((_tmpF5D.tag=13,((_tmpF5D.f1=_tmpC0->name,((_tmpF5D.f2=0,_tmpF5D)))))),_tmpF5C))));}_LL61: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpC1=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp68;if(_tmpC1->tag != 26)goto _LL24;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmpC2=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmpC1->f1)->r;if(_tmpC2->tag != 2)goto _LL24;else{_tmpC3=_tmpC2->f1;}}}_LL62: {
# 342
struct Cyc_List_List*_tmp10C=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpC3->tvs);
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpF67;struct Cyc_Absyn_UnknownDatatypeInfo _tmpF66;struct Cyc_Absyn_DatatypeInfo _tmpF65;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpF64;return(void*)((_tmpF64=_cycalloc(sizeof(*_tmpF64)),((_tmpF64[0]=((_tmpF67.tag=3,((_tmpF67.f1=((_tmpF65.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpF66.name=_tmpC3->name,((_tmpF66.is_extensible=0,_tmpF66))))),((_tmpF65.targs=_tmp10C,_tmpF65)))),_tmpF67)))),_tmpF64))));}_LL24:;}
# 357 "tcutil.cyc"
static void*Cyc_Tcutil_copy_designator(int pt,void*d){
void*_tmp111=d;struct Cyc_Absyn_Exp*_tmp113;struct _dyneither_ptr*_tmp115;_LL64: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp112=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp111;if(_tmp112->tag != 0)goto _LL66;else{_tmp113=_tmp112->f1;}}_LL65: {
struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpF6A;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpF69;return(void*)((_tmpF69=_cycalloc(sizeof(*_tmpF69)),((_tmpF69[0]=((_tmpF6A.tag=0,((_tmpF6A.f1=Cyc_Tcutil_deep_copy_exp(pt,_tmp113),_tmpF6A)))),_tmpF69))));}_LL66: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp114=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp111;if(_tmp114->tag != 1)goto _LL63;else{_tmp115=_tmp114->f1;}}_LL67:
 return d;_LL63:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 363
static struct _tuple19*Cyc_Tcutil_copy_eds(int pt,struct _tuple19*e){
# 365
struct _tuple19*_tmpF6B;return(_tmpF6B=_cycalloc(sizeof(*_tmpF6B)),((_tmpF6B->f1=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,pt,(e[0]).f1),((_tmpF6B->f2=Cyc_Tcutil_deep_copy_exp(pt,(e[0]).f2),_tmpF6B)))));}
# 368
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*new_e;
int _tmp119=preserve_types;
{void*_tmp11A=e->r;void*_tmp11B=_tmp11A;union Cyc_Absyn_Cnst _tmp11D;struct _tuple2*_tmp11F;void*_tmp120;enum Cyc_Absyn_Primop _tmp122;struct Cyc_List_List*_tmp123;struct Cyc_Absyn_Exp*_tmp125;struct Cyc_Core_Opt*_tmp126;struct Cyc_Absyn_Exp*_tmp127;struct Cyc_Absyn_Exp*_tmp129;enum Cyc_Absyn_Incrementor _tmp12A;struct Cyc_Absyn_Exp*_tmp12C;struct Cyc_Absyn_Exp*_tmp12D;struct Cyc_Absyn_Exp*_tmp12E;struct Cyc_Absyn_Exp*_tmp130;struct Cyc_Absyn_Exp*_tmp131;struct Cyc_Absyn_Exp*_tmp133;struct Cyc_Absyn_Exp*_tmp134;struct Cyc_Absyn_Exp*_tmp136;struct Cyc_Absyn_Exp*_tmp137;struct Cyc_Absyn_Exp*_tmp139;struct Cyc_List_List*_tmp13A;struct Cyc_Absyn_VarargCallInfo*_tmp13B;int _tmp13C;struct Cyc_Absyn_Exp*_tmp13E;int _tmp13F;struct Cyc_Absyn_Exp*_tmp141;struct Cyc_Absyn_Exp*_tmp143;struct Cyc_List_List*_tmp144;void*_tmp146;struct Cyc_Absyn_Exp*_tmp147;int _tmp148;enum Cyc_Absyn_Coercion _tmp149;struct Cyc_Absyn_Exp*_tmp14B;struct Cyc_Absyn_Exp*_tmp14D;struct Cyc_Absyn_Exp*_tmp14E;void*_tmp150;struct Cyc_Absyn_Exp*_tmp152;void*_tmp154;struct Cyc_List_List*_tmp155;struct Cyc_Absyn_Exp*_tmp157;struct Cyc_Absyn_Exp*_tmp159;struct _dyneither_ptr*_tmp15A;int _tmp15B;int _tmp15C;struct Cyc_Absyn_Exp*_tmp15E;struct _dyneither_ptr*_tmp15F;int _tmp160;int _tmp161;struct Cyc_Absyn_Exp*_tmp163;struct Cyc_Absyn_Exp*_tmp164;struct Cyc_List_List*_tmp166;struct _dyneither_ptr*_tmp168;struct Cyc_Absyn_Tqual _tmp169;void*_tmp16A;struct Cyc_List_List*_tmp16B;struct Cyc_List_List*_tmp16D;struct Cyc_Absyn_Vardecl*_tmp16F;struct Cyc_Absyn_Exp*_tmp170;struct Cyc_Absyn_Exp*_tmp171;int _tmp172;struct Cyc_Absyn_Exp*_tmp174;void*_tmp175;int _tmp176;struct _tuple2*_tmp178;struct Cyc_List_List*_tmp179;struct Cyc_List_List*_tmp17A;struct Cyc_Absyn_Aggrdecl*_tmp17B;void*_tmp17D;struct Cyc_List_List*_tmp17E;struct Cyc_List_List*_tmp180;struct Cyc_Absyn_Datatypedecl*_tmp181;struct Cyc_Absyn_Datatypefield*_tmp182;struct _tuple2*_tmp184;struct Cyc_Absyn_Enumdecl*_tmp185;struct Cyc_Absyn_Enumfield*_tmp186;struct _tuple2*_tmp188;void*_tmp189;struct Cyc_Absyn_Enumfield*_tmp18A;int _tmp18C;struct Cyc_Absyn_Exp*_tmp18D;void**_tmp18E;struct Cyc_Absyn_Exp*_tmp18F;int _tmp190;struct Cyc_Absyn_Exp*_tmp192;struct Cyc_Absyn_Exp*_tmp193;struct Cyc_Core_Opt*_tmp195;struct Cyc_List_List*_tmp196;struct Cyc_Absyn_Exp*_tmp199;struct _dyneither_ptr*_tmp19A;void*_tmp19C;int _tmp19E;struct _dyneither_ptr _tmp19F;_LL69: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp11C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp11C->tag != 0)goto _LL6B;else{_tmp11D=_tmp11C->f1;}}_LL6A:
 new_e=Cyc_Absyn_const_exp(_tmp11D,e->loc);goto _LL68;_LL6B: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp11E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp11E->tag != 1)goto _LL6D;else{_tmp11F=_tmp11E->f1;_tmp120=(void*)_tmp11E->f2;}}_LL6C:
 new_e=Cyc_Absyn_varb_exp(_tmp11F,_tmp120,e->loc);goto _LL68;_LL6D: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp121=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp121->tag != 2)goto _LL6F;else{_tmp122=_tmp121->f1;_tmp123=_tmp121->f2;}}_LL6E:
 new_e=Cyc_Absyn_primop_exp(_tmp122,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp119,_tmp123),e->loc);goto _LL68;_LL6F: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp124=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp124->tag != 3)goto _LL71;else{_tmp125=_tmp124->f1;_tmp126=_tmp124->f2;_tmp127=_tmp124->f3;}}_LL70:
# 376
{struct Cyc_Core_Opt*_tmpF6C;new_e=Cyc_Absyn_assignop_exp(Cyc_Tcutil_deep_copy_exp(_tmp119,_tmp125),(unsigned int)_tmp126?(_tmpF6C=_cycalloc_atomic(sizeof(*_tmpF6C)),((_tmpF6C->v=(void*)((enum Cyc_Absyn_Primop)_tmp126->v),_tmpF6C))): 0,Cyc_Tcutil_deep_copy_exp(_tmp119,_tmp127),e->loc);}
goto _LL68;_LL71: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp128=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp128->tag != 4)goto _LL73;else{_tmp129=_tmp128->f1;_tmp12A=_tmp128->f2;}}_LL72:
 new_e=Cyc_Absyn_increment_exp(Cyc_Tcutil_deep_copy_exp(_tmp119,_tmp129),_tmp12A,e->loc);goto _LL68;_LL73: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp12B=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp12B->tag != 5)goto _LL75;else{_tmp12C=_tmp12B->f1;_tmp12D=_tmp12B->f2;_tmp12E=_tmp12B->f3;}}_LL74:
# 380
 new_e=Cyc_Absyn_conditional_exp(Cyc_Tcutil_deep_copy_exp(_tmp119,_tmp12C),Cyc_Tcutil_deep_copy_exp(_tmp119,_tmp12D),Cyc_Tcutil_deep_copy_exp(_tmp119,_tmp12E),e->loc);goto _LL68;_LL75: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp12F=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp12F->tag != 6)goto _LL77;else{_tmp130=_tmp12F->f1;_tmp131=_tmp12F->f2;}}_LL76:
 new_e=Cyc_Absyn_and_exp(Cyc_Tcutil_deep_copy_exp(_tmp119,_tmp130),Cyc_Tcutil_deep_copy_exp(_tmp119,_tmp131),e->loc);goto _LL68;_LL77: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp132=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp132->tag != 7)goto _LL79;else{_tmp133=_tmp132->f1;_tmp134=_tmp132->f2;}}_LL78:
 new_e=Cyc_Absyn_or_exp(Cyc_Tcutil_deep_copy_exp(_tmp119,_tmp133),Cyc_Tcutil_deep_copy_exp(_tmp119,_tmp134),e->loc);goto _LL68;_LL79: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp135=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp135->tag != 8)goto _LL7B;else{_tmp136=_tmp135->f1;_tmp137=_tmp135->f2;}}_LL7A:
 new_e=Cyc_Absyn_seq_exp(Cyc_Tcutil_deep_copy_exp(_tmp119,_tmp136),Cyc_Tcutil_deep_copy_exp(_tmp119,_tmp137),e->loc);goto _LL68;_LL7B: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp138=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp138->tag != 9)goto _LL7D;else{_tmp139=_tmp138->f1;_tmp13A=_tmp138->f2;_tmp13B=_tmp138->f3;_tmp13C=_tmp138->f4;}}_LL7C:
# 385
{struct Cyc_Absyn_VarargCallInfo*_tmp1A1=_tmp13B;int _tmp1A2;struct Cyc_List_List*_tmp1A3;struct Cyc_Absyn_VarargInfo*_tmp1A4;_LLBA: if(_tmp1A1 == 0)goto _LLBC;_tmp1A2=_tmp1A1->num_varargs;_tmp1A3=_tmp1A1->injectors;_tmp1A4=_tmp1A1->vai;_LLBB: {
# 387
struct _dyneither_ptr*_tmp1A6;struct Cyc_Absyn_Tqual _tmp1A7;void*_tmp1A8;int _tmp1A9;struct Cyc_Absyn_VarargInfo*_tmp1A5=_tmp1A4;_tmp1A6=_tmp1A5->name;_tmp1A7=_tmp1A5->tq;_tmp1A8=_tmp1A5->type;_tmp1A9=_tmp1A5->inject;
{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpF76;struct Cyc_Absyn_VarargInfo*_tmpF75;struct Cyc_Absyn_VarargCallInfo*_tmpF74;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpF73;new_e=Cyc_Absyn_new_exp((void*)((_tmpF73=_cycalloc(sizeof(*_tmpF73)),((_tmpF73[0]=((_tmpF76.tag=9,((_tmpF76.f1=
Cyc_Tcutil_deep_copy_exp(_tmp119,_tmp139),((_tmpF76.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp119,_tmp13A),((_tmpF76.f3=(
(_tmpF74=_cycalloc(sizeof(*_tmpF74)),((_tmpF74->num_varargs=_tmp1A2,((_tmpF74->injectors=_tmp1A3,((_tmpF74->vai=(
(_tmpF75=_cycalloc(sizeof(*_tmpF75)),((_tmpF75->name=_tmp1A6,((_tmpF75->tq=_tmp1A7,((_tmpF75->type=Cyc_Tcutil_copy_type(_tmp1A8),((_tmpF75->inject=_tmp1A9,_tmpF75)))))))))),_tmpF74)))))))),((_tmpF76.f4=_tmp13C,_tmpF76)))))))))),_tmpF73)))),e->loc);}
# 393
goto _LLB9;}_LLBC:;_LLBD:
# 395
{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpF79;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpF78;new_e=Cyc_Absyn_new_exp((void*)((_tmpF78=_cycalloc(sizeof(*_tmpF78)),((_tmpF78[0]=((_tmpF79.tag=9,((_tmpF79.f1=Cyc_Tcutil_deep_copy_exp(_tmp119,_tmp139),((_tmpF79.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp119,_tmp13A),((_tmpF79.f3=_tmp13B,((_tmpF79.f4=_tmp13C,_tmpF79)))))))))),_tmpF78)))),e->loc);}
goto _LLB9;_LLB9:;}
# 398
goto _LL68;_LL7D: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp13D=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp13D->tag != 10)goto _LL7F;else{_tmp13E=_tmp13D->f1;_tmp13F=_tmp13D->f2;}}_LL7E:
# 400
 new_e=_tmp13F?Cyc_Absyn_rethrow_exp(Cyc_Tcutil_deep_copy_exp(_tmp119,_tmp13E),e->loc): Cyc_Absyn_throw_exp(Cyc_Tcutil_deep_copy_exp(_tmp119,_tmp13E),e->loc);
goto _LL68;_LL7F: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp140=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp140->tag != 11)goto _LL81;else{_tmp141=_tmp140->f1;}}_LL80:
 new_e=Cyc_Absyn_noinstantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmp119,_tmp141),e->loc);
goto _LL68;_LL81: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp142=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp142->tag != 12)goto _LL83;else{_tmp143=_tmp142->f1;_tmp144=_tmp142->f2;}}_LL82:
# 405
 new_e=Cyc_Absyn_instantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmp119,_tmp143),((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp144),e->loc);
goto _LL68;_LL83: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp145=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp145->tag != 13)goto _LL85;else{_tmp146=(void*)_tmp145->f1;_tmp147=_tmp145->f2;_tmp148=_tmp145->f3;_tmp149=_tmp145->f4;}}_LL84:
# 408
 new_e=Cyc_Absyn_cast_exp(Cyc_Tcutil_copy_type(_tmp146),Cyc_Tcutil_deep_copy_exp(_tmp119,_tmp147),_tmp148,_tmp149,e->loc);goto _LL68;_LL85: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp14A=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp14A->tag != 14)goto _LL87;else{_tmp14B=_tmp14A->f1;}}_LL86:
 new_e=Cyc_Absyn_address_exp(Cyc_Tcutil_deep_copy_exp(_tmp119,_tmp14B),e->loc);goto _LL68;_LL87: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp14C=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp14C->tag != 15)goto _LL89;else{_tmp14D=_tmp14C->f1;_tmp14E=_tmp14C->f2;}}_LL88: {
# 411
struct Cyc_Absyn_Exp*eo1=_tmp14D;if(_tmp14D != 0)eo1=Cyc_Tcutil_deep_copy_exp(_tmp119,_tmp14D);
new_e=Cyc_Absyn_New_exp(eo1,Cyc_Tcutil_deep_copy_exp(_tmp119,_tmp14E),e->loc);
goto _LL68;}_LL89: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp14F=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp14F->tag != 16)goto _LL8B;else{_tmp150=(void*)_tmp14F->f1;}}_LL8A:
 new_e=Cyc_Absyn_sizeoftyp_exp(Cyc_Tcutil_copy_type(_tmp150),e->loc);
goto _LL68;_LL8B: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp151=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp151->tag != 17)goto _LL8D;else{_tmp152=_tmp151->f1;}}_LL8C:
 new_e=Cyc_Absyn_sizeofexp_exp(Cyc_Tcutil_deep_copy_exp(_tmp119,_tmp152),e->loc);goto _LL68;_LL8D: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp153=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp153->tag != 18)goto _LL8F;else{_tmp154=(void*)_tmp153->f1;_tmp155=_tmp153->f2;}}_LL8E:
# 418
 new_e=Cyc_Absyn_offsetof_exp(Cyc_Tcutil_copy_type(_tmp154),_tmp155,e->loc);goto _LL68;_LL8F: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp156=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp156->tag != 19)goto _LL91;else{_tmp157=_tmp156->f1;}}_LL90:
 new_e=Cyc_Absyn_deref_exp(Cyc_Tcutil_deep_copy_exp(_tmp119,_tmp157),e->loc);goto _LL68;_LL91: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp158=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp158->tag != 20)goto _LL93;else{_tmp159=_tmp158->f1;_tmp15A=_tmp158->f2;_tmp15B=_tmp158->f3;_tmp15C=_tmp158->f4;}}_LL92:
# 421
{struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpF7C;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpF7B;new_e=Cyc_Absyn_new_exp((void*)((_tmpF7B=_cycalloc(sizeof(*_tmpF7B)),((_tmpF7B[0]=((_tmpF7C.tag=20,((_tmpF7C.f1=Cyc_Tcutil_deep_copy_exp(_tmp119,_tmp159),((_tmpF7C.f2=_tmp15A,((_tmpF7C.f3=_tmp15B,((_tmpF7C.f4=_tmp15C,_tmpF7C)))))))))),_tmpF7B)))),e->loc);}goto _LL68;_LL93: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp15D=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp15D->tag != 21)goto _LL95;else{_tmp15E=_tmp15D->f1;_tmp15F=_tmp15D->f2;_tmp160=_tmp15D->f3;_tmp161=_tmp15D->f4;}}_LL94:
# 423
{struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpF7F;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpF7E;new_e=Cyc_Absyn_new_exp((void*)((_tmpF7E=_cycalloc(sizeof(*_tmpF7E)),((_tmpF7E[0]=((_tmpF7F.tag=21,((_tmpF7F.f1=Cyc_Tcutil_deep_copy_exp(_tmp119,_tmp15E),((_tmpF7F.f2=_tmp15F,((_tmpF7F.f3=_tmp160,((_tmpF7F.f4=_tmp161,_tmpF7F)))))))))),_tmpF7E)))),e->loc);}goto _LL68;_LL95: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp162=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp162->tag != 22)goto _LL97;else{_tmp163=_tmp162->f1;_tmp164=_tmp162->f2;}}_LL96:
 new_e=Cyc_Absyn_subscript_exp(Cyc_Tcutil_deep_copy_exp(_tmp119,_tmp163),Cyc_Tcutil_deep_copy_exp(_tmp119,_tmp164),e->loc);
goto _LL68;_LL97: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp165=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp165->tag != 23)goto _LL99;else{_tmp166=_tmp165->f1;}}_LL98:
 new_e=Cyc_Absyn_tuple_exp(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp119,_tmp166),e->loc);goto _LL68;_LL99: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp167=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp167->tag != 24)goto _LL9B;else{_tmp168=(_tmp167->f1)->f1;_tmp169=(_tmp167->f1)->f2;_tmp16A=(_tmp167->f1)->f3;_tmp16B=_tmp167->f2;}}_LL9A: {
# 428
struct _dyneither_ptr*vopt1=_tmp168;
if(_tmp168 != 0)vopt1=_tmp168;
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpF85;struct _tuple10*_tmpF84;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpF83;new_e=Cyc_Absyn_new_exp((void*)((_tmpF83=_cycalloc(sizeof(*_tmpF83)),((_tmpF83[0]=((_tmpF85.tag=24,((_tmpF85.f1=((_tmpF84=_cycalloc(sizeof(*_tmpF84)),((_tmpF84->f1=vopt1,((_tmpF84->f2=_tmp169,((_tmpF84->f3=Cyc_Tcutil_copy_type(_tmp16A),_tmpF84)))))))),((_tmpF85.f2=
((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp119,_tmp16B),_tmpF85)))))),_tmpF83)))),e->loc);}
goto _LL68;}_LL9B: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp16C=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp16C->tag != 25)goto _LL9D;else{_tmp16D=_tmp16C->f1;}}_LL9C:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmpF88;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpF87;new_e=Cyc_Absyn_new_exp((void*)((_tmpF87=_cycalloc(sizeof(*_tmpF87)),((_tmpF87[0]=((_tmpF88.tag=25,((_tmpF88.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp119,_tmp16D),_tmpF88)))),_tmpF87)))),e->loc);}
goto _LL68;_LL9D: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp16E=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp16E->tag != 26)goto _LL9F;else{_tmp16F=_tmp16E->f1;_tmp170=_tmp16E->f2;_tmp171=_tmp16E->f3;_tmp172=_tmp16E->f4;}}_LL9E:
# 436
{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpF8B;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpF8A;new_e=Cyc_Absyn_new_exp((void*)((_tmpF8A=_cycalloc(sizeof(*_tmpF8A)),((_tmpF8A[0]=((_tmpF8B.tag=26,((_tmpF8B.f1=_tmp16F,((_tmpF8B.f2=Cyc_Tcutil_deep_copy_exp(_tmp119,_tmp170),((_tmpF8B.f3=Cyc_Tcutil_deep_copy_exp(_tmp119,_tmp171),((_tmpF8B.f4=_tmp172,_tmpF8B)))))))))),_tmpF8A)))),e->loc);}
goto _LL68;_LL9F: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp173=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp173->tag != 27)goto _LLA1;else{_tmp174=_tmp173->f1;_tmp175=(void*)_tmp173->f2;_tmp176=_tmp173->f3;}}_LLA0:
# 439
{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpF8E;struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpF8D;new_e=Cyc_Absyn_new_exp((void*)((_tmpF8D=_cycalloc(sizeof(*_tmpF8D)),((_tmpF8D[0]=((_tmpF8E.tag=27,((_tmpF8E.f1=Cyc_Tcutil_deep_copy_exp(_tmp119,_tmp174),((_tmpF8E.f2=Cyc_Tcutil_copy_type(_tmp175),((_tmpF8E.f3=_tmp176,_tmpF8E)))))))),_tmpF8D)))),_tmp174->loc);}
# 441
goto _LL68;_LLA1: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp177=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp177->tag != 28)goto _LLA3;else{_tmp178=_tmp177->f1;_tmp179=_tmp177->f2;_tmp17A=_tmp177->f3;_tmp17B=_tmp177->f4;}}_LLA2:
# 443
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpF91;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpF90;new_e=Cyc_Absyn_new_exp((void*)((_tmpF90=_cycalloc(sizeof(*_tmpF90)),((_tmpF90[0]=((_tmpF91.tag=28,((_tmpF91.f1=_tmp178,((_tmpF91.f2=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp179),((_tmpF91.f3=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp119,_tmp17A),((_tmpF91.f4=_tmp17B,_tmpF91)))))))))),_tmpF90)))),e->loc);}
# 445
goto _LL68;_LLA3: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp17C=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp17C->tag != 29)goto _LLA5;else{_tmp17D=(void*)_tmp17C->f1;_tmp17E=_tmp17C->f2;}}_LLA4:
# 447
{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmpF94;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpF93;new_e=Cyc_Absyn_new_exp((void*)((_tmpF93=_cycalloc(sizeof(*_tmpF93)),((_tmpF93[0]=((_tmpF94.tag=29,((_tmpF94.f1=Cyc_Tcutil_copy_type(_tmp17D),((_tmpF94.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp119,_tmp17E),_tmpF94)))))),_tmpF93)))),e->loc);}
goto _LL68;_LLA5: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp17F=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp17F->tag != 30)goto _LLA7;else{_tmp180=_tmp17F->f1;_tmp181=_tmp17F->f2;_tmp182=_tmp17F->f3;}}_LLA6:
# 450
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmpF97;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpF96;new_e=Cyc_Absyn_new_exp((void*)((_tmpF96=_cycalloc(sizeof(*_tmpF96)),((_tmpF96[0]=((_tmpF97.tag=30,((_tmpF97.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp119,_tmp180),((_tmpF97.f2=_tmp181,((_tmpF97.f3=_tmp182,_tmpF97)))))))),_tmpF96)))),e->loc);}
goto _LL68;_LLA7: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp183=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp183->tag != 31)goto _LLA9;else{_tmp184=_tmp183->f1;_tmp185=_tmp183->f2;_tmp186=_tmp183->f3;}}_LLA8:
 new_e=e;goto _LL68;_LLA9: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp187=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp187->tag != 32)goto _LLAB;else{_tmp188=_tmp187->f1;_tmp189=(void*)_tmp187->f2;_tmp18A=_tmp187->f3;}}_LLAA:
# 454
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpF9A;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpF99;new_e=Cyc_Absyn_new_exp((void*)((_tmpF99=_cycalloc(sizeof(*_tmpF99)),((_tmpF99[0]=((_tmpF9A.tag=32,((_tmpF9A.f1=_tmp188,((_tmpF9A.f2=Cyc_Tcutil_copy_type(_tmp189),((_tmpF9A.f3=_tmp18A,_tmpF9A)))))))),_tmpF99)))),e->loc);}
goto _LL68;_LLAB: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp18B=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp18B->tag != 33)goto _LLAD;else{_tmp18C=(_tmp18B->f1).is_calloc;_tmp18D=(_tmp18B->f1).rgn;_tmp18E=(_tmp18B->f1).elt_type;_tmp18F=(_tmp18B->f1).num_elts;_tmp190=(_tmp18B->f1).fat_result;}}_LLAC: {
# 457
struct Cyc_Absyn_Exp*_tmp1C5=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp18D;if(_tmp18D != 0)r1=Cyc_Tcutil_deep_copy_exp(_tmp119,_tmp18D);{
void**t1=_tmp18E;if(_tmp18E != 0){void**_tmpF9B;t1=((_tmpF9B=_cycalloc(sizeof(*_tmpF9B)),((_tmpF9B[0]=Cyc_Tcutil_copy_type(*_tmp18E),_tmpF9B))));}
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpFA1;struct Cyc_Absyn_MallocInfo _tmpFA0;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpF9F;_tmp1C5->r=(void*)((_tmpF9F=_cycalloc(sizeof(*_tmpF9F)),((_tmpF9F[0]=((_tmpFA1.tag=33,((_tmpFA1.f1=((_tmpFA0.is_calloc=_tmp18C,((_tmpFA0.rgn=r1,((_tmpFA0.elt_type=t1,((_tmpFA0.num_elts=_tmp18F,((_tmpFA0.fat_result=_tmp190,_tmpFA0)))))))))),_tmpFA1)))),_tmpF9F))));}
new_e=_tmp1C5;
goto _LL68;};}_LLAD: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp191=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp191->tag != 34)goto _LLAF;else{_tmp192=_tmp191->f1;_tmp193=_tmp191->f2;}}_LLAE:
 new_e=Cyc_Absyn_swap_exp(Cyc_Tcutil_deep_copy_exp(_tmp119,_tmp192),Cyc_Tcutil_deep_copy_exp(_tmp119,_tmp193),e->loc);goto _LL68;_LLAF: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp194=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp194->tag != 35)goto _LLB1;else{_tmp195=_tmp194->f1;_tmp196=_tmp194->f2;}}_LLB0: {
# 465
struct Cyc_Core_Opt*nopt1=_tmp195;
if(_tmp195 != 0){struct Cyc_Core_Opt*_tmpFA2;nopt1=((_tmpFA2=_cycalloc(sizeof(*_tmpFA2)),((_tmpFA2->v=(struct _tuple2*)_tmp195->v,_tmpFA2))));}
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpFA5;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpFA4;new_e=Cyc_Absyn_new_exp((void*)((_tmpFA4=_cycalloc(sizeof(*_tmpFA4)),((_tmpFA4[0]=((_tmpFA5.tag=35,((_tmpFA5.f1=nopt1,((_tmpFA5.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp119,_tmp196),_tmpFA5)))))),_tmpFA4)))),e->loc);}
goto _LL68;}_LLB1: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp197=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp197->tag != 36)goto _LLB3;}_LLB2: {
# 470
struct Cyc_Core_Failure_exn_struct _tmpFAB;const char*_tmpFAA;struct Cyc_Core_Failure_exn_struct*_tmpFA9;(int)_throw((void*)((_tmpFA9=_cycalloc(sizeof(*_tmpFA9)),((_tmpFA9[0]=((_tmpFAB.tag=Cyc_Core_Failure,((_tmpFAB.f1=((_tmpFAA="deep_copy: statement expressions unsupported",_tag_dyneither(_tmpFAA,sizeof(char),45))),_tmpFAB)))),_tmpFA9)))));}_LLB3: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp198=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp198->tag != 37)goto _LLB5;else{_tmp199=_tmp198->f1;_tmp19A=_tmp198->f2;}}_LLB4:
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpFAE;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpFAD;new_e=Cyc_Absyn_new_exp((void*)((_tmpFAD=_cycalloc(sizeof(*_tmpFAD)),((_tmpFAD[0]=((_tmpFAE.tag=37,((_tmpFAE.f1=Cyc_Tcutil_deep_copy_exp(_tmp119,_tmp199),((_tmpFAE.f2=_tmp19A,_tmpFAE)))))),_tmpFAD)))),e->loc);}
goto _LL68;_LLB5: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp19B=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp19B->tag != 38)goto _LLB7;else{_tmp19C=(void*)_tmp19B->f1;}}_LLB6:
 new_e=Cyc_Absyn_valueof_exp(Cyc_Tcutil_copy_type(_tmp19C),e->loc);
goto _LL68;_LLB7: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp19D=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp11B;if(_tmp19D->tag != 39)goto _LL68;else{_tmp19E=_tmp19D->f1;_tmp19F=_tmp19D->f2;}}_LLB8:
 new_e=Cyc_Absyn_asm_exp(_tmp19E,_tmp19F,e->loc);goto _LL68;_LL68:;}
# 478
if(preserve_types)new_e->topt=e->topt;
return new_e;}
# 482
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*e){
if(e == 0)return 0;else{
return Cyc_Tcutil_deep_copy_exp(preserve_types,e);}}struct _tuple20{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple21{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 495 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
enum Cyc_Absyn_KindQual _tmp1D3;enum Cyc_Absyn_AliasQual _tmp1D4;struct Cyc_Absyn_Kind*_tmp1D2=ka1;_tmp1D3=_tmp1D2->kind;_tmp1D4=_tmp1D2->aliasqual;{
enum Cyc_Absyn_KindQual _tmp1D6;enum Cyc_Absyn_AliasQual _tmp1D7;struct Cyc_Absyn_Kind*_tmp1D5=ka2;_tmp1D6=_tmp1D5->kind;_tmp1D7=_tmp1D5->aliasqual;
# 499
if(_tmp1D3 != _tmp1D6){
struct _tuple20 _tmpFAF;struct _tuple20 _tmp1D8=(_tmpFAF.f1=_tmp1D3,((_tmpFAF.f2=_tmp1D6,_tmpFAF)));struct _tuple20 _tmp1D9=_tmp1D8;_LLBF: if(_tmp1D9.f1 != Cyc_Absyn_BoxKind)goto _LLC1;if(_tmp1D9.f2 != Cyc_Absyn_MemKind)goto _LLC1;_LLC0:
 goto _LLC2;_LLC1: if(_tmp1D9.f1 != Cyc_Absyn_BoxKind)goto _LLC3;if(_tmp1D9.f2 != Cyc_Absyn_AnyKind)goto _LLC3;_LLC2:
 goto _LLC4;_LLC3: if(_tmp1D9.f1 != Cyc_Absyn_MemKind)goto _LLC5;if(_tmp1D9.f2 != Cyc_Absyn_AnyKind)goto _LLC5;_LLC4:
 goto _LLBE;_LLC5:;_LLC6:
 return 0;_LLBE:;}
# 508
if(_tmp1D4 != _tmp1D7){
struct _tuple21 _tmpFB0;struct _tuple21 _tmp1DB=(_tmpFB0.f1=_tmp1D4,((_tmpFB0.f2=_tmp1D7,_tmpFB0)));struct _tuple21 _tmp1DC=_tmp1DB;_LLC8: if(_tmp1DC.f1 != Cyc_Absyn_Aliasable)goto _LLCA;if(_tmp1DC.f2 != Cyc_Absyn_Top)goto _LLCA;_LLC9:
 goto _LLCB;_LLCA: if(_tmp1DC.f1 != Cyc_Absyn_Unique)goto _LLCC;if(_tmp1DC.f2 != Cyc_Absyn_Top)goto _LLCC;_LLCB:
 return 1;_LLCC:;_LLCD:
 return 0;_LLC7:;}
# 515
return 1;};}
# 518
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp1DE=Cyc_Absyn_compress_kb(tv->kind);void*_tmp1DF=_tmp1DE;struct Cyc_Absyn_Kind*_tmp1E1;struct Cyc_Absyn_Kind*_tmp1E3;_LLCF: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1E0=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1DF;if(_tmp1E0->tag != 0)goto _LLD1;else{_tmp1E1=_tmp1E0->f1;}}_LLD0:
 return _tmp1E1;_LLD1: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1E2=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1DF;if(_tmp1E2->tag != 2)goto _LLD3;else{_tmp1E3=_tmp1E2->f2;}}_LLD2:
 return _tmp1E3;_LLD3:;_LLD4:
# 523
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpFB3;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpFB2;tv->kind=(void*)((_tmpFB2=_cycalloc(sizeof(*_tmpFB2)),((_tmpFB2[0]=((_tmpFB3.tag=2,((_tmpFB3.f1=0,((_tmpFB3.f2=def,_tmpFB3)))))),_tmpFB2))));}
return def;_LLCE:;}
# 528
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
struct _tuple0 _tmpFB4;struct _tuple0 _tmp1E6=(_tmpFB4.f1=Cyc_Absyn_compress_kb(kb1),((_tmpFB4.f2=Cyc_Absyn_compress_kb(kb2),_tmpFB4)));struct _tuple0 _tmp1E7=_tmp1E6;struct Cyc_Absyn_Kind*_tmp1E9;struct Cyc_Absyn_Kind*_tmp1EB;struct Cyc_Core_Opt**_tmp1ED;struct Cyc_Absyn_Kind*_tmp1EE;struct Cyc_Absyn_Kind*_tmp1F0;struct Cyc_Absyn_Kind*_tmp1F2;struct Cyc_Core_Opt**_tmp1F4;struct Cyc_Absyn_Kind*_tmp1F5;struct Cyc_Core_Opt**_tmp1F7;struct Cyc_Absyn_Kind*_tmp1F8;struct Cyc_Core_Opt**_tmp1FA;struct Cyc_Absyn_Kind*_tmp1FB;struct Cyc_Core_Opt**_tmp1FD;void*_tmp1FE;void*_tmp1FF;struct Cyc_Core_Opt**_tmp201;_LLD6:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1E8=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1E7.f1;if(_tmp1E8->tag != 0)goto _LLD8;else{_tmp1E9=_tmp1E8->f1;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1EA=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1E7.f2;if(_tmp1EA->tag != 0)goto _LLD8;else{_tmp1EB=_tmp1EA->f1;}};_LLD7:
 return _tmp1E9 == _tmp1EB;_LLD8:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1EC=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1E7.f1;if(_tmp1EC->tag != 2)goto _LLDA;else{_tmp1ED=(struct Cyc_Core_Opt**)& _tmp1EC->f1;_tmp1EE=_tmp1EC->f2;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1EF=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1E7.f2;if(_tmp1EF->tag != 0)goto _LLDA;else{_tmp1F0=_tmp1EF->f1;}};_LLD9:
# 532
 if(Cyc_Tcutil_kind_leq(_tmp1F0,_tmp1EE)){
{struct Cyc_Core_Opt*_tmpFB5;*_tmp1ED=((_tmpFB5=_cycalloc(sizeof(*_tmpFB5)),((_tmpFB5->v=kb2,_tmpFB5))));}
return 1;}else{
return 0;}_LLDA:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1F1=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1E7.f1;if(_tmp1F1->tag != 0)goto _LLDC;else{_tmp1F2=_tmp1F1->f1;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1F3=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1E7.f2;if(_tmp1F3->tag != 2)goto _LLDC;else{_tmp1F4=(struct Cyc_Core_Opt**)& _tmp1F3->f1;_tmp1F5=_tmp1F3->f2;}};_LLDB:
# 537
 if(Cyc_Tcutil_kind_leq(_tmp1F2,_tmp1F5)){
{struct Cyc_Core_Opt*_tmpFB6;*_tmp1F4=((_tmpFB6=_cycalloc(sizeof(*_tmpFB6)),((_tmpFB6->v=kb1,_tmpFB6))));}
return 1;}else{
return 0;}_LLDC:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1F6=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1E7.f1;if(_tmp1F6->tag != 2)goto _LLDE;else{_tmp1F7=(struct Cyc_Core_Opt**)& _tmp1F6->f1;_tmp1F8=_tmp1F6->f2;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1F9=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1E7.f2;if(_tmp1F9->tag != 2)goto _LLDE;else{_tmp1FA=(struct Cyc_Core_Opt**)& _tmp1F9->f1;_tmp1FB=_tmp1F9->f2;}};_LLDD:
# 542
 if(Cyc_Tcutil_kind_leq(_tmp1F8,_tmp1FB)){
{struct Cyc_Core_Opt*_tmpFB7;*_tmp1FA=((_tmpFB7=_cycalloc(sizeof(*_tmpFB7)),((_tmpFB7->v=kb1,_tmpFB7))));}
return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp1FB,_tmp1F8)){
{struct Cyc_Core_Opt*_tmpFB8;*_tmp1F7=((_tmpFB8=_cycalloc(sizeof(*_tmpFB8)),((_tmpFB8->v=kb2,_tmpFB8))));}
return 1;}else{
return 0;}}_LLDE:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp1FC=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1E7.f1;if(_tmp1FC->tag != 1)goto _LLE0;else{_tmp1FD=(struct Cyc_Core_Opt**)& _tmp1FC->f1;}}_tmp1FE=_tmp1E7.f2;_LLDF:
 _tmp1FF=_tmp1FE;_tmp201=_tmp1FD;goto _LLE1;_LLE0: _tmp1FF=_tmp1E7.f1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp200=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1E7.f2;if(_tmp200->tag != 1)goto _LLD5;else{_tmp201=(struct Cyc_Core_Opt**)& _tmp200->f1;}};_LLE1:
# 551
{struct Cyc_Core_Opt*_tmpFB9;*_tmp201=((_tmpFB9=_cycalloc(sizeof(*_tmpFB9)),((_tmpFB9->v=_tmp1FF,_tmpFB9))));}
return 1;_LLD5:;}
# 556
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp208=Cyc_Tcutil_compress(t);void*_tmp209=_tmp208;struct Cyc_Absyn_Tvar*_tmp20B;_LLE3: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp20A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp209;if(_tmp20A->tag != 2)goto _LLE5;else{_tmp20B=_tmp20A->f1;}}_LLE4: {
# 559
void*_tmp20C=_tmp20B->kind;
_tmp20B->kind=kb;{
struct _tuple16 _tmpFBA;return(_tmpFBA.f1=_tmp20B,((_tmpFBA.f2=_tmp20C,_tmpFBA)));};}_LLE5:;_LLE6: {
# 563
const char*_tmpFBE;void*_tmpFBD[1];struct Cyc_String_pa_PrintArg_struct _tmpFBC;(_tmpFBC.tag=0,((_tmpFBC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpFBD[0]=& _tmpFBC,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFBE="swap_kind: cannot update the kind of %s",_tag_dyneither(_tmpFBE,sizeof(char),40))),_tag_dyneither(_tmpFBD,sizeof(void*),1)))))));}_LLE2:;}
# 569
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_typ,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 571
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_typ_kind(field_typ);
if(ts != 0  && (k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak)){
# 575
struct _RegionHandle _tmp211=_new_region("temp");struct _RegionHandle*temp=& _tmp211;_push_region(temp);
{struct Cyc_List_List*_tmp212=0;
# 578
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp213=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp214=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;
struct Cyc_Absyn_Kind*_tmp215=Cyc_Tcutil_tvar_kind(_tmp213,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp216=_tmp215;_LLE8: if(_tmp216->kind != Cyc_Absyn_IntKind)goto _LLEA;_LLE9:
 goto _LLEB;_LLEA: if(_tmp216->kind != Cyc_Absyn_AnyKind)goto _LLEC;_LLEB:
# 584
{struct _tuple16*_tmpFC1;struct Cyc_List_List*_tmpFC0;_tmp212=((_tmpFC0=_region_malloc(temp,sizeof(*_tmpFC0)),((_tmpFC0->hd=((_tmpFC1=_region_malloc(temp,sizeof(*_tmpFC1)),((_tmpFC1->f1=_tmp213,((_tmpFC1->f2=_tmp214,_tmpFC1)))))),((_tmpFC0->tl=_tmp212,_tmpFC0))))));}goto _LLE7;_LLEC:;_LLED:
 goto _LLE7;_LLE7:;}
# 588
if(_tmp212 != 0){
field_typ=Cyc_Tcutil_rsubstitute(temp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp212),field_typ);
k=Cyc_Tcutil_typ_kind(field_typ);}}
# 576
;_pop_region(temp);}
# 593
return k;}
# 600
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t){
void*_tmp219=Cyc_Tcutil_compress(t);void*_tmp21A=_tmp219;struct Cyc_Core_Opt*_tmp21C;struct Cyc_Absyn_Tvar*_tmp21E;enum Cyc_Absyn_Size_of _tmp221;struct Cyc_Absyn_Datatypedecl*_tmp22D;struct Cyc_Absyn_Datatypefield*_tmp22E;enum Cyc_Absyn_AggrKind _tmp232;struct Cyc_List_List*_tmp233;struct Cyc_Absyn_AggrdeclImpl*_tmp234;int _tmp235;struct Cyc_List_List*_tmp236;struct Cyc_Absyn_PtrInfo _tmp239;struct Cyc_Absyn_Exp*_tmp23E;struct Cyc_Absyn_Typedefdecl*_tmp241;_LLEF: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp21B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp21A;if(_tmp21B->tag != 1)goto _LLF1;else{_tmp21C=_tmp21B->f1;}}_LLF0:
 return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp21C))->v;_LLF1: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp21D=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp21A;if(_tmp21D->tag != 2)goto _LLF3;else{_tmp21E=_tmp21D->f1;}}_LLF2:
 return Cyc_Tcutil_tvar_kind(_tmp21E,& Cyc_Tcutil_bk);_LLF3: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp21F=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp21A;if(_tmp21F->tag != 0)goto _LLF5;}_LLF4:
 return& Cyc_Tcutil_mk;_LLF5: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp220=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp21A;if(_tmp220->tag != 6)goto _LLF7;else{_tmp221=_tmp220->f2;}}_LLF6:
# 606
 return(_tmp221 == Cyc_Absyn_Int_sz  || _tmp221 == Cyc_Absyn_Long_sz)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;_LLF7: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp222=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp21A;if(_tmp222->tag != 7)goto _LLF9;}_LLF8:
# 608
 return& Cyc_Tcutil_mk;_LLF9: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp223=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp21A;if(_tmp223->tag != 9)goto _LLFB;}_LLFA:
 return& Cyc_Tcutil_ak;_LLFB: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp224=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp21A;if(_tmp224->tag != 16)goto _LLFD;}_LLFC:
 goto _LLFE;_LLFD: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp225=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp21A;if(_tmp225->tag != 13)goto _LLFF;}_LLFE:
 goto _LL100;_LLFF: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp226=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp21A;if(_tmp226->tag != 14)goto _LL101;}_LL100:
 goto _LL102;_LL101: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp227=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp21A;if(_tmp227->tag != 15)goto _LL103;}_LL102:
 return& Cyc_Tcutil_bk;_LL103: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp228=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp21A;if(_tmp228->tag != 21)goto _LL105;}_LL104:
 return& Cyc_Tcutil_urk;_LL105: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp229=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp21A;if(_tmp229->tag != 20)goto _LL107;}_LL106:
 return& Cyc_Tcutil_rk;_LL107: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp22A=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp21A;if(_tmp22A->tag != 22)goto _LL109;}_LL108:
 return& Cyc_Tcutil_trk;_LL109: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp22B=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp21A;if(_tmp22B->tag != 3)goto _LL10B;}_LL10A:
# 619
 return& Cyc_Tcutil_ak;_LL10B: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp22C=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp21A;if(_tmp22C->tag != 4)goto _LL10D;else{if((((_tmp22C->f1).field_info).KnownDatatypefield).tag != 2)goto _LL10D;_tmp22D=((struct _tuple3)(((_tmp22C->f1).field_info).KnownDatatypefield).val).f1;_tmp22E=((struct _tuple3)(((_tmp22C->f1).field_info).KnownDatatypefield).val).f2;}}_LL10C:
# 621
 return& Cyc_Tcutil_mk;_LL10D: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp22F=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp21A;if(_tmp22F->tag != 4)goto _LL10F;else{if((((_tmp22F->f1).field_info).UnknownDatatypefield).tag != 1)goto _LL10F;}}_LL10E: {
# 623
const char*_tmpFC4;void*_tmpFC3;(_tmpFC3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFC4="typ_kind: Unresolved DatatypeFieldType",_tag_dyneither(_tmpFC4,sizeof(char),39))),_tag_dyneither(_tmpFC3,sizeof(void*),0)));}_LL10F: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp230=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp21A;if(_tmp230->tag != 11)goto _LL111;else{if((((_tmp230->f1).aggr_info).UnknownAggr).tag != 1)goto _LL111;}}_LL110:
# 626
 return& Cyc_Tcutil_ak;_LL111: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp231=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp21A;if(_tmp231->tag != 11)goto _LL113;else{if((((_tmp231->f1).aggr_info).KnownAggr).tag != 2)goto _LL113;_tmp232=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp231->f1).aggr_info).KnownAggr).val))->kind;_tmp233=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp231->f1).aggr_info).KnownAggr).val))->tvs;_tmp234=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp231->f1).aggr_info).KnownAggr).val))->impl;_tmp235=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp231->f1).aggr_info).KnownAggr).val))->expected_mem_kind;_tmp236=(_tmp231->f1).targs;}}_LL112:
# 628
 if(_tmp234 == 0){
if(_tmp235)
return& Cyc_Tcutil_mk;else{
# 632
return& Cyc_Tcutil_ak;}}{
# 634
struct Cyc_List_List*_tmp24D=_tmp234->fields;
if(_tmp24D == 0)return& Cyc_Tcutil_mk;
# 637
if(_tmp232 == Cyc_Absyn_StructA){
for(0;_tmp24D->tl != 0;_tmp24D=_tmp24D->tl){;}{
void*_tmp24E=((struct Cyc_Absyn_Aggrfield*)_tmp24D->hd)->type;
struct Cyc_Absyn_Kind*_tmp24F=Cyc_Tcutil_field_kind(_tmp24E,_tmp236,_tmp233);
if(_tmp24F == & Cyc_Tcutil_ak  || _tmp24F == & Cyc_Tcutil_tak)return _tmp24F;};}else{
# 645
for(0;_tmp24D != 0;_tmp24D=_tmp24D->tl){
void*_tmp250=((struct Cyc_Absyn_Aggrfield*)_tmp24D->hd)->type;
struct Cyc_Absyn_Kind*_tmp251=Cyc_Tcutil_field_kind(_tmp250,_tmp236,_tmp233);
if(_tmp251 == & Cyc_Tcutil_ak  || _tmp251 == & Cyc_Tcutil_tak)return _tmp251;}}
# 651
return& Cyc_Tcutil_mk;};_LL113: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp237=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp21A;if(_tmp237->tag != 12)goto _LL115;}_LL114:
 return& Cyc_Tcutil_mk;_LL115: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp238=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21A;if(_tmp238->tag != 5)goto _LL117;else{_tmp239=_tmp238->f1;}}_LL116: {
# 654
void*_tmp252=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,(_tmp239.ptr_atts).bounds);void*_tmp253=_tmp252;_LL130: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp254=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp253;if(_tmp254->tag != 0)goto _LL132;}_LL131: {
# 656
enum Cyc_Absyn_AliasQual _tmp256=(Cyc_Tcutil_typ_kind((_tmp239.ptr_atts).rgn))->aliasqual;switch(_tmp256){case Cyc_Absyn_Aliasable: _LL134:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL135:
 return& Cyc_Tcutil_umk;case Cyc_Absyn_Top: _LL136:
 return& Cyc_Tcutil_tmk;}}_LL132: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp255=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp253;if(_tmp255->tag != 1)goto _LL12F;}_LL133: {
# 662
enum Cyc_Absyn_AliasQual _tmp257=(Cyc_Tcutil_typ_kind((_tmp239.ptr_atts).rgn))->aliasqual;switch(_tmp257){case Cyc_Absyn_Aliasable: _LL138:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL139:
 return& Cyc_Tcutil_ubk;case Cyc_Absyn_Top: _LL13A:
 return& Cyc_Tcutil_tbk;}}_LL12F:;}_LL117: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp23A=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp21A;if(_tmp23A->tag != 18)goto _LL119;}_LL118:
# 668
 return& Cyc_Tcutil_ik;_LL119: {struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp23B=(struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp21A;if(_tmp23B->tag != 27)goto _LL11B;}_LL11A:
# 672
 return& Cyc_Tcutil_ak;_LL11B: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp23C=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp21A;if(_tmp23C->tag != 19)goto _LL11D;}_LL11C:
 return& Cyc_Tcutil_bk;_LL11D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp23D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp21A;if(_tmp23D->tag != 8)goto _LL11F;else{_tmp23E=(_tmp23D->f1).num_elts;}}_LL11E:
# 675
 if(_tmp23E == 0  || Cyc_Tcutil_is_const_exp(_tmp23E))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;_LL11F: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp23F=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp21A;if(_tmp23F->tag != 10)goto _LL121;}_LL120:
 return& Cyc_Tcutil_mk;_LL121: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp240=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp21A;if(_tmp240->tag != 17)goto _LL123;else{_tmp241=_tmp240->f3;}}_LL122:
# 679
 if(_tmp241 == 0  || _tmp241->kind == 0){
const char*_tmpFC8;void*_tmpFC7[1];struct Cyc_String_pa_PrintArg_struct _tmpFC6;(_tmpFC6.tag=0,((_tmpFC6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpFC7[0]=& _tmpFC6,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFC8="typ_kind: typedef found: %s",_tag_dyneither(_tmpFC8,sizeof(char),28))),_tag_dyneither(_tmpFC7,sizeof(void*),1)))))));}
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp241->kind))->v;_LL123: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp242=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp21A;if(_tmp242->tag != 26)goto _LL125;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp243=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp242->f1)->r;if(_tmp243->tag != 0)goto _LL125;}}_LL124:
 return& Cyc_Tcutil_ak;_LL125: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp244=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp21A;if(_tmp244->tag != 26)goto _LL127;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp245=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp244->f1)->r;if(_tmp245->tag != 1)goto _LL127;}}_LL126:
 return& Cyc_Tcutil_bk;_LL127: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp246=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp21A;if(_tmp246->tag != 26)goto _LL129;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp247=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp246->f1)->r;if(_tmp247->tag != 2)goto _LL129;}}_LL128:
 return& Cyc_Tcutil_ak;_LL129: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp248=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp21A;if(_tmp248->tag != 23)goto _LL12B;}_LL12A:
 goto _LL12C;_LL12B: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp249=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp21A;if(_tmp249->tag != 24)goto _LL12D;}_LL12C:
 goto _LL12E;_LL12D: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp24A=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp21A;if(_tmp24A->tag != 25)goto _LLEE;}_LL12E:
 return& Cyc_Tcutil_ek;_LLEE:;}
# 691
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2  || k1->kind == k2->kind  && k1->aliasqual == k2->aliasqual;}
# 696
int Cyc_Tcutil_unify(void*t1,void*t2){
struct _handler_cons _tmp25B;_push_handler(& _tmp25B);{int _tmp25D=0;if(setjmp(_tmp25B.handler))_tmp25D=1;if(!_tmp25D){
Cyc_Tcutil_unify_it(t1,t2);{
int _tmp25E=1;_npop_handler(0);return _tmp25E;};
# 698
;_pop_handler();}else{void*_tmp25C=(void*)_exn_thrown;void*_tmp260=_tmp25C;void*_tmp262;_LL13D: {struct Cyc_Tcutil_Unify_exn_struct*_tmp261=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp260;if(_tmp261->tag != Cyc_Tcutil_Unify)goto _LL13F;}_LL13E:
# 700
 return 0;_LL13F: _tmp262=_tmp260;_LL140:(void)_rethrow(_tmp262);_LL13C:;}};}
# 705
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
t=Cyc_Tcutil_compress(t);{
void*_tmp263=t;struct Cyc_Absyn_Tvar*_tmp265;void*_tmp267;struct Cyc_Core_Opt**_tmp268;struct Cyc_Absyn_PtrInfo _tmp26A;void*_tmp26C;struct Cyc_List_List*_tmp26E;void*_tmp26F;struct Cyc_Absyn_Tqual _tmp270;void*_tmp271;struct Cyc_List_List*_tmp272;int _tmp273;struct Cyc_Absyn_VarargInfo*_tmp274;struct Cyc_List_List*_tmp275;struct Cyc_List_List*_tmp276;struct Cyc_Absyn_Exp*_tmp277;struct Cyc_List_List*_tmp278;struct Cyc_Absyn_Exp*_tmp279;struct Cyc_List_List*_tmp27A;struct Cyc_List_List*_tmp27C;struct Cyc_List_List*_tmp27E;struct Cyc_List_List*_tmp280;struct Cyc_List_List*_tmp282;struct Cyc_List_List*_tmp284;struct Cyc_List_List*_tmp286;void*_tmp288;void*_tmp28A;void*_tmp28C;void*_tmp28E;struct Cyc_List_List*_tmp290;_LL142: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp264=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp263;if(_tmp264->tag != 2)goto _LL144;else{_tmp265=_tmp264->f1;}}_LL143:
# 710
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp265)){
{const char*_tmpFC9;Cyc_Tcutil_failure_reason=((_tmpFC9="(type variable would escape scope)",_tag_dyneither(_tmpFC9,sizeof(char),35)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 714
goto _LL141;_LL144: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp266=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp263;if(_tmp266->tag != 1)goto _LL146;else{_tmp267=(void*)_tmp266->f2;_tmp268=(struct Cyc_Core_Opt**)& _tmp266->f4;}}_LL145:
# 716
 if(t == evar){
{const char*_tmpFCA;Cyc_Tcutil_failure_reason=((_tmpFCA="(occurs check)",_tag_dyneither(_tmpFCA,sizeof(char),15)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{
# 720
if(_tmp267 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp267);else{
# 723
int problem=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp268))->v;for(0;s != 0;s=s->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}
# 730
if(problem){
struct Cyc_List_List*_tmp293=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp268))->v;for(0;s != 0;s=s->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
struct Cyc_List_List*_tmpFCB;_tmp293=((_tmpFCB=_cycalloc(sizeof(*_tmpFCB)),((_tmpFCB->hd=(struct Cyc_Absyn_Tvar*)s->hd,((_tmpFCB->tl=_tmp293,_tmpFCB))))));}}}{
# 736
struct Cyc_Core_Opt*_tmpFCC;*_tmp268=((_tmpFCC=_cycalloc(sizeof(*_tmpFCC)),((_tmpFCC->v=_tmp293,_tmpFCC))));};}}}
# 739
goto _LL141;_LL146: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp269=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp263;if(_tmp269->tag != 5)goto _LL148;else{_tmp26A=_tmp269->f1;}}_LL147:
# 742
 Cyc_Tcutil_occurs(evar,r,env,_tmp26A.elt_typ);
Cyc_Tcutil_occurs(evar,r,env,(_tmp26A.ptr_atts).rgn);
goto _LL141;_LL148: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp26B=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp263;if(_tmp26B->tag != 8)goto _LL14A;else{_tmp26C=(_tmp26B->f1).elt_type;}}_LL149:
# 746
 Cyc_Tcutil_occurs(evar,r,env,_tmp26C);goto _LL141;_LL14A: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp26D=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp263;if(_tmp26D->tag != 9)goto _LL14C;else{_tmp26E=(_tmp26D->f1).tvars;_tmp26F=(_tmp26D->f1).effect;_tmp270=(_tmp26D->f1).ret_tqual;_tmp271=(_tmp26D->f1).ret_typ;_tmp272=(_tmp26D->f1).args;_tmp273=(_tmp26D->f1).c_varargs;_tmp274=(_tmp26D->f1).cyc_varargs;_tmp275=(_tmp26D->f1).rgn_po;_tmp276=(_tmp26D->f1).attributes;_tmp277=(_tmp26D->f1).requires_clause;_tmp278=(_tmp26D->f1).requires_relns;_tmp279=(_tmp26D->f1).ensures_clause;_tmp27A=(_tmp26D->f1).ensures_relns;}}_LL14B:
# 749
 env=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp26E,env);
if(_tmp26F != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp26F);
Cyc_Tcutil_occurs(evar,r,env,_tmp271);
for(0;_tmp272 != 0;_tmp272=_tmp272->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple10*)_tmp272->hd)).f3);}
if(_tmp274 != 0)
Cyc_Tcutil_occurs(evar,r,env,_tmp274->type);
for(0;_tmp275 != 0;_tmp275=_tmp275->tl){
struct _tuple0*_tmp296=(struct _tuple0*)_tmp275->hd;void*_tmp298;void*_tmp299;struct _tuple0*_tmp297=_tmp296;_tmp298=_tmp297->f1;_tmp299=_tmp297->f2;
Cyc_Tcutil_occurs(evar,r,env,_tmp298);
Cyc_Tcutil_occurs(evar,r,env,_tmp299);}
# 761
goto _LL141;_LL14C: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp27B=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp263;if(_tmp27B->tag != 10)goto _LL14E;else{_tmp27C=_tmp27B->f1;}}_LL14D:
# 763
 for(0;_tmp27C != 0;_tmp27C=_tmp27C->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple12*)_tmp27C->hd)).f2);}
goto _LL141;_LL14E: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp27D=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp263;if(_tmp27D->tag != 3)goto _LL150;else{_tmp27E=(_tmp27D->f1).targs;}}_LL14F:
# 767
 Cyc_Tcutil_occurslist(evar,r,env,_tmp27E);goto _LL141;_LL150: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp27F=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp263;if(_tmp27F->tag != 17)goto _LL152;else{_tmp280=_tmp27F->f2;}}_LL151:
 _tmp282=_tmp280;goto _LL153;_LL152: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp281=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp263;if(_tmp281->tag != 4)goto _LL154;else{_tmp282=(_tmp281->f1).targs;}}_LL153:
 _tmp284=_tmp282;goto _LL155;_LL154: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp283=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp263;if(_tmp283->tag != 11)goto _LL156;else{_tmp284=(_tmp283->f1).targs;}}_LL155:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp284);goto _LL141;_LL156: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp285=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp263;if(_tmp285->tag != 12)goto _LL158;else{_tmp286=_tmp285->f2;}}_LL157:
# 773
 for(0;_tmp286 != 0;_tmp286=_tmp286->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp286->hd)->type);}
goto _LL141;_LL158: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp287=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp263;if(_tmp287->tag != 19)goto _LL15A;else{_tmp288=(void*)_tmp287->f1;}}_LL159:
 _tmp28A=_tmp288;goto _LL15B;_LL15A: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp289=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp263;if(_tmp289->tag != 23)goto _LL15C;else{_tmp28A=(void*)_tmp289->f1;}}_LL15B:
 _tmp28C=_tmp28A;goto _LL15D;_LL15C: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp28B=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp263;if(_tmp28B->tag != 15)goto _LL15E;else{_tmp28C=(void*)_tmp28B->f1;}}_LL15D:
 _tmp28E=_tmp28C;goto _LL15F;_LL15E: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp28D=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp263;if(_tmp28D->tag != 25)goto _LL160;else{_tmp28E=(void*)_tmp28D->f1;}}_LL15F:
 Cyc_Tcutil_occurs(evar,r,env,_tmp28E);goto _LL141;_LL160: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp28F=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp263;if(_tmp28F->tag != 24)goto _LL162;else{_tmp290=_tmp28F->f1;}}_LL161:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp290);goto _LL141;_LL162:;_LL163:
# 783
 goto _LL141;_LL141:;};}
# 786
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
# 788
for(0;ts != 0;ts=ts->tl){
Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}
# 793
static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){
Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0  || t2 != 0)
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 801
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){
if(tq1.print_const  && !tq1.real_const){
const char*_tmpFCF;void*_tmpFCE;(_tmpFCE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFCF="tq1 real_const not set.",_tag_dyneither(_tmpFCF,sizeof(char),24))),_tag_dyneither(_tmpFCE,sizeof(void*),0)));}
if(tq2.print_const  && !tq2.real_const){
const char*_tmpFD2;void*_tmpFD1;(_tmpFD1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFD2="tq2 real_const not set.",_tag_dyneither(_tmpFD2,sizeof(char),24))),_tag_dyneither(_tmpFD1,sizeof(void*),0)));}
# 807
if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
# 810
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;
{const char*_tmpFD3;Cyc_Tcutil_failure_reason=((_tmpFD3="(qualifiers don't match)",_tag_dyneither(_tmpFD3,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 818
Cyc_Tcutil_tq1_const=0;
Cyc_Tcutil_tq2_const=0;}
# 822
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
return(tq1.real_const == tq2.real_const  && tq1.q_volatile == tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}
# 829
static void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason){
# 831
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return;{
union Cyc_Absyn_Constraint*_tmp29F=x;void*_tmp2A0;_LL165: if((_tmp29F->No_constr).tag != 3)goto _LL167;_LL166:
# 836
{union Cyc_Absyn_Constraint _tmpFD4;*x=(((_tmpFD4.Forward_constr).val=y,(((_tmpFD4.Forward_constr).tag=2,_tmpFD4))));}return;_LL167: if((_tmp29F->Eq_constr).tag != 1)goto _LL169;_tmp2A0=(void*)(_tmp29F->Eq_constr).val;_LL168: {
# 838
union Cyc_Absyn_Constraint*_tmp2A2=y;void*_tmp2A3;_LL16C: if((_tmp2A2->No_constr).tag != 3)goto _LL16E;_LL16D:
*y=*x;return;_LL16E: if((_tmp2A2->Eq_constr).tag != 1)goto _LL170;_tmp2A3=(void*)(_tmp2A2->Eq_constr).val;_LL16F:
# 841
 if(cmp(_tmp2A0,_tmp2A3)!= 0){
Cyc_Tcutil_failure_reason=reason;
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 845
return;_LL170: if((_tmp2A2->Forward_constr).tag != 2)goto _LL16B;_LL171: {
const char*_tmpFD7;void*_tmpFD6;(_tmpFD6=0,Cyc_Tcutil_impos(((_tmpFD7="unify_conref: forward after compress(2)",_tag_dyneither(_tmpFD7,sizeof(char),40))),_tag_dyneither(_tmpFD6,sizeof(void*),0)));}_LL16B:;}_LL169: if((_tmp29F->Forward_constr).tag != 2)goto _LL164;_LL16A: {
# 848
const char*_tmpFDA;void*_tmpFD9;(_tmpFD9=0,Cyc_Tcutil_impos(((_tmpFDA="unify_conref: forward after compress",_tag_dyneither(_tmpFDA,sizeof(char),37))),_tag_dyneither(_tmpFD9,sizeof(void*),0)));}_LL164:;};}
# 852
static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
struct _handler_cons _tmp2A8;_push_handler(& _tmp2A8);{int _tmp2AA=0;if(setjmp(_tmp2A8.handler))_tmp2AA=1;if(!_tmp2AA){
Cyc_Tcutil_unify_it_conrefs(cmp,x,y,_tag_dyneither(0,0,0));{
int _tmp2AB=1;_npop_handler(0);return _tmp2AB;};
# 854
;_pop_handler();}else{void*_tmp2A9=(void*)_exn_thrown;void*_tmp2AD=_tmp2A9;void*_tmp2AF;_LL173: {struct Cyc_Tcutil_Unify_exn_struct*_tmp2AE=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp2AD;if(_tmp2AE->tag != Cyc_Tcutil_Unify)goto _LL175;}_LL174:
# 856
 return 0;_LL175: _tmp2AF=_tmp2AD;_LL176:(void)_rethrow(_tmp2AF);_LL172:;}};}
# 859
static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmpFDB;struct _tuple0 _tmp2B0=(_tmpFDB.f1=b1,((_tmpFDB.f2=b2,_tmpFDB)));struct _tuple0 _tmp2B1=_tmp2B0;struct Cyc_Absyn_Exp*_tmp2B7;struct Cyc_Absyn_Exp*_tmp2B9;_LL178:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2B2=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2B1.f1;if(_tmp2B2->tag != 0)goto _LL17A;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2B3=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2B1.f2;if(_tmp2B3->tag != 0)goto _LL17A;};_LL179:
 return 0;_LL17A: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2B4=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2B1.f1;if(_tmp2B4->tag != 0)goto _LL17C;}_LL17B:
 return - 1;_LL17C: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2B5=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2B1.f2;if(_tmp2B5->tag != 0)goto _LL17E;}_LL17D:
 return 1;_LL17E:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2B6=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2B1.f1;if(_tmp2B6->tag != 1)goto _LL177;else{_tmp2B7=_tmp2B6->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2B8=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2B1.f2;if(_tmp2B8->tag != 1)goto _LL177;else{_tmp2B9=_tmp2B8->f1;}};_LL17F:
# 865
 return Cyc_Evexp_const_exp_cmp(_tmp2B7,_tmp2B9);_LL177:;}
# 869
static void Cyc_Tcutil_unify_cmp_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
{struct _tuple0 _tmpFDC;struct _tuple0 _tmp2BB=(_tmpFDC.f1=e1->r,((_tmpFDC.f2=e2->r,_tmpFDC)));struct _tuple0 _tmp2BC=_tmp2BB;struct Cyc_Absyn_Exp*_tmp2BE;struct Cyc_Absyn_Exp*_tmp2C0;void*_tmp2C2;void*_tmp2C4;_LL181: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp2BD=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2BC.f1;if(_tmp2BD->tag != 13)goto _LL183;else{_tmp2BE=_tmp2BD->f2;}}_LL182:
# 875
 Cyc_Tcutil_unify_cmp_exp(_tmp2BE,e2);
return;_LL183: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp2BF=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2BC.f2;if(_tmp2BF->tag != 13)goto _LL185;else{_tmp2C0=_tmp2BF->f2;}}_LL184:
# 878
 Cyc_Tcutil_unify_cmp_exp(e1,_tmp2C0);
return;_LL185: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp2C1=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp2BC.f1;if(_tmp2C1->tag != 38)goto _LL187;else{_tmp2C2=(void*)_tmp2C1->f1;}}_LL186:
# 881
{void*_tmp2C5=Cyc_Tcutil_compress(_tmp2C2);void*_tmp2C6=_tmp2C5;_LL18C: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp2C7=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C6;if(_tmp2C7->tag != 1)goto _LL18E;}_LL18D:
# 883
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpFDF;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpFDE;Cyc_Tcutil_unify_it(_tmp2C2,(void*)((_tmpFDE=_cycalloc(sizeof(*_tmpFDE)),((_tmpFDE[0]=((_tmpFDF.tag=18,((_tmpFDF.f1=e2,_tmpFDF)))),_tmpFDE)))));}
return;_LL18E:;_LL18F:
# 886
 goto _LL18B;_LL18B:;}
# 888
goto _LL180;_LL187: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp2C3=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp2BC.f2;if(_tmp2C3->tag != 38)goto _LL189;else{_tmp2C4=(void*)_tmp2C3->f1;}}_LL188:
# 890
{void*_tmp2CA=Cyc_Tcutil_compress(_tmp2C4);void*_tmp2CB=_tmp2CA;_LL191: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp2CC=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2CB;if(_tmp2CC->tag != 1)goto _LL193;}_LL192:
# 892
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpFE2;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpFE1;Cyc_Tcutil_unify_it(_tmp2C4,(void*)((_tmpFE1=_cycalloc(sizeof(*_tmpFE1)),((_tmpFE1[0]=((_tmpFE2.tag=18,((_tmpFE2.f1=e1,_tmpFE2)))),_tmpFE1)))));}
return;_LL193:;_LL194:
# 895
 goto _LL190;_LL190:;}
# 897
goto _LL180;_LL189:;_LL18A:
 goto _LL180;_LL180:;}
# 900
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 903
static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){
struct _tuple0 _tmpFE3;struct _tuple0 _tmp2D0=(_tmpFE3.f1=b1,((_tmpFE3.f2=b2,_tmpFE3)));struct _tuple0 _tmp2D1=_tmp2D0;struct Cyc_Absyn_Exp*_tmp2D7;struct Cyc_Absyn_Exp*_tmp2D9;_LL196:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2D2=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2D1.f1;if(_tmp2D2->tag != 0)goto _LL198;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2D3=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2D1.f2;if(_tmp2D3->tag != 0)goto _LL198;};_LL197:
 return 0;_LL198: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2D4=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2D1.f1;if(_tmp2D4->tag != 0)goto _LL19A;}_LL199:
 return - 1;_LL19A: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2D5=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2D1.f2;if(_tmp2D5->tag != 0)goto _LL19C;}_LL19B:
 return 1;_LL19C:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2D6=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2D1.f1;if(_tmp2D6->tag != 1)goto _LL195;else{_tmp2D7=_tmp2D6->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2D8=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2D1.f2;if(_tmp2D8->tag != 1)goto _LL195;else{_tmp2D9=_tmp2D8->f1;}};_LL19D: {
# 909
int r=Cyc_Evexp_const_exp_cmp(_tmp2D7,_tmp2D9);
if(r == 0)return 0;{
struct _handler_cons _tmp2DA;_push_handler(& _tmp2DA);{int _tmp2DC=0;if(setjmp(_tmp2DA.handler))_tmp2DC=1;if(!_tmp2DC){
Cyc_Tcutil_unify_cmp_exp(_tmp2D7,_tmp2D9);{
int _tmp2DD=0;_npop_handler(0);return _tmp2DD;};
# 912
;_pop_handler();}else{void*_tmp2DB=(void*)_exn_thrown;void*_tmp2DF=_tmp2DB;void*_tmp2E1;_LL19F: {struct Cyc_Tcutil_Unify_exn_struct*_tmp2E0=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp2DF;if(_tmp2E0->tag != Cyc_Tcutil_Unify)goto _LL1A1;}_LL1A0:
# 915
 return r;_LL1A1: _tmp2E1=_tmp2DF;_LL1A2:(void)_rethrow(_tmp2E1);_LL19E:;}};};}_LL195:;}
# 920
static int Cyc_Tcutil_attribute_case_number(void*att){
void*_tmp2E3=att;_LL1A4: {struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2E4=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2E3;if(_tmp2E4->tag != 0)goto _LL1A6;}_LL1A5:
 return 0;_LL1A6: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp2E5=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp2E3;if(_tmp2E5->tag != 1)goto _LL1A8;}_LL1A7:
 return 1;_LL1A8: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp2E6=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp2E3;if(_tmp2E6->tag != 2)goto _LL1AA;}_LL1A9:
 return 2;_LL1AA: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp2E7=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp2E3;if(_tmp2E7->tag != 3)goto _LL1AC;}_LL1AB:
 return 3;_LL1AC: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp2E8=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp2E3;if(_tmp2E8->tag != 4)goto _LL1AE;}_LL1AD:
 return 4;_LL1AE: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp2E9=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp2E3;if(_tmp2E9->tag != 5)goto _LL1B0;}_LL1AF:
 return 5;_LL1B0: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2EA=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2E3;if(_tmp2EA->tag != 6)goto _LL1B2;}_LL1B1:
 return 6;_LL1B2: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp2EB=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp2E3;if(_tmp2EB->tag != 7)goto _LL1B4;}_LL1B3:
 return 7;_LL1B4: {struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2EC=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2E3;if(_tmp2EC->tag != 8)goto _LL1B6;}_LL1B5:
 return 8;_LL1B6: {struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*_tmp2ED=(struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*)_tmp2E3;if(_tmp2ED->tag != 9)goto _LL1B8;}_LL1B7:
 return 9;_LL1B8: {struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*_tmp2EE=(struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*)_tmp2E3;if(_tmp2EE->tag != 10)goto _LL1BA;}_LL1B9:
 return 10;_LL1BA: {struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*_tmp2EF=(struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*)_tmp2E3;if(_tmp2EF->tag != 11)goto _LL1BC;}_LL1BB:
 return 11;_LL1BC: {struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*_tmp2F0=(struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*)_tmp2E3;if(_tmp2F0->tag != 12)goto _LL1BE;}_LL1BD:
 return 12;_LL1BE: {struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*_tmp2F1=(struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*)_tmp2E3;if(_tmp2F1->tag != 13)goto _LL1C0;}_LL1BF:
 return 13;_LL1C0: {struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*_tmp2F2=(struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*)_tmp2E3;if(_tmp2F2->tag != 14)goto _LL1C2;}_LL1C1:
 return 14;_LL1C2: {struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*_tmp2F3=(struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*)_tmp2E3;if(_tmp2F3->tag != 15)goto _LL1C4;}_LL1C3:
 return 15;_LL1C4: {struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*_tmp2F4=(struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*)_tmp2E3;if(_tmp2F4->tag != 16)goto _LL1C6;}_LL1C5:
 return 16;_LL1C6: {struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*_tmp2F5=(struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*)_tmp2E3;if(_tmp2F5->tag != 17)goto _LL1C8;}_LL1C7:
 return 17;_LL1C8: {struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*_tmp2F6=(struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*)_tmp2E3;if(_tmp2F6->tag != 18)goto _LL1CA;}_LL1C9:
 return 18;_LL1CA: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp2F7=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2E3;if(_tmp2F7->tag != 19)goto _LL1CC;}_LL1CB:
 return 19;_LL1CC: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2F8=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2E3;if(_tmp2F8->tag != 20)goto _LL1CE;}_LL1CD:
 return 20;_LL1CE:;_LL1CF:
 return 21;_LL1A3:;}
# 947
static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple0 _tmpFE4;struct _tuple0 _tmp2F9=(_tmpFE4.f1=att1,((_tmpFE4.f2=att2,_tmpFE4)));struct _tuple0 _tmp2FA=_tmp2F9;int _tmp2FC;int _tmp2FE;int _tmp300;int _tmp302;struct Cyc_Absyn_Exp*_tmp304;struct Cyc_Absyn_Exp*_tmp306;struct _dyneither_ptr _tmp308;struct _dyneither_ptr _tmp30A;enum Cyc_Absyn_Format_Type _tmp30C;int _tmp30D;int _tmp30E;enum Cyc_Absyn_Format_Type _tmp310;int _tmp311;int _tmp312;_LL1D1:{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2FB=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2FA.f1;if(_tmp2FB->tag != 0)goto _LL1D3;else{_tmp2FC=_tmp2FB->f1;}}{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2FD=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2FA.f2;if(_tmp2FD->tag != 0)goto _LL1D3;else{_tmp2FE=_tmp2FD->f1;}};_LL1D2:
 _tmp300=_tmp2FC;_tmp302=_tmp2FE;goto _LL1D4;_LL1D3:{struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2FF=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2FA.f1;if(_tmp2FF->tag != 20)goto _LL1D5;else{_tmp300=_tmp2FF->f1;}}{struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp301=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2FA.f2;if(_tmp301->tag != 20)goto _LL1D5;else{_tmp302=_tmp301->f1;}};_LL1D4:
# 951
 return Cyc_Core_intcmp(_tmp300,_tmp302);_LL1D5:{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp303=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2FA.f1;if(_tmp303->tag != 6)goto _LL1D7;else{_tmp304=_tmp303->f1;}}{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp305=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2FA.f2;if(_tmp305->tag != 6)goto _LL1D7;else{_tmp306=_tmp305->f1;}};_LL1D6:
# 953
 if(_tmp304 == _tmp306)return 0;
if(_tmp304 == 0)return - 1;
if(_tmp306 == 0)return 1;
return Cyc_Evexp_const_exp_cmp(_tmp304,_tmp306);_LL1D7:{struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp307=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2FA.f1;if(_tmp307->tag != 8)goto _LL1D9;else{_tmp308=_tmp307->f1;}}{struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp309=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2FA.f2;if(_tmp309->tag != 8)goto _LL1D9;else{_tmp30A=_tmp309->f1;}};_LL1D8:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp308,(struct _dyneither_ptr)_tmp30A);_LL1D9:{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp30B=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2FA.f1;if(_tmp30B->tag != 19)goto _LL1DB;else{_tmp30C=_tmp30B->f1;_tmp30D=_tmp30B->f2;_tmp30E=_tmp30B->f3;}}{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp30F=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2FA.f2;if(_tmp30F->tag != 19)goto _LL1DB;else{_tmp310=_tmp30F->f1;_tmp311=_tmp30F->f2;_tmp312=_tmp30F->f3;}};_LL1DA: {
# 959
int _tmp313=Cyc_Core_intcmp((int)((unsigned int)_tmp30C),(int)((unsigned int)_tmp310));
if(_tmp313 != 0)return _tmp313;{
int _tmp314=Cyc_Core_intcmp(_tmp30D,_tmp311);
if(_tmp314 != 0)return _tmp314;
return Cyc_Core_intcmp(_tmp30E,_tmp312);};}_LL1DB:;_LL1DC:
# 965
 return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(att2));_LL1D0:;}
# 969
static int Cyc_Tcutil_equal_att(void*a1,void*a2){
return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}
# 973
int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}
return 1;}
# 982
static void*Cyc_Tcutil_rgns_of(void*t);
# 984
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 988
static struct _tuple16*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_tmp316=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp317=_tmp316;_LL1DE: if(_tmp317->kind != Cyc_Absyn_RgnKind)goto _LL1E0;if(_tmp317->aliasqual != Cyc_Absyn_Unique)goto _LL1E0;_LL1DF:
 t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL1DD;_LL1E0: if(_tmp317->kind != Cyc_Absyn_RgnKind)goto _LL1E2;if(_tmp317->aliasqual != Cyc_Absyn_Aliasable)goto _LL1E2;_LL1E1:
 t=(void*)& Cyc_Absyn_HeapRgn_val;goto _LL1DD;_LL1E2: if(_tmp317->kind != Cyc_Absyn_EffKind)goto _LL1E4;_LL1E3:
 t=Cyc_Absyn_empty_effect;goto _LL1DD;_LL1E4: if(_tmp317->kind != Cyc_Absyn_IntKind)goto _LL1E6;_LL1E5:
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpFE7;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpFE6;t=(void*)((_tmpFE6=_cycalloc(sizeof(*_tmpFE6)),((_tmpFE6[0]=((_tmpFE7.tag=18,((_tmpFE7.f1=Cyc_Absyn_uint_exp(0,0),_tmpFE7)))),_tmpFE6))));}goto _LL1DD;_LL1E6:;_LL1E7:
 t=Cyc_Absyn_sint_typ;goto _LL1DD;_LL1DD:;}{
# 997
struct _tuple16*_tmpFE8;return(_tmpFE8=_cycalloc(sizeof(*_tmpFE8)),((_tmpFE8->f1=tv,((_tmpFE8->f2=t,_tmpFE8)))));};}
# 1004
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp31B=Cyc_Tcutil_compress(t);void*_tmp31C=_tmp31B;void*_tmp327;void*_tmp329;void*_tmp32A;struct Cyc_List_List*_tmp32C;void*_tmp32E;void*_tmp32F;void*_tmp331;struct Cyc_List_List*_tmp333;struct Cyc_List_List*_tmp335;struct Cyc_List_List*_tmp337;struct Cyc_List_List*_tmp339;struct Cyc_List_List*_tmp33C;void*_tmp33D;struct Cyc_Absyn_Tqual _tmp33E;void*_tmp33F;struct Cyc_List_List*_tmp340;struct Cyc_Absyn_VarargInfo*_tmp341;struct Cyc_List_List*_tmp342;void*_tmp349;struct Cyc_List_List*_tmp34B;_LL1E9: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp31D=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp31C;if(_tmp31D->tag != 0)goto _LL1EB;}_LL1EA:
 goto _LL1EC;_LL1EB: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp31E=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp31C;if(_tmp31E->tag != 7)goto _LL1ED;}_LL1EC:
 goto _LL1EE;_LL1ED: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp31F=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp31C;if(_tmp31F->tag != 13)goto _LL1EF;}_LL1EE:
 goto _LL1F0;_LL1EF: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp320=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp31C;if(_tmp320->tag != 14)goto _LL1F1;}_LL1F0:
 goto _LL1F2;_LL1F1: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp321=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp31C;if(_tmp321->tag != 18)goto _LL1F3;}_LL1F2:
 goto _LL1F4;_LL1F3: {struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp322=(struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp31C;if(_tmp322->tag != 27)goto _LL1F5;}_LL1F4:
 goto _LL1F6;_LL1F5: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp323=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp31C;if(_tmp323->tag != 6)goto _LL1F7;}_LL1F6:
 return Cyc_Absyn_empty_effect;_LL1F7: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp324=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp31C;if(_tmp324->tag != 1)goto _LL1F9;}_LL1F8:
 goto _LL1FA;_LL1F9: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp325=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp31C;if(_tmp325->tag != 2)goto _LL1FB;}_LL1FA: {
# 1015
struct Cyc_Absyn_Kind*_tmp34D=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp34E=_tmp34D;_LL222: if(_tmp34E->kind != Cyc_Absyn_RgnKind)goto _LL224;_LL223: {
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpFEB;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpFEA;return(void*)((_tmpFEA=_cycalloc(sizeof(*_tmpFEA)),((_tmpFEA[0]=((_tmpFEB.tag=23,((_tmpFEB.f1=t,_tmpFEB)))),_tmpFEA))));}_LL224: if(_tmp34E->kind != Cyc_Absyn_EffKind)goto _LL226;_LL225:
 return t;_LL226: if(_tmp34E->kind != Cyc_Absyn_IntKind)goto _LL228;_LL227:
 return Cyc_Absyn_empty_effect;_LL228:;_LL229: {
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpFEE;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpFED;return(void*)((_tmpFED=_cycalloc(sizeof(*_tmpFED)),((_tmpFED[0]=((_tmpFEE.tag=25,((_tmpFEE.f1=t,_tmpFEE)))),_tmpFED))));}_LL221:;}_LL1FB: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp326=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp31C;if(_tmp326->tag != 15)goto _LL1FD;else{_tmp327=(void*)_tmp326->f1;}}_LL1FC: {
# 1021
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpFF1;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpFF0;return(void*)((_tmpFF0=_cycalloc(sizeof(*_tmpFF0)),((_tmpFF0[0]=((_tmpFF1.tag=23,((_tmpFF1.f1=_tmp327,_tmpFF1)))),_tmpFF0))));}_LL1FD: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp328=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp31C;if(_tmp328->tag != 16)goto _LL1FF;else{_tmp329=(void*)_tmp328->f1;_tmp32A=(void*)_tmp328->f2;}}_LL1FE: {
# 1025
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpFF4;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpFF3;return(void*)((_tmpFF3=_cycalloc(sizeof(*_tmpFF3)),((_tmpFF3[0]=((_tmpFF4.tag=23,((_tmpFF4.f1=_tmp32A,_tmpFF4)))),_tmpFF3))));}_LL1FF: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp32B=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp31C;if(_tmp32B->tag != 3)goto _LL201;else{_tmp32C=(_tmp32B->f1).targs;}}_LL200: {
# 1027
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp32C);
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpFF7;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpFF6;return Cyc_Tcutil_normalize_effect((void*)((_tmpFF6=_cycalloc(sizeof(*_tmpFF6)),((_tmpFF6[0]=((_tmpFF7.tag=24,((_tmpFF7.f1=ts,_tmpFF7)))),_tmpFF6)))));}_LL201: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp32D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp31C;if(_tmp32D->tag != 5)goto _LL203;else{_tmp32E=(_tmp32D->f1).elt_typ;_tmp32F=((_tmp32D->f1).ptr_atts).rgn;}}_LL202: {
# 1031
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1006;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1005;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1004;void*_tmp1003[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1002;return Cyc_Tcutil_normalize_effect((void*)((_tmp1002=_cycalloc(sizeof(*_tmp1002)),((_tmp1002[0]=((_tmp1006.tag=24,((_tmp1006.f1=((_tmp1003[1]=Cyc_Tcutil_rgns_of(_tmp32E),((_tmp1003[0]=(void*)((_tmp1005=_cycalloc(sizeof(*_tmp1005)),((_tmp1005[0]=((_tmp1004.tag=23,((_tmp1004.f1=_tmp32F,_tmp1004)))),_tmp1005)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1003,sizeof(void*),2)))))),_tmp1006)))),_tmp1002)))));}_LL203: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp330=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp31C;if(_tmp330->tag != 8)goto _LL205;else{_tmp331=(_tmp330->f1).elt_type;}}_LL204:
# 1033
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp331));_LL205: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp332=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp31C;if(_tmp332->tag != 10)goto _LL207;else{_tmp333=_tmp332->f1;}}_LL206: {
# 1035
struct Cyc_List_List*_tmp35E=0;
for(0;_tmp333 != 0;_tmp333=_tmp333->tl){
struct Cyc_List_List*_tmp1007;_tmp35E=((_tmp1007=_cycalloc(sizeof(*_tmp1007)),((_tmp1007->hd=(*((struct _tuple12*)_tmp333->hd)).f2,((_tmp1007->tl=_tmp35E,_tmp1007))))));}
_tmp335=_tmp35E;goto _LL208;}_LL207: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp334=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp31C;if(_tmp334->tag != 4)goto _LL209;else{_tmp335=(_tmp334->f1).targs;}}_LL208:
 _tmp337=_tmp335;goto _LL20A;_LL209: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp336=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp31C;if(_tmp336->tag != 11)goto _LL20B;else{_tmp337=(_tmp336->f1).targs;}}_LL20A: {
# 1041
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp100A;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1009;return Cyc_Tcutil_normalize_effect((void*)((_tmp1009=_cycalloc(sizeof(*_tmp1009)),((_tmp1009[0]=((_tmp100A.tag=24,((_tmp100A.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp337),_tmp100A)))),_tmp1009)))));}_LL20B: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp338=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp31C;if(_tmp338->tag != 12)goto _LL20D;else{_tmp339=_tmp338->f2;}}_LL20C: {
# 1043
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp100D;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp100C;return Cyc_Tcutil_normalize_effect((void*)((_tmp100C=_cycalloc(sizeof(*_tmp100C)),((_tmp100C[0]=((_tmp100D.tag=24,((_tmp100D.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp339),_tmp100D)))),_tmp100C)))));}_LL20D: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp33A=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp31C;if(_tmp33A->tag != 19)goto _LL20F;}_LL20E:
 return Cyc_Absyn_empty_effect;_LL20F: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp33B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp31C;if(_tmp33B->tag != 9)goto _LL211;else{_tmp33C=(_tmp33B->f1).tvars;_tmp33D=(_tmp33B->f1).effect;_tmp33E=(_tmp33B->f1).ret_tqual;_tmp33F=(_tmp33B->f1).ret_typ;_tmp340=(_tmp33B->f1).args;_tmp341=(_tmp33B->f1).cyc_varargs;_tmp342=(_tmp33B->f1).rgn_po;}}_LL210: {
# 1053
void*_tmp364=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp33C),(void*)_check_null(_tmp33D));
return Cyc_Tcutil_normalize_effect(_tmp364);}_LL211: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp343=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp31C;if(_tmp343->tag != 21)goto _LL213;}_LL212:
 goto _LL214;_LL213: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp344=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp31C;if(_tmp344->tag != 22)goto _LL215;}_LL214:
 goto _LL216;_LL215: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp345=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp31C;if(_tmp345->tag != 20)goto _LL217;}_LL216:
 return Cyc_Absyn_empty_effect;_LL217: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp346=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp31C;if(_tmp346->tag != 23)goto _LL219;}_LL218:
 goto _LL21A;_LL219: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp347=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp31C;if(_tmp347->tag != 24)goto _LL21B;}_LL21A:
 return t;_LL21B: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp348=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp31C;if(_tmp348->tag != 25)goto _LL21D;else{_tmp349=(void*)_tmp348->f1;}}_LL21C:
 return Cyc_Tcutil_rgns_of(_tmp349);_LL21D: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp34A=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp31C;if(_tmp34A->tag != 17)goto _LL21F;else{_tmp34B=_tmp34A->f2;}}_LL21E: {
# 1062
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1010;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp100F;return Cyc_Tcutil_normalize_effect((void*)((_tmp100F=_cycalloc(sizeof(*_tmp100F)),((_tmp100F[0]=((_tmp1010.tag=24,((_tmp1010.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp34B),_tmp1010)))),_tmp100F)))));}_LL21F: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp34C=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp31C;if(_tmp34C->tag != 26)goto _LL1E8;}_LL220: {
const char*_tmp1013;void*_tmp1012;(_tmp1012=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1013="typedecl in rgns_of",_tag_dyneither(_tmp1013,sizeof(char),20))),_tag_dyneither(_tmp1012,sizeof(void*),0)));}_LL1E8:;}
# 1070
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp369=e;struct Cyc_List_List**_tmp36B;void*_tmp36D;_LL22B: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp36A=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp369;if(_tmp36A->tag != 24)goto _LL22D;else{_tmp36B=(struct Cyc_List_List**)& _tmp36A->f1;}}_LL22C: {
# 1074
int redo_join=0;
{struct Cyc_List_List*effs=*_tmp36B;for(0;effs != 0;effs=effs->tl){
void*_tmp36E=(void*)effs->hd;
effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp36E));{
void*_tmp36F=(void*)effs->hd;void*_tmp370=_tmp36F;_LL232: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp371=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp370;if(_tmp371->tag != 24)goto _LL234;}_LL233:
 goto _LL235;_LL234: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp372=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp370;if(_tmp372->tag != 23)goto _LL236;else{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp373=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)((void*)_tmp372->f1);if(_tmp373->tag != 20)goto _LL236;}}_LL235:
 redo_join=1;goto _LL231;_LL236: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp374=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp370;if(_tmp374->tag != 23)goto _LL238;else{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp375=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)((void*)_tmp374->f1);if(_tmp375->tag != 22)goto _LL238;}}_LL237:
 redo_join=1;goto _LL231;_LL238: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp376=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp370;if(_tmp376->tag != 23)goto _LL23A;else{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp377=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)((void*)_tmp376->f1);if(_tmp377->tag != 21)goto _LL23A;}}_LL239:
 redo_join=1;goto _LL231;_LL23A:;_LL23B:
 goto _LL231;_LL231:;};}}
# 1086
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*_tmp36B;for(0;effs != 0;effs=effs->tl){
void*_tmp378=Cyc_Tcutil_compress((void*)effs->hd);void*_tmp379=_tmp378;struct Cyc_List_List*_tmp37B;void*_tmp382;_LL23D: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp37A=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp379;if(_tmp37A->tag != 24)goto _LL23F;else{_tmp37B=_tmp37A->f1;}}_LL23E:
# 1091
 effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp37B,effects);
goto _LL23C;_LL23F: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp37C=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp379;if(_tmp37C->tag != 23)goto _LL241;else{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp37D=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)((void*)_tmp37C->f1);if(_tmp37D->tag != 20)goto _LL241;}}_LL240:
 goto _LL23C;_LL241: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp37E=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp379;if(_tmp37E->tag != 23)goto _LL243;else{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp37F=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)((void*)_tmp37E->f1);if(_tmp37F->tag != 22)goto _LL243;}}_LL242:
 goto _LL23C;_LL243: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp380=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp379;if(_tmp380->tag != 23)goto _LL245;else{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp381=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)((void*)_tmp380->f1);if(_tmp381->tag != 21)goto _LL245;}}_LL244:
 goto _LL23C;_LL245: _tmp382=_tmp379;_LL246:
{struct Cyc_List_List*_tmp1014;effects=((_tmp1014=_cycalloc(sizeof(*_tmp1014)),((_tmp1014->hd=_tmp382,((_tmp1014->tl=effects,_tmp1014))))));}goto _LL23C;_LL23C:;}}
# 1099
*_tmp36B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
return e;};}_LL22D: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp36C=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp369;if(_tmp36C->tag != 25)goto _LL22F;else{_tmp36D=(void*)_tmp36C->f1;}}_LL22E: {
# 1102
void*_tmp384=Cyc_Tcutil_compress(_tmp36D);void*_tmp385=_tmp384;_LL248: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp386=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp385;if(_tmp386->tag != 1)goto _LL24A;}_LL249:
 goto _LL24B;_LL24A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp387=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp385;if(_tmp387->tag != 2)goto _LL24C;}_LL24B:
 return e;_LL24C:;_LL24D:
 return Cyc_Tcutil_rgns_of(_tmp36D);_LL247:;}_LL22F:;_LL230:
# 1107
 return e;_LL22A:;};}
# 1112
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp101A;struct Cyc_Absyn_FnInfo _tmp1019;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1018;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp388=(_tmp1018=_cycalloc(sizeof(*_tmp1018)),((_tmp1018[0]=((_tmp101A.tag=9,((_tmp101A.f1=((_tmp1019.tvars=0,((_tmp1019.effect=eff,((_tmp1019.ret_tqual=
Cyc_Absyn_empty_tqual(0),((_tmp1019.ret_typ=(void*)& Cyc_Absyn_VoidType_val,((_tmp1019.args=0,((_tmp1019.c_varargs=0,((_tmp1019.cyc_varargs=0,((_tmp1019.rgn_po=0,((_tmp1019.attributes=0,((_tmp1019.requires_clause=0,((_tmp1019.requires_relns=0,((_tmp1019.ensures_clause=0,((_tmp1019.ensures_relns=0,_tmp1019)))))))))))))))))))))))))),_tmp101A)))),_tmp1018)));
# 1122
return Cyc_Absyn_atb_typ((void*)_tmp388,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}
# 1129
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);
if((r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (void*)& Cyc_Absyn_UniqueRgn_val) || r == (void*)& Cyc_Absyn_RefCntRgn_val)
return 1;{
void*_tmp38C=Cyc_Tcutil_compress(e);void*_tmp38D=_tmp38C;void*_tmp38F;struct Cyc_List_List*_tmp391;void*_tmp393;struct Cyc_Core_Opt*_tmp395;void**_tmp396;struct Cyc_Core_Opt*_tmp397;_LL24F: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp38E=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp38D;if(_tmp38E->tag != 23)goto _LL251;else{_tmp38F=(void*)_tmp38E->f1;}}_LL250:
# 1136
 if(constrain)return Cyc_Tcutil_unify(r,_tmp38F);
_tmp38F=Cyc_Tcutil_compress(_tmp38F);
if(r == _tmp38F)return 1;{
struct _tuple0 _tmp101B;struct _tuple0 _tmp398=(_tmp101B.f1=r,((_tmp101B.f2=_tmp38F,_tmp101B)));struct _tuple0 _tmp399=_tmp398;struct Cyc_Absyn_Tvar*_tmp39B;struct Cyc_Absyn_Tvar*_tmp39D;_LL25A:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp39A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp399.f1;if(_tmp39A->tag != 2)goto _LL25C;else{_tmp39B=_tmp39A->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp39C=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp399.f2;if(_tmp39C->tag != 2)goto _LL25C;else{_tmp39D=_tmp39C->f1;}};_LL25B:
 return Cyc_Absyn_tvar_cmp(_tmp39B,_tmp39D)== 0;_LL25C:;_LL25D:
 return 0;_LL259:;};_LL251: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp390=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp38D;if(_tmp390->tag != 24)goto _LL253;else{_tmp391=_tmp390->f1;}}_LL252:
# 1144
 for(0;_tmp391 != 0;_tmp391=_tmp391->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp391->hd))return 1;}
return 0;_LL253: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp392=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp38D;if(_tmp392->tag != 25)goto _LL255;else{_tmp393=(void*)_tmp392->f1;}}_LL254: {
# 1148
void*_tmp39F=Cyc_Tcutil_rgns_of(_tmp393);void*_tmp3A0=_tmp39F;void*_tmp3A2;void*_tmp3A3;_LL25F: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3A1=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3A0;if(_tmp3A1->tag != 25)goto _LL261;else{_tmp3A2=(void*)_tmp3A1->f1;}}_LL260:
# 1150
 if(!constrain)return 0;{
void*_tmp3A4=Cyc_Tcutil_compress(_tmp3A2);void*_tmp3A5=_tmp3A4;struct Cyc_Core_Opt*_tmp3A7;void**_tmp3A8;struct Cyc_Core_Opt*_tmp3A9;_LL264: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3A6=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3A5;if(_tmp3A6->tag != 1)goto _LL266;else{_tmp3A7=_tmp3A6->f1;_tmp3A8=(void**)((void**)& _tmp3A6->f2);_tmp3A9=_tmp3A6->f4;}}_LL265: {
# 1155
void*_tmp3AA=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp3A9);
# 1158
Cyc_Tcutil_occurs(_tmp3AA,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3A9))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp102A;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1029;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1028;void*_tmp1027[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1026;void*_tmp3AB=Cyc_Tcutil_dummy_fntype((void*)((_tmp1026=_cycalloc(sizeof(*_tmp1026)),((_tmp1026[0]=((_tmp102A.tag=24,((_tmp102A.f1=((_tmp1027[1]=(void*)((_tmp1029=_cycalloc(sizeof(*_tmp1029)),((_tmp1029[0]=((_tmp1028.tag=23,((_tmp1028.f1=r,_tmp1028)))),_tmp1029)))),((_tmp1027[0]=_tmp3AA,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1027,sizeof(void*),2)))))),_tmp102A)))),_tmp1026)))));
*_tmp3A8=_tmp3AB;
return 1;};}_LL266:;_LL267:
 return 0;_LL263:;};_LL261: _tmp3A3=_tmp3A0;_LL262:
# 1164
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp3A3);_LL25E:;}_LL255: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp394=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp38D;if(_tmp394->tag != 1)goto _LL257;else{_tmp395=_tmp394->f1;_tmp396=(void**)((void**)& _tmp394->f2);_tmp397=_tmp394->f4;}}_LL256:
# 1167
 if(_tmp395 == 0  || ((struct Cyc_Absyn_Kind*)_tmp395->v)->kind != Cyc_Absyn_EffKind){
const char*_tmp102D;void*_tmp102C;(_tmp102C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp102D="effect evar has wrong kind",_tag_dyneither(_tmp102D,sizeof(char),27))),_tag_dyneither(_tmp102C,sizeof(void*),0)));}
if(!constrain)return 0;{
# 1172
void*_tmp3B3=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp397);
# 1175
Cyc_Tcutil_occurs(_tmp3B3,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp397))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1042;struct Cyc_List_List*_tmp1041;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1040;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp103F;struct Cyc_List_List*_tmp103E;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp103D;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3B4=(_tmp103D=_cycalloc(sizeof(*_tmp103D)),((_tmp103D[0]=((_tmp1042.tag=24,((_tmp1042.f1=((_tmp103E=_cycalloc(sizeof(*_tmp103E)),((_tmp103E->hd=_tmp3B3,((_tmp103E->tl=((_tmp1041=_cycalloc(sizeof(*_tmp1041)),((_tmp1041->hd=(void*)((_tmp103F=_cycalloc(sizeof(*_tmp103F)),((_tmp103F[0]=((_tmp1040.tag=23,((_tmp1040.f1=r,_tmp1040)))),_tmp103F)))),((_tmp1041->tl=0,_tmp1041)))))),_tmp103E)))))),_tmp1042)))),_tmp103D)));
*_tmp396=(void*)_tmp3B4;
return 1;};};_LL257:;_LL258:
 return 0;_LL24E:;};}
# 1186
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Tcutil_compress(t);{
void*_tmp3BB=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_tmp3BC=_tmp3BB;struct Cyc_List_List*_tmp3BF;void*_tmp3C1;struct Cyc_Core_Opt*_tmp3C3;void**_tmp3C4;struct Cyc_Core_Opt*_tmp3C5;_LL269: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3BD=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3BC;if(_tmp3BD->tag != 23)goto _LL26B;}_LL26A:
 return 0;_LL26B: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3BE=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3BC;if(_tmp3BE->tag != 24)goto _LL26D;else{_tmp3BF=_tmp3BE->f1;}}_LL26C:
# 1191
 for(0;_tmp3BF != 0;_tmp3BF=_tmp3BF->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp3BF->hd))
return 1;}
return 0;_LL26D: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3C0=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3BC;if(_tmp3C0->tag != 25)goto _LL26F;else{_tmp3C1=(void*)_tmp3C0->f1;}}_LL26E:
# 1196
 _tmp3C1=Cyc_Tcutil_compress(_tmp3C1);
if(t == _tmp3C1)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp3C1);{
void*_tmp3C6=Cyc_Tcutil_rgns_of(t);void*_tmp3C7=_tmp3C6;void*_tmp3C9;void*_tmp3CA;_LL274: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3C8=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3C7;if(_tmp3C8->tag != 25)goto _LL276;else{_tmp3C9=(void*)_tmp3C8->f1;}}_LL275: {
# 1201
struct _tuple0 _tmp1043;struct _tuple0 _tmp3CB=(_tmp1043.f1=Cyc_Tcutil_compress(_tmp3C9),((_tmp1043.f2=_tmp3C1,_tmp1043)));struct _tuple0 _tmp3CC=_tmp3CB;struct Cyc_Absyn_Tvar*_tmp3CE;struct Cyc_Absyn_Tvar*_tmp3D0;_LL279:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3CD=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3CC.f1;if(_tmp3CD->tag != 2)goto _LL27B;else{_tmp3CE=_tmp3CD->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3CF=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3CC.f2;if(_tmp3CF->tag != 2)goto _LL27B;else{_tmp3D0=_tmp3CF->f1;}};_LL27A:
 return Cyc_Tcutil_unify(t,_tmp3C1);_LL27B:;_LL27C:
 return _tmp3C9 == _tmp3C1;_LL278:;}_LL276: _tmp3CA=_tmp3C7;_LL277:
# 1205
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp3CA);_LL273:;};_LL26F: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3C2=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3BC;if(_tmp3C2->tag != 1)goto _LL271;else{_tmp3C3=_tmp3C2->f1;_tmp3C4=(void**)((void**)& _tmp3C2->f2);_tmp3C5=_tmp3C2->f4;}}_LL270:
# 1208
 if(_tmp3C3 == 0  || ((struct Cyc_Absyn_Kind*)_tmp3C3->v)->kind != Cyc_Absyn_EffKind){
const char*_tmp1046;void*_tmp1045;(_tmp1045=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1046="effect evar has wrong kind",_tag_dyneither(_tmp1046,sizeof(char),27))),_tag_dyneither(_tmp1045,sizeof(void*),0)));}
if(!may_constrain_evars)return 0;{
# 1213
void*_tmp3D4=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp3C5);
# 1216
Cyc_Tcutil_occurs(_tmp3D4,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3C5))->v,t);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp105B;struct Cyc_List_List*_tmp105A;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp1059;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1058;struct Cyc_List_List*_tmp1057;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1056;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3D5=(_tmp1056=_cycalloc(sizeof(*_tmp1056)),((_tmp1056[0]=((_tmp105B.tag=24,((_tmp105B.f1=((_tmp1057=_cycalloc(sizeof(*_tmp1057)),((_tmp1057->hd=_tmp3D4,((_tmp1057->tl=((_tmp105A=_cycalloc(sizeof(*_tmp105A)),((_tmp105A->hd=(void*)((_tmp1058=_cycalloc(sizeof(*_tmp1058)),((_tmp1058[0]=((_tmp1059.tag=25,((_tmp1059.f1=t,_tmp1059)))),_tmp1058)))),((_tmp105A->tl=0,_tmp105A)))))),_tmp1057)))))),_tmp105B)))),_tmp1056)));
*_tmp3C4=(void*)_tmp3D5;
return 1;};};_LL271:;_LL272:
 return 0;_LL268:;};}
# 1227
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp3DC=e;struct Cyc_Absyn_Tvar*_tmp3DE;struct Cyc_List_List*_tmp3E0;void*_tmp3E2;struct Cyc_Core_Opt*_tmp3E4;void**_tmp3E5;struct Cyc_Core_Opt*_tmp3E6;_LL27E: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3DD=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3DC;if(_tmp3DD->tag != 2)goto _LL280;else{_tmp3DE=_tmp3DD->f1;}}_LL27F:
 return Cyc_Absyn_tvar_cmp(v,_tmp3DE)== 0;_LL280: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3DF=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3DC;if(_tmp3DF->tag != 24)goto _LL282;else{_tmp3E0=_tmp3DF->f1;}}_LL281:
# 1232
 for(0;_tmp3E0 != 0;_tmp3E0=_tmp3E0->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)_tmp3E0->hd))
return 1;}
return 0;_LL282: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3E1=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3DC;if(_tmp3E1->tag != 25)goto _LL284;else{_tmp3E2=(void*)_tmp3E1->f1;}}_LL283: {
# 1237
void*_tmp3E7=Cyc_Tcutil_rgns_of(_tmp3E2);void*_tmp3E8=_tmp3E7;void*_tmp3EA;void*_tmp3EB;_LL289: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3E9=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3E8;if(_tmp3E9->tag != 25)goto _LL28B;else{_tmp3EA=(void*)_tmp3E9->f1;}}_LL28A:
# 1239
 if(!may_constrain_evars)return 0;{
void*_tmp3EC=Cyc_Tcutil_compress(_tmp3EA);void*_tmp3ED=_tmp3EC;struct Cyc_Core_Opt*_tmp3EF;void**_tmp3F0;struct Cyc_Core_Opt*_tmp3F1;_LL28E: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3EE=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3ED;if(_tmp3EE->tag != 1)goto _LL290;else{_tmp3EF=_tmp3EE->f1;_tmp3F0=(void**)((void**)& _tmp3EE->f2);_tmp3F1=_tmp3EE->f4;}}_LL28F: {
# 1245
void*_tmp3F2=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp3F1);
# 1247
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3F1))->v,v))return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp106A;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1069;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1068;void*_tmp1067[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1066;void*_tmp3F3=Cyc_Tcutil_dummy_fntype((void*)((_tmp1066=_cycalloc(sizeof(*_tmp1066)),((_tmp1066[0]=((_tmp106A.tag=24,((_tmp106A.f1=((_tmp1067[1]=(void*)((_tmp1069=_cycalloc(sizeof(*_tmp1069)),((_tmp1069[0]=((_tmp1068.tag=2,((_tmp1068.f1=v,_tmp1068)))),_tmp1069)))),((_tmp1067[0]=_tmp3F2,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1067,sizeof(void*),2)))))),_tmp106A)))),_tmp1066)))));
*_tmp3F0=_tmp3F3;
return 1;};}_LL290:;_LL291:
 return 0;_LL28D:;};_LL28B: _tmp3EB=_tmp3E8;_LL28C:
# 1253
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp3EB);_LL288:;}_LL284: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3E3=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3DC;if(_tmp3E3->tag != 1)goto _LL286;else{_tmp3E4=_tmp3E3->f1;_tmp3E5=(void**)((void**)& _tmp3E3->f2);_tmp3E6=_tmp3E3->f4;}}_LL285:
# 1256
 if(_tmp3E4 == 0  || ((struct Cyc_Absyn_Kind*)_tmp3E4->v)->kind != Cyc_Absyn_EffKind){
const char*_tmp106D;void*_tmp106C;(_tmp106C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp106D="effect evar has wrong kind",_tag_dyneither(_tmp106D,sizeof(char),27))),_tag_dyneither(_tmp106C,sizeof(void*),0)));}{
# 1260
void*_tmp3FB=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp3E6);
# 1262
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3E6))->v,v))
return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1082;struct Cyc_List_List*_tmp1081;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1080;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp107F;struct Cyc_List_List*_tmp107E;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp107D;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3FC=(_tmp107D=_cycalloc(sizeof(*_tmp107D)),((_tmp107D[0]=((_tmp1082.tag=24,((_tmp1082.f1=((_tmp107E=_cycalloc(sizeof(*_tmp107E)),((_tmp107E->hd=_tmp3FB,((_tmp107E->tl=((_tmp1081=_cycalloc(sizeof(*_tmp1081)),((_tmp1081->hd=(void*)((_tmp107F=_cycalloc(sizeof(*_tmp107F)),((_tmp107F[0]=((_tmp1080.tag=2,((_tmp1080.f1=v,_tmp1080)))),_tmp107F)))),((_tmp1081->tl=0,_tmp1081)))))),_tmp107E)))))),_tmp1082)))),_tmp107D)));
*_tmp3E5=(void*)_tmp3FC;
return 1;};};_LL286:;_LL287:
 return 0;_LL27D:;};}
# 1272
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp403=e;struct Cyc_List_List*_tmp405;void*_tmp407;_LL293: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp404=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp403;if(_tmp404->tag != 24)goto _LL295;else{_tmp405=_tmp404->f1;}}_LL294:
# 1276
 for(0;_tmp405 != 0;_tmp405=_tmp405->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp405->hd))
return 1;}
return 0;_LL295: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp406=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp403;if(_tmp406->tag != 25)goto _LL297;else{_tmp407=(void*)_tmp406->f1;}}_LL296: {
# 1281
void*_tmp409=Cyc_Tcutil_rgns_of(_tmp407);void*_tmp40A=_tmp409;void*_tmp40C;void*_tmp40D;_LL29C: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp40B=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp40A;if(_tmp40B->tag != 25)goto _LL29E;else{_tmp40C=(void*)_tmp40B->f1;}}_LL29D:
 return 0;_LL29E: _tmp40D=_tmp40A;_LL29F:
 return Cyc_Tcutil_evar_in_effect(evar,_tmp40D);_LL29B:;}_LL297: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp408=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp403;if(_tmp408->tag != 1)goto _LL299;}_LL298:
# 1285
 return evar == e;_LL299:;_LL29A:
 return 0;_LL292:;};}
# 1299 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1304
void*_tmp40E=Cyc_Tcutil_compress(e1);void*_tmp40F=_tmp40E;struct Cyc_List_List*_tmp411;void*_tmp413;struct Cyc_Absyn_Tvar*_tmp415;void*_tmp417;void**_tmp419;struct Cyc_Core_Opt*_tmp41A;_LL2A1: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp410=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp40F;if(_tmp410->tag != 24)goto _LL2A3;else{_tmp411=_tmp410->f1;}}_LL2A2:
# 1306
 for(0;_tmp411 != 0;_tmp411=_tmp411->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp411->hd,e2))
return 0;}
return 1;_LL2A3: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp412=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp40F;if(_tmp412->tag != 23)goto _LL2A5;else{_tmp413=(void*)_tmp412->f1;}}_LL2A4:
# 1317
 return Cyc_Tcutil_region_in_effect(may_constrain_evars,_tmp413,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp413,(void*)& Cyc_Absyn_HeapRgn_val);_LL2A5: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp414=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp40F;if(_tmp414->tag != 2)goto _LL2A7;else{_tmp415=_tmp414->f1;}}_LL2A6:
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp415,e2);_LL2A7: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp416=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp40F;if(_tmp416->tag != 25)goto _LL2A9;else{_tmp417=(void*)_tmp416->f1;}}_LL2A8: {
# 1321
void*_tmp41B=Cyc_Tcutil_rgns_of(_tmp417);void*_tmp41C=_tmp41B;void*_tmp41E;void*_tmp41F;_LL2AE: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp41D=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp41C;if(_tmp41D->tag != 25)goto _LL2B0;else{_tmp41E=(void*)_tmp41D->f1;}}_LL2AF:
# 1326
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp41E,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp41E,Cyc_Absyn_sint_typ);_LL2B0: _tmp41F=_tmp41C;_LL2B1:
 return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp41F,e2);_LL2AD:;}_LL2A9: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp418=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp40F;if(_tmp418->tag != 1)goto _LL2AB;else{_tmp419=(void**)((void**)& _tmp418->f2);_tmp41A=_tmp418->f4;}}_LL2AA:
# 1331
 if(!Cyc_Tcutil_evar_in_effect(e1,e2)){
# 1335
*_tmp419=Cyc_Absyn_empty_effect;
# 1338
return 1;}else{
# 1340
return 0;}_LL2AB:;_LL2AC: {
const char*_tmp1086;void*_tmp1085[1];struct Cyc_String_pa_PrintArg_struct _tmp1084;(_tmp1084.tag=0,((_tmp1084.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1)),((_tmp1085[0]=& _tmp1084,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1086="subset_effect: bad effect: %s",_tag_dyneither(_tmp1086,sizeof(char),30))),_tag_dyneither(_tmp1085,sizeof(void*),1)))))));}_LL2A0:;}
# 1356 "tcutil.cyc"
static int Cyc_Tcutil_unify_effect(void*e1,void*e2){
e1=Cyc_Tcutil_normalize_effect(e1);
e2=Cyc_Tcutil_normalize_effect(e2);
if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;
if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;
return 0;}
# 1372
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1374
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple0*_tmp423=(struct _tuple0*)r1->hd;void*_tmp425;void*_tmp426;struct _tuple0*_tmp424=_tmp423;_tmp425=_tmp424->f1;_tmp426=_tmp424->f2;{
int found=_tmp425 == (void*)& Cyc_Absyn_HeapRgn_val;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
struct _tuple0*_tmp427=(struct _tuple0*)r2->hd;void*_tmp429;void*_tmp42A;struct _tuple0*_tmp428=_tmp427;_tmp429=_tmp428->f1;_tmp42A=_tmp428->f2;
if(Cyc_Tcutil_unify(_tmp425,_tmp429) && Cyc_Tcutil_unify(_tmp426,_tmp42A)){
found=1;
break;}}}
# 1384
if(!found)return 0;};}}
# 1386
return 1;}
# 1389
typedef struct Cyc_List_List*Cyc_Tcutil_relns_t;
# 1393
static int Cyc_Tcutil_check_logical_implication(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
for(0;r2 != 0;r2=r2->tl){
struct Cyc_Relations_Reln*_tmp42B=Cyc_Relations_negate(Cyc_Core_heap_region,(struct Cyc_Relations_Reln*)r2->hd);
struct Cyc_List_List*_tmp1087;struct Cyc_List_List*_tmp42C=(_tmp1087=_cycalloc(sizeof(*_tmp1087)),((_tmp1087->hd=_tmp42B,((_tmp1087->tl=r1,_tmp1087)))));
if(Cyc_Relations_consistent_relations(_tmp42C))return 0;}
# 1399
return 1;}
# 1404
static int Cyc_Tcutil_check_logical_equivalence(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
if(r1 == r2)return 1;
return Cyc_Tcutil_check_logical_implication(r1,r2) && Cyc_Tcutil_check_logical_implication(r2,r1);}
# 1410
static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1412
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple22{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*f2;};
# 1416
void Cyc_Tcutil_unify_it(void*t1,void*t2){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=_tag_dyneither(0,0,0);
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return;
{void*_tmp42E=t1;struct Cyc_Core_Opt*_tmp430;void**_tmp431;struct Cyc_Core_Opt*_tmp432;_LL2B3: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp42F=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp42E;if(_tmp42F->tag != 1)goto _LL2B5;else{_tmp430=_tmp42F->f1;_tmp431=(void**)((void**)& _tmp42F->f2);_tmp432=_tmp42F->f4;}}_LL2B4:
# 1427
 Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp432))->v,t2);{
struct Cyc_Absyn_Kind*_tmp433=Cyc_Tcutil_typ_kind(t2);
# 1432
if(Cyc_Tcutil_kind_leq(_tmp433,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp430))->v)){
*_tmp431=t2;
return;}else{
# 1436
{void*_tmp434=t2;void**_tmp436;struct Cyc_Core_Opt*_tmp437;struct Cyc_Absyn_PtrInfo _tmp439;_LL2B8: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp435=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp434;if(_tmp435->tag != 1)goto _LL2BA;else{_tmp436=(void**)((void**)& _tmp435->f2);_tmp437=_tmp435->f4;}}_LL2B9: {
# 1439
struct Cyc_List_List*_tmp43A=(struct Cyc_List_List*)_tmp432->v;
{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp437))->v;for(0;s2 != 0;s2=s2->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp43A,(struct Cyc_Absyn_Tvar*)s2->hd)){
{const char*_tmp1088;Cyc_Tcutil_failure_reason=((_tmp1088="(type variable would escape scope)",_tag_dyneither(_tmp1088,sizeof(char),35)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1446
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp430->v,_tmp433)){
*_tmp436=t1;return;}
# 1449
{const char*_tmp1089;Cyc_Tcutil_failure_reason=((_tmp1089="(kinds are incompatible)",_tag_dyneither(_tmp1089,sizeof(char),25)));}
goto _LL2B7;}_LL2BA:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp438=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp434;if(_tmp438->tag != 5)goto _LL2BC;else{_tmp439=_tmp438->f1;}}if(!(((struct Cyc_Absyn_Kind*)_tmp430->v)->kind == Cyc_Absyn_BoxKind))goto _LL2BC;_LL2BB: {
# 1455
union Cyc_Absyn_Constraint*_tmp43D=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((_tmp439.ptr_atts).bounds);
{union Cyc_Absyn_Constraint*_tmp43E=_tmp43D;_LL2BF: if((_tmp43E->No_constr).tag != 3)goto _LL2C1;_LL2C0:
# 1459
{struct _union_Constraint_Eq_constr*_tmp108A;(_tmp108A=& _tmp43D->Eq_constr,((_tmp108A->tag=1,_tmp108A->val=Cyc_Absyn_bounds_one)));}
*_tmp431=t2;
return;_LL2C1:;_LL2C2:
 goto _LL2BE;_LL2BE:;}
# 1464
goto _LL2B7;}_LL2BC:;_LL2BD:
 goto _LL2B7;_LL2B7:;}
# 1467
{const char*_tmp108B;Cyc_Tcutil_failure_reason=((_tmp108B="(kinds are incompatible)",_tag_dyneither(_tmp108B,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};_LL2B5:;_LL2B6:
# 1470
 goto _LL2B2;_LL2B2:;}
# 1475
{struct _tuple0 _tmp108C;struct _tuple0 _tmp441=(_tmp108C.f1=t2,((_tmp108C.f2=t1,_tmp108C)));struct _tuple0 _tmp442=_tmp441;struct Cyc_Absyn_Tvar*_tmp447;struct Cyc_Absyn_Tvar*_tmp449;union Cyc_Absyn_AggrInfoU _tmp44B;struct Cyc_List_List*_tmp44C;union Cyc_Absyn_AggrInfoU _tmp44E;struct Cyc_List_List*_tmp44F;struct _tuple2*_tmp451;struct _tuple2*_tmp453;struct Cyc_List_List*_tmp455;struct Cyc_List_List*_tmp457;struct Cyc_Absyn_Datatypedecl*_tmp459;struct Cyc_List_List*_tmp45A;struct Cyc_Absyn_Datatypedecl*_tmp45C;struct Cyc_List_List*_tmp45D;struct Cyc_Absyn_Datatypedecl*_tmp45F;struct Cyc_Absyn_Datatypefield*_tmp460;struct Cyc_List_List*_tmp461;struct Cyc_Absyn_Datatypedecl*_tmp463;struct Cyc_Absyn_Datatypefield*_tmp464;struct Cyc_List_List*_tmp465;void*_tmp467;struct Cyc_Absyn_Tqual _tmp468;void*_tmp469;union Cyc_Absyn_Constraint*_tmp46A;union Cyc_Absyn_Constraint*_tmp46B;union Cyc_Absyn_Constraint*_tmp46C;void*_tmp46E;struct Cyc_Absyn_Tqual _tmp46F;void*_tmp470;union Cyc_Absyn_Constraint*_tmp471;union Cyc_Absyn_Constraint*_tmp472;union Cyc_Absyn_Constraint*_tmp473;enum Cyc_Absyn_Sign _tmp475;enum Cyc_Absyn_Size_of _tmp476;enum Cyc_Absyn_Sign _tmp478;enum Cyc_Absyn_Size_of _tmp479;int _tmp47B;int _tmp47D;void*_tmp47F;void*_tmp481;struct Cyc_Absyn_Exp*_tmp483;struct Cyc_Absyn_Exp*_tmp485;void*_tmp487;struct Cyc_Absyn_Tqual _tmp488;struct Cyc_Absyn_Exp*_tmp489;union Cyc_Absyn_Constraint*_tmp48A;void*_tmp48C;struct Cyc_Absyn_Tqual _tmp48D;struct Cyc_Absyn_Exp*_tmp48E;union Cyc_Absyn_Constraint*_tmp48F;struct Cyc_List_List*_tmp491;void*_tmp492;struct Cyc_Absyn_Tqual _tmp493;void*_tmp494;struct Cyc_List_List*_tmp495;int _tmp496;struct Cyc_Absyn_VarargInfo*_tmp497;struct Cyc_List_List*_tmp498;struct Cyc_List_List*_tmp499;struct Cyc_Absyn_Exp*_tmp49A;struct Cyc_List_List*_tmp49B;struct Cyc_Absyn_Exp*_tmp49C;struct Cyc_List_List*_tmp49D;struct Cyc_List_List*_tmp49F;void*_tmp4A0;struct Cyc_Absyn_Tqual _tmp4A1;void*_tmp4A2;struct Cyc_List_List*_tmp4A3;int _tmp4A4;struct Cyc_Absyn_VarargInfo*_tmp4A5;struct Cyc_List_List*_tmp4A6;struct Cyc_List_List*_tmp4A7;struct Cyc_Absyn_Exp*_tmp4A8;struct Cyc_List_List*_tmp4A9;struct Cyc_Absyn_Exp*_tmp4AA;struct Cyc_List_List*_tmp4AB;struct Cyc_List_List*_tmp4AD;struct Cyc_List_List*_tmp4AF;enum Cyc_Absyn_AggrKind _tmp4B1;struct Cyc_List_List*_tmp4B2;enum Cyc_Absyn_AggrKind _tmp4B4;struct Cyc_List_List*_tmp4B5;struct Cyc_List_List*_tmp4B7;struct Cyc_Absyn_Typedefdecl*_tmp4B8;struct Cyc_List_List*_tmp4BA;struct Cyc_Absyn_Typedefdecl*_tmp4BB;void*_tmp4C3;void*_tmp4C5;void*_tmp4C7;void*_tmp4C8;void*_tmp4CA;void*_tmp4CB;_LL2C4: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp443=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp442.f1;if(_tmp443->tag != 1)goto _LL2C6;}_LL2C5:
# 1478
 Cyc_Tcutil_unify_it(t2,t1);
return;_LL2C6:{struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp444=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp442.f1;if(_tmp444->tag != 0)goto _LL2C8;}{struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp445=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp442.f2;if(_tmp445->tag != 0)goto _LL2C8;};_LL2C7:
# 1481
 return;_LL2C8:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp446=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp442.f1;if(_tmp446->tag != 2)goto _LL2CA;else{_tmp447=_tmp446->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp448=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp442.f2;if(_tmp448->tag != 2)goto _LL2CA;else{_tmp449=_tmp448->f1;}};_LL2C9: {
# 1484
struct _dyneither_ptr*_tmp4D2=_tmp447->name;
struct _dyneither_ptr*_tmp4D3=_tmp449->name;
# 1487
int _tmp4D4=_tmp447->identity;
int _tmp4D5=_tmp449->identity;
if(_tmp4D5 == _tmp4D4)return;
{const char*_tmp108D;Cyc_Tcutil_failure_reason=((_tmp108D="(variable types are not the same)",_tag_dyneither(_tmp108D,sizeof(char),34)));}
goto _LL2C3;}_LL2CA:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp44A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp442.f1;if(_tmp44A->tag != 11)goto _LL2CC;else{_tmp44B=(_tmp44A->f1).aggr_info;_tmp44C=(_tmp44A->f1).targs;}}{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp44D=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp442.f2;if(_tmp44D->tag != 11)goto _LL2CC;else{_tmp44E=(_tmp44D->f1).aggr_info;_tmp44F=(_tmp44D->f1).targs;}};_LL2CB: {
# 1494
struct _tuple13 _tmp4D7=Cyc_Absyn_aggr_kinded_name(_tmp44E);enum Cyc_Absyn_AggrKind _tmp4D9;struct _tuple2*_tmp4DA;struct _tuple13 _tmp4D8=_tmp4D7;_tmp4D9=_tmp4D8.f1;_tmp4DA=_tmp4D8.f2;{
struct _tuple13 _tmp4DB=Cyc_Absyn_aggr_kinded_name(_tmp44B);enum Cyc_Absyn_AggrKind _tmp4DD;struct _tuple2*_tmp4DE;struct _tuple13 _tmp4DC=_tmp4DB;_tmp4DD=_tmp4DC.f1;_tmp4DE=_tmp4DC.f2;
if(_tmp4D9 != _tmp4DD){{const char*_tmp108E;Cyc_Tcutil_failure_reason=((_tmp108E="(struct and union type)",_tag_dyneither(_tmp108E,sizeof(char),24)));}goto _LL2C3;}
if(Cyc_Absyn_qvar_cmp(_tmp4DA,_tmp4DE)!= 0){{const char*_tmp108F;Cyc_Tcutil_failure_reason=((_tmp108F="(different type name)",_tag_dyneither(_tmp108F,sizeof(char),22)));}goto _LL2C3;}
Cyc_Tcutil_unify_list(_tmp44F,_tmp44C);
return;};}_LL2CC:{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp450=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp442.f1;if(_tmp450->tag != 13)goto _LL2CE;else{_tmp451=_tmp450->f1;}}{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp452=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp442.f2;if(_tmp452->tag != 13)goto _LL2CE;else{_tmp453=_tmp452->f1;}};_LL2CD:
# 1503
 if(Cyc_Absyn_qvar_cmp(_tmp451,_tmp453)== 0)return;
{const char*_tmp1090;Cyc_Tcutil_failure_reason=((_tmp1090="(different enum types)",_tag_dyneither(_tmp1090,sizeof(char),23)));}
goto _LL2C3;_LL2CE:{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp454=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp442.f1;if(_tmp454->tag != 14)goto _LL2D0;else{_tmp455=_tmp454->f1;}}{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp456=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp442.f2;if(_tmp456->tag != 14)goto _LL2D0;else{_tmp457=_tmp456->f1;}};_LL2CF: {
# 1508
int bad=0;
for(0;_tmp455 != 0  && _tmp457 != 0;(_tmp455=_tmp455->tl,_tmp457=_tmp457->tl)){
struct Cyc_Absyn_Enumfield*_tmp4E2=(struct Cyc_Absyn_Enumfield*)_tmp455->hd;
struct Cyc_Absyn_Enumfield*_tmp4E3=(struct Cyc_Absyn_Enumfield*)_tmp457->hd;
if(Cyc_Absyn_qvar_cmp(_tmp4E2->name,_tmp4E3->name)!= 0){
{const char*_tmp1091;Cyc_Tcutil_failure_reason=((_tmp1091="(different names for enum fields)",_tag_dyneither(_tmp1091,sizeof(char),34)));}
bad=1;
break;}
# 1517
if(_tmp4E2->tag == _tmp4E3->tag)continue;
if(_tmp4E2->tag == 0  || _tmp4E3->tag == 0){
{const char*_tmp1092;Cyc_Tcutil_failure_reason=((_tmp1092="(different tag values for enum fields)",_tag_dyneither(_tmp1092,sizeof(char),39)));}
bad=1;
break;}
# 1523
if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp4E2->tag),(struct Cyc_Absyn_Exp*)_check_null(_tmp4E3->tag))){
{const char*_tmp1093;Cyc_Tcutil_failure_reason=((_tmp1093="(different tag values for enum fields)",_tag_dyneither(_tmp1093,sizeof(char),39)));}
bad=1;
break;}}
# 1529
if(bad)goto _LL2C3;
if(_tmp455 == 0  && _tmp457 == 0)return;
{const char*_tmp1094;Cyc_Tcutil_failure_reason=((_tmp1094="(different number of fields for enums)",_tag_dyneither(_tmp1094,sizeof(char),39)));}
goto _LL2C3;}_LL2D0:{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp458=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp442.f1;if(_tmp458->tag != 3)goto _LL2D2;else{if((((_tmp458->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2D2;_tmp459=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp458->f1).datatype_info).KnownDatatype).val);_tmp45A=(_tmp458->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp45B=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp442.f2;if(_tmp45B->tag != 3)goto _LL2D2;else{if((((_tmp45B->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2D2;_tmp45C=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp45B->f1).datatype_info).KnownDatatype).val);_tmp45D=(_tmp45B->f1).targs;}};_LL2D1:
# 1536
 if(_tmp459 == _tmp45C  || Cyc_Absyn_qvar_cmp(_tmp459->name,_tmp45C->name)== 0){
Cyc_Tcutil_unify_list(_tmp45D,_tmp45A);
return;}
# 1540
goto _LL2C3;_LL2D2:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp45E=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp442.f1;if(_tmp45E->tag != 4)goto _LL2D4;else{if((((_tmp45E->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2D4;_tmp45F=((struct _tuple3)(((_tmp45E->f1).field_info).KnownDatatypefield).val).f1;_tmp460=((struct _tuple3)(((_tmp45E->f1).field_info).KnownDatatypefield).val).f2;_tmp461=(_tmp45E->f1).targs;}}{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp462=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp442.f2;if(_tmp462->tag != 4)goto _LL2D4;else{if((((_tmp462->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2D4;_tmp463=((struct _tuple3)(((_tmp462->f1).field_info).KnownDatatypefield).val).f1;_tmp464=((struct _tuple3)(((_tmp462->f1).field_info).KnownDatatypefield).val).f2;_tmp465=(_tmp462->f1).targs;}};_LL2D3:
# 1544
 if((_tmp45F == _tmp463  || Cyc_Absyn_qvar_cmp(_tmp45F->name,_tmp463->name)== 0) && (
_tmp460 == _tmp464  || Cyc_Absyn_qvar_cmp(_tmp460->name,_tmp464->name)== 0)){
Cyc_Tcutil_unify_list(_tmp465,_tmp461);
return;}
# 1549
{const char*_tmp1095;Cyc_Tcutil_failure_reason=((_tmp1095="(different datatype field types)",_tag_dyneither(_tmp1095,sizeof(char),33)));}
goto _LL2C3;_LL2D4:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp466=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp442.f1;if(_tmp466->tag != 5)goto _LL2D6;else{_tmp467=(_tmp466->f1).elt_typ;_tmp468=(_tmp466->f1).elt_tq;_tmp469=((_tmp466->f1).ptr_atts).rgn;_tmp46A=((_tmp466->f1).ptr_atts).nullable;_tmp46B=((_tmp466->f1).ptr_atts).bounds;_tmp46C=((_tmp466->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp46D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp442.f2;if(_tmp46D->tag != 5)goto _LL2D6;else{_tmp46E=(_tmp46D->f1).elt_typ;_tmp46F=(_tmp46D->f1).elt_tq;_tmp470=((_tmp46D->f1).ptr_atts).rgn;_tmp471=((_tmp46D->f1).ptr_atts).nullable;_tmp472=((_tmp46D->f1).ptr_atts).bounds;_tmp473=((_tmp46D->f1).ptr_atts).zero_term;}};_LL2D5:
# 1554
 Cyc_Tcutil_unify_it(_tmp46E,_tmp467);
Cyc_Tcutil_unify_it(_tmp469,_tmp470);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp1096;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp473,_tmp46C,((_tmp1096="(not both zero terminated)",_tag_dyneither(_tmp1096,sizeof(char),27))));}
Cyc_Tcutil_unify_tqual(_tmp46F,_tmp46E,_tmp468,_tmp467);
{const char*_tmp1097;((void(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp472,_tmp46B,((_tmp1097="(different pointer bounds)",_tag_dyneither(_tmp1097,sizeof(char),27))));}
# 1562
{void*_tmp4EB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp472);void*_tmp4EC=_tmp4EB;_LL301: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp4ED=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp4EC;if(_tmp4ED->tag != 0)goto _LL303;}_LL302:
 return;_LL303:;_LL304:
 goto _LL300;_LL300:;}
# 1566
{const char*_tmp1098;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp471,_tmp46A,((_tmp1098="(incompatible pointer types)",_tag_dyneither(_tmp1098,sizeof(char),29))));}
return;_LL2D6:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp474=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp442.f1;if(_tmp474->tag != 6)goto _LL2D8;else{_tmp475=_tmp474->f1;_tmp476=_tmp474->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp477=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp442.f2;if(_tmp477->tag != 6)goto _LL2D8;else{_tmp478=_tmp477->f1;_tmp479=_tmp477->f2;}};_LL2D7:
# 1571
 if(_tmp478 == _tmp475  && ((_tmp479 == _tmp476  || _tmp479 == Cyc_Absyn_Int_sz  && _tmp476 == Cyc_Absyn_Long_sz) || 
_tmp479 == Cyc_Absyn_Long_sz  && _tmp476 == Cyc_Absyn_Int_sz))return;
{const char*_tmp1099;Cyc_Tcutil_failure_reason=((_tmp1099="(different integral types)",_tag_dyneither(_tmp1099,sizeof(char),27)));}
goto _LL2C3;_LL2D8:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp47A=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp442.f1;if(_tmp47A->tag != 7)goto _LL2DA;else{_tmp47B=_tmp47A->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp47C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp442.f2;if(_tmp47C->tag != 7)goto _LL2DA;else{_tmp47D=_tmp47C->f1;}};_LL2D9:
# 1577
 if(_tmp47D == 0  && _tmp47B == 0)return;else{
if(_tmp47D == 1  && _tmp47B == 1)return;else{
# 1580
if(((_tmp47D != 0  && _tmp47D != 1) && _tmp47B != 0) && _tmp47B != 1)return;}}
goto _LL2C3;_LL2DA:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp47E=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp442.f1;if(_tmp47E->tag != 19)goto _LL2DC;else{_tmp47F=(void*)_tmp47E->f1;}}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp480=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp442.f2;if(_tmp480->tag != 19)goto _LL2DC;else{_tmp481=(void*)_tmp480->f1;}};_LL2DB:
# 1583
 Cyc_Tcutil_unify_it(_tmp47F,_tmp481);return;_LL2DC:{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp482=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp442.f1;if(_tmp482->tag != 18)goto _LL2DE;else{_tmp483=_tmp482->f1;}}{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp484=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp442.f2;if(_tmp484->tag != 18)goto _LL2DE;else{_tmp485=_tmp484->f1;}};_LL2DD:
# 1586
 if(!Cyc_Evexp_same_const_exp(_tmp483,_tmp485)){
{const char*_tmp109A;Cyc_Tcutil_failure_reason=((_tmp109A="(cannot prove expressions are the same)",_tag_dyneither(_tmp109A,sizeof(char),40)));}
goto _LL2C3;}
# 1590
return;_LL2DE:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp486=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp442.f1;if(_tmp486->tag != 8)goto _LL2E0;else{_tmp487=(_tmp486->f1).elt_type;_tmp488=(_tmp486->f1).tq;_tmp489=(_tmp486->f1).num_elts;_tmp48A=(_tmp486->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp48B=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp442.f2;if(_tmp48B->tag != 8)goto _LL2E0;else{_tmp48C=(_tmp48B->f1).elt_type;_tmp48D=(_tmp48B->f1).tq;_tmp48E=(_tmp48B->f1).num_elts;_tmp48F=(_tmp48B->f1).zero_term;}};_LL2DF:
# 1594
 Cyc_Tcutil_unify_it(_tmp48C,_tmp487);
Cyc_Tcutil_unify_tqual(_tmp48D,_tmp48C,_tmp488,_tmp487);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp109B;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp48A,_tmp48F,((_tmp109B="(not both zero terminated)",_tag_dyneither(_tmp109B,sizeof(char),27))));}
if(_tmp489 == _tmp48E)return;
if(_tmp489 == 0  || _tmp48E == 0)goto _LL2C3;
if(Cyc_Evexp_same_const_exp(_tmp489,_tmp48E))
return;
{const char*_tmp109C;Cyc_Tcutil_failure_reason=((_tmp109C="(different array sizes)",_tag_dyneither(_tmp109C,sizeof(char),24)));}
goto _LL2C3;_LL2E0:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp490=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp442.f1;if(_tmp490->tag != 9)goto _LL2E2;else{_tmp491=(_tmp490->f1).tvars;_tmp492=(_tmp490->f1).effect;_tmp493=(_tmp490->f1).ret_tqual;_tmp494=(_tmp490->f1).ret_typ;_tmp495=(_tmp490->f1).args;_tmp496=(_tmp490->f1).c_varargs;_tmp497=(_tmp490->f1).cyc_varargs;_tmp498=(_tmp490->f1).rgn_po;_tmp499=(_tmp490->f1).attributes;_tmp49A=(_tmp490->f1).requires_clause;_tmp49B=(_tmp490->f1).requires_relns;_tmp49C=(_tmp490->f1).ensures_clause;_tmp49D=(_tmp490->f1).ensures_relns;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp49E=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp442.f2;if(_tmp49E->tag != 9)goto _LL2E2;else{_tmp49F=(_tmp49E->f1).tvars;_tmp4A0=(_tmp49E->f1).effect;_tmp4A1=(_tmp49E->f1).ret_tqual;_tmp4A2=(_tmp49E->f1).ret_typ;_tmp4A3=(_tmp49E->f1).args;_tmp4A4=(_tmp49E->f1).c_varargs;_tmp4A5=(_tmp49E->f1).cyc_varargs;_tmp4A6=(_tmp49E->f1).rgn_po;_tmp4A7=(_tmp49E->f1).attributes;_tmp4A8=(_tmp49E->f1).requires_clause;_tmp4A9=(_tmp49E->f1).requires_relns;_tmp4AA=(_tmp49E->f1).ensures_clause;_tmp4AB=(_tmp49E->f1).ensures_relns;}};_LL2E1: {
# 1608
int done=0;
struct _RegionHandle _tmp4F3=_new_region("rgn");struct _RegionHandle*rgn=& _tmp4F3;_push_region(rgn);{
struct Cyc_List_List*inst=0;
while(_tmp49F != 0){
if(_tmp491 == 0){
{const char*_tmp109D;Cyc_Tcutil_failure_reason=((_tmp109D="(second function type has too few type variables)",_tag_dyneither(_tmp109D,sizeof(char),50)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1616
void*_tmp4F5=((struct Cyc_Absyn_Tvar*)_tmp49F->hd)->kind;
void*_tmp4F6=((struct Cyc_Absyn_Tvar*)_tmp491->hd)->kind;
if(!Cyc_Tcutil_unify_kindbound(_tmp4F5,_tmp4F6)){
{const char*_tmp10A3;void*_tmp10A2[3];struct Cyc_String_pa_PrintArg_struct _tmp10A1;struct Cyc_String_pa_PrintArg_struct _tmp10A0;struct Cyc_String_pa_PrintArg_struct _tmp109F;Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)((_tmp109F.tag=0,((_tmp109F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1622
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp491->hd,& Cyc_Tcutil_bk))),((_tmp10A0.tag=0,((_tmp10A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1621
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp49F->hd,& Cyc_Tcutil_bk))),((_tmp10A1.tag=0,((_tmp10A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1620
Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)_tmp49F->hd)),((_tmp10A2[0]=& _tmp10A1,((_tmp10A2[1]=& _tmp10A0,((_tmp10A2[2]=& _tmp109F,Cyc_aprintf(((_tmp10A3="(type var %s has different kinds %s and %s)",_tag_dyneither(_tmp10A3,sizeof(char),44))),_tag_dyneither(_tmp10A2,sizeof(void*),3))))))))))))))))))));}
# 1623
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1625
{struct _tuple16*_tmp10AD;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp10AC;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp10AB;struct Cyc_List_List*_tmp10AA;inst=((_tmp10AA=_region_malloc(rgn,sizeof(*_tmp10AA)),((_tmp10AA->hd=((_tmp10AD=_region_malloc(rgn,sizeof(*_tmp10AD)),((_tmp10AD->f1=(struct Cyc_Absyn_Tvar*)_tmp491->hd,((_tmp10AD->f2=(void*)((_tmp10AB=_cycalloc(sizeof(*_tmp10AB)),((_tmp10AB[0]=((_tmp10AC.tag=2,((_tmp10AC.f1=(struct Cyc_Absyn_Tvar*)_tmp49F->hd,_tmp10AC)))),_tmp10AB)))),_tmp10AD)))))),((_tmp10AA->tl=inst,_tmp10AA))))));}
_tmp49F=_tmp49F->tl;
_tmp491=_tmp491->tl;};}
# 1629
if(_tmp491 != 0){
{const char*_tmp10AE;Cyc_Tcutil_failure_reason=((_tmp10AE="(second function type has too many type variables)",_tag_dyneither(_tmp10AE,sizeof(char),51)));}
_npop_handler(0);goto _LL2C3;}
# 1633
if(inst != 0){
{struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp10BA;struct Cyc_Absyn_FnInfo _tmp10B9;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp10B8;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp10B4;struct Cyc_Absyn_FnInfo _tmp10B3;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp10B2;Cyc_Tcutil_unify_it((void*)((_tmp10B2=_cycalloc(sizeof(*_tmp10B2)),((_tmp10B2[0]=((_tmp10B4.tag=9,((_tmp10B4.f1=((_tmp10B3.tvars=0,((_tmp10B3.effect=_tmp4A0,((_tmp10B3.ret_tqual=_tmp4A1,((_tmp10B3.ret_typ=_tmp4A2,((_tmp10B3.args=_tmp4A3,((_tmp10B3.c_varargs=_tmp4A4,((_tmp10B3.cyc_varargs=_tmp4A5,((_tmp10B3.rgn_po=_tmp4A6,((_tmp10B3.attributes=_tmp4A7,((_tmp10B3.requires_clause=_tmp49A,((_tmp10B3.requires_relns=_tmp49B,((_tmp10B3.ensures_clause=_tmp49C,((_tmp10B3.ensures_relns=_tmp49D,_tmp10B3)))))))))))))))))))))))))),_tmp10B4)))),_tmp10B2)))),
# 1637
Cyc_Tcutil_rsubstitute(rgn,inst,(void*)(
(_tmp10B8=_cycalloc(sizeof(*_tmp10B8)),((_tmp10B8[0]=((_tmp10BA.tag=9,((_tmp10BA.f1=((_tmp10B9.tvars=0,((_tmp10B9.effect=_tmp492,((_tmp10B9.ret_tqual=_tmp493,((_tmp10B9.ret_typ=_tmp494,((_tmp10B9.args=_tmp495,((_tmp10B9.c_varargs=_tmp496,((_tmp10B9.cyc_varargs=_tmp497,((_tmp10B9.rgn_po=_tmp498,((_tmp10B9.attributes=_tmp499,((_tmp10B9.requires_clause=_tmp4A8,((_tmp10B9.requires_relns=_tmp4A9,((_tmp10B9.ensures_clause=_tmp4AA,((_tmp10B9.ensures_relns=_tmp4AB,_tmp10B9)))))))))))))))))))))))))),_tmp10BA)))),_tmp10B8))))));}
# 1642
done=1;}}
# 1645
if(done){
_npop_handler(0);return;}
Cyc_Tcutil_unify_it(_tmp4A2,_tmp494);
Cyc_Tcutil_unify_tqual(_tmp4A1,_tmp4A2,_tmp493,_tmp494);
for(0;_tmp4A3 != 0  && _tmp495 != 0;(_tmp4A3=_tmp4A3->tl,_tmp495=_tmp495->tl)){
struct _tuple10 _tmp507=*((struct _tuple10*)_tmp4A3->hd);struct Cyc_Absyn_Tqual _tmp509;void*_tmp50A;struct _tuple10 _tmp508=_tmp507;_tmp509=_tmp508.f2;_tmp50A=_tmp508.f3;{
struct _tuple10 _tmp50B=*((struct _tuple10*)_tmp495->hd);struct Cyc_Absyn_Tqual _tmp50D;void*_tmp50E;struct _tuple10 _tmp50C=_tmp50B;_tmp50D=_tmp50C.f2;_tmp50E=_tmp50C.f3;
Cyc_Tcutil_unify_it(_tmp50A,_tmp50E);
Cyc_Tcutil_unify_tqual(_tmp509,_tmp50A,_tmp50D,_tmp50E);};}
# 1655
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(_tmp4A3 != 0  || _tmp495 != 0){
{const char*_tmp10BB;Cyc_Tcutil_failure_reason=((_tmp10BB="(function types have different number of arguments)",_tag_dyneither(_tmp10BB,sizeof(char),52)));}
_npop_handler(0);goto _LL2C3;}
# 1661
if(_tmp4A4 != _tmp496){
{const char*_tmp10BC;Cyc_Tcutil_failure_reason=((_tmp10BC="(only one function type takes C varargs)",_tag_dyneither(_tmp10BC,sizeof(char),41)));}
_npop_handler(0);goto _LL2C3;}{
# 1666
int bad_cyc_vararg=0;
{struct _tuple22 _tmp10BD;struct _tuple22 _tmp511=(_tmp10BD.f1=_tmp4A5,((_tmp10BD.f2=_tmp497,_tmp10BD)));struct _tuple22 _tmp512=_tmp511;struct _dyneither_ptr*_tmp513;struct Cyc_Absyn_Tqual _tmp514;void*_tmp515;int _tmp516;struct _dyneither_ptr*_tmp517;struct Cyc_Absyn_Tqual _tmp518;void*_tmp519;int _tmp51A;_LL306: if(_tmp512.f1 != 0)goto _LL308;if(_tmp512.f2 != 0)goto _LL308;_LL307:
 goto _LL305;_LL308: if(_tmp512.f1 != 0)goto _LL30A;_LL309:
 goto _LL30B;_LL30A: if(_tmp512.f2 != 0)goto _LL30C;_LL30B:
# 1671
 bad_cyc_vararg=1;
{const char*_tmp10BE;Cyc_Tcutil_failure_reason=((_tmp10BE="(only one function type takes varargs)",_tag_dyneither(_tmp10BE,sizeof(char),39)));}
goto _LL305;_LL30C: if(_tmp512.f1 == 0)goto _LL305;_tmp513=(_tmp512.f1)->name;_tmp514=(_tmp512.f1)->tq;_tmp515=(_tmp512.f1)->type;_tmp516=(_tmp512.f1)->inject;if(_tmp512.f2 == 0)goto _LL305;_tmp517=(_tmp512.f2)->name;_tmp518=(_tmp512.f2)->tq;_tmp519=(_tmp512.f2)->type;_tmp51A=(_tmp512.f2)->inject;_LL30D:
# 1675
 Cyc_Tcutil_unify_it(_tmp515,_tmp519);
Cyc_Tcutil_unify_tqual(_tmp514,_tmp515,_tmp518,_tmp519);
if(_tmp516 != _tmp51A){
bad_cyc_vararg=1;{
const char*_tmp10BF;Cyc_Tcutil_failure_reason=((_tmp10BF="(only one function type injects varargs)",_tag_dyneither(_tmp10BF,sizeof(char),41)));};}
# 1681
goto _LL305;_LL305:;}
# 1683
if(bad_cyc_vararg){_npop_handler(0);goto _LL2C3;}{
# 1686
int bad_effect=0;
{struct _tuple0 _tmp10C0;struct _tuple0 _tmp51E=(_tmp10C0.f1=_tmp4A0,((_tmp10C0.f2=_tmp492,_tmp10C0)));struct _tuple0 _tmp51F=_tmp51E;_LL30F: if(_tmp51F.f1 != 0)goto _LL311;if(_tmp51F.f2 != 0)goto _LL311;_LL310:
 goto _LL30E;_LL311: if(_tmp51F.f1 != 0)goto _LL313;_LL312:
 goto _LL314;_LL313: if(_tmp51F.f2 != 0)goto _LL315;_LL314:
 bad_effect=1;goto _LL30E;_LL315:;_LL316:
 bad_effect=!Cyc_Tcutil_unify_effect((void*)_check_null(_tmp4A0),(void*)_check_null(_tmp492));goto _LL30E;_LL30E:;}
# 1693
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(bad_effect){
{const char*_tmp10C1;Cyc_Tcutil_failure_reason=((_tmp10C1="(function effects do not match)",_tag_dyneither(_tmp10C1,sizeof(char),32)));}
_npop_handler(0);goto _LL2C3;}
# 1699
if(!Cyc_Tcutil_same_atts(_tmp499,_tmp4A7)){
{const char*_tmp10C2;Cyc_Tcutil_failure_reason=((_tmp10C2="(function types have different attributes)",_tag_dyneither(_tmp10C2,sizeof(char),43)));}
_npop_handler(0);goto _LL2C3;}
# 1703
if(!Cyc_Tcutil_same_rgn_po(_tmp498,_tmp4A6)){
{const char*_tmp10C3;Cyc_Tcutil_failure_reason=((_tmp10C3="(function types have different region lifetime orderings)",_tag_dyneither(_tmp10C3,sizeof(char),58)));}
_npop_handler(0);goto _LL2C3;}
# 1707
if(!Cyc_Tcutil_check_logical_equivalence(_tmp49B,_tmp4A9)){
{const char*_tmp10C4;Cyc_Tcutil_failure_reason=((_tmp10C4="(@requires clauses not equivalent)",_tag_dyneither(_tmp10C4,sizeof(char),35)));}
_npop_handler(0);goto _LL2C3;}
# 1711
if(!Cyc_Tcutil_check_logical_equivalence(_tmp49D,_tmp4AB)){
{const char*_tmp10C5;Cyc_Tcutil_failure_reason=((_tmp10C5="(@ensures clauses not equivalent)",_tag_dyneither(_tmp10C5,sizeof(char),34)));}
_npop_handler(0);goto _LL2C3;}
# 1715
_npop_handler(0);return;};};
# 1609
;_pop_region(rgn);}_LL2E2:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp4AC=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp442.f1;if(_tmp4AC->tag != 10)goto _LL2E4;else{_tmp4AD=_tmp4AC->f1;}}{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp4AE=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp442.f2;if(_tmp4AE->tag != 10)goto _LL2E4;else{_tmp4AF=_tmp4AE->f1;}};_LL2E3:
# 1718
 for(0;_tmp4AF != 0  && _tmp4AD != 0;(_tmp4AF=_tmp4AF->tl,_tmp4AD=_tmp4AD->tl)){
struct _tuple12 _tmp526=*((struct _tuple12*)_tmp4AF->hd);struct Cyc_Absyn_Tqual _tmp528;void*_tmp529;struct _tuple12 _tmp527=_tmp526;_tmp528=_tmp527.f1;_tmp529=_tmp527.f2;{
struct _tuple12 _tmp52A=*((struct _tuple12*)_tmp4AD->hd);struct Cyc_Absyn_Tqual _tmp52C;void*_tmp52D;struct _tuple12 _tmp52B=_tmp52A;_tmp52C=_tmp52B.f1;_tmp52D=_tmp52B.f2;
Cyc_Tcutil_unify_it(_tmp529,_tmp52D);
Cyc_Tcutil_unify_tqual(_tmp528,_tmp529,_tmp52C,_tmp52D);};}
# 1724
if(_tmp4AF == 0  && _tmp4AD == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp10C6;Cyc_Tcutil_failure_reason=((_tmp10C6="(tuple types have different numbers of components)",_tag_dyneither(_tmp10C6,sizeof(char),51)));}
goto _LL2C3;_LL2E4:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp4B0=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp442.f1;if(_tmp4B0->tag != 12)goto _LL2E6;else{_tmp4B1=_tmp4B0->f1;_tmp4B2=_tmp4B0->f2;}}{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp4B3=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp442.f2;if(_tmp4B3->tag != 12)goto _LL2E6;else{_tmp4B4=_tmp4B3->f1;_tmp4B5=_tmp4B3->f2;}};_LL2E5:
# 1731
 if(_tmp4B4 != _tmp4B1){{const char*_tmp10C7;Cyc_Tcutil_failure_reason=((_tmp10C7="(struct and union type)",_tag_dyneither(_tmp10C7,sizeof(char),24)));}goto _LL2C3;}
for(0;_tmp4B5 != 0  && _tmp4B2 != 0;(_tmp4B5=_tmp4B5->tl,_tmp4B2=_tmp4B2->tl)){
struct Cyc_Absyn_Aggrfield*_tmp530=(struct Cyc_Absyn_Aggrfield*)_tmp4B5->hd;
struct Cyc_Absyn_Aggrfield*_tmp531=(struct Cyc_Absyn_Aggrfield*)_tmp4B2->hd;
if(Cyc_strptrcmp(_tmp530->name,_tmp531->name)!= 0){
{const char*_tmp10C8;Cyc_Tcutil_failure_reason=((_tmp10C8="(different member names)",_tag_dyneither(_tmp10C8,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1739
Cyc_Tcutil_unify_it(_tmp530->type,_tmp531->type);
Cyc_Tcutil_unify_tqual(_tmp530->tq,_tmp530->type,_tmp531->tq,_tmp531->type);
if(!Cyc_Tcutil_same_atts(_tmp530->attributes,_tmp531->attributes)){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp10C9;Cyc_Tcutil_failure_reason=((_tmp10C9="(different attributes on member)",_tag_dyneither(_tmp10C9,sizeof(char),33)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1747
if((_tmp530->width != 0  && _tmp531->width == 0  || 
_tmp531->width != 0  && _tmp530->width == 0) || 
(_tmp530->width != 0  && _tmp531->width != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp530->width),(struct Cyc_Absyn_Exp*)_check_null(_tmp531->width))){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp10CA;Cyc_Tcutil_failure_reason=((_tmp10CA="(different bitfield widths on member)",_tag_dyneither(_tmp10CA,sizeof(char),38)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1756
if((_tmp530->requires_clause != 0  && _tmp531->requires_clause == 0  || 
_tmp530->requires_clause == 0  && _tmp531->requires_clause != 0) || 
(_tmp530->requires_clause == 0  && _tmp531->requires_clause != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp530->requires_clause),(struct Cyc_Absyn_Exp*)_check_null(_tmp531->requires_clause))){
# 1761
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp10CB;Cyc_Tcutil_failure_reason=((_tmp10CB="(different @requires clauses on member)",_tag_dyneither(_tmp10CB,sizeof(char),40)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1767
if(_tmp4B5 == 0  && _tmp4B2 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp10CC;Cyc_Tcutil_failure_reason=((_tmp10CC="(different number of members)",_tag_dyneither(_tmp10CC,sizeof(char),30)));}
goto _LL2C3;_LL2E6:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp4B6=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp442.f1;if(_tmp4B6->tag != 17)goto _LL2E8;else{_tmp4B7=_tmp4B6->f2;_tmp4B8=_tmp4B6->f3;}}{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp4B9=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp442.f2;if(_tmp4B9->tag != 17)goto _LL2E8;else{_tmp4BA=_tmp4B9->f2;_tmp4BB=_tmp4B9->f3;}};_LL2E7:
# 1773
 if(_tmp4B8 != _tmp4BB){
{const char*_tmp10CD;Cyc_Tcutil_failure_reason=((_tmp10CD="(different abstract typedefs)",_tag_dyneither(_tmp10CD,sizeof(char),30)));}
goto _LL2C3;}
# 1777
{const char*_tmp10CE;Cyc_Tcutil_failure_reason=((_tmp10CE="(type parameters to typedef differ)",_tag_dyneither(_tmp10CE,sizeof(char),36)));}
Cyc_Tcutil_unify_list(_tmp4B7,_tmp4BA);
return;_LL2E8:{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp4BC=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp442.f1;if(_tmp4BC->tag != 20)goto _LL2EA;}{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp4BD=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp442.f2;if(_tmp4BD->tag != 20)goto _LL2EA;};_LL2E9:
 return;_LL2EA:{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp4BE=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp442.f1;if(_tmp4BE->tag != 21)goto _LL2EC;}{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp4BF=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp442.f2;if(_tmp4BF->tag != 21)goto _LL2EC;};_LL2EB:
 return;_LL2EC:{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp4C0=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp442.f1;if(_tmp4C0->tag != 22)goto _LL2EE;}{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp4C1=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp442.f2;if(_tmp4C1->tag != 22)goto _LL2EE;};_LL2ED:
 return;_LL2EE:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4C2=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp442.f1;if(_tmp4C2->tag != 15)goto _LL2F0;else{_tmp4C3=(void*)_tmp4C2->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4C4=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp442.f2;if(_tmp4C4->tag != 15)goto _LL2F0;else{_tmp4C5=(void*)_tmp4C4->f1;}};_LL2EF:
# 1784
 Cyc_Tcutil_unify_it(_tmp4C3,_tmp4C5);
return;_LL2F0:{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp4C6=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp442.f1;if(_tmp4C6->tag != 16)goto _LL2F2;else{_tmp4C7=(void*)_tmp4C6->f1;_tmp4C8=(void*)_tmp4C6->f2;}}{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp4C9=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp442.f2;if(_tmp4C9->tag != 16)goto _LL2F2;else{_tmp4CA=(void*)_tmp4C9->f1;_tmp4CB=(void*)_tmp4C9->f2;}};_LL2F1:
# 1787
 Cyc_Tcutil_unify_it(_tmp4C7,_tmp4CA);
Cyc_Tcutil_unify_it(_tmp4C8,_tmp4CB);
return;_LL2F2: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp4CC=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp442.f1;if(_tmp4CC->tag != 24)goto _LL2F4;}_LL2F3:
 goto _LL2F5;_LL2F4: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp4CD=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp442.f2;if(_tmp4CD->tag != 24)goto _LL2F6;}_LL2F5:
 goto _LL2F7;_LL2F6: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4CE=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp442.f1;if(_tmp4CE->tag != 23)goto _LL2F8;}_LL2F7:
 goto _LL2F9;_LL2F8: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp4CF=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp442.f1;if(_tmp4CF->tag != 25)goto _LL2FA;}_LL2F9:
 goto _LL2FB;_LL2FA: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp4D0=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp442.f2;if(_tmp4D0->tag != 25)goto _LL2FC;}_LL2FB:
 goto _LL2FD;_LL2FC: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4D1=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp442.f2;if(_tmp4D1->tag != 23)goto _LL2FE;}_LL2FD:
# 1796
 if(Cyc_Tcutil_unify_effect(t1,t2))return;
{const char*_tmp10CF;Cyc_Tcutil_failure_reason=((_tmp10CF="(effects don't unify)",_tag_dyneither(_tmp10CF,sizeof(char),22)));}
goto _LL2C3;_LL2FE:;_LL2FF:
 goto _LL2C3;_LL2C3:;}
# 1801
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1804
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0  && a2 != 0)return - 1;
if(a1 != 0  && a2 == 0)return 1;
return cmp((void*)_check_null(a1),(void*)_check_null(a2));}
# 1811
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp53B=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
int _tmp53C=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp53B,_tmp53C);}
# 1818
static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return 0;{
union Cyc_Absyn_Constraint*_tmp53D=x;void*_tmp53E;_LL318: if((_tmp53D->No_constr).tag != 3)goto _LL31A;_LL319:
 return - 1;_LL31A: if((_tmp53D->Eq_constr).tag != 1)goto _LL31C;_tmp53E=(void*)(_tmp53D->Eq_constr).val;_LL31B: {
# 1825
union Cyc_Absyn_Constraint*_tmp53F=y;void*_tmp540;_LL31F: if((_tmp53F->No_constr).tag != 3)goto _LL321;_LL320:
 return 1;_LL321: if((_tmp53F->Eq_constr).tag != 1)goto _LL323;_tmp540=(void*)(_tmp53F->Eq_constr).val;_LL322:
 return cmp(_tmp53E,_tmp540);_LL323: if((_tmp53F->Forward_constr).tag != 2)goto _LL31E;_LL324: {
const char*_tmp10D2;void*_tmp10D1;(_tmp10D1=0,Cyc_Tcutil_impos(((_tmp10D2="unify_conref: forward after compress(2)",_tag_dyneither(_tmp10D2,sizeof(char),40))),_tag_dyneither(_tmp10D1,sizeof(void*),0)));}_LL31E:;}_LL31C: if((_tmp53D->Forward_constr).tag != 2)goto _LL317;_LL31D: {
# 1830
const char*_tmp10D5;void*_tmp10D4;(_tmp10D4=0,Cyc_Tcutil_impos(((_tmp10D5="unify_conref: forward after compress",_tag_dyneither(_tmp10D5,sizeof(char),37))),_tag_dyneither(_tmp10D4,sizeof(void*),0)));}_LL317:;};}
# 1834
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct Cyc_Absyn_Tqual _tmp546;void*_tmp547;struct _tuple12*_tmp545=tqt1;_tmp546=_tmp545->f1;_tmp547=_tmp545->f2;{
struct Cyc_Absyn_Tqual _tmp549;void*_tmp54A;struct _tuple12*_tmp548=tqt2;_tmp549=_tmp548->f1;_tmp54A=_tmp548->f2;{
int _tmp54B=Cyc_Tcutil_tqual_cmp(_tmp546,_tmp549);
if(_tmp54B != 0)return _tmp54B;
return Cyc_Tcutil_typecmp(_tmp547,_tmp54A);};};}
# 1842
static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp54C=Cyc_strptrcmp(f1->name,f2->name);
if(_tmp54C != 0)return _tmp54C;{
int _tmp54D=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);
if(_tmp54D != 0)return _tmp54D;{
int _tmp54E=Cyc_Tcutil_typecmp(f1->type,f2->type);
if(_tmp54E != 0)return _tmp54E;{
int _tmp54F=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);
if(_tmp54F != 0)return _tmp54F;
_tmp54F=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);
if(_tmp54F != 0)return _tmp54F;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->requires_clause,f2->requires_clause);};};};}
# 1856
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp550=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp550 != 0)return _tmp550;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}
# 1862
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp551=t;_LL326: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp552=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp551;if(_tmp552->tag != 0)goto _LL328;}_LL327:
 return 0;_LL328: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp553=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp551;if(_tmp553->tag != 1)goto _LL32A;}_LL329:
 return 1;_LL32A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp554=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp551;if(_tmp554->tag != 2)goto _LL32C;}_LL32B:
 return 2;_LL32C: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp555=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp551;if(_tmp555->tag != 3)goto _LL32E;}_LL32D:
 return 3;_LL32E: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp556=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp551;if(_tmp556->tag != 4)goto _LL330;}_LL32F:
 return 4;_LL330: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp557=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp551;if(_tmp557->tag != 5)goto _LL332;}_LL331:
 return 5;_LL332: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp558=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp551;if(_tmp558->tag != 6)goto _LL334;}_LL333:
 return 6;_LL334: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp559=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp551;if(_tmp559->tag != 7)goto _LL336;else{if(_tmp559->f1 != 0)goto _LL336;}}_LL335:
 return 7;_LL336: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp55A=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp551;if(_tmp55A->tag != 7)goto _LL338;}_LL337:
 return 8;_LL338: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp55B=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp551;if(_tmp55B->tag != 8)goto _LL33A;}_LL339:
 return 9;_LL33A: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp55C=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp551;if(_tmp55C->tag != 9)goto _LL33C;}_LL33B:
 return 10;_LL33C: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp55D=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp551;if(_tmp55D->tag != 10)goto _LL33E;}_LL33D:
 return 11;_LL33E: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp55E=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp551;if(_tmp55E->tag != 11)goto _LL340;}_LL33F:
 return 12;_LL340: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp55F=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp551;if(_tmp55F->tag != 12)goto _LL342;}_LL341:
 return 14;_LL342: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp560=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp551;if(_tmp560->tag != 13)goto _LL344;}_LL343:
 return 16;_LL344: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp561=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp551;if(_tmp561->tag != 14)goto _LL346;}_LL345:
 return 17;_LL346: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp562=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp551;if(_tmp562->tag != 15)goto _LL348;}_LL347:
 return 18;_LL348: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp563=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp551;if(_tmp563->tag != 17)goto _LL34A;}_LL349:
 return 19;_LL34A: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp564=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp551;if(_tmp564->tag != 21)goto _LL34C;}_LL34B:
 return 20;_LL34C: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp565=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp551;if(_tmp565->tag != 20)goto _LL34E;}_LL34D:
 return 21;_LL34E: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp566=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp551;if(_tmp566->tag != 23)goto _LL350;}_LL34F:
 return 22;_LL350: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp567=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp551;if(_tmp567->tag != 24)goto _LL352;}_LL351:
 return 23;_LL352: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp568=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp551;if(_tmp568->tag != 25)goto _LL354;}_LL353:
 return 24;_LL354: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp569=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp551;if(_tmp569->tag != 22)goto _LL356;}_LL355:
 return 25;_LL356: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp56A=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp551;if(_tmp56A->tag != 19)goto _LL358;}_LL357:
 return 26;_LL358: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp56B=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp551;if(_tmp56B->tag != 16)goto _LL35A;}_LL359:
 return 27;_LL35A: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp56C=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp551;if(_tmp56C->tag != 18)goto _LL35C;}_LL35B:
 return 28;_LL35C: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp56D=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp551;if(_tmp56D->tag != 26)goto _LL35E;}_LL35D:
 return 29;_LL35E: {struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp56E=(struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp551;if(_tmp56E->tag != 27)goto _LL325;}_LL35F:
 return 30;_LL325:;}
# 1898
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return 0;{
int _tmp56F=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));
if(_tmp56F != 0)
return _tmp56F;{
# 1907
struct _tuple0 _tmp10D6;struct _tuple0 _tmp570=(_tmp10D6.f1=t2,((_tmp10D6.f2=t1,_tmp10D6)));struct _tuple0 _tmp571=_tmp570;struct Cyc_Absyn_Tvar*_tmp575;struct Cyc_Absyn_Tvar*_tmp577;union Cyc_Absyn_AggrInfoU _tmp579;struct Cyc_List_List*_tmp57A;union Cyc_Absyn_AggrInfoU _tmp57C;struct Cyc_List_List*_tmp57D;struct _tuple2*_tmp57F;struct _tuple2*_tmp581;struct Cyc_List_List*_tmp583;struct Cyc_List_List*_tmp585;struct Cyc_Absyn_Datatypedecl*_tmp587;struct Cyc_List_List*_tmp588;struct Cyc_Absyn_Datatypedecl*_tmp58A;struct Cyc_List_List*_tmp58B;struct Cyc_Absyn_Datatypedecl*_tmp58D;struct Cyc_Absyn_Datatypefield*_tmp58E;struct Cyc_List_List*_tmp58F;struct Cyc_Absyn_Datatypedecl*_tmp591;struct Cyc_Absyn_Datatypefield*_tmp592;struct Cyc_List_List*_tmp593;void*_tmp595;struct Cyc_Absyn_Tqual _tmp596;void*_tmp597;union Cyc_Absyn_Constraint*_tmp598;union Cyc_Absyn_Constraint*_tmp599;union Cyc_Absyn_Constraint*_tmp59A;void*_tmp59C;struct Cyc_Absyn_Tqual _tmp59D;void*_tmp59E;union Cyc_Absyn_Constraint*_tmp59F;union Cyc_Absyn_Constraint*_tmp5A0;union Cyc_Absyn_Constraint*_tmp5A1;enum Cyc_Absyn_Sign _tmp5A3;enum Cyc_Absyn_Size_of _tmp5A4;enum Cyc_Absyn_Sign _tmp5A6;enum Cyc_Absyn_Size_of _tmp5A7;int _tmp5A9;int _tmp5AB;void*_tmp5AD;struct Cyc_Absyn_Tqual _tmp5AE;struct Cyc_Absyn_Exp*_tmp5AF;union Cyc_Absyn_Constraint*_tmp5B0;void*_tmp5B2;struct Cyc_Absyn_Tqual _tmp5B3;struct Cyc_Absyn_Exp*_tmp5B4;union Cyc_Absyn_Constraint*_tmp5B5;struct Cyc_List_List*_tmp5B9;struct Cyc_List_List*_tmp5BB;enum Cyc_Absyn_AggrKind _tmp5BD;struct Cyc_List_List*_tmp5BE;enum Cyc_Absyn_AggrKind _tmp5C0;struct Cyc_List_List*_tmp5C1;void*_tmp5C3;void*_tmp5C5;void*_tmp5C7;void*_tmp5C8;void*_tmp5CA;void*_tmp5CB;void*_tmp5CD;void*_tmp5CF;struct Cyc_Absyn_Exp*_tmp5D1;struct Cyc_Absyn_Exp*_tmp5D3;struct Cyc_Absyn_Exp*_tmp5D5;struct Cyc_Absyn_Exp*_tmp5D7;_LL361:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp572=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp571.f1;if(_tmp572->tag != 1)goto _LL363;}{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp573=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp571.f2;if(_tmp573->tag != 1)goto _LL363;};_LL362: {
# 1909
const char*_tmp10D9;void*_tmp10D8;(_tmp10D8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10D9="typecmp: can only compare closed types",_tag_dyneither(_tmp10D9,sizeof(char),39))),_tag_dyneither(_tmp10D8,sizeof(void*),0)));}_LL363:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp574=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp571.f1;if(_tmp574->tag != 2)goto _LL365;else{_tmp575=_tmp574->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp576=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp571.f2;if(_tmp576->tag != 2)goto _LL365;else{_tmp577=_tmp576->f1;}};_LL364:
# 1913
 return Cyc_Core_intcmp(_tmp577->identity,_tmp575->identity);_LL365:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp578=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp571.f1;if(_tmp578->tag != 11)goto _LL367;else{_tmp579=(_tmp578->f1).aggr_info;_tmp57A=(_tmp578->f1).targs;}}{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp57B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp571.f2;if(_tmp57B->tag != 11)goto _LL367;else{_tmp57C=(_tmp57B->f1).aggr_info;_tmp57D=(_tmp57B->f1).targs;}};_LL366: {
# 1917
struct _tuple13 _tmp5E0=Cyc_Absyn_aggr_kinded_name(_tmp579);struct _tuple2*_tmp5E2;struct _tuple13 _tmp5E1=_tmp5E0;_tmp5E2=_tmp5E1.f2;{
struct _tuple13 _tmp5E3=Cyc_Absyn_aggr_kinded_name(_tmp57C);struct _tuple2*_tmp5E5;struct _tuple13 _tmp5E4=_tmp5E3;_tmp5E5=_tmp5E4.f2;{
int _tmp5E6=Cyc_Absyn_qvar_cmp(_tmp5E2,_tmp5E5);
if(_tmp5E6 != 0)return _tmp5E6;else{
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp57A,_tmp57D);}};};}_LL367:{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp57E=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp571.f1;if(_tmp57E->tag != 13)goto _LL369;else{_tmp57F=_tmp57E->f1;}}{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp580=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp571.f2;if(_tmp580->tag != 13)goto _LL369;else{_tmp581=_tmp580->f1;}};_LL368:
# 1924
 return Cyc_Absyn_qvar_cmp(_tmp57F,_tmp581);_LL369:{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp582=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp571.f1;if(_tmp582->tag != 14)goto _LL36B;else{_tmp583=_tmp582->f1;}}{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp584=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp571.f2;if(_tmp584->tag != 14)goto _LL36B;else{_tmp585=_tmp584->f1;}};_LL36A:
# 1927
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp583,_tmp585);_LL36B:{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp586=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp571.f1;if(_tmp586->tag != 3)goto _LL36D;else{if((((_tmp586->f1).datatype_info).KnownDatatype).tag != 2)goto _LL36D;_tmp587=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp586->f1).datatype_info).KnownDatatype).val);_tmp588=(_tmp586->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp589=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp571.f2;if(_tmp589->tag != 3)goto _LL36D;else{if((((_tmp589->f1).datatype_info).KnownDatatype).tag != 2)goto _LL36D;_tmp58A=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp589->f1).datatype_info).KnownDatatype).val);_tmp58B=(_tmp589->f1).targs;}};_LL36C:
# 1931
 if(_tmp58A == _tmp587)return 0;{
int _tmp5E7=Cyc_Absyn_qvar_cmp(_tmp58A->name,_tmp587->name);
if(_tmp5E7 != 0)return _tmp5E7;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp58B,_tmp588);};_LL36D:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp58C=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp571.f1;if(_tmp58C->tag != 4)goto _LL36F;else{if((((_tmp58C->f1).field_info).KnownDatatypefield).tag != 2)goto _LL36F;_tmp58D=((struct _tuple3)(((_tmp58C->f1).field_info).KnownDatatypefield).val).f1;_tmp58E=((struct _tuple3)(((_tmp58C->f1).field_info).KnownDatatypefield).val).f2;_tmp58F=(_tmp58C->f1).targs;}}{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp590=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp571.f2;if(_tmp590->tag != 4)goto _LL36F;else{if((((_tmp590->f1).field_info).KnownDatatypefield).tag != 2)goto _LL36F;_tmp591=((struct _tuple3)(((_tmp590->f1).field_info).KnownDatatypefield).val).f1;_tmp592=((struct _tuple3)(((_tmp590->f1).field_info).KnownDatatypefield).val).f2;_tmp593=(_tmp590->f1).targs;}};_LL36E:
# 1938
 if(_tmp591 == _tmp58D)return 0;{
int _tmp5E8=Cyc_Absyn_qvar_cmp(_tmp58D->name,_tmp591->name);
if(_tmp5E8 != 0)return _tmp5E8;{
int _tmp5E9=Cyc_Absyn_qvar_cmp(_tmp58E->name,_tmp592->name);
if(_tmp5E9 != 0)return _tmp5E9;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp593,_tmp58F);};};_LL36F:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp594=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp571.f1;if(_tmp594->tag != 5)goto _LL371;else{_tmp595=(_tmp594->f1).elt_typ;_tmp596=(_tmp594->f1).elt_tq;_tmp597=((_tmp594->f1).ptr_atts).rgn;_tmp598=((_tmp594->f1).ptr_atts).nullable;_tmp599=((_tmp594->f1).ptr_atts).bounds;_tmp59A=((_tmp594->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp59B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp571.f2;if(_tmp59B->tag != 5)goto _LL371;else{_tmp59C=(_tmp59B->f1).elt_typ;_tmp59D=(_tmp59B->f1).elt_tq;_tmp59E=((_tmp59B->f1).ptr_atts).rgn;_tmp59F=((_tmp59B->f1).ptr_atts).nullable;_tmp5A0=((_tmp59B->f1).ptr_atts).bounds;_tmp5A1=((_tmp59B->f1).ptr_atts).zero_term;}};_LL370: {
# 1947
int _tmp5EA=Cyc_Tcutil_typecmp(_tmp59C,_tmp595);
if(_tmp5EA != 0)return _tmp5EA;{
int _tmp5EB=Cyc_Tcutil_typecmp(_tmp59E,_tmp597);
if(_tmp5EB != 0)return _tmp5EB;{
int _tmp5EC=Cyc_Tcutil_tqual_cmp(_tmp59D,_tmp596);
if(_tmp5EC != 0)return _tmp5EC;{
int _tmp5ED=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp5A0,_tmp599);
if(_tmp5ED != 0)return _tmp5ED;{
int _tmp5EE=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp5A1,_tmp59A);
if(_tmp5EE != 0)return _tmp5EE;
{void*_tmp5EF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5A0);void*_tmp5F0=_tmp5EF;_LL396: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5F1=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5F0;if(_tmp5F1->tag != 0)goto _LL398;}_LL397:
 return 0;_LL398:;_LL399:
 goto _LL395;_LL395:;}
# 1961
return((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp59F,_tmp598);};};};};}_LL371:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A2=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp571.f1;if(_tmp5A2->tag != 6)goto _LL373;else{_tmp5A3=_tmp5A2->f1;_tmp5A4=_tmp5A2->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp571.f2;if(_tmp5A5->tag != 6)goto _LL373;else{_tmp5A6=_tmp5A5->f1;_tmp5A7=_tmp5A5->f2;}};_LL372:
# 1964
 if(_tmp5A6 != _tmp5A3)return Cyc_Core_intcmp((int)((unsigned int)_tmp5A6),(int)((unsigned int)_tmp5A3));
if(_tmp5A7 != _tmp5A4)return Cyc_Core_intcmp((int)((unsigned int)_tmp5A7),(int)((unsigned int)_tmp5A4));
return 0;_LL373:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5A8=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp571.f1;if(_tmp5A8->tag != 7)goto _LL375;else{_tmp5A9=_tmp5A8->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5AA=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp571.f2;if(_tmp5AA->tag != 7)goto _LL375;else{_tmp5AB=_tmp5AA->f1;}};_LL374:
# 1969
 if(_tmp5A9 == _tmp5AB)return 0;else{
if(_tmp5AB == 0)return - 1;else{
if(_tmp5AB == 1  && _tmp5A9 == 0)return - 1;else{
return 1;}}}_LL375:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5AC=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp571.f1;if(_tmp5AC->tag != 8)goto _LL377;else{_tmp5AD=(_tmp5AC->f1).elt_type;_tmp5AE=(_tmp5AC->f1).tq;_tmp5AF=(_tmp5AC->f1).num_elts;_tmp5B0=(_tmp5AC->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5B1=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp571.f2;if(_tmp5B1->tag != 8)goto _LL377;else{_tmp5B2=(_tmp5B1->f1).elt_type;_tmp5B3=(_tmp5B1->f1).tq;_tmp5B4=(_tmp5B1->f1).num_elts;_tmp5B5=(_tmp5B1->f1).zero_term;}};_LL376: {
# 1976
int _tmp5F2=Cyc_Tcutil_tqual_cmp(_tmp5B3,_tmp5AE);
if(_tmp5F2 != 0)return _tmp5F2;{
int _tmp5F3=Cyc_Tcutil_typecmp(_tmp5B2,_tmp5AD);
if(_tmp5F3 != 0)return _tmp5F3;{
int _tmp5F4=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp5B0,_tmp5B5);
if(_tmp5F4 != 0)return _tmp5F4;
if(_tmp5AF == _tmp5B4)return 0;
if(_tmp5AF == 0  || _tmp5B4 == 0){
const char*_tmp10DC;void*_tmp10DB;(_tmp10DB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10DC="missing expression in array index",_tag_dyneither(_tmp10DC,sizeof(char),34))),_tag_dyneither(_tmp10DB,sizeof(void*),0)));}
# 1986
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp5AF,_tmp5B4);};};}_LL377:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp5B6=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp571.f1;if(_tmp5B6->tag != 9)goto _LL379;}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp5B7=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp571.f2;if(_tmp5B7->tag != 9)goto _LL379;};_LL378: {
# 1989
const char*_tmp10DF;void*_tmp10DE;(_tmp10DE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10DF="typecmp: function types not handled",_tag_dyneither(_tmp10DF,sizeof(char),36))),_tag_dyneither(_tmp10DE,sizeof(void*),0)));}_LL379:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp5B8=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp571.f1;if(_tmp5B8->tag != 10)goto _LL37B;else{_tmp5B9=_tmp5B8->f1;}}{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp5BA=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp571.f2;if(_tmp5BA->tag != 10)goto _LL37B;else{_tmp5BB=_tmp5BA->f1;}};_LL37A:
# 1992
 return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp5BB,_tmp5B9);_LL37B:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp5BC=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp571.f1;if(_tmp5BC->tag != 12)goto _LL37D;else{_tmp5BD=_tmp5BC->f1;_tmp5BE=_tmp5BC->f2;}}{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp5BF=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp571.f2;if(_tmp5BF->tag != 12)goto _LL37D;else{_tmp5C0=_tmp5BF->f1;_tmp5C1=_tmp5BF->f2;}};_LL37C:
# 1995
 if(_tmp5C0 != _tmp5BD){
if(_tmp5C0 == Cyc_Absyn_StructA)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp5C1,_tmp5BE);_LL37D:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp5C2=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp571.f1;if(_tmp5C2->tag != 15)goto _LL37F;else{_tmp5C3=(void*)_tmp5C2->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp5C4=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp571.f2;if(_tmp5C4->tag != 15)goto _LL37F;else{_tmp5C5=(void*)_tmp5C4->f1;}};_LL37E:
# 2000
 return Cyc_Tcutil_typecmp(_tmp5C3,_tmp5C5);_LL37F:{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp5C6=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp571.f1;if(_tmp5C6->tag != 16)goto _LL381;else{_tmp5C7=(void*)_tmp5C6->f1;_tmp5C8=(void*)_tmp5C6->f2;}}{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp5C9=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp571.f2;if(_tmp5C9->tag != 16)goto _LL381;else{_tmp5CA=(void*)_tmp5C9->f1;_tmp5CB=(void*)_tmp5C9->f2;}};_LL380: {
# 2002
int _tmp5F9=Cyc_Tcutil_typecmp(_tmp5C7,_tmp5CA);
if(_tmp5F9 != 0)return _tmp5F9;else{
return Cyc_Tcutil_typecmp(_tmp5C8,_tmp5CB);}}_LL381:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5CC=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp571.f1;if(_tmp5CC->tag != 19)goto _LL383;else{_tmp5CD=(void*)_tmp5CC->f1;}}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5CE=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp571.f2;if(_tmp5CE->tag != 19)goto _LL383;else{_tmp5CF=(void*)_tmp5CE->f1;}};_LL382:
 return Cyc_Tcutil_typecmp(_tmp5CD,_tmp5CF);_LL383:{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp5D0=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp571.f1;if(_tmp5D0->tag != 18)goto _LL385;else{_tmp5D1=_tmp5D0->f1;}}{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp5D2=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp571.f2;if(_tmp5D2->tag != 18)goto _LL385;else{_tmp5D3=_tmp5D2->f1;}};_LL384:
 _tmp5D5=_tmp5D1;_tmp5D7=_tmp5D3;goto _LL386;_LL385:{struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp5D4=(struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp571.f1;if(_tmp5D4->tag != 27)goto _LL387;else{_tmp5D5=_tmp5D4->f1;}}{struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp5D6=(struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp571.f2;if(_tmp5D6->tag != 27)goto _LL387;else{_tmp5D7=_tmp5D6->f1;}};_LL386:
# 2008
 return Cyc_Evexp_const_exp_cmp(_tmp5D5,_tmp5D7);_LL387: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp5D8=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp571.f1;if(_tmp5D8->tag != 24)goto _LL389;}_LL388:
 goto _LL38A;_LL389: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp5D9=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp571.f2;if(_tmp5D9->tag != 24)goto _LL38B;}_LL38A:
 goto _LL38C;_LL38B: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp5DA=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp571.f1;if(_tmp5DA->tag != 23)goto _LL38D;}_LL38C:
 goto _LL38E;_LL38D: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp5DB=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp571.f1;if(_tmp5DB->tag != 25)goto _LL38F;}_LL38E:
 goto _LL390;_LL38F: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp5DC=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp571.f2;if(_tmp5DC->tag != 25)goto _LL391;}_LL390:
 goto _LL392;_LL391: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp5DD=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp571.f2;if(_tmp5DD->tag != 23)goto _LL393;}_LL392: {
const char*_tmp10E2;void*_tmp10E1;(_tmp10E1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10E2="typecmp: effects not handled",_tag_dyneither(_tmp10E2,sizeof(char),29))),_tag_dyneither(_tmp10E1,sizeof(void*),0)));}_LL393:;_LL394: {
const char*_tmp10E5;void*_tmp10E4;(_tmp10E4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10E5="Unmatched case in typecmp",_tag_dyneither(_tmp10E5,sizeof(char),26))),_tag_dyneither(_tmp10E4,sizeof(void*),0)));}_LL360:;};};}
# 2019
int Cyc_Tcutil_is_arithmetic_type(void*t){
void*_tmp5FF=Cyc_Tcutil_compress(t);void*_tmp600=_tmp5FF;_LL39B: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp601=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp600;if(_tmp601->tag != 6)goto _LL39D;}_LL39C:
 goto _LL39E;_LL39D: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp602=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp600;if(_tmp602->tag != 7)goto _LL39F;}_LL39E:
 goto _LL3A0;_LL39F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp603=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp600;if(_tmp603->tag != 13)goto _LL3A1;}_LL3A0:
 goto _LL3A2;_LL3A1: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp604=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp600;if(_tmp604->tag != 14)goto _LL3A3;}_LL3A2:
 return 1;_LL3A3:;_LL3A4:
 return 0;_LL39A:;}
# 2031
int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp10E6;struct _tuple0 _tmp605=(_tmp10E6.f1=t1,((_tmp10E6.f2=t2,_tmp10E6)));struct _tuple0 _tmp606=_tmp605;int _tmp608;int _tmp60A;_LL3A6:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp607=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp606.f1;if(_tmp607->tag != 7)goto _LL3A8;else{_tmp608=_tmp607->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp609=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp606.f2;if(_tmp609->tag != 7)goto _LL3A8;else{_tmp60A=_tmp609->f1;}};_LL3A7:
# 2036
 return _tmp60A == 0  && _tmp608 != 0  || (_tmp60A == 1  && _tmp608 != 0) && _tmp608 != 1;_LL3A8:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp60B=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp606.f1;if(_tmp60B->tag != 7)goto _LL3AA;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp60C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp606.f2;if(_tmp60C->tag != 6)goto _LL3AA;};_LL3A9:
 goto _LL3AB;_LL3AA:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp60D=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp606.f1;if(_tmp60D->tag != 7)goto _LL3AC;}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp60E=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp606.f2;if(_tmp60E->tag != 19)goto _LL3AC;};_LL3AB:
 return 1;_LL3AC:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp60F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp606.f1;if(_tmp60F->tag != 6)goto _LL3AE;else{if(_tmp60F->f2 != Cyc_Absyn_LongLong_sz)goto _LL3AE;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp610=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp606.f2;if(_tmp610->tag != 6)goto _LL3AE;else{if(_tmp610->f2 != Cyc_Absyn_LongLong_sz)goto _LL3AE;}};_LL3AD:
 return 0;_LL3AE: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp611=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp606.f1;if(_tmp611->tag != 6)goto _LL3B0;else{if(_tmp611->f2 != Cyc_Absyn_LongLong_sz)goto _LL3B0;}}_LL3AF:
 return 1;_LL3B0:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp612=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp606.f1;if(_tmp612->tag != 6)goto _LL3B2;else{if(_tmp612->f2 != Cyc_Absyn_Long_sz)goto _LL3B2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp613=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp606.f2;if(_tmp613->tag != 6)goto _LL3B2;else{if(_tmp613->f2 != Cyc_Absyn_Int_sz)goto _LL3B2;}};_LL3B1:
# 2043
 goto _LL3B3;_LL3B2:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp614=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp606.f1;if(_tmp614->tag != 6)goto _LL3B4;else{if(_tmp614->f2 != Cyc_Absyn_Int_sz)goto _LL3B4;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp615=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp606.f2;if(_tmp615->tag != 6)goto _LL3B4;else{if(_tmp615->f2 != Cyc_Absyn_Long_sz)goto _LL3B4;}};_LL3B3:
 return 0;_LL3B4:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp616=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp606.f1;if(_tmp616->tag != 6)goto _LL3B6;else{if(_tmp616->f2 != Cyc_Absyn_Long_sz)goto _LL3B6;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp617=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp606.f2;if(_tmp617->tag != 7)goto _LL3B6;else{if(_tmp617->f1 != 0)goto _LL3B6;}};_LL3B5:
# 2046
 goto _LL3B7;_LL3B6:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp618=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp606.f1;if(_tmp618->tag != 6)goto _LL3B8;else{if(_tmp618->f2 != Cyc_Absyn_Int_sz)goto _LL3B8;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp619=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp606.f2;if(_tmp619->tag != 7)goto _LL3B8;else{if(_tmp619->f1 != 0)goto _LL3B8;}};_LL3B7:
 goto _LL3B9;_LL3B8:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp61A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp606.f1;if(_tmp61A->tag != 6)goto _LL3BA;else{if(_tmp61A->f2 != Cyc_Absyn_Long_sz)goto _LL3BA;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp61B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp606.f2;if(_tmp61B->tag != 6)goto _LL3BA;else{if(_tmp61B->f2 != Cyc_Absyn_Short_sz)goto _LL3BA;}};_LL3B9:
 goto _LL3BB;_LL3BA:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp61C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp606.f1;if(_tmp61C->tag != 6)goto _LL3BC;else{if(_tmp61C->f2 != Cyc_Absyn_Int_sz)goto _LL3BC;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp61D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp606.f2;if(_tmp61D->tag != 6)goto _LL3BC;else{if(_tmp61D->f2 != Cyc_Absyn_Short_sz)goto _LL3BC;}};_LL3BB:
 goto _LL3BD;_LL3BC:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp61E=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp606.f1;if(_tmp61E->tag != 19)goto _LL3BE;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp61F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp606.f2;if(_tmp61F->tag != 6)goto _LL3BE;else{if(_tmp61F->f2 != Cyc_Absyn_Short_sz)goto _LL3BE;}};_LL3BD:
 goto _LL3BF;_LL3BE:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp620=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp606.f1;if(_tmp620->tag != 6)goto _LL3C0;else{if(_tmp620->f2 != Cyc_Absyn_Long_sz)goto _LL3C0;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp621=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp606.f2;if(_tmp621->tag != 6)goto _LL3C0;else{if(_tmp621->f2 != Cyc_Absyn_Char_sz)goto _LL3C0;}};_LL3BF:
 goto _LL3C1;_LL3C0:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp622=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp606.f1;if(_tmp622->tag != 6)goto _LL3C2;else{if(_tmp622->f2 != Cyc_Absyn_Int_sz)goto _LL3C2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp623=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp606.f2;if(_tmp623->tag != 6)goto _LL3C2;else{if(_tmp623->f2 != Cyc_Absyn_Char_sz)goto _LL3C2;}};_LL3C1:
 goto _LL3C3;_LL3C2:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp624=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp606.f1;if(_tmp624->tag != 6)goto _LL3C4;else{if(_tmp624->f2 != Cyc_Absyn_Short_sz)goto _LL3C4;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp625=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp606.f2;if(_tmp625->tag != 6)goto _LL3C4;else{if(_tmp625->f2 != Cyc_Absyn_Char_sz)goto _LL3C4;}};_LL3C3:
 goto _LL3C5;_LL3C4:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp626=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp606.f1;if(_tmp626->tag != 19)goto _LL3C6;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp627=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp606.f2;if(_tmp627->tag != 6)goto _LL3C6;else{if(_tmp627->f2 != Cyc_Absyn_Char_sz)goto _LL3C6;}};_LL3C5:
 return 1;_LL3C6:;_LL3C7:
# 2056
 return 0;_LL3A5:;};}
# 2062
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
# 2065
struct _RegionHandle _tmp629=_new_region("r");struct _RegionHandle*r=& _tmp629;_push_region(r);{
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0  || 
Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v)){
struct Cyc_Core_Opt*_tmp10E7;max_arith_type=((_tmp10E7=_region_malloc(r,sizeof(*_tmp10E7)),((_tmp10E7->v=t1,_tmp10E7))));}}}}
# 2075
if(max_arith_type != 0){
if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v)){
int _tmp62B=0;_npop_handler(0);return _tmp62B;}}}
# 2080
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){
{const char*_tmp10EC;void*_tmp10EB[2];struct Cyc_String_pa_PrintArg_struct _tmp10EA;struct Cyc_String_pa_PrintArg_struct _tmp10E9;(_tmp10E9.tag=0,((_tmp10E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))),((_tmp10EA.tag=0,((_tmp10EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp10EB[0]=& _tmp10EA,((_tmp10EB[1]=& _tmp10E9,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)el->hd)->loc,((_tmp10EC="type mismatch: expecting %s but found %s",_tag_dyneither(_tmp10EC,sizeof(char),41))),_tag_dyneither(_tmp10EB,sizeof(void*),2)))))))))))));}{
int _tmp630=0;_npop_handler(0);return _tmp630;};}}}{
# 2086
int _tmp631=1;_npop_handler(0);return _tmp631;};
# 2065
;_pop_region(r);}
# 2091
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_typ(te,e)){
void*_tmp632=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp633=_tmp632;_LL3C9: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp634=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp633;if(_tmp634->tag != 5)goto _LL3CB;}_LL3CA:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);goto _LL3C8;_LL3CB:;_LL3CC:
 return 0;_LL3C8:;}
# 2097
return 1;}
# 2100
int Cyc_Tcutil_is_integral_type(void*t){
void*_tmp635=Cyc_Tcutil_compress(t);void*_tmp636=_tmp635;_LL3CE: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp637=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp636;if(_tmp637->tag != 6)goto _LL3D0;}_LL3CF:
 goto _LL3D1;_LL3D0: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp638=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp636;if(_tmp638->tag != 19)goto _LL3D2;}_LL3D1:
 goto _LL3D3;_LL3D2: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp639=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp636;if(_tmp639->tag != 13)goto _LL3D4;}_LL3D3:
 goto _LL3D5;_LL3D4: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp63A=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp636;if(_tmp63A->tag != 14)goto _LL3D6;}_LL3D5:
 return 1;_LL3D6:;_LL3D7:
 return 0;_LL3CD:;}
# 2111
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
return 1;
# 2115
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_typ)){
const char*_tmp10EF;void*_tmp10EE;(_tmp10EE=0,Cyc_Tcutil_warn(e->loc,((_tmp10EF="integral size mismatch; conversion supplied",_tag_dyneither(_tmp10EF,sizeof(char),44))),_tag_dyneither(_tmp10EE,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 2121
return 0;}
# 2125
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ))
return 1;
# 2129
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_sint_typ)){
const char*_tmp10F2;void*_tmp10F1;(_tmp10F1=0,Cyc_Tcutil_warn(e->loc,((_tmp10F2="integral size mismatch; conversion supplied",_tag_dyneither(_tmp10F2,sizeof(char),44))),_tag_dyneither(_tmp10F1,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 2135
return 0;}
# 2140
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);
# 2148
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
# 2150
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp10F3;struct _tuple0 _tmp63F=(_tmp10F3.f1=t1,((_tmp10F3.f2=t2,_tmp10F3)));struct _tuple0 _tmp640=_tmp63F;struct Cyc_Absyn_PtrInfo _tmp642;struct Cyc_Absyn_PtrInfo _tmp644;void*_tmp646;struct Cyc_Absyn_Tqual _tmp647;struct Cyc_Absyn_Exp*_tmp648;union Cyc_Absyn_Constraint*_tmp649;void*_tmp64B;struct Cyc_Absyn_Tqual _tmp64C;struct Cyc_Absyn_Exp*_tmp64D;union Cyc_Absyn_Constraint*_tmp64E;_LL3D9:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp641=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp640.f1;if(_tmp641->tag != 5)goto _LL3DB;else{_tmp642=_tmp641->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp643=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp640.f2;if(_tmp643->tag != 5)goto _LL3DB;else{_tmp644=_tmp643->f1;}};_LL3DA: {
# 2154
int okay=1;
# 2156
if(!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp642.ptr_atts).nullable,(_tmp644.ptr_atts).nullable))
# 2159
okay=!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp642.ptr_atts).nullable);
# 2161
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp642.ptr_atts).bounds,(_tmp644.ptr_atts).bounds)){
# 2164
struct _tuple0 _tmp10F4;struct _tuple0 _tmp651=(_tmp10F4.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp642.ptr_atts).bounds),((_tmp10F4.f2=
((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp644.ptr_atts).bounds),_tmp10F4)));
# 2164
struct _tuple0 _tmp652=_tmp651;struct Cyc_Absyn_Exp*_tmp658;struct Cyc_Absyn_Exp*_tmp65A;struct Cyc_Absyn_Exp*_tmp65D;_LL3E2:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp653=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp652.f1;if(_tmp653->tag != 1)goto _LL3E4;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp654=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp652.f2;if(_tmp654->tag != 0)goto _LL3E4;};_LL3E3:
# 2166
 goto _LL3E5;_LL3E4:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp655=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp652.f1;if(_tmp655->tag != 0)goto _LL3E6;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp656=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp652.f2;if(_tmp656->tag != 0)goto _LL3E6;};_LL3E5:
 okay=1;goto _LL3E1;_LL3E6:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp657=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp652.f1;if(_tmp657->tag != 1)goto _LL3E8;else{_tmp658=_tmp657->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp659=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp652.f2;if(_tmp659->tag != 1)goto _LL3E8;else{_tmp65A=_tmp659->f1;}};_LL3E7:
# 2169
 okay=okay  && Cyc_Evexp_lte_const_exp(_tmp65A,_tmp658);
# 2173
if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp644.ptr_atts).zero_term)){
const char*_tmp10F7;void*_tmp10F6;(_tmp10F6=0,Cyc_Tcutil_warn(loc,((_tmp10F7="implicit cast to shorter array",_tag_dyneither(_tmp10F7,sizeof(char),31))),_tag_dyneither(_tmp10F6,sizeof(void*),0)));}
goto _LL3E1;_LL3E8:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp65B=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp652.f1;if(_tmp65B->tag != 0)goto _LL3E1;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp65C=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp652.f2;if(_tmp65C->tag != 1)goto _LL3E1;else{_tmp65D=_tmp65C->f1;}};_LL3E9:
# 2178
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp642.ptr_atts).zero_term) && 
Cyc_Tcutil_is_bound_one((_tmp644.ptr_atts).bounds))
goto _LL3E1;
okay=0;
goto _LL3E1;_LL3E1:;}
# 2187
okay=okay  && (!(_tmp642.elt_tq).real_const  || (_tmp644.elt_tq).real_const);
# 2190
if(!Cyc_Tcutil_unify((_tmp642.ptr_atts).rgn,(_tmp644.ptr_atts).rgn)){
if(Cyc_Tcenv_region_outlives(te,(_tmp642.ptr_atts).rgn,(_tmp644.ptr_atts).rgn)){
if(Cyc_Tcutil_warn_region_coerce){
const char*_tmp10FC;void*_tmp10FB[2];struct Cyc_String_pa_PrintArg_struct _tmp10FA;struct Cyc_String_pa_PrintArg_struct _tmp10F9;(_tmp10F9.tag=0,((_tmp10F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2195
Cyc_Absynpp_typ2string((_tmp644.ptr_atts).rgn)),((_tmp10FA.tag=0,((_tmp10FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2194
Cyc_Absynpp_typ2string((_tmp642.ptr_atts).rgn)),((_tmp10FB[0]=& _tmp10FA,((_tmp10FB[1]=& _tmp10F9,Cyc_Tcutil_warn(loc,((_tmp10FC="implicit cast form region %s to region %s",_tag_dyneither(_tmp10FC,sizeof(char),42))),_tag_dyneither(_tmp10FB,sizeof(void*),2)))))))))))));}}else{
# 2196
okay=0;}}
# 2200
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp642.ptr_atts).zero_term,(_tmp644.ptr_atts).zero_term) || 
# 2203
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,(_tmp642.ptr_atts).zero_term) && (_tmp644.elt_tq).real_const);{
# 2211
int _tmp665=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp644.ptr_atts).bounds,Cyc_Absyn_bounds_one_conref) && !
# 2214
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp644.ptr_atts).zero_term);
# 2218
okay=okay  && (Cyc_Tcutil_unify(_tmp642.elt_typ,_tmp644.elt_typ) || 
(_tmp665  && ((_tmp644.elt_tq).real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp644.elt_typ)))) && Cyc_Tcutil_ptrsubtype(te,0,_tmp642.elt_typ,_tmp644.elt_typ));
# 2221
return okay;};}_LL3DB:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp645=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp640.f1;if(_tmp645->tag != 8)goto _LL3DD;else{_tmp646=(_tmp645->f1).elt_type;_tmp647=(_tmp645->f1).tq;_tmp648=(_tmp645->f1).num_elts;_tmp649=(_tmp645->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp64A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp640.f2;if(_tmp64A->tag != 8)goto _LL3DD;else{_tmp64B=(_tmp64A->f1).elt_type;_tmp64C=(_tmp64A->f1).tq;_tmp64D=(_tmp64A->f1).num_elts;_tmp64E=(_tmp64A->f1).zero_term;}};_LL3DC: {
# 2225
int okay;
# 2227
okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp649,_tmp64E) && (
(_tmp648 != 0  && _tmp64D != 0) && Cyc_Evexp_same_const_exp(_tmp648,_tmp64D));
# 2230
return(okay  && Cyc_Tcutil_unify(_tmp646,_tmp64B)) && (!_tmp647.real_const  || _tmp64C.real_const);}_LL3DD:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp64F=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp640.f1;if(_tmp64F->tag != 19)goto _LL3DF;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp650=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp640.f2;if(_tmp650->tag != 6)goto _LL3DF;};_LL3DE:
# 2232
 return 0;_LL3DF:;_LL3E0:
# 2234
 return Cyc_Tcutil_unify(t1,t2);_LL3D8:;};}
# 2238
int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp667=Cyc_Tcutil_compress(t);void*_tmp668=_tmp667;_LL3EB: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp669=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp668;if(_tmp669->tag != 5)goto _LL3ED;}_LL3EC:
 return 1;_LL3ED:;_LL3EE:
 return 0;_LL3EA:;}
# 2244
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp66A=Cyc_Tcutil_compress(t);void*_tmp66B=_tmp66A;void*_tmp66D;_LL3F0: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp66C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp66B;if(_tmp66C->tag != 5)goto _LL3F2;else{_tmp66D=(_tmp66C->f1).elt_typ;}}_LL3F1:
 return _tmp66D;_LL3F2:;_LL3F3: {
const char*_tmp10FF;void*_tmp10FE;(_tmp10FE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10FF="pointer_elt_type",_tag_dyneither(_tmp10FF,sizeof(char),17))),_tag_dyneither(_tmp10FE,sizeof(void*),0)));}_LL3EF:;}
# 2250
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp670=Cyc_Tcutil_compress(t);void*_tmp671=_tmp670;struct Cyc_Absyn_PtrAtts*_tmp673;_LL3F5: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp672=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp671;if(_tmp672->tag != 5)goto _LL3F7;else{_tmp673=(struct Cyc_Absyn_PtrAtts*)&(_tmp672->f1).ptr_atts;}}_LL3F6:
 return _tmp673->rgn;_LL3F7:;_LL3F8: {
const char*_tmp1102;void*_tmp1101;(_tmp1101=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1102="pointer_elt_type",_tag_dyneither(_tmp1102,sizeof(char),17))),_tag_dyneither(_tmp1101,sizeof(void*),0)));}_LL3F4:;}
# 2257
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp676=Cyc_Tcutil_compress(t);void*_tmp677=_tmp676;void*_tmp679;_LL3FA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp678=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp677;if(_tmp678->tag != 5)goto _LL3FC;else{_tmp679=((_tmp678->f1).ptr_atts).rgn;}}_LL3FB:
# 2260
*rgn=_tmp679;
return 1;_LL3FC:;_LL3FD:
 return 0;_LL3F9:;}
# 2267
int Cyc_Tcutil_is_boxed(void*t){
return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;}
# 2271
int Cyc_Tcutil_is_dyneither_ptr(void*t){
void*_tmp67A=Cyc_Tcutil_compress(t);void*_tmp67B=_tmp67A;union Cyc_Absyn_Constraint*_tmp67D;_LL3FF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp67C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp67B;if(_tmp67C->tag != 5)goto _LL401;else{_tmp67D=((_tmp67C->f1).ptr_atts).bounds;}}_LL400:
# 2274
 return((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp67D)== (void*)& Cyc_Absyn_DynEither_b_val;_LL401:;_LL402:
 return 0;_LL3FE:;}
# 2280
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp67E=e->r;void*_tmp67F=_tmp67E;struct _dyneither_ptr _tmp685;void*_tmp687;struct Cyc_Absyn_Exp*_tmp688;_LL404: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp680=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp67F;if(_tmp680->tag != 0)goto _LL406;else{if(((_tmp680->f1).Int_c).tag != 5)goto _LL406;if(((struct _tuple7)((_tmp680->f1).Int_c).val).f2 != 0)goto _LL406;}}_LL405:
 goto _LL407;_LL406: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp681=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp67F;if(_tmp681->tag != 0)goto _LL408;else{if(((_tmp681->f1).Char_c).tag != 2)goto _LL408;if(((struct _tuple5)((_tmp681->f1).Char_c).val).f2 != 0)goto _LL408;}}_LL407:
 goto _LL409;_LL408: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp682=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp67F;if(_tmp682->tag != 0)goto _LL40A;else{if(((_tmp682->f1).Short_c).tag != 4)goto _LL40A;if(((struct _tuple6)((_tmp682->f1).Short_c).val).f2 != 0)goto _LL40A;}}_LL409:
 goto _LL40B;_LL40A: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp683=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp67F;if(_tmp683->tag != 0)goto _LL40C;else{if(((_tmp683->f1).LongLong_c).tag != 6)goto _LL40C;if(((struct _tuple8)((_tmp683->f1).LongLong_c).val).f2 != 0)goto _LL40C;}}_LL40B:
# 2286
 return 1;_LL40C: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp684=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp67F;if(_tmp684->tag != 0)goto _LL40E;else{if(((_tmp684->f1).Wchar_c).tag != 3)goto _LL40E;_tmp685=(struct _dyneither_ptr)((_tmp684->f1).Wchar_c).val;}}_LL40D: {
# 2288
unsigned long _tmp689=Cyc_strlen((struct _dyneither_ptr)_tmp685);
int i=0;
if(_tmp689 >= 2  && *((const char*)_check_dyneither_subscript(_tmp685,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp685,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmp685,sizeof(char),1))== 'x'  && _tmp689 >= 3) && *((const char*)_check_dyneither_subscript(_tmp685,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp689;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp685,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 2298
return 0;}}_LL40E: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp686=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp67F;if(_tmp686->tag != 13)goto _LL410;else{_tmp687=(void*)_tmp686->f1;_tmp688=_tmp686->f2;}}_LL40F:
 return Cyc_Tcutil_is_zero(_tmp688) && Cyc_Tcutil_admits_zero(_tmp687);_LL410:;_LL411:
 return 0;_LL403:;}
# 2304
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
# 2311
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 2316
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 2321
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
# 2328
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 2333
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 2338
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
enum Cyc_Absyn_KindQual _tmp68B;enum Cyc_Absyn_AliasQual _tmp68C;struct Cyc_Absyn_Kind*_tmp68A=ka;_tmp68B=_tmp68A->kind;_tmp68C=_tmp68A->aliasqual;
switch(_tmp68C){case Cyc_Absyn_Aliasable: _LL412:
# 2342
 switch(_tmp68B){case Cyc_Absyn_AnyKind: _LL414:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LL415:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL416:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL417:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL418:
 return& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL419:
 return& Cyc_Tcutil_iko;}case Cyc_Absyn_Unique: _LL413:
# 2351
 switch(_tmp68B){case Cyc_Absyn_AnyKind: _LL41C:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL41D:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL41E:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL41F:
 return& Cyc_Tcutil_urko;default: _LL420:
 break;}
# 2358
break;case Cyc_Absyn_Top: _LL41B:
# 2360
 switch(_tmp68B){case Cyc_Absyn_AnyKind: _LL423:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL424:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL425:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL426:
 return& Cyc_Tcutil_trko;default: _LL427:
 break;}
# 2367
break;}{
# 2369
const char*_tmp1106;void*_tmp1105[1];struct Cyc_String_pa_PrintArg_struct _tmp1104;(_tmp1104.tag=0,((_tmp1104.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmp1105[0]=& _tmp1104,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1106="kind_to_opt: bad kind %s\n",_tag_dyneither(_tmp1106,sizeof(char),26))),_tag_dyneither(_tmp1105,sizeof(void*),1)))))));};}
# 2372
static void**Cyc_Tcutil_kind_to_b(struct Cyc_Absyn_Kind*ka){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ab_v={0,& Cyc_Tcutil_ak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct mb_v={0,& Cyc_Tcutil_mk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct bb_v={0,& Cyc_Tcutil_bk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rb_v={0,& Cyc_Tcutil_rk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct eb_v={0,& Cyc_Tcutil_ek};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ib_v={0,& Cyc_Tcutil_ik};
# 2380
static void*ab=(void*)& ab_v;
static void*mb=(void*)& mb_v;
static void*bb=(void*)& bb_v;
static void*rb=(void*)& rb_v;
static void*eb=(void*)& eb_v;
static void*ib=(void*)& ib_v;
# 2387
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tab_v={0,& Cyc_Tcutil_tak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tmb_v={0,& Cyc_Tcutil_tmk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tbb_v={0,& Cyc_Tcutil_tbk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct trb_v={0,& Cyc_Tcutil_trk};
# 2392
static void*tab=(void*)& tab_v;
static void*tmb=(void*)& tmb_v;
static void*tbb=(void*)& tbb_v;
static void*trb=(void*)& trb_v;
# 2397
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct uab_v={0,& Cyc_Tcutil_uak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct umb_v={0,& Cyc_Tcutil_umk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ubb_v={0,& Cyc_Tcutil_ubk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct urb_v={0,& Cyc_Tcutil_urk};
# 2402
static void*uab=(void*)& uab_v;
static void*umb=(void*)& umb_v;
static void*ubb=(void*)& ubb_v;
static void*urb=(void*)& urb_v;
# 2407
enum Cyc_Absyn_KindQual _tmp691;enum Cyc_Absyn_AliasQual _tmp692;struct Cyc_Absyn_Kind*_tmp690=ka;_tmp691=_tmp690->kind;_tmp692=_tmp690->aliasqual;
switch(_tmp692){case Cyc_Absyn_Aliasable: _LL429:
# 2410
 switch(_tmp691){case Cyc_Absyn_AnyKind: _LL42B:
 return& ab;case Cyc_Absyn_MemKind: _LL42C:
 return& mb;case Cyc_Absyn_BoxKind: _LL42D:
 return& bb;case Cyc_Absyn_RgnKind: _LL42E:
 return& rb;case Cyc_Absyn_EffKind: _LL42F:
 return& eb;case Cyc_Absyn_IntKind: _LL430:
 return& ib;}case Cyc_Absyn_Unique: _LL42A:
# 2419
 switch(_tmp691){case Cyc_Absyn_AnyKind: _LL433:
 return& uab;case Cyc_Absyn_MemKind: _LL434:
 return& umb;case Cyc_Absyn_BoxKind: _LL435:
 return& ubb;case Cyc_Absyn_RgnKind: _LL436:
 return& urb;default: _LL437:
 break;}
# 2426
break;case Cyc_Absyn_Top: _LL432:
# 2428
 switch(_tmp691){case Cyc_Absyn_AnyKind: _LL43A:
 return& tab;case Cyc_Absyn_MemKind: _LL43B:
 return& tmb;case Cyc_Absyn_BoxKind: _LL43C:
 return& tbb;case Cyc_Absyn_RgnKind: _LL43D:
 return& trb;default: _LL43E:
 break;}
# 2435
break;}{
# 2437
const char*_tmp110A;void*_tmp1109[1];struct Cyc_String_pa_PrintArg_struct _tmp1108;(_tmp1108.tag=0,((_tmp1108.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmp1109[0]=& _tmp1108,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp110A="kind_to_b: bad kind %s\n",_tag_dyneither(_tmp110A,sizeof(char),24))),_tag_dyneither(_tmp1109,sizeof(void*),1)))))));};}
# 2440
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return*Cyc_Tcutil_kind_to_b(k);}
# 2443
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
# 2445
struct Cyc_Core_Opt*_tmp110B;return(_tmp110B=_cycalloc(sizeof(*_tmp110B)),((_tmp110B->v=Cyc_Tcutil_kind_to_bound(k),_tmp110B)));}
# 2450
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){
{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp110E;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp110D;e1->r=(void*)((_tmp110D=_cycalloc(sizeof(*_tmp110D)),((_tmp110D[0]=((_tmp110E.tag=0,((_tmp110E.f1=Cyc_Absyn_Null_c,_tmp110E)))),_tmp110D))));}{
struct Cyc_Core_Opt*_tmp6A7=Cyc_Tcenv_lookup_opt_type_vars(te);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp1114;struct Cyc_Absyn_PtrInfo _tmp1113;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1112;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6A8=(_tmp1112=_cycalloc(sizeof(*_tmp1112)),((_tmp1112[0]=((_tmp1114.tag=5,((_tmp1114.f1=((_tmp1113.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp6A7),((_tmp1113.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp1113.ptr_atts=(
((_tmp1113.ptr_atts).rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp6A7),(((_tmp1113.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp1113.ptr_atts).bounds=
# 2458
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp1113.ptr_atts).zero_term=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp1113.ptr_atts).ptrloc=0,_tmp1113.ptr_atts)))))))))),_tmp1113)))))),_tmp1114)))),_tmp1112)));
e1->topt=(void*)_tmp6A8;{
int bogus=0;
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
if(bogus != 0){
const char*_tmp1119;void*_tmp1118[2];struct Cyc_String_pa_PrintArg_struct _tmp1117;struct Cyc_String_pa_PrintArg_struct _tmp1116;(_tmp1116.tag=0,((_tmp1116.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(e1->loc)),((_tmp1117.tag=0,((_tmp1117.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp1118[0]=& _tmp1117,((_tmp1118[1]=& _tmp1116,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1119="zero_to_null resulted in an alias coercion on %s at %s\n",_tag_dyneither(_tmp1119,sizeof(char),56))),_tag_dyneither(_tmp1118,sizeof(void*),2)))))))))))));}
return retv;};};}
# 2468
return 0;}
# 2471
struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
switch(c){case Cyc_Absyn_Unknown_coercion: _LL440: {
const char*_tmp111A;return(_tmp111A="unknown",_tag_dyneither(_tmp111A,sizeof(char),8));}case Cyc_Absyn_No_coercion: _LL441: {
const char*_tmp111B;return(_tmp111B="no coercion",_tag_dyneither(_tmp111B,sizeof(char),12));}case Cyc_Absyn_NonNull_to_Null: _LL442: {
const char*_tmp111C;return(_tmp111C="null check",_tag_dyneither(_tmp111C,sizeof(char),11));}case Cyc_Absyn_Other_coercion: _LL443: {
const char*_tmp111D;return(_tmp111D="other coercion",_tag_dyneither(_tmp111D,sizeof(char),15));}}}
# 2480
int Cyc_Tcutil_warn_alias_coerce=0;
# 2486
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0,& Cyc_Tcutil_rk};
# 2490
static int counter=0;
struct _dyneither_ptr*_tmp112A;const char*_tmp1129;void*_tmp1128[1];struct Cyc_Int_pa_PrintArg_struct _tmp1127;struct _tuple2*_tmp1126;struct _tuple2*v=(_tmp1126=_cycalloc(sizeof(*_tmp1126)),((_tmp1126->f1=Cyc_Absyn_Loc_n,((_tmp1126->f2=((_tmp112A=_cycalloc(sizeof(*_tmp112A)),((_tmp112A[0]=(struct _dyneither_ptr)((_tmp1127.tag=1,((_tmp1127.f1=(unsigned long)counter ++,((_tmp1128[0]=& _tmp1127,Cyc_aprintf(((_tmp1129="__aliasvar%d",_tag_dyneither(_tmp1129,sizeof(char),13))),_tag_dyneither(_tmp1128,sizeof(void*),1)))))))),_tmp112A)))),_tmp1126)))));
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(v,e_typ,e);
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp112D;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp112C;struct Cyc_Absyn_Exp*ve=Cyc_Absyn_varb_exp(v,(void*)((_tmp112C=_cycalloc(sizeof(*_tmp112C)),((_tmp112C[0]=((_tmp112D.tag=4,((_tmp112D.f1=vd,_tmp112D)))),_tmp112C)))),e->loc);
# 2499
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 2501
{void*_tmp6B4=Cyc_Tcutil_compress(e_typ);void*_tmp6B5=_tmp6B4;void*_tmp6B7;struct Cyc_Absyn_Tqual _tmp6B8;void*_tmp6B9;union Cyc_Absyn_Constraint*_tmp6BA;union Cyc_Absyn_Constraint*_tmp6BB;union Cyc_Absyn_Constraint*_tmp6BC;struct Cyc_Absyn_PtrLoc*_tmp6BD;_LL446: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6B6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6B5;if(_tmp6B6->tag != 5)goto _LL448;else{_tmp6B7=(_tmp6B6->f1).elt_typ;_tmp6B8=(_tmp6B6->f1).elt_tq;_tmp6B9=((_tmp6B6->f1).ptr_atts).rgn;_tmp6BA=((_tmp6B6->f1).ptr_atts).nullable;_tmp6BB=((_tmp6B6->f1).ptr_atts).bounds;_tmp6BC=((_tmp6B6->f1).ptr_atts).zero_term;_tmp6BD=((_tmp6B6->f1).ptr_atts).ptrloc;}}_LL447:
# 2503
{void*_tmp6BE=Cyc_Tcutil_compress(_tmp6B9);void*_tmp6BF=_tmp6BE;void**_tmp6C1;struct Cyc_Core_Opt*_tmp6C2;_LL44B: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp6C0=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp6BF;if(_tmp6C0->tag != 1)goto _LL44D;else{_tmp6C1=(void**)((void**)& _tmp6C0->f2);_tmp6C2=_tmp6C0->f4;}}_LL44C: {
# 2505
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1130;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp112F;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp6C3=(_tmp112F=_cycalloc(sizeof(*_tmp112F)),((_tmp112F[0]=((_tmp1130.tag=2,((_tmp1130.f1=tv,_tmp1130)))),_tmp112F)));
*_tmp6C1=(void*)_tmp6C3;
goto _LL44A;}_LL44D:;_LL44E:
 goto _LL44A;_LL44A:;}
# 2510
goto _LL445;_LL448:;_LL449:
 goto _LL445;_LL445:;}
# 2514
e->topt=0;
vd->initializer=0;{
# 2518
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 2520
struct _tuple15 _tmp1131;return(_tmp1131.f1=d,((_tmp1131.f2=ve,_tmp1131)));};}
# 2525
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ,void*wants_typ,unsigned int loc){
# 2528
struct _RegionHandle _tmp6CF=_new_region("r");struct _RegionHandle*r=& _tmp6CF;_push_region(r);
if((Cyc_Tcutil_is_noalias_path(r,e) && Cyc_Tcutil_is_noalias_pointer(e_typ,0)) && 
Cyc_Tcutil_is_pointer_type(e_typ)){
# 2533
void*_tmp6D0=Cyc_Tcutil_compress(wants_typ);void*_tmp6D1=_tmp6D0;void*_tmp6D3;_LL450: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6D2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6D1;if(_tmp6D2->tag != 5)goto _LL452;else{_tmp6D3=((_tmp6D2->f1).ptr_atts).rgn;}}_LL451: {
# 2535
void*_tmp6D5=Cyc_Tcutil_compress(_tmp6D3);void*_tmp6D6=_tmp6D5;_LL457: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp6D7=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp6D6;if(_tmp6D7->tag != 20)goto _LL459;}_LL458: {
int _tmp6D8=0;_npop_handler(0);return _tmp6D8;}_LL459:;_LL45A: {
# 2538
struct Cyc_Absyn_Kind*_tmp6D9=Cyc_Tcutil_typ_kind(_tmp6D3);
int _tmp6DA=_tmp6D9->kind == Cyc_Absyn_RgnKind  && _tmp6D9->aliasqual == Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp6DA;}_LL456:;}_LL452: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp6D4=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6D1;if(_tmp6D4->tag != 17)goto _LL454;}_LL453: {
# 2542
const char*_tmp1136;void*_tmp1135[2];struct Cyc_String_pa_PrintArg_struct _tmp1134;struct Cyc_String_pa_PrintArg_struct _tmp1133;(_tmp1133.tag=0,((_tmp1133.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(loc)),((_tmp1134.tag=0,((_tmp1134.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_compress(wants_typ))),((_tmp1135[0]=& _tmp1134,((_tmp1135[1]=& _tmp1133,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1136="got typedef %s in can_insert_alias at %s\n",_tag_dyneither(_tmp1136,sizeof(char),42))),_tag_dyneither(_tmp1135,sizeof(void*),2)))))))))))));}_LL454:;_LL455: {
int _tmp6DF=0;_npop_handler(0);return _tmp6DF;}_LL44F:;}{
# 2547
int _tmp6E0=0;_npop_handler(0);return _tmp6E0;};
# 2529
;_pop_region(r);}
# 2551
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 2556
if(Cyc_Tcutil_unify(t1,t2))return 1;
# 2558
if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
# 2560
if(Cyc_Tcutil_will_lose_precision(t1,t2)){
const char*_tmp113B;void*_tmp113A[2];struct Cyc_String_pa_PrintArg_struct _tmp1139;struct Cyc_String_pa_PrintArg_struct _tmp1138;(_tmp1138.tag=0,((_tmp1138.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp1139.tag=0,((_tmp1139.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp113A[0]=& _tmp1139,((_tmp113A[1]=& _tmp1138,Cyc_Tcutil_warn(e->loc,((_tmp113B="integral size mismatch; %s -> %s conversion supplied",_tag_dyneither(_tmp113B,sizeof(char),53))),_tag_dyneither(_tmp113A,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
return 1;}else{
# 2569
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce){
const char*_tmp1141;void*_tmp1140[3];struct Cyc_String_pa_PrintArg_struct _tmp113F;struct Cyc_String_pa_PrintArg_struct _tmp113E;struct Cyc_String_pa_PrintArg_struct _tmp113D;(_tmp113D.tag=0,((_tmp113D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp113E.tag=0,((_tmp113E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp113F.tag=0,((_tmp113F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp1140[0]=& _tmp113F,((_tmp1140[1]=& _tmp113E,((_tmp1140[2]=& _tmp113D,Cyc_Tcutil_warn(e->loc,((_tmp1141="implicit alias coercion for %s:%s to %s",_tag_dyneither(_tmp1141,sizeof(char),40))),_tag_dyneither(_tmp1140,sizeof(void*),3)))))))))))))))))));}
*alias_coercion=1;}
# 2576
if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);
return 1;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e))
return 1;else{
if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= Cyc_Absyn_Unknown_coercion){
# 2584
if(c != Cyc_Absyn_No_coercion)Cyc_Tcutil_unchecked_cast(te,e,t2,c);
if(c != Cyc_Absyn_NonNull_to_Null){
const char*_tmp1146;void*_tmp1145[2];struct Cyc_String_pa_PrintArg_struct _tmp1144;struct Cyc_String_pa_PrintArg_struct _tmp1143;(_tmp1143.tag=0,((_tmp1143.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp1144.tag=0,((_tmp1144.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp1145[0]=& _tmp1144,((_tmp1145[1]=& _tmp1143,Cyc_Tcutil_warn(e->loc,((_tmp1146="implicit cast from %s to %s",_tag_dyneither(_tmp1146,sizeof(char),28))),_tag_dyneither(_tmp1145,sizeof(void*),2)))))))))))));}
return 1;}else{
# 2590
return 0;}}}}}
# 2597
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){
# 2600
int bogus=0;
return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}
# 2604
int Cyc_Tcutil_coerceable(void*t){
void*_tmp6EE=Cyc_Tcutil_compress(t);void*_tmp6EF=_tmp6EE;_LL45C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6F0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6EF;if(_tmp6F0->tag != 6)goto _LL45E;}_LL45D:
 goto _LL45F;_LL45E: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp6F1=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp6EF;if(_tmp6F1->tag != 7)goto _LL460;}_LL45F:
 return 1;_LL460:;_LL461:
 return 0;_LL45B:;}
# 2622 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 2626
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ_f(struct _tuple23*env,struct Cyc_Absyn_Aggrfield*x){
# 2629
struct _tuple23 _tmp6F2=*env;struct Cyc_List_List*_tmp6F4;struct _RegionHandle*_tmp6F5;struct Cyc_Tcenv_Tenv*_tmp6F6;int _tmp6F7;struct _tuple23 _tmp6F3=_tmp6F2;_tmp6F4=_tmp6F3.f1;_tmp6F5=_tmp6F3.f2;_tmp6F6=_tmp6F3.f3;_tmp6F7=_tmp6F3.f4;{
# 2631
void*_tmp6F8=_tmp6F4 == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp6F5,_tmp6F4,x->type);
struct Cyc_List_List*_tmp6F9=Cyc_Tcutil_flatten_typ(_tmp6F5,_tmp6F7,_tmp6F6,_tmp6F8);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6F9)== 1){
struct _tuple12*_tmp1149;struct Cyc_List_List*_tmp1148;return(_tmp1148=_region_malloc(_tmp6F5,sizeof(*_tmp1148)),((_tmp1148->hd=((_tmp1149=_region_malloc(_tmp6F5,sizeof(*_tmp1149)),((_tmp1149->f1=x->tq,((_tmp1149->f2=_tmp6F8,_tmp1149)))))),((_tmp1148->tl=0,_tmp1148)))));}else{
return _tmp6F9;}};}struct _tuple24{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2637
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple24*env,struct _tuple12*x){
# 2639
struct _tuple24 _tmp6FC=*env;struct _RegionHandle*_tmp6FE;struct Cyc_Tcenv_Tenv*_tmp6FF;int _tmp700;struct _tuple24 _tmp6FD=_tmp6FC;_tmp6FE=_tmp6FD.f1;_tmp6FF=_tmp6FD.f2;_tmp700=_tmp6FD.f3;{
struct _tuple12 _tmp701=*x;struct Cyc_Absyn_Tqual _tmp703;void*_tmp704;struct _tuple12 _tmp702=_tmp701;_tmp703=_tmp702.f1;_tmp704=_tmp702.f2;{
struct Cyc_List_List*_tmp705=Cyc_Tcutil_flatten_typ(_tmp6FE,_tmp700,_tmp6FF,_tmp704);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp705)== 1){
struct _tuple12*_tmp114C;struct Cyc_List_List*_tmp114B;return(_tmp114B=_region_malloc(_tmp6FE,sizeof(*_tmp114B)),((_tmp114B->hd=((_tmp114C=_region_malloc(_tmp6FE,sizeof(*_tmp114C)),((_tmp114C->f1=_tmp703,((_tmp114C->f2=_tmp704,_tmp114C)))))),((_tmp114B->tl=0,_tmp114B)))));}else{
return _tmp705;}};};}
# 2646
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2650
if(flatten){
t1=Cyc_Tcutil_compress(t1);{
void*_tmp708=t1;struct Cyc_List_List*_tmp70B;struct Cyc_Absyn_Aggrdecl*_tmp70D;struct Cyc_List_List*_tmp70E;struct Cyc_List_List*_tmp710;_LL463: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp709=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp708;if(_tmp709->tag != 0)goto _LL465;}_LL464:
 return 0;_LL465: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp70A=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp708;if(_tmp70A->tag != 10)goto _LL467;else{_tmp70B=_tmp70A->f1;}}_LL466: {
# 2655
struct _tuple24 _tmp114D;struct _tuple24 _tmp711=(_tmp114D.f1=r,((_tmp114D.f2=te,((_tmp114D.f3=flatten,_tmp114D)))));
# 2657
struct Cyc_List_List*_tmp712=_tmp70B;struct _tuple12*_tmp713;struct Cyc_List_List*_tmp714;_LL46E: if(_tmp712 != 0)goto _LL470;_LL46F:
 return 0;_LL470: if(_tmp712 == 0)goto _LL46D;_tmp713=(struct _tuple12*)_tmp712->hd;_tmp714=_tmp712->tl;_LL471: {
# 2660
struct Cyc_List_List*_tmp715=Cyc_Tcutil_rcopy_tqt(& _tmp711,_tmp713);
_tmp711.f3=0;{
struct Cyc_List_List*_tmp716=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple12*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp711,_tmp70B);
struct Cyc_List_List*_tmp114E;struct Cyc_List_List*_tmp717=(_tmp114E=_region_malloc(r,sizeof(*_tmp114E)),((_tmp114E->hd=_tmp715,((_tmp114E->tl=_tmp716,_tmp114E)))));
struct Cyc_List_List*_tmp114F;return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,((_tmp114F=_region_malloc(r,sizeof(*_tmp114F)),((_tmp114F->hd=_tmp715,((_tmp114F->tl=_tmp716,_tmp114F)))))));};}_LL46D:;}_LL467: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp70C=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp708;if(_tmp70C->tag != 11)goto _LL469;else{if((((_tmp70C->f1).aggr_info).KnownAggr).tag != 2)goto _LL469;_tmp70D=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp70C->f1).aggr_info).KnownAggr).val);_tmp70E=(_tmp70C->f1).targs;}}_LL468:
# 2668
 if(((_tmp70D->kind == Cyc_Absyn_UnionA  || _tmp70D->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp70D->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp70D->impl))->rgn_po != 0){
# 2670
struct _tuple12*_tmp1152;struct Cyc_List_List*_tmp1151;return(_tmp1151=_region_malloc(r,sizeof(*_tmp1151)),((_tmp1151->hd=((_tmp1152=_region_malloc(r,sizeof(*_tmp1152)),((_tmp1152->f1=Cyc_Absyn_empty_tqual(0),((_tmp1152->f2=t1,_tmp1152)))))),((_tmp1151->tl=0,_tmp1151)))));}{
struct Cyc_List_List*_tmp71D=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp70D->tvs,_tmp70E);
struct _tuple23 _tmp1153;struct _tuple23 env=(_tmp1153.f1=_tmp71D,((_tmp1153.f2=r,((_tmp1153.f3=te,((_tmp1153.f4=flatten,_tmp1153)))))));
struct Cyc_List_List*_tmp71E=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp70D->impl))->fields;struct Cyc_List_List*_tmp71F=_tmp71E;struct Cyc_Absyn_Aggrfield*_tmp720;struct Cyc_List_List*_tmp721;_LL473: if(_tmp71F != 0)goto _LL475;_LL474:
 return 0;_LL475: if(_tmp71F == 0)goto _LL472;_tmp720=(struct Cyc_Absyn_Aggrfield*)_tmp71F->hd;_tmp721=_tmp71F->tl;_LL476: {
# 2676
struct Cyc_List_List*_tmp722=Cyc_Tcutil_flatten_typ_f(& env,_tmp720);
env.f4=0;{
struct Cyc_List_List*_tmp723=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp721);
struct Cyc_List_List*_tmp1154;struct Cyc_List_List*_tmp724=(_tmp1154=_region_malloc(r,sizeof(*_tmp1154)),((_tmp1154->hd=_tmp722,((_tmp1154->tl=_tmp723,_tmp1154)))));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp724);};}_LL472:;};_LL469: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp70F=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp708;if(_tmp70F->tag != 12)goto _LL46B;else{if(_tmp70F->f1 != Cyc_Absyn_StructA)goto _LL46B;_tmp710=_tmp70F->f2;}}_LL46A: {
# 2683
struct _tuple23 _tmp1155;struct _tuple23 env=(_tmp1155.f1=0,((_tmp1155.f2=r,((_tmp1155.f3=te,((_tmp1155.f4=flatten,_tmp1155)))))));
struct Cyc_List_List*_tmp727=_tmp710;struct Cyc_Absyn_Aggrfield*_tmp728;struct Cyc_List_List*_tmp729;_LL478: if(_tmp727 != 0)goto _LL47A;_LL479:
 return 0;_LL47A: if(_tmp727 == 0)goto _LL477;_tmp728=(struct Cyc_Absyn_Aggrfield*)_tmp727->hd;_tmp729=_tmp727->tl;_LL47B: {
# 2687
struct Cyc_List_List*_tmp72A=Cyc_Tcutil_flatten_typ_f(& env,_tmp728);
env.f4=0;{
struct Cyc_List_List*_tmp72B=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp729);
struct Cyc_List_List*_tmp1156;struct Cyc_List_List*_tmp72C=(_tmp1156=_region_malloc(r,sizeof(*_tmp1156)),((_tmp1156->hd=_tmp72A,((_tmp1156->tl=_tmp72B,_tmp1156)))));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp72C);};}_LL477:;}_LL46B:;_LL46C:
# 2693
 goto _LL462;_LL462:;};}{
# 2696
struct _tuple12*_tmp1159;struct Cyc_List_List*_tmp1158;return(_tmp1158=_region_malloc(r,sizeof(*_tmp1158)),((_tmp1158->hd=((_tmp1159=_region_malloc(r,sizeof(*_tmp1159)),((_tmp1159->f1=Cyc_Absyn_empty_tqual(0),((_tmp1159->f2=t1,_tmp1159)))))),((_tmp1158->tl=0,_tmp1158)))));};}
# 2700
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp731=(void*)t->hd;void*_tmp732=_tmp731;_LL47D: {struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*_tmp733=(struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*)_tmp732;if(_tmp733->tag != 23)goto _LL47F;}_LL47E:
 goto _LL480;_LL47F: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp734=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp732;if(_tmp734->tag != 4)goto _LL481;}_LL480:
 goto _LL482;_LL481: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp735=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp732;if(_tmp735->tag != 20)goto _LL483;}_LL482:
# 2706
 continue;_LL483:;_LL484:
# 2708
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;_LL47C:;}}
# 2711
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}
# 2714
return 1;}
# 2717
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp115A;struct _tuple0 _tmp736=(_tmp115A.f1=Cyc_Tcutil_compress(t1),((_tmp115A.f2=Cyc_Tcutil_compress(t2),_tmp115A)));struct _tuple0 _tmp737=_tmp736;enum Cyc_Absyn_Size_of _tmp739;enum Cyc_Absyn_Size_of _tmp73B;_LL486:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp738=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp737.f1;if(_tmp738->tag != 6)goto _LL488;else{_tmp739=_tmp738->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp73A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp737.f2;if(_tmp73A->tag != 6)goto _LL488;else{_tmp73B=_tmp73A->f2;}};_LL487:
# 2720
 return(_tmp739 == _tmp73B  || _tmp739 == Cyc_Absyn_Int_sz  && _tmp73B == Cyc_Absyn_Long_sz) || 
_tmp739 == Cyc_Absyn_Long_sz  && _tmp73B == Cyc_Absyn_Int_sz;_LL488:;_LL489:
 return 0;_LL485:;}
# 2728
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2731
if(Cyc_Tcutil_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}
# 2736
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp115B;struct _tuple0 _tmp73D=(_tmp115B.f1=t1,((_tmp115B.f2=t2,_tmp115B)));struct _tuple0 _tmp73E=_tmp73D;void*_tmp740;struct Cyc_Absyn_Tqual _tmp741;void*_tmp742;union Cyc_Absyn_Constraint*_tmp743;union Cyc_Absyn_Constraint*_tmp744;union Cyc_Absyn_Constraint*_tmp745;void*_tmp747;struct Cyc_Absyn_Tqual _tmp748;void*_tmp749;union Cyc_Absyn_Constraint*_tmp74A;union Cyc_Absyn_Constraint*_tmp74B;union Cyc_Absyn_Constraint*_tmp74C;struct Cyc_Absyn_Datatypedecl*_tmp74E;struct Cyc_Absyn_Datatypefield*_tmp74F;struct Cyc_List_List*_tmp750;struct Cyc_Absyn_Datatypedecl*_tmp752;struct Cyc_List_List*_tmp753;struct Cyc_Absyn_FnInfo _tmp755;struct Cyc_Absyn_FnInfo _tmp757;_LL48B:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp73F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp73E.f1;if(_tmp73F->tag != 5)goto _LL48D;else{_tmp740=(_tmp73F->f1).elt_typ;_tmp741=(_tmp73F->f1).elt_tq;_tmp742=((_tmp73F->f1).ptr_atts).rgn;_tmp743=((_tmp73F->f1).ptr_atts).nullable;_tmp744=((_tmp73F->f1).ptr_atts).bounds;_tmp745=((_tmp73F->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp746=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp73E.f2;if(_tmp746->tag != 5)goto _LL48D;else{_tmp747=(_tmp746->f1).elt_typ;_tmp748=(_tmp746->f1).elt_tq;_tmp749=((_tmp746->f1).ptr_atts).rgn;_tmp74A=((_tmp746->f1).ptr_atts).nullable;_tmp74B=((_tmp746->f1).ptr_atts).bounds;_tmp74C=((_tmp746->f1).ptr_atts).zero_term;}};_LL48C:
# 2744
 if(_tmp741.real_const  && !_tmp748.real_const)
return 0;
# 2747
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp743,_tmp74A) && 
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp743)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp74A))
return 0;
# 2751
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp745,_tmp74C) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp745)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp74C))
return 0;
# 2755
if((!Cyc_Tcutil_unify(_tmp742,_tmp749) && !Cyc_Tcenv_region_outlives(te,_tmp742,_tmp749)) && !
Cyc_Tcutil_subtype(te,assume,_tmp742,_tmp749))
return 0;
# 2759
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp744,_tmp74B)){
struct _tuple0 _tmp115C;struct _tuple0 _tmp758=(_tmp115C.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp744),((_tmp115C.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp74B),_tmp115C)));struct _tuple0 _tmp759=_tmp758;struct Cyc_Absyn_Exp*_tmp75D;struct Cyc_Absyn_Exp*_tmp75F;_LL494:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp75A=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp759.f1;if(_tmp75A->tag != 0)goto _LL496;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp75B=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp759.f2;if(_tmp75B->tag != 0)goto _LL496;};_LL495:
 goto _LL493;_LL496:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp75C=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp759.f1;if(_tmp75C->tag != 1)goto _LL498;else{_tmp75D=_tmp75C->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp75E=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp759.f2;if(_tmp75E->tag != 1)goto _LL498;else{_tmp75F=_tmp75E->f1;}};_LL497:
# 2764
 if(!Cyc_Evexp_lte_const_exp(_tmp75F,_tmp75D))
return 0;
goto _LL493;_LL498:;_LL499:
 return 0;_LL493:;}
# 2772
if(!_tmp748.real_const  && _tmp741.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp747)))
return 0;}{
# 2778
int _tmp761=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp74B,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp74C);
# 2784
struct _tuple0*_tmp115F;struct Cyc_List_List*_tmp115E;return(_tmp761  && Cyc_Tcutil_ptrsubtype(te,((_tmp115E=_cycalloc(sizeof(*_tmp115E)),((_tmp115E->hd=((_tmp115F=_cycalloc(sizeof(*_tmp115F)),((_tmp115F->f1=t1,((_tmp115F->f2=t2,_tmp115F)))))),((_tmp115E->tl=assume,_tmp115E)))))),_tmp740,_tmp747) || Cyc_Tcutil_unify(_tmp740,_tmp747)) || Cyc_Tcutil_isomorphic(_tmp740,_tmp747);};_LL48D:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp74D=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp73E.f1;if(_tmp74D->tag != 4)goto _LL48F;else{if((((_tmp74D->f1).field_info).KnownDatatypefield).tag != 2)goto _LL48F;_tmp74E=((struct _tuple3)(((_tmp74D->f1).field_info).KnownDatatypefield).val).f1;_tmp74F=((struct _tuple3)(((_tmp74D->f1).field_info).KnownDatatypefield).val).f2;_tmp750=(_tmp74D->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp751=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp73E.f2;if(_tmp751->tag != 3)goto _LL48F;else{if((((_tmp751->f1).datatype_info).KnownDatatype).tag != 2)goto _LL48F;_tmp752=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp751->f1).datatype_info).KnownDatatype).val);_tmp753=(_tmp751->f1).targs;}};_LL48E:
# 2790
 if(_tmp74E != _tmp752  && Cyc_Absyn_qvar_cmp(_tmp74E->name,_tmp752->name)!= 0)return 0;
# 2792
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp750)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp753))return 0;
for(0;_tmp750 != 0;(_tmp750=_tmp750->tl,_tmp753=_tmp753->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp750->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp753))->hd))return 0;}
return 1;_LL48F:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp754=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp73E.f1;if(_tmp754->tag != 9)goto _LL491;else{_tmp755=_tmp754->f1;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp756=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp73E.f2;if(_tmp756->tag != 9)goto _LL491;else{_tmp757=_tmp756->f1;}};_LL490:
# 2799
 if(_tmp755.tvars != 0  || _tmp757.tvars != 0){
struct Cyc_List_List*_tmp764=_tmp755.tvars;
struct Cyc_List_List*_tmp765=_tmp757.tvars;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp764)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp765))return 0;{
struct _RegionHandle _tmp766=_new_region("r");struct _RegionHandle*r=& _tmp766;_push_region(r);
{struct Cyc_List_List*inst=0;
while(_tmp764 != 0){
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp764->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp765))->hd)->kind)){int _tmp767=0;_npop_handler(0);return _tmp767;}
{struct _tuple16*_tmp1169;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1168;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1167;struct Cyc_List_List*_tmp1166;inst=((_tmp1166=_region_malloc(r,sizeof(*_tmp1166)),((_tmp1166->hd=((_tmp1169=_region_malloc(r,sizeof(*_tmp1169)),((_tmp1169->f1=(struct Cyc_Absyn_Tvar*)_tmp765->hd,((_tmp1169->f2=(void*)((_tmp1167=_cycalloc(sizeof(*_tmp1167)),((_tmp1167[0]=((_tmp1168.tag=2,((_tmp1168.f1=(struct Cyc_Absyn_Tvar*)_tmp764->hd,_tmp1168)))),_tmp1167)))),_tmp1169)))))),((_tmp1166->tl=inst,_tmp1166))))));}
_tmp764=_tmp764->tl;
_tmp765=_tmp765->tl;}
# 2811
if(inst != 0){
_tmp755.tvars=0;
_tmp757.tvars=0;{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp116F;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp116E;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp116C;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp116B;int _tmp770=Cyc_Tcutil_subtype(te,assume,(void*)((_tmp116B=_cycalloc(sizeof(*_tmp116B)),((_tmp116B[0]=((_tmp116C.tag=9,((_tmp116C.f1=_tmp755,_tmp116C)))),_tmp116B)))),(void*)((_tmp116E=_cycalloc(sizeof(*_tmp116E)),((_tmp116E[0]=((_tmp116F.tag=9,((_tmp116F.f1=_tmp757,_tmp116F)))),_tmp116E)))));_npop_handler(0);return _tmp770;};}}
# 2804
;_pop_region(r);};}
# 2819
if(!Cyc_Tcutil_subtype(te,assume,_tmp755.ret_typ,_tmp757.ret_typ))return 0;{
struct Cyc_List_List*_tmp771=_tmp755.args;
struct Cyc_List_List*_tmp772=_tmp757.args;
# 2824
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp771)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp772))return 0;
# 2826
for(0;_tmp771 != 0;(_tmp771=_tmp771->tl,_tmp772=_tmp772->tl)){
struct _tuple10 _tmp773=*((struct _tuple10*)_tmp771->hd);struct Cyc_Absyn_Tqual _tmp775;void*_tmp776;struct _tuple10 _tmp774=_tmp773;_tmp775=_tmp774.f2;_tmp776=_tmp774.f3;{
struct _tuple10 _tmp777=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp772))->hd);struct Cyc_Absyn_Tqual _tmp779;void*_tmp77A;struct _tuple10 _tmp778=_tmp777;_tmp779=_tmp778.f2;_tmp77A=_tmp778.f3;
# 2830
if(_tmp779.real_const  && !_tmp775.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp77A,_tmp776))
return 0;};}
# 2834
if(_tmp755.c_varargs != _tmp757.c_varargs)return 0;
if(_tmp755.cyc_varargs != 0  && _tmp757.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp77B=*_tmp755.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp77C=*_tmp757.cyc_varargs;
# 2839
if((_tmp77C.tq).real_const  && !(_tmp77B.tq).real_const  || !
Cyc_Tcutil_subtype(te,assume,_tmp77C.type,_tmp77B.type))
return 0;}else{
if(_tmp755.cyc_varargs != 0  || _tmp757.cyc_varargs != 0)return 0;}
# 2844
if(!Cyc_Tcutil_subset_effect(1,(void*)_check_null(_tmp755.effect),(void*)_check_null(_tmp757.effect)))return 0;
# 2846
if(!Cyc_Tcutil_sub_rgnpo(_tmp755.rgn_po,_tmp757.rgn_po))return 0;
# 2848
if(!Cyc_Tcutil_sub_attributes(_tmp755.attributes,_tmp757.attributes))return 0;
# 2850
if(!Cyc_Tcutil_check_logical_implication(_tmp757.requires_relns,_tmp755.requires_relns))
return 0;
# 2853
if(!Cyc_Tcutil_check_logical_implication(_tmp755.ensures_relns,_tmp757.ensures_relns))
return 0;
# 2856
return 1;};_LL491:;_LL492:
 return 0;_LL48A:;};}
# 2868 "tcutil.cyc"
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2870
struct _RegionHandle _tmp77E=_new_region("temp");struct _RegionHandle*temp=& _tmp77E;_push_region(temp);
{struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(temp,1,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(temp,1,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0){int _tmp77F=0;_npop_handler(0);return _tmp77F;}{
struct _tuple12*_tmp780=(struct _tuple12*)tqs1->hd;struct Cyc_Absyn_Tqual _tmp782;void*_tmp783;struct _tuple12*_tmp781=_tmp780;_tmp782=_tmp781->f1;_tmp783=_tmp781->f2;{
struct _tuple12*_tmp784=(struct _tuple12*)tqs2->hd;struct Cyc_Absyn_Tqual _tmp786;void*_tmp787;struct _tuple12*_tmp785=_tmp784;_tmp786=_tmp785->f1;_tmp787=_tmp785->f2;
# 2878
if(_tmp782.real_const  && !_tmp786.real_const){int _tmp788=0;_npop_handler(0);return _tmp788;}
# 2880
if((_tmp786.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp787))) && 
Cyc_Tcutil_subtype(te,assume,_tmp783,_tmp787))
# 2883
continue;
# 2885
if(Cyc_Tcutil_unify(_tmp783,_tmp787))
# 2887
continue;
# 2889
if(Cyc_Tcutil_isomorphic(_tmp783,_tmp787))
# 2891
continue;{
# 2894
int _tmp789=0;_npop_handler(0);return _tmp789;};};};}{
# 2896
int _tmp78A=1;_npop_handler(0);return _tmp78A;};}
# 2871
;_pop_region(temp);}
# 2900
static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp78B=Cyc_Tcutil_compress(t);void*_tmp78C=_tmp78B;_LL49B: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp78D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp78C;if(_tmp78D->tag != 6)goto _LL49D;else{if(_tmp78D->f2 != Cyc_Absyn_Char_sz)goto _LL49D;}}_LL49C:
 return 1;_LL49D:;_LL49E:
 return 0;_LL49A:;}
# 2909
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
# 2915
if(t2 == (void*)& Cyc_Absyn_VoidType_val)
return Cyc_Absyn_No_coercion;
{void*_tmp78E=t2;_LL4A0: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp78F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp78E;if(_tmp78F->tag != 6)goto _LL4A2;else{if(_tmp78F->f2 != Cyc_Absyn_Int_sz)goto _LL4A2;}}_LL4A1:
# 2919
 goto _LL4A3;_LL4A2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp790=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp78E;if(_tmp790->tag != 6)goto _LL4A4;else{if(_tmp790->f2 != Cyc_Absyn_Long_sz)goto _LL4A4;}}_LL4A3:
# 2921
 if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL49F;_LL4A4:;_LL4A5:
 goto _LL49F;_LL49F:;}{
# 2925
void*_tmp791=t1;void*_tmp793;struct Cyc_Absyn_Tqual _tmp794;void*_tmp795;union Cyc_Absyn_Constraint*_tmp796;union Cyc_Absyn_Constraint*_tmp797;union Cyc_Absyn_Constraint*_tmp798;void*_tmp79A;struct Cyc_Absyn_Tqual _tmp79B;struct Cyc_Absyn_Exp*_tmp79C;union Cyc_Absyn_Constraint*_tmp79D;struct Cyc_Absyn_Enumdecl*_tmp79F;void*_tmp7A3;_LL4A7: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp792=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp791;if(_tmp792->tag != 5)goto _LL4A9;else{_tmp793=(_tmp792->f1).elt_typ;_tmp794=(_tmp792->f1).elt_tq;_tmp795=((_tmp792->f1).ptr_atts).rgn;_tmp796=((_tmp792->f1).ptr_atts).nullable;_tmp797=((_tmp792->f1).ptr_atts).bounds;_tmp798=((_tmp792->f1).ptr_atts).zero_term;}}_LL4A8:
# 2933
{void*_tmp7A4=t2;void*_tmp7A6;struct Cyc_Absyn_Tqual _tmp7A7;void*_tmp7A8;union Cyc_Absyn_Constraint*_tmp7A9;union Cyc_Absyn_Constraint*_tmp7AA;union Cyc_Absyn_Constraint*_tmp7AB;_LL4B6: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp7A5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7A4;if(_tmp7A5->tag != 5)goto _LL4B8;else{_tmp7A6=(_tmp7A5->f1).elt_typ;_tmp7A7=(_tmp7A5->f1).elt_tq;_tmp7A8=((_tmp7A5->f1).ptr_atts).rgn;_tmp7A9=((_tmp7A5->f1).ptr_atts).nullable;_tmp7AA=((_tmp7A5->f1).ptr_atts).bounds;_tmp7AB=((_tmp7A5->f1).ptr_atts).zero_term;}}_LL4B7: {
# 2937
enum Cyc_Absyn_Coercion coercion=Cyc_Absyn_Other_coercion;
struct _tuple0*_tmp1172;struct Cyc_List_List*_tmp1171;struct Cyc_List_List*_tmp7AC=(_tmp1171=_cycalloc(sizeof(*_tmp1171)),((_tmp1171->hd=((_tmp1172=_cycalloc(sizeof(*_tmp1172)),((_tmp1172->f1=t1,((_tmp1172->f2=t2,_tmp1172)))))),((_tmp1171->tl=0,_tmp1171)))));
int _tmp7AD=_tmp7A7.real_const  || !_tmp794.real_const;
# 2951 "tcutil.cyc"
int _tmp7AE=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp7AA,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp7AB);
# 2955
int _tmp7AF=_tmp7AD  && (
((_tmp7AE  && Cyc_Tcutil_ptrsubtype(te,_tmp7AC,_tmp793,_tmp7A6) || 
Cyc_Tcutil_unify(_tmp793,_tmp7A6)) || Cyc_Tcutil_isomorphic(_tmp793,_tmp7A6)) || Cyc_Tcutil_unify(_tmp7A6,(void*)& Cyc_Absyn_VoidType_val));
# 2959
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp798,_tmp7AB) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp7AB);
# 2963
int _tmp7B0=_tmp7AF?0:((Cyc_Tcutil_bits_only(_tmp793) && Cyc_Tcutil_is_char_type(_tmp7A6)) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp7AB)) && (
_tmp7A7.real_const  || !_tmp794.real_const);
int bounds_ok=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp797,_tmp7AA);
if(!bounds_ok  && !_tmp7B0){
struct _tuple0 _tmp1173;struct _tuple0 _tmp7B1=(_tmp1173.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp797),((_tmp1173.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp7AA),_tmp1173)));struct _tuple0 _tmp7B2=_tmp7B1;struct Cyc_Absyn_Exp*_tmp7B4;struct Cyc_Absyn_Exp*_tmp7B6;_LL4BB:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp7B3=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp7B2.f1;if(_tmp7B3->tag != 1)goto _LL4BD;else{_tmp7B4=_tmp7B3->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp7B5=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp7B2.f2;if(_tmp7B5->tag != 1)goto _LL4BD;else{_tmp7B6=_tmp7B5->f1;}};_LL4BC:
# 2973
 if(Cyc_Evexp_lte_const_exp(_tmp7B6,_tmp7B4))
bounds_ok=1;
goto _LL4BA;_LL4BD:;_LL4BE:
# 2978
 bounds_ok=1;goto _LL4BA;_LL4BA:;}
# 2980
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp796) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp7A9))
coercion=Cyc_Absyn_NonNull_to_Null;
# 2985
if(((bounds_ok  && zeroterm_ok) && (_tmp7AF  || _tmp7B0)) && (
Cyc_Tcutil_unify(_tmp795,_tmp7A8) || Cyc_Tcenv_region_outlives(te,_tmp795,_tmp7A8)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};}_LL4B8:;_LL4B9:
 goto _LL4B5;_LL4B5:;}
# 2991
return Cyc_Absyn_Unknown_coercion;_LL4A9: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp799=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp791;if(_tmp799->tag != 8)goto _LL4AB;else{_tmp79A=(_tmp799->f1).elt_type;_tmp79B=(_tmp799->f1).tq;_tmp79C=(_tmp799->f1).num_elts;_tmp79D=(_tmp799->f1).zero_term;}}_LL4AA:
# 2993
{void*_tmp7BA=t2;void*_tmp7BC;struct Cyc_Absyn_Tqual _tmp7BD;struct Cyc_Absyn_Exp*_tmp7BE;union Cyc_Absyn_Constraint*_tmp7BF;_LL4C0: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp7BB=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp7BA;if(_tmp7BB->tag != 8)goto _LL4C2;else{_tmp7BC=(_tmp7BB->f1).elt_type;_tmp7BD=(_tmp7BB->f1).tq;_tmp7BE=(_tmp7BB->f1).num_elts;_tmp7BF=(_tmp7BB->f1).zero_term;}}_LL4C1: {
# 2995
int okay;
okay=
(((_tmp79C != 0  && _tmp7BE != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp79D,_tmp7BF)) && 
Cyc_Evexp_lte_const_exp(_tmp7BE,_tmp79C)) && 
Cyc_Evexp_lte_const_exp(_tmp79C,_tmp7BE);
return
# 3002
(okay  && Cyc_Tcutil_unify(_tmp79A,_tmp7BC)) && (!_tmp79B.real_const  || _tmp7BD.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}_LL4C2:;_LL4C3:
# 3004
 return Cyc_Absyn_Unknown_coercion;_LL4BF:;}
# 3006
return Cyc_Absyn_Unknown_coercion;_LL4AB: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp79E=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp791;if(_tmp79E->tag != 13)goto _LL4AD;else{_tmp79F=_tmp79E->f2;}}_LL4AC:
# 3010
{void*_tmp7C0=t2;struct Cyc_Absyn_Enumdecl*_tmp7C2;_LL4C5: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp7C1=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp7C0;if(_tmp7C1->tag != 13)goto _LL4C7;else{_tmp7C2=_tmp7C1->f2;}}_LL4C6:
# 3012
 if((_tmp79F->fields != 0  && _tmp7C2->fields != 0) && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp79F->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp7C2->fields))->v))
return Cyc_Absyn_Other_coercion;
goto _LL4C4;_LL4C7:;_LL4C8:
 goto _LL4C4;_LL4C4:;}
# 3018
goto _LL4AE;_LL4AD: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7A0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp791;if(_tmp7A0->tag != 6)goto _LL4AF;}_LL4AE:
 goto _LL4B0;_LL4AF: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7A1=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp791;if(_tmp7A1->tag != 7)goto _LL4B1;}_LL4B0:
 return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;_LL4B1: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp7A2=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp791;if(_tmp7A2->tag != 15)goto _LL4B3;else{_tmp7A3=(void*)_tmp7A2->f1;}}_LL4B2:
# 3023
{void*_tmp7C3=t2;void*_tmp7C5;_LL4CA: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp7C4=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp7C3;if(_tmp7C4->tag != 15)goto _LL4CC;else{_tmp7C5=(void*)_tmp7C4->f1;}}_LL4CB:
# 3025
 if(Cyc_Tcenv_region_outlives(te,_tmp7A3,_tmp7C5))return Cyc_Absyn_No_coercion;
goto _LL4C9;_LL4CC:;_LL4CD:
 goto _LL4C9;_LL4C9:;}
# 3029
return Cyc_Absyn_Unknown_coercion;_LL4B3:;_LL4B4:
 return Cyc_Absyn_Unknown_coercion;_LL4A6:;};}
# 3035
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),t)){
struct Cyc_Absyn_Exp*_tmp7C6=Cyc_Absyn_copy_exp(e);
{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp1176;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp1175;e->r=(void*)((_tmp1175=_cycalloc(sizeof(*_tmp1175)),((_tmp1175[0]=((_tmp1176.tag=13,((_tmp1176.f1=t,((_tmp1176.f2=_tmp7C6,((_tmp1176.f3=0,((_tmp1176.f4=c,_tmp1176)))))))))),_tmp1175))));}
e->topt=t;}}
# 3043
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp7C9=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp7CA=_tmp7C9;_LL4CF: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7CB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7CA;if(_tmp7CB->tag != 6)goto _LL4D1;}_LL4D0:
 goto _LL4D2;_LL4D1: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp7CC=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp7CA;if(_tmp7CC->tag != 13)goto _LL4D3;}_LL4D2:
 goto _LL4D4;_LL4D3: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp7CD=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp7CA;if(_tmp7CD->tag != 14)goto _LL4D5;}_LL4D4:
 goto _LL4D6;_LL4D5: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp7CE=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp7CA;if(_tmp7CE->tag != 19)goto _LL4D7;}_LL4D6:
 return 1;_LL4D7: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp7CF=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp7CA;if(_tmp7CF->tag != 1)goto _LL4D9;}_LL4D8:
 return Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ);_LL4D9:;_LL4DA:
 return 0;_LL4CE:;}
# 3054
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp7D0=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp7D1=_tmp7D0;_LL4DC: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7D2=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp7D1;if(_tmp7D2->tag != 7)goto _LL4DE;}_LL4DD:
 return 1;_LL4DE:;_LL4DF:
 return 0;_LL4DB:;};}
# 3063
int Cyc_Tcutil_is_function_type(void*t){
void*_tmp7D3=Cyc_Tcutil_compress(t);void*_tmp7D4=_tmp7D3;_LL4E1: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp7D5=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp7D4;if(_tmp7D5->tag != 9)goto _LL4E3;}_LL4E2:
 return 1;_LL4E3:;_LL4E4:
 return 0;_LL4E0:;}
# 3070
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
struct _tuple0 _tmp1177;struct _tuple0 _tmp7D6=(_tmp1177.f1=t1,((_tmp1177.f2=t2,_tmp1177)));struct _tuple0 _tmp7D7=_tmp7D6;int _tmp7D9;int _tmp7DB;_LL4E6:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7D8=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp7D7.f1;if(_tmp7D8->tag != 7)goto _LL4E8;else{_tmp7D9=_tmp7D8->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7DA=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp7D7.f2;if(_tmp7DA->tag != 7)goto _LL4E8;else{_tmp7DB=_tmp7DA->f1;}};_LL4E7:
# 3073
 if(_tmp7D9 != 0  && _tmp7D9 != 1)return t1;else{
if(_tmp7DB != 0  && _tmp7DB != 1)return t2;else{
if(_tmp7D9 >= _tmp7DB)return t1;else{
return t2;}}}_LL4E8: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7DC=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp7D7.f1;if(_tmp7DC->tag != 7)goto _LL4EA;}_LL4E9:
 return t1;_LL4EA: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7DD=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp7D7.f2;if(_tmp7DD->tag != 7)goto _LL4EC;}_LL4EB:
 return t2;_LL4EC: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7DE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7D7.f1;if(_tmp7DE->tag != 6)goto _LL4EE;else{if(_tmp7DE->f1 != Cyc_Absyn_Unsigned)goto _LL4EE;if(_tmp7DE->f2 != Cyc_Absyn_LongLong_sz)goto _LL4EE;}}_LL4ED:
 goto _LL4EF;_LL4EE: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7DF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7D7.f2;if(_tmp7DF->tag != 6)goto _LL4F0;else{if(_tmp7DF->f1 != Cyc_Absyn_Unsigned)goto _LL4F0;if(_tmp7DF->f2 != Cyc_Absyn_LongLong_sz)goto _LL4F0;}}_LL4EF:
 return Cyc_Absyn_ulonglong_typ;_LL4F0: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7E0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7D7.f1;if(_tmp7E0->tag != 6)goto _LL4F2;else{if(_tmp7E0->f2 != Cyc_Absyn_LongLong_sz)goto _LL4F2;}}_LL4F1:
 goto _LL4F3;_LL4F2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7E1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7D7.f2;if(_tmp7E1->tag != 6)goto _LL4F4;else{if(_tmp7E1->f2 != Cyc_Absyn_LongLong_sz)goto _LL4F4;}}_LL4F3:
 return Cyc_Absyn_slonglong_typ;_LL4F4: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7E2=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7D7.f1;if(_tmp7E2->tag != 6)goto _LL4F6;else{if(_tmp7E2->f1 != Cyc_Absyn_Unsigned)goto _LL4F6;if(_tmp7E2->f2 != Cyc_Absyn_Long_sz)goto _LL4F6;}}_LL4F5:
 goto _LL4F7;_LL4F6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7E3=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7D7.f2;if(_tmp7E3->tag != 6)goto _LL4F8;else{if(_tmp7E3->f1 != Cyc_Absyn_Unsigned)goto _LL4F8;if(_tmp7E3->f2 != Cyc_Absyn_Long_sz)goto _LL4F8;}}_LL4F7:
 return Cyc_Absyn_ulong_typ;_LL4F8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7E4=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7D7.f1;if(_tmp7E4->tag != 6)goto _LL4FA;else{if(_tmp7E4->f1 != Cyc_Absyn_Unsigned)goto _LL4FA;if(_tmp7E4->f2 != Cyc_Absyn_Int_sz)goto _LL4FA;}}_LL4F9:
# 3086
 goto _LL4FB;_LL4FA: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7E5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7D7.f2;if(_tmp7E5->tag != 6)goto _LL4FC;else{if(_tmp7E5->f1 != Cyc_Absyn_Unsigned)goto _LL4FC;if(_tmp7E5->f2 != Cyc_Absyn_Int_sz)goto _LL4FC;}}_LL4FB:
 return Cyc_Absyn_uint_typ;_LL4FC: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7E6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7D7.f1;if(_tmp7E6->tag != 6)goto _LL4FE;else{if(_tmp7E6->f2 != Cyc_Absyn_Long_sz)goto _LL4FE;}}_LL4FD:
 goto _LL4FF;_LL4FE: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7E7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7D7.f2;if(_tmp7E7->tag != 6)goto _LL500;else{if(_tmp7E7->f2 != Cyc_Absyn_Long_sz)goto _LL500;}}_LL4FF:
 return Cyc_Absyn_slong_typ;_LL500:;_LL501:
 return Cyc_Absyn_sint_typ;_LL4E5:;}
# 3095
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_tmp7E9=e->r;void*_tmp7EA=_tmp7E9;_LL503: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp7EB=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp7EA;if(_tmp7EB->tag != 3)goto _LL505;else{if(_tmp7EB->f2 != 0)goto _LL505;}}_LL504:
{const char*_tmp117A;void*_tmp1179;(_tmp1179=0,Cyc_Tcutil_warn(e->loc,((_tmp117A="assignment in test",_tag_dyneither(_tmp117A,sizeof(char),19))),_tag_dyneither(_tmp1179,sizeof(void*),0)));}goto _LL502;_LL505:;_LL506:
 goto _LL502;_LL502:;}
# 3109 "tcutil.cyc"
static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){
c1=Cyc_Absyn_compress_kb(c1);
c2=Cyc_Absyn_compress_kb(c2);
if(c1 == c2)return 1;{
struct _tuple0 _tmp117B;struct _tuple0 _tmp7EE=(_tmp117B.f1=c1,((_tmp117B.f2=c2,_tmp117B)));struct _tuple0 _tmp7EF=_tmp7EE;struct Cyc_Absyn_Kind*_tmp7F1;struct Cyc_Absyn_Kind*_tmp7F3;struct Cyc_Core_Opt**_tmp7F5;struct Cyc_Core_Opt**_tmp7F7;struct Cyc_Core_Opt**_tmp7F9;struct Cyc_Absyn_Kind*_tmp7FA;struct Cyc_Absyn_Kind*_tmp7FC;struct Cyc_Absyn_Kind*_tmp7FE;struct Cyc_Core_Opt**_tmp800;struct Cyc_Absyn_Kind*_tmp801;struct Cyc_Core_Opt**_tmp803;struct Cyc_Absyn_Kind*_tmp804;struct Cyc_Core_Opt**_tmp806;struct Cyc_Absyn_Kind*_tmp807;_LL508:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp7F0=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7EF.f1;if(_tmp7F0->tag != 0)goto _LL50A;else{_tmp7F1=_tmp7F0->f1;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp7F2=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7EF.f2;if(_tmp7F2->tag != 0)goto _LL50A;else{_tmp7F3=_tmp7F2->f1;}};_LL509:
 return _tmp7F1 == _tmp7F3;_LL50A: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp7F4=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7EF.f2;if(_tmp7F4->tag != 1)goto _LL50C;else{_tmp7F5=(struct Cyc_Core_Opt**)& _tmp7F4->f1;}}_LL50B:
{struct Cyc_Core_Opt*_tmp117C;*_tmp7F5=((_tmp117C=_cycalloc(sizeof(*_tmp117C)),((_tmp117C->v=c1,_tmp117C))));}return 1;_LL50C: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp7F6=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7EF.f1;if(_tmp7F6->tag != 1)goto _LL50E;else{_tmp7F7=(struct Cyc_Core_Opt**)& _tmp7F6->f1;}}_LL50D:
{struct Cyc_Core_Opt*_tmp117D;*_tmp7F7=((_tmp117D=_cycalloc(sizeof(*_tmp117D)),((_tmp117D->v=c2,_tmp117D))));}return 1;_LL50E:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp7F8=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7EF.f1;if(_tmp7F8->tag != 2)goto _LL510;else{_tmp7F9=(struct Cyc_Core_Opt**)& _tmp7F8->f1;_tmp7FA=_tmp7F8->f2;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp7FB=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7EF.f2;if(_tmp7FB->tag != 0)goto _LL510;else{_tmp7FC=_tmp7FB->f1;}};_LL50F:
# 3118
 if(Cyc_Tcutil_kind_leq(_tmp7FC,_tmp7FA)){
{struct Cyc_Core_Opt*_tmp117E;*_tmp7F9=((_tmp117E=_cycalloc(sizeof(*_tmp117E)),((_tmp117E->v=c2,_tmp117E))));}return 1;}else{
return 0;}_LL510:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp7FD=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7EF.f1;if(_tmp7FD->tag != 0)goto _LL512;else{_tmp7FE=_tmp7FD->f1;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp7FF=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7EF.f2;if(_tmp7FF->tag != 2)goto _LL512;else{_tmp800=(struct Cyc_Core_Opt**)& _tmp7FF->f1;_tmp801=_tmp7FF->f2;}};_LL511:
# 3122
 if(Cyc_Tcutil_kind_leq(_tmp7FE,_tmp801)){
{struct Cyc_Core_Opt*_tmp117F;*_tmp800=((_tmp117F=_cycalloc(sizeof(*_tmp117F)),((_tmp117F->v=c1,_tmp117F))));}return 1;}else{
return 0;}_LL512:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp802=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7EF.f1;if(_tmp802->tag != 2)goto _LL507;else{_tmp803=(struct Cyc_Core_Opt**)& _tmp802->f1;_tmp804=_tmp802->f2;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp805=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7EF.f2;if(_tmp805->tag != 2)goto _LL507;else{_tmp806=(struct Cyc_Core_Opt**)& _tmp805->f1;_tmp807=_tmp805->f2;}};_LL513:
# 3126
 if(Cyc_Tcutil_kind_leq(_tmp804,_tmp807)){
{struct Cyc_Core_Opt*_tmp1180;*_tmp806=((_tmp1180=_cycalloc(sizeof(*_tmp1180)),((_tmp1180->v=c1,_tmp1180))));}return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp807,_tmp804)){
{struct Cyc_Core_Opt*_tmp1181;*_tmp803=((_tmp1181=_cycalloc(sizeof(*_tmp1181)),((_tmp1181->v=c2,_tmp1181))));}return 1;}else{
return 0;}}_LL507:;};}
# 3135
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 3140
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
const char*_tmp1185;void*_tmp1184[1];struct Cyc_Int_pa_PrintArg_struct _tmp1183;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmp1183.tag=1,((_tmp1183.f1=(unsigned long)i,((_tmp1184[0]=& _tmp1183,Cyc_aprintf(((_tmp1185="#%d",_tag_dyneither(_tmp1185,sizeof(char),4))),_tag_dyneither(_tmp1184,sizeof(void*),1))))))));
struct _dyneither_ptr*_tmp1188;struct Cyc_Absyn_Tvar*_tmp1187;return(_tmp1187=_cycalloc(sizeof(*_tmp1187)),((_tmp1187->name=((_tmp1188=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp1188[0]=(struct _dyneither_ptr)s,_tmp1188)))),((_tmp1187->identity=- 1,((_tmp1187->kind=k,_tmp1187)))))));}
# 3147
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _dyneither_ptr _tmp814=*t->name;
return*((const char*)_check_dyneither_subscript(_tmp814,sizeof(char),0))== '#';}
# 3152
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
{const char*_tmp118C;void*_tmp118B[1];struct Cyc_String_pa_PrintArg_struct _tmp118A;(_tmp118A.tag=0,((_tmp118A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name),((_tmp118B[0]=& _tmp118A,Cyc_printf(((_tmp118C="%s",_tag_dyneither(_tmp118C,sizeof(char),3))),_tag_dyneither(_tmp118B,sizeof(void*),1)))))));}
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
const char*_tmp118D;struct _dyneither_ptr _tmp818=Cyc_strconcat(((_tmp118D="`",_tag_dyneither(_tmp118D,sizeof(char),2))),(struct _dyneither_ptr)*t->name);
{char _tmp1190;char _tmp118F;struct _dyneither_ptr _tmp118E;(_tmp118E=_dyneither_ptr_plus(_tmp818,sizeof(char),1),((_tmp118F=*((char*)_check_dyneither_subscript(_tmp118E,sizeof(char),0)),((_tmp1190='t',((_get_dyneither_size(_tmp118E,sizeof(char))== 1  && (_tmp118F == '\000'  && _tmp1190 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp118E.curr)=_tmp1190)))))));}{
struct _dyneither_ptr*_tmp1191;t->name=((_tmp1191=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp1191[0]=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp818),_tmp1191))));};};}
# 3161
static struct _tuple10*Cyc_Tcutil_fndecl2typ_f(struct _tuple10*x){
# 3163
struct _tuple10*_tmp1192;return(_tmp1192=_cycalloc(sizeof(*_tmp1192)),((_tmp1192->f1=(*x).f1,((_tmp1192->f2=(*x).f2,((_tmp1192->f3=(*x).f3,_tmp1192)))))));}
# 3166
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_typ == 0){
# 3173
struct Cyc_List_List*_tmp81F=0;
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
struct Cyc_List_List*_tmp1193;_tmp81F=((_tmp1193=_cycalloc(sizeof(*_tmp1193)),((_tmp1193->hd=(void*)atts->hd,((_tmp1193->tl=_tmp81F,_tmp1193))))));}}}{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1199;struct Cyc_Absyn_FnInfo _tmp1198;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1197;return(void*)((_tmp1197=_cycalloc(sizeof(*_tmp1197)),((_tmp1197[0]=((_tmp1199.tag=9,((_tmp1199.f1=((_tmp1198.tvars=fd->tvs,((_tmp1198.effect=fd->effect,((_tmp1198.ret_tqual=fd->ret_tqual,((_tmp1198.ret_typ=fd->ret_type,((_tmp1198.args=
((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args),((_tmp1198.c_varargs=fd->c_varargs,((_tmp1198.cyc_varargs=fd->cyc_varargs,((_tmp1198.rgn_po=fd->rgn_po,((_tmp1198.attributes=_tmp81F,((_tmp1198.requires_clause=fd->requires_clause,((_tmp1198.requires_relns=fd->requires_relns,((_tmp1198.ensures_clause=fd->ensures_clause,((_tmp1198.ensures_relns=fd->ensures_relns,_tmp1198)))))))))))))))))))))))))),_tmp1199)))),_tmp1197))));};}
# 3186
return(void*)_check_null(fd->cached_typ);}
# 3192
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 3194
union Cyc_Relations_RelnOp _tmp824=*rop;union Cyc_Relations_RelnOp _tmp825=_tmp824;struct Cyc_Absyn_Vardecl*_tmp826;struct Cyc_Absyn_Vardecl*_tmp827;_LL515: if((_tmp825.RVar).tag != 2)goto _LL517;_tmp826=(struct Cyc_Absyn_Vardecl*)(_tmp825.RVar).val;_LL516: {
# 3196
struct _tuple2 _tmp828=*_tmp826->name;union Cyc_Absyn_Nmspace _tmp82A;struct _dyneither_ptr*_tmp82B;struct _tuple2 _tmp829=_tmp828;_tmp82A=_tmp829.f1;_tmp82B=_tmp829.f2;
if(!((int)((_tmp82A.Loc_n).tag == 4)))goto _LL514;
{const char*_tmp119A;if(Cyc_strcmp((struct _dyneither_ptr)*_tmp82B,((_tmp119A="return_value",_tag_dyneither(_tmp119A,sizeof(char),13))))== 0){
*rop=Cyc_Relations_RReturn();
goto _LL514;}}{
# 3202
unsigned int c=0;
{struct Cyc_List_List*_tmp82D=args;for(0;_tmp82D != 0;(_tmp82D=_tmp82D->tl,c ++)){
struct _tuple10*_tmp82E=(struct _tuple10*)_tmp82D->hd;struct _dyneither_ptr*_tmp830;struct _tuple10*_tmp82F=_tmp82E;_tmp830=_tmp82F->f1;
if(_tmp830 != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp82B,(struct _dyneither_ptr)*_tmp830)== 0){
*rop=Cyc_Relations_RParam(c);
break;}}}}
# 3212
goto _LL514;};}_LL517: if((_tmp825.RNumelts).tag != 3)goto _LL519;_tmp827=(struct Cyc_Absyn_Vardecl*)(_tmp825.RNumelts).val;_LL518: {
# 3214
struct _tuple2 _tmp831=*_tmp827->name;union Cyc_Absyn_Nmspace _tmp833;struct _dyneither_ptr*_tmp834;struct _tuple2 _tmp832=_tmp831;_tmp833=_tmp832.f1;_tmp834=_tmp832.f2;
if(!((int)((_tmp833.Loc_n).tag == 4)))goto _LL514;{
unsigned int c=0;
{struct Cyc_List_List*_tmp835=args;for(0;_tmp835 != 0;(_tmp835=_tmp835->tl,c ++)){
struct _tuple10*_tmp836=(struct _tuple10*)_tmp835->hd;struct _dyneither_ptr*_tmp838;struct _tuple10*_tmp837=_tmp836;_tmp838=_tmp837->f1;
if(_tmp838 != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp834,(struct _dyneither_ptr)*_tmp838)== 0){
*rop=Cyc_Relations_RParamNumelts(c);
break;}}}}
# 3226
goto _LL514;};}_LL519:;_LL51A:
 goto _LL514;_LL514:;}
# 3231
static void Cyc_Tcutil_replace_rops(struct Cyc_List_List*args,struct Cyc_Relations_Reln*r){
# 3233
Cyc_Tcutil_replace_rop(args,& r->rop1);
Cyc_Tcutil_replace_rop(args,& r->rop2);}
# 3237
static struct Cyc_List_List*Cyc_Tcutil_extract_relns(struct Cyc_List_List*args,struct Cyc_Absyn_Exp*e){
# 3239
if(e == 0)return 0;{
struct Cyc_List_List*_tmp839=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,_tmp839);
return _tmp839;};}struct _tuple25{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 3246
static void*Cyc_Tcutil_fst_fdarg(struct _tuple25*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct Cyc_Absyn_Tqual _tmp83B;void*_tmp83C;struct _tuple12*_tmp83A=pr;_tmp83B=_tmp83A->f1;_tmp83C=_tmp83A->f2;
if(_tmp83C == t)return pr;else{
struct _tuple12*_tmp119B;return(_tmp119B=_cycalloc(sizeof(*_tmp119B)),((_tmp119B->f1=_tmp83B,((_tmp119B->f2=t,_tmp119B)))));}}struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 3253
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
# 3255
struct _tuple26*_tmp119E;struct _tuple27*_tmp119D;return(_tmp119D=_region_malloc(rgn,sizeof(*_tmp119D)),((_tmp119D->f1=((_tmp119E=_region_malloc(rgn,sizeof(*_tmp119E)),((_tmp119E->f1=(*y).f1,((_tmp119E->f2=(*y).f2,_tmp119E)))))),((_tmp119D->f2=(*y).f3,_tmp119D)))));}
# 3257
static struct _tuple10*Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){
# 3259
struct _tuple10 _tmp840=*orig_arg;struct _dyneither_ptr*_tmp842;struct Cyc_Absyn_Tqual _tmp843;void*_tmp844;struct _tuple10 _tmp841=_tmp840;_tmp842=_tmp841.f1;_tmp843=_tmp841.f2;_tmp844=_tmp841.f3;
if(t == _tmp844)return orig_arg;{
struct _tuple10*_tmp119F;return(_tmp119F=_cycalloc(sizeof(*_tmp119F)),((_tmp119F->f1=_tmp842,((_tmp119F->f2=_tmp843,((_tmp119F->f3=t,_tmp119F)))))));};}
# 3263
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){
return f->type;}
# 3266
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 3271
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 3273
struct Cyc_Absyn_Exp*_tmp11A0;return(_tmp11A0=_cycalloc(sizeof(*_tmp11A0)),((_tmp11A0->topt=old->topt,((_tmp11A0->r=r,((_tmp11A0->loc=old->loc,((_tmp11A0->annot=old->annot,_tmp11A0)))))))));}
# 3278
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp847=e->r;void*_tmp848=_tmp847;enum Cyc_Absyn_Primop _tmp84E;struct Cyc_List_List*_tmp84F;struct Cyc_Absyn_Exp*_tmp851;struct Cyc_Absyn_Exp*_tmp852;struct Cyc_Absyn_Exp*_tmp853;struct Cyc_Absyn_Exp*_tmp855;struct Cyc_Absyn_Exp*_tmp856;struct Cyc_Absyn_Exp*_tmp858;struct Cyc_Absyn_Exp*_tmp859;struct Cyc_Absyn_Exp*_tmp85B;struct Cyc_Absyn_Exp*_tmp85C;void*_tmp85E;struct Cyc_Absyn_Exp*_tmp85F;int _tmp860;enum Cyc_Absyn_Coercion _tmp861;void*_tmp863;struct Cyc_Absyn_Exp*_tmp865;void*_tmp867;struct Cyc_List_List*_tmp868;void*_tmp86A;_LL51C: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp849=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp848;if(_tmp849->tag != 0)goto _LL51E;}_LL51D:
 goto _LL51F;_LL51E: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp84A=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp848;if(_tmp84A->tag != 31)goto _LL520;}_LL51F:
 goto _LL521;_LL520: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp84B=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp848;if(_tmp84B->tag != 32)goto _LL522;}_LL521:
 goto _LL523;_LL522: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp84C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp848;if(_tmp84C->tag != 1)goto _LL524;}_LL523:
 return e;_LL524: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp84D=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp848;if(_tmp84D->tag != 2)goto _LL526;else{_tmp84E=_tmp84D->f1;_tmp84F=_tmp84D->f2;}}_LL525:
# 3286
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp84F)== 1){
struct Cyc_Absyn_Exp*_tmp86B=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp84F))->hd;
struct Cyc_Absyn_Exp*_tmp86C=Cyc_Tcutil_rsubsexp(r,inst,_tmp86B);
if(_tmp86C == _tmp86B)return e;{
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp11A6;struct Cyc_Absyn_Exp*_tmp11A5[1];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp11A4;return Cyc_Tcutil_copye(e,(void*)((_tmp11A4=_cycalloc(sizeof(*_tmp11A4)),((_tmp11A4[0]=((_tmp11A6.tag=2,((_tmp11A6.f1=_tmp84E,((_tmp11A6.f2=((_tmp11A5[0]=_tmp86C,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp11A5,sizeof(struct Cyc_Absyn_Exp*),1)))),_tmp11A6)))))),_tmp11A4)))));};}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp84F)== 2){
struct Cyc_Absyn_Exp*_tmp870=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp84F))->hd;
struct Cyc_Absyn_Exp*_tmp871=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp84F->tl))->hd;
struct Cyc_Absyn_Exp*_tmp872=Cyc_Tcutil_rsubsexp(r,inst,_tmp870);
struct Cyc_Absyn_Exp*_tmp873=Cyc_Tcutil_rsubsexp(r,inst,_tmp871);
if(_tmp872 == _tmp870  && _tmp873 == _tmp871)return e;{
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp11AC;struct Cyc_Absyn_Exp*_tmp11AB[2];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp11AA;return Cyc_Tcutil_copye(e,(void*)((_tmp11AA=_cycalloc(sizeof(*_tmp11AA)),((_tmp11AA[0]=((_tmp11AC.tag=2,((_tmp11AC.f1=_tmp84E,((_tmp11AC.f2=((_tmp11AB[1]=_tmp873,((_tmp11AB[0]=_tmp872,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp11AB,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmp11AC)))))),_tmp11AA)))));};}else{
const char*_tmp11AF;void*_tmp11AE;return(_tmp11AE=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11AF="primop does not have 1 or 2 args!",_tag_dyneither(_tmp11AF,sizeof(char),34))),_tag_dyneither(_tmp11AE,sizeof(void*),0)));}}_LL526: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp850=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp848;if(_tmp850->tag != 5)goto _LL528;else{_tmp851=_tmp850->f1;_tmp852=_tmp850->f2;_tmp853=_tmp850->f3;}}_LL527: {
# 3300
struct Cyc_Absyn_Exp*_tmp879=Cyc_Tcutil_rsubsexp(r,inst,_tmp851);
struct Cyc_Absyn_Exp*_tmp87A=Cyc_Tcutil_rsubsexp(r,inst,_tmp852);
struct Cyc_Absyn_Exp*_tmp87B=Cyc_Tcutil_rsubsexp(r,inst,_tmp853);
if((_tmp879 == _tmp851  && _tmp87A == _tmp852) && _tmp87B == _tmp853)return e;{
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp11B2;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp11B1;return Cyc_Tcutil_copye(e,(void*)((_tmp11B1=_cycalloc(sizeof(*_tmp11B1)),((_tmp11B1[0]=((_tmp11B2.tag=5,((_tmp11B2.f1=_tmp879,((_tmp11B2.f2=_tmp87A,((_tmp11B2.f3=_tmp87B,_tmp11B2)))))))),_tmp11B1)))));};}_LL528: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp854=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp848;if(_tmp854->tag != 6)goto _LL52A;else{_tmp855=_tmp854->f1;_tmp856=_tmp854->f2;}}_LL529: {
# 3306
struct Cyc_Absyn_Exp*_tmp87E=Cyc_Tcutil_rsubsexp(r,inst,_tmp855);
struct Cyc_Absyn_Exp*_tmp87F=Cyc_Tcutil_rsubsexp(r,inst,_tmp856);
if(_tmp87E == _tmp855  && _tmp87F == _tmp856)return e;{
struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmp11B5;struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp11B4;return Cyc_Tcutil_copye(e,(void*)((_tmp11B4=_cycalloc(sizeof(*_tmp11B4)),((_tmp11B4[0]=((_tmp11B5.tag=6,((_tmp11B5.f1=_tmp87E,((_tmp11B5.f2=_tmp87F,_tmp11B5)))))),_tmp11B4)))));};}_LL52A: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp857=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp848;if(_tmp857->tag != 7)goto _LL52C;else{_tmp858=_tmp857->f1;_tmp859=_tmp857->f2;}}_LL52B: {
# 3311
struct Cyc_Absyn_Exp*_tmp882=Cyc_Tcutil_rsubsexp(r,inst,_tmp858);
struct Cyc_Absyn_Exp*_tmp883=Cyc_Tcutil_rsubsexp(r,inst,_tmp859);
if(_tmp882 == _tmp858  && _tmp883 == _tmp859)return e;{
struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmp11B8;struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp11B7;return Cyc_Tcutil_copye(e,(void*)((_tmp11B7=_cycalloc(sizeof(*_tmp11B7)),((_tmp11B7[0]=((_tmp11B8.tag=7,((_tmp11B8.f1=_tmp882,((_tmp11B8.f2=_tmp883,_tmp11B8)))))),_tmp11B7)))));};}_LL52C: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp85A=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp848;if(_tmp85A->tag != 8)goto _LL52E;else{_tmp85B=_tmp85A->f1;_tmp85C=_tmp85A->f2;}}_LL52D: {
# 3316
struct Cyc_Absyn_Exp*_tmp886=Cyc_Tcutil_rsubsexp(r,inst,_tmp85B);
struct Cyc_Absyn_Exp*_tmp887=Cyc_Tcutil_rsubsexp(r,inst,_tmp85C);
if(_tmp886 == _tmp85B  && _tmp887 == _tmp85C)return e;{
struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmp11BB;struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp11BA;return Cyc_Tcutil_copye(e,(void*)((_tmp11BA=_cycalloc(sizeof(*_tmp11BA)),((_tmp11BA[0]=((_tmp11BB.tag=8,((_tmp11BB.f1=_tmp886,((_tmp11BB.f2=_tmp887,_tmp11BB)))))),_tmp11BA)))));};}_LL52E: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp85D=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp848;if(_tmp85D->tag != 13)goto _LL530;else{_tmp85E=(void*)_tmp85D->f1;_tmp85F=_tmp85D->f2;_tmp860=_tmp85D->f3;_tmp861=_tmp85D->f4;}}_LL52F: {
# 3321
struct Cyc_Absyn_Exp*_tmp88A=Cyc_Tcutil_rsubsexp(r,inst,_tmp85F);
void*_tmp88B=Cyc_Tcutil_rsubstitute(r,inst,_tmp85E);
if(_tmp88A == _tmp85F  && _tmp88B == _tmp85E)return e;{
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp11BE;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp11BD;return Cyc_Tcutil_copye(e,(void*)((_tmp11BD=_cycalloc(sizeof(*_tmp11BD)),((_tmp11BD[0]=((_tmp11BE.tag=13,((_tmp11BE.f1=_tmp88B,((_tmp11BE.f2=_tmp88A,((_tmp11BE.f3=_tmp860,((_tmp11BE.f4=_tmp861,_tmp11BE)))))))))),_tmp11BD)))));};}_LL530: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp862=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp848;if(_tmp862->tag != 16)goto _LL532;else{_tmp863=(void*)_tmp862->f1;}}_LL531: {
# 3326
void*_tmp88E=Cyc_Tcutil_rsubstitute(r,inst,_tmp863);
if(_tmp88E == _tmp863)return e;{
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp11C1;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp11C0;return Cyc_Tcutil_copye(e,(void*)((_tmp11C0=_cycalloc(sizeof(*_tmp11C0)),((_tmp11C0[0]=((_tmp11C1.tag=16,((_tmp11C1.f1=_tmp88E,_tmp11C1)))),_tmp11C0)))));};}_LL532: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp864=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp848;if(_tmp864->tag != 17)goto _LL534;else{_tmp865=_tmp864->f1;}}_LL533: {
# 3330
struct Cyc_Absyn_Exp*_tmp891=Cyc_Tcutil_rsubsexp(r,inst,_tmp865);
if(_tmp891 == _tmp865)return e;{
struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmp11C4;struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp11C3;return Cyc_Tcutil_copye(e,(void*)((_tmp11C3=_cycalloc(sizeof(*_tmp11C3)),((_tmp11C3[0]=((_tmp11C4.tag=17,((_tmp11C4.f1=_tmp891,_tmp11C4)))),_tmp11C3)))));};}_LL534: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp866=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp848;if(_tmp866->tag != 18)goto _LL536;else{_tmp867=(void*)_tmp866->f1;_tmp868=_tmp866->f2;}}_LL535: {
# 3334
void*_tmp894=Cyc_Tcutil_rsubstitute(r,inst,_tmp867);
if(_tmp894 == _tmp867)return e;{
struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp11C7;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp11C6;return Cyc_Tcutil_copye(e,(void*)((_tmp11C6=_cycalloc(sizeof(*_tmp11C6)),((_tmp11C6[0]=((_tmp11C7.tag=18,((_tmp11C7.f1=_tmp894,((_tmp11C7.f2=_tmp868,_tmp11C7)))))),_tmp11C6)))));};}_LL536: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp869=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp848;if(_tmp869->tag != 38)goto _LL538;else{_tmp86A=(void*)_tmp869->f1;}}_LL537: {
# 3338
void*_tmp897=Cyc_Tcutil_rsubstitute(r,inst,_tmp86A);
if(_tmp897 == _tmp86A)return e;{
# 3341
void*_tmp898=Cyc_Tcutil_compress(_tmp897);void*_tmp899=_tmp898;struct Cyc_Absyn_Exp*_tmp89B;_LL53B: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp89A=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp899;if(_tmp89A->tag != 18)goto _LL53D;else{_tmp89B=_tmp89A->f1;}}_LL53C:
 return _tmp89B;_LL53D:;_LL53E: {
# 3344
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp11CA;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp11C9;return Cyc_Tcutil_copye(e,(void*)((_tmp11C9=_cycalloc(sizeof(*_tmp11C9)),((_tmp11C9[0]=((_tmp11CA.tag=38,((_tmp11CA.f1=_tmp897,_tmp11CA)))),_tmp11C9)))));}_LL53A:;};}_LL538:;_LL539: {
# 3347
const char*_tmp11CD;void*_tmp11CC;return(_tmp11CC=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11CD="non-type-level-expression in Tcutil::rsubsexp",_tag_dyneither(_tmp11CD,sizeof(char),46))),_tag_dyneither(_tmp11CC,sizeof(void*),0)));}_LL51B:;}
# 3351
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 3354
if(e == 0)return 0;else{
return Cyc_Tcutil_rsubsexp(r,inst,e);}}
# 3358
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 3361
void*_tmp8A0=f->type;
struct Cyc_Absyn_Exp*_tmp8A1=f->requires_clause;
void*_tmp8A2=Cyc_Tcutil_rsubstitute(r,inst,_tmp8A0);
struct Cyc_Absyn_Exp*_tmp8A3=Cyc_Tcutil_rsubs_exp_opt(r,inst,_tmp8A1);
if(_tmp8A0 == _tmp8A2  && _tmp8A1 == _tmp8A3)return f;else{
struct Cyc_Absyn_Aggrfield*_tmp11CE;return(_tmp11CE=_cycalloc(sizeof(*_tmp11CE)),((_tmp11CE->name=f->name,((_tmp11CE->tq=f->tq,((_tmp11CE->type=_tmp8A2,((_tmp11CE->width=f->width,((_tmp11CE->attributes=f->attributes,((_tmp11CE->requires_clause=_tmp8A3,_tmp11CE)))))))))))));}}
# 3371
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 3374
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp8A5=(struct Cyc_Absyn_Aggrfield*)fs->hd;
struct Cyc_List_List*_tmp8A6=fs->tl;
struct Cyc_Absyn_Aggrfield*_tmp8A7=Cyc_Tcutil_subst_aggrfield(r,inst,_tmp8A5);
struct Cyc_List_List*_tmp8A8=Cyc_Tcutil_subst_aggrfields(r,inst,_tmp8A6);
if(_tmp8A7 == _tmp8A5  && _tmp8A8 == _tmp8A6)return fs;{
# 3381
struct Cyc_List_List*_tmp11CF;return(_tmp11CF=_cycalloc(sizeof(*_tmp11CF)),((_tmp11CF->hd=_tmp8A7,((_tmp11CF->tl=_tmp8A8,_tmp11CF)))));};};}
# 3384
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 3387
struct _tuple1 _tmp8AA=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct Cyc_List_List*_tmp8AC;struct Cyc_List_List*_tmp8AD;struct _tuple1 _tmp8AB=_tmp8AA;_tmp8AC=_tmp8AB.f1;_tmp8AD=_tmp8AB.f2;{
struct Cyc_List_List*_tmp8AE=Cyc_Tcutil_substs(rgn,inst,_tmp8AC);
struct Cyc_List_List*_tmp8AF=Cyc_Tcutil_substs(rgn,inst,_tmp8AD);
if(_tmp8AE == _tmp8AC  && _tmp8AF == _tmp8AD)
return rgn_po;else{
# 3393
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp8AE,_tmp8AF);}};}
# 3396
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3399
void*_tmp8B0=Cyc_Tcutil_compress(t);void*_tmp8B1=_tmp8B0;struct Cyc_Absyn_Tvar*_tmp8B3;union Cyc_Absyn_AggrInfoU _tmp8B5;struct Cyc_List_List*_tmp8B6;union Cyc_Absyn_DatatypeInfoU _tmp8B8;struct Cyc_List_List*_tmp8B9;union Cyc_Absyn_DatatypeFieldInfoU _tmp8BB;struct Cyc_List_List*_tmp8BC;struct _tuple2*_tmp8BE;struct Cyc_List_List*_tmp8BF;struct Cyc_Absyn_Typedefdecl*_tmp8C0;void*_tmp8C1;void*_tmp8C3;struct Cyc_Absyn_Tqual _tmp8C4;struct Cyc_Absyn_Exp*_tmp8C5;union Cyc_Absyn_Constraint*_tmp8C6;unsigned int _tmp8C7;void*_tmp8C9;struct Cyc_Absyn_Tqual _tmp8CA;void*_tmp8CB;union Cyc_Absyn_Constraint*_tmp8CC;union Cyc_Absyn_Constraint*_tmp8CD;union Cyc_Absyn_Constraint*_tmp8CE;struct Cyc_List_List*_tmp8D0;void*_tmp8D1;struct Cyc_Absyn_Tqual _tmp8D2;void*_tmp8D3;struct Cyc_List_List*_tmp8D4;int _tmp8D5;struct Cyc_Absyn_VarargInfo*_tmp8D6;struct Cyc_List_List*_tmp8D7;struct Cyc_List_List*_tmp8D8;struct Cyc_Absyn_Exp*_tmp8D9;struct Cyc_Absyn_Exp*_tmp8DA;struct Cyc_List_List*_tmp8DC;enum Cyc_Absyn_AggrKind _tmp8DE;struct Cyc_List_List*_tmp8DF;void*_tmp8E1;void*_tmp8E3;void*_tmp8E5;void*_tmp8E6;void*_tmp8E8;struct Cyc_Absyn_Exp*_tmp8EA;struct Cyc_Absyn_Exp*_tmp8EC;void*_tmp8F6;void*_tmp8F8;struct Cyc_List_List*_tmp8FA;_LL540: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8B2=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp8B1;if(_tmp8B2->tag != 2)goto _LL542;else{_tmp8B3=_tmp8B2->f1;}}_LL541: {
# 3402
struct _handler_cons _tmp8FC;_push_handler(& _tmp8FC);{int _tmp8FE=0;if(setjmp(_tmp8FC.handler))_tmp8FE=1;if(!_tmp8FE){{void*_tmp8FF=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp8B3);_npop_handler(0);return _tmp8FF;};_pop_handler();}else{void*_tmp8FD=(void*)_exn_thrown;void*_tmp901=_tmp8FD;void*_tmp903;_LL579: {struct Cyc_Core_Not_found_exn_struct*_tmp902=(struct Cyc_Core_Not_found_exn_struct*)_tmp901;if(_tmp902->tag != Cyc_Core_Not_found)goto _LL57B;}_LL57A:
 return t;_LL57B: _tmp903=_tmp901;_LL57C:(void)_rethrow(_tmp903);_LL578:;}};}_LL542: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp8B4=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp8B1;if(_tmp8B4->tag != 11)goto _LL544;else{_tmp8B5=(_tmp8B4->f1).aggr_info;_tmp8B6=(_tmp8B4->f1).targs;}}_LL543: {
# 3405
struct Cyc_List_List*_tmp904=Cyc_Tcutil_substs(rgn,inst,_tmp8B6);
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp11D5;struct Cyc_Absyn_AggrInfo _tmp11D4;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp11D3;return _tmp904 == _tmp8B6?t:(void*)((_tmp11D3=_cycalloc(sizeof(*_tmp11D3)),((_tmp11D3[0]=((_tmp11D5.tag=11,((_tmp11D5.f1=((_tmp11D4.aggr_info=_tmp8B5,((_tmp11D4.targs=_tmp904,_tmp11D4)))),_tmp11D5)))),_tmp11D3))));}_LL544: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp8B7=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp8B1;if(_tmp8B7->tag != 3)goto _LL546;else{_tmp8B8=(_tmp8B7->f1).datatype_info;_tmp8B9=(_tmp8B7->f1).targs;}}_LL545: {
# 3408
struct Cyc_List_List*_tmp908=Cyc_Tcutil_substs(rgn,inst,_tmp8B9);
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp11DB;struct Cyc_Absyn_DatatypeInfo _tmp11DA;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp11D9;return _tmp908 == _tmp8B9?t:(void*)(
(_tmp11D9=_cycalloc(sizeof(*_tmp11D9)),((_tmp11D9[0]=((_tmp11DB.tag=3,((_tmp11DB.f1=((_tmp11DA.datatype_info=_tmp8B8,((_tmp11DA.targs=_tmp908,_tmp11DA)))),_tmp11DB)))),_tmp11D9))));}_LL546: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp8BA=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp8B1;if(_tmp8BA->tag != 4)goto _LL548;else{_tmp8BB=(_tmp8BA->f1).field_info;_tmp8BC=(_tmp8BA->f1).targs;}}_LL547: {
# 3412
struct Cyc_List_List*_tmp90C=Cyc_Tcutil_substs(rgn,inst,_tmp8BC);
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp11E1;struct Cyc_Absyn_DatatypeFieldInfo _tmp11E0;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp11DF;return _tmp90C == _tmp8BC?t:(void*)(
(_tmp11DF=_cycalloc(sizeof(*_tmp11DF)),((_tmp11DF[0]=((_tmp11E1.tag=4,((_tmp11E1.f1=((_tmp11E0.field_info=_tmp8BB,((_tmp11E0.targs=_tmp90C,_tmp11E0)))),_tmp11E1)))),_tmp11DF))));}_LL548: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp8BD=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp8B1;if(_tmp8BD->tag != 17)goto _LL54A;else{_tmp8BE=_tmp8BD->f1;_tmp8BF=_tmp8BD->f2;_tmp8C0=_tmp8BD->f3;_tmp8C1=(void*)_tmp8BD->f4;}}_LL549: {
# 3416
struct Cyc_List_List*_tmp910=Cyc_Tcutil_substs(rgn,inst,_tmp8BF);
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp11E4;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp11E3;return _tmp910 == _tmp8BF?t:(void*)((_tmp11E3=_cycalloc(sizeof(*_tmp11E3)),((_tmp11E3[0]=((_tmp11E4.tag=17,((_tmp11E4.f1=_tmp8BE,((_tmp11E4.f2=_tmp910,((_tmp11E4.f3=_tmp8C0,((_tmp11E4.f4=_tmp8C1,_tmp11E4)))))))))),_tmp11E3))));}_LL54A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8C2=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8B1;if(_tmp8C2->tag != 8)goto _LL54C;else{_tmp8C3=(_tmp8C2->f1).elt_type;_tmp8C4=(_tmp8C2->f1).tq;_tmp8C5=(_tmp8C2->f1).num_elts;_tmp8C6=(_tmp8C2->f1).zero_term;_tmp8C7=(_tmp8C2->f1).zt_loc;}}_LL54B: {
# 3419
void*_tmp913=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8C3);
struct Cyc_Absyn_Exp*_tmp914=_tmp8C5 == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,_tmp8C5);
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp11EA;struct Cyc_Absyn_ArrayInfo _tmp11E9;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp11E8;return _tmp913 == _tmp8C3  && _tmp914 == _tmp8C5?t:(void*)(
(_tmp11E8=_cycalloc(sizeof(*_tmp11E8)),((_tmp11E8[0]=((_tmp11EA.tag=8,((_tmp11EA.f1=((_tmp11E9.elt_type=_tmp913,((_tmp11E9.tq=_tmp8C4,((_tmp11E9.num_elts=_tmp914,((_tmp11E9.zero_term=_tmp8C6,((_tmp11E9.zt_loc=_tmp8C7,_tmp11E9)))))))))),_tmp11EA)))),_tmp11E8))));}_LL54C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8C8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B1;if(_tmp8C8->tag != 5)goto _LL54E;else{_tmp8C9=(_tmp8C8->f1).elt_typ;_tmp8CA=(_tmp8C8->f1).elt_tq;_tmp8CB=((_tmp8C8->f1).ptr_atts).rgn;_tmp8CC=((_tmp8C8->f1).ptr_atts).nullable;_tmp8CD=((_tmp8C8->f1).ptr_atts).bounds;_tmp8CE=((_tmp8C8->f1).ptr_atts).zero_term;}}_LL54D: {
# 3424
void*_tmp918=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8C9);
void*_tmp919=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8CB);
union Cyc_Absyn_Constraint*_tmp91A=_tmp8CD;
{void*_tmp91B=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp8CD);void*_tmp91C=_tmp91B;struct Cyc_Absyn_Exp*_tmp91E;_LL57E: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp91D=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp91C;if(_tmp91D->tag != 1)goto _LL580;else{_tmp91E=_tmp91D->f1;}}_LL57F: {
# 3429
struct Cyc_Absyn_Exp*_tmp91F=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp91E);
if(_tmp91F != _tmp91E){
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp11ED;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp11EC;_tmp91A=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp11EC=_cycalloc(sizeof(*_tmp11EC)),((_tmp11EC[0]=((_tmp11ED.tag=1,((_tmp11ED.f1=_tmp91F,_tmp11ED)))),_tmp11EC)))));}
goto _LL57D;}_LL580:;_LL581:
 goto _LL57D;_LL57D:;}
# 3435
if((_tmp918 == _tmp8C9  && _tmp919 == _tmp8CB) && _tmp91A == _tmp8CD)
return t;{
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp11F3;struct Cyc_Absyn_PtrInfo _tmp11F2;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp11F1;return(void*)((_tmp11F1=_cycalloc(sizeof(*_tmp11F1)),((_tmp11F1[0]=((_tmp11F3.tag=5,((_tmp11F3.f1=((_tmp11F2.elt_typ=_tmp918,((_tmp11F2.elt_tq=_tmp8CA,((_tmp11F2.ptr_atts=(((_tmp11F2.ptr_atts).rgn=_tmp919,(((_tmp11F2.ptr_atts).nullable=_tmp8CC,(((_tmp11F2.ptr_atts).bounds=_tmp91A,(((_tmp11F2.ptr_atts).zero_term=_tmp8CE,(((_tmp11F2.ptr_atts).ptrloc=0,_tmp11F2.ptr_atts)))))))))),_tmp11F2)))))),_tmp11F3)))),_tmp11F1))));};}_LL54E: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp8CF=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp8B1;if(_tmp8CF->tag != 9)goto _LL550;else{_tmp8D0=(_tmp8CF->f1).tvars;_tmp8D1=(_tmp8CF->f1).effect;_tmp8D2=(_tmp8CF->f1).ret_tqual;_tmp8D3=(_tmp8CF->f1).ret_typ;_tmp8D4=(_tmp8CF->f1).args;_tmp8D5=(_tmp8CF->f1).c_varargs;_tmp8D6=(_tmp8CF->f1).cyc_varargs;_tmp8D7=(_tmp8CF->f1).rgn_po;_tmp8D8=(_tmp8CF->f1).attributes;_tmp8D9=(_tmp8CF->f1).requires_clause;_tmp8DA=(_tmp8CF->f1).ensures_clause;}}_LL54F:
# 3441
{struct Cyc_List_List*_tmp925=_tmp8D0;for(0;_tmp925 != 0;_tmp925=_tmp925->tl){
struct _tuple16*_tmp11FD;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp11FC;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp11FB;struct Cyc_List_List*_tmp11FA;inst=((_tmp11FA=_region_malloc(rgn,sizeof(*_tmp11FA)),((_tmp11FA->hd=((_tmp11FD=_region_malloc(rgn,sizeof(*_tmp11FD)),((_tmp11FD->f1=(struct Cyc_Absyn_Tvar*)_tmp925->hd,((_tmp11FD->f2=(void*)((_tmp11FB=_cycalloc(sizeof(*_tmp11FB)),((_tmp11FB[0]=((_tmp11FC.tag=2,((_tmp11FC.f1=(struct Cyc_Absyn_Tvar*)_tmp925->hd,_tmp11FC)))),_tmp11FB)))),_tmp11FD)))))),((_tmp11FA->tl=inst,_tmp11FA))))));}}{
struct _tuple1 _tmp92A=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp8D4));
# 3443
struct Cyc_List_List*_tmp92C;struct Cyc_List_List*_tmp92D;struct _tuple1 _tmp92B=_tmp92A;_tmp92C=_tmp92B.f1;_tmp92D=_tmp92B.f2;{
# 3445
struct Cyc_List_List*_tmp92E=_tmp8D4;
struct Cyc_List_List*_tmp92F=Cyc_Tcutil_substs(rgn,inst,_tmp92D);
if(_tmp92F != _tmp92D)
_tmp92E=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmp8D4,_tmp92F);{
void*eff2;
if(_tmp8D1 == 0)
eff2=0;else{
# 3453
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8D1);
if(new_eff == _tmp8D1)
eff2=_tmp8D1;else{
# 3457
eff2=new_eff;}}{
# 3459
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(_tmp8D6 == 0)
cyc_varargs2=0;else{
# 3463
struct Cyc_Absyn_VarargInfo _tmp930=*_tmp8D6;struct _dyneither_ptr*_tmp932;struct Cyc_Absyn_Tqual _tmp933;void*_tmp934;int _tmp935;struct Cyc_Absyn_VarargInfo _tmp931=_tmp930;_tmp932=_tmp931.name;_tmp933=_tmp931.tq;_tmp934=_tmp931.type;_tmp935=_tmp931.inject;{
void*_tmp936=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp934);
if(_tmp936 == _tmp934)cyc_varargs2=_tmp8D6;else{
# 3467
struct Cyc_Absyn_VarargInfo*_tmp11FE;cyc_varargs2=((_tmp11FE=_cycalloc(sizeof(*_tmp11FE)),((_tmp11FE->name=_tmp932,((_tmp11FE->tq=_tmp933,((_tmp11FE->type=_tmp936,((_tmp11FE->inject=_tmp935,_tmp11FE))))))))));}};}{
# 3469
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp8D7);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp8D9);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp8DA);
struct Cyc_List_List*_tmp938=Cyc_Tcutil_extract_relns(_tmp92E,req2);
struct Cyc_List_List*_tmp939=Cyc_Tcutil_extract_relns(_tmp92E,ens2);
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1204;struct Cyc_Absyn_FnInfo _tmp1203;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1202;return(void*)(
(_tmp1202=_cycalloc(sizeof(*_tmp1202)),((_tmp1202[0]=((_tmp1204.tag=9,((_tmp1204.f1=((_tmp1203.tvars=_tmp8D0,((_tmp1203.effect=eff2,((_tmp1203.ret_tqual=_tmp8D2,((_tmp1203.ret_typ=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8D3),((_tmp1203.args=_tmp92E,((_tmp1203.c_varargs=_tmp8D5,((_tmp1203.cyc_varargs=cyc_varargs2,((_tmp1203.rgn_po=rgn_po2,((_tmp1203.attributes=_tmp8D8,((_tmp1203.requires_clause=req2,((_tmp1203.requires_relns=_tmp938,((_tmp1203.ensures_clause=ens2,((_tmp1203.ensures_relns=_tmp939,_tmp1203)))))))))))))))))))))))))),_tmp1204)))),_tmp1202))));};};};};};_LL550: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp8DB=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp8B1;if(_tmp8DB->tag != 10)goto _LL552;else{_tmp8DC=_tmp8DB->f1;}}_LL551: {
# 3479
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp93D=_tmp8DC;for(0;_tmp93D != 0;_tmp93D=_tmp93D->tl){
void*_tmp93E=(*((struct _tuple12*)_tmp93D->hd)).f2;
void*_tmp93F=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp93E);
if(_tmp93E != _tmp93F)
change=1;{
# 3487
struct Cyc_List_List*_tmp1205;ts2=((_tmp1205=_region_malloc(rgn,sizeof(*_tmp1205)),((_tmp1205->hd=_tmp93F,((_tmp1205->tl=ts2,_tmp1205))))));};}}
# 3489
if(!change)
return t;{
struct Cyc_List_List*_tmp941=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp8DC,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp1208;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp1207;return(void*)((_tmp1207=_cycalloc(sizeof(*_tmp1207)),((_tmp1207[0]=((_tmp1208.tag=10,((_tmp1208.f1=_tmp941,_tmp1208)))),_tmp1207))));};}_LL552: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp8DD=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp8B1;if(_tmp8DD->tag != 12)goto _LL554;else{_tmp8DE=_tmp8DD->f1;_tmp8DF=_tmp8DD->f2;}}_LL553: {
# 3494
struct Cyc_List_List*_tmp944=Cyc_Tcutil_subst_aggrfields(rgn,inst,_tmp8DF);
if(_tmp8DF == _tmp944)return t;{
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp120B;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp120A;return(void*)((_tmp120A=_cycalloc(sizeof(*_tmp120A)),((_tmp120A[0]=((_tmp120B.tag=12,((_tmp120B.f1=_tmp8DE,((_tmp120B.f2=_tmp944,_tmp120B)))))),_tmp120A))));};}_LL554: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp8E0=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp8B1;if(_tmp8E0->tag != 1)goto _LL556;else{_tmp8E1=(void*)_tmp8E0->f2;}}_LL555:
# 3498
 if(_tmp8E1 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8E1);else{
return t;}_LL556: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp8E2=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp8B1;if(_tmp8E2->tag != 15)goto _LL558;else{_tmp8E3=(void*)_tmp8E2->f1;}}_LL557: {
# 3501
void*_tmp947=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8E3);
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp120E;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp120D;return _tmp947 == _tmp8E3?t:(void*)((_tmp120D=_cycalloc(sizeof(*_tmp120D)),((_tmp120D[0]=((_tmp120E.tag=15,((_tmp120E.f1=_tmp947,_tmp120E)))),_tmp120D))));}_LL558: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp8E4=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp8B1;if(_tmp8E4->tag != 16)goto _LL55A;else{_tmp8E5=(void*)_tmp8E4->f1;_tmp8E6=(void*)_tmp8E4->f2;}}_LL559: {
# 3504
void*_tmp94A=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8E5);
void*_tmp94B=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8E6);
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp1211;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp1210;return _tmp94A == _tmp8E5  && _tmp94B == _tmp8E6?t:(void*)((_tmp1210=_cycalloc(sizeof(*_tmp1210)),((_tmp1210[0]=((_tmp1211.tag=16,((_tmp1211.f1=_tmp94A,((_tmp1211.f2=_tmp94B,_tmp1211)))))),_tmp1210))));}_LL55A: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp8E7=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp8B1;if(_tmp8E7->tag != 19)goto _LL55C;else{_tmp8E8=(void*)_tmp8E7->f1;}}_LL55B: {
# 3508
void*_tmp94E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8E8);
struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp1214;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp1213;return _tmp94E == _tmp8E8?t:(void*)((_tmp1213=_cycalloc(sizeof(*_tmp1213)),((_tmp1213[0]=((_tmp1214.tag=19,((_tmp1214.f1=_tmp94E,_tmp1214)))),_tmp1213))));}_LL55C: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp8E9=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp8B1;if(_tmp8E9->tag != 18)goto _LL55E;else{_tmp8EA=_tmp8E9->f1;}}_LL55D: {
# 3511
struct Cyc_Absyn_Exp*_tmp951=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp8EA);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp1217;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1216;return _tmp951 == _tmp8EA?t:(void*)((_tmp1216=_cycalloc(sizeof(*_tmp1216)),((_tmp1216[0]=((_tmp1217.tag=18,((_tmp1217.f1=_tmp951,_tmp1217)))),_tmp1216))));}_LL55E: {struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp8EB=(struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp8B1;if(_tmp8EB->tag != 27)goto _LL560;else{_tmp8EC=_tmp8EB->f1;}}_LL55F: {
# 3514
struct Cyc_Absyn_Exp*_tmp954=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp8EC);
struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmp121A;struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp1219;return _tmp954 == _tmp8EC?t:(void*)((_tmp1219=_cycalloc(sizeof(*_tmp1219)),((_tmp1219[0]=((_tmp121A.tag=27,((_tmp121A.f1=_tmp954,_tmp121A)))),_tmp1219))));}_LL560: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp8ED=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp8B1;if(_tmp8ED->tag != 13)goto _LL562;}_LL561:
 goto _LL563;_LL562: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp8EE=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp8B1;if(_tmp8EE->tag != 14)goto _LL564;}_LL563:
 goto _LL565;_LL564: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp8EF=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp8B1;if(_tmp8EF->tag != 0)goto _LL566;}_LL565:
 goto _LL567;_LL566: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8F0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp8B1;if(_tmp8F0->tag != 6)goto _LL568;}_LL567:
 goto _LL569;_LL568: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp8F1=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp8B1;if(_tmp8F1->tag != 7)goto _LL56A;}_LL569:
 goto _LL56B;_LL56A: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp8F2=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp8B1;if(_tmp8F2->tag != 22)goto _LL56C;}_LL56B:
 goto _LL56D;_LL56C: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp8F3=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp8B1;if(_tmp8F3->tag != 21)goto _LL56E;}_LL56D:
 goto _LL56F;_LL56E: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp8F4=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp8B1;if(_tmp8F4->tag != 20)goto _LL570;}_LL56F:
 return t;_LL570: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp8F5=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp8B1;if(_tmp8F5->tag != 25)goto _LL572;else{_tmp8F6=(void*)_tmp8F5->f1;}}_LL571: {
# 3525
void*_tmp957=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8F6);
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp121D;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp121C;return _tmp957 == _tmp8F6?t:(void*)((_tmp121C=_cycalloc(sizeof(*_tmp121C)),((_tmp121C[0]=((_tmp121D.tag=25,((_tmp121D.f1=_tmp957,_tmp121D)))),_tmp121C))));}_LL572: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp8F7=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp8B1;if(_tmp8F7->tag != 23)goto _LL574;else{_tmp8F8=(void*)_tmp8F7->f1;}}_LL573: {
# 3528
void*_tmp95A=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8F8);
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1220;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp121F;return _tmp95A == _tmp8F8?t:(void*)((_tmp121F=_cycalloc(sizeof(*_tmp121F)),((_tmp121F[0]=((_tmp1220.tag=23,((_tmp1220.f1=_tmp95A,_tmp1220)))),_tmp121F))));}_LL574: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp8F9=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp8B1;if(_tmp8F9->tag != 24)goto _LL576;else{_tmp8FA=_tmp8F9->f1;}}_LL575: {
# 3531
struct Cyc_List_List*_tmp95D=Cyc_Tcutil_substs(rgn,inst,_tmp8FA);
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1223;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1222;return _tmp95D == _tmp8FA?t:(void*)((_tmp1222=_cycalloc(sizeof(*_tmp1222)),((_tmp1222[0]=((_tmp1223.tag=24,((_tmp1223.f1=_tmp95D,_tmp1223)))),_tmp1222))));}_LL576: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp8FB=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp8B1;if(_tmp8FB->tag != 26)goto _LL53F;}_LL577: {
const char*_tmp1226;void*_tmp1225;(_tmp1225=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1226="found typedecltype in rsubs",_tag_dyneither(_tmp1226,sizeof(char),28))),_tag_dyneither(_tmp1225,sizeof(void*),0)));}_LL53F:;}
# 3537
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 3540
if(ts == 0)
return 0;{
void*_tmp962=(void*)ts->hd;
struct Cyc_List_List*_tmp963=ts->tl;
void*_tmp964=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp962);
struct Cyc_List_List*_tmp965=Cyc_Tcutil_substs(rgn,inst,_tmp963);
if(_tmp962 == _tmp964  && _tmp963 == _tmp965)
return ts;{
struct Cyc_List_List*_tmp1227;return(_tmp1227=_cycalloc(sizeof(*_tmp1227)),((_tmp1227->hd=_tmp964,((_tmp1227->tl=_tmp965,_tmp1227)))));};};}
# 3551
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 3558
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp967=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
struct Cyc_Core_Opt*_tmp122A;struct _tuple16*_tmp1229;return(_tmp1229=_cycalloc(sizeof(*_tmp1229)),((_tmp1229->f1=tv,((_tmp1229->f2=Cyc_Absyn_new_evar(_tmp967,((_tmp122A=_cycalloc(sizeof(*_tmp122A)),((_tmp122A->v=s,_tmp122A))))),_tmp1229)))));}
# 3563
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*env,struct Cyc_Absyn_Tvar*tv){
# 3565
struct Cyc_List_List*_tmp96B;struct _RegionHandle*_tmp96C;struct _tuple17*_tmp96A=env;_tmp96B=_tmp96A->f1;_tmp96C=_tmp96A->f2;{
struct Cyc_Core_Opt*_tmp96D=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
struct Cyc_Core_Opt*_tmp122D;struct _tuple16*_tmp122C;return(_tmp122C=_region_malloc(_tmp96C,sizeof(*_tmp122C)),((_tmp122C->f1=tv,((_tmp122C->f2=Cyc_Absyn_new_evar(_tmp96D,((_tmp122D=_cycalloc(sizeof(*_tmp122D)),((_tmp122D->v=_tmp96B,_tmp122D))))),_tmp122C)))));};}
# 3575
static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(unsigned int loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3579
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2)){
const char*_tmp1233;void*_tmp1232[3];struct Cyc_String_pa_PrintArg_struct _tmp1231;struct Cyc_String_pa_PrintArg_struct _tmp1230;struct Cyc_String_pa_PrintArg_struct _tmp122F;(_tmp122F.tag=0,((_tmp122F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(k2)),((_tmp1230.tag=0,((_tmp1230.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1)),((_tmp1231.tag=0,((_tmp1231.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmp1232[0]=& _tmp1231,((_tmp1232[1]=& _tmp1230,((_tmp1232[2]=& _tmp122F,Cyc_Tcutil_terr(loc,((_tmp1233="type variable %s is used with inconsistent kinds %s and %s",_tag_dyneither(_tmp1233,sizeof(char),59))),_tag_dyneither(_tmp1232,sizeof(void*),3)))))))))))))))))));}
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){
const char*_tmp1236;void*_tmp1235;(_tmp1235=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1236="same type variable has different identity!",_tag_dyneither(_tmp1236,sizeof(char),43))),_tag_dyneither(_tmp1235,sizeof(void*),0)));}}
return tvs;}}}
# 3593
tv->identity=Cyc_Tcutil_new_tvar_id();{
struct Cyc_List_List*_tmp1237;return(_tmp1237=_cycalloc(sizeof(*_tmp1237)),((_tmp1237->hd=tv,((_tmp1237->tl=tvs,_tmp1237)))));};}
# 3599
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3601
if(tv->identity == - 1){
const char*_tmp123A;void*_tmp1239;(_tmp1239=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp123A="fast_add_free_tvar: bad identity in tv",_tag_dyneither(_tmp123A,sizeof(char),39))),_tag_dyneither(_tmp1239,sizeof(void*),0)));}
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3605
struct Cyc_Absyn_Tvar*_tmp97A=(struct Cyc_Absyn_Tvar*)tvs2->hd;
if(_tmp97A->identity == - 1){
const char*_tmp123D;void*_tmp123C;(_tmp123C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp123D="fast_add_free_tvar: bad identity in tvs2",_tag_dyneither(_tmp123D,sizeof(char),41))),_tag_dyneither(_tmp123C,sizeof(void*),0)));}
if(_tmp97A->identity == tv->identity)
return tvs;}}{
# 3612
struct Cyc_List_List*_tmp123E;return(_tmp123E=_cycalloc(sizeof(*_tmp123E)),((_tmp123E->hd=tv,((_tmp123E->tl=tvs,_tmp123E)))));};}struct _tuple28{struct Cyc_Absyn_Tvar*f1;int f2;};
# 3618
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 3623
if(tv->identity == - 1){
const char*_tmp1241;void*_tmp1240;(_tmp1240=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1241="fast_add_free_tvar_bool: bad identity in tv",_tag_dyneither(_tmp1241,sizeof(char),44))),_tag_dyneither(_tmp1240,sizeof(void*),0)));}
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3627
struct _tuple28*_tmp980=(struct _tuple28*)tvs2->hd;struct Cyc_Absyn_Tvar*_tmp982;int*_tmp983;struct _tuple28*_tmp981=_tmp980;_tmp982=_tmp981->f1;_tmp983=(int*)& _tmp981->f2;
if(_tmp982->identity == - 1){
const char*_tmp1244;void*_tmp1243;(_tmp1243=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1244="fast_add_free_tvar_bool: bad identity in tvs2",_tag_dyneither(_tmp1244,sizeof(char),46))),_tag_dyneither(_tmp1243,sizeof(void*),0)));}
if(_tmp982->identity == tv->identity){
*_tmp983=*_tmp983  || b;
return tvs;}}}{
# 3635
struct _tuple28*_tmp1247;struct Cyc_List_List*_tmp1246;return(_tmp1246=_region_malloc(r,sizeof(*_tmp1246)),((_tmp1246->hd=((_tmp1247=_region_malloc(r,sizeof(*_tmp1247)),((_tmp1247->f1=tv,((_tmp1247->f2=b,_tmp1247)))))),((_tmp1246->tl=tvs,_tmp1246)))));};}
# 3639
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3641
if(tv->identity == - 1){
const char*_tmp124B;void*_tmp124A[1];struct Cyc_String_pa_PrintArg_struct _tmp1249;(_tmp1249.tag=0,((_tmp1249.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(tv)),((_tmp124A[0]=& _tmp1249,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp124B="bound tvar id for %s is NULL",_tag_dyneither(_tmp124B,sizeof(char),29))),_tag_dyneither(_tmp124A,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp124C;return(_tmp124C=_cycalloc(sizeof(*_tmp124C)),((_tmp124C->hd=tv,((_tmp124C->tl=tvs,_tmp124C)))));};}struct _tuple29{void*f1;int f2;};
# 3650
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 3653
void*_tmp98C=Cyc_Tcutil_compress(e);void*_tmp98D=_tmp98C;int _tmp98F;_LL583: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp98E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp98D;if(_tmp98E->tag != 1)goto _LL585;else{_tmp98F=_tmp98E->f3;}}_LL584:
# 3655
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple29*_tmp990=(struct _tuple29*)es2->hd;void*_tmp992;int*_tmp993;struct _tuple29*_tmp991=_tmp990;_tmp992=_tmp991->f1;_tmp993=(int*)& _tmp991->f2;{
void*_tmp994=Cyc_Tcutil_compress(_tmp992);void*_tmp995=_tmp994;int _tmp997;_LL588: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp996=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp995;if(_tmp996->tag != 1)goto _LL58A;else{_tmp997=_tmp996->f3;}}_LL589:
# 3659
 if(_tmp98F == _tmp997){
if(b != *_tmp993)*_tmp993=1;
return es;}
# 3663
goto _LL587;_LL58A:;_LL58B:
 goto _LL587;_LL587:;};}}{
# 3667
struct _tuple29*_tmp124F;struct Cyc_List_List*_tmp124E;return(_tmp124E=_region_malloc(r,sizeof(*_tmp124E)),((_tmp124E->hd=((_tmp124F=_region_malloc(r,sizeof(*_tmp124F)),((_tmp124F->f1=e,((_tmp124F->f2=b,_tmp124F)))))),((_tmp124E->tl=es,_tmp124E)))));};_LL585:;_LL586:
 return es;_LL582:;}
# 3672
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3675
struct Cyc_List_List*r=0;
for(0;tvs != 0;tvs=tvs->tl){
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3684
if(!present){struct Cyc_List_List*_tmp1250;r=((_tmp1250=_region_malloc(rgn,sizeof(*_tmp1250)),((_tmp1250->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1250->tl=r,_tmp1250))))));}}
# 3686
r=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);
return r;}
# 3691
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3695
struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp99B=*((struct _tuple28*)tvs->hd);struct Cyc_Absyn_Tvar*_tmp99D;int _tmp99E;struct _tuple28 _tmp99C=_tmp99B;_tmp99D=_tmp99C.f1;_tmp99E=_tmp99C.f2;{
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp99D->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3705
if(!present){struct Cyc_List_List*_tmp1251;res=((_tmp1251=_region_malloc(r,sizeof(*_tmp1251)),((_tmp1251->hd=(struct _tuple28*)tvs->hd,((_tmp1251->tl=res,_tmp1251))))));}};}
# 3707
res=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);
return res;}
# 3711
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){
# 3713
if(width != 0){
unsigned int w=0;
if(!Cyc_Tcutil_is_const_exp(width)){
const char*_tmp1255;void*_tmp1254[1];struct Cyc_String_pa_PrintArg_struct _tmp1253;(_tmp1253.tag=0,((_tmp1253.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1254[0]=& _tmp1253,Cyc_Tcutil_terr(loc,((_tmp1255="bitfield %s does not have constant width",_tag_dyneither(_tmp1255,sizeof(char),41))),_tag_dyneither(_tmp1254,sizeof(void*),1)))))));}else{
# 3718
struct _tuple14 _tmp9A3=Cyc_Evexp_eval_const_uint_exp(width);unsigned int _tmp9A5;int _tmp9A6;struct _tuple14 _tmp9A4=_tmp9A3;_tmp9A5=_tmp9A4.f1;_tmp9A6=_tmp9A4.f2;
if(!_tmp9A6){
const char*_tmp1258;void*_tmp1257;(_tmp1257=0,Cyc_Tcutil_terr(loc,((_tmp1258="bitfield width cannot use sizeof or offsetof",_tag_dyneither(_tmp1258,sizeof(char),45))),_tag_dyneither(_tmp1257,sizeof(void*),0)));}
w=_tmp9A5;}{
# 3723
void*_tmp9A9=Cyc_Tcutil_compress(field_typ);void*_tmp9AA=_tmp9A9;enum Cyc_Absyn_Size_of _tmp9AC;_LL58D: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp9AB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp9AA;if(_tmp9AB->tag != 6)goto _LL58F;else{_tmp9AC=_tmp9AB->f2;}}_LL58E:
# 3726
 switch(_tmp9AC){case Cyc_Absyn_Char_sz: _LL591:
 if(w > 8){const char*_tmp125B;void*_tmp125A;(_tmp125A=0,Cyc_Tcutil_terr(loc,((_tmp125B="bitfield larger than type",_tag_dyneither(_tmp125B,sizeof(char),26))),_tag_dyneither(_tmp125A,sizeof(void*),0)));}break;case Cyc_Absyn_Short_sz: _LL592:
 if(w > 16){const char*_tmp125E;void*_tmp125D;(_tmp125D=0,Cyc_Tcutil_terr(loc,((_tmp125E="bitfield larger than type",_tag_dyneither(_tmp125E,sizeof(char),26))),_tag_dyneither(_tmp125D,sizeof(void*),0)));}break;case Cyc_Absyn_Long_sz: _LL593:
 goto _LL594;case Cyc_Absyn_Int_sz: _LL594:
# 3731
 if(w > 32){const char*_tmp1261;void*_tmp1260;(_tmp1260=0,Cyc_Tcutil_terr(loc,((_tmp1261="bitfield larger than type",_tag_dyneither(_tmp1261,sizeof(char),26))),_tag_dyneither(_tmp1260,sizeof(void*),0)));}break;case Cyc_Absyn_LongLong_sz: _LL595:
# 3733
 if(w > 64){const char*_tmp1264;void*_tmp1263;(_tmp1263=0,Cyc_Tcutil_terr(loc,((_tmp1264="bitfield larger than type",_tag_dyneither(_tmp1264,sizeof(char),26))),_tag_dyneither(_tmp1263,sizeof(void*),0)));}break;}
# 3735
goto _LL58C;_LL58F:;_LL590:
# 3737
{const char*_tmp1269;void*_tmp1268[2];struct Cyc_String_pa_PrintArg_struct _tmp1267;struct Cyc_String_pa_PrintArg_struct _tmp1266;(_tmp1266.tag=0,((_tmp1266.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(field_typ)),((_tmp1267.tag=0,((_tmp1267.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1268[0]=& _tmp1267,((_tmp1268[1]=& _tmp1266,Cyc_Tcutil_terr(loc,((_tmp1269="bitfield %s must have integral type but has type %s",_tag_dyneither(_tmp1269,sizeof(char),52))),_tag_dyneither(_tmp1268,sizeof(void*),2)))))))))))));}
goto _LL58C;_LL58C:;};}}
# 3744
static void Cyc_Tcutil_check_field_atts(unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp9B9=(void*)atts->hd;void*_tmp9BA=_tmp9B9;_LL598: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp9BB=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp9BA;if(_tmp9BB->tag != 7)goto _LL59A;}_LL599:
 continue;_LL59A: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp9BC=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp9BA;if(_tmp9BC->tag != 6)goto _LL59C;}_LL59B:
 continue;_LL59C:;_LL59D: {
const char*_tmp126E;void*_tmp126D[2];struct Cyc_String_pa_PrintArg_struct _tmp126C;struct Cyc_String_pa_PrintArg_struct _tmp126B;(_tmp126B.tag=0,((_tmp126B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp126C.tag=0,((_tmp126C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp126D[0]=& _tmp126C,((_tmp126D[1]=& _tmp126B,Cyc_Tcutil_terr(loc,((_tmp126E="bad attribute %s on member %s",_tag_dyneither(_tmp126E,sizeof(char),30))),_tag_dyneither(_tmp126D,sizeof(void*),2)))))))))))));}_LL597:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
# 3768
typedef struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_cvtenv_t;
# 3772
int Cyc_Tcutil_extract_const_from_typedef(unsigned int loc,int declared_const,void*t){
void*_tmp9C1=t;struct Cyc_Absyn_Typedefdecl*_tmp9C3;void*_tmp9C4;_LL59F: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp9C2=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9C1;if(_tmp9C2->tag != 17)goto _LL5A1;else{_tmp9C3=_tmp9C2->f3;_tmp9C4=(void*)_tmp9C2->f4;}}_LL5A0:
# 3775
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp9C3))->tq).real_const  || (_tmp9C3->tq).print_const){
if(declared_const){const char*_tmp1271;void*_tmp1270;(_tmp1270=0,Cyc_Tcutil_warn(loc,((_tmp1271="extra const",_tag_dyneither(_tmp1271,sizeof(char),12))),_tag_dyneither(_tmp1270,sizeof(void*),0)));}
return 1;}
# 3780
if((unsigned int)_tmp9C4)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp9C4);else{
return declared_const;}_LL5A1:;_LL5A2:
 return declared_const;_LL59E:;}
# 3787
static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td != 0){
if(td->defn != 0){
void*_tmp9C7=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_tmp9C8=_tmp9C7;void*_tmp9CA;_LL5A4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9C9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9C8;if(_tmp9C9->tag != 5)goto _LL5A6;else{_tmp9CA=((_tmp9C9->f1).ptr_atts).rgn;}}_LL5A5:
# 3792
{void*_tmp9CB=Cyc_Tcutil_compress(_tmp9CA);void*_tmp9CC=_tmp9CB;struct Cyc_Absyn_Tvar*_tmp9CE;_LL5A9: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp9CD=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9CC;if(_tmp9CD->tag != 2)goto _LL5AB;else{_tmp9CE=_tmp9CD->f1;}}_LL5AA:
# 3794
 return Cyc_Absyn_tvar_cmp(tvar,_tmp9CE)== 0;_LL5AB:;_LL5AC:
 goto _LL5A8;_LL5A8:;}
# 3797
goto _LL5A3;_LL5A6:;_LL5A7:
 goto _LL5A3;_LL5A3:;}}else{
# 3803
return 1;}
return 0;}
# 3807
static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 3810
void*_tmp9CF=Cyc_Absyn_compress_kb(tvar->kind);void*_tmp9D0=_tmp9CF;_LL5AE: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9D1=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9D0;if(_tmp9D1->tag != 2)goto _LL5B0;else{if((_tmp9D1->f2)->kind != Cyc_Absyn_RgnKind)goto _LL5B0;if((_tmp9D1->f2)->aliasqual != Cyc_Absyn_Top)goto _LL5B0;}}_LL5AF:
 goto _LL5B1;_LL5B0: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp9D2=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp9D0;if(_tmp9D2->tag != 0)goto _LL5B2;else{if((_tmp9D2->f1)->kind != Cyc_Absyn_RgnKind)goto _LL5B2;if((_tmp9D2->f1)->aliasqual != Cyc_Absyn_Top)goto _LL5B2;}}_LL5B1:
# 3819
 if(((expected_kind->kind == Cyc_Absyn_BoxKind  || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind) && 
# 3822
Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){
if(expected_kind->aliasqual == Cyc_Absyn_Aliasable)
return& Cyc_Tcutil_rk;else{
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}
# 3828
return& Cyc_Tcutil_trk;_LL5B2:;_LL5B3:
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);_LL5AD:;}
# 3834
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);struct _tuple30{struct Cyc_Tcutil_CVTEnv f1;struct Cyc_List_List*f2;};
# 3838
static struct _tuple30 Cyc_Tcutil_check_clause(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct _dyneither_ptr clause_name,struct Cyc_Absyn_Exp*clause){
# 3841
struct Cyc_List_List*relns=0;
if(clause != 0){
Cyc_Tcexp_tcExp(te,0,clause);
if(!Cyc_Tcutil_is_integral(clause)){
const char*_tmp1276;void*_tmp1275[2];struct Cyc_String_pa_PrintArg_struct _tmp1274;struct Cyc_String_pa_PrintArg_struct _tmp1273;(_tmp1273.tag=0,((_tmp1273.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(clause->topt))),((_tmp1274.tag=0,((_tmp1274.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name),((_tmp1275[0]=& _tmp1274,((_tmp1275[1]=& _tmp1273,Cyc_Tcutil_terr(loc,((_tmp1276="%s clause has type %s instead of integral type",_tag_dyneither(_tmp1276,sizeof(char),47))),_tag_dyneither(_tmp1275,sizeof(void*),2)))))))))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(clause,te,cvtenv);
relns=Cyc_Relations_exp2relns(Cyc_Core_heap_region,clause);
if(!Cyc_Relations_consistent_relations(relns)){
const char*_tmp127B;void*_tmp127A[2];struct Cyc_String_pa_PrintArg_struct _tmp1279;struct Cyc_String_pa_PrintArg_struct _tmp1278;(_tmp1278.tag=0,((_tmp1278.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(clause)),((_tmp1279.tag=0,((_tmp1279.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name),((_tmp127A[0]=& _tmp1279,((_tmp127A[1]=& _tmp1278,Cyc_Tcutil_terr(clause->loc,((_tmp127B="%s clause '%s' may be unsatisfiable",_tag_dyneither(_tmp127B,sizeof(char),36))),_tag_dyneither(_tmp127A,sizeof(void*),2)))))))))))));}}{
# 3853
struct _tuple30 _tmp127C;return(_tmp127C.f1=cvtenv,((_tmp127C.f2=relns,_tmp127C)));};}struct _tuple31{enum Cyc_Absyn_Format_Type f1;void*f2;};
# 3885 "tcutil.cyc"
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect,int allow_abs_aggr){
# 3893
{void*_tmp9DC=Cyc_Tcutil_compress(t);void*_tmp9DD=_tmp9DC;struct Cyc_Core_Opt**_tmp9E0;void**_tmp9E1;struct Cyc_Absyn_Tvar*_tmp9E3;void*_tmp9E5;void***_tmp9E6;struct Cyc_List_List*_tmp9E8;struct _tuple2*_tmp9EA;struct Cyc_Absyn_Enumdecl**_tmp9EB;union Cyc_Absyn_DatatypeInfoU*_tmp9ED;struct Cyc_List_List**_tmp9EE;union Cyc_Absyn_DatatypeFieldInfoU*_tmp9F0;struct Cyc_List_List*_tmp9F1;void*_tmp9F3;struct Cyc_Absyn_Tqual*_tmp9F4;void*_tmp9F5;union Cyc_Absyn_Constraint*_tmp9F6;union Cyc_Absyn_Constraint*_tmp9F7;union Cyc_Absyn_Constraint*_tmp9F8;void*_tmp9FA;struct Cyc_Absyn_Exp*_tmp9FC;struct Cyc_Absyn_Exp*_tmp9FE;void*_tmpA02;struct Cyc_Absyn_Tqual*_tmpA03;struct Cyc_Absyn_Exp**_tmpA04;union Cyc_Absyn_Constraint*_tmpA05;unsigned int _tmpA06;struct Cyc_List_List**_tmpA08;void**_tmpA09;struct Cyc_Absyn_Tqual*_tmpA0A;void*_tmpA0B;struct Cyc_List_List*_tmpA0C;int _tmpA0D;struct Cyc_Absyn_VarargInfo*_tmpA0E;struct Cyc_List_List*_tmpA0F;struct Cyc_List_List*_tmpA10;struct Cyc_Absyn_Exp*_tmpA11;struct Cyc_List_List**_tmpA12;struct Cyc_Absyn_Exp*_tmpA13;struct Cyc_List_List**_tmpA14;struct Cyc_List_List*_tmpA16;enum Cyc_Absyn_AggrKind _tmpA18;struct Cyc_List_List*_tmpA19;union Cyc_Absyn_AggrInfoU*_tmpA1B;struct Cyc_List_List**_tmpA1C;struct _tuple2*_tmpA1E;struct Cyc_List_List**_tmpA1F;struct Cyc_Absyn_Typedefdecl**_tmpA20;void**_tmpA21;void*_tmpA26;void*_tmpA28;void*_tmpA29;void*_tmpA2B;void*_tmpA2D;struct Cyc_List_List*_tmpA2F;_LL5B5: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp9DE=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp9DD;if(_tmp9DE->tag != 0)goto _LL5B7;}_LL5B6:
 goto _LL5B4;_LL5B7: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp9DF=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp9DD;if(_tmp9DF->tag != 1)goto _LL5B9;else{_tmp9E0=(struct Cyc_Core_Opt**)& _tmp9DF->f1;_tmp9E1=(void**)((void**)& _tmp9DF->f2);}}_LL5B8:
# 3897
 if(*_tmp9E0 == 0  || 
Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp9E0))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp9E0))->v,expected_kind))
*_tmp9E0=Cyc_Tcutil_kind_to_opt(expected_kind);
if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){
# 3902
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
*_tmp9E1=(void*)& Cyc_Absyn_UniqueRgn_val;else{
# 3905
*_tmp9E1=(void*)& Cyc_Absyn_HeapRgn_val;}}else{
if(cvtenv.generalize_evars){
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp127F;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp127E;struct Cyc_Absyn_Tvar*_tmpA30=Cyc_Tcutil_new_tvar((void*)((_tmp127E=_cycalloc(sizeof(*_tmp127E)),((_tmp127E[0]=((_tmp127F.tag=2,((_tmp127F.f1=0,((_tmp127F.f2=expected_kind,_tmp127F)))))),_tmp127E)))));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1282;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1281;*_tmp9E1=(void*)((_tmp1281=_cycalloc(sizeof(*_tmp1281)),((_tmp1281[0]=((_tmp1282.tag=2,((_tmp1282.f1=_tmpA30,_tmp1282)))),_tmp1281))));}
_tmp9E3=_tmpA30;goto _LL5BA;}else{
# 3911
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}
# 3913
goto _LL5B4;_LL5B9: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp9E2=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9DD;if(_tmp9E2->tag != 2)goto _LL5BB;else{_tmp9E3=_tmp9E2->f1;}}_LL5BA:
# 3915
{void*_tmpA35=Cyc_Absyn_compress_kb(_tmp9E3->kind);void*_tmpA36=_tmpA35;struct Cyc_Core_Opt**_tmpA38;_LL5EE: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA37=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA36;if(_tmpA37->tag != 1)goto _LL5F0;else{_tmpA38=(struct Cyc_Core_Opt**)& _tmpA37->f1;}}_LL5EF:
# 3917
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1288;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1287;struct Cyc_Core_Opt*_tmp1286;*_tmpA38=((_tmp1286=_cycalloc(sizeof(*_tmp1286)),((_tmp1286->v=(void*)((_tmp1288=_cycalloc(sizeof(*_tmp1288)),((_tmp1288[0]=((_tmp1287.tag=2,((_tmp1287.f1=0,((_tmp1287.f2=expected_kind,_tmp1287)))))),_tmp1288)))),_tmp1286))));}goto _LL5ED;_LL5F0:;_LL5F1:
 goto _LL5ED;_LL5ED:;}
# 3922
cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp9E3);
# 3925
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp9E3,put_in_effect);
# 3927
{void*_tmpA3C=Cyc_Absyn_compress_kb(_tmp9E3->kind);void*_tmpA3D=_tmpA3C;struct Cyc_Core_Opt**_tmpA3F;struct Cyc_Absyn_Kind*_tmpA40;_LL5F3: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA3E=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA3D;if(_tmpA3E->tag != 2)goto _LL5F5;else{_tmpA3F=(struct Cyc_Core_Opt**)& _tmpA3E->f1;_tmpA40=_tmpA3E->f2;}}_LL5F4:
# 3929
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmpA40)){
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp128E;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp128D;struct Cyc_Core_Opt*_tmp128C;*_tmpA3F=((_tmp128C=_cycalloc(sizeof(*_tmp128C)),((_tmp128C->v=(void*)((_tmp128E=_cycalloc(sizeof(*_tmp128E)),((_tmp128E[0]=((_tmp128D.tag=2,((_tmp128D.f1=0,((_tmp128D.f2=expected_kind,_tmp128D)))))),_tmp128E)))),_tmp128C))));}
goto _LL5F2;_LL5F5:;_LL5F6:
 goto _LL5F2;_LL5F2:;}
# 3934
goto _LL5B4;_LL5BB: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp9E4=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp9DD;if(_tmp9E4->tag != 26)goto _LL5BD;else{_tmp9E5=(_tmp9E4->f1)->r;_tmp9E6=(void***)& _tmp9E4->f2;}}_LL5BC: {
# 3940
void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmpA44=_tmp9E5;struct Cyc_Absyn_Aggrdecl*_tmpA46;struct Cyc_Absyn_Enumdecl*_tmpA48;struct Cyc_Absyn_Datatypedecl*_tmpA4A;_LL5F8: {struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmpA45=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmpA44;if(_tmpA45->tag != 0)goto _LL5FA;else{_tmpA46=_tmpA45->f1;}}_LL5F9:
# 3943
 if(te->in_extern_c_include)
_tmpA46->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmpA46);goto _LL5F7;_LL5FA: {struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpA47=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmpA44;if(_tmpA47->tag != 1)goto _LL5FC;else{_tmpA48=_tmpA47->f1;}}_LL5FB:
# 3947
 if(te->in_extern_c_include)
_tmpA48->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmpA48);goto _LL5F7;_LL5FC: {struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmpA49=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmpA44;if(_tmpA49->tag != 2)goto _LL5F7;else{_tmpA4A=_tmpA49->f1;}}_LL5FD:
# 3951
 Cyc_Tc_tcDatatypedecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmpA4A);goto _LL5F7;_LL5F7:;}
# 3953
{void**_tmp128F;*_tmp9E6=((_tmp128F=_cycalloc(sizeof(*_tmp128F)),((_tmp128F[0]=new_t,_tmp128F))));}
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect,allow_abs_aggr);}_LL5BD: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp9E7=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp9DD;if(_tmp9E7->tag != 14)goto _LL5BF;else{_tmp9E8=_tmp9E7->f1;}}_LL5BE: {
# 3959
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct _RegionHandle _tmpA4C=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmpA4C;_push_region(uprev_rgn);{
struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;
for(0;_tmp9E8 != 0;_tmp9E8=_tmp9E8->tl){
struct Cyc_Absyn_Enumfield*_tmpA4D=(struct Cyc_Absyn_Enumfield*)_tmp9E8->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmpA4D->name).f2)){
const char*_tmp1293;void*_tmp1292[1];struct Cyc_String_pa_PrintArg_struct _tmp1291;(_tmp1291.tag=0,((_tmp1291.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpA4D->name).f2),((_tmp1292[0]=& _tmp1291,Cyc_Tcutil_terr(_tmpA4D->loc,((_tmp1293="duplicate enum field name %s",_tag_dyneither(_tmp1293,sizeof(char),29))),_tag_dyneither(_tmp1292,sizeof(void*),1)))))));}else{
# 3968
struct Cyc_List_List*_tmp1294;prev_fields=((_tmp1294=_region_malloc(uprev_rgn,sizeof(*_tmp1294)),((_tmp1294->hd=(*_tmpA4D->name).f2,((_tmp1294->tl=prev_fields,_tmp1294))))));}
# 3970
if(_tmpA4D->tag == 0)
_tmpA4D->tag=Cyc_Absyn_uint_exp(tag_count,_tmpA4D->loc);else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmpA4D->tag))){
const char*_tmp1298;void*_tmp1297[1];struct Cyc_String_pa_PrintArg_struct _tmp1296;(_tmp1296.tag=0,((_tmp1296.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpA4D->name).f2),((_tmp1297[0]=& _tmp1296,Cyc_Tcutil_terr(loc,((_tmp1298="enum field %s: expression is not constant",_tag_dyneither(_tmp1298,sizeof(char),42))),_tag_dyneither(_tmp1297,sizeof(void*),1)))))));}}{
# 3975
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmpA4D->tag))).f1;
tag_count=t1 + 1;
{union Cyc_Absyn_Nmspace _tmpA55=(*_tmpA4D->name).f1;union Cyc_Absyn_Nmspace _tmpA56=_tmpA55;_LL5FF: if((_tmpA56.Rel_n).tag != 1)goto _LL601;_LL600:
# 3979
(*_tmpA4D->name).f1=Cyc_Absyn_Abs_n(te->ns,0);goto _LL5FE;_LL601:;_LL602:
 goto _LL5FE;_LL5FE:;}{
# 3982
struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp129E;struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct _tmp129D;struct _tuple29*_tmp129C;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple29*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmpA4D->name).f2,(
(_tmp129C=_cycalloc(sizeof(*_tmp129C)),((_tmp129C->f1=(void*)((_tmp129E=_cycalloc(sizeof(*_tmp129E)),((_tmp129E[0]=((_tmp129D.tag=4,((_tmp129D.f1=t,((_tmp129D.f2=_tmpA4D,_tmp129D)))))),_tmp129E)))),((_tmp129C->f2=1,_tmp129C)))))));};};}}
# 3986
_npop_handler(0);goto _LL5B4;
# 3960
;_pop_region(uprev_rgn);}_LL5BF: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp9E9=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp9DD;if(_tmp9E9->tag != 13)goto _LL5C1;else{_tmp9EA=_tmp9E9->f1;_tmp9EB=(struct Cyc_Absyn_Enumdecl**)& _tmp9E9->f2;}}_LL5C0:
# 3988
 if(*_tmp9EB == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp9EB))->fields == 0){
struct _handler_cons _tmpA5A;_push_handler(& _tmpA5A);{int _tmpA5C=0;if(setjmp(_tmpA5A.handler))_tmpA5C=1;if(!_tmpA5C){
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp9EA);
*_tmp9EB=*ed;}
# 3990
;_pop_handler();}else{void*_tmpA5B=(void*)_exn_thrown;void*_tmpA5E=_tmpA5B;void*_tmpA60;_LL604: {struct Cyc_Dict_Absent_exn_struct*_tmpA5F=(struct Cyc_Dict_Absent_exn_struct*)_tmpA5E;if(_tmpA5F->tag != Cyc_Dict_Absent)goto _LL606;}_LL605: {
# 3994
struct Cyc_Tcenv_Genv*_tmpA61=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp129F;struct Cyc_Absyn_Enumdecl*_tmpA62=(_tmp129F=_cycalloc(sizeof(*_tmp129F)),((_tmp129F->sc=Cyc_Absyn_Extern,((_tmp129F->name=_tmp9EA,((_tmp129F->fields=0,_tmp129F)))))));
Cyc_Tc_tcEnumdecl(te,_tmpA61,loc,_tmpA62);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp9EA);
*_tmp9EB=*ed;
goto _LL603;};}_LL606: _tmpA60=_tmpA5E;_LL607:(void)_rethrow(_tmpA60);_LL603:;}};}{
# 4003
struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp9EB);
# 4005
*_tmp9EA=(ed->name)[0];
goto _LL5B4;};_LL5C1: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp9EC=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp9DD;if(_tmp9EC->tag != 3)goto _LL5C3;else{_tmp9ED=(union Cyc_Absyn_DatatypeInfoU*)&(_tmp9EC->f1).datatype_info;_tmp9EE=(struct Cyc_List_List**)&(_tmp9EC->f1).targs;}}_LL5C2: {
# 4008
struct Cyc_List_List*_tmpA64=*_tmp9EE;
{union Cyc_Absyn_DatatypeInfoU _tmpA65=*_tmp9ED;union Cyc_Absyn_DatatypeInfoU _tmpA66=_tmpA65;struct _tuple2*_tmpA67;int _tmpA68;struct Cyc_Absyn_Datatypedecl*_tmpA69;_LL609: if((_tmpA66.UnknownDatatype).tag != 1)goto _LL60B;_tmpA67=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpA66.UnknownDatatype).val).name;_tmpA68=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpA66.UnknownDatatype).val).is_extensible;_LL60A: {
# 4011
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmpA6A;_push_handler(& _tmpA6A);{int _tmpA6C=0;if(setjmp(_tmpA6A.handler))_tmpA6C=1;if(!_tmpA6C){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpA67);;_pop_handler();}else{void*_tmpA6B=(void*)_exn_thrown;void*_tmpA6E=_tmpA6B;void*_tmpA70;_LL60E: {struct Cyc_Dict_Absent_exn_struct*_tmpA6F=(struct Cyc_Dict_Absent_exn_struct*)_tmpA6E;if(_tmpA6F->tag != Cyc_Dict_Absent)goto _LL610;}_LL60F: {
# 4015
struct Cyc_Tcenv_Genv*_tmpA71=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Datatypedecl*_tmp12A0;struct Cyc_Absyn_Datatypedecl*_tmpA72=(_tmp12A0=_cycalloc(sizeof(*_tmp12A0)),((_tmp12A0->sc=Cyc_Absyn_Extern,((_tmp12A0->name=_tmpA67,((_tmp12A0->tvs=0,((_tmp12A0->fields=0,((_tmp12A0->is_extensible=_tmpA68,_tmp12A0)))))))))));
Cyc_Tc_tcDatatypedecl(te,_tmpA71,loc,_tmpA72);
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpA67);
# 4020
if(_tmpA64 != 0){
{const char*_tmp12A4;void*_tmp12A3[1];struct Cyc_String_pa_PrintArg_struct _tmp12A2;(_tmp12A2.tag=0,((_tmp12A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA67)),((_tmp12A3[0]=& _tmp12A2,Cyc_Tcutil_terr(loc,((_tmp12A4="declare parameterized datatype %s before using",_tag_dyneither(_tmp12A4,sizeof(char),47))),_tag_dyneither(_tmp12A3,sizeof(void*),1)))))));}
return cvtenv;}
# 4025
goto _LL60D;}_LL610: _tmpA70=_tmpA6E;_LL611:(void)_rethrow(_tmpA70);_LL60D:;}};}
# 4031
if(_tmpA68  && !(*tudp)->is_extensible){
const char*_tmp12A8;void*_tmp12A7[1];struct Cyc_String_pa_PrintArg_struct _tmp12A6;(_tmp12A6.tag=0,((_tmp12A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA67)),((_tmp12A7[0]=& _tmp12A6,Cyc_Tcutil_terr(loc,((_tmp12A8="datatype %s was not declared @extensible",_tag_dyneither(_tmp12A8,sizeof(char),41))),_tag_dyneither(_tmp12A7,sizeof(void*),1)))))));}
*_tmp9ED=Cyc_Absyn_KnownDatatype(tudp);
_tmpA69=*tudp;goto _LL60C;}_LL60B: if((_tmpA66.KnownDatatype).tag != 2)goto _LL608;_tmpA69=*((struct Cyc_Absyn_Datatypedecl**)(_tmpA66.KnownDatatype).val);_LL60C: {
# 4038
struct Cyc_List_List*tvs=_tmpA69->tvs;
for(0;_tmpA64 != 0  && tvs != 0;(_tmpA64=_tmpA64->tl,tvs=tvs->tl)){
void*t=(void*)_tmpA64->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 4044
{struct _tuple0 _tmp12A9;struct _tuple0 _tmpA7A=(_tmp12A9.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp12A9.f2=t,_tmp12A9)));struct _tuple0 _tmpA7B=_tmpA7A;struct Cyc_Absyn_Tvar*_tmpA7E;_LL613:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA7C=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA7B.f1;if(_tmpA7C->tag != 1)goto _LL615;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA7D=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA7B.f2;if(_tmpA7D->tag != 2)goto _LL615;else{_tmpA7E=_tmpA7D->f1;}};_LL614:
# 4046
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpA7E);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpA7E,1);
continue;_LL615:;_LL616:
 goto _LL612;_LL612:;}{
# 4051
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 4055
if(_tmpA64 != 0){
const char*_tmp12AD;void*_tmp12AC[1];struct Cyc_String_pa_PrintArg_struct _tmp12AB;(_tmp12AB.tag=0,((_tmp12AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA69->name)),((_tmp12AC[0]=& _tmp12AB,Cyc_Tcutil_terr(loc,((_tmp12AD="too many type arguments for datatype %s",_tag_dyneither(_tmp12AD,sizeof(char),40))),_tag_dyneither(_tmp12AC,sizeof(void*),1)))))));}
if(tvs != 0){
# 4060
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp12AE;hidden_ts=((_tmp12AE=_cycalloc(sizeof(*_tmp12AE)),((_tmp12AE->hd=e,((_tmp12AE->tl=hidden_ts,_tmp12AE))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1,allow_abs_aggr);}
# 4067
*_tmp9EE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp9EE,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4069
goto _LL608;}_LL608:;}
# 4071
goto _LL5B4;}_LL5C3: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp9EF=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp9DD;if(_tmp9EF->tag != 4)goto _LL5C5;else{_tmp9F0=(union Cyc_Absyn_DatatypeFieldInfoU*)&(_tmp9EF->f1).field_info;_tmp9F1=(_tmp9EF->f1).targs;}}_LL5C4:
# 4074
{union Cyc_Absyn_DatatypeFieldInfoU _tmpA84=*_tmp9F0;union Cyc_Absyn_DatatypeFieldInfoU _tmpA85=_tmpA84;struct _tuple2*_tmpA86;struct _tuple2*_tmpA87;int _tmpA88;struct Cyc_Absyn_Datatypedecl*_tmpA89;struct Cyc_Absyn_Datatypefield*_tmpA8A;_LL618: if((_tmpA85.UnknownDatatypefield).tag != 1)goto _LL61A;_tmpA86=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpA85.UnknownDatatypefield).val).datatype_name;_tmpA87=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpA85.UnknownDatatypefield).val).field_name;_tmpA88=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpA85.UnknownDatatypefield).val).is_extensible;_LL619: {
# 4076
struct Cyc_Absyn_Datatypedecl*tud;
struct Cyc_Absyn_Datatypefield*tuf;
{struct _handler_cons _tmpA8B;_push_handler(& _tmpA8B);{int _tmpA8D=0;if(setjmp(_tmpA8B.handler))_tmpA8D=1;if(!_tmpA8D){*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpA86);;_pop_handler();}else{void*_tmpA8C=(void*)_exn_thrown;void*_tmpA8F=_tmpA8C;void*_tmpA91;_LL61D: {struct Cyc_Dict_Absent_exn_struct*_tmpA90=(struct Cyc_Dict_Absent_exn_struct*)_tmpA8F;if(_tmpA90->tag != Cyc_Dict_Absent)goto _LL61F;}_LL61E:
# 4080
{const char*_tmp12B2;void*_tmp12B1[1];struct Cyc_String_pa_PrintArg_struct _tmp12B0;(_tmp12B0.tag=0,((_tmp12B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA86)),((_tmp12B1[0]=& _tmp12B0,Cyc_Tcutil_terr(loc,((_tmp12B2="unbound datatype %s",_tag_dyneither(_tmp12B2,sizeof(char),20))),_tag_dyneither(_tmp12B1,sizeof(void*),1)))))));}
return cvtenv;_LL61F: _tmpA91=_tmpA8F;_LL620:(void)_rethrow(_tmpA91);_LL61C:;}};}
# 4083
{struct _handler_cons _tmpA95;_push_handler(& _tmpA95);{int _tmpA97=0;if(setjmp(_tmpA95.handler))_tmpA97=1;if(!_tmpA97){
{struct _RegionHandle _tmpA98=_new_region("r");struct _RegionHandle*r=& _tmpA98;_push_region(r);
{void*_tmpA99=Cyc_Tcenv_lookup_ordinary(r,te,loc,_tmpA87,0);void*_tmpA9A=_tmpA99;struct Cyc_Absyn_Datatypedecl*_tmpA9C;struct Cyc_Absyn_Datatypefield*_tmpA9D;_LL622: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpA9B=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpA9A;if(_tmpA9B->tag != 2)goto _LL624;else{_tmpA9C=_tmpA9B->f1;_tmpA9D=_tmpA9B->f2;}}_LL623:
# 4087
 tuf=_tmpA9D;
tud=_tmpA9C;
if(_tmpA88  && !tud->is_extensible){
const char*_tmp12B6;void*_tmp12B5[1];struct Cyc_String_pa_PrintArg_struct _tmp12B4;(_tmp12B4.tag=0,((_tmp12B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA86)),((_tmp12B5[0]=& _tmp12B4,Cyc_Tcutil_terr(loc,((_tmp12B6="datatype %s was not declared @extensible",_tag_dyneither(_tmp12B6,sizeof(char),41))),_tag_dyneither(_tmp12B5,sizeof(void*),1)))))));}
goto _LL621;_LL624:;_LL625:
{const char*_tmp12BB;void*_tmp12BA[2];struct Cyc_String_pa_PrintArg_struct _tmp12B9;struct Cyc_String_pa_PrintArg_struct _tmp12B8;(_tmp12B8.tag=0,((_tmp12B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA86)),((_tmp12B9.tag=0,((_tmp12B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA87)),((_tmp12BA[0]=& _tmp12B9,((_tmp12BA[1]=& _tmp12B8,Cyc_Tcutil_terr(loc,((_tmp12BB="unbound field %s in type datatype %s",_tag_dyneither(_tmp12BB,sizeof(char),37))),_tag_dyneither(_tmp12BA,sizeof(void*),2)))))))))))));}{
struct Cyc_Tcutil_CVTEnv _tmpAA5=cvtenv;_npop_handler(1);return _tmpAA5;};_LL621:;}
# 4085
;_pop_region(r);}
# 4084
;_pop_handler();}else{void*_tmpA96=(void*)_exn_thrown;void*_tmpAA7=_tmpA96;void*_tmpAA9;_LL627: {struct Cyc_Dict_Absent_exn_struct*_tmpAA8=(struct Cyc_Dict_Absent_exn_struct*)_tmpAA7;if(_tmpAA8->tag != Cyc_Dict_Absent)goto _LL629;}_LL628:
# 4100
{const char*_tmp12C0;void*_tmp12BF[2];struct Cyc_String_pa_PrintArg_struct _tmp12BE;struct Cyc_String_pa_PrintArg_struct _tmp12BD;(_tmp12BD.tag=0,((_tmp12BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA86)),((_tmp12BE.tag=0,((_tmp12BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA87)),((_tmp12BF[0]=& _tmp12BE,((_tmp12BF[1]=& _tmp12BD,Cyc_Tcutil_terr(loc,((_tmp12C0="unbound field %s in type datatype %s",_tag_dyneither(_tmp12C0,sizeof(char),37))),_tag_dyneither(_tmp12BF,sizeof(void*),2)))))))))))));}
return cvtenv;_LL629: _tmpAA9=_tmpAA7;_LL62A:(void)_rethrow(_tmpAA9);_LL626:;}};}
# 4104
*_tmp9F0=Cyc_Absyn_KnownDatatypefield(tud,tuf);
_tmpA89=tud;_tmpA8A=tuf;goto _LL61B;}_LL61A: if((_tmpA85.KnownDatatypefield).tag != 2)goto _LL617;_tmpA89=((struct _tuple3)(_tmpA85.KnownDatatypefield).val).f1;_tmpA8A=((struct _tuple3)(_tmpA85.KnownDatatypefield).val).f2;_LL61B: {
# 4108
struct Cyc_List_List*tvs=_tmpA89->tvs;
for(0;_tmp9F1 != 0  && tvs != 0;(_tmp9F1=_tmp9F1->tl,tvs=tvs->tl)){
void*t=(void*)_tmp9F1->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 4114
{struct _tuple0 _tmp12C1;struct _tuple0 _tmpAAE=(_tmp12C1.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp12C1.f2=t,_tmp12C1)));struct _tuple0 _tmpAAF=_tmpAAE;struct Cyc_Absyn_Tvar*_tmpAB2;_LL62C:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpAB0=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpAAF.f1;if(_tmpAB0->tag != 1)goto _LL62E;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpAB1=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpAAF.f2;if(_tmpAB1->tag != 2)goto _LL62E;else{_tmpAB2=_tmpAB1->f1;}};_LL62D:
# 4116
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpAB2);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpAB2,1);
continue;_LL62E:;_LL62F:
 goto _LL62B;_LL62B:;}{
# 4121
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 4125
if(_tmp9F1 != 0){
const char*_tmp12C6;void*_tmp12C5[2];struct Cyc_String_pa_PrintArg_struct _tmp12C4;struct Cyc_String_pa_PrintArg_struct _tmp12C3;(_tmp12C3.tag=0,((_tmp12C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA8A->name)),((_tmp12C4.tag=0,((_tmp12C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA89->name)),((_tmp12C5[0]=& _tmp12C4,((_tmp12C5[1]=& _tmp12C3,Cyc_Tcutil_terr(loc,((_tmp12C6="too many type arguments for datatype %s.%s",_tag_dyneither(_tmp12C6,sizeof(char),43))),_tag_dyneither(_tmp12C5,sizeof(void*),2)))))))))))));}
if(tvs != 0){
const char*_tmp12CB;void*_tmp12CA[2];struct Cyc_String_pa_PrintArg_struct _tmp12C9;struct Cyc_String_pa_PrintArg_struct _tmp12C8;(_tmp12C8.tag=0,((_tmp12C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA8A->name)),((_tmp12C9.tag=0,((_tmp12C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA89->name)),((_tmp12CA[0]=& _tmp12C9,((_tmp12CA[1]=& _tmp12C8,Cyc_Tcutil_terr(loc,((_tmp12CB="too few type arguments for datatype %s.%s",_tag_dyneither(_tmp12CB,sizeof(char),42))),_tag_dyneither(_tmp12CA,sizeof(void*),2)))))))))))));}
goto _LL617;}_LL617:;}
# 4133
goto _LL5B4;_LL5C5: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9F2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9DD;if(_tmp9F2->tag != 5)goto _LL5C7;else{_tmp9F3=(_tmp9F2->f1).elt_typ;_tmp9F4=(struct Cyc_Absyn_Tqual*)&(_tmp9F2->f1).elt_tq;_tmp9F5=((_tmp9F2->f1).ptr_atts).rgn;_tmp9F6=((_tmp9F2->f1).ptr_atts).nullable;_tmp9F7=((_tmp9F2->f1).ptr_atts).bounds;_tmp9F8=((_tmp9F2->f1).ptr_atts).zero_term;}}_LL5C6: {
# 4136
int is_zero_terminated;
# 4138
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp9F3,1,1);
_tmp9F4->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp9F4->print_const,_tmp9F3);{
struct Cyc_Absyn_Kind*k;
{enum Cyc_Absyn_AliasQual _tmpABC=expected_kind->aliasqual;switch(_tmpABC){case Cyc_Absyn_Aliasable: _LL630:
 k=& Cyc_Tcutil_rk;break;case Cyc_Absyn_Unique: _LL631:
 k=& Cyc_Tcutil_urk;break;case Cyc_Absyn_Top: _LL632:
 k=& Cyc_Tcutil_trk;break;}}
# 4146
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmp9F5,1,1);
{union Cyc_Absyn_Constraint*_tmpABD=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp9F8);union Cyc_Absyn_Constraint*_tmpABE=_tmpABD;_LL635: if((_tmpABE->No_constr).tag != 3)goto _LL637;_LL636:
# 4151
{void*_tmpABF=Cyc_Tcutil_compress(_tmp9F3);void*_tmpAC0=_tmpABF;_LL63C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpAC1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpAC0;if(_tmpAC1->tag != 6)goto _LL63E;else{if(_tmpAC1->f2 != Cyc_Absyn_Char_sz)goto _LL63E;}}_LL63D:
# 4153
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp9F8,Cyc_Absyn_true_conref);
is_zero_terminated=1;
goto _LL63B;_LL63E:;_LL63F:
# 4157
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp9F8,Cyc_Absyn_false_conref);
is_zero_terminated=0;
goto _LL63B;_LL63B:;}
# 4161
goto _LL634;_LL637: if((_tmpABE->Eq_constr).tag != 1)goto _LL639;if((int)(_tmpABE->Eq_constr).val != 1)goto _LL639;_LL638:
# 4164
 if(!Cyc_Tcutil_admits_zero(_tmp9F3)){
const char*_tmp12CF;void*_tmp12CE[1];struct Cyc_String_pa_PrintArg_struct _tmp12CD;(_tmp12CD.tag=0,((_tmp12CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp9F3)),((_tmp12CE[0]=& _tmp12CD,Cyc_Tcutil_terr(loc,((_tmp12CF="cannot have a pointer to zero-terminated %s elements",_tag_dyneither(_tmp12CF,sizeof(char),53))),_tag_dyneither(_tmp12CE,sizeof(void*),1)))))));}
is_zero_terminated=1;
goto _LL634;_LL639:;_LL63A:
# 4170
 is_zero_terminated=0;
goto _LL634;_LL634:;}
# 4174
{void*_tmpAC5=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp9F7);void*_tmpAC6=_tmpAC5;struct Cyc_Absyn_Exp*_tmpAC9;_LL641: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpAC7=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpAC6;if(_tmpAC7->tag != 0)goto _LL643;}_LL642:
 goto _LL640;_LL643: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpAC8=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpAC6;if(_tmpAC8->tag != 1)goto _LL640;else{_tmpAC9=_tmpAC8->f1;}}_LL644: {
# 4177
struct _RegionHandle _tmpACA=_new_region("temp");struct _RegionHandle*temp=& _tmpACA;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmpACB=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmpACB,0,_tmpAC9);}
# 4181
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpAC9,te,cvtenv);
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpAC9)){
const char*_tmp12D2;void*_tmp12D1;(_tmp12D1=0,Cyc_Tcutil_terr(loc,((_tmp12D2="pointer bounds expression is not an unsigned int",_tag_dyneither(_tmp12D2,sizeof(char),49))),_tag_dyneither(_tmp12D1,sizeof(void*),0)));}{
struct _tuple14 _tmpACE=Cyc_Evexp_eval_const_uint_exp(_tmpAC9);unsigned int _tmpAD0;int _tmpAD1;struct _tuple14 _tmpACF=_tmpACE;_tmpAD0=_tmpACF.f1;_tmpAD1=_tmpACF.f2;
if(is_zero_terminated  && (!_tmpAD1  || _tmpAD0 < 1)){
const char*_tmp12D5;void*_tmp12D4;(_tmp12D4=0,Cyc_Tcutil_terr(loc,((_tmp12D5="zero-terminated pointer cannot point to empty sequence",_tag_dyneither(_tmp12D5,sizeof(char),55))),_tag_dyneither(_tmp12D4,sizeof(void*),0)));}
_npop_handler(0);goto _LL640;};
# 4177
;_pop_region(temp);}_LL640:;}
# 4189
goto _LL5B4;};}_LL5C7: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp9F9=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp9DD;if(_tmp9F9->tag != 19)goto _LL5C9;else{_tmp9FA=(void*)_tmp9F9->f1;}}_LL5C8:
# 4191
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp9FA,1,1);goto _LL5B4;_LL5C9: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp9FB=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp9DD;if(_tmp9FB->tag != 18)goto _LL5CB;else{_tmp9FC=_tmp9FB->f1;}}_LL5CA: {
# 4196
struct _RegionHandle _tmpAD4=_new_region("temp");struct _RegionHandle*temp=& _tmpAD4;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmpAD5=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmpAD5,0,_tmp9FC);}
# 4200
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp9FC)){
const char*_tmp12D8;void*_tmp12D7;(_tmp12D7=0,Cyc_Tcutil_terr(loc,((_tmp12D8="valueof_t requires an int expression",_tag_dyneither(_tmp12D8,sizeof(char),37))),_tag_dyneither(_tmp12D7,sizeof(void*),0)));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp9FC,te,cvtenv);
_npop_handler(0);goto _LL5B4;
# 4196
;_pop_region(temp);}_LL5CB: {struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp9FD=(struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp9DD;if(_tmp9FD->tag != 27)goto _LL5CD;else{_tmp9FE=_tmp9FD->f1;}}_LL5CC: {
# 4208
struct _RegionHandle _tmpAD8=_new_region("temp");struct _RegionHandle*temp=& _tmpAD8;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmpAD9=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmpAD9,0,_tmp9FE);}
# 4212
_npop_handler(0);goto _LL5B4;
# 4208
;_pop_region(temp);}_LL5CD: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp9FF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp9DD;if(_tmp9FF->tag != 6)goto _LL5CF;}_LL5CE:
# 4213
 goto _LL5D0;_LL5CF: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpA00=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp9DD;if(_tmpA00->tag != 7)goto _LL5D1;}_LL5D0:
 goto _LL5B4;_LL5D1: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpA01=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9DD;if(_tmpA01->tag != 8)goto _LL5D3;else{_tmpA02=(_tmpA01->f1).elt_type;_tmpA03=(struct Cyc_Absyn_Tqual*)&(_tmpA01->f1).tq;_tmpA04=(struct Cyc_Absyn_Exp**)&(_tmpA01->f1).num_elts;_tmpA05=(_tmpA01->f1).zero_term;_tmpA06=(_tmpA01->f1).zt_loc;}}_LL5D2: {
# 4218
struct Cyc_Absyn_Exp*_tmpADA=*_tmpA04;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpA02,1,allow_abs_aggr);
_tmpA03->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpA03->print_const,_tmpA02);{
int is_zero_terminated;
{union Cyc_Absyn_Constraint*_tmpADB=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmpA05);union Cyc_Absyn_Constraint*_tmpADC=_tmpADB;_LL646: if((_tmpADC->No_constr).tag != 3)goto _LL648;_LL647:
# 4225
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpA05,Cyc_Absyn_false_conref);
is_zero_terminated=0;
# 4240 "tcutil.cyc"
goto _LL645;_LL648: if((_tmpADC->Eq_constr).tag != 1)goto _LL64A;if((int)(_tmpADC->Eq_constr).val != 1)goto _LL64A;_LL649:
# 4243
 if(!Cyc_Tcutil_admits_zero(_tmpA02)){
const char*_tmp12DC;void*_tmp12DB[1];struct Cyc_String_pa_PrintArg_struct _tmp12DA;(_tmp12DA.tag=0,((_tmp12DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmpA02)),((_tmp12DB[0]=& _tmp12DA,Cyc_Tcutil_terr(loc,((_tmp12DC="cannot have a zero-terminated array of %s elements",_tag_dyneither(_tmp12DC,sizeof(char),51))),_tag_dyneither(_tmp12DB,sizeof(void*),1)))))));}
is_zero_terminated=1;
goto _LL645;_LL64A:;_LL64B:
# 4249
 is_zero_terminated=0;
goto _LL645;_LL645:;}
# 4254
if(_tmpADA == 0){
# 4256
if(is_zero_terminated)
# 4258
*_tmpA04=(_tmpADA=Cyc_Absyn_uint_exp(1,0));else{
# 4261
{const char*_tmp12DF;void*_tmp12DE;(_tmp12DE=0,Cyc_Tcutil_warn(loc,((_tmp12DF="array bound defaults to 1 here",_tag_dyneither(_tmp12DF,sizeof(char),31))),_tag_dyneither(_tmp12DE,sizeof(void*),0)));}
*_tmpA04=(_tmpADA=Cyc_Absyn_uint_exp(1,0));}}{
# 4265
struct _RegionHandle _tmpAE2=_new_region("temp");struct _RegionHandle*temp=& _tmpAE2;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmpAE3=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmpAE3,0,_tmpADA);}
# 4269
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpADA)){
const char*_tmp12E2;void*_tmp12E1;(_tmp12E1=0,Cyc_Tcutil_terr(loc,((_tmp12E2="array bounds expression is not an unsigned int",_tag_dyneither(_tmp12E2,sizeof(char),47))),_tag_dyneither(_tmp12E1,sizeof(void*),0)));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpADA,te,cvtenv);{
# 4276
struct _tuple14 _tmpAE6=Cyc_Evexp_eval_const_uint_exp(_tmpADA);unsigned int _tmpAE8;int _tmpAE9;struct _tuple14 _tmpAE7=_tmpAE6;_tmpAE8=_tmpAE7.f1;_tmpAE9=_tmpAE7.f2;
# 4278
if((is_zero_terminated  && _tmpAE9) && _tmpAE8 < 1){
const char*_tmp12E5;void*_tmp12E4;(_tmp12E4=0,Cyc_Tcutil_warn(loc,((_tmp12E5="zero terminated array cannot have zero elements",_tag_dyneither(_tmp12E5,sizeof(char),48))),_tag_dyneither(_tmp12E4,sizeof(void*),0)));}
# 4281
if((_tmpAE9  && _tmpAE8 < 1) && Cyc_Cyclone_tovc_r){
{const char*_tmp12E8;void*_tmp12E7;(_tmp12E7=0,Cyc_Tcutil_warn(loc,((_tmp12E8="arrays with 0 elements are not supported except with gcc -- changing to 1.",_tag_dyneither(_tmp12E8,sizeof(char),75))),_tag_dyneither(_tmp12E7,sizeof(void*),0)));}
*_tmpA04=Cyc_Absyn_uint_exp(1,0);}
# 4285
_npop_handler(0);goto _LL5B4;};
# 4265
;_pop_region(temp);};};}_LL5D3: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpA07=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9DD;if(_tmpA07->tag != 9)goto _LL5D5;else{_tmpA08=(struct Cyc_List_List**)&(_tmpA07->f1).tvars;_tmpA09=(void**)&(_tmpA07->f1).effect;_tmpA0A=(struct Cyc_Absyn_Tqual*)&(_tmpA07->f1).ret_tqual;_tmpA0B=(_tmpA07->f1).ret_typ;_tmpA0C=(_tmpA07->f1).args;_tmpA0D=(_tmpA07->f1).c_varargs;_tmpA0E=(_tmpA07->f1).cyc_varargs;_tmpA0F=(_tmpA07->f1).rgn_po;_tmpA10=(_tmpA07->f1).attributes;_tmpA11=(_tmpA07->f1).requires_clause;_tmpA12=(struct Cyc_List_List**)&(_tmpA07->f1).requires_relns;_tmpA13=(_tmpA07->f1).ensures_clause;_tmpA14=(struct Cyc_List_List**)&(_tmpA07->f1).ensures_relns;}}_LL5D4: {
# 4292
int num_convs=0;
int seen_cdecl=0;
int seen_stdcall=0;
int seen_fastcall=0;
int seen_format=0;
enum Cyc_Absyn_Format_Type ft=Cyc_Absyn_Printf_ft;
int fmt_desc_arg=-1;
int fmt_arg_start=-1;
for(0;_tmpA10 != 0;_tmpA10=_tmpA10->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmpA10->hd)){
const char*_tmp12EC;void*_tmp12EB[1];struct Cyc_String_pa_PrintArg_struct _tmp12EA;(_tmp12EA.tag=0,((_tmp12EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmpA10->hd)),((_tmp12EB[0]=& _tmp12EA,Cyc_Tcutil_terr(loc,((_tmp12EC="bad function type attribute %s",_tag_dyneither(_tmp12EC,sizeof(char),31))),_tag_dyneither(_tmp12EB,sizeof(void*),1)))))));}{
void*_tmpAF1=(void*)_tmpA10->hd;void*_tmpAF2=_tmpAF1;enum Cyc_Absyn_Format_Type _tmpAF7;int _tmpAF8;int _tmpAF9;_LL64D: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpAF3=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpAF2;if(_tmpAF3->tag != 1)goto _LL64F;}_LL64E:
# 4305
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL64C;_LL64F: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpAF4=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpAF2;if(_tmpAF4->tag != 2)goto _LL651;}_LL650:
# 4307
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL64C;_LL651: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpAF5=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpAF2;if(_tmpAF5->tag != 3)goto _LL653;}_LL652:
# 4309
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL64C;_LL653: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpAF6=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpAF2;if(_tmpAF6->tag != 19)goto _LL655;else{_tmpAF7=_tmpAF6->f1;_tmpAF8=_tmpAF6->f2;_tmpAF9=_tmpAF6->f3;}}_LL654:
# 4311
 if(!seen_format){
seen_format=1;
ft=_tmpAF7;
fmt_desc_arg=_tmpAF8;
fmt_arg_start=_tmpAF9;}else{
# 4317
const char*_tmp12EF;void*_tmp12EE;(_tmp12EE=0,Cyc_Tcutil_terr(loc,((_tmp12EF="function can't have multiple format attributes",_tag_dyneither(_tmp12EF,sizeof(char),47))),_tag_dyneither(_tmp12EE,sizeof(void*),0)));}
goto _LL64C;_LL655:;_LL656:
 goto _LL64C;_LL64C:;};}
# 4322
if(num_convs > 1){
const char*_tmp12F2;void*_tmp12F1;(_tmp12F1=0,Cyc_Tcutil_terr(loc,((_tmp12F2="function can't have multiple calling conventions",_tag_dyneither(_tmp12F2,sizeof(char),49))),_tag_dyneither(_tmp12F1,sizeof(void*),0)));}
# 4327
Cyc_Tcutil_check_unique_tvars(loc,*_tmpA08);
{struct Cyc_List_List*b=*_tmpA08;for(0;b != 0;b=b->tl){
((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{
void*_tmpAFE=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmpAFF=_tmpAFE;_LL658: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpB00=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpAFF;if(_tmpB00->tag != 0)goto _LL65A;else{if((_tmpB00->f1)->kind != Cyc_Absyn_MemKind)goto _LL65A;}}_LL659:
# 4333
{const char*_tmp12F6;void*_tmp12F5[1];struct Cyc_String_pa_PrintArg_struct _tmp12F4;(_tmp12F4.tag=0,((_tmp12F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name),((_tmp12F5[0]=& _tmp12F4,Cyc_Tcutil_terr(loc,((_tmp12F6="function attempts to abstract Mem type variable %s",_tag_dyneither(_tmp12F6,sizeof(char),51))),_tag_dyneither(_tmp12F5,sizeof(void*),1)))))));}
goto _LL657;_LL65A:;_LL65B:
 goto _LL657;_LL657:;};}}{
# 4341
struct _RegionHandle _tmpB04=_new_region("newr");struct _RegionHandle*newr=& _tmpB04;_push_region(newr);{
struct Cyc_Tcutil_CVTEnv _tmp12F7;struct Cyc_Tcutil_CVTEnv _tmpB05=
(_tmp12F7.r=newr,((_tmp12F7.kind_env=cvtenv.kind_env,((_tmp12F7.free_vars=0,((_tmp12F7.free_evars=0,((_tmp12F7.generalize_evars=cvtenv.generalize_evars,((_tmp12F7.fn_result=1,_tmp12F7)))))))))));
# 4347
_tmpB05=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpB05,& Cyc_Tcutil_tmk,_tmpA0B,1,1);
_tmpA0A->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpA0A->print_const,_tmpA0B);
_tmpB05.fn_result=0;
{struct Cyc_List_List*a=_tmpA0C;for(0;a != 0;a=a->tl){
struct _tuple10*_tmpB06=(struct _tuple10*)a->hd;
void*_tmpB07=(*_tmpB06).f3;
_tmpB05=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpB05,& Cyc_Tcutil_tmk,_tmpB07,1,1);{
int _tmpB08=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpB06).f2).print_const,_tmpB07);
((*_tmpB06).f2).real_const=_tmpB08;
# 4358
if(Cyc_Tcutil_is_array(_tmpB07)){
# 4361
void*_tmpB09=Cyc_Absyn_new_evar(0,0);
_tmpB05=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpB05,& Cyc_Tcutil_rk,_tmpB09,1,1);
(*_tmpB06).f3=Cyc_Tcutil_promote_array(_tmpB07,_tmpB09,0);}};}}
# 4368
if(_tmpA0E != 0){
if(_tmpA0D){const char*_tmp12FA;void*_tmp12F9;(_tmp12F9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12FA="both c_vararg and cyc_vararg",_tag_dyneither(_tmp12FA,sizeof(char),29))),_tag_dyneither(_tmp12F9,sizeof(void*),0)));}{
struct Cyc_Absyn_VarargInfo _tmpB0C=*_tmpA0E;void*_tmpB0E;int _tmpB0F;struct Cyc_Absyn_VarargInfo _tmpB0D=_tmpB0C;_tmpB0E=_tmpB0D.type;_tmpB0F=_tmpB0D.inject;
_tmpB05=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpB05,& Cyc_Tcutil_tmk,_tmpB0E,1,1);
(_tmpA0E->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmpA0E->tq).print_const,_tmpB0E);
# 4374
if(_tmpB0F){
void*_tmpB10=Cyc_Tcutil_compress(_tmpB0E);void*_tmpB11=_tmpB10;void*_tmpB13;union Cyc_Absyn_Constraint*_tmpB14;union Cyc_Absyn_Constraint*_tmpB15;_LL65D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpB12=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB11;if(_tmpB12->tag != 5)goto _LL65F;else{_tmpB13=(_tmpB12->f1).elt_typ;_tmpB14=((_tmpB12->f1).ptr_atts).bounds;_tmpB15=((_tmpB12->f1).ptr_atts).zero_term;}}_LL65E:
# 4377
{void*_tmpB16=Cyc_Tcutil_compress(_tmpB13);void*_tmpB17=_tmpB16;_LL662: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpB18=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpB17;if(_tmpB18->tag != 3)goto _LL664;}_LL663:
# 4379
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmpB15)){
const char*_tmp12FD;void*_tmp12FC;(_tmp12FC=0,Cyc_Tcutil_terr(loc,((_tmp12FD="can't inject into a zeroterm pointer",_tag_dyneither(_tmp12FD,sizeof(char),37))),_tag_dyneither(_tmp12FC,sizeof(void*),0)));}
{void*_tmpB1B=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmpB14);void*_tmpB1C=_tmpB1B;_LL667: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpB1D=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpB1C;if(_tmpB1D->tag != 0)goto _LL669;}_LL668:
# 4383
{const char*_tmp1300;void*_tmp12FF;(_tmp12FF=0,Cyc_Tcutil_terr(loc,((_tmp1300="can't inject into a fat pointer to datatype",_tag_dyneither(_tmp1300,sizeof(char),44))),_tag_dyneither(_tmp12FF,sizeof(void*),0)));}
goto _LL666;_LL669:;_LL66A:
 goto _LL666;_LL666:;}
# 4387
goto _LL661;_LL664:;_LL665:
{const char*_tmp1303;void*_tmp1302;(_tmp1302=0,Cyc_Tcutil_terr(loc,((_tmp1303="can't inject a non-datatype type",_tag_dyneither(_tmp1303,sizeof(char),33))),_tag_dyneither(_tmp1302,sizeof(void*),0)));}goto _LL661;_LL661:;}
# 4390
goto _LL65C;_LL65F:;_LL660:
{const char*_tmp1306;void*_tmp1305;(_tmp1305=0,Cyc_Tcutil_terr(loc,((_tmp1306="expecting a datatype pointer type",_tag_dyneither(_tmp1306,sizeof(char),34))),_tag_dyneither(_tmp1305,sizeof(void*),0)));}goto _LL65C;_LL65C:;}};}
# 4396
if(seen_format){
int _tmpB24=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpA0C);
if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmpB24) || fmt_arg_start < 0) || 
# 4400
(_tmpA0E == 0  && !_tmpA0D) && fmt_arg_start != 0) || 
(_tmpA0E != 0  || _tmpA0D) && fmt_arg_start != _tmpB24 + 1){
# 4403
const char*_tmp1309;void*_tmp1308;(_tmp1308=0,Cyc_Tcutil_terr(loc,((_tmp1309="bad format descriptor",_tag_dyneither(_tmp1309,sizeof(char),22))),_tag_dyneither(_tmp1308,sizeof(void*),0)));}else{
# 4406
struct _tuple10 _tmpB27=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmpA0C,fmt_desc_arg - 1);void*_tmpB29;struct _tuple10 _tmpB28=_tmpB27;_tmpB29=_tmpB28.f3;
# 4408
{void*_tmpB2A=Cyc_Tcutil_compress(_tmpB29);void*_tmpB2B=_tmpB2A;void*_tmpB2D;union Cyc_Absyn_Constraint*_tmpB2E;union Cyc_Absyn_Constraint*_tmpB2F;_LL66C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpB2C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB2B;if(_tmpB2C->tag != 5)goto _LL66E;else{_tmpB2D=(_tmpB2C->f1).elt_typ;_tmpB2E=((_tmpB2C->f1).ptr_atts).bounds;_tmpB2F=((_tmpB2C->f1).ptr_atts).zero_term;}}_LL66D:
# 4411
{struct _tuple0 _tmp130A;struct _tuple0 _tmpB30=(_tmp130A.f1=Cyc_Tcutil_compress(_tmpB2D),((_tmp130A.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpB2E),_tmp130A)));struct _tuple0 _tmpB31=_tmpB30;_LL671:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpB32=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpB31.f1;if(_tmpB32->tag != 6)goto _LL673;else{if(_tmpB32->f2 != Cyc_Absyn_Char_sz)goto _LL673;}}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpB33=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpB31.f2;if(_tmpB33->tag != 0)goto _LL673;};_LL672:
# 4413
 if(_tmpA0D){
const char*_tmp130D;void*_tmp130C;(_tmp130C=0,Cyc_Tcutil_terr(loc,((_tmp130D="format descriptor is not a char * type",_tag_dyneither(_tmp130D,sizeof(char),39))),_tag_dyneither(_tmp130C,sizeof(void*),0)));}
goto _LL670;_LL673: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpB34=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpB31.f1;if(_tmpB34->tag != 6)goto _LL675;else{if(_tmpB34->f2 != Cyc_Absyn_Char_sz)goto _LL675;}}_LL674:
# 4417
 if(!_tmpA0D){
const char*_tmp1310;void*_tmp130F;(_tmp130F=0,Cyc_Tcutil_terr(loc,((_tmp1310="format descriptor is not a char ? type",_tag_dyneither(_tmp1310,sizeof(char),39))),_tag_dyneither(_tmp130F,sizeof(void*),0)));}
goto _LL670;_LL675:;_LL676:
# 4421
{const char*_tmp1313;void*_tmp1312;(_tmp1312=0,Cyc_Tcutil_terr(loc,((_tmp1313="format descriptor is not a string type",_tag_dyneither(_tmp1313,sizeof(char),39))),_tag_dyneither(_tmp1312,sizeof(void*),0)));}
goto _LL670;_LL670:;}
# 4424
goto _LL66B;_LL66E:;_LL66F:
{const char*_tmp1316;void*_tmp1315;(_tmp1315=0,Cyc_Tcutil_terr(loc,((_tmp1316="format descriptor is not a string type",_tag_dyneither(_tmp1316,sizeof(char),39))),_tag_dyneither(_tmp1315,sizeof(void*),0)));}goto _LL66B;_LL66B:;}
# 4427
if(fmt_arg_start != 0  && !_tmpA0D){
# 4431
int problem;
{struct _tuple31 _tmp1317;struct _tuple31 _tmpB3E=(_tmp1317.f1=ft,((_tmp1317.f2=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmpA0E))->type)),_tmp1317)));struct _tuple31 _tmpB3F=_tmpB3E;struct Cyc_Absyn_Datatypedecl*_tmpB41;struct Cyc_Absyn_Datatypedecl*_tmpB43;_LL678: if(_tmpB3F.f1 != Cyc_Absyn_Printf_ft)goto _LL67A;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpB40=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpB3F.f2;if(_tmpB40->tag != 3)goto _LL67A;else{if((((_tmpB40->f1).datatype_info).KnownDatatype).tag != 2)goto _LL67A;_tmpB41=*((struct Cyc_Absyn_Datatypedecl**)(((_tmpB40->f1).datatype_info).KnownDatatype).val);}};_LL679:
# 4434
 problem=Cyc_Absyn_qvar_cmp(_tmpB41->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL677;_LL67A: if(_tmpB3F.f1 != Cyc_Absyn_Scanf_ft)goto _LL67C;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpB42=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpB3F.f2;if(_tmpB42->tag != 3)goto _LL67C;else{if((((_tmpB42->f1).datatype_info).KnownDatatype).tag != 2)goto _LL67C;_tmpB43=*((struct Cyc_Absyn_Datatypedecl**)(((_tmpB42->f1).datatype_info).KnownDatatype).val);}};_LL67B:
# 4436
 problem=Cyc_Absyn_qvar_cmp(_tmpB43->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL677;_LL67C:;_LL67D:
# 4438
 problem=1;goto _LL677;_LL677:;}
# 4440
if(problem){
const char*_tmp131A;void*_tmp1319;(_tmp1319=0,Cyc_Tcutil_terr(loc,((_tmp131A="format attribute and vararg types don't match",_tag_dyneither(_tmp131A,sizeof(char),46))),_tag_dyneither(_tmp1319,sizeof(void*),0)));}}}}
# 4448
{struct Cyc_List_List*rpo=_tmpA0F;for(0;rpo != 0;rpo=rpo->tl){
struct _tuple0*_tmpB47=(struct _tuple0*)rpo->hd;void*_tmpB49;void*_tmpB4A;struct _tuple0*_tmpB48=_tmpB47;_tmpB49=_tmpB48->f1;_tmpB4A=_tmpB48->f2;
_tmpB05=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpB05,& Cyc_Tcutil_ek,_tmpB49,1,1);
_tmpB05=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpB05,& Cyc_Tcutil_trk,_tmpB4A,1,1);}}{
# 4456
struct Cyc_Tcenv_Fenv*_tmpB4B=Cyc_Tcenv_bogus_fenv(newr,_tmpA0B,_tmpA0C);
struct Cyc_Tcenv_Tenv*_tmp131B;struct Cyc_Tcenv_Tenv*_tmpB4C=(_tmp131B=_region_malloc(newr,sizeof(*_tmp131B)),((_tmp131B->ns=te->ns,((_tmp131B->ae=te->ae,((_tmp131B->le=_tmpB4B,((_tmp131B->allow_valueof=1,((_tmp131B->in_extern_c_include=te->in_extern_c_include,_tmp131B)))))))))));
const char*_tmp131C;struct _tuple30 _tmpB4D=Cyc_Tcutil_check_clause(loc,_tmpB4C,_tmpB05,((_tmp131C="@requires",_tag_dyneither(_tmp131C,sizeof(char),10))),_tmpA11);struct Cyc_Tcutil_CVTEnv _tmpB4F;struct Cyc_List_List*_tmpB50;struct _tuple30 _tmpB4E=_tmpB4D;_tmpB4F=_tmpB4E.f1;_tmpB50=_tmpB4E.f2;
_tmpB05=_tmpB4F;
*_tmpA12=_tmpB50;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmpA0C,_tmpB50);{
# 4463
struct _dyneither_ptr*_tmpB51=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"return_value",sizeof(char),13);
struct _tuple2*_tmp131F;struct Cyc_Absyn_Vardecl*_tmp131E;struct Cyc_Absyn_Vardecl*_tmpB52=(_tmp131E=_cycalloc(sizeof(*_tmp131E)),((_tmp131E->sc=Cyc_Absyn_Public,((_tmp131E->name=((_tmp131F=_cycalloc(sizeof(*_tmp131F)),((_tmp131F->f1=Cyc_Absyn_Loc_n,((_tmp131F->f2=_tmpB51,_tmp131F)))))),((_tmp131E->tq=
Cyc_Absyn_empty_tqual(0),((_tmp131E->type=_tmpA0B,((_tmp131E->initializer=0,((_tmp131E->rgn=
Cyc_Tcenv_curr_rgn(_tmpB4C),((_tmp131E->attributes=0,((_tmp131E->escapes=0,_tmp131E)))))))))))))))));
# 4468
_tmpB4C=Cyc_Tcenv_add_local_var(newr,0,_tmpB4C,_tmpB52);{
const char*_tmp1320;struct _tuple30 _tmpB53=Cyc_Tcutil_check_clause(loc,_tmpB4C,_tmpB05,((_tmp1320="@ensures",_tag_dyneither(_tmp1320,sizeof(char),9))),_tmpA13);struct Cyc_Tcutil_CVTEnv _tmpB55;struct Cyc_List_List*_tmpB56;struct _tuple30 _tmpB54=_tmpB53;_tmpB55=_tmpB54.f1;_tmpB56=_tmpB54.f2;
_tmpB05=_tmpB55;
*_tmpA14=_tmpB56;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmpA0C,_tmpB56);
# 4474
if(*_tmpA09 != 0)
_tmpB05=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpB05,& Cyc_Tcutil_ek,(void*)_check_null(*_tmpA09),1,1);else{
# 4477
struct Cyc_List_List*effect=0;
# 4482
{struct Cyc_List_List*tvs=_tmpB05.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmpB57=*((struct _tuple28*)tvs->hd);struct Cyc_Absyn_Tvar*_tmpB59;int _tmpB5A;struct _tuple28 _tmpB58=_tmpB57;_tmpB59=_tmpB58.f1;_tmpB5A=_tmpB58.f2;
if(!_tmpB5A)continue;{
void*_tmpB5B=Cyc_Absyn_compress_kb(_tmpB59->kind);void*_tmpB5C=_tmpB5B;struct Cyc_Core_Opt**_tmpB5E;struct Cyc_Absyn_Kind*_tmpB5F;struct Cyc_Absyn_Kind*_tmpB61;struct Cyc_Core_Opt**_tmpB65;struct Cyc_Core_Opt**_tmpB68;_LL67F:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB5D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB5C;if(_tmpB5D->tag != 2)goto _LL681;else{_tmpB5E=(struct Cyc_Core_Opt**)& _tmpB5D->f1;_tmpB5F=_tmpB5D->f2;}}if(!(_tmpB5F->kind == Cyc_Absyn_RgnKind))goto _LL681;_LL680:
# 4488
 if(_tmpB5F->aliasqual == Cyc_Absyn_Top){
*_tmpB5E=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);_tmpB61=_tmpB5F;goto _LL682;}
# 4491
*_tmpB5E=Cyc_Tcutil_kind_to_bound_opt(_tmpB5F);_tmpB61=_tmpB5F;goto _LL682;_LL681:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpB60=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpB5C;if(_tmpB60->tag != 0)goto _LL683;else{_tmpB61=_tmpB60->f1;}}if(!(_tmpB61->kind == Cyc_Absyn_RgnKind))goto _LL683;_LL682:
# 4493
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp132F;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp132E;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp132D;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp132C;struct Cyc_List_List*_tmp132B;effect=((_tmp132B=_cycalloc(sizeof(*_tmp132B)),((_tmp132B->hd=(void*)((_tmp132F=_cycalloc(sizeof(*_tmp132F)),((_tmp132F[0]=((_tmp132C.tag=23,((_tmp132C.f1=(void*)((_tmp132E=_cycalloc(sizeof(*_tmp132E)),((_tmp132E[0]=((_tmp132D.tag=2,((_tmp132D.f1=_tmpB59,_tmp132D)))),_tmp132E)))),_tmp132C)))),_tmp132F)))),((_tmp132B->tl=effect,_tmp132B))))));}goto _LL67E;_LL683: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB62=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB5C;if(_tmpB62->tag != 2)goto _LL685;else{if((_tmpB62->f2)->kind != Cyc_Absyn_IntKind)goto _LL685;}}_LL684:
 goto _LL686;_LL685: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpB63=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpB5C;if(_tmpB63->tag != 0)goto _LL687;else{if((_tmpB63->f1)->kind != Cyc_Absyn_IntKind)goto _LL687;}}_LL686:
 goto _LL67E;_LL687: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB64=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB5C;if(_tmpB64->tag != 2)goto _LL689;else{_tmpB65=(struct Cyc_Core_Opt**)& _tmpB64->f1;if((_tmpB64->f2)->kind != Cyc_Absyn_EffKind)goto _LL689;}}_LL688:
# 4497
*_tmpB65=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);goto _LL68A;_LL689: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpB66=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpB5C;if(_tmpB66->tag != 0)goto _LL68B;else{if((_tmpB66->f1)->kind != Cyc_Absyn_EffKind)goto _LL68B;}}_LL68A:
# 4499
{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1335;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1334;struct Cyc_List_List*_tmp1333;effect=((_tmp1333=_cycalloc(sizeof(*_tmp1333)),((_tmp1333->hd=(void*)((_tmp1335=_cycalloc(sizeof(*_tmp1335)),((_tmp1335[0]=((_tmp1334.tag=2,((_tmp1334.f1=_tmpB59,_tmp1334)))),_tmp1335)))),((_tmp1333->tl=effect,_tmp1333))))));}goto _LL67E;_LL68B: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpB67=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpB5C;if(_tmpB67->tag != 1)goto _LL68D;else{_tmpB68=(struct Cyc_Core_Opt**)& _tmpB67->f1;}}_LL68C:
# 4501
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp133B;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp133A;struct Cyc_Core_Opt*_tmp1339;*_tmpB68=((_tmp1339=_cycalloc(sizeof(*_tmp1339)),((_tmp1339->v=(void*)((_tmp133B=_cycalloc(sizeof(*_tmp133B)),((_tmp133B[0]=((_tmp133A.tag=2,((_tmp133A.f1=0,((_tmp133A.f2=& Cyc_Tcutil_ak,_tmp133A)))))),_tmp133B)))),_tmp1339))));}goto _LL68E;_LL68D:;_LL68E:
# 4504
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp134A;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1349;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1348;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp1347;struct Cyc_List_List*_tmp1346;effect=((_tmp1346=_cycalloc(sizeof(*_tmp1346)),((_tmp1346->hd=(void*)((_tmp134A=_cycalloc(sizeof(*_tmp134A)),((_tmp134A[0]=((_tmp1347.tag=25,((_tmp1347.f1=(void*)((_tmp1349=_cycalloc(sizeof(*_tmp1349)),((_tmp1349[0]=((_tmp1348.tag=2,((_tmp1348.f1=_tmpB59,_tmp1348)))),_tmp1349)))),_tmp1347)))),_tmp134A)))),((_tmp1346->tl=effect,_tmp1346))))));}goto _LL67E;_LL67E:;};}}
# 4508
{struct Cyc_List_List*ts=_tmpB05.free_evars;for(0;ts != 0;ts=ts->tl){
struct _tuple29 _tmpB79=*((struct _tuple29*)ts->hd);void*_tmpB7B;int _tmpB7C;struct _tuple29 _tmpB7A=_tmpB79;_tmpB7B=_tmpB7A.f1;_tmpB7C=_tmpB7A.f2;
if(!_tmpB7C)continue;{
struct Cyc_Absyn_Kind*_tmpB7D=Cyc_Tcutil_typ_kind(_tmpB7B);struct Cyc_Absyn_Kind*_tmpB7E=_tmpB7D;_LL690: if(_tmpB7E->kind != Cyc_Absyn_RgnKind)goto _LL692;_LL691:
# 4513
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1350;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp134F;struct Cyc_List_List*_tmp134E;effect=((_tmp134E=_cycalloc(sizeof(*_tmp134E)),((_tmp134E->hd=(void*)((_tmp1350=_cycalloc(sizeof(*_tmp1350)),((_tmp1350[0]=((_tmp134F.tag=23,((_tmp134F.f1=_tmpB7B,_tmp134F)))),_tmp1350)))),((_tmp134E->tl=effect,_tmp134E))))));}goto _LL68F;_LL692: if(_tmpB7E->kind != Cyc_Absyn_EffKind)goto _LL694;_LL693:
# 4515
{struct Cyc_List_List*_tmp1351;effect=((_tmp1351=_cycalloc(sizeof(*_tmp1351)),((_tmp1351->hd=_tmpB7B,((_tmp1351->tl=effect,_tmp1351))))));}goto _LL68F;_LL694: if(_tmpB7E->kind != Cyc_Absyn_IntKind)goto _LL696;_LL695:
 goto _LL68F;_LL696:;_LL697:
# 4518
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1357;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp1356;struct Cyc_List_List*_tmp1355;effect=((_tmp1355=_cycalloc(sizeof(*_tmp1355)),((_tmp1355->hd=(void*)((_tmp1357=_cycalloc(sizeof(*_tmp1357)),((_tmp1357[0]=((_tmp1356.tag=25,((_tmp1356.f1=_tmpB7B,_tmp1356)))),_tmp1357)))),((_tmp1355->tl=effect,_tmp1355))))));}goto _LL68F;_LL68F:;};}}{
# 4521
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp135A;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1359;*_tmpA09=(void*)((_tmp1359=_cycalloc(sizeof(*_tmp1359)),((_tmp1359[0]=((_tmp135A.tag=24,((_tmp135A.f1=effect,_tmp135A)))),_tmp1359))));};}
# 4527
if(*_tmpA08 != 0){
struct Cyc_List_List*bs=*_tmpA08;for(0;bs != 0;bs=bs->tl){
void*_tmpB88=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);void*_tmpB89=_tmpB88;struct Cyc_Core_Opt**_tmpB8B;struct Cyc_Core_Opt**_tmpB8D;struct Cyc_Core_Opt**_tmpB8F;struct Cyc_Core_Opt**_tmpB91;struct Cyc_Core_Opt**_tmpB93;struct Cyc_Core_Opt**_tmpB95;struct Cyc_Core_Opt**_tmpB97;struct Cyc_Core_Opt**_tmpB99;struct Cyc_Core_Opt**_tmpB9B;struct Cyc_Core_Opt**_tmpB9D;struct Cyc_Absyn_Kind*_tmpB9E;_LL699: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpB8A=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpB89;if(_tmpB8A->tag != 1)goto _LL69B;else{_tmpB8B=(struct Cyc_Core_Opt**)& _tmpB8A->f1;}}_LL69A:
# 4531
{const char*_tmp135E;void*_tmp135D[1];struct Cyc_String_pa_PrintArg_struct _tmp135C;(_tmp135C.tag=0,((_tmp135C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((_tmp135D[0]=& _tmp135C,Cyc_Tcutil_warn(loc,((_tmp135E="Type variable %s unconstrained, assuming boxed",_tag_dyneither(_tmp135E,sizeof(char),47))),_tag_dyneither(_tmp135D,sizeof(void*),1)))))));}
# 4533
_tmpB8D=_tmpB8B;goto _LL69C;_LL69B: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB8C=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB89;if(_tmpB8C->tag != 2)goto _LL69D;else{_tmpB8D=(struct Cyc_Core_Opt**)& _tmpB8C->f1;if((_tmpB8C->f2)->kind != Cyc_Absyn_BoxKind)goto _LL69D;if((_tmpB8C->f2)->aliasqual != Cyc_Absyn_Top)goto _LL69D;}}_LL69C:
 _tmpB8F=_tmpB8D;goto _LL69E;_LL69D: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB8E=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB89;if(_tmpB8E->tag != 2)goto _LL69F;else{_tmpB8F=(struct Cyc_Core_Opt**)& _tmpB8E->f1;if((_tmpB8E->f2)->kind != Cyc_Absyn_MemKind)goto _LL69F;if((_tmpB8E->f2)->aliasqual != Cyc_Absyn_Top)goto _LL69F;}}_LL69E:
 _tmpB91=_tmpB8F;goto _LL6A0;_LL69F: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB90=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB89;if(_tmpB90->tag != 2)goto _LL6A1;else{_tmpB91=(struct Cyc_Core_Opt**)& _tmpB90->f1;if((_tmpB90->f2)->kind != Cyc_Absyn_MemKind)goto _LL6A1;if((_tmpB90->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL6A1;}}_LL6A0:
 _tmpB93=_tmpB91;goto _LL6A2;_LL6A1: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB92=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB89;if(_tmpB92->tag != 2)goto _LL6A3;else{_tmpB93=(struct Cyc_Core_Opt**)& _tmpB92->f1;if((_tmpB92->f2)->kind != Cyc_Absyn_AnyKind)goto _LL6A3;if((_tmpB92->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6A3;}}_LL6A2:
 _tmpB95=_tmpB93;goto _LL6A4;_LL6A3: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB94=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB89;if(_tmpB94->tag != 2)goto _LL6A5;else{_tmpB95=(struct Cyc_Core_Opt**)& _tmpB94->f1;if((_tmpB94->f2)->kind != Cyc_Absyn_AnyKind)goto _LL6A5;if((_tmpB94->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL6A5;}}_LL6A4:
# 4539
*_tmpB95=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL698;_LL6A5: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB96=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB89;if(_tmpB96->tag != 2)goto _LL6A7;else{_tmpB97=(struct Cyc_Core_Opt**)& _tmpB96->f1;if((_tmpB96->f2)->kind != Cyc_Absyn_MemKind)goto _LL6A7;if((_tmpB96->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL6A7;}}_LL6A6:
 _tmpB99=_tmpB97;goto _LL6A8;_LL6A7: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB98=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB89;if(_tmpB98->tag != 2)goto _LL6A9;else{_tmpB99=(struct Cyc_Core_Opt**)& _tmpB98->f1;if((_tmpB98->f2)->kind != Cyc_Absyn_AnyKind)goto _LL6A9;if((_tmpB98->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL6A9;}}_LL6A8:
# 4542
*_tmpB99=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL698;_LL6A9: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB9A=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB89;if(_tmpB9A->tag != 2)goto _LL6AB;else{_tmpB9B=(struct Cyc_Core_Opt**)& _tmpB9A->f1;if((_tmpB9A->f2)->kind != Cyc_Absyn_RgnKind)goto _LL6AB;if((_tmpB9A->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6AB;}}_LL6AA:
# 4544
*_tmpB9B=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL698;_LL6AB: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB9C=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB89;if(_tmpB9C->tag != 2)goto _LL6AD;else{_tmpB9D=(struct Cyc_Core_Opt**)& _tmpB9C->f1;_tmpB9E=_tmpB9C->f2;}}_LL6AC:
# 4546
*_tmpB9D=Cyc_Tcutil_kind_to_bound_opt(_tmpB9E);goto _LL698;_LL6AD: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpB9F=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpB89;if(_tmpB9F->tag != 0)goto _LL6AF;else{if((_tmpB9F->f1)->kind != Cyc_Absyn_MemKind)goto _LL6AF;}}_LL6AE:
# 4548
{const char*_tmp1361;void*_tmp1360;(_tmp1360=0,Cyc_Tcutil_terr(loc,((_tmp1361="functions can't abstract M types",_tag_dyneither(_tmp1361,sizeof(char),33))),_tag_dyneither(_tmp1360,sizeof(void*),0)));}goto _LL698;_LL6AF:;_LL6B0:
 goto _LL698;_LL698:;}}
# 4553
cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,_tmpB05.kind_env,*_tmpA08);
_tmpB05.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(_tmpB05.r,_tmpB05.free_vars,*_tmpA08);
# 4556
{struct Cyc_List_List*tvs=_tmpB05.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmpBA5=*((struct _tuple28*)tvs->hd);struct Cyc_Absyn_Tvar*_tmpBA7;int _tmpBA8;struct _tuple28 _tmpBA6=_tmpBA5;_tmpBA7=_tmpBA6.f1;_tmpBA8=_tmpBA6.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpBA7,_tmpBA8);}}{
# 4561
struct Cyc_List_List*evs=_tmpB05.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmpBA9=*((struct _tuple29*)evs->hd);void*_tmpBAB;int _tmpBAC;struct _tuple29 _tmpBAA=_tmpBA9;_tmpBAB=_tmpBAA.f1;_tmpBAC=_tmpBAA.f2;
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmpBAB,_tmpBAC);}};};};};}
# 4566
_npop_handler(0);goto _LL5B4;
# 4341
;_pop_region(newr);};}_LL5D5: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpA15=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp9DD;if(_tmpA15->tag != 10)goto _LL5D7;else{_tmpA16=_tmpA15->f1;}}_LL5D6:
# 4569
 for(0;_tmpA16 != 0;_tmpA16=_tmpA16->tl){
struct _tuple12*_tmpBB4=(struct _tuple12*)_tmpA16->hd;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmpBB4).f2,1,0);
((*_tmpBB4).f1).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpBB4).f1).print_const,(*_tmpBB4).f2);}
# 4575
goto _LL5B4;_LL5D7: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpA17=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp9DD;if(_tmpA17->tag != 12)goto _LL5D9;else{_tmpA18=_tmpA17->f1;_tmpA19=_tmpA17->f2;}}_LL5D8: {
# 4579
struct _RegionHandle _tmpBB5=_new_region("aprev_rgn");struct _RegionHandle*aprev_rgn=& _tmpBB5;_push_region(aprev_rgn);{
struct Cyc_List_List*prev_fields=0;
for(0;_tmpA19 != 0;_tmpA19=_tmpA19->tl){
struct Cyc_Absyn_Aggrfield*_tmpBB6=(struct Cyc_Absyn_Aggrfield*)_tmpA19->hd;struct _dyneither_ptr*_tmpBB8;struct Cyc_Absyn_Tqual*_tmpBB9;void*_tmpBBA;struct Cyc_Absyn_Exp*_tmpBBB;struct Cyc_List_List*_tmpBBC;struct Cyc_Absyn_Exp*_tmpBBD;struct Cyc_Absyn_Aggrfield*_tmpBB7=_tmpBB6;_tmpBB8=_tmpBB7->name;_tmpBB9=(struct Cyc_Absyn_Tqual*)& _tmpBB7->tq;_tmpBBA=_tmpBB7->type;_tmpBBB=_tmpBB7->width;_tmpBBC=_tmpBB7->attributes;_tmpBBD=_tmpBB7->requires_clause;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpBB8)){
const char*_tmp1365;void*_tmp1364[1];struct Cyc_String_pa_PrintArg_struct _tmp1363;(_tmp1363.tag=0,((_tmp1363.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpBB8),((_tmp1364[0]=& _tmp1363,Cyc_Tcutil_terr(loc,((_tmp1365="duplicate field %s",_tag_dyneither(_tmp1365,sizeof(char),19))),_tag_dyneither(_tmp1364,sizeof(void*),1)))))));}
{const char*_tmp1366;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpBB8,((_tmp1366="",_tag_dyneither(_tmp1366,sizeof(char),1))))!= 0){
struct Cyc_List_List*_tmp1367;prev_fields=((_tmp1367=_region_malloc(aprev_rgn,sizeof(*_tmp1367)),((_tmp1367->hd=_tmpBB8,((_tmp1367->tl=prev_fields,_tmp1367))))));}}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpBBA,1,0);
_tmpBB9->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpBB9->print_const,_tmpBBA);
# 4593
Cyc_Tcutil_check_bitfield(loc,te,_tmpBBA,_tmpBBB,_tmpBB8);
Cyc_Tcutil_check_field_atts(loc,_tmpBB8,_tmpBBC);
if(_tmpBBD != 0){
# 4597
if(_tmpA18 != Cyc_Absyn_UnionA){
const char*_tmp136A;void*_tmp1369;(_tmp1369=0,Cyc_Tcutil_terr(loc,((_tmp136A="@requires clause is only allowed on union members",_tag_dyneither(_tmp136A,sizeof(char),50))),_tag_dyneither(_tmp1369,sizeof(void*),0)));}{
struct _RegionHandle _tmpBC5=_new_region("temp");struct _RegionHandle*temp=& _tmpBC5;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmpBC6=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmpBC6,0,_tmpBBD);}
# 4603
if(!Cyc_Tcutil_is_integral(_tmpBBD)){
const char*_tmp136E;void*_tmp136D[1];struct Cyc_String_pa_PrintArg_struct _tmp136C;(_tmp136C.tag=0,((_tmp136C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmpBBD->topt))),((_tmp136D[0]=& _tmp136C,Cyc_Tcutil_terr(loc,((_tmp136E="@requires clause has type %s instead of integral type",_tag_dyneither(_tmp136E,sizeof(char),54))),_tag_dyneither(_tmp136D,sizeof(void*),1)))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpBBD,te,cvtenv);
# 4599
;_pop_region(temp);};}}}
# 4610
_npop_handler(0);goto _LL5B4;
# 4579
;_pop_region(aprev_rgn);}_LL5D9: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpA1A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp9DD;if(_tmpA1A->tag != 11)goto _LL5DB;else{_tmpA1B=(union Cyc_Absyn_AggrInfoU*)&(_tmpA1A->f1).aggr_info;_tmpA1C=(struct Cyc_List_List**)&(_tmpA1A->f1).targs;}}_LL5DA:
# 4613
{union Cyc_Absyn_AggrInfoU _tmpBCA=*_tmpA1B;union Cyc_Absyn_AggrInfoU _tmpBCB=_tmpBCA;enum Cyc_Absyn_AggrKind _tmpBCC;struct _tuple2*_tmpBCD;struct Cyc_Core_Opt*_tmpBCE;struct Cyc_Absyn_Aggrdecl*_tmpBCF;_LL6B2: if((_tmpBCB.UnknownAggr).tag != 1)goto _LL6B4;_tmpBCC=((struct _tuple4)(_tmpBCB.UnknownAggr).val).f1;_tmpBCD=((struct _tuple4)(_tmpBCB.UnknownAggr).val).f2;_tmpBCE=((struct _tuple4)(_tmpBCB.UnknownAggr).val).f3;_LL6B3: {
# 4615
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmpBD0;_push_handler(& _tmpBD0);{int _tmpBD2=0;if(setjmp(_tmpBD0.handler))_tmpBD2=1;if(!_tmpBD2){
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmpBCD);{
struct Cyc_Absyn_Aggrdecl*_tmpBD3=*adp;
if(_tmpBD3->kind != _tmpBCC){
if(_tmpBD3->kind == Cyc_Absyn_StructA){
const char*_tmp1373;void*_tmp1372[2];struct Cyc_String_pa_PrintArg_struct _tmp1371;struct Cyc_String_pa_PrintArg_struct _tmp1370;(_tmp1370.tag=0,((_tmp1370.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpBCD)),((_tmp1371.tag=0,((_tmp1371.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4621
Cyc_Absynpp_qvar2string(_tmpBCD)),((_tmp1372[0]=& _tmp1371,((_tmp1372[1]=& _tmp1370,Cyc_Tcutil_terr(loc,((_tmp1373="expecting struct %s instead of union %s",_tag_dyneither(_tmp1373,sizeof(char),40))),_tag_dyneither(_tmp1372,sizeof(void*),2)))))))))))));}else{
# 4624
const char*_tmp1378;void*_tmp1377[2];struct Cyc_String_pa_PrintArg_struct _tmp1376;struct Cyc_String_pa_PrintArg_struct _tmp1375;(_tmp1375.tag=0,((_tmp1375.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpBCD)),((_tmp1376.tag=0,((_tmp1376.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4624
Cyc_Absynpp_qvar2string(_tmpBCD)),((_tmp1377[0]=& _tmp1376,((_tmp1377[1]=& _tmp1375,Cyc_Tcutil_terr(loc,((_tmp1378="expecting union %s instead of struct %s",_tag_dyneither(_tmp1378,sizeof(char),40))),_tag_dyneither(_tmp1377,sizeof(void*),2)))))))))))));}}
# 4627
if((unsigned int)_tmpBCE  && (int)_tmpBCE->v){
if(!((unsigned int)_tmpBD3->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpBD3->impl))->tagged){
const char*_tmp137C;void*_tmp137B[1];struct Cyc_String_pa_PrintArg_struct _tmp137A;(_tmp137A.tag=0,((_tmp137A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpBCD)),((_tmp137B[0]=& _tmp137A,Cyc_Tcutil_terr(loc,((_tmp137C="@tagged qualfiers don't agree on union %s",_tag_dyneither(_tmp137C,sizeof(char),42))),_tag_dyneither(_tmp137B,sizeof(void*),1)))))));}}
# 4633
*_tmpA1B=Cyc_Absyn_KnownAggr(adp);};
# 4617
;_pop_handler();}else{void*_tmpBD1=(void*)_exn_thrown;void*_tmpBE0=_tmpBD1;void*_tmpBE2;_LL6B7: {struct Cyc_Dict_Absent_exn_struct*_tmpBE1=(struct Cyc_Dict_Absent_exn_struct*)_tmpBE0;if(_tmpBE1->tag != Cyc_Dict_Absent)goto _LL6B9;}_LL6B8: {
# 4637
struct Cyc_Tcenv_Genv*_tmpBE3=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp137D;struct Cyc_Absyn_Aggrdecl*_tmpBE4=(_tmp137D=_cycalloc(sizeof(*_tmp137D)),((_tmp137D->kind=_tmpBCC,((_tmp137D->sc=Cyc_Absyn_Extern,((_tmp137D->name=_tmpBCD,((_tmp137D->tvs=0,((_tmp137D->impl=0,((_tmp137D->attributes=0,((_tmp137D->expected_mem_kind=0,_tmp137D)))))))))))))));
Cyc_Tc_tcAggrdecl(te,_tmpBE3,loc,_tmpBE4);
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmpBCD);
*_tmpA1B=Cyc_Absyn_KnownAggr(adp);
# 4643
if(*_tmpA1C != 0){
{const char*_tmp1381;void*_tmp1380[1];struct Cyc_String_pa_PrintArg_struct _tmp137F;(_tmp137F.tag=0,((_tmp137F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpBCD)),((_tmp1380[0]=& _tmp137F,Cyc_Tcutil_terr(loc,((_tmp1381="declare parameterized type %s before using",_tag_dyneither(_tmp1381,sizeof(char),43))),_tag_dyneither(_tmp1380,sizeof(void*),1)))))));}
return cvtenv;}
# 4648
goto _LL6B6;}_LL6B9: _tmpBE2=_tmpBE0;_LL6BA:(void)_rethrow(_tmpBE2);_LL6B6:;}};}
# 4650
_tmpBCF=*adp;goto _LL6B5;}_LL6B4: if((_tmpBCB.KnownAggr).tag != 2)goto _LL6B1;_tmpBCF=*((struct Cyc_Absyn_Aggrdecl**)(_tmpBCB.KnownAggr).val);_LL6B5: {
# 4652
struct Cyc_List_List*tvs=_tmpBCF->tvs;
struct Cyc_List_List*ts=*_tmpA1C;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Tvar*_tmpBE9=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmpBEA=(void*)ts->hd;
# 4660
{struct _tuple0 _tmp1382;struct _tuple0 _tmpBEB=(_tmp1382.f1=Cyc_Absyn_compress_kb(_tmpBE9->kind),((_tmp1382.f2=_tmpBEA,_tmp1382)));struct _tuple0 _tmpBEC=_tmpBEB;struct Cyc_Absyn_Tvar*_tmpBEF;_LL6BC:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpBED=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpBEC.f1;if(_tmpBED->tag != 1)goto _LL6BE;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpBEE=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpBEC.f2;if(_tmpBEE->tag != 2)goto _LL6BE;else{_tmpBEF=_tmpBEE->f1;}};_LL6BD:
# 4662
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpBEF);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpBEF,1);
continue;_LL6BE:;_LL6BF:
 goto _LL6BB;_LL6BB:;}{
# 4667
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 4671
if(ts != 0){
const char*_tmp1386;void*_tmp1385[1];struct Cyc_String_pa_PrintArg_struct _tmp1384;(_tmp1384.tag=0,((_tmp1384.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBCF->name)),((_tmp1385[0]=& _tmp1384,Cyc_Tcutil_terr(loc,((_tmp1386="too many parameters for type %s",_tag_dyneither(_tmp1386,sizeof(char),32))),_tag_dyneither(_tmp1385,sizeof(void*),1)))))));}
if(tvs != 0){
# 4675
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp1387;hidden_ts=((_tmp1387=_cycalloc(sizeof(*_tmp1387)),((_tmp1387->hd=e,((_tmp1387->tl=hidden_ts,_tmp1387))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);}
# 4682
*_tmpA1C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpA1C,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4684
if((allow_abs_aggr  && _tmpBCF->impl == 0) && !
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,expected_kind))
# 4689
_tmpBCF->expected_mem_kind=1;}_LL6B1:;}
# 4692
goto _LL5B4;_LL5DB: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA1D=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9DD;if(_tmpA1D->tag != 17)goto _LL5DD;else{_tmpA1E=_tmpA1D->f1;_tmpA1F=(struct Cyc_List_List**)& _tmpA1D->f2;_tmpA20=(struct Cyc_Absyn_Typedefdecl**)& _tmpA1D->f3;_tmpA21=(void**)((void**)& _tmpA1D->f4);}}_LL5DC: {
# 4695
struct Cyc_List_List*targs=*_tmpA1F;
# 4697
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmpBF5;_push_handler(& _tmpBF5);{int _tmpBF7=0;if(setjmp(_tmpBF5.handler))_tmpBF7=1;if(!_tmpBF7){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmpA1E);;_pop_handler();}else{void*_tmpBF6=(void*)_exn_thrown;void*_tmpBF9=_tmpBF6;void*_tmpBFB;_LL6C1: {struct Cyc_Dict_Absent_exn_struct*_tmpBFA=(struct Cyc_Dict_Absent_exn_struct*)_tmpBF9;if(_tmpBFA->tag != Cyc_Dict_Absent)goto _LL6C3;}_LL6C2:
# 4700
{const char*_tmp138B;void*_tmp138A[1];struct Cyc_String_pa_PrintArg_struct _tmp1389;(_tmp1389.tag=0,((_tmp1389.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA1E)),((_tmp138A[0]=& _tmp1389,Cyc_Tcutil_terr(loc,((_tmp138B="unbound typedef name %s",_tag_dyneither(_tmp138B,sizeof(char),24))),_tag_dyneither(_tmp138A,sizeof(void*),1)))))));}
return cvtenv;_LL6C3: _tmpBFB=_tmpBF9;_LL6C4:(void)_rethrow(_tmpBFB);_LL6C0:;}};}
# 4703
*_tmpA20=td;
# 4705
_tmpA1E[0]=(td->name)[0];{
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;
struct _RegionHandle _tmpBFF=_new_region("temp");struct _RegionHandle*temp=& _tmpBFF;_push_region(temp);{
struct Cyc_List_List*inst=0;
# 4711
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,td);
# 4716
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{
struct _tuple16*_tmp138E;struct Cyc_List_List*_tmp138D;inst=((_tmp138D=_region_malloc(temp,sizeof(*_tmp138D)),((_tmp138D->hd=((_tmp138E=_region_malloc(temp,sizeof(*_tmp138E)),((_tmp138E->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp138E->f2=(void*)ts->hd,_tmp138E)))))),((_tmp138D->tl=inst,_tmp138D))))));};}
# 4720
if(ts != 0){
const char*_tmp1392;void*_tmp1391[1];struct Cyc_String_pa_PrintArg_struct _tmp1390;(_tmp1390.tag=0,((_tmp1390.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA1E)),((_tmp1391[0]=& _tmp1390,Cyc_Tcutil_terr(loc,((_tmp1392="too many parameters for typedef %s",_tag_dyneither(_tmp1392,sizeof(char),35))),_tag_dyneither(_tmp1391,sizeof(void*),1)))))));}
if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;
# 4725
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp1393;hidden_ts=((_tmp1393=_cycalloc(sizeof(*_tmp1393)),((_tmp1393->hd=e,((_tmp1393->tl=hidden_ts,_tmp1393))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);{
struct _tuple16*_tmp1396;struct Cyc_List_List*_tmp1395;inst=((_tmp1395=_cycalloc(sizeof(*_tmp1395)),((_tmp1395->hd=((_tmp1396=_cycalloc(sizeof(*_tmp1396)),((_tmp1396->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1396->f2=e,_tmp1396)))))),((_tmp1395->tl=inst,_tmp1395))))));};}
# 4733
*_tmpA1F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4735
if(td->defn != 0){
void*new_typ=
inst == 0?(void*)_check_null(td->defn):
 Cyc_Tcutil_rsubstitute(temp,inst,(void*)_check_null(td->defn));
*_tmpA21=new_typ;}}
# 4742
_npop_handler(0);goto _LL5B4;
# 4708
;_pop_region(temp);};}_LL5DD: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpA22=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp9DD;if(_tmpA22->tag != 22)goto _LL5DF;}_LL5DE:
# 4743
 goto _LL5E0;_LL5DF: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpA23=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp9DD;if(_tmpA23->tag != 21)goto _LL5E1;}_LL5E0:
 goto _LL5E2;_LL5E1: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmpA24=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp9DD;if(_tmpA24->tag != 20)goto _LL5E3;}_LL5E2:
 goto _LL5B4;_LL5E3: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA25=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp9DD;if(_tmpA25->tag != 15)goto _LL5E5;else{_tmpA26=(void*)_tmpA25->f1;}}_LL5E4:
# 4747
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmpA26,1,allow_abs_aggr);goto _LL5B4;_LL5E5: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpA27=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp9DD;if(_tmpA27->tag != 16)goto _LL5E7;else{_tmpA28=(void*)_tmpA27->f1;_tmpA29=(void*)_tmpA27->f2;}}_LL5E6:
# 4750
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmpA28,0,0);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmpA29,1,0);
goto _LL5B4;_LL5E7: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpA2A=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp9DD;if(_tmpA2A->tag != 23)goto _LL5E9;else{_tmpA2B=(void*)_tmpA2A->f1;}}_LL5E8:
# 4754
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmpA2B,1,1);goto _LL5B4;_LL5E9: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpA2C=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp9DD;if(_tmpA2C->tag != 25)goto _LL5EB;else{_tmpA2D=(void*)_tmpA2C->f1;}}_LL5EA:
# 4756
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpA2D,1,1);goto _LL5B4;_LL5EB: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpA2E=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp9DD;if(_tmpA2E->tag != 24)goto _LL5B4;else{_tmpA2F=_tmpA2E->f1;}}_LL5EC:
# 4758
 for(0;_tmpA2F != 0;_tmpA2F=_tmpA2F->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmpA2F->hd,1,1);}
goto _LL5B4;_LL5B4:;}
# 4762
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind)){
const char*_tmp139C;void*_tmp139B[3];struct Cyc_String_pa_PrintArg_struct _tmp139A;struct Cyc_String_pa_PrintArg_struct _tmp1399;struct Cyc_String_pa_PrintArg_struct _tmp1398;(_tmp1398.tag=0,((_tmp1398.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(expected_kind)),((_tmp1399.tag=0,((_tmp1399.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmp139A.tag=0,((_tmp139A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp139B[0]=& _tmp139A,((_tmp139B[1]=& _tmp1399,((_tmp139B[2]=& _tmp1398,Cyc_Tcutil_terr(loc,((_tmp139C="type %s has kind %s but as used here needs kind %s",_tag_dyneither(_tmp139C,sizeof(char),51))),_tag_dyneither(_tmp139B,sizeof(void*),3)))))))))))))))))));}
# 4766
return cvtenv;}
# 4774
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){
# 4776
{void*_tmpC0D=e->r;void*_tmpC0E=_tmpC0D;struct Cyc_List_List*_tmpC14;struct Cyc_Absyn_Exp*_tmpC16;struct Cyc_Absyn_Exp*_tmpC17;struct Cyc_Absyn_Exp*_tmpC18;struct Cyc_Absyn_Exp*_tmpC1A;struct Cyc_Absyn_Exp*_tmpC1B;struct Cyc_Absyn_Exp*_tmpC1D;struct Cyc_Absyn_Exp*_tmpC1E;struct Cyc_Absyn_Exp*_tmpC20;struct Cyc_Absyn_Exp*_tmpC21;void*_tmpC23;struct Cyc_Absyn_Exp*_tmpC24;void*_tmpC26;void*_tmpC28;void*_tmpC2A;struct Cyc_Absyn_Exp*_tmpC2C;_LL6C6: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpC0F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpC0E;if(_tmpC0F->tag != 0)goto _LL6C8;}_LL6C7:
 goto _LL6C9;_LL6C8: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpC10=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpC0E;if(_tmpC10->tag != 31)goto _LL6CA;}_LL6C9:
 goto _LL6CB;_LL6CA: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpC11=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpC0E;if(_tmpC11->tag != 32)goto _LL6CC;}_LL6CB:
 goto _LL6CD;_LL6CC: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpC12=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC0E;if(_tmpC12->tag != 1)goto _LL6CE;}_LL6CD:
 goto _LL6C5;_LL6CE: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpC13=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC0E;if(_tmpC13->tag != 2)goto _LL6D0;else{_tmpC14=_tmpC13->f2;}}_LL6CF:
# 4782
 for(0;_tmpC14 != 0;_tmpC14=_tmpC14->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmpC14->hd,te,cvtenv);}
goto _LL6C5;_LL6D0: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpC15=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpC0E;if(_tmpC15->tag != 5)goto _LL6D2;else{_tmpC16=_tmpC15->f1;_tmpC17=_tmpC15->f2;_tmpC18=_tmpC15->f3;}}_LL6D1:
# 4786
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC16,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC17,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC18,te,cvtenv);
goto _LL6C5;_LL6D2: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpC19=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpC0E;if(_tmpC19->tag != 6)goto _LL6D4;else{_tmpC1A=_tmpC19->f1;_tmpC1B=_tmpC19->f2;}}_LL6D3:
 _tmpC1D=_tmpC1A;_tmpC1E=_tmpC1B;goto _LL6D5;_LL6D4: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmpC1C=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpC0E;if(_tmpC1C->tag != 7)goto _LL6D6;else{_tmpC1D=_tmpC1C->f1;_tmpC1E=_tmpC1C->f2;}}_LL6D5:
 _tmpC20=_tmpC1D;_tmpC21=_tmpC1E;goto _LL6D7;_LL6D6: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpC1F=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpC0E;if(_tmpC1F->tag != 8)goto _LL6D8;else{_tmpC20=_tmpC1F->f1;_tmpC21=_tmpC1F->f2;}}_LL6D7:
# 4793
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC20,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC21,te,cvtenv);
goto _LL6C5;_LL6D8: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpC22=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpC0E;if(_tmpC22->tag != 13)goto _LL6DA;else{_tmpC23=(void*)_tmpC22->f1;_tmpC24=_tmpC22->f2;}}_LL6D9:
# 4797
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC24,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpC23,1,0);
goto _LL6C5;_LL6DA: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpC25=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpC0E;if(_tmpC25->tag != 18)goto _LL6DC;else{_tmpC26=(void*)_tmpC25->f1;}}_LL6DB:
 _tmpC28=_tmpC26;goto _LL6DD;_LL6DC: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpC27=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpC0E;if(_tmpC27->tag != 16)goto _LL6DE;else{_tmpC28=(void*)_tmpC27->f1;}}_LL6DD:
# 4802
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpC28,1,0);
goto _LL6C5;_LL6DE: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmpC29=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpC0E;if(_tmpC29->tag != 38)goto _LL6E0;else{_tmpC2A=(void*)_tmpC29->f1;}}_LL6DF:
# 4805
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpC2A,1,0);
goto _LL6C5;_LL6E0: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpC2B=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpC0E;if(_tmpC2B->tag != 17)goto _LL6E2;else{_tmpC2C=_tmpC2B->f1;}}_LL6E1:
# 4808
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC2C,te,cvtenv);
goto _LL6C5;_LL6E2:;_LL6E3: {
# 4811
const char*_tmp139F;void*_tmp139E;(_tmp139E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp139F="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",_tag_dyneither(_tmp139F,sizeof(char),66))),_tag_dyneither(_tmp139E,sizeof(void*),0)));}_LL6C5:;}
# 4813
return cvtenv;}
# 4816
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,int allow_abs_aggr,void*t){
# 4822
struct Cyc_List_List*_tmpC2F=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1,allow_abs_aggr);
# 4825
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
struct _tuple28 _tmpC30=*((struct _tuple28*)vs->hd);struct Cyc_Absyn_Tvar*_tmpC32;struct _tuple28 _tmpC31=_tmpC30;_tmpC32=_tmpC31.f1;
cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(_tmpC2F,_tmpC32);}}
# 4833
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmpC33=*((struct _tuple29*)evs->hd);void*_tmpC35;struct _tuple29 _tmpC34=_tmpC33;_tmpC35=_tmpC34.f1;{
void*_tmpC36=Cyc_Tcutil_compress(_tmpC35);void*_tmpC37=_tmpC36;struct Cyc_Core_Opt**_tmpC39;_LL6E5: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpC38=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpC37;if(_tmpC38->tag != 1)goto _LL6E7;else{_tmpC39=(struct Cyc_Core_Opt**)& _tmpC38->f4;}}_LL6E6:
# 4837
 if(*_tmpC39 == 0){
struct Cyc_Core_Opt*_tmp13A0;*_tmpC39=((_tmp13A0=_cycalloc(sizeof(*_tmp13A0)),((_tmp13A0->v=_tmpC2F,_tmp13A0))));}else{
# 4841
struct Cyc_List_List*_tmpC3B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmpC39))->v;
struct Cyc_List_List*_tmpC3C=0;
for(0;_tmpC3B != 0;_tmpC3B=_tmpC3B->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmpC2F,(struct Cyc_Absyn_Tvar*)_tmpC3B->hd)){
struct Cyc_List_List*_tmp13A1;_tmpC3C=((_tmp13A1=_cycalloc(sizeof(*_tmp13A1)),((_tmp13A1->hd=(struct Cyc_Absyn_Tvar*)_tmpC3B->hd,((_tmp13A1->tl=_tmpC3C,_tmp13A1))))));}}{
struct Cyc_Core_Opt*_tmp13A2;*_tmpC39=((_tmp13A2=_cycalloc(sizeof(*_tmp13A2)),((_tmp13A2->v=_tmpC3C,_tmp13A2))));};}
# 4848
goto _LL6E4;_LL6E7:;_LL6E8:
 goto _LL6E4;_LL6E4:;};}}
# 4852
return cvt;}
# 4858
void Cyc_Tcutil_check_free_evars(struct Cyc_List_List*free_evars,void*in_t,unsigned int loc){
# 4860
for(0;free_evars != 0;free_evars=free_evars->tl){
void*e=(void*)free_evars->hd;
struct Cyc_Absyn_Kind*_tmpC3F=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind*_tmpC40=_tmpC3F;_LL6EA: if(_tmpC40->kind != Cyc_Absyn_RgnKind)goto _LL6EC;if(_tmpC40->aliasqual != Cyc_Absyn_Unique)goto _LL6EC;_LL6EB:
# 4864
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp13A5;void*_tmp13A4;(_tmp13A4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13A5="can't unify evar with unique region!",_tag_dyneither(_tmp13A5,sizeof(char),37))),_tag_dyneither(_tmp13A4,sizeof(void*),0)));}
goto _LL6E9;_LL6EC: if(_tmpC40->kind != Cyc_Absyn_RgnKind)goto _LL6EE;if(_tmpC40->aliasqual != Cyc_Absyn_Aliasable)goto _LL6EE;_LL6ED:
 goto _LL6EF;_LL6EE: if(_tmpC40->kind != Cyc_Absyn_RgnKind)goto _LL6F0;if(_tmpC40->aliasqual != Cyc_Absyn_Top)goto _LL6F0;_LL6EF:
# 4869
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp13A8;void*_tmp13A7;(_tmp13A7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13A8="can't unify evar with heap!",_tag_dyneither(_tmp13A8,sizeof(char),28))),_tag_dyneither(_tmp13A7,sizeof(void*),0)));}
goto _LL6E9;_LL6F0: if(_tmpC40->kind != Cyc_Absyn_EffKind)goto _LL6F2;_LL6F1:
# 4872
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*_tmp13AB;void*_tmp13AA;(_tmp13AA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13AB="can't unify evar with {}!",_tag_dyneither(_tmp13AB,sizeof(char),26))),_tag_dyneither(_tmp13AA,sizeof(void*),0)));}
goto _LL6E9;_LL6F2:;_LL6F3:
# 4875
{const char*_tmp13B0;void*_tmp13AF[2];struct Cyc_String_pa_PrintArg_struct _tmp13AE;struct Cyc_String_pa_PrintArg_struct _tmp13AD;(_tmp13AD.tag=0,((_tmp13AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(in_t)),((_tmp13AE.tag=0,((_tmp13AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e)),((_tmp13AF[0]=& _tmp13AE,((_tmp13AF[1]=& _tmp13AD,Cyc_Tcutil_terr(loc,((_tmp13B0="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(_tmp13B0,sizeof(char),52))),_tag_dyneither(_tmp13AF,sizeof(void*),2)))))))))))));}
goto _LL6E9;_LL6E9:;}}
# 4887
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_List_List*_tmpC4B=Cyc_Tcenv_lookup_type_vars(te);
struct Cyc_Absyn_Kind*expected_kind=generalize_evars?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
struct _RegionHandle _tmpC4C=_new_region("temp");struct _RegionHandle*temp=& _tmpC4C;_push_region(temp);
{struct Cyc_Tcutil_CVTEnv _tmp13B1;struct Cyc_Tcutil_CVTEnv _tmpC4D=Cyc_Tcutil_check_valid_type(loc,te,(
(_tmp13B1.r=temp,((_tmp13B1.kind_env=_tmpC4B,((_tmp13B1.free_vars=0,((_tmp13B1.free_evars=0,((_tmp13B1.generalize_evars=generalize_evars,((_tmp13B1.fn_result=0,_tmp13B1)))))))))))),expected_kind,1,t);
# 4896
struct Cyc_List_List*_tmpC4E=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmpC4D.free_vars);
struct Cyc_List_List*_tmpC4F=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmpC4D.free_evars);
# 4900
if(_tmpC4B != 0){
struct Cyc_List_List*_tmpC50=0;
{struct Cyc_List_List*_tmpC51=_tmpC4E;for(0;(unsigned int)_tmpC51;_tmpC51=_tmpC51->tl){
struct Cyc_Absyn_Tvar*_tmpC52=(struct Cyc_Absyn_Tvar*)_tmpC51->hd;
int found=0;
{struct Cyc_List_List*_tmpC53=_tmpC4B;for(0;(unsigned int)_tmpC53;_tmpC53=_tmpC53->tl){
if(Cyc_Absyn_tvar_cmp(_tmpC52,(struct Cyc_Absyn_Tvar*)_tmpC53->hd)== 0){found=1;break;}}}
if(!found){
struct Cyc_List_List*_tmp13B2;_tmpC50=((_tmp13B2=_region_malloc(temp,sizeof(*_tmp13B2)),((_tmp13B2->hd=(struct Cyc_Absyn_Tvar*)_tmpC51->hd,((_tmp13B2->tl=_tmpC50,_tmp13B2))))));}}}
# 4910
_tmpC4E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpC50);}
# 4916
{struct Cyc_List_List*x=_tmpC4E;for(0;x != 0;x=x->tl){
void*_tmpC55=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);void*_tmpC56=_tmpC55;struct Cyc_Core_Opt**_tmpC58;struct Cyc_Core_Opt**_tmpC5A;struct Cyc_Core_Opt**_tmpC5C;struct Cyc_Core_Opt**_tmpC5E;struct Cyc_Core_Opt**_tmpC60;struct Cyc_Core_Opt**_tmpC62;struct Cyc_Core_Opt**_tmpC64;struct Cyc_Absyn_Kind*_tmpC65;enum Cyc_Absyn_AliasQual _tmpC67;_LL6F5: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpC57=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpC56;if(_tmpC57->tag != 1)goto _LL6F7;else{_tmpC58=(struct Cyc_Core_Opt**)& _tmpC57->f1;}}_LL6F6:
 _tmpC5A=_tmpC58;goto _LL6F8;_LL6F7: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC59=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC56;if(_tmpC59->tag != 2)goto _LL6F9;else{_tmpC5A=(struct Cyc_Core_Opt**)& _tmpC59->f1;if((_tmpC59->f2)->kind != Cyc_Absyn_BoxKind)goto _LL6F9;if((_tmpC59->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6F9;}}_LL6F8:
 _tmpC5C=_tmpC5A;goto _LL6FA;_LL6F9: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC5B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC56;if(_tmpC5B->tag != 2)goto _LL6FB;else{_tmpC5C=(struct Cyc_Core_Opt**)& _tmpC5B->f1;if((_tmpC5B->f2)->kind != Cyc_Absyn_MemKind)goto _LL6FB;if((_tmpC5B->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6FB;}}_LL6FA:
 _tmpC5E=_tmpC5C;goto _LL6FC;_LL6FB: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC5D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC56;if(_tmpC5D->tag != 2)goto _LL6FD;else{_tmpC5E=(struct Cyc_Core_Opt**)& _tmpC5D->f1;if((_tmpC5D->f2)->kind != Cyc_Absyn_MemKind)goto _LL6FD;if((_tmpC5D->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL6FD;}}_LL6FC:
# 4922
*_tmpC5E=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL6F4;_LL6FD: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC5F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC56;if(_tmpC5F->tag != 2)goto _LL6FF;else{_tmpC60=(struct Cyc_Core_Opt**)& _tmpC5F->f1;if((_tmpC5F->f2)->kind != Cyc_Absyn_MemKind)goto _LL6FF;if((_tmpC5F->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL6FF;}}_LL6FE:
# 4924
*_tmpC60=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL6F4;_LL6FF: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC61=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC56;if(_tmpC61->tag != 2)goto _LL701;else{_tmpC62=(struct Cyc_Core_Opt**)& _tmpC61->f1;if((_tmpC61->f2)->kind != Cyc_Absyn_RgnKind)goto _LL701;if((_tmpC61->f2)->aliasqual != Cyc_Absyn_Top)goto _LL701;}}_LL700:
# 4926
*_tmpC62=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL6F4;_LL701: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC63=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC56;if(_tmpC63->tag != 2)goto _LL703;else{_tmpC64=(struct Cyc_Core_Opt**)& _tmpC63->f1;_tmpC65=_tmpC63->f2;}}_LL702:
# 4928
*_tmpC64=Cyc_Tcutil_kind_to_bound_opt(_tmpC65);goto _LL6F4;_LL703: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpC66=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpC56;if(_tmpC66->tag != 0)goto _LL705;else{if((_tmpC66->f1)->kind != Cyc_Absyn_MemKind)goto _LL705;_tmpC67=(_tmpC66->f1)->aliasqual;}}_LL704:
# 4930
{const char*_tmp13B9;void*_tmp13B8[2];struct Cyc_String_pa_PrintArg_struct _tmp13B7;struct Cyc_Absyn_Kind*_tmp13B6;struct Cyc_String_pa_PrintArg_struct _tmp13B5;(_tmp13B5.tag=0,((_tmp13B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(((_tmp13B6=_cycalloc_atomic(sizeof(*_tmp13B6)),((_tmp13B6->kind=Cyc_Absyn_MemKind,((_tmp13B6->aliasqual=_tmpC67,_tmp13B6)))))))),((_tmp13B7.tag=0,((_tmp13B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd)),((_tmp13B8[0]=& _tmp13B7,((_tmp13B8[1]=& _tmp13B5,Cyc_Tcutil_terr(loc,((_tmp13B9="type variable %s cannot have kind %s",_tag_dyneither(_tmp13B9,sizeof(char),37))),_tag_dyneither(_tmp13B8,sizeof(void*),2)))))))))))));}
goto _LL6F4;_LL705:;_LL706:
 goto _LL6F4;_LL6F4:;}}
# 4937
if(_tmpC4E != 0  || _tmpC4F != 0){
{void*_tmpC6D=Cyc_Tcutil_compress(t);void*_tmpC6E=_tmpC6D;struct Cyc_List_List**_tmpC70;_LL708: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpC6F=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC6E;if(_tmpC6F->tag != 9)goto _LL70A;else{_tmpC70=(struct Cyc_List_List**)&(_tmpC6F->f1).tvars;}}_LL709:
# 4940
 if(*_tmpC70 == 0){
# 4942
*_tmpC70=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmpC4E);
_tmpC4E=0;}
# 4945
goto _LL707;_LL70A:;_LL70B:
 goto _LL707;_LL707:;}
# 4948
if(_tmpC4E != 0){
const char*_tmp13BD;void*_tmp13BC[1];struct Cyc_String_pa_PrintArg_struct _tmp13BB;(_tmp13BB.tag=0,((_tmp13BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmpC4E->hd)->name),((_tmp13BC[0]=& _tmp13BB,Cyc_Tcutil_terr(loc,((_tmp13BD="unbound type variable %s",_tag_dyneither(_tmp13BD,sizeof(char),25))),_tag_dyneither(_tmp13BC,sizeof(void*),1)))))));}
Cyc_Tcutil_check_free_evars(_tmpC4F,t,loc);}}
# 4892
;_pop_region(temp);}
# 4959
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 4962
Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);
{void*_tmpC75=Cyc_Tcutil_compress(t);void*_tmpC76=_tmpC75;struct Cyc_List_List*_tmpC78;void*_tmpC79;struct Cyc_Absyn_Tqual _tmpC7A;void*_tmpC7B;struct Cyc_List_List*_tmpC7C;struct Cyc_Absyn_Exp*_tmpC7D;struct Cyc_List_List*_tmpC7E;struct Cyc_Absyn_Exp*_tmpC7F;struct Cyc_List_List*_tmpC80;_LL70D: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpC77=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC76;if(_tmpC77->tag != 9)goto _LL70F;else{_tmpC78=(_tmpC77->f1).tvars;_tmpC79=(_tmpC77->f1).effect;_tmpC7A=(_tmpC77->f1).ret_tqual;_tmpC7B=(_tmpC77->f1).ret_typ;_tmpC7C=(_tmpC77->f1).args;_tmpC7D=(_tmpC77->f1).requires_clause;_tmpC7E=(_tmpC77->f1).requires_relns;_tmpC7F=(_tmpC77->f1).ensures_clause;_tmpC80=(_tmpC77->f1).ensures_relns;}}_LL70E:
# 4965
 fd->tvs=_tmpC78;
fd->effect=_tmpC79;
{struct Cyc_List_List*_tmpC81=fd->args;for(0;_tmpC81 != 0;(_tmpC81=_tmpC81->tl,_tmpC7C=_tmpC7C->tl)){
# 4969
(*((struct _tuple10*)_tmpC81->hd)).f2=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmpC7C))->hd)).f2;
(*((struct _tuple10*)_tmpC81->hd)).f3=(*((struct _tuple10*)_tmpC7C->hd)).f3;}}
# 4972
fd->ret_tqual=_tmpC7A;
fd->ret_type=_tmpC7B;
(fd->ret_tqual).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmpC7B);
fd->requires_clause=_tmpC7D;
fd->requires_relns=_tmpC7E;
fd->ensures_clause=_tmpC7F;
fd->ensures_relns=_tmpC80;
goto _LL70C;_LL70F:;_LL710: {
const char*_tmp13C0;void*_tmp13BF;(_tmp13BF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13C0="check_fndecl_valid_type: not a FnType",_tag_dyneither(_tmp13C0,sizeof(char),38))),_tag_dyneither(_tmp13BF,sizeof(void*),0)));}_LL70C:;}{
# 4983
struct _RegionHandle _tmpC84=_new_region("r");struct _RegionHandle*r=& _tmpC84;_push_region(r);{
const char*_tmp13C1;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _dyneither_ptr*(*)(struct _tuple10*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,(
(_tmp13C1="function declaration has repeated parameter",_tag_dyneither(_tmp13C1,sizeof(char),44))));}
# 4988
fd->cached_typ=t;
# 4983
;_pop_region(r);};}
# 4993
void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,int allow_abs_aggr,void*t){
# 4999
struct _RegionHandle _tmpC86=_new_region("r");struct _RegionHandle*r=& _tmpC86;_push_region(r);
{struct Cyc_Tcutil_CVTEnv _tmp13C2;struct Cyc_Tcutil_CVTEnv _tmpC87=Cyc_Tcutil_check_valid_type(loc,te,(
(_tmp13C2.r=r,((_tmp13C2.kind_env=bound_tvars,((_tmp13C2.free_vars=0,((_tmp13C2.free_evars=0,((_tmp13C2.generalize_evars=0,((_tmp13C2.fn_result=0,_tmp13C2)))))))))))),expected_kind,allow_abs_aggr,t);
# 5003
struct Cyc_List_List*_tmpC88=Cyc_Tcutil_remove_bound_tvars(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmpC87.free_vars),bound_tvars);
# 5005
struct Cyc_List_List*_tmpC89=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmpC87.free_evars);
{struct Cyc_List_List*fs=_tmpC88;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmpC8A=((struct Cyc_Absyn_Tvar*)fs->hd)->name;
const char*_tmp13C7;void*_tmp13C6[2];struct Cyc_String_pa_PrintArg_struct _tmp13C5;struct Cyc_String_pa_PrintArg_struct _tmp13C4;(_tmp13C4.tag=0,((_tmp13C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp13C5.tag=0,((_tmp13C5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpC8A),((_tmp13C6[0]=& _tmp13C5,((_tmp13C6[1]=& _tmp13C4,Cyc_Tcutil_terr(loc,((_tmp13C7="unbound type variable %s in type %s",_tag_dyneither(_tmp13C7,sizeof(char),36))),_tag_dyneither(_tmp13C6,sizeof(void*),2)))))))))))));}}
# 5010
if(!allow_evars)
Cyc_Tcutil_check_free_evars(_tmpC89,t,loc);}
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
if(cmp(vs->hd,vs2->hd)== 0){
const char*_tmp13CC;void*_tmp13CB[2];struct Cyc_String_pa_PrintArg_struct _tmp13CA;struct Cyc_String_pa_PrintArg_struct _tmp13C9;(_tmp13C9.tag=0,((_tmp13C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string(vs->hd)),((_tmp13CA.tag=0,((_tmp13CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp13CB[0]=& _tmp13CA,((_tmp13CB[1]=& _tmp13C9,Cyc_Tcutil_terr(loc,((_tmp13CC="%s: %s",_tag_dyneither(_tmp13CC,sizeof(char),7))),_tag_dyneither(_tmp13CB,sizeof(void*),2)))))))))))));}}}}
# 5035
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s){
return*s;}
# 5039
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr msg){
((void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 5043
void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
const char*_tmp13CD;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,((_tmp13CD="duplicate type variable",_tag_dyneither(_tmp13CD,sizeof(char),24))));}struct _tuple32{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple33{struct Cyc_List_List*f1;void*f2;};struct _tuple34{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 5057 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 5062
struct _RegionHandle _tmpC95=_new_region("temp");struct _RegionHandle*temp=& _tmpC95;_push_region(temp);
# 5066
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
const char*_tmp13CE;if(Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,((_tmp13CE="",_tag_dyneither(_tmp13CE,sizeof(char),1))))!= 0){
struct _tuple32*_tmp13D1;struct Cyc_List_List*_tmp13D0;fields=((_tmp13D0=_region_malloc(temp,sizeof(*_tmp13D0)),((_tmp13D0->hd=((_tmp13D1=_region_malloc(temp,sizeof(*_tmp13D1)),((_tmp13D1->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,((_tmp13D1->f2=0,_tmp13D1)))))),((_tmp13D0->tl=fields,_tmp13D0))))));}}}
# 5071
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 5073
const char*_tmp13D3;const char*_tmp13D2;struct _dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?(_tmp13D3="struct",_tag_dyneither(_tmp13D3,sizeof(char),7)):((_tmp13D2="union",_tag_dyneither(_tmp13D2,sizeof(char),6)));
# 5076
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple33*_tmpC99=(struct _tuple33*)des->hd;struct Cyc_List_List*_tmpC9B;void*_tmpC9C;struct _tuple33*_tmpC9A=_tmpC99;_tmpC9B=_tmpC9A->f1;_tmpC9C=_tmpC9A->f2;
if(_tmpC9B == 0){
# 5081
struct Cyc_List_List*_tmpC9D=fields;
for(0;_tmpC9D != 0;_tmpC9D=_tmpC9D->tl){
if(!(*((struct _tuple32*)_tmpC9D->hd)).f2){
(*((struct _tuple32*)_tmpC9D->hd)).f2=1;
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp13D9;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp13D8;struct Cyc_List_List*_tmp13D7;(*((struct _tuple33*)des->hd)).f1=((_tmp13D7=_cycalloc(sizeof(*_tmp13D7)),((_tmp13D7->hd=(void*)((_tmp13D9=_cycalloc(sizeof(*_tmp13D9)),((_tmp13D9[0]=((_tmp13D8.tag=1,((_tmp13D8.f1=((*((struct _tuple32*)_tmpC9D->hd)).f1)->name,_tmp13D8)))),_tmp13D9)))),((_tmp13D7->tl=0,_tmp13D7))))));}
{struct _tuple34*_tmp13DC;struct Cyc_List_List*_tmp13DB;ans=((_tmp13DB=_region_malloc(rgn,sizeof(*_tmp13DB)),((_tmp13DB->hd=((_tmp13DC=_region_malloc(rgn,sizeof(*_tmp13DC)),((_tmp13DC->f1=(*((struct _tuple32*)_tmpC9D->hd)).f1,((_tmp13DC->f2=_tmpC9C,_tmp13DC)))))),((_tmp13DB->tl=ans,_tmp13DB))))));}
break;}}
# 5089
if(_tmpC9D == 0){
const char*_tmp13E0;void*_tmp13DF[1];struct Cyc_String_pa_PrintArg_struct _tmp13DE;(_tmp13DE.tag=0,((_tmp13DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp13DF[0]=& _tmp13DE,Cyc_Tcutil_terr(loc,((_tmp13E0="too many arguments to %s",_tag_dyneither(_tmp13E0,sizeof(char),25))),_tag_dyneither(_tmp13DF,sizeof(void*),1)))))));}}else{
if(_tmpC9B->tl != 0){
# 5093
const char*_tmp13E3;void*_tmp13E2;(_tmp13E2=0,Cyc_Tcutil_terr(loc,((_tmp13E3="multiple designators are not yet supported",_tag_dyneither(_tmp13E3,sizeof(char),43))),_tag_dyneither(_tmp13E2,sizeof(void*),0)));}else{
# 5096
void*_tmpCA8=(void*)_tmpC9B->hd;void*_tmpCA9=_tmpCA8;struct _dyneither_ptr*_tmpCAC;_LL712: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpCAA=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpCA9;if(_tmpCAA->tag != 0)goto _LL714;}_LL713:
# 5098
{const char*_tmp13E7;void*_tmp13E6[1];struct Cyc_String_pa_PrintArg_struct _tmp13E5;(_tmp13E5.tag=0,((_tmp13E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp13E6[0]=& _tmp13E5,Cyc_Tcutil_terr(loc,((_tmp13E7="array designator used in argument to %s",_tag_dyneither(_tmp13E7,sizeof(char),40))),_tag_dyneither(_tmp13E6,sizeof(void*),1)))))));}
goto _LL711;_LL714: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpCAB=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpCA9;if(_tmpCAB->tag != 1)goto _LL711;else{_tmpCAC=_tmpCAB->f1;}}_LL715: {
# 5101
struct Cyc_List_List*_tmpCB0=fields;
for(0;_tmpCB0 != 0;_tmpCB0=_tmpCB0->tl){
if(Cyc_strptrcmp(_tmpCAC,((*((struct _tuple32*)_tmpCB0->hd)).f1)->name)== 0){
if((*((struct _tuple32*)_tmpCB0->hd)).f2){
const char*_tmp13EB;void*_tmp13EA[1];struct Cyc_String_pa_PrintArg_struct _tmp13E9;(_tmp13E9.tag=0,((_tmp13E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpCAC),((_tmp13EA[0]=& _tmp13E9,Cyc_Tcutil_terr(loc,((_tmp13EB="member %s has already been used as an argument",_tag_dyneither(_tmp13EB,sizeof(char),47))),_tag_dyneither(_tmp13EA,sizeof(void*),1)))))));}
(*((struct _tuple32*)_tmpCB0->hd)).f2=1;
{struct _tuple34*_tmp13EE;struct Cyc_List_List*_tmp13ED;ans=((_tmp13ED=_region_malloc(rgn,sizeof(*_tmp13ED)),((_tmp13ED->hd=((_tmp13EE=_region_malloc(rgn,sizeof(*_tmp13EE)),((_tmp13EE->f1=(*((struct _tuple32*)_tmpCB0->hd)).f1,((_tmp13EE->f2=_tmpC9C,_tmp13EE)))))),((_tmp13ED->tl=ans,_tmp13ED))))));}
break;}}
# 5110
if(_tmpCB0 == 0){
const char*_tmp13F2;void*_tmp13F1[1];struct Cyc_String_pa_PrintArg_struct _tmp13F0;(_tmp13F0.tag=0,((_tmp13F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpCAC),((_tmp13F1[0]=& _tmp13F0,Cyc_Tcutil_terr(loc,((_tmp13F2="bad field designator %s",_tag_dyneither(_tmp13F2,sizeof(char),24))),_tag_dyneither(_tmp13F1,sizeof(void*),1)))))));}
goto _LL711;}_LL711:;}}}
# 5115
if(aggr_kind == Cyc_Absyn_StructA)
# 5117
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple32*)fields->hd)).f2){
{const char*_tmp13F5;void*_tmp13F4;(_tmp13F4=0,Cyc_Tcutil_terr(loc,((_tmp13F5="too few arguments to struct",_tag_dyneither(_tmp13F5,sizeof(char),28))),_tag_dyneither(_tmp13F4,sizeof(void*),0)));}
break;}}else{
# 5124
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple32*)fields->hd)).f2){
if(found){const char*_tmp13F8;void*_tmp13F7;(_tmp13F7=0,Cyc_Tcutil_terr(loc,((_tmp13F8="only one member of a union is allowed",_tag_dyneither(_tmp13F8,sizeof(char),38))),_tag_dyneither(_tmp13F7,sizeof(void*),0)));}
found=1;}}
# 5131
if(!found){const char*_tmp13FB;void*_tmp13FA;(_tmp13FA=0,Cyc_Tcutil_terr(loc,((_tmp13FB="missing member for union",_tag_dyneither(_tmp13FB,sizeof(char),25))),_tag_dyneither(_tmp13FA,sizeof(void*),0)));}}{
# 5134
struct Cyc_List_List*_tmpCBF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0);return _tmpCBF;};};}
# 5066
;_pop_region(temp);}
# 5137
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmpCC2=Cyc_Tcutil_compress(t);void*_tmpCC3=_tmpCC2;void*_tmpCC5;union Cyc_Absyn_Constraint*_tmpCC6;_LL717: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCC4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCC3;if(_tmpCC4->tag != 5)goto _LL719;else{_tmpCC5=(_tmpCC4->f1).elt_typ;_tmpCC6=((_tmpCC4->f1).ptr_atts).bounds;}}_LL718: {
# 5140
void*_tmpCC7=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpCC6);void*_tmpCC8=_tmpCC7;_LL71C: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpCC9=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpCC8;if(_tmpCC9->tag != 0)goto _LL71E;}_LL71D:
# 5142
*elt_typ_dest=_tmpCC5;
return 1;_LL71E:;_LL71F:
 return 0;_LL71B:;}_LL719:;_LL71A:
# 5146
 return 0;_LL716:;}
# 5150
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmpCCA=Cyc_Tcutil_compress(t);void*_tmpCCB=_tmpCCA;void*_tmpCCD;union Cyc_Absyn_Constraint*_tmpCCE;_LL721: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCCC=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCCB;if(_tmpCCC->tag != 5)goto _LL723;else{_tmpCCD=(_tmpCCC->f1).elt_typ;_tmpCCE=((_tmpCCC->f1).ptr_atts).zero_term;}}_LL722:
# 5153
*elt_typ_dest=_tmpCCD;
return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpCCE);_LL723:;_LL724:
 return 0;_LL720:;}
# 5161
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type){
# 5163
void*_tmpCCF=Cyc_Tcutil_compress(t);void*_tmpCD0=_tmpCCF;void*_tmpCD2;union Cyc_Absyn_Constraint*_tmpCD3;union Cyc_Absyn_Constraint*_tmpCD4;void*_tmpCD6;struct Cyc_Absyn_Tqual _tmpCD7;struct Cyc_Absyn_Exp*_tmpCD8;union Cyc_Absyn_Constraint*_tmpCD9;_LL726: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCD1=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCD0;if(_tmpCD1->tag != 5)goto _LL728;else{_tmpCD2=(_tmpCD1->f1).elt_typ;_tmpCD3=((_tmpCD1->f1).ptr_atts).bounds;_tmpCD4=((_tmpCD1->f1).ptr_atts).zero_term;}}_LL727:
# 5165
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpCD4)){
*ptr_type=t;
*elt_type=_tmpCD2;
{void*_tmpCDA=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmpCD3);void*_tmpCDB=_tmpCDA;_LL72D: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpCDC=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpCDB;if(_tmpCDC->tag != 0)goto _LL72F;}_LL72E:
*is_dyneither=1;goto _LL72C;_LL72F:;_LL730:
*is_dyneither=0;goto _LL72C;_LL72C:;}
# 5172
return 1;}else{
return 0;}_LL728: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCD5=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpCD0;if(_tmpCD5->tag != 8)goto _LL72A;else{_tmpCD6=(_tmpCD5->f1).elt_type;_tmpCD7=(_tmpCD5->f1).tq;_tmpCD8=(_tmpCD5->f1).num_elts;_tmpCD9=(_tmpCD5->f1).zero_term;}}_LL729:
# 5175
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpCD9)){
*elt_type=_tmpCD6;
*is_dyneither=0;
*ptr_type=Cyc_Tcutil_promote_array(t,(void*)& Cyc_Absyn_HeapRgn_val,0);
return 1;}else{
return 0;}_LL72A:;_LL72B:
 return 0;_LL725:;}
# 5188
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){
# 5190
void*_tmpCDD=e1->r;void*_tmpCDE=_tmpCDD;struct Cyc_Absyn_Exp*_tmpCE1;struct Cyc_Absyn_Exp*_tmpCE3;struct Cyc_Absyn_Exp*_tmpCE5;struct Cyc_Absyn_Exp*_tmpCE7;struct Cyc_Absyn_Exp*_tmpCE9;struct Cyc_Absyn_Exp*_tmpCEB;_LL732: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpCDF=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpCDE;if(_tmpCDF->tag != 13)goto _LL734;}_LL733: {
# 5192
const char*_tmp13FF;void*_tmp13FE[1];struct Cyc_String_pa_PrintArg_struct _tmp13FD;(_tmp13FD.tag=0,((_tmp13FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp13FE[0]=& _tmp13FD,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13FF="we have a cast in a lhs:  %s",_tag_dyneither(_tmp13FF,sizeof(char),29))),_tag_dyneither(_tmp13FE,sizeof(void*),1)))))));}_LL734: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpCE0=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpCDE;if(_tmpCE0->tag != 19)goto _LL736;else{_tmpCE1=_tmpCE0->f1;}}_LL735:
 _tmpCE3=_tmpCE1;goto _LL737;_LL736: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpCE2=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpCDE;if(_tmpCE2->tag != 22)goto _LL738;else{_tmpCE3=_tmpCE2->f1;}}_LL737:
# 5195
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpCE3->topt),ptr_type,is_dyneither,elt_type);_LL738: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpCE4=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpCDE;if(_tmpCE4->tag != 21)goto _LL73A;else{_tmpCE5=_tmpCE4->f1;}}_LL739:
 _tmpCE7=_tmpCE5;goto _LL73B;_LL73A: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpCE6=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpCDE;if(_tmpCE6->tag != 20)goto _LL73C;else{_tmpCE7=_tmpCE6->f1;}}_LL73B:
# 5199
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpCE7->topt),ptr_type,is_dyneither,elt_type)){
const char*_tmp1403;void*_tmp1402[1];struct Cyc_String_pa_PrintArg_struct _tmp1401;(_tmp1401.tag=0,((_tmp1401.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1)),((_tmp1402[0]=& _tmp1401,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1403="found zero pointer aggregate member assignment: %s",_tag_dyneither(_tmp1403,sizeof(char),51))),_tag_dyneither(_tmp1402,sizeof(void*),1)))))));}
return 0;_LL73C: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpCE8=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpCDE;if(_tmpCE8->tag != 12)goto _LL73E;else{_tmpCE9=_tmpCE8->f1;}}_LL73D:
 _tmpCEB=_tmpCE9;goto _LL73F;_LL73E: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpCEA=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpCDE;if(_tmpCEA->tag != 11)goto _LL740;else{_tmpCEB=_tmpCEA->f1;}}_LL73F:
# 5205
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpCEB->topt),ptr_type,is_dyneither,elt_type)){
const char*_tmp1407;void*_tmp1406[1];struct Cyc_String_pa_PrintArg_struct _tmp1405;(_tmp1405.tag=0,((_tmp1405.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1)),((_tmp1406[0]=& _tmp1405,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1407="found zero pointer instantiate/noinstantiate: %s",_tag_dyneither(_tmp1407,sizeof(char),49))),_tag_dyneither(_tmp1406,sizeof(void*),1)))))));}
return 0;_LL740: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpCEC=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpCDE;if(_tmpCEC->tag != 1)goto _LL742;}_LL741:
 return 0;_LL742:;_LL743: {
# 5211
const char*_tmp140B;void*_tmp140A[1];struct Cyc_String_pa_PrintArg_struct _tmp1409;(_tmp1409.tag=0,((_tmp1409.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp140A[0]=& _tmp1409,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp140B="found bad lhs in is_zero_ptr_deref: %s",_tag_dyneither(_tmp140B,sizeof(char),39))),_tag_dyneither(_tmp140A,sizeof(void*),1)))))));}_LL731:;}
# 5215
int Cyc_Tcutil_is_tagged_pointer_typ(void*t){
void*ignore=(void*)& Cyc_Absyn_VoidType_val;
return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
# 5226
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
void*_tmpCF9=Cyc_Tcutil_compress(r);void*_tmpCFA=_tmpCF9;struct Cyc_Absyn_Tvar*_tmpCFE;_LL745: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpCFB=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmpCFA;if(_tmpCFB->tag != 22)goto _LL747;}_LL746:
 return !must_be_unique;_LL747: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpCFC=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmpCFA;if(_tmpCFC->tag != 21)goto _LL749;}_LL748:
 return 1;_LL749: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpCFD=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpCFA;if(_tmpCFD->tag != 2)goto _LL74B;else{_tmpCFE=_tmpCFD->f1;}}_LL74A: {
# 5231
struct Cyc_Absyn_Kind*_tmpCFF=Cyc_Tcutil_tvar_kind(_tmpCFE,& Cyc_Tcutil_rk);enum Cyc_Absyn_KindQual _tmpD01;enum Cyc_Absyn_AliasQual _tmpD02;struct Cyc_Absyn_Kind*_tmpD00=_tmpCFF;_tmpD01=_tmpD00->kind;_tmpD02=_tmpD00->aliasqual;
if(_tmpD01 == Cyc_Absyn_RgnKind  && (_tmpD02 == Cyc_Absyn_Unique  || _tmpD02 == Cyc_Absyn_Top  && !must_be_unique)){
void*_tmpD03=Cyc_Absyn_compress_kb(_tmpCFE->kind);void*_tmpD04=_tmpD03;struct Cyc_Core_Opt**_tmpD06;_LL74E: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpD05=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpD04;if(_tmpD05->tag != 2)goto _LL750;else{_tmpD06=(struct Cyc_Core_Opt**)& _tmpD05->f1;if((_tmpD05->f2)->kind != Cyc_Absyn_RgnKind)goto _LL750;if((_tmpD05->f2)->aliasqual != Cyc_Absyn_Top)goto _LL750;}}_LL74F:
# 5235
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1411;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1410;struct Cyc_Core_Opt*_tmp140F;*_tmpD06=((_tmp140F=_cycalloc(sizeof(*_tmp140F)),((_tmp140F->v=(void*)((_tmp1411=_cycalloc(sizeof(*_tmp1411)),((_tmp1411[0]=((_tmp1410.tag=2,((_tmp1410.f1=0,((_tmp1410.f2=& Cyc_Tcutil_rk,_tmp1410)))))),_tmp1411)))),_tmp140F))));}
return 0;_LL750:;_LL751:
 return 1;_LL74D:;}
# 5240
return 0;}_LL74B:;_LL74C:
 return 0;_LL744:;}
# 5247
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmpD0A=Cyc_Tcutil_compress(t);void*_tmpD0B=_tmpD0A;void*_tmpD0D;struct Cyc_Absyn_Tvar*_tmpD0F;_LL753: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD0C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD0B;if(_tmpD0C->tag != 5)goto _LL755;else{_tmpD0D=((_tmpD0C->f1).ptr_atts).rgn;}}_LL754:
# 5250
 return Cyc_Tcutil_is_noalias_region(_tmpD0D,must_be_unique);_LL755: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpD0E=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpD0B;if(_tmpD0E->tag != 2)goto _LL757;else{_tmpD0F=_tmpD0E->f1;}}_LL756: {
# 5252
struct Cyc_Absyn_Kind*_tmpD10=Cyc_Tcutil_tvar_kind(_tmpD0F,& Cyc_Tcutil_bk);enum Cyc_Absyn_KindQual _tmpD12;enum Cyc_Absyn_AliasQual _tmpD13;struct Cyc_Absyn_Kind*_tmpD11=_tmpD10;_tmpD12=_tmpD11->kind;_tmpD13=_tmpD11->aliasqual;
switch(_tmpD12){case Cyc_Absyn_BoxKind: _LL759:
 goto _LL75A;case Cyc_Absyn_AnyKind: _LL75A: goto _LL75B;case Cyc_Absyn_MemKind: _LL75B:
 if(_tmpD13 == Cyc_Absyn_Unique  || _tmpD13 == Cyc_Absyn_Top  && !must_be_unique){
void*_tmpD14=Cyc_Absyn_compress_kb(_tmpD0F->kind);void*_tmpD15=_tmpD14;struct Cyc_Core_Opt**_tmpD17;enum Cyc_Absyn_KindQual _tmpD18;_LL75E: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpD16=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpD15;if(_tmpD16->tag != 2)goto _LL760;else{_tmpD17=(struct Cyc_Core_Opt**)& _tmpD16->f1;_tmpD18=(_tmpD16->f2)->kind;if((_tmpD16->f2)->aliasqual != Cyc_Absyn_Top)goto _LL760;}}_LL75F:
# 5258
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp141B;struct Cyc_Absyn_Kind*_tmp141A;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1419;struct Cyc_Core_Opt*_tmp1418;*_tmpD17=((_tmp1418=_cycalloc(sizeof(*_tmp1418)),((_tmp1418->v=(void*)((_tmp141B=_cycalloc(sizeof(*_tmp141B)),((_tmp141B[0]=((_tmp1419.tag=2,((_tmp1419.f1=0,((_tmp1419.f2=((_tmp141A=_cycalloc_atomic(sizeof(*_tmp141A)),((_tmp141A->kind=_tmpD18,((_tmp141A->aliasqual=Cyc_Absyn_Aliasable,_tmp141A)))))),_tmp1419)))))),_tmp141B)))),_tmp1418))));}
return 0;_LL760:;_LL761:
# 5262
 return 1;_LL75D:;}
# 5265
return 0;default: _LL75C:
 return 0;}}_LL757:;_LL758:
# 5268
 return 0;_LL752:;}
# 5271
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*rgn,void*t){
void*_tmpD1D=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(_tmpD1D,0))return 1;{
void*_tmpD1E=_tmpD1D;struct Cyc_List_List*_tmpD20;struct Cyc_Absyn_Aggrdecl**_tmpD22;struct Cyc_List_List*_tmpD23;struct Cyc_List_List*_tmpD25;union Cyc_Absyn_DatatypeInfoU _tmpD28;struct Cyc_List_List*_tmpD29;union Cyc_Absyn_DatatypeFieldInfoU _tmpD2B;struct Cyc_List_List*_tmpD2C;_LL764: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpD1F=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpD1E;if(_tmpD1F->tag != 10)goto _LL766;else{_tmpD20=_tmpD1F->f1;}}_LL765:
# 5276
 while(_tmpD20 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(*((struct _tuple12*)_tmpD20->hd)).f2))return 1;
_tmpD20=_tmpD20->tl;}
# 5280
return 0;_LL766: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD21=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpD1E;if(_tmpD21->tag != 11)goto _LL768;else{if((((_tmpD21->f1).aggr_info).KnownAggr).tag != 2)goto _LL768;_tmpD22=(struct Cyc_Absyn_Aggrdecl**)(((_tmpD21->f1).aggr_info).KnownAggr).val;_tmpD23=(_tmpD21->f1).targs;}}_LL767:
# 5282
 if((*_tmpD22)->impl == 0)return 0;else{
# 5284
struct Cyc_List_List*_tmpD2D=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,(*_tmpD22)->tvs,_tmpD23);
struct Cyc_List_List*_tmpD2E=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpD22)->impl))->fields;
void*t;
while(_tmpD2E != 0){
t=_tmpD2D == 0?((struct Cyc_Absyn_Aggrfield*)_tmpD2E->hd)->type: Cyc_Tcutil_rsubstitute(rgn,_tmpD2D,((struct Cyc_Absyn_Aggrfield*)_tmpD2E->hd)->type);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpD2E=_tmpD2E->tl;}
# 5292
return 0;}_LL768: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpD24=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpD1E;if(_tmpD24->tag != 12)goto _LL76A;else{_tmpD25=_tmpD24->f2;}}_LL769:
# 5295
 while(_tmpD25 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,((struct Cyc_Absyn_Aggrfield*)_tmpD25->hd)->type))return 1;
_tmpD25=_tmpD25->tl;}
# 5299
return 0;_LL76A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD26=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpD1E;if(_tmpD26->tag != 11)goto _LL76C;else{if((((_tmpD26->f1).aggr_info).UnknownAggr).tag != 1)goto _LL76C;}}_LL76B:
# 5302
 return 0;_LL76C: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpD27=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpD1E;if(_tmpD27->tag != 3)goto _LL76E;else{_tmpD28=(_tmpD27->f1).datatype_info;_tmpD29=(_tmpD27->f1).targs;}}_LL76D: {
# 5304
union Cyc_Absyn_DatatypeInfoU _tmpD2F=_tmpD28;struct _tuple2*_tmpD30;int _tmpD31;struct Cyc_List_List*_tmpD32;struct Cyc_Core_Opt*_tmpD33;_LL773: if((_tmpD2F.UnknownDatatype).tag != 1)goto _LL775;_tmpD30=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpD2F.UnknownDatatype).val).name;_tmpD31=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpD2F.UnknownDatatype).val).is_extensible;_LL774:
# 5307
 return 0;_LL775: if((_tmpD2F.KnownDatatype).tag != 2)goto _LL772;_tmpD32=(*((struct Cyc_Absyn_Datatypedecl**)(_tmpD2F.KnownDatatype).val))->tvs;_tmpD33=(*((struct Cyc_Absyn_Datatypedecl**)(_tmpD2F.KnownDatatype).val))->fields;_LL776:
# 5310
 return 0;_LL772:;}_LL76E: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpD2A=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpD1E;if(_tmpD2A->tag != 4)goto _LL770;else{_tmpD2B=(_tmpD2A->f1).field_info;_tmpD2C=(_tmpD2A->f1).targs;}}_LL76F: {
# 5313
union Cyc_Absyn_DatatypeFieldInfoU _tmpD34=_tmpD2B;struct Cyc_Absyn_Datatypedecl*_tmpD35;struct Cyc_Absyn_Datatypefield*_tmpD36;_LL778: if((_tmpD34.UnknownDatatypefield).tag != 1)goto _LL77A;_LL779:
# 5316
 return 0;_LL77A: if((_tmpD34.KnownDatatypefield).tag != 2)goto _LL777;_tmpD35=((struct _tuple3)(_tmpD34.KnownDatatypefield).val).f1;_tmpD36=((struct _tuple3)(_tmpD34.KnownDatatypefield).val).f2;_LL77B: {
# 5318
struct Cyc_List_List*_tmpD37=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpD35->tvs,_tmpD2C);
struct Cyc_List_List*_tmpD38=_tmpD36->typs;
while(_tmpD38 != 0){
_tmpD1D=_tmpD37 == 0?(*((struct _tuple12*)_tmpD38->hd)).f2: Cyc_Tcutil_rsubstitute(rgn,_tmpD37,(*((struct _tuple12*)_tmpD38->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,_tmpD1D))return 1;
_tmpD38=_tmpD38->tl;}
# 5325
return 0;}_LL777:;}_LL770:;_LL771:
# 5327
 return 0;_LL763:;};}
# 5331
static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,struct _dyneither_ptr*f){
void*_tmpD39=Cyc_Tcutil_compress(t);void*_tmpD3A=_tmpD39;struct Cyc_Absyn_Aggrdecl*_tmpD3C;struct Cyc_List_List*_tmpD3D;struct Cyc_List_List*_tmpD3F;_LL77D: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD3B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpD3A;if(_tmpD3B->tag != 11)goto _LL77F;else{if((((_tmpD3B->f1).aggr_info).KnownAggr).tag != 2)goto _LL77F;_tmpD3C=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpD3B->f1).aggr_info).KnownAggr).val);_tmpD3D=(_tmpD3B->f1).targs;}}_LL77E:
# 5334
 _tmpD3F=_tmpD3C->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD3C->impl))->fields;goto _LL780;_LL77F: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpD3E=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpD3A;if(_tmpD3E->tag != 12)goto _LL781;else{_tmpD3F=_tmpD3E->f2;}}_LL780: {
# 5336
struct Cyc_Absyn_Aggrfield*_tmpD40=Cyc_Absyn_lookup_field(_tmpD3F,f);
{struct Cyc_Absyn_Aggrfield*_tmpD41=_tmpD40;void*_tmpD42;_LL784: if(_tmpD41 != 0)goto _LL786;_LL785: {
const char*_tmp141E;void*_tmp141D;(_tmp141D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp141E="is_noalias_field: missing field",_tag_dyneither(_tmp141E,sizeof(char),32))),_tag_dyneither(_tmp141D,sizeof(void*),0)));}_LL786: if(_tmpD41 == 0)goto _LL783;_tmpD42=_tmpD41->type;_LL787:
# 5340
 return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpD42);_LL783:;}
# 5342
return 0;}_LL781:;_LL782: {
# 5344
const char*_tmp1422;void*_tmp1421[1];struct Cyc_String_pa_PrintArg_struct _tmp1420;(_tmp1420.tag=0,((_tmp1420.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp1421[0]=& _tmp1420,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1422="is_noalias_field: invalid type |%s|",_tag_dyneither(_tmp1422,sizeof(char),36))),_tag_dyneither(_tmp1421,sizeof(void*),1)))))));}_LL77C:;}
# 5352
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
void*_tmpD48=e->r;void*_tmpD49=_tmpD48;struct Cyc_Absyn_Exp*_tmpD4D;struct Cyc_Absyn_Exp*_tmpD4F;struct Cyc_Absyn_Exp*_tmpD51;struct _dyneither_ptr*_tmpD52;struct Cyc_Absyn_Exp*_tmpD54;struct Cyc_Absyn_Exp*_tmpD55;struct Cyc_Absyn_Exp*_tmpD57;struct Cyc_Absyn_Exp*_tmpD58;struct Cyc_Absyn_Exp*_tmpD5A;struct Cyc_Absyn_Exp*_tmpD5C;struct Cyc_Absyn_Stmt*_tmpD5E;_LL789: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpD4A=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD49;if(_tmpD4A->tag != 1)goto _LL78B;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpD4B=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmpD4A->f2);if(_tmpD4B->tag != 1)goto _LL78B;}}_LL78A:
 return 0;_LL78B: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpD4C=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpD49;if(_tmpD4C->tag != 21)goto _LL78D;else{_tmpD4D=_tmpD4C->f1;}}_LL78C:
 _tmpD4F=_tmpD4D;goto _LL78E;_LL78D: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpD4E=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpD49;if(_tmpD4E->tag != 19)goto _LL78F;else{_tmpD4F=_tmpD4E->f1;}}_LL78E:
# 5357
 return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmpD4F->topt),1) && 
Cyc_Tcutil_is_noalias_path(r,_tmpD4F);_LL78F: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpD50=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpD49;if(_tmpD50->tag != 20)goto _LL791;else{_tmpD51=_tmpD50->f1;_tmpD52=_tmpD50->f2;}}_LL790:
 return Cyc_Tcutil_is_noalias_path(r,_tmpD51);_LL791: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpD53=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpD49;if(_tmpD53->tag != 22)goto _LL793;else{_tmpD54=_tmpD53->f1;_tmpD55=_tmpD53->f2;}}_LL792: {
# 5361
void*_tmpD5F=Cyc_Tcutil_compress((void*)_check_null(_tmpD54->topt));void*_tmpD60=_tmpD5F;_LL79E: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpD61=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpD60;if(_tmpD61->tag != 10)goto _LL7A0;}_LL79F:
 return Cyc_Tcutil_is_noalias_path(r,_tmpD54);_LL7A0:;_LL7A1:
 return 0;_LL79D:;}_LL793: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpD56=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD49;if(_tmpD56->tag != 5)goto _LL795;else{_tmpD57=_tmpD56->f2;_tmpD58=_tmpD56->f3;}}_LL794:
# 5366
 return Cyc_Tcutil_is_noalias_path(r,_tmpD57) && Cyc_Tcutil_is_noalias_path(r,_tmpD58);_LL795: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpD59=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpD49;if(_tmpD59->tag != 8)goto _LL797;else{_tmpD5A=_tmpD59->f2;}}_LL796:
 _tmpD5C=_tmpD5A;goto _LL798;_LL797: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpD5B=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD49;if(_tmpD5B->tag != 13)goto _LL799;else{_tmpD5C=_tmpD5B->f2;}}_LL798:
 return Cyc_Tcutil_is_noalias_path(r,_tmpD5C);_LL799: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmpD5D=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpD49;if(_tmpD5D->tag != 36)goto _LL79B;else{_tmpD5E=_tmpD5D->f1;}}_LL79A:
# 5370
 while(1){
void*_tmpD62=_tmpD5E->r;void*_tmpD63=_tmpD62;struct Cyc_Absyn_Stmt*_tmpD65;struct Cyc_Absyn_Stmt*_tmpD66;struct Cyc_Absyn_Decl*_tmpD68;struct Cyc_Absyn_Stmt*_tmpD69;struct Cyc_Absyn_Exp*_tmpD6B;_LL7A3: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpD64=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpD63;if(_tmpD64->tag != 2)goto _LL7A5;else{_tmpD65=_tmpD64->f1;_tmpD66=_tmpD64->f2;}}_LL7A4:
 _tmpD5E=_tmpD66;goto _LL7A2;_LL7A5: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpD67=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpD63;if(_tmpD67->tag != 12)goto _LL7A7;else{_tmpD68=_tmpD67->f1;_tmpD69=_tmpD67->f2;}}_LL7A6:
 _tmpD5E=_tmpD69;goto _LL7A2;_LL7A7: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpD6A=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpD63;if(_tmpD6A->tag != 1)goto _LL7A9;else{_tmpD6B=_tmpD6A->f1;}}_LL7A8:
 return Cyc_Tcutil_is_noalias_path(r,_tmpD6B);_LL7A9:;_LL7AA: {
const char*_tmp1425;void*_tmp1424;(_tmp1424=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1425="is_noalias_stmt_exp: ill-formed StmtExp",_tag_dyneither(_tmp1425,sizeof(char),40))),_tag_dyneither(_tmp1424,sizeof(void*),0)));}_LL7A2:;}_LL79B:;_LL79C:
# 5378
 return 1;_LL788:;}
# 5395 "tcutil.cyc"
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5397
struct _tuple18 _tmp1426;struct _tuple18 bogus_ans=(_tmp1426.f1=0,((_tmp1426.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp1426)));
void*_tmpD6E=e->r;void*_tmpD6F=_tmpD6E;struct _tuple2*_tmpD71;void*_tmpD72;struct Cyc_Absyn_Exp*_tmpD74;struct _dyneither_ptr*_tmpD75;int _tmpD76;struct Cyc_Absyn_Exp*_tmpD78;struct _dyneither_ptr*_tmpD79;int _tmpD7A;struct Cyc_Absyn_Exp*_tmpD7C;struct Cyc_Absyn_Exp*_tmpD7E;struct Cyc_Absyn_Exp*_tmpD7F;_LL7AC: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpD70=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD6F;if(_tmpD70->tag != 1)goto _LL7AE;else{_tmpD71=_tmpD70->f1;_tmpD72=(void*)_tmpD70->f2;}}_LL7AD: {
# 5401
void*_tmpD80=_tmpD72;struct Cyc_Absyn_Vardecl*_tmpD84;struct Cyc_Absyn_Vardecl*_tmpD86;struct Cyc_Absyn_Vardecl*_tmpD88;struct Cyc_Absyn_Vardecl*_tmpD8A;_LL7B9: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpD81=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpD80;if(_tmpD81->tag != 0)goto _LL7BB;}_LL7BA:
 goto _LL7BC;_LL7BB: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpD82=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpD80;if(_tmpD82->tag != 2)goto _LL7BD;}_LL7BC:
 return bogus_ans;_LL7BD: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpD83=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpD80;if(_tmpD83->tag != 1)goto _LL7BF;else{_tmpD84=_tmpD83->f1;}}_LL7BE: {
# 5405
void*_tmpD8B=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpD8C=_tmpD8B;_LL7C6: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD8D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD8C;if(_tmpD8D->tag != 8)goto _LL7C8;}_LL7C7: {
# 5407
struct _tuple18 _tmp1427;return(_tmp1427.f1=1,((_tmp1427.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp1427)));}_LL7C8:;_LL7C9: {
struct _tuple18 _tmp1428;return(_tmp1428.f1=(_tmpD84->tq).real_const,((_tmp1428.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp1428)));}_LL7C5:;}_LL7BF: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD85=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpD80;if(_tmpD85->tag != 4)goto _LL7C1;else{_tmpD86=_tmpD85->f1;}}_LL7C0: {
# 5411
void*_tmpD90=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpD91=_tmpD90;_LL7CB: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD92=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD91;if(_tmpD92->tag != 8)goto _LL7CD;}_LL7CC: {
struct _tuple18 _tmp1429;return(_tmp1429.f1=1,((_tmp1429.f2=(void*)_check_null(_tmpD86->rgn),_tmp1429)));}_LL7CD:;_LL7CE:
# 5414
 _tmpD86->escapes=1;{
struct _tuple18 _tmp142A;return(_tmp142A.f1=(_tmpD86->tq).real_const,((_tmp142A.f2=(void*)_check_null(_tmpD86->rgn),_tmp142A)));};_LL7CA:;}_LL7C1: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpD87=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpD80;if(_tmpD87->tag != 5)goto _LL7C3;else{_tmpD88=_tmpD87->f1;}}_LL7C2:
# 5417
 _tmpD8A=_tmpD88;goto _LL7C4;_LL7C3: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpD89=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpD80;if(_tmpD89->tag != 3)goto _LL7B8;else{_tmpD8A=_tmpD89->f1;}}_LL7C4:
# 5419
 _tmpD8A->escapes=1;{
struct _tuple18 _tmp142B;return(_tmp142B.f1=(_tmpD8A->tq).real_const,((_tmp142B.f2=(void*)_check_null(_tmpD8A->rgn),_tmp142B)));};_LL7B8:;}_LL7AE: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpD73=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpD6F;if(_tmpD73->tag != 20)goto _LL7B0;else{_tmpD74=_tmpD73->f1;_tmpD75=_tmpD73->f2;_tmpD76=_tmpD73->f3;}}_LL7AF:
# 5424
 if(_tmpD76)return bogus_ans;{
# 5427
void*_tmpD96=Cyc_Tcutil_compress((void*)_check_null(_tmpD74->topt));void*_tmpD97=_tmpD96;struct Cyc_List_List*_tmpD99;struct Cyc_Absyn_Aggrdecl*_tmpD9B;_LL7D0: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpD98=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpD97;if(_tmpD98->tag != 12)goto _LL7D2;else{_tmpD99=_tmpD98->f2;}}_LL7D1: {
# 5429
struct Cyc_Absyn_Aggrfield*_tmpD9C=Cyc_Absyn_lookup_field(_tmpD99,_tmpD75);
if(_tmpD9C != 0  && _tmpD9C->width == 0){
struct _tuple18 _tmpD9D=Cyc_Tcutil_addressof_props(te,_tmpD74);int _tmpD9F;void*_tmpDA0;struct _tuple18 _tmpD9E=_tmpD9D;_tmpD9F=_tmpD9E.f1;_tmpDA0=_tmpD9E.f2;{
struct _tuple18 _tmp142C;return(_tmp142C.f1=(_tmpD9C->tq).real_const  || _tmpD9F,((_tmp142C.f2=_tmpDA0,_tmp142C)));};}
# 5434
return bogus_ans;}_LL7D2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD9A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpD97;if(_tmpD9A->tag != 11)goto _LL7D4;else{if((((_tmpD9A->f1).aggr_info).KnownAggr).tag != 2)goto _LL7D4;_tmpD9B=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpD9A->f1).aggr_info).KnownAggr).val);}}_LL7D3: {
# 5436
struct Cyc_Absyn_Aggrfield*_tmpDA2=Cyc_Absyn_lookup_decl_field(_tmpD9B,_tmpD75);
if(_tmpDA2 != 0  && _tmpDA2->width == 0){
struct _tuple18 _tmpDA3=Cyc_Tcutil_addressof_props(te,_tmpD74);int _tmpDA5;void*_tmpDA6;struct _tuple18 _tmpDA4=_tmpDA3;_tmpDA5=_tmpDA4.f1;_tmpDA6=_tmpDA4.f2;{
struct _tuple18 _tmp142D;return(_tmp142D.f1=(_tmpDA2->tq).real_const  || _tmpDA5,((_tmp142D.f2=_tmpDA6,_tmp142D)));};}
# 5441
return bogus_ans;}_LL7D4:;_LL7D5:
 return bogus_ans;_LL7CF:;};_LL7B0: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpD77=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpD6F;if(_tmpD77->tag != 21)goto _LL7B2;else{_tmpD78=_tmpD77->f1;_tmpD79=_tmpD77->f2;_tmpD7A=_tmpD77->f3;}}_LL7B1:
# 5446
 if(_tmpD7A)return bogus_ans;{
# 5450
void*_tmpDA8=Cyc_Tcutil_compress((void*)_check_null(_tmpD78->topt));void*_tmpDA9=_tmpDA8;void*_tmpDAB;void*_tmpDAC;_LL7D7: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpDAA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpDA9;if(_tmpDAA->tag != 5)goto _LL7D9;else{_tmpDAB=(_tmpDAA->f1).elt_typ;_tmpDAC=((_tmpDAA->f1).ptr_atts).rgn;}}_LL7D8: {
# 5452
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmpDAD=Cyc_Tcutil_compress(_tmpDAB);void*_tmpDAE=_tmpDAD;struct Cyc_List_List*_tmpDB0;struct Cyc_Absyn_Aggrdecl*_tmpDB2;_LL7DC: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpDAF=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpDAE;if(_tmpDAF->tag != 12)goto _LL7DE;else{_tmpDB0=_tmpDAF->f2;}}_LL7DD:
# 5455
 finfo=Cyc_Absyn_lookup_field(_tmpDB0,_tmpD79);goto _LL7DB;_LL7DE: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpDB1=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpDAE;if(_tmpDB1->tag != 11)goto _LL7E0;else{if((((_tmpDB1->f1).aggr_info).KnownAggr).tag != 2)goto _LL7E0;_tmpDB2=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpDB1->f1).aggr_info).KnownAggr).val);}}_LL7DF:
# 5457
 finfo=Cyc_Absyn_lookup_decl_field(_tmpDB2,_tmpD79);goto _LL7DB;_LL7E0:;_LL7E1:
 return bogus_ans;_LL7DB:;}
# 5460
if(finfo != 0  && finfo->width == 0){
struct _tuple18 _tmp142E;return(_tmp142E.f1=(finfo->tq).real_const,((_tmp142E.f2=_tmpDAC,_tmp142E)));}
return bogus_ans;}_LL7D9:;_LL7DA:
 return bogus_ans;_LL7D6:;};_LL7B2: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpD7B=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpD6F;if(_tmpD7B->tag != 19)goto _LL7B4;else{_tmpD7C=_tmpD7B->f1;}}_LL7B3: {
# 5467
void*_tmpDB4=Cyc_Tcutil_compress((void*)_check_null(_tmpD7C->topt));void*_tmpDB5=_tmpDB4;struct Cyc_Absyn_Tqual _tmpDB7;void*_tmpDB8;_LL7E3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpDB6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpDB5;if(_tmpDB6->tag != 5)goto _LL7E5;else{_tmpDB7=(_tmpDB6->f1).elt_tq;_tmpDB8=((_tmpDB6->f1).ptr_atts).rgn;}}_LL7E4: {
# 5469
struct _tuple18 _tmp142F;return(_tmp142F.f1=_tmpDB7.real_const,((_tmp142F.f2=_tmpDB8,_tmp142F)));}_LL7E5:;_LL7E6:
 return bogus_ans;_LL7E2:;}_LL7B4: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpD7D=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpD6F;if(_tmpD7D->tag != 22)goto _LL7B6;else{_tmpD7E=_tmpD7D->f1;_tmpD7F=_tmpD7D->f2;}}_LL7B5: {
# 5475
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmpD7E->topt));
void*_tmpDBA=t;struct Cyc_List_List*_tmpDBC;struct Cyc_Absyn_Tqual _tmpDBE;void*_tmpDBF;struct Cyc_Absyn_Tqual _tmpDC1;_LL7E8: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpDBB=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpDBA;if(_tmpDBB->tag != 10)goto _LL7EA;else{_tmpDBC=_tmpDBB->f1;}}_LL7E9: {
# 5479
struct _tuple14 _tmpDC2=Cyc_Evexp_eval_const_uint_exp(_tmpD7F);unsigned int _tmpDC4;int _tmpDC5;struct _tuple14 _tmpDC3=_tmpDC2;_tmpDC4=_tmpDC3.f1;_tmpDC5=_tmpDC3.f2;
if(!_tmpDC5)
return bogus_ans;{
struct _tuple12*_tmpDC6=Cyc_Absyn_lookup_tuple_field(_tmpDBC,(int)_tmpDC4);
if(_tmpDC6 != 0){
struct _tuple18 _tmp1430;return(_tmp1430.f1=((*_tmpDC6).f1).real_const,((_tmp1430.f2=(Cyc_Tcutil_addressof_props(te,_tmpD7E)).f2,_tmp1430)));}
return bogus_ans;};}_LL7EA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpDBD=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpDBA;if(_tmpDBD->tag != 5)goto _LL7EC;else{_tmpDBE=(_tmpDBD->f1).elt_tq;_tmpDBF=((_tmpDBD->f1).ptr_atts).rgn;}}_LL7EB: {
# 5487
struct _tuple18 _tmp1431;return(_tmp1431.f1=_tmpDBE.real_const,((_tmp1431.f2=_tmpDBF,_tmp1431)));}_LL7EC: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpDC0=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpDBA;if(_tmpDC0->tag != 8)goto _LL7EE;else{_tmpDC1=(_tmpDC0->f1).tq;}}_LL7ED: {
# 5493
struct _tuple18 _tmp1432;return(_tmp1432.f1=_tmpDC1.real_const,((_tmp1432.f2=(Cyc_Tcutil_addressof_props(te,_tmpD7E)).f2,_tmp1432)));}_LL7EE:;_LL7EF:
 return bogus_ans;_LL7E7:;}_LL7B6:;_LL7B7:
# 5497
{const char*_tmp1435;void*_tmp1434;(_tmp1434=0,Cyc_Tcutil_terr(e->loc,((_tmp1435="unary & applied to non-lvalue",_tag_dyneither(_tmp1435,sizeof(char),30))),_tag_dyneither(_tmp1434,sizeof(void*),0)));}
return bogus_ans;_LL7AB:;}
# 5504
void Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,union Cyc_Absyn_Constraint*b){
b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(b);{
void*_tmpDCD=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);void*_tmpDCE=_tmpDCD;struct Cyc_Absyn_Exp*_tmpDD1;_LL7F1: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpDCF=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpDCE;if(_tmpDCF->tag != 0)goto _LL7F3;}_LL7F2:
 return;_LL7F3: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpDD0=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpDCE;if(_tmpDD0->tag != 1)goto _LL7F0;else{_tmpDD1=_tmpDD0->f1;}}_LL7F4: {
# 5509
struct _tuple14 _tmpDD2=Cyc_Evexp_eval_const_uint_exp(_tmpDD1);unsigned int _tmpDD4;int _tmpDD5;struct _tuple14 _tmpDD3=_tmpDD2;_tmpDD4=_tmpDD3.f1;_tmpDD5=_tmpDD3.f2;
if(_tmpDD5  && _tmpDD4 <= i){
const char*_tmp143A;void*_tmp1439[2];struct Cyc_Int_pa_PrintArg_struct _tmp1438;struct Cyc_Int_pa_PrintArg_struct _tmp1437;(_tmp1437.tag=1,((_tmp1437.f1=(unsigned long)((int)i),((_tmp1438.tag=1,((_tmp1438.f1=(unsigned long)((int)_tmpDD4),((_tmp1439[0]=& _tmp1438,((_tmp1439[1]=& _tmp1437,Cyc_Tcutil_terr(loc,((_tmp143A="dereference is out of bounds: %d <= %d",_tag_dyneither(_tmp143A,sizeof(char),39))),_tag_dyneither(_tmp1439,sizeof(void*),2)))))))))))));}
return;}_LL7F0:;};}
# 5516
void Cyc_Tcutil_check_nonzero_bound(unsigned int loc,union Cyc_Absyn_Constraint*b){
Cyc_Tcutil_check_bound(loc,0,b);}
# 5520
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b){
void*_tmpDDA=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);void*_tmpDDB=_tmpDDA;struct Cyc_Absyn_Exp*_tmpDDD;_LL7F6: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpDDC=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpDDB;if(_tmpDDC->tag != 1)goto _LL7F8;else{_tmpDDD=_tmpDDC->f1;}}_LL7F7: {
# 5523
struct _tuple14 _tmpDDE=Cyc_Evexp_eval_const_uint_exp(_tmpDDD);unsigned int _tmpDE0;int _tmpDE1;struct _tuple14 _tmpDDF=_tmpDDE;_tmpDE0=_tmpDDF.f1;_tmpDE1=_tmpDDF.f2;
return _tmpDE1  && _tmpDE0 == 1;}_LL7F8:;_LL7F9:
 return 0;_LL7F5:;}
# 5529
int Cyc_Tcutil_bits_only(void*t){
void*_tmpDE2=Cyc_Tcutil_compress(t);void*_tmpDE3=_tmpDE2;void*_tmpDEA;union Cyc_Absyn_Constraint*_tmpDEB;struct Cyc_List_List*_tmpDED;struct Cyc_Absyn_Aggrdecl*_tmpDF0;struct Cyc_List_List*_tmpDF1;struct Cyc_List_List*_tmpDF3;_LL7FB: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpDE4=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpDE3;if(_tmpDE4->tag != 0)goto _LL7FD;}_LL7FC:
 goto _LL7FE;_LL7FD: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpDE5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpDE3;if(_tmpDE5->tag != 6)goto _LL7FF;}_LL7FE:
 goto _LL800;_LL7FF: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpDE6=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpDE3;if(_tmpDE6->tag != 7)goto _LL801;}_LL800:
 return 1;_LL801: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpDE7=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpDE3;if(_tmpDE7->tag != 13)goto _LL803;}_LL802:
 goto _LL804;_LL803: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpDE8=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpDE3;if(_tmpDE8->tag != 14)goto _LL805;}_LL804:
 return 0;_LL805: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpDE9=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpDE3;if(_tmpDE9->tag != 8)goto _LL807;else{_tmpDEA=(_tmpDE9->f1).elt_type;_tmpDEB=(_tmpDE9->f1).zero_term;}}_LL806:
# 5539
 return !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpDEB) && Cyc_Tcutil_bits_only(_tmpDEA);_LL807: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpDEC=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpDE3;if(_tmpDEC->tag != 10)goto _LL809;else{_tmpDED=_tmpDEC->f1;}}_LL808:
# 5541
 for(0;_tmpDED != 0;_tmpDED=_tmpDED->tl){
if(!Cyc_Tcutil_bits_only((*((struct _tuple12*)_tmpDED->hd)).f2))return 0;}
return 1;_LL809: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpDEE=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpDE3;if(_tmpDEE->tag != 11)goto _LL80B;else{if((((_tmpDEE->f1).aggr_info).UnknownAggr).tag != 1)goto _LL80B;}}_LL80A:
 return 0;_LL80B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpDEF=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpDE3;if(_tmpDEF->tag != 11)goto _LL80D;else{if((((_tmpDEF->f1).aggr_info).KnownAggr).tag != 2)goto _LL80D;_tmpDF0=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpDEF->f1).aggr_info).KnownAggr).val);_tmpDF1=(_tmpDEF->f1).targs;}}_LL80C:
# 5546
 if(_tmpDF0->impl == 0)
return 0;{
int okay=1;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpDF0->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
if(okay)return 1;{
struct _RegionHandle _tmpDF4=_new_region("rgn");struct _RegionHandle*rgn=& _tmpDF4;_push_region(rgn);
{struct Cyc_List_List*_tmpDF5=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpDF0->tvs,_tmpDF1);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpDF0->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpDF5,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpDF6=0;_npop_handler(0);return _tmpDF6;}}}{
int _tmpDF7=1;_npop_handler(0);return _tmpDF7;};}
# 5553
;_pop_region(rgn);};};_LL80D: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpDF2=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpDE3;if(_tmpDF2->tag != 12)goto _LL80F;else{_tmpDF3=_tmpDF2->f2;}}_LL80E:
# 5559
 for(0;_tmpDF3 != 0;_tmpDF3=_tmpDF3->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)_tmpDF3->hd)->type))return 0;}
return 1;_LL80F:;_LL810:
 return 0;_LL7FA:;}
# 5567
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e){
while(1){
void*_tmpDF8=e->r;void*_tmpDF9=_tmpDF8;struct Cyc_Absyn_Exp*_tmpDFC;struct Cyc_Absyn_Exp*_tmpDFE;_LL812: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpDFA=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpDF9;if(_tmpDFA->tag != 1)goto _LL814;}_LL813:
 return 1;_LL814: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpDFB=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpDF9;if(_tmpDFB->tag != 11)goto _LL816;else{_tmpDFC=_tmpDFB->f1;}}_LL815:
 _tmpDFE=_tmpDFC;goto _LL817;_LL816: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpDFD=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpDF9;if(_tmpDFD->tag != 12)goto _LL818;else{_tmpDFE=_tmpDFD->f1;}}_LL817:
 e=_tmpDFE;continue;_LL818:;_LL819:
# 5574
 return 0;_LL811:;}}
# 5584
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmpDFF=e->r;void*_tmpE00=_tmpDFF;struct _tuple2*_tmpE08;void*_tmpE09;struct Cyc_Absyn_Exp*_tmpE0B;struct Cyc_Absyn_Exp*_tmpE0C;struct Cyc_Absyn_Exp*_tmpE0D;struct Cyc_Absyn_Exp*_tmpE0F;struct Cyc_Absyn_Exp*_tmpE10;struct Cyc_Absyn_Exp*_tmpE12;struct Cyc_Absyn_Exp*_tmpE14;void*_tmpE16;struct Cyc_Absyn_Exp*_tmpE17;void*_tmpE19;struct Cyc_Absyn_Exp*_tmpE1A;struct Cyc_Absyn_Exp*_tmpE1C;struct Cyc_Absyn_Exp*_tmpE1E;struct Cyc_Absyn_Exp*_tmpE1F;struct Cyc_Absyn_Exp*_tmpE21;struct Cyc_List_List*_tmpE23;struct Cyc_List_List*_tmpE25;struct Cyc_List_List*_tmpE27;enum Cyc_Absyn_Primop _tmpE29;struct Cyc_List_List*_tmpE2A;struct Cyc_List_List*_tmpE2C;struct Cyc_List_List*_tmpE2E;_LL81B: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE01=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpE00;if(_tmpE01->tag != 0)goto _LL81D;}_LL81C:
 goto _LL81E;_LL81D: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpE02=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpE00;if(_tmpE02->tag != 16)goto _LL81F;}_LL81E:
 goto _LL820;_LL81F: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpE03=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpE00;if(_tmpE03->tag != 17)goto _LL821;}_LL820:
 goto _LL822;_LL821: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpE04=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpE00;if(_tmpE04->tag != 18)goto _LL823;}_LL822:
 goto _LL824;_LL823: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpE05=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpE00;if(_tmpE05->tag != 31)goto _LL825;}_LL824:
 goto _LL826;_LL825: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpE06=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpE00;if(_tmpE06->tag != 32)goto _LL827;}_LL826:
 return 1;_LL827: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpE07=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpE00;if(_tmpE07->tag != 1)goto _LL829;else{_tmpE08=_tmpE07->f1;_tmpE09=(void*)_tmpE07->f2;}}_LL828: {
# 5595
void*_tmpE2F=_tmpE09;struct Cyc_Absyn_Vardecl*_tmpE32;struct Cyc_Absyn_Vardecl*_tmpE34;_LL84A: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpE30=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpE2F;if(_tmpE30->tag != 2)goto _LL84C;}_LL84B:
 return 1;_LL84C: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpE31=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpE2F;if(_tmpE31->tag != 1)goto _LL84E;else{_tmpE32=_tmpE31->f1;}}_LL84D: {
# 5598
void*_tmpE36=Cyc_Tcutil_compress(_tmpE32->type);void*_tmpE37=_tmpE36;_LL855: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpE38=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE37;if(_tmpE38->tag != 8)goto _LL857;}_LL856:
 goto _LL858;_LL857: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE39=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE37;if(_tmpE39->tag != 9)goto _LL859;}_LL858:
 return 1;_LL859:;_LL85A:
 return var_okay;_LL854:;}_LL84E: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpE33=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpE2F;if(_tmpE33->tag != 4)goto _LL850;else{_tmpE34=_tmpE33->f1;}}_LL84F:
# 5605
 if(_tmpE34->sc == Cyc_Absyn_Static){
void*_tmpE3A=Cyc_Tcutil_compress(_tmpE34->type);void*_tmpE3B=_tmpE3A;_LL85C: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpE3C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE3B;if(_tmpE3C->tag != 8)goto _LL85E;}_LL85D:
 goto _LL85F;_LL85E: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE3D=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE3B;if(_tmpE3D->tag != 9)goto _LL860;}_LL85F:
 return 1;_LL860:;_LL861:
 return var_okay;_LL85B:;}else{
# 5612
return var_okay;}_LL850: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpE35=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpE2F;if(_tmpE35->tag != 0)goto _LL852;}_LL851:
 return 0;_LL852:;_LL853:
 return var_okay;_LL849:;}_LL829: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpE0A=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpE00;if(_tmpE0A->tag != 5)goto _LL82B;else{_tmpE0B=_tmpE0A->f1;_tmpE0C=_tmpE0A->f2;_tmpE0D=_tmpE0A->f3;}}_LL82A:
# 5618
 return(Cyc_Tcutil_cnst_exp(0,_tmpE0B) && 
Cyc_Tcutil_cnst_exp(0,_tmpE0C)) && 
Cyc_Tcutil_cnst_exp(0,_tmpE0D);_LL82B: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpE0E=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpE00;if(_tmpE0E->tag != 8)goto _LL82D;else{_tmpE0F=_tmpE0E->f1;_tmpE10=_tmpE0E->f2;}}_LL82C:
# 5622
 return Cyc_Tcutil_cnst_exp(0,_tmpE0F) && Cyc_Tcutil_cnst_exp(0,_tmpE10);_LL82D: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpE11=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpE00;if(_tmpE11->tag != 11)goto _LL82F;else{_tmpE12=_tmpE11->f1;}}_LL82E:
 _tmpE14=_tmpE12;goto _LL830;_LL82F: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpE13=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpE00;if(_tmpE13->tag != 12)goto _LL831;else{_tmpE14=_tmpE13->f1;}}_LL830:
# 5625
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpE14);_LL831: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpE15=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpE00;if(_tmpE15->tag != 13)goto _LL833;else{_tmpE16=(void*)_tmpE15->f1;_tmpE17=_tmpE15->f2;if(_tmpE15->f4 != Cyc_Absyn_No_coercion)goto _LL833;}}_LL832:
# 5627
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpE17);_LL833: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpE18=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpE00;if(_tmpE18->tag != 13)goto _LL835;else{_tmpE19=(void*)_tmpE18->f1;_tmpE1A=_tmpE18->f2;}}_LL834:
# 5630
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpE1A);_LL835: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmpE1B=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpE00;if(_tmpE1B->tag != 14)goto _LL837;else{_tmpE1C=_tmpE1B->f1;}}_LL836:
# 5632
 return Cyc_Tcutil_cnst_exp(1,_tmpE1C);_LL837: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpE1D=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpE00;if(_tmpE1D->tag != 26)goto _LL839;else{_tmpE1E=_tmpE1D->f2;_tmpE1F=_tmpE1D->f3;}}_LL838:
# 5634
 return Cyc_Tcutil_cnst_exp(0,_tmpE1E) && Cyc_Tcutil_cnst_exp(0,_tmpE1F);_LL839: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpE20=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpE00;if(_tmpE20->tag != 27)goto _LL83B;else{_tmpE21=_tmpE20->f1;}}_LL83A:
# 5636
 return Cyc_Tcutil_cnst_exp(0,_tmpE21);_LL83B: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpE22=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpE00;if(_tmpE22->tag != 25)goto _LL83D;else{_tmpE23=_tmpE22->f1;}}_LL83C:
 _tmpE25=_tmpE23;goto _LL83E;_LL83D: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpE24=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpE00;if(_tmpE24->tag != 29)goto _LL83F;else{_tmpE25=_tmpE24->f2;}}_LL83E:
 _tmpE27=_tmpE25;goto _LL840;_LL83F: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpE26=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpE00;if(_tmpE26->tag != 28)goto _LL841;else{_tmpE27=_tmpE26->f3;}}_LL840:
# 5640
 for(0;_tmpE27 != 0;_tmpE27=_tmpE27->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple19*)_tmpE27->hd)).f2))
return 0;}
return 1;_LL841: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpE28=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpE00;if(_tmpE28->tag != 2)goto _LL843;else{_tmpE29=_tmpE28->f1;_tmpE2A=_tmpE28->f2;}}_LL842:
# 5645
 _tmpE2C=_tmpE2A;goto _LL844;_LL843: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmpE2B=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpE00;if(_tmpE2B->tag != 23)goto _LL845;else{_tmpE2C=_tmpE2B->f1;}}_LL844:
 _tmpE2E=_tmpE2C;goto _LL846;_LL845: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpE2D=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpE00;if(_tmpE2D->tag != 30)goto _LL847;else{_tmpE2E=_tmpE2D->f1;}}_LL846:
# 5648
 for(0;_tmpE2E != 0;_tmpE2E=_tmpE2E->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmpE2E->hd))
return 0;}
return 1;_LL847:;_LL848:
 return 0;_LL81A:;}
# 5656
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 5660
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);
# 5662
int Cyc_Tcutil_supports_default(void*t){
void*_tmpE3E=Cyc_Tcutil_compress(t);void*_tmpE3F=_tmpE3E;union Cyc_Absyn_Constraint*_tmpE44;union Cyc_Absyn_Constraint*_tmpE45;void*_tmpE47;struct Cyc_List_List*_tmpE49;union Cyc_Absyn_AggrInfoU _tmpE4B;struct Cyc_List_List*_tmpE4C;struct Cyc_List_List*_tmpE4E;_LL863: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpE40=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpE3F;if(_tmpE40->tag != 0)goto _LL865;}_LL864:
 goto _LL866;_LL865: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpE41=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpE3F;if(_tmpE41->tag != 6)goto _LL867;}_LL866:
 goto _LL868;_LL867: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpE42=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpE3F;if(_tmpE42->tag != 7)goto _LL869;}_LL868:
 return 1;_LL869: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE43=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE3F;if(_tmpE43->tag != 5)goto _LL86B;else{_tmpE44=((_tmpE43->f1).ptr_atts).nullable;_tmpE45=((_tmpE43->f1).ptr_atts).bounds;}}_LL86A: {
# 5669
void*_tmpE51=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpE45);void*_tmpE52=_tmpE51;_LL87A: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpE53=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpE52;if(_tmpE53->tag != 0)goto _LL87C;}_LL87B:
 return 1;_LL87C:;_LL87D:
# 5672
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmpE44);_LL879:;}_LL86B: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpE46=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE3F;if(_tmpE46->tag != 8)goto _LL86D;else{_tmpE47=(_tmpE46->f1).elt_type;}}_LL86C:
# 5675
 return Cyc_Tcutil_supports_default(_tmpE47);_LL86D: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpE48=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpE3F;if(_tmpE48->tag != 10)goto _LL86F;else{_tmpE49=_tmpE48->f1;}}_LL86E:
# 5677
 for(0;_tmpE49 != 0;_tmpE49=_tmpE49->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple12*)_tmpE49->hd)).f2))return 0;}
return 1;_LL86F: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpE4A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpE3F;if(_tmpE4A->tag != 11)goto _LL871;else{_tmpE4B=(_tmpE4A->f1).aggr_info;_tmpE4C=(_tmpE4A->f1).targs;}}_LL870: {
# 5681
struct Cyc_Absyn_Aggrdecl*_tmpE54=Cyc_Absyn_get_known_aggrdecl(_tmpE4B);
if(_tmpE54->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpE54->impl))->exist_vars != 0)return 0;
return Cyc_Tcutil_fields_support_default(_tmpE54->tvs,_tmpE4C,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpE54->impl))->fields);}_LL871: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpE4D=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpE3F;if(_tmpE4D->tag != 12)goto _LL873;else{_tmpE4E=_tmpE4D->f2;}}_LL872:
 return Cyc_Tcutil_fields_support_default(0,0,_tmpE4E);_LL873: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpE4F=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpE3F;if(_tmpE4F->tag != 14)goto _LL875;}_LL874:
# 5687
 goto _LL876;_LL875: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpE50=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpE3F;if(_tmpE50->tag != 13)goto _LL877;}_LL876:
 return 1;_LL877:;_LL878:
# 5690
 return 0;_LL862:;}
# 5695
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t){
void*_tmpE55=t;struct Cyc_Absyn_Typedefdecl*_tmpE57;_LL87F: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpE56=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpE55;if(_tmpE56->tag != 17)goto _LL881;else{_tmpE57=_tmpE56->f3;}}_LL880:
# 5698
 if(_tmpE57 != 0){
struct Cyc_Absyn_Tqual _tmpE58=_tmpE57->tq;
if(((_tmpE58.print_const  || _tmpE58.q_volatile) || _tmpE58.q_restrict) || _tmpE58.real_const){
# 5703
const char*_tmp143E;void*_tmp143D[1];struct Cyc_String_pa_PrintArg_struct _tmp143C;(_tmp143C.tag=0,((_tmp143C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp143D[0]=& _tmp143C,Cyc_Tcutil_warn(loc,((_tmp143E="qualifier within typedef type %s is ignored",_tag_dyneither(_tmp143E,sizeof(char),44))),_tag_dyneither(_tmp143D,sizeof(void*),1)))))));}}
# 5706
goto _LL87E;_LL881:;_LL882:
 goto _LL87E;_LL87E:;}
# 5711
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 5713
struct _RegionHandle _tmpE5C=_new_region("rgn");struct _RegionHandle*rgn=& _tmpE5C;_push_region(rgn);{
struct Cyc_List_List*_tmpE5D=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_supports_default(t)){int _tmpE5E=1;_npop_handler(0);return _tmpE5E;}
t=Cyc_Tcutil_rsubstitute(rgn,_tmpE5D,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpE5F=0;_npop_handler(0);return _tmpE5F;}}}{
# 5722
int _tmpE60=1;_npop_handler(0);return _tmpE60;};
# 5713
;_pop_region(rgn);}
# 5728
int Cyc_Tcutil_admits_zero(void*t){
void*_tmpE61=Cyc_Tcutil_compress(t);void*_tmpE62=_tmpE61;union Cyc_Absyn_Constraint*_tmpE66;union Cyc_Absyn_Constraint*_tmpE67;_LL884: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpE63=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpE62;if(_tmpE63->tag != 6)goto _LL886;}_LL885:
 goto _LL887;_LL886: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpE64=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpE62;if(_tmpE64->tag != 7)goto _LL888;}_LL887:
 return 1;_LL888: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE65=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE62;if(_tmpE65->tag != 5)goto _LL88A;else{_tmpE66=((_tmpE65->f1).ptr_atts).nullable;_tmpE67=((_tmpE65->f1).ptr_atts).bounds;}}_LL889: {
# 5733
void*_tmpE68=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpE67);void*_tmpE69=_tmpE68;_LL88D: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpE6A=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpE69;if(_tmpE6A->tag != 0)goto _LL88F;}_LL88E:
# 5737
 return 0;_LL88F:;_LL890:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpE66);_LL88C:;}_LL88A:;_LL88B:
# 5740
 return 0;_LL883:;}
# 5744
int Cyc_Tcutil_is_noreturn(void*t){
{void*_tmpE6B=Cyc_Tcutil_compress(t);void*_tmpE6C=_tmpE6B;void*_tmpE6E;struct Cyc_List_List*_tmpE70;_LL892: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE6D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE6C;if(_tmpE6D->tag != 5)goto _LL894;else{_tmpE6E=(_tmpE6D->f1).elt_typ;}}_LL893:
 return Cyc_Tcutil_is_noreturn(_tmpE6E);_LL894: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE6F=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE6C;if(_tmpE6F->tag != 9)goto _LL896;else{_tmpE70=(_tmpE6F->f1).attributes;}}_LL895:
# 5748
 for(0;_tmpE70 != 0;_tmpE70=_tmpE70->tl){
void*_tmpE71=(void*)_tmpE70->hd;void*_tmpE72=_tmpE71;_LL899: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmpE73=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmpE72;if(_tmpE73->tag != 4)goto _LL89B;}_LL89A:
 return 1;_LL89B:;_LL89C:
 continue;_LL898:;}
# 5753
goto _LL891;_LL896:;_LL897:
 goto _LL891;_LL891:;}
# 5756
return 0;}
# 5761
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmpE74=Cyc_Tcutil_compress(t);void*_tmpE75=_tmpE74;struct Cyc_List_List**_tmpE77;_LL89E: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE76=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE75;if(_tmpE76->tag != 9)goto _LL8A0;else{_tmpE77=(struct Cyc_List_List**)&(_tmpE76->f1).attributes;}}_LL89F: {
# 5764
struct Cyc_List_List*_tmpE78=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Tcutil_attribute_cmp,*_tmpE77,(void*)atts->hd)){
struct Cyc_List_List*_tmp143F;*_tmpE77=((_tmp143F=_cycalloc(sizeof(*_tmp143F)),((_tmp143F->hd=(void*)atts->hd,((_tmp143F->tl=*_tmpE77,_tmp143F))))));}}else{
# 5771
struct Cyc_List_List*_tmp1440;_tmpE78=((_tmp1440=_cycalloc(sizeof(*_tmp1440)),((_tmp1440->hd=(void*)atts->hd,((_tmp1440->tl=_tmpE78,_tmp1440))))));}}
return _tmpE78;}_LL8A0:;_LL8A1: {
const char*_tmp1443;void*_tmp1442;(_tmp1442=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1443="transfer_fn_type_atts",_tag_dyneither(_tmp1443,sizeof(char),22))),_tag_dyneither(_tmp1442,sizeof(void*),0)));}_LL89D:;}
# 5778
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
struct Cyc_Absyn_Exp*bound=0;
{void*_tmpE7D=Cyc_Tcutil_compress(t);void*_tmpE7E=_tmpE7D;struct Cyc_Absyn_PtrInfo*_tmpE80;struct Cyc_Absyn_Exp*_tmpE82;_LL8A3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE7F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE7E;if(_tmpE7F->tag != 5)goto _LL8A5;else{_tmpE80=(struct Cyc_Absyn_PtrInfo*)& _tmpE7F->f1;}}_LL8A4:
# 5782
{void*_tmpE83=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)((_tmpE80->ptr_atts).bounds);void*_tmpE84=_tmpE83;struct Cyc_Absyn_Exp*_tmpE86;_LL8AA: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpE85=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpE84;if(_tmpE85->tag != 1)goto _LL8AC;else{_tmpE86=_tmpE85->f1;}}_LL8AB:
 bound=_tmpE86;goto _LL8A9;_LL8AC:;_LL8AD:
 goto _LL8A9;_LL8A9:;}
# 5786
goto _LL8A2;_LL8A5: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpE81=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE7E;if(_tmpE81->tag != 8)goto _LL8A7;else{_tmpE82=(_tmpE81->f1).num_elts;}}_LL8A6:
# 5788
 bound=_tmpE82;
goto _LL8A2;_LL8A7:;_LL8A8:
 goto _LL8A2;_LL8A2:;}
# 5792
return bound;}
# 5797
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
{void*_tmpE87=b;struct Cyc_Absyn_Vardecl*_tmpE89;struct Cyc_Absyn_Vardecl*_tmpE8B;struct Cyc_Absyn_Vardecl*_tmpE8D;struct Cyc_Absyn_Vardecl*_tmpE8F;_LL8AF: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpE88=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpE87;if(_tmpE88->tag != 5)goto _LL8B1;else{_tmpE89=_tmpE88->f1;}}_LL8B0:
 _tmpE8B=_tmpE89;goto _LL8B2;_LL8B1: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpE8A=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpE87;if(_tmpE8A->tag != 4)goto _LL8B3;else{_tmpE8B=_tmpE8A->f1;}}_LL8B2:
 _tmpE8D=_tmpE8B;goto _LL8B4;_LL8B3: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpE8C=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpE87;if(_tmpE8C->tag != 3)goto _LL8B5;else{_tmpE8D=_tmpE8C->f1;}}_LL8B4:
 _tmpE8F=_tmpE8D;goto _LL8B6;_LL8B5: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpE8E=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpE87;if(_tmpE8E->tag != 1)goto _LL8B7;else{_tmpE8F=_tmpE8E->f1;}}_LL8B6:
# 5803
 if(!_tmpE8F->escapes)return _tmpE8F;
goto _LL8AE;_LL8B7:;_LL8B8:
 goto _LL8AE;_LL8AE:;}
# 5807
return 0;}
# 5811
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmpE90=0;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0){struct Cyc_List_List*_tmp1444;_tmpE90=((_tmp1444=_cycalloc(sizeof(*_tmp1444)),((_tmp1444->hd=*((void**)_check_null((void**)x->hd)),((_tmp1444->tl=_tmpE90,_tmp1444))))));}}}
return _tmpE90;}
# 5818
int Cyc_Tcutil_is_array(void*t){
# 5820
void*_tmpE92=Cyc_Tcutil_compress(t);void*_tmpE93=_tmpE92;_LL8BA: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpE94=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE93;if(_tmpE94->tag != 8)goto _LL8BC;}_LL8BB:
# 5822
 return 1;_LL8BC:;_LL8BD:
 return 0;_LL8B9:;}
# 5827
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
# 5829
void*_tmpE95=Cyc_Tcutil_compress(t);void*_tmpE96=_tmpE95;void*_tmpE98;struct Cyc_Absyn_Tqual _tmpE99;struct Cyc_Absyn_Exp*_tmpE9A;union Cyc_Absyn_Constraint*_tmpE9B;unsigned int _tmpE9C;_LL8BF: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpE97=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE96;if(_tmpE97->tag != 8)goto _LL8C1;else{_tmpE98=(_tmpE97->f1).elt_type;_tmpE99=(_tmpE97->f1).tq;_tmpE9A=(_tmpE97->f1).num_elts;_tmpE9B=(_tmpE97->f1).zero_term;_tmpE9C=(_tmpE97->f1).zt_loc;}}_LL8C0: {
# 5831
void*b;
if(_tmpE9A == 0)
b=(void*)& Cyc_Absyn_DynEither_b_val;else{
# 5835
struct Cyc_Absyn_Exp*bnd=_tmpE9A;
if(convert_tag){
if(bnd->topt == 0){
const char*_tmp1447;void*_tmp1446;(_tmp1446=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1447="cannot convert tag without type!",_tag_dyneither(_tmp1447,sizeof(char),33))),_tag_dyneither(_tmp1446,sizeof(void*),0)));}{
void*_tmpE9F=Cyc_Tcutil_compress((void*)_check_null(bnd->topt));void*_tmpEA0=_tmpE9F;void*_tmpEA2;_LL8C4: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpEA1=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpEA0;if(_tmpEA1->tag != 19)goto _LL8C6;else{_tmpEA2=(void*)_tmpEA1->f1;}}_LL8C5:
# 5841
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp144A;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp1449;b=(void*)((_tmp1449=_cycalloc(sizeof(*_tmp1449)),((_tmp1449[0]=((_tmp144A.tag=1,((_tmp144A.f1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmpEA2,0),0,Cyc_Absyn_No_coercion,0),_tmp144A)))),_tmp1449))));}
goto _LL8C3;_LL8C6:;_LL8C7:
# 5844
 if(Cyc_Tcutil_is_const_exp(bnd)){
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp144D;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp144C;b=(void*)((_tmp144C=_cycalloc(sizeof(*_tmp144C)),((_tmp144C[0]=((_tmp144D.tag=1,((_tmp144D.f1=bnd,_tmp144D)))),_tmp144C))));}else{
# 5847
b=(void*)& Cyc_Absyn_DynEither_b_val;}_LL8C3:;};}else{
# 5851
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp1450;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp144F;b=(void*)((_tmp144F=_cycalloc(sizeof(*_tmp144F)),((_tmp144F[0]=((_tmp1450.tag=1,((_tmp1450.f1=bnd,_tmp1450)))),_tmp144F))));}}
# 5853
return Cyc_Absyn_atb_typ(_tmpE98,rgn,_tmpE99,b,_tmpE9B);}_LL8C1:;_LL8C2:
# 5856
 return t;_LL8BE:;}
