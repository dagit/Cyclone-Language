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
# 224 "cycboot.h"
int Cyc_vfprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;
# 177
void Cyc_Core_ufree(struct _dyneither_ptr ptr);
# 190
struct _dyneither_ptr Cyc_Core_alias_refptr(struct _dyneither_ptr ptr);struct Cyc_Core_DynamicRegion;
# 205
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_region_key_t;
# 211
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_uregion_key_t;
# 216
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_rcregion_key_t;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 237
struct Cyc_Core_NewDynamicRegion Cyc_Core_new_rckey();
# 245
void Cyc_Core_free_rckey(struct Cyc_Core_DynamicRegion*k);
# 251
void*Cyc_Core_open_region(struct Cyc_Core_DynamicRegion*key,void*arg,void*(*body)(struct _RegionHandle*h,void*arg));
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
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 133
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 175
struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*,struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 251
int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x);
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple0 Cyc_List_split(struct Cyc_List_List*x);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 71
struct _dyneither_ptr Cyc_strcpy(struct _dyneither_ptr dest,struct _dyneither_ptr src);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;
# 54 "set.h"
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
# 69
struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 68
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));
# 72
struct Cyc_Dict_Dict Cyc_Dict_rshare(struct _RegionHandle*,struct Cyc_Dict_Dict);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 113
void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k);
# 122 "dict.h"
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;
# 41
struct _dyneither_ptr Cyc_Position_string_of_segment(unsigned int);struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};struct Cyc_Xarray_Xarray{struct _dyneither_ptr elmts;int num_elmts;};
# 38 "xarray.h"
typedef struct Cyc_Xarray_Xarray*Cyc_Xarray_xarray_t;
# 40
int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);
# 42
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);
# 57
struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate_empty(struct _RegionHandle*);
# 69
int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);
# 121
void Cyc_Xarray_reuse(struct Cyc_Xarray_Xarray*xarr);struct Cyc_Relations_Reln;
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 105
typedef struct _tuple1*Cyc_Absyn_qvar_t;typedef struct _tuple1*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_opt_t;
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
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 365
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 385
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 390
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 444 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
# 447
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 460
typedef void*Cyc_Absyn_funcparams_t;
# 463
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 527
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 543
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 545
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign,long long);
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr,int);
# 553
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 560
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 578
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 707 "absyn.h"
extern struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val;struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 742
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
# 770
extern struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 929
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 936
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 946
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 949
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
# 956
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 959
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 974
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 976
extern void*Cyc_Absyn_sint_typ;
# 988
void*Cyc_Absyn_exn_typ();
# 999
extern void*Cyc_Absyn_bounds_one;
# 1007
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1013
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);
# 1015
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1018
void*Cyc_Absyn_void_star_typ();
# 1020
void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple1*name);
void*Cyc_Absyn_unionq_typ(struct _tuple1*name);
# 1026
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1030
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1032
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1038
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);
# 1043
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
# 1046
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple1*,void*,unsigned int);
# 1050
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1053
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1059
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1062
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1065
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1070
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1074
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1076
struct Cyc_Absyn_Exp*Cyc_Absyn_rethrow_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1079
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
# 1084
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1089
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1095
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned int);
# 1101
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1111
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned int loc);
# 1114
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned int loc);
# 1118
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1120
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc);
# 1125
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1129
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1134
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
# 1177
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1180
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1190
struct _dyneither_ptr*Cyc_Absyn_fieldname(int);
# 1194
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1210
extern int Cyc_Absyn_no_regions;struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 65 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 70
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_RgnOrder_RgnPO;
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
# 139 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 64
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 68
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 71
void*Cyc_Tcutil_compress(void*t);
# 81
int Cyc_Tcutil_is_arithmetic_type(void*);
# 102
int Cyc_Tcutil_is_pointer_type(void*t);
# 104
int Cyc_Tcutil_is_boxed(void*t);
int Cyc_Tcutil_is_dyneither_ptr(void*t);
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 111
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 119
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
# 161
int Cyc_Tcutil_unify(void*,void*);
# 166
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 256 "tcutil.h"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 262
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 264
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 266
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 276
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;};
# 334
void*Cyc_Tcutil_snd_tqt(struct _tuple11*);
# 363
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 367
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 370
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 373
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);struct _tuple12{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple12 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 40 "relations.h"
typedef union Cyc_Relations_RelnOp Cyc_Relations_reln_op_t;
# 42
union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*);
# 51
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0,Cyc_Relations_Rneq  = 1,Cyc_Relations_Rlte  = 2,Cyc_Relations_Rlt  = 3};
typedef enum Cyc_Relations_Relation Cyc_Relations_relation_t;struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 77
int Cyc_Relations_exp2relnop(struct Cyc_Absyn_Exp*e,union Cyc_Relations_RelnOp*p);
# 88
struct Cyc_List_List*Cyc_Relations_add_relation(struct _RegionHandle*rgn,union Cyc_Relations_RelnOp rop1,enum Cyc_Relations_Relation r,union Cyc_Relations_RelnOp rop2,struct Cyc_List_List*relns);
# 122
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);
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
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple13{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple13 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 144 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 163
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 235 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 32 "toc.h"
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);
# 35
struct _tuple1*Cyc_Toc_temp_var();
extern struct _dyneither_ptr Cyc_Toc_globals;
# 66 "toc.cyc"
extern int Cyc_noexpand_r;
# 77 "toc.cyc"
int Cyc_Toc_warn_bounds_checks=0;
int Cyc_Toc_warn_all_null_deref=0;
unsigned int Cyc_Toc_total_bounds_checks=0;
unsigned int Cyc_Toc_bounds_checks_eliminated=0;
# 86
static struct Cyc_List_List*Cyc_Toc_result_decls=0;struct Cyc_Toc_TocState{struct Cyc_List_List**tuple_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_List_List**abs_struct_types;struct Cyc_Set_Set**datatypes_so_far;struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*temp_labels;};struct _tuple14{struct _tuple1*f1;struct _tuple1*f2;};
# 106
int Cyc_Toc_qvar_tag_cmp(struct _tuple14*x,struct _tuple14*y){
struct _tuple14 _tmp0=*x;struct _tuple1*_tmp2;struct _tuple1*_tmp3;struct _tuple14 _tmp1=_tmp0;_tmp2=_tmp1.f1;_tmp3=_tmp1.f2;{
struct _tuple14 _tmp4=*y;struct _tuple1*_tmp6;struct _tuple1*_tmp7;struct _tuple14 _tmp5=_tmp4;_tmp6=_tmp5.f1;_tmp7=_tmp5.f2;{
int i=Cyc_Absyn_qvar_cmp(_tmp2,_tmp6);
if(i != 0)return i;
return Cyc_Absyn_qvar_cmp(_tmp3,_tmp7);};};}
# 115
static struct Cyc_Toc_TocState*Cyc_Toc_empty_toc_state(struct _RegionHandle*d,int unused){
struct Cyc_Dict_Dict*_tmpA5C;struct Cyc_Dict_Dict*_tmpA5B;struct Cyc_Set_Set**_tmpA5A;struct Cyc_List_List**_tmpA59;struct Cyc_Dict_Dict*_tmpA58;struct Cyc_List_List**_tmpA57;struct Cyc_Toc_TocState*_tmpA56;return(_tmpA56=_region_malloc(d,sizeof(*_tmpA56)),((_tmpA56->tuple_types=(
(_tmpA57=_region_malloc(d,sizeof(*_tmpA57)),((_tmpA57[0]=0,_tmpA57)))),((_tmpA56->aggrs_so_far=(
(_tmpA58=_region_malloc(d,sizeof(*_tmpA58)),((_tmpA58[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpA58)))),((_tmpA56->abs_struct_types=(
(_tmpA59=_region_malloc(d,sizeof(*_tmpA59)),((_tmpA59[0]=0,_tmpA59)))),((_tmpA56->datatypes_so_far=(
(_tmpA5A=_region_malloc(d,sizeof(*_tmpA5A)),((_tmpA5A[0]=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpA5A)))),((_tmpA56->xdatatypes_so_far=(
(_tmpA5B=_region_malloc(d,sizeof(*_tmpA5B)),((_tmpA5B[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpA5B)))),((_tmpA56->qvar_tags=(
(_tmpA5C=_region_malloc(d,sizeof(*_tmpA5C)),((_tmpA5C[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),_tmpA5C)))),((_tmpA56->temp_labels=
((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d),_tmpA56)))))))))))))));}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 132
typedef struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state_t;
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple15{struct Cyc_Toc_TocState*f1;void*f2;};
# 139
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple15*)){
# 142
struct Cyc_Toc_TocStateWrap*ts=0;
_swap_word(& ts,& Cyc_Toc_toc_state);{
struct Cyc_Toc_TocStateWrap _tmpF=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Core_DynamicRegion*_tmp11;struct Cyc_Toc_TocState*_tmp12;struct Cyc_Toc_TocStateWrap _tmp10=_tmpF;_tmp11=_tmp10.dyn;_tmp12=_tmp10.state;{
struct _dyneither_ptr _tmp13=((struct _dyneither_ptr(*)(struct _dyneither_ptr ptr))Cyc_Core_alias_refptr)(_tag_dyneither(_tmp11,sizeof(struct Cyc_Core_DynamicRegion),1));
{struct Cyc_Toc_TocStateWrap _tmpA5D;*ts=((_tmpA5D.dyn=(struct Cyc_Core_DynamicRegion*)_tmp11,((_tmpA5D.state=(struct Cyc_Toc_TocState*)_tmp12,_tmpA5D))));}
_swap_word(& ts,& Cyc_Toc_toc_state);{
struct _tuple15 _tmpA5E;struct _tuple15 _tmp15=(_tmpA5E.f1=_tmp12,((_tmpA5E.f2=arg,_tmpA5E)));
void*_tmp16=((void*(*)(struct Cyc_Core_DynamicRegion*key,struct _tuple15*arg,void*(*body)(struct _RegionHandle*h,struct _tuple15*arg)))Cyc_Core_open_region)((struct Cyc_Core_DynamicRegion*)_untag_dyneither_ptr(_tmp13,sizeof(struct Cyc_Core_DynamicRegion),1),& _tmp15,f);
Cyc_Core_free_rckey((struct Cyc_Core_DynamicRegion*)_untag_dyneither_ptr(_tmp13,sizeof(struct Cyc_Core_DynamicRegion),1));
return _tmp16;};};};}struct _tuple16{struct _tuple1*f1;void*(*f2)(struct _tuple1*);};struct _tuple17{struct Cyc_Toc_TocState*f1;struct _tuple16*f2;};struct _tuple18{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
# 154
static void*Cyc_Toc_aggrdecl_type_body(struct _RegionHandle*d,struct _tuple17*env){
# 157
struct _tuple17 _tmp18=*env;struct Cyc_Toc_TocState*_tmp1A;struct _tuple1*_tmp1B;void*(*_tmp1C)(struct _tuple1*);struct _tuple17 _tmp19=_tmp18;_tmp1A=_tmp19.f1;_tmp1B=(_tmp19.f2)->f1;_tmp1C=(_tmp19.f2)->f2;{
struct _tuple18**v=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp1A->aggrs_so_far,_tmp1B);
if(v == 0)
return _tmp1C(_tmp1B);else{
# 162
struct _tuple18*_tmp1D=*v;void*_tmp1F;struct _tuple18*_tmp1E=_tmp1D;_tmp1F=_tmp1E->f2;
return _tmp1F;}};}
# 167
void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){
struct _tuple16 _tmpA5F;struct _tuple16 env=(_tmpA5F.f1=q,((_tmpA5F.f2=type_maker,_tmpA5F)));
return((void*(*)(struct _tuple16*arg,void*(*f)(struct _RegionHandle*,struct _tuple17*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_aggrdecl_type_body);}
# 172
static int Cyc_Toc_tuple_type_counter=0;
static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;char Cyc_Toc_Toc_Unimplemented[18]="Toc_Unimplemented";struct Cyc_Toc_Toc_Unimplemented_exn_struct{char*tag;};char Cyc_Toc_Toc_Impossible[15]="Toc_Impossible";struct Cyc_Toc_Toc_Impossible_exn_struct{char*tag;};
# 179
static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 182
Cyc_vfprintf(Cyc_stderr,fmt,ap);
{const char*_tmpA62;void*_tmpA61;(_tmpA61=0,Cyc_fprintf(Cyc_stderr,((_tmpA62="\n",_tag_dyneither(_tmpA62,sizeof(char),2))),_tag_dyneither(_tmpA61,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Unimplemented_exn_struct _tmpA65;struct Cyc_Toc_Toc_Unimplemented_exn_struct*_tmpA64;(int)_throw((void*)((_tmpA64=_cycalloc_atomic(sizeof(*_tmpA64)),((_tmpA64[0]=((_tmpA65.tag=Cyc_Toc_Toc_Unimplemented,_tmpA65)),_tmpA64)))));};}
# 187
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 190
Cyc_vfprintf(Cyc_stderr,fmt,ap);
{const char*_tmpA68;void*_tmpA67;(_tmpA67=0,Cyc_fprintf(Cyc_stderr,((_tmpA68="\n",_tag_dyneither(_tmpA68,sizeof(char),2))),_tag_dyneither(_tmpA67,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Impossible_exn_struct _tmpA6B;struct Cyc_Toc_Toc_Impossible_exn_struct*_tmpA6A;(int)_throw((void*)((_tmpA6A=_cycalloc_atomic(sizeof(*_tmpA6A)),((_tmpA6A[0]=((_tmpA6B.tag=Cyc_Toc_Toc_Impossible,_tmpA6B)),_tmpA6A)))));};}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_exn_struct{char*tag;};static char _tmp29[5]="curr";
# 205 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc_curr_string={_tmp29,_tmp29,_tmp29 + 5};static struct _dyneither_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp2A[4]="tag";
static struct _dyneither_ptr Cyc_Toc_tag_string={_tmp2A,_tmp2A,_tmp2A + 4};static struct _dyneither_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp2B[4]="val";
static struct _dyneither_ptr Cyc_Toc_val_string={_tmp2B,_tmp2B,_tmp2B + 4};static struct _dyneither_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp2C[14]="_handler_cons";
static struct _dyneither_ptr Cyc_Toc__handler_cons_string={_tmp2C,_tmp2C,_tmp2C + 14};static struct _dyneither_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;static char _tmp2D[8]="handler";
static struct _dyneither_ptr Cyc_Toc_handler_string={_tmp2D,_tmp2D,_tmp2D + 8};static struct _dyneither_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp2E[14]="_RegionHandle";
static struct _dyneither_ptr Cyc_Toc__RegionHandle_string={_tmp2E,_tmp2E,_tmp2E + 14};static struct _dyneither_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;static char _tmp2F[17]="_DynRegionHandle";
static struct _dyneither_ptr Cyc_Toc__DynRegionHandle_string={_tmp2F,_tmp2F,_tmp2F + 17};static struct _dyneither_ptr*Cyc_Toc__DynRegionHandle_sp=& Cyc_Toc__DynRegionHandle_string;static char _tmp30[16]="_DynRegionFrame";
static struct _dyneither_ptr Cyc_Toc__DynRegionFrame_string={_tmp30,_tmp30,_tmp30 + 16};static struct _dyneither_ptr*Cyc_Toc__DynRegionFrame_sp=& Cyc_Toc__DynRegionFrame_string;
# 218
struct _dyneither_ptr Cyc_Toc_globals={(void*)0,(void*)0,(void*)(0 + 0)};static char _tmp31[7]="_throw";
# 227 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc__throw_str={_tmp31,_tmp31,_tmp31 + 7};static struct _tuple1 Cyc_Toc__throw_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_re={1,& Cyc_Toc__throw_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)& Cyc_Toc__throw_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp33[7]="setjmp";
static struct _dyneither_ptr Cyc_Toc_setjmp_str={_tmp33,_tmp33,_tmp33 + 7};static struct _tuple1 Cyc_Toc_setjmp_pr={{.Loc_n={4,0}},& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc_setjmp_re={1,& Cyc_Toc_setjmp_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)& Cyc_Toc_setjmp_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmp35[14]="_push_handler";
static struct _dyneither_ptr Cyc_Toc__push_handler_str={_tmp35,_tmp35,_tmp35 + 14};static struct _tuple1 Cyc_Toc__push_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_handler_re={1,& Cyc_Toc__push_handler_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)& Cyc_Toc__push_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static char _tmp37[13]="_pop_handler";
static struct _dyneither_ptr Cyc_Toc__pop_handler_str={_tmp37,_tmp37,_tmp37 + 13};static struct _tuple1 Cyc_Toc__pop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_handler_re={1,& Cyc_Toc__pop_handler_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={0,(void*)& Cyc_Toc__pop_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static char _tmp39[12]="_exn_thrown";
static struct _dyneither_ptr Cyc_Toc__exn_thrown_str={_tmp39,_tmp39,_tmp39 + 12};static struct _tuple1 Cyc_Toc__exn_thrown_pr={{.Loc_n={4,0}},& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__exn_thrown_re={1,& Cyc_Toc__exn_thrown_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)& Cyc_Toc__exn_thrown_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;static char _tmp3B[14]="_npop_handler";
static struct _dyneither_ptr Cyc_Toc__npop_handler_str={_tmp3B,_tmp3B,_tmp3B + 14};static struct _tuple1 Cyc_Toc__npop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__npop_handler_re={1,& Cyc_Toc__npop_handler_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)& Cyc_Toc__npop_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;static char _tmp3D[12]="_check_null";
static struct _dyneither_ptr Cyc_Toc__check_null_str={_tmp3D,_tmp3D,_tmp3D + 12};static struct _tuple1 Cyc_Toc__check_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_null_re={1,& Cyc_Toc__check_null_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)& Cyc_Toc__check_null_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;static char _tmp3F[28]="_check_known_subscript_null";
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_null_str={_tmp3F,_tmp3F,_tmp3F + 28};static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_null_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_null_re={1,& Cyc_Toc__check_known_subscript_null_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={0,(void*)& Cyc_Toc__check_known_subscript_null_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;static char _tmp41[31]="_check_known_subscript_notnull";
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_notnull_str={_tmp41,_tmp41,_tmp41 + 31};static struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_notnull_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_notnull_re={1,& Cyc_Toc__check_known_subscript_notnull_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)& Cyc_Toc__check_known_subscript_notnull_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp43[27]="_check_dyneither_subscript";
static struct _dyneither_ptr Cyc_Toc__check_dyneither_subscript_str={_tmp43,_tmp43,_tmp43 + 27};static struct _tuple1 Cyc_Toc__check_dyneither_subscript_pr={{.Loc_n={4,0}},& Cyc_Toc__check_dyneither_subscript_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_dyneither_subscript_re={1,& Cyc_Toc__check_dyneither_subscript_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={0,(void*)& Cyc_Toc__check_dyneither_subscript_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;static char _tmp45[15]="_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_str={_tmp45,_tmp45,_tmp45 + 15};static struct _tuple1 Cyc_Toc__dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_re={1,& Cyc_Toc__dyneither_ptr_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={0,(void*)& Cyc_Toc__dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=& Cyc_Toc__dyneither_ptr_ev;static char _tmp47[15]="_tag_dyneither";
static struct _dyneither_ptr Cyc_Toc__tag_dyneither_str={_tmp47,_tmp47,_tmp47 + 15};static struct _tuple1 Cyc_Toc__tag_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__tag_dyneither_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__tag_dyneither_re={1,& Cyc_Toc__tag_dyneither_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={0,(void*)& Cyc_Toc__tag_dyneither_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;static char _tmp49[20]="_init_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__init_dyneither_ptr_str={_tmp49,_tmp49,_tmp49 + 20};static struct _tuple1 Cyc_Toc__init_dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__init_dyneither_ptr_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__init_dyneither_ptr_re={1,& Cyc_Toc__init_dyneither_ptr_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__init_dyneither_ptr_ev={0,(void*)& Cyc_Toc__init_dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__init_dyneither_ptr_e=& Cyc_Toc__init_dyneither_ptr_ev;static char _tmp4B[21]="_untag_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__untag_dyneither_ptr_str={_tmp4B,_tmp4B,_tmp4B + 21};static struct _tuple1 Cyc_Toc__untag_dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__untag_dyneither_ptr_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__untag_dyneither_ptr_re={1,& Cyc_Toc__untag_dyneither_ptr_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={0,(void*)& Cyc_Toc__untag_dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;static char _tmp4D[20]="_get_dyneither_size";
static struct _dyneither_ptr Cyc_Toc__get_dyneither_size_str={_tmp4D,_tmp4D,_tmp4D + 20};static struct _tuple1 Cyc_Toc__get_dyneither_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_dyneither_size_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_dyneither_size_re={1,& Cyc_Toc__get_dyneither_size_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_dyneither_size_ev={0,(void*)& Cyc_Toc__get_dyneither_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=& Cyc_Toc__get_dyneither_size_ev;static char _tmp4F[19]="_get_zero_arr_size";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_str={_tmp4F,_tmp4F,_tmp4F + 19};static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_re={1,& Cyc_Toc__get_zero_arr_size_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;static char _tmp51[24]="_get_zero_arr_size_char";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_char_str={_tmp51,_tmp51,_tmp51 + 24};static struct _tuple1 Cyc_Toc__get_zero_arr_size_char_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_char_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_char_re={1,& Cyc_Toc__get_zero_arr_size_char_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;static char _tmp53[25]="_get_zero_arr_size_short";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_short_str={_tmp53,_tmp53,_tmp53 + 25};static struct _tuple1 Cyc_Toc__get_zero_arr_size_short_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_short_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_short_re={1,& Cyc_Toc__get_zero_arr_size_short_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;static char _tmp55[23]="_get_zero_arr_size_int";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_int_str={_tmp55,_tmp55,_tmp55 + 23};static struct _tuple1 Cyc_Toc__get_zero_arr_size_int_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_int_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_int_re={1,& Cyc_Toc__get_zero_arr_size_int_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_int_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=& Cyc_Toc__get_zero_arr_size_int_ev;static char _tmp57[25]="_get_zero_arr_size_float";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_float_str={_tmp57,_tmp57,_tmp57 + 25};static struct _tuple1 Cyc_Toc__get_zero_arr_size_float_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_float_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_float_re={1,& Cyc_Toc__get_zero_arr_size_float_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_float_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=& Cyc_Toc__get_zero_arr_size_float_ev;static char _tmp59[26]="_get_zero_arr_size_double";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_double_str={_tmp59,_tmp59,_tmp59 + 26};static struct _tuple1 Cyc_Toc__get_zero_arr_size_double_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_double_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_double_re={1,& Cyc_Toc__get_zero_arr_size_double_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;static char _tmp5B[30]="_get_zero_arr_size_longdouble";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_longdouble_str={_tmp5B,_tmp5B,_tmp5B + 30};static struct _tuple1 Cyc_Toc__get_zero_arr_size_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_longdouble_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_longdouble_re={1,& Cyc_Toc__get_zero_arr_size_longdouble_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=& Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _tmp5D[28]="_get_zero_arr_size_voidstar";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_tmp5D,_tmp5D,_tmp5D + 28};static struct _tuple1 Cyc_Toc__get_zero_arr_size_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_voidstar_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_voidstar_re={1,& Cyc_Toc__get_zero_arr_size_voidstar_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;static char _tmp5F[20]="_dyneither_ptr_plus";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_plus_str={_tmp5F,_tmp5F,_tmp5F + 20};static struct _tuple1 Cyc_Toc__dyneither_ptr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_plus_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_plus_re={1,& Cyc_Toc__dyneither_ptr_plus_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=& Cyc_Toc__dyneither_ptr_plus_ev;static char _tmp61[15]="_zero_arr_plus";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_str={_tmp61,_tmp61,_tmp61 + 15};static struct _tuple1 Cyc_Toc__zero_arr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_re={1,& Cyc_Toc__zero_arr_plus_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)& Cyc_Toc__zero_arr_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static char _tmp63[20]="_zero_arr_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_char_str={_tmp63,_tmp63,_tmp63 + 20};static struct _tuple1 Cyc_Toc__zero_arr_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_char_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_char_re={1,& Cyc_Toc__zero_arr_plus_char_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_plus_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=& Cyc_Toc__zero_arr_plus_char_ev;static char _tmp65[21]="_zero_arr_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_short_str={_tmp65,_tmp65,_tmp65 + 21};static struct _tuple1 Cyc_Toc__zero_arr_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_short_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_short_re={1,& Cyc_Toc__zero_arr_plus_short_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_plus_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=& Cyc_Toc__zero_arr_plus_short_ev;static char _tmp67[19]="_zero_arr_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_int_str={_tmp67,_tmp67,_tmp67 + 19};static struct _tuple1 Cyc_Toc__zero_arr_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_int_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_int_re={1,& Cyc_Toc__zero_arr_plus_int_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_plus_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=& Cyc_Toc__zero_arr_plus_int_ev;static char _tmp69[21]="_zero_arr_plus_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_float_str={_tmp69,_tmp69,_tmp69 + 21};static struct _tuple1 Cyc_Toc__zero_arr_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_float_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_float_re={1,& Cyc_Toc__zero_arr_plus_float_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_plus_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=& Cyc_Toc__zero_arr_plus_float_ev;static char _tmp6B[22]="_zero_arr_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_double_str={_tmp6B,_tmp6B,_tmp6B + 22};static struct _tuple1 Cyc_Toc__zero_arr_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_double_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_double_re={1,& Cyc_Toc__zero_arr_plus_double_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_plus_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=& Cyc_Toc__zero_arr_plus_double_ev;static char _tmp6D[26]="_zero_arr_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_longdouble_str={_tmp6D,_tmp6D,_tmp6D + 26};static struct _tuple1 Cyc_Toc__zero_arr_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_longdouble_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_longdouble_re={1,& Cyc_Toc__zero_arr_plus_longdouble_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_plus_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;static char _tmp6F[24]="_zero_arr_plus_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_voidstar_str={_tmp6F,_tmp6F,_tmp6F + 24};static struct _tuple1 Cyc_Toc__zero_arr_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_voidstar_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_voidstar_re={1,& Cyc_Toc__zero_arr_plus_voidstar_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_plus_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;static char _tmp71[28]="_dyneither_ptr_inplace_plus";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_str={_tmp71,_tmp71,_tmp71 + 28};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;static char _tmp73[23]="_zero_arr_inplace_plus";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_str={_tmp73,_tmp73,_tmp73 + 23};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_re={1,& Cyc_Toc__zero_arr_inplace_plus_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;static char _tmp75[28]="_zero_arr_inplace_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={_tmp75,_tmp75,_tmp75 + 28};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_char_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_char_re={1,& Cyc_Toc__zero_arr_inplace_plus_char_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;static char _tmp77[29]="_zero_arr_inplace_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_short_str={_tmp77,_tmp77,_tmp77 + 29};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_short_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_short_re={1,& Cyc_Toc__zero_arr_inplace_plus_short_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=& Cyc_Toc__zero_arr_inplace_plus_short_ev;static char _tmp79[27]="_zero_arr_inplace_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={_tmp79,_tmp79,_tmp79 + 27};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_int_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_int_re={1,& Cyc_Toc__zero_arr_inplace_plus_int_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;static char _tmp7B[29]="_zero_arr_inplace_plus_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_float_str={_tmp7B,_tmp7B,_tmp7B + 29};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_float_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_float_re={1,& Cyc_Toc__zero_arr_inplace_plus_float_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=& Cyc_Toc__zero_arr_inplace_plus_float_ev;static char _tmp7D[30]="_zero_arr_inplace_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_double_str={_tmp7D,_tmp7D,_tmp7D + 30};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_double_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_double_re={1,& Cyc_Toc__zero_arr_inplace_plus_double_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=& Cyc_Toc__zero_arr_inplace_plus_double_ev;static char _tmp7F[34]="_zero_arr_inplace_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_tmp7F,_tmp7F,_tmp7F + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_re={1,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;static char _tmp81[32]="_zero_arr_inplace_plus_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_tmp81,_tmp81,_tmp81 + 32};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_voidstar_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1,& Cyc_Toc__zero_arr_inplace_plus_voidstar_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;static char _tmp83[33]="_dyneither_ptr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmp83,_tmp83,_tmp83 + 33};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_post_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=& Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;static char _tmp85[28]="_zero_arr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={_tmp85,_tmp85,_tmp85 + 28};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;static char _tmp87[33]="_zero_arr_inplace_plus_post_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_tmp87,_tmp87,_tmp87 + 33};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_char_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_char_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_char_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=& Cyc_Toc__zero_arr_inplace_plus_post_char_ev;static char _tmp89[34]="_zero_arr_inplace_plus_post_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_tmp89,_tmp89,_tmp89 + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_short_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_short_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=& Cyc_Toc__zero_arr_inplace_plus_post_short_ev;static char _tmp8B[32]="_zero_arr_inplace_plus_post_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_tmp8B,_tmp8B,_tmp8B + 32};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_int_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_int_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_int_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=& Cyc_Toc__zero_arr_inplace_plus_post_int_ev;static char _tmp8D[34]="_zero_arr_inplace_plus_post_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_tmp8D,_tmp8D,_tmp8D + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_float_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_float_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=& Cyc_Toc__zero_arr_inplace_plus_post_float_ev;static char _tmp8F[35]="_zero_arr_inplace_plus_post_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={_tmp8F,_tmp8F,_tmp8F + 35};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_double_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_double_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;static char _tmp91[39]="_zero_arr_inplace_plus_post_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_tmp91,_tmp91,_tmp91 + 39};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;static char _tmp93[37]="_zero_arr_inplace_plus_post_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_tmp93,_tmp93,_tmp93 + 37};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;static char _tmp95[10]="_cycalloc";
static struct _dyneither_ptr Cyc_Toc__cycalloc_str={_tmp95,_tmp95,_tmp95 + 10};static struct _tuple1 Cyc_Toc__cycalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_re={1,& Cyc_Toc__cycalloc_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={0,(void*)& Cyc_Toc__cycalloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char _tmp97[11]="_cyccalloc";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_str={_tmp97,_tmp97,_tmp97 + 11};static struct _tuple1 Cyc_Toc__cyccalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_re={1,& Cyc_Toc__cyccalloc_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)& Cyc_Toc__cyccalloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static char _tmp99[17]="_cycalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cycalloc_atomic_str={_tmp99,_tmp99,_tmp99 + 17};static struct _tuple1 Cyc_Toc__cycalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_atomic_re={1,& Cyc_Toc__cycalloc_atomic_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)& Cyc_Toc__cycalloc_atomic_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;static char _tmp9B[18]="_cyccalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_atomic_str={_tmp9B,_tmp9B,_tmp9B + 18};static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_atomic_re={1,& Cyc_Toc__cyccalloc_atomic_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)& Cyc_Toc__cyccalloc_atomic_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;static char _tmp9D[15]="_region_malloc";
static struct _dyneither_ptr Cyc_Toc__region_malloc_str={_tmp9D,_tmp9D,_tmp9D + 15};static struct _tuple1 Cyc_Toc__region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_malloc_re={1,& Cyc_Toc__region_malloc_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)& Cyc_Toc__region_malloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;static char _tmp9F[15]="_region_calloc";
static struct _dyneither_ptr Cyc_Toc__region_calloc_str={_tmp9F,_tmp9F,_tmp9F + 15};static struct _tuple1 Cyc_Toc__region_calloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_calloc_re={1,& Cyc_Toc__region_calloc_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)& Cyc_Toc__region_calloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static char _tmpA1[13]="_check_times";
static struct _dyneither_ptr Cyc_Toc__check_times_str={_tmpA1,_tmpA1,_tmpA1 + 13};static struct _tuple1 Cyc_Toc__check_times_pr={{.Loc_n={4,0}},& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_times_re={1,& Cyc_Toc__check_times_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)& Cyc_Toc__check_times_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=& Cyc_Toc__check_times_ev;static char _tmpA3[12]="_new_region";
static struct _dyneither_ptr Cyc_Toc__new_region_str={_tmpA3,_tmpA3,_tmpA3 + 12};static struct _tuple1 Cyc_Toc__new_region_pr={{.Loc_n={4,0}},& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__new_region_re={1,& Cyc_Toc__new_region_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)& Cyc_Toc__new_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static char _tmpA5[13]="_push_region";
static struct _dyneither_ptr Cyc_Toc__push_region_str={_tmpA5,_tmpA5,_tmpA5 + 13};static struct _tuple1 Cyc_Toc__push_region_pr={{.Loc_n={4,0}},& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_region_re={1,& Cyc_Toc__push_region_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)& Cyc_Toc__push_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;static char _tmpA7[12]="_pop_region";
static struct _dyneither_ptr Cyc_Toc__pop_region_str={_tmpA7,_tmpA7,_tmpA7 + 12};static struct _tuple1 Cyc_Toc__pop_region_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_region_re={1,& Cyc_Toc__pop_region_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)& Cyc_Toc__pop_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static char _tmpA9[16]="_open_dynregion";
static struct _dyneither_ptr Cyc_Toc__open_dynregion_str={_tmpA9,_tmpA9,_tmpA9 + 16};static struct _tuple1 Cyc_Toc__open_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__open_dynregion_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__open_dynregion_re={1,& Cyc_Toc__open_dynregion_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__open_dynregion_ev={0,(void*)& Cyc_Toc__open_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__open_dynregion_e=& Cyc_Toc__open_dynregion_ev;static char _tmpAB[16]="_push_dynregion";
static struct _dyneither_ptr Cyc_Toc__push_dynregion_str={_tmpAB,_tmpAB,_tmpAB + 16};static struct _tuple1 Cyc_Toc__push_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__push_dynregion_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_dynregion_re={1,& Cyc_Toc__push_dynregion_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__push_dynregion_ev={0,(void*)& Cyc_Toc__push_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_dynregion_e=& Cyc_Toc__push_dynregion_ev;static char _tmpAD[15]="_pop_dynregion";
static struct _dyneither_ptr Cyc_Toc__pop_dynregion_str={_tmpAD,_tmpAD,_tmpAD + 15};static struct _tuple1 Cyc_Toc__pop_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_dynregion_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_dynregion_re={1,& Cyc_Toc__pop_dynregion_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__pop_dynregion_ev={0,(void*)& Cyc_Toc__pop_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_dynregion_e=& Cyc_Toc__pop_dynregion_ev;static char _tmpAF[14]="_reset_region";
static struct _dyneither_ptr Cyc_Toc__reset_region_str={_tmpAF,_tmpAF,_tmpAF + 14};static struct _tuple1 Cyc_Toc__reset_region_pr={{.Loc_n={4,0}},& Cyc_Toc__reset_region_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__reset_region_re={1,& Cyc_Toc__reset_region_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__reset_region_ev={0,(void*)& Cyc_Toc__reset_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__reset_region_e=& Cyc_Toc__reset_region_ev;static char _tmpB1[19]="_throw_arraybounds";
static struct _dyneither_ptr Cyc_Toc__throw_arraybounds_str={_tmpB1,_tmpB1,_tmpB1 + 19};static struct _tuple1 Cyc_Toc__throw_arraybounds_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_arraybounds_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_arraybounds_re={1,& Cyc_Toc__throw_arraybounds_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(void*)& Cyc_Toc__throw_arraybounds_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;static char _tmpB3[29]="_dyneither_ptr_decrease_size";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_decrease_size_str={_tmpB3,_tmpB3,_tmpB3 + 29};static struct _tuple1 Cyc_Toc__dyneither_ptr_decrease_size_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_decrease_size_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_decrease_size_re={1,& Cyc_Toc__dyneither_ptr_decrease_size_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_decrease_size_ev={0,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=& Cyc_Toc__dyneither_ptr_decrease_size_ev;static char _tmpB5[11]="_swap_word";
static struct _dyneither_ptr Cyc_Toc__swap_word_str={_tmpB5,_tmpB5,_tmpB5 + 11};static struct _tuple1 Cyc_Toc__swap_word_pr={{.Loc_n={4,0}},& Cyc_Toc__swap_word_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__swap_word_re={1,& Cyc_Toc__swap_word_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__swap_word_ev={0,(void*)& Cyc_Toc__swap_word_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__swap_word_e=& Cyc_Toc__swap_word_ev;static char _tmpB7[16]="_swap_dyneither";
static struct _dyneither_ptr Cyc_Toc__swap_dyneither_str={_tmpB7,_tmpB7,_tmpB7 + 16};static struct _tuple1 Cyc_Toc__swap_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__swap_dyneither_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__swap_dyneither_re={1,& Cyc_Toc__swap_dyneither_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__swap_dyneither_ev={0,(void*)& Cyc_Toc__swap_dyneither_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__swap_dyneither_e=& Cyc_Toc__swap_dyneither_ev;static char _tmpB9[13]="_throw_match";
static struct _dyneither_ptr Cyc_Toc__throw_match_str={_tmpB9,_tmpB9,_tmpB9 + 13};static struct _tuple1 Cyc_Toc__throw_match_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_match_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_match_re={1,& Cyc_Toc__throw_match_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__throw_match_ev={0,(void*)& Cyc_Toc__throw_match_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_match_e=& Cyc_Toc__throw_match_ev;static char _tmpBB[9]="_rethrow";
static struct _dyneither_ptr Cyc_Toc__rethrow_str={_tmpBB,_tmpBB,_tmpBB + 9};static struct _tuple1 Cyc_Toc__rethrow_pr={{.Loc_n={4,0}},& Cyc_Toc__rethrow_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__rethrow_re={1,& Cyc_Toc__rethrow_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__rethrow_ev={0,(void*)& Cyc_Toc__rethrow_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__rethrow_e=& Cyc_Toc__rethrow_ev;
# 301
static struct Cyc_Absyn_AggrType_Absyn_Type_struct Cyc_Toc_dyneither_ptr_typ_v={11,{{.UnknownAggr={1,{Cyc_Absyn_StructA,& Cyc_Toc__dyneither_ptr_pr,0}}},0}};
# 304
static void*Cyc_Toc_dyneither_ptr_typ=(void*)& Cyc_Toc_dyneither_ptr_typ_v;
# 306
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0};
# 308
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
static struct Cyc_Absyn_Stmt**skip_stmt_opt=0;
if(skip_stmt_opt == 0){
struct Cyc_Absyn_Stmt**_tmpA6C;skip_stmt_opt=((_tmpA6C=_cycalloc(sizeof(*_tmpA6C)),((_tmpA6C[0]=Cyc_Absyn_skip_stmt(0),_tmpA6C))));}
return*skip_stmt_opt;}
# 317
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}
# 320
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmpA6F;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpA6E;return(void*)((_tmpA6E=_cycalloc(sizeof(*_tmpA6E)),((_tmpA6E[0]=((_tmpA6F.tag=13,((_tmpA6F.f1=t,((_tmpA6F.f2=e,((_tmpA6F.f3=0,((_tmpA6F.f4=Cyc_Absyn_No_coercion,_tmpA6F)))))))))),_tmpA6E))));}
# 323
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmpA72;struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpA71;return(void*)((_tmpA71=_cycalloc(sizeof(*_tmpA71)),((_tmpA71[0]=((_tmpA72.tag=19,((_tmpA72.f1=e,_tmpA72)))),_tmpA71))));}
# 326
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmpA75;struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpA74;return(void*)((_tmpA74=_cycalloc(sizeof(*_tmpA74)),((_tmpA74[0]=((_tmpA75.tag=22,((_tmpA75.f1=e1,((_tmpA75.f2=e2,_tmpA75)))))),_tmpA74))));}
# 329
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmpA78;struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmpA77;return(void*)((_tmpA77=_cycalloc(sizeof(*_tmpA77)),((_tmpA77[0]=((_tmpA78.tag=36,((_tmpA78.f1=s,_tmpA78)))),_tmpA77))));}
# 332
static void*Cyc_Toc_sizeoftyp_exp_r(void*t){
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpA7B;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpA7A;return(void*)((_tmpA7A=_cycalloc(sizeof(*_tmpA7A)),((_tmpA7A[0]=((_tmpA7B.tag=16,((_tmpA7B.f1=t,_tmpA7B)))),_tmpA7A))));}
# 335
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpA7E;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpA7D;return(void*)((_tmpA7D=_cycalloc(sizeof(*_tmpA7D)),((_tmpA7D[0]=((_tmpA7E.tag=9,((_tmpA7E.f1=e,((_tmpA7E.f2=es,((_tmpA7E.f3=0,((_tmpA7E.f4=1,_tmpA7E)))))))))),_tmpA7D))));}
# 338
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmpA81;struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpA80;return(void*)((_tmpA80=_cycalloc(sizeof(*_tmpA80)),((_tmpA80[0]=((_tmpA81.tag=1,((_tmpA81.f1=e,_tmpA81)))),_tmpA80))));}
# 341
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmpA84;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpA83;return(void*)((_tmpA83=_cycalloc(sizeof(*_tmpA83)),((_tmpA83[0]=((_tmpA84.tag=2,((_tmpA84.f1=s1,((_tmpA84.f2=s2,_tmpA84)))))),_tmpA83))));}
# 344
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmpA87;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpA86;return(void*)((_tmpA86=_cycalloc(sizeof(*_tmpA86)),((_tmpA86[0]=((_tmpA87.tag=5,((_tmpA87.f1=e1,((_tmpA87.f2=e2,((_tmpA87.f3=e3,_tmpA87)))))))),_tmpA86))));}
# 347
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpA8A;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpA89;return(void*)((_tmpA89=_cycalloc(sizeof(*_tmpA89)),((_tmpA89[0]=((_tmpA8A.tag=20,((_tmpA8A.f1=e,((_tmpA8A.f2=n,((_tmpA8A.f3=0,((_tmpA8A.f4=0,_tmpA8A)))))))))),_tmpA89))));}
# 350
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpA8D;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpA8C;return(void*)((_tmpA8C=_cycalloc(sizeof(*_tmpA8C)),((_tmpA8C[0]=((_tmpA8D.tag=21,((_tmpA8D.f1=e,((_tmpA8D.f2=n,((_tmpA8D.f3=0,((_tmpA8D.f4=0,_tmpA8D)))))))))),_tmpA8C))));}
# 353
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 356
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpA90;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpA8F;return(void*)((_tmpA8F=_cycalloc(sizeof(*_tmpA8F)),((_tmpA8F[0]=((_tmpA90.tag=35,((_tmpA90.f1=tdopt,((_tmpA90.f2=ds,_tmpA90)))))),_tmpA8F))));}
# 358
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmpA93;struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmpA92;return(void*)((_tmpA92=_cycalloc(sizeof(*_tmpA92)),((_tmpA92[0]=((_tmpA93.tag=8,((_tmpA93.f1=v,((_tmpA93.f2=s,_tmpA93)))))),_tmpA92))));}
# 361
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 365
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,unsigned int loc){
void*_tmpDA=e->r;void*_tmpDB=_tmpDA;struct Cyc_Absyn_Exp*_tmpDD;_LL1: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpDC=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpDB;if(_tmpDC->tag != 19)goto _LL3;else{_tmpDD=_tmpDC->f1;}}_LL2:
 return Cyc_Absyn_aggrarrow_exp(_tmpDD,f,loc);_LL3:;_LL4:
 return Cyc_Absyn_aggrmember_exp(e,f,loc);_LL0:;}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 382
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
# 391
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 400
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
# 409
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
# 420
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
struct Cyc_Absyn_Exp*function;
{void*_tmpDE=Cyc_Tcutil_compress(t);void*_tmpDF=_tmpDE;enum Cyc_Absyn_Size_of _tmpE1;_LL6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpE0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpDF;if(_tmpE0->tag != 6)goto _LL8;else{_tmpE1=_tmpE0->f2;}}_LL7:
# 424
 switch(_tmpE1){case Cyc_Absyn_Char_sz: _LL12:
 function=fS->fchar;break;case Cyc_Absyn_Short_sz: _LL13:
 function=fS->fshort;break;case Cyc_Absyn_Int_sz: _LL14:
 function=fS->fint;break;default: _LL15: {
struct Cyc_Core_Impossible_exn_struct _tmpA99;const char*_tmpA98;struct Cyc_Core_Impossible_exn_struct*_tmpA97;(int)_throw((void*)((_tmpA97=_cycalloc(sizeof(*_tmpA97)),((_tmpA97[0]=((_tmpA99.tag=Cyc_Core_Impossible,((_tmpA99.f1=((_tmpA98="impossible IntType (not char, short or int)",_tag_dyneither(_tmpA98,sizeof(char),44))),_tmpA99)))),_tmpA97)))));}}
# 430
goto _LL5;_LL8: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpE2=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpDF;if(_tmpE2->tag != 7)goto _LLA;else{if(_tmpE2->f1 != 0)goto _LLA;}}_LL9:
# 432
 function=fS->ffloat;
goto _LL5;_LLA: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpE3=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpDF;if(_tmpE3->tag != 7)goto _LLC;else{if(_tmpE3->f1 != 1)goto _LLC;}}_LLB:
# 435
 function=fS->fdouble;
goto _LL5;_LLC: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpE4=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpDF;if(_tmpE4->tag != 7)goto _LLE;}_LLD:
# 438
 function=fS->flongdouble;
goto _LL5;_LLE: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpDF;if(_tmpE5->tag != 5)goto _LL10;}_LLF:
# 441
 function=fS->fvoidstar;
goto _LL5;_LL10:;_LL11: {
# 444
struct Cyc_Core_Impossible_exn_struct _tmpAA6;const char*_tmpAA5;void*_tmpAA4[1];struct Cyc_String_pa_PrintArg_struct _tmpAA3;struct Cyc_Core_Impossible_exn_struct*_tmpAA2;(int)_throw((void*)((_tmpAA2=_cycalloc(sizeof(*_tmpAA2)),((_tmpAA2[0]=((_tmpAA6.tag=Cyc_Core_Impossible,((_tmpAA6.f1=(struct _dyneither_ptr)((_tmpAA3.tag=0,((_tmpAA3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpAA4[0]=& _tmpAA3,Cyc_aprintf(((_tmpAA5="impossible expression type %s (not int, float, double, or pointer)",_tag_dyneither(_tmpAA5,sizeof(char),67))),_tag_dyneither(_tmpAA4,sizeof(void*),1)))))))),_tmpAA6)))),_tmpAA2)))));}_LL5:;}
# 446
return function;}
# 448
struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
return Cyc_Toc_getFunctionType(fS,(void*)_check_null(arr->topt));}
# 451
struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmpEE=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_tmpEF=_tmpEE;void*_tmpF1;_LL18: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpF0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpEF;if(_tmpF0->tag != 5)goto _LL1A;else{_tmpF1=(_tmpF0->f1).elt_typ;}}_LL19:
# 454
 return Cyc_Toc_getFunctionType(fS,_tmpF1);_LL1A:;_LL1B: {
struct Cyc_Core_Impossible_exn_struct _tmpAAC;const char*_tmpAAB;struct Cyc_Core_Impossible_exn_struct*_tmpAAA;(int)_throw((void*)((_tmpAAA=_cycalloc(sizeof(*_tmpAAA)),((_tmpAAA[0]=((_tmpAAC.tag=Cyc_Core_Impossible,((_tmpAAC.f1=((_tmpAAB="impossible type (not pointer)",_tag_dyneither(_tmpAAB,sizeof(char),30))),_tmpAAC)))),_tmpAAA)))));}_LL17:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 462
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmpF5=e->r;void*_tmpF6=_tmpF5;char _tmpF8;struct _dyneither_ptr _tmpFA;short _tmpFC;int _tmpFE;long long _tmp100;struct Cyc_Absyn_Exp*_tmp103;struct Cyc_List_List*_tmp105;struct Cyc_List_List*_tmp107;struct Cyc_List_List*_tmp109;struct Cyc_List_List*_tmp10B;struct Cyc_List_List*_tmp10D;_LL1D: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpF7=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF6;if(_tmpF7->tag != 0)goto _LL1F;else{if(((_tmpF7->f1).Char_c).tag != 2)goto _LL1F;_tmpF8=((struct _tuple4)((_tmpF7->f1).Char_c).val).f2;}}_LL1E:
 return _tmpF8 == '\000';_LL1F: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpF9=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF6;if(_tmpF9->tag != 0)goto _LL21;else{if(((_tmpF9->f1).Wchar_c).tag != 3)goto _LL21;_tmpFA=(struct _dyneither_ptr)((_tmpF9->f1).Wchar_c).val;}}_LL20: {
# 466
unsigned long _tmp10E=Cyc_strlen((struct _dyneither_ptr)_tmpFA);
int i=0;
if(_tmp10E >= 2  && *((const char*)_check_dyneither_subscript(_tmpFA,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmpFA,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmpFA,sizeof(char),1))== 'x'  && _tmp10E >= 3) && *((const char*)_check_dyneither_subscript(_tmpFA,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp10E;++ i){
if(*((const char*)_check_dyneither_subscript(_tmpFA,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 476
return 0;}}_LL21: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpFB=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF6;if(_tmpFB->tag != 0)goto _LL23;else{if(((_tmpFB->f1).Short_c).tag != 4)goto _LL23;_tmpFC=((struct _tuple5)((_tmpFB->f1).Short_c).val).f2;}}_LL22:
 return _tmpFC == 0;_LL23: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpFD=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF6;if(_tmpFD->tag != 0)goto _LL25;else{if(((_tmpFD->f1).Int_c).tag != 5)goto _LL25;_tmpFE=((struct _tuple6)((_tmpFD->f1).Int_c).val).f2;}}_LL24:
 return _tmpFE == 0;_LL25: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpFF=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF6;if(_tmpFF->tag != 0)goto _LL27;else{if(((_tmpFF->f1).LongLong_c).tag != 6)goto _LL27;_tmp100=((struct _tuple7)((_tmpFF->f1).LongLong_c).val).f2;}}_LL26:
 return _tmp100 == 0;_LL27: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp101=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF6;if(_tmp101->tag != 0)goto _LL29;else{if(((_tmp101->f1).Null_c).tag != 1)goto _LL29;}}_LL28:
 return 1;_LL29: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp102=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpF6;if(_tmp102->tag != 13)goto _LL2B;else{_tmp103=_tmp102->f2;}}_LL2A:
 return Cyc_Toc_is_zero(_tmp103);_LL2B: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp104=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpF6;if(_tmp104->tag != 23)goto _LL2D;else{_tmp105=_tmp104->f1;}}_LL2C:
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmp105);_LL2D: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp106=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpF6;if(_tmp106->tag != 25)goto _LL2F;else{_tmp107=_tmp106->f1;}}_LL2E:
 _tmp109=_tmp107;goto _LL30;_LL2F: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp108=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpF6;if(_tmp108->tag != 28)goto _LL31;else{_tmp109=_tmp108->f3;}}_LL30:
 _tmp10B=_tmp109;goto _LL32;_LL31: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp10A=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpF6;if(_tmp10A->tag != 24)goto _LL33;else{_tmp10B=_tmp10A->f2;}}_LL32:
 _tmp10D=_tmp10B;goto _LL34;_LL33: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp10C=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpF6;if(_tmp10C->tag != 35)goto _LL35;else{_tmp10D=_tmp10C->f2;}}_LL34:
# 487
 for(0;_tmp10D != 0;_tmp10D=_tmp10D->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple19*)_tmp10D->hd)).f2))return 0;}
return 1;_LL35:;_LL36:
 return 0;_LL1C:;}
# 495
static int Cyc_Toc_is_nullable(void*t){
void*_tmp10F=Cyc_Tcutil_compress(t);void*_tmp110=_tmp10F;union Cyc_Absyn_Constraint*_tmp112;_LL38: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp111=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp110;if(_tmp111->tag != 5)goto _LL3A;else{_tmp112=((_tmp111->f1).ptr_atts).nullable;}}_LL39:
# 498
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp112);_LL3A:;_LL3B: {
const char*_tmpAAF;void*_tmpAAE;(_tmpAAE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAAF="is_nullable",_tag_dyneither(_tmpAAF,sizeof(char),12))),_tag_dyneither(_tmpAAE,sizeof(void*),0)));}_LL37:;}
# 504
static struct _dyneither_ptr Cyc_Toc_collapse_qvar(struct _dyneither_ptr s,struct _tuple1*x){
unsigned int slen=Cyc_strlen((struct _dyneither_ptr)s);
unsigned int len=9 + slen;
union Cyc_Absyn_Nmspace _tmp116;struct _dyneither_ptr _tmp117;struct _tuple1*_tmp115=x;_tmp116=_tmp115->f1;_tmp117=*_tmp115->f2;{
unsigned int vlen=Cyc_strlen((struct _dyneither_ptr)_tmp117);
len +=1 + vlen;{
struct Cyc_List_List*nms;
{union Cyc_Absyn_Nmspace _tmp118=_tmp116;struct Cyc_List_List*_tmp119;struct Cyc_List_List*_tmp11A;struct Cyc_List_List*_tmp11B;_LL3D: if((_tmp118.Loc_n).tag != 4)goto _LL3F;_LL3E:
 nms=0;goto _LL3C;_LL3F: if((_tmp118.Rel_n).tag != 1)goto _LL41;_tmp119=(struct Cyc_List_List*)(_tmp118.Rel_n).val;_LL40:
 nms=_tmp119;goto _LL3C;_LL41: if((_tmp118.Abs_n).tag != 2)goto _LL43;_tmp11A=(struct Cyc_List_List*)(_tmp118.Abs_n).val;_LL42:
 nms=_tmp11A;goto _LL3C;_LL43: if((_tmp118.C_n).tag != 3)goto _LL3C;_tmp11B=(struct Cyc_List_List*)(_tmp118.C_n).val;_LL44:
 nms=_tmp11B;goto _LL3C;_LL3C:;}
# 517
{struct Cyc_List_List*_tmp11C=nms;for(0;_tmp11C != 0;_tmp11C=_tmp11C->tl){
len +=1 + Cyc_strlen((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp11C->hd));}}{
char*_tmpAB1;unsigned int _tmpAB0;struct _dyneither_ptr buf=(_tmpAB0=len,((_tmpAB1=_cyccalloc_atomic(sizeof(char),_tmpAB0),_tag_dyneither(_tmpAB1,sizeof(char),_tmpAB0))));
struct _dyneither_ptr p=buf;
Cyc_strcpy(p,(struct _dyneither_ptr)s);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)slen);
for(0;nms != 0;nms=nms->tl){
struct _dyneither_ptr*_tmp11D=(struct _dyneither_ptr*)nms->hd;struct _dyneither_ptr _tmp11F;struct _dyneither_ptr*_tmp11E=_tmp11D;_tmp11F=*_tmp11E;
{char _tmpAB4;char _tmpAB3;struct _dyneither_ptr _tmpAB2;(_tmpAB2=p,((_tmpAB3=*((char*)_check_dyneither_subscript(_tmpAB2,sizeof(char),0)),((_tmpAB4='_',((_get_dyneither_size(_tmpAB2,sizeof(char))== 1  && (_tmpAB3 == '\000'  && _tmpAB4 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpAB2.curr)=_tmpAB4)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp11F);
_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)Cyc_strlen((struct _dyneither_ptr)_tmp11F));}
# 528
{char _tmpAB7;char _tmpAB6;struct _dyneither_ptr _tmpAB5;(_tmpAB5=p,((_tmpAB6=*((char*)_check_dyneither_subscript(_tmpAB5,sizeof(char),0)),((_tmpAB7='_',((_get_dyneither_size(_tmpAB5,sizeof(char))== 1  && (_tmpAB6 == '\000'  && _tmpAB7 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpAB5.curr)=_tmpAB7)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp117);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)vlen);
{const char*_tmpAB8;Cyc_strcpy(p,((_tmpAB8="_struct",_tag_dyneither(_tmpAB8,sizeof(char),8))));}
return(struct _dyneither_ptr)buf;};};};}struct _tuple20{struct Cyc_Toc_TocState*f1;struct _tuple14*f2;};static char _tmp140[8]="*bogus*";
# 540
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple20*env){
# 543
static struct _dyneither_ptr bogus_string={_tmp140,_tmp140,_tmp140 + 8};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
static struct _tuple14 pair={& bogus_qvar,& bogus_qvar};
# 547
struct _tuple20 _tmp129=*env;struct Cyc_Dict_Dict*_tmp12B;struct _tuple14*_tmp12C;struct _tuple20 _tmp12A=_tmp129;_tmp12B=(_tmp12A.f1)->qvar_tags;_tmp12C=_tmp12A.f2;{
struct _tuple14 _tmp12D=*_tmp12C;struct _tuple1*_tmp12F;struct _tuple1*_tmp130;struct _tuple14 _tmp12E=_tmp12D;_tmp12F=_tmp12E.f1;_tmp130=_tmp12E.f2;{
struct _handler_cons _tmp131;_push_handler(& _tmp131);{int _tmp133=0;if(setjmp(_tmp131.handler))_tmp133=1;if(!_tmp133){
{struct _tuple1*_tmp134=((struct _tuple1*(*)(struct Cyc_Dict_Dict d,int(*cmp)(struct _tuple14*,struct _tuple14*),struct _tuple14*k))Cyc_Dict_lookup_other)(*_tmp12B,Cyc_Toc_qvar_tag_cmp,_tmp12C);_npop_handler(0);return _tmp134;};_pop_handler();}else{void*_tmp132=(void*)_exn_thrown;void*_tmp136=_tmp132;void*_tmp138;_LL46: {struct Cyc_Dict_Absent_exn_struct*_tmp137=(struct Cyc_Dict_Absent_exn_struct*)_tmp136;if(_tmp137->tag != Cyc_Dict_Absent)goto _LL48;}_LL47: {
# 553
struct _tuple14*_tmpAB9;struct _tuple14*_tmp139=(_tmpAB9=_cycalloc(sizeof(*_tmpAB9)),((_tmpAB9->f1=_tmp12F,((_tmpAB9->f2=_tmp130,_tmpAB9)))));
union Cyc_Absyn_Nmspace _tmp13B;struct _dyneither_ptr _tmp13C;struct _tuple1*_tmp13A=_tmp12F;_tmp13B=_tmp13A->f1;_tmp13C=*_tmp13A->f2;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmp13C,_tmp130);
struct _dyneither_ptr*_tmpABC;struct _tuple1*_tmpABB;struct _tuple1*res=(_tmpABB=_cycalloc(sizeof(*_tmpABB)),((_tmpABB->f1=_tmp13B,((_tmpABB->f2=((_tmpABC=_cycalloc(sizeof(*_tmpABC)),((_tmpABC[0]=newvar,_tmpABC)))),_tmpABB)))));
*_tmp12B=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmp12B,_tmp139,res);
return res;};}_LL48: _tmp138=_tmp136;_LL49:(void)_rethrow(_tmp138);_LL45:;}};};};}
# 562
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple14 _tmpABD;struct _tuple14 env=(_tmpABD.f1=fieldname,((_tmpABD.f2=dtname,_tmpABD)));
return((struct _tuple1*(*)(struct _tuple14*arg,struct _tuple1*(*f)(struct _RegionHandle*,struct _tuple20*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_collapse_qvars_body);}
# 567
static void*Cyc_Toc_typ_to_c(void*t);struct _tuple21{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple22{void*f1;struct Cyc_List_List*f2;};
# 571
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple21*env){
# 574
struct _tuple21 _tmp142=*env;struct Cyc_List_List**_tmp144;struct Cyc_List_List*_tmp145;struct _tuple21 _tmp143=_tmp142;_tmp144=(_tmp143.f1)->tuple_types;_tmp145=_tmp143.f2;
# 576
{struct Cyc_List_List*_tmp146=*_tmp144;for(0;_tmp146 != 0;_tmp146=_tmp146->tl){
struct _tuple22*_tmp147=(struct _tuple22*)_tmp146->hd;void*_tmp149;struct Cyc_List_List*_tmp14A;struct _tuple22*_tmp148=_tmp147;_tmp149=_tmp148->f1;_tmp14A=_tmp148->f2;{
struct Cyc_List_List*_tmp14B=_tmp145;
for(0;_tmp14B != 0  && _tmp14A != 0;(_tmp14B=_tmp14B->tl,_tmp14A=_tmp14A->tl)){
if(!Cyc_Tcutil_unify((*((struct _tuple11*)_tmp14B->hd)).f2,(void*)_tmp14A->hd))
break;}
if(_tmp14B == 0  && _tmp14A == 0)
return _tmp149;};}}{
# 586
struct Cyc_Int_pa_PrintArg_struct _tmpAC5;void*_tmpAC4[1];const char*_tmpAC3;struct _dyneither_ptr*_tmpAC2;struct _dyneither_ptr*xname=(_tmpAC2=_cycalloc(sizeof(*_tmpAC2)),((_tmpAC2[0]=(struct _dyneither_ptr)((_tmpAC5.tag=1,((_tmpAC5.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpAC4[0]=& _tmpAC5,Cyc_aprintf(((_tmpAC3="_tuple%d",_tag_dyneither(_tmpAC3,sizeof(char),9))),_tag_dyneither(_tmpAC4,sizeof(void*),1)))))))),_tmpAC2)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,_tmp145);
struct Cyc_List_List*_tmp14C=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
struct Cyc_Absyn_Aggrfield*_tmpAC8;struct Cyc_List_List*_tmpAC7;_tmp14C=((_tmpAC7=_cycalloc(sizeof(*_tmpAC7)),((_tmpAC7->hd=((_tmpAC8=_cycalloc(sizeof(*_tmpAC8)),((_tmpAC8->name=Cyc_Absyn_fieldname(i),((_tmpAC8->tq=Cyc_Toc_mt_tq,((_tmpAC8->type=(void*)ts2->hd,((_tmpAC8->width=0,((_tmpAC8->attributes=0,((_tmpAC8->requires_clause=0,_tmpAC8)))))))))))))),((_tmpAC7->tl=_tmp14C,_tmpAC7))))));}}
_tmp14C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp14C);{
struct Cyc_Absyn_AggrdeclImpl*_tmpACD;struct _tuple1*_tmpACC;struct Cyc_Absyn_Aggrdecl*_tmpACB;struct Cyc_Absyn_Aggrdecl*_tmp14F=(_tmpACB=_cycalloc(sizeof(*_tmpACB)),((_tmpACB->kind=Cyc_Absyn_StructA,((_tmpACB->sc=Cyc_Absyn_Public,((_tmpACB->name=(
(_tmpACC=_cycalloc(sizeof(*_tmpACC)),((_tmpACC->f1=Cyc_Absyn_Rel_n(0),((_tmpACC->f2=xname,_tmpACC)))))),((_tmpACB->tvs=0,((_tmpACB->impl=(
(_tmpACD=_cycalloc(sizeof(*_tmpACD)),((_tmpACD->exist_vars=0,((_tmpACD->rgn_po=0,((_tmpACD->fields=_tmp14C,((_tmpACD->tagged=0,_tmpACD)))))))))),((_tmpACB->attributes=0,((_tmpACB->expected_mem_kind=0,_tmpACB)))))))))))))));
# 601
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpAD3;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpAD2;struct Cyc_List_List*_tmpAD1;Cyc_Toc_result_decls=((_tmpAD1=_cycalloc(sizeof(*_tmpAD1)),((_tmpAD1->hd=Cyc_Absyn_new_decl((void*)((_tmpAD3=_cycalloc(sizeof(*_tmpAD3)),((_tmpAD3[0]=((_tmpAD2.tag=5,((_tmpAD2.f1=_tmp14F,_tmpAD2)))),_tmpAD3)))),0),((_tmpAD1->tl=Cyc_Toc_result_decls,_tmpAD1))))));}
{struct _tuple22*_tmpAD6;struct Cyc_List_List*_tmpAD5;*_tmp144=((_tmpAD5=_region_malloc(d,sizeof(*_tmpAD5)),((_tmpAD5->hd=((_tmpAD6=_region_malloc(d,sizeof(*_tmpAD6)),((_tmpAD6->f1=x,((_tmpAD6->f2=ts,_tmpAD6)))))),((_tmpAD5->tl=*_tmp144,_tmpAD5))))));}
return x;};};}
# 606
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return((void*(*)(struct Cyc_List_List*arg,void*(*f)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state)(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple23{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple24{struct Cyc_Toc_TocState*f1;struct _tuple23*f2;};struct _tuple25{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 615
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple24*env){
# 623
struct _tuple24 _tmp15C=*env;struct Cyc_List_List**_tmp15E;struct _tuple1*_tmp15F;struct Cyc_List_List*_tmp160;struct Cyc_List_List*_tmp161;struct Cyc_List_List*_tmp162;struct _tuple24 _tmp15D=_tmp15C;_tmp15E=(_tmp15D.f1)->abs_struct_types;_tmp15F=(_tmp15D.f2)->f1;_tmp160=(_tmp15D.f2)->f2;_tmp161=(_tmp15D.f2)->f3;_tmp162=(_tmp15D.f2)->f4;
# 628
{struct Cyc_List_List*_tmp163=*_tmp15E;for(0;_tmp163 != 0;_tmp163=_tmp163->tl){
struct _tuple25*_tmp164=(struct _tuple25*)_tmp163->hd;struct _tuple1*_tmp166;struct Cyc_List_List*_tmp167;void*_tmp168;struct _tuple25*_tmp165=_tmp164;_tmp166=_tmp165->f1;_tmp167=_tmp165->f2;_tmp168=_tmp165->f3;
# 631
if(Cyc_Absyn_qvar_cmp(_tmp166,_tmp15F)== 0  && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp161)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp167)){
int okay=1;
{struct Cyc_List_List*_tmp169=_tmp161;for(0;_tmp169 != 0;(_tmp169=_tmp169->tl,_tmp167=_tmp167->tl)){
void*_tmp16A=(void*)_tmp169->hd;
void*_tmp16B=(void*)((struct Cyc_List_List*)_check_null(_tmp167))->hd;
# 638
{struct Cyc_Absyn_Kind*_tmp16C=Cyc_Tcutil_typ_kind(_tmp16A);struct Cyc_Absyn_Kind*_tmp16D=_tmp16C;_LL4B: if(_tmp16D->kind != Cyc_Absyn_EffKind)goto _LL4D;_LL4C:
 goto _LL4E;_LL4D: if(_tmp16D->kind != Cyc_Absyn_RgnKind)goto _LL4F;_LL4E:
# 643
 continue;_LL4F:;_LL50:
# 646
 if(Cyc_Tcutil_unify(_tmp16A,_tmp16B) || Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(_tmp16A),Cyc_Toc_typ_to_c(_tmp16B)))
# 648
continue;
# 651
okay=0;
goto _LL4A;_LL4A:;}
# 654
break;}}
# 656
if(okay)
# 658
return _tmp168;}}}{
# 666
struct Cyc_Int_pa_PrintArg_struct _tmpADE;void*_tmpADD[1];const char*_tmpADC;struct _dyneither_ptr*_tmpADB;struct _dyneither_ptr*xname=(_tmpADB=_cycalloc(sizeof(*_tmpADB)),((_tmpADB[0]=(struct _dyneither_ptr)((_tmpADE.tag=1,((_tmpADE.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpADD[0]=& _tmpADE,Cyc_aprintf(((_tmpADC="_tuple%d",_tag_dyneither(_tmpADC,sizeof(char),9))),_tag_dyneither(_tmpADD,sizeof(void*),1)))))))),_tmpADB)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp16E=0;
# 670
{struct _tuple25*_tmpAE1;struct Cyc_List_List*_tmpAE0;*_tmp15E=((_tmpAE0=_region_malloc(d,sizeof(*_tmpAE0)),((_tmpAE0->hd=((_tmpAE1=_region_malloc(d,sizeof(*_tmpAE1)),((_tmpAE1->f1=_tmp15F,((_tmpAE1->f2=_tmp161,((_tmpAE1->f3=x,_tmpAE1)))))))),((_tmpAE0->tl=*_tmp15E,_tmpAE0))))));}{
# 673
struct _RegionHandle _tmp171=_new_region("r");struct _RegionHandle*r=& _tmp171;_push_region(r);
{struct Cyc_List_List*_tmp172=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp160,_tmp161);
for(0;_tmp162 != 0;_tmp162=_tmp162->tl){
struct Cyc_Absyn_Aggrfield*_tmp173=(struct Cyc_Absyn_Aggrfield*)_tmp162->hd;
void*t=_tmp173->type;
struct Cyc_List_List*atts=_tmp173->attributes;
# 681
if(_tmp162->tl == 0  && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(t))){
struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpAE7;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpAE6;struct Cyc_List_List*_tmpAE5;atts=((_tmpAE5=_cycalloc(sizeof(*_tmpAE5)),((_tmpAE5->hd=(void*)((_tmpAE7=_cycalloc(sizeof(*_tmpAE7)),((_tmpAE7[0]=((_tmpAE6.tag=6,((_tmpAE6.f1=0,_tmpAE6)))),_tmpAE7)))),((_tmpAE5->tl=atts,_tmpAE5))))));}
# 684
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp172,t));
# 687
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val)){
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpAED;struct Cyc_Absyn_ArrayInfo _tmpAEC;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpAEB;t=(void*)((_tmpAEB=_cycalloc(sizeof(*_tmpAEB)),((_tmpAEB[0]=((_tmpAED.tag=8,((_tmpAED.f1=((_tmpAEC.elt_type=Cyc_Absyn_void_star_typ(),((_tmpAEC.tq=Cyc_Absyn_empty_tqual(0),((_tmpAEC.num_elts=
Cyc_Absyn_uint_exp(0,0),((_tmpAEC.zero_term=((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0),((_tmpAEC.zt_loc=0,_tmpAEC)))))))))),_tmpAED)))),_tmpAEB))));}{
# 691
struct Cyc_Absyn_Aggrfield*_tmpAF0;struct Cyc_List_List*_tmpAEF;_tmp16E=((_tmpAEF=_cycalloc(sizeof(*_tmpAEF)),((_tmpAEF->hd=((_tmpAF0=_cycalloc(sizeof(*_tmpAF0)),((_tmpAF0->name=_tmp173->name,((_tmpAF0->tq=Cyc_Toc_mt_tq,((_tmpAF0->type=t,((_tmpAF0->width=_tmp173->width,((_tmpAF0->attributes=atts,((_tmpAF0->requires_clause=0,_tmpAF0)))))))))))))),((_tmpAEF->tl=_tmp16E,_tmpAEF))))));};}
# 693
_tmp16E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp16E);{
struct Cyc_Absyn_AggrdeclImpl*_tmpAF5;struct _tuple1*_tmpAF4;struct Cyc_Absyn_Aggrdecl*_tmpAF3;struct Cyc_Absyn_Aggrdecl*_tmp17C=(_tmpAF3=_cycalloc(sizeof(*_tmpAF3)),((_tmpAF3->kind=Cyc_Absyn_StructA,((_tmpAF3->sc=Cyc_Absyn_Public,((_tmpAF3->name=(
(_tmpAF4=_cycalloc(sizeof(*_tmpAF4)),((_tmpAF4->f1=Cyc_Absyn_Rel_n(0),((_tmpAF4->f2=xname,_tmpAF4)))))),((_tmpAF3->tvs=0,((_tmpAF3->impl=(
(_tmpAF5=_cycalloc(sizeof(*_tmpAF5)),((_tmpAF5->exist_vars=0,((_tmpAF5->rgn_po=0,((_tmpAF5->fields=_tmp16E,((_tmpAF5->tagged=0,_tmpAF5)))))))))),((_tmpAF3->attributes=0,((_tmpAF3->expected_mem_kind=0,_tmpAF3)))))))))))))));
# 701
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpAFB;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpAFA;struct Cyc_List_List*_tmpAF9;Cyc_Toc_result_decls=((_tmpAF9=_cycalloc(sizeof(*_tmpAF9)),((_tmpAF9->hd=Cyc_Absyn_new_decl((void*)((_tmpAFB=_cycalloc(sizeof(*_tmpAFB)),((_tmpAFB[0]=((_tmpAFA.tag=5,((_tmpAFA.f1=_tmp17C,_tmpAFA)))),_tmpAFB)))),0),((_tmpAF9->tl=Cyc_Toc_result_decls,_tmpAF9))))));}{
void*_tmp180=x;_npop_handler(0);return _tmp180;};};}
# 674
;_pop_region(r);};};}
# 705
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 709
struct _tuple23 _tmpAFC;struct _tuple23 env=(_tmpAFC.f1=struct_name,((_tmpAFC.f2=type_vars,((_tmpAFC.f3=type_args,((_tmpAFC.f4=fields,_tmpAFC)))))));
return((void*(*)(struct _tuple23*arg,void*(*f)(struct _RegionHandle*,struct _tuple24*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 716
struct _tuple1*Cyc_Toc_temp_var(){
int _tmp189=Cyc_Toc_temp_var_counter ++;
struct _dyneither_ptr*_tmpB09;const char*_tmpB08;void*_tmpB07[1];struct Cyc_Int_pa_PrintArg_struct _tmpB06;struct _tuple1*_tmpB05;struct _tuple1*res=(_tmpB05=_cycalloc(sizeof(*_tmpB05)),((_tmpB05->f1=Cyc_Absyn_Loc_n,((_tmpB05->f2=((_tmpB09=_cycalloc(sizeof(*_tmpB09)),((_tmpB09[0]=(struct _dyneither_ptr)((_tmpB06.tag=1,((_tmpB06.f1=(unsigned int)_tmp189,((_tmpB07[0]=& _tmpB06,Cyc_aprintf(((_tmpB08="_tmp%X",_tag_dyneither(_tmpB08,sizeof(char),7))),_tag_dyneither(_tmpB07,sizeof(void*),1)))))))),_tmpB09)))),_tmpB05)))));
return res;}struct _tuple26{struct Cyc_Toc_TocState*f1;int f2;};
# 724
static struct _dyneither_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple26*env){
struct _tuple26 _tmp18F=*env;struct Cyc_Xarray_Xarray*_tmp191;struct _tuple26 _tmp190=_tmp18F;_tmp191=(_tmp190.f1)->temp_labels;{
int _tmp192=Cyc_Toc_fresh_label_counter ++;
if(_tmp192 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp191))
return((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp191,_tmp192);{
struct Cyc_Int_pa_PrintArg_struct _tmpB11;void*_tmpB10[1];const char*_tmpB0F;struct _dyneither_ptr*_tmpB0E;struct _dyneither_ptr*res=(_tmpB0E=_cycalloc(sizeof(*_tmpB0E)),((_tmpB0E[0]=(struct _dyneither_ptr)((_tmpB11.tag=1,((_tmpB11.f1=(unsigned int)_tmp192,((_tmpB10[0]=& _tmpB11,Cyc_aprintf(((_tmpB0F="_LL%X",_tag_dyneither(_tmpB0F,sizeof(char),6))),_tag_dyneither(_tmpB10,sizeof(void*),1)))))))),_tmpB0E)));
if(((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp191,res)!= _tmp192){
const char*_tmpB14;void*_tmpB13;(_tmpB13=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB14="fresh_label: add_ind returned bad index...",_tag_dyneither(_tmpB14,sizeof(char),43))),_tag_dyneither(_tmpB13,sizeof(void*),0)));}
return res;};};}
# 735
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
return((struct _dyneither_ptr*(*)(int arg,struct _dyneither_ptr*(*f)(struct _RegionHandle*,struct _tuple26*)))Cyc_Toc_use_toc_state)(0,Cyc_Toc_fresh_label_body);}
# 742
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
struct Cyc_List_List*_tmp199=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp199))->hd)->name)!= 0){
++ ans;
_tmp199=_tmp199->tl;}
# 749
return Cyc_Absyn_uint_exp((unsigned int)ans,0);}
# 755
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_result_decls);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
# 760
struct _tuple9 _tmp19A=*a;struct _dyneither_ptr*_tmp19C;struct Cyc_Absyn_Tqual _tmp19D;void*_tmp19E;struct _tuple9 _tmp19B=_tmp19A;_tmp19C=_tmp19B.f1;_tmp19D=_tmp19B.f2;_tmp19E=_tmp19B.f3;{
struct _tuple9*_tmpB15;return(_tmpB15=_cycalloc(sizeof(*_tmpB15)),((_tmpB15->f1=_tmp19C,((_tmpB15->f2=_tmp19D,((_tmpB15->f3=Cyc_Toc_typ_to_c(_tmp19E),_tmpB15)))))));};}
# 763
static struct _tuple11*Cyc_Toc_typ_to_c_f(struct _tuple11*x){
struct _tuple11 _tmp1A0=*x;struct Cyc_Absyn_Tqual _tmp1A2;void*_tmp1A3;struct _tuple11 _tmp1A1=_tmp1A0;_tmp1A2=_tmp1A1.f1;_tmp1A3=_tmp1A1.f2;{
struct _tuple11*_tmpB16;return(_tmpB16=_cycalloc(sizeof(*_tmpB16)),((_tmpB16->f1=_tmp1A2,((_tmpB16->f2=Cyc_Toc_typ_to_c(_tmp1A3),_tmpB16)))));};}
# 782 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp1A5=Cyc_Tcutil_compress(t);void*_tmp1A6=_tmp1A5;void*_tmp1A8;struct Cyc_Absyn_Tqual _tmp1A9;struct Cyc_Absyn_Exp*_tmp1AA;union Cyc_Absyn_Constraint*_tmp1AB;unsigned int _tmp1AC;void*_tmp1AE;_LL52: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1A7=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1A6;if(_tmp1A7->tag != 8)goto _LL54;else{_tmp1A8=(_tmp1A7->f1).elt_type;_tmp1A9=(_tmp1A7->f1).tq;_tmp1AA=(_tmp1A7->f1).num_elts;_tmp1AB=(_tmp1A7->f1).zero_term;_tmp1AC=(_tmp1A7->f1).zt_loc;}}_LL53:
# 785
 return Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(_tmp1A8),_tmp1A9);_LL54:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp1AD=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A6;if(_tmp1AD->tag != 1)goto _LL56;else{_tmp1AE=(void*)_tmp1AD->f2;}}if(!(_tmp1AE != 0))goto _LL56;_LL55:
 return Cyc_Toc_typ_to_c_array(_tmp1AE);_LL56:;_LL57:
 return Cyc_Toc_typ_to_c(t);_LL51:;}
# 791
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){
# 793
struct Cyc_Absyn_Aggrfield*_tmpB17;return(_tmpB17=_cycalloc(sizeof(*_tmpB17)),((_tmpB17->name=f->name,((_tmpB17->tq=Cyc_Toc_mt_tq,((_tmpB17->type=
# 795
Cyc_Toc_typ_to_c(f->type),((_tmpB17->width=f->width,((_tmpB17->attributes=f->attributes,((_tmpB17->requires_clause=0,_tmpB17)))))))))))));}
# 800
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 802
return;}
# 805
static void*Cyc_Toc_char_star_typ(){
static void**cs=0;
if(cs == 0){
void**_tmpB18;cs=((_tmpB18=_cycalloc(sizeof(*_tmpB18)),((_tmpB18[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpB18))));}
# 810
return*cs;}
# 812
static void*Cyc_Toc_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpB19;r=((_tmpB19=_cycalloc(sizeof(*_tmpB19)),((_tmpB19[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),_tmpB19))));}
# 817
return*r;}
# 819
static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpB1A;r=((_tmpB1A=_cycalloc(sizeof(*_tmpB1A)),((_tmpB1A[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),_tmpB1A))));}
# 824
return*r;}
# 826
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp1B3=Cyc_Tcutil_compress(t);void*_tmp1B4=_tmp1B3;struct Cyc_Absyn_Tvar*_tmp1B6;_LL59: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1B5=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1B4;if(_tmp1B5->tag != 2)goto _LL5B;else{_tmp1B6=_tmp1B5->f1;}}_LL5A:
# 829
 return Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tbk);_LL5B:;_LL5C:
 return 0;_LL58:;}
# 833
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp1B7=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp1B8=_tmp1B7;_LL5E: if(_tmp1B8->kind != Cyc_Absyn_AnyKind)goto _LL60;_LL5F:
 return 1;_LL60:;_LL61:
 return 0;_LL5D:;}
# 840
static void*Cyc_Toc_typ_to_c(void*t){
void*_tmp1B9=t;void**_tmp1BC;struct Cyc_Absyn_Tvar*_tmp1BE;struct Cyc_Absyn_Datatypedecl*_tmp1C1;struct Cyc_Absyn_Datatypefield*_tmp1C2;void*_tmp1C5;struct Cyc_Absyn_Tqual _tmp1C6;union Cyc_Absyn_Constraint*_tmp1C7;void*_tmp1CB;struct Cyc_Absyn_Tqual _tmp1CC;struct Cyc_Absyn_Exp*_tmp1CD;unsigned int _tmp1CE;struct Cyc_Absyn_Tqual _tmp1D0;void*_tmp1D1;struct Cyc_List_List*_tmp1D2;int _tmp1D3;struct Cyc_Absyn_VarargInfo*_tmp1D4;struct Cyc_List_List*_tmp1D5;struct Cyc_List_List*_tmp1D7;enum Cyc_Absyn_AggrKind _tmp1D9;struct Cyc_List_List*_tmp1DA;union Cyc_Absyn_AggrInfoU _tmp1DC;struct Cyc_List_List*_tmp1DD;struct _tuple1*_tmp1DF;struct Cyc_List_List*_tmp1E1;struct _tuple1*_tmp1E3;struct Cyc_List_List*_tmp1E4;struct Cyc_Absyn_Typedefdecl*_tmp1E5;void*_tmp1E6;void*_tmp1E9;struct Cyc_Absyn_Exp*_tmp1F2;struct Cyc_Absyn_Exp*_tmp1F4;struct Cyc_Absyn_Aggrdecl*_tmp1F7;struct Cyc_Absyn_Enumdecl*_tmp1FA;struct Cyc_Absyn_Datatypedecl*_tmp1FD;void**_tmp1FE;_LL63: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp1BA=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp1B9;if(_tmp1BA->tag != 0)goto _LL65;}_LL64:
 return t;_LL65: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp1BB=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B9;if(_tmp1BB->tag != 1)goto _LL67;else{_tmp1BC=(void**)((void**)& _tmp1BB->f2);}}_LL66:
# 844
 if(*_tmp1BC == 0){
*_tmp1BC=Cyc_Absyn_sint_typ;
return Cyc_Absyn_sint_typ;}
# 848
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp1BC));_LL67: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1BD=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1B9;if(_tmp1BD->tag != 2)goto _LL69;else{_tmp1BE=_tmp1BD->f1;}}_LL68:
# 850
 if((Cyc_Tcutil_tvar_kind(_tmp1BE,& Cyc_Tcutil_bk))->kind == Cyc_Absyn_AnyKind)
# 853
return(void*)& Cyc_Absyn_VoidType_val;else{
# 855
return Cyc_Absyn_void_star_typ();}_LL69: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp1BF=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1B9;if(_tmp1BF->tag != 3)goto _LL6B;}_LL6A:
# 857
 return(void*)& Cyc_Absyn_VoidType_val;_LL6B: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp1C0=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1B9;if(_tmp1C0->tag != 4)goto _LL6D;else{if((((_tmp1C0->f1).field_info).KnownDatatypefield).tag != 2)goto _LL6D;_tmp1C1=((struct _tuple2)(((_tmp1C0->f1).field_info).KnownDatatypefield).val).f1;_tmp1C2=((struct _tuple2)(((_tmp1C0->f1).field_info).KnownDatatypefield).val).f2;}}_LL6C:
# 859
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp1C2->name,_tmp1C1->name));_LL6D: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp1C3=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1B9;if(_tmp1C3->tag != 4)goto _LL6F;}_LL6E: {
const char*_tmpB1D;void*_tmpB1C;(_tmpB1C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB1D="unresolved DatatypeFieldType",_tag_dyneither(_tmpB1D,sizeof(char),29))),_tag_dyneither(_tmpB1C,sizeof(void*),0)));}_LL6F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1C4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B9;if(_tmp1C4->tag != 5)goto _LL71;else{_tmp1C5=(_tmp1C4->f1).elt_typ;_tmp1C6=(_tmp1C4->f1).elt_tq;_tmp1C7=((_tmp1C4->f1).ptr_atts).bounds;}}_LL70:
# 864
 _tmp1C5=Cyc_Toc_typ_to_c(_tmp1C5);{
void*_tmp201=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp1C7);void*_tmp202=_tmp201;_LLA2: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp203=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp202;if(_tmp203->tag != 0)goto _LLA4;}_LLA3:
 return Cyc_Toc_dyneither_ptr_typ;_LLA4:;_LLA5:
 return Cyc_Absyn_star_typ(_tmp1C5,(void*)& Cyc_Absyn_HeapRgn_val,_tmp1C6,Cyc_Absyn_false_conref);_LLA1:;};_LL71: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp1C8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1B9;if(_tmp1C8->tag != 6)goto _LL73;}_LL72:
# 869
 goto _LL74;_LL73: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp1C9=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp1B9;if(_tmp1C9->tag != 7)goto _LL75;}_LL74:
 return t;_LL75: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1CA=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1B9;if(_tmp1CA->tag != 8)goto _LL77;else{_tmp1CB=(_tmp1CA->f1).elt_type;_tmp1CC=(_tmp1CA->f1).tq;_tmp1CD=(_tmp1CA->f1).num_elts;_tmp1CE=(_tmp1CA->f1).zt_loc;}}_LL76:
# 872
 return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c(_tmp1CB),_tmp1CC,_tmp1CD,Cyc_Absyn_false_conref,_tmp1CE);_LL77: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1CF=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B9;if(_tmp1CF->tag != 9)goto _LL79;else{_tmp1D0=(_tmp1CF->f1).ret_tqual;_tmp1D1=(_tmp1CF->f1).ret_typ;_tmp1D2=(_tmp1CF->f1).args;_tmp1D3=(_tmp1CF->f1).c_varargs;_tmp1D4=(_tmp1CF->f1).cyc_varargs;_tmp1D5=(_tmp1CF->f1).attributes;}}_LL78: {
# 878
struct Cyc_List_List*_tmp204=0;
for(0;_tmp1D5 != 0;_tmp1D5=_tmp1D5->tl){
void*_tmp205=(void*)_tmp1D5->hd;void*_tmp206=_tmp205;_LLA7: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp207=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp206;if(_tmp207->tag != 4)goto _LLA9;}_LLA8:
 goto _LLAA;_LLA9: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp208=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp206;if(_tmp208->tag != 5)goto _LLAB;}_LLAA:
 goto _LLAC;_LLAB: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp209=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp206;if(_tmp209->tag != 19)goto _LLAD;}_LLAC:
 continue;_LLAD: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp20A=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp206;if(_tmp20A->tag != 22)goto _LLAF;}_LLAE:
 continue;_LLAF: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp20B=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp206;if(_tmp20B->tag != 21)goto _LLB1;}_LLB0:
 continue;_LLB1: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp20C=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp206;if(_tmp20C->tag != 20)goto _LLB3;}_LLB2:
 continue;_LLB3:;_LLB4:
{struct Cyc_List_List*_tmpB1E;_tmp204=((_tmpB1E=_cycalloc(sizeof(*_tmpB1E)),((_tmpB1E->hd=(void*)_tmp1D5->hd,((_tmpB1E->tl=_tmp204,_tmpB1E))))));}goto _LLA6;_LLA6:;}{
# 889
struct Cyc_List_List*_tmp20E=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp1D2);
if(_tmp1D4 != 0){
# 892
void*_tmp20F=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp1D4->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple9*_tmpB1F;struct _tuple9*_tmp210=(_tmpB1F=_cycalloc(sizeof(*_tmpB1F)),((_tmpB1F->f1=_tmp1D4->name,((_tmpB1F->f2=_tmp1D4->tq,((_tmpB1F->f3=_tmp20F,_tmpB1F)))))));
struct Cyc_List_List*_tmpB20;_tmp20E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp20E,((_tmpB20=_cycalloc(sizeof(*_tmpB20)),((_tmpB20->hd=_tmp210,((_tmpB20->tl=0,_tmpB20)))))));}{
# 896
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpB26;struct Cyc_Absyn_FnInfo _tmpB25;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpB24;return(void*)((_tmpB24=_cycalloc(sizeof(*_tmpB24)),((_tmpB24[0]=((_tmpB26.tag=9,((_tmpB26.f1=((_tmpB25.tvars=0,((_tmpB25.effect=0,((_tmpB25.ret_tqual=_tmp1D0,((_tmpB25.ret_typ=Cyc_Toc_typ_to_c(_tmp1D1),((_tmpB25.args=_tmp20E,((_tmpB25.c_varargs=_tmp1D3,((_tmpB25.cyc_varargs=0,((_tmpB25.rgn_po=0,((_tmpB25.attributes=_tmp204,((_tmpB25.requires_clause=0,((_tmpB25.requires_relns=0,((_tmpB25.ensures_clause=0,((_tmpB25.ensures_relns=0,_tmpB25)))))))))))))))))))))))))),_tmpB26)))),_tmpB24))));};};}_LL79: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp1D6=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1B9;if(_tmp1D6->tag != 10)goto _LL7B;else{_tmp1D7=_tmp1D6->f1;}}_LL7A:
# 901
 _tmp1D7=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp1D7);
return Cyc_Toc_add_tuple_type(_tmp1D7);_LL7B: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1D8=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1B9;if(_tmp1D8->tag != 12)goto _LL7D;else{_tmp1D9=_tmp1D8->f1;_tmp1DA=_tmp1D8->f2;}}_LL7C: {
# 906
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpB29;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpB28;return(void*)((_tmpB28=_cycalloc(sizeof(*_tmpB28)),((_tmpB28[0]=((_tmpB29.tag=12,((_tmpB29.f1=_tmp1D9,((_tmpB29.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp1DA),_tmpB29)))))),_tmpB28))));}_LL7D: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1DB=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B9;if(_tmp1DB->tag != 11)goto _LL7F;else{_tmp1DC=(_tmp1DB->f1).aggr_info;_tmp1DD=(_tmp1DB->f1).targs;}}_LL7E:
# 910
{union Cyc_Absyn_AggrInfoU _tmp218=_tmp1DC;_LLB6: if((_tmp218.UnknownAggr).tag != 1)goto _LLB8;_LLB7:
 return t;_LLB8:;_LLB9:
 goto _LLB5;_LLB5:;}{
# 914
struct Cyc_Absyn_Aggrdecl*_tmp219=Cyc_Absyn_get_known_aggrdecl(_tmp1DC);
if(_tmp219->expected_mem_kind){
# 917
if(_tmp219->impl == 0){
const char*_tmpB32;void*_tmpB31[2];const char*_tmpB30;const char*_tmpB2F;struct Cyc_String_pa_PrintArg_struct _tmpB2E;struct Cyc_String_pa_PrintArg_struct _tmpB2D;(_tmpB2D.tag=0,((_tmpB2D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 920
Cyc_Absynpp_qvar2string(_tmp219->name)),((_tmpB2E.tag=0,((_tmpB2E.f1=(struct _dyneither_ptr)(
# 919
_tmp219->kind == Cyc_Absyn_UnionA?(_tmpB2F="union",_tag_dyneither(_tmpB2F,sizeof(char),6)):((_tmpB30="struct",_tag_dyneither(_tmpB30,sizeof(char),7)))),((_tmpB31[0]=& _tmpB2E,((_tmpB31[1]=& _tmpB2D,Cyc_Tcutil_warn(0,((_tmpB32="%s %s was never defined.",_tag_dyneither(_tmpB32,sizeof(char),25))),_tag_dyneither(_tmpB31,sizeof(void*),2)))))))))))));}}
# 925
if(_tmp219->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp219->name,Cyc_Absyn_unionq_typ);{
struct Cyc_List_List*_tmp220=_tmp219->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp219->impl))->fields;
if(_tmp220 == 0)return Cyc_Toc_aggrdecl_type(_tmp219->name,Cyc_Absyn_strctq);
for(0;_tmp220->tl != 0;_tmp220=_tmp220->tl){;}{
void*_tmp221=((struct Cyc_Absyn_Aggrfield*)_tmp220->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp221))){
if(_tmp219->expected_mem_kind){
const char*_tmpB36;void*_tmpB35[1];struct Cyc_String_pa_PrintArg_struct _tmpB34;(_tmpB34.tag=0,((_tmpB34.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp219->name)),((_tmpB35[0]=& _tmpB34,Cyc_Tcutil_warn(0,((_tmpB36="struct %s ended up being abstract.",_tag_dyneither(_tmpB36,sizeof(char),35))),_tag_dyneither(_tmpB35,sizeof(void*),1)))))));}{
# 939
struct _RegionHandle _tmp225=_new_region("r");struct _RegionHandle*r=& _tmp225;_push_region(r);
{struct Cyc_List_List*_tmp226=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp219->tvs,_tmp1DD);
void*_tmp227=Cyc_Tcutil_rsubstitute(r,_tmp226,_tmp221);
if(Cyc_Toc_is_abstract_type(_tmp227)){void*_tmp228=Cyc_Toc_aggrdecl_type(_tmp219->name,Cyc_Absyn_strctq);_npop_handler(0);return _tmp228;}{
void*_tmp229=Cyc_Toc_add_struct_type(_tmp219->name,_tmp219->tvs,_tmp1DD,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp219->impl))->fields);_npop_handler(0);return _tmp229;};}
# 940
;_pop_region(r);};}
# 945
return Cyc_Toc_aggrdecl_type(_tmp219->name,Cyc_Absyn_strctq);};};};_LL7F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp1DE=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp1B9;if(_tmp1DE->tag != 13)goto _LL81;else{_tmp1DF=_tmp1DE->f1;}}_LL80:
 return t;_LL81: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp1E0=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1B9;if(_tmp1E0->tag != 14)goto _LL83;else{_tmp1E1=_tmp1E0->f1;}}_LL82:
 Cyc_Toc_enumfields_to_c(_tmp1E1);return t;_LL83: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp1E2=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1B9;if(_tmp1E2->tag != 17)goto _LL85;else{_tmp1E3=_tmp1E2->f1;_tmp1E4=_tmp1E2->f2;_tmp1E5=_tmp1E2->f3;_tmp1E6=(void*)_tmp1E2->f4;}}_LL84:
# 949
 if(_tmp1E6 == 0  || Cyc_noexpand_r){
if(_tmp1E4 != 0){
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpB39;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpB38;return(void*)((_tmpB38=_cycalloc(sizeof(*_tmpB38)),((_tmpB38[0]=((_tmpB39.tag=17,((_tmpB39.f1=_tmp1E3,((_tmpB39.f2=0,((_tmpB39.f3=_tmp1E5,((_tmpB39.f4=0,_tmpB39)))))))))),_tmpB38))));}else{
return t;}}else{
# 954
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpB3C;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpB3B;return(void*)((_tmpB3B=_cycalloc(sizeof(*_tmpB3B)),((_tmpB3B[0]=((_tmpB3C.tag=17,((_tmpB3C.f1=_tmp1E3,((_tmpB3C.f2=0,((_tmpB3C.f3=_tmp1E5,((_tmpB3C.f4=Cyc_Toc_typ_to_c(_tmp1E6),_tmpB3C)))))))))),_tmpB3B))));}_LL85: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp1E7=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp1B9;if(_tmp1E7->tag != 19)goto _LL87;}_LL86:
 return Cyc_Absyn_uint_typ;_LL87: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp1E8=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1B9;if(_tmp1E8->tag != 15)goto _LL89;else{_tmp1E9=(void*)_tmp1E8->f1;}}_LL88:
 return Cyc_Toc_rgn_typ();_LL89: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp1EA=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp1B9;if(_tmp1EA->tag != 16)goto _LL8B;}_LL8A:
 return Cyc_Toc_dyn_rgn_typ();_LL8B: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp1EB=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp1B9;if(_tmp1EB->tag != 20)goto _LL8D;}_LL8C:
# 960
 goto _LL8E;_LL8D: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp1EC=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp1B9;if(_tmp1EC->tag != 21)goto _LL8F;}_LL8E:
 goto _LL90;_LL8F: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp1ED=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp1B9;if(_tmp1ED->tag != 22)goto _LL91;}_LL90:
 goto _LL92;_LL91: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1EE=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp1B9;if(_tmp1EE->tag != 23)goto _LL93;}_LL92:
 goto _LL94;_LL93: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1EF=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp1B9;if(_tmp1EF->tag != 24)goto _LL95;}_LL94:
 goto _LL96;_LL95: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1F0=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp1B9;if(_tmp1F0->tag != 25)goto _LL97;}_LL96:
 return Cyc_Absyn_void_star_typ();_LL97: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1F1=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp1B9;if(_tmp1F1->tag != 18)goto _LL99;else{_tmp1F2=_tmp1F1->f1;}}_LL98:
# 970
 return t;_LL99: {struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp1F3=(struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp1B9;if(_tmp1F3->tag != 27)goto _LL9B;else{_tmp1F4=_tmp1F3->f1;}}_LL9A:
# 972
 return t;_LL9B: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1F5=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1B9;if(_tmp1F5->tag != 26)goto _LL9D;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp1F6=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp1F5->f1)->r;if(_tmp1F6->tag != 0)goto _LL9D;else{_tmp1F7=_tmp1F6->f1;}}}_LL9C:
# 974
 Cyc_Toc_aggrdecl_to_c(_tmp1F7,1);
if(_tmp1F7->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp1F7->name,Cyc_Absyn_unionq_typ);else{
return Cyc_Toc_aggrdecl_type(_tmp1F7->name,Cyc_Absyn_strctq);}_LL9D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1F8=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1B9;if(_tmp1F8->tag != 26)goto _LL9F;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp1F9=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp1F8->f1)->r;if(_tmp1F9->tag != 1)goto _LL9F;else{_tmp1FA=_tmp1F9->f1;}}}_LL9E:
# 979
 Cyc_Toc_enumdecl_to_c(_tmp1FA);
return t;_LL9F: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1FB=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1B9;if(_tmp1FB->tag != 26)goto _LL62;else{{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp1FC=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp1FB->f1)->r;if(_tmp1FC->tag != 2)goto _LL62;else{_tmp1FD=_tmp1FC->f1;}}_tmp1FE=_tmp1FB->f2;}}_LLA0:
# 982
 Cyc_Toc_datatypedecl_to_c(_tmp1FD);
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp1FE)));_LL62:;}
# 987
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned int l){
void*_tmp22E=t;void*_tmp230;struct Cyc_Absyn_Tqual _tmp231;_LLBB: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp22F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp22E;if(_tmp22F->tag != 8)goto _LLBD;else{_tmp230=(_tmp22F->f1).elt_type;_tmp231=(_tmp22F->f1).tq;}}_LLBC:
# 990
 return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp230,(void*)& Cyc_Absyn_HeapRgn_val,_tmp231,Cyc_Absyn_false_conref),e);_LLBD:;_LLBE:
 return Cyc_Toc_cast_it(t,e);_LLBA:;}
# 997
static int Cyc_Toc_atomic_typ(void*t){
void*_tmp232=Cyc_Tcutil_compress(t);void*_tmp233=_tmp232;void*_tmp23D;union Cyc_Absyn_AggrInfoU _tmp23F;struct Cyc_List_List*_tmp241;struct Cyc_Absyn_Datatypedecl*_tmp243;struct Cyc_Absyn_Datatypefield*_tmp244;struct Cyc_List_List*_tmp246;_LLC0: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp234=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp233;if(_tmp234->tag != 0)goto _LLC2;}_LLC1:
 return 1;_LLC2: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp235=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp233;if(_tmp235->tag != 2)goto _LLC4;}_LLC3:
 return 0;_LLC4: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp236=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp233;if(_tmp236->tag != 6)goto _LLC6;}_LLC5:
 goto _LLC7;_LLC6: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp237=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp233;if(_tmp237->tag != 13)goto _LLC8;}_LLC7:
 goto _LLC9;_LLC8: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp238=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp233;if(_tmp238->tag != 14)goto _LLCA;}_LLC9:
 goto _LLCB;_LLCA: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp239=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp233;if(_tmp239->tag != 7)goto _LLCC;}_LLCB:
 goto _LLCD;_LLCC: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp23A=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp233;if(_tmp23A->tag != 9)goto _LLCE;}_LLCD:
 goto _LLCF;_LLCE: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp23B=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp233;if(_tmp23B->tag != 19)goto _LLD0;}_LLCF:
 return 1;_LLD0: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp23C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp233;if(_tmp23C->tag != 8)goto _LLD2;else{_tmp23D=(_tmp23C->f1).elt_type;}}_LLD1:
 return Cyc_Toc_atomic_typ(_tmp23D);_LLD2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp23E=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp233;if(_tmp23E->tag != 11)goto _LLD4;else{_tmp23F=(_tmp23E->f1).aggr_info;}}_LLD3:
# 1014
{union Cyc_Absyn_AggrInfoU _tmp24B=_tmp23F;_LLE5: if((_tmp24B.UnknownAggr).tag != 1)goto _LLE7;_LLE6:
 return 0;_LLE7:;_LLE8:
 goto _LLE4;_LLE4:;}{
# 1018
struct Cyc_Absyn_Aggrdecl*_tmp24C=Cyc_Absyn_get_known_aggrdecl(_tmp23F);
if(_tmp24C->impl == 0)
return 0;
{struct Cyc_List_List*_tmp24D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp24C->impl))->fields;for(0;_tmp24D != 0;_tmp24D=_tmp24D->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp24D->hd)->type))return 0;}}
return 1;};_LLD4: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp240=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp233;if(_tmp240->tag != 12)goto _LLD6;else{_tmp241=_tmp240->f2;}}_LLD5:
# 1025
 for(0;_tmp241 != 0;_tmp241=_tmp241->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp241->hd)->type))return 0;}
return 1;_LLD6: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp242=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp233;if(_tmp242->tag != 4)goto _LLD8;else{if((((_tmp242->f1).field_info).KnownDatatypefield).tag != 2)goto _LLD8;_tmp243=((struct _tuple2)(((_tmp242->f1).field_info).KnownDatatypefield).val).f1;_tmp244=((struct _tuple2)(((_tmp242->f1).field_info).KnownDatatypefield).val).f2;}}_LLD7:
# 1029
 _tmp246=_tmp244->typs;goto _LLD9;_LLD8: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp245=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp233;if(_tmp245->tag != 10)goto _LLDA;else{_tmp246=_tmp245->f1;}}_LLD9:
# 1031
 for(0;_tmp246 != 0;_tmp246=_tmp246->tl){
if(!Cyc_Toc_atomic_typ((*((struct _tuple11*)_tmp246->hd)).f2))return 0;}
return 1;_LLDA: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp247=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp233;if(_tmp247->tag != 3)goto _LLDC;}_LLDB:
# 1036
 goto _LLDD;_LLDC: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp248=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp233;if(_tmp248->tag != 5)goto _LLDE;}_LLDD:
 goto _LLDF;_LLDE: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp249=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp233;if(_tmp249->tag != 16)goto _LLE0;}_LLDF:
 goto _LLE1;_LLE0: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp24A=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp233;if(_tmp24A->tag != 15)goto _LLE2;}_LLE1:
 return 0;_LLE2:;_LLE3: {
const char*_tmpB40;void*_tmpB3F[1];struct Cyc_String_pa_PrintArg_struct _tmpB3E;(_tmpB3E.tag=0,((_tmpB3E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpB3F[0]=& _tmpB3E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB40="atomic_typ:  bad type %s",_tag_dyneither(_tmpB40,sizeof(char),25))),_tag_dyneither(_tmpB3F,sizeof(void*),1)))))));}_LLBF:;}
# 1044
static int Cyc_Toc_is_void_star(void*t){
void*_tmp251=Cyc_Tcutil_compress(t);void*_tmp252=_tmp251;void*_tmp254;_LLEA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp253=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp252;if(_tmp253->tag != 5)goto _LLEC;else{_tmp254=(_tmp253->f1).elt_typ;}}_LLEB: {
# 1047
void*_tmp255=Cyc_Tcutil_compress(_tmp254);void*_tmp256=_tmp255;_LLEF: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp257=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp256;if(_tmp257->tag != 0)goto _LLF1;}_LLF0:
 return 1;_LLF1:;_LLF2:
 return 0;_LLEE:;}_LLEC:;_LLED:
# 1051
 return 0;_LLE9:;}
# 1055
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1059
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1064
static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){
void*_tmp258=Cyc_Tcutil_compress(t);void*_tmp259=_tmp258;union Cyc_Absyn_AggrInfoU _tmp25B;struct Cyc_List_List*_tmp25D;_LLF4: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp25A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp259;if(_tmp25A->tag != 11)goto _LLF6;else{_tmp25B=(_tmp25A->f1).aggr_info;}}_LLF5: {
# 1067
struct Cyc_Absyn_Aggrdecl*_tmp25E=Cyc_Absyn_get_known_aggrdecl(_tmp25B);
if(_tmp25E->impl == 0){
const char*_tmpB43;void*_tmpB42;(_tmpB42=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB43="is_poly_field: type missing fields",_tag_dyneither(_tmpB43,sizeof(char),35))),_tag_dyneither(_tmpB42,sizeof(void*),0)));}
_tmp25D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp25E->impl))->fields;goto _LLF7;}_LLF6: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp25C=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp259;if(_tmp25C->tag != 12)goto _LLF8;else{_tmp25D=_tmp25C->f2;}}_LLF7: {
# 1072
struct Cyc_Absyn_Aggrfield*_tmp261=Cyc_Absyn_lookup_field(_tmp25D,f);
if(_tmp261 == 0){
const char*_tmpB47;void*_tmpB46[1];struct Cyc_String_pa_PrintArg_struct _tmpB45;(_tmpB45.tag=0,((_tmpB45.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpB46[0]=& _tmpB45,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB47="is_poly_field: bad field %s",_tag_dyneither(_tmpB47,sizeof(char),28))),_tag_dyneither(_tmpB46,sizeof(void*),1)))))));}
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp261->type);}_LLF8:;_LLF9: {
const char*_tmpB4B;void*_tmpB4A[1];struct Cyc_String_pa_PrintArg_struct _tmpB49;(_tmpB49.tag=0,((_tmpB49.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpB4A[0]=& _tmpB49,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB4B="is_poly_field: bad type %s",_tag_dyneither(_tmpB4B,sizeof(char),27))),_tag_dyneither(_tmpB4A,sizeof(void*),1)))))));}_LLF3:;}
# 1083
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp268=e->r;void*_tmp269=_tmp268;struct Cyc_Absyn_Exp*_tmp26B;struct _dyneither_ptr*_tmp26C;struct Cyc_Absyn_Exp*_tmp26E;struct _dyneither_ptr*_tmp26F;_LLFB: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp26A=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp26A->tag != 20)goto _LLFD;else{_tmp26B=_tmp26A->f1;_tmp26C=_tmp26A->f2;}}_LLFC:
# 1086
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp26B->topt),_tmp26C) && !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));_LLFD: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp26D=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp269;if(_tmp26D->tag != 21)goto _LLFF;else{_tmp26E=_tmp26D->f1;_tmp26F=_tmp26D->f2;}}_LLFE: {
# 1089
void*_tmp270=Cyc_Tcutil_compress((void*)_check_null(_tmp26E->topt));void*_tmp271=_tmp270;void*_tmp273;_LL102: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp272=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp271;if(_tmp272->tag != 5)goto _LL104;else{_tmp273=(_tmp272->f1).elt_typ;}}_LL103:
# 1091
 return Cyc_Toc_is_poly_field(_tmp273,_tmp26F) && !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));_LL104:;_LL105: {
const char*_tmpB4F;void*_tmpB4E[1];struct Cyc_String_pa_PrintArg_struct _tmpB4D;(_tmpB4D.tag=0,((_tmpB4D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp26E->topt))),((_tmpB4E[0]=& _tmpB4D,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB4F="is_poly_project: bad type %s",_tag_dyneither(_tmpB4F,sizeof(char),29))),_tag_dyneither(_tmpB4E,sizeof(void*),1)))))));}_LL101:;}_LLFF:;_LL100:
# 1094
 return 0;_LLFA:;}
# 1099
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpB50;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((_tmpB50=_cycalloc(sizeof(*_tmpB50)),((_tmpB50->hd=s,((_tmpB50->tl=0,_tmpB50)))))),0);}
# 1103
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpB51;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((_tmpB51=_cycalloc(sizeof(*_tmpB51)),((_tmpB51->hd=s,((_tmpB51->tl=0,_tmpB51)))))),0);}
# 1107
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1113
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*_tmpB52[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmpB52[1]=s,((_tmpB52[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB52,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
# 1117
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type)){
struct Cyc_Absyn_Exp*_tmpB53[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpB53[1]=n,((_tmpB53[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB53,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{
# 1121
struct Cyc_Absyn_Exp*_tmpB54[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpB54[1]=n,((_tmpB54[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB54,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}
# 1124
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*_tmpB55[3];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,((_tmpB55[2]=n,((_tmpB55[1]=s,((_tmpB55[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB55,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
# 1129
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
struct Cyc_List_List*_tmpB56;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((_tmpB56=_cycalloc(sizeof(*_tmpB56)),((_tmpB56->hd=e,((_tmpB56->tl=0,_tmpB56)))))),0);}
# 1132
static struct Cyc_Absyn_Exp*Cyc_Toc_newrethrow_exp(struct Cyc_Absyn_Exp*e){
struct Cyc_List_List*_tmpB57;return Cyc_Absyn_fncall_exp(Cyc_Toc__rethrow_e,((_tmpB57=_cycalloc(sizeof(*_tmpB57)),((_tmpB57->hd=e,((_tmpB57->tl=0,_tmpB57)))))),0);}
# 1136
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
struct Cyc_Absyn_Exp*_tmpB58[0];return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB58,sizeof(struct Cyc_Absyn_Exp*),0)),0),0);}
# 1142
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1150
int is_string=0;
{void*_tmp280=e->r;void*_tmp281=_tmp280;_LL107: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp282=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp281;if(_tmp282->tag != 0)goto _LL109;else{if(((_tmp282->f1).String_c).tag != 8)goto _LL109;}}_LL108:
 is_string=1;goto _LL106;_LL109: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp283=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp281;if(_tmp283->tag != 0)goto _LL10B;else{if(((_tmp283->f1).Wstring_c).tag != 9)goto _LL10B;}}_LL10A:
 is_string=1;goto _LL106;_LL10B:;_LL10C:
 goto _LL106;_LL106:;}{
# 1156
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
if(is_string){
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_conref,0);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB5E;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB5D;struct Cyc_List_List*_tmpB5C;Cyc_Toc_result_decls=((_tmpB5C=_cycalloc(sizeof(*_tmpB5C)),((_tmpB5C->hd=Cyc_Absyn_new_decl((void*)((_tmpB5E=_cycalloc(sizeof(*_tmpB5E)),((_tmpB5E[0]=((_tmpB5D.tag=0,((_tmpB5D.f1=vd,_tmpB5D)))),_tmpB5E)))),0),((_tmpB5C->tl=Cyc_Toc_result_decls,_tmpB5C))))));}
xexp=Cyc_Absyn_var_exp(x,0);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{
# 1166
xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),e);
# 1168
xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
# 1170
struct Cyc_Absyn_Exp*urm_exp;
{struct _tuple19*_tmpB65;struct _tuple19*_tmpB64;struct _tuple19*_tmpB63;struct _tuple19*_tmpB62[3];urm_exp=Cyc_Absyn_unresolvedmem_exp(0,((_tmpB62[2]=(
# 1173
(_tmpB63=_cycalloc(sizeof(*_tmpB63)),((_tmpB63->f1=0,((_tmpB63->f2=xplussz,_tmpB63)))))),((_tmpB62[1]=(
# 1172
(_tmpB64=_cycalloc(sizeof(*_tmpB64)),((_tmpB64->f1=0,((_tmpB64->f2=xexp,_tmpB64)))))),((_tmpB62[0]=(
# 1171
(_tmpB65=_cycalloc(sizeof(*_tmpB65)),((_tmpB65->f1=0,((_tmpB65->f2=xexp,_tmpB65)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB62,sizeof(struct _tuple19*),3)))))))),0);}
# 1174
return urm_exp;};};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_StructInfo{struct Cyc_Absyn_Exp**varsizeexp;struct Cyc_Absyn_Exp*lhs_exp;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;int*in_lhs;struct Cyc_Toc_StructInfo struct_info;int*in_sizeof;struct _RegionHandle*rgn;};
# 1217 "toc.cyc"
typedef struct Cyc_Toc_Env*Cyc_Toc_env_t;
# 1219
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
int _tmp28C;struct Cyc_Toc_Env*_tmp28B=nv;_tmp28C=_tmp28B->toplevel;
return _tmp28C;}
# 1223
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
int*_tmp28E;struct Cyc_Toc_Env*_tmp28D=nv;_tmp28E=_tmp28D->in_lhs;
return*_tmp28E;}
# 1228
static struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple1*x){
struct Cyc_Dict_Dict _tmp290;struct Cyc_Toc_Env*_tmp28F=nv;_tmp290=_tmp28F->varmap;
return((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp290,x);}
# 1234
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
int*_tmpB6D;struct Cyc_Absyn_Exp**_tmpB6C;int*_tmpB6B;struct Cyc_Toc_Env*_tmpB6A;return(_tmpB6A=_region_malloc(r,sizeof(*_tmpB6A)),((_tmpB6A->break_lab=(struct _dyneither_ptr**)0,((_tmpB6A->continue_lab=(struct _dyneither_ptr**)0,((_tmpB6A->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpB6A->varmap=(struct Cyc_Dict_Dict)
# 1238
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((_tmpB6A->toplevel=(int)1,((_tmpB6A->in_lhs=(int*)(
# 1240
(_tmpB6B=_region_malloc(r,sizeof(*_tmpB6B)),((_tmpB6B[0]=0,_tmpB6B)))),((_tmpB6A->struct_info=(struct Cyc_Toc_StructInfo)(
((_tmpB6A->struct_info).varsizeexp=((_tmpB6C=_cycalloc(sizeof(*_tmpB6C)),((_tmpB6C[0]=0,_tmpB6C)))),(((_tmpB6A->struct_info).lhs_exp=0,_tmpB6A->struct_info)))),((_tmpB6A->in_sizeof=(int*)(
(_tmpB6D=_region_malloc(r,sizeof(*_tmpB6D)),((_tmpB6D[0]=0,_tmpB6D)))),((_tmpB6A->rgn=(struct _RegionHandle*)r,_tmpB6A)))))))))))))))))));}
# 1246
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp296;struct _dyneither_ptr**_tmp297;struct Cyc_Toc_FallthruInfo*_tmp298;struct Cyc_Dict_Dict _tmp299;int _tmp29A;int*_tmp29B;struct Cyc_Toc_StructInfo _tmp29C;int*_tmp29D;struct Cyc_Toc_Env*_tmp295=e;_tmp296=_tmp295->break_lab;_tmp297=_tmp295->continue_lab;_tmp298=_tmp295->fallthru_info;_tmp299=_tmp295->varmap;_tmp29A=_tmp295->toplevel;_tmp29B=_tmp295->in_lhs;_tmp29C=_tmp295->struct_info;_tmp29D=_tmp295->in_sizeof;{
struct Cyc_Toc_Env*_tmpB6E;return(_tmpB6E=_region_malloc(r,sizeof(*_tmpB6E)),((_tmpB6E->break_lab=(struct _dyneither_ptr**)_tmp296,((_tmpB6E->continue_lab=(struct _dyneither_ptr**)_tmp297,((_tmpB6E->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp298,((_tmpB6E->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp299),((_tmpB6E->toplevel=(int)_tmp29A,((_tmpB6E->in_lhs=(int*)_tmp29B,((_tmpB6E->struct_info=(struct Cyc_Toc_StructInfo)_tmp29C,((_tmpB6E->in_sizeof=(int*)_tmp29D,((_tmpB6E->rgn=(struct _RegionHandle*)r,_tmpB6E)))))))))))))))))));};}
# 1251
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp2A0;struct _dyneither_ptr**_tmp2A1;struct Cyc_Toc_FallthruInfo*_tmp2A2;struct Cyc_Dict_Dict _tmp2A3;int _tmp2A4;int*_tmp2A5;struct Cyc_Toc_StructInfo _tmp2A6;int*_tmp2A7;struct Cyc_Toc_Env*_tmp29F=e;_tmp2A0=_tmp29F->break_lab;_tmp2A1=_tmp29F->continue_lab;_tmp2A2=_tmp29F->fallthru_info;_tmp2A3=_tmp29F->varmap;_tmp2A4=_tmp29F->toplevel;_tmp2A5=_tmp29F->in_lhs;_tmp2A6=_tmp29F->struct_info;_tmp2A7=_tmp29F->in_sizeof;{
struct Cyc_Toc_Env*_tmpB6F;return(_tmpB6F=_region_malloc(r,sizeof(*_tmpB6F)),((_tmpB6F->break_lab=(struct _dyneither_ptr**)_tmp2A0,((_tmpB6F->continue_lab=(struct _dyneither_ptr**)_tmp2A1,((_tmpB6F->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2A2,((_tmpB6F->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2A3),((_tmpB6F->toplevel=(int)0,((_tmpB6F->in_lhs=(int*)_tmp2A5,((_tmpB6F->struct_info=(struct Cyc_Toc_StructInfo)_tmp2A6,((_tmpB6F->in_sizeof=(int*)_tmp2A7,((_tmpB6F->rgn=(struct _RegionHandle*)r,_tmpB6F)))))))))))))))))));};}
# 1255
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp2AA;struct _dyneither_ptr**_tmp2AB;struct Cyc_Toc_FallthruInfo*_tmp2AC;struct Cyc_Dict_Dict _tmp2AD;int _tmp2AE;int*_tmp2AF;struct Cyc_Toc_StructInfo _tmp2B0;int*_tmp2B1;struct Cyc_Toc_Env*_tmp2A9=e;_tmp2AA=_tmp2A9->break_lab;_tmp2AB=_tmp2A9->continue_lab;_tmp2AC=_tmp2A9->fallthru_info;_tmp2AD=_tmp2A9->varmap;_tmp2AE=_tmp2A9->toplevel;_tmp2AF=_tmp2A9->in_lhs;_tmp2B0=_tmp2A9->struct_info;_tmp2B1=_tmp2A9->in_sizeof;{
struct Cyc_Toc_Env*_tmpB70;return(_tmpB70=_region_malloc(r,sizeof(*_tmpB70)),((_tmpB70->break_lab=(struct _dyneither_ptr**)_tmp2AA,((_tmpB70->continue_lab=(struct _dyneither_ptr**)_tmp2AB,((_tmpB70->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2AC,((_tmpB70->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2AD),((_tmpB70->toplevel=(int)1,((_tmpB70->in_lhs=(int*)_tmp2AF,((_tmpB70->struct_info=(struct Cyc_Toc_StructInfo)_tmp2B0,((_tmpB70->in_sizeof=(int*)_tmp2B1,((_tmpB70->rgn=(struct _RegionHandle*)r,_tmpB70)))))))))))))))))));};}
# 1259
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
int*_tmp2B4;struct Cyc_Toc_Env*_tmp2B3=e;_tmp2B4=_tmp2B3->in_lhs;
*_tmp2B4=b;}
# 1263
static int Cyc_Toc_set_in_sizeof(struct Cyc_Toc_Env*e,int b){
int*_tmp2B6;struct Cyc_Toc_Env*_tmp2B5=e;_tmp2B6=_tmp2B5->in_sizeof;{
int _tmp2B7=*_tmp2B6;
*_tmp2B6=b;
return _tmp2B7;};}
# 1269
static int Cyc_Toc_in_sizeof(struct Cyc_Toc_Env*e){
int*_tmp2B9;struct Cyc_Toc_Env*_tmp2B8=e;_tmp2B9=_tmp2B8->in_sizeof;
return*_tmp2B9;}
# 1274
static struct Cyc_Toc_Env*Cyc_Toc_set_lhs_exp(struct _RegionHandle*r,struct Cyc_Absyn_Exp*exp,struct Cyc_Toc_Env*e){
# 1276
struct _dyneither_ptr**_tmp2BB;struct _dyneither_ptr**_tmp2BC;struct Cyc_Toc_FallthruInfo*_tmp2BD;struct Cyc_Dict_Dict _tmp2BE;int _tmp2BF;int*_tmp2C0;struct Cyc_Absyn_Exp**_tmp2C1;int*_tmp2C2;struct Cyc_Toc_Env*_tmp2BA=e;_tmp2BB=_tmp2BA->break_lab;_tmp2BC=_tmp2BA->continue_lab;_tmp2BD=_tmp2BA->fallthru_info;_tmp2BE=_tmp2BA->varmap;_tmp2BF=_tmp2BA->toplevel;_tmp2C0=_tmp2BA->in_lhs;_tmp2C1=(_tmp2BA->struct_info).varsizeexp;_tmp2C2=_tmp2BA->in_sizeof;{
struct Cyc_Toc_Env*_tmpB71;return(_tmpB71=_region_malloc(r,sizeof(*_tmpB71)),((_tmpB71->break_lab=(struct _dyneither_ptr**)_tmp2BB,((_tmpB71->continue_lab=(struct _dyneither_ptr**)_tmp2BC,((_tmpB71->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2BD,((_tmpB71->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2BE),((_tmpB71->toplevel=(int)_tmp2BF,((_tmpB71->in_lhs=(int*)_tmp2C0,((_tmpB71->struct_info=(struct Cyc_Toc_StructInfo)(((_tmpB71->struct_info).varsizeexp=_tmp2C1,(((_tmpB71->struct_info).lhs_exp=exp,_tmpB71->struct_info)))),((_tmpB71->in_sizeof=(int*)_tmp2C2,((_tmpB71->rgn=(struct _RegionHandle*)r,_tmpB71)))))))))))))))))));};}
# 1282
static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple1*x,struct Cyc_Absyn_Exp*y){
{union Cyc_Absyn_Nmspace _tmp2C4=(*x).f1;union Cyc_Absyn_Nmspace _tmp2C5=_tmp2C4;_LL10E: if((_tmp2C5.Rel_n).tag != 1)goto _LL110;_LL10F: {
# 1285
const char*_tmpB75;void*_tmpB74[1];struct Cyc_String_pa_PrintArg_struct _tmpB73;(_tmpB73.tag=0,((_tmpB73.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(x)),((_tmpB74[0]=& _tmpB73,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB75="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(_tmpB75,sizeof(char),30))),_tag_dyneither(_tmpB74,sizeof(void*),1)))))));}_LL110:;_LL111:
 goto _LL10D;_LL10D:;}{
# 1288
struct _dyneither_ptr**_tmp2CA;struct _dyneither_ptr**_tmp2CB;struct Cyc_Toc_FallthruInfo*_tmp2CC;struct Cyc_Dict_Dict _tmp2CD;int _tmp2CE;int*_tmp2CF;struct Cyc_Toc_StructInfo _tmp2D0;int*_tmp2D1;struct Cyc_Toc_Env*_tmp2C9=e;_tmp2CA=_tmp2C9->break_lab;_tmp2CB=_tmp2C9->continue_lab;_tmp2CC=_tmp2C9->fallthru_info;_tmp2CD=_tmp2C9->varmap;_tmp2CE=_tmp2C9->toplevel;_tmp2CF=_tmp2C9->in_lhs;_tmp2D0=_tmp2C9->struct_info;_tmp2D1=_tmp2C9->in_sizeof;{
struct Cyc_Dict_Dict _tmp2D2=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2CD),x,y);
struct Cyc_Toc_Env*_tmpB76;return(_tmpB76=_region_malloc(r,sizeof(*_tmpB76)),((_tmpB76->break_lab=(struct _dyneither_ptr**)_tmp2CA,((_tmpB76->continue_lab=(struct _dyneither_ptr**)_tmp2CB,((_tmpB76->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2CC,((_tmpB76->varmap=(struct Cyc_Dict_Dict)_tmp2D2,((_tmpB76->toplevel=(int)_tmp2CE,((_tmpB76->in_lhs=(int*)_tmp2CF,((_tmpB76->struct_info=(struct Cyc_Toc_StructInfo)_tmp2D0,((_tmpB76->in_sizeof=(int*)_tmp2D1,((_tmpB76->rgn=(struct _RegionHandle*)r,_tmpB76)))))))))))))))))));};};}
# 1295
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp2D5;struct _dyneither_ptr**_tmp2D6;struct Cyc_Toc_FallthruInfo*_tmp2D7;struct Cyc_Dict_Dict _tmp2D8;int _tmp2D9;int*_tmp2DA;struct Cyc_Toc_StructInfo _tmp2DB;int*_tmp2DC;struct Cyc_Toc_Env*_tmp2D4=e;_tmp2D5=_tmp2D4->break_lab;_tmp2D6=_tmp2D4->continue_lab;_tmp2D7=_tmp2D4->fallthru_info;_tmp2D8=_tmp2D4->varmap;_tmp2D9=_tmp2D4->toplevel;_tmp2DA=_tmp2D4->in_lhs;_tmp2DB=_tmp2D4->struct_info;_tmp2DC=_tmp2D4->in_sizeof;{
struct Cyc_Toc_Env*_tmpB77;return(_tmpB77=_region_malloc(r,sizeof(*_tmpB77)),((_tmpB77->break_lab=(struct _dyneither_ptr**)0,((_tmpB77->continue_lab=(struct _dyneither_ptr**)0,((_tmpB77->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2D7,((_tmpB77->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2D8),((_tmpB77->toplevel=(int)_tmp2D9,((_tmpB77->in_lhs=(int*)_tmp2DA,((_tmpB77->struct_info=(struct Cyc_Toc_StructInfo)_tmp2DB,((_tmpB77->in_sizeof=(int*)_tmp2DC,((_tmpB77->rgn=(struct _RegionHandle*)r,_tmpB77)))))))))))))))))));};}
# 1301
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*next_case_env){
# 1306
struct Cyc_List_List*fallthru_vars=0;
for(0;fallthru_binders != 0;fallthru_binders=fallthru_binders->tl){
struct Cyc_List_List*_tmpB78;fallthru_vars=((_tmpB78=_region_malloc(r,sizeof(*_tmpB78)),((_tmpB78->hd=((struct Cyc_Absyn_Vardecl*)fallthru_binders->hd)->name,((_tmpB78->tl=fallthru_vars,_tmpB78))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fallthru_vars);{
struct _dyneither_ptr**_tmp2E0;struct _dyneither_ptr**_tmp2E1;struct Cyc_Toc_FallthruInfo*_tmp2E2;struct Cyc_Dict_Dict _tmp2E3;int _tmp2E4;int*_tmp2E5;struct Cyc_Toc_StructInfo _tmp2E6;int*_tmp2E7;struct Cyc_Toc_Env*_tmp2DF=e;_tmp2E0=_tmp2DF->break_lab;_tmp2E1=_tmp2DF->continue_lab;_tmp2E2=_tmp2DF->fallthru_info;_tmp2E3=_tmp2DF->varmap;_tmp2E4=_tmp2DF->toplevel;_tmp2E5=_tmp2DF->in_lhs;_tmp2E6=_tmp2DF->struct_info;_tmp2E7=_tmp2DF->in_sizeof;{
struct Cyc_Dict_Dict _tmp2E9;struct Cyc_Toc_Env*_tmp2E8=next_case_env;_tmp2E9=_tmp2E8->varmap;{
struct Cyc_Toc_FallthruInfo*_tmpB79;struct Cyc_Toc_FallthruInfo*fi=
(_tmpB79=_region_malloc(r,sizeof(*_tmpB79)),((_tmpB79->label=fallthru_l,((_tmpB79->binders=fallthru_vars,((_tmpB79->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2E9),_tmpB79)))))));
struct _dyneither_ptr**_tmpB7C;struct Cyc_Toc_Env*_tmpB7B;return(_tmpB7B=_region_malloc(r,sizeof(*_tmpB7B)),((_tmpB7B->break_lab=(struct _dyneither_ptr**)((_tmpB7C=_region_malloc(r,sizeof(*_tmpB7C)),((_tmpB7C[0]=break_l,_tmpB7C)))),((_tmpB7B->continue_lab=(struct _dyneither_ptr**)_tmp2E1,((_tmpB7B->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi,((_tmpB7B->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2E3),((_tmpB7B->toplevel=(int)_tmp2E4,((_tmpB7B->in_lhs=(int*)_tmp2E5,((_tmpB7B->struct_info=(struct Cyc_Toc_StructInfo)_tmp2E6,((_tmpB7B->in_sizeof=(int*)_tmp2E7,((_tmpB7B->rgn=(struct _RegionHandle*)r,_tmpB7B)))))))))))))))))));};};};}
# 1318
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1321
struct _dyneither_ptr**_tmp2EE;struct _dyneither_ptr**_tmp2EF;struct Cyc_Toc_FallthruInfo*_tmp2F0;struct Cyc_Dict_Dict _tmp2F1;int _tmp2F2;int*_tmp2F3;struct Cyc_Toc_StructInfo _tmp2F4;int*_tmp2F5;struct Cyc_Toc_Env*_tmp2ED=e;_tmp2EE=_tmp2ED->break_lab;_tmp2EF=_tmp2ED->continue_lab;_tmp2F0=_tmp2ED->fallthru_info;_tmp2F1=_tmp2ED->varmap;_tmp2F2=_tmp2ED->toplevel;_tmp2F3=_tmp2ED->in_lhs;_tmp2F4=_tmp2ED->struct_info;_tmp2F5=_tmp2ED->in_sizeof;{
struct _dyneither_ptr**_tmpB7F;struct Cyc_Toc_Env*_tmpB7E;return(_tmpB7E=_region_malloc(r,sizeof(*_tmpB7E)),((_tmpB7E->break_lab=(struct _dyneither_ptr**)((_tmpB7F=_region_malloc(r,sizeof(*_tmpB7F)),((_tmpB7F[0]=break_l,_tmpB7F)))),((_tmpB7E->continue_lab=(struct _dyneither_ptr**)_tmp2EF,((_tmpB7E->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpB7E->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2F1),((_tmpB7E->toplevel=(int)_tmp2F2,((_tmpB7E->in_lhs=(int*)_tmp2F3,((_tmpB7E->struct_info=(struct Cyc_Toc_StructInfo)_tmp2F4,((_tmpB7E->in_sizeof=(int*)_tmp2F5,((_tmpB7E->rgn=(struct _RegionHandle*)r,_tmpB7E)))))))))))))))))));};}
# 1328
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1331
struct _dyneither_ptr**_tmp2F9;struct _dyneither_ptr**_tmp2FA;struct Cyc_Toc_FallthruInfo*_tmp2FB;struct Cyc_Dict_Dict _tmp2FC;int _tmp2FD;int*_tmp2FE;struct Cyc_Toc_StructInfo _tmp2FF;int*_tmp300;struct Cyc_Toc_Env*_tmp2F8=e;_tmp2F9=_tmp2F8->break_lab;_tmp2FA=_tmp2F8->continue_lab;_tmp2FB=_tmp2F8->fallthru_info;_tmp2FC=_tmp2F8->varmap;_tmp2FD=_tmp2F8->toplevel;_tmp2FE=_tmp2F8->in_lhs;_tmp2FF=_tmp2F8->struct_info;_tmp300=_tmp2F8->in_sizeof;{
struct Cyc_Toc_FallthruInfo*_tmpB82;struct Cyc_Toc_Env*_tmpB81;return(_tmpB81=_region_malloc(r,sizeof(*_tmpB81)),((_tmpB81->break_lab=(struct _dyneither_ptr**)0,((_tmpB81->continue_lab=(struct _dyneither_ptr**)_tmp2FA,((_tmpB81->fallthru_info=(struct Cyc_Toc_FallthruInfo*)((_tmpB82=_region_malloc(r,sizeof(*_tmpB82)),((_tmpB82->label=next_l,((_tmpB82->binders=0,((_tmpB82->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),_tmpB82)))))))),((_tmpB81->varmap=(struct Cyc_Dict_Dict)
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2FC),((_tmpB81->toplevel=(int)_tmp2FD,((_tmpB81->in_lhs=(int*)_tmp2FE,((_tmpB81->struct_info=(struct Cyc_Toc_StructInfo)_tmp2FF,((_tmpB81->in_sizeof=(int*)_tmp300,((_tmpB81->rgn=(struct _RegionHandle*)r,_tmpB81)))))))))))))))))));};}
# 1346 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1349
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
# 1351
void*_tmp303=e->annot;void*_tmp304=_tmp303;_LL113: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp305=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp304;if(_tmp305->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL115;}_LL114:
# 1354
 return Cyc_Toc_is_nullable((void*)_check_null(e->topt));_LL115: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp306=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp304;if(_tmp306->tag != Cyc_CfFlowInfo_NotZero)goto _LL117;}_LL116:
# 1357
 return 0;_LL117: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp307=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp304;if(_tmp307->tag != Cyc_CfFlowInfo_IsZero)goto _LL119;}_LL118:
# 1359
{const char*_tmpB85;void*_tmpB84;(_tmpB84=0,Cyc_Tcutil_terr(e->loc,((_tmpB85="dereference of NULL pointer",_tag_dyneither(_tmpB85,sizeof(char),28))),_tag_dyneither(_tmpB84,sizeof(void*),0)));}
return 0;_LL119: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp308=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp304;if(_tmp308->tag != Cyc_Absyn_EmptyAnnot)goto _LL11B;}_LL11A:
# 1365
 return 1;_LL11B:;_LL11C: {
const char*_tmpB88;void*_tmpB87;(_tmpB87=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB88="need_null_check",_tag_dyneither(_tmpB88,sizeof(char),16))),_tag_dyneither(_tmpB87,sizeof(void*),0)));}_LL112:;}
# 1370
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp30D=e->annot;void*_tmp30E=_tmp30D;struct Cyc_List_List*_tmp310;struct Cyc_List_List*_tmp312;_LL11E: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp30F=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp30E;if(_tmp30F->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL120;else{_tmp310=_tmp30F->f1;}}_LL11F:
 return _tmp310;_LL120: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp311=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp30E;if(_tmp311->tag != Cyc_CfFlowInfo_NotZero)goto _LL122;else{_tmp312=_tmp311->f1;}}_LL121:
 return _tmp312;_LL122: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp313=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp30E;if(_tmp313->tag != Cyc_CfFlowInfo_IsZero)goto _LL124;}_LL123:
# 1375
{const char*_tmpB8B;void*_tmpB8A;(_tmpB8A=0,Cyc_Tcutil_terr(e->loc,((_tmpB8B="dereference of NULL pointer",_tag_dyneither(_tmpB8B,sizeof(char),28))),_tag_dyneither(_tmpB8A,sizeof(void*),0)));}
return 0;_LL124: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp314=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp30E;if(_tmp314->tag != Cyc_Absyn_EmptyAnnot)goto _LL126;}_LL125:
 return 0;_LL126:;_LL127: {
const char*_tmpB8E;void*_tmpB8D;(_tmpB8D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB8E="get_relns",_tag_dyneither(_tmpB8E,sizeof(char),10))),_tag_dyneither(_tmpB8D,sizeof(void*),0)));}_LL11D:;}static char _tmp31E[8]="*bogus*";
# 1385
static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
# 1395 "toc.cyc"
union Cyc_Relations_RelnOp rop_i=Cyc_Relations_RConst(0);
int valid_rop_i=Cyc_Relations_exp2relnop(i,& rop_i);
# 1398
struct Cyc_Absyn_Vardecl*x;
{void*_tmp319=a->r;void*_tmp31A=_tmp319;void*_tmp31C;_LL129: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp31B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp31A;if(_tmp31B->tag != 1)goto _LL12B;else{_tmp31C=(void*)_tmp31B->f2;}}_LL12A: {
# 1401
struct Cyc_Absyn_Vardecl*_tmp31D=Cyc_Tcutil_nonesc_vardecl(_tmp31C);
if(_tmp31D == 0)goto _LL12C;
x=_tmp31D;
goto _LL128;}_LL12B:;_LL12C: {
# 1407
static struct _dyneither_ptr bogus_string={_tmp31E,_tmp31E,_tmp31E + 8};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
# 1410
static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};
# 1416
x=& bogus_vardecl;
x->type=a_typ;}_LL128:;}{
# 1419
void*_tmp31F=a_typ;
union Cyc_Relations_RelnOp rop_a=Cyc_Relations_RNumelts(x);
# 1424
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound(_tmp31F);
if(bound != 0){
union Cyc_Relations_RelnOp rop_bound=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(bound,& rop_bound))
relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_bound,Cyc_Relations_Rlte,rop_a,relns);}
# 1433
if(valid_rop_i){
struct Cyc_List_List*_tmp320=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlte,rop_i,relns);
# 1441
if(!Cyc_Relations_consistent_relations(_tmp320))return 1;}
# 1444
inner_loop: {
void*_tmp321=i->r;void*_tmp322=_tmp321;void*_tmp324;struct Cyc_Absyn_Exp*_tmp325;struct Cyc_Absyn_Exp*_tmp327;struct Cyc_Absyn_Exp*_tmp328;_LL12E: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp323=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp322;if(_tmp323->tag != 13)goto _LL130;else{_tmp324=(void*)_tmp323->f1;_tmp325=_tmp323->f2;}}_LL12F:
 i=_tmp325;goto inner_loop;_LL130: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp326=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp322;if(_tmp326->tag != 2)goto _LL132;else{if(_tmp326->f1 != Cyc_Absyn_Mod)goto _LL132;if(_tmp326->f2 == 0)goto _LL132;_tmp327=(struct Cyc_Absyn_Exp*)(_tmp326->f2)->hd;if((_tmp326->f2)->tl == 0)goto _LL132;_tmp328=(struct Cyc_Absyn_Exp*)((_tmp326->f2)->tl)->hd;}}_LL131: {
# 1450
union Cyc_Relations_RelnOp rop_z=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmp328,& rop_z)){
# 1453
struct Cyc_List_List*_tmp329=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlt,rop_z,relns);
# 1455
return !Cyc_Relations_consistent_relations(_tmp329);}
# 1457
goto _LL12D;}_LL132:;_LL133:
 goto _LL12D;_LL12D:;}
# 1460
return 0;};}
# 1463
static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0){const char*_tmpB91;void*_tmpB90;(_tmpB90=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB91="Missing type in primop ",_tag_dyneither(_tmpB91,sizeof(char),24))),_tag_dyneither(_tmpB90,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));}
# 1467
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0){const char*_tmpB94;void*_tmpB93;(_tmpB93=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB94="Missing type in primop ",_tag_dyneither(_tmpB94,sizeof(char),24))),_tag_dyneither(_tmpB93,sizeof(void*),0)));}
return(void*)_check_null(e->topt);}
# 1471
static struct _tuple11*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
struct _tuple11*_tmpB95;return(_tmpB95=_cycalloc(sizeof(*_tmpB95)),((_tmpB95->f1=Cyc_Toc_mt_tq,((_tmpB95->f2=Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmpB95)))));}
# 1474
static struct _tuple19*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple19*_tmpB96;return(_tmpB96=_cycalloc(sizeof(*_tmpB96)),((_tmpB96->f1=0,((_tmpB96->f2=e,_tmpB96)))));};}
# 1481
static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic,int do_declare){
# 1484
struct Cyc_Absyn_Exp*eo;
void*t;
if(pointer){
struct Cyc_Absyn_Exp*_tmp330=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(x,0),0);
if(*(nv->struct_info).varsizeexp != 0){
_tmp330=Cyc_Absyn_add_exp(_tmp330,(struct Cyc_Absyn_Exp*)_check_null(*(nv->struct_info).varsizeexp),0);
*(nv->struct_info).varsizeexp=0;}
# 1492
t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);
if(rgnopt == 0  || Cyc_Absyn_no_regions)
eo=is_atomic?Cyc_Toc_malloc_atomic(_tmp330): Cyc_Toc_malloc_ptr(_tmp330);else{
# 1496
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);
eo=Cyc_Toc_rmalloc_exp(r,_tmp330);}}else{
# 1501
t=struct_typ;
eo=0;}
# 1504
if(do_declare){
void*_tmp331=x->r;void*_tmp332=_tmp331;struct _tuple1*_tmp334;_LL135: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp333=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp332;if(_tmp333->tag != 1)goto _LL137;else{_tmp334=_tmp333->f1;}}_LL136:
# 1507
 return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp334,t,eo,s,0),0);_LL137:;_LL138: {
# 1509
const char*_tmpB99;void*_tmpB98;(_tmpB98=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB99="bogus x to make_struct",_tag_dyneither(_tmpB99,sizeof(char),23))),_tag_dyneither(_tmpB98,sizeof(void*),0)));}_LL134:;}else{
# 1513
if(eo != 0)
return Cyc_Absyn_stmt_exp(Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(x,eo,0),0),s,0),0);else{
# 1516
return Cyc_Absyn_stmt_exp(s,0);}}}
# 1520
static int Cyc_Toc_abstract_aggr(void*typ){
void*_tmp337=Cyc_Tcutil_compress(typ);void*_tmp338=_tmp337;union Cyc_Absyn_AggrInfoU _tmp33A;struct Cyc_List_List*_tmp33B;struct Cyc_List_List*_tmp33D;_LL13A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp339=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp338;if(_tmp339->tag != 11)goto _LL13C;else{_tmp33A=(_tmp339->f1).aggr_info;_tmp33B=(_tmp339->f1).targs;}}_LL13B: {
# 1523
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(_tmp33A);
if(ad->impl == 0)return 0;{
struct Cyc_List_List*aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
_tmp33D=aggrfields;goto _LL13D;};}_LL13C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp33C=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp338;if(_tmp33C->tag != 12)goto _LL13E;else{_tmp33D=_tmp33C->f2;}}_LL13D:
# 1528
 if(_tmp33D == 0)return 0;
return 1;_LL13E:;_LL13F:
# 1534
 return 0;_LL139:;}
# 1538
static void*Cyc_Toc_array_elt_type(void*t){
void*_tmp33E=Cyc_Tcutil_compress(t);void*_tmp33F=_tmp33E;void*_tmp341;_LL141: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp340=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp33F;if(_tmp340->tag != 8)goto _LL143;else{_tmp341=(_tmp340->f1).elt_type;}}_LL142:
 return _tmp341;_LL143:;_LL144: {
const char*_tmpB9D;void*_tmpB9C[1];struct Cyc_String_pa_PrintArg_struct _tmpB9B;(_tmpB9B.tag=0,((_tmpB9B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpB9C[0]=& _tmpB9B,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB9D="array_elt_type called on %s",_tag_dyneither(_tmpB9D,sizeof(char),28))),_tag_dyneither(_tmpB9C,sizeof(void*),1)))))));}_LL140:;}
# 1545
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated);
# 1551
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s);
# 1556
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,void*array_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){
# 1561
void*orig_elt_type=Cyc_Toc_array_elt_type(array_type);
int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;
{struct Cyc_List_List*_tmp345=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;_tmp345 != 0;_tmp345=_tmp345->tl){
# 1565
struct _tuple19*_tmp346=(struct _tuple19*)_tmp345->hd;struct Cyc_List_List*_tmp348;struct Cyc_Absyn_Exp*_tmp349;struct _tuple19*_tmp347=_tmp346;_tmp348=_tmp347->f1;_tmp349=_tmp347->f2;{
# 1569
struct Cyc_Absyn_Exp*e_index;
if(_tmp348 == 0)
e_index=Cyc_Absyn_signed_int_exp(count --,0);else{
# 1573
if(_tmp348->tl != 0){const char*_tmpBA0;void*_tmpB9F;(_tmpB9F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBA0="multiple designators in array",_tag_dyneither(_tmpBA0,sizeof(char),30))),_tag_dyneither(_tmpB9F,sizeof(void*),0)));}{
void*_tmp34C=(void*)_tmp348->hd;
void*_tmp34D=_tmp34C;struct Cyc_Absyn_Exp*_tmp34F;_LL146: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp34E=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp34D;if(_tmp34E->tag != 0)goto _LL148;else{_tmp34F=_tmp34E->f1;}}_LL147:
# 1577
 Cyc_Toc_exp_to_c(nv,_tmp34F);
e_index=_tmp34F;
goto _LL145;_LL148: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp350=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp34D;if(_tmp350->tag != 1)goto _LL145;}_LL149: {
const char*_tmpBA3;void*_tmpBA2;(_tmpBA2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBA3="field name designators in array",_tag_dyneither(_tmpBA3,sizeof(char),32))),_tag_dyneither(_tmpBA2,sizeof(void*),0)));}_LL145:;};}{
# 1583
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);
void*_tmp353=_tmp349->r;void*_tmp354=_tmp353;struct Cyc_List_List*_tmp356;struct Cyc_Absyn_Vardecl*_tmp358;struct Cyc_Absyn_Exp*_tmp359;struct Cyc_Absyn_Exp*_tmp35A;int _tmp35B;void*_tmp35D;struct Cyc_List_List*_tmp35E;_LL14B: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp355=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp354;if(_tmp355->tag != 25)goto _LL14D;else{_tmp356=_tmp355->f1;}}_LL14C:
# 1586
 s=Cyc_Toc_init_array(nv,orig_elt_type,lval,_tmp356,s);
goto _LL14A;_LL14D: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp357=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp354;if(_tmp357->tag != 26)goto _LL14F;else{_tmp358=_tmp357->f1;_tmp359=_tmp357->f2;_tmp35A=_tmp357->f3;_tmp35B=_tmp357->f4;}}_LL14E:
# 1589
 s=Cyc_Toc_init_comprehension(nv,orig_elt_type,lval,_tmp358,_tmp359,_tmp35A,_tmp35B,s,0);
# 1591
goto _LL14A;_LL14F: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp35C=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp354;if(_tmp35C->tag != 29)goto _LL151;else{_tmp35D=(void*)_tmp35C->f1;_tmp35E=_tmp35C->f2;}}_LL150:
# 1593
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp35D,_tmp35E,s);
goto _LL14A;_LL151: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp35F=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp354;if(_tmp35F->tag != 27)goto _LL153;}_LL152:
# 1596
 goto _LL14A;_LL153:;_LL154:
# 1598
 Cyc_Toc_exp_to_c(nv,_tmp349);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(orig_elt_type),_tmp349),0),s,0);
goto _LL14A;_LL14A:;};};}}
# 1604
return s;}
# 1609
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated){
# 1614
struct _tuple1*_tmp360=vd->name;
void*expected_elt_type;
{void*_tmp361=Cyc_Tcutil_compress(comprehension_type);void*_tmp362=_tmp361;void*_tmp364;void*_tmp366;_LL156: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp363=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp362;if(_tmp363->tag != 8)goto _LL158;else{_tmp364=(_tmp363->f1).elt_type;}}_LL157:
# 1618
 expected_elt_type=_tmp364;goto _LL155;_LL158: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp365=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp362;if(_tmp365->tag != 5)goto _LL15A;else{_tmp366=(_tmp365->f1).elt_typ;}}_LL159:
# 1620
 expected_elt_type=_tmp366;goto _LL155;_LL15A:;_LL15B: {
const char*_tmpBA6;void*_tmpBA5;(_tmpBA5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA6="init_comprehension passed a bad type!",_tag_dyneither(_tmpBA6,sizeof(char),38))),_tag_dyneither(_tmpBA5,sizeof(void*),0)));}_LL155:;}{
# 1623
void*_tmp369=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));
if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);
# 1627
{void*_tmp36A=e2->r;void*_tmp36B=_tmp36A;_LL15D: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp36C=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp36B;if(_tmp36C->tag != 27)goto _LL15F;}_LL15E:
 return Cyc_Absyn_skip_stmt(0);_LL15F:;_LL160:
 goto _LL15C;_LL15C:;}{
# 1631
struct _RegionHandle _tmp36D=_new_region("r2");struct _RegionHandle*r2=& _tmp36D;_push_region(r2);
{struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpBA9;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpBA8;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(r2,nv,_tmp360,Cyc_Absyn_varb_exp(_tmp360,(void*)((_tmpBA8=_cycalloc(sizeof(*_tmpBA8)),((_tmpBA8[0]=((_tmpBA9.tag=4,((_tmpBA9.f1=vd,_tmpBA9)))),_tmpBA8)))),0));
struct _tuple1*max=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp360,0),Cyc_Absyn_signed_int_exp(0,0),0);
struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp360,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp360,0),0);
# 1639
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp360,0),0);
struct Cyc_Absyn_Stmt*body;
# 1643
{void*_tmp36E=e2->r;void*_tmp36F=_tmp36E;struct Cyc_List_List*_tmp371;struct Cyc_Absyn_Vardecl*_tmp373;struct Cyc_Absyn_Exp*_tmp374;struct Cyc_Absyn_Exp*_tmp375;int _tmp376;void*_tmp378;struct Cyc_List_List*_tmp379;_LL162: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp370=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp36F;if(_tmp370->tag != 25)goto _LL164;else{_tmp371=_tmp370->f1;}}_LL163:
# 1645
 body=Cyc_Toc_init_array(nv2,expected_elt_type,lval,_tmp371,Cyc_Toc_skip_stmt_dl());
goto _LL161;_LL164: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp372=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp36F;if(_tmp372->tag != 26)goto _LL166;else{_tmp373=_tmp372->f1;_tmp374=_tmp372->f2;_tmp375=_tmp372->f3;_tmp376=_tmp372->f4;}}_LL165:
# 1648
 body=Cyc_Toc_init_comprehension(nv2,expected_elt_type,lval,_tmp373,_tmp374,_tmp375,_tmp376,Cyc_Toc_skip_stmt_dl(),0);
goto _LL161;_LL166: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp377=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp36F;if(_tmp377->tag != 29)goto _LL168;else{_tmp378=(void*)_tmp377->f1;_tmp379=_tmp377->f2;}}_LL167:
# 1651
 body=Cyc_Toc_init_anon_struct(nv,lval,_tmp378,_tmp379,Cyc_Toc_skip_stmt_dl());
goto _LL161;_LL168:;_LL169:
# 1654
 Cyc_Toc_exp_to_c(nv2,e2);
body=Cyc_Absyn_assign_stmt(lval,Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(expected_elt_type),e2),0);
goto _LL161;_LL161:;}{
# 1658
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0);
# 1660
if(zero_term){
# 1665
struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(_tmp369,Cyc_Absyn_uint_exp(0,0)),0);
s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}{
# 1670
struct Cyc_Absyn_Stmt*_tmp37A=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,e1,
Cyc_Absyn_declare_stmt(_tmp360,Cyc_Absyn_uint_typ,0,s2,0),0),s,0);_npop_handler(0);return _tmp37A;};};}
# 1632
;_pop_region(r2);};};}
# 1676
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s){
# 1679
{struct Cyc_List_List*_tmp37D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles);for(0;_tmp37D != 0;_tmp37D=_tmp37D->tl){
struct _tuple19*_tmp37E=(struct _tuple19*)_tmp37D->hd;struct Cyc_List_List*_tmp380;struct Cyc_Absyn_Exp*_tmp381;struct _tuple19*_tmp37F=_tmp37E;_tmp380=_tmp37F->f1;_tmp381=_tmp37F->f2;
if(_tmp380 == 0){
const char*_tmpBAC;void*_tmpBAB;(_tmpBAB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBAC="empty designator list",_tag_dyneither(_tmpBAC,sizeof(char),22))),_tag_dyneither(_tmpBAB,sizeof(void*),0)));}
if(_tmp380->tl != 0){
const char*_tmpBAF;void*_tmpBAE;(_tmpBAE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBAF="too many designators in anonymous struct",_tag_dyneither(_tmpBAF,sizeof(char),41))),_tag_dyneither(_tmpBAE,sizeof(void*),0)));}{
void*_tmp386=(void*)_tmp380->hd;void*_tmp387=_tmp386;struct _dyneither_ptr*_tmp389;_LL16B: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp388=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp387;if(_tmp388->tag != 1)goto _LL16D;else{_tmp389=_tmp388->f1;}}_LL16C: {
# 1687
struct Cyc_Absyn_Exp*lval=Cyc_Toc_member_exp(lhs,_tmp389,0);
{void*_tmp38A=_tmp381->r;void*_tmp38B=_tmp38A;struct Cyc_List_List*_tmp38D;struct Cyc_Absyn_Vardecl*_tmp38F;struct Cyc_Absyn_Exp*_tmp390;struct Cyc_Absyn_Exp*_tmp391;int _tmp392;void*_tmp394;struct Cyc_List_List*_tmp395;_LL170: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp38C=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp38B;if(_tmp38C->tag != 25)goto _LL172;else{_tmp38D=_tmp38C->f1;}}_LL171:
# 1690
 s=Cyc_Toc_init_array(nv,(void*)_check_null(_tmp381->topt),lval,_tmp38D,s);goto _LL16F;_LL172: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp38E=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp38B;if(_tmp38E->tag != 26)goto _LL174;else{_tmp38F=_tmp38E->f1;_tmp390=_tmp38E->f2;_tmp391=_tmp38E->f3;_tmp392=_tmp38E->f4;}}_LL173:
# 1692
 s=Cyc_Toc_init_comprehension(nv,(void*)_check_null(_tmp381->topt),lval,_tmp38F,_tmp390,_tmp391,_tmp392,s,0);
goto _LL16F;_LL174: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp393=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp38B;if(_tmp393->tag != 29)goto _LL176;else{_tmp394=(void*)_tmp393->f1;_tmp395=_tmp393->f2;}}_LL175:
# 1695
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp394,_tmp395,s);goto _LL16F;_LL176: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp396=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp38B;if(_tmp396->tag != 27)goto _LL178;}_LL177:
# 1697
 goto _LL16F;_LL178:;_LL179:
# 1699
 Cyc_Toc_exp_to_c(nv,_tmp381);
# 1701
if(Cyc_Toc_is_poly_field(struct_type,_tmp389))
_tmp381=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp381);
# 1704
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp381,0),0),s,0);
goto _LL16F;_LL16F:;}
# 1707
goto _LL16A;}_LL16D:;_LL16E: {
const char*_tmpBB2;void*_tmpBB1;(_tmpBB1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBB2="array designator in struct",_tag_dyneither(_tmpBB2,sizeof(char),27))),_tag_dyneither(_tmpBB1,sizeof(void*),0)));}_LL16A:;};}}
# 1711
return s;}
# 1716
static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){
# 1720
struct _RegionHandle _tmp399=_new_region("r");struct _RegionHandle*r=& _tmp399;_push_region(r);
{struct Cyc_List_List*_tmp39A=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);
void*_tmp39B=Cyc_Toc_add_tuple_type(_tmp39A);
# 1724
struct _tuple1*_tmp39C=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp39D=Cyc_Absyn_var_exp(_tmp39C,0);
struct Cyc_Absyn_Stmt*_tmp39E=Cyc_Absyn_exp_stmt(_tmp39D,0);
# 1728
struct Cyc_Absyn_Exp*(*_tmp39F)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
# 1730
int is_atomic=1;
struct Cyc_List_List*_tmp3A0=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);
{int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp3A0);for(0;_tmp3A0 != 0;(_tmp3A0=_tmp3A0->tl,-- i)){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp3A0->hd;
struct Cyc_Absyn_Exp*lval=_tmp39F(_tmp39D,Cyc_Absyn_fieldname(i),0);
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(e->topt));{
void*_tmp3A1=e->r;void*_tmp3A2=_tmp3A1;struct Cyc_List_List*_tmp3A4;struct Cyc_Absyn_Vardecl*_tmp3A6;struct Cyc_Absyn_Exp*_tmp3A7;struct Cyc_Absyn_Exp*_tmp3A8;int _tmp3A9;_LL17B: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp3A3=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3A2;if(_tmp3A3->tag != 25)goto _LL17D;else{_tmp3A4=_tmp3A3->f1;}}_LL17C:
# 1738
 _tmp39E=Cyc_Toc_init_array(nv,(void*)_check_null(e->topt),lval,_tmp3A4,_tmp39E);
goto _LL17A;_LL17D: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp3A5=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3A2;if(_tmp3A5->tag != 26)goto _LL17F;else{_tmp3A6=_tmp3A5->f1;_tmp3A7=_tmp3A5->f2;_tmp3A8=_tmp3A5->f3;_tmp3A9=_tmp3A5->f4;}}_LL17E:
# 1741
 _tmp39E=Cyc_Toc_init_comprehension(nv,(void*)_check_null(e->topt),lval,_tmp3A6,_tmp3A7,_tmp3A8,_tmp3A9,_tmp39E,0);
# 1743
goto _LL17A;_LL17F: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp3AA=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp3A2;if(_tmp3AA->tag != 27)goto _LL181;}_LL180:
# 1745
 goto _LL17A;_LL181:;_LL182:
# 1748
 Cyc_Toc_exp_to_c(nv,e);
_tmp39E=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp39F(_tmp39D,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp39E,0);
# 1751
goto _LL17A;_LL17A:;};}}{
# 1754
struct Cyc_Absyn_Exp*_tmp3AB=Cyc_Toc_make_struct(nv,Cyc_Absyn_var_exp(_tmp39C,0),_tmp39B,_tmp39E,pointer,rgnopt,is_atomic,1);_npop_handler(0);return _tmp3AB;};}
# 1721
;_pop_region(r);}
# 1758
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp3AC=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp3AC != 0;_tmp3AC=_tmp3AC->tl){
struct Cyc_Absyn_Aggrfield*_tmp3AD=(struct Cyc_Absyn_Aggrfield*)_tmp3AC->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp3AD->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}{
# 1765
struct Cyc_String_pa_PrintArg_struct _tmpBBA;void*_tmpBB9[1];const char*_tmpBB8;void*_tmpBB7;(_tmpBB7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((struct _dyneither_ptr)((_tmpBBA.tag=0,((_tmpBBA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpBB9[0]=& _tmpBBA,Cyc_aprintf(((_tmpBB8="get_member_offset %s failed",_tag_dyneither(_tmpBB8,sizeof(char),28))),_tag_dyneither(_tmpBB9,sizeof(void*),1)))))))),_tag_dyneither(_tmpBB7,sizeof(void*),0)));};}
# 1768
static int Cyc_Toc_init_variable_array(void**typ){
{void*_tmp3B2=Cyc_Tcutil_compress(*typ);void*_tmp3B3=_tmp3B2;void*_tmp3B5;struct Cyc_Absyn_Tqual _tmp3B6;struct Cyc_Absyn_Exp*_tmp3B7;union Cyc_Absyn_Constraint*_tmp3B8;unsigned int _tmp3B9;_LL184: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp3B4=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B3;if(_tmp3B4->tag != 8)goto _LL186;else{_tmp3B5=(_tmp3B4->f1).elt_type;_tmp3B6=(_tmp3B4->f1).tq;_tmp3B7=(_tmp3B4->f1).num_elts;_tmp3B8=(_tmp3B4->f1).zero_term;_tmp3B9=(_tmp3B4->f1).zt_loc;}}_LL185:
# 1773
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp3B7))){
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpBC0;struct Cyc_Absyn_ArrayInfo _tmpBBF;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpBBE;*typ=(void*)((_tmpBBE=_cycalloc(sizeof(*_tmpBBE)),((_tmpBBE[0]=((_tmpBC0.tag=8,((_tmpBC0.f1=((_tmpBBF.elt_type=_tmp3B5,((_tmpBBF.tq=_tmp3B6,((_tmpBBF.num_elts=Cyc_Absyn_uint_exp(0,0),((_tmpBBF.zero_term=_tmp3B8,((_tmpBBF.zt_loc=_tmp3B9,_tmpBBF)))))))))),_tmpBC0)))),_tmpBBE))));}
return 1;}
# 1777
goto _LL183;_LL186:;_LL187:
 goto _LL183;_LL183:;}
# 1780
return 0;}
# 1783
static int Cyc_Toc_is_array_type(void*t){
void*_tmp3BD=Cyc_Tcutil_compress(t);void*_tmp3BE=_tmp3BD;_LL189: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp3BF=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3BE;if(_tmp3BF->tag != 8)goto _LL18B;}_LL18A:
 return 1;_LL18B:;_LL18C:
 return 0;_LL188:;}
# 1790
struct Cyc_Absyn_Aggrdecl*Cyc_Toc_update_aggr_type(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*fn,void*new_field_type){
# 1792
struct Cyc_List_List*new_fields=0;
struct Cyc_List_List*fields;
for(fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;fields != 0;fields=fields->tl){
# 1796
if(!Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,fn)){
struct Cyc_Absyn_Aggrfield*_tmp3C0=(struct Cyc_Absyn_Aggrfield*)fields->hd;
struct Cyc_List_List*_tmp3C1=_tmp3C0->attributes;
void*_tmp3C2=_tmp3C0->type;
if((!Cyc_Toc_is_array_type(_tmp3C2),Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp3C2)))){
struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpBC6;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpBC5;struct Cyc_List_List*_tmpBC4;_tmp3C1=((_tmpBC4=_cycalloc(sizeof(*_tmpBC4)),((_tmpBC4->hd=(void*)((_tmpBC6=_cycalloc(sizeof(*_tmpBC6)),((_tmpBC6[0]=((_tmpBC5.tag=6,((_tmpBC5.f1=0,_tmpBC5)))),_tmpBC6)))),((_tmpBC4->tl=_tmp3C1,_tmpBC4))))));}{
struct Cyc_Absyn_Aggrfield*_tmpBC7;struct Cyc_Absyn_Aggrfield*_tmp3C6=(_tmpBC7=_cycalloc(sizeof(*_tmpBC7)),((_tmpBC7->name=_tmp3C0->name,((_tmpBC7->tq=Cyc_Toc_mt_tq,((_tmpBC7->type=new_field_type,((_tmpBC7->width=_tmp3C0->width,((_tmpBC7->attributes=_tmp3C1,((_tmpBC7->requires_clause=0,_tmpBC7)))))))))))));
# 1808
struct Cyc_List_List*_tmpBC8;new_fields=((_tmpBC8=_cycalloc(sizeof(*_tmpBC8)),((_tmpBC8->hd=_tmp3C6,((_tmpBC8->tl=new_fields,_tmpBC8))))));};}else{
# 1810
struct Cyc_List_List*_tmpBC9;new_fields=((_tmpBC9=_cycalloc(sizeof(*_tmpBC9)),((_tmpBC9->hd=(struct Cyc_Absyn_Aggrfield*)fields->hd,((_tmpBC9->tl=new_fields,_tmpBC9))))));}}{
# 1812
struct Cyc_List_List*_tmp3CA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);
struct Cyc_Int_pa_PrintArg_struct _tmpBD1;void*_tmpBD0[1];const char*_tmpBCF;struct _dyneither_ptr*_tmpBCE;struct _dyneither_ptr*name=
(_tmpBCE=_cycalloc(sizeof(*_tmpBCE)),((_tmpBCE[0]=(struct _dyneither_ptr)((_tmpBD1.tag=1,((_tmpBD1.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpBD0[0]=& _tmpBD1,Cyc_aprintf(((_tmpBCF="_genStruct%d",_tag_dyneither(_tmpBCF,sizeof(char),13))),_tag_dyneither(_tmpBD0,sizeof(void*),1)))))))),_tmpBCE)));
struct _tuple1*_tmpBD2;struct _tuple1*qv=(_tmpBD2=_cycalloc(sizeof(*_tmpBD2)),((_tmpBD2->f1=Cyc_Absyn_Abs_n(0,1),((_tmpBD2->f2=name,_tmpBD2)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpBD3;struct Cyc_Absyn_AggrdeclImpl*_tmp3CB=(_tmpBD3=_cycalloc(sizeof(*_tmpBD3)),((_tmpBD3->exist_vars=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,((_tmpBD3->rgn_po=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po,((_tmpBD3->fields=_tmp3CA,((_tmpBD3->tagged=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged,_tmpBD3)))))))));
# 1820
struct Cyc_Absyn_Aggrdecl*_tmpBD4;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpBD4=_cycalloc(sizeof(*_tmpBD4)),((_tmpBD4->kind=ad->kind,((_tmpBD4->sc=ad->sc,((_tmpBD4->name=qv,((_tmpBD4->tvs=ad->tvs,((_tmpBD4->impl=_tmp3CB,((_tmpBD4->expected_mem_kind=0,((_tmpBD4->attributes=ad->attributes,_tmpBD4)))))))))))))));
# 1827
Cyc_Toc_aggrdecl_to_c(new_ad,1);
return new_ad;};}
# 1831
static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*nv,void*struct_type,struct Cyc_List_List*exist_types,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*dles,struct _tuple1*tdn){
# 1837
int do_declare=0;
struct Cyc_Absyn_Exp*xexp;
if((nv->struct_info).lhs_exp == 0){
struct _tuple1*_tmp3D3=Cyc_Toc_temp_var();
xexp=Cyc_Absyn_var_exp(_tmp3D3,0);
do_declare=1;}else{
# 1845
xexp=(struct Cyc_Absyn_Exp*)_check_null((nv->struct_info).lhs_exp);
nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);}{
# 1848
struct Cyc_Absyn_Stmt*_tmp3D4=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(xexp),0);
# 1852
struct Cyc_Absyn_Exp**varexp;
{void*_tmp3D5=_tmp3D4->r;void*_tmp3D6=_tmp3D5;struct Cyc_Absyn_Exp**_tmp3D8;_LL18E: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp3D7=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3D6;if(_tmp3D7->tag != 1)goto _LL190;else{_tmp3D8=(struct Cyc_Absyn_Exp**)& _tmp3D7->f1;}}_LL18F:
 varexp=_tmp3D8;goto _LL18D;_LL190:;_LL191: {
const char*_tmpBD7;void*_tmpBD6;(_tmpBD6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBD7="stmt not an expression!",_tag_dyneither(_tmpBD7,sizeof(char),24))),_tag_dyneither(_tmpBD6,sizeof(void*),0)));}_LL18D:;}{
# 1858
struct Cyc_Absyn_Exp*(*_tmp3DB)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
void*aggr_typ=Cyc_Toc_typ_to_c(struct_type);
void*orig_typ=aggr_typ;
# 1862
int is_atomic=1;
struct Cyc_List_List*forall_types;
struct Cyc_Absyn_Aggrdecl*ad;
struct Cyc_List_List*aggrfields;struct Cyc_List_List*orig_aggrfields;
struct _dyneither_ptr**index=0;
{void*_tmp3DC=Cyc_Tcutil_compress(struct_type);void*_tmp3DD=_tmp3DC;union Cyc_Absyn_AggrInfoU _tmp3DF;struct Cyc_List_List*_tmp3E0;_LL193: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3DE=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3DD;if(_tmp3DE->tag != 11)goto _LL195;else{_tmp3DF=(_tmp3DE->f1).aggr_info;_tmp3E0=(_tmp3DE->f1).targs;}}_LL194:
# 1869
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp3DF);
orig_aggrfields=(aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
forall_types=_tmp3E0;
goto _LL192;_LL195:;_LL196: {
const char*_tmpBDA;void*_tmpBD9;(_tmpBD9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBDA="init_struct: bad struct type",_tag_dyneither(_tmpBDA,sizeof(char),29))),_tag_dyneither(_tmpBD9,sizeof(void*),0)));}_LL192:;}
# 1876
if(exist_types != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields != 0){
struct Cyc_List_List*_tmp3E3=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
for(0;_tmp3E3->tl != 0;_tmp3E3=_tmp3E3->tl){;}{
struct Cyc_Absyn_Aggrfield*_tmp3E4=(struct Cyc_Absyn_Aggrfield*)_tmp3E3->hd;
struct _RegionHandle _tmp3E5=_new_region("temp");struct _RegionHandle*temp=& _tmp3E5;_push_region(temp);
{struct Cyc_List_List*_tmp3E6=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,forall_types);
# 1885
if(Cyc_Toc_is_abstract_type(Cyc_Tcutil_rsubstitute(temp,_tmp3E6,_tmp3E4->type))){
struct Cyc_List_List*_tmp3E7=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,exist_types);
struct Cyc_List_List*_tmp3E8=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(temp,_tmp3E6,_tmp3E7);
# 1889
struct Cyc_List_List*new_fields=0;
for(_tmp3E3=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;_tmp3E3 != 0;_tmp3E3=_tmp3E3->tl){
struct Cyc_Absyn_Aggrfield*_tmp3E9=(struct Cyc_Absyn_Aggrfield*)_tmp3E3->hd;
struct Cyc_Absyn_Aggrfield*_tmpBDB;struct Cyc_Absyn_Aggrfield*_tmp3EA=(_tmpBDB=_cycalloc(sizeof(*_tmpBDB)),((_tmpBDB->name=_tmp3E9->name,((_tmpBDB->tq=Cyc_Toc_mt_tq,((_tmpBDB->type=
# 1894
Cyc_Tcutil_rsubstitute(temp,_tmp3E8,_tmp3E9->type),((_tmpBDB->width=_tmp3E9->width,((_tmpBDB->attributes=_tmp3E9->attributes,((_tmpBDB->requires_clause=0,_tmpBDB)))))))))))));
# 1902
if(_tmp3E3->tl == 0){
if(Cyc_Toc_init_variable_array(& _tmp3EA->type)){
struct _dyneither_ptr**_tmpBDC;index=((_tmpBDC=_cycalloc(sizeof(*_tmpBDC)),((_tmpBDC[0]=_tmp3EA->name,_tmpBDC))));}}{
# 1909
struct Cyc_List_List*_tmpBDD;new_fields=((_tmpBDD=_cycalloc(sizeof(*_tmpBDD)),((_tmpBDD->hd=_tmp3EA,((_tmpBDD->tl=new_fields,_tmpBDD))))));};}
# 1911
exist_types=0;
aggrfields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);{
struct Cyc_Int_pa_PrintArg_struct _tmpBE5;void*_tmpBE4[1];const char*_tmpBE3;struct _dyneither_ptr*_tmpBE2;struct _dyneither_ptr*name=
(_tmpBE2=_cycalloc(sizeof(*_tmpBE2)),((_tmpBE2[0]=(struct _dyneither_ptr)((_tmpBE5.tag=1,((_tmpBE5.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpBE4[0]=& _tmpBE5,Cyc_aprintf(((_tmpBE3="_genStruct%d",_tag_dyneither(_tmpBE3,sizeof(char),13))),_tag_dyneither(_tmpBE4,sizeof(void*),1)))))))),_tmpBE2)));
struct _tuple1*_tmpBE6;struct _tuple1*qv=(_tmpBE6=_cycalloc(sizeof(*_tmpBE6)),((_tmpBE6->f1=Cyc_Absyn_Abs_n(0,1),((_tmpBE6->f2=name,_tmpBE6)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpBE7;struct Cyc_Absyn_AggrdeclImpl*_tmp3EE=(_tmpBE7=_cycalloc(sizeof(*_tmpBE7)),((_tmpBE7->exist_vars=0,((_tmpBE7->rgn_po=0,((_tmpBE7->fields=aggrfields,((_tmpBE7->tagged=0,_tmpBE7)))))))));
# 1920
struct Cyc_Absyn_Aggrdecl*_tmpBE8;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpBE8=_cycalloc(sizeof(*_tmpBE8)),((_tmpBE8->kind=Cyc_Absyn_StructA,((_tmpBE8->sc=Cyc_Absyn_Public,((_tmpBE8->name=qv,((_tmpBE8->tvs=0,((_tmpBE8->expected_mem_kind=0,((_tmpBE8->impl=_tmp3EE,((_tmpBE8->attributes=0,_tmpBE8)))))))))))))));
# 1927
Cyc_Toc_aggrdecl_to_c(new_ad,1);
ad=new_ad;
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpBF2;struct Cyc_Absyn_Aggrdecl**_tmpBF1;struct Cyc_Absyn_AggrInfo _tmpBF0;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpBEF;struct_type=(void*)((_tmpBEF=_cycalloc(sizeof(*_tmpBEF)),((_tmpBEF[0]=((_tmpBF2.tag=11,((_tmpBF2.f1=((_tmpBF0.aggr_info=Cyc_Absyn_KnownAggr(((_tmpBF1=_cycalloc(sizeof(*_tmpBF1)),((_tmpBF1[0]=new_ad,_tmpBF1))))),((_tmpBF0.targs=0,_tmpBF0)))),_tmpBF2)))),_tmpBEF))));}
# 1931
aggr_typ=Cyc_Toc_typ_to_c(struct_type);};}}
# 1881
;_pop_region(temp);};}{
# 1935
int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct _RegionHandle _tmp3FA=_new_region("r");struct _RegionHandle*r=& _tmp3FA;_push_region(r);
{struct Cyc_List_List*_tmp3FB=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp3FB != 0;_tmp3FB=_tmp3FB->tl){
struct _tuple19*_tmp3FC=(struct _tuple19*)_tmp3FB->hd;struct Cyc_List_List*_tmp3FE;struct Cyc_Absyn_Exp*_tmp3FF;struct _tuple19*_tmp3FD=_tmp3FC;_tmp3FE=_tmp3FD->f1;_tmp3FF=_tmp3FD->f2;
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(_tmp3FF->topt));
if(_tmp3FE == 0){
const char*_tmpBF5;void*_tmpBF4;(_tmpBF4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBF5="empty designator list",_tag_dyneither(_tmpBF5,sizeof(char),22))),_tag_dyneither(_tmpBF4,sizeof(void*),0)));}
if(_tmp3FE->tl != 0){
# 1945
struct _tuple1*_tmp402=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp403=Cyc_Absyn_var_exp(_tmp402,0);
for(0;_tmp3FE != 0;_tmp3FE=_tmp3FE->tl){
void*_tmp404=(void*)_tmp3FE->hd;void*_tmp405=_tmp404;struct _dyneither_ptr*_tmp407;_LL198: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp406=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp405;if(_tmp406->tag != 1)goto _LL19A;else{_tmp407=_tmp406->f1;}}_LL199:
# 1951
 if(Cyc_Toc_is_poly_field(struct_type,_tmp407))
_tmp403=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp403);
_tmp3D4=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3DB(xexp,_tmp407,0),_tmp403,0),0),_tmp3D4,0);
# 1955
goto _LL197;_LL19A:;_LL19B: {
const char*_tmpBF8;void*_tmpBF7;(_tmpBF7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBF8="array designator in struct",_tag_dyneither(_tmpBF8,sizeof(char),27))),_tag_dyneither(_tmpBF7,sizeof(void*),0)));}_LL197:;}
# 1959
Cyc_Toc_exp_to_c(nv,_tmp3FF);
_tmp3D4=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp403,_tmp3FF,0),0),_tmp3D4,0);}else{
# 1962
void*_tmp40A=(void*)_tmp3FE->hd;void*_tmp40B=_tmp40A;struct _dyneither_ptr*_tmp40D;_LL19D: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp40C=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp40B;if(_tmp40C->tag != 1)goto _LL19F;else{_tmp40D=_tmp40C->f1;}}_LL19E: {
# 1964
struct Cyc_Absyn_Exp*lval=_tmp3DB(xexp,_tmp40D,0);
if(is_tagged_union){
int i=Cyc_Toc_get_member_offset(ad,_tmp40D);
struct Cyc_Absyn_Exp*f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct Cyc_Absyn_Exp*lhs=Cyc_Toc_member_exp(lval,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,f_tag_exp,0);
_tmp3D4=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp3D4,0);
lval=Cyc_Toc_member_exp(lval,Cyc_Toc_val_sp,0);}{
# 1973
int e1_translated=0;
{void*_tmp40E=_tmp3FF->r;void*_tmp40F=_tmp40E;struct Cyc_List_List*_tmp411;struct Cyc_Absyn_Vardecl*_tmp413;struct Cyc_Absyn_Exp*_tmp414;struct Cyc_Absyn_Exp*_tmp415;int _tmp416;struct Cyc_Absyn_Exp*_tmp418;void*_tmp419;void*_tmp41B;struct Cyc_List_List*_tmp41C;_LL1A2: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp410=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp40F;if(_tmp410->tag != 25)goto _LL1A4;else{_tmp411=_tmp410->f1;}}_LL1A3:
# 1976
 _tmp3D4=Cyc_Toc_init_array(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(aggrfields,_tmp40D)))->type,lval,_tmp411,_tmp3D4);
# 1978
goto _LL1A1;_LL1A4: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp412=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp40F;if(_tmp412->tag != 26)goto _LL1A6;else{_tmp413=_tmp412->f1;_tmp414=_tmp412->f2;_tmp415=_tmp412->f3;_tmp416=_tmp412->f4;}}_LL1A5:
# 1980
 _tmp3D4=Cyc_Toc_init_comprehension(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(do_declare?aggrfields: orig_aggrfields,_tmp40D)))->type,lval,_tmp413,_tmp414,_tmp415,_tmp416,_tmp3D4,0);
# 1984
e1_translated=1;
_tmp418=_tmp414;_tmp419=(void*)_check_null(_tmp415->topt);goto _LL1A7;_LL1A6: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp417=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp40F;if(_tmp417->tag != 27)goto _LL1A8;else{_tmp418=_tmp417->f1;_tmp419=(void*)_tmp417->f2;}}_LL1A7:
# 1989
 if(index != 0  && !Cyc_strptrcmp(*index,_tmp40D)){
if(!e1_translated)Cyc_Toc_exp_to_c(nv,_tmp418);{
void*_tmp41D=Cyc_Toc_typ_to_c(_tmp419);
struct _tuple6 _tmpBFC;union Cyc_Absyn_Cnst _tmpBFB;struct Cyc_Absyn_Exp*_tmpBF9[2];*(nv->struct_info).varsizeexp=
Cyc_Absyn_add_exp(Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpBF9[1]=Cyc_Absyn_sizeoftyp_exp(_tmp41D,0),((_tmpBF9[0]=_tmp418,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBF9,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),
Cyc_Absyn_const_exp((((_tmpBFB.Int_c).val=((_tmpBFC.f1=Cyc_Absyn_Unsigned,((_tmpBFC.f2=(int)sizeof(double),_tmpBFC)))),(((_tmpBFB.Int_c).tag=5,_tmpBFB)))),0),0);};}
# 1997
goto _LL1A1;_LL1A8: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp41A=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp40F;if(_tmp41A->tag != 29)goto _LL1AA;else{_tmp41B=(void*)_tmp41A->f1;_tmp41C=_tmp41A->f2;}}_LL1A9:
# 1999
 _tmp3D4=Cyc_Toc_init_anon_struct(nv,lval,_tmp41B,_tmp41C,_tmp3D4);goto _LL1A1;_LL1AA:;_LL1AB: {
# 2001
void*old_e_typ=(void*)_check_null(_tmp3FF->topt);
int was_ptr_type=Cyc_Toc_is_pointer_or_boxed_tvar(old_e_typ);
Cyc_Toc_exp_to_c(Cyc_Toc_set_lhs_exp(nv->rgn,lval,nv),_tmp3FF);
{void*_tmp421=old_e_typ;_LL1AD: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp422=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp421;if(_tmp422->tag != 11)goto _LL1AF;}_LL1AE:
# 2008
 if(old_e_typ != _tmp3FF->topt  && !Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(old_e_typ),(void*)_check_null(_tmp3FF->topt))){
# 2012
ad=Cyc_Toc_update_aggr_type(ad,_tmp40D,(void*)_check_null(_tmp3FF->topt));
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpC06;struct Cyc_Absyn_Aggrdecl**_tmpC05;struct Cyc_Absyn_AggrInfo _tmpC04;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC03;struct_type=(void*)(
(_tmpC03=_cycalloc(sizeof(*_tmpC03)),((_tmpC03[0]=((_tmpC06.tag=11,((_tmpC06.f1=((_tmpC04.aggr_info=Cyc_Absyn_KnownAggr(((_tmpC05=_cycalloc(sizeof(*_tmpC05)),((_tmpC05[0]=ad,_tmpC05))))),((_tmpC04.targs=0,_tmpC04)))),_tmpC06)))),_tmpC03))));}
# 2016
aggr_typ=Cyc_Toc_typ_to_c(struct_type);}
# 2018
goto _LL1AC;_LL1AF:;_LL1B0:
 goto _LL1AC;_LL1AC:;}{
# 2021
struct Cyc_Absyn_Aggrfield*_tmp427=Cyc_Absyn_lookup_field(aggrfields,_tmp40D);
# 2023
if(Cyc_Toc_is_poly_field(struct_type,_tmp40D) && !was_ptr_type)
_tmp3FF=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3FF);
# 2026
if(exist_types != 0)
_tmp3FF=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp427))->type),_tmp3FF);
# 2029
_tmp3D4=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp3FF,0),0),_tmp3D4,0);
goto _LL1A1;};}_LL1A1:;}
# 2032
goto _LL19C;};}_LL19F:;_LL1A0: {
const char*_tmpC09;void*_tmpC08;(_tmpC08=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC09="array designator in struct",_tag_dyneither(_tmpC09,sizeof(char),27))),_tag_dyneither(_tmpC08,sizeof(void*),0)));}_LL19C:;}}}
# 2039
if(aggr_typ != orig_typ  && pointer){
void*cast_type=Cyc_Absyn_cstar_typ(orig_typ,Cyc_Toc_mt_tq);
*varexp=Cyc_Toc_cast_it(cast_type,*varexp);}{
# 2044
struct Cyc_Absyn_Exp*_tmp42A=Cyc_Toc_make_struct(nv,xexp,aggr_typ,_tmp3D4,pointer,rgnopt,is_atomic,do_declare);
# 2046
_tmp42A->topt=struct_type;{
struct Cyc_Absyn_Exp*_tmp42B=_tmp42A;_npop_handler(0);return _tmp42B;};};
# 1937
;_pop_region(r);};};};}struct _tuple27{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 2052
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple27*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
# 2055
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0);}
# 2058
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmpC0C;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmpC0B;return Cyc_Absyn_new_exp((void*)((_tmpC0B=_cycalloc(sizeof(*_tmpC0B)),((_tmpC0B[0]=((_tmpC0C.tag=4,((_tmpC0C.f1=e1,((_tmpC0C.f2=incr,_tmpC0C)))))),_tmpC0B)))),0);}
# 2062
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 2071
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 2073
void*_tmp42E=e1->r;void*_tmp42F=_tmp42E;struct Cyc_Absyn_Stmt*_tmp431;void*_tmp433;struct Cyc_Absyn_Exp*_tmp434;struct Cyc_Absyn_Exp*_tmp436;struct _dyneither_ptr*_tmp437;int _tmp438;int _tmp439;_LL1B2: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp430=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp42F;if(_tmp430->tag != 36)goto _LL1B4;else{_tmp431=_tmp430->f1;}}_LL1B3:
 Cyc_Toc_lvalue_assign_stmt(_tmp431,fs,f,f_env);goto _LL1B1;_LL1B4: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp432=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp42F;if(_tmp432->tag != 13)goto _LL1B6;else{_tmp433=(void*)_tmp432->f1;_tmp434=_tmp432->f2;}}_LL1B5:
 Cyc_Toc_lvalue_assign(_tmp434,fs,f,f_env);goto _LL1B1;_LL1B6: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp435=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp42F;if(_tmp435->tag != 20)goto _LL1B8;else{_tmp436=_tmp435->f1;_tmp437=_tmp435->f2;_tmp438=_tmp435->f3;_tmp439=_tmp435->f4;}}_LL1B7:
# 2078
 e1->r=_tmp436->r;
{struct Cyc_List_List*_tmpC0D;Cyc_Toc_lvalue_assign(e1,((_tmpC0D=_cycalloc(sizeof(*_tmpC0D)),((_tmpC0D->hd=_tmp437,((_tmpC0D->tl=fs,_tmpC0D)))))),f,f_env);}
goto _LL1B1;_LL1B8:;_LL1B9: {
# 2086
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 2088
for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;
goto _LL1B1;}_LL1B1:;}
# 2094
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 2096
void*_tmp43B=s->r;void*_tmp43C=_tmp43B;struct Cyc_Absyn_Exp*_tmp43E;struct Cyc_Absyn_Decl*_tmp440;struct Cyc_Absyn_Stmt*_tmp441;struct Cyc_Absyn_Stmt*_tmp443;_LL1BB: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp43D=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp43C;if(_tmp43D->tag != 1)goto _LL1BD;else{_tmp43E=_tmp43D->f1;}}_LL1BC:
 Cyc_Toc_lvalue_assign(_tmp43E,fs,f,f_env);goto _LL1BA;_LL1BD: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp43F=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp43C;if(_tmp43F->tag != 12)goto _LL1BF;else{_tmp440=_tmp43F->f1;_tmp441=_tmp43F->f2;}}_LL1BE:
# 2099
 Cyc_Toc_lvalue_assign_stmt(_tmp441,fs,f,f_env);goto _LL1BA;_LL1BF: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp442=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp43C;if(_tmp442->tag != 2)goto _LL1C1;else{_tmp443=_tmp442->f2;}}_LL1C0:
 Cyc_Toc_lvalue_assign_stmt(_tmp443,fs,f,f_env);goto _LL1BA;_LL1C1:;_LL1C2: {
const char*_tmpC11;void*_tmpC10[1];struct Cyc_String_pa_PrintArg_struct _tmpC0F;(_tmpC0F.tag=0,((_tmpC0F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpC10[0]=& _tmpC0F,Cyc_Toc_toc_impos(((_tmpC11="lvalue_assign_stmt: %s",_tag_dyneither(_tmpC11,sizeof(char),23))),_tag_dyneither(_tmpC10,sizeof(void*),1)))))));}_LL1BA:;}
# 2105
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 2109
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp447=e->r;void*_tmp448=_tmp447;void**_tmp44A;struct Cyc_Absyn_Exp**_tmp44B;struct Cyc_Absyn_Exp*_tmp44D;struct Cyc_Absyn_Stmt*_tmp44F;_LL1C4: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp449=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp448;if(_tmp449->tag != 13)goto _LL1C6;else{_tmp44A=(void**)((void**)& _tmp449->f1);_tmp44B=(struct Cyc_Absyn_Exp**)& _tmp449->f2;}}_LL1C5:
# 2112
*_tmp44B=Cyc_Toc_push_address_exp(*_tmp44B);
*_tmp44A=Cyc_Absyn_cstar_typ(*_tmp44A,Cyc_Toc_mt_tq);
return e;_LL1C6: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp44C=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp448;if(_tmp44C->tag != 19)goto _LL1C8;else{_tmp44D=_tmp44C->f1;}}_LL1C7:
# 2116
 return _tmp44D;_LL1C8: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp44E=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp448;if(_tmp44E->tag != 36)goto _LL1CA;else{_tmp44F=_tmp44E->f1;}}_LL1C9:
# 2120
 Cyc_Toc_push_address_stmt(_tmp44F);
return e;_LL1CA:;_LL1CB:
# 2123
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{
const char*_tmpC15;void*_tmpC14[1];struct Cyc_String_pa_PrintArg_struct _tmpC13;(_tmpC13.tag=0,((_tmpC13.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC14[0]=& _tmpC13,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC15="can't take & of exp %s",_tag_dyneither(_tmpC15,sizeof(char),23))),_tag_dyneither(_tmpC14,sizeof(void*),1)))))));};_LL1C3:;}
# 2128
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp453=s->r;void*_tmp454=_tmp453;struct Cyc_Absyn_Stmt*_tmp456;struct Cyc_Absyn_Stmt*_tmp458;struct Cyc_Absyn_Exp**_tmp45A;_LL1CD: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp455=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp454;if(_tmp455->tag != 2)goto _LL1CF;else{_tmp456=_tmp455->f2;}}_LL1CE:
 _tmp458=_tmp456;goto _LL1D0;_LL1CF: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp457=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp454;if(_tmp457->tag != 12)goto _LL1D1;else{_tmp458=_tmp457->f2;}}_LL1D0:
 Cyc_Toc_push_address_stmt(_tmp458);goto _LL1CC;_LL1D1: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp459=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp454;if(_tmp459->tag != 1)goto _LL1D3;else{_tmp45A=(struct Cyc_Absyn_Exp**)& _tmp459->f1;}}_LL1D2:
*_tmp45A=Cyc_Toc_push_address_exp(*_tmp45A);goto _LL1CC;_LL1D3:;_LL1D4: {
# 2134
const char*_tmpC19;void*_tmpC18[1];struct Cyc_String_pa_PrintArg_struct _tmpC17;(_tmpC17.tag=0,((_tmpC17.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpC18[0]=& _tmpC17,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC19="can't take & of stmt %s",_tag_dyneither(_tmpC19,sizeof(char),24))),_tag_dyneither(_tmpC18,sizeof(void*),1)))))));}_LL1CC:;}
# 2139
static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 2141
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 2143
if(x == 0)return 0;
{struct Cyc_List_List*_tmpC1A;result=((_tmpC1A=_region_malloc(r2,sizeof(*_tmpC1A)),((_tmpC1A->hd=f(x->hd,env),((_tmpC1A->tl=0,_tmpC1A))))));}
prev=result;
for(x=x->tl;x != 0;x=x->tl){
{struct Cyc_List_List*_tmpC1B;((struct Cyc_List_List*)_check_null(prev))->tl=((_tmpC1B=_region_malloc(r2,sizeof(*_tmpC1B)),((_tmpC1B->hd=f(x->hd,env),((_tmpC1B->tl=0,_tmpC1B))))));}
prev=prev->tl;}
# 2150
return result;}
# 2152
static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,x);}
# 2156
static struct _tuple19*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){
struct _tuple19*_tmpC1C;return(_tmpC1C=_cycalloc(sizeof(*_tmpC1C)),((_tmpC1C->f1=0,((_tmpC1C->f2=e,_tmpC1C)))));}
# 2160
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_tmp461=Cyc_Tcutil_compress(t);void*_tmp462=_tmp461;struct Cyc_Absyn_PtrInfo _tmp464;_LL1D6: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp463=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp462;if(_tmp463->tag != 5)goto _LL1D8;else{_tmp464=_tmp463->f1;}}_LL1D7:
 return _tmp464;_LL1D8:;_LL1D9: {
const char*_tmpC1F;void*_tmpC1E;(_tmpC1E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC1F="get_ptr_typ: not a pointer!",_tag_dyneither(_tmpC1F,sizeof(char),28))),_tag_dyneither(_tmpC1E,sizeof(void*),0)));}_LL1D5:;}
# 2170
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp467=Cyc_Tcutil_compress(t);void*_tmp468=_tmp467;enum Cyc_Absyn_Sign _tmp46B;enum Cyc_Absyn_Sign _tmp46D;enum Cyc_Absyn_Sign _tmp471;enum Cyc_Absyn_Sign _tmp473;enum Cyc_Absyn_Sign _tmp475;int _tmp479;_LL1DB: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp469=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp468;if(_tmp469->tag != 5)goto _LL1DD;}_LL1DC:
 res=Cyc_Absyn_null_exp(0);goto _LL1DA;_LL1DD: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp46A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp468;if(_tmp46A->tag != 6)goto _LL1DF;else{_tmp46B=_tmp46A->f1;if(_tmp46A->f2 != Cyc_Absyn_Char_sz)goto _LL1DF;}}_LL1DE:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp46B,'\000'),0);goto _LL1DA;_LL1DF: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp46C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp468;if(_tmp46C->tag != 6)goto _LL1E1;else{_tmp46D=_tmp46C->f1;if(_tmp46C->f2 != Cyc_Absyn_Short_sz)goto _LL1E1;}}_LL1E0:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp46D,0),0);goto _LL1DA;_LL1E1: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp46E=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp468;if(_tmp46E->tag != 13)goto _LL1E3;}_LL1E2:
 goto _LL1E4;_LL1E3: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp46F=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp468;if(_tmp46F->tag != 14)goto _LL1E5;}_LL1E4:
 _tmp471=Cyc_Absyn_Unsigned;goto _LL1E6;_LL1E5: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp470=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp468;if(_tmp470->tag != 6)goto _LL1E7;else{_tmp471=_tmp470->f1;if(_tmp470->f2 != Cyc_Absyn_Int_sz)goto _LL1E7;}}_LL1E6:
 _tmp473=_tmp471;goto _LL1E8;_LL1E7: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp472=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp468;if(_tmp472->tag != 6)goto _LL1E9;else{_tmp473=_tmp472->f1;if(_tmp472->f2 != Cyc_Absyn_Long_sz)goto _LL1E9;}}_LL1E8:
# 2180
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp473,0),0);goto _LL1DA;_LL1E9: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp474=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp468;if(_tmp474->tag != 6)goto _LL1EB;else{_tmp475=_tmp474->f1;if(_tmp474->f2 != Cyc_Absyn_LongLong_sz)goto _LL1EB;}}_LL1EA:
# 2182
 res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp475,(long long)0),0);goto _LL1DA;_LL1EB: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp476=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp468;if(_tmp476->tag != 7)goto _LL1ED;else{if(_tmp476->f1 != 0)goto _LL1ED;}}_LL1EC:
{const char*_tmpC20;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpC20="0.0F",_tag_dyneither(_tmpC20,sizeof(char),5))),0),0);}goto _LL1DA;_LL1ED: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp477=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp468;if(_tmp477->tag != 7)goto _LL1EF;else{if(_tmp477->f1 != 1)goto _LL1EF;}}_LL1EE:
{const char*_tmpC21;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpC21="0.0",_tag_dyneither(_tmpC21,sizeof(char),4))),1),0);}goto _LL1DA;_LL1EF: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp478=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp468;if(_tmp478->tag != 7)goto _LL1F1;else{_tmp479=_tmp478->f1;}}_LL1F0:
{const char*_tmpC22;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpC22="0.0L",_tag_dyneither(_tmpC22,sizeof(char),5))),_tmp479),0);}goto _LL1DA;_LL1F1:;_LL1F2: {
# 2187
const char*_tmpC26;void*_tmpC25[1];struct Cyc_String_pa_PrintArg_struct _tmpC24;(_tmpC24.tag=0,((_tmpC24.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpC25[0]=& _tmpC24,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC26="found non-zero type %s in generate_zero",_tag_dyneither(_tmpC26,sizeof(char),40))),_tag_dyneither(_tmpC25,sizeof(void*),1)))))));}_LL1DA:;}
# 2189
res->topt=t;
return res;}
# 2196
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 2209 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp480=Cyc_Toc_typ_to_c(elt_type);
void*_tmp481=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp482=Cyc_Absyn_cstar_typ(_tmp480,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmpC27;struct Cyc_Core_Opt*_tmp483=(_tmpC27=_cycalloc(sizeof(*_tmpC27)),((_tmpC27->v=_tmp482,_tmpC27)));
struct Cyc_Absyn_Exp*xinit;
{void*_tmp484=e1->r;void*_tmp485=_tmp484;struct Cyc_Absyn_Exp*_tmp487;struct Cyc_Absyn_Exp*_tmp489;struct Cyc_Absyn_Exp*_tmp48A;_LL1F4: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp486=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp485;if(_tmp486->tag != 19)goto _LL1F6;else{_tmp487=_tmp486->f1;}}_LL1F5:
# 2217
 if(!is_dyneither){
_tmp487=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp487,0,Cyc_Absyn_Other_coercion,0);
_tmp487->topt=fat_ptr_type;}
# 2221
Cyc_Toc_exp_to_c(nv,_tmp487);xinit=_tmp487;goto _LL1F3;_LL1F6: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp488=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp485;if(_tmp488->tag != 22)goto _LL1F8;else{_tmp489=_tmp488->f1;_tmp48A=_tmp488->f2;}}_LL1F7:
# 2223
 if(!is_dyneither){
_tmp489=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp489,0,Cyc_Absyn_Other_coercion,0);
_tmp489->topt=fat_ptr_type;}
# 2227
Cyc_Toc_exp_to_c(nv,_tmp489);Cyc_Toc_exp_to_c(nv,_tmp48A);
{struct Cyc_Absyn_Exp*_tmpC28[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpC28[2]=_tmp48A,((_tmpC28[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpC28[0]=_tmp489,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC28,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL1F3;_LL1F8:;_LL1F9: {
const char*_tmpC2B;void*_tmpC2A;(_tmpC2A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC2B="found bad lhs for zero-terminated pointer assignment",_tag_dyneither(_tmpC2B,sizeof(char),53))),_tag_dyneither(_tmpC2A,sizeof(void*),0)));}_LL1F3:;}{
# 2233
struct _tuple1*_tmp48E=Cyc_Toc_temp_var();
struct _RegionHandle _tmp48F=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp48F;_push_region(rgn2);
{struct Cyc_Toc_Env*_tmp490=Cyc_Toc_add_varmap(rgn2,nv,_tmp48E,Cyc_Absyn_var_exp(_tmp48E,0));
struct Cyc_Absyn_Vardecl*_tmpC2C;struct Cyc_Absyn_Vardecl*_tmp491=(_tmpC2C=_cycalloc(sizeof(*_tmpC2C)),((_tmpC2C->sc=Cyc_Absyn_Public,((_tmpC2C->name=_tmp48E,((_tmpC2C->tq=Cyc_Toc_mt_tq,((_tmpC2C->type=_tmp481,((_tmpC2C->initializer=xinit,((_tmpC2C->rgn=0,((_tmpC2C->attributes=0,((_tmpC2C->escapes=0,_tmpC2C)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpC2F;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpC2E;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp492=(_tmpC2E=_cycalloc(sizeof(*_tmpC2E)),((_tmpC2E[0]=((_tmpC2F.tag=4,((_tmpC2F.f1=_tmp491,_tmpC2F)))),_tmpC2E)));
struct Cyc_Absyn_Exp*_tmp493=Cyc_Absyn_varb_exp(_tmp48E,(void*)_tmp492,0);
_tmp493->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp494=Cyc_Absyn_deref_exp(_tmp493,0);
_tmp494->topt=elt_type;
Cyc_Toc_exp_to_c(_tmp490,_tmp494);{
struct _tuple1*_tmp495=Cyc_Toc_temp_var();
_tmp490=Cyc_Toc_add_varmap(rgn2,_tmp490,_tmp495,Cyc_Absyn_var_exp(_tmp495,0));{
struct Cyc_Absyn_Vardecl*_tmpC30;struct Cyc_Absyn_Vardecl*_tmp496=(_tmpC30=_cycalloc(sizeof(*_tmpC30)),((_tmpC30->sc=Cyc_Absyn_Public,((_tmpC30->name=_tmp495,((_tmpC30->tq=Cyc_Toc_mt_tq,((_tmpC30->type=_tmp480,((_tmpC30->initializer=_tmp494,((_tmpC30->rgn=0,((_tmpC30->attributes=0,((_tmpC30->escapes=0,_tmpC30)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpC33;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpC32;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp497=(_tmpC32=_cycalloc(sizeof(*_tmpC32)),((_tmpC32[0]=((_tmpC33.tag=4,((_tmpC33.f1=_tmp496,_tmpC33)))),_tmpC32)));
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp498=Cyc_Absyn_varb_exp(_tmp495,(void*)_tmp497,0);
_tmp498->topt=_tmp494->topt;
z_init=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp498,e2,0);
z_init->topt=_tmp498->topt;}
# 2254
Cyc_Toc_exp_to_c(_tmp490,z_init);{
struct _tuple1*_tmp499=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmpC34;struct Cyc_Absyn_Vardecl*_tmp49A=(_tmpC34=_cycalloc(sizeof(*_tmpC34)),((_tmpC34->sc=Cyc_Absyn_Public,((_tmpC34->name=_tmp499,((_tmpC34->tq=Cyc_Toc_mt_tq,((_tmpC34->type=_tmp480,((_tmpC34->initializer=z_init,((_tmpC34->rgn=0,((_tmpC34->attributes=0,((_tmpC34->escapes=0,_tmpC34)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpC37;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpC36;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp49B=(_tmpC36=_cycalloc(sizeof(*_tmpC36)),((_tmpC36[0]=((_tmpC37.tag=4,((_tmpC37.f1=_tmp49A,_tmpC37)))),_tmpC36)));
_tmp490=Cyc_Toc_add_varmap(rgn2,_tmp490,_tmp499,Cyc_Absyn_var_exp(_tmp499,0));{
# 2261
struct Cyc_Absyn_Exp*_tmp49C=Cyc_Absyn_varb_exp(_tmp495,(void*)_tmp497,0);_tmp49C->topt=_tmp494->topt;{
struct Cyc_Absyn_Exp*_tmp49D=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp49E=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp49C,_tmp49D,0);
_tmp49E->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp490,_tmp49E);{
# 2267
struct Cyc_Absyn_Exp*_tmp49F=Cyc_Absyn_varb_exp(_tmp499,(void*)_tmp49B,0);_tmp49F->topt=_tmp494->topt;{
struct Cyc_Absyn_Exp*_tmp4A0=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp4A1=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp49F,_tmp4A0,0);
_tmp4A1->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp490,_tmp4A1);{
# 2273
struct Cyc_Absyn_Exp*_tmpC38[2];struct Cyc_List_List*_tmp4A2=(_tmpC38[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpC38[0]=
# 2273
Cyc_Absyn_varb_exp(_tmp48E,(void*)_tmp492,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC38,sizeof(struct Cyc_Absyn_Exp*),2)))));
# 2275
struct Cyc_Absyn_Exp*_tmp4A3=Cyc_Absyn_uint_exp(1,0);
struct Cyc_Absyn_Exp*xsize;
xsize=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp4A2,0),_tmp4A3,0);{
# 2280
struct Cyc_Absyn_Exp*_tmp4A4=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp49E,_tmp4A1,0),0);
struct Cyc_Absyn_Stmt*_tmp4A5=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0),0);
struct Cyc_Absyn_Exp*_tmp4A6=Cyc_Toc_member_exp(Cyc_Absyn_varb_exp(_tmp48E,(void*)_tmp492,0),Cyc_Toc_curr_sp,0);
_tmp4A6=Cyc_Toc_cast_it(_tmp482,_tmp4A6);{
struct Cyc_Absyn_Exp*_tmp4A7=Cyc_Absyn_deref_exp(_tmp4A6,0);
struct Cyc_Absyn_Exp*_tmp4A8=Cyc_Absyn_assign_exp(_tmp4A7,Cyc_Absyn_var_exp(_tmp499,0),0);
struct Cyc_Absyn_Stmt*_tmp4A9=Cyc_Absyn_exp_stmt(_tmp4A8,0);
_tmp4A9=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp4A4,_tmp4A5,Cyc_Absyn_skip_stmt(0),0),_tmp4A9,0);
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpC3E;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpC3D;struct Cyc_Absyn_Decl*_tmpC3C;_tmp4A9=Cyc_Absyn_decl_stmt(((_tmpC3C=_cycalloc(sizeof(*_tmpC3C)),((_tmpC3C->r=(void*)((_tmpC3E=_cycalloc(sizeof(*_tmpC3E)),((_tmpC3E[0]=((_tmpC3D.tag=0,((_tmpC3D.f1=_tmp49A,_tmpC3D)))),_tmpC3E)))),((_tmpC3C->loc=0,_tmpC3C)))))),_tmp4A9,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpC44;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpC43;struct Cyc_Absyn_Decl*_tmpC42;_tmp4A9=Cyc_Absyn_decl_stmt(((_tmpC42=_cycalloc(sizeof(*_tmpC42)),((_tmpC42->r=(void*)((_tmpC44=_cycalloc(sizeof(*_tmpC44)),((_tmpC44[0]=((_tmpC43.tag=0,((_tmpC43.f1=_tmp496,_tmpC43)))),_tmpC44)))),((_tmpC42->loc=0,_tmpC42)))))),_tmp4A9,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpC4A;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpC49;struct Cyc_Absyn_Decl*_tmpC48;_tmp4A9=Cyc_Absyn_decl_stmt(((_tmpC48=_cycalloc(sizeof(*_tmpC48)),((_tmpC48->r=(void*)((_tmpC4A=_cycalloc(sizeof(*_tmpC4A)),((_tmpC4A[0]=((_tmpC49.tag=0,((_tmpC49.f1=_tmp491,_tmpC49)))),_tmpC4A)))),((_tmpC48->loc=0,_tmpC48)))))),_tmp4A9,0);}
e->r=Cyc_Toc_stmt_exp_r(_tmp4A9);};};};};};};};};};};};}
# 2235
;_pop_region(rgn2);};}
# 2306 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 2310
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp4BE=Cyc_Tcutil_compress(aggrtype);void*_tmp4BF=_tmp4BE;union Cyc_Absyn_AggrInfoU _tmp4C1;_LL1FB: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4C0=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4BF;if(_tmp4C0->tag != 11)goto _LL1FD;else{_tmp4C1=(_tmp4C0->f1).aggr_info;}}_LL1FC:
# 2313
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp4C1);goto _LL1FA;_LL1FD:;_LL1FE: {
struct Cyc_String_pa_PrintArg_struct _tmpC52;void*_tmpC51[1];const char*_tmpC50;void*_tmpC4F;(_tmpC4F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmpC52.tag=0,((_tmpC52.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpC51[0]=& _tmpC52,Cyc_aprintf(((_tmpC50="expecting union but found %s in check_tagged_union",_tag_dyneither(_tmpC50,sizeof(char),51))),_tag_dyneither(_tmpC51,sizeof(void*),1)))))))),_tag_dyneither(_tmpC4F,sizeof(void*),0)));}_LL1FA:;}{
# 2317
struct _tuple1*_tmp4C6=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4C7=Cyc_Absyn_var_exp(_tmp4C6,0);
struct Cyc_Absyn_Exp*_tmp4C8=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp4C9=Cyc_Absyn_aggrarrow_exp(_tmp4C7,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp4CA=Cyc_Absyn_neq_exp(_tmp4C9,_tmp4C8,0);
struct Cyc_Absyn_Exp*_tmp4CB=Cyc_Absyn_aggrarrow_exp(_tmp4C7,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp4CC=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp4CB,0),0);
struct Cyc_Absyn_Stmt*_tmp4CD=Cyc_Absyn_ifthenelse_stmt(_tmp4CA,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
void*_tmp4CE=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp4CF=Cyc_Absyn_address_exp(aggrproj(e1,f,0),0);
struct Cyc_Absyn_Stmt*_tmp4D0=Cyc_Absyn_declare_stmt(_tmp4C6,_tmp4CE,_tmp4CF,Cyc_Absyn_seq_stmt(_tmp4CD,_tmp4CC,0),0);
return Cyc_Toc_stmt_exp_r(_tmp4D0);}else{
# 2331
struct Cyc_Absyn_Exp*_tmp4D1=Cyc_Toc_member_exp(aggrproj(_tmp4C7,f,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp4D2=Cyc_Absyn_neq_exp(_tmp4D1,_tmp4C8,0);
struct Cyc_Absyn_Exp*_tmp4D3=Cyc_Toc_member_exp(aggrproj(_tmp4C7,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp4D4=Cyc_Absyn_exp_stmt(_tmp4D3,0);
struct Cyc_Absyn_Stmt*_tmp4D5=Cyc_Absyn_ifthenelse_stmt(_tmp4D2,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*_tmp4D6=Cyc_Absyn_declare_stmt(_tmp4C6,e1_c_type,e1,Cyc_Absyn_seq_stmt(_tmp4D5,_tmp4D4,0),0);
return Cyc_Toc_stmt_exp_r(_tmp4D6);}};}
# 2341
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _dyneither_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 2344
void*_tmp4D7=Cyc_Tcutil_compress(t);void*_tmp4D8=_tmp4D7;union Cyc_Absyn_AggrInfoU _tmp4DA;_LL200: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4D9=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4D8;if(_tmp4D9->tag != 11)goto _LL202;else{_tmp4DA=(_tmp4D9->f1).aggr_info;}}_LL201: {
# 2346
struct Cyc_Absyn_Aggrdecl*_tmp4DB=Cyc_Absyn_get_known_aggrdecl(_tmp4DA);
*f_tag=Cyc_Toc_get_member_offset(_tmp4DB,f);{
# 2349
const char*_tmpC57;void*_tmpC56[2];struct Cyc_String_pa_PrintArg_struct _tmpC55;struct Cyc_String_pa_PrintArg_struct _tmpC54;struct _dyneither_ptr str=(struct _dyneither_ptr)((_tmpC54.tag=0,((_tmpC54.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpC55.tag=0,((_tmpC55.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp4DB->name).f2),((_tmpC56[0]=& _tmpC55,((_tmpC56[1]=& _tmpC54,Cyc_aprintf(((_tmpC57="_union_%s_%s",_tag_dyneither(_tmpC57,sizeof(char),13))),_tag_dyneither(_tmpC56,sizeof(void*),2))))))))))))));
{struct _dyneither_ptr*_tmpC58;*tagged_member_type=Cyc_Absyn_strct(((_tmpC58=_cycalloc(sizeof(*_tmpC58)),((_tmpC58[0]=str,_tmpC58)))));}
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4DB->impl))->tagged;};}_LL202:;_LL203:
 return 0;_LL1FF:;}
# 2360
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 2363
void*_tmp4E1=e->r;void*_tmp4E2=_tmp4E1;struct Cyc_Absyn_Exp*_tmp4E4;struct Cyc_Absyn_Exp*_tmp4E6;struct _dyneither_ptr*_tmp4E7;int*_tmp4E8;struct Cyc_Absyn_Exp*_tmp4EA;struct _dyneither_ptr*_tmp4EB;int*_tmp4EC;_LL205: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4E3=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4E2;if(_tmp4E3->tag != 13)goto _LL207;else{_tmp4E4=_tmp4E3->f2;}}_LL206: {
const char*_tmpC5B;void*_tmpC5A;(_tmpC5A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC5B="cast on lhs!",_tag_dyneither(_tmpC5B,sizeof(char),13))),_tag_dyneither(_tmpC5A,sizeof(void*),0)));}_LL207: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp4E5=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4E2;if(_tmp4E5->tag != 20)goto _LL209;else{_tmp4E6=_tmp4E5->f1;_tmp4E7=_tmp4E5->f2;_tmp4E8=(int*)& _tmp4E5->f4;}}_LL208:
# 2366
 return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(_tmp4E6->topt),_tmp4E7,f_tag,tagged_member_type,clear_read,_tmp4E8);_LL209: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp4E9=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4E2;if(_tmp4E9->tag != 21)goto _LL20B;else{_tmp4EA=_tmp4E9->f1;_tmp4EB=_tmp4E9->f2;_tmp4EC=(int*)& _tmp4E9->f4;}}_LL20A: {
# 2369
void*_tmp4EF=Cyc_Tcutil_compress((void*)_check_null(_tmp4EA->topt));void*_tmp4F0=_tmp4EF;void*_tmp4F2;_LL20E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4F1=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4F0;if(_tmp4F1->tag != 5)goto _LL210;else{_tmp4F2=(_tmp4F1->f1).elt_typ;}}_LL20F:
# 2371
 return Cyc_Toc_is_tagged_union_project_impl(_tmp4F2,_tmp4EB,f_tag,tagged_member_type,clear_read,_tmp4EC);_LL210:;_LL211:
# 2373
 return 0;_LL20D:;}_LL20B:;_LL20C:
# 2375
 return 0;_LL204:;}
# 2387 "toc.cyc"
void Cyc_Toc_add_tagged_union_check_for_swap(struct Cyc_Absyn_Exp*e,int tag,void*mem_type){
struct _tuple1*_tmp4F3=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp4F3,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_val,0),0);
struct Cyc_Absyn_Exp*_tmp4F4=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_ifthenelse_stmt(_tmp4F4,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp4F3,Cyc_Absyn_cstar_typ(mem_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(e)),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
e->r=Cyc_Toc_stmt_exp_r(s1);}
# 2411 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 2415
struct _tuple1*_tmp4F5=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp4F5,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{
# 2425
struct Cyc_Absyn_Exp*_tmp4F6=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
s2=Cyc_Absyn_ifthenelse_stmt(_tmp4F6,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{
# 2428
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp4F5,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(e1),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple28{struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};
# 2434
static void Cyc_Toc_new_comprehension_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,void*new_e_type,struct Cyc_Absyn_Exp*rgnopt,void*old_typ,struct Cyc_Absyn_Exp*e1,void*t1,int zero_term,struct Cyc_Absyn_Exp*init_exp,struct Cyc_Absyn_Vardecl*vd){
# 2449 "toc.cyc"
struct _tuple1*max=Cyc_Toc_temp_var();
struct _tuple1*a=Cyc_Toc_temp_var();
void*old_elt_typ=t1;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c(nv,e1);{
struct Cyc_Absyn_Exp*_tmp4F7=Cyc_Absyn_var_exp(max,0);
# 2457
struct Cyc_Absyn_Stmt*s;
if(zero_term)
_tmp4F7=Cyc_Absyn_add_exp(_tmp4F7,Cyc_Absyn_uint_exp(1,0),0);
if(init_exp != 0)
s=Cyc_Toc_init_comprehension(nv,new_e_type,
Cyc_Absyn_var_exp(a,0),(struct Cyc_Absyn_Vardecl*)_check_null(vd),Cyc_Absyn_var_exp(max,0),init_exp,zero_term,
Cyc_Toc_skip_stmt_dl(),1);else{
if(zero_term)
s=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(a,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(elt_typ,Cyc_Absyn_uint_exp(0,0)),0),0);else{
# 2468
s=Cyc_Absyn_skip_stmt(0);}}{
# 2470
struct _RegionHandle _tmp4F8=_new_region("r");struct _RegionHandle*r=& _tmp4F8;_push_region(r);
{struct _tuple28*_tmpC5E;struct Cyc_List_List*_tmpC5D;struct Cyc_List_List*decls=
(_tmpC5D=_region_malloc(r,sizeof(*_tmpC5D)),((_tmpC5D->hd=((_tmpC5E=_region_malloc(r,sizeof(*_tmpC5E)),((_tmpC5E->f1=max,((_tmpC5E->f2=Cyc_Absyn_uint_typ,((_tmpC5E->f3=e1,_tmpC5E)))))))),((_tmpC5D->tl=0,_tmpC5D)))));
struct Cyc_Absyn_Exp*ai;
if(rgnopt == 0  || Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*_tmpC5F[2];ai=Cyc_Toc_malloc_exp(old_elt_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpC5F[1]=_tmp4F7,((_tmpC5F[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC5F,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));}else{
# 2480
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);{
struct Cyc_Absyn_Exp*_tmpC60[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpC60[1]=_tmp4F7,((_tmpC60[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC60,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{
# 2486
struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);
{struct _tuple28*_tmpC63;struct Cyc_List_List*_tmpC62;decls=((_tmpC62=_region_malloc(r,sizeof(*_tmpC62)),((_tmpC62->hd=((_tmpC63=_region_malloc(r,sizeof(*_tmpC63)),((_tmpC63->f1=a,((_tmpC63->f2=ptr_typ,((_tmpC63->f3=ainit,_tmpC63)))))))),((_tmpC62->tl=decls,_tmpC62))))));}
if(!Cyc_Tcutil_is_pointer_type(old_typ)){
const char*_tmpC66;void*_tmpC65;(_tmpC65=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC66="new_comprehension_to_c: comprehension is not an array type",_tag_dyneither(_tmpC66,sizeof(char),59))),_tag_dyneither(_tmpC65,sizeof(void*),0)));}
if(Cyc_Tcutil_is_dyneither_ptr(old_typ)){
struct _tuple1*_tmp4FF=Cyc_Toc_temp_var();
void*_tmp500=Cyc_Toc_typ_to_c(old_typ);
struct Cyc_Absyn_Exp*_tmp501=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpC67[3];struct Cyc_Absyn_Exp*_tmp502=
Cyc_Absyn_fncall_exp(_tmp501,(
(_tmpC67[2]=_tmp4F7,((_tmpC67[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((_tmpC67[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC67,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 2498
{struct _tuple28*_tmpC6A;struct Cyc_List_List*_tmpC69;decls=((_tmpC69=_region_malloc(r,sizeof(*_tmpC69)),((_tmpC69->hd=((_tmpC6A=_region_malloc(r,sizeof(*_tmpC6A)),((_tmpC6A->f1=_tmp4FF,((_tmpC6A->f2=_tmp500,((_tmpC6A->f3=_tmp502,_tmpC6A)))))))),((_tmpC69->tl=decls,_tmpC69))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp4FF,0),0),0);}else{
# 2501
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}
{struct Cyc_List_List*_tmp506=decls;for(0;_tmp506 != 0;_tmp506=_tmp506->tl){
struct _tuple28 _tmp507=*((struct _tuple28*)_tmp506->hd);struct _tuple1*_tmp509;void*_tmp50A;struct Cyc_Absyn_Exp*_tmp50B;struct _tuple28 _tmp508=_tmp507;_tmp509=_tmp508.f1;_tmp50A=_tmp508.f2;_tmp50B=_tmp508.f3;
s=Cyc_Absyn_declare_stmt(_tmp509,_tmp50A,_tmp50B,s,0);}}
# 2506
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2471
;_pop_region(r);};};}struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static void _tmpCB9(unsigned int*_tmpCB8,unsigned int*_tmpCB7,struct _tuple1***_tmpCB5){for(*_tmpCB8=0;*_tmpCB8 < *_tmpCB7;(*_tmpCB8)++){(*_tmpCB5)[*_tmpCB8]=(struct _tuple1*)
# 2881 "toc.cyc"
Cyc_Toc_temp_var();}}
# 2510 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
void*_tmp50E=e->r;
if(e->topt == 0){
const char*_tmpC6E;void*_tmpC6D[1];struct Cyc_String_pa_PrintArg_struct _tmpC6C;(_tmpC6C.tag=0,((_tmpC6C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC6D[0]=& _tmpC6C,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC6E="exp_to_c: no type for %s",_tag_dyneither(_tmpC6E,sizeof(char),25))),_tag_dyneither(_tmpC6D,sizeof(void*),1)))))));}
# 2516
if((nv->struct_info).lhs_exp != 0){
void*_tmp512=_tmp50E;_LL213: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp513=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp513->tag != 28)goto _LL215;}_LL214:
 goto _LL212;_LL215:;_LL216:
 nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);_LL212:;}{
# 2524
void*old_typ=(void*)_check_null(e->topt);
void*_tmp514=_tmp50E;struct _tuple1*_tmp518;void*_tmp519;enum Cyc_Absyn_Primop _tmp51B;struct Cyc_List_List*_tmp51C;struct Cyc_Absyn_Exp*_tmp51E;enum Cyc_Absyn_Incrementor _tmp51F;struct Cyc_Absyn_Exp*_tmp521;struct Cyc_Core_Opt*_tmp522;struct Cyc_Absyn_Exp*_tmp523;struct Cyc_Absyn_Exp*_tmp525;struct Cyc_Absyn_Exp*_tmp526;struct Cyc_Absyn_Exp*_tmp527;struct Cyc_Absyn_Exp*_tmp529;struct Cyc_Absyn_Exp*_tmp52A;struct Cyc_Absyn_Exp*_tmp52C;struct Cyc_Absyn_Exp*_tmp52D;struct Cyc_Absyn_Exp*_tmp52F;struct Cyc_Absyn_Exp*_tmp530;struct Cyc_Absyn_Exp*_tmp532;struct Cyc_List_List*_tmp533;struct Cyc_Absyn_Exp*_tmp535;struct Cyc_List_List*_tmp536;int _tmp537;struct Cyc_List_List*_tmp538;struct Cyc_Absyn_VarargInfo*_tmp539;struct Cyc_Absyn_Exp*_tmp53B;int _tmp53C;struct Cyc_Absyn_Exp*_tmp53E;struct Cyc_Absyn_Exp*_tmp540;struct Cyc_List_List*_tmp541;void**_tmp543;struct Cyc_Absyn_Exp*_tmp544;int _tmp545;enum Cyc_Absyn_Coercion _tmp546;struct Cyc_Absyn_Exp*_tmp548;struct Cyc_Absyn_Exp*_tmp54A;struct Cyc_Absyn_Exp*_tmp54B;struct Cyc_Absyn_Exp*_tmp54D;void*_tmp54F;void*_tmp551;struct Cyc_List_List*_tmp552;struct Cyc_Absyn_Exp*_tmp554;struct Cyc_Absyn_Exp*_tmp556;struct _dyneither_ptr*_tmp557;int _tmp558;int _tmp559;struct Cyc_Absyn_Exp*_tmp55B;struct _dyneither_ptr*_tmp55C;int _tmp55D;int _tmp55E;struct Cyc_Absyn_Exp*_tmp560;struct Cyc_Absyn_Exp*_tmp561;struct Cyc_List_List*_tmp563;struct Cyc_List_List*_tmp565;struct Cyc_Absyn_Vardecl*_tmp567;struct Cyc_Absyn_Exp*_tmp568;struct Cyc_Absyn_Exp*_tmp569;int _tmp56A;struct Cyc_Absyn_Exp*_tmp56C;void*_tmp56D;int _tmp56E;struct _tuple1*_tmp570;struct Cyc_List_List*_tmp571;struct Cyc_List_List*_tmp572;struct Cyc_Absyn_Aggrdecl*_tmp573;void*_tmp575;struct Cyc_List_List*_tmp576;struct Cyc_List_List*_tmp578;struct Cyc_Absyn_Datatypedecl*_tmp579;struct Cyc_Absyn_Datatypefield*_tmp57A;int _tmp57E;struct Cyc_Absyn_Exp*_tmp57F;void**_tmp580;struct Cyc_Absyn_Exp*_tmp581;int _tmp582;struct Cyc_Absyn_Exp*_tmp584;struct Cyc_Absyn_Exp*_tmp585;struct Cyc_Absyn_Exp*_tmp587;struct _dyneither_ptr*_tmp588;struct Cyc_Absyn_Stmt*_tmp58A;_LL218: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp515=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp515->tag != 0)goto _LL21A;else{if(((_tmp515->f1).Null_c).tag != 1)goto _LL21A;}}_LL219: {
# 2531
struct Cyc_Absyn_Exp*_tmp58F=Cyc_Absyn_uint_exp(0,0);
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp58F,_tmp58F))->r;else{
# 2536
struct Cyc_Absyn_Exp*_tmpC6F[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((_tmpC6F[2]=_tmp58F,((_tmpC6F[1]=_tmp58F,((_tmpC6F[0]=_tmp58F,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC6F,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}else{
# 2538
e->r=(void*)& Cyc_Toc_zero_exp;}
# 2540
goto _LL217;}_LL21A: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp516=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp516->tag != 0)goto _LL21C;}_LL21B:
 goto _LL217;_LL21C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp517=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp517->tag != 1)goto _LL21E;else{_tmp518=_tmp517->f1;_tmp519=(void*)_tmp517->f2;}}_LL21D:
# 2543
{struct _handler_cons _tmp591;_push_handler(& _tmp591);{int _tmp593=0;if(setjmp(_tmp591.handler))_tmp593=1;if(!_tmp593){e->r=(Cyc_Toc_lookup_varmap(nv,_tmp518))->r;;_pop_handler();}else{void*_tmp592=(void*)_exn_thrown;void*_tmp595=_tmp592;void*_tmp597;_LL26D: {struct Cyc_Dict_Absent_exn_struct*_tmp596=(struct Cyc_Dict_Absent_exn_struct*)_tmp595;if(_tmp596->tag != Cyc_Dict_Absent)goto _LL26F;}_LL26E: {
# 2545
const char*_tmpC73;void*_tmpC72[1];struct Cyc_String_pa_PrintArg_struct _tmpC71;(_tmpC71.tag=0,((_tmpC71.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp518)),((_tmpC72[0]=& _tmpC71,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC73="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpC73,sizeof(char),32))),_tag_dyneither(_tmpC72,sizeof(void*),1)))))));}_LL26F: _tmp597=_tmp595;_LL270:(void)_rethrow(_tmp597);_LL26C:;}};}
# 2547
goto _LL217;_LL21E: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp51A=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp51A->tag != 2)goto _LL220;else{_tmp51B=_tmp51A->f1;_tmp51C=_tmp51A->f2;}}_LL21F: {
# 2550
struct Cyc_List_List*_tmp59B=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp51C);
# 2552
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp51C);
switch(_tmp51B){case Cyc_Absyn_Numelts: _LL271: {
# 2555
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp51C))->hd;
{void*_tmp59C=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_tmp59D=_tmp59C;struct Cyc_Absyn_Exp*_tmp59F;void*_tmp5A1;union Cyc_Absyn_Constraint*_tmp5A2;union Cyc_Absyn_Constraint*_tmp5A3;union Cyc_Absyn_Constraint*_tmp5A4;_LL274: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp59E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp59D;if(_tmp59E->tag != 8)goto _LL276;else{_tmp59F=(_tmp59E->f1).num_elts;}}_LL275:
# 2559
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp59F))){
const char*_tmpC76;void*_tmpC75;(_tmpC75=0,Cyc_Tcutil_terr(e->loc,((_tmpC76="can't calculate numelts",_tag_dyneither(_tmpC76,sizeof(char),24))),_tag_dyneither(_tmpC75,sizeof(void*),0)));}
e->r=_tmp59F->r;goto _LL273;_LL276: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5A0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp59D;if(_tmp5A0->tag != 5)goto _LL278;else{_tmp5A1=(_tmp5A0->f1).elt_typ;_tmp5A2=((_tmp5A0->f1).ptr_atts).nullable;_tmp5A3=((_tmp5A0->f1).ptr_atts).bounds;_tmp5A4=((_tmp5A0->f1).ptr_atts).zero_term;}}_LL277:
# 2563
{void*_tmp5A7=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5A3);void*_tmp5A8=_tmp5A7;struct Cyc_Absyn_Exp*_tmp5AB;_LL27B: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5A9=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5A8;if(_tmp5A9->tag != 0)goto _LL27D;}_LL27C:
# 2565
{struct Cyc_Absyn_Exp*_tmpC77[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,(
(_tmpC77[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5A1),0),((_tmpC77[0]=(struct Cyc_Absyn_Exp*)_tmp51C->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC77,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL27A;_LL27D: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5AA=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5A8;if(_tmp5AA->tag != 1)goto _LL27A;else{_tmp5AB=_tmp5AA->f1;}}_LL27E:
# 2571
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5A4)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp51C->hd);
# 2575
struct Cyc_Absyn_Exp*_tmpC78[2];e->r=Cyc_Toc_fncall_exp_r(function_e,((_tmpC78[1]=_tmp5AB,((_tmpC78[0]=(struct Cyc_Absyn_Exp*)_tmp51C->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC78,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5A2)){
if(!Cyc_Evexp_c_can_eval(_tmp5AB)){
const char*_tmpC7B;void*_tmpC7A;(_tmpC7A=0,Cyc_Tcutil_terr(e->loc,((_tmpC7B="can't calculate numelts",_tag_dyneither(_tmpC7B,sizeof(char),24))),_tag_dyneither(_tmpC7A,sizeof(void*),0)));}
# 2580
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp5AB,Cyc_Absyn_uint_exp(0,0));}else{
# 2582
e->r=_tmp5AB->r;goto _LL27A;}}
# 2584
goto _LL27A;_LL27A:;}
# 2586
goto _LL273;_LL278:;_LL279: {
# 2588
const char*_tmpC80;void*_tmpC7F[2];struct Cyc_String_pa_PrintArg_struct _tmpC7E;struct Cyc_String_pa_PrintArg_struct _tmpC7D;(_tmpC7D.tag=0,((_tmpC7D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpC7E.tag=0,((_tmpC7E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpC7F[0]=& _tmpC7E,((_tmpC7F[1]=& _tmpC7D,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC80="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpC80,sizeof(char),41))),_tag_dyneither(_tmpC7F,sizeof(void*),2)))))))))))));}_LL273:;}
# 2591
break;}case Cyc_Absyn_Plus: _LL272:
# 2596
{void*_tmp5B4=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp59B))->hd);void*_tmp5B5=_tmp5B4;void*_tmp5B7;union Cyc_Absyn_Constraint*_tmp5B8;union Cyc_Absyn_Constraint*_tmp5B9;_LL281: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5B6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B5;if(_tmp5B6->tag != 5)goto _LL283;else{_tmp5B7=(_tmp5B6->f1).elt_typ;_tmp5B8=((_tmp5B6->f1).ptr_atts).bounds;_tmp5B9=((_tmp5B6->f1).ptr_atts).zero_term;}}_LL282:
# 2598
{void*_tmp5BA=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5B8);void*_tmp5BB=_tmp5BA;struct Cyc_Absyn_Exp*_tmp5BE;_LL286: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5BC=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5BB;if(_tmp5BC->tag != 0)goto _LL288;}_LL287: {
# 2600
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp51C))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp51C->tl))->hd;
{struct Cyc_Absyn_Exp*_tmpC81[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpC81[2]=e2,((_tmpC81[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5B7),0),((_tmpC81[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC81,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}
goto _LL285;}_LL288: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5BD=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5BB;if(_tmp5BD->tag != 1)goto _LL285;else{_tmp5BE=_tmp5BD->f1;}}_LL289:
# 2606
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5B9)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp51C))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp51C->tl))->hd;
struct Cyc_Absyn_Exp*_tmpC82[3];e->r=(Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1),((_tmpC82[2]=e2,((_tmpC82[1]=_tmp5BE,((_tmpC82[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC82,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0))->r;}
# 2611
goto _LL285;_LL285:;}
# 2613
goto _LL280;_LL283:;_LL284:
# 2615
 goto _LL280;_LL280:;}
# 2617
break;case Cyc_Absyn_Minus: _LL27F: {
# 2622
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(_tmp59B))->hd,& elt_typ)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp51C))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp51C->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(_tmp59B->tl))->hd)){
e1->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);
e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp);
e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{
# 2632
struct Cyc_Absyn_Exp*_tmpC83[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpC83[2]=
Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0),((_tmpC83[1]=
# 2633
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpC83[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC83,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 2637
break;}case Cyc_Absyn_Eq: _LL28A:
 goto _LL28B;case Cyc_Absyn_Neq: _LL28B:
 goto _LL28C;case Cyc_Absyn_Gt: _LL28C:
 goto _LL28D;case Cyc_Absyn_Gte: _LL28D:
 goto _LL28E;case Cyc_Absyn_Lt: _LL28E:
 goto _LL28F;case Cyc_Absyn_Lte: _LL28F: {
# 2645
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp51C))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp51C->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp59B))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp59B->tl))->hd;
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& elt_typ))
e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,0));
if(Cyc_Tcutil_is_tagged_pointer_typ(t2))
e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp,0));
break;}default: _LL290:
 break;}
# 2657
goto _LL217;}_LL220: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp51D=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp51D->tag != 4)goto _LL222;else{_tmp51E=_tmp51D->f1;_tmp51F=_tmp51D->f2;}}_LL221: {
# 2659
void*e2_cyc_typ=(void*)_check_null(_tmp51E->topt);
# 2668 "toc.cyc"
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
const char*_tmpC84;struct _dyneither_ptr incr_str=(_tmpC84="increment",_tag_dyneither(_tmpC84,sizeof(char),10));
if(_tmp51F == Cyc_Absyn_PreDec  || _tmp51F == Cyc_Absyn_PostDec){const char*_tmpC85;incr_str=((_tmpC85="decrement",_tag_dyneither(_tmpC85,sizeof(char),10)));}
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp51E,& ptr_type,& is_dyneither,& elt_type)){
{const char*_tmpC89;void*_tmpC88[1];struct Cyc_String_pa_PrintArg_struct _tmpC87;(_tmpC87.tag=0,((_tmpC87.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC88[0]=& _tmpC87,Cyc_Tcutil_terr(e->loc,((_tmpC89="in-place %s is not supported when dereferencing a zero-terminated pointer",_tag_dyneither(_tmpC89,sizeof(char),74))),_tag_dyneither(_tmpC88,sizeof(void*),1)))))));}{
const char*_tmpC8C;void*_tmpC8B;(_tmpC8B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC8C="in-place inc/dec on zero-term",_tag_dyneither(_tmpC8C,sizeof(char),30))),_tag_dyneither(_tmpC8B,sizeof(void*),0)));};}{
# 2677
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
int f_tag=0;
if(Cyc_Toc_is_tagged_union_project(_tmp51E,& f_tag,& tunion_member_type,1)){
struct Cyc_Absyn_Exp*_tmp5C8=Cyc_Absyn_signed_int_exp(1,0);
_tmp5C8->topt=Cyc_Absyn_sint_typ;
switch(_tmp51F){case Cyc_Absyn_PreInc: _LL292:
# 2684
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpC92;struct Cyc_Core_Opt*_tmpC91;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpC90;e->r=(void*)((_tmpC90=_cycalloc(sizeof(*_tmpC90)),((_tmpC90[0]=((_tmpC92.tag=3,((_tmpC92.f1=_tmp51E,((_tmpC92.f2=((_tmpC91=_cycalloc_atomic(sizeof(*_tmpC91)),((_tmpC91->v=(void*)Cyc_Absyn_Plus,_tmpC91)))),((_tmpC92.f3=_tmp5C8,_tmpC92)))))))),_tmpC90))));}
Cyc_Toc_exp_to_c(nv,e);
return;case Cyc_Absyn_PreDec: _LL293:
# 2688
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpC98;struct Cyc_Core_Opt*_tmpC97;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpC96;e->r=(void*)((_tmpC96=_cycalloc(sizeof(*_tmpC96)),((_tmpC96[0]=((_tmpC98.tag=3,((_tmpC98.f1=_tmp51E,((_tmpC98.f2=((_tmpC97=_cycalloc_atomic(sizeof(*_tmpC97)),((_tmpC97->v=(void*)Cyc_Absyn_Minus,_tmpC97)))),((_tmpC98.f3=_tmp5C8,_tmpC98)))))))),_tmpC96))));}
Cyc_Toc_exp_to_c(nv,e);
return;default: _LL294:
# 2692
{const char*_tmpC9C;void*_tmpC9B[1];struct Cyc_String_pa_PrintArg_struct _tmpC9A;(_tmpC9A.tag=0,((_tmpC9A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC9B[0]=& _tmpC9A,Cyc_Tcutil_terr(e->loc,((_tmpC9C="in-place post-%s is not supported on @tagged union members",_tag_dyneither(_tmpC9C,sizeof(char),59))),_tag_dyneither(_tmpC9B,sizeof(void*),1)))))));}{
# 2694
const char*_tmpC9F;void*_tmpC9E;(_tmpC9E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC9F="in-place inc/dec on @tagged union",_tag_dyneither(_tmpC9F,sizeof(char),34))),_tag_dyneither(_tmpC9E,sizeof(void*),0)));};}}
# 2697
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp51E);
Cyc_Toc_set_lhs(nv,0);{
# 2702
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp51F == Cyc_Absyn_PostInc  || _tmp51F == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2708
if(_tmp51F == Cyc_Absyn_PreDec  || _tmp51F == Cyc_Absyn_PostDec)
change=-1;{
struct Cyc_Absyn_Exp*_tmpCA0[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpCA0[2]=
# 2712
Cyc_Absyn_signed_int_exp(change,0),((_tmpCA0[1]=
# 2711
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpCA0[0]=
# 2710
Cyc_Toc_push_address_exp(_tmp51E),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCA0,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2713
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2717
struct Cyc_Toc_functionSet*_tmp5D5=_tmp51F != Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
# 2719
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp5D5,_tmp51E);
struct Cyc_Absyn_Exp*_tmpCA1[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpCA1[1]=
Cyc_Absyn_signed_int_exp(1,0),((_tmpCA1[0]=
# 2720
Cyc_Toc_push_address_exp(_tmp51E),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCA1,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
# 2722
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp51E)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp51E,0,Cyc_Toc_incr_lvalue,_tmp51F);
e->r=_tmp51E->r;}}}
# 2726
goto _LL217;};};}_LL222: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp520=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp520->tag != 3)goto _LL224;else{_tmp521=_tmp520->f1;_tmp522=_tmp520->f2;_tmp523=_tmp520->f3;}}_LL223: {
# 2745 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp521->topt);
void*e2_old_typ=(void*)_check_null(_tmp523->topt);
int f_tag=0;
void*tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Toc_is_tagged_union_project(_tmp521,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp521);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp523);
e->r=Cyc_Toc_tagged_union_assignop(_tmp521,e1_old_typ,_tmp522,_tmp523,e2_old_typ,f_tag,tagged_member_struct_type);
# 2756
return;}{
# 2758
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp521,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp521,_tmp522,_tmp523,ptr_type,is_dyneither,elt_type);
# 2764
return;}{
# 2768
int e1_poly=Cyc_Toc_is_poly_project(_tmp521);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp521);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp523);{
# 2774
int done=0;
if(_tmp522 != 0){
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp5D8=(enum Cyc_Absyn_Primop)_tmp522->v;switch(_tmp5D8){case Cyc_Absyn_Plus: _LL296:
 change=_tmp523;break;case Cyc_Absyn_Minus: _LL297:
# 2782
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp523,0);break;default: _LL298: {
const char*_tmpCA4;void*_tmpCA3;(_tmpCA3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCA4="bad t ? pointer arithmetic",_tag_dyneither(_tmpCA4,sizeof(char),27))),_tag_dyneither(_tmpCA3,sizeof(void*),0)));}}}
# 2785
done=1;{
# 2787
struct Cyc_Absyn_Exp*_tmp5DB=Cyc_Toc__dyneither_ptr_inplace_plus_e;
struct Cyc_Absyn_Exp*_tmpCA5[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5DB,((_tmpCA5[2]=change,((_tmpCA5[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpCA5[0]=
# 2788
Cyc_Toc_push_address_exp(_tmp521),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCA5,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2791
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2794
enum Cyc_Absyn_Primop _tmp5DD=(enum Cyc_Absyn_Primop)_tmp522->v;switch(_tmp5DD){case Cyc_Absyn_Plus: _LL29A:
# 2796
 done=1;
{struct Cyc_Absyn_Exp*_tmpCA6[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp521),((_tmpCA6[1]=_tmp523,((_tmpCA6[0]=_tmp521,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCA6,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
break;default: _LL29B: {
const char*_tmpCA9;void*_tmpCA8;(_tmpCA8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCA9="bad zero-terminated pointer arithmetic",_tag_dyneither(_tmpCA9,sizeof(char),39))),_tag_dyneither(_tmpCA8,sizeof(void*),0)));}}}}}
# 2803
if(!done){
# 2805
if(e1_poly)
_tmp523->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp523->r,0));
# 2811
if(!Cyc_Absyn_is_lvalue(_tmp521)){
{struct _tuple27 _tmpCAC;struct _tuple27*_tmpCAB;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple27*),struct _tuple27*f_env))Cyc_Toc_lvalue_assign)(_tmp521,0,Cyc_Toc_assignop_lvalue,((_tmpCAB=_cycalloc(sizeof(struct _tuple27)* 1),((_tmpCAB[0]=(struct _tuple27)((_tmpCAC.f1=_tmp522,((_tmpCAC.f2=_tmp523,_tmpCAC)))),_tmpCAB)))));}
e->r=_tmp521->r;}}
# 2816
goto _LL217;};};};}_LL224: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp524=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp524->tag != 5)goto _LL226;else{_tmp525=_tmp524->f1;_tmp526=_tmp524->f2;_tmp527=_tmp524->f3;}}_LL225:
# 2818
 Cyc_Toc_exp_to_c(nv,_tmp525);
Cyc_Toc_exp_to_c(nv,_tmp526);
Cyc_Toc_exp_to_c(nv,_tmp527);
goto _LL217;_LL226: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp528=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp528->tag != 6)goto _LL228;else{_tmp529=_tmp528->f1;_tmp52A=_tmp528->f2;}}_LL227:
# 2823
 Cyc_Toc_exp_to_c(nv,_tmp529);
Cyc_Toc_exp_to_c(nv,_tmp52A);
goto _LL217;_LL228: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp52B=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp52B->tag != 7)goto _LL22A;else{_tmp52C=_tmp52B->f1;_tmp52D=_tmp52B->f2;}}_LL229:
# 2827
 Cyc_Toc_exp_to_c(nv,_tmp52C);
Cyc_Toc_exp_to_c(nv,_tmp52D);
goto _LL217;_LL22A: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp52E=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp52E->tag != 8)goto _LL22C;else{_tmp52F=_tmp52E->f1;_tmp530=_tmp52E->f2;}}_LL22B:
# 2831
 Cyc_Toc_exp_to_c(nv,_tmp52F);
Cyc_Toc_exp_to_c(nv,_tmp530);
goto _LL217;_LL22C: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp531=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp531->tag != 9)goto _LL22E;else{_tmp532=_tmp531->f1;_tmp533=_tmp531->f2;if(_tmp531->f3 != 0)goto _LL22E;}}_LL22D:
# 2835
 Cyc_Toc_exp_to_c(nv,_tmp532);
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp533);
goto _LL217;_LL22E: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp534=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp534->tag != 9)goto _LL230;else{_tmp535=_tmp534->f1;_tmp536=_tmp534->f2;if(_tmp534->f3 == 0)goto _LL230;_tmp537=(_tmp534->f3)->num_varargs;_tmp538=(_tmp534->f3)->injectors;_tmp539=(_tmp534->f3)->vai;}}_LL22F: {
# 2847 "toc.cyc"
struct _RegionHandle _tmp5E3=_new_region("r");struct _RegionHandle*r=& _tmp5E3;_push_region(r);{
struct _tuple1*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp537,0);
void*cva_type=Cyc_Toc_typ_to_c(_tmp539->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_conref,0);
# 2855
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp536);
int num_normargs=num_args - _tmp537;
# 2859
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp536=_tmp536->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp536))->hd);{
struct Cyc_List_List*_tmpCAD;new_args=((_tmpCAD=_cycalloc(sizeof(*_tmpCAD)),((_tmpCAD->hd=(struct Cyc_Absyn_Exp*)_tmp536->hd,((_tmpCAD->tl=new_args,_tmpCAD))))));};}}
# 2864
{struct Cyc_Absyn_Exp*_tmpCB0[3];struct Cyc_List_List*_tmpCAF;new_args=((_tmpCAF=_cycalloc(sizeof(*_tmpCAF)),((_tmpCAF->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpCB0[2]=num_varargs_exp,((_tmpCB0[1]=
Cyc_Absyn_sizeoftyp_exp(cva_type,0),((_tmpCB0[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCB0,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpCAF->tl=new_args,_tmpCAF))))));}
# 2869
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);
# 2871
Cyc_Toc_exp_to_c(nv,_tmp535);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp535,new_args,0),0);
# 2875
if(_tmp539->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp5E7=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp539->type));void*_tmp5E8=_tmp5E7;struct Cyc_Absyn_Datatypedecl*_tmp5EA;_LL29E: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5E9=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5E8;if(_tmp5E9->tag != 3)goto _LL2A0;else{if((((_tmp5E9->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2A0;_tmp5EA=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp5E9->f1).datatype_info).KnownDatatype).val);}}_LL29F:
 tud=_tmp5EA;goto _LL29D;_LL2A0:;_LL2A1: {
const char*_tmpCB3;void*_tmpCB2;(_tmpCB2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCB3="toc: unknown datatype in vararg with inject",_tag_dyneither(_tmpCB3,sizeof(char),44))),_tag_dyneither(_tmpCB2,sizeof(void*),0)));}_LL29D:;}{
# 2881
unsigned int _tmpCB8;unsigned int _tmpCB7;struct _dyneither_ptr _tmpCB6;struct _tuple1**_tmpCB5;unsigned int _tmpCB4;struct _dyneither_ptr vs=(_tmpCB4=(unsigned int)_tmp537,((_tmpCB5=(struct _tuple1**)_region_malloc(r,_check_times(sizeof(struct _tuple1*),_tmpCB4)),((_tmpCB6=_tag_dyneither(_tmpCB5,sizeof(struct _tuple1*),_tmpCB4),((((_tmpCB7=_tmpCB4,_tmpCB9(& _tmpCB8,& _tmpCB7,& _tmpCB5))),_tmpCB6)))))));
# 2883
if(_tmp537 != 0){
# 2885
struct Cyc_List_List*_tmp5ED=0;
{int i=_tmp537 - 1;for(0;i >= 0;-- i){
struct Cyc_List_List*_tmpCBA;_tmp5ED=((_tmpCBA=_cycalloc(sizeof(*_tmpCBA)),((_tmpCBA->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0),0)),((_tmpCBA->tl=_tmp5ED,_tmpCBA))))));}}
# 2889
s=Cyc_Absyn_declare_stmt(argv,arr_type,Cyc_Absyn_unresolvedmem_exp(0,_tmp5ED,0),s,0);{
# 2892
int i=0;for(0;_tmp536 != 0;(((_tmp536=_tmp536->tl,_tmp538=_tmp538->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp536->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0);
struct Cyc_Absyn_Datatypefield*_tmp5EF=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp538))->hd;struct _tuple1*_tmp5F1;struct Cyc_List_List*_tmp5F2;struct Cyc_Absyn_Datatypefield*_tmp5F0=_tmp5EF;_tmp5F1=_tmp5F0->name;_tmp5F2=_tmp5F0->typs;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp5F2))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);
# 2907
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);
# 2910
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Toc_tag_sp,0),
Cyc_Toc_datatype_tag(tud,_tmp5F1),0),s,0);
# 2913
s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp5F1,tud->name)),0,s,0);};}};}else{
# 2920
struct _tuple19*_tmpCBB[3];struct Cyc_List_List*_tmp5F3=(_tmpCBB[2]=
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpCBB[1]=
# 2920
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpCBB[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCBB,sizeof(struct _tuple19*),3)))))));
# 2922
s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),Cyc_Absyn_uint_exp(0,0),s,0);}};}else{
# 2932
{int i=0;for(0;_tmp536 != 0;(_tmp536=_tmp536->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp536->hd);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp536->hd,0),s,0);}}
# 2938
s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}
# 2951 "toc.cyc"
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2953
_npop_handler(0);goto _LL217;
# 2847 "toc.cyc"
;_pop_region(r);}_LL230: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp53A=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp53A->tag != 10)goto _LL232;else{_tmp53B=_tmp53A->f1;_tmp53C=_tmp53A->f2;}}_LL231:
# 2956 "toc.cyc"
 Cyc_Toc_exp_to_c(nv,_tmp53B);{
struct Cyc_Absyn_Exp*_tmp5F9=_tmp53C?Cyc_Toc_newrethrow_exp(_tmp53B): Cyc_Toc_newthrow_exp(_tmp53B);
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),_tmp5F9,0))->r;
goto _LL217;};_LL232: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp53D=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp53D->tag != 11)goto _LL234;else{_tmp53E=_tmp53D->f1;}}_LL233:
 Cyc_Toc_exp_to_c(nv,_tmp53E);goto _LL217;_LL234: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp53F=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp53F->tag != 12)goto _LL236;else{_tmp540=_tmp53F->f1;_tmp541=_tmp53F->f2;}}_LL235:
# 2962
 Cyc_Toc_exp_to_c(nv,_tmp540);
# 2971 "toc.cyc"
for(0;_tmp541 != 0;_tmp541=_tmp541->tl){
enum Cyc_Absyn_KindQual _tmp5FA=(Cyc_Tcutil_typ_kind((void*)_tmp541->hd))->kind;
if(_tmp5FA != Cyc_Absyn_EffKind  && _tmp5FA != Cyc_Absyn_RgnKind){
{void*_tmp5FB=Cyc_Tcutil_compress((void*)_tmp541->hd);void*_tmp5FC=_tmp5FB;_LL2A3: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp5FD=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp5FC;if(_tmp5FD->tag != 2)goto _LL2A5;}_LL2A4:
 goto _LL2A6;_LL2A5: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5FE=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5FC;if(_tmp5FE->tag != 3)goto _LL2A7;}_LL2A6:
 continue;_LL2A7:;_LL2A8:
# 2978
 e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp540,0))->r;
goto _LL2A2;_LL2A2:;}
# 2981
break;}}
# 2984
goto _LL217;_LL236: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp542=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp542->tag != 13)goto _LL238;else{_tmp543=(void**)((void**)& _tmp542->f1);_tmp544=_tmp542->f2;_tmp545=_tmp542->f3;_tmp546=_tmp542->f4;}}_LL237: {
# 2986
void*old_t2=(void*)_check_null(_tmp544->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp543;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp543=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp544);
# 2993
{struct _tuple29 _tmpCBC;struct _tuple29 _tmp5FF=(_tmpCBC.f1=Cyc_Tcutil_compress(old_t2),((_tmpCBC.f2=Cyc_Tcutil_compress(new_typ),_tmpCBC)));struct _tuple29 _tmp600=_tmp5FF;struct Cyc_Absyn_PtrInfo _tmp602;struct Cyc_Absyn_PtrInfo _tmp604;struct Cyc_Absyn_PtrInfo _tmp606;_LL2AA:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp601=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp600.f1;if(_tmp601->tag != 5)goto _LL2AC;else{_tmp602=_tmp601->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp603=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp600.f2;if(_tmp603->tag != 5)goto _LL2AC;else{_tmp604=_tmp603->f1;}};_LL2AB: {
# 2995
int _tmp608=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp602.ptr_atts).nullable);
int _tmp609=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp604.ptr_atts).nullable);
void*_tmp60A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp602.ptr_atts).bounds);
void*_tmp60B=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp604.ptr_atts).bounds);
int _tmp60C=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp602.ptr_atts).zero_term);
int _tmp60D=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp604.ptr_atts).zero_term);
{struct _tuple29 _tmpCBD;struct _tuple29 _tmp60E=(_tmpCBD.f1=_tmp60A,((_tmpCBD.f2=_tmp60B,_tmpCBD)));struct _tuple29 _tmp60F=_tmp60E;struct Cyc_Absyn_Exp*_tmp611;struct Cyc_Absyn_Exp*_tmp613;struct Cyc_Absyn_Exp*_tmp615;struct Cyc_Absyn_Exp*_tmp619;_LL2B1:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp610=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp60F.f1;if(_tmp610->tag != 1)goto _LL2B3;else{_tmp611=_tmp610->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp612=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp60F.f2;if(_tmp612->tag != 1)goto _LL2B3;else{_tmp613=_tmp612->f1;}};_LL2B2:
# 3003
 if((!Cyc_Evexp_c_can_eval(_tmp611) || !Cyc_Evexp_c_can_eval(_tmp613)) && !
Cyc_Evexp_same_const_exp(_tmp611,_tmp613)){
const char*_tmpCC0;void*_tmpCBF;(_tmpCBF=0,Cyc_Tcutil_terr(e->loc,((_tmpCC0="can't validate cast due to potential size differences",_tag_dyneither(_tmpCC0,sizeof(char),54))),_tag_dyneither(_tmpCBF,sizeof(void*),0)));}
if(_tmp608  && !_tmp609){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpCC3;void*_tmpCC2;(_tmpCC2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCC3="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpCC3,sizeof(char),44))),_tag_dyneither(_tmpCC2,sizeof(void*),0)));}
# 3012
if(_tmp546 != Cyc_Absyn_NonNull_to_Null){
const char*_tmpCC7;void*_tmpCC6[1];struct Cyc_String_pa_PrintArg_struct _tmpCC5;(_tmpCC5.tag=0,((_tmpCC5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpCC6[0]=& _tmpCC5,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCC7="null-check conversion mis-classified: %s",_tag_dyneither(_tmpCC7,sizeof(char),41))),_tag_dyneither(_tmpCC6,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp544);
if(do_null_check){
if(!_tmp545){
const char*_tmpCCA;void*_tmpCC9;(_tmpCC9=0,Cyc_Tcutil_warn(e->loc,((_tmpCCA="inserted null check due to implicit cast from * to @ type",_tag_dyneither(_tmpCCA,sizeof(char),58))),_tag_dyneither(_tmpCC9,sizeof(void*),0)));}{
# 3019
struct Cyc_List_List*_tmpCCB;e->r=Cyc_Toc_cast_it_r(*_tmp543,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpCCB=_cycalloc(sizeof(*_tmpCCB)),((_tmpCCB->hd=_tmp544,((_tmpCCB->tl=0,_tmpCCB)))))),0));};}else{
# 3023
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp544->r;}};}else{
# 3028
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp544->r;}
# 3035
goto _LL2B0;_LL2B3:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp614=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp60F.f1;if(_tmp614->tag != 1)goto _LL2B5;else{_tmp615=_tmp614->f1;}}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp616=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp60F.f2;if(_tmp616->tag != 0)goto _LL2B5;};_LL2B4:
# 3037
 if(!Cyc_Evexp_c_can_eval(_tmp615)){
const char*_tmpCCE;void*_tmpCCD;(_tmpCCD=0,Cyc_Tcutil_terr(e->loc,((_tmpCCE="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpCCE,sizeof(char),71))),_tag_dyneither(_tmpCCD,sizeof(void*),0)));}
# 3040
if(_tmp546 == Cyc_Absyn_NonNull_to_Null){
const char*_tmpCD2;void*_tmpCD1[1];struct Cyc_String_pa_PrintArg_struct _tmpCD0;(_tmpCD0.tag=0,((_tmpCD0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpCD1[0]=& _tmpCD0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCD2="conversion mis-classified as null-check: %s",_tag_dyneither(_tmpCD2,sizeof(char),44))),_tag_dyneither(_tmpCD1,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){
# 3044
if((_tmp60C  && !(_tmp604.elt_tq).real_const) && !_tmp60D)
# 3047
_tmp615=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp615,Cyc_Absyn_uint_exp(1,0),0);
# 3049
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp615,_tmp544))->r;}else{
# 3051
struct Cyc_Absyn_Exp*_tmp62B=Cyc_Toc__tag_dyneither_e;
# 3053
if(_tmp60C){
# 3058
struct _tuple1*_tmp62C=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp62D=Cyc_Absyn_var_exp(_tmp62C,0);
struct Cyc_Absyn_Exp*arg3;
# 3063
{void*_tmp62E=_tmp544->r;void*_tmp62F=_tmp62E;_LL2BA: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp630=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp630->tag != 0)goto _LL2BC;else{if(((_tmp630->f1).String_c).tag != 8)goto _LL2BC;}}_LL2BB:
# 3065
 arg3=_tmp615;goto _LL2B9;_LL2BC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp631=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp631->tag != 0)goto _LL2BE;else{if(((_tmp631->f1).Wstring_c).tag != 9)goto _LL2BE;}}_LL2BD:
# 3067
 arg3=_tmp615;goto _LL2B9;_LL2BE:;_LL2BF:
# 3069
{struct Cyc_Absyn_Exp*_tmpCD3[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp544),(
# 3071
(_tmpCD3[1]=_tmp615,((_tmpCD3[0]=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp62D),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCD3,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
goto _LL2B9;_LL2B9:;}
# 3074
if(!_tmp60D  && !(_tmp604.elt_tq).real_const)
# 3077
arg3=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{
# 3079
struct Cyc_Absyn_Exp*_tmp633=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp604.elt_typ),0);
struct Cyc_Absyn_Exp*_tmpCD4[3];struct Cyc_Absyn_Exp*_tmp634=Cyc_Absyn_fncall_exp(_tmp62B,((_tmpCD4[2]=arg3,((_tmpCD4[1]=_tmp633,((_tmpCD4[0]=_tmp62D,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCD4,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
struct Cyc_Absyn_Stmt*_tmp635=Cyc_Absyn_exp_stmt(_tmp634,0);
_tmp635=Cyc_Absyn_declare_stmt(_tmp62C,Cyc_Toc_typ_to_c(old_t2),_tmp544,_tmp635,0);
e->r=Cyc_Toc_stmt_exp_r(_tmp635);};}else{
# 3086
struct Cyc_Absyn_Exp*_tmpCD5[3];e->r=Cyc_Toc_fncall_exp_r(_tmp62B,(
(_tmpCD5[2]=_tmp615,((_tmpCD5[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp604.elt_typ),0),((_tmpCD5[0]=_tmp544,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCD5,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 3092
goto _LL2B0;_LL2B5:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp617=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp60F.f1;if(_tmp617->tag != 0)goto _LL2B7;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp618=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp60F.f2;if(_tmp618->tag != 1)goto _LL2B7;else{_tmp619=_tmp618->f1;}};_LL2B6:
# 3094
 if(!Cyc_Evexp_c_can_eval(_tmp619)){
const char*_tmpCD8;void*_tmpCD7;(_tmpCD7=0,Cyc_Tcutil_terr(e->loc,((_tmpCD8="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpCD8,sizeof(char),71))),_tag_dyneither(_tmpCD7,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpCDB;void*_tmpCDA;(_tmpCDA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCDB="can't coerce t? to t* or t@ at the top-level",_tag_dyneither(_tmpCDB,sizeof(char),45))),_tag_dyneither(_tmpCDA,sizeof(void*),0)));}{
# 3106 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp63C=_tmp619;
if(_tmp60C  && !_tmp60D)
_tmp63C=Cyc_Absyn_add_exp(_tmp619,Cyc_Absyn_uint_exp(1,0),0);{
# 3113
struct Cyc_Absyn_Exp*_tmp63D=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpCDC[3];struct Cyc_Absyn_Exp*_tmp63E=Cyc_Absyn_fncall_exp(_tmp63D,(
(_tmpCDC[2]=_tmp63C,((_tmpCDC[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp602.elt_typ),0),((_tmpCDC[0]=_tmp544,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCDC,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 3118
if(_tmp609){
struct Cyc_List_List*_tmpCDD;_tmp63E->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,(
(_tmpCDD=_cycalloc(sizeof(*_tmpCDD)),((_tmpCDD->hd=Cyc_Absyn_copy_exp(_tmp63E),((_tmpCDD->tl=0,_tmpCDD)))))));}
e->r=Cyc_Toc_cast_it_r(*_tmp543,_tmp63E);
goto _LL2B0;};};_LL2B7:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp61A=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp60F.f1;if(_tmp61A->tag != 0)goto _LL2B0;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp61B=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp60F.f2;if(_tmp61B->tag != 0)goto _LL2B0;};_LL2B8:
# 3126
 DynCast:
 if((_tmp60C  && !_tmp60D) && !(_tmp604.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpCE0;void*_tmpCDF;(_tmpCDF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCE0="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",_tag_dyneither(_tmpCE0,sizeof(char),70))),_tag_dyneither(_tmpCDF,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp643=Cyc_Toc__dyneither_ptr_decrease_size_e;
struct Cyc_Absyn_Exp*_tmpCE1[3];e->r=Cyc_Toc_fncall_exp_r(_tmp643,(
(_tmpCE1[2]=
Cyc_Absyn_uint_exp(1,0),((_tmpCE1[1]=
# 3132
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp602.elt_typ),0),((_tmpCE1[0]=_tmp544,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCE1,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}
# 3135
goto _LL2B0;_LL2B0:;}
# 3137
goto _LL2A9;}_LL2AC:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp605=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp600.f1;if(_tmp605->tag != 5)goto _LL2AE;else{_tmp606=_tmp605->f1;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp607=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp600.f2;if(_tmp607->tag != 6)goto _LL2AE;};_LL2AD:
# 3139
{void*_tmp646=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp606.ptr_atts).bounds);void*_tmp647=_tmp646;_LL2C1: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp648=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp647;if(_tmp648->tag != 0)goto _LL2C3;}_LL2C2:
# 3141
 _tmp544->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp544->r,_tmp544->loc),Cyc_Toc_curr_sp);goto _LL2C0;_LL2C3:;_LL2C4:
 goto _LL2C0;_LL2C0:;}
# 3144
goto _LL2A9;_LL2AE:;_LL2AF:
# 3146
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp544->r;
goto _LL2A9;_LL2A9:;}
# 3150
goto _LL217;}_LL238: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp547=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp547->tag != 14)goto _LL23A;else{_tmp548=_tmp547->f1;}}_LL239:
# 3154
{void*_tmp64A=_tmp548->r;void*_tmp64B=_tmp64A;struct _tuple1*_tmp64D;struct Cyc_List_List*_tmp64E;struct Cyc_List_List*_tmp64F;struct Cyc_List_List*_tmp651;_LL2C6: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp64C=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp64B;if(_tmp64C->tag != 28)goto _LL2C8;else{_tmp64D=_tmp64C->f1;_tmp64E=_tmp64C->f2;_tmp64F=_tmp64C->f3;}}_LL2C7:
# 3156
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpCE5;void*_tmpCE4[1];struct Cyc_String_pa_PrintArg_struct _tmpCE3;(_tmpCE3.tag=0,((_tmpCE3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp548->loc)),((_tmpCE4[0]=& _tmpCE3,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCE5="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpCE5,sizeof(char),42))),_tag_dyneither(_tmpCE4,sizeof(void*),1)))))));}{
struct Cyc_Absyn_Exp*_tmp655=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp548->topt),_tmp64E,1,0,_tmp64F,_tmp64D);
e->r=_tmp655->r;
e->topt=_tmp655->topt;
goto _LL2C5;};_LL2C8: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp650=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp64B;if(_tmp650->tag != 23)goto _LL2CA;else{_tmp651=_tmp650->f1;}}_LL2C9:
# 3164
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpCE9;void*_tmpCE8[1];struct Cyc_String_pa_PrintArg_struct _tmpCE7;(_tmpCE7.tag=0,((_tmpCE7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp548->loc)),((_tmpCE8[0]=& _tmpCE7,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCE9="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpCE9,sizeof(char),42))),_tag_dyneither(_tmpCE8,sizeof(void*),1)))))));}
e->r=(Cyc_Toc_init_tuple(nv,1,0,_tmp651))->r;
goto _LL2C5;_LL2CA:;_LL2CB:
# 3170
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp548);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp548)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp548,0,Cyc_Toc_address_lvalue,1);
# 3176
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp548);}else{
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind((void*)_check_null(_tmp548->topt))))
# 3180
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp548);}
# 3182
goto _LL2C5;_LL2C5:;}
# 3184
goto _LL217;_LL23A: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp549=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp549->tag != 15)goto _LL23C;else{_tmp54A=_tmp549->f1;_tmp54B=_tmp549->f2;}}_LL23B:
# 3187
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpCED;void*_tmpCEC[1];struct Cyc_String_pa_PrintArg_struct _tmpCEB;(_tmpCEB.tag=0,((_tmpCEB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp54B->loc)),((_tmpCEC[0]=& _tmpCEB,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCED="%s: new at top-level",_tag_dyneither(_tmpCED,sizeof(char),21))),_tag_dyneither(_tmpCEC,sizeof(void*),1)))))));}{
void*new_e_type=(void*)_check_null(_tmp54B->topt);
{void*_tmp65C=_tmp54B->r;void*_tmp65D=_tmp65C;struct Cyc_List_List*_tmp65F;struct Cyc_Absyn_Vardecl*_tmp661;struct Cyc_Absyn_Exp*_tmp662;struct Cyc_Absyn_Exp*_tmp663;int _tmp664;struct Cyc_Absyn_Exp*_tmp666;void*_tmp667;int _tmp668;struct _tuple1*_tmp66A;struct Cyc_List_List*_tmp66B;struct Cyc_List_List*_tmp66C;struct Cyc_Absyn_Aggrdecl*_tmp66D;struct Cyc_List_List*_tmp66F;_LL2CD: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp65E=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp65D;if(_tmp65E->tag != 25)goto _LL2CF;else{_tmp65F=_tmp65E->f1;}}_LL2CE: {
# 3195
struct _tuple1*_tmp670=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp671=Cyc_Absyn_var_exp(_tmp670,0);
struct Cyc_Absyn_Stmt*_tmp672=Cyc_Toc_init_array(nv,new_e_type,_tmp671,_tmp65F,Cyc_Absyn_exp_stmt(_tmp671,0));
void*old_elt_typ;
{void*_tmp673=Cyc_Tcutil_compress(old_typ);void*_tmp674=_tmp673;void*_tmp676;struct Cyc_Absyn_Tqual _tmp677;union Cyc_Absyn_Constraint*_tmp678;_LL2DA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp675=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp674;if(_tmp675->tag != 5)goto _LL2DC;else{_tmp676=(_tmp675->f1).elt_typ;_tmp677=(_tmp675->f1).elt_tq;_tmp678=((_tmp675->f1).ptr_atts).zero_term;}}_LL2DB:
# 3201
 old_elt_typ=_tmp676;goto _LL2D9;_LL2DC:;_LL2DD: {
# 3203
const char*_tmpCF0;void*_tmpCEF;old_elt_typ=(
(_tmpCEF=0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCF0="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(_tmpCF0,sizeof(char),52))),_tag_dyneither(_tmpCEF,sizeof(void*),0))));}_LL2D9:;}{
# 3206
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*_tmp67B=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp67C=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),
Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp65F),0),0);
struct Cyc_Absyn_Exp*e1;
if(_tmp54A == 0  || Cyc_Absyn_no_regions)
e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp67C);else{
# 3214
struct Cyc_Absyn_Exp*r=_tmp54A;
Cyc_Toc_exp_to_c(nv,r);
e1=Cyc_Toc_rmalloc_exp(r,_tmp67C);}
# 3219
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp670,_tmp67B,e1,_tmp672,0));
goto _LL2CC;};}_LL2CF: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp660=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp65D;if(_tmp660->tag != 26)goto _LL2D1;else{_tmp661=_tmp660->f1;_tmp662=_tmp660->f2;_tmp663=_tmp660->f3;_tmp664=_tmp660->f4;}}_LL2D0:
# 3223
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp54A,old_typ,_tmp662,(void*)_check_null(_tmp663->topt),_tmp664,_tmp663,_tmp661);
goto _LL2CC;_LL2D1: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp665=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp65D;if(_tmp665->tag != 27)goto _LL2D3;else{_tmp666=_tmp665->f1;_tmp667=(void*)_tmp665->f2;_tmp668=_tmp665->f3;}}_LL2D2:
# 3227
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp54A,old_typ,_tmp666,_tmp667,_tmp668,0,0);
goto _LL2CC;_LL2D3: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp669=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp65D;if(_tmp669->tag != 28)goto _LL2D5;else{_tmp66A=_tmp669->f1;_tmp66B=_tmp669->f2;_tmp66C=_tmp669->f3;_tmp66D=_tmp669->f4;}}_LL2D4: {
# 3232
struct Cyc_Absyn_Exp*_tmp67D=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp54B->topt),_tmp66B,1,_tmp54A,_tmp66C,_tmp66A);
e->r=_tmp67D->r;
e->topt=_tmp67D->topt;
goto _LL2CC;}_LL2D5: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp66E=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp65D;if(_tmp66E->tag != 23)goto _LL2D7;else{_tmp66F=_tmp66E->f1;}}_LL2D6:
# 3238
 e->r=(Cyc_Toc_init_tuple(nv,1,_tmp54A,_tmp66F))->r;
goto _LL2CC;_LL2D7:;_LL2D8: {
# 3245
void*old_elt_typ=(void*)_check_null(_tmp54B->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
# 3248
struct _tuple1*_tmp67E=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp67F=Cyc_Absyn_var_exp(_tmp67E,0);
struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp67F,0),0);
struct Cyc_Absyn_Exp*inner_mexp=mexp;
if(_tmp54A == 0  || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
# 3255
struct Cyc_Absyn_Exp*r=_tmp54A;
Cyc_Toc_exp_to_c(nv,r);
mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{
# 3263
int done=0;
{void*_tmp680=_tmp54B->r;void*_tmp681=_tmp680;void*_tmp683;struct Cyc_Absyn_Exp*_tmp684;_LL2DF: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp682=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp681;if(_tmp682->tag != 13)goto _LL2E1;else{_tmp683=(void*)_tmp682->f1;_tmp684=_tmp682->f2;}}_LL2E0:
# 3266
{struct _tuple29 _tmpCF1;struct _tuple29 _tmp685=(_tmpCF1.f1=Cyc_Tcutil_compress(_tmp683),((_tmpCF1.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp684->topt)),_tmpCF1)));struct _tuple29 _tmp686=_tmp685;void*_tmp688;union Cyc_Absyn_Constraint*_tmp689;union Cyc_Absyn_Constraint*_tmp68B;_LL2E4:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp687=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp686.f1;if(_tmp687->tag != 5)goto _LL2E6;else{_tmp688=(_tmp687->f1).elt_typ;_tmp689=((_tmp687->f1).ptr_atts).bounds;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp68A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp686.f2;if(_tmp68A->tag != 5)goto _LL2E6;else{_tmp68B=((_tmp68A->f1).ptr_atts).bounds;}};_LL2E5:
# 3269
{struct _tuple29 _tmpCF2;struct _tuple29 _tmp68C=(_tmpCF2.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp689),((_tmpCF2.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp68B),_tmpCF2)));struct _tuple29 _tmp68D=_tmp68C;struct Cyc_Absyn_Exp*_tmp690;_LL2E9:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp68E=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp68D.f1;if(_tmp68E->tag != 0)goto _LL2EB;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp68F=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp68D.f2;if(_tmp68F->tag != 1)goto _LL2EB;else{_tmp690=_tmp68F->f1;}};_LL2EA:
# 3271
 Cyc_Toc_exp_to_c(nv,_tmp684);
inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);
done=1;{
struct Cyc_Absyn_Exp*_tmp691=Cyc_Toc__init_dyneither_ptr_e;
{struct Cyc_Absyn_Exp*_tmpCF3[4];e->r=Cyc_Toc_fncall_exp_r(_tmp691,(
(_tmpCF3[3]=_tmp690,((_tmpCF3[2]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp688),0),((_tmpCF3[1]=_tmp684,((_tmpCF3[0]=mexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCF3,sizeof(struct Cyc_Absyn_Exp*),4)))))))))));}
# 3279
goto _LL2E8;};_LL2EB:;_LL2EC:
 goto _LL2E8;_LL2E8:;}
# 3282
goto _LL2E3;_LL2E6:;_LL2E7:
 goto _LL2E3;_LL2E3:;}
# 3285
goto _LL2DE;_LL2E1:;_LL2E2:
 goto _LL2DE;_LL2DE:;}
# 3288
if(!done){
struct Cyc_Absyn_Stmt*_tmp695=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp67F),0);
struct Cyc_Absyn_Exp*_tmp696=Cyc_Absyn_signed_int_exp(0,0);
Cyc_Toc_exp_to_c(nv,_tmp54B);
_tmp695=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp67F,_tmp696,0),_tmp54B,0),_tmp695,0);{
# 3294
void*_tmp697=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp67E,_tmp697,mexp,_tmp695,0));};}
# 3297
goto _LL2CC;};}_LL2CC:;}
# 3299
goto _LL217;};_LL23C: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp54C=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp54C->tag != 17)goto _LL23E;else{_tmp54D=_tmp54C->f1;}}_LL23D: {
# 3302
int _tmp698=Cyc_Toc_set_in_sizeof(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp54D);
Cyc_Toc_set_in_sizeof(nv,_tmp698);
goto _LL217;}_LL23E: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp54E=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp54E->tag != 16)goto _LL240;else{_tmp54F=(void*)_tmp54E->f1;}}_LL23F:
{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpCF6;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpCF5;e->r=(void*)((_tmpCF5=_cycalloc(sizeof(*_tmpCF5)),((_tmpCF5[0]=((_tmpCF6.tag=16,((_tmpCF6.f1=Cyc_Toc_typ_to_c(_tmp54F),_tmpCF6)))),_tmpCF5))));}goto _LL217;_LL240: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp550=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp550->tag != 18)goto _LL242;else{_tmp551=(void*)_tmp550->f1;_tmp552=_tmp550->f2;}}_LL241: {
# 3308
void*_tmp69B=_tmp551;
struct Cyc_List_List*_tmp69C=_tmp552;
for(0;_tmp69C != 0;_tmp69C=_tmp69C->tl){
void*_tmp69D=(void*)_tmp69C->hd;void*_tmp69E=_tmp69D;struct _dyneither_ptr*_tmp6A0;unsigned int _tmp6A2;_LL2EE: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp69F=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp69E;if(_tmp69F->tag != 0)goto _LL2F0;else{_tmp6A0=_tmp69F->f1;}}_LL2EF:
 goto _LL2ED;_LL2F0: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp6A1=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp69E;if(_tmp6A1->tag != 1)goto _LL2ED;else{_tmp6A2=_tmp6A1->f1;}}_LL2F1:
# 3314
{void*_tmp6A3=Cyc_Tcutil_compress(_tmp69B);void*_tmp6A4=_tmp6A3;union Cyc_Absyn_AggrInfoU _tmp6A6;struct Cyc_List_List*_tmp6A8;struct Cyc_List_List*_tmp6AA;struct Cyc_Absyn_Datatypefield*_tmp6AC;_LL2F3: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp6A5=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp6A4;if(_tmp6A5->tag != 11)goto _LL2F5;else{_tmp6A6=(_tmp6A5->f1).aggr_info;}}_LL2F4: {
# 3316
struct Cyc_Absyn_Aggrdecl*_tmp6AD=Cyc_Absyn_get_known_aggrdecl(_tmp6A6);
if(_tmp6AD->impl == 0){
const char*_tmpCF9;void*_tmpCF8;(_tmpCF8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCF9="struct fields must be known",_tag_dyneither(_tmpCF9,sizeof(char),28))),_tag_dyneither(_tmpCF8,sizeof(void*),0)));}
_tmp6A8=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6AD->impl))->fields;goto _LL2F6;}_LL2F5: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp6A7=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6A4;if(_tmp6A7->tag != 12)goto _LL2F7;else{_tmp6A8=_tmp6A7->f2;}}_LL2F6: {
# 3321
struct Cyc_Absyn_Aggrfield*_tmp6B0=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp6A8,(int)_tmp6A2);
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpCFC;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpCFB;_tmp69C->hd=(void*)((void*)((_tmpCFB=_cycalloc(sizeof(*_tmpCFB)),((_tmpCFB[0]=((_tmpCFC.tag=0,((_tmpCFC.f1=_tmp6B0->name,_tmpCFC)))),_tmpCFB)))));}
_tmp69B=_tmp6B0->type;
goto _LL2F2;}_LL2F7: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6A9=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6A4;if(_tmp6A9->tag != 10)goto _LL2F9;else{_tmp6AA=_tmp6A9->f1;}}_LL2F8:
# 3326
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpCFF;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpCFE;_tmp69C->hd=(void*)((void*)((_tmpCFE=_cycalloc(sizeof(*_tmpCFE)),((_tmpCFE[0]=((_tmpCFF.tag=0,((_tmpCFF.f1=Cyc_Absyn_fieldname((int)(_tmp6A2 + 1)),_tmpCFF)))),_tmpCFE)))));}
_tmp69B=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp6AA,(int)_tmp6A2)).f2;
goto _LL2F2;_LL2F9: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp6AB=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp6A4;if(_tmp6AB->tag != 4)goto _LL2FB;else{if((((_tmp6AB->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2FB;_tmp6AC=((struct _tuple2)(((_tmp6AB->f1).field_info).KnownDatatypefield).val).f2;}}_LL2FA:
# 3330
 if(_tmp6A2 == 0){
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpD02;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpD01;_tmp69C->hd=(void*)((void*)((_tmpD01=_cycalloc(sizeof(*_tmpD01)),((_tmpD01[0]=((_tmpD02.tag=0,((_tmpD02.f1=Cyc_Toc_tag_sp,_tmpD02)))),_tmpD01)))));}else{
# 3333
_tmp69B=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp6AC->typs,(int)(_tmp6A2 - 1))).f2;{
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpD05;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpD04;_tmp69C->hd=(void*)((void*)((_tmpD04=_cycalloc(sizeof(*_tmpD04)),((_tmpD04[0]=((_tmpD05.tag=0,((_tmpD05.f1=Cyc_Absyn_fieldname((int)_tmp6A2),_tmpD05)))),_tmpD04)))));};}
# 3336
goto _LL2F2;_LL2FB:;_LL2FC:
{const char*_tmpD08;void*_tmpD07;(_tmpD07=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD08="impossible type for offsetof tuple index",_tag_dyneither(_tmpD08,sizeof(char),41))),_tag_dyneither(_tmpD07,sizeof(void*),0)));}
goto _LL2F2;_LL2F2:;}
# 3340
goto _LL2ED;_LL2ED:;}
# 3343
{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpD0B;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpD0A;e->r=(void*)((_tmpD0A=_cycalloc(sizeof(*_tmpD0A)),((_tmpD0A[0]=((_tmpD0B.tag=18,((_tmpD0B.f1=Cyc_Toc_typ_to_c(_tmp551),((_tmpD0B.f2=_tmp552,_tmpD0B)))))),_tmpD0A))));}
goto _LL217;}_LL242: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp553=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp553->tag != 19)goto _LL244;else{_tmp554=_tmp553->f1;}}_LL243: {
# 3346
int _tmp6BD=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp6BE=Cyc_Tcutil_compress((void*)_check_null(_tmp554->topt));
{void*_tmp6BF=_tmp6BE;void*_tmp6C1;struct Cyc_Absyn_Tqual _tmp6C2;void*_tmp6C3;union Cyc_Absyn_Constraint*_tmp6C4;union Cyc_Absyn_Constraint*_tmp6C5;union Cyc_Absyn_Constraint*_tmp6C6;_LL2FE: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6C0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6BF;if(_tmp6C0->tag != 5)goto _LL300;else{_tmp6C1=(_tmp6C0->f1).elt_typ;_tmp6C2=(_tmp6C0->f1).elt_tq;_tmp6C3=((_tmp6C0->f1).ptr_atts).rgn;_tmp6C4=((_tmp6C0->f1).ptr_atts).nullable;_tmp6C5=((_tmp6C0->f1).ptr_atts).bounds;_tmp6C6=((_tmp6C0->f1).ptr_atts).zero_term;}}_LL2FF:
# 3351
{void*_tmp6C7=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6C5);void*_tmp6C8=_tmp6C7;struct Cyc_Absyn_Exp*_tmp6CA;_LL303: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6C9=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6C8;if(_tmp6C9->tag != 1)goto _LL305;else{_tmp6CA=_tmp6C9->f1;}}_LL304: {
# 3353
int do_null_check=!Cyc_Toc_in_sizeof(nv) && Cyc_Toc_need_null_check(_tmp554);
Cyc_Toc_exp_to_c(nv,_tmp554);
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpD0E;void*_tmpD0D;(_tmpD0D=0,Cyc_Tcutil_warn(e->loc,((_tmpD0E="inserted null check due to dereference",_tag_dyneither(_tmpD0E,sizeof(char),39))),_tag_dyneither(_tmpD0D,sizeof(void*),0)));}{
# 3359
struct Cyc_List_List*_tmpD0F;_tmp554->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(_tmp6BE),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpD0F=_cycalloc(sizeof(*_tmpD0F)),((_tmpD0F->hd=Cyc_Absyn_copy_exp(_tmp554),((_tmpD0F->tl=0,_tmpD0F)))))),0));};}
# 3367
if(!Cyc_Toc_in_sizeof(nv) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6C6)){
struct _tuple12 _tmp6CF=Cyc_Evexp_eval_const_uint_exp(_tmp6CA);unsigned int _tmp6D1;int _tmp6D2;struct _tuple12 _tmp6D0=_tmp6CF;_tmp6D1=_tmp6D0.f1;_tmp6D2=_tmp6D0.f2;
# 3373
if(!_tmp6D2  || _tmp6D1 <= 0){
const char*_tmpD12;void*_tmpD11;(_tmpD11=0,Cyc_Tcutil_terr(e->loc,((_tmpD12="cannot determine dereference is in bounds",_tag_dyneither(_tmpD12,sizeof(char),42))),_tag_dyneither(_tmpD11,sizeof(void*),0)));}}
# 3376
goto _LL302;}_LL305: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6CB=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6C8;if(_tmp6CB->tag != 0)goto _LL302;}_LL306: {
# 3379
struct Cyc_Absyn_Exp*_tmp6D5=Cyc_Absyn_uint_exp(0,0);
_tmp6D5->topt=Cyc_Absyn_uint_typ;
e->r=Cyc_Toc_subscript_exp_r(_tmp554,_tmp6D5);
Cyc_Toc_exp_to_c(nv,e);
goto _LL302;}_LL302:;}
# 3385
goto _LL2FD;_LL300:;_LL301: {
const char*_tmpD15;void*_tmpD14;(_tmpD14=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD15="exp_to_c: Deref: non-pointer",_tag_dyneither(_tmpD15,sizeof(char),29))),_tag_dyneither(_tmpD14,sizeof(void*),0)));}_LL2FD:;}
# 3388
Cyc_Toc_set_lhs(nv,_tmp6BD);
goto _LL217;};}_LL244: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp555=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp555->tag != 20)goto _LL246;else{_tmp556=_tmp555->f1;_tmp557=_tmp555->f2;_tmp558=_tmp555->f3;_tmp559=_tmp555->f4;}}_LL245: {
# 3391
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp556->topt);
Cyc_Toc_exp_to_c(nv,_tmp556);
if(_tmp558  && _tmp559)
e->r=Cyc_Toc_check_tagged_union(_tmp556,Cyc_Toc_typ_to_c(e1_cyc_type),e1_cyc_type,_tmp557,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);
# 3399
if(is_poly)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
goto _LL217;}_LL246: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp55A=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp55A->tag != 21)goto _LL248;else{_tmp55B=_tmp55A->f1;_tmp55C=_tmp55A->f2;_tmp55D=_tmp55A->f3;_tmp55E=_tmp55A->f4;}}_LL247: {
# 3403
int _tmp6D8=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp55B->topt));
int do_null_check=Cyc_Toc_need_null_check(_tmp55B);
Cyc_Toc_exp_to_c(nv,_tmp55B);{
int is_poly=Cyc_Toc_is_poly_project(e);
struct Cyc_Absyn_PtrInfo _tmp6D9=Cyc_Toc_get_ptr_type(e1typ);void*_tmp6DB;struct Cyc_Absyn_Tqual _tmp6DC;void*_tmp6DD;union Cyc_Absyn_Constraint*_tmp6DE;union Cyc_Absyn_Constraint*_tmp6DF;union Cyc_Absyn_Constraint*_tmp6E0;struct Cyc_Absyn_PtrInfo _tmp6DA=_tmp6D9;_tmp6DB=_tmp6DA.elt_typ;_tmp6DC=_tmp6DA.elt_tq;_tmp6DD=(_tmp6DA.ptr_atts).rgn;_tmp6DE=(_tmp6DA.ptr_atts).nullable;_tmp6DF=(_tmp6DA.ptr_atts).bounds;_tmp6E0=(_tmp6DA.ptr_atts).zero_term;
{void*_tmp6E1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6DF);void*_tmp6E2=_tmp6E1;struct Cyc_Absyn_Exp*_tmp6E4;_LL308: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6E3=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6E2;if(_tmp6E3->tag != 1)goto _LL30A;else{_tmp6E4=_tmp6E3->f1;}}_LL309: {
# 3414
struct _tuple12 _tmp6E6=Cyc_Evexp_eval_const_uint_exp(_tmp6E4);unsigned int _tmp6E8;int _tmp6E9;struct _tuple12 _tmp6E7=_tmp6E6;_tmp6E8=_tmp6E7.f1;_tmp6E9=_tmp6E7.f2;
if(_tmp6E9){
if(_tmp6E8 < 1){
const char*_tmpD18;void*_tmpD17;(_tmpD17=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD18="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpD18,sizeof(char),44))),_tag_dyneither(_tmpD17,sizeof(void*),0)));}
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpD1B;void*_tmpD1A;(_tmpD1A=0,Cyc_Tcutil_warn(e->loc,((_tmpD1B="inserted null check due to dereference",_tag_dyneither(_tmpD1B,sizeof(char),39))),_tag_dyneither(_tmpD1A,sizeof(void*),0)));}{
# 3422
struct Cyc_Absyn_Exp*_tmpD1C[1];_tmp55B->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpD1C[0]=Cyc_Absyn_new_exp(_tmp55B->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD1C,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}}else{
# 3427
if(!Cyc_Evexp_c_can_eval(_tmp6E4)){
const char*_tmpD1F;void*_tmpD1E;(_tmpD1E=0,Cyc_Tcutil_terr(e->loc,((_tmpD1F="cannot determine pointer dereference in bounds",_tag_dyneither(_tmpD1F,sizeof(char),47))),_tag_dyneither(_tmpD1E,sizeof(void*),0)));}{
# 3431
struct Cyc_Absyn_Exp*_tmpD20[4];_tmp55B->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpD20[3]=
# 3435
Cyc_Absyn_uint_exp(0,0),((_tmpD20[2]=
# 3434
Cyc_Absyn_sizeoftyp_exp(_tmp6DB,0),((_tmpD20[1]=_tmp6E4,((_tmpD20[0]=
# 3433
Cyc_Absyn_new_exp(_tmp55B->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD20,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0));};}
# 3437
goto _LL307;}_LL30A: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6E5=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6E2;if(_tmp6E5->tag != 0)goto _LL307;}_LL30B: {
# 3440
void*ta1=Cyc_Toc_typ_to_c(_tmp6DB);
{struct Cyc_Absyn_Exp*_tmpD21[3];_tmp55B->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp6DC),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_dyneither_subscript_e,(
(_tmpD21[2]=
# 3445
Cyc_Absyn_uint_exp(0,0),((_tmpD21[1]=
# 3444
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpD21[0]=
# 3443
Cyc_Absyn_new_exp(_tmp55B->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD21,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}
# 3448
goto _LL307;}_LL307:;}
# 3450
if(_tmp55D  && _tmp55E)
e->r=Cyc_Toc_check_tagged_union(_tmp55B,Cyc_Toc_typ_to_c(e1typ),_tmp6DB,_tmp55C,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);
# 3453
if(is_poly  && _tmp55E)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
Cyc_Toc_set_lhs(nv,_tmp6D8);
goto _LL217;};};}_LL248: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp55F=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp55F->tag != 22)goto _LL24A;else{_tmp560=_tmp55F->f1;_tmp561=_tmp55F->f2;}}_LL249: {
# 3458
int _tmp6F3=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp6F4=Cyc_Tcutil_compress((void*)_check_null(_tmp560->topt));
# 3462
{void*_tmp6F5=_tmp6F4;struct Cyc_List_List*_tmp6F7;void*_tmp6F9;struct Cyc_Absyn_Tqual _tmp6FA;void*_tmp6FB;union Cyc_Absyn_Constraint*_tmp6FC;union Cyc_Absyn_Constraint*_tmp6FD;union Cyc_Absyn_Constraint*_tmp6FE;_LL30D: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6F6=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6F5;if(_tmp6F6->tag != 10)goto _LL30F;else{_tmp6F7=_tmp6F6->f1;}}_LL30E:
# 3465
 Cyc_Toc_exp_to_c(nv,_tmp560);
Cyc_Toc_exp_to_c(nv,_tmp561);{
struct _tuple12 _tmp6FF=Cyc_Evexp_eval_const_uint_exp(_tmp561);unsigned int _tmp701;int _tmp702;struct _tuple12 _tmp700=_tmp6FF;_tmp701=_tmp700.f1;_tmp702=_tmp700.f2;
if(!_tmp702){
const char*_tmpD24;void*_tmpD23;(_tmpD23=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD24="unknown tuple subscript in translation to C",_tag_dyneither(_tmpD24,sizeof(char),44))),_tag_dyneither(_tmpD23,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp560,Cyc_Absyn_fieldname((int)(_tmp701 + 1)));
goto _LL30C;};_LL30F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6F8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6F5;if(_tmp6F8->tag != 5)goto _LL311;else{_tmp6F9=(_tmp6F8->f1).elt_typ;_tmp6FA=(_tmp6F8->f1).elt_tq;_tmp6FB=((_tmp6F8->f1).ptr_atts).rgn;_tmp6FC=((_tmp6F8->f1).ptr_atts).nullable;_tmp6FD=((_tmp6F8->f1).ptr_atts).bounds;_tmp6FE=((_tmp6F8->f1).ptr_atts).zero_term;}}_LL310: {
# 3473
struct Cyc_List_List*_tmp705=Cyc_Toc_get_relns(_tmp560);
int _tmp706=Cyc_Toc_need_null_check(_tmp560);
int _tmp707=Cyc_Toc_in_sizeof(nv);
# 3481
int in_bnds=_tmp707;
{void*_tmp708=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6FD);void*_tmp709=_tmp708;_LL314:;_LL315:
# 3484
 in_bnds=in_bnds  || Cyc_Toc_check_bounds(_tmp6F4,_tmp705,_tmp560,_tmp561);
if(Cyc_Toc_warn_bounds_checks  && !in_bnds){
const char*_tmpD28;void*_tmpD27[1];struct Cyc_String_pa_PrintArg_struct _tmpD26;(_tmpD26.tag=0,((_tmpD26.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpD27[0]=& _tmpD26,Cyc_Tcutil_warn(e->loc,((_tmpD28="bounds check necessary for %s",_tag_dyneither(_tmpD28,sizeof(char),30))),_tag_dyneither(_tmpD27,sizeof(void*),1)))))));}_LL313:;}
# 3491
Cyc_Toc_exp_to_c(nv,_tmp560);
Cyc_Toc_exp_to_c(nv,_tmp561);
++ Cyc_Toc_total_bounds_checks;
{void*_tmp70D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6FD);void*_tmp70E=_tmp70D;struct Cyc_Absyn_Exp*_tmp710;_LL317: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp70F=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp70E;if(_tmp70F->tag != 1)goto _LL319;else{_tmp710=_tmp70F->f1;}}_LL318: {
# 3496
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6FC) && _tmp706;
void*ta1=Cyc_Toc_typ_to_c(_tmp6F9);
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp6FA);
if(in_bnds  && !possibly_null)
++ Cyc_Toc_bounds_checks_eliminated;else{
if(in_bnds  && possibly_null){
++ Cyc_Toc_bounds_checks_eliminated;
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpD2B;void*_tmpD2A;(_tmpD2A=0,Cyc_Tcutil_warn(e->loc,((_tmpD2B="inserted null check due to dereference",_tag_dyneither(_tmpD2B,sizeof(char),39))),_tag_dyneither(_tmpD2A,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpD2C[1];_tmp560->r=Cyc_Toc_cast_it_r(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpD2C[0]=Cyc_Absyn_copy_exp(_tmp560),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD2C,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}else{
# 3508
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6FE)){
# 3510
if(!Cyc_Evexp_c_can_eval(_tmp710)){
const char*_tmpD2F;void*_tmpD2E;(_tmpD2E=0,Cyc_Tcutil_terr(e->loc,((_tmpD2F="cannot determine subscript is in bounds",_tag_dyneither(_tmpD2F,sizeof(char),40))),_tag_dyneither(_tmpD2E,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp560);
struct Cyc_Absyn_Exp*_tmpD30[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(function_e,(
(_tmpD30[2]=_tmp561,((_tmpD30[1]=_tmp710,((_tmpD30[0]=_tmp560,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD30,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp710)){
const char*_tmpD33;void*_tmpD32;(_tmpD32=0,Cyc_Tcutil_terr(e->loc,((_tmpD33="cannot determine subscript is in bounds",_tag_dyneither(_tmpD33,sizeof(char),40))),_tag_dyneither(_tmpD32,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpD36;void*_tmpD35;(_tmpD35=0,Cyc_Tcutil_warn(e->loc,((_tmpD36="inserted null check due to dereference",_tag_dyneither(_tmpD36,sizeof(char),39))),_tag_dyneither(_tmpD35,sizeof(void*),0)));}{
# 3522
struct Cyc_Absyn_Exp*_tmpD37[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpD37[3]=_tmp561,((_tmpD37[2]=
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpD37[1]=_tmp710,((_tmpD37[0]=_tmp560,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD37,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{
# 3528
if(!Cyc_Evexp_c_can_eval(_tmp710)){
const char*_tmpD3A;void*_tmpD39;(_tmpD39=0,Cyc_Tcutil_terr(e->loc,((_tmpD3A="cannot determine subscript is in bounds",_tag_dyneither(_tmpD3A,sizeof(char),40))),_tag_dyneither(_tmpD39,sizeof(void*),0)));}{
# 3531
struct Cyc_Absyn_Exp*_tmpD3B[2];_tmp561->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,(
(_tmpD3B[1]=Cyc_Absyn_copy_exp(_tmp561),((_tmpD3B[0]=_tmp710,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD3B,sizeof(struct Cyc_Absyn_Exp*),2)))))));};}}}}
# 3534
goto _LL316;}_LL319: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp711=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp70E;if(_tmp711->tag != 0)goto _LL316;}_LL31A: {
# 3536
void*ta1=Cyc_Toc_typ_to_c(_tmp6F9);
if(in_bnds){
# 3540
++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6FA),
Cyc_Toc_member_exp(_tmp560,Cyc_Toc_curr_sp,0)),_tmp561);}else{
# 3545
struct Cyc_Absyn_Exp*_tmp720=Cyc_Toc__check_dyneither_subscript_e;
struct Cyc_Absyn_Exp*_tmpD3C[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6FA),
Cyc_Absyn_fncall_exp(_tmp720,(
(_tmpD3C[2]=_tmp561,((_tmpD3C[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpD3C[0]=_tmp560,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD3C,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}
# 3551
goto _LL316;}_LL316:;}
# 3553
goto _LL30C;}_LL311:;_LL312: {
const char*_tmpD3F;void*_tmpD3E;(_tmpD3E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD3F="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(_tmpD3F,sizeof(char),49))),_tag_dyneither(_tmpD3E,sizeof(void*),0)));}_LL30C:;}
# 3556
Cyc_Toc_set_lhs(nv,_tmp6F3);
goto _LL217;};}_LL24A: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp562=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp562->tag != 23)goto _LL24C;else{_tmp563=_tmp562->f1;}}_LL24B:
# 3559
 if(!Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp563))->r;else{
# 3564
struct Cyc_List_List*_tmp724=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp563);
void*_tmp725=Cyc_Toc_add_tuple_type(_tmp724);
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp563 != 0;(_tmp563=_tmp563->tl,i ++)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp563->hd);{
struct _tuple19*_tmpD42;struct Cyc_List_List*_tmpD41;dles=((_tmpD41=_cycalloc(sizeof(*_tmpD41)),((_tmpD41->hd=((_tmpD42=_cycalloc(sizeof(*_tmpD42)),((_tmpD42->f1=0,((_tmpD42->f2=(struct Cyc_Absyn_Exp*)_tmp563->hd,_tmpD42)))))),((_tmpD41->tl=dles,_tmpD41))))));};}}
# 3571
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}
# 3574
goto _LL217;_LL24C: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp564=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp564->tag != 25)goto _LL24E;else{_tmp565=_tmp564->f1;}}_LL24D:
# 3578
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp565);
{struct Cyc_List_List*_tmp728=_tmp565;for(0;_tmp728 != 0;_tmp728=_tmp728->tl){
struct _tuple19*_tmp729=(struct _tuple19*)_tmp728->hd;struct Cyc_Absyn_Exp*_tmp72B;struct _tuple19*_tmp72A=_tmp729;_tmp72B=_tmp72A->f2;
Cyc_Toc_exp_to_c(nv,_tmp72B);}}
# 3583
goto _LL217;_LL24E: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp566=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp566->tag != 26)goto _LL250;else{_tmp567=_tmp566->f1;_tmp568=_tmp566->f2;_tmp569=_tmp566->f3;_tmp56A=_tmp566->f4;}}_LL24F: {
# 3587
struct _tuple12 _tmp72C=Cyc_Evexp_eval_const_uint_exp(_tmp568);unsigned int _tmp72E;int _tmp72F;struct _tuple12 _tmp72D=_tmp72C;_tmp72E=_tmp72D.f1;_tmp72F=_tmp72D.f2;{
void*_tmp730=Cyc_Toc_typ_to_c((void*)_check_null(_tmp569->topt));
Cyc_Toc_exp_to_c(nv,_tmp569);{
struct Cyc_List_List*es=0;
# 3592
if(!Cyc_Toc_is_zero(_tmp569)){
if(!_tmp72F){
const char*_tmpD45;void*_tmpD44;(_tmpD44=0,Cyc_Tcutil_terr(_tmp568->loc,((_tmpD45="cannot determine value of constant",_tag_dyneither(_tmpD45,sizeof(char),35))),_tag_dyneither(_tmpD44,sizeof(void*),0)));}
{unsigned int i=0;for(0;i < _tmp72E;++ i){
struct _tuple19*_tmpD48;struct Cyc_List_List*_tmpD47;es=((_tmpD47=_cycalloc(sizeof(*_tmpD47)),((_tmpD47->hd=((_tmpD48=_cycalloc(sizeof(*_tmpD48)),((_tmpD48->f1=0,((_tmpD48->f2=_tmp569,_tmpD48)))))),((_tmpD47->tl=es,_tmpD47))))));}}
# 3598
if(_tmp56A){
struct Cyc_Absyn_Exp*_tmp735=Cyc_Toc_cast_it(_tmp730,Cyc_Absyn_uint_exp(0,0));
struct _tuple19*_tmpD4B;struct Cyc_List_List*_tmpD4A;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,((_tmpD4A=_cycalloc(sizeof(*_tmpD4A)),((_tmpD4A->hd=((_tmpD4B=_cycalloc(sizeof(*_tmpD4B)),((_tmpD4B->f1=0,((_tmpD4B->f2=_tmp735,_tmpD4B)))))),((_tmpD4A->tl=0,_tmpD4A)))))));}}
# 3603
e->r=Cyc_Toc_unresolvedmem_exp_r(0,es);
goto _LL217;};};}_LL250: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp56B=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp56B->tag != 27)goto _LL252;else{_tmp56C=_tmp56B->f1;_tmp56D=(void*)_tmp56B->f2;_tmp56E=_tmp56B->f3;}}_LL251:
# 3608
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,0);
goto _LL217;_LL252: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp56F=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp56F->tag != 28)goto _LL254;else{_tmp570=_tmp56F->f1;_tmp571=_tmp56F->f2;_tmp572=_tmp56F->f3;_tmp573=_tmp56F->f4;}}_LL253:
# 3613
 if(!Cyc_Toc_is_toplevel(nv)){
struct Cyc_Absyn_Exp*_tmp738=Cyc_Toc_init_struct(nv,old_typ,_tmp571,0,0,_tmp572,_tmp570);
e->r=_tmp738->r;
e->topt=_tmp738->topt;}else{
# 3624
if(_tmp573 == 0){
const char*_tmpD4E;void*_tmpD4D;(_tmpD4D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD4E="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpD4E,sizeof(char),38))),_tag_dyneither(_tmpD4D,sizeof(void*),0)));}{
struct Cyc_Absyn_Aggrdecl*sd2=_tmp573;
# 3628
struct _RegionHandle _tmp73B=_new_region("rgn");struct _RegionHandle*rgn=& _tmp73B;_push_region(rgn);
{struct Cyc_List_List*_tmp73C=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp572,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
# 3632
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp573->impl))->tagged){
# 3634
struct _tuple30*_tmp73D=(struct _tuple30*)((struct Cyc_List_List*)_check_null(_tmp73C))->hd;struct Cyc_Absyn_Aggrfield*_tmp73F;struct Cyc_Absyn_Exp*_tmp740;struct _tuple30*_tmp73E=_tmp73D;_tmp73F=_tmp73E->f1;_tmp740=_tmp73E->f2;{
void*_tmp741=(void*)_check_null(_tmp740->topt);
void*_tmp742=_tmp73F->type;
Cyc_Toc_exp_to_c(nv,_tmp740);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp742) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp741))
_tmp740->r=
Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp740->r,0));{
# 3643
int i=Cyc_Toc_get_member_offset(_tmp573,_tmp73F->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct _tuple19*_tmpD53;struct _tuple19*_tmpD52;struct _tuple19*_tmpD51[2];struct Cyc_List_List*_tmp743=(_tmpD51[1]=((_tmpD52=_cycalloc(sizeof(*_tmpD52)),((_tmpD52->f1=0,((_tmpD52->f2=_tmp740,_tmpD52)))))),((_tmpD51[0]=((_tmpD53=_cycalloc(sizeof(*_tmpD53)),((_tmpD53->f1=0,((_tmpD53->f2=field_tag_exp,_tmpD53)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD51,sizeof(struct _tuple19*),2)))));
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp743,0);
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpD59;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpD58;void*_tmpD57[1];struct Cyc_List_List*ds=(_tmpD57[0]=(void*)((_tmpD59=_cycalloc(sizeof(*_tmpD59)),((_tmpD59[0]=((_tmpD58.tag=1,((_tmpD58.f1=_tmp73F->name,_tmpD58)))),_tmpD59)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD57,sizeof(void*),1)));
struct _tuple19*_tmpD5C;struct _tuple19*_tmpD5B[1];struct Cyc_List_List*dles=(_tmpD5B[0]=((_tmpD5C=_cycalloc(sizeof(*_tmpD5C)),((_tmpD5C->f1=ds,((_tmpD5C->f2=umem,_tmpD5C)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD5B,sizeof(struct _tuple19*),1)));
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{
# 3652
struct Cyc_List_List*_tmp74C=0;
struct Cyc_List_List*_tmp74D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields;
for(0;_tmp74D != 0;_tmp74D=_tmp74D->tl){
struct Cyc_List_List*_tmp74E=_tmp73C;for(0;_tmp74E != 0;_tmp74E=_tmp74E->tl){
if((*((struct _tuple30*)_tmp74E->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp74D->hd){
struct _tuple30*_tmp74F=(struct _tuple30*)_tmp74E->hd;struct Cyc_Absyn_Aggrfield*_tmp751;struct Cyc_Absyn_Exp*_tmp752;struct _tuple30*_tmp750=_tmp74F;_tmp751=_tmp750->f1;_tmp752=_tmp750->f2;{
void*_tmp753=Cyc_Toc_typ_to_c(_tmp751->type);
void*_tmp754=Cyc_Toc_typ_to_c((void*)_check_null(_tmp752->topt));
Cyc_Toc_exp_to_c(nv,_tmp752);
# 3662
if(!Cyc_Tcutil_unify(_tmp753,_tmp754)){
# 3664
if(!Cyc_Tcutil_is_arithmetic_type(_tmp753) || !
Cyc_Tcutil_is_arithmetic_type(_tmp754))
_tmp752=Cyc_Toc_cast_it(_tmp753,Cyc_Absyn_copy_exp(_tmp752));}
{struct _tuple19*_tmpD5F;struct Cyc_List_List*_tmpD5E;_tmp74C=((_tmpD5E=_cycalloc(sizeof(*_tmpD5E)),((_tmpD5E->hd=((_tmpD5F=_cycalloc(sizeof(*_tmpD5F)),((_tmpD5F->f1=0,((_tmpD5F->f2=_tmp752,_tmpD5F)))))),((_tmpD5E->tl=_tmp74C,_tmpD5E))))));}
break;};}}}
# 3671
e->r=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp74C));}}
# 3629
;_pop_region(rgn);};}
# 3675
goto _LL217;_LL254: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp574=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp574->tag != 29)goto _LL256;else{_tmp575=(void*)_tmp574->f1;_tmp576=_tmp574->f2;}}_LL255: {
# 3677
struct Cyc_List_List*fs;
{void*_tmp757=Cyc_Tcutil_compress(_tmp575);void*_tmp758=_tmp757;struct Cyc_List_List*_tmp75A;_LL31C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp759=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp758;if(_tmp759->tag != 12)goto _LL31E;else{_tmp75A=_tmp759->f2;}}_LL31D:
 fs=_tmp75A;goto _LL31B;_LL31E:;_LL31F: {
const char*_tmpD63;void*_tmpD62[1];struct Cyc_String_pa_PrintArg_struct _tmpD61;(_tmpD61.tag=0,((_tmpD61.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp575)),((_tmpD62[0]=& _tmpD61,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD63="anon struct has type %s",_tag_dyneither(_tmpD63,sizeof(char),24))),_tag_dyneither(_tmpD62,sizeof(void*),1)))))));}_LL31B:;}{
# 3682
struct _RegionHandle _tmp75E=_new_region("rgn");struct _RegionHandle*rgn=& _tmp75E;_push_region(rgn);{
struct Cyc_List_List*_tmp75F=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp576,Cyc_Absyn_StructA,fs);
for(0;_tmp75F != 0;_tmp75F=_tmp75F->tl){
struct _tuple30*_tmp760=(struct _tuple30*)_tmp75F->hd;struct Cyc_Absyn_Aggrfield*_tmp762;struct Cyc_Absyn_Exp*_tmp763;struct _tuple30*_tmp761=_tmp760;_tmp762=_tmp761->f1;_tmp763=_tmp761->f2;{
void*_tmp764=(void*)_check_null(_tmp763->topt);
void*_tmp765=_tmp762->type;
Cyc_Toc_exp_to_c(nv,_tmp763);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp765) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp764))
_tmp763->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp763->r,0));};}
# 3696
e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp576);}
# 3698
_npop_handler(0);goto _LL217;
# 3682
;_pop_region(rgn);};}_LL256: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp577=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp577->tag != 30)goto _LL258;else{_tmp578=_tmp577->f1;_tmp579=_tmp577->f2;_tmp57A=_tmp577->f3;}}_LL257: {
# 3701
void*datatype_ctype;
struct Cyc_Absyn_Exp*tag_exp;
struct _tuple1*_tmp766=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp767=Cyc_Absyn_var_exp(_tmp766,0);
struct Cyc_Absyn_Exp*mem_exp;
datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp57A->name,_tmp579->name));
tag_exp=_tmp579->is_extensible?Cyc_Absyn_var_exp(_tmp57A->name,0):
 Cyc_Toc_datatype_tag(_tmp579,_tmp57A->name);
mem_exp=_tmp767;{
struct Cyc_List_List*_tmp768=_tmp57A->typs;
# 3712
if(Cyc_Toc_is_toplevel(nv)){
# 3714
struct Cyc_List_List*dles=0;
for(0;_tmp578 != 0;(_tmp578=_tmp578->tl,_tmp768=_tmp768->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp578->hd;
void*_tmp769=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp768))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp769))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct _tuple19*_tmpD66;struct Cyc_List_List*_tmpD65;dles=((_tmpD65=_cycalloc(sizeof(*_tmpD65)),((_tmpD65->hd=((_tmpD66=_cycalloc(sizeof(*_tmpD66)),((_tmpD66->f1=0,((_tmpD66->f2=cur_e,_tmpD66)))))),((_tmpD65->tl=dles,_tmpD65))))));};}
# 3725
{struct _tuple19*_tmpD69;struct Cyc_List_List*_tmpD68;dles=((_tmpD68=_cycalloc(sizeof(*_tmpD68)),((_tmpD68->hd=((_tmpD69=_cycalloc(sizeof(*_tmpD69)),((_tmpD69->f1=0,((_tmpD69->f2=tag_exp,_tmpD69)))))),((_tmpD68->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles),_tmpD68))))));}
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{
# 3731
struct Cyc_List_List*_tmpD6A;struct Cyc_List_List*_tmp76E=
(_tmpD6A=_cycalloc(sizeof(*_tmpD6A)),((_tmpD6A->hd=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Toc_tag_sp,0),tag_exp,0),((_tmpD6A->tl=0,_tmpD6A)))));
# 3734
{int i=1;for(0;_tmp578 != 0;(((_tmp578=_tmp578->tl,i ++)),_tmp768=_tmp768->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp578->hd;
void*_tmp76F=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp768))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp76F))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct Cyc_Absyn_Stmt*_tmp770=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Absyn_fieldname(i),0),cur_e,0);
# 3744
struct Cyc_List_List*_tmpD6B;_tmp76E=((_tmpD6B=_cycalloc(sizeof(*_tmpD6B)),((_tmpD6B->hd=_tmp770,((_tmpD6B->tl=_tmp76E,_tmpD6B))))));};}}{
# 3746
struct Cyc_Absyn_Stmt*_tmp772=Cyc_Absyn_exp_stmt(_tmp767,0);
struct Cyc_List_List*_tmpD6C;struct Cyc_Absyn_Stmt*_tmp773=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpD6C=_cycalloc(sizeof(*_tmpD6C)),((_tmpD6C->hd=_tmp772,((_tmpD6C->tl=_tmp76E,_tmpD6C))))))),0);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp766,datatype_ctype,0,_tmp773,0));};}
# 3750
goto _LL217;};}_LL258: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp57B=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp57B->tag != 31)goto _LL25A;}_LL259:
# 3752
 goto _LL25B;_LL25A: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp57C=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp57C->tag != 32)goto _LL25C;}_LL25B:
 goto _LL217;_LL25C: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp57D=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp57D->tag != 33)goto _LL25E;else{_tmp57E=(_tmp57D->f1).is_calloc;_tmp57F=(_tmp57D->f1).rgn;_tmp580=(_tmp57D->f1).elt_type;_tmp581=(_tmp57D->f1).num_elts;_tmp582=(_tmp57D->f1).fat_result;}}_LL25D: {
# 3756
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp580)));
Cyc_Toc_exp_to_c(nv,_tmp581);
# 3760
if(_tmp582){
struct _tuple1*_tmp776=Cyc_Toc_temp_var();
struct _tuple1*_tmp777=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp57E){
xexp=_tmp581;
if(_tmp57F != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp57F;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp776,0));}else{
# 3771
pexp=Cyc_Toc_calloc_exp(*_tmp580,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp776,0));}{
# 3773
struct Cyc_Absyn_Exp*_tmpD6D[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpD6D[2]=
Cyc_Absyn_var_exp(_tmp776,0),((_tmpD6D[1]=
# 3774
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpD6D[0]=Cyc_Absyn_var_exp(_tmp777,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD6D,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{
# 3777
if(_tmp57F != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp57F;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp776,0));}else{
# 3782
pexp=Cyc_Toc_malloc_exp(*_tmp580,Cyc_Absyn_var_exp(_tmp776,0));}{
# 3784
struct Cyc_Absyn_Exp*_tmpD6E[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((_tmpD6E[2]=
Cyc_Absyn_var_exp(_tmp776,0),((_tmpD6E[1]=
# 3784
Cyc_Absyn_uint_exp(1,0),((_tmpD6E[0]=Cyc_Absyn_var_exp(_tmp777,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD6E,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}{
# 3787
struct Cyc_Absyn_Stmt*_tmp77A=Cyc_Absyn_declare_stmt(_tmp776,Cyc_Absyn_uint_typ,_tmp581,
Cyc_Absyn_declare_stmt(_tmp777,Cyc_Absyn_cstar_typ(t_c,Cyc_Toc_mt_tq),pexp,
Cyc_Absyn_exp_stmt(rexp,0),0),0);
e->r=Cyc_Toc_stmt_exp_r(_tmp77A);};}else{
# 3792
if(_tmp57E){
if(_tmp57F != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp57F;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp581))->r;}else{
# 3798
e->r=(Cyc_Toc_calloc_exp(*_tmp580,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp581))->r;}}else{
# 3801
if(_tmp57F != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp57F;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rmalloc_exp(rgn,_tmp581))->r;}else{
# 3806
e->r=(Cyc_Toc_malloc_exp(*_tmp580,_tmp581))->r;}}}
# 3810
goto _LL217;}_LL25E: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp583=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp583->tag != 34)goto _LL260;else{_tmp584=_tmp583->f1;_tmp585=_tmp583->f2;}}_LL25F: {
# 3819
void*e1_old_typ=(void*)_check_null(_tmp584->topt);
void*e2_old_typ=(void*)_check_null(_tmp585->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ) && !Cyc_Tcutil_is_pointer_type(e1_old_typ)){
const char*_tmpD71;void*_tmpD70;(_tmpD70=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD71="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",_tag_dyneither(_tmpD71,sizeof(char),57))),_tag_dyneither(_tmpD70,sizeof(void*),0)));}{
# 3825
struct Cyc_Absyn_Exp*swap_fn;
if(Cyc_Tcutil_is_dyneither_ptr(e1_old_typ))
swap_fn=Cyc_Toc__swap_dyneither_e;else{
# 3829
swap_fn=Cyc_Toc__swap_word_e;}{
# 3833
int f1_tag=0;
void*tagged_mem_type1=(void*)& Cyc_Absyn_VoidType_val;
int e1_tmem=Cyc_Toc_is_tagged_union_project(_tmp584,& f1_tag,& tagged_mem_type1,1);
int f2_tag=0;
void*tagged_mem_type2=(void*)& Cyc_Absyn_VoidType_val;
int e2_tmem=Cyc_Toc_is_tagged_union_project(_tmp585,& f2_tag,& tagged_mem_type2,1);
# 3840
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp584);
Cyc_Toc_exp_to_c(nv,_tmp585);
Cyc_Toc_set_lhs(nv,0);
# 3846
if(e1_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp584,f1_tag,tagged_mem_type1);else{
# 3850
_tmp584=Cyc_Toc_push_address_exp(_tmp584);}
# 3852
if(e2_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp585,f2_tag,tagged_mem_type2);else{
# 3856
_tmp585=Cyc_Toc_push_address_exp(_tmp585);}
# 3858
{struct Cyc_Absyn_Exp*_tmpD72[2];e->r=Cyc_Toc_fncall_exp_r(swap_fn,((_tmpD72[1]=_tmp585,((_tmpD72[0]=_tmp584,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD72,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
# 3860
goto _LL217;};};}_LL260: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp586=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp586->tag != 37)goto _LL262;else{_tmp587=_tmp586->f1;_tmp588=_tmp586->f2;}}_LL261: {
# 3863
void*_tmp77E=Cyc_Tcutil_compress((void*)_check_null(_tmp587->topt));
Cyc_Toc_exp_to_c(nv,_tmp587);
{void*_tmp77F=_tmp77E;struct Cyc_Absyn_Aggrdecl*_tmp781;_LL321: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp780=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp77F;if(_tmp780->tag != 11)goto _LL323;else{if((((_tmp780->f1).aggr_info).KnownAggr).tag != 2)goto _LL323;_tmp781=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp780->f1).aggr_info).KnownAggr).val);}}_LL322: {
# 3867
struct Cyc_Absyn_Exp*_tmp782=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp781,_tmp588),0);
struct Cyc_Absyn_Exp*_tmp783=Cyc_Toc_member_exp(_tmp587,_tmp588,0);
struct Cyc_Absyn_Exp*_tmp784=Cyc_Toc_member_exp(_tmp783,Cyc_Toc_tag_sp,0);
e->r=(Cyc_Absyn_eq_exp(_tmp784,_tmp782,0))->r;
goto _LL320;}_LL323:;_LL324: {
const char*_tmpD75;void*_tmpD74;(_tmpD74=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD75="non-aggregate type in tagcheck",_tag_dyneither(_tmpD75,sizeof(char),31))),_tag_dyneither(_tmpD74,sizeof(void*),0)));}_LL320:;}
# 3874
goto _LL217;}_LL262: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp589=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp589->tag != 36)goto _LL264;else{_tmp58A=_tmp589->f1;}}_LL263:
 Cyc_Toc_stmt_to_c(nv,_tmp58A);goto _LL217;_LL264: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp58B=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp58B->tag != 35)goto _LL266;}_LL265: {
const char*_tmpD78;void*_tmpD77;(_tmpD77=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD78="UnresolvedMem",_tag_dyneither(_tmpD78,sizeof(char),14))),_tag_dyneither(_tmpD77,sizeof(void*),0)));}_LL266: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp58C=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp58C->tag != 24)goto _LL268;}_LL267: {
const char*_tmpD7B;void*_tmpD7A;(_tmpD7A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpD7B="compoundlit",_tag_dyneither(_tmpD7B,sizeof(char),12))),_tag_dyneither(_tmpD7A,sizeof(void*),0)));}_LL268: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp58D=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp58D->tag != 38)goto _LL26A;}_LL269: {
const char*_tmpD7E;void*_tmpD7D;(_tmpD7D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD7E="valueof(-)",_tag_dyneither(_tmpD7E,sizeof(char),11))),_tag_dyneither(_tmpD7D,sizeof(void*),0)));}_LL26A: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp58E=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp514;if(_tmp58E->tag != 39)goto _LL217;}_LL26B: {
const char*_tmpD81;void*_tmpD80;(_tmpD80=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD81="__asm__",_tag_dyneither(_tmpD81,sizeof(char),8))),_tag_dyneither(_tmpD80,sizeof(void*),0)));}_LL217:;};}
# 3911 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){
return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
# 3914
Cyc_Toc_skip_stmt_dl(),0);}
# 3917
static int Cyc_Toc_path_length(struct Cyc_Absyn_Exp*e){
void*_tmp78F=e->r;void*_tmp790=_tmp78F;struct Cyc_Absyn_Exp*_tmp792;struct Cyc_Absyn_Exp*_tmp794;struct Cyc_Absyn_Exp*_tmp796;_LL326: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp791=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp790;if(_tmp791->tag != 13)goto _LL328;else{_tmp792=_tmp791->f2;}}_LL327:
 return Cyc_Toc_path_length(_tmp792);_LL328: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp793=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp790;if(_tmp793->tag != 19)goto _LL32A;else{_tmp794=_tmp793->f1;}}_LL329:
 return 1 + Cyc_Toc_path_length(_tmp794);_LL32A: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp795=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp790;if(_tmp795->tag != 20)goto _LL32C;else{_tmp796=_tmp795->f1;}}_LL32B:
 return Cyc_Toc_path_length(_tmp796);_LL32C:;_LL32D:
 return 0;_LL325:;}struct _tuple31{struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple32{struct _tuple1*f1;void*f2;};struct _tuple33{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3926
static struct _tuple31 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
# 3931
struct Cyc_Absyn_Stmt*s;
{void*_tmp797=p->r;void*_tmp798=_tmp797;struct _tuple1*_tmp79A;struct _tuple1*_tmp79C;struct Cyc_Absyn_Pat*_tmp79D;struct _tuple1*_tmp79F;struct Cyc_Absyn_Vardecl*_tmp7A2;struct Cyc_Absyn_Pat*_tmp7A3;enum Cyc_Absyn_Sign _tmp7A6;int _tmp7A7;char _tmp7A9;struct _dyneither_ptr _tmp7AB;int _tmp7AC;struct Cyc_Absyn_Enumdecl*_tmp7AE;struct Cyc_Absyn_Enumfield*_tmp7AF;void*_tmp7B1;struct Cyc_Absyn_Enumfield*_tmp7B2;struct Cyc_Absyn_Datatypedecl*_tmp7B5;struct Cyc_Absyn_Datatypefield*_tmp7B6;struct Cyc_List_List*_tmp7B7;struct Cyc_List_List*_tmp7B9;struct Cyc_List_List*_tmp7BB;union Cyc_Absyn_AggrInfoU _tmp7BE;struct Cyc_List_List*_tmp7BF;struct Cyc_Absyn_Pat*_tmp7C1;_LL32F: {struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp799=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp798;if(_tmp799->tag != 2)goto _LL331;else{_tmp79A=(_tmp799->f2)->name;}}_LL330: {
# 3934
struct Cyc_Absyn_Pat*_tmp7C5=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0);
_tmp7C5->topt=p->topt;
_tmp79C=_tmp79A;_tmp79D=_tmp7C5;goto _LL332;}_LL331: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp79B=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp798;if(_tmp79B->tag != 1)goto _LL333;else{_tmp79C=(_tmp79B->f1)->name;_tmp79D=_tmp79B->f2;}}_LL332: {
# 3939
struct _tuple1*v=Cyc_Toc_temp_var();
void*_tmp7C6=(void*)_check_null(_tmp79D->topt);
{struct _tuple32*_tmpD84;struct Cyc_List_List*_tmpD83;decls=((_tmpD83=_region_malloc(rgn,sizeof(*_tmpD83)),((_tmpD83->hd=((_tmpD84=_region_malloc(rgn,sizeof(*_tmpD84)),((_tmpD84->f1=v,((_tmpD84->f2=Cyc_Toc_typ_to_c_array(_tmp7C6),_tmpD84)))))),((_tmpD83->tl=decls,_tmpD83))))));}{
struct _tuple31 _tmp7C9=Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp79C,Cyc_Absyn_var_exp(v,0)),rgn,_tmp7C6,
Cyc_Absyn_var_exp(v,0),path,_tmp79D,fail_stmt,decls);
nv=_tmp7C9.f1;
decls=_tmp7C9.f2;{
struct Cyc_Absyn_Stmt*_tmp7CA=_tmp7C9.f3;
struct Cyc_Absyn_Stmt*_tmp7CB=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(v,0),r,0);
s=Cyc_Absyn_seq_stmt(_tmp7CB,_tmp7CA,0);
goto _LL32E;};};}_LL333: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp79E=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp798;if(_tmp79E->tag != 4)goto _LL335;else{_tmp79F=(_tmp79E->f2)->name;}}_LL334: {
# 3952
struct _tuple1*_tmp7CC=Cyc_Toc_temp_var();
void*_tmp7CD=(void*)_check_null(p->topt);
{struct _tuple32*_tmpD87;struct Cyc_List_List*_tmpD86;decls=((_tmpD86=_region_malloc(rgn,sizeof(*_tmpD86)),((_tmpD86->hd=((_tmpD87=_region_malloc(rgn,sizeof(*_tmpD87)),((_tmpD87->f1=_tmp7CC,((_tmpD87->f2=Cyc_Toc_typ_to_c_array(_tmp7CD),_tmpD87)))))),((_tmpD86->tl=decls,_tmpD86))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp79F,Cyc_Absyn_var_exp(_tmp7CC,0));
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7CC,0),r,0);
goto _LL32E;}_LL335: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp7A0=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp798;if(_tmp7A0->tag != 0)goto _LL337;}_LL336:
 s=Cyc_Toc_skip_stmt_dl();goto _LL32E;_LL337: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp7A1=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp798;if(_tmp7A1->tag != 3)goto _LL339;else{_tmp7A2=_tmp7A1->f1;_tmp7A3=_tmp7A1->f2;}}_LL338: {
# 3961
struct _tuple1*_tmp7D0=Cyc_Toc_temp_var();
{struct _tuple32*_tmpD8A;struct Cyc_List_List*_tmpD89;decls=((_tmpD89=_region_malloc(rgn,sizeof(*_tmpD89)),((_tmpD89->hd=((_tmpD8A=_region_malloc(rgn,sizeof(*_tmpD8A)),((_tmpD8A->f1=_tmp7D0,((_tmpD8A->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),_tmpD8A)))))),((_tmpD89->tl=decls,_tmpD89))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp7A2->name,Cyc_Absyn_var_exp(_tmp7D0,0));
# 3965
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7D0,0),
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(path)),0);{
struct _tuple31 _tmp7D3=Cyc_Toc_xlate_pat(nv,rgn,t,r,path,_tmp7A3,fail_stmt,decls);
_tmp7D3.f3=Cyc_Absyn_seq_stmt(s,_tmp7D3.f3,0);
return _tmp7D3;};}_LL339: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp7A4=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp798;if(_tmp7A4->tag != 9)goto _LL33B;}_LL33A:
# 3972
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL32E;_LL33B: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp7A5=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp798;if(_tmp7A5->tag != 10)goto _LL33D;else{_tmp7A6=_tmp7A5->f1;_tmp7A7=_tmp7A5->f2;}}_LL33C:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(_tmp7A6,_tmp7A7,0),fail_stmt);goto _LL32E;_LL33D: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp7A8=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp798;if(_tmp7A8->tag != 11)goto _LL33F;else{_tmp7A9=_tmp7A8->f1;}}_LL33E:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp7A9,0),fail_stmt);goto _LL32E;_LL33F: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp7AA=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp798;if(_tmp7AA->tag != 12)goto _LL341;else{_tmp7AB=_tmp7AA->f1;_tmp7AC=_tmp7AA->f2;}}_LL340:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp7AB,_tmp7AC,0),fail_stmt);goto _LL32E;_LL341: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp7AD=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp798;if(_tmp7AD->tag != 13)goto _LL343;else{_tmp7AE=_tmp7AD->f1;_tmp7AF=_tmp7AD->f2;}}_LL342:
# 3977
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmpD8D;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpD8C;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpD8C=_cycalloc(sizeof(*_tmpD8C)),((_tmpD8C[0]=((_tmpD8D.tag=31,((_tmpD8D.f1=_tmp7AF->name,((_tmpD8D.f2=_tmp7AE,((_tmpD8D.f3=_tmp7AF,_tmpD8D)))))))),_tmpD8C)))),0),fail_stmt);}
goto _LL32E;_LL343: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp7B0=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp798;if(_tmp7B0->tag != 14)goto _LL345;else{_tmp7B1=(void*)_tmp7B0->f1;_tmp7B2=_tmp7B0->f2;}}_LL344:
# 3980
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpD90;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpD8F;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpD8F=_cycalloc(sizeof(*_tmpD8F)),((_tmpD8F[0]=((_tmpD90.tag=32,((_tmpD90.f1=_tmp7B2->name,((_tmpD90.f2=_tmp7B1,((_tmpD90.f3=_tmp7B2,_tmpD90)))))))),_tmpD8F)))),0),fail_stmt);}
goto _LL32E;_LL345: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp7B3=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp798;if(_tmp7B3->tag != 6)goto _LL347;else{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp7B4=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(_tmp7B3->f1)->r;if(_tmp7B4->tag != 8)goto _LL347;else{_tmp7B5=_tmp7B4->f1;_tmp7B6=_tmp7B4->f2;_tmp7B7=_tmp7B4->f3;}}}_LL346:
# 3990
 s=Cyc_Toc_skip_stmt_dl();{
struct _tuple1*_tmp7D8=Cyc_Toc_temp_var();
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(_tmp7B6->name,_tmp7B5->name);
void*_tmp7D9=Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);
int cnt=1;
struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(_tmp7D9,r);
struct Cyc_List_List*_tmp7DA=_tmp7B6->typs;
for(0;_tmp7B7 != 0;(((_tmp7B7=_tmp7B7->tl,_tmp7DA=((struct Cyc_List_List*)_check_null(_tmp7DA))->tl)),++ cnt)){
struct Cyc_Absyn_Pat*_tmp7DB=(struct Cyc_Absyn_Pat*)_tmp7B7->hd;
if(_tmp7DB->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp7DC=(*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp7DA))->hd)).f2;
void*_tmp7DD=(void*)_check_null(_tmp7DB->topt);
void*_tmp7DE=Cyc_Toc_typ_to_c_array(_tmp7DD);
struct Cyc_Absyn_Exp*_tmp7DF=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp7D8,0),Cyc_Absyn_fieldname(cnt),0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp7DC)))
_tmp7DF=Cyc_Toc_cast_it(_tmp7DE,_tmp7DF);{
# 4008
struct _tuple31 _tmp7E0=Cyc_Toc_xlate_pat(nv,rgn,_tmp7DD,_tmp7DF,_tmp7DF,_tmp7DB,fail_stmt,decls);
# 4010
nv=_tmp7E0.f1;
decls=_tmp7E0.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp7E0.f3,0);};};}{
# 4014
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp7D8,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*tag_exp=
_tmp7B5->is_extensible?Cyc_Absyn_var_exp(_tmp7B6->name,0): Cyc_Toc_datatype_tag(_tmp7B5,_tmp7B6->name);
struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,tag_exp,0);
s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);
if(Cyc_Toc_is_nullable((void*)_check_null(p->topt)))
# 4022
s=Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp7D8,0),Cyc_Absyn_uint_exp(0,0),0),fail_stmt,s,0);
# 4025
s=Cyc_Absyn_declare_stmt(_tmp7D8,_tmp7D9,rcast,s,0);
goto _LL32E;};};_LL347: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp7B8=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp798;if(_tmp7B8->tag != 8)goto _LL349;else{_tmp7B9=_tmp7B8->f3;}}_LL348:
# 4028
 _tmp7BB=_tmp7B9;goto _LL34A;_LL349: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp7BA=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp798;if(_tmp7BA->tag != 5)goto _LL34B;else{_tmp7BB=_tmp7BA->f1;}}_LL34A:
# 4036
 s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;
for(0;_tmp7BB != 0;(_tmp7BB=_tmp7BB->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp7E1=(struct Cyc_Absyn_Pat*)_tmp7BB->hd;
if(_tmp7E1->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp7E2=(void*)_check_null(_tmp7E1->topt);
struct _dyneither_ptr*_tmp7E3=Cyc_Absyn_fieldname(cnt);
struct _tuple31 _tmp7E4=Cyc_Toc_xlate_pat(nv,rgn,_tmp7E2,Cyc_Toc_member_exp(r,_tmp7E3,0),
Cyc_Toc_member_exp(path,_tmp7E3,0),_tmp7E1,fail_stmt,decls);
nv=_tmp7E4.f1;
decls=_tmp7E4.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp7E4.f3,0);};}
# 4050
goto _LL32E;};_LL34B: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp7BC=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp798;if(_tmp7BC->tag != 7)goto _LL34D;else{if(_tmp7BC->f1 != 0)goto _LL34D;}}_LL34C: {
# 4052
const char*_tmpD93;void*_tmpD92;(_tmpD92=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD93="unresolved aggregate pattern!",_tag_dyneither(_tmpD93,sizeof(char),30))),_tag_dyneither(_tmpD92,sizeof(void*),0)));}_LL34D: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp7BD=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp798;if(_tmp7BD->tag != 7)goto _LL34F;else{if(_tmp7BD->f1 == 0)goto _LL34F;_tmp7BE=(_tmp7BD->f1)->aggr_info;_tmp7BF=_tmp7BD->f3;}}_LL34E: {
# 4054
struct Cyc_Absyn_Aggrdecl*_tmp7E7=Cyc_Absyn_get_known_aggrdecl(_tmp7BE);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7E7->impl))->tagged){
# 4058
struct _tuple33*_tmp7E8=(struct _tuple33*)((struct Cyc_List_List*)_check_null(_tmp7BF))->hd;struct Cyc_List_List*_tmp7EA;struct Cyc_Absyn_Pat*_tmp7EB;struct _tuple33*_tmp7E9=_tmp7E8;_tmp7EA=_tmp7E9->f1;_tmp7EB=_tmp7E9->f2;{
struct _dyneither_ptr*f;
{void*_tmp7EC=(void*)((struct Cyc_List_List*)_check_null(_tmp7EA))->hd;void*_tmp7ED=_tmp7EC;struct _dyneither_ptr*_tmp7EF;_LL358: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp7EE=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp7ED;if(_tmp7EE->tag != 1)goto _LL35A;else{_tmp7EF=_tmp7EE->f1;}}_LL359:
 f=_tmp7EF;goto _LL357;_LL35A:;_LL35B: {
const char*_tmpD96;void*_tmpD95;(_tmpD95=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD96="no field name in tagged union pattern",_tag_dyneither(_tmpD96,sizeof(char),38))),_tag_dyneither(_tmpD95,sizeof(void*),0)));}_LL357:;}{
# 4065
void*_tmp7F2=(void*)_check_null(_tmp7EB->topt);
void*_tmp7F3=Cyc_Toc_typ_to_c_array(_tmp7F2);
# 4068
struct Cyc_Absyn_Exp*_tmp7F4=Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(path,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*_tmp7F5=Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp7F5=Cyc_Toc_cast_it(_tmp7F3,_tmp7F5);
_tmp7F4=Cyc_Toc_cast_it(_tmp7F3,_tmp7F4);{
struct _tuple31 _tmp7F6=Cyc_Toc_xlate_pat(nv,rgn,_tmp7F2,_tmp7F5,_tmp7F4,_tmp7EB,fail_stmt,decls);
# 4076
nv=_tmp7F6.f1;
decls=_tmp7F6.f2;{
struct Cyc_Absyn_Stmt*_tmp7F7=_tmp7F6.f3;
struct Cyc_Absyn_Stmt*_tmp7F8=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(r,f,0),Cyc_Toc_tag_sp,0),
Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp7E7,f),0),fail_stmt);
# 4084
s=Cyc_Absyn_seq_stmt(_tmp7F8,_tmp7F7,0);};};};};}else{
# 4087
s=Cyc_Toc_skip_stmt_dl();
for(0;_tmp7BF != 0;_tmp7BF=_tmp7BF->tl){
struct _tuple33*_tmp7F9=(struct _tuple33*)_tmp7BF->hd;
struct Cyc_Absyn_Pat*_tmp7FA=(*_tmp7F9).f2;
if(_tmp7FA->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*f;
{void*_tmp7FB=(void*)((struct Cyc_List_List*)_check_null((*_tmp7F9).f1))->hd;void*_tmp7FC=_tmp7FB;struct _dyneither_ptr*_tmp7FE;_LL35D: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp7FD=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp7FC;if(_tmp7FD->tag != 1)goto _LL35F;else{_tmp7FE=_tmp7FD->f1;}}_LL35E:
 f=_tmp7FE;goto _LL35C;_LL35F:;_LL360: {
struct Cyc_Toc_Match_error_exn_struct _tmpD99;struct Cyc_Toc_Match_error_exn_struct*_tmpD98;(int)_throw((void*)((_tmpD98=_cycalloc_atomic(sizeof(*_tmpD98)),((_tmpD98[0]=((_tmpD99.tag=Cyc_Toc_Match_error,_tmpD99)),_tmpD98)))));}_LL35C:;}{
# 4098
void*_tmp801=(void*)_check_null(_tmp7FA->topt);
void*_tmp802=Cyc_Toc_typ_to_c_array(_tmp801);
struct Cyc_Absyn_Exp*_tmp803=Cyc_Toc_member_exp(r,f,0);
void*_tmp804=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7E7->impl))->fields,f)))->type;
struct Cyc_Absyn_Exp*_tmp805=Cyc_Toc_member_exp(path,f,0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp804))
_tmp803=Cyc_Toc_cast_it(_tmp802,_tmp803);else{
if(!Cyc_Toc_is_array_type(_tmp804) && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp804))){
# 4108
_tmp803=Cyc_Absyn_deref_exp(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(_tmp802,Cyc_Toc_mt_tq),
Cyc_Absyn_address_exp(_tmp803,0)),0);
# 4111
_tmp805=Cyc_Toc_push_address_exp(_tmp805);
_tmp805=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(_tmp802,Cyc_Toc_mt_tq),_tmp805);
_tmp805=Cyc_Absyn_deref_exp(_tmp805,0);}}{
# 4115
struct _tuple31 _tmp806=Cyc_Toc_xlate_pat(nv,rgn,_tmp801,_tmp803,_tmp805,_tmp7FA,fail_stmt,decls);
nv=_tmp806.f1;
decls=_tmp806.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp806.f3,0);};};};}}
# 4121
goto _LL32E;}_LL34F: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp7C0=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp798;if(_tmp7C0->tag != 6)goto _LL351;else{_tmp7C1=_tmp7C0->f1;}}_LL350: {
# 4124
void*_tmp807=(void*)_check_null(_tmp7C1->topt);
# 4127
struct _tuple31 _tmp808=Cyc_Toc_xlate_pat(nv,rgn,_tmp807,Cyc_Absyn_deref_exp(r,0),Cyc_Absyn_deref_exp(path,0),_tmp7C1,fail_stmt,decls);
# 4129
nv=_tmp808.f1;
decls=_tmp808.f2;{
struct Cyc_Absyn_Stmt*_tmp809=_tmp808.f3;
if(Cyc_Toc_is_nullable(t))
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,
# 4135
Cyc_Toc_skip_stmt_dl(),0),_tmp809,0);else{
# 4137
s=_tmp809;}
goto _LL32E;};}_LL351: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp7C2=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp798;if(_tmp7C2->tag != 15)goto _LL353;}_LL352: {
const char*_tmpD9C;void*_tmpD9B;(_tmpD9B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD9C="unknownid pat",_tag_dyneither(_tmpD9C,sizeof(char),14))),_tag_dyneither(_tmpD9B,sizeof(void*),0)));}_LL353: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp7C3=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp798;if(_tmp7C3->tag != 16)goto _LL355;}_LL354: {
const char*_tmpD9F;void*_tmpD9E;(_tmpD9E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD9F="unknowncall pat",_tag_dyneither(_tmpD9F,sizeof(char),16))),_tag_dyneither(_tmpD9E,sizeof(void*),0)));}_LL355: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp7C4=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp798;if(_tmp7C4->tag != 17)goto _LL32E;}_LL356: {
const char*_tmpDA2;void*_tmpDA1;(_tmpDA1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDA2="exp pat",_tag_dyneither(_tmpDA2,sizeof(char),8))),_tag_dyneither(_tmpDA1,sizeof(void*),0)));}_LL32E:;}{
# 4143
struct _tuple31 _tmpDA3;return(_tmpDA3.f1=nv,((_tmpDA3.f2=decls,((_tmpDA3.f3=s,_tmpDA3)))));};}struct _tuple34{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};
# 4180 "toc.cyc"
static struct _tuple34*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 4182
struct _tuple34*_tmpDA4;return(_tmpDA4=_region_malloc(r,sizeof(*_tmpDA4)),((_tmpDA4->f1=Cyc_Toc_fresh_label(),((_tmpDA4->f2=Cyc_Toc_fresh_label(),((_tmpDA4->f3=sc,_tmpDA4)))))));}
# 4185
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){
# 4187
Cyc_Toc_exp_to_c(nv,e);{
void*_tmp812=(void*)_check_null(e->topt);
# 4192
int leave_as_switch;
{void*_tmp813=Cyc_Tcutil_compress(_tmp812);void*_tmp814=_tmp813;_LL362: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp815=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp814;if(_tmp815->tag != 6)goto _LL364;}_LL363:
 goto _LL365;_LL364: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp816=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp814;if(_tmp816->tag != 13)goto _LL366;}_LL365:
 leave_as_switch=1;goto _LL361;_LL366:;_LL367:
 leave_as_switch=0;goto _LL361;_LL361:;}
# 4198
{struct Cyc_List_List*_tmp817=scs;for(0;_tmp817 != 0;_tmp817=_tmp817->tl){
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp817->hd)->pat_vars))->v != 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp817->hd)->where_clause != 0){
leave_as_switch=0;
break;}}}
# 4203
if(leave_as_switch){
# 4205
struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();
{struct Cyc_List_List*_tmp818=scs;for(0;_tmp818 != 0;_tmp818=_tmp818->tl){
struct Cyc_Absyn_Stmt*_tmp819=((struct Cyc_Absyn_Switch_clause*)_tmp818->hd)->body;
((struct Cyc_Absyn_Switch_clause*)_tmp818->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp819,0);
next_l=Cyc_Toc_fresh_label();{
struct _RegionHandle _tmp81A=_new_region("rgn");struct _RegionHandle*rgn=& _tmp81A;_push_region(rgn);
Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(rgn,nv,next_l),_tmp819);;_pop_region(rgn);};}}
# 4214
return;}{
# 4217
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 4222
struct _RegionHandle _tmp81B=_new_region("rgn");struct _RegionHandle*rgn=& _tmp81B;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp81C=Cyc_Toc_share_env(rgn,nv);
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple34*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,rgn,scs);
struct Cyc_List_List*test_stmts=0;
struct Cyc_List_List*nvs=0;
struct Cyc_List_List*decls=0;
# 4233
{struct Cyc_List_List*_tmp81D=lscs;for(0;_tmp81D != 0;_tmp81D=_tmp81D->tl){
struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple34*)_tmp81D->hd)).f3;
struct _dyneither_ptr*fail_lab=_tmp81D->tl == 0?end_l:(*((struct _tuple34*)((struct Cyc_List_List*)_check_null(_tmp81D->tl))->hd)).f1;
struct _tuple31 _tmp81E=Cyc_Toc_xlate_pat(_tmp81C,rgn,_tmp812,r,path,sc->pattern,
Cyc_Absyn_goto_stmt(fail_lab,0),decls);
# 4236
struct Cyc_Toc_Env*_tmp820;struct Cyc_List_List*_tmp821;struct Cyc_Absyn_Stmt*_tmp822;struct _tuple31 _tmp81F=_tmp81E;_tmp820=_tmp81F.f1;_tmp821=_tmp81F.f2;_tmp822=_tmp81F.f3;
# 4239
if(sc->where_clause != 0){
struct Cyc_Absyn_Exp*_tmp823=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);
Cyc_Toc_exp_to_c(_tmp820,_tmp823);
_tmp822=Cyc_Absyn_seq_stmt(_tmp822,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp823,0),
Cyc_Absyn_goto_stmt(fail_lab,0),
Cyc_Toc_skip_stmt_dl(),0),0);}
# 4246
decls=_tmp821;
{struct Cyc_List_List*_tmpDA5;nvs=((_tmpDA5=_region_malloc(rgn,sizeof(*_tmpDA5)),((_tmpDA5->hd=_tmp820,((_tmpDA5->tl=nvs,_tmpDA5))))));}{
struct Cyc_List_List*_tmpDA6;test_stmts=((_tmpDA6=_region_malloc(rgn,sizeof(*_tmpDA6)),((_tmpDA6->hd=_tmp822,((_tmpDA6->tl=test_stmts,_tmpDA6))))));};}}
# 4250
nvs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);
test_stmts=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{
struct Cyc_List_List*stmts=0;
# 4255
for(0;lscs != 0;
(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple34*_tmp826=(struct _tuple34*)lscs->hd;struct _dyneither_ptr*_tmp828;struct _dyneither_ptr*_tmp829;struct Cyc_Absyn_Switch_clause*_tmp82A;struct _tuple34*_tmp827=_tmp826;_tmp828=_tmp827->f1;_tmp829=_tmp827->f2;_tmp82A=_tmp827->f3;{
struct Cyc_Toc_Env*_tmp82B=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs))->hd;
struct Cyc_Absyn_Stmt*s=_tmp82A->body;
struct _RegionHandle _tmp82C=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp82C;_push_region(rgn2);
if(lscs->tl != 0){
struct _tuple34*_tmp82D=(struct _tuple34*)((struct Cyc_List_List*)_check_null(lscs->tl))->hd;struct _dyneither_ptr*_tmp82F;struct Cyc_Absyn_Switch_clause*_tmp830;struct _tuple34*_tmp82E=_tmp82D;_tmp82F=_tmp82E->f2;_tmp830=_tmp82E->f3;{
struct Cyc_List_List*_tmp831=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp830->pat_vars))->v)).f1);
_tmp831=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp831);
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(rgn2,_tmp82B,end_l,_tmp82F,_tmp831,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs->tl))->hd),s);};}else{
# 4270
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn2,_tmp82B,end_l),s);}
# 4272
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp828,(struct Cyc_Absyn_Stmt*)((struct Cyc_List_List*)_check_null(test_stmts))->hd,0),
Cyc_Absyn_label_stmt(_tmp829,s,0),0);{
struct Cyc_List_List*_tmpDA7;stmts=((_tmpDA7=_region_malloc(rgn,sizeof(*_tmpDA7)),((_tmpDA7->hd=s,((_tmpDA7->tl=stmts,_tmpDA7))))));};
# 4260
;_pop_region(rgn2);};}{
# 4276
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),
Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),0),0);
# 4279
for(decls;decls != 0;decls=decls->tl){
struct _tuple32*_tmp833=(struct _tuple32*)decls->hd;struct _tuple1*_tmp835;void*_tmp836;struct _tuple32*_tmp834=_tmp833;_tmp835=_tmp834->f1;_tmp836=_tmp834->f2;
res=Cyc_Absyn_declare_stmt(_tmp835,_tmp836,0,res,0);}
# 4284
whole_s->r=(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),e,res,0))->r;};};}
# 4223
;_pop_region(rgn);};};}
# 4289
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 4295
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 4297
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
struct Cyc_List_List*_tmpDA8;return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,(
(_tmpDA8=_cycalloc(sizeof(*_tmpDA8)),((_tmpDA8->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0),((_tmpDA8->tl=0,_tmpDA8)))))),0),0);}
# 4301
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt(s->r,0));}struct _tuple35{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 4306
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 4308
while(1){
void*_tmp838=s->r;void*_tmp839=_tmp838;struct Cyc_Absyn_Exp*_tmp83C;struct Cyc_Absyn_Stmt*_tmp83E;struct Cyc_Absyn_Stmt*_tmp83F;struct Cyc_Absyn_Exp*_tmp841;struct Cyc_Absyn_Exp*_tmp843;struct Cyc_Absyn_Stmt*_tmp844;struct Cyc_Absyn_Stmt*_tmp845;struct Cyc_Absyn_Exp*_tmp847;struct Cyc_Absyn_Stmt*_tmp848;struct Cyc_Absyn_Stmt*_tmp84A;struct Cyc_Absyn_Stmt*_tmp84C;struct Cyc_Absyn_Stmt*_tmp84E;struct Cyc_Absyn_Exp*_tmp850;struct Cyc_Absyn_Exp*_tmp851;struct Cyc_Absyn_Exp*_tmp852;struct Cyc_Absyn_Stmt*_tmp853;struct Cyc_Absyn_Exp*_tmp855;struct Cyc_List_List*_tmp856;struct Cyc_List_List*_tmp858;struct Cyc_Absyn_Switch_clause**_tmp859;struct Cyc_Absyn_Decl*_tmp85B;struct Cyc_Absyn_Stmt*_tmp85C;struct _dyneither_ptr*_tmp85E;struct Cyc_Absyn_Stmt*_tmp85F;struct Cyc_Absyn_Stmt*_tmp861;struct Cyc_Absyn_Exp*_tmp862;struct Cyc_Absyn_Stmt*_tmp864;struct Cyc_List_List*_tmp865;struct Cyc_Absyn_Exp*_tmp867;_LL369: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp83A=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp839;if(_tmp83A->tag != 0)goto _LL36B;}_LL36A:
# 4311
 return;_LL36B: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp83B=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp839;if(_tmp83B->tag != 1)goto _LL36D;else{_tmp83C=_tmp83B->f1;}}_LL36C:
# 4313
 Cyc_Toc_exp_to_c(nv,_tmp83C);
return;_LL36D: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp83D=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp839;if(_tmp83D->tag != 2)goto _LL36F;else{_tmp83E=_tmp83D->f1;_tmp83F=_tmp83D->f2;}}_LL36E:
# 4316
 Cyc_Toc_stmt_to_c(nv,_tmp83E);
s=_tmp83F;
continue;_LL36F: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp840=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp839;if(_tmp840->tag != 3)goto _LL371;else{_tmp841=_tmp840->f1;}}_LL370: {
# 4320
void*topt=0;
if(_tmp841 != 0){
topt=Cyc_Toc_typ_to_c((void*)_check_null(_tmp841->topt));
Cyc_Toc_exp_to_c(nv,_tmp841);}
# 4326
if(s->try_depth > 0){
if(topt != 0){
struct _tuple1*_tmp868=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp869=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp868,0),0);
s->r=(Cyc_Absyn_declare_stmt(_tmp868,topt,_tmp841,
Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(s->try_depth),_tmp869,0),0))->r;}else{
# 4334
Cyc_Toc_do_npop_before(s->try_depth,s);}}
# 4336
return;}_LL371: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp842=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp839;if(_tmp842->tag != 4)goto _LL373;else{_tmp843=_tmp842->f1;_tmp844=_tmp842->f2;_tmp845=_tmp842->f3;}}_LL372:
# 4338
 Cyc_Toc_exp_to_c(nv,_tmp843);
Cyc_Toc_stmt_to_c(nv,_tmp844);
s=_tmp845;
continue;_LL373: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp846=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp839;if(_tmp846->tag != 5)goto _LL375;else{_tmp847=(_tmp846->f1).f1;_tmp848=_tmp846->f2;}}_LL374:
# 4343
 Cyc_Toc_exp_to_c(nv,_tmp847);{
struct _RegionHandle _tmp86A=_new_region("temp");struct _RegionHandle*temp=& _tmp86A;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp848);
# 4347
_npop_handler(0);return;
# 4344
;_pop_region(temp);};_LL375: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp849=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp839;if(_tmp849->tag != 6)goto _LL377;else{_tmp84A=_tmp849->f1;}}_LL376: {
# 4349
struct _dyneither_ptr**_tmp86C;struct Cyc_Toc_Env*_tmp86B=nv;_tmp86C=_tmp86B->break_lab;
if(_tmp86C != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp86C,0);{
# 4353
int dest_depth=_tmp84A == 0?0: _tmp84A->try_depth;
Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);
return;};}_LL377: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp84B=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp839;if(_tmp84B->tag != 7)goto _LL379;else{_tmp84C=_tmp84B->f1;}}_LL378: {
# 4357
struct _dyneither_ptr**_tmp86E;struct Cyc_Toc_Env*_tmp86D=nv;_tmp86E=_tmp86D->continue_lab;
if(_tmp86E != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp86E,0);
_tmp84E=_tmp84C;goto _LL37A;}_LL379: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp84D=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp839;if(_tmp84D->tag != 8)goto _LL37B;else{_tmp84E=_tmp84D->f2;}}_LL37A:
# 4363
 Cyc_Toc_do_npop_before(s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp84E))->try_depth,s);
return;_LL37B: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp84F=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp839;if(_tmp84F->tag != 9)goto _LL37D;else{_tmp850=_tmp84F->f1;_tmp851=(_tmp84F->f2).f1;_tmp852=(_tmp84F->f3).f1;_tmp853=_tmp84F->f4;}}_LL37C:
# 4367
 Cyc_Toc_exp_to_c(nv,_tmp850);Cyc_Toc_exp_to_c(nv,_tmp851);Cyc_Toc_exp_to_c(nv,_tmp852);{
struct _RegionHandle _tmp86F=_new_region("temp");struct _RegionHandle*temp=& _tmp86F;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp853);
# 4371
_npop_handler(0);return;
# 4368
;_pop_region(temp);};_LL37D: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp854=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp839;if(_tmp854->tag != 10)goto _LL37F;else{_tmp855=_tmp854->f1;_tmp856=_tmp854->f2;}}_LL37E:
# 4373
 Cyc_Toc_xlate_switch(nv,s,_tmp855,_tmp856);
return;_LL37F: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp857=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp839;if(_tmp857->tag != 11)goto _LL381;else{_tmp858=_tmp857->f1;_tmp859=_tmp857->f2;}}_LL380: {
# 4376
struct Cyc_Toc_FallthruInfo*_tmp871;struct Cyc_Toc_Env*_tmp870=nv;_tmp871=_tmp870->fallthru_info;
if(_tmp871 == 0){
const char*_tmpDAB;void*_tmpDAA;(_tmpDAA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDAB="fallthru in unexpected place",_tag_dyneither(_tmpDAB,sizeof(char),29))),_tag_dyneither(_tmpDAA,sizeof(void*),0)));}{
struct Cyc_Toc_FallthruInfo _tmp874=*_tmp871;struct _dyneither_ptr*_tmp876;struct Cyc_List_List*_tmp877;struct Cyc_Dict_Dict _tmp878;struct Cyc_Toc_FallthruInfo _tmp875=_tmp874;_tmp876=_tmp875.label;_tmp877=_tmp875.binders;_tmp878=_tmp875.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp876,0);
# 4382
Cyc_Toc_do_npop_before(s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp859)))->body)->try_depth,s2);{
struct Cyc_List_List*_tmp879=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp877);
struct Cyc_List_List*_tmp87A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp858);
for(0;_tmp879 != 0;(_tmp879=_tmp879->tl,_tmp87A=_tmp87A->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp87A))->hd);
s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp878,(struct _tuple1*)_tmp879->hd),(struct Cyc_Absyn_Exp*)_tmp87A->hd,0),s2,0);}
# 4390
s->r=s2->r;
return;};};};}_LL381: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp85A=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp839;if(_tmp85A->tag != 12)goto _LL383;else{_tmp85B=_tmp85A->f1;_tmp85C=_tmp85A->f2;}}_LL382:
# 4396
{void*_tmp87B=_tmp85B->r;void*_tmp87C=_tmp87B;struct Cyc_Absyn_Vardecl*_tmp87E;struct Cyc_Absyn_Pat*_tmp880;struct Cyc_Absyn_Exp*_tmp881;struct Cyc_List_List*_tmp883;struct Cyc_Absyn_Fndecl*_tmp885;struct Cyc_Absyn_Tvar*_tmp887;struct Cyc_Absyn_Vardecl*_tmp888;int _tmp889;struct Cyc_Absyn_Exp*_tmp88A;_LL38C: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp87D=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp87C;if(_tmp87D->tag != 0)goto _LL38E;else{_tmp87E=_tmp87D->f1;}}_LL38D: {
# 4398
struct _RegionHandle _tmp88B=_new_region("temp");struct _RegionHandle*temp=& _tmp88B;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpDAE;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpDAD;struct Cyc_Toc_Env*_tmp88C=Cyc_Toc_add_varmap(temp,nv,_tmp87E->name,
Cyc_Absyn_varb_exp(_tmp87E->name,(void*)((_tmpDAD=_cycalloc(sizeof(*_tmpDAD)),((_tmpDAD[0]=((_tmpDAE.tag=4,((_tmpDAE.f1=_tmp87E,_tmpDAE)))),_tmpDAD)))),0));
Cyc_Toc_local_decl_to_c(_tmp88C,_tmp88C,_tmp87E,_tmp85C);}
# 4403
_npop_handler(0);goto _LL38B;
# 4398
;_pop_region(temp);}_LL38E: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp87F=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp87C;if(_tmp87F->tag != 2)goto _LL390;else{_tmp880=_tmp87F->f1;_tmp881=_tmp87F->f3;}}_LL38F:
# 4407
{void*_tmp88F=_tmp880->r;void*_tmp890=_tmp88F;struct Cyc_Absyn_Vardecl*_tmp892;_LL399: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp891=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp890;if(_tmp891->tag != 1)goto _LL39B;else{_tmp892=_tmp891->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp893=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp891->f2)->r;if(_tmp893->tag != 0)goto _LL39B;};}}_LL39A: {
# 4409
struct _tuple1*old_name=_tmp892->name;
struct _tuple1*new_name=Cyc_Toc_temp_var();
_tmp892->name=new_name;
_tmp892->initializer=_tmp881;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpDB1;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpDB0;_tmp85B->r=(void*)((_tmpDB0=_cycalloc(sizeof(*_tmpDB0)),((_tmpDB0[0]=((_tmpDB1.tag=0,((_tmpDB1.f1=_tmp892,_tmpDB1)))),_tmpDB0))));}{
struct _RegionHandle _tmp896=_new_region("temp");struct _RegionHandle*temp=& _tmp896;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpDB4;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpDB3;struct Cyc_Toc_Env*_tmp897=
Cyc_Toc_add_varmap(temp,nv,old_name,
Cyc_Absyn_varb_exp(new_name,(void*)((_tmpDB3=_cycalloc(sizeof(*_tmpDB3)),((_tmpDB3[0]=((_tmpDB4.tag=4,((_tmpDB4.f1=_tmp892,_tmpDB4)))),_tmpDB3)))),0));
Cyc_Toc_local_decl_to_c(_tmp897,nv,_tmp892,_tmp85C);}
# 4420
_npop_handler(0);goto _LL398;
# 4414
;_pop_region(temp);};}_LL39B:;_LL39C:
# 4425
 s->r=(Cyc_Toc_letdecl_to_c(nv,_tmp880,(void*)_check_null(_tmp881->topt),_tmp881,_tmp85C))->r;
goto _LL398;_LL398:;}
# 4428
goto _LL38B;_LL390: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp882=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp87C;if(_tmp882->tag != 3)goto _LL392;else{_tmp883=_tmp882->f1;}}_LL391: {
# 4441 "toc.cyc"
struct Cyc_List_List*_tmp89A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp883);
if(_tmp89A == 0){
const char*_tmpDB7;void*_tmpDB6;(_tmpDB6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpDB7="empty Letv_d",_tag_dyneither(_tmpDB7,sizeof(char),13))),_tag_dyneither(_tmpDB6,sizeof(void*),0)));}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpDBA;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpDB9;_tmp85B->r=(void*)((_tmpDB9=_cycalloc(sizeof(*_tmpDB9)),((_tmpDB9[0]=((_tmpDBA.tag=0,((_tmpDBA.f1=(struct Cyc_Absyn_Vardecl*)_tmp89A->hd,_tmpDBA)))),_tmpDB9))));}
_tmp89A=_tmp89A->tl;
for(0;_tmp89A != 0;_tmp89A=_tmp89A->tl){
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpDBD;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpDBC;struct Cyc_Absyn_Decl*_tmp89F=Cyc_Absyn_new_decl((void*)((_tmpDBC=_cycalloc(sizeof(*_tmpDBC)),((_tmpDBC[0]=((_tmpDBD.tag=0,((_tmpDBD.f1=(struct Cyc_Absyn_Vardecl*)_tmp89A->hd,_tmpDBD)))),_tmpDBC)))),0);
s->r=(Cyc_Absyn_decl_stmt(_tmp89F,Cyc_Absyn_new_stmt(s->r,0),0))->r;}
# 4450
Cyc_Toc_stmt_to_c(nv,s);
goto _LL38B;}_LL392: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp884=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp87C;if(_tmp884->tag != 1)goto _LL394;else{_tmp885=_tmp884->f1;}}_LL393: {
# 4453
struct _tuple1*_tmp8A2=_tmp885->name;
struct _RegionHandle _tmp8A3=_new_region("temp");struct _RegionHandle*temp=& _tmp8A3;_push_region(temp);{
struct Cyc_Toc_Env*_tmp8A4=Cyc_Toc_add_varmap(temp,nv,_tmp885->name,Cyc_Absyn_var_exp(_tmp8A2,0));
Cyc_Toc_fndecl_to_c(_tmp8A4,_tmp885,0);
Cyc_Toc_stmt_to_c(_tmp8A4,_tmp85C);}
# 4459
_npop_handler(0);goto _LL38B;
# 4454
;_pop_region(temp);}_LL394: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp886=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp87C;if(_tmp886->tag != 4)goto _LL396;else{_tmp887=_tmp886->f1;_tmp888=_tmp886->f2;_tmp889=_tmp886->f3;_tmp88A=_tmp886->f4;}}_LL395: {
# 4461
struct Cyc_Absyn_Stmt*_tmp8A5=_tmp85C;
# 4469
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple1*rh_var=Cyc_Toc_temp_var();
struct _tuple1*x_var=_tmp888->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);
# 4476
struct _RegionHandle _tmp8A6=_new_region("temp");struct _RegionHandle*temp=& _tmp8A6;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(temp,nv,x_var,x_exp),_tmp8A5);
# 4479
if(Cyc_Absyn_no_regions)
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_uint_exp(0,0),_tmp8A5,0))->r;else{
if((unsigned int)_tmp88A){
Cyc_Toc_exp_to_c(nv,_tmp88A);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(Cyc_Absyn_aggrarrow_exp(_tmp88A,_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"h",sizeof(char),2),0),0);
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,_tmp8A5,0))->r;};}else{
# 4487
struct Cyc_Absyn_Exp*_tmpDC0[1];struct Cyc_Absyn_Exp*_tmpDBF[1];struct Cyc_List_List*_tmpDBE;s->r=(
Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,(
(_tmpDBE=_cycalloc(sizeof(*_tmpDBE)),((_tmpDBE->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0),((_tmpDBE->tl=0,_tmpDBE)))))),0),
# 4492
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpDBF[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpDBF,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),
Cyc_Absyn_seq_stmt(_tmp8A5,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpDC0[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpDC0,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}}
# 4498
_npop_handler(0);return;
# 4476
;_pop_region(temp);}_LL396:;_LL397: {
# 4499
const char*_tmpDC3;void*_tmpDC2;(_tmpDC2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpDC3="bad nested declaration within function",_tag_dyneither(_tmpDC3,sizeof(char),39))),_tag_dyneither(_tmpDC2,sizeof(void*),0)));}_LL38B:;}
# 4501
return;_LL383: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp85D=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp839;if(_tmp85D->tag != 13)goto _LL385;else{_tmp85E=_tmp85D->f1;_tmp85F=_tmp85D->f2;}}_LL384:
# 4503
 s=_tmp85F;continue;_LL385: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp860=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp839;if(_tmp860->tag != 14)goto _LL387;else{_tmp861=_tmp860->f1;_tmp862=(_tmp860->f2).f1;}}_LL386: {
# 4505
struct _RegionHandle _tmp8AD=_new_region("temp");struct _RegionHandle*temp=& _tmp8AD;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp861);
Cyc_Toc_exp_to_c(nv,_tmp862);
# 4509
_npop_handler(0);return;
# 4505
;_pop_region(temp);}_LL387: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp863=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp839;if(_tmp863->tag != 15)goto _LL389;else{_tmp864=_tmp863->f1;_tmp865=_tmp863->f2;}}_LL388: {
# 4526 "toc.cyc"
struct _tuple1*h_var=Cyc_Toc_temp_var();
struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
# 4537
e_exp->topt=e_typ;{
struct _RegionHandle _tmp8AE=_new_region("temp");struct _RegionHandle*temp=& _tmp8AE;_push_region(temp);{
struct Cyc_Toc_Env*_tmp8AF=Cyc_Toc_add_varmap(temp,nv,e_var,e_exp);
# 4541
Cyc_Toc_stmt_to_c(_tmp8AF,_tmp864);{
struct Cyc_Absyn_Stmt*_tmp8B0=Cyc_Absyn_seq_stmt(_tmp864,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
# 4547
struct _tuple1*_tmp8B1=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp8B2=Cyc_Absyn_var_exp(_tmp8B1,0);
struct Cyc_Absyn_Vardecl*_tmp8B3=Cyc_Absyn_new_vardecl(_tmp8B1,Cyc_Absyn_exn_typ(),0);
_tmp8B2->topt=Cyc_Absyn_exn_typ();{
struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpDCD;struct Cyc_Absyn_Pat*_tmpDCC;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpDCB;struct Cyc_Absyn_Pat*_tmpDCA;struct Cyc_Absyn_Pat*_tmp8B4=(_tmpDCA=_cycalloc(sizeof(*_tmpDCA)),((_tmpDCA->r=(void*)((_tmpDCD=_cycalloc(sizeof(*_tmpDCD)),((_tmpDCD[0]=((_tmpDCB.tag=1,((_tmpDCB.f1=_tmp8B3,((_tmpDCB.f2=((_tmpDCC=_cycalloc(sizeof(*_tmpDCC)),((_tmpDCC->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpDCC->topt=Cyc_Absyn_exn_typ(),((_tmpDCC->loc=0,_tmpDCC)))))))),_tmpDCB)))))),_tmpDCD)))),((_tmpDCA->topt=Cyc_Absyn_exn_typ(),((_tmpDCA->loc=0,_tmpDCA)))))));
struct Cyc_Absyn_Exp*_tmp8B5=Cyc_Absyn_rethrow_exp(_tmp8B2,0);
_tmp8B5->topt=(void*)& Cyc_Absyn_VoidType_val;{
struct Cyc_Absyn_Stmt*_tmp8B6=Cyc_Absyn_exp_stmt(_tmp8B5,0);
struct Cyc_Core_Opt*_tmpDDC;struct _tuple35*_tmpDDB;struct Cyc_Absyn_Vardecl**_tmpDDA;struct Cyc_List_List*_tmpDD9;struct Cyc_Absyn_Switch_clause*_tmpDD8;struct Cyc_Absyn_Switch_clause*_tmp8B7=
(_tmpDD8=_cycalloc(sizeof(*_tmpDD8)),((_tmpDD8->pattern=_tmp8B4,((_tmpDD8->pat_vars=((_tmpDDC=_cycalloc(sizeof(*_tmpDDC)),((_tmpDDC->v=((_tmpDD9=_cycalloc(sizeof(*_tmpDD9)),((_tmpDD9->hd=((_tmpDDB=_cycalloc(sizeof(*_tmpDDB)),((_tmpDDB->f1=((_tmpDDA=_cycalloc(sizeof(*_tmpDDA)),((_tmpDDA[0]=_tmp8B3,_tmpDDA)))),((_tmpDDB->f2=_tmp8B2,_tmpDDB)))))),((_tmpDD9->tl=0,_tmpDD9)))))),_tmpDDC)))),((_tmpDD8->where_clause=0,((_tmpDD8->body=_tmp8B6,((_tmpDD8->loc=0,_tmpDD8)))))))))));
# 4558
struct Cyc_List_List*_tmpDDD;struct Cyc_Absyn_Stmt*_tmp8B8=Cyc_Absyn_switch_stmt(e_exp,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp865,(
(_tmpDDD=_cycalloc(sizeof(*_tmpDDD)),((_tmpDDD->hd=_tmp8B7,((_tmpDDD->tl=0,_tmpDDD))))))),0);
# 4562
Cyc_Toc_stmt_to_c(_tmp8AF,_tmp8B8);{
# 4565
struct Cyc_List_List*_tmpDDE;struct Cyc_Absyn_Exp*_tmp8B9=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,(
(_tmpDDE=_cycalloc(sizeof(*_tmpDDE)),((_tmpDDE->hd=Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0),((_tmpDDE->tl=0,_tmpDDE)))))),0);
# 4569
struct Cyc_List_List*_tmpDDF;struct Cyc_Absyn_Stmt*_tmp8BA=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,(
(_tmpDDF=_cycalloc(sizeof(*_tmpDDF)),((_tmpDDF->hd=Cyc_Absyn_address_exp(h_exp,0),((_tmpDDF->tl=0,_tmpDDF)))))),0),0);
# 4573
struct Cyc_Absyn_Exp*_tmp8BB=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0);
struct Cyc_Absyn_Exp*_tmp8BC=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0);
s->r=(
Cyc_Absyn_declare_stmt(h_var,h_typ,0,
Cyc_Absyn_seq_stmt(_tmp8BA,
Cyc_Absyn_declare_stmt(was_thrown_var,was_thrown_typ,_tmp8BB,
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp8B9,
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp8BC,0),
Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0),_tmp8B0,
# 4584
Cyc_Absyn_declare_stmt(e_var,e_typ,
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp8B8,0),0),0),0),0),0))->r;};};};};}
# 4588
_npop_handler(0);return;
# 4538
;_pop_region(temp);};}_LL389: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp866=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp839;if(_tmp866->tag != 16)goto _LL368;else{_tmp867=_tmp866->f1;}}_LL38A:
# 4590
 if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{
# 4593
Cyc_Toc_exp_to_c(nv,_tmp867);{
struct Cyc_List_List*_tmpDE0;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((_tmpDE0=_cycalloc(sizeof(*_tmpDE0)),((_tmpDE0->hd=_tmp867,((_tmpDE0->tl=0,_tmpDE0)))))),0));};}
# 4596
return;_LL368:;}}
# 4605
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->requires_clause=0;
f->ensures_clause=0;
f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp8CA=_new_region("frgn");struct _RegionHandle*frgn=& _tmp8CA;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp8CB=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp8CC=f->args;for(0;_tmp8CC != 0;_tmp8CC=_tmp8CC->tl){
struct _tuple1*_tmpDE1;struct _tuple1*_tmp8CD=(_tmpDE1=_cycalloc(sizeof(*_tmpDE1)),((_tmpDE1->f1=Cyc_Absyn_Loc_n,((_tmpDE1->f2=(*((struct _tuple9*)_tmp8CC->hd)).f1,_tmpDE1)))));
(*((struct _tuple9*)_tmp8CC->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp8CC->hd)).f3);
_tmp8CB=Cyc_Toc_add_varmap(frgn,_tmp8CB,_tmp8CD,Cyc_Absyn_var_exp(_tmp8CD,0));}}
# 4622
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}
# 4626
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp8CF=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));struct _dyneither_ptr*_tmp8D1;struct Cyc_Absyn_Tqual _tmp8D2;void*_tmp8D3;int _tmp8D4;struct Cyc_Absyn_VarargInfo _tmp8D0=_tmp8CF;_tmp8D1=_tmp8D0.name;_tmp8D2=_tmp8D0.tq;_tmp8D3=_tmp8D0.type;_tmp8D4=_tmp8D0.inject;{
void*_tmp8D5=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp8D3,(void*)& Cyc_Absyn_HeapRgn_val,_tmp8D2,Cyc_Absyn_false_conref));
struct _tuple1*_tmpDE2;struct _tuple1*_tmp8D6=(_tmpDE2=_cycalloc(sizeof(*_tmpDE2)),((_tmpDE2->f1=Cyc_Absyn_Loc_n,((_tmpDE2->f2=(struct _dyneither_ptr*)_check_null(_tmp8D1),_tmpDE2)))));
{struct _tuple9*_tmpDE5;struct Cyc_List_List*_tmpDE4;f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpDE4=_cycalloc(sizeof(*_tmpDE4)),((_tmpDE4->hd=((_tmpDE5=_cycalloc(sizeof(*_tmpDE5)),((_tmpDE5->f1=_tmp8D1,((_tmpDE5->f2=_tmp8D2,((_tmpDE5->f3=_tmp8D5,_tmpDE5)))))))),((_tmpDE4->tl=0,_tmpDE4)))))));}
_tmp8CB=Cyc_Toc_add_varmap(frgn,_tmp8CB,_tmp8D6,Cyc_Absyn_var_exp(_tmp8D6,0));
f->cyc_varargs=0;};}
# 4635
{struct Cyc_List_List*_tmp8DA=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp8DA != 0;_tmp8DA=_tmp8DA->tl){
((struct Cyc_Absyn_Vardecl*)_tmp8DA->hd)->type=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp8DA->hd)->type);}}
# 4638
Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp8CB),f->body);}
# 4614
;_pop_region(frgn);};}
# 4643
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
switch(s){case Cyc_Absyn_Abstract: _LL39D:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL39E:
 return Cyc_Absyn_Extern;default: _LL39F:
 return s;}}struct _tuple36{struct Cyc_Absyn_Aggrdecl*f1;int f2;};struct _tuple37{struct Cyc_Toc_TocState*f1;struct _tuple36*f2;};
# 4660 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple37*env){
# 4663
struct _tuple37 _tmp8DB=*env;struct Cyc_Toc_TocState*_tmp8DD;struct Cyc_Absyn_Aggrdecl*_tmp8DE;int _tmp8DF;struct _tuple37 _tmp8DC=_tmp8DB;_tmp8DD=_tmp8DC.f1;_tmp8DE=(_tmp8DC.f2)->f1;_tmp8DF=(_tmp8DC.f2)->f2;{
struct _tuple1*_tmp8E0=_tmp8DE->name;
struct Cyc_Toc_TocState _tmp8E1=*_tmp8DD;struct Cyc_Dict_Dict*_tmp8E3;struct Cyc_Toc_TocState _tmp8E2=_tmp8E1;_tmp8E3=_tmp8E2.aggrs_so_far;{
int seen_defn_before;
struct _tuple18**_tmp8E4=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp8E3,_tmp8E0);
if(_tmp8E4 == 0){
seen_defn_before=0;{
struct _tuple18*v;
if(_tmp8DE->kind == Cyc_Absyn_StructA){
struct _tuple18*_tmpDE6;v=((_tmpDE6=_region_malloc(d,sizeof(*_tmpDE6)),((_tmpDE6->f1=_tmp8DE,((_tmpDE6->f2=Cyc_Absyn_strctq(_tmp8E0),_tmpDE6))))));}else{
# 4674
struct _tuple18*_tmpDE7;v=((_tmpDE7=_region_malloc(d,sizeof(*_tmpDE7)),((_tmpDE7->f1=_tmp8DE,((_tmpDE7->f2=Cyc_Absyn_unionq_typ(_tmp8E0),_tmpDE7))))));}
*_tmp8E3=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp8E3,_tmp8E0,v);};}else{
# 4677
struct _tuple18*_tmp8E7=*_tmp8E4;struct Cyc_Absyn_Aggrdecl*_tmp8E9;void*_tmp8EA;struct _tuple18*_tmp8E8=_tmp8E7;_tmp8E9=_tmp8E8->f1;_tmp8EA=_tmp8E8->f2;
if(_tmp8E9->impl == 0){
{struct _tuple18*_tmpDE8;*_tmp8E3=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp8E3,_tmp8E0,((_tmpDE8=_region_malloc(d,sizeof(*_tmpDE8)),((_tmpDE8->f1=_tmp8DE,((_tmpDE8->f2=_tmp8EA,_tmpDE8)))))));}
seen_defn_before=0;}else{
# 4682
seen_defn_before=1;}}{
# 4684
struct Cyc_Absyn_Aggrdecl*_tmpDE9;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpDE9=_cycalloc(sizeof(*_tmpDE9)),((_tmpDE9->kind=_tmp8DE->kind,((_tmpDE9->sc=Cyc_Absyn_Public,((_tmpDE9->name=_tmp8DE->name,((_tmpDE9->tvs=0,((_tmpDE9->impl=0,((_tmpDE9->expected_mem_kind=0,((_tmpDE9->attributes=_tmp8DE->attributes,_tmpDE9)))))))))))))));
# 4691
if(_tmp8DE->impl != 0  && !seen_defn_before){
{struct Cyc_Absyn_AggrdeclImpl*_tmpDEA;new_ad->impl=((_tmpDEA=_cycalloc(sizeof(*_tmpDEA)),((_tmpDEA->exist_vars=0,((_tmpDEA->rgn_po=0,((_tmpDEA->fields=0,((_tmpDEA->tagged=0,_tmpDEA))))))))));}{
# 4696
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp8ED=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp8DE->impl))->fields;for(0;_tmp8ED != 0;_tmp8ED=_tmp8ED->tl){
# 4700
struct Cyc_Absyn_Aggrfield*_tmp8EE=(struct Cyc_Absyn_Aggrfield*)_tmp8ED->hd;
void*_tmp8EF=_tmp8EE->type;
struct Cyc_List_List*_tmp8F0=_tmp8EE->attributes;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp8EF)) && (
_tmp8DE->kind == Cyc_Absyn_StructA  && _tmp8ED->tl == 0  || _tmp8DE->kind == Cyc_Absyn_UnionA)){
# 4714 "toc.cyc"
void*_tmp8F1=Cyc_Tcutil_compress(_tmp8EF);void*_tmp8F2=_tmp8F1;void*_tmp8F4;struct Cyc_Absyn_Tqual _tmp8F5;union Cyc_Absyn_Constraint*_tmp8F6;unsigned int _tmp8F7;_LL3A2: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8F3=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8F2;if(_tmp8F3->tag != 8)goto _LL3A4;else{_tmp8F4=(_tmp8F3->f1).elt_type;_tmp8F5=(_tmp8F3->f1).tq;_tmp8F6=(_tmp8F3->f1).zero_term;_tmp8F7=(_tmp8F3->f1).zt_loc;}}_LL3A3:
# 4717
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpDF0;struct Cyc_Absyn_ArrayInfo _tmpDEF;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpDEE;_tmp8EF=(void*)((_tmpDEE=_cycalloc(sizeof(*_tmpDEE)),((_tmpDEE[0]=((_tmpDF0.tag=8,((_tmpDF0.f1=((_tmpDEF.elt_type=_tmp8F4,((_tmpDEF.tq=_tmp8F5,((_tmpDEF.num_elts=Cyc_Absyn_uint_exp(0,0),((_tmpDEF.zero_term=_tmp8F6,((_tmpDEF.zt_loc=_tmp8F7,_tmpDEF)))))))))),_tmpDF0)))),_tmpDEE))));}
goto _LL3A1;_LL3A4:;_LL3A5:
# 4720
{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpDF6;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpDF5;struct Cyc_List_List*_tmpDF4;_tmp8F0=((_tmpDF4=_cycalloc(sizeof(*_tmpDF4)),((_tmpDF4->hd=(void*)((_tmpDF6=_cycalloc(sizeof(*_tmpDF6)),((_tmpDF6[0]=((_tmpDF5.tag=6,((_tmpDF5.f1=0,_tmpDF5)))),_tmpDF6)))),((_tmpDF4->tl=_tmp8F0,_tmpDF4))))));}{
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpDFC;struct Cyc_Absyn_ArrayInfo _tmpDFB;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpDFA;_tmp8EF=(void*)((_tmpDFA=_cycalloc(sizeof(*_tmpDFA)),((_tmpDFA[0]=((_tmpDFC.tag=8,((_tmpDFC.f1=((_tmpDFB.elt_type=Cyc_Absyn_void_star_typ(),((_tmpDFB.tq=
Cyc_Absyn_empty_tqual(0),((_tmpDFB.num_elts=
Cyc_Absyn_uint_exp(0,0),((_tmpDFB.zero_term=
((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0),((_tmpDFB.zt_loc=0,_tmpDFB)))))))))),_tmpDFC)))),_tmpDFA))));};_LL3A1:;}{
# 4727
struct Cyc_Absyn_Aggrfield*_tmpDFD;struct Cyc_Absyn_Aggrfield*_tmp901=(_tmpDFD=_cycalloc(sizeof(*_tmpDFD)),((_tmpDFD->name=_tmp8EE->name,((_tmpDFD->tq=Cyc_Toc_mt_tq,((_tmpDFD->type=
# 4729
Cyc_Toc_typ_to_c(_tmp8EF),((_tmpDFD->width=_tmp8EE->width,((_tmpDFD->attributes=_tmp8F0,((_tmpDFD->requires_clause=0,_tmpDFD)))))))))))));
# 4737
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp8DE->impl))->tagged){
void*_tmp902=_tmp901->type;
struct _dyneither_ptr*_tmp903=_tmp901->name;
const char*_tmpE02;void*_tmpE01[2];struct Cyc_String_pa_PrintArg_struct _tmpE00;struct Cyc_String_pa_PrintArg_struct _tmpDFF;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpDFF.tag=0,((_tmpDFF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp903),((_tmpE00.tag=0,((_tmpE00.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp8DE->name).f2),((_tmpE01[0]=& _tmpE00,((_tmpE01[1]=& _tmpDFF,Cyc_aprintf(((_tmpE02="_union_%s_%s",_tag_dyneither(_tmpE02,sizeof(char),13))),_tag_dyneither(_tmpE01,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpE03;struct _dyneither_ptr*str=(_tmpE03=_cycalloc(sizeof(*_tmpE03)),((_tmpE03[0]=s,_tmpE03)));
struct Cyc_Absyn_Aggrfield*_tmpE04;struct Cyc_Absyn_Aggrfield*_tmp904=(_tmpE04=_cycalloc(sizeof(*_tmpE04)),((_tmpE04->name=Cyc_Toc_val_sp,((_tmpE04->tq=Cyc_Toc_mt_tq,((_tmpE04->type=_tmp902,((_tmpE04->width=0,((_tmpE04->attributes=0,((_tmpE04->requires_clause=0,_tmpE04)))))))))))));
struct Cyc_Absyn_Aggrfield*_tmpE05;struct Cyc_Absyn_Aggrfield*_tmp905=(_tmpE05=_cycalloc(sizeof(*_tmpE05)),((_tmpE05->name=Cyc_Toc_tag_sp,((_tmpE05->tq=Cyc_Toc_mt_tq,((_tmpE05->type=Cyc_Absyn_sint_typ,((_tmpE05->width=0,((_tmpE05->attributes=0,((_tmpE05->requires_clause=0,_tmpE05)))))))))))));
struct Cyc_Absyn_Aggrfield*_tmpE06[2];struct Cyc_List_List*_tmp906=(_tmpE06[1]=_tmp904,((_tmpE06[0]=_tmp905,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpE06,sizeof(struct Cyc_Absyn_Aggrfield*),2)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpE0B;struct _tuple1*_tmpE0A;struct Cyc_Absyn_Aggrdecl*_tmpE09;struct Cyc_Absyn_Aggrdecl*_tmp907=(_tmpE09=_cycalloc(sizeof(*_tmpE09)),((_tmpE09->kind=Cyc_Absyn_StructA,((_tmpE09->sc=Cyc_Absyn_Public,((_tmpE09->name=(
(_tmpE0A=_cycalloc(sizeof(*_tmpE0A)),((_tmpE0A->f1=Cyc_Absyn_Loc_n,((_tmpE0A->f2=str,_tmpE0A)))))),((_tmpE09->tvs=0,((_tmpE09->impl=(
(_tmpE0B=_cycalloc(sizeof(*_tmpE0B)),((_tmpE0B->exist_vars=0,((_tmpE0B->rgn_po=0,((_tmpE0B->fields=_tmp906,((_tmpE0B->tagged=0,_tmpE0B)))))))))),((_tmpE09->expected_mem_kind=0,((_tmpE09->attributes=0,_tmpE09)))))))))))))));
# 4750
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpE11;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpE10;struct Cyc_List_List*_tmpE0F;Cyc_Toc_result_decls=((_tmpE0F=_cycalloc(sizeof(*_tmpE0F)),((_tmpE0F->hd=Cyc_Absyn_new_decl((void*)((_tmpE11=_cycalloc(sizeof(*_tmpE11)),((_tmpE11[0]=((_tmpE10.tag=5,((_tmpE10.f1=_tmp907,_tmpE10)))),_tmpE11)))),0),((_tmpE0F->tl=Cyc_Toc_result_decls,_tmpE0F))))));}
_tmp901->type=Cyc_Absyn_strct(str);}{
# 4753
struct Cyc_List_List*_tmpE12;new_fields=((_tmpE12=_cycalloc(sizeof(*_tmpE12)),((_tmpE12->hd=_tmp901,((_tmpE12->tl=new_fields,_tmpE12))))));};};}}
# 4755
(new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);};}
# 4757
if(_tmp8DF){
struct Cyc_Absyn_Decl*_tmpE1C;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpE1B;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpE1A;struct Cyc_List_List*_tmpE19;Cyc_Toc_result_decls=((_tmpE19=_cycalloc(sizeof(*_tmpE19)),((_tmpE19->hd=((_tmpE1C=_cycalloc(sizeof(*_tmpE1C)),((_tmpE1C->r=(void*)((_tmpE1A=_cycalloc(sizeof(*_tmpE1A)),((_tmpE1A[0]=((_tmpE1B.tag=5,((_tmpE1B.f1=new_ad,_tmpE1B)))),_tmpE1A)))),((_tmpE1C->loc=0,_tmpE1C)))))),((_tmpE19->tl=Cyc_Toc_result_decls,_tmpE19))))));}
return 0;};};};}
# 4762
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple36 _tmpE1D;struct _tuple36 p=(_tmpE1D.f1=ad,((_tmpE1D.f2=add_to_decls,_tmpE1D)));
((int(*)(struct _tuple36*arg,int(*f)(struct _RegionHandle*,struct _tuple37*)))Cyc_Toc_use_toc_state)(& p,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple38{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 4791 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple38*env){
# 4794
struct _tuple38 _tmp91E=*env;struct Cyc_Set_Set**_tmp920;struct Cyc_Absyn_Datatypedecl*_tmp921;struct _tuple38 _tmp91F=_tmp91E;_tmp920=(_tmp91F.f1)->datatypes_so_far;_tmp921=_tmp91F.f2;{
struct _tuple1*_tmp922=_tmp921->name;
if(_tmp921->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*_tmp920,_tmp922))
return 0;
*_tmp920=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*_tmp920,_tmp922);
{struct Cyc_List_List*_tmp923=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp921->fields))->v;for(0;_tmp923 != 0;_tmp923=_tmp923->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp923->hd;
# 4802
struct Cyc_List_List*_tmp924=0;
int i=1;
{struct Cyc_List_List*_tmp925=f->typs;for(0;_tmp925 != 0;(_tmp925=_tmp925->tl,i ++)){
struct _dyneither_ptr*_tmp926=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmpE1E;struct Cyc_Absyn_Aggrfield*_tmp927=(_tmpE1E=_cycalloc(sizeof(*_tmpE1E)),((_tmpE1E->name=_tmp926,((_tmpE1E->tq=(*((struct _tuple11*)_tmp925->hd)).f1,((_tmpE1E->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp925->hd)).f2),((_tmpE1E->width=0,((_tmpE1E->attributes=0,((_tmpE1E->requires_clause=0,_tmpE1E)))))))))))));
struct Cyc_List_List*_tmpE1F;_tmp924=((_tmpE1F=_cycalloc(sizeof(*_tmpE1F)),((_tmpE1F->hd=_tmp927,((_tmpE1F->tl=_tmp924,_tmpE1F))))));}}
# 4810
{struct Cyc_Absyn_Aggrfield*_tmpE22;struct Cyc_List_List*_tmpE21;_tmp924=((_tmpE21=_cycalloc(sizeof(*_tmpE21)),((_tmpE21->hd=((_tmpE22=_cycalloc(sizeof(*_tmpE22)),((_tmpE22->name=Cyc_Toc_tag_sp,((_tmpE22->tq=Cyc_Toc_mt_tq,((_tmpE22->type=Cyc_Absyn_sint_typ,((_tmpE22->width=0,((_tmpE22->attributes=0,((_tmpE22->requires_clause=0,_tmpE22)))))))))))))),((_tmpE21->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp924),_tmpE21))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpE25;struct Cyc_Absyn_Aggrdecl*_tmpE24;struct Cyc_Absyn_Aggrdecl*_tmp92C=
(_tmpE24=_cycalloc(sizeof(*_tmpE24)),((_tmpE24->kind=Cyc_Absyn_StructA,((_tmpE24->sc=Cyc_Absyn_Public,((_tmpE24->name=
Cyc_Toc_collapse_qvars(f->name,_tmp921->name),((_tmpE24->tvs=0,((_tmpE24->expected_mem_kind=0,((_tmpE24->impl=(
# 4816
(_tmpE25=_cycalloc(sizeof(*_tmpE25)),((_tmpE25->exist_vars=0,((_tmpE25->rgn_po=0,((_tmpE25->fields=_tmp924,((_tmpE25->tagged=0,_tmpE25)))))))))),((_tmpE24->attributes=0,_tmpE24)))))))))))))));
# 4818
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpE2B;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpE2A;struct Cyc_List_List*_tmpE29;Cyc_Toc_result_decls=((_tmpE29=_cycalloc(sizeof(*_tmpE29)),((_tmpE29->hd=Cyc_Absyn_new_decl((void*)((_tmpE2B=_cycalloc(sizeof(*_tmpE2B)),((_tmpE2B[0]=((_tmpE2A.tag=5,((_tmpE2A.f1=_tmp92C,_tmpE2A)))),_tmpE2B)))),0),((_tmpE29->tl=Cyc_Toc_result_decls,_tmpE29))))));};}}
# 4820
return 0;};}
# 4823
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple38*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 4842 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple38*env){
# 4845
struct _tuple38 _tmp932=*env;struct Cyc_Toc_TocState*_tmp934;struct Cyc_Absyn_Datatypedecl*_tmp935;struct _tuple38 _tmp933=_tmp932;_tmp934=_tmp933.f1;_tmp935=_tmp933.f2;
if(_tmp935->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp936=*_tmp934;struct Cyc_Dict_Dict*_tmp938;struct Cyc_Toc_TocState _tmp937=_tmp936;_tmp938=_tmp937.xdatatypes_so_far;{
struct _tuple1*_tmp939=_tmp935->name;
{struct Cyc_List_List*_tmp93A=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp935->fields))->v;for(0;_tmp93A != 0;_tmp93A=_tmp93A->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp93A->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp93B=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0);
void*_tmp93C=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,_tmp93B,Cyc_Absyn_false_conref,0);
# 4856
int*_tmp93D=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp938,f->name);int*_tmp93E=_tmp93D;_LL3A7: if(_tmp93E != 0)goto _LL3A9;_LL3A8: {
# 4858
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0);{
# 4862
struct Cyc_Absyn_Vardecl*_tmp93F=Cyc_Absyn_new_vardecl(f->name,_tmp93C,initopt);
_tmp93F->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpE31;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpE30;struct Cyc_List_List*_tmpE2F;Cyc_Toc_result_decls=((_tmpE2F=_cycalloc(sizeof(*_tmpE2F)),((_tmpE2F->hd=Cyc_Absyn_new_decl((void*)((_tmpE31=_cycalloc(sizeof(*_tmpE31)),((_tmpE31[0]=((_tmpE30.tag=0,((_tmpE30.f1=_tmp93F,_tmpE30)))),_tmpE31)))),0),((_tmpE2F->tl=Cyc_Toc_result_decls,_tmpE2F))))));}
*_tmp938=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp938,f->name,f->sc != Cyc_Absyn_Extern);{
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp943=f->typs;for(0;_tmp943 != 0;(_tmp943=_tmp943->tl,i ++)){
struct Cyc_Int_pa_PrintArg_struct _tmpE39;void*_tmpE38[1];const char*_tmpE37;struct _dyneither_ptr*_tmpE36;struct _dyneither_ptr*_tmp944=(_tmpE36=_cycalloc(sizeof(*_tmpE36)),((_tmpE36[0]=(struct _dyneither_ptr)((_tmpE39.tag=1,((_tmpE39.f1=(unsigned long)i,((_tmpE38[0]=& _tmpE39,Cyc_aprintf(((_tmpE37="f%d",_tag_dyneither(_tmpE37,sizeof(char),4))),_tag_dyneither(_tmpE38,sizeof(void*),1)))))))),_tmpE36)));
struct Cyc_Absyn_Aggrfield*_tmpE3A;struct Cyc_Absyn_Aggrfield*_tmp945=(_tmpE3A=_cycalloc(sizeof(*_tmpE3A)),((_tmpE3A->name=_tmp944,((_tmpE3A->tq=(*((struct _tuple11*)_tmp943->hd)).f1,((_tmpE3A->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp943->hd)).f2),((_tmpE3A->width=0,((_tmpE3A->attributes=0,((_tmpE3A->requires_clause=0,_tmpE3A)))))))))))));
struct Cyc_List_List*_tmpE3B;fields=((_tmpE3B=_cycalloc(sizeof(*_tmpE3B)),((_tmpE3B->hd=_tmp945,((_tmpE3B->tl=fields,_tmpE3B))))));}}
# 4875
{struct Cyc_Absyn_Aggrfield*_tmpE3E;struct Cyc_List_List*_tmpE3D;fields=((_tmpE3D=_cycalloc(sizeof(*_tmpE3D)),((_tmpE3D->hd=((_tmpE3E=_cycalloc(sizeof(*_tmpE3E)),((_tmpE3E->name=Cyc_Toc_tag_sp,((_tmpE3E->tq=Cyc_Toc_mt_tq,((_tmpE3E->type=
Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq),((_tmpE3E->width=0,((_tmpE3E->attributes=0,((_tmpE3E->requires_clause=0,_tmpE3E)))))))))))))),((_tmpE3D->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmpE3D))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpE41;struct Cyc_Absyn_Aggrdecl*_tmpE40;struct Cyc_Absyn_Aggrdecl*_tmp94E=
(_tmpE40=_cycalloc(sizeof(*_tmpE40)),((_tmpE40->kind=Cyc_Absyn_StructA,((_tmpE40->sc=Cyc_Absyn_Public,((_tmpE40->name=
Cyc_Toc_collapse_qvars(f->name,_tmp935->name),((_tmpE40->tvs=0,((_tmpE40->expected_mem_kind=0,((_tmpE40->impl=(
# 4882
(_tmpE41=_cycalloc(sizeof(*_tmpE41)),((_tmpE41->exist_vars=0,((_tmpE41->rgn_po=0,((_tmpE41->fields=fields,((_tmpE41->tagged=0,_tmpE41)))))))))),((_tmpE40->attributes=0,_tmpE40)))))))))))))));
# 4884
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpE47;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpE46;struct Cyc_List_List*_tmpE45;Cyc_Toc_result_decls=((_tmpE45=_cycalloc(sizeof(*_tmpE45)),((_tmpE45->hd=Cyc_Absyn_new_decl((void*)((_tmpE47=_cycalloc(sizeof(*_tmpE47)),((_tmpE47[0]=((_tmpE46.tag=5,((_tmpE46.f1=_tmp94E,_tmpE46)))),_tmpE47)))),0),((_tmpE45->tl=Cyc_Toc_result_decls,_tmpE45))))));}
# 4886
goto _LL3A6;};};};}_LL3A9: if(_tmp93E == 0)goto _LL3AB;if(*_tmp93E != 0)goto _LL3AB;_LL3AA:
# 4888
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp954=Cyc_Absyn_string_exp(*fn,0);
struct Cyc_Absyn_Vardecl*_tmp955=Cyc_Absyn_new_vardecl(f->name,_tmp93C,_tmp954);
_tmp955->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpE4D;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpE4C;struct Cyc_List_List*_tmpE4B;Cyc_Toc_result_decls=((_tmpE4B=_cycalloc(sizeof(*_tmpE4B)),((_tmpE4B->hd=Cyc_Absyn_new_decl((void*)((_tmpE4D=_cycalloc(sizeof(*_tmpE4D)),((_tmpE4D[0]=((_tmpE4C.tag=0,((_tmpE4C.f1=_tmp955,_tmpE4C)))),_tmpE4D)))),0),((_tmpE4B->tl=Cyc_Toc_result_decls,_tmpE4B))))));}
*_tmp938=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp938,f->name,1);}
# 4895
goto _LL3A6;_LL3AB:;_LL3AC:
 goto _LL3A6;_LL3A6:;}}
# 4899
return 0;};};}
# 4902
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple38*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 4907
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4915
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
vd->type=Cyc_Toc_typ_to_c(old_typ);
# 4919
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(body_nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp959=init->r;void*_tmp95A=_tmp959;struct Cyc_Absyn_Vardecl*_tmp95C;struct Cyc_Absyn_Exp*_tmp95D;struct Cyc_Absyn_Exp*_tmp95E;int _tmp95F;_LL3AE: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp95B=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp95B->tag != 26)goto _LL3B0;else{_tmp95C=_tmp95B->f1;_tmp95D=_tmp95B->f2;_tmp95E=_tmp95B->f3;_tmp95F=_tmp95B->f4;}}_LL3AF:
# 4926
 vd->initializer=0;
s->r=(Cyc_Toc_init_comprehension(init_nv,old_typ,Cyc_Absyn_var_exp(vd->name,0),_tmp95C,_tmp95D,_tmp95E,_tmp95F,
Cyc_Absyn_new_stmt(s->r,0),0))->r;
goto _LL3AD;_LL3B0:;_LL3B1:
# 4931
 if(vd->sc == Cyc_Absyn_Static){
# 4935
struct _RegionHandle _tmp960=_new_region("temp");struct _RegionHandle*temp=& _tmp960;_push_region(temp);
{struct Cyc_Toc_Env*_tmp961=Cyc_Toc_set_toplevel(temp,init_nv);
Cyc_Toc_exp_to_c(_tmp961,init);}
# 4936
;_pop_region(temp);}else{
# 4940
Cyc_Toc_exp_to_c(init_nv,init);}
goto _LL3AD;_LL3AD:;}else{
# 4945
void*_tmp962=Cyc_Tcutil_compress(old_typ);void*_tmp963=_tmp962;void*_tmp965;struct Cyc_Absyn_Exp*_tmp966;union Cyc_Absyn_Constraint*_tmp967;_LL3B3:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp964=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp963;if(_tmp964->tag != 8)goto _LL3B5;else{_tmp965=(_tmp964->f1).elt_type;_tmp966=(_tmp964->f1).num_elts;_tmp967=(_tmp964->f1).zero_term;}}if(!
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp967))goto _LL3B5;_LL3B4:
 if(_tmp966 == 0){
const char*_tmpE50;void*_tmpE4F;(_tmpE4F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE50="can't initialize zero-terminated array -- size unknown",_tag_dyneither(_tmpE50,sizeof(char),55))),_tag_dyneither(_tmpE4F,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=_tmp966;
struct Cyc_Absyn_Exp*_tmp96A=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),
Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);
# 4953
struct Cyc_Absyn_Exp*_tmp96B=Cyc_Absyn_signed_int_exp(0,0);
s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp96A,_tmp96B,0),0),
Cyc_Absyn_new_stmt(s->r,0));
goto _LL3B2;};_LL3B5:;_LL3B6:
 goto _LL3B2;_LL3B2:;}}
# 4967
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*x=Cyc_Toc_temp_var();
struct _RegionHandle _tmp96C=_new_region("prgn");struct _RegionHandle*prgn=& _tmp96C;_push_region(prgn);{
struct Cyc_Absyn_Stmt*_tmp96D=Cyc_Toc_throw_match_stmt();
struct Cyc_Toc_Env*_tmp96E=Cyc_Toc_share_env(prgn,nv);
struct _tuple31 _tmp96F=
Cyc_Toc_xlate_pat(_tmp96E,prgn,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,
Cyc_Toc_throw_match_stmt(),0);
# 4973
struct Cyc_Toc_Env*_tmp971;struct Cyc_List_List*_tmp972;struct Cyc_Absyn_Stmt*_tmp973;struct _tuple31 _tmp970=_tmp96F;_tmp971=_tmp970.f1;_tmp972=_tmp970.f2;_tmp973=_tmp970.f3;
# 4976
Cyc_Toc_stmt_to_c(_tmp971,s);
s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),e,Cyc_Absyn_seq_stmt(_tmp973,s,0),0);
for(0;_tmp972 != 0;_tmp972=_tmp972->tl){
struct _tuple32*_tmp974=(struct _tuple32*)_tmp972->hd;struct _tuple1*_tmp976;void*_tmp977;struct _tuple32*_tmp975=_tmp974;_tmp976=_tmp975->f1;_tmp977=_tmp975->f2;
s=Cyc_Absyn_declare_stmt(_tmp976,_tmp977,0,s,0);}}{
# 4983
struct Cyc_Absyn_Stmt*_tmp978=s;_npop_handler(0);return _tmp978;};
# 4970
;_pop_region(prgn);};}
# 4989
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp979=e->r;void*_tmp97A=_tmp979;struct Cyc_Absyn_Exp*_tmp97C;struct Cyc_Absyn_Exp*_tmp97E;struct Cyc_Absyn_Exp*_tmp980;struct Cyc_Absyn_Exp*_tmp982;struct Cyc_Absyn_Exp*_tmp984;struct Cyc_Absyn_Exp*_tmp986;struct Cyc_Absyn_Exp*_tmp988;struct Cyc_Absyn_Exp*_tmp98A;struct Cyc_List_List*_tmp98C;struct Cyc_Absyn_Exp*_tmp98E;struct Cyc_Absyn_Exp*_tmp98F;struct Cyc_Absyn_Exp*_tmp991;struct Cyc_Absyn_Exp*_tmp992;struct Cyc_Absyn_Exp*_tmp994;struct Cyc_Absyn_Exp*_tmp995;struct Cyc_Absyn_Exp*_tmp997;struct Cyc_Absyn_Exp*_tmp998;struct Cyc_Absyn_Exp*_tmp99A;struct Cyc_Absyn_Exp*_tmp99B;struct Cyc_Absyn_Exp*_tmp99D;struct Cyc_Absyn_Exp*_tmp99E;struct Cyc_Absyn_Exp*_tmp9A0;struct Cyc_Absyn_Exp*_tmp9A1;struct Cyc_Absyn_Exp*_tmp9A2;struct Cyc_Absyn_Exp*_tmp9A4;struct Cyc_List_List*_tmp9A5;void**_tmp9A7;struct Cyc_Absyn_Exp*_tmp9A8;void**_tmp9AA;struct Cyc_List_List*_tmp9AB;struct Cyc_List_List*_tmp9AD;struct Cyc_List_List*_tmp9AF;void**_tmp9B1;void**_tmp9B3;struct Cyc_Absyn_Stmt*_tmp9B5;struct Cyc_Absyn_MallocInfo*_tmp9B7;_LL3B8: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp97B=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp97B->tag != 19)goto _LL3BA;else{_tmp97C=_tmp97B->f1;}}_LL3B9:
 _tmp97E=_tmp97C;goto _LL3BB;_LL3BA: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp97D=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp97D->tag != 20)goto _LL3BC;else{_tmp97E=_tmp97D->f1;}}_LL3BB:
 _tmp980=_tmp97E;goto _LL3BD;_LL3BC: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp97F=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp97F->tag != 21)goto _LL3BE;else{_tmp980=_tmp97F->f1;}}_LL3BD:
 _tmp982=_tmp980;goto _LL3BF;_LL3BE: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp981=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp981->tag != 14)goto _LL3C0;else{_tmp982=_tmp981->f1;}}_LL3BF:
 _tmp984=_tmp982;goto _LL3C1;_LL3C0: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp983=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp983->tag != 10)goto _LL3C2;else{_tmp984=_tmp983->f1;}}_LL3C1:
 _tmp986=_tmp984;goto _LL3C3;_LL3C2: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp985=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp985->tag != 11)goto _LL3C4;else{_tmp986=_tmp985->f1;}}_LL3C3:
 _tmp988=_tmp986;goto _LL3C5;_LL3C4: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp987=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp987->tag != 17)goto _LL3C6;else{_tmp988=_tmp987->f1;}}_LL3C5:
 _tmp98A=_tmp988;goto _LL3C7;_LL3C6: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp989=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp989->tag != 4)goto _LL3C8;else{_tmp98A=_tmp989->f1;}}_LL3C7:
 Cyc_Toc_exptypes_to_c(_tmp98A);goto _LL3B7;_LL3C8: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp98B=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp98B->tag != 2)goto _LL3CA;else{_tmp98C=_tmp98B->f2;}}_LL3C9:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp98C);goto _LL3B7;_LL3CA: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp98D=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp98D->tag != 6)goto _LL3CC;else{_tmp98E=_tmp98D->f1;_tmp98F=_tmp98D->f2;}}_LL3CB:
 _tmp991=_tmp98E;_tmp992=_tmp98F;goto _LL3CD;_LL3CC: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp990=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp990->tag != 7)goto _LL3CE;else{_tmp991=_tmp990->f1;_tmp992=_tmp990->f2;}}_LL3CD:
 _tmp994=_tmp991;_tmp995=_tmp992;goto _LL3CF;_LL3CE: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp993=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp993->tag != 8)goto _LL3D0;else{_tmp994=_tmp993->f1;_tmp995=_tmp993->f2;}}_LL3CF:
 _tmp997=_tmp994;_tmp998=_tmp995;goto _LL3D1;_LL3D0: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp996=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp996->tag != 22)goto _LL3D2;else{_tmp997=_tmp996->f1;_tmp998=_tmp996->f2;}}_LL3D1:
 _tmp99A=_tmp997;_tmp99B=_tmp998;goto _LL3D3;_LL3D2: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp999=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp999->tag != 34)goto _LL3D4;else{_tmp99A=_tmp999->f1;_tmp99B=_tmp999->f2;}}_LL3D3:
 _tmp99D=_tmp99A;_tmp99E=_tmp99B;goto _LL3D5;_LL3D4: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp99C=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp99C->tag != 3)goto _LL3D6;else{_tmp99D=_tmp99C->f1;_tmp99E=_tmp99C->f3;}}_LL3D5:
 Cyc_Toc_exptypes_to_c(_tmp99D);Cyc_Toc_exptypes_to_c(_tmp99E);goto _LL3B7;_LL3D6: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp99F=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp99F->tag != 5)goto _LL3D8;else{_tmp9A0=_tmp99F->f1;_tmp9A1=_tmp99F->f2;_tmp9A2=_tmp99F->f3;}}_LL3D7:
# 5007
 Cyc_Toc_exptypes_to_c(_tmp9A0);Cyc_Toc_exptypes_to_c(_tmp9A1);Cyc_Toc_exptypes_to_c(_tmp9A2);goto _LL3B7;_LL3D8: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp9A3=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp9A3->tag != 9)goto _LL3DA;else{_tmp9A4=_tmp9A3->f1;_tmp9A5=_tmp9A3->f2;}}_LL3D9:
# 5009
 Cyc_Toc_exptypes_to_c(_tmp9A4);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp9A5);goto _LL3B7;_LL3DA: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp9A6=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp9A6->tag != 13)goto _LL3DC;else{_tmp9A7=(void**)((void**)& _tmp9A6->f1);_tmp9A8=_tmp9A6->f2;}}_LL3DB:
*_tmp9A7=Cyc_Toc_typ_to_c(*_tmp9A7);Cyc_Toc_exptypes_to_c(_tmp9A8);goto _LL3B7;_LL3DC: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp9A9=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp9A9->tag != 24)goto _LL3DE;else{_tmp9AA=(void**)&(_tmp9A9->f1)->f3;_tmp9AB=_tmp9A9->f2;}}_LL3DD:
# 5012
*_tmp9AA=Cyc_Toc_typ_to_c(*_tmp9AA);
_tmp9AD=_tmp9AB;goto _LL3DF;_LL3DE: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp9AC=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp9AC->tag != 35)goto _LL3E0;else{_tmp9AD=_tmp9AC->f2;}}_LL3DF:
 _tmp9AF=_tmp9AD;goto _LL3E1;_LL3E0: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp9AE=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp9AE->tag != 25)goto _LL3E2;else{_tmp9AF=_tmp9AE->f1;}}_LL3E1:
# 5016
 for(0;_tmp9AF != 0;_tmp9AF=_tmp9AF->tl){
struct _tuple19 _tmp9C7=*((struct _tuple19*)_tmp9AF->hd);struct Cyc_Absyn_Exp*_tmp9C9;struct _tuple19 _tmp9C8=_tmp9C7;_tmp9C9=_tmp9C8.f2;
Cyc_Toc_exptypes_to_c(_tmp9C9);}
# 5020
goto _LL3B7;_LL3E2: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp9B0=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp9B0->tag != 18)goto _LL3E4;else{_tmp9B1=(void**)((void**)& _tmp9B0->f1);}}_LL3E3:
 _tmp9B3=_tmp9B1;goto _LL3E5;_LL3E4: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp9B2=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp9B2->tag != 16)goto _LL3E6;else{_tmp9B3=(void**)((void**)& _tmp9B2->f1);}}_LL3E5:
*_tmp9B3=Cyc_Toc_typ_to_c(*_tmp9B3);goto _LL3B7;_LL3E6: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp9B4=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp9B4->tag != 36)goto _LL3E8;else{_tmp9B5=_tmp9B4->f1;}}_LL3E7:
 Cyc_Toc_stmttypes_to_c(_tmp9B5);goto _LL3B7;_LL3E8: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp9B6=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp9B6->tag != 33)goto _LL3EA;else{_tmp9B7=(struct Cyc_Absyn_MallocInfo*)& _tmp9B6->f1;}}_LL3E9:
# 5025
 if(_tmp9B7->elt_type != 0){
void**_tmpE51;_tmp9B7->elt_type=((_tmpE51=_cycalloc(sizeof(*_tmpE51)),((_tmpE51[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp9B7->elt_type))),_tmpE51))));}
Cyc_Toc_exptypes_to_c(_tmp9B7->num_elts);
goto _LL3B7;_LL3EA: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp9B8=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp9B8->tag != 0)goto _LL3EC;}_LL3EB:
 goto _LL3ED;_LL3EC: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp9B9=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp9B9->tag != 1)goto _LL3EE;}_LL3ED:
 goto _LL3EF;_LL3EE: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp9BA=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp9BA->tag != 31)goto _LL3F0;}_LL3EF:
 goto _LL3F1;_LL3F0: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp9BB=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp9BB->tag != 39)goto _LL3F2;}_LL3F1:
 goto _LL3F3;_LL3F2: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp9BC=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp9BC->tag != 32)goto _LL3F4;}_LL3F3:
 goto _LL3B7;_LL3F4: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp9BD=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp9BD->tag != 29)goto _LL3F6;}_LL3F5:
# 5035
 goto _LL3F7;_LL3F6: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp9BE=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp9BE->tag != 30)goto _LL3F8;}_LL3F7:
 goto _LL3F9;_LL3F8: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp9BF=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp9BF->tag != 28)goto _LL3FA;}_LL3F9:
 goto _LL3FB;_LL3FA: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp9C0=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp9C0->tag != 26)goto _LL3FC;}_LL3FB:
 goto _LL3FD;_LL3FC: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp9C1=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp9C1->tag != 27)goto _LL3FE;}_LL3FD:
 goto _LL3FF;_LL3FE: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp9C2=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp9C2->tag != 23)goto _LL400;}_LL3FF:
 goto _LL401;_LL400: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp9C3=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp9C3->tag != 12)goto _LL402;}_LL401:
 goto _LL403;_LL402: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp9C4=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp9C4->tag != 15)goto _LL404;}_LL403:
 goto _LL405;_LL404: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp9C5=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp9C5->tag != 38)goto _LL406;}_LL405:
 goto _LL407;_LL406: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp9C6=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp97A;if(_tmp9C6->tag != 37)goto _LL3B7;}_LL407:
# 5045
{const char*_tmpE54;void*_tmpE53;(_tmpE53=0,Cyc_Tcutil_terr(e->loc,((_tmpE54="Cyclone expression within C code",_tag_dyneither(_tmpE54,sizeof(char),33))),_tag_dyneither(_tmpE53,sizeof(void*),0)));}
goto _LL3B7;_LL3B7:;}
# 5050
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp9CD=d->r;void*_tmp9CE=_tmp9CD;struct Cyc_Absyn_Vardecl*_tmp9D0;struct Cyc_Absyn_Fndecl*_tmp9D2;struct Cyc_Absyn_Aggrdecl*_tmp9D4;struct Cyc_Absyn_Enumdecl*_tmp9D6;struct Cyc_Absyn_Typedefdecl*_tmp9D8;_LL409: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp9CF=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp9CE;if(_tmp9CF->tag != 0)goto _LL40B;else{_tmp9D0=_tmp9CF->f1;}}_LL40A:
# 5053
 _tmp9D0->type=Cyc_Toc_typ_to_c(_tmp9D0->type);
if(_tmp9D0->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9D0->initializer));
goto _LL408;_LL40B: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp9D1=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp9CE;if(_tmp9D1->tag != 1)goto _LL40D;else{_tmp9D2=_tmp9D1->f1;}}_LL40C:
# 5057
 _tmp9D2->ret_type=Cyc_Toc_typ_to_c(_tmp9D2->ret_type);
{struct Cyc_List_List*_tmp9E3=_tmp9D2->args;for(0;_tmp9E3 != 0;_tmp9E3=_tmp9E3->tl){
(*((struct _tuple9*)_tmp9E3->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp9E3->hd)).f3);}}
# 5061
goto _LL408;_LL40D: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp9D3=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp9CE;if(_tmp9D3->tag != 5)goto _LL40F;else{_tmp9D4=_tmp9D3->f1;}}_LL40E:
 Cyc_Toc_aggrdecl_to_c(_tmp9D4,1);goto _LL408;_LL40F: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp9D5=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp9CE;if(_tmp9D5->tag != 7)goto _LL411;else{_tmp9D6=_tmp9D5->f1;}}_LL410:
# 5064
 if(_tmp9D6->fields != 0){
struct Cyc_List_List*_tmp9E4=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp9D6->fields))->v;for(0;_tmp9E4 != 0;_tmp9E4=_tmp9E4->tl){
struct Cyc_Absyn_Enumfield*_tmp9E5=(struct Cyc_Absyn_Enumfield*)_tmp9E4->hd;
if(_tmp9E5->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9E5->tag));}}
# 5069
goto _LL408;_LL411: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp9D7=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp9CE;if(_tmp9D7->tag != 8)goto _LL413;else{_tmp9D8=_tmp9D7->f1;}}_LL412:
 _tmp9D8->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp9D8->defn));goto _LL408;_LL413: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp9D9=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp9CE;if(_tmp9D9->tag != 2)goto _LL415;}_LL414:
 goto _LL416;_LL415: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp9DA=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp9CE;if(_tmp9DA->tag != 3)goto _LL417;}_LL416:
 goto _LL418;_LL417: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp9DB=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp9CE;if(_tmp9DB->tag != 6)goto _LL419;}_LL418:
 goto _LL41A;_LL419: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp9DC=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp9CE;if(_tmp9DC->tag != 9)goto _LL41B;}_LL41A:
 goto _LL41C;_LL41B: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp9DD=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp9CE;if(_tmp9DD->tag != 10)goto _LL41D;}_LL41C:
 goto _LL41E;_LL41D: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp9DE=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp9CE;if(_tmp9DE->tag != 11)goto _LL41F;}_LL41E:
 goto _LL420;_LL41F: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp9DF=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp9CE;if(_tmp9DF->tag != 12)goto _LL421;}_LL420:
 goto _LL422;_LL421: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp9E0=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp9CE;if(_tmp9E0->tag != 4)goto _LL423;}_LL422:
# 5080
{const char*_tmpE57;void*_tmpE56;(_tmpE56=0,Cyc_Tcutil_terr(d->loc,((_tmpE57="Cyclone declaration within C code",_tag_dyneither(_tmpE57,sizeof(char),34))),_tag_dyneither(_tmpE56,sizeof(void*),0)));}
goto _LL408;_LL423: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp9E1=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp9CE;if(_tmp9E1->tag != 13)goto _LL425;}_LL424:
 goto _LL426;_LL425: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp9E2=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp9CE;if(_tmp9E2->tag != 14)goto _LL408;}_LL426:
# 5084
 goto _LL408;_LL408:;}
# 5088
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp9E8=s->r;void*_tmp9E9=_tmp9E8;struct Cyc_Absyn_Exp*_tmp9EB;struct Cyc_Absyn_Stmt*_tmp9ED;struct Cyc_Absyn_Stmt*_tmp9EE;struct Cyc_Absyn_Exp*_tmp9F0;struct Cyc_Absyn_Exp*_tmp9F2;struct Cyc_Absyn_Stmt*_tmp9F3;struct Cyc_Absyn_Stmt*_tmp9F4;struct Cyc_Absyn_Exp*_tmp9F6;struct Cyc_Absyn_Stmt*_tmp9F7;struct Cyc_Absyn_Exp*_tmp9F9;struct Cyc_Absyn_Exp*_tmp9FA;struct Cyc_Absyn_Exp*_tmp9FB;struct Cyc_Absyn_Stmt*_tmp9FC;struct Cyc_Absyn_Exp*_tmp9FE;struct Cyc_List_List*_tmp9FF;struct Cyc_Absyn_Decl*_tmpA01;struct Cyc_Absyn_Stmt*_tmpA02;struct Cyc_Absyn_Stmt*_tmpA04;struct Cyc_Absyn_Exp*_tmpA05;struct Cyc_Absyn_Stmt*_tmpA07;_LL428: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp9EA=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp9E9;if(_tmp9EA->tag != 1)goto _LL42A;else{_tmp9EB=_tmp9EA->f1;}}_LL429:
 Cyc_Toc_exptypes_to_c(_tmp9EB);goto _LL427;_LL42A: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp9EC=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp9E9;if(_tmp9EC->tag != 2)goto _LL42C;else{_tmp9ED=_tmp9EC->f1;_tmp9EE=_tmp9EC->f2;}}_LL42B:
 Cyc_Toc_stmttypes_to_c(_tmp9ED);Cyc_Toc_stmttypes_to_c(_tmp9EE);goto _LL427;_LL42C: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp9EF=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp9E9;if(_tmp9EF->tag != 3)goto _LL42E;else{_tmp9F0=_tmp9EF->f1;}}_LL42D:
 if(_tmp9F0 != 0)Cyc_Toc_exptypes_to_c(_tmp9F0);goto _LL427;_LL42E: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp9F1=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp9E9;if(_tmp9F1->tag != 4)goto _LL430;else{_tmp9F2=_tmp9F1->f1;_tmp9F3=_tmp9F1->f2;_tmp9F4=_tmp9F1->f3;}}_LL42F:
# 5094
 Cyc_Toc_exptypes_to_c(_tmp9F2);Cyc_Toc_stmttypes_to_c(_tmp9F3);Cyc_Toc_stmttypes_to_c(_tmp9F4);goto _LL427;_LL430: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp9F5=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp9E9;if(_tmp9F5->tag != 5)goto _LL432;else{_tmp9F6=(_tmp9F5->f1).f1;_tmp9F7=_tmp9F5->f2;}}_LL431:
# 5096
 Cyc_Toc_exptypes_to_c(_tmp9F6);Cyc_Toc_stmttypes_to_c(_tmp9F7);goto _LL427;_LL432: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp9F8=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp9E9;if(_tmp9F8->tag != 9)goto _LL434;else{_tmp9F9=_tmp9F8->f1;_tmp9FA=(_tmp9F8->f2).f1;_tmp9FB=(_tmp9F8->f3).f1;_tmp9FC=_tmp9F8->f4;}}_LL433:
# 5098
 Cyc_Toc_exptypes_to_c(_tmp9F9);Cyc_Toc_exptypes_to_c(_tmp9FA);Cyc_Toc_exptypes_to_c(_tmp9FB);
Cyc_Toc_stmttypes_to_c(_tmp9FC);goto _LL427;_LL434: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp9FD=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp9E9;if(_tmp9FD->tag != 10)goto _LL436;else{_tmp9FE=_tmp9FD->f1;_tmp9FF=_tmp9FD->f2;}}_LL435:
# 5101
 Cyc_Toc_exptypes_to_c(_tmp9FE);
for(0;_tmp9FF != 0;_tmp9FF=_tmp9FF->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp9FF->hd)->body);}
goto _LL427;_LL436: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpA00=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp9E9;if(_tmpA00->tag != 12)goto _LL438;else{_tmpA01=_tmpA00->f1;_tmpA02=_tmpA00->f2;}}_LL437:
 Cyc_Toc_decltypes_to_c(_tmpA01);Cyc_Toc_stmttypes_to_c(_tmpA02);goto _LL427;_LL438: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmpA03=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp9E9;if(_tmpA03->tag != 14)goto _LL43A;else{_tmpA04=_tmpA03->f1;_tmpA05=(_tmpA03->f2).f1;}}_LL439:
 Cyc_Toc_stmttypes_to_c(_tmpA04);Cyc_Toc_exptypes_to_c(_tmpA05);goto _LL427;_LL43A: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpA06=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp9E9;if(_tmpA06->tag != 13)goto _LL43C;else{_tmpA07=_tmpA06->f2;}}_LL43B:
 Cyc_Toc_stmttypes_to_c(_tmpA07);goto _LL427;_LL43C: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmpA08=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp9E9;if(_tmpA08->tag != 0)goto _LL43E;}_LL43D:
 goto _LL43F;_LL43E: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmpA09=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp9E9;if(_tmpA09->tag != 6)goto _LL440;}_LL43F:
 goto _LL441;_LL440: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmpA0A=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp9E9;if(_tmpA0A->tag != 7)goto _LL442;}_LL441:
 goto _LL443;_LL442: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmpA0B=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp9E9;if(_tmpA0B->tag != 8)goto _LL444;}_LL443:
# 5111
 goto _LL427;_LL444: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmpA0C=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp9E9;if(_tmpA0C->tag != 11)goto _LL446;}_LL445:
# 5114
{struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct _tmpE5A;struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmpE59;s->r=(void*)((_tmpE59=_cycalloc_atomic(sizeof(*_tmpE59)),((_tmpE59[0]=((_tmpE5A.tag=0,_tmpE5A)),_tmpE59))));}
goto _LL427;_LL446: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmpA0D=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp9E9;if(_tmpA0D->tag != 15)goto _LL448;}_LL447:
 goto _LL449;_LL448: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmpA0E=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp9E9;if(_tmpA0E->tag != 16)goto _LL427;}_LL449:
# 5118
{struct Cyc_String_pa_PrintArg_struct _tmpE62;void*_tmpE61[1];const char*_tmpE60;void*_tmpE5F;(_tmpE5F=0,Cyc_Tcutil_terr(s->loc,(struct _dyneither_ptr)((_tmpE62.tag=0,((_tmpE62.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpE61[0]=& _tmpE62,Cyc_aprintf(((_tmpE60="Cyclone statement in C code: %s",_tag_dyneither(_tmpE60,sizeof(char),32))),_tag_dyneither(_tmpE61,sizeof(void*),1)))))))),_tag_dyneither(_tmpE5F,sizeof(void*),0)));}
goto _LL427;_LL427:;}
# 5127
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv)){
const char*_tmpE65;void*_tmpE64;(_tmpE64=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE65="decls_to_c: not at toplevel!",_tag_dyneither(_tmpE65,sizeof(char),29))),_tag_dyneither(_tmpE64,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmpA17=d->r;void*_tmpA18=_tmpA17;struct Cyc_Absyn_Vardecl*_tmpA1A;struct Cyc_Absyn_Fndecl*_tmpA1C;struct Cyc_Absyn_Aggrdecl*_tmpA21;struct Cyc_Absyn_Datatypedecl*_tmpA23;struct Cyc_Absyn_Enumdecl*_tmpA25;struct Cyc_Absyn_Typedefdecl*_tmpA27;struct Cyc_List_List*_tmpA2B;struct Cyc_List_List*_tmpA2D;struct Cyc_List_List*_tmpA2F;struct Cyc_List_List*_tmpA31;_LL44B: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpA19=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpA18;if(_tmpA19->tag != 0)goto _LL44D;else{_tmpA1A=_tmpA19->f1;}}_LL44C: {
# 5134
struct _tuple1*_tmpA32=_tmpA1A->name;
# 5136
if(_tmpA1A->sc == Cyc_Absyn_ExternC){
struct _tuple1*_tmpE66;_tmpA32=((_tmpE66=_cycalloc(sizeof(*_tmpE66)),((_tmpE66->f1=Cyc_Absyn_Rel_n(0),((_tmpE66->f2=(*_tmpA32).f2,_tmpE66))))));}
if(_tmpA1A->initializer != 0){
if(_tmpA1A->sc == Cyc_Absyn_ExternC)_tmpA1A->sc=Cyc_Absyn_Public;
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmpA1A->initializer));else{
# 5143
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmpA1A->initializer));}}
# 5145
{struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmpE69;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpE68;nv=Cyc_Toc_add_varmap(r,nv,_tmpA1A->name,Cyc_Absyn_varb_exp(_tmpA32,(void*)((_tmpE68=_cycalloc(sizeof(*_tmpE68)),((_tmpE68[0]=((_tmpE69.tag=1,((_tmpE69.f1=_tmpA1A,_tmpE69)))),_tmpE68)))),0));}
_tmpA1A->name=_tmpA32;
_tmpA1A->sc=Cyc_Toc_scope_to_c(_tmpA1A->sc);
_tmpA1A->type=Cyc_Toc_typ_to_c(_tmpA1A->type);
{struct Cyc_List_List*_tmpE6A;Cyc_Toc_result_decls=((_tmpE6A=_cycalloc(sizeof(*_tmpE6A)),((_tmpE6A->hd=d,((_tmpE6A->tl=Cyc_Toc_result_decls,_tmpE6A))))));}
goto _LL44A;}_LL44D: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpA1B=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpA18;if(_tmpA1B->tag != 1)goto _LL44F;else{_tmpA1C=_tmpA1B->f1;}}_LL44E: {
# 5152
struct _tuple1*_tmpA37=_tmpA1C->name;
# 5154
if(_tmpA1C->sc == Cyc_Absyn_ExternC){
{struct _tuple1*_tmpE6B;_tmpA37=((_tmpE6B=_cycalloc(sizeof(*_tmpE6B)),((_tmpE6B->f1=Cyc_Absyn_Abs_n(0,1),((_tmpE6B->f2=(*_tmpA37).f2,_tmpE6B))))));}
_tmpA1C->sc=Cyc_Absyn_Public;}
# 5158
nv=Cyc_Toc_add_varmap(r,nv,_tmpA1C->name,Cyc_Absyn_var_exp(_tmpA37,0));
_tmpA1C->name=_tmpA37;
Cyc_Toc_fndecl_to_c(nv,_tmpA1C,cinclude);
{struct Cyc_List_List*_tmpE6C;Cyc_Toc_result_decls=((_tmpE6C=_cycalloc(sizeof(*_tmpE6C)),((_tmpE6C->hd=d,((_tmpE6C->tl=Cyc_Toc_result_decls,_tmpE6C))))));}
goto _LL44A;}_LL44F: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmpA1D=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpA18;if(_tmpA1D->tag != 2)goto _LL451;}_LL450:
 goto _LL452;_LL451: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmpA1E=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmpA18;if(_tmpA1E->tag != 3)goto _LL453;}_LL452: {
# 5165
const char*_tmpE6F;void*_tmpE6E;(_tmpE6E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE6F="letdecl at toplevel",_tag_dyneither(_tmpE6F,sizeof(char),20))),_tag_dyneither(_tmpE6E,sizeof(void*),0)));}_LL453: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmpA1F=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpA18;if(_tmpA1F->tag != 4)goto _LL455;}_LL454: {
# 5167
const char*_tmpE72;void*_tmpE71;(_tmpE71=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE72="region decl at toplevel",_tag_dyneither(_tmpE72,sizeof(char),24))),_tag_dyneither(_tmpE71,sizeof(void*),0)));}_LL455: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpA20=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpA18;if(_tmpA20->tag != 5)goto _LL457;else{_tmpA21=_tmpA20->f1;}}_LL456:
# 5171
 Cyc_Toc_aggrdecl_to_c(_tmpA21,1);
goto _LL44A;_LL457: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmpA22=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmpA18;if(_tmpA22->tag != 6)goto _LL459;else{_tmpA23=_tmpA22->f1;}}_LL458:
# 5174
 if(_tmpA23->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmpA23);else{
# 5177
Cyc_Toc_datatypedecl_to_c(_tmpA23);}
goto _LL44A;_LL459: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmpA24=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpA18;if(_tmpA24->tag != 7)goto _LL45B;else{_tmpA25=_tmpA24->f1;}}_LL45A:
# 5180
 Cyc_Toc_enumdecl_to_c(_tmpA25);
{struct Cyc_List_List*_tmpE73;Cyc_Toc_result_decls=((_tmpE73=_cycalloc(sizeof(*_tmpE73)),((_tmpE73->hd=d,((_tmpE73->tl=Cyc_Toc_result_decls,_tmpE73))))));}
goto _LL44A;_LL45B: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmpA26=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpA18;if(_tmpA26->tag != 8)goto _LL45D;else{_tmpA27=_tmpA26->f1;}}_LL45C:
# 5184
 _tmpA27->tvs=0;
if(_tmpA27->defn != 0)
_tmpA27->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmpA27->defn));else{
# 5188
enum Cyc_Absyn_KindQual _tmpA3F=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmpA27->kind))->v)->kind;switch(_tmpA3F){case Cyc_Absyn_BoxKind: _LL469:
 _tmpA27->defn=Cyc_Absyn_void_star_typ();break;default: _LL46A:
 _tmpA27->defn=(void*)& Cyc_Absyn_VoidType_val;break;}}
# 5193
{struct Cyc_List_List*_tmpE74;Cyc_Toc_result_decls=((_tmpE74=_cycalloc(sizeof(*_tmpE74)),((_tmpE74->hd=d,((_tmpE74->tl=Cyc_Toc_result_decls,_tmpE74))))));}
goto _LL44A;_LL45D: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmpA28=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmpA18;if(_tmpA28->tag != 13)goto _LL45F;}_LL45E:
 goto _LL460;_LL45F: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmpA29=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmpA18;if(_tmpA29->tag != 14)goto _LL461;}_LL460:
# 5197
 goto _LL44A;_LL461: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpA2A=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpA18;if(_tmpA2A->tag != 9)goto _LL463;else{_tmpA2B=_tmpA2A->f2;}}_LL462:
 _tmpA2D=_tmpA2B;goto _LL464;_LL463: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmpA2C=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpA18;if(_tmpA2C->tag != 10)goto _LL465;else{_tmpA2D=_tmpA2C->f2;}}_LL464:
 _tmpA2F=_tmpA2D;goto _LL466;_LL465: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmpA2E=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmpA18;if(_tmpA2E->tag != 11)goto _LL467;else{_tmpA2F=_tmpA2E->f1;}}_LL466:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmpA2F,top,cinclude);goto _LL44A;_LL467: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA30=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmpA18;if(_tmpA30->tag != 12)goto _LL44A;else{_tmpA31=_tmpA30->f1;}}_LL468:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmpA31,top,1);goto _LL44A;_LL44A:;};}
# 5204
return nv;}
# 5208
static void Cyc_Toc_init(){
struct Cyc_Core_NewDynamicRegion _tmpA41=Cyc_Core_new_rckey();struct Cyc_Core_DynamicRegion*_tmpA43;struct Cyc_Core_NewDynamicRegion _tmpA42=_tmpA41;_tmpA43=_tmpA42.key;{
struct Cyc_Toc_TocState*_tmpA44=((struct Cyc_Toc_TocState*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Toc_TocState*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmpA43,0,Cyc_Toc_empty_toc_state);
{struct Cyc_Toc_TocStateWrap*_tmpE75;Cyc_Toc_toc_state=((_tmpE75=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE75)),((_tmpE75->dyn=(struct Cyc_Core_DynamicRegion*)_tmpA43,((_tmpE75->state=(struct Cyc_Toc_TocState*)_tmpA44,_tmpE75))))));}
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0;
Cyc_Toc_bounds_checks_eliminated=0;{
struct _dyneither_ptr**_tmpE76;Cyc_Toc_globals=_tag_dyneither(((_tmpE76=_cycalloc(sizeof(struct _dyneither_ptr*)* 41),((_tmpE76[0]=(struct _dyneither_ptr*)& Cyc_Toc__throw_str,((_tmpE76[1]=(struct _dyneither_ptr*)& Cyc_Toc_setjmp_str,((_tmpE76[2]=(struct _dyneither_ptr*)& Cyc_Toc__push_handler_str,((_tmpE76[3]=(struct _dyneither_ptr*)& Cyc_Toc__pop_handler_str,((_tmpE76[4]=(struct _dyneither_ptr*)& Cyc_Toc__exn_thrown_str,((_tmpE76[5]=(struct _dyneither_ptr*)& Cyc_Toc__npop_handler_str,((_tmpE76[6]=(struct _dyneither_ptr*)& Cyc_Toc__check_null_str,((_tmpE76[7]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_null_str,((_tmpE76[8]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_notnull_str,((_tmpE76[9]=(struct _dyneither_ptr*)& Cyc_Toc__check_dyneither_subscript_str,((_tmpE76[10]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_str,((_tmpE76[11]=(struct _dyneither_ptr*)& Cyc_Toc__tag_dyneither_str,((_tmpE76[12]=(struct _dyneither_ptr*)& Cyc_Toc__init_dyneither_ptr_str,((_tmpE76[13]=(struct _dyneither_ptr*)& Cyc_Toc__untag_dyneither_ptr_str,((_tmpE76[14]=(struct _dyneither_ptr*)& Cyc_Toc__get_dyneither_size_str,((_tmpE76[15]=(struct _dyneither_ptr*)& Cyc_Toc__get_zero_arr_size_str,((_tmpE76[16]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_plus_str,((_tmpE76[17]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_plus_str,((_tmpE76[18]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpE76[19]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_str,((_tmpE76[20]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpE76[21]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_post_str,((_tmpE76[22]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_str,((_tmpE76[23]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_str,((_tmpE76[24]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_atomic_str,((_tmpE76[25]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_atomic_str,((_tmpE76[26]=(struct _dyneither_ptr*)& Cyc_Toc__region_malloc_str,((_tmpE76[27]=(struct _dyneither_ptr*)& Cyc_Toc__region_calloc_str,((_tmpE76[28]=(struct _dyneither_ptr*)& Cyc_Toc__check_times_str,((_tmpE76[29]=(struct _dyneither_ptr*)& Cyc_Toc__new_region_str,((_tmpE76[30]=(struct _dyneither_ptr*)& Cyc_Toc__push_region_str,((_tmpE76[31]=(struct _dyneither_ptr*)& Cyc_Toc__pop_region_str,((_tmpE76[32]=(struct _dyneither_ptr*)& Cyc_Toc__open_dynregion_str,((_tmpE76[33]=(struct _dyneither_ptr*)& Cyc_Toc__push_dynregion_str,((_tmpE76[34]=(struct _dyneither_ptr*)& Cyc_Toc__pop_dynregion_str,((_tmpE76[35]=(struct _dyneither_ptr*)& Cyc_Toc__reset_region_str,((_tmpE76[36]=(struct _dyneither_ptr*)& Cyc_Toc__throw_arraybounds_str,((_tmpE76[37]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_decrease_size_str,((_tmpE76[38]=(struct _dyneither_ptr*)& Cyc_Toc__throw_match_str,((_tmpE76[39]=(struct _dyneither_ptr*)& Cyc_Toc__swap_word_str,((_tmpE76[40]=(struct _dyneither_ptr*)& Cyc_Toc__swap_dyneither_str,_tmpE76)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),sizeof(struct _dyneither_ptr*),41);};};}
# 5264
static int Cyc_Toc_destroy_labels(struct _RegionHandle*d,struct Cyc_Toc_TocState*ts){
struct Cyc_Toc_TocState _tmpA47=*ts;struct Cyc_Xarray_Xarray*_tmpA49;struct Cyc_Toc_TocState _tmpA48=_tmpA47;_tmpA49=_tmpA48.temp_labels;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmpA49);
return 0;}
# 5272
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){
Cyc_Toc_init();{
struct _RegionHandle _tmpA4A=_new_region("start");struct _RegionHandle*start=& _tmpA4A;_push_region(start);
Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(start),ds,1,0);{
# 5281
struct Cyc_Toc_TocStateWrap*ts=0;
_swap_word(& ts,& Cyc_Toc_toc_state);{
struct Cyc_Toc_TocStateWrap _tmpA4B=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Core_DynamicRegion*_tmpA4D;struct Cyc_Toc_TocState*_tmpA4E;struct Cyc_Toc_TocStateWrap _tmpA4C=_tmpA4B;_tmpA4D=_tmpA4C.dyn;_tmpA4E=_tmpA4C.state;
((int(*)(struct Cyc_Core_DynamicRegion*key,struct Cyc_Toc_TocState*arg,int(*body)(struct _RegionHandle*h,struct Cyc_Toc_TocState*arg)))Cyc_Core_open_region)(_tmpA4D,_tmpA4E,Cyc_Toc_destroy_labels);
Cyc_Core_free_rckey(_tmpA4D);
((void(*)(struct _dyneither_ptr ptr))Cyc_Core_ufree)(_tag_dyneither(ts,sizeof(struct Cyc_Toc_TocStateWrap),1));};};{
# 5288
struct Cyc_List_List*_tmpA4F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0);return _tmpA4F;};
# 5274
;_pop_region(start);};}
