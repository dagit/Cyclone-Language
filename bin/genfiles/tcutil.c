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
{const char*_tmpB71;if(Cyc_strcmp(((_tmpB71="(qualifiers don't match)",_tag_dyneither(_tmpB71,sizeof(char),25))),(struct _dyneither_ptr)Cyc_Tcutil_failure_reason)== 0){
{const char*_tmpB75;void*_tmpB74[1];struct Cyc_String_pa_PrintArg_struct _tmpB73;(_tmpB73.tag=0,((_tmpB73.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((_tmpB74[0]=& _tmpB73,Cyc_fprintf(Cyc_stderr,((_tmpB75="  %s\n",_tag_dyneither(_tmpB75,sizeof(char),6))),_tag_dyneither(_tmpB74,sizeof(void*),1)))))));}
return;}}
# 75
{const char*_tmpB76;if(Cyc_strcmp(((_tmpB76="(function effects do not match)",_tag_dyneither(_tmpB76,sizeof(char),32))),(struct _dyneither_ptr)Cyc_Tcutil_failure_reason)== 0){
struct Cyc_Absynpp_Params _tmp6=Cyc_Absynpp_tc_params_r;
_tmp6.print_all_effects=1;
Cyc_Absynpp_set_params(& _tmp6);}}{
# 80
struct _dyneither_ptr s1=Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure);
struct _dyneither_ptr s2=Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure);
int pos=2;
{const char*_tmpB7A;void*_tmpB79[1];struct Cyc_String_pa_PrintArg_struct _tmpB78;(_tmpB78.tag=0,((_tmpB78.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmpB79[0]=& _tmpB78,Cyc_fprintf(Cyc_stderr,((_tmpB7A="  %s",_tag_dyneither(_tmpB7A,sizeof(char),5))),_tag_dyneither(_tmpB79,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s1,sizeof(char));
if(pos + 5 >= 80){
{const char*_tmpB7D;void*_tmpB7C;(_tmpB7C=0,Cyc_fprintf(Cyc_stderr,((_tmpB7D="\n\t",_tag_dyneither(_tmpB7D,sizeof(char),3))),_tag_dyneither(_tmpB7C,sizeof(void*),0)));}
pos=8;}else{
# 89
{const char*_tmpB80;void*_tmpB7F;(_tmpB7F=0,Cyc_fprintf(Cyc_stderr,((_tmpB80=" ",_tag_dyneither(_tmpB80,sizeof(char),2))),_tag_dyneither(_tmpB7F,sizeof(void*),0)));}
++ pos;}
# 92
{const char*_tmpB83;void*_tmpB82;(_tmpB82=0,Cyc_fprintf(Cyc_stderr,((_tmpB83="and ",_tag_dyneither(_tmpB83,sizeof(char),5))),_tag_dyneither(_tmpB82,sizeof(void*),0)));}
pos +=4;
if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){
{const char*_tmpB86;void*_tmpB85;(_tmpB85=0,Cyc_fprintf(Cyc_stderr,((_tmpB86="\n\t",_tag_dyneither(_tmpB86,sizeof(char),3))),_tag_dyneither(_tmpB85,sizeof(void*),0)));}
pos=8;}
# 98
{const char*_tmpB8A;void*_tmpB89[1];struct Cyc_String_pa_PrintArg_struct _tmpB88;(_tmpB88.tag=0,((_tmpB88.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmpB89[0]=& _tmpB88,Cyc_fprintf(Cyc_stderr,((_tmpB8A="%s ",_tag_dyneither(_tmpB8A,sizeof(char),4))),_tag_dyneither(_tmpB89,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s2,sizeof(char))+ 1;
if(pos + 17 >= 80){
{const char*_tmpB8D;void*_tmpB8C;(_tmpB8C=0,Cyc_fprintf(Cyc_stderr,((_tmpB8D="\n\t",_tag_dyneither(_tmpB8D,sizeof(char),3))),_tag_dyneither(_tmpB8C,sizeof(void*),0)));}
pos=8;}
# 104
{const char*_tmpB90;void*_tmpB8F;(_tmpB8F=0,Cyc_fprintf(Cyc_stderr,((_tmpB90="are not compatible. ",_tag_dyneither(_tmpB90,sizeof(char),21))),_tag_dyneither(_tmpB8F,sizeof(void*),0)));}
pos +=17;
if((char*)Cyc_Tcutil_failure_reason.curr != (char*)(_tag_dyneither(0,0,0)).curr){
if(pos + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason)>= 80){
const char*_tmpB93;void*_tmpB92;(_tmpB92=0,Cyc_fprintf(Cyc_stderr,((_tmpB93="\n\t",_tag_dyneither(_tmpB93,sizeof(char),3))),_tag_dyneither(_tmpB92,sizeof(void*),0)));}{
# 110
const char*_tmpB97;void*_tmpB96[1];struct Cyc_String_pa_PrintArg_struct _tmpB95;(_tmpB95.tag=0,((_tmpB95.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((_tmpB96[0]=& _tmpB95,Cyc_fprintf(Cyc_stderr,((_tmpB97="%s",_tag_dyneither(_tmpB97,sizeof(char),3))),_tag_dyneither(_tmpB96,sizeof(void*),1)))))));};}
# 112
{const char*_tmpB9A;void*_tmpB99;(_tmpB99=0,Cyc_fprintf(Cyc_stderr,((_tmpB9A="\n",_tag_dyneither(_tmpB9A,sizeof(char),2))),_tag_dyneither(_tmpB99,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);};}
# 116
void Cyc_Tcutil_terr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 119
Cyc_Position_post_error(Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}
# 122
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 125
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
{const char*_tmpB9E;void*_tmpB9D[1];struct Cyc_String_pa_PrintArg_struct _tmpB9C;(_tmpB9C.tag=0,((_tmpB9C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmpB9D[0]=& _tmpB9C,Cyc_fprintf(Cyc_stderr,((_tmpB9E="Compiler Error (Tcutil::impos): %s\n",_tag_dyneither(_tmpB9E,sizeof(char),36))),_tag_dyneither(_tmpB9D,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Core_Impossible_exn_struct _tmpBA1;struct Cyc_Core_Impossible_exn_struct*_tmpBA0;(int)_throw((void*)((_tmpBA0=_cycalloc(sizeof(*_tmpBA0)),((_tmpBA0[0]=((_tmpBA1.tag=Cyc_Core_Impossible,((_tmpBA1.f1=msg,_tmpBA1)))),_tmpBA0)))));};}char Cyc_Tcutil_AbortTypeCheckingFunction[26]="AbortTypeCheckingFunction";
# 133
static struct _dyneither_ptr Cyc_Tcutil_tvar2string(struct Cyc_Absyn_Tvar*tv){
return*tv->name;}
# 137
void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){
for(0;tvs != 0;tvs=tvs->tl){
const char*_tmpBA6;void*_tmpBA5[2];struct Cyc_String_pa_PrintArg_struct _tmpBA4;struct Cyc_String_pa_PrintArg_struct _tmpBA3;(_tmpBA3.tag=0,((_tmpBA3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((_tmpBA4.tag=0,((_tmpBA4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)tvs->hd)),((_tmpBA5[0]=& _tmpBA4,((_tmpBA5[1]=& _tmpBA3,Cyc_fprintf(Cyc_stderr,((_tmpBA6="%s::%s ",_tag_dyneither(_tmpBA6,sizeof(char),8))),_tag_dyneither(_tmpBA5,sizeof(void*),2)))))))))))));}
{const char*_tmpBA9;void*_tmpBA8;(_tmpBA8=0,Cyc_fprintf(Cyc_stderr,((_tmpBA9="\n",_tag_dyneither(_tmpBA9,sizeof(char),2))),_tag_dyneither(_tmpBA8,sizeof(void*),0)));}Cyc_fflush(Cyc_stderr);}
# 144
static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;
# 149
void Cyc_Tcutil_warn(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 152
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
{struct Cyc_List_List*_tmpBAA;Cyc_Tcutil_warning_segs=((_tmpBAA=_cycalloc(sizeof(*_tmpBAA)),((_tmpBAA->hd=(void*)sg,((_tmpBAA->tl=Cyc_Tcutil_warning_segs,_tmpBAA))))));}{
struct _dyneither_ptr*_tmpBAD;struct Cyc_List_List*_tmpBAC;Cyc_Tcutil_warning_msgs=((_tmpBAC=_cycalloc(sizeof(*_tmpBAC)),((_tmpBAC->hd=((_tmpBAD=_cycalloc(sizeof(*_tmpBAD)),((_tmpBAD[0]=msg,_tmpBAD)))),((_tmpBAC->tl=Cyc_Tcutil_warning_msgs,_tmpBAC))))));};}
# 156
void Cyc_Tcutil_flush_warnings(){
if(Cyc_Tcutil_warning_segs == 0)
return;
{const char*_tmpBB0;void*_tmpBAF;(_tmpBAF=0,Cyc_fprintf(Cyc_stderr,((_tmpBB0="***Warnings***\n",_tag_dyneither(_tmpBB0,sizeof(char),16))),_tag_dyneither(_tmpBAF,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp30=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);
Cyc_Tcutil_warning_segs=0;
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);
while(Cyc_Tcutil_warning_msgs != 0){
{const char*_tmpBB5;void*_tmpBB4[2];struct Cyc_String_pa_PrintArg_struct _tmpBB3;struct Cyc_String_pa_PrintArg_struct _tmpBB2;(_tmpBB2.tag=0,((_tmpBB2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd)),((_tmpBB3.tag=0,((_tmpBB3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp30))->hd)),((_tmpBB4[0]=& _tmpBB3,((_tmpBB4[1]=& _tmpBB2,Cyc_fprintf(Cyc_stderr,((_tmpBB5="%s: %s\n",_tag_dyneither(_tmpBB5,sizeof(char),8))),_tag_dyneither(_tmpBB4,sizeof(void*),2)))))))))))));}
_tmp30=_tmp30->tl;
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}
# 168
{const char*_tmpBB8;void*_tmpBB7;(_tmpBB7=0,Cyc_fprintf(Cyc_stderr,((_tmpBB8="**************\n",_tag_dyneither(_tmpBB8,sizeof(char),16))),_tag_dyneither(_tmpBB7,sizeof(void*),0)));}
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
void*_tmp44=Cyc_Absyn_compress_kb(kb);void*_tmp45=_tmp44;struct Cyc_Absyn_Kind*_tmp46;switch(*((int*)_tmp45)){case 1: _LL1E: _LL1F: {
struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpBBB;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpBBA;return(void*)((_tmpBBA=_cycalloc(sizeof(*_tmpBBA)),((_tmpBBA[0]=((_tmpBBB.tag=1,((_tmpBBB.f1=0,_tmpBBB)))),_tmpBBA))));}case 2: _LL20: _tmp46=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp45)->f2;_LL21: {
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpBBE;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpBBD;return(void*)((_tmpBBD=_cycalloc(sizeof(*_tmpBBD)),((_tmpBBD[0]=((_tmpBBE.tag=2,((_tmpBBE.f1=0,((_tmpBBE.f2=_tmp46,_tmpBBE)))))),_tmpBBD))));}default: _LL22: _LL23:
 return kb;}_LL1D:;}
# 235
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 237
struct Cyc_Absyn_Tvar*_tmpBBF;return(_tmpBBF=_cycalloc(sizeof(*_tmpBBF)),((_tmpBBF->name=tv->name,((_tmpBBF->identity=- 1,((_tmpBBF->kind=Cyc_Tcutil_copy_kindbound(tv->kind),_tmpBBF)))))));}
# 239
static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
# 241
struct _dyneither_ptr*_tmp4D;struct Cyc_Absyn_Tqual _tmp4E;void*_tmp4F;struct _tuple10*_tmp4C=arg;_tmp4D=_tmp4C->f1;_tmp4E=_tmp4C->f2;_tmp4F=_tmp4C->f3;{
struct _tuple10*_tmpBC0;return(_tmpBC0=_cycalloc(sizeof(*_tmpBC0)),((_tmpBC0->f1=_tmp4D,((_tmpBC0->f2=_tmp4E,((_tmpBC0->f3=Cyc_Tcutil_copy_type(_tmp4F),_tmpBC0)))))));};}
# 244
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
struct Cyc_Absyn_Tqual _tmp52;void*_tmp53;struct _tuple12*_tmp51=arg;_tmp52=_tmp51->f1;_tmp53=_tmp51->f2;{
struct _tuple12*_tmpBC1;return(_tmpBC1=_cycalloc(sizeof(*_tmpBC1)),((_tmpBC1->f1=_tmp52,((_tmpBC1->f2=Cyc_Tcutil_copy_type(_tmp53),_tmpBC1)))));};}
# 248
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*);
# 250
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_Absyn_Aggrfield*_tmpBC2;return(_tmpBC2=_cycalloc(sizeof(*_tmpBC2)),((_tmpBC2->name=f->name,((_tmpBC2->tq=f->tq,((_tmpBC2->type=Cyc_Tcutil_copy_type(f->type),((_tmpBC2->width=f->width,((_tmpBC2->attributes=f->attributes,((_tmpBC2->requires_clause=
Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause),_tmpBC2)))))))))))));}
# 254
static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
void*_tmp57;void*_tmp58;struct _tuple0*_tmp56=x;_tmp57=_tmp56->f1;_tmp58=_tmp56->f2;{
struct _tuple0*_tmpBC3;return(_tmpBC3=_cycalloc(sizeof(*_tmpBC3)),((_tmpBC3->f1=Cyc_Tcutil_copy_type(_tmp57),((_tmpBC3->f2=Cyc_Tcutil_copy_type(_tmp58),_tmpBC3)))));};}
# 258
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){
struct Cyc_Absyn_Enumfield*_tmpBC4;return(_tmpBC4=_cycalloc(sizeof(*_tmpBC4)),((_tmpBC4->name=f->name,((_tmpBC4->tag=f->tag,((_tmpBC4->loc=f->loc,_tmpBC4)))))));}
# 261
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpBC7;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpBC6;return(void*)((_tmpBC6=_cycalloc(sizeof(*_tmpBC6)),((_tmpBC6[0]=((_tmpBC7.tag=2,((_tmpBC7.f1=Cyc_Tcutil_copy_tvar(t),_tmpBC7)))),_tmpBC6))));}
# 265
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp5D=Cyc_Tcutil_compress(t);void*_tmp5E=_tmp5D;struct Cyc_Absyn_Datatypedecl*_tmp5F;struct Cyc_Absyn_Enumdecl*_tmp60;struct Cyc_Absyn_Aggrdecl*_tmp61;void*_tmp62;struct Cyc_List_List*_tmp63;void*_tmp64;struct _tuple2*_tmp65;struct Cyc_List_List*_tmp66;struct Cyc_Absyn_Typedefdecl*_tmp67;void*_tmp68;void*_tmp69;void*_tmp6A;struct Cyc_Absyn_Exp*_tmp6B;struct Cyc_Absyn_Exp*_tmp6C;void*_tmp6D;struct Cyc_List_List*_tmp6E;struct _tuple2*_tmp6F;struct Cyc_Absyn_Enumdecl*_tmp70;enum Cyc_Absyn_AggrKind _tmp71;struct Cyc_List_List*_tmp72;struct Cyc_Absyn_Aggrdecl**_tmp73;struct Cyc_List_List*_tmp74;enum Cyc_Absyn_AggrKind _tmp75;struct _tuple2*_tmp76;struct Cyc_Core_Opt*_tmp77;struct Cyc_List_List*_tmp78;struct Cyc_List_List*_tmp79;struct Cyc_List_List*_tmp7A;void*_tmp7B;struct Cyc_Absyn_Tqual _tmp7C;void*_tmp7D;struct Cyc_List_List*_tmp7E;int _tmp7F;struct Cyc_Absyn_VarargInfo*_tmp80;struct Cyc_List_List*_tmp81;struct Cyc_List_List*_tmp82;struct Cyc_Absyn_Exp*_tmp83;struct Cyc_List_List*_tmp84;struct Cyc_Absyn_Exp*_tmp85;struct Cyc_List_List*_tmp86;void*_tmp87;struct Cyc_Absyn_Tqual _tmp88;struct Cyc_Absyn_Exp*_tmp89;union Cyc_Absyn_Constraint*_tmp8A;unsigned int _tmp8B;void*_tmp8C;struct Cyc_Absyn_Tqual _tmp8D;void*_tmp8E;union Cyc_Absyn_Constraint*_tmp8F;union Cyc_Absyn_Constraint*_tmp90;union Cyc_Absyn_Constraint*_tmp91;struct Cyc_Absyn_PtrLoc*_tmp92;union Cyc_Absyn_DatatypeFieldInfoU _tmp93;struct Cyc_List_List*_tmp94;union Cyc_Absyn_DatatypeInfoU _tmp95;struct Cyc_List_List*_tmp96;struct Cyc_Absyn_Tvar*_tmp97;switch(*((int*)_tmp5E)){case 0: _LL25: _LL26:
 goto _LL28;case 1: _LL27: _LL28:
 return t;case 2: _LL29: _tmp97=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp5E)->f1;_LL2A: {
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpBCA;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpBC9;return(void*)((_tmpBC9=_cycalloc(sizeof(*_tmpBC9)),((_tmpBC9[0]=((_tmpBCA.tag=2,((_tmpBCA.f1=Cyc_Tcutil_copy_tvar(_tmp97),_tmpBCA)))),_tmpBC9))));}case 3: _LL2B: _tmp95=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5E)->f1).datatype_info;_tmp96=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5E)->f1).targs;_LL2C: {
# 271
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpBD0;struct Cyc_Absyn_DatatypeInfo _tmpBCF;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpBCE;return(void*)((_tmpBCE=_cycalloc(sizeof(*_tmpBCE)),((_tmpBCE[0]=((_tmpBD0.tag=3,((_tmpBD0.f1=((_tmpBCF.datatype_info=_tmp95,((_tmpBCF.targs=Cyc_Tcutil_copy_types(_tmp96),_tmpBCF)))),_tmpBD0)))),_tmpBCE))));}case 4: _LL2D: _tmp93=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp5E)->f1).field_info;_tmp94=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp5E)->f1).targs;_LL2E: {
# 273
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpBD6;struct Cyc_Absyn_DatatypeFieldInfo _tmpBD5;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpBD4;return(void*)((_tmpBD4=_cycalloc(sizeof(*_tmpBD4)),((_tmpBD4[0]=((_tmpBD6.tag=4,((_tmpBD6.f1=((_tmpBD5.field_info=_tmp93,((_tmpBD5.targs=Cyc_Tcutil_copy_types(_tmp94),_tmpBD5)))),_tmpBD6)))),_tmpBD4))));}case 5: _LL2F: _tmp8C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).elt_typ;_tmp8D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).elt_tq;_tmp8E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).ptr_atts).rgn;_tmp8F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).ptr_atts).nullable;_tmp90=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).ptr_atts).bounds;_tmp91=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).ptr_atts).zero_term;_tmp92=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).ptr_atts).ptrloc;_LL30: {
# 275
void*_tmpA0=Cyc_Tcutil_copy_type(_tmp8C);
void*_tmpA1=Cyc_Tcutil_copy_type(_tmp8E);
union Cyc_Absyn_Constraint*_tmpA2=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp8F);
struct Cyc_Absyn_Tqual _tmpA3=_tmp8D;
# 280
union Cyc_Absyn_Constraint*_tmpA4=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp90);
union Cyc_Absyn_Constraint*_tmpA5=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp91);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpBDC;struct Cyc_Absyn_PtrInfo _tmpBDB;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpBDA;return(void*)((_tmpBDA=_cycalloc(sizeof(*_tmpBDA)),((_tmpBDA[0]=((_tmpBDC.tag=5,((_tmpBDC.f1=((_tmpBDB.elt_typ=_tmpA0,((_tmpBDB.elt_tq=_tmpA3,((_tmpBDB.ptr_atts=(((_tmpBDB.ptr_atts).rgn=_tmpA1,(((_tmpBDB.ptr_atts).nullable=_tmpA2,(((_tmpBDB.ptr_atts).bounds=_tmpA4,(((_tmpBDB.ptr_atts).zero_term=_tmpA5,(((_tmpBDB.ptr_atts).ptrloc=_tmp92,_tmpBDB.ptr_atts)))))))))),_tmpBDB)))))),_tmpBDC)))),_tmpBDA))));}case 6: _LL31: _LL32:
 goto _LL34;case 7: _LL33: _LL34:
 return t;case 8: _LL35: _tmp87=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E)->f1).elt_type;_tmp88=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E)->f1).tq;_tmp89=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E)->f1).num_elts;_tmp8A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E)->f1).zero_term;_tmp8B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E)->f1).zt_loc;_LL36: {
# 286
struct Cyc_Absyn_Exp*eopt2=Cyc_Tcutil_deep_copy_exp_opt(1,_tmp89);
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpBE2;struct Cyc_Absyn_ArrayInfo _tmpBE1;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpBE0;return(void*)((_tmpBE0=_cycalloc(sizeof(*_tmpBE0)),((_tmpBE0[0]=((_tmpBE2.tag=8,((_tmpBE2.f1=((_tmpBE1.elt_type=Cyc_Tcutil_copy_type(_tmp87),((_tmpBE1.tq=_tmp88,((_tmpBE1.num_elts=eopt2,((_tmpBE1.zero_term=
((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp8A),((_tmpBE1.zt_loc=_tmp8B,_tmpBE1)))))))))),_tmpBE2)))),_tmpBE0))));}case 9: _LL37: _tmp7A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).tvars;_tmp7B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).effect;_tmp7C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).ret_tqual;_tmp7D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).ret_typ;_tmp7E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).args;_tmp7F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).c_varargs;_tmp80=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).cyc_varargs;_tmp81=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).rgn_po;_tmp82=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).attributes;_tmp83=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).requires_clause;_tmp84=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).requires_relns;_tmp85=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).ensures_clause;_tmp86=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).ensures_relns;_LL38: {
# 290
struct Cyc_List_List*_tmpAC=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp7A);
void*effopt2=_tmp7B == 0?0: Cyc_Tcutil_copy_type(_tmp7B);
void*_tmpAD=Cyc_Tcutil_copy_type(_tmp7D);
struct Cyc_List_List*_tmpAE=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp7E);
int _tmpAF=_tmp7F;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(_tmp80 != 0){
struct Cyc_Absyn_VarargInfo*cv=_tmp80;
struct Cyc_Absyn_VarargInfo*_tmpBE3;cyc_varargs2=((_tmpBE3=_cycalloc(sizeof(*_tmpBE3)),((_tmpBE3->name=cv->name,((_tmpBE3->tq=cv->tq,((_tmpBE3->type=Cyc_Tcutil_copy_type(cv->type),((_tmpBE3->inject=cv->inject,_tmpBE3))))))))));}{
# 301
struct Cyc_List_List*_tmpB1=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp81);
struct Cyc_List_List*_tmpB2=_tmp82;
struct Cyc_Absyn_Exp*_tmpB3=Cyc_Tcutil_deep_copy_exp_opt(1,_tmp83);
struct Cyc_List_List*_tmpB4=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp84);
struct Cyc_Absyn_Exp*_tmpB5=Cyc_Tcutil_deep_copy_exp_opt(1,_tmp85);
struct Cyc_List_List*_tmpB6=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp86);
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpBE9;struct Cyc_Absyn_FnInfo _tmpBE8;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpBE7;return(void*)((_tmpBE7=_cycalloc(sizeof(*_tmpBE7)),((_tmpBE7[0]=((_tmpBE9.tag=9,((_tmpBE9.f1=((_tmpBE8.tvars=_tmpAC,((_tmpBE8.effect=effopt2,((_tmpBE8.ret_tqual=_tmp7C,((_tmpBE8.ret_typ=_tmpAD,((_tmpBE8.args=_tmpAE,((_tmpBE8.c_varargs=_tmpAF,((_tmpBE8.cyc_varargs=cyc_varargs2,((_tmpBE8.rgn_po=_tmpB1,((_tmpBE8.attributes=_tmpB2,((_tmpBE8.requires_clause=_tmpB3,((_tmpBE8.requires_relns=_tmpB4,((_tmpBE8.ensures_clause=_tmpB5,((_tmpBE8.ensures_relns=_tmpB6,_tmpBE8)))))))))))))))))))))))))),_tmpBE9)))),_tmpBE7))));};}case 10: _LL39: _tmp79=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5E)->f1;_LL3A: {
# 310
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpBEC;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpBEB;return(void*)((_tmpBEB=_cycalloc(sizeof(*_tmpBEB)),((_tmpBEB[0]=((_tmpBEC.tag=10,((_tmpBEC.f1=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp79),_tmpBEC)))),_tmpBEB))));}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5E)->f1).aggr_info).UnknownAggr).tag == 1){_LL3B: _tmp75=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5E)->f1).aggr_info).UnknownAggr).val).f1;_tmp76=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5E)->f1).aggr_info).UnknownAggr).val).f2;_tmp77=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5E)->f1).aggr_info).UnknownAggr).val).f3;_tmp78=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5E)->f1).targs;_LL3C: {
# 312
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpBF2;struct Cyc_Absyn_AggrInfo _tmpBF1;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpBF0;return(void*)((_tmpBF0=_cycalloc(sizeof(*_tmpBF0)),((_tmpBF0[0]=((_tmpBF2.tag=11,((_tmpBF2.f1=((_tmpBF1.aggr_info=Cyc_Absyn_UnknownAggr(_tmp75,_tmp76,_tmp77),((_tmpBF1.targs=Cyc_Tcutil_copy_types(_tmp78),_tmpBF1)))),_tmpBF2)))),_tmpBF0))));}}else{_LL3D: _tmp73=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5E)->f1).aggr_info).KnownAggr).val;_tmp74=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5E)->f1).targs;_LL3E: {
# 314
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpBF8;struct Cyc_Absyn_AggrInfo _tmpBF7;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpBF6;return(void*)((_tmpBF6=_cycalloc(sizeof(*_tmpBF6)),((_tmpBF6[0]=((_tmpBF8.tag=11,((_tmpBF8.f1=((_tmpBF7.aggr_info=Cyc_Absyn_KnownAggr(_tmp73),((_tmpBF7.targs=Cyc_Tcutil_copy_types(_tmp74),_tmpBF7)))),_tmpBF8)))),_tmpBF6))));}}case 12: _LL3F: _tmp71=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5E)->f1;_tmp72=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5E)->f2;_LL40: {
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpBFB;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpBFA;return(void*)((_tmpBFA=_cycalloc(sizeof(*_tmpBFA)),((_tmpBFA[0]=((_tmpBFB.tag=12,((_tmpBFB.f1=_tmp71,((_tmpBFB.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmp72),_tmpBFB)))))),_tmpBFA))));}case 13: _LL41: _tmp6F=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5E)->f1;_tmp70=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5E)->f2;_LL42: {
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpBFE;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpBFD;return(void*)((_tmpBFD=_cycalloc(sizeof(*_tmpBFD)),((_tmpBFD[0]=((_tmpBFE.tag=13,((_tmpBFE.f1=_tmp6F,((_tmpBFE.f2=_tmp70,_tmpBFE)))))),_tmpBFD))));}case 14: _LL43: _tmp6E=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp5E)->f1;_LL44: {
struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpC01;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpC00;return(void*)((_tmpC00=_cycalloc(sizeof(*_tmpC00)),((_tmpC00[0]=((_tmpC01.tag=14,((_tmpC01.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmp6E),_tmpC01)))),_tmpC00))));}case 19: _LL45: _tmp6D=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5E)->f1;_LL46: {
struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpC04;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpC03;return(void*)((_tmpC03=_cycalloc(sizeof(*_tmpC03)),((_tmpC03[0]=((_tmpC04.tag=19,((_tmpC04.f1=Cyc_Tcutil_copy_type(_tmp6D),_tmpC04)))),_tmpC03))));}case 18: _LL47: _tmp6C=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp5E)->f1;_LL48: {
# 321
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpC07;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpC06;return(void*)((_tmpC06=_cycalloc(sizeof(*_tmpC06)),((_tmpC06[0]=((_tmpC07.tag=18,((_tmpC07.f1=_tmp6C,_tmpC07)))),_tmpC06))));}case 27: _LL49: _tmp6B=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp5E)->f1;_LL4A: {
# 324
struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmpC0A;struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmpC09;return(void*)((_tmpC09=_cycalloc(sizeof(*_tmpC09)),((_tmpC09[0]=((_tmpC0A.tag=27,((_tmpC0A.f1=_tmp6B,_tmpC0A)))),_tmpC09))));}case 15: _LL4B: _tmp6A=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5E)->f1;_LL4C: {
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpC0D;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpC0C;return(void*)((_tmpC0C=_cycalloc(sizeof(*_tmpC0C)),((_tmpC0C[0]=((_tmpC0D.tag=15,((_tmpC0D.f1=Cyc_Tcutil_copy_type(_tmp6A),_tmpC0D)))),_tmpC0C))));}case 16: _LL4D: _tmp68=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp5E)->f1;_tmp69=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp5E)->f2;_LL4E: {
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpC10;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpC0F;return(void*)((_tmpC0F=_cycalloc(sizeof(*_tmpC0F)),((_tmpC0F[0]=((_tmpC10.tag=16,((_tmpC10.f1=Cyc_Tcutil_copy_type(_tmp68),((_tmpC10.f2=Cyc_Tcutil_copy_type(_tmp69),_tmpC10)))))),_tmpC0F))));}case 17: _LL4F: _tmp65=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5E)->f1;_tmp66=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5E)->f2;_tmp67=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5E)->f3;_LL50: {
# 328
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpC13;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpC12;return(void*)((_tmpC12=_cycalloc(sizeof(*_tmpC12)),((_tmpC12[0]=((_tmpC13.tag=17,((_tmpC13.f1=_tmp65,((_tmpC13.f2=Cyc_Tcutil_copy_types(_tmp66),((_tmpC13.f3=_tmp67,((_tmpC13.f4=0,_tmpC13)))))))))),_tmpC12))));}case 21: _LL51: _LL52:
 goto _LL54;case 22: _LL53: _LL54:
 goto _LL56;case 28: _LL55: _LL56:
 goto _LL58;case 20: _LL57: _LL58:
 return t;case 23: _LL59: _tmp64=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp5E)->f1;_LL5A: {
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC16;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpC15;return(void*)((_tmpC15=_cycalloc(sizeof(*_tmpC15)),((_tmpC15[0]=((_tmpC16.tag=23,((_tmpC16.f1=Cyc_Tcutil_copy_type(_tmp64),_tmpC16)))),_tmpC15))));}case 24: _LL5B: _tmp63=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp5E)->f1;_LL5C: {
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC19;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpC18;return(void*)((_tmpC18=_cycalloc(sizeof(*_tmpC18)),((_tmpC18[0]=((_tmpC19.tag=24,((_tmpC19.f1=Cyc_Tcutil_copy_types(_tmp63),_tmpC19)))),_tmpC18))));}case 25: _LL5D: _tmp62=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp5E)->f1;_LL5E: {
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpC1C;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpC1B;return(void*)((_tmpC1B=_cycalloc(sizeof(*_tmpC1B)),((_tmpC1B[0]=((_tmpC1C.tag=25,((_tmpC1C.f1=Cyc_Tcutil_copy_type(_tmp62),_tmpC1C)))),_tmpC1B))));}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5E)->f1)->r)){case 0: _LL5F: _tmp61=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5E)->f1)->r)->f1;_LL60: {
# 338
struct Cyc_List_List*_tmpDA=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmp61->tvs);
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpC22;struct Cyc_Absyn_AggrInfo _tmpC21;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC20;return(void*)((_tmpC20=_cycalloc(sizeof(*_tmpC20)),((_tmpC20[0]=((_tmpC22.tag=11,((_tmpC22.f1=((_tmpC21.aggr_info=Cyc_Absyn_UnknownAggr(_tmp61->kind,_tmp61->name,0),((_tmpC21.targs=_tmpDA,_tmpC21)))),_tmpC22)))),_tmpC20))));}case 1: _LL61: _tmp60=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5E)->f1)->r)->f1;_LL62: {
# 341
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpC25;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpC24;return(void*)((_tmpC24=_cycalloc(sizeof(*_tmpC24)),((_tmpC24[0]=((_tmpC25.tag=13,((_tmpC25.f1=_tmp60->name,((_tmpC25.f2=0,_tmpC25)))))),_tmpC24))));}default: _LL63: _tmp5F=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5E)->f1)->r)->f1;_LL64: {
# 343
struct Cyc_List_List*_tmpE0=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmp5F->tvs);
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpC2F;struct Cyc_Absyn_UnknownDatatypeInfo _tmpC2E;struct Cyc_Absyn_DatatypeInfo _tmpC2D;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpC2C;return(void*)((_tmpC2C=_cycalloc(sizeof(*_tmpC2C)),((_tmpC2C[0]=((_tmpC2F.tag=3,((_tmpC2F.f1=((_tmpC2D.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpC2E.name=_tmp5F->name,((_tmpC2E.is_extensible=0,_tmpC2E))))),((_tmpC2D.targs=_tmpE0,_tmpC2D)))),_tmpC2F)))),_tmpC2C))));}}}_LL24:;}
# 358 "tcutil.cyc"
static void*Cyc_Tcutil_copy_designator(int pt,void*d){
void*_tmpE5=d;struct _dyneither_ptr*_tmpE6;struct Cyc_Absyn_Exp*_tmpE7;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpE5)->tag == 0){_LL66: _tmpE7=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpE5)->f1;_LL67: {
struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpC32;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpC31;return(void*)((_tmpC31=_cycalloc(sizeof(*_tmpC31)),((_tmpC31[0]=((_tmpC32.tag=0,((_tmpC32.f1=Cyc_Tcutil_deep_copy_exp(pt,_tmpE7),_tmpC32)))),_tmpC31))));}}else{_LL68: _tmpE6=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpE5)->f1;_LL69:
 return d;}_LL65:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 364
static struct _tuple19*Cyc_Tcutil_copy_eds(int pt,struct _tuple19*e){
# 366
struct _tuple19*_tmpC33;return(_tmpC33=_cycalloc(sizeof(*_tmpC33)),((_tmpC33->f1=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,pt,(e[0]).f1),((_tmpC33->f2=Cyc_Tcutil_deep_copy_exp(pt,(e[0]).f2),_tmpC33)))));}
# 369
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*new_e;
int _tmpEB=preserve_types;
{void*_tmpEC=e->r;void*_tmpED=_tmpEC;int _tmpEE;struct _dyneither_ptr _tmpEF;void*_tmpF0;struct Cyc_Absyn_Exp*_tmpF1;struct _dyneither_ptr*_tmpF2;struct Cyc_Core_Opt*_tmpF3;struct Cyc_List_List*_tmpF4;struct Cyc_Absyn_Exp*_tmpF5;struct Cyc_Absyn_Exp*_tmpF6;int _tmpF7;struct Cyc_Absyn_Exp*_tmpF8;void**_tmpF9;struct Cyc_Absyn_Exp*_tmpFA;int _tmpFB;int _tmpFC;struct _tuple2*_tmpFD;void*_tmpFE;struct Cyc_Absyn_Enumfield*_tmpFF;struct _tuple2*_tmp100;struct Cyc_Absyn_Enumdecl*_tmp101;struct Cyc_Absyn_Enumfield*_tmp102;struct Cyc_List_List*_tmp103;struct Cyc_Absyn_Datatypedecl*_tmp104;struct Cyc_Absyn_Datatypefield*_tmp105;void*_tmp106;struct Cyc_List_List*_tmp107;struct _tuple2*_tmp108;struct Cyc_List_List*_tmp109;struct Cyc_List_List*_tmp10A;struct Cyc_Absyn_Aggrdecl*_tmp10B;struct Cyc_Absyn_Exp*_tmp10C;void*_tmp10D;int _tmp10E;struct Cyc_Absyn_Vardecl*_tmp10F;struct Cyc_Absyn_Exp*_tmp110;struct Cyc_Absyn_Exp*_tmp111;int _tmp112;struct Cyc_List_List*_tmp113;struct _dyneither_ptr*_tmp114;struct Cyc_Absyn_Tqual _tmp115;void*_tmp116;struct Cyc_List_List*_tmp117;struct Cyc_List_List*_tmp118;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp11B;struct _dyneither_ptr*_tmp11C;int _tmp11D;int _tmp11E;struct Cyc_Absyn_Exp*_tmp11F;struct _dyneither_ptr*_tmp120;int _tmp121;int _tmp122;struct Cyc_Absyn_Exp*_tmp123;void*_tmp124;struct Cyc_List_List*_tmp125;struct Cyc_Absyn_Exp*_tmp126;void*_tmp127;struct Cyc_Absyn_Exp*_tmp128;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_Absyn_Exp*_tmp12A;void*_tmp12B;struct Cyc_Absyn_Exp*_tmp12C;int _tmp12D;enum Cyc_Absyn_Coercion _tmp12E;struct Cyc_Absyn_Exp*_tmp12F;struct Cyc_List_List*_tmp130;struct Cyc_Absyn_Exp*_tmp131;struct Cyc_Absyn_Exp*_tmp132;int _tmp133;struct Cyc_Absyn_Exp*_tmp134;struct Cyc_List_List*_tmp135;struct Cyc_Absyn_VarargCallInfo*_tmp136;int _tmp137;struct Cyc_Absyn_Exp*_tmp138;struct Cyc_Absyn_Exp*_tmp139;struct Cyc_Absyn_Exp*_tmp13A;struct Cyc_Absyn_Exp*_tmp13B;struct Cyc_Absyn_Exp*_tmp13C;struct Cyc_Absyn_Exp*_tmp13D;struct Cyc_Absyn_Exp*_tmp13E;struct Cyc_Absyn_Exp*_tmp13F;struct Cyc_Absyn_Exp*_tmp140;struct Cyc_Absyn_Exp*_tmp141;enum Cyc_Absyn_Incrementor _tmp142;struct Cyc_Absyn_Exp*_tmp143;struct Cyc_Core_Opt*_tmp144;struct Cyc_Absyn_Exp*_tmp145;enum Cyc_Absyn_Primop _tmp146;struct Cyc_List_List*_tmp147;struct _tuple2*_tmp148;void*_tmp149;union Cyc_Absyn_Cnst _tmp14A;switch(*((int*)_tmpED)){case 0: _LL6B: _tmp14A=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LL6C:
 new_e=Cyc_Absyn_const_exp(_tmp14A,e->loc);goto _LL6A;case 1: _LL6D: _tmp148=((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp149=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL6E:
 new_e=Cyc_Absyn_varb_exp(_tmp148,_tmp149,e->loc);goto _LL6A;case 2: _LL6F: _tmp146=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp147=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL70:
 new_e=Cyc_Absyn_primop_exp(_tmp146,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpEB,_tmp147),e->loc);goto _LL6A;case 3: _LL71: _tmp143=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp144=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp145=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_LL72:
# 377
{struct Cyc_Core_Opt*_tmpC34;new_e=Cyc_Absyn_assignop_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp143),(unsigned int)_tmp144?(_tmpC34=_cycalloc_atomic(sizeof(*_tmpC34)),((_tmpC34->v=(void*)_tmp144->v,_tmpC34))): 0,Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp145),e->loc);}
goto _LL6A;case 4: _LL73: _tmp141=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp142=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL74:
 new_e=Cyc_Absyn_increment_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp141),_tmp142,e->loc);goto _LL6A;case 5: _LL75: _tmp13E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp13F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp140=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_LL76:
# 381
 new_e=Cyc_Absyn_conditional_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp13E),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp13F),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp140),e->loc);goto _LL6A;case 6: _LL77: _tmp13C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp13D=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL78:
 new_e=Cyc_Absyn_and_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp13C),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp13D),e->loc);goto _LL6A;case 7: _LL79: _tmp13A=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp13B=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL7A:
 new_e=Cyc_Absyn_or_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp13A),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp13B),e->loc);goto _LL6A;case 8: _LL7B: _tmp138=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp139=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL7C:
 new_e=Cyc_Absyn_seq_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp138),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp139),e->loc);goto _LL6A;case 9: _LL7D: _tmp134=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp135=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp136=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_tmp137=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpED)->f4;_LL7E:
# 386
{struct Cyc_Absyn_VarargCallInfo*_tmp14C=_tmp136;int _tmp14D;struct Cyc_List_List*_tmp14E;struct Cyc_Absyn_VarargInfo*_tmp14F;if(_tmp14C != 0){_LLBC: _tmp14D=_tmp14C->num_varargs;_tmp14E=_tmp14C->injectors;_tmp14F=_tmp14C->vai;_LLBD: {
# 388
struct _dyneither_ptr*_tmp151;struct Cyc_Absyn_Tqual _tmp152;void*_tmp153;int _tmp154;struct Cyc_Absyn_VarargInfo*_tmp150=_tmp14F;_tmp151=_tmp150->name;_tmp152=_tmp150->tq;_tmp153=_tmp150->type;_tmp154=_tmp150->inject;
{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpC3E;struct Cyc_Absyn_VarargInfo*_tmpC3D;struct Cyc_Absyn_VarargCallInfo*_tmpC3C;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpC3B;new_e=Cyc_Absyn_new_exp((void*)((_tmpC3B=_cycalloc(sizeof(*_tmpC3B)),((_tmpC3B[0]=((_tmpC3E.tag=9,((_tmpC3E.f1=
Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp134),((_tmpC3E.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpEB,_tmp135),((_tmpC3E.f3=(
(_tmpC3C=_cycalloc(sizeof(*_tmpC3C)),((_tmpC3C->num_varargs=_tmp14D,((_tmpC3C->injectors=_tmp14E,((_tmpC3C->vai=(
(_tmpC3D=_cycalloc(sizeof(*_tmpC3D)),((_tmpC3D->name=_tmp151,((_tmpC3D->tq=_tmp152,((_tmpC3D->type=Cyc_Tcutil_copy_type(_tmp153),((_tmpC3D->inject=_tmp154,_tmpC3D)))))))))),_tmpC3C)))))))),((_tmpC3E.f4=_tmp137,_tmpC3E)))))))))),_tmpC3B)))),e->loc);}
# 394
goto _LLBB;}}else{_LLBE: _LLBF:
# 396
{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpC41;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpC40;new_e=Cyc_Absyn_new_exp((void*)((_tmpC40=_cycalloc(sizeof(*_tmpC40)),((_tmpC40[0]=((_tmpC41.tag=9,((_tmpC41.f1=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp134),((_tmpC41.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpEB,_tmp135),((_tmpC41.f3=_tmp136,((_tmpC41.f4=_tmp137,_tmpC41)))))))))),_tmpC40)))),e->loc);}
goto _LLBB;}_LLBB:;}
# 399
goto _LL6A;case 10: _LL7F: _tmp132=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp133=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL80:
# 401
 new_e=_tmp133?Cyc_Absyn_rethrow_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp132),e->loc): Cyc_Absyn_throw_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp132),e->loc);
goto _LL6A;case 11: _LL81: _tmp131=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LL82:
 new_e=Cyc_Absyn_noinstantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp131),e->loc);
goto _LL6A;case 12: _LL83: _tmp12F=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp130=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL84:
# 406
 new_e=Cyc_Absyn_instantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp12F),((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp130),e->loc);
goto _LL6A;case 13: _LL85: _tmp12B=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp12C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp12D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_tmp12E=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpED)->f4;_LL86:
# 409
 new_e=Cyc_Absyn_cast_exp(Cyc_Tcutil_copy_type(_tmp12B),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp12C),_tmp12D,_tmp12E,e->loc);goto _LL6A;case 14: _LL87: _tmp12A=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LL88:
 new_e=Cyc_Absyn_address_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp12A),e->loc);goto _LL6A;case 15: _LL89: _tmp128=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp129=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL8A: {
# 412
struct Cyc_Absyn_Exp*eo1=_tmp128;if(_tmp128 != 0)eo1=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp128);
new_e=Cyc_Absyn_New_exp(eo1,Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp129),e->loc);
goto _LL6A;}case 16: _LL8B: _tmp127=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LL8C:
 new_e=Cyc_Absyn_sizeoftyp_exp(Cyc_Tcutil_copy_type(_tmp127),e->loc);
goto _LL6A;case 17: _LL8D: _tmp126=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LL8E:
 new_e=Cyc_Absyn_sizeofexp_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp126),e->loc);goto _LL6A;case 18: _LL8F: _tmp124=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp125=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL90:
# 419
 new_e=Cyc_Absyn_offsetof_exp(Cyc_Tcutil_copy_type(_tmp124),_tmp125,e->loc);goto _LL6A;case 19: _LL91: _tmp123=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LL92:
 new_e=Cyc_Absyn_deref_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp123),e->loc);goto _LL6A;case 20: _LL93: _tmp11F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp120=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp121=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_tmp122=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpED)->f4;_LL94:
# 422
{struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpC44;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpC43;new_e=Cyc_Absyn_new_exp((void*)((_tmpC43=_cycalloc(sizeof(*_tmpC43)),((_tmpC43[0]=((_tmpC44.tag=20,((_tmpC44.f1=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp11F),((_tmpC44.f2=_tmp120,((_tmpC44.f3=_tmp121,((_tmpC44.f4=_tmp122,_tmpC44)))))))))),_tmpC43)))),e->loc);}goto _LL6A;case 21: _LL95: _tmp11B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp11C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp11D=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_tmp11E=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpED)->f4;_LL96:
# 424
{struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpC47;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpC46;new_e=Cyc_Absyn_new_exp((void*)((_tmpC46=_cycalloc(sizeof(*_tmpC46)),((_tmpC46[0]=((_tmpC47.tag=21,((_tmpC47.f1=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp11B),((_tmpC47.f2=_tmp11C,((_tmpC47.f3=_tmp11D,((_tmpC47.f4=_tmp11E,_tmpC47)))))))))),_tmpC46)))),e->loc);}goto _LL6A;case 22: _LL97: _tmp119=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp11A=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL98:
 new_e=Cyc_Absyn_subscript_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp119),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp11A),e->loc);
goto _LL6A;case 23: _LL99: _tmp118=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LL9A:
 new_e=Cyc_Absyn_tuple_exp(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpEB,_tmp118),e->loc);goto _LL6A;case 24: _LL9B: _tmp114=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpED)->f1)->f1;_tmp115=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpED)->f1)->f2;_tmp116=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpED)->f1)->f3;_tmp117=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LL9C: {
# 429
struct _dyneither_ptr*vopt1=_tmp114;
if(_tmp114 != 0)vopt1=_tmp114;
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpC4D;struct _tuple10*_tmpC4C;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpC4B;new_e=Cyc_Absyn_new_exp((void*)((_tmpC4B=_cycalloc(sizeof(*_tmpC4B)),((_tmpC4B[0]=((_tmpC4D.tag=24,((_tmpC4D.f1=((_tmpC4C=_cycalloc(sizeof(*_tmpC4C)),((_tmpC4C->f1=vopt1,((_tmpC4C->f2=_tmp115,((_tmpC4C->f3=Cyc_Tcutil_copy_type(_tmp116),_tmpC4C)))))))),((_tmpC4D.f2=
((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpEB,_tmp117),_tmpC4D)))))),_tmpC4B)))),e->loc);}
goto _LL6A;}case 25: _LL9D: _tmp113=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LL9E:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmpC50;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpC4F;new_e=Cyc_Absyn_new_exp((void*)((_tmpC4F=_cycalloc(sizeof(*_tmpC4F)),((_tmpC4F[0]=((_tmpC50.tag=25,((_tmpC50.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpEB,_tmp113),_tmpC50)))),_tmpC4F)))),e->loc);}
goto _LL6A;case 26: _LL9F: _tmp10F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp110=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp111=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_tmp112=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpED)->f4;_LLA0:
# 437
{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpC53;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpC52;new_e=Cyc_Absyn_new_exp((void*)((_tmpC52=_cycalloc(sizeof(*_tmpC52)),((_tmpC52[0]=((_tmpC53.tag=26,((_tmpC53.f1=_tmp10F,((_tmpC53.f2=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp110),((_tmpC53.f3=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp111),((_tmpC53.f4=_tmp112,_tmpC53)))))))))),_tmpC52)))),e->loc);}
goto _LL6A;case 27: _LLA1: _tmp10C=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp10D=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp10E=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_LLA2:
# 440
{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpC56;struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpC55;new_e=Cyc_Absyn_new_exp((void*)((_tmpC55=_cycalloc(sizeof(*_tmpC55)),((_tmpC55[0]=((_tmpC56.tag=27,((_tmpC56.f1=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmp10C),((_tmpC56.f2=Cyc_Tcutil_copy_type(_tmp10D),((_tmpC56.f3=_tmp10E,_tmpC56)))))))),_tmpC55)))),_tmp10C->loc);}
# 442
goto _LL6A;case 28: _LLA3: _tmp108=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp109=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp10A=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_tmp10B=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpED)->f4;_LLA4:
# 444
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpC59;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpC58;new_e=Cyc_Absyn_new_exp((void*)((_tmpC58=_cycalloc(sizeof(*_tmpC58)),((_tmpC58[0]=((_tmpC59.tag=28,((_tmpC59.f1=_tmp108,((_tmpC59.f2=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp109),((_tmpC59.f3=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpEB,_tmp10A),((_tmpC59.f4=_tmp10B,_tmpC59)))))))))),_tmpC58)))),e->loc);}
# 446
goto _LL6A;case 29: _LLA5: _tmp106=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp107=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LLA6:
# 448
{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmpC5C;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpC5B;new_e=Cyc_Absyn_new_exp((void*)((_tmpC5B=_cycalloc(sizeof(*_tmpC5B)),((_tmpC5B[0]=((_tmpC5C.tag=29,((_tmpC5C.f1=Cyc_Tcutil_copy_type(_tmp106),((_tmpC5C.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpEB,_tmp107),_tmpC5C)))))),_tmpC5B)))),e->loc);}
goto _LL6A;case 30: _LLA7: _tmp103=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp104=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp105=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_LLA8:
# 451
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmpC5F;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpC5E;new_e=Cyc_Absyn_new_exp((void*)((_tmpC5E=_cycalloc(sizeof(*_tmpC5E)),((_tmpC5E[0]=((_tmpC5F.tag=30,((_tmpC5F.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpEB,_tmp103),((_tmpC5F.f2=_tmp104,((_tmpC5F.f3=_tmp105,_tmpC5F)))))))),_tmpC5E)))),e->loc);}
goto _LL6A;case 31: _LLA9: _tmp100=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmp101=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmp102=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_LLAA:
 new_e=e;goto _LL6A;case 32: _LLAB: _tmpFD=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmpFE=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_tmpFF=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpED)->f3;_LLAC:
# 455
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpC62;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpC61;new_e=Cyc_Absyn_new_exp((void*)((_tmpC61=_cycalloc(sizeof(*_tmpC61)),((_tmpC61[0]=((_tmpC62.tag=32,((_tmpC62.f1=_tmpFD,((_tmpC62.f2=Cyc_Tcutil_copy_type(_tmpFE),((_tmpC62.f3=_tmpFF,_tmpC62)))))))),_tmpC61)))),e->loc);}
goto _LL6A;case 33: _LLAD: _tmpF7=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpED)->f1).is_calloc;_tmpF8=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpED)->f1).rgn;_tmpF9=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpED)->f1).elt_type;_tmpFA=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpED)->f1).num_elts;_tmpFB=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpED)->f1).fat_result;_tmpFC=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpED)->f1).inline_call;_LLAE: {
# 458
struct Cyc_Absyn_Exp*_tmp170=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmpF8;if(_tmpF8 != 0)r1=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmpF8);{
void**t1=_tmpF9;if(_tmpF9 != 0){void**_tmpC63;t1=((_tmpC63=_cycalloc(sizeof(*_tmpC63)),((_tmpC63[0]=Cyc_Tcutil_copy_type(*_tmpF9),_tmpC63))));}
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpC69;struct Cyc_Absyn_MallocInfo _tmpC68;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpC67;_tmp170->r=(void*)((_tmpC67=_cycalloc(sizeof(*_tmpC67)),((_tmpC67[0]=((_tmpC69.tag=33,((_tmpC69.f1=((_tmpC68.is_calloc=_tmpF7,((_tmpC68.rgn=r1,((_tmpC68.elt_type=t1,((_tmpC68.num_elts=_tmpFA,((_tmpC68.fat_result=_tmpFB,((_tmpC68.inline_call=_tmpFC,_tmpC68)))))))))))),_tmpC69)))),_tmpC67))));}
new_e=_tmp170;
goto _LL6A;};}case 34: _LLAF: _tmpF5=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmpF6=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LLB0:
 new_e=Cyc_Absyn_swap_exp(Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmpF5),Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmpF6),e->loc);goto _LL6A;case 35: _LLB1: _tmpF3=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmpF4=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LLB2: {
# 466
struct Cyc_Core_Opt*nopt1=_tmpF3;
if(_tmpF3 != 0){struct Cyc_Core_Opt*_tmpC6A;nopt1=((_tmpC6A=_cycalloc(sizeof(*_tmpC6A)),((_tmpC6A->v=(struct _tuple2*)_tmpF3->v,_tmpC6A))));}
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpC6D;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpC6C;new_e=Cyc_Absyn_new_exp((void*)((_tmpC6C=_cycalloc(sizeof(*_tmpC6C)),((_tmpC6C[0]=((_tmpC6D.tag=35,((_tmpC6D.f1=nopt1,((_tmpC6D.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpEB,_tmpF4),_tmpC6D)))))),_tmpC6C)))),e->loc);}
goto _LL6A;}case 36: _LLB3: _LLB4: {
# 471
struct Cyc_Core_Failure_exn_struct _tmpC73;const char*_tmpC72;struct Cyc_Core_Failure_exn_struct*_tmpC71;(int)_throw((void*)((_tmpC71=_cycalloc(sizeof(*_tmpC71)),((_tmpC71[0]=((_tmpC73.tag=Cyc_Core_Failure,((_tmpC73.f1=((_tmpC72="deep_copy: statement expressions unsupported",_tag_dyneither(_tmpC72,sizeof(char),45))),_tmpC73)))),_tmpC71)))));}case 37: _LLB5: _tmpF1=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmpF2=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LLB6:
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpC76;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpC75;new_e=Cyc_Absyn_new_exp((void*)((_tmpC75=_cycalloc(sizeof(*_tmpC75)),((_tmpC75[0]=((_tmpC76.tag=37,((_tmpC76.f1=Cyc_Tcutil_deep_copy_exp(_tmpEB,_tmpF1),((_tmpC76.f2=_tmpF2,_tmpC76)))))),_tmpC75)))),e->loc);}
goto _LL6A;case 38: _LLB7: _tmpF0=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_LLB8:
 new_e=Cyc_Absyn_valueof_exp(Cyc_Tcutil_copy_type(_tmpF0),e->loc);
goto _LL6A;default: _LLB9: _tmpEE=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpED)->f1;_tmpEF=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpED)->f2;_LLBA:
 new_e=Cyc_Absyn_asm_exp(_tmpEE,_tmpEF,e->loc);goto _LL6A;}_LL6A:;}
# 479
if(preserve_types)new_e->topt=e->topt;
return new_e;}
# 483
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*e){
if(e == 0)return 0;else{
return Cyc_Tcutil_deep_copy_exp(preserve_types,e);}}struct _tuple20{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple21{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 496 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
enum Cyc_Absyn_KindQual _tmp17E;enum Cyc_Absyn_AliasQual _tmp17F;struct Cyc_Absyn_Kind*_tmp17D=ka1;_tmp17E=_tmp17D->kind;_tmp17F=_tmp17D->aliasqual;{
enum Cyc_Absyn_KindQual _tmp181;enum Cyc_Absyn_AliasQual _tmp182;struct Cyc_Absyn_Kind*_tmp180=ka2;_tmp181=_tmp180->kind;_tmp182=_tmp180->aliasqual;
# 500
if(_tmp17E != _tmp181){
struct _tuple20 _tmpC77;struct _tuple20 _tmp183=(_tmpC77.f1=_tmp17E,((_tmpC77.f2=_tmp181,_tmpC77)));struct _tuple20 _tmp184=_tmp183;switch(_tmp184.f1){case Cyc_Absyn_BoxKind: switch(_tmp184.f2){case Cyc_Absyn_MemKind: _LLC1: _LLC2:
 goto _LLC4;case Cyc_Absyn_AnyKind: _LLC3: _LLC4:
 goto _LLC6;default: goto _LLC7;}case Cyc_Absyn_MemKind: if(_tmp184.f2 == Cyc_Absyn_AnyKind){_LLC5: _LLC6:
 goto _LLC0;}else{goto _LLC7;}default: _LLC7: _LLC8:
 return 0;}_LLC0:;}
# 509
if(_tmp17F != _tmp182){
struct _tuple21 _tmpC78;struct _tuple21 _tmp186=(_tmpC78.f1=_tmp17F,((_tmpC78.f2=_tmp182,_tmpC78)));struct _tuple21 _tmp187=_tmp186;switch(_tmp187.f1){case Cyc_Absyn_Aliasable: if(_tmp187.f2 == Cyc_Absyn_Top){_LLCA: _LLCB:
 goto _LLCD;}else{goto _LLCE;}case Cyc_Absyn_Unique: if(_tmp187.f2 == Cyc_Absyn_Top){_LLCC: _LLCD:
 return 1;}else{goto _LLCE;}default: _LLCE: _LLCF:
 return 0;}_LLC9:;}
# 516
return 1;};}
# 519
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp189=Cyc_Absyn_compress_kb(tv->kind);void*_tmp18A=_tmp189;struct Cyc_Absyn_Kind*_tmp18B;struct Cyc_Absyn_Kind*_tmp18C;switch(*((int*)_tmp18A)){case 0: _LLD1: _tmp18C=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp18A)->f1;_LLD2:
 return _tmp18C;case 2: _LLD3: _tmp18B=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp18A)->f2;_LLD4:
 return _tmp18B;default: _LLD5: _LLD6:
# 524
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpC7B;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC7A;tv->kind=(void*)((_tmpC7A=_cycalloc(sizeof(*_tmpC7A)),((_tmpC7A[0]=((_tmpC7B.tag=2,((_tmpC7B.f1=0,((_tmpC7B.f2=def,_tmpC7B)))))),_tmpC7A))));}
return def;}_LLD0:;}
# 529
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
struct _tuple0 _tmpC7C;struct _tuple0 _tmp18F=(_tmpC7C.f1=Cyc_Absyn_compress_kb(kb1),((_tmpC7C.f2=Cyc_Absyn_compress_kb(kb2),_tmpC7C)));struct _tuple0 _tmp190=_tmp18F;struct Cyc_Core_Opt**_tmp191;void*_tmp192;void*_tmp193;struct Cyc_Core_Opt**_tmp194;struct Cyc_Core_Opt**_tmp195;struct Cyc_Absyn_Kind*_tmp196;struct Cyc_Core_Opt**_tmp197;struct Cyc_Absyn_Kind*_tmp198;struct Cyc_Core_Opt**_tmp199;struct Cyc_Absyn_Kind*_tmp19A;struct Cyc_Absyn_Kind*_tmp19B;struct Cyc_Absyn_Kind*_tmp19C;struct Cyc_Core_Opt**_tmp19D;struct Cyc_Absyn_Kind*_tmp19E;struct Cyc_Absyn_Kind*_tmp19F;struct Cyc_Absyn_Kind*_tmp1A0;switch(*((int*)_tmp190.f1)){case 0: switch(*((int*)_tmp190.f2)){case 0: _LLD8: _tmp19F=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp190.f1)->f1;_tmp1A0=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp190.f2)->f1;_LLD9:
 return _tmp19F == _tmp1A0;case 2: _LLDC: _tmp19C=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp190.f1)->f1;_tmp19D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp190.f2)->f1;_tmp19E=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp190.f2)->f2;_LLDD:
# 538
 if(Cyc_Tcutil_kind_leq(_tmp19C,_tmp19E)){
{struct Cyc_Core_Opt*_tmpC7D;*_tmp19D=((_tmpC7D=_cycalloc(sizeof(*_tmpC7D)),((_tmpC7D->v=kb1,_tmpC7D))));}
return 1;}else{
return 0;}default: goto _LLE2;}case 2: switch(*((int*)_tmp190.f2)){case 0: _LLDA: _tmp199=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp190.f1)->f1;_tmp19A=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp190.f1)->f2;_tmp19B=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp190.f2)->f1;_LLDB:
# 533
 if(Cyc_Tcutil_kind_leq(_tmp19B,_tmp19A)){
{struct Cyc_Core_Opt*_tmpC7E;*_tmp199=((_tmpC7E=_cycalloc(sizeof(*_tmpC7E)),((_tmpC7E->v=kb2,_tmpC7E))));}
return 1;}else{
return 0;}case 2: _LLDE: _tmp195=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp190.f1)->f1;_tmp196=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp190.f1)->f2;_tmp197=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp190.f2)->f1;_tmp198=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp190.f2)->f2;_LLDF:
# 543
 if(Cyc_Tcutil_kind_leq(_tmp196,_tmp198)){
{struct Cyc_Core_Opt*_tmpC7F;*_tmp197=((_tmpC7F=_cycalloc(sizeof(*_tmpC7F)),((_tmpC7F->v=kb1,_tmpC7F))));}
return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp198,_tmp196)){
{struct Cyc_Core_Opt*_tmpC80;*_tmp195=((_tmpC80=_cycalloc(sizeof(*_tmpC80)),((_tmpC80->v=kb2,_tmpC80))));}
return 1;}else{
return 0;}}default: _LLE2: _tmp193=_tmp190.f1;_tmp194=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp190.f2)->f1;_LLE3:
# 552
{struct Cyc_Core_Opt*_tmpC81;*_tmp194=((_tmpC81=_cycalloc(sizeof(*_tmpC81)),((_tmpC81->v=_tmp193,_tmpC81))));}
return 1;}default: _LLE0: _tmp191=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp190.f1)->f1;_tmp192=_tmp190.f2;_LLE1:
# 550
 _tmp193=_tmp192;_tmp194=_tmp191;goto _LLE3;}_LLD7:;}
# 557
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp1A7=Cyc_Tcutil_compress(t);void*_tmp1A8=_tmp1A7;struct Cyc_Absyn_Tvar*_tmp1A9;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A8)->tag == 2){_LLE5: _tmp1A9=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A8)->f1;_LLE6: {
# 560
void*_tmp1AA=_tmp1A9->kind;
_tmp1A9->kind=kb;{
struct _tuple16 _tmpC82;return(_tmpC82.f1=_tmp1A9,((_tmpC82.f2=_tmp1AA,_tmpC82)));};}}else{_LLE7: _LLE8: {
# 564
const char*_tmpC86;void*_tmpC85[1];struct Cyc_String_pa_PrintArg_struct _tmpC84;(_tmpC84.tag=0,((_tmpC84.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpC85[0]=& _tmpC84,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC86="swap_kind: cannot update the kind of %s",_tag_dyneither(_tmpC86,sizeof(char),40))),_tag_dyneither(_tmpC85,sizeof(void*),1)))))));}}_LLE4:;}
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
struct Cyc_Absyn_Kind*_tmp1B3=Cyc_Tcutil_tvar_kind(_tmp1B1,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp1B4=_tmp1B3;switch(((struct Cyc_Absyn_Kind*)_tmp1B4)->kind){case Cyc_Absyn_IntKind: _LLEA: _LLEB:
 goto _LLED;case Cyc_Absyn_AnyKind: _LLEC: _LLED:
# 585
{struct _tuple16*_tmpC89;struct Cyc_List_List*_tmpC88;_tmp1B0=((_tmpC88=_region_malloc(temp,sizeof(*_tmpC88)),((_tmpC88->hd=((_tmpC89=_region_malloc(temp,sizeof(*_tmpC89)),((_tmpC89->f1=_tmp1B1,((_tmpC89->f2=_tmp1B2,_tmpC89)))))),((_tmpC88->tl=_tmp1B0,_tmpC88))))));}goto _LLE9;default: _LLEE: _LLEF:
 goto _LLE9;}_LLE9:;}
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
void*_tmp1B7=Cyc_Tcutil_compress(t);void*_tmp1B8=_tmp1B7;struct Cyc_Absyn_Typedefdecl*_tmp1B9;struct Cyc_Absyn_Exp*_tmp1BA;struct Cyc_Absyn_Kind*_tmp1BB;struct Cyc_Absyn_PtrInfo _tmp1BC;enum Cyc_Absyn_AggrKind _tmp1BD;struct Cyc_List_List*_tmp1BE;struct Cyc_Absyn_AggrdeclImpl*_tmp1BF;int _tmp1C0;struct Cyc_List_List*_tmp1C1;struct Cyc_Absyn_Datatypedecl*_tmp1C2;struct Cyc_Absyn_Datatypefield*_tmp1C3;enum Cyc_Absyn_Size_of _tmp1C4;struct Cyc_Absyn_Tvar*_tmp1C5;struct Cyc_Core_Opt*_tmp1C6;switch(*((int*)_tmp1B8)){case 1: _LLF1: _tmp1C6=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B8)->f1;_LLF2:
 return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp1C6))->v;case 2: _LLF3: _tmp1C5=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1B8)->f1;_LLF4:
 return Cyc_Tcutil_tvar_kind(_tmp1C5,& Cyc_Tcutil_bk);case 0: _LLF5: _LLF6:
 return& Cyc_Tcutil_mk;case 6: _LLF7: _tmp1C4=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1B8)->f2;_LLF8:
# 607
 return(_tmp1C4 == Cyc_Absyn_Int_sz  || _tmp1C4 == Cyc_Absyn_Long_sz)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;case 7: _LLF9: _LLFA:
# 609
 return& Cyc_Tcutil_mk;case 9: _LLFB: _LLFC:
 return& Cyc_Tcutil_ak;case 16: _LLFD: _LLFE:
 goto _LL100;case 13: _LLFF: _LL100:
 goto _LL102;case 14: _LL101: _LL102:
 goto _LL104;case 15: _LL103: _LL104:
 return& Cyc_Tcutil_bk;case 21: _LL105: _LL106:
 return& Cyc_Tcutil_urk;case 20: _LL107: _LL108:
 return& Cyc_Tcutil_rk;case 22: _LL109: _LL10A:
 return& Cyc_Tcutil_trk;case 3: _LL10B: _LL10C:
# 620
 return& Cyc_Tcutil_ak;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1B8)->f1).field_info).KnownDatatypefield).tag == 2){_LL10D: _tmp1C2=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1B8)->f1).field_info).KnownDatatypefield).val).f1;_tmp1C3=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1B8)->f1).field_info).KnownDatatypefield).val).f2;_LL10E:
# 622
 return& Cyc_Tcutil_mk;}else{_LL10F: _LL110: {
# 624
const char*_tmpC8C;void*_tmpC8B;(_tmpC8B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC8C="typ_kind: Unresolved DatatypeFieldType",_tag_dyneither(_tmpC8C,sizeof(char),39))),_tag_dyneither(_tmpC8B,sizeof(void*),0)));}}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B8)->f1).aggr_info).UnknownAggr).tag == 1){_LL111: _LL112:
# 627
 return& Cyc_Tcutil_ak;}else{_LL113: _tmp1BD=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B8)->f1).aggr_info).KnownAggr).val)->kind;_tmp1BE=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B8)->f1).aggr_info).KnownAggr).val)->tvs;_tmp1BF=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B8)->f1).aggr_info).KnownAggr).val)->impl;_tmp1C0=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B8)->f1).aggr_info).KnownAggr).val)->expected_mem_kind;_tmp1C1=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B8)->f1).targs;_LL114:
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
return& Cyc_Tcutil_mk;};}case 12: _LL115: _LL116:
 return& Cyc_Tcutil_mk;case 5: _LL117: _tmp1BC=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B8)->f1;_LL118: {
# 655
void*_tmp1CE=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,(_tmp1BC.ptr_atts).bounds);void*_tmp1CF=_tmp1CE;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1CF)->tag == 0){_LL134: _LL135: {
# 657
enum Cyc_Absyn_AliasQual _tmp1D0=(Cyc_Tcutil_typ_kind((_tmp1BC.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp1D1=_tmp1D0;switch(_tmp1D1){case Cyc_Absyn_Aliasable: _LL139: _LL13A:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL13B: _LL13C:
 return& Cyc_Tcutil_umk;default: _LL13D: _LL13E:
 return& Cyc_Tcutil_tmk;}_LL138:;}}else{_LL136: _LL137: {
# 663
enum Cyc_Absyn_AliasQual _tmp1D2=(Cyc_Tcutil_typ_kind((_tmp1BC.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp1D3=_tmp1D2;switch(_tmp1D3){case Cyc_Absyn_Aliasable: _LL140: _LL141:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL142: _LL143:
 return& Cyc_Tcutil_ubk;default: _LL144: _LL145:
 return& Cyc_Tcutil_tbk;}_LL13F:;}}_LL133:;}case 18: _LL119: _LL11A:
# 669
 return& Cyc_Tcutil_ik;case 28: _LL11B: _tmp1BB=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp1B8)->f2;_LL11C:
 return _tmp1BB;case 27: _LL11D: _LL11E:
# 674
 return& Cyc_Tcutil_ak;case 19: _LL11F: _LL120:
 return& Cyc_Tcutil_bk;case 8: _LL121: _tmp1BA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1B8)->f1).num_elts;_LL122:
# 677
 if(_tmp1BA == 0  || Cyc_Tcutil_is_const_exp(_tmp1BA))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;case 10: _LL123: _LL124:
 return& Cyc_Tcutil_mk;case 17: _LL125: _tmp1B9=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1B8)->f3;_LL126:
# 681
 if(_tmp1B9 == 0  || _tmp1B9->kind == 0){
const char*_tmpC90;void*_tmpC8F[1];struct Cyc_String_pa_PrintArg_struct _tmpC8E;(_tmpC8E.tag=0,((_tmpC8E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpC8F[0]=& _tmpC8E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC90="typ_kind: typedef found: %s",_tag_dyneither(_tmpC90,sizeof(char),28))),_tag_dyneither(_tmpC8F,sizeof(void*),1)))))));}
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp1B9->kind))->v;case 26: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1B8)->f1)->r)){case 0: _LL127: _LL128:
 return& Cyc_Tcutil_ak;case 1: _LL129: _LL12A:
 return& Cyc_Tcutil_bk;default: _LL12B: _LL12C:
 return& Cyc_Tcutil_ak;}case 23: _LL12D: _LL12E:
 goto _LL130;case 24: _LL12F: _LL130:
 goto _LL132;default: _LL131: _LL132:
 return& Cyc_Tcutil_ek;}_LLF0:;}
# 693
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2  || k1->kind == k2->kind  && k1->aliasqual == k2->aliasqual;}
# 698
int Cyc_Tcutil_unify(void*t1,void*t2){
struct _handler_cons _tmp1D7;_push_handler(& _tmp1D7);{int _tmp1D9=0;if(setjmp(_tmp1D7.handler))_tmp1D9=1;if(!_tmp1D9){
Cyc_Tcutil_unify_it(t1,t2);{
int _tmp1DA=1;_npop_handler(0);return _tmp1DA;};
# 700
;_pop_handler();}else{void*_tmp1D8=(void*)_exn_thrown;void*_tmp1DB=_tmp1D8;void*_tmp1DC;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp1DB)->tag == Cyc_Tcutil_Unify){_LL147: _LL148:
# 702
 return 0;}else{_LL149: _tmp1DC=_tmp1DB;_LL14A:(int)_rethrow(_tmp1DC);}_LL146:;}};}
# 707
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
t=Cyc_Tcutil_compress(t);{
void*_tmp1DD=t;struct Cyc_List_List*_tmp1DE;void*_tmp1DF;void*_tmp1E0;void*_tmp1E1;void*_tmp1E2;struct Cyc_List_List*_tmp1E3;struct Cyc_List_List*_tmp1E4;struct Cyc_List_List*_tmp1E5;struct Cyc_List_List*_tmp1E6;struct Cyc_List_List*_tmp1E7;struct Cyc_List_List*_tmp1E8;struct Cyc_List_List*_tmp1E9;void*_tmp1EA;struct Cyc_Absyn_Tqual _tmp1EB;void*_tmp1EC;struct Cyc_List_List*_tmp1ED;int _tmp1EE;struct Cyc_Absyn_VarargInfo*_tmp1EF;struct Cyc_List_List*_tmp1F0;struct Cyc_List_List*_tmp1F1;struct Cyc_Absyn_Exp*_tmp1F2;struct Cyc_List_List*_tmp1F3;struct Cyc_Absyn_Exp*_tmp1F4;struct Cyc_List_List*_tmp1F5;void*_tmp1F6;struct Cyc_Absyn_PtrInfo _tmp1F7;void*_tmp1F8;struct Cyc_Core_Opt**_tmp1F9;struct Cyc_Absyn_Tvar*_tmp1FA;switch(*((int*)_tmp1DD)){case 2: _LL14C: _tmp1FA=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1DD)->f1;_LL14D:
# 712
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp1FA)){
{const char*_tmpC91;Cyc_Tcutil_failure_reason=((_tmpC91="(type variable would escape scope)",_tag_dyneither(_tmpC91,sizeof(char),35)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 716
goto _LL14B;case 1: _LL14E: _tmp1F8=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1DD)->f2;_tmp1F9=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1DD)->f4;_LL14F:
# 718
 if(t == evar){
{const char*_tmpC92;Cyc_Tcutil_failure_reason=((_tmpC92="(occurs check)",_tag_dyneither(_tmpC92,sizeof(char),15)));}
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
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
struct Cyc_List_List*_tmpC93;_tmp1FD=((_tmpC93=_cycalloc(sizeof(*_tmpC93)),((_tmpC93->hd=(struct Cyc_Absyn_Tvar*)s->hd,((_tmpC93->tl=_tmp1FD,_tmpC93))))));}}}{
# 738
struct Cyc_Core_Opt*_tmpC94;*_tmp1F9=((_tmpC94=_cycalloc(sizeof(*_tmpC94)),((_tmpC94->v=_tmp1FD,_tmpC94))));};}}}
# 741
goto _LL14B;case 5: _LL150: _tmp1F7=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1DD)->f1;_LL151:
# 744
 Cyc_Tcutil_occurs(evar,r,env,_tmp1F7.elt_typ);
Cyc_Tcutil_occurs(evar,r,env,(_tmp1F7.ptr_atts).rgn);
goto _LL14B;case 8: _LL152: _tmp1F6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1DD)->f1).elt_type;_LL153:
# 748
 Cyc_Tcutil_occurs(evar,r,env,_tmp1F6);goto _LL14B;case 9: _LL154: _tmp1E9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DD)->f1).tvars;_tmp1EA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DD)->f1).effect;_tmp1EB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DD)->f1).ret_tqual;_tmp1EC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DD)->f1).ret_typ;_tmp1ED=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DD)->f1).args;_tmp1EE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DD)->f1).c_varargs;_tmp1EF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DD)->f1).cyc_varargs;_tmp1F0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DD)->f1).rgn_po;_tmp1F1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DD)->f1).attributes;_tmp1F2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DD)->f1).requires_clause;_tmp1F3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DD)->f1).requires_relns;_tmp1F4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DD)->f1).ensures_clause;_tmp1F5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DD)->f1).ensures_relns;_LL155:
# 751
 env=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp1E9,env);
if(_tmp1EA != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp1EA);
Cyc_Tcutil_occurs(evar,r,env,_tmp1EC);
for(0;_tmp1ED != 0;_tmp1ED=_tmp1ED->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple10*)_tmp1ED->hd)).f3);}
if(_tmp1EF != 0)
Cyc_Tcutil_occurs(evar,r,env,_tmp1EF->type);
for(0;_tmp1F0 != 0;_tmp1F0=_tmp1F0->tl){
struct _tuple0*_tmp200=(struct _tuple0*)_tmp1F0->hd;void*_tmp202;void*_tmp203;struct _tuple0*_tmp201=_tmp200;_tmp202=_tmp201->f1;_tmp203=_tmp201->f2;
Cyc_Tcutil_occurs(evar,r,env,_tmp202);
Cyc_Tcutil_occurs(evar,r,env,_tmp203);}
# 763
goto _LL14B;case 10: _LL156: _tmp1E8=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1DD)->f1;_LL157:
# 765
 for(0;_tmp1E8 != 0;_tmp1E8=_tmp1E8->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple12*)_tmp1E8->hd)).f2);}
goto _LL14B;case 3: _LL158: _tmp1E7=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1DD)->f1).targs;_LL159:
# 769
 Cyc_Tcutil_occurslist(evar,r,env,_tmp1E7);goto _LL14B;case 17: _LL15A: _tmp1E6=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1DD)->f2;_LL15B:
 _tmp1E5=_tmp1E6;goto _LL15D;case 4: _LL15C: _tmp1E5=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1DD)->f1).targs;_LL15D:
 _tmp1E4=_tmp1E5;goto _LL15F;case 11: _LL15E: _tmp1E4=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1DD)->f1).targs;_LL15F:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp1E4);goto _LL14B;case 12: _LL160: _tmp1E3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1DD)->f2;_LL161:
# 775
 for(0;_tmp1E3 != 0;_tmp1E3=_tmp1E3->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp1E3->hd)->type);}
goto _LL14B;case 19: _LL162: _tmp1E2=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp1DD)->f1;_LL163:
 _tmp1E1=_tmp1E2;goto _LL165;case 23: _LL164: _tmp1E1=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp1DD)->f1;_LL165:
 _tmp1E0=_tmp1E1;goto _LL167;case 15: _LL166: _tmp1E0=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1DD)->f1;_LL167:
 _tmp1DF=_tmp1E0;goto _LL169;case 25: _LL168: _tmp1DF=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp1DD)->f1;_LL169:
 Cyc_Tcutil_occurs(evar,r,env,_tmp1DF);goto _LL14B;case 24: _LL16A: _tmp1DE=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp1DD)->f1;_LL16B:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp1DE);goto _LL14B;default: _LL16C: _LL16D:
# 785
 goto _LL14B;}_LL14B:;};}
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
const char*_tmpC97;void*_tmpC96;(_tmpC96=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC97="tq1 real_const not set.",_tag_dyneither(_tmpC97,sizeof(char),24))),_tag_dyneither(_tmpC96,sizeof(void*),0)));}
if(tq2.print_const  && !tq2.real_const){
const char*_tmpC9A;void*_tmpC99;(_tmpC99=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC9A="tq2 real_const not set.",_tag_dyneither(_tmpC9A,sizeof(char),24))),_tag_dyneither(_tmpC99,sizeof(void*),0)));}
# 809
if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
# 812
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;
{const char*_tmpC9B;Cyc_Tcutil_failure_reason=((_tmpC9B="(qualifiers don't match)",_tag_dyneither(_tmpC9B,sizeof(char),25)));}
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
union Cyc_Absyn_Constraint*_tmp209=x;void*_tmp20A;switch((((union Cyc_Absyn_Constraint*)_tmp209)->Eq_constr).tag){case 3: _LL16F: _LL170:
# 838
{union Cyc_Absyn_Constraint _tmpC9C;*x=(((_tmpC9C.Forward_constr).val=y,(((_tmpC9C.Forward_constr).tag=2,_tmpC9C))));}return;case 1: _LL171: _tmp20A=(void*)(_tmp209->Eq_constr).val;_LL172: {
# 840
union Cyc_Absyn_Constraint*_tmp20C=y;void*_tmp20D;switch((((union Cyc_Absyn_Constraint*)_tmp20C)->Eq_constr).tag){case 3: _LL176: _LL177:
*y=*x;return;case 1: _LL178: _tmp20D=(void*)(_tmp20C->Eq_constr).val;_LL179:
# 843
 if(cmp(_tmp20A,_tmp20D)!= 0){
Cyc_Tcutil_failure_reason=reason;
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 847
return;default: _LL17A: _LL17B: {
const char*_tmpC9F;void*_tmpC9E;(_tmpC9E=0,Cyc_Tcutil_impos(((_tmpC9F="unify_conref: forward after compress(2)",_tag_dyneither(_tmpC9F,sizeof(char),40))),_tag_dyneither(_tmpC9E,sizeof(void*),0)));}}_LL175:;}default: _LL173: _LL174: {
# 850
const char*_tmpCA2;void*_tmpCA1;(_tmpCA1=0,Cyc_Tcutil_impos(((_tmpCA2="unify_conref: forward after compress",_tag_dyneither(_tmpCA2,sizeof(char),37))),_tag_dyneither(_tmpCA1,sizeof(void*),0)));}}_LL16E:;};}
# 854
static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
struct _handler_cons _tmp212;_push_handler(& _tmp212);{int _tmp214=0;if(setjmp(_tmp212.handler))_tmp214=1;if(!_tmp214){
Cyc_Tcutil_unify_it_conrefs(cmp,x,y,_tag_dyneither(0,0,0));{
int _tmp215=1;_npop_handler(0);return _tmp215;};
# 856
;_pop_handler();}else{void*_tmp213=(void*)_exn_thrown;void*_tmp216=_tmp213;void*_tmp217;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp216)->tag == Cyc_Tcutil_Unify){_LL17D: _LL17E:
# 858
 return 0;}else{_LL17F: _tmp217=_tmp216;_LL180:(int)_rethrow(_tmp217);}_LL17C:;}};}
# 861
static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmpCA3;struct _tuple0 _tmp218=(_tmpCA3.f1=b1,((_tmpCA3.f2=b2,_tmpCA3)));struct _tuple0 _tmp219=_tmp218;struct Cyc_Absyn_Exp*_tmp21A;struct Cyc_Absyn_Exp*_tmp21B;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp219.f1)->tag == 0){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp219.f2)->tag == 0){_LL182: _LL183:
 return 0;}else{_LL184: _LL185:
 return - 1;}}else{if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp219.f2)->tag == 0){_LL186: _LL187:
 return 1;}else{_LL188: _tmp21A=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp219.f1)->f1;_tmp21B=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp219.f2)->f1;_LL189:
# 867
 return Cyc_Evexp_const_exp_cmp(_tmp21A,_tmp21B);}}_LL181:;}
# 871
static void Cyc_Tcutil_unify_cmp_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
{struct _tuple0 _tmpCA4;struct _tuple0 _tmp21D=(_tmpCA4.f1=e1->r,((_tmpCA4.f2=e2->r,_tmpCA4)));struct _tuple0 _tmp21E=_tmp21D;void*_tmp21F;void*_tmp220;struct Cyc_Absyn_Exp*_tmp221;struct Cyc_Absyn_Exp*_tmp222;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp21E.f1)->tag == 13){_LL18B: _tmp222=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp21E.f1)->f2;_LL18C:
# 877
 Cyc_Tcutil_unify_cmp_exp(_tmp222,e2);
return;}else{if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp21E.f2)->tag == 13){_LL18D: _tmp221=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp21E.f2)->f2;_LL18E:
# 880
 Cyc_Tcutil_unify_cmp_exp(e1,_tmp221);
return;}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp21E.f1)->tag == 38){_LL18F: _tmp220=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp21E.f1)->f1;_LL190:
# 883
{void*_tmp223=Cyc_Tcutil_compress(_tmp220);void*_tmp224=_tmp223;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp224)->tag == 1){_LL196: _LL197:
# 885
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpCA7;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpCA6;Cyc_Tcutil_unify_it(_tmp220,(void*)((_tmpCA6=_cycalloc(sizeof(*_tmpCA6)),((_tmpCA6[0]=((_tmpCA7.tag=18,((_tmpCA7.f1=e2,_tmpCA7)))),_tmpCA6)))));}
return;}else{_LL198: _LL199:
# 888
 goto _LL195;}_LL195:;}
# 890
goto _LL18A;}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp21E.f2)->tag == 38){_LL191: _tmp21F=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp21E.f2)->f1;_LL192:
# 892
{void*_tmp227=Cyc_Tcutil_compress(_tmp21F);void*_tmp228=_tmp227;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp228)->tag == 1){_LL19B: _LL19C:
# 894
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpCAA;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpCA9;Cyc_Tcutil_unify_it(_tmp21F,(void*)((_tmpCA9=_cycalloc(sizeof(*_tmpCA9)),((_tmpCA9[0]=((_tmpCAA.tag=18,((_tmpCAA.f1=e1,_tmpCAA)))),_tmpCA9)))));}
return;}else{_LL19D: _LL19E:
# 897
 goto _LL19A;}_LL19A:;}
# 899
goto _LL18A;}else{_LL193: _LL194:
 goto _LL18A;}}}}_LL18A:;}
# 902
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 905
static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){
struct _tuple0 _tmpCAB;struct _tuple0 _tmp22C=(_tmpCAB.f1=b1,((_tmpCAB.f2=b2,_tmpCAB)));struct _tuple0 _tmp22D=_tmp22C;struct Cyc_Absyn_Exp*_tmp22E;struct Cyc_Absyn_Exp*_tmp22F;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp22D.f1)->tag == 0){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp22D.f2)->tag == 0){_LL1A0: _LL1A1:
 return 0;}else{_LL1A2: _LL1A3:
 return - 1;}}else{if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp22D.f2)->tag == 0){_LL1A4: _LL1A5:
 return 1;}else{_LL1A6: _tmp22E=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp22D.f1)->f1;_tmp22F=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp22D.f2)->f1;_LL1A7: {
# 911
int r=Cyc_Evexp_const_exp_cmp(_tmp22E,_tmp22F);
if(r == 0)return 0;{
struct _handler_cons _tmp230;_push_handler(& _tmp230);{int _tmp232=0;if(setjmp(_tmp230.handler))_tmp232=1;if(!_tmp232){
Cyc_Tcutil_unify_cmp_exp(_tmp22E,_tmp22F);{
int _tmp233=0;_npop_handler(0);return _tmp233;};
# 914
;_pop_handler();}else{void*_tmp231=(void*)_exn_thrown;void*_tmp234=_tmp231;void*_tmp235;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp234)->tag == Cyc_Tcutil_Unify){_LL1A9: _LL1AA:
# 917
 return r;}else{_LL1AB: _tmp235=_tmp234;_LL1AC:(int)_rethrow(_tmp235);}_LL1A8:;}};};}}}_LL19F:;}
# 922
static int Cyc_Tcutil_attribute_case_number(void*att){
void*_tmp237=att;switch(*((int*)_tmp237)){case 0: _LL1AE: _LL1AF:
 return 0;case 1: _LL1B0: _LL1B1:
 return 1;case 2: _LL1B2: _LL1B3:
 return 2;case 3: _LL1B4: _LL1B5:
 return 3;case 4: _LL1B6: _LL1B7:
 return 4;case 5: _LL1B8: _LL1B9:
 return 5;case 6: _LL1BA: _LL1BB:
 return 6;case 7: _LL1BC: _LL1BD:
 return 7;case 8: _LL1BE: _LL1BF:
 return 8;case 9: _LL1C0: _LL1C1:
 return 9;case 10: _LL1C2: _LL1C3:
 return 10;case 11: _LL1C4: _LL1C5:
 return 11;case 12: _LL1C6: _LL1C7:
 return 12;case 13: _LL1C8: _LL1C9:
 return 13;case 14: _LL1CA: _LL1CB:
 return 14;case 15: _LL1CC: _LL1CD:
 return 15;case 16: _LL1CE: _LL1CF:
 return 16;case 17: _LL1D0: _LL1D1:
 return 17;case 18: _LL1D2: _LL1D3:
 return 18;case 19: _LL1D4: _LL1D5:
 return 19;case 20: _LL1D6: _LL1D7:
 return 20;default: _LL1D8: _LL1D9:
 return 21;}_LL1AD:;}
# 949
static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple0 _tmpCAC;struct _tuple0 _tmp238=(_tmpCAC.f1=att1,((_tmpCAC.f2=att2,_tmpCAC)));struct _tuple0 _tmp239=_tmp238;enum Cyc_Absyn_Format_Type _tmp23A;int _tmp23B;int _tmp23C;enum Cyc_Absyn_Format_Type _tmp23D;int _tmp23E;int _tmp23F;struct _dyneither_ptr _tmp240;struct _dyneither_ptr _tmp241;struct Cyc_Absyn_Exp*_tmp242;struct Cyc_Absyn_Exp*_tmp243;int _tmp244;int _tmp245;int _tmp246;int _tmp247;switch(*((int*)_tmp239.f1)){case 0: if(((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp239.f2)->tag == 0){_LL1DB: _tmp246=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp239.f1)->f1;_tmp247=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp239.f2)->f1;_LL1DC:
 _tmp244=_tmp246;_tmp245=_tmp247;goto _LL1DE;}else{goto _LL1E5;}case 20: if(((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp239.f2)->tag == 20){_LL1DD: _tmp244=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp239.f1)->f1;_tmp245=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp239.f2)->f1;_LL1DE:
# 953
 return Cyc_Core_intcmp(_tmp244,_tmp245);}else{goto _LL1E5;}case 6: if(((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp239.f2)->tag == 6){_LL1DF: _tmp242=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp239.f1)->f1;_tmp243=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp239.f2)->f1;_LL1E0:
# 955
 if(_tmp242 == _tmp243)return 0;
if(_tmp242 == 0)return - 1;
if(_tmp243 == 0)return 1;
return Cyc_Evexp_const_exp_cmp(_tmp242,_tmp243);}else{goto _LL1E5;}case 8: if(((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp239.f2)->tag == 8){_LL1E1: _tmp240=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp239.f1)->f1;_tmp241=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp239.f2)->f1;_LL1E2:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp240,(struct _dyneither_ptr)_tmp241);}else{goto _LL1E5;}case 19: if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp239.f2)->tag == 19){_LL1E3: _tmp23A=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp239.f1)->f1;_tmp23B=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp239.f1)->f2;_tmp23C=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp239.f1)->f3;_tmp23D=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp239.f2)->f1;_tmp23E=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp239.f2)->f2;_tmp23F=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp239.f2)->f3;_LL1E4: {
# 961
int _tmp248=Cyc_Core_intcmp((int)((unsigned int)_tmp23A),(int)((unsigned int)_tmp23D));
if(_tmp248 != 0)return _tmp248;{
int _tmp249=Cyc_Core_intcmp(_tmp23B,_tmp23E);
if(_tmp249 != 0)return _tmp249;
return Cyc_Core_intcmp(_tmp23C,_tmp23F);};}}else{goto _LL1E5;}default: _LL1E5: _LL1E6:
# 967
 return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(att2));}_LL1DA:;}
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
{struct Cyc_Absyn_Kind*_tmp24B=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp24C=_tmp24B;switch(((struct Cyc_Absyn_Kind*)_tmp24C)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp24C)->aliasqual){case Cyc_Absyn_Unique: _LL1E8: _LL1E9:
 t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL1E7;case Cyc_Absyn_Aliasable: _LL1EA: _LL1EB:
 t=(void*)& Cyc_Absyn_HeapRgn_val;goto _LL1E7;default: goto _LL1F0;}case Cyc_Absyn_EffKind: _LL1EC: _LL1ED:
 t=Cyc_Absyn_empty_effect;goto _LL1E7;case Cyc_Absyn_IntKind: _LL1EE: _LL1EF:
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpCAF;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpCAE;t=(void*)((_tmpCAE=_cycalloc(sizeof(*_tmpCAE)),((_tmpCAE[0]=((_tmpCAF.tag=18,((_tmpCAF.f1=Cyc_Absyn_uint_exp(0,0),_tmpCAF)))),_tmpCAE))));}goto _LL1E7;default: _LL1F0: _LL1F1:
 t=Cyc_Absyn_sint_typ;goto _LL1E7;}_LL1E7:;}{
# 999
struct _tuple16*_tmpCB0;return(_tmpCB0=_cycalloc(sizeof(*_tmpCB0)),((_tmpCB0->f1=tv,((_tmpCB0->f2=t,_tmpCB0)))));};}
# 1006
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp250=Cyc_Tcutil_compress(t);void*_tmp251=_tmp250;struct Cyc_List_List*_tmp252;void*_tmp253;struct Cyc_List_List*_tmp254;void*_tmp255;struct Cyc_Absyn_Tqual _tmp256;void*_tmp257;struct Cyc_List_List*_tmp258;struct Cyc_Absyn_VarargInfo*_tmp259;struct Cyc_List_List*_tmp25A;struct Cyc_List_List*_tmp25B;struct Cyc_List_List*_tmp25C;struct Cyc_List_List*_tmp25D;struct Cyc_List_List*_tmp25E;void*_tmp25F;void*_tmp260;void*_tmp261;struct Cyc_List_List*_tmp262;void*_tmp263;void*_tmp264;void*_tmp265;switch(*((int*)_tmp251)){case 0: _LL1F3: _LL1F4:
 goto _LL1F6;case 7: _LL1F5: _LL1F6:
 goto _LL1F8;case 13: _LL1F7: _LL1F8:
 goto _LL1FA;case 14: _LL1F9: _LL1FA:
 goto _LL1FC;case 18: _LL1FB: _LL1FC:
 goto _LL1FE;case 27: _LL1FD: _LL1FE:
 goto _LL200;case 28: _LL1FF: _LL200:
 goto _LL202;case 6: _LL201: _LL202:
 return Cyc_Absyn_empty_effect;case 1: _LL203: _LL204:
 goto _LL206;case 2: _LL205: _LL206: {
# 1018
struct Cyc_Absyn_Kind*_tmp266=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp267=_tmp266;switch(((struct Cyc_Absyn_Kind*)_tmp267)->kind){case Cyc_Absyn_RgnKind: _LL22E: _LL22F: {
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpCB3;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpCB2;return(void*)((_tmpCB2=_cycalloc(sizeof(*_tmpCB2)),((_tmpCB2[0]=((_tmpCB3.tag=23,((_tmpCB3.f1=t,_tmpCB3)))),_tmpCB2))));}case Cyc_Absyn_EffKind: _LL230: _LL231:
 return t;case Cyc_Absyn_IntKind: _LL232: _LL233:
 return Cyc_Absyn_empty_effect;default: _LL234: _LL235: {
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpCB6;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpCB5;return(void*)((_tmpCB5=_cycalloc(sizeof(*_tmpCB5)),((_tmpCB5[0]=((_tmpCB6.tag=25,((_tmpCB6.f1=t,_tmpCB6)))),_tmpCB5))));}}_LL22D:;}case 15: _LL207: _tmp265=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp251)->f1;_LL208: {
# 1024
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpCB9;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpCB8;return(void*)((_tmpCB8=_cycalloc(sizeof(*_tmpCB8)),((_tmpCB8[0]=((_tmpCB9.tag=23,((_tmpCB9.f1=_tmp265,_tmpCB9)))),_tmpCB8))));}case 16: _LL209: _tmp263=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp251)->f1;_tmp264=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp251)->f2;_LL20A: {
# 1028
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpCBC;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpCBB;return(void*)((_tmpCBB=_cycalloc(sizeof(*_tmpCBB)),((_tmpCBB[0]=((_tmpCBC.tag=23,((_tmpCBC.f1=_tmp264,_tmpCBC)))),_tmpCBB))));}case 3: _LL20B: _tmp262=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp251)->f1).targs;_LL20C: {
# 1030
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp262);
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCBF;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCBE;return Cyc_Tcutil_normalize_effect((void*)((_tmpCBE=_cycalloc(sizeof(*_tmpCBE)),((_tmpCBE[0]=((_tmpCBF.tag=24,((_tmpCBF.f1=ts,_tmpCBF)))),_tmpCBE)))));}case 5: _LL20D: _tmp260=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp251)->f1).elt_typ;_tmp261=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp251)->f1).ptr_atts).rgn;_LL20E: {
# 1034
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCCE;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpCCD;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpCCC;void*_tmpCCB[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCCA;return Cyc_Tcutil_normalize_effect((void*)((_tmpCCA=_cycalloc(sizeof(*_tmpCCA)),((_tmpCCA[0]=((_tmpCCE.tag=24,((_tmpCCE.f1=((_tmpCCB[1]=Cyc_Tcutil_rgns_of(_tmp260),((_tmpCCB[0]=(void*)((_tmpCCD=_cycalloc(sizeof(*_tmpCCD)),((_tmpCCD[0]=((_tmpCCC.tag=23,((_tmpCCC.f1=_tmp261,_tmpCCC)))),_tmpCCD)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCCB,sizeof(void*),2)))))),_tmpCCE)))),_tmpCCA)))));}case 8: _LL20F: _tmp25F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp251)->f1).elt_type;_LL210:
# 1036
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp25F));case 10: _LL211: _tmp25E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp251)->f1;_LL212: {
# 1038
struct Cyc_List_List*_tmp277=0;
for(0;_tmp25E != 0;_tmp25E=_tmp25E->tl){
struct Cyc_List_List*_tmpCCF;_tmp277=((_tmpCCF=_cycalloc(sizeof(*_tmpCCF)),((_tmpCCF->hd=(*((struct _tuple12*)_tmp25E->hd)).f2,((_tmpCCF->tl=_tmp277,_tmpCCF))))));}
_tmp25D=_tmp277;goto _LL214;}case 4: _LL213: _tmp25D=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp251)->f1).targs;_LL214:
 _tmp25C=_tmp25D;goto _LL216;case 11: _LL215: _tmp25C=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp251)->f1).targs;_LL216: {
# 1044
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCD2;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCD1;return Cyc_Tcutil_normalize_effect((void*)((_tmpCD1=_cycalloc(sizeof(*_tmpCD1)),((_tmpCD1[0]=((_tmpCD2.tag=24,((_tmpCD2.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp25C),_tmpCD2)))),_tmpCD1)))));}case 12: _LL217: _tmp25B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp251)->f2;_LL218: {
# 1046
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCD5;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCD4;return Cyc_Tcutil_normalize_effect((void*)((_tmpCD4=_cycalloc(sizeof(*_tmpCD4)),((_tmpCD4[0]=((_tmpCD5.tag=24,((_tmpCD5.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp25B),_tmpCD5)))),_tmpCD4)))));}case 19: _LL219: _LL21A:
 return Cyc_Absyn_empty_effect;case 9: _LL21B: _tmp254=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp251)->f1).tvars;_tmp255=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp251)->f1).effect;_tmp256=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp251)->f1).ret_tqual;_tmp257=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp251)->f1).ret_typ;_tmp258=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp251)->f1).args;_tmp259=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp251)->f1).cyc_varargs;_tmp25A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp251)->f1).rgn_po;_LL21C: {
# 1056
void*_tmp27D=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp254),(void*)_check_null(_tmp255));
return Cyc_Tcutil_normalize_effect(_tmp27D);}case 21: _LL21D: _LL21E:
 goto _LL220;case 22: _LL21F: _LL220:
 goto _LL222;case 20: _LL221: _LL222:
 return Cyc_Absyn_empty_effect;case 23: _LL223: _LL224:
 goto _LL226;case 24: _LL225: _LL226:
 return t;case 25: _LL227: _tmp253=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp251)->f1;_LL228:
 return Cyc_Tcutil_rgns_of(_tmp253);case 17: _LL229: _tmp252=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp251)->f2;_LL22A: {
# 1065
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCD8;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCD7;return Cyc_Tcutil_normalize_effect((void*)((_tmpCD7=_cycalloc(sizeof(*_tmpCD7)),((_tmpCD7[0]=((_tmpCD8.tag=24,((_tmpCD8.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp252),_tmpCD8)))),_tmpCD7)))));}default: _LL22B: _LL22C: {
const char*_tmpCDB;void*_tmpCDA;(_tmpCDA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCDB="typedecl in rgns_of",_tag_dyneither(_tmpCDB,sizeof(char),20))),_tag_dyneither(_tmpCDA,sizeof(void*),0)));}}_LL1F2:;}
# 1073
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp282=e;void*_tmp283;struct Cyc_List_List**_tmp284;switch(*((int*)_tmp282)){case 24: _LL237: _tmp284=(struct Cyc_List_List**)&((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp282)->f1;_LL238: {
# 1077
int redo_join=0;
{struct Cyc_List_List*effs=*_tmp284;for(0;effs != 0;effs=effs->tl){
void*_tmp285=(void*)effs->hd;
effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp285));{
void*_tmp286=(void*)effs->hd;void*_tmp287=_tmp286;switch(*((int*)_tmp287)){case 24: _LL23E: _LL23F:
 goto _LL241;case 23: switch(*((int*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp287)->f1)){case 20: _LL240: _LL241:
 redo_join=1;goto _LL23D;case 22: _LL242: _LL243:
 redo_join=1;goto _LL23D;case 21: _LL244: _LL245:
 redo_join=1;goto _LL23D;default: goto _LL246;}default: _LL246: _LL247:
 goto _LL23D;}_LL23D:;};}}
# 1089
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*_tmp284;for(0;effs != 0;effs=effs->tl){
void*_tmp288=Cyc_Tcutil_compress((void*)effs->hd);void*_tmp289=_tmp288;void*_tmp28A;struct Cyc_List_List*_tmp28B;switch(*((int*)_tmp289)){case 24: _LL249: _tmp28B=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp289)->f1;_LL24A:
# 1094
 effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp28B,effects);
goto _LL248;case 23: switch(*((int*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp289)->f1)){case 20: _LL24B: _LL24C:
 goto _LL248;case 22: _LL24D: _LL24E:
 goto _LL248;case 21: _LL24F: _LL250:
 goto _LL248;default: goto _LL251;}default: _LL251: _tmp28A=_tmp289;_LL252:
{struct Cyc_List_List*_tmpCDC;effects=((_tmpCDC=_cycalloc(sizeof(*_tmpCDC)),((_tmpCDC->hd=_tmp28A,((_tmpCDC->tl=effects,_tmpCDC))))));}goto _LL248;}_LL248:;}}
# 1102
*_tmp284=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
return e;};}case 25: _LL239: _tmp283=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp282)->f1;_LL23A: {
# 1105
void*_tmp28D=Cyc_Tcutil_compress(_tmp283);void*_tmp28E=_tmp28D;switch(*((int*)_tmp28E)){case 1: _LL254: _LL255:
 goto _LL257;case 2: _LL256: _LL257:
 return e;default: _LL258: _LL259:
 return Cyc_Tcutil_rgns_of(_tmp283);}_LL253:;}default: _LL23B: _LL23C:
# 1110
 return e;}_LL236:;};}
# 1115
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpCE2;struct Cyc_Absyn_FnInfo _tmpCE1;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpCE0;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp28F=(_tmpCE0=_cycalloc(sizeof(*_tmpCE0)),((_tmpCE0[0]=((_tmpCE2.tag=9,((_tmpCE2.f1=((_tmpCE1.tvars=0,((_tmpCE1.effect=eff,((_tmpCE1.ret_tqual=
Cyc_Absyn_empty_tqual(0),((_tmpCE1.ret_typ=(void*)& Cyc_Absyn_VoidType_val,((_tmpCE1.args=0,((_tmpCE1.c_varargs=0,((_tmpCE1.cyc_varargs=0,((_tmpCE1.rgn_po=0,((_tmpCE1.attributes=0,((_tmpCE1.requires_clause=0,((_tmpCE1.requires_relns=0,((_tmpCE1.ensures_clause=0,((_tmpCE1.ensures_relns=0,_tmpCE1)))))))))))))))))))))))))),_tmpCE2)))),_tmpCE0)));
# 1125
return Cyc_Absyn_atb_typ((void*)_tmp28F,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}
# 1132
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);
if((r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (void*)& Cyc_Absyn_UniqueRgn_val) || r == (void*)& Cyc_Absyn_RefCntRgn_val)
return 1;{
void*_tmp293=Cyc_Tcutil_compress(e);void*_tmp294=_tmp293;struct Cyc_Core_Opt*_tmp295;void**_tmp296;struct Cyc_Core_Opt*_tmp297;void*_tmp298;struct Cyc_List_List*_tmp299;void*_tmp29A;switch(*((int*)_tmp294)){case 23: _LL25B: _tmp29A=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp294)->f1;_LL25C:
# 1139
 if(constrain)return Cyc_Tcutil_unify(r,_tmp29A);
_tmp29A=Cyc_Tcutil_compress(_tmp29A);
if(r == _tmp29A)return 1;{
struct _tuple0 _tmpCE3;struct _tuple0 _tmp29B=(_tmpCE3.f1=r,((_tmpCE3.f2=_tmp29A,_tmpCE3)));struct _tuple0 _tmp29C=_tmp29B;struct Cyc_Absyn_Tvar*_tmp29D;struct Cyc_Absyn_Tvar*_tmp29E;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp29C.f1)->tag == 2){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp29C.f2)->tag == 2){_LL266: _tmp29D=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp29C.f1)->f1;_tmp29E=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp29C.f2)->f1;_LL267:
 return Cyc_Absyn_tvar_cmp(_tmp29D,_tmp29E)== 0;}else{goto _LL268;}}else{_LL268: _LL269:
 return 0;}_LL265:;};case 24: _LL25D: _tmp299=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp294)->f1;_LL25E:
# 1147
 for(0;_tmp299 != 0;_tmp299=_tmp299->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp299->hd))return 1;}
return 0;case 25: _LL25F: _tmp298=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp294)->f1;_LL260: {
# 1151
void*_tmp2A0=Cyc_Tcutil_rgns_of(_tmp298);void*_tmp2A1=_tmp2A0;void*_tmp2A2;void*_tmp2A3;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2A1)->tag == 25){_LL26B: _tmp2A3=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2A1)->f1;_LL26C:
# 1153
 if(!constrain)return 0;{
void*_tmp2A4=Cyc_Tcutil_compress(_tmp2A3);void*_tmp2A5=_tmp2A4;struct Cyc_Core_Opt*_tmp2A6;void**_tmp2A7;struct Cyc_Core_Opt*_tmp2A8;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A5)->tag == 1){_LL270: _tmp2A6=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A5)->f1;_tmp2A7=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A5)->f2;_tmp2A8=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A5)->f4;_LL271: {
# 1158
void*_tmp2A9=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2A8);
# 1161
Cyc_Tcutil_occurs(_tmp2A9,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2A8))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCF2;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpCF1;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpCF0;void*_tmpCEF[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCEE;void*_tmp2AA=Cyc_Tcutil_dummy_fntype((void*)((_tmpCEE=_cycalloc(sizeof(*_tmpCEE)),((_tmpCEE[0]=((_tmpCF2.tag=24,((_tmpCF2.f1=((_tmpCEF[1]=(void*)((_tmpCF1=_cycalloc(sizeof(*_tmpCF1)),((_tmpCF1[0]=((_tmpCF0.tag=23,((_tmpCF0.f1=r,_tmpCF0)))),_tmpCF1)))),((_tmpCEF[0]=_tmp2A9,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCEF,sizeof(void*),2)))))),_tmpCF2)))),_tmpCEE)))));
*_tmp2A7=_tmp2AA;
return 1;};}}else{_LL272: _LL273:
 return 0;}_LL26F:;};}else{_LL26D: _tmp2A2=_tmp2A1;_LL26E:
# 1167
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp2A2);}_LL26A:;}case 1: _LL261: _tmp295=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp294)->f1;_tmp296=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp294)->f2;_tmp297=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp294)->f4;_LL262:
# 1170
 if(_tmp295 == 0  || ((struct Cyc_Absyn_Kind*)_tmp295->v)->kind != Cyc_Absyn_EffKind){
const char*_tmpCF5;void*_tmpCF4;(_tmpCF4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCF5="effect evar has wrong kind",_tag_dyneither(_tmpCF5,sizeof(char),27))),_tag_dyneither(_tmpCF4,sizeof(void*),0)));}
if(!constrain)return 0;{
# 1175
void*_tmp2B2=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp297);
# 1178
Cyc_Tcutil_occurs(_tmp2B2,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp297))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD0A;struct Cyc_List_List*_tmpD09;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpD08;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpD07;struct Cyc_List_List*_tmpD06;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD05;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2B3=(_tmpD05=_cycalloc(sizeof(*_tmpD05)),((_tmpD05[0]=((_tmpD0A.tag=24,((_tmpD0A.f1=((_tmpD06=_cycalloc(sizeof(*_tmpD06)),((_tmpD06->hd=_tmp2B2,((_tmpD06->tl=((_tmpD09=_cycalloc(sizeof(*_tmpD09)),((_tmpD09->hd=(void*)((_tmpD07=_cycalloc(sizeof(*_tmpD07)),((_tmpD07[0]=((_tmpD08.tag=23,((_tmpD08.f1=r,_tmpD08)))),_tmpD07)))),((_tmpD09->tl=0,_tmpD09)))))),_tmpD06)))))),_tmpD0A)))),_tmpD05)));
*_tmp296=(void*)_tmp2B3;
return 1;};};default: _LL263: _LL264:
 return 0;}_LL25A:;};}
# 1189
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Tcutil_compress(t);{
void*_tmp2BA=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_tmp2BB=_tmp2BA;struct Cyc_Core_Opt*_tmp2BC;void**_tmp2BD;struct Cyc_Core_Opt*_tmp2BE;void*_tmp2BF;struct Cyc_List_List*_tmp2C0;switch(*((int*)_tmp2BB)){case 23: _LL275: _LL276:
 return 0;case 24: _LL277: _tmp2C0=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2BB)->f1;_LL278:
# 1194
 for(0;_tmp2C0 != 0;_tmp2C0=_tmp2C0->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp2C0->hd))
return 1;}
return 0;case 25: _LL279: _tmp2BF=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2BB)->f1;_LL27A:
# 1199
 _tmp2BF=Cyc_Tcutil_compress(_tmp2BF);
if(t == _tmp2BF)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp2BF);{
void*_tmp2C1=Cyc_Tcutil_rgns_of(t);void*_tmp2C2=_tmp2C1;void*_tmp2C3;void*_tmp2C4;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2C2)->tag == 25){_LL280: _tmp2C4=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2C2)->f1;_LL281: {
# 1204
struct _tuple0 _tmpD0B;struct _tuple0 _tmp2C5=(_tmpD0B.f1=Cyc_Tcutil_compress(_tmp2C4),((_tmpD0B.f2=_tmp2BF,_tmpD0B)));struct _tuple0 _tmp2C6=_tmp2C5;struct Cyc_Absyn_Tvar*_tmp2C7;struct Cyc_Absyn_Tvar*_tmp2C8;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2C6.f1)->tag == 2){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2C6.f2)->tag == 2){_LL285: _tmp2C7=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2C6.f1)->f1;_tmp2C8=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2C6.f2)->f1;_LL286:
 return Cyc_Tcutil_unify(t,_tmp2BF);}else{goto _LL287;}}else{_LL287: _LL288:
 return _tmp2C4 == _tmp2BF;}_LL284:;}}else{_LL282: _tmp2C3=_tmp2C2;_LL283:
# 1208
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp2C3);}_LL27F:;};case 1: _LL27B: _tmp2BC=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2BB)->f1;_tmp2BD=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2BB)->f2;_tmp2BE=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2BB)->f4;_LL27C:
# 1211
 if(_tmp2BC == 0  || ((struct Cyc_Absyn_Kind*)_tmp2BC->v)->kind != Cyc_Absyn_EffKind){
const char*_tmpD0E;void*_tmpD0D;(_tmpD0D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD0E="effect evar has wrong kind",_tag_dyneither(_tmpD0E,sizeof(char),27))),_tag_dyneither(_tmpD0D,sizeof(void*),0)));}
if(!may_constrain_evars)return 0;{
# 1216
void*_tmp2CC=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2BE);
# 1219
Cyc_Tcutil_occurs(_tmp2CC,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2BE))->v,t);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD23;struct Cyc_List_List*_tmpD22;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpD21;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpD20;struct Cyc_List_List*_tmpD1F;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD1E;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2CD=(_tmpD1E=_cycalloc(sizeof(*_tmpD1E)),((_tmpD1E[0]=((_tmpD23.tag=24,((_tmpD23.f1=((_tmpD1F=_cycalloc(sizeof(*_tmpD1F)),((_tmpD1F->hd=_tmp2CC,((_tmpD1F->tl=((_tmpD22=_cycalloc(sizeof(*_tmpD22)),((_tmpD22->hd=(void*)((_tmpD20=_cycalloc(sizeof(*_tmpD20)),((_tmpD20[0]=((_tmpD21.tag=25,((_tmpD21.f1=t,_tmpD21)))),_tmpD20)))),((_tmpD22->tl=0,_tmpD22)))))),_tmpD1F)))))),_tmpD23)))),_tmpD1E)));
*_tmp2BD=(void*)_tmp2CD;
return 1;};};default: _LL27D: _LL27E:
 return 0;}_LL274:;};}
# 1230
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp2D4=e;struct Cyc_Core_Opt*_tmp2D5;void**_tmp2D6;struct Cyc_Core_Opt*_tmp2D7;void*_tmp2D8;struct Cyc_List_List*_tmp2D9;struct Cyc_Absyn_Tvar*_tmp2DA;switch(*((int*)_tmp2D4)){case 2: _LL28A: _tmp2DA=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2D4)->f1;_LL28B:
 return Cyc_Absyn_tvar_cmp(v,_tmp2DA)== 0;case 24: _LL28C: _tmp2D9=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2D4)->f1;_LL28D:
# 1235
 for(0;_tmp2D9 != 0;_tmp2D9=_tmp2D9->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)_tmp2D9->hd))
return 1;}
return 0;case 25: _LL28E: _tmp2D8=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2D4)->f1;_LL28F: {
# 1240
void*_tmp2DB=Cyc_Tcutil_rgns_of(_tmp2D8);void*_tmp2DC=_tmp2DB;void*_tmp2DD;void*_tmp2DE;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2DC)->tag == 25){_LL295: _tmp2DE=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2DC)->f1;_LL296:
# 1242
 if(!may_constrain_evars)return 0;{
void*_tmp2DF=Cyc_Tcutil_compress(_tmp2DE);void*_tmp2E0=_tmp2DF;struct Cyc_Core_Opt*_tmp2E1;void**_tmp2E2;struct Cyc_Core_Opt*_tmp2E3;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2E0)->tag == 1){_LL29A: _tmp2E1=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2E0)->f1;_tmp2E2=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2E0)->f2;_tmp2E3=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2E0)->f4;_LL29B: {
# 1248
void*_tmp2E4=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2E3);
# 1250
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2E3))->v,v))return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD32;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpD31;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpD30;void*_tmpD2F[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD2E;void*_tmp2E5=Cyc_Tcutil_dummy_fntype((void*)((_tmpD2E=_cycalloc(sizeof(*_tmpD2E)),((_tmpD2E[0]=((_tmpD32.tag=24,((_tmpD32.f1=((_tmpD2F[1]=(void*)((_tmpD31=_cycalloc(sizeof(*_tmpD31)),((_tmpD31[0]=((_tmpD30.tag=2,((_tmpD30.f1=v,_tmpD30)))),_tmpD31)))),((_tmpD2F[0]=_tmp2E4,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD2F,sizeof(void*),2)))))),_tmpD32)))),_tmpD2E)))));
*_tmp2E2=_tmp2E5;
return 1;};}}else{_LL29C: _LL29D:
 return 0;}_LL299:;};}else{_LL297: _tmp2DD=_tmp2DC;_LL298:
# 1256
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp2DD);}_LL294:;}case 1: _LL290: _tmp2D5=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2D4)->f1;_tmp2D6=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2D4)->f2;_tmp2D7=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2D4)->f4;_LL291:
# 1259
 if(_tmp2D5 == 0  || ((struct Cyc_Absyn_Kind*)_tmp2D5->v)->kind != Cyc_Absyn_EffKind){
const char*_tmpD35;void*_tmpD34;(_tmpD34=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD35="effect evar has wrong kind",_tag_dyneither(_tmpD35,sizeof(char),27))),_tag_dyneither(_tmpD34,sizeof(void*),0)));}{
# 1263
void*_tmp2ED=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2D7);
# 1265
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2D7))->v,v))
return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD4A;struct Cyc_List_List*_tmpD49;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpD48;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpD47;struct Cyc_List_List*_tmpD46;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD45;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2EE=(_tmpD45=_cycalloc(sizeof(*_tmpD45)),((_tmpD45[0]=((_tmpD4A.tag=24,((_tmpD4A.f1=((_tmpD46=_cycalloc(sizeof(*_tmpD46)),((_tmpD46->hd=_tmp2ED,((_tmpD46->tl=((_tmpD49=_cycalloc(sizeof(*_tmpD49)),((_tmpD49->hd=(void*)((_tmpD47=_cycalloc(sizeof(*_tmpD47)),((_tmpD47[0]=((_tmpD48.tag=2,((_tmpD48.f1=v,_tmpD48)))),_tmpD47)))),((_tmpD49->tl=0,_tmpD49)))))),_tmpD46)))))),_tmpD4A)))),_tmpD45)));
*_tmp2D6=(void*)_tmp2EE;
return 1;};};default: _LL292: _LL293:
 return 0;}_LL289:;};}
# 1275
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp2F5=e;void*_tmp2F6;struct Cyc_List_List*_tmp2F7;switch(*((int*)_tmp2F5)){case 24: _LL29F: _tmp2F7=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2F5)->f1;_LL2A0:
# 1279
 for(0;_tmp2F7 != 0;_tmp2F7=_tmp2F7->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp2F7->hd))
return 1;}
return 0;case 25: _LL2A1: _tmp2F6=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2F5)->f1;_LL2A2: {
# 1284
void*_tmp2F8=Cyc_Tcutil_rgns_of(_tmp2F6);void*_tmp2F9=_tmp2F8;void*_tmp2FA;void*_tmp2FB;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2F9)->tag == 25){_LL2A8: _tmp2FB=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2F9)->f1;_LL2A9:
 return 0;}else{_LL2AA: _tmp2FA=_tmp2F9;_LL2AB:
 return Cyc_Tcutil_evar_in_effect(evar,_tmp2FA);}_LL2A7:;}case 1: _LL2A3: _LL2A4:
# 1288
 return evar == e;default: _LL2A5: _LL2A6:
 return 0;}_LL29E:;};}
# 1302 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1307
void*_tmp2FC=Cyc_Tcutil_compress(e1);void*_tmp2FD=_tmp2FC;void**_tmp2FE;struct Cyc_Core_Opt*_tmp2FF;void*_tmp300;struct Cyc_Absyn_Tvar*_tmp301;void*_tmp302;struct Cyc_List_List*_tmp303;switch(*((int*)_tmp2FD)){case 24: _LL2AD: _tmp303=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2FD)->f1;_LL2AE:
# 1309
 for(0;_tmp303 != 0;_tmp303=_tmp303->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp303->hd,e2))
return 0;}
return 1;case 23: _LL2AF: _tmp302=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp2FD)->f1;_LL2B0:
# 1320
 return Cyc_Tcutil_region_in_effect(may_constrain_evars,_tmp302,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp302,(void*)& Cyc_Absyn_HeapRgn_val);case 2: _LL2B1: _tmp301=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2FD)->f1;_LL2B2:
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp301,e2);case 25: _LL2B3: _tmp300=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2FD)->f1;_LL2B4: {
# 1324
void*_tmp304=Cyc_Tcutil_rgns_of(_tmp300);void*_tmp305=_tmp304;void*_tmp306;void*_tmp307;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp305)->tag == 25){_LL2BA: _tmp307=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp305)->f1;_LL2BB:
# 1329
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp307,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp307,Cyc_Absyn_sint_typ);}else{_LL2BC: _tmp306=_tmp305;_LL2BD:
 return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp306,e2);}_LL2B9:;}case 1: _LL2B5: _tmp2FE=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2FD)->f2;_tmp2FF=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2FD)->f4;_LL2B6:
# 1334
 if(!Cyc_Tcutil_evar_in_effect(e1,e2)){
# 1338
*_tmp2FE=Cyc_Absyn_empty_effect;
# 1341
return 1;}else{
# 1343
return 0;}default: _LL2B7: _LL2B8: {
const char*_tmpD4E;void*_tmpD4D[1];struct Cyc_String_pa_PrintArg_struct _tmpD4C;(_tmpD4C.tag=0,((_tmpD4C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1)),((_tmpD4D[0]=& _tmpD4C,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD4E="subset_effect: bad effect: %s",_tag_dyneither(_tmpD4E,sizeof(char),30))),_tag_dyneither(_tmpD4D,sizeof(void*),1)))))));}}_LL2AC:;}
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
struct _tuple0*_tmp30B=(struct _tuple0*)r1->hd;void*_tmp30D;void*_tmp30E;struct _tuple0*_tmp30C=_tmp30B;_tmp30D=_tmp30C->f1;_tmp30E=_tmp30C->f2;{
int found=_tmp30D == (void*)& Cyc_Absyn_HeapRgn_val;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
struct _tuple0*_tmp30F=(struct _tuple0*)r2->hd;void*_tmp311;void*_tmp312;struct _tuple0*_tmp310=_tmp30F;_tmp311=_tmp310->f1;_tmp312=_tmp310->f2;
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
struct Cyc_List_List*_tmpD4F;struct Cyc_List_List*_tmp314=(_tmpD4F=_cycalloc(sizeof(*_tmpD4F)),((_tmpD4F->hd=_tmp313,((_tmpD4F->tl=r1,_tmpD4F)))));
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
{void*_tmp316=t1;struct Cyc_Core_Opt*_tmp317;void**_tmp318;struct Cyc_Core_Opt*_tmp319;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp316)->tag == 1){_LL2BF: _tmp317=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp316)->f1;_tmp318=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp316)->f2;_tmp319=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp316)->f4;_LL2C0:
# 1430
 Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp319))->v,t2);{
struct Cyc_Absyn_Kind*_tmp31A=Cyc_Tcutil_typ_kind(t2);
# 1435
if(Cyc_Tcutil_kind_leq(_tmp31A,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp317))->v)){
*_tmp318=t2;
return;}else{
# 1439
{void*_tmp31B=t2;struct Cyc_Absyn_PtrInfo _tmp31C;void**_tmp31D;struct Cyc_Core_Opt*_tmp31E;switch(*((int*)_tmp31B)){case 1: _LL2C4: _tmp31D=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp31B)->f2;_tmp31E=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp31B)->f4;_LL2C5: {
# 1442
struct Cyc_List_List*_tmp31F=(struct Cyc_List_List*)_tmp319->v;
{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp31E))->v;for(0;s2 != 0;s2=s2->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp31F,(struct Cyc_Absyn_Tvar*)s2->hd)){
{const char*_tmpD50;Cyc_Tcutil_failure_reason=((_tmpD50="(type variable would escape scope)",_tag_dyneither(_tmpD50,sizeof(char),35)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1449
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp317->v,_tmp31A)){
*_tmp31D=t1;return;}
# 1452
{const char*_tmpD51;Cyc_Tcutil_failure_reason=((_tmpD51="(kinds are incompatible)",_tag_dyneither(_tmpD51,sizeof(char),25)));}
goto _LL2C3;}case 5: _LL2C6: _tmp31C=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp31B)->f1;if(((struct Cyc_Absyn_Kind*)_tmp317->v)->kind == Cyc_Absyn_BoxKind){_LL2C7: {
# 1458
union Cyc_Absyn_Constraint*_tmp322=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((_tmp31C.ptr_atts).bounds);
{union Cyc_Absyn_Constraint*_tmp323=_tmp322;if((((union Cyc_Absyn_Constraint*)_tmp323)->No_constr).tag == 3){_LL2CB: _LL2CC:
# 1462
{struct _union_Constraint_Eq_constr*_tmpD52;(_tmpD52=& _tmp322->Eq_constr,((_tmpD52->tag=1,_tmpD52->val=Cyc_Absyn_bounds_one)));}
*_tmp318=t2;
return;}else{_LL2CD: _LL2CE:
 goto _LL2CA;}_LL2CA:;}
# 1467
goto _LL2C3;}}else{goto _LL2C8;}default: _LL2C8: _LL2C9:
 goto _LL2C3;}_LL2C3:;}
# 1470
{const char*_tmpD53;Cyc_Tcutil_failure_reason=((_tmpD53="(kinds are incompatible)",_tag_dyneither(_tmpD53,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};}else{_LL2C1: _LL2C2:
# 1473
 goto _LL2BE;}_LL2BE:;}
# 1478
{struct _tuple0 _tmpD54;struct _tuple0 _tmp326=(_tmpD54.f1=t2,((_tmpD54.f2=t1,_tmpD54)));struct _tuple0 _tmp327=_tmp326;void*_tmp328;void*_tmp329;void*_tmp32A;void*_tmp32B;void*_tmp32C;void*_tmp32D;struct Cyc_List_List*_tmp32E;struct Cyc_Absyn_Typedefdecl*_tmp32F;struct Cyc_List_List*_tmp330;struct Cyc_Absyn_Typedefdecl*_tmp331;enum Cyc_Absyn_AggrKind _tmp332;struct Cyc_List_List*_tmp333;enum Cyc_Absyn_AggrKind _tmp334;struct Cyc_List_List*_tmp335;struct Cyc_List_List*_tmp336;struct Cyc_List_List*_tmp337;struct Cyc_List_List*_tmp338;void*_tmp339;struct Cyc_Absyn_Tqual _tmp33A;void*_tmp33B;struct Cyc_List_List*_tmp33C;int _tmp33D;struct Cyc_Absyn_VarargInfo*_tmp33E;struct Cyc_List_List*_tmp33F;struct Cyc_List_List*_tmp340;struct Cyc_Absyn_Exp*_tmp341;struct Cyc_List_List*_tmp342;struct Cyc_Absyn_Exp*_tmp343;struct Cyc_List_List*_tmp344;struct Cyc_List_List*_tmp345;void*_tmp346;struct Cyc_Absyn_Tqual _tmp347;void*_tmp348;struct Cyc_List_List*_tmp349;int _tmp34A;struct Cyc_Absyn_VarargInfo*_tmp34B;struct Cyc_List_List*_tmp34C;struct Cyc_List_List*_tmp34D;struct Cyc_Absyn_Exp*_tmp34E;struct Cyc_List_List*_tmp34F;struct Cyc_Absyn_Exp*_tmp350;struct Cyc_List_List*_tmp351;void*_tmp352;struct Cyc_Absyn_Tqual _tmp353;struct Cyc_Absyn_Exp*_tmp354;union Cyc_Absyn_Constraint*_tmp355;void*_tmp356;struct Cyc_Absyn_Tqual _tmp357;struct Cyc_Absyn_Exp*_tmp358;union Cyc_Absyn_Constraint*_tmp359;struct Cyc_Absyn_Exp*_tmp35A;struct Cyc_Absyn_Exp*_tmp35B;void*_tmp35C;void*_tmp35D;int _tmp35E;int _tmp35F;enum Cyc_Absyn_Sign _tmp360;enum Cyc_Absyn_Size_of _tmp361;enum Cyc_Absyn_Sign _tmp362;enum Cyc_Absyn_Size_of _tmp363;void*_tmp364;struct Cyc_Absyn_Tqual _tmp365;void*_tmp366;union Cyc_Absyn_Constraint*_tmp367;union Cyc_Absyn_Constraint*_tmp368;union Cyc_Absyn_Constraint*_tmp369;void*_tmp36A;struct Cyc_Absyn_Tqual _tmp36B;void*_tmp36C;union Cyc_Absyn_Constraint*_tmp36D;union Cyc_Absyn_Constraint*_tmp36E;union Cyc_Absyn_Constraint*_tmp36F;struct Cyc_Absyn_Datatypedecl*_tmp370;struct Cyc_Absyn_Datatypefield*_tmp371;struct Cyc_List_List*_tmp372;struct Cyc_Absyn_Datatypedecl*_tmp373;struct Cyc_Absyn_Datatypefield*_tmp374;struct Cyc_List_List*_tmp375;struct Cyc_Absyn_Datatypedecl*_tmp376;struct Cyc_List_List*_tmp377;struct Cyc_Absyn_Datatypedecl*_tmp378;struct Cyc_List_List*_tmp379;struct Cyc_List_List*_tmp37A;struct Cyc_List_List*_tmp37B;struct _tuple2*_tmp37C;struct _tuple2*_tmp37D;union Cyc_Absyn_AggrInfoU _tmp37E;struct Cyc_List_List*_tmp37F;union Cyc_Absyn_AggrInfoU _tmp380;struct Cyc_List_List*_tmp381;struct Cyc_Absyn_Tvar*_tmp382;struct Cyc_Absyn_Tvar*_tmp383;switch(*((int*)_tmp327.f1)){case 1: _LL2D0: _LL2D1:
# 1481
 Cyc_Tcutil_unify_it(t2,t1);
return;case 0: switch(*((int*)_tmp327.f2)){case 0: _LL2D2: _LL2D3:
# 1484
 return;case 24: goto _LL300;case 25: goto _LL306;case 23: goto _LL308;default: goto _LL30A;}case 2: switch(*((int*)_tmp327.f2)){case 2: _LL2D4: _tmp382=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp327.f1)->f1;_tmp383=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp327.f2)->f1;_LL2D5: {
# 1487
struct _dyneither_ptr*_tmp384=_tmp382->name;
struct _dyneither_ptr*_tmp385=_tmp383->name;
# 1490
int _tmp386=_tmp382->identity;
int _tmp387=_tmp383->identity;
if(_tmp387 == _tmp386)return;
{const char*_tmpD55;Cyc_Tcutil_failure_reason=((_tmpD55="(variable types are not the same)",_tag_dyneither(_tmpD55,sizeof(char),34)));}
goto _LL2CF;}case 24: goto _LL300;case 25: goto _LL306;case 23: goto _LL308;default: goto _LL30A;}case 11: switch(*((int*)_tmp327.f2)){case 11: _LL2D6: _tmp37E=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp327.f1)->f1).aggr_info;_tmp37F=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp327.f1)->f1).targs;_tmp380=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp327.f2)->f1).aggr_info;_tmp381=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp327.f2)->f1).targs;_LL2D7: {
# 1497
struct _tuple13 _tmp389=Cyc_Absyn_aggr_kinded_name(_tmp380);enum Cyc_Absyn_AggrKind _tmp38B;struct _tuple2*_tmp38C;struct _tuple13 _tmp38A=_tmp389;_tmp38B=_tmp38A.f1;_tmp38C=_tmp38A.f2;{
struct _tuple13 _tmp38D=Cyc_Absyn_aggr_kinded_name(_tmp37E);enum Cyc_Absyn_AggrKind _tmp38F;struct _tuple2*_tmp390;struct _tuple13 _tmp38E=_tmp38D;_tmp38F=_tmp38E.f1;_tmp390=_tmp38E.f2;
if(_tmp38B != _tmp38F){{const char*_tmpD56;Cyc_Tcutil_failure_reason=((_tmpD56="(struct and union type)",_tag_dyneither(_tmpD56,sizeof(char),24)));}goto _LL2CF;}
if(Cyc_Absyn_qvar_cmp(_tmp38C,_tmp390)!= 0){{const char*_tmpD57;Cyc_Tcutil_failure_reason=((_tmpD57="(different type name)",_tag_dyneither(_tmpD57,sizeof(char),22)));}goto _LL2CF;}
Cyc_Tcutil_unify_list(_tmp381,_tmp37F);
return;};}case 24: goto _LL300;case 25: goto _LL306;case 23: goto _LL308;default: goto _LL30A;}case 13: switch(*((int*)_tmp327.f2)){case 13: _LL2D8: _tmp37C=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp327.f1)->f1;_tmp37D=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp327.f2)->f1;_LL2D9:
# 1506
 if(Cyc_Absyn_qvar_cmp(_tmp37C,_tmp37D)== 0)return;
{const char*_tmpD58;Cyc_Tcutil_failure_reason=((_tmpD58="(different enum types)",_tag_dyneither(_tmpD58,sizeof(char),23)));}
goto _LL2CF;case 24: goto _LL300;case 25: goto _LL306;case 23: goto _LL308;default: goto _LL30A;}case 14: switch(*((int*)_tmp327.f2)){case 14: _LL2DA: _tmp37A=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp327.f1)->f1;_tmp37B=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp327.f2)->f1;_LL2DB: {
# 1511
int bad=0;
for(0;_tmp37A != 0  && _tmp37B != 0;(_tmp37A=_tmp37A->tl,_tmp37B=_tmp37B->tl)){
struct Cyc_Absyn_Enumfield*_tmp394=(struct Cyc_Absyn_Enumfield*)_tmp37A->hd;
struct Cyc_Absyn_Enumfield*_tmp395=(struct Cyc_Absyn_Enumfield*)_tmp37B->hd;
if(Cyc_Absyn_qvar_cmp(_tmp394->name,_tmp395->name)!= 0){
{const char*_tmpD59;Cyc_Tcutil_failure_reason=((_tmpD59="(different names for enum fields)",_tag_dyneither(_tmpD59,sizeof(char),34)));}
bad=1;
break;}
# 1520
if(_tmp394->tag == _tmp395->tag)continue;
if(_tmp394->tag == 0  || _tmp395->tag == 0){
{const char*_tmpD5A;Cyc_Tcutil_failure_reason=((_tmpD5A="(different tag values for enum fields)",_tag_dyneither(_tmpD5A,sizeof(char),39)));}
bad=1;
break;}
# 1526
if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp394->tag),(struct Cyc_Absyn_Exp*)_check_null(_tmp395->tag))){
{const char*_tmpD5B;Cyc_Tcutil_failure_reason=((_tmpD5B="(different tag values for enum fields)",_tag_dyneither(_tmpD5B,sizeof(char),39)));}
bad=1;
break;}}
# 1532
if(bad)goto _LL2CF;
if(_tmp37A == 0  && _tmp37B == 0)return;
{const char*_tmpD5C;Cyc_Tcutil_failure_reason=((_tmpD5C="(different number of fields for enums)",_tag_dyneither(_tmpD5C,sizeof(char),39)));}
goto _LL2CF;}case 24: goto _LL300;case 25: goto _LL306;case 23: goto _LL308;default: goto _LL30A;}case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp327.f1)->f1).datatype_info).KnownDatatype).tag == 2)switch(*((int*)_tmp327.f2)){case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp327.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL2DC: _tmp376=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp327.f1)->f1).datatype_info).KnownDatatype).val;_tmp377=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp327.f1)->f1).targs;_tmp378=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp327.f2)->f1).datatype_info).KnownDatatype).val;_tmp379=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp327.f2)->f1).targs;_LL2DD:
# 1539
 if(_tmp376 == _tmp378  || Cyc_Absyn_qvar_cmp(_tmp376->name,_tmp378->name)== 0){
Cyc_Tcutil_unify_list(_tmp379,_tmp377);
return;}
# 1543
goto _LL2CF;}else{goto _LL30A;}case 24: goto _LL300;case 25: goto _LL306;case 23: goto _LL308;default: goto _LL30A;}else{switch(*((int*)_tmp327.f2)){case 24: goto _LL300;case 25: goto _LL306;case 23: goto _LL308;default: goto _LL30A;}}case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp327.f1)->f1).field_info).KnownDatatypefield).tag == 2)switch(*((int*)_tmp327.f2)){case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp327.f2)->f1).field_info).KnownDatatypefield).tag == 2){_LL2DE: _tmp370=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp327.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp371=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp327.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp372=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp327.f1)->f1).targs;_tmp373=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp327.f2)->f1).field_info).KnownDatatypefield).val).f1;_tmp374=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp327.f2)->f1).field_info).KnownDatatypefield).val).f2;_tmp375=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp327.f2)->f1).targs;_LL2DF:
# 1547
 if((_tmp370 == _tmp373  || Cyc_Absyn_qvar_cmp(_tmp370->name,_tmp373->name)== 0) && (
_tmp371 == _tmp374  || Cyc_Absyn_qvar_cmp(_tmp371->name,_tmp374->name)== 0)){
Cyc_Tcutil_unify_list(_tmp375,_tmp372);
return;}
# 1552
{const char*_tmpD5D;Cyc_Tcutil_failure_reason=((_tmpD5D="(different datatype field types)",_tag_dyneither(_tmpD5D,sizeof(char),33)));}
goto _LL2CF;}else{goto _LL30A;}case 24: goto _LL300;case 25: goto _LL306;case 23: goto _LL308;default: goto _LL30A;}else{switch(*((int*)_tmp327.f2)){case 24: goto _LL300;case 25: goto _LL306;case 23: goto _LL308;default: goto _LL30A;}}case 5: switch(*((int*)_tmp327.f2)){case 5: _LL2E0: _tmp364=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp327.f1)->f1).elt_typ;_tmp365=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp327.f1)->f1).elt_tq;_tmp366=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp327.f1)->f1).ptr_atts).rgn;_tmp367=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp327.f1)->f1).ptr_atts).nullable;_tmp368=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp327.f1)->f1).ptr_atts).bounds;_tmp369=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp327.f1)->f1).ptr_atts).zero_term;_tmp36A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp327.f2)->f1).elt_typ;_tmp36B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp327.f2)->f1).elt_tq;_tmp36C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp327.f2)->f1).ptr_atts).rgn;_tmp36D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp327.f2)->f1).ptr_atts).nullable;_tmp36E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp327.f2)->f1).ptr_atts).bounds;_tmp36F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp327.f2)->f1).ptr_atts).zero_term;_LL2E1:
# 1557
 Cyc_Tcutil_unify_it(_tmp36A,_tmp364);
Cyc_Tcutil_unify_it(_tmp366,_tmp36C);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpD5E;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp36F,_tmp369,((_tmpD5E="(not both zero terminated)",_tag_dyneither(_tmpD5E,sizeof(char),27))));}
Cyc_Tcutil_unify_tqual(_tmp36B,_tmp36A,_tmp365,_tmp364);
{const char*_tmpD5F;((void(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp36E,_tmp368,((_tmpD5F="(different pointer bounds)",_tag_dyneither(_tmpD5F,sizeof(char),27))));}
# 1565
{void*_tmp39D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp36E);void*_tmp39E=_tmp39D;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp39E)->tag == 0){_LL30D: _LL30E:
 return;}else{_LL30F: _LL310:
 goto _LL30C;}_LL30C:;}
# 1569
{const char*_tmpD60;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp36D,_tmp367,((_tmpD60="(incompatible pointer types)",_tag_dyneither(_tmpD60,sizeof(char),29))));}
return;case 24: goto _LL300;case 25: goto _LL306;case 23: goto _LL308;default: goto _LL30A;}case 6: switch(*((int*)_tmp327.f2)){case 6: _LL2E2: _tmp360=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp327.f1)->f1;_tmp361=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp327.f1)->f2;_tmp362=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp327.f2)->f1;_tmp363=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp327.f2)->f2;_LL2E3:
# 1574
 if(_tmp362 == _tmp360  && ((_tmp363 == _tmp361  || _tmp363 == Cyc_Absyn_Int_sz  && _tmp361 == Cyc_Absyn_Long_sz) || 
_tmp363 == Cyc_Absyn_Long_sz  && _tmp361 == Cyc_Absyn_Int_sz))return;
{const char*_tmpD61;Cyc_Tcutil_failure_reason=((_tmpD61="(different integral types)",_tag_dyneither(_tmpD61,sizeof(char),27)));}
goto _LL2CF;case 24: goto _LL300;case 25: goto _LL306;case 23: goto _LL308;default: goto _LL30A;}case 7: switch(*((int*)_tmp327.f2)){case 7: _LL2E4: _tmp35E=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp327.f1)->f1;_tmp35F=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp327.f2)->f1;_LL2E5:
# 1580
 if(_tmp35F == 0  && _tmp35E == 0)return;else{
if(_tmp35F == 1  && _tmp35E == 1)return;else{
# 1583
if(((_tmp35F != 0  && _tmp35F != 1) && _tmp35E != 0) && _tmp35E != 1)return;}}
goto _LL2CF;case 24: goto _LL300;case 25: goto _LL306;case 23: goto _LL308;default: goto _LL30A;}case 19: switch(*((int*)_tmp327.f2)){case 19: _LL2E6: _tmp35C=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp327.f1)->f1;_tmp35D=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp327.f2)->f1;_LL2E7:
# 1586
 Cyc_Tcutil_unify_it(_tmp35C,_tmp35D);return;case 24: goto _LL300;case 25: goto _LL306;case 23: goto _LL308;default: goto _LL30A;}case 18: switch(*((int*)_tmp327.f2)){case 18: _LL2E8: _tmp35A=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp327.f1)->f1;_tmp35B=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp327.f2)->f1;_LL2E9:
# 1589
 if(!Cyc_Evexp_same_const_exp(_tmp35A,_tmp35B)){
{const char*_tmpD62;Cyc_Tcutil_failure_reason=((_tmpD62="(cannot prove expressions are the same)",_tag_dyneither(_tmpD62,sizeof(char),40)));}
goto _LL2CF;}
# 1593
return;case 24: goto _LL300;case 25: goto _LL306;case 23: goto _LL308;default: goto _LL30A;}case 8: switch(*((int*)_tmp327.f2)){case 8: _LL2EA: _tmp352=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp327.f1)->f1).elt_type;_tmp353=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp327.f1)->f1).tq;_tmp354=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp327.f1)->f1).num_elts;_tmp355=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp327.f1)->f1).zero_term;_tmp356=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp327.f2)->f1).elt_type;_tmp357=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp327.f2)->f1).tq;_tmp358=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp327.f2)->f1).num_elts;_tmp359=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp327.f2)->f1).zero_term;_LL2EB:
# 1597
 Cyc_Tcutil_unify_it(_tmp356,_tmp352);
Cyc_Tcutil_unify_tqual(_tmp357,_tmp356,_tmp353,_tmp352);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpD63;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp355,_tmp359,((_tmpD63="(not both zero terminated)",_tag_dyneither(_tmpD63,sizeof(char),27))));}
if(_tmp354 == _tmp358)return;
if(_tmp354 == 0  || _tmp358 == 0)goto _LL2CF;
if(Cyc_Evexp_same_const_exp(_tmp354,_tmp358))
return;
{const char*_tmpD64;Cyc_Tcutil_failure_reason=((_tmpD64="(different array sizes)",_tag_dyneither(_tmpD64,sizeof(char),24)));}
goto _LL2CF;case 24: goto _LL300;case 25: goto _LL306;case 23: goto _LL308;default: goto _LL30A;}case 9: switch(*((int*)_tmp327.f2)){case 9: _LL2EC: _tmp338=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f1)->f1).tvars;_tmp339=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f1)->f1).effect;_tmp33A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f1)->f1).ret_tqual;_tmp33B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f1)->f1).ret_typ;_tmp33C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f1)->f1).args;_tmp33D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f1)->f1).c_varargs;_tmp33E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f1)->f1).cyc_varargs;_tmp33F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f1)->f1).rgn_po;_tmp340=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f1)->f1).attributes;_tmp341=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f1)->f1).requires_clause;_tmp342=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f1)->f1).requires_relns;_tmp343=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f1)->f1).ensures_clause;_tmp344=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f1)->f1).ensures_relns;_tmp345=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f2)->f1).tvars;_tmp346=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f2)->f1).effect;_tmp347=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f2)->f1).ret_tqual;_tmp348=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f2)->f1).ret_typ;_tmp349=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f2)->f1).args;_tmp34A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f2)->f1).c_varargs;_tmp34B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f2)->f1).cyc_varargs;_tmp34C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f2)->f1).rgn_po;_tmp34D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f2)->f1).attributes;_tmp34E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f2)->f1).requires_clause;_tmp34F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f2)->f1).requires_relns;_tmp350=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f2)->f1).ensures_clause;_tmp351=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp327.f2)->f1).ensures_relns;_LL2ED: {
# 1611
int done=0;
struct _RegionHandle _tmp3A4=_new_region("rgn");struct _RegionHandle*rgn=& _tmp3A4;_push_region(rgn);{
struct Cyc_List_List*inst=0;
while(_tmp345 != 0){
if(_tmp338 == 0){
{const char*_tmpD65;Cyc_Tcutil_failure_reason=((_tmpD65="(second function type has too few type variables)",_tag_dyneither(_tmpD65,sizeof(char),50)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1619
void*_tmp3A6=((struct Cyc_Absyn_Tvar*)_tmp345->hd)->kind;
void*_tmp3A7=((struct Cyc_Absyn_Tvar*)_tmp338->hd)->kind;
if(!Cyc_Tcutil_unify_kindbound(_tmp3A6,_tmp3A7)){
{const char*_tmpD6B;void*_tmpD6A[3];struct Cyc_String_pa_PrintArg_struct _tmpD69;struct Cyc_String_pa_PrintArg_struct _tmpD68;struct Cyc_String_pa_PrintArg_struct _tmpD67;Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)((_tmpD67.tag=0,((_tmpD67.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1625
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp338->hd,& Cyc_Tcutil_bk))),((_tmpD68.tag=0,((_tmpD68.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1624
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp345->hd,& Cyc_Tcutil_bk))),((_tmpD69.tag=0,((_tmpD69.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1623
Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)_tmp345->hd)),((_tmpD6A[0]=& _tmpD69,((_tmpD6A[1]=& _tmpD68,((_tmpD6A[2]=& _tmpD67,Cyc_aprintf(((_tmpD6B="(type var %s has different kinds %s and %s)",_tag_dyneither(_tmpD6B,sizeof(char),44))),_tag_dyneither(_tmpD6A,sizeof(void*),3))))))))))))))))))));}
# 1626
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1628
{struct _tuple16*_tmpD75;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpD74;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpD73;struct Cyc_List_List*_tmpD72;inst=((_tmpD72=_region_malloc(rgn,sizeof(*_tmpD72)),((_tmpD72->hd=((_tmpD75=_region_malloc(rgn,sizeof(*_tmpD75)),((_tmpD75->f1=(struct Cyc_Absyn_Tvar*)_tmp338->hd,((_tmpD75->f2=(void*)((_tmpD73=_cycalloc(sizeof(*_tmpD73)),((_tmpD73[0]=((_tmpD74.tag=2,((_tmpD74.f1=(struct Cyc_Absyn_Tvar*)_tmp345->hd,_tmpD74)))),_tmpD73)))),_tmpD75)))))),((_tmpD72->tl=inst,_tmpD72))))));}
_tmp345=_tmp345->tl;
_tmp338=_tmp338->tl;};}
# 1632
if(_tmp338 != 0){
{const char*_tmpD76;Cyc_Tcutil_failure_reason=((_tmpD76="(second function type has too many type variables)",_tag_dyneither(_tmpD76,sizeof(char),51)));}
_npop_handler(0);goto _LL2CF;}
# 1636
if(inst != 0){
{struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpD82;struct Cyc_Absyn_FnInfo _tmpD81;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD80;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpD7C;struct Cyc_Absyn_FnInfo _tmpD7B;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD7A;Cyc_Tcutil_unify_it((void*)((_tmpD7A=_cycalloc(sizeof(*_tmpD7A)),((_tmpD7A[0]=((_tmpD7C.tag=9,((_tmpD7C.f1=((_tmpD7B.tvars=0,((_tmpD7B.effect=_tmp346,((_tmpD7B.ret_tqual=_tmp347,((_tmpD7B.ret_typ=_tmp348,((_tmpD7B.args=_tmp349,((_tmpD7B.c_varargs=_tmp34A,((_tmpD7B.cyc_varargs=_tmp34B,((_tmpD7B.rgn_po=_tmp34C,((_tmpD7B.attributes=_tmp34D,((_tmpD7B.requires_clause=_tmp341,((_tmpD7B.requires_relns=_tmp342,((_tmpD7B.ensures_clause=_tmp343,((_tmpD7B.ensures_relns=_tmp344,_tmpD7B)))))))))))))))))))))))))),_tmpD7C)))),_tmpD7A)))),
# 1640
Cyc_Tcutil_rsubstitute(rgn,inst,(void*)(
(_tmpD80=_cycalloc(sizeof(*_tmpD80)),((_tmpD80[0]=((_tmpD82.tag=9,((_tmpD82.f1=((_tmpD81.tvars=0,((_tmpD81.effect=_tmp339,((_tmpD81.ret_tqual=_tmp33A,((_tmpD81.ret_typ=_tmp33B,((_tmpD81.args=_tmp33C,((_tmpD81.c_varargs=_tmp33D,((_tmpD81.cyc_varargs=_tmp33E,((_tmpD81.rgn_po=_tmp33F,((_tmpD81.attributes=_tmp340,((_tmpD81.requires_clause=_tmp34E,((_tmpD81.requires_relns=_tmp34F,((_tmpD81.ensures_clause=_tmp350,((_tmpD81.ensures_relns=_tmp351,_tmpD81)))))))))))))))))))))))))),_tmpD82)))),_tmpD80))))));}
# 1645
done=1;}}
# 1648
if(done){
_npop_handler(0);return;}
Cyc_Tcutil_unify_it(_tmp348,_tmp33B);
Cyc_Tcutil_unify_tqual(_tmp347,_tmp348,_tmp33A,_tmp33B);
for(0;_tmp349 != 0  && _tmp33C != 0;(_tmp349=_tmp349->tl,_tmp33C=_tmp33C->tl)){
struct _tuple10 _tmp3B8=*((struct _tuple10*)_tmp349->hd);struct Cyc_Absyn_Tqual _tmp3BA;void*_tmp3BB;struct _tuple10 _tmp3B9=_tmp3B8;_tmp3BA=_tmp3B9.f2;_tmp3BB=_tmp3B9.f3;{
struct _tuple10 _tmp3BC=*((struct _tuple10*)_tmp33C->hd);struct Cyc_Absyn_Tqual _tmp3BE;void*_tmp3BF;struct _tuple10 _tmp3BD=_tmp3BC;_tmp3BE=_tmp3BD.f2;_tmp3BF=_tmp3BD.f3;
Cyc_Tcutil_unify_it(_tmp3BB,_tmp3BF);
Cyc_Tcutil_unify_tqual(_tmp3BA,_tmp3BB,_tmp3BE,_tmp3BF);};}
# 1658
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(_tmp349 != 0  || _tmp33C != 0){
{const char*_tmpD83;Cyc_Tcutil_failure_reason=((_tmpD83="(function types have different number of arguments)",_tag_dyneither(_tmpD83,sizeof(char),52)));}
_npop_handler(0);goto _LL2CF;}
# 1664
if(_tmp34A != _tmp33D){
{const char*_tmpD84;Cyc_Tcutil_failure_reason=((_tmpD84="(only one function type takes C varargs)",_tag_dyneither(_tmpD84,sizeof(char),41)));}
_npop_handler(0);goto _LL2CF;}{
# 1669
int bad_cyc_vararg=0;
{struct _tuple22 _tmpD85;struct _tuple22 _tmp3C2=(_tmpD85.f1=_tmp34B,((_tmpD85.f2=_tmp33E,_tmpD85)));struct _tuple22 _tmp3C3=_tmp3C2;struct _dyneither_ptr*_tmp3C4;struct Cyc_Absyn_Tqual _tmp3C5;void*_tmp3C6;int _tmp3C7;struct _dyneither_ptr*_tmp3C8;struct Cyc_Absyn_Tqual _tmp3C9;void*_tmp3CA;int _tmp3CB;if(_tmp3C3.f1 == 0){if(_tmp3C3.f2 == 0){_LL312: _LL313:
 goto _LL311;}else{_LL314: _LL315:
 goto _LL317;}}else{if(_tmp3C3.f2 == 0){_LL316: _LL317:
# 1674
 bad_cyc_vararg=1;
{const char*_tmpD86;Cyc_Tcutil_failure_reason=((_tmpD86="(only one function type takes varargs)",_tag_dyneither(_tmpD86,sizeof(char),39)));}
goto _LL311;}else{_LL318: _tmp3C4=(_tmp3C3.f1)->name;_tmp3C5=(_tmp3C3.f1)->tq;_tmp3C6=(_tmp3C3.f1)->type;_tmp3C7=(_tmp3C3.f1)->inject;_tmp3C8=(_tmp3C3.f2)->name;_tmp3C9=(_tmp3C3.f2)->tq;_tmp3CA=(_tmp3C3.f2)->type;_tmp3CB=(_tmp3C3.f2)->inject;_LL319:
# 1678
 Cyc_Tcutil_unify_it(_tmp3C6,_tmp3CA);
Cyc_Tcutil_unify_tqual(_tmp3C5,_tmp3C6,_tmp3C9,_tmp3CA);
if(_tmp3C7 != _tmp3CB){
bad_cyc_vararg=1;{
const char*_tmpD87;Cyc_Tcutil_failure_reason=((_tmpD87="(only one function type injects varargs)",_tag_dyneither(_tmpD87,sizeof(char),41)));};}
# 1684
goto _LL311;}}_LL311:;}
# 1686
if(bad_cyc_vararg){_npop_handler(0);goto _LL2CF;}{
# 1689
int bad_effect=0;
{struct _tuple0 _tmpD88;struct _tuple0 _tmp3CF=(_tmpD88.f1=_tmp346,((_tmpD88.f2=_tmp339,_tmpD88)));struct _tuple0 _tmp3D0=_tmp3CF;if(_tmp3D0.f1 == 0){if(_tmp3D0.f2 == 0){_LL31B: _LL31C:
 goto _LL31A;}else{_LL31D: _LL31E:
 goto _LL320;}}else{if(_tmp3D0.f2 == 0){_LL31F: _LL320:
 bad_effect=1;goto _LL31A;}else{_LL321: _LL322:
 bad_effect=!Cyc_Tcutil_unify_effect((void*)_check_null(_tmp346),(void*)_check_null(_tmp339));goto _LL31A;}}_LL31A:;}
# 1696
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(bad_effect){
{const char*_tmpD89;Cyc_Tcutil_failure_reason=((_tmpD89="(function effects do not match)",_tag_dyneither(_tmpD89,sizeof(char),32)));}
_npop_handler(0);goto _LL2CF;}
# 1702
if(!Cyc_Tcutil_same_atts(_tmp340,_tmp34D)){
{const char*_tmpD8A;Cyc_Tcutil_failure_reason=((_tmpD8A="(function types have different attributes)",_tag_dyneither(_tmpD8A,sizeof(char),43)));}
_npop_handler(0);goto _LL2CF;}
# 1706
if(!Cyc_Tcutil_same_rgn_po(_tmp33F,_tmp34C)){
{const char*_tmpD8B;Cyc_Tcutil_failure_reason=((_tmpD8B="(function types have different region lifetime orderings)",_tag_dyneither(_tmpD8B,sizeof(char),58)));}
_npop_handler(0);goto _LL2CF;}
# 1710
if(!Cyc_Tcutil_check_logical_equivalence(_tmp342,_tmp34F)){
{const char*_tmpD8C;Cyc_Tcutil_failure_reason=((_tmpD8C="(@requires clauses not equivalent)",_tag_dyneither(_tmpD8C,sizeof(char),35)));}
_npop_handler(0);goto _LL2CF;}
# 1714
if(!Cyc_Tcutil_check_logical_equivalence(_tmp344,_tmp351)){
{const char*_tmpD8D;Cyc_Tcutil_failure_reason=((_tmpD8D="(@ensures clauses not equivalent)",_tag_dyneither(_tmpD8D,sizeof(char),34)));}
_npop_handler(0);goto _LL2CF;}
# 1718
_npop_handler(0);return;};};
# 1612
;_pop_region(rgn);}case 24: goto _LL300;case 25: goto _LL306;case 23: goto _LL308;default: goto _LL30A;}case 10: switch(*((int*)_tmp327.f2)){case 10: _LL2EE: _tmp336=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp327.f1)->f1;_tmp337=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp327.f2)->f1;_LL2EF:
# 1721
 for(0;_tmp337 != 0  && _tmp336 != 0;(_tmp337=_tmp337->tl,_tmp336=_tmp336->tl)){
struct _tuple12 _tmp3D7=*((struct _tuple12*)_tmp337->hd);struct Cyc_Absyn_Tqual _tmp3D9;void*_tmp3DA;struct _tuple12 _tmp3D8=_tmp3D7;_tmp3D9=_tmp3D8.f1;_tmp3DA=_tmp3D8.f2;{
struct _tuple12 _tmp3DB=*((struct _tuple12*)_tmp336->hd);struct Cyc_Absyn_Tqual _tmp3DD;void*_tmp3DE;struct _tuple12 _tmp3DC=_tmp3DB;_tmp3DD=_tmp3DC.f1;_tmp3DE=_tmp3DC.f2;
Cyc_Tcutil_unify_it(_tmp3DA,_tmp3DE);
Cyc_Tcutil_unify_tqual(_tmp3D9,_tmp3DA,_tmp3DD,_tmp3DE);};}
# 1727
if(_tmp337 == 0  && _tmp336 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpD8E;Cyc_Tcutil_failure_reason=((_tmpD8E="(tuple types have different numbers of components)",_tag_dyneither(_tmpD8E,sizeof(char),51)));}
goto _LL2CF;case 24: goto _LL300;case 25: goto _LL306;case 23: goto _LL308;default: goto _LL30A;}case 12: switch(*((int*)_tmp327.f2)){case 12: _LL2F0: _tmp332=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp327.f1)->f1;_tmp333=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp327.f1)->f2;_tmp334=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp327.f2)->f1;_tmp335=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp327.f2)->f2;_LL2F1:
# 1734
 if(_tmp334 != _tmp332){{const char*_tmpD8F;Cyc_Tcutil_failure_reason=((_tmpD8F="(struct and union type)",_tag_dyneither(_tmpD8F,sizeof(char),24)));}goto _LL2CF;}
for(0;_tmp335 != 0  && _tmp333 != 0;(_tmp335=_tmp335->tl,_tmp333=_tmp333->tl)){
struct Cyc_Absyn_Aggrfield*_tmp3E1=(struct Cyc_Absyn_Aggrfield*)_tmp335->hd;
struct Cyc_Absyn_Aggrfield*_tmp3E2=(struct Cyc_Absyn_Aggrfield*)_tmp333->hd;
if(Cyc_strptrcmp(_tmp3E1->name,_tmp3E2->name)!= 0){
{const char*_tmpD90;Cyc_Tcutil_failure_reason=((_tmpD90="(different member names)",_tag_dyneither(_tmpD90,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1742
Cyc_Tcutil_unify_it(_tmp3E1->type,_tmp3E2->type);
Cyc_Tcutil_unify_tqual(_tmp3E1->tq,_tmp3E1->type,_tmp3E2->tq,_tmp3E2->type);
if(!Cyc_Tcutil_same_atts(_tmp3E1->attributes,_tmp3E2->attributes)){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpD91;Cyc_Tcutil_failure_reason=((_tmpD91="(different attributes on member)",_tag_dyneither(_tmpD91,sizeof(char),33)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1750
if((_tmp3E1->width != 0  && _tmp3E2->width == 0  || 
_tmp3E2->width != 0  && _tmp3E1->width == 0) || 
(_tmp3E1->width != 0  && _tmp3E2->width != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp3E1->width),(struct Cyc_Absyn_Exp*)_check_null(_tmp3E2->width))){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpD92;Cyc_Tcutil_failure_reason=((_tmpD92="(different bitfield widths on member)",_tag_dyneither(_tmpD92,sizeof(char),38)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1759
if((_tmp3E1->requires_clause != 0  && _tmp3E2->requires_clause == 0  || 
_tmp3E1->requires_clause == 0  && _tmp3E2->requires_clause != 0) || 
(_tmp3E1->requires_clause == 0  && _tmp3E2->requires_clause != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp3E1->requires_clause),(struct Cyc_Absyn_Exp*)_check_null(_tmp3E2->requires_clause))){
# 1764
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpD93;Cyc_Tcutil_failure_reason=((_tmpD93="(different @requires clauses on member)",_tag_dyneither(_tmpD93,sizeof(char),40)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1770
if(_tmp335 == 0  && _tmp333 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpD94;Cyc_Tcutil_failure_reason=((_tmpD94="(different number of members)",_tag_dyneither(_tmpD94,sizeof(char),30)));}
goto _LL2CF;case 24: goto _LL300;case 25: goto _LL306;case 23: goto _LL308;default: goto _LL30A;}case 17: switch(*((int*)_tmp327.f2)){case 17: _LL2F2: _tmp32E=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp327.f1)->f2;_tmp32F=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp327.f1)->f3;_tmp330=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp327.f2)->f2;_tmp331=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp327.f2)->f3;_LL2F3:
# 1776
 if(_tmp32F != _tmp331){
{const char*_tmpD95;Cyc_Tcutil_failure_reason=((_tmpD95="(different abstract typedefs)",_tag_dyneither(_tmpD95,sizeof(char),30)));}
goto _LL2CF;}
# 1780
{const char*_tmpD96;Cyc_Tcutil_failure_reason=((_tmpD96="(type parameters to typedef differ)",_tag_dyneither(_tmpD96,sizeof(char),36)));}
Cyc_Tcutil_unify_list(_tmp32E,_tmp330);
return;case 24: goto _LL300;case 25: goto _LL306;case 23: goto _LL308;default: goto _LL30A;}case 20: switch(*((int*)_tmp327.f2)){case 20: _LL2F4: _LL2F5:
 return;case 24: goto _LL300;case 25: goto _LL306;case 23: goto _LL308;default: goto _LL30A;}case 21: switch(*((int*)_tmp327.f2)){case 21: _LL2F6: _LL2F7:
 return;case 24: goto _LL300;case 25: goto _LL306;case 23: goto _LL308;default: goto _LL30A;}case 22: switch(*((int*)_tmp327.f2)){case 22: _LL2F8: _LL2F9:
 return;case 24: goto _LL300;case 25: goto _LL306;case 23: goto _LL308;default: goto _LL30A;}case 15: switch(*((int*)_tmp327.f2)){case 15: _LL2FA: _tmp32C=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp327.f1)->f1;_tmp32D=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp327.f2)->f1;_LL2FB:
# 1787
 Cyc_Tcutil_unify_it(_tmp32C,_tmp32D);
return;case 24: goto _LL300;case 25: goto _LL306;case 23: goto _LL308;default: goto _LL30A;}case 16: switch(*((int*)_tmp327.f2)){case 16: _LL2FC: _tmp328=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp327.f1)->f1;_tmp329=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp327.f1)->f2;_tmp32A=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp327.f2)->f1;_tmp32B=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp327.f2)->f2;_LL2FD:
# 1790
 Cyc_Tcutil_unify_it(_tmp328,_tmp32A);
Cyc_Tcutil_unify_it(_tmp329,_tmp32B);
return;case 24: goto _LL300;case 25: goto _LL306;case 23: goto _LL308;default: goto _LL30A;}case 24: _LL2FE: _LL2FF:
 goto _LL301;default: if(((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp327.f2)->tag == 24){_LL300: _LL301:
 goto _LL303;}else{switch(*((int*)_tmp327.f1)){case 23: _LL302: _LL303:
 goto _LL305;case 25: _LL304: _LL305:
 goto _LL307;default: switch(*((int*)_tmp327.f2)){case 25: _LL306: _LL307:
 goto _LL309;case 23: _LL308: _LL309:
# 1799
 if(Cyc_Tcutil_unify_effect(t1,t2))return;
{const char*_tmpD97;Cyc_Tcutil_failure_reason=((_tmpD97="(effects don't unify)",_tag_dyneither(_tmpD97,sizeof(char),22)));}
goto _LL2CF;default: _LL30A: _LL30B:
 goto _LL2CF;}}}}_LL2CF:;}
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
union Cyc_Absyn_Constraint*_tmp3EE=x;void*_tmp3EF;switch((((union Cyc_Absyn_Constraint*)_tmp3EE)->Eq_constr).tag){case 3: _LL324: _LL325:
 return - 1;case 1: _LL326: _tmp3EF=(void*)(_tmp3EE->Eq_constr).val;_LL327: {
# 1828
union Cyc_Absyn_Constraint*_tmp3F0=y;void*_tmp3F1;switch((((union Cyc_Absyn_Constraint*)_tmp3F0)->Eq_constr).tag){case 3: _LL32B: _LL32C:
 return 1;case 1: _LL32D: _tmp3F1=(void*)(_tmp3F0->Eq_constr).val;_LL32E:
 return cmp(_tmp3EF,_tmp3F1);default: _LL32F: _LL330: {
const char*_tmpD9A;void*_tmpD99;(_tmpD99=0,Cyc_Tcutil_impos(((_tmpD9A="unify_conref: forward after compress(2)",_tag_dyneither(_tmpD9A,sizeof(char),40))),_tag_dyneither(_tmpD99,sizeof(void*),0)));}}_LL32A:;}default: _LL328: _LL329: {
# 1833
const char*_tmpD9D;void*_tmpD9C;(_tmpD9C=0,Cyc_Tcutil_impos(((_tmpD9D="unify_conref: forward after compress",_tag_dyneither(_tmpD9D,sizeof(char),37))),_tag_dyneither(_tmpD9C,sizeof(void*),0)));}}_LL323:;};}
# 1837
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct Cyc_Absyn_Tqual _tmp3F7;void*_tmp3F8;struct _tuple12*_tmp3F6=tqt1;_tmp3F7=_tmp3F6->f1;_tmp3F8=_tmp3F6->f2;{
struct Cyc_Absyn_Tqual _tmp3FA;void*_tmp3FB;struct _tuple12*_tmp3F9=tqt2;_tmp3FA=_tmp3F9->f1;_tmp3FB=_tmp3F9->f2;{
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
void*_tmp402=t;switch(*((int*)_tmp402)){case 0: _LL332: _LL333:
 return 0;case 1: _LL334: _LL335:
 return 1;case 2: _LL336: _LL337:
 return 2;case 3: _LL338: _LL339:
 return 3;case 4: _LL33A: _LL33B:
 return 4;case 5: _LL33C: _LL33D:
 return 5;case 6: _LL33E: _LL33F:
 return 6;case 7: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp402)->f1 == 0){_LL340: _LL341:
 return 7;}else{_LL342: _LL343:
 return 8;}case 8: _LL344: _LL345:
 return 9;case 9: _LL346: _LL347:
 return 10;case 10: _LL348: _LL349:
 return 11;case 11: _LL34A: _LL34B:
 return 12;case 12: _LL34C: _LL34D:
 return 14;case 13: _LL34E: _LL34F:
 return 16;case 14: _LL350: _LL351:
 return 17;case 15: _LL352: _LL353:
 return 18;case 17: _LL354: _LL355:
 return 19;case 21: _LL356: _LL357:
 return 20;case 20: _LL358: _LL359:
 return 21;case 23: _LL35A: _LL35B:
 return 22;case 24: _LL35C: _LL35D:
 return 23;case 25: _LL35E: _LL35F:
 return 24;case 22: _LL360: _LL361:
 return 25;case 19: _LL362: _LL363:
 return 26;case 16: _LL364: _LL365:
 return 27;case 18: _LL366: _LL367:
 return 28;case 26: _LL368: _LL369:
 return 29;case 27: _LL36A: _LL36B:
 return 30;default: _LL36C: _LL36D:
 return 31;}_LL331:;}
# 1902
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return 0;{
int _tmp403=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));
if(_tmp403 != 0)
return _tmp403;{
# 1911
struct _tuple0 _tmpD9E;struct _tuple0 _tmp404=(_tmpD9E.f1=t2,((_tmpD9E.f2=t1,_tmpD9E)));struct _tuple0 _tmp405=_tmp404;struct _dyneither_ptr _tmp406;struct _dyneither_ptr _tmp407;struct Cyc_Absyn_Exp*_tmp408;struct Cyc_Absyn_Exp*_tmp409;struct Cyc_Absyn_Exp*_tmp40A;struct Cyc_Absyn_Exp*_tmp40B;void*_tmp40C;void*_tmp40D;void*_tmp40E;void*_tmp40F;void*_tmp410;void*_tmp411;void*_tmp412;void*_tmp413;enum Cyc_Absyn_AggrKind _tmp414;struct Cyc_List_List*_tmp415;enum Cyc_Absyn_AggrKind _tmp416;struct Cyc_List_List*_tmp417;struct Cyc_List_List*_tmp418;struct Cyc_List_List*_tmp419;void*_tmp41A;struct Cyc_Absyn_Tqual _tmp41B;struct Cyc_Absyn_Exp*_tmp41C;union Cyc_Absyn_Constraint*_tmp41D;void*_tmp41E;struct Cyc_Absyn_Tqual _tmp41F;struct Cyc_Absyn_Exp*_tmp420;union Cyc_Absyn_Constraint*_tmp421;int _tmp422;int _tmp423;enum Cyc_Absyn_Sign _tmp424;enum Cyc_Absyn_Size_of _tmp425;enum Cyc_Absyn_Sign _tmp426;enum Cyc_Absyn_Size_of _tmp427;void*_tmp428;struct Cyc_Absyn_Tqual _tmp429;void*_tmp42A;union Cyc_Absyn_Constraint*_tmp42B;union Cyc_Absyn_Constraint*_tmp42C;union Cyc_Absyn_Constraint*_tmp42D;void*_tmp42E;struct Cyc_Absyn_Tqual _tmp42F;void*_tmp430;union Cyc_Absyn_Constraint*_tmp431;union Cyc_Absyn_Constraint*_tmp432;union Cyc_Absyn_Constraint*_tmp433;struct Cyc_Absyn_Datatypedecl*_tmp434;struct Cyc_Absyn_Datatypefield*_tmp435;struct Cyc_List_List*_tmp436;struct Cyc_Absyn_Datatypedecl*_tmp437;struct Cyc_Absyn_Datatypefield*_tmp438;struct Cyc_List_List*_tmp439;struct Cyc_Absyn_Datatypedecl*_tmp43A;struct Cyc_List_List*_tmp43B;struct Cyc_Absyn_Datatypedecl*_tmp43C;struct Cyc_List_List*_tmp43D;struct Cyc_List_List*_tmp43E;struct Cyc_List_List*_tmp43F;struct _tuple2*_tmp440;struct _tuple2*_tmp441;union Cyc_Absyn_AggrInfoU _tmp442;struct Cyc_List_List*_tmp443;union Cyc_Absyn_AggrInfoU _tmp444;struct Cyc_List_List*_tmp445;struct Cyc_Absyn_Tvar*_tmp446;struct Cyc_Absyn_Tvar*_tmp447;switch(*((int*)_tmp405.f1)){case 1: switch(*((int*)_tmp405.f2)){case 1: _LL36F: _LL370: {
# 1913
const char*_tmpDA1;void*_tmpDA0;(_tmpDA0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpDA1="typecmp: can only compare closed types",_tag_dyneither(_tmpDA1,sizeof(char),39))),_tag_dyneither(_tmpDA0,sizeof(void*),0)));}case 24: goto _LL399;case 25: goto _LL39F;case 23: goto _LL3A1;default: goto _LL3A3;}case 2: switch(*((int*)_tmp405.f2)){case 2: _LL371: _tmp446=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp405.f1)->f1;_tmp447=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp405.f2)->f1;_LL372:
# 1917
 return Cyc_Core_intcmp(_tmp447->identity,_tmp446->identity);case 24: goto _LL399;case 25: goto _LL39F;case 23: goto _LL3A1;default: goto _LL3A3;}case 11: switch(*((int*)_tmp405.f2)){case 11: _LL373: _tmp442=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp405.f1)->f1).aggr_info;_tmp443=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp405.f1)->f1).targs;_tmp444=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp405.f2)->f1).aggr_info;_tmp445=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp405.f2)->f1).targs;_LL374: {
# 1921
struct _tuple13 _tmp44A=Cyc_Absyn_aggr_kinded_name(_tmp442);struct _tuple2*_tmp44C;struct _tuple13 _tmp44B=_tmp44A;_tmp44C=_tmp44B.f2;{
struct _tuple13 _tmp44D=Cyc_Absyn_aggr_kinded_name(_tmp444);struct _tuple2*_tmp44F;struct _tuple13 _tmp44E=_tmp44D;_tmp44F=_tmp44E.f2;{
int _tmp450=Cyc_Absyn_qvar_cmp(_tmp44C,_tmp44F);
if(_tmp450 != 0)return _tmp450;else{
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp443,_tmp445);}};};}case 24: goto _LL399;case 25: goto _LL39F;case 23: goto _LL3A1;default: goto _LL3A3;}case 13: switch(*((int*)_tmp405.f2)){case 13: _LL375: _tmp440=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp405.f1)->f1;_tmp441=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp405.f2)->f1;_LL376:
# 1928
 return Cyc_Absyn_qvar_cmp(_tmp440,_tmp441);case 24: goto _LL399;case 25: goto _LL39F;case 23: goto _LL3A1;default: goto _LL3A3;}case 14: switch(*((int*)_tmp405.f2)){case 14: _LL377: _tmp43E=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp405.f1)->f1;_tmp43F=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp405.f2)->f1;_LL378:
# 1931
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp43E,_tmp43F);case 24: goto _LL399;case 25: goto _LL39F;case 23: goto _LL3A1;default: goto _LL3A3;}case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp405.f1)->f1).datatype_info).KnownDatatype).tag == 2)switch(*((int*)_tmp405.f2)){case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp405.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL379: _tmp43A=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp405.f1)->f1).datatype_info).KnownDatatype).val;_tmp43B=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp405.f1)->f1).targs;_tmp43C=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp405.f2)->f1).datatype_info).KnownDatatype).val;_tmp43D=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp405.f2)->f1).targs;_LL37A:
# 1935
 if(_tmp43C == _tmp43A)return 0;{
int _tmp451=Cyc_Absyn_qvar_cmp(_tmp43C->name,_tmp43A->name);
if(_tmp451 != 0)return _tmp451;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp43D,_tmp43B);};}else{goto _LL3A3;}case 24: goto _LL399;case 25: goto _LL39F;case 23: goto _LL3A1;default: goto _LL3A3;}else{switch(*((int*)_tmp405.f2)){case 24: goto _LL399;case 25: goto _LL39F;case 23: goto _LL3A1;default: goto _LL3A3;}}case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp405.f1)->f1).field_info).KnownDatatypefield).tag == 2)switch(*((int*)_tmp405.f2)){case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp405.f2)->f1).field_info).KnownDatatypefield).tag == 2){_LL37B: _tmp434=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp405.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp435=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp405.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp436=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp405.f1)->f1).targs;_tmp437=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp405.f2)->f1).field_info).KnownDatatypefield).val).f1;_tmp438=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp405.f2)->f1).field_info).KnownDatatypefield).val).f2;_tmp439=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp405.f2)->f1).targs;_LL37C:
# 1942
 if(_tmp437 == _tmp434)return 0;{
int _tmp452=Cyc_Absyn_qvar_cmp(_tmp434->name,_tmp437->name);
if(_tmp452 != 0)return _tmp452;{
int _tmp453=Cyc_Absyn_qvar_cmp(_tmp435->name,_tmp438->name);
if(_tmp453 != 0)return _tmp453;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp439,_tmp436);};};}else{goto _LL3A3;}case 24: goto _LL399;case 25: goto _LL39F;case 23: goto _LL3A1;default: goto _LL3A3;}else{switch(*((int*)_tmp405.f2)){case 24: goto _LL399;case 25: goto _LL39F;case 23: goto _LL3A1;default: goto _LL3A3;}}case 5: switch(*((int*)_tmp405.f2)){case 5: _LL37D: _tmp428=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp405.f1)->f1).elt_typ;_tmp429=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp405.f1)->f1).elt_tq;_tmp42A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp405.f1)->f1).ptr_atts).rgn;_tmp42B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp405.f1)->f1).ptr_atts).nullable;_tmp42C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp405.f1)->f1).ptr_atts).bounds;_tmp42D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp405.f1)->f1).ptr_atts).zero_term;_tmp42E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp405.f2)->f1).elt_typ;_tmp42F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp405.f2)->f1).elt_tq;_tmp430=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp405.f2)->f1).ptr_atts).rgn;_tmp431=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp405.f2)->f1).ptr_atts).nullable;_tmp432=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp405.f2)->f1).ptr_atts).bounds;_tmp433=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp405.f2)->f1).ptr_atts).zero_term;_LL37E: {
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
{void*_tmp459=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp432);void*_tmp45A=_tmp459;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp45A)->tag == 0){_LL3A6: _LL3A7:
 return 0;}else{_LL3A8: _LL3A9:
 goto _LL3A5;}_LL3A5:;}
# 1965
return((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp431,_tmp42B);};};};};}case 24: goto _LL399;case 25: goto _LL39F;case 23: goto _LL3A1;default: goto _LL3A3;}case 6: switch(*((int*)_tmp405.f2)){case 6: _LL37F: _tmp424=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp405.f1)->f1;_tmp425=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp405.f1)->f2;_tmp426=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp405.f2)->f1;_tmp427=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp405.f2)->f2;_LL380:
# 1968
 if(_tmp426 != _tmp424)return Cyc_Core_intcmp((int)((unsigned int)_tmp426),(int)((unsigned int)_tmp424));
if(_tmp427 != _tmp425)return Cyc_Core_intcmp((int)((unsigned int)_tmp427),(int)((unsigned int)_tmp425));
return 0;case 24: goto _LL399;case 25: goto _LL39F;case 23: goto _LL3A1;default: goto _LL3A3;}case 7: switch(*((int*)_tmp405.f2)){case 7: _LL381: _tmp422=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp405.f1)->f1;_tmp423=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp405.f2)->f1;_LL382:
# 1973
 if(_tmp422 == _tmp423)return 0;else{
if(_tmp423 == 0)return - 1;else{
if(_tmp423 == 1  && _tmp422 == 0)return - 1;else{
return 1;}}}case 24: goto _LL399;case 25: goto _LL39F;case 23: goto _LL3A1;default: goto _LL3A3;}case 8: switch(*((int*)_tmp405.f2)){case 8: _LL383: _tmp41A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp405.f1)->f1).elt_type;_tmp41B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp405.f1)->f1).tq;_tmp41C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp405.f1)->f1).num_elts;_tmp41D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp405.f1)->f1).zero_term;_tmp41E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp405.f2)->f1).elt_type;_tmp41F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp405.f2)->f1).tq;_tmp420=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp405.f2)->f1).num_elts;_tmp421=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp405.f2)->f1).zero_term;_LL384: {
# 1980
int _tmp45B=Cyc_Tcutil_tqual_cmp(_tmp41F,_tmp41B);
if(_tmp45B != 0)return _tmp45B;{
int _tmp45C=Cyc_Tcutil_typecmp(_tmp41E,_tmp41A);
if(_tmp45C != 0)return _tmp45C;{
int _tmp45D=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp41D,_tmp421);
if(_tmp45D != 0)return _tmp45D;
if(_tmp41C == _tmp420)return 0;
if(_tmp41C == 0  || _tmp420 == 0){
const char*_tmpDA4;void*_tmpDA3;(_tmpDA3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpDA4="missing expression in array index",_tag_dyneither(_tmpDA4,sizeof(char),34))),_tag_dyneither(_tmpDA3,sizeof(void*),0)));}
# 1990
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp41C,_tmp420);};};}case 24: goto _LL399;case 25: goto _LL39F;case 23: goto _LL3A1;default: goto _LL3A3;}case 9: switch(*((int*)_tmp405.f2)){case 9: _LL385: _LL386: {
# 1993
const char*_tmpDA7;void*_tmpDA6;(_tmpDA6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpDA7="typecmp: function types not handled",_tag_dyneither(_tmpDA7,sizeof(char),36))),_tag_dyneither(_tmpDA6,sizeof(void*),0)));}case 24: goto _LL399;case 25: goto _LL39F;case 23: goto _LL3A1;default: goto _LL3A3;}case 10: switch(*((int*)_tmp405.f2)){case 10: _LL387: _tmp418=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp405.f1)->f1;_tmp419=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp405.f2)->f1;_LL388:
# 1996
 return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp419,_tmp418);case 24: goto _LL399;case 25: goto _LL39F;case 23: goto _LL3A1;default: goto _LL3A3;}case 12: switch(*((int*)_tmp405.f2)){case 12: _LL389: _tmp414=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp405.f1)->f1;_tmp415=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp405.f1)->f2;_tmp416=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp405.f2)->f1;_tmp417=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp405.f2)->f2;_LL38A:
# 1999
 if(_tmp416 != _tmp414){
if(_tmp416 == Cyc_Absyn_StructA)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp417,_tmp415);case 24: goto _LL399;case 25: goto _LL39F;case 23: goto _LL3A1;default: goto _LL3A3;}case 15: switch(*((int*)_tmp405.f2)){case 15: _LL38B: _tmp412=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp405.f1)->f1;_tmp413=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp405.f2)->f1;_LL38C:
# 2004
 return Cyc_Tcutil_typecmp(_tmp412,_tmp413);case 24: goto _LL399;case 25: goto _LL39F;case 23: goto _LL3A1;default: goto _LL3A3;}case 16: switch(*((int*)_tmp405.f2)){case 16: _LL38D: _tmp40E=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp405.f1)->f1;_tmp40F=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp405.f1)->f2;_tmp410=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp405.f2)->f1;_tmp411=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp405.f2)->f2;_LL38E: {
# 2006
int _tmp462=Cyc_Tcutil_typecmp(_tmp40E,_tmp410);
if(_tmp462 != 0)return _tmp462;else{
return Cyc_Tcutil_typecmp(_tmp40F,_tmp411);}}case 24: goto _LL399;case 25: goto _LL39F;case 23: goto _LL3A1;default: goto _LL3A3;}case 19: switch(*((int*)_tmp405.f2)){case 19: _LL38F: _tmp40C=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp405.f1)->f1;_tmp40D=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp405.f2)->f1;_LL390:
 return Cyc_Tcutil_typecmp(_tmp40C,_tmp40D);case 24: goto _LL399;case 25: goto _LL39F;case 23: goto _LL3A1;default: goto _LL3A3;}case 18: switch(*((int*)_tmp405.f2)){case 18: _LL391: _tmp40A=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp405.f1)->f1;_tmp40B=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp405.f2)->f1;_LL392:
 _tmp408=_tmp40A;_tmp409=_tmp40B;goto _LL394;case 24: goto _LL399;case 25: goto _LL39F;case 23: goto _LL3A1;default: goto _LL3A3;}case 27: switch(*((int*)_tmp405.f2)){case 27: _LL393: _tmp408=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp405.f1)->f1;_tmp409=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp405.f2)->f1;_LL394:
# 2012
 return Cyc_Evexp_const_exp_cmp(_tmp408,_tmp409);case 24: goto _LL399;case 25: goto _LL39F;case 23: goto _LL3A1;default: goto _LL3A3;}case 28: switch(*((int*)_tmp405.f2)){case 28: _LL395: _tmp406=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp405.f1)->f1;_tmp407=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp405.f2)->f1;_LL396:
# 2014
 return Cyc_strcmp((struct _dyneither_ptr)_tmp406,(struct _dyneither_ptr)_tmp407);case 24: goto _LL399;case 25: goto _LL39F;case 23: goto _LL3A1;default: goto _LL3A3;}case 24: _LL397: _LL398:
 goto _LL39A;default: if(((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp405.f2)->tag == 24){_LL399: _LL39A:
 goto _LL39C;}else{switch(*((int*)_tmp405.f1)){case 23: _LL39B: _LL39C:
 goto _LL39E;case 25: _LL39D: _LL39E:
 goto _LL3A0;default: switch(*((int*)_tmp405.f2)){case 25: _LL39F: _LL3A0:
 goto _LL3A2;case 23: _LL3A1: _LL3A2: {
const char*_tmpDAA;void*_tmpDA9;(_tmpDA9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpDAA="typecmp: effects not handled",_tag_dyneither(_tmpDAA,sizeof(char),29))),_tag_dyneither(_tmpDA9,sizeof(void*),0)));}default: _LL3A3: _LL3A4: {
const char*_tmpDAD;void*_tmpDAC;(_tmpDAC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpDAD="Unmatched case in typecmp",_tag_dyneither(_tmpDAD,sizeof(char),26))),_tag_dyneither(_tmpDAC,sizeof(void*),0)));}}}}}_LL36E:;};};}
# 2025
int Cyc_Tcutil_is_arithmetic_type(void*t){
void*_tmp468=Cyc_Tcutil_compress(t);void*_tmp469=_tmp468;switch(*((int*)_tmp469)){case 6: _LL3AB: _LL3AC:
 goto _LL3AE;case 7: _LL3AD: _LL3AE:
 goto _LL3B0;case 13: _LL3AF: _LL3B0:
 goto _LL3B2;case 14: _LL3B1: _LL3B2:
 return 1;default: _LL3B3: _LL3B4:
 return 0;}_LL3AA:;}
# 2037
int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmpDAE;struct _tuple0 _tmp46A=(_tmpDAE.f1=t1,((_tmpDAE.f2=t2,_tmpDAE)));struct _tuple0 _tmp46B=_tmp46A;int _tmp46C;int _tmp46D;switch(*((int*)_tmp46B.f1)){case 7: switch(*((int*)_tmp46B.f2)){case 7: _LL3B6: _tmp46C=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp46B.f1)->f1;_tmp46D=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp46B.f2)->f1;_LL3B7:
# 2042
 return _tmp46D == 0  && _tmp46C != 0  || (_tmp46D == 1  && _tmp46C != 0) && _tmp46C != 1;case 6: _LL3B8: _LL3B9:
 goto _LL3BB;case 19: _LL3BA: _LL3BB:
 return 1;default: goto _LL3D6;}case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46B.f1)->f2){case Cyc_Absyn_LongLong_sz: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46B.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46B.f2)->f2 == Cyc_Absyn_LongLong_sz){_LL3BC: _LL3BD:
 return 0;}else{goto _LL3BE;}}else{_LL3BE: _LL3BF:
 return 1;}case Cyc_Absyn_Long_sz: switch(*((int*)_tmp46B.f2)){case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46B.f2)->f2){case Cyc_Absyn_Int_sz: _LL3C0: _LL3C1:
# 2049
 goto _LL3C3;case Cyc_Absyn_Short_sz: _LL3C8: _LL3C9:
# 2054
 goto _LL3CB;case Cyc_Absyn_Char_sz: _LL3CE: _LL3CF:
# 2057
 goto _LL3D1;default: goto _LL3D6;}case 7: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp46B.f2)->f1 == 0){_LL3C4: _LL3C5:
# 2052
 goto _LL3C7;}else{goto _LL3D6;}default: goto _LL3D6;}case Cyc_Absyn_Int_sz: switch(*((int*)_tmp46B.f2)){case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46B.f2)->f2){case Cyc_Absyn_Long_sz: _LL3C2: _LL3C3:
# 2050
 return 0;case Cyc_Absyn_Short_sz: _LL3CA: _LL3CB:
# 2055
 goto _LL3CD;case Cyc_Absyn_Char_sz: _LL3D0: _LL3D1:
# 2058
 goto _LL3D3;default: goto _LL3D6;}case 7: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp46B.f2)->f1 == 0){_LL3C6: _LL3C7:
# 2053
 goto _LL3C9;}else{goto _LL3D6;}default: goto _LL3D6;}case Cyc_Absyn_Short_sz: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46B.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46B.f2)->f2 == Cyc_Absyn_Char_sz){_LL3D2: _LL3D3:
# 2059
 goto _LL3D5;}else{goto _LL3D6;}}else{goto _LL3D6;}default: goto _LL3D6;}case 19: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46B.f2)->tag == 6)switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46B.f2)->f2){case Cyc_Absyn_Short_sz: _LL3CC: _LL3CD:
# 2056
 goto _LL3CF;case Cyc_Absyn_Char_sz: _LL3D4: _LL3D5:
# 2060
 return 1;default: goto _LL3D6;}else{goto _LL3D6;}default: _LL3D6: _LL3D7:
# 2062
 return 0;}_LL3B5:;};}
# 2068
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
# 2071
struct _RegionHandle _tmp46F=_new_region("r");struct _RegionHandle*r=& _tmp46F;_push_region(r);{
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0  || 
Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v)){
struct Cyc_Core_Opt*_tmpDAF;max_arith_type=((_tmpDAF=_region_malloc(r,sizeof(*_tmpDAF)),((_tmpDAF->v=t1,_tmpDAF))));}}}}
# 2081
if(max_arith_type != 0){
if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v)){
int _tmp471=0;_npop_handler(0);return _tmp471;}}}
# 2086
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){
{const char*_tmpDB4;void*_tmpDB3[2];struct Cyc_String_pa_PrintArg_struct _tmpDB2;struct Cyc_String_pa_PrintArg_struct _tmpDB1;(_tmpDB1.tag=0,((_tmpDB1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))),((_tmpDB2.tag=0,((_tmpDB2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpDB3[0]=& _tmpDB2,((_tmpDB3[1]=& _tmpDB1,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)el->hd)->loc,((_tmpDB4="type mismatch: expecting %s but found %s",_tag_dyneither(_tmpDB4,sizeof(char),41))),_tag_dyneither(_tmpDB3,sizeof(void*),2)))))))))))));}{
int _tmp476=0;_npop_handler(0);return _tmp476;};}}}{
# 2092
int _tmp477=1;_npop_handler(0);return _tmp477;};
# 2071
;_pop_region(r);}
# 2097
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_typ(te,e)){
void*_tmp478=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp479=_tmp478;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp479)->tag == 5){_LL3D9: _LL3DA:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);goto _LL3D8;}else{_LL3DB: _LL3DC:
 return 0;}_LL3D8:;}
# 2103
return 1;}
# 2106
int Cyc_Tcutil_is_integral_type(void*t){
void*_tmp47A=Cyc_Tcutil_compress(t);void*_tmp47B=_tmp47A;switch(*((int*)_tmp47B)){case 6: _LL3DE: _LL3DF:
 goto _LL3E1;case 19: _LL3E0: _LL3E1:
 goto _LL3E3;case 13: _LL3E2: _LL3E3:
 goto _LL3E5;case 14: _LL3E4: _LL3E5:
 return 1;default: _LL3E6: _LL3E7:
 return 0;}_LL3DD:;}
# 2117
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
return 1;
# 2121
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_typ)){
const char*_tmpDB7;void*_tmpDB6;(_tmpDB6=0,Cyc_Tcutil_warn(e->loc,((_tmpDB7="integral size mismatch; conversion supplied",_tag_dyneither(_tmpDB7,sizeof(char),44))),_tag_dyneither(_tmpDB6,sizeof(void*),0)));}
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
const char*_tmpDBA;void*_tmpDB9;(_tmpDB9=0,Cyc_Tcutil_warn(e->loc,((_tmpDBA="integral size mismatch; conversion supplied",_tag_dyneither(_tmpDBA,sizeof(char),44))),_tag_dyneither(_tmpDB9,sizeof(void*),0)));}
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
struct _tuple0 _tmpDBB;struct _tuple0 _tmp480=(_tmpDBB.f1=t1,((_tmpDBB.f2=t2,_tmpDBB)));struct _tuple0 _tmp481=_tmp480;void*_tmp482;struct Cyc_Absyn_Tqual _tmp483;struct Cyc_Absyn_Exp*_tmp484;union Cyc_Absyn_Constraint*_tmp485;void*_tmp486;struct Cyc_Absyn_Tqual _tmp487;struct Cyc_Absyn_Exp*_tmp488;union Cyc_Absyn_Constraint*_tmp489;struct Cyc_Absyn_PtrInfo _tmp48A;struct Cyc_Absyn_PtrInfo _tmp48B;switch(*((int*)_tmp481.f1)){case 5: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp481.f2)->tag == 5){_LL3E9: _tmp48A=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp481.f1)->f1;_tmp48B=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp481.f2)->f1;_LL3EA: {
# 2160
int okay=1;
# 2162
if(!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp48A.ptr_atts).nullable,(_tmp48B.ptr_atts).nullable))
# 2165
okay=!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp48A.ptr_atts).nullable);
# 2167
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp48A.ptr_atts).bounds,(_tmp48B.ptr_atts).bounds)){
# 2170
struct _tuple0 _tmpDBC;struct _tuple0 _tmp48C=(_tmpDBC.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp48A.ptr_atts).bounds),((_tmpDBC.f2=
((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp48B.ptr_atts).bounds),_tmpDBC)));
# 2170
struct _tuple0 _tmp48D=_tmp48C;struct Cyc_Absyn_Exp*_tmp48E;struct Cyc_Absyn_Exp*_tmp48F;struct Cyc_Absyn_Exp*_tmp490;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp48D.f1)->tag == 1){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp48D.f2)->tag == 0){_LL3F2: _LL3F3:
# 2172
 goto _LL3F5;}else{_LL3F6: _tmp48F=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp48D.f1)->f1;_tmp490=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp48D.f2)->f1;_LL3F7:
# 2175
 okay=okay  && Cyc_Evexp_lte_const_exp(_tmp490,_tmp48F);
# 2179
if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp48B.ptr_atts).zero_term)){
const char*_tmpDBF;void*_tmpDBE;(_tmpDBE=0,Cyc_Tcutil_warn(loc,((_tmpDBF="implicit cast to shorter array",_tag_dyneither(_tmpDBF,sizeof(char),31))),_tag_dyneither(_tmpDBE,sizeof(void*),0)));}
goto _LL3F1;}}else{if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp48D.f2)->tag == 0){_LL3F4: _LL3F5:
# 2173
 okay=1;goto _LL3F1;}else{_LL3F8: _tmp48E=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp48D.f2)->f1;_LL3F9:
# 2184
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp48A.ptr_atts).zero_term) && 
Cyc_Tcutil_is_bound_one((_tmp48B.ptr_atts).bounds))
goto _LL3F1;
okay=0;
goto _LL3F1;}}_LL3F1:;}
# 2193
okay=okay  && (!(_tmp48A.elt_tq).real_const  || (_tmp48B.elt_tq).real_const);
# 2196
if(!Cyc_Tcutil_unify((_tmp48A.ptr_atts).rgn,(_tmp48B.ptr_atts).rgn)){
if(Cyc_Tcenv_region_outlives(te,(_tmp48A.ptr_atts).rgn,(_tmp48B.ptr_atts).rgn)){
if(Cyc_Tcutil_warn_region_coerce){
const char*_tmpDC4;void*_tmpDC3[2];struct Cyc_String_pa_PrintArg_struct _tmpDC2;struct Cyc_String_pa_PrintArg_struct _tmpDC1;(_tmpDC1.tag=0,((_tmpDC1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2201
Cyc_Absynpp_typ2string((_tmp48B.ptr_atts).rgn)),((_tmpDC2.tag=0,((_tmpDC2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2200
Cyc_Absynpp_typ2string((_tmp48A.ptr_atts).rgn)),((_tmpDC3[0]=& _tmpDC2,((_tmpDC3[1]=& _tmpDC1,Cyc_Tcutil_warn(loc,((_tmpDC4="implicit cast form region %s to region %s",_tag_dyneither(_tmpDC4,sizeof(char),42))),_tag_dyneither(_tmpDC3,sizeof(void*),2)))))))))))));}}else{
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
return okay;};}}else{goto _LL3EF;}case 8: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp481.f2)->tag == 8){_LL3EB: _tmp482=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp481.f1)->f1).elt_type;_tmp483=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp481.f1)->f1).tq;_tmp484=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp481.f1)->f1).num_elts;_tmp485=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp481.f1)->f1).zero_term;_tmp486=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp481.f2)->f1).elt_type;_tmp487=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp481.f2)->f1).tq;_tmp488=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp481.f2)->f1).num_elts;_tmp489=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp481.f2)->f1).zero_term;_LL3EC: {
# 2231
int okay;
# 2233
okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp485,_tmp489) && (
(_tmp484 != 0  && _tmp488 != 0) && Cyc_Evexp_same_const_exp(_tmp484,_tmp488));
# 2236
return(okay  && Cyc_Tcutil_unify(_tmp482,_tmp486)) && (!_tmp483.real_const  || _tmp487.real_const);}}else{goto _LL3EF;}case 19: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp481.f2)->tag == 6){_LL3ED: _LL3EE:
# 2238
 return 0;}else{goto _LL3EF;}default: _LL3EF: _LL3F0:
# 2240
 return Cyc_Tcutil_unify(t1,t2);}_LL3E8:;};}
# 2244
int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp49A=Cyc_Tcutil_compress(t);void*_tmp49B=_tmp49A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49B)->tag == 5){_LL3FB: _LL3FC:
 return 1;}else{_LL3FD: _LL3FE:
 return 0;}_LL3FA:;}
# 2250
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp49C=Cyc_Tcutil_compress(t);void*_tmp49D=_tmp49C;void*_tmp49E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49D)->tag == 5){_LL400: _tmp49E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49D)->f1).elt_typ;_LL401:
 return _tmp49E;}else{_LL402: _LL403: {
const char*_tmpDC7;void*_tmpDC6;(_tmpDC6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpDC7="pointer_elt_type",_tag_dyneither(_tmpDC7,sizeof(char),17))),_tag_dyneither(_tmpDC6,sizeof(void*),0)));}}_LL3FF:;}
# 2256
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp4A1=Cyc_Tcutil_compress(t);void*_tmp4A2=_tmp4A1;struct Cyc_Absyn_PtrAtts*_tmp4A3;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A2)->tag == 5){_LL405: _tmp4A3=(struct Cyc_Absyn_PtrAtts*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A2)->f1).ptr_atts;_LL406:
 return _tmp4A3->rgn;}else{_LL407: _LL408: {
const char*_tmpDCA;void*_tmpDC9;(_tmpDC9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpDCA="pointer_elt_type",_tag_dyneither(_tmpDCA,sizeof(char),17))),_tag_dyneither(_tmpDC9,sizeof(void*),0)));}}_LL404:;}
# 2263
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp4A6=Cyc_Tcutil_compress(t);void*_tmp4A7=_tmp4A6;void*_tmp4A8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A7)->tag == 5){_LL40A: _tmp4A8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A7)->f1).ptr_atts).rgn;_LL40B:
# 2266
*rgn=_tmp4A8;
return 1;}else{_LL40C: _LL40D:
 return 0;}_LL409:;}
# 2273
int Cyc_Tcutil_is_boxed(void*t){
return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;}
# 2277
int Cyc_Tcutil_is_dyneither_ptr(void*t){
void*_tmp4A9=Cyc_Tcutil_compress(t);void*_tmp4AA=_tmp4A9;union Cyc_Absyn_Constraint*_tmp4AB;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AA)->tag == 5){_LL40F: _tmp4AB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AA)->f1).ptr_atts).bounds;_LL410:
# 2280
 return((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp4AB)== (void*)& Cyc_Absyn_DynEither_b_val;}else{_LL411: _LL412:
 return 0;}_LL40E:;}
# 2286
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp4AC=e->r;void*_tmp4AD=_tmp4AC;void*_tmp4AE;struct Cyc_Absyn_Exp*_tmp4AF;struct _dyneither_ptr _tmp4B0;switch(*((int*)_tmp4AD)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4AD)->f1).Wchar_c).tag){case 5: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4AD)->f1).Int_c).val).f2 == 0){_LL414: _LL415:
 goto _LL417;}else{goto _LL420;}case 2: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4AD)->f1).Char_c).val).f2 == 0){_LL416: _LL417:
 goto _LL419;}else{goto _LL420;}case 4: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4AD)->f1).Short_c).val).f2 == 0){_LL418: _LL419:
 goto _LL41B;}else{goto _LL420;}case 6: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4AD)->f1).LongLong_c).val).f2 == 0){_LL41A: _LL41B:
# 2292
 return 1;}else{goto _LL420;}case 3: _LL41C: _tmp4B0=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4AD)->f1).Wchar_c).val;_LL41D: {
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
return 0;}}default: goto _LL420;}case 13: _LL41E: _tmp4AE=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4AD)->f1;_tmp4AF=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4AD)->f2;_LL41F:
 return Cyc_Tcutil_is_zero(_tmp4AF) && Cyc_Tcutil_admits_zero(_tmp4AE);default: _LL420: _LL421:
 return 0;}_LL413:;}
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
enum Cyc_Absyn_KindQual _tmp4B3;enum Cyc_Absyn_AliasQual _tmp4B4;struct Cyc_Absyn_Kind*_tmp4B2=ka;_tmp4B3=_tmp4B2->kind;_tmp4B4=_tmp4B2->aliasqual;
{enum Cyc_Absyn_AliasQual _tmp4B5=_tmp4B4;switch(_tmp4B5){case Cyc_Absyn_Aliasable: _LL423: _LL424: {
# 2348
enum Cyc_Absyn_KindQual _tmp4B6=_tmp4B3;switch(_tmp4B6){case Cyc_Absyn_AnyKind: _LL42A: _LL42B:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LL42C: _LL42D:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL42E: _LL42F:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL430: _LL431:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL432: _LL433:
 return& Cyc_Tcutil_eko;default: _LL434: _LL435:
 return& Cyc_Tcutil_iko;}_LL429:;}case Cyc_Absyn_Unique: _LL425: _LL426:
# 2357
{enum Cyc_Absyn_KindQual _tmp4B7=_tmp4B3;switch(_tmp4B7){case Cyc_Absyn_AnyKind: _LL437: _LL438:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL439: _LL43A:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL43B: _LL43C:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL43D: _LL43E:
 return& Cyc_Tcutil_urko;default: _LL43F: _LL440:
 goto _LL436;}_LL436:;}
# 2364
goto _LL422;default: _LL427: _LL428:
# 2366
{enum Cyc_Absyn_KindQual _tmp4B8=_tmp4B3;switch(_tmp4B8){case Cyc_Absyn_AnyKind: _LL442: _LL443:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL444: _LL445:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL446: _LL447:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL448: _LL449:
 return& Cyc_Tcutil_trko;default: _LL44A: _LL44B:
 goto _LL441;}_LL441:;}
# 2373
goto _LL422;}_LL422:;}{
# 2375
const char*_tmpDCE;void*_tmpDCD[1];struct Cyc_String_pa_PrintArg_struct _tmpDCC;(_tmpDCC.tag=0,((_tmpDCC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmpDCD[0]=& _tmpDCC,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpDCE="kind_to_opt: bad kind %s\n",_tag_dyneither(_tmpDCE,sizeof(char),26))),_tag_dyneither(_tmpDCD,sizeof(void*),1)))))));};}
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
enum Cyc_Absyn_KindQual _tmp4BD;enum Cyc_Absyn_AliasQual _tmp4BE;struct Cyc_Absyn_Kind*_tmp4BC=ka;_tmp4BD=_tmp4BC->kind;_tmp4BE=_tmp4BC->aliasqual;
{enum Cyc_Absyn_AliasQual _tmp4BF=_tmp4BE;switch(_tmp4BF){case Cyc_Absyn_Aliasable: _LL44D: _LL44E: {
# 2416
enum Cyc_Absyn_KindQual _tmp4C0=_tmp4BD;switch(_tmp4C0){case Cyc_Absyn_AnyKind: _LL454: _LL455:
 return& ab;case Cyc_Absyn_MemKind: _LL456: _LL457:
 return& mb;case Cyc_Absyn_BoxKind: _LL458: _LL459:
 return& bb;case Cyc_Absyn_RgnKind: _LL45A: _LL45B:
 return& rb;case Cyc_Absyn_EffKind: _LL45C: _LL45D:
 return& eb;default: _LL45E: _LL45F:
 return& ib;}_LL453:;}case Cyc_Absyn_Unique: _LL44F: _LL450:
# 2425
{enum Cyc_Absyn_KindQual _tmp4C1=_tmp4BD;switch(_tmp4C1){case Cyc_Absyn_AnyKind: _LL461: _LL462:
 return& uab;case Cyc_Absyn_MemKind: _LL463: _LL464:
 return& umb;case Cyc_Absyn_BoxKind: _LL465: _LL466:
 return& ubb;case Cyc_Absyn_RgnKind: _LL467: _LL468:
 return& urb;default: _LL469: _LL46A:
 goto _LL460;}_LL460:;}
# 2432
goto _LL44C;default: _LL451: _LL452:
# 2434
{enum Cyc_Absyn_KindQual _tmp4C2=_tmp4BD;switch(_tmp4C2){case Cyc_Absyn_AnyKind: _LL46C: _LL46D:
 return& tab;case Cyc_Absyn_MemKind: _LL46E: _LL46F:
 return& tmb;case Cyc_Absyn_BoxKind: _LL470: _LL471:
 return& tbb;case Cyc_Absyn_RgnKind: _LL472: _LL473:
 return& trb;default: _LL474: _LL475:
 goto _LL46B;}_LL46B:;}
# 2441
goto _LL44C;}_LL44C:;}{
# 2443
const char*_tmpDD2;void*_tmpDD1[1];struct Cyc_String_pa_PrintArg_struct _tmpDD0;(_tmpDD0.tag=0,((_tmpDD0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmpDD1[0]=& _tmpDD0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpDD2="kind_to_b: bad kind %s\n",_tag_dyneither(_tmpDD2,sizeof(char),24))),_tag_dyneither(_tmpDD1,sizeof(void*),1)))))));};}
# 2446
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return*Cyc_Tcutil_kind_to_b(k);}
# 2449
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
# 2451
struct Cyc_Core_Opt*_tmpDD3;return(_tmpDD3=_cycalloc(sizeof(*_tmpDD3)),((_tmpDD3->v=Cyc_Tcutil_kind_to_bound(k),_tmpDD3)));}
# 2456
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){
{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmpDD6;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpDD5;e1->r=(void*)((_tmpDD5=_cycalloc(sizeof(*_tmpDD5)),((_tmpDD5[0]=((_tmpDD6.tag=0,((_tmpDD6.f1=Cyc_Absyn_Null_c,_tmpDD6)))),_tmpDD5))));}{
struct Cyc_Core_Opt*_tmp4D7=Cyc_Tcenv_lookup_opt_type_vars(te);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpDDC;struct Cyc_Absyn_PtrInfo _tmpDDB;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpDDA;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4D8=(_tmpDDA=_cycalloc(sizeof(*_tmpDDA)),((_tmpDDA[0]=((_tmpDDC.tag=5,((_tmpDDC.f1=((_tmpDDB.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp4D7),((_tmpDDB.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmpDDB.ptr_atts=(
((_tmpDDB.ptr_atts).rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp4D7),(((_tmpDDB.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmpDDB.ptr_atts).bounds=
# 2464
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmpDDB.ptr_atts).zero_term=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmpDDB.ptr_atts).ptrloc=0,_tmpDDB.ptr_atts)))))))))),_tmpDDB)))))),_tmpDDC)))),_tmpDDA)));
e1->topt=(void*)_tmp4D8;{
int bogus=0;
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
if(bogus != 0){
const char*_tmpDE1;void*_tmpDE0[2];struct Cyc_String_pa_PrintArg_struct _tmpDDF;struct Cyc_String_pa_PrintArg_struct _tmpDDE;(_tmpDDE.tag=0,((_tmpDDE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(e1->loc)),((_tmpDDF.tag=0,((_tmpDDF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmpDE0[0]=& _tmpDDF,((_tmpDE0[1]=& _tmpDDE,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpDE1="zero_to_null resulted in an alias coercion on %s at %s\n",_tag_dyneither(_tmpDE1,sizeof(char),56))),_tag_dyneither(_tmpDE0,sizeof(void*),2)))))))))))));}
return retv;};};}
# 2474
return 0;}
# 2477
struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
enum Cyc_Absyn_Coercion _tmp4E0=c;switch(_tmp4E0){case Cyc_Absyn_Unknown_coercion: _LL477: _LL478: {
const char*_tmpDE2;return(_tmpDE2="unknown",_tag_dyneither(_tmpDE2,sizeof(char),8));}case Cyc_Absyn_No_coercion: _LL479: _LL47A: {
const char*_tmpDE3;return(_tmpDE3="no coercion",_tag_dyneither(_tmpDE3,sizeof(char),12));}case Cyc_Absyn_NonNull_to_Null: _LL47B: _LL47C: {
const char*_tmpDE4;return(_tmpDE4="null check",_tag_dyneither(_tmpDE4,sizeof(char),11));}default: _LL47D: _LL47E: {
const char*_tmpDE5;return(_tmpDE5="other coercion",_tag_dyneither(_tmpDE5,sizeof(char),15));}}_LL476:;}
# 2486
int Cyc_Tcutil_warn_alias_coerce=0;
# 2492
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0,& Cyc_Tcutil_rk};
# 2496
static int counter=0;
struct _dyneither_ptr*_tmpDF2;const char*_tmpDF1;void*_tmpDF0[1];struct Cyc_Int_pa_PrintArg_struct _tmpDEF;struct _tuple2*_tmpDEE;struct _tuple2*v=(_tmpDEE=_cycalloc(sizeof(*_tmpDEE)),((_tmpDEE->f1=Cyc_Absyn_Loc_n,((_tmpDEE->f2=((_tmpDF2=_cycalloc(sizeof(*_tmpDF2)),((_tmpDF2[0]=(struct _dyneither_ptr)((_tmpDEF.tag=1,((_tmpDEF.f1=(unsigned long)counter ++,((_tmpDF0[0]=& _tmpDEF,Cyc_aprintf(((_tmpDF1="__aliasvar%d",_tag_dyneither(_tmpDF1,sizeof(char),13))),_tag_dyneither(_tmpDF0,sizeof(void*),1)))))))),_tmpDF2)))),_tmpDEE)))));
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(v,e_typ,e);
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpDF5;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpDF4;struct Cyc_Absyn_Exp*ve=Cyc_Absyn_varb_exp(v,(void*)((_tmpDF4=_cycalloc(sizeof(*_tmpDF4)),((_tmpDF4[0]=((_tmpDF5.tag=4,((_tmpDF5.f1=vd,_tmpDF5)))),_tmpDF4)))),e->loc);
# 2505
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 2507
{void*_tmp4E5=Cyc_Tcutil_compress(e_typ);void*_tmp4E6=_tmp4E5;void*_tmp4E7;struct Cyc_Absyn_Tqual _tmp4E8;void*_tmp4E9;union Cyc_Absyn_Constraint*_tmp4EA;union Cyc_Absyn_Constraint*_tmp4EB;union Cyc_Absyn_Constraint*_tmp4EC;struct Cyc_Absyn_PtrLoc*_tmp4ED;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E6)->tag == 5){_LL480: _tmp4E7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E6)->f1).elt_typ;_tmp4E8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E6)->f1).elt_tq;_tmp4E9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E6)->f1).ptr_atts).rgn;_tmp4EA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E6)->f1).ptr_atts).nullable;_tmp4EB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E6)->f1).ptr_atts).bounds;_tmp4EC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E6)->f1).ptr_atts).zero_term;_tmp4ED=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E6)->f1).ptr_atts).ptrloc;_LL481:
# 2509
{void*_tmp4EE=Cyc_Tcutil_compress(_tmp4E9);void*_tmp4EF=_tmp4EE;void**_tmp4F0;struct Cyc_Core_Opt*_tmp4F1;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4EF)->tag == 1){_LL485: _tmp4F0=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4EF)->f2;_tmp4F1=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4EF)->f4;_LL486: {
# 2511
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpDF8;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpDF7;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4F2=(_tmpDF7=_cycalloc(sizeof(*_tmpDF7)),((_tmpDF7[0]=((_tmpDF8.tag=2,((_tmpDF8.f1=tv,_tmpDF8)))),_tmpDF7)));
*_tmp4F0=(void*)_tmp4F2;
goto _LL484;}}else{_LL487: _LL488:
 goto _LL484;}_LL484:;}
# 2516
goto _LL47F;}else{_LL482: _LL483:
 goto _LL47F;}_LL47F:;}
# 2520
e->topt=0;
vd->initializer=0;{
# 2524
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 2526
struct _tuple15 _tmpDF9;return(_tmpDF9.f1=d,((_tmpDF9.f2=ve,_tmpDF9)));};}
# 2531
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ,void*wants_typ,unsigned int loc){
# 2534
struct _RegionHandle _tmp4FE=_new_region("r");struct _RegionHandle*r=& _tmp4FE;_push_region(r);
if((Cyc_Tcutil_is_noalias_path(r,e) && Cyc_Tcutil_is_noalias_pointer(e_typ,0)) && 
Cyc_Tcutil_is_pointer_type(e_typ)){
# 2539
void*_tmp4FF=Cyc_Tcutil_compress(wants_typ);void*_tmp500=_tmp4FF;void*_tmp501;switch(*((int*)_tmp500)){case 5: _LL48A: _tmp501=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp500)->f1).ptr_atts).rgn;_LL48B: {
# 2541
void*_tmp502=Cyc_Tcutil_compress(_tmp501);void*_tmp503=_tmp502;if(((struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp503)->tag == 20){_LL491: _LL492: {
int _tmp504=0;_npop_handler(0);return _tmp504;}}else{_LL493: _LL494: {
# 2544
struct Cyc_Absyn_Kind*_tmp505=Cyc_Tcutil_typ_kind(_tmp501);
int _tmp506=_tmp505->kind == Cyc_Absyn_RgnKind  && _tmp505->aliasqual == Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp506;}}_LL490:;}case 17: _LL48C: _LL48D: {
# 2548
const char*_tmpDFE;void*_tmpDFD[2];struct Cyc_String_pa_PrintArg_struct _tmpDFC;struct Cyc_String_pa_PrintArg_struct _tmpDFB;(_tmpDFB.tag=0,((_tmpDFB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(loc)),((_tmpDFC.tag=0,((_tmpDFC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_compress(wants_typ))),((_tmpDFD[0]=& _tmpDFC,((_tmpDFD[1]=& _tmpDFB,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpDFE="got typedef %s in can_insert_alias at %s\n",_tag_dyneither(_tmpDFE,sizeof(char),42))),_tag_dyneither(_tmpDFD,sizeof(void*),2)))))))))))));}default: _LL48E: _LL48F: {
int _tmp50B=0;_npop_handler(0);return _tmp50B;}}_LL489:;}{
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
if(Cyc_Tcutil_will_lose_precision(t1,t2)){
const char*_tmpE03;void*_tmpE02[2];struct Cyc_String_pa_PrintArg_struct _tmpE01;struct Cyc_String_pa_PrintArg_struct _tmpE00;(_tmpE00.tag=0,((_tmpE00.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmpE01.tag=0,((_tmpE01.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmpE02[0]=& _tmpE01,((_tmpE02[1]=& _tmpE00,Cyc_Tcutil_warn(e->loc,((_tmpE03="integral size mismatch; %s -> %s conversion supplied",_tag_dyneither(_tmpE03,sizeof(char),53))),_tag_dyneither(_tmpE02,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
return 1;}else{
# 2575
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce){
const char*_tmpE09;void*_tmpE08[3];struct Cyc_String_pa_PrintArg_struct _tmpE07;struct Cyc_String_pa_PrintArg_struct _tmpE06;struct Cyc_String_pa_PrintArg_struct _tmpE05;(_tmpE05.tag=0,((_tmpE05.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmpE06.tag=0,((_tmpE06.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmpE07.tag=0,((_tmpE07.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpE08[0]=& _tmpE07,((_tmpE08[1]=& _tmpE06,((_tmpE08[2]=& _tmpE05,Cyc_Tcutil_warn(e->loc,((_tmpE09="implicit alias coercion for %s:%s to %s",_tag_dyneither(_tmpE09,sizeof(char),40))),_tag_dyneither(_tmpE08,sizeof(void*),3)))))))))))))))))));}
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
const char*_tmpE0E;void*_tmpE0D[2];struct Cyc_String_pa_PrintArg_struct _tmpE0C;struct Cyc_String_pa_PrintArg_struct _tmpE0B;(_tmpE0B.tag=0,((_tmpE0B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmpE0C.tag=0,((_tmpE0C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmpE0D[0]=& _tmpE0C,((_tmpE0D[1]=& _tmpE0B,Cyc_Tcutil_warn(e->loc,((_tmpE0E="implicit cast from %s to %s",_tag_dyneither(_tmpE0E,sizeof(char),28))),_tag_dyneither(_tmpE0D,sizeof(void*),2)))))))))))));}
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
void*_tmp51A=Cyc_Tcutil_compress(t);void*_tmp51B=_tmp51A;switch(*((int*)_tmp51B)){case 6: _LL496: _LL497:
 goto _LL499;case 7: _LL498: _LL499:
 return 1;default: _LL49A: _LL49B:
 return 0;}_LL495:;}
# 2628 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 2632
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ_f(struct _tuple23*env,struct Cyc_Absyn_Aggrfield*x){
# 2635
struct _tuple23 _tmp51C=*env;struct Cyc_List_List*_tmp51E;struct _RegionHandle*_tmp51F;struct Cyc_Tcenv_Tenv*_tmp520;int _tmp521;struct _tuple23 _tmp51D=_tmp51C;_tmp51E=_tmp51D.f1;_tmp51F=_tmp51D.f2;_tmp520=_tmp51D.f3;_tmp521=_tmp51D.f4;{
# 2637
void*_tmp522=_tmp51E == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp51F,_tmp51E,x->type);
struct Cyc_List_List*_tmp523=Cyc_Tcutil_flatten_typ(_tmp51F,_tmp521,_tmp520,_tmp522);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp523)== 1){
struct _tuple12*_tmpE11;struct Cyc_List_List*_tmpE10;return(_tmpE10=_region_malloc(_tmp51F,sizeof(*_tmpE10)),((_tmpE10->hd=((_tmpE11=_region_malloc(_tmp51F,sizeof(*_tmpE11)),((_tmpE11->f1=x->tq,((_tmpE11->f2=_tmp522,_tmpE11)))))),((_tmpE10->tl=0,_tmpE10)))));}else{
return _tmp523;}};}struct _tuple24{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2643
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple24*env,struct _tuple12*x){
# 2645
struct _tuple24 _tmp526=*env;struct _RegionHandle*_tmp528;struct Cyc_Tcenv_Tenv*_tmp529;int _tmp52A;struct _tuple24 _tmp527=_tmp526;_tmp528=_tmp527.f1;_tmp529=_tmp527.f2;_tmp52A=_tmp527.f3;{
struct _tuple12 _tmp52B=*x;struct Cyc_Absyn_Tqual _tmp52D;void*_tmp52E;struct _tuple12 _tmp52C=_tmp52B;_tmp52D=_tmp52C.f1;_tmp52E=_tmp52C.f2;{
struct Cyc_List_List*_tmp52F=Cyc_Tcutil_flatten_typ(_tmp528,_tmp52A,_tmp529,_tmp52E);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp52F)== 1){
struct _tuple12*_tmpE14;struct Cyc_List_List*_tmpE13;return(_tmpE13=_region_malloc(_tmp528,sizeof(*_tmpE13)),((_tmpE13->hd=((_tmpE14=_region_malloc(_tmp528,sizeof(*_tmpE14)),((_tmpE14->f1=_tmp52D,((_tmpE14->f2=_tmp52E,_tmpE14)))))),((_tmpE13->tl=0,_tmpE13)))));}else{
return _tmp52F;}};};}
# 2652
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2656
if(flatten){
t1=Cyc_Tcutil_compress(t1);{
void*_tmp532=t1;struct Cyc_List_List*_tmp533;struct Cyc_Absyn_Aggrdecl*_tmp534;struct Cyc_List_List*_tmp535;struct Cyc_List_List*_tmp536;switch(*((int*)_tmp532)){case 0: _LL49D: _LL49E:
 return 0;case 10: _LL49F: _tmp536=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp532)->f1;_LL4A0: {
# 2661
struct _tuple24 _tmpE15;struct _tuple24 _tmp537=(_tmpE15.f1=r,((_tmpE15.f2=te,((_tmpE15.f3=flatten,_tmpE15)))));
# 2663
struct Cyc_List_List*_tmp538=_tmp536;struct _tuple12*_tmp539;struct Cyc_List_List*_tmp53A;if(_tmp538 == 0){_LL4A8: _LL4A9:
 return 0;}else{_LL4AA: _tmp539=(struct _tuple12*)_tmp538->hd;_tmp53A=_tmp538->tl;_LL4AB: {
# 2666
struct Cyc_List_List*_tmp53B=Cyc_Tcutil_rcopy_tqt(& _tmp537,_tmp539);
_tmp537.f3=0;{
struct Cyc_List_List*_tmp53C=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple12*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp537,_tmp536);
struct Cyc_List_List*_tmpE16;struct Cyc_List_List*_tmp53D=(_tmpE16=_region_malloc(r,sizeof(*_tmpE16)),((_tmpE16->hd=_tmp53B,((_tmpE16->tl=_tmp53C,_tmpE16)))));
struct Cyc_List_List*_tmpE17;return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,((_tmpE17=_region_malloc(r,sizeof(*_tmpE17)),((_tmpE17->hd=_tmp53B,((_tmpE17->tl=_tmp53C,_tmpE17)))))));};}}_LL4A7:;}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp532)->f1).aggr_info).KnownAggr).tag == 2){_LL4A1: _tmp534=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp532)->f1).aggr_info).KnownAggr).val;_tmp535=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp532)->f1).targs;_LL4A2:
# 2674
 if(((_tmp534->kind == Cyc_Absyn_UnionA  || _tmp534->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp534->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp534->impl))->rgn_po != 0){
# 2676
struct _tuple12*_tmpE1A;struct Cyc_List_List*_tmpE19;return(_tmpE19=_region_malloc(r,sizeof(*_tmpE19)),((_tmpE19->hd=((_tmpE1A=_region_malloc(r,sizeof(*_tmpE1A)),((_tmpE1A->f1=Cyc_Absyn_empty_tqual(0),((_tmpE1A->f2=t1,_tmpE1A)))))),((_tmpE19->tl=0,_tmpE19)))));}{
struct Cyc_List_List*_tmp543=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp534->tvs,_tmp535);
struct _tuple23 _tmpE1B;struct _tuple23 env=(_tmpE1B.f1=_tmp543,((_tmpE1B.f2=r,((_tmpE1B.f3=te,((_tmpE1B.f4=flatten,_tmpE1B)))))));
struct Cyc_List_List*_tmp544=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp534->impl))->fields;struct Cyc_List_List*_tmp545=_tmp544;struct Cyc_Absyn_Aggrfield*_tmp546;struct Cyc_List_List*_tmp547;if(_tmp545 == 0){_LL4AD: _LL4AE:
 return 0;}else{_LL4AF: _tmp546=(struct Cyc_Absyn_Aggrfield*)_tmp545->hd;_tmp547=_tmp545->tl;_LL4B0: {
# 2682
struct Cyc_List_List*_tmp548=Cyc_Tcutil_flatten_typ_f(& env,_tmp546);
env.f4=0;{
struct Cyc_List_List*_tmp549=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp547);
struct Cyc_List_List*_tmpE1C;struct Cyc_List_List*_tmp54A=(_tmpE1C=_region_malloc(r,sizeof(*_tmpE1C)),((_tmpE1C->hd=_tmp548,((_tmpE1C->tl=_tmp549,_tmpE1C)))));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp54A);};}}_LL4AC:;};}else{goto _LL4A5;}case 12: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp532)->f1 == Cyc_Absyn_StructA){_LL4A3: _tmp533=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp532)->f2;_LL4A4: {
# 2689
struct _tuple23 _tmpE1D;struct _tuple23 env=(_tmpE1D.f1=0,((_tmpE1D.f2=r,((_tmpE1D.f3=te,((_tmpE1D.f4=flatten,_tmpE1D)))))));
struct Cyc_List_List*_tmp54D=_tmp533;struct Cyc_Absyn_Aggrfield*_tmp54E;struct Cyc_List_List*_tmp54F;if(_tmp54D == 0){_LL4B2: _LL4B3:
 return 0;}else{_LL4B4: _tmp54E=(struct Cyc_Absyn_Aggrfield*)_tmp54D->hd;_tmp54F=_tmp54D->tl;_LL4B5: {
# 2693
struct Cyc_List_List*_tmp550=Cyc_Tcutil_flatten_typ_f(& env,_tmp54E);
env.f4=0;{
struct Cyc_List_List*_tmp551=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp54F);
struct Cyc_List_List*_tmpE1E;struct Cyc_List_List*_tmp552=(_tmpE1E=_region_malloc(r,sizeof(*_tmpE1E)),((_tmpE1E->hd=_tmp550,((_tmpE1E->tl=_tmp551,_tmpE1E)))));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp552);};}}_LL4B1:;}}else{goto _LL4A5;}default: _LL4A5: _LL4A6:
# 2699
 goto _LL49C;}_LL49C:;};}{
# 2702
struct _tuple12*_tmpE21;struct Cyc_List_List*_tmpE20;return(_tmpE20=_region_malloc(r,sizeof(*_tmpE20)),((_tmpE20->hd=((_tmpE21=_region_malloc(r,sizeof(*_tmpE21)),((_tmpE21->f1=Cyc_Absyn_empty_tqual(0),((_tmpE21->f2=t1,_tmpE21)))))),((_tmpE20->tl=0,_tmpE20)))));};}
# 2706
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp557=(void*)t->hd;void*_tmp558=_tmp557;switch(*((int*)_tmp558)){case 23: _LL4B7: _LL4B8:
 goto _LL4BA;case 4: _LL4B9: _LL4BA:
 goto _LL4BC;case 20: _LL4BB: _LL4BC:
# 2712
 continue;default: _LL4BD: _LL4BE:
# 2714
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;}_LL4B6:;}}
# 2717
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}
# 2720
return 1;}
# 2723
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmpE22;struct _tuple0 _tmp559=(_tmpE22.f1=Cyc_Tcutil_compress(t1),((_tmpE22.f2=Cyc_Tcutil_compress(t2),_tmpE22)));struct _tuple0 _tmp55A=_tmp559;enum Cyc_Absyn_Size_of _tmp55B;enum Cyc_Absyn_Size_of _tmp55C;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp55A.f1)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp55A.f2)->tag == 6){_LL4C0: _tmp55B=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp55A.f1)->f2;_tmp55C=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp55A.f2)->f2;_LL4C1:
# 2726
 return(_tmp55B == _tmp55C  || _tmp55B == Cyc_Absyn_Int_sz  && _tmp55C == Cyc_Absyn_Long_sz) || 
_tmp55B == Cyc_Absyn_Long_sz  && _tmp55C == Cyc_Absyn_Int_sz;}else{goto _LL4C2;}}else{_LL4C2: _LL4C3:
 return 0;}_LL4BF:;}
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
struct _tuple0 _tmpE23;struct _tuple0 _tmp55E=(_tmpE23.f1=t1,((_tmpE23.f2=t2,_tmpE23)));struct _tuple0 _tmp55F=_tmp55E;struct Cyc_Absyn_FnInfo _tmp560;struct Cyc_Absyn_FnInfo _tmp561;struct Cyc_Absyn_Datatypedecl*_tmp562;struct Cyc_Absyn_Datatypefield*_tmp563;struct Cyc_List_List*_tmp564;struct Cyc_Absyn_Datatypedecl*_tmp565;struct Cyc_List_List*_tmp566;void*_tmp567;struct Cyc_Absyn_Tqual _tmp568;void*_tmp569;union Cyc_Absyn_Constraint*_tmp56A;union Cyc_Absyn_Constraint*_tmp56B;union Cyc_Absyn_Constraint*_tmp56C;void*_tmp56D;struct Cyc_Absyn_Tqual _tmp56E;void*_tmp56F;union Cyc_Absyn_Constraint*_tmp570;union Cyc_Absyn_Constraint*_tmp571;union Cyc_Absyn_Constraint*_tmp572;switch(*((int*)_tmp55F.f1)){case 5: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55F.f2)->tag == 5){_LL4C5: _tmp567=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55F.f1)->f1).elt_typ;_tmp568=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55F.f1)->f1).elt_tq;_tmp569=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55F.f1)->f1).ptr_atts).rgn;_tmp56A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55F.f1)->f1).ptr_atts).nullable;_tmp56B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55F.f1)->f1).ptr_atts).bounds;_tmp56C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55F.f1)->f1).ptr_atts).zero_term;_tmp56D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55F.f2)->f1).elt_typ;_tmp56E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55F.f2)->f1).elt_tq;_tmp56F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55F.f2)->f1).ptr_atts).rgn;_tmp570=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55F.f2)->f1).ptr_atts).nullable;_tmp571=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55F.f2)->f1).ptr_atts).bounds;_tmp572=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55F.f2)->f1).ptr_atts).zero_term;_LL4C6:
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
struct _tuple0 _tmpE24;struct _tuple0 _tmp573=(_tmpE24.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp56B),((_tmpE24.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp571),_tmpE24)));struct _tuple0 _tmp574=_tmp573;struct Cyc_Absyn_Exp*_tmp575;struct Cyc_Absyn_Exp*_tmp576;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp574.f1)->tag == 0){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp574.f2)->tag == 0){_LL4CE: _LL4CF:
 goto _LL4CD;}else{goto _LL4D2;}}else{if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp574.f2)->tag == 1){_LL4D0: _tmp575=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp574.f1)->f1;_tmp576=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp574.f2)->f1;_LL4D1:
# 2770
 if(!Cyc_Evexp_lte_const_exp(_tmp576,_tmp575))
return 0;
goto _LL4CD;}else{_LL4D2: _LL4D3:
 return 0;}}_LL4CD:;}
# 2778
if(!_tmp56E.real_const  && _tmp568.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp56D)))
return 0;}{
# 2784
int _tmp578=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp571,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp572);
# 2790
struct _tuple0*_tmpE27;struct Cyc_List_List*_tmpE26;return(_tmp578  && Cyc_Tcutil_ptrsubtype(te,((_tmpE26=_cycalloc(sizeof(*_tmpE26)),((_tmpE26->hd=((_tmpE27=_cycalloc(sizeof(*_tmpE27)),((_tmpE27->f1=t1,((_tmpE27->f2=t2,_tmpE27)))))),((_tmpE26->tl=assume,_tmpE26)))))),_tmp567,_tmp56D) || Cyc_Tcutil_unify(_tmp567,_tmp56D)) || Cyc_Tcutil_isomorphic(_tmp567,_tmp56D);};}else{goto _LL4CB;}case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp55F.f1)->f1).field_info).KnownDatatypefield).tag == 2){if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp55F.f2)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp55F.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL4C7: _tmp562=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp55F.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp563=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp55F.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp564=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp55F.f1)->f1).targs;_tmp565=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp55F.f2)->f1).datatype_info).KnownDatatype).val;_tmp566=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp55F.f2)->f1).targs;_LL4C8:
# 2796
 if(_tmp562 != _tmp565  && Cyc_Absyn_qvar_cmp(_tmp562->name,_tmp565->name)!= 0)return 0;
# 2798
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp564)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp566))return 0;
for(0;_tmp564 != 0;(_tmp564=_tmp564->tl,_tmp566=_tmp566->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp564->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp566))->hd))return 0;}
return 1;}else{goto _LL4CB;}}else{goto _LL4CB;}}else{goto _LL4CB;}case 9: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp55F.f2)->tag == 9){_LL4C9: _tmp560=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp55F.f1)->f1;_tmp561=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp55F.f2)->f1;_LL4CA:
# 2805
 if(_tmp560.tvars != 0  || _tmp561.tvars != 0){
struct Cyc_List_List*_tmp57B=_tmp560.tvars;
struct Cyc_List_List*_tmp57C=_tmp561.tvars;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp57B)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp57C))return 0;{
struct _RegionHandle _tmp57D=_new_region("r");struct _RegionHandle*r=& _tmp57D;_push_region(r);
{struct Cyc_List_List*inst=0;
while(_tmp57B != 0){
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp57B->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp57C))->hd)->kind)){int _tmp57E=0;_npop_handler(0);return _tmp57E;}
{struct _tuple16*_tmpE31;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpE30;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpE2F;struct Cyc_List_List*_tmpE2E;inst=((_tmpE2E=_region_malloc(r,sizeof(*_tmpE2E)),((_tmpE2E->hd=((_tmpE31=_region_malloc(r,sizeof(*_tmpE31)),((_tmpE31->f1=(struct Cyc_Absyn_Tvar*)_tmp57C->hd,((_tmpE31->f2=(void*)((_tmpE2F=_cycalloc(sizeof(*_tmpE2F)),((_tmpE2F[0]=((_tmpE30.tag=2,((_tmpE30.f1=(struct Cyc_Absyn_Tvar*)_tmp57B->hd,_tmpE30)))),_tmpE2F)))),_tmpE31)))))),((_tmpE2E->tl=inst,_tmpE2E))))));}
_tmp57B=_tmp57B->tl;
_tmp57C=_tmp57C->tl;}
# 2817
if(inst != 0){
_tmp560.tvars=0;
_tmp561.tvars=0;{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpE37;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE36;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpE34;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE33;int _tmp587=Cyc_Tcutil_subtype(te,assume,(void*)((_tmpE33=_cycalloc(sizeof(*_tmpE33)),((_tmpE33[0]=((_tmpE34.tag=9,((_tmpE34.f1=_tmp560,_tmpE34)))),_tmpE33)))),(void*)((_tmpE36=_cycalloc(sizeof(*_tmpE36)),((_tmpE36[0]=((_tmpE37.tag=9,((_tmpE37.f1=_tmp561,_tmpE37)))),_tmpE36)))));_npop_handler(0);return _tmp587;};}}
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
struct _tuple10 _tmp58A=*((struct _tuple10*)_tmp588->hd);struct Cyc_Absyn_Tqual _tmp58C;void*_tmp58D;struct _tuple10 _tmp58B=_tmp58A;_tmp58C=_tmp58B.f2;_tmp58D=_tmp58B.f3;{
struct _tuple10 _tmp58E=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp589))->hd);struct Cyc_Absyn_Tqual _tmp590;void*_tmp591;struct _tuple10 _tmp58F=_tmp58E;_tmp590=_tmp58F.f2;_tmp591=_tmp58F.f3;
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
return 1;};}else{goto _LL4CB;}default: _LL4CB: _LL4CC:
 return 0;}_LL4C4:;};}
# 2874 "tcutil.cyc"
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2876
struct _RegionHandle _tmp595=_new_region("temp");struct _RegionHandle*temp=& _tmp595;_push_region(temp);
{struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(temp,1,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(temp,1,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0){int _tmp596=0;_npop_handler(0);return _tmp596;}{
struct _tuple12*_tmp597=(struct _tuple12*)tqs1->hd;struct Cyc_Absyn_Tqual _tmp599;void*_tmp59A;struct _tuple12*_tmp598=_tmp597;_tmp599=_tmp598->f1;_tmp59A=_tmp598->f2;{
struct _tuple12*_tmp59B=(struct _tuple12*)tqs2->hd;struct Cyc_Absyn_Tqual _tmp59D;void*_tmp59E;struct _tuple12*_tmp59C=_tmp59B;_tmp59D=_tmp59C->f1;_tmp59E=_tmp59C->f2;
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
void*_tmp5A2=Cyc_Tcutil_compress(t);void*_tmp5A3=_tmp5A2;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A3)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A3)->f2 == Cyc_Absyn_Char_sz){_LL4D5: _LL4D6:
 return 1;}else{goto _LL4D7;}}else{_LL4D7: _LL4D8:
 return 0;}_LL4D4:;}
# 2915
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
# 2921
if(t2 == (void*)& Cyc_Absyn_VoidType_val)
return Cyc_Absyn_No_coercion;
{void*_tmp5A4=t2;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A4)->tag == 6)switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A4)->f2){case Cyc_Absyn_Int_sz: _LL4DA: _LL4DB:
# 2925
 goto _LL4DD;case Cyc_Absyn_Long_sz: _LL4DC: _LL4DD:
# 2927
 if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL4D9;default: goto _LL4DE;}else{_LL4DE: _LL4DF:
 goto _LL4D9;}_LL4D9:;}{
# 2931
void*_tmp5A5=t1;void*_tmp5A6;struct Cyc_Absyn_Enumdecl*_tmp5A7;void*_tmp5A8;struct Cyc_Absyn_Tqual _tmp5A9;struct Cyc_Absyn_Exp*_tmp5AA;union Cyc_Absyn_Constraint*_tmp5AB;void*_tmp5AC;struct Cyc_Absyn_Tqual _tmp5AD;void*_tmp5AE;union Cyc_Absyn_Constraint*_tmp5AF;union Cyc_Absyn_Constraint*_tmp5B0;union Cyc_Absyn_Constraint*_tmp5B1;switch(*((int*)_tmp5A5)){case 5: _LL4E1: _tmp5AC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A5)->f1).elt_typ;_tmp5AD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A5)->f1).elt_tq;_tmp5AE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A5)->f1).ptr_atts).rgn;_tmp5AF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A5)->f1).ptr_atts).nullable;_tmp5B0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A5)->f1).ptr_atts).bounds;_tmp5B1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A5)->f1).ptr_atts).zero_term;_LL4E2:
# 2939
{void*_tmp5B2=t2;void*_tmp5B3;struct Cyc_Absyn_Tqual _tmp5B4;void*_tmp5B5;union Cyc_Absyn_Constraint*_tmp5B6;union Cyc_Absyn_Constraint*_tmp5B7;union Cyc_Absyn_Constraint*_tmp5B8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B2)->tag == 5){_LL4F0: _tmp5B3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B2)->f1).elt_typ;_tmp5B4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B2)->f1).elt_tq;_tmp5B5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B2)->f1).ptr_atts).rgn;_tmp5B6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B2)->f1).ptr_atts).nullable;_tmp5B7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B2)->f1).ptr_atts).bounds;_tmp5B8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B2)->f1).ptr_atts).zero_term;_LL4F1: {
# 2943
enum Cyc_Absyn_Coercion coercion=Cyc_Absyn_Other_coercion;
struct _tuple0*_tmpE3A;struct Cyc_List_List*_tmpE39;struct Cyc_List_List*_tmp5B9=(_tmpE39=_cycalloc(sizeof(*_tmpE39)),((_tmpE39->hd=((_tmpE3A=_cycalloc(sizeof(*_tmpE3A)),((_tmpE3A->f1=t1,((_tmpE3A->f2=t2,_tmpE3A)))))),((_tmpE39->tl=0,_tmpE39)))));
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
struct _tuple0 _tmpE3B;struct _tuple0 _tmp5BE=(_tmpE3B.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp5B0),((_tmpE3B.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp5B7),_tmpE3B)));struct _tuple0 _tmp5BF=_tmp5BE;struct Cyc_Absyn_Exp*_tmp5C0;struct Cyc_Absyn_Exp*_tmp5C1;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5BF.f1)->tag == 1){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5BF.f2)->tag == 1){_LL4F5: _tmp5C0=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5BF.f1)->f1;_tmp5C1=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5BF.f2)->f1;_LL4F6:
# 2979
 if(Cyc_Evexp_lte_const_exp(_tmp5C1,_tmp5C0))
bounds_ok=1;
goto _LL4F4;}else{goto _LL4F7;}}else{_LL4F7: _LL4F8:
# 2984
 bounds_ok=1;goto _LL4F4;}_LL4F4:;}
# 2986
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5AF) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5B6))
coercion=Cyc_Absyn_NonNull_to_Null;
# 2991
if(((bounds_ok  && zeroterm_ok) && (_tmp5BC  || _tmp5BD)) && (
Cyc_Tcutil_unify(_tmp5AE,_tmp5B5) || Cyc_Tcenv_region_outlives(te,_tmp5AE,_tmp5B5)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};}}else{_LL4F2: _LL4F3:
 goto _LL4EF;}_LL4EF:;}
# 2997
return Cyc_Absyn_Unknown_coercion;case 8: _LL4E3: _tmp5A8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A5)->f1).elt_type;_tmp5A9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A5)->f1).tq;_tmp5AA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A5)->f1).num_elts;_tmp5AB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A5)->f1).zero_term;_LL4E4:
# 2999
{void*_tmp5C5=t2;void*_tmp5C6;struct Cyc_Absyn_Tqual _tmp5C7;struct Cyc_Absyn_Exp*_tmp5C8;union Cyc_Absyn_Constraint*_tmp5C9;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5C5)->tag == 8){_LL4FA: _tmp5C6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5C5)->f1).elt_type;_tmp5C7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5C5)->f1).tq;_tmp5C8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5C5)->f1).num_elts;_tmp5C9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5C5)->f1).zero_term;_LL4FB: {
# 3001
int okay;
okay=
(((_tmp5AA != 0  && _tmp5C8 != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp5AB,_tmp5C9)) && 
Cyc_Evexp_lte_const_exp(_tmp5C8,_tmp5AA)) && 
Cyc_Evexp_lte_const_exp(_tmp5AA,_tmp5C8);
return
# 3008
(okay  && Cyc_Tcutil_unify(_tmp5A8,_tmp5C6)) && (!_tmp5A9.real_const  || _tmp5C7.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}}else{_LL4FC: _LL4FD:
# 3010
 return Cyc_Absyn_Unknown_coercion;}_LL4F9:;}
# 3012
return Cyc_Absyn_Unknown_coercion;case 13: _LL4E5: _tmp5A7=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5A5)->f2;_LL4E6:
# 3016
{void*_tmp5CA=t2;struct Cyc_Absyn_Enumdecl*_tmp5CB;if(((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5CA)->tag == 13){_LL4FF: _tmp5CB=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5CA)->f2;_LL500:
# 3018
 if((_tmp5A7->fields != 0  && _tmp5CB->fields != 0) && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5A7->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5CB->fields))->v))
return Cyc_Absyn_Other_coercion;
goto _LL4FE;}else{_LL501: _LL502:
 goto _LL4FE;}_LL4FE:;}
# 3024
goto _LL4E8;case 6: _LL4E7: _LL4E8:
 goto _LL4EA;case 7: _LL4E9: _LL4EA:
 return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;case 15: _LL4EB: _tmp5A6=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5A5)->f1;_LL4EC:
# 3029
{void*_tmp5CC=t2;void*_tmp5CD;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5CC)->tag == 15){_LL504: _tmp5CD=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5CC)->f1;_LL505:
# 3031
 if(Cyc_Tcenv_region_outlives(te,_tmp5A6,_tmp5CD))return Cyc_Absyn_No_coercion;
goto _LL503;}else{_LL506: _LL507:
 goto _LL503;}_LL503:;}
# 3035
return Cyc_Absyn_Unknown_coercion;default: _LL4ED: _LL4EE:
 return Cyc_Absyn_Unknown_coercion;}_LL4E0:;};}
# 3041
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),t)){
struct Cyc_Absyn_Exp*_tmp5CE=Cyc_Absyn_copy_exp(e);
{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmpE3E;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpE3D;e->r=(void*)((_tmpE3D=_cycalloc(sizeof(*_tmpE3D)),((_tmpE3D[0]=((_tmpE3E.tag=13,((_tmpE3E.f1=t,((_tmpE3E.f2=_tmp5CE,((_tmpE3E.f3=0,((_tmpE3E.f4=c,_tmpE3E)))))))))),_tmpE3D))));}
e->topt=t;}}
# 3049
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp5D1=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp5D2=_tmp5D1;switch(*((int*)_tmp5D2)){case 6: _LL509: _LL50A:
 goto _LL50C;case 13: _LL50B: _LL50C:
 goto _LL50E;case 14: _LL50D: _LL50E:
 goto _LL510;case 19: _LL50F: _LL510:
 return 1;case 1: _LL511: _LL512:
 return Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ);default: _LL513: _LL514:
 return 0;}_LL508:;}
# 3060
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp5D3=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp5D4=_tmp5D3;if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D4)->tag == 7){_LL516: _LL517:
 return 1;}else{_LL518: _LL519:
 return 0;}_LL515:;};}
# 3069
int Cyc_Tcutil_is_function_type(void*t){
void*_tmp5D5=Cyc_Tcutil_compress(t);void*_tmp5D6=_tmp5D5;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5D6)->tag == 9){_LL51B: _LL51C:
 return 1;}else{_LL51D: _LL51E:
 return 0;}_LL51A:;}
# 3076
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
struct _tuple0 _tmpE3F;struct _tuple0 _tmp5D7=(_tmpE3F.f1=t1,((_tmpE3F.f2=t2,_tmpE3F)));struct _tuple0 _tmp5D8=_tmp5D7;int _tmp5D9;int _tmp5DA;if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D8.f1)->tag == 7){if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D8.f2)->tag == 7){_LL520: _tmp5D9=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D8.f1)->f1;_tmp5DA=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D8.f2)->f1;_LL521:
# 3079
 if(_tmp5D9 != 0  && _tmp5D9 != 1)return t1;else{
if(_tmp5DA != 0  && _tmp5DA != 1)return t2;else{
if(_tmp5D9 >= _tmp5DA)return t1;else{
return t2;}}}}else{_LL522: _LL523:
 return t1;}}else{if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D8.f2)->tag == 7){_LL524: _LL525:
 return t2;}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f1)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f1)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f1)->f2 == Cyc_Absyn_LongLong_sz){_LL526: _LL527:
 goto _LL529;}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL528;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL52E;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL530;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f1)->f2 == Cyc_Absyn_Int_sz)goto _LL532;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f2)->f2 == Cyc_Absyn_Int_sz)goto _LL534;else{goto _LL53A;}}}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL52C;else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f1)->f2){case Cyc_Absyn_Long_sz: goto _LL52E;case Cyc_Absyn_Int_sz: goto _LL532;default: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL538;else{goto _LL53A;}}}}}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f1)->f2){case Cyc_Absyn_Long_sz: _LL52E: _LL52F:
# 3089
 goto _LL531;case Cyc_Absyn_Int_sz: _LL532: _LL533:
# 3092
 goto _LL535;default: goto _LL53A;}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL528;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL52A;else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f2)->f2){case Cyc_Absyn_Long_sz: goto _LL530;case Cyc_Absyn_Int_sz: goto _LL534;default: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL536;else{goto _LL53A;}}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL52A;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL52C;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL536;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL538;else{goto _LL53A;}}}}}}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f1)->f2){case Cyc_Absyn_LongLong_sz: _LL52A: _LL52B:
# 3087
 goto _LL52D;case Cyc_Absyn_Long_sz: _LL536: _LL537:
# 3094
 goto _LL539;default: goto _LL53A;}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f2)->f1 == Cyc_Absyn_Unsigned)switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f2)->f2){case Cyc_Absyn_LongLong_sz: _LL528: _LL529:
# 3086
 return Cyc_Absyn_ulonglong_typ;case Cyc_Absyn_Long_sz: _LL530: _LL531:
# 3090
 return Cyc_Absyn_ulong_typ;case Cyc_Absyn_Int_sz: _LL534: _LL535:
# 3093
 return Cyc_Absyn_uint_typ;default: goto _LL53A;}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D8.f2)->f2){case Cyc_Absyn_LongLong_sz: _LL52C: _LL52D:
# 3088
 return Cyc_Absyn_slonglong_typ;case Cyc_Absyn_Long_sz: _LL538: _LL539:
# 3095
 return Cyc_Absyn_slong_typ;default: goto _LL53A;}}}else{_LL53A: _LL53B:
 return Cyc_Absyn_sint_typ;}}}}_LL51F:;}
# 3101
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_tmp5DC=e->r;void*_tmp5DD=_tmp5DC;if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp5DD)->tag == 3){if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp5DD)->f2 == 0){_LL53D: _LL53E:
{const char*_tmpE42;void*_tmpE41;(_tmpE41=0,Cyc_Tcutil_warn(e->loc,((_tmpE42="assignment in test",_tag_dyneither(_tmpE42,sizeof(char),19))),_tag_dyneither(_tmpE41,sizeof(void*),0)));}goto _LL53C;}else{goto _LL53F;}}else{_LL53F: _LL540:
 goto _LL53C;}_LL53C:;}
# 3115 "tcutil.cyc"
static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){
c1=Cyc_Absyn_compress_kb(c1);
c2=Cyc_Absyn_compress_kb(c2);
if(c1 == c2)return 1;{
struct _tuple0 _tmpE43;struct _tuple0 _tmp5E0=(_tmpE43.f1=c1,((_tmpE43.f2=c2,_tmpE43)));struct _tuple0 _tmp5E1=_tmp5E0;struct Cyc_Core_Opt**_tmp5E2;struct Cyc_Absyn_Kind*_tmp5E3;struct Cyc_Core_Opt**_tmp5E4;struct Cyc_Absyn_Kind*_tmp5E5;struct Cyc_Core_Opt**_tmp5E6;struct Cyc_Absyn_Kind*_tmp5E7;struct Cyc_Absyn_Kind*_tmp5E8;struct Cyc_Core_Opt**_tmp5E9;struct Cyc_Core_Opt**_tmp5EA;struct Cyc_Absyn_Kind*_tmp5EB;struct Cyc_Core_Opt**_tmp5EC;struct Cyc_Absyn_Kind*_tmp5ED;struct Cyc_Absyn_Kind*_tmp5EE;struct Cyc_Absyn_Kind*_tmp5EF;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5E1.f1)->tag == 0)switch(*((int*)_tmp5E1.f2)){case 0: _LL542: _tmp5EE=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5E1.f1)->f1;_tmp5EF=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5E1.f2)->f1;_LL543:
 return _tmp5EE == _tmp5EF;case 1: goto _LL544;default: _LL54A: _tmp5EB=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5E1.f1)->f1;_tmp5EC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5E1.f2)->f1;_tmp5ED=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5E1.f2)->f2;_LL54B:
# 3128
 if(Cyc_Tcutil_kind_leq(_tmp5EB,_tmp5ED)){
{struct Cyc_Core_Opt*_tmpE44;*_tmp5EC=((_tmpE44=_cycalloc(sizeof(*_tmpE44)),((_tmpE44->v=c1,_tmpE44))));}return 1;}else{
return 0;}}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5E1.f2)->tag == 1){_LL544: _tmp5EA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5E1.f2)->f1;_LL545:
# 3121
{struct Cyc_Core_Opt*_tmpE45;*_tmp5EA=((_tmpE45=_cycalloc(sizeof(*_tmpE45)),((_tmpE45->v=c1,_tmpE45))));}return 1;}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5E1.f1)->tag == 1){_LL546: _tmp5E9=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5E1.f1)->f1;_LL547:
{struct Cyc_Core_Opt*_tmpE46;*_tmp5E9=((_tmpE46=_cycalloc(sizeof(*_tmpE46)),((_tmpE46->v=c2,_tmpE46))));}return 1;}else{if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5E1.f2)->tag == 0){_LL548: _tmp5E6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5E1.f1)->f1;_tmp5E7=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5E1.f1)->f2;_tmp5E8=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5E1.f2)->f1;_LL549:
# 3124
 if(Cyc_Tcutil_kind_leq(_tmp5E8,_tmp5E7)){
{struct Cyc_Core_Opt*_tmpE47;*_tmp5E6=((_tmpE47=_cycalloc(sizeof(*_tmpE47)),((_tmpE47->v=c2,_tmpE47))));}return 1;}else{
return 0;}}else{_LL54C: _tmp5E2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5E1.f1)->f1;_tmp5E3=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5E1.f1)->f2;_tmp5E4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5E1.f2)->f1;_tmp5E5=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5E1.f2)->f2;_LL54D:
# 3132
 if(Cyc_Tcutil_kind_leq(_tmp5E3,_tmp5E5)){
{struct Cyc_Core_Opt*_tmpE48;*_tmp5E4=((_tmpE48=_cycalloc(sizeof(*_tmpE48)),((_tmpE48->v=c1,_tmpE48))));}return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp5E5,_tmp5E3)){
{struct Cyc_Core_Opt*_tmpE49;*_tmp5E2=((_tmpE49=_cycalloc(sizeof(*_tmpE49)),((_tmpE49->v=c2,_tmpE49))));}return 1;}else{
return 0;}}}}}}_LL541:;};}
# 3141
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 3146
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
const char*_tmpE4D;void*_tmpE4C[1];struct Cyc_Int_pa_PrintArg_struct _tmpE4B;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpE4B.tag=1,((_tmpE4B.f1=(unsigned long)i,((_tmpE4C[0]=& _tmpE4B,Cyc_aprintf(((_tmpE4D="#%d",_tag_dyneither(_tmpE4D,sizeof(char),4))),_tag_dyneither(_tmpE4C,sizeof(void*),1))))))));
struct _dyneither_ptr*_tmpE50;struct Cyc_Absyn_Tvar*_tmpE4F;return(_tmpE4F=_cycalloc(sizeof(*_tmpE4F)),((_tmpE4F->name=((_tmpE50=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmpE50[0]=(struct _dyneither_ptr)s,_tmpE50)))),((_tmpE4F->identity=- 1,((_tmpE4F->kind=k,_tmpE4F)))))));}
# 3153
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _dyneither_ptr _tmp5FC=*t->name;
return*((const char*)_check_dyneither_subscript(_tmp5FC,sizeof(char),0))== '#';}
# 3158
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
{const char*_tmpE54;void*_tmpE53[1];struct Cyc_String_pa_PrintArg_struct _tmpE52;(_tmpE52.tag=0,((_tmpE52.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name),((_tmpE53[0]=& _tmpE52,Cyc_printf(((_tmpE54="%s",_tag_dyneither(_tmpE54,sizeof(char),3))),_tag_dyneither(_tmpE53,sizeof(void*),1)))))));}
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
const char*_tmpE55;struct _dyneither_ptr _tmp600=Cyc_strconcat(((_tmpE55="`",_tag_dyneither(_tmpE55,sizeof(char),2))),(struct _dyneither_ptr)*t->name);
{char _tmpE58;char _tmpE57;struct _dyneither_ptr _tmpE56;(_tmpE56=_dyneither_ptr_plus(_tmp600,sizeof(char),1),((_tmpE57=*((char*)_check_dyneither_subscript(_tmpE56,sizeof(char),0)),((_tmpE58='t',((_get_dyneither_size(_tmpE56,sizeof(char))== 1  && (_tmpE57 == '\000'  && _tmpE58 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpE56.curr)=_tmpE58)))))));}{
struct _dyneither_ptr*_tmpE59;t->name=((_tmpE59=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmpE59[0]=(struct _dyneither_ptr)_tmp600,_tmpE59))));};};}
# 3167
static struct _tuple10*Cyc_Tcutil_fndecl2typ_f(struct _tuple10*x){
# 3169
struct _tuple10*_tmpE5A;return(_tmpE5A=_cycalloc(sizeof(*_tmpE5A)),((_tmpE5A->f1=(*x).f1,((_tmpE5A->f2=(*x).f2,((_tmpE5A->f3=(*x).f3,_tmpE5A)))))));}
# 3172
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_typ == 0){
# 3179
struct Cyc_List_List*_tmp607=0;
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
struct Cyc_List_List*_tmpE5B;_tmp607=((_tmpE5B=_cycalloc(sizeof(*_tmpE5B)),((_tmpE5B->hd=(void*)atts->hd,((_tmpE5B->tl=_tmp607,_tmpE5B))))));}}}{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpE61;struct Cyc_Absyn_FnInfo _tmpE60;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE5F;return(void*)((_tmpE5F=_cycalloc(sizeof(*_tmpE5F)),((_tmpE5F[0]=((_tmpE61.tag=9,((_tmpE61.f1=((_tmpE60.tvars=fd->tvs,((_tmpE60.effect=fd->effect,((_tmpE60.ret_tqual=fd->ret_tqual,((_tmpE60.ret_typ=fd->ret_type,((_tmpE60.args=
((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args),((_tmpE60.c_varargs=fd->c_varargs,((_tmpE60.cyc_varargs=fd->cyc_varargs,((_tmpE60.rgn_po=fd->rgn_po,((_tmpE60.attributes=_tmp607,((_tmpE60.requires_clause=fd->requires_clause,((_tmpE60.requires_relns=fd->requires_relns,((_tmpE60.ensures_clause=fd->ensures_clause,((_tmpE60.ensures_relns=fd->ensures_relns,_tmpE60)))))))))))))))))))))))))),_tmpE61)))),_tmpE5F))));};}
# 3192
return(void*)_check_null(fd->cached_typ);}
# 3198
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 3200
union Cyc_Relations_RelnOp _tmp60C=*rop;union Cyc_Relations_RelnOp _tmp60D=_tmp60C;struct Cyc_Absyn_Vardecl*_tmp60E;struct Cyc_Absyn_Vardecl*_tmp60F;switch((_tmp60D.RNumelts).tag){case 2: _LL54F: _tmp60F=(_tmp60D.RVar).val;_LL550: {
# 3202
struct _tuple2 _tmp610=*_tmp60F->name;union Cyc_Absyn_Nmspace _tmp612;struct _dyneither_ptr*_tmp613;struct _tuple2 _tmp611=_tmp610;_tmp612=_tmp611.f1;_tmp613=_tmp611.f2;
if(!((int)((_tmp612.Loc_n).tag == 4)))goto _LL54E;
{const char*_tmpE62;if(Cyc_strcmp((struct _dyneither_ptr)*_tmp613,((_tmpE62="return_value",_tag_dyneither(_tmpE62,sizeof(char),13))))== 0){
*rop=Cyc_Relations_RReturn();
goto _LL54E;}}{
# 3208
unsigned int c=0;
{struct Cyc_List_List*_tmp615=args;for(0;_tmp615 != 0;(_tmp615=_tmp615->tl,c ++)){
struct _tuple10*_tmp616=(struct _tuple10*)_tmp615->hd;struct _dyneither_ptr*_tmp618;struct _tuple10*_tmp617=_tmp616;_tmp618=_tmp617->f1;
if(_tmp618 != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp613,(struct _dyneither_ptr)*_tmp618)== 0){
*rop=Cyc_Relations_RParam(c);
break;}}}}
# 3218
goto _LL54E;};}case 3: _LL551: _tmp60E=(_tmp60D.RNumelts).val;_LL552: {
# 3220
struct _tuple2 _tmp619=*_tmp60E->name;union Cyc_Absyn_Nmspace _tmp61B;struct _dyneither_ptr*_tmp61C;struct _tuple2 _tmp61A=_tmp619;_tmp61B=_tmp61A.f1;_tmp61C=_tmp61A.f2;
if(!((int)((_tmp61B.Loc_n).tag == 4)))goto _LL54E;{
unsigned int c=0;
{struct Cyc_List_List*_tmp61D=args;for(0;_tmp61D != 0;(_tmp61D=_tmp61D->tl,c ++)){
struct _tuple10*_tmp61E=(struct _tuple10*)_tmp61D->hd;struct _dyneither_ptr*_tmp620;struct _tuple10*_tmp61F=_tmp61E;_tmp620=_tmp61F->f1;
if(_tmp620 != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp61C,(struct _dyneither_ptr)*_tmp620)== 0){
*rop=Cyc_Relations_RParamNumelts(c);
break;}}}}
# 3232
goto _LL54E;};}default: _LL553: _LL554:
 goto _LL54E;}_LL54E:;}
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
struct Cyc_Absyn_Tqual _tmp623;void*_tmp624;struct _tuple12*_tmp622=pr;_tmp623=_tmp622->f1;_tmp624=_tmp622->f2;
if(_tmp624 == t)return pr;else{
struct _tuple12*_tmpE63;return(_tmpE63=_cycalloc(sizeof(*_tmpE63)),((_tmpE63->f1=_tmp623,((_tmpE63->f2=t,_tmpE63)))));}}struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 3259
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
# 3261
struct _tuple26*_tmpE66;struct _tuple27*_tmpE65;return(_tmpE65=_region_malloc(rgn,sizeof(*_tmpE65)),((_tmpE65->f1=((_tmpE66=_region_malloc(rgn,sizeof(*_tmpE66)),((_tmpE66->f1=(*y).f1,((_tmpE66->f2=(*y).f2,_tmpE66)))))),((_tmpE65->f2=(*y).f3,_tmpE65)))));}
# 3263
static struct _tuple10*Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){
# 3265
struct _tuple10 _tmp628=*orig_arg;struct _dyneither_ptr*_tmp62A;struct Cyc_Absyn_Tqual _tmp62B;void*_tmp62C;struct _tuple10 _tmp629=_tmp628;_tmp62A=_tmp629.f1;_tmp62B=_tmp629.f2;_tmp62C=_tmp629.f3;
if(t == _tmp62C)return orig_arg;{
struct _tuple10*_tmpE67;return(_tmpE67=_cycalloc(sizeof(*_tmpE67)),((_tmpE67->f1=_tmp62A,((_tmpE67->f2=_tmp62B,((_tmpE67->f3=t,_tmpE67)))))));};}
# 3269
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){
return f->type;}
# 3272
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 3277
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 3279
struct Cyc_Absyn_Exp*_tmpE68;return(_tmpE68=_cycalloc(sizeof(*_tmpE68)),((_tmpE68->topt=old->topt,((_tmpE68->r=r,((_tmpE68->loc=old->loc,((_tmpE68->annot=old->annot,_tmpE68)))))))));}
# 3284
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp62F=e->r;void*_tmp630=_tmp62F;void*_tmp631;void*_tmp632;struct Cyc_List_List*_tmp633;struct Cyc_Absyn_Exp*_tmp634;void*_tmp635;void*_tmp636;struct Cyc_Absyn_Exp*_tmp637;int _tmp638;enum Cyc_Absyn_Coercion _tmp639;struct Cyc_Absyn_Exp*_tmp63A;struct Cyc_Absyn_Exp*_tmp63B;struct Cyc_Absyn_Exp*_tmp63C;struct Cyc_Absyn_Exp*_tmp63D;struct Cyc_Absyn_Exp*_tmp63E;struct Cyc_Absyn_Exp*_tmp63F;struct Cyc_Absyn_Exp*_tmp640;struct Cyc_Absyn_Exp*_tmp641;struct Cyc_Absyn_Exp*_tmp642;enum Cyc_Absyn_Primop _tmp643;struct Cyc_List_List*_tmp644;switch(*((int*)_tmp630)){case 0: _LL556: _LL557:
 goto _LL559;case 31: _LL558: _LL559:
 goto _LL55B;case 32: _LL55A: _LL55B:
 goto _LL55D;case 1: _LL55C: _LL55D:
 return e;case 2: _LL55E: _tmp643=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp630)->f1;_tmp644=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp630)->f2;_LL55F:
# 3292
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp644)== 1){
struct Cyc_Absyn_Exp*_tmp645=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp644))->hd;
struct Cyc_Absyn_Exp*_tmp646=Cyc_Tcutil_rsubsexp(r,inst,_tmp645);
if(_tmp646 == _tmp645)return e;{
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmpE6E;struct Cyc_Absyn_Exp*_tmpE6D[1];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpE6C;return Cyc_Tcutil_copye(e,(void*)((_tmpE6C=_cycalloc(sizeof(*_tmpE6C)),((_tmpE6C[0]=((_tmpE6E.tag=2,((_tmpE6E.f1=_tmp643,((_tmpE6E.f2=((_tmpE6D[0]=_tmp646,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpE6D,sizeof(struct Cyc_Absyn_Exp*),1)))),_tmpE6E)))))),_tmpE6C)))));};}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp644)== 2){
struct Cyc_Absyn_Exp*_tmp64A=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp644))->hd;
struct Cyc_Absyn_Exp*_tmp64B=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp644->tl))->hd;
struct Cyc_Absyn_Exp*_tmp64C=Cyc_Tcutil_rsubsexp(r,inst,_tmp64A);
struct Cyc_Absyn_Exp*_tmp64D=Cyc_Tcutil_rsubsexp(r,inst,_tmp64B);
if(_tmp64C == _tmp64A  && _tmp64D == _tmp64B)return e;{
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmpE74;struct Cyc_Absyn_Exp*_tmpE73[2];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpE72;return Cyc_Tcutil_copye(e,(void*)((_tmpE72=_cycalloc(sizeof(*_tmpE72)),((_tmpE72[0]=((_tmpE74.tag=2,((_tmpE74.f1=_tmp643,((_tmpE74.f2=((_tmpE73[1]=_tmp64D,((_tmpE73[0]=_tmp64C,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpE73,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmpE74)))))),_tmpE72)))));};}else{
const char*_tmpE77;void*_tmpE76;return(_tmpE76=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE77="primop does not have 1 or 2 args!",_tag_dyneither(_tmpE77,sizeof(char),34))),_tag_dyneither(_tmpE76,sizeof(void*),0)));}}case 5: _LL560: _tmp640=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp630)->f1;_tmp641=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp630)->f2;_tmp642=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp630)->f3;_LL561: {
# 3306
struct Cyc_Absyn_Exp*_tmp653=Cyc_Tcutil_rsubsexp(r,inst,_tmp640);
struct Cyc_Absyn_Exp*_tmp654=Cyc_Tcutil_rsubsexp(r,inst,_tmp641);
struct Cyc_Absyn_Exp*_tmp655=Cyc_Tcutil_rsubsexp(r,inst,_tmp642);
if((_tmp653 == _tmp640  && _tmp654 == _tmp641) && _tmp655 == _tmp642)return e;{
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmpE7A;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpE79;return Cyc_Tcutil_copye(e,(void*)((_tmpE79=_cycalloc(sizeof(*_tmpE79)),((_tmpE79[0]=((_tmpE7A.tag=5,((_tmpE7A.f1=_tmp653,((_tmpE7A.f2=_tmp654,((_tmpE7A.f3=_tmp655,_tmpE7A)))))))),_tmpE79)))));};}case 6: _LL562: _tmp63E=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp630)->f1;_tmp63F=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp630)->f2;_LL563: {
# 3312
struct Cyc_Absyn_Exp*_tmp658=Cyc_Tcutil_rsubsexp(r,inst,_tmp63E);
struct Cyc_Absyn_Exp*_tmp659=Cyc_Tcutil_rsubsexp(r,inst,_tmp63F);
if(_tmp658 == _tmp63E  && _tmp659 == _tmp63F)return e;{
struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmpE7D;struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpE7C;return Cyc_Tcutil_copye(e,(void*)((_tmpE7C=_cycalloc(sizeof(*_tmpE7C)),((_tmpE7C[0]=((_tmpE7D.tag=6,((_tmpE7D.f1=_tmp658,((_tmpE7D.f2=_tmp659,_tmpE7D)))))),_tmpE7C)))));};}case 7: _LL564: _tmp63C=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp630)->f1;_tmp63D=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp630)->f2;_LL565: {
# 3317
struct Cyc_Absyn_Exp*_tmp65C=Cyc_Tcutil_rsubsexp(r,inst,_tmp63C);
struct Cyc_Absyn_Exp*_tmp65D=Cyc_Tcutil_rsubsexp(r,inst,_tmp63D);
if(_tmp65C == _tmp63C  && _tmp65D == _tmp63D)return e;{
struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmpE80;struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmpE7F;return Cyc_Tcutil_copye(e,(void*)((_tmpE7F=_cycalloc(sizeof(*_tmpE7F)),((_tmpE7F[0]=((_tmpE80.tag=7,((_tmpE80.f1=_tmp65C,((_tmpE80.f2=_tmp65D,_tmpE80)))))),_tmpE7F)))));};}case 8: _LL566: _tmp63A=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp630)->f1;_tmp63B=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp630)->f2;_LL567: {
# 3322
struct Cyc_Absyn_Exp*_tmp660=Cyc_Tcutil_rsubsexp(r,inst,_tmp63A);
struct Cyc_Absyn_Exp*_tmp661=Cyc_Tcutil_rsubsexp(r,inst,_tmp63B);
if(_tmp660 == _tmp63A  && _tmp661 == _tmp63B)return e;{
struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmpE83;struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpE82;return Cyc_Tcutil_copye(e,(void*)((_tmpE82=_cycalloc(sizeof(*_tmpE82)),((_tmpE82[0]=((_tmpE83.tag=8,((_tmpE83.f1=_tmp660,((_tmpE83.f2=_tmp661,_tmpE83)))))),_tmpE82)))));};}case 13: _LL568: _tmp636=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp630)->f1;_tmp637=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp630)->f2;_tmp638=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp630)->f3;_tmp639=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp630)->f4;_LL569: {
# 3327
struct Cyc_Absyn_Exp*_tmp664=Cyc_Tcutil_rsubsexp(r,inst,_tmp637);
void*_tmp665=Cyc_Tcutil_rsubstitute(r,inst,_tmp636);
if(_tmp664 == _tmp637  && _tmp665 == _tmp636)return e;{
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmpE86;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpE85;return Cyc_Tcutil_copye(e,(void*)((_tmpE85=_cycalloc(sizeof(*_tmpE85)),((_tmpE85[0]=((_tmpE86.tag=13,((_tmpE86.f1=_tmp665,((_tmpE86.f2=_tmp664,((_tmpE86.f3=_tmp638,((_tmpE86.f4=_tmp639,_tmpE86)))))))))),_tmpE85)))));};}case 16: _LL56A: _tmp635=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp630)->f1;_LL56B: {
# 3332
void*_tmp668=Cyc_Tcutil_rsubstitute(r,inst,_tmp635);
if(_tmp668 == _tmp635)return e;{
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpE89;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpE88;return Cyc_Tcutil_copye(e,(void*)((_tmpE88=_cycalloc(sizeof(*_tmpE88)),((_tmpE88[0]=((_tmpE89.tag=16,((_tmpE89.f1=_tmp668,_tmpE89)))),_tmpE88)))));};}case 17: _LL56C: _tmp634=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp630)->f1;_LL56D: {
# 3336
struct Cyc_Absyn_Exp*_tmp66B=Cyc_Tcutil_rsubsexp(r,inst,_tmp634);
if(_tmp66B == _tmp634)return e;{
struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmpE8C;struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpE8B;return Cyc_Tcutil_copye(e,(void*)((_tmpE8B=_cycalloc(sizeof(*_tmpE8B)),((_tmpE8B[0]=((_tmpE8C.tag=17,((_tmpE8C.f1=_tmp66B,_tmpE8C)))),_tmpE8B)))));};}case 18: _LL56E: _tmp632=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp630)->f1;_tmp633=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp630)->f2;_LL56F: {
# 3340
void*_tmp66E=Cyc_Tcutil_rsubstitute(r,inst,_tmp632);
if(_tmp66E == _tmp632)return e;{
struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpE8F;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpE8E;return Cyc_Tcutil_copye(e,(void*)((_tmpE8E=_cycalloc(sizeof(*_tmpE8E)),((_tmpE8E[0]=((_tmpE8F.tag=18,((_tmpE8F.f1=_tmp66E,((_tmpE8F.f2=_tmp633,_tmpE8F)))))),_tmpE8E)))));};}case 38: _LL570: _tmp631=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp630)->f1;_LL571: {
# 3344
void*_tmp671=Cyc_Tcutil_rsubstitute(r,inst,_tmp631);
if(_tmp671 == _tmp631)return e;{
# 3347
void*_tmp672=Cyc_Tcutil_compress(_tmp671);void*_tmp673=_tmp672;struct Cyc_Absyn_Exp*_tmp674;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp673)->tag == 18){_LL575: _tmp674=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp673)->f1;_LL576:
 return _tmp674;}else{_LL577: _LL578: {
# 3350
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmpE92;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmpE91;return Cyc_Tcutil_copye(e,(void*)((_tmpE91=_cycalloc(sizeof(*_tmpE91)),((_tmpE91[0]=((_tmpE92.tag=38,((_tmpE92.f1=_tmp671,_tmpE92)))),_tmpE91)))));}}_LL574:;};}default: _LL572: _LL573: {
# 3353
const char*_tmpE95;void*_tmpE94;return(_tmpE94=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE95="non-type-level-expression in Tcutil::rsubsexp",_tag_dyneither(_tmpE95,sizeof(char),46))),_tag_dyneither(_tmpE94,sizeof(void*),0)));}}_LL555:;}
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
struct Cyc_Absyn_Aggrfield*_tmpE96;return(_tmpE96=_cycalloc(sizeof(*_tmpE96)),((_tmpE96->name=f->name,((_tmpE96->tq=f->tq,((_tmpE96->type=_tmp67B,((_tmpE96->width=f->width,((_tmpE96->attributes=f->attributes,((_tmpE96->requires_clause=_tmp67C,_tmpE96)))))))))))));}}
# 3377
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 3380
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp67E=(struct Cyc_Absyn_Aggrfield*)fs->hd;
struct Cyc_List_List*_tmp67F=fs->tl;
struct Cyc_Absyn_Aggrfield*_tmp680=Cyc_Tcutil_subst_aggrfield(r,inst,_tmp67E);
struct Cyc_List_List*_tmp681=Cyc_Tcutil_subst_aggrfields(r,inst,_tmp67F);
if(_tmp680 == _tmp67E  && _tmp681 == _tmp67F)return fs;{
# 3387
struct Cyc_List_List*_tmpE97;return(_tmpE97=_cycalloc(sizeof(*_tmpE97)),((_tmpE97->hd=_tmp680,((_tmpE97->tl=_tmp681,_tmpE97)))));};};}
# 3390
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 3393
struct _tuple1 _tmp683=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct Cyc_List_List*_tmp685;struct Cyc_List_List*_tmp686;struct _tuple1 _tmp684=_tmp683;_tmp685=_tmp684.f1;_tmp686=_tmp684.f2;{
struct Cyc_List_List*_tmp687=Cyc_Tcutil_substs(rgn,inst,_tmp685);
struct Cyc_List_List*_tmp688=Cyc_Tcutil_substs(rgn,inst,_tmp686);
if(_tmp687 == _tmp685  && _tmp688 == _tmp686)
return rgn_po;else{
# 3399
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp687,_tmp688);}};}
# 3402
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3405
void*_tmp689=Cyc_Tcutil_compress(t);void*_tmp68A=_tmp689;struct Cyc_List_List*_tmp68B;void*_tmp68C;void*_tmp68D;struct Cyc_Absyn_Exp*_tmp68E;struct Cyc_Absyn_Exp*_tmp68F;void*_tmp690;void*_tmp691;void*_tmp692;void*_tmp693;void*_tmp694;enum Cyc_Absyn_AggrKind _tmp695;struct Cyc_List_List*_tmp696;struct Cyc_List_List*_tmp697;struct Cyc_List_List*_tmp698;void*_tmp699;struct Cyc_Absyn_Tqual _tmp69A;void*_tmp69B;struct Cyc_List_List*_tmp69C;int _tmp69D;struct Cyc_Absyn_VarargInfo*_tmp69E;struct Cyc_List_List*_tmp69F;struct Cyc_List_List*_tmp6A0;struct Cyc_Absyn_Exp*_tmp6A1;struct Cyc_Absyn_Exp*_tmp6A2;void*_tmp6A3;struct Cyc_Absyn_Tqual _tmp6A4;void*_tmp6A5;union Cyc_Absyn_Constraint*_tmp6A6;union Cyc_Absyn_Constraint*_tmp6A7;union Cyc_Absyn_Constraint*_tmp6A8;void*_tmp6A9;struct Cyc_Absyn_Tqual _tmp6AA;struct Cyc_Absyn_Exp*_tmp6AB;union Cyc_Absyn_Constraint*_tmp6AC;unsigned int _tmp6AD;struct _tuple2*_tmp6AE;struct Cyc_List_List*_tmp6AF;struct Cyc_Absyn_Typedefdecl*_tmp6B0;void*_tmp6B1;union Cyc_Absyn_DatatypeFieldInfoU _tmp6B2;struct Cyc_List_List*_tmp6B3;union Cyc_Absyn_DatatypeInfoU _tmp6B4;struct Cyc_List_List*_tmp6B5;union Cyc_Absyn_AggrInfoU _tmp6B6;struct Cyc_List_List*_tmp6B7;struct Cyc_Absyn_Tvar*_tmp6B8;switch(*((int*)_tmp68A)){case 2: _LL57A: _tmp6B8=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp68A)->f1;_LL57B: {
# 3408
struct _handler_cons _tmp6B9;_push_handler(& _tmp6B9);{int _tmp6BB=0;if(setjmp(_tmp6B9.handler))_tmp6BB=1;if(!_tmp6BB){{void*_tmp6BC=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp6B8);_npop_handler(0);return _tmp6BC;};_pop_handler();}else{void*_tmp6BA=(void*)_exn_thrown;void*_tmp6BD=_tmp6BA;void*_tmp6BE;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp6BD)->tag == Cyc_Core_Not_found){_LL5B5: _LL5B6:
 return t;}else{_LL5B7: _tmp6BE=_tmp6BD;_LL5B8:(int)_rethrow(_tmp6BE);}_LL5B4:;}};}case 11: _LL57C: _tmp6B6=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp68A)->f1).aggr_info;_tmp6B7=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp68A)->f1).targs;_LL57D: {
# 3411
struct Cyc_List_List*_tmp6BF=Cyc_Tcutil_substs(rgn,inst,_tmp6B7);
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpE9D;struct Cyc_Absyn_AggrInfo _tmpE9C;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpE9B;return _tmp6BF == _tmp6B7?t:(void*)((_tmpE9B=_cycalloc(sizeof(*_tmpE9B)),((_tmpE9B[0]=((_tmpE9D.tag=11,((_tmpE9D.f1=((_tmpE9C.aggr_info=_tmp6B6,((_tmpE9C.targs=_tmp6BF,_tmpE9C)))),_tmpE9D)))),_tmpE9B))));}case 3: _LL57E: _tmp6B4=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp68A)->f1).datatype_info;_tmp6B5=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp68A)->f1).targs;_LL57F: {
# 3414
struct Cyc_List_List*_tmp6C3=Cyc_Tcutil_substs(rgn,inst,_tmp6B5);
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpEA3;struct Cyc_Absyn_DatatypeInfo _tmpEA2;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpEA1;return _tmp6C3 == _tmp6B5?t:(void*)(
(_tmpEA1=_cycalloc(sizeof(*_tmpEA1)),((_tmpEA1[0]=((_tmpEA3.tag=3,((_tmpEA3.f1=((_tmpEA2.datatype_info=_tmp6B4,((_tmpEA2.targs=_tmp6C3,_tmpEA2)))),_tmpEA3)))),_tmpEA1))));}case 4: _LL580: _tmp6B2=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp68A)->f1).field_info;_tmp6B3=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp68A)->f1).targs;_LL581: {
# 3418
struct Cyc_List_List*_tmp6C7=Cyc_Tcutil_substs(rgn,inst,_tmp6B3);
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpEA9;struct Cyc_Absyn_DatatypeFieldInfo _tmpEA8;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpEA7;return _tmp6C7 == _tmp6B3?t:(void*)(
(_tmpEA7=_cycalloc(sizeof(*_tmpEA7)),((_tmpEA7[0]=((_tmpEA9.tag=4,((_tmpEA9.f1=((_tmpEA8.field_info=_tmp6B2,((_tmpEA8.targs=_tmp6C7,_tmpEA8)))),_tmpEA9)))),_tmpEA7))));}case 17: _LL582: _tmp6AE=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp68A)->f1;_tmp6AF=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp68A)->f2;_tmp6B0=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp68A)->f3;_tmp6B1=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp68A)->f4;_LL583: {
# 3422
struct Cyc_List_List*_tmp6CB=Cyc_Tcutil_substs(rgn,inst,_tmp6AF);
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpEAC;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpEAB;return _tmp6CB == _tmp6AF?t:(void*)((_tmpEAB=_cycalloc(sizeof(*_tmpEAB)),((_tmpEAB[0]=((_tmpEAC.tag=17,((_tmpEAC.f1=_tmp6AE,((_tmpEAC.f2=_tmp6CB,((_tmpEAC.f3=_tmp6B0,((_tmpEAC.f4=_tmp6B1,_tmpEAC)))))))))),_tmpEAB))));}case 8: _LL584: _tmp6A9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp68A)->f1).elt_type;_tmp6AA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp68A)->f1).tq;_tmp6AB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp68A)->f1).num_elts;_tmp6AC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp68A)->f1).zero_term;_tmp6AD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp68A)->f1).zt_loc;_LL585: {
# 3425
void*_tmp6CE=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6A9);
struct Cyc_Absyn_Exp*_tmp6CF=_tmp6AB == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,_tmp6AB);
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpEB2;struct Cyc_Absyn_ArrayInfo _tmpEB1;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpEB0;return _tmp6CE == _tmp6A9  && _tmp6CF == _tmp6AB?t:(void*)(
(_tmpEB0=_cycalloc(sizeof(*_tmpEB0)),((_tmpEB0[0]=((_tmpEB2.tag=8,((_tmpEB2.f1=((_tmpEB1.elt_type=_tmp6CE,((_tmpEB1.tq=_tmp6AA,((_tmpEB1.num_elts=_tmp6CF,((_tmpEB1.zero_term=_tmp6AC,((_tmpEB1.zt_loc=_tmp6AD,_tmpEB1)))))))))),_tmpEB2)))),_tmpEB0))));}case 5: _LL586: _tmp6A3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp68A)->f1).elt_typ;_tmp6A4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp68A)->f1).elt_tq;_tmp6A5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp68A)->f1).ptr_atts).rgn;_tmp6A6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp68A)->f1).ptr_atts).nullable;_tmp6A7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp68A)->f1).ptr_atts).bounds;_tmp6A8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp68A)->f1).ptr_atts).zero_term;_LL587: {
# 3430
void*_tmp6D3=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6A3);
void*_tmp6D4=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6A5);
union Cyc_Absyn_Constraint*_tmp6D5=_tmp6A7;
{void*_tmp6D6=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp6A7);void*_tmp6D7=_tmp6D6;struct Cyc_Absyn_Exp*_tmp6D8;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6D7)->tag == 1){_LL5BA: _tmp6D8=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6D7)->f1;_LL5BB: {
# 3435
struct Cyc_Absyn_Exp*_tmp6D9=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp6D8);
if(_tmp6D9 != _tmp6D8){
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpEB5;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpEB4;_tmp6D5=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmpEB4=_cycalloc(sizeof(*_tmpEB4)),((_tmpEB4[0]=((_tmpEB5.tag=1,((_tmpEB5.f1=_tmp6D9,_tmpEB5)))),_tmpEB4)))));}
goto _LL5B9;}}else{_LL5BC: _LL5BD:
 goto _LL5B9;}_LL5B9:;}
# 3441
if((_tmp6D3 == _tmp6A3  && _tmp6D4 == _tmp6A5) && _tmp6D5 == _tmp6A7)
return t;{
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpEBB;struct Cyc_Absyn_PtrInfo _tmpEBA;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpEB9;return(void*)((_tmpEB9=_cycalloc(sizeof(*_tmpEB9)),((_tmpEB9[0]=((_tmpEBB.tag=5,((_tmpEBB.f1=((_tmpEBA.elt_typ=_tmp6D3,((_tmpEBA.elt_tq=_tmp6A4,((_tmpEBA.ptr_atts=(((_tmpEBA.ptr_atts).rgn=_tmp6D4,(((_tmpEBA.ptr_atts).nullable=_tmp6A6,(((_tmpEBA.ptr_atts).bounds=_tmp6D5,(((_tmpEBA.ptr_atts).zero_term=_tmp6A8,(((_tmpEBA.ptr_atts).ptrloc=0,_tmpEBA.ptr_atts)))))))))),_tmpEBA)))))),_tmpEBB)))),_tmpEB9))));};}case 9: _LL588: _tmp698=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68A)->f1).tvars;_tmp699=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68A)->f1).effect;_tmp69A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68A)->f1).ret_tqual;_tmp69B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68A)->f1).ret_typ;_tmp69C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68A)->f1).args;_tmp69D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68A)->f1).c_varargs;_tmp69E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68A)->f1).cyc_varargs;_tmp69F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68A)->f1).rgn_po;_tmp6A0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68A)->f1).attributes;_tmp6A1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68A)->f1).requires_clause;_tmp6A2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68A)->f1).ensures_clause;_LL589:
# 3447
{struct Cyc_List_List*_tmp6DF=_tmp698;for(0;_tmp6DF != 0;_tmp6DF=_tmp6DF->tl){
struct _tuple16*_tmpEC5;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpEC4;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpEC3;struct Cyc_List_List*_tmpEC2;inst=((_tmpEC2=_region_malloc(rgn,sizeof(*_tmpEC2)),((_tmpEC2->hd=((_tmpEC5=_region_malloc(rgn,sizeof(*_tmpEC5)),((_tmpEC5->f1=(struct Cyc_Absyn_Tvar*)_tmp6DF->hd,((_tmpEC5->f2=(void*)((_tmpEC3=_cycalloc(sizeof(*_tmpEC3)),((_tmpEC3[0]=((_tmpEC4.tag=2,((_tmpEC4.f1=(struct Cyc_Absyn_Tvar*)_tmp6DF->hd,_tmpEC4)))),_tmpEC3)))),_tmpEC5)))))),((_tmpEC2->tl=inst,_tmpEC2))))));}}{
struct _tuple1 _tmp6E4=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp69C));
# 3449
struct Cyc_List_List*_tmp6E6;struct Cyc_List_List*_tmp6E7;struct _tuple1 _tmp6E5=_tmp6E4;_tmp6E6=_tmp6E5.f1;_tmp6E7=_tmp6E5.f2;{
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
struct Cyc_Absyn_VarargInfo _tmp6EA=*_tmp69E;struct _dyneither_ptr*_tmp6EC;struct Cyc_Absyn_Tqual _tmp6ED;void*_tmp6EE;int _tmp6EF;struct Cyc_Absyn_VarargInfo _tmp6EB=_tmp6EA;_tmp6EC=_tmp6EB.name;_tmp6ED=_tmp6EB.tq;_tmp6EE=_tmp6EB.type;_tmp6EF=_tmp6EB.inject;{
void*_tmp6F0=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6EE);
if(_tmp6F0 == _tmp6EE)cyc_varargs2=_tmp69E;else{
# 3473
struct Cyc_Absyn_VarargInfo*_tmpEC6;cyc_varargs2=((_tmpEC6=_cycalloc(sizeof(*_tmpEC6)),((_tmpEC6->name=_tmp6EC,((_tmpEC6->tq=_tmp6ED,((_tmpEC6->type=_tmp6F0,((_tmpEC6->inject=_tmp6EF,_tmpEC6))))))))));}};}{
# 3475
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp69F);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp6A1);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp6A2);
struct Cyc_List_List*_tmp6F2=Cyc_Tcutil_extract_relns(_tmp6E8,req2);
struct Cyc_List_List*_tmp6F3=Cyc_Tcutil_extract_relns(_tmp6E8,ens2);
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpECC;struct Cyc_Absyn_FnInfo _tmpECB;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpECA;return(void*)(
(_tmpECA=_cycalloc(sizeof(*_tmpECA)),((_tmpECA[0]=((_tmpECC.tag=9,((_tmpECC.f1=((_tmpECB.tvars=_tmp698,((_tmpECB.effect=eff2,((_tmpECB.ret_tqual=_tmp69A,((_tmpECB.ret_typ=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp69B),((_tmpECB.args=_tmp6E8,((_tmpECB.c_varargs=_tmp69D,((_tmpECB.cyc_varargs=cyc_varargs2,((_tmpECB.rgn_po=rgn_po2,((_tmpECB.attributes=_tmp6A0,((_tmpECB.requires_clause=req2,((_tmpECB.requires_relns=_tmp6F2,((_tmpECB.ensures_clause=ens2,((_tmpECB.ensures_relns=_tmp6F3,_tmpECB)))))))))))))))))))))))))),_tmpECC)))),_tmpECA))));};};};};};case 10: _LL58A: _tmp697=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp68A)->f1;_LL58B: {
# 3485
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp6F7=_tmp697;for(0;_tmp6F7 != 0;_tmp6F7=_tmp6F7->tl){
void*_tmp6F8=(*((struct _tuple12*)_tmp6F7->hd)).f2;
void*_tmp6F9=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6F8);
if(_tmp6F8 != _tmp6F9)
change=1;{
# 3493
struct Cyc_List_List*_tmpECD;ts2=((_tmpECD=_region_malloc(rgn,sizeof(*_tmpECD)),((_tmpECD->hd=_tmp6F9,((_tmpECD->tl=ts2,_tmpECD))))));};}}
# 3495
if(!change)
return t;{
struct Cyc_List_List*_tmp6FB=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp697,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpED0;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpECF;return(void*)((_tmpECF=_cycalloc(sizeof(*_tmpECF)),((_tmpECF[0]=((_tmpED0.tag=10,((_tmpED0.f1=_tmp6FB,_tmpED0)))),_tmpECF))));};}case 12: _LL58C: _tmp695=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp68A)->f1;_tmp696=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp68A)->f2;_LL58D: {
# 3500
struct Cyc_List_List*_tmp6FE=Cyc_Tcutil_subst_aggrfields(rgn,inst,_tmp696);
if(_tmp696 == _tmp6FE)return t;{
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpED3;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpED2;return(void*)((_tmpED2=_cycalloc(sizeof(*_tmpED2)),((_tmpED2[0]=((_tmpED3.tag=12,((_tmpED3.f1=_tmp695,((_tmpED3.f2=_tmp6FE,_tmpED3)))))),_tmpED2))));};}case 1: _LL58E: _tmp694=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp68A)->f2;_LL58F:
# 3504
 if(_tmp694 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp694);else{
return t;}case 15: _LL590: _tmp693=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp68A)->f1;_LL591: {
# 3507
void*_tmp701=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp693);
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpED6;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpED5;return _tmp701 == _tmp693?t:(void*)((_tmpED5=_cycalloc(sizeof(*_tmpED5)),((_tmpED5[0]=((_tmpED6.tag=15,((_tmpED6.f1=_tmp701,_tmpED6)))),_tmpED5))));}case 16: _LL592: _tmp691=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp68A)->f1;_tmp692=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp68A)->f2;_LL593: {
# 3510
void*_tmp704=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp691);
void*_tmp705=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp692);
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpED9;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpED8;return _tmp704 == _tmp691  && _tmp705 == _tmp692?t:(void*)((_tmpED8=_cycalloc(sizeof(*_tmpED8)),((_tmpED8[0]=((_tmpED9.tag=16,((_tmpED9.f1=_tmp704,((_tmpED9.f2=_tmp705,_tmpED9)))))),_tmpED8))));}case 19: _LL594: _tmp690=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp68A)->f1;_LL595: {
# 3514
void*_tmp708=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp690);
struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpEDC;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpEDB;return _tmp708 == _tmp690?t:(void*)((_tmpEDB=_cycalloc(sizeof(*_tmpEDB)),((_tmpEDB[0]=((_tmpEDC.tag=19,((_tmpEDC.f1=_tmp708,_tmpEDC)))),_tmpEDB))));}case 18: _LL596: _tmp68F=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp68A)->f1;_LL597: {
# 3517
struct Cyc_Absyn_Exp*_tmp70B=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp68F);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpEDF;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpEDE;return _tmp70B == _tmp68F?t:(void*)((_tmpEDE=_cycalloc(sizeof(*_tmpEDE)),((_tmpEDE[0]=((_tmpEDF.tag=18,((_tmpEDF.f1=_tmp70B,_tmpEDF)))),_tmpEDE))));}case 27: _LL598: _tmp68E=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp68A)->f1;_LL599: {
# 3520
struct Cyc_Absyn_Exp*_tmp70E=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp68E);
struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmpEE2;struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmpEE1;return _tmp70E == _tmp68E?t:(void*)((_tmpEE1=_cycalloc(sizeof(*_tmpEE1)),((_tmpEE1[0]=((_tmpEE2.tag=27,((_tmpEE2.f1=_tmp70E,_tmpEE2)))),_tmpEE1))));}case 13: _LL59A: _LL59B:
 goto _LL59D;case 14: _LL59C: _LL59D:
 goto _LL59F;case 0: _LL59E: _LL59F:
 goto _LL5A1;case 6: _LL5A0: _LL5A1:
 goto _LL5A3;case 7: _LL5A2: _LL5A3:
 goto _LL5A5;case 22: _LL5A4: _LL5A5:
 goto _LL5A7;case 21: _LL5A6: _LL5A7:
 goto _LL5A9;case 28: _LL5A8: _LL5A9:
 goto _LL5AB;case 20: _LL5AA: _LL5AB:
 return t;case 25: _LL5AC: _tmp68D=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp68A)->f1;_LL5AD: {
# 3532
void*_tmp711=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp68D);
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpEE5;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpEE4;return _tmp711 == _tmp68D?t:(void*)((_tmpEE4=_cycalloc(sizeof(*_tmpEE4)),((_tmpEE4[0]=((_tmpEE5.tag=25,((_tmpEE5.f1=_tmp711,_tmpEE5)))),_tmpEE4))));}case 23: _LL5AE: _tmp68C=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp68A)->f1;_LL5AF: {
# 3535
void*_tmp714=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp68C);
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEE8;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEE7;return _tmp714 == _tmp68C?t:(void*)((_tmpEE7=_cycalloc(sizeof(*_tmpEE7)),((_tmpEE7[0]=((_tmpEE8.tag=23,((_tmpEE8.f1=_tmp714,_tmpEE8)))),_tmpEE7))));}case 24: _LL5B0: _tmp68B=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp68A)->f1;_LL5B1: {
# 3538
struct Cyc_List_List*_tmp717=Cyc_Tcutil_substs(rgn,inst,_tmp68B);
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEEB;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEEA;return _tmp717 == _tmp68B?t:(void*)((_tmpEEA=_cycalloc(sizeof(*_tmpEEA)),((_tmpEEA[0]=((_tmpEEB.tag=24,((_tmpEEB.f1=_tmp717,_tmpEEB)))),_tmpEEA))));}default: _LL5B2: _LL5B3: {
const char*_tmpEEE;void*_tmpEED;(_tmpEED=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpEEE="found typedecltype in rsubs",_tag_dyneither(_tmpEEE,sizeof(char),28))),_tag_dyneither(_tmpEED,sizeof(void*),0)));}}_LL579:;}
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
return ts;{
struct Cyc_List_List*_tmpEEF;return(_tmpEEF=_cycalloc(sizeof(*_tmpEEF)),((_tmpEEF->hd=_tmp71E,((_tmpEEF->tl=_tmp71F,_tmpEEF)))));};};}
# 3558
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 3565
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp721=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
struct Cyc_Core_Opt*_tmpEF2;struct _tuple16*_tmpEF1;return(_tmpEF1=_cycalloc(sizeof(*_tmpEF1)),((_tmpEF1->f1=tv,((_tmpEF1->f2=Cyc_Absyn_new_evar(_tmp721,((_tmpEF2=_cycalloc(sizeof(*_tmpEF2)),((_tmpEF2->v=s,_tmpEF2))))),_tmpEF1)))));}
# 3570
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*env,struct Cyc_Absyn_Tvar*tv){
# 3572
struct Cyc_List_List*_tmp725;struct _RegionHandle*_tmp726;struct _tuple17*_tmp724=env;_tmp725=_tmp724->f1;_tmp726=_tmp724->f2;{
struct Cyc_Core_Opt*_tmp727=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
struct Cyc_Core_Opt*_tmpEF5;struct _tuple16*_tmpEF4;return(_tmpEF4=_region_malloc(_tmp726,sizeof(*_tmpEF4)),((_tmpEF4->f1=tv,((_tmpEF4->f2=Cyc_Absyn_new_evar(_tmp727,((_tmpEF5=_cycalloc(sizeof(*_tmpEF5)),((_tmpEF5->v=_tmp725,_tmpEF5))))),_tmpEF4)))));};}
# 3582
static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(unsigned int loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3586
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2)){
const char*_tmpEFB;void*_tmpEFA[3];struct Cyc_String_pa_PrintArg_struct _tmpEF9;struct Cyc_String_pa_PrintArg_struct _tmpEF8;struct Cyc_String_pa_PrintArg_struct _tmpEF7;(_tmpEF7.tag=0,((_tmpEF7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(k2)),((_tmpEF8.tag=0,((_tmpEF8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1)),((_tmpEF9.tag=0,((_tmpEF9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmpEFA[0]=& _tmpEF9,((_tmpEFA[1]=& _tmpEF8,((_tmpEFA[2]=& _tmpEF7,Cyc_Tcutil_terr(loc,((_tmpEFB="type variable %s is used with inconsistent kinds %s and %s",_tag_dyneither(_tmpEFB,sizeof(char),59))),_tag_dyneither(_tmpEFA,sizeof(void*),3)))))))))))))))))));}
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){
const char*_tmpEFE;void*_tmpEFD;(_tmpEFD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpEFE="same type variable has different identity!",_tag_dyneither(_tmpEFE,sizeof(char),43))),_tag_dyneither(_tmpEFD,sizeof(void*),0)));}}
return tvs;}}}
# 3600
tv->identity=Cyc_Tcutil_new_tvar_id();{
struct Cyc_List_List*_tmpEFF;return(_tmpEFF=_cycalloc(sizeof(*_tmpEFF)),((_tmpEFF->hd=tv,((_tmpEFF->tl=tvs,_tmpEFF)))));};}
# 3606
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3608
if(tv->identity == - 1){
const char*_tmpF02;void*_tmpF01;(_tmpF01=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF02="fast_add_free_tvar: bad identity in tv",_tag_dyneither(_tmpF02,sizeof(char),39))),_tag_dyneither(_tmpF01,sizeof(void*),0)));}
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3612
struct Cyc_Absyn_Tvar*_tmp734=(struct Cyc_Absyn_Tvar*)tvs2->hd;
if(_tmp734->identity == - 1){
const char*_tmpF05;void*_tmpF04;(_tmpF04=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF05="fast_add_free_tvar: bad identity in tvs2",_tag_dyneither(_tmpF05,sizeof(char),41))),_tag_dyneither(_tmpF04,sizeof(void*),0)));}
if(_tmp734->identity == tv->identity)
return tvs;}}{
# 3619
struct Cyc_List_List*_tmpF06;return(_tmpF06=_cycalloc(sizeof(*_tmpF06)),((_tmpF06->hd=tv,((_tmpF06->tl=tvs,_tmpF06)))));};}struct _tuple28{struct Cyc_Absyn_Tvar*f1;int f2;};
# 3625
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 3630
if(tv->identity == - 1){
const char*_tmpF09;void*_tmpF08;(_tmpF08=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF09="fast_add_free_tvar_bool: bad identity in tv",_tag_dyneither(_tmpF09,sizeof(char),44))),_tag_dyneither(_tmpF08,sizeof(void*),0)));}
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3634
struct _tuple28*_tmp73A=(struct _tuple28*)tvs2->hd;struct Cyc_Absyn_Tvar*_tmp73C;int*_tmp73D;struct _tuple28*_tmp73B=_tmp73A;_tmp73C=_tmp73B->f1;_tmp73D=(int*)& _tmp73B->f2;
if(_tmp73C->identity == - 1){
const char*_tmpF0C;void*_tmpF0B;(_tmpF0B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF0C="fast_add_free_tvar_bool: bad identity in tvs2",_tag_dyneither(_tmpF0C,sizeof(char),46))),_tag_dyneither(_tmpF0B,sizeof(void*),0)));}
if(_tmp73C->identity == tv->identity){
*_tmp73D=*_tmp73D  || b;
return tvs;}}}{
# 3642
struct _tuple28*_tmpF0F;struct Cyc_List_List*_tmpF0E;return(_tmpF0E=_region_malloc(r,sizeof(*_tmpF0E)),((_tmpF0E->hd=((_tmpF0F=_region_malloc(r,sizeof(*_tmpF0F)),((_tmpF0F->f1=tv,((_tmpF0F->f2=b,_tmpF0F)))))),((_tmpF0E->tl=tvs,_tmpF0E)))));};}
# 3646
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3648
if(tv->identity == - 1){
const char*_tmpF13;void*_tmpF12[1];struct Cyc_String_pa_PrintArg_struct _tmpF11;(_tmpF11.tag=0,((_tmpF11.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(tv)),((_tmpF12[0]=& _tmpF11,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF13="bound tvar id for %s is NULL",_tag_dyneither(_tmpF13,sizeof(char),29))),_tag_dyneither(_tmpF12,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmpF14;return(_tmpF14=_cycalloc(sizeof(*_tmpF14)),((_tmpF14->hd=tv,((_tmpF14->tl=tvs,_tmpF14)))));};}struct _tuple29{void*f1;int f2;};
# 3657
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 3660
void*_tmp746=Cyc_Tcutil_compress(e);void*_tmp747=_tmp746;int _tmp748;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp747)->tag == 1){_LL5BF: _tmp748=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp747)->f3;_LL5C0:
# 3662
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple29*_tmp749=(struct _tuple29*)es2->hd;void*_tmp74B;int*_tmp74C;struct _tuple29*_tmp74A=_tmp749;_tmp74B=_tmp74A->f1;_tmp74C=(int*)& _tmp74A->f2;{
void*_tmp74D=Cyc_Tcutil_compress(_tmp74B);void*_tmp74E=_tmp74D;int _tmp74F;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp74E)->tag == 1){_LL5C4: _tmp74F=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp74E)->f3;_LL5C5:
# 3666
 if(_tmp748 == _tmp74F){
if(b != *_tmp74C)*_tmp74C=1;
return es;}
# 3670
goto _LL5C3;}else{_LL5C6: _LL5C7:
 goto _LL5C3;}_LL5C3:;};}}{
# 3674
struct _tuple29*_tmpF17;struct Cyc_List_List*_tmpF16;return(_tmpF16=_region_malloc(r,sizeof(*_tmpF16)),((_tmpF16->hd=((_tmpF17=_region_malloc(r,sizeof(*_tmpF17)),((_tmpF17->f1=e,((_tmpF17->f2=b,_tmpF17)))))),((_tmpF16->tl=es,_tmpF16)))));};}else{_LL5C1: _LL5C2:
 return es;}_LL5BE:;}
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
if(!present){struct Cyc_List_List*_tmpF18;r=((_tmpF18=_region_malloc(rgn,sizeof(*_tmpF18)),((_tmpF18->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmpF18->tl=r,_tmpF18))))));}}
# 3693
r=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);
return r;}
# 3698
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3702
struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp753=*((struct _tuple28*)tvs->hd);struct Cyc_Absyn_Tvar*_tmp755;int _tmp756;struct _tuple28 _tmp754=_tmp753;_tmp755=_tmp754.f1;_tmp756=_tmp754.f2;{
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp755->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3712
if(!present){struct Cyc_List_List*_tmpF19;res=((_tmpF19=_region_malloc(r,sizeof(*_tmpF19)),((_tmpF19->hd=(struct _tuple28*)tvs->hd,((_tmpF19->tl=res,_tmpF19))))));}};}
# 3714
res=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);
return res;}
# 3718
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){
# 3720
if(width != 0){
unsigned int w=0;
if(!Cyc_Tcutil_is_const_exp(width)){
const char*_tmpF1D;void*_tmpF1C[1];struct Cyc_String_pa_PrintArg_struct _tmpF1B;(_tmpF1B.tag=0,((_tmpF1B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmpF1C[0]=& _tmpF1B,Cyc_Tcutil_terr(loc,((_tmpF1D="bitfield %s does not have constant width",_tag_dyneither(_tmpF1D,sizeof(char),41))),_tag_dyneither(_tmpF1C,sizeof(void*),1)))))));}else{
# 3725
struct _tuple14 _tmp75B=Cyc_Evexp_eval_const_uint_exp(width);unsigned int _tmp75D;int _tmp75E;struct _tuple14 _tmp75C=_tmp75B;_tmp75D=_tmp75C.f1;_tmp75E=_tmp75C.f2;
if(!_tmp75E){
const char*_tmpF20;void*_tmpF1F;(_tmpF1F=0,Cyc_Tcutil_terr(loc,((_tmpF20="bitfield width cannot use sizeof or offsetof",_tag_dyneither(_tmpF20,sizeof(char),45))),_tag_dyneither(_tmpF1F,sizeof(void*),0)));}
w=_tmp75D;}{
# 3730
void*_tmp761=Cyc_Tcutil_compress(field_typ);void*_tmp762=_tmp761;enum Cyc_Absyn_Size_of _tmp763;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp762)->tag == 6){_LL5C9: _tmp763=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp762)->f2;_LL5CA:
# 3733
{enum Cyc_Absyn_Size_of _tmp764=_tmp763;switch(_tmp764){case Cyc_Absyn_Char_sz: _LL5CE: _LL5CF:
 if(w > 8){const char*_tmpF23;void*_tmpF22;(_tmpF22=0,Cyc_Tcutil_terr(loc,((_tmpF23="bitfield larger than type",_tag_dyneither(_tmpF23,sizeof(char),26))),_tag_dyneither(_tmpF22,sizeof(void*),0)));}goto _LL5CD;case Cyc_Absyn_Short_sz: _LL5D0: _LL5D1:
 if(w > 16){const char*_tmpF26;void*_tmpF25;(_tmpF25=0,Cyc_Tcutil_terr(loc,((_tmpF26="bitfield larger than type",_tag_dyneither(_tmpF26,sizeof(char),26))),_tag_dyneither(_tmpF25,sizeof(void*),0)));}goto _LL5CD;case Cyc_Absyn_Long_sz: _LL5D2: _LL5D3:
 goto _LL5D5;case Cyc_Absyn_Int_sz: _LL5D4: _LL5D5:
# 3738
 if(w > 32){const char*_tmpF29;void*_tmpF28;(_tmpF28=0,Cyc_Tcutil_terr(loc,((_tmpF29="bitfield larger than type",_tag_dyneither(_tmpF29,sizeof(char),26))),_tag_dyneither(_tmpF28,sizeof(void*),0)));}goto _LL5CD;default: _LL5D6: _LL5D7:
# 3740
 if(w > 64){const char*_tmpF2C;void*_tmpF2B;(_tmpF2B=0,Cyc_Tcutil_terr(loc,((_tmpF2C="bitfield larger than type",_tag_dyneither(_tmpF2C,sizeof(char),26))),_tag_dyneither(_tmpF2B,sizeof(void*),0)));}goto _LL5CD;}_LL5CD:;}
# 3742
goto _LL5C8;}else{_LL5CB: _LL5CC:
# 3744
{const char*_tmpF31;void*_tmpF30[2];struct Cyc_String_pa_PrintArg_struct _tmpF2F;struct Cyc_String_pa_PrintArg_struct _tmpF2E;(_tmpF2E.tag=0,((_tmpF2E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(field_typ)),((_tmpF2F.tag=0,((_tmpF2F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmpF30[0]=& _tmpF2F,((_tmpF30[1]=& _tmpF2E,Cyc_Tcutil_terr(loc,((_tmpF31="bitfield %s must have integral type but has type %s",_tag_dyneither(_tmpF31,sizeof(char),52))),_tag_dyneither(_tmpF30,sizeof(void*),2)))))))))))));}
goto _LL5C8;}_LL5C8:;};}}
# 3751
static void Cyc_Tcutil_check_field_atts(unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp771=(void*)atts->hd;void*_tmp772=_tmp771;switch(*((int*)_tmp772)){case 7: _LL5D9: _LL5DA:
 continue;case 6: _LL5DB: _LL5DC:
 continue;default: _LL5DD: _LL5DE: {
const char*_tmpF36;void*_tmpF35[2];struct Cyc_String_pa_PrintArg_struct _tmpF34;struct Cyc_String_pa_PrintArg_struct _tmpF33;(_tmpF33.tag=0,((_tmpF33.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmpF34.tag=0,((_tmpF34.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmpF35[0]=& _tmpF34,((_tmpF35[1]=& _tmpF33,Cyc_Tcutil_terr(loc,((_tmpF36="bad attribute %s on member %s",_tag_dyneither(_tmpF36,sizeof(char),30))),_tag_dyneither(_tmpF35,sizeof(void*),2)))))))))))));}}_LL5D8:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
# 3775
typedef struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_cvtenv_t;
# 3779
int Cyc_Tcutil_extract_const_from_typedef(unsigned int loc,int declared_const,void*t){
void*_tmp777=t;struct Cyc_Absyn_Typedefdecl*_tmp778;void*_tmp779;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp777)->tag == 17){_LL5E0: _tmp778=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp777)->f3;_tmp779=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp777)->f4;_LL5E1:
# 3782
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp778))->tq).real_const  || (_tmp778->tq).print_const){
if(declared_const){const char*_tmpF39;void*_tmpF38;(_tmpF38=0,Cyc_Tcutil_warn(loc,((_tmpF39="extra const",_tag_dyneither(_tmpF39,sizeof(char),12))),_tag_dyneither(_tmpF38,sizeof(void*),0)));}
return 1;}
# 3787
if((unsigned int)_tmp779)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp779);else{
return declared_const;}}else{_LL5E2: _LL5E3:
 return declared_const;}_LL5DF:;}
# 3794
static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td != 0){
if(td->defn != 0){
void*_tmp77C=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_tmp77D=_tmp77C;void*_tmp77E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp77D)->tag == 5){_LL5E5: _tmp77E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp77D)->f1).ptr_atts).rgn;_LL5E6:
# 3799
{void*_tmp77F=Cyc_Tcutil_compress(_tmp77E);void*_tmp780=_tmp77F;struct Cyc_Absyn_Tvar*_tmp781;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp780)->tag == 2){_LL5EA: _tmp781=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp780)->f1;_LL5EB:
# 3801
 return Cyc_Absyn_tvar_cmp(tvar,_tmp781)== 0;}else{_LL5EC: _LL5ED:
 goto _LL5E9;}_LL5E9:;}
# 3804
goto _LL5E4;}else{_LL5E7: _LL5E8:
 goto _LL5E4;}_LL5E4:;}}else{
# 3810
return 1;}
return 0;}
# 3814
static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 3817
void*_tmp782=Cyc_Absyn_compress_kb(tvar->kind);void*_tmp783=_tmp782;switch(*((int*)_tmp783)){case 2: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp783)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp783)->f2)->aliasqual == Cyc_Absyn_Top){_LL5EF: _LL5F0:
 goto _LL5F2;}else{goto _LL5F3;}}else{goto _LL5F3;}case 0: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp783)->f1)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp783)->f1)->aliasqual == Cyc_Absyn_Top){_LL5F1: _LL5F2:
# 3826
 if(((expected_kind->kind == Cyc_Absyn_BoxKind  || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind) && 
# 3829
Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){
if(expected_kind->aliasqual == Cyc_Absyn_Aliasable)
return& Cyc_Tcutil_rk;else{
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}
# 3835
return& Cyc_Tcutil_trk;}else{goto _LL5F3;}}else{goto _LL5F3;}default: _LL5F3: _LL5F4:
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);}_LL5EE:;}
# 3841
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);struct _tuple30{struct Cyc_Tcutil_CVTEnv f1;struct Cyc_List_List*f2;};
# 3845
static struct _tuple30 Cyc_Tcutil_check_clause(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct _dyneither_ptr clause_name,struct Cyc_Absyn_Exp*clause){
# 3848
struct Cyc_List_List*relns=0;
if(clause != 0){
Cyc_Tcexp_tcExp(te,0,clause);
if(!Cyc_Tcutil_is_integral(clause)){
const char*_tmpF3E;void*_tmpF3D[2];struct Cyc_String_pa_PrintArg_struct _tmpF3C;struct Cyc_String_pa_PrintArg_struct _tmpF3B;(_tmpF3B.tag=0,((_tmpF3B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(clause->topt))),((_tmpF3C.tag=0,((_tmpF3C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name),((_tmpF3D[0]=& _tmpF3C,((_tmpF3D[1]=& _tmpF3B,Cyc_Tcutil_terr(loc,((_tmpF3E="%s clause has type %s instead of integral type",_tag_dyneither(_tmpF3E,sizeof(char),47))),_tag_dyneither(_tmpF3D,sizeof(void*),2)))))))))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(clause,te,cvtenv);
relns=Cyc_Relations_exp2relns(Cyc_Core_heap_region,clause);
if(!Cyc_Relations_consistent_relations(relns)){
const char*_tmpF43;void*_tmpF42[2];struct Cyc_String_pa_PrintArg_struct _tmpF41;struct Cyc_String_pa_PrintArg_struct _tmpF40;(_tmpF40.tag=0,((_tmpF40.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(clause)),((_tmpF41.tag=0,((_tmpF41.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name),((_tmpF42[0]=& _tmpF41,((_tmpF42[1]=& _tmpF40,Cyc_Tcutil_terr(clause->loc,((_tmpF43="%s clause '%s' may be unsatisfiable",_tag_dyneither(_tmpF43,sizeof(char),36))),_tag_dyneither(_tmpF42,sizeof(void*),2)))))))))))));}}{
# 3860
struct _tuple30 _tmpF44;return(_tmpF44.f1=cvtenv,((_tmpF44.f2=relns,_tmpF44)));};}struct _tuple31{enum Cyc_Absyn_Format_Type f1;void*f2;};
# 3892 "tcutil.cyc"
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect,int allow_abs_aggr){
# 3900
{void*_tmp78D=Cyc_Tcutil_compress(t);void*_tmp78E=_tmp78D;struct Cyc_List_List*_tmp78F;void*_tmp790;void*_tmp791;void*_tmp792;void*_tmp793;void*_tmp794;struct _tuple2*_tmp795;struct Cyc_List_List**_tmp796;struct Cyc_Absyn_Typedefdecl**_tmp797;void**_tmp798;union Cyc_Absyn_AggrInfoU*_tmp799;struct Cyc_List_List**_tmp79A;enum Cyc_Absyn_AggrKind _tmp79B;struct Cyc_List_List*_tmp79C;struct Cyc_List_List*_tmp79D;struct Cyc_List_List**_tmp79E;void**_tmp79F;struct Cyc_Absyn_Tqual*_tmp7A0;void*_tmp7A1;struct Cyc_List_List*_tmp7A2;int _tmp7A3;struct Cyc_Absyn_VarargInfo*_tmp7A4;struct Cyc_List_List*_tmp7A5;struct Cyc_List_List*_tmp7A6;struct Cyc_Absyn_Exp*_tmp7A7;struct Cyc_List_List**_tmp7A8;struct Cyc_Absyn_Exp*_tmp7A9;struct Cyc_List_List**_tmp7AA;void*_tmp7AB;struct Cyc_Absyn_Tqual*_tmp7AC;struct Cyc_Absyn_Exp**_tmp7AD;union Cyc_Absyn_Constraint*_tmp7AE;unsigned int _tmp7AF;struct Cyc_Absyn_Exp*_tmp7B0;struct Cyc_Absyn_Exp*_tmp7B1;void*_tmp7B2;void*_tmp7B3;struct Cyc_Absyn_Tqual*_tmp7B4;void*_tmp7B5;union Cyc_Absyn_Constraint*_tmp7B6;union Cyc_Absyn_Constraint*_tmp7B7;union Cyc_Absyn_Constraint*_tmp7B8;union Cyc_Absyn_DatatypeFieldInfoU*_tmp7B9;struct Cyc_List_List*_tmp7BA;union Cyc_Absyn_DatatypeInfoU*_tmp7BB;struct Cyc_List_List**_tmp7BC;struct _tuple2*_tmp7BD;struct Cyc_Absyn_Enumdecl**_tmp7BE;struct Cyc_List_List*_tmp7BF;void*_tmp7C0;void***_tmp7C1;struct Cyc_Absyn_Tvar*_tmp7C2;struct Cyc_Core_Opt**_tmp7C3;void**_tmp7C4;switch(*((int*)_tmp78E)){case 0: _LL5F6: _LL5F7:
 goto _LL5F5;case 1: _LL5F8: _tmp7C3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp78E)->f1;_tmp7C4=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp78E)->f2;_LL5F9:
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
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpF47;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpF46;struct Cyc_Absyn_Tvar*_tmp7C5=Cyc_Tcutil_new_tvar((void*)((_tmpF46=_cycalloc(sizeof(*_tmpF46)),((_tmpF46[0]=((_tmpF47.tag=2,((_tmpF47.f1=0,((_tmpF47.f2=expected_kind,_tmpF47)))))),_tmpF46)))));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpF4A;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpF49;*_tmp7C4=(void*)((_tmpF49=_cycalloc(sizeof(*_tmpF49)),((_tmpF49[0]=((_tmpF4A.tag=2,((_tmpF4A.f1=_tmp7C5,_tmpF4A)))),_tmpF49))));}
_tmp7C2=_tmp7C5;goto _LL5FB;}else{
# 3918
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}
# 3920
goto _LL5F5;case 2: _LL5FA: _tmp7C2=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp78E)->f1;_LL5FB:
# 3922
{void*_tmp7CA=Cyc_Absyn_compress_kb(_tmp7C2->kind);void*_tmp7CB=_tmp7CA;struct Cyc_Core_Opt**_tmp7CC;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7CB)->tag == 1){_LL631: _tmp7CC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7CB)->f1;_LL632:
# 3924
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpF50;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpF4F;struct Cyc_Core_Opt*_tmpF4E;*_tmp7CC=((_tmpF4E=_cycalloc(sizeof(*_tmpF4E)),((_tmpF4E->v=(void*)((_tmpF50=_cycalloc(sizeof(*_tmpF50)),((_tmpF50[0]=((_tmpF4F.tag=2,((_tmpF4F.f1=0,((_tmpF4F.f2=expected_kind,_tmpF4F)))))),_tmpF50)))),_tmpF4E))));}goto _LL630;}else{_LL633: _LL634:
 goto _LL630;}_LL630:;}
# 3929
cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp7C2);
# 3932
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp7C2,put_in_effect);
# 3934
{void*_tmp7D0=Cyc_Absyn_compress_kb(_tmp7C2->kind);void*_tmp7D1=_tmp7D0;struct Cyc_Core_Opt**_tmp7D2;struct Cyc_Absyn_Kind*_tmp7D3;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7D1)->tag == 2){_LL636: _tmp7D2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7D1)->f1;_tmp7D3=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7D1)->f2;_LL637:
# 3936
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmp7D3)){
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpF56;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpF55;struct Cyc_Core_Opt*_tmpF54;*_tmp7D2=((_tmpF54=_cycalloc(sizeof(*_tmpF54)),((_tmpF54->v=(void*)((_tmpF56=_cycalloc(sizeof(*_tmpF56)),((_tmpF56[0]=((_tmpF55.tag=2,((_tmpF55.f1=0,((_tmpF55.f2=expected_kind,_tmpF55)))))),_tmpF56)))),_tmpF54))));}
goto _LL635;}else{_LL638: _LL639:
 goto _LL635;}_LL635:;}
# 3941
goto _LL5F5;case 26: _LL5FC: _tmp7C0=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp78E)->f1)->r;_tmp7C1=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp78E)->f2;_LL5FD: {
# 3947
void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmp7D7=_tmp7C0;struct Cyc_Absyn_Datatypedecl*_tmp7D8;struct Cyc_Absyn_Enumdecl*_tmp7D9;struct Cyc_Absyn_Aggrdecl*_tmp7DA;switch(*((int*)_tmp7D7)){case 0: _LL63B: _tmp7DA=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp7D7)->f1;_LL63C:
# 3950
 if(te->in_extern_c_include)
_tmp7DA->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp7DA);goto _LL63A;case 1: _LL63D: _tmp7D9=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp7D7)->f1;_LL63E:
# 3954
 if(te->in_extern_c_include)
_tmp7D9->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp7D9);goto _LL63A;default: _LL63F: _tmp7D8=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp7D7)->f1;_LL640:
# 3958
 Cyc_Tc_tcDatatypedecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp7D8);goto _LL63A;}_LL63A:;}
# 3960
{void**_tmpF57;*_tmp7C1=((_tmpF57=_cycalloc(sizeof(*_tmpF57)),((_tmpF57[0]=new_t,_tmpF57))));}
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect,allow_abs_aggr);}case 14: _LL5FE: _tmp7BF=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp78E)->f1;_LL5FF: {
# 3966
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct _RegionHandle _tmp7DC=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp7DC;_push_region(uprev_rgn);{
struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;
for(0;_tmp7BF != 0;_tmp7BF=_tmp7BF->tl){
struct Cyc_Absyn_Enumfield*_tmp7DD=(struct Cyc_Absyn_Enumfield*)_tmp7BF->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp7DD->name).f2)){
const char*_tmpF5B;void*_tmpF5A[1];struct Cyc_String_pa_PrintArg_struct _tmpF59;(_tmpF59.tag=0,((_tmpF59.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp7DD->name).f2),((_tmpF5A[0]=& _tmpF59,Cyc_Tcutil_terr(_tmp7DD->loc,((_tmpF5B="duplicate enum field name %s",_tag_dyneither(_tmpF5B,sizeof(char),29))),_tag_dyneither(_tmpF5A,sizeof(void*),1)))))));}else{
# 3975
struct Cyc_List_List*_tmpF5C;prev_fields=((_tmpF5C=_region_malloc(uprev_rgn,sizeof(*_tmpF5C)),((_tmpF5C->hd=(*_tmp7DD->name).f2,((_tmpF5C->tl=prev_fields,_tmpF5C))))));}
# 3977
if(_tmp7DD->tag == 0)
_tmp7DD->tag=Cyc_Absyn_uint_exp(tag_count,_tmp7DD->loc);else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp7DD->tag))){
const char*_tmpF60;void*_tmpF5F[1];struct Cyc_String_pa_PrintArg_struct _tmpF5E;(_tmpF5E.tag=0,((_tmpF5E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp7DD->name).f2),((_tmpF5F[0]=& _tmpF5E,Cyc_Tcutil_terr(loc,((_tmpF60="enum field %s: expression is not constant",_tag_dyneither(_tmpF60,sizeof(char),42))),_tag_dyneither(_tmpF5F,sizeof(void*),1)))))));}}{
# 3982
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp7DD->tag))).f1;
tag_count=t1 + 1;
{union Cyc_Absyn_Nmspace _tmp7E5=(*_tmp7DD->name).f1;union Cyc_Absyn_Nmspace _tmp7E6=_tmp7E5;if((_tmp7E6.Rel_n).tag == 1){_LL642: _LL643:
# 3986
(*_tmp7DD->name).f1=Cyc_Absyn_Abs_n(te->ns,0);goto _LL641;}else{_LL644: _LL645:
 goto _LL641;}_LL641:;}{
# 3989
struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmpF66;struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct _tmpF65;struct _tuple29*_tmpF64;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple29*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp7DD->name).f2,(
(_tmpF64=_cycalloc(sizeof(*_tmpF64)),((_tmpF64->f1=(void*)((_tmpF66=_cycalloc(sizeof(*_tmpF66)),((_tmpF66[0]=((_tmpF65.tag=4,((_tmpF65.f1=t,((_tmpF65.f2=_tmp7DD,_tmpF65)))))),_tmpF66)))),((_tmpF64->f2=1,_tmpF64)))))));};};}}
# 3993
_npop_handler(0);goto _LL5F5;
# 3967
;_pop_region(uprev_rgn);}case 13: _LL600: _tmp7BD=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp78E)->f1;_tmp7BE=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp78E)->f2;_LL601:
# 3995
 if(*_tmp7BE == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp7BE))->fields == 0){
struct _handler_cons _tmp7EA;_push_handler(& _tmp7EA);{int _tmp7EC=0;if(setjmp(_tmp7EA.handler))_tmp7EC=1;if(!_tmp7EC){
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp7BD);
*_tmp7BE=*ed;}
# 3997
;_pop_handler();}else{void*_tmp7EB=(void*)_exn_thrown;void*_tmp7ED=_tmp7EB;void*_tmp7EE;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp7ED)->tag == Cyc_Dict_Absent){_LL647: _LL648: {
# 4001
struct Cyc_Tcenv_Genv*_tmp7EF=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmpF67;struct Cyc_Absyn_Enumdecl*_tmp7F0=(_tmpF67=_cycalloc(sizeof(*_tmpF67)),((_tmpF67->sc=Cyc_Absyn_Extern,((_tmpF67->name=_tmp7BD,((_tmpF67->fields=0,_tmpF67)))))));
Cyc_Tc_tcEnumdecl(te,_tmp7EF,loc,_tmp7F0);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp7BD);
*_tmp7BE=*ed;
goto _LL646;};}}else{_LL649: _tmp7EE=_tmp7ED;_LL64A:(int)_rethrow(_tmp7EE);}_LL646:;}};}{
# 4010
struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp7BE);
# 4012
*_tmp7BD=(ed->name)[0];
goto _LL5F5;};case 3: _LL602: _tmp7BB=(union Cyc_Absyn_DatatypeInfoU*)&(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp78E)->f1).datatype_info;_tmp7BC=(struct Cyc_List_List**)&(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp78E)->f1).targs;_LL603: {
# 4015
struct Cyc_List_List*_tmp7F2=*_tmp7BC;
{union Cyc_Absyn_DatatypeInfoU _tmp7F3=*_tmp7BB;union Cyc_Absyn_DatatypeInfoU _tmp7F4=_tmp7F3;struct Cyc_Absyn_Datatypedecl*_tmp7F5;struct _tuple2*_tmp7F6;int _tmp7F7;if((_tmp7F4.UnknownDatatype).tag == 1){_LL64C: _tmp7F6=((_tmp7F4.UnknownDatatype).val).name;_tmp7F7=((_tmp7F4.UnknownDatatype).val).is_extensible;_LL64D: {
# 4018
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmp7F8;_push_handler(& _tmp7F8);{int _tmp7FA=0;if(setjmp(_tmp7F8.handler))_tmp7FA=1;if(!_tmp7FA){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp7F6);;_pop_handler();}else{void*_tmp7F9=(void*)_exn_thrown;void*_tmp7FB=_tmp7F9;void*_tmp7FC;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp7FB)->tag == Cyc_Dict_Absent){_LL651: _LL652: {
# 4022
struct Cyc_Tcenv_Genv*_tmp7FD=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Datatypedecl*_tmpF68;struct Cyc_Absyn_Datatypedecl*_tmp7FE=(_tmpF68=_cycalloc(sizeof(*_tmpF68)),((_tmpF68->sc=Cyc_Absyn_Extern,((_tmpF68->name=_tmp7F6,((_tmpF68->tvs=0,((_tmpF68->fields=0,((_tmpF68->is_extensible=_tmp7F7,_tmpF68)))))))))));
Cyc_Tc_tcDatatypedecl(te,_tmp7FD,loc,_tmp7FE);
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp7F6);
# 4027
if(_tmp7F2 != 0){
{const char*_tmpF6C;void*_tmpF6B[1];struct Cyc_String_pa_PrintArg_struct _tmpF6A;(_tmpF6A.tag=0,((_tmpF6A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7F6)),((_tmpF6B[0]=& _tmpF6A,Cyc_Tcutil_terr(loc,((_tmpF6C="declare parameterized datatype %s before using",_tag_dyneither(_tmpF6C,sizeof(char),47))),_tag_dyneither(_tmpF6B,sizeof(void*),1)))))));}
return cvtenv;}
# 4032
goto _LL650;}}else{_LL653: _tmp7FC=_tmp7FB;_LL654:(int)_rethrow(_tmp7FC);}_LL650:;}};}
# 4038
if(_tmp7F7  && !(*tudp)->is_extensible){
const char*_tmpF70;void*_tmpF6F[1];struct Cyc_String_pa_PrintArg_struct _tmpF6E;(_tmpF6E.tag=0,((_tmpF6E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7F6)),((_tmpF6F[0]=& _tmpF6E,Cyc_Tcutil_terr(loc,((_tmpF70="datatype %s was not declared @extensible",_tag_dyneither(_tmpF70,sizeof(char),41))),_tag_dyneither(_tmpF6F,sizeof(void*),1)))))));}
*_tmp7BB=Cyc_Absyn_KnownDatatype(tudp);
_tmp7F5=*tudp;goto _LL64F;}}else{_LL64E: _tmp7F5=*(_tmp7F4.KnownDatatype).val;_LL64F: {
# 4045
struct Cyc_List_List*tvs=_tmp7F5->tvs;
for(0;_tmp7F2 != 0  && tvs != 0;(_tmp7F2=_tmp7F2->tl,tvs=tvs->tl)){
void*t=(void*)_tmp7F2->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 4051
{struct _tuple0 _tmpF71;struct _tuple0 _tmp806=(_tmpF71.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmpF71.f2=t,_tmpF71)));struct _tuple0 _tmp807=_tmp806;struct Cyc_Absyn_Tvar*_tmp808;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp807.f1)->tag == 1){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp807.f2)->tag == 2){_LL656: _tmp808=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp807.f2)->f1;_LL657:
# 4053
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp808);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp808,1);
continue;}else{goto _LL658;}}else{_LL658: _LL659:
 goto _LL655;}_LL655:;}{
# 4058
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 4062
if(_tmp7F2 != 0){
const char*_tmpF75;void*_tmpF74[1];struct Cyc_String_pa_PrintArg_struct _tmpF73;(_tmpF73.tag=0,((_tmpF73.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7F5->name)),((_tmpF74[0]=& _tmpF73,Cyc_Tcutil_terr(loc,((_tmpF75="too many type arguments for datatype %s",_tag_dyneither(_tmpF75,sizeof(char),40))),_tag_dyneither(_tmpF74,sizeof(void*),1)))))));}
if(tvs != 0){
# 4067
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmpF76;hidden_ts=((_tmpF76=_cycalloc(sizeof(*_tmpF76)),((_tmpF76->hd=e,((_tmpF76->tl=hidden_ts,_tmpF76))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1,allow_abs_aggr);}
# 4074
*_tmp7BC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp7BC,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4076
goto _LL64B;}}_LL64B:;}
# 4078
goto _LL5F5;}case 4: _LL604: _tmp7B9=(union Cyc_Absyn_DatatypeFieldInfoU*)&(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp78E)->f1).field_info;_tmp7BA=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp78E)->f1).targs;_LL605:
# 4081
{union Cyc_Absyn_DatatypeFieldInfoU _tmp80E=*_tmp7B9;union Cyc_Absyn_DatatypeFieldInfoU _tmp80F=_tmp80E;struct Cyc_Absyn_Datatypedecl*_tmp810;struct Cyc_Absyn_Datatypefield*_tmp811;struct _tuple2*_tmp812;struct _tuple2*_tmp813;int _tmp814;if((_tmp80F.UnknownDatatypefield).tag == 1){_LL65B: _tmp812=((_tmp80F.UnknownDatatypefield).val).datatype_name;_tmp813=((_tmp80F.UnknownDatatypefield).val).field_name;_tmp814=((_tmp80F.UnknownDatatypefield).val).is_extensible;_LL65C: {
# 4083
struct Cyc_Absyn_Datatypedecl*tud;
struct Cyc_Absyn_Datatypefield*tuf;
{struct _handler_cons _tmp815;_push_handler(& _tmp815);{int _tmp817=0;if(setjmp(_tmp815.handler))_tmp817=1;if(!_tmp817){*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp812);;_pop_handler();}else{void*_tmp816=(void*)_exn_thrown;void*_tmp818=_tmp816;void*_tmp819;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp818)->tag == Cyc_Dict_Absent){_LL660: _LL661:
# 4087
{const char*_tmpF7A;void*_tmpF79[1];struct Cyc_String_pa_PrintArg_struct _tmpF78;(_tmpF78.tag=0,((_tmpF78.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp812)),((_tmpF79[0]=& _tmpF78,Cyc_Tcutil_terr(loc,((_tmpF7A="unbound datatype %s",_tag_dyneither(_tmpF7A,sizeof(char),20))),_tag_dyneither(_tmpF79,sizeof(void*),1)))))));}
return cvtenv;}else{_LL662: _tmp819=_tmp818;_LL663:(int)_rethrow(_tmp819);}_LL65F:;}};}
# 4090
{struct _handler_cons _tmp81D;_push_handler(& _tmp81D);{int _tmp81F=0;if(setjmp(_tmp81D.handler))_tmp81F=1;if(!_tmp81F){
{struct _RegionHandle _tmp820=_new_region("r");struct _RegionHandle*r=& _tmp820;_push_region(r);
{void*_tmp821=Cyc_Tcenv_lookup_ordinary(r,te,loc,_tmp813,0);void*_tmp822=_tmp821;struct Cyc_Absyn_Datatypedecl*_tmp823;struct Cyc_Absyn_Datatypefield*_tmp824;if(((struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp822)->tag == 2){_LL665: _tmp823=((struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp822)->f1;_tmp824=((struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp822)->f2;_LL666:
# 4094
 tuf=_tmp824;
tud=_tmp823;
if(_tmp814  && !tud->is_extensible){
const char*_tmpF7E;void*_tmpF7D[1];struct Cyc_String_pa_PrintArg_struct _tmpF7C;(_tmpF7C.tag=0,((_tmpF7C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp812)),((_tmpF7D[0]=& _tmpF7C,Cyc_Tcutil_terr(loc,((_tmpF7E="datatype %s was not declared @extensible",_tag_dyneither(_tmpF7E,sizeof(char),41))),_tag_dyneither(_tmpF7D,sizeof(void*),1)))))));}
goto _LL664;}else{_LL667: _LL668:
{const char*_tmpF83;void*_tmpF82[2];struct Cyc_String_pa_PrintArg_struct _tmpF81;struct Cyc_String_pa_PrintArg_struct _tmpF80;(_tmpF80.tag=0,((_tmpF80.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp812)),((_tmpF81.tag=0,((_tmpF81.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp813)),((_tmpF82[0]=& _tmpF81,((_tmpF82[1]=& _tmpF80,Cyc_Tcutil_terr(loc,((_tmpF83="unbound field %s in type datatype %s",_tag_dyneither(_tmpF83,sizeof(char),37))),_tag_dyneither(_tmpF82,sizeof(void*),2)))))))))))));}{
struct Cyc_Tcutil_CVTEnv _tmp82C=cvtenv;_npop_handler(1);return _tmp82C;};}_LL664:;}
# 4092
;_pop_region(r);}
# 4091
;_pop_handler();}else{void*_tmp81E=(void*)_exn_thrown;void*_tmp82D=_tmp81E;void*_tmp82E;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp82D)->tag == Cyc_Dict_Absent){_LL66A: _LL66B:
# 4107
{const char*_tmpF88;void*_tmpF87[2];struct Cyc_String_pa_PrintArg_struct _tmpF86;struct Cyc_String_pa_PrintArg_struct _tmpF85;(_tmpF85.tag=0,((_tmpF85.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp812)),((_tmpF86.tag=0,((_tmpF86.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp813)),((_tmpF87[0]=& _tmpF86,((_tmpF87[1]=& _tmpF85,Cyc_Tcutil_terr(loc,((_tmpF88="unbound field %s in type datatype %s",_tag_dyneither(_tmpF88,sizeof(char),37))),_tag_dyneither(_tmpF87,sizeof(void*),2)))))))))))));}
return cvtenv;}else{_LL66C: _tmp82E=_tmp82D;_LL66D:(int)_rethrow(_tmp82E);}_LL669:;}};}
# 4111
*_tmp7B9=Cyc_Absyn_KnownDatatypefield(tud,tuf);
_tmp810=tud;_tmp811=tuf;goto _LL65E;}}else{_LL65D: _tmp810=((_tmp80F.KnownDatatypefield).val).f1;_tmp811=((_tmp80F.KnownDatatypefield).val).f2;_LL65E: {
# 4115
struct Cyc_List_List*tvs=_tmp810->tvs;
for(0;_tmp7BA != 0  && tvs != 0;(_tmp7BA=_tmp7BA->tl,tvs=tvs->tl)){
void*t=(void*)_tmp7BA->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 4121
{struct _tuple0 _tmpF89;struct _tuple0 _tmp833=(_tmpF89.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmpF89.f2=t,_tmpF89)));struct _tuple0 _tmp834=_tmp833;struct Cyc_Absyn_Tvar*_tmp835;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp834.f1)->tag == 1){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp834.f2)->tag == 2){_LL66F: _tmp835=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp834.f2)->f1;_LL670:
# 4123
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp835);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp835,1);
continue;}else{goto _LL671;}}else{_LL671: _LL672:
 goto _LL66E;}_LL66E:;}{
# 4128
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 4132
if(_tmp7BA != 0){
const char*_tmpF8E;void*_tmpF8D[2];struct Cyc_String_pa_PrintArg_struct _tmpF8C;struct Cyc_String_pa_PrintArg_struct _tmpF8B;(_tmpF8B.tag=0,((_tmpF8B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp811->name)),((_tmpF8C.tag=0,((_tmpF8C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp810->name)),((_tmpF8D[0]=& _tmpF8C,((_tmpF8D[1]=& _tmpF8B,Cyc_Tcutil_terr(loc,((_tmpF8E="too many type arguments for datatype %s.%s",_tag_dyneither(_tmpF8E,sizeof(char),43))),_tag_dyneither(_tmpF8D,sizeof(void*),2)))))))))))));}
if(tvs != 0){
const char*_tmpF93;void*_tmpF92[2];struct Cyc_String_pa_PrintArg_struct _tmpF91;struct Cyc_String_pa_PrintArg_struct _tmpF90;(_tmpF90.tag=0,((_tmpF90.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp811->name)),((_tmpF91.tag=0,((_tmpF91.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp810->name)),((_tmpF92[0]=& _tmpF91,((_tmpF92[1]=& _tmpF90,Cyc_Tcutil_terr(loc,((_tmpF93="too few type arguments for datatype %s.%s",_tag_dyneither(_tmpF93,sizeof(char),42))),_tag_dyneither(_tmpF92,sizeof(void*),2)))))))))))));}
goto _LL65A;}}_LL65A:;}
# 4140
goto _LL5F5;case 5: _LL606: _tmp7B3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp78E)->f1).elt_typ;_tmp7B4=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp78E)->f1).elt_tq;_tmp7B5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp78E)->f1).ptr_atts).rgn;_tmp7B6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp78E)->f1).ptr_atts).nullable;_tmp7B7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp78E)->f1).ptr_atts).bounds;_tmp7B8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp78E)->f1).ptr_atts).zero_term;_LL607: {
# 4143
int is_zero_terminated;
# 4145
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp7B3,1,1);
_tmp7B4->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp7B4->print_const,_tmp7B3);{
struct Cyc_Absyn_Kind*k;
{enum Cyc_Absyn_AliasQual _tmp83F=expected_kind->aliasqual;enum Cyc_Absyn_AliasQual _tmp840=_tmp83F;switch(_tmp840){case Cyc_Absyn_Aliasable: _LL674: _LL675:
 k=& Cyc_Tcutil_rk;goto _LL673;case Cyc_Absyn_Unique: _LL676: _LL677:
 k=& Cyc_Tcutil_urk;goto _LL673;default: _LL678: _LL679:
 k=& Cyc_Tcutil_trk;goto _LL673;}_LL673:;}
# 4153
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmp7B5,1,1);
{union Cyc_Absyn_Constraint*_tmp841=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp7B8);union Cyc_Absyn_Constraint*_tmp842=_tmp841;int _tmp843;switch((((union Cyc_Absyn_Constraint*)_tmp842)->Eq_constr).tag){case 3: _LL67B: _LL67C:
# 4158
{void*_tmp844=Cyc_Tcutil_compress(_tmp7B3);void*_tmp845=_tmp844;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp845)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp845)->f2 == Cyc_Absyn_Char_sz){_LL682: _LL683:
# 4160
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp7B8,Cyc_Absyn_true_conref);
is_zero_terminated=1;
goto _LL681;}else{goto _LL684;}}else{_LL684: _LL685:
# 4164
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp7B8,Cyc_Absyn_false_conref);
is_zero_terminated=0;
goto _LL681;}_LL681:;}
# 4168
goto _LL67A;case 1: _LL67D: _tmp843=(int)(_tmp842->Eq_constr).val;if(_tmp843){_LL67E:
# 4171
 if(!Cyc_Tcutil_admits_zero(_tmp7B3)){
const char*_tmpF97;void*_tmpF96[1];struct Cyc_String_pa_PrintArg_struct _tmpF95;(_tmpF95.tag=0,((_tmpF95.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp7B3)),((_tmpF96[0]=& _tmpF95,Cyc_Tcutil_terr(loc,((_tmpF97="cannot have a pointer to zero-terminated %s elements",_tag_dyneither(_tmpF97,sizeof(char),53))),_tag_dyneither(_tmpF96,sizeof(void*),1)))))));}
is_zero_terminated=1;
goto _LL67A;}else{goto _LL67F;}default: _LL67F: _LL680:
# 4177
 is_zero_terminated=0;
goto _LL67A;}_LL67A:;}
# 4181
{void*_tmp849=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp7B7);void*_tmp84A=_tmp849;struct Cyc_Absyn_Exp*_tmp84B;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp84A)->tag == 0){_LL687: _LL688:
 goto _LL686;}else{_LL689: _tmp84B=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp84A)->f1;_LL68A: {
# 4184
struct _RegionHandle _tmp84C=_new_region("temp");struct _RegionHandle*temp=& _tmp84C;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp84D=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp84D,0,_tmp84B);}
# 4188
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp84B,te,cvtenv);
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp84B)){
const char*_tmpF9A;void*_tmpF99;(_tmpF99=0,Cyc_Tcutil_terr(loc,((_tmpF9A="pointer bounds expression is not an unsigned int",_tag_dyneither(_tmpF9A,sizeof(char),49))),_tag_dyneither(_tmpF99,sizeof(void*),0)));}{
struct _tuple14 _tmp850=Cyc_Evexp_eval_const_uint_exp(_tmp84B);unsigned int _tmp852;int _tmp853;struct _tuple14 _tmp851=_tmp850;_tmp852=_tmp851.f1;_tmp853=_tmp851.f2;
if(is_zero_terminated  && (!_tmp853  || _tmp852 < 1)){
const char*_tmpF9D;void*_tmpF9C;(_tmpF9C=0,Cyc_Tcutil_terr(loc,((_tmpF9D="zero-terminated pointer cannot point to empty sequence",_tag_dyneither(_tmpF9D,sizeof(char),55))),_tag_dyneither(_tmpF9C,sizeof(void*),0)));}
_npop_handler(0);goto _LL686;};
# 4184
;_pop_region(temp);}}_LL686:;}
# 4196
goto _LL5F5;};}case 19: _LL608: _tmp7B2=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp78E)->f1;_LL609:
# 4198
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp7B2,1,1);goto _LL5F5;case 28: _LL60A: _LL60B:
 goto _LL5F5;case 18: _LL60C: _tmp7B1=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp78E)->f1;_LL60D: {
# 4204
struct _RegionHandle _tmp856=_new_region("temp");struct _RegionHandle*temp=& _tmp856;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp857=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp857,0,_tmp7B1);}
# 4208
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp7B1)){
const char*_tmpFA0;void*_tmpF9F;(_tmpF9F=0,Cyc_Tcutil_terr(loc,((_tmpFA0="valueof_t requires an int expression",_tag_dyneither(_tmpFA0,sizeof(char),37))),_tag_dyneither(_tmpF9F,sizeof(void*),0)));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7B1,te,cvtenv);
_npop_handler(0);goto _LL5F5;
# 4204
;_pop_region(temp);}case 27: _LL60E: _tmp7B0=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp78E)->f1;_LL60F: {
# 4216
struct _RegionHandle _tmp85A=_new_region("temp");struct _RegionHandle*temp=& _tmp85A;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp85B=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp85B,0,_tmp7B0);}
# 4220
_npop_handler(0);goto _LL5F5;
# 4216
;_pop_region(temp);}case 6: _LL610: _LL611:
# 4221
 goto _LL613;case 7: _LL612: _LL613:
 goto _LL5F5;case 8: _LL614: _tmp7AB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp78E)->f1).elt_type;_tmp7AC=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp78E)->f1).tq;_tmp7AD=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp78E)->f1).num_elts;_tmp7AE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp78E)->f1).zero_term;_tmp7AF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp78E)->f1).zt_loc;_LL615: {
# 4226
struct Cyc_Absyn_Exp*_tmp85C=*_tmp7AD;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp7AB,1,allow_abs_aggr);
_tmp7AC->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp7AC->print_const,_tmp7AB);{
int is_zero_terminated;
{union Cyc_Absyn_Constraint*_tmp85D=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp7AE);union Cyc_Absyn_Constraint*_tmp85E=_tmp85D;int _tmp85F;switch((((union Cyc_Absyn_Constraint*)_tmp85E)->Eq_constr).tag){case 3: _LL68C: _LL68D:
# 4233
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp7AE,Cyc_Absyn_false_conref);
is_zero_terminated=0;
# 4248 "tcutil.cyc"
goto _LL68B;case 1: _LL68E: _tmp85F=(int)(_tmp85E->Eq_constr).val;if(_tmp85F){_LL68F:
# 4251
 if(!Cyc_Tcutil_admits_zero(_tmp7AB)){
const char*_tmpFA4;void*_tmpFA3[1];struct Cyc_String_pa_PrintArg_struct _tmpFA2;(_tmpFA2.tag=0,((_tmpFA2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp7AB)),((_tmpFA3[0]=& _tmpFA2,Cyc_Tcutil_terr(loc,((_tmpFA4="cannot have a zero-terminated array of %s elements",_tag_dyneither(_tmpFA4,sizeof(char),51))),_tag_dyneither(_tmpFA3,sizeof(void*),1)))))));}
is_zero_terminated=1;
goto _LL68B;}else{goto _LL690;}default: _LL690: _LL691:
# 4257
 is_zero_terminated=0;
goto _LL68B;}_LL68B:;}
# 4262
if(_tmp85C == 0){
# 4264
if(is_zero_terminated)
# 4266
*_tmp7AD=(_tmp85C=Cyc_Absyn_uint_exp(1,0));else{
# 4269
{const char*_tmpFA7;void*_tmpFA6;(_tmpFA6=0,Cyc_Tcutil_warn(loc,((_tmpFA7="array bound defaults to 1 here",_tag_dyneither(_tmpFA7,sizeof(char),31))),_tag_dyneither(_tmpFA6,sizeof(void*),0)));}
*_tmp7AD=(_tmp85C=Cyc_Absyn_uint_exp(1,0));}}{
# 4273
struct _RegionHandle _tmp865=_new_region("temp");struct _RegionHandle*temp=& _tmp865;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp866=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp866,0,_tmp85C);}
# 4277
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp85C)){
const char*_tmpFAA;void*_tmpFA9;(_tmpFA9=0,Cyc_Tcutil_terr(loc,((_tmpFAA="array bounds expression is not an unsigned int",_tag_dyneither(_tmpFAA,sizeof(char),47))),_tag_dyneither(_tmpFA9,sizeof(void*),0)));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp85C,te,cvtenv);{
# 4284
struct _tuple14 _tmp869=Cyc_Evexp_eval_const_uint_exp(_tmp85C);unsigned int _tmp86B;int _tmp86C;struct _tuple14 _tmp86A=_tmp869;_tmp86B=_tmp86A.f1;_tmp86C=_tmp86A.f2;
# 4286
if((is_zero_terminated  && _tmp86C) && _tmp86B < 1){
const char*_tmpFAD;void*_tmpFAC;(_tmpFAC=0,Cyc_Tcutil_warn(loc,((_tmpFAD="zero terminated array cannot have zero elements",_tag_dyneither(_tmpFAD,sizeof(char),48))),_tag_dyneither(_tmpFAC,sizeof(void*),0)));}
# 4289
if((_tmp86C  && _tmp86B < 1) && Cyc_Cyclone_tovc_r){
{const char*_tmpFB0;void*_tmpFAF;(_tmpFAF=0,Cyc_Tcutil_warn(loc,((_tmpFB0="arrays with 0 elements are not supported except with gcc -- changing to 1.",_tag_dyneither(_tmpFB0,sizeof(char),75))),_tag_dyneither(_tmpFAF,sizeof(void*),0)));}
*_tmp7AD=Cyc_Absyn_uint_exp(1,0);}
# 4293
_npop_handler(0);goto _LL5F5;};
# 4273
;_pop_region(temp);};};}case 9: _LL616: _tmp79E=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp78E)->f1).tvars;_tmp79F=(void**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp78E)->f1).effect;_tmp7A0=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp78E)->f1).ret_tqual;_tmp7A1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp78E)->f1).ret_typ;_tmp7A2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp78E)->f1).args;_tmp7A3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp78E)->f1).c_varargs;_tmp7A4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp78E)->f1).cyc_varargs;_tmp7A5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp78E)->f1).rgn_po;_tmp7A6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp78E)->f1).attributes;_tmp7A7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp78E)->f1).requires_clause;_tmp7A8=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp78E)->f1).requires_relns;_tmp7A9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp78E)->f1).ensures_clause;_tmp7AA=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp78E)->f1).ensures_relns;_LL617: {
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
if(!Cyc_Absyn_fntype_att((void*)_tmp7A6->hd)){
const char*_tmpFB4;void*_tmpFB3[1];struct Cyc_String_pa_PrintArg_struct _tmpFB2;(_tmpFB2.tag=0,((_tmpFB2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp7A6->hd)),((_tmpFB3[0]=& _tmpFB2,Cyc_Tcutil_terr(loc,((_tmpFB4="bad function type attribute %s",_tag_dyneither(_tmpFB4,sizeof(char),31))),_tag_dyneither(_tmpFB3,sizeof(void*),1)))))));}{
void*_tmp874=(void*)_tmp7A6->hd;void*_tmp875=_tmp874;enum Cyc_Absyn_Format_Type _tmp876;int _tmp877;int _tmp878;switch(*((int*)_tmp875)){case 1: _LL693: _LL694:
# 4313
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL692;case 2: _LL695: _LL696:
# 4315
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL692;case 3: _LL697: _LL698:
# 4317
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL692;case 19: _LL699: _tmp876=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp875)->f1;_tmp877=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp875)->f2;_tmp878=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp875)->f3;_LL69A:
# 4319
 if(!seen_format){
seen_format=1;
ft=_tmp876;
fmt_desc_arg=_tmp877;
fmt_arg_start=_tmp878;}else{
# 4325
const char*_tmpFB7;void*_tmpFB6;(_tmpFB6=0,Cyc_Tcutil_terr(loc,((_tmpFB7="function can't have multiple format attributes",_tag_dyneither(_tmpFB7,sizeof(char),47))),_tag_dyneither(_tmpFB6,sizeof(void*),0)));}
goto _LL692;default: _LL69B: _LL69C:
 goto _LL692;}_LL692:;};}
# 4330
if(num_convs > 1){
const char*_tmpFBA;void*_tmpFB9;(_tmpFB9=0,Cyc_Tcutil_terr(loc,((_tmpFBA="function can't have multiple calling conventions",_tag_dyneither(_tmpFBA,sizeof(char),49))),_tag_dyneither(_tmpFB9,sizeof(void*),0)));}
# 4335
Cyc_Tcutil_check_unique_tvars(loc,*_tmp79E);
{struct Cyc_List_List*b=*_tmp79E;for(0;b != 0;b=b->tl){
((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{
void*_tmp87D=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp87E=_tmp87D;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp87E)->tag == 0){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp87E)->f1)->kind == Cyc_Absyn_MemKind){_LL69E: _LL69F:
# 4341
{const char*_tmpFBE;void*_tmpFBD[1];struct Cyc_String_pa_PrintArg_struct _tmpFBC;(_tmpFBC.tag=0,((_tmpFBC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name),((_tmpFBD[0]=& _tmpFBC,Cyc_Tcutil_terr(loc,((_tmpFBE="function attempts to abstract Mem type variable %s",_tag_dyneither(_tmpFBE,sizeof(char),51))),_tag_dyneither(_tmpFBD,sizeof(void*),1)))))));}
goto _LL69D;}else{goto _LL6A0;}}else{_LL6A0: _LL6A1:
 goto _LL69D;}_LL69D:;};}}{
# 4349
struct _RegionHandle _tmp882=_new_region("newr");struct _RegionHandle*newr=& _tmp882;_push_region(newr);{
struct Cyc_Tcutil_CVTEnv _tmpFBF;struct Cyc_Tcutil_CVTEnv _tmp883=
(_tmpFBF.r=newr,((_tmpFBF.kind_env=cvtenv.kind_env,((_tmpFBF.free_vars=0,((_tmpFBF.free_evars=0,((_tmpFBF.generalize_evars=cvtenv.generalize_evars,((_tmpFBF.fn_result=1,_tmpFBF)))))))))));
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
if(_tmp7A3){const char*_tmpFC2;void*_tmpFC1;(_tmpFC1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFC2="both c_vararg and cyc_vararg",_tag_dyneither(_tmpFC2,sizeof(char),29))),_tag_dyneither(_tmpFC1,sizeof(void*),0)));}{
struct Cyc_Absyn_VarargInfo _tmp88A=*_tmp7A4;void*_tmp88C;int _tmp88D;struct Cyc_Absyn_VarargInfo _tmp88B=_tmp88A;_tmp88C=_tmp88B.type;_tmp88D=_tmp88B.inject;
_tmp883=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp883,& Cyc_Tcutil_tmk,_tmp88C,1,1);
(_tmp7A4->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmp7A4->tq).print_const,_tmp88C);
# 4382
if(_tmp88D){
void*_tmp88E=Cyc_Tcutil_compress(_tmp88C);void*_tmp88F=_tmp88E;void*_tmp890;union Cyc_Absyn_Constraint*_tmp891;union Cyc_Absyn_Constraint*_tmp892;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp88F)->tag == 5){_LL6A3: _tmp890=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp88F)->f1).elt_typ;_tmp891=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp88F)->f1).ptr_atts).bounds;_tmp892=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp88F)->f1).ptr_atts).zero_term;_LL6A4:
# 4385
{void*_tmp893=Cyc_Tcutil_compress(_tmp890);void*_tmp894=_tmp893;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp894)->tag == 3){_LL6A8: _LL6A9:
# 4387
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp892)){
const char*_tmpFC5;void*_tmpFC4;(_tmpFC4=0,Cyc_Tcutil_terr(loc,((_tmpFC5="can't inject into a zeroterm pointer",_tag_dyneither(_tmpFC5,sizeof(char),37))),_tag_dyneither(_tmpFC4,sizeof(void*),0)));}
{void*_tmp897=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp891);void*_tmp898=_tmp897;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp898)->tag == 0){_LL6AD: _LL6AE:
# 4391
{const char*_tmpFC8;void*_tmpFC7;(_tmpFC7=0,Cyc_Tcutil_terr(loc,((_tmpFC8="can't inject into a fat pointer to datatype",_tag_dyneither(_tmpFC8,sizeof(char),44))),_tag_dyneither(_tmpFC7,sizeof(void*),0)));}
goto _LL6AC;}else{_LL6AF: _LL6B0:
 goto _LL6AC;}_LL6AC:;}
# 4395
goto _LL6A7;}else{_LL6AA: _LL6AB:
{const char*_tmpFCB;void*_tmpFCA;(_tmpFCA=0,Cyc_Tcutil_terr(loc,((_tmpFCB="can't inject a non-datatype type",_tag_dyneither(_tmpFCB,sizeof(char),33))),_tag_dyneither(_tmpFCA,sizeof(void*),0)));}goto _LL6A7;}_LL6A7:;}
# 4398
goto _LL6A2;}else{_LL6A5: _LL6A6:
{const char*_tmpFCE;void*_tmpFCD;(_tmpFCD=0,Cyc_Tcutil_terr(loc,((_tmpFCE="expecting a datatype pointer type",_tag_dyneither(_tmpFCE,sizeof(char),34))),_tag_dyneither(_tmpFCD,sizeof(void*),0)));}goto _LL6A2;}_LL6A2:;}};}
# 4404
if(seen_format){
int _tmp89F=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp7A2);
if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmp89F) || fmt_arg_start < 0) || 
# 4408
(_tmp7A4 == 0  && !_tmp7A3) && fmt_arg_start != 0) || 
(_tmp7A4 != 0  || _tmp7A3) && fmt_arg_start != _tmp89F + 1){
# 4411
const char*_tmpFD1;void*_tmpFD0;(_tmpFD0=0,Cyc_Tcutil_terr(loc,((_tmpFD1="bad format descriptor",_tag_dyneither(_tmpFD1,sizeof(char),22))),_tag_dyneither(_tmpFD0,sizeof(void*),0)));}else{
# 4414
struct _tuple10 _tmp8A2=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp7A2,fmt_desc_arg - 1);void*_tmp8A4;struct _tuple10 _tmp8A3=_tmp8A2;_tmp8A4=_tmp8A3.f3;
# 4416
{void*_tmp8A5=Cyc_Tcutil_compress(_tmp8A4);void*_tmp8A6=_tmp8A5;void*_tmp8A7;union Cyc_Absyn_Constraint*_tmp8A8;union Cyc_Absyn_Constraint*_tmp8A9;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8A6)->tag == 5){_LL6B2: _tmp8A7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8A6)->f1).elt_typ;_tmp8A8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8A6)->f1).ptr_atts).bounds;_tmp8A9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8A6)->f1).ptr_atts).zero_term;_LL6B3:
# 4419
{struct _tuple0 _tmpFD2;struct _tuple0 _tmp8AA=(_tmpFD2.f1=Cyc_Tcutil_compress(_tmp8A7),((_tmpFD2.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp8A8),_tmpFD2)));struct _tuple0 _tmp8AB=_tmp8AA;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp8AB.f1)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp8AB.f1)->f2 == Cyc_Absyn_Char_sz){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp8AB.f2)->tag == 0){_LL6B7: _LL6B8:
# 4421
 if(_tmp7A3){
const char*_tmpFD5;void*_tmpFD4;(_tmpFD4=0,Cyc_Tcutil_terr(loc,((_tmpFD5="format descriptor is not a char * type",_tag_dyneither(_tmpFD5,sizeof(char),39))),_tag_dyneither(_tmpFD4,sizeof(void*),0)));}
goto _LL6B6;}else{_LL6B9: _LL6BA:
# 4425
 if(!_tmp7A3){
const char*_tmpFD8;void*_tmpFD7;(_tmpFD7=0,Cyc_Tcutil_terr(loc,((_tmpFD8="format descriptor is not a char ? type",_tag_dyneither(_tmpFD8,sizeof(char),39))),_tag_dyneither(_tmpFD7,sizeof(void*),0)));}
goto _LL6B6;}}else{goto _LL6BB;}}else{_LL6BB: _LL6BC:
# 4429
{const char*_tmpFDB;void*_tmpFDA;(_tmpFDA=0,Cyc_Tcutil_terr(loc,((_tmpFDB="format descriptor is not a string type",_tag_dyneither(_tmpFDB,sizeof(char),39))),_tag_dyneither(_tmpFDA,sizeof(void*),0)));}
goto _LL6B6;}_LL6B6:;}
# 4432
goto _LL6B1;}else{_LL6B4: _LL6B5:
{const char*_tmpFDE;void*_tmpFDD;(_tmpFDD=0,Cyc_Tcutil_terr(loc,((_tmpFDE="format descriptor is not a string type",_tag_dyneither(_tmpFDE,sizeof(char),39))),_tag_dyneither(_tmpFDD,sizeof(void*),0)));}goto _LL6B1;}_LL6B1:;}
# 4435
if(fmt_arg_start != 0  && !_tmp7A3){
# 4439
int problem;
{struct _tuple31 _tmpFDF;struct _tuple31 _tmp8B5=(_tmpFDF.f1=ft,((_tmpFDF.f2=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp7A4))->type)),_tmpFDF)));struct _tuple31 _tmp8B6=_tmp8B5;struct Cyc_Absyn_Datatypedecl*_tmp8B7;struct Cyc_Absyn_Datatypedecl*_tmp8B8;if(_tmp8B6.f1 == Cyc_Absyn_Printf_ft){if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp8B6.f2)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp8B6.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL6BE: _tmp8B8=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp8B6.f2)->f1).datatype_info).KnownDatatype).val;_LL6BF:
# 4442
 problem=Cyc_Absyn_qvar_cmp(_tmp8B8->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL6BD;}else{goto _LL6C2;}}else{goto _LL6C2;}}else{if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp8B6.f2)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp8B6.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL6C0: _tmp8B7=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp8B6.f2)->f1).datatype_info).KnownDatatype).val;_LL6C1:
# 4444
 problem=Cyc_Absyn_qvar_cmp(_tmp8B7->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL6BD;}else{goto _LL6C2;}}else{_LL6C2: _LL6C3:
# 4446
 problem=1;goto _LL6BD;}}_LL6BD:;}
# 4448
if(problem){
const char*_tmpFE2;void*_tmpFE1;(_tmpFE1=0,Cyc_Tcutil_terr(loc,((_tmpFE2="format attribute and vararg types don't match",_tag_dyneither(_tmpFE2,sizeof(char),46))),_tag_dyneither(_tmpFE1,sizeof(void*),0)));}}}}
# 4456
{struct Cyc_List_List*rpo=_tmp7A5;for(0;rpo != 0;rpo=rpo->tl){
struct _tuple0*_tmp8BC=(struct _tuple0*)rpo->hd;void*_tmp8BE;void*_tmp8BF;struct _tuple0*_tmp8BD=_tmp8BC;_tmp8BE=_tmp8BD->f1;_tmp8BF=_tmp8BD->f2;
_tmp883=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp883,& Cyc_Tcutil_ek,_tmp8BE,1,1);
_tmp883=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp883,& Cyc_Tcutil_trk,_tmp8BF,1,1);}}{
# 4464
struct Cyc_Tcenv_Fenv*_tmp8C0=Cyc_Tcenv_bogus_fenv(newr,_tmp7A1,_tmp7A2);
struct Cyc_Tcenv_Tenv*_tmpFE3;struct Cyc_Tcenv_Tenv*_tmp8C1=(_tmpFE3=_region_malloc(newr,sizeof(*_tmpFE3)),((_tmpFE3->ns=te->ns,((_tmpFE3->ae=te->ae,((_tmpFE3->le=_tmp8C0,((_tmpFE3->allow_valueof=1,((_tmpFE3->in_extern_c_include=te->in_extern_c_include,_tmpFE3)))))))))));
const char*_tmpFE4;struct _tuple30 _tmp8C2=Cyc_Tcutil_check_clause(loc,_tmp8C1,_tmp883,((_tmpFE4="@requires",_tag_dyneither(_tmpFE4,sizeof(char),10))),_tmp7A7);struct Cyc_Tcutil_CVTEnv _tmp8C4;struct Cyc_List_List*_tmp8C5;struct _tuple30 _tmp8C3=_tmp8C2;_tmp8C4=_tmp8C3.f1;_tmp8C5=_tmp8C3.f2;
_tmp883=_tmp8C4;
*_tmp7A8=_tmp8C5;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp7A2,_tmp8C5);{
# 4471
struct _dyneither_ptr*_tmp8C6=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"return_value",sizeof(char),13);
struct _tuple2*_tmpFE7;struct Cyc_Absyn_Vardecl*_tmpFE6;struct Cyc_Absyn_Vardecl*_tmp8C7=(_tmpFE6=_cycalloc(sizeof(*_tmpFE6)),((_tmpFE6->sc=Cyc_Absyn_Public,((_tmpFE6->name=((_tmpFE7=_cycalloc(sizeof(*_tmpFE7)),((_tmpFE7->f1=Cyc_Absyn_Loc_n,((_tmpFE7->f2=_tmp8C6,_tmpFE7)))))),((_tmpFE6->tq=
Cyc_Absyn_empty_tqual(0),((_tmpFE6->type=_tmp7A1,((_tmpFE6->initializer=0,((_tmpFE6->rgn=
Cyc_Tcenv_curr_rgn(_tmp8C1),((_tmpFE6->attributes=0,((_tmpFE6->escapes=0,_tmpFE6)))))))))))))))));
# 4476
_tmp8C1=Cyc_Tcenv_add_local_var(newr,0,_tmp8C1,_tmp8C7);{
const char*_tmpFE8;struct _tuple30 _tmp8C8=Cyc_Tcutil_check_clause(loc,_tmp8C1,_tmp883,((_tmpFE8="@ensures",_tag_dyneither(_tmpFE8,sizeof(char),9))),_tmp7A9);struct Cyc_Tcutil_CVTEnv _tmp8CA;struct Cyc_List_List*_tmp8CB;struct _tuple30 _tmp8C9=_tmp8C8;_tmp8CA=_tmp8C9.f1;_tmp8CB=_tmp8C9.f2;
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
struct _tuple28 _tmp8CC=*((struct _tuple28*)tvs->hd);struct Cyc_Absyn_Tvar*_tmp8CE;int _tmp8CF;struct _tuple28 _tmp8CD=_tmp8CC;_tmp8CE=_tmp8CD.f1;_tmp8CF=_tmp8CD.f2;
if(!_tmp8CF)continue;{
void*_tmp8D0=Cyc_Absyn_compress_kb(_tmp8CE->kind);void*_tmp8D1=_tmp8D0;struct Cyc_Core_Opt**_tmp8D2;struct Cyc_Absyn_Kind*_tmp8D3;struct Cyc_Core_Opt**_tmp8D4;struct Cyc_Core_Opt**_tmp8D5;struct Cyc_Absyn_Kind*_tmp8D6;switch(*((int*)_tmp8D1)){case 2: _LL6C5: _tmp8D5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8D1)->f1;_tmp8D6=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8D1)->f2;if(_tmp8D6->kind == Cyc_Absyn_RgnKind){_LL6C6:
# 4496
 if(_tmp8D6->aliasqual == Cyc_Absyn_Top){
*_tmp8D5=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);_tmp8D3=_tmp8D6;goto _LL6C8;}
# 4499
*_tmp8D5=Cyc_Tcutil_kind_to_bound_opt(_tmp8D6);_tmp8D3=_tmp8D6;goto _LL6C8;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8D1)->f2)->kind){case Cyc_Absyn_IntKind: _LL6C9: _LL6CA:
# 4502
 goto _LL6CC;case Cyc_Absyn_EffKind: _LL6CD: _tmp8D4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8D1)->f1;_LL6CE:
# 4505
*_tmp8D4=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);goto _LL6D0;default: goto _LL6D3;}}case 0: _LL6C7: _tmp8D3=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp8D1)->f1;if(_tmp8D3->kind == Cyc_Absyn_RgnKind){_LL6C8:
# 4501
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpFF7;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpFF6;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpFF5;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpFF4;struct Cyc_List_List*_tmpFF3;effect=((_tmpFF3=_cycalloc(sizeof(*_tmpFF3)),((_tmpFF3->hd=(void*)((_tmpFF7=_cycalloc(sizeof(*_tmpFF7)),((_tmpFF7[0]=((_tmpFF4.tag=23,((_tmpFF4.f1=(void*)((_tmpFF6=_cycalloc(sizeof(*_tmpFF6)),((_tmpFF6[0]=((_tmpFF5.tag=2,((_tmpFF5.f1=_tmp8CE,_tmpFF5)))),_tmpFF6)))),_tmpFF4)))),_tmpFF7)))),((_tmpFF3->tl=effect,_tmpFF3))))));}goto _LL6C4;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp8D1)->f1)->kind){case Cyc_Absyn_IntKind: _LL6CB: _LL6CC:
# 4503
 goto _LL6C4;case Cyc_Absyn_EffKind: _LL6CF: _LL6D0:
# 4507
{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpFFD;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpFFC;struct Cyc_List_List*_tmpFFB;effect=((_tmpFFB=_cycalloc(sizeof(*_tmpFFB)),((_tmpFFB->hd=(void*)((_tmpFFD=_cycalloc(sizeof(*_tmpFFD)),((_tmpFFD[0]=((_tmpFFC.tag=2,((_tmpFFC.f1=_tmp8CE,_tmpFFC)))),_tmpFFD)))),((_tmpFFB->tl=effect,_tmpFFB))))));}goto _LL6C4;default: _LL6D3: _LL6D4:
# 4512
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp100C;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp100B;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp100A;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp1009;struct Cyc_List_List*_tmp1008;effect=((_tmp1008=_cycalloc(sizeof(*_tmp1008)),((_tmp1008->hd=(void*)((_tmp100C=_cycalloc(sizeof(*_tmp100C)),((_tmp100C[0]=((_tmp1009.tag=25,((_tmp1009.f1=(void*)((_tmp100B=_cycalloc(sizeof(*_tmp100B)),((_tmp100B[0]=((_tmp100A.tag=2,((_tmp100A.f1=_tmp8CE,_tmp100A)))),_tmp100B)))),_tmp1009)))),_tmp100C)))),((_tmp1008->tl=effect,_tmp1008))))));}goto _LL6C4;}}default: _LL6D1: _tmp8D2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp8D1)->f1;_LL6D2:
# 4509
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1012;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1011;struct Cyc_Core_Opt*_tmp1010;*_tmp8D2=((_tmp1010=_cycalloc(sizeof(*_tmp1010)),((_tmp1010->v=(void*)((_tmp1012=_cycalloc(sizeof(*_tmp1012)),((_tmp1012[0]=((_tmp1011.tag=2,((_tmp1011.f1=0,((_tmp1011.f2=& Cyc_Tcutil_ak,_tmp1011)))))),_tmp1012)))),_tmp1010))));}goto _LL6D4;}_LL6C4:;};}}
# 4516
{struct Cyc_List_List*ts=_tmp883.free_evars;for(0;ts != 0;ts=ts->tl){
struct _tuple29 _tmp8E7=*((struct _tuple29*)ts->hd);void*_tmp8E9;int _tmp8EA;struct _tuple29 _tmp8E8=_tmp8E7;_tmp8E9=_tmp8E8.f1;_tmp8EA=_tmp8E8.f2;
if(!_tmp8EA)continue;{
struct Cyc_Absyn_Kind*_tmp8EB=Cyc_Tcutil_typ_kind(_tmp8E9);struct Cyc_Absyn_Kind*_tmp8EC=_tmp8EB;switch(((struct Cyc_Absyn_Kind*)_tmp8EC)->kind){case Cyc_Absyn_RgnKind: _LL6D6: _LL6D7:
# 4521
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1018;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1017;struct Cyc_List_List*_tmp1016;effect=((_tmp1016=_cycalloc(sizeof(*_tmp1016)),((_tmp1016->hd=(void*)((_tmp1018=_cycalloc(sizeof(*_tmp1018)),((_tmp1018[0]=((_tmp1017.tag=23,((_tmp1017.f1=_tmp8E9,_tmp1017)))),_tmp1018)))),((_tmp1016->tl=effect,_tmp1016))))));}goto _LL6D5;case Cyc_Absyn_EffKind: _LL6D8: _LL6D9:
# 4523
{struct Cyc_List_List*_tmp1019;effect=((_tmp1019=_cycalloc(sizeof(*_tmp1019)),((_tmp1019->hd=_tmp8E9,((_tmp1019->tl=effect,_tmp1019))))));}goto _LL6D5;case Cyc_Absyn_IntKind: _LL6DA: _LL6DB:
 goto _LL6D5;default: _LL6DC: _LL6DD:
# 4526
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp101F;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp101E;struct Cyc_List_List*_tmp101D;effect=((_tmp101D=_cycalloc(sizeof(*_tmp101D)),((_tmp101D->hd=(void*)((_tmp101F=_cycalloc(sizeof(*_tmp101F)),((_tmp101F[0]=((_tmp101E.tag=25,((_tmp101E.f1=_tmp8E9,_tmp101E)))),_tmp101F)))),((_tmp101D->tl=effect,_tmp101D))))));}goto _LL6D5;}_LL6D5:;};}}{
# 4529
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1022;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1021;*_tmp79F=(void*)((_tmp1021=_cycalloc(sizeof(*_tmp1021)),((_tmp1021[0]=((_tmp1022.tag=24,((_tmp1022.f1=effect,_tmp1022)))),_tmp1021))));};}
# 4535
if(*_tmp79E != 0){
struct Cyc_List_List*bs=*_tmp79E;for(0;bs != 0;bs=bs->tl){
void*_tmp8F6=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);void*_tmp8F7=_tmp8F6;struct Cyc_Core_Opt**_tmp8F8;struct Cyc_Absyn_Kind*_tmp8F9;struct Cyc_Core_Opt**_tmp8FA;struct Cyc_Core_Opt**_tmp8FB;struct Cyc_Core_Opt**_tmp8FC;struct Cyc_Core_Opt**_tmp8FD;struct Cyc_Core_Opt**_tmp8FE;struct Cyc_Core_Opt**_tmp8FF;struct Cyc_Core_Opt**_tmp900;struct Cyc_Core_Opt**_tmp901;struct Cyc_Core_Opt**_tmp902;switch(*((int*)_tmp8F7)){case 1: _LL6DF: _tmp902=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp8F7)->f1;_LL6E0:
# 4539
{const char*_tmp1026;void*_tmp1025[1];struct Cyc_String_pa_PrintArg_struct _tmp1024;(_tmp1024.tag=0,((_tmp1024.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((_tmp1025[0]=& _tmp1024,Cyc_Tcutil_warn(loc,((_tmp1026="Type variable %s unconstrained, assuming boxed",_tag_dyneither(_tmp1026,sizeof(char),47))),_tag_dyneither(_tmp1025,sizeof(void*),1)))))));}
# 4541
_tmp901=_tmp902;goto _LL6E2;case 2: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F7)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F7)->f2)->aliasqual == Cyc_Absyn_Top){_LL6E1: _tmp901=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F7)->f1;_LL6E2:
 _tmp900=_tmp901;goto _LL6E4;}else{goto _LL6F1;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F7)->f2)->aliasqual){case Cyc_Absyn_Top: _LL6E3: _tmp900=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F7)->f1;_LL6E4:
 _tmp8FF=_tmp900;goto _LL6E6;case Cyc_Absyn_Aliasable: _LL6E5: _tmp8FF=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F7)->f1;_LL6E6:
 _tmp8FD=_tmp8FF;goto _LL6E8;default: _LL6EB: _tmp8FE=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F7)->f1;_LL6EC:
# 4548
 _tmp8FB=_tmp8FE;goto _LL6EE;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F7)->f2)->aliasqual){case Cyc_Absyn_Top: _LL6E7: _tmp8FD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F7)->f1;_LL6E8:
# 4545
 _tmp8FC=_tmp8FD;goto _LL6EA;case Cyc_Absyn_Aliasable: _LL6E9: _tmp8FC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F7)->f1;_LL6EA:
# 4547
*_tmp8FC=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL6DE;default: _LL6ED: _tmp8FB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F7)->f1;_LL6EE:
# 4550
*_tmp8FB=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL6DE;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F7)->f2)->aliasqual == Cyc_Absyn_Top){_LL6EF: _tmp8FA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F7)->f1;_LL6F0:
# 4552
*_tmp8FA=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL6DE;}else{goto _LL6F1;}default: _LL6F1: _tmp8F8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F7)->f1;_tmp8F9=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F7)->f2;_LL6F2:
# 4554
*_tmp8F8=Cyc_Tcutil_kind_to_bound_opt(_tmp8F9);goto _LL6DE;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp8F7)->f1)->kind == Cyc_Absyn_MemKind){_LL6F3: _LL6F4:
# 4556
{const char*_tmp1029;void*_tmp1028;(_tmp1028=0,Cyc_Tcutil_terr(loc,((_tmp1029="functions can't abstract M types",_tag_dyneither(_tmp1029,sizeof(char),33))),_tag_dyneither(_tmp1028,sizeof(void*),0)));}goto _LL6DE;}else{_LL6F5: _LL6F6:
 goto _LL6DE;}}_LL6DE:;}}
# 4561
cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,_tmp883.kind_env,*_tmp79E);
_tmp883.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(_tmp883.r,_tmp883.free_vars,*_tmp79E);
# 4564
{struct Cyc_List_List*tvs=_tmp883.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp908=*((struct _tuple28*)tvs->hd);struct Cyc_Absyn_Tvar*_tmp90A;int _tmp90B;struct _tuple28 _tmp909=_tmp908;_tmp90A=_tmp909.f1;_tmp90B=_tmp909.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp90A,_tmp90B);}}{
# 4569
struct Cyc_List_List*evs=_tmp883.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmp90C=*((struct _tuple29*)evs->hd);void*_tmp90E;int _tmp90F;struct _tuple29 _tmp90D=_tmp90C;_tmp90E=_tmp90D.f1;_tmp90F=_tmp90D.f2;
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmp90E,_tmp90F);}};};};};}
# 4574
_npop_handler(0);goto _LL5F5;
# 4349
;_pop_region(newr);};}case 10: _LL618: _tmp79D=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp78E)->f1;_LL619:
# 4577
 for(0;_tmp79D != 0;_tmp79D=_tmp79D->tl){
struct _tuple12*_tmp917=(struct _tuple12*)_tmp79D->hd;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmp917).f2,1,0);
((*_tmp917).f1).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp917).f1).print_const,(*_tmp917).f2);}
# 4583
goto _LL5F5;case 12: _LL61A: _tmp79B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp78E)->f1;_tmp79C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp78E)->f2;_LL61B: {
# 4587
struct _RegionHandle _tmp918=_new_region("aprev_rgn");struct _RegionHandle*aprev_rgn=& _tmp918;_push_region(aprev_rgn);{
struct Cyc_List_List*prev_fields=0;
for(0;_tmp79C != 0;_tmp79C=_tmp79C->tl){
struct Cyc_Absyn_Aggrfield*_tmp919=(struct Cyc_Absyn_Aggrfield*)_tmp79C->hd;struct _dyneither_ptr*_tmp91B;struct Cyc_Absyn_Tqual*_tmp91C;void*_tmp91D;struct Cyc_Absyn_Exp*_tmp91E;struct Cyc_List_List*_tmp91F;struct Cyc_Absyn_Exp*_tmp920;struct Cyc_Absyn_Aggrfield*_tmp91A=_tmp919;_tmp91B=_tmp91A->name;_tmp91C=(struct Cyc_Absyn_Tqual*)& _tmp91A->tq;_tmp91D=_tmp91A->type;_tmp91E=_tmp91A->width;_tmp91F=_tmp91A->attributes;_tmp920=_tmp91A->requires_clause;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp91B)){
const char*_tmp102D;void*_tmp102C[1];struct Cyc_String_pa_PrintArg_struct _tmp102B;(_tmp102B.tag=0,((_tmp102B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp91B),((_tmp102C[0]=& _tmp102B,Cyc_Tcutil_terr(loc,((_tmp102D="duplicate field %s",_tag_dyneither(_tmp102D,sizeof(char),19))),_tag_dyneither(_tmp102C,sizeof(void*),1)))))));}
{const char*_tmp102E;if(Cyc_strcmp((struct _dyneither_ptr)*_tmp91B,((_tmp102E="",_tag_dyneither(_tmp102E,sizeof(char),1))))!= 0){
struct Cyc_List_List*_tmp102F;prev_fields=((_tmp102F=_region_malloc(aprev_rgn,sizeof(*_tmp102F)),((_tmp102F->hd=_tmp91B,((_tmp102F->tl=prev_fields,_tmp102F))))));}}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp91D,1,0);
_tmp91C->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp91C->print_const,_tmp91D);
# 4601
Cyc_Tcutil_check_bitfield(loc,te,_tmp91D,_tmp91E,_tmp91B);
Cyc_Tcutil_check_field_atts(loc,_tmp91B,_tmp91F);
if(_tmp920 != 0){
# 4605
if(_tmp79B != Cyc_Absyn_UnionA){
const char*_tmp1032;void*_tmp1031;(_tmp1031=0,Cyc_Tcutil_terr(loc,((_tmp1032="@requires clause is only allowed on union members",_tag_dyneither(_tmp1032,sizeof(char),50))),_tag_dyneither(_tmp1031,sizeof(void*),0)));}{
struct _RegionHandle _tmp928=_new_region("temp");struct _RegionHandle*temp=& _tmp928;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp929=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp929,0,_tmp920);}
# 4611
if(!Cyc_Tcutil_is_integral(_tmp920)){
const char*_tmp1036;void*_tmp1035[1];struct Cyc_String_pa_PrintArg_struct _tmp1034;(_tmp1034.tag=0,((_tmp1034.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmp920->topt))),((_tmp1035[0]=& _tmp1034,Cyc_Tcutil_terr(loc,((_tmp1036="@requires clause has type %s instead of integral type",_tag_dyneither(_tmp1036,sizeof(char),54))),_tag_dyneither(_tmp1035,sizeof(void*),1)))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp920,te,cvtenv);
# 4607
;_pop_region(temp);};}}}
# 4618
_npop_handler(0);goto _LL5F5;
# 4587
;_pop_region(aprev_rgn);}case 11: _LL61C: _tmp799=(union Cyc_Absyn_AggrInfoU*)&(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp78E)->f1).aggr_info;_tmp79A=(struct Cyc_List_List**)&(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp78E)->f1).targs;_LL61D:
# 4621
{union Cyc_Absyn_AggrInfoU _tmp92D=*_tmp799;union Cyc_Absyn_AggrInfoU _tmp92E=_tmp92D;struct Cyc_Absyn_Aggrdecl*_tmp92F;enum Cyc_Absyn_AggrKind _tmp930;struct _tuple2*_tmp931;struct Cyc_Core_Opt*_tmp932;if((_tmp92E.UnknownAggr).tag == 1){_LL6F8: _tmp930=((_tmp92E.UnknownAggr).val).f1;_tmp931=((_tmp92E.UnknownAggr).val).f2;_tmp932=((_tmp92E.UnknownAggr).val).f3;_LL6F9: {
# 4623
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmp933;_push_handler(& _tmp933);{int _tmp935=0;if(setjmp(_tmp933.handler))_tmp935=1;if(!_tmp935){
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp931);{
struct Cyc_Absyn_Aggrdecl*_tmp936=*adp;
if(_tmp936->kind != _tmp930){
if(_tmp936->kind == Cyc_Absyn_StructA){
const char*_tmp103B;void*_tmp103A[2];struct Cyc_String_pa_PrintArg_struct _tmp1039;struct Cyc_String_pa_PrintArg_struct _tmp1038;(_tmp1038.tag=0,((_tmp1038.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp931)),((_tmp1039.tag=0,((_tmp1039.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4629
Cyc_Absynpp_qvar2string(_tmp931)),((_tmp103A[0]=& _tmp1039,((_tmp103A[1]=& _tmp1038,Cyc_Tcutil_terr(loc,((_tmp103B="expecting struct %s instead of union %s",_tag_dyneither(_tmp103B,sizeof(char),40))),_tag_dyneither(_tmp103A,sizeof(void*),2)))))))))))));}else{
# 4632
const char*_tmp1040;void*_tmp103F[2];struct Cyc_String_pa_PrintArg_struct _tmp103E;struct Cyc_String_pa_PrintArg_struct _tmp103D;(_tmp103D.tag=0,((_tmp103D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp931)),((_tmp103E.tag=0,((_tmp103E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4632
Cyc_Absynpp_qvar2string(_tmp931)),((_tmp103F[0]=& _tmp103E,((_tmp103F[1]=& _tmp103D,Cyc_Tcutil_terr(loc,((_tmp1040="expecting union %s instead of struct %s",_tag_dyneither(_tmp1040,sizeof(char),40))),_tag_dyneither(_tmp103F,sizeof(void*),2)))))))))))));}}
# 4635
if((unsigned int)_tmp932  && (int)_tmp932->v){
if(!((unsigned int)_tmp936->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp936->impl))->tagged){
const char*_tmp1044;void*_tmp1043[1];struct Cyc_String_pa_PrintArg_struct _tmp1042;(_tmp1042.tag=0,((_tmp1042.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp931)),((_tmp1043[0]=& _tmp1042,Cyc_Tcutil_terr(loc,((_tmp1044="@tagged qualfiers don't agree on union %s",_tag_dyneither(_tmp1044,sizeof(char),42))),_tag_dyneither(_tmp1043,sizeof(void*),1)))))));}}
# 4641
*_tmp799=Cyc_Absyn_KnownAggr(adp);};
# 4625
;_pop_handler();}else{void*_tmp934=(void*)_exn_thrown;void*_tmp942=_tmp934;void*_tmp943;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp942)->tag == Cyc_Dict_Absent){_LL6FD: _LL6FE: {
# 4645
struct Cyc_Tcenv_Genv*_tmp944=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp1045;struct Cyc_Absyn_Aggrdecl*_tmp945=(_tmp1045=_cycalloc(sizeof(*_tmp1045)),((_tmp1045->kind=_tmp930,((_tmp1045->sc=Cyc_Absyn_Extern,((_tmp1045->name=_tmp931,((_tmp1045->tvs=0,((_tmp1045->impl=0,((_tmp1045->attributes=0,((_tmp1045->expected_mem_kind=0,_tmp1045)))))))))))))));
Cyc_Tc_tcAggrdecl(te,_tmp944,loc,_tmp945);
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp931);
*_tmp799=Cyc_Absyn_KnownAggr(adp);
# 4651
if(*_tmp79A != 0){
{const char*_tmp1049;void*_tmp1048[1];struct Cyc_String_pa_PrintArg_struct _tmp1047;(_tmp1047.tag=0,((_tmp1047.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp931)),((_tmp1048[0]=& _tmp1047,Cyc_Tcutil_terr(loc,((_tmp1049="declare parameterized type %s before using",_tag_dyneither(_tmp1049,sizeof(char),43))),_tag_dyneither(_tmp1048,sizeof(void*),1)))))));}
return cvtenv;}
# 4656
goto _LL6FC;}}else{_LL6FF: _tmp943=_tmp942;_LL700:(int)_rethrow(_tmp943);}_LL6FC:;}};}
# 4658
_tmp92F=*adp;goto _LL6FB;}}else{_LL6FA: _tmp92F=*(_tmp92E.KnownAggr).val;_LL6FB: {
# 4660
struct Cyc_List_List*tvs=_tmp92F->tvs;
struct Cyc_List_List*ts=*_tmp79A;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Tvar*_tmp94A=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp94B=(void*)ts->hd;
# 4668
{struct _tuple0 _tmp104A;struct _tuple0 _tmp94C=(_tmp104A.f1=Cyc_Absyn_compress_kb(_tmp94A->kind),((_tmp104A.f2=_tmp94B,_tmp104A)));struct _tuple0 _tmp94D=_tmp94C;struct Cyc_Absyn_Tvar*_tmp94E;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp94D.f1)->tag == 1){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp94D.f2)->tag == 2){_LL702: _tmp94E=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp94D.f2)->f1;_LL703:
# 4670
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp94E);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp94E,1);
continue;}else{goto _LL704;}}else{_LL704: _LL705:
 goto _LL701;}_LL701:;}{
# 4675
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 4679
if(ts != 0){
const char*_tmp104E;void*_tmp104D[1];struct Cyc_String_pa_PrintArg_struct _tmp104C;(_tmp104C.tag=0,((_tmp104C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp92F->name)),((_tmp104D[0]=& _tmp104C,Cyc_Tcutil_terr(loc,((_tmp104E="too many parameters for type %s",_tag_dyneither(_tmp104E,sizeof(char),32))),_tag_dyneither(_tmp104D,sizeof(void*),1)))))));}
if(tvs != 0){
# 4683
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp104F;hidden_ts=((_tmp104F=_cycalloc(sizeof(*_tmp104F)),((_tmp104F->hd=e,((_tmp104F->tl=hidden_ts,_tmp104F))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);}
# 4690
*_tmp79A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp79A,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4692
if((allow_abs_aggr  && _tmp92F->impl == 0) && !
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,expected_kind))
# 4697
_tmp92F->expected_mem_kind=1;}}_LL6F7:;}
# 4700
goto _LL5F5;case 17: _LL61E: _tmp795=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp78E)->f1;_tmp796=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp78E)->f2;_tmp797=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp78E)->f3;_tmp798=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp78E)->f4;_LL61F: {
# 4703
struct Cyc_List_List*targs=*_tmp796;
# 4705
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmp954;_push_handler(& _tmp954);{int _tmp956=0;if(setjmp(_tmp954.handler))_tmp956=1;if(!_tmp956){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp795);;_pop_handler();}else{void*_tmp955=(void*)_exn_thrown;void*_tmp957=_tmp955;void*_tmp958;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp957)->tag == Cyc_Dict_Absent){_LL707: _LL708:
# 4708
{const char*_tmp1053;void*_tmp1052[1];struct Cyc_String_pa_PrintArg_struct _tmp1051;(_tmp1051.tag=0,((_tmp1051.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp795)),((_tmp1052[0]=& _tmp1051,Cyc_Tcutil_terr(loc,((_tmp1053="unbound typedef name %s",_tag_dyneither(_tmp1053,sizeof(char),24))),_tag_dyneither(_tmp1052,sizeof(void*),1)))))));}
return cvtenv;}else{_LL709: _tmp958=_tmp957;_LL70A:(int)_rethrow(_tmp958);}_LL706:;}};}
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
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{
struct _tuple16*_tmp1056;struct Cyc_List_List*_tmp1055;inst=((_tmp1055=_region_malloc(temp,sizeof(*_tmp1055)),((_tmp1055->hd=((_tmp1056=_region_malloc(temp,sizeof(*_tmp1056)),((_tmp1056->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1056->f2=(void*)ts->hd,_tmp1056)))))),((_tmp1055->tl=inst,_tmp1055))))));};}
# 4728
if(ts != 0){
const char*_tmp105A;void*_tmp1059[1];struct Cyc_String_pa_PrintArg_struct _tmp1058;(_tmp1058.tag=0,((_tmp1058.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp795)),((_tmp1059[0]=& _tmp1058,Cyc_Tcutil_terr(loc,((_tmp105A="too many parameters for typedef %s",_tag_dyneither(_tmp105A,sizeof(char),35))),_tag_dyneither(_tmp1059,sizeof(void*),1)))))));}
if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;
# 4733
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp105B;hidden_ts=((_tmp105B=_cycalloc(sizeof(*_tmp105B)),((_tmp105B->hd=e,((_tmp105B->tl=hidden_ts,_tmp105B))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);{
struct _tuple16*_tmp105E;struct Cyc_List_List*_tmp105D;inst=((_tmp105D=_cycalloc(sizeof(*_tmp105D)),((_tmp105D->hd=((_tmp105E=_cycalloc(sizeof(*_tmp105E)),((_tmp105E->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp105E->f2=e,_tmp105E)))))),((_tmp105D->tl=inst,_tmp105D))))));};}
# 4741
*_tmp796=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4743
if(td->defn != 0){
void*new_typ=
inst == 0?(void*)_check_null(td->defn):
 Cyc_Tcutil_rsubstitute(temp,inst,(void*)_check_null(td->defn));
*_tmp798=new_typ;}}
# 4750
_npop_handler(0);goto _LL5F5;
# 4716
;_pop_region(temp);};}case 22: _LL620: _LL621:
# 4751
 goto _LL623;case 21: _LL622: _LL623:
 goto _LL625;case 20: _LL624: _LL625:
 goto _LL5F5;case 15: _LL626: _tmp794=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp78E)->f1;_LL627:
# 4755
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp794,1,allow_abs_aggr);goto _LL5F5;case 16: _LL628: _tmp792=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp78E)->f1;_tmp793=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp78E)->f2;_LL629:
# 4758
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp792,0,0);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp793,1,0);
goto _LL5F5;case 23: _LL62A: _tmp791=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp78E)->f1;_LL62B:
# 4762
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp791,1,1);goto _LL5F5;case 25: _LL62C: _tmp790=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp78E)->f1;_LL62D:
# 4764
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp790,1,1);goto _LL5F5;default: _LL62E: _tmp78F=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp78E)->f1;_LL62F:
# 4766
 for(0;_tmp78F != 0;_tmp78F=_tmp78F->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmp78F->hd,1,1);}
goto _LL5F5;}_LL5F5:;}
# 4770
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind)){
const char*_tmp1064;void*_tmp1063[3];struct Cyc_String_pa_PrintArg_struct _tmp1062;struct Cyc_String_pa_PrintArg_struct _tmp1061;struct Cyc_String_pa_PrintArg_struct _tmp1060;(_tmp1060.tag=0,((_tmp1060.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(expected_kind)),((_tmp1061.tag=0,((_tmp1061.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmp1062.tag=0,((_tmp1062.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1063[0]=& _tmp1062,((_tmp1063[1]=& _tmp1061,((_tmp1063[2]=& _tmp1060,Cyc_Tcutil_terr(loc,((_tmp1064="type %s has kind %s but as used here needs kind %s",_tag_dyneither(_tmp1064,sizeof(char),51))),_tag_dyneither(_tmp1063,sizeof(void*),3)))))))))))))))))));}
# 4774
return cvtenv;}
# 4782
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){
# 4784
{void*_tmp96A=e->r;void*_tmp96B=_tmp96A;struct Cyc_Absyn_Exp*_tmp96C;void*_tmp96D;void*_tmp96E;void*_tmp96F;void*_tmp970;struct Cyc_Absyn_Exp*_tmp971;struct Cyc_Absyn_Exp*_tmp972;struct Cyc_Absyn_Exp*_tmp973;struct Cyc_Absyn_Exp*_tmp974;struct Cyc_Absyn_Exp*_tmp975;struct Cyc_Absyn_Exp*_tmp976;struct Cyc_Absyn_Exp*_tmp977;struct Cyc_Absyn_Exp*_tmp978;struct Cyc_Absyn_Exp*_tmp979;struct Cyc_Absyn_Exp*_tmp97A;struct Cyc_List_List*_tmp97B;switch(*((int*)_tmp96B)){case 0: _LL70C: _LL70D:
 goto _LL70F;case 31: _LL70E: _LL70F:
 goto _LL711;case 32: _LL710: _LL711:
 goto _LL713;case 1: _LL712: _LL713:
 goto _LL70B;case 2: _LL714: _tmp97B=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp96B)->f2;_LL715:
# 4790
 for(0;_tmp97B != 0;_tmp97B=_tmp97B->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmp97B->hd,te,cvtenv);}
goto _LL70B;case 5: _LL716: _tmp978=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp96B)->f1;_tmp979=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp96B)->f2;_tmp97A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp96B)->f3;_LL717:
# 4794
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp978,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp979,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp97A,te,cvtenv);
goto _LL70B;case 6: _LL718: _tmp976=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp96B)->f1;_tmp977=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp96B)->f2;_LL719:
 _tmp974=_tmp976;_tmp975=_tmp977;goto _LL71B;case 7: _LL71A: _tmp974=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp96B)->f1;_tmp975=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp96B)->f2;_LL71B:
 _tmp972=_tmp974;_tmp973=_tmp975;goto _LL71D;case 8: _LL71C: _tmp972=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp96B)->f1;_tmp973=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp96B)->f2;_LL71D:
# 4801
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp972,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp973,te,cvtenv);
goto _LL70B;case 13: _LL71E: _tmp970=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp96B)->f1;_tmp971=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp96B)->f2;_LL71F:
# 4805
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp971,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp970,1,0);
goto _LL70B;case 18: _LL720: _tmp96F=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp96B)->f1;_LL721:
 _tmp96E=_tmp96F;goto _LL723;case 16: _LL722: _tmp96E=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp96B)->f1;_LL723:
# 4810
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp96E,1,0);
goto _LL70B;case 38: _LL724: _tmp96D=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp96B)->f1;_LL725:
# 4813
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp96D,1,0);
goto _LL70B;case 17: _LL726: _tmp96C=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp96B)->f1;_LL727:
# 4816
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp96C,te,cvtenv);
goto _LL70B;default: _LL728: _LL729: {
# 4819
const char*_tmp1067;void*_tmp1066;(_tmp1066=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1067="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",_tag_dyneither(_tmp1067,sizeof(char),66))),_tag_dyneither(_tmp1066,sizeof(void*),0)));}}_LL70B:;}
# 4821
return cvtenv;}
# 4824
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,int allow_abs_aggr,void*t){
# 4830
struct Cyc_List_List*_tmp97E=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1,allow_abs_aggr);
# 4833
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
struct _tuple28 _tmp97F=*((struct _tuple28*)vs->hd);struct Cyc_Absyn_Tvar*_tmp981;struct _tuple28 _tmp980=_tmp97F;_tmp981=_tmp980.f1;
cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(_tmp97E,_tmp981);}}
# 4841
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmp982=*((struct _tuple29*)evs->hd);void*_tmp984;struct _tuple29 _tmp983=_tmp982;_tmp984=_tmp983.f1;{
void*_tmp985=Cyc_Tcutil_compress(_tmp984);void*_tmp986=_tmp985;struct Cyc_Core_Opt**_tmp987;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp986)->tag == 1){_LL72B: _tmp987=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp986)->f4;_LL72C:
# 4845
 if(*_tmp987 == 0){
struct Cyc_Core_Opt*_tmp1068;*_tmp987=((_tmp1068=_cycalloc(sizeof(*_tmp1068)),((_tmp1068->v=_tmp97E,_tmp1068))));}else{
# 4849
struct Cyc_List_List*_tmp989=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp987))->v;
struct Cyc_List_List*_tmp98A=0;
for(0;_tmp989 != 0;_tmp989=_tmp989->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp97E,(struct Cyc_Absyn_Tvar*)_tmp989->hd)){
struct Cyc_List_List*_tmp1069;_tmp98A=((_tmp1069=_cycalloc(sizeof(*_tmp1069)),((_tmp1069->hd=(struct Cyc_Absyn_Tvar*)_tmp989->hd,((_tmp1069->tl=_tmp98A,_tmp1069))))));}}{
struct Cyc_Core_Opt*_tmp106A;*_tmp987=((_tmp106A=_cycalloc(sizeof(*_tmp106A)),((_tmp106A->v=_tmp98A,_tmp106A))));};}
# 4856
goto _LL72A;}else{_LL72D: _LL72E:
 goto _LL72A;}_LL72A:;};}}
# 4860
return cvt;}
# 4866
void Cyc_Tcutil_check_free_evars(struct Cyc_List_List*free_evars,void*in_t,unsigned int loc){
# 4868
for(0;free_evars != 0;free_evars=free_evars->tl){
void*e=(void*)free_evars->hd;
struct Cyc_Absyn_Kind*_tmp98D=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind*_tmp98E=_tmp98D;switch(((struct Cyc_Absyn_Kind*)_tmp98E)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp98E)->aliasqual){case Cyc_Absyn_Unique: _LL730: _LL731:
# 4872
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp106D;void*_tmp106C;(_tmp106C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp106D="can't unify evar with unique region!",_tag_dyneither(_tmp106D,sizeof(char),37))),_tag_dyneither(_tmp106C,sizeof(void*),0)));}
goto _LL72F;case Cyc_Absyn_Aliasable: _LL732: _LL733:
 goto _LL735;default: _LL734: _LL735:
# 4877
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp1070;void*_tmp106F;(_tmp106F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1070="can't unify evar with heap!",_tag_dyneither(_tmp1070,sizeof(char),28))),_tag_dyneither(_tmp106F,sizeof(void*),0)));}
goto _LL72F;}case Cyc_Absyn_EffKind: _LL736: _LL737:
# 4880
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*_tmp1073;void*_tmp1072;(_tmp1072=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1073="can't unify evar with {}!",_tag_dyneither(_tmp1073,sizeof(char),26))),_tag_dyneither(_tmp1072,sizeof(void*),0)));}
goto _LL72F;default: _LL738: _LL739:
# 4883
{const char*_tmp1078;void*_tmp1077[2];struct Cyc_String_pa_PrintArg_struct _tmp1076;struct Cyc_String_pa_PrintArg_struct _tmp1075;(_tmp1075.tag=0,((_tmp1075.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(in_t)),((_tmp1076.tag=0,((_tmp1076.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e)),((_tmp1077[0]=& _tmp1076,((_tmp1077[1]=& _tmp1075,Cyc_Tcutil_terr(loc,((_tmp1078="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(_tmp1078,sizeof(char),52))),_tag_dyneither(_tmp1077,sizeof(void*),2)))))))))))));}
goto _LL72F;}_LL72F:;}}
# 4895
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_List_List*_tmp999=Cyc_Tcenv_lookup_type_vars(te);
struct Cyc_Absyn_Kind*expected_kind=generalize_evars?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
struct _RegionHandle _tmp99A=_new_region("temp");struct _RegionHandle*temp=& _tmp99A;_push_region(temp);
{struct Cyc_Tcutil_CVTEnv _tmp1079;struct Cyc_Tcutil_CVTEnv _tmp99B=Cyc_Tcutil_check_valid_type(loc,te,(
(_tmp1079.r=temp,((_tmp1079.kind_env=_tmp999,((_tmp1079.free_vars=0,((_tmp1079.free_evars=0,((_tmp1079.generalize_evars=generalize_evars,((_tmp1079.fn_result=0,_tmp1079)))))))))))),expected_kind,1,t);
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
if(!found){
struct Cyc_List_List*_tmp107A;_tmp99E=((_tmp107A=_region_malloc(temp,sizeof(*_tmp107A)),((_tmp107A->hd=(struct Cyc_Absyn_Tvar*)_tmp99F->hd,((_tmp107A->tl=_tmp99E,_tmp107A))))));}}}
# 4918
_tmp99C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp99E);}
# 4924
{struct Cyc_List_List*x=_tmp99C;for(0;x != 0;x=x->tl){
void*_tmp9A3=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);void*_tmp9A4=_tmp9A3;enum Cyc_Absyn_AliasQual _tmp9A5;struct Cyc_Core_Opt**_tmp9A6;struct Cyc_Absyn_Kind*_tmp9A7;struct Cyc_Core_Opt**_tmp9A8;struct Cyc_Core_Opt**_tmp9A9;struct Cyc_Core_Opt**_tmp9AA;struct Cyc_Core_Opt**_tmp9AB;struct Cyc_Core_Opt**_tmp9AC;struct Cyc_Core_Opt**_tmp9AD;switch(*((int*)_tmp9A4)){case 1: _LL73B: _tmp9AD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp9A4)->f1;_LL73C:
 _tmp9AC=_tmp9AD;goto _LL73E;case 2: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9A4)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9A4)->f2)->aliasqual == Cyc_Absyn_Top){_LL73D: _tmp9AC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9A4)->f1;_LL73E:
 _tmp9AB=_tmp9AC;goto _LL740;}else{goto _LL747;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9A4)->f2)->aliasqual){case Cyc_Absyn_Top: _LL73F: _tmp9AB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9A4)->f1;_LL740:
 _tmp9AA=_tmp9AB;goto _LL742;case Cyc_Absyn_Aliasable: _LL741: _tmp9AA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9A4)->f1;_LL742:
# 4930
*_tmp9AA=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL73A;default: _LL743: _tmp9A9=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9A4)->f1;_LL744:
# 4932
*_tmp9A9=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL73A;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9A4)->f2)->aliasqual == Cyc_Absyn_Top){_LL745: _tmp9A8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9A4)->f1;_LL746:
# 4934
*_tmp9A8=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL73A;}else{goto _LL747;}default: _LL747: _tmp9A6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9A4)->f1;_tmp9A7=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9A4)->f2;_LL748:
# 4936
*_tmp9A6=Cyc_Tcutil_kind_to_bound_opt(_tmp9A7);goto _LL73A;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp9A4)->f1)->kind == Cyc_Absyn_MemKind){_LL749: _tmp9A5=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp9A4)->f1)->aliasqual;_LL74A:
# 4938
{const char*_tmp1081;void*_tmp1080[2];struct Cyc_String_pa_PrintArg_struct _tmp107F;struct Cyc_Absyn_Kind*_tmp107E;struct Cyc_String_pa_PrintArg_struct _tmp107D;(_tmp107D.tag=0,((_tmp107D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(((_tmp107E=_cycalloc_atomic(sizeof(*_tmp107E)),((_tmp107E->kind=Cyc_Absyn_MemKind,((_tmp107E->aliasqual=_tmp9A5,_tmp107E)))))))),((_tmp107F.tag=0,((_tmp107F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd)),((_tmp1080[0]=& _tmp107F,((_tmp1080[1]=& _tmp107D,Cyc_Tcutil_terr(loc,((_tmp1081="type variable %s cannot have kind %s",_tag_dyneither(_tmp1081,sizeof(char),37))),_tag_dyneither(_tmp1080,sizeof(void*),2)))))))))))));}
goto _LL73A;}else{_LL74B: _LL74C:
 goto _LL73A;}}_LL73A:;}}
# 4945
if(_tmp99C != 0  || _tmp99D != 0){
{void*_tmp9B3=Cyc_Tcutil_compress(t);void*_tmp9B4=_tmp9B3;struct Cyc_List_List**_tmp9B5;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9B4)->tag == 9){_LL74E: _tmp9B5=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9B4)->f1).tvars;_LL74F:
# 4948
 if(*_tmp9B5 == 0){
# 4950
*_tmp9B5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmp99C);
_tmp99C=0;}
# 4953
goto _LL74D;}else{_LL750: _LL751:
 goto _LL74D;}_LL74D:;}
# 4956
if(_tmp99C != 0){
const char*_tmp1085;void*_tmp1084[1];struct Cyc_String_pa_PrintArg_struct _tmp1083;(_tmp1083.tag=0,((_tmp1083.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmp99C->hd)->name),((_tmp1084[0]=& _tmp1083,Cyc_Tcutil_terr(loc,((_tmp1085="unbound type variable %s",_tag_dyneither(_tmp1085,sizeof(char),25))),_tag_dyneither(_tmp1084,sizeof(void*),1)))))));}
Cyc_Tcutil_check_free_evars(_tmp99D,t,loc);}}
# 4900
;_pop_region(temp);}
# 4967
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 4970
Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);
{void*_tmp9BA=Cyc_Tcutil_compress(t);void*_tmp9BB=_tmp9BA;struct Cyc_List_List*_tmp9BC;void*_tmp9BD;struct Cyc_Absyn_Tqual _tmp9BE;void*_tmp9BF;struct Cyc_List_List*_tmp9C0;struct Cyc_Absyn_Exp*_tmp9C1;struct Cyc_List_List*_tmp9C2;struct Cyc_Absyn_Exp*_tmp9C3;struct Cyc_List_List*_tmp9C4;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9BB)->tag == 9){_LL753: _tmp9BC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9BB)->f1).tvars;_tmp9BD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9BB)->f1).effect;_tmp9BE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9BB)->f1).ret_tqual;_tmp9BF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9BB)->f1).ret_typ;_tmp9C0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9BB)->f1).args;_tmp9C1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9BB)->f1).requires_clause;_tmp9C2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9BB)->f1).requires_relns;_tmp9C3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9BB)->f1).ensures_clause;_tmp9C4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9BB)->f1).ensures_relns;_LL754:
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
goto _LL752;}else{_LL755: _LL756: {
const char*_tmp1088;void*_tmp1087;(_tmp1087=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1088="check_fndecl_valid_type: not a FnType",_tag_dyneither(_tmp1088,sizeof(char),38))),_tag_dyneither(_tmp1087,sizeof(void*),0)));}}_LL752:;}{
# 4991
struct _RegionHandle _tmp9C8=_new_region("r");struct _RegionHandle*r=& _tmp9C8;_push_region(r);{
const char*_tmp1089;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _dyneither_ptr*(*)(struct _tuple10*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,(
(_tmp1089="function declaration has repeated parameter",_tag_dyneither(_tmp1089,sizeof(char),44))));}
# 4996
fd->cached_typ=t;
# 4991
;_pop_region(r);};}
# 5001
void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,int allow_abs_aggr,void*t){
# 5007
struct _RegionHandle _tmp9CA=_new_region("r");struct _RegionHandle*r=& _tmp9CA;_push_region(r);
{struct Cyc_Tcutil_CVTEnv _tmp108A;struct Cyc_Tcutil_CVTEnv _tmp9CB=Cyc_Tcutil_check_valid_type(loc,te,(
(_tmp108A.r=r,((_tmp108A.kind_env=bound_tvars,((_tmp108A.free_vars=0,((_tmp108A.free_evars=0,((_tmp108A.generalize_evars=0,((_tmp108A.fn_result=0,_tmp108A)))))))))))),expected_kind,allow_abs_aggr,t);
# 5011
struct Cyc_List_List*_tmp9CC=Cyc_Tcutil_remove_bound_tvars(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmp9CB.free_vars),bound_tvars);
# 5013
struct Cyc_List_List*_tmp9CD=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmp9CB.free_evars);
{struct Cyc_List_List*fs=_tmp9CC;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmp9CE=((struct Cyc_Absyn_Tvar*)fs->hd)->name;
const char*_tmp108F;void*_tmp108E[2];struct Cyc_String_pa_PrintArg_struct _tmp108D;struct Cyc_String_pa_PrintArg_struct _tmp108C;(_tmp108C.tag=0,((_tmp108C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp108D.tag=0,((_tmp108D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp9CE),((_tmp108E[0]=& _tmp108D,((_tmp108E[1]=& _tmp108C,Cyc_Tcutil_terr(loc,((_tmp108F="unbound type variable %s in type %s",_tag_dyneither(_tmp108F,sizeof(char),36))),_tag_dyneither(_tmp108E,sizeof(void*),2)))))))))))));}}
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
if(cmp(vs->hd,vs2->hd)== 0){
const char*_tmp1094;void*_tmp1093[2];struct Cyc_String_pa_PrintArg_struct _tmp1092;struct Cyc_String_pa_PrintArg_struct _tmp1091;(_tmp1091.tag=0,((_tmp1091.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string(vs->hd)),((_tmp1092.tag=0,((_tmp1092.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp1093[0]=& _tmp1092,((_tmp1093[1]=& _tmp1091,Cyc_Tcutil_terr(loc,((_tmp1094="%s: %s",_tag_dyneither(_tmp1094,sizeof(char),7))),_tag_dyneither(_tmp1093,sizeof(void*),2)))))))))))));}}}}
# 5043
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s){
return*s;}
# 5047
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr msg){
((void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 5051
void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
const char*_tmp1095;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,((_tmp1095="duplicate type variable",_tag_dyneither(_tmp1095,sizeof(char),24))));}struct _tuple32{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple33{struct Cyc_List_List*f1;void*f2;};struct _tuple34{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 5065 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 5070
struct _RegionHandle _tmp9D9=_new_region("temp");struct _RegionHandle*temp=& _tmp9D9;_push_region(temp);
# 5074
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
const char*_tmp1096;if(Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,((_tmp1096="",_tag_dyneither(_tmp1096,sizeof(char),1))))!= 0){
struct _tuple32*_tmp1099;struct Cyc_List_List*_tmp1098;fields=((_tmp1098=_region_malloc(temp,sizeof(*_tmp1098)),((_tmp1098->hd=((_tmp1099=_region_malloc(temp,sizeof(*_tmp1099)),((_tmp1099->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,((_tmp1099->f2=0,_tmp1099)))))),((_tmp1098->tl=fields,_tmp1098))))));}}}
# 5079
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 5081
const char*_tmp109B;const char*_tmp109A;struct _dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?(_tmp109B="struct",_tag_dyneither(_tmp109B,sizeof(char),7)):((_tmp109A="union",_tag_dyneither(_tmp109A,sizeof(char),6)));
# 5084
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple33*_tmp9DD=(struct _tuple33*)des->hd;struct Cyc_List_List*_tmp9DF;void*_tmp9E0;struct _tuple33*_tmp9DE=_tmp9DD;_tmp9DF=_tmp9DE->f1;_tmp9E0=_tmp9DE->f2;
if(_tmp9DF == 0){
# 5089
struct Cyc_List_List*_tmp9E1=fields;
for(0;_tmp9E1 != 0;_tmp9E1=_tmp9E1->tl){
if(!(*((struct _tuple32*)_tmp9E1->hd)).f2){
(*((struct _tuple32*)_tmp9E1->hd)).f2=1;
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp10A1;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp10A0;struct Cyc_List_List*_tmp109F;(*((struct _tuple33*)des->hd)).f1=((_tmp109F=_cycalloc(sizeof(*_tmp109F)),((_tmp109F->hd=(void*)((_tmp10A1=_cycalloc(sizeof(*_tmp10A1)),((_tmp10A1[0]=((_tmp10A0.tag=1,((_tmp10A0.f1=((*((struct _tuple32*)_tmp9E1->hd)).f1)->name,_tmp10A0)))),_tmp10A1)))),((_tmp109F->tl=0,_tmp109F))))));}
{struct _tuple34*_tmp10A4;struct Cyc_List_List*_tmp10A3;ans=((_tmp10A3=_region_malloc(rgn,sizeof(*_tmp10A3)),((_tmp10A3->hd=((_tmp10A4=_region_malloc(rgn,sizeof(*_tmp10A4)),((_tmp10A4->f1=(*((struct _tuple32*)_tmp9E1->hd)).f1,((_tmp10A4->f2=_tmp9E0,_tmp10A4)))))),((_tmp10A3->tl=ans,_tmp10A3))))));}
break;}}
# 5097
if(_tmp9E1 == 0){
const char*_tmp10A8;void*_tmp10A7[1];struct Cyc_String_pa_PrintArg_struct _tmp10A6;(_tmp10A6.tag=0,((_tmp10A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp10A7[0]=& _tmp10A6,Cyc_Tcutil_terr(loc,((_tmp10A8="too many arguments to %s",_tag_dyneither(_tmp10A8,sizeof(char),25))),_tag_dyneither(_tmp10A7,sizeof(void*),1)))))));}}else{
if(_tmp9DF->tl != 0){
# 5101
const char*_tmp10AB;void*_tmp10AA;(_tmp10AA=0,Cyc_Tcutil_terr(loc,((_tmp10AB="multiple designators are not yet supported",_tag_dyneither(_tmp10AB,sizeof(char),43))),_tag_dyneither(_tmp10AA,sizeof(void*),0)));}else{
# 5104
void*_tmp9EC=(void*)_tmp9DF->hd;void*_tmp9ED=_tmp9EC;struct _dyneither_ptr*_tmp9EE;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp9ED)->tag == 0){_LL758: _LL759:
# 5106
{const char*_tmp10AF;void*_tmp10AE[1];struct Cyc_String_pa_PrintArg_struct _tmp10AD;(_tmp10AD.tag=0,((_tmp10AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp10AE[0]=& _tmp10AD,Cyc_Tcutil_terr(loc,((_tmp10AF="array designator used in argument to %s",_tag_dyneither(_tmp10AF,sizeof(char),40))),_tag_dyneither(_tmp10AE,sizeof(void*),1)))))));}
goto _LL757;}else{_LL75A: _tmp9EE=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp9ED)->f1;_LL75B: {
# 5109
struct Cyc_List_List*_tmp9F2=fields;
for(0;_tmp9F2 != 0;_tmp9F2=_tmp9F2->tl){
if(Cyc_strptrcmp(_tmp9EE,((*((struct _tuple32*)_tmp9F2->hd)).f1)->name)== 0){
if((*((struct _tuple32*)_tmp9F2->hd)).f2){
const char*_tmp10B3;void*_tmp10B2[1];struct Cyc_String_pa_PrintArg_struct _tmp10B1;(_tmp10B1.tag=0,((_tmp10B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp9EE),((_tmp10B2[0]=& _tmp10B1,Cyc_Tcutil_terr(loc,((_tmp10B3="member %s has already been used as an argument",_tag_dyneither(_tmp10B3,sizeof(char),47))),_tag_dyneither(_tmp10B2,sizeof(void*),1)))))));}
(*((struct _tuple32*)_tmp9F2->hd)).f2=1;
{struct _tuple34*_tmp10B6;struct Cyc_List_List*_tmp10B5;ans=((_tmp10B5=_region_malloc(rgn,sizeof(*_tmp10B5)),((_tmp10B5->hd=((_tmp10B6=_region_malloc(rgn,sizeof(*_tmp10B6)),((_tmp10B6->f1=(*((struct _tuple32*)_tmp9F2->hd)).f1,((_tmp10B6->f2=_tmp9E0,_tmp10B6)))))),((_tmp10B5->tl=ans,_tmp10B5))))));}
break;}}
# 5118
if(_tmp9F2 == 0){
const char*_tmp10BA;void*_tmp10B9[1];struct Cyc_String_pa_PrintArg_struct _tmp10B8;(_tmp10B8.tag=0,((_tmp10B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp9EE),((_tmp10B9[0]=& _tmp10B8,Cyc_Tcutil_terr(loc,((_tmp10BA="bad field designator %s",_tag_dyneither(_tmp10BA,sizeof(char),24))),_tag_dyneither(_tmp10B9,sizeof(void*),1)))))));}
goto _LL757;}}_LL757:;}}}
# 5123
if(aggr_kind == Cyc_Absyn_StructA)
# 5125
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple32*)fields->hd)).f2){
{const char*_tmp10BD;void*_tmp10BC;(_tmp10BC=0,Cyc_Tcutil_terr(loc,((_tmp10BD="too few arguments to struct",_tag_dyneither(_tmp10BD,sizeof(char),28))),_tag_dyneither(_tmp10BC,sizeof(void*),0)));}
break;}}else{
# 5132
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple32*)fields->hd)).f2){
if(found){const char*_tmp10C0;void*_tmp10BF;(_tmp10BF=0,Cyc_Tcutil_terr(loc,((_tmp10C0="only one member of a union is allowed",_tag_dyneither(_tmp10C0,sizeof(char),38))),_tag_dyneither(_tmp10BF,sizeof(void*),0)));}
found=1;}}
# 5139
if(!found){const char*_tmp10C3;void*_tmp10C2;(_tmp10C2=0,Cyc_Tcutil_terr(loc,((_tmp10C3="missing member for union",_tag_dyneither(_tmp10C3,sizeof(char),25))),_tag_dyneither(_tmp10C2,sizeof(void*),0)));}}{
# 5142
struct Cyc_List_List*_tmpA01=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0);return _tmpA01;};};}
# 5074
;_pop_region(temp);}
# 5145
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmpA04=Cyc_Tcutil_compress(t);void*_tmpA05=_tmpA04;void*_tmpA06;union Cyc_Absyn_Constraint*_tmpA07;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA05)->tag == 5){_LL75D: _tmpA06=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA05)->f1).elt_typ;_tmpA07=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA05)->f1).ptr_atts).bounds;_LL75E: {
# 5148
void*_tmpA08=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpA07);void*_tmpA09=_tmpA08;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpA09)->tag == 0){_LL762: _LL763:
# 5150
*elt_typ_dest=_tmpA06;
return 1;}else{_LL764: _LL765:
 return 0;}_LL761:;}}else{_LL75F: _LL760:
# 5154
 return 0;}_LL75C:;}
# 5158
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmpA0A=Cyc_Tcutil_compress(t);void*_tmpA0B=_tmpA0A;void*_tmpA0C;union Cyc_Absyn_Constraint*_tmpA0D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA0B)->tag == 5){_LL767: _tmpA0C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA0B)->f1).elt_typ;_tmpA0D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA0B)->f1).ptr_atts).zero_term;_LL768:
# 5161
*elt_typ_dest=_tmpA0C;
return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpA0D);}else{_LL769: _LL76A:
 return 0;}_LL766:;}
# 5169
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type){
# 5171
void*_tmpA0E=Cyc_Tcutil_compress(t);void*_tmpA0F=_tmpA0E;void*_tmpA10;struct Cyc_Absyn_Tqual _tmpA11;struct Cyc_Absyn_Exp*_tmpA12;union Cyc_Absyn_Constraint*_tmpA13;void*_tmpA14;union Cyc_Absyn_Constraint*_tmpA15;union Cyc_Absyn_Constraint*_tmpA16;switch(*((int*)_tmpA0F)){case 5: _LL76C: _tmpA14=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA0F)->f1).elt_typ;_tmpA15=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA0F)->f1).ptr_atts).bounds;_tmpA16=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA0F)->f1).ptr_atts).zero_term;_LL76D:
# 5173
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpA16)){
*ptr_type=t;
*elt_type=_tmpA14;
{void*_tmpA17=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmpA15);void*_tmpA18=_tmpA17;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpA18)->tag == 0){_LL773: _LL774:
*is_dyneither=1;goto _LL772;}else{_LL775: _LL776:
*is_dyneither=0;goto _LL772;}_LL772:;}
# 5180
return 1;}else{
return 0;}case 8: _LL76E: _tmpA10=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA0F)->f1).elt_type;_tmpA11=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA0F)->f1).tq;_tmpA12=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA0F)->f1).num_elts;_tmpA13=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA0F)->f1).zero_term;_LL76F:
# 5183
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpA13)){
*elt_type=_tmpA10;
*is_dyneither=0;
*ptr_type=Cyc_Tcutil_promote_array(t,(void*)& Cyc_Absyn_HeapRgn_val,0);
return 1;}else{
return 0;}default: _LL770: _LL771:
 return 0;}_LL76B:;}
# 5196
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){
# 5198
void*_tmpA19=e1->r;void*_tmpA1A=_tmpA19;struct Cyc_Absyn_Exp*_tmpA1B;struct Cyc_Absyn_Exp*_tmpA1C;struct Cyc_Absyn_Exp*_tmpA1D;struct Cyc_Absyn_Exp*_tmpA1E;struct Cyc_Absyn_Exp*_tmpA1F;struct Cyc_Absyn_Exp*_tmpA20;switch(*((int*)_tmpA1A)){case 13: _LL778: _LL779: {
# 5200
const char*_tmp10C7;void*_tmp10C6[1];struct Cyc_String_pa_PrintArg_struct _tmp10C5;(_tmp10C5.tag=0,((_tmp10C5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp10C6[0]=& _tmp10C5,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10C7="we have a cast in a lhs:  %s",_tag_dyneither(_tmp10C7,sizeof(char),29))),_tag_dyneither(_tmp10C6,sizeof(void*),1)))))));}case 19: _LL77A: _tmpA20=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpA1A)->f1;_LL77B:
 _tmpA1F=_tmpA20;goto _LL77D;case 22: _LL77C: _tmpA1F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA1A)->f1;_LL77D:
# 5203
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpA1F->topt),ptr_type,is_dyneither,elt_type);case 21: _LL77E: _tmpA1E=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA1A)->f1;_LL77F:
 _tmpA1D=_tmpA1E;goto _LL781;case 20: _LL780: _tmpA1D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA1A)->f1;_LL781:
# 5207
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpA1D->topt),ptr_type,is_dyneither,elt_type)){
const char*_tmp10CB;void*_tmp10CA[1];struct Cyc_String_pa_PrintArg_struct _tmp10C9;(_tmp10C9.tag=0,((_tmp10C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1)),((_tmp10CA[0]=& _tmp10C9,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10CB="found zero pointer aggregate member assignment: %s",_tag_dyneither(_tmp10CB,sizeof(char),51))),_tag_dyneither(_tmp10CA,sizeof(void*),1)))))));}
return 0;case 12: _LL782: _tmpA1C=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpA1A)->f1;_LL783:
 _tmpA1B=_tmpA1C;goto _LL785;case 11: _LL784: _tmpA1B=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpA1A)->f1;_LL785:
# 5213
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpA1B->topt),ptr_type,is_dyneither,elt_type)){
const char*_tmp10CF;void*_tmp10CE[1];struct Cyc_String_pa_PrintArg_struct _tmp10CD;(_tmp10CD.tag=0,((_tmp10CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1)),((_tmp10CE[0]=& _tmp10CD,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10CF="found zero pointer instantiate/noinstantiate: %s",_tag_dyneither(_tmp10CF,sizeof(char),49))),_tag_dyneither(_tmp10CE,sizeof(void*),1)))))));}
return 0;case 1: _LL786: _LL787:
 return 0;default: _LL788: _LL789: {
# 5219
const char*_tmp10D3;void*_tmp10D2[1];struct Cyc_String_pa_PrintArg_struct _tmp10D1;(_tmp10D1.tag=0,((_tmp10D1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp10D2[0]=& _tmp10D1,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10D3="found bad lhs in is_zero_ptr_deref: %s",_tag_dyneither(_tmp10D3,sizeof(char),39))),_tag_dyneither(_tmp10D2,sizeof(void*),1)))))));}}_LL777:;}
# 5223
int Cyc_Tcutil_is_tagged_pointer_typ(void*t){
void*ignore=(void*)& Cyc_Absyn_VoidType_val;
return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
# 5234
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
# 5237
void*_tmpA2D=Cyc_Tcutil_compress(r);void*_tmpA2E=_tmpA2D;struct Cyc_Absyn_Tvar*_tmpA2F;enum Cyc_Absyn_KindQual _tmpA30;enum Cyc_Absyn_AliasQual _tmpA31;switch(*((int*)_tmpA2E)){case 22: _LL78B: _LL78C:
 return !must_be_unique;case 21: _LL78D: _LL78E:
 return 1;case 17: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpA2E)->f3 != 0){if(((struct Cyc_Absyn_Typedefdecl*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpA2E)->f3)->kind != 0){if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpA2E)->f4 == 0){_LL78F: _tmpA30=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpA2E)->f3)->kind)->v)->kind;_tmpA31=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpA2E)->f3)->kind)->v)->aliasqual;_LL790:
# 5241
 return _tmpA30 == Cyc_Absyn_RgnKind  && (_tmpA31 == Cyc_Absyn_Unique  || _tmpA31 == Cyc_Absyn_Top  && !must_be_unique);}else{goto _LL793;}}else{goto _LL793;}}else{goto _LL793;}case 2: _LL791: _tmpA2F=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA2E)->f1;_LL792: {
# 5245
struct Cyc_Absyn_Kind*_tmpA32=Cyc_Tcutil_tvar_kind(_tmpA2F,& Cyc_Tcutil_rk);enum Cyc_Absyn_KindQual _tmpA34;enum Cyc_Absyn_AliasQual _tmpA35;struct Cyc_Absyn_Kind*_tmpA33=_tmpA32;_tmpA34=_tmpA33->kind;_tmpA35=_tmpA33->aliasqual;
if(_tmpA34 == Cyc_Absyn_RgnKind  && (_tmpA35 == Cyc_Absyn_Unique  || _tmpA35 == Cyc_Absyn_Top  && !must_be_unique)){
void*_tmpA36=Cyc_Absyn_compress_kb(_tmpA2F->kind);void*_tmpA37=_tmpA36;struct Cyc_Core_Opt**_tmpA38;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA37)->tag == 2){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA37)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA37)->f2)->aliasqual == Cyc_Absyn_Top){_LL796: _tmpA38=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA37)->f1;_LL797:
# 5249
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp10D9;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp10D8;struct Cyc_Core_Opt*_tmp10D7;*_tmpA38=((_tmp10D7=_cycalloc(sizeof(*_tmp10D7)),((_tmp10D7->v=(void*)((_tmp10D9=_cycalloc(sizeof(*_tmp10D9)),((_tmp10D9[0]=((_tmp10D8.tag=2,((_tmp10D8.f1=0,((_tmp10D8.f2=& Cyc_Tcutil_rk,_tmp10D8)))))),_tmp10D9)))),_tmp10D7))));}
return 0;}else{goto _LL798;}}else{goto _LL798;}}else{_LL798: _LL799:
 return 1;}_LL795:;}
# 5254
return 0;}default: _LL793: _LL794:
 return 0;}_LL78A:;}
# 5261
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmpA3C=Cyc_Tcutil_compress(t);void*_tmpA3D=_tmpA3C;struct Cyc_Absyn_Tvar*_tmpA3E;void*_tmpA3F;switch(*((int*)_tmpA3D)){case 5: _LL79B: _tmpA3F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA3D)->f1).ptr_atts).rgn;_LL79C:
# 5264
 return Cyc_Tcutil_is_noalias_region(_tmpA3F,must_be_unique);case 2: _LL79D: _tmpA3E=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA3D)->f1;_LL79E: {
# 5266
struct Cyc_Absyn_Kind*_tmpA40=Cyc_Tcutil_tvar_kind(_tmpA3E,& Cyc_Tcutil_bk);enum Cyc_Absyn_KindQual _tmpA42;enum Cyc_Absyn_AliasQual _tmpA43;struct Cyc_Absyn_Kind*_tmpA41=_tmpA40;_tmpA42=_tmpA41->kind;_tmpA43=_tmpA41->aliasqual;{
enum Cyc_Absyn_KindQual _tmpA44=_tmpA42;switch(_tmpA44){case Cyc_Absyn_BoxKind: _LL7A2: _LL7A3:
 goto _LL7A5;case Cyc_Absyn_AnyKind: _LL7A4: _LL7A5: goto _LL7A7;case Cyc_Absyn_MemKind: _LL7A6: _LL7A7:
 if(_tmpA43 == Cyc_Absyn_Unique  || _tmpA43 == Cyc_Absyn_Top  && !must_be_unique){
void*_tmpA45=Cyc_Absyn_compress_kb(_tmpA3E->kind);void*_tmpA46=_tmpA45;struct Cyc_Core_Opt**_tmpA47;enum Cyc_Absyn_KindQual _tmpA48;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA46)->tag == 2){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA46)->f2)->aliasqual == Cyc_Absyn_Top){_LL7AB: _tmpA47=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA46)->f1;_tmpA48=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA46)->f2)->kind;_LL7AC:
# 5272
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp10E3;struct Cyc_Absyn_Kind*_tmp10E2;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp10E1;struct Cyc_Core_Opt*_tmp10E0;*_tmpA47=((_tmp10E0=_cycalloc(sizeof(*_tmp10E0)),((_tmp10E0->v=(void*)((_tmp10E3=_cycalloc(sizeof(*_tmp10E3)),((_tmp10E3[0]=((_tmp10E1.tag=2,((_tmp10E1.f1=0,((_tmp10E1.f2=((_tmp10E2=_cycalloc_atomic(sizeof(*_tmp10E2)),((_tmp10E2->kind=_tmpA48,((_tmp10E2->aliasqual=Cyc_Absyn_Aliasable,_tmp10E2)))))),_tmp10E1)))))),_tmp10E3)))),_tmp10E0))));}
return 0;}else{goto _LL7AD;}}else{_LL7AD: _LL7AE:
# 5276
 return 1;}_LL7AA:;}
# 5279
return 0;default: _LL7A8: _LL7A9:
 return 0;}_LL7A1:;};}default: _LL79F: _LL7A0:
# 5282
 return 0;}_LL79A:;}
# 5285
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*rgn,void*t){
void*_tmpA4D=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(_tmpA4D,0))return 1;{
void*_tmpA4E=_tmpA4D;union Cyc_Absyn_DatatypeFieldInfoU _tmpA4F;struct Cyc_List_List*_tmpA50;union Cyc_Absyn_DatatypeInfoU _tmpA51;struct Cyc_List_List*_tmpA52;struct Cyc_List_List*_tmpA53;struct Cyc_Absyn_Aggrdecl**_tmpA54;struct Cyc_List_List*_tmpA55;struct Cyc_List_List*_tmpA56;switch(*((int*)_tmpA4E)){case 10: _LL7B0: _tmpA56=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA4E)->f1;_LL7B1:
# 5290
 while(_tmpA56 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(*((struct _tuple12*)_tmpA56->hd)).f2))return 1;
_tmpA56=_tmpA56->tl;}
# 5294
return 0;case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA4E)->f1).aggr_info).KnownAggr).tag == 2){_LL7B2: _tmpA54=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA4E)->f1).aggr_info).KnownAggr).val;_tmpA55=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA4E)->f1).targs;_LL7B3:
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
return 0;}}else{_LL7B6: _LL7B7:
# 5316
 return 0;}case 12: _LL7B4: _tmpA53=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA4E)->f2;_LL7B5:
# 5309
 while(_tmpA53 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,((struct Cyc_Absyn_Aggrfield*)_tmpA53->hd)->type))return 1;
_tmpA53=_tmpA53->tl;}
# 5313
return 0;case 3: _LL7B8: _tmpA51=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA4E)->f1).datatype_info;_tmpA52=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA4E)->f1).targs;_LL7B9: {
# 5318
union Cyc_Absyn_DatatypeInfoU _tmpA59=_tmpA51;struct Cyc_List_List*_tmpA5A;struct Cyc_Core_Opt*_tmpA5B;struct _tuple2*_tmpA5C;int _tmpA5D;if((_tmpA59.UnknownDatatype).tag == 1){_LL7BF: _tmpA5C=((_tmpA59.UnknownDatatype).val).name;_tmpA5D=((_tmpA59.UnknownDatatype).val).is_extensible;_LL7C0:
# 5321
 return 0;}else{_LL7C1: _tmpA5A=(*(_tmpA59.KnownDatatype).val)->tvs;_tmpA5B=(*(_tmpA59.KnownDatatype).val)->fields;_LL7C2:
# 5324
 return 0;}_LL7BE:;}case 4: _LL7BA: _tmpA4F=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpA4E)->f1).field_info;_tmpA50=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpA4E)->f1).targs;_LL7BB: {
# 5327
union Cyc_Absyn_DatatypeFieldInfoU _tmpA5E=_tmpA4F;struct Cyc_Absyn_Datatypedecl*_tmpA5F;struct Cyc_Absyn_Datatypefield*_tmpA60;if((_tmpA5E.UnknownDatatypefield).tag == 1){_LL7C4: _LL7C5:
# 5330
 return 0;}else{_LL7C6: _tmpA5F=((_tmpA5E.KnownDatatypefield).val).f1;_tmpA60=((_tmpA5E.KnownDatatypefield).val).f2;_LL7C7: {
# 5332
struct Cyc_List_List*_tmpA61=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpA5F->tvs,_tmpA50);
struct Cyc_List_List*_tmpA62=_tmpA60->typs;
while(_tmpA62 != 0){
_tmpA4D=_tmpA61 == 0?(*((struct _tuple12*)_tmpA62->hd)).f2: Cyc_Tcutil_rsubstitute(rgn,_tmpA61,(*((struct _tuple12*)_tmpA62->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,_tmpA4D))return 1;
_tmpA62=_tmpA62->tl;}
# 5339
return 0;}}_LL7C3:;}default: _LL7BC: _LL7BD:
# 5341
 return 0;}_LL7AF:;};}
# 5345
static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,struct _dyneither_ptr*f){
void*_tmpA63=Cyc_Tcutil_compress(t);void*_tmpA64=_tmpA63;struct Cyc_List_List*_tmpA65;struct Cyc_Absyn_Aggrdecl*_tmpA66;struct Cyc_List_List*_tmpA67;switch(*((int*)_tmpA64)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA64)->f1).aggr_info).KnownAggr).tag == 2){_LL7C9: _tmpA66=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA64)->f1).aggr_info).KnownAggr).val;_tmpA67=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA64)->f1).targs;_LL7CA:
# 5348
 _tmpA65=_tmpA66->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpA66->impl))->fields;goto _LL7CC;}else{goto _LL7CD;}case 12: _LL7CB: _tmpA65=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA64)->f2;_LL7CC: {
# 5350
struct Cyc_Absyn_Aggrfield*_tmpA68=Cyc_Absyn_lookup_field(_tmpA65,f);
{struct Cyc_Absyn_Aggrfield*_tmpA69=_tmpA68;void*_tmpA6A;if(_tmpA69 == 0){_LL7D0: _LL7D1: {
const char*_tmp10E6;void*_tmp10E5;(_tmp10E5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10E6="is_noalias_field: missing field",_tag_dyneither(_tmp10E6,sizeof(char),32))),_tag_dyneither(_tmp10E5,sizeof(void*),0)));}}else{_LL7D2: _tmpA6A=_tmpA69->type;_LL7D3:
# 5354
 return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpA6A);}_LL7CF:;}
# 5356
return 0;}default: _LL7CD: _LL7CE: {
# 5358
const char*_tmp10EA;void*_tmp10E9[1];struct Cyc_String_pa_PrintArg_struct _tmp10E8;(_tmp10E8.tag=0,((_tmp10E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp10E9[0]=& _tmp10E8,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10EA="is_noalias_field: invalid type |%s|",_tag_dyneither(_tmp10EA,sizeof(char),36))),_tag_dyneither(_tmp10E9,sizeof(void*),1)))))));}}_LL7C8:;}
# 5366
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
void*_tmpA70=e->r;void*_tmpA71=_tmpA70;struct Cyc_Absyn_Stmt*_tmpA72;struct Cyc_Absyn_Exp*_tmpA73;struct Cyc_Absyn_Exp*_tmpA74;struct Cyc_Absyn_Exp*_tmpA75;struct Cyc_Absyn_Exp*_tmpA76;struct Cyc_Absyn_Exp*_tmpA77;struct Cyc_Absyn_Exp*_tmpA78;struct Cyc_Absyn_Exp*_tmpA79;struct _dyneither_ptr*_tmpA7A;struct Cyc_Absyn_Exp*_tmpA7B;struct Cyc_Absyn_Exp*_tmpA7C;switch(*((int*)_tmpA71)){case 1: if(((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA71)->f2)->tag == 1){_LL7D5: _LL7D6:
 return 0;}else{goto _LL7E7;}case 21: _LL7D7: _tmpA7C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA71)->f1;_LL7D8:
 _tmpA7B=_tmpA7C;goto _LL7DA;case 19: _LL7D9: _tmpA7B=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpA71)->f1;_LL7DA:
# 5371
 return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmpA7B->topt),1) && 
Cyc_Tcutil_is_noalias_path(r,_tmpA7B);case 20: _LL7DB: _tmpA79=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA71)->f1;_tmpA7A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA71)->f2;_LL7DC:
 return Cyc_Tcutil_is_noalias_path(r,_tmpA79);case 22: _LL7DD: _tmpA77=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA71)->f1;_tmpA78=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA71)->f2;_LL7DE: {
# 5375
void*_tmpA7D=Cyc_Tcutil_compress((void*)_check_null(_tmpA77->topt));void*_tmpA7E=_tmpA7D;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA7E)->tag == 10){_LL7EA: _LL7EB:
 return Cyc_Tcutil_is_noalias_path(r,_tmpA77);}else{_LL7EC: _LL7ED:
 return 0;}_LL7E9:;}case 5: _LL7DF: _tmpA75=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA71)->f2;_tmpA76=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA71)->f3;_LL7E0:
# 5380
 return Cyc_Tcutil_is_noalias_path(r,_tmpA75) && Cyc_Tcutil_is_noalias_path(r,_tmpA76);case 8: _LL7E1: _tmpA74=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpA71)->f2;_LL7E2:
 _tmpA73=_tmpA74;goto _LL7E4;case 13: _LL7E3: _tmpA73=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA71)->f2;_LL7E4:
 return Cyc_Tcutil_is_noalias_path(r,_tmpA73);case 36: _LL7E5: _tmpA72=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpA71)->f1;_LL7E6:
# 5384
 while(1){
void*_tmpA7F=_tmpA72->r;void*_tmpA80=_tmpA7F;struct Cyc_Absyn_Exp*_tmpA81;struct Cyc_Absyn_Decl*_tmpA82;struct Cyc_Absyn_Stmt*_tmpA83;struct Cyc_Absyn_Stmt*_tmpA84;struct Cyc_Absyn_Stmt*_tmpA85;switch(*((int*)_tmpA80)){case 2: _LL7EF: _tmpA84=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpA80)->f1;_tmpA85=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpA80)->f2;_LL7F0:
 _tmpA72=_tmpA85;goto _LL7EE;case 12: _LL7F1: _tmpA82=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpA80)->f1;_tmpA83=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpA80)->f2;_LL7F2:
 _tmpA72=_tmpA83;goto _LL7EE;case 1: _LL7F3: _tmpA81=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpA80)->f1;_LL7F4:
 return Cyc_Tcutil_is_noalias_path(r,_tmpA81);default: _LL7F5: _LL7F6: {
const char*_tmp10ED;void*_tmp10EC;(_tmp10EC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10ED="is_noalias_stmt_exp: ill-formed StmtExp",_tag_dyneither(_tmp10ED,sizeof(char),40))),_tag_dyneither(_tmp10EC,sizeof(void*),0)));}}_LL7EE:;}default: _LL7E7: _LL7E8:
# 5392
 return 1;}_LL7D4:;}
# 5409 "tcutil.cyc"
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5411
struct _tuple18 _tmp10EE;struct _tuple18 bogus_ans=(_tmp10EE.f1=0,((_tmp10EE.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp10EE)));
void*_tmpA88=e->r;void*_tmpA89=_tmpA88;struct Cyc_Absyn_Exp*_tmpA8A;struct Cyc_Absyn_Exp*_tmpA8B;struct Cyc_Absyn_Exp*_tmpA8C;struct Cyc_Absyn_Exp*_tmpA8D;struct _dyneither_ptr*_tmpA8E;int _tmpA8F;struct Cyc_Absyn_Exp*_tmpA90;struct _dyneither_ptr*_tmpA91;int _tmpA92;struct _tuple2*_tmpA93;void*_tmpA94;switch(*((int*)_tmpA89)){case 1: _LL7F8: _tmpA93=((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA89)->f1;_tmpA94=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA89)->f2;_LL7F9: {
# 5415
void*_tmpA95=_tmpA94;struct Cyc_Absyn_Vardecl*_tmpA96;struct Cyc_Absyn_Vardecl*_tmpA97;struct Cyc_Absyn_Vardecl*_tmpA98;struct Cyc_Absyn_Vardecl*_tmpA99;switch(*((int*)_tmpA95)){case 0: _LL805: _LL806:
 goto _LL808;case 2: _LL807: _LL808:
 return bogus_ans;case 1: _LL809: _tmpA99=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpA95)->f1;_LL80A: {
# 5419
void*_tmpA9A=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpA9B=_tmpA9A;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA9B)->tag == 8){_LL812: _LL813: {
# 5421
struct _tuple18 _tmp10EF;return(_tmp10EF.f1=1,((_tmp10EF.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp10EF)));}}else{_LL814: _LL815: {
struct _tuple18 _tmp10F0;return(_tmp10F0.f1=(_tmpA99->tq).real_const,((_tmp10F0.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp10F0)));}}_LL811:;}case 4: _LL80B: _tmpA98=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpA95)->f1;_LL80C: {
# 5425
void*_tmpA9E=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpA9F=_tmpA9E;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA9F)->tag == 8){_LL817: _LL818: {
struct _tuple18 _tmp10F1;return(_tmp10F1.f1=1,((_tmp10F1.f2=(void*)_check_null(_tmpA98->rgn),_tmp10F1)));}}else{_LL819: _LL81A:
# 5428
 _tmpA98->escapes=1;{
struct _tuple18 _tmp10F2;return(_tmp10F2.f1=(_tmpA98->tq).real_const,((_tmp10F2.f2=(void*)_check_null(_tmpA98->rgn),_tmp10F2)));};}_LL816:;}case 5: _LL80D: _tmpA97=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpA95)->f1;_LL80E:
# 5431
 _tmpA96=_tmpA97;goto _LL810;default: _LL80F: _tmpA96=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpA95)->f1;_LL810:
# 5433
 _tmpA96->escapes=1;{
struct _tuple18 _tmp10F3;return(_tmp10F3.f1=(_tmpA96->tq).real_const,((_tmp10F3.f2=(void*)_check_null(_tmpA96->rgn),_tmp10F3)));};}_LL804:;}case 20: _LL7FA: _tmpA90=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA89)->f1;_tmpA91=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA89)->f2;_tmpA92=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA89)->f3;_LL7FB:
# 5438
 if(_tmpA92)return bogus_ans;{
# 5441
void*_tmpAA3=Cyc_Tcutil_compress((void*)_check_null(_tmpA90->topt));void*_tmpAA4=_tmpAA3;struct Cyc_Absyn_Aggrdecl*_tmpAA5;struct Cyc_List_List*_tmpAA6;switch(*((int*)_tmpAA4)){case 12: _LL81C: _tmpAA6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpAA4)->f2;_LL81D: {
# 5443
struct Cyc_Absyn_Aggrfield*_tmpAA7=Cyc_Absyn_lookup_field(_tmpAA6,_tmpA91);
if(_tmpAA7 != 0  && _tmpAA7->width == 0){
struct _tuple18 _tmpAA8=Cyc_Tcutil_addressof_props(te,_tmpA90);int _tmpAAA;void*_tmpAAB;struct _tuple18 _tmpAA9=_tmpAA8;_tmpAAA=_tmpAA9.f1;_tmpAAB=_tmpAA9.f2;{
struct _tuple18 _tmp10F4;return(_tmp10F4.f1=(_tmpAA7->tq).real_const  || _tmpAAA,((_tmp10F4.f2=_tmpAAB,_tmp10F4)));};}
# 5448
return bogus_ans;}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAA4)->f1).aggr_info).KnownAggr).tag == 2){_LL81E: _tmpAA5=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAA4)->f1).aggr_info).KnownAggr).val;_LL81F: {
# 5450
struct Cyc_Absyn_Aggrfield*_tmpAAD=Cyc_Absyn_lookup_decl_field(_tmpAA5,_tmpA91);
if(_tmpAAD != 0  && _tmpAAD->width == 0){
struct _tuple18 _tmpAAE=Cyc_Tcutil_addressof_props(te,_tmpA90);int _tmpAB0;void*_tmpAB1;struct _tuple18 _tmpAAF=_tmpAAE;_tmpAB0=_tmpAAF.f1;_tmpAB1=_tmpAAF.f2;{
struct _tuple18 _tmp10F5;return(_tmp10F5.f1=(_tmpAAD->tq).real_const  || _tmpAB0,((_tmp10F5.f2=_tmpAB1,_tmp10F5)));};}
# 5455
return bogus_ans;}}else{goto _LL820;}default: _LL820: _LL821:
 return bogus_ans;}_LL81B:;};case 21: _LL7FC: _tmpA8D=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA89)->f1;_tmpA8E=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA89)->f2;_tmpA8F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA89)->f3;_LL7FD:
# 5460
 if(_tmpA8F)return bogus_ans;{
# 5464
void*_tmpAB3=Cyc_Tcutil_compress((void*)_check_null(_tmpA8D->topt));void*_tmpAB4=_tmpAB3;void*_tmpAB5;void*_tmpAB6;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAB4)->tag == 5){_LL823: _tmpAB5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAB4)->f1).elt_typ;_tmpAB6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAB4)->f1).ptr_atts).rgn;_LL824: {
# 5466
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmpAB7=Cyc_Tcutil_compress(_tmpAB5);void*_tmpAB8=_tmpAB7;struct Cyc_Absyn_Aggrdecl*_tmpAB9;struct Cyc_List_List*_tmpABA;switch(*((int*)_tmpAB8)){case 12: _LL828: _tmpABA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpAB8)->f2;_LL829:
# 5469
 finfo=Cyc_Absyn_lookup_field(_tmpABA,_tmpA8E);goto _LL827;case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAB8)->f1).aggr_info).KnownAggr).tag == 2){_LL82A: _tmpAB9=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAB8)->f1).aggr_info).KnownAggr).val;_LL82B:
# 5471
 finfo=Cyc_Absyn_lookup_decl_field(_tmpAB9,_tmpA8E);goto _LL827;}else{goto _LL82C;}default: _LL82C: _LL82D:
 return bogus_ans;}_LL827:;}
# 5474
if(finfo != 0  && finfo->width == 0){
struct _tuple18 _tmp10F6;return(_tmp10F6.f1=(finfo->tq).real_const,((_tmp10F6.f2=_tmpAB6,_tmp10F6)));}
return bogus_ans;}}else{_LL825: _LL826:
 return bogus_ans;}_LL822:;};case 19: _LL7FE: _tmpA8C=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpA89)->f1;_LL7FF: {
# 5481
void*_tmpABC=Cyc_Tcutil_compress((void*)_check_null(_tmpA8C->topt));void*_tmpABD=_tmpABC;struct Cyc_Absyn_Tqual _tmpABE;void*_tmpABF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpABD)->tag == 5){_LL82F: _tmpABE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpABD)->f1).elt_tq;_tmpABF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpABD)->f1).ptr_atts).rgn;_LL830: {
# 5483
struct _tuple18 _tmp10F7;return(_tmp10F7.f1=_tmpABE.real_const,((_tmp10F7.f2=_tmpABF,_tmp10F7)));}}else{_LL831: _LL832:
 return bogus_ans;}_LL82E:;}case 22: _LL800: _tmpA8A=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA89)->f1;_tmpA8B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA89)->f2;_LL801: {
# 5489
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmpA8A->topt));
void*_tmpAC1=t;struct Cyc_Absyn_Tqual _tmpAC2;struct Cyc_Absyn_Tqual _tmpAC3;void*_tmpAC4;struct Cyc_List_List*_tmpAC5;switch(*((int*)_tmpAC1)){case 10: _LL834: _tmpAC5=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpAC1)->f1;_LL835: {
# 5493
struct _tuple14 _tmpAC6=Cyc_Evexp_eval_const_uint_exp(_tmpA8B);unsigned int _tmpAC8;int _tmpAC9;struct _tuple14 _tmpAC7=_tmpAC6;_tmpAC8=_tmpAC7.f1;_tmpAC9=_tmpAC7.f2;
if(!_tmpAC9)
return bogus_ans;{
struct _tuple12*_tmpACA=Cyc_Absyn_lookup_tuple_field(_tmpAC5,(int)_tmpAC8);
if(_tmpACA != 0){
struct _tuple18 _tmp10F8;return(_tmp10F8.f1=((*_tmpACA).f1).real_const,((_tmp10F8.f2=(Cyc_Tcutil_addressof_props(te,_tmpA8A)).f2,_tmp10F8)));}
return bogus_ans;};}case 5: _LL836: _tmpAC3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAC1)->f1).elt_tq;_tmpAC4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAC1)->f1).ptr_atts).rgn;_LL837: {
# 5501
struct _tuple18 _tmp10F9;return(_tmp10F9.f1=_tmpAC3.real_const,((_tmp10F9.f2=_tmpAC4,_tmp10F9)));}case 8: _LL838: _tmpAC2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAC1)->f1).tq;_LL839: {
# 5507
struct _tuple18 _tmp10FA;return(_tmp10FA.f1=_tmpAC2.real_const,((_tmp10FA.f2=(Cyc_Tcutil_addressof_props(te,_tmpA8A)).f2,_tmp10FA)));}default: _LL83A: _LL83B:
 return bogus_ans;}_LL833:;}default: _LL802: _LL803:
# 5511
{const char*_tmp10FD;void*_tmp10FC;(_tmp10FC=0,Cyc_Tcutil_terr(e->loc,((_tmp10FD="unary & applied to non-lvalue",_tag_dyneither(_tmp10FD,sizeof(char),30))),_tag_dyneither(_tmp10FC,sizeof(void*),0)));}
return bogus_ans;}_LL7F7:;}
# 5518
void Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,union Cyc_Absyn_Constraint*b){
b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(b);{
void*_tmpAD1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);void*_tmpAD2=_tmpAD1;struct Cyc_Absyn_Exp*_tmpAD3;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpAD2)->tag == 0){_LL83D: _LL83E:
 return;}else{_LL83F: _tmpAD3=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpAD2)->f1;_LL840: {
# 5523
struct _tuple14 _tmpAD4=Cyc_Evexp_eval_const_uint_exp(_tmpAD3);unsigned int _tmpAD6;int _tmpAD7;struct _tuple14 _tmpAD5=_tmpAD4;_tmpAD6=_tmpAD5.f1;_tmpAD7=_tmpAD5.f2;
if(_tmpAD7  && _tmpAD6 <= i){
const char*_tmp1102;void*_tmp1101[2];struct Cyc_Int_pa_PrintArg_struct _tmp1100;struct Cyc_Int_pa_PrintArg_struct _tmp10FF;(_tmp10FF.tag=1,((_tmp10FF.f1=(unsigned long)((int)i),((_tmp1100.tag=1,((_tmp1100.f1=(unsigned long)((int)_tmpAD6),((_tmp1101[0]=& _tmp1100,((_tmp1101[1]=& _tmp10FF,Cyc_Tcutil_terr(loc,((_tmp1102="dereference is out of bounds: %d <= %d",_tag_dyneither(_tmp1102,sizeof(char),39))),_tag_dyneither(_tmp1101,sizeof(void*),2)))))))))))));}
return;}}_LL83C:;};}
# 5530
void Cyc_Tcutil_check_nonzero_bound(unsigned int loc,union Cyc_Absyn_Constraint*b){
Cyc_Tcutil_check_bound(loc,0,b);}
# 5534
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b){
void*_tmpADC=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);void*_tmpADD=_tmpADC;struct Cyc_Absyn_Exp*_tmpADE;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpADD)->tag == 1){_LL842: _tmpADE=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpADD)->f1;_LL843: {
# 5537
struct _tuple14 _tmpADF=Cyc_Evexp_eval_const_uint_exp(_tmpADE);unsigned int _tmpAE1;int _tmpAE2;struct _tuple14 _tmpAE0=_tmpADF;_tmpAE1=_tmpAE0.f1;_tmpAE2=_tmpAE0.f2;
return _tmpAE2  && _tmpAE1 == 1;}}else{_LL844: _LL845:
 return 0;}_LL841:;}
# 5543
int Cyc_Tcutil_bits_only(void*t){
void*_tmpAE3=Cyc_Tcutil_compress(t);void*_tmpAE4=_tmpAE3;struct Cyc_List_List*_tmpAE5;struct Cyc_Absyn_Aggrdecl*_tmpAE6;struct Cyc_List_List*_tmpAE7;struct Cyc_List_List*_tmpAE8;void*_tmpAE9;union Cyc_Absyn_Constraint*_tmpAEA;switch(*((int*)_tmpAE4)){case 0: _LL847: _LL848:
 goto _LL84A;case 6: _LL849: _LL84A:
 goto _LL84C;case 7: _LL84B: _LL84C:
 return 1;case 13: _LL84D: _LL84E:
 goto _LL850;case 14: _LL84F: _LL850:
 return 0;case 8: _LL851: _tmpAE9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAE4)->f1).elt_type;_tmpAEA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAE4)->f1).zero_term;_LL852:
# 5553
 return !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpAEA) && Cyc_Tcutil_bits_only(_tmpAE9);case 10: _LL853: _tmpAE8=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpAE4)->f1;_LL854:
# 5555
 for(0;_tmpAE8 != 0;_tmpAE8=_tmpAE8->tl){
if(!Cyc_Tcutil_bits_only((*((struct _tuple12*)_tmpAE8->hd)).f2))return 0;}
return 1;case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAE4)->f1).aggr_info).UnknownAggr).tag == 1){_LL855: _LL856:
 return 0;}else{_LL857: _tmpAE6=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAE4)->f1).aggr_info).KnownAggr).val;_tmpAE7=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAE4)->f1).targs;_LL858:
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
;_pop_region(rgn);};};}case 12: _LL859: _tmpAE5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpAE4)->f2;_LL85A:
# 5573
 for(0;_tmpAE5 != 0;_tmpAE5=_tmpAE5->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)_tmpAE5->hd)->type))return 0;}
return 1;default: _LL85B: _LL85C:
 return 0;}_LL846:;}
# 5581
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e){
while(1){
void*_tmpAEF=e->r;void*_tmpAF0=_tmpAEF;struct Cyc_Absyn_Exp*_tmpAF1;struct Cyc_Absyn_Exp*_tmpAF2;switch(*((int*)_tmpAF0)){case 1: _LL85E: _LL85F:
 return 1;case 11: _LL860: _tmpAF2=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpAF0)->f1;_LL861:
 _tmpAF1=_tmpAF2;goto _LL863;case 12: _LL862: _tmpAF1=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpAF0)->f1;_LL863:
 e=_tmpAF1;continue;default: _LL864: _LL865:
# 5588
 return 0;}_LL85D:;}}
# 5598
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmpAF3=e->r;void*_tmpAF4=_tmpAF3;struct Cyc_List_List*_tmpAF5;struct Cyc_List_List*_tmpAF6;enum Cyc_Absyn_Primop _tmpAF7;struct Cyc_List_List*_tmpAF8;struct Cyc_List_List*_tmpAF9;struct Cyc_List_List*_tmpAFA;struct Cyc_List_List*_tmpAFB;struct Cyc_Absyn_Exp*_tmpAFC;struct Cyc_Absyn_Exp*_tmpAFD;struct Cyc_Absyn_Exp*_tmpAFE;struct Cyc_Absyn_Exp*_tmpAFF;void*_tmpB00;struct Cyc_Absyn_Exp*_tmpB01;void*_tmpB02;struct Cyc_Absyn_Exp*_tmpB03;struct Cyc_Absyn_Exp*_tmpB04;struct Cyc_Absyn_Exp*_tmpB05;struct Cyc_Absyn_Exp*_tmpB06;struct Cyc_Absyn_Exp*_tmpB07;struct Cyc_Absyn_Exp*_tmpB08;struct Cyc_Absyn_Exp*_tmpB09;struct Cyc_Absyn_Exp*_tmpB0A;struct _tuple2*_tmpB0B;void*_tmpB0C;switch(*((int*)_tmpAF4)){case 0: _LL867: _LL868:
 goto _LL86A;case 16: _LL869: _LL86A:
 goto _LL86C;case 17: _LL86B: _LL86C:
 goto _LL86E;case 18: _LL86D: _LL86E:
 goto _LL870;case 31: _LL86F: _LL870:
 goto _LL872;case 32: _LL871: _LL872:
 return 1;case 1: _LL873: _tmpB0B=((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpAF4)->f1;_tmpB0C=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpAF4)->f2;_LL874: {
# 5609
void*_tmpB0D=_tmpB0C;struct Cyc_Absyn_Vardecl*_tmpB0E;struct Cyc_Absyn_Vardecl*_tmpB0F;switch(*((int*)_tmpB0D)){case 2: _LL896: _LL897:
 return 1;case 1: _LL898: _tmpB0F=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpB0D)->f1;_LL899: {
# 5612
void*_tmpB10=Cyc_Tcutil_compress(_tmpB0F->type);void*_tmpB11=_tmpB10;switch(*((int*)_tmpB11)){case 8: _LL8A1: _LL8A2:
 goto _LL8A4;case 9: _LL8A3: _LL8A4:
 return 1;default: _LL8A5: _LL8A6:
 return var_okay;}_LL8A0:;}case 4: _LL89A: _tmpB0E=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpB0D)->f1;_LL89B:
# 5619
 if(_tmpB0E->sc == Cyc_Absyn_Static){
void*_tmpB12=Cyc_Tcutil_compress(_tmpB0E->type);void*_tmpB13=_tmpB12;switch(*((int*)_tmpB13)){case 8: _LL8A8: _LL8A9:
 goto _LL8AB;case 9: _LL8AA: _LL8AB:
 return 1;default: _LL8AC: _LL8AD:
 return var_okay;}_LL8A7:;}else{
# 5626
return var_okay;}case 0: _LL89C: _LL89D:
 return 0;default: _LL89E: _LL89F:
 return var_okay;}_LL895:;}case 5: _LL875: _tmpB08=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAF4)->f1;_tmpB09=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAF4)->f2;_tmpB0A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAF4)->f3;_LL876:
# 5632
 return(Cyc_Tcutil_cnst_exp(0,_tmpB08) && 
Cyc_Tcutil_cnst_exp(0,_tmpB09)) && 
Cyc_Tcutil_cnst_exp(0,_tmpB0A);case 8: _LL877: _tmpB06=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpAF4)->f1;_tmpB07=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpAF4)->f2;_LL878:
# 5636
 return Cyc_Tcutil_cnst_exp(0,_tmpB06) && Cyc_Tcutil_cnst_exp(0,_tmpB07);case 11: _LL879: _tmpB05=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpAF4)->f1;_LL87A:
 _tmpB04=_tmpB05;goto _LL87C;case 12: _LL87B: _tmpB04=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpAF4)->f1;_LL87C:
# 5639
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpB04);case 13: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAF4)->f4 == Cyc_Absyn_No_coercion){_LL87D: _tmpB02=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAF4)->f1;_tmpB03=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAF4)->f2;_LL87E:
# 5641
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpB03);}else{_LL87F: _tmpB00=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAF4)->f1;_tmpB01=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAF4)->f2;_LL880:
# 5644
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpB01);}case 14: _LL881: _tmpAFF=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpAF4)->f1;_LL882:
# 5646
 return Cyc_Tcutil_cnst_exp(1,_tmpAFF);case 26: _LL883: _tmpAFD=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpAF4)->f2;_tmpAFE=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpAF4)->f3;_LL884:
# 5648
 return Cyc_Tcutil_cnst_exp(0,_tmpAFD) && Cyc_Tcutil_cnst_exp(0,_tmpAFE);case 27: _LL885: _tmpAFC=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpAF4)->f1;_LL886:
# 5650
 return Cyc_Tcutil_cnst_exp(0,_tmpAFC);case 25: _LL887: _tmpAFB=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpAF4)->f1;_LL888:
 _tmpAFA=_tmpAFB;goto _LL88A;case 29: _LL889: _tmpAFA=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpAF4)->f2;_LL88A:
 _tmpAF9=_tmpAFA;goto _LL88C;case 28: _LL88B: _tmpAF9=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpAF4)->f3;_LL88C:
# 5654
 for(0;_tmpAF9 != 0;_tmpAF9=_tmpAF9->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple19*)_tmpAF9->hd)).f2))
return 0;}
return 1;case 2: _LL88D: _tmpAF7=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpAF4)->f1;_tmpAF8=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpAF4)->f2;_LL88E:
# 5659
 _tmpAF6=_tmpAF8;goto _LL890;case 23: _LL88F: _tmpAF6=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpAF4)->f1;_LL890:
 _tmpAF5=_tmpAF6;goto _LL892;case 30: _LL891: _tmpAF5=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpAF4)->f1;_LL892:
# 5662
 for(0;_tmpAF5 != 0;_tmpAF5=_tmpAF5->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmpAF5->hd))
return 0;}
return 1;default: _LL893: _LL894:
 return 0;}_LL866:;}
# 5670
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 5674
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);
# 5676
int Cyc_Tcutil_supports_default(void*t){
void*_tmpB14=Cyc_Tcutil_compress(t);void*_tmpB15=_tmpB14;struct Cyc_List_List*_tmpB16;union Cyc_Absyn_AggrInfoU _tmpB17;struct Cyc_List_List*_tmpB18;struct Cyc_List_List*_tmpB19;void*_tmpB1A;union Cyc_Absyn_Constraint*_tmpB1B;union Cyc_Absyn_Constraint*_tmpB1C;switch(*((int*)_tmpB15)){case 0: _LL8AF: _LL8B0:
 goto _LL8B2;case 6: _LL8B1: _LL8B2:
 goto _LL8B4;case 7: _LL8B3: _LL8B4:
 return 1;case 5: _LL8B5: _tmpB1B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB15)->f1).ptr_atts).nullable;_tmpB1C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB15)->f1).ptr_atts).bounds;_LL8B6: {
# 5683
void*_tmpB1D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpB1C);void*_tmpB1E=_tmpB1D;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpB1E)->tag == 0){_LL8C6: _LL8C7:
 return 1;}else{_LL8C8: _LL8C9:
# 5686
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmpB1B);}_LL8C5:;}case 8: _LL8B7: _tmpB1A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB15)->f1).elt_type;_LL8B8:
# 5689
 return Cyc_Tcutil_supports_default(_tmpB1A);case 10: _LL8B9: _tmpB19=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpB15)->f1;_LL8BA:
# 5691
 for(0;_tmpB19 != 0;_tmpB19=_tmpB19->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple12*)_tmpB19->hd)).f2))return 0;}
return 1;case 11: _LL8BB: _tmpB17=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpB15)->f1).aggr_info;_tmpB18=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpB15)->f1).targs;_LL8BC: {
# 5695
struct Cyc_Absyn_Aggrdecl*_tmpB1F=Cyc_Absyn_get_known_aggrdecl(_tmpB17);
if(_tmpB1F->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB1F->impl))->exist_vars != 0)return 0;
return Cyc_Tcutil_fields_support_default(_tmpB1F->tvs,_tmpB18,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB1F->impl))->fields);}case 12: _LL8BD: _tmpB16=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpB15)->f2;_LL8BE:
 return Cyc_Tcutil_fields_support_default(0,0,_tmpB16);case 14: _LL8BF: _LL8C0:
# 5701
 goto _LL8C2;case 13: _LL8C1: _LL8C2:
 return 1;default: _LL8C3: _LL8C4:
# 5704
 return 0;}_LL8AE:;}
# 5709
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t){
void*_tmpB20=t;struct Cyc_Absyn_Typedefdecl*_tmpB21;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpB20)->tag == 17){_LL8CB: _tmpB21=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpB20)->f3;_LL8CC:
# 5712
 if(_tmpB21 != 0){
struct Cyc_Absyn_Tqual _tmpB22=_tmpB21->tq;
if(((_tmpB22.print_const  || _tmpB22.q_volatile) || _tmpB22.q_restrict) || _tmpB22.real_const){
# 5717
const char*_tmp1106;void*_tmp1105[1];struct Cyc_String_pa_PrintArg_struct _tmp1104;(_tmp1104.tag=0,((_tmp1104.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1105[0]=& _tmp1104,Cyc_Tcutil_warn(loc,((_tmp1106="qualifier within typedef type %s is ignored",_tag_dyneither(_tmp1106,sizeof(char),44))),_tag_dyneither(_tmp1105,sizeof(void*),1)))))));}}
# 5720
goto _LL8CA;}else{_LL8CD: _LL8CE:
 goto _LL8CA;}_LL8CA:;}
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
void*_tmpB2B=Cyc_Tcutil_compress(t);void*_tmpB2C=_tmpB2B;union Cyc_Absyn_Constraint*_tmpB2D;union Cyc_Absyn_Constraint*_tmpB2E;switch(*((int*)_tmpB2C)){case 6: _LL8D0: _LL8D1:
 goto _LL8D3;case 7: _LL8D2: _LL8D3:
 return 1;case 5: _LL8D4: _tmpB2D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB2C)->f1).ptr_atts).nullable;_tmpB2E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB2C)->f1).ptr_atts).bounds;_LL8D5: {
# 5747
void*_tmpB2F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpB2E);void*_tmpB30=_tmpB2F;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpB30)->tag == 0){_LL8D9: _LL8DA:
# 5751
 return 0;}else{_LL8DB: _LL8DC:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpB2D);}_LL8D8:;}default: _LL8D6: _LL8D7:
# 5754
 return 0;}_LL8CF:;}
# 5758
int Cyc_Tcutil_is_noreturn(void*t){
{void*_tmpB31=Cyc_Tcutil_compress(t);void*_tmpB32=_tmpB31;struct Cyc_List_List*_tmpB33;void*_tmpB34;switch(*((int*)_tmpB32)){case 5: _LL8DE: _tmpB34=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB32)->f1).elt_typ;_LL8DF:
 return Cyc_Tcutil_is_noreturn(_tmpB34);case 9: _LL8E0: _tmpB33=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB32)->f1).attributes;_LL8E1:
# 5762
 for(0;_tmpB33 != 0;_tmpB33=_tmpB33->tl){
void*_tmpB35=(void*)_tmpB33->hd;void*_tmpB36=_tmpB35;if(((struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmpB36)->tag == 4){_LL8E5: _LL8E6:
 return 1;}else{_LL8E7: _LL8E8:
 continue;}_LL8E4:;}
# 5767
goto _LL8DD;default: _LL8E2: _LL8E3:
 goto _LL8DD;}_LL8DD:;}
# 5770
return 0;}
# 5775
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmpB37=Cyc_Tcutil_compress(t);void*_tmpB38=_tmpB37;struct Cyc_List_List**_tmpB39;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB38)->tag == 9){_LL8EA: _tmpB39=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB38)->f1).attributes;_LL8EB: {
# 5778
struct Cyc_List_List*_tmpB3A=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Tcutil_attribute_cmp,*_tmpB39,(void*)atts->hd)){
struct Cyc_List_List*_tmp1107;*_tmpB39=((_tmp1107=_cycalloc(sizeof(*_tmp1107)),((_tmp1107->hd=(void*)atts->hd,((_tmp1107->tl=*_tmpB39,_tmp1107))))));}}else{
# 5785
struct Cyc_List_List*_tmp1108;_tmpB3A=((_tmp1108=_cycalloc(sizeof(*_tmp1108)),((_tmp1108->hd=(void*)atts->hd,((_tmp1108->tl=_tmpB3A,_tmp1108))))));}}
return _tmpB3A;}}else{_LL8EC: _LL8ED: {
const char*_tmp110B;void*_tmp110A;(_tmp110A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp110B="transfer_fn_type_atts",_tag_dyneither(_tmp110B,sizeof(char),22))),_tag_dyneither(_tmp110A,sizeof(void*),0)));}}_LL8E9:;}
# 5792
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
struct Cyc_Absyn_Exp*bound=0;
{void*_tmpB3F=Cyc_Tcutil_compress(t);void*_tmpB40=_tmpB3F;struct Cyc_Absyn_Exp*_tmpB41;struct Cyc_Absyn_PtrInfo*_tmpB42;switch(*((int*)_tmpB40)){case 5: _LL8EF: _tmpB42=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB40)->f1;_LL8F0:
# 5796
{void*_tmpB43=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)((_tmpB42->ptr_atts).bounds);void*_tmpB44=_tmpB43;struct Cyc_Absyn_Exp*_tmpB45;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpB44)->tag == 1){_LL8F6: _tmpB45=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpB44)->f1;_LL8F7:
 bound=_tmpB45;goto _LL8F5;}else{_LL8F8: _LL8F9:
 goto _LL8F5;}_LL8F5:;}
# 5800
goto _LL8EE;case 8: _LL8F1: _tmpB41=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB40)->f1).num_elts;_LL8F2:
# 5802
 bound=_tmpB41;
goto _LL8EE;default: _LL8F3: _LL8F4:
 goto _LL8EE;}_LL8EE:;}
# 5806
return bound;}
# 5811
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
{void*_tmpB46=b;struct Cyc_Absyn_Vardecl*_tmpB47;struct Cyc_Absyn_Vardecl*_tmpB48;struct Cyc_Absyn_Vardecl*_tmpB49;struct Cyc_Absyn_Vardecl*_tmpB4A;switch(*((int*)_tmpB46)){case 5: _LL8FB: _tmpB4A=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpB46)->f1;_LL8FC:
 _tmpB49=_tmpB4A;goto _LL8FE;case 4: _LL8FD: _tmpB49=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpB46)->f1;_LL8FE:
 _tmpB48=_tmpB49;goto _LL900;case 3: _LL8FF: _tmpB48=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpB46)->f1;_LL900:
 _tmpB47=_tmpB48;goto _LL902;case 1: _LL901: _tmpB47=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpB46)->f1;_LL902:
# 5817
 if(!_tmpB47->escapes)return _tmpB47;
goto _LL8FA;default: _LL903: _LL904:
 goto _LL8FA;}_LL8FA:;}
# 5821
return 0;}
# 5825
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmpB4B=0;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0){struct Cyc_List_List*_tmp110C;_tmpB4B=((_tmp110C=_cycalloc(sizeof(*_tmp110C)),((_tmp110C->hd=*((void**)_check_null((void**)x->hd)),((_tmp110C->tl=_tmpB4B,_tmp110C))))));}}}
return _tmpB4B;}
# 5832
int Cyc_Tcutil_is_array(void*t){
# 5834
void*_tmpB4D=Cyc_Tcutil_compress(t);void*_tmpB4E=_tmpB4D;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB4E)->tag == 8){_LL906: _LL907:
# 5836
 return 1;}else{_LL908: _LL909:
 return 0;}_LL905:;}
# 5841
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
# 5843
void*_tmpB4F=Cyc_Tcutil_compress(t);void*_tmpB50=_tmpB4F;void*_tmpB51;struct Cyc_Absyn_Tqual _tmpB52;struct Cyc_Absyn_Exp*_tmpB53;union Cyc_Absyn_Constraint*_tmpB54;unsigned int _tmpB55;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB50)->tag == 8){_LL90B: _tmpB51=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB50)->f1).elt_type;_tmpB52=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB50)->f1).tq;_tmpB53=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB50)->f1).num_elts;_tmpB54=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB50)->f1).zero_term;_tmpB55=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB50)->f1).zt_loc;_LL90C: {
# 5845
void*b;
if(_tmpB53 == 0)
b=(void*)& Cyc_Absyn_DynEither_b_val;else{
# 5849
struct Cyc_Absyn_Exp*bnd=_tmpB53;
if(convert_tag){
if(bnd->topt == 0){
const char*_tmp110F;void*_tmp110E;(_tmp110E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp110F="cannot convert tag without type!",_tag_dyneither(_tmp110F,sizeof(char),33))),_tag_dyneither(_tmp110E,sizeof(void*),0)));}{
void*_tmpB58=Cyc_Tcutil_compress((void*)_check_null(bnd->topt));void*_tmpB59=_tmpB58;void*_tmpB5A;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpB59)->tag == 19){_LL910: _tmpB5A=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpB59)->f1;_LL911:
# 5855
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp1112;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp1111;b=(void*)((_tmp1111=_cycalloc(sizeof(*_tmp1111)),((_tmp1111[0]=((_tmp1112.tag=1,((_tmp1112.f1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmpB5A,0),0,Cyc_Absyn_No_coercion,0),_tmp1112)))),_tmp1111))));}
goto _LL90F;}else{_LL912: _LL913:
# 5858
 if(Cyc_Tcutil_is_const_exp(bnd)){
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp1115;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp1114;b=(void*)((_tmp1114=_cycalloc(sizeof(*_tmp1114)),((_tmp1114[0]=((_tmp1115.tag=1,((_tmp1115.f1=bnd,_tmp1115)))),_tmp1114))));}else{
# 5861
b=(void*)& Cyc_Absyn_DynEither_b_val;}}_LL90F:;};}else{
# 5865
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp1118;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp1117;b=(void*)((_tmp1117=_cycalloc(sizeof(*_tmp1117)),((_tmp1117[0]=((_tmp1118.tag=1,((_tmp1118.f1=bnd,_tmp1118)))),_tmp1117))));}}
# 5867
return Cyc_Absyn_atb_typ(_tmpB51,rgn,_tmpB52,b,_tmpB54);}}else{_LL90D: _LL90E:
# 5870
 return t;}_LL90A:;}
# 5875
int Cyc_Tcutil_zeroable_type(void*t){
void*_tmpB61=Cyc_Tcutil_compress(t);void*_tmpB62=_tmpB61;struct Cyc_List_List*_tmpB63;union Cyc_Absyn_AggrInfoU _tmpB64;struct Cyc_List_List*_tmpB65;struct Cyc_List_List*_tmpB66;void*_tmpB67;union Cyc_Absyn_Constraint*_tmpB68;switch(*((int*)_tmpB62)){case 0: _LL915: _LL916:
 return 1;case 1: _LL917: _LL918:
 goto _LL91A;case 2: _LL919: _LL91A:
 goto _LL91C;case 3: _LL91B: _LL91C:
 goto _LL91E;case 4: _LL91D: _LL91E:
 return 0;case 5: _LL91F: _tmpB68=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB62)->f1).ptr_atts).nullable;_LL920:
# 5883
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmpB68);case 6: _LL921: _LL922:
 goto _LL924;case 7: _LL923: _LL924:
 return 1;case 8: _LL925: _tmpB67=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB62)->f1).elt_type;_LL926:
 return Cyc_Tcutil_zeroable_type(_tmpB67);case 9: _LL927: _LL928:
 return 0;case 10: _LL929: _tmpB66=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpB62)->f1;_LL92A:
# 5889
 for(0;(unsigned int)_tmpB66;_tmpB66=_tmpB66->tl){
if(!Cyc_Tcutil_zeroable_type((*((struct _tuple12*)_tmpB66->hd)).f2))return 0;}
return 1;case 11: _LL92B: _tmpB64=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpB62)->f1).aggr_info;_tmpB65=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpB62)->f1).targs;_LL92C: {
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
;_pop_region(r);};}case 13: _LL92D: _LL92E:
# 5902
 return 1;case 19: _LL92F: _LL930:
 return 1;case 12: _LL931: _tmpB63=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpB62)->f2;_LL932:
# 5905
 for(0;_tmpB63 != 0;_tmpB63=_tmpB63->tl){
if(!Cyc_Tcutil_zeroable_type(((struct Cyc_Absyn_Aggrfield*)_tmpB63->hd)->type))return 0;}
return 1;case 14: _LL933: _LL934:
 return 1;case 17: _LL935: _LL936:
 return 0;case 16: _LL937: _LL938:
 return 0;case 15: _LL939: _LL93A:
 return 0;case 28: _LL93B: _LL93C:
 return 0;case 26: _LL93D: _LL93E:
# 5914
 goto _LL940;case 27: _LL93F: _LL940:
 goto _LL942;case 18: _LL941: _LL942:
 goto _LL944;case 20: _LL943: _LL944:
 goto _LL946;case 21: _LL945: _LL946:
 goto _LL948;case 22: _LL947: _LL948:
 goto _LL94A;case 23: _LL949: _LL94A:
 goto _LL94C;case 24: _LL94B: _LL94C:
 goto _LL94E;default: _LL94D: _LL94E: {
const char*_tmp111C;void*_tmp111B[1];struct Cyc_String_pa_PrintArg_struct _tmp111A;(_tmp111A.tag=0,((_tmp111A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp111B[0]=& _tmp111A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp111C="bad type `%s' in zeroable type",_tag_dyneither(_tmp111C,sizeof(char),31))),_tag_dyneither(_tmp111B,sizeof(void*),1)))))));}}_LL914:;}
