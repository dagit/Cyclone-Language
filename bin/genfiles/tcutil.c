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
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 320
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 339
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 351
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 372
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 377
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 432 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 448
typedef void*Cyc_Absyn_funcparams_t;
# 451
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 515
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 530
extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;
# 541
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 548
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 566
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 922 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 924
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 932
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 935
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x);
# 939
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
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
# 960
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 962
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;
# 966
extern void*Cyc_Absyn_empty_effect;
# 976
extern struct _tuple2*Cyc_Absyn_datatype_print_arg_qvar;
extern struct _tuple2*Cyc_Absyn_datatype_scanf_arg_qvar;
# 985
extern void*Cyc_Absyn_bounds_one;
# 990
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 1016
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1026
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1033
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned int);
# 1035
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
# 1038
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1048
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1050
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned int);
# 1055
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1061
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
# 1073
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
# 1079
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1119
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);
# 1166
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1168
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1170
struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1172
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
# 1174
int Cyc_Absyn_fntype_att(void*a);struct _tuple13{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;};
# 1178
struct _tuple13 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1180
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
void*_tmp37=t;void*_tmp40;struct Cyc_Absyn_Exp*_tmp3F;struct Cyc_Absyn_Exp*_tmp3E;void**_tmp3D;void**_tmp3C;switch(*((int*)_tmp37)){case 1: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp37)->f2 == 0){_LL1: _LL2:
 goto _LL4;}else{_LL7: _tmp3C=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp37)->f2;_LL8: {
# 193
void*ta=(void*)_check_null(*_tmp3C);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp3C=t2;
return t2;}}case 17: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp37)->f4 == 0){_LL3: _LL4:
# 185
 return t;}else{_LL5: _tmp3D=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp37)->f4;_LL6: {
# 187
void*ta=(void*)_check_null(*_tmp3D);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp3D=t2;
return t2;}}case 18: _LL9: _tmp3E=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp37)->f1;_LLA:
# 199
 Cyc_Evexp_eval_const_uint_exp(_tmp3E);{
void*_tmp38=_tmp3E->r;void*_tmp39=_tmp38;void*_tmp3A;if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp39)->tag == 38){_LL12: _tmp3A=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp39)->f1;_LL13:
 return Cyc_Tcutil_compress(_tmp3A);}else{_LL14: _LL15:
 return t;}_LL11:;};case 27: _LLB: _tmp3F=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp37)->f1;_LLC: {
# 205
void*_tmp3B=_tmp3F->topt;
if(_tmp3B != 0)return _tmp3B;else{
return t;}}case 26: if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp37)->f2 != 0){_LLD: _tmp40=*((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp37)->f2;_LLE:
# 209
 return Cyc_Tcutil_compress(_tmp40);}else{goto _LLF;}default: _LLF: _LL10:
 return t;}_LL0:;}
# 217
void*Cyc_Tcutil_copy_type(void*t);
static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){
return((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);}
# 221
static union Cyc_Absyn_Constraint*Cyc_Tcutil_copy_conref(union Cyc_Absyn_Constraint*cptr){
union Cyc_Absyn_Constraint*_tmp41=cptr;union Cyc_Absyn_Constraint*_tmp43;void*_tmp42;switch((((union Cyc_Absyn_Constraint*)_tmp41)->Eq_constr).tag){case 3: _LL17: _LL18:
 return Cyc_Absyn_empty_conref();case 1: _LL19: _tmp42=(void*)(_tmp41->Eq_constr).val;_LL1A:
 return Cyc_Absyn_new_conref(_tmp42);default: _LL1B: _tmp43=(_tmp41->Forward_constr).val;_LL1C:
 return Cyc_Tcutil_copy_conref(_tmp43);}_LL16:;}
# 228
static void*Cyc_Tcutil_copy_kindbound(void*kb){
void*_tmp44=Cyc_Absyn_compress_kb(kb);void*_tmp45=_tmp44;struct Cyc_Absyn_Kind*_tmp4A;switch(*((int*)_tmp45)){case 1: _LL1E: _LL1F:
 return(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46[0]=({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp47;_tmp47.tag=1;_tmp47.f1=0;_tmp47;});_tmp46;});case 2: _LL20: _tmp4A=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp45)->f2;_LL21:
 return(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp48=_cycalloc(sizeof(*_tmp48));_tmp48[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp49;_tmp49.tag=2;_tmp49.f1=0;_tmp49.f2=_tmp4A;_tmp49;});_tmp48;});default: _LL22: _LL23:
 return kb;}_LL1D:;}
# 235
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 237
return({struct Cyc_Absyn_Tvar*_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B->name=tv->name;_tmp4B->identity=- 1;_tmp4B->kind=Cyc_Tcutil_copy_kindbound(tv->kind);_tmp4B;});}
# 239
static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
# 241
struct _tuple10*_tmp4C=arg;struct _dyneither_ptr*_tmp50;struct Cyc_Absyn_Tqual _tmp4F;void*_tmp4E;_LL25: _tmp50=_tmp4C->f1;_tmp4F=_tmp4C->f2;_tmp4E=_tmp4C->f3;_LL26:;
return({struct _tuple10*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D->f1=_tmp50;_tmp4D->f2=_tmp4F;_tmp4D->f3=Cyc_Tcutil_copy_type(_tmp4E);_tmp4D;});}
# 244
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
struct _tuple12*_tmp51=arg;struct Cyc_Absyn_Tqual _tmp54;void*_tmp53;_LL28: _tmp54=_tmp51->f1;_tmp53=_tmp51->f2;_LL29:;
return({struct _tuple12*_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52->f1=_tmp54;_tmp52->f2=Cyc_Tcutil_copy_type(_tmp53);_tmp52;});}
# 248
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*);
# 250
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
return({struct Cyc_Absyn_Aggrfield*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55->name=f->name;_tmp55->tq=f->tq;_tmp55->type=Cyc_Tcutil_copy_type(f->type);_tmp55->width=f->width;_tmp55->attributes=f->attributes;_tmp55->requires_clause=
Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause);_tmp55;});}
# 254
static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
struct _tuple0*_tmp56=x;void*_tmp59;void*_tmp58;_LL2B: _tmp59=_tmp56->f1;_tmp58=_tmp56->f2;_LL2C:;
return({struct _tuple0*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->f1=Cyc_Tcutil_copy_type(_tmp59);_tmp57->f2=Cyc_Tcutil_copy_type(_tmp58);_tmp57;});}
# 258
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){
return({struct Cyc_Absyn_Enumfield*_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A->name=f->name;_tmp5A->tag=f->tag;_tmp5A->loc=f->loc;_tmp5A;});}
# 261
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp5B=_cycalloc(sizeof(*_tmp5B));_tmp5B[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp5C;_tmp5C.tag=2;_tmp5C.f1=Cyc_Tcutil_copy_tvar(t);_tmp5C;});_tmp5B;});}
# 265
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp5D=Cyc_Tcutil_compress(t);void*_tmp5E=_tmp5D;struct Cyc_Absyn_Datatypedecl*_tmpE4;struct Cyc_Absyn_Enumdecl*_tmpE3;struct Cyc_Absyn_Aggrdecl*_tmpE2;void*_tmpE1;struct Cyc_List_List*_tmpE0;void*_tmpDF;struct _tuple2*_tmpDE;struct Cyc_List_List*_tmpDD;struct Cyc_Absyn_Typedefdecl*_tmpDC;void*_tmpDB;void*_tmpDA;void*_tmpD9;struct Cyc_Absyn_Exp*_tmpD8;struct Cyc_Absyn_Exp*_tmpD7;void*_tmpD6;struct Cyc_List_List*_tmpD5;struct _tuple2*_tmpD4;struct Cyc_Absyn_Enumdecl*_tmpD3;enum Cyc_Absyn_AggrKind _tmpD2;struct Cyc_List_List*_tmpD1;struct Cyc_Absyn_Aggrdecl**_tmpD0;struct Cyc_List_List*_tmpCF;enum Cyc_Absyn_AggrKind _tmpCE;struct _tuple2*_tmpCD;struct Cyc_Core_Opt*_tmpCC;struct Cyc_List_List*_tmpCB;struct Cyc_List_List*_tmpCA;struct Cyc_List_List*_tmpC9;void*_tmpC8;struct Cyc_Absyn_Tqual _tmpC7;void*_tmpC6;struct Cyc_List_List*_tmpC5;int _tmpC4;struct Cyc_Absyn_VarargInfo*_tmpC3;struct Cyc_List_List*_tmpC2;struct Cyc_List_List*_tmpC1;struct Cyc_Absyn_Exp*_tmpC0;struct Cyc_List_List*_tmpBF;struct Cyc_Absyn_Exp*_tmpBE;struct Cyc_List_List*_tmpBD;void*_tmpBC;struct Cyc_Absyn_Tqual _tmpBB;struct Cyc_Absyn_Exp*_tmpBA;union Cyc_Absyn_Constraint*_tmpB9;unsigned int _tmpB8;void*_tmpB7;struct Cyc_Absyn_Tqual _tmpB6;void*_tmpB5;union Cyc_Absyn_Constraint*_tmpB4;union Cyc_Absyn_Constraint*_tmpB3;union Cyc_Absyn_Constraint*_tmpB2;struct Cyc_Absyn_PtrLoc*_tmpB1;union Cyc_Absyn_DatatypeFieldInfoU _tmpB0;struct Cyc_List_List*_tmpAF;union Cyc_Absyn_DatatypeInfoU _tmpAE;struct Cyc_List_List*_tmpAD;struct Cyc_Absyn_Tvar*_tmpAC;switch(*((int*)_tmp5E)){case 0: _LL2E: _LL2F:
 goto _LL31;case 1: _LL30: _LL31:
 return t;case 2: _LL32: _tmpAC=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp5E)->f1;_LL33:
 return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp60;_tmp60.tag=2;_tmp60.f1=Cyc_Tcutil_copy_tvar(_tmpAC);_tmp60;});_tmp5F;});case 3: _LL34: _tmpAE=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5E)->f1).datatype_info;_tmpAD=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5E)->f1).targs;_LL35:
# 271
 return(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61[0]=({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp62;_tmp62.tag=3;_tmp62.f1=({struct Cyc_Absyn_DatatypeInfo _tmp63;_tmp63.datatype_info=_tmpAE;_tmp63.targs=Cyc_Tcutil_copy_types(_tmpAD);_tmp63;});_tmp62;});_tmp61;});case 4: _LL36: _tmpB0=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp5E)->f1).field_info;_tmpAF=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp5E)->f1).targs;_LL37:
# 273
 return(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64[0]=({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp65;_tmp65.tag=4;_tmp65.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmp66;_tmp66.field_info=_tmpB0;_tmp66.targs=Cyc_Tcutil_copy_types(_tmpAF);_tmp66;});_tmp65;});_tmp64;});case 5: _LL38: _tmpB7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).elt_typ;_tmpB6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).elt_tq;_tmpB5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).ptr_atts).rgn;_tmpB4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).ptr_atts).nullable;_tmpB3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).ptr_atts).bounds;_tmpB2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).ptr_atts).zero_term;_tmpB1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).ptr_atts).ptrloc;_LL39: {
# 275
void*_tmp67=Cyc_Tcutil_copy_type(_tmpB7);
void*_tmp68=Cyc_Tcutil_copy_type(_tmpB5);
union Cyc_Absyn_Constraint*_tmp69=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmpB4);
struct Cyc_Absyn_Tqual _tmp6A=_tmpB6;
# 280
union Cyc_Absyn_Constraint*_tmp6B=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmpB3);
union Cyc_Absyn_Constraint*_tmp6C=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmpB2);
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp6E;_tmp6E.tag=5;_tmp6E.f1=({struct Cyc_Absyn_PtrInfo _tmp6F;_tmp6F.elt_typ=_tmp67;_tmp6F.elt_tq=_tmp6A;_tmp6F.ptr_atts=({(_tmp6F.ptr_atts).rgn=_tmp68;(_tmp6F.ptr_atts).nullable=_tmp69;(_tmp6F.ptr_atts).bounds=_tmp6B;(_tmp6F.ptr_atts).zero_term=_tmp6C;(_tmp6F.ptr_atts).ptrloc=_tmpB1;_tmp6F.ptr_atts;});_tmp6F;});_tmp6E;});_tmp6D;});}case 6: _LL3A: _LL3B:
 goto _LL3D;case 7: _LL3C: _LL3D:
 return t;case 8: _LL3E: _tmpBC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E)->f1).elt_type;_tmpBB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E)->f1).tq;_tmpBA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E)->f1).num_elts;_tmpB9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E)->f1).zero_term;_tmpB8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E)->f1).zt_loc;_LL3F: {
# 286
struct Cyc_Absyn_Exp*eopt2=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpBA);
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp71;_tmp71.tag=8;_tmp71.f1=({struct Cyc_Absyn_ArrayInfo _tmp72;_tmp72.elt_type=Cyc_Tcutil_copy_type(_tmpBC);_tmp72.tq=_tmpBB;_tmp72.num_elts=eopt2;_tmp72.zero_term=
((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmpB9);_tmp72.zt_loc=_tmpB8;_tmp72;});_tmp71;});_tmp70;});}case 9: _LL40: _tmpC9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).tvars;_tmpC8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).effect;_tmpC7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).ret_tqual;_tmpC6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).ret_typ;_tmpC5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).args;_tmpC4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).c_varargs;_tmpC3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).cyc_varargs;_tmpC2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).rgn_po;_tmpC1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).attributes;_tmpC0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).requires_clause;_tmpBF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).requires_relns;_tmpBE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).ensures_clause;_tmpBD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).ensures_relns;_LL41: {
# 290
struct Cyc_List_List*_tmp73=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmpC9);
void*effopt2=_tmpC8 == 0?0: Cyc_Tcutil_copy_type(_tmpC8);
void*_tmp74=Cyc_Tcutil_copy_type(_tmpC6);
struct Cyc_List_List*_tmp75=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmpC5);
int _tmp76=_tmpC4;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(_tmpC3 != 0){
struct Cyc_Absyn_VarargInfo*cv=_tmpC3;
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->name=cv->name;_tmp77->tq=cv->tq;_tmp77->type=Cyc_Tcutil_copy_type(cv->type);_tmp77->inject=cv->inject;_tmp77;});}{
# 301
struct Cyc_List_List*_tmp78=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmpC2);
struct Cyc_List_List*_tmp79=_tmpC1;
struct Cyc_Absyn_Exp*_tmp7A=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpC0);
struct Cyc_List_List*_tmp7B=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpBF);
struct Cyc_Absyn_Exp*_tmp7C=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpBE);
struct Cyc_List_List*_tmp7D=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpBD);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp7F;_tmp7F.tag=9;_tmp7F.f1=({struct Cyc_Absyn_FnInfo _tmp80;_tmp80.tvars=_tmp73;_tmp80.effect=effopt2;_tmp80.ret_tqual=_tmpC7;_tmp80.ret_typ=_tmp74;_tmp80.args=_tmp75;_tmp80.c_varargs=_tmp76;_tmp80.cyc_varargs=cyc_varargs2;_tmp80.rgn_po=_tmp78;_tmp80.attributes=_tmp79;_tmp80.requires_clause=_tmp7A;_tmp80.requires_relns=_tmp7B;_tmp80.ensures_clause=_tmp7C;_tmp80.ensures_relns=_tmp7D;_tmp80;});_tmp7F;});_tmp7E;});};}case 10: _LL42: _tmpCA=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5E)->f1;_LL43:
# 310
 return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81[0]=({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp82;_tmp82.tag=10;_tmp82.f1=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmpCA);_tmp82;});_tmp81;});case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5E)->f1).aggr_info).UnknownAggr).tag == 1){_LL44: _tmpCE=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5E)->f1).aggr_info).UnknownAggr).val).f1;_tmpCD=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5E)->f1).aggr_info).UnknownAggr).val).f2;_tmpCC=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5E)->f1).aggr_info).UnknownAggr).val).f3;_tmpCB=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5E)->f1).targs;_LL45:
# 312
 return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp84;_tmp84.tag=11;_tmp84.f1=({struct Cyc_Absyn_AggrInfo _tmp85;_tmp85.aggr_info=Cyc_Absyn_UnknownAggr(_tmpCE,_tmpCD,_tmpCC);_tmp85.targs=Cyc_Tcutil_copy_types(_tmpCB);_tmp85;});_tmp84;});_tmp83;});}else{_LL46: _tmpD0=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5E)->f1).aggr_info).KnownAggr).val;_tmpCF=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5E)->f1).targs;_LL47:
# 314
 return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp87;_tmp87.tag=11;_tmp87.f1=({struct Cyc_Absyn_AggrInfo _tmp88;_tmp88.aggr_info=Cyc_Absyn_KnownAggr(_tmpD0);_tmp88.targs=Cyc_Tcutil_copy_types(_tmpCF);_tmp88;});_tmp87;});_tmp86;});}case 12: _LL48: _tmpD2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5E)->f1;_tmpD1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5E)->f2;_LL49:
 return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89[0]=({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp8A;_tmp8A.tag=12;_tmp8A.f1=_tmpD2;_tmp8A.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmpD1);_tmp8A;});_tmp89;});case 13: _LL4A: _tmpD4=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5E)->f1;_tmpD3=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5E)->f2;_LL4B:
 return(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B[0]=({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp8C;_tmp8C.tag=13;_tmp8C.f1=_tmpD4;_tmp8C.f2=_tmpD3;_tmp8C;});_tmp8B;});case 14: _LL4C: _tmpD5=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp5E)->f1;_LL4D:
 return(void*)({struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D[0]=({struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmp8E;_tmp8E.tag=14;_tmp8E.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmpD5);_tmp8E;});_tmp8D;});case 19: _LL4E: _tmpD6=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5E)->f1;_LL4F:
 return(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F[0]=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp90;_tmp90.tag=19;_tmp90.f1=Cyc_Tcutil_copy_type(_tmpD6);_tmp90;});_tmp8F;});case 18: _LL50: _tmpD7=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp5E)->f1;_LL51:
# 321
 return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp92;_tmp92.tag=18;_tmp92.f1=_tmpD7;_tmp92;});_tmp91;});case 27: _LL52: _tmpD8=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp5E)->f1;_LL53:
# 324
 return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93[0]=({struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmp94;_tmp94.tag=27;_tmp94.f1=_tmpD8;_tmp94;});_tmp93;});case 15: _LL54: _tmpD9=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5E)->f1;_LL55:
 return(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95[0]=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp96;_tmp96.tag=15;_tmp96.f1=Cyc_Tcutil_copy_type(_tmpD9);_tmp96;});_tmp95;});case 16: _LL56: _tmpDB=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp5E)->f1;_tmpDA=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp5E)->f2;_LL57:
 return(void*)({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97[0]=({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp98;_tmp98.tag=16;_tmp98.f1=Cyc_Tcutil_copy_type(_tmpDB);_tmp98.f2=Cyc_Tcutil_copy_type(_tmpDA);_tmp98;});_tmp97;});case 17: _LL58: _tmpDE=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5E)->f1;_tmpDD=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5E)->f2;_tmpDC=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5E)->f3;_LL59:
# 328
 return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99[0]=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp9A;_tmp9A.tag=17;_tmp9A.f1=_tmpDE;_tmp9A.f2=Cyc_Tcutil_copy_types(_tmpDD);_tmp9A.f3=_tmpDC;_tmp9A.f4=0;_tmp9A;});_tmp99;});case 21: _LL5A: _LL5B:
 goto _LL5D;case 22: _LL5C: _LL5D:
 goto _LL5F;case 28: _LL5E: _LL5F:
 goto _LL61;case 20: _LL60: _LL61:
 return t;case 23: _LL62: _tmpDF=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp5E)->f1;_LL63:
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp9B=_cycalloc(sizeof(*_tmp9B));_tmp9B[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp9C;_tmp9C.tag=23;_tmp9C.f1=Cyc_Tcutil_copy_type(_tmpDF);_tmp9C;});_tmp9B;});case 24: _LL64: _tmpE0=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp5E)->f1;_LL65:
 return(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp9E;_tmp9E.tag=24;_tmp9E.f1=Cyc_Tcutil_copy_types(_tmpE0);_tmp9E;});_tmp9D;});case 25: _LL66: _tmpE1=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp5E)->f1;_LL67:
 return(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpA0;_tmpA0.tag=25;_tmpA0.f1=Cyc_Tcutil_copy_type(_tmpE1);_tmpA0;});_tmp9F;});default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5E)->f1)->r)){case 0: _LL68: _tmpE2=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5E)->f1)->r)->f1;_LL69: {
# 338
struct Cyc_List_List*_tmpA1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpE2->tvs);
return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpA3;_tmpA3.tag=11;_tmpA3.f1=({struct Cyc_Absyn_AggrInfo _tmpA4;_tmpA4.aggr_info=Cyc_Absyn_UnknownAggr(_tmpE2->kind,_tmpE2->name,0);_tmpA4.targs=_tmpA1;_tmpA4;});_tmpA3;});_tmpA2;});}case 1: _LL6A: _tmpE3=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5E)->f1)->r)->f1;_LL6B:
# 341
 return(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5[0]=({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpA6;_tmpA6.tag=13;_tmpA6.f1=_tmpE3->name;_tmpA6.f2=0;_tmpA6;});_tmpA5;});default: _LL6C: _tmpE4=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5E)->f1)->r)->f1;_LL6D: {
# 343
struct Cyc_List_List*_tmpA7=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpE4->tvs);
return(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8[0]=({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpA9;_tmpA9.tag=3;_tmpA9.f1=({struct Cyc_Absyn_DatatypeInfo _tmpAA;_tmpAA.datatype_info=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmpAB;_tmpAB.name=_tmpE4->name;_tmpAB.is_extensible=0;_tmpAB;}));_tmpAA.targs=_tmpA7;_tmpAA;});_tmpA9;});_tmpA8;});}}}_LL2D:;}
# 358 "tcutil.cyc"
static void*Cyc_Tcutil_copy_designator(int pt,void*d){
void*_tmpE5=d;struct _dyneither_ptr*_tmpE9;struct Cyc_Absyn_Exp*_tmpE8;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpE5)->tag == 0){_LL6F: _tmpE8=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpE5)->f1;_LL70:
 return(void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpE6=_cycalloc(sizeof(*_tmpE6));_tmpE6[0]=({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpE7;_tmpE7.tag=0;_tmpE7.f1=Cyc_Tcutil_deep_copy_exp(pt,_tmpE8);_tmpE7;});_tmpE6;});}else{_LL71: _tmpE9=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpE5)->f1;_LL72:
 return d;}_LL6E:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 364
static struct _tuple19*Cyc_Tcutil_copy_eds(int pt,struct _tuple19*e){
# 366
return({struct _tuple19*_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA->f1=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,pt,(e[0]).f1);_tmpEA->f2=Cyc_Tcutil_deep_copy_exp(pt,(e[0]).f2);_tmpEA;});}
# 369
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*new_e;
int _tmpEB=preserve_types;
{void*_tmpEC=e->r;void*_tmpED=_tmpEC;int _tmp17B;struct _dyneither_ptr _tmp17A;void*_tmp179;struct Cyc_Absyn_Exp*_tmp178;struct _dyneither_ptr*_tmp177;struct Cyc_Core_Opt*_tmp176;struct Cyc_List_List*_tmp175;struct Cyc_Absyn_Exp*_tmp174;struct Cyc_Absyn_Exp*_tmp173;int _tmp172;struct Cyc_Absyn_Exp*_tmp171;void**_tmp170;struct Cyc_Absyn_Exp*_tmp16F;int _tmp16E;int _tmp16D;struct _tuple2*_tmp16C;void*_tmp16B;struct Cyc_Absyn_Enumfield*_tmp16A;struct _tuple2*_tmp169;struct Cyc_Absyn_Enumdecl*_tmp168;struct Cyc_Absyn_Enumfield*_tmp167;struct Cyc_List_List*_tmp166;struct Cyc_Absyn_Datatypedecl*_tmp165;struct Cyc_Absyn_Datatypefield*_tmp164;void*_tmp163;struct Cyc_List_List*_tmp162;struct _tuple2*_tmp161;struct Cyc_List_List*_tmp160;struct Cyc_List_List*_tmp15F;struct Cyc_Absyn_Aggrdecl*_tmp15E;struct Cyc_Absyn_Exp*_tmp15D;void*_tmp15C;int _tmp15B;struct Cyc_Absyn_Vardecl*_tmp15A;struct Cyc_Absyn_Exp*_tmp159;struct Cyc_Absyn_Exp*_tmp158;int _tmp157;struct Cyc_List_List*_tmp156;struct _dyneither_ptr*_tmp155;struct Cyc_Absyn_Tqual _tmp154;void*_tmp153;struct Cyc_List_List*_tmp152;struct Cyc_List_List*_tmp151;struct Cyc_Absyn_Exp*_tmp150;struct Cyc_Absyn_Exp*_tmp14F;struct Cyc_Absyn_Exp*_tmp14E;struct _dyneither_ptr*_tmp14D;int _tmp14C;int _tmp14B;struct Cyc_Absyn_Exp*_tmp14A;struct _dyneither_ptr*_tmp149;int _tmp148;int _tmp147;struct Cyc_Absyn_Exp*_tmp146;void*_tmp145;struct Cyc_List_List*_tmp144;struct Cyc_Absyn_Exp*_tmp143;void*_tmp142;struct Cyc_Absyn_Exp*_tmp141;struct Cyc_Absyn_Exp*_tmp140;struct Cyc_Absyn_Exp*_tmp13F;void*_tmp13E;struct Cyc_Absyn_Exp*_tmp13D;int _tmp13C;enum Cyc_Absyn_Coercion _tmp13B;struct Cyc_Absyn_Exp*_tmp13A;struct Cyc_List_List*_tmp139;struct Cyc_Absyn_Exp*_tmp138;struct Cyc_Absyn_Exp*_tmp137;int _tmp136;struct Cyc_Absyn_Exp*_tmp135;struct Cyc_List_List*_tmp134;struct Cyc_Absyn_VarargCallInfo*_tmp133;int _tmp132;struct Cyc_Absyn_Exp*_tmp131;struct Cyc_Absyn_Exp*_tmp130;struct Cyc_Absyn_Exp*_tmp12F;struct Cyc_Absyn_Exp*_tmp12E;struct Cyc_Absyn_Exp*_tmp12D;struct Cyc_Absyn_Exp*_tmp12C;struct Cyc_Absyn_Exp*_tmp12B;struct Cyc_Absyn_Exp*_tmp12A;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_Absyn_Exp*_tmp128;enum Cyc_Absyn_Incrementor _tmp127;struct Cyc_Absyn_Exp*_tmp126;struct Cyc_Core_Opt*_tmp125;struct Cyc_Absyn_Exp*_tmp124;enum Cyc_Absyn_Primop _tmp123;struct Cyc_List_List*_tmp122;void*_tmp121;union Cyc_Absyn_Cnst _tmp120;switch(*((int*)_tmpED)){case 0: _LL74: _tmp120=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LL75:
 new_e=Cyc_Absyn_const_exp(_tmp120,e->loc);goto _LL73;case 1: _LL76: _tmp121=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LL77:
 new_e=Cyc_Absyn_varb_exp(_tmp121,e->loc);goto _LL73;case 2: _LL78: _tmp123=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp122=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL79:
 new_e=Cyc_Absyn_primop_exp(_tmp123,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpEB,_tmp122),e->loc);goto _LL73;case 3: _LL7A: _tmp126=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp125=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp124=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_LL7B:
# 377
 new_e=Cyc_Absyn_assignop_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp126),(unsigned int)_tmp125?({struct Cyc_Core_Opt*_tmpEE=_cycalloc_atomic(sizeof(*_tmpEE));_tmpEE->v=(void*)_tmp125->v;_tmpEE;}): 0,Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp124),e->loc);
goto _LL73;case 4: _LL7C: _tmp128=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp127=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL7D:
 new_e=Cyc_Absyn_increment_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp128),_tmp127,e->loc);goto _LL73;case 5: _LL7E: _tmp12B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp12A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp129=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_LL7F:
# 381
 new_e=Cyc_Absyn_conditional_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp12B),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp12A),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp129),e->loc);goto _LL73;case 6: _LL80: _tmp12D=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp12C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL81:
 new_e=Cyc_Absyn_and_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp12D),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp12C),e->loc);goto _LL73;case 7: _LL82: _tmp12F=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp12E=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL83:
 new_e=Cyc_Absyn_or_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp12F),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp12E),e->loc);goto _LL73;case 8: _LL84: _tmp131=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp130=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL85:
 new_e=Cyc_Absyn_seq_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp131),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp130),e->loc);goto _LL73;case 9: _LL86: _tmp135=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp134=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp133=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_tmp132=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpED)->f4;_LL87:
# 386
{struct Cyc_Absyn_VarargCallInfo*_tmpEF=_tmp133;int _tmpFD;struct Cyc_List_List*_tmpFC;struct Cyc_Absyn_VarargInfo*_tmpFB;if(_tmpEF != 0){_LLC5: _tmpFD=_tmpEF->num_varargs;_tmpFC=_tmpEF->injectors;_tmpFB=_tmpEF->vai;_LLC6: {
# 388
struct Cyc_Absyn_VarargInfo*_tmpF0=_tmpFB;struct _dyneither_ptr*_tmpF8;struct Cyc_Absyn_Tqual _tmpF7;void*_tmpF6;int _tmpF5;_LLCA: _tmpF8=_tmpF0->name;_tmpF7=_tmpF0->tq;_tmpF6=_tmpF0->type;_tmpF5=_tmpF0->inject;_LLCB:;
new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpF1=_cycalloc(sizeof(*_tmpF1));_tmpF1[0]=({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpF2;_tmpF2.tag=9;_tmpF2.f1=
Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp135);_tmpF2.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpEB,_tmp134);_tmpF2.f3=({struct Cyc_Absyn_VarargCallInfo*_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3->num_varargs=_tmpFD;_tmpF3->injectors=_tmpFC;_tmpF3->vai=({struct Cyc_Absyn_VarargInfo*_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4->name=_tmpF8;_tmpF4->tq=_tmpF7;_tmpF4->type=
# 392
Cyc_Tcutil_copy_type(_tmpF6);_tmpF4->inject=_tmpF5;_tmpF4;});_tmpF3;});_tmpF2.f4=_tmp132;_tmpF2;});_tmpF1;}),e->loc);
# 394
goto _LLC4;}}else{_LLC7: _LLC8:
# 396
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9[0]=({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpFA;_tmpFA.tag=9;_tmpFA.f1=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp135);_tmpFA.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpEB,_tmp134);_tmpFA.f3=_tmp133;_tmpFA.f4=_tmp132;_tmpFA;});_tmpF9;}),e->loc);
goto _LLC4;}_LLC4:;}
# 399
goto _LL73;case 10: _LL88: _tmp137=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp136=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL89:
# 401
 new_e=_tmp136?Cyc_Absyn_rethrow_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp137),e->loc): Cyc_Absyn_throw_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp137),e->loc);
goto _LL73;case 11: _LL8A: _tmp138=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LL8B:
 new_e=Cyc_Absyn_noinstantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp138),e->loc);
goto _LL73;case 12: _LL8C: _tmp13A=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp139=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL8D:
# 406
 new_e=Cyc_Absyn_instantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp13A),((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp139),e->loc);
goto _LL73;case 13: _LL8E: _tmp13E=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp13D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp13C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_tmp13B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpED)->f4;_LL8F:
# 409
 new_e=Cyc_Absyn_cast_exp(Cyc_Tcutil_copy_type(_tmp13E),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp13D),_tmp13C,_tmp13B,e->loc);goto _LL73;case 14: _LL90: _tmp13F=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LL91:
 new_e=Cyc_Absyn_address_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp13F),e->loc);goto _LL73;case 15: _LL92: _tmp141=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp140=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL93: {
# 412
struct Cyc_Absyn_Exp*eo1=_tmp141;if(_tmp141 != 0)eo1=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp141);
new_e=Cyc_Absyn_New_exp(eo1,Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp140),e->loc);
goto _LL73;}case 16: _LL94: _tmp142=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LL95:
 new_e=Cyc_Absyn_sizeoftyp_exp(Cyc_Tcutil_copy_type(_tmp142),e->loc);
goto _LL73;case 17: _LL96: _tmp143=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LL97:
 new_e=Cyc_Absyn_sizeofexp_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp143),e->loc);goto _LL73;case 18: _LL98: _tmp145=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp144=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL99:
# 419
 new_e=Cyc_Absyn_offsetof_exp(Cyc_Tcutil_copy_type(_tmp145),_tmp144,e->loc);goto _LL73;case 19: _LL9A: _tmp146=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LL9B:
 new_e=Cyc_Absyn_deref_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp146),e->loc);goto _LL73;case 20: _LL9C: _tmp14A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp149=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp148=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_tmp147=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpED)->f4;_LL9D:
# 422
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpFE=_cycalloc(sizeof(*_tmpFE));_tmpFE[0]=({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpFF;_tmpFF.tag=20;_tmpFF.f1=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp14A);_tmpFF.f2=_tmp149;_tmpFF.f3=_tmp148;_tmpFF.f4=_tmp147;_tmpFF;});_tmpFE;}),e->loc);goto _LL73;case 21: _LL9E: _tmp14E=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp14D=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp14C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_tmp14B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpED)->f4;_LL9F:
# 424
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp100=_cycalloc(sizeof(*_tmp100));_tmp100[0]=({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmp101;_tmp101.tag=21;_tmp101.f1=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp14E);_tmp101.f2=_tmp14D;_tmp101.f3=_tmp14C;_tmp101.f4=_tmp14B;_tmp101;});_tmp100;}),e->loc);goto _LL73;case 22: _LLA0: _tmp150=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp14F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LLA1:
 new_e=Cyc_Absyn_subscript_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp150),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp14F),e->loc);
goto _LL73;case 23: _LLA2: _tmp151=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LLA3:
 new_e=Cyc_Absyn_tuple_exp(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpEB,_tmp151),e->loc);goto _LL73;case 24: _LLA4: _tmp155=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpED)->f1)->f1;_tmp154=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpED)->f1)->f2;_tmp153=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpED)->f1)->f3;_tmp152=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LLA5: {
# 429
struct _dyneither_ptr*vopt1=_tmp155;
if(_tmp155 != 0)vopt1=_tmp155;
new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp102=_cycalloc(sizeof(*_tmp102));_tmp102[0]=({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmp103;_tmp103.tag=24;_tmp103.f1=({struct _tuple10*_tmp104=_cycalloc(sizeof(*_tmp104));_tmp104->f1=vopt1;_tmp104->f2=_tmp154;_tmp104->f3=Cyc_Tcutil_copy_type(_tmp153);_tmp104;});_tmp103.f2=
((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpEB,_tmp152);_tmp103;});_tmp102;}),e->loc);
goto _LL73;}case 25: _LLA6: _tmp156=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LLA7:
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp105=_cycalloc(sizeof(*_tmp105));_tmp105[0]=({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp106;_tmp106.tag=25;_tmp106.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpEB,_tmp156);_tmp106;});_tmp105;}),e->loc);
goto _LL73;case 26: _LLA8: _tmp15A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp159=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp158=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_tmp157=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpED)->f4;_LLA9:
# 437
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp107=_cycalloc(sizeof(*_tmp107));_tmp107[0]=({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmp108;_tmp108.tag=26;_tmp108.f1=_tmp15A;_tmp108.f2=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp159);_tmp108.f3=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp158);_tmp108.f4=_tmp157;_tmp108;});_tmp107;}),e->loc);
goto _LL73;case 27: _LLAA: _tmp15D=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp15C=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp15B=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_LLAB:
# 440
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp109=_cycalloc(sizeof(*_tmp109));_tmp109[0]=({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmp10A;_tmp10A.tag=27;_tmp10A.f1=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp15D);_tmp10A.f2=Cyc_Tcutil_copy_type(_tmp15C);_tmp10A.f3=_tmp15B;_tmp10A;});_tmp109;}),_tmp15D->loc);
# 442
goto _LL73;case 28: _LLAC: _tmp161=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp160=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp15F=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_tmp15E=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpED)->f4;_LLAD:
# 444
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp10B=_cycalloc(sizeof(*_tmp10B));_tmp10B[0]=({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp10C;_tmp10C.tag=28;_tmp10C.f1=_tmp161;_tmp10C.f2=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp160);_tmp10C.f3=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpEB,_tmp15F);_tmp10C.f4=_tmp15E;_tmp10C;});_tmp10B;}),e->loc);
# 446
goto _LL73;case 29: _LLAE: _tmp163=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp162=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LLAF:
# 448
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp10D=_cycalloc(sizeof(*_tmp10D));_tmp10D[0]=({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmp10E;_tmp10E.tag=29;_tmp10E.f1=Cyc_Tcutil_copy_type(_tmp163);_tmp10E.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpEB,_tmp162);_tmp10E;});_tmp10D;}),e->loc);
goto _LL73;case 30: _LLB0: _tmp166=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp165=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp164=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_LLB1:
# 451
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F[0]=({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp110;_tmp110.tag=30;_tmp110.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpEB,_tmp166);_tmp110.f2=_tmp165;_tmp110.f3=_tmp164;_tmp110;});_tmp10F;}),e->loc);
goto _LL73;case 31: _LLB2: _tmp169=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp168=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp167=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_LLB3:
 new_e=e;goto _LL73;case 32: _LLB4: _tmp16C=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp16B=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp16A=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_LLB5:
# 455
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111[0]=({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp112;_tmp112.tag=32;_tmp112.f1=_tmp16C;_tmp112.f2=Cyc_Tcutil_copy_type(_tmp16B);_tmp112.f3=_tmp16A;_tmp112;});_tmp111;}),e->loc);
goto _LL73;case 33: _LLB6: _tmp172=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpED)->f1).is_calloc;_tmp171=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpED)->f1).rgn;_tmp170=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpED)->f1).elt_type;_tmp16F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpED)->f1).num_elts;_tmp16E=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpED)->f1).fat_result;_tmp16D=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpED)->f1).inline_call;_LLB7: {
# 458
struct Cyc_Absyn_Exp*_tmp113=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp171;if(_tmp171 != 0)r1=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp171);{
void**t1=_tmp170;if(_tmp170 != 0)t1=({void**_tmp114=_cycalloc(sizeof(*_tmp114));_tmp114[0]=Cyc_Tcutil_copy_type(*_tmp170);_tmp114;});
_tmp113->r=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115[0]=({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmp116;_tmp116.tag=33;_tmp116.f1=({struct Cyc_Absyn_MallocInfo _tmp117;_tmp117.is_calloc=_tmp172;_tmp117.rgn=r1;_tmp117.elt_type=t1;_tmp117.num_elts=_tmp16F;_tmp117.fat_result=_tmp16E;_tmp117.inline_call=_tmp16D;_tmp117;});_tmp116;});_tmp115;});
new_e=_tmp113;
goto _LL73;};}case 34: _LLB8: _tmp174=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp173=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LLB9:
 new_e=Cyc_Absyn_swap_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp174),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp173),e->loc);goto _LL73;case 35: _LLBA: _tmp176=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp175=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LLBB: {
# 466
struct Cyc_Core_Opt*nopt1=_tmp176;
if(_tmp176 != 0)nopt1=({struct Cyc_Core_Opt*_tmp118=_cycalloc(sizeof(*_tmp118));_tmp118->v=(struct _tuple2*)_tmp176->v;_tmp118;});
new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119[0]=({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp11A;_tmp11A.tag=35;_tmp11A.f1=nopt1;_tmp11A.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpEB,_tmp175);_tmp11A;});_tmp119;}),e->loc);
goto _LL73;}case 36: _LLBC: _LLBD:
# 471
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp11B=_cycalloc(sizeof(*_tmp11B));_tmp11B[0]=({struct Cyc_Core_Failure_exn_struct _tmp11C;_tmp11C.tag=Cyc_Core_Failure;_tmp11C.f1=({const char*_tmp11D="deep_copy: statement expressions unsupported";_tag_dyneither(_tmp11D,sizeof(char),45);});_tmp11C;});_tmp11B;}));case 37: _LLBE: _tmp178=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp177=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LLBF:
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp11E=_cycalloc(sizeof(*_tmp11E));_tmp11E[0]=({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmp11F;_tmp11F.tag=37;_tmp11F.f1=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp178);_tmp11F.f2=_tmp177;_tmp11F;});_tmp11E;}),e->loc);
goto _LL73;case 38: _LLC0: _tmp179=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LLC1:
 new_e=Cyc_Absyn_valueof_exp(Cyc_Tcutil_copy_type(_tmp179),e->loc);
goto _LL73;default: _LLC2: _tmp17B=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp17A=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LLC3:
 new_e=Cyc_Absyn_asm_exp(_tmp17B,_tmp17A,e->loc);goto _LL73;}_LL73:;}
# 479
if(preserve_types)new_e->topt=e->topt;
return new_e;}
# 483
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*e){
if(e == 0)return 0;else{
return Cyc_Tcutil_deep_copy_exp(preserve_types,e);}}struct _tuple20{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple21{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 496 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
struct Cyc_Absyn_Kind*_tmp17C=ka1;enum Cyc_Absyn_KindQual _tmp187;enum Cyc_Absyn_AliasQual _tmp186;_LLCD: _tmp187=_tmp17C->kind;_tmp186=_tmp17C->aliasqual;_LLCE:;{
struct Cyc_Absyn_Kind*_tmp17D=ka2;enum Cyc_Absyn_KindQual _tmp185;enum Cyc_Absyn_AliasQual _tmp184;_LLD0: _tmp185=_tmp17D->kind;_tmp184=_tmp17D->aliasqual;_LLD1:;
# 500
if(_tmp187 != _tmp185){
struct _tuple20 _tmp17E=({struct _tuple20 _tmp180;_tmp180.f1=_tmp187;_tmp180.f2=_tmp185;_tmp180;});struct _tuple20 _tmp17F=_tmp17E;switch(_tmp17F.f1){case Cyc_Absyn_BoxKind: switch(_tmp17F.f2){case Cyc_Absyn_MemKind: _LLD3: _LLD4:
 goto _LLD6;case Cyc_Absyn_AnyKind: _LLD5: _LLD6:
 goto _LLD8;default: goto _LLD9;}case Cyc_Absyn_MemKind: if(_tmp17F.f2 == Cyc_Absyn_AnyKind){_LLD7: _LLD8:
 goto _LLD2;}else{goto _LLD9;}default: _LLD9: _LLDA:
 return 0;}_LLD2:;}
# 509
if(_tmp186 != _tmp184){
struct _tuple21 _tmp181=({struct _tuple21 _tmp183;_tmp183.f1=_tmp186;_tmp183.f2=_tmp184;_tmp183;});struct _tuple21 _tmp182=_tmp181;switch(_tmp182.f1){case Cyc_Absyn_Aliasable: if(_tmp182.f2 == Cyc_Absyn_Top){_LLDC: _LLDD:
 goto _LLDF;}else{goto _LLE0;}case Cyc_Absyn_Unique: if(_tmp182.f2 == Cyc_Absyn_Top){_LLDE: _LLDF:
 return 1;}else{goto _LLE0;}default: _LLE0: _LLE1:
 return 0;}_LLDB:;}
# 516
return 1;};}
# 519
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp188=Cyc_Absyn_compress_kb(tv->kind);void*_tmp189=_tmp188;struct Cyc_Absyn_Kind*_tmp18D;struct Cyc_Absyn_Kind*_tmp18C;switch(*((int*)_tmp189)){case 0: _LLE3: _tmp18C=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp189)->f1;_LLE4:
 return _tmp18C;case 2: _LLE5: _tmp18D=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp189)->f2;_LLE6:
 return _tmp18D;default: _LLE7: _LLE8:
# 524
 tv->kind=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp18A=_cycalloc(sizeof(*_tmp18A));_tmp18A[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp18B;_tmp18B.tag=2;_tmp18B.f1=0;_tmp18B.f2=def;_tmp18B;});_tmp18A;});
return def;}_LLE2:;}
# 529
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
struct _tuple0 _tmp18E=({struct _tuple0 _tmp1A5;_tmp1A5.f1=Cyc_Absyn_compress_kb(kb1);_tmp1A5.f2=Cyc_Absyn_compress_kb(kb2);_tmp1A5;});struct _tuple0 _tmp18F=_tmp18E;struct Cyc_Core_Opt**_tmp1A4;void*_tmp1A3;void*_tmp1A2;struct Cyc_Core_Opt**_tmp1A1;struct Cyc_Core_Opt**_tmp1A0;struct Cyc_Absyn_Kind*_tmp19F;struct Cyc_Core_Opt**_tmp19E;struct Cyc_Absyn_Kind*_tmp19D;struct Cyc_Core_Opt**_tmp19C;struct Cyc_Absyn_Kind*_tmp19B;struct Cyc_Absyn_Kind*_tmp19A;struct Cyc_Absyn_Kind*_tmp199;struct Cyc_Core_Opt**_tmp198;struct Cyc_Absyn_Kind*_tmp197;struct Cyc_Absyn_Kind*_tmp196;struct Cyc_Absyn_Kind*_tmp195;switch(*((int*)_tmp18F.f1)){case 0: switch(*((int*)_tmp18F.f2)){case 0: _LLEA: _tmp196=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp18F.f1)->f1;_tmp195=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp18F.f2)->f1;_LLEB:
 return _tmp196 == _tmp195;case 2: _LLEE: _tmp199=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp18F.f1)->f1;_tmp198=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp18F.f2)->f1;_tmp197=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp18F.f2)->f2;_LLEF:
# 538
 if(Cyc_Tcutil_kind_leq(_tmp199,_tmp197)){
*_tmp198=({struct Cyc_Core_Opt*_tmp191=_cycalloc(sizeof(*_tmp191));_tmp191->v=kb1;_tmp191;});
return 1;}else{
return 0;}default: goto _LLF4;}case 2: switch(*((int*)_tmp18F.f2)){case 0: _LLEC: _tmp19C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp18F.f1)->f1;_tmp19B=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp18F.f1)->f2;_tmp19A=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp18F.f2)->f1;_LLED:
# 533
 if(Cyc_Tcutil_kind_leq(_tmp19A,_tmp19B)){
*_tmp19C=({struct Cyc_Core_Opt*_tmp190=_cycalloc(sizeof(*_tmp190));_tmp190->v=kb2;_tmp190;});
return 1;}else{
return 0;}case 2: _LLF0: _tmp1A0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp18F.f1)->f1;_tmp19F=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp18F.f1)->f2;_tmp19E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp18F.f2)->f1;_tmp19D=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp18F.f2)->f2;_LLF1:
# 543
 if(Cyc_Tcutil_kind_leq(_tmp19F,_tmp19D)){
*_tmp19E=({struct Cyc_Core_Opt*_tmp192=_cycalloc(sizeof(*_tmp192));_tmp192->v=kb1;_tmp192;});
return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp19D,_tmp19F)){
*_tmp1A0=({struct Cyc_Core_Opt*_tmp193=_cycalloc(sizeof(*_tmp193));_tmp193->v=kb2;_tmp193;});
return 1;}else{
return 0;}}default: _LLF4: _tmp1A2=_tmp18F.f1;_tmp1A1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp18F.f2)->f1;_LLF5:
# 552
*_tmp1A1=({struct Cyc_Core_Opt*_tmp194=_cycalloc(sizeof(*_tmp194));_tmp194->v=_tmp1A2;_tmp194;});
return 1;}default: _LLF2: _tmp1A4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp18F.f1)->f1;_tmp1A3=_tmp18F.f2;_LLF3:
# 550
 _tmp1A2=_tmp1A3;_tmp1A1=_tmp1A4;goto _LLF5;}_LLE9:;}
# 557
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp1A6=Cyc_Tcutil_compress(t);void*_tmp1A7=_tmp1A6;struct Cyc_Absyn_Tvar*_tmp1AD;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A7)->tag == 2){_LLF7: _tmp1AD=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A7)->f1;_LLF8: {
# 560
void*_tmp1A8=_tmp1AD->kind;
_tmp1AD->kind=kb;
return({struct _tuple16 _tmp1A9;_tmp1A9.f1=_tmp1AD;_tmp1A9.f2=_tmp1A8;_tmp1A9;});}}else{_LLF9: _LLFA:
# 564
({struct Cyc_String_pa_PrintArg_struct _tmp1AC;_tmp1AC.tag=0;_tmp1AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp1AA[1]={& _tmp1AC};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1AB="swap_kind: cannot update the kind of %s";_tag_dyneither(_tmp1AB,sizeof(char),40);}),_tag_dyneither(_tmp1AA,sizeof(void*),1));});});}_LLF6:;}
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
void*_tmp1B6=Cyc_Tcutil_compress(t);void*_tmp1B7=_tmp1B6;struct Cyc_Absyn_Typedefdecl*_tmp1D5;struct Cyc_Absyn_Exp*_tmp1D4;struct Cyc_Absyn_Kind*_tmp1D3;struct Cyc_Absyn_PtrInfo _tmp1D2;enum Cyc_Absyn_AggrKind _tmp1D1;struct Cyc_List_List*_tmp1D0;struct Cyc_Absyn_AggrdeclImpl*_tmp1CF;int _tmp1CE;struct Cyc_List_List*_tmp1CD;struct Cyc_Absyn_Datatypedecl*_tmp1CC;struct Cyc_Absyn_Datatypefield*_tmp1CB;enum Cyc_Absyn_Size_of _tmp1CA;struct Cyc_Absyn_Tvar*_tmp1C9;struct Cyc_Core_Opt*_tmp1C8;switch(*((int*)_tmp1B7)){case 1: _LL103: _tmp1C8=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B7)->f1;_LL104:
 return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp1C8))->v;case 2: _LL105: _tmp1C9=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1B7)->f1;_LL106:
 return Cyc_Tcutil_tvar_kind(_tmp1C9,& Cyc_Tcutil_bk);case 0: _LL107: _LL108:
 return& Cyc_Tcutil_mk;case 6: _LL109: _tmp1CA=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1B7)->f2;_LL10A:
# 607
 return(_tmp1CA == Cyc_Absyn_Int_sz  || _tmp1CA == Cyc_Absyn_Long_sz)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;case 7: _LL10B: _LL10C:
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
 return& Cyc_Tcutil_ak;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1B7)->f1).field_info).KnownDatatypefield).tag == 2){_LL11F: _tmp1CC=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1B7)->f1).field_info).KnownDatatypefield).val).f1;_tmp1CB=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1B7)->f1).field_info).KnownDatatypefield).val).f2;_LL120:
# 622
 return& Cyc_Tcutil_mk;}else{_LL121: _LL122:
# 624
({void*_tmp1B8=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1B9="typ_kind: Unresolved DatatypeFieldType";_tag_dyneither(_tmp1B9,sizeof(char),39);}),_tag_dyneither(_tmp1B8,sizeof(void*),0));});}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B7)->f1).aggr_info).UnknownAggr).tag == 1){_LL123: _LL124:
# 627
 return& Cyc_Tcutil_ak;}else{_LL125: _tmp1D1=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B7)->f1).aggr_info).KnownAggr).val)->kind;_tmp1D0=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B7)->f1).aggr_info).KnownAggr).val)->tvs;_tmp1CF=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B7)->f1).aggr_info).KnownAggr).val)->impl;_tmp1CE=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B7)->f1).aggr_info).KnownAggr).val)->expected_mem_kind;_tmp1CD=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B7)->f1).targs;_LL126:
# 629
 if(_tmp1CF == 0){
if(_tmp1CE)
return& Cyc_Tcutil_mk;else{
# 633
return& Cyc_Tcutil_ak;}}{
# 635
struct Cyc_List_List*_tmp1BA=_tmp1CF->fields;
if(_tmp1BA == 0)return& Cyc_Tcutil_mk;
# 638
if(_tmp1D1 == Cyc_Absyn_StructA){
for(0;_tmp1BA->tl != 0;_tmp1BA=_tmp1BA->tl){;}{
void*_tmp1BB=((struct Cyc_Absyn_Aggrfield*)_tmp1BA->hd)->type;
struct Cyc_Absyn_Kind*_tmp1BC=Cyc_Tcutil_field_kind(_tmp1BB,_tmp1CD,_tmp1D0);
if(_tmp1BC == & Cyc_Tcutil_ak  || _tmp1BC == & Cyc_Tcutil_tak)return _tmp1BC;};}else{
# 646
for(0;_tmp1BA != 0;_tmp1BA=_tmp1BA->tl){
void*_tmp1BD=((struct Cyc_Absyn_Aggrfield*)_tmp1BA->hd)->type;
struct Cyc_Absyn_Kind*_tmp1BE=Cyc_Tcutil_field_kind(_tmp1BD,_tmp1CD,_tmp1D0);
if(_tmp1BE == & Cyc_Tcutil_ak  || _tmp1BE == & Cyc_Tcutil_tak)return _tmp1BE;}}
# 652
return& Cyc_Tcutil_mk;};}case 12: _LL127: _LL128:
 return& Cyc_Tcutil_mk;case 5: _LL129: _tmp1D2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B7)->f1;_LL12A: {
# 655
void*_tmp1BF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,(_tmp1D2.ptr_atts).bounds);void*_tmp1C0=_tmp1BF;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1C0)->tag == 0){_LL146: _LL147: {
# 657
enum Cyc_Absyn_AliasQual _tmp1C1=(Cyc_Tcutil_typ_kind((_tmp1D2.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp1C2=_tmp1C1;switch(_tmp1C2){case Cyc_Absyn_Aliasable: _LL14B: _LL14C:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL14D: _LL14E:
 return& Cyc_Tcutil_umk;default: _LL14F: _LL150:
 return& Cyc_Tcutil_tmk;}_LL14A:;}}else{_LL148: _LL149: {
# 663
enum Cyc_Absyn_AliasQual _tmp1C3=(Cyc_Tcutil_typ_kind((_tmp1D2.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp1C4=_tmp1C3;switch(_tmp1C4){case Cyc_Absyn_Aliasable: _LL152: _LL153:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL154: _LL155:
 return& Cyc_Tcutil_ubk;default: _LL156: _LL157:
 return& Cyc_Tcutil_tbk;}_LL151:;}}_LL145:;}case 18: _LL12B: _LL12C:
# 669
 return& Cyc_Tcutil_ik;case 28: _LL12D: _tmp1D3=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp1B7)->f2;_LL12E:
 return _tmp1D3;case 27: _LL12F: _LL130:
# 674
 return& Cyc_Tcutil_ak;case 19: _LL131: _LL132:
 return& Cyc_Tcutil_bk;case 8: _LL133: _tmp1D4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1B7)->f1).num_elts;_LL134:
# 677
 if(_tmp1D4 == 0  || Cyc_Tcutil_is_const_exp(_tmp1D4))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;case 10: _LL135: _LL136:
 return& Cyc_Tcutil_mk;case 17: _LL137: _tmp1D5=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1B7)->f3;_LL138:
# 681
 if(_tmp1D5 == 0  || _tmp1D5->kind == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1C7;_tmp1C7.tag=0;_tmp1C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp1C5[1]={& _tmp1C7};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1C6="typ_kind: typedef found: %s";_tag_dyneither(_tmp1C6,sizeof(char),28);}),_tag_dyneither(_tmp1C5,sizeof(void*),1));});});
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp1D5->kind))->v;case 26: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1B7)->f1)->r)){case 0: _LL139: _LL13A:
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
void*_tmp1DC=t;struct Cyc_List_List*_tmp202;void*_tmp201;void*_tmp200;void*_tmp1FF;void*_tmp1FE;struct Cyc_List_List*_tmp1FD;struct Cyc_List_List*_tmp1FC;struct Cyc_List_List*_tmp1FB;struct Cyc_List_List*_tmp1FA;struct Cyc_List_List*_tmp1F9;struct Cyc_List_List*_tmp1F8;struct Cyc_List_List*_tmp1F7;void*_tmp1F6;struct Cyc_Absyn_Tqual _tmp1F5;void*_tmp1F4;struct Cyc_List_List*_tmp1F3;int _tmp1F2;struct Cyc_Absyn_VarargInfo*_tmp1F1;struct Cyc_List_List*_tmp1F0;struct Cyc_List_List*_tmp1EF;struct Cyc_Absyn_Exp*_tmp1EE;struct Cyc_List_List*_tmp1ED;struct Cyc_Absyn_Exp*_tmp1EC;struct Cyc_List_List*_tmp1EB;void*_tmp1EA;struct Cyc_Absyn_PtrInfo _tmp1E9;void*_tmp1E8;struct Cyc_Core_Opt**_tmp1E7;struct Cyc_Absyn_Tvar*_tmp1E6;switch(*((int*)_tmp1DC)){case 2: _LL15E: _tmp1E6=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1DC)->f1;_LL15F:
# 712
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp1E6)){
Cyc_Tcutil_failure_reason=({const char*_tmp1DD="(type variable would escape scope)";_tag_dyneither(_tmp1DD,sizeof(char),35);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 716
goto _LL15D;case 1: _LL160: _tmp1E8=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1DC)->f2;_tmp1E7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1DC)->f4;_LL161:
# 718
 if(t == evar){
Cyc_Tcutil_failure_reason=({const char*_tmp1DE="(occurs check)";_tag_dyneither(_tmp1DE,sizeof(char),15);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{
# 722
if(_tmp1E8 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp1E8);else{
# 725
int problem=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp1E7))->v;for(0;s != 0;s=s->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}
# 732
if(problem){
struct Cyc_List_List*_tmp1DF=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp1E7))->v;for(0;s != 0;s=s->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd))
_tmp1DF=({struct Cyc_List_List*_tmp1E0=_cycalloc(sizeof(*_tmp1E0));_tmp1E0->hd=(struct Cyc_Absyn_Tvar*)s->hd;_tmp1E0->tl=_tmp1DF;_tmp1E0;});}}
# 738
*_tmp1E7=({struct Cyc_Core_Opt*_tmp1E1=_cycalloc(sizeof(*_tmp1E1));_tmp1E1->v=_tmp1DF;_tmp1E1;});}}}
# 741
goto _LL15D;case 5: _LL162: _tmp1E9=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1DC)->f1;_LL163:
# 744
 Cyc_Tcutil_occurs(evar,r,env,_tmp1E9.elt_typ);
Cyc_Tcutil_occurs(evar,r,env,(_tmp1E9.ptr_atts).rgn);
goto _LL15D;case 8: _LL164: _tmp1EA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1DC)->f1).elt_type;_LL165:
# 748
 Cyc_Tcutil_occurs(evar,r,env,_tmp1EA);goto _LL15D;case 9: _LL166: _tmp1F7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DC)->f1).tvars;_tmp1F6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DC)->f1).effect;_tmp1F5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DC)->f1).ret_tqual;_tmp1F4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DC)->f1).ret_typ;_tmp1F3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DC)->f1).args;_tmp1F2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DC)->f1).c_varargs;_tmp1F1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DC)->f1).cyc_varargs;_tmp1F0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DC)->f1).rgn_po;_tmp1EF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DC)->f1).attributes;_tmp1EE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DC)->f1).requires_clause;_tmp1ED=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DC)->f1).requires_relns;_tmp1EC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DC)->f1).ensures_clause;_tmp1EB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DC)->f1).ensures_relns;_LL167:
# 751
 env=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp1F7,env);
if(_tmp1F6 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp1F6);
Cyc_Tcutil_occurs(evar,r,env,_tmp1F4);
for(0;_tmp1F3 != 0;_tmp1F3=_tmp1F3->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple10*)_tmp1F3->hd)).f3);}
if(_tmp1F1 != 0)
Cyc_Tcutil_occurs(evar,r,env,_tmp1F1->type);
for(0;_tmp1F0 != 0;_tmp1F0=_tmp1F0->tl){
struct _tuple0*_tmp1E2=(struct _tuple0*)_tmp1F0->hd;struct _tuple0*_tmp1E3=_tmp1E2;void*_tmp1E5;void*_tmp1E4;_LL181: _tmp1E5=_tmp1E3->f1;_tmp1E4=_tmp1E3->f2;_LL182:;
Cyc_Tcutil_occurs(evar,r,env,_tmp1E5);
Cyc_Tcutil_occurs(evar,r,env,_tmp1E4);}
# 763
goto _LL15D;case 10: _LL168: _tmp1F8=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1DC)->f1;_LL169:
# 765
 for(0;_tmp1F8 != 0;_tmp1F8=_tmp1F8->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple12*)_tmp1F8->hd)).f2);}
goto _LL15D;case 3: _LL16A: _tmp1F9=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1DC)->f1).targs;_LL16B:
# 769
 Cyc_Tcutil_occurslist(evar,r,env,_tmp1F9);goto _LL15D;case 17: _LL16C: _tmp1FA=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1DC)->f2;_LL16D:
 _tmp1FB=_tmp1FA;goto _LL16F;case 4: _LL16E: _tmp1FB=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1DC)->f1).targs;_LL16F:
 _tmp1FC=_tmp1FB;goto _LL171;case 11: _LL170: _tmp1FC=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1DC)->f1).targs;_LL171:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp1FC);goto _LL15D;case 12: _LL172: _tmp1FD=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1DC)->f2;_LL173:
# 775
 for(0;_tmp1FD != 0;_tmp1FD=_tmp1FD->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp1FD->hd)->type);}
goto _LL15D;case 19: _LL174: _tmp1FE=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp1DC)->f1;_LL175:
 _tmp1FF=_tmp1FE;goto _LL177;case 23: _LL176: _tmp1FF=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp1DC)->f1;_LL177:
 _tmp200=_tmp1FF;goto _LL179;case 15: _LL178: _tmp200=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1DC)->f1;_LL179:
 _tmp201=_tmp200;goto _LL17B;case 25: _LL17A: _tmp201=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp1DC)->f1;_LL17B:
 Cyc_Tcutil_occurs(evar,r,env,_tmp201);goto _LL15D;case 24: _LL17C: _tmp202=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp1DC)->f1;_LL17D:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp202);goto _LL15D;default: _LL17E: _LL17F:
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
union Cyc_Absyn_Constraint*_tmp208=x;void*_tmp210;switch((((union Cyc_Absyn_Constraint*)_tmp208)->Eq_constr).tag){case 3: _LL184: _LL185:
# 838
*x=({union Cyc_Absyn_Constraint _tmp209;(_tmp209.Forward_constr).val=y;(_tmp209.Forward_constr).tag=2;_tmp209;});return;case 1: _LL186: _tmp210=(void*)(_tmp208->Eq_constr).val;_LL187: {
# 840
union Cyc_Absyn_Constraint*_tmp20A=y;void*_tmp20D;switch((((union Cyc_Absyn_Constraint*)_tmp20A)->Eq_constr).tag){case 3: _LL18B: _LL18C:
*y=*x;return;case 1: _LL18D: _tmp20D=(void*)(_tmp20A->Eq_constr).val;_LL18E:
# 843
 if(cmp(_tmp210,_tmp20D)!= 0){
Cyc_Tcutil_failure_reason=reason;
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 847
return;default: _LL18F: _LL190:
({void*_tmp20B=0;Cyc_Tcutil_impos(({const char*_tmp20C="unify_conref: forward after compress(2)";_tag_dyneither(_tmp20C,sizeof(char),40);}),_tag_dyneither(_tmp20B,sizeof(void*),0));});}_LL18A:;}default: _LL188: _LL189:
# 850
({void*_tmp20E=0;Cyc_Tcutil_impos(({const char*_tmp20F="unify_conref: forward after compress";_tag_dyneither(_tmp20F,sizeof(char),37);}),_tag_dyneither(_tmp20E,sizeof(void*),0));});}_LL183:;};}
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
struct _tuple0 _tmp217=({struct _tuple0 _tmp21B;_tmp21B.f1=b1;_tmp21B.f2=b2;_tmp21B;});struct _tuple0 _tmp218=_tmp217;struct Cyc_Absyn_Exp*_tmp21A;struct Cyc_Absyn_Exp*_tmp219;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp218.f1)->tag == 0){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp218.f2)->tag == 0){_LL197: _LL198:
 return 0;}else{_LL199: _LL19A:
 return - 1;}}else{if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp218.f2)->tag == 0){_LL19B: _LL19C:
 return 1;}else{_LL19D: _tmp21A=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp218.f1)->f1;_tmp219=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp218.f2)->f1;_LL19E:
# 867
 return Cyc_Evexp_const_exp_cmp(_tmp21A,_tmp219);}}_LL196:;}
# 871
static void Cyc_Tcutil_unify_cmp_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
{struct _tuple0 _tmp21C=({struct _tuple0 _tmp22A;_tmp22A.f1=e1->r;_tmp22A.f2=e2->r;_tmp22A;});struct _tuple0 _tmp21D=_tmp21C;void*_tmp229;void*_tmp228;struct Cyc_Absyn_Exp*_tmp227;struct Cyc_Absyn_Exp*_tmp226;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp21D.f1)->tag == 13){_LL1A0: _tmp226=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp21D.f1)->f2;_LL1A1:
# 877
 Cyc_Tcutil_unify_cmp_exp(_tmp226,e2);
return;}else{if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp21D.f2)->tag == 13){_LL1A2: _tmp227=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp21D.f2)->f2;_LL1A3:
# 880
 Cyc_Tcutil_unify_cmp_exp(e1,_tmp227);
return;}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp21D.f1)->tag == 38){_LL1A4: _tmp228=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp21D.f1)->f1;_LL1A5:
# 883
{void*_tmp21E=Cyc_Tcutil_compress(_tmp228);void*_tmp21F=_tmp21E;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp21F)->tag == 1){_LL1AB: _LL1AC:
# 885
 Cyc_Tcutil_unify_it(_tmp228,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp220=_cycalloc(sizeof(*_tmp220));_tmp220[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp221;_tmp221.tag=18;_tmp221.f1=e2;_tmp221;});_tmp220;}));
return;}else{_LL1AD: _LL1AE:
# 888
 goto _LL1AA;}_LL1AA:;}
# 890
goto _LL19F;}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp21D.f2)->tag == 38){_LL1A6: _tmp229=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp21D.f2)->f1;_LL1A7:
# 892
{void*_tmp222=Cyc_Tcutil_compress(_tmp229);void*_tmp223=_tmp222;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp223)->tag == 1){_LL1B0: _LL1B1:
# 894
 Cyc_Tcutil_unify_it(_tmp229,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp224=_cycalloc(sizeof(*_tmp224));_tmp224[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp225;_tmp225.tag=18;_tmp225.f1=e1;_tmp225;});_tmp224;}));
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
struct _tuple0 _tmp22B=({struct _tuple0 _tmp235;_tmp235.f1=b1;_tmp235.f2=b2;_tmp235;});struct _tuple0 _tmp22C=_tmp22B;struct Cyc_Absyn_Exp*_tmp234;struct Cyc_Absyn_Exp*_tmp233;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp22C.f1)->tag == 0){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp22C.f2)->tag == 0){_LL1B5: _LL1B6:
 return 0;}else{_LL1B7: _LL1B8:
 return - 1;}}else{if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp22C.f2)->tag == 0){_LL1B9: _LL1BA:
 return 1;}else{_LL1BB: _tmp234=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp22C.f1)->f1;_tmp233=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp22C.f2)->f1;_LL1BC: {
# 911
int r=Cyc_Evexp_const_exp_cmp(_tmp234,_tmp233);
if(r == 0)return 0;{
struct _handler_cons _tmp22D;_push_handler(& _tmp22D);{int _tmp22F=0;if(setjmp(_tmp22D.handler))_tmp22F=1;if(!_tmp22F){
Cyc_Tcutil_unify_cmp_exp(_tmp234,_tmp233);{
int _tmp230=0;_npop_handler(0);return _tmp230;};
# 914
;_pop_handler();}else{void*_tmp22E=(void*)_exn_thrown;void*_tmp231=_tmp22E;void*_tmp232;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp231)->tag == Cyc_Tcutil_Unify){_LL1BE: _LL1BF:
# 917
 return r;}else{_LL1C0: _tmp232=_tmp231;_LL1C1:(int)_rethrow(_tmp232);}_LL1BD:;}};};}}}_LL1B4:;}
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
struct _tuple0 _tmp237=({struct _tuple0 _tmp249;_tmp249.f1=att1;_tmp249.f2=att2;_tmp249;});struct _tuple0 _tmp238=_tmp237;enum Cyc_Absyn_Format_Type _tmp248;int _tmp247;int _tmp246;enum Cyc_Absyn_Format_Type _tmp245;int _tmp244;int _tmp243;struct _dyneither_ptr _tmp242;struct _dyneither_ptr _tmp241;struct Cyc_Absyn_Exp*_tmp240;struct Cyc_Absyn_Exp*_tmp23F;int _tmp23E;int _tmp23D;int _tmp23C;int _tmp23B;switch(*((int*)_tmp238.f1)){case 0: if(((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp238.f2)->tag == 0){_LL1F0: _tmp23C=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp238.f1)->f1;_tmp23B=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp238.f2)->f1;_LL1F1:
 _tmp23E=_tmp23C;_tmp23D=_tmp23B;goto _LL1F3;}else{goto _LL1FA;}case 20: if(((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp238.f2)->tag == 20){_LL1F2: _tmp23E=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp238.f1)->f1;_tmp23D=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp238.f2)->f1;_LL1F3:
# 953
 return Cyc_Core_intcmp(_tmp23E,_tmp23D);}else{goto _LL1FA;}case 6: if(((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp238.f2)->tag == 6){_LL1F4: _tmp240=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp238.f1)->f1;_tmp23F=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp238.f2)->f1;_LL1F5:
# 955
 if(_tmp240 == _tmp23F)return 0;
if(_tmp240 == 0)return - 1;
if(_tmp23F == 0)return 1;
return Cyc_Evexp_const_exp_cmp(_tmp240,_tmp23F);}else{goto _LL1FA;}case 8: if(((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp238.f2)->tag == 8){_LL1F6: _tmp242=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp238.f1)->f1;_tmp241=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp238.f2)->f1;_LL1F7:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp242,(struct _dyneither_ptr)_tmp241);}else{goto _LL1FA;}case 19: if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp238.f2)->tag == 19){_LL1F8: _tmp248=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp238.f1)->f1;_tmp247=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp238.f1)->f2;_tmp246=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp238.f1)->f3;_tmp245=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp238.f2)->f1;_tmp244=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp238.f2)->f2;_tmp243=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp238.f2)->f3;_LL1F9: {
# 961
int _tmp239=Cyc_Core_intcmp((int)((unsigned int)_tmp248),(int)((unsigned int)_tmp245));
if(_tmp239 != 0)return _tmp239;{
int _tmp23A=Cyc_Core_intcmp(_tmp247,_tmp244);
if(_tmp23A != 0)return _tmp23A;
return Cyc_Core_intcmp(_tmp246,_tmp243);};}}else{goto _LL1FA;}default: _LL1FA: _LL1FB:
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
void*_tmp24F=Cyc_Tcutil_compress(t);void*_tmp250=_tmp24F;struct Cyc_List_List*_tmp280;void*_tmp27F;struct Cyc_List_List*_tmp27E;void*_tmp27D;struct Cyc_Absyn_Tqual _tmp27C;void*_tmp27B;struct Cyc_List_List*_tmp27A;struct Cyc_Absyn_VarargInfo*_tmp279;struct Cyc_List_List*_tmp278;struct Cyc_List_List*_tmp277;struct Cyc_List_List*_tmp276;struct Cyc_List_List*_tmp275;struct Cyc_List_List*_tmp274;void*_tmp273;void*_tmp272;void*_tmp271;struct Cyc_List_List*_tmp270;void*_tmp26F;void*_tmp26E;void*_tmp26D;switch(*((int*)_tmp250)){case 0: _LL208: _LL209:
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
struct Cyc_Absyn_Kind*_tmp251=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp252=_tmp251;switch(((struct Cyc_Absyn_Kind*)_tmp252)->kind){case Cyc_Absyn_RgnKind: _LL243: _LL244:
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp253=_cycalloc(sizeof(*_tmp253));_tmp253[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp254;_tmp254.tag=23;_tmp254.f1=t;_tmp254;});_tmp253;});case Cyc_Absyn_EffKind: _LL245: _LL246:
 return t;case Cyc_Absyn_IntKind: _LL247: _LL248:
 return Cyc_Absyn_empty_effect;default: _LL249: _LL24A:
 return(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp255=_cycalloc(sizeof(*_tmp255));_tmp255[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp256;_tmp256.tag=25;_tmp256.f1=t;_tmp256;});_tmp255;});}_LL242:;}case 15: _LL21C: _tmp26D=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp250)->f1;_LL21D:
# 1024
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp257=_cycalloc(sizeof(*_tmp257));_tmp257[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp258;_tmp258.tag=23;_tmp258.f1=_tmp26D;_tmp258;});_tmp257;});case 16: _LL21E: _tmp26F=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp250)->f1;_tmp26E=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp250)->f2;_LL21F:
# 1028
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp259=_cycalloc(sizeof(*_tmp259));_tmp259[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp25A;_tmp25A.tag=23;_tmp25A.f1=_tmp26E;_tmp25A;});_tmp259;});case 3: _LL220: _tmp270=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp250)->f1).targs;_LL221: {
# 1030
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp270);
return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp25B=_cycalloc(sizeof(*_tmp25B));_tmp25B[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp25C;_tmp25C.tag=24;_tmp25C.f1=ts;_tmp25C;});_tmp25B;}));}case 5: _LL222: _tmp272=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp250)->f1).elt_typ;_tmp271=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp250)->f1).ptr_atts).rgn;_LL223:
# 1034
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp25D=_cycalloc(sizeof(*_tmp25D));_tmp25D[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp25E;_tmp25E.tag=24;_tmp25E.f1=({void*_tmp25F[2];_tmp25F[1]=Cyc_Tcutil_rgns_of(_tmp272);_tmp25F[0]=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp260=_cycalloc(sizeof(*_tmp260));_tmp260[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp261;_tmp261.tag=23;_tmp261.f1=_tmp271;_tmp261;});_tmp260;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp25F,sizeof(void*),2));});_tmp25E;});_tmp25D;}));case 8: _LL224: _tmp273=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp250)->f1).elt_type;_LL225:
# 1036
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp273));case 10: _LL226: _tmp274=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp250)->f1;_LL227: {
# 1038
struct Cyc_List_List*_tmp262=0;
for(0;_tmp274 != 0;_tmp274=_tmp274->tl){
_tmp262=({struct Cyc_List_List*_tmp263=_cycalloc(sizeof(*_tmp263));_tmp263->hd=(*((struct _tuple12*)_tmp274->hd)).f2;_tmp263->tl=_tmp262;_tmp263;});}
_tmp275=_tmp262;goto _LL229;}case 4: _LL228: _tmp275=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp250)->f1).targs;_LL229:
 _tmp276=_tmp275;goto _LL22B;case 11: _LL22A: _tmp276=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp250)->f1).targs;_LL22B:
# 1044
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp264=_cycalloc(sizeof(*_tmp264));_tmp264[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp265;_tmp265.tag=24;_tmp265.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp276);_tmp265;});_tmp264;}));case 12: _LL22C: _tmp277=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp250)->f2;_LL22D:
# 1046
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp266=_cycalloc(sizeof(*_tmp266));_tmp266[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp267;_tmp267.tag=24;_tmp267.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp277);_tmp267;});_tmp266;}));case 19: _LL22E: _LL22F:
 return Cyc_Absyn_empty_effect;case 9: _LL230: _tmp27E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp250)->f1).tvars;_tmp27D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp250)->f1).effect;_tmp27C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp250)->f1).ret_tqual;_tmp27B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp250)->f1).ret_typ;_tmp27A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp250)->f1).args;_tmp279=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp250)->f1).cyc_varargs;_tmp278=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp250)->f1).rgn_po;_LL231: {
# 1056
void*_tmp268=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp27E),(void*)_check_null(_tmp27D));
return Cyc_Tcutil_normalize_effect(_tmp268);}case 21: _LL232: _LL233:
 goto _LL235;case 22: _LL234: _LL235:
 goto _LL237;case 20: _LL236: _LL237:
 return Cyc_Absyn_empty_effect;case 23: _LL238: _LL239:
 goto _LL23B;case 24: _LL23A: _LL23B:
 return t;case 25: _LL23C: _tmp27F=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp250)->f1;_LL23D:
 return Cyc_Tcutil_rgns_of(_tmp27F);case 17: _LL23E: _tmp280=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp250)->f2;_LL23F:
# 1065
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp269=_cycalloc(sizeof(*_tmp269));_tmp269[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp26A;_tmp26A.tag=24;_tmp26A.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp280);_tmp26A;});_tmp269;}));default: _LL240: _LL241:
({void*_tmp26B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp26C="typedecl in rgns_of";_tag_dyneither(_tmp26C,sizeof(char),20);}),_tag_dyneither(_tmp26B,sizeof(void*),0));});}_LL207:;}
# 1073
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp281=e;void*_tmp28D;struct Cyc_List_List**_tmp28C;switch(*((int*)_tmp281)){case 24: _LL24C: _tmp28C=(struct Cyc_List_List**)&((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp281)->f1;_LL24D: {
# 1077
int redo_join=0;
{struct Cyc_List_List*effs=*_tmp28C;for(0;effs != 0;effs=effs->tl){
void*_tmp282=(void*)effs->hd;
effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp282));{
void*_tmp283=(void*)effs->hd;void*_tmp284=_tmp283;switch(*((int*)_tmp284)){case 24: _LL253: _LL254:
 goto _LL256;case 23: switch(*((int*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp284)->f1)){case 20: _LL255: _LL256:
 redo_join=1;goto _LL252;case 22: _LL257: _LL258:
 redo_join=1;goto _LL252;case 21: _LL259: _LL25A:
 redo_join=1;goto _LL252;default: goto _LL25B;}default: _LL25B: _LL25C:
 goto _LL252;}_LL252:;};}}
# 1089
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*_tmp28C;for(0;effs != 0;effs=effs->tl){
void*_tmp285=Cyc_Tcutil_compress((void*)effs->hd);void*_tmp286=_tmp285;void*_tmp289;struct Cyc_List_List*_tmp288;switch(*((int*)_tmp286)){case 24: _LL25E: _tmp288=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp286)->f1;_LL25F:
# 1094
 effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp288,effects);
goto _LL25D;case 23: switch(*((int*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp286)->f1)){case 20: _LL260: _LL261:
 goto _LL25D;case 22: _LL262: _LL263:
 goto _LL25D;case 21: _LL264: _LL265:
 goto _LL25D;default: goto _LL266;}default: _LL266: _tmp289=_tmp286;_LL267:
 effects=({struct Cyc_List_List*_tmp287=_cycalloc(sizeof(*_tmp287));_tmp287->hd=_tmp289;_tmp287->tl=effects;_tmp287;});goto _LL25D;}_LL25D:;}}
# 1102
*_tmp28C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
return e;};}case 25: _LL24E: _tmp28D=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp281)->f1;_LL24F: {
# 1105
void*_tmp28A=Cyc_Tcutil_compress(_tmp28D);void*_tmp28B=_tmp28A;switch(*((int*)_tmp28B)){case 1: _LL269: _LL26A:
 goto _LL26C;case 2: _LL26B: _LL26C:
 return e;default: _LL26D: _LL26E:
 return Cyc_Tcutil_rgns_of(_tmp28D);}_LL268:;}default: _LL250: _LL251:
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
void*_tmp292=Cyc_Tcutil_compress(e);void*_tmp293=_tmp292;struct Cyc_Core_Opt*_tmp2B8;void**_tmp2B7;struct Cyc_Core_Opt*_tmp2B6;void*_tmp2B5;struct Cyc_List_List*_tmp2B4;void*_tmp2B3;switch(*((int*)_tmp293)){case 23: _LL270: _tmp2B3=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp293)->f1;_LL271:
# 1139
 if(constrain)return Cyc_Tcutil_unify(r,_tmp2B3);
_tmp2B3=Cyc_Tcutil_compress(_tmp2B3);
if(r == _tmp2B3)return 1;{
struct _tuple0 _tmp294=({struct _tuple0 _tmp298;_tmp298.f1=r;_tmp298.f2=_tmp2B3;_tmp298;});struct _tuple0 _tmp295=_tmp294;struct Cyc_Absyn_Tvar*_tmp297;struct Cyc_Absyn_Tvar*_tmp296;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp295.f1)->tag == 2){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp295.f2)->tag == 2){_LL27B: _tmp297=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp295.f1)->f1;_tmp296=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp295.f2)->f1;_LL27C:
 return Cyc_Absyn_tvar_cmp(_tmp297,_tmp296)== 0;}else{goto _LL27D;}}else{_LL27D: _LL27E:
 return 0;}_LL27A:;};case 24: _LL272: _tmp2B4=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp293)->f1;_LL273:
# 1147
 for(0;_tmp2B4 != 0;_tmp2B4=_tmp2B4->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp2B4->hd))return 1;}
return 0;case 25: _LL274: _tmp2B5=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp293)->f1;_LL275: {
# 1151
void*_tmp299=Cyc_Tcutil_rgns_of(_tmp2B5);void*_tmp29A=_tmp299;void*_tmp2A8;void*_tmp2A7;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp29A)->tag == 25){_LL280: _tmp2A7=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp29A)->f1;_LL281:
# 1153
 if(!constrain)return 0;{
void*_tmp29B=Cyc_Tcutil_compress(_tmp2A7);void*_tmp29C=_tmp29B;struct Cyc_Core_Opt*_tmp2A6;void**_tmp2A5;struct Cyc_Core_Opt*_tmp2A4;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp29C)->tag == 1){_LL285: _tmp2A6=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp29C)->f1;_tmp2A5=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp29C)->f2;_tmp2A4=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp29C)->f4;_LL286: {
# 1158
void*_tmp29D=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2A4);
# 1161
Cyc_Tcutil_occurs(_tmp29D,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2A4))->v,r);{
void*_tmp29E=Cyc_Tcutil_dummy_fntype((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp29F=_cycalloc(sizeof(*_tmp29F));_tmp29F[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2A0;_tmp2A0.tag=24;_tmp2A0.f1=({void*_tmp2A1[2];_tmp2A1[1]=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp2A2=_cycalloc(sizeof(*_tmp2A2));_tmp2A2[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp2A3;_tmp2A3.tag=23;_tmp2A3.f1=r;_tmp2A3;});_tmp2A2;});_tmp2A1[0]=_tmp29D;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2A1,sizeof(void*),2));});_tmp2A0;});_tmp29F;}));
*_tmp2A5=_tmp29E;
return 1;};}}else{_LL287: _LL288:
 return 0;}_LL284:;};}else{_LL282: _tmp2A8=_tmp29A;_LL283:
# 1167
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp2A8);}_LL27F:;}case 1: _LL276: _tmp2B8=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp293)->f1;_tmp2B7=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp293)->f2;_tmp2B6=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp293)->f4;_LL277:
# 1170
 if(_tmp2B8 == 0  || ((struct Cyc_Absyn_Kind*)_tmp2B8->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp2A9=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2AA="effect evar has wrong kind";_tag_dyneither(_tmp2AA,sizeof(char),27);}),_tag_dyneither(_tmp2A9,sizeof(void*),0));});
if(!constrain)return 0;{
# 1175
void*_tmp2AB=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2B6);
# 1178
Cyc_Tcutil_occurs(_tmp2AB,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2B6))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2AC=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2AD=_cycalloc(sizeof(*_tmp2AD));_tmp2AD[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2AE;_tmp2AE.tag=24;_tmp2AE.f1=({struct Cyc_List_List*_tmp2AF=_cycalloc(sizeof(*_tmp2AF));_tmp2AF->hd=_tmp2AB;_tmp2AF->tl=({struct Cyc_List_List*_tmp2B0=_cycalloc(sizeof(*_tmp2B0));_tmp2B0->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp2B1=_cycalloc(sizeof(*_tmp2B1));_tmp2B1[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp2B2;_tmp2B2.tag=23;_tmp2B2.f1=r;_tmp2B2;});_tmp2B1;});_tmp2B0->tl=0;_tmp2B0;});_tmp2AF;});_tmp2AE;});_tmp2AD;});
*_tmp2B7=(void*)_tmp2AC;
return 1;};};default: _LL278: _LL279:
 return 0;}_LL26F:;};}
# 1189
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Tcutil_compress(t);{
void*_tmp2B9=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_tmp2BA=_tmp2B9;struct Cyc_Core_Opt*_tmp2D2;void**_tmp2D1;struct Cyc_Core_Opt*_tmp2D0;void*_tmp2CF;struct Cyc_List_List*_tmp2CE;switch(*((int*)_tmp2BA)){case 23: _LL28A: _LL28B:
 return 0;case 24: _LL28C: _tmp2CE=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2BA)->f1;_LL28D:
# 1194
 for(0;_tmp2CE != 0;_tmp2CE=_tmp2CE->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp2CE->hd))
return 1;}
return 0;case 25: _LL28E: _tmp2CF=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2BA)->f1;_LL28F:
# 1199
 _tmp2CF=Cyc_Tcutil_compress(_tmp2CF);
if(t == _tmp2CF)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp2CF);{
void*_tmp2BB=Cyc_Tcutil_rgns_of(t);void*_tmp2BC=_tmp2BB;void*_tmp2C3;void*_tmp2C2;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2BC)->tag == 25){_LL295: _tmp2C2=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2BC)->f1;_LL296: {
# 1204
struct _tuple0 _tmp2BD=({struct _tuple0 _tmp2C1;_tmp2C1.f1=Cyc_Tcutil_compress(_tmp2C2);_tmp2C1.f2=_tmp2CF;_tmp2C1;});struct _tuple0 _tmp2BE=_tmp2BD;struct Cyc_Absyn_Tvar*_tmp2C0;struct Cyc_Absyn_Tvar*_tmp2BF;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2BE.f1)->tag == 2){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2BE.f2)->tag == 2){_LL29A: _tmp2C0=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2BE.f1)->f1;_tmp2BF=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2BE.f2)->f1;_LL29B:
 return Cyc_Tcutil_unify(t,_tmp2CF);}else{goto _LL29C;}}else{_LL29C: _LL29D:
 return _tmp2C2 == _tmp2CF;}_LL299:;}}else{_LL297: _tmp2C3=_tmp2BC;_LL298:
# 1208
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp2C3);}_LL294:;};case 1: _LL290: _tmp2D2=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2BA)->f1;_tmp2D1=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2BA)->f2;_tmp2D0=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2BA)->f4;_LL291:
# 1211
 if(_tmp2D2 == 0  || ((struct Cyc_Absyn_Kind*)_tmp2D2->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp2C4=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2C5="effect evar has wrong kind";_tag_dyneither(_tmp2C5,sizeof(char),27);}),_tag_dyneither(_tmp2C4,sizeof(void*),0));});
if(!may_constrain_evars)return 0;{
# 1216
void*_tmp2C6=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2D0);
# 1219
Cyc_Tcutil_occurs(_tmp2C6,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2D0))->v,t);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2C7=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2C8=_cycalloc(sizeof(*_tmp2C8));_tmp2C8[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2C9;_tmp2C9.tag=24;_tmp2C9.f1=({struct Cyc_List_List*_tmp2CA=_cycalloc(sizeof(*_tmp2CA));_tmp2CA->hd=_tmp2C6;_tmp2CA->tl=({struct Cyc_List_List*_tmp2CB=_cycalloc(sizeof(*_tmp2CB));_tmp2CB->hd=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp2CC=_cycalloc(sizeof(*_tmp2CC));_tmp2CC[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp2CD;_tmp2CD.tag=25;_tmp2CD.f1=t;_tmp2CD;});_tmp2CC;});_tmp2CB->tl=0;_tmp2CB;});_tmp2CA;});_tmp2C9;});_tmp2C8;});
*_tmp2D1=(void*)_tmp2C7;
return 1;};};default: _LL292: _LL293:
 return 0;}_LL289:;};}
# 1230
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp2D3=e;struct Cyc_Core_Opt*_tmp2F3;void**_tmp2F2;struct Cyc_Core_Opt*_tmp2F1;void*_tmp2F0;struct Cyc_List_List*_tmp2EF;struct Cyc_Absyn_Tvar*_tmp2EE;switch(*((int*)_tmp2D3)){case 2: _LL29F: _tmp2EE=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2D3)->f1;_LL2A0:
 return Cyc_Absyn_tvar_cmp(v,_tmp2EE)== 0;case 24: _LL2A1: _tmp2EF=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2D3)->f1;_LL2A2:
# 1235
 for(0;_tmp2EF != 0;_tmp2EF=_tmp2EF->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)_tmp2EF->hd))
return 1;}
return 0;case 25: _LL2A3: _tmp2F0=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2D3)->f1;_LL2A4: {
# 1240
void*_tmp2D4=Cyc_Tcutil_rgns_of(_tmp2F0);void*_tmp2D5=_tmp2D4;void*_tmp2E3;void*_tmp2E2;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2D5)->tag == 25){_LL2AA: _tmp2E2=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2D5)->f1;_LL2AB:
# 1242
 if(!may_constrain_evars)return 0;{
void*_tmp2D6=Cyc_Tcutil_compress(_tmp2E2);void*_tmp2D7=_tmp2D6;struct Cyc_Core_Opt*_tmp2E1;void**_tmp2E0;struct Cyc_Core_Opt*_tmp2DF;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2D7)->tag == 1){_LL2AF: _tmp2E1=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2D7)->f1;_tmp2E0=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2D7)->f2;_tmp2DF=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2D7)->f4;_LL2B0: {
# 1248
void*_tmp2D8=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2DF);
# 1250
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2DF))->v,v))return 0;{
void*_tmp2D9=Cyc_Tcutil_dummy_fntype((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2DA=_cycalloc(sizeof(*_tmp2DA));_tmp2DA[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2DB;_tmp2DB.tag=24;_tmp2DB.f1=({void*_tmp2DC[2];_tmp2DC[1]=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2DD=_cycalloc(sizeof(*_tmp2DD));_tmp2DD[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp2DE;_tmp2DE.tag=2;_tmp2DE.f1=v;_tmp2DE;});_tmp2DD;});_tmp2DC[0]=_tmp2D8;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2DC,sizeof(void*),2));});_tmp2DB;});_tmp2DA;}));
*_tmp2E0=_tmp2D9;
return 1;};}}else{_LL2B1: _LL2B2:
 return 0;}_LL2AE:;};}else{_LL2AC: _tmp2E3=_tmp2D5;_LL2AD:
# 1256
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp2E3);}_LL2A9:;}case 1: _LL2A5: _tmp2F3=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2D3)->f1;_tmp2F2=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2D3)->f2;_tmp2F1=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2D3)->f4;_LL2A6:
# 1259
 if(_tmp2F3 == 0  || ((struct Cyc_Absyn_Kind*)_tmp2F3->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp2E4=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2E5="effect evar has wrong kind";_tag_dyneither(_tmp2E5,sizeof(char),27);}),_tag_dyneither(_tmp2E4,sizeof(void*),0));});{
# 1263
void*_tmp2E6=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2F1);
# 1265
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2F1))->v,v))
return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2E7=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2E8=_cycalloc(sizeof(*_tmp2E8));_tmp2E8[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2E9;_tmp2E9.tag=24;_tmp2E9.f1=({struct Cyc_List_List*_tmp2EA=_cycalloc(sizeof(*_tmp2EA));_tmp2EA->hd=_tmp2E6;_tmp2EA->tl=({struct Cyc_List_List*_tmp2EB=_cycalloc(sizeof(*_tmp2EB));_tmp2EB->hd=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2EC=_cycalloc(sizeof(*_tmp2EC));_tmp2EC[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp2ED;_tmp2ED.tag=2;_tmp2ED.f1=v;_tmp2ED;});_tmp2EC;});_tmp2EB->tl=0;_tmp2EB;});_tmp2EA;});_tmp2E9;});_tmp2E8;});
*_tmp2F2=(void*)_tmp2E7;
return 1;};};default: _LL2A7: _LL2A8:
 return 0;}_LL29E:;};}
# 1275
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp2F4=e;void*_tmp2FA;struct Cyc_List_List*_tmp2F9;switch(*((int*)_tmp2F4)){case 24: _LL2B4: _tmp2F9=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2F4)->f1;_LL2B5:
# 1279
 for(0;_tmp2F9 != 0;_tmp2F9=_tmp2F9->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp2F9->hd))
return 1;}
return 0;case 25: _LL2B6: _tmp2FA=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2F4)->f1;_LL2B7: {
# 1284
void*_tmp2F5=Cyc_Tcutil_rgns_of(_tmp2FA);void*_tmp2F6=_tmp2F5;void*_tmp2F8;void*_tmp2F7;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2F6)->tag == 25){_LL2BD: _tmp2F7=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2F6)->f1;_LL2BE:
 return 0;}else{_LL2BF: _tmp2F8=_tmp2F6;_LL2C0:
 return Cyc_Tcutil_evar_in_effect(evar,_tmp2F8);}_LL2BC:;}case 1: _LL2B8: _LL2B9:
# 1288
 return evar == e;default: _LL2BA: _LL2BB:
 return 0;}_LL2B3:;};}
# 1302 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1307
void*_tmp2FB=Cyc_Tcutil_compress(e1);void*_tmp2FC=_tmp2FB;void**_tmp309;struct Cyc_Core_Opt*_tmp308;void*_tmp307;struct Cyc_Absyn_Tvar*_tmp306;void*_tmp305;struct Cyc_List_List*_tmp304;switch(*((int*)_tmp2FC)){case 24: _LL2C2: _tmp304=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2FC)->f1;_LL2C3:
# 1309
 for(0;_tmp304 != 0;_tmp304=_tmp304->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp304->hd,e2))
return 0;}
return 1;case 23: _LL2C4: _tmp305=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp2FC)->f1;_LL2C5:
# 1320
 return Cyc_Tcutil_region_in_effect(may_constrain_evars,_tmp305,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp305,(void*)& Cyc_Absyn_HeapRgn_val);case 2: _LL2C6: _tmp306=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2FC)->f1;_LL2C7:
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp306,e2);case 25: _LL2C8: _tmp307=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2FC)->f1;_LL2C9: {
# 1324
void*_tmp2FD=Cyc_Tcutil_rgns_of(_tmp307);void*_tmp2FE=_tmp2FD;void*_tmp300;void*_tmp2FF;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2FE)->tag == 25){_LL2CF: _tmp2FF=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2FE)->f1;_LL2D0:
# 1329
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp2FF,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp2FF,Cyc_Absyn_sint_typ);}else{_LL2D1: _tmp300=_tmp2FE;_LL2D2:
 return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp300,e2);}_LL2CE:;}case 1: _LL2CA: _tmp309=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2FC)->f2;_tmp308=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2FC)->f4;_LL2CB:
# 1334
 if(!Cyc_Tcutil_evar_in_effect(e1,e2)){
# 1338
*_tmp309=Cyc_Absyn_empty_effect;
# 1341
return 1;}else{
# 1343
return 0;}default: _LL2CC: _LL2CD:
({struct Cyc_String_pa_PrintArg_struct _tmp303;_tmp303.tag=0;_tmp303.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1));({void*_tmp301[1]={& _tmp303};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp302="subset_effect: bad effect: %s";_tag_dyneither(_tmp302,sizeof(char),30);}),_tag_dyneither(_tmp301,sizeof(void*),1));});});}_LL2C1:;}
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
struct _tuple0*_tmp30A=(struct _tuple0*)r1->hd;struct _tuple0*_tmp30B=_tmp30A;void*_tmp311;void*_tmp310;_LL2D4: _tmp311=_tmp30B->f1;_tmp310=_tmp30B->f2;_LL2D5:;{
int found=_tmp311 == (void*)& Cyc_Absyn_HeapRgn_val;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
struct _tuple0*_tmp30C=(struct _tuple0*)r2->hd;struct _tuple0*_tmp30D=_tmp30C;void*_tmp30F;void*_tmp30E;_LL2D7: _tmp30F=_tmp30D->f1;_tmp30E=_tmp30D->f2;_LL2D8:;
if(Cyc_Tcutil_unify(_tmp311,_tmp30F) && Cyc_Tcutil_unify(_tmp310,_tmp30E)){
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
{void*_tmp315=t1;struct Cyc_Core_Opt*_tmp324;void**_tmp323;struct Cyc_Core_Opt*_tmp322;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp315)->tag == 1){_LL2DA: _tmp324=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp315)->f1;_tmp323=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp315)->f2;_tmp322=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp315)->f4;_LL2DB:
# 1430
 Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp322))->v,t2);{
struct Cyc_Absyn_Kind*_tmp316=Cyc_Tcutil_typ_kind(t2);
# 1435
if(Cyc_Tcutil_kind_leq(_tmp316,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp324))->v)){
*_tmp323=t2;
return;}else{
# 1439
{void*_tmp317=t2;struct Cyc_Absyn_PtrInfo _tmp320;void**_tmp31F;struct Cyc_Core_Opt*_tmp31E;switch(*((int*)_tmp317)){case 1: _LL2DF: _tmp31F=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp317)->f2;_tmp31E=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp317)->f4;_LL2E0: {
# 1442
struct Cyc_List_List*_tmp318=(struct Cyc_List_List*)_tmp322->v;
{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp31E))->v;for(0;s2 != 0;s2=s2->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp318,(struct Cyc_Absyn_Tvar*)s2->hd)){
Cyc_Tcutil_failure_reason=({const char*_tmp319="(type variable would escape scope)";_tag_dyneither(_tmp319,sizeof(char),35);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1449
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp324->v,_tmp316)){
*_tmp31F=t1;return;}
# 1452
Cyc_Tcutil_failure_reason=({const char*_tmp31A="(kinds are incompatible)";_tag_dyneither(_tmp31A,sizeof(char),25);});
goto _LL2DE;}case 5: _LL2E1: _tmp320=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp317)->f1;if(((struct Cyc_Absyn_Kind*)_tmp324->v)->kind == Cyc_Absyn_BoxKind){_LL2E2: {
# 1458
union Cyc_Absyn_Constraint*_tmp31B=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((_tmp320.ptr_atts).bounds);
{union Cyc_Absyn_Constraint*_tmp31C=_tmp31B;if((((union Cyc_Absyn_Constraint*)_tmp31C)->No_constr).tag == 3){_LL2E6: _LL2E7:
# 1462
({struct _union_Constraint_Eq_constr*_tmp31D=& _tmp31B->Eq_constr;_tmp31D->tag=1;_tmp31D->val=Cyc_Absyn_bounds_one;});
*_tmp323=t2;
return;}else{_LL2E8: _LL2E9:
 goto _LL2E5;}_LL2E5:;}
# 1467
goto _LL2DE;}}else{goto _LL2E3;}default: _LL2E3: _LL2E4:
 goto _LL2DE;}_LL2DE:;}
# 1470
Cyc_Tcutil_failure_reason=({const char*_tmp321="(kinds are incompatible)";_tag_dyneither(_tmp321,sizeof(char),25);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};}else{_LL2DC: _LL2DD:
# 1473
 goto _LL2D9;}_LL2D9:;}
# 1478
{struct _tuple0 _tmp325=({struct _tuple0 _tmp3EA;_tmp3EA.f1=t2;_tmp3EA.f2=t1;_tmp3EA;});struct _tuple0 _tmp326=_tmp325;void*_tmp3E9;void*_tmp3E8;void*_tmp3E7;void*_tmp3E6;void*_tmp3E5;void*_tmp3E4;struct Cyc_List_List*_tmp3E3;struct Cyc_Absyn_Typedefdecl*_tmp3E2;struct Cyc_List_List*_tmp3E1;struct Cyc_Absyn_Typedefdecl*_tmp3E0;enum Cyc_Absyn_AggrKind _tmp3DF;struct Cyc_List_List*_tmp3DE;enum Cyc_Absyn_AggrKind _tmp3DD;struct Cyc_List_List*_tmp3DC;struct Cyc_List_List*_tmp3DB;struct Cyc_List_List*_tmp3DA;struct Cyc_List_List*_tmp3D9;void*_tmp3D8;struct Cyc_Absyn_Tqual _tmp3D7;void*_tmp3D6;struct Cyc_List_List*_tmp3D5;int _tmp3D4;struct Cyc_Absyn_VarargInfo*_tmp3D3;struct Cyc_List_List*_tmp3D2;struct Cyc_List_List*_tmp3D1;struct Cyc_Absyn_Exp*_tmp3D0;struct Cyc_List_List*_tmp3CF;struct Cyc_Absyn_Exp*_tmp3CE;struct Cyc_List_List*_tmp3CD;struct Cyc_List_List*_tmp3CC;void*_tmp3CB;struct Cyc_Absyn_Tqual _tmp3CA;void*_tmp3C9;struct Cyc_List_List*_tmp3C8;int _tmp3C7;struct Cyc_Absyn_VarargInfo*_tmp3C6;struct Cyc_List_List*_tmp3C5;struct Cyc_List_List*_tmp3C4;struct Cyc_Absyn_Exp*_tmp3C3;struct Cyc_List_List*_tmp3C2;struct Cyc_Absyn_Exp*_tmp3C1;struct Cyc_List_List*_tmp3C0;void*_tmp3BF;struct Cyc_Absyn_Tqual _tmp3BE;struct Cyc_Absyn_Exp*_tmp3BD;union Cyc_Absyn_Constraint*_tmp3BC;void*_tmp3BB;struct Cyc_Absyn_Tqual _tmp3BA;struct Cyc_Absyn_Exp*_tmp3B9;union Cyc_Absyn_Constraint*_tmp3B8;struct Cyc_Absyn_Exp*_tmp3B7;struct Cyc_Absyn_Exp*_tmp3B6;void*_tmp3B5;void*_tmp3B4;int _tmp3B3;int _tmp3B2;enum Cyc_Absyn_Sign _tmp3B1;enum Cyc_Absyn_Size_of _tmp3B0;enum Cyc_Absyn_Sign _tmp3AF;enum Cyc_Absyn_Size_of _tmp3AE;void*_tmp3AD;struct Cyc_Absyn_Tqual _tmp3AC;void*_tmp3AB;union Cyc_Absyn_Constraint*_tmp3AA;union Cyc_Absyn_Constraint*_tmp3A9;union Cyc_Absyn_Constraint*_tmp3A8;void*_tmp3A7;struct Cyc_Absyn_Tqual _tmp3A6;void*_tmp3A5;union Cyc_Absyn_Constraint*_tmp3A4;union Cyc_Absyn_Constraint*_tmp3A3;union Cyc_Absyn_Constraint*_tmp3A2;struct Cyc_Absyn_Datatypedecl*_tmp3A1;struct Cyc_Absyn_Datatypefield*_tmp3A0;struct Cyc_List_List*_tmp39F;struct Cyc_Absyn_Datatypedecl*_tmp39E;struct Cyc_Absyn_Datatypefield*_tmp39D;struct Cyc_List_List*_tmp39C;struct Cyc_Absyn_Datatypedecl*_tmp39B;struct Cyc_List_List*_tmp39A;struct Cyc_Absyn_Datatypedecl*_tmp399;struct Cyc_List_List*_tmp398;struct Cyc_List_List*_tmp397;struct Cyc_List_List*_tmp396;struct _tuple2*_tmp395;struct _tuple2*_tmp394;union Cyc_Absyn_AggrInfoU _tmp393;struct Cyc_List_List*_tmp392;union Cyc_Absyn_AggrInfoU _tmp391;struct Cyc_List_List*_tmp390;struct Cyc_Absyn_Tvar*_tmp38F;struct Cyc_Absyn_Tvar*_tmp38E;switch(*((int*)_tmp326.f1)){case 1: _LL2EB: _LL2EC:
# 1481
 Cyc_Tcutil_unify_it(t2,t1);
return;case 0: switch(*((int*)_tmp326.f2)){case 0: _LL2ED: _LL2EE:
# 1484
 return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 2: switch(*((int*)_tmp326.f2)){case 2: _LL2EF: _tmp38F=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp326.f1)->f1;_tmp38E=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp326.f2)->f1;_LL2F0: {
# 1487
struct _dyneither_ptr*_tmp327=_tmp38F->name;
struct _dyneither_ptr*_tmp328=_tmp38E->name;
# 1490
int _tmp329=_tmp38F->identity;
int _tmp32A=_tmp38E->identity;
if(_tmp32A == _tmp329)return;
Cyc_Tcutil_failure_reason=({const char*_tmp32B="(variable types are not the same)";_tag_dyneither(_tmp32B,sizeof(char),34);});
goto _LL2EA;}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 11: switch(*((int*)_tmp326.f2)){case 11: _LL2F1: _tmp393=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp326.f1)->f1).aggr_info;_tmp392=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp326.f1)->f1).targs;_tmp391=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp326.f2)->f1).aggr_info;_tmp390=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp326.f2)->f1).targs;_LL2F2: {
# 1497
struct _tuple13 _tmp32C=Cyc_Absyn_aggr_kinded_name(_tmp391);struct _tuple13 _tmp32D=_tmp32C;enum Cyc_Absyn_AggrKind _tmp335;struct _tuple2*_tmp334;_LL328: _tmp335=_tmp32D.f1;_tmp334=_tmp32D.f2;_LL329:;{
struct _tuple13 _tmp32E=Cyc_Absyn_aggr_kinded_name(_tmp393);struct _tuple13 _tmp32F=_tmp32E;enum Cyc_Absyn_AggrKind _tmp333;struct _tuple2*_tmp332;_LL32B: _tmp333=_tmp32F.f1;_tmp332=_tmp32F.f2;_LL32C:;
if(_tmp335 != _tmp333){Cyc_Tcutil_failure_reason=({const char*_tmp330="(struct and union type)";_tag_dyneither(_tmp330,sizeof(char),24);});goto _LL2EA;}
if(Cyc_Absyn_qvar_cmp(_tmp334,_tmp332)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp331="(different type name)";_tag_dyneither(_tmp331,sizeof(char),22);});goto _LL2EA;}
Cyc_Tcutil_unify_list(_tmp390,_tmp392);
return;};}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 13: switch(*((int*)_tmp326.f2)){case 13: _LL2F3: _tmp395=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp326.f1)->f1;_tmp394=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp326.f2)->f1;_LL2F4:
# 1506
 if(Cyc_Absyn_qvar_cmp(_tmp395,_tmp394)== 0)return;
Cyc_Tcutil_failure_reason=({const char*_tmp336="(different enum types)";_tag_dyneither(_tmp336,sizeof(char),23);});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 14: switch(*((int*)_tmp326.f2)){case 14: _LL2F5: _tmp397=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp326.f1)->f1;_tmp396=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp326.f2)->f1;_LL2F6: {
# 1511
int bad=0;
for(0;_tmp397 != 0  && _tmp396 != 0;(_tmp397=_tmp397->tl,_tmp396=_tmp396->tl)){
struct Cyc_Absyn_Enumfield*_tmp337=(struct Cyc_Absyn_Enumfield*)_tmp397->hd;
struct Cyc_Absyn_Enumfield*_tmp338=(struct Cyc_Absyn_Enumfield*)_tmp396->hd;
if(Cyc_Absyn_qvar_cmp(_tmp337->name,_tmp338->name)!= 0){
Cyc_Tcutil_failure_reason=({const char*_tmp339="(different names for enum fields)";_tag_dyneither(_tmp339,sizeof(char),34);});
bad=1;
break;}
# 1520
if(_tmp337->tag == _tmp338->tag)continue;
if(_tmp337->tag == 0  || _tmp338->tag == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp33A="(different tag values for enum fields)";_tag_dyneither(_tmp33A,sizeof(char),39);});
bad=1;
break;}
# 1526
if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp337->tag),(struct Cyc_Absyn_Exp*)_check_null(_tmp338->tag))){
Cyc_Tcutil_failure_reason=({const char*_tmp33B="(different tag values for enum fields)";_tag_dyneither(_tmp33B,sizeof(char),39);});
bad=1;
break;}}
# 1532
if(bad)goto _LL2EA;
if(_tmp397 == 0  && _tmp396 == 0)return;
Cyc_Tcutil_failure_reason=({const char*_tmp33C="(different number of fields for enums)";_tag_dyneither(_tmp33C,sizeof(char),39);});
goto _LL2EA;}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp326.f1)->f1).datatype_info).KnownDatatype).tag == 2)switch(*((int*)_tmp326.f2)){case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp326.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL2F7: _tmp39B=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp326.f1)->f1).datatype_info).KnownDatatype).val;_tmp39A=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp326.f1)->f1).targs;_tmp399=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp326.f2)->f1).datatype_info).KnownDatatype).val;_tmp398=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp326.f2)->f1).targs;_LL2F8:
# 1539
 if(_tmp39B == _tmp399  || Cyc_Absyn_qvar_cmp(_tmp39B->name,_tmp399->name)== 0){
Cyc_Tcutil_unify_list(_tmp398,_tmp39A);
return;}
# 1543
goto _LL2EA;}else{goto _LL325;}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}else{switch(*((int*)_tmp326.f2)){case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}}case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp326.f1)->f1).field_info).KnownDatatypefield).tag == 2)switch(*((int*)_tmp326.f2)){case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp326.f2)->f1).field_info).KnownDatatypefield).tag == 2){_LL2F9: _tmp3A1=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp326.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp3A0=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp326.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp39F=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp326.f1)->f1).targs;_tmp39E=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp326.f2)->f1).field_info).KnownDatatypefield).val).f1;_tmp39D=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp326.f2)->f1).field_info).KnownDatatypefield).val).f2;_tmp39C=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp326.f2)->f1).targs;_LL2FA:
# 1547
 if((_tmp3A1 == _tmp39E  || Cyc_Absyn_qvar_cmp(_tmp3A1->name,_tmp39E->name)== 0) && (
_tmp3A0 == _tmp39D  || Cyc_Absyn_qvar_cmp(_tmp3A0->name,_tmp39D->name)== 0)){
Cyc_Tcutil_unify_list(_tmp39C,_tmp39F);
return;}
# 1552
Cyc_Tcutil_failure_reason=({const char*_tmp33D="(different datatype field types)";_tag_dyneither(_tmp33D,sizeof(char),33);});
goto _LL2EA;}else{goto _LL325;}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}else{switch(*((int*)_tmp326.f2)){case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}}case 5: switch(*((int*)_tmp326.f2)){case 5: _LL2FB: _tmp3AD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326.f1)->f1).elt_typ;_tmp3AC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326.f1)->f1).elt_tq;_tmp3AB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326.f1)->f1).ptr_atts).rgn;_tmp3AA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326.f1)->f1).ptr_atts).nullable;_tmp3A9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326.f1)->f1).ptr_atts).bounds;_tmp3A8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326.f1)->f1).ptr_atts).zero_term;_tmp3A7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326.f2)->f1).elt_typ;_tmp3A6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326.f2)->f1).elt_tq;_tmp3A5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326.f2)->f1).ptr_atts).rgn;_tmp3A4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326.f2)->f1).ptr_atts).nullable;_tmp3A3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326.f2)->f1).ptr_atts).bounds;_tmp3A2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326.f2)->f1).ptr_atts).zero_term;_LL2FC:
# 1557
 Cyc_Tcutil_unify_it(_tmp3A7,_tmp3AD);
Cyc_Tcutil_unify_it(_tmp3AB,_tmp3A5);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp3A2,_tmp3A8,({const char*_tmp33E="(not both zero terminated)";_tag_dyneither(_tmp33E,sizeof(char),27);}));
Cyc_Tcutil_unify_tqual(_tmp3A6,_tmp3A7,_tmp3AC,_tmp3AD);
((void(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp3A3,_tmp3A9,({const char*_tmp33F="(different pointer bounds)";_tag_dyneither(_tmp33F,sizeof(char),27);}));
# 1565
{void*_tmp340=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp3A3);void*_tmp341=_tmp340;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp341)->tag == 0){_LL32E: _LL32F:
 return;}else{_LL330: _LL331:
 goto _LL32D;}_LL32D:;}
# 1569
((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp3A4,_tmp3AA,({const char*_tmp342="(incompatible pointer types)";_tag_dyneither(_tmp342,sizeof(char),29);}));
return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 6: switch(*((int*)_tmp326.f2)){case 6: _LL2FD: _tmp3B1=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp326.f1)->f1;_tmp3B0=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp326.f1)->f2;_tmp3AF=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp326.f2)->f1;_tmp3AE=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp326.f2)->f2;_LL2FE:
# 1574
 if(_tmp3AF == _tmp3B1  && ((_tmp3AE == _tmp3B0  || _tmp3AE == Cyc_Absyn_Int_sz  && _tmp3B0 == Cyc_Absyn_Long_sz) || 
_tmp3AE == Cyc_Absyn_Long_sz  && _tmp3B0 == Cyc_Absyn_Int_sz))return;
Cyc_Tcutil_failure_reason=({const char*_tmp343="(different integral types)";_tag_dyneither(_tmp343,sizeof(char),27);});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 7: switch(*((int*)_tmp326.f2)){case 7: _LL2FF: _tmp3B3=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp326.f1)->f1;_tmp3B2=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp326.f2)->f1;_LL300:
# 1580
 if(_tmp3B2 == 0  && _tmp3B3 == 0)return;else{
if(_tmp3B2 == 1  && _tmp3B3 == 1)return;else{
# 1583
if(((_tmp3B2 != 0  && _tmp3B2 != 1) && _tmp3B3 != 0) && _tmp3B3 != 1)return;}}
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 19: switch(*((int*)_tmp326.f2)){case 19: _LL301: _tmp3B5=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp326.f1)->f1;_tmp3B4=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp326.f2)->f1;_LL302:
# 1586
 Cyc_Tcutil_unify_it(_tmp3B5,_tmp3B4);return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 18: switch(*((int*)_tmp326.f2)){case 18: _LL303: _tmp3B7=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp326.f1)->f1;_tmp3B6=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp326.f2)->f1;_LL304:
# 1589
 if(!Cyc_Evexp_same_const_exp(_tmp3B7,_tmp3B6)){
Cyc_Tcutil_failure_reason=({const char*_tmp344="(cannot prove expressions are the same)";_tag_dyneither(_tmp344,sizeof(char),40);});
goto _LL2EA;}
# 1593
return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 8: switch(*((int*)_tmp326.f2)){case 8: _LL305: _tmp3BF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp326.f1)->f1).elt_type;_tmp3BE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp326.f1)->f1).tq;_tmp3BD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp326.f1)->f1).num_elts;_tmp3BC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp326.f1)->f1).zero_term;_tmp3BB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp326.f2)->f1).elt_type;_tmp3BA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp326.f2)->f1).tq;_tmp3B9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp326.f2)->f1).num_elts;_tmp3B8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp326.f2)->f1).zero_term;_LL306:
# 1597
 Cyc_Tcutil_unify_it(_tmp3BB,_tmp3BF);
Cyc_Tcutil_unify_tqual(_tmp3BA,_tmp3BB,_tmp3BE,_tmp3BF);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp3BC,_tmp3B8,({const char*_tmp345="(not both zero terminated)";_tag_dyneither(_tmp345,sizeof(char),27);}));
if(_tmp3BD == _tmp3B9)return;
if(_tmp3BD == 0  || _tmp3B9 == 0)goto _LL2EA;
if(Cyc_Evexp_same_const_exp(_tmp3BD,_tmp3B9))
return;
Cyc_Tcutil_failure_reason=({const char*_tmp346="(different array sizes)";_tag_dyneither(_tmp346,sizeof(char),24);});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 9: switch(*((int*)_tmp326.f2)){case 9: _LL307: _tmp3D9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f1)->f1).tvars;_tmp3D8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f1)->f1).effect;_tmp3D7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f1)->f1).ret_tqual;_tmp3D6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f1)->f1).ret_typ;_tmp3D5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f1)->f1).args;_tmp3D4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f1)->f1).c_varargs;_tmp3D3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f1)->f1).cyc_varargs;_tmp3D2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f1)->f1).rgn_po;_tmp3D1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f1)->f1).attributes;_tmp3D0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f1)->f1).requires_clause;_tmp3CF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f1)->f1).requires_relns;_tmp3CE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f1)->f1).ensures_clause;_tmp3CD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f1)->f1).ensures_relns;_tmp3CC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f2)->f1).tvars;_tmp3CB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f2)->f1).effect;_tmp3CA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f2)->f1).ret_tqual;_tmp3C9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f2)->f1).ret_typ;_tmp3C8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f2)->f1).args;_tmp3C7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f2)->f1).c_varargs;_tmp3C6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f2)->f1).cyc_varargs;_tmp3C5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f2)->f1).rgn_po;_tmp3C4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f2)->f1).attributes;_tmp3C3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f2)->f1).requires_clause;_tmp3C2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f2)->f1).requires_relns;_tmp3C1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f2)->f1).ensures_clause;_tmp3C0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp326.f2)->f1).ensures_relns;_LL308: {
# 1611
int done=0;
{struct _RegionHandle _tmp347=_new_region("rgn");struct _RegionHandle*rgn=& _tmp347;_push_region(rgn);
{struct Cyc_List_List*inst=0;
while(_tmp3CC != 0){
if(_tmp3D9 == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp348="(second function type has too few type variables)";_tag_dyneither(_tmp348,sizeof(char),50);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1619
void*_tmp349=((struct Cyc_Absyn_Tvar*)_tmp3CC->hd)->kind;
void*_tmp34A=((struct Cyc_Absyn_Tvar*)_tmp3D9->hd)->kind;
if(!Cyc_Tcutil_unify_kindbound(_tmp349,_tmp34A)){
Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp34F;_tmp34F.tag=0;_tmp34F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1625
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp3D9->hd,& Cyc_Tcutil_bk)));({struct Cyc_String_pa_PrintArg_struct _tmp34E;_tmp34E.tag=0;_tmp34E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1624
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp3CC->hd,& Cyc_Tcutil_bk)));({struct Cyc_String_pa_PrintArg_struct _tmp34D;_tmp34D.tag=0;_tmp34D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1623
Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)_tmp3CC->hd));({void*_tmp34B[3]={& _tmp34D,& _tmp34E,& _tmp34F};Cyc_aprintf(({const char*_tmp34C="(type var %s has different kinds %s and %s)";_tag_dyneither(_tmp34C,sizeof(char),44);}),_tag_dyneither(_tmp34B,sizeof(void*),3));});});});});
# 1626
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1628
inst=({struct Cyc_List_List*_tmp350=_region_malloc(rgn,sizeof(*_tmp350));_tmp350->hd=({struct _tuple16*_tmp351=_region_malloc(rgn,sizeof(*_tmp351));_tmp351->f1=(struct Cyc_Absyn_Tvar*)_tmp3D9->hd;_tmp351->f2=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp352=_cycalloc(sizeof(*_tmp352));_tmp352[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp353;_tmp353.tag=2;_tmp353.f1=(struct Cyc_Absyn_Tvar*)_tmp3CC->hd;_tmp353;});_tmp352;});_tmp351;});_tmp350->tl=inst;_tmp350;});
_tmp3CC=_tmp3CC->tl;
_tmp3D9=_tmp3D9->tl;};}
# 1632
if(_tmp3D9 != 0){
Cyc_Tcutil_failure_reason=({const char*_tmp354="(second function type has too many type variables)";_tag_dyneither(_tmp354,sizeof(char),51);});
_npop_handler(0);goto _LL2EA;}
# 1636
if(inst != 0){
Cyc_Tcutil_unify_it((void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp355=_cycalloc(sizeof(*_tmp355));_tmp355[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp356;_tmp356.tag=9;_tmp356.f1=({struct Cyc_Absyn_FnInfo _tmp357;_tmp357.tvars=0;_tmp357.effect=_tmp3CB;_tmp357.ret_tqual=_tmp3CA;_tmp357.ret_typ=_tmp3C9;_tmp357.args=_tmp3C8;_tmp357.c_varargs=_tmp3C7;_tmp357.cyc_varargs=_tmp3C6;_tmp357.rgn_po=_tmp3C5;_tmp357.attributes=_tmp3C4;_tmp357.requires_clause=_tmp3D0;_tmp357.requires_relns=_tmp3CF;_tmp357.ensures_clause=_tmp3CE;_tmp357.ensures_relns=_tmp3CD;_tmp357;});_tmp356;});_tmp355;}),
# 1640
Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp358=_cycalloc(sizeof(*_tmp358));_tmp358[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp359;_tmp359.tag=9;_tmp359.f1=({struct Cyc_Absyn_FnInfo _tmp35A;_tmp35A.tvars=0;_tmp35A.effect=_tmp3D8;_tmp35A.ret_tqual=_tmp3D7;_tmp35A.ret_typ=_tmp3D6;_tmp35A.args=_tmp3D5;_tmp35A.c_varargs=_tmp3D4;_tmp35A.cyc_varargs=_tmp3D3;_tmp35A.rgn_po=_tmp3D2;_tmp35A.attributes=_tmp3D1;_tmp35A.requires_clause=_tmp3C3;_tmp35A.requires_relns=_tmp3C2;_tmp35A.ensures_clause=_tmp3C1;_tmp35A.ensures_relns=_tmp3C0;_tmp35A;});_tmp359;});_tmp358;})));
# 1645
done=1;}}
# 1613
;_pop_region(rgn);}
# 1648
if(done)
return;
Cyc_Tcutil_unify_it(_tmp3C9,_tmp3D6);
Cyc_Tcutil_unify_tqual(_tmp3CA,_tmp3C9,_tmp3D7,_tmp3D6);
for(0;_tmp3C8 != 0  && _tmp3D5 != 0;(_tmp3C8=_tmp3C8->tl,_tmp3D5=_tmp3D5->tl)){
struct _tuple10 _tmp35B=*((struct _tuple10*)_tmp3C8->hd);struct _tuple10 _tmp35C=_tmp35B;struct Cyc_Absyn_Tqual _tmp362;void*_tmp361;_LL333: _tmp362=_tmp35C.f2;_tmp361=_tmp35C.f3;_LL334:;{
struct _tuple10 _tmp35D=*((struct _tuple10*)_tmp3D5->hd);struct _tuple10 _tmp35E=_tmp35D;struct Cyc_Absyn_Tqual _tmp360;void*_tmp35F;_LL336: _tmp360=_tmp35E.f2;_tmp35F=_tmp35E.f3;_LL337:;
Cyc_Tcutil_unify_it(_tmp361,_tmp35F);
Cyc_Tcutil_unify_tqual(_tmp362,_tmp361,_tmp360,_tmp35F);};}
# 1658
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(_tmp3C8 != 0  || _tmp3D5 != 0){
Cyc_Tcutil_failure_reason=({const char*_tmp363="(function types have different number of arguments)";_tag_dyneither(_tmp363,sizeof(char),52);});
goto _LL2EA;}
# 1664
if(_tmp3C7 != _tmp3D4){
Cyc_Tcutil_failure_reason=({const char*_tmp364="(only one function type takes C varargs)";_tag_dyneither(_tmp364,sizeof(char),41);});
goto _LL2EA;}{
# 1669
int bad_cyc_vararg=0;
{struct _tuple22 _tmp365=({struct _tuple22 _tmp371;_tmp371.f1=_tmp3C6;_tmp371.f2=_tmp3D3;_tmp371;});struct _tuple22 _tmp366=_tmp365;struct _dyneither_ptr*_tmp370;struct Cyc_Absyn_Tqual _tmp36F;void*_tmp36E;int _tmp36D;struct _dyneither_ptr*_tmp36C;struct Cyc_Absyn_Tqual _tmp36B;void*_tmp36A;int _tmp369;if(_tmp366.f1 == 0){if(_tmp366.f2 == 0){_LL339: _LL33A:
 goto _LL338;}else{_LL33B: _LL33C:
 goto _LL33E;}}else{if(_tmp366.f2 == 0){_LL33D: _LL33E:
# 1674
 bad_cyc_vararg=1;
Cyc_Tcutil_failure_reason=({const char*_tmp367="(only one function type takes varargs)";_tag_dyneither(_tmp367,sizeof(char),39);});
goto _LL338;}else{_LL33F: _tmp370=(_tmp366.f1)->name;_tmp36F=(_tmp366.f1)->tq;_tmp36E=(_tmp366.f1)->type;_tmp36D=(_tmp366.f1)->inject;_tmp36C=(_tmp366.f2)->name;_tmp36B=(_tmp366.f2)->tq;_tmp36A=(_tmp366.f2)->type;_tmp369=(_tmp366.f2)->inject;_LL340:
# 1678
 Cyc_Tcutil_unify_it(_tmp36E,_tmp36A);
Cyc_Tcutil_unify_tqual(_tmp36F,_tmp36E,_tmp36B,_tmp36A);
if(_tmp36D != _tmp369){
bad_cyc_vararg=1;
Cyc_Tcutil_failure_reason=({const char*_tmp368="(only one function type injects varargs)";_tag_dyneither(_tmp368,sizeof(char),41);});}
# 1684
goto _LL338;}}_LL338:;}
# 1686
if(bad_cyc_vararg)goto _LL2EA;{
# 1689
int bad_effect=0;
{struct _tuple0 _tmp372=({struct _tuple0 _tmp374;_tmp374.f1=_tmp3CB;_tmp374.f2=_tmp3D8;_tmp374;});struct _tuple0 _tmp373=_tmp372;if(_tmp373.f1 == 0){if(_tmp373.f2 == 0){_LL342: _LL343:
 goto _LL341;}else{_LL344: _LL345:
 goto _LL347;}}else{if(_tmp373.f2 == 0){_LL346: _LL347:
 bad_effect=1;goto _LL341;}else{_LL348: _LL349:
 bad_effect=!Cyc_Tcutil_unify_effect((void*)_check_null(_tmp3CB),(void*)_check_null(_tmp3D8));goto _LL341;}}_LL341:;}
# 1696
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(bad_effect){
Cyc_Tcutil_failure_reason=({const char*_tmp375="(function effects do not match)";_tag_dyneither(_tmp375,sizeof(char),32);});
goto _LL2EA;}
# 1702
if(!Cyc_Tcutil_same_atts(_tmp3D1,_tmp3C4)){
Cyc_Tcutil_failure_reason=({const char*_tmp376="(function types have different attributes)";_tag_dyneither(_tmp376,sizeof(char),43);});
goto _LL2EA;}
# 1706
if(!Cyc_Tcutil_same_rgn_po(_tmp3D2,_tmp3C5)){
Cyc_Tcutil_failure_reason=({const char*_tmp377="(function types have different region lifetime orderings)";_tag_dyneither(_tmp377,sizeof(char),58);});
goto _LL2EA;}
# 1710
if(!Cyc_Tcutil_check_logical_equivalence(_tmp3CF,_tmp3C2)){
Cyc_Tcutil_failure_reason=({const char*_tmp378="(@requires clauses not equivalent)";_tag_dyneither(_tmp378,sizeof(char),35);});
goto _LL2EA;}
# 1714
if(!Cyc_Tcutil_check_logical_equivalence(_tmp3CD,_tmp3C0)){
Cyc_Tcutil_failure_reason=({const char*_tmp379="(@ensures clauses not equivalent)";_tag_dyneither(_tmp379,sizeof(char),34);});
goto _LL2EA;}
# 1718
return;};};}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 10: switch(*((int*)_tmp326.f2)){case 10: _LL309: _tmp3DB=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp326.f1)->f1;_tmp3DA=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp326.f2)->f1;_LL30A:
# 1721
 for(0;_tmp3DA != 0  && _tmp3DB != 0;(_tmp3DA=_tmp3DA->tl,_tmp3DB=_tmp3DB->tl)){
struct _tuple12 _tmp37A=*((struct _tuple12*)_tmp3DA->hd);struct _tuple12 _tmp37B=_tmp37A;struct Cyc_Absyn_Tqual _tmp381;void*_tmp380;_LL34B: _tmp381=_tmp37B.f1;_tmp380=_tmp37B.f2;_LL34C:;{
struct _tuple12 _tmp37C=*((struct _tuple12*)_tmp3DB->hd);struct _tuple12 _tmp37D=_tmp37C;struct Cyc_Absyn_Tqual _tmp37F;void*_tmp37E;_LL34E: _tmp37F=_tmp37D.f1;_tmp37E=_tmp37D.f2;_LL34F:;
Cyc_Tcutil_unify_it(_tmp380,_tmp37E);
Cyc_Tcutil_unify_tqual(_tmp381,_tmp380,_tmp37F,_tmp37E);};}
# 1727
if(_tmp3DA == 0  && _tmp3DB == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp382="(tuple types have different numbers of components)";_tag_dyneither(_tmp382,sizeof(char),51);});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 12: switch(*((int*)_tmp326.f2)){case 12: _LL30B: _tmp3DF=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp326.f1)->f1;_tmp3DE=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp326.f1)->f2;_tmp3DD=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp326.f2)->f1;_tmp3DC=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp326.f2)->f2;_LL30C:
# 1734
 if(_tmp3DD != _tmp3DF){Cyc_Tcutil_failure_reason=({const char*_tmp383="(struct and union type)";_tag_dyneither(_tmp383,sizeof(char),24);});goto _LL2EA;}
for(0;_tmp3DC != 0  && _tmp3DE != 0;(_tmp3DC=_tmp3DC->tl,_tmp3DE=_tmp3DE->tl)){
struct Cyc_Absyn_Aggrfield*_tmp384=(struct Cyc_Absyn_Aggrfield*)_tmp3DC->hd;
struct Cyc_Absyn_Aggrfield*_tmp385=(struct Cyc_Absyn_Aggrfield*)_tmp3DE->hd;
if(Cyc_strptrcmp(_tmp384->name,_tmp385->name)!= 0){
Cyc_Tcutil_failure_reason=({const char*_tmp386="(different member names)";_tag_dyneither(_tmp386,sizeof(char),25);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1742
Cyc_Tcutil_unify_it(_tmp384->type,_tmp385->type);
Cyc_Tcutil_unify_tqual(_tmp384->tq,_tmp384->type,_tmp385->tq,_tmp385->type);
if(!Cyc_Tcutil_same_atts(_tmp384->attributes,_tmp385->attributes)){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp387="(different attributes on member)";_tag_dyneither(_tmp387,sizeof(char),33);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1750
if((_tmp384->width != 0  && _tmp385->width == 0  || 
_tmp385->width != 0  && _tmp384->width == 0) || 
(_tmp384->width != 0  && _tmp385->width != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp384->width),(struct Cyc_Absyn_Exp*)_check_null(_tmp385->width))){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp388="(different bitfield widths on member)";_tag_dyneither(_tmp388,sizeof(char),38);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1759
if((_tmp384->requires_clause != 0  && _tmp385->requires_clause == 0  || 
_tmp384->requires_clause == 0  && _tmp385->requires_clause != 0) || 
(_tmp384->requires_clause == 0  && _tmp385->requires_clause != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp384->requires_clause),(struct Cyc_Absyn_Exp*)_check_null(_tmp385->requires_clause))){
# 1764
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp389="(different @requires clauses on member)";_tag_dyneither(_tmp389,sizeof(char),40);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1770
if(_tmp3DC == 0  && _tmp3DE == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp38A="(different number of members)";_tag_dyneither(_tmp38A,sizeof(char),30);});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 17: switch(*((int*)_tmp326.f2)){case 17: _LL30D: _tmp3E3=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp326.f1)->f2;_tmp3E2=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp326.f1)->f3;_tmp3E1=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp326.f2)->f2;_tmp3E0=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp326.f2)->f3;_LL30E:
# 1776
 if(_tmp3E2 != _tmp3E0){
Cyc_Tcutil_failure_reason=({const char*_tmp38B="(different abstract typedefs)";_tag_dyneither(_tmp38B,sizeof(char),30);});
goto _LL2EA;}
# 1780
Cyc_Tcutil_failure_reason=({const char*_tmp38C="(type parameters to typedef differ)";_tag_dyneither(_tmp38C,sizeof(char),36);});
Cyc_Tcutil_unify_list(_tmp3E3,_tmp3E1);
return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 20: switch(*((int*)_tmp326.f2)){case 20: _LL30F: _LL310:
 return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 21: switch(*((int*)_tmp326.f2)){case 21: _LL311: _LL312:
 return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 22: switch(*((int*)_tmp326.f2)){case 22: _LL313: _LL314:
 return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 15: switch(*((int*)_tmp326.f2)){case 15: _LL315: _tmp3E5=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp326.f1)->f1;_tmp3E4=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp326.f2)->f1;_LL316:
# 1787
 Cyc_Tcutil_unify_it(_tmp3E5,_tmp3E4);
return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 16: switch(*((int*)_tmp326.f2)){case 16: _LL317: _tmp3E9=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp326.f1)->f1;_tmp3E8=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp326.f1)->f2;_tmp3E7=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp326.f2)->f1;_tmp3E6=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp326.f2)->f2;_LL318:
# 1790
 Cyc_Tcutil_unify_it(_tmp3E9,_tmp3E7);
Cyc_Tcutil_unify_it(_tmp3E8,_tmp3E6);
return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 24: _LL319: _LL31A:
 goto _LL31C;default: if(((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp326.f2)->tag == 24){_LL31B: _LL31C:
 goto _LL31E;}else{switch(*((int*)_tmp326.f1)){case 23: _LL31D: _LL31E:
 goto _LL320;case 25: _LL31F: _LL320:
 goto _LL322;default: switch(*((int*)_tmp326.f2)){case 25: _LL321: _LL322:
 goto _LL324;case 23: _LL323: _LL324:
# 1799
 if(Cyc_Tcutil_unify_effect(t1,t2))return;
Cyc_Tcutil_failure_reason=({const char*_tmp38D="(effects don't unify)";_tag_dyneither(_tmp38D,sizeof(char),22);});
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
union Cyc_Absyn_Constraint*_tmp3ED=x;void*_tmp3F4;switch((((union Cyc_Absyn_Constraint*)_tmp3ED)->Eq_constr).tag){case 3: _LL351: _LL352:
 return - 1;case 1: _LL353: _tmp3F4=(void*)(_tmp3ED->Eq_constr).val;_LL354: {
# 1828
union Cyc_Absyn_Constraint*_tmp3EE=y;void*_tmp3F1;switch((((union Cyc_Absyn_Constraint*)_tmp3EE)->Eq_constr).tag){case 3: _LL358: _LL359:
 return 1;case 1: _LL35A: _tmp3F1=(void*)(_tmp3EE->Eq_constr).val;_LL35B:
 return cmp(_tmp3F4,_tmp3F1);default: _LL35C: _LL35D:
({void*_tmp3EF=0;Cyc_Tcutil_impos(({const char*_tmp3F0="unify_conref: forward after compress(2)";_tag_dyneither(_tmp3F0,sizeof(char),40);}),_tag_dyneither(_tmp3EF,sizeof(void*),0));});}_LL357:;}default: _LL355: _LL356:
# 1833
({void*_tmp3F2=0;Cyc_Tcutil_impos(({const char*_tmp3F3="unify_conref: forward after compress";_tag_dyneither(_tmp3F3,sizeof(char),37);}),_tag_dyneither(_tmp3F2,sizeof(void*),0));});}_LL350:;};}
# 1837
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct _tuple12*_tmp3F5=tqt1;struct Cyc_Absyn_Tqual _tmp3FB;void*_tmp3FA;_LL35F: _tmp3FB=_tmp3F5->f1;_tmp3FA=_tmp3F5->f2;_LL360:;{
struct _tuple12*_tmp3F6=tqt2;struct Cyc_Absyn_Tqual _tmp3F9;void*_tmp3F8;_LL362: _tmp3F9=_tmp3F6->f1;_tmp3F8=_tmp3F6->f2;_LL363:;{
int _tmp3F7=Cyc_Tcutil_tqual_cmp(_tmp3FB,_tmp3F9);
if(_tmp3F7 != 0)return _tmp3F7;
return Cyc_Tcutil_typecmp(_tmp3FA,_tmp3F8);};};}
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
struct _tuple0 _tmp403=({struct _tuple0 _tmp466;_tmp466.f1=t2;_tmp466.f2=t1;_tmp466;});struct _tuple0 _tmp404=_tmp403;struct _dyneither_ptr _tmp465;struct _dyneither_ptr _tmp464;struct Cyc_Absyn_Exp*_tmp463;struct Cyc_Absyn_Exp*_tmp462;struct Cyc_Absyn_Exp*_tmp461;struct Cyc_Absyn_Exp*_tmp460;void*_tmp45F;void*_tmp45E;void*_tmp45D;void*_tmp45C;void*_tmp45B;void*_tmp45A;void*_tmp459;void*_tmp458;enum Cyc_Absyn_AggrKind _tmp457;struct Cyc_List_List*_tmp456;enum Cyc_Absyn_AggrKind _tmp455;struct Cyc_List_List*_tmp454;struct Cyc_List_List*_tmp453;struct Cyc_List_List*_tmp452;void*_tmp451;struct Cyc_Absyn_Tqual _tmp450;struct Cyc_Absyn_Exp*_tmp44F;union Cyc_Absyn_Constraint*_tmp44E;void*_tmp44D;struct Cyc_Absyn_Tqual _tmp44C;struct Cyc_Absyn_Exp*_tmp44B;union Cyc_Absyn_Constraint*_tmp44A;int _tmp449;int _tmp448;enum Cyc_Absyn_Sign _tmp447;enum Cyc_Absyn_Size_of _tmp446;enum Cyc_Absyn_Sign _tmp445;enum Cyc_Absyn_Size_of _tmp444;void*_tmp443;struct Cyc_Absyn_Tqual _tmp442;void*_tmp441;union Cyc_Absyn_Constraint*_tmp440;union Cyc_Absyn_Constraint*_tmp43F;union Cyc_Absyn_Constraint*_tmp43E;void*_tmp43D;struct Cyc_Absyn_Tqual _tmp43C;void*_tmp43B;union Cyc_Absyn_Constraint*_tmp43A;union Cyc_Absyn_Constraint*_tmp439;union Cyc_Absyn_Constraint*_tmp438;struct Cyc_Absyn_Datatypedecl*_tmp437;struct Cyc_Absyn_Datatypefield*_tmp436;struct Cyc_List_List*_tmp435;struct Cyc_Absyn_Datatypedecl*_tmp434;struct Cyc_Absyn_Datatypefield*_tmp433;struct Cyc_List_List*_tmp432;struct Cyc_Absyn_Datatypedecl*_tmp431;struct Cyc_List_List*_tmp430;struct Cyc_Absyn_Datatypedecl*_tmp42F;struct Cyc_List_List*_tmp42E;struct Cyc_List_List*_tmp42D;struct Cyc_List_List*_tmp42C;struct _tuple2*_tmp42B;struct _tuple2*_tmp42A;union Cyc_Absyn_AggrInfoU _tmp429;struct Cyc_List_List*_tmp428;union Cyc_Absyn_AggrInfoU _tmp427;struct Cyc_List_List*_tmp426;struct Cyc_Absyn_Tvar*_tmp425;struct Cyc_Absyn_Tvar*_tmp424;switch(*((int*)_tmp404.f1)){case 1: switch(*((int*)_tmp404.f2)){case 1: _LL3A2: _LL3A3:
# 1913
({void*_tmp405=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp406="typecmp: can only compare closed types";_tag_dyneither(_tmp406,sizeof(char),39);}),_tag_dyneither(_tmp405,sizeof(void*),0));});case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 2: switch(*((int*)_tmp404.f2)){case 2: _LL3A4: _tmp425=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp404.f1)->f1;_tmp424=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp404.f2)->f1;_LL3A5:
# 1917
 return Cyc_Core_intcmp(_tmp424->identity,_tmp425->identity);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 11: switch(*((int*)_tmp404.f2)){case 11: _LL3A6: _tmp429=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp404.f1)->f1).aggr_info;_tmp428=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp404.f1)->f1).targs;_tmp427=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp404.f2)->f1).aggr_info;_tmp426=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp404.f2)->f1).targs;_LL3A7: {
# 1921
struct _tuple13 _tmp407=Cyc_Absyn_aggr_kinded_name(_tmp429);struct _tuple13 _tmp408=_tmp407;struct _tuple2*_tmp40D;_LL3D9: _tmp40D=_tmp408.f2;_LL3DA:;{
struct _tuple13 _tmp409=Cyc_Absyn_aggr_kinded_name(_tmp427);struct _tuple13 _tmp40A=_tmp409;struct _tuple2*_tmp40C;_LL3DC: _tmp40C=_tmp40A.f2;_LL3DD:;{
int _tmp40B=Cyc_Absyn_qvar_cmp(_tmp40D,_tmp40C);
if(_tmp40B != 0)return _tmp40B;else{
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp428,_tmp426);}};};}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 13: switch(*((int*)_tmp404.f2)){case 13: _LL3A8: _tmp42B=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp404.f1)->f1;_tmp42A=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp404.f2)->f1;_LL3A9:
# 1928
 return Cyc_Absyn_qvar_cmp(_tmp42B,_tmp42A);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 14: switch(*((int*)_tmp404.f2)){case 14: _LL3AA: _tmp42D=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp404.f1)->f1;_tmp42C=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp404.f2)->f1;_LL3AB:
# 1931
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp42D,_tmp42C);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp404.f1)->f1).datatype_info).KnownDatatype).tag == 2)switch(*((int*)_tmp404.f2)){case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp404.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL3AC: _tmp431=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp404.f1)->f1).datatype_info).KnownDatatype).val;_tmp430=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp404.f1)->f1).targs;_tmp42F=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp404.f2)->f1).datatype_info).KnownDatatype).val;_tmp42E=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp404.f2)->f1).targs;_LL3AD:
# 1935
 if(_tmp42F == _tmp431)return 0;{
int _tmp40E=Cyc_Absyn_qvar_cmp(_tmp42F->name,_tmp431->name);
if(_tmp40E != 0)return _tmp40E;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp42E,_tmp430);};}else{goto _LL3D6;}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}else{switch(*((int*)_tmp404.f2)){case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}}case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp404.f1)->f1).field_info).KnownDatatypefield).tag == 2)switch(*((int*)_tmp404.f2)){case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp404.f2)->f1).field_info).KnownDatatypefield).tag == 2){_LL3AE: _tmp437=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp404.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp436=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp404.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp435=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp404.f1)->f1).targs;_tmp434=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp404.f2)->f1).field_info).KnownDatatypefield).val).f1;_tmp433=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp404.f2)->f1).field_info).KnownDatatypefield).val).f2;_tmp432=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp404.f2)->f1).targs;_LL3AF:
# 1942
 if(_tmp434 == _tmp437)return 0;{
int _tmp40F=Cyc_Absyn_qvar_cmp(_tmp437->name,_tmp434->name);
if(_tmp40F != 0)return _tmp40F;{
int _tmp410=Cyc_Absyn_qvar_cmp(_tmp436->name,_tmp433->name);
if(_tmp410 != 0)return _tmp410;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp432,_tmp435);};};}else{goto _LL3D6;}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}else{switch(*((int*)_tmp404.f2)){case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}}case 5: switch(*((int*)_tmp404.f2)){case 5: _LL3B0: _tmp443=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp404.f1)->f1).elt_typ;_tmp442=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp404.f1)->f1).elt_tq;_tmp441=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp404.f1)->f1).ptr_atts).rgn;_tmp440=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp404.f1)->f1).ptr_atts).nullable;_tmp43F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp404.f1)->f1).ptr_atts).bounds;_tmp43E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp404.f1)->f1).ptr_atts).zero_term;_tmp43D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp404.f2)->f1).elt_typ;_tmp43C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp404.f2)->f1).elt_tq;_tmp43B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp404.f2)->f1).ptr_atts).rgn;_tmp43A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp404.f2)->f1).ptr_atts).nullable;_tmp439=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp404.f2)->f1).ptr_atts).bounds;_tmp438=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp404.f2)->f1).ptr_atts).zero_term;_LL3B1: {
# 1951
int _tmp411=Cyc_Tcutil_typecmp(_tmp43D,_tmp443);
if(_tmp411 != 0)return _tmp411;{
int _tmp412=Cyc_Tcutil_typecmp(_tmp43B,_tmp441);
if(_tmp412 != 0)return _tmp412;{
int _tmp413=Cyc_Tcutil_tqual_cmp(_tmp43C,_tmp442);
if(_tmp413 != 0)return _tmp413;{
int _tmp414=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp439,_tmp43F);
if(_tmp414 != 0)return _tmp414;{
int _tmp415=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp438,_tmp43E);
if(_tmp415 != 0)return _tmp415;
{void*_tmp416=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp439);void*_tmp417=_tmp416;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp417)->tag == 0){_LL3DF: _LL3E0:
 return 0;}else{_LL3E1: _LL3E2:
 goto _LL3DE;}_LL3DE:;}
# 1965
return((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp43A,_tmp440);};};};};}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 6: switch(*((int*)_tmp404.f2)){case 6: _LL3B2: _tmp447=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp404.f1)->f1;_tmp446=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp404.f1)->f2;_tmp445=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp404.f2)->f1;_tmp444=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp404.f2)->f2;_LL3B3:
# 1968
 if(_tmp445 != _tmp447)return Cyc_Core_intcmp((int)((unsigned int)_tmp445),(int)((unsigned int)_tmp447));
if(_tmp444 != _tmp446)return Cyc_Core_intcmp((int)((unsigned int)_tmp444),(int)((unsigned int)_tmp446));
return 0;case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 7: switch(*((int*)_tmp404.f2)){case 7: _LL3B4: _tmp449=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp404.f1)->f1;_tmp448=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp404.f2)->f1;_LL3B5:
# 1973
 if(_tmp449 == _tmp448)return 0;else{
if(_tmp448 == 0)return - 1;else{
if(_tmp448 == 1  && _tmp449 == 0)return - 1;else{
return 1;}}}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 8: switch(*((int*)_tmp404.f2)){case 8: _LL3B6: _tmp451=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp404.f1)->f1).elt_type;_tmp450=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp404.f1)->f1).tq;_tmp44F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp404.f1)->f1).num_elts;_tmp44E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp404.f1)->f1).zero_term;_tmp44D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp404.f2)->f1).elt_type;_tmp44C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp404.f2)->f1).tq;_tmp44B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp404.f2)->f1).num_elts;_tmp44A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp404.f2)->f1).zero_term;_LL3B7: {
# 1980
int _tmp418=Cyc_Tcutil_tqual_cmp(_tmp44C,_tmp450);
if(_tmp418 != 0)return _tmp418;{
int _tmp419=Cyc_Tcutil_typecmp(_tmp44D,_tmp451);
if(_tmp419 != 0)return _tmp419;{
int _tmp41A=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp44E,_tmp44A);
if(_tmp41A != 0)return _tmp41A;
if(_tmp44F == _tmp44B)return 0;
if(_tmp44F == 0  || _tmp44B == 0)
({void*_tmp41B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp41C="missing expression in array index";_tag_dyneither(_tmp41C,sizeof(char),34);}),_tag_dyneither(_tmp41B,sizeof(void*),0));});
# 1990
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp44F,_tmp44B);};};}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 9: switch(*((int*)_tmp404.f2)){case 9: _LL3B8: _LL3B9:
# 1993
({void*_tmp41D=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp41E="typecmp: function types not handled";_tag_dyneither(_tmp41E,sizeof(char),36);}),_tag_dyneither(_tmp41D,sizeof(void*),0));});case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 10: switch(*((int*)_tmp404.f2)){case 10: _LL3BA: _tmp453=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp404.f1)->f1;_tmp452=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp404.f2)->f1;_LL3BB:
# 1996
 return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp452,_tmp453);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 12: switch(*((int*)_tmp404.f2)){case 12: _LL3BC: _tmp457=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp404.f1)->f1;_tmp456=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp404.f1)->f2;_tmp455=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp404.f2)->f1;_tmp454=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp404.f2)->f2;_LL3BD:
# 1999
 if(_tmp455 != _tmp457){
if(_tmp455 == Cyc_Absyn_StructA)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp454,_tmp456);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 15: switch(*((int*)_tmp404.f2)){case 15: _LL3BE: _tmp459=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp404.f1)->f1;_tmp458=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp404.f2)->f1;_LL3BF:
# 2004
 return Cyc_Tcutil_typecmp(_tmp459,_tmp458);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 16: switch(*((int*)_tmp404.f2)){case 16: _LL3C0: _tmp45D=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp404.f1)->f1;_tmp45C=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp404.f1)->f2;_tmp45B=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp404.f2)->f1;_tmp45A=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp404.f2)->f2;_LL3C1: {
# 2006
int _tmp41F=Cyc_Tcutil_typecmp(_tmp45D,_tmp45B);
if(_tmp41F != 0)return _tmp41F;else{
return Cyc_Tcutil_typecmp(_tmp45C,_tmp45A);}}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 19: switch(*((int*)_tmp404.f2)){case 19: _LL3C2: _tmp45F=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp404.f1)->f1;_tmp45E=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp404.f2)->f1;_LL3C3:
 return Cyc_Tcutil_typecmp(_tmp45F,_tmp45E);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 18: switch(*((int*)_tmp404.f2)){case 18: _LL3C4: _tmp461=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp404.f1)->f1;_tmp460=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp404.f2)->f1;_LL3C5:
 _tmp463=_tmp461;_tmp462=_tmp460;goto _LL3C7;case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 27: switch(*((int*)_tmp404.f2)){case 27: _LL3C6: _tmp463=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp404.f1)->f1;_tmp462=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp404.f2)->f1;_LL3C7:
# 2012
 return Cyc_Evexp_const_exp_cmp(_tmp463,_tmp462);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 28: switch(*((int*)_tmp404.f2)){case 28: _LL3C8: _tmp465=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp404.f1)->f1;_tmp464=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp404.f2)->f1;_LL3C9:
# 2014
 return Cyc_strcmp((struct _dyneither_ptr)_tmp465,(struct _dyneither_ptr)_tmp464);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 24: _LL3CA: _LL3CB:
 goto _LL3CD;default: if(((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp404.f2)->tag == 24){_LL3CC: _LL3CD:
 goto _LL3CF;}else{switch(*((int*)_tmp404.f1)){case 23: _LL3CE: _LL3CF:
 goto _LL3D1;case 25: _LL3D0: _LL3D1:
 goto _LL3D3;default: switch(*((int*)_tmp404.f2)){case 25: _LL3D2: _LL3D3:
 goto _LL3D5;case 23: _LL3D4: _LL3D5:
({void*_tmp420=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp421="typecmp: effects not handled";_tag_dyneither(_tmp421,sizeof(char),29);}),_tag_dyneither(_tmp420,sizeof(void*),0));});default: _LL3D6: _LL3D7:
({void*_tmp422=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp423="Unmatched case in typecmp";_tag_dyneither(_tmp423,sizeof(char),26);}),_tag_dyneither(_tmp422,sizeof(void*),0));});}}}}_LL3A1:;};};}
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
struct _tuple0 _tmp469=({struct _tuple0 _tmp46D;_tmp46D.f1=t1;_tmp46D.f2=t2;_tmp46D;});struct _tuple0 _tmp46A=_tmp469;int _tmp46C;int _tmp46B;switch(*((int*)_tmp46A.f1)){case 7: switch(*((int*)_tmp46A.f2)){case 7: _LL3EF: _tmp46C=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp46A.f1)->f1;_tmp46B=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp46A.f2)->f1;_LL3F0:
# 2042
 return _tmp46B == 0  && _tmp46C != 0  || (_tmp46B == 1  && _tmp46C != 0) && _tmp46C != 1;case 6: _LL3F1: _LL3F2:
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
struct _tuple0 _tmp47F=({struct _tuple0 _tmp498;_tmp498.f1=t1;_tmp498.f2=t2;_tmp498;});struct _tuple0 _tmp480=_tmp47F;void*_tmp497;struct Cyc_Absyn_Tqual _tmp496;struct Cyc_Absyn_Exp*_tmp495;union Cyc_Absyn_Constraint*_tmp494;void*_tmp493;struct Cyc_Absyn_Tqual _tmp492;struct Cyc_Absyn_Exp*_tmp491;union Cyc_Absyn_Constraint*_tmp490;struct Cyc_Absyn_PtrInfo _tmp48F;struct Cyc_Absyn_PtrInfo _tmp48E;switch(*((int*)_tmp480.f1)){case 5: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp480.f2)->tag == 5){_LL422: _tmp48F=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp480.f1)->f1;_tmp48E=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp480.f2)->f1;_LL423: {
# 2160
int okay=1;
# 2162
if(!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp48F.ptr_atts).nullable,(_tmp48E.ptr_atts).nullable))
# 2165
okay=!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp48F.ptr_atts).nullable);
# 2167
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp48F.ptr_atts).bounds,(_tmp48E.ptr_atts).bounds)){
# 2170
struct _tuple0 _tmp481=({struct _tuple0 _tmp488;_tmp488.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp48F.ptr_atts).bounds);_tmp488.f2=
((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp48E.ptr_atts).bounds);_tmp488;});
# 2170
struct _tuple0 _tmp482=_tmp481;struct Cyc_Absyn_Exp*_tmp487;struct Cyc_Absyn_Exp*_tmp486;struct Cyc_Absyn_Exp*_tmp485;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp482.f1)->tag == 1){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp482.f2)->tag == 0){_LL42B: _LL42C:
# 2172
 goto _LL42E;}else{_LL42F: _tmp486=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp482.f1)->f1;_tmp485=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp482.f2)->f1;_LL430:
# 2175
 okay=okay  && Cyc_Evexp_lte_const_exp(_tmp485,_tmp486);
# 2179
if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp48E.ptr_atts).zero_term))
({void*_tmp483=0;Cyc_Tcutil_warn(loc,({const char*_tmp484="implicit cast to shorter array";_tag_dyneither(_tmp484,sizeof(char),31);}),_tag_dyneither(_tmp483,sizeof(void*),0));});
goto _LL42A;}}else{if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp482.f2)->tag == 0){_LL42D: _LL42E:
# 2173
 okay=1;goto _LL42A;}else{_LL431: _tmp487=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp482.f2)->f1;_LL432:
# 2184
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp48F.ptr_atts).zero_term) && 
Cyc_Tcutil_is_bound_one((_tmp48E.ptr_atts).bounds))
goto _LL42A;
okay=0;
goto _LL42A;}}_LL42A:;}
# 2193
okay=okay  && (!(_tmp48F.elt_tq).real_const  || (_tmp48E.elt_tq).real_const);
# 2196
if(!Cyc_Tcutil_unify((_tmp48F.ptr_atts).rgn,(_tmp48E.ptr_atts).rgn)){
if(Cyc_Tcenv_region_outlives(te,(_tmp48F.ptr_atts).rgn,(_tmp48E.ptr_atts).rgn)){
if(Cyc_Tcutil_warn_region_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp48C;_tmp48C.tag=0;_tmp48C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2201
Cyc_Absynpp_typ2string((_tmp48E.ptr_atts).rgn));({struct Cyc_String_pa_PrintArg_struct _tmp48B;_tmp48B.tag=0;_tmp48B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2200
Cyc_Absynpp_typ2string((_tmp48F.ptr_atts).rgn));({void*_tmp489[2]={& _tmp48B,& _tmp48C};Cyc_Tcutil_warn(loc,({const char*_tmp48A="implicit cast form region %s to region %s";_tag_dyneither(_tmp48A,sizeof(char),42);}),_tag_dyneither(_tmp489,sizeof(void*),2));});});});}else{
# 2202
okay=0;}}
# 2206
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp48F.ptr_atts).zero_term,(_tmp48E.ptr_atts).zero_term) || 
# 2209
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,(_tmp48F.ptr_atts).zero_term) && (_tmp48E.elt_tq).real_const);{
# 2217
int _tmp48D=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp48E.ptr_atts).bounds,Cyc_Absyn_bounds_one_conref) && !
# 2220
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp48E.ptr_atts).zero_term);
# 2224
okay=okay  && (Cyc_Tcutil_unify(_tmp48F.elt_typ,_tmp48E.elt_typ) || 
(_tmp48D  && ((_tmp48E.elt_tq).real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp48E.elt_typ)))) && Cyc_Tcutil_ptrsubtype(te,0,_tmp48F.elt_typ,_tmp48E.elt_typ));
# 2227
return okay;};}}else{goto _LL428;}case 8: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp480.f2)->tag == 8){_LL424: _tmp497=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp480.f1)->f1).elt_type;_tmp496=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp480.f1)->f1).tq;_tmp495=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp480.f1)->f1).num_elts;_tmp494=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp480.f1)->f1).zero_term;_tmp493=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp480.f2)->f1).elt_type;_tmp492=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp480.f2)->f1).tq;_tmp491=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp480.f2)->f1).num_elts;_tmp490=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp480.f2)->f1).zero_term;_LL425: {
# 2231
int okay;
# 2233
okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp494,_tmp490) && (
(_tmp495 != 0  && _tmp491 != 0) && Cyc_Evexp_same_const_exp(_tmp495,_tmp491));
# 2236
return(okay  && Cyc_Tcutil_unify(_tmp497,_tmp493)) && (!_tmp496.real_const  || _tmp492.real_const);}}else{goto _LL428;}case 19: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp480.f2)->tag == 6){_LL426: _LL427:
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
void*_tmp49B=Cyc_Tcutil_compress(t);void*_tmp49C=_tmp49B;void*_tmp49F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49C)->tag == 5){_LL439: _tmp49F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49C)->f1).elt_typ;_LL43A:
 return _tmp49F;}else{_LL43B: _LL43C:
({void*_tmp49D=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp49E="pointer_elt_type";_tag_dyneither(_tmp49E,sizeof(char),17);}),_tag_dyneither(_tmp49D,sizeof(void*),0));});}_LL438:;}
# 2256
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp4A0=Cyc_Tcutil_compress(t);void*_tmp4A1=_tmp4A0;struct Cyc_Absyn_PtrAtts*_tmp4A4;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A1)->tag == 5){_LL43E: _tmp4A4=(struct Cyc_Absyn_PtrAtts*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A1)->f1).ptr_atts;_LL43F:
 return _tmp4A4->rgn;}else{_LL440: _LL441:
({void*_tmp4A2=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4A3="pointer_elt_type";_tag_dyneither(_tmp4A3,sizeof(char),17);}),_tag_dyneither(_tmp4A2,sizeof(void*),0));});}_LL43D:;}
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
void*_tmp4AB=e->r;void*_tmp4AC=_tmp4AB;void*_tmp4B0;struct Cyc_Absyn_Exp*_tmp4AF;struct _dyneither_ptr _tmp4AE;switch(*((int*)_tmp4AC)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4AC)->f1).Wchar_c).tag){case 5: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4AC)->f1).Int_c).val).f2 == 0){_LL44D: _LL44E:
 goto _LL450;}else{goto _LL459;}case 2: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4AC)->f1).Char_c).val).f2 == 0){_LL44F: _LL450:
 goto _LL452;}else{goto _LL459;}case 4: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4AC)->f1).Short_c).val).f2 == 0){_LL451: _LL452:
 goto _LL454;}else{goto _LL459;}case 6: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4AC)->f1).LongLong_c).val).f2 == 0){_LL453: _LL454:
# 2292
 return 1;}else{goto _LL459;}case 3: _LL455: _tmp4AE=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4AC)->f1).Wchar_c).val;_LL456: {
# 2294
unsigned long _tmp4AD=Cyc_strlen((struct _dyneither_ptr)_tmp4AE);
int i=0;
if(_tmp4AD >= 2  && *((const char*)_check_dyneither_subscript(_tmp4AE,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp4AE,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmp4AE,sizeof(char),1))== 'x'  && _tmp4AD >= 3) && *((const char*)_check_dyneither_subscript(_tmp4AE,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp4AD;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp4AE,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 2304
return 0;}}default: goto _LL459;}case 13: _LL457: _tmp4B0=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4AC)->f1;_tmp4AF=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4AC)->f2;_LL458:
 return Cyc_Tcutil_is_zero(_tmp4AF) && Cyc_Tcutil_admits_zero(_tmp4B0);default: _LL459: _LL45A:
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
struct Cyc_Absyn_Kind*_tmp4B1=ka;enum Cyc_Absyn_KindQual _tmp4BA;enum Cyc_Absyn_AliasQual _tmp4B9;_LL45C: _tmp4BA=_tmp4B1->kind;_tmp4B9=_tmp4B1->aliasqual;_LL45D:;
{enum Cyc_Absyn_AliasQual _tmp4B2=_tmp4B9;switch(_tmp4B2){case Cyc_Absyn_Aliasable: _LL45F: _LL460: {
# 2348
enum Cyc_Absyn_KindQual _tmp4B3=_tmp4BA;switch(_tmp4B3){case Cyc_Absyn_AnyKind: _LL466: _LL467:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LL468: _LL469:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL46A: _LL46B:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL46C: _LL46D:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL46E: _LL46F:
 return& Cyc_Tcutil_eko;default: _LL470: _LL471:
 return& Cyc_Tcutil_iko;}_LL465:;}case Cyc_Absyn_Unique: _LL461: _LL462:
# 2357
{enum Cyc_Absyn_KindQual _tmp4B4=_tmp4BA;switch(_tmp4B4){case Cyc_Absyn_AnyKind: _LL473: _LL474:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL475: _LL476:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL477: _LL478:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL479: _LL47A:
 return& Cyc_Tcutil_urko;default: _LL47B: _LL47C:
 goto _LL472;}_LL472:;}
# 2364
goto _LL45E;default: _LL463: _LL464:
# 2366
{enum Cyc_Absyn_KindQual _tmp4B5=_tmp4BA;switch(_tmp4B5){case Cyc_Absyn_AnyKind: _LL47E: _LL47F:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL480: _LL481:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL482: _LL483:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL484: _LL485:
 return& Cyc_Tcutil_trko;default: _LL486: _LL487:
 goto _LL47D;}_LL47D:;}
# 2373
goto _LL45E;}_LL45E:;}
# 2375
({struct Cyc_String_pa_PrintArg_struct _tmp4B8;_tmp4B8.tag=0;_tmp4B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka));({void*_tmp4B6[1]={& _tmp4B8};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4B7="kind_to_opt: bad kind %s\n";_tag_dyneither(_tmp4B7,sizeof(char),26);}),_tag_dyneither(_tmp4B6,sizeof(void*),1));});});}
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
struct Cyc_Absyn_Kind*_tmp4BB=ka;enum Cyc_Absyn_KindQual _tmp4C4;enum Cyc_Absyn_AliasQual _tmp4C3;_LL489: _tmp4C4=_tmp4BB->kind;_tmp4C3=_tmp4BB->aliasqual;_LL48A:;
{enum Cyc_Absyn_AliasQual _tmp4BC=_tmp4C3;switch(_tmp4BC){case Cyc_Absyn_Aliasable: _LL48C: _LL48D: {
# 2416
enum Cyc_Absyn_KindQual _tmp4BD=_tmp4C4;switch(_tmp4BD){case Cyc_Absyn_AnyKind: _LL493: _LL494:
 return& ab;case Cyc_Absyn_MemKind: _LL495: _LL496:
 return& mb;case Cyc_Absyn_BoxKind: _LL497: _LL498:
 return& bb;case Cyc_Absyn_RgnKind: _LL499: _LL49A:
 return& rb;case Cyc_Absyn_EffKind: _LL49B: _LL49C:
 return& eb;default: _LL49D: _LL49E:
 return& ib;}_LL492:;}case Cyc_Absyn_Unique: _LL48E: _LL48F:
# 2425
{enum Cyc_Absyn_KindQual _tmp4BE=_tmp4C4;switch(_tmp4BE){case Cyc_Absyn_AnyKind: _LL4A0: _LL4A1:
 return& uab;case Cyc_Absyn_MemKind: _LL4A2: _LL4A3:
 return& umb;case Cyc_Absyn_BoxKind: _LL4A4: _LL4A5:
 return& ubb;case Cyc_Absyn_RgnKind: _LL4A6: _LL4A7:
 return& urb;default: _LL4A8: _LL4A9:
 goto _LL49F;}_LL49F:;}
# 2432
goto _LL48B;default: _LL490: _LL491:
# 2434
{enum Cyc_Absyn_KindQual _tmp4BF=_tmp4C4;switch(_tmp4BF){case Cyc_Absyn_AnyKind: _LL4AB: _LL4AC:
 return& tab;case Cyc_Absyn_MemKind: _LL4AD: _LL4AE:
 return& tmb;case Cyc_Absyn_BoxKind: _LL4AF: _LL4B0:
 return& tbb;case Cyc_Absyn_RgnKind: _LL4B1: _LL4B2:
 return& trb;default: _LL4B3: _LL4B4:
 goto _LL4AA;}_LL4AA:;}
# 2441
goto _LL48B;}_LL48B:;}
# 2443
({struct Cyc_String_pa_PrintArg_struct _tmp4C2;_tmp4C2.tag=0;_tmp4C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka));({void*_tmp4C0[1]={& _tmp4C2};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4C1="kind_to_b: bad kind %s\n";_tag_dyneither(_tmp4C1,sizeof(char),24);}),_tag_dyneither(_tmp4C0,sizeof(void*),1));});});}
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
{void*_tmp4E4=Cyc_Tcutil_compress(e_typ);void*_tmp4E5=_tmp4E4;void*_tmp4F3;struct Cyc_Absyn_Tqual _tmp4F2;void*_tmp4F1;union Cyc_Absyn_Constraint*_tmp4F0;union Cyc_Absyn_Constraint*_tmp4EF;union Cyc_Absyn_Constraint*_tmp4EE;struct Cyc_Absyn_PtrLoc*_tmp4ED;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E5)->tag == 5){_LL4BF: _tmp4F3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E5)->f1).elt_typ;_tmp4F2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E5)->f1).elt_tq;_tmp4F1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E5)->f1).ptr_atts).rgn;_tmp4F0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E5)->f1).ptr_atts).nullable;_tmp4EF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E5)->f1).ptr_atts).bounds;_tmp4EE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E5)->f1).ptr_atts).zero_term;_tmp4ED=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E5)->f1).ptr_atts).ptrloc;_LL4C0:
# 2509
{void*_tmp4E6=Cyc_Tcutil_compress(_tmp4F1);void*_tmp4E7=_tmp4E6;void**_tmp4EC;struct Cyc_Core_Opt*_tmp4EB;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4E7)->tag == 1){_LL4C4: _tmp4EC=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4E7)->f2;_tmp4EB=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4E7)->f4;_LL4C5: {
# 2511
struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4E8=({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4E9=_cycalloc(sizeof(*_tmp4E9));_tmp4E9[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4EA;_tmp4EA.tag=2;_tmp4EA.f1=tv;_tmp4EA;});_tmp4E9;});
*_tmp4EC=(void*)_tmp4E8;
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
void*_tmp4FE=Cyc_Tcutil_compress(wants_typ);void*_tmp4FF=_tmp4FE;void*_tmp50A;switch(*((int*)_tmp4FF)){case 5: _LL4C9: _tmp50A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FF)->f1).ptr_atts).rgn;_LL4CA: {
# 2541
void*_tmp500=Cyc_Tcutil_compress(_tmp50A);void*_tmp501=_tmp500;if(((struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp501)->tag == 20){_LL4D0: _LL4D1: {
int _tmp502=0;_npop_handler(0);return _tmp502;}}else{_LL4D2: _LL4D3: {
# 2544
struct Cyc_Absyn_Kind*_tmp503=Cyc_Tcutil_typ_kind(_tmp50A);
int _tmp504=_tmp503->kind == Cyc_Absyn_RgnKind  && _tmp503->aliasqual == Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp504;}}_LL4CF:;}case 17: _LL4CB: _LL4CC:
# 2548
({struct Cyc_String_pa_PrintArg_struct _tmp508;_tmp508.tag=0;_tmp508.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(loc));({struct Cyc_String_pa_PrintArg_struct _tmp507;_tmp507.tag=0;_tmp507.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_compress(wants_typ)));({void*_tmp505[2]={& _tmp507,& _tmp508};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp506="got typedef %s in can_insert_alias at %s\n";_tag_dyneither(_tmp506,sizeof(char),42);}),_tag_dyneither(_tmp505,sizeof(void*),2));});});});default: _LL4CD: _LL4CE: {
int _tmp509=0;_npop_handler(0);return _tmp509;}}_LL4C8:;}{
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
struct _tuple23 _tmp51B=*env;struct _tuple23 _tmp51C=_tmp51B;struct Cyc_List_List*_tmp524;struct _RegionHandle*_tmp523;struct Cyc_Tcenv_Tenv*_tmp522;int _tmp521;_LL4DC: _tmp524=_tmp51C.f1;_tmp523=_tmp51C.f2;_tmp522=_tmp51C.f3;_tmp521=_tmp51C.f4;_LL4DD:;{
# 2637
void*_tmp51D=_tmp524 == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp523,_tmp524,x->type);
struct Cyc_List_List*_tmp51E=Cyc_Tcutil_flatten_typ(_tmp523,_tmp521,_tmp522,_tmp51D);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp51E)== 1)
return({struct Cyc_List_List*_tmp51F=_region_malloc(_tmp523,sizeof(*_tmp51F));_tmp51F->hd=({struct _tuple12*_tmp520=_region_malloc(_tmp523,sizeof(*_tmp520));_tmp520->f1=x->tq;_tmp520->f2=_tmp51D;_tmp520;});_tmp51F->tl=0;_tmp51F;});else{
return _tmp51E;}};}struct _tuple24{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2643
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple24*env,struct _tuple12*x){
# 2645
struct _tuple24 _tmp525=*env;struct _tuple24 _tmp526=_tmp525;struct _RegionHandle*_tmp530;struct Cyc_Tcenv_Tenv*_tmp52F;int _tmp52E;_LL4DF: _tmp530=_tmp526.f1;_tmp52F=_tmp526.f2;_tmp52E=_tmp526.f3;_LL4E0:;{
struct _tuple12 _tmp527=*x;struct _tuple12 _tmp528=_tmp527;struct Cyc_Absyn_Tqual _tmp52D;void*_tmp52C;_LL4E2: _tmp52D=_tmp528.f1;_tmp52C=_tmp528.f2;_LL4E3:;{
struct Cyc_List_List*_tmp529=Cyc_Tcutil_flatten_typ(_tmp530,_tmp52E,_tmp52F,_tmp52C);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp529)== 1)
return({struct Cyc_List_List*_tmp52A=_region_malloc(_tmp530,sizeof(*_tmp52A));_tmp52A->hd=({struct _tuple12*_tmp52B=_region_malloc(_tmp530,sizeof(*_tmp52B));_tmp52B->f1=_tmp52D;_tmp52B->f2=_tmp52C;_tmp52B;});_tmp52A->tl=0;_tmp52A;});else{
return _tmp529;}};};}
# 2652
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2656
if(flatten){
t1=Cyc_Tcutil_compress(t1);{
void*_tmp531=t1;struct Cyc_List_List*_tmp553;struct Cyc_Absyn_Aggrdecl*_tmp552;struct Cyc_List_List*_tmp551;struct Cyc_List_List*_tmp550;switch(*((int*)_tmp531)){case 0: _LL4E5: _LL4E6:
 return 0;case 10: _LL4E7: _tmp550=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp531)->f1;_LL4E8: {
# 2661
struct _tuple24 _tmp532=({struct _tuple24 _tmp53B;_tmp53B.f1=r;_tmp53B.f2=te;_tmp53B.f3=flatten;_tmp53B;});
# 2663
struct Cyc_List_List*_tmp533=_tmp550;struct _tuple12*_tmp53A;struct Cyc_List_List*_tmp539;if(_tmp533 == 0){_LL4F0: _LL4F1:
 return 0;}else{_LL4F2: _tmp53A=(struct _tuple12*)_tmp533->hd;_tmp539=_tmp533->tl;_LL4F3: {
# 2666
struct Cyc_List_List*_tmp534=Cyc_Tcutil_rcopy_tqt(& _tmp532,_tmp53A);
_tmp532.f3=0;{
struct Cyc_List_List*_tmp535=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple12*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp532,_tmp550);
struct Cyc_List_List*_tmp536=({struct Cyc_List_List*_tmp538=_region_malloc(r,sizeof(*_tmp538));_tmp538->hd=_tmp534;_tmp538->tl=_tmp535;_tmp538;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,({struct Cyc_List_List*_tmp537=_region_malloc(r,sizeof(*_tmp537));_tmp537->hd=_tmp534;_tmp537->tl=_tmp535;_tmp537;}));};}}_LL4EF:;}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp531)->f1).aggr_info).KnownAggr).tag == 2){_LL4E9: _tmp552=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp531)->f1).aggr_info).KnownAggr).val;_tmp551=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp531)->f1).targs;_LL4EA:
# 2674
 if(((_tmp552->kind == Cyc_Absyn_UnionA  || _tmp552->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp552->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp552->impl))->rgn_po != 0)
# 2676
return({struct Cyc_List_List*_tmp53C=_region_malloc(r,sizeof(*_tmp53C));_tmp53C->hd=({struct _tuple12*_tmp53D=_region_malloc(r,sizeof(*_tmp53D));_tmp53D->f1=Cyc_Absyn_empty_tqual(0);_tmp53D->f2=t1;_tmp53D;});_tmp53C->tl=0;_tmp53C;});{
struct Cyc_List_List*_tmp53E=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp552->tvs,_tmp551);
struct _tuple23 env=({struct _tuple23 _tmp547;_tmp547.f1=_tmp53E;_tmp547.f2=r;_tmp547.f3=te;_tmp547.f4=flatten;_tmp547;});
struct Cyc_List_List*_tmp53F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp552->impl))->fields;struct Cyc_List_List*_tmp540=_tmp53F;struct Cyc_Absyn_Aggrfield*_tmp546;struct Cyc_List_List*_tmp545;if(_tmp540 == 0){_LL4F5: _LL4F6:
 return 0;}else{_LL4F7: _tmp546=(struct Cyc_Absyn_Aggrfield*)_tmp540->hd;_tmp545=_tmp540->tl;_LL4F8: {
# 2682
struct Cyc_List_List*_tmp541=Cyc_Tcutil_flatten_typ_f(& env,_tmp546);
env.f4=0;{
struct Cyc_List_List*_tmp542=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp545);
struct Cyc_List_List*_tmp543=({struct Cyc_List_List*_tmp544=_region_malloc(r,sizeof(*_tmp544));_tmp544->hd=_tmp541;_tmp544->tl=_tmp542;_tmp544;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp543);};}}_LL4F4:;};}else{goto _LL4ED;}case 12: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp531)->f1 == Cyc_Absyn_StructA){_LL4EB: _tmp553=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp531)->f2;_LL4EC: {
# 2689
struct _tuple23 env=({struct _tuple23 _tmp54F;_tmp54F.f1=0;_tmp54F.f2=r;_tmp54F.f3=te;_tmp54F.f4=flatten;_tmp54F;});
struct Cyc_List_List*_tmp548=_tmp553;struct Cyc_Absyn_Aggrfield*_tmp54E;struct Cyc_List_List*_tmp54D;if(_tmp548 == 0){_LL4FA: _LL4FB:
 return 0;}else{_LL4FC: _tmp54E=(struct Cyc_Absyn_Aggrfield*)_tmp548->hd;_tmp54D=_tmp548->tl;_LL4FD: {
# 2693
struct Cyc_List_List*_tmp549=Cyc_Tcutil_flatten_typ_f(& env,_tmp54E);
env.f4=0;{
struct Cyc_List_List*_tmp54A=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp54D);
struct Cyc_List_List*_tmp54B=({struct Cyc_List_List*_tmp54C=_region_malloc(r,sizeof(*_tmp54C));_tmp54C->hd=_tmp549;_tmp54C->tl=_tmp54A;_tmp54C;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp54B);};}}_LL4F9:;}}else{goto _LL4ED;}default: _LL4ED: _LL4EE:
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
struct _tuple0 _tmp558=({struct _tuple0 _tmp55C;_tmp55C.f1=Cyc_Tcutil_compress(t1);_tmp55C.f2=Cyc_Tcutil_compress(t2);_tmp55C;});struct _tuple0 _tmp559=_tmp558;enum Cyc_Absyn_Size_of _tmp55B;enum Cyc_Absyn_Size_of _tmp55A;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp559.f1)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp559.f2)->tag == 6){_LL508: _tmp55B=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp559.f1)->f2;_tmp55A=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp559.f2)->f2;_LL509:
# 2726
 return(_tmp55B == _tmp55A  || _tmp55B == Cyc_Absyn_Int_sz  && _tmp55A == Cyc_Absyn_Long_sz) || 
_tmp55B == Cyc_Absyn_Long_sz  && _tmp55A == Cyc_Absyn_Int_sz;}else{goto _LL50A;}}else{_LL50A: _LL50B:
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
struct _tuple0 _tmp55D=({struct _tuple0 _tmp593;_tmp593.f1=t1;_tmp593.f2=t2;_tmp593;});struct _tuple0 _tmp55E=_tmp55D;struct Cyc_Absyn_FnInfo _tmp592;struct Cyc_Absyn_FnInfo _tmp591;struct Cyc_Absyn_Datatypedecl*_tmp590;struct Cyc_Absyn_Datatypefield*_tmp58F;struct Cyc_List_List*_tmp58E;struct Cyc_Absyn_Datatypedecl*_tmp58D;struct Cyc_List_List*_tmp58C;void*_tmp58B;struct Cyc_Absyn_Tqual _tmp58A;void*_tmp589;union Cyc_Absyn_Constraint*_tmp588;union Cyc_Absyn_Constraint*_tmp587;union Cyc_Absyn_Constraint*_tmp586;void*_tmp585;struct Cyc_Absyn_Tqual _tmp584;void*_tmp583;union Cyc_Absyn_Constraint*_tmp582;union Cyc_Absyn_Constraint*_tmp581;union Cyc_Absyn_Constraint*_tmp580;switch(*((int*)_tmp55E.f1)){case 5: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55E.f2)->tag == 5){_LL50D: _tmp58B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55E.f1)->f1).elt_typ;_tmp58A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55E.f1)->f1).elt_tq;_tmp589=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55E.f1)->f1).ptr_atts).rgn;_tmp588=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55E.f1)->f1).ptr_atts).nullable;_tmp587=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55E.f1)->f1).ptr_atts).bounds;_tmp586=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55E.f1)->f1).ptr_atts).zero_term;_tmp585=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55E.f2)->f1).elt_typ;_tmp584=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55E.f2)->f1).elt_tq;_tmp583=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55E.f2)->f1).ptr_atts).rgn;_tmp582=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55E.f2)->f1).ptr_atts).nullable;_tmp581=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55E.f2)->f1).ptr_atts).bounds;_tmp580=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55E.f2)->f1).ptr_atts).zero_term;_LL50E:
# 2750
 if(_tmp58A.real_const  && !_tmp584.real_const)
return 0;
# 2753
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp588,_tmp582) && 
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp588)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp582))
return 0;
# 2757
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp586,_tmp580) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp586)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp580))
return 0;
# 2761
if((!Cyc_Tcutil_unify(_tmp589,_tmp583) && !Cyc_Tcenv_region_outlives(te,_tmp589,_tmp583)) && !
Cyc_Tcutil_subtype(te,assume,_tmp589,_tmp583))
return 0;
# 2765
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp587,_tmp581)){
struct _tuple0 _tmp55F=({struct _tuple0 _tmp563;_tmp563.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp587);_tmp563.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp581);_tmp563;});struct _tuple0 _tmp560=_tmp55F;struct Cyc_Absyn_Exp*_tmp562;struct Cyc_Absyn_Exp*_tmp561;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp560.f1)->tag == 0){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp560.f2)->tag == 0){_LL516: _LL517:
 goto _LL515;}else{goto _LL51A;}}else{if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp560.f2)->tag == 1){_LL518: _tmp562=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp560.f1)->f1;_tmp561=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp560.f2)->f1;_LL519:
# 2770
 if(!Cyc_Evexp_lte_const_exp(_tmp561,_tmp562))
return 0;
goto _LL515;}else{_LL51A: _LL51B:
 return 0;}}_LL515:;}
# 2778
if(!_tmp584.real_const  && _tmp58A.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp585)))
return 0;}{
# 2784
int _tmp564=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp581,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp580);
# 2790
return(_tmp564  && Cyc_Tcutil_ptrsubtype(te,({struct Cyc_List_List*_tmp565=_cycalloc(sizeof(*_tmp565));_tmp565->hd=({struct _tuple0*_tmp566=_cycalloc(sizeof(*_tmp566));_tmp566->f1=t1;_tmp566->f2=t2;_tmp566;});_tmp565->tl=assume;_tmp565;}),_tmp58B,_tmp585) || Cyc_Tcutil_unify(_tmp58B,_tmp585)) || Cyc_Tcutil_isomorphic(_tmp58B,_tmp585);};}else{goto _LL513;}case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp55E.f1)->f1).field_info).KnownDatatypefield).tag == 2){if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp55E.f2)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp55E.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL50F: _tmp590=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp55E.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp58F=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp55E.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp58E=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp55E.f1)->f1).targs;_tmp58D=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp55E.f2)->f1).datatype_info).KnownDatatype).val;_tmp58C=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp55E.f2)->f1).targs;_LL510:
# 2796
 if(_tmp590 != _tmp58D  && Cyc_Absyn_qvar_cmp(_tmp590->name,_tmp58D->name)!= 0)return 0;
# 2798
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp58E)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp58C))return 0;
for(0;_tmp58E != 0;(_tmp58E=_tmp58E->tl,_tmp58C=_tmp58C->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp58E->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp58C))->hd))return 0;}
return 1;}else{goto _LL513;}}else{goto _LL513;}}else{goto _LL513;}case 9: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp55E.f2)->tag == 9){_LL511: _tmp592=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp55E.f1)->f1;_tmp591=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp55E.f2)->f1;_LL512:
# 2805
 if(_tmp592.tvars != 0  || _tmp591.tvars != 0){
struct Cyc_List_List*_tmp567=_tmp592.tvars;
struct Cyc_List_List*_tmp568=_tmp591.tvars;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp567)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp568))return 0;{
struct _RegionHandle _tmp569=_new_region("r");struct _RegionHandle*r=& _tmp569;_push_region(r);
{struct Cyc_List_List*inst=0;
while(_tmp567 != 0){
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp567->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp568))->hd)->kind)){int _tmp56A=0;_npop_handler(0);return _tmp56A;}
inst=({struct Cyc_List_List*_tmp56B=_region_malloc(r,sizeof(*_tmp56B));_tmp56B->hd=({struct _tuple16*_tmp56C=_region_malloc(r,sizeof(*_tmp56C));_tmp56C->f1=(struct Cyc_Absyn_Tvar*)_tmp568->hd;_tmp56C->f2=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp56D=_cycalloc(sizeof(*_tmp56D));_tmp56D[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp56E;_tmp56E.tag=2;_tmp56E.f1=(struct Cyc_Absyn_Tvar*)_tmp567->hd;_tmp56E;});_tmp56D;});_tmp56C;});_tmp56B->tl=inst;_tmp56B;});
_tmp567=_tmp567->tl;
_tmp568=_tmp568->tl;}
# 2817
if(inst != 0){
_tmp592.tvars=0;
_tmp591.tvars=0;{
int _tmp573=Cyc_Tcutil_subtype(te,assume,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp56F=_cycalloc(sizeof(*_tmp56F));_tmp56F[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp570;_tmp570.tag=9;_tmp570.f1=_tmp592;_tmp570;});_tmp56F;}),(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp571=_cycalloc(sizeof(*_tmp571));_tmp571[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp572;_tmp572.tag=9;_tmp572.f1=_tmp591;_tmp572;});_tmp571;}));_npop_handler(0);return _tmp573;};}}
# 2810
;_pop_region(r);};}
# 2825
if(!Cyc_Tcutil_subtype(te,assume,_tmp592.ret_typ,_tmp591.ret_typ))return 0;{
struct Cyc_List_List*_tmp574=_tmp592.args;
struct Cyc_List_List*_tmp575=_tmp591.args;
# 2830
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp574)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp575))return 0;
# 2832
for(0;_tmp574 != 0;(_tmp574=_tmp574->tl,_tmp575=_tmp575->tl)){
struct _tuple10 _tmp576=*((struct _tuple10*)_tmp574->hd);struct _tuple10 _tmp577=_tmp576;struct Cyc_Absyn_Tqual _tmp57D;void*_tmp57C;_LL51D: _tmp57D=_tmp577.f2;_tmp57C=_tmp577.f3;_LL51E:;{
struct _tuple10 _tmp578=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp575))->hd);struct _tuple10 _tmp579=_tmp578;struct Cyc_Absyn_Tqual _tmp57B;void*_tmp57A;_LL520: _tmp57B=_tmp579.f2;_tmp57A=_tmp579.f3;_LL521:;
# 2836
if(_tmp57B.real_const  && !_tmp57D.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp57A,_tmp57C))
return 0;};}
# 2840
if(_tmp592.c_varargs != _tmp591.c_varargs)return 0;
if(_tmp592.cyc_varargs != 0  && _tmp591.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp57E=*_tmp592.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp57F=*_tmp591.cyc_varargs;
# 2845
if((_tmp57F.tq).real_const  && !(_tmp57E.tq).real_const  || !
Cyc_Tcutil_subtype(te,assume,_tmp57F.type,_tmp57E.type))
return 0;}else{
if(_tmp592.cyc_varargs != 0  || _tmp591.cyc_varargs != 0)return 0;}
# 2850
if(!Cyc_Tcutil_subset_effect(1,(void*)_check_null(_tmp592.effect),(void*)_check_null(_tmp591.effect)))return 0;
# 2852
if(!Cyc_Tcutil_sub_rgnpo(_tmp592.rgn_po,_tmp591.rgn_po))return 0;
# 2854
if(!Cyc_Tcutil_sub_attributes(_tmp592.attributes,_tmp591.attributes))return 0;
# 2856
if(!Cyc_Tcutil_check_logical_implication(_tmp591.requires_relns,_tmp592.requires_relns))
return 0;
# 2859
if(!Cyc_Tcutil_check_logical_implication(_tmp592.ensures_relns,_tmp591.ensures_relns))
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
struct _tuple12*_tmp596=(struct _tuple12*)tqs1->hd;struct _tuple12*_tmp597=_tmp596;struct Cyc_Absyn_Tqual _tmp59F;void*_tmp59E;_LL523: _tmp59F=_tmp597->f1;_tmp59E=_tmp597->f2;_LL524:;{
struct _tuple12*_tmp598=(struct _tuple12*)tqs2->hd;struct _tuple12*_tmp599=_tmp598;struct Cyc_Absyn_Tqual _tmp59D;void*_tmp59C;_LL526: _tmp59D=_tmp599->f1;_tmp59C=_tmp599->f2;_LL527:;
# 2884
if(_tmp59F.real_const  && !_tmp59D.real_const){int _tmp59A=0;_npop_handler(0);return _tmp59A;}
# 2886
if((_tmp59D.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp59C))) && 
Cyc_Tcutil_subtype(te,assume,_tmp59E,_tmp59C))
# 2889
continue;
# 2891
if(Cyc_Tcutil_unify(_tmp59E,_tmp59C))
# 2893
continue;
# 2895
if(Cyc_Tcutil_isomorphic(_tmp59E,_tmp59C))
# 2897
continue;{
# 2900
int _tmp59B=0;_npop_handler(0);return _tmp59B;};};};}{
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
void*_tmp5A4=t1;void*_tmp5CC;struct Cyc_Absyn_Enumdecl*_tmp5CB;void*_tmp5CA;struct Cyc_Absyn_Tqual _tmp5C9;struct Cyc_Absyn_Exp*_tmp5C8;union Cyc_Absyn_Constraint*_tmp5C7;void*_tmp5C6;struct Cyc_Absyn_Tqual _tmp5C5;void*_tmp5C4;union Cyc_Absyn_Constraint*_tmp5C3;union Cyc_Absyn_Constraint*_tmp5C2;union Cyc_Absyn_Constraint*_tmp5C1;switch(*((int*)_tmp5A4)){case 5: _LL535: _tmp5C6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A4)->f1).elt_typ;_tmp5C5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A4)->f1).elt_tq;_tmp5C4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A4)->f1).ptr_atts).rgn;_tmp5C3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A4)->f1).ptr_atts).nullable;_tmp5C2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A4)->f1).ptr_atts).bounds;_tmp5C1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A4)->f1).ptr_atts).zero_term;_LL536:
# 2939
{void*_tmp5A5=t2;void*_tmp5B7;struct Cyc_Absyn_Tqual _tmp5B6;void*_tmp5B5;union Cyc_Absyn_Constraint*_tmp5B4;union Cyc_Absyn_Constraint*_tmp5B3;union Cyc_Absyn_Constraint*_tmp5B2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A5)->tag == 5){_LL544: _tmp5B7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A5)->f1).elt_typ;_tmp5B6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A5)->f1).elt_tq;_tmp5B5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A5)->f1).ptr_atts).rgn;_tmp5B4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A5)->f1).ptr_atts).nullable;_tmp5B3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A5)->f1).ptr_atts).bounds;_tmp5B2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A5)->f1).ptr_atts).zero_term;_LL545: {
# 2943
enum Cyc_Absyn_Coercion coercion=Cyc_Absyn_Other_coercion;
struct Cyc_List_List*_tmp5A6=({struct Cyc_List_List*_tmp5B0=_cycalloc(sizeof(*_tmp5B0));_tmp5B0->hd=({struct _tuple0*_tmp5B1=_cycalloc(sizeof(*_tmp5B1));_tmp5B1->f1=t1;_tmp5B1->f2=t2;_tmp5B1;});_tmp5B0->tl=0;_tmp5B0;});
int _tmp5A7=_tmp5B6.real_const  || !_tmp5C5.real_const;
# 2957 "tcutil.cyc"
int _tmp5A8=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp5B3,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp5B2);
# 2961
int _tmp5A9=_tmp5A7  && (
((_tmp5A8  && Cyc_Tcutil_ptrsubtype(te,_tmp5A6,_tmp5C6,_tmp5B7) || 
Cyc_Tcutil_unify(_tmp5C6,_tmp5B7)) || Cyc_Tcutil_isomorphic(_tmp5C6,_tmp5B7)) || Cyc_Tcutil_unify(_tmp5B7,(void*)& Cyc_Absyn_VoidType_val));
# 2965
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp5C1,_tmp5B2) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp5B2);
# 2969
int _tmp5AA=_tmp5A9?0:((Cyc_Tcutil_bits_only(_tmp5C6) && Cyc_Tcutil_is_char_type(_tmp5B7)) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5B2)) && (
_tmp5B6.real_const  || !_tmp5C5.real_const);
int bounds_ok=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp5C2,_tmp5B3);
if(!bounds_ok  && !_tmp5AA){
struct _tuple0 _tmp5AB=({struct _tuple0 _tmp5AF;_tmp5AF.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp5C2);_tmp5AF.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp5B3);_tmp5AF;});struct _tuple0 _tmp5AC=_tmp5AB;struct Cyc_Absyn_Exp*_tmp5AE;struct Cyc_Absyn_Exp*_tmp5AD;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5AC.f1)->tag == 1){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5AC.f2)->tag == 1){_LL549: _tmp5AE=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5AC.f1)->f1;_tmp5AD=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5AC.f2)->f1;_LL54A:
# 2979
 if(Cyc_Evexp_lte_const_exp(_tmp5AD,_tmp5AE))
bounds_ok=1;
goto _LL548;}else{goto _LL54B;}}else{_LL54B: _LL54C:
# 2984
 bounds_ok=1;goto _LL548;}_LL548:;}
# 2986
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5C3) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5B4))
coercion=Cyc_Absyn_NonNull_to_Null;
# 2991
if(((bounds_ok  && zeroterm_ok) && (_tmp5A9  || _tmp5AA)) && (
Cyc_Tcutil_unify(_tmp5C4,_tmp5B5) || Cyc_Tcenv_region_outlives(te,_tmp5C4,_tmp5B5)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};}}else{_LL546: _LL547:
 goto _LL543;}_LL543:;}
# 2997
return Cyc_Absyn_Unknown_coercion;case 8: _LL537: _tmp5CA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A4)->f1).elt_type;_tmp5C9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A4)->f1).tq;_tmp5C8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A4)->f1).num_elts;_tmp5C7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A4)->f1).zero_term;_LL538:
# 2999
{void*_tmp5B8=t2;void*_tmp5BC;struct Cyc_Absyn_Tqual _tmp5BB;struct Cyc_Absyn_Exp*_tmp5BA;union Cyc_Absyn_Constraint*_tmp5B9;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5B8)->tag == 8){_LL54E: _tmp5BC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5B8)->f1).elt_type;_tmp5BB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5B8)->f1).tq;_tmp5BA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5B8)->f1).num_elts;_tmp5B9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5B8)->f1).zero_term;_LL54F: {
# 3001
int okay;
okay=
(((_tmp5C8 != 0  && _tmp5BA != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp5C7,_tmp5B9)) && 
Cyc_Evexp_lte_const_exp(_tmp5BA,_tmp5C8)) && 
Cyc_Evexp_lte_const_exp(_tmp5C8,_tmp5BA);
return
# 3008
(okay  && Cyc_Tcutil_unify(_tmp5CA,_tmp5BC)) && (!_tmp5C9.real_const  || _tmp5BB.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}}else{_LL550: _LL551:
# 3010
 return Cyc_Absyn_Unknown_coercion;}_LL54D:;}
# 3012
return Cyc_Absyn_Unknown_coercion;case 13: _LL539: _tmp5CB=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5A4)->f2;_LL53A:
# 3016
{void*_tmp5BD=t2;struct Cyc_Absyn_Enumdecl*_tmp5BE;if(((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5BD)->tag == 13){_LL553: _tmp5BE=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5BD)->f2;_LL554:
# 3018
 if((_tmp5CB->fields != 0  && _tmp5BE->fields != 0) && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5CB->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5BE->fields))->v))
return Cyc_Absyn_Other_coercion;
goto _LL552;}else{_LL555: _LL556:
 goto _LL552;}_LL552:;}
# 3024
goto _LL53C;case 6: _LL53B: _LL53C:
 goto _LL53E;case 7: _LL53D: _LL53E:
 return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;case 15: _LL53F: _tmp5CC=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5A4)->f1;_LL540:
# 3029
{void*_tmp5BF=t2;void*_tmp5C0;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5BF)->tag == 15){_LL558: _tmp5C0=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5BF)->f1;_LL559:
# 3031
 if(Cyc_Tcenv_region_outlives(te,_tmp5CC,_tmp5C0))return Cyc_Absyn_No_coercion;
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
struct _tuple0 _tmp5D6=({struct _tuple0 _tmp5DA;_tmp5DA.f1=t1;_tmp5DA.f2=t2;_tmp5DA;});struct _tuple0 _tmp5D7=_tmp5D6;int _tmp5D9;int _tmp5D8;if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D7.f1)->tag == 7){if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D7.f2)->tag == 7){_LL574: _tmp5D9=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D7.f1)->f1;_tmp5D8=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D7.f2)->f1;_LL575:
# 3079
 if(_tmp5D9 != 0  && _tmp5D9 != 1)return t1;else{
if(_tmp5D8 != 0  && _tmp5D8 != 1)return t2;else{
if(_tmp5D9 >= _tmp5D8)return t1;else{
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
struct _tuple0 _tmp5DB=({struct _tuple0 _tmp5F1;_tmp5F1.f1=c1;_tmp5F1.f2=c2;_tmp5F1;});struct _tuple0 _tmp5DC=_tmp5DB;struct Cyc_Core_Opt**_tmp5F0;struct Cyc_Absyn_Kind*_tmp5EF;struct Cyc_Core_Opt**_tmp5EE;struct Cyc_Absyn_Kind*_tmp5ED;struct Cyc_Core_Opt**_tmp5EC;struct Cyc_Absyn_Kind*_tmp5EB;struct Cyc_Absyn_Kind*_tmp5EA;struct Cyc_Core_Opt**_tmp5E9;struct Cyc_Core_Opt**_tmp5E8;struct Cyc_Absyn_Kind*_tmp5E7;struct Cyc_Core_Opt**_tmp5E6;struct Cyc_Absyn_Kind*_tmp5E5;struct Cyc_Absyn_Kind*_tmp5E4;struct Cyc_Absyn_Kind*_tmp5E3;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5DC.f1)->tag == 0)switch(*((int*)_tmp5DC.f2)){case 0: _LL591: _tmp5E4=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5DC.f1)->f1;_tmp5E3=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5DC.f2)->f1;_LL592:
 return _tmp5E4 == _tmp5E3;case 1: goto _LL593;default: _LL599: _tmp5E7=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5DC.f1)->f1;_tmp5E6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5DC.f2)->f1;_tmp5E5=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5DC.f2)->f2;_LL59A:
# 3120
 if(Cyc_Tcutil_kind_leq(_tmp5E7,_tmp5E5)){
*_tmp5E6=({struct Cyc_Core_Opt*_tmp5E0=_cycalloc(sizeof(*_tmp5E0));_tmp5E0->v=c1;_tmp5E0;});return 1;}else{
return 0;}}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5DC.f2)->tag == 1){_LL593: _tmp5E8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5DC.f2)->f1;_LL594:
# 3113
*_tmp5E8=({struct Cyc_Core_Opt*_tmp5DD=_cycalloc(sizeof(*_tmp5DD));_tmp5DD->v=c1;_tmp5DD;});return 1;}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5DC.f1)->tag == 1){_LL595: _tmp5E9=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5DC.f1)->f1;_LL596:
*_tmp5E9=({struct Cyc_Core_Opt*_tmp5DE=_cycalloc(sizeof(*_tmp5DE));_tmp5DE->v=c2;_tmp5DE;});return 1;}else{if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5DC.f2)->tag == 0){_LL597: _tmp5EC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5DC.f1)->f1;_tmp5EB=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5DC.f1)->f2;_tmp5EA=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5DC.f2)->f1;_LL598:
# 3116
 if(Cyc_Tcutil_kind_leq(_tmp5EA,_tmp5EB)){
*_tmp5EC=({struct Cyc_Core_Opt*_tmp5DF=_cycalloc(sizeof(*_tmp5DF));_tmp5DF->v=c2;_tmp5DF;});return 1;}else{
return 0;}}else{_LL59B: _tmp5F0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5DC.f1)->f1;_tmp5EF=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5DC.f1)->f2;_tmp5EE=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5DC.f2)->f1;_tmp5ED=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5DC.f2)->f2;_LL59C:
# 3124
 if(Cyc_Tcutil_kind_leq(_tmp5EF,_tmp5ED)){
*_tmp5EE=({struct Cyc_Core_Opt*_tmp5E1=_cycalloc(sizeof(*_tmp5E1));_tmp5E1->v=c1;_tmp5E1;});return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp5ED,_tmp5EF)){
*_tmp5F0=({struct Cyc_Core_Opt*_tmp5E2=_cycalloc(sizeof(*_tmp5E2));_tmp5E2->v=c2;_tmp5E2;});return 1;}else{
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
union Cyc_Relations_RelnOp _tmp607=*rop;union Cyc_Relations_RelnOp _tmp608=_tmp607;struct Cyc_Absyn_Vardecl*_tmp61B;struct Cyc_Absyn_Vardecl*_tmp61A;switch((_tmp608.RNumelts).tag){case 2: _LL59E: _tmp61A=(_tmp608.RVar).val;_LL59F: {
# 3194
struct _tuple2 _tmp609=*_tmp61A->name;struct _tuple2 _tmp60A=_tmp609;union Cyc_Absyn_Nmspace _tmp611;struct _dyneither_ptr*_tmp610;_LL5A5: _tmp611=_tmp60A.f1;_tmp610=_tmp60A.f2;_LL5A6:;
if(!((int)((_tmp611.Loc_n).tag == 4)))goto _LL59D;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp610,({const char*_tmp60B="return_value";_tag_dyneither(_tmp60B,sizeof(char),13);}))== 0){
*rop=Cyc_Relations_RReturn();
goto _LL59D;}{
# 3200
unsigned int c=0;
{struct Cyc_List_List*_tmp60C=args;for(0;_tmp60C != 0;(_tmp60C=_tmp60C->tl,c ++)){
struct _tuple10*_tmp60D=(struct _tuple10*)_tmp60C->hd;struct _tuple10*_tmp60E=_tmp60D;struct _dyneither_ptr*_tmp60F;_LL5A8: _tmp60F=_tmp60E->f1;_LL5A9:;
if(_tmp60F != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp610,(struct _dyneither_ptr)*_tmp60F)== 0){
*rop=Cyc_Relations_RParam(c);
break;}}}}
# 3210
goto _LL59D;};}case 3: _LL5A0: _tmp61B=(_tmp608.RNumelts).val;_LL5A1: {
# 3212
struct _tuple2 _tmp612=*_tmp61B->name;struct _tuple2 _tmp613=_tmp612;union Cyc_Absyn_Nmspace _tmp619;struct _dyneither_ptr*_tmp618;_LL5AB: _tmp619=_tmp613.f1;_tmp618=_tmp613.f2;_LL5AC:;
if(!((int)((_tmp619.Loc_n).tag == 4)))goto _LL59D;{
unsigned int c=0;
{struct Cyc_List_List*_tmp614=args;for(0;_tmp614 != 0;(_tmp614=_tmp614->tl,c ++)){
struct _tuple10*_tmp615=(struct _tuple10*)_tmp614->hd;struct _tuple10*_tmp616=_tmp615;struct _dyneither_ptr*_tmp617;_LL5AE: _tmp617=_tmp616->f1;_LL5AF:;
if(_tmp617 != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp618,(struct _dyneither_ptr)*_tmp617)== 0){
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
struct _tuple12*_tmp61D=pr;struct Cyc_Absyn_Tqual _tmp620;void*_tmp61F;_LL5B1: _tmp620=_tmp61D->f1;_tmp61F=_tmp61D->f2;_LL5B2:;
if(_tmp61F == t)return pr;else{
return({struct _tuple12*_tmp61E=_cycalloc(sizeof(*_tmp61E));_tmp61E->f1=_tmp620;_tmp61E->f2=t;_tmp61E;});}}struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 3251
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
# 3253
return({struct _tuple27*_tmp621=_region_malloc(rgn,sizeof(*_tmp621));_tmp621->f1=({struct _tuple26*_tmp622=_region_malloc(rgn,sizeof(*_tmp622));_tmp622->f1=(*y).f1;_tmp622->f2=(*y).f2;_tmp622;});_tmp621->f2=(*y).f3;_tmp621;});}
# 3255
static struct _tuple10*Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){
# 3257
struct _tuple10 _tmp623=*orig_arg;struct _tuple10 _tmp624=_tmp623;struct _dyneither_ptr*_tmp628;struct Cyc_Absyn_Tqual _tmp627;void*_tmp626;_LL5B4: _tmp628=_tmp624.f1;_tmp627=_tmp624.f2;_tmp626=_tmp624.f3;_LL5B5:;
if(t == _tmp626)return orig_arg;
return({struct _tuple10*_tmp625=_cycalloc(sizeof(*_tmp625));_tmp625->f1=_tmp628;_tmp625->f2=_tmp627;_tmp625->f3=t;_tmp625;});}
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
void*_tmp62A=e->r;void*_tmp62B=_tmp62A;void*_tmp673;void*_tmp672;struct Cyc_List_List*_tmp671;struct Cyc_Absyn_Exp*_tmp670;void*_tmp66F;void*_tmp66E;struct Cyc_Absyn_Exp*_tmp66D;int _tmp66C;enum Cyc_Absyn_Coercion _tmp66B;struct Cyc_Absyn_Exp*_tmp66A;struct Cyc_Absyn_Exp*_tmp669;struct Cyc_Absyn_Exp*_tmp668;struct Cyc_Absyn_Exp*_tmp667;struct Cyc_Absyn_Exp*_tmp666;struct Cyc_Absyn_Exp*_tmp665;struct Cyc_Absyn_Exp*_tmp664;struct Cyc_Absyn_Exp*_tmp663;struct Cyc_Absyn_Exp*_tmp662;enum Cyc_Absyn_Primop _tmp661;struct Cyc_List_List*_tmp660;switch(*((int*)_tmp62B)){case 0: _LL5B7: _LL5B8:
 goto _LL5BA;case 31: _LL5B9: _LL5BA:
 goto _LL5BC;case 32: _LL5BB: _LL5BC:
 goto _LL5BE;case 1: _LL5BD: _LL5BE:
 return e;case 2: _LL5BF: _tmp661=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp62B)->f1;_tmp660=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp62B)->f2;_LL5C0:
# 3284
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp660)== 1){
struct Cyc_Absyn_Exp*_tmp62C=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp660))->hd;
struct Cyc_Absyn_Exp*_tmp62D=Cyc_Tcutil_rsubsexp(r,inst,_tmp62C);
if(_tmp62D == _tmp62C)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp62E=_cycalloc(sizeof(*_tmp62E));_tmp62E[0]=({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp62F;_tmp62F.tag=2;_tmp62F.f1=_tmp661;_tmp62F.f2=({struct Cyc_Absyn_Exp*_tmp630[1];_tmp630[0]=_tmp62D;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp630,sizeof(struct Cyc_Absyn_Exp*),1));});_tmp62F;});_tmp62E;}));}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp660)== 2){
struct Cyc_Absyn_Exp*_tmp631=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp660))->hd;
struct Cyc_Absyn_Exp*_tmp632=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp660->tl))->hd;
struct Cyc_Absyn_Exp*_tmp633=Cyc_Tcutil_rsubsexp(r,inst,_tmp631);
struct Cyc_Absyn_Exp*_tmp634=Cyc_Tcutil_rsubsexp(r,inst,_tmp632);
if(_tmp633 == _tmp631  && _tmp634 == _tmp632)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp635=_cycalloc(sizeof(*_tmp635));_tmp635[0]=({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp636;_tmp636.tag=2;_tmp636.f1=_tmp661;_tmp636.f2=({struct Cyc_Absyn_Exp*_tmp637[2];_tmp637[1]=_tmp634;_tmp637[0]=_tmp633;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp637,sizeof(struct Cyc_Absyn_Exp*),2));});_tmp636;});_tmp635;}));}else{
return({void*_tmp638=0;((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp639="primop does not have 1 or 2 args!";_tag_dyneither(_tmp639,sizeof(char),34);}),_tag_dyneither(_tmp638,sizeof(void*),0));});}}case 5: _LL5C1: _tmp664=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp62B)->f1;_tmp663=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp62B)->f2;_tmp662=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp62B)->f3;_LL5C2: {
# 3298
struct Cyc_Absyn_Exp*_tmp63A=Cyc_Tcutil_rsubsexp(r,inst,_tmp664);
struct Cyc_Absyn_Exp*_tmp63B=Cyc_Tcutil_rsubsexp(r,inst,_tmp663);
struct Cyc_Absyn_Exp*_tmp63C=Cyc_Tcutil_rsubsexp(r,inst,_tmp662);
if((_tmp63A == _tmp664  && _tmp63B == _tmp663) && _tmp63C == _tmp662)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp63D=_cycalloc(sizeof(*_tmp63D));_tmp63D[0]=({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp63E;_tmp63E.tag=5;_tmp63E.f1=_tmp63A;_tmp63E.f2=_tmp63B;_tmp63E.f3=_tmp63C;_tmp63E;});_tmp63D;}));}case 6: _LL5C3: _tmp666=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp62B)->f1;_tmp665=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp62B)->f2;_LL5C4: {
# 3304
struct Cyc_Absyn_Exp*_tmp63F=Cyc_Tcutil_rsubsexp(r,inst,_tmp666);
struct Cyc_Absyn_Exp*_tmp640=Cyc_Tcutil_rsubsexp(r,inst,_tmp665);
if(_tmp63F == _tmp666  && _tmp640 == _tmp665)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp641=_cycalloc(sizeof(*_tmp641));_tmp641[0]=({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmp642;_tmp642.tag=6;_tmp642.f1=_tmp63F;_tmp642.f2=_tmp640;_tmp642;});_tmp641;}));}case 7: _LL5C5: _tmp668=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp62B)->f1;_tmp667=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp62B)->f2;_LL5C6: {
# 3309
struct Cyc_Absyn_Exp*_tmp643=Cyc_Tcutil_rsubsexp(r,inst,_tmp668);
struct Cyc_Absyn_Exp*_tmp644=Cyc_Tcutil_rsubsexp(r,inst,_tmp667);
if(_tmp643 == _tmp668  && _tmp644 == _tmp667)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp645=_cycalloc(sizeof(*_tmp645));_tmp645[0]=({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmp646;_tmp646.tag=7;_tmp646.f1=_tmp643;_tmp646.f2=_tmp644;_tmp646;});_tmp645;}));}case 8: _LL5C7: _tmp66A=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp62B)->f1;_tmp669=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp62B)->f2;_LL5C8: {
# 3314
struct Cyc_Absyn_Exp*_tmp647=Cyc_Tcutil_rsubsexp(r,inst,_tmp66A);
struct Cyc_Absyn_Exp*_tmp648=Cyc_Tcutil_rsubsexp(r,inst,_tmp669);
if(_tmp647 == _tmp66A  && _tmp648 == _tmp669)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp649=_cycalloc(sizeof(*_tmp649));_tmp649[0]=({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmp64A;_tmp64A.tag=8;_tmp64A.f1=_tmp647;_tmp64A.f2=_tmp648;_tmp64A;});_tmp649;}));}case 13: _LL5C9: _tmp66E=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp62B)->f1;_tmp66D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp62B)->f2;_tmp66C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp62B)->f3;_tmp66B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp62B)->f4;_LL5CA: {
# 3319
struct Cyc_Absyn_Exp*_tmp64B=Cyc_Tcutil_rsubsexp(r,inst,_tmp66D);
void*_tmp64C=Cyc_Tcutil_rsubstitute(r,inst,_tmp66E);
if(_tmp64B == _tmp66D  && _tmp64C == _tmp66E)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp64D=_cycalloc(sizeof(*_tmp64D));_tmp64D[0]=({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp64E;_tmp64E.tag=13;_tmp64E.f1=_tmp64C;_tmp64E.f2=_tmp64B;_tmp64E.f3=_tmp66C;_tmp64E.f4=_tmp66B;_tmp64E;});_tmp64D;}));}case 16: _LL5CB: _tmp66F=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp62B)->f1;_LL5CC: {
# 3324
void*_tmp64F=Cyc_Tcutil_rsubstitute(r,inst,_tmp66F);
if(_tmp64F == _tmp66F)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp650=_cycalloc(sizeof(*_tmp650));_tmp650[0]=({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp651;_tmp651.tag=16;_tmp651.f1=_tmp64F;_tmp651;});_tmp650;}));}case 17: _LL5CD: _tmp670=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp62B)->f1;_LL5CE: {
# 3328
struct Cyc_Absyn_Exp*_tmp652=Cyc_Tcutil_rsubsexp(r,inst,_tmp670);
if(_tmp652 == _tmp670)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp653=_cycalloc(sizeof(*_tmp653));_tmp653[0]=({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmp654;_tmp654.tag=17;_tmp654.f1=_tmp652;_tmp654;});_tmp653;}));}case 18: _LL5CF: _tmp672=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp62B)->f1;_tmp671=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp62B)->f2;_LL5D0: {
# 3332
void*_tmp655=Cyc_Tcutil_rsubstitute(r,inst,_tmp672);
if(_tmp655 == _tmp672)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp656=_cycalloc(sizeof(*_tmp656));_tmp656[0]=({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp657;_tmp657.tag=18;_tmp657.f1=_tmp655;_tmp657.f2=_tmp671;_tmp657;});_tmp656;}));}case 38: _LL5D1: _tmp673=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp62B)->f1;_LL5D2: {
# 3336
void*_tmp658=Cyc_Tcutil_rsubstitute(r,inst,_tmp673);
if(_tmp658 == _tmp673)return e;{
# 3339
void*_tmp659=Cyc_Tcutil_compress(_tmp658);void*_tmp65A=_tmp659;struct Cyc_Absyn_Exp*_tmp65D;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp65A)->tag == 18){_LL5D6: _tmp65D=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp65A)->f1;_LL5D7:
 return _tmp65D;}else{_LL5D8: _LL5D9:
# 3342
 return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp65B=_cycalloc(sizeof(*_tmp65B));_tmp65B[0]=({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp65C;_tmp65C.tag=38;_tmp65C.f1=_tmp658;_tmp65C;});_tmp65B;}));}_LL5D5:;};}default: _LL5D3: _LL5D4:
# 3345
 return({void*_tmp65E=0;((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp65F="non-type-level-expression in Tcutil::rsubsexp";_tag_dyneither(_tmp65F,sizeof(char),46);}),_tag_dyneither(_tmp65E,sizeof(void*),0));});}_LL5B6:;}
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
struct _tuple1 _tmp67E=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct _tuple1 _tmp67F=_tmp67E;struct Cyc_List_List*_tmp683;struct Cyc_List_List*_tmp682;_LL5DB: _tmp683=_tmp67F.f1;_tmp682=_tmp67F.f2;_LL5DC:;{
struct Cyc_List_List*_tmp680=Cyc_Tcutil_substs(rgn,inst,_tmp683);
struct Cyc_List_List*_tmp681=Cyc_Tcutil_substs(rgn,inst,_tmp682);
if(_tmp680 == _tmp683  && _tmp681 == _tmp682)
return rgn_po;else{
# 3391
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp680,_tmp681);}};}
# 3394
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3397
void*_tmp684=Cyc_Tcutil_compress(t);void*_tmp685=_tmp684;struct Cyc_List_List*_tmp716;void*_tmp715;void*_tmp714;struct Cyc_Absyn_Exp*_tmp713;struct Cyc_Absyn_Exp*_tmp712;void*_tmp711;void*_tmp710;void*_tmp70F;void*_tmp70E;void*_tmp70D;enum Cyc_Absyn_AggrKind _tmp70C;struct Cyc_List_List*_tmp70B;struct Cyc_List_List*_tmp70A;struct Cyc_List_List*_tmp709;void*_tmp708;struct Cyc_Absyn_Tqual _tmp707;void*_tmp706;struct Cyc_List_List*_tmp705;int _tmp704;struct Cyc_Absyn_VarargInfo*_tmp703;struct Cyc_List_List*_tmp702;struct Cyc_List_List*_tmp701;struct Cyc_Absyn_Exp*_tmp700;struct Cyc_Absyn_Exp*_tmp6FF;void*_tmp6FE;struct Cyc_Absyn_Tqual _tmp6FD;void*_tmp6FC;union Cyc_Absyn_Constraint*_tmp6FB;union Cyc_Absyn_Constraint*_tmp6FA;union Cyc_Absyn_Constraint*_tmp6F9;void*_tmp6F8;struct Cyc_Absyn_Tqual _tmp6F7;struct Cyc_Absyn_Exp*_tmp6F6;union Cyc_Absyn_Constraint*_tmp6F5;unsigned int _tmp6F4;struct _tuple2*_tmp6F3;struct Cyc_List_List*_tmp6F2;struct Cyc_Absyn_Typedefdecl*_tmp6F1;void*_tmp6F0;union Cyc_Absyn_DatatypeFieldInfoU _tmp6EF;struct Cyc_List_List*_tmp6EE;union Cyc_Absyn_DatatypeInfoU _tmp6ED;struct Cyc_List_List*_tmp6EC;union Cyc_Absyn_AggrInfoU _tmp6EB;struct Cyc_List_List*_tmp6EA;struct Cyc_Absyn_Tvar*_tmp6E9;switch(*((int*)_tmp685)){case 2: _LL5DE: _tmp6E9=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp685)->f1;_LL5DF: {
# 3400
struct _handler_cons _tmp686;_push_handler(& _tmp686);{int _tmp688=0;if(setjmp(_tmp686.handler))_tmp688=1;if(!_tmp688){{void*_tmp689=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp6E9);_npop_handler(0);return _tmp689;};_pop_handler();}else{void*_tmp687=(void*)_exn_thrown;void*_tmp68A=_tmp687;void*_tmp68B;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp68A)->tag == Cyc_Core_Not_found){_LL619: _LL61A:
 return t;}else{_LL61B: _tmp68B=_tmp68A;_LL61C:(int)_rethrow(_tmp68B);}_LL618:;}};}case 11: _LL5E0: _tmp6EB=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp685)->f1).aggr_info;_tmp6EA=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp685)->f1).targs;_LL5E1: {
# 3403
struct Cyc_List_List*_tmp68C=Cyc_Tcutil_substs(rgn,inst,_tmp6EA);
return _tmp68C == _tmp6EA?t:(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp68D=_cycalloc(sizeof(*_tmp68D));_tmp68D[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp68E;_tmp68E.tag=11;_tmp68E.f1=({struct Cyc_Absyn_AggrInfo _tmp68F;_tmp68F.aggr_info=_tmp6EB;_tmp68F.targs=_tmp68C;_tmp68F;});_tmp68E;});_tmp68D;});}case 3: _LL5E2: _tmp6ED=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp685)->f1).datatype_info;_tmp6EC=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp685)->f1).targs;_LL5E3: {
# 3406
struct Cyc_List_List*_tmp690=Cyc_Tcutil_substs(rgn,inst,_tmp6EC);
return _tmp690 == _tmp6EC?t:(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp691=_cycalloc(sizeof(*_tmp691));_tmp691[0]=({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp692;_tmp692.tag=3;_tmp692.f1=({struct Cyc_Absyn_DatatypeInfo _tmp693;_tmp693.datatype_info=_tmp6ED;_tmp693.targs=_tmp690;_tmp693;});_tmp692;});_tmp691;});}case 4: _LL5E4: _tmp6EF=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp685)->f1).field_info;_tmp6EE=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp685)->f1).targs;_LL5E5: {
# 3410
struct Cyc_List_List*_tmp694=Cyc_Tcutil_substs(rgn,inst,_tmp6EE);
return _tmp694 == _tmp6EE?t:(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp695=_cycalloc(sizeof(*_tmp695));_tmp695[0]=({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp696;_tmp696.tag=4;_tmp696.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmp697;_tmp697.field_info=_tmp6EF;_tmp697.targs=_tmp694;_tmp697;});_tmp696;});_tmp695;});}case 17: _LL5E6: _tmp6F3=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp685)->f1;_tmp6F2=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp685)->f2;_tmp6F1=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp685)->f3;_tmp6F0=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp685)->f4;_LL5E7: {
# 3414
struct Cyc_List_List*_tmp698=Cyc_Tcutil_substs(rgn,inst,_tmp6F2);
return _tmp698 == _tmp6F2?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp699=_cycalloc(sizeof(*_tmp699));_tmp699[0]=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp69A;_tmp69A.tag=17;_tmp69A.f1=_tmp6F3;_tmp69A.f2=_tmp698;_tmp69A.f3=_tmp6F1;_tmp69A.f4=_tmp6F0;_tmp69A;});_tmp699;});}case 8: _LL5E8: _tmp6F8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp685)->f1).elt_type;_tmp6F7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp685)->f1).tq;_tmp6F6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp685)->f1).num_elts;_tmp6F5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp685)->f1).zero_term;_tmp6F4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp685)->f1).zt_loc;_LL5E9: {
# 3417
void*_tmp69B=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6F8);
struct Cyc_Absyn_Exp*_tmp69C=_tmp6F6 == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,_tmp6F6);
return _tmp69B == _tmp6F8  && _tmp69C == _tmp6F6?t:(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp69D=_cycalloc(sizeof(*_tmp69D));_tmp69D[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp69E;_tmp69E.tag=8;_tmp69E.f1=({struct Cyc_Absyn_ArrayInfo _tmp69F;_tmp69F.elt_type=_tmp69B;_tmp69F.tq=_tmp6F7;_tmp69F.num_elts=_tmp69C;_tmp69F.zero_term=_tmp6F5;_tmp69F.zt_loc=_tmp6F4;_tmp69F;});_tmp69E;});_tmp69D;});}case 5: _LL5EA: _tmp6FE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp685)->f1).elt_typ;_tmp6FD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp685)->f1).elt_tq;_tmp6FC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp685)->f1).ptr_atts).rgn;_tmp6FB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp685)->f1).ptr_atts).nullable;_tmp6FA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp685)->f1).ptr_atts).bounds;_tmp6F9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp685)->f1).ptr_atts).zero_term;_LL5EB: {
# 3422
void*_tmp6A0=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6FE);
void*_tmp6A1=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6FC);
union Cyc_Absyn_Constraint*_tmp6A2=_tmp6FA;
{void*_tmp6A3=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp6FA);void*_tmp6A4=_tmp6A3;struct Cyc_Absyn_Exp*_tmp6A8;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6A4)->tag == 1){_LL61E: _tmp6A8=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6A4)->f1;_LL61F: {
# 3427
struct Cyc_Absyn_Exp*_tmp6A5=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp6A8);
if(_tmp6A5 != _tmp6A8)
_tmp6A2=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6A6=_cycalloc(sizeof(*_tmp6A6));_tmp6A6[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp6A7;_tmp6A7.tag=1;_tmp6A7.f1=_tmp6A5;_tmp6A7;});_tmp6A6;}));
goto _LL61D;}}else{_LL620: _LL621:
 goto _LL61D;}_LL61D:;}
# 3433
if((_tmp6A0 == _tmp6FE  && _tmp6A1 == _tmp6FC) && _tmp6A2 == _tmp6FA)
return t;
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6A9=_cycalloc(sizeof(*_tmp6A9));_tmp6A9[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp6AA;_tmp6AA.tag=5;_tmp6AA.f1=({struct Cyc_Absyn_PtrInfo _tmp6AB;_tmp6AB.elt_typ=_tmp6A0;_tmp6AB.elt_tq=_tmp6FD;_tmp6AB.ptr_atts=({(_tmp6AB.ptr_atts).rgn=_tmp6A1;(_tmp6AB.ptr_atts).nullable=_tmp6FB;(_tmp6AB.ptr_atts).bounds=_tmp6A2;(_tmp6AB.ptr_atts).zero_term=_tmp6F9;(_tmp6AB.ptr_atts).ptrloc=0;_tmp6AB.ptr_atts;});_tmp6AB;});_tmp6AA;});_tmp6A9;});}case 9: _LL5EC: _tmp709=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp685)->f1).tvars;_tmp708=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp685)->f1).effect;_tmp707=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp685)->f1).ret_tqual;_tmp706=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp685)->f1).ret_typ;_tmp705=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp685)->f1).args;_tmp704=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp685)->f1).c_varargs;_tmp703=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp685)->f1).cyc_varargs;_tmp702=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp685)->f1).rgn_po;_tmp701=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp685)->f1).attributes;_tmp700=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp685)->f1).requires_clause;_tmp6FF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp685)->f1).ensures_clause;_LL5ED:
# 3439
{struct Cyc_List_List*_tmp6AC=_tmp709;for(0;_tmp6AC != 0;_tmp6AC=_tmp6AC->tl){
inst=({struct Cyc_List_List*_tmp6AD=_region_malloc(rgn,sizeof(*_tmp6AD));_tmp6AD->hd=({struct _tuple16*_tmp6AE=_region_malloc(rgn,sizeof(*_tmp6AE));_tmp6AE->f1=(struct Cyc_Absyn_Tvar*)_tmp6AC->hd;_tmp6AE->f2=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp6AF=_cycalloc(sizeof(*_tmp6AF));_tmp6AF[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp6B0;_tmp6B0.tag=2;_tmp6B0.f1=(struct Cyc_Absyn_Tvar*)_tmp6AC->hd;_tmp6B0;});_tmp6AF;});_tmp6AE;});_tmp6AD->tl=inst;_tmp6AD;});}}{
struct _tuple1 _tmp6B1=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp705));
# 3441
struct _tuple1 _tmp6B2=_tmp6B1;struct Cyc_List_List*_tmp6C3;struct Cyc_List_List*_tmp6C2;_LL623: _tmp6C3=_tmp6B2.f1;_tmp6C2=_tmp6B2.f2;_LL624:;{
# 3443
struct Cyc_List_List*_tmp6B3=_tmp705;
struct Cyc_List_List*_tmp6B4=Cyc_Tcutil_substs(rgn,inst,_tmp6C2);
if(_tmp6B4 != _tmp6C2)
_tmp6B3=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmp705,_tmp6B4);{
void*eff2;
if(_tmp708 == 0)
eff2=0;else{
# 3451
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp708);
if(new_eff == _tmp708)
eff2=_tmp708;else{
# 3455
eff2=new_eff;}}{
# 3457
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(_tmp703 == 0)
cyc_varargs2=0;else{
# 3461
struct Cyc_Absyn_VarargInfo _tmp6B5=*_tmp703;struct Cyc_Absyn_VarargInfo _tmp6B6=_tmp6B5;struct _dyneither_ptr*_tmp6BC;struct Cyc_Absyn_Tqual _tmp6BB;void*_tmp6BA;int _tmp6B9;_LL626: _tmp6BC=_tmp6B6.name;_tmp6BB=_tmp6B6.tq;_tmp6BA=_tmp6B6.type;_tmp6B9=_tmp6B6.inject;_LL627:;{
void*_tmp6B7=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6BA);
if(_tmp6B7 == _tmp6BA)cyc_varargs2=_tmp703;else{
# 3465
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp6B8=_cycalloc(sizeof(*_tmp6B8));_tmp6B8->name=_tmp6BC;_tmp6B8->tq=_tmp6BB;_tmp6B8->type=_tmp6B7;_tmp6B8->inject=_tmp6B9;_tmp6B8;});}};}{
# 3467
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp702);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp700);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp6FF);
struct Cyc_List_List*_tmp6BD=Cyc_Tcutil_extract_relns(_tmp6B3,req2);
struct Cyc_List_List*_tmp6BE=Cyc_Tcutil_extract_relns(_tmp6B3,ens2);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp6BF=_cycalloc(sizeof(*_tmp6BF));_tmp6BF[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp6C0;_tmp6C0.tag=9;_tmp6C0.f1=({struct Cyc_Absyn_FnInfo _tmp6C1;_tmp6C1.tvars=_tmp709;_tmp6C1.effect=eff2;_tmp6C1.ret_tqual=_tmp707;_tmp6C1.ret_typ=
Cyc_Tcutil_rsubstitute(rgn,inst,_tmp706);_tmp6C1.args=_tmp6B3;_tmp6C1.c_varargs=_tmp704;_tmp6C1.cyc_varargs=cyc_varargs2;_tmp6C1.rgn_po=rgn_po2;_tmp6C1.attributes=_tmp701;_tmp6C1.requires_clause=req2;_tmp6C1.requires_relns=_tmp6BD;_tmp6C1.ensures_clause=ens2;_tmp6C1.ensures_relns=_tmp6BE;_tmp6C1;});_tmp6C0;});_tmp6BF;});};};};};};case 10: _LL5EE: _tmp70A=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp685)->f1;_LL5EF: {
# 3477
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp6C4=_tmp70A;for(0;_tmp6C4 != 0;_tmp6C4=_tmp6C4->tl){
void*_tmp6C5=(*((struct _tuple12*)_tmp6C4->hd)).f2;
void*_tmp6C6=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6C5);
if(_tmp6C5 != _tmp6C6)
change=1;
# 3485
ts2=({struct Cyc_List_List*_tmp6C7=_region_malloc(rgn,sizeof(*_tmp6C7));_tmp6C7->hd=_tmp6C6;_tmp6C7->tl=ts2;_tmp6C7;});}}
# 3487
if(!change)
return t;{
struct Cyc_List_List*_tmp6C8=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp70A,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6C9=_cycalloc(sizeof(*_tmp6C9));_tmp6C9[0]=({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp6CA;_tmp6CA.tag=10;_tmp6CA.f1=_tmp6C8;_tmp6CA;});_tmp6C9;});};}case 12: _LL5F0: _tmp70C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp685)->f1;_tmp70B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp685)->f2;_LL5F1: {
# 3492
struct Cyc_List_List*_tmp6CB=Cyc_Tcutil_subst_aggrfields(rgn,inst,_tmp70B);
if(_tmp70B == _tmp6CB)return t;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp6CC=_cycalloc(sizeof(*_tmp6CC));_tmp6CC[0]=({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp6CD;_tmp6CD.tag=12;_tmp6CD.f1=_tmp70C;_tmp6CD.f2=_tmp6CB;_tmp6CD;});_tmp6CC;});}case 1: _LL5F2: _tmp70D=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp685)->f2;_LL5F3:
# 3496
 if(_tmp70D != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp70D);else{
return t;}case 15: _LL5F4: _tmp70E=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp685)->f1;_LL5F5: {
# 3499
void*_tmp6CE=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp70E);
return _tmp6CE == _tmp70E?t:(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp6CF=_cycalloc(sizeof(*_tmp6CF));_tmp6CF[0]=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp6D0;_tmp6D0.tag=15;_tmp6D0.f1=_tmp6CE;_tmp6D0;});_tmp6CF;});}case 16: _LL5F6: _tmp710=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp685)->f1;_tmp70F=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp685)->f2;_LL5F7: {
# 3502
void*_tmp6D1=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp710);
void*_tmp6D2=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp70F);
return _tmp6D1 == _tmp710  && _tmp6D2 == _tmp70F?t:(void*)({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp6D3=_cycalloc(sizeof(*_tmp6D3));_tmp6D3[0]=({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp6D4;_tmp6D4.tag=16;_tmp6D4.f1=_tmp6D1;_tmp6D4.f2=_tmp6D2;_tmp6D4;});_tmp6D3;});}case 19: _LL5F8: _tmp711=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp685)->f1;_LL5F9: {
# 3506
void*_tmp6D5=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp711);
return _tmp6D5 == _tmp711?t:(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp6D6=_cycalloc(sizeof(*_tmp6D6));_tmp6D6[0]=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp6D7;_tmp6D7.tag=19;_tmp6D7.f1=_tmp6D5;_tmp6D7;});_tmp6D6;});}case 18: _LL5FA: _tmp712=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp685)->f1;_LL5FB: {
# 3509
struct Cyc_Absyn_Exp*_tmp6D8=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp712);
return _tmp6D8 == _tmp712?t:(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp6D9=_cycalloc(sizeof(*_tmp6D9));_tmp6D9[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp6DA;_tmp6DA.tag=18;_tmp6DA.f1=_tmp6D8;_tmp6DA;});_tmp6D9;});}case 27: _LL5FC: _tmp713=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp685)->f1;_LL5FD: {
# 3512
struct Cyc_Absyn_Exp*_tmp6DB=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp713);
return _tmp6DB == _tmp713?t:(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp6DC=_cycalloc(sizeof(*_tmp6DC));_tmp6DC[0]=({struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmp6DD;_tmp6DD.tag=27;_tmp6DD.f1=_tmp6DB;_tmp6DD;});_tmp6DC;});}case 13: _LL5FE: _LL5FF:
 goto _LL601;case 14: _LL600: _LL601:
 goto _LL603;case 0: _LL602: _LL603:
 goto _LL605;case 6: _LL604: _LL605:
 goto _LL607;case 7: _LL606: _LL607:
 goto _LL609;case 22: _LL608: _LL609:
 goto _LL60B;case 21: _LL60A: _LL60B:
 goto _LL60D;case 28: _LL60C: _LL60D:
 goto _LL60F;case 20: _LL60E: _LL60F:
 return t;case 25: _LL610: _tmp714=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp685)->f1;_LL611: {
# 3524
void*_tmp6DE=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp714);
return _tmp6DE == _tmp714?t:(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp6DF=_cycalloc(sizeof(*_tmp6DF));_tmp6DF[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp6E0;_tmp6E0.tag=25;_tmp6E0.f1=_tmp6DE;_tmp6E0;});_tmp6DF;});}case 23: _LL612: _tmp715=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp685)->f1;_LL613: {
# 3527
void*_tmp6E1=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp715);
return _tmp6E1 == _tmp715?t:(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp6E2=_cycalloc(sizeof(*_tmp6E2));_tmp6E2[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp6E3;_tmp6E3.tag=23;_tmp6E3.f1=_tmp6E1;_tmp6E3;});_tmp6E2;});}case 24: _LL614: _tmp716=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp685)->f1;_LL615: {
# 3530
struct Cyc_List_List*_tmp6E4=Cyc_Tcutil_substs(rgn,inst,_tmp716);
return _tmp6E4 == _tmp716?t:(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp6E5=_cycalloc(sizeof(*_tmp6E5));_tmp6E5[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp6E6;_tmp6E6.tag=24;_tmp6E6.f1=_tmp6E4;_tmp6E6;});_tmp6E5;});}default: _LL616: _LL617:
({void*_tmp6E7=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp6E8="found typedecltype in rsubs";_tag_dyneither(_tmp6E8,sizeof(char),28);}),_tag_dyneither(_tmp6E7,sizeof(void*),0));});}_LL5DD:;}
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
struct _tuple17*_tmp71F=env;struct Cyc_List_List*_tmp724;struct _RegionHandle*_tmp723;_LL629: _tmp724=_tmp71F->f1;_tmp723=_tmp71F->f2;_LL62A:;{
struct Cyc_Core_Opt*_tmp720=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple16*_tmp721=_region_malloc(_tmp723,sizeof(*_tmp721));_tmp721->f1=tv;_tmp721->f2=Cyc_Absyn_new_evar(_tmp720,({struct Cyc_Core_Opt*_tmp722=_cycalloc(sizeof(*_tmp722));_tmp722->v=_tmp724;_tmp722;}));_tmp721;});};}
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
struct _tuple28*_tmp735=(struct _tuple28*)tvs2->hd;struct _tuple28*_tmp736=_tmp735;struct Cyc_Absyn_Tvar*_tmp73A;int*_tmp739;_LL62C: _tmp73A=_tmp736->f1;_tmp739=(int*)& _tmp736->f2;_LL62D:;
if(_tmp73A->identity == - 1)
({void*_tmp737=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp738="fast_add_free_tvar_bool: bad identity in tvs2";_tag_dyneither(_tmp738,sizeof(char),46);}),_tag_dyneither(_tmp737,sizeof(void*),0));});
if(_tmp73A->identity == tv->identity){
*_tmp739=*_tmp739  || b;
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
void*_tmp741=Cyc_Tcutil_compress(e);void*_tmp742=_tmp741;int _tmp74C;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp742)->tag == 1){_LL62F: _tmp74C=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp742)->f3;_LL630:
# 3654
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple29*_tmp743=(struct _tuple29*)es2->hd;struct _tuple29*_tmp744=_tmp743;void*_tmp749;int*_tmp748;_LL634: _tmp749=_tmp744->f1;_tmp748=(int*)& _tmp744->f2;_LL635:;{
void*_tmp745=Cyc_Tcutil_compress(_tmp749);void*_tmp746=_tmp745;int _tmp747;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp746)->tag == 1){_LL637: _tmp747=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp746)->f3;_LL638:
# 3658
 if(_tmp74C == _tmp747){
if(b != *_tmp748)*_tmp748=1;
return es;}
# 3662
goto _LL636;}else{_LL639: _LL63A:
 goto _LL636;}_LL636:;};}}
# 3666
return({struct Cyc_List_List*_tmp74A=_region_malloc(r,sizeof(*_tmp74A));_tmp74A->hd=({struct _tuple29*_tmp74B=_region_malloc(r,sizeof(*_tmp74B));_tmp74B->f1=e;_tmp74B->f2=b;_tmp74B;});_tmp74A->tl=es;_tmp74A;});}else{_LL631: _LL632:
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
struct _tuple28 _tmp74E=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp74F=_tmp74E;struct Cyc_Absyn_Tvar*_tmp752;int _tmp751;_LL63C: _tmp752=_tmp74F.f1;_tmp751=_tmp74F.f2;_LL63D:;{
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp752->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3704
if(!present)res=({struct Cyc_List_List*_tmp750=_region_malloc(r,sizeof(*_tmp750));_tmp750->hd=(struct _tuple28*)tvs->hd;_tmp750->tl=res;_tmp750;});};}
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
struct _tuple14 _tmp756=Cyc_Evexp_eval_const_uint_exp(width);struct _tuple14 _tmp757=_tmp756;unsigned int _tmp75B;int _tmp75A;_LL63F: _tmp75B=_tmp757.f1;_tmp75A=_tmp757.f2;_LL640:;
if(!_tmp75A)
({void*_tmp758=0;Cyc_Tcutil_terr(loc,({const char*_tmp759="bitfield width cannot use sizeof or offsetof";_tag_dyneither(_tmp759,sizeof(char),45);}),_tag_dyneither(_tmp758,sizeof(void*),0));});
w=_tmp75B;}{
# 3722
void*_tmp75C=Cyc_Tcutil_compress(field_typ);void*_tmp75D=_tmp75C;enum Cyc_Absyn_Size_of _tmp76B;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp75D)->tag == 6){_LL642: _tmp76B=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp75D)->f2;_LL643:
# 3725
{enum Cyc_Absyn_Size_of _tmp75E=_tmp76B;switch(_tmp75E){case Cyc_Absyn_Char_sz: _LL647: _LL648:
 if(w > 8)({void*_tmp75F=0;Cyc_Tcutil_terr(loc,({const char*_tmp760="bitfield larger than type";_tag_dyneither(_tmp760,sizeof(char),26);}),_tag_dyneither(_tmp75F,sizeof(void*),0));});goto _LL646;case Cyc_Absyn_Short_sz: _LL649: _LL64A:
 if(w > 16)({void*_tmp761=0;Cyc_Tcutil_terr(loc,({const char*_tmp762="bitfield larger than type";_tag_dyneither(_tmp762,sizeof(char),26);}),_tag_dyneither(_tmp761,sizeof(void*),0));});goto _LL646;case Cyc_Absyn_Long_sz: _LL64B: _LL64C:
 goto _LL64E;case Cyc_Absyn_Int_sz: _LL64D: _LL64E:
# 3730
 if(w > 32)({void*_tmp763=0;Cyc_Tcutil_terr(loc,({const char*_tmp764="bitfield larger than type";_tag_dyneither(_tmp764,sizeof(char),26);}),_tag_dyneither(_tmp763,sizeof(void*),0));});goto _LL646;default: _LL64F: _LL650:
# 3732
 if(w > 64)({void*_tmp765=0;Cyc_Tcutil_terr(loc,({const char*_tmp766="bitfield larger than type";_tag_dyneither(_tmp766,sizeof(char),26);}),_tag_dyneither(_tmp765,sizeof(void*),0));});goto _LL646;}_LL646:;}
# 3734
goto _LL641;}else{_LL644: _LL645:
# 3736
({struct Cyc_String_pa_PrintArg_struct _tmp76A;_tmp76A.tag=0;_tmp76A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(field_typ));({struct Cyc_String_pa_PrintArg_struct _tmp769;_tmp769.tag=0;_tmp769.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);({void*_tmp767[2]={& _tmp769,& _tmp76A};Cyc_Tcutil_terr(loc,({const char*_tmp768="bitfield %s must have integral type but has type %s";_tag_dyneither(_tmp768,sizeof(char),52);}),_tag_dyneither(_tmp767,sizeof(void*),2));});});});
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
void*_tmp772=t;struct Cyc_Absyn_Typedefdecl*_tmp776;void*_tmp775;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp772)->tag == 17){_LL659: _tmp776=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp772)->f3;_tmp775=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp772)->f4;_LL65A:
# 3774
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp776))->tq).real_const  || (_tmp776->tq).print_const){
if(declared_const)({void*_tmp773=0;Cyc_Tcutil_warn(loc,({const char*_tmp774="extra const";_tag_dyneither(_tmp774,sizeof(char),12);}),_tag_dyneither(_tmp773,sizeof(void*),0));});
return 1;}
# 3779
if((unsigned int)_tmp775)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp775);else{
return declared_const;}}else{_LL65B: _LL65C:
 return declared_const;}_LL658:;}
# 3786
static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td != 0){
if(td->defn != 0){
void*_tmp777=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_tmp778=_tmp777;void*_tmp77C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp778)->tag == 5){_LL65E: _tmp77C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp778)->f1).ptr_atts).rgn;_LL65F:
# 3791
{void*_tmp779=Cyc_Tcutil_compress(_tmp77C);void*_tmp77A=_tmp779;struct Cyc_Absyn_Tvar*_tmp77B;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp77A)->tag == 2){_LL663: _tmp77B=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp77A)->f1;_LL664:
# 3793
 return Cyc_Absyn_tvar_cmp(tvar,_tmp77B)== 0;}else{_LL665: _LL666:
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
{void*_tmp788=Cyc_Tcutil_compress(t);void*_tmp789=_tmp788;struct Cyc_List_List*_tmp95F;void*_tmp95E;void*_tmp95D;void*_tmp95C;void*_tmp95B;void*_tmp95A;struct _tuple2*_tmp959;struct Cyc_List_List**_tmp958;struct Cyc_Absyn_Typedefdecl**_tmp957;void**_tmp956;union Cyc_Absyn_AggrInfoU*_tmp955;struct Cyc_List_List**_tmp954;enum Cyc_Absyn_AggrKind _tmp953;struct Cyc_List_List*_tmp952;struct Cyc_List_List*_tmp951;struct Cyc_List_List**_tmp950;void**_tmp94F;struct Cyc_Absyn_Tqual*_tmp94E;void*_tmp94D;struct Cyc_List_List*_tmp94C;int _tmp94B;struct Cyc_Absyn_VarargInfo*_tmp94A;struct Cyc_List_List*_tmp949;struct Cyc_List_List*_tmp948;struct Cyc_Absyn_Exp*_tmp947;struct Cyc_List_List**_tmp946;struct Cyc_Absyn_Exp*_tmp945;struct Cyc_List_List**_tmp944;void*_tmp943;struct Cyc_Absyn_Tqual*_tmp942;struct Cyc_Absyn_Exp**_tmp941;union Cyc_Absyn_Constraint*_tmp940;unsigned int _tmp93F;struct Cyc_Absyn_Exp*_tmp93E;struct Cyc_Absyn_Exp*_tmp93D;void*_tmp93C;void*_tmp93B;struct Cyc_Absyn_Tqual*_tmp93A;void*_tmp939;union Cyc_Absyn_Constraint*_tmp938;union Cyc_Absyn_Constraint*_tmp937;union Cyc_Absyn_Constraint*_tmp936;union Cyc_Absyn_DatatypeFieldInfoU*_tmp935;struct Cyc_List_List*_tmp934;union Cyc_Absyn_DatatypeInfoU*_tmp933;struct Cyc_List_List**_tmp932;struct _tuple2*_tmp931;struct Cyc_Absyn_Enumdecl**_tmp930;struct Cyc_List_List*_tmp92F;void*_tmp92E;void***_tmp92D;struct Cyc_Absyn_Tvar*_tmp92C;struct Cyc_Core_Opt**_tmp92B;void**_tmp92A;switch(*((int*)_tmp789)){case 0: _LL66F: _LL670:
 goto _LL66E;case 1: _LL671: _tmp92B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp789)->f1;_tmp92A=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp789)->f2;_LL672:
# 3896
 if(*_tmp92B == 0  || 
Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp92B))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp92B))->v,expected_kind))
*_tmp92B=Cyc_Tcutil_kind_to_opt(expected_kind);
if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){
# 3901
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
*_tmp92A=(void*)& Cyc_Absyn_UniqueRgn_val;else{
# 3904
*_tmp92A=(void*)& Cyc_Absyn_HeapRgn_val;}}else{
if(cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar*_tmp78A=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp78D=_cycalloc(sizeof(*_tmp78D));_tmp78D[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp78E;_tmp78E.tag=2;_tmp78E.f1=0;_tmp78E.f2=expected_kind;_tmp78E;});_tmp78D;}));
*_tmp92A=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp78B=_cycalloc(sizeof(*_tmp78B));_tmp78B[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp78C;_tmp78C.tag=2;_tmp78C.f1=_tmp78A;_tmp78C;});_tmp78B;});
_tmp92C=_tmp78A;goto _LL674;}else{
# 3910
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}
# 3912
goto _LL66E;case 2: _LL673: _tmp92C=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp789)->f1;_LL674:
# 3914
{void*_tmp78F=Cyc_Absyn_compress_kb(_tmp92C->kind);void*_tmp790=_tmp78F;struct Cyc_Core_Opt**_tmp794;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp790)->tag == 1){_LL6AA: _tmp794=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp790)->f1;_LL6AB:
# 3916
*_tmp794=({struct Cyc_Core_Opt*_tmp791=_cycalloc(sizeof(*_tmp791));_tmp791->v=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp792=_cycalloc(sizeof(*_tmp792));_tmp792[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp793;_tmp793.tag=2;_tmp793.f1=0;_tmp793.f2=expected_kind;_tmp793;});_tmp792;});_tmp791;});goto _LL6A9;}else{_LL6AC: _LL6AD:
 goto _LL6A9;}_LL6A9:;}
# 3921
cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp92C);
# 3924
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp92C,put_in_effect);
# 3926
{void*_tmp795=Cyc_Absyn_compress_kb(_tmp92C->kind);void*_tmp796=_tmp795;struct Cyc_Core_Opt**_tmp79B;struct Cyc_Absyn_Kind*_tmp79A;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp796)->tag == 2){_LL6AF: _tmp79B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp796)->f1;_tmp79A=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp796)->f2;_LL6B0:
# 3928
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmp79A))
*_tmp79B=({struct Cyc_Core_Opt*_tmp797=_cycalloc(sizeof(*_tmp797));_tmp797->v=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp798=_cycalloc(sizeof(*_tmp798));_tmp798[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp799;_tmp799.tag=2;_tmp799.f1=0;_tmp799.f2=expected_kind;_tmp799;});_tmp798;});_tmp797;});
goto _LL6AE;}else{_LL6B1: _LL6B2:
 goto _LL6AE;}_LL6AE:;}
# 3933
goto _LL66E;case 26: _LL675: _tmp92E=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp789)->f1)->r;_tmp92D=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp789)->f2;_LL676: {
# 3939
void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmp79C=_tmp92E;struct Cyc_Absyn_Datatypedecl*_tmp79F;struct Cyc_Absyn_Enumdecl*_tmp79E;struct Cyc_Absyn_Aggrdecl*_tmp79D;switch(*((int*)_tmp79C)){case 0: _LL6B4: _tmp79D=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp79C)->f1;_LL6B5:
# 3942
 if(te->in_extern_c_include)
_tmp79D->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp79D);goto _LL6B3;case 1: _LL6B6: _tmp79E=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp79C)->f1;_LL6B7:
# 3946
 if(te->in_extern_c_include)
_tmp79E->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp79E);goto _LL6B3;default: _LL6B8: _tmp79F=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp79C)->f1;_LL6B9:
# 3950
 Cyc_Tc_tcDatatypedecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp79F);goto _LL6B3;}_LL6B3:;}
# 3952
*_tmp92D=({void**_tmp7A0=_cycalloc(sizeof(*_tmp7A0));_tmp7A0[0]=new_t;_tmp7A0;});
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect,allow_abs_aggr);}case 14: _LL677: _tmp92F=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp789)->f1;_LL678: {
# 3958
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct _RegionHandle _tmp7A1=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp7A1;_push_region(uprev_rgn);{
struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;
for(0;_tmp92F != 0;_tmp92F=_tmp92F->tl){
struct Cyc_Absyn_Enumfield*_tmp7A2=(struct Cyc_Absyn_Enumfield*)_tmp92F->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp7A2->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp7A5;_tmp7A5.tag=0;_tmp7A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp7A2->name).f2);({void*_tmp7A3[1]={& _tmp7A5};Cyc_Tcutil_terr(_tmp7A2->loc,({const char*_tmp7A4="duplicate enum field name %s";_tag_dyneither(_tmp7A4,sizeof(char),29);}),_tag_dyneither(_tmp7A3,sizeof(void*),1));});});else{
# 3967
prev_fields=({struct Cyc_List_List*_tmp7A6=_region_malloc(uprev_rgn,sizeof(*_tmp7A6));_tmp7A6->hd=(*_tmp7A2->name).f2;_tmp7A6->tl=prev_fields;_tmp7A6;});}
# 3969
if(_tmp7A2->tag == 0)
_tmp7A2->tag=Cyc_Absyn_uint_exp(tag_count,_tmp7A2->loc);else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp7A2->tag)))
({struct Cyc_String_pa_PrintArg_struct _tmp7A9;_tmp7A9.tag=0;_tmp7A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp7A2->name).f2);({void*_tmp7A7[1]={& _tmp7A9};Cyc_Tcutil_terr(loc,({const char*_tmp7A8="enum field %s: expression is not constant";_tag_dyneither(_tmp7A8,sizeof(char),42);}),_tag_dyneither(_tmp7A7,sizeof(void*),1));});});}{
# 3974
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp7A2->tag))).f1;
tag_count=t1 + 1;
{union Cyc_Absyn_Nmspace _tmp7AA=(*_tmp7A2->name).f1;union Cyc_Absyn_Nmspace _tmp7AB=_tmp7AA;if((_tmp7AB.Rel_n).tag == 1){_LL6BB: _LL6BC:
# 3978
(*_tmp7A2->name).f1=Cyc_Absyn_Abs_n(te->ns,0);goto _LL6BA;}else{_LL6BD: _LL6BE:
 goto _LL6BA;}_LL6BA:;}
# 3981
ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple29*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp7A2->name).f2,({struct _tuple29*_tmp7AC=_cycalloc(sizeof(*_tmp7AC));_tmp7AC->f1=(void*)({struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp7AD=_cycalloc(sizeof(*_tmp7AD));_tmp7AD[0]=({struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct _tmp7AE;_tmp7AE.tag=4;_tmp7AE.f1=t;_tmp7AE.f2=_tmp7A2;_tmp7AE;});_tmp7AD;});_tmp7AC->f2=1;_tmp7AC;}));};}}
# 3985
_npop_handler(0);goto _LL66E;
# 3959
;_pop_region(uprev_rgn);}case 13: _LL679: _tmp931=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp789)->f1;_tmp930=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp789)->f2;_LL67A:
# 3987
 if(*_tmp930 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp930))->fields == 0){
struct _handler_cons _tmp7AF;_push_handler(& _tmp7AF);{int _tmp7B1=0;if(setjmp(_tmp7AF.handler))_tmp7B1=1;if(!_tmp7B1){
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp931);
*_tmp930=*ed;}
# 3989
;_pop_handler();}else{void*_tmp7B0=(void*)_exn_thrown;void*_tmp7B2=_tmp7B0;void*_tmp7B6;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp7B2)->tag == Cyc_Dict_Absent){_LL6C0: _LL6C1: {
# 3993
struct Cyc_Tcenv_Genv*_tmp7B3=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp7B4=({struct Cyc_Absyn_Enumdecl*_tmp7B5=_cycalloc(sizeof(*_tmp7B5));_tmp7B5->sc=Cyc_Absyn_Extern;_tmp7B5->name=_tmp931;_tmp7B5->fields=0;_tmp7B5;});
Cyc_Tc_tcEnumdecl(te,_tmp7B3,loc,_tmp7B4);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp931);
*_tmp930=*ed;
goto _LL6BF;};}}else{_LL6C2: _tmp7B6=_tmp7B2;_LL6C3:(int)_rethrow(_tmp7B6);}_LL6BF:;}};}{
# 4002
struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp930);
# 4004
*_tmp931=(ed->name)[0];
goto _LL66E;};case 3: _LL67B: _tmp933=(union Cyc_Absyn_DatatypeInfoU*)&(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp789)->f1).datatype_info;_tmp932=(struct Cyc_List_List**)&(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp789)->f1).targs;_LL67C: {
# 4007
struct Cyc_List_List*_tmp7B7=*_tmp932;
{union Cyc_Absyn_DatatypeInfoU _tmp7B8=*_tmp933;union Cyc_Absyn_DatatypeInfoU _tmp7B9=_tmp7B8;struct Cyc_Absyn_Datatypedecl*_tmp7D2;struct _tuple2*_tmp7D1;int _tmp7D0;if((_tmp7B9.UnknownDatatype).tag == 1){_LL6C5: _tmp7D1=((_tmp7B9.UnknownDatatype).val).name;_tmp7D0=((_tmp7B9.UnknownDatatype).val).is_extensible;_LL6C6: {
# 4010
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmp7BA;_push_handler(& _tmp7BA);{int _tmp7BC=0;if(setjmp(_tmp7BA.handler))_tmp7BC=1;if(!_tmp7BC){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp7D1);;_pop_handler();}else{void*_tmp7BB=(void*)_exn_thrown;void*_tmp7BD=_tmp7BB;void*_tmp7C4;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp7BD)->tag == Cyc_Dict_Absent){_LL6CA: _LL6CB: {
# 4014
struct Cyc_Tcenv_Genv*_tmp7BE=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Datatypedecl*_tmp7BF=({struct Cyc_Absyn_Datatypedecl*_tmp7C3=_cycalloc(sizeof(*_tmp7C3));_tmp7C3->sc=Cyc_Absyn_Extern;_tmp7C3->name=_tmp7D1;_tmp7C3->tvs=0;_tmp7C3->fields=0;_tmp7C3->is_extensible=_tmp7D0;_tmp7C3;});
Cyc_Tc_tcDatatypedecl(te,_tmp7BE,loc,_tmp7BF);
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp7D1);
# 4019
if(_tmp7B7 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp7C2;_tmp7C2.tag=0;_tmp7C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7D1));({void*_tmp7C0[1]={& _tmp7C2};Cyc_Tcutil_terr(loc,({const char*_tmp7C1="declare parameterized datatype %s before using";_tag_dyneither(_tmp7C1,sizeof(char),47);}),_tag_dyneither(_tmp7C0,sizeof(void*),1));});});
return cvtenv;}
# 4024
goto _LL6C9;}}else{_LL6CC: _tmp7C4=_tmp7BD;_LL6CD:(int)_rethrow(_tmp7C4);}_LL6C9:;}};}
# 4030
if(_tmp7D0  && !(*tudp)->is_extensible)
({struct Cyc_String_pa_PrintArg_struct _tmp7C7;_tmp7C7.tag=0;_tmp7C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7D1));({void*_tmp7C5[1]={& _tmp7C7};Cyc_Tcutil_terr(loc,({const char*_tmp7C6="datatype %s was not declared @extensible";_tag_dyneither(_tmp7C6,sizeof(char),41);}),_tag_dyneither(_tmp7C5,sizeof(void*),1));});});
*_tmp933=Cyc_Absyn_KnownDatatype(tudp);
_tmp7D2=*tudp;goto _LL6C8;}}else{_LL6C7: _tmp7D2=*(_tmp7B9.KnownDatatype).val;_LL6C8: {
# 4037
struct Cyc_List_List*tvs=_tmp7D2->tvs;
for(0;_tmp7B7 != 0  && tvs != 0;(_tmp7B7=_tmp7B7->tl,tvs=tvs->tl)){
void*t=(void*)_tmp7B7->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 4043
{struct _tuple0 _tmp7C8=({struct _tuple0 _tmp7CB;_tmp7CB.f1=Cyc_Absyn_compress_kb(tv->kind);_tmp7CB.f2=t;_tmp7CB;});struct _tuple0 _tmp7C9=_tmp7C8;struct Cyc_Absyn_Tvar*_tmp7CA;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7C9.f1)->tag == 1){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7C9.f2)->tag == 2){_LL6CF: _tmp7CA=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7C9.f2)->f1;_LL6D0:
# 4045
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp7CA);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp7CA,1);
continue;}else{goto _LL6D1;}}else{_LL6D1: _LL6D2:
 goto _LL6CE;}_LL6CE:;}{
# 4050
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 4054
if(_tmp7B7 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp7CE;_tmp7CE.tag=0;_tmp7CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7D2->name));({void*_tmp7CC[1]={& _tmp7CE};Cyc_Tcutil_terr(loc,({const char*_tmp7CD="too many type arguments for datatype %s";_tag_dyneither(_tmp7CD,sizeof(char),40);}),_tag_dyneither(_tmp7CC,sizeof(void*),1));});});
if(tvs != 0){
# 4059
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp7CF=_cycalloc(sizeof(*_tmp7CF));_tmp7CF->hd=e;_tmp7CF->tl=hidden_ts;_tmp7CF;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1,allow_abs_aggr);}
# 4066
*_tmp932=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp932,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4068
goto _LL6C4;}}_LL6C4:;}
# 4070
goto _LL66E;}case 4: _LL67D: _tmp935=(union Cyc_Absyn_DatatypeFieldInfoU*)&(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp789)->f1).field_info;_tmp934=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp789)->f1).targs;_LL67E:
# 4073
{union Cyc_Absyn_DatatypeFieldInfoU _tmp7D3=*_tmp935;union Cyc_Absyn_DatatypeFieldInfoU _tmp7D4=_tmp7D3;struct Cyc_Absyn_Datatypedecl*_tmp803;struct Cyc_Absyn_Datatypefield*_tmp802;struct _tuple2*_tmp801;struct _tuple2*_tmp800;int _tmp7FF;if((_tmp7D4.UnknownDatatypefield).tag == 1){_LL6D4: _tmp801=((_tmp7D4.UnknownDatatypefield).val).datatype_name;_tmp800=((_tmp7D4.UnknownDatatypefield).val).field_name;_tmp7FF=((_tmp7D4.UnknownDatatypefield).val).is_extensible;_LL6D5: {
# 4075
struct Cyc_Absyn_Datatypedecl*tud;
struct Cyc_Absyn_Datatypefield*tuf;
{struct _handler_cons _tmp7D5;_push_handler(& _tmp7D5);{int _tmp7D7=0;if(setjmp(_tmp7D5.handler))_tmp7D7=1;if(!_tmp7D7){*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp801);;_pop_handler();}else{void*_tmp7D6=(void*)_exn_thrown;void*_tmp7D8=_tmp7D6;void*_tmp7DC;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp7D8)->tag == Cyc_Dict_Absent){_LL6D9: _LL6DA:
# 4079
({struct Cyc_String_pa_PrintArg_struct _tmp7DB;_tmp7DB.tag=0;_tmp7DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp801));({void*_tmp7D9[1]={& _tmp7DB};Cyc_Tcutil_terr(loc,({const char*_tmp7DA="unbound datatype %s";_tag_dyneither(_tmp7DA,sizeof(char),20);}),_tag_dyneither(_tmp7D9,sizeof(void*),1));});});
return cvtenv;}else{_LL6DB: _tmp7DC=_tmp7D8;_LL6DC:(int)_rethrow(_tmp7DC);}_LL6D8:;}};}
# 4082
{struct _handler_cons _tmp7DD;_push_handler(& _tmp7DD);{int _tmp7DF=0;if(setjmp(_tmp7DD.handler))_tmp7DF=1;if(!_tmp7DF){
{struct _RegionHandle _tmp7E0=_new_region("r");struct _RegionHandle*r=& _tmp7E0;_push_region(r);
{void*_tmp7E1=Cyc_Tcenv_lookup_ordinary(r,te,loc,_tmp800,0);void*_tmp7E2=_tmp7E1;struct Cyc_Absyn_Datatypedecl*_tmp7EC;struct Cyc_Absyn_Datatypefield*_tmp7EB;if(((struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp7E2)->tag == 2){_LL6DE: _tmp7EC=((struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp7E2)->f1;_tmp7EB=((struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp7E2)->f2;_LL6DF:
# 4086
 tuf=_tmp7EB;
tud=_tmp7EC;
if(_tmp7FF  && !tud->is_extensible)
({struct Cyc_String_pa_PrintArg_struct _tmp7E5;_tmp7E5.tag=0;_tmp7E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp801));({void*_tmp7E3[1]={& _tmp7E5};Cyc_Tcutil_terr(loc,({const char*_tmp7E4="datatype %s was not declared @extensible";_tag_dyneither(_tmp7E4,sizeof(char),41);}),_tag_dyneither(_tmp7E3,sizeof(void*),1));});});
goto _LL6DD;}else{_LL6E0: _LL6E1:
({struct Cyc_String_pa_PrintArg_struct _tmp7E9;_tmp7E9.tag=0;_tmp7E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp801));({struct Cyc_String_pa_PrintArg_struct _tmp7E8;_tmp7E8.tag=0;_tmp7E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp800));({void*_tmp7E6[2]={& _tmp7E8,& _tmp7E9};Cyc_Tcutil_terr(loc,({const char*_tmp7E7="unbound field %s in type datatype %s";_tag_dyneither(_tmp7E7,sizeof(char),37);}),_tag_dyneither(_tmp7E6,sizeof(void*),2));});});});{
struct Cyc_Tcutil_CVTEnv _tmp7EA=cvtenv;_npop_handler(1);return _tmp7EA;};}_LL6DD:;}
# 4084
;_pop_region(r);}
# 4083
;_pop_handler();}else{void*_tmp7DE=(void*)_exn_thrown;void*_tmp7ED=_tmp7DE;void*_tmp7F2;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp7ED)->tag == Cyc_Dict_Absent){_LL6E3: _LL6E4:
# 4099
({struct Cyc_String_pa_PrintArg_struct _tmp7F1;_tmp7F1.tag=0;_tmp7F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp801));({struct Cyc_String_pa_PrintArg_struct _tmp7F0;_tmp7F0.tag=0;_tmp7F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp800));({void*_tmp7EE[2]={& _tmp7F0,& _tmp7F1};Cyc_Tcutil_terr(loc,({const char*_tmp7EF="unbound field %s in type datatype %s";_tag_dyneither(_tmp7EF,sizeof(char),37);}),_tag_dyneither(_tmp7EE,sizeof(void*),2));});});});
return cvtenv;}else{_LL6E5: _tmp7F2=_tmp7ED;_LL6E6:(int)_rethrow(_tmp7F2);}_LL6E2:;}};}
# 4103
*_tmp935=Cyc_Absyn_KnownDatatypefield(tud,tuf);
_tmp803=tud;_tmp802=tuf;goto _LL6D7;}}else{_LL6D6: _tmp803=((_tmp7D4.KnownDatatypefield).val).f1;_tmp802=((_tmp7D4.KnownDatatypefield).val).f2;_LL6D7: {
# 4107
struct Cyc_List_List*tvs=_tmp803->tvs;
for(0;_tmp934 != 0  && tvs != 0;(_tmp934=_tmp934->tl,tvs=tvs->tl)){
void*t=(void*)_tmp934->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 4113
{struct _tuple0 _tmp7F3=({struct _tuple0 _tmp7F6;_tmp7F6.f1=Cyc_Absyn_compress_kb(tv->kind);_tmp7F6.f2=t;_tmp7F6;});struct _tuple0 _tmp7F4=_tmp7F3;struct Cyc_Absyn_Tvar*_tmp7F5;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7F4.f1)->tag == 1){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7F4.f2)->tag == 2){_LL6E8: _tmp7F5=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7F4.f2)->f1;_LL6E9:
# 4115
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp7F5);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp7F5,1);
continue;}else{goto _LL6EA;}}else{_LL6EA: _LL6EB:
 goto _LL6E7;}_LL6E7:;}{
# 4120
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 4124
if(_tmp934 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp7FA;_tmp7FA.tag=0;_tmp7FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp802->name));({struct Cyc_String_pa_PrintArg_struct _tmp7F9;_tmp7F9.tag=0;_tmp7F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp803->name));({void*_tmp7F7[2]={& _tmp7F9,& _tmp7FA};Cyc_Tcutil_terr(loc,({const char*_tmp7F8="too many type arguments for datatype %s.%s";_tag_dyneither(_tmp7F8,sizeof(char),43);}),_tag_dyneither(_tmp7F7,sizeof(void*),2));});});});
if(tvs != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp7FE;_tmp7FE.tag=0;_tmp7FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp802->name));({struct Cyc_String_pa_PrintArg_struct _tmp7FD;_tmp7FD.tag=0;_tmp7FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp803->name));({void*_tmp7FB[2]={& _tmp7FD,& _tmp7FE};Cyc_Tcutil_terr(loc,({const char*_tmp7FC="too few type arguments for datatype %s.%s";_tag_dyneither(_tmp7FC,sizeof(char),42);}),_tag_dyneither(_tmp7FB,sizeof(void*),2));});});});
goto _LL6D3;}}_LL6D3:;}
# 4132
goto _LL66E;case 5: _LL67F: _tmp93B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp789)->f1).elt_typ;_tmp93A=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp789)->f1).elt_tq;_tmp939=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp789)->f1).ptr_atts).rgn;_tmp938=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp789)->f1).ptr_atts).nullable;_tmp937=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp789)->f1).ptr_atts).bounds;_tmp936=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp789)->f1).ptr_atts).zero_term;_LL680: {
# 4135
int is_zero_terminated;
# 4137
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp93B,1,1);
_tmp93A->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp93A->print_const,_tmp93B);{
struct Cyc_Absyn_Kind*k;
{enum Cyc_Absyn_AliasQual _tmp804=expected_kind->aliasqual;enum Cyc_Absyn_AliasQual _tmp805=_tmp804;switch(_tmp805){case Cyc_Absyn_Aliasable: _LL6ED: _LL6EE:
 k=& Cyc_Tcutil_rk;goto _LL6EC;case Cyc_Absyn_Unique: _LL6EF: _LL6F0:
 k=& Cyc_Tcutil_urk;goto _LL6EC;default: _LL6F1: _LL6F2:
 k=& Cyc_Tcutil_trk;goto _LL6EC;}_LL6EC:;}
# 4145
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmp939,1,1);
{union Cyc_Absyn_Constraint*_tmp806=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp936);union Cyc_Absyn_Constraint*_tmp807=_tmp806;int _tmp80D;switch((((union Cyc_Absyn_Constraint*)_tmp807)->Eq_constr).tag){case 3: _LL6F4: _LL6F5:
# 4150
{void*_tmp808=Cyc_Tcutil_compress(_tmp93B);void*_tmp809=_tmp808;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp809)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp809)->f2 == Cyc_Absyn_Char_sz){_LL6FB: _LL6FC:
# 4152
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp936,Cyc_Absyn_true_conref);
is_zero_terminated=1;
goto _LL6FA;}else{goto _LL6FD;}}else{_LL6FD: _LL6FE:
# 4156
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp936,Cyc_Absyn_false_conref);
is_zero_terminated=0;
goto _LL6FA;}_LL6FA:;}
# 4160
goto _LL6F3;case 1: _LL6F6: _tmp80D=(int)(_tmp807->Eq_constr).val;if(_tmp80D){_LL6F7:
# 4163
 if(!Cyc_Tcutil_admits_zero(_tmp93B))
({struct Cyc_String_pa_PrintArg_struct _tmp80C;_tmp80C.tag=0;_tmp80C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp93B));({void*_tmp80A[1]={& _tmp80C};Cyc_Tcutil_terr(loc,({const char*_tmp80B="cannot have a pointer to zero-terminated %s elements";_tag_dyneither(_tmp80B,sizeof(char),53);}),_tag_dyneither(_tmp80A,sizeof(void*),1));});});
is_zero_terminated=1;
goto _LL6F3;}else{goto _LL6F8;}default: _LL6F8: _LL6F9:
# 4169
 is_zero_terminated=0;
goto _LL6F3;}_LL6F3:;}
# 4173
{void*_tmp80E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp937);void*_tmp80F=_tmp80E;struct Cyc_Absyn_Exp*_tmp81A;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp80F)->tag == 0){_LL700: _LL701:
 goto _LL6FF;}else{_LL702: _tmp81A=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp80F)->f1;_LL703: {
# 4176
struct _RegionHandle _tmp810=_new_region("temp");struct _RegionHandle*temp=& _tmp810;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp811=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp811,0,_tmp81A);}
# 4180
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp81A,te,cvtenv);
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp81A))
({void*_tmp812=0;Cyc_Tcutil_terr(loc,({const char*_tmp813="pointer bounds expression is not an unsigned int";_tag_dyneither(_tmp813,sizeof(char),49);}),_tag_dyneither(_tmp812,sizeof(void*),0));});{
struct _tuple14 _tmp814=Cyc_Evexp_eval_const_uint_exp(_tmp81A);struct _tuple14 _tmp815=_tmp814;unsigned int _tmp819;int _tmp818;_LL705: _tmp819=_tmp815.f1;_tmp818=_tmp815.f2;_LL706:;
if(is_zero_terminated  && (!_tmp818  || _tmp819 < 1))
({void*_tmp816=0;Cyc_Tcutil_terr(loc,({const char*_tmp817="zero-terminated pointer cannot point to empty sequence";_tag_dyneither(_tmp817,sizeof(char),55);}),_tag_dyneither(_tmp816,sizeof(void*),0));});
_npop_handler(0);goto _LL6FF;};
# 4176
;_pop_region(temp);}}_LL6FF:;}
# 4188
goto _LL66E;};}case 19: _LL681: _tmp93C=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp789)->f1;_LL682:
# 4190
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp93C,1,1);goto _LL66E;case 28: _LL683: _LL684:
 goto _LL66E;case 18: _LL685: _tmp93D=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp789)->f1;_LL686: {
# 4196
struct _RegionHandle _tmp81B=_new_region("temp");struct _RegionHandle*temp=& _tmp81B;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp81C=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp81C,0,_tmp93D);}
# 4200
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp93D))
({void*_tmp81D=0;Cyc_Tcutil_terr(loc,({const char*_tmp81E="valueof_t requires an int expression";_tag_dyneither(_tmp81E,sizeof(char),37);}),_tag_dyneither(_tmp81D,sizeof(void*),0));});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp93D,te,cvtenv);
_npop_handler(0);goto _LL66E;
# 4196
;_pop_region(temp);}case 27: _LL687: _tmp93E=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp789)->f1;_LL688: {
# 4208
struct _RegionHandle _tmp81F=_new_region("temp");struct _RegionHandle*temp=& _tmp81F;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp820=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp820,0,_tmp93E);}
# 4212
_npop_handler(0);goto _LL66E;
# 4208
;_pop_region(temp);}case 6: _LL689: _LL68A:
# 4213
 goto _LL68C;case 7: _LL68B: _LL68C:
 goto _LL66E;case 8: _LL68D: _tmp943=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp789)->f1).elt_type;_tmp942=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp789)->f1).tq;_tmp941=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp789)->f1).num_elts;_tmp940=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp789)->f1).zero_term;_tmp93F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp789)->f1).zt_loc;_LL68E: {
# 4218
struct Cyc_Absyn_Exp*_tmp821=*_tmp941;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp943,1,allow_abs_aggr);
_tmp942->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp942->print_const,_tmp943);{
int is_zero_terminated;
{union Cyc_Absyn_Constraint*_tmp822=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp940);union Cyc_Absyn_Constraint*_tmp823=_tmp822;int _tmp827;switch((((union Cyc_Absyn_Constraint*)_tmp823)->Eq_constr).tag){case 3: _LL708: _LL709:
# 4225
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp940,Cyc_Absyn_false_conref);
is_zero_terminated=0;
# 4240 "tcutil.cyc"
goto _LL707;case 1: _LL70A: _tmp827=(int)(_tmp823->Eq_constr).val;if(_tmp827){_LL70B:
# 4243
 if(!Cyc_Tcutil_admits_zero(_tmp943))
({struct Cyc_String_pa_PrintArg_struct _tmp826;_tmp826.tag=0;_tmp826.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp943));({void*_tmp824[1]={& _tmp826};Cyc_Tcutil_terr(loc,({const char*_tmp825="cannot have a zero-terminated array of %s elements";_tag_dyneither(_tmp825,sizeof(char),51);}),_tag_dyneither(_tmp824,sizeof(void*),1));});});
is_zero_terminated=1;
goto _LL707;}else{goto _LL70C;}default: _LL70C: _LL70D:
# 4249
 is_zero_terminated=0;
goto _LL707;}_LL707:;}
# 4254
if(_tmp821 == 0){
# 4256
if(is_zero_terminated)
# 4258
*_tmp941=(_tmp821=Cyc_Absyn_uint_exp(1,0));else{
# 4261
({void*_tmp828=0;Cyc_Tcutil_warn(loc,({const char*_tmp829="array bound defaults to 1 here";_tag_dyneither(_tmp829,sizeof(char),31);}),_tag_dyneither(_tmp828,sizeof(void*),0));});
*_tmp941=(_tmp821=Cyc_Absyn_uint_exp(1,0));}}{
# 4265
struct _RegionHandle _tmp82A=_new_region("temp");struct _RegionHandle*temp=& _tmp82A;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp82B=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp82B,0,_tmp821);}
# 4269
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp821))
({void*_tmp82C=0;Cyc_Tcutil_terr(loc,({const char*_tmp82D="array bounds expression is not an unsigned int";_tag_dyneither(_tmp82D,sizeof(char),47);}),_tag_dyneither(_tmp82C,sizeof(void*),0));});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp821,te,cvtenv);{
# 4276
struct _tuple14 _tmp82E=Cyc_Evexp_eval_const_uint_exp(_tmp821);struct _tuple14 _tmp82F=_tmp82E;unsigned int _tmp835;int _tmp834;_LL70F: _tmp835=_tmp82F.f1;_tmp834=_tmp82F.f2;_LL710:;
# 4278
if((is_zero_terminated  && _tmp834) && _tmp835 < 1)
({void*_tmp830=0;Cyc_Tcutil_warn(loc,({const char*_tmp831="zero terminated array cannot have zero elements";_tag_dyneither(_tmp831,sizeof(char),48);}),_tag_dyneither(_tmp830,sizeof(void*),0));});
# 4281
if((_tmp834  && _tmp835 < 1) && Cyc_Cyclone_tovc_r){
({void*_tmp832=0;Cyc_Tcutil_warn(loc,({const char*_tmp833="arrays with 0 elements are not supported except with gcc -- changing to 1.";_tag_dyneither(_tmp833,sizeof(char),75);}),_tag_dyneither(_tmp832,sizeof(void*),0));});
*_tmp941=Cyc_Absyn_uint_exp(1,0);}
# 4285
_npop_handler(0);goto _LL66E;};
# 4265
;_pop_region(temp);};};}case 9: _LL68F: _tmp950=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp789)->f1).tvars;_tmp94F=(void**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp789)->f1).effect;_tmp94E=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp789)->f1).ret_tqual;_tmp94D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp789)->f1).ret_typ;_tmp94C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp789)->f1).args;_tmp94B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp789)->f1).c_varargs;_tmp94A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp789)->f1).cyc_varargs;_tmp949=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp789)->f1).rgn_po;_tmp948=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp789)->f1).attributes;_tmp947=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp789)->f1).requires_clause;_tmp946=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp789)->f1).requires_relns;_tmp945=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp789)->f1).ensures_clause;_tmp944=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp789)->f1).ensures_relns;_LL690: {
# 4292
int num_convs=0;
int seen_cdecl=0;
int seen_stdcall=0;
int seen_fastcall=0;
int seen_format=0;
enum Cyc_Absyn_Format_Type ft=Cyc_Absyn_Printf_ft;
int fmt_desc_arg=-1;
int fmt_arg_start=-1;
for(0;_tmp948 != 0;_tmp948=_tmp948->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp948->hd))
({struct Cyc_String_pa_PrintArg_struct _tmp838;_tmp838.tag=0;_tmp838.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp948->hd));({void*_tmp836[1]={& _tmp838};Cyc_Tcutil_terr(loc,({const char*_tmp837="bad function type attribute %s";_tag_dyneither(_tmp837,sizeof(char),31);}),_tag_dyneither(_tmp836,sizeof(void*),1));});});{
void*_tmp839=(void*)_tmp948->hd;void*_tmp83A=_tmp839;enum Cyc_Absyn_Format_Type _tmp83F;int _tmp83E;int _tmp83D;switch(*((int*)_tmp83A)){case 1: _LL712: _LL713:
# 4305
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL711;case 2: _LL714: _LL715:
# 4307
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL711;case 3: _LL716: _LL717:
# 4309
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL711;case 19: _LL718: _tmp83F=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp83A)->f1;_tmp83E=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp83A)->f2;_tmp83D=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp83A)->f3;_LL719:
# 4311
 if(!seen_format){
seen_format=1;
ft=_tmp83F;
fmt_desc_arg=_tmp83E;
fmt_arg_start=_tmp83D;}else{
# 4317
({void*_tmp83B=0;Cyc_Tcutil_terr(loc,({const char*_tmp83C="function can't have multiple format attributes";_tag_dyneither(_tmp83C,sizeof(char),47);}),_tag_dyneither(_tmp83B,sizeof(void*),0));});}
goto _LL711;default: _LL71A: _LL71B:
 goto _LL711;}_LL711:;};}
# 4322
if(num_convs > 1)
({void*_tmp840=0;Cyc_Tcutil_terr(loc,({const char*_tmp841="function can't have multiple calling conventions";_tag_dyneither(_tmp841,sizeof(char),49);}),_tag_dyneither(_tmp840,sizeof(void*),0));});
# 4327
Cyc_Tcutil_check_unique_tvars(loc,*_tmp950);
{struct Cyc_List_List*b=*_tmp950;for(0;b != 0;b=b->tl){
((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{
void*_tmp842=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp843=_tmp842;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp843)->tag == 0){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp843)->f1)->kind == Cyc_Absyn_MemKind){_LL71D: _LL71E:
# 4333
({struct Cyc_String_pa_PrintArg_struct _tmp846;_tmp846.tag=0;_tmp846.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name);({void*_tmp844[1]={& _tmp846};Cyc_Tcutil_terr(loc,({const char*_tmp845="function attempts to abstract Mem type variable %s";_tag_dyneither(_tmp845,sizeof(char),51);}),_tag_dyneither(_tmp844,sizeof(void*),1));});});
goto _LL71C;}else{goto _LL71F;}}else{_LL71F: _LL720:
 goto _LL71C;}_LL71C:;};}}{
# 4341
struct _RegionHandle _tmp847=_new_region("newr");struct _RegionHandle*newr=& _tmp847;_push_region(newr);{
struct Cyc_Tcutil_CVTEnv _tmp848=({struct Cyc_Tcutil_CVTEnv _tmp8DB;_tmp8DB.r=newr;_tmp8DB.kind_env=cvtenv.kind_env;_tmp8DB.free_vars=0;_tmp8DB.free_evars=0;_tmp8DB.generalize_evars=cvtenv.generalize_evars;_tmp8DB.fn_result=1;_tmp8DB;});
# 4347
_tmp848=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp848,& Cyc_Tcutil_tmk,_tmp94D,1,1);
_tmp94E->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp94E->print_const,_tmp94D);
_tmp848.fn_result=0;
{struct Cyc_List_List*a=_tmp94C;for(0;a != 0;a=a->tl){
struct _tuple10*_tmp849=(struct _tuple10*)a->hd;
void*_tmp84A=(*_tmp849).f3;
_tmp848=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp848,& Cyc_Tcutil_tmk,_tmp84A,1,1);{
int _tmp84B=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp849).f2).print_const,_tmp84A);
((*_tmp849).f2).real_const=_tmp84B;
# 4358
if(Cyc_Tcutil_is_array(_tmp84A)){
# 4361
void*_tmp84C=Cyc_Absyn_new_evar(0,0);
_tmp848=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp848,& Cyc_Tcutil_rk,_tmp84C,1,1);
(*_tmp849).f3=Cyc_Tcutil_promote_array(_tmp84A,_tmp84C,0);}};}}
# 4368
if(_tmp94A != 0){
if(_tmp94B)({void*_tmp84D=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp84E="both c_vararg and cyc_vararg";_tag_dyneither(_tmp84E,sizeof(char),29);}),_tag_dyneither(_tmp84D,sizeof(void*),0));});{
struct Cyc_Absyn_VarargInfo _tmp84F=*_tmp94A;struct Cyc_Absyn_VarargInfo _tmp850=_tmp84F;void*_tmp863;int _tmp862;_LL722: _tmp863=_tmp850.type;_tmp862=_tmp850.inject;_LL723:;
_tmp848=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp848,& Cyc_Tcutil_tmk,_tmp863,1,1);
(_tmp94A->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmp94A->tq).print_const,_tmp863);
# 4374
if(_tmp862){
void*_tmp851=Cyc_Tcutil_compress(_tmp863);void*_tmp852=_tmp851;void*_tmp861;union Cyc_Absyn_Constraint*_tmp860;union Cyc_Absyn_Constraint*_tmp85F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp852)->tag == 5){_LL725: _tmp861=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp852)->f1).elt_typ;_tmp860=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp852)->f1).ptr_atts).bounds;_tmp85F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp852)->f1).ptr_atts).zero_term;_LL726:
# 4377
{void*_tmp853=Cyc_Tcutil_compress(_tmp861);void*_tmp854=_tmp853;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp854)->tag == 3){_LL72A: _LL72B:
# 4379
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp85F))
({void*_tmp855=0;Cyc_Tcutil_terr(loc,({const char*_tmp856="can't inject into a zeroterm pointer";_tag_dyneither(_tmp856,sizeof(char),37);}),_tag_dyneither(_tmp855,sizeof(void*),0));});
{void*_tmp857=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp860);void*_tmp858=_tmp857;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp858)->tag == 0){_LL72F: _LL730:
# 4383
({void*_tmp859=0;Cyc_Tcutil_terr(loc,({const char*_tmp85A="can't inject into a fat pointer to datatype";_tag_dyneither(_tmp85A,sizeof(char),44);}),_tag_dyneither(_tmp859,sizeof(void*),0));});
goto _LL72E;}else{_LL731: _LL732:
 goto _LL72E;}_LL72E:;}
# 4387
goto _LL729;}else{_LL72C: _LL72D:
({void*_tmp85B=0;Cyc_Tcutil_terr(loc,({const char*_tmp85C="can't inject a non-datatype type";_tag_dyneither(_tmp85C,sizeof(char),33);}),_tag_dyneither(_tmp85B,sizeof(void*),0));});goto _LL729;}_LL729:;}
# 4390
goto _LL724;}else{_LL727: _LL728:
({void*_tmp85D=0;Cyc_Tcutil_terr(loc,({const char*_tmp85E="expecting a datatype pointer type";_tag_dyneither(_tmp85E,sizeof(char),34);}),_tag_dyneither(_tmp85D,sizeof(void*),0));});goto _LL724;}_LL724:;}};}
# 4396
if(seen_format){
int _tmp864=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp94C);
if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmp864) || fmt_arg_start < 0) || 
# 4400
(_tmp94A == 0  && !_tmp94B) && fmt_arg_start != 0) || 
(_tmp94A != 0  || _tmp94B) && fmt_arg_start != _tmp864 + 1)
# 4403
({void*_tmp865=0;Cyc_Tcutil_terr(loc,({const char*_tmp866="bad format descriptor";_tag_dyneither(_tmp866,sizeof(char),22);}),_tag_dyneither(_tmp865,sizeof(void*),0));});else{
# 4406
struct _tuple10 _tmp867=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp94C,fmt_desc_arg - 1);struct _tuple10 _tmp868=_tmp867;void*_tmp880;_LL734: _tmp880=_tmp868.f3;_LL735:;
# 4408
{void*_tmp869=Cyc_Tcutil_compress(_tmp880);void*_tmp86A=_tmp869;void*_tmp878;union Cyc_Absyn_Constraint*_tmp877;union Cyc_Absyn_Constraint*_tmp876;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp86A)->tag == 5){_LL737: _tmp878=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp86A)->f1).elt_typ;_tmp877=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp86A)->f1).ptr_atts).bounds;_tmp876=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp86A)->f1).ptr_atts).zero_term;_LL738:
# 4411
{struct _tuple0 _tmp86B=({struct _tuple0 _tmp873;_tmp873.f1=Cyc_Tcutil_compress(_tmp878);_tmp873.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp877);_tmp873;});struct _tuple0 _tmp86C=_tmp86B;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp86C.f1)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp86C.f1)->f2 == Cyc_Absyn_Char_sz){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp86C.f2)->tag == 0){_LL73C: _LL73D:
# 4413
 if(_tmp94B)
({void*_tmp86D=0;Cyc_Tcutil_terr(loc,({const char*_tmp86E="format descriptor is not a char * type";_tag_dyneither(_tmp86E,sizeof(char),39);}),_tag_dyneither(_tmp86D,sizeof(void*),0));});
goto _LL73B;}else{_LL73E: _LL73F:
# 4417
 if(!_tmp94B)
({void*_tmp86F=0;Cyc_Tcutil_terr(loc,({const char*_tmp870="format descriptor is not a char ? type";_tag_dyneither(_tmp870,sizeof(char),39);}),_tag_dyneither(_tmp86F,sizeof(void*),0));});
goto _LL73B;}}else{goto _LL740;}}else{_LL740: _LL741:
# 4421
({void*_tmp871=0;Cyc_Tcutil_terr(loc,({const char*_tmp872="format descriptor is not a string type";_tag_dyneither(_tmp872,sizeof(char),39);}),_tag_dyneither(_tmp871,sizeof(void*),0));});
goto _LL73B;}_LL73B:;}
# 4424
goto _LL736;}else{_LL739: _LL73A:
({void*_tmp874=0;Cyc_Tcutil_terr(loc,({const char*_tmp875="format descriptor is not a string type";_tag_dyneither(_tmp875,sizeof(char),39);}),_tag_dyneither(_tmp874,sizeof(void*),0));});goto _LL736;}_LL736:;}
# 4427
if(fmt_arg_start != 0  && !_tmp94B){
# 4431
int problem;
{struct _tuple31 _tmp879=({struct _tuple31 _tmp87D;_tmp87D.f1=ft;_tmp87D.f2=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp94A))->type));_tmp87D;});struct _tuple31 _tmp87A=_tmp879;struct Cyc_Absyn_Datatypedecl*_tmp87C;struct Cyc_Absyn_Datatypedecl*_tmp87B;if(_tmp87A.f1 == Cyc_Absyn_Printf_ft){if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp87A.f2)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp87A.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL743: _tmp87B=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp87A.f2)->f1).datatype_info).KnownDatatype).val;_LL744:
# 4434
 problem=Cyc_Absyn_qvar_cmp(_tmp87B->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL742;}else{goto _LL747;}}else{goto _LL747;}}else{if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp87A.f2)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp87A.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL745: _tmp87C=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp87A.f2)->f1).datatype_info).KnownDatatype).val;_LL746:
# 4436
 problem=Cyc_Absyn_qvar_cmp(_tmp87C->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL742;}else{goto _LL747;}}else{_LL747: _LL748:
# 4438
 problem=1;goto _LL742;}}_LL742:;}
# 4440
if(problem)
({void*_tmp87E=0;Cyc_Tcutil_terr(loc,({const char*_tmp87F="format attribute and vararg types don't match";_tag_dyneither(_tmp87F,sizeof(char),46);}),_tag_dyneither(_tmp87E,sizeof(void*),0));});}}}
# 4448
{struct Cyc_List_List*rpo=_tmp949;for(0;rpo != 0;rpo=rpo->tl){
struct _tuple0*_tmp881=(struct _tuple0*)rpo->hd;struct _tuple0*_tmp882=_tmp881;void*_tmp884;void*_tmp883;_LL74A: _tmp884=_tmp882->f1;_tmp883=_tmp882->f2;_LL74B:;
_tmp848=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp848,& Cyc_Tcutil_ek,_tmp884,1,1);
_tmp848=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp848,& Cyc_Tcutil_trk,_tmp883,1,1);}}{
# 4456
struct Cyc_Tcenv_Fenv*_tmp885=Cyc_Tcenv_bogus_fenv(newr,_tmp94D,_tmp94C);
struct Cyc_Tcenv_Tenv*_tmp886=({struct Cyc_Tcenv_Tenv*_tmp8DA=_region_malloc(newr,sizeof(*_tmp8DA));_tmp8DA->ns=te->ns;_tmp8DA->ae=te->ae;_tmp8DA->le=_tmp885;_tmp8DA->allow_valueof=1;_tmp8DA->in_extern_c_include=te->in_extern_c_include;_tmp8DA;});
struct _tuple30 _tmp887=Cyc_Tcutil_check_clause(loc,_tmp886,_tmp848,({const char*_tmp8D9="@requires";_tag_dyneither(_tmp8D9,sizeof(char),10);}),_tmp947);struct _tuple30 _tmp888=_tmp887;struct Cyc_Tcutil_CVTEnv _tmp8D8;struct Cyc_List_List*_tmp8D7;_LL74D: _tmp8D8=_tmp888.f1;_tmp8D7=_tmp888.f2;_LL74E:;
_tmp848=_tmp8D8;
*_tmp946=_tmp8D7;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp94C,_tmp8D7);{
# 4463
struct _dyneither_ptr*_tmp889=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"return_value",sizeof(char),13);
struct Cyc_Absyn_Vardecl*_tmp88A=({struct Cyc_Absyn_Vardecl*_tmp8D4=_cycalloc(sizeof(*_tmp8D4));_tmp8D4->sc=Cyc_Absyn_Public;_tmp8D4->name=({struct _tuple2*_tmp8D5=_cycalloc(sizeof(*_tmp8D5));_tmp8D5->f1=Cyc_Absyn_Loc_n;_tmp8D5->f2=_tmp889;_tmp8D5;});_tmp8D4->tq=
Cyc_Absyn_empty_tqual(0);_tmp8D4->type=_tmp94D;_tmp8D4->initializer=0;_tmp8D4->rgn=
Cyc_Tcenv_curr_rgn(_tmp886);_tmp8D4->attributes=0;_tmp8D4->escapes=0;_tmp8D4;});
# 4468
_tmp886=Cyc_Tcenv_add_local_var(newr,0,_tmp886,_tmp88A);{
struct _tuple30 _tmp88B=Cyc_Tcutil_check_clause(loc,_tmp886,_tmp848,({const char*_tmp8D3="@ensures";_tag_dyneither(_tmp8D3,sizeof(char),9);}),_tmp945);struct _tuple30 _tmp88C=_tmp88B;struct Cyc_Tcutil_CVTEnv _tmp8D2;struct Cyc_List_List*_tmp8D1;_LL750: _tmp8D2=_tmp88C.f1;_tmp8D1=_tmp88C.f2;_LL751:;
_tmp848=_tmp8D2;
*_tmp944=_tmp8D1;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp94C,_tmp8D1);
# 4474
if(*_tmp94F != 0)
_tmp848=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp848,& Cyc_Tcutil_ek,(void*)_check_null(*_tmp94F),1,1);else{
# 4477
struct Cyc_List_List*effect=0;
# 4482
{struct Cyc_List_List*tvs=_tmp848.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp88D=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp88E=_tmp88D;struct Cyc_Absyn_Tvar*_tmp8A7;int _tmp8A6;_LL753: _tmp8A7=_tmp88E.f1;_tmp8A6=_tmp88E.f2;_LL754:;
if(!_tmp8A6)continue;{
void*_tmp88F=Cyc_Absyn_compress_kb(_tmp8A7->kind);void*_tmp890=_tmp88F;struct Cyc_Core_Opt**_tmp8A5;struct Cyc_Absyn_Kind*_tmp8A4;struct Cyc_Core_Opt**_tmp8A3;struct Cyc_Core_Opt**_tmp8A2;struct Cyc_Absyn_Kind*_tmp8A1;switch(*((int*)_tmp890)){case 2: _LL756: _tmp8A2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp890)->f1;_tmp8A1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp890)->f2;if(_tmp8A1->kind == Cyc_Absyn_RgnKind){_LL757:
# 4488
 if(_tmp8A1->aliasqual == Cyc_Absyn_Top){
*_tmp8A2=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);_tmp8A4=_tmp8A1;goto _LL759;}
# 4491
*_tmp8A2=Cyc_Tcutil_kind_to_bound_opt(_tmp8A1);_tmp8A4=_tmp8A1;goto _LL759;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp890)->f2)->kind){case Cyc_Absyn_IntKind: _LL75A: _LL75B:
# 4494
 goto _LL75D;case Cyc_Absyn_EffKind: _LL75E: _tmp8A3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp890)->f1;_LL75F:
# 4497
*_tmp8A3=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);goto _LL761;default: goto _LL764;}}case 0: _LL758: _tmp8A4=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp890)->f1;if(_tmp8A4->kind == Cyc_Absyn_RgnKind){_LL759:
# 4493
 effect=({struct Cyc_List_List*_tmp891=_cycalloc(sizeof(*_tmp891));_tmp891->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp892=_cycalloc(sizeof(*_tmp892));_tmp892[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp893;_tmp893.tag=23;_tmp893.f1=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp894=_cycalloc(sizeof(*_tmp894));_tmp894[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp895;_tmp895.tag=2;_tmp895.f1=_tmp8A7;_tmp895;});_tmp894;});_tmp893;});_tmp892;});_tmp891->tl=effect;_tmp891;});goto _LL755;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp890)->f1)->kind){case Cyc_Absyn_IntKind: _LL75C: _LL75D:
# 4495
 goto _LL755;case Cyc_Absyn_EffKind: _LL760: _LL761:
# 4499
 effect=({struct Cyc_List_List*_tmp896=_cycalloc(sizeof(*_tmp896));_tmp896->hd=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp897=_cycalloc(sizeof(*_tmp897));_tmp897[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp898;_tmp898.tag=2;_tmp898.f1=_tmp8A7;_tmp898;});_tmp897;});_tmp896->tl=effect;_tmp896;});goto _LL755;default: _LL764: _LL765:
# 4504
 effect=({struct Cyc_List_List*_tmp89C=_cycalloc(sizeof(*_tmp89C));_tmp89C->hd=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp89D=_cycalloc(sizeof(*_tmp89D));_tmp89D[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp89E;_tmp89E.tag=25;_tmp89E.f1=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp89F=_cycalloc(sizeof(*_tmp89F));_tmp89F[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8A0;_tmp8A0.tag=2;_tmp8A0.f1=_tmp8A7;_tmp8A0;});_tmp89F;});_tmp89E;});_tmp89D;});_tmp89C->tl=effect;_tmp89C;});goto _LL755;}}default: _LL762: _tmp8A5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp890)->f1;_LL763:
# 4501
*_tmp8A5=({struct Cyc_Core_Opt*_tmp899=_cycalloc(sizeof(*_tmp899));_tmp899->v=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp89A=_cycalloc(sizeof(*_tmp89A));_tmp89A[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp89B;_tmp89B.tag=2;_tmp89B.f1=0;_tmp89B.f2=& Cyc_Tcutil_ak;_tmp89B;});_tmp89A;});_tmp899;});goto _LL765;}_LL755:;};}}
# 4508
{struct Cyc_List_List*ts=_tmp848.free_evars;for(0;ts != 0;ts=ts->tl){
struct _tuple29 _tmp8A8=*((struct _tuple29*)ts->hd);struct _tuple29 _tmp8A9=_tmp8A8;void*_tmp8B4;int _tmp8B3;_LL767: _tmp8B4=_tmp8A9.f1;_tmp8B3=_tmp8A9.f2;_LL768:;
if(!_tmp8B3)continue;{
struct Cyc_Absyn_Kind*_tmp8AA=Cyc_Tcutil_typ_kind(_tmp8B4);struct Cyc_Absyn_Kind*_tmp8AB=_tmp8AA;switch(((struct Cyc_Absyn_Kind*)_tmp8AB)->kind){case Cyc_Absyn_RgnKind: _LL76A: _LL76B:
# 4513
 effect=({struct Cyc_List_List*_tmp8AC=_cycalloc(sizeof(*_tmp8AC));_tmp8AC->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp8AD=_cycalloc(sizeof(*_tmp8AD));_tmp8AD[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp8AE;_tmp8AE.tag=23;_tmp8AE.f1=_tmp8B4;_tmp8AE;});_tmp8AD;});_tmp8AC->tl=effect;_tmp8AC;});goto _LL769;case Cyc_Absyn_EffKind: _LL76C: _LL76D:
# 4515
 effect=({struct Cyc_List_List*_tmp8AF=_cycalloc(sizeof(*_tmp8AF));_tmp8AF->hd=_tmp8B4;_tmp8AF->tl=effect;_tmp8AF;});goto _LL769;case Cyc_Absyn_IntKind: _LL76E: _LL76F:
 goto _LL769;default: _LL770: _LL771:
# 4518
 effect=({struct Cyc_List_List*_tmp8B0=_cycalloc(sizeof(*_tmp8B0));_tmp8B0->hd=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp8B1=_cycalloc(sizeof(*_tmp8B1));_tmp8B1[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp8B2;_tmp8B2.tag=25;_tmp8B2.f1=_tmp8B4;_tmp8B2;});_tmp8B1;});_tmp8B0->tl=effect;_tmp8B0;});goto _LL769;}_LL769:;};}}
# 4521
*_tmp94F=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp8B5=_cycalloc(sizeof(*_tmp8B5));_tmp8B5[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp8B6;_tmp8B6.tag=24;_tmp8B6.f1=effect;_tmp8B6;});_tmp8B5;});}
# 4527
if(*_tmp950 != 0){
struct Cyc_List_List*bs=*_tmp950;for(0;bs != 0;bs=bs->tl){
void*_tmp8B7=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);void*_tmp8B8=_tmp8B7;struct Cyc_Core_Opt**_tmp8C8;struct Cyc_Absyn_Kind*_tmp8C7;struct Cyc_Core_Opt**_tmp8C6;struct Cyc_Core_Opt**_tmp8C5;struct Cyc_Core_Opt**_tmp8C4;struct Cyc_Core_Opt**_tmp8C3;struct Cyc_Core_Opt**_tmp8C2;struct Cyc_Core_Opt**_tmp8C1;struct Cyc_Core_Opt**_tmp8C0;struct Cyc_Core_Opt**_tmp8BF;struct Cyc_Core_Opt**_tmp8BE;switch(*((int*)_tmp8B8)){case 1: _LL773: _tmp8BE=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp8B8)->f1;_LL774:
# 4531
({struct Cyc_String_pa_PrintArg_struct _tmp8BB;_tmp8BB.tag=0;_tmp8BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name);({void*_tmp8B9[1]={& _tmp8BB};Cyc_Tcutil_warn(loc,({const char*_tmp8BA="Type variable %s unconstrained, assuming boxed";_tag_dyneither(_tmp8BA,sizeof(char),47);}),_tag_dyneither(_tmp8B9,sizeof(void*),1));});});
# 4533
_tmp8BF=_tmp8BE;goto _LL776;case 2: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B8)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B8)->f2)->aliasqual == Cyc_Absyn_Top){_LL775: _tmp8BF=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B8)->f1;_LL776:
 _tmp8C0=_tmp8BF;goto _LL778;}else{goto _LL785;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B8)->f2)->aliasqual){case Cyc_Absyn_Top: _LL777: _tmp8C0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B8)->f1;_LL778:
 _tmp8C1=_tmp8C0;goto _LL77A;case Cyc_Absyn_Aliasable: _LL779: _tmp8C1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B8)->f1;_LL77A:
 _tmp8C3=_tmp8C1;goto _LL77C;default: _LL77F: _tmp8C2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B8)->f1;_LL780:
# 4540
 _tmp8C5=_tmp8C2;goto _LL782;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B8)->f2)->aliasqual){case Cyc_Absyn_Top: _LL77B: _tmp8C3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B8)->f1;_LL77C:
# 4537
 _tmp8C4=_tmp8C3;goto _LL77E;case Cyc_Absyn_Aliasable: _LL77D: _tmp8C4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B8)->f1;_LL77E:
# 4539
*_tmp8C4=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL772;default: _LL781: _tmp8C5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B8)->f1;_LL782:
# 4542
*_tmp8C5=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL772;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B8)->f2)->aliasqual == Cyc_Absyn_Top){_LL783: _tmp8C6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B8)->f1;_LL784:
# 4544
*_tmp8C6=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL772;}else{goto _LL785;}default: _LL785: _tmp8C8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B8)->f1;_tmp8C7=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B8)->f2;_LL786:
# 4546
*_tmp8C8=Cyc_Tcutil_kind_to_bound_opt(_tmp8C7);goto _LL772;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp8B8)->f1)->kind == Cyc_Absyn_MemKind){_LL787: _LL788:
# 4548
({void*_tmp8BC=0;Cyc_Tcutil_terr(loc,({const char*_tmp8BD="functions can't abstract M types";_tag_dyneither(_tmp8BD,sizeof(char),33);}),_tag_dyneither(_tmp8BC,sizeof(void*),0));});goto _LL772;}else{_LL789: _LL78A:
 goto _LL772;}}_LL772:;}}
# 4553
cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,_tmp848.kind_env,*_tmp950);
_tmp848.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(_tmp848.r,_tmp848.free_vars,*_tmp950);
# 4556
{struct Cyc_List_List*tvs=_tmp848.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp8C9=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp8CA=_tmp8C9;struct Cyc_Absyn_Tvar*_tmp8CC;int _tmp8CB;_LL78C: _tmp8CC=_tmp8CA.f1;_tmp8CB=_tmp8CA.f2;_LL78D:;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp8CC,_tmp8CB);}}{
# 4561
struct Cyc_List_List*evs=_tmp848.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmp8CD=*((struct _tuple29*)evs->hd);struct _tuple29 _tmp8CE=_tmp8CD;void*_tmp8D0;int _tmp8CF;_LL78F: _tmp8D0=_tmp8CE.f1;_tmp8CF=_tmp8CE.f2;_LL790:;
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmp8D0,_tmp8CF);}};};};};}
# 4566
_npop_handler(0);goto _LL66E;
# 4341
;_pop_region(newr);};}case 10: _LL691: _tmp951=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp789)->f1;_LL692:
# 4569
 for(0;_tmp951 != 0;_tmp951=_tmp951->tl){
struct _tuple12*_tmp8DC=(struct _tuple12*)_tmp951->hd;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmp8DC).f2,1,0);
((*_tmp8DC).f1).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp8DC).f1).print_const,(*_tmp8DC).f2);}
# 4575
goto _LL66E;case 12: _LL693: _tmp953=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp789)->f1;_tmp952=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp789)->f2;_LL694: {
# 4579
struct _RegionHandle _tmp8DD=_new_region("aprev_rgn");struct _RegionHandle*aprev_rgn=& _tmp8DD;_push_region(aprev_rgn);{
struct Cyc_List_List*prev_fields=0;
for(0;_tmp952 != 0;_tmp952=_tmp952->tl){
struct Cyc_Absyn_Aggrfield*_tmp8DE=(struct Cyc_Absyn_Aggrfield*)_tmp952->hd;struct Cyc_Absyn_Aggrfield*_tmp8DF=_tmp8DE;struct _dyneither_ptr*_tmp8F1;struct Cyc_Absyn_Tqual*_tmp8F0;void*_tmp8EF;struct Cyc_Absyn_Exp*_tmp8EE;struct Cyc_List_List*_tmp8ED;struct Cyc_Absyn_Exp*_tmp8EC;_LL792: _tmp8F1=_tmp8DF->name;_tmp8F0=(struct Cyc_Absyn_Tqual*)& _tmp8DF->tq;_tmp8EF=_tmp8DF->type;_tmp8EE=_tmp8DF->width;_tmp8ED=_tmp8DF->attributes;_tmp8EC=_tmp8DF->requires_clause;_LL793:;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp8F1))
({struct Cyc_String_pa_PrintArg_struct _tmp8E2;_tmp8E2.tag=0;_tmp8E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp8F1);({void*_tmp8E0[1]={& _tmp8E2};Cyc_Tcutil_terr(loc,({const char*_tmp8E1="duplicate field %s";_tag_dyneither(_tmp8E1,sizeof(char),19);}),_tag_dyneither(_tmp8E0,sizeof(void*),1));});});
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp8F1,({const char*_tmp8E3="";_tag_dyneither(_tmp8E3,sizeof(char),1);}))!= 0)
prev_fields=({struct Cyc_List_List*_tmp8E4=_region_malloc(aprev_rgn,sizeof(*_tmp8E4));_tmp8E4->hd=_tmp8F1;_tmp8E4->tl=prev_fields;_tmp8E4;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp8EF,1,0);
_tmp8F0->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp8F0->print_const,_tmp8EF);
# 4593
Cyc_Tcutil_check_bitfield(loc,te,_tmp8EF,_tmp8EE,_tmp8F1);
Cyc_Tcutil_check_field_atts(loc,_tmp8F1,_tmp8ED);
if(_tmp8EC != 0){
# 4597
if(_tmp953 != Cyc_Absyn_UnionA)
({void*_tmp8E5=0;Cyc_Tcutil_terr(loc,({const char*_tmp8E6="@requires clause is only allowed on union members";_tag_dyneither(_tmp8E6,sizeof(char),50);}),_tag_dyneither(_tmp8E5,sizeof(void*),0));});{
struct _RegionHandle _tmp8E7=_new_region("temp");struct _RegionHandle*temp=& _tmp8E7;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp8E8=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp8E8,0,_tmp8EC);}
# 4603
if(!Cyc_Tcutil_is_integral(_tmp8EC))
({struct Cyc_String_pa_PrintArg_struct _tmp8EB;_tmp8EB.tag=0;_tmp8EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmp8EC->topt)));({void*_tmp8E9[1]={& _tmp8EB};Cyc_Tcutil_terr(loc,({const char*_tmp8EA="@requires clause has type %s instead of integral type";_tag_dyneither(_tmp8EA,sizeof(char),54);}),_tag_dyneither(_tmp8E9,sizeof(void*),1));});});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp8EC,te,cvtenv);
# 4599
;_pop_region(temp);};}}}
# 4610
_npop_handler(0);goto _LL66E;
# 4579
;_pop_region(aprev_rgn);}case 11: _LL695: _tmp955=(union Cyc_Absyn_AggrInfoU*)&(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp789)->f1).aggr_info;_tmp954=(struct Cyc_List_List**)&(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp789)->f1).targs;_LL696:
# 4613
{union Cyc_Absyn_AggrInfoU _tmp8F2=*_tmp955;union Cyc_Absyn_AggrInfoU _tmp8F3=_tmp8F2;struct Cyc_Absyn_Aggrdecl*_tmp918;enum Cyc_Absyn_AggrKind _tmp917;struct _tuple2*_tmp916;struct Cyc_Core_Opt*_tmp915;if((_tmp8F3.UnknownAggr).tag == 1){_LL795: _tmp917=((_tmp8F3.UnknownAggr).val).f1;_tmp916=((_tmp8F3.UnknownAggr).val).f2;_tmp915=((_tmp8F3.UnknownAggr).val).f3;_LL796: {
# 4615
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmp8F4;_push_handler(& _tmp8F4);{int _tmp8F6=0;if(setjmp(_tmp8F4.handler))_tmp8F6=1;if(!_tmp8F6){
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp916);{
struct Cyc_Absyn_Aggrdecl*_tmp8F7=*adp;
if(_tmp8F7->kind != _tmp917){
if(_tmp8F7->kind == Cyc_Absyn_StructA)
({struct Cyc_String_pa_PrintArg_struct _tmp8FB;_tmp8FB.tag=0;_tmp8FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp916));({struct Cyc_String_pa_PrintArg_struct _tmp8FA;_tmp8FA.tag=0;_tmp8FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4621
Cyc_Absynpp_qvar2string(_tmp916));({void*_tmp8F8[2]={& _tmp8FA,& _tmp8FB};Cyc_Tcutil_terr(loc,({const char*_tmp8F9="expecting struct %s instead of union %s";_tag_dyneither(_tmp8F9,sizeof(char),40);}),_tag_dyneither(_tmp8F8,sizeof(void*),2));});});});else{
# 4624
({struct Cyc_String_pa_PrintArg_struct _tmp8FF;_tmp8FF.tag=0;_tmp8FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp916));({struct Cyc_String_pa_PrintArg_struct _tmp8FE;_tmp8FE.tag=0;_tmp8FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4624
Cyc_Absynpp_qvar2string(_tmp916));({void*_tmp8FC[2]={& _tmp8FE,& _tmp8FF};Cyc_Tcutil_terr(loc,({const char*_tmp8FD="expecting union %s instead of struct %s";_tag_dyneither(_tmp8FD,sizeof(char),40);}),_tag_dyneither(_tmp8FC,sizeof(void*),2));});});});}}
# 4627
if((unsigned int)_tmp915  && (int)_tmp915->v){
if(!((unsigned int)_tmp8F7->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp8F7->impl))->tagged)
({struct Cyc_String_pa_PrintArg_struct _tmp902;_tmp902.tag=0;_tmp902.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp916));({void*_tmp900[1]={& _tmp902};Cyc_Tcutil_terr(loc,({const char*_tmp901="@tagged qualfiers don't agree on union %s";_tag_dyneither(_tmp901,sizeof(char),42);}),_tag_dyneither(_tmp900,sizeof(void*),1));});});}
# 4633
*_tmp955=Cyc_Absyn_KnownAggr(adp);};
# 4617
;_pop_handler();}else{void*_tmp8F5=(void*)_exn_thrown;void*_tmp903=_tmp8F5;void*_tmp90A;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp903)->tag == Cyc_Dict_Absent){_LL79A: _LL79B: {
# 4637
struct Cyc_Tcenv_Genv*_tmp904=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp905=({struct Cyc_Absyn_Aggrdecl*_tmp909=_cycalloc(sizeof(*_tmp909));_tmp909->kind=_tmp917;_tmp909->sc=Cyc_Absyn_Extern;_tmp909->name=_tmp916;_tmp909->tvs=0;_tmp909->impl=0;_tmp909->attributes=0;_tmp909->expected_mem_kind=0;_tmp909;});
Cyc_Tc_tcAggrdecl(te,_tmp904,loc,_tmp905);
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp916);
*_tmp955=Cyc_Absyn_KnownAggr(adp);
# 4643
if(*_tmp954 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp908;_tmp908.tag=0;_tmp908.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp916));({void*_tmp906[1]={& _tmp908};Cyc_Tcutil_terr(loc,({const char*_tmp907="declare parameterized type %s before using";_tag_dyneither(_tmp907,sizeof(char),43);}),_tag_dyneither(_tmp906,sizeof(void*),1));});});
return cvtenv;}
# 4648
goto _LL799;}}else{_LL79C: _tmp90A=_tmp903;_LL79D:(int)_rethrow(_tmp90A);}_LL799:;}};}
# 4650
_tmp918=*adp;goto _LL798;}}else{_LL797: _tmp918=*(_tmp8F3.KnownAggr).val;_LL798: {
# 4652
struct Cyc_List_List*tvs=_tmp918->tvs;
struct Cyc_List_List*ts=*_tmp954;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Tvar*_tmp90B=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp90C=(void*)ts->hd;
# 4660
{struct _tuple0 _tmp90D=({struct _tuple0 _tmp910;_tmp910.f1=Cyc_Absyn_compress_kb(_tmp90B->kind);_tmp910.f2=_tmp90C;_tmp910;});struct _tuple0 _tmp90E=_tmp90D;struct Cyc_Absyn_Tvar*_tmp90F;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp90E.f1)->tag == 1){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp90E.f2)->tag == 2){_LL79F: _tmp90F=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp90E.f2)->f1;_LL7A0:
# 4662
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp90F);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp90F,1);
continue;}else{goto _LL7A1;}}else{_LL7A1: _LL7A2:
 goto _LL79E;}_LL79E:;}{
# 4667
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 4671
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp913;_tmp913.tag=0;_tmp913.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp918->name));({void*_tmp911[1]={& _tmp913};Cyc_Tcutil_terr(loc,({const char*_tmp912="too many parameters for type %s";_tag_dyneither(_tmp912,sizeof(char),32);}),_tag_dyneither(_tmp911,sizeof(void*),1));});});
if(tvs != 0){
# 4675
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp914=_cycalloc(sizeof(*_tmp914));_tmp914->hd=e;_tmp914->tl=hidden_ts;_tmp914;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);}
# 4682
*_tmp954=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp954,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4684
if((allow_abs_aggr  && _tmp918->impl == 0) && !
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,expected_kind))
# 4689
_tmp918->expected_mem_kind=1;}}_LL794:;}
# 4692
goto _LL66E;case 17: _LL697: _tmp959=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp789)->f1;_tmp958=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp789)->f2;_tmp957=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp789)->f3;_tmp956=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp789)->f4;_LL698: {
# 4695
struct Cyc_List_List*targs=*_tmp958;
# 4697
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmp919;_push_handler(& _tmp919);{int _tmp91B=0;if(setjmp(_tmp919.handler))_tmp91B=1;if(!_tmp91B){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp959);;_pop_handler();}else{void*_tmp91A=(void*)_exn_thrown;void*_tmp91C=_tmp91A;void*_tmp920;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp91C)->tag == Cyc_Dict_Absent){_LL7A4: _LL7A5:
# 4700
({struct Cyc_String_pa_PrintArg_struct _tmp91F;_tmp91F.tag=0;_tmp91F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp959));({void*_tmp91D[1]={& _tmp91F};Cyc_Tcutil_terr(loc,({const char*_tmp91E="unbound typedef name %s";_tag_dyneither(_tmp91E,sizeof(char),24);}),_tag_dyneither(_tmp91D,sizeof(void*),1));});});
return cvtenv;}else{_LL7A6: _tmp920=_tmp91C;_LL7A7:(int)_rethrow(_tmp920);}_LL7A3:;}};}
# 4703
*_tmp957=td;
# 4705
_tmp959[0]=(td->name)[0];{
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;
struct _RegionHandle _tmp921=_new_region("temp");struct _RegionHandle*temp=& _tmp921;_push_region(temp);{
struct Cyc_List_List*inst=0;
# 4711
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,td);
# 4716
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
inst=({struct Cyc_List_List*_tmp922=_region_malloc(temp,sizeof(*_tmp922));_tmp922->hd=({struct _tuple16*_tmp923=_region_malloc(temp,sizeof(*_tmp923));_tmp923->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp923->f2=(void*)ts->hd;_tmp923;});_tmp922->tl=inst;_tmp922;});}
# 4720
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp926;_tmp926.tag=0;_tmp926.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp959));({void*_tmp924[1]={& _tmp926};Cyc_Tcutil_terr(loc,({const char*_tmp925="too many parameters for typedef %s";_tag_dyneither(_tmp925,sizeof(char),35);}),_tag_dyneither(_tmp924,sizeof(void*),1));});});
if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;
# 4725
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp927=_cycalloc(sizeof(*_tmp927));_tmp927->hd=e;_tmp927->tl=hidden_ts;_tmp927;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);
inst=({struct Cyc_List_List*_tmp928=_cycalloc(sizeof(*_tmp928));_tmp928->hd=({struct _tuple16*_tmp929=_cycalloc(sizeof(*_tmp929));_tmp929->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp929->f2=e;_tmp929;});_tmp928->tl=inst;_tmp928;});}
# 4733
*_tmp958=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4735
if(td->defn != 0){
void*new_typ=
inst == 0?(void*)_check_null(td->defn):
 Cyc_Tcutil_rsubstitute(temp,inst,(void*)_check_null(td->defn));
*_tmp956=new_typ;}}
# 4742
_npop_handler(0);goto _LL66E;
# 4708
;_pop_region(temp);};}case 22: _LL699: _LL69A:
# 4743
 goto _LL69C;case 21: _LL69B: _LL69C:
 goto _LL69E;case 20: _LL69D: _LL69E:
 goto _LL66E;case 15: _LL69F: _tmp95A=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp789)->f1;_LL6A0:
# 4747
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp95A,1,allow_abs_aggr);goto _LL66E;case 16: _LL6A1: _tmp95C=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp789)->f1;_tmp95B=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp789)->f2;_LL6A2:
# 4750
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp95C,0,0);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp95B,1,0);
goto _LL66E;case 23: _LL6A3: _tmp95D=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp789)->f1;_LL6A4:
# 4754
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp95D,1,1);goto _LL66E;case 25: _LL6A5: _tmp95E=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp789)->f1;_LL6A6:
# 4756
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp95E,1,1);goto _LL66E;default: _LL6A7: _tmp95F=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp789)->f1;_LL6A8:
# 4758
 for(0;_tmp95F != 0;_tmp95F=_tmp95F->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmp95F->hd,1,1);}
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
{void*_tmp965=e->r;void*_tmp966=_tmp965;struct Cyc_Absyn_Exp*_tmp978;void*_tmp977;void*_tmp976;void*_tmp975;void*_tmp974;struct Cyc_Absyn_Exp*_tmp973;struct Cyc_Absyn_Exp*_tmp972;struct Cyc_Absyn_Exp*_tmp971;struct Cyc_Absyn_Exp*_tmp970;struct Cyc_Absyn_Exp*_tmp96F;struct Cyc_Absyn_Exp*_tmp96E;struct Cyc_Absyn_Exp*_tmp96D;struct Cyc_Absyn_Exp*_tmp96C;struct Cyc_Absyn_Exp*_tmp96B;struct Cyc_Absyn_Exp*_tmp96A;struct Cyc_List_List*_tmp969;switch(*((int*)_tmp966)){case 0: _LL7A9: _LL7AA:
 goto _LL7AC;case 31: _LL7AB: _LL7AC:
 goto _LL7AE;case 32: _LL7AD: _LL7AE:
 goto _LL7B0;case 1: _LL7AF: _LL7B0:
 goto _LL7A8;case 2: _LL7B1: _tmp969=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp966)->f2;_LL7B2:
# 4782
 for(0;_tmp969 != 0;_tmp969=_tmp969->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmp969->hd,te,cvtenv);}
goto _LL7A8;case 5: _LL7B3: _tmp96C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp966)->f1;_tmp96B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp966)->f2;_tmp96A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp966)->f3;_LL7B4:
# 4786
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp96C,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp96B,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp96A,te,cvtenv);
goto _LL7A8;case 6: _LL7B5: _tmp96E=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp966)->f1;_tmp96D=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp966)->f2;_LL7B6:
 _tmp970=_tmp96E;_tmp96F=_tmp96D;goto _LL7B8;case 7: _LL7B7: _tmp970=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp966)->f1;_tmp96F=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp966)->f2;_LL7B8:
 _tmp972=_tmp970;_tmp971=_tmp96F;goto _LL7BA;case 8: _LL7B9: _tmp972=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp966)->f1;_tmp971=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp966)->f2;_LL7BA:
# 4793
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp972,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp971,te,cvtenv);
goto _LL7A8;case 13: _LL7BB: _tmp974=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp966)->f1;_tmp973=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp966)->f2;_LL7BC:
# 4797
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp973,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp974,1,0);
goto _LL7A8;case 18: _LL7BD: _tmp975=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp966)->f1;_LL7BE:
 _tmp976=_tmp975;goto _LL7C0;case 16: _LL7BF: _tmp976=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp966)->f1;_LL7C0:
# 4802
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp976,1,0);
goto _LL7A8;case 38: _LL7C1: _tmp977=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp966)->f1;_LL7C2:
# 4805
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp977,1,0);
goto _LL7A8;case 17: _LL7C3: _tmp978=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp966)->f1;_LL7C4:
# 4808
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp978,te,cvtenv);
goto _LL7A8;default: _LL7C5: _LL7C6:
# 4811
({void*_tmp967=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp968="non-type-level-expression in Tcutil::i_check_valid_type_level_exp";_tag_dyneither(_tmp968,sizeof(char),66);}),_tag_dyneither(_tmp967,sizeof(void*),0));});}_LL7A8:;}
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
struct _tuple29 _tmp97D=*((struct _tuple29*)evs->hd);struct _tuple29 _tmp97E=_tmp97D;void*_tmp987;_LL7CB: _tmp987=_tmp97E.f1;_LL7CC:;{
void*_tmp97F=Cyc_Tcutil_compress(_tmp987);void*_tmp980=_tmp97F;struct Cyc_Core_Opt**_tmp986;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp980)->tag == 1){_LL7CE: _tmp986=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp980)->f4;_LL7CF:
# 4837
 if(*_tmp986 == 0)
*_tmp986=({struct Cyc_Core_Opt*_tmp981=_cycalloc(sizeof(*_tmp981));_tmp981->v=_tmp979;_tmp981;});else{
# 4841
struct Cyc_List_List*_tmp982=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp986))->v;
struct Cyc_List_List*_tmp983=0;
for(0;_tmp982 != 0;_tmp982=_tmp982->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp979,(struct Cyc_Absyn_Tvar*)_tmp982->hd))
_tmp983=({struct Cyc_List_List*_tmp984=_cycalloc(sizeof(*_tmp984));_tmp984->hd=(struct Cyc_Absyn_Tvar*)_tmp982->hd;_tmp984->tl=_tmp983;_tmp984;});}
*_tmp986=({struct Cyc_Core_Opt*_tmp985=_cycalloc(sizeof(*_tmp985));_tmp985->v=_tmp983;_tmp985;});}
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
void*_tmp99E=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);void*_tmp99F=_tmp99E;enum Cyc_Absyn_AliasQual _tmp9AD;struct Cyc_Core_Opt**_tmp9AC;struct Cyc_Absyn_Kind*_tmp9AB;struct Cyc_Core_Opt**_tmp9AA;struct Cyc_Core_Opt**_tmp9A9;struct Cyc_Core_Opt**_tmp9A8;struct Cyc_Core_Opt**_tmp9A7;struct Cyc_Core_Opt**_tmp9A6;struct Cyc_Core_Opt**_tmp9A5;switch(*((int*)_tmp99F)){case 1: _LL7DE: _tmp9A5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp99F)->f1;_LL7DF:
 _tmp9A6=_tmp9A5;goto _LL7E1;case 2: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99F)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99F)->f2)->aliasqual == Cyc_Absyn_Top){_LL7E0: _tmp9A6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99F)->f1;_LL7E1:
 _tmp9A7=_tmp9A6;goto _LL7E3;}else{goto _LL7EA;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99F)->f2)->aliasqual){case Cyc_Absyn_Top: _LL7E2: _tmp9A7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99F)->f1;_LL7E3:
 _tmp9A8=_tmp9A7;goto _LL7E5;case Cyc_Absyn_Aliasable: _LL7E4: _tmp9A8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99F)->f1;_LL7E5:
# 4922
*_tmp9A8=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL7DD;default: _LL7E6: _tmp9A9=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99F)->f1;_LL7E7:
# 4924
*_tmp9A9=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL7DD;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99F)->f2)->aliasqual == Cyc_Absyn_Top){_LL7E8: _tmp9AA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99F)->f1;_LL7E9:
# 4926
*_tmp9AA=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL7DD;}else{goto _LL7EA;}default: _LL7EA: _tmp9AC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99F)->f1;_tmp9AB=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99F)->f2;_LL7EB:
# 4928
*_tmp9AC=Cyc_Tcutil_kind_to_bound_opt(_tmp9AB);goto _LL7DD;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp99F)->f1)->kind == Cyc_Absyn_MemKind){_LL7EC: _tmp9AD=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp99F)->f1)->aliasqual;_LL7ED:
# 4930
({struct Cyc_String_pa_PrintArg_struct _tmp9A3;_tmp9A3.tag=0;_tmp9A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp9A4=_cycalloc_atomic(sizeof(*_tmp9A4));_tmp9A4->kind=Cyc_Absyn_MemKind;_tmp9A4->aliasqual=_tmp9AD;_tmp9A4;})));({struct Cyc_String_pa_PrintArg_struct _tmp9A2;_tmp9A2.tag=0;_tmp9A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd));({void*_tmp9A0[2]={& _tmp9A2,& _tmp9A3};Cyc_Tcutil_terr(loc,({const char*_tmp9A1="type variable %s cannot have kind %s";_tag_dyneither(_tmp9A1,sizeof(char),37);}),_tag_dyneither(_tmp9A0,sizeof(void*),2));});});});
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
{void*_tmp9B5=Cyc_Tcutil_compress(t);void*_tmp9B6=_tmp9B5;struct Cyc_List_List*_tmp9C2;void*_tmp9C1;struct Cyc_Absyn_Tqual _tmp9C0;void*_tmp9BF;struct Cyc_List_List*_tmp9BE;struct Cyc_Absyn_Exp*_tmp9BD;struct Cyc_List_List*_tmp9BC;struct Cyc_Absyn_Exp*_tmp9BB;struct Cyc_List_List*_tmp9BA;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9B6)->tag == 9){_LL7F6: _tmp9C2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9B6)->f1).tvars;_tmp9C1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9B6)->f1).effect;_tmp9C0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9B6)->f1).ret_tqual;_tmp9BF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9B6)->f1).ret_typ;_tmp9BE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9B6)->f1).args;_tmp9BD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9B6)->f1).requires_clause;_tmp9BC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9B6)->f1).requires_relns;_tmp9BB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9B6)->f1).ensures_clause;_tmp9BA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9B6)->f1).ensures_relns;_LL7F7:
# 4965
 fd->tvs=_tmp9C2;
fd->effect=_tmp9C1;
{struct Cyc_List_List*_tmp9B7=fd->args;for(0;_tmp9B7 != 0;(_tmp9B7=_tmp9B7->tl,_tmp9BE=_tmp9BE->tl)){
# 4969
(*((struct _tuple10*)_tmp9B7->hd)).f2=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp9BE))->hd)).f2;
(*((struct _tuple10*)_tmp9B7->hd)).f3=(*((struct _tuple10*)_tmp9BE->hd)).f3;}}
# 4972
fd->ret_tqual=_tmp9C0;
fd->ret_type=_tmp9BF;
(fd->ret_tqual).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmp9BF);
fd->requires_clause=_tmp9BD;
fd->requires_relns=_tmp9BC;
fd->ensures_clause=_tmp9BB;
fd->ensures_relns=_tmp9BA;
goto _LL7F5;}else{_LL7F8: _LL7F9:
({void*_tmp9B8=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp9B9="check_fndecl_valid_type: not a FnType";_tag_dyneither(_tmp9B9,sizeof(char),38);}),_tag_dyneither(_tmp9B8,sizeof(void*),0));});}_LL7F5:;}{
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
struct _tuple33*_tmp9D8=(struct _tuple33*)des->hd;struct _tuple33*_tmp9D9=_tmp9D8;struct Cyc_List_List*_tmp9F5;void*_tmp9F4;_LL7FB: _tmp9F5=_tmp9D9->f1;_tmp9F4=_tmp9D9->f2;_LL7FC:;
if(_tmp9F5 == 0){
# 5081
struct Cyc_List_List*_tmp9DA=fields;
for(0;_tmp9DA != 0;_tmp9DA=_tmp9DA->tl){
if(!(*((struct _tuple32*)_tmp9DA->hd)).f2){
(*((struct _tuple32*)_tmp9DA->hd)).f2=1;
(*((struct _tuple33*)des->hd)).f1=({struct Cyc_List_List*_tmp9DB=_cycalloc(sizeof(*_tmp9DB));_tmp9DB->hd=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp9DC=_cycalloc(sizeof(*_tmp9DC));_tmp9DC[0]=({struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp9DD;_tmp9DD.tag=1;_tmp9DD.f1=((*((struct _tuple32*)_tmp9DA->hd)).f1)->name;_tmp9DD;});_tmp9DC;});_tmp9DB->tl=0;_tmp9DB;});
ans=({struct Cyc_List_List*_tmp9DE=_region_malloc(rgn,sizeof(*_tmp9DE));_tmp9DE->hd=({struct _tuple34*_tmp9DF=_region_malloc(rgn,sizeof(*_tmp9DF));_tmp9DF->f1=(*((struct _tuple32*)_tmp9DA->hd)).f1;_tmp9DF->f2=_tmp9F4;_tmp9DF;});_tmp9DE->tl=ans;_tmp9DE;});
break;}}
# 5089
if(_tmp9DA == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp9E2;_tmp9E2.tag=0;_tmp9E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);({void*_tmp9E0[1]={& _tmp9E2};Cyc_Tcutil_terr(loc,({const char*_tmp9E1="too many arguments to %s";_tag_dyneither(_tmp9E1,sizeof(char),25);}),_tag_dyneither(_tmp9E0,sizeof(void*),1));});});}else{
if(_tmp9F5->tl != 0)
# 5093
({void*_tmp9E3=0;Cyc_Tcutil_terr(loc,({const char*_tmp9E4="multiple designators are not yet supported";_tag_dyneither(_tmp9E4,sizeof(char),43);}),_tag_dyneither(_tmp9E3,sizeof(void*),0));});else{
# 5096
void*_tmp9E5=(void*)_tmp9F5->hd;void*_tmp9E6=_tmp9E5;struct _dyneither_ptr*_tmp9F3;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp9E6)->tag == 0){_LL7FE: _LL7FF:
# 5098
({struct Cyc_String_pa_PrintArg_struct _tmp9E9;_tmp9E9.tag=0;_tmp9E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);({void*_tmp9E7[1]={& _tmp9E9};Cyc_Tcutil_terr(loc,({const char*_tmp9E8="array designator used in argument to %s";_tag_dyneither(_tmp9E8,sizeof(char),40);}),_tag_dyneither(_tmp9E7,sizeof(void*),1));});});
goto _LL7FD;}else{_LL800: _tmp9F3=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp9E6)->f1;_LL801: {
# 5101
struct Cyc_List_List*_tmp9EA=fields;
for(0;_tmp9EA != 0;_tmp9EA=_tmp9EA->tl){
if(Cyc_strptrcmp(_tmp9F3,((*((struct _tuple32*)_tmp9EA->hd)).f1)->name)== 0){
if((*((struct _tuple32*)_tmp9EA->hd)).f2)
({struct Cyc_String_pa_PrintArg_struct _tmp9ED;_tmp9ED.tag=0;_tmp9ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp9F3);({void*_tmp9EB[1]={& _tmp9ED};Cyc_Tcutil_terr(loc,({const char*_tmp9EC="member %s has already been used as an argument";_tag_dyneither(_tmp9EC,sizeof(char),47);}),_tag_dyneither(_tmp9EB,sizeof(void*),1));});});
(*((struct _tuple32*)_tmp9EA->hd)).f2=1;
ans=({struct Cyc_List_List*_tmp9EE=_region_malloc(rgn,sizeof(*_tmp9EE));_tmp9EE->hd=({struct _tuple34*_tmp9EF=_region_malloc(rgn,sizeof(*_tmp9EF));_tmp9EF->f1=(*((struct _tuple32*)_tmp9EA->hd)).f1;_tmp9EF->f2=_tmp9F4;_tmp9EF;});_tmp9EE->tl=ans;_tmp9EE;});
break;}}
# 5110
if(_tmp9EA == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp9F2;_tmp9F2.tag=0;_tmp9F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp9F3);({void*_tmp9F0[1]={& _tmp9F2};Cyc_Tcutil_terr(loc,({const char*_tmp9F1="bad field designator %s";_tag_dyneither(_tmp9F1,sizeof(char),24);}),_tag_dyneither(_tmp9F0,sizeof(void*),1));});});
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
void*_tmp9FF=Cyc_Tcutil_compress(t);void*_tmpA00=_tmp9FF;void*_tmpA04;union Cyc_Absyn_Constraint*_tmpA03;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA00)->tag == 5){_LL803: _tmpA04=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA00)->f1).elt_typ;_tmpA03=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA00)->f1).ptr_atts).bounds;_LL804: {
# 5140
void*_tmpA01=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpA03);void*_tmpA02=_tmpA01;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpA02)->tag == 0){_LL808: _LL809:
# 5142
*elt_typ_dest=_tmpA04;
return 1;}else{_LL80A: _LL80B:
 return 0;}_LL807:;}}else{_LL805: _LL806:
# 5146
 return 0;}_LL802:;}
# 5150
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmpA05=Cyc_Tcutil_compress(t);void*_tmpA06=_tmpA05;void*_tmpA08;union Cyc_Absyn_Constraint*_tmpA07;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA06)->tag == 5){_LL80D: _tmpA08=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA06)->f1).elt_typ;_tmpA07=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA06)->f1).ptr_atts).zero_term;_LL80E:
# 5153
*elt_typ_dest=_tmpA08;
return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpA07);}else{_LL80F: _LL810:
 return 0;}_LL80C:;}
# 5161
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type){
# 5163
void*_tmpA09=Cyc_Tcutil_compress(t);void*_tmpA0A=_tmpA09;void*_tmpA13;struct Cyc_Absyn_Tqual _tmpA12;struct Cyc_Absyn_Exp*_tmpA11;union Cyc_Absyn_Constraint*_tmpA10;void*_tmpA0F;union Cyc_Absyn_Constraint*_tmpA0E;union Cyc_Absyn_Constraint*_tmpA0D;switch(*((int*)_tmpA0A)){case 5: _LL812: _tmpA0F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA0A)->f1).elt_typ;_tmpA0E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA0A)->f1).ptr_atts).bounds;_tmpA0D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA0A)->f1).ptr_atts).zero_term;_LL813:
# 5165
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpA0D)){
*ptr_type=t;
*elt_type=_tmpA0F;
{void*_tmpA0B=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmpA0E);void*_tmpA0C=_tmpA0B;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpA0C)->tag == 0){_LL819: _LL81A:
*is_dyneither=1;goto _LL818;}else{_LL81B: _LL81C:
*is_dyneither=0;goto _LL818;}_LL818:;}
# 5172
return 1;}else{
return 0;}case 8: _LL814: _tmpA13=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA0A)->f1).elt_type;_tmpA12=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA0A)->f1).tq;_tmpA11=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA0A)->f1).num_elts;_tmpA10=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA0A)->f1).zero_term;_LL815:
# 5175
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpA10)){
*elt_type=_tmpA13;
*is_dyneither=0;
*ptr_type=Cyc_Tcutil_promote_array(t,(void*)& Cyc_Absyn_HeapRgn_val,0);
return 1;}else{
return 0;}default: _LL816: _LL817:
 return 0;}_LL811:;}
# 5188
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){
# 5190
void*_tmpA14=e1->r;void*_tmpA15=_tmpA14;struct Cyc_Absyn_Exp*_tmpA27;struct Cyc_Absyn_Exp*_tmpA26;struct Cyc_Absyn_Exp*_tmpA25;struct Cyc_Absyn_Exp*_tmpA24;struct Cyc_Absyn_Exp*_tmpA23;struct Cyc_Absyn_Exp*_tmpA22;switch(*((int*)_tmpA15)){case 13: _LL81E: _LL81F:
# 5192
({struct Cyc_String_pa_PrintArg_struct _tmpA18;_tmpA18.tag=0;_tmpA18.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));({void*_tmpA16[1]={& _tmpA18};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA17="we have a cast in a lhs:  %s";_tag_dyneither(_tmpA17,sizeof(char),29);}),_tag_dyneither(_tmpA16,sizeof(void*),1));});});case 19: _LL820: _tmpA22=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpA15)->f1;_LL821:
 _tmpA23=_tmpA22;goto _LL823;case 22: _LL822: _tmpA23=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA15)->f1;_LL823:
# 5195
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpA23->topt),ptr_type,is_dyneither,elt_type);case 21: _LL824: _tmpA24=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA15)->f1;_LL825:
 _tmpA25=_tmpA24;goto _LL827;case 20: _LL826: _tmpA25=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA15)->f1;_LL827:
# 5199
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpA25->topt),ptr_type,is_dyneither,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmpA1B;_tmpA1B.tag=0;_tmpA1B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1));({void*_tmpA19[1]={& _tmpA1B};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA1A="found zero pointer aggregate member assignment: %s";_tag_dyneither(_tmpA1A,sizeof(char),51);}),_tag_dyneither(_tmpA19,sizeof(void*),1));});});
return 0;case 12: _LL828: _tmpA26=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpA15)->f1;_LL829:
 _tmpA27=_tmpA26;goto _LL82B;case 11: _LL82A: _tmpA27=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpA15)->f1;_LL82B:
# 5205
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpA27->topt),ptr_type,is_dyneither,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmpA1E;_tmpA1E.tag=0;_tmpA1E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1));({void*_tmpA1C[1]={& _tmpA1E};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA1D="found zero pointer instantiate/noinstantiate: %s";_tag_dyneither(_tmpA1D,sizeof(char),49);}),_tag_dyneither(_tmpA1C,sizeof(void*),1));});});
return 0;case 1: _LL82C: _LL82D:
 return 0;default: _LL82E: _LL82F:
({struct Cyc_String_pa_PrintArg_struct _tmpA21;_tmpA21.tag=0;_tmpA21.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));({void*_tmpA1F[1]={& _tmpA21};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA20="found bad lhs in is_zero_ptr_deref: %s";_tag_dyneither(_tmpA20,sizeof(char),39);}),_tag_dyneither(_tmpA1F,sizeof(void*),1));});});}_LL81D:;}
# 5214
int Cyc_Tcutil_is_tagged_pointer_typ(void*t){
void*ignore=(void*)& Cyc_Absyn_VoidType_val;
return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
# 5225
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
# 5228
void*_tmpA28=Cyc_Tcutil_compress(r);void*_tmpA29=_tmpA28;struct Cyc_Absyn_Tvar*_tmpA36;enum Cyc_Absyn_KindQual _tmpA35;enum Cyc_Absyn_AliasQual _tmpA34;switch(*((int*)_tmpA29)){case 22: _LL831: _LL832:
 return !must_be_unique;case 21: _LL833: _LL834:
 return 1;case 17: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpA29)->f3 != 0){if(((struct Cyc_Absyn_Typedefdecl*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpA29)->f3)->kind != 0){if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpA29)->f4 == 0){_LL835: _tmpA35=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpA29)->f3)->kind)->v)->kind;_tmpA34=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpA29)->f3)->kind)->v)->aliasqual;_LL836:
# 5232
 return _tmpA35 == Cyc_Absyn_RgnKind  && (_tmpA34 == Cyc_Absyn_Unique  || _tmpA34 == Cyc_Absyn_Top  && !must_be_unique);}else{goto _LL839;}}else{goto _LL839;}}else{goto _LL839;}case 2: _LL837: _tmpA36=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA29)->f1;_LL838: {
# 5236
struct Cyc_Absyn_Kind*_tmpA2A=Cyc_Tcutil_tvar_kind(_tmpA36,& Cyc_Tcutil_rk);struct Cyc_Absyn_Kind*_tmpA2B=_tmpA2A;enum Cyc_Absyn_KindQual _tmpA33;enum Cyc_Absyn_AliasQual _tmpA32;_LL83C: _tmpA33=_tmpA2B->kind;_tmpA32=_tmpA2B->aliasqual;_LL83D:;
if(_tmpA33 == Cyc_Absyn_RgnKind  && (_tmpA32 == Cyc_Absyn_Unique  || _tmpA32 == Cyc_Absyn_Top  && !must_be_unique)){
void*_tmpA2C=Cyc_Absyn_compress_kb(_tmpA36->kind);void*_tmpA2D=_tmpA2C;struct Cyc_Core_Opt**_tmpA31;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA2D)->tag == 2){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA2D)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA2D)->f2)->aliasqual == Cyc_Absyn_Top){_LL83F: _tmpA31=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA2D)->f1;_LL840:
# 5240
*_tmpA31=({struct Cyc_Core_Opt*_tmpA2E=_cycalloc(sizeof(*_tmpA2E));_tmpA2E->v=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA2F=_cycalloc(sizeof(*_tmpA2F));_tmpA2F[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpA30;_tmpA30.tag=2;_tmpA30.f1=0;_tmpA30.f2=& Cyc_Tcutil_rk;_tmpA30;});_tmpA2F;});_tmpA2E;});
return 0;}else{goto _LL841;}}else{goto _LL841;}}else{_LL841: _LL842:
 return 1;}_LL83E:;}
# 5245
return 0;}default: _LL839: _LL83A:
 return 0;}_LL830:;}
# 5252
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmpA37=Cyc_Tcutil_compress(t);void*_tmpA38=_tmpA37;struct Cyc_Absyn_Tvar*_tmpA47;void*_tmpA46;switch(*((int*)_tmpA38)){case 5: _LL844: _tmpA46=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA38)->f1).ptr_atts).rgn;_LL845:
# 5255
 return Cyc_Tcutil_is_noalias_region(_tmpA46,must_be_unique);case 2: _LL846: _tmpA47=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA38)->f1;_LL847: {
# 5257
struct Cyc_Absyn_Kind*_tmpA39=Cyc_Tcutil_tvar_kind(_tmpA47,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmpA3A=_tmpA39;enum Cyc_Absyn_KindQual _tmpA45;enum Cyc_Absyn_AliasQual _tmpA44;_LL84B: _tmpA45=_tmpA3A->kind;_tmpA44=_tmpA3A->aliasqual;_LL84C:;{
enum Cyc_Absyn_KindQual _tmpA3B=_tmpA45;switch(_tmpA3B){case Cyc_Absyn_BoxKind: _LL84E: _LL84F:
 goto _LL851;case Cyc_Absyn_AnyKind: _LL850: _LL851: goto _LL853;case Cyc_Absyn_MemKind: _LL852: _LL853:
 if(_tmpA44 == Cyc_Absyn_Unique  || _tmpA44 == Cyc_Absyn_Top  && !must_be_unique){
void*_tmpA3C=Cyc_Absyn_compress_kb(_tmpA47->kind);void*_tmpA3D=_tmpA3C;struct Cyc_Core_Opt**_tmpA43;enum Cyc_Absyn_KindQual _tmpA42;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA3D)->tag == 2){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA3D)->f2)->aliasqual == Cyc_Absyn_Top){_LL857: _tmpA43=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA3D)->f1;_tmpA42=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA3D)->f2)->kind;_LL858:
# 5263
*_tmpA43=({struct Cyc_Core_Opt*_tmpA3E=_cycalloc(sizeof(*_tmpA3E));_tmpA3E->v=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA3F=_cycalloc(sizeof(*_tmpA3F));_tmpA3F[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpA40;_tmpA40.tag=2;_tmpA40.f1=0;_tmpA40.f2=({struct Cyc_Absyn_Kind*_tmpA41=_cycalloc_atomic(sizeof(*_tmpA41));_tmpA41->kind=_tmpA42;_tmpA41->aliasqual=Cyc_Absyn_Aliasable;_tmpA41;});_tmpA40;});_tmpA3F;});_tmpA3E;});
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
void*_tmpA49=_tmpA48;union Cyc_Absyn_DatatypeFieldInfoU _tmpA5D;struct Cyc_List_List*_tmpA5C;union Cyc_Absyn_DatatypeInfoU _tmpA5B;struct Cyc_List_List*_tmpA5A;struct Cyc_List_List*_tmpA59;struct Cyc_Absyn_Aggrdecl**_tmpA58;struct Cyc_List_List*_tmpA57;struct Cyc_List_List*_tmpA56;switch(*((int*)_tmpA49)){case 10: _LL85C: _tmpA56=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA49)->f1;_LL85D:
# 5281
 while(_tmpA56 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(*((struct _tuple12*)_tmpA56->hd)).f2))return 1;
_tmpA56=_tmpA56->tl;}
# 5285
return 0;case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA49)->f1).aggr_info).KnownAggr).tag == 2){_LL85E: _tmpA58=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA49)->f1).aggr_info).KnownAggr).val;_tmpA57=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA49)->f1).targs;_LL85F:
# 5287
 if((*_tmpA58)->impl == 0)return 0;else{
# 5289
struct Cyc_List_List*_tmpA4A=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,(*_tmpA58)->tvs,_tmpA57);
struct Cyc_List_List*_tmpA4B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpA58)->impl))->fields;
void*t;
while(_tmpA4B != 0){
t=_tmpA4A == 0?((struct Cyc_Absyn_Aggrfield*)_tmpA4B->hd)->type: Cyc_Tcutil_rsubstitute(rgn,_tmpA4A,((struct Cyc_Absyn_Aggrfield*)_tmpA4B->hd)->type);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpA4B=_tmpA4B->tl;}
# 5297
return 0;}}else{_LL862: _LL863:
# 5307
 return 0;}case 12: _LL860: _tmpA59=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA49)->f2;_LL861:
# 5300
 while(_tmpA59 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,((struct Cyc_Absyn_Aggrfield*)_tmpA59->hd)->type))return 1;
_tmpA59=_tmpA59->tl;}
# 5304
return 0;case 3: _LL864: _tmpA5B=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA49)->f1).datatype_info;_tmpA5A=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA49)->f1).targs;_LL865: {
# 5309
union Cyc_Absyn_DatatypeInfoU _tmpA4C=_tmpA5B;struct Cyc_List_List*_tmpA50;struct Cyc_Core_Opt*_tmpA4F;struct _tuple2*_tmpA4E;int _tmpA4D;if((_tmpA4C.UnknownDatatype).tag == 1){_LL86B: _tmpA4E=((_tmpA4C.UnknownDatatype).val).name;_tmpA4D=((_tmpA4C.UnknownDatatype).val).is_extensible;_LL86C:
# 5312
 return 0;}else{_LL86D: _tmpA50=(*(_tmpA4C.KnownDatatype).val)->tvs;_tmpA4F=(*(_tmpA4C.KnownDatatype).val)->fields;_LL86E:
# 5315
 return 0;}_LL86A:;}case 4: _LL866: _tmpA5D=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpA49)->f1).field_info;_tmpA5C=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpA49)->f1).targs;_LL867: {
# 5318
union Cyc_Absyn_DatatypeFieldInfoU _tmpA51=_tmpA5D;struct Cyc_Absyn_Datatypedecl*_tmpA55;struct Cyc_Absyn_Datatypefield*_tmpA54;if((_tmpA51.UnknownDatatypefield).tag == 1){_LL870: _LL871:
# 5321
 return 0;}else{_LL872: _tmpA55=((_tmpA51.KnownDatatypefield).val).f1;_tmpA54=((_tmpA51.KnownDatatypefield).val).f2;_LL873: {
# 5323
struct Cyc_List_List*_tmpA52=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpA55->tvs,_tmpA5C);
struct Cyc_List_List*_tmpA53=_tmpA54->typs;
while(_tmpA53 != 0){
_tmpA48=_tmpA52 == 0?(*((struct _tuple12*)_tmpA53->hd)).f2: Cyc_Tcutil_rsubstitute(rgn,_tmpA52,(*((struct _tuple12*)_tmpA53->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,_tmpA48))return 1;
_tmpA53=_tmpA53->tl;}
# 5330
return 0;}}_LL86F:;}default: _LL868: _LL869:
# 5332
 return 0;}_LL85B:;};}
# 5336
static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,struct _dyneither_ptr*f){
void*_tmpA5E=Cyc_Tcutil_compress(t);void*_tmpA5F=_tmpA5E;struct Cyc_List_List*_tmpA6A;struct Cyc_Absyn_Aggrdecl*_tmpA69;struct Cyc_List_List*_tmpA68;switch(*((int*)_tmpA5F)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA5F)->f1).aggr_info).KnownAggr).tag == 2){_LL875: _tmpA69=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA5F)->f1).aggr_info).KnownAggr).val;_tmpA68=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA5F)->f1).targs;_LL876:
# 5339
 _tmpA6A=_tmpA69->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpA69->impl))->fields;goto _LL878;}else{goto _LL879;}case 12: _LL877: _tmpA6A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA5F)->f2;_LL878: {
# 5341
struct Cyc_Absyn_Aggrfield*_tmpA60=Cyc_Absyn_lookup_field(_tmpA6A,f);
{struct Cyc_Absyn_Aggrfield*_tmpA61=_tmpA60;void*_tmpA64;if(_tmpA61 == 0){_LL87C: _LL87D:
({void*_tmpA62=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA63="is_noalias_field: missing field";_tag_dyneither(_tmpA63,sizeof(char),32);}),_tag_dyneither(_tmpA62,sizeof(void*),0));});}else{_LL87E: _tmpA64=_tmpA61->type;_LL87F:
# 5345
 return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpA64);}_LL87B:;}
# 5347
return 0;}default: _LL879: _LL87A:
# 5349
({struct Cyc_String_pa_PrintArg_struct _tmpA67;_tmpA67.tag=0;_tmpA67.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));({void*_tmpA65[1]={& _tmpA67};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA66="is_noalias_field: invalid type |%s|";_tag_dyneither(_tmpA66,sizeof(char),36);}),_tag_dyneither(_tmpA65,sizeof(void*),1));});});}_LL874:;}
# 5357
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
void*_tmpA6B=e->r;void*_tmpA6C=_tmpA6B;struct Cyc_Absyn_Stmt*_tmpA82;struct Cyc_Absyn_Exp*_tmpA81;struct Cyc_Absyn_Exp*_tmpA80;struct Cyc_Absyn_Exp*_tmpA7F;struct Cyc_Absyn_Exp*_tmpA7E;struct Cyc_Absyn_Exp*_tmpA7D;struct Cyc_Absyn_Exp*_tmpA7C;struct Cyc_Absyn_Exp*_tmpA7B;struct _dyneither_ptr*_tmpA7A;struct Cyc_Absyn_Exp*_tmpA79;struct Cyc_Absyn_Exp*_tmpA78;switch(*((int*)_tmpA6C)){case 1: if(((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA6C)->f1)->tag == 1){_LL881: _LL882:
 return 0;}else{goto _LL893;}case 21: _LL883: _tmpA78=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA6C)->f1;_LL884:
 _tmpA79=_tmpA78;goto _LL886;case 19: _LL885: _tmpA79=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpA6C)->f1;_LL886:
# 5362
 return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmpA79->topt),1) && 
Cyc_Tcutil_is_noalias_path(r,_tmpA79);case 20: _LL887: _tmpA7B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA6C)->f1;_tmpA7A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA6C)->f2;_LL888:
 return Cyc_Tcutil_is_noalias_path(r,_tmpA7B);case 22: _LL889: _tmpA7D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA6C)->f1;_tmpA7C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA6C)->f2;_LL88A: {
# 5366
void*_tmpA6D=Cyc_Tcutil_compress((void*)_check_null(_tmpA7D->topt));void*_tmpA6E=_tmpA6D;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA6E)->tag == 10){_LL896: _LL897:
 return Cyc_Tcutil_is_noalias_path(r,_tmpA7D);}else{_LL898: _LL899:
 return 0;}_LL895:;}case 5: _LL88B: _tmpA7F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA6C)->f2;_tmpA7E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA6C)->f3;_LL88C:
# 5371
 return Cyc_Tcutil_is_noalias_path(r,_tmpA7F) && Cyc_Tcutil_is_noalias_path(r,_tmpA7E);case 8: _LL88D: _tmpA80=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpA6C)->f2;_LL88E:
 _tmpA81=_tmpA80;goto _LL890;case 13: _LL88F: _tmpA81=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA6C)->f2;_LL890:
 return Cyc_Tcutil_is_noalias_path(r,_tmpA81);case 36: _LL891: _tmpA82=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpA6C)->f1;_LL892:
# 5375
 while(1){
void*_tmpA6F=_tmpA82->r;void*_tmpA70=_tmpA6F;struct Cyc_Absyn_Exp*_tmpA77;struct Cyc_Absyn_Decl*_tmpA76;struct Cyc_Absyn_Stmt*_tmpA75;struct Cyc_Absyn_Stmt*_tmpA74;struct Cyc_Absyn_Stmt*_tmpA73;switch(*((int*)_tmpA70)){case 2: _LL89B: _tmpA74=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpA70)->f1;_tmpA73=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpA70)->f2;_LL89C:
 _tmpA82=_tmpA73;goto _LL89A;case 12: _LL89D: _tmpA76=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpA70)->f1;_tmpA75=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpA70)->f2;_LL89E:
 _tmpA82=_tmpA75;goto _LL89A;case 1: _LL89F: _tmpA77=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpA70)->f1;_LL8A0:
 return Cyc_Tcutil_is_noalias_path(r,_tmpA77);default: _LL8A1: _LL8A2:
({void*_tmpA71=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA72="is_noalias_stmt_exp: ill-formed StmtExp";_tag_dyneither(_tmpA72,sizeof(char),40);}),_tag_dyneither(_tmpA71,sizeof(void*),0));});}_LL89A:;}default: _LL893: _LL894:
# 5383
 return 1;}_LL880:;}
# 5400 "tcutil.cyc"
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5402
struct _tuple18 bogus_ans=({struct _tuple18 _tmpACA;_tmpACA.f1=0;_tmpACA.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpACA;});
void*_tmpA83=e->r;void*_tmpA84=_tmpA83;struct Cyc_Absyn_Exp*_tmpAC9;struct Cyc_Absyn_Exp*_tmpAC8;struct Cyc_Absyn_Exp*_tmpAC7;struct Cyc_Absyn_Exp*_tmpAC6;struct _dyneither_ptr*_tmpAC5;int _tmpAC4;struct Cyc_Absyn_Exp*_tmpAC3;struct _dyneither_ptr*_tmpAC2;int _tmpAC1;void*_tmpAC0;switch(*((int*)_tmpA84)){case 1: _LL8A4: _tmpAC0=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA84)->f1;_LL8A5: {
# 5406
void*_tmpA85=_tmpAC0;struct Cyc_Absyn_Vardecl*_tmpA92;struct Cyc_Absyn_Vardecl*_tmpA91;struct Cyc_Absyn_Vardecl*_tmpA90;struct Cyc_Absyn_Vardecl*_tmpA8F;switch(*((int*)_tmpA85)){case 0: _LL8B1: _LL8B2:
 goto _LL8B4;case 2: _LL8B3: _LL8B4:
 return bogus_ans;case 1: _LL8B5: _tmpA8F=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpA85)->f1;_LL8B6: {
# 5410
void*_tmpA86=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpA87=_tmpA86;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA87)->tag == 8){_LL8BE: _LL8BF:
# 5412
 return({struct _tuple18 _tmpA88;_tmpA88.f1=1;_tmpA88.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpA88;});}else{_LL8C0: _LL8C1:
 return({struct _tuple18 _tmpA89;_tmpA89.f1=(_tmpA8F->tq).real_const;_tmpA89.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpA89;});}_LL8BD:;}case 4: _LL8B7: _tmpA90=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpA85)->f1;_LL8B8: {
# 5416
void*_tmpA8A=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpA8B=_tmpA8A;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA8B)->tag == 8){_LL8C3: _LL8C4:
 return({struct _tuple18 _tmpA8C;_tmpA8C.f1=1;_tmpA8C.f2=(void*)_check_null(_tmpA90->rgn);_tmpA8C;});}else{_LL8C5: _LL8C6:
# 5419
 _tmpA90->escapes=1;
return({struct _tuple18 _tmpA8D;_tmpA8D.f1=(_tmpA90->tq).real_const;_tmpA8D.f2=(void*)_check_null(_tmpA90->rgn);_tmpA8D;});}_LL8C2:;}case 5: _LL8B9: _tmpA91=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpA85)->f1;_LL8BA:
# 5422
 _tmpA92=_tmpA91;goto _LL8BC;default: _LL8BB: _tmpA92=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpA85)->f1;_LL8BC:
# 5424
 _tmpA92->escapes=1;
return({struct _tuple18 _tmpA8E;_tmpA8E.f1=(_tmpA92->tq).real_const;_tmpA8E.f2=(void*)_check_null(_tmpA92->rgn);_tmpA8E;});}_LL8B0:;}case 20: _LL8A6: _tmpAC3=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA84)->f1;_tmpAC2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA84)->f2;_tmpAC1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA84)->f3;_LL8A7:
# 5429
 if(_tmpAC1)return bogus_ans;{
# 5432
void*_tmpA93=Cyc_Tcutil_compress((void*)_check_null(_tmpAC3->topt));void*_tmpA94=_tmpA93;struct Cyc_Absyn_Aggrdecl*_tmpAA2;struct Cyc_List_List*_tmpAA1;switch(*((int*)_tmpA94)){case 12: _LL8C8: _tmpAA1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA94)->f2;_LL8C9: {
# 5434
struct Cyc_Absyn_Aggrfield*_tmpA95=Cyc_Absyn_lookup_field(_tmpAA1,_tmpAC2);
if(_tmpA95 != 0  && _tmpA95->width == 0){
struct _tuple18 _tmpA96=Cyc_Tcutil_addressof_props(te,_tmpAC3);struct _tuple18 _tmpA97=_tmpA96;int _tmpA9A;void*_tmpA99;_LL8CF: _tmpA9A=_tmpA97.f1;_tmpA99=_tmpA97.f2;_LL8D0:;
return({struct _tuple18 _tmpA98;_tmpA98.f1=(_tmpA95->tq).real_const  || _tmpA9A;_tmpA98.f2=_tmpA99;_tmpA98;});}
# 5439
return bogus_ans;}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA94)->f1).aggr_info).KnownAggr).tag == 2){_LL8CA: _tmpAA2=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA94)->f1).aggr_info).KnownAggr).val;_LL8CB: {
# 5441
struct Cyc_Absyn_Aggrfield*_tmpA9B=Cyc_Absyn_lookup_decl_field(_tmpAA2,_tmpAC2);
if(_tmpA9B != 0  && _tmpA9B->width == 0){
struct _tuple18 _tmpA9C=Cyc_Tcutil_addressof_props(te,_tmpAC3);struct _tuple18 _tmpA9D=_tmpA9C;int _tmpAA0;void*_tmpA9F;_LL8D2: _tmpAA0=_tmpA9D.f1;_tmpA9F=_tmpA9D.f2;_LL8D3:;
return({struct _tuple18 _tmpA9E;_tmpA9E.f1=(_tmpA9B->tq).real_const  || _tmpAA0;_tmpA9E.f2=_tmpA9F;_tmpA9E;});}
# 5446
return bogus_ans;}}else{goto _LL8CC;}default: _LL8CC: _LL8CD:
 return bogus_ans;}_LL8C7:;};case 21: _LL8A8: _tmpAC6=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA84)->f1;_tmpAC5=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA84)->f2;_tmpAC4=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA84)->f3;_LL8A9:
# 5451
 if(_tmpAC4)return bogus_ans;{
# 5455
void*_tmpAA3=Cyc_Tcutil_compress((void*)_check_null(_tmpAC6->topt));void*_tmpAA4=_tmpAA3;void*_tmpAAB;void*_tmpAAA;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAA4)->tag == 5){_LL8D5: _tmpAAB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAA4)->f1).elt_typ;_tmpAAA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAA4)->f1).ptr_atts).rgn;_LL8D6: {
# 5457
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmpAA5=Cyc_Tcutil_compress(_tmpAAB);void*_tmpAA6=_tmpAA5;struct Cyc_Absyn_Aggrdecl*_tmpAA8;struct Cyc_List_List*_tmpAA7;switch(*((int*)_tmpAA6)){case 12: _LL8DA: _tmpAA7=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpAA6)->f2;_LL8DB:
# 5460
 finfo=Cyc_Absyn_lookup_field(_tmpAA7,_tmpAC5);goto _LL8D9;case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAA6)->f1).aggr_info).KnownAggr).tag == 2){_LL8DC: _tmpAA8=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAA6)->f1).aggr_info).KnownAggr).val;_LL8DD:
# 5462
 finfo=Cyc_Absyn_lookup_decl_field(_tmpAA8,_tmpAC5);goto _LL8D9;}else{goto _LL8DE;}default: _LL8DE: _LL8DF:
 return bogus_ans;}_LL8D9:;}
# 5465
if(finfo != 0  && finfo->width == 0)
return({struct _tuple18 _tmpAA9;_tmpAA9.f1=(finfo->tq).real_const;_tmpAA9.f2=_tmpAAA;_tmpAA9;});
return bogus_ans;}}else{_LL8D7: _LL8D8:
 return bogus_ans;}_LL8D4:;};case 19: _LL8AA: _tmpAC7=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpA84)->f1;_LL8AB: {
# 5472
void*_tmpAAC=Cyc_Tcutil_compress((void*)_check_null(_tmpAC7->topt));void*_tmpAAD=_tmpAAC;struct Cyc_Absyn_Tqual _tmpAB0;void*_tmpAAF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAAD)->tag == 5){_LL8E1: _tmpAB0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAAD)->f1).elt_tq;_tmpAAF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAAD)->f1).ptr_atts).rgn;_LL8E2:
# 5474
 return({struct _tuple18 _tmpAAE;_tmpAAE.f1=_tmpAB0.real_const;_tmpAAE.f2=_tmpAAF;_tmpAAE;});}else{_LL8E3: _LL8E4:
 return bogus_ans;}_LL8E0:;}case 22: _LL8AC: _tmpAC9=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA84)->f1;_tmpAC8=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA84)->f2;_LL8AD: {
# 5480
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmpAC9->topt));
void*_tmpAB1=t;struct Cyc_Absyn_Tqual _tmpABD;struct Cyc_Absyn_Tqual _tmpABC;void*_tmpABB;struct Cyc_List_List*_tmpABA;switch(*((int*)_tmpAB1)){case 10: _LL8E6: _tmpABA=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpAB1)->f1;_LL8E7: {
# 5484
struct _tuple14 _tmpAB2=Cyc_Evexp_eval_const_uint_exp(_tmpAC8);struct _tuple14 _tmpAB3=_tmpAB2;unsigned int _tmpAB7;int _tmpAB6;_LL8EF: _tmpAB7=_tmpAB3.f1;_tmpAB6=_tmpAB3.f2;_LL8F0:;
if(!_tmpAB6)
return bogus_ans;{
struct _tuple12*_tmpAB4=Cyc_Absyn_lookup_tuple_field(_tmpABA,(int)_tmpAB7);
if(_tmpAB4 != 0)
return({struct _tuple18 _tmpAB5;_tmpAB5.f1=((*_tmpAB4).f1).real_const;_tmpAB5.f2=(Cyc_Tcutil_addressof_props(te,_tmpAC9)).f2;_tmpAB5;});
return bogus_ans;};}case 5: _LL8E8: _tmpABC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAB1)->f1).elt_tq;_tmpABB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAB1)->f1).ptr_atts).rgn;_LL8E9:
# 5492
 return({struct _tuple18 _tmpAB8;_tmpAB8.f1=_tmpABC.real_const;_tmpAB8.f2=_tmpABB;_tmpAB8;});case 8: _LL8EA: _tmpABD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAB1)->f1).tq;_LL8EB:
# 5498
 return({struct _tuple18 _tmpAB9;_tmpAB9.f1=_tmpABD.real_const;_tmpAB9.f2=(Cyc_Tcutil_addressof_props(te,_tmpAC9)).f2;_tmpAB9;});default: _LL8EC: _LL8ED:
 return bogus_ans;}_LL8E5:;}default: _LL8AE: _LL8AF:
# 5502
({void*_tmpABE=0;Cyc_Tcutil_terr(e->loc,({const char*_tmpABF="unary & applied to non-lvalue";_tag_dyneither(_tmpABF,sizeof(char),30);}),_tag_dyneither(_tmpABE,sizeof(void*),0));});
return bogus_ans;}_LL8A3:;}
# 5509
void Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,union Cyc_Absyn_Constraint*b){
b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(b);{
void*_tmpACB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);void*_tmpACC=_tmpACB;struct Cyc_Absyn_Exp*_tmpAD5;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpACC)->tag == 0){_LL8F2: _LL8F3:
 return;}else{_LL8F4: _tmpAD5=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpACC)->f1;_LL8F5: {
# 5514
struct _tuple14 _tmpACD=Cyc_Evexp_eval_const_uint_exp(_tmpAD5);struct _tuple14 _tmpACE=_tmpACD;unsigned int _tmpAD4;int _tmpAD3;_LL8F7: _tmpAD4=_tmpACE.f1;_tmpAD3=_tmpACE.f2;_LL8F8:;
if(_tmpAD3  && _tmpAD4 <= i)
({struct Cyc_Int_pa_PrintArg_struct _tmpAD2;_tmpAD2.tag=1;_tmpAD2.f1=(unsigned long)((int)i);({struct Cyc_Int_pa_PrintArg_struct _tmpAD1;_tmpAD1.tag=1;_tmpAD1.f1=(unsigned long)((int)_tmpAD4);({void*_tmpACF[2]={& _tmpAD1,& _tmpAD2};Cyc_Tcutil_terr(loc,({const char*_tmpAD0="dereference is out of bounds: %d <= %d";_tag_dyneither(_tmpAD0,sizeof(char),39);}),_tag_dyneither(_tmpACF,sizeof(void*),2));});});});
return;}}_LL8F1:;};}
# 5521
void Cyc_Tcutil_check_nonzero_bound(unsigned int loc,union Cyc_Absyn_Constraint*b){
Cyc_Tcutil_check_bound(loc,0,b);}
# 5525
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b){
void*_tmpAD6=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);void*_tmpAD7=_tmpAD6;struct Cyc_Absyn_Exp*_tmpADC;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpAD7)->tag == 1){_LL8FA: _tmpADC=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpAD7)->f1;_LL8FB: {
# 5528
struct _tuple14 _tmpAD8=Cyc_Evexp_eval_const_uint_exp(_tmpADC);struct _tuple14 _tmpAD9=_tmpAD8;unsigned int _tmpADB;int _tmpADA;_LL8FF: _tmpADB=_tmpAD9.f1;_tmpADA=_tmpAD9.f2;_LL900:;
return _tmpADA  && _tmpADB == 1;}}else{_LL8FC: _LL8FD:
 return 0;}_LL8F9:;}
# 5534
int Cyc_Tcutil_bits_only(void*t){
void*_tmpADD=Cyc_Tcutil_compress(t);void*_tmpADE=_tmpADD;struct Cyc_List_List*_tmpAE8;struct Cyc_Absyn_Aggrdecl*_tmpAE7;struct Cyc_List_List*_tmpAE6;struct Cyc_List_List*_tmpAE5;void*_tmpAE4;union Cyc_Absyn_Constraint*_tmpAE3;switch(*((int*)_tmpADE)){case 0: _LL902: _LL903:
 goto _LL905;case 6: _LL904: _LL905:
 goto _LL907;case 7: _LL906: _LL907:
 return 1;case 13: _LL908: _LL909:
 goto _LL90B;case 14: _LL90A: _LL90B:
 return 0;case 8: _LL90C: _tmpAE4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpADE)->f1).elt_type;_tmpAE3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpADE)->f1).zero_term;_LL90D:
# 5544
 return !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpAE3) && Cyc_Tcutil_bits_only(_tmpAE4);case 10: _LL90E: _tmpAE5=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpADE)->f1;_LL90F:
# 5546
 for(0;_tmpAE5 != 0;_tmpAE5=_tmpAE5->tl){
if(!Cyc_Tcutil_bits_only((*((struct _tuple12*)_tmpAE5->hd)).f2))return 0;}
return 1;case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpADE)->f1).aggr_info).UnknownAggr).tag == 1){_LL910: _LL911:
 return 0;}else{_LL912: _tmpAE7=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpADE)->f1).aggr_info).KnownAggr).val;_tmpAE6=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpADE)->f1).targs;_LL913:
# 5551
 if(_tmpAE7->impl == 0)
return 0;{
int okay=1;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAE7->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
if(okay)return 1;{
struct _RegionHandle _tmpADF=_new_region("rgn");struct _RegionHandle*rgn=& _tmpADF;_push_region(rgn);
{struct Cyc_List_List*_tmpAE0=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpAE7->tvs,_tmpAE6);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAE7->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpAE0,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpAE1=0;_npop_handler(0);return _tmpAE1;}}}{
int _tmpAE2=1;_npop_handler(0);return _tmpAE2;};}
# 5558
;_pop_region(rgn);};};}case 12: _LL914: _tmpAE8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpADE)->f2;_LL915:
# 5563
 for(0;_tmpAE8 != 0;_tmpAE8=_tmpAE8->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)_tmpAE8->hd)->type))return 0;}
return 1;default: _LL916: _LL917:
 return 0;}_LL901:;}
# 5571
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e){
while(1){
void*_tmpAE9=e->r;void*_tmpAEA=_tmpAE9;struct Cyc_Absyn_Exp*_tmpAEC;struct Cyc_Absyn_Exp*_tmpAEB;switch(*((int*)_tmpAEA)){case 1: _LL919: _LL91A:
 return 1;case 11: _LL91B: _tmpAEB=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpAEA)->f1;_LL91C:
 _tmpAEC=_tmpAEB;goto _LL91E;case 12: _LL91D: _tmpAEC=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpAEA)->f1;_LL91E:
 e=_tmpAEC;continue;default: _LL91F: _LL920:
# 5578
 return 0;}_LL918:;}}
# 5588
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmpAED=e->r;void*_tmpAEE=_tmpAED;struct Cyc_List_List*_tmpB0C;struct Cyc_List_List*_tmpB0B;enum Cyc_Absyn_Primop _tmpB0A;struct Cyc_List_List*_tmpB09;struct Cyc_List_List*_tmpB08;struct Cyc_List_List*_tmpB07;struct Cyc_List_List*_tmpB06;struct Cyc_Absyn_Exp*_tmpB05;struct Cyc_Absyn_Exp*_tmpB04;struct Cyc_Absyn_Exp*_tmpB03;struct Cyc_Absyn_Exp*_tmpB02;void*_tmpB01;struct Cyc_Absyn_Exp*_tmpB00;void*_tmpAFF;struct Cyc_Absyn_Exp*_tmpAFE;struct Cyc_Absyn_Exp*_tmpAFD;struct Cyc_Absyn_Exp*_tmpAFC;struct Cyc_Absyn_Exp*_tmpAFB;struct Cyc_Absyn_Exp*_tmpAFA;struct Cyc_Absyn_Exp*_tmpAF9;struct Cyc_Absyn_Exp*_tmpAF8;struct Cyc_Absyn_Exp*_tmpAF7;void*_tmpAF6;switch(*((int*)_tmpAEE)){case 0: _LL922: _LL923:
 goto _LL925;case 16: _LL924: _LL925:
 goto _LL927;case 17: _LL926: _LL927:
 goto _LL929;case 18: _LL928: _LL929:
 goto _LL92B;case 31: _LL92A: _LL92B:
 goto _LL92D;case 32: _LL92C: _LL92D:
 return 1;case 1: _LL92E: _tmpAF6=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpAEE)->f1;_LL92F: {
# 5599
void*_tmpAEF=_tmpAF6;struct Cyc_Absyn_Vardecl*_tmpAF5;struct Cyc_Absyn_Vardecl*_tmpAF4;switch(*((int*)_tmpAEF)){case 2: _LL951: _LL952:
 return 1;case 1: _LL953: _tmpAF4=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpAEF)->f1;_LL954: {
# 5602
void*_tmpAF0=Cyc_Tcutil_compress(_tmpAF4->type);void*_tmpAF1=_tmpAF0;switch(*((int*)_tmpAF1)){case 8: _LL95C: _LL95D:
 goto _LL95F;case 9: _LL95E: _LL95F:
 return 1;default: _LL960: _LL961:
 return var_okay;}_LL95B:;}case 4: _LL955: _tmpAF5=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpAEF)->f1;_LL956:
# 5609
 if(_tmpAF5->sc == Cyc_Absyn_Static){
void*_tmpAF2=Cyc_Tcutil_compress(_tmpAF5->type);void*_tmpAF3=_tmpAF2;switch(*((int*)_tmpAF3)){case 8: _LL963: _LL964:
 goto _LL966;case 9: _LL965: _LL966:
 return 1;default: _LL967: _LL968:
 return var_okay;}_LL962:;}else{
# 5616
return var_okay;}case 0: _LL957: _LL958:
 return 0;default: _LL959: _LL95A:
 return var_okay;}_LL950:;}case 5: _LL930: _tmpAF9=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAEE)->f1;_tmpAF8=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAEE)->f2;_tmpAF7=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAEE)->f3;_LL931:
# 5622
 return(Cyc_Tcutil_cnst_exp(0,_tmpAF9) && 
Cyc_Tcutil_cnst_exp(0,_tmpAF8)) && 
Cyc_Tcutil_cnst_exp(0,_tmpAF7);case 8: _LL932: _tmpAFB=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpAEE)->f1;_tmpAFA=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpAEE)->f2;_LL933:
# 5626
 return Cyc_Tcutil_cnst_exp(0,_tmpAFB) && Cyc_Tcutil_cnst_exp(0,_tmpAFA);case 11: _LL934: _tmpAFC=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpAEE)->f1;_LL935:
 _tmpAFD=_tmpAFC;goto _LL937;case 12: _LL936: _tmpAFD=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpAEE)->f1;_LL937:
# 5629
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpAFD);case 13: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAEE)->f4 == Cyc_Absyn_No_coercion){_LL938: _tmpAFF=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAEE)->f1;_tmpAFE=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAEE)->f2;_LL939:
# 5631
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpAFE);}else{_LL93A: _tmpB01=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAEE)->f1;_tmpB00=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAEE)->f2;_LL93B:
# 5634
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpB00);}case 14: _LL93C: _tmpB02=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpAEE)->f1;_LL93D:
# 5636
 return Cyc_Tcutil_cnst_exp(1,_tmpB02);case 26: _LL93E: _tmpB04=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpAEE)->f2;_tmpB03=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpAEE)->f3;_LL93F:
# 5638
 return Cyc_Tcutil_cnst_exp(0,_tmpB04) && Cyc_Tcutil_cnst_exp(0,_tmpB03);case 27: _LL940: _tmpB05=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpAEE)->f1;_LL941:
# 5640
 return Cyc_Tcutil_cnst_exp(0,_tmpB05);case 25: _LL942: _tmpB06=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpAEE)->f1;_LL943:
 _tmpB07=_tmpB06;goto _LL945;case 29: _LL944: _tmpB07=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpAEE)->f2;_LL945:
 _tmpB08=_tmpB07;goto _LL947;case 28: _LL946: _tmpB08=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpAEE)->f3;_LL947:
# 5644
 for(0;_tmpB08 != 0;_tmpB08=_tmpB08->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple19*)_tmpB08->hd)).f2))
return 0;}
return 1;case 2: _LL948: _tmpB0A=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpAEE)->f1;_tmpB09=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpAEE)->f2;_LL949:
# 5649
 _tmpB0B=_tmpB09;goto _LL94B;case 23: _LL94A: _tmpB0B=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpAEE)->f1;_LL94B:
 _tmpB0C=_tmpB0B;goto _LL94D;case 30: _LL94C: _tmpB0C=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpAEE)->f1;_LL94D:
# 5652
 for(0;_tmpB0C != 0;_tmpB0C=_tmpB0C->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmpB0C->hd))
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
void*_tmpB0D=Cyc_Tcutil_compress(t);void*_tmpB0E=_tmpB0D;struct Cyc_List_List*_tmpB18;union Cyc_Absyn_AggrInfoU _tmpB17;struct Cyc_List_List*_tmpB16;struct Cyc_List_List*_tmpB15;void*_tmpB14;union Cyc_Absyn_Constraint*_tmpB13;union Cyc_Absyn_Constraint*_tmpB12;switch(*((int*)_tmpB0E)){case 0: _LL96A: _LL96B:
 goto _LL96D;case 6: _LL96C: _LL96D:
 goto _LL96F;case 7: _LL96E: _LL96F:
 return 1;case 5: _LL970: _tmpB13=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB0E)->f1).ptr_atts).nullable;_tmpB12=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB0E)->f1).ptr_atts).bounds;_LL971: {
# 5673
void*_tmpB0F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpB12);void*_tmpB10=_tmpB0F;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpB10)->tag == 0){_LL981: _LL982:
 return 1;}else{_LL983: _LL984:
# 5676
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmpB13);}_LL980:;}case 8: _LL972: _tmpB14=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB0E)->f1).elt_type;_LL973:
# 5679
 return Cyc_Tcutil_supports_default(_tmpB14);case 10: _LL974: _tmpB15=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpB0E)->f1;_LL975:
# 5681
 for(0;_tmpB15 != 0;_tmpB15=_tmpB15->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple12*)_tmpB15->hd)).f2))return 0;}
return 1;case 11: _LL976: _tmpB17=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpB0E)->f1).aggr_info;_tmpB16=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpB0E)->f1).targs;_LL977: {
# 5685
struct Cyc_Absyn_Aggrdecl*_tmpB11=Cyc_Absyn_get_known_aggrdecl(_tmpB17);
if(_tmpB11->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB11->impl))->exist_vars != 0)return 0;
return Cyc_Tcutil_fields_support_default(_tmpB11->tvs,_tmpB16,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB11->impl))->fields);}case 12: _LL978: _tmpB18=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpB0E)->f2;_LL979:
 return Cyc_Tcutil_fields_support_default(0,0,_tmpB18);case 14: _LL97A: _LL97B:
# 5691
 goto _LL97D;case 13: _LL97C: _LL97D:
 return 1;default: _LL97E: _LL97F:
# 5694
 return 0;}_LL969:;}
# 5699
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t){
void*_tmpB19=t;struct Cyc_Absyn_Typedefdecl*_tmpB1E;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpB19)->tag == 17){_LL986: _tmpB1E=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpB19)->f3;_LL987:
# 5702
 if(_tmpB1E != 0){
struct Cyc_Absyn_Tqual _tmpB1A=_tmpB1E->tq;
if(((_tmpB1A.print_const  || _tmpB1A.q_volatile) || _tmpB1A.q_restrict) || _tmpB1A.real_const)
# 5707
({struct Cyc_String_pa_PrintArg_struct _tmpB1D;_tmpB1D.tag=0;_tmpB1D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmpB1B[1]={& _tmpB1D};Cyc_Tcutil_warn(loc,({const char*_tmpB1C="qualifier within typedef type %s is ignored";_tag_dyneither(_tmpB1C,sizeof(char),44);}),_tag_dyneither(_tmpB1B,sizeof(void*),1));});});}
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
void*_tmpB24=Cyc_Tcutil_compress(t);void*_tmpB25=_tmpB24;union Cyc_Absyn_Constraint*_tmpB29;union Cyc_Absyn_Constraint*_tmpB28;switch(*((int*)_tmpB25)){case 6: _LL98B: _LL98C:
 goto _LL98E;case 7: _LL98D: _LL98E:
 return 1;case 5: _LL98F: _tmpB29=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB25)->f1).ptr_atts).nullable;_tmpB28=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB25)->f1).ptr_atts).bounds;_LL990: {
# 5736
void*_tmpB26=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpB28);void*_tmpB27=_tmpB26;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpB27)->tag == 0){_LL994: _LL995:
# 5740
 return 0;}else{_LL996: _LL997:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpB29);}_LL993:;}default: _LL991: _LL992:
# 5743
 return 0;}_LL98A:;}
# 5747
int Cyc_Tcutil_is_noreturn(void*t){
{void*_tmpB2A=Cyc_Tcutil_compress(t);void*_tmpB2B=_tmpB2A;struct Cyc_List_List*_tmpB2F;void*_tmpB2E;switch(*((int*)_tmpB2B)){case 5: _LL999: _tmpB2E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB2B)->f1).elt_typ;_LL99A:
 return Cyc_Tcutil_is_noreturn(_tmpB2E);case 9: _LL99B: _tmpB2F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB2B)->f1).attributes;_LL99C:
# 5751
 for(0;_tmpB2F != 0;_tmpB2F=_tmpB2F->tl){
void*_tmpB2C=(void*)_tmpB2F->hd;void*_tmpB2D=_tmpB2C;if(((struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmpB2D)->tag == 4){_LL9A0: _LL9A1:
 return 1;}else{_LL9A2: _LL9A3:
 continue;}_LL99F:;}
# 5756
goto _LL998;default: _LL99D: _LL99E:
 goto _LL998;}_LL998:;}
# 5759
return 0;}
# 5764
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmpB30=Cyc_Tcutil_compress(t);void*_tmpB31=_tmpB30;struct Cyc_List_List**_tmpB37;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB31)->tag == 9){_LL9A5: _tmpB37=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB31)->f1).attributes;_LL9A6: {
# 5767
struct Cyc_List_List*_tmpB32=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Tcutil_attribute_cmp,*_tmpB37,(void*)atts->hd))
*_tmpB37=({struct Cyc_List_List*_tmpB33=_cycalloc(sizeof(*_tmpB33));_tmpB33->hd=(void*)atts->hd;_tmpB33->tl=*_tmpB37;_tmpB33;});}else{
# 5774
_tmpB32=({struct Cyc_List_List*_tmpB34=_cycalloc(sizeof(*_tmpB34));_tmpB34->hd=(void*)atts->hd;_tmpB34->tl=_tmpB32;_tmpB34;});}}
return _tmpB32;}}else{_LL9A7: _LL9A8:
({void*_tmpB35=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpB36="transfer_fn_type_atts";_tag_dyneither(_tmpB36,sizeof(char),22);}),_tag_dyneither(_tmpB35,sizeof(void*),0));});}_LL9A4:;}
# 5781
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
struct Cyc_Absyn_Exp*bound=0;
{void*_tmpB38=Cyc_Tcutil_compress(t);void*_tmpB39=_tmpB38;struct Cyc_Absyn_Exp*_tmpB3E;struct Cyc_Absyn_PtrInfo*_tmpB3D;switch(*((int*)_tmpB39)){case 5: _LL9AA: _tmpB3D=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB39)->f1;_LL9AB:
# 5785
{void*_tmpB3A=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)((_tmpB3D->ptr_atts).bounds);void*_tmpB3B=_tmpB3A;struct Cyc_Absyn_Exp*_tmpB3C;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpB3B)->tag == 1){_LL9B1: _tmpB3C=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpB3B)->f1;_LL9B2:
 bound=_tmpB3C;goto _LL9B0;}else{_LL9B3: _LL9B4:
 goto _LL9B0;}_LL9B0:;}
# 5789
goto _LL9A9;case 8: _LL9AC: _tmpB3E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB39)->f1).num_elts;_LL9AD:
# 5791
 bound=_tmpB3E;
goto _LL9A9;default: _LL9AE: _LL9AF:
 goto _LL9A9;}_LL9A9:;}
# 5795
return bound;}
# 5800
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
{void*_tmpB3F=b;struct Cyc_Absyn_Vardecl*_tmpB43;struct Cyc_Absyn_Vardecl*_tmpB42;struct Cyc_Absyn_Vardecl*_tmpB41;struct Cyc_Absyn_Vardecl*_tmpB40;switch(*((int*)_tmpB3F)){case 5: _LL9B6: _tmpB40=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpB3F)->f1;_LL9B7:
 _tmpB41=_tmpB40;goto _LL9B9;case 4: _LL9B8: _tmpB41=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpB3F)->f1;_LL9B9:
 _tmpB42=_tmpB41;goto _LL9BB;case 3: _LL9BA: _tmpB42=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpB3F)->f1;_LL9BB:
 _tmpB43=_tmpB42;goto _LL9BD;case 1: _LL9BC: _tmpB43=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpB3F)->f1;_LL9BD:
# 5806
 if(!_tmpB43->escapes)return _tmpB43;
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
void*_tmpB48=Cyc_Tcutil_compress(t);void*_tmpB49=_tmpB48;void*_tmpB59;struct Cyc_Absyn_Tqual _tmpB58;struct Cyc_Absyn_Exp*_tmpB57;union Cyc_Absyn_Constraint*_tmpB56;unsigned int _tmpB55;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB49)->tag == 8){_LL9C6: _tmpB59=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB49)->f1).elt_type;_tmpB58=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB49)->f1).tq;_tmpB57=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB49)->f1).num_elts;_tmpB56=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB49)->f1).zero_term;_tmpB55=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB49)->f1).zt_loc;_LL9C7: {
# 5834
void*b;
if(_tmpB57 == 0)
b=(void*)& Cyc_Absyn_DynEither_b_val;else{
# 5838
struct Cyc_Absyn_Exp*bnd=_tmpB57;
if(convert_tag){
if(bnd->topt == 0)
({void*_tmpB4A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpB4B="cannot convert tag without type!";_tag_dyneither(_tmpB4B,sizeof(char),33);}),_tag_dyneither(_tmpB4A,sizeof(void*),0));});{
void*_tmpB4C=Cyc_Tcutil_compress((void*)_check_null(bnd->topt));void*_tmpB4D=_tmpB4C;void*_tmpB52;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpB4D)->tag == 19){_LL9CB: _tmpB52=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpB4D)->f1;_LL9CC:
# 5844
 b=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpB4E=_cycalloc(sizeof(*_tmpB4E));_tmpB4E[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpB4F;_tmpB4F.tag=1;_tmpB4F.f1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmpB52,0),0,Cyc_Absyn_No_coercion,0);_tmpB4F;});_tmpB4E;});
goto _LL9CA;}else{_LL9CD: _LL9CE:
# 5847
 if(Cyc_Tcutil_is_const_exp(bnd))
b=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpB50=_cycalloc(sizeof(*_tmpB50));_tmpB50[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpB51;_tmpB51.tag=1;_tmpB51.f1=bnd;_tmpB51;});_tmpB50;});else{
# 5850
b=(void*)& Cyc_Absyn_DynEither_b_val;}}_LL9CA:;};}else{
# 5854
b=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpB53=_cycalloc(sizeof(*_tmpB53));_tmpB53[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpB54;_tmpB54.tag=1;_tmpB54.f1=bnd;_tmpB54;});_tmpB53;});}}
# 5856
return Cyc_Absyn_atb_typ(_tmpB59,rgn,_tmpB58,b,_tmpB56);}}else{_LL9C8: _LL9C9:
# 5859
 return t;}_LL9C5:;}
# 5864
int Cyc_Tcutil_zeroable_type(void*t){
void*_tmpB5A=Cyc_Tcutil_compress(t);void*_tmpB5B=_tmpB5A;struct Cyc_List_List*_tmpB69;union Cyc_Absyn_AggrInfoU _tmpB68;struct Cyc_List_List*_tmpB67;struct Cyc_List_List*_tmpB66;void*_tmpB65;union Cyc_Absyn_Constraint*_tmpB64;switch(*((int*)_tmpB5B)){case 0: _LL9D0: _LL9D1:
 return 1;case 1: _LL9D2: _LL9D3:
 goto _LL9D5;case 2: _LL9D4: _LL9D5:
 goto _LL9D7;case 3: _LL9D6: _LL9D7:
 goto _LL9D9;case 4: _LL9D8: _LL9D9:
 return 0;case 5: _LL9DA: _tmpB64=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB5B)->f1).ptr_atts).nullable;_LL9DB:
# 5872
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmpB64);case 6: _LL9DC: _LL9DD:
 goto _LL9DF;case 7: _LL9DE: _LL9DF:
 return 1;case 8: _LL9E0: _tmpB65=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB5B)->f1).elt_type;_LL9E1:
 return Cyc_Tcutil_zeroable_type(_tmpB65);case 9: _LL9E2: _LL9E3:
 return 0;case 10: _LL9E4: _tmpB66=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpB5B)->f1;_LL9E5:
# 5878
 for(0;(unsigned int)_tmpB66;_tmpB66=_tmpB66->tl){
if(!Cyc_Tcutil_zeroable_type((*((struct _tuple12*)_tmpB66->hd)).f2))return 0;}
return 1;case 11: _LL9E6: _tmpB68=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpB5B)->f1).aggr_info;_tmpB67=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpB5B)->f1).targs;_LL9E7: {
# 5882
struct Cyc_Absyn_Aggrdecl*_tmpB5C=Cyc_Absyn_get_known_aggrdecl(_tmpB68);
if(_tmpB5C->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB5C->impl))->exist_vars != 0)
return 0;{
struct _RegionHandle _tmpB5D=_new_region("r");struct _RegionHandle*r=& _tmpB5D;_push_region(r);
{struct Cyc_List_List*_tmpB5E=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmpB5C->tvs,_tmpB67);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB5C->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmpB5E,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpB5F=0;_npop_handler(0);return _tmpB5F;}}}{
int _tmpB60=1;_npop_handler(0);return _tmpB60;};}
# 5886
;_pop_region(r);};}case 13: _LL9E8: _LL9E9:
# 5890
 return 1;case 19: _LL9EA: _LL9EB:
 return 1;case 12: _LL9EC: _tmpB69=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpB5B)->f2;_LL9ED:
# 5893
 for(0;_tmpB69 != 0;_tmpB69=_tmpB69->tl){
if(!Cyc_Tcutil_zeroable_type(((struct Cyc_Absyn_Aggrfield*)_tmpB69->hd)->type))return 0;}
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
({struct Cyc_String_pa_PrintArg_struct _tmpB63;_tmpB63.tag=0;_tmpB63.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmpB61[1]={& _tmpB63};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpB62="bad type `%s' in zeroable type";_tag_dyneither(_tmpB62,sizeof(char),31);}),_tag_dyneither(_tmpB61,sizeof(void*),1));});});}_LL9CF:;}
