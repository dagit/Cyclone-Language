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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 256
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{int tag;};
# 291
typedef void*Cyc_Absyn_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 338
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 357
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 369
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
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
# 548
extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;
# 559
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 566
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 584
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 944 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 946
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 954
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 957
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x);
# 961
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
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
# 982
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 984
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;
# 988
extern void*Cyc_Absyn_empty_effect;
# 998
extern struct _tuple2*Cyc_Absyn_datatype_print_arg_qvar;
extern struct _tuple2*Cyc_Absyn_datatype_scanf_arg_qvar;
# 1007
extern void*Cyc_Absyn_bounds_one;
# 1012
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 1038
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1048
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1055
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple2*,void*,unsigned int);
# 1057
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
# 1060
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1070
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1072
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned int);
# 1077
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1083
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
# 1095
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
# 1101
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1141
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);
# 1188
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1190
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1192
struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1194
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
# 1196
int Cyc_Absyn_fntype_att(void*a);struct _tuple13{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;};
# 1200
struct _tuple13 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1202
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
{void*_tmpEC=e->r;void*_tmpED=_tmpEC;int _tmpEE;struct _dyneither_ptr _tmpEF;void*_tmpF0;struct Cyc_Absyn_Exp*_tmpF1;struct _dyneither_ptr*_tmpF2;struct Cyc_Core_Opt*_tmpF3;struct Cyc_List_List*_tmpF4;struct Cyc_Absyn_Exp*_tmpF5;struct Cyc_Absyn_Exp*_tmpF6;int _tmpF7;struct Cyc_Absyn_Exp*_tmpF8;void**_tmpF9;struct Cyc_Absyn_Exp*_tmpFA;int _tmpFB;int _tmpFC;struct _tuple2*_tmpFD;void*_tmpFE;struct Cyc_Absyn_Enumfield*_tmpFF;struct _tuple2*_tmp100;struct Cyc_Absyn_Enumdecl*_tmp101;struct Cyc_Absyn_Enumfield*_tmp102;struct Cyc_List_List*_tmp103;struct Cyc_Absyn_Datatypedecl*_tmp104;struct Cyc_Absyn_Datatypefield*_tmp105;void*_tmp106;struct Cyc_List_List*_tmp107;struct _tuple2*_tmp108;struct Cyc_List_List*_tmp109;struct Cyc_List_List*_tmp10A;struct Cyc_Absyn_Aggrdecl*_tmp10B;struct Cyc_Absyn_Exp*_tmp10C;void*_tmp10D;int _tmp10E;struct Cyc_Absyn_Vardecl*_tmp10F;struct Cyc_Absyn_Exp*_tmp110;struct Cyc_Absyn_Exp*_tmp111;int _tmp112;struct Cyc_List_List*_tmp113;struct _dyneither_ptr*_tmp114;struct Cyc_Absyn_Tqual _tmp115;void*_tmp116;struct Cyc_List_List*_tmp117;struct Cyc_List_List*_tmp118;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp11B;struct _dyneither_ptr*_tmp11C;int _tmp11D;int _tmp11E;struct Cyc_Absyn_Exp*_tmp11F;struct _dyneither_ptr*_tmp120;int _tmp121;int _tmp122;struct Cyc_Absyn_Exp*_tmp123;void*_tmp124;struct Cyc_List_List*_tmp125;struct Cyc_Absyn_Exp*_tmp126;void*_tmp127;struct Cyc_Absyn_Exp*_tmp128;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_Absyn_Exp*_tmp12A;void*_tmp12B;struct Cyc_Absyn_Exp*_tmp12C;int _tmp12D;enum Cyc_Absyn_Coercion _tmp12E;struct Cyc_Absyn_Exp*_tmp12F;struct Cyc_List_List*_tmp130;struct Cyc_Absyn_Exp*_tmp131;struct Cyc_Absyn_Exp*_tmp132;int _tmp133;struct Cyc_Absyn_Exp*_tmp134;struct Cyc_List_List*_tmp135;struct Cyc_Absyn_VarargCallInfo*_tmp136;int _tmp137;struct Cyc_Absyn_Exp*_tmp138;struct Cyc_Absyn_Exp*_tmp139;struct Cyc_Absyn_Exp*_tmp13A;struct Cyc_Absyn_Exp*_tmp13B;struct Cyc_Absyn_Exp*_tmp13C;struct Cyc_Absyn_Exp*_tmp13D;struct Cyc_Absyn_Exp*_tmp13E;struct Cyc_Absyn_Exp*_tmp13F;struct Cyc_Absyn_Exp*_tmp140;struct Cyc_Absyn_Exp*_tmp141;enum Cyc_Absyn_Incrementor _tmp142;struct Cyc_Absyn_Exp*_tmp143;struct Cyc_Core_Opt*_tmp144;struct Cyc_Absyn_Exp*_tmp145;enum Cyc_Absyn_Primop _tmp146;struct Cyc_List_List*_tmp147;struct _tuple2*_tmp148;void*_tmp149;union Cyc_Absyn_Cnst _tmp14A;switch(*((int*)_tmpED)){case 0: _LL74: _tmp14A=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LL75:
 new_e=Cyc_Absyn_const_exp(_tmp14A,e->loc);goto _LL73;case 1: _LL76: _tmp148=((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp149=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL77:
 new_e=Cyc_Absyn_varb_exp(_tmp148,_tmp149,e->loc);goto _LL73;case 2: _LL78: _tmp146=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp147=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL79:
 new_e=Cyc_Absyn_primop_exp(_tmp146,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpEB,_tmp147),e->loc);goto _LL73;case 3: _LL7A: _tmp143=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp144=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp145=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_LL7B:
# 377
 new_e=Cyc_Absyn_assignop_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp143),(unsigned int)_tmp144?({struct Cyc_Core_Opt*_tmp14B=_cycalloc_atomic(sizeof(*_tmp14B));_tmp14B->v=(void*)_tmp144->v;_tmp14B;}): 0,Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp145),e->loc);
goto _LL73;case 4: _LL7C: _tmp141=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp142=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL7D:
 new_e=Cyc_Absyn_increment_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp141),_tmp142,e->loc);goto _LL73;case 5: _LL7E: _tmp13E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp13F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp140=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_LL7F:
# 381
 new_e=Cyc_Absyn_conditional_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp13E),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp13F),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp140),e->loc);goto _LL73;case 6: _LL80: _tmp13C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp13D=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL81:
 new_e=Cyc_Absyn_and_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp13C),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp13D),e->loc);goto _LL73;case 7: _LL82: _tmp13A=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp13B=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL83:
 new_e=Cyc_Absyn_or_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp13A),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp13B),e->loc);goto _LL73;case 8: _LL84: _tmp138=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp139=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL85:
 new_e=Cyc_Absyn_seq_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp138),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp139),e->loc);goto _LL73;case 9: _LL86: _tmp134=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp135=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp136=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_tmp137=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpED)->f4;_LL87:
# 386
{struct Cyc_Absyn_VarargCallInfo*_tmp14C=_tmp136;int _tmp14D;struct Cyc_List_List*_tmp14E;struct Cyc_Absyn_VarargInfo*_tmp14F;if(_tmp14C != 0){_LLC5: _tmp14D=_tmp14C->num_varargs;_tmp14E=_tmp14C->injectors;_tmp14F=_tmp14C->vai;_LLC6: {
# 388
struct Cyc_Absyn_VarargInfo*_tmp150=_tmp14F;struct _dyneither_ptr*_tmp151;struct Cyc_Absyn_Tqual _tmp152;void*_tmp153;int _tmp154;_LLCA: _tmp151=_tmp150->name;_tmp152=_tmp150->tq;_tmp153=_tmp150->type;_tmp154=_tmp150->inject;_LLCB:;
new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp155=_cycalloc(sizeof(*_tmp155));_tmp155[0]=({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp156;_tmp156.tag=9;_tmp156.f1=
Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp134);_tmp156.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpEB,_tmp135);_tmp156.f3=({struct Cyc_Absyn_VarargCallInfo*_tmp157=_cycalloc(sizeof(*_tmp157));_tmp157->num_varargs=_tmp14D;_tmp157->injectors=_tmp14E;_tmp157->vai=({struct Cyc_Absyn_VarargInfo*_tmp158=_cycalloc(sizeof(*_tmp158));_tmp158->name=_tmp151;_tmp158->tq=_tmp152;_tmp158->type=
# 392
Cyc_Tcutil_copy_type(_tmp153);_tmp158->inject=_tmp154;_tmp158;});_tmp157;});_tmp156.f4=_tmp137;_tmp156;});_tmp155;}),e->loc);
# 394
goto _LLC4;}}else{_LLC7: _LLC8:
# 396
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp159=_cycalloc(sizeof(*_tmp159));_tmp159[0]=({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp15A;_tmp15A.tag=9;_tmp15A.f1=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp134);_tmp15A.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpEB,_tmp135);_tmp15A.f3=_tmp136;_tmp15A.f4=_tmp137;_tmp15A;});_tmp159;}),e->loc);
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
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp15B=_cycalloc(sizeof(*_tmp15B));_tmp15B[0]=({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmp15C;_tmp15C.tag=20;_tmp15C.f1=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp11F);_tmp15C.f2=_tmp120;_tmp15C.f3=_tmp121;_tmp15C.f4=_tmp122;_tmp15C;});_tmp15B;}),e->loc);goto _LL73;case 21: _LL9E: _tmp11B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp11C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp11D=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_tmp11E=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpED)->f4;_LL9F:
# 424
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp15D=_cycalloc(sizeof(*_tmp15D));_tmp15D[0]=({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmp15E;_tmp15E.tag=21;_tmp15E.f1=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp11B);_tmp15E.f2=_tmp11C;_tmp15E.f3=_tmp11D;_tmp15E.f4=_tmp11E;_tmp15E;});_tmp15D;}),e->loc);goto _LL73;case 22: _LLA0: _tmp119=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp11A=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LLA1:
 new_e=Cyc_Absyn_subscript_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp119),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp11A),e->loc);
goto _LL73;case 23: _LLA2: _tmp118=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LLA3:
 new_e=Cyc_Absyn_tuple_exp(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpEB,_tmp118),e->loc);goto _LL73;case 24: _LLA4: _tmp114=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpED)->f1)->f1;_tmp115=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpED)->f1)->f2;_tmp116=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpED)->f1)->f3;_tmp117=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LLA5: {
# 429
struct _dyneither_ptr*vopt1=_tmp114;
if(_tmp114 != 0)vopt1=_tmp114;
new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp15F=_cycalloc(sizeof(*_tmp15F));_tmp15F[0]=({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmp160;_tmp160.tag=24;_tmp160.f1=({struct _tuple10*_tmp161=_cycalloc(sizeof(*_tmp161));_tmp161->f1=vopt1;_tmp161->f2=_tmp115;_tmp161->f3=Cyc_Tcutil_copy_type(_tmp116);_tmp161;});_tmp160.f2=
((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpEB,_tmp117);_tmp160;});_tmp15F;}),e->loc);
goto _LL73;}case 25: _LLA6: _tmp113=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LLA7:
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp162=_cycalloc(sizeof(*_tmp162));_tmp162[0]=({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp163;_tmp163.tag=25;_tmp163.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpEB,_tmp113);_tmp163;});_tmp162;}),e->loc);
goto _LL73;case 26: _LLA8: _tmp10F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp110=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp111=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_tmp112=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpED)->f4;_LLA9:
# 437
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp164=_cycalloc(sizeof(*_tmp164));_tmp164[0]=({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmp165;_tmp165.tag=26;_tmp165.f1=_tmp10F;_tmp165.f2=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp110);_tmp165.f3=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp111);_tmp165.f4=_tmp112;_tmp165;});_tmp164;}),e->loc);
goto _LL73;case 27: _LLAA: _tmp10C=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp10D=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp10E=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_LLAB:
# 440
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp166=_cycalloc(sizeof(*_tmp166));_tmp166[0]=({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmp167;_tmp167.tag=27;_tmp167.f1=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp10C);_tmp167.f2=Cyc_Tcutil_copy_type(_tmp10D);_tmp167.f3=_tmp10E;_tmp167;});_tmp166;}),_tmp10C->loc);
# 442
goto _LL73;case 28: _LLAC: _tmp108=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp109=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp10A=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_tmp10B=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpED)->f4;_LLAD:
# 444
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp168=_cycalloc(sizeof(*_tmp168));_tmp168[0]=({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp169;_tmp169.tag=28;_tmp169.f1=_tmp108;_tmp169.f2=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp109);_tmp169.f3=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpEB,_tmp10A);_tmp169.f4=_tmp10B;_tmp169;});_tmp168;}),e->loc);
# 446
goto _LL73;case 29: _LLAE: _tmp106=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp107=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LLAF:
# 448
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp16A=_cycalloc(sizeof(*_tmp16A));_tmp16A[0]=({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmp16B;_tmp16B.tag=29;_tmp16B.f1=Cyc_Tcutil_copy_type(_tmp106);_tmp16B.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpEB,_tmp107);_tmp16B;});_tmp16A;}),e->loc);
goto _LL73;case 30: _LLB0: _tmp103=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp104=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp105=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_LLB1:
# 451
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp16C=_cycalloc(sizeof(*_tmp16C));_tmp16C[0]=({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp16D;_tmp16D.tag=30;_tmp16D.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpEB,_tmp103);_tmp16D.f2=_tmp104;_tmp16D.f3=_tmp105;_tmp16D;});_tmp16C;}),e->loc);
goto _LL73;case 31: _LLB2: _tmp100=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp101=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp102=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_LLB3:
 new_e=e;goto _LL73;case 32: _LLB4: _tmpFD=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmpFE=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmpFF=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_LLB5:
# 455
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp16E=_cycalloc(sizeof(*_tmp16E));_tmp16E[0]=({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp16F;_tmp16F.tag=32;_tmp16F.f1=_tmpFD;_tmp16F.f2=Cyc_Tcutil_copy_type(_tmpFE);_tmp16F.f3=_tmpFF;_tmp16F;});_tmp16E;}),e->loc);
goto _LL73;case 33: _LLB6: _tmpF7=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpED)->f1).is_calloc;_tmpF8=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpED)->f1).rgn;_tmpF9=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpED)->f1).elt_type;_tmpFA=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpED)->f1).num_elts;_tmpFB=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpED)->f1).fat_result;_tmpFC=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpED)->f1).inline_call;_LLB7: {
# 458
struct Cyc_Absyn_Exp*_tmp170=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmpF8;if(_tmpF8 != 0)r1=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmpF8);{
void**t1=_tmpF9;if(_tmpF9 != 0)t1=({void**_tmp171=_cycalloc(sizeof(*_tmp171));_tmp171[0]=Cyc_Tcutil_copy_type(*_tmpF9);_tmp171;});
_tmp170->r=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp172=_cycalloc(sizeof(*_tmp172));_tmp172[0]=({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmp173;_tmp173.tag=33;_tmp173.f1=({struct Cyc_Absyn_MallocInfo _tmp174;_tmp174.is_calloc=_tmpF7;_tmp174.rgn=r1;_tmp174.elt_type=t1;_tmp174.num_elts=_tmpFA;_tmp174.fat_result=_tmpFB;_tmp174.inline_call=_tmpFC;_tmp174;});_tmp173;});_tmp172;});
new_e=_tmp170;
goto _LL73;};}case 34: _LLB8: _tmpF5=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmpF6=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LLB9:
 new_e=Cyc_Absyn_swap_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmpF5),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmpF6),e->loc);goto _LL73;case 35: _LLBA: _tmpF3=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmpF4=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LLBB: {
# 466
struct Cyc_Core_Opt*nopt1=_tmpF3;
if(_tmpF3 != 0)nopt1=({struct Cyc_Core_Opt*_tmp175=_cycalloc(sizeof(*_tmp175));_tmp175->v=(struct _tuple2*)_tmpF3->v;_tmp175;});
new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp176=_cycalloc(sizeof(*_tmp176));_tmp176[0]=({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp177;_tmp177.tag=35;_tmp177.f1=nopt1;_tmp177.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpEB,_tmpF4);_tmp177;});_tmp176;}),e->loc);
goto _LL73;}case 36: _LLBC: _LLBD:
# 471
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp178=_cycalloc(sizeof(*_tmp178));_tmp178[0]=({struct Cyc_Core_Failure_exn_struct _tmp179;_tmp179.tag=Cyc_Core_Failure;_tmp179.f1=({const char*_tmp17A="deep_copy: statement expressions unsupported";_tag_dyneither(_tmp17A,sizeof(char),45);});_tmp179;});_tmp178;}));case 37: _LLBE: _tmpF1=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmpF2=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LLBF:
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp17B=_cycalloc(sizeof(*_tmp17B));_tmp17B[0]=({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmp17C;_tmp17C.tag=37;_tmp17C.f1=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmpF1);_tmp17C.f2=_tmpF2;_tmp17C;});_tmp17B;}),e->loc);
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
struct Cyc_Absyn_Kind*_tmp17D=ka1;enum Cyc_Absyn_KindQual _tmp17E;enum Cyc_Absyn_AliasQual _tmp17F;_LLCD: _tmp17E=_tmp17D->kind;_tmp17F=_tmp17D->aliasqual;_LLCE:;{
struct Cyc_Absyn_Kind*_tmp180=ka2;enum Cyc_Absyn_KindQual _tmp181;enum Cyc_Absyn_AliasQual _tmp182;_LLD0: _tmp181=_tmp180->kind;_tmp182=_tmp180->aliasqual;_LLD1:;
# 500
if(_tmp17E != _tmp181){
struct _tuple20 _tmp183=({struct _tuple20 _tmp185;_tmp185.f1=_tmp17E;_tmp185.f2=_tmp181;_tmp185;});struct _tuple20 _tmp184=_tmp183;switch(_tmp184.f1){case Cyc_Absyn_BoxKind: switch(_tmp184.f2){case Cyc_Absyn_MemKind: _LLD3: _LLD4:
 goto _LLD6;case Cyc_Absyn_AnyKind: _LLD5: _LLD6:
 goto _LLD8;default: goto _LLD9;}case Cyc_Absyn_MemKind: if(_tmp184.f2 == Cyc_Absyn_AnyKind){_LLD7: _LLD8:
 goto _LLD2;}else{goto _LLD9;}default: _LLD9: _LLDA:
 return 0;}_LLD2:;}
# 509
if(_tmp17F != _tmp182){
struct _tuple21 _tmp186=({struct _tuple21 _tmp188;_tmp188.f1=_tmp17F;_tmp188.f2=_tmp182;_tmp188;});struct _tuple21 _tmp187=_tmp186;switch(_tmp187.f1){case Cyc_Absyn_Aliasable: if(_tmp187.f2 == Cyc_Absyn_Top){_LLDC: _LLDD:
 goto _LLDF;}else{goto _LLE0;}case Cyc_Absyn_Unique: if(_tmp187.f2 == Cyc_Absyn_Top){_LLDE: _LLDF:
 return 1;}else{goto _LLE0;}default: _LLE0: _LLE1:
 return 0;}_LLDB:;}
# 516
return 1;};}
# 519
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp189=Cyc_Absyn_compress_kb(tv->kind);void*_tmp18A=_tmp189;struct Cyc_Absyn_Kind*_tmp18B;struct Cyc_Absyn_Kind*_tmp18C;switch(*((int*)_tmp18A)){case 0: _LLE3: _tmp18C=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp18A)->f1;_LLE4:
 return _tmp18C;case 2: _LLE5: _tmp18B=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp18A)->f2;_LLE6:
 return _tmp18B;default: _LLE7: _LLE8:
# 524
 tv->kind=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp18D=_cycalloc(sizeof(*_tmp18D));_tmp18D[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp18E;_tmp18E.tag=2;_tmp18E.f1=0;_tmp18E.f2=def;_tmp18E;});_tmp18D;});
return def;}_LLE2:;}
# 529
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
struct _tuple0 _tmp18F=({struct _tuple0 _tmp1A6;_tmp1A6.f1=Cyc_Absyn_compress_kb(kb1);_tmp1A6.f2=Cyc_Absyn_compress_kb(kb2);_tmp1A6;});struct _tuple0 _tmp190=_tmp18F;struct Cyc_Core_Opt**_tmp191;void*_tmp192;void*_tmp193;struct Cyc_Core_Opt**_tmp194;struct Cyc_Core_Opt**_tmp195;struct Cyc_Absyn_Kind*_tmp196;struct Cyc_Core_Opt**_tmp197;struct Cyc_Absyn_Kind*_tmp198;struct Cyc_Core_Opt**_tmp199;struct Cyc_Absyn_Kind*_tmp19A;struct Cyc_Absyn_Kind*_tmp19B;struct Cyc_Absyn_Kind*_tmp19C;struct Cyc_Core_Opt**_tmp19D;struct Cyc_Absyn_Kind*_tmp19E;struct Cyc_Absyn_Kind*_tmp19F;struct Cyc_Absyn_Kind*_tmp1A0;switch(*((int*)_tmp190.f1)){case 0: switch(*((int*)_tmp190.f2)){case 0: _LLEA: _tmp19F=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp190.f1)->f1;_tmp1A0=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp190.f2)->f1;_LLEB:
 return _tmp19F == _tmp1A0;case 2: _LLEE: _tmp19C=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp190.f1)->f1;_tmp19D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp190.f2)->f1;_tmp19E=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp190.f2)->f2;_LLEF:
# 538
 if(Cyc_Tcutil_kind_leq(_tmp19C,_tmp19E)){
*_tmp19D=({struct Cyc_Core_Opt*_tmp1A2=_cycalloc(sizeof(*_tmp1A2));_tmp1A2->v=kb1;_tmp1A2;});
return 1;}else{
return 0;}default: goto _LLF4;}case 2: switch(*((int*)_tmp190.f2)){case 0: _LLEC: _tmp199=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp190.f1)->f1;_tmp19A=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp190.f1)->f2;_tmp19B=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp190.f2)->f1;_LLED:
# 533
 if(Cyc_Tcutil_kind_leq(_tmp19B,_tmp19A)){
*_tmp199=({struct Cyc_Core_Opt*_tmp1A1=_cycalloc(sizeof(*_tmp1A1));_tmp1A1->v=kb2;_tmp1A1;});
return 1;}else{
return 0;}case 2: _LLF0: _tmp195=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp190.f1)->f1;_tmp196=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp190.f1)->f2;_tmp197=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp190.f2)->f1;_tmp198=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp190.f2)->f2;_LLF1:
# 543
 if(Cyc_Tcutil_kind_leq(_tmp196,_tmp198)){
*_tmp197=({struct Cyc_Core_Opt*_tmp1A3=_cycalloc(sizeof(*_tmp1A3));_tmp1A3->v=kb1;_tmp1A3;});
return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp198,_tmp196)){
*_tmp195=({struct Cyc_Core_Opt*_tmp1A4=_cycalloc(sizeof(*_tmp1A4));_tmp1A4->v=kb2;_tmp1A4;});
return 1;}else{
return 0;}}default: _LLF4: _tmp193=_tmp190.f1;_tmp194=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp190.f2)->f1;_LLF5:
# 552
*_tmp194=({struct Cyc_Core_Opt*_tmp1A5=_cycalloc(sizeof(*_tmp1A5));_tmp1A5->v=_tmp193;_tmp1A5;});
return 1;}default: _LLF2: _tmp191=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp190.f1)->f1;_tmp192=_tmp190.f2;_LLF3:
# 550
 _tmp193=_tmp192;_tmp194=_tmp191;goto _LLF5;}_LLE9:;}
# 557
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp1A7=Cyc_Tcutil_compress(t);void*_tmp1A8=_tmp1A7;struct Cyc_Absyn_Tvar*_tmp1A9;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A8)->tag == 2){_LLF7: _tmp1A9=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A8)->f1;_LLF8: {
# 560
void*_tmp1AA=_tmp1A9->kind;
_tmp1A9->kind=kb;
return({struct _tuple16 _tmp1AB;_tmp1AB.f1=_tmp1A9;_tmp1AB.f2=_tmp1AA;_tmp1AB;});}}else{_LLF9: _LLFA:
# 564
({struct Cyc_String_pa_PrintArg_struct _tmp1AE;_tmp1AE.tag=0;_tmp1AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp1AC[1]={& _tmp1AE};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1AD="swap_kind: cannot update the kind of %s";_tag_dyneither(_tmp1AD,sizeof(char),40);}),_tag_dyneither(_tmp1AC,sizeof(void*),1));});});}_LLF6:;}
# 570
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_typ,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 572
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_typ_kind(field_typ);
if(ts != 0  && (k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak)){
# 576
struct _RegionHandle _tmp1AF=_new_region("temp");struct _RegionHandle*temp=& _tmp1AF;_push_region(temp);
{struct Cyc_List_List*_tmp1B0=0;
# 579
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp1B1=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp1B2=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;
struct Cyc_Absyn_Kind*_tmp1B3=Cyc_Tcutil_tvar_kind(_tmp1B1,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp1B4=_tmp1B3;switch(((struct Cyc_Absyn_Kind*)_tmp1B4)->kind){case Cyc_Absyn_IntKind: _LLFC: _LLFD:
 goto _LLFF;case Cyc_Absyn_AnyKind: _LLFE: _LLFF:
# 585
 _tmp1B0=({struct Cyc_List_List*_tmp1B5=_region_malloc(temp,sizeof(*_tmp1B5));_tmp1B5->hd=({struct _tuple16*_tmp1B6=_region_malloc(temp,sizeof(*_tmp1B6));_tmp1B6->f1=_tmp1B1;_tmp1B6->f2=_tmp1B2;_tmp1B6;});_tmp1B5->tl=_tmp1B0;_tmp1B5;});goto _LLFB;default: _LL100: _LL101:
 goto _LLFB;}_LLFB:;}
# 589
if(_tmp1B0 != 0){
field_typ=Cyc_Tcutil_rsubstitute(temp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1B0),field_typ);
k=Cyc_Tcutil_typ_kind(field_typ);}}
# 577
;_pop_region(temp);}
# 594
return k;}
# 601
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t){
void*_tmp1B7=Cyc_Tcutil_compress(t);void*_tmp1B8=_tmp1B7;struct Cyc_Absyn_Typedefdecl*_tmp1B9;struct Cyc_Absyn_Exp*_tmp1BA;struct Cyc_Absyn_Kind*_tmp1BB;struct Cyc_Absyn_PtrInfo _tmp1BC;enum Cyc_Absyn_AggrKind _tmp1BD;struct Cyc_List_List*_tmp1BE;struct Cyc_Absyn_AggrdeclImpl*_tmp1BF;int _tmp1C0;struct Cyc_List_List*_tmp1C1;struct Cyc_Absyn_Datatypedecl*_tmp1C2;struct Cyc_Absyn_Datatypefield*_tmp1C3;enum Cyc_Absyn_Size_of _tmp1C4;struct Cyc_Absyn_Tvar*_tmp1C5;struct Cyc_Core_Opt*_tmp1C6;switch(*((int*)_tmp1B8)){case 1: _LL103: _tmp1C6=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B8)->f1;_LL104:
 return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp1C6))->v;case 2: _LL105: _tmp1C5=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1B8)->f1;_LL106:
 return Cyc_Tcutil_tvar_kind(_tmp1C5,& Cyc_Tcutil_bk);case 0: _LL107: _LL108:
 return& Cyc_Tcutil_mk;case 6: _LL109: _tmp1C4=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1B8)->f2;_LL10A:
# 607
 return(_tmp1C4 == Cyc_Absyn_Int_sz  || _tmp1C4 == Cyc_Absyn_Long_sz)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;case 7: _LL10B: _LL10C:
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
 return& Cyc_Tcutil_ak;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1B8)->f1).field_info).KnownDatatypefield).tag == 2){_LL11F: _tmp1C2=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1B8)->f1).field_info).KnownDatatypefield).val).f1;_tmp1C3=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1B8)->f1).field_info).KnownDatatypefield).val).f2;_LL120:
# 622
 return& Cyc_Tcutil_mk;}else{_LL121: _LL122:
# 624
({void*_tmp1C7=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1C8="typ_kind: Unresolved DatatypeFieldType";_tag_dyneither(_tmp1C8,sizeof(char),39);}),_tag_dyneither(_tmp1C7,sizeof(void*),0));});}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B8)->f1).aggr_info).UnknownAggr).tag == 1){_LL123: _LL124:
# 627
 return& Cyc_Tcutil_ak;}else{_LL125: _tmp1BD=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B8)->f1).aggr_info).KnownAggr).val)->kind;_tmp1BE=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B8)->f1).aggr_info).KnownAggr).val)->tvs;_tmp1BF=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B8)->f1).aggr_info).KnownAggr).val)->impl;_tmp1C0=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B8)->f1).aggr_info).KnownAggr).val)->expected_mem_kind;_tmp1C1=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B8)->f1).targs;_LL126:
# 629
 if(_tmp1BF == 0){
if(_tmp1C0)
return& Cyc_Tcutil_mk;else{
# 633
return& Cyc_Tcutil_ak;}}{
# 635
struct Cyc_List_List*_tmp1C9=_tmp1BF->fields;
if(_tmp1C9 == 0)return& Cyc_Tcutil_mk;
# 638
if(_tmp1BD == Cyc_Absyn_StructA){
for(0;_tmp1C9->tl != 0;_tmp1C9=_tmp1C9->tl){;}{
void*_tmp1CA=((struct Cyc_Absyn_Aggrfield*)_tmp1C9->hd)->type;
struct Cyc_Absyn_Kind*_tmp1CB=Cyc_Tcutil_field_kind(_tmp1CA,_tmp1C1,_tmp1BE);
if(_tmp1CB == & Cyc_Tcutil_ak  || _tmp1CB == & Cyc_Tcutil_tak)return _tmp1CB;};}else{
# 646
for(0;_tmp1C9 != 0;_tmp1C9=_tmp1C9->tl){
void*_tmp1CC=((struct Cyc_Absyn_Aggrfield*)_tmp1C9->hd)->type;
struct Cyc_Absyn_Kind*_tmp1CD=Cyc_Tcutil_field_kind(_tmp1CC,_tmp1C1,_tmp1BE);
if(_tmp1CD == & Cyc_Tcutil_ak  || _tmp1CD == & Cyc_Tcutil_tak)return _tmp1CD;}}
# 652
return& Cyc_Tcutil_mk;};}case 12: _LL127: _LL128:
 return& Cyc_Tcutil_mk;case 5: _LL129: _tmp1BC=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B8)->f1;_LL12A: {
# 655
void*_tmp1CE=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,(_tmp1BC.ptr_atts).bounds);void*_tmp1CF=_tmp1CE;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1CF)->tag == 0){_LL146: _LL147: {
# 657
enum Cyc_Absyn_AliasQual _tmp1D0=(Cyc_Tcutil_typ_kind((_tmp1BC.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp1D1=_tmp1D0;switch(_tmp1D1){case Cyc_Absyn_Aliasable: _LL14B: _LL14C:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL14D: _LL14E:
 return& Cyc_Tcutil_umk;default: _LL14F: _LL150:
 return& Cyc_Tcutil_tmk;}_LL14A:;}}else{_LL148: _LL149: {
# 663
enum Cyc_Absyn_AliasQual _tmp1D2=(Cyc_Tcutil_typ_kind((_tmp1BC.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp1D3=_tmp1D2;switch(_tmp1D3){case Cyc_Absyn_Aliasable: _LL152: _LL153:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL154: _LL155:
 return& Cyc_Tcutil_ubk;default: _LL156: _LL157:
 return& Cyc_Tcutil_tbk;}_LL151:;}}_LL145:;}case 18: _LL12B: _LL12C:
# 669
 return& Cyc_Tcutil_ik;case 28: _LL12D: _tmp1BB=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp1B8)->f2;_LL12E:
 return _tmp1BB;case 27: _LL12F: _LL130:
# 674
 return& Cyc_Tcutil_ak;case 19: _LL131: _LL132:
 return& Cyc_Tcutil_bk;case 8: _LL133: _tmp1BA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1B8)->f1).num_elts;_LL134:
# 677
 if(_tmp1BA == 0  || Cyc_Tcutil_is_const_exp(_tmp1BA))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;case 10: _LL135: _LL136:
 return& Cyc_Tcutil_mk;case 17: _LL137: _tmp1B9=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1B8)->f3;_LL138:
# 681
 if(_tmp1B9 == 0  || _tmp1B9->kind == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1D6;_tmp1D6.tag=0;_tmp1D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp1D4[1]={& _tmp1D6};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1D5="typ_kind: typedef found: %s";_tag_dyneither(_tmp1D5,sizeof(char),28);}),_tag_dyneither(_tmp1D4,sizeof(void*),1));});});
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp1B9->kind))->v;case 26: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1B8)->f1)->r)){case 0: _LL139: _LL13A:
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
struct _handler_cons _tmp1D7;_push_handler(& _tmp1D7);{int _tmp1D9=0;if(setjmp(_tmp1D7.handler))_tmp1D9=1;if(!_tmp1D9){
Cyc_Tcutil_unify_it(t1,t2);{
int _tmp1DA=1;_npop_handler(0);return _tmp1DA;};
# 700
;_pop_handler();}else{void*_tmp1D8=(void*)_exn_thrown;void*_tmp1DB=_tmp1D8;void*_tmp1DC;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp1DB)->tag == Cyc_Tcutil_Unify){_LL159: _LL15A:
# 702
 return 0;}else{_LL15B: _tmp1DC=_tmp1DB;_LL15C:(int)_rethrow(_tmp1DC);}_LL158:;}};}
# 707
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
t=Cyc_Tcutil_compress(t);{
void*_tmp1DD=t;struct Cyc_List_List*_tmp1DE;void*_tmp1DF;void*_tmp1E0;void*_tmp1E1;void*_tmp1E2;struct Cyc_List_List*_tmp1E3;struct Cyc_List_List*_tmp1E4;struct Cyc_List_List*_tmp1E5;struct Cyc_List_List*_tmp1E6;struct Cyc_List_List*_tmp1E7;struct Cyc_List_List*_tmp1E8;struct Cyc_List_List*_tmp1E9;void*_tmp1EA;struct Cyc_Absyn_Tqual _tmp1EB;void*_tmp1EC;struct Cyc_List_List*_tmp1ED;int _tmp1EE;struct Cyc_Absyn_VarargInfo*_tmp1EF;struct Cyc_List_List*_tmp1F0;struct Cyc_List_List*_tmp1F1;struct Cyc_Absyn_Exp*_tmp1F2;struct Cyc_List_List*_tmp1F3;struct Cyc_Absyn_Exp*_tmp1F4;struct Cyc_List_List*_tmp1F5;void*_tmp1F6;struct Cyc_Absyn_PtrInfo _tmp1F7;void*_tmp1F8;struct Cyc_Core_Opt**_tmp1F9;struct Cyc_Absyn_Tvar*_tmp1FA;switch(*((int*)_tmp1DD)){case 2: _LL15E: _tmp1FA=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1DD)->f1;_LL15F:
# 712
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp1FA)){
Cyc_Tcutil_failure_reason=({const char*_tmp1FB="(type variable would escape scope)";_tag_dyneither(_tmp1FB,sizeof(char),35);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 716
goto _LL15D;case 1: _LL160: _tmp1F8=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1DD)->f2;_tmp1F9=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1DD)->f4;_LL161:
# 718
 if(t == evar){
Cyc_Tcutil_failure_reason=({const char*_tmp1FC="(occurs check)";_tag_dyneither(_tmp1FC,sizeof(char),15);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{
# 722
if(_tmp1F8 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp1F8);else{
# 725
int problem=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp1F9))->v;for(0;s != 0;s=s->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}
# 732
if(problem){
struct Cyc_List_List*_tmp1FD=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp1F9))->v;for(0;s != 0;s=s->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd))
_tmp1FD=({struct Cyc_List_List*_tmp1FE=_cycalloc(sizeof(*_tmp1FE));_tmp1FE->hd=(struct Cyc_Absyn_Tvar*)s->hd;_tmp1FE->tl=_tmp1FD;_tmp1FE;});}}
# 738
*_tmp1F9=({struct Cyc_Core_Opt*_tmp1FF=_cycalloc(sizeof(*_tmp1FF));_tmp1FF->v=_tmp1FD;_tmp1FF;});}}}
# 741
goto _LL15D;case 5: _LL162: _tmp1F7=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1DD)->f1;_LL163:
# 744
 Cyc_Tcutil_occurs(evar,r,env,_tmp1F7.elt_typ);
Cyc_Tcutil_occurs(evar,r,env,(_tmp1F7.ptr_atts).rgn);
goto _LL15D;case 8: _LL164: _tmp1F6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1DD)->f1).elt_type;_LL165:
# 748
 Cyc_Tcutil_occurs(evar,r,env,_tmp1F6);goto _LL15D;case 9: _LL166: _tmp1E9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DD)->f1).tvars;_tmp1EA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DD)->f1).effect;_tmp1EB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DD)->f1).ret_tqual;_tmp1EC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DD)->f1).ret_typ;_tmp1ED=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DD)->f1).args;_tmp1EE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DD)->f1).c_varargs;_tmp1EF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DD)->f1).cyc_varargs;_tmp1F0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DD)->f1).rgn_po;_tmp1F1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DD)->f1).attributes;_tmp1F2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DD)->f1).requires_clause;_tmp1F3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DD)->f1).requires_relns;_tmp1F4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DD)->f1).ensures_clause;_tmp1F5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DD)->f1).ensures_relns;_LL167:
# 751
 env=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp1E9,env);
if(_tmp1EA != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp1EA);
Cyc_Tcutil_occurs(evar,r,env,_tmp1EC);
for(0;_tmp1ED != 0;_tmp1ED=_tmp1ED->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple10*)_tmp1ED->hd)).f3);}
if(_tmp1EF != 0)
Cyc_Tcutil_occurs(evar,r,env,_tmp1EF->type);
for(0;_tmp1F0 != 0;_tmp1F0=_tmp1F0->tl){
struct _tuple0*_tmp200=(struct _tuple0*)_tmp1F0->hd;struct _tuple0*_tmp201=_tmp200;void*_tmp202;void*_tmp203;_LL181: _tmp202=_tmp201->f1;_tmp203=_tmp201->f2;_LL182:;
Cyc_Tcutil_occurs(evar,r,env,_tmp202);
Cyc_Tcutil_occurs(evar,r,env,_tmp203);}
# 763
goto _LL15D;case 10: _LL168: _tmp1E8=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1DD)->f1;_LL169:
# 765
 for(0;_tmp1E8 != 0;_tmp1E8=_tmp1E8->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple12*)_tmp1E8->hd)).f2);}
goto _LL15D;case 3: _LL16A: _tmp1E7=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1DD)->f1).targs;_LL16B:
# 769
 Cyc_Tcutil_occurslist(evar,r,env,_tmp1E7);goto _LL15D;case 17: _LL16C: _tmp1E6=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1DD)->f2;_LL16D:
 _tmp1E5=_tmp1E6;goto _LL16F;case 4: _LL16E: _tmp1E5=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1DD)->f1).targs;_LL16F:
 _tmp1E4=_tmp1E5;goto _LL171;case 11: _LL170: _tmp1E4=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1DD)->f1).targs;_LL171:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp1E4);goto _LL15D;case 12: _LL172: _tmp1E3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1DD)->f2;_LL173:
# 775
 for(0;_tmp1E3 != 0;_tmp1E3=_tmp1E3->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp1E3->hd)->type);}
goto _LL15D;case 19: _LL174: _tmp1E2=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp1DD)->f1;_LL175:
 _tmp1E1=_tmp1E2;goto _LL177;case 23: _LL176: _tmp1E1=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp1DD)->f1;_LL177:
 _tmp1E0=_tmp1E1;goto _LL179;case 15: _LL178: _tmp1E0=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1DD)->f1;_LL179:
 _tmp1DF=_tmp1E0;goto _LL17B;case 25: _LL17A: _tmp1DF=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp1DD)->f1;_LL17B:
 Cyc_Tcutil_occurs(evar,r,env,_tmp1DF);goto _LL15D;case 24: _LL17C: _tmp1DE=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp1DD)->f1;_LL17D:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp1DE);goto _LL15D;default: _LL17E: _LL17F:
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
({void*_tmp204=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp205="tq1 real_const not set.";_tag_dyneither(_tmp205,sizeof(char),24);}),_tag_dyneither(_tmp204,sizeof(void*),0));});
if(tq2.print_const  && !tq2.real_const)
({void*_tmp206=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp207="tq2 real_const not set.";_tag_dyneither(_tmp207,sizeof(char),24);}),_tag_dyneither(_tmp206,sizeof(void*),0));});
# 809
if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
# 812
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;
Cyc_Tcutil_failure_reason=({const char*_tmp208="(qualifiers don't match)";_tag_dyneither(_tmp208,sizeof(char),25);});
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
union Cyc_Absyn_Constraint*_tmp209=x;void*_tmp20A;switch((((union Cyc_Absyn_Constraint*)_tmp209)->Eq_constr).tag){case 3: _LL184: _LL185:
# 838
*x=({union Cyc_Absyn_Constraint _tmp20B;(_tmp20B.Forward_constr).val=y;(_tmp20B.Forward_constr).tag=2;_tmp20B;});return;case 1: _LL186: _tmp20A=(void*)(_tmp209->Eq_constr).val;_LL187: {
# 840
union Cyc_Absyn_Constraint*_tmp20C=y;void*_tmp20D;switch((((union Cyc_Absyn_Constraint*)_tmp20C)->Eq_constr).tag){case 3: _LL18B: _LL18C:
*y=*x;return;case 1: _LL18D: _tmp20D=(void*)(_tmp20C->Eq_constr).val;_LL18E:
# 843
 if(cmp(_tmp20A,_tmp20D)!= 0){
Cyc_Tcutil_failure_reason=reason;
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 847
return;default: _LL18F: _LL190:
({void*_tmp20E=0;Cyc_Tcutil_impos(({const char*_tmp20F="unify_conref: forward after compress(2)";_tag_dyneither(_tmp20F,sizeof(char),40);}),_tag_dyneither(_tmp20E,sizeof(void*),0));});}_LL18A:;}default: _LL188: _LL189:
# 850
({void*_tmp210=0;Cyc_Tcutil_impos(({const char*_tmp211="unify_conref: forward after compress";_tag_dyneither(_tmp211,sizeof(char),37);}),_tag_dyneither(_tmp210,sizeof(void*),0));});}_LL183:;};}
# 854
static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
struct _handler_cons _tmp212;_push_handler(& _tmp212);{int _tmp214=0;if(setjmp(_tmp212.handler))_tmp214=1;if(!_tmp214){
Cyc_Tcutil_unify_it_conrefs(cmp,x,y,_tag_dyneither(0,0,0));{
int _tmp215=1;_npop_handler(0);return _tmp215;};
# 856
;_pop_handler();}else{void*_tmp213=(void*)_exn_thrown;void*_tmp216=_tmp213;void*_tmp217;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp216)->tag == Cyc_Tcutil_Unify){_LL192: _LL193:
# 858
 return 0;}else{_LL194: _tmp217=_tmp216;_LL195:(int)_rethrow(_tmp217);}_LL191:;}};}
# 861
static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmp218=({struct _tuple0 _tmp21C;_tmp21C.f1=b1;_tmp21C.f2=b2;_tmp21C;});struct _tuple0 _tmp219=_tmp218;struct Cyc_Absyn_Exp*_tmp21A;struct Cyc_Absyn_Exp*_tmp21B;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp219.f1)->tag == 0){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp219.f2)->tag == 0){_LL197: _LL198:
 return 0;}else{_LL199: _LL19A:
 return - 1;}}else{if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp219.f2)->tag == 0){_LL19B: _LL19C:
 return 1;}else{_LL19D: _tmp21A=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp219.f1)->f1;_tmp21B=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp219.f2)->f1;_LL19E:
# 867
 return Cyc_Evexp_const_exp_cmp(_tmp21A,_tmp21B);}}_LL196:;}
# 871
static void Cyc_Tcutil_unify_cmp_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
{struct _tuple0 _tmp21D=({struct _tuple0 _tmp22B;_tmp22B.f1=e1->r;_tmp22B.f2=e2->r;_tmp22B;});struct _tuple0 _tmp21E=_tmp21D;void*_tmp21F;void*_tmp220;struct Cyc_Absyn_Exp*_tmp221;struct Cyc_Absyn_Exp*_tmp222;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp21E.f1)->tag == 13){_LL1A0: _tmp222=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp21E.f1)->f2;_LL1A1:
# 877
 Cyc_Tcutil_unify_cmp_exp(_tmp222,e2);
return;}else{if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp21E.f2)->tag == 13){_LL1A2: _tmp221=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp21E.f2)->f2;_LL1A3:
# 880
 Cyc_Tcutil_unify_cmp_exp(e1,_tmp221);
return;}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp21E.f1)->tag == 38){_LL1A4: _tmp220=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp21E.f1)->f1;_LL1A5:
# 883
{void*_tmp223=Cyc_Tcutil_compress(_tmp220);void*_tmp224=_tmp223;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp224)->tag == 1){_LL1AB: _LL1AC:
# 885
 Cyc_Tcutil_unify_it(_tmp220,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp225=_cycalloc(sizeof(*_tmp225));_tmp225[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp226;_tmp226.tag=18;_tmp226.f1=e2;_tmp226;});_tmp225;}));
return;}else{_LL1AD: _LL1AE:
# 888
 goto _LL1AA;}_LL1AA:;}
# 890
goto _LL19F;}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp21E.f2)->tag == 38){_LL1A6: _tmp21F=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp21E.f2)->f1;_LL1A7:
# 892
{void*_tmp227=Cyc_Tcutil_compress(_tmp21F);void*_tmp228=_tmp227;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp228)->tag == 1){_LL1B0: _LL1B1:
# 894
 Cyc_Tcutil_unify_it(_tmp21F,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp229=_cycalloc(sizeof(*_tmp229));_tmp229[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp22A;_tmp22A.tag=18;_tmp22A.f1=e1;_tmp22A;});_tmp229;}));
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
struct _tuple0 _tmp22C=({struct _tuple0 _tmp236;_tmp236.f1=b1;_tmp236.f2=b2;_tmp236;});struct _tuple0 _tmp22D=_tmp22C;struct Cyc_Absyn_Exp*_tmp22E;struct Cyc_Absyn_Exp*_tmp22F;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp22D.f1)->tag == 0){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp22D.f2)->tag == 0){_LL1B5: _LL1B6:
 return 0;}else{_LL1B7: _LL1B8:
 return - 1;}}else{if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp22D.f2)->tag == 0){_LL1B9: _LL1BA:
 return 1;}else{_LL1BB: _tmp22E=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp22D.f1)->f1;_tmp22F=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp22D.f2)->f1;_LL1BC: {
# 911
int r=Cyc_Evexp_const_exp_cmp(_tmp22E,_tmp22F);
if(r == 0)return 0;{
struct _handler_cons _tmp230;_push_handler(& _tmp230);{int _tmp232=0;if(setjmp(_tmp230.handler))_tmp232=1;if(!_tmp232){
Cyc_Tcutil_unify_cmp_exp(_tmp22E,_tmp22F);{
int _tmp233=0;_npop_handler(0);return _tmp233;};
# 914
;_pop_handler();}else{void*_tmp231=(void*)_exn_thrown;void*_tmp234=_tmp231;void*_tmp235;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp234)->tag == Cyc_Tcutil_Unify){_LL1BE: _LL1BF:
# 917
 return r;}else{_LL1C0: _tmp235=_tmp234;_LL1C1:(int)_rethrow(_tmp235);}_LL1BD:;}};};}}}_LL1B4:;}
# 922
static int Cyc_Tcutil_attribute_case_number(void*att){
void*_tmp237=att;switch(*((int*)_tmp237)){case 0: _LL1C3: _LL1C4:
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
struct _tuple0 _tmp238=({struct _tuple0 _tmp24A;_tmp24A.f1=att1;_tmp24A.f2=att2;_tmp24A;});struct _tuple0 _tmp239=_tmp238;enum Cyc_Absyn_Format_Type _tmp23A;int _tmp23B;int _tmp23C;enum Cyc_Absyn_Format_Type _tmp23D;int _tmp23E;int _tmp23F;struct _dyneither_ptr _tmp240;struct _dyneither_ptr _tmp241;struct Cyc_Absyn_Exp*_tmp242;struct Cyc_Absyn_Exp*_tmp243;int _tmp244;int _tmp245;int _tmp246;int _tmp247;switch(*((int*)_tmp239.f1)){case 0: if(((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp239.f2)->tag == 0){_LL1F0: _tmp246=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp239.f1)->f1;_tmp247=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp239.f2)->f1;_LL1F1:
 _tmp244=_tmp246;_tmp245=_tmp247;goto _LL1F3;}else{goto _LL1FA;}case 20: if(((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp239.f2)->tag == 20){_LL1F2: _tmp244=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp239.f1)->f1;_tmp245=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp239.f2)->f1;_LL1F3:
# 953
 return Cyc_Core_intcmp(_tmp244,_tmp245);}else{goto _LL1FA;}case 6: if(((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp239.f2)->tag == 6){_LL1F4: _tmp242=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp239.f1)->f1;_tmp243=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp239.f2)->f1;_LL1F5:
# 955
 if(_tmp242 == _tmp243)return 0;
if(_tmp242 == 0)return - 1;
if(_tmp243 == 0)return 1;
return Cyc_Evexp_const_exp_cmp(_tmp242,_tmp243);}else{goto _LL1FA;}case 8: if(((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp239.f2)->tag == 8){_LL1F6: _tmp240=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp239.f1)->f1;_tmp241=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp239.f2)->f1;_LL1F7:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp240,(struct _dyneither_ptr)_tmp241);}else{goto _LL1FA;}case 19: if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp239.f2)->tag == 19){_LL1F8: _tmp23A=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp239.f1)->f1;_tmp23B=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp239.f1)->f2;_tmp23C=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp239.f1)->f3;_tmp23D=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp239.f2)->f1;_tmp23E=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp239.f2)->f2;_tmp23F=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp239.f2)->f3;_LL1F9: {
# 961
int _tmp248=Cyc_Core_intcmp((int)((unsigned int)_tmp23A),(int)((unsigned int)_tmp23D));
if(_tmp248 != 0)return _tmp248;{
int _tmp249=Cyc_Core_intcmp(_tmp23B,_tmp23E);
if(_tmp249 != 0)return _tmp249;
return Cyc_Core_intcmp(_tmp23C,_tmp23F);};}}else{goto _LL1FA;}default: _LL1FA: _LL1FB:
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
{struct Cyc_Absyn_Kind*_tmp24B=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp24C=_tmp24B;switch(((struct Cyc_Absyn_Kind*)_tmp24C)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp24C)->aliasqual){case Cyc_Absyn_Unique: _LL1FD: _LL1FE:
 t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL1FC;case Cyc_Absyn_Aliasable: _LL1FF: _LL200:
 t=(void*)& Cyc_Absyn_HeapRgn_val;goto _LL1FC;default: goto _LL205;}case Cyc_Absyn_EffKind: _LL201: _LL202:
 t=Cyc_Absyn_empty_effect;goto _LL1FC;case Cyc_Absyn_IntKind: _LL203: _LL204:
 t=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp24D=_cycalloc(sizeof(*_tmp24D));_tmp24D[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp24E;_tmp24E.tag=18;_tmp24E.f1=Cyc_Absyn_uint_exp(0,0);_tmp24E;});_tmp24D;});goto _LL1FC;default: _LL205: _LL206:
 t=Cyc_Absyn_sint_typ;goto _LL1FC;}_LL1FC:;}
# 999
return({struct _tuple16*_tmp24F=_cycalloc(sizeof(*_tmp24F));_tmp24F->f1=tv;_tmp24F->f2=t;_tmp24F;});}
# 1006
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp250=Cyc_Tcutil_compress(t);void*_tmp251=_tmp250;struct Cyc_List_List*_tmp252;void*_tmp253;struct Cyc_List_List*_tmp254;void*_tmp255;struct Cyc_Absyn_Tqual _tmp256;void*_tmp257;struct Cyc_List_List*_tmp258;struct Cyc_Absyn_VarargInfo*_tmp259;struct Cyc_List_List*_tmp25A;struct Cyc_List_List*_tmp25B;struct Cyc_List_List*_tmp25C;struct Cyc_List_List*_tmp25D;struct Cyc_List_List*_tmp25E;void*_tmp25F;void*_tmp260;void*_tmp261;struct Cyc_List_List*_tmp262;void*_tmp263;void*_tmp264;void*_tmp265;switch(*((int*)_tmp251)){case 0: _LL208: _LL209:
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
struct Cyc_Absyn_Kind*_tmp266=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp267=_tmp266;switch(((struct Cyc_Absyn_Kind*)_tmp267)->kind){case Cyc_Absyn_RgnKind: _LL243: _LL244:
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp268=_cycalloc(sizeof(*_tmp268));_tmp268[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp269;_tmp269.tag=23;_tmp269.f1=t;_tmp269;});_tmp268;});case Cyc_Absyn_EffKind: _LL245: _LL246:
 return t;case Cyc_Absyn_IntKind: _LL247: _LL248:
 return Cyc_Absyn_empty_effect;default: _LL249: _LL24A:
 return(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp26A=_cycalloc(sizeof(*_tmp26A));_tmp26A[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp26B;_tmp26B.tag=25;_tmp26B.f1=t;_tmp26B;});_tmp26A;});}_LL242:;}case 15: _LL21C: _tmp265=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp251)->f1;_LL21D:
# 1024
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp26C=_cycalloc(sizeof(*_tmp26C));_tmp26C[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp26D;_tmp26D.tag=23;_tmp26D.f1=_tmp265;_tmp26D;});_tmp26C;});case 16: _LL21E: _tmp263=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp251)->f1;_tmp264=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp251)->f2;_LL21F:
# 1028
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp26E=_cycalloc(sizeof(*_tmp26E));_tmp26E[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp26F;_tmp26F.tag=23;_tmp26F.f1=_tmp264;_tmp26F;});_tmp26E;});case 3: _LL220: _tmp262=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp251)->f1).targs;_LL221: {
# 1030
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp262);
return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp270=_cycalloc(sizeof(*_tmp270));_tmp270[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp271;_tmp271.tag=24;_tmp271.f1=ts;_tmp271;});_tmp270;}));}case 5: _LL222: _tmp260=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp251)->f1).elt_typ;_tmp261=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp251)->f1).ptr_atts).rgn;_LL223:
# 1034
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp272=_cycalloc(sizeof(*_tmp272));_tmp272[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp273;_tmp273.tag=24;_tmp273.f1=({void*_tmp274[2];_tmp274[1]=Cyc_Tcutil_rgns_of(_tmp260);_tmp274[0]=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp275=_cycalloc(sizeof(*_tmp275));_tmp275[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp276;_tmp276.tag=23;_tmp276.f1=_tmp261;_tmp276;});_tmp275;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp274,sizeof(void*),2));});_tmp273;});_tmp272;}));case 8: _LL224: _tmp25F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp251)->f1).elt_type;_LL225:
# 1036
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp25F));case 10: _LL226: _tmp25E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp251)->f1;_LL227: {
# 1038
struct Cyc_List_List*_tmp277=0;
for(0;_tmp25E != 0;_tmp25E=_tmp25E->tl){
_tmp277=({struct Cyc_List_List*_tmp278=_cycalloc(sizeof(*_tmp278));_tmp278->hd=(*((struct _tuple12*)_tmp25E->hd)).f2;_tmp278->tl=_tmp277;_tmp278;});}
_tmp25D=_tmp277;goto _LL229;}case 4: _LL228: _tmp25D=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp251)->f1).targs;_LL229:
 _tmp25C=_tmp25D;goto _LL22B;case 11: _LL22A: _tmp25C=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp251)->f1).targs;_LL22B:
# 1044
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp279=_cycalloc(sizeof(*_tmp279));_tmp279[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp27A;_tmp27A.tag=24;_tmp27A.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp25C);_tmp27A;});_tmp279;}));case 12: _LL22C: _tmp25B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp251)->f2;_LL22D:
# 1046
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp27B=_cycalloc(sizeof(*_tmp27B));_tmp27B[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp27C;_tmp27C.tag=24;_tmp27C.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp25B);_tmp27C;});_tmp27B;}));case 19: _LL22E: _LL22F:
 return Cyc_Absyn_empty_effect;case 9: _LL230: _tmp254=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp251)->f1).tvars;_tmp255=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp251)->f1).effect;_tmp256=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp251)->f1).ret_tqual;_tmp257=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp251)->f1).ret_typ;_tmp258=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp251)->f1).args;_tmp259=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp251)->f1).cyc_varargs;_tmp25A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp251)->f1).rgn_po;_LL231: {
# 1056
void*_tmp27D=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp254),(void*)_check_null(_tmp255));
return Cyc_Tcutil_normalize_effect(_tmp27D);}case 21: _LL232: _LL233:
 goto _LL235;case 22: _LL234: _LL235:
 goto _LL237;case 20: _LL236: _LL237:
 return Cyc_Absyn_empty_effect;case 23: _LL238: _LL239:
 goto _LL23B;case 24: _LL23A: _LL23B:
 return t;case 25: _LL23C: _tmp253=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp251)->f1;_LL23D:
 return Cyc_Tcutil_rgns_of(_tmp253);case 17: _LL23E: _tmp252=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp251)->f2;_LL23F:
# 1065
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp27E=_cycalloc(sizeof(*_tmp27E));_tmp27E[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp27F;_tmp27F.tag=24;_tmp27F.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp252);_tmp27F;});_tmp27E;}));default: _LL240: _LL241:
({void*_tmp280=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp281="typedecl in rgns_of";_tag_dyneither(_tmp281,sizeof(char),20);}),_tag_dyneither(_tmp280,sizeof(void*),0));});}_LL207:;}
# 1073
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp282=e;void*_tmp283;struct Cyc_List_List**_tmp284;switch(*((int*)_tmp282)){case 24: _LL24C: _tmp284=(struct Cyc_List_List**)&((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp282)->f1;_LL24D: {
# 1077
int redo_join=0;
{struct Cyc_List_List*effs=*_tmp284;for(0;effs != 0;effs=effs->tl){
void*_tmp285=(void*)effs->hd;
effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp285));{
void*_tmp286=(void*)effs->hd;void*_tmp287=_tmp286;switch(*((int*)_tmp287)){case 24: _LL253: _LL254:
 goto _LL256;case 23: switch(*((int*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp287)->f1)){case 20: _LL255: _LL256:
 redo_join=1;goto _LL252;case 22: _LL257: _LL258:
 redo_join=1;goto _LL252;case 21: _LL259: _LL25A:
 redo_join=1;goto _LL252;default: goto _LL25B;}default: _LL25B: _LL25C:
 goto _LL252;}_LL252:;};}}
# 1089
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*_tmp284;for(0;effs != 0;effs=effs->tl){
void*_tmp288=Cyc_Tcutil_compress((void*)effs->hd);void*_tmp289=_tmp288;void*_tmp28A;struct Cyc_List_List*_tmp28B;switch(*((int*)_tmp289)){case 24: _LL25E: _tmp28B=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp289)->f1;_LL25F:
# 1094
 effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp28B,effects);
goto _LL25D;case 23: switch(*((int*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp289)->f1)){case 20: _LL260: _LL261:
 goto _LL25D;case 22: _LL262: _LL263:
 goto _LL25D;case 21: _LL264: _LL265:
 goto _LL25D;default: goto _LL266;}default: _LL266: _tmp28A=_tmp289;_LL267:
 effects=({struct Cyc_List_List*_tmp28C=_cycalloc(sizeof(*_tmp28C));_tmp28C->hd=_tmp28A;_tmp28C->tl=effects;_tmp28C;});goto _LL25D;}_LL25D:;}}
# 1102
*_tmp284=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
return e;};}case 25: _LL24E: _tmp283=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp282)->f1;_LL24F: {
# 1105
void*_tmp28D=Cyc_Tcutil_compress(_tmp283);void*_tmp28E=_tmp28D;switch(*((int*)_tmp28E)){case 1: _LL269: _LL26A:
 goto _LL26C;case 2: _LL26B: _LL26C:
 return e;default: _LL26D: _LL26E:
 return Cyc_Tcutil_rgns_of(_tmp283);}_LL268:;}default: _LL250: _LL251:
# 1110
 return e;}_LL24B:;};}
# 1115
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp28F=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp290=_cycalloc(sizeof(*_tmp290));_tmp290[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp291;_tmp291.tag=9;_tmp291.f1=({struct Cyc_Absyn_FnInfo _tmp292;_tmp292.tvars=0;_tmp292.effect=eff;_tmp292.ret_tqual=
Cyc_Absyn_empty_tqual(0);_tmp292.ret_typ=(void*)& Cyc_Absyn_VoidType_val;_tmp292.args=0;_tmp292.c_varargs=0;_tmp292.cyc_varargs=0;_tmp292.rgn_po=0;_tmp292.attributes=0;_tmp292.requires_clause=0;_tmp292.requires_relns=0;_tmp292.ensures_clause=0;_tmp292.ensures_relns=0;_tmp292;});_tmp291;});_tmp290;});
# 1125
return Cyc_Absyn_atb_typ((void*)_tmp28F,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}
# 1132
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);
if((r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (void*)& Cyc_Absyn_UniqueRgn_val) || r == (void*)& Cyc_Absyn_RefCntRgn_val)
return 1;{
void*_tmp293=Cyc_Tcutil_compress(e);void*_tmp294=_tmp293;struct Cyc_Core_Opt*_tmp295;void**_tmp296;struct Cyc_Core_Opt*_tmp297;void*_tmp298;struct Cyc_List_List*_tmp299;void*_tmp29A;switch(*((int*)_tmp294)){case 23: _LL270: _tmp29A=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp294)->f1;_LL271:
# 1139
 if(constrain)return Cyc_Tcutil_unify(r,_tmp29A);
_tmp29A=Cyc_Tcutil_compress(_tmp29A);
if(r == _tmp29A)return 1;{
struct _tuple0 _tmp29B=({struct _tuple0 _tmp29F;_tmp29F.f1=r;_tmp29F.f2=_tmp29A;_tmp29F;});struct _tuple0 _tmp29C=_tmp29B;struct Cyc_Absyn_Tvar*_tmp29D;struct Cyc_Absyn_Tvar*_tmp29E;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp29C.f1)->tag == 2){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp29C.f2)->tag == 2){_LL27B: _tmp29D=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp29C.f1)->f1;_tmp29E=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp29C.f2)->f1;_LL27C:
 return Cyc_Absyn_tvar_cmp(_tmp29D,_tmp29E)== 0;}else{goto _LL27D;}}else{_LL27D: _LL27E:
 return 0;}_LL27A:;};case 24: _LL272: _tmp299=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp294)->f1;_LL273:
# 1147
 for(0;_tmp299 != 0;_tmp299=_tmp299->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp299->hd))return 1;}
return 0;case 25: _LL274: _tmp298=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp294)->f1;_LL275: {
# 1151
void*_tmp2A0=Cyc_Tcutil_rgns_of(_tmp298);void*_tmp2A1=_tmp2A0;void*_tmp2A2;void*_tmp2A3;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2A1)->tag == 25){_LL280: _tmp2A3=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2A1)->f1;_LL281:
# 1153
 if(!constrain)return 0;{
void*_tmp2A4=Cyc_Tcutil_compress(_tmp2A3);void*_tmp2A5=_tmp2A4;struct Cyc_Core_Opt*_tmp2A6;void**_tmp2A7;struct Cyc_Core_Opt*_tmp2A8;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A5)->tag == 1){_LL285: _tmp2A6=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A5)->f1;_tmp2A7=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A5)->f2;_tmp2A8=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A5)->f4;_LL286: {
# 1158
void*_tmp2A9=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2A8);
# 1161
Cyc_Tcutil_occurs(_tmp2A9,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2A8))->v,r);{
void*_tmp2AA=Cyc_Tcutil_dummy_fntype((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2AB=_cycalloc(sizeof(*_tmp2AB));_tmp2AB[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2AC;_tmp2AC.tag=24;_tmp2AC.f1=({void*_tmp2AD[2];_tmp2AD[1]=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp2AE=_cycalloc(sizeof(*_tmp2AE));_tmp2AE[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp2AF;_tmp2AF.tag=23;_tmp2AF.f1=r;_tmp2AF;});_tmp2AE;});_tmp2AD[0]=_tmp2A9;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2AD,sizeof(void*),2));});_tmp2AC;});_tmp2AB;}));
*_tmp2A7=_tmp2AA;
return 1;};}}else{_LL287: _LL288:
 return 0;}_LL284:;};}else{_LL282: _tmp2A2=_tmp2A1;_LL283:
# 1167
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp2A2);}_LL27F:;}case 1: _LL276: _tmp295=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp294)->f1;_tmp296=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp294)->f2;_tmp297=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp294)->f4;_LL277:
# 1170
 if(_tmp295 == 0  || ((struct Cyc_Absyn_Kind*)_tmp295->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp2B0=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2B1="effect evar has wrong kind";_tag_dyneither(_tmp2B1,sizeof(char),27);}),_tag_dyneither(_tmp2B0,sizeof(void*),0));});
if(!constrain)return 0;{
# 1175
void*_tmp2B2=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp297);
# 1178
Cyc_Tcutil_occurs(_tmp2B2,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp297))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2B3=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2B4=_cycalloc(sizeof(*_tmp2B4));_tmp2B4[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2B5;_tmp2B5.tag=24;_tmp2B5.f1=({struct Cyc_List_List*_tmp2B6=_cycalloc(sizeof(*_tmp2B6));_tmp2B6->hd=_tmp2B2;_tmp2B6->tl=({struct Cyc_List_List*_tmp2B7=_cycalloc(sizeof(*_tmp2B7));_tmp2B7->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp2B8=_cycalloc(sizeof(*_tmp2B8));_tmp2B8[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp2B9;_tmp2B9.tag=23;_tmp2B9.f1=r;_tmp2B9;});_tmp2B8;});_tmp2B7->tl=0;_tmp2B7;});_tmp2B6;});_tmp2B5;});_tmp2B4;});
*_tmp296=(void*)_tmp2B3;
return 1;};};default: _LL278: _LL279:
 return 0;}_LL26F:;};}
# 1189
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Tcutil_compress(t);{
void*_tmp2BA=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_tmp2BB=_tmp2BA;struct Cyc_Core_Opt*_tmp2BC;void**_tmp2BD;struct Cyc_Core_Opt*_tmp2BE;void*_tmp2BF;struct Cyc_List_List*_tmp2C0;switch(*((int*)_tmp2BB)){case 23: _LL28A: _LL28B:
 return 0;case 24: _LL28C: _tmp2C0=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2BB)->f1;_LL28D:
# 1194
 for(0;_tmp2C0 != 0;_tmp2C0=_tmp2C0->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp2C0->hd))
return 1;}
return 0;case 25: _LL28E: _tmp2BF=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2BB)->f1;_LL28F:
# 1199
 _tmp2BF=Cyc_Tcutil_compress(_tmp2BF);
if(t == _tmp2BF)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp2BF);{
void*_tmp2C1=Cyc_Tcutil_rgns_of(t);void*_tmp2C2=_tmp2C1;void*_tmp2C3;void*_tmp2C4;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2C2)->tag == 25){_LL295: _tmp2C4=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2C2)->f1;_LL296: {
# 1204
struct _tuple0 _tmp2C5=({struct _tuple0 _tmp2C9;_tmp2C9.f1=Cyc_Tcutil_compress(_tmp2C4);_tmp2C9.f2=_tmp2BF;_tmp2C9;});struct _tuple0 _tmp2C6=_tmp2C5;struct Cyc_Absyn_Tvar*_tmp2C7;struct Cyc_Absyn_Tvar*_tmp2C8;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2C6.f1)->tag == 2){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2C6.f2)->tag == 2){_LL29A: _tmp2C7=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2C6.f1)->f1;_tmp2C8=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2C6.f2)->f1;_LL29B:
 return Cyc_Tcutil_unify(t,_tmp2BF);}else{goto _LL29C;}}else{_LL29C: _LL29D:
 return _tmp2C4 == _tmp2BF;}_LL299:;}}else{_LL297: _tmp2C3=_tmp2C2;_LL298:
# 1208
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp2C3);}_LL294:;};case 1: _LL290: _tmp2BC=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2BB)->f1;_tmp2BD=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2BB)->f2;_tmp2BE=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2BB)->f4;_LL291:
# 1211
 if(_tmp2BC == 0  || ((struct Cyc_Absyn_Kind*)_tmp2BC->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp2CA=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2CB="effect evar has wrong kind";_tag_dyneither(_tmp2CB,sizeof(char),27);}),_tag_dyneither(_tmp2CA,sizeof(void*),0));});
if(!may_constrain_evars)return 0;{
# 1216
void*_tmp2CC=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2BE);
# 1219
Cyc_Tcutil_occurs(_tmp2CC,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2BE))->v,t);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2CD=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2CE=_cycalloc(sizeof(*_tmp2CE));_tmp2CE[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2CF;_tmp2CF.tag=24;_tmp2CF.f1=({struct Cyc_List_List*_tmp2D0=_cycalloc(sizeof(*_tmp2D0));_tmp2D0->hd=_tmp2CC;_tmp2D0->tl=({struct Cyc_List_List*_tmp2D1=_cycalloc(sizeof(*_tmp2D1));_tmp2D1->hd=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp2D2=_cycalloc(sizeof(*_tmp2D2));_tmp2D2[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp2D3;_tmp2D3.tag=25;_tmp2D3.f1=t;_tmp2D3;});_tmp2D2;});_tmp2D1->tl=0;_tmp2D1;});_tmp2D0;});_tmp2CF;});_tmp2CE;});
*_tmp2BD=(void*)_tmp2CD;
return 1;};};default: _LL292: _LL293:
 return 0;}_LL289:;};}
# 1230
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp2D4=e;struct Cyc_Core_Opt*_tmp2D5;void**_tmp2D6;struct Cyc_Core_Opt*_tmp2D7;void*_tmp2D8;struct Cyc_List_List*_tmp2D9;struct Cyc_Absyn_Tvar*_tmp2DA;switch(*((int*)_tmp2D4)){case 2: _LL29F: _tmp2DA=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2D4)->f1;_LL2A0:
 return Cyc_Absyn_tvar_cmp(v,_tmp2DA)== 0;case 24: _LL2A1: _tmp2D9=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2D4)->f1;_LL2A2:
# 1235
 for(0;_tmp2D9 != 0;_tmp2D9=_tmp2D9->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)_tmp2D9->hd))
return 1;}
return 0;case 25: _LL2A3: _tmp2D8=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2D4)->f1;_LL2A4: {
# 1240
void*_tmp2DB=Cyc_Tcutil_rgns_of(_tmp2D8);void*_tmp2DC=_tmp2DB;void*_tmp2DD;void*_tmp2DE;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2DC)->tag == 25){_LL2AA: _tmp2DE=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2DC)->f1;_LL2AB:
# 1242
 if(!may_constrain_evars)return 0;{
void*_tmp2DF=Cyc_Tcutil_compress(_tmp2DE);void*_tmp2E0=_tmp2DF;struct Cyc_Core_Opt*_tmp2E1;void**_tmp2E2;struct Cyc_Core_Opt*_tmp2E3;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2E0)->tag == 1){_LL2AF: _tmp2E1=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2E0)->f1;_tmp2E2=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2E0)->f2;_tmp2E3=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2E0)->f4;_LL2B0: {
# 1248
void*_tmp2E4=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2E3);
# 1250
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2E3))->v,v))return 0;{
void*_tmp2E5=Cyc_Tcutil_dummy_fntype((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2E6=_cycalloc(sizeof(*_tmp2E6));_tmp2E6[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2E7;_tmp2E7.tag=24;_tmp2E7.f1=({void*_tmp2E8[2];_tmp2E8[1]=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2E9=_cycalloc(sizeof(*_tmp2E9));_tmp2E9[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp2EA;_tmp2EA.tag=2;_tmp2EA.f1=v;_tmp2EA;});_tmp2E9;});_tmp2E8[0]=_tmp2E4;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2E8,sizeof(void*),2));});_tmp2E7;});_tmp2E6;}));
*_tmp2E2=_tmp2E5;
return 1;};}}else{_LL2B1: _LL2B2:
 return 0;}_LL2AE:;};}else{_LL2AC: _tmp2DD=_tmp2DC;_LL2AD:
# 1256
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp2DD);}_LL2A9:;}case 1: _LL2A5: _tmp2D5=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2D4)->f1;_tmp2D6=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2D4)->f2;_tmp2D7=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2D4)->f4;_LL2A6:
# 1259
 if(_tmp2D5 == 0  || ((struct Cyc_Absyn_Kind*)_tmp2D5->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp2EB=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2EC="effect evar has wrong kind";_tag_dyneither(_tmp2EC,sizeof(char),27);}),_tag_dyneither(_tmp2EB,sizeof(void*),0));});{
# 1263
void*_tmp2ED=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2D7);
# 1265
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2D7))->v,v))
return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2EE=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2EF=_cycalloc(sizeof(*_tmp2EF));_tmp2EF[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2F0;_tmp2F0.tag=24;_tmp2F0.f1=({struct Cyc_List_List*_tmp2F1=_cycalloc(sizeof(*_tmp2F1));_tmp2F1->hd=_tmp2ED;_tmp2F1->tl=({struct Cyc_List_List*_tmp2F2=_cycalloc(sizeof(*_tmp2F2));_tmp2F2->hd=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2F3=_cycalloc(sizeof(*_tmp2F3));_tmp2F3[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp2F4;_tmp2F4.tag=2;_tmp2F4.f1=v;_tmp2F4;});_tmp2F3;});_tmp2F2->tl=0;_tmp2F2;});_tmp2F1;});_tmp2F0;});_tmp2EF;});
*_tmp2D6=(void*)_tmp2EE;
return 1;};};default: _LL2A7: _LL2A8:
 return 0;}_LL29E:;};}
# 1275
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp2F5=e;void*_tmp2F6;struct Cyc_List_List*_tmp2F7;switch(*((int*)_tmp2F5)){case 24: _LL2B4: _tmp2F7=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2F5)->f1;_LL2B5:
# 1279
 for(0;_tmp2F7 != 0;_tmp2F7=_tmp2F7->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp2F7->hd))
return 1;}
return 0;case 25: _LL2B6: _tmp2F6=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2F5)->f1;_LL2B7: {
# 1284
void*_tmp2F8=Cyc_Tcutil_rgns_of(_tmp2F6);void*_tmp2F9=_tmp2F8;void*_tmp2FA;void*_tmp2FB;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2F9)->tag == 25){_LL2BD: _tmp2FB=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2F9)->f1;_LL2BE:
 return 0;}else{_LL2BF: _tmp2FA=_tmp2F9;_LL2C0:
 return Cyc_Tcutil_evar_in_effect(evar,_tmp2FA);}_LL2BC:;}case 1: _LL2B8: _LL2B9:
# 1288
 return evar == e;default: _LL2BA: _LL2BB:
 return 0;}_LL2B3:;};}
# 1302 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1307
void*_tmp2FC=Cyc_Tcutil_compress(e1);void*_tmp2FD=_tmp2FC;void**_tmp2FE;struct Cyc_Core_Opt*_tmp2FF;void*_tmp300;struct Cyc_Absyn_Tvar*_tmp301;void*_tmp302;struct Cyc_List_List*_tmp303;switch(*((int*)_tmp2FD)){case 24: _LL2C2: _tmp303=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2FD)->f1;_LL2C3:
# 1309
 for(0;_tmp303 != 0;_tmp303=_tmp303->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp303->hd,e2))
return 0;}
return 1;case 23: _LL2C4: _tmp302=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp2FD)->f1;_LL2C5:
# 1320
 return Cyc_Tcutil_region_in_effect(may_constrain_evars,_tmp302,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp302,(void*)& Cyc_Absyn_HeapRgn_val);case 2: _LL2C6: _tmp301=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2FD)->f1;_LL2C7:
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp301,e2);case 25: _LL2C8: _tmp300=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2FD)->f1;_LL2C9: {
# 1324
void*_tmp304=Cyc_Tcutil_rgns_of(_tmp300);void*_tmp305=_tmp304;void*_tmp306;void*_tmp307;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp305)->tag == 25){_LL2CF: _tmp307=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp305)->f1;_LL2D0:
# 1329
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp307,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp307,Cyc_Absyn_sint_typ);}else{_LL2D1: _tmp306=_tmp305;_LL2D2:
 return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp306,e2);}_LL2CE:;}case 1: _LL2CA: _tmp2FE=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2FD)->f2;_tmp2FF=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2FD)->f4;_LL2CB:
# 1334
 if(!Cyc_Tcutil_evar_in_effect(e1,e2)){
# 1338
*_tmp2FE=Cyc_Absyn_empty_effect;
# 1341
return 1;}else{
# 1343
return 0;}default: _LL2CC: _LL2CD:
({struct Cyc_String_pa_PrintArg_struct _tmp30A;_tmp30A.tag=0;_tmp30A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1));({void*_tmp308[1]={& _tmp30A};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp309="subset_effect: bad effect: %s";_tag_dyneither(_tmp309,sizeof(char),30);}),_tag_dyneither(_tmp308,sizeof(void*),1));});});}_LL2C1:;}
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
struct _tuple0*_tmp30B=(struct _tuple0*)r1->hd;struct _tuple0*_tmp30C=_tmp30B;void*_tmp30D;void*_tmp30E;_LL2D4: _tmp30D=_tmp30C->f1;_tmp30E=_tmp30C->f2;_LL2D5:;{
int found=_tmp30D == (void*)& Cyc_Absyn_HeapRgn_val;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
struct _tuple0*_tmp30F=(struct _tuple0*)r2->hd;struct _tuple0*_tmp310=_tmp30F;void*_tmp311;void*_tmp312;_LL2D7: _tmp311=_tmp310->f1;_tmp312=_tmp310->f2;_LL2D8:;
if(Cyc_Tcutil_unify(_tmp30D,_tmp311) && Cyc_Tcutil_unify(_tmp30E,_tmp312)){
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
struct Cyc_Relations_Reln*_tmp313=Cyc_Relations_negate(Cyc_Core_heap_region,(struct Cyc_Relations_Reln*)r2->hd);
struct Cyc_List_List*_tmp314=({struct Cyc_List_List*_tmp315=_cycalloc(sizeof(*_tmp315));_tmp315->hd=_tmp313;_tmp315->tl=r1;_tmp315;});
if(Cyc_Relations_consistent_relations(_tmp314))return 0;}
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
{void*_tmp316=t1;struct Cyc_Core_Opt*_tmp317;void**_tmp318;struct Cyc_Core_Opt*_tmp319;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp316)->tag == 1){_LL2DA: _tmp317=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp316)->f1;_tmp318=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp316)->f2;_tmp319=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp316)->f4;_LL2DB:
# 1430
 Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp319))->v,t2);{
struct Cyc_Absyn_Kind*_tmp31A=Cyc_Tcutil_typ_kind(t2);
# 1435
if(Cyc_Tcutil_kind_leq(_tmp31A,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp317))->v)){
*_tmp318=t2;
return;}else{
# 1439
{void*_tmp31B=t2;struct Cyc_Absyn_PtrInfo _tmp31C;void**_tmp31D;struct Cyc_Core_Opt*_tmp31E;switch(*((int*)_tmp31B)){case 1: _LL2DF: _tmp31D=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp31B)->f2;_tmp31E=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp31B)->f4;_LL2E0: {
# 1442
struct Cyc_List_List*_tmp31F=(struct Cyc_List_List*)_tmp319->v;
{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp31E))->v;for(0;s2 != 0;s2=s2->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp31F,(struct Cyc_Absyn_Tvar*)s2->hd)){
Cyc_Tcutil_failure_reason=({const char*_tmp320="(type variable would escape scope)";_tag_dyneither(_tmp320,sizeof(char),35);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1449
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp317->v,_tmp31A)){
*_tmp31D=t1;return;}
# 1452
Cyc_Tcutil_failure_reason=({const char*_tmp321="(kinds are incompatible)";_tag_dyneither(_tmp321,sizeof(char),25);});
goto _LL2DE;}case 5: _LL2E1: _tmp31C=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp31B)->f1;if(((struct Cyc_Absyn_Kind*)_tmp317->v)->kind == Cyc_Absyn_BoxKind){_LL2E2: {
# 1458
union Cyc_Absyn_Constraint*_tmp322=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((_tmp31C.ptr_atts).bounds);
{union Cyc_Absyn_Constraint*_tmp323=_tmp322;if((((union Cyc_Absyn_Constraint*)_tmp323)->No_constr).tag == 3){_LL2E6: _LL2E7:
# 1462
({struct _union_Constraint_Eq_constr*_tmp324=& _tmp322->Eq_constr;_tmp324->tag=1;_tmp324->val=Cyc_Absyn_bounds_one;});
*_tmp318=t2;
return;}else{_LL2E8: _LL2E9:
 goto _LL2E5;}_LL2E5:;}
# 1467
goto _LL2DE;}}else{goto _LL2E3;}default: _LL2E3: _LL2E4:
 goto _LL2DE;}_LL2DE:;}
# 1470
Cyc_Tcutil_failure_reason=({const char*_tmp325="(kinds are incompatible)";_tag_dyneither(_tmp325,sizeof(char),25);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};}else{_LL2DC: _LL2DD:
# 1473
 goto _LL2D9;}_LL2D9:;}
# 1478
{struct _tuple0 _tmp326=({struct _tuple0 _tmp3EB;_tmp3EB.f1=t2;_tmp3EB.f2=t1;_tmp3EB;});struct _tuple0 _tmp327=_tmp326;void*_tmp328;void*_tmp329;void*_tmp32A;void*_tmp32B;void*_tmp32C;void*_tmp32D;struct Cyc_List_List*_tmp32E;struct Cyc_Absyn_Typedefdecl*_tmp32F;struct Cyc_List_List*_tmp330;struct Cyc_Absyn_Typedefdecl*_tmp331;enum Cyc_Absyn_AggrKind _tmp332;struct Cyc_List_List*_tmp333;enum Cyc_Absyn_AggrKind _tmp334;struct Cyc_List_List*_tmp335;struct Cyc_List_List*_tmp336;struct Cyc_List_List*_tmp337;struct Cyc_List_List*_tmp338;void*_tmp339;struct Cyc_Absyn_Tqual _tmp33A;void*_tmp33B;struct Cyc_List_List*_tmp33C;int _tmp33D;struct Cyc_Absyn_VarargInfo*_tmp33E;struct Cyc_List_List*_tmp33F;struct Cyc_List_List*_tmp340;struct Cyc_Absyn_Exp*_tmp341;struct Cyc_List_List*_tmp342;struct Cyc_Absyn_Exp*_tmp343;struct Cyc_List_List*_tmp344;struct Cyc_List_List*_tmp345;void*_tmp346;struct Cyc_Absyn_Tqual _tmp347;void*_tmp348;struct Cyc_List_List*_tmp349;int _tmp34A;struct Cyc_Absyn_VarargInfo*_tmp34B;struct Cyc_List_List*_tmp34C;struct Cyc_List_List*_tmp34D;struct Cyc_Absyn_Exp*_tmp34E;struct Cyc_List_List*_tmp34F;struct Cyc_Absyn_Exp*_tmp350;struct Cyc_List_List*_tmp351;void*_tmp352;struct Cyc_Absyn_Tqual _tmp353;struct Cyc_Absyn_Exp*_tmp354;union Cyc_Absyn_Constraint*_tmp355;void*_tmp356;struct Cyc_Absyn_Tqual _tmp357;struct Cyc_Absyn_Exp*_tmp358;union Cyc_Absyn_Constraint*_tmp359;struct Cyc_Absyn_Exp*_tmp35A;struct Cyc_Absyn_Exp*_tmp35B;void*_tmp35C;void*_tmp35D;int _tmp35E;int _tmp35F;enum Cyc_Absyn_Sign _tmp360;enum Cyc_Absyn_Size_of _tmp361;enum Cyc_Absyn_Sign _tmp362;enum Cyc_Absyn_Size_of _tmp363;void*_tmp364;struct Cyc_Absyn_Tqual _tmp365;void*_tmp366;union Cyc_Absyn_Constraint*_tmp367;union Cyc_Absyn_Constraint*_tmp368;union Cyc_Absyn_Constraint*_tmp369;void*_tmp36A;struct Cyc_Absyn_Tqual _tmp36B;void*_tmp36C;union Cyc_Absyn_Constraint*_tmp36D;union Cyc_Absyn_Constraint*_tmp36E;union Cyc_Absyn_Constraint*_tmp36F;struct Cyc_Absyn_Datatypedecl*_tmp370;struct Cyc_Absyn_Datatypefield*_tmp371;struct Cyc_List_List*_tmp372;struct Cyc_Absyn_Datatypedecl*_tmp373;struct Cyc_Absyn_Datatypefield*_tmp374;struct Cyc_List_List*_tmp375;struct Cyc_Absyn_Datatypedecl*_tmp376;struct Cyc_List_List*_tmp377;struct Cyc_Absyn_Datatypedecl*_tmp378;struct Cyc_List_List*_tmp379;struct Cyc_List_List*_tmp37A;struct Cyc_List_List*_tmp37B;struct _tuple2*_tmp37C;struct _tuple2*_tmp37D;union Cyc_Absyn_AggrInfoU _tmp37E;struct Cyc_List_List*_tmp37F;union Cyc_Absyn_AggrInfoU _tmp380;struct Cyc_List_List*_tmp381;struct Cyc_Absyn_Tvar*_tmp382;struct Cyc_Absyn_Tvar*_tmp383;switch(*((int*)_tmp327.f1)){case 1: _LL2EB: _LL2EC:
# 1481
 Cyc_Tcutil_unify_it(t2,t1);
return;case 0: switch(*((int*)_tmp327.f2)){case 0: _LL2ED: _LL2EE:
# 1484
 return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 2: switch(*((int*)_tmp327.f2)){case 2: _LL2EF: _tmp382=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp327.f1)->f1;_tmp383=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp327.f2)->f1;_LL2F0: {
# 1487
struct _dyneither_ptr*_tmp384=_tmp382->name;
struct _dyneither_ptr*_tmp385=_tmp383->name;
# 1490
int _tmp386=_tmp382->identity;
int _tmp387=_tmp383->identity;
if(_tmp387 == _tmp386)return;
Cyc_Tcutil_failure_reason=({const char*_tmp388="(variable types are not the same)";_tag_dyneither(_tmp388,sizeof(char),34);});
goto _LL2EA;}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 11: switch(*((int*)_tmp327.f2)){case 11: _LL2F1: _tmp37E=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp327.f1)->f1).aggr_info;_tmp37F=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp327.f1)->f1).targs;_tmp380=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp327.f2)->f1).aggr_info;_tmp381=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp327.f2)->f1).targs;_LL2F2: {
# 1497
struct _tuple13 _tmp389=Cyc_Absyn_aggr_kinded_name(_tmp380);struct _tuple13 _tmp38A=_tmp389;enum Cyc_Absyn_AggrKind _tmp38B;struct _tuple2*_tmp38C;_LL328: _tmp38B=_tmp38A.f1;_tmp38C=_tmp38A.f2;_LL329:;{
struct _tuple13 _tmp38D=Cyc_Absyn_aggr_kinded_name(_tmp37E);struct _tuple13 _tmp38E=_tmp38D;enum Cyc_Absyn_AggrKind _tmp38F;struct _tuple2*_tmp390;_LL32B: _tmp38F=_tmp38E.f1;_tmp390=_tmp38E.f2;_LL32C:;
if(_tmp38B != _tmp38F){Cyc_Tcutil_failure_reason=({const char*_tmp391="(struct and union type)";_tag_dyneither(_tmp391,sizeof(char),24);});goto _LL2EA;}
if(Cyc_Absyn_qvar_cmp(_tmp38C,_tmp390)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp392="(different type name)";_tag_dyneither(_tmp392,sizeof(char),22);});goto _LL2EA;}
Cyc_Tcutil_unify_list(_tmp381,_tmp37F);
return;};}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 13: switch(*((int*)_tmp327.f2)){case 13: _LL2F3: _tmp37C=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp327.f1)->f1;_tmp37D=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp327.f2)->f1;_LL2F4:
# 1506
 if(Cyc_Absyn_qvar_cmp(_tmp37C,_tmp37D)== 0)return;
Cyc_Tcutil_failure_reason=({const char*_tmp393="(different enum types)";_tag_dyneither(_tmp393,sizeof(char),23);});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 14: switch(*((int*)_tmp327.f2)){case 14: _LL2F5: _tmp37A=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp327.f1)->f1;_tmp37B=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp327.f2)->f1;_LL2F6: {
# 1511
int bad=0;
for(0;_tmp37A != 0  && _tmp37B != 0;(_tmp37A=_tmp37A->tl,_tmp37B=_tmp37B->tl)){
struct Cyc_Absyn_Enumfield*_tmp394=(struct Cyc_Absyn_Enumfield*)_tmp37A->hd;
struct Cyc_Absyn_Enumfield*_tmp395=(struct Cyc_Absyn_Enumfield*)_tmp37B->hd;
if(Cyc_Absyn_qvar_cmp(_tmp394->name,_tmp395->name)!= 0){
Cyc_Tcutil_failure_reason=({const char*_tmp396="(different names for enum fields)";_tag_dyneither(_tmp396,sizeof(char),34);});
bad=1;
break;}
# 1520
if(_tmp394->tag == _tmp395->tag)continue;
if(_tmp394->tag == 0  || _tmp395->tag == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp397="(different tag values for enum fields)";_tag_dyneither(_tmp397,sizeof(char),39);});
bad=1;
break;}
# 1526
if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp394->tag),(struct Cyc_Absyn_Exp*)_check_null(_tmp395->tag))){
Cyc_Tcutil_failure_reason=({const char*_tmp398="(different tag values for enum fields)";_tag_dyneither(_tmp398,sizeof(char),39);});
bad=1;
break;}}
# 1532
if(bad)goto _LL2EA;
if(_tmp37A == 0  && _tmp37B == 0)return;
Cyc_Tcutil_failure_reason=({const char*_tmp399="(different number of fields for enums)";_tag_dyneither(_tmp399,sizeof(char),39);});
goto _LL2EA;}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp327.f1)->f1).datatype_info).KnownDatatype).tag == 2)switch(*((int*)_tmp327.f2)){case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp327.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL2F7: _tmp376=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp327.f1)->f1).datatype_info).KnownDatatype).val;_tmp377=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp327.f1)->f1).targs;_tmp378=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp327.f2)->f1).datatype_info).KnownDatatype).val;_tmp379=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp327.f2)->f1).targs;_LL2F8:
# 1539
 if(_tmp376 == _tmp378  || Cyc_Absyn_qvar_cmp(_tmp376->name,_tmp378->name)== 0){
Cyc_Tcutil_unify_list(_tmp379,_tmp377);
return;}
# 1543
goto _LL2EA;}else{goto _LL325;}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}else{switch(*((int*)_tmp327.f2)){case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}}case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp327.f1)->f1).field_info).KnownDatatypefield).tag == 2)switch(*((int*)_tmp327.f2)){case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp327.f2)->f1).field_info).KnownDatatypefield).tag == 2){_LL2F9: _tmp370=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp327.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp371=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp327.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp372=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp327.f1)->f1).targs;_tmp373=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp327.f2)->f1).field_info).KnownDatatypefield).val).f1;_tmp374=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp327.f2)->f1).field_info).KnownDatatypefield).val).f2;_tmp375=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp327.f2)->f1).targs;_LL2FA:
# 1547
 if((_tmp370 == _tmp373  || Cyc_Absyn_qvar_cmp(_tmp370->name,_tmp373->name)== 0) && (
_tmp371 == _tmp374  || Cyc_Absyn_qvar_cmp(_tmp371->name,_tmp374->name)== 0)){
Cyc_Tcutil_unify_list(_tmp375,_tmp372);
return;}
# 1552
Cyc_Tcutil_failure_reason=({const char*_tmp39A="(different datatype field types)";_tag_dyneither(_tmp39A,sizeof(char),33);});
goto _LL2EA;}else{goto _LL325;}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}else{switch(*((int*)_tmp327.f2)){case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}}case 5: switch(*((int*)_tmp327.f2)){case 5: _LL2FB: _tmp364=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp327.f1)->f1).elt_typ;_tmp365=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp327.f1)->f1).elt_tq;_tmp366=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp327.f1)->f1).ptr_atts).rgn;_tmp367=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp327.f1)->f1).ptr_atts).nullable;_tmp368=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp327.f1)->f1).ptr_atts).bounds;_tmp369=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp327.f1)->f1).ptr_atts).zero_term;_tmp36A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp327.f2)->f1).elt_typ;_tmp36B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp327.f2)->f1).elt_tq;_tmp36C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp327.f2)->f1).ptr_atts).rgn;_tmp36D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp327.f2)->f1).ptr_atts).nullable;_tmp36E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp327.f2)->f1).ptr_atts).bounds;_tmp36F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp327.f2)->f1).ptr_atts).zero_term;_LL2FC:
# 1557
 Cyc_Tcutil_unify_it(_tmp36A,_tmp364);
Cyc_Tcutil_unify_it(_tmp366,_tmp36C);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp36F,_tmp369,({const char*_tmp39B="(not both zero terminated)";_tag_dyneither(_tmp39B,sizeof(char),27);}));
Cyc_Tcutil_unify_tqual(_tmp36B,_tmp36A,_tmp365,_tmp364);
((void(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp36E,_tmp368,({const char*_tmp39C="(different pointer bounds)";_tag_dyneither(_tmp39C,sizeof(char),27);}));
# 1565
{void*_tmp39D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp36E);void*_tmp39E=_tmp39D;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp39E)->tag == 0){_LL32E: _LL32F:
 return;}else{_LL330: _LL331:
 goto _LL32D;}_LL32D:;}
# 1569
((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp36D,_tmp367,({const char*_tmp39F="(incompatible pointer types)";_tag_dyneither(_tmp39F,sizeof(char),29);}));
return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 6: switch(*((int*)_tmp327.f2)){case 6: _LL2FD: _tmp360=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp327.f1)->f1;_tmp361=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp327.f1)->f2;_tmp362=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp327.f2)->f1;_tmp363=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp327.f2)->f2;_LL2FE:
# 1574
 if(_tmp362 == _tmp360  && ((_tmp363 == _tmp361  || _tmp363 == Cyc_Absyn_Int_sz  && _tmp361 == Cyc_Absyn_Long_sz) || 
_tmp363 == Cyc_Absyn_Long_sz  && _tmp361 == Cyc_Absyn_Int_sz))return;
Cyc_Tcutil_failure_reason=({const char*_tmp3A0="(different integral types)";_tag_dyneither(_tmp3A0,sizeof(char),27);});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 7: switch(*((int*)_tmp327.f2)){case 7: _LL2FF: _tmp35E=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp327.f1)->f1;_tmp35F=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp327.f2)->f1;_LL300:
# 1580
 if(_tmp35F == 0  && _tmp35E == 0)return;else{
if(_tmp35F == 1  && _tmp35E == 1)return;else{
# 1583
if(((_tmp35F != 0  && _tmp35F != 1) && _tmp35E != 0) && _tmp35E != 1)return;}}
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 19: switch(*((int*)_tmp327.f2)){case 19: _LL301: _tmp35C=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp327.f1)->f1;_tmp35D=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp327.f2)->f1;_LL302:
# 1586
 Cyc_Tcutil_unify_it(_tmp35C,_tmp35D);return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 18: switch(*((int*)_tmp327.f2)){case 18: _LL303: _tmp35A=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp327.f1)->f1;_tmp35B=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp327.f2)->f1;_LL304:
# 1589
 if(!Cyc_Evexp_same_const_exp(_tmp35A,_tmp35B)){
Cyc_Tcutil_failure_reason=({const char*_tmp3A1="(cannot prove expressions are the same)";_tag_dyneither(_tmp3A1,sizeof(char),40);});
goto _LL2EA;}
# 1593
return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 8: switch(*((int*)_tmp327.f2)){case 8: _LL305: _tmp352=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp327.f1)->f1).elt_type;_tmp353=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp327.f1)->f1).tq;_tmp354=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp327.f1)->f1).num_elts;_tmp355=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp327.f1)->f1).zero_term;_tmp356=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp327.f2)->f1).elt_type;_tmp357=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp327.f2)->f1).tq;_tmp358=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp327.f2)->f1).num_elts;_tmp359=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp327.f2)->f1).zero_term;_LL306:
# 1597
 Cyc_Tcutil_unify_it(_tmp356,_tmp352);
Cyc_Tcutil_unify_tqual(_tmp357,_tmp356,_tmp353,_tmp352);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp355,_tmp359,({const char*_tmp3A2="(not both zero terminated)";_tag_dyneither(_tmp3A2,sizeof(char),27);}));
if(_tmp354 == _tmp358)return;
if(_tmp354 == 0  || _tmp358 == 0)goto _LL2EA;
if(Cyc_Evexp_same_const_exp(_tmp354,_tmp358))
return;
Cyc_Tcutil_failure_reason=({const char*_tmp3A3="(different array sizes)";_tag_dyneither(_tmp3A3,sizeof(char),24);});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 9: switch(*((int*)_tmp327.f2)){case 9: _LL307: _tmp338=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f1)->f1).tvars;_tmp339=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f1)->f1).effect;_tmp33A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f1)->f1).ret_tqual;_tmp33B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f1)->f1).ret_typ;_tmp33C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f1)->f1).args;_tmp33D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f1)->f1).c_varargs;_tmp33E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f1)->f1).cyc_varargs;_tmp33F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f1)->f1).rgn_po;_tmp340=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f1)->f1).attributes;_tmp341=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f1)->f1).requires_clause;_tmp342=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f1)->f1).requires_relns;_tmp343=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f1)->f1).ensures_clause;_tmp344=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f1)->f1).ensures_relns;_tmp345=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f2)->f1).tvars;_tmp346=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f2)->f1).effect;_tmp347=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f2)->f1).ret_tqual;_tmp348=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f2)->f1).ret_typ;_tmp349=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f2)->f1).args;_tmp34A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f2)->f1).c_varargs;_tmp34B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f2)->f1).cyc_varargs;_tmp34C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f2)->f1).rgn_po;_tmp34D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f2)->f1).attributes;_tmp34E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f2)->f1).requires_clause;_tmp34F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f2)->f1).requires_relns;_tmp350=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f2)->f1).ensures_clause;_tmp351=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f2)->f1).ensures_relns;_LL308: {
# 1611
int done=0;
struct _RegionHandle _tmp3A4=_new_region("rgn");struct _RegionHandle*rgn=& _tmp3A4;_push_region(rgn);{
struct Cyc_List_List*inst=0;
while(_tmp345 != 0){
if(_tmp338 == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp3A5="(second function type has too few type variables)";_tag_dyneither(_tmp3A5,sizeof(char),50);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1619
void*_tmp3A6=((struct Cyc_Absyn_Tvar*)_tmp345->hd)->kind;
void*_tmp3A7=((struct Cyc_Absyn_Tvar*)_tmp338->hd)->kind;
if(!Cyc_Tcutil_unify_kindbound(_tmp3A6,_tmp3A7)){
Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3AC;_tmp3AC.tag=0;_tmp3AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1625
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp338->hd,& Cyc_Tcutil_bk)));({struct Cyc_String_pa_PrintArg_struct _tmp3AB;_tmp3AB.tag=0;_tmp3AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1624
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp345->hd,& Cyc_Tcutil_bk)));({struct Cyc_String_pa_PrintArg_struct _tmp3AA;_tmp3AA.tag=0;_tmp3AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1623
Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)_tmp345->hd));({void*_tmp3A8[3]={& _tmp3AA,& _tmp3AB,& _tmp3AC};Cyc_aprintf(({const char*_tmp3A9="(type var %s has different kinds %s and %s)";_tag_dyneither(_tmp3A9,sizeof(char),44);}),_tag_dyneither(_tmp3A8,sizeof(void*),3));});});});});
# 1626
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1628
inst=({struct Cyc_List_List*_tmp3AD=_region_malloc(rgn,sizeof(*_tmp3AD));_tmp3AD->hd=({struct _tuple16*_tmp3AE=_region_malloc(rgn,sizeof(*_tmp3AE));_tmp3AE->f1=(struct Cyc_Absyn_Tvar*)_tmp338->hd;_tmp3AE->f2=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3AF=_cycalloc(sizeof(*_tmp3AF));_tmp3AF[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp3B0;_tmp3B0.tag=2;_tmp3B0.f1=(struct Cyc_Absyn_Tvar*)_tmp345->hd;_tmp3B0;});_tmp3AF;});_tmp3AE;});_tmp3AD->tl=inst;_tmp3AD;});
_tmp345=_tmp345->tl;
_tmp338=_tmp338->tl;};}
# 1632
if(_tmp338 != 0){
Cyc_Tcutil_failure_reason=({const char*_tmp3B1="(second function type has too many type variables)";_tag_dyneither(_tmp3B1,sizeof(char),51);});
_npop_handler(0);goto _LL2EA;}
# 1636
if(inst != 0){
Cyc_Tcutil_unify_it((void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp3B2=_cycalloc(sizeof(*_tmp3B2));_tmp3B2[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp3B3;_tmp3B3.tag=9;_tmp3B3.f1=({struct Cyc_Absyn_FnInfo _tmp3B4;_tmp3B4.tvars=0;_tmp3B4.effect=_tmp346;_tmp3B4.ret_tqual=_tmp347;_tmp3B4.ret_typ=_tmp348;_tmp3B4.args=_tmp349;_tmp3B4.c_varargs=_tmp34A;_tmp3B4.cyc_varargs=_tmp34B;_tmp3B4.rgn_po=_tmp34C;_tmp3B4.attributes=_tmp34D;_tmp3B4.requires_clause=_tmp341;_tmp3B4.requires_relns=_tmp342;_tmp3B4.ensures_clause=_tmp343;_tmp3B4.ensures_relns=_tmp344;_tmp3B4;});_tmp3B3;});_tmp3B2;}),
# 1640
Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp3B5=_cycalloc(sizeof(*_tmp3B5));_tmp3B5[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp3B6;_tmp3B6.tag=9;_tmp3B6.f1=({struct Cyc_Absyn_FnInfo _tmp3B7;_tmp3B7.tvars=0;_tmp3B7.effect=_tmp339;_tmp3B7.ret_tqual=_tmp33A;_tmp3B7.ret_typ=_tmp33B;_tmp3B7.args=_tmp33C;_tmp3B7.c_varargs=_tmp33D;_tmp3B7.cyc_varargs=_tmp33E;_tmp3B7.rgn_po=_tmp33F;_tmp3B7.attributes=_tmp340;_tmp3B7.requires_clause=_tmp34E;_tmp3B7.requires_relns=_tmp34F;_tmp3B7.ensures_clause=_tmp350;_tmp3B7.ensures_relns=_tmp351;_tmp3B7;});_tmp3B6;});_tmp3B5;})));
# 1645
done=1;}}
# 1648
if(done){
_npop_handler(0);return;}
Cyc_Tcutil_unify_it(_tmp348,_tmp33B);
Cyc_Tcutil_unify_tqual(_tmp347,_tmp348,_tmp33A,_tmp33B);
for(0;_tmp349 != 0  && _tmp33C != 0;(_tmp349=_tmp349->tl,_tmp33C=_tmp33C->tl)){
struct _tuple10 _tmp3B8=*((struct _tuple10*)_tmp349->hd);struct _tuple10 _tmp3B9=_tmp3B8;struct Cyc_Absyn_Tqual _tmp3BA;void*_tmp3BB;_LL333: _tmp3BA=_tmp3B9.f2;_tmp3BB=_tmp3B9.f3;_LL334:;{
struct _tuple10 _tmp3BC=*((struct _tuple10*)_tmp33C->hd);struct _tuple10 _tmp3BD=_tmp3BC;struct Cyc_Absyn_Tqual _tmp3BE;void*_tmp3BF;_LL336: _tmp3BE=_tmp3BD.f2;_tmp3BF=_tmp3BD.f3;_LL337:;
Cyc_Tcutil_unify_it(_tmp3BB,_tmp3BF);
Cyc_Tcutil_unify_tqual(_tmp3BA,_tmp3BB,_tmp3BE,_tmp3BF);};}
# 1658
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(_tmp349 != 0  || _tmp33C != 0){
Cyc_Tcutil_failure_reason=({const char*_tmp3C0="(function types have different number of arguments)";_tag_dyneither(_tmp3C0,sizeof(char),52);});
_npop_handler(0);goto _LL2EA;}
# 1664
if(_tmp34A != _tmp33D){
Cyc_Tcutil_failure_reason=({const char*_tmp3C1="(only one function type takes C varargs)";_tag_dyneither(_tmp3C1,sizeof(char),41);});
_npop_handler(0);goto _LL2EA;}{
# 1669
int bad_cyc_vararg=0;
{struct _tuple22 _tmp3C2=({struct _tuple22 _tmp3CE;_tmp3CE.f1=_tmp34B;_tmp3CE.f2=_tmp33E;_tmp3CE;});struct _tuple22 _tmp3C3=_tmp3C2;struct _dyneither_ptr*_tmp3C4;struct Cyc_Absyn_Tqual _tmp3C5;void*_tmp3C6;int _tmp3C7;struct _dyneither_ptr*_tmp3C8;struct Cyc_Absyn_Tqual _tmp3C9;void*_tmp3CA;int _tmp3CB;if(_tmp3C3.f1 == 0){if(_tmp3C3.f2 == 0){_LL339: _LL33A:
 goto _LL338;}else{_LL33B: _LL33C:
 goto _LL33E;}}else{if(_tmp3C3.f2 == 0){_LL33D: _LL33E:
# 1674
 bad_cyc_vararg=1;
Cyc_Tcutil_failure_reason=({const char*_tmp3CC="(only one function type takes varargs)";_tag_dyneither(_tmp3CC,sizeof(char),39);});
goto _LL338;}else{_LL33F: _tmp3C4=(_tmp3C3.f1)->name;_tmp3C5=(_tmp3C3.f1)->tq;_tmp3C6=(_tmp3C3.f1)->type;_tmp3C7=(_tmp3C3.f1)->inject;_tmp3C8=(_tmp3C3.f2)->name;_tmp3C9=(_tmp3C3.f2)->tq;_tmp3CA=(_tmp3C3.f2)->type;_tmp3CB=(_tmp3C3.f2)->inject;_LL340:
# 1678
 Cyc_Tcutil_unify_it(_tmp3C6,_tmp3CA);
Cyc_Tcutil_unify_tqual(_tmp3C5,_tmp3C6,_tmp3C9,_tmp3CA);
if(_tmp3C7 != _tmp3CB){
bad_cyc_vararg=1;
Cyc_Tcutil_failure_reason=({const char*_tmp3CD="(only one function type injects varargs)";_tag_dyneither(_tmp3CD,sizeof(char),41);});}
# 1684
goto _LL338;}}_LL338:;}
# 1686
if(bad_cyc_vararg){_npop_handler(0);goto _LL2EA;}{
# 1689
int bad_effect=0;
{struct _tuple0 _tmp3CF=({struct _tuple0 _tmp3D1;_tmp3D1.f1=_tmp346;_tmp3D1.f2=_tmp339;_tmp3D1;});struct _tuple0 _tmp3D0=_tmp3CF;if(_tmp3D0.f1 == 0){if(_tmp3D0.f2 == 0){_LL342: _LL343:
 goto _LL341;}else{_LL344: _LL345:
 goto _LL347;}}else{if(_tmp3D0.f2 == 0){_LL346: _LL347:
 bad_effect=1;goto _LL341;}else{_LL348: _LL349:
 bad_effect=!Cyc_Tcutil_unify_effect((void*)_check_null(_tmp346),(void*)_check_null(_tmp339));goto _LL341;}}_LL341:;}
# 1696
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(bad_effect){
Cyc_Tcutil_failure_reason=({const char*_tmp3D2="(function effects do not match)";_tag_dyneither(_tmp3D2,sizeof(char),32);});
_npop_handler(0);goto _LL2EA;}
# 1702
if(!Cyc_Tcutil_same_atts(_tmp340,_tmp34D)){
Cyc_Tcutil_failure_reason=({const char*_tmp3D3="(function types have different attributes)";_tag_dyneither(_tmp3D3,sizeof(char),43);});
_npop_handler(0);goto _LL2EA;}
# 1706
if(!Cyc_Tcutil_same_rgn_po(_tmp33F,_tmp34C)){
Cyc_Tcutil_failure_reason=({const char*_tmp3D4="(function types have different region lifetime orderings)";_tag_dyneither(_tmp3D4,sizeof(char),58);});
_npop_handler(0);goto _LL2EA;}
# 1710
if(!Cyc_Tcutil_check_logical_equivalence(_tmp342,_tmp34F)){
Cyc_Tcutil_failure_reason=({const char*_tmp3D5="(@requires clauses not equivalent)";_tag_dyneither(_tmp3D5,sizeof(char),35);});
_npop_handler(0);goto _LL2EA;}
# 1714
if(!Cyc_Tcutil_check_logical_equivalence(_tmp344,_tmp351)){
Cyc_Tcutil_failure_reason=({const char*_tmp3D6="(@ensures clauses not equivalent)";_tag_dyneither(_tmp3D6,sizeof(char),34);});
_npop_handler(0);goto _LL2EA;}
# 1718
_npop_handler(0);return;};};
# 1612
;_pop_region(rgn);}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 10: switch(*((int*)_tmp327.f2)){case 10: _LL309: _tmp336=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp327.f1)->f1;_tmp337=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp327.f2)->f1;_LL30A:
# 1721
 for(0;_tmp337 != 0  && _tmp336 != 0;(_tmp337=_tmp337->tl,_tmp336=_tmp336->tl)){
struct _tuple12 _tmp3D7=*((struct _tuple12*)_tmp337->hd);struct _tuple12 _tmp3D8=_tmp3D7;struct Cyc_Absyn_Tqual _tmp3D9;void*_tmp3DA;_LL34B: _tmp3D9=_tmp3D8.f1;_tmp3DA=_tmp3D8.f2;_LL34C:;{
struct _tuple12 _tmp3DB=*((struct _tuple12*)_tmp336->hd);struct _tuple12 _tmp3DC=_tmp3DB;struct Cyc_Absyn_Tqual _tmp3DD;void*_tmp3DE;_LL34E: _tmp3DD=_tmp3DC.f1;_tmp3DE=_tmp3DC.f2;_LL34F:;
Cyc_Tcutil_unify_it(_tmp3DA,_tmp3DE);
Cyc_Tcutil_unify_tqual(_tmp3D9,_tmp3DA,_tmp3DD,_tmp3DE);};}
# 1727
if(_tmp337 == 0  && _tmp336 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp3DF="(tuple types have different numbers of components)";_tag_dyneither(_tmp3DF,sizeof(char),51);});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 12: switch(*((int*)_tmp327.f2)){case 12: _LL30B: _tmp332=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp327.f1)->f1;_tmp333=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp327.f1)->f2;_tmp334=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp327.f2)->f1;_tmp335=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp327.f2)->f2;_LL30C:
# 1734
 if(_tmp334 != _tmp332){Cyc_Tcutil_failure_reason=({const char*_tmp3E0="(struct and union type)";_tag_dyneither(_tmp3E0,sizeof(char),24);});goto _LL2EA;}
for(0;_tmp335 != 0  && _tmp333 != 0;(_tmp335=_tmp335->tl,_tmp333=_tmp333->tl)){
struct Cyc_Absyn_Aggrfield*_tmp3E1=(struct Cyc_Absyn_Aggrfield*)_tmp335->hd;
struct Cyc_Absyn_Aggrfield*_tmp3E2=(struct Cyc_Absyn_Aggrfield*)_tmp333->hd;
if(Cyc_strptrcmp(_tmp3E1->name,_tmp3E2->name)!= 0){
Cyc_Tcutil_failure_reason=({const char*_tmp3E3="(different member names)";_tag_dyneither(_tmp3E3,sizeof(char),25);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1742
Cyc_Tcutil_unify_it(_tmp3E1->type,_tmp3E2->type);
Cyc_Tcutil_unify_tqual(_tmp3E1->tq,_tmp3E1->type,_tmp3E2->tq,_tmp3E2->type);
if(!Cyc_Tcutil_same_atts(_tmp3E1->attributes,_tmp3E2->attributes)){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp3E4="(different attributes on member)";_tag_dyneither(_tmp3E4,sizeof(char),33);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1750
if((_tmp3E1->width != 0  && _tmp3E2->width == 0  || 
_tmp3E2->width != 0  && _tmp3E1->width == 0) || 
(_tmp3E1->width != 0  && _tmp3E2->width != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp3E1->width),(struct Cyc_Absyn_Exp*)_check_null(_tmp3E2->width))){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp3E5="(different bitfield widths on member)";_tag_dyneither(_tmp3E5,sizeof(char),38);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1759
if((_tmp3E1->requires_clause != 0  && _tmp3E2->requires_clause == 0  || 
_tmp3E1->requires_clause == 0  && _tmp3E2->requires_clause != 0) || 
(_tmp3E1->requires_clause == 0  && _tmp3E2->requires_clause != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp3E1->requires_clause),(struct Cyc_Absyn_Exp*)_check_null(_tmp3E2->requires_clause))){
# 1764
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp3E6="(different @requires clauses on member)";_tag_dyneither(_tmp3E6,sizeof(char),40);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1770
if(_tmp335 == 0  && _tmp333 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp3E7="(different number of members)";_tag_dyneither(_tmp3E7,sizeof(char),30);});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 17: switch(*((int*)_tmp327.f2)){case 17: _LL30D: _tmp32E=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp327.f1)->f2;_tmp32F=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp327.f1)->f3;_tmp330=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp327.f2)->f2;_tmp331=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp327.f2)->f3;_LL30E:
# 1776
 if(_tmp32F != _tmp331){
Cyc_Tcutil_failure_reason=({const char*_tmp3E8="(different abstract typedefs)";_tag_dyneither(_tmp3E8,sizeof(char),30);});
goto _LL2EA;}
# 1780
Cyc_Tcutil_failure_reason=({const char*_tmp3E9="(type parameters to typedef differ)";_tag_dyneither(_tmp3E9,sizeof(char),36);});
Cyc_Tcutil_unify_list(_tmp32E,_tmp330);
return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 20: switch(*((int*)_tmp327.f2)){case 20: _LL30F: _LL310:
 return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 21: switch(*((int*)_tmp327.f2)){case 21: _LL311: _LL312:
 return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 22: switch(*((int*)_tmp327.f2)){case 22: _LL313: _LL314:
 return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 15: switch(*((int*)_tmp327.f2)){case 15: _LL315: _tmp32C=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp327.f1)->f1;_tmp32D=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp327.f2)->f1;_LL316:
# 1787
 Cyc_Tcutil_unify_it(_tmp32C,_tmp32D);
return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 16: switch(*((int*)_tmp327.f2)){case 16: _LL317: _tmp328=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp327.f1)->f1;_tmp329=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp327.f1)->f2;_tmp32A=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp327.f2)->f1;_tmp32B=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp327.f2)->f2;_LL318:
# 1790
 Cyc_Tcutil_unify_it(_tmp328,_tmp32A);
Cyc_Tcutil_unify_it(_tmp329,_tmp32B);
return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 24: _LL319: _LL31A:
 goto _LL31C;default: if(((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp327.f2)->tag == 24){_LL31B: _LL31C:
 goto _LL31E;}else{switch(*((int*)_tmp327.f1)){case 23: _LL31D: _LL31E:
 goto _LL320;case 25: _LL31F: _LL320:
 goto _LL322;default: switch(*((int*)_tmp327.f2)){case 25: _LL321: _LL322:
 goto _LL324;case 23: _LL323: _LL324:
# 1799
 if(Cyc_Tcutil_unify_effect(t1,t2))return;
Cyc_Tcutil_failure_reason=({const char*_tmp3EA="(effects don't unify)";_tag_dyneither(_tmp3EA,sizeof(char),22);});
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
int _tmp3EC=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
int _tmp3ED=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp3EC,_tmp3ED);}
# 1821
static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return 0;{
union Cyc_Absyn_Constraint*_tmp3EE=x;void*_tmp3EF;switch((((union Cyc_Absyn_Constraint*)_tmp3EE)->Eq_constr).tag){case 3: _LL351: _LL352:
 return - 1;case 1: _LL353: _tmp3EF=(void*)(_tmp3EE->Eq_constr).val;_LL354: {
# 1828
union Cyc_Absyn_Constraint*_tmp3F0=y;void*_tmp3F1;switch((((union Cyc_Absyn_Constraint*)_tmp3F0)->Eq_constr).tag){case 3: _LL358: _LL359:
 return 1;case 1: _LL35A: _tmp3F1=(void*)(_tmp3F0->Eq_constr).val;_LL35B:
 return cmp(_tmp3EF,_tmp3F1);default: _LL35C: _LL35D:
({void*_tmp3F2=0;Cyc_Tcutil_impos(({const char*_tmp3F3="unify_conref: forward after compress(2)";_tag_dyneither(_tmp3F3,sizeof(char),40);}),_tag_dyneither(_tmp3F2,sizeof(void*),0));});}_LL357:;}default: _LL355: _LL356:
# 1833
({void*_tmp3F4=0;Cyc_Tcutil_impos(({const char*_tmp3F5="unify_conref: forward after compress";_tag_dyneither(_tmp3F5,sizeof(char),37);}),_tag_dyneither(_tmp3F4,sizeof(void*),0));});}_LL350:;};}
# 1837
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct _tuple12*_tmp3F6=tqt1;struct Cyc_Absyn_Tqual _tmp3F7;void*_tmp3F8;_LL35F: _tmp3F7=_tmp3F6->f1;_tmp3F8=_tmp3F6->f2;_LL360:;{
struct _tuple12*_tmp3F9=tqt2;struct Cyc_Absyn_Tqual _tmp3FA;void*_tmp3FB;_LL362: _tmp3FA=_tmp3F9->f1;_tmp3FB=_tmp3F9->f2;_LL363:;{
int _tmp3FC=Cyc_Tcutil_tqual_cmp(_tmp3F7,_tmp3FA);
if(_tmp3FC != 0)return _tmp3FC;
return Cyc_Tcutil_typecmp(_tmp3F8,_tmp3FB);};};}
# 1845
static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp3FD=Cyc_strptrcmp(f1->name,f2->name);
if(_tmp3FD != 0)return _tmp3FD;{
int _tmp3FE=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);
if(_tmp3FE != 0)return _tmp3FE;{
int _tmp3FF=Cyc_Tcutil_typecmp(f1->type,f2->type);
if(_tmp3FF != 0)return _tmp3FF;{
int _tmp400=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);
if(_tmp400 != 0)return _tmp400;
_tmp400=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);
if(_tmp400 != 0)return _tmp400;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->requires_clause,f2->requires_clause);};};};}
# 1859
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp401=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp401 != 0)return _tmp401;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}
# 1865
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp402=t;switch(*((int*)_tmp402)){case 0: _LL365: _LL366:
 return 0;case 1: _LL367: _LL368:
 return 1;case 2: _LL369: _LL36A:
 return 2;case 3: _LL36B: _LL36C:
 return 3;case 4: _LL36D: _LL36E:
 return 4;case 5: _LL36F: _LL370:
 return 5;case 6: _LL371: _LL372:
 return 6;case 7: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp402)->f1 == 0){_LL373: _LL374:
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
int _tmp403=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));
if(_tmp403 != 0)
return _tmp403;{
# 1911
struct _tuple0 _tmp404=({struct _tuple0 _tmp467;_tmp467.f1=t2;_tmp467.f2=t1;_tmp467;});struct _tuple0 _tmp405=_tmp404;struct _dyneither_ptr _tmp406;struct _dyneither_ptr _tmp407;struct Cyc_Absyn_Exp*_tmp408;struct Cyc_Absyn_Exp*_tmp409;struct Cyc_Absyn_Exp*_tmp40A;struct Cyc_Absyn_Exp*_tmp40B;void*_tmp40C;void*_tmp40D;void*_tmp40E;void*_tmp40F;void*_tmp410;void*_tmp411;void*_tmp412;void*_tmp413;enum Cyc_Absyn_AggrKind _tmp414;struct Cyc_List_List*_tmp415;enum Cyc_Absyn_AggrKind _tmp416;struct Cyc_List_List*_tmp417;struct Cyc_List_List*_tmp418;struct Cyc_List_List*_tmp419;void*_tmp41A;struct Cyc_Absyn_Tqual _tmp41B;struct Cyc_Absyn_Exp*_tmp41C;union Cyc_Absyn_Constraint*_tmp41D;void*_tmp41E;struct Cyc_Absyn_Tqual _tmp41F;struct Cyc_Absyn_Exp*_tmp420;union Cyc_Absyn_Constraint*_tmp421;int _tmp422;int _tmp423;enum Cyc_Absyn_Sign _tmp424;enum Cyc_Absyn_Size_of _tmp425;enum Cyc_Absyn_Sign _tmp426;enum Cyc_Absyn_Size_of _tmp427;void*_tmp428;struct Cyc_Absyn_Tqual _tmp429;void*_tmp42A;union Cyc_Absyn_Constraint*_tmp42B;union Cyc_Absyn_Constraint*_tmp42C;union Cyc_Absyn_Constraint*_tmp42D;void*_tmp42E;struct Cyc_Absyn_Tqual _tmp42F;void*_tmp430;union Cyc_Absyn_Constraint*_tmp431;union Cyc_Absyn_Constraint*_tmp432;union Cyc_Absyn_Constraint*_tmp433;struct Cyc_Absyn_Datatypedecl*_tmp434;struct Cyc_Absyn_Datatypefield*_tmp435;struct Cyc_List_List*_tmp436;struct Cyc_Absyn_Datatypedecl*_tmp437;struct Cyc_Absyn_Datatypefield*_tmp438;struct Cyc_List_List*_tmp439;struct Cyc_Absyn_Datatypedecl*_tmp43A;struct Cyc_List_List*_tmp43B;struct Cyc_Absyn_Datatypedecl*_tmp43C;struct Cyc_List_List*_tmp43D;struct Cyc_List_List*_tmp43E;struct Cyc_List_List*_tmp43F;struct _tuple2*_tmp440;struct _tuple2*_tmp441;union Cyc_Absyn_AggrInfoU _tmp442;struct Cyc_List_List*_tmp443;union Cyc_Absyn_AggrInfoU _tmp444;struct Cyc_List_List*_tmp445;struct Cyc_Absyn_Tvar*_tmp446;struct Cyc_Absyn_Tvar*_tmp447;switch(*((int*)_tmp405.f1)){case 1: switch(*((int*)_tmp405.f2)){case 1: _LL3A2: _LL3A3:
# 1913
({void*_tmp448=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp449="typecmp: can only compare closed types";_tag_dyneither(_tmp449,sizeof(char),39);}),_tag_dyneither(_tmp448,sizeof(void*),0));});case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 2: switch(*((int*)_tmp405.f2)){case 2: _LL3A4: _tmp446=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp405.f1)->f1;_tmp447=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp405.f2)->f1;_LL3A5:
# 1917
 return Cyc_Core_intcmp(_tmp447->identity,_tmp446->identity);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 11: switch(*((int*)_tmp405.f2)){case 11: _LL3A6: _tmp442=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp405.f1)->f1).aggr_info;_tmp443=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp405.f1)->f1).targs;_tmp444=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp405.f2)->f1).aggr_info;_tmp445=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp405.f2)->f1).targs;_LL3A7: {
# 1921
struct _tuple13 _tmp44A=Cyc_Absyn_aggr_kinded_name(_tmp442);struct _tuple13 _tmp44B=_tmp44A;struct _tuple2*_tmp44C;_LL3D9: _tmp44C=_tmp44B.f2;_LL3DA:;{
struct _tuple13 _tmp44D=Cyc_Absyn_aggr_kinded_name(_tmp444);struct _tuple13 _tmp44E=_tmp44D;struct _tuple2*_tmp44F;_LL3DC: _tmp44F=_tmp44E.f2;_LL3DD:;{
int _tmp450=Cyc_Absyn_qvar_cmp(_tmp44C,_tmp44F);
if(_tmp450 != 0)return _tmp450;else{
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp443,_tmp445);}};};}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 13: switch(*((int*)_tmp405.f2)){case 13: _LL3A8: _tmp440=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp405.f1)->f1;_tmp441=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp405.f2)->f1;_LL3A9:
# 1928
 return Cyc_Absyn_qvar_cmp(_tmp440,_tmp441);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 14: switch(*((int*)_tmp405.f2)){case 14: _LL3AA: _tmp43E=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp405.f1)->f1;_tmp43F=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp405.f2)->f1;_LL3AB:
# 1931
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp43E,_tmp43F);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp405.f1)->f1).datatype_info).KnownDatatype).tag == 2)switch(*((int*)_tmp405.f2)){case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp405.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL3AC: _tmp43A=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp405.f1)->f1).datatype_info).KnownDatatype).val;_tmp43B=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp405.f1)->f1).targs;_tmp43C=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp405.f2)->f1).datatype_info).KnownDatatype).val;_tmp43D=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp405.f2)->f1).targs;_LL3AD:
# 1935
 if(_tmp43C == _tmp43A)return 0;{
int _tmp451=Cyc_Absyn_qvar_cmp(_tmp43C->name,_tmp43A->name);
if(_tmp451 != 0)return _tmp451;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp43D,_tmp43B);};}else{goto _LL3D6;}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}else{switch(*((int*)_tmp405.f2)){case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}}case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp405.f1)->f1).field_info).KnownDatatypefield).tag == 2)switch(*((int*)_tmp405.f2)){case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp405.f2)->f1).field_info).KnownDatatypefield).tag == 2){_LL3AE: _tmp434=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp405.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp435=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp405.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp436=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp405.f1)->f1).targs;_tmp437=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp405.f2)->f1).field_info).KnownDatatypefield).val).f1;_tmp438=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp405.f2)->f1).field_info).KnownDatatypefield).val).f2;_tmp439=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp405.f2)->f1).targs;_LL3AF:
# 1942
 if(_tmp437 == _tmp434)return 0;{
int _tmp452=Cyc_Absyn_qvar_cmp(_tmp434->name,_tmp437->name);
if(_tmp452 != 0)return _tmp452;{
int _tmp453=Cyc_Absyn_qvar_cmp(_tmp435->name,_tmp438->name);
if(_tmp453 != 0)return _tmp453;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp439,_tmp436);};};}else{goto _LL3D6;}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}else{switch(*((int*)_tmp405.f2)){case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}}case 5: switch(*((int*)_tmp405.f2)){case 5: _LL3B0: _tmp428=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp405.f1)->f1).elt_typ;_tmp429=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp405.f1)->f1).elt_tq;_tmp42A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp405.f1)->f1).ptr_atts).rgn;_tmp42B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp405.f1)->f1).ptr_atts).nullable;_tmp42C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp405.f1)->f1).ptr_atts).bounds;_tmp42D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp405.f1)->f1).ptr_atts).zero_term;_tmp42E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp405.f2)->f1).elt_typ;_tmp42F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp405.f2)->f1).elt_tq;_tmp430=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp405.f2)->f1).ptr_atts).rgn;_tmp431=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp405.f2)->f1).ptr_atts).nullable;_tmp432=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp405.f2)->f1).ptr_atts).bounds;_tmp433=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp405.f2)->f1).ptr_atts).zero_term;_LL3B1: {
# 1951
int _tmp454=Cyc_Tcutil_typecmp(_tmp42E,_tmp428);
if(_tmp454 != 0)return _tmp454;{
int _tmp455=Cyc_Tcutil_typecmp(_tmp430,_tmp42A);
if(_tmp455 != 0)return _tmp455;{
int _tmp456=Cyc_Tcutil_tqual_cmp(_tmp42F,_tmp429);
if(_tmp456 != 0)return _tmp456;{
int _tmp457=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp432,_tmp42C);
if(_tmp457 != 0)return _tmp457;{
int _tmp458=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp433,_tmp42D);
if(_tmp458 != 0)return _tmp458;
{void*_tmp459=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp432);void*_tmp45A=_tmp459;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp45A)->tag == 0){_LL3DF: _LL3E0:
 return 0;}else{_LL3E1: _LL3E2:
 goto _LL3DE;}_LL3DE:;}
# 1965
return((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp431,_tmp42B);};};};};}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 6: switch(*((int*)_tmp405.f2)){case 6: _LL3B2: _tmp424=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp405.f1)->f1;_tmp425=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp405.f1)->f2;_tmp426=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp405.f2)->f1;_tmp427=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp405.f2)->f2;_LL3B3:
# 1968
 if(_tmp426 != _tmp424)return Cyc_Core_intcmp((int)((unsigned int)_tmp426),(int)((unsigned int)_tmp424));
if(_tmp427 != _tmp425)return Cyc_Core_intcmp((int)((unsigned int)_tmp427),(int)((unsigned int)_tmp425));
return 0;case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 7: switch(*((int*)_tmp405.f2)){case 7: _LL3B4: _tmp422=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp405.f1)->f1;_tmp423=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp405.f2)->f1;_LL3B5:
# 1973
 if(_tmp422 == _tmp423)return 0;else{
if(_tmp423 == 0)return - 1;else{
if(_tmp423 == 1  && _tmp422 == 0)return - 1;else{
return 1;}}}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 8: switch(*((int*)_tmp405.f2)){case 8: _LL3B6: _tmp41A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp405.f1)->f1).elt_type;_tmp41B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp405.f1)->f1).tq;_tmp41C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp405.f1)->f1).num_elts;_tmp41D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp405.f1)->f1).zero_term;_tmp41E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp405.f2)->f1).elt_type;_tmp41F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp405.f2)->f1).tq;_tmp420=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp405.f2)->f1).num_elts;_tmp421=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp405.f2)->f1).zero_term;_LL3B7: {
# 1980
int _tmp45B=Cyc_Tcutil_tqual_cmp(_tmp41F,_tmp41B);
if(_tmp45B != 0)return _tmp45B;{
int _tmp45C=Cyc_Tcutil_typecmp(_tmp41E,_tmp41A);
if(_tmp45C != 0)return _tmp45C;{
int _tmp45D=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp41D,_tmp421);
if(_tmp45D != 0)return _tmp45D;
if(_tmp41C == _tmp420)return 0;
if(_tmp41C == 0  || _tmp420 == 0)
({void*_tmp45E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp45F="missing expression in array index";_tag_dyneither(_tmp45F,sizeof(char),34);}),_tag_dyneither(_tmp45E,sizeof(void*),0));});
# 1990
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp41C,_tmp420);};};}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 9: switch(*((int*)_tmp405.f2)){case 9: _LL3B8: _LL3B9:
# 1993
({void*_tmp460=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp461="typecmp: function types not handled";_tag_dyneither(_tmp461,sizeof(char),36);}),_tag_dyneither(_tmp460,sizeof(void*),0));});case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 10: switch(*((int*)_tmp405.f2)){case 10: _LL3BA: _tmp418=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp405.f1)->f1;_tmp419=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp405.f2)->f1;_LL3BB:
# 1996
 return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp419,_tmp418);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 12: switch(*((int*)_tmp405.f2)){case 12: _LL3BC: _tmp414=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp405.f1)->f1;_tmp415=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp405.f1)->f2;_tmp416=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp405.f2)->f1;_tmp417=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp405.f2)->f2;_LL3BD:
# 1999
 if(_tmp416 != _tmp414){
if(_tmp416 == Cyc_Absyn_StructA)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp417,_tmp415);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 15: switch(*((int*)_tmp405.f2)){case 15: _LL3BE: _tmp412=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp405.f1)->f1;_tmp413=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp405.f2)->f1;_LL3BF:
# 2004
 return Cyc_Tcutil_typecmp(_tmp412,_tmp413);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 16: switch(*((int*)_tmp405.f2)){case 16: _LL3C0: _tmp40E=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp405.f1)->f1;_tmp40F=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp405.f1)->f2;_tmp410=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp405.f2)->f1;_tmp411=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp405.f2)->f2;_LL3C1: {
# 2006
int _tmp462=Cyc_Tcutil_typecmp(_tmp40E,_tmp410);
if(_tmp462 != 0)return _tmp462;else{
return Cyc_Tcutil_typecmp(_tmp40F,_tmp411);}}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 19: switch(*((int*)_tmp405.f2)){case 19: _LL3C2: _tmp40C=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp405.f1)->f1;_tmp40D=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp405.f2)->f1;_LL3C3:
 return Cyc_Tcutil_typecmp(_tmp40C,_tmp40D);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 18: switch(*((int*)_tmp405.f2)){case 18: _LL3C4: _tmp40A=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp405.f1)->f1;_tmp40B=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp405.f2)->f1;_LL3C5:
 _tmp408=_tmp40A;_tmp409=_tmp40B;goto _LL3C7;case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 27: switch(*((int*)_tmp405.f2)){case 27: _LL3C6: _tmp408=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp405.f1)->f1;_tmp409=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp405.f2)->f1;_LL3C7:
# 2012
 return Cyc_Evexp_const_exp_cmp(_tmp408,_tmp409);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 28: switch(*((int*)_tmp405.f2)){case 28: _LL3C8: _tmp406=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp405.f1)->f1;_tmp407=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp405.f2)->f1;_LL3C9:
# 2014
 return Cyc_strcmp((struct _dyneither_ptr)_tmp406,(struct _dyneither_ptr)_tmp407);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 24: _LL3CA: _LL3CB:
 goto _LL3CD;default: if(((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp405.f2)->tag == 24){_LL3CC: _LL3CD:
 goto _LL3CF;}else{switch(*((int*)_tmp405.f1)){case 23: _LL3CE: _LL3CF:
 goto _LL3D1;case 25: _LL3D0: _LL3D1:
 goto _LL3D3;default: switch(*((int*)_tmp405.f2)){case 25: _LL3D2: _LL3D3:
 goto _LL3D5;case 23: _LL3D4: _LL3D5:
({void*_tmp463=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp464="typecmp: effects not handled";_tag_dyneither(_tmp464,sizeof(char),29);}),_tag_dyneither(_tmp463,sizeof(void*),0));});default: _LL3D6: _LL3D7:
({void*_tmp465=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp466="Unmatched case in typecmp";_tag_dyneither(_tmp466,sizeof(char),26);}),_tag_dyneither(_tmp465,sizeof(void*),0));});}}}}_LL3A1:;};};}
# 2025
int Cyc_Tcutil_is_arithmetic_type(void*t){
void*_tmp468=Cyc_Tcutil_compress(t);void*_tmp469=_tmp468;switch(*((int*)_tmp469)){case 6: _LL3E4: _LL3E5:
 goto _LL3E7;case 7: _LL3E6: _LL3E7:
 goto _LL3E9;case 13: _LL3E8: _LL3E9:
 goto _LL3EB;case 14: _LL3EA: _LL3EB:
 return 1;default: _LL3EC: _LL3ED:
 return 0;}_LL3E3:;}
# 2037
int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp46A=({struct _tuple0 _tmp46E;_tmp46E.f1=t1;_tmp46E.f2=t2;_tmp46E;});struct _tuple0 _tmp46B=_tmp46A;int _tmp46C;int _tmp46D;switch(*((int*)_tmp46B.f1)){case 7: switch(*((int*)_tmp46B.f2)){case 7: _LL3EF: _tmp46C=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp46B.f1)->f1;_tmp46D=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp46B.f2)->f1;_LL3F0:
# 2042
 return _tmp46D == 0  && _tmp46C != 0  || (_tmp46D == 1  && _tmp46C != 0) && _tmp46C != 1;case 6: _LL3F1: _LL3F2:
 goto _LL3F4;case 19: _LL3F3: _LL3F4:
 return 1;default: goto _LL40F;}case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46B.f1)->f2){case Cyc_Absyn_LongLong_sz: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46B.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46B.f2)->f2 == Cyc_Absyn_LongLong_sz){_LL3F5: _LL3F6:
 return 0;}else{goto _LL3F7;}}else{_LL3F7: _LL3F8:
 return 1;}case Cyc_Absyn_Long_sz: switch(*((int*)_tmp46B.f2)){case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46B.f2)->f2){case Cyc_Absyn_Int_sz: _LL3F9: _LL3FA:
# 2049
 goto _LL3FC;case Cyc_Absyn_Short_sz: _LL401: _LL402:
# 2054
 goto _LL404;case Cyc_Absyn_Char_sz: _LL407: _LL408:
# 2057
 goto _LL40A;default: goto _LL40F;}case 7: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp46B.f2)->f1 == 0){_LL3FD: _LL3FE:
# 2052
 goto _LL400;}else{goto _LL40F;}default: goto _LL40F;}case Cyc_Absyn_Int_sz: switch(*((int*)_tmp46B.f2)){case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46B.f2)->f2){case Cyc_Absyn_Long_sz: _LL3FB: _LL3FC:
# 2050
 return 0;case Cyc_Absyn_Short_sz: _LL403: _LL404:
# 2055
 goto _LL406;case Cyc_Absyn_Char_sz: _LL409: _LL40A:
# 2058
 goto _LL40C;default: goto _LL40F;}case 7: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp46B.f2)->f1 == 0){_LL3FF: _LL400:
# 2053
 goto _LL402;}else{goto _LL40F;}default: goto _LL40F;}case Cyc_Absyn_Short_sz: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46B.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46B.f2)->f2 == Cyc_Absyn_Char_sz){_LL40B: _LL40C:
# 2059
 goto _LL40E;}else{goto _LL40F;}}else{goto _LL40F;}default: goto _LL40F;}case 19: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46B.f2)->tag == 6)switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46B.f2)->f2){case Cyc_Absyn_Short_sz: _LL405: _LL406:
# 2056
 goto _LL408;case Cyc_Absyn_Char_sz: _LL40D: _LL40E:
# 2060
 return 1;default: goto _LL40F;}else{goto _LL40F;}default: _LL40F: _LL410:
# 2062
 return 0;}_LL3EE:;};}
# 2068
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
# 2071
struct _RegionHandle _tmp46F=_new_region("r");struct _RegionHandle*r=& _tmp46F;_push_region(r);{
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0  || 
Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
max_arith_type=({struct Cyc_Core_Opt*_tmp470=_region_malloc(r,sizeof(*_tmp470));_tmp470->v=t1;_tmp470;});}}}
# 2081
if(max_arith_type != 0){
if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v)){
int _tmp471=0;_npop_handler(0);return _tmp471;}}}
# 2086
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){
({struct Cyc_String_pa_PrintArg_struct _tmp475;_tmp475.tag=0;_tmp475.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp474;_tmp474.tag=0;_tmp474.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp472[2]={& _tmp474,& _tmp475};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)el->hd)->loc,({const char*_tmp473="type mismatch: expecting %s but found %s";_tag_dyneither(_tmp473,sizeof(char),41);}),_tag_dyneither(_tmp472,sizeof(void*),2));});});});{
int _tmp476=0;_npop_handler(0);return _tmp476;};}}}{
# 2092
int _tmp477=1;_npop_handler(0);return _tmp477;};
# 2071
;_pop_region(r);}
# 2097
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_typ(te,e)){
void*_tmp478=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp479=_tmp478;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp479)->tag == 5){_LL412: _LL413:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);goto _LL411;}else{_LL414: _LL415:
 return 0;}_LL411:;}
# 2103
return 1;}
# 2106
int Cyc_Tcutil_is_integral_type(void*t){
void*_tmp47A=Cyc_Tcutil_compress(t);void*_tmp47B=_tmp47A;switch(*((int*)_tmp47B)){case 6: _LL417: _LL418:
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
({void*_tmp47C=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp47D="integral size mismatch; conversion supplied";_tag_dyneither(_tmp47D,sizeof(char),44);}),_tag_dyneither(_tmp47C,sizeof(void*),0));});
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
({void*_tmp47E=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp47F="integral size mismatch; conversion supplied";_tag_dyneither(_tmp47F,sizeof(char),44);}),_tag_dyneither(_tmp47E,sizeof(void*),0));});
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
struct _tuple0 _tmp480=({struct _tuple0 _tmp499;_tmp499.f1=t1;_tmp499.f2=t2;_tmp499;});struct _tuple0 _tmp481=_tmp480;void*_tmp482;struct Cyc_Absyn_Tqual _tmp483;struct Cyc_Absyn_Exp*_tmp484;union Cyc_Absyn_Constraint*_tmp485;void*_tmp486;struct Cyc_Absyn_Tqual _tmp487;struct Cyc_Absyn_Exp*_tmp488;union Cyc_Absyn_Constraint*_tmp489;struct Cyc_Absyn_PtrInfo _tmp48A;struct Cyc_Absyn_PtrInfo _tmp48B;switch(*((int*)_tmp481.f1)){case 5: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp481.f2)->tag == 5){_LL422: _tmp48A=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp481.f1)->f1;_tmp48B=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp481.f2)->f1;_LL423: {
# 2160
int okay=1;
# 2162
if(!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp48A.ptr_atts).nullable,(_tmp48B.ptr_atts).nullable))
# 2165
okay=!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp48A.ptr_atts).nullable);
# 2167
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp48A.ptr_atts).bounds,(_tmp48B.ptr_atts).bounds)){
# 2170
struct _tuple0 _tmp48C=({struct _tuple0 _tmp493;_tmp493.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp48A.ptr_atts).bounds);_tmp493.f2=
((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp48B.ptr_atts).bounds);_tmp493;});
# 2170
struct _tuple0 _tmp48D=_tmp48C;struct Cyc_Absyn_Exp*_tmp48E;struct Cyc_Absyn_Exp*_tmp48F;struct Cyc_Absyn_Exp*_tmp490;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp48D.f1)->tag == 1){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp48D.f2)->tag == 0){_LL42B: _LL42C:
# 2172
 goto _LL42E;}else{_LL42F: _tmp48F=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp48D.f1)->f1;_tmp490=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp48D.f2)->f1;_LL430:
# 2175
 okay=okay  && Cyc_Evexp_lte_const_exp(_tmp490,_tmp48F);
# 2179
if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp48B.ptr_atts).zero_term))
({void*_tmp491=0;Cyc_Tcutil_warn(loc,({const char*_tmp492="implicit cast to shorter array";_tag_dyneither(_tmp492,sizeof(char),31);}),_tag_dyneither(_tmp491,sizeof(void*),0));});
goto _LL42A;}}else{if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp48D.f2)->tag == 0){_LL42D: _LL42E:
# 2173
 okay=1;goto _LL42A;}else{_LL431: _tmp48E=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp48D.f2)->f1;_LL432:
# 2184
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp48A.ptr_atts).zero_term) && 
Cyc_Tcutil_is_bound_one((_tmp48B.ptr_atts).bounds))
goto _LL42A;
okay=0;
goto _LL42A;}}_LL42A:;}
# 2193
okay=okay  && (!(_tmp48A.elt_tq).real_const  || (_tmp48B.elt_tq).real_const);
# 2196
if(!Cyc_Tcutil_unify((_tmp48A.ptr_atts).rgn,(_tmp48B.ptr_atts).rgn)){
if(Cyc_Tcenv_region_outlives(te,(_tmp48A.ptr_atts).rgn,(_tmp48B.ptr_atts).rgn)){
if(Cyc_Tcutil_warn_region_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp497;_tmp497.tag=0;_tmp497.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2201
Cyc_Absynpp_typ2string((_tmp48B.ptr_atts).rgn));({struct Cyc_String_pa_PrintArg_struct _tmp496;_tmp496.tag=0;_tmp496.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2200
Cyc_Absynpp_typ2string((_tmp48A.ptr_atts).rgn));({void*_tmp494[2]={& _tmp496,& _tmp497};Cyc_Tcutil_warn(loc,({const char*_tmp495="implicit cast form region %s to region %s";_tag_dyneither(_tmp495,sizeof(char),42);}),_tag_dyneither(_tmp494,sizeof(void*),2));});});});}else{
# 2202
okay=0;}}
# 2206
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp48A.ptr_atts).zero_term,(_tmp48B.ptr_atts).zero_term) || 
# 2209
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,(_tmp48A.ptr_atts).zero_term) && (_tmp48B.elt_tq).real_const);{
# 2217
int _tmp498=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp48B.ptr_atts).bounds,Cyc_Absyn_bounds_one_conref) && !
# 2220
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp48B.ptr_atts).zero_term);
# 2224
okay=okay  && (Cyc_Tcutil_unify(_tmp48A.elt_typ,_tmp48B.elt_typ) || 
(_tmp498  && ((_tmp48B.elt_tq).real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp48B.elt_typ)))) && Cyc_Tcutil_ptrsubtype(te,0,_tmp48A.elt_typ,_tmp48B.elt_typ));
# 2227
return okay;};}}else{goto _LL428;}case 8: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp481.f2)->tag == 8){_LL424: _tmp482=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp481.f1)->f1).elt_type;_tmp483=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp481.f1)->f1).tq;_tmp484=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp481.f1)->f1).num_elts;_tmp485=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp481.f1)->f1).zero_term;_tmp486=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp481.f2)->f1).elt_type;_tmp487=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp481.f2)->f1).tq;_tmp488=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp481.f2)->f1).num_elts;_tmp489=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp481.f2)->f1).zero_term;_LL425: {
# 2231
int okay;
# 2233
okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp485,_tmp489) && (
(_tmp484 != 0  && _tmp488 != 0) && Cyc_Evexp_same_const_exp(_tmp484,_tmp488));
# 2236
return(okay  && Cyc_Tcutil_unify(_tmp482,_tmp486)) && (!_tmp483.real_const  || _tmp487.real_const);}}else{goto _LL428;}case 19: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp481.f2)->tag == 6){_LL426: _LL427:
# 2238
 return 0;}else{goto _LL428;}default: _LL428: _LL429:
# 2240
 return Cyc_Tcutil_unify(t1,t2);}_LL421:;};}
# 2244
int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp49A=Cyc_Tcutil_compress(t);void*_tmp49B=_tmp49A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49B)->tag == 5){_LL434: _LL435:
 return 1;}else{_LL436: _LL437:
 return 0;}_LL433:;}
# 2250
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp49C=Cyc_Tcutil_compress(t);void*_tmp49D=_tmp49C;void*_tmp49E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49D)->tag == 5){_LL439: _tmp49E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49D)->f1).elt_typ;_LL43A:
 return _tmp49E;}else{_LL43B: _LL43C:
({void*_tmp49F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4A0="pointer_elt_type";_tag_dyneither(_tmp4A0,sizeof(char),17);}),_tag_dyneither(_tmp49F,sizeof(void*),0));});}_LL438:;}
# 2256
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp4A1=Cyc_Tcutil_compress(t);void*_tmp4A2=_tmp4A1;struct Cyc_Absyn_PtrAtts*_tmp4A3;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A2)->tag == 5){_LL43E: _tmp4A3=(struct Cyc_Absyn_PtrAtts*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A2)->f1).ptr_atts;_LL43F:
 return _tmp4A3->rgn;}else{_LL440: _LL441:
({void*_tmp4A4=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4A5="pointer_elt_type";_tag_dyneither(_tmp4A5,sizeof(char),17);}),_tag_dyneither(_tmp4A4,sizeof(void*),0));});}_LL43D:;}
# 2263
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp4A6=Cyc_Tcutil_compress(t);void*_tmp4A7=_tmp4A6;void*_tmp4A8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A7)->tag == 5){_LL443: _tmp4A8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A7)->f1).ptr_atts).rgn;_LL444:
# 2266
*rgn=_tmp4A8;
return 1;}else{_LL445: _LL446:
 return 0;}_LL442:;}
# 2273
int Cyc_Tcutil_is_boxed(void*t){
return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;}
# 2277
int Cyc_Tcutil_is_dyneither_ptr(void*t){
void*_tmp4A9=Cyc_Tcutil_compress(t);void*_tmp4AA=_tmp4A9;union Cyc_Absyn_Constraint*_tmp4AB;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AA)->tag == 5){_LL448: _tmp4AB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AA)->f1).ptr_atts).bounds;_LL449:
# 2280
 return((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp4AB)== (void*)& Cyc_Absyn_DynEither_b_val;}else{_LL44A: _LL44B:
 return 0;}_LL447:;}
# 2286
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp4AC=e->r;void*_tmp4AD=_tmp4AC;void*_tmp4AE;struct Cyc_Absyn_Exp*_tmp4AF;struct _dyneither_ptr _tmp4B0;switch(*((int*)_tmp4AD)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4AD)->f1).Wchar_c).tag){case 5: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4AD)->f1).Int_c).val).f2 == 0){_LL44D: _LL44E:
 goto _LL450;}else{goto _LL459;}case 2: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4AD)->f1).Char_c).val).f2 == 0){_LL44F: _LL450:
 goto _LL452;}else{goto _LL459;}case 4: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4AD)->f1).Short_c).val).f2 == 0){_LL451: _LL452:
 goto _LL454;}else{goto _LL459;}case 6: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4AD)->f1).LongLong_c).val).f2 == 0){_LL453: _LL454:
# 2292
 return 1;}else{goto _LL459;}case 3: _LL455: _tmp4B0=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4AD)->f1).Wchar_c).val;_LL456: {
# 2294
unsigned long _tmp4B1=Cyc_strlen((struct _dyneither_ptr)_tmp4B0);
int i=0;
if(_tmp4B1 >= 2  && *((const char*)_check_dyneither_subscript(_tmp4B0,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp4B0,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmp4B0,sizeof(char),1))== 'x'  && _tmp4B1 >= 3) && *((const char*)_check_dyneither_subscript(_tmp4B0,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp4B1;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp4B0,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 2304
return 0;}}default: goto _LL459;}case 13: _LL457: _tmp4AE=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4AD)->f1;_tmp4AF=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4AD)->f2;_LL458:
 return Cyc_Tcutil_is_zero(_tmp4AF) && Cyc_Tcutil_admits_zero(_tmp4AE);default: _LL459: _LL45A:
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
struct Cyc_Absyn_Kind*_tmp4B2=ka;enum Cyc_Absyn_KindQual _tmp4B3;enum Cyc_Absyn_AliasQual _tmp4B4;_LL45C: _tmp4B3=_tmp4B2->kind;_tmp4B4=_tmp4B2->aliasqual;_LL45D:;
{enum Cyc_Absyn_AliasQual _tmp4B5=_tmp4B4;switch(_tmp4B5){case Cyc_Absyn_Aliasable: _LL45F: _LL460: {
# 2348
enum Cyc_Absyn_KindQual _tmp4B6=_tmp4B3;switch(_tmp4B6){case Cyc_Absyn_AnyKind: _LL466: _LL467:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LL468: _LL469:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL46A: _LL46B:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL46C: _LL46D:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL46E: _LL46F:
 return& Cyc_Tcutil_eko;default: _LL470: _LL471:
 return& Cyc_Tcutil_iko;}_LL465:;}case Cyc_Absyn_Unique: _LL461: _LL462:
# 2357
{enum Cyc_Absyn_KindQual _tmp4B7=_tmp4B3;switch(_tmp4B7){case Cyc_Absyn_AnyKind: _LL473: _LL474:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL475: _LL476:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL477: _LL478:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL479: _LL47A:
 return& Cyc_Tcutil_urko;default: _LL47B: _LL47C:
 goto _LL472;}_LL472:;}
# 2364
goto _LL45E;default: _LL463: _LL464:
# 2366
{enum Cyc_Absyn_KindQual _tmp4B8=_tmp4B3;switch(_tmp4B8){case Cyc_Absyn_AnyKind: _LL47E: _LL47F:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL480: _LL481:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL482: _LL483:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL484: _LL485:
 return& Cyc_Tcutil_trko;default: _LL486: _LL487:
 goto _LL47D;}_LL47D:;}
# 2373
goto _LL45E;}_LL45E:;}
# 2375
({struct Cyc_String_pa_PrintArg_struct _tmp4BB;_tmp4BB.tag=0;_tmp4BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka));({void*_tmp4B9[1]={& _tmp4BB};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4BA="kind_to_opt: bad kind %s\n";_tag_dyneither(_tmp4BA,sizeof(char),26);}),_tag_dyneither(_tmp4B9,sizeof(void*),1));});});}
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
struct Cyc_Absyn_Kind*_tmp4BC=ka;enum Cyc_Absyn_KindQual _tmp4BD;enum Cyc_Absyn_AliasQual _tmp4BE;_LL489: _tmp4BD=_tmp4BC->kind;_tmp4BE=_tmp4BC->aliasqual;_LL48A:;
{enum Cyc_Absyn_AliasQual _tmp4BF=_tmp4BE;switch(_tmp4BF){case Cyc_Absyn_Aliasable: _LL48C: _LL48D: {
# 2416
enum Cyc_Absyn_KindQual _tmp4C0=_tmp4BD;switch(_tmp4C0){case Cyc_Absyn_AnyKind: _LL493: _LL494:
 return& ab;case Cyc_Absyn_MemKind: _LL495: _LL496:
 return& mb;case Cyc_Absyn_BoxKind: _LL497: _LL498:
 return& bb;case Cyc_Absyn_RgnKind: _LL499: _LL49A:
 return& rb;case Cyc_Absyn_EffKind: _LL49B: _LL49C:
 return& eb;default: _LL49D: _LL49E:
 return& ib;}_LL492:;}case Cyc_Absyn_Unique: _LL48E: _LL48F:
# 2425
{enum Cyc_Absyn_KindQual _tmp4C1=_tmp4BD;switch(_tmp4C1){case Cyc_Absyn_AnyKind: _LL4A0: _LL4A1:
 return& uab;case Cyc_Absyn_MemKind: _LL4A2: _LL4A3:
 return& umb;case Cyc_Absyn_BoxKind: _LL4A4: _LL4A5:
 return& ubb;case Cyc_Absyn_RgnKind: _LL4A6: _LL4A7:
 return& urb;default: _LL4A8: _LL4A9:
 goto _LL49F;}_LL49F:;}
# 2432
goto _LL48B;default: _LL490: _LL491:
# 2434
{enum Cyc_Absyn_KindQual _tmp4C2=_tmp4BD;switch(_tmp4C2){case Cyc_Absyn_AnyKind: _LL4AB: _LL4AC:
 return& tab;case Cyc_Absyn_MemKind: _LL4AD: _LL4AE:
 return& tmb;case Cyc_Absyn_BoxKind: _LL4AF: _LL4B0:
 return& tbb;case Cyc_Absyn_RgnKind: _LL4B1: _LL4B2:
 return& trb;default: _LL4B3: _LL4B4:
 goto _LL4AA;}_LL4AA:;}
# 2441
goto _LL48B;}_LL48B:;}
# 2443
({struct Cyc_String_pa_PrintArg_struct _tmp4C5;_tmp4C5.tag=0;_tmp4C5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka));({void*_tmp4C3[1]={& _tmp4C5};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4C4="kind_to_b: bad kind %s\n";_tag_dyneither(_tmp4C4,sizeof(char),24);}),_tag_dyneither(_tmp4C3,sizeof(void*),1));});});}
# 2446
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return*Cyc_Tcutil_kind_to_b(k);}
# 2449
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
# 2451
return({struct Cyc_Core_Opt*_tmp4D4=_cycalloc(sizeof(*_tmp4D4));_tmp4D4->v=Cyc_Tcutil_kind_to_bound(k);_tmp4D4;});}
# 2456
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){
e1->r=(void*)({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4D5=_cycalloc(sizeof(*_tmp4D5));_tmp4D5[0]=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp4D6;_tmp4D6.tag=0;_tmp4D6.f1=Cyc_Absyn_Null_c;_tmp4D6;});_tmp4D5;});{
struct Cyc_Core_Opt*_tmp4D7=Cyc_Tcenv_lookup_opt_type_vars(te);
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4D8=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4DD=_cycalloc(sizeof(*_tmp4DD));_tmp4DD[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp4DE;_tmp4DE.tag=5;_tmp4DE.f1=({struct Cyc_Absyn_PtrInfo _tmp4DF;_tmp4DF.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp4D7);_tmp4DF.elt_tq=
Cyc_Absyn_empty_tqual(0);_tmp4DF.ptr_atts=({(_tmp4DF.ptr_atts).rgn=
Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp4D7);(_tmp4DF.ptr_atts).nullable=Cyc_Absyn_true_conref;(_tmp4DF.ptr_atts).bounds=
# 2464
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp4DF.ptr_atts).zero_term=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp4DF.ptr_atts).ptrloc=0;_tmp4DF.ptr_atts;});_tmp4DF;});_tmp4DE;});_tmp4DD;});
e1->topt=(void*)_tmp4D8;{
int bogus=0;
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
if(bogus != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp4DC;_tmp4DC.tag=0;_tmp4DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(e1->loc));({struct Cyc_String_pa_PrintArg_struct _tmp4DB;_tmp4DB.tag=0;_tmp4DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));({void*_tmp4D9[2]={& _tmp4DB,& _tmp4DC};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4DA="zero_to_null resulted in an alias coercion on %s at %s\n";_tag_dyneither(_tmp4DA,sizeof(char),56);}),_tag_dyneither(_tmp4D9,sizeof(void*),2));});});});
return retv;};};}
# 2474
return 0;}
# 2477
struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
enum Cyc_Absyn_Coercion _tmp4E0=c;switch(_tmp4E0){case Cyc_Absyn_Unknown_coercion: _LL4B6: _LL4B7:
 return({const char*_tmp4E1="unknown";_tag_dyneither(_tmp4E1,sizeof(char),8);});case Cyc_Absyn_No_coercion: _LL4B8: _LL4B9:
 return({const char*_tmp4E2="no coercion";_tag_dyneither(_tmp4E2,sizeof(char),12);});case Cyc_Absyn_NonNull_to_Null: _LL4BA: _LL4BB:
 return({const char*_tmp4E3="null check";_tag_dyneither(_tmp4E3,sizeof(char),11);});default: _LL4BC: _LL4BD:
 return({const char*_tmp4E4="other coercion";_tag_dyneither(_tmp4E4,sizeof(char),15);});}_LL4B5:;}
# 2486
int Cyc_Tcutil_warn_alias_coerce=0;
# 2492
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0,& Cyc_Tcutil_rk};
# 2496
static int counter=0;
struct _tuple2*v=({struct _tuple2*_tmp4F8=_cycalloc(sizeof(*_tmp4F8));_tmp4F8->f1=Cyc_Absyn_Loc_n;_tmp4F8->f2=({struct _dyneither_ptr*_tmp4F9=_cycalloc(sizeof(*_tmp4F9));_tmp4F9[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp4FC;_tmp4FC.tag=1;_tmp4FC.f1=(unsigned long)counter ++;({void*_tmp4FA[1]={& _tmp4FC};Cyc_aprintf(({const char*_tmp4FB="__aliasvar%d";_tag_dyneither(_tmp4FB,sizeof(char),13);}),_tag_dyneither(_tmp4FA,sizeof(void*),1));});});_tmp4F9;});_tmp4F8;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(v,e_typ,e);
struct Cyc_Absyn_Exp*ve=Cyc_Absyn_varb_exp(v,(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4F6=_cycalloc(sizeof(*_tmp4F6));_tmp4F6[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp4F7;_tmp4F7.tag=4;_tmp4F7.f1=vd;_tmp4F7;});_tmp4F6;}),e->loc);
# 2505
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 2507
{void*_tmp4E5=Cyc_Tcutil_compress(e_typ);void*_tmp4E6=_tmp4E5;void*_tmp4E7;struct Cyc_Absyn_Tqual _tmp4E8;void*_tmp4E9;union Cyc_Absyn_Constraint*_tmp4EA;union Cyc_Absyn_Constraint*_tmp4EB;union Cyc_Absyn_Constraint*_tmp4EC;struct Cyc_Absyn_PtrLoc*_tmp4ED;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E6)->tag == 5){_LL4BF: _tmp4E7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E6)->f1).elt_typ;_tmp4E8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E6)->f1).elt_tq;_tmp4E9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E6)->f1).ptr_atts).rgn;_tmp4EA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E6)->f1).ptr_atts).nullable;_tmp4EB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E6)->f1).ptr_atts).bounds;_tmp4EC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E6)->f1).ptr_atts).zero_term;_tmp4ED=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E6)->f1).ptr_atts).ptrloc;_LL4C0:
# 2509
{void*_tmp4EE=Cyc_Tcutil_compress(_tmp4E9);void*_tmp4EF=_tmp4EE;void**_tmp4F0;struct Cyc_Core_Opt*_tmp4F1;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4EF)->tag == 1){_LL4C4: _tmp4F0=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4EF)->f2;_tmp4F1=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4EF)->f4;_LL4C5: {
# 2511
struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4F2=({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4F3=_cycalloc(sizeof(*_tmp4F3));_tmp4F3[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4F4;_tmp4F4.tag=2;_tmp4F4.f1=tv;_tmp4F4;});_tmp4F3;});
*_tmp4F0=(void*)_tmp4F2;
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
return({struct _tuple15 _tmp4F5;_tmp4F5.f1=d;_tmp4F5.f2=ve;_tmp4F5;});};}
# 2531
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ,void*wants_typ,unsigned int loc){
# 2534
struct _RegionHandle _tmp4FE=_new_region("r");struct _RegionHandle*r=& _tmp4FE;_push_region(r);
if((Cyc_Tcutil_is_noalias_path(r,e) && Cyc_Tcutil_is_noalias_pointer(e_typ,0)) && 
Cyc_Tcutil_is_pointer_type(e_typ)){
# 2539
void*_tmp4FF=Cyc_Tcutil_compress(wants_typ);void*_tmp500=_tmp4FF;void*_tmp501;switch(*((int*)_tmp500)){case 5: _LL4C9: _tmp501=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp500)->f1).ptr_atts).rgn;_LL4CA: {
# 2541
void*_tmp502=Cyc_Tcutil_compress(_tmp501);void*_tmp503=_tmp502;if(((struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp503)->tag == 20){_LL4D0: _LL4D1: {
int _tmp504=0;_npop_handler(0);return _tmp504;}}else{_LL4D2: _LL4D3: {
# 2544
struct Cyc_Absyn_Kind*_tmp505=Cyc_Tcutil_typ_kind(_tmp501);
int _tmp506=_tmp505->kind == Cyc_Absyn_RgnKind  && _tmp505->aliasqual == Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp506;}}_LL4CF:;}case 17: _LL4CB: _LL4CC:
# 2548
({struct Cyc_String_pa_PrintArg_struct _tmp50A;_tmp50A.tag=0;_tmp50A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(loc));({struct Cyc_String_pa_PrintArg_struct _tmp509;_tmp509.tag=0;_tmp509.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_compress(wants_typ)));({void*_tmp507[2]={& _tmp509,& _tmp50A};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp508="got typedef %s in can_insert_alias at %s\n";_tag_dyneither(_tmp508,sizeof(char),42);}),_tag_dyneither(_tmp507,sizeof(void*),2));});});});default: _LL4CD: _LL4CE: {
int _tmp50B=0;_npop_handler(0);return _tmp50B;}}_LL4C8:;}{
# 2553
int _tmp50C=0;_npop_handler(0);return _tmp50C;};
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
({struct Cyc_String_pa_PrintArg_struct _tmp510;_tmp510.tag=0;_tmp510.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_PrintArg_struct _tmp50F;_tmp50F.tag=0;_tmp50F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));({void*_tmp50D[2]={& _tmp50F,& _tmp510};Cyc_Tcutil_warn(e->loc,({const char*_tmp50E="integral size mismatch; %s -> %s conversion supplied";_tag_dyneither(_tmp50E,sizeof(char),53);}),_tag_dyneither(_tmp50D,sizeof(void*),2));});});});
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
return 1;}else{
# 2575
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp515;_tmp515.tag=0;_tmp515.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_PrintArg_struct _tmp514;_tmp514.tag=0;_tmp514.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));({struct Cyc_String_pa_PrintArg_struct _tmp513;_tmp513.tag=0;_tmp513.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp511[3]={& _tmp513,& _tmp514,& _tmp515};Cyc_Tcutil_warn(e->loc,({const char*_tmp512="implicit alias coercion for %s:%s to %s";_tag_dyneither(_tmp512,sizeof(char),40);}),_tag_dyneither(_tmp511,sizeof(void*),3));});});});});
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
({struct Cyc_String_pa_PrintArg_struct _tmp519;_tmp519.tag=0;_tmp519.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_PrintArg_struct _tmp518;_tmp518.tag=0;_tmp518.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));({void*_tmp516[2]={& _tmp518,& _tmp519};Cyc_Tcutil_warn(e->loc,({const char*_tmp517="implicit cast from %s to %s";_tag_dyneither(_tmp517,sizeof(char),28);}),_tag_dyneither(_tmp516,sizeof(void*),2));});});});
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
void*_tmp51A=Cyc_Tcutil_compress(t);void*_tmp51B=_tmp51A;switch(*((int*)_tmp51B)){case 6: _LL4D5: _LL4D6:
 goto _LL4D8;case 7: _LL4D7: _LL4D8:
 return 1;default: _LL4D9: _LL4DA:
 return 0;}_LL4D4:;}
# 2628 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 2632
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ_f(struct _tuple23*env,struct Cyc_Absyn_Aggrfield*x){
# 2635
struct _tuple23 _tmp51C=*env;struct _tuple23 _tmp51D=_tmp51C;struct Cyc_List_List*_tmp51E;struct _RegionHandle*_tmp51F;struct Cyc_Tcenv_Tenv*_tmp520;int _tmp521;_LL4DC: _tmp51E=_tmp51D.f1;_tmp51F=_tmp51D.f2;_tmp520=_tmp51D.f3;_tmp521=_tmp51D.f4;_LL4DD:;{
# 2637
void*_tmp522=_tmp51E == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp51F,_tmp51E,x->type);
struct Cyc_List_List*_tmp523=Cyc_Tcutil_flatten_typ(_tmp51F,_tmp521,_tmp520,_tmp522);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp523)== 1)
return({struct Cyc_List_List*_tmp524=_region_malloc(_tmp51F,sizeof(*_tmp524));_tmp524->hd=({struct _tuple12*_tmp525=_region_malloc(_tmp51F,sizeof(*_tmp525));_tmp525->f1=x->tq;_tmp525->f2=_tmp522;_tmp525;});_tmp524->tl=0;_tmp524;});else{
return _tmp523;}};}struct _tuple24{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2643
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple24*env,struct _tuple12*x){
# 2645
struct _tuple24 _tmp526=*env;struct _tuple24 _tmp527=_tmp526;struct _RegionHandle*_tmp528;struct Cyc_Tcenv_Tenv*_tmp529;int _tmp52A;_LL4DF: _tmp528=_tmp527.f1;_tmp529=_tmp527.f2;_tmp52A=_tmp527.f3;_LL4E0:;{
struct _tuple12 _tmp52B=*x;struct _tuple12 _tmp52C=_tmp52B;struct Cyc_Absyn_Tqual _tmp52D;void*_tmp52E;_LL4E2: _tmp52D=_tmp52C.f1;_tmp52E=_tmp52C.f2;_LL4E3:;{
struct Cyc_List_List*_tmp52F=Cyc_Tcutil_flatten_typ(_tmp528,_tmp52A,_tmp529,_tmp52E);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp52F)== 1)
return({struct Cyc_List_List*_tmp530=_region_malloc(_tmp528,sizeof(*_tmp530));_tmp530->hd=({struct _tuple12*_tmp531=_region_malloc(_tmp528,sizeof(*_tmp531));_tmp531->f1=_tmp52D;_tmp531->f2=_tmp52E;_tmp531;});_tmp530->tl=0;_tmp530;});else{
return _tmp52F;}};};}
# 2652
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2656
if(flatten){
t1=Cyc_Tcutil_compress(t1);{
void*_tmp532=t1;struct Cyc_List_List*_tmp533;struct Cyc_Absyn_Aggrdecl*_tmp534;struct Cyc_List_List*_tmp535;struct Cyc_List_List*_tmp536;switch(*((int*)_tmp532)){case 0: _LL4E5: _LL4E6:
 return 0;case 10: _LL4E7: _tmp536=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp532)->f1;_LL4E8: {
# 2661
struct _tuple24 _tmp537=({struct _tuple24 _tmp540;_tmp540.f1=r;_tmp540.f2=te;_tmp540.f3=flatten;_tmp540;});
# 2663
struct Cyc_List_List*_tmp538=_tmp536;struct _tuple12*_tmp539;struct Cyc_List_List*_tmp53A;if(_tmp538 == 0){_LL4F0: _LL4F1:
 return 0;}else{_LL4F2: _tmp539=(struct _tuple12*)_tmp538->hd;_tmp53A=_tmp538->tl;_LL4F3: {
# 2666
struct Cyc_List_List*_tmp53B=Cyc_Tcutil_rcopy_tqt(& _tmp537,_tmp539);
_tmp537.f3=0;{
struct Cyc_List_List*_tmp53C=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple12*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp537,_tmp536);
struct Cyc_List_List*_tmp53D=({struct Cyc_List_List*_tmp53F=_region_malloc(r,sizeof(*_tmp53F));_tmp53F->hd=_tmp53B;_tmp53F->tl=_tmp53C;_tmp53F;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,({struct Cyc_List_List*_tmp53E=_region_malloc(r,sizeof(*_tmp53E));_tmp53E->hd=_tmp53B;_tmp53E->tl=_tmp53C;_tmp53E;}));};}}_LL4EF:;}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp532)->f1).aggr_info).KnownAggr).tag == 2){_LL4E9: _tmp534=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp532)->f1).aggr_info).KnownAggr).val;_tmp535=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp532)->f1).targs;_LL4EA:
# 2674
 if(((_tmp534->kind == Cyc_Absyn_UnionA  || _tmp534->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp534->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp534->impl))->rgn_po != 0)
# 2676
return({struct Cyc_List_List*_tmp541=_region_malloc(r,sizeof(*_tmp541));_tmp541->hd=({struct _tuple12*_tmp542=_region_malloc(r,sizeof(*_tmp542));_tmp542->f1=Cyc_Absyn_empty_tqual(0);_tmp542->f2=t1;_tmp542;});_tmp541->tl=0;_tmp541;});{
struct Cyc_List_List*_tmp543=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp534->tvs,_tmp535);
struct _tuple23 env=({struct _tuple23 _tmp54C;_tmp54C.f1=_tmp543;_tmp54C.f2=r;_tmp54C.f3=te;_tmp54C.f4=flatten;_tmp54C;});
struct Cyc_List_List*_tmp544=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp534->impl))->fields;struct Cyc_List_List*_tmp545=_tmp544;struct Cyc_Absyn_Aggrfield*_tmp546;struct Cyc_List_List*_tmp547;if(_tmp545 == 0){_LL4F5: _LL4F6:
 return 0;}else{_LL4F7: _tmp546=(struct Cyc_Absyn_Aggrfield*)_tmp545->hd;_tmp547=_tmp545->tl;_LL4F8: {
# 2682
struct Cyc_List_List*_tmp548=Cyc_Tcutil_flatten_typ_f(& env,_tmp546);
env.f4=0;{
struct Cyc_List_List*_tmp549=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp547);
struct Cyc_List_List*_tmp54A=({struct Cyc_List_List*_tmp54B=_region_malloc(r,sizeof(*_tmp54B));_tmp54B->hd=_tmp548;_tmp54B->tl=_tmp549;_tmp54B;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp54A);};}}_LL4F4:;};}else{goto _LL4ED;}case 12: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp532)->f1 == Cyc_Absyn_StructA){_LL4EB: _tmp533=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp532)->f2;_LL4EC: {
# 2689
struct _tuple23 env=({struct _tuple23 _tmp554;_tmp554.f1=0;_tmp554.f2=r;_tmp554.f3=te;_tmp554.f4=flatten;_tmp554;});
struct Cyc_List_List*_tmp54D=_tmp533;struct Cyc_Absyn_Aggrfield*_tmp54E;struct Cyc_List_List*_tmp54F;if(_tmp54D == 0){_LL4FA: _LL4FB:
 return 0;}else{_LL4FC: _tmp54E=(struct Cyc_Absyn_Aggrfield*)_tmp54D->hd;_tmp54F=_tmp54D->tl;_LL4FD: {
# 2693
struct Cyc_List_List*_tmp550=Cyc_Tcutil_flatten_typ_f(& env,_tmp54E);
env.f4=0;{
struct Cyc_List_List*_tmp551=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp54F);
struct Cyc_List_List*_tmp552=({struct Cyc_List_List*_tmp553=_region_malloc(r,sizeof(*_tmp553));_tmp553->hd=_tmp550;_tmp553->tl=_tmp551;_tmp553;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp552);};}}_LL4F9:;}}else{goto _LL4ED;}default: _LL4ED: _LL4EE:
# 2699
 goto _LL4E4;}_LL4E4:;};}
# 2702
return({struct Cyc_List_List*_tmp555=_region_malloc(r,sizeof(*_tmp555));_tmp555->hd=({struct _tuple12*_tmp556=_region_malloc(r,sizeof(*_tmp556));_tmp556->f1=Cyc_Absyn_empty_tqual(0);_tmp556->f2=t1;_tmp556;});_tmp555->tl=0;_tmp555;});}
# 2706
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp557=(void*)t->hd;void*_tmp558=_tmp557;switch(*((int*)_tmp558)){case 23: _LL4FF: _LL500:
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
struct _tuple0 _tmp559=({struct _tuple0 _tmp55D;_tmp55D.f1=Cyc_Tcutil_compress(t1);_tmp55D.f2=Cyc_Tcutil_compress(t2);_tmp55D;});struct _tuple0 _tmp55A=_tmp559;enum Cyc_Absyn_Size_of _tmp55B;enum Cyc_Absyn_Size_of _tmp55C;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp55A.f1)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp55A.f2)->tag == 6){_LL508: _tmp55B=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp55A.f1)->f2;_tmp55C=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp55A.f2)->f2;_LL509:
# 2726
 return(_tmp55B == _tmp55C  || _tmp55B == Cyc_Absyn_Int_sz  && _tmp55C == Cyc_Absyn_Long_sz) || 
_tmp55B == Cyc_Absyn_Long_sz  && _tmp55C == Cyc_Absyn_Int_sz;}else{goto _LL50A;}}else{_LL50A: _LL50B:
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
struct _tuple0 _tmp55E=({struct _tuple0 _tmp594;_tmp594.f1=t1;_tmp594.f2=t2;_tmp594;});struct _tuple0 _tmp55F=_tmp55E;struct Cyc_Absyn_FnInfo _tmp560;struct Cyc_Absyn_FnInfo _tmp561;struct Cyc_Absyn_Datatypedecl*_tmp562;struct Cyc_Absyn_Datatypefield*_tmp563;struct Cyc_List_List*_tmp564;struct Cyc_Absyn_Datatypedecl*_tmp565;struct Cyc_List_List*_tmp566;void*_tmp567;struct Cyc_Absyn_Tqual _tmp568;void*_tmp569;union Cyc_Absyn_Constraint*_tmp56A;union Cyc_Absyn_Constraint*_tmp56B;union Cyc_Absyn_Constraint*_tmp56C;void*_tmp56D;struct Cyc_Absyn_Tqual _tmp56E;void*_tmp56F;union Cyc_Absyn_Constraint*_tmp570;union Cyc_Absyn_Constraint*_tmp571;union Cyc_Absyn_Constraint*_tmp572;switch(*((int*)_tmp55F.f1)){case 5: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55F.f2)->tag == 5){_LL50D: _tmp567=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55F.f1)->f1).elt_typ;_tmp568=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55F.f1)->f1).elt_tq;_tmp569=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55F.f1)->f1).ptr_atts).rgn;_tmp56A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55F.f1)->f1).ptr_atts).nullable;_tmp56B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55F.f1)->f1).ptr_atts).bounds;_tmp56C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55F.f1)->f1).ptr_atts).zero_term;_tmp56D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55F.f2)->f1).elt_typ;_tmp56E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55F.f2)->f1).elt_tq;_tmp56F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55F.f2)->f1).ptr_atts).rgn;_tmp570=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55F.f2)->f1).ptr_atts).nullable;_tmp571=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55F.f2)->f1).ptr_atts).bounds;_tmp572=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55F.f2)->f1).ptr_atts).zero_term;_LL50E:
# 2750
 if(_tmp568.real_const  && !_tmp56E.real_const)
return 0;
# 2753
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp56A,_tmp570) && 
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp56A)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp570))
return 0;
# 2757
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp56C,_tmp572) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp56C)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp572))
return 0;
# 2761
if((!Cyc_Tcutil_unify(_tmp569,_tmp56F) && !Cyc_Tcenv_region_outlives(te,_tmp569,_tmp56F)) && !
Cyc_Tcutil_subtype(te,assume,_tmp569,_tmp56F))
return 0;
# 2765
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp56B,_tmp571)){
struct _tuple0 _tmp573=({struct _tuple0 _tmp577;_tmp577.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp56B);_tmp577.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp571);_tmp577;});struct _tuple0 _tmp574=_tmp573;struct Cyc_Absyn_Exp*_tmp575;struct Cyc_Absyn_Exp*_tmp576;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp574.f1)->tag == 0){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp574.f2)->tag == 0){_LL516: _LL517:
 goto _LL515;}else{goto _LL51A;}}else{if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp574.f2)->tag == 1){_LL518: _tmp575=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp574.f1)->f1;_tmp576=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp574.f2)->f1;_LL519:
# 2770
 if(!Cyc_Evexp_lte_const_exp(_tmp576,_tmp575))
return 0;
goto _LL515;}else{_LL51A: _LL51B:
 return 0;}}_LL515:;}
# 2778
if(!_tmp56E.real_const  && _tmp568.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp56D)))
return 0;}{
# 2784
int _tmp578=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp571,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp572);
# 2790
return(_tmp578  && Cyc_Tcutil_ptrsubtype(te,({struct Cyc_List_List*_tmp579=_cycalloc(sizeof(*_tmp579));_tmp579->hd=({struct _tuple0*_tmp57A=_cycalloc(sizeof(*_tmp57A));_tmp57A->f1=t1;_tmp57A->f2=t2;_tmp57A;});_tmp579->tl=assume;_tmp579;}),_tmp567,_tmp56D) || Cyc_Tcutil_unify(_tmp567,_tmp56D)) || Cyc_Tcutil_isomorphic(_tmp567,_tmp56D);};}else{goto _LL513;}case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp55F.f1)->f1).field_info).KnownDatatypefield).tag == 2){if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp55F.f2)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp55F.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL50F: _tmp562=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp55F.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp563=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp55F.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp564=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp55F.f1)->f1).targs;_tmp565=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp55F.f2)->f1).datatype_info).KnownDatatype).val;_tmp566=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp55F.f2)->f1).targs;_LL510:
# 2796
 if(_tmp562 != _tmp565  && Cyc_Absyn_qvar_cmp(_tmp562->name,_tmp565->name)!= 0)return 0;
# 2798
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp564)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp566))return 0;
for(0;_tmp564 != 0;(_tmp564=_tmp564->tl,_tmp566=_tmp566->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp564->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp566))->hd))return 0;}
return 1;}else{goto _LL513;}}else{goto _LL513;}}else{goto _LL513;}case 9: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp55F.f2)->tag == 9){_LL511: _tmp560=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp55F.f1)->f1;_tmp561=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp55F.f2)->f1;_LL512:
# 2805
 if(_tmp560.tvars != 0  || _tmp561.tvars != 0){
struct Cyc_List_List*_tmp57B=_tmp560.tvars;
struct Cyc_List_List*_tmp57C=_tmp561.tvars;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp57B)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp57C))return 0;{
struct _RegionHandle _tmp57D=_new_region("r");struct _RegionHandle*r=& _tmp57D;_push_region(r);
{struct Cyc_List_List*inst=0;
while(_tmp57B != 0){
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp57B->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp57C))->hd)->kind)){int _tmp57E=0;_npop_handler(0);return _tmp57E;}
inst=({struct Cyc_List_List*_tmp57F=_region_malloc(r,sizeof(*_tmp57F));_tmp57F->hd=({struct _tuple16*_tmp580=_region_malloc(r,sizeof(*_tmp580));_tmp580->f1=(struct Cyc_Absyn_Tvar*)_tmp57C->hd;_tmp580->f2=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp581=_cycalloc(sizeof(*_tmp581));_tmp581[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp582;_tmp582.tag=2;_tmp582.f1=(struct Cyc_Absyn_Tvar*)_tmp57B->hd;_tmp582;});_tmp581;});_tmp580;});_tmp57F->tl=inst;_tmp57F;});
_tmp57B=_tmp57B->tl;
_tmp57C=_tmp57C->tl;}
# 2817
if(inst != 0){
_tmp560.tvars=0;
_tmp561.tvars=0;{
int _tmp587=Cyc_Tcutil_subtype(te,assume,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp583=_cycalloc(sizeof(*_tmp583));_tmp583[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp584;_tmp584.tag=9;_tmp584.f1=_tmp560;_tmp584;});_tmp583;}),(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp585=_cycalloc(sizeof(*_tmp585));_tmp585[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp586;_tmp586.tag=9;_tmp586.f1=_tmp561;_tmp586;});_tmp585;}));_npop_handler(0);return _tmp587;};}}
# 2810
;_pop_region(r);};}
# 2825
if(!Cyc_Tcutil_subtype(te,assume,_tmp560.ret_typ,_tmp561.ret_typ))return 0;{
struct Cyc_List_List*_tmp588=_tmp560.args;
struct Cyc_List_List*_tmp589=_tmp561.args;
# 2830
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp588)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp589))return 0;
# 2832
for(0;_tmp588 != 0;(_tmp588=_tmp588->tl,_tmp589=_tmp589->tl)){
struct _tuple10 _tmp58A=*((struct _tuple10*)_tmp588->hd);struct _tuple10 _tmp58B=_tmp58A;struct Cyc_Absyn_Tqual _tmp58C;void*_tmp58D;_LL51D: _tmp58C=_tmp58B.f2;_tmp58D=_tmp58B.f3;_LL51E:;{
struct _tuple10 _tmp58E=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp589))->hd);struct _tuple10 _tmp58F=_tmp58E;struct Cyc_Absyn_Tqual _tmp590;void*_tmp591;_LL520: _tmp590=_tmp58F.f2;_tmp591=_tmp58F.f3;_LL521:;
# 2836
if(_tmp590.real_const  && !_tmp58C.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp591,_tmp58D))
return 0;};}
# 2840
if(_tmp560.c_varargs != _tmp561.c_varargs)return 0;
if(_tmp560.cyc_varargs != 0  && _tmp561.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp592=*_tmp560.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp593=*_tmp561.cyc_varargs;
# 2845
if((_tmp593.tq).real_const  && !(_tmp592.tq).real_const  || !
Cyc_Tcutil_subtype(te,assume,_tmp593.type,_tmp592.type))
return 0;}else{
if(_tmp560.cyc_varargs != 0  || _tmp561.cyc_varargs != 0)return 0;}
# 2850
if(!Cyc_Tcutil_subset_effect(1,(void*)_check_null(_tmp560.effect),(void*)_check_null(_tmp561.effect)))return 0;
# 2852
if(!Cyc_Tcutil_sub_rgnpo(_tmp560.rgn_po,_tmp561.rgn_po))return 0;
# 2854
if(!Cyc_Tcutil_sub_attributes(_tmp560.attributes,_tmp561.attributes))return 0;
# 2856
if(!Cyc_Tcutil_check_logical_implication(_tmp561.requires_relns,_tmp560.requires_relns))
return 0;
# 2859
if(!Cyc_Tcutil_check_logical_implication(_tmp560.ensures_relns,_tmp561.ensures_relns))
return 0;
# 2862
return 1;};}else{goto _LL513;}default: _LL513: _LL514:
 return 0;}_LL50C:;};}
# 2874 "tcutil.cyc"
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2876
struct _RegionHandle _tmp595=_new_region("temp");struct _RegionHandle*temp=& _tmp595;_push_region(temp);
{struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(temp,1,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(temp,1,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0){int _tmp596=0;_npop_handler(0);return _tmp596;}{
struct _tuple12*_tmp597=(struct _tuple12*)tqs1->hd;struct _tuple12*_tmp598=_tmp597;struct Cyc_Absyn_Tqual _tmp599;void*_tmp59A;_LL523: _tmp599=_tmp598->f1;_tmp59A=_tmp598->f2;_LL524:;{
struct _tuple12*_tmp59B=(struct _tuple12*)tqs2->hd;struct _tuple12*_tmp59C=_tmp59B;struct Cyc_Absyn_Tqual _tmp59D;void*_tmp59E;_LL526: _tmp59D=_tmp59C->f1;_tmp59E=_tmp59C->f2;_LL527:;
# 2884
if(_tmp599.real_const  && !_tmp59D.real_const){int _tmp59F=0;_npop_handler(0);return _tmp59F;}
# 2886
if((_tmp59D.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp59E))) && 
Cyc_Tcutil_subtype(te,assume,_tmp59A,_tmp59E))
# 2889
continue;
# 2891
if(Cyc_Tcutil_unify(_tmp59A,_tmp59E))
# 2893
continue;
# 2895
if(Cyc_Tcutil_isomorphic(_tmp59A,_tmp59E))
# 2897
continue;{
# 2900
int _tmp5A0=0;_npop_handler(0);return _tmp5A0;};};};}{
# 2902
int _tmp5A1=1;_npop_handler(0);return _tmp5A1;};}
# 2877
;_pop_region(temp);}
# 2906
static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp5A2=Cyc_Tcutil_compress(t);void*_tmp5A3=_tmp5A2;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A3)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A3)->f2 == Cyc_Absyn_Char_sz){_LL529: _LL52A:
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
{void*_tmp5A4=t2;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A4)->tag == 6)switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A4)->f2){case Cyc_Absyn_Int_sz: _LL52E: _LL52F:
# 2925
 goto _LL531;case Cyc_Absyn_Long_sz: _LL530: _LL531:
# 2927
 if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL52D;default: goto _LL532;}else{_LL532: _LL533:
 goto _LL52D;}_LL52D:;}{
# 2931
void*_tmp5A5=t1;void*_tmp5A6;struct Cyc_Absyn_Enumdecl*_tmp5A7;void*_tmp5A8;struct Cyc_Absyn_Tqual _tmp5A9;struct Cyc_Absyn_Exp*_tmp5AA;union Cyc_Absyn_Constraint*_tmp5AB;void*_tmp5AC;struct Cyc_Absyn_Tqual _tmp5AD;void*_tmp5AE;union Cyc_Absyn_Constraint*_tmp5AF;union Cyc_Absyn_Constraint*_tmp5B0;union Cyc_Absyn_Constraint*_tmp5B1;switch(*((int*)_tmp5A5)){case 5: _LL535: _tmp5AC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A5)->f1).elt_typ;_tmp5AD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A5)->f1).elt_tq;_tmp5AE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A5)->f1).ptr_atts).rgn;_tmp5AF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A5)->f1).ptr_atts).nullable;_tmp5B0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A5)->f1).ptr_atts).bounds;_tmp5B1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A5)->f1).ptr_atts).zero_term;_LL536:
# 2939
{void*_tmp5B2=t2;void*_tmp5B3;struct Cyc_Absyn_Tqual _tmp5B4;void*_tmp5B5;union Cyc_Absyn_Constraint*_tmp5B6;union Cyc_Absyn_Constraint*_tmp5B7;union Cyc_Absyn_Constraint*_tmp5B8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B2)->tag == 5){_LL544: _tmp5B3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B2)->f1).elt_typ;_tmp5B4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B2)->f1).elt_tq;_tmp5B5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B2)->f1).ptr_atts).rgn;_tmp5B6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B2)->f1).ptr_atts).nullable;_tmp5B7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B2)->f1).ptr_atts).bounds;_tmp5B8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B2)->f1).ptr_atts).zero_term;_LL545: {
# 2943
enum Cyc_Absyn_Coercion coercion=Cyc_Absyn_Other_coercion;
struct Cyc_List_List*_tmp5B9=({struct Cyc_List_List*_tmp5C3=_cycalloc(sizeof(*_tmp5C3));_tmp5C3->hd=({struct _tuple0*_tmp5C4=_cycalloc(sizeof(*_tmp5C4));_tmp5C4->f1=t1;_tmp5C4->f2=t2;_tmp5C4;});_tmp5C3->tl=0;_tmp5C3;});
int _tmp5BA=_tmp5B4.real_const  || !_tmp5AD.real_const;
# 2957 "tcutil.cyc"
int _tmp5BB=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp5B7,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp5B8);
# 2961
int _tmp5BC=_tmp5BA  && (
((_tmp5BB  && Cyc_Tcutil_ptrsubtype(te,_tmp5B9,_tmp5AC,_tmp5B3) || 
Cyc_Tcutil_unify(_tmp5AC,_tmp5B3)) || Cyc_Tcutil_isomorphic(_tmp5AC,_tmp5B3)) || Cyc_Tcutil_unify(_tmp5B3,(void*)& Cyc_Absyn_VoidType_val));
# 2965
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp5B1,_tmp5B8) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp5B8);
# 2969
int _tmp5BD=_tmp5BC?0:((Cyc_Tcutil_bits_only(_tmp5AC) && Cyc_Tcutil_is_char_type(_tmp5B3)) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5B8)) && (
_tmp5B4.real_const  || !_tmp5AD.real_const);
int bounds_ok=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp5B0,_tmp5B7);
if(!bounds_ok  && !_tmp5BD){
struct _tuple0 _tmp5BE=({struct _tuple0 _tmp5C2;_tmp5C2.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp5B0);_tmp5C2.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp5B7);_tmp5C2;});struct _tuple0 _tmp5BF=_tmp5BE;struct Cyc_Absyn_Exp*_tmp5C0;struct Cyc_Absyn_Exp*_tmp5C1;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5BF.f1)->tag == 1){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5BF.f2)->tag == 1){_LL549: _tmp5C0=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5BF.f1)->f1;_tmp5C1=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5BF.f2)->f1;_LL54A:
# 2979
 if(Cyc_Evexp_lte_const_exp(_tmp5C1,_tmp5C0))
bounds_ok=1;
goto _LL548;}else{goto _LL54B;}}else{_LL54B: _LL54C:
# 2984
 bounds_ok=1;goto _LL548;}_LL548:;}
# 2986
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5AF) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5B6))
coercion=Cyc_Absyn_NonNull_to_Null;
# 2991
if(((bounds_ok  && zeroterm_ok) && (_tmp5BC  || _tmp5BD)) && (
Cyc_Tcutil_unify(_tmp5AE,_tmp5B5) || Cyc_Tcenv_region_outlives(te,_tmp5AE,_tmp5B5)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};}}else{_LL546: _LL547:
 goto _LL543;}_LL543:;}
# 2997
return Cyc_Absyn_Unknown_coercion;case 8: _LL537: _tmp5A8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A5)->f1).elt_type;_tmp5A9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A5)->f1).tq;_tmp5AA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A5)->f1).num_elts;_tmp5AB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A5)->f1).zero_term;_LL538:
# 2999
{void*_tmp5C5=t2;void*_tmp5C6;struct Cyc_Absyn_Tqual _tmp5C7;struct Cyc_Absyn_Exp*_tmp5C8;union Cyc_Absyn_Constraint*_tmp5C9;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5C5)->tag == 8){_LL54E: _tmp5C6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5C5)->f1).elt_type;_tmp5C7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5C5)->f1).tq;_tmp5C8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5C5)->f1).num_elts;_tmp5C9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5C5)->f1).zero_term;_LL54F: {
# 3001
int okay;
okay=
(((_tmp5AA != 0  && _tmp5C8 != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp5AB,_tmp5C9)) && 
Cyc_Evexp_lte_const_exp(_tmp5C8,_tmp5AA)) && 
Cyc_Evexp_lte_const_exp(_tmp5AA,_tmp5C8);
return
# 3008
(okay  && Cyc_Tcutil_unify(_tmp5A8,_tmp5C6)) && (!_tmp5A9.real_const  || _tmp5C7.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}}else{_LL550: _LL551:
# 3010
 return Cyc_Absyn_Unknown_coercion;}_LL54D:;}
# 3012
return Cyc_Absyn_Unknown_coercion;case 13: _LL539: _tmp5A7=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5A5)->f2;_LL53A:
# 3016
{void*_tmp5CA=t2;struct Cyc_Absyn_Enumdecl*_tmp5CB;if(((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5CA)->tag == 13){_LL553: _tmp5CB=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5CA)->f2;_LL554:
# 3018
 if((_tmp5A7->fields != 0  && _tmp5CB->fields != 0) && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5A7->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5CB->fields))->v))
return Cyc_Absyn_Other_coercion;
goto _LL552;}else{_LL555: _LL556:
 goto _LL552;}_LL552:;}
# 3024
goto _LL53C;case 6: _LL53B: _LL53C:
 goto _LL53E;case 7: _LL53D: _LL53E:
 return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;case 15: _LL53F: _tmp5A6=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5A5)->f1;_LL540:
# 3029
{void*_tmp5CC=t2;void*_tmp5CD;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5CC)->tag == 15){_LL558: _tmp5CD=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5CC)->f1;_LL559:
# 3031
 if(Cyc_Tcenv_region_outlives(te,_tmp5A6,_tmp5CD))return Cyc_Absyn_No_coercion;
goto _LL557;}else{_LL55A: _LL55B:
 goto _LL557;}_LL557:;}
# 3035
return Cyc_Absyn_Unknown_coercion;default: _LL541: _LL542:
 return Cyc_Absyn_Unknown_coercion;}_LL534:;};}
# 3041
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),t)){
struct Cyc_Absyn_Exp*_tmp5CE=Cyc_Absyn_copy_exp(e);
e->r=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp5CF=_cycalloc(sizeof(*_tmp5CF));_tmp5CF[0]=({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp5D0;_tmp5D0.tag=13;_tmp5D0.f1=t;_tmp5D0.f2=_tmp5CE;_tmp5D0.f3=0;_tmp5D0.f4=c;_tmp5D0;});_tmp5CF;});
e->topt=t;}}
# 3049
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp5D1=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp5D2=_tmp5D1;switch(*((int*)_tmp5D2)){case 6: _LL55D: _LL55E:
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
void*_tmp5D3=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp5D4=_tmp5D3;if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D4)->tag == 7){_LL56A: _LL56B:
 return 1;}else{_LL56C: _LL56D:
 return 0;}_LL569:;};}
# 3069
int Cyc_Tcutil_is_function_type(void*t){
void*_tmp5D5=Cyc_Tcutil_compress(t);void*_tmp5D6=_tmp5D5;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5D6)->tag == 9){_LL56F: _LL570:
 return 1;}else{_LL571: _LL572:
 return 0;}_LL56E:;}
# 3076
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
struct _tuple0 _tmp5D7=({struct _tuple0 _tmp5DB;_tmp5DB.f1=t1;_tmp5DB.f2=t2;_tmp5DB;});struct _tuple0 _tmp5D8=_tmp5D7;int _tmp5D9;int _tmp5DA;if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D8.f1)->tag == 7){if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D8.f2)->tag == 7){_LL574: _tmp5D9=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D8.f1)->f1;_tmp5DA=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D8.f2)->f1;_LL575:
# 3079
 if(_tmp5D9 != 0  && _tmp5D9 != 1)return t1;else{
if(_tmp5DA != 0  && _tmp5DA != 1)return t2;else{
if(_tmp5D9 >= _tmp5DA)return t1;else{
return t2;}}}}else{_LL576: _LL577:
 return t1;}}else{if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D8.f2)->tag == 7){_LL578: _LL579:
 return t2;}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f1)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f1)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f1)->f2 == Cyc_Absyn_LongLong_sz){_LL57A: _LL57B:
 goto _LL57D;}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL57C;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL582;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL584;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f1)->f2 == Cyc_Absyn_Int_sz)goto _LL586;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f2)->f2 == Cyc_Absyn_Int_sz)goto _LL588;else{goto _LL58E;}}}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL580;else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f1)->f2){case Cyc_Absyn_Long_sz: goto _LL582;case Cyc_Absyn_Int_sz: goto _LL586;default: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL58C;else{goto _LL58E;}}}}}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f1)->f2){case Cyc_Absyn_Long_sz: _LL582: _LL583:
# 3089
 goto _LL585;case Cyc_Absyn_Int_sz: _LL586: _LL587:
# 3092
 goto _LL589;default: goto _LL58E;}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL57C;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL57E;else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f2)->f2){case Cyc_Absyn_Long_sz: goto _LL584;case Cyc_Absyn_Int_sz: goto _LL588;default: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL58A;else{goto _LL58E;}}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL57E;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL580;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL58A;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL58C;else{goto _LL58E;}}}}}}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f1)->f2){case Cyc_Absyn_LongLong_sz: _LL57E: _LL57F:
# 3087
 goto _LL581;case Cyc_Absyn_Long_sz: _LL58A: _LL58B:
# 3094
 goto _LL58D;default: goto _LL58E;}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f2)->f1 == Cyc_Absyn_Unsigned)switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f2)->f2){case Cyc_Absyn_LongLong_sz: _LL57C: _LL57D:
# 3086
 return Cyc_Absyn_ulonglong_typ;case Cyc_Absyn_Long_sz: _LL584: _LL585:
# 3090
 return Cyc_Absyn_ulong_typ;case Cyc_Absyn_Int_sz: _LL588: _LL589:
# 3093
 return Cyc_Absyn_uint_typ;default: goto _LL58E;}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f2)->f2){case Cyc_Absyn_LongLong_sz: _LL580: _LL581:
# 3088
 return Cyc_Absyn_slonglong_typ;case Cyc_Absyn_Long_sz: _LL58C: _LL58D:
# 3095
 return Cyc_Absyn_slong_typ;default: goto _LL58E;}}}else{_LL58E: _LL58F:
 return Cyc_Absyn_sint_typ;}}}}_LL573:;}
# 3101
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_tmp5DC=e->r;void*_tmp5DD=_tmp5DC;if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp5DD)->tag == 3){if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp5DD)->f2 == 0){_LL591: _LL592:
({void*_tmp5DE=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp5DF="assignment in test";_tag_dyneither(_tmp5DF,sizeof(char),19);}),_tag_dyneither(_tmp5DE,sizeof(void*),0));});goto _LL590;}else{goto _LL593;}}else{_LL593: _LL594:
 goto _LL590;}_LL590:;}
# 3115 "tcutil.cyc"
static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){
c1=Cyc_Absyn_compress_kb(c1);
c2=Cyc_Absyn_compress_kb(c2);
if(c1 == c2)return 1;{
struct _tuple0 _tmp5E0=({struct _tuple0 _tmp5F6;_tmp5F6.f1=c1;_tmp5F6.f2=c2;_tmp5F6;});struct _tuple0 _tmp5E1=_tmp5E0;struct Cyc_Core_Opt**_tmp5E2;struct Cyc_Absyn_Kind*_tmp5E3;struct Cyc_Core_Opt**_tmp5E4;struct Cyc_Absyn_Kind*_tmp5E5;struct Cyc_Core_Opt**_tmp5E6;struct Cyc_Absyn_Kind*_tmp5E7;struct Cyc_Absyn_Kind*_tmp5E8;struct Cyc_Core_Opt**_tmp5E9;struct Cyc_Core_Opt**_tmp5EA;struct Cyc_Absyn_Kind*_tmp5EB;struct Cyc_Core_Opt**_tmp5EC;struct Cyc_Absyn_Kind*_tmp5ED;struct Cyc_Absyn_Kind*_tmp5EE;struct Cyc_Absyn_Kind*_tmp5EF;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5E1.f1)->tag == 0)switch(*((int*)_tmp5E1.f2)){case 0: _LL596: _tmp5EE=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5E1.f1)->f1;_tmp5EF=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5E1.f2)->f1;_LL597:
 return _tmp5EE == _tmp5EF;case 1: goto _LL598;default: _LL59E: _tmp5EB=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5E1.f1)->f1;_tmp5EC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5E1.f2)->f1;_tmp5ED=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5E1.f2)->f2;_LL59F:
# 3128
 if(Cyc_Tcutil_kind_leq(_tmp5EB,_tmp5ED)){
*_tmp5EC=({struct Cyc_Core_Opt*_tmp5F3=_cycalloc(sizeof(*_tmp5F3));_tmp5F3->v=c1;_tmp5F3;});return 1;}else{
return 0;}}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5E1.f2)->tag == 1){_LL598: _tmp5EA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5E1.f2)->f1;_LL599:
# 3121
*_tmp5EA=({struct Cyc_Core_Opt*_tmp5F0=_cycalloc(sizeof(*_tmp5F0));_tmp5F0->v=c1;_tmp5F0;});return 1;}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5E1.f1)->tag == 1){_LL59A: _tmp5E9=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5E1.f1)->f1;_LL59B:
*_tmp5E9=({struct Cyc_Core_Opt*_tmp5F1=_cycalloc(sizeof(*_tmp5F1));_tmp5F1->v=c2;_tmp5F1;});return 1;}else{if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5E1.f2)->tag == 0){_LL59C: _tmp5E6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5E1.f1)->f1;_tmp5E7=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5E1.f1)->f2;_tmp5E8=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5E1.f2)->f1;_LL59D:
# 3124
 if(Cyc_Tcutil_kind_leq(_tmp5E8,_tmp5E7)){
*_tmp5E6=({struct Cyc_Core_Opt*_tmp5F2=_cycalloc(sizeof(*_tmp5F2));_tmp5F2->v=c2;_tmp5F2;});return 1;}else{
return 0;}}else{_LL5A0: _tmp5E2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5E1.f1)->f1;_tmp5E3=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5E1.f1)->f2;_tmp5E4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5E1.f2)->f1;_tmp5E5=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5E1.f2)->f2;_LL5A1:
# 3132
 if(Cyc_Tcutil_kind_leq(_tmp5E3,_tmp5E5)){
*_tmp5E4=({struct Cyc_Core_Opt*_tmp5F4=_cycalloc(sizeof(*_tmp5F4));_tmp5F4->v=c1;_tmp5F4;});return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp5E5,_tmp5E3)){
*_tmp5E2=({struct Cyc_Core_Opt*_tmp5F5=_cycalloc(sizeof(*_tmp5F5));_tmp5F5->v=c2;_tmp5F5;});return 1;}else{
return 0;}}}}}}_LL595:;};}
# 3141
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 3146
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp5FB;_tmp5FB.tag=1;_tmp5FB.f1=(unsigned long)i;({void*_tmp5F9[1]={& _tmp5FB};Cyc_aprintf(({const char*_tmp5FA="#%d";_tag_dyneither(_tmp5FA,sizeof(char),4);}),_tag_dyneither(_tmp5F9,sizeof(void*),1));});});
return({struct Cyc_Absyn_Tvar*_tmp5F7=_cycalloc(sizeof(*_tmp5F7));_tmp5F7->name=({struct _dyneither_ptr*_tmp5F8=_cycalloc(sizeof(struct _dyneither_ptr)* 1);_tmp5F8[0]=(struct _dyneither_ptr)s;_tmp5F8;});_tmp5F7->identity=- 1;_tmp5F7->kind=k;_tmp5F7;});}
# 3153
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _dyneither_ptr _tmp5FC=*t->name;
return*((const char*)_check_dyneither_subscript(_tmp5FC,sizeof(char),0))== '#';}
# 3158
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
({struct Cyc_String_pa_PrintArg_struct _tmp5FF;_tmp5FF.tag=0;_tmp5FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name);({void*_tmp5FD[1]={& _tmp5FF};Cyc_printf(({const char*_tmp5FE="%s";_tag_dyneither(_tmp5FE,sizeof(char),3);}),_tag_dyneither(_tmp5FD,sizeof(void*),1));});});
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
struct _dyneither_ptr _tmp600=Cyc_strconcat(({const char*_tmp605="`";_tag_dyneither(_tmp605,sizeof(char),2);}),(struct _dyneither_ptr)*t->name);
({struct _dyneither_ptr _tmp601=_dyneither_ptr_plus(_tmp600,sizeof(char),1);char _tmp602=*((char*)_check_dyneither_subscript(_tmp601,sizeof(char),0));char _tmp603='t';if(_get_dyneither_size(_tmp601,sizeof(char))== 1  && (_tmp602 == '\000'  && _tmp603 != '\000'))_throw_arraybounds();*((char*)_tmp601.curr)=_tmp603;});
t->name=({struct _dyneither_ptr*_tmp604=_cycalloc(sizeof(struct _dyneither_ptr)* 1);_tmp604[0]=(struct _dyneither_ptr)_tmp600;_tmp604;});};}
# 3167
static struct _tuple10*Cyc_Tcutil_fndecl2typ_f(struct _tuple10*x){
# 3169
return({struct _tuple10*_tmp606=_cycalloc(sizeof(*_tmp606));_tmp606->f1=(*x).f1;_tmp606->f2=(*x).f2;_tmp606->f3=(*x).f3;_tmp606;});}
# 3172
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_typ == 0){
# 3179
struct Cyc_List_List*_tmp607=0;
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd))
_tmp607=({struct Cyc_List_List*_tmp608=_cycalloc(sizeof(*_tmp608));_tmp608->hd=(void*)atts->hd;_tmp608->tl=_tmp607;_tmp608;});}}
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp609=_cycalloc(sizeof(*_tmp609));_tmp609[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp60A;_tmp60A.tag=9;_tmp60A.f1=({struct Cyc_Absyn_FnInfo _tmp60B;_tmp60B.tvars=fd->tvs;_tmp60B.effect=fd->effect;_tmp60B.ret_tqual=fd->ret_tqual;_tmp60B.ret_typ=fd->ret_type;_tmp60B.args=
((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args);_tmp60B.c_varargs=fd->c_varargs;_tmp60B.cyc_varargs=fd->cyc_varargs;_tmp60B.rgn_po=fd->rgn_po;_tmp60B.attributes=_tmp607;_tmp60B.requires_clause=fd->requires_clause;_tmp60B.requires_relns=fd->requires_relns;_tmp60B.ensures_clause=fd->ensures_clause;_tmp60B.ensures_relns=fd->ensures_relns;_tmp60B;});_tmp60A;});_tmp609;});}
# 3192
return(void*)_check_null(fd->cached_typ);}
# 3198
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 3200
union Cyc_Relations_RelnOp _tmp60C=*rop;union Cyc_Relations_RelnOp _tmp60D=_tmp60C;struct Cyc_Absyn_Vardecl*_tmp60E;struct Cyc_Absyn_Vardecl*_tmp60F;switch((_tmp60D.RNumelts).tag){case 2: _LL5A3: _tmp60F=(_tmp60D.RVar).val;_LL5A4: {
# 3202
struct _tuple2 _tmp610=*_tmp60F->name;struct _tuple2 _tmp611=_tmp610;union Cyc_Absyn_Nmspace _tmp612;struct _dyneither_ptr*_tmp613;_LL5AA: _tmp612=_tmp611.f1;_tmp613=_tmp611.f2;_LL5AB:;
if(!((int)((_tmp612.Loc_n).tag == 4)))goto _LL5A2;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp613,({const char*_tmp614="return_value";_tag_dyneither(_tmp614,sizeof(char),13);}))== 0){
*rop=Cyc_Relations_RReturn();
goto _LL5A2;}{
# 3208
unsigned int c=0;
{struct Cyc_List_List*_tmp615=args;for(0;_tmp615 != 0;(_tmp615=_tmp615->tl,c ++)){
struct _tuple10*_tmp616=(struct _tuple10*)_tmp615->hd;struct _tuple10*_tmp617=_tmp616;struct _dyneither_ptr*_tmp618;_LL5AD: _tmp618=_tmp617->f1;_LL5AE:;
if(_tmp618 != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp613,(struct _dyneither_ptr)*_tmp618)== 0){
*rop=Cyc_Relations_RParam(c);
break;}}}}
# 3218
goto _LL5A2;};}case 3: _LL5A5: _tmp60E=(_tmp60D.RNumelts).val;_LL5A6: {
# 3220
struct _tuple2 _tmp619=*_tmp60E->name;struct _tuple2 _tmp61A=_tmp619;union Cyc_Absyn_Nmspace _tmp61B;struct _dyneither_ptr*_tmp61C;_LL5B0: _tmp61B=_tmp61A.f1;_tmp61C=_tmp61A.f2;_LL5B1:;
if(!((int)((_tmp61B.Loc_n).tag == 4)))goto _LL5A2;{
unsigned int c=0;
{struct Cyc_List_List*_tmp61D=args;for(0;_tmp61D != 0;(_tmp61D=_tmp61D->tl,c ++)){
struct _tuple10*_tmp61E=(struct _tuple10*)_tmp61D->hd;struct _tuple10*_tmp61F=_tmp61E;struct _dyneither_ptr*_tmp620;_LL5B3: _tmp620=_tmp61F->f1;_LL5B4:;
if(_tmp620 != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp61C,(struct _dyneither_ptr)*_tmp620)== 0){
*rop=Cyc_Relations_RParamNumelts(c);
break;}}}}
# 3232
goto _LL5A2;};}default: _LL5A7: _LL5A8:
 goto _LL5A2;}_LL5A2:;}
# 3237
static void Cyc_Tcutil_replace_rops(struct Cyc_List_List*args,struct Cyc_Relations_Reln*r){
# 3239
Cyc_Tcutil_replace_rop(args,& r->rop1);
Cyc_Tcutil_replace_rop(args,& r->rop2);}
# 3243
static struct Cyc_List_List*Cyc_Tcutil_extract_relns(struct Cyc_List_List*args,struct Cyc_Absyn_Exp*e){
# 3245
if(e == 0)return 0;{
struct Cyc_List_List*_tmp621=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,_tmp621);
return _tmp621;};}struct _tuple25{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 3252
static void*Cyc_Tcutil_fst_fdarg(struct _tuple25*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct _tuple12*_tmp622=pr;struct Cyc_Absyn_Tqual _tmp623;void*_tmp624;_LL5B6: _tmp623=_tmp622->f1;_tmp624=_tmp622->f2;_LL5B7:;
if(_tmp624 == t)return pr;else{
return({struct _tuple12*_tmp625=_cycalloc(sizeof(*_tmp625));_tmp625->f1=_tmp623;_tmp625->f2=t;_tmp625;});}}struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 3259
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
# 3261
return({struct _tuple27*_tmp626=_region_malloc(rgn,sizeof(*_tmp626));_tmp626->f1=({struct _tuple26*_tmp627=_region_malloc(rgn,sizeof(*_tmp627));_tmp627->f1=(*y).f1;_tmp627->f2=(*y).f2;_tmp627;});_tmp626->f2=(*y).f3;_tmp626;});}
# 3263
static struct _tuple10*Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){
# 3265
struct _tuple10 _tmp628=*orig_arg;struct _tuple10 _tmp629=_tmp628;struct _dyneither_ptr*_tmp62A;struct Cyc_Absyn_Tqual _tmp62B;void*_tmp62C;_LL5B9: _tmp62A=_tmp629.f1;_tmp62B=_tmp629.f2;_tmp62C=_tmp629.f3;_LL5BA:;
if(t == _tmp62C)return orig_arg;
return({struct _tuple10*_tmp62D=_cycalloc(sizeof(*_tmp62D));_tmp62D->f1=_tmp62A;_tmp62D->f2=_tmp62B;_tmp62D->f3=t;_tmp62D;});}
# 3269
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){
return f->type;}
# 3272
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 3277
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 3279
return({struct Cyc_Absyn_Exp*_tmp62E=_cycalloc(sizeof(*_tmp62E));_tmp62E->topt=old->topt;_tmp62E->r=r;_tmp62E->loc=old->loc;_tmp62E->annot=old->annot;_tmp62E;});}
# 3284
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp62F=e->r;void*_tmp630=_tmp62F;void*_tmp631;void*_tmp632;struct Cyc_List_List*_tmp633;struct Cyc_Absyn_Exp*_tmp634;void*_tmp635;void*_tmp636;struct Cyc_Absyn_Exp*_tmp637;int _tmp638;enum Cyc_Absyn_Coercion _tmp639;struct Cyc_Absyn_Exp*_tmp63A;struct Cyc_Absyn_Exp*_tmp63B;struct Cyc_Absyn_Exp*_tmp63C;struct Cyc_Absyn_Exp*_tmp63D;struct Cyc_Absyn_Exp*_tmp63E;struct Cyc_Absyn_Exp*_tmp63F;struct Cyc_Absyn_Exp*_tmp640;struct Cyc_Absyn_Exp*_tmp641;struct Cyc_Absyn_Exp*_tmp642;enum Cyc_Absyn_Primop _tmp643;struct Cyc_List_List*_tmp644;switch(*((int*)_tmp630)){case 0: _LL5BC: _LL5BD:
 goto _LL5BF;case 31: _LL5BE: _LL5BF:
 goto _LL5C1;case 32: _LL5C0: _LL5C1:
 goto _LL5C3;case 1: _LL5C2: _LL5C3:
 return e;case 2: _LL5C4: _tmp643=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp630)->f1;_tmp644=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp630)->f2;_LL5C5:
# 3292
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp644)== 1){
struct Cyc_Absyn_Exp*_tmp645=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp644))->hd;
struct Cyc_Absyn_Exp*_tmp646=Cyc_Tcutil_rsubsexp(r,inst,_tmp645);
if(_tmp646 == _tmp645)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp647=_cycalloc(sizeof(*_tmp647));_tmp647[0]=({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp648;_tmp648.tag=2;_tmp648.f1=_tmp643;_tmp648.f2=({struct Cyc_Absyn_Exp*_tmp649[1];_tmp649[0]=_tmp646;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp649,sizeof(struct Cyc_Absyn_Exp*),1));});_tmp648;});_tmp647;}));}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp644)== 2){
struct Cyc_Absyn_Exp*_tmp64A=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp644))->hd;
struct Cyc_Absyn_Exp*_tmp64B=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp644->tl))->hd;
struct Cyc_Absyn_Exp*_tmp64C=Cyc_Tcutil_rsubsexp(r,inst,_tmp64A);
struct Cyc_Absyn_Exp*_tmp64D=Cyc_Tcutil_rsubsexp(r,inst,_tmp64B);
if(_tmp64C == _tmp64A  && _tmp64D == _tmp64B)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp64E=_cycalloc(sizeof(*_tmp64E));_tmp64E[0]=({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp64F;_tmp64F.tag=2;_tmp64F.f1=_tmp643;_tmp64F.f2=({struct Cyc_Absyn_Exp*_tmp650[2];_tmp650[1]=_tmp64D;_tmp650[0]=_tmp64C;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp650,sizeof(struct Cyc_Absyn_Exp*),2));});_tmp64F;});_tmp64E;}));}else{
return({void*_tmp651=0;((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp652="primop does not have 1 or 2 args!";_tag_dyneither(_tmp652,sizeof(char),34);}),_tag_dyneither(_tmp651,sizeof(void*),0));});}}case 5: _LL5C6: _tmp640=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp630)->f1;_tmp641=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp630)->f2;_tmp642=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp630)->f3;_LL5C7: {
# 3306
struct Cyc_Absyn_Exp*_tmp653=Cyc_Tcutil_rsubsexp(r,inst,_tmp640);
struct Cyc_Absyn_Exp*_tmp654=Cyc_Tcutil_rsubsexp(r,inst,_tmp641);
struct Cyc_Absyn_Exp*_tmp655=Cyc_Tcutil_rsubsexp(r,inst,_tmp642);
if((_tmp653 == _tmp640  && _tmp654 == _tmp641) && _tmp655 == _tmp642)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp656=_cycalloc(sizeof(*_tmp656));_tmp656[0]=({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp657;_tmp657.tag=5;_tmp657.f1=_tmp653;_tmp657.f2=_tmp654;_tmp657.f3=_tmp655;_tmp657;});_tmp656;}));}case 6: _LL5C8: _tmp63E=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp630)->f1;_tmp63F=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp630)->f2;_LL5C9: {
# 3312
struct Cyc_Absyn_Exp*_tmp658=Cyc_Tcutil_rsubsexp(r,inst,_tmp63E);
struct Cyc_Absyn_Exp*_tmp659=Cyc_Tcutil_rsubsexp(r,inst,_tmp63F);
if(_tmp658 == _tmp63E  && _tmp659 == _tmp63F)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp65A=_cycalloc(sizeof(*_tmp65A));_tmp65A[0]=({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmp65B;_tmp65B.tag=6;_tmp65B.f1=_tmp658;_tmp65B.f2=_tmp659;_tmp65B;});_tmp65A;}));}case 7: _LL5CA: _tmp63C=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp630)->f1;_tmp63D=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp630)->f2;_LL5CB: {
# 3317
struct Cyc_Absyn_Exp*_tmp65C=Cyc_Tcutil_rsubsexp(r,inst,_tmp63C);
struct Cyc_Absyn_Exp*_tmp65D=Cyc_Tcutil_rsubsexp(r,inst,_tmp63D);
if(_tmp65C == _tmp63C  && _tmp65D == _tmp63D)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp65E=_cycalloc(sizeof(*_tmp65E));_tmp65E[0]=({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmp65F;_tmp65F.tag=7;_tmp65F.f1=_tmp65C;_tmp65F.f2=_tmp65D;_tmp65F;});_tmp65E;}));}case 8: _LL5CC: _tmp63A=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp630)->f1;_tmp63B=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp630)->f2;_LL5CD: {
# 3322
struct Cyc_Absyn_Exp*_tmp660=Cyc_Tcutil_rsubsexp(r,inst,_tmp63A);
struct Cyc_Absyn_Exp*_tmp661=Cyc_Tcutil_rsubsexp(r,inst,_tmp63B);
if(_tmp660 == _tmp63A  && _tmp661 == _tmp63B)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp662=_cycalloc(sizeof(*_tmp662));_tmp662[0]=({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmp663;_tmp663.tag=8;_tmp663.f1=_tmp660;_tmp663.f2=_tmp661;_tmp663;});_tmp662;}));}case 13: _LL5CE: _tmp636=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp630)->f1;_tmp637=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp630)->f2;_tmp638=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp630)->f3;_tmp639=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp630)->f4;_LL5CF: {
# 3327
struct Cyc_Absyn_Exp*_tmp664=Cyc_Tcutil_rsubsexp(r,inst,_tmp637);
void*_tmp665=Cyc_Tcutil_rsubstitute(r,inst,_tmp636);
if(_tmp664 == _tmp637  && _tmp665 == _tmp636)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp666=_cycalloc(sizeof(*_tmp666));_tmp666[0]=({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp667;_tmp667.tag=13;_tmp667.f1=_tmp665;_tmp667.f2=_tmp664;_tmp667.f3=_tmp638;_tmp667.f4=_tmp639;_tmp667;});_tmp666;}));}case 16: _LL5D0: _tmp635=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp630)->f1;_LL5D1: {
# 3332
void*_tmp668=Cyc_Tcutil_rsubstitute(r,inst,_tmp635);
if(_tmp668 == _tmp635)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp669=_cycalloc(sizeof(*_tmp669));_tmp669[0]=({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp66A;_tmp66A.tag=16;_tmp66A.f1=_tmp668;_tmp66A;});_tmp669;}));}case 17: _LL5D2: _tmp634=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp630)->f1;_LL5D3: {
# 3336
struct Cyc_Absyn_Exp*_tmp66B=Cyc_Tcutil_rsubsexp(r,inst,_tmp634);
if(_tmp66B == _tmp634)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp66C=_cycalloc(sizeof(*_tmp66C));_tmp66C[0]=({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmp66D;_tmp66D.tag=17;_tmp66D.f1=_tmp66B;_tmp66D;});_tmp66C;}));}case 18: _LL5D4: _tmp632=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp630)->f1;_tmp633=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp630)->f2;_LL5D5: {
# 3340
void*_tmp66E=Cyc_Tcutil_rsubstitute(r,inst,_tmp632);
if(_tmp66E == _tmp632)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp66F=_cycalloc(sizeof(*_tmp66F));_tmp66F[0]=({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp670;_tmp670.tag=18;_tmp670.f1=_tmp66E;_tmp670.f2=_tmp633;_tmp670;});_tmp66F;}));}case 38: _LL5D6: _tmp631=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp630)->f1;_LL5D7: {
# 3344
void*_tmp671=Cyc_Tcutil_rsubstitute(r,inst,_tmp631);
if(_tmp671 == _tmp631)return e;{
# 3347
void*_tmp672=Cyc_Tcutil_compress(_tmp671);void*_tmp673=_tmp672;struct Cyc_Absyn_Exp*_tmp674;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp673)->tag == 18){_LL5DB: _tmp674=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp673)->f1;_LL5DC:
 return _tmp674;}else{_LL5DD: _LL5DE:
# 3350
 return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp675=_cycalloc(sizeof(*_tmp675));_tmp675[0]=({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp676;_tmp676.tag=38;_tmp676.f1=_tmp671;_tmp676;});_tmp675;}));}_LL5DA:;};}default: _LL5D8: _LL5D9:
# 3353
 return({void*_tmp677=0;((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp678="non-type-level-expression in Tcutil::rsubsexp";_tag_dyneither(_tmp678,sizeof(char),46);}),_tag_dyneither(_tmp677,sizeof(void*),0));});}_LL5BB:;}
# 3357
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 3360
if(e == 0)return 0;else{
return Cyc_Tcutil_rsubsexp(r,inst,e);}}
# 3364
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 3367
void*_tmp679=f->type;
struct Cyc_Absyn_Exp*_tmp67A=f->requires_clause;
void*_tmp67B=Cyc_Tcutil_rsubstitute(r,inst,_tmp679);
struct Cyc_Absyn_Exp*_tmp67C=Cyc_Tcutil_rsubs_exp_opt(r,inst,_tmp67A);
if(_tmp679 == _tmp67B  && _tmp67A == _tmp67C)return f;else{
return({struct Cyc_Absyn_Aggrfield*_tmp67D=_cycalloc(sizeof(*_tmp67D));_tmp67D->name=f->name;_tmp67D->tq=f->tq;_tmp67D->type=_tmp67B;_tmp67D->width=f->width;_tmp67D->attributes=f->attributes;_tmp67D->requires_clause=_tmp67C;_tmp67D;});}}
# 3377
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 3380
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp67E=(struct Cyc_Absyn_Aggrfield*)fs->hd;
struct Cyc_List_List*_tmp67F=fs->tl;
struct Cyc_Absyn_Aggrfield*_tmp680=Cyc_Tcutil_subst_aggrfield(r,inst,_tmp67E);
struct Cyc_List_List*_tmp681=Cyc_Tcutil_subst_aggrfields(r,inst,_tmp67F);
if(_tmp680 == _tmp67E  && _tmp681 == _tmp67F)return fs;
# 3387
return({struct Cyc_List_List*_tmp682=_cycalloc(sizeof(*_tmp682));_tmp682->hd=_tmp680;_tmp682->tl=_tmp681;_tmp682;});};}
# 3390
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 3393
struct _tuple1 _tmp683=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct _tuple1 _tmp684=_tmp683;struct Cyc_List_List*_tmp685;struct Cyc_List_List*_tmp686;_LL5E0: _tmp685=_tmp684.f1;_tmp686=_tmp684.f2;_LL5E1:;{
struct Cyc_List_List*_tmp687=Cyc_Tcutil_substs(rgn,inst,_tmp685);
struct Cyc_List_List*_tmp688=Cyc_Tcutil_substs(rgn,inst,_tmp686);
if(_tmp687 == _tmp685  && _tmp688 == _tmp686)
return rgn_po;else{
# 3399
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp687,_tmp688);}};}
# 3402
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3405
void*_tmp689=Cyc_Tcutil_compress(t);void*_tmp68A=_tmp689;struct Cyc_List_List*_tmp68B;void*_tmp68C;void*_tmp68D;struct Cyc_Absyn_Exp*_tmp68E;struct Cyc_Absyn_Exp*_tmp68F;void*_tmp690;void*_tmp691;void*_tmp692;void*_tmp693;void*_tmp694;enum Cyc_Absyn_AggrKind _tmp695;struct Cyc_List_List*_tmp696;struct Cyc_List_List*_tmp697;struct Cyc_List_List*_tmp698;void*_tmp699;struct Cyc_Absyn_Tqual _tmp69A;void*_tmp69B;struct Cyc_List_List*_tmp69C;int _tmp69D;struct Cyc_Absyn_VarargInfo*_tmp69E;struct Cyc_List_List*_tmp69F;struct Cyc_List_List*_tmp6A0;struct Cyc_Absyn_Exp*_tmp6A1;struct Cyc_Absyn_Exp*_tmp6A2;void*_tmp6A3;struct Cyc_Absyn_Tqual _tmp6A4;void*_tmp6A5;union Cyc_Absyn_Constraint*_tmp6A6;union Cyc_Absyn_Constraint*_tmp6A7;union Cyc_Absyn_Constraint*_tmp6A8;void*_tmp6A9;struct Cyc_Absyn_Tqual _tmp6AA;struct Cyc_Absyn_Exp*_tmp6AB;union Cyc_Absyn_Constraint*_tmp6AC;unsigned int _tmp6AD;struct _tuple2*_tmp6AE;struct Cyc_List_List*_tmp6AF;struct Cyc_Absyn_Typedefdecl*_tmp6B0;void*_tmp6B1;union Cyc_Absyn_DatatypeFieldInfoU _tmp6B2;struct Cyc_List_List*_tmp6B3;union Cyc_Absyn_DatatypeInfoU _tmp6B4;struct Cyc_List_List*_tmp6B5;union Cyc_Absyn_AggrInfoU _tmp6B6;struct Cyc_List_List*_tmp6B7;struct Cyc_Absyn_Tvar*_tmp6B8;switch(*((int*)_tmp68A)){case 2: _LL5E3: _tmp6B8=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp68A)->f1;_LL5E4: {
# 3408
struct _handler_cons _tmp6B9;_push_handler(& _tmp6B9);{int _tmp6BB=0;if(setjmp(_tmp6B9.handler))_tmp6BB=1;if(!_tmp6BB){{void*_tmp6BC=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp6B8);_npop_handler(0);return _tmp6BC;};_pop_handler();}else{void*_tmp6BA=(void*)_exn_thrown;void*_tmp6BD=_tmp6BA;void*_tmp6BE;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp6BD)->tag == Cyc_Core_Not_found){_LL61E: _LL61F:
 return t;}else{_LL620: _tmp6BE=_tmp6BD;_LL621:(int)_rethrow(_tmp6BE);}_LL61D:;}};}case 11: _LL5E5: _tmp6B6=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp68A)->f1).aggr_info;_tmp6B7=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp68A)->f1).targs;_LL5E6: {
# 3411
struct Cyc_List_List*_tmp6BF=Cyc_Tcutil_substs(rgn,inst,_tmp6B7);
return _tmp6BF == _tmp6B7?t:(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp6C0=_cycalloc(sizeof(*_tmp6C0));_tmp6C0[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp6C1;_tmp6C1.tag=11;_tmp6C1.f1=({struct Cyc_Absyn_AggrInfo _tmp6C2;_tmp6C2.aggr_info=_tmp6B6;_tmp6C2.targs=_tmp6BF;_tmp6C2;});_tmp6C1;});_tmp6C0;});}case 3: _LL5E7: _tmp6B4=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp68A)->f1).datatype_info;_tmp6B5=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp68A)->f1).targs;_LL5E8: {
# 3414
struct Cyc_List_List*_tmp6C3=Cyc_Tcutil_substs(rgn,inst,_tmp6B5);
return _tmp6C3 == _tmp6B5?t:(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp6C4=_cycalloc(sizeof(*_tmp6C4));_tmp6C4[0]=({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp6C5;_tmp6C5.tag=3;_tmp6C5.f1=({struct Cyc_Absyn_DatatypeInfo _tmp6C6;_tmp6C6.datatype_info=_tmp6B4;_tmp6C6.targs=_tmp6C3;_tmp6C6;});_tmp6C5;});_tmp6C4;});}case 4: _LL5E9: _tmp6B2=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp68A)->f1).field_info;_tmp6B3=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp68A)->f1).targs;_LL5EA: {
# 3418
struct Cyc_List_List*_tmp6C7=Cyc_Tcutil_substs(rgn,inst,_tmp6B3);
return _tmp6C7 == _tmp6B3?t:(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp6C8=_cycalloc(sizeof(*_tmp6C8));_tmp6C8[0]=({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp6C9;_tmp6C9.tag=4;_tmp6C9.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmp6CA;_tmp6CA.field_info=_tmp6B2;_tmp6CA.targs=_tmp6C7;_tmp6CA;});_tmp6C9;});_tmp6C8;});}case 17: _LL5EB: _tmp6AE=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp68A)->f1;_tmp6AF=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp68A)->f2;_tmp6B0=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp68A)->f3;_tmp6B1=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp68A)->f4;_LL5EC: {
# 3422
struct Cyc_List_List*_tmp6CB=Cyc_Tcutil_substs(rgn,inst,_tmp6AF);
return _tmp6CB == _tmp6AF?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp6CC=_cycalloc(sizeof(*_tmp6CC));_tmp6CC[0]=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp6CD;_tmp6CD.tag=17;_tmp6CD.f1=_tmp6AE;_tmp6CD.f2=_tmp6CB;_tmp6CD.f3=_tmp6B0;_tmp6CD.f4=_tmp6B1;_tmp6CD;});_tmp6CC;});}case 8: _LL5ED: _tmp6A9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp68A)->f1).elt_type;_tmp6AA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp68A)->f1).tq;_tmp6AB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp68A)->f1).num_elts;_tmp6AC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp68A)->f1).zero_term;_tmp6AD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp68A)->f1).zt_loc;_LL5EE: {
# 3425
void*_tmp6CE=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6A9);
struct Cyc_Absyn_Exp*_tmp6CF=_tmp6AB == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,_tmp6AB);
return _tmp6CE == _tmp6A9  && _tmp6CF == _tmp6AB?t:(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp6D0=_cycalloc(sizeof(*_tmp6D0));_tmp6D0[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp6D1;_tmp6D1.tag=8;_tmp6D1.f1=({struct Cyc_Absyn_ArrayInfo _tmp6D2;_tmp6D2.elt_type=_tmp6CE;_tmp6D2.tq=_tmp6AA;_tmp6D2.num_elts=_tmp6CF;_tmp6D2.zero_term=_tmp6AC;_tmp6D2.zt_loc=_tmp6AD;_tmp6D2;});_tmp6D1;});_tmp6D0;});}case 5: _LL5EF: _tmp6A3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp68A)->f1).elt_typ;_tmp6A4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp68A)->f1).elt_tq;_tmp6A5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp68A)->f1).ptr_atts).rgn;_tmp6A6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp68A)->f1).ptr_atts).nullable;_tmp6A7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp68A)->f1).ptr_atts).bounds;_tmp6A8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp68A)->f1).ptr_atts).zero_term;_LL5F0: {
# 3430
void*_tmp6D3=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6A3);
void*_tmp6D4=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6A5);
union Cyc_Absyn_Constraint*_tmp6D5=_tmp6A7;
{void*_tmp6D6=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp6A7);void*_tmp6D7=_tmp6D6;struct Cyc_Absyn_Exp*_tmp6D8;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6D7)->tag == 1){_LL623: _tmp6D8=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6D7)->f1;_LL624: {
# 3435
struct Cyc_Absyn_Exp*_tmp6D9=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp6D8);
if(_tmp6D9 != _tmp6D8)
_tmp6D5=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6DA=_cycalloc(sizeof(*_tmp6DA));_tmp6DA[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp6DB;_tmp6DB.tag=1;_tmp6DB.f1=_tmp6D9;_tmp6DB;});_tmp6DA;}));
goto _LL622;}}else{_LL625: _LL626:
 goto _LL622;}_LL622:;}
# 3441
if((_tmp6D3 == _tmp6A3  && _tmp6D4 == _tmp6A5) && _tmp6D5 == _tmp6A7)
return t;
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6DC=_cycalloc(sizeof(*_tmp6DC));_tmp6DC[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp6DD;_tmp6DD.tag=5;_tmp6DD.f1=({struct Cyc_Absyn_PtrInfo _tmp6DE;_tmp6DE.elt_typ=_tmp6D3;_tmp6DE.elt_tq=_tmp6A4;_tmp6DE.ptr_atts=({(_tmp6DE.ptr_atts).rgn=_tmp6D4;(_tmp6DE.ptr_atts).nullable=_tmp6A6;(_tmp6DE.ptr_atts).bounds=_tmp6D5;(_tmp6DE.ptr_atts).zero_term=_tmp6A8;(_tmp6DE.ptr_atts).ptrloc=0;_tmp6DE.ptr_atts;});_tmp6DE;});_tmp6DD;});_tmp6DC;});}case 9: _LL5F1: _tmp698=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68A)->f1).tvars;_tmp699=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68A)->f1).effect;_tmp69A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68A)->f1).ret_tqual;_tmp69B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68A)->f1).ret_typ;_tmp69C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68A)->f1).args;_tmp69D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68A)->f1).c_varargs;_tmp69E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68A)->f1).cyc_varargs;_tmp69F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68A)->f1).rgn_po;_tmp6A0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68A)->f1).attributes;_tmp6A1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68A)->f1).requires_clause;_tmp6A2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68A)->f1).ensures_clause;_LL5F2:
# 3447
{struct Cyc_List_List*_tmp6DF=_tmp698;for(0;_tmp6DF != 0;_tmp6DF=_tmp6DF->tl){
inst=({struct Cyc_List_List*_tmp6E0=_region_malloc(rgn,sizeof(*_tmp6E0));_tmp6E0->hd=({struct _tuple16*_tmp6E1=_region_malloc(rgn,sizeof(*_tmp6E1));_tmp6E1->f1=(struct Cyc_Absyn_Tvar*)_tmp6DF->hd;_tmp6E1->f2=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp6E2=_cycalloc(sizeof(*_tmp6E2));_tmp6E2[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp6E3;_tmp6E3.tag=2;_tmp6E3.f1=(struct Cyc_Absyn_Tvar*)_tmp6DF->hd;_tmp6E3;});_tmp6E2;});_tmp6E1;});_tmp6E0->tl=inst;_tmp6E0;});}}{
struct _tuple1 _tmp6E4=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp69C));
# 3449
struct _tuple1 _tmp6E5=_tmp6E4;struct Cyc_List_List*_tmp6E6;struct Cyc_List_List*_tmp6E7;_LL628: _tmp6E6=_tmp6E5.f1;_tmp6E7=_tmp6E5.f2;_LL629:;{
# 3451
struct Cyc_List_List*_tmp6E8=_tmp69C;
struct Cyc_List_List*_tmp6E9=Cyc_Tcutil_substs(rgn,inst,_tmp6E7);
if(_tmp6E9 != _tmp6E7)
_tmp6E8=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmp69C,_tmp6E9);{
void*eff2;
if(_tmp699 == 0)
eff2=0;else{
# 3459
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp699);
if(new_eff == _tmp699)
eff2=_tmp699;else{
# 3463
eff2=new_eff;}}{
# 3465
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(_tmp69E == 0)
cyc_varargs2=0;else{
# 3469
struct Cyc_Absyn_VarargInfo _tmp6EA=*_tmp69E;struct Cyc_Absyn_VarargInfo _tmp6EB=_tmp6EA;struct _dyneither_ptr*_tmp6EC;struct Cyc_Absyn_Tqual _tmp6ED;void*_tmp6EE;int _tmp6EF;_LL62B: _tmp6EC=_tmp6EB.name;_tmp6ED=_tmp6EB.tq;_tmp6EE=_tmp6EB.type;_tmp6EF=_tmp6EB.inject;_LL62C:;{
void*_tmp6F0=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6EE);
if(_tmp6F0 == _tmp6EE)cyc_varargs2=_tmp69E;else{
# 3473
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp6F1=_cycalloc(sizeof(*_tmp6F1));_tmp6F1->name=_tmp6EC;_tmp6F1->tq=_tmp6ED;_tmp6F1->type=_tmp6F0;_tmp6F1->inject=_tmp6EF;_tmp6F1;});}};}{
# 3475
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp69F);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp6A1);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp6A2);
struct Cyc_List_List*_tmp6F2=Cyc_Tcutil_extract_relns(_tmp6E8,req2);
struct Cyc_List_List*_tmp6F3=Cyc_Tcutil_extract_relns(_tmp6E8,ens2);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp6F4=_cycalloc(sizeof(*_tmp6F4));_tmp6F4[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp6F5;_tmp6F5.tag=9;_tmp6F5.f1=({struct Cyc_Absyn_FnInfo _tmp6F6;_tmp6F6.tvars=_tmp698;_tmp6F6.effect=eff2;_tmp6F6.ret_tqual=_tmp69A;_tmp6F6.ret_typ=
Cyc_Tcutil_rsubstitute(rgn,inst,_tmp69B);_tmp6F6.args=_tmp6E8;_tmp6F6.c_varargs=_tmp69D;_tmp6F6.cyc_varargs=cyc_varargs2;_tmp6F6.rgn_po=rgn_po2;_tmp6F6.attributes=_tmp6A0;_tmp6F6.requires_clause=req2;_tmp6F6.requires_relns=_tmp6F2;_tmp6F6.ensures_clause=ens2;_tmp6F6.ensures_relns=_tmp6F3;_tmp6F6;});_tmp6F5;});_tmp6F4;});};};};};};case 10: _LL5F3: _tmp697=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp68A)->f1;_LL5F4: {
# 3485
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp6F7=_tmp697;for(0;_tmp6F7 != 0;_tmp6F7=_tmp6F7->tl){
void*_tmp6F8=(*((struct _tuple12*)_tmp6F7->hd)).f2;
void*_tmp6F9=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6F8);
if(_tmp6F8 != _tmp6F9)
change=1;
# 3493
ts2=({struct Cyc_List_List*_tmp6FA=_region_malloc(rgn,sizeof(*_tmp6FA));_tmp6FA->hd=_tmp6F9;_tmp6FA->tl=ts2;_tmp6FA;});}}
# 3495
if(!change)
return t;{
struct Cyc_List_List*_tmp6FB=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp697,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6FC=_cycalloc(sizeof(*_tmp6FC));_tmp6FC[0]=({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp6FD;_tmp6FD.tag=10;_tmp6FD.f1=_tmp6FB;_tmp6FD;});_tmp6FC;});};}case 12: _LL5F5: _tmp695=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp68A)->f1;_tmp696=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp68A)->f2;_LL5F6: {
# 3500
struct Cyc_List_List*_tmp6FE=Cyc_Tcutil_subst_aggrfields(rgn,inst,_tmp696);
if(_tmp696 == _tmp6FE)return t;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp6FF=_cycalloc(sizeof(*_tmp6FF));_tmp6FF[0]=({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp700;_tmp700.tag=12;_tmp700.f1=_tmp695;_tmp700.f2=_tmp6FE;_tmp700;});_tmp6FF;});}case 1: _LL5F7: _tmp694=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp68A)->f2;_LL5F8:
# 3504
 if(_tmp694 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp694);else{
return t;}case 15: _LL5F9: _tmp693=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp68A)->f1;_LL5FA: {
# 3507
void*_tmp701=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp693);
return _tmp701 == _tmp693?t:(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp702=_cycalloc(sizeof(*_tmp702));_tmp702[0]=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp703;_tmp703.tag=15;_tmp703.f1=_tmp701;_tmp703;});_tmp702;});}case 16: _LL5FB: _tmp691=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp68A)->f1;_tmp692=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp68A)->f2;_LL5FC: {
# 3510
void*_tmp704=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp691);
void*_tmp705=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp692);
return _tmp704 == _tmp691  && _tmp705 == _tmp692?t:(void*)({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp706=_cycalloc(sizeof(*_tmp706));_tmp706[0]=({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp707;_tmp707.tag=16;_tmp707.f1=_tmp704;_tmp707.f2=_tmp705;_tmp707;});_tmp706;});}case 19: _LL5FD: _tmp690=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp68A)->f1;_LL5FE: {
# 3514
void*_tmp708=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp690);
return _tmp708 == _tmp690?t:(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp709=_cycalloc(sizeof(*_tmp709));_tmp709[0]=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp70A;_tmp70A.tag=19;_tmp70A.f1=_tmp708;_tmp70A;});_tmp709;});}case 18: _LL5FF: _tmp68F=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp68A)->f1;_LL600: {
# 3517
struct Cyc_Absyn_Exp*_tmp70B=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp68F);
return _tmp70B == _tmp68F?t:(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp70C=_cycalloc(sizeof(*_tmp70C));_tmp70C[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp70D;_tmp70D.tag=18;_tmp70D.f1=_tmp70B;_tmp70D;});_tmp70C;});}case 27: _LL601: _tmp68E=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp68A)->f1;_LL602: {
# 3520
struct Cyc_Absyn_Exp*_tmp70E=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp68E);
return _tmp70E == _tmp68E?t:(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp70F=_cycalloc(sizeof(*_tmp70F));_tmp70F[0]=({struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmp710;_tmp710.tag=27;_tmp710.f1=_tmp70E;_tmp710;});_tmp70F;});}case 13: _LL603: _LL604:
 goto _LL606;case 14: _LL605: _LL606:
 goto _LL608;case 0: _LL607: _LL608:
 goto _LL60A;case 6: _LL609: _LL60A:
 goto _LL60C;case 7: _LL60B: _LL60C:
 goto _LL60E;case 22: _LL60D: _LL60E:
 goto _LL610;case 21: _LL60F: _LL610:
 goto _LL612;case 28: _LL611: _LL612:
 goto _LL614;case 20: _LL613: _LL614:
 return t;case 25: _LL615: _tmp68D=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp68A)->f1;_LL616: {
# 3532
void*_tmp711=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp68D);
return _tmp711 == _tmp68D?t:(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp712=_cycalloc(sizeof(*_tmp712));_tmp712[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp713;_tmp713.tag=25;_tmp713.f1=_tmp711;_tmp713;});_tmp712;});}case 23: _LL617: _tmp68C=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp68A)->f1;_LL618: {
# 3535
void*_tmp714=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp68C);
return _tmp714 == _tmp68C?t:(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp715=_cycalloc(sizeof(*_tmp715));_tmp715[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp716;_tmp716.tag=23;_tmp716.f1=_tmp714;_tmp716;});_tmp715;});}case 24: _LL619: _tmp68B=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp68A)->f1;_LL61A: {
# 3538
struct Cyc_List_List*_tmp717=Cyc_Tcutil_substs(rgn,inst,_tmp68B);
return _tmp717 == _tmp68B?t:(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp718=_cycalloc(sizeof(*_tmp718));_tmp718[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp719;_tmp719.tag=24;_tmp719.f1=_tmp717;_tmp719;});_tmp718;});}default: _LL61B: _LL61C:
({void*_tmp71A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp71B="found typedecltype in rsubs";_tag_dyneither(_tmp71B,sizeof(char),28);}),_tag_dyneither(_tmp71A,sizeof(void*),0));});}_LL5E2:;}
# 3544
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 3547
if(ts == 0)
return 0;{
void*_tmp71C=(void*)ts->hd;
struct Cyc_List_List*_tmp71D=ts->tl;
void*_tmp71E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp71C);
struct Cyc_List_List*_tmp71F=Cyc_Tcutil_substs(rgn,inst,_tmp71D);
if(_tmp71C == _tmp71E  && _tmp71D == _tmp71F)
return ts;
return({struct Cyc_List_List*_tmp720=_cycalloc(sizeof(*_tmp720));_tmp720->hd=_tmp71E;_tmp720->tl=_tmp71F;_tmp720;});};}
# 3558
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 3565
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp721=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple16*_tmp722=_cycalloc(sizeof(*_tmp722));_tmp722->f1=tv;_tmp722->f2=Cyc_Absyn_new_evar(_tmp721,({struct Cyc_Core_Opt*_tmp723=_cycalloc(sizeof(*_tmp723));_tmp723->v=s;_tmp723;}));_tmp722;});}
# 3570
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*env,struct Cyc_Absyn_Tvar*tv){
# 3572
struct _tuple17*_tmp724=env;struct Cyc_List_List*_tmp725;struct _RegionHandle*_tmp726;_LL62E: _tmp725=_tmp724->f1;_tmp726=_tmp724->f2;_LL62F:;{
struct Cyc_Core_Opt*_tmp727=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple16*_tmp728=_region_malloc(_tmp726,sizeof(*_tmp728));_tmp728->f1=tv;_tmp728->f2=Cyc_Absyn_new_evar(_tmp727,({struct Cyc_Core_Opt*_tmp729=_cycalloc(sizeof(*_tmp729));_tmp729->v=_tmp725;_tmp729;}));_tmp728;});};}
# 3582
static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(unsigned int loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3586
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2))
({struct Cyc_String_pa_PrintArg_struct _tmp72E;_tmp72E.tag=0;_tmp72E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(k2));({struct Cyc_String_pa_PrintArg_struct _tmp72D;_tmp72D.tag=0;_tmp72D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1));({struct Cyc_String_pa_PrintArg_struct _tmp72C;_tmp72C.tag=0;_tmp72C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name);({void*_tmp72A[3]={& _tmp72C,& _tmp72D,& _tmp72E};Cyc_Tcutil_terr(loc,({const char*_tmp72B="type variable %s is used with inconsistent kinds %s and %s";_tag_dyneither(_tmp72B,sizeof(char),59);}),_tag_dyneither(_tmp72A,sizeof(void*),3));});});});});
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)
({void*_tmp72F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp730="same type variable has different identity!";_tag_dyneither(_tmp730,sizeof(char),43);}),_tag_dyneither(_tmp72F,sizeof(void*),0));});}
return tvs;}}}
# 3600
tv->identity=Cyc_Tcutil_new_tvar_id();
return({struct Cyc_List_List*_tmp731=_cycalloc(sizeof(*_tmp731));_tmp731->hd=tv;_tmp731->tl=tvs;_tmp731;});}
# 3606
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3608
if(tv->identity == - 1)
({void*_tmp732=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp733="fast_add_free_tvar: bad identity in tv";_tag_dyneither(_tmp733,sizeof(char),39);}),_tag_dyneither(_tmp732,sizeof(void*),0));});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3612
struct Cyc_Absyn_Tvar*_tmp734=(struct Cyc_Absyn_Tvar*)tvs2->hd;
if(_tmp734->identity == - 1)
({void*_tmp735=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp736="fast_add_free_tvar: bad identity in tvs2";_tag_dyneither(_tmp736,sizeof(char),41);}),_tag_dyneither(_tmp735,sizeof(void*),0));});
if(_tmp734->identity == tv->identity)
return tvs;}}
# 3619
return({struct Cyc_List_List*_tmp737=_cycalloc(sizeof(*_tmp737));_tmp737->hd=tv;_tmp737->tl=tvs;_tmp737;});}struct _tuple28{struct Cyc_Absyn_Tvar*f1;int f2;};
# 3625
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 3630
if(tv->identity == - 1)
({void*_tmp738=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp739="fast_add_free_tvar_bool: bad identity in tv";_tag_dyneither(_tmp739,sizeof(char),44);}),_tag_dyneither(_tmp738,sizeof(void*),0));});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3634
struct _tuple28*_tmp73A=(struct _tuple28*)tvs2->hd;struct _tuple28*_tmp73B=_tmp73A;struct Cyc_Absyn_Tvar*_tmp73C;int*_tmp73D;_LL631: _tmp73C=_tmp73B->f1;_tmp73D=(int*)& _tmp73B->f2;_LL632:;
if(_tmp73C->identity == - 1)
({void*_tmp73E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp73F="fast_add_free_tvar_bool: bad identity in tvs2";_tag_dyneither(_tmp73F,sizeof(char),46);}),_tag_dyneither(_tmp73E,sizeof(void*),0));});
if(_tmp73C->identity == tv->identity){
*_tmp73D=*_tmp73D  || b;
return tvs;}}}
# 3642
return({struct Cyc_List_List*_tmp740=_region_malloc(r,sizeof(*_tmp740));_tmp740->hd=({struct _tuple28*_tmp741=_region_malloc(r,sizeof(*_tmp741));_tmp741->f1=tv;_tmp741->f2=b;_tmp741;});_tmp740->tl=tvs;_tmp740;});}
# 3646
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3648
if(tv->identity == - 1)
({struct Cyc_String_pa_PrintArg_struct _tmp744;_tmp744.tag=0;_tmp744.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(tv));({void*_tmp742[1]={& _tmp744};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp743="bound tvar id for %s is NULL";_tag_dyneither(_tmp743,sizeof(char),29);}),_tag_dyneither(_tmp742,sizeof(void*),1));});});
return({struct Cyc_List_List*_tmp745=_cycalloc(sizeof(*_tmp745));_tmp745->hd=tv;_tmp745->tl=tvs;_tmp745;});}struct _tuple29{void*f1;int f2;};
# 3657
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 3660
void*_tmp746=Cyc_Tcutil_compress(e);void*_tmp747=_tmp746;int _tmp748;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp747)->tag == 1){_LL634: _tmp748=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp747)->f3;_LL635:
# 3662
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple29*_tmp749=(struct _tuple29*)es2->hd;struct _tuple29*_tmp74A=_tmp749;void*_tmp74B;int*_tmp74C;_LL639: _tmp74B=_tmp74A->f1;_tmp74C=(int*)& _tmp74A->f2;_LL63A:;{
void*_tmp74D=Cyc_Tcutil_compress(_tmp74B);void*_tmp74E=_tmp74D;int _tmp74F;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp74E)->tag == 1){_LL63C: _tmp74F=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp74E)->f3;_LL63D:
# 3666
 if(_tmp748 == _tmp74F){
if(b != *_tmp74C)*_tmp74C=1;
return es;}
# 3670
goto _LL63B;}else{_LL63E: _LL63F:
 goto _LL63B;}_LL63B:;};}}
# 3674
return({struct Cyc_List_List*_tmp750=_region_malloc(r,sizeof(*_tmp750));_tmp750->hd=({struct _tuple29*_tmp751=_region_malloc(r,sizeof(*_tmp751));_tmp751->f1=e;_tmp751->f2=b;_tmp751;});_tmp750->tl=es;_tmp750;});}else{_LL636: _LL637:
 return es;}_LL633:;}
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
if(!present)r=({struct Cyc_List_List*_tmp752=_region_malloc(rgn,sizeof(*_tmp752));_tmp752->hd=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp752->tl=r;_tmp752;});}
# 3693
r=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);
return r;}
# 3698
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3702
struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp753=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp754=_tmp753;struct Cyc_Absyn_Tvar*_tmp755;int _tmp756;_LL641: _tmp755=_tmp754.f1;_tmp756=_tmp754.f2;_LL642:;{
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp755->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3712
if(!present)res=({struct Cyc_List_List*_tmp757=_region_malloc(r,sizeof(*_tmp757));_tmp757->hd=(struct _tuple28*)tvs->hd;_tmp757->tl=res;_tmp757;});};}
# 3714
res=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);
return res;}
# 3718
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){
# 3720
if(width != 0){
unsigned int w=0;
if(!Cyc_Tcutil_is_const_exp(width))
({struct Cyc_String_pa_PrintArg_struct _tmp75A;_tmp75A.tag=0;_tmp75A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);({void*_tmp758[1]={& _tmp75A};Cyc_Tcutil_terr(loc,({const char*_tmp759="bitfield %s does not have constant width";_tag_dyneither(_tmp759,sizeof(char),41);}),_tag_dyneither(_tmp758,sizeof(void*),1));});});else{
# 3725
struct _tuple14 _tmp75B=Cyc_Evexp_eval_const_uint_exp(width);struct _tuple14 _tmp75C=_tmp75B;unsigned int _tmp75D;int _tmp75E;_LL644: _tmp75D=_tmp75C.f1;_tmp75E=_tmp75C.f2;_LL645:;
if(!_tmp75E)
({void*_tmp75F=0;Cyc_Tcutil_terr(loc,({const char*_tmp760="bitfield width cannot use sizeof or offsetof";_tag_dyneither(_tmp760,sizeof(char),45);}),_tag_dyneither(_tmp75F,sizeof(void*),0));});
w=_tmp75D;}{
# 3730
void*_tmp761=Cyc_Tcutil_compress(field_typ);void*_tmp762=_tmp761;enum Cyc_Absyn_Size_of _tmp763;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp762)->tag == 6){_LL647: _tmp763=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp762)->f2;_LL648:
# 3733
{enum Cyc_Absyn_Size_of _tmp764=_tmp763;switch(_tmp764){case Cyc_Absyn_Char_sz: _LL64C: _LL64D:
 if(w > 8)({void*_tmp765=0;Cyc_Tcutil_terr(loc,({const char*_tmp766="bitfield larger than type";_tag_dyneither(_tmp766,sizeof(char),26);}),_tag_dyneither(_tmp765,sizeof(void*),0));});goto _LL64B;case Cyc_Absyn_Short_sz: _LL64E: _LL64F:
 if(w > 16)({void*_tmp767=0;Cyc_Tcutil_terr(loc,({const char*_tmp768="bitfield larger than type";_tag_dyneither(_tmp768,sizeof(char),26);}),_tag_dyneither(_tmp767,sizeof(void*),0));});goto _LL64B;case Cyc_Absyn_Long_sz: _LL650: _LL651:
 goto _LL653;case Cyc_Absyn_Int_sz: _LL652: _LL653:
# 3738
 if(w > 32)({void*_tmp769=0;Cyc_Tcutil_terr(loc,({const char*_tmp76A="bitfield larger than type";_tag_dyneither(_tmp76A,sizeof(char),26);}),_tag_dyneither(_tmp769,sizeof(void*),0));});goto _LL64B;default: _LL654: _LL655:
# 3740
 if(w > 64)({void*_tmp76B=0;Cyc_Tcutil_terr(loc,({const char*_tmp76C="bitfield larger than type";_tag_dyneither(_tmp76C,sizeof(char),26);}),_tag_dyneither(_tmp76B,sizeof(void*),0));});goto _LL64B;}_LL64B:;}
# 3742
goto _LL646;}else{_LL649: _LL64A:
# 3744
({struct Cyc_String_pa_PrintArg_struct _tmp770;_tmp770.tag=0;_tmp770.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(field_typ));({struct Cyc_String_pa_PrintArg_struct _tmp76F;_tmp76F.tag=0;_tmp76F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);({void*_tmp76D[2]={& _tmp76F,& _tmp770};Cyc_Tcutil_terr(loc,({const char*_tmp76E="bitfield %s must have integral type but has type %s";_tag_dyneither(_tmp76E,sizeof(char),52);}),_tag_dyneither(_tmp76D,sizeof(void*),2));});});});
goto _LL646;}_LL646:;};}}
# 3751
static void Cyc_Tcutil_check_field_atts(unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp771=(void*)atts->hd;void*_tmp772=_tmp771;switch(*((int*)_tmp772)){case 7: _LL657: _LL658:
 continue;case 6: _LL659: _LL65A:
 continue;default: _LL65B: _LL65C:
({struct Cyc_String_pa_PrintArg_struct _tmp776;_tmp776.tag=0;_tmp776.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);({struct Cyc_String_pa_PrintArg_struct _tmp775;_tmp775.tag=0;_tmp775.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd));({void*_tmp773[2]={& _tmp775,& _tmp776};Cyc_Tcutil_terr(loc,({const char*_tmp774="bad attribute %s on member %s";_tag_dyneither(_tmp774,sizeof(char),30);}),_tag_dyneither(_tmp773,sizeof(void*),2));});});});}_LL656:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
# 3775
typedef struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_cvtenv_t;
# 3779
int Cyc_Tcutil_extract_const_from_typedef(unsigned int loc,int declared_const,void*t){
void*_tmp777=t;struct Cyc_Absyn_Typedefdecl*_tmp778;void*_tmp779;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp777)->tag == 17){_LL65E: _tmp778=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp777)->f3;_tmp779=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp777)->f4;_LL65F:
# 3782
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp778))->tq).real_const  || (_tmp778->tq).print_const){
if(declared_const)({void*_tmp77A=0;Cyc_Tcutil_warn(loc,({const char*_tmp77B="extra const";_tag_dyneither(_tmp77B,sizeof(char),12);}),_tag_dyneither(_tmp77A,sizeof(void*),0));});
return 1;}
# 3787
if((unsigned int)_tmp779)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp779);else{
return declared_const;}}else{_LL660: _LL661:
 return declared_const;}_LL65D:;}
# 3794
static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td != 0){
if(td->defn != 0){
void*_tmp77C=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_tmp77D=_tmp77C;void*_tmp77E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp77D)->tag == 5){_LL663: _tmp77E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp77D)->f1).ptr_atts).rgn;_LL664:
# 3799
{void*_tmp77F=Cyc_Tcutil_compress(_tmp77E);void*_tmp780=_tmp77F;struct Cyc_Absyn_Tvar*_tmp781;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp780)->tag == 2){_LL668: _tmp781=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp780)->f1;_LL669:
# 3801
 return Cyc_Absyn_tvar_cmp(tvar,_tmp781)== 0;}else{_LL66A: _LL66B:
 goto _LL667;}_LL667:;}
# 3804
goto _LL662;}else{_LL665: _LL666:
 goto _LL662;}_LL662:;}}else{
# 3810
return 1;}
return 0;}
# 3814
static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 3817
void*_tmp782=Cyc_Absyn_compress_kb(tvar->kind);void*_tmp783=_tmp782;switch(*((int*)_tmp783)){case 2: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp783)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp783)->f2)->aliasqual == Cyc_Absyn_Top){_LL66D: _LL66E:
 goto _LL670;}else{goto _LL671;}}else{goto _LL671;}case 0: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp783)->f1)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp783)->f1)->aliasqual == Cyc_Absyn_Top){_LL66F: _LL670:
# 3826
 if(((expected_kind->kind == Cyc_Absyn_BoxKind  || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind) && 
# 3829
Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){
if(expected_kind->aliasqual == Cyc_Absyn_Aliasable)
return& Cyc_Tcutil_rk;else{
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}
# 3835
return& Cyc_Tcutil_trk;}else{goto _LL671;}}else{goto _LL671;}default: _LL671: _LL672:
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);}_LL66C:;}
# 3841
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);struct _tuple30{struct Cyc_Tcutil_CVTEnv f1;struct Cyc_List_List*f2;};
# 3845
static struct _tuple30 Cyc_Tcutil_check_clause(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct _dyneither_ptr clause_name,struct Cyc_Absyn_Exp*clause){
# 3848
struct Cyc_List_List*relns=0;
if(clause != 0){
Cyc_Tcexp_tcExp(te,0,clause);
if(!Cyc_Tcutil_is_integral(clause))
({struct Cyc_String_pa_PrintArg_struct _tmp787;_tmp787.tag=0;_tmp787.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(clause->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp786;_tmp786.tag=0;_tmp786.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name);({void*_tmp784[2]={& _tmp786,& _tmp787};Cyc_Tcutil_terr(loc,({const char*_tmp785="%s clause has type %s instead of integral type";_tag_dyneither(_tmp785,sizeof(char),47);}),_tag_dyneither(_tmp784,sizeof(void*),2));});});});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(clause,te,cvtenv);
relns=Cyc_Relations_exp2relns(Cyc_Core_heap_region,clause);
if(!Cyc_Relations_consistent_relations(relns))
({struct Cyc_String_pa_PrintArg_struct _tmp78B;_tmp78B.tag=0;_tmp78B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(clause));({struct Cyc_String_pa_PrintArg_struct _tmp78A;_tmp78A.tag=0;_tmp78A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name);({void*_tmp788[2]={& _tmp78A,& _tmp78B};Cyc_Tcutil_terr(clause->loc,({const char*_tmp789="%s clause '%s' may be unsatisfiable";_tag_dyneither(_tmp789,sizeof(char),36);}),_tag_dyneither(_tmp788,sizeof(void*),2));});});});}
# 3860
return({struct _tuple30 _tmp78C;_tmp78C.f1=cvtenv;_tmp78C.f2=relns;_tmp78C;});}struct _tuple31{enum Cyc_Absyn_Format_Type f1;void*f2;};
# 3892 "tcutil.cyc"
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect,int allow_abs_aggr){
# 3900
{void*_tmp78D=Cyc_Tcutil_compress(t);void*_tmp78E=_tmp78D;struct Cyc_List_List*_tmp78F;void*_tmp790;void*_tmp791;void*_tmp792;void*_tmp793;void*_tmp794;struct _tuple2*_tmp795;struct Cyc_List_List**_tmp796;struct Cyc_Absyn_Typedefdecl**_tmp797;void**_tmp798;union Cyc_Absyn_AggrInfoU*_tmp799;struct Cyc_List_List**_tmp79A;enum Cyc_Absyn_AggrKind _tmp79B;struct Cyc_List_List*_tmp79C;struct Cyc_List_List*_tmp79D;struct Cyc_List_List**_tmp79E;void**_tmp79F;struct Cyc_Absyn_Tqual*_tmp7A0;void*_tmp7A1;struct Cyc_List_List*_tmp7A2;int _tmp7A3;struct Cyc_Absyn_VarargInfo*_tmp7A4;struct Cyc_List_List*_tmp7A5;struct Cyc_List_List*_tmp7A6;struct Cyc_Absyn_Exp*_tmp7A7;struct Cyc_List_List**_tmp7A8;struct Cyc_Absyn_Exp*_tmp7A9;struct Cyc_List_List**_tmp7AA;void*_tmp7AB;struct Cyc_Absyn_Tqual*_tmp7AC;struct Cyc_Absyn_Exp**_tmp7AD;union Cyc_Absyn_Constraint*_tmp7AE;unsigned int _tmp7AF;struct Cyc_Absyn_Exp*_tmp7B0;struct Cyc_Absyn_Exp*_tmp7B1;void*_tmp7B2;void*_tmp7B3;struct Cyc_Absyn_Tqual*_tmp7B4;void*_tmp7B5;union Cyc_Absyn_Constraint*_tmp7B6;union Cyc_Absyn_Constraint*_tmp7B7;union Cyc_Absyn_Constraint*_tmp7B8;union Cyc_Absyn_DatatypeFieldInfoU*_tmp7B9;struct Cyc_List_List*_tmp7BA;union Cyc_Absyn_DatatypeInfoU*_tmp7BB;struct Cyc_List_List**_tmp7BC;struct _tuple2*_tmp7BD;struct Cyc_Absyn_Enumdecl**_tmp7BE;struct Cyc_List_List*_tmp7BF;void*_tmp7C0;void***_tmp7C1;struct Cyc_Absyn_Tvar*_tmp7C2;struct Cyc_Core_Opt**_tmp7C3;void**_tmp7C4;switch(*((int*)_tmp78E)){case 0: _LL674: _LL675:
 goto _LL673;case 1: _LL676: _tmp7C3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp78E)->f1;_tmp7C4=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp78E)->f2;_LL677:
# 3904
 if(*_tmp7C3 == 0  || 
Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp7C3))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp7C3))->v,expected_kind))
*_tmp7C3=Cyc_Tcutil_kind_to_opt(expected_kind);
if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){
# 3909
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
*_tmp7C4=(void*)& Cyc_Absyn_UniqueRgn_val;else{
# 3912
*_tmp7C4=(void*)& Cyc_Absyn_HeapRgn_val;}}else{
if(cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar*_tmp7C5=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp7C8=_cycalloc(sizeof(*_tmp7C8));_tmp7C8[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp7C9;_tmp7C9.tag=2;_tmp7C9.f1=0;_tmp7C9.f2=expected_kind;_tmp7C9;});_tmp7C8;}));
*_tmp7C4=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp7C6=_cycalloc(sizeof(*_tmp7C6));_tmp7C6[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp7C7;_tmp7C7.tag=2;_tmp7C7.f1=_tmp7C5;_tmp7C7;});_tmp7C6;});
_tmp7C2=_tmp7C5;goto _LL679;}else{
# 3918
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}
# 3920
goto _LL673;case 2: _LL678: _tmp7C2=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp78E)->f1;_LL679:
# 3922
{void*_tmp7CA=Cyc_Absyn_compress_kb(_tmp7C2->kind);void*_tmp7CB=_tmp7CA;struct Cyc_Core_Opt**_tmp7CC;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7CB)->tag == 1){_LL6AF: _tmp7CC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7CB)->f1;_LL6B0:
# 3924
*_tmp7CC=({struct Cyc_Core_Opt*_tmp7CD=_cycalloc(sizeof(*_tmp7CD));_tmp7CD->v=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp7CE=_cycalloc(sizeof(*_tmp7CE));_tmp7CE[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp7CF;_tmp7CF.tag=2;_tmp7CF.f1=0;_tmp7CF.f2=expected_kind;_tmp7CF;});_tmp7CE;});_tmp7CD;});goto _LL6AE;}else{_LL6B1: _LL6B2:
 goto _LL6AE;}_LL6AE:;}
# 3929
cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp7C2);
# 3932
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp7C2,put_in_effect);
# 3934
{void*_tmp7D0=Cyc_Absyn_compress_kb(_tmp7C2->kind);void*_tmp7D1=_tmp7D0;struct Cyc_Core_Opt**_tmp7D2;struct Cyc_Absyn_Kind*_tmp7D3;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7D1)->tag == 2){_LL6B4: _tmp7D2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7D1)->f1;_tmp7D3=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7D1)->f2;_LL6B5:
# 3936
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmp7D3))
*_tmp7D2=({struct Cyc_Core_Opt*_tmp7D4=_cycalloc(sizeof(*_tmp7D4));_tmp7D4->v=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp7D5=_cycalloc(sizeof(*_tmp7D5));_tmp7D5[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp7D6;_tmp7D6.tag=2;_tmp7D6.f1=0;_tmp7D6.f2=expected_kind;_tmp7D6;});_tmp7D5;});_tmp7D4;});
goto _LL6B3;}else{_LL6B6: _LL6B7:
 goto _LL6B3;}_LL6B3:;}
# 3941
goto _LL673;case 26: _LL67A: _tmp7C0=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp78E)->f1)->r;_tmp7C1=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp78E)->f2;_LL67B: {
# 3947
void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmp7D7=_tmp7C0;struct Cyc_Absyn_Datatypedecl*_tmp7D8;struct Cyc_Absyn_Enumdecl*_tmp7D9;struct Cyc_Absyn_Aggrdecl*_tmp7DA;switch(*((int*)_tmp7D7)){case 0: _LL6B9: _tmp7DA=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp7D7)->f1;_LL6BA:
# 3950
 if(te->in_extern_c_include)
_tmp7DA->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp7DA);goto _LL6B8;case 1: _LL6BB: _tmp7D9=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp7D7)->f1;_LL6BC:
# 3954
 if(te->in_extern_c_include)
_tmp7D9->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp7D9);goto _LL6B8;default: _LL6BD: _tmp7D8=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp7D7)->f1;_LL6BE:
# 3958
 Cyc_Tc_tcDatatypedecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp7D8);goto _LL6B8;}_LL6B8:;}
# 3960
*_tmp7C1=({void**_tmp7DB=_cycalloc(sizeof(*_tmp7DB));_tmp7DB[0]=new_t;_tmp7DB;});
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect,allow_abs_aggr);}case 14: _LL67C: _tmp7BF=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp78E)->f1;_LL67D: {
# 3966
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct _RegionHandle _tmp7DC=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp7DC;_push_region(uprev_rgn);{
struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;
for(0;_tmp7BF != 0;_tmp7BF=_tmp7BF->tl){
struct Cyc_Absyn_Enumfield*_tmp7DD=(struct Cyc_Absyn_Enumfield*)_tmp7BF->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp7DD->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp7E0;_tmp7E0.tag=0;_tmp7E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp7DD->name).f2);({void*_tmp7DE[1]={& _tmp7E0};Cyc_Tcutil_terr(_tmp7DD->loc,({const char*_tmp7DF="duplicate enum field name %s";_tag_dyneither(_tmp7DF,sizeof(char),29);}),_tag_dyneither(_tmp7DE,sizeof(void*),1));});});else{
# 3975
prev_fields=({struct Cyc_List_List*_tmp7E1=_region_malloc(uprev_rgn,sizeof(*_tmp7E1));_tmp7E1->hd=(*_tmp7DD->name).f2;_tmp7E1->tl=prev_fields;_tmp7E1;});}
# 3977
if(_tmp7DD->tag == 0)
_tmp7DD->tag=Cyc_Absyn_uint_exp(tag_count,_tmp7DD->loc);else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp7DD->tag)))
({struct Cyc_String_pa_PrintArg_struct _tmp7E4;_tmp7E4.tag=0;_tmp7E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp7DD->name).f2);({void*_tmp7E2[1]={& _tmp7E4};Cyc_Tcutil_terr(loc,({const char*_tmp7E3="enum field %s: expression is not constant";_tag_dyneither(_tmp7E3,sizeof(char),42);}),_tag_dyneither(_tmp7E2,sizeof(void*),1));});});}{
# 3982
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp7DD->tag))).f1;
tag_count=t1 + 1;
{union Cyc_Absyn_Nmspace _tmp7E5=(*_tmp7DD->name).f1;union Cyc_Absyn_Nmspace _tmp7E6=_tmp7E5;if((_tmp7E6.Rel_n).tag == 1){_LL6C0: _LL6C1:
# 3986
(*_tmp7DD->name).f1=Cyc_Absyn_Abs_n(te->ns,0);goto _LL6BF;}else{_LL6C2: _LL6C3:
 goto _LL6BF;}_LL6BF:;}
# 3989
ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple29*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp7DD->name).f2,({struct _tuple29*_tmp7E7=_cycalloc(sizeof(*_tmp7E7));_tmp7E7->f1=(void*)({struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp7E8=_cycalloc(sizeof(*_tmp7E8));_tmp7E8[0]=({struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct _tmp7E9;_tmp7E9.tag=4;_tmp7E9.f1=t;_tmp7E9.f2=_tmp7DD;_tmp7E9;});_tmp7E8;});_tmp7E7->f2=1;_tmp7E7;}));};}}
# 3993
_npop_handler(0);goto _LL673;
# 3967
;_pop_region(uprev_rgn);}case 13: _LL67E: _tmp7BD=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp78E)->f1;_tmp7BE=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp78E)->f2;_LL67F:
# 3995
 if(*_tmp7BE == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp7BE))->fields == 0){
struct _handler_cons _tmp7EA;_push_handler(& _tmp7EA);{int _tmp7EC=0;if(setjmp(_tmp7EA.handler))_tmp7EC=1;if(!_tmp7EC){
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp7BD);
*_tmp7BE=*ed;}
# 3997
;_pop_handler();}else{void*_tmp7EB=(void*)_exn_thrown;void*_tmp7ED=_tmp7EB;void*_tmp7EE;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp7ED)->tag == Cyc_Dict_Absent){_LL6C5: _LL6C6: {
# 4001
struct Cyc_Tcenv_Genv*_tmp7EF=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp7F0=({struct Cyc_Absyn_Enumdecl*_tmp7F1=_cycalloc(sizeof(*_tmp7F1));_tmp7F1->sc=Cyc_Absyn_Extern;_tmp7F1->name=_tmp7BD;_tmp7F1->fields=0;_tmp7F1;});
Cyc_Tc_tcEnumdecl(te,_tmp7EF,loc,_tmp7F0);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp7BD);
*_tmp7BE=*ed;
goto _LL6C4;};}}else{_LL6C7: _tmp7EE=_tmp7ED;_LL6C8:(int)_rethrow(_tmp7EE);}_LL6C4:;}};}{
# 4010
struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp7BE);
# 4012
*_tmp7BD=(ed->name)[0];
goto _LL673;};case 3: _LL680: _tmp7BB=(union Cyc_Absyn_DatatypeInfoU*)&(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp78E)->f1).datatype_info;_tmp7BC=(struct Cyc_List_List**)&(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp78E)->f1).targs;_LL681: {
# 4015
struct Cyc_List_List*_tmp7F2=*_tmp7BC;
{union Cyc_Absyn_DatatypeInfoU _tmp7F3=*_tmp7BB;union Cyc_Absyn_DatatypeInfoU _tmp7F4=_tmp7F3;struct Cyc_Absyn_Datatypedecl*_tmp7F5;struct _tuple2*_tmp7F6;int _tmp7F7;if((_tmp7F4.UnknownDatatype).tag == 1){_LL6CA: _tmp7F6=((_tmp7F4.UnknownDatatype).val).name;_tmp7F7=((_tmp7F4.UnknownDatatype).val).is_extensible;_LL6CB: {
# 4018
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmp7F8;_push_handler(& _tmp7F8);{int _tmp7FA=0;if(setjmp(_tmp7F8.handler))_tmp7FA=1;if(!_tmp7FA){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp7F6);;_pop_handler();}else{void*_tmp7F9=(void*)_exn_thrown;void*_tmp7FB=_tmp7F9;void*_tmp7FC;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp7FB)->tag == Cyc_Dict_Absent){_LL6CF: _LL6D0: {
# 4022
struct Cyc_Tcenv_Genv*_tmp7FD=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Datatypedecl*_tmp7FE=({struct Cyc_Absyn_Datatypedecl*_tmp802=_cycalloc(sizeof(*_tmp802));_tmp802->sc=Cyc_Absyn_Extern;_tmp802->name=_tmp7F6;_tmp802->tvs=0;_tmp802->fields=0;_tmp802->is_extensible=_tmp7F7;_tmp802;});
Cyc_Tc_tcDatatypedecl(te,_tmp7FD,loc,_tmp7FE);
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp7F6);
# 4027
if(_tmp7F2 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp801;_tmp801.tag=0;_tmp801.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7F6));({void*_tmp7FF[1]={& _tmp801};Cyc_Tcutil_terr(loc,({const char*_tmp800="declare parameterized datatype %s before using";_tag_dyneither(_tmp800,sizeof(char),47);}),_tag_dyneither(_tmp7FF,sizeof(void*),1));});});
return cvtenv;}
# 4032
goto _LL6CE;}}else{_LL6D1: _tmp7FC=_tmp7FB;_LL6D2:(int)_rethrow(_tmp7FC);}_LL6CE:;}};}
# 4038
if(_tmp7F7  && !(*tudp)->is_extensible)
({struct Cyc_String_pa_PrintArg_struct _tmp805;_tmp805.tag=0;_tmp805.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7F6));({void*_tmp803[1]={& _tmp805};Cyc_Tcutil_terr(loc,({const char*_tmp804="datatype %s was not declared @extensible";_tag_dyneither(_tmp804,sizeof(char),41);}),_tag_dyneither(_tmp803,sizeof(void*),1));});});
*_tmp7BB=Cyc_Absyn_KnownDatatype(tudp);
_tmp7F5=*tudp;goto _LL6CD;}}else{_LL6CC: _tmp7F5=*(_tmp7F4.KnownDatatype).val;_LL6CD: {
# 4045
struct Cyc_List_List*tvs=_tmp7F5->tvs;
for(0;_tmp7F2 != 0  && tvs != 0;(_tmp7F2=_tmp7F2->tl,tvs=tvs->tl)){
void*t=(void*)_tmp7F2->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 4051
{struct _tuple0 _tmp806=({struct _tuple0 _tmp809;_tmp809.f1=Cyc_Absyn_compress_kb(tv->kind);_tmp809.f2=t;_tmp809;});struct _tuple0 _tmp807=_tmp806;struct Cyc_Absyn_Tvar*_tmp808;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp807.f1)->tag == 1){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp807.f2)->tag == 2){_LL6D4: _tmp808=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp807.f2)->f1;_LL6D5:
# 4053
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp808);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp808,1);
continue;}else{goto _LL6D6;}}else{_LL6D6: _LL6D7:
 goto _LL6D3;}_LL6D3:;}{
# 4058
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 4062
if(_tmp7F2 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp80C;_tmp80C.tag=0;_tmp80C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7F5->name));({void*_tmp80A[1]={& _tmp80C};Cyc_Tcutil_terr(loc,({const char*_tmp80B="too many type arguments for datatype %s";_tag_dyneither(_tmp80B,sizeof(char),40);}),_tag_dyneither(_tmp80A,sizeof(void*),1));});});
if(tvs != 0){
# 4067
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp80D=_cycalloc(sizeof(*_tmp80D));_tmp80D->hd=e;_tmp80D->tl=hidden_ts;_tmp80D;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1,allow_abs_aggr);}
# 4074
*_tmp7BC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp7BC,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4076
goto _LL6C9;}}_LL6C9:;}
# 4078
goto _LL673;}case 4: _LL682: _tmp7B9=(union Cyc_Absyn_DatatypeFieldInfoU*)&(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp78E)->f1).field_info;_tmp7BA=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp78E)->f1).targs;_LL683:
# 4081
{union Cyc_Absyn_DatatypeFieldInfoU _tmp80E=*_tmp7B9;union Cyc_Absyn_DatatypeFieldInfoU _tmp80F=_tmp80E;struct Cyc_Absyn_Datatypedecl*_tmp810;struct Cyc_Absyn_Datatypefield*_tmp811;struct _tuple2*_tmp812;struct _tuple2*_tmp813;int _tmp814;if((_tmp80F.UnknownDatatypefield).tag == 1){_LL6D9: _tmp812=((_tmp80F.UnknownDatatypefield).val).datatype_name;_tmp813=((_tmp80F.UnknownDatatypefield).val).field_name;_tmp814=((_tmp80F.UnknownDatatypefield).val).is_extensible;_LL6DA: {
# 4083
struct Cyc_Absyn_Datatypedecl*tud;
struct Cyc_Absyn_Datatypefield*tuf;
{struct _handler_cons _tmp815;_push_handler(& _tmp815);{int _tmp817=0;if(setjmp(_tmp815.handler))_tmp817=1;if(!_tmp817){*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp812);;_pop_handler();}else{void*_tmp816=(void*)_exn_thrown;void*_tmp818=_tmp816;void*_tmp819;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp818)->tag == Cyc_Dict_Absent){_LL6DE: _LL6DF:
# 4087
({struct Cyc_String_pa_PrintArg_struct _tmp81C;_tmp81C.tag=0;_tmp81C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp812));({void*_tmp81A[1]={& _tmp81C};Cyc_Tcutil_terr(loc,({const char*_tmp81B="unbound datatype %s";_tag_dyneither(_tmp81B,sizeof(char),20);}),_tag_dyneither(_tmp81A,sizeof(void*),1));});});
return cvtenv;}else{_LL6E0: _tmp819=_tmp818;_LL6E1:(int)_rethrow(_tmp819);}_LL6DD:;}};}
# 4090
{struct _handler_cons _tmp81D;_push_handler(& _tmp81D);{int _tmp81F=0;if(setjmp(_tmp81D.handler))_tmp81F=1;if(!_tmp81F){
{struct _RegionHandle _tmp820=_new_region("r");struct _RegionHandle*r=& _tmp820;_push_region(r);
{void*_tmp821=Cyc_Tcenv_lookup_ordinary(r,te,loc,_tmp813,0);void*_tmp822=_tmp821;struct Cyc_Absyn_Datatypedecl*_tmp823;struct Cyc_Absyn_Datatypefield*_tmp824;if(((struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp822)->tag == 2){_LL6E3: _tmp823=((struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp822)->f1;_tmp824=((struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp822)->f2;_LL6E4:
# 4094
 tuf=_tmp824;
tud=_tmp823;
if(_tmp814  && !tud->is_extensible)
({struct Cyc_String_pa_PrintArg_struct _tmp827;_tmp827.tag=0;_tmp827.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp812));({void*_tmp825[1]={& _tmp827};Cyc_Tcutil_terr(loc,({const char*_tmp826="datatype %s was not declared @extensible";_tag_dyneither(_tmp826,sizeof(char),41);}),_tag_dyneither(_tmp825,sizeof(void*),1));});});
goto _LL6E2;}else{_LL6E5: _LL6E6:
({struct Cyc_String_pa_PrintArg_struct _tmp82B;_tmp82B.tag=0;_tmp82B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp812));({struct Cyc_String_pa_PrintArg_struct _tmp82A;_tmp82A.tag=0;_tmp82A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp813));({void*_tmp828[2]={& _tmp82A,& _tmp82B};Cyc_Tcutil_terr(loc,({const char*_tmp829="unbound field %s in type datatype %s";_tag_dyneither(_tmp829,sizeof(char),37);}),_tag_dyneither(_tmp828,sizeof(void*),2));});});});{
struct Cyc_Tcutil_CVTEnv _tmp82C=cvtenv;_npop_handler(1);return _tmp82C;};}_LL6E2:;}
# 4092
;_pop_region(r);}
# 4091
;_pop_handler();}else{void*_tmp81E=(void*)_exn_thrown;void*_tmp82D=_tmp81E;void*_tmp82E;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp82D)->tag == Cyc_Dict_Absent){_LL6E8: _LL6E9:
# 4107
({struct Cyc_String_pa_PrintArg_struct _tmp832;_tmp832.tag=0;_tmp832.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp812));({struct Cyc_String_pa_PrintArg_struct _tmp831;_tmp831.tag=0;_tmp831.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp813));({void*_tmp82F[2]={& _tmp831,& _tmp832};Cyc_Tcutil_terr(loc,({const char*_tmp830="unbound field %s in type datatype %s";_tag_dyneither(_tmp830,sizeof(char),37);}),_tag_dyneither(_tmp82F,sizeof(void*),2));});});});
return cvtenv;}else{_LL6EA: _tmp82E=_tmp82D;_LL6EB:(int)_rethrow(_tmp82E);}_LL6E7:;}};}
# 4111
*_tmp7B9=Cyc_Absyn_KnownDatatypefield(tud,tuf);
_tmp810=tud;_tmp811=tuf;goto _LL6DC;}}else{_LL6DB: _tmp810=((_tmp80F.KnownDatatypefield).val).f1;_tmp811=((_tmp80F.KnownDatatypefield).val).f2;_LL6DC: {
# 4115
struct Cyc_List_List*tvs=_tmp810->tvs;
for(0;_tmp7BA != 0  && tvs != 0;(_tmp7BA=_tmp7BA->tl,tvs=tvs->tl)){
void*t=(void*)_tmp7BA->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 4121
{struct _tuple0 _tmp833=({struct _tuple0 _tmp836;_tmp836.f1=Cyc_Absyn_compress_kb(tv->kind);_tmp836.f2=t;_tmp836;});struct _tuple0 _tmp834=_tmp833;struct Cyc_Absyn_Tvar*_tmp835;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp834.f1)->tag == 1){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp834.f2)->tag == 2){_LL6ED: _tmp835=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp834.f2)->f1;_LL6EE:
# 4123
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp835);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp835,1);
continue;}else{goto _LL6EF;}}else{_LL6EF: _LL6F0:
 goto _LL6EC;}_LL6EC:;}{
# 4128
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 4132
if(_tmp7BA != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp83A;_tmp83A.tag=0;_tmp83A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp811->name));({struct Cyc_String_pa_PrintArg_struct _tmp839;_tmp839.tag=0;_tmp839.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp810->name));({void*_tmp837[2]={& _tmp839,& _tmp83A};Cyc_Tcutil_terr(loc,({const char*_tmp838="too many type arguments for datatype %s.%s";_tag_dyneither(_tmp838,sizeof(char),43);}),_tag_dyneither(_tmp837,sizeof(void*),2));});});});
if(tvs != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp83E;_tmp83E.tag=0;_tmp83E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp811->name));({struct Cyc_String_pa_PrintArg_struct _tmp83D;_tmp83D.tag=0;_tmp83D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp810->name));({void*_tmp83B[2]={& _tmp83D,& _tmp83E};Cyc_Tcutil_terr(loc,({const char*_tmp83C="too few type arguments for datatype %s.%s";_tag_dyneither(_tmp83C,sizeof(char),42);}),_tag_dyneither(_tmp83B,sizeof(void*),2));});});});
goto _LL6D8;}}_LL6D8:;}
# 4140
goto _LL673;case 5: _LL684: _tmp7B3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp78E)->f1).elt_typ;_tmp7B4=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp78E)->f1).elt_tq;_tmp7B5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp78E)->f1).ptr_atts).rgn;_tmp7B6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp78E)->f1).ptr_atts).nullable;_tmp7B7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp78E)->f1).ptr_atts).bounds;_tmp7B8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp78E)->f1).ptr_atts).zero_term;_LL685: {
# 4143
int is_zero_terminated;
# 4145
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp7B3,1,1);
_tmp7B4->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp7B4->print_const,_tmp7B3);{
struct Cyc_Absyn_Kind*k;
{enum Cyc_Absyn_AliasQual _tmp83F=expected_kind->aliasqual;enum Cyc_Absyn_AliasQual _tmp840=_tmp83F;switch(_tmp840){case Cyc_Absyn_Aliasable: _LL6F2: _LL6F3:
 k=& Cyc_Tcutil_rk;goto _LL6F1;case Cyc_Absyn_Unique: _LL6F4: _LL6F5:
 k=& Cyc_Tcutil_urk;goto _LL6F1;default: _LL6F6: _LL6F7:
 k=& Cyc_Tcutil_trk;goto _LL6F1;}_LL6F1:;}
# 4153
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmp7B5,1,1);
{union Cyc_Absyn_Constraint*_tmp841=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp7B8);union Cyc_Absyn_Constraint*_tmp842=_tmp841;int _tmp843;switch((((union Cyc_Absyn_Constraint*)_tmp842)->Eq_constr).tag){case 3: _LL6F9: _LL6FA:
# 4158
{void*_tmp844=Cyc_Tcutil_compress(_tmp7B3);void*_tmp845=_tmp844;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp845)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp845)->f2 == Cyc_Absyn_Char_sz){_LL700: _LL701:
# 4160
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp7B8,Cyc_Absyn_true_conref);
is_zero_terminated=1;
goto _LL6FF;}else{goto _LL702;}}else{_LL702: _LL703:
# 4164
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp7B8,Cyc_Absyn_false_conref);
is_zero_terminated=0;
goto _LL6FF;}_LL6FF:;}
# 4168
goto _LL6F8;case 1: _LL6FB: _tmp843=(int)(_tmp842->Eq_constr).val;if(_tmp843){_LL6FC:
# 4171
 if(!Cyc_Tcutil_admits_zero(_tmp7B3))
({struct Cyc_String_pa_PrintArg_struct _tmp848;_tmp848.tag=0;_tmp848.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp7B3));({void*_tmp846[1]={& _tmp848};Cyc_Tcutil_terr(loc,({const char*_tmp847="cannot have a pointer to zero-terminated %s elements";_tag_dyneither(_tmp847,sizeof(char),53);}),_tag_dyneither(_tmp846,sizeof(void*),1));});});
is_zero_terminated=1;
goto _LL6F8;}else{goto _LL6FD;}default: _LL6FD: _LL6FE:
# 4177
 is_zero_terminated=0;
goto _LL6F8;}_LL6F8:;}
# 4181
{void*_tmp849=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp7B7);void*_tmp84A=_tmp849;struct Cyc_Absyn_Exp*_tmp84B;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp84A)->tag == 0){_LL705: _LL706:
 goto _LL704;}else{_LL707: _tmp84B=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp84A)->f1;_LL708: {
# 4184
struct _RegionHandle _tmp84C=_new_region("temp");struct _RegionHandle*temp=& _tmp84C;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp84D=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp84D,0,_tmp84B);}
# 4188
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp84B,te,cvtenv);
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp84B))
({void*_tmp84E=0;Cyc_Tcutil_terr(loc,({const char*_tmp84F="pointer bounds expression is not an unsigned int";_tag_dyneither(_tmp84F,sizeof(char),49);}),_tag_dyneither(_tmp84E,sizeof(void*),0));});{
struct _tuple14 _tmp850=Cyc_Evexp_eval_const_uint_exp(_tmp84B);struct _tuple14 _tmp851=_tmp850;unsigned int _tmp852;int _tmp853;_LL70A: _tmp852=_tmp851.f1;_tmp853=_tmp851.f2;_LL70B:;
if(is_zero_terminated  && (!_tmp853  || _tmp852 < 1))
({void*_tmp854=0;Cyc_Tcutil_terr(loc,({const char*_tmp855="zero-terminated pointer cannot point to empty sequence";_tag_dyneither(_tmp855,sizeof(char),55);}),_tag_dyneither(_tmp854,sizeof(void*),0));});
_npop_handler(0);goto _LL704;};
# 4184
;_pop_region(temp);}}_LL704:;}
# 4196
goto _LL673;};}case 19: _LL686: _tmp7B2=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp78E)->f1;_LL687:
# 4198
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp7B2,1,1);goto _LL673;case 28: _LL688: _LL689:
 goto _LL673;case 18: _LL68A: _tmp7B1=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp78E)->f1;_LL68B: {
# 4204
struct _RegionHandle _tmp856=_new_region("temp");struct _RegionHandle*temp=& _tmp856;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp857=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp857,0,_tmp7B1);}
# 4208
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp7B1))
({void*_tmp858=0;Cyc_Tcutil_terr(loc,({const char*_tmp859="valueof_t requires an int expression";_tag_dyneither(_tmp859,sizeof(char),37);}),_tag_dyneither(_tmp858,sizeof(void*),0));});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7B1,te,cvtenv);
_npop_handler(0);goto _LL673;
# 4204
;_pop_region(temp);}case 27: _LL68C: _tmp7B0=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp78E)->f1;_LL68D: {
# 4216
struct _RegionHandle _tmp85A=_new_region("temp");struct _RegionHandle*temp=& _tmp85A;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp85B=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp85B,0,_tmp7B0);}
# 4220
_npop_handler(0);goto _LL673;
# 4216
;_pop_region(temp);}case 6: _LL68E: _LL68F:
# 4221
 goto _LL691;case 7: _LL690: _LL691:
 goto _LL673;case 8: _LL692: _tmp7AB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp78E)->f1).elt_type;_tmp7AC=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp78E)->f1).tq;_tmp7AD=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp78E)->f1).num_elts;_tmp7AE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp78E)->f1).zero_term;_tmp7AF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp78E)->f1).zt_loc;_LL693: {
# 4226
struct Cyc_Absyn_Exp*_tmp85C=*_tmp7AD;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp7AB,1,allow_abs_aggr);
_tmp7AC->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp7AC->print_const,_tmp7AB);{
int is_zero_terminated;
{union Cyc_Absyn_Constraint*_tmp85D=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp7AE);union Cyc_Absyn_Constraint*_tmp85E=_tmp85D;int _tmp85F;switch((((union Cyc_Absyn_Constraint*)_tmp85E)->Eq_constr).tag){case 3: _LL70D: _LL70E:
# 4233
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp7AE,Cyc_Absyn_false_conref);
is_zero_terminated=0;
# 4248 "tcutil.cyc"
goto _LL70C;case 1: _LL70F: _tmp85F=(int)(_tmp85E->Eq_constr).val;if(_tmp85F){_LL710:
# 4251
 if(!Cyc_Tcutil_admits_zero(_tmp7AB))
({struct Cyc_String_pa_PrintArg_struct _tmp862;_tmp862.tag=0;_tmp862.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp7AB));({void*_tmp860[1]={& _tmp862};Cyc_Tcutil_terr(loc,({const char*_tmp861="cannot have a zero-terminated array of %s elements";_tag_dyneither(_tmp861,sizeof(char),51);}),_tag_dyneither(_tmp860,sizeof(void*),1));});});
is_zero_terminated=1;
goto _LL70C;}else{goto _LL711;}default: _LL711: _LL712:
# 4257
 is_zero_terminated=0;
goto _LL70C;}_LL70C:;}
# 4262
if(_tmp85C == 0){
# 4264
if(is_zero_terminated)
# 4266
*_tmp7AD=(_tmp85C=Cyc_Absyn_uint_exp(1,0));else{
# 4269
({void*_tmp863=0;Cyc_Tcutil_warn(loc,({const char*_tmp864="array bound defaults to 1 here";_tag_dyneither(_tmp864,sizeof(char),31);}),_tag_dyneither(_tmp863,sizeof(void*),0));});
*_tmp7AD=(_tmp85C=Cyc_Absyn_uint_exp(1,0));}}{
# 4273
struct _RegionHandle _tmp865=_new_region("temp");struct _RegionHandle*temp=& _tmp865;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp866=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp866,0,_tmp85C);}
# 4277
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp85C))
({void*_tmp867=0;Cyc_Tcutil_terr(loc,({const char*_tmp868="array bounds expression is not an unsigned int";_tag_dyneither(_tmp868,sizeof(char),47);}),_tag_dyneither(_tmp867,sizeof(void*),0));});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp85C,te,cvtenv);{
# 4284
struct _tuple14 _tmp869=Cyc_Evexp_eval_const_uint_exp(_tmp85C);struct _tuple14 _tmp86A=_tmp869;unsigned int _tmp86B;int _tmp86C;_LL714: _tmp86B=_tmp86A.f1;_tmp86C=_tmp86A.f2;_LL715:;
# 4286
if((is_zero_terminated  && _tmp86C) && _tmp86B < 1)
({void*_tmp86D=0;Cyc_Tcutil_warn(loc,({const char*_tmp86E="zero terminated array cannot have zero elements";_tag_dyneither(_tmp86E,sizeof(char),48);}),_tag_dyneither(_tmp86D,sizeof(void*),0));});
# 4289
if((_tmp86C  && _tmp86B < 1) && Cyc_Cyclone_tovc_r){
({void*_tmp86F=0;Cyc_Tcutil_warn(loc,({const char*_tmp870="arrays with 0 elements are not supported except with gcc -- changing to 1.";_tag_dyneither(_tmp870,sizeof(char),75);}),_tag_dyneither(_tmp86F,sizeof(void*),0));});
*_tmp7AD=Cyc_Absyn_uint_exp(1,0);}
# 4293
_npop_handler(0);goto _LL673;};
# 4273
;_pop_region(temp);};};}case 9: _LL694: _tmp79E=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp78E)->f1).tvars;_tmp79F=(void**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp78E)->f1).effect;_tmp7A0=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp78E)->f1).ret_tqual;_tmp7A1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp78E)->f1).ret_typ;_tmp7A2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp78E)->f1).args;_tmp7A3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp78E)->f1).c_varargs;_tmp7A4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp78E)->f1).cyc_varargs;_tmp7A5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp78E)->f1).rgn_po;_tmp7A6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp78E)->f1).attributes;_tmp7A7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp78E)->f1).requires_clause;_tmp7A8=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp78E)->f1).requires_relns;_tmp7A9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp78E)->f1).ensures_clause;_tmp7AA=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp78E)->f1).ensures_relns;_LL695: {
# 4300
int num_convs=0;
int seen_cdecl=0;
int seen_stdcall=0;
int seen_fastcall=0;
int seen_format=0;
enum Cyc_Absyn_Format_Type ft=Cyc_Absyn_Printf_ft;
int fmt_desc_arg=-1;
int fmt_arg_start=-1;
for(0;_tmp7A6 != 0;_tmp7A6=_tmp7A6->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp7A6->hd))
({struct Cyc_String_pa_PrintArg_struct _tmp873;_tmp873.tag=0;_tmp873.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp7A6->hd));({void*_tmp871[1]={& _tmp873};Cyc_Tcutil_terr(loc,({const char*_tmp872="bad function type attribute %s";_tag_dyneither(_tmp872,sizeof(char),31);}),_tag_dyneither(_tmp871,sizeof(void*),1));});});{
void*_tmp874=(void*)_tmp7A6->hd;void*_tmp875=_tmp874;enum Cyc_Absyn_Format_Type _tmp876;int _tmp877;int _tmp878;switch(*((int*)_tmp875)){case 1: _LL717: _LL718:
# 4313
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL716;case 2: _LL719: _LL71A:
# 4315
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL716;case 3: _LL71B: _LL71C:
# 4317
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL716;case 19: _LL71D: _tmp876=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp875)->f1;_tmp877=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp875)->f2;_tmp878=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp875)->f3;_LL71E:
# 4319
 if(!seen_format){
seen_format=1;
ft=_tmp876;
fmt_desc_arg=_tmp877;
fmt_arg_start=_tmp878;}else{
# 4325
({void*_tmp879=0;Cyc_Tcutil_terr(loc,({const char*_tmp87A="function can't have multiple format attributes";_tag_dyneither(_tmp87A,sizeof(char),47);}),_tag_dyneither(_tmp879,sizeof(void*),0));});}
goto _LL716;default: _LL71F: _LL720:
 goto _LL716;}_LL716:;};}
# 4330
if(num_convs > 1)
({void*_tmp87B=0;Cyc_Tcutil_terr(loc,({const char*_tmp87C="function can't have multiple calling conventions";_tag_dyneither(_tmp87C,sizeof(char),49);}),_tag_dyneither(_tmp87B,sizeof(void*),0));});
# 4335
Cyc_Tcutil_check_unique_tvars(loc,*_tmp79E);
{struct Cyc_List_List*b=*_tmp79E;for(0;b != 0;b=b->tl){
((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{
void*_tmp87D=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp87E=_tmp87D;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp87E)->tag == 0){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp87E)->f1)->kind == Cyc_Absyn_MemKind){_LL722: _LL723:
# 4341
({struct Cyc_String_pa_PrintArg_struct _tmp881;_tmp881.tag=0;_tmp881.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name);({void*_tmp87F[1]={& _tmp881};Cyc_Tcutil_terr(loc,({const char*_tmp880="function attempts to abstract Mem type variable %s";_tag_dyneither(_tmp880,sizeof(char),51);}),_tag_dyneither(_tmp87F,sizeof(void*),1));});});
goto _LL721;}else{goto _LL724;}}else{_LL724: _LL725:
 goto _LL721;}_LL721:;};}}{
# 4349
struct _RegionHandle _tmp882=_new_region("newr");struct _RegionHandle*newr=& _tmp882;_push_region(newr);{
struct Cyc_Tcutil_CVTEnv _tmp883=({struct Cyc_Tcutil_CVTEnv _tmp916;_tmp916.r=newr;_tmp916.kind_env=cvtenv.kind_env;_tmp916.free_vars=0;_tmp916.free_evars=0;_tmp916.generalize_evars=cvtenv.generalize_evars;_tmp916.fn_result=1;_tmp916;});
# 4355
_tmp883=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp883,& Cyc_Tcutil_tmk,_tmp7A1,1,1);
_tmp7A0->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp7A0->print_const,_tmp7A1);
_tmp883.fn_result=0;
{struct Cyc_List_List*a=_tmp7A2;for(0;a != 0;a=a->tl){
struct _tuple10*_tmp884=(struct _tuple10*)a->hd;
void*_tmp885=(*_tmp884).f3;
_tmp883=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp883,& Cyc_Tcutil_tmk,_tmp885,1,1);{
int _tmp886=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp884).f2).print_const,_tmp885);
((*_tmp884).f2).real_const=_tmp886;
# 4366
if(Cyc_Tcutil_is_array(_tmp885)){
# 4369
void*_tmp887=Cyc_Absyn_new_evar(0,0);
_tmp883=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp883,& Cyc_Tcutil_rk,_tmp887,1,1);
(*_tmp884).f3=Cyc_Tcutil_promote_array(_tmp885,_tmp887,0);}};}}
# 4376
if(_tmp7A4 != 0){
if(_tmp7A3)({void*_tmp888=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp889="both c_vararg and cyc_vararg";_tag_dyneither(_tmp889,sizeof(char),29);}),_tag_dyneither(_tmp888,sizeof(void*),0));});{
struct Cyc_Absyn_VarargInfo _tmp88A=*_tmp7A4;struct Cyc_Absyn_VarargInfo _tmp88B=_tmp88A;void*_tmp88C;int _tmp88D;_LL727: _tmp88C=_tmp88B.type;_tmp88D=_tmp88B.inject;_LL728:;
_tmp883=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp883,& Cyc_Tcutil_tmk,_tmp88C,1,1);
(_tmp7A4->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmp7A4->tq).print_const,_tmp88C);
# 4382
if(_tmp88D){
void*_tmp88E=Cyc_Tcutil_compress(_tmp88C);void*_tmp88F=_tmp88E;void*_tmp890;union Cyc_Absyn_Constraint*_tmp891;union Cyc_Absyn_Constraint*_tmp892;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp88F)->tag == 5){_LL72A: _tmp890=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp88F)->f1).elt_typ;_tmp891=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp88F)->f1).ptr_atts).bounds;_tmp892=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp88F)->f1).ptr_atts).zero_term;_LL72B:
# 4385
{void*_tmp893=Cyc_Tcutil_compress(_tmp890);void*_tmp894=_tmp893;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp894)->tag == 3){_LL72F: _LL730:
# 4387
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp892))
({void*_tmp895=0;Cyc_Tcutil_terr(loc,({const char*_tmp896="can't inject into a zeroterm pointer";_tag_dyneither(_tmp896,sizeof(char),37);}),_tag_dyneither(_tmp895,sizeof(void*),0));});
{void*_tmp897=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp891);void*_tmp898=_tmp897;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp898)->tag == 0){_LL734: _LL735:
# 4391
({void*_tmp899=0;Cyc_Tcutil_terr(loc,({const char*_tmp89A="can't inject into a fat pointer to datatype";_tag_dyneither(_tmp89A,sizeof(char),44);}),_tag_dyneither(_tmp899,sizeof(void*),0));});
goto _LL733;}else{_LL736: _LL737:
 goto _LL733;}_LL733:;}
# 4395
goto _LL72E;}else{_LL731: _LL732:
({void*_tmp89B=0;Cyc_Tcutil_terr(loc,({const char*_tmp89C="can't inject a non-datatype type";_tag_dyneither(_tmp89C,sizeof(char),33);}),_tag_dyneither(_tmp89B,sizeof(void*),0));});goto _LL72E;}_LL72E:;}
# 4398
goto _LL729;}else{_LL72C: _LL72D:
({void*_tmp89D=0;Cyc_Tcutil_terr(loc,({const char*_tmp89E="expecting a datatype pointer type";_tag_dyneither(_tmp89E,sizeof(char),34);}),_tag_dyneither(_tmp89D,sizeof(void*),0));});goto _LL729;}_LL729:;}};}
# 4404
if(seen_format){
int _tmp89F=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp7A2);
if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmp89F) || fmt_arg_start < 0) || 
# 4408
(_tmp7A4 == 0  && !_tmp7A3) && fmt_arg_start != 0) || 
(_tmp7A4 != 0  || _tmp7A3) && fmt_arg_start != _tmp89F + 1)
# 4411
({void*_tmp8A0=0;Cyc_Tcutil_terr(loc,({const char*_tmp8A1="bad format descriptor";_tag_dyneither(_tmp8A1,sizeof(char),22);}),_tag_dyneither(_tmp8A0,sizeof(void*),0));});else{
# 4414
struct _tuple10 _tmp8A2=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp7A2,fmt_desc_arg - 1);struct _tuple10 _tmp8A3=_tmp8A2;void*_tmp8A4;_LL739: _tmp8A4=_tmp8A3.f3;_LL73A:;
# 4416
{void*_tmp8A5=Cyc_Tcutil_compress(_tmp8A4);void*_tmp8A6=_tmp8A5;void*_tmp8A7;union Cyc_Absyn_Constraint*_tmp8A8;union Cyc_Absyn_Constraint*_tmp8A9;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8A6)->tag == 5){_LL73C: _tmp8A7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8A6)->f1).elt_typ;_tmp8A8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8A6)->f1).ptr_atts).bounds;_tmp8A9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8A6)->f1).ptr_atts).zero_term;_LL73D:
# 4419
{struct _tuple0 _tmp8AA=({struct _tuple0 _tmp8B2;_tmp8B2.f1=Cyc_Tcutil_compress(_tmp8A7);_tmp8B2.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp8A8);_tmp8B2;});struct _tuple0 _tmp8AB=_tmp8AA;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp8AB.f1)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp8AB.f1)->f2 == Cyc_Absyn_Char_sz){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp8AB.f2)->tag == 0){_LL741: _LL742:
# 4421
 if(_tmp7A3)
({void*_tmp8AC=0;Cyc_Tcutil_terr(loc,({const char*_tmp8AD="format descriptor is not a char * type";_tag_dyneither(_tmp8AD,sizeof(char),39);}),_tag_dyneither(_tmp8AC,sizeof(void*),0));});
goto _LL740;}else{_LL743: _LL744:
# 4425
 if(!_tmp7A3)
({void*_tmp8AE=0;Cyc_Tcutil_terr(loc,({const char*_tmp8AF="format descriptor is not a char ? type";_tag_dyneither(_tmp8AF,sizeof(char),39);}),_tag_dyneither(_tmp8AE,sizeof(void*),0));});
goto _LL740;}}else{goto _LL745;}}else{_LL745: _LL746:
# 4429
({void*_tmp8B0=0;Cyc_Tcutil_terr(loc,({const char*_tmp8B1="format descriptor is not a string type";_tag_dyneither(_tmp8B1,sizeof(char),39);}),_tag_dyneither(_tmp8B0,sizeof(void*),0));});
goto _LL740;}_LL740:;}
# 4432
goto _LL73B;}else{_LL73E: _LL73F:
({void*_tmp8B3=0;Cyc_Tcutil_terr(loc,({const char*_tmp8B4="format descriptor is not a string type";_tag_dyneither(_tmp8B4,sizeof(char),39);}),_tag_dyneither(_tmp8B3,sizeof(void*),0));});goto _LL73B;}_LL73B:;}
# 4435
if(fmt_arg_start != 0  && !_tmp7A3){
# 4439
int problem;
{struct _tuple31 _tmp8B5=({struct _tuple31 _tmp8B9;_tmp8B9.f1=ft;_tmp8B9.f2=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp7A4))->type));_tmp8B9;});struct _tuple31 _tmp8B6=_tmp8B5;struct Cyc_Absyn_Datatypedecl*_tmp8B7;struct Cyc_Absyn_Datatypedecl*_tmp8B8;if(_tmp8B6.f1 == Cyc_Absyn_Printf_ft){if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp8B6.f2)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp8B6.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL748: _tmp8B8=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp8B6.f2)->f1).datatype_info).KnownDatatype).val;_LL749:
# 4442
 problem=Cyc_Absyn_qvar_cmp(_tmp8B8->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL747;}else{goto _LL74C;}}else{goto _LL74C;}}else{if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp8B6.f2)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp8B6.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL74A: _tmp8B7=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp8B6.f2)->f1).datatype_info).KnownDatatype).val;_LL74B:
# 4444
 problem=Cyc_Absyn_qvar_cmp(_tmp8B7->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL747;}else{goto _LL74C;}}else{_LL74C: _LL74D:
# 4446
 problem=1;goto _LL747;}}_LL747:;}
# 4448
if(problem)
({void*_tmp8BA=0;Cyc_Tcutil_terr(loc,({const char*_tmp8BB="format attribute and vararg types don't match";_tag_dyneither(_tmp8BB,sizeof(char),46);}),_tag_dyneither(_tmp8BA,sizeof(void*),0));});}}}
# 4456
{struct Cyc_List_List*rpo=_tmp7A5;for(0;rpo != 0;rpo=rpo->tl){
struct _tuple0*_tmp8BC=(struct _tuple0*)rpo->hd;struct _tuple0*_tmp8BD=_tmp8BC;void*_tmp8BE;void*_tmp8BF;_LL74F: _tmp8BE=_tmp8BD->f1;_tmp8BF=_tmp8BD->f2;_LL750:;
_tmp883=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp883,& Cyc_Tcutil_ek,_tmp8BE,1,1);
_tmp883=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp883,& Cyc_Tcutil_trk,_tmp8BF,1,1);}}{
# 4464
struct Cyc_Tcenv_Fenv*_tmp8C0=Cyc_Tcenv_bogus_fenv(newr,_tmp7A1,_tmp7A2);
struct Cyc_Tcenv_Tenv*_tmp8C1=({struct Cyc_Tcenv_Tenv*_tmp915=_region_malloc(newr,sizeof(*_tmp915));_tmp915->ns=te->ns;_tmp915->ae=te->ae;_tmp915->le=_tmp8C0;_tmp915->allow_valueof=1;_tmp915->in_extern_c_include=te->in_extern_c_include;_tmp915;});
struct _tuple30 _tmp8C2=Cyc_Tcutil_check_clause(loc,_tmp8C1,_tmp883,({const char*_tmp914="@requires";_tag_dyneither(_tmp914,sizeof(char),10);}),_tmp7A7);struct _tuple30 _tmp8C3=_tmp8C2;struct Cyc_Tcutil_CVTEnv _tmp8C4;struct Cyc_List_List*_tmp8C5;_LL752: _tmp8C4=_tmp8C3.f1;_tmp8C5=_tmp8C3.f2;_LL753:;
_tmp883=_tmp8C4;
*_tmp7A8=_tmp8C5;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp7A2,_tmp8C5);{
# 4471
struct _dyneither_ptr*_tmp8C6=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"return_value",sizeof(char),13);
struct Cyc_Absyn_Vardecl*_tmp8C7=({struct Cyc_Absyn_Vardecl*_tmp911=_cycalloc(sizeof(*_tmp911));_tmp911->sc=Cyc_Absyn_Public;_tmp911->name=({struct _tuple2*_tmp912=_cycalloc(sizeof(*_tmp912));_tmp912->f1=Cyc_Absyn_Loc_n;_tmp912->f2=_tmp8C6;_tmp912;});_tmp911->tq=
Cyc_Absyn_empty_tqual(0);_tmp911->type=_tmp7A1;_tmp911->initializer=0;_tmp911->rgn=
Cyc_Tcenv_curr_rgn(_tmp8C1);_tmp911->attributes=0;_tmp911->escapes=0;_tmp911;});
# 4476
_tmp8C1=Cyc_Tcenv_add_local_var(newr,0,_tmp8C1,_tmp8C7);{
struct _tuple30 _tmp8C8=Cyc_Tcutil_check_clause(loc,_tmp8C1,_tmp883,({const char*_tmp910="@ensures";_tag_dyneither(_tmp910,sizeof(char),9);}),_tmp7A9);struct _tuple30 _tmp8C9=_tmp8C8;struct Cyc_Tcutil_CVTEnv _tmp8CA;struct Cyc_List_List*_tmp8CB;_LL755: _tmp8CA=_tmp8C9.f1;_tmp8CB=_tmp8C9.f2;_LL756:;
_tmp883=_tmp8CA;
*_tmp7AA=_tmp8CB;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp7A2,_tmp8CB);
# 4482
if(*_tmp79F != 0)
_tmp883=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp883,& Cyc_Tcutil_ek,(void*)_check_null(*_tmp79F),1,1);else{
# 4485
struct Cyc_List_List*effect=0;
# 4490
{struct Cyc_List_List*tvs=_tmp883.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp8CC=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp8CD=_tmp8CC;struct Cyc_Absyn_Tvar*_tmp8CE;int _tmp8CF;_LL758: _tmp8CE=_tmp8CD.f1;_tmp8CF=_tmp8CD.f2;_LL759:;
if(!_tmp8CF)continue;{
void*_tmp8D0=Cyc_Absyn_compress_kb(_tmp8CE->kind);void*_tmp8D1=_tmp8D0;struct Cyc_Core_Opt**_tmp8D2;struct Cyc_Absyn_Kind*_tmp8D3;struct Cyc_Core_Opt**_tmp8D4;struct Cyc_Core_Opt**_tmp8D5;struct Cyc_Absyn_Kind*_tmp8D6;switch(*((int*)_tmp8D1)){case 2: _LL75B: _tmp8D5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8D1)->f1;_tmp8D6=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8D1)->f2;if(_tmp8D6->kind == Cyc_Absyn_RgnKind){_LL75C:
# 4496
 if(_tmp8D6->aliasqual == Cyc_Absyn_Top){
*_tmp8D5=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);_tmp8D3=_tmp8D6;goto _LL75E;}
# 4499
*_tmp8D5=Cyc_Tcutil_kind_to_bound_opt(_tmp8D6);_tmp8D3=_tmp8D6;goto _LL75E;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8D1)->f2)->kind){case Cyc_Absyn_IntKind: _LL75F: _LL760:
# 4502
 goto _LL762;case Cyc_Absyn_EffKind: _LL763: _tmp8D4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8D1)->f1;_LL764:
# 4505
*_tmp8D4=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);goto _LL766;default: goto _LL769;}}case 0: _LL75D: _tmp8D3=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp8D1)->f1;if(_tmp8D3->kind == Cyc_Absyn_RgnKind){_LL75E:
# 4501
 effect=({struct Cyc_List_List*_tmp8D7=_cycalloc(sizeof(*_tmp8D7));_tmp8D7->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp8D8=_cycalloc(sizeof(*_tmp8D8));_tmp8D8[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp8D9;_tmp8D9.tag=23;_tmp8D9.f1=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8DA=_cycalloc(sizeof(*_tmp8DA));_tmp8DA[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8DB;_tmp8DB.tag=2;_tmp8DB.f1=_tmp8CE;_tmp8DB;});_tmp8DA;});_tmp8D9;});_tmp8D8;});_tmp8D7->tl=effect;_tmp8D7;});goto _LL75A;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp8D1)->f1)->kind){case Cyc_Absyn_IntKind: _LL761: _LL762:
# 4503
 goto _LL75A;case Cyc_Absyn_EffKind: _LL765: _LL766:
# 4507
 effect=({struct Cyc_List_List*_tmp8DC=_cycalloc(sizeof(*_tmp8DC));_tmp8DC->hd=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8DD=_cycalloc(sizeof(*_tmp8DD));_tmp8DD[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8DE;_tmp8DE.tag=2;_tmp8DE.f1=_tmp8CE;_tmp8DE;});_tmp8DD;});_tmp8DC->tl=effect;_tmp8DC;});goto _LL75A;default: _LL769: _LL76A:
# 4512
 effect=({struct Cyc_List_List*_tmp8E2=_cycalloc(sizeof(*_tmp8E2));_tmp8E2->hd=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp8E3=_cycalloc(sizeof(*_tmp8E3));_tmp8E3[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp8E4;_tmp8E4.tag=25;_tmp8E4.f1=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8E5=_cycalloc(sizeof(*_tmp8E5));_tmp8E5[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8E6;_tmp8E6.tag=2;_tmp8E6.f1=_tmp8CE;_tmp8E6;});_tmp8E5;});_tmp8E4;});_tmp8E3;});_tmp8E2->tl=effect;_tmp8E2;});goto _LL75A;}}default: _LL767: _tmp8D2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp8D1)->f1;_LL768:
# 4509
*_tmp8D2=({struct Cyc_Core_Opt*_tmp8DF=_cycalloc(sizeof(*_tmp8DF));_tmp8DF->v=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8E0=_cycalloc(sizeof(*_tmp8E0));_tmp8E0[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp8E1;_tmp8E1.tag=2;_tmp8E1.f1=0;_tmp8E1.f2=& Cyc_Tcutil_ak;_tmp8E1;});_tmp8E0;});_tmp8DF;});goto _LL76A;}_LL75A:;};}}
# 4516
{struct Cyc_List_List*ts=_tmp883.free_evars;for(0;ts != 0;ts=ts->tl){
struct _tuple29 _tmp8E7=*((struct _tuple29*)ts->hd);struct _tuple29 _tmp8E8=_tmp8E7;void*_tmp8E9;int _tmp8EA;_LL76C: _tmp8E9=_tmp8E8.f1;_tmp8EA=_tmp8E8.f2;_LL76D:;
if(!_tmp8EA)continue;{
struct Cyc_Absyn_Kind*_tmp8EB=Cyc_Tcutil_typ_kind(_tmp8E9);struct Cyc_Absyn_Kind*_tmp8EC=_tmp8EB;switch(((struct Cyc_Absyn_Kind*)_tmp8EC)->kind){case Cyc_Absyn_RgnKind: _LL76F: _LL770:
# 4521
 effect=({struct Cyc_List_List*_tmp8ED=_cycalloc(sizeof(*_tmp8ED));_tmp8ED->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp8EE=_cycalloc(sizeof(*_tmp8EE));_tmp8EE[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp8EF;_tmp8EF.tag=23;_tmp8EF.f1=_tmp8E9;_tmp8EF;});_tmp8EE;});_tmp8ED->tl=effect;_tmp8ED;});goto _LL76E;case Cyc_Absyn_EffKind: _LL771: _LL772:
# 4523
 effect=({struct Cyc_List_List*_tmp8F0=_cycalloc(sizeof(*_tmp8F0));_tmp8F0->hd=_tmp8E9;_tmp8F0->tl=effect;_tmp8F0;});goto _LL76E;case Cyc_Absyn_IntKind: _LL773: _LL774:
 goto _LL76E;default: _LL775: _LL776:
# 4526
 effect=({struct Cyc_List_List*_tmp8F1=_cycalloc(sizeof(*_tmp8F1));_tmp8F1->hd=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp8F2=_cycalloc(sizeof(*_tmp8F2));_tmp8F2[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp8F3;_tmp8F3.tag=25;_tmp8F3.f1=_tmp8E9;_tmp8F3;});_tmp8F2;});_tmp8F1->tl=effect;_tmp8F1;});goto _LL76E;}_LL76E:;};}}
# 4529
*_tmp79F=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp8F4=_cycalloc(sizeof(*_tmp8F4));_tmp8F4[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp8F5;_tmp8F5.tag=24;_tmp8F5.f1=effect;_tmp8F5;});_tmp8F4;});}
# 4535
if(*_tmp79E != 0){
struct Cyc_List_List*bs=*_tmp79E;for(0;bs != 0;bs=bs->tl){
void*_tmp8F6=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);void*_tmp8F7=_tmp8F6;struct Cyc_Core_Opt**_tmp8F8;struct Cyc_Absyn_Kind*_tmp8F9;struct Cyc_Core_Opt**_tmp8FA;struct Cyc_Core_Opt**_tmp8FB;struct Cyc_Core_Opt**_tmp8FC;struct Cyc_Core_Opt**_tmp8FD;struct Cyc_Core_Opt**_tmp8FE;struct Cyc_Core_Opt**_tmp8FF;struct Cyc_Core_Opt**_tmp900;struct Cyc_Core_Opt**_tmp901;struct Cyc_Core_Opt**_tmp902;switch(*((int*)_tmp8F7)){case 1: _LL778: _tmp902=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp8F7)->f1;_LL779:
# 4539
({struct Cyc_String_pa_PrintArg_struct _tmp905;_tmp905.tag=0;_tmp905.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name);({void*_tmp903[1]={& _tmp905};Cyc_Tcutil_warn(loc,({const char*_tmp904="Type variable %s unconstrained, assuming boxed";_tag_dyneither(_tmp904,sizeof(char),47);}),_tag_dyneither(_tmp903,sizeof(void*),1));});});
# 4541
_tmp901=_tmp902;goto _LL77B;case 2: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F7)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F7)->f2)->aliasqual == Cyc_Absyn_Top){_LL77A: _tmp901=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F7)->f1;_LL77B:
 _tmp900=_tmp901;goto _LL77D;}else{goto _LL78A;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F7)->f2)->aliasqual){case Cyc_Absyn_Top: _LL77C: _tmp900=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F7)->f1;_LL77D:
 _tmp8FF=_tmp900;goto _LL77F;case Cyc_Absyn_Aliasable: _LL77E: _tmp8FF=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F7)->f1;_LL77F:
 _tmp8FD=_tmp8FF;goto _LL781;default: _LL784: _tmp8FE=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F7)->f1;_LL785:
# 4548
 _tmp8FB=_tmp8FE;goto _LL787;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F7)->f2)->aliasqual){case Cyc_Absyn_Top: _LL780: _tmp8FD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F7)->f1;_LL781:
# 4545
 _tmp8FC=_tmp8FD;goto _LL783;case Cyc_Absyn_Aliasable: _LL782: _tmp8FC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F7)->f1;_LL783:
# 4547
*_tmp8FC=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL777;default: _LL786: _tmp8FB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F7)->f1;_LL787:
# 4550
*_tmp8FB=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL777;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F7)->f2)->aliasqual == Cyc_Absyn_Top){_LL788: _tmp8FA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F7)->f1;_LL789:
# 4552
*_tmp8FA=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL777;}else{goto _LL78A;}default: _LL78A: _tmp8F8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F7)->f1;_tmp8F9=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F7)->f2;_LL78B:
# 4554
*_tmp8F8=Cyc_Tcutil_kind_to_bound_opt(_tmp8F9);goto _LL777;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp8F7)->f1)->kind == Cyc_Absyn_MemKind){_LL78C: _LL78D:
# 4556
({void*_tmp906=0;Cyc_Tcutil_terr(loc,({const char*_tmp907="functions can't abstract M types";_tag_dyneither(_tmp907,sizeof(char),33);}),_tag_dyneither(_tmp906,sizeof(void*),0));});goto _LL777;}else{_LL78E: _LL78F:
 goto _LL777;}}_LL777:;}}
# 4561
cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,_tmp883.kind_env,*_tmp79E);
_tmp883.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(_tmp883.r,_tmp883.free_vars,*_tmp79E);
# 4564
{struct Cyc_List_List*tvs=_tmp883.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp908=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp909=_tmp908;struct Cyc_Absyn_Tvar*_tmp90A;int _tmp90B;_LL791: _tmp90A=_tmp909.f1;_tmp90B=_tmp909.f2;_LL792:;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp90A,_tmp90B);}}{
# 4569
struct Cyc_List_List*evs=_tmp883.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmp90C=*((struct _tuple29*)evs->hd);struct _tuple29 _tmp90D=_tmp90C;void*_tmp90E;int _tmp90F;_LL794: _tmp90E=_tmp90D.f1;_tmp90F=_tmp90D.f2;_LL795:;
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmp90E,_tmp90F);}};};};};}
# 4574
_npop_handler(0);goto _LL673;
# 4349
;_pop_region(newr);};}case 10: _LL696: _tmp79D=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp78E)->f1;_LL697:
# 4577
 for(0;_tmp79D != 0;_tmp79D=_tmp79D->tl){
struct _tuple12*_tmp917=(struct _tuple12*)_tmp79D->hd;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmp917).f2,1,0);
((*_tmp917).f1).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp917).f1).print_const,(*_tmp917).f2);}
# 4583
goto _LL673;case 12: _LL698: _tmp79B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp78E)->f1;_tmp79C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp78E)->f2;_LL699: {
# 4587
struct _RegionHandle _tmp918=_new_region("aprev_rgn");struct _RegionHandle*aprev_rgn=& _tmp918;_push_region(aprev_rgn);{
struct Cyc_List_List*prev_fields=0;
for(0;_tmp79C != 0;_tmp79C=_tmp79C->tl){
struct Cyc_Absyn_Aggrfield*_tmp919=(struct Cyc_Absyn_Aggrfield*)_tmp79C->hd;struct Cyc_Absyn_Aggrfield*_tmp91A=_tmp919;struct _dyneither_ptr*_tmp91B;struct Cyc_Absyn_Tqual*_tmp91C;void*_tmp91D;struct Cyc_Absyn_Exp*_tmp91E;struct Cyc_List_List*_tmp91F;struct Cyc_Absyn_Exp*_tmp920;_LL797: _tmp91B=_tmp91A->name;_tmp91C=(struct Cyc_Absyn_Tqual*)& _tmp91A->tq;_tmp91D=_tmp91A->type;_tmp91E=_tmp91A->width;_tmp91F=_tmp91A->attributes;_tmp920=_tmp91A->requires_clause;_LL798:;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp91B))
({struct Cyc_String_pa_PrintArg_struct _tmp923;_tmp923.tag=0;_tmp923.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp91B);({void*_tmp921[1]={& _tmp923};Cyc_Tcutil_terr(loc,({const char*_tmp922="duplicate field %s";_tag_dyneither(_tmp922,sizeof(char),19);}),_tag_dyneither(_tmp921,sizeof(void*),1));});});
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp91B,({const char*_tmp924="";_tag_dyneither(_tmp924,sizeof(char),1);}))!= 0)
prev_fields=({struct Cyc_List_List*_tmp925=_region_malloc(aprev_rgn,sizeof(*_tmp925));_tmp925->hd=_tmp91B;_tmp925->tl=prev_fields;_tmp925;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp91D,1,0);
_tmp91C->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp91C->print_const,_tmp91D);
# 4601
Cyc_Tcutil_check_bitfield(loc,te,_tmp91D,_tmp91E,_tmp91B);
Cyc_Tcutil_check_field_atts(loc,_tmp91B,_tmp91F);
if(_tmp920 != 0){
# 4605
if(_tmp79B != Cyc_Absyn_UnionA)
({void*_tmp926=0;Cyc_Tcutil_terr(loc,({const char*_tmp927="@requires clause is only allowed on union members";_tag_dyneither(_tmp927,sizeof(char),50);}),_tag_dyneither(_tmp926,sizeof(void*),0));});{
struct _RegionHandle _tmp928=_new_region("temp");struct _RegionHandle*temp=& _tmp928;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp929=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp929,0,_tmp920);}
# 4611
if(!Cyc_Tcutil_is_integral(_tmp920))
({struct Cyc_String_pa_PrintArg_struct _tmp92C;_tmp92C.tag=0;_tmp92C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmp920->topt)));({void*_tmp92A[1]={& _tmp92C};Cyc_Tcutil_terr(loc,({const char*_tmp92B="@requires clause has type %s instead of integral type";_tag_dyneither(_tmp92B,sizeof(char),54);}),_tag_dyneither(_tmp92A,sizeof(void*),1));});});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp920,te,cvtenv);
# 4607
;_pop_region(temp);};}}}
# 4618
_npop_handler(0);goto _LL673;
# 4587
;_pop_region(aprev_rgn);}case 11: _LL69A: _tmp799=(union Cyc_Absyn_AggrInfoU*)&(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp78E)->f1).aggr_info;_tmp79A=(struct Cyc_List_List**)&(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp78E)->f1).targs;_LL69B:
# 4621
{union Cyc_Absyn_AggrInfoU _tmp92D=*_tmp799;union Cyc_Absyn_AggrInfoU _tmp92E=_tmp92D;struct Cyc_Absyn_Aggrdecl*_tmp92F;enum Cyc_Absyn_AggrKind _tmp930;struct _tuple2*_tmp931;struct Cyc_Core_Opt*_tmp932;if((_tmp92E.UnknownAggr).tag == 1){_LL79A: _tmp930=((_tmp92E.UnknownAggr).val).f1;_tmp931=((_tmp92E.UnknownAggr).val).f2;_tmp932=((_tmp92E.UnknownAggr).val).f3;_LL79B: {
# 4623
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmp933;_push_handler(& _tmp933);{int _tmp935=0;if(setjmp(_tmp933.handler))_tmp935=1;if(!_tmp935){
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp931);{
struct Cyc_Absyn_Aggrdecl*_tmp936=*adp;
if(_tmp936->kind != _tmp930){
if(_tmp936->kind == Cyc_Absyn_StructA)
({struct Cyc_String_pa_PrintArg_struct _tmp93A;_tmp93A.tag=0;_tmp93A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp931));({struct Cyc_String_pa_PrintArg_struct _tmp939;_tmp939.tag=0;_tmp939.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4629
Cyc_Absynpp_qvar2string(_tmp931));({void*_tmp937[2]={& _tmp939,& _tmp93A};Cyc_Tcutil_terr(loc,({const char*_tmp938="expecting struct %s instead of union %s";_tag_dyneither(_tmp938,sizeof(char),40);}),_tag_dyneither(_tmp937,sizeof(void*),2));});});});else{
# 4632
({struct Cyc_String_pa_PrintArg_struct _tmp93E;_tmp93E.tag=0;_tmp93E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp931));({struct Cyc_String_pa_PrintArg_struct _tmp93D;_tmp93D.tag=0;_tmp93D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4632
Cyc_Absynpp_qvar2string(_tmp931));({void*_tmp93B[2]={& _tmp93D,& _tmp93E};Cyc_Tcutil_terr(loc,({const char*_tmp93C="expecting union %s instead of struct %s";_tag_dyneither(_tmp93C,sizeof(char),40);}),_tag_dyneither(_tmp93B,sizeof(void*),2));});});});}}
# 4635
if((unsigned int)_tmp932  && (int)_tmp932->v){
if(!((unsigned int)_tmp936->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp936->impl))->tagged)
({struct Cyc_String_pa_PrintArg_struct _tmp941;_tmp941.tag=0;_tmp941.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp931));({void*_tmp93F[1]={& _tmp941};Cyc_Tcutil_terr(loc,({const char*_tmp940="@tagged qualfiers don't agree on union %s";_tag_dyneither(_tmp940,sizeof(char),42);}),_tag_dyneither(_tmp93F,sizeof(void*),1));});});}
# 4641
*_tmp799=Cyc_Absyn_KnownAggr(adp);};
# 4625
;_pop_handler();}else{void*_tmp934=(void*)_exn_thrown;void*_tmp942=_tmp934;void*_tmp943;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp942)->tag == Cyc_Dict_Absent){_LL79F: _LL7A0: {
# 4645
struct Cyc_Tcenv_Genv*_tmp944=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp945=({struct Cyc_Absyn_Aggrdecl*_tmp949=_cycalloc(sizeof(*_tmp949));_tmp949->kind=_tmp930;_tmp949->sc=Cyc_Absyn_Extern;_tmp949->name=_tmp931;_tmp949->tvs=0;_tmp949->impl=0;_tmp949->attributes=0;_tmp949->expected_mem_kind=0;_tmp949;});
Cyc_Tc_tcAggrdecl(te,_tmp944,loc,_tmp945);
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp931);
*_tmp799=Cyc_Absyn_KnownAggr(adp);
# 4651
if(*_tmp79A != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp948;_tmp948.tag=0;_tmp948.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp931));({void*_tmp946[1]={& _tmp948};Cyc_Tcutil_terr(loc,({const char*_tmp947="declare parameterized type %s before using";_tag_dyneither(_tmp947,sizeof(char),43);}),_tag_dyneither(_tmp946,sizeof(void*),1));});});
return cvtenv;}
# 4656
goto _LL79E;}}else{_LL7A1: _tmp943=_tmp942;_LL7A2:(int)_rethrow(_tmp943);}_LL79E:;}};}
# 4658
_tmp92F=*adp;goto _LL79D;}}else{_LL79C: _tmp92F=*(_tmp92E.KnownAggr).val;_LL79D: {
# 4660
struct Cyc_List_List*tvs=_tmp92F->tvs;
struct Cyc_List_List*ts=*_tmp79A;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Tvar*_tmp94A=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp94B=(void*)ts->hd;
# 4668
{struct _tuple0 _tmp94C=({struct _tuple0 _tmp94F;_tmp94F.f1=Cyc_Absyn_compress_kb(_tmp94A->kind);_tmp94F.f2=_tmp94B;_tmp94F;});struct _tuple0 _tmp94D=_tmp94C;struct Cyc_Absyn_Tvar*_tmp94E;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp94D.f1)->tag == 1){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp94D.f2)->tag == 2){_LL7A4: _tmp94E=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp94D.f2)->f1;_LL7A5:
# 4670
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp94E);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp94E,1);
continue;}else{goto _LL7A6;}}else{_LL7A6: _LL7A7:
 goto _LL7A3;}_LL7A3:;}{
# 4675
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 4679
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp952;_tmp952.tag=0;_tmp952.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp92F->name));({void*_tmp950[1]={& _tmp952};Cyc_Tcutil_terr(loc,({const char*_tmp951="too many parameters for type %s";_tag_dyneither(_tmp951,sizeof(char),32);}),_tag_dyneither(_tmp950,sizeof(void*),1));});});
if(tvs != 0){
# 4683
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp953=_cycalloc(sizeof(*_tmp953));_tmp953->hd=e;_tmp953->tl=hidden_ts;_tmp953;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);}
# 4690
*_tmp79A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp79A,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4692
if((allow_abs_aggr  && _tmp92F->impl == 0) && !
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,expected_kind))
# 4697
_tmp92F->expected_mem_kind=1;}}_LL799:;}
# 4700
goto _LL673;case 17: _LL69C: _tmp795=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp78E)->f1;_tmp796=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp78E)->f2;_tmp797=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp78E)->f3;_tmp798=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp78E)->f4;_LL69D: {
# 4703
struct Cyc_List_List*targs=*_tmp796;
# 4705
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmp954;_push_handler(& _tmp954);{int _tmp956=0;if(setjmp(_tmp954.handler))_tmp956=1;if(!_tmp956){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp795);;_pop_handler();}else{void*_tmp955=(void*)_exn_thrown;void*_tmp957=_tmp955;void*_tmp958;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp957)->tag == Cyc_Dict_Absent){_LL7A9: _LL7AA:
# 4708
({struct Cyc_String_pa_PrintArg_struct _tmp95B;_tmp95B.tag=0;_tmp95B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp795));({void*_tmp959[1]={& _tmp95B};Cyc_Tcutil_terr(loc,({const char*_tmp95A="unbound typedef name %s";_tag_dyneither(_tmp95A,sizeof(char),24);}),_tag_dyneither(_tmp959,sizeof(void*),1));});});
return cvtenv;}else{_LL7AB: _tmp958=_tmp957;_LL7AC:(int)_rethrow(_tmp958);}_LL7A8:;}};}
# 4711
*_tmp797=td;
# 4713
_tmp795[0]=(td->name)[0];{
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;
struct _RegionHandle _tmp95C=_new_region("temp");struct _RegionHandle*temp=& _tmp95C;_push_region(temp);{
struct Cyc_List_List*inst=0;
# 4719
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,td);
# 4724
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
inst=({struct Cyc_List_List*_tmp95D=_region_malloc(temp,sizeof(*_tmp95D));_tmp95D->hd=({struct _tuple16*_tmp95E=_region_malloc(temp,sizeof(*_tmp95E));_tmp95E->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp95E->f2=(void*)ts->hd;_tmp95E;});_tmp95D->tl=inst;_tmp95D;});}
# 4728
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp961;_tmp961.tag=0;_tmp961.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp795));({void*_tmp95F[1]={& _tmp961};Cyc_Tcutil_terr(loc,({const char*_tmp960="too many parameters for typedef %s";_tag_dyneither(_tmp960,sizeof(char),35);}),_tag_dyneither(_tmp95F,sizeof(void*),1));});});
if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;
# 4733
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp962=_cycalloc(sizeof(*_tmp962));_tmp962->hd=e;_tmp962->tl=hidden_ts;_tmp962;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);
inst=({struct Cyc_List_List*_tmp963=_cycalloc(sizeof(*_tmp963));_tmp963->hd=({struct _tuple16*_tmp964=_cycalloc(sizeof(*_tmp964));_tmp964->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp964->f2=e;_tmp964;});_tmp963->tl=inst;_tmp963;});}
# 4741
*_tmp796=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4743
if(td->defn != 0){
void*new_typ=
inst == 0?(void*)_check_null(td->defn):
 Cyc_Tcutil_rsubstitute(temp,inst,(void*)_check_null(td->defn));
*_tmp798=new_typ;}}
# 4750
_npop_handler(0);goto _LL673;
# 4716
;_pop_region(temp);};}case 22: _LL69E: _LL69F:
# 4751
 goto _LL6A1;case 21: _LL6A0: _LL6A1:
 goto _LL6A3;case 20: _LL6A2: _LL6A3:
 goto _LL673;case 15: _LL6A4: _tmp794=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp78E)->f1;_LL6A5:
# 4755
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp794,1,allow_abs_aggr);goto _LL673;case 16: _LL6A6: _tmp792=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp78E)->f1;_tmp793=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp78E)->f2;_LL6A7:
# 4758
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp792,0,0);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp793,1,0);
goto _LL673;case 23: _LL6A8: _tmp791=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp78E)->f1;_LL6A9:
# 4762
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp791,1,1);goto _LL673;case 25: _LL6AA: _tmp790=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp78E)->f1;_LL6AB:
# 4764
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp790,1,1);goto _LL673;default: _LL6AC: _tmp78F=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp78E)->f1;_LL6AD:
# 4766
 for(0;_tmp78F != 0;_tmp78F=_tmp78F->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmp78F->hd,1,1);}
goto _LL673;}_LL673:;}
# 4770
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind))
({struct Cyc_String_pa_PrintArg_struct _tmp969;_tmp969.tag=0;_tmp969.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(expected_kind));({struct Cyc_String_pa_PrintArg_struct _tmp968;_tmp968.tag=0;_tmp968.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t)));({struct Cyc_String_pa_PrintArg_struct _tmp967;_tmp967.tag=0;_tmp967.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp965[3]={& _tmp967,& _tmp968,& _tmp969};Cyc_Tcutil_terr(loc,({const char*_tmp966="type %s has kind %s but as used here needs kind %s";_tag_dyneither(_tmp966,sizeof(char),51);}),_tag_dyneither(_tmp965,sizeof(void*),3));});});});});
# 4774
return cvtenv;}
# 4782
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){
# 4784
{void*_tmp96A=e->r;void*_tmp96B=_tmp96A;struct Cyc_Absyn_Exp*_tmp96C;void*_tmp96D;void*_tmp96E;void*_tmp96F;void*_tmp970;struct Cyc_Absyn_Exp*_tmp971;struct Cyc_Absyn_Exp*_tmp972;struct Cyc_Absyn_Exp*_tmp973;struct Cyc_Absyn_Exp*_tmp974;struct Cyc_Absyn_Exp*_tmp975;struct Cyc_Absyn_Exp*_tmp976;struct Cyc_Absyn_Exp*_tmp977;struct Cyc_Absyn_Exp*_tmp978;struct Cyc_Absyn_Exp*_tmp979;struct Cyc_Absyn_Exp*_tmp97A;struct Cyc_List_List*_tmp97B;switch(*((int*)_tmp96B)){case 0: _LL7AE: _LL7AF:
 goto _LL7B1;case 31: _LL7B0: _LL7B1:
 goto _LL7B3;case 32: _LL7B2: _LL7B3:
 goto _LL7B5;case 1: _LL7B4: _LL7B5:
 goto _LL7AD;case 2: _LL7B6: _tmp97B=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp96B)->f2;_LL7B7:
# 4790
 for(0;_tmp97B != 0;_tmp97B=_tmp97B->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmp97B->hd,te,cvtenv);}
goto _LL7AD;case 5: _LL7B8: _tmp978=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp96B)->f1;_tmp979=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp96B)->f2;_tmp97A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp96B)->f3;_LL7B9:
# 4794
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp978,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp979,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp97A,te,cvtenv);
goto _LL7AD;case 6: _LL7BA: _tmp976=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp96B)->f1;_tmp977=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp96B)->f2;_LL7BB:
 _tmp974=_tmp976;_tmp975=_tmp977;goto _LL7BD;case 7: _LL7BC: _tmp974=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp96B)->f1;_tmp975=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp96B)->f2;_LL7BD:
 _tmp972=_tmp974;_tmp973=_tmp975;goto _LL7BF;case 8: _LL7BE: _tmp972=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp96B)->f1;_tmp973=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp96B)->f2;_LL7BF:
# 4801
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp972,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp973,te,cvtenv);
goto _LL7AD;case 13: _LL7C0: _tmp970=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp96B)->f1;_tmp971=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp96B)->f2;_LL7C1:
# 4805
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp971,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp970,1,0);
goto _LL7AD;case 18: _LL7C2: _tmp96F=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp96B)->f1;_LL7C3:
 _tmp96E=_tmp96F;goto _LL7C5;case 16: _LL7C4: _tmp96E=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp96B)->f1;_LL7C5:
# 4810
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp96E,1,0);
goto _LL7AD;case 38: _LL7C6: _tmp96D=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp96B)->f1;_LL7C7:
# 4813
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp96D,1,0);
goto _LL7AD;case 17: _LL7C8: _tmp96C=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp96B)->f1;_LL7C9:
# 4816
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp96C,te,cvtenv);
goto _LL7AD;default: _LL7CA: _LL7CB:
# 4819
({void*_tmp97C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp97D="non-type-level-expression in Tcutil::i_check_valid_type_level_exp";_tag_dyneither(_tmp97D,sizeof(char),66);}),_tag_dyneither(_tmp97C,sizeof(void*),0));});}_LL7AD:;}
# 4821
return cvtenv;}
# 4824
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,int allow_abs_aggr,void*t){
# 4830
struct Cyc_List_List*_tmp97E=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1,allow_abs_aggr);
# 4833
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
struct _tuple28 _tmp97F=*((struct _tuple28*)vs->hd);struct _tuple28 _tmp980=_tmp97F;struct Cyc_Absyn_Tvar*_tmp981;_LL7CD: _tmp981=_tmp980.f1;_LL7CE:;
cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(_tmp97E,_tmp981);}}
# 4841
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmp982=*((struct _tuple29*)evs->hd);struct _tuple29 _tmp983=_tmp982;void*_tmp984;_LL7D0: _tmp984=_tmp983.f1;_LL7D1:;{
void*_tmp985=Cyc_Tcutil_compress(_tmp984);void*_tmp986=_tmp985;struct Cyc_Core_Opt**_tmp987;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp986)->tag == 1){_LL7D3: _tmp987=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp986)->f4;_LL7D4:
# 4845
 if(*_tmp987 == 0)
*_tmp987=({struct Cyc_Core_Opt*_tmp988=_cycalloc(sizeof(*_tmp988));_tmp988->v=_tmp97E;_tmp988;});else{
# 4849
struct Cyc_List_List*_tmp989=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp987))->v;
struct Cyc_List_List*_tmp98A=0;
for(0;_tmp989 != 0;_tmp989=_tmp989->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp97E,(struct Cyc_Absyn_Tvar*)_tmp989->hd))
_tmp98A=({struct Cyc_List_List*_tmp98B=_cycalloc(sizeof(*_tmp98B));_tmp98B->hd=(struct Cyc_Absyn_Tvar*)_tmp989->hd;_tmp98B->tl=_tmp98A;_tmp98B;});}
*_tmp987=({struct Cyc_Core_Opt*_tmp98C=_cycalloc(sizeof(*_tmp98C));_tmp98C->v=_tmp98A;_tmp98C;});}
# 4856
goto _LL7D2;}else{_LL7D5: _LL7D6:
 goto _LL7D2;}_LL7D2:;};}}
# 4860
return cvt;}
# 4866
void Cyc_Tcutil_check_free_evars(struct Cyc_List_List*free_evars,void*in_t,unsigned int loc){
# 4868
for(0;free_evars != 0;free_evars=free_evars->tl){
void*e=(void*)free_evars->hd;
struct Cyc_Absyn_Kind*_tmp98D=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind*_tmp98E=_tmp98D;switch(((struct Cyc_Absyn_Kind*)_tmp98E)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp98E)->aliasqual){case Cyc_Absyn_Unique: _LL7D8: _LL7D9:
# 4872
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val))
({void*_tmp98F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp990="can't unify evar with unique region!";_tag_dyneither(_tmp990,sizeof(char),37);}),_tag_dyneither(_tmp98F,sizeof(void*),0));});
goto _LL7D7;case Cyc_Absyn_Aliasable: _LL7DA: _LL7DB:
 goto _LL7DD;default: _LL7DC: _LL7DD:
# 4877
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val))({void*_tmp991=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp992="can't unify evar with heap!";_tag_dyneither(_tmp992,sizeof(char),28);}),_tag_dyneither(_tmp991,sizeof(void*),0));});
goto _LL7D7;}case Cyc_Absyn_EffKind: _LL7DE: _LL7DF:
# 4880
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({void*_tmp993=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp994="can't unify evar with {}!";_tag_dyneither(_tmp994,sizeof(char),26);}),_tag_dyneither(_tmp993,sizeof(void*),0));});
goto _LL7D7;default: _LL7E0: _LL7E1:
# 4883
({struct Cyc_String_pa_PrintArg_struct _tmp998;_tmp998.tag=0;_tmp998.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(in_t));({struct Cyc_String_pa_PrintArg_struct _tmp997;_tmp997.tag=0;_tmp997.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e));({void*_tmp995[2]={& _tmp997,& _tmp998};Cyc_Tcutil_terr(loc,({const char*_tmp996="hidden type variable %s isn't abstracted in type %s";_tag_dyneither(_tmp996,sizeof(char),52);}),_tag_dyneither(_tmp995,sizeof(void*),2));});});});
goto _LL7D7;}_LL7D7:;}}
# 4895
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_List_List*_tmp999=Cyc_Tcenv_lookup_type_vars(te);
struct Cyc_Absyn_Kind*expected_kind=generalize_evars?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
struct _RegionHandle _tmp99A=_new_region("temp");struct _RegionHandle*temp=& _tmp99A;_push_region(temp);
{struct Cyc_Tcutil_CVTEnv _tmp99B=Cyc_Tcutil_check_valid_type(loc,te,({struct Cyc_Tcutil_CVTEnv _tmp9B9;_tmp9B9.r=temp;_tmp9B9.kind_env=_tmp999;_tmp9B9.free_vars=0;_tmp9B9.free_evars=0;_tmp9B9.generalize_evars=generalize_evars;_tmp9B9.fn_result=0;_tmp9B9;}),expected_kind,1,t);
# 4904
struct Cyc_List_List*_tmp99C=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmp99B.free_vars);
struct Cyc_List_List*_tmp99D=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmp99B.free_evars);
# 4908
if(_tmp999 != 0){
struct Cyc_List_List*_tmp99E=0;
{struct Cyc_List_List*_tmp99F=_tmp99C;for(0;(unsigned int)_tmp99F;_tmp99F=_tmp99F->tl){
struct Cyc_Absyn_Tvar*_tmp9A0=(struct Cyc_Absyn_Tvar*)_tmp99F->hd;
int found=0;
{struct Cyc_List_List*_tmp9A1=_tmp999;for(0;(unsigned int)_tmp9A1;_tmp9A1=_tmp9A1->tl){
if(Cyc_Absyn_tvar_cmp(_tmp9A0,(struct Cyc_Absyn_Tvar*)_tmp9A1->hd)== 0){found=1;break;}}}
if(!found)
_tmp99E=({struct Cyc_List_List*_tmp9A2=_region_malloc(temp,sizeof(*_tmp9A2));_tmp9A2->hd=(struct Cyc_Absyn_Tvar*)_tmp99F->hd;_tmp9A2->tl=_tmp99E;_tmp9A2;});}}
# 4918
_tmp99C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp99E);}
# 4924
{struct Cyc_List_List*x=_tmp99C;for(0;x != 0;x=x->tl){
void*_tmp9A3=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);void*_tmp9A4=_tmp9A3;enum Cyc_Absyn_AliasQual _tmp9A5;struct Cyc_Core_Opt**_tmp9A6;struct Cyc_Absyn_Kind*_tmp9A7;struct Cyc_Core_Opt**_tmp9A8;struct Cyc_Core_Opt**_tmp9A9;struct Cyc_Core_Opt**_tmp9AA;struct Cyc_Core_Opt**_tmp9AB;struct Cyc_Core_Opt**_tmp9AC;struct Cyc_Core_Opt**_tmp9AD;switch(*((int*)_tmp9A4)){case 1: _LL7E3: _tmp9AD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp9A4)->f1;_LL7E4:
 _tmp9AC=_tmp9AD;goto _LL7E6;case 2: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9A4)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9A4)->f2)->aliasqual == Cyc_Absyn_Top){_LL7E5: _tmp9AC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9A4)->f1;_LL7E6:
 _tmp9AB=_tmp9AC;goto _LL7E8;}else{goto _LL7EF;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9A4)->f2)->aliasqual){case Cyc_Absyn_Top: _LL7E7: _tmp9AB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9A4)->f1;_LL7E8:
 _tmp9AA=_tmp9AB;goto _LL7EA;case Cyc_Absyn_Aliasable: _LL7E9: _tmp9AA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9A4)->f1;_LL7EA:
# 4930
*_tmp9AA=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL7E2;default: _LL7EB: _tmp9A9=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9A4)->f1;_LL7EC:
# 4932
*_tmp9A9=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL7E2;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9A4)->f2)->aliasqual == Cyc_Absyn_Top){_LL7ED: _tmp9A8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9A4)->f1;_LL7EE:
# 4934
*_tmp9A8=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL7E2;}else{goto _LL7EF;}default: _LL7EF: _tmp9A6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9A4)->f1;_tmp9A7=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9A4)->f2;_LL7F0:
# 4936
*_tmp9A6=Cyc_Tcutil_kind_to_bound_opt(_tmp9A7);goto _LL7E2;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp9A4)->f1)->kind == Cyc_Absyn_MemKind){_LL7F1: _tmp9A5=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp9A4)->f1)->aliasqual;_LL7F2:
# 4938
({struct Cyc_String_pa_PrintArg_struct _tmp9B1;_tmp9B1.tag=0;_tmp9B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp9B2=_cycalloc_atomic(sizeof(*_tmp9B2));_tmp9B2->kind=Cyc_Absyn_MemKind;_tmp9B2->aliasqual=_tmp9A5;_tmp9B2;})));({struct Cyc_String_pa_PrintArg_struct _tmp9B0;_tmp9B0.tag=0;_tmp9B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd));({void*_tmp9AE[2]={& _tmp9B0,& _tmp9B1};Cyc_Tcutil_terr(loc,({const char*_tmp9AF="type variable %s cannot have kind %s";_tag_dyneither(_tmp9AF,sizeof(char),37);}),_tag_dyneither(_tmp9AE,sizeof(void*),2));});});});
goto _LL7E2;}else{_LL7F3: _LL7F4:
 goto _LL7E2;}}_LL7E2:;}}
# 4945
if(_tmp99C != 0  || _tmp99D != 0){
{void*_tmp9B3=Cyc_Tcutil_compress(t);void*_tmp9B4=_tmp9B3;struct Cyc_List_List**_tmp9B5;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9B4)->tag == 9){_LL7F6: _tmp9B5=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9B4)->f1).tvars;_LL7F7:
# 4948
 if(*_tmp9B5 == 0){
# 4950
*_tmp9B5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmp99C);
_tmp99C=0;}
# 4953
goto _LL7F5;}else{_LL7F8: _LL7F9:
 goto _LL7F5;}_LL7F5:;}
# 4956
if(_tmp99C != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp9B8;_tmp9B8.tag=0;_tmp9B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmp99C->hd)->name);({void*_tmp9B6[1]={& _tmp9B8};Cyc_Tcutil_terr(loc,({const char*_tmp9B7="unbound type variable %s";_tag_dyneither(_tmp9B7,sizeof(char),25);}),_tag_dyneither(_tmp9B6,sizeof(void*),1));});});
Cyc_Tcutil_check_free_evars(_tmp99D,t,loc);}}
# 4900
;_pop_region(temp);}
# 4967
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 4970
Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);
{void*_tmp9BA=Cyc_Tcutil_compress(t);void*_tmp9BB=_tmp9BA;struct Cyc_List_List*_tmp9BC;void*_tmp9BD;struct Cyc_Absyn_Tqual _tmp9BE;void*_tmp9BF;struct Cyc_List_List*_tmp9C0;struct Cyc_Absyn_Exp*_tmp9C1;struct Cyc_List_List*_tmp9C2;struct Cyc_Absyn_Exp*_tmp9C3;struct Cyc_List_List*_tmp9C4;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9BB)->tag == 9){_LL7FB: _tmp9BC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9BB)->f1).tvars;_tmp9BD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9BB)->f1).effect;_tmp9BE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9BB)->f1).ret_tqual;_tmp9BF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9BB)->f1).ret_typ;_tmp9C0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9BB)->f1).args;_tmp9C1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9BB)->f1).requires_clause;_tmp9C2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9BB)->f1).requires_relns;_tmp9C3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9BB)->f1).ensures_clause;_tmp9C4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9BB)->f1).ensures_relns;_LL7FC:
# 4973
 fd->tvs=_tmp9BC;
fd->effect=_tmp9BD;
{struct Cyc_List_List*_tmp9C5=fd->args;for(0;_tmp9C5 != 0;(_tmp9C5=_tmp9C5->tl,_tmp9C0=_tmp9C0->tl)){
# 4977
(*((struct _tuple10*)_tmp9C5->hd)).f2=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp9C0))->hd)).f2;
(*((struct _tuple10*)_tmp9C5->hd)).f3=(*((struct _tuple10*)_tmp9C0->hd)).f3;}}
# 4980
fd->ret_tqual=_tmp9BE;
fd->ret_type=_tmp9BF;
(fd->ret_tqual).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmp9BF);
fd->requires_clause=_tmp9C1;
fd->requires_relns=_tmp9C2;
fd->ensures_clause=_tmp9C3;
fd->ensures_relns=_tmp9C4;
goto _LL7FA;}else{_LL7FD: _LL7FE:
({void*_tmp9C6=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp9C7="check_fndecl_valid_type: not a FnType";_tag_dyneither(_tmp9C7,sizeof(char),38);}),_tag_dyneither(_tmp9C6,sizeof(void*),0));});}_LL7FA:;}{
# 4991
struct _RegionHandle _tmp9C8=_new_region("r");struct _RegionHandle*r=& _tmp9C8;_push_region(r);
Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _dyneither_ptr*(*)(struct _tuple10*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,({const char*_tmp9C9="function declaration has repeated parameter";_tag_dyneither(_tmp9C9,sizeof(char),44);}));
# 4996
fd->cached_typ=t;
# 4991
;_pop_region(r);};}
# 5001
void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,int allow_abs_aggr,void*t){
# 5007
struct _RegionHandle _tmp9CA=_new_region("r");struct _RegionHandle*r=& _tmp9CA;_push_region(r);
{struct Cyc_Tcutil_CVTEnv _tmp9CB=Cyc_Tcutil_check_valid_type(loc,te,({struct Cyc_Tcutil_CVTEnv _tmp9D3;_tmp9D3.r=r;_tmp9D3.kind_env=bound_tvars;_tmp9D3.free_vars=0;_tmp9D3.free_evars=0;_tmp9D3.generalize_evars=0;_tmp9D3.fn_result=0;_tmp9D3;}),expected_kind,allow_abs_aggr,t);
# 5011
struct Cyc_List_List*_tmp9CC=Cyc_Tcutil_remove_bound_tvars(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmp9CB.free_vars),bound_tvars);
# 5013
struct Cyc_List_List*_tmp9CD=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmp9CB.free_evars);
{struct Cyc_List_List*fs=_tmp9CC;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmp9CE=((struct Cyc_Absyn_Tvar*)fs->hd)->name;
({struct Cyc_String_pa_PrintArg_struct _tmp9D2;_tmp9D2.tag=0;_tmp9D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({struct Cyc_String_pa_PrintArg_struct _tmp9D1;_tmp9D1.tag=0;_tmp9D1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp9CE);({void*_tmp9CF[2]={& _tmp9D1,& _tmp9D2};Cyc_Tcutil_terr(loc,({const char*_tmp9D0="unbound type variable %s in type %s";_tag_dyneither(_tmp9D0,sizeof(char),36);}),_tag_dyneither(_tmp9CF,sizeof(void*),2));});});});}}
# 5018
if(!allow_evars)
Cyc_Tcutil_check_free_evars(_tmp9CD,t,loc);}
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
if(cmp(vs->hd,vs2->hd)== 0)
({struct Cyc_String_pa_PrintArg_struct _tmp9D7;_tmp9D7.tag=0;_tmp9D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string(vs->hd));({struct Cyc_String_pa_PrintArg_struct _tmp9D6;_tmp9D6.tag=0;_tmp9D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg);({void*_tmp9D4[2]={& _tmp9D6,& _tmp9D7};Cyc_Tcutil_terr(loc,({const char*_tmp9D5="%s: %s";_tag_dyneither(_tmp9D5,sizeof(char),7);}),_tag_dyneither(_tmp9D4,sizeof(void*),2));});});});}}}
# 5043
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s){
return*s;}
# 5047
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr msg){
((void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 5051
void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,({const char*_tmp9D8="duplicate type variable";_tag_dyneither(_tmp9D8,sizeof(char),24);}));}struct _tuple32{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple33{struct Cyc_List_List*f1;void*f2;};struct _tuple34{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 5065 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 5070
struct _RegionHandle _tmp9D9=_new_region("temp");struct _RegionHandle*temp=& _tmp9D9;_push_region(temp);
# 5074
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
if(Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,({const char*_tmp9DA="";_tag_dyneither(_tmp9DA,sizeof(char),1);}))!= 0)
fields=({struct Cyc_List_List*_tmp9DB=_region_malloc(temp,sizeof(*_tmp9DB));_tmp9DB->hd=({struct _tuple32*_tmp9DC=_region_malloc(temp,sizeof(*_tmp9DC));_tmp9DC->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd;_tmp9DC->f2=0;_tmp9DC;});_tmp9DB->tl=fields;_tmp9DB;});}}
# 5079
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 5081
struct _dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?({const char*_tmpA02="struct";_tag_dyneither(_tmpA02,sizeof(char),7);}):({const char*_tmpA03="union";_tag_dyneither(_tmpA03,sizeof(char),6);});
# 5084
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple33*_tmp9DD=(struct _tuple33*)des->hd;struct _tuple33*_tmp9DE=_tmp9DD;struct Cyc_List_List*_tmp9DF;void*_tmp9E0;_LL800: _tmp9DF=_tmp9DE->f1;_tmp9E0=_tmp9DE->f2;_LL801:;
if(_tmp9DF == 0){
# 5089
struct Cyc_List_List*_tmp9E1=fields;
for(0;_tmp9E1 != 0;_tmp9E1=_tmp9E1->tl){
if(!(*((struct _tuple32*)_tmp9E1->hd)).f2){
(*((struct _tuple32*)_tmp9E1->hd)).f2=1;
(*((struct _tuple33*)des->hd)).f1=({struct Cyc_List_List*_tmp9E2=_cycalloc(sizeof(*_tmp9E2));_tmp9E2->hd=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp9E3=_cycalloc(sizeof(*_tmp9E3));_tmp9E3[0]=({struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp9E4;_tmp9E4.tag=1;_tmp9E4.f1=((*((struct _tuple32*)_tmp9E1->hd)).f1)->name;_tmp9E4;});_tmp9E3;});_tmp9E2->tl=0;_tmp9E2;});
ans=({struct Cyc_List_List*_tmp9E5=_region_malloc(rgn,sizeof(*_tmp9E5));_tmp9E5->hd=({struct _tuple34*_tmp9E6=_region_malloc(rgn,sizeof(*_tmp9E6));_tmp9E6->f1=(*((struct _tuple32*)_tmp9E1->hd)).f1;_tmp9E6->f2=_tmp9E0;_tmp9E6;});_tmp9E5->tl=ans;_tmp9E5;});
break;}}
# 5097
if(_tmp9E1 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp9E9;_tmp9E9.tag=0;_tmp9E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);({void*_tmp9E7[1]={& _tmp9E9};Cyc_Tcutil_terr(loc,({const char*_tmp9E8="too many arguments to %s";_tag_dyneither(_tmp9E8,sizeof(char),25);}),_tag_dyneither(_tmp9E7,sizeof(void*),1));});});}else{
if(_tmp9DF->tl != 0)
# 5101
({void*_tmp9EA=0;Cyc_Tcutil_terr(loc,({const char*_tmp9EB="multiple designators are not yet supported";_tag_dyneither(_tmp9EB,sizeof(char),43);}),_tag_dyneither(_tmp9EA,sizeof(void*),0));});else{
# 5104
void*_tmp9EC=(void*)_tmp9DF->hd;void*_tmp9ED=_tmp9EC;struct _dyneither_ptr*_tmp9EE;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp9ED)->tag == 0){_LL803: _LL804:
# 5106
({struct Cyc_String_pa_PrintArg_struct _tmp9F1;_tmp9F1.tag=0;_tmp9F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);({void*_tmp9EF[1]={& _tmp9F1};Cyc_Tcutil_terr(loc,({const char*_tmp9F0="array designator used in argument to %s";_tag_dyneither(_tmp9F0,sizeof(char),40);}),_tag_dyneither(_tmp9EF,sizeof(void*),1));});});
goto _LL802;}else{_LL805: _tmp9EE=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp9ED)->f1;_LL806: {
# 5109
struct Cyc_List_List*_tmp9F2=fields;
for(0;_tmp9F2 != 0;_tmp9F2=_tmp9F2->tl){
if(Cyc_strptrcmp(_tmp9EE,((*((struct _tuple32*)_tmp9F2->hd)).f1)->name)== 0){
if((*((struct _tuple32*)_tmp9F2->hd)).f2)
({struct Cyc_String_pa_PrintArg_struct _tmp9F5;_tmp9F5.tag=0;_tmp9F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp9EE);({void*_tmp9F3[1]={& _tmp9F5};Cyc_Tcutil_terr(loc,({const char*_tmp9F4="member %s has already been used as an argument";_tag_dyneither(_tmp9F4,sizeof(char),47);}),_tag_dyneither(_tmp9F3,sizeof(void*),1));});});
(*((struct _tuple32*)_tmp9F2->hd)).f2=1;
ans=({struct Cyc_List_List*_tmp9F6=_region_malloc(rgn,sizeof(*_tmp9F6));_tmp9F6->hd=({struct _tuple34*_tmp9F7=_region_malloc(rgn,sizeof(*_tmp9F7));_tmp9F7->f1=(*((struct _tuple32*)_tmp9F2->hd)).f1;_tmp9F7->f2=_tmp9E0;_tmp9F7;});_tmp9F6->tl=ans;_tmp9F6;});
break;}}
# 5118
if(_tmp9F2 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp9FA;_tmp9FA.tag=0;_tmp9FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp9EE);({void*_tmp9F8[1]={& _tmp9FA};Cyc_Tcutil_terr(loc,({const char*_tmp9F9="bad field designator %s";_tag_dyneither(_tmp9F9,sizeof(char),24);}),_tag_dyneither(_tmp9F8,sizeof(void*),1));});});
goto _LL802;}}_LL802:;}}}
# 5123
if(aggr_kind == Cyc_Absyn_StructA)
# 5125
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple32*)fields->hd)).f2){
({void*_tmp9FB=0;Cyc_Tcutil_terr(loc,({const char*_tmp9FC="too few arguments to struct";_tag_dyneither(_tmp9FC,sizeof(char),28);}),_tag_dyneither(_tmp9FB,sizeof(void*),0));});
break;}}else{
# 5132
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple32*)fields->hd)).f2){
if(found)({void*_tmp9FD=0;Cyc_Tcutil_terr(loc,({const char*_tmp9FE="only one member of a union is allowed";_tag_dyneither(_tmp9FE,sizeof(char),38);}),_tag_dyneither(_tmp9FD,sizeof(void*),0));});
found=1;}}
# 5139
if(!found)({void*_tmp9FF=0;Cyc_Tcutil_terr(loc,({const char*_tmpA00="missing member for union";_tag_dyneither(_tmpA00,sizeof(char),25);}),_tag_dyneither(_tmp9FF,sizeof(void*),0));});}{
# 5142
struct Cyc_List_List*_tmpA01=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0);return _tmpA01;};};}
# 5074
;_pop_region(temp);}
# 5145
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmpA04=Cyc_Tcutil_compress(t);void*_tmpA05=_tmpA04;void*_tmpA06;union Cyc_Absyn_Constraint*_tmpA07;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA05)->tag == 5){_LL808: _tmpA06=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA05)->f1).elt_typ;_tmpA07=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA05)->f1).ptr_atts).bounds;_LL809: {
# 5148
void*_tmpA08=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpA07);void*_tmpA09=_tmpA08;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpA09)->tag == 0){_LL80D: _LL80E:
# 5150
*elt_typ_dest=_tmpA06;
return 1;}else{_LL80F: _LL810:
 return 0;}_LL80C:;}}else{_LL80A: _LL80B:
# 5154
 return 0;}_LL807:;}
# 5158
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmpA0A=Cyc_Tcutil_compress(t);void*_tmpA0B=_tmpA0A;void*_tmpA0C;union Cyc_Absyn_Constraint*_tmpA0D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA0B)->tag == 5){_LL812: _tmpA0C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA0B)->f1).elt_typ;_tmpA0D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA0B)->f1).ptr_atts).zero_term;_LL813:
# 5161
*elt_typ_dest=_tmpA0C;
return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpA0D);}else{_LL814: _LL815:
 return 0;}_LL811:;}
# 5169
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type){
# 5171
void*_tmpA0E=Cyc_Tcutil_compress(t);void*_tmpA0F=_tmpA0E;void*_tmpA10;struct Cyc_Absyn_Tqual _tmpA11;struct Cyc_Absyn_Exp*_tmpA12;union Cyc_Absyn_Constraint*_tmpA13;void*_tmpA14;union Cyc_Absyn_Constraint*_tmpA15;union Cyc_Absyn_Constraint*_tmpA16;switch(*((int*)_tmpA0F)){case 5: _LL817: _tmpA14=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA0F)->f1).elt_typ;_tmpA15=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA0F)->f1).ptr_atts).bounds;_tmpA16=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA0F)->f1).ptr_atts).zero_term;_LL818:
# 5173
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpA16)){
*ptr_type=t;
*elt_type=_tmpA14;
{void*_tmpA17=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmpA15);void*_tmpA18=_tmpA17;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpA18)->tag == 0){_LL81E: _LL81F:
*is_dyneither=1;goto _LL81D;}else{_LL820: _LL821:
*is_dyneither=0;goto _LL81D;}_LL81D:;}
# 5180
return 1;}else{
return 0;}case 8: _LL819: _tmpA10=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA0F)->f1).elt_type;_tmpA11=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA0F)->f1).tq;_tmpA12=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA0F)->f1).num_elts;_tmpA13=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA0F)->f1).zero_term;_LL81A:
# 5183
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpA13)){
*elt_type=_tmpA10;
*is_dyneither=0;
*ptr_type=Cyc_Tcutil_promote_array(t,(void*)& Cyc_Absyn_HeapRgn_val,0);
return 1;}else{
return 0;}default: _LL81B: _LL81C:
 return 0;}_LL816:;}
# 5196
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){
# 5198
void*_tmpA19=e1->r;void*_tmpA1A=_tmpA19;struct Cyc_Absyn_Exp*_tmpA1B;struct Cyc_Absyn_Exp*_tmpA1C;struct Cyc_Absyn_Exp*_tmpA1D;struct Cyc_Absyn_Exp*_tmpA1E;struct Cyc_Absyn_Exp*_tmpA1F;struct Cyc_Absyn_Exp*_tmpA20;switch(*((int*)_tmpA1A)){case 13: _LL823: _LL824:
# 5200
({struct Cyc_String_pa_PrintArg_struct _tmpA23;_tmpA23.tag=0;_tmpA23.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));({void*_tmpA21[1]={& _tmpA23};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA22="we have a cast in a lhs:  %s";_tag_dyneither(_tmpA22,sizeof(char),29);}),_tag_dyneither(_tmpA21,sizeof(void*),1));});});case 19: _LL825: _tmpA20=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpA1A)->f1;_LL826:
 _tmpA1F=_tmpA20;goto _LL828;case 22: _LL827: _tmpA1F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA1A)->f1;_LL828:
# 5203
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpA1F->topt),ptr_type,is_dyneither,elt_type);case 21: _LL829: _tmpA1E=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA1A)->f1;_LL82A:
 _tmpA1D=_tmpA1E;goto _LL82C;case 20: _LL82B: _tmpA1D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA1A)->f1;_LL82C:
# 5207
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpA1D->topt),ptr_type,is_dyneither,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmpA26;_tmpA26.tag=0;_tmpA26.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1));({void*_tmpA24[1]={& _tmpA26};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA25="found zero pointer aggregate member assignment: %s";_tag_dyneither(_tmpA25,sizeof(char),51);}),_tag_dyneither(_tmpA24,sizeof(void*),1));});});
return 0;case 12: _LL82D: _tmpA1C=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpA1A)->f1;_LL82E:
 _tmpA1B=_tmpA1C;goto _LL830;case 11: _LL82F: _tmpA1B=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpA1A)->f1;_LL830:
# 5213
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpA1B->topt),ptr_type,is_dyneither,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmpA29;_tmpA29.tag=0;_tmpA29.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1));({void*_tmpA27[1]={& _tmpA29};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA28="found zero pointer instantiate/noinstantiate: %s";_tag_dyneither(_tmpA28,sizeof(char),49);}),_tag_dyneither(_tmpA27,sizeof(void*),1));});});
return 0;case 1: _LL831: _LL832:
 return 0;default: _LL833: _LL834:
# 5219
({struct Cyc_String_pa_PrintArg_struct _tmpA2C;_tmpA2C.tag=0;_tmpA2C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));({void*_tmpA2A[1]={& _tmpA2C};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA2B="found bad lhs in is_zero_ptr_deref: %s";_tag_dyneither(_tmpA2B,sizeof(char),39);}),_tag_dyneither(_tmpA2A,sizeof(void*),1));});});}_LL822:;}
# 5223
int Cyc_Tcutil_is_tagged_pointer_typ(void*t){
void*ignore=(void*)& Cyc_Absyn_VoidType_val;
return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
# 5234
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
# 5237
void*_tmpA2D=Cyc_Tcutil_compress(r);void*_tmpA2E=_tmpA2D;struct Cyc_Absyn_Tvar*_tmpA2F;enum Cyc_Absyn_KindQual _tmpA30;enum Cyc_Absyn_AliasQual _tmpA31;switch(*((int*)_tmpA2E)){case 22: _LL836: _LL837:
 return !must_be_unique;case 21: _LL838: _LL839:
 return 1;case 17: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpA2E)->f3 != 0){if(((struct Cyc_Absyn_Typedefdecl*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpA2E)->f3)->kind != 0){if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpA2E)->f4 == 0){_LL83A: _tmpA30=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpA2E)->f3)->kind)->v)->kind;_tmpA31=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpA2E)->f3)->kind)->v)->aliasqual;_LL83B:
# 5241
 return _tmpA30 == Cyc_Absyn_RgnKind  && (_tmpA31 == Cyc_Absyn_Unique  || _tmpA31 == Cyc_Absyn_Top  && !must_be_unique);}else{goto _LL83E;}}else{goto _LL83E;}}else{goto _LL83E;}case 2: _LL83C: _tmpA2F=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA2E)->f1;_LL83D: {
# 5245
struct Cyc_Absyn_Kind*_tmpA32=Cyc_Tcutil_tvar_kind(_tmpA2F,& Cyc_Tcutil_rk);struct Cyc_Absyn_Kind*_tmpA33=_tmpA32;enum Cyc_Absyn_KindQual _tmpA34;enum Cyc_Absyn_AliasQual _tmpA35;_LL841: _tmpA34=_tmpA33->kind;_tmpA35=_tmpA33->aliasqual;_LL842:;
if(_tmpA34 == Cyc_Absyn_RgnKind  && (_tmpA35 == Cyc_Absyn_Unique  || _tmpA35 == Cyc_Absyn_Top  && !must_be_unique)){
void*_tmpA36=Cyc_Absyn_compress_kb(_tmpA2F->kind);void*_tmpA37=_tmpA36;struct Cyc_Core_Opt**_tmpA38;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA37)->tag == 2){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA37)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA37)->f2)->aliasqual == Cyc_Absyn_Top){_LL844: _tmpA38=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA37)->f1;_LL845:
# 5249
*_tmpA38=({struct Cyc_Core_Opt*_tmpA39=_cycalloc(sizeof(*_tmpA39));_tmpA39->v=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA3A=_cycalloc(sizeof(*_tmpA3A));_tmpA3A[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpA3B;_tmpA3B.tag=2;_tmpA3B.f1=0;_tmpA3B.f2=& Cyc_Tcutil_rk;_tmpA3B;});_tmpA3A;});_tmpA39;});
return 0;}else{goto _LL846;}}else{goto _LL846;}}else{_LL846: _LL847:
 return 1;}_LL843:;}
# 5254
return 0;}default: _LL83E: _LL83F:
 return 0;}_LL835:;}
# 5261
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmpA3C=Cyc_Tcutil_compress(t);void*_tmpA3D=_tmpA3C;struct Cyc_Absyn_Tvar*_tmpA3E;void*_tmpA3F;switch(*((int*)_tmpA3D)){case 5: _LL849: _tmpA3F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA3D)->f1).ptr_atts).rgn;_LL84A:
# 5264
 return Cyc_Tcutil_is_noalias_region(_tmpA3F,must_be_unique);case 2: _LL84B: _tmpA3E=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA3D)->f1;_LL84C: {
# 5266
struct Cyc_Absyn_Kind*_tmpA40=Cyc_Tcutil_tvar_kind(_tmpA3E,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmpA41=_tmpA40;enum Cyc_Absyn_KindQual _tmpA42;enum Cyc_Absyn_AliasQual _tmpA43;_LL850: _tmpA42=_tmpA41->kind;_tmpA43=_tmpA41->aliasqual;_LL851:;{
enum Cyc_Absyn_KindQual _tmpA44=_tmpA42;switch(_tmpA44){case Cyc_Absyn_BoxKind: _LL853: _LL854:
 goto _LL856;case Cyc_Absyn_AnyKind: _LL855: _LL856: goto _LL858;case Cyc_Absyn_MemKind: _LL857: _LL858:
 if(_tmpA43 == Cyc_Absyn_Unique  || _tmpA43 == Cyc_Absyn_Top  && !must_be_unique){
void*_tmpA45=Cyc_Absyn_compress_kb(_tmpA3E->kind);void*_tmpA46=_tmpA45;struct Cyc_Core_Opt**_tmpA47;enum Cyc_Absyn_KindQual _tmpA48;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA46)->tag == 2){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA46)->f2)->aliasqual == Cyc_Absyn_Top){_LL85C: _tmpA47=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA46)->f1;_tmpA48=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA46)->f2)->kind;_LL85D:
# 5272
*_tmpA47=({struct Cyc_Core_Opt*_tmpA49=_cycalloc(sizeof(*_tmpA49));_tmpA49->v=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA4A=_cycalloc(sizeof(*_tmpA4A));_tmpA4A[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpA4B;_tmpA4B.tag=2;_tmpA4B.f1=0;_tmpA4B.f2=({struct Cyc_Absyn_Kind*_tmpA4C=_cycalloc_atomic(sizeof(*_tmpA4C));_tmpA4C->kind=_tmpA48;_tmpA4C->aliasqual=Cyc_Absyn_Aliasable;_tmpA4C;});_tmpA4B;});_tmpA4A;});_tmpA49;});
return 0;}else{goto _LL85E;}}else{_LL85E: _LL85F:
# 5276
 return 1;}_LL85B:;}
# 5279
return 0;default: _LL859: _LL85A:
 return 0;}_LL852:;};}default: _LL84D: _LL84E:
# 5282
 return 0;}_LL848:;}
# 5285
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*rgn,void*t){
void*_tmpA4D=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(_tmpA4D,0))return 1;{
void*_tmpA4E=_tmpA4D;union Cyc_Absyn_DatatypeFieldInfoU _tmpA4F;struct Cyc_List_List*_tmpA50;union Cyc_Absyn_DatatypeInfoU _tmpA51;struct Cyc_List_List*_tmpA52;struct Cyc_List_List*_tmpA53;struct Cyc_Absyn_Aggrdecl**_tmpA54;struct Cyc_List_List*_tmpA55;struct Cyc_List_List*_tmpA56;switch(*((int*)_tmpA4E)){case 10: _LL861: _tmpA56=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA4E)->f1;_LL862:
# 5290
 while(_tmpA56 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(*((struct _tuple12*)_tmpA56->hd)).f2))return 1;
_tmpA56=_tmpA56->tl;}
# 5294
return 0;case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA4E)->f1).aggr_info).KnownAggr).tag == 2){_LL863: _tmpA54=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA4E)->f1).aggr_info).KnownAggr).val;_tmpA55=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA4E)->f1).targs;_LL864:
# 5296
 if((*_tmpA54)->impl == 0)return 0;else{
# 5298
struct Cyc_List_List*_tmpA57=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,(*_tmpA54)->tvs,_tmpA55);
struct Cyc_List_List*_tmpA58=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpA54)->impl))->fields;
void*t;
while(_tmpA58 != 0){
t=_tmpA57 == 0?((struct Cyc_Absyn_Aggrfield*)_tmpA58->hd)->type: Cyc_Tcutil_rsubstitute(rgn,_tmpA57,((struct Cyc_Absyn_Aggrfield*)_tmpA58->hd)->type);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpA58=_tmpA58->tl;}
# 5306
return 0;}}else{_LL867: _LL868:
# 5316
 return 0;}case 12: _LL865: _tmpA53=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA4E)->f2;_LL866:
# 5309
 while(_tmpA53 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,((struct Cyc_Absyn_Aggrfield*)_tmpA53->hd)->type))return 1;
_tmpA53=_tmpA53->tl;}
# 5313
return 0;case 3: _LL869: _tmpA51=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA4E)->f1).datatype_info;_tmpA52=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA4E)->f1).targs;_LL86A: {
# 5318
union Cyc_Absyn_DatatypeInfoU _tmpA59=_tmpA51;struct Cyc_List_List*_tmpA5A;struct Cyc_Core_Opt*_tmpA5B;struct _tuple2*_tmpA5C;int _tmpA5D;if((_tmpA59.UnknownDatatype).tag == 1){_LL870: _tmpA5C=((_tmpA59.UnknownDatatype).val).name;_tmpA5D=((_tmpA59.UnknownDatatype).val).is_extensible;_LL871:
# 5321
 return 0;}else{_LL872: _tmpA5A=(*(_tmpA59.KnownDatatype).val)->tvs;_tmpA5B=(*(_tmpA59.KnownDatatype).val)->fields;_LL873:
# 5324
 return 0;}_LL86F:;}case 4: _LL86B: _tmpA4F=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpA4E)->f1).field_info;_tmpA50=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpA4E)->f1).targs;_LL86C: {
# 5327
union Cyc_Absyn_DatatypeFieldInfoU _tmpA5E=_tmpA4F;struct Cyc_Absyn_Datatypedecl*_tmpA5F;struct Cyc_Absyn_Datatypefield*_tmpA60;if((_tmpA5E.UnknownDatatypefield).tag == 1){_LL875: _LL876:
# 5330
 return 0;}else{_LL877: _tmpA5F=((_tmpA5E.KnownDatatypefield).val).f1;_tmpA60=((_tmpA5E.KnownDatatypefield).val).f2;_LL878: {
# 5332
struct Cyc_List_List*_tmpA61=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpA5F->tvs,_tmpA50);
struct Cyc_List_List*_tmpA62=_tmpA60->typs;
while(_tmpA62 != 0){
_tmpA4D=_tmpA61 == 0?(*((struct _tuple12*)_tmpA62->hd)).f2: Cyc_Tcutil_rsubstitute(rgn,_tmpA61,(*((struct _tuple12*)_tmpA62->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,_tmpA4D))return 1;
_tmpA62=_tmpA62->tl;}
# 5339
return 0;}}_LL874:;}default: _LL86D: _LL86E:
# 5341
 return 0;}_LL860:;};}
# 5345
static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,struct _dyneither_ptr*f){
void*_tmpA63=Cyc_Tcutil_compress(t);void*_tmpA64=_tmpA63;struct Cyc_List_List*_tmpA65;struct Cyc_Absyn_Aggrdecl*_tmpA66;struct Cyc_List_List*_tmpA67;switch(*((int*)_tmpA64)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA64)->f1).aggr_info).KnownAggr).tag == 2){_LL87A: _tmpA66=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA64)->f1).aggr_info).KnownAggr).val;_tmpA67=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA64)->f1).targs;_LL87B:
# 5348
 _tmpA65=_tmpA66->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpA66->impl))->fields;goto _LL87D;}else{goto _LL87E;}case 12: _LL87C: _tmpA65=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA64)->f2;_LL87D: {
# 5350
struct Cyc_Absyn_Aggrfield*_tmpA68=Cyc_Absyn_lookup_field(_tmpA65,f);
{struct Cyc_Absyn_Aggrfield*_tmpA69=_tmpA68;void*_tmpA6A;if(_tmpA69 == 0){_LL881: _LL882:
({void*_tmpA6B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA6C="is_noalias_field: missing field";_tag_dyneither(_tmpA6C,sizeof(char),32);}),_tag_dyneither(_tmpA6B,sizeof(void*),0));});}else{_LL883: _tmpA6A=_tmpA69->type;_LL884:
# 5354
 return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpA6A);}_LL880:;}
# 5356
return 0;}default: _LL87E: _LL87F:
# 5358
({struct Cyc_String_pa_PrintArg_struct _tmpA6F;_tmpA6F.tag=0;_tmpA6F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));({void*_tmpA6D[1]={& _tmpA6F};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA6E="is_noalias_field: invalid type |%s|";_tag_dyneither(_tmpA6E,sizeof(char),36);}),_tag_dyneither(_tmpA6D,sizeof(void*),1));});});}_LL879:;}
# 5366
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
void*_tmpA70=e->r;void*_tmpA71=_tmpA70;struct Cyc_Absyn_Stmt*_tmpA72;struct Cyc_Absyn_Exp*_tmpA73;struct Cyc_Absyn_Exp*_tmpA74;struct Cyc_Absyn_Exp*_tmpA75;struct Cyc_Absyn_Exp*_tmpA76;struct Cyc_Absyn_Exp*_tmpA77;struct Cyc_Absyn_Exp*_tmpA78;struct Cyc_Absyn_Exp*_tmpA79;struct _dyneither_ptr*_tmpA7A;struct Cyc_Absyn_Exp*_tmpA7B;struct Cyc_Absyn_Exp*_tmpA7C;switch(*((int*)_tmpA71)){case 1: if(((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA71)->f2)->tag == 1){_LL886: _LL887:
 return 0;}else{goto _LL898;}case 21: _LL888: _tmpA7C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA71)->f1;_LL889:
 _tmpA7B=_tmpA7C;goto _LL88B;case 19: _LL88A: _tmpA7B=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpA71)->f1;_LL88B:
# 5371
 return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmpA7B->topt),1) && 
Cyc_Tcutil_is_noalias_path(r,_tmpA7B);case 20: _LL88C: _tmpA79=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA71)->f1;_tmpA7A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA71)->f2;_LL88D:
 return Cyc_Tcutil_is_noalias_path(r,_tmpA79);case 22: _LL88E: _tmpA77=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA71)->f1;_tmpA78=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA71)->f2;_LL88F: {
# 5375
void*_tmpA7D=Cyc_Tcutil_compress((void*)_check_null(_tmpA77->topt));void*_tmpA7E=_tmpA7D;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA7E)->tag == 10){_LL89B: _LL89C:
 return Cyc_Tcutil_is_noalias_path(r,_tmpA77);}else{_LL89D: _LL89E:
 return 0;}_LL89A:;}case 5: _LL890: _tmpA75=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA71)->f2;_tmpA76=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA71)->f3;_LL891:
# 5380
 return Cyc_Tcutil_is_noalias_path(r,_tmpA75) && Cyc_Tcutil_is_noalias_path(r,_tmpA76);case 8: _LL892: _tmpA74=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpA71)->f2;_LL893:
 _tmpA73=_tmpA74;goto _LL895;case 13: _LL894: _tmpA73=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA71)->f2;_LL895:
 return Cyc_Tcutil_is_noalias_path(r,_tmpA73);case 36: _LL896: _tmpA72=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpA71)->f1;_LL897:
# 5384
 while(1){
void*_tmpA7F=_tmpA72->r;void*_tmpA80=_tmpA7F;struct Cyc_Absyn_Exp*_tmpA81;struct Cyc_Absyn_Decl*_tmpA82;struct Cyc_Absyn_Stmt*_tmpA83;struct Cyc_Absyn_Stmt*_tmpA84;struct Cyc_Absyn_Stmt*_tmpA85;switch(*((int*)_tmpA80)){case 2: _LL8A0: _tmpA84=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpA80)->f1;_tmpA85=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpA80)->f2;_LL8A1:
 _tmpA72=_tmpA85;goto _LL89F;case 12: _LL8A2: _tmpA82=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpA80)->f1;_tmpA83=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpA80)->f2;_LL8A3:
 _tmpA72=_tmpA83;goto _LL89F;case 1: _LL8A4: _tmpA81=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpA80)->f1;_LL8A5:
 return Cyc_Tcutil_is_noalias_path(r,_tmpA81);default: _LL8A6: _LL8A7:
({void*_tmpA86=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA87="is_noalias_stmt_exp: ill-formed StmtExp";_tag_dyneither(_tmpA87,sizeof(char),40);}),_tag_dyneither(_tmpA86,sizeof(void*),0));});}_LL89F:;}default: _LL898: _LL899:
# 5392
 return 1;}_LL885:;}
# 5409 "tcutil.cyc"
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5411
struct _tuple18 bogus_ans=({struct _tuple18 _tmpAD0;_tmpAD0.f1=0;_tmpAD0.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpAD0;});
void*_tmpA88=e->r;void*_tmpA89=_tmpA88;struct Cyc_Absyn_Exp*_tmpA8A;struct Cyc_Absyn_Exp*_tmpA8B;struct Cyc_Absyn_Exp*_tmpA8C;struct Cyc_Absyn_Exp*_tmpA8D;struct _dyneither_ptr*_tmpA8E;int _tmpA8F;struct Cyc_Absyn_Exp*_tmpA90;struct _dyneither_ptr*_tmpA91;int _tmpA92;struct _tuple2*_tmpA93;void*_tmpA94;switch(*((int*)_tmpA89)){case 1: _LL8A9: _tmpA93=((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA89)->f1;_tmpA94=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA89)->f2;_LL8AA: {
# 5415
void*_tmpA95=_tmpA94;struct Cyc_Absyn_Vardecl*_tmpA96;struct Cyc_Absyn_Vardecl*_tmpA97;struct Cyc_Absyn_Vardecl*_tmpA98;struct Cyc_Absyn_Vardecl*_tmpA99;switch(*((int*)_tmpA95)){case 0: _LL8B6: _LL8B7:
 goto _LL8B9;case 2: _LL8B8: _LL8B9:
 return bogus_ans;case 1: _LL8BA: _tmpA99=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpA95)->f1;_LL8BB: {
# 5419
void*_tmpA9A=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpA9B=_tmpA9A;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA9B)->tag == 8){_LL8C3: _LL8C4:
# 5421
 return({struct _tuple18 _tmpA9C;_tmpA9C.f1=1;_tmpA9C.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpA9C;});}else{_LL8C5: _LL8C6:
 return({struct _tuple18 _tmpA9D;_tmpA9D.f1=(_tmpA99->tq).real_const;_tmpA9D.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpA9D;});}_LL8C2:;}case 4: _LL8BC: _tmpA98=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpA95)->f1;_LL8BD: {
# 5425
void*_tmpA9E=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpA9F=_tmpA9E;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA9F)->tag == 8){_LL8C8: _LL8C9:
 return({struct _tuple18 _tmpAA0;_tmpAA0.f1=1;_tmpAA0.f2=(void*)_check_null(_tmpA98->rgn);_tmpAA0;});}else{_LL8CA: _LL8CB:
# 5428
 _tmpA98->escapes=1;
return({struct _tuple18 _tmpAA1;_tmpAA1.f1=(_tmpA98->tq).real_const;_tmpAA1.f2=(void*)_check_null(_tmpA98->rgn);_tmpAA1;});}_LL8C7:;}case 5: _LL8BE: _tmpA97=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpA95)->f1;_LL8BF:
# 5431
 _tmpA96=_tmpA97;goto _LL8C1;default: _LL8C0: _tmpA96=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpA95)->f1;_LL8C1:
# 5433
 _tmpA96->escapes=1;
return({struct _tuple18 _tmpAA2;_tmpAA2.f1=(_tmpA96->tq).real_const;_tmpAA2.f2=(void*)_check_null(_tmpA96->rgn);_tmpAA2;});}_LL8B5:;}case 20: _LL8AB: _tmpA90=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA89)->f1;_tmpA91=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA89)->f2;_tmpA92=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA89)->f3;_LL8AC:
# 5438
 if(_tmpA92)return bogus_ans;{
# 5441
void*_tmpAA3=Cyc_Tcutil_compress((void*)_check_null(_tmpA90->topt));void*_tmpAA4=_tmpAA3;struct Cyc_Absyn_Aggrdecl*_tmpAA5;struct Cyc_List_List*_tmpAA6;switch(*((int*)_tmpAA4)){case 12: _LL8CD: _tmpAA6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpAA4)->f2;_LL8CE: {
# 5443
struct Cyc_Absyn_Aggrfield*_tmpAA7=Cyc_Absyn_lookup_field(_tmpAA6,_tmpA91);
if(_tmpAA7 != 0  && _tmpAA7->width == 0){
struct _tuple18 _tmpAA8=Cyc_Tcutil_addressof_props(te,_tmpA90);struct _tuple18 _tmpAA9=_tmpAA8;int _tmpAAA;void*_tmpAAB;_LL8D4: _tmpAAA=_tmpAA9.f1;_tmpAAB=_tmpAA9.f2;_LL8D5:;
return({struct _tuple18 _tmpAAC;_tmpAAC.f1=(_tmpAA7->tq).real_const  || _tmpAAA;_tmpAAC.f2=_tmpAAB;_tmpAAC;});}
# 5448
return bogus_ans;}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAA4)->f1).aggr_info).KnownAggr).tag == 2){_LL8CF: _tmpAA5=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAA4)->f1).aggr_info).KnownAggr).val;_LL8D0: {
# 5450
struct Cyc_Absyn_Aggrfield*_tmpAAD=Cyc_Absyn_lookup_decl_field(_tmpAA5,_tmpA91);
if(_tmpAAD != 0  && _tmpAAD->width == 0){
struct _tuple18 _tmpAAE=Cyc_Tcutil_addressof_props(te,_tmpA90);struct _tuple18 _tmpAAF=_tmpAAE;int _tmpAB0;void*_tmpAB1;_LL8D7: _tmpAB0=_tmpAAF.f1;_tmpAB1=_tmpAAF.f2;_LL8D8:;
return({struct _tuple18 _tmpAB2;_tmpAB2.f1=(_tmpAAD->tq).real_const  || _tmpAB0;_tmpAB2.f2=_tmpAB1;_tmpAB2;});}
# 5455
return bogus_ans;}}else{goto _LL8D1;}default: _LL8D1: _LL8D2:
 return bogus_ans;}_LL8CC:;};case 21: _LL8AD: _tmpA8D=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA89)->f1;_tmpA8E=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA89)->f2;_tmpA8F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA89)->f3;_LL8AE:
# 5460
 if(_tmpA8F)return bogus_ans;{
# 5464
void*_tmpAB3=Cyc_Tcutil_compress((void*)_check_null(_tmpA8D->topt));void*_tmpAB4=_tmpAB3;void*_tmpAB5;void*_tmpAB6;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAB4)->tag == 5){_LL8DA: _tmpAB5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAB4)->f1).elt_typ;_tmpAB6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAB4)->f1).ptr_atts).rgn;_LL8DB: {
# 5466
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmpAB7=Cyc_Tcutil_compress(_tmpAB5);void*_tmpAB8=_tmpAB7;struct Cyc_Absyn_Aggrdecl*_tmpAB9;struct Cyc_List_List*_tmpABA;switch(*((int*)_tmpAB8)){case 12: _LL8DF: _tmpABA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpAB8)->f2;_LL8E0:
# 5469
 finfo=Cyc_Absyn_lookup_field(_tmpABA,_tmpA8E);goto _LL8DE;case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAB8)->f1).aggr_info).KnownAggr).tag == 2){_LL8E1: _tmpAB9=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAB8)->f1).aggr_info).KnownAggr).val;_LL8E2:
# 5471
 finfo=Cyc_Absyn_lookup_decl_field(_tmpAB9,_tmpA8E);goto _LL8DE;}else{goto _LL8E3;}default: _LL8E3: _LL8E4:
 return bogus_ans;}_LL8DE:;}
# 5474
if(finfo != 0  && finfo->width == 0)
return({struct _tuple18 _tmpABB;_tmpABB.f1=(finfo->tq).real_const;_tmpABB.f2=_tmpAB6;_tmpABB;});
return bogus_ans;}}else{_LL8DC: _LL8DD:
 return bogus_ans;}_LL8D9:;};case 19: _LL8AF: _tmpA8C=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpA89)->f1;_LL8B0: {
# 5481
void*_tmpABC=Cyc_Tcutil_compress((void*)_check_null(_tmpA8C->topt));void*_tmpABD=_tmpABC;struct Cyc_Absyn_Tqual _tmpABE;void*_tmpABF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpABD)->tag == 5){_LL8E6: _tmpABE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpABD)->f1).elt_tq;_tmpABF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpABD)->f1).ptr_atts).rgn;_LL8E7:
# 5483
 return({struct _tuple18 _tmpAC0;_tmpAC0.f1=_tmpABE.real_const;_tmpAC0.f2=_tmpABF;_tmpAC0;});}else{_LL8E8: _LL8E9:
 return bogus_ans;}_LL8E5:;}case 22: _LL8B1: _tmpA8A=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA89)->f1;_tmpA8B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA89)->f2;_LL8B2: {
# 5489
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmpA8A->topt));
void*_tmpAC1=t;struct Cyc_Absyn_Tqual _tmpAC2;struct Cyc_Absyn_Tqual _tmpAC3;void*_tmpAC4;struct Cyc_List_List*_tmpAC5;switch(*((int*)_tmpAC1)){case 10: _LL8EB: _tmpAC5=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpAC1)->f1;_LL8EC: {
# 5493
struct _tuple14 _tmpAC6=Cyc_Evexp_eval_const_uint_exp(_tmpA8B);struct _tuple14 _tmpAC7=_tmpAC6;unsigned int _tmpAC8;int _tmpAC9;_LL8F4: _tmpAC8=_tmpAC7.f1;_tmpAC9=_tmpAC7.f2;_LL8F5:;
if(!_tmpAC9)
return bogus_ans;{
struct _tuple12*_tmpACA=Cyc_Absyn_lookup_tuple_field(_tmpAC5,(int)_tmpAC8);
if(_tmpACA != 0)
return({struct _tuple18 _tmpACB;_tmpACB.f1=((*_tmpACA).f1).real_const;_tmpACB.f2=(Cyc_Tcutil_addressof_props(te,_tmpA8A)).f2;_tmpACB;});
return bogus_ans;};}case 5: _LL8ED: _tmpAC3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAC1)->f1).elt_tq;_tmpAC4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAC1)->f1).ptr_atts).rgn;_LL8EE:
# 5501
 return({struct _tuple18 _tmpACC;_tmpACC.f1=_tmpAC3.real_const;_tmpACC.f2=_tmpAC4;_tmpACC;});case 8: _LL8EF: _tmpAC2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAC1)->f1).tq;_LL8F0:
# 5507
 return({struct _tuple18 _tmpACD;_tmpACD.f1=_tmpAC2.real_const;_tmpACD.f2=(Cyc_Tcutil_addressof_props(te,_tmpA8A)).f2;_tmpACD;});default: _LL8F1: _LL8F2:
 return bogus_ans;}_LL8EA:;}default: _LL8B3: _LL8B4:
# 5511
({void*_tmpACE=0;Cyc_Tcutil_terr(e->loc,({const char*_tmpACF="unary & applied to non-lvalue";_tag_dyneither(_tmpACF,sizeof(char),30);}),_tag_dyneither(_tmpACE,sizeof(void*),0));});
return bogus_ans;}_LL8A8:;}
# 5518
void Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,union Cyc_Absyn_Constraint*b){
b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(b);{
void*_tmpAD1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);void*_tmpAD2=_tmpAD1;struct Cyc_Absyn_Exp*_tmpAD3;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpAD2)->tag == 0){_LL8F7: _LL8F8:
 return;}else{_LL8F9: _tmpAD3=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpAD2)->f1;_LL8FA: {
# 5523
struct _tuple14 _tmpAD4=Cyc_Evexp_eval_const_uint_exp(_tmpAD3);struct _tuple14 _tmpAD5=_tmpAD4;unsigned int _tmpAD6;int _tmpAD7;_LL8FC: _tmpAD6=_tmpAD5.f1;_tmpAD7=_tmpAD5.f2;_LL8FD:;
if(_tmpAD7  && _tmpAD6 <= i)
({struct Cyc_Int_pa_PrintArg_struct _tmpADB;_tmpADB.tag=1;_tmpADB.f1=(unsigned long)((int)i);({struct Cyc_Int_pa_PrintArg_struct _tmpADA;_tmpADA.tag=1;_tmpADA.f1=(unsigned long)((int)_tmpAD6);({void*_tmpAD8[2]={& _tmpADA,& _tmpADB};Cyc_Tcutil_terr(loc,({const char*_tmpAD9="dereference is out of bounds: %d <= %d";_tag_dyneither(_tmpAD9,sizeof(char),39);}),_tag_dyneither(_tmpAD8,sizeof(void*),2));});});});
return;}}_LL8F6:;};}
# 5530
void Cyc_Tcutil_check_nonzero_bound(unsigned int loc,union Cyc_Absyn_Constraint*b){
Cyc_Tcutil_check_bound(loc,0,b);}
# 5534
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b){
void*_tmpADC=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);void*_tmpADD=_tmpADC;struct Cyc_Absyn_Exp*_tmpADE;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpADD)->tag == 1){_LL8FF: _tmpADE=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpADD)->f1;_LL900: {
# 5537
struct _tuple14 _tmpADF=Cyc_Evexp_eval_const_uint_exp(_tmpADE);struct _tuple14 _tmpAE0=_tmpADF;unsigned int _tmpAE1;int _tmpAE2;_LL904: _tmpAE1=_tmpAE0.f1;_tmpAE2=_tmpAE0.f2;_LL905:;
return _tmpAE2  && _tmpAE1 == 1;}}else{_LL901: _LL902:
 return 0;}_LL8FE:;}
# 5543
int Cyc_Tcutil_bits_only(void*t){
void*_tmpAE3=Cyc_Tcutil_compress(t);void*_tmpAE4=_tmpAE3;struct Cyc_List_List*_tmpAE5;struct Cyc_Absyn_Aggrdecl*_tmpAE6;struct Cyc_List_List*_tmpAE7;struct Cyc_List_List*_tmpAE8;void*_tmpAE9;union Cyc_Absyn_Constraint*_tmpAEA;switch(*((int*)_tmpAE4)){case 0: _LL907: _LL908:
 goto _LL90A;case 6: _LL909: _LL90A:
 goto _LL90C;case 7: _LL90B: _LL90C:
 return 1;case 13: _LL90D: _LL90E:
 goto _LL910;case 14: _LL90F: _LL910:
 return 0;case 8: _LL911: _tmpAE9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAE4)->f1).elt_type;_tmpAEA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAE4)->f1).zero_term;_LL912:
# 5553
 return !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpAEA) && Cyc_Tcutil_bits_only(_tmpAE9);case 10: _LL913: _tmpAE8=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpAE4)->f1;_LL914:
# 5555
 for(0;_tmpAE8 != 0;_tmpAE8=_tmpAE8->tl){
if(!Cyc_Tcutil_bits_only((*((struct _tuple12*)_tmpAE8->hd)).f2))return 0;}
return 1;case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAE4)->f1).aggr_info).UnknownAggr).tag == 1){_LL915: _LL916:
 return 0;}else{_LL917: _tmpAE6=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAE4)->f1).aggr_info).KnownAggr).val;_tmpAE7=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAE4)->f1).targs;_LL918:
# 5560
 if(_tmpAE6->impl == 0)
return 0;{
int okay=1;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAE6->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
if(okay)return 1;{
struct _RegionHandle _tmpAEB=_new_region("rgn");struct _RegionHandle*rgn=& _tmpAEB;_push_region(rgn);
{struct Cyc_List_List*_tmpAEC=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpAE6->tvs,_tmpAE7);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAE6->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpAEC,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpAED=0;_npop_handler(0);return _tmpAED;}}}{
int _tmpAEE=1;_npop_handler(0);return _tmpAEE;};}
# 5567
;_pop_region(rgn);};};}case 12: _LL919: _tmpAE5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpAE4)->f2;_LL91A:
# 5573
 for(0;_tmpAE5 != 0;_tmpAE5=_tmpAE5->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)_tmpAE5->hd)->type))return 0;}
return 1;default: _LL91B: _LL91C:
 return 0;}_LL906:;}
# 5581
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e){
while(1){
void*_tmpAEF=e->r;void*_tmpAF0=_tmpAEF;struct Cyc_Absyn_Exp*_tmpAF1;struct Cyc_Absyn_Exp*_tmpAF2;switch(*((int*)_tmpAF0)){case 1: _LL91E: _LL91F:
 return 1;case 11: _LL920: _tmpAF2=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpAF0)->f1;_LL921:
 _tmpAF1=_tmpAF2;goto _LL923;case 12: _LL922: _tmpAF1=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpAF0)->f1;_LL923:
 e=_tmpAF1;continue;default: _LL924: _LL925:
# 5588
 return 0;}_LL91D:;}}
# 5598
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmpAF3=e->r;void*_tmpAF4=_tmpAF3;struct Cyc_List_List*_tmpAF5;struct Cyc_List_List*_tmpAF6;enum Cyc_Absyn_Primop _tmpAF7;struct Cyc_List_List*_tmpAF8;struct Cyc_List_List*_tmpAF9;struct Cyc_List_List*_tmpAFA;struct Cyc_List_List*_tmpAFB;struct Cyc_Absyn_Exp*_tmpAFC;struct Cyc_Absyn_Exp*_tmpAFD;struct Cyc_Absyn_Exp*_tmpAFE;struct Cyc_Absyn_Exp*_tmpAFF;void*_tmpB00;struct Cyc_Absyn_Exp*_tmpB01;void*_tmpB02;struct Cyc_Absyn_Exp*_tmpB03;struct Cyc_Absyn_Exp*_tmpB04;struct Cyc_Absyn_Exp*_tmpB05;struct Cyc_Absyn_Exp*_tmpB06;struct Cyc_Absyn_Exp*_tmpB07;struct Cyc_Absyn_Exp*_tmpB08;struct Cyc_Absyn_Exp*_tmpB09;struct Cyc_Absyn_Exp*_tmpB0A;struct _tuple2*_tmpB0B;void*_tmpB0C;switch(*((int*)_tmpAF4)){case 0: _LL927: _LL928:
 goto _LL92A;case 16: _LL929: _LL92A:
 goto _LL92C;case 17: _LL92B: _LL92C:
 goto _LL92E;case 18: _LL92D: _LL92E:
 goto _LL930;case 31: _LL92F: _LL930:
 goto _LL932;case 32: _LL931: _LL932:
 return 1;case 1: _LL933: _tmpB0B=((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpAF4)->f1;_tmpB0C=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpAF4)->f2;_LL934: {
# 5609
void*_tmpB0D=_tmpB0C;struct Cyc_Absyn_Vardecl*_tmpB0E;struct Cyc_Absyn_Vardecl*_tmpB0F;switch(*((int*)_tmpB0D)){case 2: _LL956: _LL957:
 return 1;case 1: _LL958: _tmpB0F=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpB0D)->f1;_LL959: {
# 5612
void*_tmpB10=Cyc_Tcutil_compress(_tmpB0F->type);void*_tmpB11=_tmpB10;switch(*((int*)_tmpB11)){case 8: _LL961: _LL962:
 goto _LL964;case 9: _LL963: _LL964:
 return 1;default: _LL965: _LL966:
 return var_okay;}_LL960:;}case 4: _LL95A: _tmpB0E=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpB0D)->f1;_LL95B:
# 5619
 if(_tmpB0E->sc == Cyc_Absyn_Static){
void*_tmpB12=Cyc_Tcutil_compress(_tmpB0E->type);void*_tmpB13=_tmpB12;switch(*((int*)_tmpB13)){case 8: _LL968: _LL969:
 goto _LL96B;case 9: _LL96A: _LL96B:
 return 1;default: _LL96C: _LL96D:
 return var_okay;}_LL967:;}else{
# 5626
return var_okay;}case 0: _LL95C: _LL95D:
 return 0;default: _LL95E: _LL95F:
 return var_okay;}_LL955:;}case 5: _LL935: _tmpB08=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAF4)->f1;_tmpB09=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAF4)->f2;_tmpB0A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAF4)->f3;_LL936:
# 5632
 return(Cyc_Tcutil_cnst_exp(0,_tmpB08) && 
Cyc_Tcutil_cnst_exp(0,_tmpB09)) && 
Cyc_Tcutil_cnst_exp(0,_tmpB0A);case 8: _LL937: _tmpB06=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpAF4)->f1;_tmpB07=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpAF4)->f2;_LL938:
# 5636
 return Cyc_Tcutil_cnst_exp(0,_tmpB06) && Cyc_Tcutil_cnst_exp(0,_tmpB07);case 11: _LL939: _tmpB05=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpAF4)->f1;_LL93A:
 _tmpB04=_tmpB05;goto _LL93C;case 12: _LL93B: _tmpB04=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpAF4)->f1;_LL93C:
# 5639
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpB04);case 13: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAF4)->f4 == Cyc_Absyn_No_coercion){_LL93D: _tmpB02=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAF4)->f1;_tmpB03=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAF4)->f2;_LL93E:
# 5641
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpB03);}else{_LL93F: _tmpB00=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAF4)->f1;_tmpB01=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAF4)->f2;_LL940:
# 5644
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpB01);}case 14: _LL941: _tmpAFF=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpAF4)->f1;_LL942:
# 5646
 return Cyc_Tcutil_cnst_exp(1,_tmpAFF);case 26: _LL943: _tmpAFD=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpAF4)->f2;_tmpAFE=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpAF4)->f3;_LL944:
# 5648
 return Cyc_Tcutil_cnst_exp(0,_tmpAFD) && Cyc_Tcutil_cnst_exp(0,_tmpAFE);case 27: _LL945: _tmpAFC=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpAF4)->f1;_LL946:
# 5650
 return Cyc_Tcutil_cnst_exp(0,_tmpAFC);case 25: _LL947: _tmpAFB=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpAF4)->f1;_LL948:
 _tmpAFA=_tmpAFB;goto _LL94A;case 29: _LL949: _tmpAFA=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpAF4)->f2;_LL94A:
 _tmpAF9=_tmpAFA;goto _LL94C;case 28: _LL94B: _tmpAF9=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpAF4)->f3;_LL94C:
# 5654
 for(0;_tmpAF9 != 0;_tmpAF9=_tmpAF9->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple19*)_tmpAF9->hd)).f2))
return 0;}
return 1;case 2: _LL94D: _tmpAF7=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpAF4)->f1;_tmpAF8=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpAF4)->f2;_LL94E:
# 5659
 _tmpAF6=_tmpAF8;goto _LL950;case 23: _LL94F: _tmpAF6=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpAF4)->f1;_LL950:
 _tmpAF5=_tmpAF6;goto _LL952;case 30: _LL951: _tmpAF5=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpAF4)->f1;_LL952:
# 5662
 for(0;_tmpAF5 != 0;_tmpAF5=_tmpAF5->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmpAF5->hd))
return 0;}
return 1;default: _LL953: _LL954:
 return 0;}_LL926:;}
# 5670
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 5674
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);
# 5676
int Cyc_Tcutil_supports_default(void*t){
void*_tmpB14=Cyc_Tcutil_compress(t);void*_tmpB15=_tmpB14;struct Cyc_List_List*_tmpB16;union Cyc_Absyn_AggrInfoU _tmpB17;struct Cyc_List_List*_tmpB18;struct Cyc_List_List*_tmpB19;void*_tmpB1A;union Cyc_Absyn_Constraint*_tmpB1B;union Cyc_Absyn_Constraint*_tmpB1C;switch(*((int*)_tmpB15)){case 0: _LL96F: _LL970:
 goto _LL972;case 6: _LL971: _LL972:
 goto _LL974;case 7: _LL973: _LL974:
 return 1;case 5: _LL975: _tmpB1B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB15)->f1).ptr_atts).nullable;_tmpB1C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB15)->f1).ptr_atts).bounds;_LL976: {
# 5683
void*_tmpB1D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpB1C);void*_tmpB1E=_tmpB1D;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpB1E)->tag == 0){_LL986: _LL987:
 return 1;}else{_LL988: _LL989:
# 5686
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmpB1B);}_LL985:;}case 8: _LL977: _tmpB1A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB15)->f1).elt_type;_LL978:
# 5689
 return Cyc_Tcutil_supports_default(_tmpB1A);case 10: _LL979: _tmpB19=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpB15)->f1;_LL97A:
# 5691
 for(0;_tmpB19 != 0;_tmpB19=_tmpB19->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple12*)_tmpB19->hd)).f2))return 0;}
return 1;case 11: _LL97B: _tmpB17=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpB15)->f1).aggr_info;_tmpB18=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpB15)->f1).targs;_LL97C: {
# 5695
struct Cyc_Absyn_Aggrdecl*_tmpB1F=Cyc_Absyn_get_known_aggrdecl(_tmpB17);
if(_tmpB1F->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB1F->impl))->exist_vars != 0)return 0;
return Cyc_Tcutil_fields_support_default(_tmpB1F->tvs,_tmpB18,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB1F->impl))->fields);}case 12: _LL97D: _tmpB16=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpB15)->f2;_LL97E:
 return Cyc_Tcutil_fields_support_default(0,0,_tmpB16);case 14: _LL97F: _LL980:
# 5701
 goto _LL982;case 13: _LL981: _LL982:
 return 1;default: _LL983: _LL984:
# 5704
 return 0;}_LL96E:;}
# 5709
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t){
void*_tmpB20=t;struct Cyc_Absyn_Typedefdecl*_tmpB21;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpB20)->tag == 17){_LL98B: _tmpB21=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpB20)->f3;_LL98C:
# 5712
 if(_tmpB21 != 0){
struct Cyc_Absyn_Tqual _tmpB22=_tmpB21->tq;
if(((_tmpB22.print_const  || _tmpB22.q_volatile) || _tmpB22.q_restrict) || _tmpB22.real_const)
# 5717
({struct Cyc_String_pa_PrintArg_struct _tmpB25;_tmpB25.tag=0;_tmpB25.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmpB23[1]={& _tmpB25};Cyc_Tcutil_warn(loc,({const char*_tmpB24="qualifier within typedef type %s is ignored";_tag_dyneither(_tmpB24,sizeof(char),44);}),_tag_dyneither(_tmpB23,sizeof(void*),1));});});}
# 5720
goto _LL98A;}else{_LL98D: _LL98E:
 goto _LL98A;}_LL98A:;}
# 5725
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 5727
struct _RegionHandle _tmpB26=_new_region("rgn");struct _RegionHandle*rgn=& _tmpB26;_push_region(rgn);{
struct Cyc_List_List*_tmpB27=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_supports_default(t)){int _tmpB28=1;_npop_handler(0);return _tmpB28;}
t=Cyc_Tcutil_rsubstitute(rgn,_tmpB27,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpB29=0;_npop_handler(0);return _tmpB29;}}}{
# 5736
int _tmpB2A=1;_npop_handler(0);return _tmpB2A;};
# 5727
;_pop_region(rgn);}
# 5742
int Cyc_Tcutil_admits_zero(void*t){
void*_tmpB2B=Cyc_Tcutil_compress(t);void*_tmpB2C=_tmpB2B;union Cyc_Absyn_Constraint*_tmpB2D;union Cyc_Absyn_Constraint*_tmpB2E;switch(*((int*)_tmpB2C)){case 6: _LL990: _LL991:
 goto _LL993;case 7: _LL992: _LL993:
 return 1;case 5: _LL994: _tmpB2D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB2C)->f1).ptr_atts).nullable;_tmpB2E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB2C)->f1).ptr_atts).bounds;_LL995: {
# 5747
void*_tmpB2F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpB2E);void*_tmpB30=_tmpB2F;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpB30)->tag == 0){_LL999: _LL99A:
# 5751
 return 0;}else{_LL99B: _LL99C:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpB2D);}_LL998:;}default: _LL996: _LL997:
# 5754
 return 0;}_LL98F:;}
# 5758
int Cyc_Tcutil_is_noreturn(void*t){
{void*_tmpB31=Cyc_Tcutil_compress(t);void*_tmpB32=_tmpB31;struct Cyc_List_List*_tmpB33;void*_tmpB34;switch(*((int*)_tmpB32)){case 5: _LL99E: _tmpB34=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB32)->f1).elt_typ;_LL99F:
 return Cyc_Tcutil_is_noreturn(_tmpB34);case 9: _LL9A0: _tmpB33=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB32)->f1).attributes;_LL9A1:
# 5762
 for(0;_tmpB33 != 0;_tmpB33=_tmpB33->tl){
void*_tmpB35=(void*)_tmpB33->hd;void*_tmpB36=_tmpB35;if(((struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmpB36)->tag == 4){_LL9A5: _LL9A6:
 return 1;}else{_LL9A7: _LL9A8:
 continue;}_LL9A4:;}
# 5767
goto _LL99D;default: _LL9A2: _LL9A3:
 goto _LL99D;}_LL99D:;}
# 5770
return 0;}
# 5775
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmpB37=Cyc_Tcutil_compress(t);void*_tmpB38=_tmpB37;struct Cyc_List_List**_tmpB39;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB38)->tag == 9){_LL9AA: _tmpB39=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB38)->f1).attributes;_LL9AB: {
# 5778
struct Cyc_List_List*_tmpB3A=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Tcutil_attribute_cmp,*_tmpB39,(void*)atts->hd))
*_tmpB39=({struct Cyc_List_List*_tmpB3B=_cycalloc(sizeof(*_tmpB3B));_tmpB3B->hd=(void*)atts->hd;_tmpB3B->tl=*_tmpB39;_tmpB3B;});}else{
# 5785
_tmpB3A=({struct Cyc_List_List*_tmpB3C=_cycalloc(sizeof(*_tmpB3C));_tmpB3C->hd=(void*)atts->hd;_tmpB3C->tl=_tmpB3A;_tmpB3C;});}}
return _tmpB3A;}}else{_LL9AC: _LL9AD:
({void*_tmpB3D=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpB3E="transfer_fn_type_atts";_tag_dyneither(_tmpB3E,sizeof(char),22);}),_tag_dyneither(_tmpB3D,sizeof(void*),0));});}_LL9A9:;}
# 5792
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
struct Cyc_Absyn_Exp*bound=0;
{void*_tmpB3F=Cyc_Tcutil_compress(t);void*_tmpB40=_tmpB3F;struct Cyc_Absyn_Exp*_tmpB41;struct Cyc_Absyn_PtrInfo*_tmpB42;switch(*((int*)_tmpB40)){case 5: _LL9AF: _tmpB42=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB40)->f1;_LL9B0:
# 5796
{void*_tmpB43=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)((_tmpB42->ptr_atts).bounds);void*_tmpB44=_tmpB43;struct Cyc_Absyn_Exp*_tmpB45;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpB44)->tag == 1){_LL9B6: _tmpB45=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpB44)->f1;_LL9B7:
 bound=_tmpB45;goto _LL9B5;}else{_LL9B8: _LL9B9:
 goto _LL9B5;}_LL9B5:;}
# 5800
goto _LL9AE;case 8: _LL9B1: _tmpB41=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB40)->f1).num_elts;_LL9B2:
# 5802
 bound=_tmpB41;
goto _LL9AE;default: _LL9B3: _LL9B4:
 goto _LL9AE;}_LL9AE:;}
# 5806
return bound;}
# 5811
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
{void*_tmpB46=b;struct Cyc_Absyn_Vardecl*_tmpB47;struct Cyc_Absyn_Vardecl*_tmpB48;struct Cyc_Absyn_Vardecl*_tmpB49;struct Cyc_Absyn_Vardecl*_tmpB4A;switch(*((int*)_tmpB46)){case 5: _LL9BB: _tmpB4A=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpB46)->f1;_LL9BC:
 _tmpB49=_tmpB4A;goto _LL9BE;case 4: _LL9BD: _tmpB49=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpB46)->f1;_LL9BE:
 _tmpB48=_tmpB49;goto _LL9C0;case 3: _LL9BF: _tmpB48=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpB46)->f1;_LL9C0:
 _tmpB47=_tmpB48;goto _LL9C2;case 1: _LL9C1: _tmpB47=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpB46)->f1;_LL9C2:
# 5817
 if(!_tmpB47->escapes)return _tmpB47;
goto _LL9BA;default: _LL9C3: _LL9C4:
 goto _LL9BA;}_LL9BA:;}
# 5821
return 0;}
# 5825
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmpB4B=0;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0)_tmpB4B=({struct Cyc_List_List*_tmpB4C=_cycalloc(sizeof(*_tmpB4C));_tmpB4C->hd=*((void**)_check_null((void**)x->hd));_tmpB4C->tl=_tmpB4B;_tmpB4C;});}}
return _tmpB4B;}
# 5832
int Cyc_Tcutil_is_array(void*t){
# 5834
void*_tmpB4D=Cyc_Tcutil_compress(t);void*_tmpB4E=_tmpB4D;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB4E)->tag == 8){_LL9C6: _LL9C7:
# 5836
 return 1;}else{_LL9C8: _LL9C9:
 return 0;}_LL9C5:;}
# 5841
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
# 5843
void*_tmpB4F=Cyc_Tcutil_compress(t);void*_tmpB50=_tmpB4F;void*_tmpB51;struct Cyc_Absyn_Tqual _tmpB52;struct Cyc_Absyn_Exp*_tmpB53;union Cyc_Absyn_Constraint*_tmpB54;unsigned int _tmpB55;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB50)->tag == 8){_LL9CB: _tmpB51=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB50)->f1).elt_type;_tmpB52=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB50)->f1).tq;_tmpB53=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB50)->f1).num_elts;_tmpB54=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB50)->f1).zero_term;_tmpB55=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB50)->f1).zt_loc;_LL9CC: {
# 5845
void*b;
if(_tmpB53 == 0)
b=(void*)& Cyc_Absyn_DynEither_b_val;else{
# 5849
struct Cyc_Absyn_Exp*bnd=_tmpB53;
if(convert_tag){
if(bnd->topt == 0)
({void*_tmpB56=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpB57="cannot convert tag without type!";_tag_dyneither(_tmpB57,sizeof(char),33);}),_tag_dyneither(_tmpB56,sizeof(void*),0));});{
void*_tmpB58=Cyc_Tcutil_compress((void*)_check_null(bnd->topt));void*_tmpB59=_tmpB58;void*_tmpB5A;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpB59)->tag == 19){_LL9D0: _tmpB5A=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpB59)->f1;_LL9D1:
# 5855
 b=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpB5B=_cycalloc(sizeof(*_tmpB5B));_tmpB5B[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpB5C;_tmpB5C.tag=1;_tmpB5C.f1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmpB5A,0),0,Cyc_Absyn_No_coercion,0);_tmpB5C;});_tmpB5B;});
goto _LL9CF;}else{_LL9D2: _LL9D3:
# 5858
 if(Cyc_Tcutil_is_const_exp(bnd))
b=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpB5D=_cycalloc(sizeof(*_tmpB5D));_tmpB5D[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpB5E;_tmpB5E.tag=1;_tmpB5E.f1=bnd;_tmpB5E;});_tmpB5D;});else{
# 5861
b=(void*)& Cyc_Absyn_DynEither_b_val;}}_LL9CF:;};}else{
# 5865
b=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpB5F=_cycalloc(sizeof(*_tmpB5F));_tmpB5F[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpB60;_tmpB60.tag=1;_tmpB60.f1=bnd;_tmpB60;});_tmpB5F;});}}
# 5867
return Cyc_Absyn_atb_typ(_tmpB51,rgn,_tmpB52,b,_tmpB54);}}else{_LL9CD: _LL9CE:
# 5870
 return t;}_LL9CA:;}
# 5875
int Cyc_Tcutil_zeroable_type(void*t){
void*_tmpB61=Cyc_Tcutil_compress(t);void*_tmpB62=_tmpB61;struct Cyc_List_List*_tmpB63;union Cyc_Absyn_AggrInfoU _tmpB64;struct Cyc_List_List*_tmpB65;struct Cyc_List_List*_tmpB66;void*_tmpB67;union Cyc_Absyn_Constraint*_tmpB68;switch(*((int*)_tmpB62)){case 0: _LL9D5: _LL9D6:
 return 1;case 1: _LL9D7: _LL9D8:
 goto _LL9DA;case 2: _LL9D9: _LL9DA:
 goto _LL9DC;case 3: _LL9DB: _LL9DC:
 goto _LL9DE;case 4: _LL9DD: _LL9DE:
 return 0;case 5: _LL9DF: _tmpB68=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB62)->f1).ptr_atts).nullable;_LL9E0:
# 5883
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmpB68);case 6: _LL9E1: _LL9E2:
 goto _LL9E4;case 7: _LL9E3: _LL9E4:
 return 1;case 8: _LL9E5: _tmpB67=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB62)->f1).elt_type;_LL9E6:
 return Cyc_Tcutil_zeroable_type(_tmpB67);case 9: _LL9E7: _LL9E8:
 return 0;case 10: _LL9E9: _tmpB66=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpB62)->f1;_LL9EA:
# 5889
 for(0;(unsigned int)_tmpB66;_tmpB66=_tmpB66->tl){
if(!Cyc_Tcutil_zeroable_type((*((struct _tuple12*)_tmpB66->hd)).f2))return 0;}
return 1;case 11: _LL9EB: _tmpB64=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpB62)->f1).aggr_info;_tmpB65=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpB62)->f1).targs;_LL9EC: {
# 5893
struct Cyc_Absyn_Aggrdecl*_tmpB69=Cyc_Absyn_get_known_aggrdecl(_tmpB64);
if(_tmpB69->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB69->impl))->exist_vars != 0)
return 0;{
struct _RegionHandle _tmpB6A=_new_region("r");struct _RegionHandle*r=& _tmpB6A;_push_region(r);
{struct Cyc_List_List*_tmpB6B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmpB69->tvs,_tmpB65);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB69->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmpB6B,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpB6C=0;_npop_handler(0);return _tmpB6C;}}}{
int _tmpB6D=1;_npop_handler(0);return _tmpB6D;};}
# 5897
;_pop_region(r);};}case 13: _LL9ED: _LL9EE:
# 5902
 return 1;case 19: _LL9EF: _LL9F0:
 return 1;case 12: _LL9F1: _tmpB63=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpB62)->f2;_LL9F2:
# 5905
 for(0;_tmpB63 != 0;_tmpB63=_tmpB63->tl){
if(!Cyc_Tcutil_zeroable_type(((struct Cyc_Absyn_Aggrfield*)_tmpB63->hd)->type))return 0;}
return 1;case 14: _LL9F3: _LL9F4:
 return 1;case 17: _LL9F5: _LL9F6:
 return 0;case 16: _LL9F7: _LL9F8:
 return 0;case 15: _LL9F9: _LL9FA:
 return 0;case 28: _LL9FB: _LL9FC:
 return 0;case 26: _LL9FD: _LL9FE:
# 5914
 goto _LLA00;case 27: _LL9FF: _LLA00:
 goto _LLA02;case 18: _LLA01: _LLA02:
 goto _LLA04;case 20: _LLA03: _LLA04:
 goto _LLA06;case 21: _LLA05: _LLA06:
 goto _LLA08;case 22: _LLA07: _LLA08:
 goto _LLA0A;case 23: _LLA09: _LLA0A:
 goto _LLA0C;case 24: _LLA0B: _LLA0C:
 goto _LLA0E;default: _LLA0D: _LLA0E:
({struct Cyc_String_pa_PrintArg_struct _tmpB70;_tmpB70.tag=0;_tmpB70.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmpB6E[1]={& _tmpB70};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpB6F="bad type `%s' in zeroable type";_tag_dyneither(_tmpB6F,sizeof(char),31);}),_tag_dyneither(_tmpB6E,sizeof(void*),1));});});}_LL9D4:;}
