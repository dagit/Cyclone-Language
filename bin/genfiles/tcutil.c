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
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 33 "position.h"
typedef unsigned int Cyc_Position_seg_t;
# 37
struct _dyneither_ptr Cyc_Position_string_of_segment(unsigned int);
struct Cyc_List_List*Cyc_Position_strings_of_segments(struct Cyc_List_List*);struct Cyc_Position_Error;
# 42
typedef struct Cyc_Position_Error*Cyc_Position_error_t;
struct Cyc_Position_Error*Cyc_Position_mk_err(unsigned int,struct _dyneither_ptr);
# 47
extern int Cyc_Position_num_errors;
extern int Cyc_Position_max_errors;
void Cyc_Position_post_error(struct Cyc_Position_Error*);struct Cyc_Relations_Reln;
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 249
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 317
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 336
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 348
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 369
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 374
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 429 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 445
typedef void*Cyc_Absyn_funcparams_t;
# 448
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 512
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 527
extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;
# 538
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 545
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 563
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 919 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 921
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 929
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 932
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x);
# 936
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
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
# 957
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 959
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;
# 963
extern void*Cyc_Absyn_empty_effect;
# 973
extern struct _tuple2*Cyc_Absyn_datatype_print_arg_qvar;
extern struct _tuple2*Cyc_Absyn_datatype_scanf_arg_qvar;
# 982
extern void*Cyc_Absyn_bounds_one;
# 987
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 1013
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1023
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1030
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned int);
# 1032
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
# 1035
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1045
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1047
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned int);
# 1052
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1058
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
# 1070
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
# 1076
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1116
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);
# 1163
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1165
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1167
struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1169
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
# 1171
int Cyc_Absyn_fntype_att(void*a);struct _tuple13{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;};
# 1175
struct _tuple13 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1177
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
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);struct Cyc_RgnOrder_RgnPO;
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
typedef void*Cyc_Tcenv_resolved_t;struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};
# 70
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 74
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 85
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 105 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(struct _RegionHandle*,void*ret_type,struct Cyc_List_List*args);
# 109
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 113
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 116
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 119
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 133
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
# 136
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
# 174
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 186
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
# 42 "tc.h"
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct Cyc_Absyn_Aggrdecl*);
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct Cyc_Absyn_Enumdecl*);
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
# 118
Cyc_Position_post_error(Cyc_Position_mk_err(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}
# 121
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 124
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
({struct Cyc_String_pa_PrintArg_struct _tmp22;_tmp22.tag=0;_tmp22.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg);({void*_tmp20[1]={& _tmp22};Cyc_fprintf(Cyc_stderr,({const char*_tmp21="Compiler Error (Tcutil::impos): %s\n";_tag_dyneither(_tmp21,sizeof(char),36);}),_tag_dyneither(_tmp20,sizeof(void*),1));});});
Cyc_fflush(Cyc_stderr);
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp23=_cycalloc(sizeof(*_tmp23));_tmp23[0]=({struct Cyc_Core_Impossible_exn_struct _tmp24;_tmp24.tag=Cyc_Core_Impossible;_tmp24.f1=msg;_tmp24;});_tmp23;}));}char Cyc_Tcutil_AbortTypeCheckingFunction[26]="AbortTypeCheckingFunction";
# 132
static struct _dyneither_ptr Cyc_Tcutil_tvar2string(struct Cyc_Absyn_Tvar*tv){
return*tv->name;}
# 136
void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){
for(0;tvs != 0;tvs=tvs->tl){
({struct Cyc_String_pa_PrintArg_struct _tmp28;_tmp28.tag=0;_tmp28.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind));({struct Cyc_String_pa_PrintArg_struct _tmp27;_tmp27.tag=0;_tmp27.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)tvs->hd));({void*_tmp25[2]={& _tmp27,& _tmp28};Cyc_fprintf(Cyc_stderr,({const char*_tmp26="%s::%s ";_tag_dyneither(_tmp26,sizeof(char),8);}),_tag_dyneither(_tmp25,sizeof(void*),2));});});});}
({void*_tmp29=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2A="\n";_tag_dyneither(_tmp2A,sizeof(char),2);}),_tag_dyneither(_tmp29,sizeof(void*),0));});Cyc_fflush(Cyc_stderr);}
# 143
static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;
# 148
void Cyc_Tcutil_warn(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 151
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
Cyc_Tcutil_warning_segs=({struct Cyc_List_List*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B->hd=(void*)sg;_tmp2B->tl=Cyc_Tcutil_warning_segs;_tmp2B;});
Cyc_Tcutil_warning_msgs=({struct Cyc_List_List*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C->hd=({struct _dyneither_ptr*_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D[0]=msg;_tmp2D;});_tmp2C->tl=Cyc_Tcutil_warning_msgs;_tmp2C;});}
# 155
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
# 167
({void*_tmp35=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp36="**************\n";_tag_dyneither(_tmp36,sizeof(char),16);}),_tag_dyneither(_tmp35,sizeof(void*),0));});
Cyc_fflush(Cyc_stderr);};}
# 172
struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set=0;
# 176
static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
return tv1->identity - tv2->identity;}
# 181
void*Cyc_Tcutil_compress(void*t){
void*_tmp37=t;void*_tmp40;struct Cyc_Absyn_Exp*_tmp3F;struct Cyc_Absyn_Exp*_tmp3E;void**_tmp3D;void**_tmp3C;switch(*((int*)_tmp37)){case 1: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp37)->f2 == 0){_LL1: _LL2:
 goto _LL4;}else{_LL7: _tmp3C=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp37)->f2;_LL8: {
# 192
void*ta=(void*)_check_null(*_tmp3C);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp3C=t2;
return t2;}}case 17: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp37)->f4 == 0){_LL3: _LL4:
# 184
 return t;}else{_LL5: _tmp3D=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp37)->f4;_LL6: {
# 186
void*ta=(void*)_check_null(*_tmp3D);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp3D=t2;
return t2;}}case 18: _LL9: _tmp3E=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp37)->f1;_LLA:
# 198
 Cyc_Evexp_eval_const_uint_exp(_tmp3E);{
void*_tmp38=_tmp3E->r;void*_tmp39=_tmp38;void*_tmp3A;if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp39)->tag == 38){_LL12: _tmp3A=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp39)->f1;_LL13:
 return Cyc_Tcutil_compress(_tmp3A);}else{_LL14: _LL15:
 return t;}_LL11:;};case 27: _LLB: _tmp3F=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp37)->f1;_LLC: {
# 204
void*_tmp3B=_tmp3F->topt;
if(_tmp3B != 0)return _tmp3B;else{
return t;}}case 26: if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp37)->f2 != 0){_LLD: _tmp40=*((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp37)->f2;_LLE:
# 208
 return Cyc_Tcutil_compress(_tmp40);}else{goto _LLF;}default: _LLF: _LL10:
 return t;}_LL0:;}
# 216
void*Cyc_Tcutil_copy_type(void*t);
static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){
return((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);}
# 220
static union Cyc_Absyn_Constraint*Cyc_Tcutil_copy_conref(union Cyc_Absyn_Constraint*cptr){
union Cyc_Absyn_Constraint*_tmp41=cptr;union Cyc_Absyn_Constraint*_tmp43;void*_tmp42;switch((((union Cyc_Absyn_Constraint*)_tmp41)->Eq_constr).tag){case 3: _LL17: _LL18:
 return Cyc_Absyn_empty_conref();case 1: _LL19: _tmp42=(void*)(_tmp41->Eq_constr).val;_LL1A:
 return Cyc_Absyn_new_conref(_tmp42);default: _LL1B: _tmp43=(_tmp41->Forward_constr).val;_LL1C:
 return Cyc_Tcutil_copy_conref(_tmp43);}_LL16:;}
# 227
static void*Cyc_Tcutil_copy_kindbound(void*kb){
void*_tmp44=Cyc_Absyn_compress_kb(kb);void*_tmp45=_tmp44;struct Cyc_Absyn_Kind*_tmp4A;switch(*((int*)_tmp45)){case 1: _LL1E: _LL1F:
 return(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46[0]=({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp47;_tmp47.tag=1;_tmp47.f1=0;_tmp47;});_tmp46;});case 2: _LL20: _tmp4A=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp45)->f2;_LL21:
 return(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp48=_cycalloc(sizeof(*_tmp48));_tmp48[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp49;_tmp49.tag=2;_tmp49.f1=0;_tmp49.f2=_tmp4A;_tmp49;});_tmp48;});default: _LL22: _LL23:
 return kb;}_LL1D:;}
# 234
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 236
return({struct Cyc_Absyn_Tvar*_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B->name=tv->name;_tmp4B->identity=- 1;_tmp4B->kind=Cyc_Tcutil_copy_kindbound(tv->kind);_tmp4B;});}
# 238
static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
# 240
struct _tuple10*_tmp4C=arg;struct _dyneither_ptr*_tmp50;struct Cyc_Absyn_Tqual _tmp4F;void*_tmp4E;_LL25: _tmp50=_tmp4C->f1;_tmp4F=_tmp4C->f2;_tmp4E=_tmp4C->f3;_LL26:;
return({struct _tuple10*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D->f1=_tmp50;_tmp4D->f2=_tmp4F;_tmp4D->f3=Cyc_Tcutil_copy_type(_tmp4E);_tmp4D;});}
# 243
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
struct _tuple12*_tmp51=arg;struct Cyc_Absyn_Tqual _tmp54;void*_tmp53;_LL28: _tmp54=_tmp51->f1;_tmp53=_tmp51->f2;_LL29:;
return({struct _tuple12*_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52->f1=_tmp54;_tmp52->f2=Cyc_Tcutil_copy_type(_tmp53);_tmp52;});}
# 247
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*);
# 249
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
return({struct Cyc_Absyn_Aggrfield*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55->name=f->name;_tmp55->tq=f->tq;_tmp55->type=Cyc_Tcutil_copy_type(f->type);_tmp55->width=f->width;_tmp55->attributes=f->attributes;_tmp55->requires_clause=
Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause);_tmp55;});}
# 253
static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
struct _tuple0*_tmp56=x;void*_tmp59;void*_tmp58;_LL2B: _tmp59=_tmp56->f1;_tmp58=_tmp56->f2;_LL2C:;
return({struct _tuple0*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->f1=Cyc_Tcutil_copy_type(_tmp59);_tmp57->f2=Cyc_Tcutil_copy_type(_tmp58);_tmp57;});}
# 257
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){
return({struct Cyc_Absyn_Enumfield*_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A->name=f->name;_tmp5A->tag=f->tag;_tmp5A->loc=f->loc;_tmp5A;});}
# 260
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp5B=_cycalloc(sizeof(*_tmp5B));_tmp5B[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp5C;_tmp5C.tag=2;_tmp5C.f1=Cyc_Tcutil_copy_tvar(t);_tmp5C;});_tmp5B;});}
# 264
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp5D=Cyc_Tcutil_compress(t);void*_tmp5E=_tmp5D;struct Cyc_Absyn_Datatypedecl*_tmpE4;struct Cyc_Absyn_Enumdecl*_tmpE3;struct Cyc_Absyn_Aggrdecl*_tmpE2;void*_tmpE1;struct Cyc_List_List*_tmpE0;void*_tmpDF;struct _tuple2*_tmpDE;struct Cyc_List_List*_tmpDD;struct Cyc_Absyn_Typedefdecl*_tmpDC;void*_tmpDB;void*_tmpDA;void*_tmpD9;struct Cyc_Absyn_Exp*_tmpD8;struct Cyc_Absyn_Exp*_tmpD7;void*_tmpD6;struct Cyc_List_List*_tmpD5;struct _tuple2*_tmpD4;struct Cyc_Absyn_Enumdecl*_tmpD3;enum Cyc_Absyn_AggrKind _tmpD2;struct Cyc_List_List*_tmpD1;struct Cyc_Absyn_Aggrdecl**_tmpD0;struct Cyc_List_List*_tmpCF;enum Cyc_Absyn_AggrKind _tmpCE;struct _tuple2*_tmpCD;struct Cyc_Core_Opt*_tmpCC;struct Cyc_List_List*_tmpCB;struct Cyc_List_List*_tmpCA;struct Cyc_List_List*_tmpC9;void*_tmpC8;struct Cyc_Absyn_Tqual _tmpC7;void*_tmpC6;struct Cyc_List_List*_tmpC5;int _tmpC4;struct Cyc_Absyn_VarargInfo*_tmpC3;struct Cyc_List_List*_tmpC2;struct Cyc_List_List*_tmpC1;struct Cyc_Absyn_Exp*_tmpC0;struct Cyc_List_List*_tmpBF;struct Cyc_Absyn_Exp*_tmpBE;struct Cyc_List_List*_tmpBD;void*_tmpBC;struct Cyc_Absyn_Tqual _tmpBB;struct Cyc_Absyn_Exp*_tmpBA;union Cyc_Absyn_Constraint*_tmpB9;unsigned int _tmpB8;void*_tmpB7;struct Cyc_Absyn_Tqual _tmpB6;void*_tmpB5;union Cyc_Absyn_Constraint*_tmpB4;union Cyc_Absyn_Constraint*_tmpB3;union Cyc_Absyn_Constraint*_tmpB2;struct Cyc_Absyn_PtrLoc*_tmpB1;union Cyc_Absyn_DatatypeFieldInfoU _tmpB0;struct Cyc_List_List*_tmpAF;union Cyc_Absyn_DatatypeInfoU _tmpAE;struct Cyc_List_List*_tmpAD;struct Cyc_Absyn_Tvar*_tmpAC;switch(*((int*)_tmp5E)){case 0: _LL2E: _LL2F:
 goto _LL31;case 1: _LL30: _LL31:
 return t;case 2: _LL32: _tmpAC=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp5E)->f1;_LL33:
 return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp60;_tmp60.tag=2;_tmp60.f1=Cyc_Tcutil_copy_tvar(_tmpAC);_tmp60;});_tmp5F;});case 3: _LL34: _tmpAE=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5E)->f1).datatype_info;_tmpAD=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5E)->f1).targs;_LL35:
# 270
 return(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61[0]=({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp62;_tmp62.tag=3;_tmp62.f1=({struct Cyc_Absyn_DatatypeInfo _tmp63;_tmp63.datatype_info=_tmpAE;_tmp63.targs=Cyc_Tcutil_copy_types(_tmpAD);_tmp63;});_tmp62;});_tmp61;});case 4: _LL36: _tmpB0=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp5E)->f1).field_info;_tmpAF=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp5E)->f1).targs;_LL37:
# 272
 return(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64[0]=({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp65;_tmp65.tag=4;_tmp65.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmp66;_tmp66.field_info=_tmpB0;_tmp66.targs=Cyc_Tcutil_copy_types(_tmpAF);_tmp66;});_tmp65;});_tmp64;});case 5: _LL38: _tmpB7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).elt_typ;_tmpB6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).elt_tq;_tmpB5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).ptr_atts).rgn;_tmpB4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).ptr_atts).nullable;_tmpB3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).ptr_atts).bounds;_tmpB2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).ptr_atts).zero_term;_tmpB1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).ptr_atts).ptrloc;_LL39: {
# 274
void*_tmp67=Cyc_Tcutil_copy_type(_tmpB7);
void*_tmp68=Cyc_Tcutil_copy_type(_tmpB5);
union Cyc_Absyn_Constraint*_tmp69=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmpB4);
struct Cyc_Absyn_Tqual _tmp6A=_tmpB6;
# 279
union Cyc_Absyn_Constraint*_tmp6B=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmpB3);
union Cyc_Absyn_Constraint*_tmp6C=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmpB2);
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp6E;_tmp6E.tag=5;_tmp6E.f1=({struct Cyc_Absyn_PtrInfo _tmp6F;_tmp6F.elt_typ=_tmp67;_tmp6F.elt_tq=_tmp6A;_tmp6F.ptr_atts=({(_tmp6F.ptr_atts).rgn=_tmp68;(_tmp6F.ptr_atts).nullable=_tmp69;(_tmp6F.ptr_atts).bounds=_tmp6B;(_tmp6F.ptr_atts).zero_term=_tmp6C;(_tmp6F.ptr_atts).ptrloc=_tmpB1;_tmp6F.ptr_atts;});_tmp6F;});_tmp6E;});_tmp6D;});}case 6: _LL3A: _LL3B:
 goto _LL3D;case 7: _LL3C: _LL3D:
 return t;case 8: _LL3E: _tmpBC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E)->f1).elt_type;_tmpBB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E)->f1).tq;_tmpBA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E)->f1).num_elts;_tmpB9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E)->f1).zero_term;_tmpB8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E)->f1).zt_loc;_LL3F: {
# 285
struct Cyc_Absyn_Exp*eopt2=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpBA);
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp71;_tmp71.tag=8;_tmp71.f1=({struct Cyc_Absyn_ArrayInfo _tmp72;_tmp72.elt_type=Cyc_Tcutil_copy_type(_tmpBC);_tmp72.tq=_tmpBB;_tmp72.num_elts=eopt2;_tmp72.zero_term=
((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmpB9);_tmp72.zt_loc=_tmpB8;_tmp72;});_tmp71;});_tmp70;});}case 9: _LL40: _tmpC9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).tvars;_tmpC8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).effect;_tmpC7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).ret_tqual;_tmpC6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).ret_typ;_tmpC5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).args;_tmpC4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).c_varargs;_tmpC3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).cyc_varargs;_tmpC2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).rgn_po;_tmpC1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).attributes;_tmpC0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).requires_clause;_tmpBF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).requires_relns;_tmpBE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).ensures_clause;_tmpBD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).ensures_relns;_LL41: {
# 289
struct Cyc_List_List*_tmp73=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmpC9);
void*effopt2=_tmpC8 == 0?0: Cyc_Tcutil_copy_type(_tmpC8);
void*_tmp74=Cyc_Tcutil_copy_type(_tmpC6);
struct Cyc_List_List*_tmp75=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmpC5);
int _tmp76=_tmpC4;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(_tmpC3 != 0){
struct Cyc_Absyn_VarargInfo*cv=_tmpC3;
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->name=cv->name;_tmp77->tq=cv->tq;_tmp77->type=Cyc_Tcutil_copy_type(cv->type);_tmp77->inject=cv->inject;_tmp77;});}{
# 300
struct Cyc_List_List*_tmp78=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmpC2);
struct Cyc_List_List*_tmp79=_tmpC1;
struct Cyc_Absyn_Exp*_tmp7A=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpC0);
struct Cyc_List_List*_tmp7B=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpBF);
struct Cyc_Absyn_Exp*_tmp7C=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpBE);
struct Cyc_List_List*_tmp7D=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpBD);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp7F;_tmp7F.tag=9;_tmp7F.f1=({struct Cyc_Absyn_FnInfo _tmp80;_tmp80.tvars=_tmp73;_tmp80.effect=effopt2;_tmp80.ret_tqual=_tmpC7;_tmp80.ret_typ=_tmp74;_tmp80.args=_tmp75;_tmp80.c_varargs=_tmp76;_tmp80.cyc_varargs=cyc_varargs2;_tmp80.rgn_po=_tmp78;_tmp80.attributes=_tmp79;_tmp80.requires_clause=_tmp7A;_tmp80.requires_relns=_tmp7B;_tmp80.ensures_clause=_tmp7C;_tmp80.ensures_relns=_tmp7D;_tmp80;});_tmp7F;});_tmp7E;});};}case 10: _LL42: _tmpCA=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5E)->f1;_LL43:
# 309
 return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81[0]=({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp82;_tmp82.tag=10;_tmp82.f1=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmpCA);_tmp82;});_tmp81;});case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5E)->f1).aggr_info).UnknownAggr).tag == 1){_LL44: _tmpCE=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5E)->f1).aggr_info).UnknownAggr).val).f1;_tmpCD=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5E)->f1).aggr_info).UnknownAggr).val).f2;_tmpCC=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5E)->f1).aggr_info).UnknownAggr).val).f3;_tmpCB=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5E)->f1).targs;_LL45:
# 311
 return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp84;_tmp84.tag=11;_tmp84.f1=({struct Cyc_Absyn_AggrInfo _tmp85;_tmp85.aggr_info=Cyc_Absyn_UnknownAggr(_tmpCE,_tmpCD,_tmpCC);_tmp85.targs=Cyc_Tcutil_copy_types(_tmpCB);_tmp85;});_tmp84;});_tmp83;});}else{_LL46: _tmpD0=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5E)->f1).aggr_info).KnownAggr).val;_tmpCF=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5E)->f1).targs;_LL47:
# 313
 return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp87;_tmp87.tag=11;_tmp87.f1=({struct Cyc_Absyn_AggrInfo _tmp88;_tmp88.aggr_info=Cyc_Absyn_KnownAggr(_tmpD0);_tmp88.targs=Cyc_Tcutil_copy_types(_tmpCF);_tmp88;});_tmp87;});_tmp86;});}case 12: _LL48: _tmpD2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5E)->f1;_tmpD1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5E)->f2;_LL49:
 return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89[0]=({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp8A;_tmp8A.tag=12;_tmp8A.f1=_tmpD2;_tmp8A.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmpD1);_tmp8A;});_tmp89;});case 13: _LL4A: _tmpD4=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5E)->f1;_tmpD3=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5E)->f2;_LL4B:
 return(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B[0]=({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp8C;_tmp8C.tag=13;_tmp8C.f1=_tmpD4;_tmp8C.f2=_tmpD3;_tmp8C;});_tmp8B;});case 14: _LL4C: _tmpD5=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp5E)->f1;_LL4D:
 return(void*)({struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D[0]=({struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmp8E;_tmp8E.tag=14;_tmp8E.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmpD5);_tmp8E;});_tmp8D;});case 19: _LL4E: _tmpD6=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5E)->f1;_LL4F:
 return(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F[0]=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp90;_tmp90.tag=19;_tmp90.f1=Cyc_Tcutil_copy_type(_tmpD6);_tmp90;});_tmp8F;});case 18: _LL50: _tmpD7=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp5E)->f1;_LL51:
# 320
 return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp92;_tmp92.tag=18;_tmp92.f1=_tmpD7;_tmp92;});_tmp91;});case 27: _LL52: _tmpD8=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp5E)->f1;_LL53:
# 323
 return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93[0]=({struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmp94;_tmp94.tag=27;_tmp94.f1=_tmpD8;_tmp94;});_tmp93;});case 15: _LL54: _tmpD9=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5E)->f1;_LL55:
 return(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95[0]=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp96;_tmp96.tag=15;_tmp96.f1=Cyc_Tcutil_copy_type(_tmpD9);_tmp96;});_tmp95;});case 16: _LL56: _tmpDB=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp5E)->f1;_tmpDA=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp5E)->f2;_LL57:
 return(void*)({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97[0]=({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp98;_tmp98.tag=16;_tmp98.f1=Cyc_Tcutil_copy_type(_tmpDB);_tmp98.f2=Cyc_Tcutil_copy_type(_tmpDA);_tmp98;});_tmp97;});case 17: _LL58: _tmpDE=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5E)->f1;_tmpDD=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5E)->f2;_tmpDC=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5E)->f3;_LL59:
# 327
 return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99[0]=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp9A;_tmp9A.tag=17;_tmp9A.f1=_tmpDE;_tmp9A.f2=Cyc_Tcutil_copy_types(_tmpDD);_tmp9A.f3=_tmpDC;_tmp9A.f4=0;_tmp9A;});_tmp99;});case 21: _LL5A: _LL5B:
 goto _LL5D;case 22: _LL5C: _LL5D:
 goto _LL5F;case 28: _LL5E: _LL5F:
 goto _LL61;case 20: _LL60: _LL61:
 return t;case 23: _LL62: _tmpDF=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp5E)->f1;_LL63:
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp9B=_cycalloc(sizeof(*_tmp9B));_tmp9B[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp9C;_tmp9C.tag=23;_tmp9C.f1=Cyc_Tcutil_copy_type(_tmpDF);_tmp9C;});_tmp9B;});case 24: _LL64: _tmpE0=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp5E)->f1;_LL65:
 return(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp9E;_tmp9E.tag=24;_tmp9E.f1=Cyc_Tcutil_copy_types(_tmpE0);_tmp9E;});_tmp9D;});case 25: _LL66: _tmpE1=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp5E)->f1;_LL67:
 return(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpA0;_tmpA0.tag=25;_tmpA0.f1=Cyc_Tcutil_copy_type(_tmpE1);_tmpA0;});_tmp9F;});default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5E)->f1)->r)){case 0: _LL68: _tmpE2=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5E)->f1)->r)->f1;_LL69: {
# 337
struct Cyc_List_List*_tmpA1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpE2->tvs);
return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpA3;_tmpA3.tag=11;_tmpA3.f1=({struct Cyc_Absyn_AggrInfo _tmpA4;_tmpA4.aggr_info=Cyc_Absyn_UnknownAggr(_tmpE2->kind,_tmpE2->name,0);_tmpA4.targs=_tmpA1;_tmpA4;});_tmpA3;});_tmpA2;});}case 1: _LL6A: _tmpE3=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5E)->f1)->r)->f1;_LL6B:
# 340
 return(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5[0]=({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpA6;_tmpA6.tag=13;_tmpA6.f1=_tmpE3->name;_tmpA6.f2=0;_tmpA6;});_tmpA5;});default: _LL6C: _tmpE4=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5E)->f1)->r)->f1;_LL6D: {
# 342
struct Cyc_List_List*_tmpA7=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpE4->tvs);
return(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8[0]=({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpA9;_tmpA9.tag=3;_tmpA9.f1=({struct Cyc_Absyn_DatatypeInfo _tmpAA;_tmpAA.datatype_info=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmpAB;_tmpAB.name=_tmpE4->name;_tmpAB.is_extensible=0;_tmpAB;}));_tmpAA.targs=_tmpA7;_tmpAA;});_tmpA9;});_tmpA8;});}}}_LL2D:;}
# 357 "tcutil.cyc"
static void*Cyc_Tcutil_copy_designator(int pt,void*d){
void*_tmpE5=d;struct _dyneither_ptr*_tmpE9;struct Cyc_Absyn_Exp*_tmpE8;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpE5)->tag == 0){_LL6F: _tmpE8=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpE5)->f1;_LL70:
 return(void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpE6=_cycalloc(sizeof(*_tmpE6));_tmpE6[0]=({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpE7;_tmpE7.tag=0;_tmpE7.f1=Cyc_Tcutil_deep_copy_exp(pt,_tmpE8);_tmpE7;});_tmpE6;});}else{_LL71: _tmpE9=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpE5)->f1;_LL72:
 return d;}_LL6E:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 363
static struct _tuple19*Cyc_Tcutil_copy_eds(int pt,struct _tuple19*e){
# 365
return({struct _tuple19*_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA->f1=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,pt,(e[0]).f1);_tmpEA->f2=Cyc_Tcutil_deep_copy_exp(pt,(e[0]).f2);_tmpEA;});}
# 368
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*new_e;
int _tmpEB=preserve_types;
{void*_tmpEC=e->r;void*_tmpED=_tmpEC;int _tmp179;struct _dyneither_ptr _tmp178;void*_tmp177;struct Cyc_Absyn_Exp*_tmp176;struct _dyneither_ptr*_tmp175;struct Cyc_Core_Opt*_tmp174;struct Cyc_List_List*_tmp173;struct Cyc_Absyn_Exp*_tmp172;struct Cyc_Absyn_Exp*_tmp171;int _tmp170;struct Cyc_Absyn_Exp*_tmp16F;void**_tmp16E;struct Cyc_Absyn_Exp*_tmp16D;int _tmp16C;int _tmp16B;void*_tmp16A;struct Cyc_Absyn_Enumfield*_tmp169;struct Cyc_Absyn_Enumdecl*_tmp168;struct Cyc_Absyn_Enumfield*_tmp167;struct Cyc_List_List*_tmp166;struct Cyc_Absyn_Datatypedecl*_tmp165;struct Cyc_Absyn_Datatypefield*_tmp164;void*_tmp163;struct Cyc_List_List*_tmp162;struct _tuple2*_tmp161;struct Cyc_List_List*_tmp160;struct Cyc_List_List*_tmp15F;struct Cyc_Absyn_Aggrdecl*_tmp15E;struct Cyc_Absyn_Exp*_tmp15D;void*_tmp15C;int _tmp15B;struct Cyc_Absyn_Vardecl*_tmp15A;struct Cyc_Absyn_Exp*_tmp159;struct Cyc_Absyn_Exp*_tmp158;int _tmp157;struct Cyc_List_List*_tmp156;struct _dyneither_ptr*_tmp155;struct Cyc_Absyn_Tqual _tmp154;void*_tmp153;struct Cyc_List_List*_tmp152;struct Cyc_List_List*_tmp151;struct Cyc_Absyn_Exp*_tmp150;struct Cyc_Absyn_Exp*_tmp14F;struct Cyc_Absyn_Exp*_tmp14E;struct _dyneither_ptr*_tmp14D;int _tmp14C;int _tmp14B;struct Cyc_Absyn_Exp*_tmp14A;struct _dyneither_ptr*_tmp149;int _tmp148;int _tmp147;struct Cyc_Absyn_Exp*_tmp146;void*_tmp145;struct Cyc_List_List*_tmp144;struct Cyc_Absyn_Exp*_tmp143;void*_tmp142;struct Cyc_Absyn_Exp*_tmp141;struct Cyc_Absyn_Exp*_tmp140;struct Cyc_Absyn_Exp*_tmp13F;void*_tmp13E;struct Cyc_Absyn_Exp*_tmp13D;int _tmp13C;enum Cyc_Absyn_Coercion _tmp13B;struct Cyc_Absyn_Exp*_tmp13A;struct Cyc_List_List*_tmp139;struct Cyc_Absyn_Exp*_tmp138;struct Cyc_Absyn_Exp*_tmp137;int _tmp136;struct Cyc_Absyn_Exp*_tmp135;struct Cyc_List_List*_tmp134;struct Cyc_Absyn_VarargCallInfo*_tmp133;int _tmp132;struct Cyc_Absyn_Exp*_tmp131;struct Cyc_Absyn_Exp*_tmp130;struct Cyc_Absyn_Exp*_tmp12F;struct Cyc_Absyn_Exp*_tmp12E;struct Cyc_Absyn_Exp*_tmp12D;struct Cyc_Absyn_Exp*_tmp12C;struct Cyc_Absyn_Exp*_tmp12B;struct Cyc_Absyn_Exp*_tmp12A;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_Absyn_Exp*_tmp128;enum Cyc_Absyn_Incrementor _tmp127;struct Cyc_Absyn_Exp*_tmp126;struct Cyc_Core_Opt*_tmp125;struct Cyc_Absyn_Exp*_tmp124;enum Cyc_Absyn_Primop _tmp123;struct Cyc_List_List*_tmp122;void*_tmp121;union Cyc_Absyn_Cnst _tmp120;switch(*((int*)_tmpED)){case 0: _LL74: _tmp120=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LL75:
 new_e=Cyc_Absyn_const_exp(_tmp120,e->loc);goto _LL73;case 1: _LL76: _tmp121=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LL77:
 new_e=Cyc_Absyn_varb_exp(_tmp121,e->loc);goto _LL73;case 2: _LL78: _tmp123=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp122=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL79:
 new_e=Cyc_Absyn_primop_exp(_tmp123,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpEB,_tmp122),e->loc);goto _LL73;case 3: _LL7A: _tmp126=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp125=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp124=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_LL7B:
# 376
 new_e=Cyc_Absyn_assignop_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp126),(unsigned int)_tmp125?({struct Cyc_Core_Opt*_tmpEE=_cycalloc_atomic(sizeof(*_tmpEE));_tmpEE->v=(void*)_tmp125->v;_tmpEE;}): 0,Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp124),e->loc);
goto _LL73;case 4: _LL7C: _tmp128=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp127=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL7D:
 new_e=Cyc_Absyn_increment_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp128),_tmp127,e->loc);goto _LL73;case 5: _LL7E: _tmp12B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp12A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp129=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_LL7F:
# 380
 new_e=Cyc_Absyn_conditional_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp12B),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp12A),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp129),e->loc);goto _LL73;case 6: _LL80: _tmp12D=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp12C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL81:
 new_e=Cyc_Absyn_and_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp12D),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp12C),e->loc);goto _LL73;case 7: _LL82: _tmp12F=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp12E=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL83:
 new_e=Cyc_Absyn_or_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp12F),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp12E),e->loc);goto _LL73;case 8: _LL84: _tmp131=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp130=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL85:
 new_e=Cyc_Absyn_seq_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp131),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp130),e->loc);goto _LL73;case 9: _LL86: _tmp135=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp134=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp133=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_tmp132=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpED)->f4;_LL87:
# 385
{struct Cyc_Absyn_VarargCallInfo*_tmpEF=_tmp133;int _tmpFD;struct Cyc_List_List*_tmpFC;struct Cyc_Absyn_VarargInfo*_tmpFB;if(_tmpEF != 0){_LLC5: _tmpFD=_tmpEF->num_varargs;_tmpFC=_tmpEF->injectors;_tmpFB=_tmpEF->vai;_LLC6: {
# 387
struct Cyc_Absyn_VarargInfo*_tmpF0=_tmpFB;struct _dyneither_ptr*_tmpF8;struct Cyc_Absyn_Tqual _tmpF7;void*_tmpF6;int _tmpF5;_LLCA: _tmpF8=_tmpF0->name;_tmpF7=_tmpF0->tq;_tmpF6=_tmpF0->type;_tmpF5=_tmpF0->inject;_LLCB:;
new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpF1=_cycalloc(sizeof(*_tmpF1));_tmpF1[0]=({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpF2;_tmpF2.tag=9;_tmpF2.f1=
Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp135);_tmpF2.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpEB,_tmp134);_tmpF2.f3=({struct Cyc_Absyn_VarargCallInfo*_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3->num_varargs=_tmpFD;_tmpF3->injectors=_tmpFC;_tmpF3->vai=({struct Cyc_Absyn_VarargInfo*_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4->name=_tmpF8;_tmpF4->tq=_tmpF7;_tmpF4->type=
# 391
Cyc_Tcutil_copy_type(_tmpF6);_tmpF4->inject=_tmpF5;_tmpF4;});_tmpF3;});_tmpF2.f4=_tmp132;_tmpF2;});_tmpF1;}),e->loc);
# 393
goto _LLC4;}}else{_LLC7: _LLC8:
# 395
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9[0]=({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpFA;_tmpFA.tag=9;_tmpFA.f1=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp135);_tmpFA.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpEB,_tmp134);_tmpFA.f3=_tmp133;_tmpFA.f4=_tmp132;_tmpFA;});_tmpF9;}),e->loc);
goto _LLC4;}_LLC4:;}
# 398
goto _LL73;case 10: _LL88: _tmp137=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp136=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL89:
# 400
 new_e=_tmp136?Cyc_Absyn_rethrow_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp137),e->loc): Cyc_Absyn_throw_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp137),e->loc);
goto _LL73;case 11: _LL8A: _tmp138=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LL8B:
 new_e=Cyc_Absyn_noinstantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp138),e->loc);
goto _LL73;case 12: _LL8C: _tmp13A=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp139=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL8D:
# 405
 new_e=Cyc_Absyn_instantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp13A),((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp139),e->loc);
goto _LL73;case 13: _LL8E: _tmp13E=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp13D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp13C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_tmp13B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpED)->f4;_LL8F:
# 408
 new_e=Cyc_Absyn_cast_exp(Cyc_Tcutil_copy_type(_tmp13E),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp13D),_tmp13C,_tmp13B,e->loc);goto _LL73;case 14: _LL90: _tmp13F=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LL91:
 new_e=Cyc_Absyn_address_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp13F),e->loc);goto _LL73;case 15: _LL92: _tmp141=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp140=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL93: {
# 411
struct Cyc_Absyn_Exp*eo1=_tmp141;if(_tmp141 != 0)eo1=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp141);
new_e=Cyc_Absyn_New_exp(eo1,Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp140),e->loc);
goto _LL73;}case 16: _LL94: _tmp142=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LL95:
 new_e=Cyc_Absyn_sizeoftyp_exp(Cyc_Tcutil_copy_type(_tmp142),e->loc);
goto _LL73;case 17: _LL96: _tmp143=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LL97:
 new_e=Cyc_Absyn_sizeofexp_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp143),e->loc);goto _LL73;case 18: _LL98: _tmp145=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp144=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL99:
# 418
 new_e=Cyc_Absyn_offsetof_exp(Cyc_Tcutil_copy_type(_tmp145),_tmp144,e->loc);goto _LL73;case 19: _LL9A: _tmp146=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LL9B:
 new_e=Cyc_Absyn_deref_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp146),e->loc);goto _LL73;case 20: _LL9C: _tmp14A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp149=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp148=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_tmp147=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpED)->f4;_LL9D:
# 421
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpFE=_cycalloc(sizeof(*_tmpFE));_tmpFE[0]=({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpFF;_tmpFF.tag=20;_tmpFF.f1=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp14A);_tmpFF.f2=_tmp149;_tmpFF.f3=_tmp148;_tmpFF.f4=_tmp147;_tmpFF;});_tmpFE;}),e->loc);goto _LL73;case 21: _LL9E: _tmp14E=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp14D=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp14C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_tmp14B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpED)->f4;_LL9F:
# 423
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp100=_cycalloc(sizeof(*_tmp100));_tmp100[0]=({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmp101;_tmp101.tag=21;_tmp101.f1=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp14E);_tmp101.f2=_tmp14D;_tmp101.f3=_tmp14C;_tmp101.f4=_tmp14B;_tmp101;});_tmp100;}),e->loc);goto _LL73;case 22: _LLA0: _tmp150=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp14F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LLA1:
 new_e=Cyc_Absyn_subscript_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp150),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp14F),e->loc);
goto _LL73;case 23: _LLA2: _tmp151=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LLA3:
 new_e=Cyc_Absyn_tuple_exp(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpEB,_tmp151),e->loc);goto _LL73;case 24: _LLA4: _tmp155=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpED)->f1)->f1;_tmp154=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpED)->f1)->f2;_tmp153=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpED)->f1)->f3;_tmp152=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LLA5: {
# 428
struct _dyneither_ptr*vopt1=_tmp155;
if(_tmp155 != 0)vopt1=_tmp155;
new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp102=_cycalloc(sizeof(*_tmp102));_tmp102[0]=({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmp103;_tmp103.tag=24;_tmp103.f1=({struct _tuple10*_tmp104=_cycalloc(sizeof(*_tmp104));_tmp104->f1=vopt1;_tmp104->f2=_tmp154;_tmp104->f3=Cyc_Tcutil_copy_type(_tmp153);_tmp104;});_tmp103.f2=
((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpEB,_tmp152);_tmp103;});_tmp102;}),e->loc);
goto _LL73;}case 25: _LLA6: _tmp156=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LLA7:
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp105=_cycalloc(sizeof(*_tmp105));_tmp105[0]=({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp106;_tmp106.tag=25;_tmp106.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpEB,_tmp156);_tmp106;});_tmp105;}),e->loc);
goto _LL73;case 26: _LLA8: _tmp15A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp159=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp158=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_tmp157=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpED)->f4;_LLA9:
# 436
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp107=_cycalloc(sizeof(*_tmp107));_tmp107[0]=({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmp108;_tmp108.tag=26;_tmp108.f1=_tmp15A;_tmp108.f2=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp159);_tmp108.f3=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp158);_tmp108.f4=_tmp157;_tmp108;});_tmp107;}),e->loc);
goto _LL73;case 27: _LLAA: _tmp15D=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp15C=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp15B=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_LLAB:
# 439
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp109=_cycalloc(sizeof(*_tmp109));_tmp109[0]=({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmp10A;_tmp10A.tag=27;_tmp10A.f1=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp15D);_tmp10A.f2=Cyc_Tcutil_copy_type(_tmp15C);_tmp10A.f3=_tmp15B;_tmp10A;});_tmp109;}),_tmp15D->loc);
# 441
goto _LL73;case 28: _LLAC: _tmp161=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp160=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp15F=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_tmp15E=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpED)->f4;_LLAD:
# 443
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp10B=_cycalloc(sizeof(*_tmp10B));_tmp10B[0]=({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp10C;_tmp10C.tag=28;_tmp10C.f1=_tmp161;_tmp10C.f2=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp160);_tmp10C.f3=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpEB,_tmp15F);_tmp10C.f4=_tmp15E;_tmp10C;});_tmp10B;}),e->loc);
# 445
goto _LL73;case 29: _LLAE: _tmp163=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp162=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LLAF:
# 447
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp10D=_cycalloc(sizeof(*_tmp10D));_tmp10D[0]=({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmp10E;_tmp10E.tag=29;_tmp10E.f1=Cyc_Tcutil_copy_type(_tmp163);_tmp10E.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpEB,_tmp162);_tmp10E;});_tmp10D;}),e->loc);
goto _LL73;case 30: _LLB0: _tmp166=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp165=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp164=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_LLB1:
# 450
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F[0]=({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp110;_tmp110.tag=30;_tmp110.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpEB,_tmp166);_tmp110.f2=_tmp165;_tmp110.f3=_tmp164;_tmp110;});_tmp10F;}),e->loc);
goto _LL73;case 31: _LLB2: _tmp168=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp167=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LLB3:
 new_e=e;goto _LL73;case 32: _LLB4: _tmp16A=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp169=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LLB5:
# 454
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111[0]=({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp112;_tmp112.tag=32;_tmp112.f1=Cyc_Tcutil_copy_type(_tmp16A);_tmp112.f2=_tmp169;_tmp112;});_tmp111;}),e->loc);
goto _LL73;case 33: _LLB6: _tmp170=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpED)->f1).is_calloc;_tmp16F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpED)->f1).rgn;_tmp16E=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpED)->f1).elt_type;_tmp16D=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpED)->f1).num_elts;_tmp16C=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpED)->f1).fat_result;_tmp16B=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpED)->f1).inline_call;_LLB7: {
# 457
struct Cyc_Absyn_Exp*_tmp113=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp16F;if(_tmp16F != 0)r1=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp16F);{
void**t1=_tmp16E;if(_tmp16E != 0)t1=({void**_tmp114=_cycalloc(sizeof(*_tmp114));_tmp114[0]=Cyc_Tcutil_copy_type(*_tmp16E);_tmp114;});
_tmp113->r=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115[0]=({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmp116;_tmp116.tag=33;_tmp116.f1=({struct Cyc_Absyn_MallocInfo _tmp117;_tmp117.is_calloc=_tmp170;_tmp117.rgn=r1;_tmp117.elt_type=t1;_tmp117.num_elts=_tmp16D;_tmp117.fat_result=_tmp16C;_tmp117.inline_call=_tmp16B;_tmp117;});_tmp116;});_tmp115;});
new_e=_tmp113;
goto _LL73;};}case 34: _LLB8: _tmp172=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp171=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LLB9:
 new_e=Cyc_Absyn_swap_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp172),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp171),e->loc);goto _LL73;case 35: _LLBA: _tmp174=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp173=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LLBB: {
# 465
struct Cyc_Core_Opt*nopt1=_tmp174;
if(_tmp174 != 0)nopt1=({struct Cyc_Core_Opt*_tmp118=_cycalloc(sizeof(*_tmp118));_tmp118->v=(struct _tuple2*)_tmp174->v;_tmp118;});
new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119[0]=({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp11A;_tmp11A.tag=35;_tmp11A.f1=nopt1;_tmp11A.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpEB,_tmp173);_tmp11A;});_tmp119;}),e->loc);
goto _LL73;}case 36: _LLBC: _LLBD:
# 470
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp11B=_cycalloc(sizeof(*_tmp11B));_tmp11B[0]=({struct Cyc_Core_Failure_exn_struct _tmp11C;_tmp11C.tag=Cyc_Core_Failure;_tmp11C.f1=({const char*_tmp11D="deep_copy: statement expressions unsupported";_tag_dyneither(_tmp11D,sizeof(char),45);});_tmp11C;});_tmp11B;}));case 37: _LLBE: _tmp176=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp175=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LLBF:
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp11E=_cycalloc(sizeof(*_tmp11E));_tmp11E[0]=({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmp11F;_tmp11F.tag=37;_tmp11F.f1=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp176);_tmp11F.f2=_tmp175;_tmp11F;});_tmp11E;}),e->loc);
goto _LL73;case 38: _LLC0: _tmp177=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LLC1:
 new_e=Cyc_Absyn_valueof_exp(Cyc_Tcutil_copy_type(_tmp177),e->loc);
goto _LL73;default: _LLC2: _tmp179=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp178=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LLC3:
 new_e=Cyc_Absyn_asm_exp(_tmp179,_tmp178,e->loc);goto _LL73;}_LL73:;}
# 478
if(preserve_types)new_e->topt=e->topt;
return new_e;}
# 482
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*e){
if(e == 0)return 0;else{
return Cyc_Tcutil_deep_copy_exp(preserve_types,e);}}struct _tuple20{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple21{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 495 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
struct Cyc_Absyn_Kind*_tmp17A=ka1;enum Cyc_Absyn_KindQual _tmp185;enum Cyc_Absyn_AliasQual _tmp184;_LLCD: _tmp185=_tmp17A->kind;_tmp184=_tmp17A->aliasqual;_LLCE:;{
struct Cyc_Absyn_Kind*_tmp17B=ka2;enum Cyc_Absyn_KindQual _tmp183;enum Cyc_Absyn_AliasQual _tmp182;_LLD0: _tmp183=_tmp17B->kind;_tmp182=_tmp17B->aliasqual;_LLD1:;
# 499
if(_tmp185 != _tmp183){
struct _tuple20 _tmp17C=({struct _tuple20 _tmp17E;_tmp17E.f1=_tmp185;_tmp17E.f2=_tmp183;_tmp17E;});struct _tuple20 _tmp17D=_tmp17C;switch(_tmp17D.f1){case Cyc_Absyn_BoxKind: switch(_tmp17D.f2){case Cyc_Absyn_MemKind: _LLD3: _LLD4:
 goto _LLD6;case Cyc_Absyn_AnyKind: _LLD5: _LLD6:
 goto _LLD8;default: goto _LLD9;}case Cyc_Absyn_MemKind: if(_tmp17D.f2 == Cyc_Absyn_AnyKind){_LLD7: _LLD8:
 goto _LLD2;}else{goto _LLD9;}default: _LLD9: _LLDA:
 return 0;}_LLD2:;}
# 508
if(_tmp184 != _tmp182){
struct _tuple21 _tmp17F=({struct _tuple21 _tmp181;_tmp181.f1=_tmp184;_tmp181.f2=_tmp182;_tmp181;});struct _tuple21 _tmp180=_tmp17F;switch(_tmp180.f1){case Cyc_Absyn_Aliasable: if(_tmp180.f2 == Cyc_Absyn_Top){_LLDC: _LLDD:
 goto _LLDF;}else{goto _LLE0;}case Cyc_Absyn_Unique: if(_tmp180.f2 == Cyc_Absyn_Top){_LLDE: _LLDF:
 return 1;}else{goto _LLE0;}default: _LLE0: _LLE1:
 return 0;}_LLDB:;}
# 515
return 1;};}
# 518
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp186=Cyc_Absyn_compress_kb(tv->kind);void*_tmp187=_tmp186;struct Cyc_Absyn_Kind*_tmp18B;struct Cyc_Absyn_Kind*_tmp18A;switch(*((int*)_tmp187)){case 0: _LLE3: _tmp18A=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp187)->f1;_LLE4:
 return _tmp18A;case 2: _LLE5: _tmp18B=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp187)->f2;_LLE6:
 return _tmp18B;default: _LLE7: _LLE8:
# 523
 tv->kind=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp188=_cycalloc(sizeof(*_tmp188));_tmp188[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp189;_tmp189.tag=2;_tmp189.f1=0;_tmp189.f2=def;_tmp189;});_tmp188;});
return def;}_LLE2:;}
# 528
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
struct _tuple0 _tmp18C=({struct _tuple0 _tmp1A3;_tmp1A3.f1=Cyc_Absyn_compress_kb(kb1);_tmp1A3.f2=Cyc_Absyn_compress_kb(kb2);_tmp1A3;});struct _tuple0 _tmp18D=_tmp18C;struct Cyc_Core_Opt**_tmp1A2;void*_tmp1A1;void*_tmp1A0;struct Cyc_Core_Opt**_tmp19F;struct Cyc_Core_Opt**_tmp19E;struct Cyc_Absyn_Kind*_tmp19D;struct Cyc_Core_Opt**_tmp19C;struct Cyc_Absyn_Kind*_tmp19B;struct Cyc_Core_Opt**_tmp19A;struct Cyc_Absyn_Kind*_tmp199;struct Cyc_Absyn_Kind*_tmp198;struct Cyc_Absyn_Kind*_tmp197;struct Cyc_Core_Opt**_tmp196;struct Cyc_Absyn_Kind*_tmp195;struct Cyc_Absyn_Kind*_tmp194;struct Cyc_Absyn_Kind*_tmp193;switch(*((int*)_tmp18D.f1)){case 0: switch(*((int*)_tmp18D.f2)){case 0: _LLEA: _tmp194=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp18D.f1)->f1;_tmp193=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp18D.f2)->f1;_LLEB:
 return _tmp194 == _tmp193;case 2: _LLEE: _tmp197=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp18D.f1)->f1;_tmp196=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp18D.f2)->f1;_tmp195=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp18D.f2)->f2;_LLEF:
# 537
 if(Cyc_Tcutil_kind_leq(_tmp197,_tmp195)){
*_tmp196=({struct Cyc_Core_Opt*_tmp18F=_cycalloc(sizeof(*_tmp18F));_tmp18F->v=kb1;_tmp18F;});
return 1;}else{
return 0;}default: goto _LLF4;}case 2: switch(*((int*)_tmp18D.f2)){case 0: _LLEC: _tmp19A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp18D.f1)->f1;_tmp199=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp18D.f1)->f2;_tmp198=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp18D.f2)->f1;_LLED:
# 532
 if(Cyc_Tcutil_kind_leq(_tmp198,_tmp199)){
*_tmp19A=({struct Cyc_Core_Opt*_tmp18E=_cycalloc(sizeof(*_tmp18E));_tmp18E->v=kb2;_tmp18E;});
return 1;}else{
return 0;}case 2: _LLF0: _tmp19E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp18D.f1)->f1;_tmp19D=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp18D.f1)->f2;_tmp19C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp18D.f2)->f1;_tmp19B=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp18D.f2)->f2;_LLF1:
# 542
 if(Cyc_Tcutil_kind_leq(_tmp19D,_tmp19B)){
*_tmp19C=({struct Cyc_Core_Opt*_tmp190=_cycalloc(sizeof(*_tmp190));_tmp190->v=kb1;_tmp190;});
return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp19B,_tmp19D)){
*_tmp19E=({struct Cyc_Core_Opt*_tmp191=_cycalloc(sizeof(*_tmp191));_tmp191->v=kb2;_tmp191;});
return 1;}else{
return 0;}}default: _LLF4: _tmp1A0=_tmp18D.f1;_tmp19F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp18D.f2)->f1;_LLF5:
# 551
*_tmp19F=({struct Cyc_Core_Opt*_tmp192=_cycalloc(sizeof(*_tmp192));_tmp192->v=_tmp1A0;_tmp192;});
return 1;}default: _LLF2: _tmp1A2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp18D.f1)->f1;_tmp1A1=_tmp18D.f2;_LLF3:
# 549
 _tmp1A0=_tmp1A1;_tmp19F=_tmp1A2;goto _LLF5;}_LLE9:;}
# 556
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp1A4=Cyc_Tcutil_compress(t);void*_tmp1A5=_tmp1A4;struct Cyc_Absyn_Tvar*_tmp1AB;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A5)->tag == 2){_LLF7: _tmp1AB=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A5)->f1;_LLF8: {
# 559
void*_tmp1A6=_tmp1AB->kind;
_tmp1AB->kind=kb;
return({struct _tuple16 _tmp1A7;_tmp1A7.f1=_tmp1AB;_tmp1A7.f2=_tmp1A6;_tmp1A7;});}}else{_LLF9: _LLFA:
# 563
({struct Cyc_String_pa_PrintArg_struct _tmp1AA;_tmp1AA.tag=0;_tmp1AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp1A8[1]={& _tmp1AA};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1A9="swap_kind: cannot update the kind of %s";_tag_dyneither(_tmp1A9,sizeof(char),40);}),_tag_dyneither(_tmp1A8,sizeof(void*),1));});});}_LLF6:;}
# 569
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_typ,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 571
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_typ_kind(field_typ);
if(ts != 0  && (k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak)){
# 575
struct _RegionHandle _tmp1AC=_new_region("temp");struct _RegionHandle*temp=& _tmp1AC;_push_region(temp);
{struct Cyc_List_List*_tmp1AD=0;
# 578
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp1AE=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp1AF=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;
struct Cyc_Absyn_Kind*_tmp1B0=Cyc_Tcutil_tvar_kind(_tmp1AE,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp1B1=_tmp1B0;switch(((struct Cyc_Absyn_Kind*)_tmp1B1)->kind){case Cyc_Absyn_IntKind: _LLFC: _LLFD:
 goto _LLFF;case Cyc_Absyn_AnyKind: _LLFE: _LLFF:
# 584
 _tmp1AD=({struct Cyc_List_List*_tmp1B2=_region_malloc(temp,sizeof(*_tmp1B2));_tmp1B2->hd=({struct _tuple16*_tmp1B3=_region_malloc(temp,sizeof(*_tmp1B3));_tmp1B3->f1=_tmp1AE;_tmp1B3->f2=_tmp1AF;_tmp1B3;});_tmp1B2->tl=_tmp1AD;_tmp1B2;});goto _LLFB;default: _LL100: _LL101:
 goto _LLFB;}_LLFB:;}
# 588
if(_tmp1AD != 0){
field_typ=Cyc_Tcutil_rsubstitute(temp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1AD),field_typ);
k=Cyc_Tcutil_typ_kind(field_typ);}}
# 576
;_pop_region(temp);}
# 593
return k;}
# 600
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t){
void*_tmp1B4=Cyc_Tcutil_compress(t);void*_tmp1B5=_tmp1B4;struct Cyc_Absyn_Typedefdecl*_tmp1D3;struct Cyc_Absyn_Exp*_tmp1D2;struct Cyc_Absyn_Kind*_tmp1D1;struct Cyc_Absyn_PtrInfo _tmp1D0;enum Cyc_Absyn_AggrKind _tmp1CF;struct Cyc_List_List*_tmp1CE;struct Cyc_Absyn_AggrdeclImpl*_tmp1CD;int _tmp1CC;struct Cyc_List_List*_tmp1CB;struct Cyc_Absyn_Datatypedecl*_tmp1CA;struct Cyc_Absyn_Datatypefield*_tmp1C9;enum Cyc_Absyn_Size_of _tmp1C8;struct Cyc_Absyn_Tvar*_tmp1C7;struct Cyc_Core_Opt*_tmp1C6;switch(*((int*)_tmp1B5)){case 1: _LL103: _tmp1C6=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B5)->f1;_LL104:
 return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp1C6))->v;case 2: _LL105: _tmp1C7=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1B5)->f1;_LL106:
 return Cyc_Tcutil_tvar_kind(_tmp1C7,& Cyc_Tcutil_bk);case 0: _LL107: _LL108:
 return& Cyc_Tcutil_mk;case 6: _LL109: _tmp1C8=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1B5)->f2;_LL10A:
# 606
 return(_tmp1C8 == Cyc_Absyn_Int_sz  || _tmp1C8 == Cyc_Absyn_Long_sz)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;case 7: _LL10B: _LL10C:
# 608
 return& Cyc_Tcutil_mk;case 9: _LL10D: _LL10E:
 return& Cyc_Tcutil_ak;case 16: _LL10F: _LL110:
 goto _LL112;case 13: _LL111: _LL112:
 goto _LL114;case 14: _LL113: _LL114:
 goto _LL116;case 15: _LL115: _LL116:
 return& Cyc_Tcutil_bk;case 21: _LL117: _LL118:
 return& Cyc_Tcutil_urk;case 20: _LL119: _LL11A:
 return& Cyc_Tcutil_rk;case 22: _LL11B: _LL11C:
 return& Cyc_Tcutil_trk;case 3: _LL11D: _LL11E:
# 619
 return& Cyc_Tcutil_ak;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1B5)->f1).field_info).KnownDatatypefield).tag == 2){_LL11F: _tmp1CA=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1B5)->f1).field_info).KnownDatatypefield).val).f1;_tmp1C9=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1B5)->f1).field_info).KnownDatatypefield).val).f2;_LL120:
# 621
 return& Cyc_Tcutil_mk;}else{_LL121: _LL122:
# 623
({void*_tmp1B6=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1B7="typ_kind: Unresolved DatatypeFieldType";_tag_dyneither(_tmp1B7,sizeof(char),39);}),_tag_dyneither(_tmp1B6,sizeof(void*),0));});}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B5)->f1).aggr_info).UnknownAggr).tag == 1){_LL123: _LL124:
# 626
 return& Cyc_Tcutil_ak;}else{_LL125: _tmp1CF=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B5)->f1).aggr_info).KnownAggr).val)->kind;_tmp1CE=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B5)->f1).aggr_info).KnownAggr).val)->tvs;_tmp1CD=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B5)->f1).aggr_info).KnownAggr).val)->impl;_tmp1CC=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B5)->f1).aggr_info).KnownAggr).val)->expected_mem_kind;_tmp1CB=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B5)->f1).targs;_LL126:
# 628
 if(_tmp1CD == 0){
if(_tmp1CC)
return& Cyc_Tcutil_mk;else{
# 632
return& Cyc_Tcutil_ak;}}{
# 634
struct Cyc_List_List*_tmp1B8=_tmp1CD->fields;
if(_tmp1B8 == 0)return& Cyc_Tcutil_mk;
# 637
if(_tmp1CF == Cyc_Absyn_StructA){
for(0;_tmp1B8->tl != 0;_tmp1B8=_tmp1B8->tl){;}{
void*_tmp1B9=((struct Cyc_Absyn_Aggrfield*)_tmp1B8->hd)->type;
struct Cyc_Absyn_Kind*_tmp1BA=Cyc_Tcutil_field_kind(_tmp1B9,_tmp1CB,_tmp1CE);
if(_tmp1BA == & Cyc_Tcutil_ak  || _tmp1BA == & Cyc_Tcutil_tak)return _tmp1BA;};}else{
# 645
for(0;_tmp1B8 != 0;_tmp1B8=_tmp1B8->tl){
void*_tmp1BB=((struct Cyc_Absyn_Aggrfield*)_tmp1B8->hd)->type;
struct Cyc_Absyn_Kind*_tmp1BC=Cyc_Tcutil_field_kind(_tmp1BB,_tmp1CB,_tmp1CE);
if(_tmp1BC == & Cyc_Tcutil_ak  || _tmp1BC == & Cyc_Tcutil_tak)return _tmp1BC;}}
# 651
return& Cyc_Tcutil_mk;};}case 12: _LL127: _LL128:
 return& Cyc_Tcutil_mk;case 5: _LL129: _tmp1D0=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B5)->f1;_LL12A: {
# 654
void*_tmp1BD=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,(_tmp1D0.ptr_atts).bounds);void*_tmp1BE=_tmp1BD;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1BE)->tag == 0){_LL146: _LL147: {
# 656
enum Cyc_Absyn_AliasQual _tmp1BF=(Cyc_Tcutil_typ_kind((_tmp1D0.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp1C0=_tmp1BF;switch(_tmp1C0){case Cyc_Absyn_Aliasable: _LL14B: _LL14C:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL14D: _LL14E:
 return& Cyc_Tcutil_umk;default: _LL14F: _LL150:
 return& Cyc_Tcutil_tmk;}_LL14A:;}}else{_LL148: _LL149: {
# 662
enum Cyc_Absyn_AliasQual _tmp1C1=(Cyc_Tcutil_typ_kind((_tmp1D0.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp1C2=_tmp1C1;switch(_tmp1C2){case Cyc_Absyn_Aliasable: _LL152: _LL153:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL154: _LL155:
 return& Cyc_Tcutil_ubk;default: _LL156: _LL157:
 return& Cyc_Tcutil_tbk;}_LL151:;}}_LL145:;}case 18: _LL12B: _LL12C:
# 668
 return& Cyc_Tcutil_ik;case 28: _LL12D: _tmp1D1=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp1B5)->f2;_LL12E:
 return _tmp1D1;case 27: _LL12F: _LL130:
# 673
 return& Cyc_Tcutil_ak;case 19: _LL131: _LL132:
 return& Cyc_Tcutil_bk;case 8: _LL133: _tmp1D2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1B5)->f1).num_elts;_LL134:
# 676
 if(_tmp1D2 == 0  || Cyc_Tcutil_is_const_exp(_tmp1D2))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;case 10: _LL135: _LL136:
 return& Cyc_Tcutil_mk;case 17: _LL137: _tmp1D3=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1B5)->f3;_LL138:
# 680
 if(_tmp1D3 == 0  || _tmp1D3->kind == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1C5;_tmp1C5.tag=0;_tmp1C5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp1C3[1]={& _tmp1C5};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1C4="typ_kind: typedef found: %s";_tag_dyneither(_tmp1C4,sizeof(char),28);}),_tag_dyneither(_tmp1C3,sizeof(void*),1));});});
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp1D3->kind))->v;case 26: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1B5)->f1)->r)){case 0: _LL139: _LL13A:
 return& Cyc_Tcutil_ak;case 1: _LL13B: _LL13C:
 return& Cyc_Tcutil_bk;default: _LL13D: _LL13E:
 return& Cyc_Tcutil_ak;}case 23: _LL13F: _LL140:
 goto _LL142;case 24: _LL141: _LL142:
 goto _LL144;default: _LL143: _LL144:
 return& Cyc_Tcutil_ek;}_LL102:;}
# 692
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2  || k1->kind == k2->kind  && k1->aliasqual == k2->aliasqual;}
# 697
int Cyc_Tcutil_unify(void*t1,void*t2){
struct _handler_cons _tmp1D4;_push_handler(& _tmp1D4);{int _tmp1D6=0;if(setjmp(_tmp1D4.handler))_tmp1D6=1;if(!_tmp1D6){
Cyc_Tcutil_unify_it(t1,t2);{
int _tmp1D7=1;_npop_handler(0);return _tmp1D7;};
# 699
;_pop_handler();}else{void*_tmp1D5=(void*)_exn_thrown;void*_tmp1D8=_tmp1D5;void*_tmp1D9;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp1D8)->tag == Cyc_Tcutil_Unify){_LL159: _LL15A:
# 701
 return 0;}else{_LL15B: _tmp1D9=_tmp1D8;_LL15C:(int)_rethrow(_tmp1D9);}_LL158:;}};}
# 706
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
t=Cyc_Tcutil_compress(t);{
void*_tmp1DA=t;struct Cyc_List_List*_tmp200;void*_tmp1FF;void*_tmp1FE;void*_tmp1FD;void*_tmp1FC;struct Cyc_List_List*_tmp1FB;struct Cyc_List_List*_tmp1FA;struct Cyc_List_List*_tmp1F9;struct Cyc_List_List*_tmp1F8;struct Cyc_List_List*_tmp1F7;struct Cyc_List_List*_tmp1F6;struct Cyc_List_List*_tmp1F5;void*_tmp1F4;struct Cyc_Absyn_Tqual _tmp1F3;void*_tmp1F2;struct Cyc_List_List*_tmp1F1;int _tmp1F0;struct Cyc_Absyn_VarargInfo*_tmp1EF;struct Cyc_List_List*_tmp1EE;struct Cyc_List_List*_tmp1ED;struct Cyc_Absyn_Exp*_tmp1EC;struct Cyc_List_List*_tmp1EB;struct Cyc_Absyn_Exp*_tmp1EA;struct Cyc_List_List*_tmp1E9;void*_tmp1E8;struct Cyc_Absyn_PtrInfo _tmp1E7;void*_tmp1E6;struct Cyc_Core_Opt**_tmp1E5;struct Cyc_Absyn_Tvar*_tmp1E4;switch(*((int*)_tmp1DA)){case 2: _LL15E: _tmp1E4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1DA)->f1;_LL15F:
# 711
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp1E4)){
Cyc_Tcutil_failure_reason=({const char*_tmp1DB="(type variable would escape scope)";_tag_dyneither(_tmp1DB,sizeof(char),35);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 715
goto _LL15D;case 1: _LL160: _tmp1E6=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1DA)->f2;_tmp1E5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1DA)->f4;_LL161:
# 717
 if(t == evar){
Cyc_Tcutil_failure_reason=({const char*_tmp1DC="(occurs check)";_tag_dyneither(_tmp1DC,sizeof(char),15);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{
# 721
if(_tmp1E6 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp1E6);else{
# 724
int problem=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp1E5))->v;for(0;s != 0;s=s->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}
# 731
if(problem){
struct Cyc_List_List*_tmp1DD=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp1E5))->v;for(0;s != 0;s=s->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd))
_tmp1DD=({struct Cyc_List_List*_tmp1DE=_cycalloc(sizeof(*_tmp1DE));_tmp1DE->hd=(struct Cyc_Absyn_Tvar*)s->hd;_tmp1DE->tl=_tmp1DD;_tmp1DE;});}}
# 737
*_tmp1E5=({struct Cyc_Core_Opt*_tmp1DF=_cycalloc(sizeof(*_tmp1DF));_tmp1DF->v=_tmp1DD;_tmp1DF;});}}}
# 740
goto _LL15D;case 5: _LL162: _tmp1E7=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1DA)->f1;_LL163:
# 743
 Cyc_Tcutil_occurs(evar,r,env,_tmp1E7.elt_typ);
Cyc_Tcutil_occurs(evar,r,env,(_tmp1E7.ptr_atts).rgn);
goto _LL15D;case 8: _LL164: _tmp1E8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1DA)->f1).elt_type;_LL165:
# 747
 Cyc_Tcutil_occurs(evar,r,env,_tmp1E8);goto _LL15D;case 9: _LL166: _tmp1F5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DA)->f1).tvars;_tmp1F4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DA)->f1).effect;_tmp1F3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DA)->f1).ret_tqual;_tmp1F2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DA)->f1).ret_typ;_tmp1F1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DA)->f1).args;_tmp1F0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DA)->f1).c_varargs;_tmp1EF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DA)->f1).cyc_varargs;_tmp1EE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DA)->f1).rgn_po;_tmp1ED=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DA)->f1).attributes;_tmp1EC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DA)->f1).requires_clause;_tmp1EB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DA)->f1).requires_relns;_tmp1EA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DA)->f1).ensures_clause;_tmp1E9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DA)->f1).ensures_relns;_LL167:
# 750
 env=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp1F5,env);
if(_tmp1F4 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp1F4);
Cyc_Tcutil_occurs(evar,r,env,_tmp1F2);
for(0;_tmp1F1 != 0;_tmp1F1=_tmp1F1->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple10*)_tmp1F1->hd)).f3);}
if(_tmp1EF != 0)
Cyc_Tcutil_occurs(evar,r,env,_tmp1EF->type);
for(0;_tmp1EE != 0;_tmp1EE=_tmp1EE->tl){
struct _tuple0*_tmp1E0=(struct _tuple0*)_tmp1EE->hd;struct _tuple0*_tmp1E1=_tmp1E0;void*_tmp1E3;void*_tmp1E2;_LL181: _tmp1E3=_tmp1E1->f1;_tmp1E2=_tmp1E1->f2;_LL182:;
Cyc_Tcutil_occurs(evar,r,env,_tmp1E3);
Cyc_Tcutil_occurs(evar,r,env,_tmp1E2);}
# 762
goto _LL15D;case 10: _LL168: _tmp1F6=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1DA)->f1;_LL169:
# 764
 for(0;_tmp1F6 != 0;_tmp1F6=_tmp1F6->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple12*)_tmp1F6->hd)).f2);}
goto _LL15D;case 3: _LL16A: _tmp1F7=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1DA)->f1).targs;_LL16B:
# 768
 Cyc_Tcutil_occurslist(evar,r,env,_tmp1F7);goto _LL15D;case 17: _LL16C: _tmp1F8=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1DA)->f2;_LL16D:
 _tmp1F9=_tmp1F8;goto _LL16F;case 4: _LL16E: _tmp1F9=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1DA)->f1).targs;_LL16F:
 _tmp1FA=_tmp1F9;goto _LL171;case 11: _LL170: _tmp1FA=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1DA)->f1).targs;_LL171:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp1FA);goto _LL15D;case 12: _LL172: _tmp1FB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1DA)->f2;_LL173:
# 774
 for(0;_tmp1FB != 0;_tmp1FB=_tmp1FB->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp1FB->hd)->type);}
goto _LL15D;case 19: _LL174: _tmp1FC=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp1DA)->f1;_LL175:
 _tmp1FD=_tmp1FC;goto _LL177;case 23: _LL176: _tmp1FD=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp1DA)->f1;_LL177:
 _tmp1FE=_tmp1FD;goto _LL179;case 15: _LL178: _tmp1FE=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1DA)->f1;_LL179:
 _tmp1FF=_tmp1FE;goto _LL17B;case 25: _LL17A: _tmp1FF=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp1DA)->f1;_LL17B:
 Cyc_Tcutil_occurs(evar,r,env,_tmp1FF);goto _LL15D;case 24: _LL17C: _tmp200=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp1DA)->f1;_LL17D:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp200);goto _LL15D;default: _LL17E: _LL17F:
# 784
 goto _LL15D;}_LL15D:;};}
# 787
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
# 789
for(0;ts != 0;ts=ts->tl){
Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}
# 794
static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){
Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0  || t2 != 0)
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 802
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){
if(tq1.print_const  && !tq1.real_const)
({void*_tmp201=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp202="tq1 real_const not set.";_tag_dyneither(_tmp202,sizeof(char),24);}),_tag_dyneither(_tmp201,sizeof(void*),0));});
if(tq2.print_const  && !tq2.real_const)
({void*_tmp203=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp204="tq2 real_const not set.";_tag_dyneither(_tmp204,sizeof(char),24);}),_tag_dyneither(_tmp203,sizeof(void*),0));});
# 808
if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
# 811
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;
Cyc_Tcutil_failure_reason=({const char*_tmp205="(qualifiers don't match)";_tag_dyneither(_tmp205,sizeof(char),25);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 819
Cyc_Tcutil_tq1_const=0;
Cyc_Tcutil_tq2_const=0;}
# 823
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
return(tq1.real_const == tq2.real_const  && tq1.q_volatile == tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}
# 830
static void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason){
# 832
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return;{
union Cyc_Absyn_Constraint*_tmp206=x;void*_tmp20E;switch((((union Cyc_Absyn_Constraint*)_tmp206)->Eq_constr).tag){case 3: _LL184: _LL185:
# 837
*x=({union Cyc_Absyn_Constraint _tmp207;(_tmp207.Forward_constr).val=y;(_tmp207.Forward_constr).tag=2;_tmp207;});return;case 1: _LL186: _tmp20E=(void*)(_tmp206->Eq_constr).val;_LL187: {
# 839
union Cyc_Absyn_Constraint*_tmp208=y;void*_tmp20B;switch((((union Cyc_Absyn_Constraint*)_tmp208)->Eq_constr).tag){case 3: _LL18B: _LL18C:
*y=*x;return;case 1: _LL18D: _tmp20B=(void*)(_tmp208->Eq_constr).val;_LL18E:
# 842
 if(cmp(_tmp20E,_tmp20B)!= 0){
Cyc_Tcutil_failure_reason=reason;
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 846
return;default: _LL18F: _LL190:
({void*_tmp209=0;Cyc_Tcutil_impos(({const char*_tmp20A="unify_conref: forward after compress(2)";_tag_dyneither(_tmp20A,sizeof(char),40);}),_tag_dyneither(_tmp209,sizeof(void*),0));});}_LL18A:;}default: _LL188: _LL189:
# 849
({void*_tmp20C=0;Cyc_Tcutil_impos(({const char*_tmp20D="unify_conref: forward after compress";_tag_dyneither(_tmp20D,sizeof(char),37);}),_tag_dyneither(_tmp20C,sizeof(void*),0));});}_LL183:;};}
# 853
static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
struct _handler_cons _tmp20F;_push_handler(& _tmp20F);{int _tmp211=0;if(setjmp(_tmp20F.handler))_tmp211=1;if(!_tmp211){
Cyc_Tcutil_unify_it_conrefs(cmp,x,y,_tag_dyneither(0,0,0));{
int _tmp212=1;_npop_handler(0);return _tmp212;};
# 855
;_pop_handler();}else{void*_tmp210=(void*)_exn_thrown;void*_tmp213=_tmp210;void*_tmp214;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp213)->tag == Cyc_Tcutil_Unify){_LL192: _LL193:
# 857
 return 0;}else{_LL194: _tmp214=_tmp213;_LL195:(int)_rethrow(_tmp214);}_LL191:;}};}
# 860
static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmp215=({struct _tuple0 _tmp219;_tmp219.f1=b1;_tmp219.f2=b2;_tmp219;});struct _tuple0 _tmp216=_tmp215;struct Cyc_Absyn_Exp*_tmp218;struct Cyc_Absyn_Exp*_tmp217;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp216.f1)->tag == 0){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp216.f2)->tag == 0){_LL197: _LL198:
 return 0;}else{_LL199: _LL19A:
 return - 1;}}else{if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp216.f2)->tag == 0){_LL19B: _LL19C:
 return 1;}else{_LL19D: _tmp218=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp216.f1)->f1;_tmp217=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp216.f2)->f1;_LL19E:
# 866
 return Cyc_Evexp_const_exp_cmp(_tmp218,_tmp217);}}_LL196:;}
# 870
static void Cyc_Tcutil_unify_cmp_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
{struct _tuple0 _tmp21A=({struct _tuple0 _tmp228;_tmp228.f1=e1->r;_tmp228.f2=e2->r;_tmp228;});struct _tuple0 _tmp21B=_tmp21A;void*_tmp227;void*_tmp226;struct Cyc_Absyn_Exp*_tmp225;struct Cyc_Absyn_Exp*_tmp224;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp21B.f1)->tag == 13){_LL1A0: _tmp224=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp21B.f1)->f2;_LL1A1:
# 876
 Cyc_Tcutil_unify_cmp_exp(_tmp224,e2);
return;}else{if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp21B.f2)->tag == 13){_LL1A2: _tmp225=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp21B.f2)->f2;_LL1A3:
# 879
 Cyc_Tcutil_unify_cmp_exp(e1,_tmp225);
return;}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp21B.f1)->tag == 38){_LL1A4: _tmp226=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp21B.f1)->f1;_LL1A5:
# 882
{void*_tmp21C=Cyc_Tcutil_compress(_tmp226);void*_tmp21D=_tmp21C;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp21D)->tag == 1){_LL1AB: _LL1AC:
# 884
 Cyc_Tcutil_unify_it(_tmp226,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp21E=_cycalloc(sizeof(*_tmp21E));_tmp21E[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp21F;_tmp21F.tag=18;_tmp21F.f1=e2;_tmp21F;});_tmp21E;}));
return;}else{_LL1AD: _LL1AE:
# 887
 goto _LL1AA;}_LL1AA:;}
# 889
goto _LL19F;}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp21B.f2)->tag == 38){_LL1A6: _tmp227=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp21B.f2)->f1;_LL1A7:
# 891
{void*_tmp220=Cyc_Tcutil_compress(_tmp227);void*_tmp221=_tmp220;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp221)->tag == 1){_LL1B0: _LL1B1:
# 893
 Cyc_Tcutil_unify_it(_tmp227,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp222=_cycalloc(sizeof(*_tmp222));_tmp222[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp223;_tmp223.tag=18;_tmp223.f1=e1;_tmp223;});_tmp222;}));
return;}else{_LL1B2: _LL1B3:
# 896
 goto _LL1AF;}_LL1AF:;}
# 898
goto _LL19F;}else{_LL1A8: _LL1A9:
 goto _LL19F;}}}}_LL19F:;}
# 901
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 904
static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){
struct _tuple0 _tmp229=({struct _tuple0 _tmp233;_tmp233.f1=b1;_tmp233.f2=b2;_tmp233;});struct _tuple0 _tmp22A=_tmp229;struct Cyc_Absyn_Exp*_tmp232;struct Cyc_Absyn_Exp*_tmp231;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp22A.f1)->tag == 0){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp22A.f2)->tag == 0){_LL1B5: _LL1B6:
 return 0;}else{_LL1B7: _LL1B8:
 return - 1;}}else{if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp22A.f2)->tag == 0){_LL1B9: _LL1BA:
 return 1;}else{_LL1BB: _tmp232=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp22A.f1)->f1;_tmp231=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp22A.f2)->f1;_LL1BC: {
# 910
int r=Cyc_Evexp_const_exp_cmp(_tmp232,_tmp231);
if(r == 0)return 0;{
struct _handler_cons _tmp22B;_push_handler(& _tmp22B);{int _tmp22D=0;if(setjmp(_tmp22B.handler))_tmp22D=1;if(!_tmp22D){
Cyc_Tcutil_unify_cmp_exp(_tmp232,_tmp231);{
int _tmp22E=0;_npop_handler(0);return _tmp22E;};
# 913
;_pop_handler();}else{void*_tmp22C=(void*)_exn_thrown;void*_tmp22F=_tmp22C;void*_tmp230;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp22F)->tag == Cyc_Tcutil_Unify){_LL1BE: _LL1BF:
# 916
 return r;}else{_LL1C0: _tmp230=_tmp22F;_LL1C1:(int)_rethrow(_tmp230);}_LL1BD:;}};};}}}_LL1B4:;}
# 921
static int Cyc_Tcutil_attribute_case_number(void*att){
void*_tmp234=att;switch(*((int*)_tmp234)){case 0: _LL1C3: _LL1C4:
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
# 948
static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple0 _tmp235=({struct _tuple0 _tmp247;_tmp247.f1=att1;_tmp247.f2=att2;_tmp247;});struct _tuple0 _tmp236=_tmp235;enum Cyc_Absyn_Format_Type _tmp246;int _tmp245;int _tmp244;enum Cyc_Absyn_Format_Type _tmp243;int _tmp242;int _tmp241;struct _dyneither_ptr _tmp240;struct _dyneither_ptr _tmp23F;struct Cyc_Absyn_Exp*_tmp23E;struct Cyc_Absyn_Exp*_tmp23D;int _tmp23C;int _tmp23B;int _tmp23A;int _tmp239;switch(*((int*)_tmp236.f1)){case 0: if(((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp236.f2)->tag == 0){_LL1F0: _tmp23A=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp236.f1)->f1;_tmp239=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp236.f2)->f1;_LL1F1:
 _tmp23C=_tmp23A;_tmp23B=_tmp239;goto _LL1F3;}else{goto _LL1FA;}case 20: if(((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp236.f2)->tag == 20){_LL1F2: _tmp23C=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp236.f1)->f1;_tmp23B=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp236.f2)->f1;_LL1F3:
# 952
 return Cyc_Core_intcmp(_tmp23C,_tmp23B);}else{goto _LL1FA;}case 6: if(((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp236.f2)->tag == 6){_LL1F4: _tmp23E=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp236.f1)->f1;_tmp23D=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp236.f2)->f1;_LL1F5:
# 954
 if(_tmp23E == _tmp23D)return 0;
if(_tmp23E == 0)return - 1;
if(_tmp23D == 0)return 1;
return Cyc_Evexp_const_exp_cmp(_tmp23E,_tmp23D);}else{goto _LL1FA;}case 8: if(((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp236.f2)->tag == 8){_LL1F6: _tmp240=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp236.f1)->f1;_tmp23F=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp236.f2)->f1;_LL1F7:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp240,(struct _dyneither_ptr)_tmp23F);}else{goto _LL1FA;}case 19: if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp236.f2)->tag == 19){_LL1F8: _tmp246=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp236.f1)->f1;_tmp245=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp236.f1)->f2;_tmp244=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp236.f1)->f3;_tmp243=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp236.f2)->f1;_tmp242=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp236.f2)->f2;_tmp241=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp236.f2)->f3;_LL1F9: {
# 960
int _tmp237=Cyc_Core_intcmp((int)((unsigned int)_tmp246),(int)((unsigned int)_tmp243));
if(_tmp237 != 0)return _tmp237;{
int _tmp238=Cyc_Core_intcmp(_tmp245,_tmp242);
if(_tmp238 != 0)return _tmp238;
return Cyc_Core_intcmp(_tmp244,_tmp241);};}}else{goto _LL1FA;}default: _LL1FA: _LL1FB:
# 966
 return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(att2));}_LL1EF:;}
# 970
static int Cyc_Tcutil_equal_att(void*a1,void*a2){
return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}
# 974
int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}
return 1;}
# 983
static void*Cyc_Tcutil_rgns_of(void*t);
# 985
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 989
static struct _tuple16*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_tmp248=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp249=_tmp248;switch(((struct Cyc_Absyn_Kind*)_tmp249)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp249)->aliasqual){case Cyc_Absyn_Unique: _LL1FD: _LL1FE:
 t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL1FC;case Cyc_Absyn_Aliasable: _LL1FF: _LL200:
 t=(void*)& Cyc_Absyn_HeapRgn_val;goto _LL1FC;default: goto _LL205;}case Cyc_Absyn_EffKind: _LL201: _LL202:
 t=Cyc_Absyn_empty_effect;goto _LL1FC;case Cyc_Absyn_IntKind: _LL203: _LL204:
 t=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp24A=_cycalloc(sizeof(*_tmp24A));_tmp24A[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp24B;_tmp24B.tag=18;_tmp24B.f1=Cyc_Absyn_uint_exp(0,0);_tmp24B;});_tmp24A;});goto _LL1FC;default: _LL205: _LL206:
 t=Cyc_Absyn_sint_typ;goto _LL1FC;}_LL1FC:;}
# 998
return({struct _tuple16*_tmp24C=_cycalloc(sizeof(*_tmp24C));_tmp24C->f1=tv;_tmp24C->f2=t;_tmp24C;});}
# 1005
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp24D=Cyc_Tcutil_compress(t);void*_tmp24E=_tmp24D;struct Cyc_List_List*_tmp27E;void*_tmp27D;struct Cyc_List_List*_tmp27C;void*_tmp27B;struct Cyc_Absyn_Tqual _tmp27A;void*_tmp279;struct Cyc_List_List*_tmp278;struct Cyc_Absyn_VarargInfo*_tmp277;struct Cyc_List_List*_tmp276;struct Cyc_List_List*_tmp275;struct Cyc_List_List*_tmp274;struct Cyc_List_List*_tmp273;struct Cyc_List_List*_tmp272;void*_tmp271;void*_tmp270;void*_tmp26F;struct Cyc_List_List*_tmp26E;void*_tmp26D;void*_tmp26C;void*_tmp26B;switch(*((int*)_tmp24E)){case 0: _LL208: _LL209:
 goto _LL20B;case 7: _LL20A: _LL20B:
 goto _LL20D;case 13: _LL20C: _LL20D:
 goto _LL20F;case 14: _LL20E: _LL20F:
 goto _LL211;case 18: _LL210: _LL211:
 goto _LL213;case 27: _LL212: _LL213:
 goto _LL215;case 28: _LL214: _LL215:
 goto _LL217;case 6: _LL216: _LL217:
 return Cyc_Absyn_empty_effect;case 1: _LL218: _LL219:
 goto _LL21B;case 2: _LL21A: _LL21B: {
# 1017
struct Cyc_Absyn_Kind*_tmp24F=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp250=_tmp24F;switch(((struct Cyc_Absyn_Kind*)_tmp250)->kind){case Cyc_Absyn_RgnKind: _LL243: _LL244:
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp251=_cycalloc(sizeof(*_tmp251));_tmp251[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp252;_tmp252.tag=23;_tmp252.f1=t;_tmp252;});_tmp251;});case Cyc_Absyn_EffKind: _LL245: _LL246:
 return t;case Cyc_Absyn_IntKind: _LL247: _LL248:
 return Cyc_Absyn_empty_effect;default: _LL249: _LL24A:
 return(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp253=_cycalloc(sizeof(*_tmp253));_tmp253[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp254;_tmp254.tag=25;_tmp254.f1=t;_tmp254;});_tmp253;});}_LL242:;}case 15: _LL21C: _tmp26B=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp24E)->f1;_LL21D:
# 1023
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp255=_cycalloc(sizeof(*_tmp255));_tmp255[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp256;_tmp256.tag=23;_tmp256.f1=_tmp26B;_tmp256;});_tmp255;});case 16: _LL21E: _tmp26D=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp24E)->f1;_tmp26C=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp24E)->f2;_LL21F:
# 1027
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp257=_cycalloc(sizeof(*_tmp257));_tmp257[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp258;_tmp258.tag=23;_tmp258.f1=_tmp26C;_tmp258;});_tmp257;});case 3: _LL220: _tmp26E=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp24E)->f1).targs;_LL221: {
# 1029
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp26E);
return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp259=_cycalloc(sizeof(*_tmp259));_tmp259[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp25A;_tmp25A.tag=24;_tmp25A.f1=ts;_tmp25A;});_tmp259;}));}case 5: _LL222: _tmp270=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24E)->f1).elt_typ;_tmp26F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24E)->f1).ptr_atts).rgn;_LL223:
# 1033
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp25B=_cycalloc(sizeof(*_tmp25B));_tmp25B[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp25C;_tmp25C.tag=24;_tmp25C.f1=({void*_tmp25D[2];_tmp25D[1]=Cyc_Tcutil_rgns_of(_tmp270);_tmp25D[0]=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp25E=_cycalloc(sizeof(*_tmp25E));_tmp25E[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp25F;_tmp25F.tag=23;_tmp25F.f1=_tmp26F;_tmp25F;});_tmp25E;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp25D,sizeof(void*),2));});_tmp25C;});_tmp25B;}));case 8: _LL224: _tmp271=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24E)->f1).elt_type;_LL225:
# 1035
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp271));case 10: _LL226: _tmp272=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp24E)->f1;_LL227: {
# 1037
struct Cyc_List_List*_tmp260=0;
for(0;_tmp272 != 0;_tmp272=_tmp272->tl){
_tmp260=({struct Cyc_List_List*_tmp261=_cycalloc(sizeof(*_tmp261));_tmp261->hd=(*((struct _tuple12*)_tmp272->hd)).f2;_tmp261->tl=_tmp260;_tmp261;});}
_tmp273=_tmp260;goto _LL229;}case 4: _LL228: _tmp273=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp24E)->f1).targs;_LL229:
 _tmp274=_tmp273;goto _LL22B;case 11: _LL22A: _tmp274=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp24E)->f1).targs;_LL22B:
# 1043
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp262=_cycalloc(sizeof(*_tmp262));_tmp262[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp263;_tmp263.tag=24;_tmp263.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp274);_tmp263;});_tmp262;}));case 12: _LL22C: _tmp275=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp24E)->f2;_LL22D:
# 1045
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp264=_cycalloc(sizeof(*_tmp264));_tmp264[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp265;_tmp265.tag=24;_tmp265.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp275);_tmp265;});_tmp264;}));case 19: _LL22E: _LL22F:
 return Cyc_Absyn_empty_effect;case 9: _LL230: _tmp27C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp24E)->f1).tvars;_tmp27B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp24E)->f1).effect;_tmp27A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp24E)->f1).ret_tqual;_tmp279=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp24E)->f1).ret_typ;_tmp278=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp24E)->f1).args;_tmp277=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp24E)->f1).cyc_varargs;_tmp276=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp24E)->f1).rgn_po;_LL231: {
# 1055
void*_tmp266=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp27C),(void*)_check_null(_tmp27B));
return Cyc_Tcutil_normalize_effect(_tmp266);}case 21: _LL232: _LL233:
 goto _LL235;case 22: _LL234: _LL235:
 goto _LL237;case 20: _LL236: _LL237:
 return Cyc_Absyn_empty_effect;case 23: _LL238: _LL239:
 goto _LL23B;case 24: _LL23A: _LL23B:
 return t;case 25: _LL23C: _tmp27D=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp24E)->f1;_LL23D:
 return Cyc_Tcutil_rgns_of(_tmp27D);case 17: _LL23E: _tmp27E=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp24E)->f2;_LL23F:
# 1064
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp267=_cycalloc(sizeof(*_tmp267));_tmp267[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp268;_tmp268.tag=24;_tmp268.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp27E);_tmp268;});_tmp267;}));default: _LL240: _LL241:
({void*_tmp269=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp26A="typedecl in rgns_of";_tag_dyneither(_tmp26A,sizeof(char),20);}),_tag_dyneither(_tmp269,sizeof(void*),0));});}_LL207:;}
# 1072
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp27F=e;void*_tmp28B;struct Cyc_List_List**_tmp28A;switch(*((int*)_tmp27F)){case 24: _LL24C: _tmp28A=(struct Cyc_List_List**)&((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp27F)->f1;_LL24D: {
# 1076
int redo_join=0;
{struct Cyc_List_List*effs=*_tmp28A;for(0;effs != 0;effs=effs->tl){
void*_tmp280=(void*)effs->hd;
effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp280));{
void*_tmp281=(void*)effs->hd;void*_tmp282=_tmp281;switch(*((int*)_tmp282)){case 24: _LL253: _LL254:
 goto _LL256;case 23: switch(*((int*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp282)->f1)){case 20: _LL255: _LL256:
 redo_join=1;goto _LL252;case 22: _LL257: _LL258:
 redo_join=1;goto _LL252;case 21: _LL259: _LL25A:
 redo_join=1;goto _LL252;default: goto _LL25B;}default: _LL25B: _LL25C:
 goto _LL252;}_LL252:;};}}
# 1088
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*_tmp28A;for(0;effs != 0;effs=effs->tl){
void*_tmp283=Cyc_Tcutil_compress((void*)effs->hd);void*_tmp284=_tmp283;void*_tmp287;struct Cyc_List_List*_tmp286;switch(*((int*)_tmp284)){case 24: _LL25E: _tmp286=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp284)->f1;_LL25F:
# 1093
 effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp286,effects);
goto _LL25D;case 23: switch(*((int*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp284)->f1)){case 20: _LL260: _LL261:
 goto _LL25D;case 22: _LL262: _LL263:
 goto _LL25D;case 21: _LL264: _LL265:
 goto _LL25D;default: goto _LL266;}default: _LL266: _tmp287=_tmp284;_LL267:
 effects=({struct Cyc_List_List*_tmp285=_cycalloc(sizeof(*_tmp285));_tmp285->hd=_tmp287;_tmp285->tl=effects;_tmp285;});goto _LL25D;}_LL25D:;}}
# 1101
*_tmp28A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
return e;};}case 25: _LL24E: _tmp28B=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp27F)->f1;_LL24F: {
# 1104
void*_tmp288=Cyc_Tcutil_compress(_tmp28B);void*_tmp289=_tmp288;switch(*((int*)_tmp289)){case 1: _LL269: _LL26A:
 goto _LL26C;case 2: _LL26B: _LL26C:
 return e;default: _LL26D: _LL26E:
 return Cyc_Tcutil_rgns_of(_tmp28B);}_LL268:;}default: _LL250: _LL251:
# 1109
 return e;}_LL24B:;};}
# 1114
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp28C=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp28D=_cycalloc(sizeof(*_tmp28D));_tmp28D[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp28E;_tmp28E.tag=9;_tmp28E.f1=({struct Cyc_Absyn_FnInfo _tmp28F;_tmp28F.tvars=0;_tmp28F.effect=eff;_tmp28F.ret_tqual=
Cyc_Absyn_empty_tqual(0);_tmp28F.ret_typ=(void*)& Cyc_Absyn_VoidType_val;_tmp28F.args=0;_tmp28F.c_varargs=0;_tmp28F.cyc_varargs=0;_tmp28F.rgn_po=0;_tmp28F.attributes=0;_tmp28F.requires_clause=0;_tmp28F.requires_relns=0;_tmp28F.ensures_clause=0;_tmp28F.ensures_relns=0;_tmp28F;});_tmp28E;});_tmp28D;});
# 1124
return Cyc_Absyn_atb_typ((void*)_tmp28C,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}
# 1131
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);
if((r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (void*)& Cyc_Absyn_UniqueRgn_val) || r == (void*)& Cyc_Absyn_RefCntRgn_val)
return 1;{
void*_tmp290=Cyc_Tcutil_compress(e);void*_tmp291=_tmp290;struct Cyc_Core_Opt*_tmp2B6;void**_tmp2B5;struct Cyc_Core_Opt*_tmp2B4;void*_tmp2B3;struct Cyc_List_List*_tmp2B2;void*_tmp2B1;switch(*((int*)_tmp291)){case 23: _LL270: _tmp2B1=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp291)->f1;_LL271:
# 1138
 if(constrain)return Cyc_Tcutil_unify(r,_tmp2B1);
_tmp2B1=Cyc_Tcutil_compress(_tmp2B1);
if(r == _tmp2B1)return 1;{
struct _tuple0 _tmp292=({struct _tuple0 _tmp296;_tmp296.f1=r;_tmp296.f2=_tmp2B1;_tmp296;});struct _tuple0 _tmp293=_tmp292;struct Cyc_Absyn_Tvar*_tmp295;struct Cyc_Absyn_Tvar*_tmp294;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp293.f1)->tag == 2){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp293.f2)->tag == 2){_LL27B: _tmp295=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp293.f1)->f1;_tmp294=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp293.f2)->f1;_LL27C:
 return Cyc_Absyn_tvar_cmp(_tmp295,_tmp294)== 0;}else{goto _LL27D;}}else{_LL27D: _LL27E:
 return 0;}_LL27A:;};case 24: _LL272: _tmp2B2=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp291)->f1;_LL273:
# 1146
 for(0;_tmp2B2 != 0;_tmp2B2=_tmp2B2->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp2B2->hd))return 1;}
return 0;case 25: _LL274: _tmp2B3=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp291)->f1;_LL275: {
# 1150
void*_tmp297=Cyc_Tcutil_rgns_of(_tmp2B3);void*_tmp298=_tmp297;void*_tmp2A6;void*_tmp2A5;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp298)->tag == 25){_LL280: _tmp2A5=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp298)->f1;_LL281:
# 1152
 if(!constrain)return 0;{
void*_tmp299=Cyc_Tcutil_compress(_tmp2A5);void*_tmp29A=_tmp299;struct Cyc_Core_Opt*_tmp2A4;void**_tmp2A3;struct Cyc_Core_Opt*_tmp2A2;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp29A)->tag == 1){_LL285: _tmp2A4=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp29A)->f1;_tmp2A3=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp29A)->f2;_tmp2A2=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp29A)->f4;_LL286: {
# 1157
void*_tmp29B=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2A2);
# 1160
Cyc_Tcutil_occurs(_tmp29B,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2A2))->v,r);{
void*_tmp29C=Cyc_Tcutil_dummy_fntype((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp29D=_cycalloc(sizeof(*_tmp29D));_tmp29D[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp29E;_tmp29E.tag=24;_tmp29E.f1=({void*_tmp29F[2];_tmp29F[1]=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp2A0=_cycalloc(sizeof(*_tmp2A0));_tmp2A0[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp2A1;_tmp2A1.tag=23;_tmp2A1.f1=r;_tmp2A1;});_tmp2A0;});_tmp29F[0]=_tmp29B;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp29F,sizeof(void*),2));});_tmp29E;});_tmp29D;}));
*_tmp2A3=_tmp29C;
return 1;};}}else{_LL287: _LL288:
 return 0;}_LL284:;};}else{_LL282: _tmp2A6=_tmp298;_LL283:
# 1166
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp2A6);}_LL27F:;}case 1: _LL276: _tmp2B6=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp291)->f1;_tmp2B5=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp291)->f2;_tmp2B4=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp291)->f4;_LL277:
# 1169
 if(_tmp2B6 == 0  || ((struct Cyc_Absyn_Kind*)_tmp2B6->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp2A7=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2A8="effect evar has wrong kind";_tag_dyneither(_tmp2A8,sizeof(char),27);}),_tag_dyneither(_tmp2A7,sizeof(void*),0));});
if(!constrain)return 0;{
# 1174
void*_tmp2A9=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2B4);
# 1177
Cyc_Tcutil_occurs(_tmp2A9,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2B4))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2AA=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2AB=_cycalloc(sizeof(*_tmp2AB));_tmp2AB[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2AC;_tmp2AC.tag=24;_tmp2AC.f1=({struct Cyc_List_List*_tmp2AD=_cycalloc(sizeof(*_tmp2AD));_tmp2AD->hd=_tmp2A9;_tmp2AD->tl=({struct Cyc_List_List*_tmp2AE=_cycalloc(sizeof(*_tmp2AE));_tmp2AE->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp2AF=_cycalloc(sizeof(*_tmp2AF));_tmp2AF[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp2B0;_tmp2B0.tag=23;_tmp2B0.f1=r;_tmp2B0;});_tmp2AF;});_tmp2AE->tl=0;_tmp2AE;});_tmp2AD;});_tmp2AC;});_tmp2AB;});
*_tmp2B5=(void*)_tmp2AA;
return 1;};};default: _LL278: _LL279:
 return 0;}_LL26F:;};}
# 1188
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Tcutil_compress(t);{
void*_tmp2B7=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_tmp2B8=_tmp2B7;struct Cyc_Core_Opt*_tmp2D0;void**_tmp2CF;struct Cyc_Core_Opt*_tmp2CE;void*_tmp2CD;struct Cyc_List_List*_tmp2CC;switch(*((int*)_tmp2B8)){case 23: _LL28A: _LL28B:
 return 0;case 24: _LL28C: _tmp2CC=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2B8)->f1;_LL28D:
# 1193
 for(0;_tmp2CC != 0;_tmp2CC=_tmp2CC->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp2CC->hd))
return 1;}
return 0;case 25: _LL28E: _tmp2CD=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2B8)->f1;_LL28F:
# 1198
 _tmp2CD=Cyc_Tcutil_compress(_tmp2CD);
if(t == _tmp2CD)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp2CD);{
void*_tmp2B9=Cyc_Tcutil_rgns_of(t);void*_tmp2BA=_tmp2B9;void*_tmp2C1;void*_tmp2C0;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2BA)->tag == 25){_LL295: _tmp2C0=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2BA)->f1;_LL296: {
# 1203
struct _tuple0 _tmp2BB=({struct _tuple0 _tmp2BF;_tmp2BF.f1=Cyc_Tcutil_compress(_tmp2C0);_tmp2BF.f2=_tmp2CD;_tmp2BF;});struct _tuple0 _tmp2BC=_tmp2BB;struct Cyc_Absyn_Tvar*_tmp2BE;struct Cyc_Absyn_Tvar*_tmp2BD;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2BC.f1)->tag == 2){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2BC.f2)->tag == 2){_LL29A: _tmp2BE=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2BC.f1)->f1;_tmp2BD=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2BC.f2)->f1;_LL29B:
 return Cyc_Tcutil_unify(t,_tmp2CD);}else{goto _LL29C;}}else{_LL29C: _LL29D:
 return _tmp2C0 == _tmp2CD;}_LL299:;}}else{_LL297: _tmp2C1=_tmp2BA;_LL298:
# 1207
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp2C1);}_LL294:;};case 1: _LL290: _tmp2D0=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2B8)->f1;_tmp2CF=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2B8)->f2;_tmp2CE=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2B8)->f4;_LL291:
# 1210
 if(_tmp2D0 == 0  || ((struct Cyc_Absyn_Kind*)_tmp2D0->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp2C2=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2C3="effect evar has wrong kind";_tag_dyneither(_tmp2C3,sizeof(char),27);}),_tag_dyneither(_tmp2C2,sizeof(void*),0));});
if(!may_constrain_evars)return 0;{
# 1215
void*_tmp2C4=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2CE);
# 1218
Cyc_Tcutil_occurs(_tmp2C4,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2CE))->v,t);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2C5=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2C6=_cycalloc(sizeof(*_tmp2C6));_tmp2C6[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2C7;_tmp2C7.tag=24;_tmp2C7.f1=({struct Cyc_List_List*_tmp2C8=_cycalloc(sizeof(*_tmp2C8));_tmp2C8->hd=_tmp2C4;_tmp2C8->tl=({struct Cyc_List_List*_tmp2C9=_cycalloc(sizeof(*_tmp2C9));_tmp2C9->hd=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp2CA=_cycalloc(sizeof(*_tmp2CA));_tmp2CA[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp2CB;_tmp2CB.tag=25;_tmp2CB.f1=t;_tmp2CB;});_tmp2CA;});_tmp2C9->tl=0;_tmp2C9;});_tmp2C8;});_tmp2C7;});_tmp2C6;});
*_tmp2CF=(void*)_tmp2C5;
return 1;};};default: _LL292: _LL293:
 return 0;}_LL289:;};}
# 1229
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp2D1=e;struct Cyc_Core_Opt*_tmp2F1;void**_tmp2F0;struct Cyc_Core_Opt*_tmp2EF;void*_tmp2EE;struct Cyc_List_List*_tmp2ED;struct Cyc_Absyn_Tvar*_tmp2EC;switch(*((int*)_tmp2D1)){case 2: _LL29F: _tmp2EC=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2D1)->f1;_LL2A0:
 return Cyc_Absyn_tvar_cmp(v,_tmp2EC)== 0;case 24: _LL2A1: _tmp2ED=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2D1)->f1;_LL2A2:
# 1234
 for(0;_tmp2ED != 0;_tmp2ED=_tmp2ED->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)_tmp2ED->hd))
return 1;}
return 0;case 25: _LL2A3: _tmp2EE=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2D1)->f1;_LL2A4: {
# 1239
void*_tmp2D2=Cyc_Tcutil_rgns_of(_tmp2EE);void*_tmp2D3=_tmp2D2;void*_tmp2E1;void*_tmp2E0;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2D3)->tag == 25){_LL2AA: _tmp2E0=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2D3)->f1;_LL2AB:
# 1241
 if(!may_constrain_evars)return 0;{
void*_tmp2D4=Cyc_Tcutil_compress(_tmp2E0);void*_tmp2D5=_tmp2D4;struct Cyc_Core_Opt*_tmp2DF;void**_tmp2DE;struct Cyc_Core_Opt*_tmp2DD;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2D5)->tag == 1){_LL2AF: _tmp2DF=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2D5)->f1;_tmp2DE=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2D5)->f2;_tmp2DD=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2D5)->f4;_LL2B0: {
# 1247
void*_tmp2D6=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2DD);
# 1249
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2DD))->v,v))return 0;{
void*_tmp2D7=Cyc_Tcutil_dummy_fntype((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2D8=_cycalloc(sizeof(*_tmp2D8));_tmp2D8[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2D9;_tmp2D9.tag=24;_tmp2D9.f1=({void*_tmp2DA[2];_tmp2DA[1]=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2DB=_cycalloc(sizeof(*_tmp2DB));_tmp2DB[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp2DC;_tmp2DC.tag=2;_tmp2DC.f1=v;_tmp2DC;});_tmp2DB;});_tmp2DA[0]=_tmp2D6;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2DA,sizeof(void*),2));});_tmp2D9;});_tmp2D8;}));
*_tmp2DE=_tmp2D7;
return 1;};}}else{_LL2B1: _LL2B2:
 return 0;}_LL2AE:;};}else{_LL2AC: _tmp2E1=_tmp2D3;_LL2AD:
# 1255
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp2E1);}_LL2A9:;}case 1: _LL2A5: _tmp2F1=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2D1)->f1;_tmp2F0=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2D1)->f2;_tmp2EF=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2D1)->f4;_LL2A6:
# 1258
 if(_tmp2F1 == 0  || ((struct Cyc_Absyn_Kind*)_tmp2F1->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp2E2=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2E3="effect evar has wrong kind";_tag_dyneither(_tmp2E3,sizeof(char),27);}),_tag_dyneither(_tmp2E2,sizeof(void*),0));});{
# 1262
void*_tmp2E4=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2EF);
# 1264
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2EF))->v,v))
return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2E5=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2E6=_cycalloc(sizeof(*_tmp2E6));_tmp2E6[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2E7;_tmp2E7.tag=24;_tmp2E7.f1=({struct Cyc_List_List*_tmp2E8=_cycalloc(sizeof(*_tmp2E8));_tmp2E8->hd=_tmp2E4;_tmp2E8->tl=({struct Cyc_List_List*_tmp2E9=_cycalloc(sizeof(*_tmp2E9));_tmp2E9->hd=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2EA=_cycalloc(sizeof(*_tmp2EA));_tmp2EA[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp2EB;_tmp2EB.tag=2;_tmp2EB.f1=v;_tmp2EB;});_tmp2EA;});_tmp2E9->tl=0;_tmp2E9;});_tmp2E8;});_tmp2E7;});_tmp2E6;});
*_tmp2F0=(void*)_tmp2E5;
return 1;};};default: _LL2A7: _LL2A8:
 return 0;}_LL29E:;};}
# 1274
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp2F2=e;void*_tmp2F8;struct Cyc_List_List*_tmp2F7;switch(*((int*)_tmp2F2)){case 24: _LL2B4: _tmp2F7=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2F2)->f1;_LL2B5:
# 1278
 for(0;_tmp2F7 != 0;_tmp2F7=_tmp2F7->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp2F7->hd))
return 1;}
return 0;case 25: _LL2B6: _tmp2F8=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2F2)->f1;_LL2B7: {
# 1283
void*_tmp2F3=Cyc_Tcutil_rgns_of(_tmp2F8);void*_tmp2F4=_tmp2F3;void*_tmp2F6;void*_tmp2F5;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2F4)->tag == 25){_LL2BD: _tmp2F5=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2F4)->f1;_LL2BE:
 return 0;}else{_LL2BF: _tmp2F6=_tmp2F4;_LL2C0:
 return Cyc_Tcutil_evar_in_effect(evar,_tmp2F6);}_LL2BC:;}case 1: _LL2B8: _LL2B9:
# 1287
 return evar == e;default: _LL2BA: _LL2BB:
 return 0;}_LL2B3:;};}
# 1301 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1306
void*_tmp2F9=Cyc_Tcutil_compress(e1);void*_tmp2FA=_tmp2F9;void**_tmp307;struct Cyc_Core_Opt*_tmp306;void*_tmp305;struct Cyc_Absyn_Tvar*_tmp304;void*_tmp303;struct Cyc_List_List*_tmp302;switch(*((int*)_tmp2FA)){case 24: _LL2C2: _tmp302=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2FA)->f1;_LL2C3:
# 1308
 for(0;_tmp302 != 0;_tmp302=_tmp302->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp302->hd,e2))
return 0;}
return 1;case 23: _LL2C4: _tmp303=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp2FA)->f1;_LL2C5:
# 1319
 return Cyc_Tcutil_region_in_effect(may_constrain_evars,_tmp303,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp303,(void*)& Cyc_Absyn_HeapRgn_val);case 2: _LL2C6: _tmp304=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2FA)->f1;_LL2C7:
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp304,e2);case 25: _LL2C8: _tmp305=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2FA)->f1;_LL2C9: {
# 1323
void*_tmp2FB=Cyc_Tcutil_rgns_of(_tmp305);void*_tmp2FC=_tmp2FB;void*_tmp2FE;void*_tmp2FD;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2FC)->tag == 25){_LL2CF: _tmp2FD=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2FC)->f1;_LL2D0:
# 1328
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp2FD,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp2FD,Cyc_Absyn_sint_typ);}else{_LL2D1: _tmp2FE=_tmp2FC;_LL2D2:
 return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp2FE,e2);}_LL2CE:;}case 1: _LL2CA: _tmp307=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2FA)->f2;_tmp306=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2FA)->f4;_LL2CB:
# 1333
 if(!Cyc_Tcutil_evar_in_effect(e1,e2)){
# 1337
*_tmp307=Cyc_Absyn_empty_effect;
# 1340
return 1;}else{
# 1342
return 0;}default: _LL2CC: _LL2CD:
({struct Cyc_String_pa_PrintArg_struct _tmp301;_tmp301.tag=0;_tmp301.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1));({void*_tmp2FF[1]={& _tmp301};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp300="subset_effect: bad effect: %s";_tag_dyneither(_tmp300,sizeof(char),30);}),_tag_dyneither(_tmp2FF,sizeof(void*),1));});});}_LL2C1:;}
# 1358 "tcutil.cyc"
static int Cyc_Tcutil_unify_effect(void*e1,void*e2){
e1=Cyc_Tcutil_normalize_effect(e1);
e2=Cyc_Tcutil_normalize_effect(e2);
if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;
if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;
return 0;}
# 1374
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1376
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple0*_tmp308=(struct _tuple0*)r1->hd;struct _tuple0*_tmp309=_tmp308;void*_tmp30F;void*_tmp30E;_LL2D4: _tmp30F=_tmp309->f1;_tmp30E=_tmp309->f2;_LL2D5:;{
int found=_tmp30F == (void*)& Cyc_Absyn_HeapRgn_val;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
struct _tuple0*_tmp30A=(struct _tuple0*)r2->hd;struct _tuple0*_tmp30B=_tmp30A;void*_tmp30D;void*_tmp30C;_LL2D7: _tmp30D=_tmp30B->f1;_tmp30C=_tmp30B->f2;_LL2D8:;
if(Cyc_Tcutil_unify(_tmp30F,_tmp30D) && Cyc_Tcutil_unify(_tmp30E,_tmp30C)){
found=1;
break;}}}
# 1386
if(!found)return 0;};}}
# 1388
return 1;}
# 1391
typedef struct Cyc_List_List*Cyc_Tcutil_relns_t;
# 1395
static int Cyc_Tcutil_check_logical_implication(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
for(0;r2 != 0;r2=r2->tl){
struct Cyc_Relations_Reln*_tmp310=Cyc_Relations_negate(Cyc_Core_heap_region,(struct Cyc_Relations_Reln*)r2->hd);
struct Cyc_List_List*_tmp311=({struct Cyc_List_List*_tmp312=_cycalloc(sizeof(*_tmp312));_tmp312->hd=_tmp310;_tmp312->tl=r1;_tmp312;});
if(Cyc_Relations_consistent_relations(_tmp311))return 0;}
# 1401
return 1;}
# 1406
static int Cyc_Tcutil_check_logical_equivalence(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
if(r1 == r2)return 1;
return Cyc_Tcutil_check_logical_implication(r1,r2) && Cyc_Tcutil_check_logical_implication(r2,r1);}
# 1412
static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1414
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple22{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*f2;};
# 1418
void Cyc_Tcutil_unify_it(void*t1,void*t2){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=_tag_dyneither(0,0,0);
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return;
{void*_tmp313=t1;struct Cyc_Core_Opt*_tmp322;void**_tmp321;struct Cyc_Core_Opt*_tmp320;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp313)->tag == 1){_LL2DA: _tmp322=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp313)->f1;_tmp321=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp313)->f2;_tmp320=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp313)->f4;_LL2DB:
# 1429
 Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp320))->v,t2);{
struct Cyc_Absyn_Kind*_tmp314=Cyc_Tcutil_typ_kind(t2);
# 1434
if(Cyc_Tcutil_kind_leq(_tmp314,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp322))->v)){
*_tmp321=t2;
return;}else{
# 1438
{void*_tmp315=t2;struct Cyc_Absyn_PtrInfo _tmp31E;void**_tmp31D;struct Cyc_Core_Opt*_tmp31C;switch(*((int*)_tmp315)){case 1: _LL2DF: _tmp31D=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp315)->f2;_tmp31C=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp315)->f4;_LL2E0: {
# 1441
struct Cyc_List_List*_tmp316=(struct Cyc_List_List*)_tmp320->v;
{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp31C))->v;for(0;s2 != 0;s2=s2->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp316,(struct Cyc_Absyn_Tvar*)s2->hd)){
Cyc_Tcutil_failure_reason=({const char*_tmp317="(type variable would escape scope)";_tag_dyneither(_tmp317,sizeof(char),35);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1448
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp322->v,_tmp314)){
*_tmp31D=t1;return;}
# 1451
Cyc_Tcutil_failure_reason=({const char*_tmp318="(kinds are incompatible)";_tag_dyneither(_tmp318,sizeof(char),25);});
goto _LL2DE;}case 5: _LL2E1: _tmp31E=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp315)->f1;if(((struct Cyc_Absyn_Kind*)_tmp322->v)->kind == Cyc_Absyn_BoxKind){_LL2E2: {
# 1457
union Cyc_Absyn_Constraint*_tmp319=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((_tmp31E.ptr_atts).bounds);
{union Cyc_Absyn_Constraint*_tmp31A=_tmp319;if((((union Cyc_Absyn_Constraint*)_tmp31A)->No_constr).tag == 3){_LL2E6: _LL2E7:
# 1461
({struct _union_Constraint_Eq_constr*_tmp31B=& _tmp319->Eq_constr;_tmp31B->tag=1;_tmp31B->val=Cyc_Absyn_bounds_one;});
*_tmp321=t2;
return;}else{_LL2E8: _LL2E9:
 goto _LL2E5;}_LL2E5:;}
# 1466
goto _LL2DE;}}else{goto _LL2E3;}default: _LL2E3: _LL2E4:
 goto _LL2DE;}_LL2DE:;}
# 1469
Cyc_Tcutil_failure_reason=({const char*_tmp31F="(kinds are incompatible)";_tag_dyneither(_tmp31F,sizeof(char),25);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};}else{_LL2DC: _LL2DD:
# 1472
 goto _LL2D9;}_LL2D9:;}
# 1477
{struct _tuple0 _tmp323=({struct _tuple0 _tmp3E8;_tmp3E8.f1=t2;_tmp3E8.f2=t1;_tmp3E8;});struct _tuple0 _tmp324=_tmp323;void*_tmp3E7;void*_tmp3E6;void*_tmp3E5;void*_tmp3E4;void*_tmp3E3;void*_tmp3E2;struct Cyc_List_List*_tmp3E1;struct Cyc_Absyn_Typedefdecl*_tmp3E0;struct Cyc_List_List*_tmp3DF;struct Cyc_Absyn_Typedefdecl*_tmp3DE;enum Cyc_Absyn_AggrKind _tmp3DD;struct Cyc_List_List*_tmp3DC;enum Cyc_Absyn_AggrKind _tmp3DB;struct Cyc_List_List*_tmp3DA;struct Cyc_List_List*_tmp3D9;struct Cyc_List_List*_tmp3D8;struct Cyc_List_List*_tmp3D7;void*_tmp3D6;struct Cyc_Absyn_Tqual _tmp3D5;void*_tmp3D4;struct Cyc_List_List*_tmp3D3;int _tmp3D2;struct Cyc_Absyn_VarargInfo*_tmp3D1;struct Cyc_List_List*_tmp3D0;struct Cyc_List_List*_tmp3CF;struct Cyc_Absyn_Exp*_tmp3CE;struct Cyc_List_List*_tmp3CD;struct Cyc_Absyn_Exp*_tmp3CC;struct Cyc_List_List*_tmp3CB;struct Cyc_List_List*_tmp3CA;void*_tmp3C9;struct Cyc_Absyn_Tqual _tmp3C8;void*_tmp3C7;struct Cyc_List_List*_tmp3C6;int _tmp3C5;struct Cyc_Absyn_VarargInfo*_tmp3C4;struct Cyc_List_List*_tmp3C3;struct Cyc_List_List*_tmp3C2;struct Cyc_Absyn_Exp*_tmp3C1;struct Cyc_List_List*_tmp3C0;struct Cyc_Absyn_Exp*_tmp3BF;struct Cyc_List_List*_tmp3BE;void*_tmp3BD;struct Cyc_Absyn_Tqual _tmp3BC;struct Cyc_Absyn_Exp*_tmp3BB;union Cyc_Absyn_Constraint*_tmp3BA;void*_tmp3B9;struct Cyc_Absyn_Tqual _tmp3B8;struct Cyc_Absyn_Exp*_tmp3B7;union Cyc_Absyn_Constraint*_tmp3B6;struct Cyc_Absyn_Exp*_tmp3B5;struct Cyc_Absyn_Exp*_tmp3B4;void*_tmp3B3;void*_tmp3B2;int _tmp3B1;int _tmp3B0;enum Cyc_Absyn_Sign _tmp3AF;enum Cyc_Absyn_Size_of _tmp3AE;enum Cyc_Absyn_Sign _tmp3AD;enum Cyc_Absyn_Size_of _tmp3AC;void*_tmp3AB;struct Cyc_Absyn_Tqual _tmp3AA;void*_tmp3A9;union Cyc_Absyn_Constraint*_tmp3A8;union Cyc_Absyn_Constraint*_tmp3A7;union Cyc_Absyn_Constraint*_tmp3A6;void*_tmp3A5;struct Cyc_Absyn_Tqual _tmp3A4;void*_tmp3A3;union Cyc_Absyn_Constraint*_tmp3A2;union Cyc_Absyn_Constraint*_tmp3A1;union Cyc_Absyn_Constraint*_tmp3A0;struct Cyc_Absyn_Datatypedecl*_tmp39F;struct Cyc_Absyn_Datatypefield*_tmp39E;struct Cyc_List_List*_tmp39D;struct Cyc_Absyn_Datatypedecl*_tmp39C;struct Cyc_Absyn_Datatypefield*_tmp39B;struct Cyc_List_List*_tmp39A;struct Cyc_Absyn_Datatypedecl*_tmp399;struct Cyc_List_List*_tmp398;struct Cyc_Absyn_Datatypedecl*_tmp397;struct Cyc_List_List*_tmp396;struct Cyc_List_List*_tmp395;struct Cyc_List_List*_tmp394;struct _tuple2*_tmp393;struct _tuple2*_tmp392;union Cyc_Absyn_AggrInfoU _tmp391;struct Cyc_List_List*_tmp390;union Cyc_Absyn_AggrInfoU _tmp38F;struct Cyc_List_List*_tmp38E;struct Cyc_Absyn_Tvar*_tmp38D;struct Cyc_Absyn_Tvar*_tmp38C;switch(*((int*)_tmp324.f1)){case 1: _LL2EB: _LL2EC:
# 1480
 Cyc_Tcutil_unify_it(t2,t1);
return;case 0: switch(*((int*)_tmp324.f2)){case 0: _LL2ED: _LL2EE:
# 1483
 return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 2: switch(*((int*)_tmp324.f2)){case 2: _LL2EF: _tmp38D=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp324.f1)->f1;_tmp38C=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp324.f2)->f1;_LL2F0: {
# 1486
struct _dyneither_ptr*_tmp325=_tmp38D->name;
struct _dyneither_ptr*_tmp326=_tmp38C->name;
# 1489
int _tmp327=_tmp38D->identity;
int _tmp328=_tmp38C->identity;
if(_tmp328 == _tmp327)return;
Cyc_Tcutil_failure_reason=({const char*_tmp329="(variable types are not the same)";_tag_dyneither(_tmp329,sizeof(char),34);});
goto _LL2EA;}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 11: switch(*((int*)_tmp324.f2)){case 11: _LL2F1: _tmp391=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp324.f1)->f1).aggr_info;_tmp390=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp324.f1)->f1).targs;_tmp38F=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp324.f2)->f1).aggr_info;_tmp38E=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp324.f2)->f1).targs;_LL2F2: {
# 1496
struct _tuple13 _tmp32A=Cyc_Absyn_aggr_kinded_name(_tmp38F);struct _tuple13 _tmp32B=_tmp32A;enum Cyc_Absyn_AggrKind _tmp333;struct _tuple2*_tmp332;_LL328: _tmp333=_tmp32B.f1;_tmp332=_tmp32B.f2;_LL329:;{
struct _tuple13 _tmp32C=Cyc_Absyn_aggr_kinded_name(_tmp391);struct _tuple13 _tmp32D=_tmp32C;enum Cyc_Absyn_AggrKind _tmp331;struct _tuple2*_tmp330;_LL32B: _tmp331=_tmp32D.f1;_tmp330=_tmp32D.f2;_LL32C:;
if(_tmp333 != _tmp331){Cyc_Tcutil_failure_reason=({const char*_tmp32E="(struct and union type)";_tag_dyneither(_tmp32E,sizeof(char),24);});goto _LL2EA;}
if(Cyc_Absyn_qvar_cmp(_tmp332,_tmp330)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp32F="(different type name)";_tag_dyneither(_tmp32F,sizeof(char),22);});goto _LL2EA;}
Cyc_Tcutil_unify_list(_tmp38E,_tmp390);
return;};}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 13: switch(*((int*)_tmp324.f2)){case 13: _LL2F3: _tmp393=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp324.f1)->f1;_tmp392=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp324.f2)->f1;_LL2F4:
# 1505
 if(Cyc_Absyn_qvar_cmp(_tmp393,_tmp392)== 0)return;
Cyc_Tcutil_failure_reason=({const char*_tmp334="(different enum types)";_tag_dyneither(_tmp334,sizeof(char),23);});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 14: switch(*((int*)_tmp324.f2)){case 14: _LL2F5: _tmp395=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp324.f1)->f1;_tmp394=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp324.f2)->f1;_LL2F6: {
# 1510
int bad=0;
for(0;_tmp395 != 0  && _tmp394 != 0;(_tmp395=_tmp395->tl,_tmp394=_tmp394->tl)){
struct Cyc_Absyn_Enumfield*_tmp335=(struct Cyc_Absyn_Enumfield*)_tmp395->hd;
struct Cyc_Absyn_Enumfield*_tmp336=(struct Cyc_Absyn_Enumfield*)_tmp394->hd;
if(Cyc_Absyn_qvar_cmp(_tmp335->name,_tmp336->name)!= 0){
Cyc_Tcutil_failure_reason=({const char*_tmp337="(different names for enum fields)";_tag_dyneither(_tmp337,sizeof(char),34);});
bad=1;
break;}
# 1519
if(_tmp335->tag == _tmp336->tag)continue;
if(_tmp335->tag == 0  || _tmp336->tag == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp338="(different tag values for enum fields)";_tag_dyneither(_tmp338,sizeof(char),39);});
bad=1;
break;}
# 1525
if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp335->tag),(struct Cyc_Absyn_Exp*)_check_null(_tmp336->tag))){
Cyc_Tcutil_failure_reason=({const char*_tmp339="(different tag values for enum fields)";_tag_dyneither(_tmp339,sizeof(char),39);});
bad=1;
break;}}
# 1531
if(bad)goto _LL2EA;
if(_tmp395 == 0  && _tmp394 == 0)return;
Cyc_Tcutil_failure_reason=({const char*_tmp33A="(different number of fields for enums)";_tag_dyneither(_tmp33A,sizeof(char),39);});
goto _LL2EA;}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp324.f1)->f1).datatype_info).KnownDatatype).tag == 2)switch(*((int*)_tmp324.f2)){case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp324.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL2F7: _tmp399=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp324.f1)->f1).datatype_info).KnownDatatype).val;_tmp398=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp324.f1)->f1).targs;_tmp397=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp324.f2)->f1).datatype_info).KnownDatatype).val;_tmp396=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp324.f2)->f1).targs;_LL2F8:
# 1538
 if(_tmp399 == _tmp397  || Cyc_Absyn_qvar_cmp(_tmp399->name,_tmp397->name)== 0){
Cyc_Tcutil_unify_list(_tmp396,_tmp398);
return;}
# 1542
goto _LL2EA;}else{goto _LL325;}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}else{switch(*((int*)_tmp324.f2)){case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}}case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp324.f1)->f1).field_info).KnownDatatypefield).tag == 2)switch(*((int*)_tmp324.f2)){case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp324.f2)->f1).field_info).KnownDatatypefield).tag == 2){_LL2F9: _tmp39F=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp324.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp39E=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp324.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp39D=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp324.f1)->f1).targs;_tmp39C=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp324.f2)->f1).field_info).KnownDatatypefield).val).f1;_tmp39B=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp324.f2)->f1).field_info).KnownDatatypefield).val).f2;_tmp39A=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp324.f2)->f1).targs;_LL2FA:
# 1546
 if((_tmp39F == _tmp39C  || Cyc_Absyn_qvar_cmp(_tmp39F->name,_tmp39C->name)== 0) && (
_tmp39E == _tmp39B  || Cyc_Absyn_qvar_cmp(_tmp39E->name,_tmp39B->name)== 0)){
Cyc_Tcutil_unify_list(_tmp39A,_tmp39D);
return;}
# 1551
Cyc_Tcutil_failure_reason=({const char*_tmp33B="(different datatype field types)";_tag_dyneither(_tmp33B,sizeof(char),33);});
goto _LL2EA;}else{goto _LL325;}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}else{switch(*((int*)_tmp324.f2)){case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}}case 5: switch(*((int*)_tmp324.f2)){case 5: _LL2FB: _tmp3AB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp324.f1)->f1).elt_typ;_tmp3AA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp324.f1)->f1).elt_tq;_tmp3A9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp324.f1)->f1).ptr_atts).rgn;_tmp3A8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp324.f1)->f1).ptr_atts).nullable;_tmp3A7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp324.f1)->f1).ptr_atts).bounds;_tmp3A6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp324.f1)->f1).ptr_atts).zero_term;_tmp3A5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp324.f2)->f1).elt_typ;_tmp3A4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp324.f2)->f1).elt_tq;_tmp3A3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp324.f2)->f1).ptr_atts).rgn;_tmp3A2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp324.f2)->f1).ptr_atts).nullable;_tmp3A1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp324.f2)->f1).ptr_atts).bounds;_tmp3A0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp324.f2)->f1).ptr_atts).zero_term;_LL2FC:
# 1556
 Cyc_Tcutil_unify_it(_tmp3A5,_tmp3AB);
Cyc_Tcutil_unify_it(_tmp3A9,_tmp3A3);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp3A0,_tmp3A6,({const char*_tmp33C="(not both zero terminated)";_tag_dyneither(_tmp33C,sizeof(char),27);}));
Cyc_Tcutil_unify_tqual(_tmp3A4,_tmp3A5,_tmp3AA,_tmp3AB);
((void(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp3A1,_tmp3A7,({const char*_tmp33D="(different pointer bounds)";_tag_dyneither(_tmp33D,sizeof(char),27);}));
# 1564
{void*_tmp33E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp3A1);void*_tmp33F=_tmp33E;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp33F)->tag == 0){_LL32E: _LL32F:
 return;}else{_LL330: _LL331:
 goto _LL32D;}_LL32D:;}
# 1568
((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp3A2,_tmp3A8,({const char*_tmp340="(incompatible pointer types)";_tag_dyneither(_tmp340,sizeof(char),29);}));
return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 6: switch(*((int*)_tmp324.f2)){case 6: _LL2FD: _tmp3AF=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp324.f1)->f1;_tmp3AE=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp324.f1)->f2;_tmp3AD=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp324.f2)->f1;_tmp3AC=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp324.f2)->f2;_LL2FE:
# 1573
 if(_tmp3AD == _tmp3AF  && ((_tmp3AC == _tmp3AE  || _tmp3AC == Cyc_Absyn_Int_sz  && _tmp3AE == Cyc_Absyn_Long_sz) || 
_tmp3AC == Cyc_Absyn_Long_sz  && _tmp3AE == Cyc_Absyn_Int_sz))return;
Cyc_Tcutil_failure_reason=({const char*_tmp341="(different integral types)";_tag_dyneither(_tmp341,sizeof(char),27);});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 7: switch(*((int*)_tmp324.f2)){case 7: _LL2FF: _tmp3B1=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp324.f1)->f1;_tmp3B0=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp324.f2)->f1;_LL300:
# 1579
 if(_tmp3B0 == 0  && _tmp3B1 == 0)return;else{
if(_tmp3B0 == 1  && _tmp3B1 == 1)return;else{
# 1582
if(((_tmp3B0 != 0  && _tmp3B0 != 1) && _tmp3B1 != 0) && _tmp3B1 != 1)return;}}
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 19: switch(*((int*)_tmp324.f2)){case 19: _LL301: _tmp3B3=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp324.f1)->f1;_tmp3B2=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp324.f2)->f1;_LL302:
# 1585
 Cyc_Tcutil_unify_it(_tmp3B3,_tmp3B2);return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 18: switch(*((int*)_tmp324.f2)){case 18: _LL303: _tmp3B5=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp324.f1)->f1;_tmp3B4=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp324.f2)->f1;_LL304:
# 1588
 if(!Cyc_Evexp_same_const_exp(_tmp3B5,_tmp3B4)){
Cyc_Tcutil_failure_reason=({const char*_tmp342="(cannot prove expressions are the same)";_tag_dyneither(_tmp342,sizeof(char),40);});
goto _LL2EA;}
# 1592
return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 8: switch(*((int*)_tmp324.f2)){case 8: _LL305: _tmp3BD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp324.f1)->f1).elt_type;_tmp3BC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp324.f1)->f1).tq;_tmp3BB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp324.f1)->f1).num_elts;_tmp3BA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp324.f1)->f1).zero_term;_tmp3B9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp324.f2)->f1).elt_type;_tmp3B8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp324.f2)->f1).tq;_tmp3B7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp324.f2)->f1).num_elts;_tmp3B6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp324.f2)->f1).zero_term;_LL306:
# 1596
 Cyc_Tcutil_unify_it(_tmp3B9,_tmp3BD);
Cyc_Tcutil_unify_tqual(_tmp3B8,_tmp3B9,_tmp3BC,_tmp3BD);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp3BA,_tmp3B6,({const char*_tmp343="(not both zero terminated)";_tag_dyneither(_tmp343,sizeof(char),27);}));
if(_tmp3BB == _tmp3B7)return;
if(_tmp3BB == 0  || _tmp3B7 == 0)goto _LL2EA;
if(Cyc_Evexp_same_const_exp(_tmp3BB,_tmp3B7))
return;
Cyc_Tcutil_failure_reason=({const char*_tmp344="(different array sizes)";_tag_dyneither(_tmp344,sizeof(char),24);});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 9: switch(*((int*)_tmp324.f2)){case 9: _LL307: _tmp3D7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp324.f1)->f1).tvars;_tmp3D6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp324.f1)->f1).effect;_tmp3D5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp324.f1)->f1).ret_tqual;_tmp3D4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp324.f1)->f1).ret_typ;_tmp3D3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp324.f1)->f1).args;_tmp3D2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp324.f1)->f1).c_varargs;_tmp3D1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp324.f1)->f1).cyc_varargs;_tmp3D0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp324.f1)->f1).rgn_po;_tmp3CF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp324.f1)->f1).attributes;_tmp3CE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp324.f1)->f1).requires_clause;_tmp3CD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp324.f1)->f1).requires_relns;_tmp3CC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp324.f1)->f1).ensures_clause;_tmp3CB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp324.f1)->f1).ensures_relns;_tmp3CA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp324.f2)->f1).tvars;_tmp3C9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp324.f2)->f1).effect;_tmp3C8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp324.f2)->f1).ret_tqual;_tmp3C7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp324.f2)->f1).ret_typ;_tmp3C6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp324.f2)->f1).args;_tmp3C5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp324.f2)->f1).c_varargs;_tmp3C4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp324.f2)->f1).cyc_varargs;_tmp3C3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp324.f2)->f1).rgn_po;_tmp3C2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp324.f2)->f1).attributes;_tmp3C1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp324.f2)->f1).requires_clause;_tmp3C0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp324.f2)->f1).requires_relns;_tmp3BF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp324.f2)->f1).ensures_clause;_tmp3BE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp324.f2)->f1).ensures_relns;_LL308: {
# 1610
int done=0;
{struct _RegionHandle _tmp345=_new_region("rgn");struct _RegionHandle*rgn=& _tmp345;_push_region(rgn);
{struct Cyc_List_List*inst=0;
while(_tmp3CA != 0){
if(_tmp3D7 == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp346="(second function type has too few type variables)";_tag_dyneither(_tmp346,sizeof(char),50);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1618
void*_tmp347=((struct Cyc_Absyn_Tvar*)_tmp3CA->hd)->kind;
void*_tmp348=((struct Cyc_Absyn_Tvar*)_tmp3D7->hd)->kind;
if(!Cyc_Tcutil_unify_kindbound(_tmp347,_tmp348)){
Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp34D;_tmp34D.tag=0;_tmp34D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1624
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp3D7->hd,& Cyc_Tcutil_bk)));({struct Cyc_String_pa_PrintArg_struct _tmp34C;_tmp34C.tag=0;_tmp34C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1623
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp3CA->hd,& Cyc_Tcutil_bk)));({struct Cyc_String_pa_PrintArg_struct _tmp34B;_tmp34B.tag=0;_tmp34B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1622
Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)_tmp3CA->hd));({void*_tmp349[3]={& _tmp34B,& _tmp34C,& _tmp34D};Cyc_aprintf(({const char*_tmp34A="(type var %s has different kinds %s and %s)";_tag_dyneither(_tmp34A,sizeof(char),44);}),_tag_dyneither(_tmp349,sizeof(void*),3));});});});});
# 1625
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1627
inst=({struct Cyc_List_List*_tmp34E=_region_malloc(rgn,sizeof(*_tmp34E));_tmp34E->hd=({struct _tuple16*_tmp34F=_region_malloc(rgn,sizeof(*_tmp34F));_tmp34F->f1=(struct Cyc_Absyn_Tvar*)_tmp3D7->hd;_tmp34F->f2=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp350=_cycalloc(sizeof(*_tmp350));_tmp350[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp351;_tmp351.tag=2;_tmp351.f1=(struct Cyc_Absyn_Tvar*)_tmp3CA->hd;_tmp351;});_tmp350;});_tmp34F;});_tmp34E->tl=inst;_tmp34E;});
_tmp3CA=_tmp3CA->tl;
_tmp3D7=_tmp3D7->tl;};}
# 1631
if(_tmp3D7 != 0){
Cyc_Tcutil_failure_reason=({const char*_tmp352="(second function type has too many type variables)";_tag_dyneither(_tmp352,sizeof(char),51);});
_npop_handler(0);goto _LL2EA;}
# 1635
if(inst != 0){
Cyc_Tcutil_unify_it((void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp353=_cycalloc(sizeof(*_tmp353));_tmp353[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp354;_tmp354.tag=9;_tmp354.f1=({struct Cyc_Absyn_FnInfo _tmp355;_tmp355.tvars=0;_tmp355.effect=_tmp3C9;_tmp355.ret_tqual=_tmp3C8;_tmp355.ret_typ=_tmp3C7;_tmp355.args=_tmp3C6;_tmp355.c_varargs=_tmp3C5;_tmp355.cyc_varargs=_tmp3C4;_tmp355.rgn_po=_tmp3C3;_tmp355.attributes=_tmp3C2;_tmp355.requires_clause=_tmp3CE;_tmp355.requires_relns=_tmp3CD;_tmp355.ensures_clause=_tmp3CC;_tmp355.ensures_relns=_tmp3CB;_tmp355;});_tmp354;});_tmp353;}),
# 1639
Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp356=_cycalloc(sizeof(*_tmp356));_tmp356[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp357;_tmp357.tag=9;_tmp357.f1=({struct Cyc_Absyn_FnInfo _tmp358;_tmp358.tvars=0;_tmp358.effect=_tmp3D6;_tmp358.ret_tqual=_tmp3D5;_tmp358.ret_typ=_tmp3D4;_tmp358.args=_tmp3D3;_tmp358.c_varargs=_tmp3D2;_tmp358.cyc_varargs=_tmp3D1;_tmp358.rgn_po=_tmp3D0;_tmp358.attributes=_tmp3CF;_tmp358.requires_clause=_tmp3C1;_tmp358.requires_relns=_tmp3C0;_tmp358.ensures_clause=_tmp3BF;_tmp358.ensures_relns=_tmp3BE;_tmp358;});_tmp357;});_tmp356;})));
# 1644
done=1;}}
# 1612
;_pop_region(rgn);}
# 1647
if(done)
return;
Cyc_Tcutil_unify_it(_tmp3C7,_tmp3D4);
Cyc_Tcutil_unify_tqual(_tmp3C8,_tmp3C7,_tmp3D5,_tmp3D4);
for(0;_tmp3C6 != 0  && _tmp3D3 != 0;(_tmp3C6=_tmp3C6->tl,_tmp3D3=_tmp3D3->tl)){
struct _tuple10 _tmp359=*((struct _tuple10*)_tmp3C6->hd);struct _tuple10 _tmp35A=_tmp359;struct Cyc_Absyn_Tqual _tmp360;void*_tmp35F;_LL333: _tmp360=_tmp35A.f2;_tmp35F=_tmp35A.f3;_LL334:;{
struct _tuple10 _tmp35B=*((struct _tuple10*)_tmp3D3->hd);struct _tuple10 _tmp35C=_tmp35B;struct Cyc_Absyn_Tqual _tmp35E;void*_tmp35D;_LL336: _tmp35E=_tmp35C.f2;_tmp35D=_tmp35C.f3;_LL337:;
Cyc_Tcutil_unify_it(_tmp35F,_tmp35D);
Cyc_Tcutil_unify_tqual(_tmp360,_tmp35F,_tmp35E,_tmp35D);};}
# 1657
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(_tmp3C6 != 0  || _tmp3D3 != 0){
Cyc_Tcutil_failure_reason=({const char*_tmp361="(function types have different number of arguments)";_tag_dyneither(_tmp361,sizeof(char),52);});
goto _LL2EA;}
# 1663
if(_tmp3C5 != _tmp3D2){
Cyc_Tcutil_failure_reason=({const char*_tmp362="(only one function type takes C varargs)";_tag_dyneither(_tmp362,sizeof(char),41);});
goto _LL2EA;}{
# 1668
int bad_cyc_vararg=0;
{struct _tuple22 _tmp363=({struct _tuple22 _tmp36F;_tmp36F.f1=_tmp3C4;_tmp36F.f2=_tmp3D1;_tmp36F;});struct _tuple22 _tmp364=_tmp363;struct _dyneither_ptr*_tmp36E;struct Cyc_Absyn_Tqual _tmp36D;void*_tmp36C;int _tmp36B;struct _dyneither_ptr*_tmp36A;struct Cyc_Absyn_Tqual _tmp369;void*_tmp368;int _tmp367;if(_tmp364.f1 == 0){if(_tmp364.f2 == 0){_LL339: _LL33A:
 goto _LL338;}else{_LL33B: _LL33C:
 goto _LL33E;}}else{if(_tmp364.f2 == 0){_LL33D: _LL33E:
# 1673
 bad_cyc_vararg=1;
Cyc_Tcutil_failure_reason=({const char*_tmp365="(only one function type takes varargs)";_tag_dyneither(_tmp365,sizeof(char),39);});
goto _LL338;}else{_LL33F: _tmp36E=(_tmp364.f1)->name;_tmp36D=(_tmp364.f1)->tq;_tmp36C=(_tmp364.f1)->type;_tmp36B=(_tmp364.f1)->inject;_tmp36A=(_tmp364.f2)->name;_tmp369=(_tmp364.f2)->tq;_tmp368=(_tmp364.f2)->type;_tmp367=(_tmp364.f2)->inject;_LL340:
# 1677
 Cyc_Tcutil_unify_it(_tmp36C,_tmp368);
Cyc_Tcutil_unify_tqual(_tmp36D,_tmp36C,_tmp369,_tmp368);
if(_tmp36B != _tmp367){
bad_cyc_vararg=1;
Cyc_Tcutil_failure_reason=({const char*_tmp366="(only one function type injects varargs)";_tag_dyneither(_tmp366,sizeof(char),41);});}
# 1683
goto _LL338;}}_LL338:;}
# 1685
if(bad_cyc_vararg)goto _LL2EA;{
# 1688
int bad_effect=0;
{struct _tuple0 _tmp370=({struct _tuple0 _tmp372;_tmp372.f1=_tmp3C9;_tmp372.f2=_tmp3D6;_tmp372;});struct _tuple0 _tmp371=_tmp370;if(_tmp371.f1 == 0){if(_tmp371.f2 == 0){_LL342: _LL343:
 goto _LL341;}else{_LL344: _LL345:
 goto _LL347;}}else{if(_tmp371.f2 == 0){_LL346: _LL347:
 bad_effect=1;goto _LL341;}else{_LL348: _LL349:
 bad_effect=!Cyc_Tcutil_unify_effect((void*)_check_null(_tmp3C9),(void*)_check_null(_tmp3D6));goto _LL341;}}_LL341:;}
# 1695
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(bad_effect){
Cyc_Tcutil_failure_reason=({const char*_tmp373="(function effects do not match)";_tag_dyneither(_tmp373,sizeof(char),32);});
goto _LL2EA;}
# 1701
if(!Cyc_Tcutil_same_atts(_tmp3CF,_tmp3C2)){
Cyc_Tcutil_failure_reason=({const char*_tmp374="(function types have different attributes)";_tag_dyneither(_tmp374,sizeof(char),43);});
goto _LL2EA;}
# 1705
if(!Cyc_Tcutil_same_rgn_po(_tmp3D0,_tmp3C3)){
Cyc_Tcutil_failure_reason=({const char*_tmp375="(function types have different region lifetime orderings)";_tag_dyneither(_tmp375,sizeof(char),58);});
goto _LL2EA;}
# 1709
if(!Cyc_Tcutil_check_logical_equivalence(_tmp3CD,_tmp3C0)){
Cyc_Tcutil_failure_reason=({const char*_tmp376="(@requires clauses not equivalent)";_tag_dyneither(_tmp376,sizeof(char),35);});
goto _LL2EA;}
# 1713
if(!Cyc_Tcutil_check_logical_equivalence(_tmp3CB,_tmp3BE)){
Cyc_Tcutil_failure_reason=({const char*_tmp377="(@ensures clauses not equivalent)";_tag_dyneither(_tmp377,sizeof(char),34);});
goto _LL2EA;}
# 1717
return;};};}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 10: switch(*((int*)_tmp324.f2)){case 10: _LL309: _tmp3D9=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp324.f1)->f1;_tmp3D8=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp324.f2)->f1;_LL30A:
# 1720
 for(0;_tmp3D8 != 0  && _tmp3D9 != 0;(_tmp3D8=_tmp3D8->tl,_tmp3D9=_tmp3D9->tl)){
struct _tuple12 _tmp378=*((struct _tuple12*)_tmp3D8->hd);struct _tuple12 _tmp379=_tmp378;struct Cyc_Absyn_Tqual _tmp37F;void*_tmp37E;_LL34B: _tmp37F=_tmp379.f1;_tmp37E=_tmp379.f2;_LL34C:;{
struct _tuple12 _tmp37A=*((struct _tuple12*)_tmp3D9->hd);struct _tuple12 _tmp37B=_tmp37A;struct Cyc_Absyn_Tqual _tmp37D;void*_tmp37C;_LL34E: _tmp37D=_tmp37B.f1;_tmp37C=_tmp37B.f2;_LL34F:;
Cyc_Tcutil_unify_it(_tmp37E,_tmp37C);
Cyc_Tcutil_unify_tqual(_tmp37F,_tmp37E,_tmp37D,_tmp37C);};}
# 1726
if(_tmp3D8 == 0  && _tmp3D9 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp380="(tuple types have different numbers of components)";_tag_dyneither(_tmp380,sizeof(char),51);});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 12: switch(*((int*)_tmp324.f2)){case 12: _LL30B: _tmp3DD=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp324.f1)->f1;_tmp3DC=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp324.f1)->f2;_tmp3DB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp324.f2)->f1;_tmp3DA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp324.f2)->f2;_LL30C:
# 1733
 if(_tmp3DB != _tmp3DD){Cyc_Tcutil_failure_reason=({const char*_tmp381="(struct and union type)";_tag_dyneither(_tmp381,sizeof(char),24);});goto _LL2EA;}
for(0;_tmp3DA != 0  && _tmp3DC != 0;(_tmp3DA=_tmp3DA->tl,_tmp3DC=_tmp3DC->tl)){
struct Cyc_Absyn_Aggrfield*_tmp382=(struct Cyc_Absyn_Aggrfield*)_tmp3DA->hd;
struct Cyc_Absyn_Aggrfield*_tmp383=(struct Cyc_Absyn_Aggrfield*)_tmp3DC->hd;
if(Cyc_strptrcmp(_tmp382->name,_tmp383->name)!= 0){
Cyc_Tcutil_failure_reason=({const char*_tmp384="(different member names)";_tag_dyneither(_tmp384,sizeof(char),25);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1741
Cyc_Tcutil_unify_it(_tmp382->type,_tmp383->type);
Cyc_Tcutil_unify_tqual(_tmp382->tq,_tmp382->type,_tmp383->tq,_tmp383->type);
if(!Cyc_Tcutil_same_atts(_tmp382->attributes,_tmp383->attributes)){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp385="(different attributes on member)";_tag_dyneither(_tmp385,sizeof(char),33);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1749
if((_tmp382->width != 0  && _tmp383->width == 0  || 
_tmp383->width != 0  && _tmp382->width == 0) || 
(_tmp382->width != 0  && _tmp383->width != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp382->width),(struct Cyc_Absyn_Exp*)_check_null(_tmp383->width))){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp386="(different bitfield widths on member)";_tag_dyneither(_tmp386,sizeof(char),38);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1758
if((_tmp382->requires_clause != 0  && _tmp383->requires_clause == 0  || 
_tmp382->requires_clause == 0  && _tmp383->requires_clause != 0) || 
(_tmp382->requires_clause == 0  && _tmp383->requires_clause != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp382->requires_clause),(struct Cyc_Absyn_Exp*)_check_null(_tmp383->requires_clause))){
# 1763
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp387="(different @requires clauses on member)";_tag_dyneither(_tmp387,sizeof(char),40);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1769
if(_tmp3DA == 0  && _tmp3DC == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp388="(different number of members)";_tag_dyneither(_tmp388,sizeof(char),30);});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 17: switch(*((int*)_tmp324.f2)){case 17: _LL30D: _tmp3E1=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp324.f1)->f2;_tmp3E0=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp324.f1)->f3;_tmp3DF=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp324.f2)->f2;_tmp3DE=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp324.f2)->f3;_LL30E:
# 1775
 if(_tmp3E0 != _tmp3DE){
Cyc_Tcutil_failure_reason=({const char*_tmp389="(different abstract typedefs)";_tag_dyneither(_tmp389,sizeof(char),30);});
goto _LL2EA;}
# 1779
Cyc_Tcutil_failure_reason=({const char*_tmp38A="(type parameters to typedef differ)";_tag_dyneither(_tmp38A,sizeof(char),36);});
Cyc_Tcutil_unify_list(_tmp3E1,_tmp3DF);
return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 20: switch(*((int*)_tmp324.f2)){case 20: _LL30F: _LL310:
 return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 21: switch(*((int*)_tmp324.f2)){case 21: _LL311: _LL312:
 return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 22: switch(*((int*)_tmp324.f2)){case 22: _LL313: _LL314:
 return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 15: switch(*((int*)_tmp324.f2)){case 15: _LL315: _tmp3E3=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp324.f1)->f1;_tmp3E2=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp324.f2)->f1;_LL316:
# 1786
 Cyc_Tcutil_unify_it(_tmp3E3,_tmp3E2);
return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 16: switch(*((int*)_tmp324.f2)){case 16: _LL317: _tmp3E7=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp324.f1)->f1;_tmp3E6=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp324.f1)->f2;_tmp3E5=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp324.f2)->f1;_tmp3E4=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp324.f2)->f2;_LL318:
# 1789
 Cyc_Tcutil_unify_it(_tmp3E7,_tmp3E5);
Cyc_Tcutil_unify_it(_tmp3E6,_tmp3E4);
return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 24: _LL319: _LL31A:
 goto _LL31C;default: if(((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp324.f2)->tag == 24){_LL31B: _LL31C:
 goto _LL31E;}else{switch(*((int*)_tmp324.f1)){case 23: _LL31D: _LL31E:
 goto _LL320;case 25: _LL31F: _LL320:
 goto _LL322;default: switch(*((int*)_tmp324.f2)){case 25: _LL321: _LL322:
 goto _LL324;case 23: _LL323: _LL324:
# 1798
 if(Cyc_Tcutil_unify_effect(t1,t2))return;
Cyc_Tcutil_failure_reason=({const char*_tmp38B="(effects don't unify)";_tag_dyneither(_tmp38B,sizeof(char),22);});
goto _LL2EA;default: _LL325: _LL326:
 goto _LL2EA;}}}}_LL2EA:;}
# 1803
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1806
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0  && a2 != 0)return - 1;
if(a1 != 0  && a2 == 0)return 1;
return cmp((void*)_check_null(a1),(void*)_check_null(a2));}
# 1813
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp3E9=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
int _tmp3EA=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp3E9,_tmp3EA);}
# 1820
static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return 0;{
union Cyc_Absyn_Constraint*_tmp3EB=x;void*_tmp3F2;switch((((union Cyc_Absyn_Constraint*)_tmp3EB)->Eq_constr).tag){case 3: _LL351: _LL352:
 return - 1;case 1: _LL353: _tmp3F2=(void*)(_tmp3EB->Eq_constr).val;_LL354: {
# 1827
union Cyc_Absyn_Constraint*_tmp3EC=y;void*_tmp3EF;switch((((union Cyc_Absyn_Constraint*)_tmp3EC)->Eq_constr).tag){case 3: _LL358: _LL359:
 return 1;case 1: _LL35A: _tmp3EF=(void*)(_tmp3EC->Eq_constr).val;_LL35B:
 return cmp(_tmp3F2,_tmp3EF);default: _LL35C: _LL35D:
({void*_tmp3ED=0;Cyc_Tcutil_impos(({const char*_tmp3EE="unify_conref: forward after compress(2)";_tag_dyneither(_tmp3EE,sizeof(char),40);}),_tag_dyneither(_tmp3ED,sizeof(void*),0));});}_LL357:;}default: _LL355: _LL356:
# 1832
({void*_tmp3F0=0;Cyc_Tcutil_impos(({const char*_tmp3F1="unify_conref: forward after compress";_tag_dyneither(_tmp3F1,sizeof(char),37);}),_tag_dyneither(_tmp3F0,sizeof(void*),0));});}_LL350:;};}
# 1836
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct _tuple12*_tmp3F3=tqt1;struct Cyc_Absyn_Tqual _tmp3F9;void*_tmp3F8;_LL35F: _tmp3F9=_tmp3F3->f1;_tmp3F8=_tmp3F3->f2;_LL360:;{
struct _tuple12*_tmp3F4=tqt2;struct Cyc_Absyn_Tqual _tmp3F7;void*_tmp3F6;_LL362: _tmp3F7=_tmp3F4->f1;_tmp3F6=_tmp3F4->f2;_LL363:;{
int _tmp3F5=Cyc_Tcutil_tqual_cmp(_tmp3F9,_tmp3F7);
if(_tmp3F5 != 0)return _tmp3F5;
return Cyc_Tcutil_typecmp(_tmp3F8,_tmp3F6);};};}
# 1844
static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp3FA=Cyc_strptrcmp(f1->name,f2->name);
if(_tmp3FA != 0)return _tmp3FA;{
int _tmp3FB=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);
if(_tmp3FB != 0)return _tmp3FB;{
int _tmp3FC=Cyc_Tcutil_typecmp(f1->type,f2->type);
if(_tmp3FC != 0)return _tmp3FC;{
int _tmp3FD=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);
if(_tmp3FD != 0)return _tmp3FD;
_tmp3FD=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);
if(_tmp3FD != 0)return _tmp3FD;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->requires_clause,f2->requires_clause);};};};}
# 1858
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp3FE=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp3FE != 0)return _tmp3FE;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}
# 1864
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp3FF=t;switch(*((int*)_tmp3FF)){case 0: _LL365: _LL366:
 return 0;case 1: _LL367: _LL368:
 return 1;case 2: _LL369: _LL36A:
 return 2;case 3: _LL36B: _LL36C:
 return 3;case 4: _LL36D: _LL36E:
 return 4;case 5: _LL36F: _LL370:
 return 5;case 6: _LL371: _LL372:
 return 6;case 7: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3FF)->f1 == 0){_LL373: _LL374:
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
# 1901
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return 0;{
int _tmp400=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));
if(_tmp400 != 0)
return _tmp400;{
# 1910
struct _tuple0 _tmp401=({struct _tuple0 _tmp464;_tmp464.f1=t2;_tmp464.f2=t1;_tmp464;});struct _tuple0 _tmp402=_tmp401;struct _dyneither_ptr _tmp463;struct _dyneither_ptr _tmp462;struct Cyc_Absyn_Exp*_tmp461;struct Cyc_Absyn_Exp*_tmp460;struct Cyc_Absyn_Exp*_tmp45F;struct Cyc_Absyn_Exp*_tmp45E;void*_tmp45D;void*_tmp45C;void*_tmp45B;void*_tmp45A;void*_tmp459;void*_tmp458;void*_tmp457;void*_tmp456;enum Cyc_Absyn_AggrKind _tmp455;struct Cyc_List_List*_tmp454;enum Cyc_Absyn_AggrKind _tmp453;struct Cyc_List_List*_tmp452;struct Cyc_List_List*_tmp451;struct Cyc_List_List*_tmp450;void*_tmp44F;struct Cyc_Absyn_Tqual _tmp44E;struct Cyc_Absyn_Exp*_tmp44D;union Cyc_Absyn_Constraint*_tmp44C;void*_tmp44B;struct Cyc_Absyn_Tqual _tmp44A;struct Cyc_Absyn_Exp*_tmp449;union Cyc_Absyn_Constraint*_tmp448;int _tmp447;int _tmp446;enum Cyc_Absyn_Sign _tmp445;enum Cyc_Absyn_Size_of _tmp444;enum Cyc_Absyn_Sign _tmp443;enum Cyc_Absyn_Size_of _tmp442;void*_tmp441;struct Cyc_Absyn_Tqual _tmp440;void*_tmp43F;union Cyc_Absyn_Constraint*_tmp43E;union Cyc_Absyn_Constraint*_tmp43D;union Cyc_Absyn_Constraint*_tmp43C;void*_tmp43B;struct Cyc_Absyn_Tqual _tmp43A;void*_tmp439;union Cyc_Absyn_Constraint*_tmp438;union Cyc_Absyn_Constraint*_tmp437;union Cyc_Absyn_Constraint*_tmp436;struct Cyc_Absyn_Datatypedecl*_tmp435;struct Cyc_Absyn_Datatypefield*_tmp434;struct Cyc_List_List*_tmp433;struct Cyc_Absyn_Datatypedecl*_tmp432;struct Cyc_Absyn_Datatypefield*_tmp431;struct Cyc_List_List*_tmp430;struct Cyc_Absyn_Datatypedecl*_tmp42F;struct Cyc_List_List*_tmp42E;struct Cyc_Absyn_Datatypedecl*_tmp42D;struct Cyc_List_List*_tmp42C;struct Cyc_List_List*_tmp42B;struct Cyc_List_List*_tmp42A;struct _tuple2*_tmp429;struct _tuple2*_tmp428;union Cyc_Absyn_AggrInfoU _tmp427;struct Cyc_List_List*_tmp426;union Cyc_Absyn_AggrInfoU _tmp425;struct Cyc_List_List*_tmp424;struct Cyc_Absyn_Tvar*_tmp423;struct Cyc_Absyn_Tvar*_tmp422;switch(*((int*)_tmp402.f1)){case 1: switch(*((int*)_tmp402.f2)){case 1: _LL3A2: _LL3A3:
# 1912
({void*_tmp403=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp404="typecmp: can only compare closed types";_tag_dyneither(_tmp404,sizeof(char),39);}),_tag_dyneither(_tmp403,sizeof(void*),0));});case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 2: switch(*((int*)_tmp402.f2)){case 2: _LL3A4: _tmp423=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp402.f1)->f1;_tmp422=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp402.f2)->f1;_LL3A5:
# 1916
 return Cyc_Core_intcmp(_tmp422->identity,_tmp423->identity);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 11: switch(*((int*)_tmp402.f2)){case 11: _LL3A6: _tmp427=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp402.f1)->f1).aggr_info;_tmp426=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp402.f1)->f1).targs;_tmp425=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp402.f2)->f1).aggr_info;_tmp424=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp402.f2)->f1).targs;_LL3A7: {
# 1920
struct _tuple13 _tmp405=Cyc_Absyn_aggr_kinded_name(_tmp427);struct _tuple13 _tmp406=_tmp405;struct _tuple2*_tmp40B;_LL3D9: _tmp40B=_tmp406.f2;_LL3DA:;{
struct _tuple13 _tmp407=Cyc_Absyn_aggr_kinded_name(_tmp425);struct _tuple13 _tmp408=_tmp407;struct _tuple2*_tmp40A;_LL3DC: _tmp40A=_tmp408.f2;_LL3DD:;{
int _tmp409=Cyc_Absyn_qvar_cmp(_tmp40B,_tmp40A);
if(_tmp409 != 0)return _tmp409;else{
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp426,_tmp424);}};};}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 13: switch(*((int*)_tmp402.f2)){case 13: _LL3A8: _tmp429=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp402.f1)->f1;_tmp428=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp402.f2)->f1;_LL3A9:
# 1927
 return Cyc_Absyn_qvar_cmp(_tmp429,_tmp428);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 14: switch(*((int*)_tmp402.f2)){case 14: _LL3AA: _tmp42B=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp402.f1)->f1;_tmp42A=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp402.f2)->f1;_LL3AB:
# 1930
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp42B,_tmp42A);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp402.f1)->f1).datatype_info).KnownDatatype).tag == 2)switch(*((int*)_tmp402.f2)){case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp402.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL3AC: _tmp42F=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp402.f1)->f1).datatype_info).KnownDatatype).val;_tmp42E=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp402.f1)->f1).targs;_tmp42D=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp402.f2)->f1).datatype_info).KnownDatatype).val;_tmp42C=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp402.f2)->f1).targs;_LL3AD:
# 1934
 if(_tmp42D == _tmp42F)return 0;{
int _tmp40C=Cyc_Absyn_qvar_cmp(_tmp42D->name,_tmp42F->name);
if(_tmp40C != 0)return _tmp40C;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp42C,_tmp42E);};}else{goto _LL3D6;}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}else{switch(*((int*)_tmp402.f2)){case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}}case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp402.f1)->f1).field_info).KnownDatatypefield).tag == 2)switch(*((int*)_tmp402.f2)){case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp402.f2)->f1).field_info).KnownDatatypefield).tag == 2){_LL3AE: _tmp435=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp402.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp434=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp402.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp433=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp402.f1)->f1).targs;_tmp432=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp402.f2)->f1).field_info).KnownDatatypefield).val).f1;_tmp431=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp402.f2)->f1).field_info).KnownDatatypefield).val).f2;_tmp430=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp402.f2)->f1).targs;_LL3AF:
# 1941
 if(_tmp432 == _tmp435)return 0;{
int _tmp40D=Cyc_Absyn_qvar_cmp(_tmp435->name,_tmp432->name);
if(_tmp40D != 0)return _tmp40D;{
int _tmp40E=Cyc_Absyn_qvar_cmp(_tmp434->name,_tmp431->name);
if(_tmp40E != 0)return _tmp40E;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp430,_tmp433);};};}else{goto _LL3D6;}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}else{switch(*((int*)_tmp402.f2)){case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}}case 5: switch(*((int*)_tmp402.f2)){case 5: _LL3B0: _tmp441=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp402.f1)->f1).elt_typ;_tmp440=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp402.f1)->f1).elt_tq;_tmp43F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp402.f1)->f1).ptr_atts).rgn;_tmp43E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp402.f1)->f1).ptr_atts).nullable;_tmp43D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp402.f1)->f1).ptr_atts).bounds;_tmp43C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp402.f1)->f1).ptr_atts).zero_term;_tmp43B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp402.f2)->f1).elt_typ;_tmp43A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp402.f2)->f1).elt_tq;_tmp439=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp402.f2)->f1).ptr_atts).rgn;_tmp438=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp402.f2)->f1).ptr_atts).nullable;_tmp437=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp402.f2)->f1).ptr_atts).bounds;_tmp436=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp402.f2)->f1).ptr_atts).zero_term;_LL3B1: {
# 1950
int _tmp40F=Cyc_Tcutil_typecmp(_tmp43B,_tmp441);
if(_tmp40F != 0)return _tmp40F;{
int _tmp410=Cyc_Tcutil_typecmp(_tmp439,_tmp43F);
if(_tmp410 != 0)return _tmp410;{
int _tmp411=Cyc_Tcutil_tqual_cmp(_tmp43A,_tmp440);
if(_tmp411 != 0)return _tmp411;{
int _tmp412=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp437,_tmp43D);
if(_tmp412 != 0)return _tmp412;{
int _tmp413=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp436,_tmp43C);
if(_tmp413 != 0)return _tmp413;
{void*_tmp414=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp437);void*_tmp415=_tmp414;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp415)->tag == 0){_LL3DF: _LL3E0:
 return 0;}else{_LL3E1: _LL3E2:
 goto _LL3DE;}_LL3DE:;}
# 1964
return((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp438,_tmp43E);};};};};}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 6: switch(*((int*)_tmp402.f2)){case 6: _LL3B2: _tmp445=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp402.f1)->f1;_tmp444=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp402.f1)->f2;_tmp443=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp402.f2)->f1;_tmp442=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp402.f2)->f2;_LL3B3:
# 1967
 if(_tmp443 != _tmp445)return Cyc_Core_intcmp((int)((unsigned int)_tmp443),(int)((unsigned int)_tmp445));
if(_tmp442 != _tmp444)return Cyc_Core_intcmp((int)((unsigned int)_tmp442),(int)((unsigned int)_tmp444));
return 0;case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 7: switch(*((int*)_tmp402.f2)){case 7: _LL3B4: _tmp447=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp402.f1)->f1;_tmp446=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp402.f2)->f1;_LL3B5:
# 1972
 if(_tmp447 == _tmp446)return 0;else{
if(_tmp446 == 0)return - 1;else{
if(_tmp446 == 1  && _tmp447 == 0)return - 1;else{
return 1;}}}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 8: switch(*((int*)_tmp402.f2)){case 8: _LL3B6: _tmp44F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp402.f1)->f1).elt_type;_tmp44E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp402.f1)->f1).tq;_tmp44D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp402.f1)->f1).num_elts;_tmp44C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp402.f1)->f1).zero_term;_tmp44B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp402.f2)->f1).elt_type;_tmp44A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp402.f2)->f1).tq;_tmp449=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp402.f2)->f1).num_elts;_tmp448=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp402.f2)->f1).zero_term;_LL3B7: {
# 1979
int _tmp416=Cyc_Tcutil_tqual_cmp(_tmp44A,_tmp44E);
if(_tmp416 != 0)return _tmp416;{
int _tmp417=Cyc_Tcutil_typecmp(_tmp44B,_tmp44F);
if(_tmp417 != 0)return _tmp417;{
int _tmp418=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp44C,_tmp448);
if(_tmp418 != 0)return _tmp418;
if(_tmp44D == _tmp449)return 0;
if(_tmp44D == 0  || _tmp449 == 0)
({void*_tmp419=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp41A="missing expression in array index";_tag_dyneither(_tmp41A,sizeof(char),34);}),_tag_dyneither(_tmp419,sizeof(void*),0));});
# 1989
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp44D,_tmp449);};};}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 9: switch(*((int*)_tmp402.f2)){case 9: _LL3B8: _LL3B9:
# 1992
({void*_tmp41B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp41C="typecmp: function types not handled";_tag_dyneither(_tmp41C,sizeof(char),36);}),_tag_dyneither(_tmp41B,sizeof(void*),0));});case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 10: switch(*((int*)_tmp402.f2)){case 10: _LL3BA: _tmp451=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp402.f1)->f1;_tmp450=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp402.f2)->f1;_LL3BB:
# 1995
 return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp450,_tmp451);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 12: switch(*((int*)_tmp402.f2)){case 12: _LL3BC: _tmp455=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp402.f1)->f1;_tmp454=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp402.f1)->f2;_tmp453=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp402.f2)->f1;_tmp452=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp402.f2)->f2;_LL3BD:
# 1998
 if(_tmp453 != _tmp455){
if(_tmp453 == Cyc_Absyn_StructA)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp452,_tmp454);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 15: switch(*((int*)_tmp402.f2)){case 15: _LL3BE: _tmp457=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp402.f1)->f1;_tmp456=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp402.f2)->f1;_LL3BF:
# 2003
 return Cyc_Tcutil_typecmp(_tmp457,_tmp456);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 16: switch(*((int*)_tmp402.f2)){case 16: _LL3C0: _tmp45B=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp402.f1)->f1;_tmp45A=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp402.f1)->f2;_tmp459=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp402.f2)->f1;_tmp458=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp402.f2)->f2;_LL3C1: {
# 2005
int _tmp41D=Cyc_Tcutil_typecmp(_tmp45B,_tmp459);
if(_tmp41D != 0)return _tmp41D;else{
return Cyc_Tcutil_typecmp(_tmp45A,_tmp458);}}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 19: switch(*((int*)_tmp402.f2)){case 19: _LL3C2: _tmp45D=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp402.f1)->f1;_tmp45C=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp402.f2)->f1;_LL3C3:
 return Cyc_Tcutil_typecmp(_tmp45D,_tmp45C);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 18: switch(*((int*)_tmp402.f2)){case 18: _LL3C4: _tmp45F=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp402.f1)->f1;_tmp45E=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp402.f2)->f1;_LL3C5:
 _tmp461=_tmp45F;_tmp460=_tmp45E;goto _LL3C7;case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 27: switch(*((int*)_tmp402.f2)){case 27: _LL3C6: _tmp461=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp402.f1)->f1;_tmp460=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp402.f2)->f1;_LL3C7:
# 2011
 return Cyc_Evexp_const_exp_cmp(_tmp461,_tmp460);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 28: switch(*((int*)_tmp402.f2)){case 28: _LL3C8: _tmp463=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp402.f1)->f1;_tmp462=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp402.f2)->f1;_LL3C9:
# 2013
 return Cyc_strcmp((struct _dyneither_ptr)_tmp463,(struct _dyneither_ptr)_tmp462);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 24: _LL3CA: _LL3CB:
 goto _LL3CD;default: if(((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp402.f2)->tag == 24){_LL3CC: _LL3CD:
 goto _LL3CF;}else{switch(*((int*)_tmp402.f1)){case 23: _LL3CE: _LL3CF:
 goto _LL3D1;case 25: _LL3D0: _LL3D1:
 goto _LL3D3;default: switch(*((int*)_tmp402.f2)){case 25: _LL3D2: _LL3D3:
 goto _LL3D5;case 23: _LL3D4: _LL3D5:
({void*_tmp41E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp41F="typecmp: effects not handled";_tag_dyneither(_tmp41F,sizeof(char),29);}),_tag_dyneither(_tmp41E,sizeof(void*),0));});default: _LL3D6: _LL3D7:
({void*_tmp420=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp421="Unmatched case in typecmp";_tag_dyneither(_tmp421,sizeof(char),26);}),_tag_dyneither(_tmp420,sizeof(void*),0));});}}}}_LL3A1:;};};}
# 2024
int Cyc_Tcutil_is_arithmetic_type(void*t){
void*_tmp465=Cyc_Tcutil_compress(t);void*_tmp466=_tmp465;switch(*((int*)_tmp466)){case 6: _LL3E4: _LL3E5:
 goto _LL3E7;case 7: _LL3E6: _LL3E7:
 goto _LL3E9;case 13: _LL3E8: _LL3E9:
 goto _LL3EB;case 14: _LL3EA: _LL3EB:
 return 1;default: _LL3EC: _LL3ED:
 return 0;}_LL3E3:;}
# 2036
int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp467=({struct _tuple0 _tmp46B;_tmp46B.f1=t1;_tmp46B.f2=t2;_tmp46B;});struct _tuple0 _tmp468=_tmp467;int _tmp46A;int _tmp469;switch(*((int*)_tmp468.f1)){case 7: switch(*((int*)_tmp468.f2)){case 7: _LL3EF: _tmp46A=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp468.f1)->f1;_tmp469=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp468.f2)->f1;_LL3F0:
# 2041
 return _tmp469 == 0  && _tmp46A != 0  || (_tmp469 == 1  && _tmp46A != 0) && _tmp46A != 1;case 6: _LL3F1: _LL3F2:
 goto _LL3F4;case 19: _LL3F3: _LL3F4:
 return 1;default: goto _LL40F;}case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp468.f1)->f2){case Cyc_Absyn_LongLong_sz: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp468.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp468.f2)->f2 == Cyc_Absyn_LongLong_sz){_LL3F5: _LL3F6:
 return 0;}else{goto _LL3F7;}}else{_LL3F7: _LL3F8:
 return 1;}case Cyc_Absyn_Long_sz: switch(*((int*)_tmp468.f2)){case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp468.f2)->f2){case Cyc_Absyn_Int_sz: _LL3F9: _LL3FA:
# 2048
 goto _LL3FC;case Cyc_Absyn_Short_sz: _LL401: _LL402:
# 2053
 goto _LL404;case Cyc_Absyn_Char_sz: _LL407: _LL408:
# 2056
 goto _LL40A;default: goto _LL40F;}case 7: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp468.f2)->f1 == 0){_LL3FD: _LL3FE:
# 2051
 goto _LL400;}else{goto _LL40F;}default: goto _LL40F;}case Cyc_Absyn_Int_sz: switch(*((int*)_tmp468.f2)){case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp468.f2)->f2){case Cyc_Absyn_Long_sz: _LL3FB: _LL3FC:
# 2049
 return 0;case Cyc_Absyn_Short_sz: _LL403: _LL404:
# 2054
 goto _LL406;case Cyc_Absyn_Char_sz: _LL409: _LL40A:
# 2057
 goto _LL40C;default: goto _LL40F;}case 7: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp468.f2)->f1 == 0){_LL3FF: _LL400:
# 2052
 goto _LL402;}else{goto _LL40F;}default: goto _LL40F;}case Cyc_Absyn_Short_sz: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp468.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp468.f2)->f2 == Cyc_Absyn_Char_sz){_LL40B: _LL40C:
# 2058
 goto _LL40E;}else{goto _LL40F;}}else{goto _LL40F;}default: goto _LL40F;}case 19: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp468.f2)->tag == 6)switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp468.f2)->f2){case Cyc_Absyn_Short_sz: _LL405: _LL406:
# 2055
 goto _LL408;case Cyc_Absyn_Char_sz: _LL40D: _LL40E:
# 2059
 return 1;default: goto _LL40F;}else{goto _LL40F;}default: _LL40F: _LL410:
# 2061
 return 0;}_LL3EE:;};}
# 2067
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
# 2070
struct _RegionHandle _tmp46C=_new_region("r");struct _RegionHandle*r=& _tmp46C;_push_region(r);{
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0  || 
Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
max_arith_type=({struct Cyc_Core_Opt*_tmp46D=_region_malloc(r,sizeof(*_tmp46D));_tmp46D->v=t1;_tmp46D;});}}}
# 2080
if(max_arith_type != 0){
if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v)){
int _tmp46E=0;_npop_handler(0);return _tmp46E;}}}
# 2085
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){
({struct Cyc_String_pa_PrintArg_struct _tmp472;_tmp472.tag=0;_tmp472.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp471;_tmp471.tag=0;_tmp471.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp46F[2]={& _tmp471,& _tmp472};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)el->hd)->loc,({const char*_tmp470="type mismatch: expecting %s but found %s";_tag_dyneither(_tmp470,sizeof(char),41);}),_tag_dyneither(_tmp46F,sizeof(void*),2));});});});{
int _tmp473=0;_npop_handler(0);return _tmp473;};}}}{
# 2091
int _tmp474=1;_npop_handler(0);return _tmp474;};
# 2070
;_pop_region(r);}
# 2096
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_typ(te,e)){
void*_tmp475=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp476=_tmp475;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp476)->tag == 5){_LL412: _LL413:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);goto _LL411;}else{_LL414: _LL415:
 return 0;}_LL411:;}
# 2102
return 1;}
# 2105
int Cyc_Tcutil_is_integral_type(void*t){
void*_tmp477=Cyc_Tcutil_compress(t);void*_tmp478=_tmp477;switch(*((int*)_tmp478)){case 6: _LL417: _LL418:
 goto _LL41A;case 19: _LL419: _LL41A:
 goto _LL41C;case 13: _LL41B: _LL41C:
 goto _LL41E;case 14: _LL41D: _LL41E:
 return 1;default: _LL41F: _LL420:
 return 0;}_LL416:;}
# 2116
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
return 1;
# 2120
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
({void*_tmp479=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp47A="integral size mismatch; conversion supplied";_tag_dyneither(_tmp47A,sizeof(char),44);}),_tag_dyneither(_tmp479,sizeof(void*),0));});
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 2126
return 0;}
# 2130
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ))
return 1;
# 2134
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_sint_typ))
({void*_tmp47B=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp47C="integral size mismatch; conversion supplied";_tag_dyneither(_tmp47C,sizeof(char),44);}),_tag_dyneither(_tmp47B,sizeof(void*),0));});
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 2140
return 0;}
# 2145
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);
# 2153
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
# 2155
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp47D=({struct _tuple0 _tmp496;_tmp496.f1=t1;_tmp496.f2=t2;_tmp496;});struct _tuple0 _tmp47E=_tmp47D;void*_tmp495;struct Cyc_Absyn_Tqual _tmp494;struct Cyc_Absyn_Exp*_tmp493;union Cyc_Absyn_Constraint*_tmp492;void*_tmp491;struct Cyc_Absyn_Tqual _tmp490;struct Cyc_Absyn_Exp*_tmp48F;union Cyc_Absyn_Constraint*_tmp48E;struct Cyc_Absyn_PtrInfo _tmp48D;struct Cyc_Absyn_PtrInfo _tmp48C;switch(*((int*)_tmp47E.f1)){case 5: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp47E.f2)->tag == 5){_LL422: _tmp48D=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp47E.f1)->f1;_tmp48C=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp47E.f2)->f1;_LL423: {
# 2159
int okay=1;
# 2161
if(!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp48D.ptr_atts).nullable,(_tmp48C.ptr_atts).nullable))
# 2164
okay=!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp48D.ptr_atts).nullable);
# 2166
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp48D.ptr_atts).bounds,(_tmp48C.ptr_atts).bounds)){
# 2169
struct _tuple0 _tmp47F=({struct _tuple0 _tmp486;_tmp486.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp48D.ptr_atts).bounds);_tmp486.f2=
((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp48C.ptr_atts).bounds);_tmp486;});
# 2169
struct _tuple0 _tmp480=_tmp47F;struct Cyc_Absyn_Exp*_tmp485;struct Cyc_Absyn_Exp*_tmp484;struct Cyc_Absyn_Exp*_tmp483;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp480.f1)->tag == 1){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp480.f2)->tag == 0){_LL42B: _LL42C:
# 2171
 goto _LL42E;}else{_LL42F: _tmp484=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp480.f1)->f1;_tmp483=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp480.f2)->f1;_LL430:
# 2174
 okay=okay  && Cyc_Evexp_lte_const_exp(_tmp483,_tmp484);
# 2178
if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp48C.ptr_atts).zero_term))
({void*_tmp481=0;Cyc_Tcutil_warn(loc,({const char*_tmp482="implicit cast to shorter array";_tag_dyneither(_tmp482,sizeof(char),31);}),_tag_dyneither(_tmp481,sizeof(void*),0));});
goto _LL42A;}}else{if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp480.f2)->tag == 0){_LL42D: _LL42E:
# 2172
 okay=1;goto _LL42A;}else{_LL431: _tmp485=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp480.f2)->f1;_LL432:
# 2183
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp48D.ptr_atts).zero_term) && 
Cyc_Tcutil_is_bound_one((_tmp48C.ptr_atts).bounds))
goto _LL42A;
okay=0;
goto _LL42A;}}_LL42A:;}
# 2192
okay=okay  && (!(_tmp48D.elt_tq).real_const  || (_tmp48C.elt_tq).real_const);
# 2195
if(!Cyc_Tcutil_unify((_tmp48D.ptr_atts).rgn,(_tmp48C.ptr_atts).rgn)){
if(Cyc_Tcenv_region_outlives(te,(_tmp48D.ptr_atts).rgn,(_tmp48C.ptr_atts).rgn)){
if(Cyc_Tcutil_warn_region_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp48A;_tmp48A.tag=0;_tmp48A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2200
Cyc_Absynpp_typ2string((_tmp48C.ptr_atts).rgn));({struct Cyc_String_pa_PrintArg_struct _tmp489;_tmp489.tag=0;_tmp489.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2199
Cyc_Absynpp_typ2string((_tmp48D.ptr_atts).rgn));({void*_tmp487[2]={& _tmp489,& _tmp48A};Cyc_Tcutil_warn(loc,({const char*_tmp488="implicit cast form region %s to region %s";_tag_dyneither(_tmp488,sizeof(char),42);}),_tag_dyneither(_tmp487,sizeof(void*),2));});});});}else{
# 2201
okay=0;}}
# 2205
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp48D.ptr_atts).zero_term,(_tmp48C.ptr_atts).zero_term) || 
# 2208
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,(_tmp48D.ptr_atts).zero_term) && (_tmp48C.elt_tq).real_const);{
# 2216
int _tmp48B=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp48C.ptr_atts).bounds,Cyc_Absyn_bounds_one_conref) && !
# 2219
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp48C.ptr_atts).zero_term);
# 2223
okay=okay  && (Cyc_Tcutil_unify(_tmp48D.elt_typ,_tmp48C.elt_typ) || 
(_tmp48B  && ((_tmp48C.elt_tq).real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp48C.elt_typ)))) && Cyc_Tcutil_ptrsubtype(te,0,_tmp48D.elt_typ,_tmp48C.elt_typ));
# 2226
return okay;};}}else{goto _LL428;}case 8: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp47E.f2)->tag == 8){_LL424: _tmp495=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp47E.f1)->f1).elt_type;_tmp494=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp47E.f1)->f1).tq;_tmp493=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp47E.f1)->f1).num_elts;_tmp492=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp47E.f1)->f1).zero_term;_tmp491=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp47E.f2)->f1).elt_type;_tmp490=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp47E.f2)->f1).tq;_tmp48F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp47E.f2)->f1).num_elts;_tmp48E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp47E.f2)->f1).zero_term;_LL425: {
# 2230
int okay;
# 2232
okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp492,_tmp48E) && (
(_tmp493 != 0  && _tmp48F != 0) && Cyc_Evexp_same_const_exp(_tmp493,_tmp48F));
# 2235
return(okay  && Cyc_Tcutil_unify(_tmp495,_tmp491)) && (!_tmp494.real_const  || _tmp490.real_const);}}else{goto _LL428;}case 19: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp47E.f2)->tag == 6){_LL426: _LL427:
# 2237
 return 0;}else{goto _LL428;}default: _LL428: _LL429:
# 2239
 return Cyc_Tcutil_unify(t1,t2);}_LL421:;};}
# 2243
int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp497=Cyc_Tcutil_compress(t);void*_tmp498=_tmp497;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp498)->tag == 5){_LL434: _LL435:
 return 1;}else{_LL436: _LL437:
 return 0;}_LL433:;}
# 2249
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp499=Cyc_Tcutil_compress(t);void*_tmp49A=_tmp499;void*_tmp49D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49A)->tag == 5){_LL439: _tmp49D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49A)->f1).elt_typ;_LL43A:
 return _tmp49D;}else{_LL43B: _LL43C:
({void*_tmp49B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp49C="pointer_elt_type";_tag_dyneither(_tmp49C,sizeof(char),17);}),_tag_dyneither(_tmp49B,sizeof(void*),0));});}_LL438:;}
# 2255
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp49E=Cyc_Tcutil_compress(t);void*_tmp49F=_tmp49E;struct Cyc_Absyn_PtrAtts*_tmp4A2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49F)->tag == 5){_LL43E: _tmp4A2=(struct Cyc_Absyn_PtrAtts*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49F)->f1).ptr_atts;_LL43F:
 return _tmp4A2->rgn;}else{_LL440: _LL441:
({void*_tmp4A0=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4A1="pointer_elt_type";_tag_dyneither(_tmp4A1,sizeof(char),17);}),_tag_dyneither(_tmp4A0,sizeof(void*),0));});}_LL43D:;}
# 2262
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp4A3=Cyc_Tcutil_compress(t);void*_tmp4A4=_tmp4A3;void*_tmp4A5;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A4)->tag == 5){_LL443: _tmp4A5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A4)->f1).ptr_atts).rgn;_LL444:
# 2265
*rgn=_tmp4A5;
return 1;}else{_LL445: _LL446:
 return 0;}_LL442:;}
# 2272
int Cyc_Tcutil_is_boxed(void*t){
return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;}
# 2276
int Cyc_Tcutil_is_dyneither_ptr(void*t){
void*_tmp4A6=Cyc_Tcutil_compress(t);void*_tmp4A7=_tmp4A6;union Cyc_Absyn_Constraint*_tmp4A8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A7)->tag == 5){_LL448: _tmp4A8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A7)->f1).ptr_atts).bounds;_LL449:
# 2279
 return((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp4A8)== (void*)& Cyc_Absyn_DynEither_b_val;}else{_LL44A: _LL44B:
 return 0;}_LL447:;}
# 2285
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp4A9=e->r;void*_tmp4AA=_tmp4A9;void*_tmp4AE;struct Cyc_Absyn_Exp*_tmp4AD;struct _dyneither_ptr _tmp4AC;switch(*((int*)_tmp4AA)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4AA)->f1).Wchar_c).tag){case 5: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4AA)->f1).Int_c).val).f2 == 0){_LL44D: _LL44E:
 goto _LL450;}else{goto _LL459;}case 2: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4AA)->f1).Char_c).val).f2 == 0){_LL44F: _LL450:
 goto _LL452;}else{goto _LL459;}case 4: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4AA)->f1).Short_c).val).f2 == 0){_LL451: _LL452:
 goto _LL454;}else{goto _LL459;}case 6: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4AA)->f1).LongLong_c).val).f2 == 0){_LL453: _LL454:
# 2291
 return 1;}else{goto _LL459;}case 3: _LL455: _tmp4AC=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4AA)->f1).Wchar_c).val;_LL456: {
# 2293
unsigned long _tmp4AB=Cyc_strlen((struct _dyneither_ptr)_tmp4AC);
int i=0;
if(_tmp4AB >= 2  && *((const char*)_check_dyneither_subscript(_tmp4AC,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp4AC,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmp4AC,sizeof(char),1))== 'x'  && _tmp4AB >= 3) && *((const char*)_check_dyneither_subscript(_tmp4AC,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp4AB;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp4AC,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 2303
return 0;}}default: goto _LL459;}case 13: _LL457: _tmp4AE=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4AA)->f1;_tmp4AD=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4AA)->f2;_LL458:
 return Cyc_Tcutil_is_zero(_tmp4AD) && Cyc_Tcutil_admits_zero(_tmp4AE);default: _LL459: _LL45A:
 return 0;}_LL44C:;}
# 2309
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
# 2316
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 2321
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 2326
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
# 2333
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 2338
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 2343
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp4AF=ka;enum Cyc_Absyn_KindQual _tmp4B8;enum Cyc_Absyn_AliasQual _tmp4B7;_LL45C: _tmp4B8=_tmp4AF->kind;_tmp4B7=_tmp4AF->aliasqual;_LL45D:;
{enum Cyc_Absyn_AliasQual _tmp4B0=_tmp4B7;switch(_tmp4B0){case Cyc_Absyn_Aliasable: _LL45F: _LL460: {
# 2347
enum Cyc_Absyn_KindQual _tmp4B1=_tmp4B8;switch(_tmp4B1){case Cyc_Absyn_AnyKind: _LL466: _LL467:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LL468: _LL469:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL46A: _LL46B:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL46C: _LL46D:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL46E: _LL46F:
 return& Cyc_Tcutil_eko;default: _LL470: _LL471:
 return& Cyc_Tcutil_iko;}_LL465:;}case Cyc_Absyn_Unique: _LL461: _LL462:
# 2356
{enum Cyc_Absyn_KindQual _tmp4B2=_tmp4B8;switch(_tmp4B2){case Cyc_Absyn_AnyKind: _LL473: _LL474:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL475: _LL476:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL477: _LL478:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL479: _LL47A:
 return& Cyc_Tcutil_urko;default: _LL47B: _LL47C:
 goto _LL472;}_LL472:;}
# 2363
goto _LL45E;default: _LL463: _LL464:
# 2365
{enum Cyc_Absyn_KindQual _tmp4B3=_tmp4B8;switch(_tmp4B3){case Cyc_Absyn_AnyKind: _LL47E: _LL47F:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL480: _LL481:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL482: _LL483:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL484: _LL485:
 return& Cyc_Tcutil_trko;default: _LL486: _LL487:
 goto _LL47D;}_LL47D:;}
# 2372
goto _LL45E;}_LL45E:;}
# 2374
({struct Cyc_String_pa_PrintArg_struct _tmp4B6;_tmp4B6.tag=0;_tmp4B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka));({void*_tmp4B4[1]={& _tmp4B6};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4B5="kind_to_opt: bad kind %s\n";_tag_dyneither(_tmp4B5,sizeof(char),26);}),_tag_dyneither(_tmp4B4,sizeof(void*),1));});});}
# 2377
static void**Cyc_Tcutil_kind_to_b(struct Cyc_Absyn_Kind*ka){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ab_v={0,& Cyc_Tcutil_ak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct mb_v={0,& Cyc_Tcutil_mk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct bb_v={0,& Cyc_Tcutil_bk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rb_v={0,& Cyc_Tcutil_rk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct eb_v={0,& Cyc_Tcutil_ek};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ib_v={0,& Cyc_Tcutil_ik};
# 2385
static void*ab=(void*)& ab_v;
static void*mb=(void*)& mb_v;
static void*bb=(void*)& bb_v;
static void*rb=(void*)& rb_v;
static void*eb=(void*)& eb_v;
static void*ib=(void*)& ib_v;
# 2392
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tab_v={0,& Cyc_Tcutil_tak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tmb_v={0,& Cyc_Tcutil_tmk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tbb_v={0,& Cyc_Tcutil_tbk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct trb_v={0,& Cyc_Tcutil_trk};
# 2397
static void*tab=(void*)& tab_v;
static void*tmb=(void*)& tmb_v;
static void*tbb=(void*)& tbb_v;
static void*trb=(void*)& trb_v;
# 2402
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct uab_v={0,& Cyc_Tcutil_uak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct umb_v={0,& Cyc_Tcutil_umk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ubb_v={0,& Cyc_Tcutil_ubk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct urb_v={0,& Cyc_Tcutil_urk};
# 2407
static void*uab=(void*)& uab_v;
static void*umb=(void*)& umb_v;
static void*ubb=(void*)& ubb_v;
static void*urb=(void*)& urb_v;
# 2412
struct Cyc_Absyn_Kind*_tmp4B9=ka;enum Cyc_Absyn_KindQual _tmp4C2;enum Cyc_Absyn_AliasQual _tmp4C1;_LL489: _tmp4C2=_tmp4B9->kind;_tmp4C1=_tmp4B9->aliasqual;_LL48A:;
{enum Cyc_Absyn_AliasQual _tmp4BA=_tmp4C1;switch(_tmp4BA){case Cyc_Absyn_Aliasable: _LL48C: _LL48D: {
# 2415
enum Cyc_Absyn_KindQual _tmp4BB=_tmp4C2;switch(_tmp4BB){case Cyc_Absyn_AnyKind: _LL493: _LL494:
 return& ab;case Cyc_Absyn_MemKind: _LL495: _LL496:
 return& mb;case Cyc_Absyn_BoxKind: _LL497: _LL498:
 return& bb;case Cyc_Absyn_RgnKind: _LL499: _LL49A:
 return& rb;case Cyc_Absyn_EffKind: _LL49B: _LL49C:
 return& eb;default: _LL49D: _LL49E:
 return& ib;}_LL492:;}case Cyc_Absyn_Unique: _LL48E: _LL48F:
# 2424
{enum Cyc_Absyn_KindQual _tmp4BC=_tmp4C2;switch(_tmp4BC){case Cyc_Absyn_AnyKind: _LL4A0: _LL4A1:
 return& uab;case Cyc_Absyn_MemKind: _LL4A2: _LL4A3:
 return& umb;case Cyc_Absyn_BoxKind: _LL4A4: _LL4A5:
 return& ubb;case Cyc_Absyn_RgnKind: _LL4A6: _LL4A7:
 return& urb;default: _LL4A8: _LL4A9:
 goto _LL49F;}_LL49F:;}
# 2431
goto _LL48B;default: _LL490: _LL491:
# 2433
{enum Cyc_Absyn_KindQual _tmp4BD=_tmp4C2;switch(_tmp4BD){case Cyc_Absyn_AnyKind: _LL4AB: _LL4AC:
 return& tab;case Cyc_Absyn_MemKind: _LL4AD: _LL4AE:
 return& tmb;case Cyc_Absyn_BoxKind: _LL4AF: _LL4B0:
 return& tbb;case Cyc_Absyn_RgnKind: _LL4B1: _LL4B2:
 return& trb;default: _LL4B3: _LL4B4:
 goto _LL4AA;}_LL4AA:;}
# 2440
goto _LL48B;}_LL48B:;}
# 2442
({struct Cyc_String_pa_PrintArg_struct _tmp4C0;_tmp4C0.tag=0;_tmp4C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka));({void*_tmp4BE[1]={& _tmp4C0};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4BF="kind_to_b: bad kind %s\n";_tag_dyneither(_tmp4BF,sizeof(char),24);}),_tag_dyneither(_tmp4BE,sizeof(void*),1));});});}
# 2445
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return*Cyc_Tcutil_kind_to_b(k);}
# 2448
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
# 2450
return({struct Cyc_Core_Opt*_tmp4D1=_cycalloc(sizeof(*_tmp4D1));_tmp4D1->v=Cyc_Tcutil_kind_to_bound(k);_tmp4D1;});}
# 2455
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){
e1->r=(void*)({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4D2=_cycalloc(sizeof(*_tmp4D2));_tmp4D2[0]=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp4D3;_tmp4D3.tag=0;_tmp4D3.f1=Cyc_Absyn_Null_c;_tmp4D3;});_tmp4D2;});{
struct Cyc_Core_Opt*_tmp4D4=Cyc_Tcenv_lookup_opt_type_vars(te);
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4D5=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4DA=_cycalloc(sizeof(*_tmp4DA));_tmp4DA[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp4DB;_tmp4DB.tag=5;_tmp4DB.f1=({struct Cyc_Absyn_PtrInfo _tmp4DC;_tmp4DC.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp4D4);_tmp4DC.elt_tq=
Cyc_Absyn_empty_tqual(0);_tmp4DC.ptr_atts=({(_tmp4DC.ptr_atts).rgn=
Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp4D4);(_tmp4DC.ptr_atts).nullable=Cyc_Absyn_true_conref;(_tmp4DC.ptr_atts).bounds=
# 2463
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp4DC.ptr_atts).zero_term=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp4DC.ptr_atts).ptrloc=0;_tmp4DC.ptr_atts;});_tmp4DC;});_tmp4DB;});_tmp4DA;});
e1->topt=(void*)_tmp4D5;{
int bogus=0;
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
if(bogus != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp4D9;_tmp4D9.tag=0;_tmp4D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(e1->loc));({struct Cyc_String_pa_PrintArg_struct _tmp4D8;_tmp4D8.tag=0;_tmp4D8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));({void*_tmp4D6[2]={& _tmp4D8,& _tmp4D9};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4D7="zero_to_null resulted in an alias coercion on %s at %s\n";_tag_dyneither(_tmp4D7,sizeof(char),56);}),_tag_dyneither(_tmp4D6,sizeof(void*),2));});});});
return retv;};};}
# 2473
return 0;}
# 2476
struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
enum Cyc_Absyn_Coercion _tmp4DD=c;switch(_tmp4DD){case Cyc_Absyn_Unknown_coercion: _LL4B6: _LL4B7:
 return({const char*_tmp4DE="unknown";_tag_dyneither(_tmp4DE,sizeof(char),8);});case Cyc_Absyn_No_coercion: _LL4B8: _LL4B9:
 return({const char*_tmp4DF="no coercion";_tag_dyneither(_tmp4DF,sizeof(char),12);});case Cyc_Absyn_NonNull_to_Null: _LL4BA: _LL4BB:
 return({const char*_tmp4E0="null check";_tag_dyneither(_tmp4E0,sizeof(char),11);});default: _LL4BC: _LL4BD:
 return({const char*_tmp4E1="other coercion";_tag_dyneither(_tmp4E1,sizeof(char),15);});}_LL4B5:;}
# 2485
int Cyc_Tcutil_warn_alias_coerce=0;
# 2491
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0,& Cyc_Tcutil_rk};
# 2495
static int counter=0;
struct _tuple2*v=({struct _tuple2*_tmp4F5=_cycalloc(sizeof(*_tmp4F5));_tmp4F5->f1=Cyc_Absyn_Loc_n;_tmp4F5->f2=({struct _dyneither_ptr*_tmp4F6=_cycalloc(sizeof(*_tmp4F6));_tmp4F6[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp4F9;_tmp4F9.tag=1;_tmp4F9.f1=(unsigned long)counter ++;({void*_tmp4F7[1]={& _tmp4F9};Cyc_aprintf(({const char*_tmp4F8="__aliasvar%d";_tag_dyneither(_tmp4F8,sizeof(char),13);}),_tag_dyneither(_tmp4F7,sizeof(void*),1));});});_tmp4F6;});_tmp4F5;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(v,e_typ,e);
struct Cyc_Absyn_Exp*ve=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4F3=_cycalloc(sizeof(*_tmp4F3));_tmp4F3[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp4F4;_tmp4F4.tag=4;_tmp4F4.f1=vd;_tmp4F4;});_tmp4F3;}),e->loc);
# 2504
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 2506
{void*_tmp4E2=Cyc_Tcutil_compress(e_typ);void*_tmp4E3=_tmp4E2;void*_tmp4F1;struct Cyc_Absyn_Tqual _tmp4F0;void*_tmp4EF;union Cyc_Absyn_Constraint*_tmp4EE;union Cyc_Absyn_Constraint*_tmp4ED;union Cyc_Absyn_Constraint*_tmp4EC;struct Cyc_Absyn_PtrLoc*_tmp4EB;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E3)->tag == 5){_LL4BF: _tmp4F1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E3)->f1).elt_typ;_tmp4F0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E3)->f1).elt_tq;_tmp4EF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E3)->f1).ptr_atts).rgn;_tmp4EE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E3)->f1).ptr_atts).nullable;_tmp4ED=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E3)->f1).ptr_atts).bounds;_tmp4EC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E3)->f1).ptr_atts).zero_term;_tmp4EB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E3)->f1).ptr_atts).ptrloc;_LL4C0:
# 2508
{void*_tmp4E4=Cyc_Tcutil_compress(_tmp4EF);void*_tmp4E5=_tmp4E4;void**_tmp4EA;struct Cyc_Core_Opt*_tmp4E9;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4E5)->tag == 1){_LL4C4: _tmp4EA=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4E5)->f2;_tmp4E9=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4E5)->f4;_LL4C5: {
# 2510
struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4E6=({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4E7=_cycalloc(sizeof(*_tmp4E7));_tmp4E7[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4E8;_tmp4E8.tag=2;_tmp4E8.f1=tv;_tmp4E8;});_tmp4E7;});
*_tmp4EA=(void*)_tmp4E6;
goto _LL4C3;}}else{_LL4C6: _LL4C7:
 goto _LL4C3;}_LL4C3:;}
# 2515
goto _LL4BE;}else{_LL4C1: _LL4C2:
 goto _LL4BE;}_LL4BE:;}
# 2519
e->topt=0;
vd->initializer=0;{
# 2523
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 2525
return({struct _tuple15 _tmp4F2;_tmp4F2.f1=d;_tmp4F2.f2=ve;_tmp4F2;});};}
# 2530
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ,void*wants_typ,unsigned int loc){
# 2533
struct _RegionHandle _tmp4FB=_new_region("r");struct _RegionHandle*r=& _tmp4FB;_push_region(r);
if((Cyc_Tcutil_is_noalias_path(r,e) && Cyc_Tcutil_is_noalias_pointer(e_typ,0)) && 
Cyc_Tcutil_is_pointer_type(e_typ)){
# 2538
void*_tmp4FC=Cyc_Tcutil_compress(wants_typ);void*_tmp4FD=_tmp4FC;void*_tmp508;switch(*((int*)_tmp4FD)){case 5: _LL4C9: _tmp508=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FD)->f1).ptr_atts).rgn;_LL4CA: {
# 2540
void*_tmp4FE=Cyc_Tcutil_compress(_tmp508);void*_tmp4FF=_tmp4FE;if(((struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp4FF)->tag == 20){_LL4D0: _LL4D1: {
int _tmp500=0;_npop_handler(0);return _tmp500;}}else{_LL4D2: _LL4D3: {
# 2543
struct Cyc_Absyn_Kind*_tmp501=Cyc_Tcutil_typ_kind(_tmp508);
int _tmp502=_tmp501->kind == Cyc_Absyn_RgnKind  && _tmp501->aliasqual == Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp502;}}_LL4CF:;}case 17: _LL4CB: _LL4CC:
# 2547
({struct Cyc_String_pa_PrintArg_struct _tmp506;_tmp506.tag=0;_tmp506.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(loc));({struct Cyc_String_pa_PrintArg_struct _tmp505;_tmp505.tag=0;_tmp505.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_compress(wants_typ)));({void*_tmp503[2]={& _tmp505,& _tmp506};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp504="got typedef %s in can_insert_alias at %s\n";_tag_dyneither(_tmp504,sizeof(char),42);}),_tag_dyneither(_tmp503,sizeof(void*),2));});});});default: _LL4CD: _LL4CE: {
int _tmp507=0;_npop_handler(0);return _tmp507;}}_LL4C8:;}{
# 2552
int _tmp509=0;_npop_handler(0);return _tmp509;};
# 2534
;_pop_region(r);}
# 2556
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 2561
if(Cyc_Tcutil_unify(t1,t2))return 1;
# 2563
if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
# 2565
if(Cyc_Tcutil_will_lose_precision(t1,t2))
({struct Cyc_String_pa_PrintArg_struct _tmp50D;_tmp50D.tag=0;_tmp50D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_PrintArg_struct _tmp50C;_tmp50C.tag=0;_tmp50C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));({void*_tmp50A[2]={& _tmp50C,& _tmp50D};Cyc_Tcutil_warn(e->loc,({const char*_tmp50B="integral size mismatch; %s -> %s conversion supplied";_tag_dyneither(_tmp50B,sizeof(char),53);}),_tag_dyneither(_tmp50A,sizeof(void*),2));});});});
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
return 1;}else{
# 2574
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp512;_tmp512.tag=0;_tmp512.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_PrintArg_struct _tmp511;_tmp511.tag=0;_tmp511.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));({struct Cyc_String_pa_PrintArg_struct _tmp510;_tmp510.tag=0;_tmp510.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp50E[3]={& _tmp510,& _tmp511,& _tmp512};Cyc_Tcutil_warn(e->loc,({const char*_tmp50F="implicit alias coercion for %s:%s to %s";_tag_dyneither(_tmp50F,sizeof(char),40);}),_tag_dyneither(_tmp50E,sizeof(void*),3));});});});});
*alias_coercion=1;}
# 2581
if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);
return 1;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e))
return 1;else{
if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= Cyc_Absyn_Unknown_coercion){
# 2589
if(c != Cyc_Absyn_No_coercion)Cyc_Tcutil_unchecked_cast(te,e,t2,c);
if(c != Cyc_Absyn_NonNull_to_Null)
({struct Cyc_String_pa_PrintArg_struct _tmp516;_tmp516.tag=0;_tmp516.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_PrintArg_struct _tmp515;_tmp515.tag=0;_tmp515.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));({void*_tmp513[2]={& _tmp515,& _tmp516};Cyc_Tcutil_warn(e->loc,({const char*_tmp514="implicit cast from %s to %s";_tag_dyneither(_tmp514,sizeof(char),28);}),_tag_dyneither(_tmp513,sizeof(void*),2));});});});
return 1;}else{
# 2595
return 0;}}}}}
# 2602
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){
# 2605
int bogus=0;
return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}
# 2609
int Cyc_Tcutil_coerceable(void*t){
void*_tmp517=Cyc_Tcutil_compress(t);void*_tmp518=_tmp517;switch(*((int*)_tmp518)){case 6: _LL4D5: _LL4D6:
 goto _LL4D8;case 7: _LL4D7: _LL4D8:
 return 1;default: _LL4D9: _LL4DA:
 return 0;}_LL4D4:;}
# 2627 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 2631
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ_f(struct _tuple23*env,struct Cyc_Absyn_Aggrfield*x){
# 2634
struct _tuple23 _tmp519=*env;struct _tuple23 _tmp51A=_tmp519;struct Cyc_List_List*_tmp522;struct _RegionHandle*_tmp521;struct Cyc_Tcenv_Tenv*_tmp520;int _tmp51F;_LL4DC: _tmp522=_tmp51A.f1;_tmp521=_tmp51A.f2;_tmp520=_tmp51A.f3;_tmp51F=_tmp51A.f4;_LL4DD:;{
# 2636
void*_tmp51B=_tmp522 == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp521,_tmp522,x->type);
struct Cyc_List_List*_tmp51C=Cyc_Tcutil_flatten_typ(_tmp521,_tmp51F,_tmp520,_tmp51B);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp51C)== 1)
return({struct Cyc_List_List*_tmp51D=_region_malloc(_tmp521,sizeof(*_tmp51D));_tmp51D->hd=({struct _tuple12*_tmp51E=_region_malloc(_tmp521,sizeof(*_tmp51E));_tmp51E->f1=x->tq;_tmp51E->f2=_tmp51B;_tmp51E;});_tmp51D->tl=0;_tmp51D;});else{
return _tmp51C;}};}struct _tuple24{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2642
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple24*env,struct _tuple12*x){
# 2644
struct _tuple24 _tmp523=*env;struct _tuple24 _tmp524=_tmp523;struct _RegionHandle*_tmp52E;struct Cyc_Tcenv_Tenv*_tmp52D;int _tmp52C;_LL4DF: _tmp52E=_tmp524.f1;_tmp52D=_tmp524.f2;_tmp52C=_tmp524.f3;_LL4E0:;{
struct _tuple12 _tmp525=*x;struct _tuple12 _tmp526=_tmp525;struct Cyc_Absyn_Tqual _tmp52B;void*_tmp52A;_LL4E2: _tmp52B=_tmp526.f1;_tmp52A=_tmp526.f2;_LL4E3:;{
struct Cyc_List_List*_tmp527=Cyc_Tcutil_flatten_typ(_tmp52E,_tmp52C,_tmp52D,_tmp52A);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp527)== 1)
return({struct Cyc_List_List*_tmp528=_region_malloc(_tmp52E,sizeof(*_tmp528));_tmp528->hd=({struct _tuple12*_tmp529=_region_malloc(_tmp52E,sizeof(*_tmp529));_tmp529->f1=_tmp52B;_tmp529->f2=_tmp52A;_tmp529;});_tmp528->tl=0;_tmp528;});else{
return _tmp527;}};};}
# 2651
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2655
if(flatten){
t1=Cyc_Tcutil_compress(t1);{
void*_tmp52F=t1;struct Cyc_List_List*_tmp551;struct Cyc_Absyn_Aggrdecl*_tmp550;struct Cyc_List_List*_tmp54F;struct Cyc_List_List*_tmp54E;switch(*((int*)_tmp52F)){case 0: _LL4E5: _LL4E6:
 return 0;case 10: _LL4E7: _tmp54E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp52F)->f1;_LL4E8: {
# 2660
struct _tuple24 _tmp530=({struct _tuple24 _tmp539;_tmp539.f1=r;_tmp539.f2=te;_tmp539.f3=flatten;_tmp539;});
# 2662
struct Cyc_List_List*_tmp531=_tmp54E;struct _tuple12*_tmp538;struct Cyc_List_List*_tmp537;if(_tmp531 == 0){_LL4F0: _LL4F1:
 return 0;}else{_LL4F2: _tmp538=(struct _tuple12*)_tmp531->hd;_tmp537=_tmp531->tl;_LL4F3: {
# 2665
struct Cyc_List_List*_tmp532=Cyc_Tcutil_rcopy_tqt(& _tmp530,_tmp538);
_tmp530.f3=0;{
struct Cyc_List_List*_tmp533=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple12*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp530,_tmp54E);
struct Cyc_List_List*_tmp534=({struct Cyc_List_List*_tmp536=_region_malloc(r,sizeof(*_tmp536));_tmp536->hd=_tmp532;_tmp536->tl=_tmp533;_tmp536;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,({struct Cyc_List_List*_tmp535=_region_malloc(r,sizeof(*_tmp535));_tmp535->hd=_tmp532;_tmp535->tl=_tmp533;_tmp535;}));};}}_LL4EF:;}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp52F)->f1).aggr_info).KnownAggr).tag == 2){_LL4E9: _tmp550=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp52F)->f1).aggr_info).KnownAggr).val;_tmp54F=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp52F)->f1).targs;_LL4EA:
# 2673
 if(((_tmp550->kind == Cyc_Absyn_UnionA  || _tmp550->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp550->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp550->impl))->rgn_po != 0)
# 2675
return({struct Cyc_List_List*_tmp53A=_region_malloc(r,sizeof(*_tmp53A));_tmp53A->hd=({struct _tuple12*_tmp53B=_region_malloc(r,sizeof(*_tmp53B));_tmp53B->f1=Cyc_Absyn_empty_tqual(0);_tmp53B->f2=t1;_tmp53B;});_tmp53A->tl=0;_tmp53A;});{
struct Cyc_List_List*_tmp53C=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp550->tvs,_tmp54F);
struct _tuple23 env=({struct _tuple23 _tmp545;_tmp545.f1=_tmp53C;_tmp545.f2=r;_tmp545.f3=te;_tmp545.f4=flatten;_tmp545;});
struct Cyc_List_List*_tmp53D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp550->impl))->fields;struct Cyc_List_List*_tmp53E=_tmp53D;struct Cyc_Absyn_Aggrfield*_tmp544;struct Cyc_List_List*_tmp543;if(_tmp53E == 0){_LL4F5: _LL4F6:
 return 0;}else{_LL4F7: _tmp544=(struct Cyc_Absyn_Aggrfield*)_tmp53E->hd;_tmp543=_tmp53E->tl;_LL4F8: {
# 2681
struct Cyc_List_List*_tmp53F=Cyc_Tcutil_flatten_typ_f(& env,_tmp544);
env.f4=0;{
struct Cyc_List_List*_tmp540=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp543);
struct Cyc_List_List*_tmp541=({struct Cyc_List_List*_tmp542=_region_malloc(r,sizeof(*_tmp542));_tmp542->hd=_tmp53F;_tmp542->tl=_tmp540;_tmp542;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp541);};}}_LL4F4:;};}else{goto _LL4ED;}case 12: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp52F)->f1 == Cyc_Absyn_StructA){_LL4EB: _tmp551=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp52F)->f2;_LL4EC: {
# 2688
struct _tuple23 env=({struct _tuple23 _tmp54D;_tmp54D.f1=0;_tmp54D.f2=r;_tmp54D.f3=te;_tmp54D.f4=flatten;_tmp54D;});
struct Cyc_List_List*_tmp546=_tmp551;struct Cyc_Absyn_Aggrfield*_tmp54C;struct Cyc_List_List*_tmp54B;if(_tmp546 == 0){_LL4FA: _LL4FB:
 return 0;}else{_LL4FC: _tmp54C=(struct Cyc_Absyn_Aggrfield*)_tmp546->hd;_tmp54B=_tmp546->tl;_LL4FD: {
# 2692
struct Cyc_List_List*_tmp547=Cyc_Tcutil_flatten_typ_f(& env,_tmp54C);
env.f4=0;{
struct Cyc_List_List*_tmp548=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp54B);
struct Cyc_List_List*_tmp549=({struct Cyc_List_List*_tmp54A=_region_malloc(r,sizeof(*_tmp54A));_tmp54A->hd=_tmp547;_tmp54A->tl=_tmp548;_tmp54A;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp549);};}}_LL4F9:;}}else{goto _LL4ED;}default: _LL4ED: _LL4EE:
# 2698
 goto _LL4E4;}_LL4E4:;};}
# 2701
return({struct Cyc_List_List*_tmp552=_region_malloc(r,sizeof(*_tmp552));_tmp552->hd=({struct _tuple12*_tmp553=_region_malloc(r,sizeof(*_tmp553));_tmp553->f1=Cyc_Absyn_empty_tqual(0);_tmp553->f2=t1;_tmp553;});_tmp552->tl=0;_tmp552;});}
# 2705
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp554=(void*)t->hd;void*_tmp555=_tmp554;switch(*((int*)_tmp555)){case 23: _LL4FF: _LL500:
 goto _LL502;case 4: _LL501: _LL502:
 goto _LL504;case 20: _LL503: _LL504:
# 2711
 continue;default: _LL505: _LL506:
# 2713
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;}_LL4FE:;}}
# 2716
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}
# 2719
return 1;}
# 2722
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp556=({struct _tuple0 _tmp55A;_tmp55A.f1=Cyc_Tcutil_compress(t1);_tmp55A.f2=Cyc_Tcutil_compress(t2);_tmp55A;});struct _tuple0 _tmp557=_tmp556;enum Cyc_Absyn_Size_of _tmp559;enum Cyc_Absyn_Size_of _tmp558;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp557.f1)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp557.f2)->tag == 6){_LL508: _tmp559=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp557.f1)->f2;_tmp558=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp557.f2)->f2;_LL509:
# 2725
 return(_tmp559 == _tmp558  || _tmp559 == Cyc_Absyn_Int_sz  && _tmp558 == Cyc_Absyn_Long_sz) || 
_tmp559 == Cyc_Absyn_Long_sz  && _tmp558 == Cyc_Absyn_Int_sz;}else{goto _LL50A;}}else{_LL50A: _LL50B:
 return 0;}_LL507:;}
# 2733
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2736
if(Cyc_Tcutil_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}
# 2741
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp55B=({struct _tuple0 _tmp591;_tmp591.f1=t1;_tmp591.f2=t2;_tmp591;});struct _tuple0 _tmp55C=_tmp55B;struct Cyc_Absyn_FnInfo _tmp590;struct Cyc_Absyn_FnInfo _tmp58F;struct Cyc_Absyn_Datatypedecl*_tmp58E;struct Cyc_Absyn_Datatypefield*_tmp58D;struct Cyc_List_List*_tmp58C;struct Cyc_Absyn_Datatypedecl*_tmp58B;struct Cyc_List_List*_tmp58A;void*_tmp589;struct Cyc_Absyn_Tqual _tmp588;void*_tmp587;union Cyc_Absyn_Constraint*_tmp586;union Cyc_Absyn_Constraint*_tmp585;union Cyc_Absyn_Constraint*_tmp584;void*_tmp583;struct Cyc_Absyn_Tqual _tmp582;void*_tmp581;union Cyc_Absyn_Constraint*_tmp580;union Cyc_Absyn_Constraint*_tmp57F;union Cyc_Absyn_Constraint*_tmp57E;switch(*((int*)_tmp55C.f1)){case 5: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55C.f2)->tag == 5){_LL50D: _tmp589=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55C.f1)->f1).elt_typ;_tmp588=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55C.f1)->f1).elt_tq;_tmp587=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55C.f1)->f1).ptr_atts).rgn;_tmp586=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55C.f1)->f1).ptr_atts).nullable;_tmp585=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55C.f1)->f1).ptr_atts).bounds;_tmp584=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55C.f1)->f1).ptr_atts).zero_term;_tmp583=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55C.f2)->f1).elt_typ;_tmp582=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55C.f2)->f1).elt_tq;_tmp581=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55C.f2)->f1).ptr_atts).rgn;_tmp580=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55C.f2)->f1).ptr_atts).nullable;_tmp57F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55C.f2)->f1).ptr_atts).bounds;_tmp57E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55C.f2)->f1).ptr_atts).zero_term;_LL50E:
# 2749
 if(_tmp588.real_const  && !_tmp582.real_const)
return 0;
# 2752
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp586,_tmp580) && 
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp586)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp580))
return 0;
# 2756
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp584,_tmp57E) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp584)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp57E))
return 0;
# 2760
if((!Cyc_Tcutil_unify(_tmp587,_tmp581) && !Cyc_Tcenv_region_outlives(te,_tmp587,_tmp581)) && !
Cyc_Tcutil_subtype(te,assume,_tmp587,_tmp581))
return 0;
# 2764
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp585,_tmp57F)){
struct _tuple0 _tmp55D=({struct _tuple0 _tmp561;_tmp561.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp585);_tmp561.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp57F);_tmp561;});struct _tuple0 _tmp55E=_tmp55D;struct Cyc_Absyn_Exp*_tmp560;struct Cyc_Absyn_Exp*_tmp55F;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp55E.f1)->tag == 0){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp55E.f2)->tag == 0){_LL516: _LL517:
 goto _LL515;}else{goto _LL51A;}}else{if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp55E.f2)->tag == 1){_LL518: _tmp560=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp55E.f1)->f1;_tmp55F=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp55E.f2)->f1;_LL519:
# 2769
 if(!Cyc_Evexp_lte_const_exp(_tmp55F,_tmp560))
return 0;
goto _LL515;}else{_LL51A: _LL51B:
 return 0;}}_LL515:;}
# 2777
if(!_tmp582.real_const  && _tmp588.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp583)))
return 0;}{
# 2783
int _tmp562=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp57F,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp57E);
# 2789
return(_tmp562  && Cyc_Tcutil_ptrsubtype(te,({struct Cyc_List_List*_tmp563=_cycalloc(sizeof(*_tmp563));_tmp563->hd=({struct _tuple0*_tmp564=_cycalloc(sizeof(*_tmp564));_tmp564->f1=t1;_tmp564->f2=t2;_tmp564;});_tmp563->tl=assume;_tmp563;}),_tmp589,_tmp583) || Cyc_Tcutil_unify(_tmp589,_tmp583)) || Cyc_Tcutil_isomorphic(_tmp589,_tmp583);};}else{goto _LL513;}case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp55C.f1)->f1).field_info).KnownDatatypefield).tag == 2){if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp55C.f2)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp55C.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL50F: _tmp58E=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp55C.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp58D=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp55C.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp58C=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp55C.f1)->f1).targs;_tmp58B=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp55C.f2)->f1).datatype_info).KnownDatatype).val;_tmp58A=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp55C.f2)->f1).targs;_LL510:
# 2795
 if(_tmp58E != _tmp58B  && Cyc_Absyn_qvar_cmp(_tmp58E->name,_tmp58B->name)!= 0)return 0;
# 2797
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp58C)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp58A))return 0;
for(0;_tmp58C != 0;(_tmp58C=_tmp58C->tl,_tmp58A=_tmp58A->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp58C->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp58A))->hd))return 0;}
return 1;}else{goto _LL513;}}else{goto _LL513;}}else{goto _LL513;}case 9: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp55C.f2)->tag == 9){_LL511: _tmp590=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp55C.f1)->f1;_tmp58F=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp55C.f2)->f1;_LL512:
# 2804
 if(_tmp590.tvars != 0  || _tmp58F.tvars != 0){
struct Cyc_List_List*_tmp565=_tmp590.tvars;
struct Cyc_List_List*_tmp566=_tmp58F.tvars;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp565)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp566))return 0;{
struct _RegionHandle _tmp567=_new_region("r");struct _RegionHandle*r=& _tmp567;_push_region(r);
{struct Cyc_List_List*inst=0;
while(_tmp565 != 0){
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp565->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp566))->hd)->kind)){int _tmp568=0;_npop_handler(0);return _tmp568;}
inst=({struct Cyc_List_List*_tmp569=_region_malloc(r,sizeof(*_tmp569));_tmp569->hd=({struct _tuple16*_tmp56A=_region_malloc(r,sizeof(*_tmp56A));_tmp56A->f1=(struct Cyc_Absyn_Tvar*)_tmp566->hd;_tmp56A->f2=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp56B=_cycalloc(sizeof(*_tmp56B));_tmp56B[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp56C;_tmp56C.tag=2;_tmp56C.f1=(struct Cyc_Absyn_Tvar*)_tmp565->hd;_tmp56C;});_tmp56B;});_tmp56A;});_tmp569->tl=inst;_tmp569;});
_tmp565=_tmp565->tl;
_tmp566=_tmp566->tl;}
# 2816
if(inst != 0){
_tmp590.tvars=0;
_tmp58F.tvars=0;{
int _tmp571=Cyc_Tcutil_subtype(te,assume,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp56D=_cycalloc(sizeof(*_tmp56D));_tmp56D[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp56E;_tmp56E.tag=9;_tmp56E.f1=_tmp590;_tmp56E;});_tmp56D;}),(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp56F=_cycalloc(sizeof(*_tmp56F));_tmp56F[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp570;_tmp570.tag=9;_tmp570.f1=_tmp58F;_tmp570;});_tmp56F;}));_npop_handler(0);return _tmp571;};}}
# 2809
;_pop_region(r);};}
# 2824
if(!Cyc_Tcutil_subtype(te,assume,_tmp590.ret_typ,_tmp58F.ret_typ))return 0;{
struct Cyc_List_List*_tmp572=_tmp590.args;
struct Cyc_List_List*_tmp573=_tmp58F.args;
# 2829
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp572)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp573))return 0;
# 2831
for(0;_tmp572 != 0;(_tmp572=_tmp572->tl,_tmp573=_tmp573->tl)){
struct _tuple10 _tmp574=*((struct _tuple10*)_tmp572->hd);struct _tuple10 _tmp575=_tmp574;struct Cyc_Absyn_Tqual _tmp57B;void*_tmp57A;_LL51D: _tmp57B=_tmp575.f2;_tmp57A=_tmp575.f3;_LL51E:;{
struct _tuple10 _tmp576=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp573))->hd);struct _tuple10 _tmp577=_tmp576;struct Cyc_Absyn_Tqual _tmp579;void*_tmp578;_LL520: _tmp579=_tmp577.f2;_tmp578=_tmp577.f3;_LL521:;
# 2835
if(_tmp579.real_const  && !_tmp57B.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp578,_tmp57A))
return 0;};}
# 2839
if(_tmp590.c_varargs != _tmp58F.c_varargs)return 0;
if(_tmp590.cyc_varargs != 0  && _tmp58F.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp57C=*_tmp590.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp57D=*_tmp58F.cyc_varargs;
# 2844
if((_tmp57D.tq).real_const  && !(_tmp57C.tq).real_const  || !
Cyc_Tcutil_subtype(te,assume,_tmp57D.type,_tmp57C.type))
return 0;}else{
if(_tmp590.cyc_varargs != 0  || _tmp58F.cyc_varargs != 0)return 0;}
# 2849
if(!Cyc_Tcutil_subset_effect(1,(void*)_check_null(_tmp590.effect),(void*)_check_null(_tmp58F.effect)))return 0;
# 2851
if(!Cyc_Tcutil_sub_rgnpo(_tmp590.rgn_po,_tmp58F.rgn_po))return 0;
# 2853
if(!Cyc_Tcutil_sub_attributes(_tmp590.attributes,_tmp58F.attributes))return 0;
# 2855
if(!Cyc_Tcutil_check_logical_implication(_tmp58F.requires_relns,_tmp590.requires_relns))
return 0;
# 2858
if(!Cyc_Tcutil_check_logical_implication(_tmp590.ensures_relns,_tmp58F.ensures_relns))
return 0;
# 2861
return 1;};}else{goto _LL513;}default: _LL513: _LL514:
 return 0;}_LL50C:;};}
# 2873 "tcutil.cyc"
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2875
struct _RegionHandle _tmp592=_new_region("temp");struct _RegionHandle*temp=& _tmp592;_push_region(temp);
{struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(temp,1,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(temp,1,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0){int _tmp593=0;_npop_handler(0);return _tmp593;}{
struct _tuple12*_tmp594=(struct _tuple12*)tqs1->hd;struct _tuple12*_tmp595=_tmp594;struct Cyc_Absyn_Tqual _tmp59D;void*_tmp59C;_LL523: _tmp59D=_tmp595->f1;_tmp59C=_tmp595->f2;_LL524:;{
struct _tuple12*_tmp596=(struct _tuple12*)tqs2->hd;struct _tuple12*_tmp597=_tmp596;struct Cyc_Absyn_Tqual _tmp59B;void*_tmp59A;_LL526: _tmp59B=_tmp597->f1;_tmp59A=_tmp597->f2;_LL527:;
# 2883
if(_tmp59D.real_const  && !_tmp59B.real_const){int _tmp598=0;_npop_handler(0);return _tmp598;}
# 2885
if((_tmp59B.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp59A))) && 
Cyc_Tcutil_subtype(te,assume,_tmp59C,_tmp59A))
# 2888
continue;
# 2890
if(Cyc_Tcutil_unify(_tmp59C,_tmp59A))
# 2892
continue;
# 2894
if(Cyc_Tcutil_isomorphic(_tmp59C,_tmp59A))
# 2896
continue;{
# 2899
int _tmp599=0;_npop_handler(0);return _tmp599;};};};}{
# 2901
int _tmp59E=1;_npop_handler(0);return _tmp59E;};}
# 2876
;_pop_region(temp);}
# 2905
static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp59F=Cyc_Tcutil_compress(t);void*_tmp5A0=_tmp59F;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A0)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A0)->f2 == Cyc_Absyn_Char_sz){_LL529: _LL52A:
 return 1;}else{goto _LL52B;}}else{_LL52B: _LL52C:
 return 0;}_LL528:;}
# 2914
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
# 2920
if(t2 == (void*)& Cyc_Absyn_VoidType_val)
return Cyc_Absyn_No_coercion;
{void*_tmp5A1=t2;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A1)->tag == 6)switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A1)->f2){case Cyc_Absyn_Int_sz: _LL52E: _LL52F:
# 2924
 goto _LL531;case Cyc_Absyn_Long_sz: _LL530: _LL531:
# 2926
 if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL52D;default: goto _LL532;}else{_LL532: _LL533:
 goto _LL52D;}_LL52D:;}{
# 2930
void*_tmp5A2=t1;void*_tmp5CA;struct Cyc_Absyn_Enumdecl*_tmp5C9;void*_tmp5C8;struct Cyc_Absyn_Tqual _tmp5C7;struct Cyc_Absyn_Exp*_tmp5C6;union Cyc_Absyn_Constraint*_tmp5C5;void*_tmp5C4;struct Cyc_Absyn_Tqual _tmp5C3;void*_tmp5C2;union Cyc_Absyn_Constraint*_tmp5C1;union Cyc_Absyn_Constraint*_tmp5C0;union Cyc_Absyn_Constraint*_tmp5BF;switch(*((int*)_tmp5A2)){case 5: _LL535: _tmp5C4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A2)->f1).elt_typ;_tmp5C3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A2)->f1).elt_tq;_tmp5C2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A2)->f1).ptr_atts).rgn;_tmp5C1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A2)->f1).ptr_atts).nullable;_tmp5C0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A2)->f1).ptr_atts).bounds;_tmp5BF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A2)->f1).ptr_atts).zero_term;_LL536:
# 2938
{void*_tmp5A3=t2;void*_tmp5B5;struct Cyc_Absyn_Tqual _tmp5B4;void*_tmp5B3;union Cyc_Absyn_Constraint*_tmp5B2;union Cyc_Absyn_Constraint*_tmp5B1;union Cyc_Absyn_Constraint*_tmp5B0;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A3)->tag == 5){_LL544: _tmp5B5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A3)->f1).elt_typ;_tmp5B4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A3)->f1).elt_tq;_tmp5B3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A3)->f1).ptr_atts).rgn;_tmp5B2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A3)->f1).ptr_atts).nullable;_tmp5B1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A3)->f1).ptr_atts).bounds;_tmp5B0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A3)->f1).ptr_atts).zero_term;_LL545: {
# 2942
enum Cyc_Absyn_Coercion coercion=Cyc_Absyn_Other_coercion;
struct Cyc_List_List*_tmp5A4=({struct Cyc_List_List*_tmp5AE=_cycalloc(sizeof(*_tmp5AE));_tmp5AE->hd=({struct _tuple0*_tmp5AF=_cycalloc(sizeof(*_tmp5AF));_tmp5AF->f1=t1;_tmp5AF->f2=t2;_tmp5AF;});_tmp5AE->tl=0;_tmp5AE;});
int _tmp5A5=_tmp5B4.real_const  || !_tmp5C3.real_const;
# 2956 "tcutil.cyc"
int _tmp5A6=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp5B1,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp5B0);
# 2960
int _tmp5A7=_tmp5A5  && (
((_tmp5A6  && Cyc_Tcutil_ptrsubtype(te,_tmp5A4,_tmp5C4,_tmp5B5) || 
Cyc_Tcutil_unify(_tmp5C4,_tmp5B5)) || Cyc_Tcutil_isomorphic(_tmp5C4,_tmp5B5)) || Cyc_Tcutil_unify(_tmp5B5,(void*)& Cyc_Absyn_VoidType_val));
# 2964
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp5BF,_tmp5B0) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp5B0);
# 2968
int _tmp5A8=_tmp5A7?0:((Cyc_Tcutil_bits_only(_tmp5C4) && Cyc_Tcutil_is_char_type(_tmp5B5)) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5B0)) && (
_tmp5B4.real_const  || !_tmp5C3.real_const);
int bounds_ok=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp5C0,_tmp5B1);
if(!bounds_ok  && !_tmp5A8){
struct _tuple0 _tmp5A9=({struct _tuple0 _tmp5AD;_tmp5AD.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp5C0);_tmp5AD.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp5B1);_tmp5AD;});struct _tuple0 _tmp5AA=_tmp5A9;struct Cyc_Absyn_Exp*_tmp5AC;struct Cyc_Absyn_Exp*_tmp5AB;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5AA.f1)->tag == 1){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5AA.f2)->tag == 1){_LL549: _tmp5AC=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5AA.f1)->f1;_tmp5AB=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5AA.f2)->f1;_LL54A:
# 2978
 if(Cyc_Evexp_lte_const_exp(_tmp5AB,_tmp5AC))
bounds_ok=1;
goto _LL548;}else{goto _LL54B;}}else{_LL54B: _LL54C:
# 2983
 bounds_ok=1;goto _LL548;}_LL548:;}
# 2985
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5C1) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5B2))
coercion=Cyc_Absyn_NonNull_to_Null;
# 2990
if(((bounds_ok  && zeroterm_ok) && (_tmp5A7  || _tmp5A8)) && (
Cyc_Tcutil_unify(_tmp5C2,_tmp5B3) || Cyc_Tcenv_region_outlives(te,_tmp5C2,_tmp5B3)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};}}else{_LL546: _LL547:
 goto _LL543;}_LL543:;}
# 2996
return Cyc_Absyn_Unknown_coercion;case 8: _LL537: _tmp5C8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A2)->f1).elt_type;_tmp5C7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A2)->f1).tq;_tmp5C6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A2)->f1).num_elts;_tmp5C5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A2)->f1).zero_term;_LL538:
# 2998
{void*_tmp5B6=t2;void*_tmp5BA;struct Cyc_Absyn_Tqual _tmp5B9;struct Cyc_Absyn_Exp*_tmp5B8;union Cyc_Absyn_Constraint*_tmp5B7;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5B6)->tag == 8){_LL54E: _tmp5BA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5B6)->f1).elt_type;_tmp5B9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5B6)->f1).tq;_tmp5B8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5B6)->f1).num_elts;_tmp5B7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5B6)->f1).zero_term;_LL54F: {
# 3000
int okay;
okay=
(((_tmp5C6 != 0  && _tmp5B8 != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp5C5,_tmp5B7)) && 
Cyc_Evexp_lte_const_exp(_tmp5B8,_tmp5C6)) && 
Cyc_Evexp_lte_const_exp(_tmp5C6,_tmp5B8);
return
# 3007
(okay  && Cyc_Tcutil_unify(_tmp5C8,_tmp5BA)) && (!_tmp5C7.real_const  || _tmp5B9.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}}else{_LL550: _LL551:
# 3009
 return Cyc_Absyn_Unknown_coercion;}_LL54D:;}
# 3011
return Cyc_Absyn_Unknown_coercion;case 13: _LL539: _tmp5C9=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5A2)->f2;_LL53A:
# 3015
{void*_tmp5BB=t2;struct Cyc_Absyn_Enumdecl*_tmp5BC;if(((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5BB)->tag == 13){_LL553: _tmp5BC=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5BB)->f2;_LL554:
# 3017
 if((_tmp5C9->fields != 0  && _tmp5BC->fields != 0) && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5C9->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5BC->fields))->v))
return Cyc_Absyn_Other_coercion;
goto _LL552;}else{_LL555: _LL556:
 goto _LL552;}_LL552:;}
# 3023
goto _LL53C;case 6: _LL53B: _LL53C:
 goto _LL53E;case 7: _LL53D: _LL53E:
 return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;case 15: _LL53F: _tmp5CA=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5A2)->f1;_LL540:
# 3028
{void*_tmp5BD=t2;void*_tmp5BE;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5BD)->tag == 15){_LL558: _tmp5BE=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5BD)->f1;_LL559:
# 3030
 if(Cyc_Tcenv_region_outlives(te,_tmp5CA,_tmp5BE))return Cyc_Absyn_No_coercion;
goto _LL557;}else{_LL55A: _LL55B:
 goto _LL557;}_LL557:;}
# 3034
return Cyc_Absyn_Unknown_coercion;default: _LL541: _LL542:
 return Cyc_Absyn_Unknown_coercion;}_LL534:;};}
# 3040
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),t)){
struct Cyc_Absyn_Exp*_tmp5CB=Cyc_Absyn_copy_exp(e);
e->r=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp5CC=_cycalloc(sizeof(*_tmp5CC));_tmp5CC[0]=({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp5CD;_tmp5CD.tag=13;_tmp5CD.f1=t;_tmp5CD.f2=_tmp5CB;_tmp5CD.f3=0;_tmp5CD.f4=c;_tmp5CD;});_tmp5CC;});
e->topt=t;}}
# 3048
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp5CE=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp5CF=_tmp5CE;switch(*((int*)_tmp5CF)){case 6: _LL55D: _LL55E:
 goto _LL560;case 13: _LL55F: _LL560:
 goto _LL562;case 14: _LL561: _LL562:
 goto _LL564;case 19: _LL563: _LL564:
 return 1;case 1: _LL565: _LL566:
 return Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ);default: _LL567: _LL568:
 return 0;}_LL55C:;}
# 3059
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp5D0=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp5D1=_tmp5D0;if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D1)->tag == 7){_LL56A: _LL56B:
 return 1;}else{_LL56C: _LL56D:
 return 0;}_LL569:;};}
# 3068
int Cyc_Tcutil_is_function_type(void*t){
void*_tmp5D2=Cyc_Tcutil_compress(t);void*_tmp5D3=_tmp5D2;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5D3)->tag == 9){_LL56F: _LL570:
 return 1;}else{_LL571: _LL572:
 return 0;}_LL56E:;}
# 3075
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
struct _tuple0 _tmp5D4=({struct _tuple0 _tmp5D8;_tmp5D8.f1=t1;_tmp5D8.f2=t2;_tmp5D8;});struct _tuple0 _tmp5D5=_tmp5D4;int _tmp5D7;int _tmp5D6;if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D5.f1)->tag == 7){if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D5.f2)->tag == 7){_LL574: _tmp5D7=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D5.f1)->f1;_tmp5D6=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D5.f2)->f1;_LL575:
# 3078
 if(_tmp5D7 != 0  && _tmp5D7 != 1)return t1;else{
if(_tmp5D6 != 0  && _tmp5D6 != 1)return t2;else{
if(_tmp5D7 >= _tmp5D6)return t1;else{
return t2;}}}}else{_LL576: _LL577:
 return t1;}}else{if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D5.f2)->tag == 7){_LL578: _LL579:
 return t2;}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D5.f1)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D5.f1)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D5.f1)->f2 == Cyc_Absyn_LongLong_sz){_LL57A: _LL57B:
 goto _LL57D;}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D5.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D5.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D5.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL57C;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D5.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL582;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D5.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL584;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D5.f1)->f2 == Cyc_Absyn_Int_sz)goto _LL586;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D5.f2)->f2 == Cyc_Absyn_Int_sz)goto _LL588;else{goto _LL58E;}}}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D5.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL580;else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D5.f1)->f2){case Cyc_Absyn_Long_sz: goto _LL582;case Cyc_Absyn_Int_sz: goto _LL586;default: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D5.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL58C;else{goto _LL58E;}}}}}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D5.f1)->f2){case Cyc_Absyn_Long_sz: _LL582: _LL583:
# 3088
 goto _LL585;case Cyc_Absyn_Int_sz: _LL586: _LL587:
# 3091
 goto _LL589;default: goto _LL58E;}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D5.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D5.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D5.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL57C;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D5.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL57E;else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D5.f2)->f2){case Cyc_Absyn_Long_sz: goto _LL584;case Cyc_Absyn_Int_sz: goto _LL588;default: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D5.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL58A;else{goto _LL58E;}}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D5.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL57E;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D5.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL580;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D5.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL58A;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D5.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL58C;else{goto _LL58E;}}}}}}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D5.f1)->f2){case Cyc_Absyn_LongLong_sz: _LL57E: _LL57F:
# 3086
 goto _LL581;case Cyc_Absyn_Long_sz: _LL58A: _LL58B:
# 3093
 goto _LL58D;default: goto _LL58E;}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D5.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D5.f2)->f1 == Cyc_Absyn_Unsigned)switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D5.f2)->f2){case Cyc_Absyn_LongLong_sz: _LL57C: _LL57D:
# 3085
 return Cyc_Absyn_ulonglong_typ;case Cyc_Absyn_Long_sz: _LL584: _LL585:
# 3089
 return Cyc_Absyn_ulong_typ;case Cyc_Absyn_Int_sz: _LL588: _LL589:
# 3092
 return Cyc_Absyn_uint_typ;default: goto _LL58E;}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D5.f2)->f2){case Cyc_Absyn_LongLong_sz: _LL580: _LL581:
# 3087
 return Cyc_Absyn_slonglong_typ;case Cyc_Absyn_Long_sz: _LL58C: _LL58D:
# 3094
 return Cyc_Absyn_slong_typ;default: goto _LL58E;}}}else{_LL58E: _LL58F:
 return Cyc_Absyn_sint_typ;}}}}_LL573:;}
# 3106 "tcutil.cyc"
static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){
c1=Cyc_Absyn_compress_kb(c1);
c2=Cyc_Absyn_compress_kb(c2);
if(c1 == c2)return 1;{
struct _tuple0 _tmp5D9=({struct _tuple0 _tmp5EF;_tmp5EF.f1=c1;_tmp5EF.f2=c2;_tmp5EF;});struct _tuple0 _tmp5DA=_tmp5D9;struct Cyc_Core_Opt**_tmp5EE;struct Cyc_Absyn_Kind*_tmp5ED;struct Cyc_Core_Opt**_tmp5EC;struct Cyc_Absyn_Kind*_tmp5EB;struct Cyc_Core_Opt**_tmp5EA;struct Cyc_Absyn_Kind*_tmp5E9;struct Cyc_Absyn_Kind*_tmp5E8;struct Cyc_Core_Opt**_tmp5E7;struct Cyc_Core_Opt**_tmp5E6;struct Cyc_Absyn_Kind*_tmp5E5;struct Cyc_Core_Opt**_tmp5E4;struct Cyc_Absyn_Kind*_tmp5E3;struct Cyc_Absyn_Kind*_tmp5E2;struct Cyc_Absyn_Kind*_tmp5E1;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5DA.f1)->tag == 0)switch(*((int*)_tmp5DA.f2)){case 0: _LL591: _tmp5E2=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5DA.f1)->f1;_tmp5E1=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5DA.f2)->f1;_LL592:
 return _tmp5E2 == _tmp5E1;case 1: goto _LL593;default: _LL599: _tmp5E5=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5DA.f1)->f1;_tmp5E4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5DA.f2)->f1;_tmp5E3=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5DA.f2)->f2;_LL59A:
# 3119
 if(Cyc_Tcutil_kind_leq(_tmp5E5,_tmp5E3)){
*_tmp5E4=({struct Cyc_Core_Opt*_tmp5DE=_cycalloc(sizeof(*_tmp5DE));_tmp5DE->v=c1;_tmp5DE;});return 1;}else{
return 0;}}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5DA.f2)->tag == 1){_LL593: _tmp5E6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5DA.f2)->f1;_LL594:
# 3112
*_tmp5E6=({struct Cyc_Core_Opt*_tmp5DB=_cycalloc(sizeof(*_tmp5DB));_tmp5DB->v=c1;_tmp5DB;});return 1;}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5DA.f1)->tag == 1){_LL595: _tmp5E7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5DA.f1)->f1;_LL596:
*_tmp5E7=({struct Cyc_Core_Opt*_tmp5DC=_cycalloc(sizeof(*_tmp5DC));_tmp5DC->v=c2;_tmp5DC;});return 1;}else{if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5DA.f2)->tag == 0){_LL597: _tmp5EA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5DA.f1)->f1;_tmp5E9=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5DA.f1)->f2;_tmp5E8=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5DA.f2)->f1;_LL598:
# 3115
 if(Cyc_Tcutil_kind_leq(_tmp5E8,_tmp5E9)){
*_tmp5EA=({struct Cyc_Core_Opt*_tmp5DD=_cycalloc(sizeof(*_tmp5DD));_tmp5DD->v=c2;_tmp5DD;});return 1;}else{
return 0;}}else{_LL59B: _tmp5EE=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5DA.f1)->f1;_tmp5ED=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5DA.f1)->f2;_tmp5EC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5DA.f2)->f1;_tmp5EB=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5DA.f2)->f2;_LL59C:
# 3123
 if(Cyc_Tcutil_kind_leq(_tmp5ED,_tmp5EB)){
*_tmp5EC=({struct Cyc_Core_Opt*_tmp5DF=_cycalloc(sizeof(*_tmp5DF));_tmp5DF->v=c1;_tmp5DF;});return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp5EB,_tmp5ED)){
*_tmp5EE=({struct Cyc_Core_Opt*_tmp5E0=_cycalloc(sizeof(*_tmp5E0));_tmp5E0->v=c2;_tmp5E0;});return 1;}else{
return 0;}}}}}}_LL590:;};}
# 3132
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 3137
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp5F4;_tmp5F4.tag=1;_tmp5F4.f1=(unsigned long)i;({void*_tmp5F2[1]={& _tmp5F4};Cyc_aprintf(({const char*_tmp5F3="#%d";_tag_dyneither(_tmp5F3,sizeof(char),4);}),_tag_dyneither(_tmp5F2,sizeof(void*),1));});});
return({struct Cyc_Absyn_Tvar*_tmp5F0=_cycalloc(sizeof(*_tmp5F0));_tmp5F0->name=({struct _dyneither_ptr*_tmp5F1=_cycalloc(sizeof(struct _dyneither_ptr)* 1);_tmp5F1[0]=(struct _dyneither_ptr)s;_tmp5F1;});_tmp5F0->identity=- 1;_tmp5F0->kind=k;_tmp5F0;});}
# 3144
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _dyneither_ptr _tmp5F5=*t->name;
return*((const char*)_check_dyneither_subscript(_tmp5F5,sizeof(char),0))== '#';}
# 3149
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
({struct Cyc_String_pa_PrintArg_struct _tmp5F8;_tmp5F8.tag=0;_tmp5F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name);({void*_tmp5F6[1]={& _tmp5F8};Cyc_printf(({const char*_tmp5F7="%s";_tag_dyneither(_tmp5F7,sizeof(char),3);}),_tag_dyneither(_tmp5F6,sizeof(void*),1));});});
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
struct _dyneither_ptr _tmp5F9=Cyc_strconcat(({const char*_tmp5FE="`";_tag_dyneither(_tmp5FE,sizeof(char),2);}),(struct _dyneither_ptr)*t->name);
({struct _dyneither_ptr _tmp5FA=_dyneither_ptr_plus(_tmp5F9,sizeof(char),1);char _tmp5FB=*((char*)_check_dyneither_subscript(_tmp5FA,sizeof(char),0));char _tmp5FC='t';if(_get_dyneither_size(_tmp5FA,sizeof(char))== 1  && (_tmp5FB == '\000'  && _tmp5FC != '\000'))_throw_arraybounds();*((char*)_tmp5FA.curr)=_tmp5FC;});
t->name=({struct _dyneither_ptr*_tmp5FD=_cycalloc(sizeof(struct _dyneither_ptr)* 1);_tmp5FD[0]=(struct _dyneither_ptr)_tmp5F9;_tmp5FD;});};}
# 3158
static struct _tuple10*Cyc_Tcutil_fndecl2typ_f(struct _tuple10*x){
# 3160
return({struct _tuple10*_tmp5FF=_cycalloc(sizeof(*_tmp5FF));_tmp5FF->f1=(*x).f1;_tmp5FF->f2=(*x).f2;_tmp5FF->f3=(*x).f3;_tmp5FF;});}
# 3163
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_typ == 0){
# 3170
struct Cyc_List_List*_tmp600=0;
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd))
_tmp600=({struct Cyc_List_List*_tmp601=_cycalloc(sizeof(*_tmp601));_tmp601->hd=(void*)atts->hd;_tmp601->tl=_tmp600;_tmp601;});}}
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp602=_cycalloc(sizeof(*_tmp602));_tmp602[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp603;_tmp603.tag=9;_tmp603.f1=({struct Cyc_Absyn_FnInfo _tmp604;_tmp604.tvars=fd->tvs;_tmp604.effect=fd->effect;_tmp604.ret_tqual=fd->ret_tqual;_tmp604.ret_typ=fd->ret_type;_tmp604.args=
((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args);_tmp604.c_varargs=fd->c_varargs;_tmp604.cyc_varargs=fd->cyc_varargs;_tmp604.rgn_po=fd->rgn_po;_tmp604.attributes=_tmp600;_tmp604.requires_clause=fd->requires_clause;_tmp604.requires_relns=fd->requires_relns;_tmp604.ensures_clause=fd->ensures_clause;_tmp604.ensures_relns=fd->ensures_relns;_tmp604;});_tmp603;});_tmp602;});}
# 3183
return(void*)_check_null(fd->cached_typ);}
# 3189
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 3191
union Cyc_Relations_RelnOp _tmp605=*rop;union Cyc_Relations_RelnOp _tmp606=_tmp605;struct Cyc_Absyn_Vardecl*_tmp619;struct Cyc_Absyn_Vardecl*_tmp618;switch((_tmp606.RNumelts).tag){case 2: _LL59E: _tmp618=(_tmp606.RVar).val;_LL59F: {
# 3193
struct _tuple2 _tmp607=*_tmp618->name;struct _tuple2 _tmp608=_tmp607;union Cyc_Absyn_Nmspace _tmp60F;struct _dyneither_ptr*_tmp60E;_LL5A5: _tmp60F=_tmp608.f1;_tmp60E=_tmp608.f2;_LL5A6:;
if(!((int)((_tmp60F.Loc_n).tag == 4)))goto _LL59D;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp60E,({const char*_tmp609="return_value";_tag_dyneither(_tmp609,sizeof(char),13);}))== 0){
*rop=Cyc_Relations_RReturn();
goto _LL59D;}{
# 3199
unsigned int c=0;
{struct Cyc_List_List*_tmp60A=args;for(0;_tmp60A != 0;(_tmp60A=_tmp60A->tl,c ++)){
struct _tuple10*_tmp60B=(struct _tuple10*)_tmp60A->hd;struct _tuple10*_tmp60C=_tmp60B;struct _dyneither_ptr*_tmp60D;_LL5A8: _tmp60D=_tmp60C->f1;_LL5A9:;
if(_tmp60D != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp60E,(struct _dyneither_ptr)*_tmp60D)== 0){
*rop=Cyc_Relations_RParam(c);
break;}}}}
# 3209
goto _LL59D;};}case 3: _LL5A0: _tmp619=(_tmp606.RNumelts).val;_LL5A1: {
# 3211
struct _tuple2 _tmp610=*_tmp619->name;struct _tuple2 _tmp611=_tmp610;union Cyc_Absyn_Nmspace _tmp617;struct _dyneither_ptr*_tmp616;_LL5AB: _tmp617=_tmp611.f1;_tmp616=_tmp611.f2;_LL5AC:;
if(!((int)((_tmp617.Loc_n).tag == 4)))goto _LL59D;{
unsigned int c=0;
{struct Cyc_List_List*_tmp612=args;for(0;_tmp612 != 0;(_tmp612=_tmp612->tl,c ++)){
struct _tuple10*_tmp613=(struct _tuple10*)_tmp612->hd;struct _tuple10*_tmp614=_tmp613;struct _dyneither_ptr*_tmp615;_LL5AE: _tmp615=_tmp614->f1;_LL5AF:;
if(_tmp615 != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp616,(struct _dyneither_ptr)*_tmp615)== 0){
*rop=Cyc_Relations_RParamNumelts(c);
break;}}}}
# 3223
goto _LL59D;};}default: _LL5A2: _LL5A3:
 goto _LL59D;}_LL59D:;}
# 3228
static void Cyc_Tcutil_replace_rops(struct Cyc_List_List*args,struct Cyc_Relations_Reln*r){
# 3230
Cyc_Tcutil_replace_rop(args,& r->rop1);
Cyc_Tcutil_replace_rop(args,& r->rop2);}
# 3234
static struct Cyc_List_List*Cyc_Tcutil_extract_relns(struct Cyc_List_List*args,struct Cyc_Absyn_Exp*e){
# 3236
if(e == 0)return 0;{
struct Cyc_List_List*_tmp61A=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,_tmp61A);
return _tmp61A;};}struct _tuple25{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 3243
static void*Cyc_Tcutil_fst_fdarg(struct _tuple25*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct _tuple12*_tmp61B=pr;struct Cyc_Absyn_Tqual _tmp61E;void*_tmp61D;_LL5B1: _tmp61E=_tmp61B->f1;_tmp61D=_tmp61B->f2;_LL5B2:;
if(_tmp61D == t)return pr;else{
return({struct _tuple12*_tmp61C=_cycalloc(sizeof(*_tmp61C));_tmp61C->f1=_tmp61E;_tmp61C->f2=t;_tmp61C;});}}struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 3250
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
# 3252
return({struct _tuple27*_tmp61F=_region_malloc(rgn,sizeof(*_tmp61F));_tmp61F->f1=({struct _tuple26*_tmp620=_region_malloc(rgn,sizeof(*_tmp620));_tmp620->f1=(*y).f1;_tmp620->f2=(*y).f2;_tmp620;});_tmp61F->f2=(*y).f3;_tmp61F;});}
# 3254
static struct _tuple10*Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){
# 3256
struct _tuple10 _tmp621=*orig_arg;struct _tuple10 _tmp622=_tmp621;struct _dyneither_ptr*_tmp626;struct Cyc_Absyn_Tqual _tmp625;void*_tmp624;_LL5B4: _tmp626=_tmp622.f1;_tmp625=_tmp622.f2;_tmp624=_tmp622.f3;_LL5B5:;
if(t == _tmp624)return orig_arg;
return({struct _tuple10*_tmp623=_cycalloc(sizeof(*_tmp623));_tmp623->f1=_tmp626;_tmp623->f2=_tmp625;_tmp623->f3=t;_tmp623;});}
# 3260
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){
return f->type;}
# 3263
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 3268
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 3270
return({struct Cyc_Absyn_Exp*_tmp627=_cycalloc(sizeof(*_tmp627));_tmp627->topt=old->topt;_tmp627->r=r;_tmp627->loc=old->loc;_tmp627->annot=old->annot;_tmp627;});}
# 3275
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp628=e->r;void*_tmp629=_tmp628;void*_tmp671;void*_tmp670;struct Cyc_List_List*_tmp66F;struct Cyc_Absyn_Exp*_tmp66E;void*_tmp66D;void*_tmp66C;struct Cyc_Absyn_Exp*_tmp66B;int _tmp66A;enum Cyc_Absyn_Coercion _tmp669;struct Cyc_Absyn_Exp*_tmp668;struct Cyc_Absyn_Exp*_tmp667;struct Cyc_Absyn_Exp*_tmp666;struct Cyc_Absyn_Exp*_tmp665;struct Cyc_Absyn_Exp*_tmp664;struct Cyc_Absyn_Exp*_tmp663;struct Cyc_Absyn_Exp*_tmp662;struct Cyc_Absyn_Exp*_tmp661;struct Cyc_Absyn_Exp*_tmp660;enum Cyc_Absyn_Primop _tmp65F;struct Cyc_List_List*_tmp65E;switch(*((int*)_tmp629)){case 0: _LL5B7: _LL5B8:
 goto _LL5BA;case 31: _LL5B9: _LL5BA:
 goto _LL5BC;case 32: _LL5BB: _LL5BC:
 goto _LL5BE;case 1: _LL5BD: _LL5BE:
 return e;case 2: _LL5BF: _tmp65F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp629)->f1;_tmp65E=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp629)->f2;_LL5C0:
# 3283
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp65E)== 1){
struct Cyc_Absyn_Exp*_tmp62A=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp65E))->hd;
struct Cyc_Absyn_Exp*_tmp62B=Cyc_Tcutil_rsubsexp(r,inst,_tmp62A);
if(_tmp62B == _tmp62A)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp62C=_cycalloc(sizeof(*_tmp62C));_tmp62C[0]=({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp62D;_tmp62D.tag=2;_tmp62D.f1=_tmp65F;_tmp62D.f2=({struct Cyc_Absyn_Exp*_tmp62E[1];_tmp62E[0]=_tmp62B;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp62E,sizeof(struct Cyc_Absyn_Exp*),1));});_tmp62D;});_tmp62C;}));}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp65E)== 2){
struct Cyc_Absyn_Exp*_tmp62F=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp65E))->hd;
struct Cyc_Absyn_Exp*_tmp630=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp65E->tl))->hd;
struct Cyc_Absyn_Exp*_tmp631=Cyc_Tcutil_rsubsexp(r,inst,_tmp62F);
struct Cyc_Absyn_Exp*_tmp632=Cyc_Tcutil_rsubsexp(r,inst,_tmp630);
if(_tmp631 == _tmp62F  && _tmp632 == _tmp630)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp633=_cycalloc(sizeof(*_tmp633));_tmp633[0]=({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp634;_tmp634.tag=2;_tmp634.f1=_tmp65F;_tmp634.f2=({struct Cyc_Absyn_Exp*_tmp635[2];_tmp635[1]=_tmp632;_tmp635[0]=_tmp631;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp635,sizeof(struct Cyc_Absyn_Exp*),2));});_tmp634;});_tmp633;}));}else{
return({void*_tmp636=0;((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp637="primop does not have 1 or 2 args!";_tag_dyneither(_tmp637,sizeof(char),34);}),_tag_dyneither(_tmp636,sizeof(void*),0));});}}case 5: _LL5C1: _tmp662=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp629)->f1;_tmp661=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp629)->f2;_tmp660=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp629)->f3;_LL5C2: {
# 3297
struct Cyc_Absyn_Exp*_tmp638=Cyc_Tcutil_rsubsexp(r,inst,_tmp662);
struct Cyc_Absyn_Exp*_tmp639=Cyc_Tcutil_rsubsexp(r,inst,_tmp661);
struct Cyc_Absyn_Exp*_tmp63A=Cyc_Tcutil_rsubsexp(r,inst,_tmp660);
if((_tmp638 == _tmp662  && _tmp639 == _tmp661) && _tmp63A == _tmp660)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp63B=_cycalloc(sizeof(*_tmp63B));_tmp63B[0]=({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp63C;_tmp63C.tag=5;_tmp63C.f1=_tmp638;_tmp63C.f2=_tmp639;_tmp63C.f3=_tmp63A;_tmp63C;});_tmp63B;}));}case 6: _LL5C3: _tmp664=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp629)->f1;_tmp663=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp629)->f2;_LL5C4: {
# 3303
struct Cyc_Absyn_Exp*_tmp63D=Cyc_Tcutil_rsubsexp(r,inst,_tmp664);
struct Cyc_Absyn_Exp*_tmp63E=Cyc_Tcutil_rsubsexp(r,inst,_tmp663);
if(_tmp63D == _tmp664  && _tmp63E == _tmp663)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp63F=_cycalloc(sizeof(*_tmp63F));_tmp63F[0]=({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmp640;_tmp640.tag=6;_tmp640.f1=_tmp63D;_tmp640.f2=_tmp63E;_tmp640;});_tmp63F;}));}case 7: _LL5C5: _tmp666=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp629)->f1;_tmp665=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp629)->f2;_LL5C6: {
# 3308
struct Cyc_Absyn_Exp*_tmp641=Cyc_Tcutil_rsubsexp(r,inst,_tmp666);
struct Cyc_Absyn_Exp*_tmp642=Cyc_Tcutil_rsubsexp(r,inst,_tmp665);
if(_tmp641 == _tmp666  && _tmp642 == _tmp665)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp643=_cycalloc(sizeof(*_tmp643));_tmp643[0]=({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmp644;_tmp644.tag=7;_tmp644.f1=_tmp641;_tmp644.f2=_tmp642;_tmp644;});_tmp643;}));}case 8: _LL5C7: _tmp668=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp629)->f1;_tmp667=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp629)->f2;_LL5C8: {
# 3313
struct Cyc_Absyn_Exp*_tmp645=Cyc_Tcutil_rsubsexp(r,inst,_tmp668);
struct Cyc_Absyn_Exp*_tmp646=Cyc_Tcutil_rsubsexp(r,inst,_tmp667);
if(_tmp645 == _tmp668  && _tmp646 == _tmp667)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp647=_cycalloc(sizeof(*_tmp647));_tmp647[0]=({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmp648;_tmp648.tag=8;_tmp648.f1=_tmp645;_tmp648.f2=_tmp646;_tmp648;});_tmp647;}));}case 13: _LL5C9: _tmp66C=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp629)->f1;_tmp66B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp629)->f2;_tmp66A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp629)->f3;_tmp669=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp629)->f4;_LL5CA: {
# 3318
struct Cyc_Absyn_Exp*_tmp649=Cyc_Tcutil_rsubsexp(r,inst,_tmp66B);
void*_tmp64A=Cyc_Tcutil_rsubstitute(r,inst,_tmp66C);
if(_tmp649 == _tmp66B  && _tmp64A == _tmp66C)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp64B=_cycalloc(sizeof(*_tmp64B));_tmp64B[0]=({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp64C;_tmp64C.tag=13;_tmp64C.f1=_tmp64A;_tmp64C.f2=_tmp649;_tmp64C.f3=_tmp66A;_tmp64C.f4=_tmp669;_tmp64C;});_tmp64B;}));}case 16: _LL5CB: _tmp66D=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp629)->f1;_LL5CC: {
# 3323
void*_tmp64D=Cyc_Tcutil_rsubstitute(r,inst,_tmp66D);
if(_tmp64D == _tmp66D)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp64E=_cycalloc(sizeof(*_tmp64E));_tmp64E[0]=({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp64F;_tmp64F.tag=16;_tmp64F.f1=_tmp64D;_tmp64F;});_tmp64E;}));}case 17: _LL5CD: _tmp66E=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp629)->f1;_LL5CE: {
# 3327
struct Cyc_Absyn_Exp*_tmp650=Cyc_Tcutil_rsubsexp(r,inst,_tmp66E);
if(_tmp650 == _tmp66E)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp651=_cycalloc(sizeof(*_tmp651));_tmp651[0]=({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmp652;_tmp652.tag=17;_tmp652.f1=_tmp650;_tmp652;});_tmp651;}));}case 18: _LL5CF: _tmp670=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp629)->f1;_tmp66F=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp629)->f2;_LL5D0: {
# 3331
void*_tmp653=Cyc_Tcutil_rsubstitute(r,inst,_tmp670);
if(_tmp653 == _tmp670)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp654=_cycalloc(sizeof(*_tmp654));_tmp654[0]=({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp655;_tmp655.tag=18;_tmp655.f1=_tmp653;_tmp655.f2=_tmp66F;_tmp655;});_tmp654;}));}case 38: _LL5D1: _tmp671=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp629)->f1;_LL5D2: {
# 3335
void*_tmp656=Cyc_Tcutil_rsubstitute(r,inst,_tmp671);
if(_tmp656 == _tmp671)return e;{
# 3338
void*_tmp657=Cyc_Tcutil_compress(_tmp656);void*_tmp658=_tmp657;struct Cyc_Absyn_Exp*_tmp65B;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp658)->tag == 18){_LL5D6: _tmp65B=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp658)->f1;_LL5D7:
 return _tmp65B;}else{_LL5D8: _LL5D9:
# 3341
 return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp659=_cycalloc(sizeof(*_tmp659));_tmp659[0]=({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp65A;_tmp65A.tag=38;_tmp65A.f1=_tmp656;_tmp65A;});_tmp659;}));}_LL5D5:;};}default: _LL5D3: _LL5D4:
# 3344
 return({void*_tmp65C=0;((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp65D="non-type-level-expression in Tcutil::rsubsexp";_tag_dyneither(_tmp65D,sizeof(char),46);}),_tag_dyneither(_tmp65C,sizeof(void*),0));});}_LL5B6:;}
# 3348
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 3351
if(e == 0)return 0;else{
return Cyc_Tcutil_rsubsexp(r,inst,e);}}
# 3355
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 3358
void*_tmp672=f->type;
struct Cyc_Absyn_Exp*_tmp673=f->requires_clause;
void*_tmp674=Cyc_Tcutil_rsubstitute(r,inst,_tmp672);
struct Cyc_Absyn_Exp*_tmp675=Cyc_Tcutil_rsubs_exp_opt(r,inst,_tmp673);
if(_tmp672 == _tmp674  && _tmp673 == _tmp675)return f;else{
return({struct Cyc_Absyn_Aggrfield*_tmp676=_cycalloc(sizeof(*_tmp676));_tmp676->name=f->name;_tmp676->tq=f->tq;_tmp676->type=_tmp674;_tmp676->width=f->width;_tmp676->attributes=f->attributes;_tmp676->requires_clause=_tmp675;_tmp676;});}}
# 3368
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 3371
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp677=(struct Cyc_Absyn_Aggrfield*)fs->hd;
struct Cyc_List_List*_tmp678=fs->tl;
struct Cyc_Absyn_Aggrfield*_tmp679=Cyc_Tcutil_subst_aggrfield(r,inst,_tmp677);
struct Cyc_List_List*_tmp67A=Cyc_Tcutil_subst_aggrfields(r,inst,_tmp678);
if(_tmp679 == _tmp677  && _tmp67A == _tmp678)return fs;
# 3378
return({struct Cyc_List_List*_tmp67B=_cycalloc(sizeof(*_tmp67B));_tmp67B->hd=_tmp679;_tmp67B->tl=_tmp67A;_tmp67B;});};}
# 3381
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 3384
struct _tuple1 _tmp67C=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct _tuple1 _tmp67D=_tmp67C;struct Cyc_List_List*_tmp681;struct Cyc_List_List*_tmp680;_LL5DB: _tmp681=_tmp67D.f1;_tmp680=_tmp67D.f2;_LL5DC:;{
struct Cyc_List_List*_tmp67E=Cyc_Tcutil_substs(rgn,inst,_tmp681);
struct Cyc_List_List*_tmp67F=Cyc_Tcutil_substs(rgn,inst,_tmp680);
if(_tmp67E == _tmp681  && _tmp67F == _tmp680)
return rgn_po;else{
# 3390
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp67E,_tmp67F);}};}
# 3393
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3396
void*_tmp682=Cyc_Tcutil_compress(t);void*_tmp683=_tmp682;struct Cyc_List_List*_tmp714;void*_tmp713;void*_tmp712;struct Cyc_Absyn_Exp*_tmp711;struct Cyc_Absyn_Exp*_tmp710;void*_tmp70F;void*_tmp70E;void*_tmp70D;void*_tmp70C;void*_tmp70B;enum Cyc_Absyn_AggrKind _tmp70A;struct Cyc_List_List*_tmp709;struct Cyc_List_List*_tmp708;struct Cyc_List_List*_tmp707;void*_tmp706;struct Cyc_Absyn_Tqual _tmp705;void*_tmp704;struct Cyc_List_List*_tmp703;int _tmp702;struct Cyc_Absyn_VarargInfo*_tmp701;struct Cyc_List_List*_tmp700;struct Cyc_List_List*_tmp6FF;struct Cyc_Absyn_Exp*_tmp6FE;struct Cyc_Absyn_Exp*_tmp6FD;void*_tmp6FC;struct Cyc_Absyn_Tqual _tmp6FB;void*_tmp6FA;union Cyc_Absyn_Constraint*_tmp6F9;union Cyc_Absyn_Constraint*_tmp6F8;union Cyc_Absyn_Constraint*_tmp6F7;void*_tmp6F6;struct Cyc_Absyn_Tqual _tmp6F5;struct Cyc_Absyn_Exp*_tmp6F4;union Cyc_Absyn_Constraint*_tmp6F3;unsigned int _tmp6F2;struct _tuple2*_tmp6F1;struct Cyc_List_List*_tmp6F0;struct Cyc_Absyn_Typedefdecl*_tmp6EF;void*_tmp6EE;union Cyc_Absyn_DatatypeFieldInfoU _tmp6ED;struct Cyc_List_List*_tmp6EC;union Cyc_Absyn_DatatypeInfoU _tmp6EB;struct Cyc_List_List*_tmp6EA;union Cyc_Absyn_AggrInfoU _tmp6E9;struct Cyc_List_List*_tmp6E8;struct Cyc_Absyn_Tvar*_tmp6E7;switch(*((int*)_tmp683)){case 2: _LL5DE: _tmp6E7=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp683)->f1;_LL5DF: {
# 3399
struct _handler_cons _tmp684;_push_handler(& _tmp684);{int _tmp686=0;if(setjmp(_tmp684.handler))_tmp686=1;if(!_tmp686){{void*_tmp687=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp6E7);_npop_handler(0);return _tmp687;};_pop_handler();}else{void*_tmp685=(void*)_exn_thrown;void*_tmp688=_tmp685;void*_tmp689;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp688)->tag == Cyc_Core_Not_found){_LL619: _LL61A:
 return t;}else{_LL61B: _tmp689=_tmp688;_LL61C:(int)_rethrow(_tmp689);}_LL618:;}};}case 11: _LL5E0: _tmp6E9=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp683)->f1).aggr_info;_tmp6E8=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp683)->f1).targs;_LL5E1: {
# 3402
struct Cyc_List_List*_tmp68A=Cyc_Tcutil_substs(rgn,inst,_tmp6E8);
return _tmp68A == _tmp6E8?t:(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp68B=_cycalloc(sizeof(*_tmp68B));_tmp68B[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp68C;_tmp68C.tag=11;_tmp68C.f1=({struct Cyc_Absyn_AggrInfo _tmp68D;_tmp68D.aggr_info=_tmp6E9;_tmp68D.targs=_tmp68A;_tmp68D;});_tmp68C;});_tmp68B;});}case 3: _LL5E2: _tmp6EB=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp683)->f1).datatype_info;_tmp6EA=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp683)->f1).targs;_LL5E3: {
# 3405
struct Cyc_List_List*_tmp68E=Cyc_Tcutil_substs(rgn,inst,_tmp6EA);
return _tmp68E == _tmp6EA?t:(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp68F=_cycalloc(sizeof(*_tmp68F));_tmp68F[0]=({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp690;_tmp690.tag=3;_tmp690.f1=({struct Cyc_Absyn_DatatypeInfo _tmp691;_tmp691.datatype_info=_tmp6EB;_tmp691.targs=_tmp68E;_tmp691;});_tmp690;});_tmp68F;});}case 4: _LL5E4: _tmp6ED=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp683)->f1).field_info;_tmp6EC=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp683)->f1).targs;_LL5E5: {
# 3409
struct Cyc_List_List*_tmp692=Cyc_Tcutil_substs(rgn,inst,_tmp6EC);
return _tmp692 == _tmp6EC?t:(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp693=_cycalloc(sizeof(*_tmp693));_tmp693[0]=({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp694;_tmp694.tag=4;_tmp694.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmp695;_tmp695.field_info=_tmp6ED;_tmp695.targs=_tmp692;_tmp695;});_tmp694;});_tmp693;});}case 17: _LL5E6: _tmp6F1=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp683)->f1;_tmp6F0=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp683)->f2;_tmp6EF=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp683)->f3;_tmp6EE=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp683)->f4;_LL5E7: {
# 3413
struct Cyc_List_List*_tmp696=Cyc_Tcutil_substs(rgn,inst,_tmp6F0);
return _tmp696 == _tmp6F0?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp697=_cycalloc(sizeof(*_tmp697));_tmp697[0]=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp698;_tmp698.tag=17;_tmp698.f1=_tmp6F1;_tmp698.f2=_tmp696;_tmp698.f3=_tmp6EF;_tmp698.f4=_tmp6EE;_tmp698;});_tmp697;});}case 8: _LL5E8: _tmp6F6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp683)->f1).elt_type;_tmp6F5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp683)->f1).tq;_tmp6F4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp683)->f1).num_elts;_tmp6F3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp683)->f1).zero_term;_tmp6F2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp683)->f1).zt_loc;_LL5E9: {
# 3416
void*_tmp699=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6F6);
struct Cyc_Absyn_Exp*_tmp69A=_tmp6F4 == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,_tmp6F4);
return _tmp699 == _tmp6F6  && _tmp69A == _tmp6F4?t:(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp69B=_cycalloc(sizeof(*_tmp69B));_tmp69B[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp69C;_tmp69C.tag=8;_tmp69C.f1=({struct Cyc_Absyn_ArrayInfo _tmp69D;_tmp69D.elt_type=_tmp699;_tmp69D.tq=_tmp6F5;_tmp69D.num_elts=_tmp69A;_tmp69D.zero_term=_tmp6F3;_tmp69D.zt_loc=_tmp6F2;_tmp69D;});_tmp69C;});_tmp69B;});}case 5: _LL5EA: _tmp6FC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp683)->f1).elt_typ;_tmp6FB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp683)->f1).elt_tq;_tmp6FA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp683)->f1).ptr_atts).rgn;_tmp6F9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp683)->f1).ptr_atts).nullable;_tmp6F8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp683)->f1).ptr_atts).bounds;_tmp6F7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp683)->f1).ptr_atts).zero_term;_LL5EB: {
# 3421
void*_tmp69E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6FC);
void*_tmp69F=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6FA);
union Cyc_Absyn_Constraint*_tmp6A0=_tmp6F8;
{void*_tmp6A1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp6F8);void*_tmp6A2=_tmp6A1;struct Cyc_Absyn_Exp*_tmp6A6;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6A2)->tag == 1){_LL61E: _tmp6A6=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6A2)->f1;_LL61F: {
# 3426
struct Cyc_Absyn_Exp*_tmp6A3=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp6A6);
if(_tmp6A3 != _tmp6A6)
_tmp6A0=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6A4=_cycalloc(sizeof(*_tmp6A4));_tmp6A4[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp6A5;_tmp6A5.tag=1;_tmp6A5.f1=_tmp6A3;_tmp6A5;});_tmp6A4;}));
goto _LL61D;}}else{_LL620: _LL621:
 goto _LL61D;}_LL61D:;}
# 3432
if((_tmp69E == _tmp6FC  && _tmp69F == _tmp6FA) && _tmp6A0 == _tmp6F8)
return t;
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6A7=_cycalloc(sizeof(*_tmp6A7));_tmp6A7[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp6A8;_tmp6A8.tag=5;_tmp6A8.f1=({struct Cyc_Absyn_PtrInfo _tmp6A9;_tmp6A9.elt_typ=_tmp69E;_tmp6A9.elt_tq=_tmp6FB;_tmp6A9.ptr_atts=({(_tmp6A9.ptr_atts).rgn=_tmp69F;(_tmp6A9.ptr_atts).nullable=_tmp6F9;(_tmp6A9.ptr_atts).bounds=_tmp6A0;(_tmp6A9.ptr_atts).zero_term=_tmp6F7;(_tmp6A9.ptr_atts).ptrloc=0;_tmp6A9.ptr_atts;});_tmp6A9;});_tmp6A8;});_tmp6A7;});}case 9: _LL5EC: _tmp707=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp683)->f1).tvars;_tmp706=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp683)->f1).effect;_tmp705=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp683)->f1).ret_tqual;_tmp704=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp683)->f1).ret_typ;_tmp703=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp683)->f1).args;_tmp702=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp683)->f1).c_varargs;_tmp701=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp683)->f1).cyc_varargs;_tmp700=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp683)->f1).rgn_po;_tmp6FF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp683)->f1).attributes;_tmp6FE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp683)->f1).requires_clause;_tmp6FD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp683)->f1).ensures_clause;_LL5ED:
# 3438
{struct Cyc_List_List*_tmp6AA=_tmp707;for(0;_tmp6AA != 0;_tmp6AA=_tmp6AA->tl){
inst=({struct Cyc_List_List*_tmp6AB=_region_malloc(rgn,sizeof(*_tmp6AB));_tmp6AB->hd=({struct _tuple16*_tmp6AC=_region_malloc(rgn,sizeof(*_tmp6AC));_tmp6AC->f1=(struct Cyc_Absyn_Tvar*)_tmp6AA->hd;_tmp6AC->f2=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp6AD=_cycalloc(sizeof(*_tmp6AD));_tmp6AD[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp6AE;_tmp6AE.tag=2;_tmp6AE.f1=(struct Cyc_Absyn_Tvar*)_tmp6AA->hd;_tmp6AE;});_tmp6AD;});_tmp6AC;});_tmp6AB->tl=inst;_tmp6AB;});}}{
struct _tuple1 _tmp6AF=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp703));
# 3440
struct _tuple1 _tmp6B0=_tmp6AF;struct Cyc_List_List*_tmp6C1;struct Cyc_List_List*_tmp6C0;_LL623: _tmp6C1=_tmp6B0.f1;_tmp6C0=_tmp6B0.f2;_LL624:;{
# 3442
struct Cyc_List_List*_tmp6B1=_tmp703;
struct Cyc_List_List*_tmp6B2=Cyc_Tcutil_substs(rgn,inst,_tmp6C0);
if(_tmp6B2 != _tmp6C0)
_tmp6B1=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmp703,_tmp6B2);{
void*eff2;
if(_tmp706 == 0)
eff2=0;else{
# 3450
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp706);
if(new_eff == _tmp706)
eff2=_tmp706;else{
# 3454
eff2=new_eff;}}{
# 3456
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(_tmp701 == 0)
cyc_varargs2=0;else{
# 3460
struct Cyc_Absyn_VarargInfo _tmp6B3=*_tmp701;struct Cyc_Absyn_VarargInfo _tmp6B4=_tmp6B3;struct _dyneither_ptr*_tmp6BA;struct Cyc_Absyn_Tqual _tmp6B9;void*_tmp6B8;int _tmp6B7;_LL626: _tmp6BA=_tmp6B4.name;_tmp6B9=_tmp6B4.tq;_tmp6B8=_tmp6B4.type;_tmp6B7=_tmp6B4.inject;_LL627:;{
void*_tmp6B5=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6B8);
if(_tmp6B5 == _tmp6B8)cyc_varargs2=_tmp701;else{
# 3464
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp6B6=_cycalloc(sizeof(*_tmp6B6));_tmp6B6->name=_tmp6BA;_tmp6B6->tq=_tmp6B9;_tmp6B6->type=_tmp6B5;_tmp6B6->inject=_tmp6B7;_tmp6B6;});}};}{
# 3466
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp700);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp6FE);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp6FD);
struct Cyc_List_List*_tmp6BB=Cyc_Tcutil_extract_relns(_tmp6B1,req2);
struct Cyc_List_List*_tmp6BC=Cyc_Tcutil_extract_relns(_tmp6B1,ens2);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp6BD=_cycalloc(sizeof(*_tmp6BD));_tmp6BD[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp6BE;_tmp6BE.tag=9;_tmp6BE.f1=({struct Cyc_Absyn_FnInfo _tmp6BF;_tmp6BF.tvars=_tmp707;_tmp6BF.effect=eff2;_tmp6BF.ret_tqual=_tmp705;_tmp6BF.ret_typ=
Cyc_Tcutil_rsubstitute(rgn,inst,_tmp704);_tmp6BF.args=_tmp6B1;_tmp6BF.c_varargs=_tmp702;_tmp6BF.cyc_varargs=cyc_varargs2;_tmp6BF.rgn_po=rgn_po2;_tmp6BF.attributes=_tmp6FF;_tmp6BF.requires_clause=req2;_tmp6BF.requires_relns=_tmp6BB;_tmp6BF.ensures_clause=ens2;_tmp6BF.ensures_relns=_tmp6BC;_tmp6BF;});_tmp6BE;});_tmp6BD;});};};};};};case 10: _LL5EE: _tmp708=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp683)->f1;_LL5EF: {
# 3476
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp6C2=_tmp708;for(0;_tmp6C2 != 0;_tmp6C2=_tmp6C2->tl){
void*_tmp6C3=(*((struct _tuple12*)_tmp6C2->hd)).f2;
void*_tmp6C4=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6C3);
if(_tmp6C3 != _tmp6C4)
change=1;
# 3484
ts2=({struct Cyc_List_List*_tmp6C5=_region_malloc(rgn,sizeof(*_tmp6C5));_tmp6C5->hd=_tmp6C4;_tmp6C5->tl=ts2;_tmp6C5;});}}
# 3486
if(!change)
return t;{
struct Cyc_List_List*_tmp6C6=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp708,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6C7=_cycalloc(sizeof(*_tmp6C7));_tmp6C7[0]=({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp6C8;_tmp6C8.tag=10;_tmp6C8.f1=_tmp6C6;_tmp6C8;});_tmp6C7;});};}case 12: _LL5F0: _tmp70A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp683)->f1;_tmp709=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp683)->f2;_LL5F1: {
# 3491
struct Cyc_List_List*_tmp6C9=Cyc_Tcutil_subst_aggrfields(rgn,inst,_tmp709);
if(_tmp709 == _tmp6C9)return t;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp6CA=_cycalloc(sizeof(*_tmp6CA));_tmp6CA[0]=({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp6CB;_tmp6CB.tag=12;_tmp6CB.f1=_tmp70A;_tmp6CB.f2=_tmp6C9;_tmp6CB;});_tmp6CA;});}case 1: _LL5F2: _tmp70B=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp683)->f2;_LL5F3:
# 3495
 if(_tmp70B != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp70B);else{
return t;}case 15: _LL5F4: _tmp70C=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp683)->f1;_LL5F5: {
# 3498
void*_tmp6CC=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp70C);
return _tmp6CC == _tmp70C?t:(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp6CD=_cycalloc(sizeof(*_tmp6CD));_tmp6CD[0]=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp6CE;_tmp6CE.tag=15;_tmp6CE.f1=_tmp6CC;_tmp6CE;});_tmp6CD;});}case 16: _LL5F6: _tmp70E=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp683)->f1;_tmp70D=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp683)->f2;_LL5F7: {
# 3501
void*_tmp6CF=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp70E);
void*_tmp6D0=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp70D);
return _tmp6CF == _tmp70E  && _tmp6D0 == _tmp70D?t:(void*)({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp6D1=_cycalloc(sizeof(*_tmp6D1));_tmp6D1[0]=({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp6D2;_tmp6D2.tag=16;_tmp6D2.f1=_tmp6CF;_tmp6D2.f2=_tmp6D0;_tmp6D2;});_tmp6D1;});}case 19: _LL5F8: _tmp70F=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp683)->f1;_LL5F9: {
# 3505
void*_tmp6D3=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp70F);
return _tmp6D3 == _tmp70F?t:(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp6D4=_cycalloc(sizeof(*_tmp6D4));_tmp6D4[0]=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp6D5;_tmp6D5.tag=19;_tmp6D5.f1=_tmp6D3;_tmp6D5;});_tmp6D4;});}case 18: _LL5FA: _tmp710=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp683)->f1;_LL5FB: {
# 3508
struct Cyc_Absyn_Exp*_tmp6D6=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp710);
return _tmp6D6 == _tmp710?t:(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp6D7=_cycalloc(sizeof(*_tmp6D7));_tmp6D7[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp6D8;_tmp6D8.tag=18;_tmp6D8.f1=_tmp6D6;_tmp6D8;});_tmp6D7;});}case 27: _LL5FC: _tmp711=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp683)->f1;_LL5FD: {
# 3511
struct Cyc_Absyn_Exp*_tmp6D9=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp711);
return _tmp6D9 == _tmp711?t:(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp6DA=_cycalloc(sizeof(*_tmp6DA));_tmp6DA[0]=({struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmp6DB;_tmp6DB.tag=27;_tmp6DB.f1=_tmp6D9;_tmp6DB;});_tmp6DA;});}case 13: _LL5FE: _LL5FF:
 goto _LL601;case 14: _LL600: _LL601:
 goto _LL603;case 0: _LL602: _LL603:
 goto _LL605;case 6: _LL604: _LL605:
 goto _LL607;case 7: _LL606: _LL607:
 goto _LL609;case 22: _LL608: _LL609:
 goto _LL60B;case 21: _LL60A: _LL60B:
 goto _LL60D;case 28: _LL60C: _LL60D:
 goto _LL60F;case 20: _LL60E: _LL60F:
 return t;case 25: _LL610: _tmp712=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp683)->f1;_LL611: {
# 3523
void*_tmp6DC=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp712);
return _tmp6DC == _tmp712?t:(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp6DD=_cycalloc(sizeof(*_tmp6DD));_tmp6DD[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp6DE;_tmp6DE.tag=25;_tmp6DE.f1=_tmp6DC;_tmp6DE;});_tmp6DD;});}case 23: _LL612: _tmp713=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp683)->f1;_LL613: {
# 3526
void*_tmp6DF=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp713);
return _tmp6DF == _tmp713?t:(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp6E0=_cycalloc(sizeof(*_tmp6E0));_tmp6E0[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp6E1;_tmp6E1.tag=23;_tmp6E1.f1=_tmp6DF;_tmp6E1;});_tmp6E0;});}case 24: _LL614: _tmp714=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp683)->f1;_LL615: {
# 3529
struct Cyc_List_List*_tmp6E2=Cyc_Tcutil_substs(rgn,inst,_tmp714);
return _tmp6E2 == _tmp714?t:(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp6E3=_cycalloc(sizeof(*_tmp6E3));_tmp6E3[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp6E4;_tmp6E4.tag=24;_tmp6E4.f1=_tmp6E2;_tmp6E4;});_tmp6E3;});}default: _LL616: _LL617:
({void*_tmp6E5=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp6E6="found typedecltype in rsubs";_tag_dyneither(_tmp6E6,sizeof(char),28);}),_tag_dyneither(_tmp6E5,sizeof(void*),0));});}_LL5DD:;}
# 3535
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 3538
if(ts == 0)
return 0;{
void*_tmp715=(void*)ts->hd;
struct Cyc_List_List*_tmp716=ts->tl;
void*_tmp717=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp715);
struct Cyc_List_List*_tmp718=Cyc_Tcutil_substs(rgn,inst,_tmp716);
if(_tmp715 == _tmp717  && _tmp716 == _tmp718)
return ts;
return({struct Cyc_List_List*_tmp719=_cycalloc(sizeof(*_tmp719));_tmp719->hd=_tmp717;_tmp719->tl=_tmp718;_tmp719;});};}
# 3549
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 3556
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp71A=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple16*_tmp71B=_cycalloc(sizeof(*_tmp71B));_tmp71B->f1=tv;_tmp71B->f2=Cyc_Absyn_new_evar(_tmp71A,({struct Cyc_Core_Opt*_tmp71C=_cycalloc(sizeof(*_tmp71C));_tmp71C->v=s;_tmp71C;}));_tmp71B;});}
# 3561
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*env,struct Cyc_Absyn_Tvar*tv){
# 3563
struct _tuple17*_tmp71D=env;struct Cyc_List_List*_tmp722;struct _RegionHandle*_tmp721;_LL629: _tmp722=_tmp71D->f1;_tmp721=_tmp71D->f2;_LL62A:;{
struct Cyc_Core_Opt*_tmp71E=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple16*_tmp71F=_region_malloc(_tmp721,sizeof(*_tmp71F));_tmp71F->f1=tv;_tmp71F->f2=Cyc_Absyn_new_evar(_tmp71E,({struct Cyc_Core_Opt*_tmp720=_cycalloc(sizeof(*_tmp720));_tmp720->v=_tmp722;_tmp720;}));_tmp71F;});};}
# 3573
static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(unsigned int loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3577
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2))
({struct Cyc_String_pa_PrintArg_struct _tmp727;_tmp727.tag=0;_tmp727.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(k2));({struct Cyc_String_pa_PrintArg_struct _tmp726;_tmp726.tag=0;_tmp726.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1));({struct Cyc_String_pa_PrintArg_struct _tmp725;_tmp725.tag=0;_tmp725.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name);({void*_tmp723[3]={& _tmp725,& _tmp726,& _tmp727};Cyc_Tcutil_terr(loc,({const char*_tmp724="type variable %s is used with inconsistent kinds %s and %s";_tag_dyneither(_tmp724,sizeof(char),59);}),_tag_dyneither(_tmp723,sizeof(void*),3));});});});});
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)
({void*_tmp728=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp729="same type variable has different identity!";_tag_dyneither(_tmp729,sizeof(char),43);}),_tag_dyneither(_tmp728,sizeof(void*),0));});}
return tvs;}}}
# 3591
tv->identity=Cyc_Tcutil_new_tvar_id();
return({struct Cyc_List_List*_tmp72A=_cycalloc(sizeof(*_tmp72A));_tmp72A->hd=tv;_tmp72A->tl=tvs;_tmp72A;});}
# 3597
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3599
if(tv->identity == - 1)
({void*_tmp72B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp72C="fast_add_free_tvar: bad identity in tv";_tag_dyneither(_tmp72C,sizeof(char),39);}),_tag_dyneither(_tmp72B,sizeof(void*),0));});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3603
struct Cyc_Absyn_Tvar*_tmp72D=(struct Cyc_Absyn_Tvar*)tvs2->hd;
if(_tmp72D->identity == - 1)
({void*_tmp72E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp72F="fast_add_free_tvar: bad identity in tvs2";_tag_dyneither(_tmp72F,sizeof(char),41);}),_tag_dyneither(_tmp72E,sizeof(void*),0));});
if(_tmp72D->identity == tv->identity)
return tvs;}}
# 3610
return({struct Cyc_List_List*_tmp730=_cycalloc(sizeof(*_tmp730));_tmp730->hd=tv;_tmp730->tl=tvs;_tmp730;});}struct _tuple28{struct Cyc_Absyn_Tvar*f1;int f2;};
# 3616
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 3621
if(tv->identity == - 1)
({void*_tmp731=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp732="fast_add_free_tvar_bool: bad identity in tv";_tag_dyneither(_tmp732,sizeof(char),44);}),_tag_dyneither(_tmp731,sizeof(void*),0));});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3625
struct _tuple28*_tmp733=(struct _tuple28*)tvs2->hd;struct _tuple28*_tmp734=_tmp733;struct Cyc_Absyn_Tvar*_tmp738;int*_tmp737;_LL62C: _tmp738=_tmp734->f1;_tmp737=(int*)& _tmp734->f2;_LL62D:;
if(_tmp738->identity == - 1)
({void*_tmp735=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp736="fast_add_free_tvar_bool: bad identity in tvs2";_tag_dyneither(_tmp736,sizeof(char),46);}),_tag_dyneither(_tmp735,sizeof(void*),0));});
if(_tmp738->identity == tv->identity){
*_tmp737=*_tmp737  || b;
return tvs;}}}
# 3633
return({struct Cyc_List_List*_tmp739=_region_malloc(r,sizeof(*_tmp739));_tmp739->hd=({struct _tuple28*_tmp73A=_region_malloc(r,sizeof(*_tmp73A));_tmp73A->f1=tv;_tmp73A->f2=b;_tmp73A;});_tmp739->tl=tvs;_tmp739;});}
# 3637
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3639
if(tv->identity == - 1)
({struct Cyc_String_pa_PrintArg_struct _tmp73D;_tmp73D.tag=0;_tmp73D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(tv));({void*_tmp73B[1]={& _tmp73D};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp73C="bound tvar id for %s is NULL";_tag_dyneither(_tmp73C,sizeof(char),29);}),_tag_dyneither(_tmp73B,sizeof(void*),1));});});
return({struct Cyc_List_List*_tmp73E=_cycalloc(sizeof(*_tmp73E));_tmp73E->hd=tv;_tmp73E->tl=tvs;_tmp73E;});}struct _tuple29{void*f1;int f2;};
# 3648
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 3651
void*_tmp73F=Cyc_Tcutil_compress(e);void*_tmp740=_tmp73F;int _tmp74A;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp740)->tag == 1){_LL62F: _tmp74A=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp740)->f3;_LL630:
# 3653
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple29*_tmp741=(struct _tuple29*)es2->hd;struct _tuple29*_tmp742=_tmp741;void*_tmp747;int*_tmp746;_LL634: _tmp747=_tmp742->f1;_tmp746=(int*)& _tmp742->f2;_LL635:;{
void*_tmp743=Cyc_Tcutil_compress(_tmp747);void*_tmp744=_tmp743;int _tmp745;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp744)->tag == 1){_LL637: _tmp745=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp744)->f3;_LL638:
# 3657
 if(_tmp74A == _tmp745){
if(b != *_tmp746)*_tmp746=1;
return es;}
# 3661
goto _LL636;}else{_LL639: _LL63A:
 goto _LL636;}_LL636:;};}}
# 3665
return({struct Cyc_List_List*_tmp748=_region_malloc(r,sizeof(*_tmp748));_tmp748->hd=({struct _tuple29*_tmp749=_region_malloc(r,sizeof(*_tmp749));_tmp749->f1=e;_tmp749->f2=b;_tmp749;});_tmp748->tl=es;_tmp748;});}else{_LL631: _LL632:
 return es;}_LL62E:;}
# 3670
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3673
struct Cyc_List_List*r=0;
for(0;tvs != 0;tvs=tvs->tl){
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3682
if(!present)r=({struct Cyc_List_List*_tmp74B=_region_malloc(rgn,sizeof(*_tmp74B));_tmp74B->hd=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp74B->tl=r;_tmp74B;});}
# 3684
r=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);
return r;}
# 3689
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3693
struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp74C=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp74D=_tmp74C;struct Cyc_Absyn_Tvar*_tmp750;int _tmp74F;_LL63C: _tmp750=_tmp74D.f1;_tmp74F=_tmp74D.f2;_LL63D:;{
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp750->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3703
if(!present)res=({struct Cyc_List_List*_tmp74E=_region_malloc(r,sizeof(*_tmp74E));_tmp74E->hd=(struct _tuple28*)tvs->hd;_tmp74E->tl=res;_tmp74E;});};}
# 3705
res=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);
return res;}
# 3709
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){
# 3711
if(width != 0){
unsigned int w=0;
if(!Cyc_Tcutil_is_const_exp(width))
({struct Cyc_String_pa_PrintArg_struct _tmp753;_tmp753.tag=0;_tmp753.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);({void*_tmp751[1]={& _tmp753};Cyc_Tcutil_terr(loc,({const char*_tmp752="bitfield %s does not have constant width";_tag_dyneither(_tmp752,sizeof(char),41);}),_tag_dyneither(_tmp751,sizeof(void*),1));});});else{
# 3716
struct _tuple14 _tmp754=Cyc_Evexp_eval_const_uint_exp(width);struct _tuple14 _tmp755=_tmp754;unsigned int _tmp759;int _tmp758;_LL63F: _tmp759=_tmp755.f1;_tmp758=_tmp755.f2;_LL640:;
if(!_tmp758)
({void*_tmp756=0;Cyc_Tcutil_terr(loc,({const char*_tmp757="bitfield width cannot use sizeof or offsetof";_tag_dyneither(_tmp757,sizeof(char),45);}),_tag_dyneither(_tmp756,sizeof(void*),0));});
w=_tmp759;}{
# 3721
void*_tmp75A=Cyc_Tcutil_compress(field_typ);void*_tmp75B=_tmp75A;enum Cyc_Absyn_Size_of _tmp769;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp75B)->tag == 6){_LL642: _tmp769=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp75B)->f2;_LL643:
# 3724
{enum Cyc_Absyn_Size_of _tmp75C=_tmp769;switch(_tmp75C){case Cyc_Absyn_Char_sz: _LL647: _LL648:
 if(w > 8)({void*_tmp75D=0;Cyc_Tcutil_terr(loc,({const char*_tmp75E="bitfield larger than type";_tag_dyneither(_tmp75E,sizeof(char),26);}),_tag_dyneither(_tmp75D,sizeof(void*),0));});goto _LL646;case Cyc_Absyn_Short_sz: _LL649: _LL64A:
 if(w > 16)({void*_tmp75F=0;Cyc_Tcutil_terr(loc,({const char*_tmp760="bitfield larger than type";_tag_dyneither(_tmp760,sizeof(char),26);}),_tag_dyneither(_tmp75F,sizeof(void*),0));});goto _LL646;case Cyc_Absyn_Long_sz: _LL64B: _LL64C:
 goto _LL64E;case Cyc_Absyn_Int_sz: _LL64D: _LL64E:
# 3729
 if(w > 32)({void*_tmp761=0;Cyc_Tcutil_terr(loc,({const char*_tmp762="bitfield larger than type";_tag_dyneither(_tmp762,sizeof(char),26);}),_tag_dyneither(_tmp761,sizeof(void*),0));});goto _LL646;default: _LL64F: _LL650:
# 3731
 if(w > 64)({void*_tmp763=0;Cyc_Tcutil_terr(loc,({const char*_tmp764="bitfield larger than type";_tag_dyneither(_tmp764,sizeof(char),26);}),_tag_dyneither(_tmp763,sizeof(void*),0));});goto _LL646;}_LL646:;}
# 3733
goto _LL641;}else{_LL644: _LL645:
# 3735
({struct Cyc_String_pa_PrintArg_struct _tmp768;_tmp768.tag=0;_tmp768.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(field_typ));({struct Cyc_String_pa_PrintArg_struct _tmp767;_tmp767.tag=0;_tmp767.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);({void*_tmp765[2]={& _tmp767,& _tmp768};Cyc_Tcutil_terr(loc,({const char*_tmp766="bitfield %s must have integral type but has type %s";_tag_dyneither(_tmp766,sizeof(char),52);}),_tag_dyneither(_tmp765,sizeof(void*),2));});});});
goto _LL641;}_LL641:;};}}
# 3742
static void Cyc_Tcutil_check_field_atts(unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp76A=(void*)atts->hd;void*_tmp76B=_tmp76A;switch(*((int*)_tmp76B)){case 7: _LL652: _LL653:
 continue;case 6: _LL654: _LL655:
 continue;default: _LL656: _LL657:
({struct Cyc_String_pa_PrintArg_struct _tmp76F;_tmp76F.tag=0;_tmp76F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);({struct Cyc_String_pa_PrintArg_struct _tmp76E;_tmp76E.tag=0;_tmp76E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd));({void*_tmp76C[2]={& _tmp76E,& _tmp76F};Cyc_Tcutil_terr(loc,({const char*_tmp76D="bad attribute %s on member %s";_tag_dyneither(_tmp76D,sizeof(char),30);}),_tag_dyneither(_tmp76C,sizeof(void*),2));});});});}_LL651:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
# 3766
typedef struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_cvtenv_t;
# 3770
int Cyc_Tcutil_extract_const_from_typedef(unsigned int loc,int declared_const,void*t){
void*_tmp770=t;struct Cyc_Absyn_Typedefdecl*_tmp774;void*_tmp773;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp770)->tag == 17){_LL659: _tmp774=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp770)->f3;_tmp773=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp770)->f4;_LL65A:
# 3773
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp774))->tq).real_const  || (_tmp774->tq).print_const){
if(declared_const)({void*_tmp771=0;Cyc_Tcutil_warn(loc,({const char*_tmp772="extra const";_tag_dyneither(_tmp772,sizeof(char),12);}),_tag_dyneither(_tmp771,sizeof(void*),0));});
return 1;}
# 3778
if((unsigned int)_tmp773)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp773);else{
return declared_const;}}else{_LL65B: _LL65C:
 return declared_const;}_LL658:;}
# 3785
static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td != 0){
if(td->defn != 0){
void*_tmp775=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_tmp776=_tmp775;void*_tmp77A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp776)->tag == 5){_LL65E: _tmp77A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp776)->f1).ptr_atts).rgn;_LL65F:
# 3790
{void*_tmp777=Cyc_Tcutil_compress(_tmp77A);void*_tmp778=_tmp777;struct Cyc_Absyn_Tvar*_tmp779;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp778)->tag == 2){_LL663: _tmp779=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp778)->f1;_LL664:
# 3792
 return Cyc_Absyn_tvar_cmp(tvar,_tmp779)== 0;}else{_LL665: _LL666:
 goto _LL662;}_LL662:;}
# 3795
goto _LL65D;}else{_LL660: _LL661:
 goto _LL65D;}_LL65D:;}}else{
# 3801
return 1;}
return 0;}
# 3805
static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 3808
void*_tmp77B=Cyc_Absyn_compress_kb(tvar->kind);void*_tmp77C=_tmp77B;switch(*((int*)_tmp77C)){case 2: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp77C)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp77C)->f2)->aliasqual == Cyc_Absyn_Top){_LL668: _LL669:
 goto _LL66B;}else{goto _LL66C;}}else{goto _LL66C;}case 0: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp77C)->f1)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp77C)->f1)->aliasqual == Cyc_Absyn_Top){_LL66A: _LL66B:
# 3817
 if(((expected_kind->kind == Cyc_Absyn_BoxKind  || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind) && 
# 3820
Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){
if(expected_kind->aliasqual == Cyc_Absyn_Aliasable)
return& Cyc_Tcutil_rk;else{
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}
# 3826
return& Cyc_Tcutil_trk;}else{goto _LL66C;}}else{goto _LL66C;}default: _LL66C: _LL66D:
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);}_LL667:;}
# 3832
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);struct _tuple30{struct Cyc_Tcutil_CVTEnv f1;struct Cyc_List_List*f2;};
# 3836
static struct _tuple30 Cyc_Tcutil_check_clause(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct _dyneither_ptr clause_name,struct Cyc_Absyn_Exp*clause){
# 3839
struct Cyc_List_List*relns=0;
if(clause != 0){
Cyc_Tcexp_tcExp(te,0,clause);
if(!Cyc_Tcutil_is_integral(clause))
({struct Cyc_String_pa_PrintArg_struct _tmp780;_tmp780.tag=0;_tmp780.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(clause->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp77F;_tmp77F.tag=0;_tmp77F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name);({void*_tmp77D[2]={& _tmp77F,& _tmp780};Cyc_Tcutil_terr(loc,({const char*_tmp77E="%s clause has type %s instead of integral type";_tag_dyneither(_tmp77E,sizeof(char),47);}),_tag_dyneither(_tmp77D,sizeof(void*),2));});});});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(clause,te,cvtenv);
relns=Cyc_Relations_exp2relns(Cyc_Core_heap_region,clause);
if(!Cyc_Relations_consistent_relations(relns))
({struct Cyc_String_pa_PrintArg_struct _tmp784;_tmp784.tag=0;_tmp784.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(clause));({struct Cyc_String_pa_PrintArg_struct _tmp783;_tmp783.tag=0;_tmp783.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name);({void*_tmp781[2]={& _tmp783,& _tmp784};Cyc_Tcutil_terr(clause->loc,({const char*_tmp782="%s clause '%s' may be unsatisfiable";_tag_dyneither(_tmp782,sizeof(char),36);}),_tag_dyneither(_tmp781,sizeof(void*),2));});});});}
# 3851
return({struct _tuple30 _tmp785;_tmp785.f1=cvtenv;_tmp785.f2=relns;_tmp785;});}struct _tuple31{enum Cyc_Absyn_Format_Type f1;void*f2;};
# 3882 "tcutil.cyc"
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect,int allow_abs_aggr){
# 3890
{void*_tmp786=Cyc_Tcutil_compress(t);void*_tmp787=_tmp786;struct Cyc_List_List*_tmp958;void*_tmp957;void*_tmp956;void*_tmp955;void*_tmp954;void*_tmp953;struct _tuple2*_tmp952;struct Cyc_List_List**_tmp951;struct Cyc_Absyn_Typedefdecl**_tmp950;void**_tmp94F;union Cyc_Absyn_AggrInfoU*_tmp94E;struct Cyc_List_List**_tmp94D;enum Cyc_Absyn_AggrKind _tmp94C;struct Cyc_List_List*_tmp94B;struct Cyc_List_List*_tmp94A;struct Cyc_List_List**_tmp949;void**_tmp948;struct Cyc_Absyn_Tqual*_tmp947;void*_tmp946;struct Cyc_List_List*_tmp945;int _tmp944;struct Cyc_Absyn_VarargInfo*_tmp943;struct Cyc_List_List*_tmp942;struct Cyc_List_List*_tmp941;struct Cyc_Absyn_Exp*_tmp940;struct Cyc_List_List**_tmp93F;struct Cyc_Absyn_Exp*_tmp93E;struct Cyc_List_List**_tmp93D;void*_tmp93C;struct Cyc_Absyn_Tqual*_tmp93B;struct Cyc_Absyn_Exp**_tmp93A;union Cyc_Absyn_Constraint*_tmp939;unsigned int _tmp938;struct Cyc_Absyn_Exp*_tmp937;struct Cyc_Absyn_Exp*_tmp936;void*_tmp935;void*_tmp934;struct Cyc_Absyn_Tqual*_tmp933;void*_tmp932;union Cyc_Absyn_Constraint*_tmp931;union Cyc_Absyn_Constraint*_tmp930;union Cyc_Absyn_Constraint*_tmp92F;union Cyc_Absyn_DatatypeFieldInfoU*_tmp92E;struct Cyc_List_List*_tmp92D;union Cyc_Absyn_DatatypeInfoU*_tmp92C;struct Cyc_List_List**_tmp92B;struct _tuple2*_tmp92A;struct Cyc_Absyn_Enumdecl**_tmp929;struct Cyc_List_List*_tmp928;void*_tmp927;void***_tmp926;struct Cyc_Absyn_Tvar*_tmp925;struct Cyc_Core_Opt**_tmp924;void**_tmp923;switch(*((int*)_tmp787)){case 0: _LL66F: _LL670:
 goto _LL66E;case 1: _LL671: _tmp924=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp787)->f1;_tmp923=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp787)->f2;_LL672:
# 3894
 if(*_tmp924 == 0  || 
Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp924))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp924))->v,expected_kind))
*_tmp924=Cyc_Tcutil_kind_to_opt(expected_kind);
if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){
# 3899
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
*_tmp923=(void*)& Cyc_Absyn_UniqueRgn_val;else{
# 3902
*_tmp923=(void*)& Cyc_Absyn_HeapRgn_val;}}else{
if(cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar*_tmp788=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp78B=_cycalloc(sizeof(*_tmp78B));_tmp78B[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp78C;_tmp78C.tag=2;_tmp78C.f1=0;_tmp78C.f2=expected_kind;_tmp78C;});_tmp78B;}));
*_tmp923=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp789=_cycalloc(sizeof(*_tmp789));_tmp789[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp78A;_tmp78A.tag=2;_tmp78A.f1=_tmp788;_tmp78A;});_tmp789;});
_tmp925=_tmp788;goto _LL674;}else{
# 3908
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}
# 3910
goto _LL66E;case 2: _LL673: _tmp925=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp787)->f1;_LL674:
# 3912
{void*_tmp78D=Cyc_Absyn_compress_kb(_tmp925->kind);void*_tmp78E=_tmp78D;struct Cyc_Core_Opt**_tmp792;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp78E)->tag == 1){_LL6AA: _tmp792=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp78E)->f1;_LL6AB:
# 3914
*_tmp792=({struct Cyc_Core_Opt*_tmp78F=_cycalloc(sizeof(*_tmp78F));_tmp78F->v=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp790=_cycalloc(sizeof(*_tmp790));_tmp790[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp791;_tmp791.tag=2;_tmp791.f1=0;_tmp791.f2=expected_kind;_tmp791;});_tmp790;});_tmp78F;});goto _LL6A9;}else{_LL6AC: _LL6AD:
 goto _LL6A9;}_LL6A9:;}
# 3919
cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp925);
# 3922
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp925,put_in_effect);
# 3924
{void*_tmp793=Cyc_Absyn_compress_kb(_tmp925->kind);void*_tmp794=_tmp793;struct Cyc_Core_Opt**_tmp799;struct Cyc_Absyn_Kind*_tmp798;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp794)->tag == 2){_LL6AF: _tmp799=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp794)->f1;_tmp798=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp794)->f2;_LL6B0:
# 3926
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmp798))
*_tmp799=({struct Cyc_Core_Opt*_tmp795=_cycalloc(sizeof(*_tmp795));_tmp795->v=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp796=_cycalloc(sizeof(*_tmp796));_tmp796[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp797;_tmp797.tag=2;_tmp797.f1=0;_tmp797.f2=expected_kind;_tmp797;});_tmp796;});_tmp795;});
goto _LL6AE;}else{_LL6B1: _LL6B2:
 goto _LL6AE;}_LL6AE:;}
# 3931
goto _LL66E;case 26: _LL675: _tmp927=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp787)->f1)->r;_tmp926=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp787)->f2;_LL676: {
# 3937
void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmp79A=_tmp927;struct Cyc_Absyn_Datatypedecl*_tmp79D;struct Cyc_Absyn_Enumdecl*_tmp79C;struct Cyc_Absyn_Aggrdecl*_tmp79B;switch(*((int*)_tmp79A)){case 0: _LL6B4: _tmp79B=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp79A)->f1;_LL6B5:
# 3940
 if(te->in_extern_c_include)
_tmp79B->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,loc,_tmp79B);goto _LL6B3;case 1: _LL6B6: _tmp79C=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp79A)->f1;_LL6B7:
# 3944
 if(te->in_extern_c_include)
_tmp79C->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,loc,_tmp79C);goto _LL6B3;default: _LL6B8: _tmp79D=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp79A)->f1;_LL6B9:
# 3948
 Cyc_Tc_tcDatatypedecl(te,loc,_tmp79D);goto _LL6B3;}_LL6B3:;}
# 3950
*_tmp926=({void**_tmp79E=_cycalloc(sizeof(*_tmp79E));_tmp79E[0]=new_t;_tmp79E;});
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect,allow_abs_aggr);}case 14: _LL677: _tmp928=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp787)->f1;_LL678: {
# 3956
struct _RegionHandle _tmp79F=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp79F;_push_region(uprev_rgn);{
struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;
for(0;_tmp928 != 0;_tmp928=_tmp928->tl){
struct Cyc_Absyn_Enumfield*_tmp7A0=(struct Cyc_Absyn_Enumfield*)_tmp928->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp7A0->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp7A3;_tmp7A3.tag=0;_tmp7A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp7A0->name).f2);({void*_tmp7A1[1]={& _tmp7A3};Cyc_Tcutil_terr(_tmp7A0->loc,({const char*_tmp7A2="duplicate enum field name %s";_tag_dyneither(_tmp7A2,sizeof(char),29);}),_tag_dyneither(_tmp7A1,sizeof(void*),1));});});else{
# 3964
prev_fields=({struct Cyc_List_List*_tmp7A4=_region_malloc(uprev_rgn,sizeof(*_tmp7A4));_tmp7A4->hd=(*_tmp7A0->name).f2;_tmp7A4->tl=prev_fields;_tmp7A4;});}
# 3966
if(_tmp7A0->tag == 0)
_tmp7A0->tag=Cyc_Absyn_uint_exp(tag_count,_tmp7A0->loc);else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp7A0->tag)))
({struct Cyc_String_pa_PrintArg_struct _tmp7A7;_tmp7A7.tag=0;_tmp7A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp7A0->name).f2);({void*_tmp7A5[1]={& _tmp7A7};Cyc_Tcutil_terr(loc,({const char*_tmp7A6="enum field %s: expression is not constant";_tag_dyneither(_tmp7A6,sizeof(char),42);}),_tag_dyneither(_tmp7A5,sizeof(void*),1));});});}{
# 3971
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp7A0->tag))).f1;
tag_count=t1 + 1;
(te->ae)->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple2*k,struct _tuple29*v))Cyc_Dict_insert)((te->ae)->ordinaries,_tmp7A0->name,({struct _tuple29*_tmp7A8=_cycalloc(sizeof(*_tmp7A8));_tmp7A8->f1=(void*)({struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp7A9=_cycalloc(sizeof(*_tmp7A9));_tmp7A9[0]=({struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct _tmp7AA;_tmp7AA.tag=4;_tmp7AA.f1=t;_tmp7AA.f2=_tmp7A0;_tmp7AA;});_tmp7A9;});_tmp7A8->f2=1;_tmp7A8;}));};}}
# 3977
_npop_handler(0);goto _LL66E;
# 3956
;_pop_region(uprev_rgn);}case 13: _LL679: _tmp92A=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp787)->f1;_tmp929=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp787)->f2;_LL67A:
# 3979
 if(*_tmp929 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp929))->fields == 0){
struct _handler_cons _tmp7AB;_push_handler(& _tmp7AB);{int _tmp7AD=0;if(setjmp(_tmp7AB.handler))_tmp7AD=1;if(!_tmp7AD){
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp92A);
*_tmp929=*ed;}
# 3981
;_pop_handler();}else{void*_tmp7AC=(void*)_exn_thrown;void*_tmp7AE=_tmp7AC;void*_tmp7B1;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp7AE)->tag == Cyc_Dict_Absent){_LL6BB: _LL6BC: {
# 3985
struct Cyc_Absyn_Enumdecl*_tmp7AF=({struct Cyc_Absyn_Enumdecl*_tmp7B0=_cycalloc(sizeof(*_tmp7B0));_tmp7B0->sc=Cyc_Absyn_Extern;_tmp7B0->name=_tmp92A;_tmp7B0->fields=0;_tmp7B0;});
Cyc_Tc_tcEnumdecl(te,loc,_tmp7AF);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp92A);
*_tmp929=*ed;
goto _LL6BA;};}}else{_LL6BD: _tmp7B1=_tmp7AE;_LL6BE:(int)_rethrow(_tmp7B1);}_LL6BA:;}};}{
# 3991
struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp929);
goto _LL66E;};case 3: _LL67B: _tmp92C=(union Cyc_Absyn_DatatypeInfoU*)&(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp787)->f1).datatype_info;_tmp92B=(struct Cyc_List_List**)&(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp787)->f1).targs;_LL67C: {
# 3994
struct Cyc_List_List*_tmp7B2=*_tmp92B;
{union Cyc_Absyn_DatatypeInfoU _tmp7B3=*_tmp92C;union Cyc_Absyn_DatatypeInfoU _tmp7B4=_tmp7B3;struct Cyc_Absyn_Datatypedecl*_tmp7CC;struct _tuple2*_tmp7CB;int _tmp7CA;if((_tmp7B4.UnknownDatatype).tag == 1){_LL6C0: _tmp7CB=((_tmp7B4.UnknownDatatype).val).name;_tmp7CA=((_tmp7B4.UnknownDatatype).val).is_extensible;_LL6C1: {
# 3997
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmp7B5;_push_handler(& _tmp7B5);{int _tmp7B7=0;if(setjmp(_tmp7B5.handler))_tmp7B7=1;if(!_tmp7B7){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp7CB);;_pop_handler();}else{void*_tmp7B6=(void*)_exn_thrown;void*_tmp7B8=_tmp7B6;void*_tmp7BE;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp7B8)->tag == Cyc_Dict_Absent){_LL6C5: _LL6C6: {
# 4001
struct Cyc_Absyn_Datatypedecl*_tmp7B9=({struct Cyc_Absyn_Datatypedecl*_tmp7BD=_cycalloc(sizeof(*_tmp7BD));_tmp7BD->sc=Cyc_Absyn_Extern;_tmp7BD->name=_tmp7CB;_tmp7BD->tvs=0;_tmp7BD->fields=0;_tmp7BD->is_extensible=_tmp7CA;_tmp7BD;});
Cyc_Tc_tcDatatypedecl(te,loc,_tmp7B9);
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp7CB);
# 4005
if(_tmp7B2 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp7BC;_tmp7BC.tag=0;_tmp7BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7CB));({void*_tmp7BA[1]={& _tmp7BC};Cyc_Tcutil_terr(loc,({const char*_tmp7BB="declare parameterized datatype %s before using";_tag_dyneither(_tmp7BB,sizeof(char),47);}),_tag_dyneither(_tmp7BA,sizeof(void*),1));});});
return cvtenv;}
# 4010
goto _LL6C4;}}else{_LL6C7: _tmp7BE=_tmp7B8;_LL6C8:(int)_rethrow(_tmp7BE);}_LL6C4:;}};}
# 4014
if(_tmp7CA  && !(*tudp)->is_extensible)
({struct Cyc_String_pa_PrintArg_struct _tmp7C1;_tmp7C1.tag=0;_tmp7C1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7CB));({void*_tmp7BF[1]={& _tmp7C1};Cyc_Tcutil_terr(loc,({const char*_tmp7C0="datatype %s was not declared @extensible";_tag_dyneither(_tmp7C0,sizeof(char),41);}),_tag_dyneither(_tmp7BF,sizeof(void*),1));});});
*_tmp92C=Cyc_Absyn_KnownDatatype(tudp);
_tmp7CC=*tudp;goto _LL6C3;}}else{_LL6C2: _tmp7CC=*(_tmp7B4.KnownDatatype).val;_LL6C3: {
# 4021
struct Cyc_List_List*tvs=_tmp7CC->tvs;
for(0;_tmp7B2 != 0  && tvs != 0;(_tmp7B2=_tmp7B2->tl,tvs=tvs->tl)){
void*t=(void*)_tmp7B2->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 4027
{struct _tuple0 _tmp7C2=({struct _tuple0 _tmp7C5;_tmp7C5.f1=Cyc_Absyn_compress_kb(tv->kind);_tmp7C5.f2=t;_tmp7C5;});struct _tuple0 _tmp7C3=_tmp7C2;struct Cyc_Absyn_Tvar*_tmp7C4;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7C3.f1)->tag == 1){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7C3.f2)->tag == 2){_LL6CA: _tmp7C4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7C3.f2)->f1;_LL6CB:
# 4029
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp7C4);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp7C4,1);
continue;}else{goto _LL6CC;}}else{_LL6CC: _LL6CD:
 goto _LL6C9;}_LL6C9:;}{
# 4034
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 4038
if(_tmp7B2 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp7C8;_tmp7C8.tag=0;_tmp7C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7CC->name));({void*_tmp7C6[1]={& _tmp7C8};Cyc_Tcutil_terr(loc,({const char*_tmp7C7="too many type arguments for datatype %s";_tag_dyneither(_tmp7C7,sizeof(char),40);}),_tag_dyneither(_tmp7C6,sizeof(void*),1));});});
if(tvs != 0){
# 4043
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp7C9=_cycalloc(sizeof(*_tmp7C9));_tmp7C9->hd=e;_tmp7C9->tl=hidden_ts;_tmp7C9;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1,allow_abs_aggr);}
# 4050
*_tmp92B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp92B,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4052
goto _LL6BF;}}_LL6BF:;}
# 4054
goto _LL66E;}case 4: _LL67D: _tmp92E=(union Cyc_Absyn_DatatypeFieldInfoU*)&(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp787)->f1).field_info;_tmp92D=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp787)->f1).targs;_LL67E:
# 4057
{union Cyc_Absyn_DatatypeFieldInfoU _tmp7CD=*_tmp92E;union Cyc_Absyn_DatatypeFieldInfoU _tmp7CE=_tmp7CD;struct Cyc_Absyn_Datatypedecl*_tmp7FD;struct Cyc_Absyn_Datatypefield*_tmp7FC;struct _tuple2*_tmp7FB;struct _tuple2*_tmp7FA;int _tmp7F9;if((_tmp7CE.UnknownDatatypefield).tag == 1){_LL6CF: _tmp7FB=((_tmp7CE.UnknownDatatypefield).val).datatype_name;_tmp7FA=((_tmp7CE.UnknownDatatypefield).val).field_name;_tmp7F9=((_tmp7CE.UnknownDatatypefield).val).is_extensible;_LL6D0: {
# 4059
struct Cyc_Absyn_Datatypedecl*tud;
struct Cyc_Absyn_Datatypefield*tuf;
{struct _handler_cons _tmp7CF;_push_handler(& _tmp7CF);{int _tmp7D1=0;if(setjmp(_tmp7CF.handler))_tmp7D1=1;if(!_tmp7D1){*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp7FB);;_pop_handler();}else{void*_tmp7D0=(void*)_exn_thrown;void*_tmp7D2=_tmp7D0;void*_tmp7D6;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp7D2)->tag == Cyc_Dict_Absent){_LL6D4: _LL6D5:
# 4063
({struct Cyc_String_pa_PrintArg_struct _tmp7D5;_tmp7D5.tag=0;_tmp7D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7FB));({void*_tmp7D3[1]={& _tmp7D5};Cyc_Tcutil_terr(loc,({const char*_tmp7D4="unbound datatype %s";_tag_dyneither(_tmp7D4,sizeof(char),20);}),_tag_dyneither(_tmp7D3,sizeof(void*),1));});});
return cvtenv;}else{_LL6D6: _tmp7D6=_tmp7D2;_LL6D7:(int)_rethrow(_tmp7D6);}_LL6D3:;}};}
# 4066
{struct _handler_cons _tmp7D7;_push_handler(& _tmp7D7);{int _tmp7D9=0;if(setjmp(_tmp7D7.handler))_tmp7D9=1;if(!_tmp7D9){
{struct _RegionHandle _tmp7DA=_new_region("r");struct _RegionHandle*r=& _tmp7DA;_push_region(r);
{void*_tmp7DB=Cyc_Tcenv_lookup_ordinary(r,te,loc,_tmp7FA,0);void*_tmp7DC=_tmp7DB;struct Cyc_Absyn_Datatypedecl*_tmp7E6;struct Cyc_Absyn_Datatypefield*_tmp7E5;if(((struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp7DC)->tag == 2){_LL6D9: _tmp7E6=((struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp7DC)->f1;_tmp7E5=((struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp7DC)->f2;_LL6DA:
# 4070
 tuf=_tmp7E5;
tud=_tmp7E6;
if(_tmp7F9  && !tud->is_extensible)
({struct Cyc_String_pa_PrintArg_struct _tmp7DF;_tmp7DF.tag=0;_tmp7DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7FB));({void*_tmp7DD[1]={& _tmp7DF};Cyc_Tcutil_terr(loc,({const char*_tmp7DE="datatype %s was not declared @extensible";_tag_dyneither(_tmp7DE,sizeof(char),41);}),_tag_dyneither(_tmp7DD,sizeof(void*),1));});});
goto _LL6D8;}else{_LL6DB: _LL6DC:
({struct Cyc_String_pa_PrintArg_struct _tmp7E3;_tmp7E3.tag=0;_tmp7E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7FB));({struct Cyc_String_pa_PrintArg_struct _tmp7E2;_tmp7E2.tag=0;_tmp7E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7FA));({void*_tmp7E0[2]={& _tmp7E2,& _tmp7E3};Cyc_Tcutil_terr(loc,({const char*_tmp7E1="unbound field %s in type datatype %s";_tag_dyneither(_tmp7E1,sizeof(char),37);}),_tag_dyneither(_tmp7E0,sizeof(void*),2));});});});{
struct Cyc_Tcutil_CVTEnv _tmp7E4=cvtenv;_npop_handler(1);return _tmp7E4;};}_LL6D8:;}
# 4068
;_pop_region(r);}
# 4067
;_pop_handler();}else{void*_tmp7D8=(void*)_exn_thrown;void*_tmp7E7=_tmp7D8;void*_tmp7EC;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp7E7)->tag == Cyc_Dict_Absent){_LL6DE: _LL6DF:
# 4083
({struct Cyc_String_pa_PrintArg_struct _tmp7EB;_tmp7EB.tag=0;_tmp7EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7FB));({struct Cyc_String_pa_PrintArg_struct _tmp7EA;_tmp7EA.tag=0;_tmp7EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7FA));({void*_tmp7E8[2]={& _tmp7EA,& _tmp7EB};Cyc_Tcutil_terr(loc,({const char*_tmp7E9="unbound field %s in type datatype %s";_tag_dyneither(_tmp7E9,sizeof(char),37);}),_tag_dyneither(_tmp7E8,sizeof(void*),2));});});});
return cvtenv;}else{_LL6E0: _tmp7EC=_tmp7E7;_LL6E1:(int)_rethrow(_tmp7EC);}_LL6DD:;}};}
# 4087
*_tmp92E=Cyc_Absyn_KnownDatatypefield(tud,tuf);
_tmp7FD=tud;_tmp7FC=tuf;goto _LL6D2;}}else{_LL6D1: _tmp7FD=((_tmp7CE.KnownDatatypefield).val).f1;_tmp7FC=((_tmp7CE.KnownDatatypefield).val).f2;_LL6D2: {
# 4091
struct Cyc_List_List*tvs=_tmp7FD->tvs;
for(0;_tmp92D != 0  && tvs != 0;(_tmp92D=_tmp92D->tl,tvs=tvs->tl)){
void*t=(void*)_tmp92D->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 4097
{struct _tuple0 _tmp7ED=({struct _tuple0 _tmp7F0;_tmp7F0.f1=Cyc_Absyn_compress_kb(tv->kind);_tmp7F0.f2=t;_tmp7F0;});struct _tuple0 _tmp7EE=_tmp7ED;struct Cyc_Absyn_Tvar*_tmp7EF;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7EE.f1)->tag == 1){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7EE.f2)->tag == 2){_LL6E3: _tmp7EF=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7EE.f2)->f1;_LL6E4:
# 4099
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp7EF);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp7EF,1);
continue;}else{goto _LL6E5;}}else{_LL6E5: _LL6E6:
 goto _LL6E2;}_LL6E2:;}{
# 4104
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 4108
if(_tmp92D != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp7F4;_tmp7F4.tag=0;_tmp7F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7FC->name));({struct Cyc_String_pa_PrintArg_struct _tmp7F3;_tmp7F3.tag=0;_tmp7F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7FD->name));({void*_tmp7F1[2]={& _tmp7F3,& _tmp7F4};Cyc_Tcutil_terr(loc,({const char*_tmp7F2="too many type arguments for datatype %s.%s";_tag_dyneither(_tmp7F2,sizeof(char),43);}),_tag_dyneither(_tmp7F1,sizeof(void*),2));});});});
if(tvs != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp7F8;_tmp7F8.tag=0;_tmp7F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7FC->name));({struct Cyc_String_pa_PrintArg_struct _tmp7F7;_tmp7F7.tag=0;_tmp7F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7FD->name));({void*_tmp7F5[2]={& _tmp7F7,& _tmp7F8};Cyc_Tcutil_terr(loc,({const char*_tmp7F6="too few type arguments for datatype %s.%s";_tag_dyneither(_tmp7F6,sizeof(char),42);}),_tag_dyneither(_tmp7F5,sizeof(void*),2));});});});
goto _LL6CE;}}_LL6CE:;}
# 4116
goto _LL66E;case 5: _LL67F: _tmp934=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp787)->f1).elt_typ;_tmp933=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp787)->f1).elt_tq;_tmp932=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp787)->f1).ptr_atts).rgn;_tmp931=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp787)->f1).ptr_atts).nullable;_tmp930=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp787)->f1).ptr_atts).bounds;_tmp92F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp787)->f1).ptr_atts).zero_term;_LL680: {
# 4119
int is_zero_terminated;
# 4121
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp934,1,1);
_tmp933->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp933->print_const,_tmp934);{
struct Cyc_Absyn_Kind*k;
{enum Cyc_Absyn_AliasQual _tmp7FE=expected_kind->aliasqual;enum Cyc_Absyn_AliasQual _tmp7FF=_tmp7FE;switch(_tmp7FF){case Cyc_Absyn_Aliasable: _LL6E8: _LL6E9:
 k=& Cyc_Tcutil_rk;goto _LL6E7;case Cyc_Absyn_Unique: _LL6EA: _LL6EB:
 k=& Cyc_Tcutil_urk;goto _LL6E7;default: _LL6EC: _LL6ED:
 k=& Cyc_Tcutil_trk;goto _LL6E7;}_LL6E7:;}
# 4129
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmp932,1,1);
{union Cyc_Absyn_Constraint*_tmp800=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp92F);union Cyc_Absyn_Constraint*_tmp801=_tmp800;int _tmp807;switch((((union Cyc_Absyn_Constraint*)_tmp801)->Eq_constr).tag){case 3: _LL6EF: _LL6F0:
# 4134
{void*_tmp802=Cyc_Tcutil_compress(_tmp934);void*_tmp803=_tmp802;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp803)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp803)->f2 == Cyc_Absyn_Char_sz){_LL6F6: _LL6F7:
# 4136
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp92F,Cyc_Absyn_true_conref);
is_zero_terminated=1;
goto _LL6F5;}else{goto _LL6F8;}}else{_LL6F8: _LL6F9:
# 4140
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp92F,Cyc_Absyn_false_conref);
is_zero_terminated=0;
goto _LL6F5;}_LL6F5:;}
# 4144
goto _LL6EE;case 1: _LL6F1: _tmp807=(int)(_tmp801->Eq_constr).val;if(_tmp807){_LL6F2:
# 4147
 if(!Cyc_Tcutil_admits_zero(_tmp934))
({struct Cyc_String_pa_PrintArg_struct _tmp806;_tmp806.tag=0;_tmp806.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp934));({void*_tmp804[1]={& _tmp806};Cyc_Tcutil_terr(loc,({const char*_tmp805="cannot have a pointer to zero-terminated %s elements";_tag_dyneither(_tmp805,sizeof(char),53);}),_tag_dyneither(_tmp804,sizeof(void*),1));});});
is_zero_terminated=1;
goto _LL6EE;}else{goto _LL6F3;}default: _LL6F3: _LL6F4:
# 4153
 is_zero_terminated=0;
goto _LL6EE;}_LL6EE:;}
# 4157
{void*_tmp808=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp930);void*_tmp809=_tmp808;struct Cyc_Absyn_Exp*_tmp814;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp809)->tag == 0){_LL6FB: _LL6FC:
 goto _LL6FA;}else{_LL6FD: _tmp814=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp809)->f1;_LL6FE: {
# 4160
struct _RegionHandle _tmp80A=_new_region("temp");struct _RegionHandle*temp=& _tmp80A;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp80B=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp80B,0,_tmp814);}
# 4164
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp814,te,cvtenv);
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp814))
({void*_tmp80C=0;Cyc_Tcutil_terr(loc,({const char*_tmp80D="pointer bounds expression is not an unsigned int";_tag_dyneither(_tmp80D,sizeof(char),49);}),_tag_dyneither(_tmp80C,sizeof(void*),0));});{
struct _tuple14 _tmp80E=Cyc_Evexp_eval_const_uint_exp(_tmp814);struct _tuple14 _tmp80F=_tmp80E;unsigned int _tmp813;int _tmp812;_LL700: _tmp813=_tmp80F.f1;_tmp812=_tmp80F.f2;_LL701:;
if(is_zero_terminated  && (!_tmp812  || _tmp813 < 1))
({void*_tmp810=0;Cyc_Tcutil_terr(loc,({const char*_tmp811="zero-terminated pointer cannot point to empty sequence";_tag_dyneither(_tmp811,sizeof(char),55);}),_tag_dyneither(_tmp810,sizeof(void*),0));});
_npop_handler(0);goto _LL6FA;};
# 4160
;_pop_region(temp);}}_LL6FA:;}
# 4172
goto _LL66E;};}case 19: _LL681: _tmp935=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp787)->f1;_LL682:
# 4174
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp935,1,1);goto _LL66E;case 28: _LL683: _LL684:
 goto _LL66E;case 18: _LL685: _tmp936=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp787)->f1;_LL686: {
# 4180
struct _RegionHandle _tmp815=_new_region("temp");struct _RegionHandle*temp=& _tmp815;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp816=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp816,0,_tmp936);}
# 4184
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp936))
({void*_tmp817=0;Cyc_Tcutil_terr(loc,({const char*_tmp818="valueof_t requires an int expression";_tag_dyneither(_tmp818,sizeof(char),37);}),_tag_dyneither(_tmp817,sizeof(void*),0));});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp936,te,cvtenv);
_npop_handler(0);goto _LL66E;
# 4180
;_pop_region(temp);}case 27: _LL687: _tmp937=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp787)->f1;_LL688: {
# 4192
struct _RegionHandle _tmp819=_new_region("temp");struct _RegionHandle*temp=& _tmp819;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp81A=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp81A,0,_tmp937);}
# 4196
_npop_handler(0);goto _LL66E;
# 4192
;_pop_region(temp);}case 6: _LL689: _LL68A:
# 4197
 goto _LL68C;case 7: _LL68B: _LL68C:
 goto _LL66E;case 8: _LL68D: _tmp93C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp787)->f1).elt_type;_tmp93B=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp787)->f1).tq;_tmp93A=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp787)->f1).num_elts;_tmp939=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp787)->f1).zero_term;_tmp938=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp787)->f1).zt_loc;_LL68E: {
# 4202
struct Cyc_Absyn_Exp*_tmp81B=*_tmp93A;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp93C,1,allow_abs_aggr);
_tmp93B->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp93B->print_const,_tmp93C);{
int is_zero_terminated;
{union Cyc_Absyn_Constraint*_tmp81C=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp939);union Cyc_Absyn_Constraint*_tmp81D=_tmp81C;int _tmp821;switch((((union Cyc_Absyn_Constraint*)_tmp81D)->Eq_constr).tag){case 3: _LL703: _LL704:
# 4209
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp939,Cyc_Absyn_false_conref);
is_zero_terminated=0;
# 4224 "tcutil.cyc"
goto _LL702;case 1: _LL705: _tmp821=(int)(_tmp81D->Eq_constr).val;if(_tmp821){_LL706:
# 4227
 if(!Cyc_Tcutil_admits_zero(_tmp93C))
({struct Cyc_String_pa_PrintArg_struct _tmp820;_tmp820.tag=0;_tmp820.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp93C));({void*_tmp81E[1]={& _tmp820};Cyc_Tcutil_terr(loc,({const char*_tmp81F="cannot have a zero-terminated array of %s elements";_tag_dyneither(_tmp81F,sizeof(char),51);}),_tag_dyneither(_tmp81E,sizeof(void*),1));});});
is_zero_terminated=1;
goto _LL702;}else{goto _LL707;}default: _LL707: _LL708:
# 4233
 is_zero_terminated=0;
goto _LL702;}_LL702:;}
# 4238
if(_tmp81B == 0){
# 4240
if(is_zero_terminated)
# 4242
*_tmp93A=(_tmp81B=Cyc_Absyn_uint_exp(1,0));else{
# 4245
({void*_tmp822=0;Cyc_Tcutil_warn(loc,({const char*_tmp823="array bound defaults to 1 here";_tag_dyneither(_tmp823,sizeof(char),31);}),_tag_dyneither(_tmp822,sizeof(void*),0));});
*_tmp93A=(_tmp81B=Cyc_Absyn_uint_exp(1,0));}}{
# 4249
struct _RegionHandle _tmp824=_new_region("temp");struct _RegionHandle*temp=& _tmp824;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp825=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp825,0,_tmp81B);}
# 4253
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp81B))
({void*_tmp826=0;Cyc_Tcutil_terr(loc,({const char*_tmp827="array bounds expression is not an unsigned int";_tag_dyneither(_tmp827,sizeof(char),47);}),_tag_dyneither(_tmp826,sizeof(void*),0));});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp81B,te,cvtenv);{
# 4260
struct _tuple14 _tmp828=Cyc_Evexp_eval_const_uint_exp(_tmp81B);struct _tuple14 _tmp829=_tmp828;unsigned int _tmp82F;int _tmp82E;_LL70A: _tmp82F=_tmp829.f1;_tmp82E=_tmp829.f2;_LL70B:;
# 4262
if((is_zero_terminated  && _tmp82E) && _tmp82F < 1)
({void*_tmp82A=0;Cyc_Tcutil_warn(loc,({const char*_tmp82B="zero terminated array cannot have zero elements";_tag_dyneither(_tmp82B,sizeof(char),48);}),_tag_dyneither(_tmp82A,sizeof(void*),0));});
# 4265
if((_tmp82E  && _tmp82F < 1) && Cyc_Cyclone_tovc_r){
({void*_tmp82C=0;Cyc_Tcutil_warn(loc,({const char*_tmp82D="arrays with 0 elements are not supported except with gcc -- changing to 1.";_tag_dyneither(_tmp82D,sizeof(char),75);}),_tag_dyneither(_tmp82C,sizeof(void*),0));});
*_tmp93A=Cyc_Absyn_uint_exp(1,0);}
# 4269
_npop_handler(0);goto _LL66E;};
# 4249
;_pop_region(temp);};};}case 9: _LL68F: _tmp949=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp787)->f1).tvars;_tmp948=(void**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp787)->f1).effect;_tmp947=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp787)->f1).ret_tqual;_tmp946=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp787)->f1).ret_typ;_tmp945=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp787)->f1).args;_tmp944=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp787)->f1).c_varargs;_tmp943=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp787)->f1).cyc_varargs;_tmp942=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp787)->f1).rgn_po;_tmp941=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp787)->f1).attributes;_tmp940=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp787)->f1).requires_clause;_tmp93F=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp787)->f1).requires_relns;_tmp93E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp787)->f1).ensures_clause;_tmp93D=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp787)->f1).ensures_relns;_LL690: {
# 4276
int num_convs=0;
int seen_cdecl=0;
int seen_stdcall=0;
int seen_fastcall=0;
int seen_format=0;
enum Cyc_Absyn_Format_Type ft=Cyc_Absyn_Printf_ft;
int fmt_desc_arg=-1;
int fmt_arg_start=-1;
for(0;_tmp941 != 0;_tmp941=_tmp941->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp941->hd))
({struct Cyc_String_pa_PrintArg_struct _tmp832;_tmp832.tag=0;_tmp832.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp941->hd));({void*_tmp830[1]={& _tmp832};Cyc_Tcutil_terr(loc,({const char*_tmp831="bad function type attribute %s";_tag_dyneither(_tmp831,sizeof(char),31);}),_tag_dyneither(_tmp830,sizeof(void*),1));});});{
void*_tmp833=(void*)_tmp941->hd;void*_tmp834=_tmp833;enum Cyc_Absyn_Format_Type _tmp839;int _tmp838;int _tmp837;switch(*((int*)_tmp834)){case 1: _LL70D: _LL70E:
# 4289
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL70C;case 2: _LL70F: _LL710:
# 4291
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL70C;case 3: _LL711: _LL712:
# 4293
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL70C;case 19: _LL713: _tmp839=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp834)->f1;_tmp838=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp834)->f2;_tmp837=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp834)->f3;_LL714:
# 4295
 if(!seen_format){
seen_format=1;
ft=_tmp839;
fmt_desc_arg=_tmp838;
fmt_arg_start=_tmp837;}else{
# 4301
({void*_tmp835=0;Cyc_Tcutil_terr(loc,({const char*_tmp836="function can't have multiple format attributes";_tag_dyneither(_tmp836,sizeof(char),47);}),_tag_dyneither(_tmp835,sizeof(void*),0));});}
goto _LL70C;default: _LL715: _LL716:
 goto _LL70C;}_LL70C:;};}
# 4306
if(num_convs > 1)
({void*_tmp83A=0;Cyc_Tcutil_terr(loc,({const char*_tmp83B="function can't have multiple calling conventions";_tag_dyneither(_tmp83B,sizeof(char),49);}),_tag_dyneither(_tmp83A,sizeof(void*),0));});
# 4311
Cyc_Tcutil_check_unique_tvars(loc,*_tmp949);
{struct Cyc_List_List*b=*_tmp949;for(0;b != 0;b=b->tl){
((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{
void*_tmp83C=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp83D=_tmp83C;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp83D)->tag == 0){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp83D)->f1)->kind == Cyc_Absyn_MemKind){_LL718: _LL719:
# 4317
({struct Cyc_String_pa_PrintArg_struct _tmp840;_tmp840.tag=0;_tmp840.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name);({void*_tmp83E[1]={& _tmp840};Cyc_Tcutil_terr(loc,({const char*_tmp83F="function attempts to abstract Mem type variable %s";_tag_dyneither(_tmp83F,sizeof(char),51);}),_tag_dyneither(_tmp83E,sizeof(void*),1));});});
goto _LL717;}else{goto _LL71A;}}else{_LL71A: _LL71B:
 goto _LL717;}_LL717:;};}}{
# 4325
struct _RegionHandle _tmp841=_new_region("newr");struct _RegionHandle*newr=& _tmp841;_push_region(newr);{
struct Cyc_Tcutil_CVTEnv _tmp842=({struct Cyc_Tcutil_CVTEnv _tmp8D5;_tmp8D5.r=newr;_tmp8D5.kind_env=cvtenv.kind_env;_tmp8D5.free_vars=0;_tmp8D5.free_evars=0;_tmp8D5.generalize_evars=cvtenv.generalize_evars;_tmp8D5.fn_result=1;_tmp8D5;});
# 4331
_tmp842=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp842,& Cyc_Tcutil_tmk,_tmp946,1,1);
_tmp947->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp947->print_const,_tmp946);
_tmp842.fn_result=0;
{struct Cyc_List_List*a=_tmp945;for(0;a != 0;a=a->tl){
struct _tuple10*_tmp843=(struct _tuple10*)a->hd;
void*_tmp844=(*_tmp843).f3;
_tmp842=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp842,& Cyc_Tcutil_tmk,_tmp844,1,1);{
int _tmp845=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp843).f2).print_const,_tmp844);
((*_tmp843).f2).real_const=_tmp845;
# 4342
if(Cyc_Tcutil_is_array(_tmp844)){
# 4345
void*_tmp846=Cyc_Absyn_new_evar(0,0);
_tmp842=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp842,& Cyc_Tcutil_rk,_tmp846,1,1);
(*_tmp843).f3=Cyc_Tcutil_promote_array(_tmp844,_tmp846,0);}};}}
# 4352
if(_tmp943 != 0){
if(_tmp944)({void*_tmp847=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp848="both c_vararg and cyc_vararg";_tag_dyneither(_tmp848,sizeof(char),29);}),_tag_dyneither(_tmp847,sizeof(void*),0));});{
struct Cyc_Absyn_VarargInfo _tmp849=*_tmp943;struct Cyc_Absyn_VarargInfo _tmp84A=_tmp849;void*_tmp85D;int _tmp85C;_LL71D: _tmp85D=_tmp84A.type;_tmp85C=_tmp84A.inject;_LL71E:;
_tmp842=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp842,& Cyc_Tcutil_tmk,_tmp85D,1,1);
(_tmp943->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmp943->tq).print_const,_tmp85D);
# 4358
if(_tmp85C){
void*_tmp84B=Cyc_Tcutil_compress(_tmp85D);void*_tmp84C=_tmp84B;void*_tmp85B;union Cyc_Absyn_Constraint*_tmp85A;union Cyc_Absyn_Constraint*_tmp859;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp84C)->tag == 5){_LL720: _tmp85B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp84C)->f1).elt_typ;_tmp85A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp84C)->f1).ptr_atts).bounds;_tmp859=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp84C)->f1).ptr_atts).zero_term;_LL721:
# 4361
{void*_tmp84D=Cyc_Tcutil_compress(_tmp85B);void*_tmp84E=_tmp84D;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp84E)->tag == 3){_LL725: _LL726:
# 4363
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp859))
({void*_tmp84F=0;Cyc_Tcutil_terr(loc,({const char*_tmp850="can't inject into a zeroterm pointer";_tag_dyneither(_tmp850,sizeof(char),37);}),_tag_dyneither(_tmp84F,sizeof(void*),0));});
{void*_tmp851=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp85A);void*_tmp852=_tmp851;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp852)->tag == 0){_LL72A: _LL72B:
# 4367
({void*_tmp853=0;Cyc_Tcutil_terr(loc,({const char*_tmp854="can't inject into a fat pointer to datatype";_tag_dyneither(_tmp854,sizeof(char),44);}),_tag_dyneither(_tmp853,sizeof(void*),0));});
goto _LL729;}else{_LL72C: _LL72D:
 goto _LL729;}_LL729:;}
# 4371
goto _LL724;}else{_LL727: _LL728:
({void*_tmp855=0;Cyc_Tcutil_terr(loc,({const char*_tmp856="can't inject a non-datatype type";_tag_dyneither(_tmp856,sizeof(char),33);}),_tag_dyneither(_tmp855,sizeof(void*),0));});goto _LL724;}_LL724:;}
# 4374
goto _LL71F;}else{_LL722: _LL723:
({void*_tmp857=0;Cyc_Tcutil_terr(loc,({const char*_tmp858="expecting a datatype pointer type";_tag_dyneither(_tmp858,sizeof(char),34);}),_tag_dyneither(_tmp857,sizeof(void*),0));});goto _LL71F;}_LL71F:;}};}
# 4380
if(seen_format){
int _tmp85E=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp945);
if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmp85E) || fmt_arg_start < 0) || 
# 4384
(_tmp943 == 0  && !_tmp944) && fmt_arg_start != 0) || 
(_tmp943 != 0  || _tmp944) && fmt_arg_start != _tmp85E + 1)
# 4387
({void*_tmp85F=0;Cyc_Tcutil_terr(loc,({const char*_tmp860="bad format descriptor";_tag_dyneither(_tmp860,sizeof(char),22);}),_tag_dyneither(_tmp85F,sizeof(void*),0));});else{
# 4390
struct _tuple10 _tmp861=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp945,fmt_desc_arg - 1);struct _tuple10 _tmp862=_tmp861;void*_tmp87A;_LL72F: _tmp87A=_tmp862.f3;_LL730:;
# 4392
{void*_tmp863=Cyc_Tcutil_compress(_tmp87A);void*_tmp864=_tmp863;void*_tmp872;union Cyc_Absyn_Constraint*_tmp871;union Cyc_Absyn_Constraint*_tmp870;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp864)->tag == 5){_LL732: _tmp872=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp864)->f1).elt_typ;_tmp871=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp864)->f1).ptr_atts).bounds;_tmp870=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp864)->f1).ptr_atts).zero_term;_LL733:
# 4395
{struct _tuple0 _tmp865=({struct _tuple0 _tmp86D;_tmp86D.f1=Cyc_Tcutil_compress(_tmp872);_tmp86D.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp871);_tmp86D;});struct _tuple0 _tmp866=_tmp865;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp866.f1)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp866.f1)->f2 == Cyc_Absyn_Char_sz){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp866.f2)->tag == 0){_LL737: _LL738:
# 4397
 if(_tmp944)
({void*_tmp867=0;Cyc_Tcutil_terr(loc,({const char*_tmp868="format descriptor is not a char * type";_tag_dyneither(_tmp868,sizeof(char),39);}),_tag_dyneither(_tmp867,sizeof(void*),0));});
goto _LL736;}else{_LL739: _LL73A:
# 4401
 if(!_tmp944)
({void*_tmp869=0;Cyc_Tcutil_terr(loc,({const char*_tmp86A="format descriptor is not a char ? type";_tag_dyneither(_tmp86A,sizeof(char),39);}),_tag_dyneither(_tmp869,sizeof(void*),0));});
goto _LL736;}}else{goto _LL73B;}}else{_LL73B: _LL73C:
# 4405
({void*_tmp86B=0;Cyc_Tcutil_terr(loc,({const char*_tmp86C="format descriptor is not a string type";_tag_dyneither(_tmp86C,sizeof(char),39);}),_tag_dyneither(_tmp86B,sizeof(void*),0));});
goto _LL736;}_LL736:;}
# 4408
goto _LL731;}else{_LL734: _LL735:
({void*_tmp86E=0;Cyc_Tcutil_terr(loc,({const char*_tmp86F="format descriptor is not a string type";_tag_dyneither(_tmp86F,sizeof(char),39);}),_tag_dyneither(_tmp86E,sizeof(void*),0));});goto _LL731;}_LL731:;}
# 4411
if(fmt_arg_start != 0  && !_tmp944){
# 4415
int problem;
{struct _tuple31 _tmp873=({struct _tuple31 _tmp877;_tmp877.f1=ft;_tmp877.f2=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp943))->type));_tmp877;});struct _tuple31 _tmp874=_tmp873;struct Cyc_Absyn_Datatypedecl*_tmp876;struct Cyc_Absyn_Datatypedecl*_tmp875;if(_tmp874.f1 == Cyc_Absyn_Printf_ft){if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp874.f2)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp874.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL73E: _tmp875=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp874.f2)->f1).datatype_info).KnownDatatype).val;_LL73F:
# 4418
 problem=Cyc_Absyn_qvar_cmp(_tmp875->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL73D;}else{goto _LL742;}}else{goto _LL742;}}else{if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp874.f2)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp874.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL740: _tmp876=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp874.f2)->f1).datatype_info).KnownDatatype).val;_LL741:
# 4420
 problem=Cyc_Absyn_qvar_cmp(_tmp876->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL73D;}else{goto _LL742;}}else{_LL742: _LL743:
# 4422
 problem=1;goto _LL73D;}}_LL73D:;}
# 4424
if(problem)
({void*_tmp878=0;Cyc_Tcutil_terr(loc,({const char*_tmp879="format attribute and vararg types don't match";_tag_dyneither(_tmp879,sizeof(char),46);}),_tag_dyneither(_tmp878,sizeof(void*),0));});}}}
# 4432
{struct Cyc_List_List*rpo=_tmp942;for(0;rpo != 0;rpo=rpo->tl){
struct _tuple0*_tmp87B=(struct _tuple0*)rpo->hd;struct _tuple0*_tmp87C=_tmp87B;void*_tmp87E;void*_tmp87D;_LL745: _tmp87E=_tmp87C->f1;_tmp87D=_tmp87C->f2;_LL746:;
_tmp842=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp842,& Cyc_Tcutil_ek,_tmp87E,1,1);
_tmp842=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp842,& Cyc_Tcutil_trk,_tmp87D,1,1);}}{
# 4440
struct Cyc_Tcenv_Fenv*_tmp87F=Cyc_Tcenv_bogus_fenv(newr,_tmp946,_tmp945);
struct Cyc_Tcenv_Tenv*_tmp880=({struct Cyc_Tcenv_Tenv*_tmp8D4=_region_malloc(newr,sizeof(*_tmp8D4));_tmp8D4->ns=te->ns;_tmp8D4->ae=te->ae;_tmp8D4->le=_tmp87F;_tmp8D4->allow_valueof=1;_tmp8D4->in_extern_c_include=te->in_extern_c_include;_tmp8D4;});
struct _tuple30 _tmp881=Cyc_Tcutil_check_clause(loc,_tmp880,_tmp842,({const char*_tmp8D3="@requires";_tag_dyneither(_tmp8D3,sizeof(char),10);}),_tmp940);struct _tuple30 _tmp882=_tmp881;struct Cyc_Tcutil_CVTEnv _tmp8D2;struct Cyc_List_List*_tmp8D1;_LL748: _tmp8D2=_tmp882.f1;_tmp8D1=_tmp882.f2;_LL749:;
_tmp842=_tmp8D2;
*_tmp93F=_tmp8D1;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp945,_tmp8D1);{
# 4447
struct _dyneither_ptr*_tmp883=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"return_value",sizeof(char),13);
struct Cyc_Absyn_Vardecl*_tmp884=({struct Cyc_Absyn_Vardecl*_tmp8CE=_cycalloc(sizeof(*_tmp8CE));_tmp8CE->sc=Cyc_Absyn_Public;_tmp8CE->name=({struct _tuple2*_tmp8CF=_cycalloc(sizeof(*_tmp8CF));_tmp8CF->f1=Cyc_Absyn_Loc_n;_tmp8CF->f2=_tmp883;_tmp8CF;});_tmp8CE->tq=
Cyc_Absyn_empty_tqual(0);_tmp8CE->type=_tmp946;_tmp8CE->initializer=0;_tmp8CE->rgn=
Cyc_Tcenv_curr_rgn(_tmp880);_tmp8CE->attributes=0;_tmp8CE->escapes=0;_tmp8CE;});
# 4452
_tmp880=Cyc_Tcenv_add_local_var(newr,0,_tmp880,_tmp884);{
struct _tuple30 _tmp885=Cyc_Tcutil_check_clause(loc,_tmp880,_tmp842,({const char*_tmp8CD="@ensures";_tag_dyneither(_tmp8CD,sizeof(char),9);}),_tmp93E);struct _tuple30 _tmp886=_tmp885;struct Cyc_Tcutil_CVTEnv _tmp8CC;struct Cyc_List_List*_tmp8CB;_LL74B: _tmp8CC=_tmp886.f1;_tmp8CB=_tmp886.f2;_LL74C:;
_tmp842=_tmp8CC;
*_tmp93D=_tmp8CB;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp945,_tmp8CB);
# 4458
if(*_tmp948 != 0)
_tmp842=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp842,& Cyc_Tcutil_ek,(void*)_check_null(*_tmp948),1,1);else{
# 4461
struct Cyc_List_List*effect=0;
# 4466
{struct Cyc_List_List*tvs=_tmp842.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp887=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp888=_tmp887;struct Cyc_Absyn_Tvar*_tmp8A1;int _tmp8A0;_LL74E: _tmp8A1=_tmp888.f1;_tmp8A0=_tmp888.f2;_LL74F:;
if(!_tmp8A0)continue;{
void*_tmp889=Cyc_Absyn_compress_kb(_tmp8A1->kind);void*_tmp88A=_tmp889;struct Cyc_Core_Opt**_tmp89F;struct Cyc_Absyn_Kind*_tmp89E;struct Cyc_Core_Opt**_tmp89D;struct Cyc_Core_Opt**_tmp89C;struct Cyc_Absyn_Kind*_tmp89B;switch(*((int*)_tmp88A)){case 2: _LL751: _tmp89C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp88A)->f1;_tmp89B=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp88A)->f2;if(_tmp89B->kind == Cyc_Absyn_RgnKind){_LL752:
# 4472
 if(_tmp89B->aliasqual == Cyc_Absyn_Top){
*_tmp89C=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);_tmp89E=_tmp89B;goto _LL754;}
# 4475
*_tmp89C=Cyc_Tcutil_kind_to_bound_opt(_tmp89B);_tmp89E=_tmp89B;goto _LL754;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp88A)->f2)->kind){case Cyc_Absyn_IntKind: _LL755: _LL756:
# 4478
 goto _LL758;case Cyc_Absyn_EffKind: _LL759: _tmp89D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp88A)->f1;_LL75A:
# 4481
*_tmp89D=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);goto _LL75C;default: goto _LL75F;}}case 0: _LL753: _tmp89E=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp88A)->f1;if(_tmp89E->kind == Cyc_Absyn_RgnKind){_LL754:
# 4477
 effect=({struct Cyc_List_List*_tmp88B=_cycalloc(sizeof(*_tmp88B));_tmp88B->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp88C=_cycalloc(sizeof(*_tmp88C));_tmp88C[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp88D;_tmp88D.tag=23;_tmp88D.f1=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp88E=_cycalloc(sizeof(*_tmp88E));_tmp88E[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp88F;_tmp88F.tag=2;_tmp88F.f1=_tmp8A1;_tmp88F;});_tmp88E;});_tmp88D;});_tmp88C;});_tmp88B->tl=effect;_tmp88B;});goto _LL750;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp88A)->f1)->kind){case Cyc_Absyn_IntKind: _LL757: _LL758:
# 4479
 goto _LL750;case Cyc_Absyn_EffKind: _LL75B: _LL75C:
# 4483
 effect=({struct Cyc_List_List*_tmp890=_cycalloc(sizeof(*_tmp890));_tmp890->hd=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp891=_cycalloc(sizeof(*_tmp891));_tmp891[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp892;_tmp892.tag=2;_tmp892.f1=_tmp8A1;_tmp892;});_tmp891;});_tmp890->tl=effect;_tmp890;});goto _LL750;default: _LL75F: _LL760:
# 4488
 effect=({struct Cyc_List_List*_tmp896=_cycalloc(sizeof(*_tmp896));_tmp896->hd=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp897=_cycalloc(sizeof(*_tmp897));_tmp897[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp898;_tmp898.tag=25;_tmp898.f1=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp899=_cycalloc(sizeof(*_tmp899));_tmp899[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp89A;_tmp89A.tag=2;_tmp89A.f1=_tmp8A1;_tmp89A;});_tmp899;});_tmp898;});_tmp897;});_tmp896->tl=effect;_tmp896;});goto _LL750;}}default: _LL75D: _tmp89F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp88A)->f1;_LL75E:
# 4485
*_tmp89F=({struct Cyc_Core_Opt*_tmp893=_cycalloc(sizeof(*_tmp893));_tmp893->v=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp894=_cycalloc(sizeof(*_tmp894));_tmp894[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp895;_tmp895.tag=2;_tmp895.f1=0;_tmp895.f2=& Cyc_Tcutil_ak;_tmp895;});_tmp894;});_tmp893;});goto _LL760;}_LL750:;};}}
# 4492
{struct Cyc_List_List*ts=_tmp842.free_evars;for(0;ts != 0;ts=ts->tl){
struct _tuple29 _tmp8A2=*((struct _tuple29*)ts->hd);struct _tuple29 _tmp8A3=_tmp8A2;void*_tmp8AE;int _tmp8AD;_LL762: _tmp8AE=_tmp8A3.f1;_tmp8AD=_tmp8A3.f2;_LL763:;
if(!_tmp8AD)continue;{
struct Cyc_Absyn_Kind*_tmp8A4=Cyc_Tcutil_typ_kind(_tmp8AE);struct Cyc_Absyn_Kind*_tmp8A5=_tmp8A4;switch(((struct Cyc_Absyn_Kind*)_tmp8A5)->kind){case Cyc_Absyn_RgnKind: _LL765: _LL766:
# 4497
 effect=({struct Cyc_List_List*_tmp8A6=_cycalloc(sizeof(*_tmp8A6));_tmp8A6->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp8A7=_cycalloc(sizeof(*_tmp8A7));_tmp8A7[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp8A8;_tmp8A8.tag=23;_tmp8A8.f1=_tmp8AE;_tmp8A8;});_tmp8A7;});_tmp8A6->tl=effect;_tmp8A6;});goto _LL764;case Cyc_Absyn_EffKind: _LL767: _LL768:
# 4499
 effect=({struct Cyc_List_List*_tmp8A9=_cycalloc(sizeof(*_tmp8A9));_tmp8A9->hd=_tmp8AE;_tmp8A9->tl=effect;_tmp8A9;});goto _LL764;case Cyc_Absyn_IntKind: _LL769: _LL76A:
 goto _LL764;default: _LL76B: _LL76C:
# 4502
 effect=({struct Cyc_List_List*_tmp8AA=_cycalloc(sizeof(*_tmp8AA));_tmp8AA->hd=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp8AB=_cycalloc(sizeof(*_tmp8AB));_tmp8AB[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp8AC;_tmp8AC.tag=25;_tmp8AC.f1=_tmp8AE;_tmp8AC;});_tmp8AB;});_tmp8AA->tl=effect;_tmp8AA;});goto _LL764;}_LL764:;};}}
# 4505
*_tmp948=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp8AF=_cycalloc(sizeof(*_tmp8AF));_tmp8AF[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp8B0;_tmp8B0.tag=24;_tmp8B0.f1=effect;_tmp8B0;});_tmp8AF;});}
# 4511
if(*_tmp949 != 0){
struct Cyc_List_List*bs=*_tmp949;for(0;bs != 0;bs=bs->tl){
void*_tmp8B1=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);void*_tmp8B2=_tmp8B1;struct Cyc_Core_Opt**_tmp8C2;struct Cyc_Absyn_Kind*_tmp8C1;struct Cyc_Core_Opt**_tmp8C0;struct Cyc_Core_Opt**_tmp8BF;struct Cyc_Core_Opt**_tmp8BE;struct Cyc_Core_Opt**_tmp8BD;struct Cyc_Core_Opt**_tmp8BC;struct Cyc_Core_Opt**_tmp8BB;struct Cyc_Core_Opt**_tmp8BA;struct Cyc_Core_Opt**_tmp8B9;struct Cyc_Core_Opt**_tmp8B8;switch(*((int*)_tmp8B2)){case 1: _LL76E: _tmp8B8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp8B2)->f1;_LL76F:
# 4515
({struct Cyc_String_pa_PrintArg_struct _tmp8B5;_tmp8B5.tag=0;_tmp8B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name);({void*_tmp8B3[1]={& _tmp8B5};Cyc_Tcutil_warn(loc,({const char*_tmp8B4="Type variable %s unconstrained, assuming boxed";_tag_dyneither(_tmp8B4,sizeof(char),47);}),_tag_dyneither(_tmp8B3,sizeof(void*),1));});});
# 4517
_tmp8B9=_tmp8B8;goto _LL771;case 2: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B2)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B2)->f2)->aliasqual == Cyc_Absyn_Top){_LL770: _tmp8B9=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B2)->f1;_LL771:
 _tmp8BA=_tmp8B9;goto _LL773;}else{goto _LL780;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B2)->f2)->aliasqual){case Cyc_Absyn_Top: _LL772: _tmp8BA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B2)->f1;_LL773:
 _tmp8BB=_tmp8BA;goto _LL775;case Cyc_Absyn_Aliasable: _LL774: _tmp8BB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B2)->f1;_LL775:
 _tmp8BD=_tmp8BB;goto _LL777;default: _LL77A: _tmp8BC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B2)->f1;_LL77B:
# 4524
 _tmp8BF=_tmp8BC;goto _LL77D;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B2)->f2)->aliasqual){case Cyc_Absyn_Top: _LL776: _tmp8BD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B2)->f1;_LL777:
# 4521
 _tmp8BE=_tmp8BD;goto _LL779;case Cyc_Absyn_Aliasable: _LL778: _tmp8BE=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B2)->f1;_LL779:
# 4523
*_tmp8BE=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL76D;default: _LL77C: _tmp8BF=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B2)->f1;_LL77D:
# 4526
*_tmp8BF=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL76D;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B2)->f2)->aliasqual == Cyc_Absyn_Top){_LL77E: _tmp8C0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B2)->f1;_LL77F:
# 4528
*_tmp8C0=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL76D;}else{goto _LL780;}default: _LL780: _tmp8C2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B2)->f1;_tmp8C1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B2)->f2;_LL781:
# 4530
*_tmp8C2=Cyc_Tcutil_kind_to_bound_opt(_tmp8C1);goto _LL76D;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp8B2)->f1)->kind == Cyc_Absyn_MemKind){_LL782: _LL783:
# 4532
({void*_tmp8B6=0;Cyc_Tcutil_terr(loc,({const char*_tmp8B7="functions can't abstract M types";_tag_dyneither(_tmp8B7,sizeof(char),33);}),_tag_dyneither(_tmp8B6,sizeof(void*),0));});goto _LL76D;}else{_LL784: _LL785:
 goto _LL76D;}}_LL76D:;}}
# 4537
cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,_tmp842.kind_env,*_tmp949);
_tmp842.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(_tmp842.r,_tmp842.free_vars,*_tmp949);
# 4540
{struct Cyc_List_List*tvs=_tmp842.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp8C3=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp8C4=_tmp8C3;struct Cyc_Absyn_Tvar*_tmp8C6;int _tmp8C5;_LL787: _tmp8C6=_tmp8C4.f1;_tmp8C5=_tmp8C4.f2;_LL788:;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp8C6,_tmp8C5);}}{
# 4545
struct Cyc_List_List*evs=_tmp842.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmp8C7=*((struct _tuple29*)evs->hd);struct _tuple29 _tmp8C8=_tmp8C7;void*_tmp8CA;int _tmp8C9;_LL78A: _tmp8CA=_tmp8C8.f1;_tmp8C9=_tmp8C8.f2;_LL78B:;
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmp8CA,_tmp8C9);}};};};};}
# 4550
_npop_handler(0);goto _LL66E;
# 4325
;_pop_region(newr);};}case 10: _LL691: _tmp94A=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp787)->f1;_LL692:
# 4553
 for(0;_tmp94A != 0;_tmp94A=_tmp94A->tl){
struct _tuple12*_tmp8D6=(struct _tuple12*)_tmp94A->hd;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmp8D6).f2,1,0);
((*_tmp8D6).f1).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp8D6).f1).print_const,(*_tmp8D6).f2);}
# 4559
goto _LL66E;case 12: _LL693: _tmp94C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp787)->f1;_tmp94B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp787)->f2;_LL694: {
# 4563
struct _RegionHandle _tmp8D7=_new_region("aprev_rgn");struct _RegionHandle*aprev_rgn=& _tmp8D7;_push_region(aprev_rgn);{
struct Cyc_List_List*prev_fields=0;
for(0;_tmp94B != 0;_tmp94B=_tmp94B->tl){
struct Cyc_Absyn_Aggrfield*_tmp8D8=(struct Cyc_Absyn_Aggrfield*)_tmp94B->hd;struct Cyc_Absyn_Aggrfield*_tmp8D9=_tmp8D8;struct _dyneither_ptr*_tmp8EB;struct Cyc_Absyn_Tqual*_tmp8EA;void*_tmp8E9;struct Cyc_Absyn_Exp*_tmp8E8;struct Cyc_List_List*_tmp8E7;struct Cyc_Absyn_Exp*_tmp8E6;_LL78D: _tmp8EB=_tmp8D9->name;_tmp8EA=(struct Cyc_Absyn_Tqual*)& _tmp8D9->tq;_tmp8E9=_tmp8D9->type;_tmp8E8=_tmp8D9->width;_tmp8E7=_tmp8D9->attributes;_tmp8E6=_tmp8D9->requires_clause;_LL78E:;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp8EB))
({struct Cyc_String_pa_PrintArg_struct _tmp8DC;_tmp8DC.tag=0;_tmp8DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp8EB);({void*_tmp8DA[1]={& _tmp8DC};Cyc_Tcutil_terr(loc,({const char*_tmp8DB="duplicate field %s";_tag_dyneither(_tmp8DB,sizeof(char),19);}),_tag_dyneither(_tmp8DA,sizeof(void*),1));});});
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp8EB,({const char*_tmp8DD="";_tag_dyneither(_tmp8DD,sizeof(char),1);}))!= 0)
prev_fields=({struct Cyc_List_List*_tmp8DE=_region_malloc(aprev_rgn,sizeof(*_tmp8DE));_tmp8DE->hd=_tmp8EB;_tmp8DE->tl=prev_fields;_tmp8DE;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp8E9,1,0);
_tmp8EA->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp8EA->print_const,_tmp8E9);
# 4577
Cyc_Tcutil_check_bitfield(loc,te,_tmp8E9,_tmp8E8,_tmp8EB);
Cyc_Tcutil_check_field_atts(loc,_tmp8EB,_tmp8E7);
if(_tmp8E6 != 0){
# 4581
if(_tmp94C != Cyc_Absyn_UnionA)
({void*_tmp8DF=0;Cyc_Tcutil_terr(loc,({const char*_tmp8E0="@requires clause is only allowed on union members";_tag_dyneither(_tmp8E0,sizeof(char),50);}),_tag_dyneither(_tmp8DF,sizeof(void*),0));});{
struct _RegionHandle _tmp8E1=_new_region("temp");struct _RegionHandle*temp=& _tmp8E1;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp8E2=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp8E2,0,_tmp8E6);}
# 4587
if(!Cyc_Tcutil_is_integral(_tmp8E6))
({struct Cyc_String_pa_PrintArg_struct _tmp8E5;_tmp8E5.tag=0;_tmp8E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmp8E6->topt)));({void*_tmp8E3[1]={& _tmp8E5};Cyc_Tcutil_terr(loc,({const char*_tmp8E4="@requires clause has type %s instead of integral type";_tag_dyneither(_tmp8E4,sizeof(char),54);}),_tag_dyneither(_tmp8E3,sizeof(void*),1));});});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp8E6,te,cvtenv);
# 4583
;_pop_region(temp);};}}}
# 4594
_npop_handler(0);goto _LL66E;
# 4563
;_pop_region(aprev_rgn);}case 11: _LL695: _tmp94E=(union Cyc_Absyn_AggrInfoU*)&(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp787)->f1).aggr_info;_tmp94D=(struct Cyc_List_List**)&(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp787)->f1).targs;_LL696:
# 4597
{union Cyc_Absyn_AggrInfoU _tmp8EC=*_tmp94E;union Cyc_Absyn_AggrInfoU _tmp8ED=_tmp8EC;struct Cyc_Absyn_Aggrdecl*_tmp911;enum Cyc_Absyn_AggrKind _tmp910;struct _tuple2*_tmp90F;struct Cyc_Core_Opt*_tmp90E;if((_tmp8ED.UnknownAggr).tag == 1){_LL790: _tmp910=((_tmp8ED.UnknownAggr).val).f1;_tmp90F=((_tmp8ED.UnknownAggr).val).f2;_tmp90E=((_tmp8ED.UnknownAggr).val).f3;_LL791: {
# 4599
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmp8EE;_push_handler(& _tmp8EE);{int _tmp8F0=0;if(setjmp(_tmp8EE.handler))_tmp8F0=1;if(!_tmp8F0){
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp90F);{
struct Cyc_Absyn_Aggrdecl*_tmp8F1=*adp;
if(_tmp8F1->kind != _tmp910){
if(_tmp8F1->kind == Cyc_Absyn_StructA)
({struct Cyc_String_pa_PrintArg_struct _tmp8F5;_tmp8F5.tag=0;_tmp8F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp90F));({struct Cyc_String_pa_PrintArg_struct _tmp8F4;_tmp8F4.tag=0;_tmp8F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4605
Cyc_Absynpp_qvar2string(_tmp90F));({void*_tmp8F2[2]={& _tmp8F4,& _tmp8F5};Cyc_Tcutil_terr(loc,({const char*_tmp8F3="expecting struct %s instead of union %s";_tag_dyneither(_tmp8F3,sizeof(char),40);}),_tag_dyneither(_tmp8F2,sizeof(void*),2));});});});else{
# 4608
({struct Cyc_String_pa_PrintArg_struct _tmp8F9;_tmp8F9.tag=0;_tmp8F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp90F));({struct Cyc_String_pa_PrintArg_struct _tmp8F8;_tmp8F8.tag=0;_tmp8F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4608
Cyc_Absynpp_qvar2string(_tmp90F));({void*_tmp8F6[2]={& _tmp8F8,& _tmp8F9};Cyc_Tcutil_terr(loc,({const char*_tmp8F7="expecting union %s instead of struct %s";_tag_dyneither(_tmp8F7,sizeof(char),40);}),_tag_dyneither(_tmp8F6,sizeof(void*),2));});});});}}
# 4611
if((unsigned int)_tmp90E  && (int)_tmp90E->v){
if(!((unsigned int)_tmp8F1->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp8F1->impl))->tagged)
({struct Cyc_String_pa_PrintArg_struct _tmp8FC;_tmp8FC.tag=0;_tmp8FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp90F));({void*_tmp8FA[1]={& _tmp8FC};Cyc_Tcutil_terr(loc,({const char*_tmp8FB="@tagged qualfiers don't agree on union %s";_tag_dyneither(_tmp8FB,sizeof(char),42);}),_tag_dyneither(_tmp8FA,sizeof(void*),1));});});}
# 4617
*_tmp94E=Cyc_Absyn_KnownAggr(adp);};
# 4601
;_pop_handler();}else{void*_tmp8EF=(void*)_exn_thrown;void*_tmp8FD=_tmp8EF;void*_tmp903;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp8FD)->tag == Cyc_Dict_Absent){_LL795: _LL796: {
# 4621
struct Cyc_Absyn_Aggrdecl*_tmp8FE=({struct Cyc_Absyn_Aggrdecl*_tmp902=_cycalloc(sizeof(*_tmp902));_tmp902->kind=_tmp910;_tmp902->sc=Cyc_Absyn_Extern;_tmp902->name=_tmp90F;_tmp902->tvs=0;_tmp902->impl=0;_tmp902->attributes=0;_tmp902->expected_mem_kind=0;_tmp902;});
Cyc_Tc_tcAggrdecl(te,loc,_tmp8FE);
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp90F);
*_tmp94E=Cyc_Absyn_KnownAggr(adp);
# 4626
if(*_tmp94D != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp901;_tmp901.tag=0;_tmp901.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp90F));({void*_tmp8FF[1]={& _tmp901};Cyc_Tcutil_terr(loc,({const char*_tmp900="declare parameterized type %s before using";_tag_dyneither(_tmp900,sizeof(char),43);}),_tag_dyneither(_tmp8FF,sizeof(void*),1));});});
return cvtenv;}
# 4631
goto _LL794;}}else{_LL797: _tmp903=_tmp8FD;_LL798:(int)_rethrow(_tmp903);}_LL794:;}};}
# 4633
_tmp911=*adp;goto _LL793;}}else{_LL792: _tmp911=*(_tmp8ED.KnownAggr).val;_LL793: {
# 4635
struct Cyc_List_List*tvs=_tmp911->tvs;
struct Cyc_List_List*ts=*_tmp94D;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Tvar*_tmp904=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp905=(void*)ts->hd;
# 4643
{struct _tuple0 _tmp906=({struct _tuple0 _tmp909;_tmp909.f1=Cyc_Absyn_compress_kb(_tmp904->kind);_tmp909.f2=_tmp905;_tmp909;});struct _tuple0 _tmp907=_tmp906;struct Cyc_Absyn_Tvar*_tmp908;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp907.f1)->tag == 1){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp907.f2)->tag == 2){_LL79A: _tmp908=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp907.f2)->f1;_LL79B:
# 4645
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp908);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp908,1);
continue;}else{goto _LL79C;}}else{_LL79C: _LL79D:
 goto _LL799;}_LL799:;}{
# 4650
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 4654
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp90C;_tmp90C.tag=0;_tmp90C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp911->name));({void*_tmp90A[1]={& _tmp90C};Cyc_Tcutil_terr(loc,({const char*_tmp90B="too many parameters for type %s";_tag_dyneither(_tmp90B,sizeof(char),32);}),_tag_dyneither(_tmp90A,sizeof(void*),1));});});
if(tvs != 0){
# 4658
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp90D=_cycalloc(sizeof(*_tmp90D));_tmp90D->hd=e;_tmp90D->tl=hidden_ts;_tmp90D;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);}
# 4665
*_tmp94D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp94D,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4667
if((allow_abs_aggr  && _tmp911->impl == 0) && !
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,expected_kind))
# 4672
_tmp911->expected_mem_kind=1;}}_LL78F:;}
# 4675
goto _LL66E;case 17: _LL697: _tmp952=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp787)->f1;_tmp951=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp787)->f2;_tmp950=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp787)->f3;_tmp94F=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp787)->f4;_LL698: {
# 4678
struct Cyc_List_List*targs=*_tmp951;
# 4680
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmp912;_push_handler(& _tmp912);{int _tmp914=0;if(setjmp(_tmp912.handler))_tmp914=1;if(!_tmp914){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp952);;_pop_handler();}else{void*_tmp913=(void*)_exn_thrown;void*_tmp915=_tmp913;void*_tmp919;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp915)->tag == Cyc_Dict_Absent){_LL79F: _LL7A0:
# 4683
({struct Cyc_String_pa_PrintArg_struct _tmp918;_tmp918.tag=0;_tmp918.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp952));({void*_tmp916[1]={& _tmp918};Cyc_Tcutil_terr(loc,({const char*_tmp917="unbound typedef name %s";_tag_dyneither(_tmp917,sizeof(char),24);}),_tag_dyneither(_tmp916,sizeof(void*),1));});});
return cvtenv;}else{_LL7A1: _tmp919=_tmp915;_LL7A2:(int)_rethrow(_tmp919);}_LL79E:;}};}
# 4686
*_tmp950=td;{
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;
struct _RegionHandle _tmp91A=_new_region("temp");struct _RegionHandle*temp=& _tmp91A;_push_region(temp);{
struct Cyc_List_List*inst=0;
# 4692
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,td);
# 4697
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
inst=({struct Cyc_List_List*_tmp91B=_region_malloc(temp,sizeof(*_tmp91B));_tmp91B->hd=({struct _tuple16*_tmp91C=_region_malloc(temp,sizeof(*_tmp91C));_tmp91C->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp91C->f2=(void*)ts->hd;_tmp91C;});_tmp91B->tl=inst;_tmp91B;});}
# 4701
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp91F;_tmp91F.tag=0;_tmp91F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp952));({void*_tmp91D[1]={& _tmp91F};Cyc_Tcutil_terr(loc,({const char*_tmp91E="too many parameters for typedef %s";_tag_dyneither(_tmp91E,sizeof(char),35);}),_tag_dyneither(_tmp91D,sizeof(void*),1));});});
if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;
# 4706
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp920=_cycalloc(sizeof(*_tmp920));_tmp920->hd=e;_tmp920->tl=hidden_ts;_tmp920;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);
inst=({struct Cyc_List_List*_tmp921=_cycalloc(sizeof(*_tmp921));_tmp921->hd=({struct _tuple16*_tmp922=_cycalloc(sizeof(*_tmp922));_tmp922->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp922->f2=e;_tmp922;});_tmp921->tl=inst;_tmp921;});}
# 4714
*_tmp951=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4716
if(td->defn != 0){
void*new_typ=
inst == 0?(void*)_check_null(td->defn):
 Cyc_Tcutil_rsubstitute(temp,inst,(void*)_check_null(td->defn));
*_tmp94F=new_typ;}}
# 4723
_npop_handler(0);goto _LL66E;
# 4689
;_pop_region(temp);};}case 22: _LL699: _LL69A:
# 4724
 goto _LL69C;case 21: _LL69B: _LL69C:
 goto _LL69E;case 20: _LL69D: _LL69E:
 goto _LL66E;case 15: _LL69F: _tmp953=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp787)->f1;_LL6A0:
# 4728
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp953,1,allow_abs_aggr);goto _LL66E;case 16: _LL6A1: _tmp955=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp787)->f1;_tmp954=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp787)->f2;_LL6A2:
# 4731
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp955,0,0);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp954,1,0);
goto _LL66E;case 23: _LL6A3: _tmp956=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp787)->f1;_LL6A4:
# 4735
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp956,1,1);goto _LL66E;case 25: _LL6A5: _tmp957=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp787)->f1;_LL6A6:
# 4737
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp957,1,1);goto _LL66E;default: _LL6A7: _tmp958=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp787)->f1;_LL6A8:
# 4739
 for(0;_tmp958 != 0;_tmp958=_tmp958->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmp958->hd,1,1);}
goto _LL66E;}_LL66E:;}
# 4743
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind))
({struct Cyc_String_pa_PrintArg_struct _tmp95D;_tmp95D.tag=0;_tmp95D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(expected_kind));({struct Cyc_String_pa_PrintArg_struct _tmp95C;_tmp95C.tag=0;_tmp95C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t)));({struct Cyc_String_pa_PrintArg_struct _tmp95B;_tmp95B.tag=0;_tmp95B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp959[3]={& _tmp95B,& _tmp95C,& _tmp95D};Cyc_Tcutil_terr(loc,({const char*_tmp95A="type %s has kind %s but as used here needs kind %s";_tag_dyneither(_tmp95A,sizeof(char),51);}),_tag_dyneither(_tmp959,sizeof(void*),3));});});});});
# 4747
return cvtenv;}
# 4755
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){
# 4757
{void*_tmp95E=e->r;void*_tmp95F=_tmp95E;struct Cyc_Absyn_Exp*_tmp971;void*_tmp970;void*_tmp96F;void*_tmp96E;void*_tmp96D;struct Cyc_Absyn_Exp*_tmp96C;struct Cyc_Absyn_Exp*_tmp96B;struct Cyc_Absyn_Exp*_tmp96A;struct Cyc_Absyn_Exp*_tmp969;struct Cyc_Absyn_Exp*_tmp968;struct Cyc_Absyn_Exp*_tmp967;struct Cyc_Absyn_Exp*_tmp966;struct Cyc_Absyn_Exp*_tmp965;struct Cyc_Absyn_Exp*_tmp964;struct Cyc_Absyn_Exp*_tmp963;struct Cyc_List_List*_tmp962;switch(*((int*)_tmp95F)){case 0: _LL7A4: _LL7A5:
 goto _LL7A7;case 31: _LL7A6: _LL7A7:
 goto _LL7A9;case 32: _LL7A8: _LL7A9:
 goto _LL7AB;case 1: _LL7AA: _LL7AB:
 goto _LL7A3;case 2: _LL7AC: _tmp962=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp95F)->f2;_LL7AD:
# 4763
 for(0;_tmp962 != 0;_tmp962=_tmp962->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmp962->hd,te,cvtenv);}
goto _LL7A3;case 5: _LL7AE: _tmp965=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp95F)->f1;_tmp964=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp95F)->f2;_tmp963=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp95F)->f3;_LL7AF:
# 4767
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp965,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp964,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp963,te,cvtenv);
goto _LL7A3;case 6: _LL7B0: _tmp967=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp95F)->f1;_tmp966=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp95F)->f2;_LL7B1:
 _tmp969=_tmp967;_tmp968=_tmp966;goto _LL7B3;case 7: _LL7B2: _tmp969=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp95F)->f1;_tmp968=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp95F)->f2;_LL7B3:
 _tmp96B=_tmp969;_tmp96A=_tmp968;goto _LL7B5;case 8: _LL7B4: _tmp96B=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp95F)->f1;_tmp96A=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp95F)->f2;_LL7B5:
# 4774
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp96B,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp96A,te,cvtenv);
goto _LL7A3;case 13: _LL7B6: _tmp96D=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp95F)->f1;_tmp96C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp95F)->f2;_LL7B7:
# 4778
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp96C,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp96D,1,0);
goto _LL7A3;case 18: _LL7B8: _tmp96E=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp95F)->f1;_LL7B9:
 _tmp96F=_tmp96E;goto _LL7BB;case 16: _LL7BA: _tmp96F=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp95F)->f1;_LL7BB:
# 4783
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp96F,1,0);
goto _LL7A3;case 38: _LL7BC: _tmp970=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp95F)->f1;_LL7BD:
# 4786
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp970,1,0);
goto _LL7A3;case 17: _LL7BE: _tmp971=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp95F)->f1;_LL7BF:
# 4789
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp971,te,cvtenv);
goto _LL7A3;default: _LL7C0: _LL7C1:
# 4792
({void*_tmp960=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp961="non-type-level-expression in Tcutil::i_check_valid_type_level_exp";_tag_dyneither(_tmp961,sizeof(char),66);}),_tag_dyneither(_tmp960,sizeof(void*),0));});}_LL7A3:;}
# 4794
return cvtenv;}
# 4797
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,int allow_abs_aggr,void*t){
# 4803
struct Cyc_List_List*_tmp972=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1,allow_abs_aggr);
# 4806
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
struct _tuple28 _tmp973=*((struct _tuple28*)vs->hd);struct _tuple28 _tmp974=_tmp973;struct Cyc_Absyn_Tvar*_tmp975;_LL7C3: _tmp975=_tmp974.f1;_LL7C4:;
cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(_tmp972,_tmp975);}}
# 4814
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmp976=*((struct _tuple29*)evs->hd);struct _tuple29 _tmp977=_tmp976;void*_tmp980;_LL7C6: _tmp980=_tmp977.f1;_LL7C7:;{
void*_tmp978=Cyc_Tcutil_compress(_tmp980);void*_tmp979=_tmp978;struct Cyc_Core_Opt**_tmp97F;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp979)->tag == 1){_LL7C9: _tmp97F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp979)->f4;_LL7CA:
# 4818
 if(*_tmp97F == 0)
*_tmp97F=({struct Cyc_Core_Opt*_tmp97A=_cycalloc(sizeof(*_tmp97A));_tmp97A->v=_tmp972;_tmp97A;});else{
# 4822
struct Cyc_List_List*_tmp97B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp97F))->v;
struct Cyc_List_List*_tmp97C=0;
for(0;_tmp97B != 0;_tmp97B=_tmp97B->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp972,(struct Cyc_Absyn_Tvar*)_tmp97B->hd))
_tmp97C=({struct Cyc_List_List*_tmp97D=_cycalloc(sizeof(*_tmp97D));_tmp97D->hd=(struct Cyc_Absyn_Tvar*)_tmp97B->hd;_tmp97D->tl=_tmp97C;_tmp97D;});}
*_tmp97F=({struct Cyc_Core_Opt*_tmp97E=_cycalloc(sizeof(*_tmp97E));_tmp97E->v=_tmp97C;_tmp97E;});}
# 4829
goto _LL7C8;}else{_LL7CB: _LL7CC:
 goto _LL7C8;}_LL7C8:;};}}
# 4833
return cvt;}
# 4839
void Cyc_Tcutil_check_free_evars(struct Cyc_List_List*free_evars,void*in_t,unsigned int loc){
# 4841
for(0;free_evars != 0;free_evars=free_evars->tl){
void*e=(void*)free_evars->hd;
struct Cyc_Absyn_Kind*_tmp981=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind*_tmp982=_tmp981;switch(((struct Cyc_Absyn_Kind*)_tmp982)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp982)->aliasqual){case Cyc_Absyn_Unique: _LL7CE: _LL7CF:
# 4845
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val))
({void*_tmp983=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp984="can't unify evar with unique region!";_tag_dyneither(_tmp984,sizeof(char),37);}),_tag_dyneither(_tmp983,sizeof(void*),0));});
goto _LL7CD;case Cyc_Absyn_Aliasable: _LL7D0: _LL7D1:
 goto _LL7D3;default: _LL7D2: _LL7D3:
# 4850
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val))({void*_tmp985=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp986="can't unify evar with heap!";_tag_dyneither(_tmp986,sizeof(char),28);}),_tag_dyneither(_tmp985,sizeof(void*),0));});
goto _LL7CD;}case Cyc_Absyn_EffKind: _LL7D4: _LL7D5:
# 4853
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({void*_tmp987=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp988="can't unify evar with {}!";_tag_dyneither(_tmp988,sizeof(char),26);}),_tag_dyneither(_tmp987,sizeof(void*),0));});
goto _LL7CD;default: _LL7D6: _LL7D7:
# 4856
({struct Cyc_String_pa_PrintArg_struct _tmp98C;_tmp98C.tag=0;_tmp98C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(in_t));({struct Cyc_String_pa_PrintArg_struct _tmp98B;_tmp98B.tag=0;_tmp98B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e));({void*_tmp989[2]={& _tmp98B,& _tmp98C};Cyc_Tcutil_terr(loc,({const char*_tmp98A="hidden type variable %s isn't abstracted in type %s";_tag_dyneither(_tmp98A,sizeof(char),52);}),_tag_dyneither(_tmp989,sizeof(void*),2));});});});
goto _LL7CD;}_LL7CD:;}}
# 4868
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_List_List*_tmp98D=Cyc_Tcenv_lookup_type_vars(te);
struct Cyc_Absyn_Kind*expected_kind=generalize_evars?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
struct _RegionHandle _tmp98E=_new_region("temp");struct _RegionHandle*temp=& _tmp98E;_push_region(temp);
{struct Cyc_Tcutil_CVTEnv _tmp98F=Cyc_Tcutil_check_valid_type(loc,te,({struct Cyc_Tcutil_CVTEnv _tmp9AD;_tmp9AD.r=temp;_tmp9AD.kind_env=_tmp98D;_tmp9AD.free_vars=0;_tmp9AD.free_evars=0;_tmp9AD.generalize_evars=generalize_evars;_tmp9AD.fn_result=0;_tmp9AD;}),expected_kind,1,t);
# 4877
struct Cyc_List_List*_tmp990=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmp98F.free_vars);
struct Cyc_List_List*_tmp991=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmp98F.free_evars);
# 4881
if(_tmp98D != 0){
struct Cyc_List_List*_tmp992=0;
{struct Cyc_List_List*_tmp993=_tmp990;for(0;(unsigned int)_tmp993;_tmp993=_tmp993->tl){
struct Cyc_Absyn_Tvar*_tmp994=(struct Cyc_Absyn_Tvar*)_tmp993->hd;
int found=0;
{struct Cyc_List_List*_tmp995=_tmp98D;for(0;(unsigned int)_tmp995;_tmp995=_tmp995->tl){
if(Cyc_Absyn_tvar_cmp(_tmp994,(struct Cyc_Absyn_Tvar*)_tmp995->hd)== 0){found=1;break;}}}
if(!found)
_tmp992=({struct Cyc_List_List*_tmp996=_region_malloc(temp,sizeof(*_tmp996));_tmp996->hd=(struct Cyc_Absyn_Tvar*)_tmp993->hd;_tmp996->tl=_tmp992;_tmp996;});}}
# 4891
_tmp990=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp992);}
# 4897
{struct Cyc_List_List*x=_tmp990;for(0;x != 0;x=x->tl){
void*_tmp997=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);void*_tmp998=_tmp997;enum Cyc_Absyn_AliasQual _tmp9A6;struct Cyc_Core_Opt**_tmp9A5;struct Cyc_Absyn_Kind*_tmp9A4;struct Cyc_Core_Opt**_tmp9A3;struct Cyc_Core_Opt**_tmp9A2;struct Cyc_Core_Opt**_tmp9A1;struct Cyc_Core_Opt**_tmp9A0;struct Cyc_Core_Opt**_tmp99F;struct Cyc_Core_Opt**_tmp99E;switch(*((int*)_tmp998)){case 1: _LL7D9: _tmp99E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp998)->f1;_LL7DA:
 _tmp99F=_tmp99E;goto _LL7DC;case 2: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp998)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp998)->f2)->aliasqual == Cyc_Absyn_Top){_LL7DB: _tmp99F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp998)->f1;_LL7DC:
 _tmp9A0=_tmp99F;goto _LL7DE;}else{goto _LL7E5;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp998)->f2)->aliasqual){case Cyc_Absyn_Top: _LL7DD: _tmp9A0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp998)->f1;_LL7DE:
 _tmp9A1=_tmp9A0;goto _LL7E0;case Cyc_Absyn_Aliasable: _LL7DF: _tmp9A1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp998)->f1;_LL7E0:
# 4903
*_tmp9A1=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL7D8;default: _LL7E1: _tmp9A2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp998)->f1;_LL7E2:
# 4905
*_tmp9A2=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL7D8;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp998)->f2)->aliasqual == Cyc_Absyn_Top){_LL7E3: _tmp9A3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp998)->f1;_LL7E4:
# 4907
*_tmp9A3=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL7D8;}else{goto _LL7E5;}default: _LL7E5: _tmp9A5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp998)->f1;_tmp9A4=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp998)->f2;_LL7E6:
# 4909
*_tmp9A5=Cyc_Tcutil_kind_to_bound_opt(_tmp9A4);goto _LL7D8;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp998)->f1)->kind == Cyc_Absyn_MemKind){_LL7E7: _tmp9A6=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp998)->f1)->aliasqual;_LL7E8:
# 4911
({struct Cyc_String_pa_PrintArg_struct _tmp99C;_tmp99C.tag=0;_tmp99C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp99D=_cycalloc_atomic(sizeof(*_tmp99D));_tmp99D->kind=Cyc_Absyn_MemKind;_tmp99D->aliasqual=_tmp9A6;_tmp99D;})));({struct Cyc_String_pa_PrintArg_struct _tmp99B;_tmp99B.tag=0;_tmp99B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd));({void*_tmp999[2]={& _tmp99B,& _tmp99C};Cyc_Tcutil_terr(loc,({const char*_tmp99A="type variable %s cannot have kind %s";_tag_dyneither(_tmp99A,sizeof(char),37);}),_tag_dyneither(_tmp999,sizeof(void*),2));});});});
goto _LL7D8;}else{_LL7E9: _LL7EA:
 goto _LL7D8;}}_LL7D8:;}}
# 4918
if(_tmp990 != 0  || _tmp991 != 0){
{void*_tmp9A7=Cyc_Tcutil_compress(t);void*_tmp9A8=_tmp9A7;struct Cyc_List_List**_tmp9A9;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9A8)->tag == 9){_LL7EC: _tmp9A9=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9A8)->f1).tvars;_LL7ED:
# 4921
 if(*_tmp9A9 == 0){
# 4923
*_tmp9A9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmp990);
_tmp990=0;}
# 4926
goto _LL7EB;}else{_LL7EE: _LL7EF:
 goto _LL7EB;}_LL7EB:;}
# 4929
if(_tmp990 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp9AC;_tmp9AC.tag=0;_tmp9AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmp990->hd)->name);({void*_tmp9AA[1]={& _tmp9AC};Cyc_Tcutil_terr(loc,({const char*_tmp9AB="unbound type variable %s";_tag_dyneither(_tmp9AB,sizeof(char),25);}),_tag_dyneither(_tmp9AA,sizeof(void*),1));});});
Cyc_Tcutil_check_free_evars(_tmp991,t,loc);}}
# 4873
;_pop_region(temp);}
# 4940
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 4943
Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);
{void*_tmp9AE=Cyc_Tcutil_compress(t);void*_tmp9AF=_tmp9AE;struct Cyc_List_List*_tmp9BB;void*_tmp9BA;struct Cyc_Absyn_Tqual _tmp9B9;void*_tmp9B8;struct Cyc_List_List*_tmp9B7;struct Cyc_Absyn_Exp*_tmp9B6;struct Cyc_List_List*_tmp9B5;struct Cyc_Absyn_Exp*_tmp9B4;struct Cyc_List_List*_tmp9B3;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9AF)->tag == 9){_LL7F1: _tmp9BB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9AF)->f1).tvars;_tmp9BA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9AF)->f1).effect;_tmp9B9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9AF)->f1).ret_tqual;_tmp9B8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9AF)->f1).ret_typ;_tmp9B7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9AF)->f1).args;_tmp9B6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9AF)->f1).requires_clause;_tmp9B5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9AF)->f1).requires_relns;_tmp9B4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9AF)->f1).ensures_clause;_tmp9B3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9AF)->f1).ensures_relns;_LL7F2:
# 4946
 fd->tvs=_tmp9BB;
fd->effect=_tmp9BA;
{struct Cyc_List_List*_tmp9B0=fd->args;for(0;_tmp9B0 != 0;(_tmp9B0=_tmp9B0->tl,_tmp9B7=_tmp9B7->tl)){
# 4950
(*((struct _tuple10*)_tmp9B0->hd)).f2=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp9B7))->hd)).f2;
(*((struct _tuple10*)_tmp9B0->hd)).f3=(*((struct _tuple10*)_tmp9B7->hd)).f3;}}
# 4953
fd->ret_tqual=_tmp9B9;
fd->ret_type=_tmp9B8;
(fd->ret_tqual).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmp9B8);
fd->requires_clause=_tmp9B6;
fd->requires_relns=_tmp9B5;
fd->ensures_clause=_tmp9B4;
fd->ensures_relns=_tmp9B3;
goto _LL7F0;}else{_LL7F3: _LL7F4:
({void*_tmp9B1=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp9B2="check_fndecl_valid_type: not a FnType";_tag_dyneither(_tmp9B2,sizeof(char),38);}),_tag_dyneither(_tmp9B1,sizeof(void*),0));});}_LL7F0:;}{
# 4964
struct _RegionHandle _tmp9BC=_new_region("r");struct _RegionHandle*r=& _tmp9BC;_push_region(r);
Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _dyneither_ptr*(*)(struct _tuple10*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,({const char*_tmp9BD="function declaration has repeated parameter";_tag_dyneither(_tmp9BD,sizeof(char),44);}));
# 4969
fd->cached_typ=t;
# 4964
;_pop_region(r);};}
# 4974
void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,int allow_abs_aggr,void*t){
# 4980
struct _RegionHandle _tmp9BE=_new_region("r");struct _RegionHandle*r=& _tmp9BE;_push_region(r);
{struct Cyc_Tcutil_CVTEnv _tmp9BF=Cyc_Tcutil_check_valid_type(loc,te,({struct Cyc_Tcutil_CVTEnv _tmp9C7;_tmp9C7.r=r;_tmp9C7.kind_env=bound_tvars;_tmp9C7.free_vars=0;_tmp9C7.free_evars=0;_tmp9C7.generalize_evars=0;_tmp9C7.fn_result=0;_tmp9C7;}),expected_kind,allow_abs_aggr,t);
# 4984
struct Cyc_List_List*_tmp9C0=Cyc_Tcutil_remove_bound_tvars(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmp9BF.free_vars),bound_tvars);
# 4986
struct Cyc_List_List*_tmp9C1=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmp9BF.free_evars);
{struct Cyc_List_List*fs=_tmp9C0;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmp9C2=((struct Cyc_Absyn_Tvar*)fs->hd)->name;
({struct Cyc_String_pa_PrintArg_struct _tmp9C6;_tmp9C6.tag=0;_tmp9C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({struct Cyc_String_pa_PrintArg_struct _tmp9C5;_tmp9C5.tag=0;_tmp9C5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp9C2);({void*_tmp9C3[2]={& _tmp9C5,& _tmp9C6};Cyc_Tcutil_terr(loc,({const char*_tmp9C4="unbound type variable %s in type %s";_tag_dyneither(_tmp9C4,sizeof(char),36);}),_tag_dyneither(_tmp9C3,sizeof(void*),2));});});});}}
# 4991
if(!allow_evars)
Cyc_Tcutil_check_free_evars(_tmp9C1,t,loc);}
# 4981
;_pop_region(r);}
# 4996
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
tv->identity=Cyc_Tcutil_new_tvar_id();}
# 5001
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 5007
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){
# 5010
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0)
({struct Cyc_String_pa_PrintArg_struct _tmp9CB;_tmp9CB.tag=0;_tmp9CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string(vs->hd));({struct Cyc_String_pa_PrintArg_struct _tmp9CA;_tmp9CA.tag=0;_tmp9CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg);({void*_tmp9C8[2]={& _tmp9CA,& _tmp9CB};Cyc_Tcutil_terr(loc,({const char*_tmp9C9="%s: %s";_tag_dyneither(_tmp9C9,sizeof(char),7);}),_tag_dyneither(_tmp9C8,sizeof(void*),2));});});});}}}
# 5016
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s){
return*s;}
# 5020
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr msg){
((void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 5024
void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,({const char*_tmp9CC="duplicate type variable";_tag_dyneither(_tmp9CC,sizeof(char),24);}));}struct _tuple32{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple33{struct Cyc_List_List*f1;void*f2;};struct _tuple34{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 5038 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 5043
struct _RegionHandle _tmp9CD=_new_region("temp");struct _RegionHandle*temp=& _tmp9CD;_push_region(temp);
# 5047
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
if(Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,({const char*_tmp9CE="";_tag_dyneither(_tmp9CE,sizeof(char),1);}))!= 0)
fields=({struct Cyc_List_List*_tmp9CF=_region_malloc(temp,sizeof(*_tmp9CF));_tmp9CF->hd=({struct _tuple32*_tmp9D0=_region_malloc(temp,sizeof(*_tmp9D0));_tmp9D0->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd;_tmp9D0->f2=0;_tmp9D0;});_tmp9CF->tl=fields;_tmp9CF;});}}
# 5052
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 5054
struct _dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?({const char*_tmp9F6="struct";_tag_dyneither(_tmp9F6,sizeof(char),7);}):({const char*_tmp9F7="union";_tag_dyneither(_tmp9F7,sizeof(char),6);});
# 5057
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple33*_tmp9D1=(struct _tuple33*)des->hd;struct _tuple33*_tmp9D2=_tmp9D1;struct Cyc_List_List*_tmp9EE;void*_tmp9ED;_LL7F6: _tmp9EE=_tmp9D2->f1;_tmp9ED=_tmp9D2->f2;_LL7F7:;
if(_tmp9EE == 0){
# 5062
struct Cyc_List_List*_tmp9D3=fields;
for(0;_tmp9D3 != 0;_tmp9D3=_tmp9D3->tl){
if(!(*((struct _tuple32*)_tmp9D3->hd)).f2){
(*((struct _tuple32*)_tmp9D3->hd)).f2=1;
(*((struct _tuple33*)des->hd)).f1=({struct Cyc_List_List*_tmp9D4=_cycalloc(sizeof(*_tmp9D4));_tmp9D4->hd=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp9D5=_cycalloc(sizeof(*_tmp9D5));_tmp9D5[0]=({struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp9D6;_tmp9D6.tag=1;_tmp9D6.f1=((*((struct _tuple32*)_tmp9D3->hd)).f1)->name;_tmp9D6;});_tmp9D5;});_tmp9D4->tl=0;_tmp9D4;});
ans=({struct Cyc_List_List*_tmp9D7=_region_malloc(rgn,sizeof(*_tmp9D7));_tmp9D7->hd=({struct _tuple34*_tmp9D8=_region_malloc(rgn,sizeof(*_tmp9D8));_tmp9D8->f1=(*((struct _tuple32*)_tmp9D3->hd)).f1;_tmp9D8->f2=_tmp9ED;_tmp9D8;});_tmp9D7->tl=ans;_tmp9D7;});
break;}}
# 5070
if(_tmp9D3 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp9DB;_tmp9DB.tag=0;_tmp9DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);({void*_tmp9D9[1]={& _tmp9DB};Cyc_Tcutil_terr(loc,({const char*_tmp9DA="too many arguments to %s";_tag_dyneither(_tmp9DA,sizeof(char),25);}),_tag_dyneither(_tmp9D9,sizeof(void*),1));});});}else{
if(_tmp9EE->tl != 0)
# 5074
({void*_tmp9DC=0;Cyc_Tcutil_terr(loc,({const char*_tmp9DD="multiple designators are not yet supported";_tag_dyneither(_tmp9DD,sizeof(char),43);}),_tag_dyneither(_tmp9DC,sizeof(void*),0));});else{
# 5077
void*_tmp9DE=(void*)_tmp9EE->hd;void*_tmp9DF=_tmp9DE;struct _dyneither_ptr*_tmp9EC;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp9DF)->tag == 0){_LL7F9: _LL7FA:
# 5079
({struct Cyc_String_pa_PrintArg_struct _tmp9E2;_tmp9E2.tag=0;_tmp9E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);({void*_tmp9E0[1]={& _tmp9E2};Cyc_Tcutil_terr(loc,({const char*_tmp9E1="array designator used in argument to %s";_tag_dyneither(_tmp9E1,sizeof(char),40);}),_tag_dyneither(_tmp9E0,sizeof(void*),1));});});
goto _LL7F8;}else{_LL7FB: _tmp9EC=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp9DF)->f1;_LL7FC: {
# 5082
struct Cyc_List_List*_tmp9E3=fields;
for(0;_tmp9E3 != 0;_tmp9E3=_tmp9E3->tl){
if(Cyc_strptrcmp(_tmp9EC,((*((struct _tuple32*)_tmp9E3->hd)).f1)->name)== 0){
if((*((struct _tuple32*)_tmp9E3->hd)).f2)
({struct Cyc_String_pa_PrintArg_struct _tmp9E6;_tmp9E6.tag=0;_tmp9E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp9EC);({void*_tmp9E4[1]={& _tmp9E6};Cyc_Tcutil_terr(loc,({const char*_tmp9E5="member %s has already been used as an argument";_tag_dyneither(_tmp9E5,sizeof(char),47);}),_tag_dyneither(_tmp9E4,sizeof(void*),1));});});
(*((struct _tuple32*)_tmp9E3->hd)).f2=1;
ans=({struct Cyc_List_List*_tmp9E7=_region_malloc(rgn,sizeof(*_tmp9E7));_tmp9E7->hd=({struct _tuple34*_tmp9E8=_region_malloc(rgn,sizeof(*_tmp9E8));_tmp9E8->f1=(*((struct _tuple32*)_tmp9E3->hd)).f1;_tmp9E8->f2=_tmp9ED;_tmp9E8;});_tmp9E7->tl=ans;_tmp9E7;});
break;}}
# 5091
if(_tmp9E3 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp9EB;_tmp9EB.tag=0;_tmp9EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp9EC);({void*_tmp9E9[1]={& _tmp9EB};Cyc_Tcutil_terr(loc,({const char*_tmp9EA="bad field designator %s";_tag_dyneither(_tmp9EA,sizeof(char),24);}),_tag_dyneither(_tmp9E9,sizeof(void*),1));});});
goto _LL7F8;}}_LL7F8:;}}}
# 5096
if(aggr_kind == Cyc_Absyn_StructA)
# 5098
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple32*)fields->hd)).f2){
({void*_tmp9EF=0;Cyc_Tcutil_terr(loc,({const char*_tmp9F0="too few arguments to struct";_tag_dyneither(_tmp9F0,sizeof(char),28);}),_tag_dyneither(_tmp9EF,sizeof(void*),0));});
break;}}else{
# 5105
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple32*)fields->hd)).f2){
if(found)({void*_tmp9F1=0;Cyc_Tcutil_terr(loc,({const char*_tmp9F2="only one member of a union is allowed";_tag_dyneither(_tmp9F2,sizeof(char),38);}),_tag_dyneither(_tmp9F1,sizeof(void*),0));});
found=1;}}
# 5112
if(!found)({void*_tmp9F3=0;Cyc_Tcutil_terr(loc,({const char*_tmp9F4="missing member for union";_tag_dyneither(_tmp9F4,sizeof(char),25);}),_tag_dyneither(_tmp9F3,sizeof(void*),0));});}{
# 5115
struct Cyc_List_List*_tmp9F5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0);return _tmp9F5;};};}
# 5047
;_pop_region(temp);}
# 5118
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmp9F8=Cyc_Tcutil_compress(t);void*_tmp9F9=_tmp9F8;void*_tmp9FD;union Cyc_Absyn_Constraint*_tmp9FC;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9F9)->tag == 5){_LL7FE: _tmp9FD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9F9)->f1).elt_typ;_tmp9FC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9F9)->f1).ptr_atts).bounds;_LL7FF: {
# 5121
void*_tmp9FA=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp9FC);void*_tmp9FB=_tmp9FA;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp9FB)->tag == 0){_LL803: _LL804:
# 5123
*elt_typ_dest=_tmp9FD;
return 1;}else{_LL805: _LL806:
 return 0;}_LL802:;}}else{_LL800: _LL801:
# 5127
 return 0;}_LL7FD:;}
# 5131
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmp9FE=Cyc_Tcutil_compress(t);void*_tmp9FF=_tmp9FE;void*_tmpA01;union Cyc_Absyn_Constraint*_tmpA00;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9FF)->tag == 5){_LL808: _tmpA01=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9FF)->f1).elt_typ;_tmpA00=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9FF)->f1).ptr_atts).zero_term;_LL809:
# 5134
*elt_typ_dest=_tmpA01;
return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpA00);}else{_LL80A: _LL80B:
 return 0;}_LL807:;}
# 5142
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type){
# 5144
void*_tmpA02=Cyc_Tcutil_compress(t);void*_tmpA03=_tmpA02;void*_tmpA0C;struct Cyc_Absyn_Tqual _tmpA0B;struct Cyc_Absyn_Exp*_tmpA0A;union Cyc_Absyn_Constraint*_tmpA09;void*_tmpA08;union Cyc_Absyn_Constraint*_tmpA07;union Cyc_Absyn_Constraint*_tmpA06;switch(*((int*)_tmpA03)){case 5: _LL80D: _tmpA08=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA03)->f1).elt_typ;_tmpA07=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA03)->f1).ptr_atts).bounds;_tmpA06=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA03)->f1).ptr_atts).zero_term;_LL80E:
# 5146
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpA06)){
*ptr_type=t;
*elt_type=_tmpA08;
{void*_tmpA04=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmpA07);void*_tmpA05=_tmpA04;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpA05)->tag == 0){_LL814: _LL815:
*is_dyneither=1;goto _LL813;}else{_LL816: _LL817:
*is_dyneither=0;goto _LL813;}_LL813:;}
# 5153
return 1;}else{
return 0;}case 8: _LL80F: _tmpA0C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA03)->f1).elt_type;_tmpA0B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA03)->f1).tq;_tmpA0A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA03)->f1).num_elts;_tmpA09=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA03)->f1).zero_term;_LL810:
# 5156
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpA09)){
*elt_type=_tmpA0C;
*is_dyneither=0;
*ptr_type=Cyc_Tcutil_promote_array(t,(void*)& Cyc_Absyn_HeapRgn_val,0);
return 1;}else{
return 0;}default: _LL811: _LL812:
 return 0;}_LL80C:;}
# 5169
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){
# 5171
void*_tmpA0D=e1->r;void*_tmpA0E=_tmpA0D;struct Cyc_Absyn_Exp*_tmpA20;struct Cyc_Absyn_Exp*_tmpA1F;struct Cyc_Absyn_Exp*_tmpA1E;struct Cyc_Absyn_Exp*_tmpA1D;struct Cyc_Absyn_Exp*_tmpA1C;struct Cyc_Absyn_Exp*_tmpA1B;switch(*((int*)_tmpA0E)){case 13: _LL819: _LL81A:
# 5173
({struct Cyc_String_pa_PrintArg_struct _tmpA11;_tmpA11.tag=0;_tmpA11.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));({void*_tmpA0F[1]={& _tmpA11};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA10="we have a cast in a lhs:  %s";_tag_dyneither(_tmpA10,sizeof(char),29);}),_tag_dyneither(_tmpA0F,sizeof(void*),1));});});case 19: _LL81B: _tmpA1B=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpA0E)->f1;_LL81C:
 _tmpA1C=_tmpA1B;goto _LL81E;case 22: _LL81D: _tmpA1C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA0E)->f1;_LL81E:
# 5176
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpA1C->topt),ptr_type,is_dyneither,elt_type);case 21: _LL81F: _tmpA1D=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA0E)->f1;_LL820:
 _tmpA1E=_tmpA1D;goto _LL822;case 20: _LL821: _tmpA1E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA0E)->f1;_LL822:
# 5180
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpA1E->topt),ptr_type,is_dyneither,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmpA14;_tmpA14.tag=0;_tmpA14.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1));({void*_tmpA12[1]={& _tmpA14};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA13="found zero pointer aggregate member assignment: %s";_tag_dyneither(_tmpA13,sizeof(char),51);}),_tag_dyneither(_tmpA12,sizeof(void*),1));});});
return 0;case 12: _LL823: _tmpA1F=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpA0E)->f1;_LL824:
 _tmpA20=_tmpA1F;goto _LL826;case 11: _LL825: _tmpA20=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpA0E)->f1;_LL826:
# 5186
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpA20->topt),ptr_type,is_dyneither,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmpA17;_tmpA17.tag=0;_tmpA17.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1));({void*_tmpA15[1]={& _tmpA17};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA16="found zero pointer instantiate/noinstantiate: %s";_tag_dyneither(_tmpA16,sizeof(char),49);}),_tag_dyneither(_tmpA15,sizeof(void*),1));});});
return 0;case 1: _LL827: _LL828:
 return 0;default: _LL829: _LL82A:
({struct Cyc_String_pa_PrintArg_struct _tmpA1A;_tmpA1A.tag=0;_tmpA1A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));({void*_tmpA18[1]={& _tmpA1A};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA19="found bad lhs in is_zero_ptr_deref: %s";_tag_dyneither(_tmpA19,sizeof(char),39);}),_tag_dyneither(_tmpA18,sizeof(void*),1));});});}_LL818:;}
# 5195
int Cyc_Tcutil_is_tagged_pointer_typ(void*t){
void*ignore=(void*)& Cyc_Absyn_VoidType_val;
return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
# 5206
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
# 5209
void*_tmpA21=Cyc_Tcutil_compress(r);void*_tmpA22=_tmpA21;struct Cyc_Absyn_Tvar*_tmpA2F;enum Cyc_Absyn_KindQual _tmpA2E;enum Cyc_Absyn_AliasQual _tmpA2D;switch(*((int*)_tmpA22)){case 22: _LL82C: _LL82D:
 return !must_be_unique;case 21: _LL82E: _LL82F:
 return 1;case 17: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpA22)->f3 != 0){if(((struct Cyc_Absyn_Typedefdecl*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpA22)->f3)->kind != 0){if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpA22)->f4 == 0){_LL830: _tmpA2E=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpA22)->f3)->kind)->v)->kind;_tmpA2D=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpA22)->f3)->kind)->v)->aliasqual;_LL831:
# 5213
 return _tmpA2E == Cyc_Absyn_RgnKind  && (_tmpA2D == Cyc_Absyn_Unique  || _tmpA2D == Cyc_Absyn_Top  && !must_be_unique);}else{goto _LL834;}}else{goto _LL834;}}else{goto _LL834;}case 2: _LL832: _tmpA2F=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA22)->f1;_LL833: {
# 5217
struct Cyc_Absyn_Kind*_tmpA23=Cyc_Tcutil_tvar_kind(_tmpA2F,& Cyc_Tcutil_rk);struct Cyc_Absyn_Kind*_tmpA24=_tmpA23;enum Cyc_Absyn_KindQual _tmpA2C;enum Cyc_Absyn_AliasQual _tmpA2B;_LL837: _tmpA2C=_tmpA24->kind;_tmpA2B=_tmpA24->aliasqual;_LL838:;
if(_tmpA2C == Cyc_Absyn_RgnKind  && (_tmpA2B == Cyc_Absyn_Unique  || _tmpA2B == Cyc_Absyn_Top  && !must_be_unique)){
void*_tmpA25=Cyc_Absyn_compress_kb(_tmpA2F->kind);void*_tmpA26=_tmpA25;struct Cyc_Core_Opt**_tmpA2A;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA26)->tag == 2){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA26)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA26)->f2)->aliasqual == Cyc_Absyn_Top){_LL83A: _tmpA2A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA26)->f1;_LL83B:
# 5221
*_tmpA2A=({struct Cyc_Core_Opt*_tmpA27=_cycalloc(sizeof(*_tmpA27));_tmpA27->v=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA28=_cycalloc(sizeof(*_tmpA28));_tmpA28[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpA29;_tmpA29.tag=2;_tmpA29.f1=0;_tmpA29.f2=& Cyc_Tcutil_rk;_tmpA29;});_tmpA28;});_tmpA27;});
return 0;}else{goto _LL83C;}}else{goto _LL83C;}}else{_LL83C: _LL83D:
 return 1;}_LL839:;}
# 5226
return 0;}default: _LL834: _LL835:
 return 0;}_LL82B:;}
# 5233
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmpA30=Cyc_Tcutil_compress(t);void*_tmpA31=_tmpA30;struct Cyc_Absyn_Tvar*_tmpA40;void*_tmpA3F;switch(*((int*)_tmpA31)){case 5: _LL83F: _tmpA3F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA31)->f1).ptr_atts).rgn;_LL840:
# 5236
 return Cyc_Tcutil_is_noalias_region(_tmpA3F,must_be_unique);case 2: _LL841: _tmpA40=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA31)->f1;_LL842: {
# 5238
struct Cyc_Absyn_Kind*_tmpA32=Cyc_Tcutil_tvar_kind(_tmpA40,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmpA33=_tmpA32;enum Cyc_Absyn_KindQual _tmpA3E;enum Cyc_Absyn_AliasQual _tmpA3D;_LL846: _tmpA3E=_tmpA33->kind;_tmpA3D=_tmpA33->aliasqual;_LL847:;{
enum Cyc_Absyn_KindQual _tmpA34=_tmpA3E;switch(_tmpA34){case Cyc_Absyn_BoxKind: _LL849: _LL84A:
 goto _LL84C;case Cyc_Absyn_AnyKind: _LL84B: _LL84C: goto _LL84E;case Cyc_Absyn_MemKind: _LL84D: _LL84E:
 if(_tmpA3D == Cyc_Absyn_Unique  || _tmpA3D == Cyc_Absyn_Top  && !must_be_unique){
void*_tmpA35=Cyc_Absyn_compress_kb(_tmpA40->kind);void*_tmpA36=_tmpA35;struct Cyc_Core_Opt**_tmpA3C;enum Cyc_Absyn_KindQual _tmpA3B;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA36)->tag == 2){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA36)->f2)->aliasqual == Cyc_Absyn_Top){_LL852: _tmpA3C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA36)->f1;_tmpA3B=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA36)->f2)->kind;_LL853:
# 5244
*_tmpA3C=({struct Cyc_Core_Opt*_tmpA37=_cycalloc(sizeof(*_tmpA37));_tmpA37->v=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA38=_cycalloc(sizeof(*_tmpA38));_tmpA38[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpA39;_tmpA39.tag=2;_tmpA39.f1=0;_tmpA39.f2=({struct Cyc_Absyn_Kind*_tmpA3A=_cycalloc_atomic(sizeof(*_tmpA3A));_tmpA3A->kind=_tmpA3B;_tmpA3A->aliasqual=Cyc_Absyn_Aliasable;_tmpA3A;});_tmpA39;});_tmpA38;});_tmpA37;});
return 0;}else{goto _LL854;}}else{_LL854: _LL855:
# 5248
 return 1;}_LL851:;}
# 5251
return 0;default: _LL84F: _LL850:
 return 0;}_LL848:;};}default: _LL843: _LL844:
# 5254
 return 0;}_LL83E:;}
# 5257
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*rgn,void*t){
void*_tmpA41=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(_tmpA41,0))return 1;{
void*_tmpA42=_tmpA41;union Cyc_Absyn_DatatypeFieldInfoU _tmpA56;struct Cyc_List_List*_tmpA55;union Cyc_Absyn_DatatypeInfoU _tmpA54;struct Cyc_List_List*_tmpA53;struct Cyc_List_List*_tmpA52;struct Cyc_Absyn_Aggrdecl**_tmpA51;struct Cyc_List_List*_tmpA50;struct Cyc_List_List*_tmpA4F;switch(*((int*)_tmpA42)){case 10: _LL857: _tmpA4F=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA42)->f1;_LL858:
# 5262
 while(_tmpA4F != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(*((struct _tuple12*)_tmpA4F->hd)).f2))return 1;
_tmpA4F=_tmpA4F->tl;}
# 5266
return 0;case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA42)->f1).aggr_info).KnownAggr).tag == 2){_LL859: _tmpA51=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA42)->f1).aggr_info).KnownAggr).val;_tmpA50=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA42)->f1).targs;_LL85A:
# 5268
 if((*_tmpA51)->impl == 0)return 0;else{
# 5270
struct Cyc_List_List*_tmpA43=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,(*_tmpA51)->tvs,_tmpA50);
struct Cyc_List_List*_tmpA44=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpA51)->impl))->fields;
void*t;
while(_tmpA44 != 0){
t=_tmpA43 == 0?((struct Cyc_Absyn_Aggrfield*)_tmpA44->hd)->type: Cyc_Tcutil_rsubstitute(rgn,_tmpA43,((struct Cyc_Absyn_Aggrfield*)_tmpA44->hd)->type);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpA44=_tmpA44->tl;}
# 5278
return 0;}}else{_LL85D: _LL85E:
# 5288
 return 0;}case 12: _LL85B: _tmpA52=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA42)->f2;_LL85C:
# 5281
 while(_tmpA52 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,((struct Cyc_Absyn_Aggrfield*)_tmpA52->hd)->type))return 1;
_tmpA52=_tmpA52->tl;}
# 5285
return 0;case 3: _LL85F: _tmpA54=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA42)->f1).datatype_info;_tmpA53=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA42)->f1).targs;_LL860: {
# 5290
union Cyc_Absyn_DatatypeInfoU _tmpA45=_tmpA54;struct Cyc_List_List*_tmpA49;struct Cyc_Core_Opt*_tmpA48;struct _tuple2*_tmpA47;int _tmpA46;if((_tmpA45.UnknownDatatype).tag == 1){_LL866: _tmpA47=((_tmpA45.UnknownDatatype).val).name;_tmpA46=((_tmpA45.UnknownDatatype).val).is_extensible;_LL867:
# 5293
 return 0;}else{_LL868: _tmpA49=(*(_tmpA45.KnownDatatype).val)->tvs;_tmpA48=(*(_tmpA45.KnownDatatype).val)->fields;_LL869:
# 5296
 return 0;}_LL865:;}case 4: _LL861: _tmpA56=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpA42)->f1).field_info;_tmpA55=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpA42)->f1).targs;_LL862: {
# 5299
union Cyc_Absyn_DatatypeFieldInfoU _tmpA4A=_tmpA56;struct Cyc_Absyn_Datatypedecl*_tmpA4E;struct Cyc_Absyn_Datatypefield*_tmpA4D;if((_tmpA4A.UnknownDatatypefield).tag == 1){_LL86B: _LL86C:
# 5302
 return 0;}else{_LL86D: _tmpA4E=((_tmpA4A.KnownDatatypefield).val).f1;_tmpA4D=((_tmpA4A.KnownDatatypefield).val).f2;_LL86E: {
# 5304
struct Cyc_List_List*_tmpA4B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpA4E->tvs,_tmpA55);
struct Cyc_List_List*_tmpA4C=_tmpA4D->typs;
while(_tmpA4C != 0){
_tmpA41=_tmpA4B == 0?(*((struct _tuple12*)_tmpA4C->hd)).f2: Cyc_Tcutil_rsubstitute(rgn,_tmpA4B,(*((struct _tuple12*)_tmpA4C->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,_tmpA41))return 1;
_tmpA4C=_tmpA4C->tl;}
# 5311
return 0;}}_LL86A:;}default: _LL863: _LL864:
# 5313
 return 0;}_LL856:;};}
# 5317
static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,struct _dyneither_ptr*f){
void*_tmpA57=Cyc_Tcutil_compress(t);void*_tmpA58=_tmpA57;struct Cyc_List_List*_tmpA63;struct Cyc_Absyn_Aggrdecl*_tmpA62;struct Cyc_List_List*_tmpA61;switch(*((int*)_tmpA58)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA58)->f1).aggr_info).KnownAggr).tag == 2){_LL870: _tmpA62=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA58)->f1).aggr_info).KnownAggr).val;_tmpA61=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA58)->f1).targs;_LL871:
# 5320
 _tmpA63=_tmpA62->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpA62->impl))->fields;goto _LL873;}else{goto _LL874;}case 12: _LL872: _tmpA63=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA58)->f2;_LL873: {
# 5322
struct Cyc_Absyn_Aggrfield*_tmpA59=Cyc_Absyn_lookup_field(_tmpA63,f);
{struct Cyc_Absyn_Aggrfield*_tmpA5A=_tmpA59;void*_tmpA5D;if(_tmpA5A == 0){_LL877: _LL878:
({void*_tmpA5B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA5C="is_noalias_field: missing field";_tag_dyneither(_tmpA5C,sizeof(char),32);}),_tag_dyneither(_tmpA5B,sizeof(void*),0));});}else{_LL879: _tmpA5D=_tmpA5A->type;_LL87A:
# 5326
 return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpA5D);}_LL876:;}
# 5328
return 0;}default: _LL874: _LL875:
# 5330
({struct Cyc_String_pa_PrintArg_struct _tmpA60;_tmpA60.tag=0;_tmpA60.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));({void*_tmpA5E[1]={& _tmpA60};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA5F="is_noalias_field: invalid type |%s|";_tag_dyneither(_tmpA5F,sizeof(char),36);}),_tag_dyneither(_tmpA5E,sizeof(void*),1));});});}_LL86F:;}
# 5338
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
void*_tmpA64=e->r;void*_tmpA65=_tmpA64;struct Cyc_Absyn_Stmt*_tmpA7B;struct Cyc_Absyn_Exp*_tmpA7A;struct Cyc_Absyn_Exp*_tmpA79;struct Cyc_Absyn_Exp*_tmpA78;struct Cyc_Absyn_Exp*_tmpA77;struct Cyc_Absyn_Exp*_tmpA76;struct Cyc_Absyn_Exp*_tmpA75;struct Cyc_Absyn_Exp*_tmpA74;struct _dyneither_ptr*_tmpA73;struct Cyc_Absyn_Exp*_tmpA72;struct Cyc_Absyn_Exp*_tmpA71;switch(*((int*)_tmpA65)){case 1: if(((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA65)->f1)->tag == 1){_LL87C: _LL87D:
 return 0;}else{goto _LL88E;}case 21: _LL87E: _tmpA71=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA65)->f1;_LL87F:
 _tmpA72=_tmpA71;goto _LL881;case 19: _LL880: _tmpA72=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpA65)->f1;_LL881:
# 5343
 return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmpA72->topt),1) && 
Cyc_Tcutil_is_noalias_path(r,_tmpA72);case 20: _LL882: _tmpA74=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA65)->f1;_tmpA73=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA65)->f2;_LL883:
 return Cyc_Tcutil_is_noalias_path(r,_tmpA74);case 22: _LL884: _tmpA76=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA65)->f1;_tmpA75=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA65)->f2;_LL885: {
# 5347
void*_tmpA66=Cyc_Tcutil_compress((void*)_check_null(_tmpA76->topt));void*_tmpA67=_tmpA66;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA67)->tag == 10){_LL891: _LL892:
 return Cyc_Tcutil_is_noalias_path(r,_tmpA76);}else{_LL893: _LL894:
 return 0;}_LL890:;}case 5: _LL886: _tmpA78=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA65)->f2;_tmpA77=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA65)->f3;_LL887:
# 5352
 return Cyc_Tcutil_is_noalias_path(r,_tmpA78) && Cyc_Tcutil_is_noalias_path(r,_tmpA77);case 8: _LL888: _tmpA79=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpA65)->f2;_LL889:
 _tmpA7A=_tmpA79;goto _LL88B;case 13: _LL88A: _tmpA7A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA65)->f2;_LL88B:
 return Cyc_Tcutil_is_noalias_path(r,_tmpA7A);case 36: _LL88C: _tmpA7B=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpA65)->f1;_LL88D:
# 5356
 while(1){
void*_tmpA68=_tmpA7B->r;void*_tmpA69=_tmpA68;struct Cyc_Absyn_Exp*_tmpA70;struct Cyc_Absyn_Decl*_tmpA6F;struct Cyc_Absyn_Stmt*_tmpA6E;struct Cyc_Absyn_Stmt*_tmpA6D;struct Cyc_Absyn_Stmt*_tmpA6C;switch(*((int*)_tmpA69)){case 2: _LL896: _tmpA6D=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpA69)->f1;_tmpA6C=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpA69)->f2;_LL897:
 _tmpA7B=_tmpA6C;goto _LL895;case 12: _LL898: _tmpA6F=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpA69)->f1;_tmpA6E=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpA69)->f2;_LL899:
 _tmpA7B=_tmpA6E;goto _LL895;case 1: _LL89A: _tmpA70=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpA69)->f1;_LL89B:
 return Cyc_Tcutil_is_noalias_path(r,_tmpA70);default: _LL89C: _LL89D:
({void*_tmpA6A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA6B="is_noalias_stmt_exp: ill-formed StmtExp";_tag_dyneither(_tmpA6B,sizeof(char),40);}),_tag_dyneither(_tmpA6A,sizeof(void*),0));});}_LL895:;}default: _LL88E: _LL88F:
# 5364
 return 1;}_LL87B:;}
# 5381 "tcutil.cyc"
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5383
struct _tuple18 bogus_ans=({struct _tuple18 _tmpAC3;_tmpAC3.f1=0;_tmpAC3.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpAC3;});
void*_tmpA7C=e->r;void*_tmpA7D=_tmpA7C;struct Cyc_Absyn_Exp*_tmpAC2;struct Cyc_Absyn_Exp*_tmpAC1;struct Cyc_Absyn_Exp*_tmpAC0;struct Cyc_Absyn_Exp*_tmpABF;struct _dyneither_ptr*_tmpABE;int _tmpABD;struct Cyc_Absyn_Exp*_tmpABC;struct _dyneither_ptr*_tmpABB;int _tmpABA;void*_tmpAB9;switch(*((int*)_tmpA7D)){case 1: _LL89F: _tmpAB9=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA7D)->f1;_LL8A0: {
# 5387
void*_tmpA7E=_tmpAB9;struct Cyc_Absyn_Vardecl*_tmpA8B;struct Cyc_Absyn_Vardecl*_tmpA8A;struct Cyc_Absyn_Vardecl*_tmpA89;struct Cyc_Absyn_Vardecl*_tmpA88;switch(*((int*)_tmpA7E)){case 0: _LL8AC: _LL8AD:
 goto _LL8AF;case 2: _LL8AE: _LL8AF:
 return bogus_ans;case 1: _LL8B0: _tmpA88=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpA7E)->f1;_LL8B1: {
# 5391
void*_tmpA7F=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpA80=_tmpA7F;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA80)->tag == 8){_LL8B9: _LL8BA:
# 5393
 return({struct _tuple18 _tmpA81;_tmpA81.f1=1;_tmpA81.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpA81;});}else{_LL8BB: _LL8BC:
 return({struct _tuple18 _tmpA82;_tmpA82.f1=(_tmpA88->tq).real_const;_tmpA82.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpA82;});}_LL8B8:;}case 4: _LL8B2: _tmpA89=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpA7E)->f1;_LL8B3: {
# 5397
void*_tmpA83=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpA84=_tmpA83;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA84)->tag == 8){_LL8BE: _LL8BF:
 return({struct _tuple18 _tmpA85;_tmpA85.f1=1;_tmpA85.f2=(void*)_check_null(_tmpA89->rgn);_tmpA85;});}else{_LL8C0: _LL8C1:
# 5400
 _tmpA89->escapes=1;
return({struct _tuple18 _tmpA86;_tmpA86.f1=(_tmpA89->tq).real_const;_tmpA86.f2=(void*)_check_null(_tmpA89->rgn);_tmpA86;});}_LL8BD:;}case 5: _LL8B4: _tmpA8A=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpA7E)->f1;_LL8B5:
# 5403
 _tmpA8B=_tmpA8A;goto _LL8B7;default: _LL8B6: _tmpA8B=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpA7E)->f1;_LL8B7:
# 5405
 _tmpA8B->escapes=1;
return({struct _tuple18 _tmpA87;_tmpA87.f1=(_tmpA8B->tq).real_const;_tmpA87.f2=(void*)_check_null(_tmpA8B->rgn);_tmpA87;});}_LL8AB:;}case 20: _LL8A1: _tmpABC=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA7D)->f1;_tmpABB=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA7D)->f2;_tmpABA=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA7D)->f3;_LL8A2:
# 5410
 if(_tmpABA)return bogus_ans;{
# 5413
void*_tmpA8C=Cyc_Tcutil_compress((void*)_check_null(_tmpABC->topt));void*_tmpA8D=_tmpA8C;struct Cyc_Absyn_Aggrdecl*_tmpA9B;struct Cyc_List_List*_tmpA9A;switch(*((int*)_tmpA8D)){case 12: _LL8C3: _tmpA9A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA8D)->f2;_LL8C4: {
# 5415
struct Cyc_Absyn_Aggrfield*_tmpA8E=Cyc_Absyn_lookup_field(_tmpA9A,_tmpABB);
if(_tmpA8E != 0  && _tmpA8E->width == 0){
struct _tuple18 _tmpA8F=Cyc_Tcutil_addressof_props(te,_tmpABC);struct _tuple18 _tmpA90=_tmpA8F;int _tmpA93;void*_tmpA92;_LL8CA: _tmpA93=_tmpA90.f1;_tmpA92=_tmpA90.f2;_LL8CB:;
return({struct _tuple18 _tmpA91;_tmpA91.f1=(_tmpA8E->tq).real_const  || _tmpA93;_tmpA91.f2=_tmpA92;_tmpA91;});}
# 5420
return bogus_ans;}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA8D)->f1).aggr_info).KnownAggr).tag == 2){_LL8C5: _tmpA9B=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA8D)->f1).aggr_info).KnownAggr).val;_LL8C6: {
# 5422
struct Cyc_Absyn_Aggrfield*_tmpA94=Cyc_Absyn_lookup_decl_field(_tmpA9B,_tmpABB);
if(_tmpA94 != 0  && _tmpA94->width == 0){
struct _tuple18 _tmpA95=Cyc_Tcutil_addressof_props(te,_tmpABC);struct _tuple18 _tmpA96=_tmpA95;int _tmpA99;void*_tmpA98;_LL8CD: _tmpA99=_tmpA96.f1;_tmpA98=_tmpA96.f2;_LL8CE:;
return({struct _tuple18 _tmpA97;_tmpA97.f1=(_tmpA94->tq).real_const  || _tmpA99;_tmpA97.f2=_tmpA98;_tmpA97;});}
# 5427
return bogus_ans;}}else{goto _LL8C7;}default: _LL8C7: _LL8C8:
 return bogus_ans;}_LL8C2:;};case 21: _LL8A3: _tmpABF=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA7D)->f1;_tmpABE=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA7D)->f2;_tmpABD=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA7D)->f3;_LL8A4:
# 5432
 if(_tmpABD)return bogus_ans;{
# 5436
void*_tmpA9C=Cyc_Tcutil_compress((void*)_check_null(_tmpABF->topt));void*_tmpA9D=_tmpA9C;void*_tmpAA4;void*_tmpAA3;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA9D)->tag == 5){_LL8D0: _tmpAA4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA9D)->f1).elt_typ;_tmpAA3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA9D)->f1).ptr_atts).rgn;_LL8D1: {
# 5438
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmpA9E=Cyc_Tcutil_compress(_tmpAA4);void*_tmpA9F=_tmpA9E;struct Cyc_Absyn_Aggrdecl*_tmpAA1;struct Cyc_List_List*_tmpAA0;switch(*((int*)_tmpA9F)){case 12: _LL8D5: _tmpAA0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA9F)->f2;_LL8D6:
# 5441
 finfo=Cyc_Absyn_lookup_field(_tmpAA0,_tmpABE);goto _LL8D4;case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA9F)->f1).aggr_info).KnownAggr).tag == 2){_LL8D7: _tmpAA1=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA9F)->f1).aggr_info).KnownAggr).val;_LL8D8:
# 5443
 finfo=Cyc_Absyn_lookup_decl_field(_tmpAA1,_tmpABE);goto _LL8D4;}else{goto _LL8D9;}default: _LL8D9: _LL8DA:
 return bogus_ans;}_LL8D4:;}
# 5446
if(finfo != 0  && finfo->width == 0)
return({struct _tuple18 _tmpAA2;_tmpAA2.f1=(finfo->tq).real_const;_tmpAA2.f2=_tmpAA3;_tmpAA2;});
return bogus_ans;}}else{_LL8D2: _LL8D3:
 return bogus_ans;}_LL8CF:;};case 19: _LL8A5: _tmpAC0=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpA7D)->f1;_LL8A6: {
# 5453
void*_tmpAA5=Cyc_Tcutil_compress((void*)_check_null(_tmpAC0->topt));void*_tmpAA6=_tmpAA5;struct Cyc_Absyn_Tqual _tmpAA9;void*_tmpAA8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAA6)->tag == 5){_LL8DC: _tmpAA9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAA6)->f1).elt_tq;_tmpAA8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAA6)->f1).ptr_atts).rgn;_LL8DD:
# 5455
 return({struct _tuple18 _tmpAA7;_tmpAA7.f1=_tmpAA9.real_const;_tmpAA7.f2=_tmpAA8;_tmpAA7;});}else{_LL8DE: _LL8DF:
 return bogus_ans;}_LL8DB:;}case 22: _LL8A7: _tmpAC2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA7D)->f1;_tmpAC1=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA7D)->f2;_LL8A8: {
# 5461
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmpAC2->topt));
void*_tmpAAA=t;struct Cyc_Absyn_Tqual _tmpAB6;struct Cyc_Absyn_Tqual _tmpAB5;void*_tmpAB4;struct Cyc_List_List*_tmpAB3;switch(*((int*)_tmpAAA)){case 10: _LL8E1: _tmpAB3=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpAAA)->f1;_LL8E2: {
# 5465
struct _tuple14 _tmpAAB=Cyc_Evexp_eval_const_uint_exp(_tmpAC1);struct _tuple14 _tmpAAC=_tmpAAB;unsigned int _tmpAB0;int _tmpAAF;_LL8EA: _tmpAB0=_tmpAAC.f1;_tmpAAF=_tmpAAC.f2;_LL8EB:;
if(!_tmpAAF)
return bogus_ans;{
struct _tuple12*_tmpAAD=Cyc_Absyn_lookup_tuple_field(_tmpAB3,(int)_tmpAB0);
if(_tmpAAD != 0)
return({struct _tuple18 _tmpAAE;_tmpAAE.f1=((*_tmpAAD).f1).real_const;_tmpAAE.f2=(Cyc_Tcutil_addressof_props(te,_tmpAC2)).f2;_tmpAAE;});
return bogus_ans;};}case 5: _LL8E3: _tmpAB5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAAA)->f1).elt_tq;_tmpAB4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAAA)->f1).ptr_atts).rgn;_LL8E4:
# 5473
 return({struct _tuple18 _tmpAB1;_tmpAB1.f1=_tmpAB5.real_const;_tmpAB1.f2=_tmpAB4;_tmpAB1;});case 8: _LL8E5: _tmpAB6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAAA)->f1).tq;_LL8E6:
# 5479
 return({struct _tuple18 _tmpAB2;_tmpAB2.f1=_tmpAB6.real_const;_tmpAB2.f2=(Cyc_Tcutil_addressof_props(te,_tmpAC2)).f2;_tmpAB2;});default: _LL8E7: _LL8E8:
 return bogus_ans;}_LL8E0:;}default: _LL8A9: _LL8AA:
# 5483
({void*_tmpAB7=0;Cyc_Tcutil_terr(e->loc,({const char*_tmpAB8="unary & applied to non-lvalue";_tag_dyneither(_tmpAB8,sizeof(char),30);}),_tag_dyneither(_tmpAB7,sizeof(void*),0));});
return bogus_ans;}_LL89E:;}
# 5490
void Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,union Cyc_Absyn_Constraint*b){
b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(b);{
void*_tmpAC4=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);void*_tmpAC5=_tmpAC4;struct Cyc_Absyn_Exp*_tmpACE;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpAC5)->tag == 0){_LL8ED: _LL8EE:
 return;}else{_LL8EF: _tmpACE=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpAC5)->f1;_LL8F0: {
# 5495
struct _tuple14 _tmpAC6=Cyc_Evexp_eval_const_uint_exp(_tmpACE);struct _tuple14 _tmpAC7=_tmpAC6;unsigned int _tmpACD;int _tmpACC;_LL8F2: _tmpACD=_tmpAC7.f1;_tmpACC=_tmpAC7.f2;_LL8F3:;
if(_tmpACC  && _tmpACD <= i)
({struct Cyc_Int_pa_PrintArg_struct _tmpACB;_tmpACB.tag=1;_tmpACB.f1=(unsigned long)((int)i);({struct Cyc_Int_pa_PrintArg_struct _tmpACA;_tmpACA.tag=1;_tmpACA.f1=(unsigned long)((int)_tmpACD);({void*_tmpAC8[2]={& _tmpACA,& _tmpACB};Cyc_Tcutil_terr(loc,({const char*_tmpAC9="dereference is out of bounds: %d <= %d";_tag_dyneither(_tmpAC9,sizeof(char),39);}),_tag_dyneither(_tmpAC8,sizeof(void*),2));});});});
return;}}_LL8EC:;};}
# 5502
void Cyc_Tcutil_check_nonzero_bound(unsigned int loc,union Cyc_Absyn_Constraint*b){
Cyc_Tcutil_check_bound(loc,0,b);}
# 5506
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b){
void*_tmpACF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);void*_tmpAD0=_tmpACF;struct Cyc_Absyn_Exp*_tmpAD5;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpAD0)->tag == 1){_LL8F5: _tmpAD5=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpAD0)->f1;_LL8F6: {
# 5509
struct _tuple14 _tmpAD1=Cyc_Evexp_eval_const_uint_exp(_tmpAD5);struct _tuple14 _tmpAD2=_tmpAD1;unsigned int _tmpAD4;int _tmpAD3;_LL8FA: _tmpAD4=_tmpAD2.f1;_tmpAD3=_tmpAD2.f2;_LL8FB:;
return _tmpAD3  && _tmpAD4 == 1;}}else{_LL8F7: _LL8F8:
 return 0;}_LL8F4:;}
# 5515
int Cyc_Tcutil_bits_only(void*t){
void*_tmpAD6=Cyc_Tcutil_compress(t);void*_tmpAD7=_tmpAD6;struct Cyc_List_List*_tmpAE1;struct Cyc_Absyn_Aggrdecl*_tmpAE0;struct Cyc_List_List*_tmpADF;struct Cyc_List_List*_tmpADE;void*_tmpADD;union Cyc_Absyn_Constraint*_tmpADC;switch(*((int*)_tmpAD7)){case 0: _LL8FD: _LL8FE:
 goto _LL900;case 6: _LL8FF: _LL900:
 goto _LL902;case 7: _LL901: _LL902:
 return 1;case 13: _LL903: _LL904:
 goto _LL906;case 14: _LL905: _LL906:
 return 0;case 8: _LL907: _tmpADD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAD7)->f1).elt_type;_tmpADC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAD7)->f1).zero_term;_LL908:
# 5525
 return !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpADC) && Cyc_Tcutil_bits_only(_tmpADD);case 10: _LL909: _tmpADE=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpAD7)->f1;_LL90A:
# 5527
 for(0;_tmpADE != 0;_tmpADE=_tmpADE->tl){
if(!Cyc_Tcutil_bits_only((*((struct _tuple12*)_tmpADE->hd)).f2))return 0;}
return 1;case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAD7)->f1).aggr_info).UnknownAggr).tag == 1){_LL90B: _LL90C:
 return 0;}else{_LL90D: _tmpAE0=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAD7)->f1).aggr_info).KnownAggr).val;_tmpADF=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAD7)->f1).targs;_LL90E:
# 5532
 if(_tmpAE0->impl == 0)
return 0;{
int okay=1;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAE0->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
if(okay)return 1;{
struct _RegionHandle _tmpAD8=_new_region("rgn");struct _RegionHandle*rgn=& _tmpAD8;_push_region(rgn);
{struct Cyc_List_List*_tmpAD9=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpAE0->tvs,_tmpADF);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAE0->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpAD9,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpADA=0;_npop_handler(0);return _tmpADA;}}}{
int _tmpADB=1;_npop_handler(0);return _tmpADB;};}
# 5539
;_pop_region(rgn);};};}case 12: _LL90F: _tmpAE1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpAD7)->f2;_LL910:
# 5544
 for(0;_tmpAE1 != 0;_tmpAE1=_tmpAE1->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)_tmpAE1->hd)->type))return 0;}
return 1;default: _LL911: _LL912:
 return 0;}_LL8FC:;}
# 5552
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e){
while(1){
void*_tmpAE2=e->r;void*_tmpAE3=_tmpAE2;struct Cyc_Absyn_Exp*_tmpAE5;struct Cyc_Absyn_Exp*_tmpAE4;switch(*((int*)_tmpAE3)){case 1: _LL914: _LL915:
 return 1;case 11: _LL916: _tmpAE4=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpAE3)->f1;_LL917:
 _tmpAE5=_tmpAE4;goto _LL919;case 12: _LL918: _tmpAE5=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpAE3)->f1;_LL919:
 e=_tmpAE5;continue;default: _LL91A: _LL91B:
# 5559
 return 0;}_LL913:;}}
# 5569
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmpAE6=e->r;void*_tmpAE7=_tmpAE6;struct Cyc_List_List*_tmpB05;struct Cyc_List_List*_tmpB04;enum Cyc_Absyn_Primop _tmpB03;struct Cyc_List_List*_tmpB02;struct Cyc_List_List*_tmpB01;struct Cyc_List_List*_tmpB00;struct Cyc_List_List*_tmpAFF;struct Cyc_Absyn_Exp*_tmpAFE;struct Cyc_Absyn_Exp*_tmpAFD;struct Cyc_Absyn_Exp*_tmpAFC;struct Cyc_Absyn_Exp*_tmpAFB;void*_tmpAFA;struct Cyc_Absyn_Exp*_tmpAF9;void*_tmpAF8;struct Cyc_Absyn_Exp*_tmpAF7;struct Cyc_Absyn_Exp*_tmpAF6;struct Cyc_Absyn_Exp*_tmpAF5;struct Cyc_Absyn_Exp*_tmpAF4;struct Cyc_Absyn_Exp*_tmpAF3;struct Cyc_Absyn_Exp*_tmpAF2;struct Cyc_Absyn_Exp*_tmpAF1;struct Cyc_Absyn_Exp*_tmpAF0;void*_tmpAEF;switch(*((int*)_tmpAE7)){case 0: _LL91D: _LL91E:
 goto _LL920;case 16: _LL91F: _LL920:
 goto _LL922;case 17: _LL921: _LL922:
 goto _LL924;case 18: _LL923: _LL924:
 goto _LL926;case 31: _LL925: _LL926:
 goto _LL928;case 32: _LL927: _LL928:
 return 1;case 1: _LL929: _tmpAEF=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpAE7)->f1;_LL92A: {
# 5580
void*_tmpAE8=_tmpAEF;struct Cyc_Absyn_Vardecl*_tmpAEE;struct Cyc_Absyn_Vardecl*_tmpAED;switch(*((int*)_tmpAE8)){case 2: _LL94C: _LL94D:
 return 1;case 1: _LL94E: _tmpAED=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpAE8)->f1;_LL94F: {
# 5583
void*_tmpAE9=Cyc_Tcutil_compress(_tmpAED->type);void*_tmpAEA=_tmpAE9;switch(*((int*)_tmpAEA)){case 8: _LL957: _LL958:
 goto _LL95A;case 9: _LL959: _LL95A:
 return 1;default: _LL95B: _LL95C:
 return var_okay;}_LL956:;}case 4: _LL950: _tmpAEE=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpAE8)->f1;_LL951:
# 5590
 if(_tmpAEE->sc == Cyc_Absyn_Static){
void*_tmpAEB=Cyc_Tcutil_compress(_tmpAEE->type);void*_tmpAEC=_tmpAEB;switch(*((int*)_tmpAEC)){case 8: _LL95E: _LL95F:
 goto _LL961;case 9: _LL960: _LL961:
 return 1;default: _LL962: _LL963:
 return var_okay;}_LL95D:;}else{
# 5597
return var_okay;}case 0: _LL952: _LL953:
 return 0;default: _LL954: _LL955:
 return var_okay;}_LL94B:;}case 5: _LL92B: _tmpAF2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAE7)->f1;_tmpAF1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAE7)->f2;_tmpAF0=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAE7)->f3;_LL92C:
# 5603
 return(Cyc_Tcutil_cnst_exp(0,_tmpAF2) && 
Cyc_Tcutil_cnst_exp(0,_tmpAF1)) && 
Cyc_Tcutil_cnst_exp(0,_tmpAF0);case 8: _LL92D: _tmpAF4=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpAE7)->f1;_tmpAF3=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpAE7)->f2;_LL92E:
# 5607
 return Cyc_Tcutil_cnst_exp(0,_tmpAF4) && Cyc_Tcutil_cnst_exp(0,_tmpAF3);case 11: _LL92F: _tmpAF5=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpAE7)->f1;_LL930:
 _tmpAF6=_tmpAF5;goto _LL932;case 12: _LL931: _tmpAF6=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpAE7)->f1;_LL932:
# 5610
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpAF6);case 13: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAE7)->f4 == Cyc_Absyn_No_coercion){_LL933: _tmpAF8=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAE7)->f1;_tmpAF7=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAE7)->f2;_LL934:
# 5612
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpAF7);}else{_LL935: _tmpAFA=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAE7)->f1;_tmpAF9=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAE7)->f2;_LL936:
# 5615
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpAF9);}case 14: _LL937: _tmpAFB=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpAE7)->f1;_LL938:
# 5617
 return Cyc_Tcutil_cnst_exp(1,_tmpAFB);case 26: _LL939: _tmpAFD=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpAE7)->f2;_tmpAFC=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpAE7)->f3;_LL93A:
# 5619
 return Cyc_Tcutil_cnst_exp(0,_tmpAFD) && Cyc_Tcutil_cnst_exp(0,_tmpAFC);case 27: _LL93B: _tmpAFE=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpAE7)->f1;_LL93C:
# 5621
 return Cyc_Tcutil_cnst_exp(0,_tmpAFE);case 25: _LL93D: _tmpAFF=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpAE7)->f1;_LL93E:
 _tmpB00=_tmpAFF;goto _LL940;case 29: _LL93F: _tmpB00=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpAE7)->f2;_LL940:
 _tmpB01=_tmpB00;goto _LL942;case 28: _LL941: _tmpB01=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpAE7)->f3;_LL942:
# 5625
 for(0;_tmpB01 != 0;_tmpB01=_tmpB01->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple19*)_tmpB01->hd)).f2))
return 0;}
return 1;case 2: _LL943: _tmpB03=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpAE7)->f1;_tmpB02=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpAE7)->f2;_LL944:
# 5630
 _tmpB04=_tmpB02;goto _LL946;case 23: _LL945: _tmpB04=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpAE7)->f1;_LL946:
 _tmpB05=_tmpB04;goto _LL948;case 30: _LL947: _tmpB05=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpAE7)->f1;_LL948:
# 5633
 for(0;_tmpB05 != 0;_tmpB05=_tmpB05->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmpB05->hd))
return 0;}
return 1;default: _LL949: _LL94A:
 return 0;}_LL91C:;}
# 5641
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 5645
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);
# 5647
int Cyc_Tcutil_supports_default(void*t){
void*_tmpB06=Cyc_Tcutil_compress(t);void*_tmpB07=_tmpB06;struct Cyc_List_List*_tmpB11;union Cyc_Absyn_AggrInfoU _tmpB10;struct Cyc_List_List*_tmpB0F;struct Cyc_List_List*_tmpB0E;void*_tmpB0D;union Cyc_Absyn_Constraint*_tmpB0C;union Cyc_Absyn_Constraint*_tmpB0B;switch(*((int*)_tmpB07)){case 0: _LL965: _LL966:
 goto _LL968;case 6: _LL967: _LL968:
 goto _LL96A;case 7: _LL969: _LL96A:
 return 1;case 5: _LL96B: _tmpB0C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB07)->f1).ptr_atts).nullable;_tmpB0B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB07)->f1).ptr_atts).bounds;_LL96C: {
# 5654
void*_tmpB08=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpB0B);void*_tmpB09=_tmpB08;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpB09)->tag == 0){_LL97C: _LL97D:
 return 1;}else{_LL97E: _LL97F:
# 5657
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmpB0C);}_LL97B:;}case 8: _LL96D: _tmpB0D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB07)->f1).elt_type;_LL96E:
# 5660
 return Cyc_Tcutil_supports_default(_tmpB0D);case 10: _LL96F: _tmpB0E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpB07)->f1;_LL970:
# 5662
 for(0;_tmpB0E != 0;_tmpB0E=_tmpB0E->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple12*)_tmpB0E->hd)).f2))return 0;}
return 1;case 11: _LL971: _tmpB10=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpB07)->f1).aggr_info;_tmpB0F=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpB07)->f1).targs;_LL972: {
# 5666
struct Cyc_Absyn_Aggrdecl*_tmpB0A=Cyc_Absyn_get_known_aggrdecl(_tmpB10);
if(_tmpB0A->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB0A->impl))->exist_vars != 0)return 0;
return Cyc_Tcutil_fields_support_default(_tmpB0A->tvs,_tmpB0F,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB0A->impl))->fields);}case 12: _LL973: _tmpB11=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpB07)->f2;_LL974:
 return Cyc_Tcutil_fields_support_default(0,0,_tmpB11);case 14: _LL975: _LL976:
# 5672
 goto _LL978;case 13: _LL977: _LL978:
 return 1;default: _LL979: _LL97A:
# 5675
 return 0;}_LL964:;}
# 5680
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t){
void*_tmpB12=t;struct Cyc_Absyn_Typedefdecl*_tmpB17;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpB12)->tag == 17){_LL981: _tmpB17=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpB12)->f3;_LL982:
# 5683
 if(_tmpB17 != 0){
struct Cyc_Absyn_Tqual _tmpB13=_tmpB17->tq;
if(((_tmpB13.print_const  || _tmpB13.q_volatile) || _tmpB13.q_restrict) || _tmpB13.real_const)
# 5688
({struct Cyc_String_pa_PrintArg_struct _tmpB16;_tmpB16.tag=0;_tmpB16.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmpB14[1]={& _tmpB16};Cyc_Tcutil_warn(loc,({const char*_tmpB15="qualifier within typedef type %s is ignored";_tag_dyneither(_tmpB15,sizeof(char),44);}),_tag_dyneither(_tmpB14,sizeof(void*),1));});});}
# 5691
goto _LL980;}else{_LL983: _LL984:
 goto _LL980;}_LL980:;}
# 5696
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 5698
struct _RegionHandle _tmpB18=_new_region("rgn");struct _RegionHandle*rgn=& _tmpB18;_push_region(rgn);
{struct Cyc_List_List*_tmpB19=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_supports_default(t)){int _tmpB1A=1;_npop_handler(0);return _tmpB1A;}
t=Cyc_Tcutil_rsubstitute(rgn,_tmpB19,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpB1B=0;_npop_handler(0);return _tmpB1B;}}{
# 5706
int _tmpB1C=1;_npop_handler(0);return _tmpB1C;};}
# 5699
;_pop_region(rgn);}
# 5712
int Cyc_Tcutil_admits_zero(void*t){
void*_tmpB1D=Cyc_Tcutil_compress(t);void*_tmpB1E=_tmpB1D;union Cyc_Absyn_Constraint*_tmpB22;union Cyc_Absyn_Constraint*_tmpB21;switch(*((int*)_tmpB1E)){case 6: _LL986: _LL987:
 goto _LL989;case 7: _LL988: _LL989:
 return 1;case 5: _LL98A: _tmpB22=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB1E)->f1).ptr_atts).nullable;_tmpB21=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB1E)->f1).ptr_atts).bounds;_LL98B: {
# 5717
void*_tmpB1F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpB21);void*_tmpB20=_tmpB1F;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpB20)->tag == 0){_LL98F: _LL990:
# 5721
 return 0;}else{_LL991: _LL992:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpB22);}_LL98E:;}default: _LL98C: _LL98D:
# 5724
 return 0;}_LL985:;}
# 5728
int Cyc_Tcutil_is_noreturn(void*t){
{void*_tmpB23=Cyc_Tcutil_compress(t);void*_tmpB24=_tmpB23;struct Cyc_List_List*_tmpB28;void*_tmpB27;switch(*((int*)_tmpB24)){case 5: _LL994: _tmpB27=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB24)->f1).elt_typ;_LL995:
 return Cyc_Tcutil_is_noreturn(_tmpB27);case 9: _LL996: _tmpB28=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB24)->f1).attributes;_LL997:
# 5732
 for(0;_tmpB28 != 0;_tmpB28=_tmpB28->tl){
void*_tmpB25=(void*)_tmpB28->hd;void*_tmpB26=_tmpB25;if(((struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmpB26)->tag == 4){_LL99B: _LL99C:
 return 1;}else{_LL99D: _LL99E:
 continue;}_LL99A:;}
# 5737
goto _LL993;default: _LL998: _LL999:
 goto _LL993;}_LL993:;}
# 5740
return 0;}
# 5745
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmpB29=Cyc_Tcutil_compress(t);void*_tmpB2A=_tmpB29;struct Cyc_List_List**_tmpB30;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB2A)->tag == 9){_LL9A0: _tmpB30=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB2A)->f1).attributes;_LL9A1: {
# 5748
struct Cyc_List_List*_tmpB2B=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Tcutil_attribute_cmp,*_tmpB30,(void*)atts->hd))
*_tmpB30=({struct Cyc_List_List*_tmpB2C=_cycalloc(sizeof(*_tmpB2C));_tmpB2C->hd=(void*)atts->hd;_tmpB2C->tl=*_tmpB30;_tmpB2C;});}else{
# 5755
_tmpB2B=({struct Cyc_List_List*_tmpB2D=_cycalloc(sizeof(*_tmpB2D));_tmpB2D->hd=(void*)atts->hd;_tmpB2D->tl=_tmpB2B;_tmpB2D;});}}
return _tmpB2B;}}else{_LL9A2: _LL9A3:
({void*_tmpB2E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpB2F="transfer_fn_type_atts";_tag_dyneither(_tmpB2F,sizeof(char),22);}),_tag_dyneither(_tmpB2E,sizeof(void*),0));});}_LL99F:;}
# 5762
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
struct Cyc_Absyn_Exp*bound=0;
{void*_tmpB31=Cyc_Tcutil_compress(t);void*_tmpB32=_tmpB31;struct Cyc_Absyn_Exp*_tmpB37;struct Cyc_Absyn_PtrInfo*_tmpB36;switch(*((int*)_tmpB32)){case 5: _LL9A5: _tmpB36=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB32)->f1;_LL9A6:
# 5766
{void*_tmpB33=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)((_tmpB36->ptr_atts).bounds);void*_tmpB34=_tmpB33;struct Cyc_Absyn_Exp*_tmpB35;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpB34)->tag == 1){_LL9AC: _tmpB35=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpB34)->f1;_LL9AD:
 bound=_tmpB35;goto _LL9AB;}else{_LL9AE: _LL9AF:
 goto _LL9AB;}_LL9AB:;}
# 5770
goto _LL9A4;case 8: _LL9A7: _tmpB37=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB32)->f1).num_elts;_LL9A8:
# 5772
 bound=_tmpB37;
goto _LL9A4;default: _LL9A9: _LL9AA:
 goto _LL9A4;}_LL9A4:;}
# 5776
return bound;}
# 5781
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
{void*_tmpB38=b;struct Cyc_Absyn_Vardecl*_tmpB3C;struct Cyc_Absyn_Vardecl*_tmpB3B;struct Cyc_Absyn_Vardecl*_tmpB3A;struct Cyc_Absyn_Vardecl*_tmpB39;switch(*((int*)_tmpB38)){case 5: _LL9B1: _tmpB39=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpB38)->f1;_LL9B2:
 _tmpB3A=_tmpB39;goto _LL9B4;case 4: _LL9B3: _tmpB3A=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpB38)->f1;_LL9B4:
 _tmpB3B=_tmpB3A;goto _LL9B6;case 3: _LL9B5: _tmpB3B=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpB38)->f1;_LL9B6:
 _tmpB3C=_tmpB3B;goto _LL9B8;case 1: _LL9B7: _tmpB3C=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpB38)->f1;_LL9B8:
# 5787
 if(!_tmpB3C->escapes)return _tmpB3C;
goto _LL9B0;default: _LL9B9: _LL9BA:
 goto _LL9B0;}_LL9B0:;}
# 5791
return 0;}
# 5795
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmpB3D=0;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0)_tmpB3D=({struct Cyc_List_List*_tmpB3E=_cycalloc(sizeof(*_tmpB3E));_tmpB3E->hd=*((void**)_check_null((void**)x->hd));_tmpB3E->tl=_tmpB3D;_tmpB3E;});}}
return _tmpB3D;}
# 5802
int Cyc_Tcutil_is_array(void*t){
# 5804
void*_tmpB3F=Cyc_Tcutil_compress(t);void*_tmpB40=_tmpB3F;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB40)->tag == 8){_LL9BC: _LL9BD:
# 5806
 return 1;}else{_LL9BE: _LL9BF:
 return 0;}_LL9BB:;}
# 5811
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
# 5813
void*_tmpB41=Cyc_Tcutil_compress(t);void*_tmpB42=_tmpB41;void*_tmpB52;struct Cyc_Absyn_Tqual _tmpB51;struct Cyc_Absyn_Exp*_tmpB50;union Cyc_Absyn_Constraint*_tmpB4F;unsigned int _tmpB4E;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB42)->tag == 8){_LL9C1: _tmpB52=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB42)->f1).elt_type;_tmpB51=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB42)->f1).tq;_tmpB50=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB42)->f1).num_elts;_tmpB4F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB42)->f1).zero_term;_tmpB4E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB42)->f1).zt_loc;_LL9C2: {
# 5815
void*b;
if(_tmpB50 == 0)
b=(void*)& Cyc_Absyn_DynEither_b_val;else{
# 5819
struct Cyc_Absyn_Exp*bnd=_tmpB50;
if(convert_tag){
if(bnd->topt == 0)
({void*_tmpB43=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpB44="cannot convert tag without type!";_tag_dyneither(_tmpB44,sizeof(char),33);}),_tag_dyneither(_tmpB43,sizeof(void*),0));});{
void*_tmpB45=Cyc_Tcutil_compress((void*)_check_null(bnd->topt));void*_tmpB46=_tmpB45;void*_tmpB4B;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpB46)->tag == 19){_LL9C6: _tmpB4B=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpB46)->f1;_LL9C7:
# 5825
 b=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpB47=_cycalloc(sizeof(*_tmpB47));_tmpB47[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpB48;_tmpB48.tag=1;_tmpB48.f1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmpB4B,0),0,Cyc_Absyn_No_coercion,0);_tmpB48;});_tmpB47;});
goto _LL9C5;}else{_LL9C8: _LL9C9:
# 5828
 if(Cyc_Tcutil_is_const_exp(bnd))
b=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpB49=_cycalloc(sizeof(*_tmpB49));_tmpB49[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpB4A;_tmpB4A.tag=1;_tmpB4A.f1=bnd;_tmpB4A;});_tmpB49;});else{
# 5831
b=(void*)& Cyc_Absyn_DynEither_b_val;}}_LL9C5:;};}else{
# 5835
b=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpB4C=_cycalloc(sizeof(*_tmpB4C));_tmpB4C[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpB4D;_tmpB4D.tag=1;_tmpB4D.f1=bnd;_tmpB4D;});_tmpB4C;});}}
# 5837
return Cyc_Absyn_atb_typ(_tmpB52,rgn,_tmpB51,b,_tmpB4F);}}else{_LL9C3: _LL9C4:
# 5840
 return t;}_LL9C0:;}
# 5845
int Cyc_Tcutil_zeroable_type(void*t){
void*_tmpB53=Cyc_Tcutil_compress(t);void*_tmpB54=_tmpB53;struct Cyc_List_List*_tmpB62;union Cyc_Absyn_AggrInfoU _tmpB61;struct Cyc_List_List*_tmpB60;struct Cyc_List_List*_tmpB5F;void*_tmpB5E;union Cyc_Absyn_Constraint*_tmpB5D;switch(*((int*)_tmpB54)){case 0: _LL9CB: _LL9CC:
 return 1;case 1: _LL9CD: _LL9CE:
 goto _LL9D0;case 2: _LL9CF: _LL9D0:
 goto _LL9D2;case 3: _LL9D1: _LL9D2:
 goto _LL9D4;case 4: _LL9D3: _LL9D4:
 return 0;case 5: _LL9D5: _tmpB5D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB54)->f1).ptr_atts).nullable;_LL9D6:
# 5853
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmpB5D);case 6: _LL9D7: _LL9D8:
 goto _LL9DA;case 7: _LL9D9: _LL9DA:
 return 1;case 8: _LL9DB: _tmpB5E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB54)->f1).elt_type;_LL9DC:
 return Cyc_Tcutil_zeroable_type(_tmpB5E);case 9: _LL9DD: _LL9DE:
 return 0;case 10: _LL9DF: _tmpB5F=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpB54)->f1;_LL9E0:
# 5859
 for(0;(unsigned int)_tmpB5F;_tmpB5F=_tmpB5F->tl){
if(!Cyc_Tcutil_zeroable_type((*((struct _tuple12*)_tmpB5F->hd)).f2))return 0;}
return 1;case 11: _LL9E1: _tmpB61=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpB54)->f1).aggr_info;_tmpB60=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpB54)->f1).targs;_LL9E2: {
# 5863
struct Cyc_Absyn_Aggrdecl*_tmpB55=Cyc_Absyn_get_known_aggrdecl(_tmpB61);
if(_tmpB55->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB55->impl))->exist_vars != 0)
return 0;{
struct _RegionHandle _tmpB56=_new_region("r");struct _RegionHandle*r=& _tmpB56;_push_region(r);
{struct Cyc_List_List*_tmpB57=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmpB55->tvs,_tmpB60);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB55->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmpB57,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpB58=0;_npop_handler(0);return _tmpB58;}}}{
int _tmpB59=1;_npop_handler(0);return _tmpB59;};}
# 5867
;_pop_region(r);};}case 13: _LL9E3: _LL9E4:
# 5871
 return 1;case 19: _LL9E5: _LL9E6:
 return 1;case 12: _LL9E7: _tmpB62=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpB54)->f2;_LL9E8:
# 5874
 for(0;_tmpB62 != 0;_tmpB62=_tmpB62->tl){
if(!Cyc_Tcutil_zeroable_type(((struct Cyc_Absyn_Aggrfield*)_tmpB62->hd)->type))return 0;}
return 1;case 14: _LL9E9: _LL9EA:
 return 1;case 17: _LL9EB: _LL9EC:
 return 0;case 16: _LL9ED: _LL9EE:
 return 0;case 15: _LL9EF: _LL9F0:
 return 0;case 28: _LL9F1: _LL9F2:
 return 0;case 26: _LL9F3: _LL9F4:
# 5883
 goto _LL9F6;case 27: _LL9F5: _LL9F6:
 goto _LL9F8;case 18: _LL9F7: _LL9F8:
 goto _LL9FA;case 20: _LL9F9: _LL9FA:
 goto _LL9FC;case 21: _LL9FB: _LL9FC:
 goto _LL9FE;case 22: _LL9FD: _LL9FE:
 goto _LLA00;case 23: _LL9FF: _LLA00:
 goto _LLA02;case 24: _LLA01: _LLA02:
 goto _LLA04;default: _LLA03: _LLA04:
({struct Cyc_String_pa_PrintArg_struct _tmpB5C;_tmpB5C.tag=0;_tmpB5C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmpB5A[1]={& _tmpB5C};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpB5B="bad type `%s' in zeroable type";_tag_dyneither(_tmpB5B,sizeof(char),31);}),_tag_dyneither(_tmpB5A,sizeof(void*),1));});});}_LL9CA:;}
