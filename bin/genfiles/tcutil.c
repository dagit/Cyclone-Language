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
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 442 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 458
typedef void*Cyc_Absyn_funcparams_t;
# 461
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 522
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 537
extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;
# 548
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 555
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 573
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 927 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 929
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 937
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 940
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x);
# 944
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 947
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 949
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
# 955
void*Cyc_Absyn_compress_kb(void*);
# 960
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 965
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 967
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;
# 971
extern void*Cyc_Absyn_empty_effect;
# 981
extern struct _tuple2*Cyc_Absyn_datatype_print_arg_qvar;
extern struct _tuple2*Cyc_Absyn_datatype_scanf_arg_qvar;
# 990
extern void*Cyc_Absyn_bounds_one;
# 995
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 1021
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1031
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1038
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple2*,void*,unsigned int);
# 1040
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
# 1043
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1053
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1055
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned int);
# 1060
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1066
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
# 1078
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
# 1084
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1124
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);
# 1171
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1173
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1175
struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1177
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
# 1179
int Cyc_Absyn_fntype_att(void*a);struct _tuple13{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;};
# 1183
struct _tuple13 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1185
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
typedef void*Cyc_Tcenv_jumpee_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};
# 100
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 123 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(struct _RegionHandle*,void*ret_type,struct Cyc_List_List*args);
# 128
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 132
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 135
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 137
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 151
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
# 154
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
# 209
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 221
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
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
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
# 41 "tc.h"
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
{const char*_tmpEA0;if(Cyc_strcmp(((_tmpEA0="(qualifiers don't match)",_tag_dyneither(_tmpEA0,sizeof(char),25))),(struct _dyneither_ptr)Cyc_Tcutil_failure_reason)== 0){
{const char*_tmpEA4;void*_tmpEA3[1];struct Cyc_String_pa_PrintArg_struct _tmpEA2;(_tmpEA2.tag=0,((_tmpEA2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((_tmpEA3[0]=& _tmpEA2,Cyc_fprintf(Cyc_stderr,((_tmpEA4="  %s\n",_tag_dyneither(_tmpEA4,sizeof(char),6))),_tag_dyneither(_tmpEA3,sizeof(void*),1)))))));}
return;}}
# 75
{const char*_tmpEA5;if(Cyc_strcmp(((_tmpEA5="(function effects do not match)",_tag_dyneither(_tmpEA5,sizeof(char),32))),(struct _dyneither_ptr)Cyc_Tcutil_failure_reason)== 0){
struct Cyc_Absynpp_Params _tmp6=Cyc_Absynpp_tc_params_r;
_tmp6.print_all_effects=1;
Cyc_Absynpp_set_params(& _tmp6);}}{
# 80
struct _dyneither_ptr s1=Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure);
struct _dyneither_ptr s2=Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure);
int pos=2;
{const char*_tmpEA9;void*_tmpEA8[1];struct Cyc_String_pa_PrintArg_struct _tmpEA7;(_tmpEA7.tag=0,((_tmpEA7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmpEA8[0]=& _tmpEA7,Cyc_fprintf(Cyc_stderr,((_tmpEA9="  %s",_tag_dyneither(_tmpEA9,sizeof(char),5))),_tag_dyneither(_tmpEA8,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s1,sizeof(char));
if(pos + 5 >= 80){
{const char*_tmpEAC;void*_tmpEAB;(_tmpEAB=0,Cyc_fprintf(Cyc_stderr,((_tmpEAC="\n\t",_tag_dyneither(_tmpEAC,sizeof(char),3))),_tag_dyneither(_tmpEAB,sizeof(void*),0)));}
pos=8;}else{
# 89
{const char*_tmpEAF;void*_tmpEAE;(_tmpEAE=0,Cyc_fprintf(Cyc_stderr,((_tmpEAF=" ",_tag_dyneither(_tmpEAF,sizeof(char),2))),_tag_dyneither(_tmpEAE,sizeof(void*),0)));}
++ pos;}
# 92
{const char*_tmpEB2;void*_tmpEB1;(_tmpEB1=0,Cyc_fprintf(Cyc_stderr,((_tmpEB2="and ",_tag_dyneither(_tmpEB2,sizeof(char),5))),_tag_dyneither(_tmpEB1,sizeof(void*),0)));}
pos +=4;
if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){
{const char*_tmpEB5;void*_tmpEB4;(_tmpEB4=0,Cyc_fprintf(Cyc_stderr,((_tmpEB5="\n\t",_tag_dyneither(_tmpEB5,sizeof(char),3))),_tag_dyneither(_tmpEB4,sizeof(void*),0)));}
pos=8;}
# 98
{const char*_tmpEB9;void*_tmpEB8[1];struct Cyc_String_pa_PrintArg_struct _tmpEB7;(_tmpEB7.tag=0,((_tmpEB7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmpEB8[0]=& _tmpEB7,Cyc_fprintf(Cyc_stderr,((_tmpEB9="%s ",_tag_dyneither(_tmpEB9,sizeof(char),4))),_tag_dyneither(_tmpEB8,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s2,sizeof(char))+ 1;
if(pos + 17 >= 80){
{const char*_tmpEBC;void*_tmpEBB;(_tmpEBB=0,Cyc_fprintf(Cyc_stderr,((_tmpEBC="\n\t",_tag_dyneither(_tmpEBC,sizeof(char),3))),_tag_dyneither(_tmpEBB,sizeof(void*),0)));}
pos=8;}
# 104
{const char*_tmpEBF;void*_tmpEBE;(_tmpEBE=0,Cyc_fprintf(Cyc_stderr,((_tmpEBF="are not compatible. ",_tag_dyneither(_tmpEBF,sizeof(char),21))),_tag_dyneither(_tmpEBE,sizeof(void*),0)));}
pos +=17;
if((char*)Cyc_Tcutil_failure_reason.curr != (char*)(_tag_dyneither(0,0,0)).curr){
if(pos + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason)>= 80){
const char*_tmpEC2;void*_tmpEC1;(_tmpEC1=0,Cyc_fprintf(Cyc_stderr,((_tmpEC2="\n\t",_tag_dyneither(_tmpEC2,sizeof(char),3))),_tag_dyneither(_tmpEC1,sizeof(void*),0)));}{
# 110
const char*_tmpEC6;void*_tmpEC5[1];struct Cyc_String_pa_PrintArg_struct _tmpEC4;(_tmpEC4.tag=0,((_tmpEC4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((_tmpEC5[0]=& _tmpEC4,Cyc_fprintf(Cyc_stderr,((_tmpEC6="%s",_tag_dyneither(_tmpEC6,sizeof(char),3))),_tag_dyneither(_tmpEC5,sizeof(void*),1)))))));};}
# 112
{const char*_tmpEC9;void*_tmpEC8;(_tmpEC8=0,Cyc_fprintf(Cyc_stderr,((_tmpEC9="\n",_tag_dyneither(_tmpEC9,sizeof(char),2))),_tag_dyneither(_tmpEC8,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);};}
# 116
void Cyc_Tcutil_terr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 119
Cyc_Position_post_error(Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}
# 122
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 125
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
{const char*_tmpECD;void*_tmpECC[1];struct Cyc_String_pa_PrintArg_struct _tmpECB;(_tmpECB.tag=0,((_tmpECB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmpECC[0]=& _tmpECB,Cyc_fprintf(Cyc_stderr,((_tmpECD="Compiler Error (Tcutil::impos): %s\n",_tag_dyneither(_tmpECD,sizeof(char),36))),_tag_dyneither(_tmpECC,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Core_Impossible_exn_struct _tmpED0;struct Cyc_Core_Impossible_exn_struct*_tmpECF;(int)_throw((void*)((_tmpECF=_cycalloc(sizeof(*_tmpECF)),((_tmpECF[0]=((_tmpED0.tag=Cyc_Core_Impossible,((_tmpED0.f1=msg,_tmpED0)))),_tmpECF)))));};}char Cyc_Tcutil_AbortTypeCheckingFunction[26]="AbortTypeCheckingFunction";
# 133
static struct _dyneither_ptr Cyc_Tcutil_tvar2string(struct Cyc_Absyn_Tvar*tv){
return*tv->name;}
# 137
void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){
for(0;tvs != 0;tvs=tvs->tl){
const char*_tmpED5;void*_tmpED4[2];struct Cyc_String_pa_PrintArg_struct _tmpED3;struct Cyc_String_pa_PrintArg_struct _tmpED2;(_tmpED2.tag=0,((_tmpED2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((_tmpED3.tag=0,((_tmpED3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)tvs->hd)),((_tmpED4[0]=& _tmpED3,((_tmpED4[1]=& _tmpED2,Cyc_fprintf(Cyc_stderr,((_tmpED5="%s::%s ",_tag_dyneither(_tmpED5,sizeof(char),8))),_tag_dyneither(_tmpED4,sizeof(void*),2)))))))))))));}
{const char*_tmpED8;void*_tmpED7;(_tmpED7=0,Cyc_fprintf(Cyc_stderr,((_tmpED8="\n",_tag_dyneither(_tmpED8,sizeof(char),2))),_tag_dyneither(_tmpED7,sizeof(void*),0)));}Cyc_fflush(Cyc_stderr);}
# 144
static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;
# 149
void Cyc_Tcutil_warn(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 152
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
{struct Cyc_List_List*_tmpED9;Cyc_Tcutil_warning_segs=((_tmpED9=_cycalloc(sizeof(*_tmpED9)),((_tmpED9->hd=(void*)sg,((_tmpED9->tl=Cyc_Tcutil_warning_segs,_tmpED9))))));}{
struct _dyneither_ptr*_tmpEDC;struct Cyc_List_List*_tmpEDB;Cyc_Tcutil_warning_msgs=((_tmpEDB=_cycalloc(sizeof(*_tmpEDB)),((_tmpEDB->hd=((_tmpEDC=_cycalloc(sizeof(*_tmpEDC)),((_tmpEDC[0]=msg,_tmpEDC)))),((_tmpEDB->tl=Cyc_Tcutil_warning_msgs,_tmpEDB))))));};}
# 156
void Cyc_Tcutil_flush_warnings(){
if(Cyc_Tcutil_warning_segs == 0)
return;
{const char*_tmpEDF;void*_tmpEDE;(_tmpEDE=0,Cyc_fprintf(Cyc_stderr,((_tmpEDF="***Warnings***\n",_tag_dyneither(_tmpEDF,sizeof(char),16))),_tag_dyneither(_tmpEDE,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp30=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);
Cyc_Tcutil_warning_segs=0;
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);
while(Cyc_Tcutil_warning_msgs != 0){
{const char*_tmpEE4;void*_tmpEE3[2];struct Cyc_String_pa_PrintArg_struct _tmpEE2;struct Cyc_String_pa_PrintArg_struct _tmpEE1;(_tmpEE1.tag=0,((_tmpEE1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd)),((_tmpEE2.tag=0,((_tmpEE2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp30))->hd)),((_tmpEE3[0]=& _tmpEE2,((_tmpEE3[1]=& _tmpEE1,Cyc_fprintf(Cyc_stderr,((_tmpEE4="%s: %s\n",_tag_dyneither(_tmpEE4,sizeof(char),8))),_tag_dyneither(_tmpEE3,sizeof(void*),2)))))))))))));}
_tmp30=_tmp30->tl;
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}
# 168
{const char*_tmpEE7;void*_tmpEE6;(_tmpEE6=0,Cyc_fprintf(Cyc_stderr,((_tmpEE7="**************\n",_tag_dyneither(_tmpEE7,sizeof(char),16))),_tag_dyneither(_tmpEE6,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);};}
# 173
struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set=0;
# 177
static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
return tv1->identity - tv2->identity;}
# 182
void*Cyc_Tcutil_compress(void*t){
void*_tmp37=t;void**_tmp3B;void**_tmp3D;struct Cyc_Absyn_Exp*_tmp3F;void*_tmp41;_LL1: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp38=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp37;if(_tmp38->tag != 1)goto _LL3;else{if((void*)_tmp38->f2 != 0)goto _LL3;}}_LL2:
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
void*_tmp42=_tmp3F->r;void*_tmp43=_tmp42;void*_tmp45;_LL10: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp44=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp43;if(_tmp44->tag != 38)goto _LL12;else{_tmp45=(void*)_tmp44->f1;}}_LL11:
 return Cyc_Tcutil_compress(_tmp45);_LL12:;_LL13:
 return t;_LLF:;};_LLB: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp40=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp37;if(_tmp40->tag != 26)goto _LLD;else{if(_tmp40->f2 == 0)goto _LLD;_tmp41=*_tmp40->f2;}}_LLC:
# 205
 return Cyc_Tcutil_compress(_tmp41);_LLD:;_LLE:
 return t;_LL0:;}
# 213
void*Cyc_Tcutil_copy_type(void*t);
static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){
return((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);}
# 217
static union Cyc_Absyn_Constraint*Cyc_Tcutil_copy_conref(union Cyc_Absyn_Constraint*cptr){
union Cyc_Absyn_Constraint*_tmp46=cptr;void*_tmp47;union Cyc_Absyn_Constraint*_tmp48;_LL15: if((_tmp46->No_constr).tag != 3)goto _LL17;_LL16:
 return Cyc_Absyn_empty_conref();_LL17: if((_tmp46->Eq_constr).tag != 1)goto _LL19;_tmp47=(void*)(_tmp46->Eq_constr).val;_LL18:
 return Cyc_Absyn_new_conref(_tmp47);_LL19: if((_tmp46->Forward_constr).tag != 2)goto _LL14;_tmp48=(union Cyc_Absyn_Constraint*)(_tmp46->Forward_constr).val;_LL1A:
 return Cyc_Tcutil_copy_conref(_tmp48);_LL14:;}
# 224
static void*Cyc_Tcutil_copy_kindbound(void*kb){
void*_tmp49=Cyc_Absyn_compress_kb(kb);void*_tmp4A=_tmp49;struct Cyc_Absyn_Kind*_tmp4D;_LL1C: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp4B=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp4A;if(_tmp4B->tag != 1)goto _LL1E;}_LL1D: {
struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpEEA;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpEE9;return(void*)((_tmpEE9=_cycalloc(sizeof(*_tmpEE9)),((_tmpEE9[0]=((_tmpEEA.tag=1,((_tmpEEA.f1=0,_tmpEEA)))),_tmpEE9))));}_LL1E: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp4C=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4A;if(_tmp4C->tag != 2)goto _LL20;else{_tmp4D=_tmp4C->f2;}}_LL1F: {
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpEED;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpEEC;return(void*)((_tmpEEC=_cycalloc(sizeof(*_tmpEEC)),((_tmpEEC[0]=((_tmpEED.tag=2,((_tmpEED.f1=0,((_tmpEED.f2=_tmp4D,_tmpEED)))))),_tmpEEC))));}_LL20:;_LL21:
 return kb;_LL1B:;}
# 231
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 233
struct Cyc_Absyn_Tvar*_tmpEEE;return(_tmpEEE=_cycalloc(sizeof(*_tmpEEE)),((_tmpEEE->name=tv->name,((_tmpEEE->identity=- 1,((_tmpEEE->kind=Cyc_Tcutil_copy_kindbound(tv->kind),_tmpEEE)))))));}
# 235
static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
# 237
struct _dyneither_ptr*_tmp54;struct Cyc_Absyn_Tqual _tmp55;void*_tmp56;struct _tuple10*_tmp53=arg;_tmp54=_tmp53->f1;_tmp55=_tmp53->f2;_tmp56=_tmp53->f3;{
struct _tuple10*_tmpEEF;return(_tmpEEF=_cycalloc(sizeof(*_tmpEEF)),((_tmpEEF->f1=_tmp54,((_tmpEEF->f2=_tmp55,((_tmpEEF->f3=Cyc_Tcutil_copy_type(_tmp56),_tmpEEF)))))));};}
# 240
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
struct Cyc_Absyn_Tqual _tmp59;void*_tmp5A;struct _tuple12*_tmp58=arg;_tmp59=_tmp58->f1;_tmp5A=_tmp58->f2;{
struct _tuple12*_tmpEF0;return(_tmpEF0=_cycalloc(sizeof(*_tmpEF0)),((_tmpEF0->f1=_tmp59,((_tmpEF0->f2=Cyc_Tcutil_copy_type(_tmp5A),_tmpEF0)))));};}
# 244
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*);
# 246
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_Absyn_Aggrfield*_tmpEF1;return(_tmpEF1=_cycalloc(sizeof(*_tmpEF1)),((_tmpEF1->name=f->name,((_tmpEF1->tq=f->tq,((_tmpEF1->type=Cyc_Tcutil_copy_type(f->type),((_tmpEF1->width=f->width,((_tmpEF1->attributes=f->attributes,((_tmpEF1->requires_clause=
Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause),_tmpEF1)))))))))))));}
# 250
static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
void*_tmp5E;void*_tmp5F;struct _tuple0*_tmp5D=x;_tmp5E=_tmp5D->f1;_tmp5F=_tmp5D->f2;{
struct _tuple0*_tmpEF2;return(_tmpEF2=_cycalloc(sizeof(*_tmpEF2)),((_tmpEF2->f1=Cyc_Tcutil_copy_type(_tmp5E),((_tmpEF2->f2=Cyc_Tcutil_copy_type(_tmp5F),_tmpEF2)))));};}
# 254
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){
struct Cyc_Absyn_Enumfield*_tmpEF3;return(_tmpEF3=_cycalloc(sizeof(*_tmpEF3)),((_tmpEF3->name=f->name,((_tmpEF3->tag=f->tag,((_tmpEF3->loc=f->loc,_tmpEF3)))))));}
# 257
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpEF6;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpEF5;return(void*)((_tmpEF5=_cycalloc(sizeof(*_tmpEF5)),((_tmpEF5[0]=((_tmpEF6.tag=2,((_tmpEF6.f1=Cyc_Tcutil_copy_tvar(t),_tmpEF6)))),_tmpEF5))));}
# 261
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp64=Cyc_Tcutil_compress(t);void*_tmp65=_tmp64;struct Cyc_Absyn_Tvar*_tmp69;union Cyc_Absyn_DatatypeInfoU _tmp6B;struct Cyc_List_List*_tmp6C;union Cyc_Absyn_DatatypeFieldInfoU _tmp6E;struct Cyc_List_List*_tmp6F;void*_tmp71;struct Cyc_Absyn_Tqual _tmp72;void*_tmp73;union Cyc_Absyn_Constraint*_tmp74;union Cyc_Absyn_Constraint*_tmp75;union Cyc_Absyn_Constraint*_tmp76;struct Cyc_Absyn_PtrLoc*_tmp77;void*_tmp7B;struct Cyc_Absyn_Tqual _tmp7C;struct Cyc_Absyn_Exp*_tmp7D;union Cyc_Absyn_Constraint*_tmp7E;unsigned int _tmp7F;struct Cyc_List_List*_tmp81;void*_tmp82;struct Cyc_Absyn_Tqual _tmp83;void*_tmp84;struct Cyc_List_List*_tmp85;int _tmp86;struct Cyc_Absyn_VarargInfo*_tmp87;struct Cyc_List_List*_tmp88;struct Cyc_List_List*_tmp89;struct Cyc_Absyn_Exp*_tmp8A;struct Cyc_List_List*_tmp8B;struct Cyc_Absyn_Exp*_tmp8C;struct Cyc_List_List*_tmp8D;struct Cyc_List_List*_tmp8F;enum Cyc_Absyn_AggrKind _tmp91;struct _tuple2*_tmp92;struct Cyc_Core_Opt*_tmp93;struct Cyc_List_List*_tmp94;struct Cyc_Absyn_Aggrdecl**_tmp96;struct Cyc_List_List*_tmp97;enum Cyc_Absyn_AggrKind _tmp99;struct Cyc_List_List*_tmp9A;struct _tuple2*_tmp9C;struct Cyc_Absyn_Enumdecl*_tmp9D;struct Cyc_List_List*_tmp9F;void*_tmpA1;struct Cyc_Absyn_Exp*_tmpA3;void*_tmpA5;void*_tmpA7;void*_tmpA8;struct _tuple2*_tmpAA;struct Cyc_List_List*_tmpAB;struct Cyc_Absyn_Typedefdecl*_tmpAC;void*_tmpB1;struct Cyc_List_List*_tmpB3;void*_tmpB5;struct Cyc_Absyn_Aggrdecl*_tmpB8;struct Cyc_Absyn_Enumdecl*_tmpBB;struct Cyc_Absyn_Datatypedecl*_tmpBE;_LL23: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp66=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp65;if(_tmp66->tag != 0)goto _LL25;}_LL24:
 goto _LL26;_LL25: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp67=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp65;if(_tmp67->tag != 1)goto _LL27;}_LL26:
 return t;_LL27: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp68=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp65;if(_tmp68->tag != 2)goto _LL29;else{_tmp69=_tmp68->f1;}}_LL28: {
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpEF9;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpEF8;return(void*)((_tmpEF8=_cycalloc(sizeof(*_tmpEF8)),((_tmpEF8[0]=((_tmpEF9.tag=2,((_tmpEF9.f1=Cyc_Tcutil_copy_tvar(_tmp69),_tmpEF9)))),_tmpEF8))));}_LL29: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp6A=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp65;if(_tmp6A->tag != 3)goto _LL2B;else{_tmp6B=(_tmp6A->f1).datatype_info;_tmp6C=(_tmp6A->f1).targs;}}_LL2A: {
# 267
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpEFF;struct Cyc_Absyn_DatatypeInfo _tmpEFE;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpEFD;return(void*)((_tmpEFD=_cycalloc(sizeof(*_tmpEFD)),((_tmpEFD[0]=((_tmpEFF.tag=3,((_tmpEFF.f1=((_tmpEFE.datatype_info=_tmp6B,((_tmpEFE.targs=Cyc_Tcutil_copy_types(_tmp6C),_tmpEFE)))),_tmpEFF)))),_tmpEFD))));}_LL2B: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp6D=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp65;if(_tmp6D->tag != 4)goto _LL2D;else{_tmp6E=(_tmp6D->f1).field_info;_tmp6F=(_tmp6D->f1).targs;}}_LL2C: {
# 269
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpF05;struct Cyc_Absyn_DatatypeFieldInfo _tmpF04;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpF03;return(void*)((_tmpF03=_cycalloc(sizeof(*_tmpF03)),((_tmpF03[0]=((_tmpF05.tag=4,((_tmpF05.f1=((_tmpF04.field_info=_tmp6E,((_tmpF04.targs=Cyc_Tcutil_copy_types(_tmp6F),_tmpF04)))),_tmpF05)))),_tmpF03))));}_LL2D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp70=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp65;if(_tmp70->tag != 5)goto _LL2F;else{_tmp71=(_tmp70->f1).elt_typ;_tmp72=(_tmp70->f1).elt_tq;_tmp73=((_tmp70->f1).ptr_atts).rgn;_tmp74=((_tmp70->f1).ptr_atts).nullable;_tmp75=((_tmp70->f1).ptr_atts).bounds;_tmp76=((_tmp70->f1).ptr_atts).zero_term;_tmp77=((_tmp70->f1).ptr_atts).ptrloc;}}_LL2E: {
# 271
void*_tmpC7=Cyc_Tcutil_copy_type(_tmp71);
void*_tmpC8=Cyc_Tcutil_copy_type(_tmp73);
union Cyc_Absyn_Constraint*_tmpC9=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp74);
struct Cyc_Absyn_Tqual _tmpCA=_tmp72;
# 276
union Cyc_Absyn_Constraint*_tmpCB=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp75);
union Cyc_Absyn_Constraint*_tmpCC=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp76);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpF0B;struct Cyc_Absyn_PtrInfo _tmpF0A;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpF09;return(void*)((_tmpF09=_cycalloc(sizeof(*_tmpF09)),((_tmpF09[0]=((_tmpF0B.tag=5,((_tmpF0B.f1=((_tmpF0A.elt_typ=_tmpC7,((_tmpF0A.elt_tq=_tmpCA,((_tmpF0A.ptr_atts=(((_tmpF0A.ptr_atts).rgn=_tmpC8,(((_tmpF0A.ptr_atts).nullable=_tmpC9,(((_tmpF0A.ptr_atts).bounds=_tmpCB,(((_tmpF0A.ptr_atts).zero_term=_tmpCC,(((_tmpF0A.ptr_atts).ptrloc=_tmp77,_tmpF0A.ptr_atts)))))))))),_tmpF0A)))))),_tmpF0B)))),_tmpF09))));}_LL2F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp78=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp65;if(_tmp78->tag != 6)goto _LL31;}_LL30:
 goto _LL32;_LL31: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp79=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp65;if(_tmp79->tag != 7)goto _LL33;}_LL32:
 return t;_LL33: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp7A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp65;if(_tmp7A->tag != 8)goto _LL35;else{_tmp7B=(_tmp7A->f1).elt_type;_tmp7C=(_tmp7A->f1).tq;_tmp7D=(_tmp7A->f1).num_elts;_tmp7E=(_tmp7A->f1).zero_term;_tmp7F=(_tmp7A->f1).zt_loc;}}_LL34: {
# 282
struct Cyc_Absyn_Exp*eopt2=Cyc_Tcutil_deep_copy_exp_opt(1,_tmp7D);
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpF11;struct Cyc_Absyn_ArrayInfo _tmpF10;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpF0F;return(void*)((_tmpF0F=_cycalloc(sizeof(*_tmpF0F)),((_tmpF0F[0]=((_tmpF11.tag=8,((_tmpF11.f1=((_tmpF10.elt_type=Cyc_Tcutil_copy_type(_tmp7B),((_tmpF10.tq=_tmp7C,((_tmpF10.num_elts=eopt2,((_tmpF10.zero_term=
((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp7E),((_tmpF10.zt_loc=_tmp7F,_tmpF10)))))))))),_tmpF11)))),_tmpF0F))));}_LL35: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp80=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp65;if(_tmp80->tag != 9)goto _LL37;else{_tmp81=(_tmp80->f1).tvars;_tmp82=(_tmp80->f1).effect;_tmp83=(_tmp80->f1).ret_tqual;_tmp84=(_tmp80->f1).ret_typ;_tmp85=(_tmp80->f1).args;_tmp86=(_tmp80->f1).c_varargs;_tmp87=(_tmp80->f1).cyc_varargs;_tmp88=(_tmp80->f1).rgn_po;_tmp89=(_tmp80->f1).attributes;_tmp8A=(_tmp80->f1).requires_clause;_tmp8B=(_tmp80->f1).requires_relns;_tmp8C=(_tmp80->f1).ensures_clause;_tmp8D=(_tmp80->f1).ensures_relns;}}_LL36: {
# 286
struct Cyc_List_List*_tmpD3=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp81);
void*effopt2=_tmp82 == 0?0: Cyc_Tcutil_copy_type(_tmp82);
void*_tmpD4=Cyc_Tcutil_copy_type(_tmp84);
struct Cyc_List_List*_tmpD5=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp85);
int _tmpD6=_tmp86;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(_tmp87 != 0){
struct Cyc_Absyn_VarargInfo*cv=_tmp87;
struct Cyc_Absyn_VarargInfo*_tmpF12;cyc_varargs2=((_tmpF12=_cycalloc(sizeof(*_tmpF12)),((_tmpF12->name=cv->name,((_tmpF12->tq=cv->tq,((_tmpF12->type=Cyc_Tcutil_copy_type(cv->type),((_tmpF12->inject=cv->inject,_tmpF12))))))))));}{
# 297
struct Cyc_List_List*_tmpD8=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp88);
struct Cyc_List_List*_tmpD9=_tmp89;
struct Cyc_Absyn_Exp*_tmpDA=Cyc_Tcutil_deep_copy_exp_opt(1,_tmp8A);
struct Cyc_List_List*_tmpDB=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp8B);
struct Cyc_Absyn_Exp*_tmpDC=Cyc_Tcutil_deep_copy_exp_opt(1,_tmp8C);
struct Cyc_List_List*_tmpDD=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp8D);
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpF18;struct Cyc_Absyn_FnInfo _tmpF17;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpF16;return(void*)((_tmpF16=_cycalloc(sizeof(*_tmpF16)),((_tmpF16[0]=((_tmpF18.tag=9,((_tmpF18.f1=((_tmpF17.tvars=_tmpD3,((_tmpF17.effect=effopt2,((_tmpF17.ret_tqual=_tmp83,((_tmpF17.ret_typ=_tmpD4,((_tmpF17.args=_tmpD5,((_tmpF17.c_varargs=_tmpD6,((_tmpF17.cyc_varargs=cyc_varargs2,((_tmpF17.rgn_po=_tmpD8,((_tmpF17.attributes=_tmpD9,((_tmpF17.requires_clause=_tmpDA,((_tmpF17.requires_relns=_tmpDB,((_tmpF17.ensures_clause=_tmpDC,((_tmpF17.ensures_relns=_tmpDD,_tmpF17)))))))))))))))))))))))))),_tmpF18)))),_tmpF16))));};}_LL37: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp8E=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp65;if(_tmp8E->tag != 10)goto _LL39;else{_tmp8F=_tmp8E->f1;}}_LL38: {
# 306
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpF1B;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpF1A;return(void*)((_tmpF1A=_cycalloc(sizeof(*_tmpF1A)),((_tmpF1A[0]=((_tmpF1B.tag=10,((_tmpF1B.f1=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp8F),_tmpF1B)))),_tmpF1A))));}_LL39: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp90=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp65;if(_tmp90->tag != 11)goto _LL3B;else{if((((_tmp90->f1).aggr_info).UnknownAggr).tag != 1)goto _LL3B;_tmp91=((struct _tuple4)(((_tmp90->f1).aggr_info).UnknownAggr).val).f1;_tmp92=((struct _tuple4)(((_tmp90->f1).aggr_info).UnknownAggr).val).f2;_tmp93=((struct _tuple4)(((_tmp90->f1).aggr_info).UnknownAggr).val).f3;_tmp94=(_tmp90->f1).targs;}}_LL3A: {
# 308
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpF21;struct Cyc_Absyn_AggrInfo _tmpF20;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF1F;return(void*)((_tmpF1F=_cycalloc(sizeof(*_tmpF1F)),((_tmpF1F[0]=((_tmpF21.tag=11,((_tmpF21.f1=((_tmpF20.aggr_info=Cyc_Absyn_UnknownAggr(_tmp91,_tmp92,_tmp93),((_tmpF20.targs=Cyc_Tcutil_copy_types(_tmp94),_tmpF20)))),_tmpF21)))),_tmpF1F))));}_LL3B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp95=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp65;if(_tmp95->tag != 11)goto _LL3D;else{if((((_tmp95->f1).aggr_info).KnownAggr).tag != 2)goto _LL3D;_tmp96=(struct Cyc_Absyn_Aggrdecl**)(((_tmp95->f1).aggr_info).KnownAggr).val;_tmp97=(_tmp95->f1).targs;}}_LL3C: {
# 310
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpF27;struct Cyc_Absyn_AggrInfo _tmpF26;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF25;return(void*)((_tmpF25=_cycalloc(sizeof(*_tmpF25)),((_tmpF25[0]=((_tmpF27.tag=11,((_tmpF27.f1=((_tmpF26.aggr_info=Cyc_Absyn_KnownAggr(_tmp96),((_tmpF26.targs=Cyc_Tcutil_copy_types(_tmp97),_tmpF26)))),_tmpF27)))),_tmpF25))));}_LL3D: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp98=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp65;if(_tmp98->tag != 12)goto _LL3F;else{_tmp99=_tmp98->f1;_tmp9A=_tmp98->f2;}}_LL3E: {
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpF2A;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpF29;return(void*)((_tmpF29=_cycalloc(sizeof(*_tmpF29)),((_tmpF29[0]=((_tmpF2A.tag=12,((_tmpF2A.f1=_tmp99,((_tmpF2A.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmp9A),_tmpF2A)))))),_tmpF29))));}_LL3F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp9B=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp65;if(_tmp9B->tag != 13)goto _LL41;else{_tmp9C=_tmp9B->f1;_tmp9D=_tmp9B->f2;}}_LL40: {
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpF2D;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpF2C;return(void*)((_tmpF2C=_cycalloc(sizeof(*_tmpF2C)),((_tmpF2C[0]=((_tmpF2D.tag=13,((_tmpF2D.f1=_tmp9C,((_tmpF2D.f2=_tmp9D,_tmpF2D)))))),_tmpF2C))));}_LL41: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp9E=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp65;if(_tmp9E->tag != 14)goto _LL43;else{_tmp9F=_tmp9E->f1;}}_LL42: {
struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpF30;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpF2F;return(void*)((_tmpF2F=_cycalloc(sizeof(*_tmpF2F)),((_tmpF2F[0]=((_tmpF30.tag=14,((_tmpF30.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmp9F),_tmpF30)))),_tmpF2F))));}_LL43: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpA0=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp65;if(_tmpA0->tag != 19)goto _LL45;else{_tmpA1=(void*)_tmpA0->f1;}}_LL44: {
struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpF33;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpF32;return(void*)((_tmpF32=_cycalloc(sizeof(*_tmpF32)),((_tmpF32[0]=((_tmpF33.tag=19,((_tmpF33.f1=Cyc_Tcutil_copy_type(_tmpA1),_tmpF33)))),_tmpF32))));}_LL45: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpA2=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp65;if(_tmpA2->tag != 18)goto _LL47;else{_tmpA3=_tmpA2->f1;}}_LL46: {
# 317
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpF36;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpF35;return(void*)((_tmpF35=_cycalloc(sizeof(*_tmpF35)),((_tmpF35[0]=((_tmpF36.tag=18,((_tmpF36.f1=_tmpA3,_tmpF36)))),_tmpF35))));}_LL47: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA4=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp65;if(_tmpA4->tag != 15)goto _LL49;else{_tmpA5=(void*)_tmpA4->f1;}}_LL48: {
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpF39;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpF38;return(void*)((_tmpF38=_cycalloc(sizeof(*_tmpF38)),((_tmpF38[0]=((_tmpF39.tag=15,((_tmpF39.f1=Cyc_Tcutil_copy_type(_tmpA5),_tmpF39)))),_tmpF38))));}_LL49: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpA6=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp65;if(_tmpA6->tag != 16)goto _LL4B;else{_tmpA7=(void*)_tmpA6->f1;_tmpA8=(void*)_tmpA6->f2;}}_LL4A: {
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpF3C;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpF3B;return(void*)((_tmpF3B=_cycalloc(sizeof(*_tmpF3B)),((_tmpF3B[0]=((_tmpF3C.tag=16,((_tmpF3C.f1=Cyc_Tcutil_copy_type(_tmpA7),((_tmpF3C.f2=Cyc_Tcutil_copy_type(_tmpA8),_tmpF3C)))))),_tmpF3B))));}_LL4B: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA9=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp65;if(_tmpA9->tag != 17)goto _LL4D;else{_tmpAA=_tmpA9->f1;_tmpAB=_tmpA9->f2;_tmpAC=_tmpA9->f3;}}_LL4C: {
# 321
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpF3F;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpF3E;return(void*)((_tmpF3E=_cycalloc(sizeof(*_tmpF3E)),((_tmpF3E[0]=((_tmpF3F.tag=17,((_tmpF3F.f1=_tmpAA,((_tmpF3F.f2=Cyc_Tcutil_copy_types(_tmpAB),((_tmpF3F.f3=_tmpAC,((_tmpF3F.f4=0,_tmpF3F)))))))))),_tmpF3E))));}_LL4D: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpAD=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp65;if(_tmpAD->tag != 21)goto _LL4F;}_LL4E:
 goto _LL50;_LL4F: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpAE=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp65;if(_tmpAE->tag != 22)goto _LL51;}_LL50:
 goto _LL52;_LL51: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmpAF=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp65;if(_tmpAF->tag != 20)goto _LL53;}_LL52:
 return t;_LL53: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpB0=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp65;if(_tmpB0->tag != 23)goto _LL55;else{_tmpB1=(void*)_tmpB0->f1;}}_LL54: {
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpF42;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpF41;return(void*)((_tmpF41=_cycalloc(sizeof(*_tmpF41)),((_tmpF41[0]=((_tmpF42.tag=23,((_tmpF42.f1=Cyc_Tcutil_copy_type(_tmpB1),_tmpF42)))),_tmpF41))));}_LL55: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpB2=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp65;if(_tmpB2->tag != 24)goto _LL57;else{_tmpB3=_tmpB2->f1;}}_LL56: {
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF45;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF44;return(void*)((_tmpF44=_cycalloc(sizeof(*_tmpF44)),((_tmpF44[0]=((_tmpF45.tag=24,((_tmpF45.f1=Cyc_Tcutil_copy_types(_tmpB3),_tmpF45)))),_tmpF44))));}_LL57: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpB4=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp65;if(_tmpB4->tag != 25)goto _LL59;else{_tmpB5=(void*)_tmpB4->f1;}}_LL58: {
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpF48;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpF47;return(void*)((_tmpF47=_cycalloc(sizeof(*_tmpF47)),((_tmpF47[0]=((_tmpF48.tag=25,((_tmpF48.f1=Cyc_Tcutil_copy_type(_tmpB5),_tmpF48)))),_tmpF47))));}_LL59: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB6=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp65;if(_tmpB6->tag != 26)goto _LL5B;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmpB7=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmpB6->f1)->r;if(_tmpB7->tag != 0)goto _LL5B;else{_tmpB8=_tmpB7->f1;}}}_LL5A: {
# 330
struct Cyc_List_List*_tmpFF=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpB8->tvs);
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpF4E;struct Cyc_Absyn_AggrInfo _tmpF4D;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF4C;return(void*)((_tmpF4C=_cycalloc(sizeof(*_tmpF4C)),((_tmpF4C[0]=((_tmpF4E.tag=11,((_tmpF4E.f1=((_tmpF4D.aggr_info=Cyc_Absyn_UnknownAggr(_tmpB8->kind,_tmpB8->name,0),((_tmpF4D.targs=_tmpFF,_tmpF4D)))),_tmpF4E)))),_tmpF4C))));}_LL5B: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB9=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp65;if(_tmpB9->tag != 26)goto _LL5D;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpBA=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmpB9->f1)->r;if(_tmpBA->tag != 1)goto _LL5D;else{_tmpBB=_tmpBA->f1;}}}_LL5C: {
# 333
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpF51;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpF50;return(void*)((_tmpF50=_cycalloc(sizeof(*_tmpF50)),((_tmpF50[0]=((_tmpF51.tag=13,((_tmpF51.f1=_tmpBB->name,((_tmpF51.f2=0,_tmpF51)))))),_tmpF50))));}_LL5D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpBC=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp65;if(_tmpBC->tag != 26)goto _LL22;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmpBD=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmpBC->f1)->r;if(_tmpBD->tag != 2)goto _LL22;else{_tmpBE=_tmpBD->f1;}}}_LL5E: {
# 335
struct Cyc_List_List*_tmp105=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpBE->tvs);
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpF5B;struct Cyc_Absyn_UnknownDatatypeInfo _tmpF5A;struct Cyc_Absyn_DatatypeInfo _tmpF59;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpF58;return(void*)((_tmpF58=_cycalloc(sizeof(*_tmpF58)),((_tmpF58[0]=((_tmpF5B.tag=3,((_tmpF5B.f1=((_tmpF59.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpF5A.name=_tmpBE->name,((_tmpF5A.is_extensible=0,_tmpF5A))))),((_tmpF59.targs=_tmp105,_tmpF59)))),_tmpF5B)))),_tmpF58))));}_LL22:;}
# 350 "tcutil.cyc"
static void*Cyc_Tcutil_copy_designator(int pt,void*d){
void*_tmp10A=d;struct Cyc_Absyn_Exp*_tmp10C;struct _dyneither_ptr*_tmp10E;_LL60: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp10B=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp10A;if(_tmp10B->tag != 0)goto _LL62;else{_tmp10C=_tmp10B->f1;}}_LL61: {
struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpF5E;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpF5D;return(void*)((_tmpF5D=_cycalloc(sizeof(*_tmpF5D)),((_tmpF5D[0]=((_tmpF5E.tag=0,((_tmpF5E.f1=Cyc_Tcutil_deep_copy_exp(pt,_tmp10C),_tmpF5E)))),_tmpF5D))));}_LL62: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp10D=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp10A;if(_tmp10D->tag != 1)goto _LL5F;else{_tmp10E=_tmp10D->f1;}}_LL63:
 return d;_LL5F:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 356
static struct _tuple19*Cyc_Tcutil_copy_eds(int pt,struct _tuple19*e){
# 358
struct _tuple19*_tmpF5F;return(_tmpF5F=_cycalloc(sizeof(*_tmpF5F)),((_tmpF5F->f1=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,pt,(e[0]).f1),((_tmpF5F->f2=Cyc_Tcutil_deep_copy_exp(pt,(e[0]).f2),_tmpF5F)))));}
# 361
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*new_e;
int _tmp112=preserve_types;
{void*_tmp113=e->r;void*_tmp114=_tmp113;union Cyc_Absyn_Cnst _tmp116;struct _tuple2*_tmp118;void*_tmp119;enum Cyc_Absyn_Primop _tmp11B;struct Cyc_List_List*_tmp11C;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Core_Opt*_tmp11F;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_Absyn_Exp*_tmp122;enum Cyc_Absyn_Incrementor _tmp123;struct Cyc_Absyn_Exp*_tmp125;struct Cyc_Absyn_Exp*_tmp126;struct Cyc_Absyn_Exp*_tmp127;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_Absyn_Exp*_tmp12A;struct Cyc_Absyn_Exp*_tmp12C;struct Cyc_Absyn_Exp*_tmp12D;struct Cyc_Absyn_Exp*_tmp12F;struct Cyc_Absyn_Exp*_tmp130;struct Cyc_Absyn_Exp*_tmp132;struct Cyc_List_List*_tmp133;struct Cyc_Absyn_VarargCallInfo*_tmp134;int _tmp135;struct Cyc_Absyn_Exp*_tmp137;int _tmp138;struct Cyc_Absyn_Exp*_tmp13A;struct Cyc_Absyn_Exp*_tmp13C;struct Cyc_List_List*_tmp13D;void*_tmp13F;struct Cyc_Absyn_Exp*_tmp140;int _tmp141;enum Cyc_Absyn_Coercion _tmp142;struct Cyc_Absyn_Exp*_tmp144;struct Cyc_Absyn_Exp*_tmp146;struct Cyc_Absyn_Exp*_tmp147;void*_tmp149;struct Cyc_Absyn_Exp*_tmp14B;void*_tmp14D;struct Cyc_List_List*_tmp14E;struct Cyc_Absyn_Exp*_tmp150;struct Cyc_Absyn_Exp*_tmp152;struct _dyneither_ptr*_tmp153;int _tmp154;int _tmp155;struct Cyc_Absyn_Exp*_tmp157;struct _dyneither_ptr*_tmp158;int _tmp159;int _tmp15A;struct Cyc_Absyn_Exp*_tmp15C;struct Cyc_Absyn_Exp*_tmp15D;struct Cyc_List_List*_tmp15F;struct _dyneither_ptr*_tmp161;struct Cyc_Absyn_Tqual _tmp162;void*_tmp163;struct Cyc_List_List*_tmp164;struct Cyc_List_List*_tmp166;struct Cyc_Absyn_Vardecl*_tmp168;struct Cyc_Absyn_Exp*_tmp169;struct Cyc_Absyn_Exp*_tmp16A;int _tmp16B;struct Cyc_Absyn_Exp*_tmp16D;void*_tmp16E;int _tmp16F;struct _tuple2*_tmp171;struct Cyc_List_List*_tmp172;struct Cyc_List_List*_tmp173;struct Cyc_Absyn_Aggrdecl*_tmp174;void*_tmp176;struct Cyc_List_List*_tmp177;struct Cyc_List_List*_tmp179;struct Cyc_Absyn_Datatypedecl*_tmp17A;struct Cyc_Absyn_Datatypefield*_tmp17B;struct _tuple2*_tmp17D;struct Cyc_Absyn_Enumdecl*_tmp17E;struct Cyc_Absyn_Enumfield*_tmp17F;struct _tuple2*_tmp181;void*_tmp182;struct Cyc_Absyn_Enumfield*_tmp183;int _tmp185;struct Cyc_Absyn_Exp*_tmp186;void**_tmp187;struct Cyc_Absyn_Exp*_tmp188;int _tmp189;struct Cyc_Absyn_Exp*_tmp18B;struct Cyc_Absyn_Exp*_tmp18C;struct Cyc_Core_Opt*_tmp18E;struct Cyc_List_List*_tmp18F;struct Cyc_Absyn_Exp*_tmp192;struct _dyneither_ptr*_tmp193;void*_tmp195;int _tmp197;struct _dyneither_ptr _tmp198;_LL65: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp115=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp115->tag != 0)goto _LL67;else{_tmp116=_tmp115->f1;}}_LL66:
 new_e=Cyc_Absyn_const_exp(_tmp116,e->loc);goto _LL64;_LL67: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp117=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp117->tag != 1)goto _LL69;else{_tmp118=_tmp117->f1;_tmp119=(void*)_tmp117->f2;}}_LL68:
 new_e=Cyc_Absyn_varb_exp(_tmp118,_tmp119,e->loc);goto _LL64;_LL69: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp11A=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp11A->tag != 2)goto _LL6B;else{_tmp11B=_tmp11A->f1;_tmp11C=_tmp11A->f2;}}_LL6A:
 new_e=Cyc_Absyn_primop_exp(_tmp11B,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp112,_tmp11C),e->loc);goto _LL64;_LL6B: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp11D=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp11D->tag != 3)goto _LL6D;else{_tmp11E=_tmp11D->f1;_tmp11F=_tmp11D->f2;_tmp120=_tmp11D->f3;}}_LL6C:
# 369
{struct Cyc_Core_Opt*_tmpF60;new_e=Cyc_Absyn_assignop_exp(Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp11E),(unsigned int)_tmp11F?(_tmpF60=_cycalloc_atomic(sizeof(*_tmpF60)),((_tmpF60->v=(void*)((enum Cyc_Absyn_Primop)_tmp11F->v),_tmpF60))): 0,Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp120),e->loc);}
goto _LL64;_LL6D: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp121=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp121->tag != 4)goto _LL6F;else{_tmp122=_tmp121->f1;_tmp123=_tmp121->f2;}}_LL6E:
 new_e=Cyc_Absyn_increment_exp(Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp122),_tmp123,e->loc);goto _LL64;_LL6F: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp124=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp124->tag != 5)goto _LL71;else{_tmp125=_tmp124->f1;_tmp126=_tmp124->f2;_tmp127=_tmp124->f3;}}_LL70:
# 373
 new_e=Cyc_Absyn_conditional_exp(Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp125),Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp126),Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp127),e->loc);goto _LL64;_LL71: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp128=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp128->tag != 6)goto _LL73;else{_tmp129=_tmp128->f1;_tmp12A=_tmp128->f2;}}_LL72:
 new_e=Cyc_Absyn_and_exp(Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp129),Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp12A),e->loc);goto _LL64;_LL73: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp12B=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp12B->tag != 7)goto _LL75;else{_tmp12C=_tmp12B->f1;_tmp12D=_tmp12B->f2;}}_LL74:
 new_e=Cyc_Absyn_or_exp(Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp12C),Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp12D),e->loc);goto _LL64;_LL75: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp12E=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp12E->tag != 8)goto _LL77;else{_tmp12F=_tmp12E->f1;_tmp130=_tmp12E->f2;}}_LL76:
 new_e=Cyc_Absyn_seq_exp(Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp12F),Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp130),e->loc);goto _LL64;_LL77: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp131=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp131->tag != 9)goto _LL79;else{_tmp132=_tmp131->f1;_tmp133=_tmp131->f2;_tmp134=_tmp131->f3;_tmp135=_tmp131->f4;}}_LL78:
# 378
{struct Cyc_Absyn_VarargCallInfo*_tmp19A=_tmp134;int _tmp19B;struct Cyc_List_List*_tmp19C;struct Cyc_Absyn_VarargInfo*_tmp19D;_LLB6: if(_tmp19A == 0)goto _LLB8;_tmp19B=_tmp19A->num_varargs;_tmp19C=_tmp19A->injectors;_tmp19D=_tmp19A->vai;_LLB7: {
# 380
struct _dyneither_ptr*_tmp19F;struct Cyc_Absyn_Tqual _tmp1A0;void*_tmp1A1;int _tmp1A2;struct Cyc_Absyn_VarargInfo*_tmp19E=_tmp19D;_tmp19F=_tmp19E->name;_tmp1A0=_tmp19E->tq;_tmp1A1=_tmp19E->type;_tmp1A2=_tmp19E->inject;
{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpF6A;struct Cyc_Absyn_VarargInfo*_tmpF69;struct Cyc_Absyn_VarargCallInfo*_tmpF68;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpF67;new_e=Cyc_Absyn_new_exp((void*)((_tmpF67=_cycalloc(sizeof(*_tmpF67)),((_tmpF67[0]=((_tmpF6A.tag=9,((_tmpF6A.f1=
Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp132),((_tmpF6A.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp112,_tmp133),((_tmpF6A.f3=(
(_tmpF68=_cycalloc(sizeof(*_tmpF68)),((_tmpF68->num_varargs=_tmp19B,((_tmpF68->injectors=_tmp19C,((_tmpF68->vai=(
(_tmpF69=_cycalloc(sizeof(*_tmpF69)),((_tmpF69->name=_tmp19F,((_tmpF69->tq=_tmp1A0,((_tmpF69->type=Cyc_Tcutil_copy_type(_tmp1A1),((_tmpF69->inject=_tmp1A2,_tmpF69)))))))))),_tmpF68)))))))),((_tmpF6A.f4=_tmp135,_tmpF6A)))))))))),_tmpF67)))),e->loc);}
# 386
goto _LLB5;}_LLB8:;_LLB9:
# 388
{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpF6D;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpF6C;new_e=Cyc_Absyn_new_exp((void*)((_tmpF6C=_cycalloc(sizeof(*_tmpF6C)),((_tmpF6C[0]=((_tmpF6D.tag=9,((_tmpF6D.f1=Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp132),((_tmpF6D.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp112,_tmp133),((_tmpF6D.f3=_tmp134,((_tmpF6D.f4=_tmp135,_tmpF6D)))))))))),_tmpF6C)))),e->loc);}
goto _LLB5;_LLB5:;}
# 391
goto _LL64;_LL79: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp136=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp136->tag != 10)goto _LL7B;else{_tmp137=_tmp136->f1;_tmp138=_tmp136->f2;}}_LL7A:
# 393
 new_e=_tmp138?Cyc_Absyn_rethrow_exp(Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp137),e->loc): Cyc_Absyn_throw_exp(Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp137),e->loc);
goto _LL64;_LL7B: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp139=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp139->tag != 11)goto _LL7D;else{_tmp13A=_tmp139->f1;}}_LL7C:
 new_e=Cyc_Absyn_noinstantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp13A),e->loc);
goto _LL64;_LL7D: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp13B=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp13B->tag != 12)goto _LL7F;else{_tmp13C=_tmp13B->f1;_tmp13D=_tmp13B->f2;}}_LL7E:
# 398
 new_e=Cyc_Absyn_instantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp13C),((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp13D),e->loc);
goto _LL64;_LL7F: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp13E=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp13E->tag != 13)goto _LL81;else{_tmp13F=(void*)_tmp13E->f1;_tmp140=_tmp13E->f2;_tmp141=_tmp13E->f3;_tmp142=_tmp13E->f4;}}_LL80:
# 401
 new_e=Cyc_Absyn_cast_exp(Cyc_Tcutil_copy_type(_tmp13F),Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp140),_tmp141,_tmp142,e->loc);goto _LL64;_LL81: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp143=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp143->tag != 14)goto _LL83;else{_tmp144=_tmp143->f1;}}_LL82:
 new_e=Cyc_Absyn_address_exp(Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp144),e->loc);goto _LL64;_LL83: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp145=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp145->tag != 15)goto _LL85;else{_tmp146=_tmp145->f1;_tmp147=_tmp145->f2;}}_LL84: {
# 404
struct Cyc_Absyn_Exp*eo1=_tmp146;if(_tmp146 != 0)eo1=Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp146);
new_e=Cyc_Absyn_New_exp(eo1,Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp147),e->loc);
goto _LL64;}_LL85: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp148=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp148->tag != 16)goto _LL87;else{_tmp149=(void*)_tmp148->f1;}}_LL86:
 new_e=Cyc_Absyn_sizeoftyp_exp(Cyc_Tcutil_copy_type(_tmp149),e->loc);
goto _LL64;_LL87: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp14A=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp14A->tag != 17)goto _LL89;else{_tmp14B=_tmp14A->f1;}}_LL88:
 new_e=Cyc_Absyn_sizeofexp_exp(Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp14B),e->loc);goto _LL64;_LL89: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp14C=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp14C->tag != 18)goto _LL8B;else{_tmp14D=(void*)_tmp14C->f1;_tmp14E=_tmp14C->f2;}}_LL8A:
# 411
 new_e=Cyc_Absyn_offsetof_exp(Cyc_Tcutil_copy_type(_tmp14D),_tmp14E,e->loc);goto _LL64;_LL8B: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp14F=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp14F->tag != 19)goto _LL8D;else{_tmp150=_tmp14F->f1;}}_LL8C:
 new_e=Cyc_Absyn_deref_exp(Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp150),e->loc);goto _LL64;_LL8D: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp151=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp151->tag != 20)goto _LL8F;else{_tmp152=_tmp151->f1;_tmp153=_tmp151->f2;_tmp154=_tmp151->f3;_tmp155=_tmp151->f4;}}_LL8E:
# 414
{struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpF70;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpF6F;new_e=Cyc_Absyn_new_exp((void*)((_tmpF6F=_cycalloc(sizeof(*_tmpF6F)),((_tmpF6F[0]=((_tmpF70.tag=20,((_tmpF70.f1=Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp152),((_tmpF70.f2=_tmp153,((_tmpF70.f3=_tmp154,((_tmpF70.f4=_tmp155,_tmpF70)))))))))),_tmpF6F)))),e->loc);}goto _LL64;_LL8F: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp156=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp156->tag != 21)goto _LL91;else{_tmp157=_tmp156->f1;_tmp158=_tmp156->f2;_tmp159=_tmp156->f3;_tmp15A=_tmp156->f4;}}_LL90:
# 416
{struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpF73;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpF72;new_e=Cyc_Absyn_new_exp((void*)((_tmpF72=_cycalloc(sizeof(*_tmpF72)),((_tmpF72[0]=((_tmpF73.tag=21,((_tmpF73.f1=Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp157),((_tmpF73.f2=_tmp158,((_tmpF73.f3=_tmp159,((_tmpF73.f4=_tmp15A,_tmpF73)))))))))),_tmpF72)))),e->loc);}goto _LL64;_LL91: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp15B=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp15B->tag != 22)goto _LL93;else{_tmp15C=_tmp15B->f1;_tmp15D=_tmp15B->f2;}}_LL92:
 new_e=Cyc_Absyn_subscript_exp(Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp15C),Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp15D),e->loc);
goto _LL64;_LL93: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp15E=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp15E->tag != 23)goto _LL95;else{_tmp15F=_tmp15E->f1;}}_LL94:
 new_e=Cyc_Absyn_tuple_exp(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp112,_tmp15F),e->loc);goto _LL64;_LL95: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp160=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp160->tag != 24)goto _LL97;else{_tmp161=(_tmp160->f1)->f1;_tmp162=(_tmp160->f1)->f2;_tmp163=(_tmp160->f1)->f3;_tmp164=_tmp160->f2;}}_LL96: {
# 421
struct _dyneither_ptr*vopt1=_tmp161;
if(_tmp161 != 0)vopt1=_tmp161;
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpF79;struct _tuple10*_tmpF78;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpF77;new_e=Cyc_Absyn_new_exp((void*)((_tmpF77=_cycalloc(sizeof(*_tmpF77)),((_tmpF77[0]=((_tmpF79.tag=24,((_tmpF79.f1=((_tmpF78=_cycalloc(sizeof(*_tmpF78)),((_tmpF78->f1=vopt1,((_tmpF78->f2=_tmp162,((_tmpF78->f3=Cyc_Tcutil_copy_type(_tmp163),_tmpF78)))))))),((_tmpF79.f2=
((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp112,_tmp164),_tmpF79)))))),_tmpF77)))),e->loc);}
goto _LL64;}_LL97: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp165=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp165->tag != 25)goto _LL99;else{_tmp166=_tmp165->f1;}}_LL98:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmpF7C;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpF7B;new_e=Cyc_Absyn_new_exp((void*)((_tmpF7B=_cycalloc(sizeof(*_tmpF7B)),((_tmpF7B[0]=((_tmpF7C.tag=25,((_tmpF7C.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp112,_tmp166),_tmpF7C)))),_tmpF7B)))),e->loc);}
goto _LL64;_LL99: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp167=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp167->tag != 26)goto _LL9B;else{_tmp168=_tmp167->f1;_tmp169=_tmp167->f2;_tmp16A=_tmp167->f3;_tmp16B=_tmp167->f4;}}_LL9A:
# 429
{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpF7F;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpF7E;new_e=Cyc_Absyn_new_exp((void*)((_tmpF7E=_cycalloc(sizeof(*_tmpF7E)),((_tmpF7E[0]=((_tmpF7F.tag=26,((_tmpF7F.f1=_tmp168,((_tmpF7F.f2=Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp169),((_tmpF7F.f3=Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp16A),((_tmpF7F.f4=_tmp16B,_tmpF7F)))))))))),_tmpF7E)))),e->loc);}
goto _LL64;_LL9B: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp16C=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp16C->tag != 27)goto _LL9D;else{_tmp16D=_tmp16C->f1;_tmp16E=(void*)_tmp16C->f2;_tmp16F=_tmp16C->f3;}}_LL9C:
# 432
{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpF82;struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpF81;new_e=Cyc_Absyn_new_exp((void*)((_tmpF81=_cycalloc(sizeof(*_tmpF81)),((_tmpF81[0]=((_tmpF82.tag=27,((_tmpF82.f1=Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp16D),((_tmpF82.f2=Cyc_Tcutil_copy_type(_tmp16E),((_tmpF82.f3=_tmp16F,_tmpF82)))))))),_tmpF81)))),_tmp16D->loc);}
# 434
goto _LL64;_LL9D: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp170=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp170->tag != 28)goto _LL9F;else{_tmp171=_tmp170->f1;_tmp172=_tmp170->f2;_tmp173=_tmp170->f3;_tmp174=_tmp170->f4;}}_LL9E:
# 436
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpF85;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpF84;new_e=Cyc_Absyn_new_exp((void*)((_tmpF84=_cycalloc(sizeof(*_tmpF84)),((_tmpF84[0]=((_tmpF85.tag=28,((_tmpF85.f1=_tmp171,((_tmpF85.f2=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp172),((_tmpF85.f3=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp112,_tmp173),((_tmpF85.f4=_tmp174,_tmpF85)))))))))),_tmpF84)))),e->loc);}
# 438
goto _LL64;_LL9F: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp175=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp175->tag != 29)goto _LLA1;else{_tmp176=(void*)_tmp175->f1;_tmp177=_tmp175->f2;}}_LLA0:
# 440
{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmpF88;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpF87;new_e=Cyc_Absyn_new_exp((void*)((_tmpF87=_cycalloc(sizeof(*_tmpF87)),((_tmpF87[0]=((_tmpF88.tag=29,((_tmpF88.f1=Cyc_Tcutil_copy_type(_tmp176),((_tmpF88.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp112,_tmp177),_tmpF88)))))),_tmpF87)))),e->loc);}
goto _LL64;_LLA1: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp178=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp178->tag != 30)goto _LLA3;else{_tmp179=_tmp178->f1;_tmp17A=_tmp178->f2;_tmp17B=_tmp178->f3;}}_LLA2:
# 443
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmpF8B;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpF8A;new_e=Cyc_Absyn_new_exp((void*)((_tmpF8A=_cycalloc(sizeof(*_tmpF8A)),((_tmpF8A[0]=((_tmpF8B.tag=30,((_tmpF8B.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp112,_tmp179),((_tmpF8B.f2=_tmp17A,((_tmpF8B.f3=_tmp17B,_tmpF8B)))))))),_tmpF8A)))),e->loc);}
goto _LL64;_LLA3: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp17C=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp17C->tag != 31)goto _LLA5;else{_tmp17D=_tmp17C->f1;_tmp17E=_tmp17C->f2;_tmp17F=_tmp17C->f3;}}_LLA4:
 new_e=e;goto _LL64;_LLA5: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp180=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp180->tag != 32)goto _LLA7;else{_tmp181=_tmp180->f1;_tmp182=(void*)_tmp180->f2;_tmp183=_tmp180->f3;}}_LLA6:
# 447
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpF8E;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpF8D;new_e=Cyc_Absyn_new_exp((void*)((_tmpF8D=_cycalloc(sizeof(*_tmpF8D)),((_tmpF8D[0]=((_tmpF8E.tag=32,((_tmpF8E.f1=_tmp181,((_tmpF8E.f2=Cyc_Tcutil_copy_type(_tmp182),((_tmpF8E.f3=_tmp183,_tmpF8E)))))))),_tmpF8D)))),e->loc);}
goto _LL64;_LLA7: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp184=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp184->tag != 33)goto _LLA9;else{_tmp185=(_tmp184->f1).is_calloc;_tmp186=(_tmp184->f1).rgn;_tmp187=(_tmp184->f1).elt_type;_tmp188=(_tmp184->f1).num_elts;_tmp189=(_tmp184->f1).fat_result;}}_LLA8: {
# 450
struct Cyc_Absyn_Exp*_tmp1BE=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp186;if(_tmp186 != 0)r1=Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp186);{
void**t1=_tmp187;if(_tmp187 != 0){void**_tmpF8F;t1=((_tmpF8F=_cycalloc(sizeof(*_tmpF8F)),((_tmpF8F[0]=Cyc_Tcutil_copy_type(*_tmp187),_tmpF8F))));}
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpF95;struct Cyc_Absyn_MallocInfo _tmpF94;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpF93;_tmp1BE->r=(void*)((_tmpF93=_cycalloc(sizeof(*_tmpF93)),((_tmpF93[0]=((_tmpF95.tag=33,((_tmpF95.f1=((_tmpF94.is_calloc=_tmp185,((_tmpF94.rgn=r1,((_tmpF94.elt_type=t1,((_tmpF94.num_elts=_tmp188,((_tmpF94.fat_result=_tmp189,_tmpF94)))))))))),_tmpF95)))),_tmpF93))));}
new_e=_tmp1BE;
goto _LL64;};}_LLA9: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp18A=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp18A->tag != 34)goto _LLAB;else{_tmp18B=_tmp18A->f1;_tmp18C=_tmp18A->f2;}}_LLAA:
 new_e=Cyc_Absyn_swap_exp(Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp18B),Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp18C),e->loc);goto _LL64;_LLAB: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp18D=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp18D->tag != 35)goto _LLAD;else{_tmp18E=_tmp18D->f1;_tmp18F=_tmp18D->f2;}}_LLAC: {
# 458
struct Cyc_Core_Opt*nopt1=_tmp18E;
if(_tmp18E != 0){struct Cyc_Core_Opt*_tmpF96;nopt1=((_tmpF96=_cycalloc(sizeof(*_tmpF96)),((_tmpF96->v=(struct _tuple2*)_tmp18E->v,_tmpF96))));}
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpF99;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpF98;new_e=Cyc_Absyn_new_exp((void*)((_tmpF98=_cycalloc(sizeof(*_tmpF98)),((_tmpF98[0]=((_tmpF99.tag=35,((_tmpF99.f1=nopt1,((_tmpF99.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp112,_tmp18F),_tmpF99)))))),_tmpF98)))),e->loc);}
goto _LL64;}_LLAD: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp190=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp190->tag != 36)goto _LLAF;}_LLAE: {
# 463
struct Cyc_Core_Failure_exn_struct _tmpF9F;const char*_tmpF9E;struct Cyc_Core_Failure_exn_struct*_tmpF9D;(int)_throw((void*)((_tmpF9D=_cycalloc(sizeof(*_tmpF9D)),((_tmpF9D[0]=((_tmpF9F.tag=Cyc_Core_Failure,((_tmpF9F.f1=((_tmpF9E="deep_copy: statement expressions unsupported",_tag_dyneither(_tmpF9E,sizeof(char),45))),_tmpF9F)))),_tmpF9D)))));}_LLAF: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp191=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp191->tag != 37)goto _LLB1;else{_tmp192=_tmp191->f1;_tmp193=_tmp191->f2;}}_LLB0:
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpFA2;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpFA1;new_e=Cyc_Absyn_new_exp((void*)((_tmpFA1=_cycalloc(sizeof(*_tmpFA1)),((_tmpFA1[0]=((_tmpFA2.tag=37,((_tmpFA2.f1=Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp192),((_tmpFA2.f2=_tmp193,_tmpFA2)))))),_tmpFA1)))),e->loc);}
goto _LL64;_LLB1: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp194=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp194->tag != 38)goto _LLB3;else{_tmp195=(void*)_tmp194->f1;}}_LLB2:
 new_e=Cyc_Absyn_valueof_exp(Cyc_Tcutil_copy_type(_tmp195),e->loc);
goto _LL64;_LLB3: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp196=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp196->tag != 39)goto _LL64;else{_tmp197=_tmp196->f1;_tmp198=_tmp196->f2;}}_LLB4:
 new_e=Cyc_Absyn_asm_exp(_tmp197,_tmp198,e->loc);goto _LL64;_LL64:;}
# 471
if(preserve_types)new_e->topt=e->topt;
return new_e;}
# 475
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*e){
if(e == 0)return 0;else{
return Cyc_Tcutil_deep_copy_exp(preserve_types,e);}}struct _tuple20{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple21{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 488 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
enum Cyc_Absyn_KindQual _tmp1CC;enum Cyc_Absyn_AliasQual _tmp1CD;struct Cyc_Absyn_Kind*_tmp1CB=ka1;_tmp1CC=_tmp1CB->kind;_tmp1CD=_tmp1CB->aliasqual;{
enum Cyc_Absyn_KindQual _tmp1CF;enum Cyc_Absyn_AliasQual _tmp1D0;struct Cyc_Absyn_Kind*_tmp1CE=ka2;_tmp1CF=_tmp1CE->kind;_tmp1D0=_tmp1CE->aliasqual;
# 492
if(_tmp1CC != _tmp1CF){
struct _tuple20 _tmpFA3;struct _tuple20 _tmp1D1=(_tmpFA3.f1=_tmp1CC,((_tmpFA3.f2=_tmp1CF,_tmpFA3)));struct _tuple20 _tmp1D2=_tmp1D1;_LLBB: if(_tmp1D2.f1 != Cyc_Absyn_BoxKind)goto _LLBD;if(_tmp1D2.f2 != Cyc_Absyn_MemKind)goto _LLBD;_LLBC:
 goto _LLBE;_LLBD: if(_tmp1D2.f1 != Cyc_Absyn_BoxKind)goto _LLBF;if(_tmp1D2.f2 != Cyc_Absyn_AnyKind)goto _LLBF;_LLBE:
 goto _LLC0;_LLBF: if(_tmp1D2.f1 != Cyc_Absyn_MemKind)goto _LLC1;if(_tmp1D2.f2 != Cyc_Absyn_AnyKind)goto _LLC1;_LLC0:
 goto _LLBA;_LLC1:;_LLC2:
 return 0;_LLBA:;}
# 501
if(_tmp1CD != _tmp1D0){
struct _tuple21 _tmpFA4;struct _tuple21 _tmp1D4=(_tmpFA4.f1=_tmp1CD,((_tmpFA4.f2=_tmp1D0,_tmpFA4)));struct _tuple21 _tmp1D5=_tmp1D4;_LLC4: if(_tmp1D5.f1 != Cyc_Absyn_Aliasable)goto _LLC6;if(_tmp1D5.f2 != Cyc_Absyn_Top)goto _LLC6;_LLC5:
 goto _LLC7;_LLC6: if(_tmp1D5.f1 != Cyc_Absyn_Unique)goto _LLC8;if(_tmp1D5.f2 != Cyc_Absyn_Top)goto _LLC8;_LLC7:
 return 1;_LLC8:;_LLC9:
 return 0;_LLC3:;}
# 508
return 1;};}
# 511
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp1D7=Cyc_Absyn_compress_kb(tv->kind);void*_tmp1D8=_tmp1D7;struct Cyc_Absyn_Kind*_tmp1DA;struct Cyc_Absyn_Kind*_tmp1DC;_LLCB: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1D9=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1D8;if(_tmp1D9->tag != 0)goto _LLCD;else{_tmp1DA=_tmp1D9->f1;}}_LLCC:
 return _tmp1DA;_LLCD: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1DB=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1D8;if(_tmp1DB->tag != 2)goto _LLCF;else{_tmp1DC=_tmp1DB->f2;}}_LLCE:
 return _tmp1DC;_LLCF:;_LLD0:
# 516
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpFA7;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpFA6;tv->kind=(void*)((_tmpFA6=_cycalloc(sizeof(*_tmpFA6)),((_tmpFA6[0]=((_tmpFA7.tag=2,((_tmpFA7.f1=0,((_tmpFA7.f2=def,_tmpFA7)))))),_tmpFA6))));}
return def;_LLCA:;}
# 521
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
struct _tuple0 _tmpFA8;struct _tuple0 _tmp1DF=(_tmpFA8.f1=Cyc_Absyn_compress_kb(kb1),((_tmpFA8.f2=Cyc_Absyn_compress_kb(kb2),_tmpFA8)));struct _tuple0 _tmp1E0=_tmp1DF;struct Cyc_Absyn_Kind*_tmp1E2;struct Cyc_Absyn_Kind*_tmp1E4;struct Cyc_Core_Opt**_tmp1E6;struct Cyc_Absyn_Kind*_tmp1E7;struct Cyc_Absyn_Kind*_tmp1E9;struct Cyc_Absyn_Kind*_tmp1EB;struct Cyc_Core_Opt**_tmp1ED;struct Cyc_Absyn_Kind*_tmp1EE;struct Cyc_Core_Opt**_tmp1F0;struct Cyc_Absyn_Kind*_tmp1F1;struct Cyc_Core_Opt**_tmp1F3;struct Cyc_Absyn_Kind*_tmp1F4;struct Cyc_Core_Opt**_tmp1F6;void*_tmp1F7;void*_tmp1F8;struct Cyc_Core_Opt**_tmp1FA;_LLD2:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1E1=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1E0.f1;if(_tmp1E1->tag != 0)goto _LLD4;else{_tmp1E2=_tmp1E1->f1;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1E3=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1E0.f2;if(_tmp1E3->tag != 0)goto _LLD4;else{_tmp1E4=_tmp1E3->f1;}};_LLD3:
 return _tmp1E2 == _tmp1E4;_LLD4:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1E5=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1E0.f1;if(_tmp1E5->tag != 2)goto _LLD6;else{_tmp1E6=(struct Cyc_Core_Opt**)& _tmp1E5->f1;_tmp1E7=_tmp1E5->f2;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1E8=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1E0.f2;if(_tmp1E8->tag != 0)goto _LLD6;else{_tmp1E9=_tmp1E8->f1;}};_LLD5:
# 525
 if(Cyc_Tcutil_kind_leq(_tmp1E9,_tmp1E7)){
{struct Cyc_Core_Opt*_tmpFA9;*_tmp1E6=((_tmpFA9=_cycalloc(sizeof(*_tmpFA9)),((_tmpFA9->v=kb2,_tmpFA9))));}
return 1;}else{
return 0;}_LLD6:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1EA=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1E0.f1;if(_tmp1EA->tag != 0)goto _LLD8;else{_tmp1EB=_tmp1EA->f1;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1EC=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1E0.f2;if(_tmp1EC->tag != 2)goto _LLD8;else{_tmp1ED=(struct Cyc_Core_Opt**)& _tmp1EC->f1;_tmp1EE=_tmp1EC->f2;}};_LLD7:
# 530
 if(Cyc_Tcutil_kind_leq(_tmp1EB,_tmp1EE)){
{struct Cyc_Core_Opt*_tmpFAA;*_tmp1ED=((_tmpFAA=_cycalloc(sizeof(*_tmpFAA)),((_tmpFAA->v=kb1,_tmpFAA))));}
return 1;}else{
return 0;}_LLD8:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1EF=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1E0.f1;if(_tmp1EF->tag != 2)goto _LLDA;else{_tmp1F0=(struct Cyc_Core_Opt**)& _tmp1EF->f1;_tmp1F1=_tmp1EF->f2;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1F2=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1E0.f2;if(_tmp1F2->tag != 2)goto _LLDA;else{_tmp1F3=(struct Cyc_Core_Opt**)& _tmp1F2->f1;_tmp1F4=_tmp1F2->f2;}};_LLD9:
# 535
 if(Cyc_Tcutil_kind_leq(_tmp1F1,_tmp1F4)){
{struct Cyc_Core_Opt*_tmpFAB;*_tmp1F3=((_tmpFAB=_cycalloc(sizeof(*_tmpFAB)),((_tmpFAB->v=kb1,_tmpFAB))));}
return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp1F4,_tmp1F1)){
{struct Cyc_Core_Opt*_tmpFAC;*_tmp1F0=((_tmpFAC=_cycalloc(sizeof(*_tmpFAC)),((_tmpFAC->v=kb2,_tmpFAC))));}
return 1;}else{
return 0;}}_LLDA:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp1F5=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1E0.f1;if(_tmp1F5->tag != 1)goto _LLDC;else{_tmp1F6=(struct Cyc_Core_Opt**)& _tmp1F5->f1;}}_tmp1F7=_tmp1E0.f2;_LLDB:
 _tmp1F8=_tmp1F7;_tmp1FA=_tmp1F6;goto _LLDD;_LLDC: _tmp1F8=_tmp1E0.f1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp1F9=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1E0.f2;if(_tmp1F9->tag != 1)goto _LLD1;else{_tmp1FA=(struct Cyc_Core_Opt**)& _tmp1F9->f1;}};_LLDD:
# 544
{struct Cyc_Core_Opt*_tmpFAD;*_tmp1FA=((_tmpFAD=_cycalloc(sizeof(*_tmpFAD)),((_tmpFAD->v=_tmp1F8,_tmpFAD))));}
return 1;_LLD1:;}
# 549
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp201=Cyc_Tcutil_compress(t);void*_tmp202=_tmp201;struct Cyc_Absyn_Tvar*_tmp204;_LLDF: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp203=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp202;if(_tmp203->tag != 2)goto _LLE1;else{_tmp204=_tmp203->f1;}}_LLE0: {
# 552
void*_tmp205=_tmp204->kind;
_tmp204->kind=kb;{
struct _tuple16 _tmpFAE;return(_tmpFAE.f1=_tmp204,((_tmpFAE.f2=_tmp205,_tmpFAE)));};}_LLE1:;_LLE2: {
# 556
const char*_tmpFB2;void*_tmpFB1[1];struct Cyc_String_pa_PrintArg_struct _tmpFB0;(_tmpFB0.tag=0,((_tmpFB0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpFB1[0]=& _tmpFB0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFB2="swap_kind: cannot update the kind of %s",_tag_dyneither(_tmpFB2,sizeof(char),40))),_tag_dyneither(_tmpFB1,sizeof(void*),1)))))));}_LLDE:;}
# 562
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_typ,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 564
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_typ_kind(field_typ);
if(ts != 0  && (k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak)){
# 568
struct _RegionHandle _tmp20A=_new_region("temp");struct _RegionHandle*temp=& _tmp20A;_push_region(temp);
{struct Cyc_List_List*_tmp20B=0;
# 571
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp20C=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp20D=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;
struct Cyc_Absyn_Kind*_tmp20E=Cyc_Tcutil_tvar_kind(_tmp20C,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp20F=_tmp20E;_LLE4: if(_tmp20F->kind != Cyc_Absyn_IntKind)goto _LLE6;_LLE5:
 goto _LLE7;_LLE6: if(_tmp20F->kind != Cyc_Absyn_AnyKind)goto _LLE8;_LLE7:
# 577
{struct _tuple16*_tmpFB5;struct Cyc_List_List*_tmpFB4;_tmp20B=((_tmpFB4=_region_malloc(temp,sizeof(*_tmpFB4)),((_tmpFB4->hd=((_tmpFB5=_region_malloc(temp,sizeof(*_tmpFB5)),((_tmpFB5->f1=_tmp20C,((_tmpFB5->f2=_tmp20D,_tmpFB5)))))),((_tmpFB4->tl=_tmp20B,_tmpFB4))))));}goto _LLE3;_LLE8:;_LLE9:
 goto _LLE3;_LLE3:;}
# 581
if(_tmp20B != 0){
field_typ=Cyc_Tcutil_rsubstitute(temp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp20B),field_typ);
k=Cyc_Tcutil_typ_kind(field_typ);}}
# 569
;_pop_region(temp);}
# 586
return k;}
# 593
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t){
void*_tmp212=Cyc_Tcutil_compress(t);void*_tmp213=_tmp212;struct Cyc_Core_Opt*_tmp215;struct Cyc_Absyn_Tvar*_tmp217;enum Cyc_Absyn_Size_of _tmp21A;struct Cyc_Absyn_Datatypedecl*_tmp226;struct Cyc_Absyn_Datatypefield*_tmp227;enum Cyc_Absyn_AggrKind _tmp22B;struct Cyc_List_List*_tmp22C;struct Cyc_Absyn_AggrdeclImpl*_tmp22D;struct Cyc_List_List*_tmp22E;struct Cyc_Absyn_PtrInfo _tmp231;struct Cyc_Absyn_Exp*_tmp235;struct Cyc_Absyn_Typedefdecl*_tmp238;_LLEB: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp214=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp213;if(_tmp214->tag != 1)goto _LLED;else{_tmp215=_tmp214->f1;}}_LLEC:
 return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp215))->v;_LLED: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp216=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp213;if(_tmp216->tag != 2)goto _LLEF;else{_tmp217=_tmp216->f1;}}_LLEE:
 return Cyc_Tcutil_tvar_kind(_tmp217,& Cyc_Tcutil_bk);_LLEF: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp218=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp213;if(_tmp218->tag != 0)goto _LLF1;}_LLF0:
 return& Cyc_Tcutil_mk;_LLF1: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp219=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp213;if(_tmp219->tag != 6)goto _LLF3;else{_tmp21A=_tmp219->f2;}}_LLF2:
# 599
 return(_tmp21A == Cyc_Absyn_Int_sz  || _tmp21A == Cyc_Absyn_Long_sz)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;_LLF3: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp21B=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp213;if(_tmp21B->tag != 7)goto _LLF5;}_LLF4:
# 601
 return& Cyc_Tcutil_mk;_LLF5: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp21C=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp213;if(_tmp21C->tag != 9)goto _LLF7;}_LLF6:
 return& Cyc_Tcutil_ak;_LLF7: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp21D=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp213;if(_tmp21D->tag != 16)goto _LLF9;}_LLF8:
 goto _LLFA;_LLF9: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp21E=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp213;if(_tmp21E->tag != 13)goto _LLFB;}_LLFA:
 goto _LLFC;_LLFB: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp21F=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp213;if(_tmp21F->tag != 14)goto _LLFD;}_LLFC:
 goto _LLFE;_LLFD: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp220=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp213;if(_tmp220->tag != 15)goto _LLFF;}_LLFE:
 return& Cyc_Tcutil_bk;_LLFF: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp221=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp213;if(_tmp221->tag != 21)goto _LL101;}_LL100:
 return& Cyc_Tcutil_urk;_LL101: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp222=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp213;if(_tmp222->tag != 20)goto _LL103;}_LL102:
 return& Cyc_Tcutil_rk;_LL103: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp223=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp213;if(_tmp223->tag != 22)goto _LL105;}_LL104:
 return& Cyc_Tcutil_trk;_LL105: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp224=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp213;if(_tmp224->tag != 3)goto _LL107;}_LL106:
# 612
 return& Cyc_Tcutil_ak;_LL107: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp225=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp213;if(_tmp225->tag != 4)goto _LL109;else{if((((_tmp225->f1).field_info).KnownDatatypefield).tag != 2)goto _LL109;_tmp226=((struct _tuple3)(((_tmp225->f1).field_info).KnownDatatypefield).val).f1;_tmp227=((struct _tuple3)(((_tmp225->f1).field_info).KnownDatatypefield).val).f2;}}_LL108:
# 614
 return& Cyc_Tcutil_mk;_LL109: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp228=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp213;if(_tmp228->tag != 4)goto _LL10B;else{if((((_tmp228->f1).field_info).UnknownDatatypefield).tag != 1)goto _LL10B;}}_LL10A: {
# 616
const char*_tmpFB8;void*_tmpFB7;(_tmpFB7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFB8="typ_kind: Unresolved DatatypeFieldType",_tag_dyneither(_tmpFB8,sizeof(char),39))),_tag_dyneither(_tmpFB7,sizeof(void*),0)));}_LL10B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp229=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp213;if(_tmp229->tag != 11)goto _LL10D;else{if((((_tmp229->f1).aggr_info).UnknownAggr).tag != 1)goto _LL10D;}}_LL10C:
# 619
 return& Cyc_Tcutil_ak;_LL10D: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp22A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp213;if(_tmp22A->tag != 11)goto _LL10F;else{if((((_tmp22A->f1).aggr_info).KnownAggr).tag != 2)goto _LL10F;_tmp22B=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp22A->f1).aggr_info).KnownAggr).val))->kind;_tmp22C=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp22A->f1).aggr_info).KnownAggr).val))->tvs;_tmp22D=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp22A->f1).aggr_info).KnownAggr).val))->impl;_tmp22E=(_tmp22A->f1).targs;}}_LL10E:
# 621
 if(_tmp22D == 0)return& Cyc_Tcutil_ak;{
struct Cyc_List_List*_tmp244=_tmp22D->fields;
if(_tmp244 == 0)return& Cyc_Tcutil_mk;
# 625
if(_tmp22B == Cyc_Absyn_StructA){
for(0;_tmp244->tl != 0;_tmp244=_tmp244->tl){;}{
void*_tmp245=((struct Cyc_Absyn_Aggrfield*)_tmp244->hd)->type;
struct Cyc_Absyn_Kind*_tmp246=Cyc_Tcutil_field_kind(_tmp245,_tmp22E,_tmp22C);
if(_tmp246 == & Cyc_Tcutil_ak  || _tmp246 == & Cyc_Tcutil_tak)return _tmp246;};}else{
# 633
for(0;_tmp244 != 0;_tmp244=_tmp244->tl){
void*_tmp247=((struct Cyc_Absyn_Aggrfield*)_tmp244->hd)->type;
struct Cyc_Absyn_Kind*_tmp248=Cyc_Tcutil_field_kind(_tmp247,_tmp22E,_tmp22C);
if(_tmp248 == & Cyc_Tcutil_ak  || _tmp248 == & Cyc_Tcutil_tak)return _tmp248;}}
# 639
return& Cyc_Tcutil_mk;};_LL10F: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp22F=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp213;if(_tmp22F->tag != 12)goto _LL111;}_LL110:
 return& Cyc_Tcutil_mk;_LL111: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp230=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp213;if(_tmp230->tag != 5)goto _LL113;else{_tmp231=_tmp230->f1;}}_LL112: {
# 642
void*_tmp249=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,(_tmp231.ptr_atts).bounds);void*_tmp24A=_tmp249;_LL12A: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp24B=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp24A;if(_tmp24B->tag != 0)goto _LL12C;}_LL12B: {
# 644
enum Cyc_Absyn_AliasQual _tmp24D=(Cyc_Tcutil_typ_kind((_tmp231.ptr_atts).rgn))->aliasqual;switch(_tmp24D){case Cyc_Absyn_Aliasable: _LL12E:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL12F:
 return& Cyc_Tcutil_umk;case Cyc_Absyn_Top: _LL130:
 return& Cyc_Tcutil_tmk;}}_LL12C: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp24C=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp24A;if(_tmp24C->tag != 1)goto _LL129;}_LL12D: {
# 650
enum Cyc_Absyn_AliasQual _tmp24E=(Cyc_Tcutil_typ_kind((_tmp231.ptr_atts).rgn))->aliasqual;switch(_tmp24E){case Cyc_Absyn_Aliasable: _LL132:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL133:
 return& Cyc_Tcutil_ubk;case Cyc_Absyn_Top: _LL134:
 return& Cyc_Tcutil_tbk;}}_LL129:;}_LL113: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp232=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp213;if(_tmp232->tag != 18)goto _LL115;}_LL114:
# 656
 return& Cyc_Tcutil_ik;_LL115: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp233=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp213;if(_tmp233->tag != 19)goto _LL117;}_LL116:
 return& Cyc_Tcutil_bk;_LL117: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp234=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp213;if(_tmp234->tag != 8)goto _LL119;else{_tmp235=(_tmp234->f1).num_elts;}}_LL118:
# 659
 if(_tmp235 == 0  || Cyc_Tcutil_is_const_exp(_tmp235))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;_LL119: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp236=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp213;if(_tmp236->tag != 10)goto _LL11B;}_LL11A:
 return& Cyc_Tcutil_mk;_LL11B: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp237=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp213;if(_tmp237->tag != 17)goto _LL11D;else{_tmp238=_tmp237->f3;}}_LL11C:
# 663
 if(_tmp238 == 0  || _tmp238->kind == 0){
const char*_tmpFBC;void*_tmpFBB[1];struct Cyc_String_pa_PrintArg_struct _tmpFBA;(_tmpFBA.tag=0,((_tmpFBA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpFBB[0]=& _tmpFBA,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFBC="typ_kind: typedef found: %s",_tag_dyneither(_tmpFBC,sizeof(char),28))),_tag_dyneither(_tmpFBB,sizeof(void*),1)))))));}
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp238->kind))->v;_LL11D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp239=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp213;if(_tmp239->tag != 26)goto _LL11F;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp23A=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp239->f1)->r;if(_tmp23A->tag != 0)goto _LL11F;}}_LL11E:
 return& Cyc_Tcutil_ak;_LL11F: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp23B=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp213;if(_tmp23B->tag != 26)goto _LL121;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp23C=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp23B->f1)->r;if(_tmp23C->tag != 1)goto _LL121;}}_LL120:
 return& Cyc_Tcutil_bk;_LL121: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp23D=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp213;if(_tmp23D->tag != 26)goto _LL123;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp23E=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp23D->f1)->r;if(_tmp23E->tag != 2)goto _LL123;}}_LL122:
 return& Cyc_Tcutil_ak;_LL123: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp23F=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp213;if(_tmp23F->tag != 23)goto _LL125;}_LL124:
 goto _LL126;_LL125: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp240=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp213;if(_tmp240->tag != 24)goto _LL127;}_LL126:
 goto _LL128;_LL127: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp241=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp213;if(_tmp241->tag != 25)goto _LLEA;}_LL128:
 return& Cyc_Tcutil_ek;_LLEA:;}
# 675
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2  || k1->kind == k2->kind  && k1->aliasqual == k2->aliasqual;}
# 680
int Cyc_Tcutil_unify(void*t1,void*t2){
struct _handler_cons _tmp252;_push_handler(& _tmp252);{int _tmp254=0;if(setjmp(_tmp252.handler))_tmp254=1;if(!_tmp254){
Cyc_Tcutil_unify_it(t1,t2);{
int _tmp255=1;_npop_handler(0);return _tmp255;};
# 682
;_pop_handler();}else{void*_tmp253=(void*)_exn_thrown;void*_tmp257=_tmp253;void*_tmp259;_LL137: {struct Cyc_Tcutil_Unify_exn_struct*_tmp258=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp257;if(_tmp258->tag != Cyc_Tcutil_Unify)goto _LL139;}_LL138:
# 684
 return 0;_LL139: _tmp259=_tmp257;_LL13A:(void)_rethrow(_tmp259);_LL136:;}};}
# 689
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
t=Cyc_Tcutil_compress(t);{
void*_tmp25A=t;struct Cyc_Absyn_Tvar*_tmp25C;void*_tmp25E;struct Cyc_Core_Opt**_tmp25F;struct Cyc_Absyn_PtrInfo _tmp261;void*_tmp263;struct Cyc_List_List*_tmp265;void*_tmp266;struct Cyc_Absyn_Tqual _tmp267;void*_tmp268;struct Cyc_List_List*_tmp269;int _tmp26A;struct Cyc_Absyn_VarargInfo*_tmp26B;struct Cyc_List_List*_tmp26C;struct Cyc_List_List*_tmp26D;struct Cyc_Absyn_Exp*_tmp26E;struct Cyc_List_List*_tmp26F;struct Cyc_Absyn_Exp*_tmp270;struct Cyc_List_List*_tmp271;struct Cyc_List_List*_tmp273;struct Cyc_List_List*_tmp275;struct Cyc_List_List*_tmp277;struct Cyc_List_List*_tmp279;struct Cyc_List_List*_tmp27B;struct Cyc_List_List*_tmp27D;void*_tmp27F;void*_tmp281;void*_tmp283;void*_tmp285;struct Cyc_List_List*_tmp287;_LL13C: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp25B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp25A;if(_tmp25B->tag != 2)goto _LL13E;else{_tmp25C=_tmp25B->f1;}}_LL13D:
# 694
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp25C)){
{const char*_tmpFBD;Cyc_Tcutil_failure_reason=((_tmpFBD="(type variable would escape scope)",_tag_dyneither(_tmpFBD,sizeof(char),35)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 698
goto _LL13B;_LL13E: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp25D=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp25A;if(_tmp25D->tag != 1)goto _LL140;else{_tmp25E=(void*)_tmp25D->f2;_tmp25F=(struct Cyc_Core_Opt**)& _tmp25D->f4;}}_LL13F:
# 700
 if(t == evar){
{const char*_tmpFBE;Cyc_Tcutil_failure_reason=((_tmpFBE="(occurs check)",_tag_dyneither(_tmpFBE,sizeof(char),15)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{
# 704
if(_tmp25E != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp25E);else{
# 707
int problem=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp25F))->v;for(0;s != 0;s=s->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}
# 714
if(problem){
struct Cyc_List_List*_tmp28A=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp25F))->v;for(0;s != 0;s=s->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
struct Cyc_List_List*_tmpFBF;_tmp28A=((_tmpFBF=_cycalloc(sizeof(*_tmpFBF)),((_tmpFBF->hd=(struct Cyc_Absyn_Tvar*)s->hd,((_tmpFBF->tl=_tmp28A,_tmpFBF))))));}}}{
# 720
struct Cyc_Core_Opt*_tmpFC0;*_tmp25F=((_tmpFC0=_cycalloc(sizeof(*_tmpFC0)),((_tmpFC0->v=_tmp28A,_tmpFC0))));};}}}
# 723
goto _LL13B;_LL140: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp260=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp25A;if(_tmp260->tag != 5)goto _LL142;else{_tmp261=_tmp260->f1;}}_LL141:
# 726
 Cyc_Tcutil_occurs(evar,r,env,_tmp261.elt_typ);
Cyc_Tcutil_occurs(evar,r,env,(_tmp261.ptr_atts).rgn);
goto _LL13B;_LL142: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp262=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp25A;if(_tmp262->tag != 8)goto _LL144;else{_tmp263=(_tmp262->f1).elt_type;}}_LL143:
# 730
 Cyc_Tcutil_occurs(evar,r,env,_tmp263);goto _LL13B;_LL144: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp264=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp25A;if(_tmp264->tag != 9)goto _LL146;else{_tmp265=(_tmp264->f1).tvars;_tmp266=(_tmp264->f1).effect;_tmp267=(_tmp264->f1).ret_tqual;_tmp268=(_tmp264->f1).ret_typ;_tmp269=(_tmp264->f1).args;_tmp26A=(_tmp264->f1).c_varargs;_tmp26B=(_tmp264->f1).cyc_varargs;_tmp26C=(_tmp264->f1).rgn_po;_tmp26D=(_tmp264->f1).attributes;_tmp26E=(_tmp264->f1).requires_clause;_tmp26F=(_tmp264->f1).requires_relns;_tmp270=(_tmp264->f1).ensures_clause;_tmp271=(_tmp264->f1).ensures_relns;}}_LL145:
# 733
 env=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp265,env);
if(_tmp266 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp266);
Cyc_Tcutil_occurs(evar,r,env,_tmp268);
for(0;_tmp269 != 0;_tmp269=_tmp269->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple10*)_tmp269->hd)).f3);}
if(_tmp26B != 0)
Cyc_Tcutil_occurs(evar,r,env,_tmp26B->type);
for(0;_tmp26C != 0;_tmp26C=_tmp26C->tl){
struct _tuple0*_tmp28D=(struct _tuple0*)_tmp26C->hd;void*_tmp28F;void*_tmp290;struct _tuple0*_tmp28E=_tmp28D;_tmp28F=_tmp28E->f1;_tmp290=_tmp28E->f2;
Cyc_Tcutil_occurs(evar,r,env,_tmp28F);
Cyc_Tcutil_occurs(evar,r,env,_tmp290);}
# 745
goto _LL13B;_LL146: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp272=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp25A;if(_tmp272->tag != 10)goto _LL148;else{_tmp273=_tmp272->f1;}}_LL147:
# 747
 for(0;_tmp273 != 0;_tmp273=_tmp273->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple12*)_tmp273->hd)).f2);}
goto _LL13B;_LL148: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp274=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp25A;if(_tmp274->tag != 3)goto _LL14A;else{_tmp275=(_tmp274->f1).targs;}}_LL149:
# 751
 Cyc_Tcutil_occurslist(evar,r,env,_tmp275);goto _LL13B;_LL14A: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp276=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp25A;if(_tmp276->tag != 17)goto _LL14C;else{_tmp277=_tmp276->f2;}}_LL14B:
 _tmp279=_tmp277;goto _LL14D;_LL14C: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp278=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp25A;if(_tmp278->tag != 4)goto _LL14E;else{_tmp279=(_tmp278->f1).targs;}}_LL14D:
 _tmp27B=_tmp279;goto _LL14F;_LL14E: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp27A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp25A;if(_tmp27A->tag != 11)goto _LL150;else{_tmp27B=(_tmp27A->f1).targs;}}_LL14F:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp27B);goto _LL13B;_LL150: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp27C=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp25A;if(_tmp27C->tag != 12)goto _LL152;else{_tmp27D=_tmp27C->f2;}}_LL151:
# 757
 for(0;_tmp27D != 0;_tmp27D=_tmp27D->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp27D->hd)->type);}
goto _LL13B;_LL152: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp27E=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp25A;if(_tmp27E->tag != 19)goto _LL154;else{_tmp27F=(void*)_tmp27E->f1;}}_LL153:
 _tmp281=_tmp27F;goto _LL155;_LL154: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp280=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp25A;if(_tmp280->tag != 23)goto _LL156;else{_tmp281=(void*)_tmp280->f1;}}_LL155:
 _tmp283=_tmp281;goto _LL157;_LL156: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp282=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp25A;if(_tmp282->tag != 15)goto _LL158;else{_tmp283=(void*)_tmp282->f1;}}_LL157:
 _tmp285=_tmp283;goto _LL159;_LL158: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp284=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp25A;if(_tmp284->tag != 25)goto _LL15A;else{_tmp285=(void*)_tmp284->f1;}}_LL159:
 Cyc_Tcutil_occurs(evar,r,env,_tmp285);goto _LL13B;_LL15A: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp286=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp25A;if(_tmp286->tag != 24)goto _LL15C;else{_tmp287=_tmp286->f1;}}_LL15B:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp287);goto _LL13B;_LL15C:;_LL15D:
# 766
 goto _LL13B;_LL13B:;};}
# 769
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
# 771
for(0;ts != 0;ts=ts->tl){
Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}
# 776
static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){
Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0  || t2 != 0)
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 784
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){
if(tq1.print_const  && !tq1.real_const){
const char*_tmpFC3;void*_tmpFC2;(_tmpFC2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFC3="tq1 real_const not set.",_tag_dyneither(_tmpFC3,sizeof(char),24))),_tag_dyneither(_tmpFC2,sizeof(void*),0)));}
if(tq2.print_const  && !tq2.real_const){
const char*_tmpFC6;void*_tmpFC5;(_tmpFC5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFC6="tq2 real_const not set.",_tag_dyneither(_tmpFC6,sizeof(char),24))),_tag_dyneither(_tmpFC5,sizeof(void*),0)));}
# 790
if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
# 793
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;
{const char*_tmpFC7;Cyc_Tcutil_failure_reason=((_tmpFC7="(qualifiers don't match)",_tag_dyneither(_tmpFC7,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 801
Cyc_Tcutil_tq1_const=0;
Cyc_Tcutil_tq2_const=0;}
# 805
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
return(tq1.real_const == tq2.real_const  && tq1.q_volatile == tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}
# 812
static void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason){
# 814
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return;{
union Cyc_Absyn_Constraint*_tmp296=x;void*_tmp297;_LL15F: if((_tmp296->No_constr).tag != 3)goto _LL161;_LL160:
# 819
{union Cyc_Absyn_Constraint _tmpFC8;*x=(((_tmpFC8.Forward_constr).val=y,(((_tmpFC8.Forward_constr).tag=2,_tmpFC8))));}return;_LL161: if((_tmp296->Eq_constr).tag != 1)goto _LL163;_tmp297=(void*)(_tmp296->Eq_constr).val;_LL162: {
# 821
union Cyc_Absyn_Constraint*_tmp299=y;void*_tmp29A;_LL166: if((_tmp299->No_constr).tag != 3)goto _LL168;_LL167:
*y=*x;return;_LL168: if((_tmp299->Eq_constr).tag != 1)goto _LL16A;_tmp29A=(void*)(_tmp299->Eq_constr).val;_LL169:
# 824
 if(cmp(_tmp297,_tmp29A)!= 0){
Cyc_Tcutil_failure_reason=reason;
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 828
return;_LL16A: if((_tmp299->Forward_constr).tag != 2)goto _LL165;_LL16B: {
const char*_tmpFCB;void*_tmpFCA;(_tmpFCA=0,Cyc_Tcutil_impos(((_tmpFCB="unify_conref: forward after compress(2)",_tag_dyneither(_tmpFCB,sizeof(char),40))),_tag_dyneither(_tmpFCA,sizeof(void*),0)));}_LL165:;}_LL163: if((_tmp296->Forward_constr).tag != 2)goto _LL15E;_LL164: {
# 831
const char*_tmpFCE;void*_tmpFCD;(_tmpFCD=0,Cyc_Tcutil_impos(((_tmpFCE="unify_conref: forward after compress",_tag_dyneither(_tmpFCE,sizeof(char),37))),_tag_dyneither(_tmpFCD,sizeof(void*),0)));}_LL15E:;};}
# 835
static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
struct _handler_cons _tmp29F;_push_handler(& _tmp29F);{int _tmp2A1=0;if(setjmp(_tmp29F.handler))_tmp2A1=1;if(!_tmp2A1){
Cyc_Tcutil_unify_it_conrefs(cmp,x,y,_tag_dyneither(0,0,0));{
int _tmp2A2=1;_npop_handler(0);return _tmp2A2;};
# 837
;_pop_handler();}else{void*_tmp2A0=(void*)_exn_thrown;void*_tmp2A4=_tmp2A0;void*_tmp2A6;_LL16D: {struct Cyc_Tcutil_Unify_exn_struct*_tmp2A5=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp2A4;if(_tmp2A5->tag != Cyc_Tcutil_Unify)goto _LL16F;}_LL16E:
# 839
 return 0;_LL16F: _tmp2A6=_tmp2A4;_LL170:(void)_rethrow(_tmp2A6);_LL16C:;}};}
# 842
static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmpFCF;struct _tuple0 _tmp2A7=(_tmpFCF.f1=b1,((_tmpFCF.f2=b2,_tmpFCF)));struct _tuple0 _tmp2A8=_tmp2A7;struct Cyc_Absyn_Exp*_tmp2AE;struct Cyc_Absyn_Exp*_tmp2B0;_LL172:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2A9=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2A8.f1;if(_tmp2A9->tag != 0)goto _LL174;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2AA=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2A8.f2;if(_tmp2AA->tag != 0)goto _LL174;};_LL173:
 return 0;_LL174: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2AB=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2A8.f1;if(_tmp2AB->tag != 0)goto _LL176;}_LL175:
 return - 1;_LL176: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2AC=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2A8.f2;if(_tmp2AC->tag != 0)goto _LL178;}_LL177:
 return 1;_LL178:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2AD=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2A8.f1;if(_tmp2AD->tag != 1)goto _LL171;else{_tmp2AE=_tmp2AD->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2AF=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2A8.f2;if(_tmp2AF->tag != 1)goto _LL171;else{_tmp2B0=_tmp2AF->f1;}};_LL179:
# 848
 return Cyc_Evexp_const_exp_cmp(_tmp2AE,_tmp2B0);_LL171:;}
# 852
static void Cyc_Tcutil_unify_cmp_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
{struct _tuple0 _tmpFD0;struct _tuple0 _tmp2B2=(_tmpFD0.f1=e1->r,((_tmpFD0.f2=e2->r,_tmpFD0)));struct _tuple0 _tmp2B3=_tmp2B2;struct Cyc_Absyn_Exp*_tmp2B5;struct Cyc_Absyn_Exp*_tmp2B7;void*_tmp2B9;void*_tmp2BB;_LL17B: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp2B4=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2B3.f1;if(_tmp2B4->tag != 13)goto _LL17D;else{_tmp2B5=_tmp2B4->f2;}}_LL17C:
# 858
 Cyc_Tcutil_unify_cmp_exp(_tmp2B5,e2);
return;_LL17D: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp2B6=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2B3.f2;if(_tmp2B6->tag != 13)goto _LL17F;else{_tmp2B7=_tmp2B6->f2;}}_LL17E:
# 861
 Cyc_Tcutil_unify_cmp_exp(e1,_tmp2B7);
return;_LL17F: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp2B8=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp2B3.f1;if(_tmp2B8->tag != 38)goto _LL181;else{_tmp2B9=(void*)_tmp2B8->f1;}}_LL180:
# 864
{void*_tmp2BC=Cyc_Tcutil_compress(_tmp2B9);void*_tmp2BD=_tmp2BC;_LL186: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp2BE=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2BD;if(_tmp2BE->tag != 1)goto _LL188;}_LL187:
# 866
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpFD3;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpFD2;Cyc_Tcutil_unify_it(_tmp2B9,(void*)((_tmpFD2=_cycalloc(sizeof(*_tmpFD2)),((_tmpFD2[0]=((_tmpFD3.tag=18,((_tmpFD3.f1=e2,_tmpFD3)))),_tmpFD2)))));}
return;_LL188:;_LL189:
# 869
 goto _LL185;_LL185:;}
# 871
goto _LL17A;_LL181: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp2BA=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp2B3.f2;if(_tmp2BA->tag != 38)goto _LL183;else{_tmp2BB=(void*)_tmp2BA->f1;}}_LL182:
# 873
{void*_tmp2C1=Cyc_Tcutil_compress(_tmp2BB);void*_tmp2C2=_tmp2C1;_LL18B: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp2C3=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C2;if(_tmp2C3->tag != 1)goto _LL18D;}_LL18C:
# 875
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpFD6;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpFD5;Cyc_Tcutil_unify_it(_tmp2BB,(void*)((_tmpFD5=_cycalloc(sizeof(*_tmpFD5)),((_tmpFD5[0]=((_tmpFD6.tag=18,((_tmpFD6.f1=e1,_tmpFD6)))),_tmpFD5)))));}
return;_LL18D:;_LL18E:
# 878
 goto _LL18A;_LL18A:;}
# 880
goto _LL17A;_LL183:;_LL184:
 goto _LL17A;_LL17A:;}
# 883
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 886
static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){
struct _tuple0 _tmpFD7;struct _tuple0 _tmp2C7=(_tmpFD7.f1=b1,((_tmpFD7.f2=b2,_tmpFD7)));struct _tuple0 _tmp2C8=_tmp2C7;struct Cyc_Absyn_Exp*_tmp2CE;struct Cyc_Absyn_Exp*_tmp2D0;_LL190:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2C9=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2C8.f1;if(_tmp2C9->tag != 0)goto _LL192;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2CA=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2C8.f2;if(_tmp2CA->tag != 0)goto _LL192;};_LL191:
 return 0;_LL192: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2CB=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2C8.f1;if(_tmp2CB->tag != 0)goto _LL194;}_LL193:
 return - 1;_LL194: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2CC=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2C8.f2;if(_tmp2CC->tag != 0)goto _LL196;}_LL195:
 return 1;_LL196:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2CD=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2C8.f1;if(_tmp2CD->tag != 1)goto _LL18F;else{_tmp2CE=_tmp2CD->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2CF=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2C8.f2;if(_tmp2CF->tag != 1)goto _LL18F;else{_tmp2D0=_tmp2CF->f1;}};_LL197: {
# 892
int r=Cyc_Evexp_const_exp_cmp(_tmp2CE,_tmp2D0);
if(r == 0)return 0;{
struct _handler_cons _tmp2D1;_push_handler(& _tmp2D1);{int _tmp2D3=0;if(setjmp(_tmp2D1.handler))_tmp2D3=1;if(!_tmp2D3){
Cyc_Tcutil_unify_cmp_exp(_tmp2CE,_tmp2D0);{
int _tmp2D4=0;_npop_handler(0);return _tmp2D4;};
# 895
;_pop_handler();}else{void*_tmp2D2=(void*)_exn_thrown;void*_tmp2D6=_tmp2D2;void*_tmp2D8;_LL199: {struct Cyc_Tcutil_Unify_exn_struct*_tmp2D7=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp2D6;if(_tmp2D7->tag != Cyc_Tcutil_Unify)goto _LL19B;}_LL19A:
# 898
 return r;_LL19B: _tmp2D8=_tmp2D6;_LL19C:(void)_rethrow(_tmp2D8);_LL198:;}};};}_LL18F:;}
# 903
static int Cyc_Tcutil_attribute_case_number(void*att){
void*_tmp2DA=att;_LL19E: {struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2DB=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2DA;if(_tmp2DB->tag != 0)goto _LL1A0;}_LL19F:
 return 0;_LL1A0: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp2DC=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp2DA;if(_tmp2DC->tag != 1)goto _LL1A2;}_LL1A1:
 return 1;_LL1A2: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp2DD=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp2DA;if(_tmp2DD->tag != 2)goto _LL1A4;}_LL1A3:
 return 2;_LL1A4: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp2DE=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp2DA;if(_tmp2DE->tag != 3)goto _LL1A6;}_LL1A5:
 return 3;_LL1A6: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp2DF=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp2DA;if(_tmp2DF->tag != 4)goto _LL1A8;}_LL1A7:
 return 4;_LL1A8: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp2E0=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp2DA;if(_tmp2E0->tag != 5)goto _LL1AA;}_LL1A9:
 return 5;_LL1AA: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2E1=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2DA;if(_tmp2E1->tag != 6)goto _LL1AC;}_LL1AB:
 return 6;_LL1AC: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp2E2=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp2DA;if(_tmp2E2->tag != 7)goto _LL1AE;}_LL1AD:
 return 7;_LL1AE: {struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2E3=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2DA;if(_tmp2E3->tag != 8)goto _LL1B0;}_LL1AF:
 return 8;_LL1B0: {struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*_tmp2E4=(struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*)_tmp2DA;if(_tmp2E4->tag != 9)goto _LL1B2;}_LL1B1:
 return 9;_LL1B2: {struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*_tmp2E5=(struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*)_tmp2DA;if(_tmp2E5->tag != 10)goto _LL1B4;}_LL1B3:
 return 10;_LL1B4: {struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*_tmp2E6=(struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*)_tmp2DA;if(_tmp2E6->tag != 11)goto _LL1B6;}_LL1B5:
 return 11;_LL1B6: {struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*_tmp2E7=(struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*)_tmp2DA;if(_tmp2E7->tag != 12)goto _LL1B8;}_LL1B7:
 return 12;_LL1B8: {struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*_tmp2E8=(struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*)_tmp2DA;if(_tmp2E8->tag != 13)goto _LL1BA;}_LL1B9:
 return 13;_LL1BA: {struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*_tmp2E9=(struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*)_tmp2DA;if(_tmp2E9->tag != 14)goto _LL1BC;}_LL1BB:
 return 14;_LL1BC: {struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*_tmp2EA=(struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*)_tmp2DA;if(_tmp2EA->tag != 15)goto _LL1BE;}_LL1BD:
 return 15;_LL1BE: {struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*_tmp2EB=(struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*)_tmp2DA;if(_tmp2EB->tag != 16)goto _LL1C0;}_LL1BF:
 return 16;_LL1C0: {struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*_tmp2EC=(struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*)_tmp2DA;if(_tmp2EC->tag != 17)goto _LL1C2;}_LL1C1:
 return 17;_LL1C2: {struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*_tmp2ED=(struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*)_tmp2DA;if(_tmp2ED->tag != 18)goto _LL1C4;}_LL1C3:
 return 18;_LL1C4: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp2EE=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2DA;if(_tmp2EE->tag != 19)goto _LL1C6;}_LL1C5:
 return 19;_LL1C6: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2EF=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2DA;if(_tmp2EF->tag != 20)goto _LL1C8;}_LL1C7:
 return 20;_LL1C8:;_LL1C9:
 return 21;_LL19D:;}
# 930
static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple0 _tmpFD8;struct _tuple0 _tmp2F0=(_tmpFD8.f1=att1,((_tmpFD8.f2=att2,_tmpFD8)));struct _tuple0 _tmp2F1=_tmp2F0;int _tmp2F3;int _tmp2F5;int _tmp2F7;int _tmp2F9;struct Cyc_Absyn_Exp*_tmp2FB;struct Cyc_Absyn_Exp*_tmp2FD;struct _dyneither_ptr _tmp2FF;struct _dyneither_ptr _tmp301;enum Cyc_Absyn_Format_Type _tmp303;int _tmp304;int _tmp305;enum Cyc_Absyn_Format_Type _tmp307;int _tmp308;int _tmp309;_LL1CB:{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2F2=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2F1.f1;if(_tmp2F2->tag != 0)goto _LL1CD;else{_tmp2F3=_tmp2F2->f1;}}{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2F4=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2F1.f2;if(_tmp2F4->tag != 0)goto _LL1CD;else{_tmp2F5=_tmp2F4->f1;}};_LL1CC:
 _tmp2F7=_tmp2F3;_tmp2F9=_tmp2F5;goto _LL1CE;_LL1CD:{struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2F6=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2F1.f1;if(_tmp2F6->tag != 20)goto _LL1CF;else{_tmp2F7=_tmp2F6->f1;}}{struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2F8=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2F1.f2;if(_tmp2F8->tag != 20)goto _LL1CF;else{_tmp2F9=_tmp2F8->f1;}};_LL1CE:
# 934
 return Cyc_Core_intcmp(_tmp2F7,_tmp2F9);_LL1CF:{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2FA=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2F1.f1;if(_tmp2FA->tag != 6)goto _LL1D1;else{_tmp2FB=_tmp2FA->f1;}}{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2FC=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2F1.f2;if(_tmp2FC->tag != 6)goto _LL1D1;else{_tmp2FD=_tmp2FC->f1;}};_LL1D0:
# 936
 if(_tmp2FB == _tmp2FD)return 0;
if(_tmp2FB == 0)return - 1;
if(_tmp2FD == 0)return 1;
return Cyc_Evexp_const_exp_cmp(_tmp2FB,_tmp2FD);_LL1D1:{struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2FE=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2F1.f1;if(_tmp2FE->tag != 8)goto _LL1D3;else{_tmp2FF=_tmp2FE->f1;}}{struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp300=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2F1.f2;if(_tmp300->tag != 8)goto _LL1D3;else{_tmp301=_tmp300->f1;}};_LL1D2:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp2FF,(struct _dyneither_ptr)_tmp301);_LL1D3:{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp302=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2F1.f1;if(_tmp302->tag != 19)goto _LL1D5;else{_tmp303=_tmp302->f1;_tmp304=_tmp302->f2;_tmp305=_tmp302->f3;}}{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp306=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2F1.f2;if(_tmp306->tag != 19)goto _LL1D5;else{_tmp307=_tmp306->f1;_tmp308=_tmp306->f2;_tmp309=_tmp306->f3;}};_LL1D4: {
# 942
int _tmp30A=Cyc_Core_intcmp((int)((unsigned int)_tmp303),(int)((unsigned int)_tmp307));
if(_tmp30A != 0)return _tmp30A;{
int _tmp30B=Cyc_Core_intcmp(_tmp304,_tmp308);
if(_tmp30B != 0)return _tmp30B;
return Cyc_Core_intcmp(_tmp305,_tmp309);};}_LL1D5:;_LL1D6:
# 948
 return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(att2));_LL1CA:;}
# 952
static int Cyc_Tcutil_equal_att(void*a1,void*a2){
return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}
# 956
int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}
return 1;}
# 965
static void*Cyc_Tcutil_rgns_of(void*t);
# 967
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 971
static struct _tuple16*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_tmp30D=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp30E=_tmp30D;_LL1D8: if(_tmp30E->kind != Cyc_Absyn_RgnKind)goto _LL1DA;if(_tmp30E->aliasqual != Cyc_Absyn_Unique)goto _LL1DA;_LL1D9:
 t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL1D7;_LL1DA: if(_tmp30E->kind != Cyc_Absyn_RgnKind)goto _LL1DC;if(_tmp30E->aliasqual != Cyc_Absyn_Aliasable)goto _LL1DC;_LL1DB:
 t=(void*)& Cyc_Absyn_HeapRgn_val;goto _LL1D7;_LL1DC: if(_tmp30E->kind != Cyc_Absyn_EffKind)goto _LL1DE;_LL1DD:
 t=Cyc_Absyn_empty_effect;goto _LL1D7;_LL1DE: if(_tmp30E->kind != Cyc_Absyn_IntKind)goto _LL1E0;_LL1DF:
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpFDB;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpFDA;t=(void*)((_tmpFDA=_cycalloc(sizeof(*_tmpFDA)),((_tmpFDA[0]=((_tmpFDB.tag=18,((_tmpFDB.f1=Cyc_Absyn_uint_exp(0,0),_tmpFDB)))),_tmpFDA))));}goto _LL1D7;_LL1E0:;_LL1E1:
 t=Cyc_Absyn_sint_typ;goto _LL1D7;_LL1D7:;}{
# 980
struct _tuple16*_tmpFDC;return(_tmpFDC=_cycalloc(sizeof(*_tmpFDC)),((_tmpFDC->f1=tv,((_tmpFDC->f2=t,_tmpFDC)))));};}
# 987
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp312=Cyc_Tcutil_compress(t);void*_tmp313=_tmp312;void*_tmp31D;void*_tmp31F;void*_tmp320;struct Cyc_List_List*_tmp322;void*_tmp324;void*_tmp325;void*_tmp327;struct Cyc_List_List*_tmp329;struct Cyc_List_List*_tmp32B;struct Cyc_List_List*_tmp32D;struct Cyc_List_List*_tmp32F;struct Cyc_List_List*_tmp332;void*_tmp333;struct Cyc_Absyn_Tqual _tmp334;void*_tmp335;struct Cyc_List_List*_tmp336;struct Cyc_Absyn_VarargInfo*_tmp337;struct Cyc_List_List*_tmp338;void*_tmp33F;struct Cyc_List_List*_tmp341;_LL1E3: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp314=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp313;if(_tmp314->tag != 0)goto _LL1E5;}_LL1E4:
 goto _LL1E6;_LL1E5: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp315=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp313;if(_tmp315->tag != 7)goto _LL1E7;}_LL1E6:
 goto _LL1E8;_LL1E7: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp316=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp313;if(_tmp316->tag != 13)goto _LL1E9;}_LL1E8:
 goto _LL1EA;_LL1E9: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp317=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp313;if(_tmp317->tag != 14)goto _LL1EB;}_LL1EA:
 goto _LL1EC;_LL1EB: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp318=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp313;if(_tmp318->tag != 18)goto _LL1ED;}_LL1EC:
 goto _LL1EE;_LL1ED: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp319=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp313;if(_tmp319->tag != 6)goto _LL1EF;}_LL1EE:
 return Cyc_Absyn_empty_effect;_LL1EF: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp31A=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp313;if(_tmp31A->tag != 1)goto _LL1F1;}_LL1F0:
 goto _LL1F2;_LL1F1: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp31B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp313;if(_tmp31B->tag != 2)goto _LL1F3;}_LL1F2: {
# 997
struct Cyc_Absyn_Kind*_tmp343=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp344=_tmp343;_LL21A: if(_tmp344->kind != Cyc_Absyn_RgnKind)goto _LL21C;_LL21B: {
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpFDF;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpFDE;return(void*)((_tmpFDE=_cycalloc(sizeof(*_tmpFDE)),((_tmpFDE[0]=((_tmpFDF.tag=23,((_tmpFDF.f1=t,_tmpFDF)))),_tmpFDE))));}_LL21C: if(_tmp344->kind != Cyc_Absyn_EffKind)goto _LL21E;_LL21D:
 return t;_LL21E: if(_tmp344->kind != Cyc_Absyn_IntKind)goto _LL220;_LL21F:
 return Cyc_Absyn_empty_effect;_LL220:;_LL221: {
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpFE2;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpFE1;return(void*)((_tmpFE1=_cycalloc(sizeof(*_tmpFE1)),((_tmpFE1[0]=((_tmpFE2.tag=25,((_tmpFE2.f1=t,_tmpFE2)))),_tmpFE1))));}_LL219:;}_LL1F3: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp31C=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp313;if(_tmp31C->tag != 15)goto _LL1F5;else{_tmp31D=(void*)_tmp31C->f1;}}_LL1F4: {
# 1003
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpFE5;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpFE4;return(void*)((_tmpFE4=_cycalloc(sizeof(*_tmpFE4)),((_tmpFE4[0]=((_tmpFE5.tag=23,((_tmpFE5.f1=_tmp31D,_tmpFE5)))),_tmpFE4))));}_LL1F5: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp31E=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp313;if(_tmp31E->tag != 16)goto _LL1F7;else{_tmp31F=(void*)_tmp31E->f1;_tmp320=(void*)_tmp31E->f2;}}_LL1F6: {
# 1007
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpFE8;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpFE7;return(void*)((_tmpFE7=_cycalloc(sizeof(*_tmpFE7)),((_tmpFE7[0]=((_tmpFE8.tag=23,((_tmpFE8.f1=_tmp320,_tmpFE8)))),_tmpFE7))));}_LL1F7: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp321=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp313;if(_tmp321->tag != 3)goto _LL1F9;else{_tmp322=(_tmp321->f1).targs;}}_LL1F8: {
# 1009
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp322);
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpFEB;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpFEA;return Cyc_Tcutil_normalize_effect((void*)((_tmpFEA=_cycalloc(sizeof(*_tmpFEA)),((_tmpFEA[0]=((_tmpFEB.tag=24,((_tmpFEB.f1=ts,_tmpFEB)))),_tmpFEA)))));}_LL1F9: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp323=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp313;if(_tmp323->tag != 5)goto _LL1FB;else{_tmp324=(_tmp323->f1).elt_typ;_tmp325=((_tmp323->f1).ptr_atts).rgn;}}_LL1FA: {
# 1013
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpFFA;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpFF9;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpFF8;void*_tmpFF7[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpFF6;return Cyc_Tcutil_normalize_effect((void*)((_tmpFF6=_cycalloc(sizeof(*_tmpFF6)),((_tmpFF6[0]=((_tmpFFA.tag=24,((_tmpFFA.f1=((_tmpFF7[1]=Cyc_Tcutil_rgns_of(_tmp324),((_tmpFF7[0]=(void*)((_tmpFF9=_cycalloc(sizeof(*_tmpFF9)),((_tmpFF9[0]=((_tmpFF8.tag=23,((_tmpFF8.f1=_tmp325,_tmpFF8)))),_tmpFF9)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpFF7,sizeof(void*),2)))))),_tmpFFA)))),_tmpFF6)))));}_LL1FB: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp326=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp313;if(_tmp326->tag != 8)goto _LL1FD;else{_tmp327=(_tmp326->f1).elt_type;}}_LL1FC:
# 1015
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp327));_LL1FD: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp328=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp313;if(_tmp328->tag != 10)goto _LL1FF;else{_tmp329=_tmp328->f1;}}_LL1FE: {
# 1017
struct Cyc_List_List*_tmp354=0;
for(0;_tmp329 != 0;_tmp329=_tmp329->tl){
struct Cyc_List_List*_tmpFFB;_tmp354=((_tmpFFB=_cycalloc(sizeof(*_tmpFFB)),((_tmpFFB->hd=(*((struct _tuple12*)_tmp329->hd)).f2,((_tmpFFB->tl=_tmp354,_tmpFFB))))));}
_tmp32B=_tmp354;goto _LL200;}_LL1FF: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp32A=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp313;if(_tmp32A->tag != 4)goto _LL201;else{_tmp32B=(_tmp32A->f1).targs;}}_LL200:
 _tmp32D=_tmp32B;goto _LL202;_LL201: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp32C=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp313;if(_tmp32C->tag != 11)goto _LL203;else{_tmp32D=(_tmp32C->f1).targs;}}_LL202: {
# 1023
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpFFE;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpFFD;return Cyc_Tcutil_normalize_effect((void*)((_tmpFFD=_cycalloc(sizeof(*_tmpFFD)),((_tmpFFD[0]=((_tmpFFE.tag=24,((_tmpFFE.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp32D),_tmpFFE)))),_tmpFFD)))));}_LL203: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp32E=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp313;if(_tmp32E->tag != 12)goto _LL205;else{_tmp32F=_tmp32E->f2;}}_LL204: {
# 1025
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1001;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1000;return Cyc_Tcutil_normalize_effect((void*)((_tmp1000=_cycalloc(sizeof(*_tmp1000)),((_tmp1000[0]=((_tmp1001.tag=24,((_tmp1001.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp32F),_tmp1001)))),_tmp1000)))));}_LL205: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp330=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp313;if(_tmp330->tag != 19)goto _LL207;}_LL206:
 return Cyc_Absyn_empty_effect;_LL207: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp331=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313;if(_tmp331->tag != 9)goto _LL209;else{_tmp332=(_tmp331->f1).tvars;_tmp333=(_tmp331->f1).effect;_tmp334=(_tmp331->f1).ret_tqual;_tmp335=(_tmp331->f1).ret_typ;_tmp336=(_tmp331->f1).args;_tmp337=(_tmp331->f1).cyc_varargs;_tmp338=(_tmp331->f1).rgn_po;}}_LL208: {
# 1035
void*_tmp35A=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp332),(void*)_check_null(_tmp333));
return Cyc_Tcutil_normalize_effect(_tmp35A);}_LL209: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp339=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp313;if(_tmp339->tag != 21)goto _LL20B;}_LL20A:
 goto _LL20C;_LL20B: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp33A=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp313;if(_tmp33A->tag != 22)goto _LL20D;}_LL20C:
 goto _LL20E;_LL20D: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp33B=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp313;if(_tmp33B->tag != 20)goto _LL20F;}_LL20E:
 return Cyc_Absyn_empty_effect;_LL20F: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp33C=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp313;if(_tmp33C->tag != 23)goto _LL211;}_LL210:
 goto _LL212;_LL211: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp33D=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp313;if(_tmp33D->tag != 24)goto _LL213;}_LL212:
 return t;_LL213: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp33E=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp313;if(_tmp33E->tag != 25)goto _LL215;else{_tmp33F=(void*)_tmp33E->f1;}}_LL214:
 return Cyc_Tcutil_rgns_of(_tmp33F);_LL215: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp340=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp313;if(_tmp340->tag != 17)goto _LL217;else{_tmp341=_tmp340->f2;}}_LL216: {
# 1044
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1004;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1003;return Cyc_Tcutil_normalize_effect((void*)((_tmp1003=_cycalloc(sizeof(*_tmp1003)),((_tmp1003[0]=((_tmp1004.tag=24,((_tmp1004.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp341),_tmp1004)))),_tmp1003)))));}_LL217: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp342=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp313;if(_tmp342->tag != 26)goto _LL1E2;}_LL218: {
const char*_tmp1007;void*_tmp1006;(_tmp1006=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1007="typedecl in rgns_of",_tag_dyneither(_tmp1007,sizeof(char),20))),_tag_dyneither(_tmp1006,sizeof(void*),0)));}_LL1E2:;}
# 1052
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp35F=e;struct Cyc_List_List**_tmp361;void*_tmp363;_LL223: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp360=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp35F;if(_tmp360->tag != 24)goto _LL225;else{_tmp361=(struct Cyc_List_List**)& _tmp360->f1;}}_LL224: {
# 1056
int redo_join=0;
{struct Cyc_List_List*effs=*_tmp361;for(0;effs != 0;effs=effs->tl){
void*_tmp364=(void*)effs->hd;
effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp364));{
void*_tmp365=(void*)effs->hd;void*_tmp366=_tmp365;_LL22A: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp367=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp366;if(_tmp367->tag != 24)goto _LL22C;}_LL22B:
 goto _LL22D;_LL22C: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp368=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp366;if(_tmp368->tag != 23)goto _LL22E;else{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp369=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)((void*)_tmp368->f1);if(_tmp369->tag != 20)goto _LL22E;}}_LL22D:
 redo_join=1;goto _LL229;_LL22E: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp36A=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp366;if(_tmp36A->tag != 23)goto _LL230;else{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp36B=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)((void*)_tmp36A->f1);if(_tmp36B->tag != 22)goto _LL230;}}_LL22F:
 redo_join=1;goto _LL229;_LL230: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp36C=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp366;if(_tmp36C->tag != 23)goto _LL232;else{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp36D=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)((void*)_tmp36C->f1);if(_tmp36D->tag != 21)goto _LL232;}}_LL231:
 redo_join=1;goto _LL229;_LL232:;_LL233:
 goto _LL229;_LL229:;};}}
# 1068
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*_tmp361;for(0;effs != 0;effs=effs->tl){
void*_tmp36E=Cyc_Tcutil_compress((void*)effs->hd);void*_tmp36F=_tmp36E;struct Cyc_List_List*_tmp371;void*_tmp378;_LL235: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp370=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp36F;if(_tmp370->tag != 24)goto _LL237;else{_tmp371=_tmp370->f1;}}_LL236:
# 1073
 effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp371,effects);
goto _LL234;_LL237: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp372=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp36F;if(_tmp372->tag != 23)goto _LL239;else{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp373=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)((void*)_tmp372->f1);if(_tmp373->tag != 20)goto _LL239;}}_LL238:
 goto _LL234;_LL239: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp374=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp36F;if(_tmp374->tag != 23)goto _LL23B;else{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp375=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)((void*)_tmp374->f1);if(_tmp375->tag != 22)goto _LL23B;}}_LL23A:
 goto _LL234;_LL23B: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp376=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp36F;if(_tmp376->tag != 23)goto _LL23D;else{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp377=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)((void*)_tmp376->f1);if(_tmp377->tag != 21)goto _LL23D;}}_LL23C:
 goto _LL234;_LL23D: _tmp378=_tmp36F;_LL23E:
{struct Cyc_List_List*_tmp1008;effects=((_tmp1008=_cycalloc(sizeof(*_tmp1008)),((_tmp1008->hd=_tmp378,((_tmp1008->tl=effects,_tmp1008))))));}goto _LL234;_LL234:;}}
# 1081
*_tmp361=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
return e;};}_LL225: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp362=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp35F;if(_tmp362->tag != 25)goto _LL227;else{_tmp363=(void*)_tmp362->f1;}}_LL226: {
# 1084
void*_tmp37A=Cyc_Tcutil_compress(_tmp363);void*_tmp37B=_tmp37A;_LL240: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp37C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp37B;if(_tmp37C->tag != 1)goto _LL242;}_LL241:
 goto _LL243;_LL242: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp37D=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp37B;if(_tmp37D->tag != 2)goto _LL244;}_LL243:
 return e;_LL244:;_LL245:
 return Cyc_Tcutil_rgns_of(_tmp363);_LL23F:;}_LL227:;_LL228:
# 1089
 return e;_LL222:;};}
# 1094
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp100E;struct Cyc_Absyn_FnInfo _tmp100D;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp100C;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp37E=(_tmp100C=_cycalloc(sizeof(*_tmp100C)),((_tmp100C[0]=((_tmp100E.tag=9,((_tmp100E.f1=((_tmp100D.tvars=0,((_tmp100D.effect=eff,((_tmp100D.ret_tqual=
Cyc_Absyn_empty_tqual(0),((_tmp100D.ret_typ=(void*)& Cyc_Absyn_VoidType_val,((_tmp100D.args=0,((_tmp100D.c_varargs=0,((_tmp100D.cyc_varargs=0,((_tmp100D.rgn_po=0,((_tmp100D.attributes=0,((_tmp100D.requires_clause=0,((_tmp100D.requires_relns=0,((_tmp100D.ensures_clause=0,((_tmp100D.ensures_relns=0,_tmp100D)))))))))))))))))))))))))),_tmp100E)))),_tmp100C)));
# 1104
return Cyc_Absyn_atb_typ((void*)_tmp37E,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}
# 1111
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);
if((r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (void*)& Cyc_Absyn_UniqueRgn_val) || r == (void*)& Cyc_Absyn_RefCntRgn_val)
return 1;{
void*_tmp382=Cyc_Tcutil_compress(e);void*_tmp383=_tmp382;void*_tmp385;struct Cyc_List_List*_tmp387;void*_tmp389;struct Cyc_Core_Opt*_tmp38B;void**_tmp38C;struct Cyc_Core_Opt*_tmp38D;_LL247: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp384=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp383;if(_tmp384->tag != 23)goto _LL249;else{_tmp385=(void*)_tmp384->f1;}}_LL248:
# 1118
 if(constrain)return Cyc_Tcutil_unify(r,_tmp385);
_tmp385=Cyc_Tcutil_compress(_tmp385);
if(r == _tmp385)return 1;{
struct _tuple0 _tmp100F;struct _tuple0 _tmp38E=(_tmp100F.f1=r,((_tmp100F.f2=_tmp385,_tmp100F)));struct _tuple0 _tmp38F=_tmp38E;struct Cyc_Absyn_Tvar*_tmp391;struct Cyc_Absyn_Tvar*_tmp393;_LL252:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp390=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp38F.f1;if(_tmp390->tag != 2)goto _LL254;else{_tmp391=_tmp390->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp392=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp38F.f2;if(_tmp392->tag != 2)goto _LL254;else{_tmp393=_tmp392->f1;}};_LL253:
 return Cyc_Absyn_tvar_cmp(_tmp391,_tmp393)== 0;_LL254:;_LL255:
 return 0;_LL251:;};_LL249: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp386=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp383;if(_tmp386->tag != 24)goto _LL24B;else{_tmp387=_tmp386->f1;}}_LL24A:
# 1126
 for(0;_tmp387 != 0;_tmp387=_tmp387->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp387->hd))return 1;}
return 0;_LL24B: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp388=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp383;if(_tmp388->tag != 25)goto _LL24D;else{_tmp389=(void*)_tmp388->f1;}}_LL24C: {
# 1130
void*_tmp395=Cyc_Tcutil_rgns_of(_tmp389);void*_tmp396=_tmp395;void*_tmp398;void*_tmp399;_LL257: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp397=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp396;if(_tmp397->tag != 25)goto _LL259;else{_tmp398=(void*)_tmp397->f1;}}_LL258:
# 1132
 if(!constrain)return 0;{
void*_tmp39A=Cyc_Tcutil_compress(_tmp398);void*_tmp39B=_tmp39A;struct Cyc_Core_Opt*_tmp39D;void**_tmp39E;struct Cyc_Core_Opt*_tmp39F;_LL25C: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp39C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp39B;if(_tmp39C->tag != 1)goto _LL25E;else{_tmp39D=_tmp39C->f1;_tmp39E=(void**)((void**)& _tmp39C->f2);_tmp39F=_tmp39C->f4;}}_LL25D: {
# 1137
void*_tmp3A0=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp39F);
# 1140
Cyc_Tcutil_occurs(_tmp3A0,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp39F))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp101E;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp101D;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp101C;void*_tmp101B[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp101A;void*_tmp3A1=Cyc_Tcutil_dummy_fntype((void*)((_tmp101A=_cycalloc(sizeof(*_tmp101A)),((_tmp101A[0]=((_tmp101E.tag=24,((_tmp101E.f1=((_tmp101B[1]=(void*)((_tmp101D=_cycalloc(sizeof(*_tmp101D)),((_tmp101D[0]=((_tmp101C.tag=23,((_tmp101C.f1=r,_tmp101C)))),_tmp101D)))),((_tmp101B[0]=_tmp3A0,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp101B,sizeof(void*),2)))))),_tmp101E)))),_tmp101A)))));
*_tmp39E=_tmp3A1;
return 1;};}_LL25E:;_LL25F:
 return 0;_LL25B:;};_LL259: _tmp399=_tmp396;_LL25A:
# 1146
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp399);_LL256:;}_LL24D: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp38A=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp383;if(_tmp38A->tag != 1)goto _LL24F;else{_tmp38B=_tmp38A->f1;_tmp38C=(void**)((void**)& _tmp38A->f2);_tmp38D=_tmp38A->f4;}}_LL24E:
# 1149
 if(_tmp38B == 0  || ((struct Cyc_Absyn_Kind*)_tmp38B->v)->kind != Cyc_Absyn_EffKind){
const char*_tmp1021;void*_tmp1020;(_tmp1020=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1021="effect evar has wrong kind",_tag_dyneither(_tmp1021,sizeof(char),27))),_tag_dyneither(_tmp1020,sizeof(void*),0)));}
if(!constrain)return 0;{
# 1154
void*_tmp3A9=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp38D);
# 1157
Cyc_Tcutil_occurs(_tmp3A9,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp38D))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1036;struct Cyc_List_List*_tmp1035;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1034;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1033;struct Cyc_List_List*_tmp1032;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1031;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3AA=(_tmp1031=_cycalloc(sizeof(*_tmp1031)),((_tmp1031[0]=((_tmp1036.tag=24,((_tmp1036.f1=((_tmp1032=_cycalloc(sizeof(*_tmp1032)),((_tmp1032->hd=_tmp3A9,((_tmp1032->tl=((_tmp1035=_cycalloc(sizeof(*_tmp1035)),((_tmp1035->hd=(void*)((_tmp1033=_cycalloc(sizeof(*_tmp1033)),((_tmp1033[0]=((_tmp1034.tag=23,((_tmp1034.f1=r,_tmp1034)))),_tmp1033)))),((_tmp1035->tl=0,_tmp1035)))))),_tmp1032)))))),_tmp1036)))),_tmp1031)));
*_tmp38C=(void*)_tmp3AA;
return 1;};};_LL24F:;_LL250:
 return 0;_LL246:;};}
# 1168
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Tcutil_compress(t);{
void*_tmp3B1=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_tmp3B2=_tmp3B1;struct Cyc_List_List*_tmp3B5;void*_tmp3B7;struct Cyc_Core_Opt*_tmp3B9;void**_tmp3BA;struct Cyc_Core_Opt*_tmp3BB;_LL261: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3B3=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3B2;if(_tmp3B3->tag != 23)goto _LL263;}_LL262:
 return 0;_LL263: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3B4=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3B2;if(_tmp3B4->tag != 24)goto _LL265;else{_tmp3B5=_tmp3B4->f1;}}_LL264:
# 1173
 for(0;_tmp3B5 != 0;_tmp3B5=_tmp3B5->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp3B5->hd))
return 1;}
return 0;_LL265: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3B6=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3B2;if(_tmp3B6->tag != 25)goto _LL267;else{_tmp3B7=(void*)_tmp3B6->f1;}}_LL266:
# 1178
 _tmp3B7=Cyc_Tcutil_compress(_tmp3B7);
if(t == _tmp3B7)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp3B7);{
void*_tmp3BC=Cyc_Tcutil_rgns_of(t);void*_tmp3BD=_tmp3BC;void*_tmp3BF;void*_tmp3C0;_LL26C: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3BE=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3BD;if(_tmp3BE->tag != 25)goto _LL26E;else{_tmp3BF=(void*)_tmp3BE->f1;}}_LL26D: {
# 1183
struct _tuple0 _tmp1037;struct _tuple0 _tmp3C1=(_tmp1037.f1=Cyc_Tcutil_compress(_tmp3BF),((_tmp1037.f2=_tmp3B7,_tmp1037)));struct _tuple0 _tmp3C2=_tmp3C1;struct Cyc_Absyn_Tvar*_tmp3C4;struct Cyc_Absyn_Tvar*_tmp3C6;_LL271:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3C3=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3C2.f1;if(_tmp3C3->tag != 2)goto _LL273;else{_tmp3C4=_tmp3C3->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3C5=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3C2.f2;if(_tmp3C5->tag != 2)goto _LL273;else{_tmp3C6=_tmp3C5->f1;}};_LL272:
 return Cyc_Tcutil_unify(t,_tmp3B7);_LL273:;_LL274:
 return _tmp3BF == _tmp3B7;_LL270:;}_LL26E: _tmp3C0=_tmp3BD;_LL26F:
# 1187
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp3C0);_LL26B:;};_LL267: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3B8=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3B2;if(_tmp3B8->tag != 1)goto _LL269;else{_tmp3B9=_tmp3B8->f1;_tmp3BA=(void**)((void**)& _tmp3B8->f2);_tmp3BB=_tmp3B8->f4;}}_LL268:
# 1190
 if(_tmp3B9 == 0  || ((struct Cyc_Absyn_Kind*)_tmp3B9->v)->kind != Cyc_Absyn_EffKind){
const char*_tmp103A;void*_tmp1039;(_tmp1039=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp103A="effect evar has wrong kind",_tag_dyneither(_tmp103A,sizeof(char),27))),_tag_dyneither(_tmp1039,sizeof(void*),0)));}
if(!may_constrain_evars)return 0;{
# 1195
void*_tmp3CA=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp3BB);
# 1198
Cyc_Tcutil_occurs(_tmp3CA,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3BB))->v,t);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp104F;struct Cyc_List_List*_tmp104E;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp104D;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp104C;struct Cyc_List_List*_tmp104B;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp104A;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3CB=(_tmp104A=_cycalloc(sizeof(*_tmp104A)),((_tmp104A[0]=((_tmp104F.tag=24,((_tmp104F.f1=((_tmp104B=_cycalloc(sizeof(*_tmp104B)),((_tmp104B->hd=_tmp3CA,((_tmp104B->tl=((_tmp104E=_cycalloc(sizeof(*_tmp104E)),((_tmp104E->hd=(void*)((_tmp104C=_cycalloc(sizeof(*_tmp104C)),((_tmp104C[0]=((_tmp104D.tag=25,((_tmp104D.f1=t,_tmp104D)))),_tmp104C)))),((_tmp104E->tl=0,_tmp104E)))))),_tmp104B)))))),_tmp104F)))),_tmp104A)));
*_tmp3BA=(void*)_tmp3CB;
return 1;};};_LL269:;_LL26A:
 return 0;_LL260:;};}
# 1209
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp3D2=e;struct Cyc_Absyn_Tvar*_tmp3D4;struct Cyc_List_List*_tmp3D6;void*_tmp3D8;struct Cyc_Core_Opt*_tmp3DA;void**_tmp3DB;struct Cyc_Core_Opt*_tmp3DC;_LL276: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3D3=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3D2;if(_tmp3D3->tag != 2)goto _LL278;else{_tmp3D4=_tmp3D3->f1;}}_LL277:
 return Cyc_Absyn_tvar_cmp(v,_tmp3D4)== 0;_LL278: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3D5=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3D2;if(_tmp3D5->tag != 24)goto _LL27A;else{_tmp3D6=_tmp3D5->f1;}}_LL279:
# 1214
 for(0;_tmp3D6 != 0;_tmp3D6=_tmp3D6->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)_tmp3D6->hd))
return 1;}
return 0;_LL27A: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3D7=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3D2;if(_tmp3D7->tag != 25)goto _LL27C;else{_tmp3D8=(void*)_tmp3D7->f1;}}_LL27B: {
# 1219
void*_tmp3DD=Cyc_Tcutil_rgns_of(_tmp3D8);void*_tmp3DE=_tmp3DD;void*_tmp3E0;void*_tmp3E1;_LL281: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3DF=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3DE;if(_tmp3DF->tag != 25)goto _LL283;else{_tmp3E0=(void*)_tmp3DF->f1;}}_LL282:
# 1221
 if(!may_constrain_evars)return 0;{
void*_tmp3E2=Cyc_Tcutil_compress(_tmp3E0);void*_tmp3E3=_tmp3E2;struct Cyc_Core_Opt*_tmp3E5;void**_tmp3E6;struct Cyc_Core_Opt*_tmp3E7;_LL286: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3E4=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3E3;if(_tmp3E4->tag != 1)goto _LL288;else{_tmp3E5=_tmp3E4->f1;_tmp3E6=(void**)((void**)& _tmp3E4->f2);_tmp3E7=_tmp3E4->f4;}}_LL287: {
# 1227
void*_tmp3E8=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp3E7);
# 1229
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3E7))->v,v))return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp105E;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp105D;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp105C;void*_tmp105B[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp105A;void*_tmp3E9=Cyc_Tcutil_dummy_fntype((void*)((_tmp105A=_cycalloc(sizeof(*_tmp105A)),((_tmp105A[0]=((_tmp105E.tag=24,((_tmp105E.f1=((_tmp105B[1]=(void*)((_tmp105D=_cycalloc(sizeof(*_tmp105D)),((_tmp105D[0]=((_tmp105C.tag=2,((_tmp105C.f1=v,_tmp105C)))),_tmp105D)))),((_tmp105B[0]=_tmp3E8,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp105B,sizeof(void*),2)))))),_tmp105E)))),_tmp105A)))));
*_tmp3E6=_tmp3E9;
return 1;};}_LL288:;_LL289:
 return 0;_LL285:;};_LL283: _tmp3E1=_tmp3DE;_LL284:
# 1235
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp3E1);_LL280:;}_LL27C: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3D9=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3D2;if(_tmp3D9->tag != 1)goto _LL27E;else{_tmp3DA=_tmp3D9->f1;_tmp3DB=(void**)((void**)& _tmp3D9->f2);_tmp3DC=_tmp3D9->f4;}}_LL27D:
# 1238
 if(_tmp3DA == 0  || ((struct Cyc_Absyn_Kind*)_tmp3DA->v)->kind != Cyc_Absyn_EffKind){
const char*_tmp1061;void*_tmp1060;(_tmp1060=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1061="effect evar has wrong kind",_tag_dyneither(_tmp1061,sizeof(char),27))),_tag_dyneither(_tmp1060,sizeof(void*),0)));}{
# 1242
void*_tmp3F1=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp3DC);
# 1244
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3DC))->v,v))
return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1076;struct Cyc_List_List*_tmp1075;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1074;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1073;struct Cyc_List_List*_tmp1072;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1071;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3F2=(_tmp1071=_cycalloc(sizeof(*_tmp1071)),((_tmp1071[0]=((_tmp1076.tag=24,((_tmp1076.f1=((_tmp1072=_cycalloc(sizeof(*_tmp1072)),((_tmp1072->hd=_tmp3F1,((_tmp1072->tl=((_tmp1075=_cycalloc(sizeof(*_tmp1075)),((_tmp1075->hd=(void*)((_tmp1073=_cycalloc(sizeof(*_tmp1073)),((_tmp1073[0]=((_tmp1074.tag=2,((_tmp1074.f1=v,_tmp1074)))),_tmp1073)))),((_tmp1075->tl=0,_tmp1075)))))),_tmp1072)))))),_tmp1076)))),_tmp1071)));
*_tmp3DB=(void*)_tmp3F2;
return 1;};};_LL27E:;_LL27F:
 return 0;_LL275:;};}
# 1254
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp3F9=e;struct Cyc_List_List*_tmp3FB;void*_tmp3FD;_LL28B: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3FA=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3F9;if(_tmp3FA->tag != 24)goto _LL28D;else{_tmp3FB=_tmp3FA->f1;}}_LL28C:
# 1258
 for(0;_tmp3FB != 0;_tmp3FB=_tmp3FB->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp3FB->hd))
return 1;}
return 0;_LL28D: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3FC=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3F9;if(_tmp3FC->tag != 25)goto _LL28F;else{_tmp3FD=(void*)_tmp3FC->f1;}}_LL28E: {
# 1263
void*_tmp3FF=Cyc_Tcutil_rgns_of(_tmp3FD);void*_tmp400=_tmp3FF;void*_tmp402;void*_tmp403;_LL294: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp401=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp400;if(_tmp401->tag != 25)goto _LL296;else{_tmp402=(void*)_tmp401->f1;}}_LL295:
 return 0;_LL296: _tmp403=_tmp400;_LL297:
 return Cyc_Tcutil_evar_in_effect(evar,_tmp403);_LL293:;}_LL28F: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3FE=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3F9;if(_tmp3FE->tag != 1)goto _LL291;}_LL290:
# 1267
 return evar == e;_LL291:;_LL292:
 return 0;_LL28A:;};}
# 1281 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1286
void*_tmp404=Cyc_Tcutil_compress(e1);void*_tmp405=_tmp404;struct Cyc_List_List*_tmp407;void*_tmp409;struct Cyc_Absyn_Tvar*_tmp40B;void*_tmp40D;void**_tmp40F;struct Cyc_Core_Opt*_tmp410;_LL299: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp406=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp405;if(_tmp406->tag != 24)goto _LL29B;else{_tmp407=_tmp406->f1;}}_LL29A:
# 1288
 for(0;_tmp407 != 0;_tmp407=_tmp407->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp407->hd,e2))
return 0;}
return 1;_LL29B: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp408=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp405;if(_tmp408->tag != 23)goto _LL29D;else{_tmp409=(void*)_tmp408->f1;}}_LL29C:
# 1299
 return Cyc_Tcutil_region_in_effect(may_constrain_evars,_tmp409,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp409,(void*)& Cyc_Absyn_HeapRgn_val);_LL29D: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp40A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp405;if(_tmp40A->tag != 2)goto _LL29F;else{_tmp40B=_tmp40A->f1;}}_LL29E:
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp40B,e2);_LL29F: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp40C=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp405;if(_tmp40C->tag != 25)goto _LL2A1;else{_tmp40D=(void*)_tmp40C->f1;}}_LL2A0: {
# 1303
void*_tmp411=Cyc_Tcutil_rgns_of(_tmp40D);void*_tmp412=_tmp411;void*_tmp414;void*_tmp415;_LL2A6: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp413=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp412;if(_tmp413->tag != 25)goto _LL2A8;else{_tmp414=(void*)_tmp413->f1;}}_LL2A7:
# 1308
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp414,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp414,Cyc_Absyn_sint_typ);_LL2A8: _tmp415=_tmp412;_LL2A9:
 return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp415,e2);_LL2A5:;}_LL2A1: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp40E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp405;if(_tmp40E->tag != 1)goto _LL2A3;else{_tmp40F=(void**)((void**)& _tmp40E->f2);_tmp410=_tmp40E->f4;}}_LL2A2:
# 1313
 if(!Cyc_Tcutil_evar_in_effect(e1,e2)){
# 1317
*_tmp40F=Cyc_Absyn_empty_effect;
# 1320
return 1;}else{
# 1322
return 0;}_LL2A3:;_LL2A4: {
const char*_tmp107A;void*_tmp1079[1];struct Cyc_String_pa_PrintArg_struct _tmp1078;(_tmp1078.tag=0,((_tmp1078.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1)),((_tmp1079[0]=& _tmp1078,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp107A="subset_effect: bad effect: %s",_tag_dyneither(_tmp107A,sizeof(char),30))),_tag_dyneither(_tmp1079,sizeof(void*),1)))))));}_LL298:;}
# 1338 "tcutil.cyc"
static int Cyc_Tcutil_unify_effect(void*e1,void*e2){
e1=Cyc_Tcutil_normalize_effect(e1);
e2=Cyc_Tcutil_normalize_effect(e2);
if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;
if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;
return 0;}
# 1354
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1356
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple0*_tmp419=(struct _tuple0*)r1->hd;void*_tmp41B;void*_tmp41C;struct _tuple0*_tmp41A=_tmp419;_tmp41B=_tmp41A->f1;_tmp41C=_tmp41A->f2;{
int found=_tmp41B == (void*)& Cyc_Absyn_HeapRgn_val;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
struct _tuple0*_tmp41D=(struct _tuple0*)r2->hd;void*_tmp41F;void*_tmp420;struct _tuple0*_tmp41E=_tmp41D;_tmp41F=_tmp41E->f1;_tmp420=_tmp41E->f2;
if(Cyc_Tcutil_unify(_tmp41B,_tmp41F) && Cyc_Tcutil_unify(_tmp41C,_tmp420)){
found=1;
break;}}}
# 1366
if(!found)return 0;};}}
# 1368
return 1;}
# 1371
typedef struct Cyc_List_List*Cyc_Tcutil_relns_t;
# 1375
static int Cyc_Tcutil_check_logical_implication(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
for(0;r2 != 0;r2=r2->tl){
struct Cyc_Relations_Reln*_tmp421=Cyc_Relations_negate(Cyc_Core_heap_region,(struct Cyc_Relations_Reln*)r2->hd);
struct Cyc_List_List*_tmp107B;struct Cyc_List_List*_tmp422=(_tmp107B=_cycalloc(sizeof(*_tmp107B)),((_tmp107B->hd=_tmp421,((_tmp107B->tl=r1,_tmp107B)))));
if(Cyc_Relations_consistent_relations(_tmp422))return 0;}
# 1381
return 1;}
# 1386
static int Cyc_Tcutil_check_logical_equivalence(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
if(r1 == r2)return 1;
return Cyc_Tcutil_check_logical_implication(r1,r2) && Cyc_Tcutil_check_logical_implication(r2,r1);}
# 1392
static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1394
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple22{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*f2;};
# 1398
void Cyc_Tcutil_unify_it(void*t1,void*t2){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=_tag_dyneither(0,0,0);
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return;
{void*_tmp424=t1;struct Cyc_Core_Opt*_tmp426;void**_tmp427;struct Cyc_Core_Opt*_tmp428;_LL2AB: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp425=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp424;if(_tmp425->tag != 1)goto _LL2AD;else{_tmp426=_tmp425->f1;_tmp427=(void**)((void**)& _tmp425->f2);_tmp428=_tmp425->f4;}}_LL2AC:
# 1409
 Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp428))->v,t2);{
struct Cyc_Absyn_Kind*_tmp429=Cyc_Tcutil_typ_kind(t2);
# 1414
if(Cyc_Tcutil_kind_leq(_tmp429,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp426))->v)){
*_tmp427=t2;
return;}else{
# 1418
{void*_tmp42A=t2;void**_tmp42C;struct Cyc_Core_Opt*_tmp42D;struct Cyc_Absyn_PtrInfo _tmp42F;_LL2B0: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp42B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp42A;if(_tmp42B->tag != 1)goto _LL2B2;else{_tmp42C=(void**)((void**)& _tmp42B->f2);_tmp42D=_tmp42B->f4;}}_LL2B1: {
# 1421
struct Cyc_List_List*_tmp430=(struct Cyc_List_List*)_tmp428->v;
{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp42D))->v;for(0;s2 != 0;s2=s2->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp430,(struct Cyc_Absyn_Tvar*)s2->hd)){
{const char*_tmp107C;Cyc_Tcutil_failure_reason=((_tmp107C="(type variable would escape scope)",_tag_dyneither(_tmp107C,sizeof(char),35)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1428
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp426->v,_tmp429)){
*_tmp42C=t1;return;}
# 1431
{const char*_tmp107D;Cyc_Tcutil_failure_reason=((_tmp107D="(kinds are incompatible)",_tag_dyneither(_tmp107D,sizeof(char),25)));}
goto _LL2AF;}_LL2B2:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp42E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42A;if(_tmp42E->tag != 5)goto _LL2B4;else{_tmp42F=_tmp42E->f1;}}if(!(((struct Cyc_Absyn_Kind*)_tmp426->v)->kind == Cyc_Absyn_BoxKind))goto _LL2B4;_LL2B3: {
# 1437
union Cyc_Absyn_Constraint*_tmp433=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((_tmp42F.ptr_atts).bounds);
{union Cyc_Absyn_Constraint*_tmp434=_tmp433;_LL2B7: if((_tmp434->No_constr).tag != 3)goto _LL2B9;_LL2B8:
# 1441
{struct _union_Constraint_Eq_constr*_tmp107E;(_tmp107E=& _tmp433->Eq_constr,((_tmp107E->tag=1,_tmp107E->val=Cyc_Absyn_bounds_one)));}
*_tmp427=t2;
return;_LL2B9:;_LL2BA:
 goto _LL2B6;_LL2B6:;}
# 1446
goto _LL2AF;}_LL2B4:;_LL2B5:
 goto _LL2AF;_LL2AF:;}
# 1449
{const char*_tmp107F;Cyc_Tcutil_failure_reason=((_tmp107F="(kinds are incompatible)",_tag_dyneither(_tmp107F,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};_LL2AD:;_LL2AE:
# 1452
 goto _LL2AA;_LL2AA:;}
# 1457
{struct _tuple0 _tmp1080;struct _tuple0 _tmp437=(_tmp1080.f1=t2,((_tmp1080.f2=t1,_tmp1080)));struct _tuple0 _tmp438=_tmp437;struct Cyc_Absyn_Tvar*_tmp43D;struct Cyc_Absyn_Tvar*_tmp43F;union Cyc_Absyn_AggrInfoU _tmp441;struct Cyc_List_List*_tmp442;union Cyc_Absyn_AggrInfoU _tmp444;struct Cyc_List_List*_tmp445;struct _tuple2*_tmp447;struct _tuple2*_tmp449;struct Cyc_List_List*_tmp44B;struct Cyc_List_List*_tmp44D;struct Cyc_Absyn_Datatypedecl*_tmp44F;struct Cyc_List_List*_tmp450;struct Cyc_Absyn_Datatypedecl*_tmp452;struct Cyc_List_List*_tmp453;struct Cyc_Absyn_Datatypedecl*_tmp455;struct Cyc_Absyn_Datatypefield*_tmp456;struct Cyc_List_List*_tmp457;struct Cyc_Absyn_Datatypedecl*_tmp459;struct Cyc_Absyn_Datatypefield*_tmp45A;struct Cyc_List_List*_tmp45B;void*_tmp45D;struct Cyc_Absyn_Tqual _tmp45E;void*_tmp45F;union Cyc_Absyn_Constraint*_tmp460;union Cyc_Absyn_Constraint*_tmp461;union Cyc_Absyn_Constraint*_tmp462;void*_tmp464;struct Cyc_Absyn_Tqual _tmp465;void*_tmp466;union Cyc_Absyn_Constraint*_tmp467;union Cyc_Absyn_Constraint*_tmp468;union Cyc_Absyn_Constraint*_tmp469;enum Cyc_Absyn_Sign _tmp46B;enum Cyc_Absyn_Size_of _tmp46C;enum Cyc_Absyn_Sign _tmp46E;enum Cyc_Absyn_Size_of _tmp46F;int _tmp471;int _tmp473;void*_tmp475;void*_tmp477;struct Cyc_Absyn_Exp*_tmp479;struct Cyc_Absyn_Exp*_tmp47B;void*_tmp47D;struct Cyc_Absyn_Tqual _tmp47E;struct Cyc_Absyn_Exp*_tmp47F;union Cyc_Absyn_Constraint*_tmp480;void*_tmp482;struct Cyc_Absyn_Tqual _tmp483;struct Cyc_Absyn_Exp*_tmp484;union Cyc_Absyn_Constraint*_tmp485;struct Cyc_List_List*_tmp487;void*_tmp488;struct Cyc_Absyn_Tqual _tmp489;void*_tmp48A;struct Cyc_List_List*_tmp48B;int _tmp48C;struct Cyc_Absyn_VarargInfo*_tmp48D;struct Cyc_List_List*_tmp48E;struct Cyc_List_List*_tmp48F;struct Cyc_Absyn_Exp*_tmp490;struct Cyc_List_List*_tmp491;struct Cyc_Absyn_Exp*_tmp492;struct Cyc_List_List*_tmp493;struct Cyc_List_List*_tmp495;void*_tmp496;struct Cyc_Absyn_Tqual _tmp497;void*_tmp498;struct Cyc_List_List*_tmp499;int _tmp49A;struct Cyc_Absyn_VarargInfo*_tmp49B;struct Cyc_List_List*_tmp49C;struct Cyc_List_List*_tmp49D;struct Cyc_Absyn_Exp*_tmp49E;struct Cyc_List_List*_tmp49F;struct Cyc_Absyn_Exp*_tmp4A0;struct Cyc_List_List*_tmp4A1;struct Cyc_List_List*_tmp4A3;struct Cyc_List_List*_tmp4A5;enum Cyc_Absyn_AggrKind _tmp4A7;struct Cyc_List_List*_tmp4A8;enum Cyc_Absyn_AggrKind _tmp4AA;struct Cyc_List_List*_tmp4AB;struct Cyc_List_List*_tmp4AD;struct Cyc_Absyn_Typedefdecl*_tmp4AE;struct Cyc_List_List*_tmp4B0;struct Cyc_Absyn_Typedefdecl*_tmp4B1;void*_tmp4B9;void*_tmp4BB;void*_tmp4BD;void*_tmp4BE;void*_tmp4C0;void*_tmp4C1;_LL2BC: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp439=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp438.f1;if(_tmp439->tag != 1)goto _LL2BE;}_LL2BD:
# 1460
 Cyc_Tcutil_unify_it(t2,t1);
return;_LL2BE:{struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp43A=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp438.f1;if(_tmp43A->tag != 0)goto _LL2C0;}{struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp43B=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp438.f2;if(_tmp43B->tag != 0)goto _LL2C0;};_LL2BF:
# 1463
 return;_LL2C0:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp43C=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp438.f1;if(_tmp43C->tag != 2)goto _LL2C2;else{_tmp43D=_tmp43C->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp43E=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp438.f2;if(_tmp43E->tag != 2)goto _LL2C2;else{_tmp43F=_tmp43E->f1;}};_LL2C1: {
# 1466
struct _dyneither_ptr*_tmp4C8=_tmp43D->name;
struct _dyneither_ptr*_tmp4C9=_tmp43F->name;
# 1469
int _tmp4CA=_tmp43D->identity;
int _tmp4CB=_tmp43F->identity;
if(_tmp4CB == _tmp4CA)return;
{const char*_tmp1081;Cyc_Tcutil_failure_reason=((_tmp1081="(variable types are not the same)",_tag_dyneither(_tmp1081,sizeof(char),34)));}
goto _LL2BB;}_LL2C2:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp440=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp438.f1;if(_tmp440->tag != 11)goto _LL2C4;else{_tmp441=(_tmp440->f1).aggr_info;_tmp442=(_tmp440->f1).targs;}}{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp443=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp438.f2;if(_tmp443->tag != 11)goto _LL2C4;else{_tmp444=(_tmp443->f1).aggr_info;_tmp445=(_tmp443->f1).targs;}};_LL2C3: {
# 1476
struct _tuple13 _tmp4CD=Cyc_Absyn_aggr_kinded_name(_tmp444);enum Cyc_Absyn_AggrKind _tmp4CF;struct _tuple2*_tmp4D0;struct _tuple13 _tmp4CE=_tmp4CD;_tmp4CF=_tmp4CE.f1;_tmp4D0=_tmp4CE.f2;{
struct _tuple13 _tmp4D1=Cyc_Absyn_aggr_kinded_name(_tmp441);enum Cyc_Absyn_AggrKind _tmp4D3;struct _tuple2*_tmp4D4;struct _tuple13 _tmp4D2=_tmp4D1;_tmp4D3=_tmp4D2.f1;_tmp4D4=_tmp4D2.f2;
if(_tmp4CF != _tmp4D3){{const char*_tmp1082;Cyc_Tcutil_failure_reason=((_tmp1082="(struct and union type)",_tag_dyneither(_tmp1082,sizeof(char),24)));}goto _LL2BB;}
if(Cyc_Absyn_qvar_cmp(_tmp4D0,_tmp4D4)!= 0){{const char*_tmp1083;Cyc_Tcutil_failure_reason=((_tmp1083="(different type name)",_tag_dyneither(_tmp1083,sizeof(char),22)));}goto _LL2BB;}
Cyc_Tcutil_unify_list(_tmp445,_tmp442);
return;};}_LL2C4:{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp446=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp438.f1;if(_tmp446->tag != 13)goto _LL2C6;else{_tmp447=_tmp446->f1;}}{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp448=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp438.f2;if(_tmp448->tag != 13)goto _LL2C6;else{_tmp449=_tmp448->f1;}};_LL2C5:
# 1485
 if(Cyc_Absyn_qvar_cmp(_tmp447,_tmp449)== 0)return;
{const char*_tmp1084;Cyc_Tcutil_failure_reason=((_tmp1084="(different enum types)",_tag_dyneither(_tmp1084,sizeof(char),23)));}
goto _LL2BB;_LL2C6:{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp44A=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp438.f1;if(_tmp44A->tag != 14)goto _LL2C8;else{_tmp44B=_tmp44A->f1;}}{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp44C=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp438.f2;if(_tmp44C->tag != 14)goto _LL2C8;else{_tmp44D=_tmp44C->f1;}};_LL2C7: {
# 1490
int bad=0;
for(0;_tmp44B != 0  && _tmp44D != 0;(_tmp44B=_tmp44B->tl,_tmp44D=_tmp44D->tl)){
struct Cyc_Absyn_Enumfield*_tmp4D8=(struct Cyc_Absyn_Enumfield*)_tmp44B->hd;
struct Cyc_Absyn_Enumfield*_tmp4D9=(struct Cyc_Absyn_Enumfield*)_tmp44D->hd;
if(Cyc_Absyn_qvar_cmp(_tmp4D8->name,_tmp4D9->name)!= 0){
{const char*_tmp1085;Cyc_Tcutil_failure_reason=((_tmp1085="(different names for enum fields)",_tag_dyneither(_tmp1085,sizeof(char),34)));}
bad=1;
break;}
# 1499
if(_tmp4D8->tag == _tmp4D9->tag)continue;
if(_tmp4D8->tag == 0  || _tmp4D9->tag == 0){
{const char*_tmp1086;Cyc_Tcutil_failure_reason=((_tmp1086="(different tag values for enum fields)",_tag_dyneither(_tmp1086,sizeof(char),39)));}
bad=1;
break;}
# 1505
if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp4D8->tag),(struct Cyc_Absyn_Exp*)_check_null(_tmp4D9->tag))){
{const char*_tmp1087;Cyc_Tcutil_failure_reason=((_tmp1087="(different tag values for enum fields)",_tag_dyneither(_tmp1087,sizeof(char),39)));}
bad=1;
break;}}
# 1511
if(bad)goto _LL2BB;
if(_tmp44B == 0  && _tmp44D == 0)return;
{const char*_tmp1088;Cyc_Tcutil_failure_reason=((_tmp1088="(different number of fields for enums)",_tag_dyneither(_tmp1088,sizeof(char),39)));}
goto _LL2BB;}_LL2C8:{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp44E=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp438.f1;if(_tmp44E->tag != 3)goto _LL2CA;else{if((((_tmp44E->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2CA;_tmp44F=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp44E->f1).datatype_info).KnownDatatype).val);_tmp450=(_tmp44E->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp451=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp438.f2;if(_tmp451->tag != 3)goto _LL2CA;else{if((((_tmp451->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2CA;_tmp452=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp451->f1).datatype_info).KnownDatatype).val);_tmp453=(_tmp451->f1).targs;}};_LL2C9:
# 1518
 if(_tmp44F == _tmp452  || Cyc_Absyn_qvar_cmp(_tmp44F->name,_tmp452->name)== 0){
Cyc_Tcutil_unify_list(_tmp453,_tmp450);
return;}
# 1522
goto _LL2BB;_LL2CA:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp454=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp438.f1;if(_tmp454->tag != 4)goto _LL2CC;else{if((((_tmp454->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2CC;_tmp455=((struct _tuple3)(((_tmp454->f1).field_info).KnownDatatypefield).val).f1;_tmp456=((struct _tuple3)(((_tmp454->f1).field_info).KnownDatatypefield).val).f2;_tmp457=(_tmp454->f1).targs;}}{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp458=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp438.f2;if(_tmp458->tag != 4)goto _LL2CC;else{if((((_tmp458->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2CC;_tmp459=((struct _tuple3)(((_tmp458->f1).field_info).KnownDatatypefield).val).f1;_tmp45A=((struct _tuple3)(((_tmp458->f1).field_info).KnownDatatypefield).val).f2;_tmp45B=(_tmp458->f1).targs;}};_LL2CB:
# 1526
 if((_tmp455 == _tmp459  || Cyc_Absyn_qvar_cmp(_tmp455->name,_tmp459->name)== 0) && (
_tmp456 == _tmp45A  || Cyc_Absyn_qvar_cmp(_tmp456->name,_tmp45A->name)== 0)){
Cyc_Tcutil_unify_list(_tmp45B,_tmp457);
return;}
# 1531
{const char*_tmp1089;Cyc_Tcutil_failure_reason=((_tmp1089="(different datatype field types)",_tag_dyneither(_tmp1089,sizeof(char),33)));}
goto _LL2BB;_LL2CC:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp45C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp438.f1;if(_tmp45C->tag != 5)goto _LL2CE;else{_tmp45D=(_tmp45C->f1).elt_typ;_tmp45E=(_tmp45C->f1).elt_tq;_tmp45F=((_tmp45C->f1).ptr_atts).rgn;_tmp460=((_tmp45C->f1).ptr_atts).nullable;_tmp461=((_tmp45C->f1).ptr_atts).bounds;_tmp462=((_tmp45C->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp463=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp438.f2;if(_tmp463->tag != 5)goto _LL2CE;else{_tmp464=(_tmp463->f1).elt_typ;_tmp465=(_tmp463->f1).elt_tq;_tmp466=((_tmp463->f1).ptr_atts).rgn;_tmp467=((_tmp463->f1).ptr_atts).nullable;_tmp468=((_tmp463->f1).ptr_atts).bounds;_tmp469=((_tmp463->f1).ptr_atts).zero_term;}};_LL2CD:
# 1536
 Cyc_Tcutil_unify_it(_tmp464,_tmp45D);
Cyc_Tcutil_unify_it(_tmp45F,_tmp466);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp108A;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp469,_tmp462,((_tmp108A="(not both zero terminated)",_tag_dyneither(_tmp108A,sizeof(char),27))));}
Cyc_Tcutil_unify_tqual(_tmp465,_tmp464,_tmp45E,_tmp45D);
{const char*_tmp108B;((void(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp468,_tmp461,((_tmp108B="(different pointer bounds)",_tag_dyneither(_tmp108B,sizeof(char),27))));}
# 1544
{void*_tmp4E1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp468);void*_tmp4E2=_tmp4E1;_LL2F9: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp4E3=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp4E2;if(_tmp4E3->tag != 0)goto _LL2FB;}_LL2FA:
 return;_LL2FB:;_LL2FC:
 goto _LL2F8;_LL2F8:;}
# 1548
{const char*_tmp108C;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp467,_tmp460,((_tmp108C="(incompatible pointer types)",_tag_dyneither(_tmp108C,sizeof(char),29))));}
return;_LL2CE:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp46A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp438.f1;if(_tmp46A->tag != 6)goto _LL2D0;else{_tmp46B=_tmp46A->f1;_tmp46C=_tmp46A->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp46D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp438.f2;if(_tmp46D->tag != 6)goto _LL2D0;else{_tmp46E=_tmp46D->f1;_tmp46F=_tmp46D->f2;}};_LL2CF:
# 1553
 if(_tmp46E == _tmp46B  && ((_tmp46F == _tmp46C  || _tmp46F == Cyc_Absyn_Int_sz  && _tmp46C == Cyc_Absyn_Long_sz) || 
_tmp46F == Cyc_Absyn_Long_sz  && _tmp46C == Cyc_Absyn_Int_sz))return;
{const char*_tmp108D;Cyc_Tcutil_failure_reason=((_tmp108D="(different integral types)",_tag_dyneither(_tmp108D,sizeof(char),27)));}
goto _LL2BB;_LL2D0:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp470=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp438.f1;if(_tmp470->tag != 7)goto _LL2D2;else{_tmp471=_tmp470->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp472=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp438.f2;if(_tmp472->tag != 7)goto _LL2D2;else{_tmp473=_tmp472->f1;}};_LL2D1:
# 1559
 if(_tmp473 == 0  && _tmp471 == 0)return;else{
if(_tmp473 == 1  && _tmp471 == 1)return;else{
# 1562
if(((_tmp473 != 0  && _tmp473 != 1) && _tmp471 != 0) && _tmp471 != 1)return;}}
goto _LL2BB;_LL2D2:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp474=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp438.f1;if(_tmp474->tag != 19)goto _LL2D4;else{_tmp475=(void*)_tmp474->f1;}}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp476=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp438.f2;if(_tmp476->tag != 19)goto _LL2D4;else{_tmp477=(void*)_tmp476->f1;}};_LL2D3:
# 1565
 Cyc_Tcutil_unify_it(_tmp475,_tmp477);return;_LL2D4:{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp478=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp438.f1;if(_tmp478->tag != 18)goto _LL2D6;else{_tmp479=_tmp478->f1;}}{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp47A=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp438.f2;if(_tmp47A->tag != 18)goto _LL2D6;else{_tmp47B=_tmp47A->f1;}};_LL2D5:
# 1568
 if(!Cyc_Evexp_same_const_exp(_tmp479,_tmp47B)){
{const char*_tmp108E;Cyc_Tcutil_failure_reason=((_tmp108E="(cannot prove expressions are the same)",_tag_dyneither(_tmp108E,sizeof(char),40)));}
goto _LL2BB;}
# 1572
return;_LL2D6:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp47C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp438.f1;if(_tmp47C->tag != 8)goto _LL2D8;else{_tmp47D=(_tmp47C->f1).elt_type;_tmp47E=(_tmp47C->f1).tq;_tmp47F=(_tmp47C->f1).num_elts;_tmp480=(_tmp47C->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp481=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp438.f2;if(_tmp481->tag != 8)goto _LL2D8;else{_tmp482=(_tmp481->f1).elt_type;_tmp483=(_tmp481->f1).tq;_tmp484=(_tmp481->f1).num_elts;_tmp485=(_tmp481->f1).zero_term;}};_LL2D7:
# 1576
 Cyc_Tcutil_unify_it(_tmp482,_tmp47D);
Cyc_Tcutil_unify_tqual(_tmp483,_tmp482,_tmp47E,_tmp47D);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp108F;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp480,_tmp485,((_tmp108F="(not both zero terminated)",_tag_dyneither(_tmp108F,sizeof(char),27))));}
if(_tmp47F == _tmp484)return;
if(_tmp47F == 0  || _tmp484 == 0)goto _LL2BB;
if(Cyc_Evexp_same_const_exp(_tmp47F,_tmp484))
return;
{const char*_tmp1090;Cyc_Tcutil_failure_reason=((_tmp1090="(different array sizes)",_tag_dyneither(_tmp1090,sizeof(char),24)));}
goto _LL2BB;_LL2D8:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp486=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp438.f1;if(_tmp486->tag != 9)goto _LL2DA;else{_tmp487=(_tmp486->f1).tvars;_tmp488=(_tmp486->f1).effect;_tmp489=(_tmp486->f1).ret_tqual;_tmp48A=(_tmp486->f1).ret_typ;_tmp48B=(_tmp486->f1).args;_tmp48C=(_tmp486->f1).c_varargs;_tmp48D=(_tmp486->f1).cyc_varargs;_tmp48E=(_tmp486->f1).rgn_po;_tmp48F=(_tmp486->f1).attributes;_tmp490=(_tmp486->f1).requires_clause;_tmp491=(_tmp486->f1).requires_relns;_tmp492=(_tmp486->f1).ensures_clause;_tmp493=(_tmp486->f1).ensures_relns;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp494=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp438.f2;if(_tmp494->tag != 9)goto _LL2DA;else{_tmp495=(_tmp494->f1).tvars;_tmp496=(_tmp494->f1).effect;_tmp497=(_tmp494->f1).ret_tqual;_tmp498=(_tmp494->f1).ret_typ;_tmp499=(_tmp494->f1).args;_tmp49A=(_tmp494->f1).c_varargs;_tmp49B=(_tmp494->f1).cyc_varargs;_tmp49C=(_tmp494->f1).rgn_po;_tmp49D=(_tmp494->f1).attributes;_tmp49E=(_tmp494->f1).requires_clause;_tmp49F=(_tmp494->f1).requires_relns;_tmp4A0=(_tmp494->f1).ensures_clause;_tmp4A1=(_tmp494->f1).ensures_relns;}};_LL2D9: {
# 1590
int done=0;
struct _RegionHandle _tmp4E9=_new_region("rgn");struct _RegionHandle*rgn=& _tmp4E9;_push_region(rgn);{
struct Cyc_List_List*inst=0;
while(_tmp495 != 0){
if(_tmp487 == 0){
{const char*_tmp1091;Cyc_Tcutil_failure_reason=((_tmp1091="(second function type has too few type variables)",_tag_dyneither(_tmp1091,sizeof(char),50)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1598
void*_tmp4EB=((struct Cyc_Absyn_Tvar*)_tmp495->hd)->kind;
void*_tmp4EC=((struct Cyc_Absyn_Tvar*)_tmp487->hd)->kind;
if(!Cyc_Tcutil_unify_kindbound(_tmp4EB,_tmp4EC)){
{const char*_tmp1097;void*_tmp1096[3];struct Cyc_String_pa_PrintArg_struct _tmp1095;struct Cyc_String_pa_PrintArg_struct _tmp1094;struct Cyc_String_pa_PrintArg_struct _tmp1093;Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)((_tmp1093.tag=0,((_tmp1093.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1604
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp487->hd,& Cyc_Tcutil_bk))),((_tmp1094.tag=0,((_tmp1094.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1603
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp495->hd,& Cyc_Tcutil_bk))),((_tmp1095.tag=0,((_tmp1095.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1602
Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)_tmp495->hd)),((_tmp1096[0]=& _tmp1095,((_tmp1096[1]=& _tmp1094,((_tmp1096[2]=& _tmp1093,Cyc_aprintf(((_tmp1097="(type var %s has different kinds %s and %s)",_tag_dyneither(_tmp1097,sizeof(char),44))),_tag_dyneither(_tmp1096,sizeof(void*),3))))))))))))))))))));}
# 1605
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1607
{struct _tuple16*_tmp10A1;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp10A0;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp109F;struct Cyc_List_List*_tmp109E;inst=((_tmp109E=_region_malloc(rgn,sizeof(*_tmp109E)),((_tmp109E->hd=((_tmp10A1=_region_malloc(rgn,sizeof(*_tmp10A1)),((_tmp10A1->f1=(struct Cyc_Absyn_Tvar*)_tmp487->hd,((_tmp10A1->f2=(void*)((_tmp109F=_cycalloc(sizeof(*_tmp109F)),((_tmp109F[0]=((_tmp10A0.tag=2,((_tmp10A0.f1=(struct Cyc_Absyn_Tvar*)_tmp495->hd,_tmp10A0)))),_tmp109F)))),_tmp10A1)))))),((_tmp109E->tl=inst,_tmp109E))))));}
_tmp495=_tmp495->tl;
_tmp487=_tmp487->tl;};}
# 1611
if(_tmp487 != 0){
{const char*_tmp10A2;Cyc_Tcutil_failure_reason=((_tmp10A2="(second function type has too many type variables)",_tag_dyneither(_tmp10A2,sizeof(char),51)));}
_npop_handler(0);goto _LL2BB;}
# 1615
if(inst != 0){
{struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp10AE;struct Cyc_Absyn_FnInfo _tmp10AD;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp10AC;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp10A8;struct Cyc_Absyn_FnInfo _tmp10A7;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp10A6;Cyc_Tcutil_unify_it((void*)((_tmp10A6=_cycalloc(sizeof(*_tmp10A6)),((_tmp10A6[0]=((_tmp10A8.tag=9,((_tmp10A8.f1=((_tmp10A7.tvars=0,((_tmp10A7.effect=_tmp496,((_tmp10A7.ret_tqual=_tmp497,((_tmp10A7.ret_typ=_tmp498,((_tmp10A7.args=_tmp499,((_tmp10A7.c_varargs=_tmp49A,((_tmp10A7.cyc_varargs=_tmp49B,((_tmp10A7.rgn_po=_tmp49C,((_tmp10A7.attributes=_tmp49D,((_tmp10A7.requires_clause=_tmp490,((_tmp10A7.requires_relns=_tmp491,((_tmp10A7.ensures_clause=_tmp492,((_tmp10A7.ensures_relns=_tmp493,_tmp10A7)))))))))))))))))))))))))),_tmp10A8)))),_tmp10A6)))),
# 1619
Cyc_Tcutil_rsubstitute(rgn,inst,(void*)(
(_tmp10AC=_cycalloc(sizeof(*_tmp10AC)),((_tmp10AC[0]=((_tmp10AE.tag=9,((_tmp10AE.f1=((_tmp10AD.tvars=0,((_tmp10AD.effect=_tmp488,((_tmp10AD.ret_tqual=_tmp489,((_tmp10AD.ret_typ=_tmp48A,((_tmp10AD.args=_tmp48B,((_tmp10AD.c_varargs=_tmp48C,((_tmp10AD.cyc_varargs=_tmp48D,((_tmp10AD.rgn_po=_tmp48E,((_tmp10AD.attributes=_tmp48F,((_tmp10AD.requires_clause=_tmp49E,((_tmp10AD.requires_relns=_tmp49F,((_tmp10AD.ensures_clause=_tmp4A0,((_tmp10AD.ensures_relns=_tmp4A1,_tmp10AD)))))))))))))))))))))))))),_tmp10AE)))),_tmp10AC))))));}
# 1624
done=1;}}
# 1627
if(done){
_npop_handler(0);return;}
Cyc_Tcutil_unify_it(_tmp498,_tmp48A);
Cyc_Tcutil_unify_tqual(_tmp497,_tmp498,_tmp489,_tmp48A);
for(0;_tmp499 != 0  && _tmp48B != 0;(_tmp499=_tmp499->tl,_tmp48B=_tmp48B->tl)){
struct _tuple10 _tmp4FD=*((struct _tuple10*)_tmp499->hd);struct Cyc_Absyn_Tqual _tmp4FF;void*_tmp500;struct _tuple10 _tmp4FE=_tmp4FD;_tmp4FF=_tmp4FE.f2;_tmp500=_tmp4FE.f3;{
struct _tuple10 _tmp501=*((struct _tuple10*)_tmp48B->hd);struct Cyc_Absyn_Tqual _tmp503;void*_tmp504;struct _tuple10 _tmp502=_tmp501;_tmp503=_tmp502.f2;_tmp504=_tmp502.f3;
Cyc_Tcutil_unify_it(_tmp500,_tmp504);
Cyc_Tcutil_unify_tqual(_tmp4FF,_tmp500,_tmp503,_tmp504);};}
# 1637
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(_tmp499 != 0  || _tmp48B != 0){
{const char*_tmp10AF;Cyc_Tcutil_failure_reason=((_tmp10AF="(function types have different number of arguments)",_tag_dyneither(_tmp10AF,sizeof(char),52)));}
_npop_handler(0);goto _LL2BB;}
# 1643
if(_tmp49A != _tmp48C){
{const char*_tmp10B0;Cyc_Tcutil_failure_reason=((_tmp10B0="(only one function type takes C varargs)",_tag_dyneither(_tmp10B0,sizeof(char),41)));}
_npop_handler(0);goto _LL2BB;}{
# 1648
int bad_cyc_vararg=0;
{struct _tuple22 _tmp10B1;struct _tuple22 _tmp507=(_tmp10B1.f1=_tmp49B,((_tmp10B1.f2=_tmp48D,_tmp10B1)));struct _tuple22 _tmp508=_tmp507;struct _dyneither_ptr*_tmp509;struct Cyc_Absyn_Tqual _tmp50A;void*_tmp50B;int _tmp50C;struct _dyneither_ptr*_tmp50D;struct Cyc_Absyn_Tqual _tmp50E;void*_tmp50F;int _tmp510;_LL2FE: if(_tmp508.f1 != 0)goto _LL300;if(_tmp508.f2 != 0)goto _LL300;_LL2FF:
 goto _LL2FD;_LL300: if(_tmp508.f1 != 0)goto _LL302;_LL301:
 goto _LL303;_LL302: if(_tmp508.f2 != 0)goto _LL304;_LL303:
# 1653
 bad_cyc_vararg=1;
{const char*_tmp10B2;Cyc_Tcutil_failure_reason=((_tmp10B2="(only one function type takes varargs)",_tag_dyneither(_tmp10B2,sizeof(char),39)));}
goto _LL2FD;_LL304: if(_tmp508.f1 == 0)goto _LL2FD;_tmp509=(_tmp508.f1)->name;_tmp50A=(_tmp508.f1)->tq;_tmp50B=(_tmp508.f1)->type;_tmp50C=(_tmp508.f1)->inject;if(_tmp508.f2 == 0)goto _LL2FD;_tmp50D=(_tmp508.f2)->name;_tmp50E=(_tmp508.f2)->tq;_tmp50F=(_tmp508.f2)->type;_tmp510=(_tmp508.f2)->inject;_LL305:
# 1657
 Cyc_Tcutil_unify_it(_tmp50B,_tmp50F);
Cyc_Tcutil_unify_tqual(_tmp50A,_tmp50B,_tmp50E,_tmp50F);
if(_tmp50C != _tmp510){
bad_cyc_vararg=1;{
const char*_tmp10B3;Cyc_Tcutil_failure_reason=((_tmp10B3="(only one function type injects varargs)",_tag_dyneither(_tmp10B3,sizeof(char),41)));};}
# 1663
goto _LL2FD;_LL2FD:;}
# 1665
if(bad_cyc_vararg){_npop_handler(0);goto _LL2BB;}{
# 1668
int bad_effect=0;
{struct _tuple0 _tmp10B4;struct _tuple0 _tmp514=(_tmp10B4.f1=_tmp496,((_tmp10B4.f2=_tmp488,_tmp10B4)));struct _tuple0 _tmp515=_tmp514;_LL307: if(_tmp515.f1 != 0)goto _LL309;if(_tmp515.f2 != 0)goto _LL309;_LL308:
 goto _LL306;_LL309: if(_tmp515.f1 != 0)goto _LL30B;_LL30A:
 goto _LL30C;_LL30B: if(_tmp515.f2 != 0)goto _LL30D;_LL30C:
 bad_effect=1;goto _LL306;_LL30D:;_LL30E:
 bad_effect=!Cyc_Tcutil_unify_effect((void*)_check_null(_tmp496),(void*)_check_null(_tmp488));goto _LL306;_LL306:;}
# 1675
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(bad_effect){
{const char*_tmp10B5;Cyc_Tcutil_failure_reason=((_tmp10B5="(function effects do not match)",_tag_dyneither(_tmp10B5,sizeof(char),32)));}
_npop_handler(0);goto _LL2BB;}
# 1681
if(!Cyc_Tcutil_same_atts(_tmp48F,_tmp49D)){
{const char*_tmp10B6;Cyc_Tcutil_failure_reason=((_tmp10B6="(function types have different attributes)",_tag_dyneither(_tmp10B6,sizeof(char),43)));}
_npop_handler(0);goto _LL2BB;}
# 1685
if(!Cyc_Tcutil_same_rgn_po(_tmp48E,_tmp49C)){
{const char*_tmp10B7;Cyc_Tcutil_failure_reason=((_tmp10B7="(function types have different region lifetime orderings)",_tag_dyneither(_tmp10B7,sizeof(char),58)));}
_npop_handler(0);goto _LL2BB;}
# 1689
if(!Cyc_Tcutil_check_logical_equivalence(_tmp491,_tmp49F)){
{const char*_tmp10B8;Cyc_Tcutil_failure_reason=((_tmp10B8="(@requires clauses not equivalent)",_tag_dyneither(_tmp10B8,sizeof(char),35)));}
_npop_handler(0);goto _LL2BB;}
# 1693
if(!Cyc_Tcutil_check_logical_equivalence(_tmp493,_tmp4A1)){
{const char*_tmp10B9;Cyc_Tcutil_failure_reason=((_tmp10B9="(@ensures clauses not equivalent)",_tag_dyneither(_tmp10B9,sizeof(char),34)));}
_npop_handler(0);goto _LL2BB;}
# 1697
_npop_handler(0);return;};};
# 1591
;_pop_region(rgn);}_LL2DA:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp4A2=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp438.f1;if(_tmp4A2->tag != 10)goto _LL2DC;else{_tmp4A3=_tmp4A2->f1;}}{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp4A4=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp438.f2;if(_tmp4A4->tag != 10)goto _LL2DC;else{_tmp4A5=_tmp4A4->f1;}};_LL2DB:
# 1700
 for(0;_tmp4A5 != 0  && _tmp4A3 != 0;(_tmp4A5=_tmp4A5->tl,_tmp4A3=_tmp4A3->tl)){
struct _tuple12 _tmp51C=*((struct _tuple12*)_tmp4A5->hd);struct Cyc_Absyn_Tqual _tmp51E;void*_tmp51F;struct _tuple12 _tmp51D=_tmp51C;_tmp51E=_tmp51D.f1;_tmp51F=_tmp51D.f2;{
struct _tuple12 _tmp520=*((struct _tuple12*)_tmp4A3->hd);struct Cyc_Absyn_Tqual _tmp522;void*_tmp523;struct _tuple12 _tmp521=_tmp520;_tmp522=_tmp521.f1;_tmp523=_tmp521.f2;
Cyc_Tcutil_unify_it(_tmp51F,_tmp523);
Cyc_Tcutil_unify_tqual(_tmp51E,_tmp51F,_tmp522,_tmp523);};}
# 1706
if(_tmp4A5 == 0  && _tmp4A3 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp10BA;Cyc_Tcutil_failure_reason=((_tmp10BA="(tuple types have different numbers of components)",_tag_dyneither(_tmp10BA,sizeof(char),51)));}
goto _LL2BB;_LL2DC:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp4A6=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp438.f1;if(_tmp4A6->tag != 12)goto _LL2DE;else{_tmp4A7=_tmp4A6->f1;_tmp4A8=_tmp4A6->f2;}}{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp4A9=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp438.f2;if(_tmp4A9->tag != 12)goto _LL2DE;else{_tmp4AA=_tmp4A9->f1;_tmp4AB=_tmp4A9->f2;}};_LL2DD:
# 1713
 if(_tmp4AA != _tmp4A7){{const char*_tmp10BB;Cyc_Tcutil_failure_reason=((_tmp10BB="(struct and union type)",_tag_dyneither(_tmp10BB,sizeof(char),24)));}goto _LL2BB;}
for(0;_tmp4AB != 0  && _tmp4A8 != 0;(_tmp4AB=_tmp4AB->tl,_tmp4A8=_tmp4A8->tl)){
struct Cyc_Absyn_Aggrfield*_tmp526=(struct Cyc_Absyn_Aggrfield*)_tmp4AB->hd;
struct Cyc_Absyn_Aggrfield*_tmp527=(struct Cyc_Absyn_Aggrfield*)_tmp4A8->hd;
if(Cyc_strptrcmp(_tmp526->name,_tmp527->name)!= 0){
{const char*_tmp10BC;Cyc_Tcutil_failure_reason=((_tmp10BC="(different member names)",_tag_dyneither(_tmp10BC,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1721
Cyc_Tcutil_unify_it(_tmp526->type,_tmp527->type);
Cyc_Tcutil_unify_tqual(_tmp526->tq,_tmp526->type,_tmp527->tq,_tmp527->type);
if(!Cyc_Tcutil_same_atts(_tmp526->attributes,_tmp527->attributes)){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp10BD;Cyc_Tcutil_failure_reason=((_tmp10BD="(different attributes on member)",_tag_dyneither(_tmp10BD,sizeof(char),33)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1729
if((_tmp526->width != 0  && _tmp527->width == 0  || 
_tmp527->width != 0  && _tmp526->width == 0) || 
(_tmp526->width != 0  && _tmp527->width != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp526->width),(struct Cyc_Absyn_Exp*)_check_null(_tmp527->width))){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp10BE;Cyc_Tcutil_failure_reason=((_tmp10BE="(different bitfield widths on member)",_tag_dyneither(_tmp10BE,sizeof(char),38)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1738
if((_tmp526->requires_clause != 0  && _tmp527->requires_clause == 0  || 
_tmp526->requires_clause == 0  && _tmp527->requires_clause != 0) || 
(_tmp526->requires_clause == 0  && _tmp527->requires_clause != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp526->requires_clause),(struct Cyc_Absyn_Exp*)_check_null(_tmp527->requires_clause))){
# 1743
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp10BF;Cyc_Tcutil_failure_reason=((_tmp10BF="(different @requires clauses on member)",_tag_dyneither(_tmp10BF,sizeof(char),40)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1749
if(_tmp4AB == 0  && _tmp4A8 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp10C0;Cyc_Tcutil_failure_reason=((_tmp10C0="(different number of members)",_tag_dyneither(_tmp10C0,sizeof(char),30)));}
goto _LL2BB;_LL2DE:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp4AC=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp438.f1;if(_tmp4AC->tag != 17)goto _LL2E0;else{_tmp4AD=_tmp4AC->f2;_tmp4AE=_tmp4AC->f3;}}{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp4AF=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp438.f2;if(_tmp4AF->tag != 17)goto _LL2E0;else{_tmp4B0=_tmp4AF->f2;_tmp4B1=_tmp4AF->f3;}};_LL2DF:
# 1755
 if(_tmp4AE != _tmp4B1){
{const char*_tmp10C1;Cyc_Tcutil_failure_reason=((_tmp10C1="(different abstract typedefs)",_tag_dyneither(_tmp10C1,sizeof(char),30)));}
goto _LL2BB;}
# 1759
{const char*_tmp10C2;Cyc_Tcutil_failure_reason=((_tmp10C2="(type parameters to typedef differ)",_tag_dyneither(_tmp10C2,sizeof(char),36)));}
Cyc_Tcutil_unify_list(_tmp4AD,_tmp4B0);
return;_LL2E0:{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp4B2=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp438.f1;if(_tmp4B2->tag != 20)goto _LL2E2;}{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp4B3=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp438.f2;if(_tmp4B3->tag != 20)goto _LL2E2;};_LL2E1:
 return;_LL2E2:{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp4B4=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp438.f1;if(_tmp4B4->tag != 21)goto _LL2E4;}{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp4B5=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp438.f2;if(_tmp4B5->tag != 21)goto _LL2E4;};_LL2E3:
 return;_LL2E4:{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp4B6=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp438.f1;if(_tmp4B6->tag != 22)goto _LL2E6;}{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp4B7=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp438.f2;if(_tmp4B7->tag != 22)goto _LL2E6;};_LL2E5:
 return;_LL2E6:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4B8=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp438.f1;if(_tmp4B8->tag != 15)goto _LL2E8;else{_tmp4B9=(void*)_tmp4B8->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4BA=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp438.f2;if(_tmp4BA->tag != 15)goto _LL2E8;else{_tmp4BB=(void*)_tmp4BA->f1;}};_LL2E7:
# 1766
 Cyc_Tcutil_unify_it(_tmp4B9,_tmp4BB);
return;_LL2E8:{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp4BC=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp438.f1;if(_tmp4BC->tag != 16)goto _LL2EA;else{_tmp4BD=(void*)_tmp4BC->f1;_tmp4BE=(void*)_tmp4BC->f2;}}{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp4BF=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp438.f2;if(_tmp4BF->tag != 16)goto _LL2EA;else{_tmp4C0=(void*)_tmp4BF->f1;_tmp4C1=(void*)_tmp4BF->f2;}};_LL2E9:
# 1769
 Cyc_Tcutil_unify_it(_tmp4BD,_tmp4C0);
Cyc_Tcutil_unify_it(_tmp4BE,_tmp4C1);
return;_LL2EA: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp4C2=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp438.f1;if(_tmp4C2->tag != 24)goto _LL2EC;}_LL2EB:
 goto _LL2ED;_LL2EC: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp4C3=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp438.f2;if(_tmp4C3->tag != 24)goto _LL2EE;}_LL2ED:
 goto _LL2EF;_LL2EE: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4C4=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp438.f1;if(_tmp4C4->tag != 23)goto _LL2F0;}_LL2EF:
 goto _LL2F1;_LL2F0: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp4C5=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp438.f1;if(_tmp4C5->tag != 25)goto _LL2F2;}_LL2F1:
 goto _LL2F3;_LL2F2: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp4C6=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp438.f2;if(_tmp4C6->tag != 25)goto _LL2F4;}_LL2F3:
 goto _LL2F5;_LL2F4: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4C7=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp438.f2;if(_tmp4C7->tag != 23)goto _LL2F6;}_LL2F5:
# 1778
 if(Cyc_Tcutil_unify_effect(t1,t2))return;
{const char*_tmp10C3;Cyc_Tcutil_failure_reason=((_tmp10C3="(effects don't unify)",_tag_dyneither(_tmp10C3,sizeof(char),22)));}
goto _LL2BB;_LL2F6:;_LL2F7:
 goto _LL2BB;_LL2BB:;}
# 1783
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1786
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0  && a2 != 0)return - 1;
if(a1 != 0  && a2 == 0)return 1;
return cmp((void*)_check_null(a1),(void*)_check_null(a2));}
# 1793
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp531=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
int _tmp532=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp531,_tmp532);}
# 1800
static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return 0;{
union Cyc_Absyn_Constraint*_tmp533=x;void*_tmp534;_LL310: if((_tmp533->No_constr).tag != 3)goto _LL312;_LL311:
 return - 1;_LL312: if((_tmp533->Eq_constr).tag != 1)goto _LL314;_tmp534=(void*)(_tmp533->Eq_constr).val;_LL313: {
# 1807
union Cyc_Absyn_Constraint*_tmp535=y;void*_tmp536;_LL317: if((_tmp535->No_constr).tag != 3)goto _LL319;_LL318:
 return 1;_LL319: if((_tmp535->Eq_constr).tag != 1)goto _LL31B;_tmp536=(void*)(_tmp535->Eq_constr).val;_LL31A:
 return cmp(_tmp534,_tmp536);_LL31B: if((_tmp535->Forward_constr).tag != 2)goto _LL316;_LL31C: {
const char*_tmp10C6;void*_tmp10C5;(_tmp10C5=0,Cyc_Tcutil_impos(((_tmp10C6="unify_conref: forward after compress(2)",_tag_dyneither(_tmp10C6,sizeof(char),40))),_tag_dyneither(_tmp10C5,sizeof(void*),0)));}_LL316:;}_LL314: if((_tmp533->Forward_constr).tag != 2)goto _LL30F;_LL315: {
# 1812
const char*_tmp10C9;void*_tmp10C8;(_tmp10C8=0,Cyc_Tcutil_impos(((_tmp10C9="unify_conref: forward after compress",_tag_dyneither(_tmp10C9,sizeof(char),37))),_tag_dyneither(_tmp10C8,sizeof(void*),0)));}_LL30F:;};}
# 1816
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct Cyc_Absyn_Tqual _tmp53C;void*_tmp53D;struct _tuple12*_tmp53B=tqt1;_tmp53C=_tmp53B->f1;_tmp53D=_tmp53B->f2;{
struct Cyc_Absyn_Tqual _tmp53F;void*_tmp540;struct _tuple12*_tmp53E=tqt2;_tmp53F=_tmp53E->f1;_tmp540=_tmp53E->f2;{
int _tmp541=Cyc_Tcutil_tqual_cmp(_tmp53C,_tmp53F);
if(_tmp541 != 0)return _tmp541;
return Cyc_Tcutil_typecmp(_tmp53D,_tmp540);};};}
# 1824
static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp542=Cyc_strptrcmp(f1->name,f2->name);
if(_tmp542 != 0)return _tmp542;{
int _tmp543=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);
if(_tmp543 != 0)return _tmp543;{
int _tmp544=Cyc_Tcutil_typecmp(f1->type,f2->type);
if(_tmp544 != 0)return _tmp544;{
int _tmp545=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);
if(_tmp545 != 0)return _tmp545;
_tmp545=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);
if(_tmp545 != 0)return _tmp545;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->requires_clause,f2->requires_clause);};};};}
# 1838
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp546=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp546 != 0)return _tmp546;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}
# 1844
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp547=t;_LL31E: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp548=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp547;if(_tmp548->tag != 0)goto _LL320;}_LL31F:
 return 0;_LL320: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp549=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp547;if(_tmp549->tag != 1)goto _LL322;}_LL321:
 return 1;_LL322: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp54A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp547;if(_tmp54A->tag != 2)goto _LL324;}_LL323:
 return 2;_LL324: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp54B=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp547;if(_tmp54B->tag != 3)goto _LL326;}_LL325:
 return 3;_LL326: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp54C=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp547;if(_tmp54C->tag != 4)goto _LL328;}_LL327:
 return 4;_LL328: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp54D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp547;if(_tmp54D->tag != 5)goto _LL32A;}_LL329:
 return 5;_LL32A: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp54E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp547;if(_tmp54E->tag != 6)goto _LL32C;}_LL32B:
 return 6;_LL32C: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp54F=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp547;if(_tmp54F->tag != 7)goto _LL32E;else{if(_tmp54F->f1 != 0)goto _LL32E;}}_LL32D:
 return 7;_LL32E: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp550=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp547;if(_tmp550->tag != 7)goto _LL330;}_LL32F:
 return 8;_LL330: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp551=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp547;if(_tmp551->tag != 8)goto _LL332;}_LL331:
 return 9;_LL332: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp552=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp547;if(_tmp552->tag != 9)goto _LL334;}_LL333:
 return 10;_LL334: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp553=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp547;if(_tmp553->tag != 10)goto _LL336;}_LL335:
 return 11;_LL336: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp554=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp547;if(_tmp554->tag != 11)goto _LL338;}_LL337:
 return 12;_LL338: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp555=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp547;if(_tmp555->tag != 12)goto _LL33A;}_LL339:
 return 14;_LL33A: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp556=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp547;if(_tmp556->tag != 13)goto _LL33C;}_LL33B:
 return 16;_LL33C: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp557=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp547;if(_tmp557->tag != 14)goto _LL33E;}_LL33D:
 return 17;_LL33E: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp558=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp547;if(_tmp558->tag != 15)goto _LL340;}_LL33F:
 return 18;_LL340: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp559=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp547;if(_tmp559->tag != 17)goto _LL342;}_LL341:
 return 19;_LL342: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp55A=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp547;if(_tmp55A->tag != 21)goto _LL344;}_LL343:
 return 20;_LL344: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp55B=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp547;if(_tmp55B->tag != 20)goto _LL346;}_LL345:
 return 21;_LL346: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp55C=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp547;if(_tmp55C->tag != 23)goto _LL348;}_LL347:
 return 22;_LL348: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp55D=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp547;if(_tmp55D->tag != 24)goto _LL34A;}_LL349:
 return 23;_LL34A: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp55E=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp547;if(_tmp55E->tag != 25)goto _LL34C;}_LL34B:
 return 24;_LL34C: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp55F=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp547;if(_tmp55F->tag != 22)goto _LL34E;}_LL34D:
 return 25;_LL34E: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp560=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp547;if(_tmp560->tag != 19)goto _LL350;}_LL34F:
 return 26;_LL350: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp561=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp547;if(_tmp561->tag != 16)goto _LL352;}_LL351:
 return 27;_LL352: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp562=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp547;if(_tmp562->tag != 18)goto _LL354;}_LL353:
 return 28;_LL354: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp563=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp547;if(_tmp563->tag != 26)goto _LL31D;}_LL355:
 return 29;_LL31D:;}
# 1879
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return 0;{
int _tmp564=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));
if(_tmp564 != 0)
return _tmp564;{
# 1888
struct _tuple0 _tmp10CA;struct _tuple0 _tmp565=(_tmp10CA.f1=t2,((_tmp10CA.f2=t1,_tmp10CA)));struct _tuple0 _tmp566=_tmp565;struct Cyc_Absyn_Tvar*_tmp56A;struct Cyc_Absyn_Tvar*_tmp56C;union Cyc_Absyn_AggrInfoU _tmp56E;struct Cyc_List_List*_tmp56F;union Cyc_Absyn_AggrInfoU _tmp571;struct Cyc_List_List*_tmp572;struct _tuple2*_tmp574;struct _tuple2*_tmp576;struct Cyc_List_List*_tmp578;struct Cyc_List_List*_tmp57A;struct Cyc_Absyn_Datatypedecl*_tmp57C;struct Cyc_List_List*_tmp57D;struct Cyc_Absyn_Datatypedecl*_tmp57F;struct Cyc_List_List*_tmp580;struct Cyc_Absyn_Datatypedecl*_tmp582;struct Cyc_Absyn_Datatypefield*_tmp583;struct Cyc_List_List*_tmp584;struct Cyc_Absyn_Datatypedecl*_tmp586;struct Cyc_Absyn_Datatypefield*_tmp587;struct Cyc_List_List*_tmp588;void*_tmp58A;struct Cyc_Absyn_Tqual _tmp58B;void*_tmp58C;union Cyc_Absyn_Constraint*_tmp58D;union Cyc_Absyn_Constraint*_tmp58E;union Cyc_Absyn_Constraint*_tmp58F;void*_tmp591;struct Cyc_Absyn_Tqual _tmp592;void*_tmp593;union Cyc_Absyn_Constraint*_tmp594;union Cyc_Absyn_Constraint*_tmp595;union Cyc_Absyn_Constraint*_tmp596;enum Cyc_Absyn_Sign _tmp598;enum Cyc_Absyn_Size_of _tmp599;enum Cyc_Absyn_Sign _tmp59B;enum Cyc_Absyn_Size_of _tmp59C;int _tmp59E;int _tmp5A0;void*_tmp5A2;struct Cyc_Absyn_Tqual _tmp5A3;struct Cyc_Absyn_Exp*_tmp5A4;union Cyc_Absyn_Constraint*_tmp5A5;void*_tmp5A7;struct Cyc_Absyn_Tqual _tmp5A8;struct Cyc_Absyn_Exp*_tmp5A9;union Cyc_Absyn_Constraint*_tmp5AA;struct Cyc_List_List*_tmp5AE;struct Cyc_List_List*_tmp5B0;enum Cyc_Absyn_AggrKind _tmp5B2;struct Cyc_List_List*_tmp5B3;enum Cyc_Absyn_AggrKind _tmp5B5;struct Cyc_List_List*_tmp5B6;void*_tmp5B8;void*_tmp5BA;void*_tmp5BC;void*_tmp5BD;void*_tmp5BF;void*_tmp5C0;void*_tmp5C2;void*_tmp5C4;struct Cyc_Absyn_Exp*_tmp5C6;struct Cyc_Absyn_Exp*_tmp5C8;_LL357:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp567=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp566.f1;if(_tmp567->tag != 1)goto _LL359;}{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp568=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp566.f2;if(_tmp568->tag != 1)goto _LL359;};_LL358: {
# 1890
const char*_tmp10CD;void*_tmp10CC;(_tmp10CC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10CD="typecmp: can only compare closed types",_tag_dyneither(_tmp10CD,sizeof(char),39))),_tag_dyneither(_tmp10CC,sizeof(void*),0)));}_LL359:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp569=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp566.f1;if(_tmp569->tag != 2)goto _LL35B;else{_tmp56A=_tmp569->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp56B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp566.f2;if(_tmp56B->tag != 2)goto _LL35B;else{_tmp56C=_tmp56B->f1;}};_LL35A:
# 1894
 return Cyc_Core_intcmp(_tmp56C->identity,_tmp56A->identity);_LL35B:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp56D=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp566.f1;if(_tmp56D->tag != 11)goto _LL35D;else{_tmp56E=(_tmp56D->f1).aggr_info;_tmp56F=(_tmp56D->f1).targs;}}{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp570=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp566.f2;if(_tmp570->tag != 11)goto _LL35D;else{_tmp571=(_tmp570->f1).aggr_info;_tmp572=(_tmp570->f1).targs;}};_LL35C: {
# 1898
struct _tuple13 _tmp5D1=Cyc_Absyn_aggr_kinded_name(_tmp56E);struct _tuple2*_tmp5D3;struct _tuple13 _tmp5D2=_tmp5D1;_tmp5D3=_tmp5D2.f2;{
struct _tuple13 _tmp5D4=Cyc_Absyn_aggr_kinded_name(_tmp571);struct _tuple2*_tmp5D6;struct _tuple13 _tmp5D5=_tmp5D4;_tmp5D6=_tmp5D5.f2;{
int _tmp5D7=Cyc_Absyn_qvar_cmp(_tmp5D3,_tmp5D6);
if(_tmp5D7 != 0)return _tmp5D7;else{
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp56F,_tmp572);}};};}_LL35D:{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp573=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp566.f1;if(_tmp573->tag != 13)goto _LL35F;else{_tmp574=_tmp573->f1;}}{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp575=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp566.f2;if(_tmp575->tag != 13)goto _LL35F;else{_tmp576=_tmp575->f1;}};_LL35E:
# 1905
 return Cyc_Absyn_qvar_cmp(_tmp574,_tmp576);_LL35F:{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp577=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp566.f1;if(_tmp577->tag != 14)goto _LL361;else{_tmp578=_tmp577->f1;}}{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp579=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp566.f2;if(_tmp579->tag != 14)goto _LL361;else{_tmp57A=_tmp579->f1;}};_LL360:
# 1908
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp578,_tmp57A);_LL361:{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp57B=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp566.f1;if(_tmp57B->tag != 3)goto _LL363;else{if((((_tmp57B->f1).datatype_info).KnownDatatype).tag != 2)goto _LL363;_tmp57C=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp57B->f1).datatype_info).KnownDatatype).val);_tmp57D=(_tmp57B->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp57E=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp566.f2;if(_tmp57E->tag != 3)goto _LL363;else{if((((_tmp57E->f1).datatype_info).KnownDatatype).tag != 2)goto _LL363;_tmp57F=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp57E->f1).datatype_info).KnownDatatype).val);_tmp580=(_tmp57E->f1).targs;}};_LL362:
# 1912
 if(_tmp57F == _tmp57C)return 0;{
int _tmp5D8=Cyc_Absyn_qvar_cmp(_tmp57F->name,_tmp57C->name);
if(_tmp5D8 != 0)return _tmp5D8;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp580,_tmp57D);};_LL363:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp581=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp566.f1;if(_tmp581->tag != 4)goto _LL365;else{if((((_tmp581->f1).field_info).KnownDatatypefield).tag != 2)goto _LL365;_tmp582=((struct _tuple3)(((_tmp581->f1).field_info).KnownDatatypefield).val).f1;_tmp583=((struct _tuple3)(((_tmp581->f1).field_info).KnownDatatypefield).val).f2;_tmp584=(_tmp581->f1).targs;}}{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp585=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp566.f2;if(_tmp585->tag != 4)goto _LL365;else{if((((_tmp585->f1).field_info).KnownDatatypefield).tag != 2)goto _LL365;_tmp586=((struct _tuple3)(((_tmp585->f1).field_info).KnownDatatypefield).val).f1;_tmp587=((struct _tuple3)(((_tmp585->f1).field_info).KnownDatatypefield).val).f2;_tmp588=(_tmp585->f1).targs;}};_LL364:
# 1919
 if(_tmp586 == _tmp582)return 0;{
int _tmp5D9=Cyc_Absyn_qvar_cmp(_tmp582->name,_tmp586->name);
if(_tmp5D9 != 0)return _tmp5D9;{
int _tmp5DA=Cyc_Absyn_qvar_cmp(_tmp583->name,_tmp587->name);
if(_tmp5DA != 0)return _tmp5DA;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp588,_tmp584);};};_LL365:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp589=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp566.f1;if(_tmp589->tag != 5)goto _LL367;else{_tmp58A=(_tmp589->f1).elt_typ;_tmp58B=(_tmp589->f1).elt_tq;_tmp58C=((_tmp589->f1).ptr_atts).rgn;_tmp58D=((_tmp589->f1).ptr_atts).nullable;_tmp58E=((_tmp589->f1).ptr_atts).bounds;_tmp58F=((_tmp589->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp590=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp566.f2;if(_tmp590->tag != 5)goto _LL367;else{_tmp591=(_tmp590->f1).elt_typ;_tmp592=(_tmp590->f1).elt_tq;_tmp593=((_tmp590->f1).ptr_atts).rgn;_tmp594=((_tmp590->f1).ptr_atts).nullable;_tmp595=((_tmp590->f1).ptr_atts).bounds;_tmp596=((_tmp590->f1).ptr_atts).zero_term;}};_LL366: {
# 1928
int _tmp5DB=Cyc_Tcutil_typecmp(_tmp591,_tmp58A);
if(_tmp5DB != 0)return _tmp5DB;{
int _tmp5DC=Cyc_Tcutil_typecmp(_tmp593,_tmp58C);
if(_tmp5DC != 0)return _tmp5DC;{
int _tmp5DD=Cyc_Tcutil_tqual_cmp(_tmp592,_tmp58B);
if(_tmp5DD != 0)return _tmp5DD;{
int _tmp5DE=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp595,_tmp58E);
if(_tmp5DE != 0)return _tmp5DE;{
int _tmp5DF=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp596,_tmp58F);
if(_tmp5DF != 0)return _tmp5DF;
{void*_tmp5E0=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp595);void*_tmp5E1=_tmp5E0;_LL38A: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5E2=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5E1;if(_tmp5E2->tag != 0)goto _LL38C;}_LL38B:
 return 0;_LL38C:;_LL38D:
 goto _LL389;_LL389:;}
# 1942
return((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp594,_tmp58D);};};};};}_LL367:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp597=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp566.f1;if(_tmp597->tag != 6)goto _LL369;else{_tmp598=_tmp597->f1;_tmp599=_tmp597->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp59A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp566.f2;if(_tmp59A->tag != 6)goto _LL369;else{_tmp59B=_tmp59A->f1;_tmp59C=_tmp59A->f2;}};_LL368:
# 1945
 if(_tmp59B != _tmp598)return Cyc_Core_intcmp((int)((unsigned int)_tmp59B),(int)((unsigned int)_tmp598));
if(_tmp59C != _tmp599)return Cyc_Core_intcmp((int)((unsigned int)_tmp59C),(int)((unsigned int)_tmp599));
return 0;_LL369:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp59D=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp566.f1;if(_tmp59D->tag != 7)goto _LL36B;else{_tmp59E=_tmp59D->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp59F=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp566.f2;if(_tmp59F->tag != 7)goto _LL36B;else{_tmp5A0=_tmp59F->f1;}};_LL36A:
# 1950
 if(_tmp59E == _tmp5A0)return 0;else{
if(_tmp5A0 == 0)return - 1;else{
if(_tmp5A0 == 1  && _tmp59E == 0)return - 1;else{
return 1;}}}_LL36B:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5A1=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp566.f1;if(_tmp5A1->tag != 8)goto _LL36D;else{_tmp5A2=(_tmp5A1->f1).elt_type;_tmp5A3=(_tmp5A1->f1).tq;_tmp5A4=(_tmp5A1->f1).num_elts;_tmp5A5=(_tmp5A1->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5A6=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp566.f2;if(_tmp5A6->tag != 8)goto _LL36D;else{_tmp5A7=(_tmp5A6->f1).elt_type;_tmp5A8=(_tmp5A6->f1).tq;_tmp5A9=(_tmp5A6->f1).num_elts;_tmp5AA=(_tmp5A6->f1).zero_term;}};_LL36C: {
# 1957
int _tmp5E3=Cyc_Tcutil_tqual_cmp(_tmp5A8,_tmp5A3);
if(_tmp5E3 != 0)return _tmp5E3;{
int _tmp5E4=Cyc_Tcutil_typecmp(_tmp5A7,_tmp5A2);
if(_tmp5E4 != 0)return _tmp5E4;{
int _tmp5E5=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp5A5,_tmp5AA);
if(_tmp5E5 != 0)return _tmp5E5;
if(_tmp5A4 == _tmp5A9)return 0;
if(_tmp5A4 == 0  || _tmp5A9 == 0){
const char*_tmp10D0;void*_tmp10CF;(_tmp10CF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10D0="missing expression in array index",_tag_dyneither(_tmp10D0,sizeof(char),34))),_tag_dyneither(_tmp10CF,sizeof(void*),0)));}
# 1967
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp5A4,_tmp5A9);};};}_LL36D:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp5AB=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp566.f1;if(_tmp5AB->tag != 9)goto _LL36F;}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp5AC=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp566.f2;if(_tmp5AC->tag != 9)goto _LL36F;};_LL36E: {
# 1970
const char*_tmp10D3;void*_tmp10D2;(_tmp10D2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10D3="typecmp: function types not handled",_tag_dyneither(_tmp10D3,sizeof(char),36))),_tag_dyneither(_tmp10D2,sizeof(void*),0)));}_LL36F:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp5AD=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp566.f1;if(_tmp5AD->tag != 10)goto _LL371;else{_tmp5AE=_tmp5AD->f1;}}{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp5AF=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp566.f2;if(_tmp5AF->tag != 10)goto _LL371;else{_tmp5B0=_tmp5AF->f1;}};_LL370:
# 1973
 return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp5B0,_tmp5AE);_LL371:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp5B1=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp566.f1;if(_tmp5B1->tag != 12)goto _LL373;else{_tmp5B2=_tmp5B1->f1;_tmp5B3=_tmp5B1->f2;}}{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp5B4=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp566.f2;if(_tmp5B4->tag != 12)goto _LL373;else{_tmp5B5=_tmp5B4->f1;_tmp5B6=_tmp5B4->f2;}};_LL372:
# 1976
 if(_tmp5B5 != _tmp5B2){
if(_tmp5B5 == Cyc_Absyn_StructA)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp5B6,_tmp5B3);_LL373:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp5B7=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp566.f1;if(_tmp5B7->tag != 15)goto _LL375;else{_tmp5B8=(void*)_tmp5B7->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp5B9=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp566.f2;if(_tmp5B9->tag != 15)goto _LL375;else{_tmp5BA=(void*)_tmp5B9->f1;}};_LL374:
# 1981
 return Cyc_Tcutil_typecmp(_tmp5B8,_tmp5BA);_LL375:{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp5BB=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp566.f1;if(_tmp5BB->tag != 16)goto _LL377;else{_tmp5BC=(void*)_tmp5BB->f1;_tmp5BD=(void*)_tmp5BB->f2;}}{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp5BE=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp566.f2;if(_tmp5BE->tag != 16)goto _LL377;else{_tmp5BF=(void*)_tmp5BE->f1;_tmp5C0=(void*)_tmp5BE->f2;}};_LL376: {
# 1983
int _tmp5EA=Cyc_Tcutil_typecmp(_tmp5BC,_tmp5BF);
if(_tmp5EA != 0)return _tmp5EA;else{
return Cyc_Tcutil_typecmp(_tmp5BD,_tmp5C0);}}_LL377:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5C1=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp566.f1;if(_tmp5C1->tag != 19)goto _LL379;else{_tmp5C2=(void*)_tmp5C1->f1;}}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5C3=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp566.f2;if(_tmp5C3->tag != 19)goto _LL379;else{_tmp5C4=(void*)_tmp5C3->f1;}};_LL378:
 return Cyc_Tcutil_typecmp(_tmp5C2,_tmp5C4);_LL379:{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp5C5=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp566.f1;if(_tmp5C5->tag != 18)goto _LL37B;else{_tmp5C6=_tmp5C5->f1;}}{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp5C7=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp566.f2;if(_tmp5C7->tag != 18)goto _LL37B;else{_tmp5C8=_tmp5C7->f1;}};_LL37A:
# 1988
 return Cyc_Evexp_const_exp_cmp(_tmp5C6,_tmp5C8);_LL37B: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp5C9=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp566.f1;if(_tmp5C9->tag != 24)goto _LL37D;}_LL37C:
 goto _LL37E;_LL37D: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp5CA=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp566.f2;if(_tmp5CA->tag != 24)goto _LL37F;}_LL37E:
 goto _LL380;_LL37F: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp5CB=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp566.f1;if(_tmp5CB->tag != 23)goto _LL381;}_LL380:
 goto _LL382;_LL381: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp5CC=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp566.f1;if(_tmp5CC->tag != 25)goto _LL383;}_LL382:
 goto _LL384;_LL383: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp5CD=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp566.f2;if(_tmp5CD->tag != 25)goto _LL385;}_LL384:
 goto _LL386;_LL385: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp5CE=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp566.f2;if(_tmp5CE->tag != 23)goto _LL387;}_LL386: {
const char*_tmp10D6;void*_tmp10D5;(_tmp10D5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10D6="typecmp: effects not handled",_tag_dyneither(_tmp10D6,sizeof(char),29))),_tag_dyneither(_tmp10D5,sizeof(void*),0)));}_LL387:;_LL388: {
const char*_tmp10D9;void*_tmp10D8;(_tmp10D8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10D9="Unmatched case in typecmp",_tag_dyneither(_tmp10D9,sizeof(char),26))),_tag_dyneither(_tmp10D8,sizeof(void*),0)));}_LL356:;};};}
# 1999
int Cyc_Tcutil_is_arithmetic_type(void*t){
void*_tmp5F0=Cyc_Tcutil_compress(t);void*_tmp5F1=_tmp5F0;_LL38F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5F2=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F1;if(_tmp5F2->tag != 6)goto _LL391;}_LL390:
 goto _LL392;_LL391: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5F3=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5F1;if(_tmp5F3->tag != 7)goto _LL393;}_LL392:
 goto _LL394;_LL393: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp5F4=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5F1;if(_tmp5F4->tag != 13)goto _LL395;}_LL394:
 goto _LL396;_LL395: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp5F5=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp5F1;if(_tmp5F5->tag != 14)goto _LL397;}_LL396:
 return 1;_LL397:;_LL398:
 return 0;_LL38E:;}
# 2011
int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp10DA;struct _tuple0 _tmp5F6=(_tmp10DA.f1=t1,((_tmp10DA.f2=t2,_tmp10DA)));struct _tuple0 _tmp5F7=_tmp5F6;int _tmp5F9;int _tmp5FB;_LL39A:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5F8=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5F7.f1;if(_tmp5F8->tag != 7)goto _LL39C;else{_tmp5F9=_tmp5F8->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5FA=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5F7.f2;if(_tmp5FA->tag != 7)goto _LL39C;else{_tmp5FB=_tmp5FA->f1;}};_LL39B:
# 2016
 return _tmp5FB == 0  && _tmp5F9 != 0  || (_tmp5FB == 1  && _tmp5F9 != 0) && _tmp5F9 != 1;_LL39C:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5FC=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5F7.f1;if(_tmp5FC->tag != 7)goto _LL39E;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5FD=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F7.f2;if(_tmp5FD->tag != 6)goto _LL39E;};_LL39D:
 goto _LL39F;_LL39E:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5FE=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5F7.f1;if(_tmp5FE->tag != 7)goto _LL3A0;}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5FF=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5F7.f2;if(_tmp5FF->tag != 19)goto _LL3A0;};_LL39F:
 return 1;_LL3A0:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp600=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F7.f1;if(_tmp600->tag != 6)goto _LL3A2;else{if(_tmp600->f2 != Cyc_Absyn_LongLong_sz)goto _LL3A2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp601=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F7.f2;if(_tmp601->tag != 6)goto _LL3A2;else{if(_tmp601->f2 != Cyc_Absyn_LongLong_sz)goto _LL3A2;}};_LL3A1:
 return 0;_LL3A2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp602=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F7.f1;if(_tmp602->tag != 6)goto _LL3A4;else{if(_tmp602->f2 != Cyc_Absyn_LongLong_sz)goto _LL3A4;}}_LL3A3:
 return 1;_LL3A4:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp603=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F7.f1;if(_tmp603->tag != 6)goto _LL3A6;else{if(_tmp603->f2 != Cyc_Absyn_Long_sz)goto _LL3A6;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp604=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F7.f2;if(_tmp604->tag != 6)goto _LL3A6;else{if(_tmp604->f2 != Cyc_Absyn_Int_sz)goto _LL3A6;}};_LL3A5:
# 2023
 goto _LL3A7;_LL3A6:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp605=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F7.f1;if(_tmp605->tag != 6)goto _LL3A8;else{if(_tmp605->f2 != Cyc_Absyn_Int_sz)goto _LL3A8;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp606=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F7.f2;if(_tmp606->tag != 6)goto _LL3A8;else{if(_tmp606->f2 != Cyc_Absyn_Long_sz)goto _LL3A8;}};_LL3A7:
 return 0;_LL3A8:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp607=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F7.f1;if(_tmp607->tag != 6)goto _LL3AA;else{if(_tmp607->f2 != Cyc_Absyn_Long_sz)goto _LL3AA;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp608=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5F7.f2;if(_tmp608->tag != 7)goto _LL3AA;else{if(_tmp608->f1 != 0)goto _LL3AA;}};_LL3A9:
# 2026
 goto _LL3AB;_LL3AA:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp609=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F7.f1;if(_tmp609->tag != 6)goto _LL3AC;else{if(_tmp609->f2 != Cyc_Absyn_Int_sz)goto _LL3AC;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp60A=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5F7.f2;if(_tmp60A->tag != 7)goto _LL3AC;else{if(_tmp60A->f1 != 0)goto _LL3AC;}};_LL3AB:
 goto _LL3AD;_LL3AC:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp60B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F7.f1;if(_tmp60B->tag != 6)goto _LL3AE;else{if(_tmp60B->f2 != Cyc_Absyn_Long_sz)goto _LL3AE;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp60C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F7.f2;if(_tmp60C->tag != 6)goto _LL3AE;else{if(_tmp60C->f2 != Cyc_Absyn_Short_sz)goto _LL3AE;}};_LL3AD:
 goto _LL3AF;_LL3AE:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp60D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F7.f1;if(_tmp60D->tag != 6)goto _LL3B0;else{if(_tmp60D->f2 != Cyc_Absyn_Int_sz)goto _LL3B0;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp60E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F7.f2;if(_tmp60E->tag != 6)goto _LL3B0;else{if(_tmp60E->f2 != Cyc_Absyn_Short_sz)goto _LL3B0;}};_LL3AF:
 goto _LL3B1;_LL3B0:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp60F=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5F7.f1;if(_tmp60F->tag != 19)goto _LL3B2;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp610=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F7.f2;if(_tmp610->tag != 6)goto _LL3B2;else{if(_tmp610->f2 != Cyc_Absyn_Short_sz)goto _LL3B2;}};_LL3B1:
 goto _LL3B3;_LL3B2:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp611=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F7.f1;if(_tmp611->tag != 6)goto _LL3B4;else{if(_tmp611->f2 != Cyc_Absyn_Long_sz)goto _LL3B4;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp612=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F7.f2;if(_tmp612->tag != 6)goto _LL3B4;else{if(_tmp612->f2 != Cyc_Absyn_Char_sz)goto _LL3B4;}};_LL3B3:
 goto _LL3B5;_LL3B4:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp613=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F7.f1;if(_tmp613->tag != 6)goto _LL3B6;else{if(_tmp613->f2 != Cyc_Absyn_Int_sz)goto _LL3B6;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp614=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F7.f2;if(_tmp614->tag != 6)goto _LL3B6;else{if(_tmp614->f2 != Cyc_Absyn_Char_sz)goto _LL3B6;}};_LL3B5:
 goto _LL3B7;_LL3B6:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp615=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F7.f1;if(_tmp615->tag != 6)goto _LL3B8;else{if(_tmp615->f2 != Cyc_Absyn_Short_sz)goto _LL3B8;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp616=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F7.f2;if(_tmp616->tag != 6)goto _LL3B8;else{if(_tmp616->f2 != Cyc_Absyn_Char_sz)goto _LL3B8;}};_LL3B7:
 goto _LL3B9;_LL3B8:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp617=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5F7.f1;if(_tmp617->tag != 19)goto _LL3BA;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp618=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F7.f2;if(_tmp618->tag != 6)goto _LL3BA;else{if(_tmp618->f2 != Cyc_Absyn_Char_sz)goto _LL3BA;}};_LL3B9:
 return 1;_LL3BA:;_LL3BB:
# 2036
 return 0;_LL399:;};}
# 2042
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
# 2045
struct _RegionHandle _tmp61A=_new_region("r");struct _RegionHandle*r=& _tmp61A;_push_region(r);{
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0  || 
Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v)){
struct Cyc_Core_Opt*_tmp10DB;max_arith_type=((_tmp10DB=_region_malloc(r,sizeof(*_tmp10DB)),((_tmp10DB->v=t1,_tmp10DB))));}}}}
# 2055
if(max_arith_type != 0){
if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v)){
int _tmp61C=0;_npop_handler(0);return _tmp61C;}}}
# 2060
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){
{const char*_tmp10E0;void*_tmp10DF[2];struct Cyc_String_pa_PrintArg_struct _tmp10DE;struct Cyc_String_pa_PrintArg_struct _tmp10DD;(_tmp10DD.tag=0,((_tmp10DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))),((_tmp10DE.tag=0,((_tmp10DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp10DF[0]=& _tmp10DE,((_tmp10DF[1]=& _tmp10DD,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)el->hd)->loc,((_tmp10E0="type mismatch: expecting %s but found %s",_tag_dyneither(_tmp10E0,sizeof(char),41))),_tag_dyneither(_tmp10DF,sizeof(void*),2)))))))))))));}{
int _tmp621=0;_npop_handler(0);return _tmp621;};}}}{
# 2066
int _tmp622=1;_npop_handler(0);return _tmp622;};
# 2045
;_pop_region(r);}
# 2071
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_typ(te,e)){
void*_tmp623=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp624=_tmp623;_LL3BD: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp625=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp624;if(_tmp625->tag != 5)goto _LL3BF;}_LL3BE:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);goto _LL3BC;_LL3BF:;_LL3C0:
 return 0;_LL3BC:;}
# 2077
return 1;}
# 2080
int Cyc_Tcutil_is_integral_type(void*t){
void*_tmp626=Cyc_Tcutil_compress(t);void*_tmp627=_tmp626;_LL3C2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp628=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp627;if(_tmp628->tag != 6)goto _LL3C4;}_LL3C3:
 goto _LL3C5;_LL3C4: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp629=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp627;if(_tmp629->tag != 19)goto _LL3C6;}_LL3C5:
 goto _LL3C7;_LL3C6: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp62A=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp627;if(_tmp62A->tag != 13)goto _LL3C8;}_LL3C7:
 goto _LL3C9;_LL3C8: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp62B=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp627;if(_tmp62B->tag != 14)goto _LL3CA;}_LL3C9:
 return 1;_LL3CA:;_LL3CB:
 return 0;_LL3C1:;}
# 2091
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
return 1;
# 2095
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_typ)){
const char*_tmp10E3;void*_tmp10E2;(_tmp10E2=0,Cyc_Tcutil_warn(e->loc,((_tmp10E3="integral size mismatch; conversion supplied",_tag_dyneither(_tmp10E3,sizeof(char),44))),_tag_dyneither(_tmp10E2,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 2101
return 0;}
# 2105
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ))
return 1;
# 2109
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_sint_typ)){
const char*_tmp10E6;void*_tmp10E5;(_tmp10E5=0,Cyc_Tcutil_warn(e->loc,((_tmp10E6="integral size mismatch; conversion supplied",_tag_dyneither(_tmp10E6,sizeof(char),44))),_tag_dyneither(_tmp10E5,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 2115
return 0;}
# 2120
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);
# 2128
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
# 2130
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp10E7;struct _tuple0 _tmp630=(_tmp10E7.f1=t1,((_tmp10E7.f2=t2,_tmp10E7)));struct _tuple0 _tmp631=_tmp630;struct Cyc_Absyn_PtrInfo _tmp633;struct Cyc_Absyn_PtrInfo _tmp635;void*_tmp637;struct Cyc_Absyn_Tqual _tmp638;struct Cyc_Absyn_Exp*_tmp639;union Cyc_Absyn_Constraint*_tmp63A;void*_tmp63C;struct Cyc_Absyn_Tqual _tmp63D;struct Cyc_Absyn_Exp*_tmp63E;union Cyc_Absyn_Constraint*_tmp63F;_LL3CD:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp632=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp631.f1;if(_tmp632->tag != 5)goto _LL3CF;else{_tmp633=_tmp632->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp634=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp631.f2;if(_tmp634->tag != 5)goto _LL3CF;else{_tmp635=_tmp634->f1;}};_LL3CE: {
# 2134
int okay=1;
# 2136
if(!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp633.ptr_atts).nullable,(_tmp635.ptr_atts).nullable))
# 2139
okay=!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp633.ptr_atts).nullable);
# 2141
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp633.ptr_atts).bounds,(_tmp635.ptr_atts).bounds)){
# 2144
struct _tuple0 _tmp10E8;struct _tuple0 _tmp642=(_tmp10E8.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp633.ptr_atts).bounds),((_tmp10E8.f2=
((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp635.ptr_atts).bounds),_tmp10E8)));
# 2144
struct _tuple0 _tmp643=_tmp642;struct Cyc_Absyn_Exp*_tmp649;struct Cyc_Absyn_Exp*_tmp64B;struct Cyc_Absyn_Exp*_tmp64E;_LL3D6:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp644=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp643.f1;if(_tmp644->tag != 1)goto _LL3D8;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp645=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp643.f2;if(_tmp645->tag != 0)goto _LL3D8;};_LL3D7:
# 2146
 goto _LL3D9;_LL3D8:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp646=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp643.f1;if(_tmp646->tag != 0)goto _LL3DA;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp647=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp643.f2;if(_tmp647->tag != 0)goto _LL3DA;};_LL3D9:
 okay=1;goto _LL3D5;_LL3DA:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp648=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp643.f1;if(_tmp648->tag != 1)goto _LL3DC;else{_tmp649=_tmp648->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp64A=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp643.f2;if(_tmp64A->tag != 1)goto _LL3DC;else{_tmp64B=_tmp64A->f1;}};_LL3DB:
# 2149
 okay=okay  && Cyc_Evexp_lte_const_exp(_tmp64B,_tmp649);
# 2153
if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp635.ptr_atts).zero_term)){
const char*_tmp10EB;void*_tmp10EA;(_tmp10EA=0,Cyc_Tcutil_warn(loc,((_tmp10EB="implicit cast to shorter array",_tag_dyneither(_tmp10EB,sizeof(char),31))),_tag_dyneither(_tmp10EA,sizeof(void*),0)));}
goto _LL3D5;_LL3DC:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp64C=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp643.f1;if(_tmp64C->tag != 0)goto _LL3D5;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp64D=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp643.f2;if(_tmp64D->tag != 1)goto _LL3D5;else{_tmp64E=_tmp64D->f1;}};_LL3DD:
# 2158
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp633.ptr_atts).zero_term) && 
Cyc_Tcutil_is_bound_one((_tmp635.ptr_atts).bounds))
goto _LL3D5;
okay=0;
goto _LL3D5;_LL3D5:;}
# 2167
okay=okay  && (!(_tmp633.elt_tq).real_const  || (_tmp635.elt_tq).real_const);
# 2170
if(!Cyc_Tcutil_unify((_tmp633.ptr_atts).rgn,(_tmp635.ptr_atts).rgn)){
if(Cyc_Tcenv_region_outlives(te,(_tmp633.ptr_atts).rgn,(_tmp635.ptr_atts).rgn)){
if(Cyc_Tcutil_warn_region_coerce){
const char*_tmp10F0;void*_tmp10EF[2];struct Cyc_String_pa_PrintArg_struct _tmp10EE;struct Cyc_String_pa_PrintArg_struct _tmp10ED;(_tmp10ED.tag=0,((_tmp10ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2175
Cyc_Absynpp_typ2string((_tmp635.ptr_atts).rgn)),((_tmp10EE.tag=0,((_tmp10EE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2174
Cyc_Absynpp_typ2string((_tmp633.ptr_atts).rgn)),((_tmp10EF[0]=& _tmp10EE,((_tmp10EF[1]=& _tmp10ED,Cyc_Tcutil_warn(loc,((_tmp10F0="implicit cast form region %s to region %s",_tag_dyneither(_tmp10F0,sizeof(char),42))),_tag_dyneither(_tmp10EF,sizeof(void*),2)))))))))))));}}else{
# 2176
okay=0;}}
# 2180
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp633.ptr_atts).zero_term,(_tmp635.ptr_atts).zero_term) || 
# 2183
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,(_tmp633.ptr_atts).zero_term) && (_tmp635.elt_tq).real_const);{
# 2191
int _tmp656=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp635.ptr_atts).bounds,Cyc_Absyn_bounds_one_conref) && !
# 2194
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp635.ptr_atts).zero_term);
# 2198
okay=okay  && (Cyc_Tcutil_unify(_tmp633.elt_typ,_tmp635.elt_typ) || 
(_tmp656  && ((_tmp635.elt_tq).real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp635.elt_typ)))) && Cyc_Tcutil_ptrsubtype(te,0,_tmp633.elt_typ,_tmp635.elt_typ));
# 2201
return okay;};}_LL3CF:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp636=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp631.f1;if(_tmp636->tag != 8)goto _LL3D1;else{_tmp637=(_tmp636->f1).elt_type;_tmp638=(_tmp636->f1).tq;_tmp639=(_tmp636->f1).num_elts;_tmp63A=(_tmp636->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp63B=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp631.f2;if(_tmp63B->tag != 8)goto _LL3D1;else{_tmp63C=(_tmp63B->f1).elt_type;_tmp63D=(_tmp63B->f1).tq;_tmp63E=(_tmp63B->f1).num_elts;_tmp63F=(_tmp63B->f1).zero_term;}};_LL3D0: {
# 2205
int okay;
# 2207
okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp63A,_tmp63F) && (
(_tmp639 != 0  && _tmp63E != 0) && Cyc_Evexp_same_const_exp(_tmp639,_tmp63E));
# 2210
return(okay  && Cyc_Tcutil_unify(_tmp637,_tmp63C)) && (!_tmp638.real_const  || _tmp63D.real_const);}_LL3D1:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp640=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp631.f1;if(_tmp640->tag != 19)goto _LL3D3;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp641=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp631.f2;if(_tmp641->tag != 6)goto _LL3D3;};_LL3D2:
# 2212
 return 0;_LL3D3:;_LL3D4:
# 2214
 return Cyc_Tcutil_unify(t1,t2);_LL3CC:;};}
# 2218
int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp658=Cyc_Tcutil_compress(t);void*_tmp659=_tmp658;_LL3DF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp65A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp659;if(_tmp65A->tag != 5)goto _LL3E1;}_LL3E0:
 return 1;_LL3E1:;_LL3E2:
 return 0;_LL3DE:;}
# 2224
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp65B=Cyc_Tcutil_compress(t);void*_tmp65C=_tmp65B;void*_tmp65E;_LL3E4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp65D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp65C;if(_tmp65D->tag != 5)goto _LL3E6;else{_tmp65E=(_tmp65D->f1).elt_typ;}}_LL3E5:
 return _tmp65E;_LL3E6:;_LL3E7: {
const char*_tmp10F3;void*_tmp10F2;(_tmp10F2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10F3="pointer_elt_type",_tag_dyneither(_tmp10F3,sizeof(char),17))),_tag_dyneither(_tmp10F2,sizeof(void*),0)));}_LL3E3:;}
# 2230
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp661=Cyc_Tcutil_compress(t);void*_tmp662=_tmp661;struct Cyc_Absyn_PtrAtts*_tmp664;_LL3E9: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp663=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp662;if(_tmp663->tag != 5)goto _LL3EB;else{_tmp664=(struct Cyc_Absyn_PtrAtts*)&(_tmp663->f1).ptr_atts;}}_LL3EA:
 return _tmp664->rgn;_LL3EB:;_LL3EC: {
const char*_tmp10F6;void*_tmp10F5;(_tmp10F5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10F6="pointer_elt_type",_tag_dyneither(_tmp10F6,sizeof(char),17))),_tag_dyneither(_tmp10F5,sizeof(void*),0)));}_LL3E8:;}
# 2237
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp667=Cyc_Tcutil_compress(t);void*_tmp668=_tmp667;void*_tmp66A;_LL3EE: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp669=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp668;if(_tmp669->tag != 5)goto _LL3F0;else{_tmp66A=((_tmp669->f1).ptr_atts).rgn;}}_LL3EF:
# 2240
*rgn=_tmp66A;
return 1;_LL3F0:;_LL3F1:
 return 0;_LL3ED:;}
# 2247
int Cyc_Tcutil_is_boxed(void*t){
return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;}
# 2251
int Cyc_Tcutil_is_dyneither_ptr(void*t){
void*_tmp66B=Cyc_Tcutil_compress(t);void*_tmp66C=_tmp66B;union Cyc_Absyn_Constraint*_tmp66E;_LL3F3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp66D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp66C;if(_tmp66D->tag != 5)goto _LL3F5;else{_tmp66E=((_tmp66D->f1).ptr_atts).bounds;}}_LL3F4:
# 2254
 return((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp66E)== (void*)& Cyc_Absyn_DynEither_b_val;_LL3F5:;_LL3F6:
 return 0;_LL3F2:;}
# 2260
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp66F=e->r;void*_tmp670=_tmp66F;struct _dyneither_ptr _tmp676;void*_tmp678;struct Cyc_Absyn_Exp*_tmp679;_LL3F8: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp671=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp670;if(_tmp671->tag != 0)goto _LL3FA;else{if(((_tmp671->f1).Int_c).tag != 5)goto _LL3FA;if(((struct _tuple7)((_tmp671->f1).Int_c).val).f2 != 0)goto _LL3FA;}}_LL3F9:
 goto _LL3FB;_LL3FA: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp672=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp670;if(_tmp672->tag != 0)goto _LL3FC;else{if(((_tmp672->f1).Char_c).tag != 2)goto _LL3FC;if(((struct _tuple5)((_tmp672->f1).Char_c).val).f2 != 0)goto _LL3FC;}}_LL3FB:
 goto _LL3FD;_LL3FC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp673=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp670;if(_tmp673->tag != 0)goto _LL3FE;else{if(((_tmp673->f1).Short_c).tag != 4)goto _LL3FE;if(((struct _tuple6)((_tmp673->f1).Short_c).val).f2 != 0)goto _LL3FE;}}_LL3FD:
 goto _LL3FF;_LL3FE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp674=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp670;if(_tmp674->tag != 0)goto _LL400;else{if(((_tmp674->f1).LongLong_c).tag != 6)goto _LL400;if(((struct _tuple8)((_tmp674->f1).LongLong_c).val).f2 != 0)goto _LL400;}}_LL3FF:
# 2266
 return 1;_LL400: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp675=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp670;if(_tmp675->tag != 0)goto _LL402;else{if(((_tmp675->f1).Wchar_c).tag != 3)goto _LL402;_tmp676=(struct _dyneither_ptr)((_tmp675->f1).Wchar_c).val;}}_LL401: {
# 2268
unsigned long _tmp67A=Cyc_strlen((struct _dyneither_ptr)_tmp676);
int i=0;
if(_tmp67A >= 2  && *((const char*)_check_dyneither_subscript(_tmp676,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp676,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmp676,sizeof(char),1))== 'x'  && _tmp67A >= 3) && *((const char*)_check_dyneither_subscript(_tmp676,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp67A;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp676,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 2278
return 0;}}_LL402: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp677=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp670;if(_tmp677->tag != 13)goto _LL404;else{_tmp678=(void*)_tmp677->f1;_tmp679=_tmp677->f2;}}_LL403:
 return Cyc_Tcutil_is_zero(_tmp679) && Cyc_Tcutil_admits_zero(_tmp678);_LL404:;_LL405:
 return 0;_LL3F7:;}
# 2284
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
# 2291
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 2296
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 2301
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
# 2308
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 2313
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 2318
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
enum Cyc_Absyn_KindQual _tmp67C;enum Cyc_Absyn_AliasQual _tmp67D;struct Cyc_Absyn_Kind*_tmp67B=ka;_tmp67C=_tmp67B->kind;_tmp67D=_tmp67B->aliasqual;
switch(_tmp67D){case Cyc_Absyn_Aliasable: _LL406:
# 2322
 switch(_tmp67C){case Cyc_Absyn_AnyKind: _LL408:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LL409:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL40A:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL40B:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL40C:
 return& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL40D:
 return& Cyc_Tcutil_iko;}case Cyc_Absyn_Unique: _LL407:
# 2331
 switch(_tmp67C){case Cyc_Absyn_AnyKind: _LL410:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL411:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL412:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL413:
 return& Cyc_Tcutil_urko;default: _LL414:
 break;}
# 2338
break;case Cyc_Absyn_Top: _LL40F:
# 2340
 switch(_tmp67C){case Cyc_Absyn_AnyKind: _LL417:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL418:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL419:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL41A:
 return& Cyc_Tcutil_trko;default: _LL41B:
 break;}
# 2347
break;}{
# 2349
const char*_tmp10FA;void*_tmp10F9[1];struct Cyc_String_pa_PrintArg_struct _tmp10F8;(_tmp10F8.tag=0,((_tmp10F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmp10F9[0]=& _tmp10F8,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10FA="kind_to_opt: bad kind %s\n",_tag_dyneither(_tmp10FA,sizeof(char),26))),_tag_dyneither(_tmp10F9,sizeof(void*),1)))))));};}
# 2352
static void**Cyc_Tcutil_kind_to_b(struct Cyc_Absyn_Kind*ka){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ab_v={0,& Cyc_Tcutil_ak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct mb_v={0,& Cyc_Tcutil_mk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct bb_v={0,& Cyc_Tcutil_bk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rb_v={0,& Cyc_Tcutil_rk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct eb_v={0,& Cyc_Tcutil_ek};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ib_v={0,& Cyc_Tcutil_ik};
# 2360
static void*ab=(void*)& ab_v;
static void*mb=(void*)& mb_v;
static void*bb=(void*)& bb_v;
static void*rb=(void*)& rb_v;
static void*eb=(void*)& eb_v;
static void*ib=(void*)& ib_v;
# 2367
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tab_v={0,& Cyc_Tcutil_tak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tmb_v={0,& Cyc_Tcutil_tmk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tbb_v={0,& Cyc_Tcutil_tbk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct trb_v={0,& Cyc_Tcutil_trk};
# 2372
static void*tab=(void*)& tab_v;
static void*tmb=(void*)& tmb_v;
static void*tbb=(void*)& tbb_v;
static void*trb=(void*)& trb_v;
# 2377
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct uab_v={0,& Cyc_Tcutil_uak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct umb_v={0,& Cyc_Tcutil_umk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ubb_v={0,& Cyc_Tcutil_ubk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct urb_v={0,& Cyc_Tcutil_urk};
# 2382
static void*uab=(void*)& uab_v;
static void*umb=(void*)& umb_v;
static void*ubb=(void*)& ubb_v;
static void*urb=(void*)& urb_v;
# 2387
enum Cyc_Absyn_KindQual _tmp682;enum Cyc_Absyn_AliasQual _tmp683;struct Cyc_Absyn_Kind*_tmp681=ka;_tmp682=_tmp681->kind;_tmp683=_tmp681->aliasqual;
switch(_tmp683){case Cyc_Absyn_Aliasable: _LL41D:
# 2390
 switch(_tmp682){case Cyc_Absyn_AnyKind: _LL41F:
 return& ab;case Cyc_Absyn_MemKind: _LL420:
 return& mb;case Cyc_Absyn_BoxKind: _LL421:
 return& bb;case Cyc_Absyn_RgnKind: _LL422:
 return& rb;case Cyc_Absyn_EffKind: _LL423:
 return& eb;case Cyc_Absyn_IntKind: _LL424:
 return& ib;}case Cyc_Absyn_Unique: _LL41E:
# 2399
 switch(_tmp682){case Cyc_Absyn_AnyKind: _LL427:
 return& uab;case Cyc_Absyn_MemKind: _LL428:
 return& umb;case Cyc_Absyn_BoxKind: _LL429:
 return& ubb;case Cyc_Absyn_RgnKind: _LL42A:
 return& urb;default: _LL42B:
 break;}
# 2406
break;case Cyc_Absyn_Top: _LL426:
# 2408
 switch(_tmp682){case Cyc_Absyn_AnyKind: _LL42E:
 return& tab;case Cyc_Absyn_MemKind: _LL42F:
 return& tmb;case Cyc_Absyn_BoxKind: _LL430:
 return& tbb;case Cyc_Absyn_RgnKind: _LL431:
 return& trb;default: _LL432:
 break;}
# 2415
break;}{
# 2417
const char*_tmp10FE;void*_tmp10FD[1];struct Cyc_String_pa_PrintArg_struct _tmp10FC;(_tmp10FC.tag=0,((_tmp10FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmp10FD[0]=& _tmp10FC,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10FE="kind_to_b: bad kind %s\n",_tag_dyneither(_tmp10FE,sizeof(char),24))),_tag_dyneither(_tmp10FD,sizeof(void*),1)))))));};}
# 2420
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return*Cyc_Tcutil_kind_to_b(k);}
# 2423
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
# 2425
struct Cyc_Core_Opt*_tmp10FF;return(_tmp10FF=_cycalloc(sizeof(*_tmp10FF)),((_tmp10FF->v=Cyc_Tcutil_kind_to_bound(k),_tmp10FF)));}
# 2430
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){
{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp1102;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1101;e1->r=(void*)((_tmp1101=_cycalloc(sizeof(*_tmp1101)),((_tmp1101[0]=((_tmp1102.tag=0,((_tmp1102.f1=Cyc_Absyn_Null_c,_tmp1102)))),_tmp1101))));}{
struct Cyc_Core_Opt*_tmp698=Cyc_Tcenv_lookup_opt_type_vars(te);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp1108;struct Cyc_Absyn_PtrInfo _tmp1107;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1106;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp699=(_tmp1106=_cycalloc(sizeof(*_tmp1106)),((_tmp1106[0]=((_tmp1108.tag=5,((_tmp1108.f1=((_tmp1107.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp698),((_tmp1107.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp1107.ptr_atts=(
((_tmp1107.ptr_atts).rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp698),(((_tmp1107.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp1107.ptr_atts).bounds=
# 2438
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp1107.ptr_atts).zero_term=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp1107.ptr_atts).ptrloc=0,_tmp1107.ptr_atts)))))))))),_tmp1107)))))),_tmp1108)))),_tmp1106)));
e1->topt=(void*)_tmp699;{
int bogus=0;
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
if(bogus != 0){
const char*_tmp110D;void*_tmp110C[2];struct Cyc_String_pa_PrintArg_struct _tmp110B;struct Cyc_String_pa_PrintArg_struct _tmp110A;(_tmp110A.tag=0,((_tmp110A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(e1->loc)),((_tmp110B.tag=0,((_tmp110B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp110C[0]=& _tmp110B,((_tmp110C[1]=& _tmp110A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp110D="zero_to_null resulted in an alias coercion on %s at %s\n",_tag_dyneither(_tmp110D,sizeof(char),56))),_tag_dyneither(_tmp110C,sizeof(void*),2)))))))))))));}
return retv;};};}
# 2448
return 0;}
# 2451
struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
switch(c){case Cyc_Absyn_Unknown_coercion: _LL434: {
const char*_tmp110E;return(_tmp110E="unknown",_tag_dyneither(_tmp110E,sizeof(char),8));}case Cyc_Absyn_No_coercion: _LL435: {
const char*_tmp110F;return(_tmp110F="no coercion",_tag_dyneither(_tmp110F,sizeof(char),12));}case Cyc_Absyn_NonNull_to_Null: _LL436: {
const char*_tmp1110;return(_tmp1110="null check",_tag_dyneither(_tmp1110,sizeof(char),11));}case Cyc_Absyn_Other_coercion: _LL437: {
const char*_tmp1111;return(_tmp1111="other coercion",_tag_dyneither(_tmp1111,sizeof(char),15));}}}
# 2460
int Cyc_Tcutil_warn_alias_coerce=0;
# 2466
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0,& Cyc_Tcutil_rk};
# 2470
static int counter=0;
struct _dyneither_ptr*_tmp111E;const char*_tmp111D;void*_tmp111C[1];struct Cyc_Int_pa_PrintArg_struct _tmp111B;struct _tuple2*_tmp111A;struct _tuple2*v=(_tmp111A=_cycalloc(sizeof(*_tmp111A)),((_tmp111A->f1=Cyc_Absyn_Loc_n,((_tmp111A->f2=((_tmp111E=_cycalloc(sizeof(*_tmp111E)),((_tmp111E[0]=(struct _dyneither_ptr)((_tmp111B.tag=1,((_tmp111B.f1=(unsigned long)counter ++,((_tmp111C[0]=& _tmp111B,Cyc_aprintf(((_tmp111D="__aliasvar%d",_tag_dyneither(_tmp111D,sizeof(char),13))),_tag_dyneither(_tmp111C,sizeof(void*),1)))))))),_tmp111E)))),_tmp111A)))));
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(v,e_typ,e);
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp1121;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp1120;struct Cyc_Absyn_Exp*ve=Cyc_Absyn_varb_exp(v,(void*)((_tmp1120=_cycalloc(sizeof(*_tmp1120)),((_tmp1120[0]=((_tmp1121.tag=4,((_tmp1121.f1=vd,_tmp1121)))),_tmp1120)))),e->loc);
# 2479
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 2481
{void*_tmp6A5=Cyc_Tcutil_compress(e_typ);void*_tmp6A6=_tmp6A5;void*_tmp6A8;struct Cyc_Absyn_Tqual _tmp6A9;void*_tmp6AA;union Cyc_Absyn_Constraint*_tmp6AB;union Cyc_Absyn_Constraint*_tmp6AC;union Cyc_Absyn_Constraint*_tmp6AD;struct Cyc_Absyn_PtrLoc*_tmp6AE;_LL43A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6A7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6A6;if(_tmp6A7->tag != 5)goto _LL43C;else{_tmp6A8=(_tmp6A7->f1).elt_typ;_tmp6A9=(_tmp6A7->f1).elt_tq;_tmp6AA=((_tmp6A7->f1).ptr_atts).rgn;_tmp6AB=((_tmp6A7->f1).ptr_atts).nullable;_tmp6AC=((_tmp6A7->f1).ptr_atts).bounds;_tmp6AD=((_tmp6A7->f1).ptr_atts).zero_term;_tmp6AE=((_tmp6A7->f1).ptr_atts).ptrloc;}}_LL43B:
# 2483
{void*_tmp6AF=Cyc_Tcutil_compress(_tmp6AA);void*_tmp6B0=_tmp6AF;void**_tmp6B2;struct Cyc_Core_Opt*_tmp6B3;_LL43F: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp6B1=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp6B0;if(_tmp6B1->tag != 1)goto _LL441;else{_tmp6B2=(void**)((void**)& _tmp6B1->f2);_tmp6B3=_tmp6B1->f4;}}_LL440: {
# 2485
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1124;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1123;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp6B4=(_tmp1123=_cycalloc(sizeof(*_tmp1123)),((_tmp1123[0]=((_tmp1124.tag=2,((_tmp1124.f1=tv,_tmp1124)))),_tmp1123)));
*_tmp6B2=(void*)_tmp6B4;
goto _LL43E;}_LL441:;_LL442:
 goto _LL43E;_LL43E:;}
# 2490
goto _LL439;_LL43C:;_LL43D:
 goto _LL439;_LL439:;}
# 2494
e->topt=0;
vd->initializer=0;{
# 2498
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 2500
struct _tuple15 _tmp1125;return(_tmp1125.f1=d,((_tmp1125.f2=ve,_tmp1125)));};}
# 2505
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ,void*wants_typ,unsigned int loc){
# 2508
struct _RegionHandle _tmp6C0=_new_region("r");struct _RegionHandle*r=& _tmp6C0;_push_region(r);
if((Cyc_Tcutil_is_noalias_path(r,e) && Cyc_Tcutil_is_noalias_pointer(e_typ,0)) && 
Cyc_Tcutil_is_pointer_type(e_typ)){
# 2513
void*_tmp6C1=Cyc_Tcutil_compress(wants_typ);void*_tmp6C2=_tmp6C1;void*_tmp6C4;_LL444: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6C3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6C2;if(_tmp6C3->tag != 5)goto _LL446;else{_tmp6C4=((_tmp6C3->f1).ptr_atts).rgn;}}_LL445: {
# 2515
void*_tmp6C6=Cyc_Tcutil_compress(_tmp6C4);void*_tmp6C7=_tmp6C6;_LL44B: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp6C8=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp6C7;if(_tmp6C8->tag != 20)goto _LL44D;}_LL44C: {
int _tmp6C9=0;_npop_handler(0);return _tmp6C9;}_LL44D:;_LL44E: {
# 2518
struct Cyc_Absyn_Kind*_tmp6CA=Cyc_Tcutil_typ_kind(_tmp6C4);
int _tmp6CB=_tmp6CA->kind == Cyc_Absyn_RgnKind  && _tmp6CA->aliasqual == Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp6CB;}_LL44A:;}_LL446: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp6C5=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6C2;if(_tmp6C5->tag != 17)goto _LL448;}_LL447: {
# 2522
const char*_tmp112A;void*_tmp1129[2];struct Cyc_String_pa_PrintArg_struct _tmp1128;struct Cyc_String_pa_PrintArg_struct _tmp1127;(_tmp1127.tag=0,((_tmp1127.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(loc)),((_tmp1128.tag=0,((_tmp1128.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_compress(wants_typ))),((_tmp1129[0]=& _tmp1128,((_tmp1129[1]=& _tmp1127,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp112A="got typedef %s in can_insert_alias at %s\n",_tag_dyneither(_tmp112A,sizeof(char),42))),_tag_dyneither(_tmp1129,sizeof(void*),2)))))))))))));}_LL448:;_LL449: {
int _tmp6D0=0;_npop_handler(0);return _tmp6D0;}_LL443:;}{
# 2527
int _tmp6D1=0;_npop_handler(0);return _tmp6D1;};
# 2509
;_pop_region(r);}
# 2531
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 2536
if(Cyc_Tcutil_unify(t1,t2))return 1;
# 2538
if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
# 2540
if(Cyc_Tcutil_will_lose_precision(t1,t2)){
const char*_tmp112F;void*_tmp112E[2];struct Cyc_String_pa_PrintArg_struct _tmp112D;struct Cyc_String_pa_PrintArg_struct _tmp112C;(_tmp112C.tag=0,((_tmp112C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp112D.tag=0,((_tmp112D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp112E[0]=& _tmp112D,((_tmp112E[1]=& _tmp112C,Cyc_Tcutil_warn(e->loc,((_tmp112F="integral size mismatch; %s -> %s conversion supplied",_tag_dyneither(_tmp112F,sizeof(char),53))),_tag_dyneither(_tmp112E,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
return 1;}else{
# 2549
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce){
const char*_tmp1135;void*_tmp1134[3];struct Cyc_String_pa_PrintArg_struct _tmp1133;struct Cyc_String_pa_PrintArg_struct _tmp1132;struct Cyc_String_pa_PrintArg_struct _tmp1131;(_tmp1131.tag=0,((_tmp1131.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp1132.tag=0,((_tmp1132.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp1133.tag=0,((_tmp1133.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp1134[0]=& _tmp1133,((_tmp1134[1]=& _tmp1132,((_tmp1134[2]=& _tmp1131,Cyc_Tcutil_warn(e->loc,((_tmp1135="implicit alias coercion for %s:%s to %s",_tag_dyneither(_tmp1135,sizeof(char),40))),_tag_dyneither(_tmp1134,sizeof(void*),3)))))))))))))))))));}
*alias_coercion=1;}
# 2556
if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);
return 1;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e))
return 1;else{
if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= Cyc_Absyn_Unknown_coercion){
# 2564
if(c != Cyc_Absyn_No_coercion)Cyc_Tcutil_unchecked_cast(te,e,t2,c);
if(c != Cyc_Absyn_NonNull_to_Null){
const char*_tmp113A;void*_tmp1139[2];struct Cyc_String_pa_PrintArg_struct _tmp1138;struct Cyc_String_pa_PrintArg_struct _tmp1137;(_tmp1137.tag=0,((_tmp1137.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp1138.tag=0,((_tmp1138.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp1139[0]=& _tmp1138,((_tmp1139[1]=& _tmp1137,Cyc_Tcutil_warn(e->loc,((_tmp113A="implicit cast from %s to %s",_tag_dyneither(_tmp113A,sizeof(char),28))),_tag_dyneither(_tmp1139,sizeof(void*),2)))))))))))));}
return 1;}else{
# 2570
return 0;}}}}}
# 2577
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){
# 2580
int bogus=0;
return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}
# 2584
int Cyc_Tcutil_coerceable(void*t){
void*_tmp6DF=Cyc_Tcutil_compress(t);void*_tmp6E0=_tmp6DF;_LL450: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6E1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6E0;if(_tmp6E1->tag != 6)goto _LL452;}_LL451:
 goto _LL453;_LL452: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp6E2=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp6E0;if(_tmp6E2->tag != 7)goto _LL454;}_LL453:
 return 1;_LL454:;_LL455:
 return 0;_LL44F:;}
# 2602 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 2606
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ_f(struct _tuple23*env,struct Cyc_Absyn_Aggrfield*x){
# 2609
struct _tuple23 _tmp6E3=*env;struct Cyc_List_List*_tmp6E5;struct _RegionHandle*_tmp6E6;struct Cyc_Tcenv_Tenv*_tmp6E7;int _tmp6E8;struct _tuple23 _tmp6E4=_tmp6E3;_tmp6E5=_tmp6E4.f1;_tmp6E6=_tmp6E4.f2;_tmp6E7=_tmp6E4.f3;_tmp6E8=_tmp6E4.f4;{
# 2611
void*_tmp6E9=_tmp6E5 == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp6E6,_tmp6E5,x->type);
struct Cyc_List_List*_tmp6EA=Cyc_Tcutil_flatten_typ(_tmp6E6,_tmp6E8,_tmp6E7,_tmp6E9);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6EA)== 1){
struct _tuple12*_tmp113D;struct Cyc_List_List*_tmp113C;return(_tmp113C=_region_malloc(_tmp6E6,sizeof(*_tmp113C)),((_tmp113C->hd=((_tmp113D=_region_malloc(_tmp6E6,sizeof(*_tmp113D)),((_tmp113D->f1=x->tq,((_tmp113D->f2=_tmp6E9,_tmp113D)))))),((_tmp113C->tl=0,_tmp113C)))));}else{
return _tmp6EA;}};}struct _tuple24{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2617
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple24*env,struct _tuple12*x){
# 2619
struct _tuple24 _tmp6ED=*env;struct _RegionHandle*_tmp6EF;struct Cyc_Tcenv_Tenv*_tmp6F0;int _tmp6F1;struct _tuple24 _tmp6EE=_tmp6ED;_tmp6EF=_tmp6EE.f1;_tmp6F0=_tmp6EE.f2;_tmp6F1=_tmp6EE.f3;{
struct _tuple12 _tmp6F2=*x;struct Cyc_Absyn_Tqual _tmp6F4;void*_tmp6F5;struct _tuple12 _tmp6F3=_tmp6F2;_tmp6F4=_tmp6F3.f1;_tmp6F5=_tmp6F3.f2;{
struct Cyc_List_List*_tmp6F6=Cyc_Tcutil_flatten_typ(_tmp6EF,_tmp6F1,_tmp6F0,_tmp6F5);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6F6)== 1){
struct _tuple12*_tmp1140;struct Cyc_List_List*_tmp113F;return(_tmp113F=_region_malloc(_tmp6EF,sizeof(*_tmp113F)),((_tmp113F->hd=((_tmp1140=_region_malloc(_tmp6EF,sizeof(*_tmp1140)),((_tmp1140->f1=_tmp6F4,((_tmp1140->f2=_tmp6F5,_tmp1140)))))),((_tmp113F->tl=0,_tmp113F)))));}else{
return _tmp6F6;}};};}
# 2626
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2630
if(flatten){
t1=Cyc_Tcutil_compress(t1);{
void*_tmp6F9=t1;struct Cyc_List_List*_tmp6FC;struct Cyc_Absyn_Aggrdecl*_tmp6FE;struct Cyc_List_List*_tmp6FF;struct Cyc_List_List*_tmp701;_LL457: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp6FA=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp6F9;if(_tmp6FA->tag != 0)goto _LL459;}_LL458:
 return 0;_LL459: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6FB=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6F9;if(_tmp6FB->tag != 10)goto _LL45B;else{_tmp6FC=_tmp6FB->f1;}}_LL45A: {
# 2635
struct _tuple24 _tmp1141;struct _tuple24 _tmp702=(_tmp1141.f1=r,((_tmp1141.f2=te,((_tmp1141.f3=flatten,_tmp1141)))));
# 2637
struct Cyc_List_List*_tmp703=_tmp6FC;struct _tuple12*_tmp704;struct Cyc_List_List*_tmp705;_LL462: if(_tmp703 != 0)goto _LL464;_LL463:
 return 0;_LL464: if(_tmp703 == 0)goto _LL461;_tmp704=(struct _tuple12*)_tmp703->hd;_tmp705=_tmp703->tl;_LL465: {
# 2640
struct Cyc_List_List*_tmp706=Cyc_Tcutil_rcopy_tqt(& _tmp702,_tmp704);
_tmp702.f3=0;{
struct Cyc_List_List*_tmp707=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple12*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp702,_tmp6FC);
struct Cyc_List_List*_tmp1142;struct Cyc_List_List*_tmp708=(_tmp1142=_region_malloc(r,sizeof(*_tmp1142)),((_tmp1142->hd=_tmp706,((_tmp1142->tl=_tmp707,_tmp1142)))));
struct Cyc_List_List*_tmp1143;return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,((_tmp1143=_region_malloc(r,sizeof(*_tmp1143)),((_tmp1143->hd=_tmp706,((_tmp1143->tl=_tmp707,_tmp1143)))))));};}_LL461:;}_LL45B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp6FD=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp6F9;if(_tmp6FD->tag != 11)goto _LL45D;else{if((((_tmp6FD->f1).aggr_info).KnownAggr).tag != 2)goto _LL45D;_tmp6FE=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp6FD->f1).aggr_info).KnownAggr).val);_tmp6FF=(_tmp6FD->f1).targs;}}_LL45C:
# 2648
 if(((_tmp6FE->kind == Cyc_Absyn_UnionA  || _tmp6FE->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6FE->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6FE->impl))->rgn_po != 0){
# 2650
struct _tuple12*_tmp1146;struct Cyc_List_List*_tmp1145;return(_tmp1145=_region_malloc(r,sizeof(*_tmp1145)),((_tmp1145->hd=((_tmp1146=_region_malloc(r,sizeof(*_tmp1146)),((_tmp1146->f1=Cyc_Absyn_empty_tqual(0),((_tmp1146->f2=t1,_tmp1146)))))),((_tmp1145->tl=0,_tmp1145)))));}{
struct Cyc_List_List*_tmp70E=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp6FE->tvs,_tmp6FF);
struct _tuple23 _tmp1147;struct _tuple23 env=(_tmp1147.f1=_tmp70E,((_tmp1147.f2=r,((_tmp1147.f3=te,((_tmp1147.f4=flatten,_tmp1147)))))));
struct Cyc_List_List*_tmp70F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6FE->impl))->fields;struct Cyc_List_List*_tmp710=_tmp70F;struct Cyc_Absyn_Aggrfield*_tmp711;struct Cyc_List_List*_tmp712;_LL467: if(_tmp710 != 0)goto _LL469;_LL468:
 return 0;_LL469: if(_tmp710 == 0)goto _LL466;_tmp711=(struct Cyc_Absyn_Aggrfield*)_tmp710->hd;_tmp712=_tmp710->tl;_LL46A: {
# 2656
struct Cyc_List_List*_tmp713=Cyc_Tcutil_flatten_typ_f(& env,_tmp711);
env.f4=0;{
struct Cyc_List_List*_tmp714=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp712);
struct Cyc_List_List*_tmp1148;struct Cyc_List_List*_tmp715=(_tmp1148=_region_malloc(r,sizeof(*_tmp1148)),((_tmp1148->hd=_tmp713,((_tmp1148->tl=_tmp714,_tmp1148)))));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp715);};}_LL466:;};_LL45D: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp700=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6F9;if(_tmp700->tag != 12)goto _LL45F;else{if(_tmp700->f1 != Cyc_Absyn_StructA)goto _LL45F;_tmp701=_tmp700->f2;}}_LL45E: {
# 2663
struct _tuple23 _tmp1149;struct _tuple23 env=(_tmp1149.f1=0,((_tmp1149.f2=r,((_tmp1149.f3=te,((_tmp1149.f4=flatten,_tmp1149)))))));
struct Cyc_List_List*_tmp718=_tmp701;struct Cyc_Absyn_Aggrfield*_tmp719;struct Cyc_List_List*_tmp71A;_LL46C: if(_tmp718 != 0)goto _LL46E;_LL46D:
 return 0;_LL46E: if(_tmp718 == 0)goto _LL46B;_tmp719=(struct Cyc_Absyn_Aggrfield*)_tmp718->hd;_tmp71A=_tmp718->tl;_LL46F: {
# 2667
struct Cyc_List_List*_tmp71B=Cyc_Tcutil_flatten_typ_f(& env,_tmp719);
env.f4=0;{
struct Cyc_List_List*_tmp71C=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp71A);
struct Cyc_List_List*_tmp114A;struct Cyc_List_List*_tmp71D=(_tmp114A=_region_malloc(r,sizeof(*_tmp114A)),((_tmp114A->hd=_tmp71B,((_tmp114A->tl=_tmp71C,_tmp114A)))));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp71D);};}_LL46B:;}_LL45F:;_LL460:
# 2673
 goto _LL456;_LL456:;};}{
# 2676
struct _tuple12*_tmp114D;struct Cyc_List_List*_tmp114C;return(_tmp114C=_region_malloc(r,sizeof(*_tmp114C)),((_tmp114C->hd=((_tmp114D=_region_malloc(r,sizeof(*_tmp114D)),((_tmp114D->f1=Cyc_Absyn_empty_tqual(0),((_tmp114D->f2=t1,_tmp114D)))))),((_tmp114C->tl=0,_tmp114C)))));};}
# 2680
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp722=(void*)t->hd;void*_tmp723=_tmp722;_LL471: {struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*_tmp724=(struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*)_tmp723;if(_tmp724->tag != 23)goto _LL473;}_LL472:
 goto _LL474;_LL473: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp725=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp723;if(_tmp725->tag != 4)goto _LL475;}_LL474:
 goto _LL476;_LL475: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp726=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp723;if(_tmp726->tag != 20)goto _LL477;}_LL476:
# 2686
 continue;_LL477:;_LL478:
# 2688
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;_LL470:;}}
# 2691
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}
# 2694
return 1;}
# 2697
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp114E;struct _tuple0 _tmp727=(_tmp114E.f1=Cyc_Tcutil_compress(t1),((_tmp114E.f2=Cyc_Tcutil_compress(t2),_tmp114E)));struct _tuple0 _tmp728=_tmp727;enum Cyc_Absyn_Size_of _tmp72A;enum Cyc_Absyn_Size_of _tmp72C;_LL47A:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp729=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp728.f1;if(_tmp729->tag != 6)goto _LL47C;else{_tmp72A=_tmp729->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp72B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp728.f2;if(_tmp72B->tag != 6)goto _LL47C;else{_tmp72C=_tmp72B->f2;}};_LL47B:
# 2700
 return(_tmp72A == _tmp72C  || _tmp72A == Cyc_Absyn_Int_sz  && _tmp72C == Cyc_Absyn_Long_sz) || 
_tmp72A == Cyc_Absyn_Long_sz  && _tmp72C == Cyc_Absyn_Int_sz;_LL47C:;_LL47D:
 return 0;_LL479:;}
# 2708
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2711
if(Cyc_Tcutil_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}
# 2716
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp114F;struct _tuple0 _tmp72E=(_tmp114F.f1=t1,((_tmp114F.f2=t2,_tmp114F)));struct _tuple0 _tmp72F=_tmp72E;void*_tmp731;struct Cyc_Absyn_Tqual _tmp732;void*_tmp733;union Cyc_Absyn_Constraint*_tmp734;union Cyc_Absyn_Constraint*_tmp735;union Cyc_Absyn_Constraint*_tmp736;void*_tmp738;struct Cyc_Absyn_Tqual _tmp739;void*_tmp73A;union Cyc_Absyn_Constraint*_tmp73B;union Cyc_Absyn_Constraint*_tmp73C;union Cyc_Absyn_Constraint*_tmp73D;struct Cyc_Absyn_Datatypedecl*_tmp73F;struct Cyc_Absyn_Datatypefield*_tmp740;struct Cyc_List_List*_tmp741;struct Cyc_Absyn_Datatypedecl*_tmp743;struct Cyc_List_List*_tmp744;struct Cyc_Absyn_FnInfo _tmp746;struct Cyc_Absyn_FnInfo _tmp748;_LL47F:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp730=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp72F.f1;if(_tmp730->tag != 5)goto _LL481;else{_tmp731=(_tmp730->f1).elt_typ;_tmp732=(_tmp730->f1).elt_tq;_tmp733=((_tmp730->f1).ptr_atts).rgn;_tmp734=((_tmp730->f1).ptr_atts).nullable;_tmp735=((_tmp730->f1).ptr_atts).bounds;_tmp736=((_tmp730->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp737=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp72F.f2;if(_tmp737->tag != 5)goto _LL481;else{_tmp738=(_tmp737->f1).elt_typ;_tmp739=(_tmp737->f1).elt_tq;_tmp73A=((_tmp737->f1).ptr_atts).rgn;_tmp73B=((_tmp737->f1).ptr_atts).nullable;_tmp73C=((_tmp737->f1).ptr_atts).bounds;_tmp73D=((_tmp737->f1).ptr_atts).zero_term;}};_LL480:
# 2724
 if(_tmp732.real_const  && !_tmp739.real_const)
return 0;
# 2727
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp734,_tmp73B) && 
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp734)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp73B))
return 0;
# 2731
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp736,_tmp73D) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp736)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp73D))
return 0;
# 2735
if((!Cyc_Tcutil_unify(_tmp733,_tmp73A) && !Cyc_Tcenv_region_outlives(te,_tmp733,_tmp73A)) && !
Cyc_Tcutil_subtype(te,assume,_tmp733,_tmp73A))
return 0;
# 2739
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp735,_tmp73C)){
struct _tuple0 _tmp1150;struct _tuple0 _tmp749=(_tmp1150.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp735),((_tmp1150.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp73C),_tmp1150)));struct _tuple0 _tmp74A=_tmp749;struct Cyc_Absyn_Exp*_tmp74E;struct Cyc_Absyn_Exp*_tmp750;_LL488:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp74B=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp74A.f1;if(_tmp74B->tag != 0)goto _LL48A;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp74C=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp74A.f2;if(_tmp74C->tag != 0)goto _LL48A;};_LL489:
 goto _LL487;_LL48A:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp74D=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp74A.f1;if(_tmp74D->tag != 1)goto _LL48C;else{_tmp74E=_tmp74D->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp74F=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp74A.f2;if(_tmp74F->tag != 1)goto _LL48C;else{_tmp750=_tmp74F->f1;}};_LL48B:
# 2744
 if(!Cyc_Evexp_lte_const_exp(_tmp750,_tmp74E))
return 0;
goto _LL487;_LL48C:;_LL48D:
 return 0;_LL487:;}
# 2752
if(!_tmp739.real_const  && _tmp732.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp738)))
return 0;}{
# 2758
int _tmp752=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp73C,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp73D);
# 2764
struct _tuple0*_tmp1153;struct Cyc_List_List*_tmp1152;return(_tmp752  && Cyc_Tcutil_ptrsubtype(te,((_tmp1152=_cycalloc(sizeof(*_tmp1152)),((_tmp1152->hd=((_tmp1153=_cycalloc(sizeof(*_tmp1153)),((_tmp1153->f1=t1,((_tmp1153->f2=t2,_tmp1153)))))),((_tmp1152->tl=assume,_tmp1152)))))),_tmp731,_tmp738) || Cyc_Tcutil_unify(_tmp731,_tmp738)) || Cyc_Tcutil_isomorphic(_tmp731,_tmp738);};_LL481:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp73E=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp72F.f1;if(_tmp73E->tag != 4)goto _LL483;else{if((((_tmp73E->f1).field_info).KnownDatatypefield).tag != 2)goto _LL483;_tmp73F=((struct _tuple3)(((_tmp73E->f1).field_info).KnownDatatypefield).val).f1;_tmp740=((struct _tuple3)(((_tmp73E->f1).field_info).KnownDatatypefield).val).f2;_tmp741=(_tmp73E->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp742=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp72F.f2;if(_tmp742->tag != 3)goto _LL483;else{if((((_tmp742->f1).datatype_info).KnownDatatype).tag != 2)goto _LL483;_tmp743=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp742->f1).datatype_info).KnownDatatype).val);_tmp744=(_tmp742->f1).targs;}};_LL482:
# 2770
 if(_tmp73F != _tmp743  && Cyc_Absyn_qvar_cmp(_tmp73F->name,_tmp743->name)!= 0)return 0;
# 2772
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp741)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp744))return 0;
for(0;_tmp741 != 0;(_tmp741=_tmp741->tl,_tmp744=_tmp744->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp741->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp744))->hd))return 0;}
return 1;_LL483:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp745=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72F.f1;if(_tmp745->tag != 9)goto _LL485;else{_tmp746=_tmp745->f1;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp747=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72F.f2;if(_tmp747->tag != 9)goto _LL485;else{_tmp748=_tmp747->f1;}};_LL484:
# 2779
 if(_tmp746.tvars != 0  || _tmp748.tvars != 0){
struct Cyc_List_List*_tmp755=_tmp746.tvars;
struct Cyc_List_List*_tmp756=_tmp748.tvars;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp755)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp756))return 0;{
struct _RegionHandle _tmp757=_new_region("r");struct _RegionHandle*r=& _tmp757;_push_region(r);
{struct Cyc_List_List*inst=0;
while(_tmp755 != 0){
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp755->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp756))->hd)->kind)){int _tmp758=0;_npop_handler(0);return _tmp758;}
{struct _tuple16*_tmp115D;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp115C;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp115B;struct Cyc_List_List*_tmp115A;inst=((_tmp115A=_region_malloc(r,sizeof(*_tmp115A)),((_tmp115A->hd=((_tmp115D=_region_malloc(r,sizeof(*_tmp115D)),((_tmp115D->f1=(struct Cyc_Absyn_Tvar*)_tmp756->hd,((_tmp115D->f2=(void*)((_tmp115B=_cycalloc(sizeof(*_tmp115B)),((_tmp115B[0]=((_tmp115C.tag=2,((_tmp115C.f1=(struct Cyc_Absyn_Tvar*)_tmp755->hd,_tmp115C)))),_tmp115B)))),_tmp115D)))))),((_tmp115A->tl=inst,_tmp115A))))));}
_tmp755=_tmp755->tl;
_tmp756=_tmp756->tl;}
# 2791
if(inst != 0){
_tmp746.tvars=0;
_tmp748.tvars=0;{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1163;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1162;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1160;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp115F;int _tmp761=Cyc_Tcutil_subtype(te,assume,(void*)((_tmp115F=_cycalloc(sizeof(*_tmp115F)),((_tmp115F[0]=((_tmp1160.tag=9,((_tmp1160.f1=_tmp746,_tmp1160)))),_tmp115F)))),(void*)((_tmp1162=_cycalloc(sizeof(*_tmp1162)),((_tmp1162[0]=((_tmp1163.tag=9,((_tmp1163.f1=_tmp748,_tmp1163)))),_tmp1162)))));_npop_handler(0);return _tmp761;};}}
# 2784
;_pop_region(r);};}
# 2799
if(!Cyc_Tcutil_subtype(te,assume,_tmp746.ret_typ,_tmp748.ret_typ))return 0;{
struct Cyc_List_List*_tmp762=_tmp746.args;
struct Cyc_List_List*_tmp763=_tmp748.args;
# 2804
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp762)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp763))return 0;
# 2806
for(0;_tmp762 != 0;(_tmp762=_tmp762->tl,_tmp763=_tmp763->tl)){
struct _tuple10 _tmp764=*((struct _tuple10*)_tmp762->hd);struct Cyc_Absyn_Tqual _tmp766;void*_tmp767;struct _tuple10 _tmp765=_tmp764;_tmp766=_tmp765.f2;_tmp767=_tmp765.f3;{
struct _tuple10 _tmp768=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp763))->hd);struct Cyc_Absyn_Tqual _tmp76A;void*_tmp76B;struct _tuple10 _tmp769=_tmp768;_tmp76A=_tmp769.f2;_tmp76B=_tmp769.f3;
# 2810
if(_tmp76A.real_const  && !_tmp766.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp76B,_tmp767))
return 0;};}
# 2814
if(_tmp746.c_varargs != _tmp748.c_varargs)return 0;
if(_tmp746.cyc_varargs != 0  && _tmp748.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp76C=*_tmp746.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp76D=*_tmp748.cyc_varargs;
# 2819
if((_tmp76D.tq).real_const  && !(_tmp76C.tq).real_const  || !
Cyc_Tcutil_subtype(te,assume,_tmp76D.type,_tmp76C.type))
return 0;}else{
if(_tmp746.cyc_varargs != 0  || _tmp748.cyc_varargs != 0)return 0;}
# 2824
if(!Cyc_Tcutil_subset_effect(1,(void*)_check_null(_tmp746.effect),(void*)_check_null(_tmp748.effect)))return 0;
# 2826
if(!Cyc_Tcutil_sub_rgnpo(_tmp746.rgn_po,_tmp748.rgn_po))return 0;
# 2828
if(!Cyc_Tcutil_sub_attributes(_tmp746.attributes,_tmp748.attributes))return 0;
# 2830
if(!Cyc_Tcutil_check_logical_implication(_tmp748.requires_relns,_tmp746.requires_relns))
return 0;
# 2833
if(!Cyc_Tcutil_check_logical_implication(_tmp746.ensures_relns,_tmp748.ensures_relns))
return 0;
# 2836
return 1;};_LL485:;_LL486:
 return 0;_LL47E:;};}
# 2848 "tcutil.cyc"
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2850
struct _RegionHandle _tmp76F=_new_region("temp");struct _RegionHandle*temp=& _tmp76F;_push_region(temp);
{struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(temp,1,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(temp,1,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0){int _tmp770=0;_npop_handler(0);return _tmp770;}{
struct _tuple12*_tmp771=(struct _tuple12*)tqs1->hd;struct Cyc_Absyn_Tqual _tmp773;void*_tmp774;struct _tuple12*_tmp772=_tmp771;_tmp773=_tmp772->f1;_tmp774=_tmp772->f2;{
struct _tuple12*_tmp775=(struct _tuple12*)tqs2->hd;struct Cyc_Absyn_Tqual _tmp777;void*_tmp778;struct _tuple12*_tmp776=_tmp775;_tmp777=_tmp776->f1;_tmp778=_tmp776->f2;
# 2858
if(_tmp773.real_const  && !_tmp777.real_const){int _tmp779=0;_npop_handler(0);return _tmp779;}
# 2860
if((_tmp777.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp778))) && 
Cyc_Tcutil_subtype(te,assume,_tmp774,_tmp778))
# 2863
continue;
# 2865
if(Cyc_Tcutil_unify(_tmp774,_tmp778))
# 2867
continue;
# 2869
if(Cyc_Tcutil_isomorphic(_tmp774,_tmp778))
# 2871
continue;{
# 2874
int _tmp77A=0;_npop_handler(0);return _tmp77A;};};};}{
# 2876
int _tmp77B=1;_npop_handler(0);return _tmp77B;};}
# 2851
;_pop_region(temp);}
# 2880
static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp77C=Cyc_Tcutil_compress(t);void*_tmp77D=_tmp77C;_LL48F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp77E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp77D;if(_tmp77E->tag != 6)goto _LL491;else{if(_tmp77E->f2 != Cyc_Absyn_Char_sz)goto _LL491;}}_LL490:
 return 1;_LL491:;_LL492:
 return 0;_LL48E:;}
# 2889
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
# 2895
if(t2 == (void*)& Cyc_Absyn_VoidType_val)
return Cyc_Absyn_No_coercion;
{void*_tmp77F=t2;_LL494: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp780=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp77F;if(_tmp780->tag != 6)goto _LL496;else{if(_tmp780->f2 != Cyc_Absyn_Int_sz)goto _LL496;}}_LL495:
# 2899
 goto _LL497;_LL496: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp781=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp77F;if(_tmp781->tag != 6)goto _LL498;else{if(_tmp781->f2 != Cyc_Absyn_Long_sz)goto _LL498;}}_LL497:
# 2901
 if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL493;_LL498:;_LL499:
 goto _LL493;_LL493:;}{
# 2905
void*_tmp782=t1;void*_tmp784;struct Cyc_Absyn_Tqual _tmp785;void*_tmp786;union Cyc_Absyn_Constraint*_tmp787;union Cyc_Absyn_Constraint*_tmp788;union Cyc_Absyn_Constraint*_tmp789;void*_tmp78B;struct Cyc_Absyn_Tqual _tmp78C;struct Cyc_Absyn_Exp*_tmp78D;union Cyc_Absyn_Constraint*_tmp78E;struct Cyc_Absyn_Enumdecl*_tmp790;void*_tmp794;_LL49B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp783=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp782;if(_tmp783->tag != 5)goto _LL49D;else{_tmp784=(_tmp783->f1).elt_typ;_tmp785=(_tmp783->f1).elt_tq;_tmp786=((_tmp783->f1).ptr_atts).rgn;_tmp787=((_tmp783->f1).ptr_atts).nullable;_tmp788=((_tmp783->f1).ptr_atts).bounds;_tmp789=((_tmp783->f1).ptr_atts).zero_term;}}_LL49C:
# 2913
{void*_tmp795=t2;void*_tmp797;struct Cyc_Absyn_Tqual _tmp798;void*_tmp799;union Cyc_Absyn_Constraint*_tmp79A;union Cyc_Absyn_Constraint*_tmp79B;union Cyc_Absyn_Constraint*_tmp79C;_LL4AA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp796=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp795;if(_tmp796->tag != 5)goto _LL4AC;else{_tmp797=(_tmp796->f1).elt_typ;_tmp798=(_tmp796->f1).elt_tq;_tmp799=((_tmp796->f1).ptr_atts).rgn;_tmp79A=((_tmp796->f1).ptr_atts).nullable;_tmp79B=((_tmp796->f1).ptr_atts).bounds;_tmp79C=((_tmp796->f1).ptr_atts).zero_term;}}_LL4AB: {
# 2917
enum Cyc_Absyn_Coercion coercion=Cyc_Absyn_Other_coercion;
struct _tuple0*_tmp1166;struct Cyc_List_List*_tmp1165;struct Cyc_List_List*_tmp79D=(_tmp1165=_cycalloc(sizeof(*_tmp1165)),((_tmp1165->hd=((_tmp1166=_cycalloc(sizeof(*_tmp1166)),((_tmp1166->f1=t1,((_tmp1166->f2=t2,_tmp1166)))))),((_tmp1165->tl=0,_tmp1165)))));
int _tmp79E=_tmp798.real_const  || !_tmp785.real_const;
# 2931 "tcutil.cyc"
int _tmp79F=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp79B,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp79C);
# 2935
int _tmp7A0=_tmp79E  && (
((_tmp79F  && Cyc_Tcutil_ptrsubtype(te,_tmp79D,_tmp784,_tmp797) || 
Cyc_Tcutil_unify(_tmp784,_tmp797)) || Cyc_Tcutil_isomorphic(_tmp784,_tmp797)) || Cyc_Tcutil_unify(_tmp797,(void*)& Cyc_Absyn_VoidType_val));
# 2939
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp789,_tmp79C) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp79C);
# 2943
int _tmp7A1=_tmp7A0?0:((Cyc_Tcutil_bits_only(_tmp784) && Cyc_Tcutil_is_char_type(_tmp797)) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp79C)) && (
_tmp798.real_const  || !_tmp785.real_const);
int bounds_ok=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp788,_tmp79B);
if(!bounds_ok  && !_tmp7A1){
struct _tuple0 _tmp1167;struct _tuple0 _tmp7A2=(_tmp1167.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp788),((_tmp1167.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp79B),_tmp1167)));struct _tuple0 _tmp7A3=_tmp7A2;struct Cyc_Absyn_Exp*_tmp7A5;struct Cyc_Absyn_Exp*_tmp7A7;_LL4AF:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp7A4=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp7A3.f1;if(_tmp7A4->tag != 1)goto _LL4B1;else{_tmp7A5=_tmp7A4->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp7A6=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp7A3.f2;if(_tmp7A6->tag != 1)goto _LL4B1;else{_tmp7A7=_tmp7A6->f1;}};_LL4B0:
# 2953
 if(Cyc_Evexp_lte_const_exp(_tmp7A7,_tmp7A5))
bounds_ok=1;
goto _LL4AE;_LL4B1:;_LL4B2:
# 2958
 bounds_ok=1;goto _LL4AE;_LL4AE:;}
# 2960
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp787) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp79A))
coercion=Cyc_Absyn_NonNull_to_Null;
# 2965
if(((bounds_ok  && zeroterm_ok) && (_tmp7A0  || _tmp7A1)) && (
Cyc_Tcutil_unify(_tmp786,_tmp799) || Cyc_Tcenv_region_outlives(te,_tmp786,_tmp799)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};}_LL4AC:;_LL4AD:
 goto _LL4A9;_LL4A9:;}
# 2971
return Cyc_Absyn_Unknown_coercion;_LL49D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp78A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp782;if(_tmp78A->tag != 8)goto _LL49F;else{_tmp78B=(_tmp78A->f1).elt_type;_tmp78C=(_tmp78A->f1).tq;_tmp78D=(_tmp78A->f1).num_elts;_tmp78E=(_tmp78A->f1).zero_term;}}_LL49E:
# 2973
{void*_tmp7AB=t2;void*_tmp7AD;struct Cyc_Absyn_Tqual _tmp7AE;struct Cyc_Absyn_Exp*_tmp7AF;union Cyc_Absyn_Constraint*_tmp7B0;_LL4B4: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp7AC=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp7AB;if(_tmp7AC->tag != 8)goto _LL4B6;else{_tmp7AD=(_tmp7AC->f1).elt_type;_tmp7AE=(_tmp7AC->f1).tq;_tmp7AF=(_tmp7AC->f1).num_elts;_tmp7B0=(_tmp7AC->f1).zero_term;}}_LL4B5: {
# 2975
int okay;
okay=
(((_tmp78D != 0  && _tmp7AF != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp78E,_tmp7B0)) && 
Cyc_Evexp_lte_const_exp(_tmp7AF,_tmp78D)) && 
Cyc_Evexp_lte_const_exp(_tmp78D,_tmp7AF);
return
# 2982
(okay  && Cyc_Tcutil_unify(_tmp78B,_tmp7AD)) && (!_tmp78C.real_const  || _tmp7AE.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}_LL4B6:;_LL4B7:
# 2984
 return Cyc_Absyn_Unknown_coercion;_LL4B3:;}
# 2986
return Cyc_Absyn_Unknown_coercion;_LL49F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp78F=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp782;if(_tmp78F->tag != 13)goto _LL4A1;else{_tmp790=_tmp78F->f2;}}_LL4A0:
# 2990
{void*_tmp7B1=t2;struct Cyc_Absyn_Enumdecl*_tmp7B3;_LL4B9: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp7B2=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp7B1;if(_tmp7B2->tag != 13)goto _LL4BB;else{_tmp7B3=_tmp7B2->f2;}}_LL4BA:
# 2992
 if((_tmp790->fields != 0  && _tmp7B3->fields != 0) && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp790->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp7B3->fields))->v))
return Cyc_Absyn_Other_coercion;
goto _LL4B8;_LL4BB:;_LL4BC:
 goto _LL4B8;_LL4B8:;}
# 2998
goto _LL4A2;_LL4A1: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp791=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp782;if(_tmp791->tag != 6)goto _LL4A3;}_LL4A2:
 goto _LL4A4;_LL4A3: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp792=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp782;if(_tmp792->tag != 7)goto _LL4A5;}_LL4A4:
 return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;_LL4A5: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp793=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp782;if(_tmp793->tag != 15)goto _LL4A7;else{_tmp794=(void*)_tmp793->f1;}}_LL4A6:
# 3003
{void*_tmp7B4=t2;void*_tmp7B6;_LL4BE: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp7B5=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp7B4;if(_tmp7B5->tag != 15)goto _LL4C0;else{_tmp7B6=(void*)_tmp7B5->f1;}}_LL4BF:
# 3005
 if(Cyc_Tcenv_region_outlives(te,_tmp794,_tmp7B6))return Cyc_Absyn_No_coercion;
goto _LL4BD;_LL4C0:;_LL4C1:
 goto _LL4BD;_LL4BD:;}
# 3009
return Cyc_Absyn_Unknown_coercion;_LL4A7:;_LL4A8:
 return Cyc_Absyn_Unknown_coercion;_LL49A:;};}
# 3015
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),t)){
struct Cyc_Absyn_Exp*_tmp7B7=Cyc_Absyn_copy_exp(e);
{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp116A;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp1169;e->r=(void*)((_tmp1169=_cycalloc(sizeof(*_tmp1169)),((_tmp1169[0]=((_tmp116A.tag=13,((_tmp116A.f1=t,((_tmp116A.f2=_tmp7B7,((_tmp116A.f3=0,((_tmp116A.f4=c,_tmp116A)))))))))),_tmp1169))));}
e->topt=t;}}
# 3023
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp7BA=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp7BB=_tmp7BA;_LL4C3: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7BC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7BB;if(_tmp7BC->tag != 6)goto _LL4C5;}_LL4C4:
 goto _LL4C6;_LL4C5: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp7BD=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp7BB;if(_tmp7BD->tag != 13)goto _LL4C7;}_LL4C6:
 goto _LL4C8;_LL4C7: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp7BE=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp7BB;if(_tmp7BE->tag != 14)goto _LL4C9;}_LL4C8:
 goto _LL4CA;_LL4C9: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp7BF=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp7BB;if(_tmp7BF->tag != 19)goto _LL4CB;}_LL4CA:
 return 1;_LL4CB: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp7C0=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp7BB;if(_tmp7C0->tag != 1)goto _LL4CD;}_LL4CC:
 return Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ);_LL4CD:;_LL4CE:
 return 0;_LL4C2:;}
# 3034
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp7C1=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp7C2=_tmp7C1;_LL4D0: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7C3=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp7C2;if(_tmp7C3->tag != 7)goto _LL4D2;}_LL4D1:
 return 1;_LL4D2:;_LL4D3:
 return 0;_LL4CF:;};}
# 3043
int Cyc_Tcutil_is_function_type(void*t){
void*_tmp7C4=Cyc_Tcutil_compress(t);void*_tmp7C5=_tmp7C4;_LL4D5: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp7C6=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp7C5;if(_tmp7C6->tag != 9)goto _LL4D7;}_LL4D6:
 return 1;_LL4D7:;_LL4D8:
 return 0;_LL4D4:;}
# 3050
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
struct _tuple0 _tmp116B;struct _tuple0 _tmp7C7=(_tmp116B.f1=t1,((_tmp116B.f2=t2,_tmp116B)));struct _tuple0 _tmp7C8=_tmp7C7;int _tmp7CA;int _tmp7CC;_LL4DA:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7C9=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp7C8.f1;if(_tmp7C9->tag != 7)goto _LL4DC;else{_tmp7CA=_tmp7C9->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7CB=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp7C8.f2;if(_tmp7CB->tag != 7)goto _LL4DC;else{_tmp7CC=_tmp7CB->f1;}};_LL4DB:
# 3053
 if(_tmp7CA != 0  && _tmp7CA != 1)return t1;else{
if(_tmp7CC != 0  && _tmp7CC != 1)return t2;else{
if(_tmp7CA >= _tmp7CC)return t1;else{
return t2;}}}_LL4DC: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7CD=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp7C8.f1;if(_tmp7CD->tag != 7)goto _LL4DE;}_LL4DD:
 return t1;_LL4DE: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7CE=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp7C8.f2;if(_tmp7CE->tag != 7)goto _LL4E0;}_LL4DF:
 return t2;_LL4E0: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7CF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7C8.f1;if(_tmp7CF->tag != 6)goto _LL4E2;else{if(_tmp7CF->f1 != Cyc_Absyn_Unsigned)goto _LL4E2;if(_tmp7CF->f2 != Cyc_Absyn_LongLong_sz)goto _LL4E2;}}_LL4E1:
 goto _LL4E3;_LL4E2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7D0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7C8.f2;if(_tmp7D0->tag != 6)goto _LL4E4;else{if(_tmp7D0->f1 != Cyc_Absyn_Unsigned)goto _LL4E4;if(_tmp7D0->f2 != Cyc_Absyn_LongLong_sz)goto _LL4E4;}}_LL4E3:
 return Cyc_Absyn_ulonglong_typ;_LL4E4: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7D1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7C8.f1;if(_tmp7D1->tag != 6)goto _LL4E6;else{if(_tmp7D1->f2 != Cyc_Absyn_LongLong_sz)goto _LL4E6;}}_LL4E5:
 goto _LL4E7;_LL4E6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7D2=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7C8.f2;if(_tmp7D2->tag != 6)goto _LL4E8;else{if(_tmp7D2->f2 != Cyc_Absyn_LongLong_sz)goto _LL4E8;}}_LL4E7:
 return Cyc_Absyn_slonglong_typ;_LL4E8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7D3=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7C8.f1;if(_tmp7D3->tag != 6)goto _LL4EA;else{if(_tmp7D3->f1 != Cyc_Absyn_Unsigned)goto _LL4EA;if(_tmp7D3->f2 != Cyc_Absyn_Long_sz)goto _LL4EA;}}_LL4E9:
 goto _LL4EB;_LL4EA: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7D4=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7C8.f2;if(_tmp7D4->tag != 6)goto _LL4EC;else{if(_tmp7D4->f1 != Cyc_Absyn_Unsigned)goto _LL4EC;if(_tmp7D4->f2 != Cyc_Absyn_Long_sz)goto _LL4EC;}}_LL4EB:
 return Cyc_Absyn_ulong_typ;_LL4EC: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7D5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7C8.f1;if(_tmp7D5->tag != 6)goto _LL4EE;else{if(_tmp7D5->f1 != Cyc_Absyn_Unsigned)goto _LL4EE;if(_tmp7D5->f2 != Cyc_Absyn_Int_sz)goto _LL4EE;}}_LL4ED:
# 3066
 goto _LL4EF;_LL4EE: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7D6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7C8.f2;if(_tmp7D6->tag != 6)goto _LL4F0;else{if(_tmp7D6->f1 != Cyc_Absyn_Unsigned)goto _LL4F0;if(_tmp7D6->f2 != Cyc_Absyn_Int_sz)goto _LL4F0;}}_LL4EF:
 return Cyc_Absyn_uint_typ;_LL4F0: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7D7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7C8.f1;if(_tmp7D7->tag != 6)goto _LL4F2;else{if(_tmp7D7->f2 != Cyc_Absyn_Long_sz)goto _LL4F2;}}_LL4F1:
 goto _LL4F3;_LL4F2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7D8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7C8.f2;if(_tmp7D8->tag != 6)goto _LL4F4;else{if(_tmp7D8->f2 != Cyc_Absyn_Long_sz)goto _LL4F4;}}_LL4F3:
 return Cyc_Absyn_slong_typ;_LL4F4:;_LL4F5:
 return Cyc_Absyn_sint_typ;_LL4D9:;}
# 3075
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_tmp7DA=e->r;void*_tmp7DB=_tmp7DA;_LL4F7: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp7DC=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp7DB;if(_tmp7DC->tag != 3)goto _LL4F9;else{if(_tmp7DC->f2 != 0)goto _LL4F9;}}_LL4F8:
{const char*_tmp116E;void*_tmp116D;(_tmp116D=0,Cyc_Tcutil_warn(e->loc,((_tmp116E="assignment in test",_tag_dyneither(_tmp116E,sizeof(char),19))),_tag_dyneither(_tmp116D,sizeof(void*),0)));}goto _LL4F6;_LL4F9:;_LL4FA:
 goto _LL4F6;_LL4F6:;}
# 3089 "tcutil.cyc"
static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){
c1=Cyc_Absyn_compress_kb(c1);
c2=Cyc_Absyn_compress_kb(c2);
if(c1 == c2)return 1;{
struct _tuple0 _tmp116F;struct _tuple0 _tmp7DF=(_tmp116F.f1=c1,((_tmp116F.f2=c2,_tmp116F)));struct _tuple0 _tmp7E0=_tmp7DF;struct Cyc_Absyn_Kind*_tmp7E2;struct Cyc_Absyn_Kind*_tmp7E4;struct Cyc_Core_Opt**_tmp7E6;struct Cyc_Core_Opt**_tmp7E8;struct Cyc_Core_Opt**_tmp7EA;struct Cyc_Absyn_Kind*_tmp7EB;struct Cyc_Absyn_Kind*_tmp7ED;struct Cyc_Absyn_Kind*_tmp7EF;struct Cyc_Core_Opt**_tmp7F1;struct Cyc_Absyn_Kind*_tmp7F2;struct Cyc_Core_Opt**_tmp7F4;struct Cyc_Absyn_Kind*_tmp7F5;struct Cyc_Core_Opt**_tmp7F7;struct Cyc_Absyn_Kind*_tmp7F8;_LL4FC:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp7E1=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7E0.f1;if(_tmp7E1->tag != 0)goto _LL4FE;else{_tmp7E2=_tmp7E1->f1;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp7E3=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7E0.f2;if(_tmp7E3->tag != 0)goto _LL4FE;else{_tmp7E4=_tmp7E3->f1;}};_LL4FD:
 return _tmp7E2 == _tmp7E4;_LL4FE: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp7E5=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7E0.f2;if(_tmp7E5->tag != 1)goto _LL500;else{_tmp7E6=(struct Cyc_Core_Opt**)& _tmp7E5->f1;}}_LL4FF:
{struct Cyc_Core_Opt*_tmp1170;*_tmp7E6=((_tmp1170=_cycalloc(sizeof(*_tmp1170)),((_tmp1170->v=c1,_tmp1170))));}return 1;_LL500: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp7E7=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7E0.f1;if(_tmp7E7->tag != 1)goto _LL502;else{_tmp7E8=(struct Cyc_Core_Opt**)& _tmp7E7->f1;}}_LL501:
{struct Cyc_Core_Opt*_tmp1171;*_tmp7E8=((_tmp1171=_cycalloc(sizeof(*_tmp1171)),((_tmp1171->v=c2,_tmp1171))));}return 1;_LL502:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp7E9=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7E0.f1;if(_tmp7E9->tag != 2)goto _LL504;else{_tmp7EA=(struct Cyc_Core_Opt**)& _tmp7E9->f1;_tmp7EB=_tmp7E9->f2;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp7EC=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7E0.f2;if(_tmp7EC->tag != 0)goto _LL504;else{_tmp7ED=_tmp7EC->f1;}};_LL503:
# 3098
 if(Cyc_Tcutil_kind_leq(_tmp7ED,_tmp7EB)){
{struct Cyc_Core_Opt*_tmp1172;*_tmp7EA=((_tmp1172=_cycalloc(sizeof(*_tmp1172)),((_tmp1172->v=c2,_tmp1172))));}return 1;}else{
return 0;}_LL504:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp7EE=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7E0.f1;if(_tmp7EE->tag != 0)goto _LL506;else{_tmp7EF=_tmp7EE->f1;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp7F0=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7E0.f2;if(_tmp7F0->tag != 2)goto _LL506;else{_tmp7F1=(struct Cyc_Core_Opt**)& _tmp7F0->f1;_tmp7F2=_tmp7F0->f2;}};_LL505:
# 3102
 if(Cyc_Tcutil_kind_leq(_tmp7EF,_tmp7F2)){
{struct Cyc_Core_Opt*_tmp1173;*_tmp7F1=((_tmp1173=_cycalloc(sizeof(*_tmp1173)),((_tmp1173->v=c1,_tmp1173))));}return 1;}else{
return 0;}_LL506:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp7F3=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7E0.f1;if(_tmp7F3->tag != 2)goto _LL4FB;else{_tmp7F4=(struct Cyc_Core_Opt**)& _tmp7F3->f1;_tmp7F5=_tmp7F3->f2;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp7F6=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7E0.f2;if(_tmp7F6->tag != 2)goto _LL4FB;else{_tmp7F7=(struct Cyc_Core_Opt**)& _tmp7F6->f1;_tmp7F8=_tmp7F6->f2;}};_LL507:
# 3106
 if(Cyc_Tcutil_kind_leq(_tmp7F5,_tmp7F8)){
{struct Cyc_Core_Opt*_tmp1174;*_tmp7F7=((_tmp1174=_cycalloc(sizeof(*_tmp1174)),((_tmp1174->v=c1,_tmp1174))));}return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp7F8,_tmp7F5)){
{struct Cyc_Core_Opt*_tmp1175;*_tmp7F4=((_tmp1175=_cycalloc(sizeof(*_tmp1175)),((_tmp1175->v=c2,_tmp1175))));}return 1;}else{
return 0;}}_LL4FB:;};}
# 3115
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 3120
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
const char*_tmp1179;void*_tmp1178[1];struct Cyc_Int_pa_PrintArg_struct _tmp1177;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmp1177.tag=1,((_tmp1177.f1=(unsigned long)i,((_tmp1178[0]=& _tmp1177,Cyc_aprintf(((_tmp1179="#%d",_tag_dyneither(_tmp1179,sizeof(char),4))),_tag_dyneither(_tmp1178,sizeof(void*),1))))))));
struct _dyneither_ptr*_tmp117C;struct Cyc_Absyn_Tvar*_tmp117B;return(_tmp117B=_cycalloc(sizeof(*_tmp117B)),((_tmp117B->name=((_tmp117C=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp117C[0]=(struct _dyneither_ptr)s,_tmp117C)))),((_tmp117B->identity=- 1,((_tmp117B->kind=k,_tmp117B)))))));}
# 3127
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _dyneither_ptr _tmp805=*t->name;
return*((const char*)_check_dyneither_subscript(_tmp805,sizeof(char),0))== '#';}
# 3132
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
{const char*_tmp1180;void*_tmp117F[1];struct Cyc_String_pa_PrintArg_struct _tmp117E;(_tmp117E.tag=0,((_tmp117E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name),((_tmp117F[0]=& _tmp117E,Cyc_printf(((_tmp1180="%s",_tag_dyneither(_tmp1180,sizeof(char),3))),_tag_dyneither(_tmp117F,sizeof(void*),1)))))));}
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
const char*_tmp1181;struct _dyneither_ptr _tmp809=Cyc_strconcat(((_tmp1181="`",_tag_dyneither(_tmp1181,sizeof(char),2))),(struct _dyneither_ptr)*t->name);
{char _tmp1184;char _tmp1183;struct _dyneither_ptr _tmp1182;(_tmp1182=_dyneither_ptr_plus(_tmp809,sizeof(char),1),((_tmp1183=*((char*)_check_dyneither_subscript(_tmp1182,sizeof(char),0)),((_tmp1184='t',((_get_dyneither_size(_tmp1182,sizeof(char))== 1  && (_tmp1183 == '\000'  && _tmp1184 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp1182.curr)=_tmp1184)))))));}{
struct _dyneither_ptr*_tmp1185;t->name=((_tmp1185=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp1185[0]=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp809),_tmp1185))));};};}
# 3141
static struct _tuple10*Cyc_Tcutil_fndecl2typ_f(struct _tuple10*x){
# 3143
struct _tuple10*_tmp1186;return(_tmp1186=_cycalloc(sizeof(*_tmp1186)),((_tmp1186->f1=(*x).f1,((_tmp1186->f2=(*x).f2,((_tmp1186->f3=(*x).f3,_tmp1186)))))));}
# 3146
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_typ == 0){
# 3153
struct Cyc_List_List*_tmp810=0;
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
struct Cyc_List_List*_tmp1187;_tmp810=((_tmp1187=_cycalloc(sizeof(*_tmp1187)),((_tmp1187->hd=(void*)atts->hd,((_tmp1187->tl=_tmp810,_tmp1187))))));}}}{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp118D;struct Cyc_Absyn_FnInfo _tmp118C;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp118B;return(void*)((_tmp118B=_cycalloc(sizeof(*_tmp118B)),((_tmp118B[0]=((_tmp118D.tag=9,((_tmp118D.f1=((_tmp118C.tvars=fd->tvs,((_tmp118C.effect=fd->effect,((_tmp118C.ret_tqual=fd->ret_tqual,((_tmp118C.ret_typ=fd->ret_type,((_tmp118C.args=
((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args),((_tmp118C.c_varargs=fd->c_varargs,((_tmp118C.cyc_varargs=fd->cyc_varargs,((_tmp118C.rgn_po=fd->rgn_po,((_tmp118C.attributes=_tmp810,((_tmp118C.requires_clause=fd->requires_clause,((_tmp118C.requires_relns=fd->requires_relns,((_tmp118C.ensures_clause=fd->ensures_clause,((_tmp118C.ensures_relns=fd->ensures_relns,_tmp118C)))))))))))))))))))))))))),_tmp118D)))),_tmp118B))));};}
# 3166
return(void*)_check_null(fd->cached_typ);}
# 3172
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 3174
union Cyc_Relations_RelnOp _tmp815=*rop;union Cyc_Relations_RelnOp _tmp816=_tmp815;struct Cyc_Absyn_Vardecl*_tmp817;struct Cyc_Absyn_Vardecl*_tmp818;_LL509: if((_tmp816.RVar).tag != 2)goto _LL50B;_tmp817=(struct Cyc_Absyn_Vardecl*)(_tmp816.RVar).val;_LL50A: {
# 3176
struct _tuple2 _tmp819=*_tmp817->name;union Cyc_Absyn_Nmspace _tmp81B;struct _dyneither_ptr*_tmp81C;struct _tuple2 _tmp81A=_tmp819;_tmp81B=_tmp81A.f1;_tmp81C=_tmp81A.f2;
if(!((int)((_tmp81B.Loc_n).tag == 4)))goto _LL508;
{const char*_tmp118E;if(Cyc_strcmp((struct _dyneither_ptr)*_tmp81C,((_tmp118E="return_value",_tag_dyneither(_tmp118E,sizeof(char),13))))== 0){
*rop=Cyc_Relations_RReturn();
goto _LL508;}}{
# 3182
unsigned int c=0;
{struct Cyc_List_List*_tmp81E=args;for(0;_tmp81E != 0;(_tmp81E=_tmp81E->tl,c ++)){
struct _tuple10*_tmp81F=(struct _tuple10*)_tmp81E->hd;struct _dyneither_ptr*_tmp821;struct _tuple10*_tmp820=_tmp81F;_tmp821=_tmp820->f1;
if(_tmp821 != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp81C,(struct _dyneither_ptr)*_tmp821)== 0){
*rop=Cyc_Relations_RParam(c);
break;}}}}
# 3192
goto _LL508;};}_LL50B: if((_tmp816.RNumelts).tag != 3)goto _LL50D;_tmp818=(struct Cyc_Absyn_Vardecl*)(_tmp816.RNumelts).val;_LL50C: {
# 3194
struct _tuple2 _tmp822=*_tmp818->name;union Cyc_Absyn_Nmspace _tmp824;struct _dyneither_ptr*_tmp825;struct _tuple2 _tmp823=_tmp822;_tmp824=_tmp823.f1;_tmp825=_tmp823.f2;
if(!((int)((_tmp824.Loc_n).tag == 4)))goto _LL508;{
unsigned int c=0;
{struct Cyc_List_List*_tmp826=args;for(0;_tmp826 != 0;(_tmp826=_tmp826->tl,c ++)){
struct _tuple10*_tmp827=(struct _tuple10*)_tmp826->hd;struct _dyneither_ptr*_tmp829;struct _tuple10*_tmp828=_tmp827;_tmp829=_tmp828->f1;
if(_tmp829 != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp825,(struct _dyneither_ptr)*_tmp829)== 0){
*rop=Cyc_Relations_RParamNumelts(c);
break;}}}}
# 3206
goto _LL508;};}_LL50D:;_LL50E:
 goto _LL508;_LL508:;}
# 3211
static void Cyc_Tcutil_replace_rops(struct Cyc_List_List*args,struct Cyc_Relations_Reln*r){
# 3213
Cyc_Tcutil_replace_rop(args,& r->rop1);
Cyc_Tcutil_replace_rop(args,& r->rop2);}
# 3217
static struct Cyc_List_List*Cyc_Tcutil_extract_relns(struct Cyc_List_List*args,struct Cyc_Absyn_Exp*e){
# 3219
if(e == 0)return 0;{
struct Cyc_List_List*_tmp82A=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,_tmp82A);
return _tmp82A;};}struct _tuple25{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 3226
static void*Cyc_Tcutil_fst_fdarg(struct _tuple25*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct Cyc_Absyn_Tqual _tmp82C;void*_tmp82D;struct _tuple12*_tmp82B=pr;_tmp82C=_tmp82B->f1;_tmp82D=_tmp82B->f2;
if(_tmp82D == t)return pr;else{
struct _tuple12*_tmp118F;return(_tmp118F=_cycalloc(sizeof(*_tmp118F)),((_tmp118F->f1=_tmp82C,((_tmp118F->f2=t,_tmp118F)))));}}struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 3233
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
# 3235
struct _tuple26*_tmp1192;struct _tuple27*_tmp1191;return(_tmp1191=_region_malloc(rgn,sizeof(*_tmp1191)),((_tmp1191->f1=((_tmp1192=_region_malloc(rgn,sizeof(*_tmp1192)),((_tmp1192->f1=(*y).f1,((_tmp1192->f2=(*y).f2,_tmp1192)))))),((_tmp1191->f2=(*y).f3,_tmp1191)))));}
# 3237
static struct _tuple10*Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){
# 3239
struct _tuple10 _tmp831=*orig_arg;struct _dyneither_ptr*_tmp833;struct Cyc_Absyn_Tqual _tmp834;void*_tmp835;struct _tuple10 _tmp832=_tmp831;_tmp833=_tmp832.f1;_tmp834=_tmp832.f2;_tmp835=_tmp832.f3;
if(t == _tmp835)return orig_arg;{
struct _tuple10*_tmp1193;return(_tmp1193=_cycalloc(sizeof(*_tmp1193)),((_tmp1193->f1=_tmp833,((_tmp1193->f2=_tmp834,((_tmp1193->f3=t,_tmp1193)))))));};}
# 3243
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){
return f->type;}
# 3246
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 3251
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 3253
struct Cyc_Absyn_Exp*_tmp1194;return(_tmp1194=_cycalloc(sizeof(*_tmp1194)),((_tmp1194->topt=old->topt,((_tmp1194->r=r,((_tmp1194->loc=old->loc,((_tmp1194->annot=old->annot,_tmp1194)))))))));}
# 3258
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp838=e->r;void*_tmp839=_tmp838;enum Cyc_Absyn_Primop _tmp83F;struct Cyc_List_List*_tmp840;struct Cyc_Absyn_Exp*_tmp842;struct Cyc_Absyn_Exp*_tmp843;struct Cyc_Absyn_Exp*_tmp844;struct Cyc_Absyn_Exp*_tmp846;struct Cyc_Absyn_Exp*_tmp847;struct Cyc_Absyn_Exp*_tmp849;struct Cyc_Absyn_Exp*_tmp84A;struct Cyc_Absyn_Exp*_tmp84C;struct Cyc_Absyn_Exp*_tmp84D;void*_tmp84F;struct Cyc_Absyn_Exp*_tmp850;int _tmp851;enum Cyc_Absyn_Coercion _tmp852;void*_tmp854;struct Cyc_Absyn_Exp*_tmp856;void*_tmp858;struct Cyc_List_List*_tmp859;void*_tmp85B;_LL510: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp83A=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp839;if(_tmp83A->tag != 0)goto _LL512;}_LL511:
 goto _LL513;_LL512: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp83B=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp839;if(_tmp83B->tag != 31)goto _LL514;}_LL513:
 goto _LL515;_LL514: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp83C=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp839;if(_tmp83C->tag != 32)goto _LL516;}_LL515:
 goto _LL517;_LL516: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp83D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp839;if(_tmp83D->tag != 1)goto _LL518;}_LL517:
 return e;_LL518: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp83E=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp839;if(_tmp83E->tag != 2)goto _LL51A;else{_tmp83F=_tmp83E->f1;_tmp840=_tmp83E->f2;}}_LL519:
# 3266
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp840)== 1){
struct Cyc_Absyn_Exp*_tmp85C=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp840))->hd;
struct Cyc_Absyn_Exp*_tmp85D=Cyc_Tcutil_rsubsexp(r,inst,_tmp85C);
if(_tmp85D == _tmp85C)return e;{
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp119A;struct Cyc_Absyn_Exp*_tmp1199[1];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp1198;return Cyc_Tcutil_copye(e,(void*)((_tmp1198=_cycalloc(sizeof(*_tmp1198)),((_tmp1198[0]=((_tmp119A.tag=2,((_tmp119A.f1=_tmp83F,((_tmp119A.f2=((_tmp1199[0]=_tmp85D,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1199,sizeof(struct Cyc_Absyn_Exp*),1)))),_tmp119A)))))),_tmp1198)))));};}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp840)== 2){
struct Cyc_Absyn_Exp*_tmp861=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp840))->hd;
struct Cyc_Absyn_Exp*_tmp862=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp840->tl))->hd;
struct Cyc_Absyn_Exp*_tmp863=Cyc_Tcutil_rsubsexp(r,inst,_tmp861);
struct Cyc_Absyn_Exp*_tmp864=Cyc_Tcutil_rsubsexp(r,inst,_tmp862);
if(_tmp863 == _tmp861  && _tmp864 == _tmp862)return e;{
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp11A0;struct Cyc_Absyn_Exp*_tmp119F[2];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp119E;return Cyc_Tcutil_copye(e,(void*)((_tmp119E=_cycalloc(sizeof(*_tmp119E)),((_tmp119E[0]=((_tmp11A0.tag=2,((_tmp11A0.f1=_tmp83F,((_tmp11A0.f2=((_tmp119F[1]=_tmp864,((_tmp119F[0]=_tmp863,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp119F,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmp11A0)))))),_tmp119E)))));};}else{
const char*_tmp11A3;void*_tmp11A2;return(_tmp11A2=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11A3="primop does not have 1 or 2 args!",_tag_dyneither(_tmp11A3,sizeof(char),34))),_tag_dyneither(_tmp11A2,sizeof(void*),0)));}}_LL51A: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp841=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp839;if(_tmp841->tag != 5)goto _LL51C;else{_tmp842=_tmp841->f1;_tmp843=_tmp841->f2;_tmp844=_tmp841->f3;}}_LL51B: {
# 3280
struct Cyc_Absyn_Exp*_tmp86A=Cyc_Tcutil_rsubsexp(r,inst,_tmp842);
struct Cyc_Absyn_Exp*_tmp86B=Cyc_Tcutil_rsubsexp(r,inst,_tmp843);
struct Cyc_Absyn_Exp*_tmp86C=Cyc_Tcutil_rsubsexp(r,inst,_tmp844);
if((_tmp86A == _tmp842  && _tmp86B == _tmp843) && _tmp86C == _tmp844)return e;{
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp11A6;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp11A5;return Cyc_Tcutil_copye(e,(void*)((_tmp11A5=_cycalloc(sizeof(*_tmp11A5)),((_tmp11A5[0]=((_tmp11A6.tag=5,((_tmp11A6.f1=_tmp86A,((_tmp11A6.f2=_tmp86B,((_tmp11A6.f3=_tmp86C,_tmp11A6)))))))),_tmp11A5)))));};}_LL51C: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp845=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp839;if(_tmp845->tag != 6)goto _LL51E;else{_tmp846=_tmp845->f1;_tmp847=_tmp845->f2;}}_LL51D: {
# 3286
struct Cyc_Absyn_Exp*_tmp86F=Cyc_Tcutil_rsubsexp(r,inst,_tmp846);
struct Cyc_Absyn_Exp*_tmp870=Cyc_Tcutil_rsubsexp(r,inst,_tmp847);
if(_tmp86F == _tmp846  && _tmp870 == _tmp847)return e;{
struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmp11A9;struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp11A8;return Cyc_Tcutil_copye(e,(void*)((_tmp11A8=_cycalloc(sizeof(*_tmp11A8)),((_tmp11A8[0]=((_tmp11A9.tag=6,((_tmp11A9.f1=_tmp86F,((_tmp11A9.f2=_tmp870,_tmp11A9)))))),_tmp11A8)))));};}_LL51E: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp848=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp839;if(_tmp848->tag != 7)goto _LL520;else{_tmp849=_tmp848->f1;_tmp84A=_tmp848->f2;}}_LL51F: {
# 3291
struct Cyc_Absyn_Exp*_tmp873=Cyc_Tcutil_rsubsexp(r,inst,_tmp849);
struct Cyc_Absyn_Exp*_tmp874=Cyc_Tcutil_rsubsexp(r,inst,_tmp84A);
if(_tmp873 == _tmp849  && _tmp874 == _tmp84A)return e;{
struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmp11AC;struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp11AB;return Cyc_Tcutil_copye(e,(void*)((_tmp11AB=_cycalloc(sizeof(*_tmp11AB)),((_tmp11AB[0]=((_tmp11AC.tag=7,((_tmp11AC.f1=_tmp873,((_tmp11AC.f2=_tmp874,_tmp11AC)))))),_tmp11AB)))));};}_LL520: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp84B=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp839;if(_tmp84B->tag != 8)goto _LL522;else{_tmp84C=_tmp84B->f1;_tmp84D=_tmp84B->f2;}}_LL521: {
# 3296
struct Cyc_Absyn_Exp*_tmp877=Cyc_Tcutil_rsubsexp(r,inst,_tmp84C);
struct Cyc_Absyn_Exp*_tmp878=Cyc_Tcutil_rsubsexp(r,inst,_tmp84D);
if(_tmp877 == _tmp84C  && _tmp878 == _tmp84D)return e;{
struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmp11AF;struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp11AE;return Cyc_Tcutil_copye(e,(void*)((_tmp11AE=_cycalloc(sizeof(*_tmp11AE)),((_tmp11AE[0]=((_tmp11AF.tag=8,((_tmp11AF.f1=_tmp877,((_tmp11AF.f2=_tmp878,_tmp11AF)))))),_tmp11AE)))));};}_LL522: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp84E=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp839;if(_tmp84E->tag != 13)goto _LL524;else{_tmp84F=(void*)_tmp84E->f1;_tmp850=_tmp84E->f2;_tmp851=_tmp84E->f3;_tmp852=_tmp84E->f4;}}_LL523: {
# 3301
struct Cyc_Absyn_Exp*_tmp87B=Cyc_Tcutil_rsubsexp(r,inst,_tmp850);
void*_tmp87C=Cyc_Tcutil_rsubstitute(r,inst,_tmp84F);
if(_tmp87B == _tmp850  && _tmp87C == _tmp84F)return e;{
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp11B2;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp11B1;return Cyc_Tcutil_copye(e,(void*)((_tmp11B1=_cycalloc(sizeof(*_tmp11B1)),((_tmp11B1[0]=((_tmp11B2.tag=13,((_tmp11B2.f1=_tmp87C,((_tmp11B2.f2=_tmp87B,((_tmp11B2.f3=_tmp851,((_tmp11B2.f4=_tmp852,_tmp11B2)))))))))),_tmp11B1)))));};}_LL524: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp853=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp839;if(_tmp853->tag != 16)goto _LL526;else{_tmp854=(void*)_tmp853->f1;}}_LL525: {
# 3306
void*_tmp87F=Cyc_Tcutil_rsubstitute(r,inst,_tmp854);
if(_tmp87F == _tmp854)return e;{
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp11B5;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp11B4;return Cyc_Tcutil_copye(e,(void*)((_tmp11B4=_cycalloc(sizeof(*_tmp11B4)),((_tmp11B4[0]=((_tmp11B5.tag=16,((_tmp11B5.f1=_tmp87F,_tmp11B5)))),_tmp11B4)))));};}_LL526: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp855=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp839;if(_tmp855->tag != 17)goto _LL528;else{_tmp856=_tmp855->f1;}}_LL527: {
# 3310
struct Cyc_Absyn_Exp*_tmp882=Cyc_Tcutil_rsubsexp(r,inst,_tmp856);
if(_tmp882 == _tmp856)return e;{
struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmp11B8;struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp11B7;return Cyc_Tcutil_copye(e,(void*)((_tmp11B7=_cycalloc(sizeof(*_tmp11B7)),((_tmp11B7[0]=((_tmp11B8.tag=17,((_tmp11B8.f1=_tmp882,_tmp11B8)))),_tmp11B7)))));};}_LL528: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp857=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp839;if(_tmp857->tag != 18)goto _LL52A;else{_tmp858=(void*)_tmp857->f1;_tmp859=_tmp857->f2;}}_LL529: {
# 3314
void*_tmp885=Cyc_Tcutil_rsubstitute(r,inst,_tmp858);
if(_tmp885 == _tmp858)return e;{
struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp11BB;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp11BA;return Cyc_Tcutil_copye(e,(void*)((_tmp11BA=_cycalloc(sizeof(*_tmp11BA)),((_tmp11BA[0]=((_tmp11BB.tag=18,((_tmp11BB.f1=_tmp885,((_tmp11BB.f2=_tmp859,_tmp11BB)))))),_tmp11BA)))));};}_LL52A: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp85A=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp839;if(_tmp85A->tag != 38)goto _LL52C;else{_tmp85B=(void*)_tmp85A->f1;}}_LL52B: {
# 3318
void*_tmp888=Cyc_Tcutil_rsubstitute(r,inst,_tmp85B);
if(_tmp888 == _tmp85B)return e;{
# 3321
void*_tmp889=Cyc_Tcutil_compress(_tmp888);void*_tmp88A=_tmp889;struct Cyc_Absyn_Exp*_tmp88C;_LL52F: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp88B=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp88A;if(_tmp88B->tag != 18)goto _LL531;else{_tmp88C=_tmp88B->f1;}}_LL530:
 return _tmp88C;_LL531:;_LL532: {
# 3324
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp11BE;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp11BD;return Cyc_Tcutil_copye(e,(void*)((_tmp11BD=_cycalloc(sizeof(*_tmp11BD)),((_tmp11BD[0]=((_tmp11BE.tag=38,((_tmp11BE.f1=_tmp888,_tmp11BE)))),_tmp11BD)))));}_LL52E:;};}_LL52C:;_LL52D: {
# 3327
const char*_tmp11C1;void*_tmp11C0;return(_tmp11C0=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11C1="non-type-level-expression in Tcutil::rsubsexp",_tag_dyneither(_tmp11C1,sizeof(char),46))),_tag_dyneither(_tmp11C0,sizeof(void*),0)));}_LL50F:;}
# 3331
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 3334
if(e == 0)return 0;else{
return Cyc_Tcutil_rsubsexp(r,inst,e);}}
# 3338
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 3341
void*_tmp891=f->type;
struct Cyc_Absyn_Exp*_tmp892=f->requires_clause;
void*_tmp893=Cyc_Tcutil_rsubstitute(r,inst,_tmp891);
struct Cyc_Absyn_Exp*_tmp894=Cyc_Tcutil_rsubs_exp_opt(r,inst,_tmp892);
if(_tmp891 == _tmp893  && _tmp892 == _tmp894)return f;else{
struct Cyc_Absyn_Aggrfield*_tmp11C2;return(_tmp11C2=_cycalloc(sizeof(*_tmp11C2)),((_tmp11C2->name=f->name,((_tmp11C2->tq=f->tq,((_tmp11C2->type=_tmp893,((_tmp11C2->width=f->width,((_tmp11C2->attributes=f->attributes,((_tmp11C2->requires_clause=_tmp894,_tmp11C2)))))))))))));}}
# 3351
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 3354
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp896=(struct Cyc_Absyn_Aggrfield*)fs->hd;
struct Cyc_List_List*_tmp897=fs->tl;
struct Cyc_Absyn_Aggrfield*_tmp898=Cyc_Tcutil_subst_aggrfield(r,inst,_tmp896);
struct Cyc_List_List*_tmp899=Cyc_Tcutil_subst_aggrfields(r,inst,_tmp897);
if(_tmp898 == _tmp896  && _tmp899 == _tmp897)return fs;{
# 3361
struct Cyc_List_List*_tmp11C3;return(_tmp11C3=_cycalloc(sizeof(*_tmp11C3)),((_tmp11C3->hd=_tmp898,((_tmp11C3->tl=_tmp899,_tmp11C3)))));};};}
# 3364
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 3367
struct _tuple1 _tmp89B=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct Cyc_List_List*_tmp89D;struct Cyc_List_List*_tmp89E;struct _tuple1 _tmp89C=_tmp89B;_tmp89D=_tmp89C.f1;_tmp89E=_tmp89C.f2;{
struct Cyc_List_List*_tmp89F=Cyc_Tcutil_substs(rgn,inst,_tmp89D);
struct Cyc_List_List*_tmp8A0=Cyc_Tcutil_substs(rgn,inst,_tmp89E);
if(_tmp89F == _tmp89D  && _tmp8A0 == _tmp89E)
return rgn_po;else{
# 3373
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp89F,_tmp8A0);}};}
# 3376
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3379
void*_tmp8A1=Cyc_Tcutil_compress(t);void*_tmp8A2=_tmp8A1;struct Cyc_Absyn_Tvar*_tmp8A4;union Cyc_Absyn_AggrInfoU _tmp8A6;struct Cyc_List_List*_tmp8A7;union Cyc_Absyn_DatatypeInfoU _tmp8A9;struct Cyc_List_List*_tmp8AA;union Cyc_Absyn_DatatypeFieldInfoU _tmp8AC;struct Cyc_List_List*_tmp8AD;struct _tuple2*_tmp8AF;struct Cyc_List_List*_tmp8B0;struct Cyc_Absyn_Typedefdecl*_tmp8B1;void*_tmp8B2;void*_tmp8B4;struct Cyc_Absyn_Tqual _tmp8B5;struct Cyc_Absyn_Exp*_tmp8B6;union Cyc_Absyn_Constraint*_tmp8B7;unsigned int _tmp8B8;void*_tmp8BA;struct Cyc_Absyn_Tqual _tmp8BB;void*_tmp8BC;union Cyc_Absyn_Constraint*_tmp8BD;union Cyc_Absyn_Constraint*_tmp8BE;union Cyc_Absyn_Constraint*_tmp8BF;struct Cyc_List_List*_tmp8C1;void*_tmp8C2;struct Cyc_Absyn_Tqual _tmp8C3;void*_tmp8C4;struct Cyc_List_List*_tmp8C5;int _tmp8C6;struct Cyc_Absyn_VarargInfo*_tmp8C7;struct Cyc_List_List*_tmp8C8;struct Cyc_List_List*_tmp8C9;struct Cyc_Absyn_Exp*_tmp8CA;struct Cyc_Absyn_Exp*_tmp8CB;struct Cyc_List_List*_tmp8CD;enum Cyc_Absyn_AggrKind _tmp8CF;struct Cyc_List_List*_tmp8D0;void*_tmp8D2;void*_tmp8D4;void*_tmp8D6;void*_tmp8D7;void*_tmp8D9;struct Cyc_Absyn_Exp*_tmp8DB;void*_tmp8E5;void*_tmp8E7;struct Cyc_List_List*_tmp8E9;_LL534: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8A3=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp8A2;if(_tmp8A3->tag != 2)goto _LL536;else{_tmp8A4=_tmp8A3->f1;}}_LL535: {
# 3382
struct _handler_cons _tmp8EB;_push_handler(& _tmp8EB);{int _tmp8ED=0;if(setjmp(_tmp8EB.handler))_tmp8ED=1;if(!_tmp8ED){{void*_tmp8EE=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp8A4);_npop_handler(0);return _tmp8EE;};_pop_handler();}else{void*_tmp8EC=(void*)_exn_thrown;void*_tmp8F0=_tmp8EC;void*_tmp8F2;_LL56B: {struct Cyc_Core_Not_found_exn_struct*_tmp8F1=(struct Cyc_Core_Not_found_exn_struct*)_tmp8F0;if(_tmp8F1->tag != Cyc_Core_Not_found)goto _LL56D;}_LL56C:
 return t;_LL56D: _tmp8F2=_tmp8F0;_LL56E:(void)_rethrow(_tmp8F2);_LL56A:;}};}_LL536: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp8A5=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp8A2;if(_tmp8A5->tag != 11)goto _LL538;else{_tmp8A6=(_tmp8A5->f1).aggr_info;_tmp8A7=(_tmp8A5->f1).targs;}}_LL537: {
# 3385
struct Cyc_List_List*_tmp8F3=Cyc_Tcutil_substs(rgn,inst,_tmp8A7);
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp11C9;struct Cyc_Absyn_AggrInfo _tmp11C8;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp11C7;return _tmp8F3 == _tmp8A7?t:(void*)((_tmp11C7=_cycalloc(sizeof(*_tmp11C7)),((_tmp11C7[0]=((_tmp11C9.tag=11,((_tmp11C9.f1=((_tmp11C8.aggr_info=_tmp8A6,((_tmp11C8.targs=_tmp8F3,_tmp11C8)))),_tmp11C9)))),_tmp11C7))));}_LL538: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp8A8=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp8A2;if(_tmp8A8->tag != 3)goto _LL53A;else{_tmp8A9=(_tmp8A8->f1).datatype_info;_tmp8AA=(_tmp8A8->f1).targs;}}_LL539: {
# 3388
struct Cyc_List_List*_tmp8F7=Cyc_Tcutil_substs(rgn,inst,_tmp8AA);
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp11CF;struct Cyc_Absyn_DatatypeInfo _tmp11CE;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp11CD;return _tmp8F7 == _tmp8AA?t:(void*)(
(_tmp11CD=_cycalloc(sizeof(*_tmp11CD)),((_tmp11CD[0]=((_tmp11CF.tag=3,((_tmp11CF.f1=((_tmp11CE.datatype_info=_tmp8A9,((_tmp11CE.targs=_tmp8F7,_tmp11CE)))),_tmp11CF)))),_tmp11CD))));}_LL53A: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp8AB=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp8A2;if(_tmp8AB->tag != 4)goto _LL53C;else{_tmp8AC=(_tmp8AB->f1).field_info;_tmp8AD=(_tmp8AB->f1).targs;}}_LL53B: {
# 3392
struct Cyc_List_List*_tmp8FB=Cyc_Tcutil_substs(rgn,inst,_tmp8AD);
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp11D5;struct Cyc_Absyn_DatatypeFieldInfo _tmp11D4;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp11D3;return _tmp8FB == _tmp8AD?t:(void*)(
(_tmp11D3=_cycalloc(sizeof(*_tmp11D3)),((_tmp11D3[0]=((_tmp11D5.tag=4,((_tmp11D5.f1=((_tmp11D4.field_info=_tmp8AC,((_tmp11D4.targs=_tmp8FB,_tmp11D4)))),_tmp11D5)))),_tmp11D3))));}_LL53C: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp8AE=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp8A2;if(_tmp8AE->tag != 17)goto _LL53E;else{_tmp8AF=_tmp8AE->f1;_tmp8B0=_tmp8AE->f2;_tmp8B1=_tmp8AE->f3;_tmp8B2=(void*)_tmp8AE->f4;}}_LL53D: {
# 3396
struct Cyc_List_List*_tmp8FF=Cyc_Tcutil_substs(rgn,inst,_tmp8B0);
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp11D8;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp11D7;return _tmp8FF == _tmp8B0?t:(void*)((_tmp11D7=_cycalloc(sizeof(*_tmp11D7)),((_tmp11D7[0]=((_tmp11D8.tag=17,((_tmp11D8.f1=_tmp8AF,((_tmp11D8.f2=_tmp8FF,((_tmp11D8.f3=_tmp8B1,((_tmp11D8.f4=_tmp8B2,_tmp11D8)))))))))),_tmp11D7))));}_LL53E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8B3=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8A2;if(_tmp8B3->tag != 8)goto _LL540;else{_tmp8B4=(_tmp8B3->f1).elt_type;_tmp8B5=(_tmp8B3->f1).tq;_tmp8B6=(_tmp8B3->f1).num_elts;_tmp8B7=(_tmp8B3->f1).zero_term;_tmp8B8=(_tmp8B3->f1).zt_loc;}}_LL53F: {
# 3399
void*_tmp902=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8B4);
struct Cyc_Absyn_Exp*_tmp903=_tmp8B6 == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,_tmp8B6);
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp11DE;struct Cyc_Absyn_ArrayInfo _tmp11DD;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp11DC;return _tmp902 == _tmp8B4  && _tmp903 == _tmp8B6?t:(void*)(
(_tmp11DC=_cycalloc(sizeof(*_tmp11DC)),((_tmp11DC[0]=((_tmp11DE.tag=8,((_tmp11DE.f1=((_tmp11DD.elt_type=_tmp902,((_tmp11DD.tq=_tmp8B5,((_tmp11DD.num_elts=_tmp903,((_tmp11DD.zero_term=_tmp8B7,((_tmp11DD.zt_loc=_tmp8B8,_tmp11DD)))))))))),_tmp11DE)))),_tmp11DC))));}_LL540: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8B9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8A2;if(_tmp8B9->tag != 5)goto _LL542;else{_tmp8BA=(_tmp8B9->f1).elt_typ;_tmp8BB=(_tmp8B9->f1).elt_tq;_tmp8BC=((_tmp8B9->f1).ptr_atts).rgn;_tmp8BD=((_tmp8B9->f1).ptr_atts).nullable;_tmp8BE=((_tmp8B9->f1).ptr_atts).bounds;_tmp8BF=((_tmp8B9->f1).ptr_atts).zero_term;}}_LL541: {
# 3404
void*_tmp907=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8BA);
void*_tmp908=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8BC);
union Cyc_Absyn_Constraint*_tmp909=_tmp8BE;
{void*_tmp90A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp8BE);void*_tmp90B=_tmp90A;struct Cyc_Absyn_Exp*_tmp90D;_LL570: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp90C=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp90B;if(_tmp90C->tag != 1)goto _LL572;else{_tmp90D=_tmp90C->f1;}}_LL571: {
# 3409
struct Cyc_Absyn_Exp*_tmp90E=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp90D);
if(_tmp90E != _tmp90D){
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp11E1;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp11E0;_tmp909=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp11E0=_cycalloc(sizeof(*_tmp11E0)),((_tmp11E0[0]=((_tmp11E1.tag=1,((_tmp11E1.f1=_tmp90E,_tmp11E1)))),_tmp11E0)))));}
goto _LL56F;}_LL572:;_LL573:
 goto _LL56F;_LL56F:;}
# 3415
if((_tmp907 == _tmp8BA  && _tmp908 == _tmp8BC) && _tmp909 == _tmp8BE)
return t;{
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp11E7;struct Cyc_Absyn_PtrInfo _tmp11E6;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp11E5;return(void*)((_tmp11E5=_cycalloc(sizeof(*_tmp11E5)),((_tmp11E5[0]=((_tmp11E7.tag=5,((_tmp11E7.f1=((_tmp11E6.elt_typ=_tmp907,((_tmp11E6.elt_tq=_tmp8BB,((_tmp11E6.ptr_atts=(((_tmp11E6.ptr_atts).rgn=_tmp908,(((_tmp11E6.ptr_atts).nullable=_tmp8BD,(((_tmp11E6.ptr_atts).bounds=_tmp909,(((_tmp11E6.ptr_atts).zero_term=_tmp8BF,(((_tmp11E6.ptr_atts).ptrloc=0,_tmp11E6.ptr_atts)))))))))),_tmp11E6)))))),_tmp11E7)))),_tmp11E5))));};}_LL542: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp8C0=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp8A2;if(_tmp8C0->tag != 9)goto _LL544;else{_tmp8C1=(_tmp8C0->f1).tvars;_tmp8C2=(_tmp8C0->f1).effect;_tmp8C3=(_tmp8C0->f1).ret_tqual;_tmp8C4=(_tmp8C0->f1).ret_typ;_tmp8C5=(_tmp8C0->f1).args;_tmp8C6=(_tmp8C0->f1).c_varargs;_tmp8C7=(_tmp8C0->f1).cyc_varargs;_tmp8C8=(_tmp8C0->f1).rgn_po;_tmp8C9=(_tmp8C0->f1).attributes;_tmp8CA=(_tmp8C0->f1).requires_clause;_tmp8CB=(_tmp8C0->f1).ensures_clause;}}_LL543:
# 3421
{struct Cyc_List_List*_tmp914=_tmp8C1;for(0;_tmp914 != 0;_tmp914=_tmp914->tl){
struct _tuple16*_tmp11F1;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp11F0;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp11EF;struct Cyc_List_List*_tmp11EE;inst=((_tmp11EE=_region_malloc(rgn,sizeof(*_tmp11EE)),((_tmp11EE->hd=((_tmp11F1=_region_malloc(rgn,sizeof(*_tmp11F1)),((_tmp11F1->f1=(struct Cyc_Absyn_Tvar*)_tmp914->hd,((_tmp11F1->f2=(void*)((_tmp11EF=_cycalloc(sizeof(*_tmp11EF)),((_tmp11EF[0]=((_tmp11F0.tag=2,((_tmp11F0.f1=(struct Cyc_Absyn_Tvar*)_tmp914->hd,_tmp11F0)))),_tmp11EF)))),_tmp11F1)))))),((_tmp11EE->tl=inst,_tmp11EE))))));}}{
struct _tuple1 _tmp919=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp8C5));
# 3423
struct Cyc_List_List*_tmp91B;struct Cyc_List_List*_tmp91C;struct _tuple1 _tmp91A=_tmp919;_tmp91B=_tmp91A.f1;_tmp91C=_tmp91A.f2;{
# 3425
struct Cyc_List_List*_tmp91D=_tmp8C5;
struct Cyc_List_List*_tmp91E=Cyc_Tcutil_substs(rgn,inst,_tmp91C);
if(_tmp91E != _tmp91C)
_tmp91D=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmp8C5,_tmp91E);{
void*eff2;
if(_tmp8C2 == 0)
eff2=0;else{
# 3433
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8C2);
if(new_eff == _tmp8C2)
eff2=_tmp8C2;else{
# 3437
eff2=new_eff;}}{
# 3439
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(_tmp8C7 == 0)
cyc_varargs2=0;else{
# 3443
struct Cyc_Absyn_VarargInfo _tmp91F=*_tmp8C7;struct _dyneither_ptr*_tmp921;struct Cyc_Absyn_Tqual _tmp922;void*_tmp923;int _tmp924;struct Cyc_Absyn_VarargInfo _tmp920=_tmp91F;_tmp921=_tmp920.name;_tmp922=_tmp920.tq;_tmp923=_tmp920.type;_tmp924=_tmp920.inject;{
void*_tmp925=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp923);
if(_tmp925 == _tmp923)cyc_varargs2=_tmp8C7;else{
# 3447
struct Cyc_Absyn_VarargInfo*_tmp11F2;cyc_varargs2=((_tmp11F2=_cycalloc(sizeof(*_tmp11F2)),((_tmp11F2->name=_tmp921,((_tmp11F2->tq=_tmp922,((_tmp11F2->type=_tmp925,((_tmp11F2->inject=_tmp924,_tmp11F2))))))))));}};}{
# 3449
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp8C8);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp8CA);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp8CB);
struct Cyc_List_List*_tmp927=Cyc_Tcutil_extract_relns(_tmp91D,req2);
struct Cyc_List_List*_tmp928=Cyc_Tcutil_extract_relns(_tmp91D,ens2);
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp11F8;struct Cyc_Absyn_FnInfo _tmp11F7;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp11F6;return(void*)(
(_tmp11F6=_cycalloc(sizeof(*_tmp11F6)),((_tmp11F6[0]=((_tmp11F8.tag=9,((_tmp11F8.f1=((_tmp11F7.tvars=_tmp8C1,((_tmp11F7.effect=eff2,((_tmp11F7.ret_tqual=_tmp8C3,((_tmp11F7.ret_typ=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8C4),((_tmp11F7.args=_tmp91D,((_tmp11F7.c_varargs=_tmp8C6,((_tmp11F7.cyc_varargs=cyc_varargs2,((_tmp11F7.rgn_po=rgn_po2,((_tmp11F7.attributes=_tmp8C9,((_tmp11F7.requires_clause=req2,((_tmp11F7.requires_relns=_tmp927,((_tmp11F7.ensures_clause=ens2,((_tmp11F7.ensures_relns=_tmp928,_tmp11F7)))))))))))))))))))))))))),_tmp11F8)))),_tmp11F6))));};};};};};_LL544: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp8CC=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp8A2;if(_tmp8CC->tag != 10)goto _LL546;else{_tmp8CD=_tmp8CC->f1;}}_LL545: {
# 3459
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp92C=_tmp8CD;for(0;_tmp92C != 0;_tmp92C=_tmp92C->tl){
void*_tmp92D=(*((struct _tuple12*)_tmp92C->hd)).f2;
void*_tmp92E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp92D);
if(_tmp92D != _tmp92E)
change=1;{
# 3467
struct Cyc_List_List*_tmp11F9;ts2=((_tmp11F9=_region_malloc(rgn,sizeof(*_tmp11F9)),((_tmp11F9->hd=_tmp92E,((_tmp11F9->tl=ts2,_tmp11F9))))));};}}
# 3469
if(!change)
return t;{
struct Cyc_List_List*_tmp930=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp8CD,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp11FC;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp11FB;return(void*)((_tmp11FB=_cycalloc(sizeof(*_tmp11FB)),((_tmp11FB[0]=((_tmp11FC.tag=10,((_tmp11FC.f1=_tmp930,_tmp11FC)))),_tmp11FB))));};}_LL546: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp8CE=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp8A2;if(_tmp8CE->tag != 12)goto _LL548;else{_tmp8CF=_tmp8CE->f1;_tmp8D0=_tmp8CE->f2;}}_LL547: {
# 3474
struct Cyc_List_List*_tmp933=Cyc_Tcutil_subst_aggrfields(rgn,inst,_tmp8D0);
if(_tmp8D0 == _tmp933)return t;{
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp11FF;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp11FE;return(void*)((_tmp11FE=_cycalloc(sizeof(*_tmp11FE)),((_tmp11FE[0]=((_tmp11FF.tag=12,((_tmp11FF.f1=_tmp8CF,((_tmp11FF.f2=_tmp933,_tmp11FF)))))),_tmp11FE))));};}_LL548: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp8D1=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp8A2;if(_tmp8D1->tag != 1)goto _LL54A;else{_tmp8D2=(void*)_tmp8D1->f2;}}_LL549:
# 3478
 if(_tmp8D2 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8D2);else{
return t;}_LL54A: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp8D3=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp8A2;if(_tmp8D3->tag != 15)goto _LL54C;else{_tmp8D4=(void*)_tmp8D3->f1;}}_LL54B: {
# 3481
void*_tmp936=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8D4);
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp1202;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp1201;return _tmp936 == _tmp8D4?t:(void*)((_tmp1201=_cycalloc(sizeof(*_tmp1201)),((_tmp1201[0]=((_tmp1202.tag=15,((_tmp1202.f1=_tmp936,_tmp1202)))),_tmp1201))));}_LL54C: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp8D5=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp8A2;if(_tmp8D5->tag != 16)goto _LL54E;else{_tmp8D6=(void*)_tmp8D5->f1;_tmp8D7=(void*)_tmp8D5->f2;}}_LL54D: {
# 3484
void*_tmp939=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8D6);
void*_tmp93A=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8D7);
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp1205;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp1204;return _tmp939 == _tmp8D6  && _tmp93A == _tmp8D7?t:(void*)((_tmp1204=_cycalloc(sizeof(*_tmp1204)),((_tmp1204[0]=((_tmp1205.tag=16,((_tmp1205.f1=_tmp939,((_tmp1205.f2=_tmp93A,_tmp1205)))))),_tmp1204))));}_LL54E: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp8D8=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp8A2;if(_tmp8D8->tag != 19)goto _LL550;else{_tmp8D9=(void*)_tmp8D8->f1;}}_LL54F: {
# 3488
void*_tmp93D=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8D9);
struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp1208;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp1207;return _tmp93D == _tmp8D9?t:(void*)((_tmp1207=_cycalloc(sizeof(*_tmp1207)),((_tmp1207[0]=((_tmp1208.tag=19,((_tmp1208.f1=_tmp93D,_tmp1208)))),_tmp1207))));}_LL550: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp8DA=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp8A2;if(_tmp8DA->tag != 18)goto _LL552;else{_tmp8DB=_tmp8DA->f1;}}_LL551: {
# 3491
struct Cyc_Absyn_Exp*_tmp940=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp8DB);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp120B;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp120A;return _tmp940 == _tmp8DB?t:(void*)((_tmp120A=_cycalloc(sizeof(*_tmp120A)),((_tmp120A[0]=((_tmp120B.tag=18,((_tmp120B.f1=_tmp940,_tmp120B)))),_tmp120A))));}_LL552: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp8DC=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp8A2;if(_tmp8DC->tag != 13)goto _LL554;}_LL553:
 goto _LL555;_LL554: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp8DD=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp8A2;if(_tmp8DD->tag != 14)goto _LL556;}_LL555:
 goto _LL557;_LL556: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp8DE=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp8A2;if(_tmp8DE->tag != 0)goto _LL558;}_LL557:
 goto _LL559;_LL558: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8DF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp8A2;if(_tmp8DF->tag != 6)goto _LL55A;}_LL559:
 goto _LL55B;_LL55A: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp8E0=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp8A2;if(_tmp8E0->tag != 7)goto _LL55C;}_LL55B:
 goto _LL55D;_LL55C: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp8E1=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp8A2;if(_tmp8E1->tag != 22)goto _LL55E;}_LL55D:
 goto _LL55F;_LL55E: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp8E2=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp8A2;if(_tmp8E2->tag != 21)goto _LL560;}_LL55F:
 goto _LL561;_LL560: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp8E3=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp8A2;if(_tmp8E3->tag != 20)goto _LL562;}_LL561:
 return t;_LL562: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp8E4=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp8A2;if(_tmp8E4->tag != 25)goto _LL564;else{_tmp8E5=(void*)_tmp8E4->f1;}}_LL563: {
# 3502
void*_tmp943=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8E5);
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp120E;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp120D;return _tmp943 == _tmp8E5?t:(void*)((_tmp120D=_cycalloc(sizeof(*_tmp120D)),((_tmp120D[0]=((_tmp120E.tag=25,((_tmp120E.f1=_tmp943,_tmp120E)))),_tmp120D))));}_LL564: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp8E6=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp8A2;if(_tmp8E6->tag != 23)goto _LL566;else{_tmp8E7=(void*)_tmp8E6->f1;}}_LL565: {
# 3505
void*_tmp946=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8E7);
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1211;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1210;return _tmp946 == _tmp8E7?t:(void*)((_tmp1210=_cycalloc(sizeof(*_tmp1210)),((_tmp1210[0]=((_tmp1211.tag=23,((_tmp1211.f1=_tmp946,_tmp1211)))),_tmp1210))));}_LL566: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp8E8=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp8A2;if(_tmp8E8->tag != 24)goto _LL568;else{_tmp8E9=_tmp8E8->f1;}}_LL567: {
# 3508
struct Cyc_List_List*_tmp949=Cyc_Tcutil_substs(rgn,inst,_tmp8E9);
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1214;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1213;return _tmp949 == _tmp8E9?t:(void*)((_tmp1213=_cycalloc(sizeof(*_tmp1213)),((_tmp1213[0]=((_tmp1214.tag=24,((_tmp1214.f1=_tmp949,_tmp1214)))),_tmp1213))));}_LL568: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp8EA=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp8A2;if(_tmp8EA->tag != 26)goto _LL533;}_LL569: {
const char*_tmp1217;void*_tmp1216;(_tmp1216=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1217="found typedecltype in rsubs",_tag_dyneither(_tmp1217,sizeof(char),28))),_tag_dyneither(_tmp1216,sizeof(void*),0)));}_LL533:;}
# 3514
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 3517
if(ts == 0)
return 0;{
void*_tmp94E=(void*)ts->hd;
struct Cyc_List_List*_tmp94F=ts->tl;
void*_tmp950=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp94E);
struct Cyc_List_List*_tmp951=Cyc_Tcutil_substs(rgn,inst,_tmp94F);
if(_tmp94E == _tmp950  && _tmp94F == _tmp951)
return ts;{
struct Cyc_List_List*_tmp1218;return(_tmp1218=_cycalloc(sizeof(*_tmp1218)),((_tmp1218->hd=_tmp950,((_tmp1218->tl=_tmp951,_tmp1218)))));};};}
# 3528
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 3535
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp953=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
struct Cyc_Core_Opt*_tmp121B;struct _tuple16*_tmp121A;return(_tmp121A=_cycalloc(sizeof(*_tmp121A)),((_tmp121A->f1=tv,((_tmp121A->f2=Cyc_Absyn_new_evar(_tmp953,((_tmp121B=_cycalloc(sizeof(*_tmp121B)),((_tmp121B->v=s,_tmp121B))))),_tmp121A)))));}
# 3540
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*env,struct Cyc_Absyn_Tvar*tv){
# 3542
struct Cyc_List_List*_tmp957;struct _RegionHandle*_tmp958;struct _tuple17*_tmp956=env;_tmp957=_tmp956->f1;_tmp958=_tmp956->f2;{
struct Cyc_Core_Opt*_tmp959=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
struct Cyc_Core_Opt*_tmp121E;struct _tuple16*_tmp121D;return(_tmp121D=_region_malloc(_tmp958,sizeof(*_tmp121D)),((_tmp121D->f1=tv,((_tmp121D->f2=Cyc_Absyn_new_evar(_tmp959,((_tmp121E=_cycalloc(sizeof(*_tmp121E)),((_tmp121E->v=_tmp957,_tmp121E))))),_tmp121D)))));};}
# 3552
static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(unsigned int loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3556
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2)){
const char*_tmp1224;void*_tmp1223[3];struct Cyc_String_pa_PrintArg_struct _tmp1222;struct Cyc_String_pa_PrintArg_struct _tmp1221;struct Cyc_String_pa_PrintArg_struct _tmp1220;(_tmp1220.tag=0,((_tmp1220.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(k2)),((_tmp1221.tag=0,((_tmp1221.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1)),((_tmp1222.tag=0,((_tmp1222.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmp1223[0]=& _tmp1222,((_tmp1223[1]=& _tmp1221,((_tmp1223[2]=& _tmp1220,Cyc_Tcutil_terr(loc,((_tmp1224="type variable %s is used with inconsistent kinds %s and %s",_tag_dyneither(_tmp1224,sizeof(char),59))),_tag_dyneither(_tmp1223,sizeof(void*),3)))))))))))))))))));}
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){
const char*_tmp1227;void*_tmp1226;(_tmp1226=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1227="same type variable has different identity!",_tag_dyneither(_tmp1227,sizeof(char),43))),_tag_dyneither(_tmp1226,sizeof(void*),0)));}}
return tvs;}}}
# 3570
tv->identity=Cyc_Tcutil_new_tvar_id();{
struct Cyc_List_List*_tmp1228;return(_tmp1228=_cycalloc(sizeof(*_tmp1228)),((_tmp1228->hd=tv,((_tmp1228->tl=tvs,_tmp1228)))));};}
# 3576
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3578
if(tv->identity == - 1){
const char*_tmp122B;void*_tmp122A;(_tmp122A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp122B="fast_add_free_tvar: bad identity in tv",_tag_dyneither(_tmp122B,sizeof(char),39))),_tag_dyneither(_tmp122A,sizeof(void*),0)));}
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3582
struct Cyc_Absyn_Tvar*_tmp966=(struct Cyc_Absyn_Tvar*)tvs2->hd;
if(_tmp966->identity == - 1){
const char*_tmp122E;void*_tmp122D;(_tmp122D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp122E="fast_add_free_tvar: bad identity in tvs2",_tag_dyneither(_tmp122E,sizeof(char),41))),_tag_dyneither(_tmp122D,sizeof(void*),0)));}
if(_tmp966->identity == tv->identity)
return tvs;}}{
# 3589
struct Cyc_List_List*_tmp122F;return(_tmp122F=_cycalloc(sizeof(*_tmp122F)),((_tmp122F->hd=tv,((_tmp122F->tl=tvs,_tmp122F)))));};}struct _tuple28{struct Cyc_Absyn_Tvar*f1;int f2;};
# 3595
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 3600
if(tv->identity == - 1){
const char*_tmp1232;void*_tmp1231;(_tmp1231=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1232="fast_add_free_tvar_bool: bad identity in tv",_tag_dyneither(_tmp1232,sizeof(char),44))),_tag_dyneither(_tmp1231,sizeof(void*),0)));}
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3604
struct _tuple28*_tmp96C=(struct _tuple28*)tvs2->hd;struct Cyc_Absyn_Tvar*_tmp96E;int*_tmp96F;struct _tuple28*_tmp96D=_tmp96C;_tmp96E=_tmp96D->f1;_tmp96F=(int*)& _tmp96D->f2;
if(_tmp96E->identity == - 1){
const char*_tmp1235;void*_tmp1234;(_tmp1234=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1235="fast_add_free_tvar_bool: bad identity in tvs2",_tag_dyneither(_tmp1235,sizeof(char),46))),_tag_dyneither(_tmp1234,sizeof(void*),0)));}
if(_tmp96E->identity == tv->identity){
*_tmp96F=*_tmp96F  || b;
return tvs;}}}{
# 3612
struct _tuple28*_tmp1238;struct Cyc_List_List*_tmp1237;return(_tmp1237=_region_malloc(r,sizeof(*_tmp1237)),((_tmp1237->hd=((_tmp1238=_region_malloc(r,sizeof(*_tmp1238)),((_tmp1238->f1=tv,((_tmp1238->f2=b,_tmp1238)))))),((_tmp1237->tl=tvs,_tmp1237)))));};}
# 3616
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3618
if(tv->identity == - 1){
const char*_tmp123C;void*_tmp123B[1];struct Cyc_String_pa_PrintArg_struct _tmp123A;(_tmp123A.tag=0,((_tmp123A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(tv)),((_tmp123B[0]=& _tmp123A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp123C="bound tvar id for %s is NULL",_tag_dyneither(_tmp123C,sizeof(char),29))),_tag_dyneither(_tmp123B,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp123D;return(_tmp123D=_cycalloc(sizeof(*_tmp123D)),((_tmp123D->hd=tv,((_tmp123D->tl=tvs,_tmp123D)))));};}struct _tuple29{void*f1;int f2;};
# 3627
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 3630
void*_tmp978=Cyc_Tcutil_compress(e);void*_tmp979=_tmp978;int _tmp97B;_LL575: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp97A=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp979;if(_tmp97A->tag != 1)goto _LL577;else{_tmp97B=_tmp97A->f3;}}_LL576:
# 3632
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple29*_tmp97C=(struct _tuple29*)es2->hd;void*_tmp97E;int*_tmp97F;struct _tuple29*_tmp97D=_tmp97C;_tmp97E=_tmp97D->f1;_tmp97F=(int*)& _tmp97D->f2;{
void*_tmp980=Cyc_Tcutil_compress(_tmp97E);void*_tmp981=_tmp980;int _tmp983;_LL57A: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp982=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp981;if(_tmp982->tag != 1)goto _LL57C;else{_tmp983=_tmp982->f3;}}_LL57B:
# 3636
 if(_tmp97B == _tmp983){
if(b != *_tmp97F)*_tmp97F=1;
return es;}
# 3640
goto _LL579;_LL57C:;_LL57D:
 goto _LL579;_LL579:;};}}{
# 3644
struct _tuple29*_tmp1240;struct Cyc_List_List*_tmp123F;return(_tmp123F=_region_malloc(r,sizeof(*_tmp123F)),((_tmp123F->hd=((_tmp1240=_region_malloc(r,sizeof(*_tmp1240)),((_tmp1240->f1=e,((_tmp1240->f2=b,_tmp1240)))))),((_tmp123F->tl=es,_tmp123F)))));};_LL577:;_LL578:
 return es;_LL574:;}
# 3649
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3652
struct Cyc_List_List*r=0;
for(0;tvs != 0;tvs=tvs->tl){
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3661
if(!present){struct Cyc_List_List*_tmp1241;r=((_tmp1241=_region_malloc(rgn,sizeof(*_tmp1241)),((_tmp1241->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1241->tl=r,_tmp1241))))));}}
# 3663
r=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);
return r;}
# 3668
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3672
struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp987=*((struct _tuple28*)tvs->hd);struct Cyc_Absyn_Tvar*_tmp989;int _tmp98A;struct _tuple28 _tmp988=_tmp987;_tmp989=_tmp988.f1;_tmp98A=_tmp988.f2;{
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp989->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3682
if(!present){struct Cyc_List_List*_tmp1242;res=((_tmp1242=_region_malloc(r,sizeof(*_tmp1242)),((_tmp1242->hd=(struct _tuple28*)tvs->hd,((_tmp1242->tl=res,_tmp1242))))));}};}
# 3684
res=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);
return res;}
# 3688
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){
# 3690
if(width != 0){
unsigned int w=0;
if(!Cyc_Tcutil_is_const_exp(width)){
const char*_tmp1246;void*_tmp1245[1];struct Cyc_String_pa_PrintArg_struct _tmp1244;(_tmp1244.tag=0,((_tmp1244.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1245[0]=& _tmp1244,Cyc_Tcutil_terr(loc,((_tmp1246="bitfield %s does not have constant width",_tag_dyneither(_tmp1246,sizeof(char),41))),_tag_dyneither(_tmp1245,sizeof(void*),1)))))));}else{
# 3695
struct _tuple14 _tmp98F=Cyc_Evexp_eval_const_uint_exp(width);unsigned int _tmp991;int _tmp992;struct _tuple14 _tmp990=_tmp98F;_tmp991=_tmp990.f1;_tmp992=_tmp990.f2;
if(!_tmp992){
const char*_tmp1249;void*_tmp1248;(_tmp1248=0,Cyc_Tcutil_terr(loc,((_tmp1249="bitfield width cannot use sizeof or offsetof",_tag_dyneither(_tmp1249,sizeof(char),45))),_tag_dyneither(_tmp1248,sizeof(void*),0)));}
w=_tmp991;}{
# 3700
void*_tmp995=Cyc_Tcutil_compress(field_typ);void*_tmp996=_tmp995;enum Cyc_Absyn_Size_of _tmp998;_LL57F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp997=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp996;if(_tmp997->tag != 6)goto _LL581;else{_tmp998=_tmp997->f2;}}_LL580:
# 3703
 switch(_tmp998){case Cyc_Absyn_Char_sz: _LL583:
 if(w > 8){const char*_tmp124C;void*_tmp124B;(_tmp124B=0,Cyc_Tcutil_terr(loc,((_tmp124C="bitfield larger than type",_tag_dyneither(_tmp124C,sizeof(char),26))),_tag_dyneither(_tmp124B,sizeof(void*),0)));}break;case Cyc_Absyn_Short_sz: _LL584:
 if(w > 16){const char*_tmp124F;void*_tmp124E;(_tmp124E=0,Cyc_Tcutil_terr(loc,((_tmp124F="bitfield larger than type",_tag_dyneither(_tmp124F,sizeof(char),26))),_tag_dyneither(_tmp124E,sizeof(void*),0)));}break;case Cyc_Absyn_Long_sz: _LL585:
 goto _LL586;case Cyc_Absyn_Int_sz: _LL586:
# 3708
 if(w > 32){const char*_tmp1252;void*_tmp1251;(_tmp1251=0,Cyc_Tcutil_terr(loc,((_tmp1252="bitfield larger than type",_tag_dyneither(_tmp1252,sizeof(char),26))),_tag_dyneither(_tmp1251,sizeof(void*),0)));}break;case Cyc_Absyn_LongLong_sz: _LL587:
# 3710
 if(w > 64){const char*_tmp1255;void*_tmp1254;(_tmp1254=0,Cyc_Tcutil_terr(loc,((_tmp1255="bitfield larger than type",_tag_dyneither(_tmp1255,sizeof(char),26))),_tag_dyneither(_tmp1254,sizeof(void*),0)));}break;}
# 3712
goto _LL57E;_LL581:;_LL582:
# 3714
{const char*_tmp125A;void*_tmp1259[2];struct Cyc_String_pa_PrintArg_struct _tmp1258;struct Cyc_String_pa_PrintArg_struct _tmp1257;(_tmp1257.tag=0,((_tmp1257.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(field_typ)),((_tmp1258.tag=0,((_tmp1258.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1259[0]=& _tmp1258,((_tmp1259[1]=& _tmp1257,Cyc_Tcutil_terr(loc,((_tmp125A="bitfield %s must have integral type but has type %s",_tag_dyneither(_tmp125A,sizeof(char),52))),_tag_dyneither(_tmp1259,sizeof(void*),2)))))))))))));}
goto _LL57E;_LL57E:;};}}
# 3721
static void Cyc_Tcutil_check_field_atts(unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp9A5=(void*)atts->hd;void*_tmp9A6=_tmp9A5;_LL58A: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp9A7=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp9A6;if(_tmp9A7->tag != 7)goto _LL58C;}_LL58B:
 continue;_LL58C: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp9A8=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp9A6;if(_tmp9A8->tag != 6)goto _LL58E;}_LL58D:
 continue;_LL58E:;_LL58F: {
const char*_tmp125F;void*_tmp125E[2];struct Cyc_String_pa_PrintArg_struct _tmp125D;struct Cyc_String_pa_PrintArg_struct _tmp125C;(_tmp125C.tag=0,((_tmp125C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp125D.tag=0,((_tmp125D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp125E[0]=& _tmp125D,((_tmp125E[1]=& _tmp125C,Cyc_Tcutil_terr(loc,((_tmp125F="bad attribute %s on member %s",_tag_dyneither(_tmp125F,sizeof(char),30))),_tag_dyneither(_tmp125E,sizeof(void*),2)))))))))))));}_LL589:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
# 3745
typedef struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_cvtenv_t;
# 3749
int Cyc_Tcutil_extract_const_from_typedef(unsigned int loc,int declared_const,void*t){
void*_tmp9AD=t;struct Cyc_Absyn_Typedefdecl*_tmp9AF;void*_tmp9B0;_LL591: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp9AE=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9AD;if(_tmp9AE->tag != 17)goto _LL593;else{_tmp9AF=_tmp9AE->f3;_tmp9B0=(void*)_tmp9AE->f4;}}_LL592:
# 3752
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp9AF))->tq).real_const  || (_tmp9AF->tq).print_const){
if(declared_const){const char*_tmp1262;void*_tmp1261;(_tmp1261=0,Cyc_Tcutil_warn(loc,((_tmp1262="extra const",_tag_dyneither(_tmp1262,sizeof(char),12))),_tag_dyneither(_tmp1261,sizeof(void*),0)));}
return 1;}
# 3757
if((unsigned int)_tmp9B0)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp9B0);else{
return declared_const;}_LL593:;_LL594:
 return declared_const;_LL590:;}
# 3764
static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td != 0){
if(td->defn != 0){
void*_tmp9B3=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_tmp9B4=_tmp9B3;void*_tmp9B6;_LL596: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9B5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9B4;if(_tmp9B5->tag != 5)goto _LL598;else{_tmp9B6=((_tmp9B5->f1).ptr_atts).rgn;}}_LL597:
# 3769
{void*_tmp9B7=Cyc_Tcutil_compress(_tmp9B6);void*_tmp9B8=_tmp9B7;struct Cyc_Absyn_Tvar*_tmp9BA;_LL59B: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp9B9=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9B8;if(_tmp9B9->tag != 2)goto _LL59D;else{_tmp9BA=_tmp9B9->f1;}}_LL59C:
# 3771
 return Cyc_Absyn_tvar_cmp(tvar,_tmp9BA)== 0;_LL59D:;_LL59E:
 goto _LL59A;_LL59A:;}
# 3774
goto _LL595;_LL598:;_LL599:
 goto _LL595;_LL595:;}}else{
# 3780
return 1;}
return 0;}
# 3784
static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 3787
void*_tmp9BB=Cyc_Absyn_compress_kb(tvar->kind);void*_tmp9BC=_tmp9BB;_LL5A0: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9BD=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9BC;if(_tmp9BD->tag != 2)goto _LL5A2;else{if((_tmp9BD->f2)->kind != Cyc_Absyn_RgnKind)goto _LL5A2;if((_tmp9BD->f2)->aliasqual != Cyc_Absyn_Top)goto _LL5A2;}}_LL5A1:
 goto _LL5A3;_LL5A2: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp9BE=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp9BC;if(_tmp9BE->tag != 0)goto _LL5A4;else{if((_tmp9BE->f1)->kind != Cyc_Absyn_RgnKind)goto _LL5A4;if((_tmp9BE->f1)->aliasqual != Cyc_Absyn_Top)goto _LL5A4;}}_LL5A3:
# 3796
 if(((expected_kind->kind == Cyc_Absyn_BoxKind  || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind) && 
# 3799
Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){
if(expected_kind->aliasqual == Cyc_Absyn_Aliasable)
return& Cyc_Tcutil_rk;else{
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}
# 3805
return& Cyc_Tcutil_trk;_LL5A4:;_LL5A5:
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);_LL59F:;}
# 3811
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);struct _tuple30{struct Cyc_Tcutil_CVTEnv f1;struct Cyc_List_List*f2;};
# 3815
static struct _tuple30 Cyc_Tcutil_check_clause(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct _dyneither_ptr clause_name,struct Cyc_Absyn_Exp*clause){
# 3818
struct Cyc_List_List*relns=0;
if(clause != 0){
Cyc_Tcexp_tcExp(te,0,clause);
if(!Cyc_Tcutil_is_integral(clause)){
const char*_tmp1267;void*_tmp1266[2];struct Cyc_String_pa_PrintArg_struct _tmp1265;struct Cyc_String_pa_PrintArg_struct _tmp1264;(_tmp1264.tag=0,((_tmp1264.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(clause->topt))),((_tmp1265.tag=0,((_tmp1265.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name),((_tmp1266[0]=& _tmp1265,((_tmp1266[1]=& _tmp1264,Cyc_Tcutil_terr(loc,((_tmp1267="%s clause has type %s instead of integral type",_tag_dyneither(_tmp1267,sizeof(char),47))),_tag_dyneither(_tmp1266,sizeof(void*),2)))))))))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(clause,te,cvtenv);
relns=Cyc_Relations_exp2relns(Cyc_Core_heap_region,clause);
if(!Cyc_Relations_consistent_relations(relns)){
const char*_tmp126C;void*_tmp126B[2];struct Cyc_String_pa_PrintArg_struct _tmp126A;struct Cyc_String_pa_PrintArg_struct _tmp1269;(_tmp1269.tag=0,((_tmp1269.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(clause)),((_tmp126A.tag=0,((_tmp126A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name),((_tmp126B[0]=& _tmp126A,((_tmp126B[1]=& _tmp1269,Cyc_Tcutil_terr(clause->loc,((_tmp126C="%s clause '%s' may be unsatisfiable",_tag_dyneither(_tmp126C,sizeof(char),36))),_tag_dyneither(_tmp126B,sizeof(void*),2)))))))))))));}}{
# 3830
struct _tuple30 _tmp126D;return(_tmp126D.f1=cvtenv,((_tmp126D.f2=relns,_tmp126D)));};}struct _tuple31{enum Cyc_Absyn_Format_Type f1;void*f2;};
# 3862 "tcutil.cyc"
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect){
# 3869
{void*_tmp9C8=Cyc_Tcutil_compress(t);void*_tmp9C9=_tmp9C8;struct Cyc_Core_Opt**_tmp9CC;void**_tmp9CD;struct Cyc_Absyn_Tvar*_tmp9CF;void*_tmp9D1;void***_tmp9D2;struct Cyc_List_List*_tmp9D4;struct _tuple2*_tmp9D6;struct Cyc_Absyn_Enumdecl**_tmp9D7;union Cyc_Absyn_DatatypeInfoU*_tmp9D9;struct Cyc_List_List**_tmp9DA;union Cyc_Absyn_DatatypeFieldInfoU*_tmp9DC;struct Cyc_List_List*_tmp9DD;void*_tmp9DF;struct Cyc_Absyn_Tqual*_tmp9E0;void*_tmp9E1;union Cyc_Absyn_Constraint*_tmp9E2;union Cyc_Absyn_Constraint*_tmp9E3;union Cyc_Absyn_Constraint*_tmp9E4;void*_tmp9E6;struct Cyc_Absyn_Exp*_tmp9E8;void*_tmp9EC;struct Cyc_Absyn_Tqual*_tmp9ED;struct Cyc_Absyn_Exp**_tmp9EE;union Cyc_Absyn_Constraint*_tmp9EF;unsigned int _tmp9F0;struct Cyc_List_List**_tmp9F2;void**_tmp9F3;struct Cyc_Absyn_Tqual*_tmp9F4;void*_tmp9F5;struct Cyc_List_List*_tmp9F6;int _tmp9F7;struct Cyc_Absyn_VarargInfo*_tmp9F8;struct Cyc_List_List*_tmp9F9;struct Cyc_List_List*_tmp9FA;struct Cyc_Absyn_Exp*_tmp9FB;struct Cyc_List_List**_tmp9FC;struct Cyc_Absyn_Exp*_tmp9FD;struct Cyc_List_List**_tmp9FE;struct Cyc_List_List*_tmpA00;enum Cyc_Absyn_AggrKind _tmpA02;struct Cyc_List_List*_tmpA03;union Cyc_Absyn_AggrInfoU*_tmpA05;struct Cyc_List_List**_tmpA06;struct _tuple2*_tmpA08;struct Cyc_List_List**_tmpA09;struct Cyc_Absyn_Typedefdecl**_tmpA0A;void**_tmpA0B;void*_tmpA10;void*_tmpA12;void*_tmpA13;void*_tmpA15;void*_tmpA17;struct Cyc_List_List*_tmpA19;_LL5A7: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp9CA=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp9C9;if(_tmp9CA->tag != 0)goto _LL5A9;}_LL5A8:
 goto _LL5A6;_LL5A9: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp9CB=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp9C9;if(_tmp9CB->tag != 1)goto _LL5AB;else{_tmp9CC=(struct Cyc_Core_Opt**)& _tmp9CB->f1;_tmp9CD=(void**)((void**)& _tmp9CB->f2);}}_LL5AA:
# 3873
 if(*_tmp9CC == 0  || 
Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp9CC))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp9CC))->v,expected_kind))
*_tmp9CC=Cyc_Tcutil_kind_to_opt(expected_kind);
if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){
# 3878
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
*_tmp9CD=(void*)& Cyc_Absyn_UniqueRgn_val;else{
# 3881
*_tmp9CD=(void*)& Cyc_Absyn_HeapRgn_val;}}else{
if(cvtenv.generalize_evars){
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1270;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp126F;struct Cyc_Absyn_Tvar*_tmpA1A=Cyc_Tcutil_new_tvar((void*)((_tmp126F=_cycalloc(sizeof(*_tmp126F)),((_tmp126F[0]=((_tmp1270.tag=2,((_tmp1270.f1=0,((_tmp1270.f2=expected_kind,_tmp1270)))))),_tmp126F)))));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1273;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1272;*_tmp9CD=(void*)((_tmp1272=_cycalloc(sizeof(*_tmp1272)),((_tmp1272[0]=((_tmp1273.tag=2,((_tmp1273.f1=_tmpA1A,_tmp1273)))),_tmp1272))));}
_tmp9CF=_tmpA1A;goto _LL5AC;}else{
# 3887
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}
# 3889
goto _LL5A6;_LL5AB: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp9CE=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9C9;if(_tmp9CE->tag != 2)goto _LL5AD;else{_tmp9CF=_tmp9CE->f1;}}_LL5AC:
# 3891
{void*_tmpA1F=Cyc_Absyn_compress_kb(_tmp9CF->kind);void*_tmpA20=_tmpA1F;struct Cyc_Core_Opt**_tmpA22;_LL5DE: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA21=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA20;if(_tmpA21->tag != 1)goto _LL5E0;else{_tmpA22=(struct Cyc_Core_Opt**)& _tmpA21->f1;}}_LL5DF:
# 3893
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1279;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1278;struct Cyc_Core_Opt*_tmp1277;*_tmpA22=((_tmp1277=_cycalloc(sizeof(*_tmp1277)),((_tmp1277->v=(void*)((_tmp1279=_cycalloc(sizeof(*_tmp1279)),((_tmp1279[0]=((_tmp1278.tag=2,((_tmp1278.f1=0,((_tmp1278.f2=expected_kind,_tmp1278)))))),_tmp1279)))),_tmp1277))));}goto _LL5DD;_LL5E0:;_LL5E1:
 goto _LL5DD;_LL5DD:;}
# 3898
cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp9CF);
# 3901
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp9CF,put_in_effect);
# 3903
{void*_tmpA26=Cyc_Absyn_compress_kb(_tmp9CF->kind);void*_tmpA27=_tmpA26;struct Cyc_Core_Opt**_tmpA29;struct Cyc_Absyn_Kind*_tmpA2A;_LL5E3: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA28=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA27;if(_tmpA28->tag != 2)goto _LL5E5;else{_tmpA29=(struct Cyc_Core_Opt**)& _tmpA28->f1;_tmpA2A=_tmpA28->f2;}}_LL5E4:
# 3905
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmpA2A)){
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp127F;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp127E;struct Cyc_Core_Opt*_tmp127D;*_tmpA29=((_tmp127D=_cycalloc(sizeof(*_tmp127D)),((_tmp127D->v=(void*)((_tmp127F=_cycalloc(sizeof(*_tmp127F)),((_tmp127F[0]=((_tmp127E.tag=2,((_tmp127E.f1=0,((_tmp127E.f2=expected_kind,_tmp127E)))))),_tmp127F)))),_tmp127D))));}
goto _LL5E2;_LL5E5:;_LL5E6:
 goto _LL5E2;_LL5E2:;}
# 3910
goto _LL5A6;_LL5AD: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp9D0=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp9C9;if(_tmp9D0->tag != 26)goto _LL5AF;else{_tmp9D1=(_tmp9D0->f1)->r;_tmp9D2=(void***)& _tmp9D0->f2;}}_LL5AE: {
# 3916
void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmpA2E=_tmp9D1;struct Cyc_Absyn_Aggrdecl*_tmpA30;struct Cyc_Absyn_Enumdecl*_tmpA32;struct Cyc_Absyn_Datatypedecl*_tmpA34;_LL5E8: {struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmpA2F=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmpA2E;if(_tmpA2F->tag != 0)goto _LL5EA;else{_tmpA30=_tmpA2F->f1;}}_LL5E9:
# 3919
 Cyc_Tc_tcAggrdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmpA30);goto _LL5E7;_LL5EA: {struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpA31=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmpA2E;if(_tmpA31->tag != 1)goto _LL5EC;else{_tmpA32=_tmpA31->f1;}}_LL5EB:
# 3921
 Cyc_Tc_tcEnumdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmpA32);goto _LL5E7;_LL5EC: {struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmpA33=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmpA2E;if(_tmpA33->tag != 2)goto _LL5E7;else{_tmpA34=_tmpA33->f1;}}_LL5ED:
# 3923
 Cyc_Tc_tcDatatypedecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmpA34);goto _LL5E7;_LL5E7:;}
# 3925
{void**_tmp1280;*_tmp9D2=((_tmp1280=_cycalloc(sizeof(*_tmp1280)),((_tmp1280[0]=new_t,_tmp1280))));}
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect);}_LL5AF: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp9D3=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp9C9;if(_tmp9D3->tag != 14)goto _LL5B1;else{_tmp9D4=_tmp9D3->f1;}}_LL5B0: {
# 3930
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct _RegionHandle _tmpA36=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmpA36;_push_region(uprev_rgn);{
struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;
for(0;_tmp9D4 != 0;_tmp9D4=_tmp9D4->tl){
struct Cyc_Absyn_Enumfield*_tmpA37=(struct Cyc_Absyn_Enumfield*)_tmp9D4->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmpA37->name).f2)){
const char*_tmp1284;void*_tmp1283[1];struct Cyc_String_pa_PrintArg_struct _tmp1282;(_tmp1282.tag=0,((_tmp1282.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpA37->name).f2),((_tmp1283[0]=& _tmp1282,Cyc_Tcutil_terr(_tmpA37->loc,((_tmp1284="duplicate enum field name %s",_tag_dyneither(_tmp1284,sizeof(char),29))),_tag_dyneither(_tmp1283,sizeof(void*),1)))))));}else{
# 3939
struct Cyc_List_List*_tmp1285;prev_fields=((_tmp1285=_region_malloc(uprev_rgn,sizeof(*_tmp1285)),((_tmp1285->hd=(*_tmpA37->name).f2,((_tmp1285->tl=prev_fields,_tmp1285))))));}
# 3941
if(_tmpA37->tag == 0)
_tmpA37->tag=Cyc_Absyn_uint_exp(tag_count,_tmpA37->loc);else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmpA37->tag))){
const char*_tmp1289;void*_tmp1288[1];struct Cyc_String_pa_PrintArg_struct _tmp1287;(_tmp1287.tag=0,((_tmp1287.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpA37->name).f2),((_tmp1288[0]=& _tmp1287,Cyc_Tcutil_terr(loc,((_tmp1289="enum field %s: expression is not constant",_tag_dyneither(_tmp1289,sizeof(char),42))),_tag_dyneither(_tmp1288,sizeof(void*),1)))))));}}{
# 3946
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmpA37->tag))).f1;
tag_count=t1 + 1;
{union Cyc_Absyn_Nmspace _tmpA3F=(*_tmpA37->name).f1;union Cyc_Absyn_Nmspace _tmpA40=_tmpA3F;_LL5EF: if((_tmpA40.Rel_n).tag != 1)goto _LL5F1;_LL5F0:
# 3950
(*_tmpA37->name).f1=Cyc_Absyn_Abs_n(te->ns,0);goto _LL5EE;_LL5F1:;_LL5F2:
 goto _LL5EE;_LL5EE:;}{
# 3953
struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp128F;struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct _tmp128E;struct _tuple29*_tmp128D;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple29*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmpA37->name).f2,(
(_tmp128D=_cycalloc(sizeof(*_tmp128D)),((_tmp128D->f1=(void*)((_tmp128F=_cycalloc(sizeof(*_tmp128F)),((_tmp128F[0]=((_tmp128E.tag=4,((_tmp128E.f1=t,((_tmp128E.f2=_tmpA37,_tmp128E)))))),_tmp128F)))),((_tmp128D->f2=1,_tmp128D)))))));};};}}
# 3957
_npop_handler(0);goto _LL5A6;
# 3931
;_pop_region(uprev_rgn);}_LL5B1: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp9D5=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp9C9;if(_tmp9D5->tag != 13)goto _LL5B3;else{_tmp9D6=_tmp9D5->f1;_tmp9D7=(struct Cyc_Absyn_Enumdecl**)& _tmp9D5->f2;}}_LL5B2:
# 3959
 if(*_tmp9D7 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp9D7))->fields == 0){
struct _handler_cons _tmpA44;_push_handler(& _tmpA44);{int _tmpA46=0;if(setjmp(_tmpA44.handler))_tmpA46=1;if(!_tmpA46){
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp9D6);
*_tmp9D7=*ed;}
# 3961
;_pop_handler();}else{void*_tmpA45=(void*)_exn_thrown;void*_tmpA48=_tmpA45;void*_tmpA4A;_LL5F4: {struct Cyc_Dict_Absent_exn_struct*_tmpA49=(struct Cyc_Dict_Absent_exn_struct*)_tmpA48;if(_tmpA49->tag != Cyc_Dict_Absent)goto _LL5F6;}_LL5F5: {
# 3965
struct Cyc_Tcenv_Genv*_tmpA4B=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp1290;struct Cyc_Absyn_Enumdecl*_tmpA4C=(_tmp1290=_cycalloc(sizeof(*_tmp1290)),((_tmp1290->sc=Cyc_Absyn_Extern,((_tmp1290->name=_tmp9D6,((_tmp1290->fields=0,_tmp1290)))))));
Cyc_Tc_tcEnumdecl(te,_tmpA4B,loc,_tmpA4C);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp9D6);
*_tmp9D7=*ed;
goto _LL5F3;};}_LL5F6: _tmpA4A=_tmpA48;_LL5F7:(void)_rethrow(_tmpA4A);_LL5F3:;}};}{
# 3974
struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp9D7);
# 3976
*_tmp9D6=(ed->name)[0];
goto _LL5A6;};_LL5B3: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp9D8=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp9C9;if(_tmp9D8->tag != 3)goto _LL5B5;else{_tmp9D9=(union Cyc_Absyn_DatatypeInfoU*)&(_tmp9D8->f1).datatype_info;_tmp9DA=(struct Cyc_List_List**)&(_tmp9D8->f1).targs;}}_LL5B4: {
# 3979
struct Cyc_List_List*_tmpA4E=*_tmp9DA;
{union Cyc_Absyn_DatatypeInfoU _tmpA4F=*_tmp9D9;union Cyc_Absyn_DatatypeInfoU _tmpA50=_tmpA4F;struct _tuple2*_tmpA51;int _tmpA52;struct Cyc_Absyn_Datatypedecl*_tmpA53;_LL5F9: if((_tmpA50.UnknownDatatype).tag != 1)goto _LL5FB;_tmpA51=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpA50.UnknownDatatype).val).name;_tmpA52=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpA50.UnknownDatatype).val).is_extensible;_LL5FA: {
# 3982
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmpA54;_push_handler(& _tmpA54);{int _tmpA56=0;if(setjmp(_tmpA54.handler))_tmpA56=1;if(!_tmpA56){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpA51);;_pop_handler();}else{void*_tmpA55=(void*)_exn_thrown;void*_tmpA58=_tmpA55;void*_tmpA5A;_LL5FE: {struct Cyc_Dict_Absent_exn_struct*_tmpA59=(struct Cyc_Dict_Absent_exn_struct*)_tmpA58;if(_tmpA59->tag != Cyc_Dict_Absent)goto _LL600;}_LL5FF: {
# 3986
struct Cyc_Tcenv_Genv*_tmpA5B=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Datatypedecl*_tmp1291;struct Cyc_Absyn_Datatypedecl*_tmpA5C=(_tmp1291=_cycalloc(sizeof(*_tmp1291)),((_tmp1291->sc=Cyc_Absyn_Extern,((_tmp1291->name=_tmpA51,((_tmp1291->tvs=0,((_tmp1291->fields=0,((_tmp1291->is_extensible=_tmpA52,_tmp1291)))))))))));
Cyc_Tc_tcDatatypedecl(te,_tmpA5B,loc,_tmpA5C);
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpA51);
# 3991
if(_tmpA4E != 0){
{const char*_tmp1295;void*_tmp1294[1];struct Cyc_String_pa_PrintArg_struct _tmp1293;(_tmp1293.tag=0,((_tmp1293.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA51)),((_tmp1294[0]=& _tmp1293,Cyc_Tcutil_terr(loc,((_tmp1295="declare parameterized datatype %s before using",_tag_dyneither(_tmp1295,sizeof(char),47))),_tag_dyneither(_tmp1294,sizeof(void*),1)))))));}
return cvtenv;}
# 3996
goto _LL5FD;}_LL600: _tmpA5A=_tmpA58;_LL601:(void)_rethrow(_tmpA5A);_LL5FD:;}};}
# 4002
if(_tmpA52  && !(*tudp)->is_extensible){
const char*_tmp1299;void*_tmp1298[1];struct Cyc_String_pa_PrintArg_struct _tmp1297;(_tmp1297.tag=0,((_tmp1297.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA51)),((_tmp1298[0]=& _tmp1297,Cyc_Tcutil_terr(loc,((_tmp1299="datatype %s was not declared @extensible",_tag_dyneither(_tmp1299,sizeof(char),41))),_tag_dyneither(_tmp1298,sizeof(void*),1)))))));}
*_tmp9D9=Cyc_Absyn_KnownDatatype(tudp);
_tmpA53=*tudp;goto _LL5FC;}_LL5FB: if((_tmpA50.KnownDatatype).tag != 2)goto _LL5F8;_tmpA53=*((struct Cyc_Absyn_Datatypedecl**)(_tmpA50.KnownDatatype).val);_LL5FC: {
# 4009
struct Cyc_List_List*tvs=_tmpA53->tvs;
for(0;_tmpA4E != 0  && tvs != 0;(_tmpA4E=_tmpA4E->tl,tvs=tvs->tl)){
void*t=(void*)_tmpA4E->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 4015
{struct _tuple0 _tmp129A;struct _tuple0 _tmpA64=(_tmp129A.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp129A.f2=t,_tmp129A)));struct _tuple0 _tmpA65=_tmpA64;struct Cyc_Absyn_Tvar*_tmpA68;_LL603:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA66=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA65.f1;if(_tmpA66->tag != 1)goto _LL605;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA67=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA65.f2;if(_tmpA67->tag != 2)goto _LL605;else{_tmpA68=_tmpA67->f1;}};_LL604:
# 4017
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpA68);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpA68,1);
continue;_LL605:;_LL606:
 goto _LL602;_LL602:;}{
# 4022
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 4026
if(_tmpA4E != 0){
const char*_tmp129E;void*_tmp129D[1];struct Cyc_String_pa_PrintArg_struct _tmp129C;(_tmp129C.tag=0,((_tmp129C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA53->name)),((_tmp129D[0]=& _tmp129C,Cyc_Tcutil_terr(loc,((_tmp129E="too many type arguments for datatype %s",_tag_dyneither(_tmp129E,sizeof(char),40))),_tag_dyneither(_tmp129D,sizeof(void*),1)))))));}
if(tvs != 0){
# 4031
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp129F;hidden_ts=((_tmp129F=_cycalloc(sizeof(*_tmp129F)),((_tmp129F->hd=e,((_tmp129F->tl=hidden_ts,_tmp129F))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1);}
# 4038
*_tmp9DA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp9DA,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4040
goto _LL5F8;}_LL5F8:;}
# 4042
goto _LL5A6;}_LL5B5: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp9DB=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp9C9;if(_tmp9DB->tag != 4)goto _LL5B7;else{_tmp9DC=(union Cyc_Absyn_DatatypeFieldInfoU*)&(_tmp9DB->f1).field_info;_tmp9DD=(_tmp9DB->f1).targs;}}_LL5B6:
# 4045
{union Cyc_Absyn_DatatypeFieldInfoU _tmpA6E=*_tmp9DC;union Cyc_Absyn_DatatypeFieldInfoU _tmpA6F=_tmpA6E;struct _tuple2*_tmpA70;struct _tuple2*_tmpA71;int _tmpA72;struct Cyc_Absyn_Datatypedecl*_tmpA73;struct Cyc_Absyn_Datatypefield*_tmpA74;_LL608: if((_tmpA6F.UnknownDatatypefield).tag != 1)goto _LL60A;_tmpA70=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpA6F.UnknownDatatypefield).val).datatype_name;_tmpA71=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpA6F.UnknownDatatypefield).val).field_name;_tmpA72=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpA6F.UnknownDatatypefield).val).is_extensible;_LL609: {
# 4047
struct Cyc_Absyn_Datatypedecl*tud;
struct Cyc_Absyn_Datatypefield*tuf;
{struct _handler_cons _tmpA75;_push_handler(& _tmpA75);{int _tmpA77=0;if(setjmp(_tmpA75.handler))_tmpA77=1;if(!_tmpA77){*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpA70);;_pop_handler();}else{void*_tmpA76=(void*)_exn_thrown;void*_tmpA79=_tmpA76;void*_tmpA7B;_LL60D: {struct Cyc_Dict_Absent_exn_struct*_tmpA7A=(struct Cyc_Dict_Absent_exn_struct*)_tmpA79;if(_tmpA7A->tag != Cyc_Dict_Absent)goto _LL60F;}_LL60E:
# 4051
{const char*_tmp12A3;void*_tmp12A2[1];struct Cyc_String_pa_PrintArg_struct _tmp12A1;(_tmp12A1.tag=0,((_tmp12A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA70)),((_tmp12A2[0]=& _tmp12A1,Cyc_Tcutil_terr(loc,((_tmp12A3="unbound datatype %s",_tag_dyneither(_tmp12A3,sizeof(char),20))),_tag_dyneither(_tmp12A2,sizeof(void*),1)))))));}
return cvtenv;_LL60F: _tmpA7B=_tmpA79;_LL610:(void)_rethrow(_tmpA7B);_LL60C:;}};}
# 4054
{struct _handler_cons _tmpA7F;_push_handler(& _tmpA7F);{int _tmpA81=0;if(setjmp(_tmpA7F.handler))_tmpA81=1;if(!_tmpA81){
{struct _RegionHandle _tmpA82=_new_region("r");struct _RegionHandle*r=& _tmpA82;_push_region(r);
{void*_tmpA83=Cyc_Tcenv_lookup_ordinary(r,te,loc,_tmpA71,0);void*_tmpA84=_tmpA83;struct Cyc_Absyn_Datatypedecl*_tmpA86;struct Cyc_Absyn_Datatypefield*_tmpA87;_LL612: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpA85=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpA84;if(_tmpA85->tag != 2)goto _LL614;else{_tmpA86=_tmpA85->f1;_tmpA87=_tmpA85->f2;}}_LL613:
# 4058
 tuf=_tmpA87;
tud=_tmpA86;
if(_tmpA72  && !tud->is_extensible){
const char*_tmp12A7;void*_tmp12A6[1];struct Cyc_String_pa_PrintArg_struct _tmp12A5;(_tmp12A5.tag=0,((_tmp12A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA70)),((_tmp12A6[0]=& _tmp12A5,Cyc_Tcutil_terr(loc,((_tmp12A7="datatype %s was not declared @extensible",_tag_dyneither(_tmp12A7,sizeof(char),41))),_tag_dyneither(_tmp12A6,sizeof(void*),1)))))));}
goto _LL611;_LL614:;_LL615:
{const char*_tmp12AC;void*_tmp12AB[2];struct Cyc_String_pa_PrintArg_struct _tmp12AA;struct Cyc_String_pa_PrintArg_struct _tmp12A9;(_tmp12A9.tag=0,((_tmp12A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA70)),((_tmp12AA.tag=0,((_tmp12AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA71)),((_tmp12AB[0]=& _tmp12AA,((_tmp12AB[1]=& _tmp12A9,Cyc_Tcutil_terr(loc,((_tmp12AC="unbound field %s in type datatype %s",_tag_dyneither(_tmp12AC,sizeof(char),37))),_tag_dyneither(_tmp12AB,sizeof(void*),2)))))))))))));}{
struct Cyc_Tcutil_CVTEnv _tmpA8F=cvtenv;_npop_handler(1);return _tmpA8F;};_LL611:;}
# 4056
;_pop_region(r);}
# 4055
;_pop_handler();}else{void*_tmpA80=(void*)_exn_thrown;void*_tmpA91=_tmpA80;void*_tmpA93;_LL617: {struct Cyc_Dict_Absent_exn_struct*_tmpA92=(struct Cyc_Dict_Absent_exn_struct*)_tmpA91;if(_tmpA92->tag != Cyc_Dict_Absent)goto _LL619;}_LL618:
# 4071
{const char*_tmp12B1;void*_tmp12B0[2];struct Cyc_String_pa_PrintArg_struct _tmp12AF;struct Cyc_String_pa_PrintArg_struct _tmp12AE;(_tmp12AE.tag=0,((_tmp12AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA70)),((_tmp12AF.tag=0,((_tmp12AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA71)),((_tmp12B0[0]=& _tmp12AF,((_tmp12B0[1]=& _tmp12AE,Cyc_Tcutil_terr(loc,((_tmp12B1="unbound field %s in type datatype %s",_tag_dyneither(_tmp12B1,sizeof(char),37))),_tag_dyneither(_tmp12B0,sizeof(void*),2)))))))))))));}
return cvtenv;_LL619: _tmpA93=_tmpA91;_LL61A:(void)_rethrow(_tmpA93);_LL616:;}};}
# 4075
*_tmp9DC=Cyc_Absyn_KnownDatatypefield(tud,tuf);
_tmpA73=tud;_tmpA74=tuf;goto _LL60B;}_LL60A: if((_tmpA6F.KnownDatatypefield).tag != 2)goto _LL607;_tmpA73=((struct _tuple3)(_tmpA6F.KnownDatatypefield).val).f1;_tmpA74=((struct _tuple3)(_tmpA6F.KnownDatatypefield).val).f2;_LL60B: {
# 4079
struct Cyc_List_List*tvs=_tmpA73->tvs;
for(0;_tmp9DD != 0  && tvs != 0;(_tmp9DD=_tmp9DD->tl,tvs=tvs->tl)){
void*t=(void*)_tmp9DD->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 4085
{struct _tuple0 _tmp12B2;struct _tuple0 _tmpA98=(_tmp12B2.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp12B2.f2=t,_tmp12B2)));struct _tuple0 _tmpA99=_tmpA98;struct Cyc_Absyn_Tvar*_tmpA9C;_LL61C:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA9A=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA99.f1;if(_tmpA9A->tag != 1)goto _LL61E;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA9B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA99.f2;if(_tmpA9B->tag != 2)goto _LL61E;else{_tmpA9C=_tmpA9B->f1;}};_LL61D:
# 4087
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpA9C);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpA9C,1);
continue;_LL61E:;_LL61F:
 goto _LL61B;_LL61B:;}{
# 4092
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 4096
if(_tmp9DD != 0){
const char*_tmp12B7;void*_tmp12B6[2];struct Cyc_String_pa_PrintArg_struct _tmp12B5;struct Cyc_String_pa_PrintArg_struct _tmp12B4;(_tmp12B4.tag=0,((_tmp12B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA74->name)),((_tmp12B5.tag=0,((_tmp12B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA73->name)),((_tmp12B6[0]=& _tmp12B5,((_tmp12B6[1]=& _tmp12B4,Cyc_Tcutil_terr(loc,((_tmp12B7="too many type arguments for datatype %s.%s",_tag_dyneither(_tmp12B7,sizeof(char),43))),_tag_dyneither(_tmp12B6,sizeof(void*),2)))))))))))));}
if(tvs != 0){
const char*_tmp12BC;void*_tmp12BB[2];struct Cyc_String_pa_PrintArg_struct _tmp12BA;struct Cyc_String_pa_PrintArg_struct _tmp12B9;(_tmp12B9.tag=0,((_tmp12B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA74->name)),((_tmp12BA.tag=0,((_tmp12BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA73->name)),((_tmp12BB[0]=& _tmp12BA,((_tmp12BB[1]=& _tmp12B9,Cyc_Tcutil_terr(loc,((_tmp12BC="too few type arguments for datatype %s.%s",_tag_dyneither(_tmp12BC,sizeof(char),42))),_tag_dyneither(_tmp12BB,sizeof(void*),2)))))))))))));}
goto _LL607;}_LL607:;}
# 4104
goto _LL5A6;_LL5B7: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9DE=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9C9;if(_tmp9DE->tag != 5)goto _LL5B9;else{_tmp9DF=(_tmp9DE->f1).elt_typ;_tmp9E0=(struct Cyc_Absyn_Tqual*)&(_tmp9DE->f1).elt_tq;_tmp9E1=((_tmp9DE->f1).ptr_atts).rgn;_tmp9E2=((_tmp9DE->f1).ptr_atts).nullable;_tmp9E3=((_tmp9DE->f1).ptr_atts).bounds;_tmp9E4=((_tmp9DE->f1).ptr_atts).zero_term;}}_LL5B8: {
# 4107
int is_zero_terminated;
# 4109
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp9DF,1);
_tmp9E0->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp9E0->print_const,_tmp9DF);{
struct Cyc_Absyn_Kind*k;
{enum Cyc_Absyn_AliasQual _tmpAA6=expected_kind->aliasqual;switch(_tmpAA6){case Cyc_Absyn_Aliasable: _LL620:
 k=& Cyc_Tcutil_rk;break;case Cyc_Absyn_Unique: _LL621:
 k=& Cyc_Tcutil_urk;break;case Cyc_Absyn_Top: _LL622:
 k=& Cyc_Tcutil_trk;break;}}
# 4117
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmp9E1,1);
{union Cyc_Absyn_Constraint*_tmpAA7=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp9E4);union Cyc_Absyn_Constraint*_tmpAA8=_tmpAA7;_LL625: if((_tmpAA8->No_constr).tag != 3)goto _LL627;_LL626:
# 4122
{void*_tmpAA9=Cyc_Tcutil_compress(_tmp9DF);void*_tmpAAA=_tmpAA9;_LL62C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpAAB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpAAA;if(_tmpAAB->tag != 6)goto _LL62E;else{if(_tmpAAB->f2 != Cyc_Absyn_Char_sz)goto _LL62E;}}_LL62D:
# 4124
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp9E4,Cyc_Absyn_true_conref);
is_zero_terminated=1;
goto _LL62B;_LL62E:;_LL62F:
# 4128
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp9E4,Cyc_Absyn_false_conref);
is_zero_terminated=0;
goto _LL62B;_LL62B:;}
# 4132
goto _LL624;_LL627: if((_tmpAA8->Eq_constr).tag != 1)goto _LL629;if((int)(_tmpAA8->Eq_constr).val != 1)goto _LL629;_LL628:
# 4135
 if(!Cyc_Tcutil_admits_zero(_tmp9DF)){
const char*_tmp12C0;void*_tmp12BF[1];struct Cyc_String_pa_PrintArg_struct _tmp12BE;(_tmp12BE.tag=0,((_tmp12BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp9DF)),((_tmp12BF[0]=& _tmp12BE,Cyc_Tcutil_terr(loc,((_tmp12C0="cannot have a pointer to zero-terminated %s elements",_tag_dyneither(_tmp12C0,sizeof(char),53))),_tag_dyneither(_tmp12BF,sizeof(void*),1)))))));}
is_zero_terminated=1;
goto _LL624;_LL629:;_LL62A:
# 4141
 is_zero_terminated=0;
goto _LL624;_LL624:;}
# 4145
{void*_tmpAAF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp9E3);void*_tmpAB0=_tmpAAF;struct Cyc_Absyn_Exp*_tmpAB3;_LL631: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpAB1=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpAB0;if(_tmpAB1->tag != 0)goto _LL633;}_LL632:
 goto _LL630;_LL633: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpAB2=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpAB0;if(_tmpAB2->tag != 1)goto _LL630;else{_tmpAB3=_tmpAB2->f1;}}_LL634: {
# 4148
struct _RegionHandle _tmpAB4=_new_region("temp");struct _RegionHandle*temp=& _tmpAB4;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmpAB5=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmpAB5,0,_tmpAB3);}
# 4152
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpAB3,te,cvtenv);
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpAB3)){
const char*_tmp12C3;void*_tmp12C2;(_tmp12C2=0,Cyc_Tcutil_terr(loc,((_tmp12C3="pointer bounds expression is not an unsigned int",_tag_dyneither(_tmp12C3,sizeof(char),49))),_tag_dyneither(_tmp12C2,sizeof(void*),0)));}{
struct _tuple14 _tmpAB8=Cyc_Evexp_eval_const_uint_exp(_tmpAB3);unsigned int _tmpABA;int _tmpABB;struct _tuple14 _tmpAB9=_tmpAB8;_tmpABA=_tmpAB9.f1;_tmpABB=_tmpAB9.f2;
if(is_zero_terminated  && (!_tmpABB  || _tmpABA < 1)){
const char*_tmp12C6;void*_tmp12C5;(_tmp12C5=0,Cyc_Tcutil_terr(loc,((_tmp12C6="zero-terminated pointer cannot point to empty sequence",_tag_dyneither(_tmp12C6,sizeof(char),55))),_tag_dyneither(_tmp12C5,sizeof(void*),0)));}
_npop_handler(0);goto _LL630;};
# 4148
;_pop_region(temp);}_LL630:;}
# 4160
goto _LL5A6;};}_LL5B9: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp9E5=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp9C9;if(_tmp9E5->tag != 19)goto _LL5BB;else{_tmp9E6=(void*)_tmp9E5->f1;}}_LL5BA:
# 4162
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp9E6,1);goto _LL5A6;_LL5BB: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp9E7=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp9C9;if(_tmp9E7->tag != 18)goto _LL5BD;else{_tmp9E8=_tmp9E7->f1;}}_LL5BC: {
# 4167
struct _RegionHandle _tmpABE=_new_region("temp");struct _RegionHandle*temp=& _tmpABE;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmpABF=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmpABF,0,_tmp9E8);}
# 4171
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp9E8)){
const char*_tmp12C9;void*_tmp12C8;(_tmp12C8=0,Cyc_Tcutil_terr(loc,((_tmp12C9="valueof_t requires an int expression",_tag_dyneither(_tmp12C9,sizeof(char),37))),_tag_dyneither(_tmp12C8,sizeof(void*),0)));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp9E8,te,cvtenv);
_npop_handler(0);goto _LL5A6;
# 4167
;_pop_region(temp);}_LL5BD: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp9E9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp9C9;if(_tmp9E9->tag != 6)goto _LL5BF;}_LL5BE:
# 4175
 goto _LL5C0;_LL5BF: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp9EA=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp9C9;if(_tmp9EA->tag != 7)goto _LL5C1;}_LL5C0:
 goto _LL5A6;_LL5C1: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp9EB=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9C9;if(_tmp9EB->tag != 8)goto _LL5C3;else{_tmp9EC=(_tmp9EB->f1).elt_type;_tmp9ED=(struct Cyc_Absyn_Tqual*)&(_tmp9EB->f1).tq;_tmp9EE=(struct Cyc_Absyn_Exp**)&(_tmp9EB->f1).num_elts;_tmp9EF=(_tmp9EB->f1).zero_term;_tmp9F0=(_tmp9EB->f1).zt_loc;}}_LL5C2: {
# 4180
struct Cyc_Absyn_Exp*_tmpAC2=*_tmp9EE;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp9EC,1);
_tmp9ED->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp9ED->print_const,_tmp9EC);{
int is_zero_terminated;
{union Cyc_Absyn_Constraint*_tmpAC3=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp9EF);union Cyc_Absyn_Constraint*_tmpAC4=_tmpAC3;_LL636: if((_tmpAC4->No_constr).tag != 3)goto _LL638;_LL637:
# 4187
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp9EF,Cyc_Absyn_false_conref);
is_zero_terminated=0;
# 4202 "tcutil.cyc"
goto _LL635;_LL638: if((_tmpAC4->Eq_constr).tag != 1)goto _LL63A;if((int)(_tmpAC4->Eq_constr).val != 1)goto _LL63A;_LL639:
# 4205
 if(!Cyc_Tcutil_admits_zero(_tmp9EC)){
const char*_tmp12CD;void*_tmp12CC[1];struct Cyc_String_pa_PrintArg_struct _tmp12CB;(_tmp12CB.tag=0,((_tmp12CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp9EC)),((_tmp12CC[0]=& _tmp12CB,Cyc_Tcutil_terr(loc,((_tmp12CD="cannot have a zero-terminated array of %s elements",_tag_dyneither(_tmp12CD,sizeof(char),51))),_tag_dyneither(_tmp12CC,sizeof(void*),1)))))));}
is_zero_terminated=1;
goto _LL635;_LL63A:;_LL63B:
# 4211
 is_zero_terminated=0;
goto _LL635;_LL635:;}
# 4216
if(_tmpAC2 == 0){
# 4218
if(is_zero_terminated)
# 4220
*_tmp9EE=(_tmpAC2=Cyc_Absyn_uint_exp(1,0));else{
# 4223
{const char*_tmp12D0;void*_tmp12CF;(_tmp12CF=0,Cyc_Tcutil_warn(loc,((_tmp12D0="array bound defaults to 1 here",_tag_dyneither(_tmp12D0,sizeof(char),31))),_tag_dyneither(_tmp12CF,sizeof(void*),0)));}
*_tmp9EE=(_tmpAC2=Cyc_Absyn_uint_exp(1,0));}}{
# 4227
struct _RegionHandle _tmpACA=_new_region("temp");struct _RegionHandle*temp=& _tmpACA;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmpACB=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmpACB,0,_tmpAC2);}
# 4231
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpAC2)){
const char*_tmp12D3;void*_tmp12D2;(_tmp12D2=0,Cyc_Tcutil_terr(loc,((_tmp12D3="array bounds expression is not an unsigned int",_tag_dyneither(_tmp12D3,sizeof(char),47))),_tag_dyneither(_tmp12D2,sizeof(void*),0)));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpAC2,te,cvtenv);{
# 4238
struct _tuple14 _tmpACE=Cyc_Evexp_eval_const_uint_exp(_tmpAC2);unsigned int _tmpAD0;int _tmpAD1;struct _tuple14 _tmpACF=_tmpACE;_tmpAD0=_tmpACF.f1;_tmpAD1=_tmpACF.f2;
# 4240
if((is_zero_terminated  && _tmpAD1) && _tmpAD0 < 1){
const char*_tmp12D6;void*_tmp12D5;(_tmp12D5=0,Cyc_Tcutil_warn(loc,((_tmp12D6="zero terminated array cannot have zero elements",_tag_dyneither(_tmp12D6,sizeof(char),48))),_tag_dyneither(_tmp12D5,sizeof(void*),0)));}
# 4243
if((_tmpAD1  && _tmpAD0 < 1) && Cyc_Cyclone_tovc_r){
{const char*_tmp12D9;void*_tmp12D8;(_tmp12D8=0,Cyc_Tcutil_warn(loc,((_tmp12D9="arrays with 0 elements are not supported except with gcc -- changing to 1.",_tag_dyneither(_tmp12D9,sizeof(char),75))),_tag_dyneither(_tmp12D8,sizeof(void*),0)));}
*_tmp9EE=Cyc_Absyn_uint_exp(1,0);}
# 4247
_npop_handler(0);goto _LL5A6;};
# 4227
;_pop_region(temp);};};}_LL5C3: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp9F1=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9C9;if(_tmp9F1->tag != 9)goto _LL5C5;else{_tmp9F2=(struct Cyc_List_List**)&(_tmp9F1->f1).tvars;_tmp9F3=(void**)&(_tmp9F1->f1).effect;_tmp9F4=(struct Cyc_Absyn_Tqual*)&(_tmp9F1->f1).ret_tqual;_tmp9F5=(_tmp9F1->f1).ret_typ;_tmp9F6=(_tmp9F1->f1).args;_tmp9F7=(_tmp9F1->f1).c_varargs;_tmp9F8=(_tmp9F1->f1).cyc_varargs;_tmp9F9=(_tmp9F1->f1).rgn_po;_tmp9FA=(_tmp9F1->f1).attributes;_tmp9FB=(_tmp9F1->f1).requires_clause;_tmp9FC=(struct Cyc_List_List**)&(_tmp9F1->f1).requires_relns;_tmp9FD=(_tmp9F1->f1).ensures_clause;_tmp9FE=(struct Cyc_List_List**)&(_tmp9F1->f1).ensures_relns;}}_LL5C4: {
# 4254
int num_convs=0;
int seen_cdecl=0;
int seen_stdcall=0;
int seen_fastcall=0;
int seen_format=0;
enum Cyc_Absyn_Format_Type ft=Cyc_Absyn_Printf_ft;
int fmt_desc_arg=-1;
int fmt_arg_start=-1;
for(0;_tmp9FA != 0;_tmp9FA=_tmp9FA->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp9FA->hd)){
const char*_tmp12DD;void*_tmp12DC[1];struct Cyc_String_pa_PrintArg_struct _tmp12DB;(_tmp12DB.tag=0,((_tmp12DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp9FA->hd)),((_tmp12DC[0]=& _tmp12DB,Cyc_Tcutil_terr(loc,((_tmp12DD="bad function type attribute %s",_tag_dyneither(_tmp12DD,sizeof(char),31))),_tag_dyneither(_tmp12DC,sizeof(void*),1)))))));}{
void*_tmpAD9=(void*)_tmp9FA->hd;void*_tmpADA=_tmpAD9;enum Cyc_Absyn_Format_Type _tmpADF;int _tmpAE0;int _tmpAE1;_LL63D: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpADB=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpADA;if(_tmpADB->tag != 1)goto _LL63F;}_LL63E:
# 4267
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL63C;_LL63F: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpADC=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpADA;if(_tmpADC->tag != 2)goto _LL641;}_LL640:
# 4269
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL63C;_LL641: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpADD=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpADA;if(_tmpADD->tag != 3)goto _LL643;}_LL642:
# 4271
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL63C;_LL643: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpADE=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpADA;if(_tmpADE->tag != 19)goto _LL645;else{_tmpADF=_tmpADE->f1;_tmpAE0=_tmpADE->f2;_tmpAE1=_tmpADE->f3;}}_LL644:
# 4273
 if(!seen_format){
seen_format=1;
ft=_tmpADF;
fmt_desc_arg=_tmpAE0;
fmt_arg_start=_tmpAE1;}else{
# 4279
const char*_tmp12E0;void*_tmp12DF;(_tmp12DF=0,Cyc_Tcutil_terr(loc,((_tmp12E0="function can't have multiple format attributes",_tag_dyneither(_tmp12E0,sizeof(char),47))),_tag_dyneither(_tmp12DF,sizeof(void*),0)));}
goto _LL63C;_LL645:;_LL646:
 goto _LL63C;_LL63C:;};}
# 4284
if(num_convs > 1){
const char*_tmp12E3;void*_tmp12E2;(_tmp12E2=0,Cyc_Tcutil_terr(loc,((_tmp12E3="function can't have multiple calling conventions",_tag_dyneither(_tmp12E3,sizeof(char),49))),_tag_dyneither(_tmp12E2,sizeof(void*),0)));}
# 4289
Cyc_Tcutil_check_unique_tvars(loc,*_tmp9F2);
{struct Cyc_List_List*b=*_tmp9F2;for(0;b != 0;b=b->tl){
((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{
void*_tmpAE6=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmpAE7=_tmpAE6;_LL648: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpAE8=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpAE7;if(_tmpAE8->tag != 0)goto _LL64A;else{if((_tmpAE8->f1)->kind != Cyc_Absyn_MemKind)goto _LL64A;}}_LL649:
# 4295
{const char*_tmp12E7;void*_tmp12E6[1];struct Cyc_String_pa_PrintArg_struct _tmp12E5;(_tmp12E5.tag=0,((_tmp12E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name),((_tmp12E6[0]=& _tmp12E5,Cyc_Tcutil_terr(loc,((_tmp12E7="function attempts to abstract Mem type variable %s",_tag_dyneither(_tmp12E7,sizeof(char),51))),_tag_dyneither(_tmp12E6,sizeof(void*),1)))))));}
goto _LL647;_LL64A:;_LL64B:
 goto _LL647;_LL647:;};}}{
# 4303
struct _RegionHandle _tmpAEC=_new_region("newr");struct _RegionHandle*newr=& _tmpAEC;_push_region(newr);{
struct Cyc_Tcutil_CVTEnv _tmp12E8;struct Cyc_Tcutil_CVTEnv _tmpAED=
(_tmp12E8.r=newr,((_tmp12E8.kind_env=cvtenv.kind_env,((_tmp12E8.free_vars=0,((_tmp12E8.free_evars=0,((_tmp12E8.generalize_evars=cvtenv.generalize_evars,((_tmp12E8.fn_result=1,_tmp12E8)))))))))));
# 4309
_tmpAED=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpAED,& Cyc_Tcutil_tmk,_tmp9F5,1);
_tmp9F4->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp9F4->print_const,_tmp9F5);
_tmpAED.fn_result=0;
{struct Cyc_List_List*a=_tmp9F6;for(0;a != 0;a=a->tl){
struct _tuple10*_tmpAEE=(struct _tuple10*)a->hd;
void*_tmpAEF=(*_tmpAEE).f3;
_tmpAED=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpAED,& Cyc_Tcutil_tmk,_tmpAEF,1);{
int _tmpAF0=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpAEE).f2).print_const,_tmpAEF);
((*_tmpAEE).f2).real_const=_tmpAF0;
# 4320
if(Cyc_Tcutil_is_array(_tmpAEF)){
# 4323
void*_tmpAF1=Cyc_Absyn_new_evar(0,0);
_tmpAED=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpAED,& Cyc_Tcutil_rk,_tmpAF1,1);
(*_tmpAEE).f3=Cyc_Tcutil_promote_array(_tmpAEF,_tmpAF1,0);}};}}
# 4330
if(_tmp9F8 != 0){
if(_tmp9F7){const char*_tmp12EB;void*_tmp12EA;(_tmp12EA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12EB="both c_vararg and cyc_vararg",_tag_dyneither(_tmp12EB,sizeof(char),29))),_tag_dyneither(_tmp12EA,sizeof(void*),0)));}{
struct Cyc_Absyn_VarargInfo _tmpAF4=*_tmp9F8;void*_tmpAF6;int _tmpAF7;struct Cyc_Absyn_VarargInfo _tmpAF5=_tmpAF4;_tmpAF6=_tmpAF5.type;_tmpAF7=_tmpAF5.inject;
_tmpAED=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpAED,& Cyc_Tcutil_tmk,_tmpAF6,1);
(_tmp9F8->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmp9F8->tq).print_const,_tmpAF6);
# 4336
if(_tmpAF7){
void*_tmpAF8=Cyc_Tcutil_compress(_tmpAF6);void*_tmpAF9=_tmpAF8;void*_tmpAFB;union Cyc_Absyn_Constraint*_tmpAFC;union Cyc_Absyn_Constraint*_tmpAFD;_LL64D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpAFA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAF9;if(_tmpAFA->tag != 5)goto _LL64F;else{_tmpAFB=(_tmpAFA->f1).elt_typ;_tmpAFC=((_tmpAFA->f1).ptr_atts).bounds;_tmpAFD=((_tmpAFA->f1).ptr_atts).zero_term;}}_LL64E:
# 4339
{void*_tmpAFE=Cyc_Tcutil_compress(_tmpAFB);void*_tmpAFF=_tmpAFE;_LL652: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpB00=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpAFF;if(_tmpB00->tag != 3)goto _LL654;}_LL653:
# 4341
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmpAFD)){
const char*_tmp12EE;void*_tmp12ED;(_tmp12ED=0,Cyc_Tcutil_terr(loc,((_tmp12EE="can't inject into a zeroterm pointer",_tag_dyneither(_tmp12EE,sizeof(char),37))),_tag_dyneither(_tmp12ED,sizeof(void*),0)));}
{void*_tmpB03=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmpAFC);void*_tmpB04=_tmpB03;_LL657: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpB05=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpB04;if(_tmpB05->tag != 0)goto _LL659;}_LL658:
# 4345
{const char*_tmp12F1;void*_tmp12F0;(_tmp12F0=0,Cyc_Tcutil_terr(loc,((_tmp12F1="can't inject into a fat pointer to datatype",_tag_dyneither(_tmp12F1,sizeof(char),44))),_tag_dyneither(_tmp12F0,sizeof(void*),0)));}
goto _LL656;_LL659:;_LL65A:
 goto _LL656;_LL656:;}
# 4349
goto _LL651;_LL654:;_LL655:
{const char*_tmp12F4;void*_tmp12F3;(_tmp12F3=0,Cyc_Tcutil_terr(loc,((_tmp12F4="can't inject a non-datatype type",_tag_dyneither(_tmp12F4,sizeof(char),33))),_tag_dyneither(_tmp12F3,sizeof(void*),0)));}goto _LL651;_LL651:;}
# 4352
goto _LL64C;_LL64F:;_LL650:
{const char*_tmp12F7;void*_tmp12F6;(_tmp12F6=0,Cyc_Tcutil_terr(loc,((_tmp12F7="expecting a datatype pointer type",_tag_dyneither(_tmp12F7,sizeof(char),34))),_tag_dyneither(_tmp12F6,sizeof(void*),0)));}goto _LL64C;_LL64C:;}};}
# 4358
if(seen_format){
int _tmpB0C=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp9F6);
if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmpB0C) || fmt_arg_start < 0) || 
# 4362
_tmp9F8 == 0  && fmt_arg_start != 0) || 
_tmp9F8 != 0  && fmt_arg_start != _tmpB0C + 1){
const char*_tmp12FA;void*_tmp12F9;(_tmp12F9=0,Cyc_Tcutil_terr(loc,((_tmp12FA="bad format descriptor",_tag_dyneither(_tmp12FA,sizeof(char),22))),_tag_dyneither(_tmp12F9,sizeof(void*),0)));}else{
# 4367
struct _tuple10 _tmpB0F=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp9F6,fmt_desc_arg - 1);void*_tmpB11;struct _tuple10 _tmpB10=_tmpB0F;_tmpB11=_tmpB10.f3;
# 4369
{void*_tmpB12=Cyc_Tcutil_compress(_tmpB11);void*_tmpB13=_tmpB12;void*_tmpB15;union Cyc_Absyn_Constraint*_tmpB16;union Cyc_Absyn_Constraint*_tmpB17;_LL65C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpB14=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB13;if(_tmpB14->tag != 5)goto _LL65E;else{_tmpB15=(_tmpB14->f1).elt_typ;_tmpB16=((_tmpB14->f1).ptr_atts).bounds;_tmpB17=((_tmpB14->f1).ptr_atts).zero_term;}}_LL65D:
# 4372
{struct _tuple0 _tmp12FB;struct _tuple0 _tmpB18=(_tmp12FB.f1=Cyc_Tcutil_compress(_tmpB15),((_tmp12FB.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpB16),_tmp12FB)));struct _tuple0 _tmpB19=_tmpB18;_LL661:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpB1A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpB19.f1;if(_tmpB1A->tag != 6)goto _LL663;else{if(_tmpB1A->f1 != Cyc_Absyn_None)goto _LL663;if(_tmpB1A->f2 != Cyc_Absyn_Char_sz)goto _LL663;}}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpB1B=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpB19.f2;if(_tmpB1B->tag != 0)goto _LL663;};_LL662:
 goto _LL660;_LL663:;_LL664:
{const char*_tmp12FE;void*_tmp12FD;(_tmp12FD=0,Cyc_Tcutil_terr(loc,((_tmp12FE="format descriptor is not a char ? type",_tag_dyneither(_tmp12FE,sizeof(char),39))),_tag_dyneither(_tmp12FD,sizeof(void*),0)));}goto _LL660;_LL660:;}
# 4376
goto _LL65B;_LL65E:;_LL65F:
{const char*_tmp1301;void*_tmp1300;(_tmp1300=0,Cyc_Tcutil_terr(loc,((_tmp1301="format descriptor is not a char ? type",_tag_dyneither(_tmp1301,sizeof(char),39))),_tag_dyneither(_tmp1300,sizeof(void*),0)));}goto _LL65B;_LL65B:;}
# 4379
if(fmt_arg_start != 0){
# 4383
int problem;
{struct _tuple31 _tmp1302;struct _tuple31 _tmpB21=(_tmp1302.f1=ft,((_tmp1302.f2=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp9F8))->type)),_tmp1302)));struct _tuple31 _tmpB22=_tmpB21;struct Cyc_Absyn_Datatypedecl*_tmpB24;struct Cyc_Absyn_Datatypedecl*_tmpB26;_LL666: if(_tmpB22.f1 != Cyc_Absyn_Printf_ft)goto _LL668;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpB23=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpB22.f2;if(_tmpB23->tag != 3)goto _LL668;else{if((((_tmpB23->f1).datatype_info).KnownDatatype).tag != 2)goto _LL668;_tmpB24=*((struct Cyc_Absyn_Datatypedecl**)(((_tmpB23->f1).datatype_info).KnownDatatype).val);}};_LL667:
# 4386
 problem=Cyc_Absyn_qvar_cmp(_tmpB24->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL665;_LL668: if(_tmpB22.f1 != Cyc_Absyn_Scanf_ft)goto _LL66A;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpB25=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpB22.f2;if(_tmpB25->tag != 3)goto _LL66A;else{if((((_tmpB25->f1).datatype_info).KnownDatatype).tag != 2)goto _LL66A;_tmpB26=*((struct Cyc_Absyn_Datatypedecl**)(((_tmpB25->f1).datatype_info).KnownDatatype).val);}};_LL669:
# 4388
 problem=Cyc_Absyn_qvar_cmp(_tmpB26->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL665;_LL66A:;_LL66B:
# 4390
 problem=1;goto _LL665;_LL665:;}
# 4392
if(problem){
const char*_tmp1305;void*_tmp1304;(_tmp1304=0,Cyc_Tcutil_terr(loc,((_tmp1305="format attribute and vararg types don't match",_tag_dyneither(_tmp1305,sizeof(char),46))),_tag_dyneither(_tmp1304,sizeof(void*),0)));}}}}
# 4400
{struct Cyc_List_List*rpo=_tmp9F9;for(0;rpo != 0;rpo=rpo->tl){
struct _tuple0*_tmpB2A=(struct _tuple0*)rpo->hd;void*_tmpB2C;void*_tmpB2D;struct _tuple0*_tmpB2B=_tmpB2A;_tmpB2C=_tmpB2B->f1;_tmpB2D=_tmpB2B->f2;
_tmpAED=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpAED,& Cyc_Tcutil_ek,_tmpB2C,1);
_tmpAED=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpAED,& Cyc_Tcutil_trk,_tmpB2D,1);}}{
# 4408
struct Cyc_Tcenv_Fenv*_tmpB2E=Cyc_Tcenv_bogus_fenv(newr,_tmp9F5,_tmp9F6);
struct Cyc_Tcenv_Tenv*_tmp1306;struct Cyc_Tcenv_Tenv*_tmpB2F=(_tmp1306=_region_malloc(newr,sizeof(*_tmp1306)),((_tmp1306->ns=te->ns,((_tmp1306->ae=te->ae,((_tmp1306->le=_tmpB2E,((_tmp1306->allow_valueof=1,_tmp1306)))))))));
const char*_tmp1307;struct _tuple30 _tmpB30=Cyc_Tcutil_check_clause(loc,_tmpB2F,_tmpAED,((_tmp1307="@requires",_tag_dyneither(_tmp1307,sizeof(char),10))),_tmp9FB);struct Cyc_Tcutil_CVTEnv _tmpB32;struct Cyc_List_List*_tmpB33;struct _tuple30 _tmpB31=_tmpB30;_tmpB32=_tmpB31.f1;_tmpB33=_tmpB31.f2;
_tmpAED=_tmpB32;
*_tmp9FC=_tmpB33;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp9F6,_tmpB33);{
# 4415
struct _dyneither_ptr*_tmpB34=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"return_value",sizeof(char),13);
struct _tuple2*_tmp130A;struct Cyc_Absyn_Vardecl*_tmp1309;struct Cyc_Absyn_Vardecl*_tmpB35=(_tmp1309=_cycalloc(sizeof(*_tmp1309)),((_tmp1309->sc=Cyc_Absyn_Public,((_tmp1309->name=((_tmp130A=_cycalloc(sizeof(*_tmp130A)),((_tmp130A->f1=Cyc_Absyn_Loc_n,((_tmp130A->f2=_tmpB34,_tmp130A)))))),((_tmp1309->tq=
Cyc_Absyn_empty_tqual(0),((_tmp1309->type=_tmp9F5,((_tmp1309->initializer=0,((_tmp1309->rgn=
Cyc_Tcenv_curr_rgn(_tmpB2F),((_tmp1309->attributes=0,((_tmp1309->escapes=0,_tmp1309)))))))))))))))));
# 4420
_tmpB2F=Cyc_Tcenv_add_local_var(newr,0,_tmpB2F,_tmpB35);{
const char*_tmp130B;struct _tuple30 _tmpB36=Cyc_Tcutil_check_clause(loc,_tmpB2F,_tmpAED,((_tmp130B="@ensures",_tag_dyneither(_tmp130B,sizeof(char),9))),_tmp9FD);struct Cyc_Tcutil_CVTEnv _tmpB38;struct Cyc_List_List*_tmpB39;struct _tuple30 _tmpB37=_tmpB36;_tmpB38=_tmpB37.f1;_tmpB39=_tmpB37.f2;
_tmpAED=_tmpB38;
*_tmp9FE=_tmpB39;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp9F6,_tmpB39);
# 4426
if(*_tmp9F3 != 0)
_tmpAED=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpAED,& Cyc_Tcutil_ek,(void*)_check_null(*_tmp9F3),1);else{
# 4429
struct Cyc_List_List*effect=0;
# 4434
{struct Cyc_List_List*tvs=_tmpAED.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmpB3A=*((struct _tuple28*)tvs->hd);struct Cyc_Absyn_Tvar*_tmpB3C;int _tmpB3D;struct _tuple28 _tmpB3B=_tmpB3A;_tmpB3C=_tmpB3B.f1;_tmpB3D=_tmpB3B.f2;
if(!_tmpB3D)continue;{
void*_tmpB3E=Cyc_Absyn_compress_kb(_tmpB3C->kind);void*_tmpB3F=_tmpB3E;struct Cyc_Core_Opt**_tmpB41;struct Cyc_Absyn_Kind*_tmpB42;struct Cyc_Absyn_Kind*_tmpB44;struct Cyc_Core_Opt**_tmpB48;struct Cyc_Core_Opt**_tmpB4B;_LL66D:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB40=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB3F;if(_tmpB40->tag != 2)goto _LL66F;else{_tmpB41=(struct Cyc_Core_Opt**)& _tmpB40->f1;_tmpB42=_tmpB40->f2;}}if(!(_tmpB42->kind == Cyc_Absyn_RgnKind))goto _LL66F;_LL66E:
# 4440
 if(_tmpB42->aliasqual == Cyc_Absyn_Top){
*_tmpB41=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);_tmpB44=_tmpB42;goto _LL670;}
# 4443
*_tmpB41=Cyc_Tcutil_kind_to_bound_opt(_tmpB42);_tmpB44=_tmpB42;goto _LL670;_LL66F:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpB43=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpB3F;if(_tmpB43->tag != 0)goto _LL671;else{_tmpB44=_tmpB43->f1;}}if(!(_tmpB44->kind == Cyc_Absyn_RgnKind))goto _LL671;_LL670:
# 4445
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp131A;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1319;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1318;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1317;struct Cyc_List_List*_tmp1316;effect=((_tmp1316=_cycalloc(sizeof(*_tmp1316)),((_tmp1316->hd=(void*)((_tmp131A=_cycalloc(sizeof(*_tmp131A)),((_tmp131A[0]=((_tmp1317.tag=23,((_tmp1317.f1=(void*)((_tmp1319=_cycalloc(sizeof(*_tmp1319)),((_tmp1319[0]=((_tmp1318.tag=2,((_tmp1318.f1=_tmpB3C,_tmp1318)))),_tmp1319)))),_tmp1317)))),_tmp131A)))),((_tmp1316->tl=effect,_tmp1316))))));}goto _LL66C;_LL671: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB45=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB3F;if(_tmpB45->tag != 2)goto _LL673;else{if((_tmpB45->f2)->kind != Cyc_Absyn_IntKind)goto _LL673;}}_LL672:
 goto _LL674;_LL673: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpB46=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpB3F;if(_tmpB46->tag != 0)goto _LL675;else{if((_tmpB46->f1)->kind != Cyc_Absyn_IntKind)goto _LL675;}}_LL674:
 goto _LL66C;_LL675: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB47=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB3F;if(_tmpB47->tag != 2)goto _LL677;else{_tmpB48=(struct Cyc_Core_Opt**)& _tmpB47->f1;if((_tmpB47->f2)->kind != Cyc_Absyn_EffKind)goto _LL677;}}_LL676:
# 4449
*_tmpB48=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);goto _LL678;_LL677: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpB49=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpB3F;if(_tmpB49->tag != 0)goto _LL679;else{if((_tmpB49->f1)->kind != Cyc_Absyn_EffKind)goto _LL679;}}_LL678:
# 4451
{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1320;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp131F;struct Cyc_List_List*_tmp131E;effect=((_tmp131E=_cycalloc(sizeof(*_tmp131E)),((_tmp131E->hd=(void*)((_tmp1320=_cycalloc(sizeof(*_tmp1320)),((_tmp1320[0]=((_tmp131F.tag=2,((_tmp131F.f1=_tmpB3C,_tmp131F)))),_tmp1320)))),((_tmp131E->tl=effect,_tmp131E))))));}goto _LL66C;_LL679: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpB4A=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpB3F;if(_tmpB4A->tag != 1)goto _LL67B;else{_tmpB4B=(struct Cyc_Core_Opt**)& _tmpB4A->f1;}}_LL67A:
# 4453
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1326;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1325;struct Cyc_Core_Opt*_tmp1324;*_tmpB4B=((_tmp1324=_cycalloc(sizeof(*_tmp1324)),((_tmp1324->v=(void*)((_tmp1326=_cycalloc(sizeof(*_tmp1326)),((_tmp1326[0]=((_tmp1325.tag=2,((_tmp1325.f1=0,((_tmp1325.f2=& Cyc_Tcutil_ak,_tmp1325)))))),_tmp1326)))),_tmp1324))));}goto _LL67C;_LL67B:;_LL67C:
# 4456
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1335;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1334;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1333;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp1332;struct Cyc_List_List*_tmp1331;effect=((_tmp1331=_cycalloc(sizeof(*_tmp1331)),((_tmp1331->hd=(void*)((_tmp1335=_cycalloc(sizeof(*_tmp1335)),((_tmp1335[0]=((_tmp1332.tag=25,((_tmp1332.f1=(void*)((_tmp1334=_cycalloc(sizeof(*_tmp1334)),((_tmp1334[0]=((_tmp1333.tag=2,((_tmp1333.f1=_tmpB3C,_tmp1333)))),_tmp1334)))),_tmp1332)))),_tmp1335)))),((_tmp1331->tl=effect,_tmp1331))))));}goto _LL66C;_LL66C:;};}}
# 4460
{struct Cyc_List_List*ts=_tmpAED.free_evars;for(0;ts != 0;ts=ts->tl){
struct _tuple29 _tmpB5C=*((struct _tuple29*)ts->hd);void*_tmpB5E;int _tmpB5F;struct _tuple29 _tmpB5D=_tmpB5C;_tmpB5E=_tmpB5D.f1;_tmpB5F=_tmpB5D.f2;
if(!_tmpB5F)continue;{
struct Cyc_Absyn_Kind*_tmpB60=Cyc_Tcutil_typ_kind(_tmpB5E);struct Cyc_Absyn_Kind*_tmpB61=_tmpB60;_LL67E: if(_tmpB61->kind != Cyc_Absyn_RgnKind)goto _LL680;_LL67F:
# 4465
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp133B;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp133A;struct Cyc_List_List*_tmp1339;effect=((_tmp1339=_cycalloc(sizeof(*_tmp1339)),((_tmp1339->hd=(void*)((_tmp133B=_cycalloc(sizeof(*_tmp133B)),((_tmp133B[0]=((_tmp133A.tag=23,((_tmp133A.f1=_tmpB5E,_tmp133A)))),_tmp133B)))),((_tmp1339->tl=effect,_tmp1339))))));}goto _LL67D;_LL680: if(_tmpB61->kind != Cyc_Absyn_EffKind)goto _LL682;_LL681:
# 4467
{struct Cyc_List_List*_tmp133C;effect=((_tmp133C=_cycalloc(sizeof(*_tmp133C)),((_tmp133C->hd=_tmpB5E,((_tmp133C->tl=effect,_tmp133C))))));}goto _LL67D;_LL682: if(_tmpB61->kind != Cyc_Absyn_IntKind)goto _LL684;_LL683:
 goto _LL67D;_LL684:;_LL685:
# 4470
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1342;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp1341;struct Cyc_List_List*_tmp1340;effect=((_tmp1340=_cycalloc(sizeof(*_tmp1340)),((_tmp1340->hd=(void*)((_tmp1342=_cycalloc(sizeof(*_tmp1342)),((_tmp1342[0]=((_tmp1341.tag=25,((_tmp1341.f1=_tmpB5E,_tmp1341)))),_tmp1342)))),((_tmp1340->tl=effect,_tmp1340))))));}goto _LL67D;_LL67D:;};}}{
# 4473
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1345;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1344;*_tmp9F3=(void*)((_tmp1344=_cycalloc(sizeof(*_tmp1344)),((_tmp1344[0]=((_tmp1345.tag=24,((_tmp1345.f1=effect,_tmp1345)))),_tmp1344))));};}
# 4479
if(*_tmp9F2 != 0){
struct Cyc_List_List*bs=*_tmp9F2;for(0;bs != 0;bs=bs->tl){
void*_tmpB6B=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);void*_tmpB6C=_tmpB6B;struct Cyc_Core_Opt**_tmpB6E;struct Cyc_Core_Opt**_tmpB70;struct Cyc_Core_Opt**_tmpB72;struct Cyc_Core_Opt**_tmpB74;struct Cyc_Core_Opt**_tmpB76;struct Cyc_Core_Opt**_tmpB78;struct Cyc_Core_Opt**_tmpB7A;struct Cyc_Core_Opt**_tmpB7C;struct Cyc_Core_Opt**_tmpB7E;struct Cyc_Core_Opt**_tmpB80;struct Cyc_Absyn_Kind*_tmpB81;_LL687: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpB6D=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpB6C;if(_tmpB6D->tag != 1)goto _LL689;else{_tmpB6E=(struct Cyc_Core_Opt**)& _tmpB6D->f1;}}_LL688:
# 4483
{const char*_tmp1349;void*_tmp1348[1];struct Cyc_String_pa_PrintArg_struct _tmp1347;(_tmp1347.tag=0,((_tmp1347.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((_tmp1348[0]=& _tmp1347,Cyc_Tcutil_warn(loc,((_tmp1349="Type variable %s unconstrained, assuming boxed",_tag_dyneither(_tmp1349,sizeof(char),47))),_tag_dyneither(_tmp1348,sizeof(void*),1)))))));}
# 4485
_tmpB70=_tmpB6E;goto _LL68A;_LL689: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB6F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB6C;if(_tmpB6F->tag != 2)goto _LL68B;else{_tmpB70=(struct Cyc_Core_Opt**)& _tmpB6F->f1;if((_tmpB6F->f2)->kind != Cyc_Absyn_BoxKind)goto _LL68B;if((_tmpB6F->f2)->aliasqual != Cyc_Absyn_Top)goto _LL68B;}}_LL68A:
 _tmpB72=_tmpB70;goto _LL68C;_LL68B: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB71=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB6C;if(_tmpB71->tag != 2)goto _LL68D;else{_tmpB72=(struct Cyc_Core_Opt**)& _tmpB71->f1;if((_tmpB71->f2)->kind != Cyc_Absyn_MemKind)goto _LL68D;if((_tmpB71->f2)->aliasqual != Cyc_Absyn_Top)goto _LL68D;}}_LL68C:
 _tmpB74=_tmpB72;goto _LL68E;_LL68D: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB73=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB6C;if(_tmpB73->tag != 2)goto _LL68F;else{_tmpB74=(struct Cyc_Core_Opt**)& _tmpB73->f1;if((_tmpB73->f2)->kind != Cyc_Absyn_MemKind)goto _LL68F;if((_tmpB73->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL68F;}}_LL68E:
 _tmpB76=_tmpB74;goto _LL690;_LL68F: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB75=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB6C;if(_tmpB75->tag != 2)goto _LL691;else{_tmpB76=(struct Cyc_Core_Opt**)& _tmpB75->f1;if((_tmpB75->f2)->kind != Cyc_Absyn_AnyKind)goto _LL691;if((_tmpB75->f2)->aliasqual != Cyc_Absyn_Top)goto _LL691;}}_LL690:
 _tmpB78=_tmpB76;goto _LL692;_LL691: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB77=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB6C;if(_tmpB77->tag != 2)goto _LL693;else{_tmpB78=(struct Cyc_Core_Opt**)& _tmpB77->f1;if((_tmpB77->f2)->kind != Cyc_Absyn_AnyKind)goto _LL693;if((_tmpB77->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL693;}}_LL692:
# 4491
*_tmpB78=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL686;_LL693: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB79=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB6C;if(_tmpB79->tag != 2)goto _LL695;else{_tmpB7A=(struct Cyc_Core_Opt**)& _tmpB79->f1;if((_tmpB79->f2)->kind != Cyc_Absyn_MemKind)goto _LL695;if((_tmpB79->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL695;}}_LL694:
 _tmpB7C=_tmpB7A;goto _LL696;_LL695: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB7B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB6C;if(_tmpB7B->tag != 2)goto _LL697;else{_tmpB7C=(struct Cyc_Core_Opt**)& _tmpB7B->f1;if((_tmpB7B->f2)->kind != Cyc_Absyn_AnyKind)goto _LL697;if((_tmpB7B->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL697;}}_LL696:
# 4494
*_tmpB7C=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL686;_LL697: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB7D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB6C;if(_tmpB7D->tag != 2)goto _LL699;else{_tmpB7E=(struct Cyc_Core_Opt**)& _tmpB7D->f1;if((_tmpB7D->f2)->kind != Cyc_Absyn_RgnKind)goto _LL699;if((_tmpB7D->f2)->aliasqual != Cyc_Absyn_Top)goto _LL699;}}_LL698:
# 4496
*_tmpB7E=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL686;_LL699: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB7F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB6C;if(_tmpB7F->tag != 2)goto _LL69B;else{_tmpB80=(struct Cyc_Core_Opt**)& _tmpB7F->f1;_tmpB81=_tmpB7F->f2;}}_LL69A:
# 4498
*_tmpB80=Cyc_Tcutil_kind_to_bound_opt(_tmpB81);goto _LL686;_LL69B: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpB82=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpB6C;if(_tmpB82->tag != 0)goto _LL69D;else{if((_tmpB82->f1)->kind != Cyc_Absyn_MemKind)goto _LL69D;}}_LL69C:
# 4500
{const char*_tmp134C;void*_tmp134B;(_tmp134B=0,Cyc_Tcutil_terr(loc,((_tmp134C="functions can't abstract M types",_tag_dyneither(_tmp134C,sizeof(char),33))),_tag_dyneither(_tmp134B,sizeof(void*),0)));}goto _LL686;_LL69D:;_LL69E:
 goto _LL686;_LL686:;}}
# 4505
cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,_tmpAED.kind_env,*_tmp9F2);
_tmpAED.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(_tmpAED.r,_tmpAED.free_vars,*_tmp9F2);
# 4508
{struct Cyc_List_List*tvs=_tmpAED.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmpB88=*((struct _tuple28*)tvs->hd);struct Cyc_Absyn_Tvar*_tmpB8A;int _tmpB8B;struct _tuple28 _tmpB89=_tmpB88;_tmpB8A=_tmpB89.f1;_tmpB8B=_tmpB89.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpB8A,_tmpB8B);}}{
# 4513
struct Cyc_List_List*evs=_tmpAED.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmpB8C=*((struct _tuple29*)evs->hd);void*_tmpB8E;int _tmpB8F;struct _tuple29 _tmpB8D=_tmpB8C;_tmpB8E=_tmpB8D.f1;_tmpB8F=_tmpB8D.f2;
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmpB8E,_tmpB8F);}};};};};}
# 4518
_npop_handler(0);goto _LL5A6;
# 4303
;_pop_region(newr);};}_LL5C5: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp9FF=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp9C9;if(_tmp9FF->tag != 10)goto _LL5C7;else{_tmpA00=_tmp9FF->f1;}}_LL5C6:
# 4521
 for(0;_tmpA00 != 0;_tmpA00=_tmpA00->tl){
struct _tuple12*_tmpB97=(struct _tuple12*)_tmpA00->hd;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmpB97).f2,1);
((*_tmpB97).f1).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpB97).f1).print_const,(*_tmpB97).f2);}
# 4527
goto _LL5A6;_LL5C7: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpA01=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp9C9;if(_tmpA01->tag != 12)goto _LL5C9;else{_tmpA02=_tmpA01->f1;_tmpA03=_tmpA01->f2;}}_LL5C8: {
# 4531
struct _RegionHandle _tmpB98=_new_region("aprev_rgn");struct _RegionHandle*aprev_rgn=& _tmpB98;_push_region(aprev_rgn);{
struct Cyc_List_List*prev_fields=0;
for(0;_tmpA03 != 0;_tmpA03=_tmpA03->tl){
struct Cyc_Absyn_Aggrfield*_tmpB99=(struct Cyc_Absyn_Aggrfield*)_tmpA03->hd;struct _dyneither_ptr*_tmpB9B;struct Cyc_Absyn_Tqual*_tmpB9C;void*_tmpB9D;struct Cyc_Absyn_Exp*_tmpB9E;struct Cyc_List_List*_tmpB9F;struct Cyc_Absyn_Exp*_tmpBA0;struct Cyc_Absyn_Aggrfield*_tmpB9A=_tmpB99;_tmpB9B=_tmpB9A->name;_tmpB9C=(struct Cyc_Absyn_Tqual*)& _tmpB9A->tq;_tmpB9D=_tmpB9A->type;_tmpB9E=_tmpB9A->width;_tmpB9F=_tmpB9A->attributes;_tmpBA0=_tmpB9A->requires_clause;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpB9B)){
const char*_tmp1350;void*_tmp134F[1];struct Cyc_String_pa_PrintArg_struct _tmp134E;(_tmp134E.tag=0,((_tmp134E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB9B),((_tmp134F[0]=& _tmp134E,Cyc_Tcutil_terr(loc,((_tmp1350="duplicate field %s",_tag_dyneither(_tmp1350,sizeof(char),19))),_tag_dyneither(_tmp134F,sizeof(void*),1)))))));}
{const char*_tmp1351;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpB9B,((_tmp1351="",_tag_dyneither(_tmp1351,sizeof(char),1))))!= 0){
struct Cyc_List_List*_tmp1352;prev_fields=((_tmp1352=_region_malloc(aprev_rgn,sizeof(*_tmp1352)),((_tmp1352->hd=_tmpB9B,((_tmp1352->tl=prev_fields,_tmp1352))))));}}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpB9D,1);
_tmpB9C->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpB9C->print_const,_tmpB9D);
if((_tmpA02 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmpB9D)) && _tmpBA0 == 0){
# 4543
const char*_tmp1356;void*_tmp1355[1];struct Cyc_String_pa_PrintArg_struct _tmp1354;(_tmp1354.tag=0,((_tmp1354.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB9B),((_tmp1355[0]=& _tmp1354,Cyc_Tcutil_warn(loc,((_tmp1356="union member %s is not `bits-only' so it can only be written and not read",_tag_dyneither(_tmp1356,sizeof(char),74))),_tag_dyneither(_tmp1355,sizeof(void*),1)))))));}
Cyc_Tcutil_check_bitfield(loc,te,_tmpB9D,_tmpB9E,_tmpB9B);
Cyc_Tcutil_check_field_atts(loc,_tmpB9B,_tmpB9F);
if(_tmpBA0 != 0){
# 4548
if(_tmpA02 != Cyc_Absyn_UnionA){
const char*_tmp1359;void*_tmp1358;(_tmp1358=0,Cyc_Tcutil_terr(loc,((_tmp1359="@requires clause is only allowed on union members",_tag_dyneither(_tmp1359,sizeof(char),50))),_tag_dyneither(_tmp1358,sizeof(void*),0)));}{
struct _RegionHandle _tmpBAB=_new_region("temp");struct _RegionHandle*temp=& _tmpBAB;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmpBAC=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmpBAC,0,_tmpBA0);}
# 4554
if(!Cyc_Tcutil_is_integral(_tmpBA0)){
const char*_tmp135D;void*_tmp135C[1];struct Cyc_String_pa_PrintArg_struct _tmp135B;(_tmp135B.tag=0,((_tmp135B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmpBA0->topt))),((_tmp135C[0]=& _tmp135B,Cyc_Tcutil_terr(loc,((_tmp135D="@requires clause has type %s instead of integral type",_tag_dyneither(_tmp135D,sizeof(char),54))),_tag_dyneither(_tmp135C,sizeof(void*),1)))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpBA0,te,cvtenv);
# 4550
;_pop_region(temp);};}}}
# 4561
_npop_handler(0);goto _LL5A6;
# 4531
;_pop_region(aprev_rgn);}_LL5C9: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpA04=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp9C9;if(_tmpA04->tag != 11)goto _LL5CB;else{_tmpA05=(union Cyc_Absyn_AggrInfoU*)&(_tmpA04->f1).aggr_info;_tmpA06=(struct Cyc_List_List**)&(_tmpA04->f1).targs;}}_LL5CA:
# 4564
{union Cyc_Absyn_AggrInfoU _tmpBB0=*_tmpA05;union Cyc_Absyn_AggrInfoU _tmpBB1=_tmpBB0;enum Cyc_Absyn_AggrKind _tmpBB2;struct _tuple2*_tmpBB3;struct Cyc_Core_Opt*_tmpBB4;struct Cyc_Absyn_Aggrdecl*_tmpBB5;_LL6A0: if((_tmpBB1.UnknownAggr).tag != 1)goto _LL6A2;_tmpBB2=((struct _tuple4)(_tmpBB1.UnknownAggr).val).f1;_tmpBB3=((struct _tuple4)(_tmpBB1.UnknownAggr).val).f2;_tmpBB4=((struct _tuple4)(_tmpBB1.UnknownAggr).val).f3;_LL6A1: {
# 4566
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmpBB6;_push_handler(& _tmpBB6);{int _tmpBB8=0;if(setjmp(_tmpBB6.handler))_tmpBB8=1;if(!_tmpBB8){
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmpBB3);{
struct Cyc_Absyn_Aggrdecl*_tmpBB9=*adp;
if(_tmpBB9->kind != _tmpBB2){
if(_tmpBB9->kind == Cyc_Absyn_StructA){
const char*_tmp1362;void*_tmp1361[2];struct Cyc_String_pa_PrintArg_struct _tmp1360;struct Cyc_String_pa_PrintArg_struct _tmp135F;(_tmp135F.tag=0,((_tmp135F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpBB3)),((_tmp1360.tag=0,((_tmp1360.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4572
Cyc_Absynpp_qvar2string(_tmpBB3)),((_tmp1361[0]=& _tmp1360,((_tmp1361[1]=& _tmp135F,Cyc_Tcutil_terr(loc,((_tmp1362="expecting struct %s instead of union %s",_tag_dyneither(_tmp1362,sizeof(char),40))),_tag_dyneither(_tmp1361,sizeof(void*),2)))))))))))));}else{
# 4575
const char*_tmp1367;void*_tmp1366[2];struct Cyc_String_pa_PrintArg_struct _tmp1365;struct Cyc_String_pa_PrintArg_struct _tmp1364;(_tmp1364.tag=0,((_tmp1364.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpBB3)),((_tmp1365.tag=0,((_tmp1365.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4575
Cyc_Absynpp_qvar2string(_tmpBB3)),((_tmp1366[0]=& _tmp1365,((_tmp1366[1]=& _tmp1364,Cyc_Tcutil_terr(loc,((_tmp1367="expecting union %s instead of struct %s",_tag_dyneither(_tmp1367,sizeof(char),40))),_tag_dyneither(_tmp1366,sizeof(void*),2)))))))))))));}}
# 4578
if((unsigned int)_tmpBB4  && (int)_tmpBB4->v){
if(!((unsigned int)_tmpBB9->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpBB9->impl))->tagged){
const char*_tmp136B;void*_tmp136A[1];struct Cyc_String_pa_PrintArg_struct _tmp1369;(_tmp1369.tag=0,((_tmp1369.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpBB3)),((_tmp136A[0]=& _tmp1369,Cyc_Tcutil_terr(loc,((_tmp136B="@tagged qualfiers don't agree on union %s",_tag_dyneither(_tmp136B,sizeof(char),42))),_tag_dyneither(_tmp136A,sizeof(void*),1)))))));}}
# 4584
*_tmpA05=Cyc_Absyn_KnownAggr(adp);};
# 4568
;_pop_handler();}else{void*_tmpBB7=(void*)_exn_thrown;void*_tmpBC6=_tmpBB7;void*_tmpBC8;_LL6A5: {struct Cyc_Dict_Absent_exn_struct*_tmpBC7=(struct Cyc_Dict_Absent_exn_struct*)_tmpBC6;if(_tmpBC7->tag != Cyc_Dict_Absent)goto _LL6A7;}_LL6A6: {
# 4588
struct Cyc_Tcenv_Genv*_tmpBC9=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp136C;struct Cyc_Absyn_Aggrdecl*_tmpBCA=(_tmp136C=_cycalloc(sizeof(*_tmp136C)),((_tmp136C->kind=_tmpBB2,((_tmp136C->sc=Cyc_Absyn_Extern,((_tmp136C->name=_tmpBB3,((_tmp136C->tvs=0,((_tmp136C->impl=0,((_tmp136C->attributes=0,_tmp136C)))))))))))));
Cyc_Tc_tcAggrdecl(te,_tmpBC9,loc,_tmpBCA);
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmpBB3);
*_tmpA05=Cyc_Absyn_KnownAggr(adp);
# 4594
if(*_tmpA06 != 0){
{const char*_tmp1370;void*_tmp136F[1];struct Cyc_String_pa_PrintArg_struct _tmp136E;(_tmp136E.tag=0,((_tmp136E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpBB3)),((_tmp136F[0]=& _tmp136E,Cyc_Tcutil_terr(loc,((_tmp1370="declare parameterized type %s before using",_tag_dyneither(_tmp1370,sizeof(char),43))),_tag_dyneither(_tmp136F,sizeof(void*),1)))))));}
return cvtenv;}
# 4599
goto _LL6A4;}_LL6A7: _tmpBC8=_tmpBC6;_LL6A8:(void)_rethrow(_tmpBC8);_LL6A4:;}};}
# 4601
_tmpBB5=*adp;goto _LL6A3;}_LL6A2: if((_tmpBB1.KnownAggr).tag != 2)goto _LL69F;_tmpBB5=*((struct Cyc_Absyn_Aggrdecl**)(_tmpBB1.KnownAggr).val);_LL6A3: {
# 4603
struct Cyc_List_List*tvs=_tmpBB5->tvs;
struct Cyc_List_List*ts=*_tmpA06;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Tvar*_tmpBCF=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmpBD0=(void*)ts->hd;
# 4611
{struct _tuple0 _tmp1371;struct _tuple0 _tmpBD1=(_tmp1371.f1=Cyc_Absyn_compress_kb(_tmpBCF->kind),((_tmp1371.f2=_tmpBD0,_tmp1371)));struct _tuple0 _tmpBD2=_tmpBD1;struct Cyc_Absyn_Tvar*_tmpBD5;_LL6AA:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpBD3=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpBD2.f1;if(_tmpBD3->tag != 1)goto _LL6AC;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpBD4=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpBD2.f2;if(_tmpBD4->tag != 2)goto _LL6AC;else{_tmpBD5=_tmpBD4->f1;}};_LL6AB:
# 4613
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpBD5);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpBD5,1);
continue;_LL6AC:;_LL6AD:
 goto _LL6A9;_LL6A9:;}{
# 4618
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 4622
if(ts != 0){
const char*_tmp1375;void*_tmp1374[1];struct Cyc_String_pa_PrintArg_struct _tmp1373;(_tmp1373.tag=0,((_tmp1373.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBB5->name)),((_tmp1374[0]=& _tmp1373,Cyc_Tcutil_terr(loc,((_tmp1375="too many parameters for type %s",_tag_dyneither(_tmp1375,sizeof(char),32))),_tag_dyneither(_tmp1374,sizeof(void*),1)))))));}
if(tvs != 0){
# 4626
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp1376;hidden_ts=((_tmp1376=_cycalloc(sizeof(*_tmp1376)),((_tmp1376->hd=e,((_tmp1376->tl=hidden_ts,_tmp1376))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);}
# 4633
*_tmpA06=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpA06,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}}_LL69F:;}
# 4636
goto _LL5A6;_LL5CB: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA07=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9C9;if(_tmpA07->tag != 17)goto _LL5CD;else{_tmpA08=_tmpA07->f1;_tmpA09=(struct Cyc_List_List**)& _tmpA07->f2;_tmpA0A=(struct Cyc_Absyn_Typedefdecl**)& _tmpA07->f3;_tmpA0B=(void**)((void**)& _tmpA07->f4);}}_LL5CC: {
# 4639
struct Cyc_List_List*targs=*_tmpA09;
# 4641
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmpBDB;_push_handler(& _tmpBDB);{int _tmpBDD=0;if(setjmp(_tmpBDB.handler))_tmpBDD=1;if(!_tmpBDD){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmpA08);;_pop_handler();}else{void*_tmpBDC=(void*)_exn_thrown;void*_tmpBDF=_tmpBDC;void*_tmpBE1;_LL6AF: {struct Cyc_Dict_Absent_exn_struct*_tmpBE0=(struct Cyc_Dict_Absent_exn_struct*)_tmpBDF;if(_tmpBE0->tag != Cyc_Dict_Absent)goto _LL6B1;}_LL6B0:
# 4644
{const char*_tmp137A;void*_tmp1379[1];struct Cyc_String_pa_PrintArg_struct _tmp1378;(_tmp1378.tag=0,((_tmp1378.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA08)),((_tmp1379[0]=& _tmp1378,Cyc_Tcutil_terr(loc,((_tmp137A="unbound typedef name %s",_tag_dyneither(_tmp137A,sizeof(char),24))),_tag_dyneither(_tmp1379,sizeof(void*),1)))))));}
return cvtenv;_LL6B1: _tmpBE1=_tmpBDF;_LL6B2:(void)_rethrow(_tmpBE1);_LL6AE:;}};}
# 4647
*_tmpA0A=td;
# 4649
_tmpA08[0]=(td->name)[0];{
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;
struct _RegionHandle _tmpBE5=_new_region("temp");struct _RegionHandle*temp=& _tmpBE5;_push_region(temp);{
struct Cyc_List_List*inst=0;
# 4655
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,td);
# 4660
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{
struct _tuple16*_tmp137D;struct Cyc_List_List*_tmp137C;inst=((_tmp137C=_region_malloc(temp,sizeof(*_tmp137C)),((_tmp137C->hd=((_tmp137D=_region_malloc(temp,sizeof(*_tmp137D)),((_tmp137D->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp137D->f2=(void*)ts->hd,_tmp137D)))))),((_tmp137C->tl=inst,_tmp137C))))));};}
# 4664
if(ts != 0){
const char*_tmp1381;void*_tmp1380[1];struct Cyc_String_pa_PrintArg_struct _tmp137F;(_tmp137F.tag=0,((_tmp137F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA08)),((_tmp1380[0]=& _tmp137F,Cyc_Tcutil_terr(loc,((_tmp1381="too many parameters for typedef %s",_tag_dyneither(_tmp1381,sizeof(char),35))),_tag_dyneither(_tmp1380,sizeof(void*),1)))))));}
if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;
# 4669
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp1382;hidden_ts=((_tmp1382=_cycalloc(sizeof(*_tmp1382)),((_tmp1382->hd=e,((_tmp1382->tl=hidden_ts,_tmp1382))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);{
struct _tuple16*_tmp1385;struct Cyc_List_List*_tmp1384;inst=((_tmp1384=_cycalloc(sizeof(*_tmp1384)),((_tmp1384->hd=((_tmp1385=_cycalloc(sizeof(*_tmp1385)),((_tmp1385->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1385->f2=e,_tmp1385)))))),((_tmp1384->tl=inst,_tmp1384))))));};}
# 4677
*_tmpA09=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4679
if(td->defn != 0){
void*new_typ=
inst == 0?(void*)_check_null(td->defn):
 Cyc_Tcutil_rsubstitute(temp,inst,(void*)_check_null(td->defn));
*_tmpA0B=new_typ;}}
# 4686
_npop_handler(0);goto _LL5A6;
# 4652
;_pop_region(temp);};}_LL5CD: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpA0C=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp9C9;if(_tmpA0C->tag != 22)goto _LL5CF;}_LL5CE:
# 4687
 goto _LL5D0;_LL5CF: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpA0D=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp9C9;if(_tmpA0D->tag != 21)goto _LL5D1;}_LL5D0:
 goto _LL5D2;_LL5D1: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmpA0E=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp9C9;if(_tmpA0E->tag != 20)goto _LL5D3;}_LL5D2:
 goto _LL5A6;_LL5D3: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA0F=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp9C9;if(_tmpA0F->tag != 15)goto _LL5D5;else{_tmpA10=(void*)_tmpA0F->f1;}}_LL5D4:
# 4691
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmpA10,1);goto _LL5A6;_LL5D5: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpA11=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp9C9;if(_tmpA11->tag != 16)goto _LL5D7;else{_tmpA12=(void*)_tmpA11->f1;_tmpA13=(void*)_tmpA11->f2;}}_LL5D6:
# 4694
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmpA12,0);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmpA13,1);
goto _LL5A6;_LL5D7: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpA14=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp9C9;if(_tmpA14->tag != 23)goto _LL5D9;else{_tmpA15=(void*)_tmpA14->f1;}}_LL5D8:
# 4698
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmpA15,1);goto _LL5A6;_LL5D9: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpA16=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp9C9;if(_tmpA16->tag != 25)goto _LL5DB;else{_tmpA17=(void*)_tmpA16->f1;}}_LL5DA:
# 4700
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpA17,1);goto _LL5A6;_LL5DB: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpA18=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp9C9;if(_tmpA18->tag != 24)goto _LL5A6;else{_tmpA19=_tmpA18->f1;}}_LL5DC:
# 4702
 for(0;_tmpA19 != 0;_tmpA19=_tmpA19->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmpA19->hd,1);}
goto _LL5A6;_LL5A6:;}
# 4706
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind)){
{void*_tmpBEE=t;struct Cyc_Core_Opt*_tmpBF0;void*_tmpBF1;_LL6B4: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpBEF=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpBEE;if(_tmpBEF->tag != 1)goto _LL6B6;else{_tmpBF0=_tmpBEF->f1;_tmpBF1=(void*)_tmpBEF->f2;}}_LL6B5: {
# 4709
struct _dyneither_ptr s;
{struct Cyc_Core_Opt*_tmpBF2=_tmpBF0;struct Cyc_Absyn_Kind*_tmpBF3;_LL6B9: if(_tmpBF2 != 0)goto _LL6BB;_LL6BA:
{const char*_tmp1386;s=((_tmp1386="kind=NULL ",_tag_dyneither(_tmp1386,sizeof(char),11)));}goto _LL6B8;_LL6BB: if(_tmpBF2 == 0)goto _LL6B8;_tmpBF3=(struct Cyc_Absyn_Kind*)_tmpBF2->v;_LL6BC:
{const char*_tmp138A;void*_tmp1389[1];struct Cyc_String_pa_PrintArg_struct _tmp1388;s=(struct _dyneither_ptr)((_tmp1388.tag=0,((_tmp1388.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmpBF3)),((_tmp1389[0]=& _tmp1388,Cyc_aprintf(((_tmp138A="kind=%s ",_tag_dyneither(_tmp138A,sizeof(char),9))),_tag_dyneither(_tmp1389,sizeof(void*),1))))))));}goto _LL6B8;_LL6B8:;}
# 4714
if(_tmpBF1 == 0){
const char*_tmp138E;void*_tmp138D[1];struct Cyc_String_pa_PrintArg_struct _tmp138C;s=(struct _dyneither_ptr)((_tmp138C.tag=0,((_tmp138C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp138D[0]=& _tmp138C,Cyc_aprintf(((_tmp138E="%s ref=NULL",_tag_dyneither(_tmp138E,sizeof(char),12))),_tag_dyneither(_tmp138D,sizeof(void*),1))))))));}else{
# 4717
const char*_tmp1393;void*_tmp1392[2];struct Cyc_String_pa_PrintArg_struct _tmp1391;struct Cyc_String_pa_PrintArg_struct _tmp1390;s=(struct _dyneither_ptr)((_tmp1390.tag=0,((_tmp1390.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpBF1)),((_tmp1391.tag=0,((_tmp1391.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp1392[0]=& _tmp1391,((_tmp1392[1]=& _tmp1390,Cyc_aprintf(((_tmp1393="%s ref=%s",_tag_dyneither(_tmp1393,sizeof(char),10))),_tag_dyneither(_tmp1392,sizeof(void*),2))))))))))))));}
# 4719
goto _LL6B3;}_LL6B6:;_LL6B7:
 goto _LL6B3;_LL6B3:;}{
# 4722
const char*_tmp1399;void*_tmp1398[3];struct Cyc_String_pa_PrintArg_struct _tmp1397;struct Cyc_String_pa_PrintArg_struct _tmp1396;struct Cyc_String_pa_PrintArg_struct _tmp1395;(_tmp1395.tag=0,((_tmp1395.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(expected_kind)),((_tmp1396.tag=0,((_tmp1396.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmp1397.tag=0,((_tmp1397.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1398[0]=& _tmp1397,((_tmp1398[1]=& _tmp1396,((_tmp1398[2]=& _tmp1395,Cyc_Tcutil_terr(loc,((_tmp1399="type %s has kind %s but as used here needs kind %s",_tag_dyneither(_tmp1399,sizeof(char),51))),_tag_dyneither(_tmp1398,sizeof(void*),3)))))))))))))))))));};}
# 4725
return cvtenv;}
# 4733
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){
# 4735
{void*_tmpC04=e->r;void*_tmpC05=_tmpC04;struct Cyc_List_List*_tmpC0B;struct Cyc_Absyn_Exp*_tmpC0D;struct Cyc_Absyn_Exp*_tmpC0E;struct Cyc_Absyn_Exp*_tmpC0F;struct Cyc_Absyn_Exp*_tmpC11;struct Cyc_Absyn_Exp*_tmpC12;struct Cyc_Absyn_Exp*_tmpC14;struct Cyc_Absyn_Exp*_tmpC15;struct Cyc_Absyn_Exp*_tmpC17;struct Cyc_Absyn_Exp*_tmpC18;void*_tmpC1A;struct Cyc_Absyn_Exp*_tmpC1B;void*_tmpC1D;void*_tmpC1F;void*_tmpC21;struct Cyc_Absyn_Exp*_tmpC23;_LL6BE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpC06=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpC05;if(_tmpC06->tag != 0)goto _LL6C0;}_LL6BF:
 goto _LL6C1;_LL6C0: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpC07=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpC05;if(_tmpC07->tag != 31)goto _LL6C2;}_LL6C1:
 goto _LL6C3;_LL6C2: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpC08=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpC05;if(_tmpC08->tag != 32)goto _LL6C4;}_LL6C3:
 goto _LL6C5;_LL6C4: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpC09=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC05;if(_tmpC09->tag != 1)goto _LL6C6;}_LL6C5:
 goto _LL6BD;_LL6C6: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpC0A=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC05;if(_tmpC0A->tag != 2)goto _LL6C8;else{_tmpC0B=_tmpC0A->f2;}}_LL6C7:
# 4741
 for(0;_tmpC0B != 0;_tmpC0B=_tmpC0B->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmpC0B->hd,te,cvtenv);}
goto _LL6BD;_LL6C8: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpC0C=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpC05;if(_tmpC0C->tag != 5)goto _LL6CA;else{_tmpC0D=_tmpC0C->f1;_tmpC0E=_tmpC0C->f2;_tmpC0F=_tmpC0C->f3;}}_LL6C9:
# 4745
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC0D,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC0E,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC0F,te,cvtenv);
goto _LL6BD;_LL6CA: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpC10=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpC05;if(_tmpC10->tag != 6)goto _LL6CC;else{_tmpC11=_tmpC10->f1;_tmpC12=_tmpC10->f2;}}_LL6CB:
 _tmpC14=_tmpC11;_tmpC15=_tmpC12;goto _LL6CD;_LL6CC: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmpC13=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpC05;if(_tmpC13->tag != 7)goto _LL6CE;else{_tmpC14=_tmpC13->f1;_tmpC15=_tmpC13->f2;}}_LL6CD:
 _tmpC17=_tmpC14;_tmpC18=_tmpC15;goto _LL6CF;_LL6CE: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpC16=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpC05;if(_tmpC16->tag != 8)goto _LL6D0;else{_tmpC17=_tmpC16->f1;_tmpC18=_tmpC16->f2;}}_LL6CF:
# 4752
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC17,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC18,te,cvtenv);
goto _LL6BD;_LL6D0: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpC19=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpC05;if(_tmpC19->tag != 13)goto _LL6D2;else{_tmpC1A=(void*)_tmpC19->f1;_tmpC1B=_tmpC19->f2;}}_LL6D1:
# 4756
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC1B,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpC1A,1);
goto _LL6BD;_LL6D2: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpC1C=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpC05;if(_tmpC1C->tag != 18)goto _LL6D4;else{_tmpC1D=(void*)_tmpC1C->f1;}}_LL6D3:
 _tmpC1F=_tmpC1D;goto _LL6D5;_LL6D4: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpC1E=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpC05;if(_tmpC1E->tag != 16)goto _LL6D6;else{_tmpC1F=(void*)_tmpC1E->f1;}}_LL6D5:
# 4761
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpC1F,1);
goto _LL6BD;_LL6D6: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmpC20=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpC05;if(_tmpC20->tag != 38)goto _LL6D8;else{_tmpC21=(void*)_tmpC20->f1;}}_LL6D7:
# 4764
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpC21,1);
goto _LL6BD;_LL6D8: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpC22=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpC05;if(_tmpC22->tag != 17)goto _LL6DA;else{_tmpC23=_tmpC22->f1;}}_LL6D9:
# 4767
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC23,te,cvtenv);
goto _LL6BD;_LL6DA:;_LL6DB: {
# 4770
const char*_tmp139C;void*_tmp139B;(_tmp139B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp139C="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",_tag_dyneither(_tmp139C,sizeof(char),66))),_tag_dyneither(_tmp139B,sizeof(void*),0)));}_LL6BD:;}
# 4772
return cvtenv;}
# 4775
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,void*t){
# 4780
struct Cyc_List_List*_tmpC26=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1);
# 4783
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
struct _tuple28 _tmpC27=*((struct _tuple28*)vs->hd);struct Cyc_Absyn_Tvar*_tmpC29;struct _tuple28 _tmpC28=_tmpC27;_tmpC29=_tmpC28.f1;
cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(_tmpC26,_tmpC29);}}
# 4791
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmpC2A=*((struct _tuple29*)evs->hd);void*_tmpC2C;struct _tuple29 _tmpC2B=_tmpC2A;_tmpC2C=_tmpC2B.f1;{
void*_tmpC2D=Cyc_Tcutil_compress(_tmpC2C);void*_tmpC2E=_tmpC2D;struct Cyc_Core_Opt**_tmpC30;_LL6DD: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpC2F=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpC2E;if(_tmpC2F->tag != 1)goto _LL6DF;else{_tmpC30=(struct Cyc_Core_Opt**)& _tmpC2F->f4;}}_LL6DE:
# 4795
 if(*_tmpC30 == 0){
struct Cyc_Core_Opt*_tmp139D;*_tmpC30=((_tmp139D=_cycalloc(sizeof(*_tmp139D)),((_tmp139D->v=_tmpC26,_tmp139D))));}else{
# 4799
struct Cyc_List_List*_tmpC32=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmpC30))->v;
struct Cyc_List_List*_tmpC33=0;
for(0;_tmpC32 != 0;_tmpC32=_tmpC32->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmpC26,(struct Cyc_Absyn_Tvar*)_tmpC32->hd)){
struct Cyc_List_List*_tmp139E;_tmpC33=((_tmp139E=_cycalloc(sizeof(*_tmp139E)),((_tmp139E->hd=(struct Cyc_Absyn_Tvar*)_tmpC32->hd,((_tmp139E->tl=_tmpC33,_tmp139E))))));}}{
struct Cyc_Core_Opt*_tmp139F;*_tmpC30=((_tmp139F=_cycalloc(sizeof(*_tmp139F)),((_tmp139F->v=_tmpC33,_tmp139F))));};}
# 4806
goto _LL6DC;_LL6DF:;_LL6E0:
 goto _LL6DC;_LL6DC:;};}}
# 4810
return cvt;}
# 4816
void Cyc_Tcutil_check_free_evars(struct Cyc_List_List*free_evars,void*in_t,unsigned int loc){
# 4818
for(0;free_evars != 0;free_evars=free_evars->tl){
void*e=(void*)free_evars->hd;
struct Cyc_Absyn_Kind*_tmpC36=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind*_tmpC37=_tmpC36;_LL6E2: if(_tmpC37->kind != Cyc_Absyn_RgnKind)goto _LL6E4;if(_tmpC37->aliasqual != Cyc_Absyn_Unique)goto _LL6E4;_LL6E3:
# 4822
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp13A2;void*_tmp13A1;(_tmp13A1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13A2="can't unify evar with unique region!",_tag_dyneither(_tmp13A2,sizeof(char),37))),_tag_dyneither(_tmp13A1,sizeof(void*),0)));}
goto _LL6E1;_LL6E4: if(_tmpC37->kind != Cyc_Absyn_RgnKind)goto _LL6E6;if(_tmpC37->aliasqual != Cyc_Absyn_Aliasable)goto _LL6E6;_LL6E5:
 goto _LL6E7;_LL6E6: if(_tmpC37->kind != Cyc_Absyn_RgnKind)goto _LL6E8;if(_tmpC37->aliasqual != Cyc_Absyn_Top)goto _LL6E8;_LL6E7:
# 4827
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp13A5;void*_tmp13A4;(_tmp13A4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13A5="can't unify evar with heap!",_tag_dyneither(_tmp13A5,sizeof(char),28))),_tag_dyneither(_tmp13A4,sizeof(void*),0)));}
goto _LL6E1;_LL6E8: if(_tmpC37->kind != Cyc_Absyn_EffKind)goto _LL6EA;_LL6E9:
# 4830
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*_tmp13A8;void*_tmp13A7;(_tmp13A7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13A8="can't unify evar with {}!",_tag_dyneither(_tmp13A8,sizeof(char),26))),_tag_dyneither(_tmp13A7,sizeof(void*),0)));}
goto _LL6E1;_LL6EA:;_LL6EB:
# 4833
{const char*_tmp13AD;void*_tmp13AC[2];struct Cyc_String_pa_PrintArg_struct _tmp13AB;struct Cyc_String_pa_PrintArg_struct _tmp13AA;(_tmp13AA.tag=0,((_tmp13AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(in_t)),((_tmp13AB.tag=0,((_tmp13AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e)),((_tmp13AC[0]=& _tmp13AB,((_tmp13AC[1]=& _tmp13AA,Cyc_Tcutil_terr(loc,((_tmp13AD="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(_tmp13AD,sizeof(char),52))),_tag_dyneither(_tmp13AC,sizeof(void*),2)))))))))))));}
goto _LL6E1;_LL6E1:;}}
# 4845
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_List_List*_tmpC42=Cyc_Tcenv_lookup_type_vars(te);
struct Cyc_Absyn_Kind*expected_kind=generalize_evars?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
struct _RegionHandle _tmpC43=_new_region("temp");struct _RegionHandle*temp=& _tmpC43;_push_region(temp);
{struct Cyc_Tcutil_CVTEnv _tmp13AE;struct Cyc_Tcutil_CVTEnv _tmpC44=Cyc_Tcutil_check_valid_type(loc,te,(
(_tmp13AE.r=temp,((_tmp13AE.kind_env=_tmpC42,((_tmp13AE.free_vars=0,((_tmp13AE.free_evars=0,((_tmp13AE.generalize_evars=generalize_evars,((_tmp13AE.fn_result=0,_tmp13AE)))))))))))),expected_kind,t);
# 4854
struct Cyc_List_List*_tmpC45=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmpC44.free_vars);
struct Cyc_List_List*_tmpC46=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmpC44.free_evars);
# 4858
if(_tmpC42 != 0){
struct Cyc_List_List*_tmpC47=0;
{struct Cyc_List_List*_tmpC48=_tmpC45;for(0;(unsigned int)_tmpC48;_tmpC48=_tmpC48->tl){
struct Cyc_Absyn_Tvar*_tmpC49=(struct Cyc_Absyn_Tvar*)_tmpC48->hd;
int found=0;
{struct Cyc_List_List*_tmpC4A=_tmpC42;for(0;(unsigned int)_tmpC4A;_tmpC4A=_tmpC4A->tl){
if(Cyc_Absyn_tvar_cmp(_tmpC49,(struct Cyc_Absyn_Tvar*)_tmpC4A->hd)== 0){found=1;break;}}}
if(!found){
struct Cyc_List_List*_tmp13AF;_tmpC47=((_tmp13AF=_region_malloc(temp,sizeof(*_tmp13AF)),((_tmp13AF->hd=(struct Cyc_Absyn_Tvar*)_tmpC48->hd,((_tmp13AF->tl=_tmpC47,_tmp13AF))))));}}}
# 4868
_tmpC45=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpC47);}
# 4874
{struct Cyc_List_List*x=_tmpC45;for(0;x != 0;x=x->tl){
void*_tmpC4C=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);void*_tmpC4D=_tmpC4C;struct Cyc_Core_Opt**_tmpC4F;struct Cyc_Core_Opt**_tmpC51;struct Cyc_Core_Opt**_tmpC53;struct Cyc_Core_Opt**_tmpC55;struct Cyc_Core_Opt**_tmpC57;struct Cyc_Core_Opt**_tmpC59;struct Cyc_Core_Opt**_tmpC5B;struct Cyc_Absyn_Kind*_tmpC5C;enum Cyc_Absyn_AliasQual _tmpC5E;_LL6ED: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpC4E=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpC4D;if(_tmpC4E->tag != 1)goto _LL6EF;else{_tmpC4F=(struct Cyc_Core_Opt**)& _tmpC4E->f1;}}_LL6EE:
 _tmpC51=_tmpC4F;goto _LL6F0;_LL6EF: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC50=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC4D;if(_tmpC50->tag != 2)goto _LL6F1;else{_tmpC51=(struct Cyc_Core_Opt**)& _tmpC50->f1;if((_tmpC50->f2)->kind != Cyc_Absyn_BoxKind)goto _LL6F1;if((_tmpC50->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6F1;}}_LL6F0:
 _tmpC53=_tmpC51;goto _LL6F2;_LL6F1: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC52=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC4D;if(_tmpC52->tag != 2)goto _LL6F3;else{_tmpC53=(struct Cyc_Core_Opt**)& _tmpC52->f1;if((_tmpC52->f2)->kind != Cyc_Absyn_MemKind)goto _LL6F3;if((_tmpC52->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6F3;}}_LL6F2:
 _tmpC55=_tmpC53;goto _LL6F4;_LL6F3: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC54=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC4D;if(_tmpC54->tag != 2)goto _LL6F5;else{_tmpC55=(struct Cyc_Core_Opt**)& _tmpC54->f1;if((_tmpC54->f2)->kind != Cyc_Absyn_MemKind)goto _LL6F5;if((_tmpC54->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL6F5;}}_LL6F4:
# 4880
*_tmpC55=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL6EC;_LL6F5: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC56=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC4D;if(_tmpC56->tag != 2)goto _LL6F7;else{_tmpC57=(struct Cyc_Core_Opt**)& _tmpC56->f1;if((_tmpC56->f2)->kind != Cyc_Absyn_MemKind)goto _LL6F7;if((_tmpC56->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL6F7;}}_LL6F6:
# 4882
*_tmpC57=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL6EC;_LL6F7: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC58=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC4D;if(_tmpC58->tag != 2)goto _LL6F9;else{_tmpC59=(struct Cyc_Core_Opt**)& _tmpC58->f1;if((_tmpC58->f2)->kind != Cyc_Absyn_RgnKind)goto _LL6F9;if((_tmpC58->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6F9;}}_LL6F8:
# 4884
*_tmpC59=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL6EC;_LL6F9: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC5A=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC4D;if(_tmpC5A->tag != 2)goto _LL6FB;else{_tmpC5B=(struct Cyc_Core_Opt**)& _tmpC5A->f1;_tmpC5C=_tmpC5A->f2;}}_LL6FA:
# 4886
*_tmpC5B=Cyc_Tcutil_kind_to_bound_opt(_tmpC5C);goto _LL6EC;_LL6FB: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpC5D=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpC4D;if(_tmpC5D->tag != 0)goto _LL6FD;else{if((_tmpC5D->f1)->kind != Cyc_Absyn_MemKind)goto _LL6FD;_tmpC5E=(_tmpC5D->f1)->aliasqual;}}_LL6FC:
# 4888
{const char*_tmp13B6;void*_tmp13B5[2];struct Cyc_String_pa_PrintArg_struct _tmp13B4;struct Cyc_Absyn_Kind*_tmp13B3;struct Cyc_String_pa_PrintArg_struct _tmp13B2;(_tmp13B2.tag=0,((_tmp13B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(((_tmp13B3=_cycalloc_atomic(sizeof(*_tmp13B3)),((_tmp13B3->kind=Cyc_Absyn_MemKind,((_tmp13B3->aliasqual=_tmpC5E,_tmp13B3)))))))),((_tmp13B4.tag=0,((_tmp13B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd)),((_tmp13B5[0]=& _tmp13B4,((_tmp13B5[1]=& _tmp13B2,Cyc_Tcutil_terr(loc,((_tmp13B6="type variable %s cannot have kind %s",_tag_dyneither(_tmp13B6,sizeof(char),37))),_tag_dyneither(_tmp13B5,sizeof(void*),2)))))))))))));}
goto _LL6EC;_LL6FD:;_LL6FE:
 goto _LL6EC;_LL6EC:;}}
# 4895
if(_tmpC45 != 0  || _tmpC46 != 0){
{void*_tmpC64=Cyc_Tcutil_compress(t);void*_tmpC65=_tmpC64;struct Cyc_List_List**_tmpC67;_LL700: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpC66=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC65;if(_tmpC66->tag != 9)goto _LL702;else{_tmpC67=(struct Cyc_List_List**)&(_tmpC66->f1).tvars;}}_LL701:
# 4898
 if(*_tmpC67 == 0){
# 4900
*_tmpC67=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmpC45);
_tmpC45=0;}
# 4903
goto _LL6FF;_LL702:;_LL703:
 goto _LL6FF;_LL6FF:;}
# 4906
if(_tmpC45 != 0){
const char*_tmp13BA;void*_tmp13B9[1];struct Cyc_String_pa_PrintArg_struct _tmp13B8;(_tmp13B8.tag=0,((_tmp13B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmpC45->hd)->name),((_tmp13B9[0]=& _tmp13B8,Cyc_Tcutil_terr(loc,((_tmp13BA="unbound type variable %s",_tag_dyneither(_tmp13BA,sizeof(char),25))),_tag_dyneither(_tmp13B9,sizeof(void*),1)))))));}
Cyc_Tcutil_check_free_evars(_tmpC46,t,loc);}}
# 4850
;_pop_region(temp);}
# 4917
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 4920
Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);
{void*_tmpC6C=Cyc_Tcutil_compress(t);void*_tmpC6D=_tmpC6C;struct Cyc_List_List*_tmpC6F;void*_tmpC70;struct Cyc_Absyn_Tqual _tmpC71;void*_tmpC72;struct Cyc_List_List*_tmpC73;struct Cyc_Absyn_Exp*_tmpC74;struct Cyc_List_List*_tmpC75;struct Cyc_Absyn_Exp*_tmpC76;struct Cyc_List_List*_tmpC77;_LL705: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpC6E=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC6D;if(_tmpC6E->tag != 9)goto _LL707;else{_tmpC6F=(_tmpC6E->f1).tvars;_tmpC70=(_tmpC6E->f1).effect;_tmpC71=(_tmpC6E->f1).ret_tqual;_tmpC72=(_tmpC6E->f1).ret_typ;_tmpC73=(_tmpC6E->f1).args;_tmpC74=(_tmpC6E->f1).requires_clause;_tmpC75=(_tmpC6E->f1).requires_relns;_tmpC76=(_tmpC6E->f1).ensures_clause;_tmpC77=(_tmpC6E->f1).ensures_relns;}}_LL706:
# 4923
 fd->tvs=_tmpC6F;
fd->effect=_tmpC70;
{struct Cyc_List_List*_tmpC78=fd->args;for(0;_tmpC78 != 0;(_tmpC78=_tmpC78->tl,_tmpC73=_tmpC73->tl)){
# 4927
(*((struct _tuple10*)_tmpC78->hd)).f2=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmpC73))->hd)).f2;
(*((struct _tuple10*)_tmpC78->hd)).f3=(*((struct _tuple10*)_tmpC73->hd)).f3;}}
# 4930
fd->ret_tqual=_tmpC71;
fd->ret_type=_tmpC72;
(fd->ret_tqual).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmpC72);
fd->requires_clause=_tmpC74;
fd->requires_relns=_tmpC75;
fd->ensures_clause=_tmpC76;
fd->ensures_relns=_tmpC77;
goto _LL704;_LL707:;_LL708: {
const char*_tmp13BD;void*_tmp13BC;(_tmp13BC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13BD="check_fndecl_valid_type: not a FnType",_tag_dyneither(_tmp13BD,sizeof(char),38))),_tag_dyneither(_tmp13BC,sizeof(void*),0)));}_LL704:;}{
# 4941
struct _RegionHandle _tmpC7B=_new_region("r");struct _RegionHandle*r=& _tmpC7B;_push_region(r);{
const char*_tmp13BE;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _dyneither_ptr*(*)(struct _tuple10*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,(
(_tmp13BE="function declaration has repeated parameter",_tag_dyneither(_tmp13BE,sizeof(char),44))));}
# 4946
fd->cached_typ=t;
# 4941
;_pop_region(r);};}
# 4951
void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,void*t){
# 4954
struct _RegionHandle _tmpC7D=_new_region("r");struct _RegionHandle*r=& _tmpC7D;_push_region(r);
{struct Cyc_Tcutil_CVTEnv _tmp13BF;struct Cyc_Tcutil_CVTEnv _tmpC7E=Cyc_Tcutil_check_valid_type(loc,te,(
(_tmp13BF.r=r,((_tmp13BF.kind_env=bound_tvars,((_tmp13BF.free_vars=0,((_tmp13BF.free_evars=0,((_tmp13BF.generalize_evars=0,((_tmp13BF.fn_result=0,_tmp13BF)))))))))))),expected_kind,t);
# 4958
struct Cyc_List_List*_tmpC7F=Cyc_Tcutil_remove_bound_tvars(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmpC7E.free_vars),bound_tvars);
# 4960
struct Cyc_List_List*_tmpC80=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmpC7E.free_evars);
{struct Cyc_List_List*fs=_tmpC7F;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmpC81=((struct Cyc_Absyn_Tvar*)fs->hd)->name;
const char*_tmp13C4;void*_tmp13C3[2];struct Cyc_String_pa_PrintArg_struct _tmp13C2;struct Cyc_String_pa_PrintArg_struct _tmp13C1;(_tmp13C1.tag=0,((_tmp13C1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp13C2.tag=0,((_tmp13C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpC81),((_tmp13C3[0]=& _tmp13C2,((_tmp13C3[1]=& _tmp13C1,Cyc_Tcutil_terr(loc,((_tmp13C4="unbound type variable %s in type %s",_tag_dyneither(_tmp13C4,sizeof(char),36))),_tag_dyneither(_tmp13C3,sizeof(void*),2)))))))))))));}}
# 4965
if(!allow_evars)
Cyc_Tcutil_check_free_evars(_tmpC80,t,loc);}
# 4955
;_pop_region(r);}
# 4970
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
tv->identity=Cyc_Tcutil_new_tvar_id();}
# 4975
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 4981
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){
# 4984
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0){
const char*_tmp13C9;void*_tmp13C8[2];struct Cyc_String_pa_PrintArg_struct _tmp13C7;struct Cyc_String_pa_PrintArg_struct _tmp13C6;(_tmp13C6.tag=0,((_tmp13C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string(vs->hd)),((_tmp13C7.tag=0,((_tmp13C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp13C8[0]=& _tmp13C7,((_tmp13C8[1]=& _tmp13C6,Cyc_Tcutil_terr(loc,((_tmp13C9="%s: %s",_tag_dyneither(_tmp13C9,sizeof(char),7))),_tag_dyneither(_tmp13C8,sizeof(void*),2)))))))))))));}}}}
# 4990
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s){
return*s;}
# 4994
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr msg){
((void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 4998
void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
const char*_tmp13CA;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,((_tmp13CA="duplicate type variable",_tag_dyneither(_tmp13CA,sizeof(char),24))));}struct _tuple32{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple33{struct Cyc_List_List*f1;void*f2;};struct _tuple34{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 5012 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 5017
struct _RegionHandle _tmpC8C=_new_region("temp");struct _RegionHandle*temp=& _tmpC8C;_push_region(temp);
# 5021
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
const char*_tmp13CB;if(Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,((_tmp13CB="",_tag_dyneither(_tmp13CB,sizeof(char),1))))!= 0){
struct _tuple32*_tmp13CE;struct Cyc_List_List*_tmp13CD;fields=((_tmp13CD=_region_malloc(temp,sizeof(*_tmp13CD)),((_tmp13CD->hd=((_tmp13CE=_region_malloc(temp,sizeof(*_tmp13CE)),((_tmp13CE->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,((_tmp13CE->f2=0,_tmp13CE)))))),((_tmp13CD->tl=fields,_tmp13CD))))));}}}
# 5026
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 5028
const char*_tmp13D0;const char*_tmp13CF;struct _dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?(_tmp13D0="struct",_tag_dyneither(_tmp13D0,sizeof(char),7)):((_tmp13CF="union",_tag_dyneither(_tmp13CF,sizeof(char),6)));
# 5031
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple33*_tmpC90=(struct _tuple33*)des->hd;struct Cyc_List_List*_tmpC92;void*_tmpC93;struct _tuple33*_tmpC91=_tmpC90;_tmpC92=_tmpC91->f1;_tmpC93=_tmpC91->f2;
if(_tmpC92 == 0){
# 5036
struct Cyc_List_List*_tmpC94=fields;
for(0;_tmpC94 != 0;_tmpC94=_tmpC94->tl){
if(!(*((struct _tuple32*)_tmpC94->hd)).f2){
(*((struct _tuple32*)_tmpC94->hd)).f2=1;
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp13D6;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp13D5;struct Cyc_List_List*_tmp13D4;(*((struct _tuple33*)des->hd)).f1=((_tmp13D4=_cycalloc(sizeof(*_tmp13D4)),((_tmp13D4->hd=(void*)((_tmp13D6=_cycalloc(sizeof(*_tmp13D6)),((_tmp13D6[0]=((_tmp13D5.tag=1,((_tmp13D5.f1=((*((struct _tuple32*)_tmpC94->hd)).f1)->name,_tmp13D5)))),_tmp13D6)))),((_tmp13D4->tl=0,_tmp13D4))))));}
{struct _tuple34*_tmp13D9;struct Cyc_List_List*_tmp13D8;ans=((_tmp13D8=_region_malloc(rgn,sizeof(*_tmp13D8)),((_tmp13D8->hd=((_tmp13D9=_region_malloc(rgn,sizeof(*_tmp13D9)),((_tmp13D9->f1=(*((struct _tuple32*)_tmpC94->hd)).f1,((_tmp13D9->f2=_tmpC93,_tmp13D9)))))),((_tmp13D8->tl=ans,_tmp13D8))))));}
break;}}
# 5044
if(_tmpC94 == 0){
const char*_tmp13DD;void*_tmp13DC[1];struct Cyc_String_pa_PrintArg_struct _tmp13DB;(_tmp13DB.tag=0,((_tmp13DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp13DC[0]=& _tmp13DB,Cyc_Tcutil_terr(loc,((_tmp13DD="too many arguments to %s",_tag_dyneither(_tmp13DD,sizeof(char),25))),_tag_dyneither(_tmp13DC,sizeof(void*),1)))))));}}else{
if(_tmpC92->tl != 0){
# 5048
const char*_tmp13E0;void*_tmp13DF;(_tmp13DF=0,Cyc_Tcutil_terr(loc,((_tmp13E0="multiple designators are not yet supported",_tag_dyneither(_tmp13E0,sizeof(char),43))),_tag_dyneither(_tmp13DF,sizeof(void*),0)));}else{
# 5051
void*_tmpC9F=(void*)_tmpC92->hd;void*_tmpCA0=_tmpC9F;struct _dyneither_ptr*_tmpCA3;_LL70A: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpCA1=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpCA0;if(_tmpCA1->tag != 0)goto _LL70C;}_LL70B:
# 5053
{const char*_tmp13E4;void*_tmp13E3[1];struct Cyc_String_pa_PrintArg_struct _tmp13E2;(_tmp13E2.tag=0,((_tmp13E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp13E3[0]=& _tmp13E2,Cyc_Tcutil_terr(loc,((_tmp13E4="array designator used in argument to %s",_tag_dyneither(_tmp13E4,sizeof(char),40))),_tag_dyneither(_tmp13E3,sizeof(void*),1)))))));}
goto _LL709;_LL70C: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpCA2=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpCA0;if(_tmpCA2->tag != 1)goto _LL709;else{_tmpCA3=_tmpCA2->f1;}}_LL70D: {
# 5056
struct Cyc_List_List*_tmpCA7=fields;
for(0;_tmpCA7 != 0;_tmpCA7=_tmpCA7->tl){
if(Cyc_strptrcmp(_tmpCA3,((*((struct _tuple32*)_tmpCA7->hd)).f1)->name)== 0){
if((*((struct _tuple32*)_tmpCA7->hd)).f2){
const char*_tmp13E8;void*_tmp13E7[1];struct Cyc_String_pa_PrintArg_struct _tmp13E6;(_tmp13E6.tag=0,((_tmp13E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpCA3),((_tmp13E7[0]=& _tmp13E6,Cyc_Tcutil_terr(loc,((_tmp13E8="member %s has already been used as an argument",_tag_dyneither(_tmp13E8,sizeof(char),47))),_tag_dyneither(_tmp13E7,sizeof(void*),1)))))));}
(*((struct _tuple32*)_tmpCA7->hd)).f2=1;
{struct _tuple34*_tmp13EB;struct Cyc_List_List*_tmp13EA;ans=((_tmp13EA=_region_malloc(rgn,sizeof(*_tmp13EA)),((_tmp13EA->hd=((_tmp13EB=_region_malloc(rgn,sizeof(*_tmp13EB)),((_tmp13EB->f1=(*((struct _tuple32*)_tmpCA7->hd)).f1,((_tmp13EB->f2=_tmpC93,_tmp13EB)))))),((_tmp13EA->tl=ans,_tmp13EA))))));}
break;}}
# 5065
if(_tmpCA7 == 0){
const char*_tmp13EF;void*_tmp13EE[1];struct Cyc_String_pa_PrintArg_struct _tmp13ED;(_tmp13ED.tag=0,((_tmp13ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpCA3),((_tmp13EE[0]=& _tmp13ED,Cyc_Tcutil_terr(loc,((_tmp13EF="bad field designator %s",_tag_dyneither(_tmp13EF,sizeof(char),24))),_tag_dyneither(_tmp13EE,sizeof(void*),1)))))));}
goto _LL709;}_LL709:;}}}
# 5070
if(aggr_kind == Cyc_Absyn_StructA)
# 5072
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple32*)fields->hd)).f2){
{const char*_tmp13F2;void*_tmp13F1;(_tmp13F1=0,Cyc_Tcutil_terr(loc,((_tmp13F2="too few arguments to struct",_tag_dyneither(_tmp13F2,sizeof(char),28))),_tag_dyneither(_tmp13F1,sizeof(void*),0)));}
break;}}else{
# 5079
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple32*)fields->hd)).f2){
if(found){const char*_tmp13F5;void*_tmp13F4;(_tmp13F4=0,Cyc_Tcutil_terr(loc,((_tmp13F5="only one member of a union is allowed",_tag_dyneither(_tmp13F5,sizeof(char),38))),_tag_dyneither(_tmp13F4,sizeof(void*),0)));}
found=1;}}
# 5086
if(!found){const char*_tmp13F8;void*_tmp13F7;(_tmp13F7=0,Cyc_Tcutil_terr(loc,((_tmp13F8="missing member for union",_tag_dyneither(_tmp13F8,sizeof(char),25))),_tag_dyneither(_tmp13F7,sizeof(void*),0)));}}{
# 5089
struct Cyc_List_List*_tmpCB6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0);return _tmpCB6;};};}
# 5021
;_pop_region(temp);}
# 5092
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmpCB9=Cyc_Tcutil_compress(t);void*_tmpCBA=_tmpCB9;void*_tmpCBC;union Cyc_Absyn_Constraint*_tmpCBD;_LL70F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCBB=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCBA;if(_tmpCBB->tag != 5)goto _LL711;else{_tmpCBC=(_tmpCBB->f1).elt_typ;_tmpCBD=((_tmpCBB->f1).ptr_atts).bounds;}}_LL710: {
# 5095
void*_tmpCBE=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpCBD);void*_tmpCBF=_tmpCBE;_LL714: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpCC0=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpCBF;if(_tmpCC0->tag != 0)goto _LL716;}_LL715:
# 5097
*elt_typ_dest=_tmpCBC;
return 1;_LL716:;_LL717:
 return 0;_LL713:;}_LL711:;_LL712:
# 5101
 return 0;_LL70E:;}
# 5105
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmpCC1=Cyc_Tcutil_compress(t);void*_tmpCC2=_tmpCC1;void*_tmpCC4;union Cyc_Absyn_Constraint*_tmpCC5;_LL719: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCC3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCC2;if(_tmpCC3->tag != 5)goto _LL71B;else{_tmpCC4=(_tmpCC3->f1).elt_typ;_tmpCC5=((_tmpCC3->f1).ptr_atts).zero_term;}}_LL71A:
# 5108
*elt_typ_dest=_tmpCC4;
return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpCC5);_LL71B:;_LL71C:
 return 0;_LL718:;}
# 5116
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type){
# 5118
void*_tmpCC6=Cyc_Tcutil_compress(t);void*_tmpCC7=_tmpCC6;void*_tmpCC9;union Cyc_Absyn_Constraint*_tmpCCA;union Cyc_Absyn_Constraint*_tmpCCB;void*_tmpCCD;struct Cyc_Absyn_Tqual _tmpCCE;struct Cyc_Absyn_Exp*_tmpCCF;union Cyc_Absyn_Constraint*_tmpCD0;_LL71E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCC8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCC7;if(_tmpCC8->tag != 5)goto _LL720;else{_tmpCC9=(_tmpCC8->f1).elt_typ;_tmpCCA=((_tmpCC8->f1).ptr_atts).bounds;_tmpCCB=((_tmpCC8->f1).ptr_atts).zero_term;}}_LL71F:
# 5120
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpCCB)){
*ptr_type=t;
*elt_type=_tmpCC9;
{void*_tmpCD1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmpCCA);void*_tmpCD2=_tmpCD1;_LL725: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpCD3=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpCD2;if(_tmpCD3->tag != 0)goto _LL727;}_LL726:
*is_dyneither=1;goto _LL724;_LL727:;_LL728:
*is_dyneither=0;goto _LL724;_LL724:;}
# 5127
return 1;}else{
return 0;}_LL720: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCCC=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpCC7;if(_tmpCCC->tag != 8)goto _LL722;else{_tmpCCD=(_tmpCCC->f1).elt_type;_tmpCCE=(_tmpCCC->f1).tq;_tmpCCF=(_tmpCCC->f1).num_elts;_tmpCD0=(_tmpCCC->f1).zero_term;}}_LL721:
# 5130
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpCD0)){
*elt_type=_tmpCCD;
*is_dyneither=0;
*ptr_type=Cyc_Tcutil_promote_array(t,(void*)& Cyc_Absyn_HeapRgn_val,0);
return 1;}else{
return 0;}_LL722:;_LL723:
 return 0;_LL71D:;}
# 5143
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){
# 5145
void*_tmpCD4=e1->r;void*_tmpCD5=_tmpCD4;struct Cyc_Absyn_Exp*_tmpCD8;struct Cyc_Absyn_Exp*_tmpCDA;struct Cyc_Absyn_Exp*_tmpCDC;struct Cyc_Absyn_Exp*_tmpCDE;struct Cyc_Absyn_Exp*_tmpCE0;struct Cyc_Absyn_Exp*_tmpCE2;_LL72A: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpCD6=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpCD5;if(_tmpCD6->tag != 13)goto _LL72C;}_LL72B: {
# 5147
const char*_tmp13FC;void*_tmp13FB[1];struct Cyc_String_pa_PrintArg_struct _tmp13FA;(_tmp13FA.tag=0,((_tmp13FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp13FB[0]=& _tmp13FA,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13FC="we have a cast in a lhs:  %s",_tag_dyneither(_tmp13FC,sizeof(char),29))),_tag_dyneither(_tmp13FB,sizeof(void*),1)))))));}_LL72C: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpCD7=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpCD5;if(_tmpCD7->tag != 19)goto _LL72E;else{_tmpCD8=_tmpCD7->f1;}}_LL72D:
 _tmpCDA=_tmpCD8;goto _LL72F;_LL72E: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpCD9=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpCD5;if(_tmpCD9->tag != 22)goto _LL730;else{_tmpCDA=_tmpCD9->f1;}}_LL72F:
# 5150
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpCDA->topt),ptr_type,is_dyneither,elt_type);_LL730: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpCDB=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpCD5;if(_tmpCDB->tag != 21)goto _LL732;else{_tmpCDC=_tmpCDB->f1;}}_LL731:
 _tmpCDE=_tmpCDC;goto _LL733;_LL732: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpCDD=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpCD5;if(_tmpCDD->tag != 20)goto _LL734;else{_tmpCDE=_tmpCDD->f1;}}_LL733:
# 5154
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpCDE->topt),ptr_type,is_dyneither,elt_type)){
const char*_tmp1400;void*_tmp13FF[1];struct Cyc_String_pa_PrintArg_struct _tmp13FE;(_tmp13FE.tag=0,((_tmp13FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1)),((_tmp13FF[0]=& _tmp13FE,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1400="found zero pointer aggregate member assignment: %s",_tag_dyneither(_tmp1400,sizeof(char),51))),_tag_dyneither(_tmp13FF,sizeof(void*),1)))))));}
return 0;_LL734: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpCDF=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpCD5;if(_tmpCDF->tag != 12)goto _LL736;else{_tmpCE0=_tmpCDF->f1;}}_LL735:
 _tmpCE2=_tmpCE0;goto _LL737;_LL736: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpCE1=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpCD5;if(_tmpCE1->tag != 11)goto _LL738;else{_tmpCE2=_tmpCE1->f1;}}_LL737:
# 5160
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpCE2->topt),ptr_type,is_dyneither,elt_type)){
const char*_tmp1404;void*_tmp1403[1];struct Cyc_String_pa_PrintArg_struct _tmp1402;(_tmp1402.tag=0,((_tmp1402.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1)),((_tmp1403[0]=& _tmp1402,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1404="found zero pointer instantiate/noinstantiate: %s",_tag_dyneither(_tmp1404,sizeof(char),49))),_tag_dyneither(_tmp1403,sizeof(void*),1)))))));}
return 0;_LL738: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpCE3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpCD5;if(_tmpCE3->tag != 1)goto _LL73A;}_LL739:
 return 0;_LL73A:;_LL73B: {
# 5166
const char*_tmp1408;void*_tmp1407[1];struct Cyc_String_pa_PrintArg_struct _tmp1406;(_tmp1406.tag=0,((_tmp1406.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp1407[0]=& _tmp1406,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1408="found bad lhs in is_zero_ptr_deref: %s",_tag_dyneither(_tmp1408,sizeof(char),39))),_tag_dyneither(_tmp1407,sizeof(void*),1)))))));}_LL729:;}
# 5170
int Cyc_Tcutil_is_tagged_pointer_typ(void*t){
void*ignore=(void*)& Cyc_Absyn_VoidType_val;
return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
# 5181
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
void*_tmpCF0=Cyc_Tcutil_compress(r);void*_tmpCF1=_tmpCF0;struct Cyc_Absyn_Tvar*_tmpCF5;_LL73D: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpCF2=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmpCF1;if(_tmpCF2->tag != 22)goto _LL73F;}_LL73E:
 return !must_be_unique;_LL73F: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpCF3=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmpCF1;if(_tmpCF3->tag != 21)goto _LL741;}_LL740:
 return 1;_LL741: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpCF4=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpCF1;if(_tmpCF4->tag != 2)goto _LL743;else{_tmpCF5=_tmpCF4->f1;}}_LL742: {
# 5186
struct Cyc_Absyn_Kind*_tmpCF6=Cyc_Tcutil_tvar_kind(_tmpCF5,& Cyc_Tcutil_rk);enum Cyc_Absyn_KindQual _tmpCF8;enum Cyc_Absyn_AliasQual _tmpCF9;struct Cyc_Absyn_Kind*_tmpCF7=_tmpCF6;_tmpCF8=_tmpCF7->kind;_tmpCF9=_tmpCF7->aliasqual;
if(_tmpCF8 == Cyc_Absyn_RgnKind  && (_tmpCF9 == Cyc_Absyn_Unique  || _tmpCF9 == Cyc_Absyn_Top  && !must_be_unique)){
void*_tmpCFA=Cyc_Absyn_compress_kb(_tmpCF5->kind);void*_tmpCFB=_tmpCFA;struct Cyc_Core_Opt**_tmpCFD;_LL746: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCFC=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCFB;if(_tmpCFC->tag != 2)goto _LL748;else{_tmpCFD=(struct Cyc_Core_Opt**)& _tmpCFC->f1;if((_tmpCFC->f2)->kind != Cyc_Absyn_RgnKind)goto _LL748;if((_tmpCFC->f2)->aliasqual != Cyc_Absyn_Top)goto _LL748;}}_LL747:
# 5190
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp140E;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp140D;struct Cyc_Core_Opt*_tmp140C;*_tmpCFD=((_tmp140C=_cycalloc(sizeof(*_tmp140C)),((_tmp140C->v=(void*)((_tmp140E=_cycalloc(sizeof(*_tmp140E)),((_tmp140E[0]=((_tmp140D.tag=2,((_tmp140D.f1=0,((_tmp140D.f2=& Cyc_Tcutil_rk,_tmp140D)))))),_tmp140E)))),_tmp140C))));}
return 0;_LL748:;_LL749:
 return 1;_LL745:;}
# 5195
return 0;}_LL743:;_LL744:
 return 0;_LL73C:;}
# 5202
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmpD01=Cyc_Tcutil_compress(t);void*_tmpD02=_tmpD01;void*_tmpD04;struct Cyc_Absyn_Tvar*_tmpD06;_LL74B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD03=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD02;if(_tmpD03->tag != 5)goto _LL74D;else{_tmpD04=((_tmpD03->f1).ptr_atts).rgn;}}_LL74C:
# 5205
 return Cyc_Tcutil_is_noalias_region(_tmpD04,must_be_unique);_LL74D: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpD05=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpD02;if(_tmpD05->tag != 2)goto _LL74F;else{_tmpD06=_tmpD05->f1;}}_LL74E: {
# 5207
struct Cyc_Absyn_Kind*_tmpD07=Cyc_Tcutil_tvar_kind(_tmpD06,& Cyc_Tcutil_bk);enum Cyc_Absyn_KindQual _tmpD09;enum Cyc_Absyn_AliasQual _tmpD0A;struct Cyc_Absyn_Kind*_tmpD08=_tmpD07;_tmpD09=_tmpD08->kind;_tmpD0A=_tmpD08->aliasqual;
switch(_tmpD09){case Cyc_Absyn_BoxKind: _LL751:
 goto _LL752;case Cyc_Absyn_AnyKind: _LL752: goto _LL753;case Cyc_Absyn_MemKind: _LL753:
 if(_tmpD0A == Cyc_Absyn_Unique  || _tmpD0A == Cyc_Absyn_Top  && !must_be_unique){
void*_tmpD0B=Cyc_Absyn_compress_kb(_tmpD06->kind);void*_tmpD0C=_tmpD0B;struct Cyc_Core_Opt**_tmpD0E;enum Cyc_Absyn_KindQual _tmpD0F;_LL756: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpD0D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpD0C;if(_tmpD0D->tag != 2)goto _LL758;else{_tmpD0E=(struct Cyc_Core_Opt**)& _tmpD0D->f1;_tmpD0F=(_tmpD0D->f2)->kind;if((_tmpD0D->f2)->aliasqual != Cyc_Absyn_Top)goto _LL758;}}_LL757:
# 5213
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1418;struct Cyc_Absyn_Kind*_tmp1417;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1416;struct Cyc_Core_Opt*_tmp1415;*_tmpD0E=((_tmp1415=_cycalloc(sizeof(*_tmp1415)),((_tmp1415->v=(void*)((_tmp1418=_cycalloc(sizeof(*_tmp1418)),((_tmp1418[0]=((_tmp1416.tag=2,((_tmp1416.f1=0,((_tmp1416.f2=((_tmp1417=_cycalloc_atomic(sizeof(*_tmp1417)),((_tmp1417->kind=_tmpD0F,((_tmp1417->aliasqual=Cyc_Absyn_Aliasable,_tmp1417)))))),_tmp1416)))))),_tmp1418)))),_tmp1415))));}
return 0;_LL758:;_LL759:
# 5217
 return 1;_LL755:;}
# 5220
return 0;default: _LL754:
 return 0;}}_LL74F:;_LL750:
# 5223
 return 0;_LL74A:;}
# 5226
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*rgn,void*t){
void*_tmpD14=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(_tmpD14,0))return 1;{
void*_tmpD15=_tmpD14;struct Cyc_List_List*_tmpD17;struct Cyc_Absyn_Aggrdecl**_tmpD19;struct Cyc_List_List*_tmpD1A;struct Cyc_List_List*_tmpD1C;union Cyc_Absyn_DatatypeInfoU _tmpD1F;struct Cyc_List_List*_tmpD20;union Cyc_Absyn_DatatypeFieldInfoU _tmpD22;struct Cyc_List_List*_tmpD23;_LL75C: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpD16=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpD15;if(_tmpD16->tag != 10)goto _LL75E;else{_tmpD17=_tmpD16->f1;}}_LL75D:
# 5231
 while(_tmpD17 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(*((struct _tuple12*)_tmpD17->hd)).f2))return 1;
_tmpD17=_tmpD17->tl;}
# 5235
return 0;_LL75E: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD18=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpD15;if(_tmpD18->tag != 11)goto _LL760;else{if((((_tmpD18->f1).aggr_info).KnownAggr).tag != 2)goto _LL760;_tmpD19=(struct Cyc_Absyn_Aggrdecl**)(((_tmpD18->f1).aggr_info).KnownAggr).val;_tmpD1A=(_tmpD18->f1).targs;}}_LL75F:
# 5237
 if((*_tmpD19)->impl == 0)return 0;else{
# 5239
struct Cyc_List_List*_tmpD24=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,(*_tmpD19)->tvs,_tmpD1A);
struct Cyc_List_List*_tmpD25=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpD19)->impl))->fields;
void*t;
while(_tmpD25 != 0){
t=_tmpD24 == 0?((struct Cyc_Absyn_Aggrfield*)_tmpD25->hd)->type: Cyc_Tcutil_rsubstitute(rgn,_tmpD24,((struct Cyc_Absyn_Aggrfield*)_tmpD25->hd)->type);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpD25=_tmpD25->tl;}
# 5247
return 0;}_LL760: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpD1B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpD15;if(_tmpD1B->tag != 12)goto _LL762;else{_tmpD1C=_tmpD1B->f2;}}_LL761:
# 5250
 while(_tmpD1C != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,((struct Cyc_Absyn_Aggrfield*)_tmpD1C->hd)->type))return 1;
_tmpD1C=_tmpD1C->tl;}
# 5254
return 0;_LL762: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD1D=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpD15;if(_tmpD1D->tag != 11)goto _LL764;else{if((((_tmpD1D->f1).aggr_info).UnknownAggr).tag != 1)goto _LL764;}}_LL763:
# 5257
 return 0;_LL764: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpD1E=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpD15;if(_tmpD1E->tag != 3)goto _LL766;else{_tmpD1F=(_tmpD1E->f1).datatype_info;_tmpD20=(_tmpD1E->f1).targs;}}_LL765: {
# 5259
union Cyc_Absyn_DatatypeInfoU _tmpD26=_tmpD1F;struct _tuple2*_tmpD27;int _tmpD28;struct Cyc_List_List*_tmpD29;struct Cyc_Core_Opt*_tmpD2A;_LL76B: if((_tmpD26.UnknownDatatype).tag != 1)goto _LL76D;_tmpD27=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpD26.UnknownDatatype).val).name;_tmpD28=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpD26.UnknownDatatype).val).is_extensible;_LL76C:
# 5262
 return 0;_LL76D: if((_tmpD26.KnownDatatype).tag != 2)goto _LL76A;_tmpD29=(*((struct Cyc_Absyn_Datatypedecl**)(_tmpD26.KnownDatatype).val))->tvs;_tmpD2A=(*((struct Cyc_Absyn_Datatypedecl**)(_tmpD26.KnownDatatype).val))->fields;_LL76E:
# 5265
 return 0;_LL76A:;}_LL766: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpD21=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpD15;if(_tmpD21->tag != 4)goto _LL768;else{_tmpD22=(_tmpD21->f1).field_info;_tmpD23=(_tmpD21->f1).targs;}}_LL767: {
# 5268
union Cyc_Absyn_DatatypeFieldInfoU _tmpD2B=_tmpD22;struct Cyc_Absyn_Datatypedecl*_tmpD2C;struct Cyc_Absyn_Datatypefield*_tmpD2D;_LL770: if((_tmpD2B.UnknownDatatypefield).tag != 1)goto _LL772;_LL771:
# 5271
 return 0;_LL772: if((_tmpD2B.KnownDatatypefield).tag != 2)goto _LL76F;_tmpD2C=((struct _tuple3)(_tmpD2B.KnownDatatypefield).val).f1;_tmpD2D=((struct _tuple3)(_tmpD2B.KnownDatatypefield).val).f2;_LL773: {
# 5273
struct Cyc_List_List*_tmpD2E=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpD2C->tvs,_tmpD23);
struct Cyc_List_List*_tmpD2F=_tmpD2D->typs;
while(_tmpD2F != 0){
_tmpD14=_tmpD2E == 0?(*((struct _tuple12*)_tmpD2F->hd)).f2: Cyc_Tcutil_rsubstitute(rgn,_tmpD2E,(*((struct _tuple12*)_tmpD2F->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,_tmpD14))return 1;
_tmpD2F=_tmpD2F->tl;}
# 5280
return 0;}_LL76F:;}_LL768:;_LL769:
# 5282
 return 0;_LL75B:;};}
# 5286
static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,struct _dyneither_ptr*f){
void*_tmpD30=Cyc_Tcutil_compress(t);void*_tmpD31=_tmpD30;struct Cyc_Absyn_Aggrdecl*_tmpD33;struct Cyc_List_List*_tmpD34;struct Cyc_List_List*_tmpD36;_LL775: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD32=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpD31;if(_tmpD32->tag != 11)goto _LL777;else{if((((_tmpD32->f1).aggr_info).KnownAggr).tag != 2)goto _LL777;_tmpD33=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpD32->f1).aggr_info).KnownAggr).val);_tmpD34=(_tmpD32->f1).targs;}}_LL776:
# 5289
 _tmpD36=_tmpD33->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD33->impl))->fields;goto _LL778;_LL777: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpD35=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpD31;if(_tmpD35->tag != 12)goto _LL779;else{_tmpD36=_tmpD35->f2;}}_LL778: {
# 5291
struct Cyc_Absyn_Aggrfield*_tmpD37=Cyc_Absyn_lookup_field(_tmpD36,f);
{struct Cyc_Absyn_Aggrfield*_tmpD38=_tmpD37;void*_tmpD39;_LL77C: if(_tmpD38 != 0)goto _LL77E;_LL77D: {
const char*_tmp141B;void*_tmp141A;(_tmp141A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp141B="is_noalias_field: missing field",_tag_dyneither(_tmp141B,sizeof(char),32))),_tag_dyneither(_tmp141A,sizeof(void*),0)));}_LL77E: if(_tmpD38 == 0)goto _LL77B;_tmpD39=_tmpD38->type;_LL77F:
# 5295
 return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpD39);_LL77B:;}
# 5297
return 0;}_LL779:;_LL77A: {
# 5299
const char*_tmp141F;void*_tmp141E[1];struct Cyc_String_pa_PrintArg_struct _tmp141D;(_tmp141D.tag=0,((_tmp141D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp141E[0]=& _tmp141D,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp141F="is_noalias_field: invalid type |%s|",_tag_dyneither(_tmp141F,sizeof(char),36))),_tag_dyneither(_tmp141E,sizeof(void*),1)))))));}_LL774:;}
# 5307
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
void*_tmpD3F=e->r;void*_tmpD40=_tmpD3F;struct Cyc_Absyn_Exp*_tmpD44;struct Cyc_Absyn_Exp*_tmpD46;struct Cyc_Absyn_Exp*_tmpD48;struct _dyneither_ptr*_tmpD49;struct Cyc_Absyn_Exp*_tmpD4B;struct Cyc_Absyn_Exp*_tmpD4C;struct Cyc_Absyn_Exp*_tmpD4E;struct Cyc_Absyn_Exp*_tmpD4F;struct Cyc_Absyn_Exp*_tmpD51;struct Cyc_Absyn_Exp*_tmpD53;struct Cyc_Absyn_Stmt*_tmpD55;_LL781: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpD41=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD40;if(_tmpD41->tag != 1)goto _LL783;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpD42=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmpD41->f2);if(_tmpD42->tag != 1)goto _LL783;}}_LL782:
 return 0;_LL783: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpD43=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpD40;if(_tmpD43->tag != 21)goto _LL785;else{_tmpD44=_tmpD43->f1;}}_LL784:
 _tmpD46=_tmpD44;goto _LL786;_LL785: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpD45=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpD40;if(_tmpD45->tag != 19)goto _LL787;else{_tmpD46=_tmpD45->f1;}}_LL786:
# 5312
 return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmpD46->topt),1) && 
Cyc_Tcutil_is_noalias_path(r,_tmpD46);_LL787: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpD47=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpD40;if(_tmpD47->tag != 20)goto _LL789;else{_tmpD48=_tmpD47->f1;_tmpD49=_tmpD47->f2;}}_LL788:
 return Cyc_Tcutil_is_noalias_path(r,_tmpD48);_LL789: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpD4A=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpD40;if(_tmpD4A->tag != 22)goto _LL78B;else{_tmpD4B=_tmpD4A->f1;_tmpD4C=_tmpD4A->f2;}}_LL78A: {
# 5316
void*_tmpD56=Cyc_Tcutil_compress((void*)_check_null(_tmpD4B->topt));void*_tmpD57=_tmpD56;_LL796: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpD58=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpD57;if(_tmpD58->tag != 10)goto _LL798;}_LL797:
 return Cyc_Tcutil_is_noalias_path(r,_tmpD4B);_LL798:;_LL799:
 return 0;_LL795:;}_LL78B: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpD4D=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD40;if(_tmpD4D->tag != 5)goto _LL78D;else{_tmpD4E=_tmpD4D->f2;_tmpD4F=_tmpD4D->f3;}}_LL78C:
# 5321
 return Cyc_Tcutil_is_noalias_path(r,_tmpD4E) && Cyc_Tcutil_is_noalias_path(r,_tmpD4F);_LL78D: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpD50=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpD40;if(_tmpD50->tag != 8)goto _LL78F;else{_tmpD51=_tmpD50->f2;}}_LL78E:
 _tmpD53=_tmpD51;goto _LL790;_LL78F: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpD52=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD40;if(_tmpD52->tag != 13)goto _LL791;else{_tmpD53=_tmpD52->f2;}}_LL790:
 return Cyc_Tcutil_is_noalias_path(r,_tmpD53);_LL791: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmpD54=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpD40;if(_tmpD54->tag != 36)goto _LL793;else{_tmpD55=_tmpD54->f1;}}_LL792:
# 5325
 while(1){
void*_tmpD59=_tmpD55->r;void*_tmpD5A=_tmpD59;struct Cyc_Absyn_Stmt*_tmpD5C;struct Cyc_Absyn_Stmt*_tmpD5D;struct Cyc_Absyn_Decl*_tmpD5F;struct Cyc_Absyn_Stmt*_tmpD60;struct Cyc_Absyn_Exp*_tmpD62;_LL79B: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpD5B=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpD5A;if(_tmpD5B->tag != 2)goto _LL79D;else{_tmpD5C=_tmpD5B->f1;_tmpD5D=_tmpD5B->f2;}}_LL79C:
 _tmpD55=_tmpD5D;goto _LL79A;_LL79D: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpD5E=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpD5A;if(_tmpD5E->tag != 12)goto _LL79F;else{_tmpD5F=_tmpD5E->f1;_tmpD60=_tmpD5E->f2;}}_LL79E:
 _tmpD55=_tmpD60;goto _LL79A;_LL79F: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpD61=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpD5A;if(_tmpD61->tag != 1)goto _LL7A1;else{_tmpD62=_tmpD61->f1;}}_LL7A0:
 return Cyc_Tcutil_is_noalias_path(r,_tmpD62);_LL7A1:;_LL7A2: {
const char*_tmp1422;void*_tmp1421;(_tmp1421=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1422="is_noalias_stmt_exp: ill-formed StmtExp",_tag_dyneither(_tmp1422,sizeof(char),40))),_tag_dyneither(_tmp1421,sizeof(void*),0)));}_LL79A:;}_LL793:;_LL794:
# 5333
 return 1;_LL780:;}
# 5350 "tcutil.cyc"
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5352
struct _tuple18 _tmp1423;struct _tuple18 bogus_ans=(_tmp1423.f1=0,((_tmp1423.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp1423)));
void*_tmpD65=e->r;void*_tmpD66=_tmpD65;struct _tuple2*_tmpD68;void*_tmpD69;struct Cyc_Absyn_Exp*_tmpD6B;struct _dyneither_ptr*_tmpD6C;int _tmpD6D;struct Cyc_Absyn_Exp*_tmpD6F;struct _dyneither_ptr*_tmpD70;int _tmpD71;struct Cyc_Absyn_Exp*_tmpD73;struct Cyc_Absyn_Exp*_tmpD75;struct Cyc_Absyn_Exp*_tmpD76;_LL7A4: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpD67=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD66;if(_tmpD67->tag != 1)goto _LL7A6;else{_tmpD68=_tmpD67->f1;_tmpD69=(void*)_tmpD67->f2;}}_LL7A5: {
# 5356
void*_tmpD77=_tmpD69;struct Cyc_Absyn_Vardecl*_tmpD7B;struct Cyc_Absyn_Vardecl*_tmpD7D;struct Cyc_Absyn_Vardecl*_tmpD7F;struct Cyc_Absyn_Vardecl*_tmpD81;_LL7B1: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpD78=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpD77;if(_tmpD78->tag != 0)goto _LL7B3;}_LL7B2:
 goto _LL7B4;_LL7B3: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpD79=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpD77;if(_tmpD79->tag != 2)goto _LL7B5;}_LL7B4:
 return bogus_ans;_LL7B5: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpD7A=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpD77;if(_tmpD7A->tag != 1)goto _LL7B7;else{_tmpD7B=_tmpD7A->f1;}}_LL7B6: {
# 5360
void*_tmpD82=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpD83=_tmpD82;_LL7BE: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD84=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD83;if(_tmpD84->tag != 8)goto _LL7C0;}_LL7BF: {
# 5362
struct _tuple18 _tmp1424;return(_tmp1424.f1=1,((_tmp1424.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp1424)));}_LL7C0:;_LL7C1: {
struct _tuple18 _tmp1425;return(_tmp1425.f1=(_tmpD7B->tq).real_const,((_tmp1425.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp1425)));}_LL7BD:;}_LL7B7: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD7C=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpD77;if(_tmpD7C->tag != 4)goto _LL7B9;else{_tmpD7D=_tmpD7C->f1;}}_LL7B8: {
# 5366
void*_tmpD87=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpD88=_tmpD87;_LL7C3: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD89=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD88;if(_tmpD89->tag != 8)goto _LL7C5;}_LL7C4: {
struct _tuple18 _tmp1426;return(_tmp1426.f1=1,((_tmp1426.f2=(void*)_check_null(_tmpD7D->rgn),_tmp1426)));}_LL7C5:;_LL7C6:
# 5369
 _tmpD7D->escapes=1;{
struct _tuple18 _tmp1427;return(_tmp1427.f1=(_tmpD7D->tq).real_const,((_tmp1427.f2=(void*)_check_null(_tmpD7D->rgn),_tmp1427)));};_LL7C2:;}_LL7B9: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpD7E=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpD77;if(_tmpD7E->tag != 5)goto _LL7BB;else{_tmpD7F=_tmpD7E->f1;}}_LL7BA:
# 5372
 _tmpD81=_tmpD7F;goto _LL7BC;_LL7BB: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpD80=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpD77;if(_tmpD80->tag != 3)goto _LL7B0;else{_tmpD81=_tmpD80->f1;}}_LL7BC:
# 5374
 _tmpD81->escapes=1;{
struct _tuple18 _tmp1428;return(_tmp1428.f1=(_tmpD81->tq).real_const,((_tmp1428.f2=(void*)_check_null(_tmpD81->rgn),_tmp1428)));};_LL7B0:;}_LL7A6: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpD6A=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpD66;if(_tmpD6A->tag != 20)goto _LL7A8;else{_tmpD6B=_tmpD6A->f1;_tmpD6C=_tmpD6A->f2;_tmpD6D=_tmpD6A->f3;}}_LL7A7:
# 5379
 if(_tmpD6D)return bogus_ans;{
# 5382
void*_tmpD8D=Cyc_Tcutil_compress((void*)_check_null(_tmpD6B->topt));void*_tmpD8E=_tmpD8D;struct Cyc_List_List*_tmpD90;struct Cyc_Absyn_Aggrdecl*_tmpD92;_LL7C8: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpD8F=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpD8E;if(_tmpD8F->tag != 12)goto _LL7CA;else{_tmpD90=_tmpD8F->f2;}}_LL7C9: {
# 5384
struct Cyc_Absyn_Aggrfield*_tmpD93=Cyc_Absyn_lookup_field(_tmpD90,_tmpD6C);
if(_tmpD93 != 0  && _tmpD93->width == 0){
struct _tuple18 _tmpD94=Cyc_Tcutil_addressof_props(te,_tmpD6B);int _tmpD96;void*_tmpD97;struct _tuple18 _tmpD95=_tmpD94;_tmpD96=_tmpD95.f1;_tmpD97=_tmpD95.f2;{
struct _tuple18 _tmp1429;return(_tmp1429.f1=(_tmpD93->tq).real_const  || _tmpD96,((_tmp1429.f2=_tmpD97,_tmp1429)));};}
# 5389
return bogus_ans;}_LL7CA: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD91=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpD8E;if(_tmpD91->tag != 11)goto _LL7CC;else{if((((_tmpD91->f1).aggr_info).KnownAggr).tag != 2)goto _LL7CC;_tmpD92=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpD91->f1).aggr_info).KnownAggr).val);}}_LL7CB: {
# 5391
struct Cyc_Absyn_Aggrfield*_tmpD99=Cyc_Absyn_lookup_decl_field(_tmpD92,_tmpD6C);
if(_tmpD99 != 0  && _tmpD99->width == 0){
struct _tuple18 _tmpD9A=Cyc_Tcutil_addressof_props(te,_tmpD6B);int _tmpD9C;void*_tmpD9D;struct _tuple18 _tmpD9B=_tmpD9A;_tmpD9C=_tmpD9B.f1;_tmpD9D=_tmpD9B.f2;{
struct _tuple18 _tmp142A;return(_tmp142A.f1=(_tmpD99->tq).real_const  || _tmpD9C,((_tmp142A.f2=_tmpD9D,_tmp142A)));};}
# 5396
return bogus_ans;}_LL7CC:;_LL7CD:
 return bogus_ans;_LL7C7:;};_LL7A8: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpD6E=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpD66;if(_tmpD6E->tag != 21)goto _LL7AA;else{_tmpD6F=_tmpD6E->f1;_tmpD70=_tmpD6E->f2;_tmpD71=_tmpD6E->f3;}}_LL7A9:
# 5401
 if(_tmpD71)return bogus_ans;{
# 5405
void*_tmpD9F=Cyc_Tcutil_compress((void*)_check_null(_tmpD6F->topt));void*_tmpDA0=_tmpD9F;void*_tmpDA2;void*_tmpDA3;_LL7CF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpDA1=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpDA0;if(_tmpDA1->tag != 5)goto _LL7D1;else{_tmpDA2=(_tmpDA1->f1).elt_typ;_tmpDA3=((_tmpDA1->f1).ptr_atts).rgn;}}_LL7D0: {
# 5407
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmpDA4=Cyc_Tcutil_compress(_tmpDA2);void*_tmpDA5=_tmpDA4;struct Cyc_List_List*_tmpDA7;struct Cyc_Absyn_Aggrdecl*_tmpDA9;_LL7D4: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpDA6=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpDA5;if(_tmpDA6->tag != 12)goto _LL7D6;else{_tmpDA7=_tmpDA6->f2;}}_LL7D5:
# 5410
 finfo=Cyc_Absyn_lookup_field(_tmpDA7,_tmpD70);goto _LL7D3;_LL7D6: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpDA8=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpDA5;if(_tmpDA8->tag != 11)goto _LL7D8;else{if((((_tmpDA8->f1).aggr_info).KnownAggr).tag != 2)goto _LL7D8;_tmpDA9=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpDA8->f1).aggr_info).KnownAggr).val);}}_LL7D7:
# 5412
 finfo=Cyc_Absyn_lookup_decl_field(_tmpDA9,_tmpD70);goto _LL7D3;_LL7D8:;_LL7D9:
 return bogus_ans;_LL7D3:;}
# 5415
if(finfo != 0  && finfo->width == 0){
struct _tuple18 _tmp142B;return(_tmp142B.f1=(finfo->tq).real_const,((_tmp142B.f2=_tmpDA3,_tmp142B)));}
return bogus_ans;}_LL7D1:;_LL7D2:
 return bogus_ans;_LL7CE:;};_LL7AA: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpD72=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpD66;if(_tmpD72->tag != 19)goto _LL7AC;else{_tmpD73=_tmpD72->f1;}}_LL7AB: {
# 5422
void*_tmpDAB=Cyc_Tcutil_compress((void*)_check_null(_tmpD73->topt));void*_tmpDAC=_tmpDAB;struct Cyc_Absyn_Tqual _tmpDAE;void*_tmpDAF;_LL7DB: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpDAD=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpDAC;if(_tmpDAD->tag != 5)goto _LL7DD;else{_tmpDAE=(_tmpDAD->f1).elt_tq;_tmpDAF=((_tmpDAD->f1).ptr_atts).rgn;}}_LL7DC: {
# 5424
struct _tuple18 _tmp142C;return(_tmp142C.f1=_tmpDAE.real_const,((_tmp142C.f2=_tmpDAF,_tmp142C)));}_LL7DD:;_LL7DE:
 return bogus_ans;_LL7DA:;}_LL7AC: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpD74=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpD66;if(_tmpD74->tag != 22)goto _LL7AE;else{_tmpD75=_tmpD74->f1;_tmpD76=_tmpD74->f2;}}_LL7AD: {
# 5430
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmpD75->topt));
void*_tmpDB1=t;struct Cyc_List_List*_tmpDB3;struct Cyc_Absyn_Tqual _tmpDB5;void*_tmpDB6;struct Cyc_Absyn_Tqual _tmpDB8;_LL7E0: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpDB2=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpDB1;if(_tmpDB2->tag != 10)goto _LL7E2;else{_tmpDB3=_tmpDB2->f1;}}_LL7E1: {
# 5434
struct _tuple14 _tmpDB9=Cyc_Evexp_eval_const_uint_exp(_tmpD76);unsigned int _tmpDBB;int _tmpDBC;struct _tuple14 _tmpDBA=_tmpDB9;_tmpDBB=_tmpDBA.f1;_tmpDBC=_tmpDBA.f2;
if(!_tmpDBC)
return bogus_ans;{
struct _tuple12*_tmpDBD=Cyc_Absyn_lookup_tuple_field(_tmpDB3,(int)_tmpDBB);
if(_tmpDBD != 0){
struct _tuple18 _tmp142D;return(_tmp142D.f1=((*_tmpDBD).f1).real_const,((_tmp142D.f2=(Cyc_Tcutil_addressof_props(te,_tmpD75)).f2,_tmp142D)));}
return bogus_ans;};}_LL7E2: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpDB4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpDB1;if(_tmpDB4->tag != 5)goto _LL7E4;else{_tmpDB5=(_tmpDB4->f1).elt_tq;_tmpDB6=((_tmpDB4->f1).ptr_atts).rgn;}}_LL7E3: {
# 5442
struct _tuple18 _tmp142E;return(_tmp142E.f1=_tmpDB5.real_const,((_tmp142E.f2=_tmpDB6,_tmp142E)));}_LL7E4: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpDB7=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpDB1;if(_tmpDB7->tag != 8)goto _LL7E6;else{_tmpDB8=(_tmpDB7->f1).tq;}}_LL7E5: {
# 5448
struct _tuple18 _tmp142F;return(_tmp142F.f1=_tmpDB8.real_const,((_tmp142F.f2=(Cyc_Tcutil_addressof_props(te,_tmpD75)).f2,_tmp142F)));}_LL7E6:;_LL7E7:
 return bogus_ans;_LL7DF:;}_LL7AE:;_LL7AF:
# 5452
{const char*_tmp1432;void*_tmp1431;(_tmp1431=0,Cyc_Tcutil_terr(e->loc,((_tmp1432="unary & applied to non-lvalue",_tag_dyneither(_tmp1432,sizeof(char),30))),_tag_dyneither(_tmp1431,sizeof(void*),0)));}
return bogus_ans;_LL7A3:;}
# 5459
void Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,union Cyc_Absyn_Constraint*b){
b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(b);{
void*_tmpDC4=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);void*_tmpDC5=_tmpDC4;struct Cyc_Absyn_Exp*_tmpDC8;_LL7E9: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpDC6=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpDC5;if(_tmpDC6->tag != 0)goto _LL7EB;}_LL7EA:
 return;_LL7EB: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpDC7=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpDC5;if(_tmpDC7->tag != 1)goto _LL7E8;else{_tmpDC8=_tmpDC7->f1;}}_LL7EC: {
# 5464
struct _tuple14 _tmpDC9=Cyc_Evexp_eval_const_uint_exp(_tmpDC8);unsigned int _tmpDCB;int _tmpDCC;struct _tuple14 _tmpDCA=_tmpDC9;_tmpDCB=_tmpDCA.f1;_tmpDCC=_tmpDCA.f2;
if(_tmpDCC  && _tmpDCB <= i){
const char*_tmp1437;void*_tmp1436[2];struct Cyc_Int_pa_PrintArg_struct _tmp1435;struct Cyc_Int_pa_PrintArg_struct _tmp1434;(_tmp1434.tag=1,((_tmp1434.f1=(unsigned long)((int)i),((_tmp1435.tag=1,((_tmp1435.f1=(unsigned long)((int)_tmpDCB),((_tmp1436[0]=& _tmp1435,((_tmp1436[1]=& _tmp1434,Cyc_Tcutil_terr(loc,((_tmp1437="dereference is out of bounds: %d <= %d",_tag_dyneither(_tmp1437,sizeof(char),39))),_tag_dyneither(_tmp1436,sizeof(void*),2)))))))))))));}
return;}_LL7E8:;};}
# 5471
void Cyc_Tcutil_check_nonzero_bound(unsigned int loc,union Cyc_Absyn_Constraint*b){
Cyc_Tcutil_check_bound(loc,0,b);}
# 5475
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b){
void*_tmpDD1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);void*_tmpDD2=_tmpDD1;struct Cyc_Absyn_Exp*_tmpDD4;_LL7EE: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpDD3=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpDD2;if(_tmpDD3->tag != 1)goto _LL7F0;else{_tmpDD4=_tmpDD3->f1;}}_LL7EF: {
# 5478
struct _tuple14 _tmpDD5=Cyc_Evexp_eval_const_uint_exp(_tmpDD4);unsigned int _tmpDD7;int _tmpDD8;struct _tuple14 _tmpDD6=_tmpDD5;_tmpDD7=_tmpDD6.f1;_tmpDD8=_tmpDD6.f2;
return _tmpDD8  && _tmpDD7 == 1;}_LL7F0:;_LL7F1:
 return 0;_LL7ED:;}
# 5484
int Cyc_Tcutil_bits_only(void*t){
void*_tmpDD9=Cyc_Tcutil_compress(t);void*_tmpDDA=_tmpDD9;void*_tmpDE1;union Cyc_Absyn_Constraint*_tmpDE2;struct Cyc_List_List*_tmpDE4;struct Cyc_Absyn_Aggrdecl*_tmpDE7;struct Cyc_List_List*_tmpDE8;struct Cyc_List_List*_tmpDEA;_LL7F3: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpDDB=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpDDA;if(_tmpDDB->tag != 0)goto _LL7F5;}_LL7F4:
 goto _LL7F6;_LL7F5: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpDDC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpDDA;if(_tmpDDC->tag != 6)goto _LL7F7;}_LL7F6:
 goto _LL7F8;_LL7F7: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpDDD=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpDDA;if(_tmpDDD->tag != 7)goto _LL7F9;}_LL7F8:
 return 1;_LL7F9: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpDDE=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpDDA;if(_tmpDDE->tag != 13)goto _LL7FB;}_LL7FA:
 goto _LL7FC;_LL7FB: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpDDF=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpDDA;if(_tmpDDF->tag != 14)goto _LL7FD;}_LL7FC:
 return 0;_LL7FD: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpDE0=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpDDA;if(_tmpDE0->tag != 8)goto _LL7FF;else{_tmpDE1=(_tmpDE0->f1).elt_type;_tmpDE2=(_tmpDE0->f1).zero_term;}}_LL7FE:
# 5494
 return !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpDE2) && Cyc_Tcutil_bits_only(_tmpDE1);_LL7FF: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpDE3=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpDDA;if(_tmpDE3->tag != 10)goto _LL801;else{_tmpDE4=_tmpDE3->f1;}}_LL800:
# 5496
 for(0;_tmpDE4 != 0;_tmpDE4=_tmpDE4->tl){
if(!Cyc_Tcutil_bits_only((*((struct _tuple12*)_tmpDE4->hd)).f2))return 0;}
return 1;_LL801: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpDE5=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpDDA;if(_tmpDE5->tag != 11)goto _LL803;else{if((((_tmpDE5->f1).aggr_info).UnknownAggr).tag != 1)goto _LL803;}}_LL802:
 return 0;_LL803: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpDE6=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpDDA;if(_tmpDE6->tag != 11)goto _LL805;else{if((((_tmpDE6->f1).aggr_info).KnownAggr).tag != 2)goto _LL805;_tmpDE7=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpDE6->f1).aggr_info).KnownAggr).val);_tmpDE8=(_tmpDE6->f1).targs;}}_LL804:
# 5501
 if(_tmpDE7->impl == 0)
return 0;{
int okay=1;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpDE7->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
if(okay)return 1;{
struct _RegionHandle _tmpDEB=_new_region("rgn");struct _RegionHandle*rgn=& _tmpDEB;_push_region(rgn);
{struct Cyc_List_List*_tmpDEC=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpDE7->tvs,_tmpDE8);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpDE7->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpDEC,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpDED=0;_npop_handler(0);return _tmpDED;}}}{
int _tmpDEE=1;_npop_handler(0);return _tmpDEE;};}
# 5508
;_pop_region(rgn);};};_LL805: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpDE9=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpDDA;if(_tmpDE9->tag != 12)goto _LL807;else{_tmpDEA=_tmpDE9->f2;}}_LL806:
# 5514
 for(0;_tmpDEA != 0;_tmpDEA=_tmpDEA->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)_tmpDEA->hd)->type))return 0;}
return 1;_LL807:;_LL808:
 return 0;_LL7F2:;}
# 5522
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e){
while(1){
void*_tmpDEF=e->r;void*_tmpDF0=_tmpDEF;struct Cyc_Absyn_Exp*_tmpDF3;struct Cyc_Absyn_Exp*_tmpDF5;_LL80A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpDF1=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpDF0;if(_tmpDF1->tag != 1)goto _LL80C;}_LL80B:
 return 1;_LL80C: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpDF2=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpDF0;if(_tmpDF2->tag != 11)goto _LL80E;else{_tmpDF3=_tmpDF2->f1;}}_LL80D:
 _tmpDF5=_tmpDF3;goto _LL80F;_LL80E: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpDF4=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpDF0;if(_tmpDF4->tag != 12)goto _LL810;else{_tmpDF5=_tmpDF4->f1;}}_LL80F:
 e=_tmpDF5;continue;_LL810:;_LL811:
# 5529
 return 0;_LL809:;}}
# 5539
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmpDF6=e->r;void*_tmpDF7=_tmpDF6;struct _tuple2*_tmpDFF;void*_tmpE00;struct Cyc_Absyn_Exp*_tmpE02;struct Cyc_Absyn_Exp*_tmpE03;struct Cyc_Absyn_Exp*_tmpE04;struct Cyc_Absyn_Exp*_tmpE06;struct Cyc_Absyn_Exp*_tmpE07;struct Cyc_Absyn_Exp*_tmpE09;struct Cyc_Absyn_Exp*_tmpE0B;void*_tmpE0D;struct Cyc_Absyn_Exp*_tmpE0E;void*_tmpE10;struct Cyc_Absyn_Exp*_tmpE11;struct Cyc_Absyn_Exp*_tmpE13;struct Cyc_Absyn_Exp*_tmpE15;struct Cyc_Absyn_Exp*_tmpE16;struct Cyc_Absyn_Exp*_tmpE18;struct Cyc_List_List*_tmpE1A;struct Cyc_List_List*_tmpE1C;struct Cyc_List_List*_tmpE1E;enum Cyc_Absyn_Primop _tmpE20;struct Cyc_List_List*_tmpE21;struct Cyc_List_List*_tmpE23;struct Cyc_List_List*_tmpE25;_LL813: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpDF8=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDF7;if(_tmpDF8->tag != 0)goto _LL815;}_LL814:
 goto _LL816;_LL815: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpDF9=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpDF7;if(_tmpDF9->tag != 16)goto _LL817;}_LL816:
 goto _LL818;_LL817: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpDFA=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpDF7;if(_tmpDFA->tag != 17)goto _LL819;}_LL818:
 goto _LL81A;_LL819: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpDFB=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpDF7;if(_tmpDFB->tag != 18)goto _LL81B;}_LL81A:
 goto _LL81C;_LL81B: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpDFC=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpDF7;if(_tmpDFC->tag != 31)goto _LL81D;}_LL81C:
 goto _LL81E;_LL81D: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpDFD=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpDF7;if(_tmpDFD->tag != 32)goto _LL81F;}_LL81E:
 return 1;_LL81F: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpDFE=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpDF7;if(_tmpDFE->tag != 1)goto _LL821;else{_tmpDFF=_tmpDFE->f1;_tmpE00=(void*)_tmpDFE->f2;}}_LL820: {
# 5550
void*_tmpE26=_tmpE00;struct Cyc_Absyn_Vardecl*_tmpE29;struct Cyc_Absyn_Vardecl*_tmpE2B;_LL842: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpE27=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpE26;if(_tmpE27->tag != 2)goto _LL844;}_LL843:
 return 1;_LL844: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpE28=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpE26;if(_tmpE28->tag != 1)goto _LL846;else{_tmpE29=_tmpE28->f1;}}_LL845: {
# 5553
void*_tmpE2D=Cyc_Tcutil_compress(_tmpE29->type);void*_tmpE2E=_tmpE2D;_LL84D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpE2F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE2E;if(_tmpE2F->tag != 8)goto _LL84F;}_LL84E:
 goto _LL850;_LL84F: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE30=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE2E;if(_tmpE30->tag != 9)goto _LL851;}_LL850:
 return 1;_LL851:;_LL852:
 return var_okay;_LL84C:;}_LL846: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpE2A=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpE26;if(_tmpE2A->tag != 4)goto _LL848;else{_tmpE2B=_tmpE2A->f1;}}_LL847:
# 5560
 if(_tmpE2B->sc == Cyc_Absyn_Static){
void*_tmpE31=Cyc_Tcutil_compress(_tmpE2B->type);void*_tmpE32=_tmpE31;_LL854: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpE33=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE32;if(_tmpE33->tag != 8)goto _LL856;}_LL855:
 goto _LL857;_LL856: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE34=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE32;if(_tmpE34->tag != 9)goto _LL858;}_LL857:
 return 1;_LL858:;_LL859:
 return var_okay;_LL853:;}else{
# 5567
return var_okay;}_LL848: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpE2C=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpE26;if(_tmpE2C->tag != 0)goto _LL84A;}_LL849:
 return 0;_LL84A:;_LL84B:
 return var_okay;_LL841:;}_LL821: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpE01=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDF7;if(_tmpE01->tag != 5)goto _LL823;else{_tmpE02=_tmpE01->f1;_tmpE03=_tmpE01->f2;_tmpE04=_tmpE01->f3;}}_LL822:
# 5573
 return(Cyc_Tcutil_cnst_exp(0,_tmpE02) && 
Cyc_Tcutil_cnst_exp(0,_tmpE03)) && 
Cyc_Tcutil_cnst_exp(0,_tmpE04);_LL823: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpE05=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpDF7;if(_tmpE05->tag != 8)goto _LL825;else{_tmpE06=_tmpE05->f1;_tmpE07=_tmpE05->f2;}}_LL824:
# 5577
 return Cyc_Tcutil_cnst_exp(0,_tmpE06) && Cyc_Tcutil_cnst_exp(0,_tmpE07);_LL825: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpE08=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpDF7;if(_tmpE08->tag != 11)goto _LL827;else{_tmpE09=_tmpE08->f1;}}_LL826:
 _tmpE0B=_tmpE09;goto _LL828;_LL827: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpE0A=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpDF7;if(_tmpE0A->tag != 12)goto _LL829;else{_tmpE0B=_tmpE0A->f1;}}_LL828:
# 5580
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpE0B);_LL829: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpE0C=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDF7;if(_tmpE0C->tag != 13)goto _LL82B;else{_tmpE0D=(void*)_tmpE0C->f1;_tmpE0E=_tmpE0C->f2;if(_tmpE0C->f4 != Cyc_Absyn_No_coercion)goto _LL82B;}}_LL82A:
# 5582
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpE0E);_LL82B: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpE0F=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDF7;if(_tmpE0F->tag != 13)goto _LL82D;else{_tmpE10=(void*)_tmpE0F->f1;_tmpE11=_tmpE0F->f2;}}_LL82C:
# 5585
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpE11);_LL82D: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmpE12=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpDF7;if(_tmpE12->tag != 14)goto _LL82F;else{_tmpE13=_tmpE12->f1;}}_LL82E:
# 5587
 return Cyc_Tcutil_cnst_exp(1,_tmpE13);_LL82F: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpE14=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpDF7;if(_tmpE14->tag != 26)goto _LL831;else{_tmpE15=_tmpE14->f2;_tmpE16=_tmpE14->f3;}}_LL830:
# 5589
 return Cyc_Tcutil_cnst_exp(0,_tmpE15) && Cyc_Tcutil_cnst_exp(0,_tmpE16);_LL831: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpE17=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpDF7;if(_tmpE17->tag != 27)goto _LL833;else{_tmpE18=_tmpE17->f1;}}_LL832:
# 5591
 return Cyc_Tcutil_cnst_exp(0,_tmpE18);_LL833: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpE19=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpDF7;if(_tmpE19->tag != 25)goto _LL835;else{_tmpE1A=_tmpE19->f1;}}_LL834:
 _tmpE1C=_tmpE1A;goto _LL836;_LL835: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpE1B=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpDF7;if(_tmpE1B->tag != 29)goto _LL837;else{_tmpE1C=_tmpE1B->f2;}}_LL836:
 _tmpE1E=_tmpE1C;goto _LL838;_LL837: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpE1D=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDF7;if(_tmpE1D->tag != 28)goto _LL839;else{_tmpE1E=_tmpE1D->f3;}}_LL838:
# 5595
 for(0;_tmpE1E != 0;_tmpE1E=_tmpE1E->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple19*)_tmpE1E->hd)).f2))
return 0;}
return 1;_LL839: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpE1F=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpDF7;if(_tmpE1F->tag != 2)goto _LL83B;else{_tmpE20=_tmpE1F->f1;_tmpE21=_tmpE1F->f2;}}_LL83A:
# 5600
 _tmpE23=_tmpE21;goto _LL83C;_LL83B: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmpE22=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpDF7;if(_tmpE22->tag != 23)goto _LL83D;else{_tmpE23=_tmpE22->f1;}}_LL83C:
 _tmpE25=_tmpE23;goto _LL83E;_LL83D: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpE24=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpDF7;if(_tmpE24->tag != 30)goto _LL83F;else{_tmpE25=_tmpE24->f1;}}_LL83E:
# 5603
 for(0;_tmpE25 != 0;_tmpE25=_tmpE25->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmpE25->hd))
return 0;}
return 1;_LL83F:;_LL840:
 return 0;_LL812:;}
# 5611
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 5615
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);
# 5617
int Cyc_Tcutil_supports_default(void*t){
void*_tmpE35=Cyc_Tcutil_compress(t);void*_tmpE36=_tmpE35;union Cyc_Absyn_Constraint*_tmpE3B;union Cyc_Absyn_Constraint*_tmpE3C;void*_tmpE3E;struct Cyc_List_List*_tmpE40;union Cyc_Absyn_AggrInfoU _tmpE42;struct Cyc_List_List*_tmpE43;struct Cyc_List_List*_tmpE45;_LL85B: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpE37=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpE36;if(_tmpE37->tag != 0)goto _LL85D;}_LL85C:
 goto _LL85E;_LL85D: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpE38=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpE36;if(_tmpE38->tag != 6)goto _LL85F;}_LL85E:
 goto _LL860;_LL85F: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpE39=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpE36;if(_tmpE39->tag != 7)goto _LL861;}_LL860:
 return 1;_LL861: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE3A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE36;if(_tmpE3A->tag != 5)goto _LL863;else{_tmpE3B=((_tmpE3A->f1).ptr_atts).nullable;_tmpE3C=((_tmpE3A->f1).ptr_atts).bounds;}}_LL862: {
# 5624
void*_tmpE48=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpE3C);void*_tmpE49=_tmpE48;_LL872: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpE4A=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpE49;if(_tmpE4A->tag != 0)goto _LL874;}_LL873:
 return 1;_LL874:;_LL875:
# 5627
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmpE3B);_LL871:;}_LL863: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpE3D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE36;if(_tmpE3D->tag != 8)goto _LL865;else{_tmpE3E=(_tmpE3D->f1).elt_type;}}_LL864:
# 5630
 return Cyc_Tcutil_supports_default(_tmpE3E);_LL865: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpE3F=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpE36;if(_tmpE3F->tag != 10)goto _LL867;else{_tmpE40=_tmpE3F->f1;}}_LL866:
# 5632
 for(0;_tmpE40 != 0;_tmpE40=_tmpE40->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple12*)_tmpE40->hd)).f2))return 0;}
return 1;_LL867: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpE41=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpE36;if(_tmpE41->tag != 11)goto _LL869;else{_tmpE42=(_tmpE41->f1).aggr_info;_tmpE43=(_tmpE41->f1).targs;}}_LL868: {
# 5636
struct Cyc_Absyn_Aggrdecl*_tmpE4B=Cyc_Absyn_get_known_aggrdecl(_tmpE42);
if(_tmpE4B->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpE4B->impl))->exist_vars != 0)return 0;
return Cyc_Tcutil_fields_support_default(_tmpE4B->tvs,_tmpE43,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpE4B->impl))->fields);}_LL869: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpE44=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpE36;if(_tmpE44->tag != 12)goto _LL86B;else{_tmpE45=_tmpE44->f2;}}_LL86A:
 return Cyc_Tcutil_fields_support_default(0,0,_tmpE45);_LL86B: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpE46=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpE36;if(_tmpE46->tag != 14)goto _LL86D;}_LL86C:
# 5642
 goto _LL86E;_LL86D: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpE47=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpE36;if(_tmpE47->tag != 13)goto _LL86F;}_LL86E:
 return 1;_LL86F:;_LL870:
# 5645
 return 0;_LL85A:;}
# 5650
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t){
void*_tmpE4C=t;struct Cyc_Absyn_Typedefdecl*_tmpE4E;_LL877: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpE4D=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpE4C;if(_tmpE4D->tag != 17)goto _LL879;else{_tmpE4E=_tmpE4D->f3;}}_LL878:
# 5653
 if(_tmpE4E != 0){
struct Cyc_Absyn_Tqual _tmpE4F=_tmpE4E->tq;
if(((_tmpE4F.print_const  || _tmpE4F.q_volatile) || _tmpE4F.q_restrict) || _tmpE4F.real_const){
# 5658
const char*_tmp143B;void*_tmp143A[1];struct Cyc_String_pa_PrintArg_struct _tmp1439;(_tmp1439.tag=0,((_tmp1439.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp143A[0]=& _tmp1439,Cyc_Tcutil_warn(loc,((_tmp143B="qualifier within typedef type %s is ignored",_tag_dyneither(_tmp143B,sizeof(char),44))),_tag_dyneither(_tmp143A,sizeof(void*),1)))))));}}
# 5661
goto _LL876;_LL879:;_LL87A:
 goto _LL876;_LL876:;}
# 5666
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 5668
struct _RegionHandle _tmpE53=_new_region("rgn");struct _RegionHandle*rgn=& _tmpE53;_push_region(rgn);{
struct Cyc_List_List*_tmpE54=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_supports_default(t)){int _tmpE55=1;_npop_handler(0);return _tmpE55;}
t=Cyc_Tcutil_rsubstitute(rgn,_tmpE54,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpE56=0;_npop_handler(0);return _tmpE56;}}}{
# 5677
int _tmpE57=1;_npop_handler(0);return _tmpE57;};
# 5668
;_pop_region(rgn);}
# 5683
int Cyc_Tcutil_admits_zero(void*t){
void*_tmpE58=Cyc_Tcutil_compress(t);void*_tmpE59=_tmpE58;union Cyc_Absyn_Constraint*_tmpE5D;union Cyc_Absyn_Constraint*_tmpE5E;_LL87C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpE5A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpE59;if(_tmpE5A->tag != 6)goto _LL87E;}_LL87D:
 goto _LL87F;_LL87E: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpE5B=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpE59;if(_tmpE5B->tag != 7)goto _LL880;}_LL87F:
 return 1;_LL880: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE5C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE59;if(_tmpE5C->tag != 5)goto _LL882;else{_tmpE5D=((_tmpE5C->f1).ptr_atts).nullable;_tmpE5E=((_tmpE5C->f1).ptr_atts).bounds;}}_LL881: {
# 5688
void*_tmpE5F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpE5E);void*_tmpE60=_tmpE5F;_LL885: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpE61=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpE60;if(_tmpE61->tag != 0)goto _LL887;}_LL886:
# 5692
 return 0;_LL887:;_LL888:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpE5D);_LL884:;}_LL882:;_LL883:
# 5695
 return 0;_LL87B:;}
# 5699
int Cyc_Tcutil_is_noreturn(void*t){
{void*_tmpE62=Cyc_Tcutil_compress(t);void*_tmpE63=_tmpE62;void*_tmpE65;struct Cyc_List_List*_tmpE67;_LL88A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE64=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE63;if(_tmpE64->tag != 5)goto _LL88C;else{_tmpE65=(_tmpE64->f1).elt_typ;}}_LL88B:
 return Cyc_Tcutil_is_noreturn(_tmpE65);_LL88C: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE66=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE63;if(_tmpE66->tag != 9)goto _LL88E;else{_tmpE67=(_tmpE66->f1).attributes;}}_LL88D:
# 5703
 for(0;_tmpE67 != 0;_tmpE67=_tmpE67->tl){
void*_tmpE68=(void*)_tmpE67->hd;void*_tmpE69=_tmpE68;_LL891: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmpE6A=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmpE69;if(_tmpE6A->tag != 4)goto _LL893;}_LL892:
 return 1;_LL893:;_LL894:
 continue;_LL890:;}
# 5708
goto _LL889;_LL88E:;_LL88F:
 goto _LL889;_LL889:;}
# 5711
return 0;}
# 5716
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmpE6B=Cyc_Tcutil_compress(t);void*_tmpE6C=_tmpE6B;struct Cyc_List_List**_tmpE6E;_LL896: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE6D=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE6C;if(_tmpE6D->tag != 9)goto _LL898;else{_tmpE6E=(struct Cyc_List_List**)&(_tmpE6D->f1).attributes;}}_LL897: {
# 5719
struct Cyc_List_List*_tmpE6F=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Tcutil_attribute_cmp,*_tmpE6E,(void*)atts->hd)){
struct Cyc_List_List*_tmp143C;*_tmpE6E=((_tmp143C=_cycalloc(sizeof(*_tmp143C)),((_tmp143C->hd=(void*)atts->hd,((_tmp143C->tl=*_tmpE6E,_tmp143C))))));}}else{
# 5726
struct Cyc_List_List*_tmp143D;_tmpE6F=((_tmp143D=_cycalloc(sizeof(*_tmp143D)),((_tmp143D->hd=(void*)atts->hd,((_tmp143D->tl=_tmpE6F,_tmp143D))))));}}
return _tmpE6F;}_LL898:;_LL899: {
const char*_tmp1440;void*_tmp143F;(_tmp143F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1440="transfer_fn_type_atts",_tag_dyneither(_tmp1440,sizeof(char),22))),_tag_dyneither(_tmp143F,sizeof(void*),0)));}_LL895:;}
# 5733
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
struct Cyc_Absyn_Exp*bound=0;
{void*_tmpE74=Cyc_Tcutil_compress(t);void*_tmpE75=_tmpE74;struct Cyc_Absyn_PtrInfo*_tmpE77;struct Cyc_Absyn_Exp*_tmpE79;_LL89B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE76=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE75;if(_tmpE76->tag != 5)goto _LL89D;else{_tmpE77=(struct Cyc_Absyn_PtrInfo*)& _tmpE76->f1;}}_LL89C:
# 5737
{void*_tmpE7A=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)((_tmpE77->ptr_atts).bounds);void*_tmpE7B=_tmpE7A;struct Cyc_Absyn_Exp*_tmpE7D;_LL8A2: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpE7C=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpE7B;if(_tmpE7C->tag != 1)goto _LL8A4;else{_tmpE7D=_tmpE7C->f1;}}_LL8A3:
 bound=_tmpE7D;goto _LL8A1;_LL8A4:;_LL8A5:
 goto _LL8A1;_LL8A1:;}
# 5741
goto _LL89A;_LL89D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpE78=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE75;if(_tmpE78->tag != 8)goto _LL89F;else{_tmpE79=(_tmpE78->f1).num_elts;}}_LL89E:
# 5743
 bound=_tmpE79;
goto _LL89A;_LL89F:;_LL8A0:
 goto _LL89A;_LL89A:;}
# 5747
return bound;}
# 5752
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
{void*_tmpE7E=b;struct Cyc_Absyn_Vardecl*_tmpE80;struct Cyc_Absyn_Vardecl*_tmpE82;struct Cyc_Absyn_Vardecl*_tmpE84;struct Cyc_Absyn_Vardecl*_tmpE86;_LL8A7: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpE7F=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpE7E;if(_tmpE7F->tag != 5)goto _LL8A9;else{_tmpE80=_tmpE7F->f1;}}_LL8A8:
 _tmpE82=_tmpE80;goto _LL8AA;_LL8A9: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpE81=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpE7E;if(_tmpE81->tag != 4)goto _LL8AB;else{_tmpE82=_tmpE81->f1;}}_LL8AA:
 _tmpE84=_tmpE82;goto _LL8AC;_LL8AB: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpE83=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpE7E;if(_tmpE83->tag != 3)goto _LL8AD;else{_tmpE84=_tmpE83->f1;}}_LL8AC:
 _tmpE86=_tmpE84;goto _LL8AE;_LL8AD: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpE85=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpE7E;if(_tmpE85->tag != 1)goto _LL8AF;else{_tmpE86=_tmpE85->f1;}}_LL8AE:
# 5758
 if(!_tmpE86->escapes)return _tmpE86;
goto _LL8A6;_LL8AF:;_LL8B0:
 goto _LL8A6;_LL8A6:;}
# 5762
return 0;}
# 5766
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmpE87=0;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0){struct Cyc_List_List*_tmp1441;_tmpE87=((_tmp1441=_cycalloc(sizeof(*_tmp1441)),((_tmp1441->hd=*((void**)_check_null((void**)x->hd)),((_tmp1441->tl=_tmpE87,_tmp1441))))));}}}
return _tmpE87;}
# 5773
int Cyc_Tcutil_is_array(void*t){
# 5775
void*_tmpE89=Cyc_Tcutil_compress(t);void*_tmpE8A=_tmpE89;_LL8B2: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpE8B=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE8A;if(_tmpE8B->tag != 8)goto _LL8B4;}_LL8B3:
# 5777
 return 1;_LL8B4:;_LL8B5:
 return 0;_LL8B1:;}
# 5782
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
# 5784
void*_tmpE8C=Cyc_Tcutil_compress(t);void*_tmpE8D=_tmpE8C;void*_tmpE8F;struct Cyc_Absyn_Tqual _tmpE90;struct Cyc_Absyn_Exp*_tmpE91;union Cyc_Absyn_Constraint*_tmpE92;unsigned int _tmpE93;_LL8B7: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpE8E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE8D;if(_tmpE8E->tag != 8)goto _LL8B9;else{_tmpE8F=(_tmpE8E->f1).elt_type;_tmpE90=(_tmpE8E->f1).tq;_tmpE91=(_tmpE8E->f1).num_elts;_tmpE92=(_tmpE8E->f1).zero_term;_tmpE93=(_tmpE8E->f1).zt_loc;}}_LL8B8: {
# 5786
void*b;
if(_tmpE91 == 0)
b=(void*)& Cyc_Absyn_DynEither_b_val;else{
# 5790
struct Cyc_Absyn_Exp*bnd=_tmpE91;
if(convert_tag){
if(bnd->topt == 0){
const char*_tmp1444;void*_tmp1443;(_tmp1443=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1444="cannot convert tag without type!",_tag_dyneither(_tmp1444,sizeof(char),33))),_tag_dyneither(_tmp1443,sizeof(void*),0)));}{
void*_tmpE96=Cyc_Tcutil_compress((void*)_check_null(bnd->topt));void*_tmpE97=_tmpE96;void*_tmpE99;_LL8BC: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE98=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpE97;if(_tmpE98->tag != 19)goto _LL8BE;else{_tmpE99=(void*)_tmpE98->f1;}}_LL8BD:
# 5796
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp1447;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp1446;b=(void*)((_tmp1446=_cycalloc(sizeof(*_tmp1446)),((_tmp1446[0]=((_tmp1447.tag=1,((_tmp1447.f1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmpE99,0),0,Cyc_Absyn_No_coercion,0),_tmp1447)))),_tmp1446))));}
goto _LL8BB;_LL8BE:;_LL8BF:
# 5799
 if(Cyc_Tcutil_is_const_exp(bnd)){
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp144A;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp1449;b=(void*)((_tmp1449=_cycalloc(sizeof(*_tmp1449)),((_tmp1449[0]=((_tmp144A.tag=1,((_tmp144A.f1=bnd,_tmp144A)))),_tmp1449))));}else{
# 5802
b=(void*)& Cyc_Absyn_DynEither_b_val;}_LL8BB:;};}else{
# 5806
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp144D;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp144C;b=(void*)((_tmp144C=_cycalloc(sizeof(*_tmp144C)),((_tmp144C[0]=((_tmp144D.tag=1,((_tmp144D.f1=bnd,_tmp144D)))),_tmp144C))));}}
# 5808
return Cyc_Absyn_atb_typ(_tmpE8F,rgn,_tmpE90,b,_tmpE92);}_LL8B9:;_LL8BA:
# 5811
 return t;_LL8B6:;}
