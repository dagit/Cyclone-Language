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
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 442 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
# 445
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 458
typedef void*Cyc_Absyn_funcparams_t;
# 461
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 522
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 538
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 540
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign,long long);
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr,int);
# 548
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 555
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 573
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 702 "absyn.h"
extern struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val;struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 737
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
# 765
extern struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 920
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 927
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 937
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 940
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
# 947
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 950
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 965
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 967
extern void*Cyc_Absyn_sint_typ;
# 979
void*Cyc_Absyn_exn_typ();
# 990
extern void*Cyc_Absyn_bounds_one;
# 998
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1004
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);
# 1006
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1009
void*Cyc_Absyn_void_star_typ();
# 1011
void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple1*name);
void*Cyc_Absyn_unionq_typ(struct _tuple1*name);
# 1017
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1021
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1023
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1029
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);
# 1034
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
# 1037
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple1*,void*,unsigned int);
# 1041
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1050
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1053
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1056
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1061
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1065
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1067
struct Cyc_Absyn_Exp*Cyc_Absyn_rethrow_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1070
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
# 1075
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1080
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1086
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned int);
# 1092
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1102
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned int loc);
# 1105
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned int loc);
# 1109
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1111
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc);
# 1116
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1120
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1125
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
# 1168
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1171
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1181
struct _dyneither_ptr*Cyc_Absyn_fieldname(int);
# 1185
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1201
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
typedef void*Cyc_Tcenv_jumpee_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};
# 100
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 137 "tcenv.h"
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
struct Cyc_Dict_Dict*_tmpA4C;struct Cyc_Dict_Dict*_tmpA4B;struct Cyc_Set_Set**_tmpA4A;struct Cyc_List_List**_tmpA49;struct Cyc_Dict_Dict*_tmpA48;struct Cyc_List_List**_tmpA47;struct Cyc_Toc_TocState*_tmpA46;return(_tmpA46=_region_malloc(d,sizeof(*_tmpA46)),((_tmpA46->tuple_types=(
(_tmpA47=_region_malloc(d,sizeof(*_tmpA47)),((_tmpA47[0]=0,_tmpA47)))),((_tmpA46->aggrs_so_far=(
(_tmpA48=_region_malloc(d,sizeof(*_tmpA48)),((_tmpA48[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpA48)))),((_tmpA46->abs_struct_types=(
(_tmpA49=_region_malloc(d,sizeof(*_tmpA49)),((_tmpA49[0]=0,_tmpA49)))),((_tmpA46->datatypes_so_far=(
(_tmpA4A=_region_malloc(d,sizeof(*_tmpA4A)),((_tmpA4A[0]=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpA4A)))),((_tmpA46->xdatatypes_so_far=(
(_tmpA4B=_region_malloc(d,sizeof(*_tmpA4B)),((_tmpA4B[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpA4B)))),((_tmpA46->qvar_tags=(
(_tmpA4C=_region_malloc(d,sizeof(*_tmpA4C)),((_tmpA4C[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),_tmpA4C)))),((_tmpA46->temp_labels=
((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d),_tmpA46)))))))))))))));}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
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
{struct Cyc_Toc_TocStateWrap _tmpA4D;*ts=((_tmpA4D.dyn=(struct Cyc_Core_DynamicRegion*)_tmp11,((_tmpA4D.state=(struct Cyc_Toc_TocState*)_tmp12,_tmpA4D))));}
_swap_word(& ts,& Cyc_Toc_toc_state);{
struct _tuple15 _tmpA4E;struct _tuple15 _tmp15=(_tmpA4E.f1=_tmp12,((_tmpA4E.f2=arg,_tmpA4E)));
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
struct _tuple16 _tmpA4F;struct _tuple16 env=(_tmpA4F.f1=q,((_tmpA4F.f2=type_maker,_tmpA4F)));
return((void*(*)(struct _tuple16*arg,void*(*f)(struct _RegionHandle*,struct _tuple17*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_aggrdecl_type_body);}
# 172
static int Cyc_Toc_tuple_type_counter=0;
static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;char Cyc_Toc_Toc_Unimplemented[18]="Toc_Unimplemented";struct Cyc_Toc_Toc_Unimplemented_exn_struct{char*tag;};char Cyc_Toc_Toc_Impossible[15]="Toc_Impossible";struct Cyc_Toc_Toc_Impossible_exn_struct{char*tag;};
# 179
static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 182
Cyc_vfprintf(Cyc_stderr,fmt,ap);
{const char*_tmpA52;void*_tmpA51;(_tmpA51=0,Cyc_fprintf(Cyc_stderr,((_tmpA52="\n",_tag_dyneither(_tmpA52,sizeof(char),2))),_tag_dyneither(_tmpA51,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Unimplemented_exn_struct _tmpA55;struct Cyc_Toc_Toc_Unimplemented_exn_struct*_tmpA54;(int)_throw((void*)((_tmpA54=_cycalloc_atomic(sizeof(*_tmpA54)),((_tmpA54[0]=((_tmpA55.tag=Cyc_Toc_Toc_Unimplemented,_tmpA55)),_tmpA54)))));};}
# 187
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 190
Cyc_vfprintf(Cyc_stderr,fmt,ap);
{const char*_tmpA58;void*_tmpA57;(_tmpA57=0,Cyc_fprintf(Cyc_stderr,((_tmpA58="\n",_tag_dyneither(_tmpA58,sizeof(char),2))),_tag_dyneither(_tmpA57,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Impossible_exn_struct _tmpA5B;struct Cyc_Toc_Toc_Impossible_exn_struct*_tmpA5A;(int)_throw((void*)((_tmpA5A=_cycalloc_atomic(sizeof(*_tmpA5A)),((_tmpA5A[0]=((_tmpA5B.tag=Cyc_Toc_Toc_Impossible,_tmpA5B)),_tmpA5A)))));};}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_exn_struct{char*tag;};static char _tmp29[5]="curr";
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
struct Cyc_Absyn_Stmt**_tmpA5C;skip_stmt_opt=((_tmpA5C=_cycalloc(sizeof(*_tmpA5C)),((_tmpA5C[0]=Cyc_Absyn_skip_stmt(0),_tmpA5C))));}
return*skip_stmt_opt;}
# 317
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}
# 320
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmpA5F;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpA5E;return(void*)((_tmpA5E=_cycalloc(sizeof(*_tmpA5E)),((_tmpA5E[0]=((_tmpA5F.tag=13,((_tmpA5F.f1=t,((_tmpA5F.f2=e,((_tmpA5F.f3=0,((_tmpA5F.f4=Cyc_Absyn_No_coercion,_tmpA5F)))))))))),_tmpA5E))));}
# 323
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmpA62;struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpA61;return(void*)((_tmpA61=_cycalloc(sizeof(*_tmpA61)),((_tmpA61[0]=((_tmpA62.tag=19,((_tmpA62.f1=e,_tmpA62)))),_tmpA61))));}
# 326
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmpA65;struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpA64;return(void*)((_tmpA64=_cycalloc(sizeof(*_tmpA64)),((_tmpA64[0]=((_tmpA65.tag=22,((_tmpA65.f1=e1,((_tmpA65.f2=e2,_tmpA65)))))),_tmpA64))));}
# 329
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmpA68;struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmpA67;return(void*)((_tmpA67=_cycalloc(sizeof(*_tmpA67)),((_tmpA67[0]=((_tmpA68.tag=36,((_tmpA68.f1=s,_tmpA68)))),_tmpA67))));}
# 332
static void*Cyc_Toc_sizeoftyp_exp_r(void*t){
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpA6B;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpA6A;return(void*)((_tmpA6A=_cycalloc(sizeof(*_tmpA6A)),((_tmpA6A[0]=((_tmpA6B.tag=16,((_tmpA6B.f1=t,_tmpA6B)))),_tmpA6A))));}
# 335
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpA6E;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpA6D;return(void*)((_tmpA6D=_cycalloc(sizeof(*_tmpA6D)),((_tmpA6D[0]=((_tmpA6E.tag=9,((_tmpA6E.f1=e,((_tmpA6E.f2=es,((_tmpA6E.f3=0,((_tmpA6E.f4=1,_tmpA6E)))))))))),_tmpA6D))));}
# 338
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmpA71;struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpA70;return(void*)((_tmpA70=_cycalloc(sizeof(*_tmpA70)),((_tmpA70[0]=((_tmpA71.tag=1,((_tmpA71.f1=e,_tmpA71)))),_tmpA70))));}
# 341
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmpA74;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpA73;return(void*)((_tmpA73=_cycalloc(sizeof(*_tmpA73)),((_tmpA73[0]=((_tmpA74.tag=2,((_tmpA74.f1=s1,((_tmpA74.f2=s2,_tmpA74)))))),_tmpA73))));}
# 344
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmpA77;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpA76;return(void*)((_tmpA76=_cycalloc(sizeof(*_tmpA76)),((_tmpA76[0]=((_tmpA77.tag=5,((_tmpA77.f1=e1,((_tmpA77.f2=e2,((_tmpA77.f3=e3,_tmpA77)))))))),_tmpA76))));}
# 347
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpA7A;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpA79;return(void*)((_tmpA79=_cycalloc(sizeof(*_tmpA79)),((_tmpA79[0]=((_tmpA7A.tag=20,((_tmpA7A.f1=e,((_tmpA7A.f2=n,((_tmpA7A.f3=0,((_tmpA7A.f4=0,_tmpA7A)))))))))),_tmpA79))));}
# 350
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpA7D;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpA7C;return(void*)((_tmpA7C=_cycalloc(sizeof(*_tmpA7C)),((_tmpA7C[0]=((_tmpA7D.tag=21,((_tmpA7D.f1=e,((_tmpA7D.f2=n,((_tmpA7D.f3=0,((_tmpA7D.f4=0,_tmpA7D)))))))))),_tmpA7C))));}
# 353
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 356
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpA80;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpA7F;return(void*)((_tmpA7F=_cycalloc(sizeof(*_tmpA7F)),((_tmpA7F[0]=((_tmpA80.tag=35,((_tmpA80.f1=tdopt,((_tmpA80.f2=ds,_tmpA80)))))),_tmpA7F))));}
# 358
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmpA83;struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmpA82;return(void*)((_tmpA82=_cycalloc(sizeof(*_tmpA82)),((_tmpA82[0]=((_tmpA83.tag=8,((_tmpA83.f1=v,((_tmpA83.f2=s,_tmpA83)))))),_tmpA82))));}
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
struct Cyc_Core_Impossible_exn_struct _tmpA89;const char*_tmpA88;struct Cyc_Core_Impossible_exn_struct*_tmpA87;(int)_throw((void*)((_tmpA87=_cycalloc(sizeof(*_tmpA87)),((_tmpA87[0]=((_tmpA89.tag=Cyc_Core_Impossible,((_tmpA89.f1=((_tmpA88="impossible IntType (not char, short or int)",_tag_dyneither(_tmpA88,sizeof(char),44))),_tmpA89)))),_tmpA87)))));}}
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
struct Cyc_Core_Impossible_exn_struct _tmpA96;const char*_tmpA95;void*_tmpA94[1];struct Cyc_String_pa_PrintArg_struct _tmpA93;struct Cyc_Core_Impossible_exn_struct*_tmpA92;(int)_throw((void*)((_tmpA92=_cycalloc(sizeof(*_tmpA92)),((_tmpA92[0]=((_tmpA96.tag=Cyc_Core_Impossible,((_tmpA96.f1=(struct _dyneither_ptr)((_tmpA93.tag=0,((_tmpA93.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA94[0]=& _tmpA93,Cyc_aprintf(((_tmpA95="impossible expression type %s (not int, float, double, or pointer)",_tag_dyneither(_tmpA95,sizeof(char),67))),_tag_dyneither(_tmpA94,sizeof(void*),1)))))))),_tmpA96)))),_tmpA92)))));}_LL5:;}
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
struct Cyc_Core_Impossible_exn_struct _tmpA9C;const char*_tmpA9B;struct Cyc_Core_Impossible_exn_struct*_tmpA9A;(int)_throw((void*)((_tmpA9A=_cycalloc(sizeof(*_tmpA9A)),((_tmpA9A[0]=((_tmpA9C.tag=Cyc_Core_Impossible,((_tmpA9C.f1=((_tmpA9B="impossible type (not pointer)",_tag_dyneither(_tmpA9B,sizeof(char),30))),_tmpA9C)))),_tmpA9A)))));}_LL17:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
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
const char*_tmpA9F;void*_tmpA9E;(_tmpA9E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA9F="is_nullable",_tag_dyneither(_tmpA9F,sizeof(char),12))),_tag_dyneither(_tmpA9E,sizeof(void*),0)));}_LL37:;}
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
char*_tmpAA1;unsigned int _tmpAA0;struct _dyneither_ptr buf=(_tmpAA0=len,((_tmpAA1=_cyccalloc_atomic(sizeof(char),_tmpAA0),_tag_dyneither(_tmpAA1,sizeof(char),_tmpAA0))));
struct _dyneither_ptr p=buf;
Cyc_strcpy(p,(struct _dyneither_ptr)s);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)slen);
for(0;nms != 0;nms=nms->tl){
struct _dyneither_ptr*_tmp11D=(struct _dyneither_ptr*)nms->hd;struct _dyneither_ptr _tmp11F;struct _dyneither_ptr*_tmp11E=_tmp11D;_tmp11F=*_tmp11E;
{char _tmpAA4;char _tmpAA3;struct _dyneither_ptr _tmpAA2;(_tmpAA2=p,((_tmpAA3=*((char*)_check_dyneither_subscript(_tmpAA2,sizeof(char),0)),((_tmpAA4='_',((_get_dyneither_size(_tmpAA2,sizeof(char))== 1  && (_tmpAA3 == '\000'  && _tmpAA4 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpAA2.curr)=_tmpAA4)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp11F);
_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)Cyc_strlen((struct _dyneither_ptr)_tmp11F));}
# 528
{char _tmpAA7;char _tmpAA6;struct _dyneither_ptr _tmpAA5;(_tmpAA5=p,((_tmpAA6=*((char*)_check_dyneither_subscript(_tmpAA5,sizeof(char),0)),((_tmpAA7='_',((_get_dyneither_size(_tmpAA5,sizeof(char))== 1  && (_tmpAA6 == '\000'  && _tmpAA7 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpAA5.curr)=_tmpAA7)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp117);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)vlen);
{const char*_tmpAA8;Cyc_strcpy(p,((_tmpAA8="_struct",_tag_dyneither(_tmpAA8,sizeof(char),8))));}
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
struct _tuple14*_tmpAA9;struct _tuple14*_tmp139=(_tmpAA9=_cycalloc(sizeof(*_tmpAA9)),((_tmpAA9->f1=_tmp12F,((_tmpAA9->f2=_tmp130,_tmpAA9)))));
union Cyc_Absyn_Nmspace _tmp13B;struct _dyneither_ptr _tmp13C;struct _tuple1*_tmp13A=_tmp12F;_tmp13B=_tmp13A->f1;_tmp13C=*_tmp13A->f2;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmp13C,_tmp130);
struct _dyneither_ptr*_tmpAAC;struct _tuple1*_tmpAAB;struct _tuple1*res=(_tmpAAB=_cycalloc(sizeof(*_tmpAAB)),((_tmpAAB->f1=_tmp13B,((_tmpAAB->f2=((_tmpAAC=_cycalloc(sizeof(*_tmpAAC)),((_tmpAAC[0]=newvar,_tmpAAC)))),_tmpAAB)))));
*_tmp12B=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmp12B,_tmp139,res);
return res;};}_LL48: _tmp138=_tmp136;_LL49:(void)_rethrow(_tmp138);_LL45:;}};};};}
# 562
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple14 _tmpAAD;struct _tuple14 env=(_tmpAAD.f1=fieldname,((_tmpAAD.f2=dtname,_tmpAAD)));
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
struct Cyc_Int_pa_PrintArg_struct _tmpAB5;void*_tmpAB4[1];const char*_tmpAB3;struct _dyneither_ptr*_tmpAB2;struct _dyneither_ptr*xname=(_tmpAB2=_cycalloc(sizeof(*_tmpAB2)),((_tmpAB2[0]=(struct _dyneither_ptr)((_tmpAB5.tag=1,((_tmpAB5.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpAB4[0]=& _tmpAB5,Cyc_aprintf(((_tmpAB3="_tuple%d",_tag_dyneither(_tmpAB3,sizeof(char),9))),_tag_dyneither(_tmpAB4,sizeof(void*),1)))))))),_tmpAB2)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,_tmp145);
struct Cyc_List_List*_tmp14C=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
struct Cyc_Absyn_Aggrfield*_tmpAB8;struct Cyc_List_List*_tmpAB7;_tmp14C=((_tmpAB7=_cycalloc(sizeof(*_tmpAB7)),((_tmpAB7->hd=((_tmpAB8=_cycalloc(sizeof(*_tmpAB8)),((_tmpAB8->name=Cyc_Absyn_fieldname(i),((_tmpAB8->tq=Cyc_Toc_mt_tq,((_tmpAB8->type=(void*)ts2->hd,((_tmpAB8->width=0,((_tmpAB8->attributes=0,((_tmpAB8->requires_clause=0,_tmpAB8)))))))))))))),((_tmpAB7->tl=_tmp14C,_tmpAB7))))));}}
_tmp14C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp14C);{
struct Cyc_Absyn_AggrdeclImpl*_tmpABD;struct _tuple1*_tmpABC;struct Cyc_Absyn_Aggrdecl*_tmpABB;struct Cyc_Absyn_Aggrdecl*_tmp14F=(_tmpABB=_cycalloc(sizeof(*_tmpABB)),((_tmpABB->kind=Cyc_Absyn_StructA,((_tmpABB->sc=Cyc_Absyn_Public,((_tmpABB->name=(
(_tmpABC=_cycalloc(sizeof(*_tmpABC)),((_tmpABC->f1=Cyc_Absyn_Rel_n(0),((_tmpABC->f2=xname,_tmpABC)))))),((_tmpABB->tvs=0,((_tmpABB->impl=(
(_tmpABD=_cycalloc(sizeof(*_tmpABD)),((_tmpABD->exist_vars=0,((_tmpABD->rgn_po=0,((_tmpABD->fields=_tmp14C,((_tmpABD->tagged=0,_tmpABD)))))))))),((_tmpABB->attributes=0,_tmpABB)))))))))))));
# 601
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpAC3;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpAC2;struct Cyc_List_List*_tmpAC1;Cyc_Toc_result_decls=((_tmpAC1=_cycalloc(sizeof(*_tmpAC1)),((_tmpAC1->hd=Cyc_Absyn_new_decl((void*)((_tmpAC3=_cycalloc(sizeof(*_tmpAC3)),((_tmpAC3[0]=((_tmpAC2.tag=5,((_tmpAC2.f1=_tmp14F,_tmpAC2)))),_tmpAC3)))),0),((_tmpAC1->tl=Cyc_Toc_result_decls,_tmpAC1))))));}
{struct _tuple22*_tmpAC6;struct Cyc_List_List*_tmpAC5;*_tmp144=((_tmpAC5=_region_malloc(d,sizeof(*_tmpAC5)),((_tmpAC5->hd=((_tmpAC6=_region_malloc(d,sizeof(*_tmpAC6)),((_tmpAC6->f1=x,((_tmpAC6->f2=ts,_tmpAC6)))))),((_tmpAC5->tl=*_tmp144,_tmpAC5))))));}
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
struct Cyc_Int_pa_PrintArg_struct _tmpACE;void*_tmpACD[1];const char*_tmpACC;struct _dyneither_ptr*_tmpACB;struct _dyneither_ptr*xname=(_tmpACB=_cycalloc(sizeof(*_tmpACB)),((_tmpACB[0]=(struct _dyneither_ptr)((_tmpACE.tag=1,((_tmpACE.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpACD[0]=& _tmpACE,Cyc_aprintf(((_tmpACC="_tuple%d",_tag_dyneither(_tmpACC,sizeof(char),9))),_tag_dyneither(_tmpACD,sizeof(void*),1)))))))),_tmpACB)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp16E=0;
# 670
{struct _tuple25*_tmpAD1;struct Cyc_List_List*_tmpAD0;*_tmp15E=((_tmpAD0=_region_malloc(d,sizeof(*_tmpAD0)),((_tmpAD0->hd=((_tmpAD1=_region_malloc(d,sizeof(*_tmpAD1)),((_tmpAD1->f1=_tmp15F,((_tmpAD1->f2=_tmp161,((_tmpAD1->f3=x,_tmpAD1)))))))),((_tmpAD0->tl=*_tmp15E,_tmpAD0))))));}{
# 673
struct _RegionHandle _tmp171=_new_region("r");struct _RegionHandle*r=& _tmp171;_push_region(r);
{struct Cyc_List_List*_tmp172=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp160,_tmp161);
for(0;_tmp162 != 0;_tmp162=_tmp162->tl){
struct Cyc_Absyn_Aggrfield*_tmp173=(struct Cyc_Absyn_Aggrfield*)_tmp162->hd;
void*t=_tmp173->type;
struct Cyc_List_List*atts=_tmp173->attributes;
# 681
if(_tmp162->tl == 0  && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(t))){
struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpAD7;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpAD6;struct Cyc_List_List*_tmpAD5;atts=((_tmpAD5=_cycalloc(sizeof(*_tmpAD5)),((_tmpAD5->hd=(void*)((_tmpAD7=_cycalloc(sizeof(*_tmpAD7)),((_tmpAD7[0]=((_tmpAD6.tag=6,((_tmpAD6.f1=0,_tmpAD6)))),_tmpAD7)))),((_tmpAD5->tl=atts,_tmpAD5))))));}
# 684
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp172,t));
# 687
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val)){
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpADD;struct Cyc_Absyn_ArrayInfo _tmpADC;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpADB;t=(void*)((_tmpADB=_cycalloc(sizeof(*_tmpADB)),((_tmpADB[0]=((_tmpADD.tag=8,((_tmpADD.f1=((_tmpADC.elt_type=Cyc_Absyn_void_star_typ(),((_tmpADC.tq=Cyc_Absyn_empty_tqual(0),((_tmpADC.num_elts=
Cyc_Absyn_uint_exp(0,0),((_tmpADC.zero_term=((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0),((_tmpADC.zt_loc=0,_tmpADC)))))))))),_tmpADD)))),_tmpADB))));}{
# 691
struct Cyc_Absyn_Aggrfield*_tmpAE0;struct Cyc_List_List*_tmpADF;_tmp16E=((_tmpADF=_cycalloc(sizeof(*_tmpADF)),((_tmpADF->hd=((_tmpAE0=_cycalloc(sizeof(*_tmpAE0)),((_tmpAE0->name=_tmp173->name,((_tmpAE0->tq=Cyc_Toc_mt_tq,((_tmpAE0->type=t,((_tmpAE0->width=_tmp173->width,((_tmpAE0->attributes=atts,((_tmpAE0->requires_clause=0,_tmpAE0)))))))))))))),((_tmpADF->tl=_tmp16E,_tmpADF))))));};}
# 693
_tmp16E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp16E);{
struct Cyc_Absyn_AggrdeclImpl*_tmpAE5;struct _tuple1*_tmpAE4;struct Cyc_Absyn_Aggrdecl*_tmpAE3;struct Cyc_Absyn_Aggrdecl*_tmp17C=(_tmpAE3=_cycalloc(sizeof(*_tmpAE3)),((_tmpAE3->kind=Cyc_Absyn_StructA,((_tmpAE3->sc=Cyc_Absyn_Public,((_tmpAE3->name=(
(_tmpAE4=_cycalloc(sizeof(*_tmpAE4)),((_tmpAE4->f1=Cyc_Absyn_Rel_n(0),((_tmpAE4->f2=xname,_tmpAE4)))))),((_tmpAE3->tvs=0,((_tmpAE3->impl=(
(_tmpAE5=_cycalloc(sizeof(*_tmpAE5)),((_tmpAE5->exist_vars=0,((_tmpAE5->rgn_po=0,((_tmpAE5->fields=_tmp16E,((_tmpAE5->tagged=0,_tmpAE5)))))))))),((_tmpAE3->attributes=0,_tmpAE3)))))))))))));
# 701
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpAEB;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpAEA;struct Cyc_List_List*_tmpAE9;Cyc_Toc_result_decls=((_tmpAE9=_cycalloc(sizeof(*_tmpAE9)),((_tmpAE9->hd=Cyc_Absyn_new_decl((void*)((_tmpAEB=_cycalloc(sizeof(*_tmpAEB)),((_tmpAEB[0]=((_tmpAEA.tag=5,((_tmpAEA.f1=_tmp17C,_tmpAEA)))),_tmpAEB)))),0),((_tmpAE9->tl=Cyc_Toc_result_decls,_tmpAE9))))));}{
void*_tmp180=x;_npop_handler(0);return _tmp180;};};}
# 674
;_pop_region(r);};};}
# 705
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 709
struct _tuple23 _tmpAEC;struct _tuple23 env=(_tmpAEC.f1=struct_name,((_tmpAEC.f2=type_vars,((_tmpAEC.f3=type_args,((_tmpAEC.f4=fields,_tmpAEC)))))));
return((void*(*)(struct _tuple23*arg,void*(*f)(struct _RegionHandle*,struct _tuple24*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 716
struct _tuple1*Cyc_Toc_temp_var(){
int _tmp189=Cyc_Toc_temp_var_counter ++;
struct _dyneither_ptr*_tmpAF9;const char*_tmpAF8;void*_tmpAF7[1];struct Cyc_Int_pa_PrintArg_struct _tmpAF6;struct _tuple1*_tmpAF5;struct _tuple1*res=(_tmpAF5=_cycalloc(sizeof(*_tmpAF5)),((_tmpAF5->f1=Cyc_Absyn_Loc_n,((_tmpAF5->f2=((_tmpAF9=_cycalloc(sizeof(*_tmpAF9)),((_tmpAF9[0]=(struct _dyneither_ptr)((_tmpAF6.tag=1,((_tmpAF6.f1=(unsigned int)_tmp189,((_tmpAF7[0]=& _tmpAF6,Cyc_aprintf(((_tmpAF8="_tmp%X",_tag_dyneither(_tmpAF8,sizeof(char),7))),_tag_dyneither(_tmpAF7,sizeof(void*),1)))))))),_tmpAF9)))),_tmpAF5)))));
return res;}struct _tuple26{struct Cyc_Toc_TocState*f1;int f2;};
# 724
static struct _dyneither_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple26*env){
struct _tuple26 _tmp18F=*env;struct Cyc_Xarray_Xarray*_tmp191;struct _tuple26 _tmp190=_tmp18F;_tmp191=(_tmp190.f1)->temp_labels;{
int _tmp192=Cyc_Toc_fresh_label_counter ++;
if(_tmp192 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp191))
return((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp191,_tmp192);{
struct Cyc_Int_pa_PrintArg_struct _tmpB01;void*_tmpB00[1];const char*_tmpAFF;struct _dyneither_ptr*_tmpAFE;struct _dyneither_ptr*res=(_tmpAFE=_cycalloc(sizeof(*_tmpAFE)),((_tmpAFE[0]=(struct _dyneither_ptr)((_tmpB01.tag=1,((_tmpB01.f1=(unsigned int)_tmp192,((_tmpB00[0]=& _tmpB01,Cyc_aprintf(((_tmpAFF="_LL%X",_tag_dyneither(_tmpAFF,sizeof(char),6))),_tag_dyneither(_tmpB00,sizeof(void*),1)))))))),_tmpAFE)));
if(((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp191,res)!= _tmp192){
const char*_tmpB04;void*_tmpB03;(_tmpB03=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB04="fresh_label: add_ind returned bad index...",_tag_dyneither(_tmpB04,sizeof(char),43))),_tag_dyneither(_tmpB03,sizeof(void*),0)));}
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
struct _tuple9*_tmpB05;return(_tmpB05=_cycalloc(sizeof(*_tmpB05)),((_tmpB05->f1=_tmp19C,((_tmpB05->f2=_tmp19D,((_tmpB05->f3=Cyc_Toc_typ_to_c(_tmp19E),_tmpB05)))))));};}
# 763
static struct _tuple11*Cyc_Toc_typ_to_c_f(struct _tuple11*x){
struct _tuple11 _tmp1A0=*x;struct Cyc_Absyn_Tqual _tmp1A2;void*_tmp1A3;struct _tuple11 _tmp1A1=_tmp1A0;_tmp1A2=_tmp1A1.f1;_tmp1A3=_tmp1A1.f2;{
struct _tuple11*_tmpB06;return(_tmpB06=_cycalloc(sizeof(*_tmpB06)),((_tmpB06->f1=_tmp1A2,((_tmpB06->f2=Cyc_Toc_typ_to_c(_tmp1A3),_tmpB06)))));};}
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
struct Cyc_Absyn_Aggrfield*_tmpB07;return(_tmpB07=_cycalloc(sizeof(*_tmpB07)),((_tmpB07->name=f->name,((_tmpB07->tq=Cyc_Toc_mt_tq,((_tmpB07->type=
# 795
Cyc_Toc_typ_to_c(f->type),((_tmpB07->width=f->width,((_tmpB07->attributes=f->attributes,((_tmpB07->requires_clause=0,_tmpB07)))))))))))));}
# 800
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 802
return;}
# 805
static void*Cyc_Toc_char_star_typ(){
static void**cs=0;
if(cs == 0){
void**_tmpB08;cs=((_tmpB08=_cycalloc(sizeof(*_tmpB08)),((_tmpB08[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpB08))));}
# 810
return*cs;}
# 812
static void*Cyc_Toc_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpB09;r=((_tmpB09=_cycalloc(sizeof(*_tmpB09)),((_tmpB09[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),_tmpB09))));}
# 817
return*r;}
# 819
static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpB0A;r=((_tmpB0A=_cycalloc(sizeof(*_tmpB0A)),((_tmpB0A[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),_tmpB0A))));}
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
void*_tmp1B9=t;void**_tmp1BC;struct Cyc_Absyn_Tvar*_tmp1BE;struct Cyc_Absyn_Datatypedecl*_tmp1C1;struct Cyc_Absyn_Datatypefield*_tmp1C2;void*_tmp1C5;struct Cyc_Absyn_Tqual _tmp1C6;union Cyc_Absyn_Constraint*_tmp1C7;void*_tmp1CB;struct Cyc_Absyn_Tqual _tmp1CC;struct Cyc_Absyn_Exp*_tmp1CD;unsigned int _tmp1CE;struct Cyc_Absyn_Tqual _tmp1D0;void*_tmp1D1;struct Cyc_List_List*_tmp1D2;int _tmp1D3;struct Cyc_Absyn_VarargInfo*_tmp1D4;struct Cyc_List_List*_tmp1D5;struct Cyc_List_List*_tmp1D7;enum Cyc_Absyn_AggrKind _tmp1D9;struct Cyc_List_List*_tmp1DA;union Cyc_Absyn_AggrInfoU _tmp1DC;struct Cyc_List_List*_tmp1DD;struct _tuple1*_tmp1DF;struct Cyc_List_List*_tmp1E1;struct _tuple1*_tmp1E3;struct Cyc_List_List*_tmp1E4;struct Cyc_Absyn_Typedefdecl*_tmp1E5;void*_tmp1E6;void*_tmp1E9;struct Cyc_Absyn_Exp*_tmp1F2;struct Cyc_Absyn_Aggrdecl*_tmp1F5;struct Cyc_Absyn_Enumdecl*_tmp1F8;struct Cyc_Absyn_Datatypedecl*_tmp1FB;void**_tmp1FC;_LL63: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp1BA=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp1B9;if(_tmp1BA->tag != 0)goto _LL65;}_LL64:
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
const char*_tmpB0D;void*_tmpB0C;(_tmpB0C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB0D="unresolved DatatypeFieldType",_tag_dyneither(_tmpB0D,sizeof(char),29))),_tag_dyneither(_tmpB0C,sizeof(void*),0)));}_LL6F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1C4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B9;if(_tmp1C4->tag != 5)goto _LL71;else{_tmp1C5=(_tmp1C4->f1).elt_typ;_tmp1C6=(_tmp1C4->f1).elt_tq;_tmp1C7=((_tmp1C4->f1).ptr_atts).bounds;}}_LL70:
# 864
 _tmp1C5=Cyc_Toc_typ_to_c(_tmp1C5);{
void*_tmp1FF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp1C7);void*_tmp200=_tmp1FF;_LLA0: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp201=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp200;if(_tmp201->tag != 0)goto _LLA2;}_LLA1:
 return Cyc_Toc_dyneither_ptr_typ;_LLA2:;_LLA3:
 return Cyc_Absyn_star_typ(_tmp1C5,(void*)& Cyc_Absyn_HeapRgn_val,_tmp1C6,Cyc_Absyn_false_conref);_LL9F:;};_LL71: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp1C8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1B9;if(_tmp1C8->tag != 6)goto _LL73;}_LL72:
# 869
 goto _LL74;_LL73: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp1C9=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp1B9;if(_tmp1C9->tag != 7)goto _LL75;}_LL74:
 return t;_LL75: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1CA=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1B9;if(_tmp1CA->tag != 8)goto _LL77;else{_tmp1CB=(_tmp1CA->f1).elt_type;_tmp1CC=(_tmp1CA->f1).tq;_tmp1CD=(_tmp1CA->f1).num_elts;_tmp1CE=(_tmp1CA->f1).zt_loc;}}_LL76:
# 872
 return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c(_tmp1CB),_tmp1CC,_tmp1CD,Cyc_Absyn_false_conref,_tmp1CE);_LL77: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1CF=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B9;if(_tmp1CF->tag != 9)goto _LL79;else{_tmp1D0=(_tmp1CF->f1).ret_tqual;_tmp1D1=(_tmp1CF->f1).ret_typ;_tmp1D2=(_tmp1CF->f1).args;_tmp1D3=(_tmp1CF->f1).c_varargs;_tmp1D4=(_tmp1CF->f1).cyc_varargs;_tmp1D5=(_tmp1CF->f1).attributes;}}_LL78: {
# 878
struct Cyc_List_List*_tmp202=0;
for(0;_tmp1D5 != 0;_tmp1D5=_tmp1D5->tl){
void*_tmp203=(void*)_tmp1D5->hd;void*_tmp204=_tmp203;_LLA5: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp205=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp204;if(_tmp205->tag != 4)goto _LLA7;}_LLA6:
 goto _LLA8;_LLA7: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp206=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp204;if(_tmp206->tag != 5)goto _LLA9;}_LLA8:
 goto _LLAA;_LLA9: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp207=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp204;if(_tmp207->tag != 19)goto _LLAB;}_LLAA:
 continue;_LLAB: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp208=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp204;if(_tmp208->tag != 22)goto _LLAD;}_LLAC:
 continue;_LLAD: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp209=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp204;if(_tmp209->tag != 21)goto _LLAF;}_LLAE:
 continue;_LLAF: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp20A=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp204;if(_tmp20A->tag != 20)goto _LLB1;}_LLB0:
 continue;_LLB1:;_LLB2:
{struct Cyc_List_List*_tmpB0E;_tmp202=((_tmpB0E=_cycalloc(sizeof(*_tmpB0E)),((_tmpB0E->hd=(void*)_tmp1D5->hd,((_tmpB0E->tl=_tmp202,_tmpB0E))))));}goto _LLA4;_LLA4:;}{
# 889
struct Cyc_List_List*_tmp20C=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp1D2);
if(_tmp1D4 != 0){
# 892
void*_tmp20D=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp1D4->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple9*_tmpB0F;struct _tuple9*_tmp20E=(_tmpB0F=_cycalloc(sizeof(*_tmpB0F)),((_tmpB0F->f1=_tmp1D4->name,((_tmpB0F->f2=_tmp1D4->tq,((_tmpB0F->f3=_tmp20D,_tmpB0F)))))));
struct Cyc_List_List*_tmpB10;_tmp20C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp20C,((_tmpB10=_cycalloc(sizeof(*_tmpB10)),((_tmpB10->hd=_tmp20E,((_tmpB10->tl=0,_tmpB10)))))));}{
# 896
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpB16;struct Cyc_Absyn_FnInfo _tmpB15;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpB14;return(void*)((_tmpB14=_cycalloc(sizeof(*_tmpB14)),((_tmpB14[0]=((_tmpB16.tag=9,((_tmpB16.f1=((_tmpB15.tvars=0,((_tmpB15.effect=0,((_tmpB15.ret_tqual=_tmp1D0,((_tmpB15.ret_typ=Cyc_Toc_typ_to_c(_tmp1D1),((_tmpB15.args=_tmp20C,((_tmpB15.c_varargs=_tmp1D3,((_tmpB15.cyc_varargs=0,((_tmpB15.rgn_po=0,((_tmpB15.attributes=_tmp202,((_tmpB15.requires_clause=0,((_tmpB15.requires_relns=0,((_tmpB15.ensures_clause=0,((_tmpB15.ensures_relns=0,_tmpB15)))))))))))))))))))))))))),_tmpB16)))),_tmpB14))));};};}_LL79: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp1D6=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1B9;if(_tmp1D6->tag != 10)goto _LL7B;else{_tmp1D7=_tmp1D6->f1;}}_LL7A:
# 901
 _tmp1D7=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp1D7);
return Cyc_Toc_add_tuple_type(_tmp1D7);_LL7B: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1D8=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1B9;if(_tmp1D8->tag != 12)goto _LL7D;else{_tmp1D9=_tmp1D8->f1;_tmp1DA=_tmp1D8->f2;}}_LL7C: {
# 906
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpB19;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpB18;return(void*)((_tmpB18=_cycalloc(sizeof(*_tmpB18)),((_tmpB18[0]=((_tmpB19.tag=12,((_tmpB19.f1=_tmp1D9,((_tmpB19.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp1DA),_tmpB19)))))),_tmpB18))));}_LL7D: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1DB=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B9;if(_tmp1DB->tag != 11)goto _LL7F;else{_tmp1DC=(_tmp1DB->f1).aggr_info;_tmp1DD=(_tmp1DB->f1).targs;}}_LL7E:
# 910
{union Cyc_Absyn_AggrInfoU _tmp216=_tmp1DC;_LLB4: if((_tmp216.UnknownAggr).tag != 1)goto _LLB6;_LLB5:
 return t;_LLB6:;_LLB7:
 goto _LLB3;_LLB3:;}{
# 914
struct Cyc_Absyn_Aggrdecl*_tmp217=Cyc_Absyn_get_known_aggrdecl(_tmp1DC);
# 918
if(_tmp217->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp217->name,Cyc_Absyn_unionq_typ);{
struct Cyc_List_List*_tmp218=_tmp217->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp217->impl))->fields;
if(_tmp218 == 0)return Cyc_Toc_aggrdecl_type(_tmp217->name,Cyc_Absyn_strctq);
for(0;_tmp218->tl != 0;_tmp218=_tmp218->tl){;}{
void*_tmp219=((struct Cyc_Absyn_Aggrfield*)_tmp218->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp219))){
# 928
struct _RegionHandle _tmp21A=_new_region("r");struct _RegionHandle*r=& _tmp21A;_push_region(r);
{struct Cyc_List_List*_tmp21B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp217->tvs,_tmp1DD);
void*_tmp21C=Cyc_Tcutil_rsubstitute(r,_tmp21B,_tmp219);
if(Cyc_Toc_is_abstract_type(_tmp21C)){void*_tmp21D=Cyc_Toc_aggrdecl_type(_tmp217->name,Cyc_Absyn_strctq);_npop_handler(0);return _tmp21D;}{
void*_tmp21E=Cyc_Toc_add_struct_type(_tmp217->name,_tmp217->tvs,_tmp1DD,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp217->impl))->fields);_npop_handler(0);return _tmp21E;};}
# 929
;_pop_region(r);}
# 934
return Cyc_Toc_aggrdecl_type(_tmp217->name,Cyc_Absyn_strctq);};};};_LL7F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp1DE=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp1B9;if(_tmp1DE->tag != 13)goto _LL81;else{_tmp1DF=_tmp1DE->f1;}}_LL80:
 return t;_LL81: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp1E0=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1B9;if(_tmp1E0->tag != 14)goto _LL83;else{_tmp1E1=_tmp1E0->f1;}}_LL82:
 Cyc_Toc_enumfields_to_c(_tmp1E1);return t;_LL83: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp1E2=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1B9;if(_tmp1E2->tag != 17)goto _LL85;else{_tmp1E3=_tmp1E2->f1;_tmp1E4=_tmp1E2->f2;_tmp1E5=_tmp1E2->f3;_tmp1E6=(void*)_tmp1E2->f4;}}_LL84:
# 938
 if(_tmp1E6 == 0  || Cyc_noexpand_r){
if(_tmp1E4 != 0){
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpB1C;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpB1B;return(void*)((_tmpB1B=_cycalloc(sizeof(*_tmpB1B)),((_tmpB1B[0]=((_tmpB1C.tag=17,((_tmpB1C.f1=_tmp1E3,((_tmpB1C.f2=0,((_tmpB1C.f3=_tmp1E5,((_tmpB1C.f4=0,_tmpB1C)))))))))),_tmpB1B))));}else{
return t;}}else{
# 943
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpB1F;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpB1E;return(void*)((_tmpB1E=_cycalloc(sizeof(*_tmpB1E)),((_tmpB1E[0]=((_tmpB1F.tag=17,((_tmpB1F.f1=_tmp1E3,((_tmpB1F.f2=0,((_tmpB1F.f3=_tmp1E5,((_tmpB1F.f4=Cyc_Toc_typ_to_c(_tmp1E6),_tmpB1F)))))))))),_tmpB1E))));}_LL85: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp1E7=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp1B9;if(_tmp1E7->tag != 19)goto _LL87;}_LL86:
 return Cyc_Absyn_uint_typ;_LL87: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp1E8=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1B9;if(_tmp1E8->tag != 15)goto _LL89;else{_tmp1E9=(void*)_tmp1E8->f1;}}_LL88:
 return Cyc_Toc_rgn_typ();_LL89: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp1EA=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp1B9;if(_tmp1EA->tag != 16)goto _LL8B;}_LL8A:
 return Cyc_Toc_dyn_rgn_typ();_LL8B: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp1EB=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp1B9;if(_tmp1EB->tag != 20)goto _LL8D;}_LL8C:
# 949
 goto _LL8E;_LL8D: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp1EC=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp1B9;if(_tmp1EC->tag != 21)goto _LL8F;}_LL8E:
 goto _LL90;_LL8F: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp1ED=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp1B9;if(_tmp1ED->tag != 22)goto _LL91;}_LL90:
 goto _LL92;_LL91: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1EE=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp1B9;if(_tmp1EE->tag != 23)goto _LL93;}_LL92:
 goto _LL94;_LL93: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1EF=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp1B9;if(_tmp1EF->tag != 24)goto _LL95;}_LL94:
 goto _LL96;_LL95: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1F0=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp1B9;if(_tmp1F0->tag != 25)goto _LL97;}_LL96:
 return Cyc_Absyn_void_star_typ();_LL97: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1F1=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp1B9;if(_tmp1F1->tag != 18)goto _LL99;else{_tmp1F2=_tmp1F1->f1;}}_LL98:
# 959
 return t;_LL99: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1F3=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1B9;if(_tmp1F3->tag != 26)goto _LL9B;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp1F4=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp1F3->f1)->r;if(_tmp1F4->tag != 0)goto _LL9B;else{_tmp1F5=_tmp1F4->f1;}}}_LL9A:
# 961
 Cyc_Toc_aggrdecl_to_c(_tmp1F5,1);
if(_tmp1F5->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp1F5->name,Cyc_Absyn_unionq_typ);else{
return Cyc_Toc_aggrdecl_type(_tmp1F5->name,Cyc_Absyn_strctq);}_LL9B: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1F6=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1B9;if(_tmp1F6->tag != 26)goto _LL9D;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp1F7=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp1F6->f1)->r;if(_tmp1F7->tag != 1)goto _LL9D;else{_tmp1F8=_tmp1F7->f1;}}}_LL9C:
# 966
 Cyc_Toc_enumdecl_to_c(_tmp1F8);
return t;_LL9D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1F9=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1B9;if(_tmp1F9->tag != 26)goto _LL62;else{{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp1FA=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp1F9->f1)->r;if(_tmp1FA->tag != 2)goto _LL62;else{_tmp1FB=_tmp1FA->f1;}}_tmp1FC=_tmp1F9->f2;}}_LL9E:
# 969
 Cyc_Toc_datatypedecl_to_c(_tmp1FB);
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp1FC)));_LL62:;}
# 974
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned int l){
void*_tmp223=t;void*_tmp225;struct Cyc_Absyn_Tqual _tmp226;_LLB9: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp224=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp223;if(_tmp224->tag != 8)goto _LLBB;else{_tmp225=(_tmp224->f1).elt_type;_tmp226=(_tmp224->f1).tq;}}_LLBA:
# 977
 return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp225,(void*)& Cyc_Absyn_HeapRgn_val,_tmp226,Cyc_Absyn_false_conref),e);_LLBB:;_LLBC:
 return Cyc_Toc_cast_it(t,e);_LLB8:;}
# 984
static int Cyc_Toc_atomic_typ(void*t){
void*_tmp227=Cyc_Tcutil_compress(t);void*_tmp228=_tmp227;void*_tmp232;union Cyc_Absyn_AggrInfoU _tmp234;struct Cyc_List_List*_tmp236;struct Cyc_Absyn_Datatypedecl*_tmp238;struct Cyc_Absyn_Datatypefield*_tmp239;struct Cyc_List_List*_tmp23B;_LLBE: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp229=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp228;if(_tmp229->tag != 0)goto _LLC0;}_LLBF:
 return 1;_LLC0: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp22A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp228;if(_tmp22A->tag != 2)goto _LLC2;}_LLC1:
 return 0;_LLC2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp22B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp228;if(_tmp22B->tag != 6)goto _LLC4;}_LLC3:
 goto _LLC5;_LLC4: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp22C=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp228;if(_tmp22C->tag != 13)goto _LLC6;}_LLC5:
 goto _LLC7;_LLC6: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp22D=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp228;if(_tmp22D->tag != 14)goto _LLC8;}_LLC7:
 goto _LLC9;_LLC8: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp22E=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp228;if(_tmp22E->tag != 7)goto _LLCA;}_LLC9:
 goto _LLCB;_LLCA: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp22F=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp228;if(_tmp22F->tag != 9)goto _LLCC;}_LLCB:
 goto _LLCD;_LLCC: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp230=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp228;if(_tmp230->tag != 19)goto _LLCE;}_LLCD:
 return 1;_LLCE: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp231=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp228;if(_tmp231->tag != 8)goto _LLD0;else{_tmp232=(_tmp231->f1).elt_type;}}_LLCF:
 return Cyc_Toc_atomic_typ(_tmp232);_LLD0: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp233=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp228;if(_tmp233->tag != 11)goto _LLD2;else{_tmp234=(_tmp233->f1).aggr_info;}}_LLD1:
# 1001
{union Cyc_Absyn_AggrInfoU _tmp240=_tmp234;_LLE3: if((_tmp240.UnknownAggr).tag != 1)goto _LLE5;_LLE4:
 return 0;_LLE5:;_LLE6:
 goto _LLE2;_LLE2:;}{
# 1005
struct Cyc_Absyn_Aggrdecl*_tmp241=Cyc_Absyn_get_known_aggrdecl(_tmp234);
if(_tmp241->impl == 0)
return 0;
{struct Cyc_List_List*_tmp242=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp241->impl))->fields;for(0;_tmp242 != 0;_tmp242=_tmp242->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp242->hd)->type))return 0;}}
return 1;};_LLD2: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp235=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp228;if(_tmp235->tag != 12)goto _LLD4;else{_tmp236=_tmp235->f2;}}_LLD3:
# 1012
 for(0;_tmp236 != 0;_tmp236=_tmp236->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp236->hd)->type))return 0;}
return 1;_LLD4: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp237=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp228;if(_tmp237->tag != 4)goto _LLD6;else{if((((_tmp237->f1).field_info).KnownDatatypefield).tag != 2)goto _LLD6;_tmp238=((struct _tuple2)(((_tmp237->f1).field_info).KnownDatatypefield).val).f1;_tmp239=((struct _tuple2)(((_tmp237->f1).field_info).KnownDatatypefield).val).f2;}}_LLD5:
# 1016
 _tmp23B=_tmp239->typs;goto _LLD7;_LLD6: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp23A=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp228;if(_tmp23A->tag != 10)goto _LLD8;else{_tmp23B=_tmp23A->f1;}}_LLD7:
# 1018
 for(0;_tmp23B != 0;_tmp23B=_tmp23B->tl){
if(!Cyc_Toc_atomic_typ((*((struct _tuple11*)_tmp23B->hd)).f2))return 0;}
return 1;_LLD8: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp23C=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp228;if(_tmp23C->tag != 3)goto _LLDA;}_LLD9:
# 1023
 goto _LLDB;_LLDA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp23D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp228;if(_tmp23D->tag != 5)goto _LLDC;}_LLDB:
 goto _LLDD;_LLDC: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp23E=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp228;if(_tmp23E->tag != 16)goto _LLDE;}_LLDD:
 goto _LLDF;_LLDE: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp23F=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp228;if(_tmp23F->tag != 15)goto _LLE0;}_LLDF:
 return 0;_LLE0:;_LLE1: {
const char*_tmpB23;void*_tmpB22[1];struct Cyc_String_pa_PrintArg_struct _tmpB21;(_tmpB21.tag=0,((_tmpB21.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpB22[0]=& _tmpB21,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB23="atomic_typ:  bad type %s",_tag_dyneither(_tmpB23,sizeof(char),25))),_tag_dyneither(_tmpB22,sizeof(void*),1)))))));}_LLBD:;}
# 1031
static int Cyc_Toc_is_void_star(void*t){
void*_tmp246=Cyc_Tcutil_compress(t);void*_tmp247=_tmp246;void*_tmp249;_LLE8: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp248=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp247;if(_tmp248->tag != 5)goto _LLEA;else{_tmp249=(_tmp248->f1).elt_typ;}}_LLE9: {
# 1034
void*_tmp24A=Cyc_Tcutil_compress(_tmp249);void*_tmp24B=_tmp24A;_LLED: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp24C=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp24B;if(_tmp24C->tag != 0)goto _LLEF;}_LLEE:
 return 1;_LLEF:;_LLF0:
 return 0;_LLEC:;}_LLEA:;_LLEB:
# 1038
 return 0;_LLE7:;}
# 1042
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1046
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1051
static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){
void*_tmp24D=Cyc_Tcutil_compress(t);void*_tmp24E=_tmp24D;union Cyc_Absyn_AggrInfoU _tmp250;struct Cyc_List_List*_tmp252;_LLF2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp24F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp24E;if(_tmp24F->tag != 11)goto _LLF4;else{_tmp250=(_tmp24F->f1).aggr_info;}}_LLF3: {
# 1054
struct Cyc_Absyn_Aggrdecl*_tmp253=Cyc_Absyn_get_known_aggrdecl(_tmp250);
if(_tmp253->impl == 0){
const char*_tmpB26;void*_tmpB25;(_tmpB25=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB26="is_poly_field: type missing fields",_tag_dyneither(_tmpB26,sizeof(char),35))),_tag_dyneither(_tmpB25,sizeof(void*),0)));}
_tmp252=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp253->impl))->fields;goto _LLF5;}_LLF4: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp251=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp24E;if(_tmp251->tag != 12)goto _LLF6;else{_tmp252=_tmp251->f2;}}_LLF5: {
# 1059
struct Cyc_Absyn_Aggrfield*_tmp256=Cyc_Absyn_lookup_field(_tmp252,f);
if(_tmp256 == 0){
const char*_tmpB2A;void*_tmpB29[1];struct Cyc_String_pa_PrintArg_struct _tmpB28;(_tmpB28.tag=0,((_tmpB28.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpB29[0]=& _tmpB28,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB2A="is_poly_field: bad field %s",_tag_dyneither(_tmpB2A,sizeof(char),28))),_tag_dyneither(_tmpB29,sizeof(void*),1)))))));}
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp256->type);}_LLF6:;_LLF7: {
const char*_tmpB2E;void*_tmpB2D[1];struct Cyc_String_pa_PrintArg_struct _tmpB2C;(_tmpB2C.tag=0,((_tmpB2C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpB2D[0]=& _tmpB2C,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB2E="is_poly_field: bad type %s",_tag_dyneither(_tmpB2E,sizeof(char),27))),_tag_dyneither(_tmpB2D,sizeof(void*),1)))))));}_LLF1:;}
# 1070
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp25D=e->r;void*_tmp25E=_tmp25D;struct Cyc_Absyn_Exp*_tmp260;struct _dyneither_ptr*_tmp261;struct Cyc_Absyn_Exp*_tmp263;struct _dyneither_ptr*_tmp264;_LLF9: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp25F=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp25E;if(_tmp25F->tag != 20)goto _LLFB;else{_tmp260=_tmp25F->f1;_tmp261=_tmp25F->f2;}}_LLFA:
# 1073
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp260->topt),_tmp261) && !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));_LLFB: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp262=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp25E;if(_tmp262->tag != 21)goto _LLFD;else{_tmp263=_tmp262->f1;_tmp264=_tmp262->f2;}}_LLFC: {
# 1076
void*_tmp265=Cyc_Tcutil_compress((void*)_check_null(_tmp263->topt));void*_tmp266=_tmp265;void*_tmp268;_LL100: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp267=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp266;if(_tmp267->tag != 5)goto _LL102;else{_tmp268=(_tmp267->f1).elt_typ;}}_LL101:
# 1078
 return Cyc_Toc_is_poly_field(_tmp268,_tmp264) && !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));_LL102:;_LL103: {
const char*_tmpB32;void*_tmpB31[1];struct Cyc_String_pa_PrintArg_struct _tmpB30;(_tmpB30.tag=0,((_tmpB30.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp263->topt))),((_tmpB31[0]=& _tmpB30,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB32="is_poly_project: bad type %s",_tag_dyneither(_tmpB32,sizeof(char),29))),_tag_dyneither(_tmpB31,sizeof(void*),1)))))));}_LLFF:;}_LLFD:;_LLFE:
# 1081
 return 0;_LLF8:;}
# 1086
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpB33;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((_tmpB33=_cycalloc(sizeof(*_tmpB33)),((_tmpB33->hd=s,((_tmpB33->tl=0,_tmpB33)))))),0);}
# 1090
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpB34;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((_tmpB34=_cycalloc(sizeof(*_tmpB34)),((_tmpB34->hd=s,((_tmpB34->tl=0,_tmpB34)))))),0);}
# 1094
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1100
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*_tmpB35[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmpB35[1]=s,((_tmpB35[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB35,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
# 1104
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type)){
struct Cyc_Absyn_Exp*_tmpB36[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpB36[1]=n,((_tmpB36[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB36,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{
# 1108
struct Cyc_Absyn_Exp*_tmpB37[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpB37[1]=n,((_tmpB37[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB37,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}
# 1111
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*_tmpB38[3];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,((_tmpB38[2]=n,((_tmpB38[1]=s,((_tmpB38[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB38,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
# 1116
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
struct Cyc_List_List*_tmpB39;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((_tmpB39=_cycalloc(sizeof(*_tmpB39)),((_tmpB39->hd=e,((_tmpB39->tl=0,_tmpB39)))))),0);}
# 1119
static struct Cyc_Absyn_Exp*Cyc_Toc_newrethrow_exp(struct Cyc_Absyn_Exp*e){
struct Cyc_List_List*_tmpB3A;return Cyc_Absyn_fncall_exp(Cyc_Toc__rethrow_e,((_tmpB3A=_cycalloc(sizeof(*_tmpB3A)),((_tmpB3A->hd=e,((_tmpB3A->tl=0,_tmpB3A)))))),0);}
# 1123
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
struct Cyc_Absyn_Exp*_tmpB3B[0];return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB3B,sizeof(struct Cyc_Absyn_Exp*),0)),0),0);}
# 1129
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1137
int is_string=0;
{void*_tmp275=e->r;void*_tmp276=_tmp275;_LL105: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp277=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp276;if(_tmp277->tag != 0)goto _LL107;else{if(((_tmp277->f1).String_c).tag != 8)goto _LL107;}}_LL106:
 is_string=1;goto _LL104;_LL107: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp278=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp276;if(_tmp278->tag != 0)goto _LL109;else{if(((_tmp278->f1).Wstring_c).tag != 9)goto _LL109;}}_LL108:
 is_string=1;goto _LL104;_LL109:;_LL10A:
 goto _LL104;_LL104:;}{
# 1143
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
if(is_string){
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_conref,0);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB41;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB40;struct Cyc_List_List*_tmpB3F;Cyc_Toc_result_decls=((_tmpB3F=_cycalloc(sizeof(*_tmpB3F)),((_tmpB3F->hd=Cyc_Absyn_new_decl((void*)((_tmpB41=_cycalloc(sizeof(*_tmpB41)),((_tmpB41[0]=((_tmpB40.tag=0,((_tmpB40.f1=vd,_tmpB40)))),_tmpB41)))),0),((_tmpB3F->tl=Cyc_Toc_result_decls,_tmpB3F))))));}
xexp=Cyc_Absyn_var_exp(x,0);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{
# 1153
xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),e);
# 1155
xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
# 1157
struct Cyc_Absyn_Exp*urm_exp;
{struct _tuple19*_tmpB48;struct _tuple19*_tmpB47;struct _tuple19*_tmpB46;struct _tuple19*_tmpB45[3];urm_exp=Cyc_Absyn_unresolvedmem_exp(0,((_tmpB45[2]=(
# 1160
(_tmpB46=_cycalloc(sizeof(*_tmpB46)),((_tmpB46->f1=0,((_tmpB46->f2=xplussz,_tmpB46)))))),((_tmpB45[1]=(
# 1159
(_tmpB47=_cycalloc(sizeof(*_tmpB47)),((_tmpB47->f1=0,((_tmpB47->f2=xexp,_tmpB47)))))),((_tmpB45[0]=(
# 1158
(_tmpB48=_cycalloc(sizeof(*_tmpB48)),((_tmpB48->f1=0,((_tmpB48->f2=xexp,_tmpB48)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB45,sizeof(struct _tuple19*),3)))))))),0);}
# 1161
return urm_exp;};};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_StructInfo{struct Cyc_Absyn_Exp**varsizeexp;struct Cyc_Absyn_Exp*lhs_exp;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;int*in_lhs;struct Cyc_Toc_StructInfo struct_info;int*in_sizeof;struct _RegionHandle*rgn;};
# 1204 "toc.cyc"
typedef struct Cyc_Toc_Env*Cyc_Toc_env_t;
# 1206
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
int _tmp281;struct Cyc_Toc_Env*_tmp280=nv;_tmp281=_tmp280->toplevel;
return _tmp281;}
# 1210
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
int*_tmp283;struct Cyc_Toc_Env*_tmp282=nv;_tmp283=_tmp282->in_lhs;
return*_tmp283;}
# 1215
static struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple1*x){
struct Cyc_Dict_Dict _tmp285;struct Cyc_Toc_Env*_tmp284=nv;_tmp285=_tmp284->varmap;
return((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp285,x);}
# 1221
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
int*_tmpB50;struct Cyc_Absyn_Exp**_tmpB4F;int*_tmpB4E;struct Cyc_Toc_Env*_tmpB4D;return(_tmpB4D=_region_malloc(r,sizeof(*_tmpB4D)),((_tmpB4D->break_lab=(struct _dyneither_ptr**)0,((_tmpB4D->continue_lab=(struct _dyneither_ptr**)0,((_tmpB4D->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpB4D->varmap=(struct Cyc_Dict_Dict)
# 1225
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((_tmpB4D->toplevel=(int)1,((_tmpB4D->in_lhs=(int*)(
# 1227
(_tmpB4E=_region_malloc(r,sizeof(*_tmpB4E)),((_tmpB4E[0]=0,_tmpB4E)))),((_tmpB4D->struct_info=(struct Cyc_Toc_StructInfo)(
((_tmpB4D->struct_info).varsizeexp=((_tmpB4F=_cycalloc(sizeof(*_tmpB4F)),((_tmpB4F[0]=0,_tmpB4F)))),(((_tmpB4D->struct_info).lhs_exp=0,_tmpB4D->struct_info)))),((_tmpB4D->in_sizeof=(int*)(
(_tmpB50=_region_malloc(r,sizeof(*_tmpB50)),((_tmpB50[0]=0,_tmpB50)))),((_tmpB4D->rgn=(struct _RegionHandle*)r,_tmpB4D)))))))))))))))))));}
# 1233
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp28B;struct _dyneither_ptr**_tmp28C;struct Cyc_Toc_FallthruInfo*_tmp28D;struct Cyc_Dict_Dict _tmp28E;int _tmp28F;int*_tmp290;struct Cyc_Toc_StructInfo _tmp291;int*_tmp292;struct Cyc_Toc_Env*_tmp28A=e;_tmp28B=_tmp28A->break_lab;_tmp28C=_tmp28A->continue_lab;_tmp28D=_tmp28A->fallthru_info;_tmp28E=_tmp28A->varmap;_tmp28F=_tmp28A->toplevel;_tmp290=_tmp28A->in_lhs;_tmp291=_tmp28A->struct_info;_tmp292=_tmp28A->in_sizeof;{
struct Cyc_Toc_Env*_tmpB51;return(_tmpB51=_region_malloc(r,sizeof(*_tmpB51)),((_tmpB51->break_lab=(struct _dyneither_ptr**)_tmp28B,((_tmpB51->continue_lab=(struct _dyneither_ptr**)_tmp28C,((_tmpB51->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp28D,((_tmpB51->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp28E),((_tmpB51->toplevel=(int)_tmp28F,((_tmpB51->in_lhs=(int*)_tmp290,((_tmpB51->struct_info=(struct Cyc_Toc_StructInfo)_tmp291,((_tmpB51->in_sizeof=(int*)_tmp292,((_tmpB51->rgn=(struct _RegionHandle*)r,_tmpB51)))))))))))))))))));};}
# 1238
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp295;struct _dyneither_ptr**_tmp296;struct Cyc_Toc_FallthruInfo*_tmp297;struct Cyc_Dict_Dict _tmp298;int _tmp299;int*_tmp29A;struct Cyc_Toc_StructInfo _tmp29B;int*_tmp29C;struct Cyc_Toc_Env*_tmp294=e;_tmp295=_tmp294->break_lab;_tmp296=_tmp294->continue_lab;_tmp297=_tmp294->fallthru_info;_tmp298=_tmp294->varmap;_tmp299=_tmp294->toplevel;_tmp29A=_tmp294->in_lhs;_tmp29B=_tmp294->struct_info;_tmp29C=_tmp294->in_sizeof;{
struct Cyc_Toc_Env*_tmpB52;return(_tmpB52=_region_malloc(r,sizeof(*_tmpB52)),((_tmpB52->break_lab=(struct _dyneither_ptr**)_tmp295,((_tmpB52->continue_lab=(struct _dyneither_ptr**)_tmp296,((_tmpB52->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp297,((_tmpB52->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp298),((_tmpB52->toplevel=(int)0,((_tmpB52->in_lhs=(int*)_tmp29A,((_tmpB52->struct_info=(struct Cyc_Toc_StructInfo)_tmp29B,((_tmpB52->in_sizeof=(int*)_tmp29C,((_tmpB52->rgn=(struct _RegionHandle*)r,_tmpB52)))))))))))))))))));};}
# 1242
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp29F;struct _dyneither_ptr**_tmp2A0;struct Cyc_Toc_FallthruInfo*_tmp2A1;struct Cyc_Dict_Dict _tmp2A2;int _tmp2A3;int*_tmp2A4;struct Cyc_Toc_StructInfo _tmp2A5;int*_tmp2A6;struct Cyc_Toc_Env*_tmp29E=e;_tmp29F=_tmp29E->break_lab;_tmp2A0=_tmp29E->continue_lab;_tmp2A1=_tmp29E->fallthru_info;_tmp2A2=_tmp29E->varmap;_tmp2A3=_tmp29E->toplevel;_tmp2A4=_tmp29E->in_lhs;_tmp2A5=_tmp29E->struct_info;_tmp2A6=_tmp29E->in_sizeof;{
struct Cyc_Toc_Env*_tmpB53;return(_tmpB53=_region_malloc(r,sizeof(*_tmpB53)),((_tmpB53->break_lab=(struct _dyneither_ptr**)_tmp29F,((_tmpB53->continue_lab=(struct _dyneither_ptr**)_tmp2A0,((_tmpB53->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2A1,((_tmpB53->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2A2),((_tmpB53->toplevel=(int)1,((_tmpB53->in_lhs=(int*)_tmp2A4,((_tmpB53->struct_info=(struct Cyc_Toc_StructInfo)_tmp2A5,((_tmpB53->in_sizeof=(int*)_tmp2A6,((_tmpB53->rgn=(struct _RegionHandle*)r,_tmpB53)))))))))))))))))));};}
# 1246
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
int*_tmp2A9;struct Cyc_Toc_Env*_tmp2A8=e;_tmp2A9=_tmp2A8->in_lhs;
*_tmp2A9=b;}
# 1250
static int Cyc_Toc_set_in_sizeof(struct Cyc_Toc_Env*e,int b){
int*_tmp2AB;struct Cyc_Toc_Env*_tmp2AA=e;_tmp2AB=_tmp2AA->in_sizeof;{
int _tmp2AC=*_tmp2AB;
*_tmp2AB=b;
return _tmp2AC;};}
# 1256
static int Cyc_Toc_in_sizeof(struct Cyc_Toc_Env*e){
int*_tmp2AE;struct Cyc_Toc_Env*_tmp2AD=e;_tmp2AE=_tmp2AD->in_sizeof;
return*_tmp2AE;}
# 1261
static struct Cyc_Toc_Env*Cyc_Toc_set_lhs_exp(struct _RegionHandle*r,struct Cyc_Absyn_Exp*exp,struct Cyc_Toc_Env*e){
# 1263
struct _dyneither_ptr**_tmp2B0;struct _dyneither_ptr**_tmp2B1;struct Cyc_Toc_FallthruInfo*_tmp2B2;struct Cyc_Dict_Dict _tmp2B3;int _tmp2B4;int*_tmp2B5;struct Cyc_Absyn_Exp**_tmp2B6;int*_tmp2B7;struct Cyc_Toc_Env*_tmp2AF=e;_tmp2B0=_tmp2AF->break_lab;_tmp2B1=_tmp2AF->continue_lab;_tmp2B2=_tmp2AF->fallthru_info;_tmp2B3=_tmp2AF->varmap;_tmp2B4=_tmp2AF->toplevel;_tmp2B5=_tmp2AF->in_lhs;_tmp2B6=(_tmp2AF->struct_info).varsizeexp;_tmp2B7=_tmp2AF->in_sizeof;{
struct Cyc_Toc_Env*_tmpB54;return(_tmpB54=_region_malloc(r,sizeof(*_tmpB54)),((_tmpB54->break_lab=(struct _dyneither_ptr**)_tmp2B0,((_tmpB54->continue_lab=(struct _dyneither_ptr**)_tmp2B1,((_tmpB54->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2B2,((_tmpB54->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2B3),((_tmpB54->toplevel=(int)_tmp2B4,((_tmpB54->in_lhs=(int*)_tmp2B5,((_tmpB54->struct_info=(struct Cyc_Toc_StructInfo)(((_tmpB54->struct_info).varsizeexp=_tmp2B6,(((_tmpB54->struct_info).lhs_exp=exp,_tmpB54->struct_info)))),((_tmpB54->in_sizeof=(int*)_tmp2B7,((_tmpB54->rgn=(struct _RegionHandle*)r,_tmpB54)))))))))))))))))));};}
# 1269
static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple1*x,struct Cyc_Absyn_Exp*y){
{union Cyc_Absyn_Nmspace _tmp2B9=(*x).f1;union Cyc_Absyn_Nmspace _tmp2BA=_tmp2B9;_LL10C: if((_tmp2BA.Rel_n).tag != 1)goto _LL10E;_LL10D: {
# 1272
const char*_tmpB58;void*_tmpB57[1];struct Cyc_String_pa_PrintArg_struct _tmpB56;(_tmpB56.tag=0,((_tmpB56.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(x)),((_tmpB57[0]=& _tmpB56,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB58="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(_tmpB58,sizeof(char),30))),_tag_dyneither(_tmpB57,sizeof(void*),1)))))));}_LL10E:;_LL10F:
 goto _LL10B;_LL10B:;}{
# 1275
struct _dyneither_ptr**_tmp2BF;struct _dyneither_ptr**_tmp2C0;struct Cyc_Toc_FallthruInfo*_tmp2C1;struct Cyc_Dict_Dict _tmp2C2;int _tmp2C3;int*_tmp2C4;struct Cyc_Toc_StructInfo _tmp2C5;int*_tmp2C6;struct Cyc_Toc_Env*_tmp2BE=e;_tmp2BF=_tmp2BE->break_lab;_tmp2C0=_tmp2BE->continue_lab;_tmp2C1=_tmp2BE->fallthru_info;_tmp2C2=_tmp2BE->varmap;_tmp2C3=_tmp2BE->toplevel;_tmp2C4=_tmp2BE->in_lhs;_tmp2C5=_tmp2BE->struct_info;_tmp2C6=_tmp2BE->in_sizeof;{
struct Cyc_Dict_Dict _tmp2C7=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2C2),x,y);
struct Cyc_Toc_Env*_tmpB59;return(_tmpB59=_region_malloc(r,sizeof(*_tmpB59)),((_tmpB59->break_lab=(struct _dyneither_ptr**)_tmp2BF,((_tmpB59->continue_lab=(struct _dyneither_ptr**)_tmp2C0,((_tmpB59->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2C1,((_tmpB59->varmap=(struct Cyc_Dict_Dict)_tmp2C7,((_tmpB59->toplevel=(int)_tmp2C3,((_tmpB59->in_lhs=(int*)_tmp2C4,((_tmpB59->struct_info=(struct Cyc_Toc_StructInfo)_tmp2C5,((_tmpB59->in_sizeof=(int*)_tmp2C6,((_tmpB59->rgn=(struct _RegionHandle*)r,_tmpB59)))))))))))))))))));};};}
# 1282
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp2CA;struct _dyneither_ptr**_tmp2CB;struct Cyc_Toc_FallthruInfo*_tmp2CC;struct Cyc_Dict_Dict _tmp2CD;int _tmp2CE;int*_tmp2CF;struct Cyc_Toc_StructInfo _tmp2D0;int*_tmp2D1;struct Cyc_Toc_Env*_tmp2C9=e;_tmp2CA=_tmp2C9->break_lab;_tmp2CB=_tmp2C9->continue_lab;_tmp2CC=_tmp2C9->fallthru_info;_tmp2CD=_tmp2C9->varmap;_tmp2CE=_tmp2C9->toplevel;_tmp2CF=_tmp2C9->in_lhs;_tmp2D0=_tmp2C9->struct_info;_tmp2D1=_tmp2C9->in_sizeof;{
struct Cyc_Toc_Env*_tmpB5A;return(_tmpB5A=_region_malloc(r,sizeof(*_tmpB5A)),((_tmpB5A->break_lab=(struct _dyneither_ptr**)0,((_tmpB5A->continue_lab=(struct _dyneither_ptr**)0,((_tmpB5A->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2CC,((_tmpB5A->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2CD),((_tmpB5A->toplevel=(int)_tmp2CE,((_tmpB5A->in_lhs=(int*)_tmp2CF,((_tmpB5A->struct_info=(struct Cyc_Toc_StructInfo)_tmp2D0,((_tmpB5A->in_sizeof=(int*)_tmp2D1,((_tmpB5A->rgn=(struct _RegionHandle*)r,_tmpB5A)))))))))))))))))));};}
# 1288
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*next_case_env){
# 1293
struct Cyc_List_List*fallthru_vars=0;
for(0;fallthru_binders != 0;fallthru_binders=fallthru_binders->tl){
struct Cyc_List_List*_tmpB5B;fallthru_vars=((_tmpB5B=_region_malloc(r,sizeof(*_tmpB5B)),((_tmpB5B->hd=((struct Cyc_Absyn_Vardecl*)fallthru_binders->hd)->name,((_tmpB5B->tl=fallthru_vars,_tmpB5B))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fallthru_vars);{
struct _dyneither_ptr**_tmp2D5;struct _dyneither_ptr**_tmp2D6;struct Cyc_Toc_FallthruInfo*_tmp2D7;struct Cyc_Dict_Dict _tmp2D8;int _tmp2D9;int*_tmp2DA;struct Cyc_Toc_StructInfo _tmp2DB;int*_tmp2DC;struct Cyc_Toc_Env*_tmp2D4=e;_tmp2D5=_tmp2D4->break_lab;_tmp2D6=_tmp2D4->continue_lab;_tmp2D7=_tmp2D4->fallthru_info;_tmp2D8=_tmp2D4->varmap;_tmp2D9=_tmp2D4->toplevel;_tmp2DA=_tmp2D4->in_lhs;_tmp2DB=_tmp2D4->struct_info;_tmp2DC=_tmp2D4->in_sizeof;{
struct Cyc_Dict_Dict _tmp2DE;struct Cyc_Toc_Env*_tmp2DD=next_case_env;_tmp2DE=_tmp2DD->varmap;{
struct Cyc_Toc_FallthruInfo*_tmpB5C;struct Cyc_Toc_FallthruInfo*fi=
(_tmpB5C=_region_malloc(r,sizeof(*_tmpB5C)),((_tmpB5C->label=fallthru_l,((_tmpB5C->binders=fallthru_vars,((_tmpB5C->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2DE),_tmpB5C)))))));
struct _dyneither_ptr**_tmpB5F;struct Cyc_Toc_Env*_tmpB5E;return(_tmpB5E=_region_malloc(r,sizeof(*_tmpB5E)),((_tmpB5E->break_lab=(struct _dyneither_ptr**)((_tmpB5F=_region_malloc(r,sizeof(*_tmpB5F)),((_tmpB5F[0]=break_l,_tmpB5F)))),((_tmpB5E->continue_lab=(struct _dyneither_ptr**)_tmp2D6,((_tmpB5E->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi,((_tmpB5E->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2D8),((_tmpB5E->toplevel=(int)_tmp2D9,((_tmpB5E->in_lhs=(int*)_tmp2DA,((_tmpB5E->struct_info=(struct Cyc_Toc_StructInfo)_tmp2DB,((_tmpB5E->in_sizeof=(int*)_tmp2DC,((_tmpB5E->rgn=(struct _RegionHandle*)r,_tmpB5E)))))))))))))))))));};};};}
# 1305
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1308
struct _dyneither_ptr**_tmp2E3;struct _dyneither_ptr**_tmp2E4;struct Cyc_Toc_FallthruInfo*_tmp2E5;struct Cyc_Dict_Dict _tmp2E6;int _tmp2E7;int*_tmp2E8;struct Cyc_Toc_StructInfo _tmp2E9;int*_tmp2EA;struct Cyc_Toc_Env*_tmp2E2=e;_tmp2E3=_tmp2E2->break_lab;_tmp2E4=_tmp2E2->continue_lab;_tmp2E5=_tmp2E2->fallthru_info;_tmp2E6=_tmp2E2->varmap;_tmp2E7=_tmp2E2->toplevel;_tmp2E8=_tmp2E2->in_lhs;_tmp2E9=_tmp2E2->struct_info;_tmp2EA=_tmp2E2->in_sizeof;{
struct _dyneither_ptr**_tmpB62;struct Cyc_Toc_Env*_tmpB61;return(_tmpB61=_region_malloc(r,sizeof(*_tmpB61)),((_tmpB61->break_lab=(struct _dyneither_ptr**)((_tmpB62=_region_malloc(r,sizeof(*_tmpB62)),((_tmpB62[0]=break_l,_tmpB62)))),((_tmpB61->continue_lab=(struct _dyneither_ptr**)_tmp2E4,((_tmpB61->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpB61->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2E6),((_tmpB61->toplevel=(int)_tmp2E7,((_tmpB61->in_lhs=(int*)_tmp2E8,((_tmpB61->struct_info=(struct Cyc_Toc_StructInfo)_tmp2E9,((_tmpB61->in_sizeof=(int*)_tmp2EA,((_tmpB61->rgn=(struct _RegionHandle*)r,_tmpB61)))))))))))))))))));};}
# 1315
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1318
struct _dyneither_ptr**_tmp2EE;struct _dyneither_ptr**_tmp2EF;struct Cyc_Toc_FallthruInfo*_tmp2F0;struct Cyc_Dict_Dict _tmp2F1;int _tmp2F2;int*_tmp2F3;struct Cyc_Toc_StructInfo _tmp2F4;int*_tmp2F5;struct Cyc_Toc_Env*_tmp2ED=e;_tmp2EE=_tmp2ED->break_lab;_tmp2EF=_tmp2ED->continue_lab;_tmp2F0=_tmp2ED->fallthru_info;_tmp2F1=_tmp2ED->varmap;_tmp2F2=_tmp2ED->toplevel;_tmp2F3=_tmp2ED->in_lhs;_tmp2F4=_tmp2ED->struct_info;_tmp2F5=_tmp2ED->in_sizeof;{
struct Cyc_Toc_FallthruInfo*_tmpB65;struct Cyc_Toc_Env*_tmpB64;return(_tmpB64=_region_malloc(r,sizeof(*_tmpB64)),((_tmpB64->break_lab=(struct _dyneither_ptr**)0,((_tmpB64->continue_lab=(struct _dyneither_ptr**)_tmp2EF,((_tmpB64->fallthru_info=(struct Cyc_Toc_FallthruInfo*)((_tmpB65=_region_malloc(r,sizeof(*_tmpB65)),((_tmpB65->label=next_l,((_tmpB65->binders=0,((_tmpB65->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),_tmpB65)))))))),((_tmpB64->varmap=(struct Cyc_Dict_Dict)
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2F1),((_tmpB64->toplevel=(int)_tmp2F2,((_tmpB64->in_lhs=(int*)_tmp2F3,((_tmpB64->struct_info=(struct Cyc_Toc_StructInfo)_tmp2F4,((_tmpB64->in_sizeof=(int*)_tmp2F5,((_tmpB64->rgn=(struct _RegionHandle*)r,_tmpB64)))))))))))))))))));};}
# 1333 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1336
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
# 1338
void*_tmp2F8=e->annot;void*_tmp2F9=_tmp2F8;_LL111: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2FA=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2F9;if(_tmp2FA->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL113;}_LL112:
# 1341
 return Cyc_Toc_is_nullable((void*)_check_null(e->topt));_LL113: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2FB=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2F9;if(_tmp2FB->tag != Cyc_CfFlowInfo_NotZero)goto _LL115;}_LL114:
# 1344
 return 0;_LL115: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp2FC=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2F9;if(_tmp2FC->tag != Cyc_CfFlowInfo_IsZero)goto _LL117;}_LL116:
# 1346
{const char*_tmpB68;void*_tmpB67;(_tmpB67=0,Cyc_Tcutil_terr(e->loc,((_tmpB68="dereference of NULL pointer",_tag_dyneither(_tmpB68,sizeof(char),28))),_tag_dyneither(_tmpB67,sizeof(void*),0)));}
return 0;_LL117: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp2FD=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2F9;if(_tmp2FD->tag != Cyc_Absyn_EmptyAnnot)goto _LL119;}_LL118:
# 1352
 return 1;_LL119:;_LL11A: {
const char*_tmpB6B;void*_tmpB6A;(_tmpB6A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB6B="need_null_check",_tag_dyneither(_tmpB6B,sizeof(char),16))),_tag_dyneither(_tmpB6A,sizeof(void*),0)));}_LL110:;}
# 1357
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp302=e->annot;void*_tmp303=_tmp302;struct Cyc_List_List*_tmp305;struct Cyc_List_List*_tmp307;_LL11C: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp304=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp303;if(_tmp304->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL11E;else{_tmp305=_tmp304->f1;}}_LL11D:
 return _tmp305;_LL11E: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp306=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp303;if(_tmp306->tag != Cyc_CfFlowInfo_NotZero)goto _LL120;else{_tmp307=_tmp306->f1;}}_LL11F:
 return _tmp307;_LL120: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp308=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp303;if(_tmp308->tag != Cyc_CfFlowInfo_IsZero)goto _LL122;}_LL121:
# 1362
{const char*_tmpB6E;void*_tmpB6D;(_tmpB6D=0,Cyc_Tcutil_terr(e->loc,((_tmpB6E="dereference of NULL pointer",_tag_dyneither(_tmpB6E,sizeof(char),28))),_tag_dyneither(_tmpB6D,sizeof(void*),0)));}
return 0;_LL122: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp309=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp303;if(_tmp309->tag != Cyc_Absyn_EmptyAnnot)goto _LL124;}_LL123:
 return 0;_LL124:;_LL125: {
const char*_tmpB71;void*_tmpB70;(_tmpB70=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB71="get_relns",_tag_dyneither(_tmpB71,sizeof(char),10))),_tag_dyneither(_tmpB70,sizeof(void*),0)));}_LL11B:;}static char _tmp313[8]="*bogus*";
# 1372
static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
# 1382 "toc.cyc"
union Cyc_Relations_RelnOp rop_i=Cyc_Relations_RConst(0);
int valid_rop_i=Cyc_Relations_exp2relnop(i,& rop_i);
# 1385
struct Cyc_Absyn_Vardecl*x;
{void*_tmp30E=a->r;void*_tmp30F=_tmp30E;void*_tmp311;_LL127: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp310=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp30F;if(_tmp310->tag != 1)goto _LL129;else{_tmp311=(void*)_tmp310->f2;}}_LL128: {
# 1388
struct Cyc_Absyn_Vardecl*_tmp312=Cyc_Tcutil_nonesc_vardecl(_tmp311);
if(_tmp312 == 0)goto _LL12A;
x=_tmp312;
goto _LL126;}_LL129:;_LL12A: {
# 1394
static struct _dyneither_ptr bogus_string={_tmp313,_tmp313,_tmp313 + 8};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
# 1397
static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};
# 1403
x=& bogus_vardecl;
x->type=a_typ;}_LL126:;}{
# 1406
void*_tmp314=a_typ;
union Cyc_Relations_RelnOp rop_a=Cyc_Relations_RNumelts(x);
# 1411
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound(_tmp314);
if(bound != 0){
union Cyc_Relations_RelnOp rop_bound=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(bound,& rop_bound))
relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_bound,Cyc_Relations_Rlte,rop_a,relns);}
# 1420
if(valid_rop_i){
struct Cyc_List_List*_tmp315=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlte,rop_i,relns);
# 1428
if(!Cyc_Relations_consistent_relations(_tmp315))return 1;}
# 1431
inner_loop: {
void*_tmp316=i->r;void*_tmp317=_tmp316;void*_tmp319;struct Cyc_Absyn_Exp*_tmp31A;struct Cyc_Absyn_Exp*_tmp31C;struct Cyc_Absyn_Exp*_tmp31D;_LL12C: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp318=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp317;if(_tmp318->tag != 13)goto _LL12E;else{_tmp319=(void*)_tmp318->f1;_tmp31A=_tmp318->f2;}}_LL12D:
 i=_tmp31A;goto inner_loop;_LL12E: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp31B=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp317;if(_tmp31B->tag != 2)goto _LL130;else{if(_tmp31B->f1 != Cyc_Absyn_Mod)goto _LL130;if(_tmp31B->f2 == 0)goto _LL130;_tmp31C=(struct Cyc_Absyn_Exp*)(_tmp31B->f2)->hd;if((_tmp31B->f2)->tl == 0)goto _LL130;_tmp31D=(struct Cyc_Absyn_Exp*)((_tmp31B->f2)->tl)->hd;}}_LL12F: {
# 1437
union Cyc_Relations_RelnOp rop_z=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmp31D,& rop_z)){
# 1440
struct Cyc_List_List*_tmp31E=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlt,rop_z,relns);
# 1442
return !Cyc_Relations_consistent_relations(_tmp31E);}
# 1444
goto _LL12B;}_LL130:;_LL131:
 goto _LL12B;_LL12B:;}
# 1447
return 0;};}
# 1450
static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0){const char*_tmpB74;void*_tmpB73;(_tmpB73=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB74="Missing type in primop ",_tag_dyneither(_tmpB74,sizeof(char),24))),_tag_dyneither(_tmpB73,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));}
# 1454
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0){const char*_tmpB77;void*_tmpB76;(_tmpB76=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB77="Missing type in primop ",_tag_dyneither(_tmpB77,sizeof(char),24))),_tag_dyneither(_tmpB76,sizeof(void*),0)));}
return(void*)_check_null(e->topt);}
# 1458
static struct _tuple11*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
struct _tuple11*_tmpB78;return(_tmpB78=_cycalloc(sizeof(*_tmpB78)),((_tmpB78->f1=Cyc_Toc_mt_tq,((_tmpB78->f2=Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmpB78)))));}
# 1461
static struct _tuple19*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple19*_tmpB79;return(_tmpB79=_cycalloc(sizeof(*_tmpB79)),((_tmpB79->f1=0,((_tmpB79->f2=e,_tmpB79)))));};}
# 1468
static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic,int do_declare){
# 1471
struct Cyc_Absyn_Exp*eo;
void*t;
if(pointer){
struct Cyc_Absyn_Exp*_tmp325=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(x,0),0);
if(*(nv->struct_info).varsizeexp != 0){
_tmp325=Cyc_Absyn_add_exp(_tmp325,(struct Cyc_Absyn_Exp*)_check_null(*(nv->struct_info).varsizeexp),0);
*(nv->struct_info).varsizeexp=0;}
# 1479
t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);
if(rgnopt == 0  || Cyc_Absyn_no_regions)
eo=is_atomic?Cyc_Toc_malloc_atomic(_tmp325): Cyc_Toc_malloc_ptr(_tmp325);else{
# 1483
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);
eo=Cyc_Toc_rmalloc_exp(r,_tmp325);}}else{
# 1488
t=struct_typ;
eo=0;}
# 1491
if(do_declare){
void*_tmp326=x->r;void*_tmp327=_tmp326;struct _tuple1*_tmp329;_LL133: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp328=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp327;if(_tmp328->tag != 1)goto _LL135;else{_tmp329=_tmp328->f1;}}_LL134:
# 1494
 return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp329,t,eo,s,0),0);_LL135:;_LL136: {
# 1496
const char*_tmpB7C;void*_tmpB7B;(_tmpB7B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB7C="bogus x to make_struct",_tag_dyneither(_tmpB7C,sizeof(char),23))),_tag_dyneither(_tmpB7B,sizeof(void*),0)));}_LL132:;}else{
# 1500
if(eo != 0)
return Cyc_Absyn_stmt_exp(Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(x,eo,0),0),s,0),0);else{
# 1503
return Cyc_Absyn_stmt_exp(s,0);}}}
# 1507
static int Cyc_Toc_abstract_aggr(void*typ){
void*_tmp32C=Cyc_Tcutil_compress(typ);void*_tmp32D=_tmp32C;union Cyc_Absyn_AggrInfoU _tmp32F;struct Cyc_List_List*_tmp330;struct Cyc_List_List*_tmp332;_LL138: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp32E=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp32D;if(_tmp32E->tag != 11)goto _LL13A;else{_tmp32F=(_tmp32E->f1).aggr_info;_tmp330=(_tmp32E->f1).targs;}}_LL139: {
# 1510
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(_tmp32F);
if(ad->impl == 0)return 0;{
struct Cyc_List_List*aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
_tmp332=aggrfields;goto _LL13B;};}_LL13A: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp331=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp32D;if(_tmp331->tag != 12)goto _LL13C;else{_tmp332=_tmp331->f2;}}_LL13B:
# 1515
 if(_tmp332 == 0)return 0;
return 1;_LL13C:;_LL13D:
# 1521
 return 0;_LL137:;}
# 1525
static void*Cyc_Toc_array_elt_type(void*t){
void*_tmp333=Cyc_Tcutil_compress(t);void*_tmp334=_tmp333;void*_tmp336;_LL13F: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp335=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp334;if(_tmp335->tag != 8)goto _LL141;else{_tmp336=(_tmp335->f1).elt_type;}}_LL140:
 return _tmp336;_LL141:;_LL142: {
const char*_tmpB80;void*_tmpB7F[1];struct Cyc_String_pa_PrintArg_struct _tmpB7E;(_tmpB7E.tag=0,((_tmpB7E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpB7F[0]=& _tmpB7E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB80="array_elt_type called on %s",_tag_dyneither(_tmpB80,sizeof(char),28))),_tag_dyneither(_tmpB7F,sizeof(void*),1)))))));}_LL13E:;}
# 1532
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated);
# 1538
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s);
# 1543
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,void*array_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){
# 1548
void*orig_elt_type=Cyc_Toc_array_elt_type(array_type);
int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;
{struct Cyc_List_List*_tmp33A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;_tmp33A != 0;_tmp33A=_tmp33A->tl){
# 1552
struct _tuple19*_tmp33B=(struct _tuple19*)_tmp33A->hd;struct Cyc_List_List*_tmp33D;struct Cyc_Absyn_Exp*_tmp33E;struct _tuple19*_tmp33C=_tmp33B;_tmp33D=_tmp33C->f1;_tmp33E=_tmp33C->f2;{
# 1556
struct Cyc_Absyn_Exp*e_index;
if(_tmp33D == 0)
e_index=Cyc_Absyn_signed_int_exp(count --,0);else{
# 1560
if(_tmp33D->tl != 0){const char*_tmpB83;void*_tmpB82;(_tmpB82=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB83="multiple designators in array",_tag_dyneither(_tmpB83,sizeof(char),30))),_tag_dyneither(_tmpB82,sizeof(void*),0)));}{
void*_tmp341=(void*)_tmp33D->hd;
void*_tmp342=_tmp341;struct Cyc_Absyn_Exp*_tmp344;_LL144: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp343=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp342;if(_tmp343->tag != 0)goto _LL146;else{_tmp344=_tmp343->f1;}}_LL145:
# 1564
 Cyc_Toc_exp_to_c(nv,_tmp344);
e_index=_tmp344;
goto _LL143;_LL146: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp345=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp342;if(_tmp345->tag != 1)goto _LL143;}_LL147: {
const char*_tmpB86;void*_tmpB85;(_tmpB85=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB86="field name designators in array",_tag_dyneither(_tmpB86,sizeof(char),32))),_tag_dyneither(_tmpB85,sizeof(void*),0)));}_LL143:;};}{
# 1570
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);
void*_tmp348=_tmp33E->r;void*_tmp349=_tmp348;struct Cyc_List_List*_tmp34B;struct Cyc_Absyn_Vardecl*_tmp34D;struct Cyc_Absyn_Exp*_tmp34E;struct Cyc_Absyn_Exp*_tmp34F;int _tmp350;void*_tmp352;struct Cyc_List_List*_tmp353;_LL149: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp34A=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp349;if(_tmp34A->tag != 25)goto _LL14B;else{_tmp34B=_tmp34A->f1;}}_LL14A:
# 1573
 s=Cyc_Toc_init_array(nv,orig_elt_type,lval,_tmp34B,s);
goto _LL148;_LL14B: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp34C=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp349;if(_tmp34C->tag != 26)goto _LL14D;else{_tmp34D=_tmp34C->f1;_tmp34E=_tmp34C->f2;_tmp34F=_tmp34C->f3;_tmp350=_tmp34C->f4;}}_LL14C:
# 1576
 s=Cyc_Toc_init_comprehension(nv,orig_elt_type,lval,_tmp34D,_tmp34E,_tmp34F,_tmp350,s,0);
# 1578
goto _LL148;_LL14D: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp351=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp349;if(_tmp351->tag != 29)goto _LL14F;else{_tmp352=(void*)_tmp351->f1;_tmp353=_tmp351->f2;}}_LL14E:
# 1580
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp352,_tmp353,s);
goto _LL148;_LL14F: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp354=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp349;if(_tmp354->tag != 27)goto _LL151;}_LL150:
# 1583
 goto _LL148;_LL151:;_LL152:
# 1585
 Cyc_Toc_exp_to_c(nv,_tmp33E);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(orig_elt_type),_tmp33E),0),s,0);
goto _LL148;_LL148:;};};}}
# 1591
return s;}
# 1596
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated){
# 1601
struct _tuple1*_tmp355=vd->name;
void*expected_elt_type;
{void*_tmp356=Cyc_Tcutil_compress(comprehension_type);void*_tmp357=_tmp356;void*_tmp359;void*_tmp35B;_LL154: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp358=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp357;if(_tmp358->tag != 8)goto _LL156;else{_tmp359=(_tmp358->f1).elt_type;}}_LL155:
# 1605
 expected_elt_type=_tmp359;goto _LL153;_LL156: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp35A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp357;if(_tmp35A->tag != 5)goto _LL158;else{_tmp35B=(_tmp35A->f1).elt_typ;}}_LL157:
# 1607
 expected_elt_type=_tmp35B;goto _LL153;_LL158:;_LL159: {
const char*_tmpB89;void*_tmpB88;(_tmpB88=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB89="init_comprehension passed a bad type!",_tag_dyneither(_tmpB89,sizeof(char),38))),_tag_dyneither(_tmpB88,sizeof(void*),0)));}_LL153:;}{
# 1610
void*_tmp35E=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));
if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);
# 1614
{void*_tmp35F=e2->r;void*_tmp360=_tmp35F;_LL15B: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp361=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp360;if(_tmp361->tag != 27)goto _LL15D;}_LL15C:
 return Cyc_Absyn_skip_stmt(0);_LL15D:;_LL15E:
 goto _LL15A;_LL15A:;}{
# 1618
struct _RegionHandle _tmp362=_new_region("r2");struct _RegionHandle*r2=& _tmp362;_push_region(r2);
{struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpB8C;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB8B;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(r2,nv,_tmp355,Cyc_Absyn_varb_exp(_tmp355,(void*)((_tmpB8B=_cycalloc(sizeof(*_tmpB8B)),((_tmpB8B[0]=((_tmpB8C.tag=4,((_tmpB8C.f1=vd,_tmpB8C)))),_tmpB8B)))),0));
struct _tuple1*max=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp355,0),Cyc_Absyn_signed_int_exp(0,0),0);
struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp355,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp355,0),0);
# 1626
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp355,0),0);
struct Cyc_Absyn_Stmt*body;
# 1630
{void*_tmp363=e2->r;void*_tmp364=_tmp363;struct Cyc_List_List*_tmp366;struct Cyc_Absyn_Vardecl*_tmp368;struct Cyc_Absyn_Exp*_tmp369;struct Cyc_Absyn_Exp*_tmp36A;int _tmp36B;void*_tmp36D;struct Cyc_List_List*_tmp36E;_LL160: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp365=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp364;if(_tmp365->tag != 25)goto _LL162;else{_tmp366=_tmp365->f1;}}_LL161:
# 1632
 body=Cyc_Toc_init_array(nv2,expected_elt_type,lval,_tmp366,Cyc_Toc_skip_stmt_dl());
goto _LL15F;_LL162: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp367=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp364;if(_tmp367->tag != 26)goto _LL164;else{_tmp368=_tmp367->f1;_tmp369=_tmp367->f2;_tmp36A=_tmp367->f3;_tmp36B=_tmp367->f4;}}_LL163:
# 1635
 body=Cyc_Toc_init_comprehension(nv2,expected_elt_type,lval,_tmp368,_tmp369,_tmp36A,_tmp36B,Cyc_Toc_skip_stmt_dl(),0);
goto _LL15F;_LL164: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp36C=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp364;if(_tmp36C->tag != 29)goto _LL166;else{_tmp36D=(void*)_tmp36C->f1;_tmp36E=_tmp36C->f2;}}_LL165:
# 1638
 body=Cyc_Toc_init_anon_struct(nv,lval,_tmp36D,_tmp36E,Cyc_Toc_skip_stmt_dl());
goto _LL15F;_LL166:;_LL167:
# 1641
 Cyc_Toc_exp_to_c(nv2,e2);
body=Cyc_Absyn_assign_stmt(lval,Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(expected_elt_type),e2),0);
goto _LL15F;_LL15F:;}{
# 1645
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0);
# 1647
if(zero_term){
# 1652
struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(_tmp35E,Cyc_Absyn_uint_exp(0,0)),0);
s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}{
# 1657
struct Cyc_Absyn_Stmt*_tmp36F=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,e1,
Cyc_Absyn_declare_stmt(_tmp355,Cyc_Absyn_uint_typ,0,s2,0),0),s,0);_npop_handler(0);return _tmp36F;};};}
# 1619
;_pop_region(r2);};};}
# 1663
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s){
# 1666
{struct Cyc_List_List*_tmp372=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles);for(0;_tmp372 != 0;_tmp372=_tmp372->tl){
struct _tuple19*_tmp373=(struct _tuple19*)_tmp372->hd;struct Cyc_List_List*_tmp375;struct Cyc_Absyn_Exp*_tmp376;struct _tuple19*_tmp374=_tmp373;_tmp375=_tmp374->f1;_tmp376=_tmp374->f2;
if(_tmp375 == 0){
const char*_tmpB8F;void*_tmpB8E;(_tmpB8E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB8F="empty designator list",_tag_dyneither(_tmpB8F,sizeof(char),22))),_tag_dyneither(_tmpB8E,sizeof(void*),0)));}
if(_tmp375->tl != 0){
const char*_tmpB92;void*_tmpB91;(_tmpB91=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB92="too many designators in anonymous struct",_tag_dyneither(_tmpB92,sizeof(char),41))),_tag_dyneither(_tmpB91,sizeof(void*),0)));}{
void*_tmp37B=(void*)_tmp375->hd;void*_tmp37C=_tmp37B;struct _dyneither_ptr*_tmp37E;_LL169: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp37D=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp37C;if(_tmp37D->tag != 1)goto _LL16B;else{_tmp37E=_tmp37D->f1;}}_LL16A: {
# 1674
struct Cyc_Absyn_Exp*lval=Cyc_Toc_member_exp(lhs,_tmp37E,0);
{void*_tmp37F=_tmp376->r;void*_tmp380=_tmp37F;struct Cyc_List_List*_tmp382;struct Cyc_Absyn_Vardecl*_tmp384;struct Cyc_Absyn_Exp*_tmp385;struct Cyc_Absyn_Exp*_tmp386;int _tmp387;void*_tmp389;struct Cyc_List_List*_tmp38A;_LL16E: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp381=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp380;if(_tmp381->tag != 25)goto _LL170;else{_tmp382=_tmp381->f1;}}_LL16F:
# 1677
 s=Cyc_Toc_init_array(nv,(void*)_check_null(_tmp376->topt),lval,_tmp382,s);goto _LL16D;_LL170: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp383=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp380;if(_tmp383->tag != 26)goto _LL172;else{_tmp384=_tmp383->f1;_tmp385=_tmp383->f2;_tmp386=_tmp383->f3;_tmp387=_tmp383->f4;}}_LL171:
# 1679
 s=Cyc_Toc_init_comprehension(nv,(void*)_check_null(_tmp376->topt),lval,_tmp384,_tmp385,_tmp386,_tmp387,s,0);
goto _LL16D;_LL172: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp388=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp380;if(_tmp388->tag != 29)goto _LL174;else{_tmp389=(void*)_tmp388->f1;_tmp38A=_tmp388->f2;}}_LL173:
# 1682
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp389,_tmp38A,s);goto _LL16D;_LL174: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp38B=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp380;if(_tmp38B->tag != 27)goto _LL176;}_LL175:
# 1684
 goto _LL16D;_LL176:;_LL177:
# 1686
 Cyc_Toc_exp_to_c(nv,_tmp376);
# 1688
if(Cyc_Toc_is_poly_field(struct_type,_tmp37E))
_tmp376=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp376);
# 1691
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp376,0),0),s,0);
goto _LL16D;_LL16D:;}
# 1694
goto _LL168;}_LL16B:;_LL16C: {
const char*_tmpB95;void*_tmpB94;(_tmpB94=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB95="array designator in struct",_tag_dyneither(_tmpB95,sizeof(char),27))),_tag_dyneither(_tmpB94,sizeof(void*),0)));}_LL168:;};}}
# 1698
return s;}
# 1703
static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){
# 1707
struct _RegionHandle _tmp38E=_new_region("r");struct _RegionHandle*r=& _tmp38E;_push_region(r);
{struct Cyc_List_List*_tmp38F=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);
void*_tmp390=Cyc_Toc_add_tuple_type(_tmp38F);
# 1711
struct _tuple1*_tmp391=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp392=Cyc_Absyn_var_exp(_tmp391,0);
struct Cyc_Absyn_Stmt*_tmp393=Cyc_Absyn_exp_stmt(_tmp392,0);
# 1715
struct Cyc_Absyn_Exp*(*_tmp394)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
# 1717
int is_atomic=1;
struct Cyc_List_List*_tmp395=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);
{int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp395);for(0;_tmp395 != 0;(_tmp395=_tmp395->tl,-- i)){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp395->hd;
struct Cyc_Absyn_Exp*lval=_tmp394(_tmp392,Cyc_Absyn_fieldname(i),0);
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(e->topt));{
void*_tmp396=e->r;void*_tmp397=_tmp396;struct Cyc_List_List*_tmp399;struct Cyc_Absyn_Vardecl*_tmp39B;struct Cyc_Absyn_Exp*_tmp39C;struct Cyc_Absyn_Exp*_tmp39D;int _tmp39E;_LL179: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp398=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp397;if(_tmp398->tag != 25)goto _LL17B;else{_tmp399=_tmp398->f1;}}_LL17A:
# 1725
 _tmp393=Cyc_Toc_init_array(nv,(void*)_check_null(e->topt),lval,_tmp399,_tmp393);
goto _LL178;_LL17B: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp39A=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp397;if(_tmp39A->tag != 26)goto _LL17D;else{_tmp39B=_tmp39A->f1;_tmp39C=_tmp39A->f2;_tmp39D=_tmp39A->f3;_tmp39E=_tmp39A->f4;}}_LL17C:
# 1728
 _tmp393=Cyc_Toc_init_comprehension(nv,(void*)_check_null(e->topt),lval,_tmp39B,_tmp39C,_tmp39D,_tmp39E,_tmp393,0);
# 1730
goto _LL178;_LL17D: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp39F=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp397;if(_tmp39F->tag != 27)goto _LL17F;}_LL17E:
# 1732
 goto _LL178;_LL17F:;_LL180:
# 1735
 Cyc_Toc_exp_to_c(nv,e);
_tmp393=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp394(_tmp392,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp393,0);
# 1738
goto _LL178;_LL178:;};}}{
# 1741
struct Cyc_Absyn_Exp*_tmp3A0=Cyc_Toc_make_struct(nv,Cyc_Absyn_var_exp(_tmp391,0),_tmp390,_tmp393,pointer,rgnopt,is_atomic,1);_npop_handler(0);return _tmp3A0;};}
# 1708
;_pop_region(r);}
# 1745
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp3A1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp3A1 != 0;_tmp3A1=_tmp3A1->tl){
struct Cyc_Absyn_Aggrfield*_tmp3A2=(struct Cyc_Absyn_Aggrfield*)_tmp3A1->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp3A2->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}{
# 1752
struct Cyc_String_pa_PrintArg_struct _tmpB9D;void*_tmpB9C[1];const char*_tmpB9B;void*_tmpB9A;(_tmpB9A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((struct _dyneither_ptr)((_tmpB9D.tag=0,((_tmpB9D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpB9C[0]=& _tmpB9D,Cyc_aprintf(((_tmpB9B="get_member_offset %s failed",_tag_dyneither(_tmpB9B,sizeof(char),28))),_tag_dyneither(_tmpB9C,sizeof(void*),1)))))))),_tag_dyneither(_tmpB9A,sizeof(void*),0)));};}
# 1755
static int Cyc_Toc_init_variable_array(void**typ){
{void*_tmp3A7=Cyc_Tcutil_compress(*typ);void*_tmp3A8=_tmp3A7;void*_tmp3AA;struct Cyc_Absyn_Tqual _tmp3AB;struct Cyc_Absyn_Exp*_tmp3AC;union Cyc_Absyn_Constraint*_tmp3AD;unsigned int _tmp3AE;_LL182: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp3A9=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3A8;if(_tmp3A9->tag != 8)goto _LL184;else{_tmp3AA=(_tmp3A9->f1).elt_type;_tmp3AB=(_tmp3A9->f1).tq;_tmp3AC=(_tmp3A9->f1).num_elts;_tmp3AD=(_tmp3A9->f1).zero_term;_tmp3AE=(_tmp3A9->f1).zt_loc;}}_LL183:
# 1760
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp3AC))){
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpBA3;struct Cyc_Absyn_ArrayInfo _tmpBA2;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpBA1;*typ=(void*)((_tmpBA1=_cycalloc(sizeof(*_tmpBA1)),((_tmpBA1[0]=((_tmpBA3.tag=8,((_tmpBA3.f1=((_tmpBA2.elt_type=_tmp3AA,((_tmpBA2.tq=_tmp3AB,((_tmpBA2.num_elts=Cyc_Absyn_uint_exp(0,0),((_tmpBA2.zero_term=_tmp3AD,((_tmpBA2.zt_loc=_tmp3AE,_tmpBA2)))))))))),_tmpBA3)))),_tmpBA1))));}
return 1;}
# 1764
goto _LL181;_LL184:;_LL185:
 goto _LL181;_LL181:;}
# 1767
return 0;}
# 1770
static int Cyc_Toc_is_array_type(void*t){
void*_tmp3B2=Cyc_Tcutil_compress(t);void*_tmp3B3=_tmp3B2;_LL187: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp3B4=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B3;if(_tmp3B4->tag != 8)goto _LL189;}_LL188:
 return 1;_LL189:;_LL18A:
 return 0;_LL186:;}
# 1777
struct Cyc_Absyn_Aggrdecl*Cyc_Toc_update_aggr_type(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*fn,void*new_field_type){
# 1779
struct Cyc_List_List*new_fields=0;
struct Cyc_List_List*fields;
for(fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;fields != 0;fields=fields->tl){
# 1783
if(!Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,fn)){
struct Cyc_Absyn_Aggrfield*_tmp3B5=(struct Cyc_Absyn_Aggrfield*)fields->hd;
struct Cyc_List_List*_tmp3B6=_tmp3B5->attributes;
void*_tmp3B7=_tmp3B5->type;
if((!Cyc_Toc_is_array_type(_tmp3B7),Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp3B7)))){
struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpBA9;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpBA8;struct Cyc_List_List*_tmpBA7;_tmp3B6=((_tmpBA7=_cycalloc(sizeof(*_tmpBA7)),((_tmpBA7->hd=(void*)((_tmpBA9=_cycalloc(sizeof(*_tmpBA9)),((_tmpBA9[0]=((_tmpBA8.tag=6,((_tmpBA8.f1=0,_tmpBA8)))),_tmpBA9)))),((_tmpBA7->tl=_tmp3B6,_tmpBA7))))));}{
struct Cyc_Absyn_Aggrfield*_tmpBAA;struct Cyc_Absyn_Aggrfield*_tmp3BB=(_tmpBAA=_cycalloc(sizeof(*_tmpBAA)),((_tmpBAA->name=_tmp3B5->name,((_tmpBAA->tq=Cyc_Toc_mt_tq,((_tmpBAA->type=new_field_type,((_tmpBAA->width=_tmp3B5->width,((_tmpBAA->attributes=_tmp3B6,((_tmpBAA->requires_clause=0,_tmpBAA)))))))))))));
# 1795
struct Cyc_List_List*_tmpBAB;new_fields=((_tmpBAB=_cycalloc(sizeof(*_tmpBAB)),((_tmpBAB->hd=_tmp3BB,((_tmpBAB->tl=new_fields,_tmpBAB))))));};}else{
# 1797
struct Cyc_List_List*_tmpBAC;new_fields=((_tmpBAC=_cycalloc(sizeof(*_tmpBAC)),((_tmpBAC->hd=(struct Cyc_Absyn_Aggrfield*)fields->hd,((_tmpBAC->tl=new_fields,_tmpBAC))))));}}{
# 1799
struct Cyc_List_List*_tmp3BF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);
struct Cyc_Int_pa_PrintArg_struct _tmpBB4;void*_tmpBB3[1];const char*_tmpBB2;struct _dyneither_ptr*_tmpBB1;struct _dyneither_ptr*name=
(_tmpBB1=_cycalloc(sizeof(*_tmpBB1)),((_tmpBB1[0]=(struct _dyneither_ptr)((_tmpBB4.tag=1,((_tmpBB4.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpBB3[0]=& _tmpBB4,Cyc_aprintf(((_tmpBB2="_genStruct%d",_tag_dyneither(_tmpBB2,sizeof(char),13))),_tag_dyneither(_tmpBB3,sizeof(void*),1)))))))),_tmpBB1)));
struct _tuple1*_tmpBB5;struct _tuple1*qv=(_tmpBB5=_cycalloc(sizeof(*_tmpBB5)),((_tmpBB5->f1=Cyc_Absyn_Abs_n(0,1),((_tmpBB5->f2=name,_tmpBB5)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpBB6;struct Cyc_Absyn_AggrdeclImpl*_tmp3C0=(_tmpBB6=_cycalloc(sizeof(*_tmpBB6)),((_tmpBB6->exist_vars=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,((_tmpBB6->rgn_po=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po,((_tmpBB6->fields=_tmp3BF,((_tmpBB6->tagged=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged,_tmpBB6)))))))));
# 1807
struct Cyc_Absyn_Aggrdecl*_tmpBB7;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpBB7=_cycalloc(sizeof(*_tmpBB7)),((_tmpBB7->kind=ad->kind,((_tmpBB7->sc=ad->sc,((_tmpBB7->name=qv,((_tmpBB7->tvs=ad->tvs,((_tmpBB7->impl=_tmp3C0,((_tmpBB7->attributes=ad->attributes,_tmpBB7)))))))))))));
# 1813
Cyc_Toc_aggrdecl_to_c(new_ad,1);
return new_ad;};}
# 1817
static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*nv,void*struct_type,struct Cyc_List_List*exist_types,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*dles,struct _tuple1*tdn){
# 1823
int do_declare=0;
struct Cyc_Absyn_Exp*xexp;
if((nv->struct_info).lhs_exp == 0){
struct _tuple1*_tmp3C8=Cyc_Toc_temp_var();
xexp=Cyc_Absyn_var_exp(_tmp3C8,0);
do_declare=1;}else{
# 1831
xexp=(struct Cyc_Absyn_Exp*)_check_null((nv->struct_info).lhs_exp);
nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);}{
# 1834
struct Cyc_Absyn_Stmt*_tmp3C9=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(xexp),0);
# 1838
struct Cyc_Absyn_Exp**varexp;
{void*_tmp3CA=_tmp3C9->r;void*_tmp3CB=_tmp3CA;struct Cyc_Absyn_Exp**_tmp3CD;_LL18C: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp3CC=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3CB;if(_tmp3CC->tag != 1)goto _LL18E;else{_tmp3CD=(struct Cyc_Absyn_Exp**)& _tmp3CC->f1;}}_LL18D:
 varexp=_tmp3CD;goto _LL18B;_LL18E:;_LL18F: {
const char*_tmpBBA;void*_tmpBB9;(_tmpBB9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBBA="stmt not an expression!",_tag_dyneither(_tmpBBA,sizeof(char),24))),_tag_dyneither(_tmpBB9,sizeof(void*),0)));}_LL18B:;}{
# 1844
struct Cyc_Absyn_Exp*(*_tmp3D0)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
void*aggr_typ=Cyc_Toc_typ_to_c(struct_type);
void*orig_typ=aggr_typ;
# 1848
int is_atomic=1;
struct Cyc_List_List*forall_types;
struct Cyc_Absyn_Aggrdecl*ad;
struct Cyc_List_List*aggrfields;struct Cyc_List_List*orig_aggrfields;
struct _dyneither_ptr**index=0;
{void*_tmp3D1=Cyc_Tcutil_compress(struct_type);void*_tmp3D2=_tmp3D1;union Cyc_Absyn_AggrInfoU _tmp3D4;struct Cyc_List_List*_tmp3D5;_LL191: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3D3=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3D2;if(_tmp3D3->tag != 11)goto _LL193;else{_tmp3D4=(_tmp3D3->f1).aggr_info;_tmp3D5=(_tmp3D3->f1).targs;}}_LL192:
# 1855
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp3D4);
orig_aggrfields=(aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
forall_types=_tmp3D5;
goto _LL190;_LL193:;_LL194: {
const char*_tmpBBD;void*_tmpBBC;(_tmpBBC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBBD="init_struct: bad struct type",_tag_dyneither(_tmpBBD,sizeof(char),29))),_tag_dyneither(_tmpBBC,sizeof(void*),0)));}_LL190:;}
# 1862
if(exist_types != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields != 0){
struct Cyc_List_List*_tmp3D8=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
for(0;_tmp3D8->tl != 0;_tmp3D8=_tmp3D8->tl){;}{
struct Cyc_Absyn_Aggrfield*_tmp3D9=(struct Cyc_Absyn_Aggrfield*)_tmp3D8->hd;
struct _RegionHandle _tmp3DA=_new_region("temp");struct _RegionHandle*temp=& _tmp3DA;_push_region(temp);
{struct Cyc_List_List*_tmp3DB=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,forall_types);
# 1871
if(Cyc_Toc_is_abstract_type(Cyc_Tcutil_rsubstitute(temp,_tmp3DB,_tmp3D9->type))){
struct Cyc_List_List*_tmp3DC=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,exist_types);
struct Cyc_List_List*_tmp3DD=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(temp,_tmp3DB,_tmp3DC);
# 1875
struct Cyc_List_List*new_fields=0;
for(_tmp3D8=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;_tmp3D8 != 0;_tmp3D8=_tmp3D8->tl){
struct Cyc_Absyn_Aggrfield*_tmp3DE=(struct Cyc_Absyn_Aggrfield*)_tmp3D8->hd;
struct Cyc_Absyn_Aggrfield*_tmpBBE;struct Cyc_Absyn_Aggrfield*_tmp3DF=(_tmpBBE=_cycalloc(sizeof(*_tmpBBE)),((_tmpBBE->name=_tmp3DE->name,((_tmpBBE->tq=Cyc_Toc_mt_tq,((_tmpBBE->type=
# 1880
Cyc_Tcutil_rsubstitute(temp,_tmp3DD,_tmp3DE->type),((_tmpBBE->width=_tmp3DE->width,((_tmpBBE->attributes=_tmp3DE->attributes,((_tmpBBE->requires_clause=0,_tmpBBE)))))))))))));
# 1888
if(_tmp3D8->tl == 0){
if(Cyc_Toc_init_variable_array(& _tmp3DF->type)){
struct _dyneither_ptr**_tmpBBF;index=((_tmpBBF=_cycalloc(sizeof(*_tmpBBF)),((_tmpBBF[0]=_tmp3DF->name,_tmpBBF))));}}{
# 1895
struct Cyc_List_List*_tmpBC0;new_fields=((_tmpBC0=_cycalloc(sizeof(*_tmpBC0)),((_tmpBC0->hd=_tmp3DF,((_tmpBC0->tl=new_fields,_tmpBC0))))));};}
# 1897
exist_types=0;
aggrfields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);{
struct Cyc_Int_pa_PrintArg_struct _tmpBC8;void*_tmpBC7[1];const char*_tmpBC6;struct _dyneither_ptr*_tmpBC5;struct _dyneither_ptr*name=
(_tmpBC5=_cycalloc(sizeof(*_tmpBC5)),((_tmpBC5[0]=(struct _dyneither_ptr)((_tmpBC8.tag=1,((_tmpBC8.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpBC7[0]=& _tmpBC8,Cyc_aprintf(((_tmpBC6="_genStruct%d",_tag_dyneither(_tmpBC6,sizeof(char),13))),_tag_dyneither(_tmpBC7,sizeof(void*),1)))))))),_tmpBC5)));
struct _tuple1*_tmpBC9;struct _tuple1*qv=(_tmpBC9=_cycalloc(sizeof(*_tmpBC9)),((_tmpBC9->f1=Cyc_Absyn_Abs_n(0,1),((_tmpBC9->f2=name,_tmpBC9)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpBCA;struct Cyc_Absyn_AggrdeclImpl*_tmp3E3=(_tmpBCA=_cycalloc(sizeof(*_tmpBCA)),((_tmpBCA->exist_vars=0,((_tmpBCA->rgn_po=0,((_tmpBCA->fields=aggrfields,((_tmpBCA->tagged=0,_tmpBCA)))))))));
# 1906
struct Cyc_Absyn_Aggrdecl*_tmpBCB;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpBCB=_cycalloc(sizeof(*_tmpBCB)),((_tmpBCB->kind=Cyc_Absyn_StructA,((_tmpBCB->sc=Cyc_Absyn_Public,((_tmpBCB->name=qv,((_tmpBCB->tvs=0,((_tmpBCB->impl=_tmp3E3,((_tmpBCB->attributes=0,_tmpBCB)))))))))))));
# 1912
Cyc_Toc_aggrdecl_to_c(new_ad,1);
ad=new_ad;
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpBD5;struct Cyc_Absyn_Aggrdecl**_tmpBD4;struct Cyc_Absyn_AggrInfo _tmpBD3;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpBD2;struct_type=(void*)((_tmpBD2=_cycalloc(sizeof(*_tmpBD2)),((_tmpBD2[0]=((_tmpBD5.tag=11,((_tmpBD5.f1=((_tmpBD3.aggr_info=Cyc_Absyn_KnownAggr(((_tmpBD4=_cycalloc(sizeof(*_tmpBD4)),((_tmpBD4[0]=new_ad,_tmpBD4))))),((_tmpBD3.targs=0,_tmpBD3)))),_tmpBD5)))),_tmpBD2))));}
# 1916
aggr_typ=Cyc_Toc_typ_to_c(struct_type);};}}
# 1867
;_pop_region(temp);};}{
# 1920
int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct _RegionHandle _tmp3EF=_new_region("r");struct _RegionHandle*r=& _tmp3EF;_push_region(r);
{struct Cyc_List_List*_tmp3F0=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp3F0 != 0;_tmp3F0=_tmp3F0->tl){
struct _tuple19*_tmp3F1=(struct _tuple19*)_tmp3F0->hd;struct Cyc_List_List*_tmp3F3;struct Cyc_Absyn_Exp*_tmp3F4;struct _tuple19*_tmp3F2=_tmp3F1;_tmp3F3=_tmp3F2->f1;_tmp3F4=_tmp3F2->f2;
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(_tmp3F4->topt));
if(_tmp3F3 == 0){
const char*_tmpBD8;void*_tmpBD7;(_tmpBD7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBD8="empty designator list",_tag_dyneither(_tmpBD8,sizeof(char),22))),_tag_dyneither(_tmpBD7,sizeof(void*),0)));}
if(_tmp3F3->tl != 0){
# 1930
struct _tuple1*_tmp3F7=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3F8=Cyc_Absyn_var_exp(_tmp3F7,0);
for(0;_tmp3F3 != 0;_tmp3F3=_tmp3F3->tl){
void*_tmp3F9=(void*)_tmp3F3->hd;void*_tmp3FA=_tmp3F9;struct _dyneither_ptr*_tmp3FC;_LL196: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3FB=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3FA;if(_tmp3FB->tag != 1)goto _LL198;else{_tmp3FC=_tmp3FB->f1;}}_LL197:
# 1936
 if(Cyc_Toc_is_poly_field(struct_type,_tmp3FC))
_tmp3F8=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3F8);
_tmp3C9=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3D0(xexp,_tmp3FC,0),_tmp3F8,0),0),_tmp3C9,0);
# 1940
goto _LL195;_LL198:;_LL199: {
const char*_tmpBDB;void*_tmpBDA;(_tmpBDA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBDB="array designator in struct",_tag_dyneither(_tmpBDB,sizeof(char),27))),_tag_dyneither(_tmpBDA,sizeof(void*),0)));}_LL195:;}
# 1944
Cyc_Toc_exp_to_c(nv,_tmp3F4);
_tmp3C9=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3F8,_tmp3F4,0),0),_tmp3C9,0);}else{
# 1947
void*_tmp3FF=(void*)_tmp3F3->hd;void*_tmp400=_tmp3FF;struct _dyneither_ptr*_tmp402;_LL19B: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp401=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp400;if(_tmp401->tag != 1)goto _LL19D;else{_tmp402=_tmp401->f1;}}_LL19C: {
# 1949
struct Cyc_Absyn_Exp*lval=_tmp3D0(xexp,_tmp402,0);
if(is_tagged_union){
int i=Cyc_Toc_get_member_offset(ad,_tmp402);
struct Cyc_Absyn_Exp*f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct Cyc_Absyn_Exp*lhs=Cyc_Toc_member_exp(lval,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,f_tag_exp,0);
_tmp3C9=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp3C9,0);
lval=Cyc_Toc_member_exp(lval,Cyc_Toc_val_sp,0);}{
# 1958
int e1_translated=0;
{void*_tmp403=_tmp3F4->r;void*_tmp404=_tmp403;struct Cyc_List_List*_tmp406;struct Cyc_Absyn_Vardecl*_tmp408;struct Cyc_Absyn_Exp*_tmp409;struct Cyc_Absyn_Exp*_tmp40A;int _tmp40B;struct Cyc_Absyn_Exp*_tmp40D;void*_tmp40E;void*_tmp410;struct Cyc_List_List*_tmp411;_LL1A0: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp405=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp404;if(_tmp405->tag != 25)goto _LL1A2;else{_tmp406=_tmp405->f1;}}_LL1A1:
# 1961
 _tmp3C9=Cyc_Toc_init_array(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(aggrfields,_tmp402)))->type,lval,_tmp406,_tmp3C9);
# 1963
goto _LL19F;_LL1A2: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp407=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp404;if(_tmp407->tag != 26)goto _LL1A4;else{_tmp408=_tmp407->f1;_tmp409=_tmp407->f2;_tmp40A=_tmp407->f3;_tmp40B=_tmp407->f4;}}_LL1A3:
# 1965
 _tmp3C9=Cyc_Toc_init_comprehension(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(do_declare?aggrfields: orig_aggrfields,_tmp402)))->type,lval,_tmp408,_tmp409,_tmp40A,_tmp40B,_tmp3C9,0);
# 1969
e1_translated=1;
_tmp40D=_tmp409;_tmp40E=(void*)_check_null(_tmp40A->topt);goto _LL1A5;_LL1A4: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp40C=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp404;if(_tmp40C->tag != 27)goto _LL1A6;else{_tmp40D=_tmp40C->f1;_tmp40E=(void*)_tmp40C->f2;}}_LL1A5:
# 1974
 if(index != 0  && !Cyc_strptrcmp(*index,_tmp402)){
if(!e1_translated)Cyc_Toc_exp_to_c(nv,_tmp40D);{
void*_tmp412=Cyc_Toc_typ_to_c(_tmp40E);
struct _tuple6 _tmpBDF;union Cyc_Absyn_Cnst _tmpBDE;struct Cyc_Absyn_Exp*_tmpBDC[2];*(nv->struct_info).varsizeexp=
Cyc_Absyn_add_exp(Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpBDC[1]=Cyc_Absyn_sizeoftyp_exp(_tmp412,0),((_tmpBDC[0]=_tmp40D,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBDC,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),
Cyc_Absyn_const_exp((((_tmpBDE.Int_c).val=((_tmpBDF.f1=Cyc_Absyn_Unsigned,((_tmpBDF.f2=(int)sizeof(double),_tmpBDF)))),(((_tmpBDE.Int_c).tag=5,_tmpBDE)))),0),0);};}
# 1982
goto _LL19F;_LL1A6: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp40F=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp404;if(_tmp40F->tag != 29)goto _LL1A8;else{_tmp410=(void*)_tmp40F->f1;_tmp411=_tmp40F->f2;}}_LL1A7:
# 1984
 _tmp3C9=Cyc_Toc_init_anon_struct(nv,lval,_tmp410,_tmp411,_tmp3C9);goto _LL19F;_LL1A8:;_LL1A9: {
# 1986
void*old_e_typ=(void*)_check_null(_tmp3F4->topt);
int was_ptr_type=Cyc_Toc_is_pointer_or_boxed_tvar(old_e_typ);
Cyc_Toc_exp_to_c(Cyc_Toc_set_lhs_exp(nv->rgn,lval,nv),_tmp3F4);
{void*_tmp416=old_e_typ;_LL1AB: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp417=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp416;if(_tmp417->tag != 11)goto _LL1AD;}_LL1AC:
# 1993
 if(old_e_typ != _tmp3F4->topt  && !Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(old_e_typ),(void*)_check_null(_tmp3F4->topt))){
# 1997
ad=Cyc_Toc_update_aggr_type(ad,_tmp402,(void*)_check_null(_tmp3F4->topt));
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpBE9;struct Cyc_Absyn_Aggrdecl**_tmpBE8;struct Cyc_Absyn_AggrInfo _tmpBE7;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpBE6;struct_type=(void*)(
(_tmpBE6=_cycalloc(sizeof(*_tmpBE6)),((_tmpBE6[0]=((_tmpBE9.tag=11,((_tmpBE9.f1=((_tmpBE7.aggr_info=Cyc_Absyn_KnownAggr(((_tmpBE8=_cycalloc(sizeof(*_tmpBE8)),((_tmpBE8[0]=ad,_tmpBE8))))),((_tmpBE7.targs=0,_tmpBE7)))),_tmpBE9)))),_tmpBE6))));}
# 2001
aggr_typ=Cyc_Toc_typ_to_c(struct_type);}
# 2003
goto _LL1AA;_LL1AD:;_LL1AE:
 goto _LL1AA;_LL1AA:;}{
# 2006
struct Cyc_Absyn_Aggrfield*_tmp41C=Cyc_Absyn_lookup_field(aggrfields,_tmp402);
# 2008
if(Cyc_Toc_is_poly_field(struct_type,_tmp402) && !was_ptr_type)
_tmp3F4=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3F4);
# 2011
if(exist_types != 0)
_tmp3F4=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp41C))->type),_tmp3F4);
# 2014
_tmp3C9=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp3F4,0),0),_tmp3C9,0);
goto _LL19F;};}_LL19F:;}
# 2017
goto _LL19A;};}_LL19D:;_LL19E: {
const char*_tmpBEC;void*_tmpBEB;(_tmpBEB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBEC="array designator in struct",_tag_dyneither(_tmpBEC,sizeof(char),27))),_tag_dyneither(_tmpBEB,sizeof(void*),0)));}_LL19A:;}}}
# 2024
if(aggr_typ != orig_typ  && pointer){
void*cast_type=Cyc_Absyn_cstar_typ(orig_typ,Cyc_Toc_mt_tq);
*varexp=Cyc_Toc_cast_it(cast_type,*varexp);}{
# 2029
struct Cyc_Absyn_Exp*_tmp41F=Cyc_Toc_make_struct(nv,xexp,aggr_typ,_tmp3C9,pointer,rgnopt,is_atomic,do_declare);
# 2031
_tmp41F->topt=struct_type;{
struct Cyc_Absyn_Exp*_tmp420=_tmp41F;_npop_handler(0);return _tmp420;};};
# 1922
;_pop_region(r);};};};}struct _tuple27{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 2037
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple27*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
# 2040
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0);}
# 2043
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmpBEF;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmpBEE;return Cyc_Absyn_new_exp((void*)((_tmpBEE=_cycalloc(sizeof(*_tmpBEE)),((_tmpBEE[0]=((_tmpBEF.tag=4,((_tmpBEF.f1=e1,((_tmpBEF.f2=incr,_tmpBEF)))))),_tmpBEE)))),0);}
# 2047
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 2056
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 2058
void*_tmp423=e1->r;void*_tmp424=_tmp423;struct Cyc_Absyn_Stmt*_tmp426;void*_tmp428;struct Cyc_Absyn_Exp*_tmp429;struct Cyc_Absyn_Exp*_tmp42B;struct _dyneither_ptr*_tmp42C;int _tmp42D;int _tmp42E;_LL1B0: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp425=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp424;if(_tmp425->tag != 36)goto _LL1B2;else{_tmp426=_tmp425->f1;}}_LL1B1:
 Cyc_Toc_lvalue_assign_stmt(_tmp426,fs,f,f_env);goto _LL1AF;_LL1B2: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp427=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp424;if(_tmp427->tag != 13)goto _LL1B4;else{_tmp428=(void*)_tmp427->f1;_tmp429=_tmp427->f2;}}_LL1B3:
 Cyc_Toc_lvalue_assign(_tmp429,fs,f,f_env);goto _LL1AF;_LL1B4: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp42A=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp424;if(_tmp42A->tag != 20)goto _LL1B6;else{_tmp42B=_tmp42A->f1;_tmp42C=_tmp42A->f2;_tmp42D=_tmp42A->f3;_tmp42E=_tmp42A->f4;}}_LL1B5:
# 2063
 e1->r=_tmp42B->r;
{struct Cyc_List_List*_tmpBF0;Cyc_Toc_lvalue_assign(e1,((_tmpBF0=_cycalloc(sizeof(*_tmpBF0)),((_tmpBF0->hd=_tmp42C,((_tmpBF0->tl=fs,_tmpBF0)))))),f,f_env);}
goto _LL1AF;_LL1B6:;_LL1B7: {
# 2071
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 2073
for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;
goto _LL1AF;}_LL1AF:;}
# 2079
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 2081
void*_tmp430=s->r;void*_tmp431=_tmp430;struct Cyc_Absyn_Exp*_tmp433;struct Cyc_Absyn_Decl*_tmp435;struct Cyc_Absyn_Stmt*_tmp436;struct Cyc_Absyn_Stmt*_tmp438;_LL1B9: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp432=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp431;if(_tmp432->tag != 1)goto _LL1BB;else{_tmp433=_tmp432->f1;}}_LL1BA:
 Cyc_Toc_lvalue_assign(_tmp433,fs,f,f_env);goto _LL1B8;_LL1BB: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp434=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp431;if(_tmp434->tag != 12)goto _LL1BD;else{_tmp435=_tmp434->f1;_tmp436=_tmp434->f2;}}_LL1BC:
# 2084
 Cyc_Toc_lvalue_assign_stmt(_tmp436,fs,f,f_env);goto _LL1B8;_LL1BD: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp437=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp431;if(_tmp437->tag != 2)goto _LL1BF;else{_tmp438=_tmp437->f2;}}_LL1BE:
 Cyc_Toc_lvalue_assign_stmt(_tmp438,fs,f,f_env);goto _LL1B8;_LL1BF:;_LL1C0: {
const char*_tmpBF4;void*_tmpBF3[1];struct Cyc_String_pa_PrintArg_struct _tmpBF2;(_tmpBF2.tag=0,((_tmpBF2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpBF3[0]=& _tmpBF2,Cyc_Toc_toc_impos(((_tmpBF4="lvalue_assign_stmt: %s",_tag_dyneither(_tmpBF4,sizeof(char),23))),_tag_dyneither(_tmpBF3,sizeof(void*),1)))))));}_LL1B8:;}
# 2090
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 2094
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp43C=e->r;void*_tmp43D=_tmp43C;void**_tmp43F;struct Cyc_Absyn_Exp**_tmp440;struct Cyc_Absyn_Exp*_tmp442;struct Cyc_Absyn_Stmt*_tmp444;_LL1C2: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp43E=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp43D;if(_tmp43E->tag != 13)goto _LL1C4;else{_tmp43F=(void**)((void**)& _tmp43E->f1);_tmp440=(struct Cyc_Absyn_Exp**)& _tmp43E->f2;}}_LL1C3:
# 2097
*_tmp440=Cyc_Toc_push_address_exp(*_tmp440);
*_tmp43F=Cyc_Absyn_cstar_typ(*_tmp43F,Cyc_Toc_mt_tq);
return e;_LL1C4: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp441=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp43D;if(_tmp441->tag != 19)goto _LL1C6;else{_tmp442=_tmp441->f1;}}_LL1C5:
# 2101
 return _tmp442;_LL1C6: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp443=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp43D;if(_tmp443->tag != 36)goto _LL1C8;else{_tmp444=_tmp443->f1;}}_LL1C7:
# 2105
 Cyc_Toc_push_address_stmt(_tmp444);
return e;_LL1C8:;_LL1C9:
# 2108
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{
const char*_tmpBF8;void*_tmpBF7[1];struct Cyc_String_pa_PrintArg_struct _tmpBF6;(_tmpBF6.tag=0,((_tmpBF6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpBF7[0]=& _tmpBF6,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBF8="can't take & of exp %s",_tag_dyneither(_tmpBF8,sizeof(char),23))),_tag_dyneither(_tmpBF7,sizeof(void*),1)))))));};_LL1C1:;}
# 2113
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp448=s->r;void*_tmp449=_tmp448;struct Cyc_Absyn_Stmt*_tmp44B;struct Cyc_Absyn_Stmt*_tmp44D;struct Cyc_Absyn_Exp**_tmp44F;_LL1CB: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp44A=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp449;if(_tmp44A->tag != 2)goto _LL1CD;else{_tmp44B=_tmp44A->f2;}}_LL1CC:
 _tmp44D=_tmp44B;goto _LL1CE;_LL1CD: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp44C=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp449;if(_tmp44C->tag != 12)goto _LL1CF;else{_tmp44D=_tmp44C->f2;}}_LL1CE:
 Cyc_Toc_push_address_stmt(_tmp44D);goto _LL1CA;_LL1CF: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp44E=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp449;if(_tmp44E->tag != 1)goto _LL1D1;else{_tmp44F=(struct Cyc_Absyn_Exp**)& _tmp44E->f1;}}_LL1D0:
*_tmp44F=Cyc_Toc_push_address_exp(*_tmp44F);goto _LL1CA;_LL1D1:;_LL1D2: {
# 2119
const char*_tmpBFC;void*_tmpBFB[1];struct Cyc_String_pa_PrintArg_struct _tmpBFA;(_tmpBFA.tag=0,((_tmpBFA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpBFB[0]=& _tmpBFA,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBFC="can't take & of stmt %s",_tag_dyneither(_tmpBFC,sizeof(char),24))),_tag_dyneither(_tmpBFB,sizeof(void*),1)))))));}_LL1CA:;}
# 2124
static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 2126
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 2128
if(x == 0)return 0;
{struct Cyc_List_List*_tmpBFD;result=((_tmpBFD=_region_malloc(r2,sizeof(*_tmpBFD)),((_tmpBFD->hd=f(x->hd,env),((_tmpBFD->tl=0,_tmpBFD))))));}
prev=result;
for(x=x->tl;x != 0;x=x->tl){
{struct Cyc_List_List*_tmpBFE;((struct Cyc_List_List*)_check_null(prev))->tl=((_tmpBFE=_region_malloc(r2,sizeof(*_tmpBFE)),((_tmpBFE->hd=f(x->hd,env),((_tmpBFE->tl=0,_tmpBFE))))));}
prev=prev->tl;}
# 2135
return result;}
# 2137
static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,x);}
# 2141
static struct _tuple19*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){
struct _tuple19*_tmpBFF;return(_tmpBFF=_cycalloc(sizeof(*_tmpBFF)),((_tmpBFF->f1=0,((_tmpBFF->f2=e,_tmpBFF)))));}
# 2145
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_tmp456=Cyc_Tcutil_compress(t);void*_tmp457=_tmp456;struct Cyc_Absyn_PtrInfo _tmp459;_LL1D4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp458=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp457;if(_tmp458->tag != 5)goto _LL1D6;else{_tmp459=_tmp458->f1;}}_LL1D5:
 return _tmp459;_LL1D6:;_LL1D7: {
const char*_tmpC02;void*_tmpC01;(_tmpC01=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC02="get_ptr_typ: not a pointer!",_tag_dyneither(_tmpC02,sizeof(char),28))),_tag_dyneither(_tmpC01,sizeof(void*),0)));}_LL1D3:;}
# 2155
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp45C=Cyc_Tcutil_compress(t);void*_tmp45D=_tmp45C;enum Cyc_Absyn_Sign _tmp460;enum Cyc_Absyn_Sign _tmp462;enum Cyc_Absyn_Sign _tmp466;enum Cyc_Absyn_Sign _tmp468;enum Cyc_Absyn_Sign _tmp46A;int _tmp46E;_LL1D9: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp45E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp45D;if(_tmp45E->tag != 5)goto _LL1DB;}_LL1DA:
 res=Cyc_Absyn_null_exp(0);goto _LL1D8;_LL1DB: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp45F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp45D;if(_tmp45F->tag != 6)goto _LL1DD;else{_tmp460=_tmp45F->f1;if(_tmp45F->f2 != Cyc_Absyn_Char_sz)goto _LL1DD;}}_LL1DC:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp460,'\000'),0);goto _LL1D8;_LL1DD: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp461=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp45D;if(_tmp461->tag != 6)goto _LL1DF;else{_tmp462=_tmp461->f1;if(_tmp461->f2 != Cyc_Absyn_Short_sz)goto _LL1DF;}}_LL1DE:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp462,0),0);goto _LL1D8;_LL1DF: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp463=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp45D;if(_tmp463->tag != 13)goto _LL1E1;}_LL1E0:
 goto _LL1E2;_LL1E1: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp464=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp45D;if(_tmp464->tag != 14)goto _LL1E3;}_LL1E2:
 _tmp466=Cyc_Absyn_Unsigned;goto _LL1E4;_LL1E3: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp465=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp45D;if(_tmp465->tag != 6)goto _LL1E5;else{_tmp466=_tmp465->f1;if(_tmp465->f2 != Cyc_Absyn_Int_sz)goto _LL1E5;}}_LL1E4:
 _tmp468=_tmp466;goto _LL1E6;_LL1E5: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp467=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp45D;if(_tmp467->tag != 6)goto _LL1E7;else{_tmp468=_tmp467->f1;if(_tmp467->f2 != Cyc_Absyn_Long_sz)goto _LL1E7;}}_LL1E6:
# 2165
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp468,0),0);goto _LL1D8;_LL1E7: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp469=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp45D;if(_tmp469->tag != 6)goto _LL1E9;else{_tmp46A=_tmp469->f1;if(_tmp469->f2 != Cyc_Absyn_LongLong_sz)goto _LL1E9;}}_LL1E8:
# 2167
 res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp46A,(long long)0),0);goto _LL1D8;_LL1E9: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp46B=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp45D;if(_tmp46B->tag != 7)goto _LL1EB;else{if(_tmp46B->f1 != 0)goto _LL1EB;}}_LL1EA:
{const char*_tmpC03;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpC03="0.0F",_tag_dyneither(_tmpC03,sizeof(char),5))),0),0);}goto _LL1D8;_LL1EB: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp46C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp45D;if(_tmp46C->tag != 7)goto _LL1ED;else{if(_tmp46C->f1 != 1)goto _LL1ED;}}_LL1EC:
{const char*_tmpC04;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpC04="0.0",_tag_dyneither(_tmpC04,sizeof(char),4))),1),0);}goto _LL1D8;_LL1ED: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp46D=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp45D;if(_tmp46D->tag != 7)goto _LL1EF;else{_tmp46E=_tmp46D->f1;}}_LL1EE:
{const char*_tmpC05;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpC05="0.0L",_tag_dyneither(_tmpC05,sizeof(char),5))),_tmp46E),0);}goto _LL1D8;_LL1EF:;_LL1F0: {
# 2172
const char*_tmpC09;void*_tmpC08[1];struct Cyc_String_pa_PrintArg_struct _tmpC07;(_tmpC07.tag=0,((_tmpC07.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpC08[0]=& _tmpC07,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC09="found non-zero type %s in generate_zero",_tag_dyneither(_tmpC09,sizeof(char),40))),_tag_dyneither(_tmpC08,sizeof(void*),1)))))));}_LL1D8:;}
# 2174
res->topt=t;
return res;}
# 2181
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 2194 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp475=Cyc_Toc_typ_to_c(elt_type);
void*_tmp476=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp477=Cyc_Absyn_cstar_typ(_tmp475,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmpC0A;struct Cyc_Core_Opt*_tmp478=(_tmpC0A=_cycalloc(sizeof(*_tmpC0A)),((_tmpC0A->v=_tmp477,_tmpC0A)));
struct Cyc_Absyn_Exp*xinit;
{void*_tmp479=e1->r;void*_tmp47A=_tmp479;struct Cyc_Absyn_Exp*_tmp47C;struct Cyc_Absyn_Exp*_tmp47E;struct Cyc_Absyn_Exp*_tmp47F;_LL1F2: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp47B=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp47A;if(_tmp47B->tag != 19)goto _LL1F4;else{_tmp47C=_tmp47B->f1;}}_LL1F3:
# 2202
 if(!is_dyneither){
_tmp47C=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp47C,0,Cyc_Absyn_Other_coercion,0);
_tmp47C->topt=fat_ptr_type;}
# 2206
Cyc_Toc_exp_to_c(nv,_tmp47C);xinit=_tmp47C;goto _LL1F1;_LL1F4: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp47D=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp47A;if(_tmp47D->tag != 22)goto _LL1F6;else{_tmp47E=_tmp47D->f1;_tmp47F=_tmp47D->f2;}}_LL1F5:
# 2208
 if(!is_dyneither){
_tmp47E=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp47E,0,Cyc_Absyn_Other_coercion,0);
_tmp47E->topt=fat_ptr_type;}
# 2212
Cyc_Toc_exp_to_c(nv,_tmp47E);Cyc_Toc_exp_to_c(nv,_tmp47F);
{struct Cyc_Absyn_Exp*_tmpC0B[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpC0B[2]=_tmp47F,((_tmpC0B[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpC0B[0]=_tmp47E,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC0B,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL1F1;_LL1F6:;_LL1F7: {
const char*_tmpC0E;void*_tmpC0D;(_tmpC0D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC0E="found bad lhs for zero-terminated pointer assignment",_tag_dyneither(_tmpC0E,sizeof(char),53))),_tag_dyneither(_tmpC0D,sizeof(void*),0)));}_LL1F1:;}{
# 2218
struct _tuple1*_tmp483=Cyc_Toc_temp_var();
struct _RegionHandle _tmp484=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp484;_push_region(rgn2);
{struct Cyc_Toc_Env*_tmp485=Cyc_Toc_add_varmap(rgn2,nv,_tmp483,Cyc_Absyn_var_exp(_tmp483,0));
struct Cyc_Absyn_Vardecl*_tmpC0F;struct Cyc_Absyn_Vardecl*_tmp486=(_tmpC0F=_cycalloc(sizeof(*_tmpC0F)),((_tmpC0F->sc=Cyc_Absyn_Public,((_tmpC0F->name=_tmp483,((_tmpC0F->tq=Cyc_Toc_mt_tq,((_tmpC0F->type=_tmp476,((_tmpC0F->initializer=xinit,((_tmpC0F->rgn=0,((_tmpC0F->attributes=0,((_tmpC0F->escapes=0,_tmpC0F)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpC12;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpC11;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp487=(_tmpC11=_cycalloc(sizeof(*_tmpC11)),((_tmpC11[0]=((_tmpC12.tag=4,((_tmpC12.f1=_tmp486,_tmpC12)))),_tmpC11)));
struct Cyc_Absyn_Exp*_tmp488=Cyc_Absyn_varb_exp(_tmp483,(void*)_tmp487,0);
_tmp488->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp489=Cyc_Absyn_deref_exp(_tmp488,0);
_tmp489->topt=elt_type;
Cyc_Toc_exp_to_c(_tmp485,_tmp489);{
struct _tuple1*_tmp48A=Cyc_Toc_temp_var();
_tmp485=Cyc_Toc_add_varmap(rgn2,_tmp485,_tmp48A,Cyc_Absyn_var_exp(_tmp48A,0));{
struct Cyc_Absyn_Vardecl*_tmpC13;struct Cyc_Absyn_Vardecl*_tmp48B=(_tmpC13=_cycalloc(sizeof(*_tmpC13)),((_tmpC13->sc=Cyc_Absyn_Public,((_tmpC13->name=_tmp48A,((_tmpC13->tq=Cyc_Toc_mt_tq,((_tmpC13->type=_tmp475,((_tmpC13->initializer=_tmp489,((_tmpC13->rgn=0,((_tmpC13->attributes=0,((_tmpC13->escapes=0,_tmpC13)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpC16;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpC15;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp48C=(_tmpC15=_cycalloc(sizeof(*_tmpC15)),((_tmpC15[0]=((_tmpC16.tag=4,((_tmpC16.f1=_tmp48B,_tmpC16)))),_tmpC15)));
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp48D=Cyc_Absyn_varb_exp(_tmp48A,(void*)_tmp48C,0);
_tmp48D->topt=_tmp489->topt;
z_init=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp48D,e2,0);
z_init->topt=_tmp48D->topt;}
# 2239
Cyc_Toc_exp_to_c(_tmp485,z_init);{
struct _tuple1*_tmp48E=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmpC17;struct Cyc_Absyn_Vardecl*_tmp48F=(_tmpC17=_cycalloc(sizeof(*_tmpC17)),((_tmpC17->sc=Cyc_Absyn_Public,((_tmpC17->name=_tmp48E,((_tmpC17->tq=Cyc_Toc_mt_tq,((_tmpC17->type=_tmp475,((_tmpC17->initializer=z_init,((_tmpC17->rgn=0,((_tmpC17->attributes=0,((_tmpC17->escapes=0,_tmpC17)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpC1A;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpC19;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp490=(_tmpC19=_cycalloc(sizeof(*_tmpC19)),((_tmpC19[0]=((_tmpC1A.tag=4,((_tmpC1A.f1=_tmp48F,_tmpC1A)))),_tmpC19)));
_tmp485=Cyc_Toc_add_varmap(rgn2,_tmp485,_tmp48E,Cyc_Absyn_var_exp(_tmp48E,0));{
# 2246
struct Cyc_Absyn_Exp*_tmp491=Cyc_Absyn_varb_exp(_tmp48A,(void*)_tmp48C,0);_tmp491->topt=_tmp489->topt;{
struct Cyc_Absyn_Exp*_tmp492=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp493=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp491,_tmp492,0);
_tmp493->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp485,_tmp493);{
# 2252
struct Cyc_Absyn_Exp*_tmp494=Cyc_Absyn_varb_exp(_tmp48E,(void*)_tmp490,0);_tmp494->topt=_tmp489->topt;{
struct Cyc_Absyn_Exp*_tmp495=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp496=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp494,_tmp495,0);
_tmp496->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp485,_tmp496);{
# 2258
struct Cyc_Absyn_Exp*_tmpC1B[2];struct Cyc_List_List*_tmp497=(_tmpC1B[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpC1B[0]=
# 2258
Cyc_Absyn_varb_exp(_tmp483,(void*)_tmp487,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC1B,sizeof(struct Cyc_Absyn_Exp*),2)))));
# 2260
struct Cyc_Absyn_Exp*_tmp498=Cyc_Absyn_uint_exp(1,0);
struct Cyc_Absyn_Exp*xsize;
xsize=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp497,0),_tmp498,0);{
# 2265
struct Cyc_Absyn_Exp*_tmp499=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp493,_tmp496,0),0);
struct Cyc_Absyn_Stmt*_tmp49A=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0),0);
struct Cyc_Absyn_Exp*_tmp49B=Cyc_Toc_member_exp(Cyc_Absyn_varb_exp(_tmp483,(void*)_tmp487,0),Cyc_Toc_curr_sp,0);
_tmp49B=Cyc_Toc_cast_it(_tmp477,_tmp49B);{
struct Cyc_Absyn_Exp*_tmp49C=Cyc_Absyn_deref_exp(_tmp49B,0);
struct Cyc_Absyn_Exp*_tmp49D=Cyc_Absyn_assign_exp(_tmp49C,Cyc_Absyn_var_exp(_tmp48E,0),0);
struct Cyc_Absyn_Stmt*_tmp49E=Cyc_Absyn_exp_stmt(_tmp49D,0);
_tmp49E=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp499,_tmp49A,Cyc_Absyn_skip_stmt(0),0),_tmp49E,0);
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpC21;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpC20;struct Cyc_Absyn_Decl*_tmpC1F;_tmp49E=Cyc_Absyn_decl_stmt(((_tmpC1F=_cycalloc(sizeof(*_tmpC1F)),((_tmpC1F->r=(void*)((_tmpC21=_cycalloc(sizeof(*_tmpC21)),((_tmpC21[0]=((_tmpC20.tag=0,((_tmpC20.f1=_tmp48F,_tmpC20)))),_tmpC21)))),((_tmpC1F->loc=0,_tmpC1F)))))),_tmp49E,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpC27;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpC26;struct Cyc_Absyn_Decl*_tmpC25;_tmp49E=Cyc_Absyn_decl_stmt(((_tmpC25=_cycalloc(sizeof(*_tmpC25)),((_tmpC25->r=(void*)((_tmpC27=_cycalloc(sizeof(*_tmpC27)),((_tmpC27[0]=((_tmpC26.tag=0,((_tmpC26.f1=_tmp48B,_tmpC26)))),_tmpC27)))),((_tmpC25->loc=0,_tmpC25)))))),_tmp49E,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpC2D;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpC2C;struct Cyc_Absyn_Decl*_tmpC2B;_tmp49E=Cyc_Absyn_decl_stmt(((_tmpC2B=_cycalloc(sizeof(*_tmpC2B)),((_tmpC2B->r=(void*)((_tmpC2D=_cycalloc(sizeof(*_tmpC2D)),((_tmpC2D[0]=((_tmpC2C.tag=0,((_tmpC2C.f1=_tmp486,_tmpC2C)))),_tmpC2D)))),((_tmpC2B->loc=0,_tmpC2B)))))),_tmp49E,0);}
e->r=Cyc_Toc_stmt_exp_r(_tmp49E);};};};};};};};};};};};}
# 2220
;_pop_region(rgn2);};}
# 2291 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 2295
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp4B3=Cyc_Tcutil_compress(aggrtype);void*_tmp4B4=_tmp4B3;union Cyc_Absyn_AggrInfoU _tmp4B6;_LL1F9: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4B5=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4B4;if(_tmp4B5->tag != 11)goto _LL1FB;else{_tmp4B6=(_tmp4B5->f1).aggr_info;}}_LL1FA:
# 2298
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp4B6);goto _LL1F8;_LL1FB:;_LL1FC: {
struct Cyc_String_pa_PrintArg_struct _tmpC35;void*_tmpC34[1];const char*_tmpC33;void*_tmpC32;(_tmpC32=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmpC35.tag=0,((_tmpC35.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpC34[0]=& _tmpC35,Cyc_aprintf(((_tmpC33="expecting union but found %s in check_tagged_union",_tag_dyneither(_tmpC33,sizeof(char),51))),_tag_dyneither(_tmpC34,sizeof(void*),1)))))))),_tag_dyneither(_tmpC32,sizeof(void*),0)));}_LL1F8:;}{
# 2302
struct _tuple1*_tmp4BB=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4BC=Cyc_Absyn_var_exp(_tmp4BB,0);
struct Cyc_Absyn_Exp*_tmp4BD=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp4BE=Cyc_Absyn_aggrarrow_exp(_tmp4BC,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp4BF=Cyc_Absyn_neq_exp(_tmp4BE,_tmp4BD,0);
struct Cyc_Absyn_Exp*_tmp4C0=Cyc_Absyn_aggrarrow_exp(_tmp4BC,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp4C1=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp4C0,0),0);
struct Cyc_Absyn_Stmt*_tmp4C2=Cyc_Absyn_ifthenelse_stmt(_tmp4BF,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
void*_tmp4C3=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp4C4=Cyc_Absyn_address_exp(aggrproj(e1,f,0),0);
struct Cyc_Absyn_Stmt*_tmp4C5=Cyc_Absyn_declare_stmt(_tmp4BB,_tmp4C3,_tmp4C4,Cyc_Absyn_seq_stmt(_tmp4C2,_tmp4C1,0),0);
return Cyc_Toc_stmt_exp_r(_tmp4C5);}else{
# 2316
struct Cyc_Absyn_Exp*_tmp4C6=Cyc_Toc_member_exp(aggrproj(_tmp4BC,f,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp4C7=Cyc_Absyn_neq_exp(_tmp4C6,_tmp4BD,0);
struct Cyc_Absyn_Exp*_tmp4C8=Cyc_Toc_member_exp(aggrproj(_tmp4BC,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp4C9=Cyc_Absyn_exp_stmt(_tmp4C8,0);
struct Cyc_Absyn_Stmt*_tmp4CA=Cyc_Absyn_ifthenelse_stmt(_tmp4C7,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*_tmp4CB=Cyc_Absyn_declare_stmt(_tmp4BB,e1_c_type,e1,Cyc_Absyn_seq_stmt(_tmp4CA,_tmp4C9,0),0);
return Cyc_Toc_stmt_exp_r(_tmp4CB);}};}
# 2326
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _dyneither_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 2329
void*_tmp4CC=Cyc_Tcutil_compress(t);void*_tmp4CD=_tmp4CC;union Cyc_Absyn_AggrInfoU _tmp4CF;_LL1FE: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4CE=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4CD;if(_tmp4CE->tag != 11)goto _LL200;else{_tmp4CF=(_tmp4CE->f1).aggr_info;}}_LL1FF: {
# 2331
struct Cyc_Absyn_Aggrdecl*_tmp4D0=Cyc_Absyn_get_known_aggrdecl(_tmp4CF);
*f_tag=Cyc_Toc_get_member_offset(_tmp4D0,f);{
# 2334
const char*_tmpC3A;void*_tmpC39[2];struct Cyc_String_pa_PrintArg_struct _tmpC38;struct Cyc_String_pa_PrintArg_struct _tmpC37;struct _dyneither_ptr str=(struct _dyneither_ptr)((_tmpC37.tag=0,((_tmpC37.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpC38.tag=0,((_tmpC38.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp4D0->name).f2),((_tmpC39[0]=& _tmpC38,((_tmpC39[1]=& _tmpC37,Cyc_aprintf(((_tmpC3A="_union_%s_%s",_tag_dyneither(_tmpC3A,sizeof(char),13))),_tag_dyneither(_tmpC39,sizeof(void*),2))))))))))))));
{struct _dyneither_ptr*_tmpC3B;*tagged_member_type=Cyc_Absyn_strct(((_tmpC3B=_cycalloc(sizeof(*_tmpC3B)),((_tmpC3B[0]=str,_tmpC3B)))));}
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4D0->impl))->tagged;};}_LL200:;_LL201:
 return 0;_LL1FD:;}
# 2345
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 2348
void*_tmp4D6=e->r;void*_tmp4D7=_tmp4D6;struct Cyc_Absyn_Exp*_tmp4D9;struct Cyc_Absyn_Exp*_tmp4DB;struct _dyneither_ptr*_tmp4DC;int*_tmp4DD;struct Cyc_Absyn_Exp*_tmp4DF;struct _dyneither_ptr*_tmp4E0;int*_tmp4E1;_LL203: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4D8=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp4D8->tag != 13)goto _LL205;else{_tmp4D9=_tmp4D8->f2;}}_LL204: {
const char*_tmpC3E;void*_tmpC3D;(_tmpC3D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC3E="cast on lhs!",_tag_dyneither(_tmpC3E,sizeof(char),13))),_tag_dyneither(_tmpC3D,sizeof(void*),0)));}_LL205: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp4DA=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp4DA->tag != 20)goto _LL207;else{_tmp4DB=_tmp4DA->f1;_tmp4DC=_tmp4DA->f2;_tmp4DD=(int*)& _tmp4DA->f4;}}_LL206:
# 2351
 return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(_tmp4DB->topt),_tmp4DC,f_tag,tagged_member_type,clear_read,_tmp4DD);_LL207: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp4DE=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp4DE->tag != 21)goto _LL209;else{_tmp4DF=_tmp4DE->f1;_tmp4E0=_tmp4DE->f2;_tmp4E1=(int*)& _tmp4DE->f4;}}_LL208: {
# 2354
void*_tmp4E4=Cyc_Tcutil_compress((void*)_check_null(_tmp4DF->topt));void*_tmp4E5=_tmp4E4;void*_tmp4E7;_LL20C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4E6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E5;if(_tmp4E6->tag != 5)goto _LL20E;else{_tmp4E7=(_tmp4E6->f1).elt_typ;}}_LL20D:
# 2356
 return Cyc_Toc_is_tagged_union_project_impl(_tmp4E7,_tmp4E0,f_tag,tagged_member_type,clear_read,_tmp4E1);_LL20E:;_LL20F:
# 2358
 return 0;_LL20B:;}_LL209:;_LL20A:
# 2360
 return 0;_LL202:;}
# 2372 "toc.cyc"
void Cyc_Toc_add_tagged_union_check_for_swap(struct Cyc_Absyn_Exp*e,int tag,void*mem_type){
struct _tuple1*_tmp4E8=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp4E8,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_val,0),0);
struct Cyc_Absyn_Exp*_tmp4E9=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_ifthenelse_stmt(_tmp4E9,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp4E8,Cyc_Absyn_cstar_typ(mem_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(e)),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
e->r=Cyc_Toc_stmt_exp_r(s1);}
# 2396 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 2400
struct _tuple1*_tmp4EA=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp4EA,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{
# 2410
struct Cyc_Absyn_Exp*_tmp4EB=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
s2=Cyc_Absyn_ifthenelse_stmt(_tmp4EB,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{
# 2413
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp4EA,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(e1),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple28{struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};
# 2419
static void Cyc_Toc_new_comprehension_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,void*new_e_type,struct Cyc_Absyn_Exp*rgnopt,void*old_typ,struct Cyc_Absyn_Exp*e1,void*t1,int zero_term,struct Cyc_Absyn_Exp*init_exp,struct Cyc_Absyn_Vardecl*vd){
# 2434 "toc.cyc"
struct _tuple1*max=Cyc_Toc_temp_var();
struct _tuple1*a=Cyc_Toc_temp_var();
void*old_elt_typ=t1;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c(nv,e1);{
struct Cyc_Absyn_Exp*_tmp4EC=Cyc_Absyn_var_exp(max,0);
# 2442
struct Cyc_Absyn_Stmt*s;
if(zero_term)
_tmp4EC=Cyc_Absyn_add_exp(_tmp4EC,Cyc_Absyn_uint_exp(1,0),0);
if(init_exp != 0)
s=Cyc_Toc_init_comprehension(nv,new_e_type,
Cyc_Absyn_var_exp(a,0),(struct Cyc_Absyn_Vardecl*)_check_null(vd),Cyc_Absyn_var_exp(max,0),init_exp,zero_term,
Cyc_Toc_skip_stmt_dl(),1);else{
if(zero_term)
s=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(a,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(elt_typ,Cyc_Absyn_uint_exp(0,0)),0),0);else{
# 2453
s=Cyc_Absyn_skip_stmt(0);}}{
# 2455
struct _RegionHandle _tmp4ED=_new_region("r");struct _RegionHandle*r=& _tmp4ED;_push_region(r);
{struct _tuple28*_tmpC41;struct Cyc_List_List*_tmpC40;struct Cyc_List_List*decls=
(_tmpC40=_region_malloc(r,sizeof(*_tmpC40)),((_tmpC40->hd=((_tmpC41=_region_malloc(r,sizeof(*_tmpC41)),((_tmpC41->f1=max,((_tmpC41->f2=Cyc_Absyn_uint_typ,((_tmpC41->f3=e1,_tmpC41)))))))),((_tmpC40->tl=0,_tmpC40)))));
struct Cyc_Absyn_Exp*ai;
if(rgnopt == 0  || Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*_tmpC42[2];ai=Cyc_Toc_malloc_exp(old_elt_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpC42[1]=_tmp4EC,((_tmpC42[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC42,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));}else{
# 2465
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);{
struct Cyc_Absyn_Exp*_tmpC43[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpC43[1]=_tmp4EC,((_tmpC43[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC43,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{
# 2471
struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);
{struct _tuple28*_tmpC46;struct Cyc_List_List*_tmpC45;decls=((_tmpC45=_region_malloc(r,sizeof(*_tmpC45)),((_tmpC45->hd=((_tmpC46=_region_malloc(r,sizeof(*_tmpC46)),((_tmpC46->f1=a,((_tmpC46->f2=ptr_typ,((_tmpC46->f3=ainit,_tmpC46)))))))),((_tmpC45->tl=decls,_tmpC45))))));}
if(!Cyc_Tcutil_is_pointer_type(old_typ)){
const char*_tmpC49;void*_tmpC48;(_tmpC48=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC49="new_comprehension_to_c: comprehension is not an array type",_tag_dyneither(_tmpC49,sizeof(char),59))),_tag_dyneither(_tmpC48,sizeof(void*),0)));}
if(Cyc_Tcutil_is_dyneither_ptr(old_typ)){
struct _tuple1*_tmp4F4=Cyc_Toc_temp_var();
void*_tmp4F5=Cyc_Toc_typ_to_c(old_typ);
struct Cyc_Absyn_Exp*_tmp4F6=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpC4A[3];struct Cyc_Absyn_Exp*_tmp4F7=
Cyc_Absyn_fncall_exp(_tmp4F6,(
(_tmpC4A[2]=_tmp4EC,((_tmpC4A[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((_tmpC4A[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC4A,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 2483
{struct _tuple28*_tmpC4D;struct Cyc_List_List*_tmpC4C;decls=((_tmpC4C=_region_malloc(r,sizeof(*_tmpC4C)),((_tmpC4C->hd=((_tmpC4D=_region_malloc(r,sizeof(*_tmpC4D)),((_tmpC4D->f1=_tmp4F4,((_tmpC4D->f2=_tmp4F5,((_tmpC4D->f3=_tmp4F7,_tmpC4D)))))))),((_tmpC4C->tl=decls,_tmpC4C))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp4F4,0),0),0);}else{
# 2486
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}
{struct Cyc_List_List*_tmp4FB=decls;for(0;_tmp4FB != 0;_tmp4FB=_tmp4FB->tl){
struct _tuple28 _tmp4FC=*((struct _tuple28*)_tmp4FB->hd);struct _tuple1*_tmp4FE;void*_tmp4FF;struct Cyc_Absyn_Exp*_tmp500;struct _tuple28 _tmp4FD=_tmp4FC;_tmp4FE=_tmp4FD.f1;_tmp4FF=_tmp4FD.f2;_tmp500=_tmp4FD.f3;
s=Cyc_Absyn_declare_stmt(_tmp4FE,_tmp4FF,_tmp500,s,0);}}
# 2491
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2456
;_pop_region(r);};};}struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static void _tmpC9C(unsigned int*_tmpC9B,unsigned int*_tmpC9A,struct _tuple1***_tmpC98){for(*_tmpC9B=0;*_tmpC9B < *_tmpC9A;(*_tmpC9B)++){(*_tmpC98)[*_tmpC9B]=(struct _tuple1*)
# 2866 "toc.cyc"
Cyc_Toc_temp_var();}}
# 2495 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
void*_tmp503=e->r;
if(e->topt == 0){
const char*_tmpC51;void*_tmpC50[1];struct Cyc_String_pa_PrintArg_struct _tmpC4F;(_tmpC4F.tag=0,((_tmpC4F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC50[0]=& _tmpC4F,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC51="exp_to_c: no type for %s",_tag_dyneither(_tmpC51,sizeof(char),25))),_tag_dyneither(_tmpC50,sizeof(void*),1)))))));}
# 2501
if((nv->struct_info).lhs_exp != 0){
void*_tmp507=_tmp503;_LL211: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp508=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp507;if(_tmp508->tag != 28)goto _LL213;}_LL212:
 goto _LL210;_LL213:;_LL214:
 nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);_LL210:;}{
# 2509
void*old_typ=(void*)_check_null(e->topt);
void*_tmp509=_tmp503;struct _tuple1*_tmp50D;void*_tmp50E;enum Cyc_Absyn_Primop _tmp510;struct Cyc_List_List*_tmp511;struct Cyc_Absyn_Exp*_tmp513;enum Cyc_Absyn_Incrementor _tmp514;struct Cyc_Absyn_Exp*_tmp516;struct Cyc_Core_Opt*_tmp517;struct Cyc_Absyn_Exp*_tmp518;struct Cyc_Absyn_Exp*_tmp51A;struct Cyc_Absyn_Exp*_tmp51B;struct Cyc_Absyn_Exp*_tmp51C;struct Cyc_Absyn_Exp*_tmp51E;struct Cyc_Absyn_Exp*_tmp51F;struct Cyc_Absyn_Exp*_tmp521;struct Cyc_Absyn_Exp*_tmp522;struct Cyc_Absyn_Exp*_tmp524;struct Cyc_Absyn_Exp*_tmp525;struct Cyc_Absyn_Exp*_tmp527;struct Cyc_List_List*_tmp528;struct Cyc_Absyn_Exp*_tmp52A;struct Cyc_List_List*_tmp52B;int _tmp52C;struct Cyc_List_List*_tmp52D;struct Cyc_Absyn_VarargInfo*_tmp52E;struct Cyc_Absyn_Exp*_tmp530;int _tmp531;struct Cyc_Absyn_Exp*_tmp533;struct Cyc_Absyn_Exp*_tmp535;struct Cyc_List_List*_tmp536;void**_tmp538;struct Cyc_Absyn_Exp*_tmp539;int _tmp53A;enum Cyc_Absyn_Coercion _tmp53B;struct Cyc_Absyn_Exp*_tmp53D;struct Cyc_Absyn_Exp*_tmp53F;struct Cyc_Absyn_Exp*_tmp540;struct Cyc_Absyn_Exp*_tmp542;void*_tmp544;void*_tmp546;struct Cyc_List_List*_tmp547;struct Cyc_Absyn_Exp*_tmp549;struct Cyc_Absyn_Exp*_tmp54B;struct _dyneither_ptr*_tmp54C;int _tmp54D;int _tmp54E;struct Cyc_Absyn_Exp*_tmp550;struct _dyneither_ptr*_tmp551;int _tmp552;int _tmp553;struct Cyc_Absyn_Exp*_tmp555;struct Cyc_Absyn_Exp*_tmp556;struct Cyc_List_List*_tmp558;struct Cyc_List_List*_tmp55A;struct Cyc_Absyn_Vardecl*_tmp55C;struct Cyc_Absyn_Exp*_tmp55D;struct Cyc_Absyn_Exp*_tmp55E;int _tmp55F;struct Cyc_Absyn_Exp*_tmp561;void*_tmp562;int _tmp563;struct _tuple1*_tmp565;struct Cyc_List_List*_tmp566;struct Cyc_List_List*_tmp567;struct Cyc_Absyn_Aggrdecl*_tmp568;void*_tmp56A;struct Cyc_List_List*_tmp56B;struct Cyc_List_List*_tmp56D;struct Cyc_Absyn_Datatypedecl*_tmp56E;struct Cyc_Absyn_Datatypefield*_tmp56F;int _tmp573;struct Cyc_Absyn_Exp*_tmp574;void**_tmp575;struct Cyc_Absyn_Exp*_tmp576;int _tmp577;struct Cyc_Absyn_Exp*_tmp579;struct Cyc_Absyn_Exp*_tmp57A;struct Cyc_Absyn_Exp*_tmp57C;struct _dyneither_ptr*_tmp57D;struct Cyc_Absyn_Stmt*_tmp57F;_LL216: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp50A=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp50A->tag != 0)goto _LL218;else{if(((_tmp50A->f1).Null_c).tag != 1)goto _LL218;}}_LL217: {
# 2516
struct Cyc_Absyn_Exp*_tmp584=Cyc_Absyn_uint_exp(0,0);
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp584,_tmp584))->r;else{
# 2521
struct Cyc_Absyn_Exp*_tmpC52[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((_tmpC52[2]=_tmp584,((_tmpC52[1]=_tmp584,((_tmpC52[0]=_tmp584,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC52,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}else{
# 2523
e->r=(void*)& Cyc_Toc_zero_exp;}
# 2525
goto _LL215;}_LL218: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp50B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp50B->tag != 0)goto _LL21A;}_LL219:
 goto _LL215;_LL21A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp50C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp50C->tag != 1)goto _LL21C;else{_tmp50D=_tmp50C->f1;_tmp50E=(void*)_tmp50C->f2;}}_LL21B:
# 2528
{struct _handler_cons _tmp586;_push_handler(& _tmp586);{int _tmp588=0;if(setjmp(_tmp586.handler))_tmp588=1;if(!_tmp588){e->r=(Cyc_Toc_lookup_varmap(nv,_tmp50D))->r;;_pop_handler();}else{void*_tmp587=(void*)_exn_thrown;void*_tmp58A=_tmp587;void*_tmp58C;_LL26B: {struct Cyc_Dict_Absent_exn_struct*_tmp58B=(struct Cyc_Dict_Absent_exn_struct*)_tmp58A;if(_tmp58B->tag != Cyc_Dict_Absent)goto _LL26D;}_LL26C: {
# 2530
const char*_tmpC56;void*_tmpC55[1];struct Cyc_String_pa_PrintArg_struct _tmpC54;(_tmpC54.tag=0,((_tmpC54.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp50D)),((_tmpC55[0]=& _tmpC54,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC56="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpC56,sizeof(char),32))),_tag_dyneither(_tmpC55,sizeof(void*),1)))))));}_LL26D: _tmp58C=_tmp58A;_LL26E:(void)_rethrow(_tmp58C);_LL26A:;}};}
# 2532
goto _LL215;_LL21C: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp50F=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp50F->tag != 2)goto _LL21E;else{_tmp510=_tmp50F->f1;_tmp511=_tmp50F->f2;}}_LL21D: {
# 2535
struct Cyc_List_List*_tmp590=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp511);
# 2537
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp511);
switch(_tmp510){case Cyc_Absyn_Numelts: _LL26F: {
# 2540
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp511))->hd;
{void*_tmp591=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_tmp592=_tmp591;struct Cyc_Absyn_Exp*_tmp594;void*_tmp596;union Cyc_Absyn_Constraint*_tmp597;union Cyc_Absyn_Constraint*_tmp598;union Cyc_Absyn_Constraint*_tmp599;_LL272: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp593=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp592;if(_tmp593->tag != 8)goto _LL274;else{_tmp594=(_tmp593->f1).num_elts;}}_LL273:
# 2544
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp594))){
const char*_tmpC59;void*_tmpC58;(_tmpC58=0,Cyc_Tcutil_terr(e->loc,((_tmpC59="can't calculate numelts",_tag_dyneither(_tmpC59,sizeof(char),24))),_tag_dyneither(_tmpC58,sizeof(void*),0)));}
e->r=_tmp594->r;goto _LL271;_LL274: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp595=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp592;if(_tmp595->tag != 5)goto _LL276;else{_tmp596=(_tmp595->f1).elt_typ;_tmp597=((_tmp595->f1).ptr_atts).nullable;_tmp598=((_tmp595->f1).ptr_atts).bounds;_tmp599=((_tmp595->f1).ptr_atts).zero_term;}}_LL275:
# 2548
{void*_tmp59C=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp598);void*_tmp59D=_tmp59C;struct Cyc_Absyn_Exp*_tmp5A0;_LL279: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp59E=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp59D;if(_tmp59E->tag != 0)goto _LL27B;}_LL27A:
# 2550
{struct Cyc_Absyn_Exp*_tmpC5A[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,(
(_tmpC5A[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp596),0),((_tmpC5A[0]=(struct Cyc_Absyn_Exp*)_tmp511->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC5A,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL278;_LL27B: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp59F=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp59D;if(_tmp59F->tag != 1)goto _LL278;else{_tmp5A0=_tmp59F->f1;}}_LL27C:
# 2556
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp599)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp511->hd);
# 2560
struct Cyc_Absyn_Exp*_tmpC5B[2];e->r=Cyc_Toc_fncall_exp_r(function_e,((_tmpC5B[1]=_tmp5A0,((_tmpC5B[0]=(struct Cyc_Absyn_Exp*)_tmp511->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC5B,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp597)){
if(!Cyc_Evexp_c_can_eval(_tmp5A0)){
const char*_tmpC5E;void*_tmpC5D;(_tmpC5D=0,Cyc_Tcutil_terr(e->loc,((_tmpC5E="can't calculate numelts",_tag_dyneither(_tmpC5E,sizeof(char),24))),_tag_dyneither(_tmpC5D,sizeof(void*),0)));}
# 2565
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp5A0,Cyc_Absyn_uint_exp(0,0));}else{
# 2567
e->r=_tmp5A0->r;goto _LL278;}}
# 2569
goto _LL278;_LL278:;}
# 2571
goto _LL271;_LL276:;_LL277: {
# 2573
const char*_tmpC63;void*_tmpC62[2];struct Cyc_String_pa_PrintArg_struct _tmpC61;struct Cyc_String_pa_PrintArg_struct _tmpC60;(_tmpC60.tag=0,((_tmpC60.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpC61.tag=0,((_tmpC61.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpC62[0]=& _tmpC61,((_tmpC62[1]=& _tmpC60,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC63="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpC63,sizeof(char),41))),_tag_dyneither(_tmpC62,sizeof(void*),2)))))))))))));}_LL271:;}
# 2576
break;}case Cyc_Absyn_Plus: _LL270:
# 2581
{void*_tmp5A9=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp590))->hd);void*_tmp5AA=_tmp5A9;void*_tmp5AC;union Cyc_Absyn_Constraint*_tmp5AD;union Cyc_Absyn_Constraint*_tmp5AE;_LL27F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5AB=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5AA;if(_tmp5AB->tag != 5)goto _LL281;else{_tmp5AC=(_tmp5AB->f1).elt_typ;_tmp5AD=((_tmp5AB->f1).ptr_atts).bounds;_tmp5AE=((_tmp5AB->f1).ptr_atts).zero_term;}}_LL280:
# 2583
{void*_tmp5AF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5AD);void*_tmp5B0=_tmp5AF;struct Cyc_Absyn_Exp*_tmp5B3;_LL284: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5B1=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5B0;if(_tmp5B1->tag != 0)goto _LL286;}_LL285: {
# 2585
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp511))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp511->tl))->hd;
{struct Cyc_Absyn_Exp*_tmpC64[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpC64[2]=e2,((_tmpC64[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5AC),0),((_tmpC64[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC64,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}
goto _LL283;}_LL286: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5B2=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5B0;if(_tmp5B2->tag != 1)goto _LL283;else{_tmp5B3=_tmp5B2->f1;}}_LL287:
# 2591
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5AE)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp511))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp511->tl))->hd;
struct Cyc_Absyn_Exp*_tmpC65[3];e->r=(Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1),((_tmpC65[2]=e2,((_tmpC65[1]=_tmp5B3,((_tmpC65[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC65,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0))->r;}
# 2596
goto _LL283;_LL283:;}
# 2598
goto _LL27E;_LL281:;_LL282:
# 2600
 goto _LL27E;_LL27E:;}
# 2602
break;case Cyc_Absyn_Minus: _LL27D: {
# 2607
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(_tmp590))->hd,& elt_typ)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp511))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp511->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(_tmp590->tl))->hd)){
e1->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);
e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp);
e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{
# 2617
struct Cyc_Absyn_Exp*_tmpC66[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpC66[2]=
Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0),((_tmpC66[1]=
# 2618
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpC66[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC66,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 2622
break;}case Cyc_Absyn_Eq: _LL288:
 goto _LL289;case Cyc_Absyn_Neq: _LL289:
 goto _LL28A;case Cyc_Absyn_Gt: _LL28A:
 goto _LL28B;case Cyc_Absyn_Gte: _LL28B:
 goto _LL28C;case Cyc_Absyn_Lt: _LL28C:
 goto _LL28D;case Cyc_Absyn_Lte: _LL28D: {
# 2630
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp511))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp511->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp590))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp590->tl))->hd;
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& elt_typ))
e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,0));
if(Cyc_Tcutil_is_tagged_pointer_typ(t2))
e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp,0));
break;}default: _LL28E:
 break;}
# 2642
goto _LL215;}_LL21E: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp512=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp512->tag != 4)goto _LL220;else{_tmp513=_tmp512->f1;_tmp514=_tmp512->f2;}}_LL21F: {
# 2644
void*e2_cyc_typ=(void*)_check_null(_tmp513->topt);
# 2653 "toc.cyc"
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
const char*_tmpC67;struct _dyneither_ptr incr_str=(_tmpC67="increment",_tag_dyneither(_tmpC67,sizeof(char),10));
if(_tmp514 == Cyc_Absyn_PreDec  || _tmp514 == Cyc_Absyn_PostDec){const char*_tmpC68;incr_str=((_tmpC68="decrement",_tag_dyneither(_tmpC68,sizeof(char),10)));}
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp513,& ptr_type,& is_dyneither,& elt_type)){
{const char*_tmpC6C;void*_tmpC6B[1];struct Cyc_String_pa_PrintArg_struct _tmpC6A;(_tmpC6A.tag=0,((_tmpC6A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC6B[0]=& _tmpC6A,Cyc_Tcutil_terr(e->loc,((_tmpC6C="in-place %s is not supported when dereferencing a zero-terminated pointer",_tag_dyneither(_tmpC6C,sizeof(char),74))),_tag_dyneither(_tmpC6B,sizeof(void*),1)))))));}{
const char*_tmpC6F;void*_tmpC6E;(_tmpC6E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC6F="in-place inc/dec on zero-term",_tag_dyneither(_tmpC6F,sizeof(char),30))),_tag_dyneither(_tmpC6E,sizeof(void*),0)));};}{
# 2662
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
int f_tag=0;
if(Cyc_Toc_is_tagged_union_project(_tmp513,& f_tag,& tunion_member_type,1)){
struct Cyc_Absyn_Exp*_tmp5BD=Cyc_Absyn_signed_int_exp(1,0);
_tmp5BD->topt=Cyc_Absyn_sint_typ;
switch(_tmp514){case Cyc_Absyn_PreInc: _LL290:
# 2669
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpC75;struct Cyc_Core_Opt*_tmpC74;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpC73;e->r=(void*)((_tmpC73=_cycalloc(sizeof(*_tmpC73)),((_tmpC73[0]=((_tmpC75.tag=3,((_tmpC75.f1=_tmp513,((_tmpC75.f2=((_tmpC74=_cycalloc_atomic(sizeof(*_tmpC74)),((_tmpC74->v=(void*)Cyc_Absyn_Plus,_tmpC74)))),((_tmpC75.f3=_tmp5BD,_tmpC75)))))))),_tmpC73))));}
Cyc_Toc_exp_to_c(nv,e);
return;case Cyc_Absyn_PreDec: _LL291:
# 2673
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpC7B;struct Cyc_Core_Opt*_tmpC7A;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpC79;e->r=(void*)((_tmpC79=_cycalloc(sizeof(*_tmpC79)),((_tmpC79[0]=((_tmpC7B.tag=3,((_tmpC7B.f1=_tmp513,((_tmpC7B.f2=((_tmpC7A=_cycalloc_atomic(sizeof(*_tmpC7A)),((_tmpC7A->v=(void*)Cyc_Absyn_Minus,_tmpC7A)))),((_tmpC7B.f3=_tmp5BD,_tmpC7B)))))))),_tmpC79))));}
Cyc_Toc_exp_to_c(nv,e);
return;default: _LL292:
# 2677
{const char*_tmpC7F;void*_tmpC7E[1];struct Cyc_String_pa_PrintArg_struct _tmpC7D;(_tmpC7D.tag=0,((_tmpC7D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC7E[0]=& _tmpC7D,Cyc_Tcutil_terr(e->loc,((_tmpC7F="in-place post-%s is not supported on @tagged union members",_tag_dyneither(_tmpC7F,sizeof(char),59))),_tag_dyneither(_tmpC7E,sizeof(void*),1)))))));}{
# 2679
const char*_tmpC82;void*_tmpC81;(_tmpC81=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC82="in-place inc/dec on @tagged union",_tag_dyneither(_tmpC82,sizeof(char),34))),_tag_dyneither(_tmpC81,sizeof(void*),0)));};}}
# 2682
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp513);
Cyc_Toc_set_lhs(nv,0);{
# 2687
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp514 == Cyc_Absyn_PostInc  || _tmp514 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2693
if(_tmp514 == Cyc_Absyn_PreDec  || _tmp514 == Cyc_Absyn_PostDec)
change=-1;{
struct Cyc_Absyn_Exp*_tmpC83[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpC83[2]=
# 2697
Cyc_Absyn_signed_int_exp(change,0),((_tmpC83[1]=
# 2696
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpC83[0]=
# 2695
Cyc_Toc_push_address_exp(_tmp513),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC83,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2698
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2702
struct Cyc_Toc_functionSet*_tmp5CA=_tmp514 != Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
# 2704
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp5CA,_tmp513);
struct Cyc_Absyn_Exp*_tmpC84[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpC84[1]=
Cyc_Absyn_signed_int_exp(1,0),((_tmpC84[0]=
# 2705
Cyc_Toc_push_address_exp(_tmp513),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC84,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
# 2707
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp513)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp513,0,Cyc_Toc_incr_lvalue,_tmp514);
e->r=_tmp513->r;}}}
# 2711
goto _LL215;};};}_LL220: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp515=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp515->tag != 3)goto _LL222;else{_tmp516=_tmp515->f1;_tmp517=_tmp515->f2;_tmp518=_tmp515->f3;}}_LL221: {
# 2730 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp516->topt);
void*e2_old_typ=(void*)_check_null(_tmp518->topt);
int f_tag=0;
void*tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Toc_is_tagged_union_project(_tmp516,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp516);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp518);
e->r=Cyc_Toc_tagged_union_assignop(_tmp516,e1_old_typ,_tmp517,_tmp518,e2_old_typ,f_tag,tagged_member_struct_type);
# 2741
return;}{
# 2743
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp516,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp516,_tmp517,_tmp518,ptr_type,is_dyneither,elt_type);
# 2749
return;}{
# 2753
int e1_poly=Cyc_Toc_is_poly_project(_tmp516);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp516);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp518);{
# 2759
int done=0;
if(_tmp517 != 0){
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp5CD=(enum Cyc_Absyn_Primop)_tmp517->v;switch(_tmp5CD){case Cyc_Absyn_Plus: _LL294:
 change=_tmp518;break;case Cyc_Absyn_Minus: _LL295:
# 2767
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp518,0);break;default: _LL296: {
const char*_tmpC87;void*_tmpC86;(_tmpC86=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC87="bad t ? pointer arithmetic",_tag_dyneither(_tmpC87,sizeof(char),27))),_tag_dyneither(_tmpC86,sizeof(void*),0)));}}}
# 2770
done=1;{
# 2772
struct Cyc_Absyn_Exp*_tmp5D0=Cyc_Toc__dyneither_ptr_inplace_plus_e;
struct Cyc_Absyn_Exp*_tmpC88[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5D0,((_tmpC88[2]=change,((_tmpC88[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpC88[0]=
# 2773
Cyc_Toc_push_address_exp(_tmp516),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC88,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2776
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2779
enum Cyc_Absyn_Primop _tmp5D2=(enum Cyc_Absyn_Primop)_tmp517->v;switch(_tmp5D2){case Cyc_Absyn_Plus: _LL298:
# 2781
 done=1;
{struct Cyc_Absyn_Exp*_tmpC89[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp516),((_tmpC89[1]=_tmp518,((_tmpC89[0]=_tmp516,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC89,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
break;default: _LL299: {
const char*_tmpC8C;void*_tmpC8B;(_tmpC8B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC8C="bad zero-terminated pointer arithmetic",_tag_dyneither(_tmpC8C,sizeof(char),39))),_tag_dyneither(_tmpC8B,sizeof(void*),0)));}}}}}
# 2788
if(!done){
# 2790
if(e1_poly)
_tmp518->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp518->r,0));
# 2796
if(!Cyc_Absyn_is_lvalue(_tmp516)){
{struct _tuple27 _tmpC8F;struct _tuple27*_tmpC8E;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple27*),struct _tuple27*f_env))Cyc_Toc_lvalue_assign)(_tmp516,0,Cyc_Toc_assignop_lvalue,((_tmpC8E=_cycalloc(sizeof(struct _tuple27)* 1),((_tmpC8E[0]=(struct _tuple27)((_tmpC8F.f1=_tmp517,((_tmpC8F.f2=_tmp518,_tmpC8F)))),_tmpC8E)))));}
e->r=_tmp516->r;}}
# 2801
goto _LL215;};};};}_LL222: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp519=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp519->tag != 5)goto _LL224;else{_tmp51A=_tmp519->f1;_tmp51B=_tmp519->f2;_tmp51C=_tmp519->f3;}}_LL223:
# 2803
 Cyc_Toc_exp_to_c(nv,_tmp51A);
Cyc_Toc_exp_to_c(nv,_tmp51B);
Cyc_Toc_exp_to_c(nv,_tmp51C);
goto _LL215;_LL224: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp51D=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp51D->tag != 6)goto _LL226;else{_tmp51E=_tmp51D->f1;_tmp51F=_tmp51D->f2;}}_LL225:
# 2808
 Cyc_Toc_exp_to_c(nv,_tmp51E);
Cyc_Toc_exp_to_c(nv,_tmp51F);
goto _LL215;_LL226: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp520=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp520->tag != 7)goto _LL228;else{_tmp521=_tmp520->f1;_tmp522=_tmp520->f2;}}_LL227:
# 2812
 Cyc_Toc_exp_to_c(nv,_tmp521);
Cyc_Toc_exp_to_c(nv,_tmp522);
goto _LL215;_LL228: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp523=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp523->tag != 8)goto _LL22A;else{_tmp524=_tmp523->f1;_tmp525=_tmp523->f2;}}_LL229:
# 2816
 Cyc_Toc_exp_to_c(nv,_tmp524);
Cyc_Toc_exp_to_c(nv,_tmp525);
goto _LL215;_LL22A: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp526=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp526->tag != 9)goto _LL22C;else{_tmp527=_tmp526->f1;_tmp528=_tmp526->f2;if(_tmp526->f3 != 0)goto _LL22C;}}_LL22B:
# 2820
 Cyc_Toc_exp_to_c(nv,_tmp527);
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp528);
goto _LL215;_LL22C: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp529=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp529->tag != 9)goto _LL22E;else{_tmp52A=_tmp529->f1;_tmp52B=_tmp529->f2;if(_tmp529->f3 == 0)goto _LL22E;_tmp52C=(_tmp529->f3)->num_varargs;_tmp52D=(_tmp529->f3)->injectors;_tmp52E=(_tmp529->f3)->vai;}}_LL22D: {
# 2832 "toc.cyc"
struct _RegionHandle _tmp5D8=_new_region("r");struct _RegionHandle*r=& _tmp5D8;_push_region(r);{
struct _tuple1*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp52C,0);
void*cva_type=Cyc_Toc_typ_to_c(_tmp52E->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_conref,0);
# 2840
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp52B);
int num_normargs=num_args - _tmp52C;
# 2844
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp52B=_tmp52B->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp52B))->hd);{
struct Cyc_List_List*_tmpC90;new_args=((_tmpC90=_cycalloc(sizeof(*_tmpC90)),((_tmpC90->hd=(struct Cyc_Absyn_Exp*)_tmp52B->hd,((_tmpC90->tl=new_args,_tmpC90))))));};}}
# 2849
{struct Cyc_Absyn_Exp*_tmpC93[3];struct Cyc_List_List*_tmpC92;new_args=((_tmpC92=_cycalloc(sizeof(*_tmpC92)),((_tmpC92->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpC93[2]=num_varargs_exp,((_tmpC93[1]=
Cyc_Absyn_sizeoftyp_exp(cva_type,0),((_tmpC93[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC93,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpC92->tl=new_args,_tmpC92))))));}
# 2854
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);
# 2856
Cyc_Toc_exp_to_c(nv,_tmp52A);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp52A,new_args,0),0);
# 2860
if(_tmp52E->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp5DC=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp52E->type));void*_tmp5DD=_tmp5DC;struct Cyc_Absyn_Datatypedecl*_tmp5DF;_LL29C: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5DE=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5DD;if(_tmp5DE->tag != 3)goto _LL29E;else{if((((_tmp5DE->f1).datatype_info).KnownDatatype).tag != 2)goto _LL29E;_tmp5DF=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp5DE->f1).datatype_info).KnownDatatype).val);}}_LL29D:
 tud=_tmp5DF;goto _LL29B;_LL29E:;_LL29F: {
const char*_tmpC96;void*_tmpC95;(_tmpC95=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC96="toc: unknown datatype in vararg with inject",_tag_dyneither(_tmpC96,sizeof(char),44))),_tag_dyneither(_tmpC95,sizeof(void*),0)));}_LL29B:;}{
# 2866
unsigned int _tmpC9B;unsigned int _tmpC9A;struct _dyneither_ptr _tmpC99;struct _tuple1**_tmpC98;unsigned int _tmpC97;struct _dyneither_ptr vs=(_tmpC97=(unsigned int)_tmp52C,((_tmpC98=(struct _tuple1**)_region_malloc(r,_check_times(sizeof(struct _tuple1*),_tmpC97)),((_tmpC99=_tag_dyneither(_tmpC98,sizeof(struct _tuple1*),_tmpC97),((((_tmpC9A=_tmpC97,_tmpC9C(& _tmpC9B,& _tmpC9A,& _tmpC98))),_tmpC99)))))));
# 2868
if(_tmp52C != 0){
# 2870
struct Cyc_List_List*_tmp5E2=0;
{int i=_tmp52C - 1;for(0;i >= 0;-- i){
struct Cyc_List_List*_tmpC9D;_tmp5E2=((_tmpC9D=_cycalloc(sizeof(*_tmpC9D)),((_tmpC9D->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0),0)),((_tmpC9D->tl=_tmp5E2,_tmpC9D))))));}}
# 2874
s=Cyc_Absyn_declare_stmt(argv,arr_type,Cyc_Absyn_unresolvedmem_exp(0,_tmp5E2,0),s,0);{
# 2877
int i=0;for(0;_tmp52B != 0;(((_tmp52B=_tmp52B->tl,_tmp52D=_tmp52D->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp52B->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0);
struct Cyc_Absyn_Datatypefield*_tmp5E4=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp52D))->hd;struct _tuple1*_tmp5E6;struct Cyc_List_List*_tmp5E7;struct Cyc_Absyn_Datatypefield*_tmp5E5=_tmp5E4;_tmp5E6=_tmp5E5->name;_tmp5E7=_tmp5E5->typs;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp5E7))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);
# 2892
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);
# 2895
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Toc_tag_sp,0),
Cyc_Toc_datatype_tag(tud,_tmp5E6),0),s,0);
# 2898
s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp5E6,tud->name)),0,s,0);};}};}else{
# 2905
struct _tuple19*_tmpC9E[3];struct Cyc_List_List*_tmp5E8=(_tmpC9E[2]=
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpC9E[1]=
# 2905
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpC9E[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC9E,sizeof(struct _tuple19*),3)))))));
# 2907
s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),Cyc_Absyn_uint_exp(0,0),s,0);}};}else{
# 2917
{int i=0;for(0;_tmp52B != 0;(_tmp52B=_tmp52B->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp52B->hd);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp52B->hd,0),s,0);}}
# 2923
s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}
# 2936 "toc.cyc"
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2938
_npop_handler(0);goto _LL215;
# 2832 "toc.cyc"
;_pop_region(r);}_LL22E: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp52F=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp52F->tag != 10)goto _LL230;else{_tmp530=_tmp52F->f1;_tmp531=_tmp52F->f2;}}_LL22F:
# 2941 "toc.cyc"
 Cyc_Toc_exp_to_c(nv,_tmp530);{
struct Cyc_Absyn_Exp*_tmp5EE=_tmp531?Cyc_Toc_newrethrow_exp(_tmp530): Cyc_Toc_newthrow_exp(_tmp530);
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),_tmp5EE,0))->r;
goto _LL215;};_LL230: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp532=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp532->tag != 11)goto _LL232;else{_tmp533=_tmp532->f1;}}_LL231:
 Cyc_Toc_exp_to_c(nv,_tmp533);goto _LL215;_LL232: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp534=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp534->tag != 12)goto _LL234;else{_tmp535=_tmp534->f1;_tmp536=_tmp534->f2;}}_LL233:
# 2947
 Cyc_Toc_exp_to_c(nv,_tmp535);
# 2956 "toc.cyc"
for(0;_tmp536 != 0;_tmp536=_tmp536->tl){
enum Cyc_Absyn_KindQual _tmp5EF=(Cyc_Tcutil_typ_kind((void*)_tmp536->hd))->kind;
if(_tmp5EF != Cyc_Absyn_EffKind  && _tmp5EF != Cyc_Absyn_RgnKind){
{void*_tmp5F0=Cyc_Tcutil_compress((void*)_tmp536->hd);void*_tmp5F1=_tmp5F0;_LL2A1: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp5F2=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp5F1;if(_tmp5F2->tag != 2)goto _LL2A3;}_LL2A2:
 goto _LL2A4;_LL2A3: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5F3=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5F1;if(_tmp5F3->tag != 3)goto _LL2A5;}_LL2A4:
 continue;_LL2A5:;_LL2A6:
# 2963
 e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp535,0))->r;
goto _LL2A0;_LL2A0:;}
# 2966
break;}}
# 2969
goto _LL215;_LL234: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp537=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp537->tag != 13)goto _LL236;else{_tmp538=(void**)((void**)& _tmp537->f1);_tmp539=_tmp537->f2;_tmp53A=_tmp537->f3;_tmp53B=_tmp537->f4;}}_LL235: {
# 2971
void*old_t2=(void*)_check_null(_tmp539->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp538;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp538=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp539);
# 2978
{struct _tuple29 _tmpC9F;struct _tuple29 _tmp5F4=(_tmpC9F.f1=Cyc_Tcutil_compress(old_t2),((_tmpC9F.f2=Cyc_Tcutil_compress(new_typ),_tmpC9F)));struct _tuple29 _tmp5F5=_tmp5F4;struct Cyc_Absyn_PtrInfo _tmp5F7;struct Cyc_Absyn_PtrInfo _tmp5F9;struct Cyc_Absyn_PtrInfo _tmp5FB;_LL2A8:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5F6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F5.f1;if(_tmp5F6->tag != 5)goto _LL2AA;else{_tmp5F7=_tmp5F6->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5F8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F5.f2;if(_tmp5F8->tag != 5)goto _LL2AA;else{_tmp5F9=_tmp5F8->f1;}};_LL2A9: {
# 2980
int _tmp5FD=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5F7.ptr_atts).nullable);
int _tmp5FE=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5F9.ptr_atts).nullable);
void*_tmp5FF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp5F7.ptr_atts).bounds);
void*_tmp600=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp5F9.ptr_atts).bounds);
int _tmp601=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5F7.ptr_atts).zero_term);
int _tmp602=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5F9.ptr_atts).zero_term);
{struct _tuple29 _tmpCA0;struct _tuple29 _tmp603=(_tmpCA0.f1=_tmp5FF,((_tmpCA0.f2=_tmp600,_tmpCA0)));struct _tuple29 _tmp604=_tmp603;struct Cyc_Absyn_Exp*_tmp606;struct Cyc_Absyn_Exp*_tmp608;struct Cyc_Absyn_Exp*_tmp60A;struct Cyc_Absyn_Exp*_tmp60E;_LL2AF:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp605=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp604.f1;if(_tmp605->tag != 1)goto _LL2B1;else{_tmp606=_tmp605->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp607=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp604.f2;if(_tmp607->tag != 1)goto _LL2B1;else{_tmp608=_tmp607->f1;}};_LL2B0:
# 2988
 if((!Cyc_Evexp_c_can_eval(_tmp606) || !Cyc_Evexp_c_can_eval(_tmp608)) && !
Cyc_Evexp_same_const_exp(_tmp606,_tmp608)){
const char*_tmpCA3;void*_tmpCA2;(_tmpCA2=0,Cyc_Tcutil_terr(e->loc,((_tmpCA3="can't validate cast due to potential size differences",_tag_dyneither(_tmpCA3,sizeof(char),54))),_tag_dyneither(_tmpCA2,sizeof(void*),0)));}
if(_tmp5FD  && !_tmp5FE){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpCA6;void*_tmpCA5;(_tmpCA5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCA6="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpCA6,sizeof(char),44))),_tag_dyneither(_tmpCA5,sizeof(void*),0)));}
# 2997
if(_tmp53B != Cyc_Absyn_NonNull_to_Null){
const char*_tmpCAA;void*_tmpCA9[1];struct Cyc_String_pa_PrintArg_struct _tmpCA8;(_tmpCA8.tag=0,((_tmpCA8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpCA9[0]=& _tmpCA8,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCAA="null-check conversion mis-classified: %s",_tag_dyneither(_tmpCAA,sizeof(char),41))),_tag_dyneither(_tmpCA9,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp539);
if(do_null_check){
if(!_tmp53A){
const char*_tmpCAD;void*_tmpCAC;(_tmpCAC=0,Cyc_Tcutil_warn(e->loc,((_tmpCAD="inserted null check due to implicit cast from * to @ type",_tag_dyneither(_tmpCAD,sizeof(char),58))),_tag_dyneither(_tmpCAC,sizeof(void*),0)));}{
# 3004
struct Cyc_List_List*_tmpCAE;e->r=Cyc_Toc_cast_it_r(*_tmp538,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpCAE=_cycalloc(sizeof(*_tmpCAE)),((_tmpCAE->hd=_tmp539,((_tmpCAE->tl=0,_tmpCAE)))))),0));};}else{
# 3008
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp539->r;}};}else{
# 3013
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp539->r;}
# 3020
goto _LL2AE;_LL2B1:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp609=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp604.f1;if(_tmp609->tag != 1)goto _LL2B3;else{_tmp60A=_tmp609->f1;}}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp60B=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp604.f2;if(_tmp60B->tag != 0)goto _LL2B3;};_LL2B2:
# 3022
 if(!Cyc_Evexp_c_can_eval(_tmp60A)){
const char*_tmpCB1;void*_tmpCB0;(_tmpCB0=0,Cyc_Tcutil_terr(e->loc,((_tmpCB1="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpCB1,sizeof(char),71))),_tag_dyneither(_tmpCB0,sizeof(void*),0)));}
# 3025
if(_tmp53B == Cyc_Absyn_NonNull_to_Null){
const char*_tmpCB5;void*_tmpCB4[1];struct Cyc_String_pa_PrintArg_struct _tmpCB3;(_tmpCB3.tag=0,((_tmpCB3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpCB4[0]=& _tmpCB3,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCB5="conversion mis-classified as null-check: %s",_tag_dyneither(_tmpCB5,sizeof(char),44))),_tag_dyneither(_tmpCB4,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){
# 3029
if((_tmp601  && !(_tmp5F9.elt_tq).real_const) && !_tmp602)
# 3032
_tmp60A=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp60A,Cyc_Absyn_uint_exp(1,0),0);
# 3034
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp60A,_tmp539))->r;}else{
# 3036
struct Cyc_Absyn_Exp*_tmp620=Cyc_Toc__tag_dyneither_e;
# 3038
if(_tmp601){
# 3043
struct _tuple1*_tmp621=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp622=Cyc_Absyn_var_exp(_tmp621,0);
struct Cyc_Absyn_Exp*arg3;
# 3048
{void*_tmp623=_tmp539->r;void*_tmp624=_tmp623;_LL2B8: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp625=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp624;if(_tmp625->tag != 0)goto _LL2BA;else{if(((_tmp625->f1).String_c).tag != 8)goto _LL2BA;}}_LL2B9:
# 3050
 arg3=_tmp60A;goto _LL2B7;_LL2BA: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp626=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp624;if(_tmp626->tag != 0)goto _LL2BC;else{if(((_tmp626->f1).Wstring_c).tag != 9)goto _LL2BC;}}_LL2BB:
# 3052
 arg3=_tmp60A;goto _LL2B7;_LL2BC:;_LL2BD:
# 3054
{struct Cyc_Absyn_Exp*_tmpCB6[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp539),(
# 3056
(_tmpCB6[1]=_tmp60A,((_tmpCB6[0]=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp622),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCB6,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
goto _LL2B7;_LL2B7:;}
# 3059
if(!_tmp602  && !(_tmp5F9.elt_tq).real_const)
# 3062
arg3=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{
# 3064
struct Cyc_Absyn_Exp*_tmp628=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5F9.elt_typ),0);
struct Cyc_Absyn_Exp*_tmpCB7[3];struct Cyc_Absyn_Exp*_tmp629=Cyc_Absyn_fncall_exp(_tmp620,((_tmpCB7[2]=arg3,((_tmpCB7[1]=_tmp628,((_tmpCB7[0]=_tmp622,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCB7,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
struct Cyc_Absyn_Stmt*_tmp62A=Cyc_Absyn_exp_stmt(_tmp629,0);
_tmp62A=Cyc_Absyn_declare_stmt(_tmp621,Cyc_Toc_typ_to_c(old_t2),_tmp539,_tmp62A,0);
e->r=Cyc_Toc_stmt_exp_r(_tmp62A);};}else{
# 3071
struct Cyc_Absyn_Exp*_tmpCB8[3];e->r=Cyc_Toc_fncall_exp_r(_tmp620,(
(_tmpCB8[2]=_tmp60A,((_tmpCB8[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5F9.elt_typ),0),((_tmpCB8[0]=_tmp539,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCB8,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 3077
goto _LL2AE;_LL2B3:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp60C=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp604.f1;if(_tmp60C->tag != 0)goto _LL2B5;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp60D=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp604.f2;if(_tmp60D->tag != 1)goto _LL2B5;else{_tmp60E=_tmp60D->f1;}};_LL2B4:
# 3079
 if(!Cyc_Evexp_c_can_eval(_tmp60E)){
const char*_tmpCBB;void*_tmpCBA;(_tmpCBA=0,Cyc_Tcutil_terr(e->loc,((_tmpCBB="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpCBB,sizeof(char),71))),_tag_dyneither(_tmpCBA,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpCBE;void*_tmpCBD;(_tmpCBD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCBE="can't coerce t? to t* or t@ at the top-level",_tag_dyneither(_tmpCBE,sizeof(char),45))),_tag_dyneither(_tmpCBD,sizeof(void*),0)));}{
# 3091 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp631=_tmp60E;
if(_tmp601  && !_tmp602)
_tmp631=Cyc_Absyn_add_exp(_tmp60E,Cyc_Absyn_uint_exp(1,0),0);{
# 3098
struct Cyc_Absyn_Exp*_tmp632=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpCBF[3];struct Cyc_Absyn_Exp*_tmp633=Cyc_Absyn_fncall_exp(_tmp632,(
(_tmpCBF[2]=_tmp631,((_tmpCBF[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5F7.elt_typ),0),((_tmpCBF[0]=_tmp539,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCBF,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 3103
if(_tmp5FE){
struct Cyc_List_List*_tmpCC0;_tmp633->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,(
(_tmpCC0=_cycalloc(sizeof(*_tmpCC0)),((_tmpCC0->hd=Cyc_Absyn_copy_exp(_tmp633),((_tmpCC0->tl=0,_tmpCC0)))))));}
e->r=Cyc_Toc_cast_it_r(*_tmp538,_tmp633);
goto _LL2AE;};};_LL2B5:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp60F=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp604.f1;if(_tmp60F->tag != 0)goto _LL2AE;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp610=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp604.f2;if(_tmp610->tag != 0)goto _LL2AE;};_LL2B6:
# 3111
 DynCast:
 if((_tmp601  && !_tmp602) && !(_tmp5F9.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpCC3;void*_tmpCC2;(_tmpCC2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCC3="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",_tag_dyneither(_tmpCC3,sizeof(char),70))),_tag_dyneither(_tmpCC2,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp638=Cyc_Toc__dyneither_ptr_decrease_size_e;
struct Cyc_Absyn_Exp*_tmpCC4[3];e->r=Cyc_Toc_fncall_exp_r(_tmp638,(
(_tmpCC4[2]=
Cyc_Absyn_uint_exp(1,0),((_tmpCC4[1]=
# 3117
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5F7.elt_typ),0),((_tmpCC4[0]=_tmp539,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCC4,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}
# 3120
goto _LL2AE;_LL2AE:;}
# 3122
goto _LL2A7;}_LL2AA:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5FA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F5.f1;if(_tmp5FA->tag != 5)goto _LL2AC;else{_tmp5FB=_tmp5FA->f1;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5FC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F5.f2;if(_tmp5FC->tag != 6)goto _LL2AC;};_LL2AB:
# 3124
{void*_tmp63B=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp5FB.ptr_atts).bounds);void*_tmp63C=_tmp63B;_LL2BF: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp63D=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp63C;if(_tmp63D->tag != 0)goto _LL2C1;}_LL2C0:
# 3126
 _tmp539->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp539->r,_tmp539->loc),Cyc_Toc_curr_sp);goto _LL2BE;_LL2C1:;_LL2C2:
 goto _LL2BE;_LL2BE:;}
# 3129
goto _LL2A7;_LL2AC:;_LL2AD:
# 3131
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp539->r;
goto _LL2A7;_LL2A7:;}
# 3135
goto _LL215;}_LL236: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp53C=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp53C->tag != 14)goto _LL238;else{_tmp53D=_tmp53C->f1;}}_LL237:
# 3139
{void*_tmp63F=_tmp53D->r;void*_tmp640=_tmp63F;struct _tuple1*_tmp642;struct Cyc_List_List*_tmp643;struct Cyc_List_List*_tmp644;struct Cyc_List_List*_tmp646;_LL2C4: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp641=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp640;if(_tmp641->tag != 28)goto _LL2C6;else{_tmp642=_tmp641->f1;_tmp643=_tmp641->f2;_tmp644=_tmp641->f3;}}_LL2C5:
# 3141
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpCC8;void*_tmpCC7[1];struct Cyc_String_pa_PrintArg_struct _tmpCC6;(_tmpCC6.tag=0,((_tmpCC6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp53D->loc)),((_tmpCC7[0]=& _tmpCC6,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCC8="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpCC8,sizeof(char),42))),_tag_dyneither(_tmpCC7,sizeof(void*),1)))))));}{
struct Cyc_Absyn_Exp*_tmp64A=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp53D->topt),_tmp643,1,0,_tmp644,_tmp642);
e->r=_tmp64A->r;
e->topt=_tmp64A->topt;
goto _LL2C3;};_LL2C6: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp645=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp640;if(_tmp645->tag != 23)goto _LL2C8;else{_tmp646=_tmp645->f1;}}_LL2C7:
# 3149
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpCCC;void*_tmpCCB[1];struct Cyc_String_pa_PrintArg_struct _tmpCCA;(_tmpCCA.tag=0,((_tmpCCA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp53D->loc)),((_tmpCCB[0]=& _tmpCCA,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCCC="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpCCC,sizeof(char),42))),_tag_dyneither(_tmpCCB,sizeof(void*),1)))))));}
e->r=(Cyc_Toc_init_tuple(nv,1,0,_tmp646))->r;
goto _LL2C3;_LL2C8:;_LL2C9:
# 3155
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp53D);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp53D)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp53D,0,Cyc_Toc_address_lvalue,1);
# 3161
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp53D);}else{
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind((void*)_check_null(_tmp53D->topt))))
# 3165
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp53D);}
# 3167
goto _LL2C3;_LL2C3:;}
# 3169
goto _LL215;_LL238: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp53E=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp53E->tag != 15)goto _LL23A;else{_tmp53F=_tmp53E->f1;_tmp540=_tmp53E->f2;}}_LL239:
# 3172
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpCD0;void*_tmpCCF[1];struct Cyc_String_pa_PrintArg_struct _tmpCCE;(_tmpCCE.tag=0,((_tmpCCE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp540->loc)),((_tmpCCF[0]=& _tmpCCE,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCD0="%s: new at top-level",_tag_dyneither(_tmpCD0,sizeof(char),21))),_tag_dyneither(_tmpCCF,sizeof(void*),1)))))));}{
void*new_e_type=(void*)_check_null(_tmp540->topt);
{void*_tmp651=_tmp540->r;void*_tmp652=_tmp651;struct Cyc_List_List*_tmp654;struct Cyc_Absyn_Vardecl*_tmp656;struct Cyc_Absyn_Exp*_tmp657;struct Cyc_Absyn_Exp*_tmp658;int _tmp659;struct Cyc_Absyn_Exp*_tmp65B;void*_tmp65C;int _tmp65D;struct _tuple1*_tmp65F;struct Cyc_List_List*_tmp660;struct Cyc_List_List*_tmp661;struct Cyc_Absyn_Aggrdecl*_tmp662;struct Cyc_List_List*_tmp664;_LL2CB: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp653=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp652;if(_tmp653->tag != 25)goto _LL2CD;else{_tmp654=_tmp653->f1;}}_LL2CC: {
# 3180
struct _tuple1*_tmp665=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp666=Cyc_Absyn_var_exp(_tmp665,0);
struct Cyc_Absyn_Stmt*_tmp667=Cyc_Toc_init_array(nv,new_e_type,_tmp666,_tmp654,Cyc_Absyn_exp_stmt(_tmp666,0));
void*old_elt_typ;
{void*_tmp668=Cyc_Tcutil_compress(old_typ);void*_tmp669=_tmp668;void*_tmp66B;struct Cyc_Absyn_Tqual _tmp66C;union Cyc_Absyn_Constraint*_tmp66D;_LL2D8: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp66A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp669;if(_tmp66A->tag != 5)goto _LL2DA;else{_tmp66B=(_tmp66A->f1).elt_typ;_tmp66C=(_tmp66A->f1).elt_tq;_tmp66D=((_tmp66A->f1).ptr_atts).zero_term;}}_LL2D9:
# 3186
 old_elt_typ=_tmp66B;goto _LL2D7;_LL2DA:;_LL2DB: {
# 3188
const char*_tmpCD3;void*_tmpCD2;old_elt_typ=(
(_tmpCD2=0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCD3="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(_tmpCD3,sizeof(char),52))),_tag_dyneither(_tmpCD2,sizeof(void*),0))));}_LL2D7:;}{
# 3191
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*_tmp670=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp671=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),
Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp654),0),0);
struct Cyc_Absyn_Exp*e1;
if(_tmp53F == 0  || Cyc_Absyn_no_regions)
e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp671);else{
# 3199
struct Cyc_Absyn_Exp*r=_tmp53F;
Cyc_Toc_exp_to_c(nv,r);
e1=Cyc_Toc_rmalloc_exp(r,_tmp671);}
# 3204
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp665,_tmp670,e1,_tmp667,0));
goto _LL2CA;};}_LL2CD: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp655=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp652;if(_tmp655->tag != 26)goto _LL2CF;else{_tmp656=_tmp655->f1;_tmp657=_tmp655->f2;_tmp658=_tmp655->f3;_tmp659=_tmp655->f4;}}_LL2CE:
# 3208
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp53F,old_typ,_tmp657,(void*)_check_null(_tmp658->topt),_tmp659,_tmp658,_tmp656);
goto _LL2CA;_LL2CF: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp65A=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp652;if(_tmp65A->tag != 27)goto _LL2D1;else{_tmp65B=_tmp65A->f1;_tmp65C=(void*)_tmp65A->f2;_tmp65D=_tmp65A->f3;}}_LL2D0:
# 3212
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp53F,old_typ,_tmp65B,_tmp65C,_tmp65D,0,0);
goto _LL2CA;_LL2D1: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp65E=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp652;if(_tmp65E->tag != 28)goto _LL2D3;else{_tmp65F=_tmp65E->f1;_tmp660=_tmp65E->f2;_tmp661=_tmp65E->f3;_tmp662=_tmp65E->f4;}}_LL2D2: {
# 3217
struct Cyc_Absyn_Exp*_tmp672=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp540->topt),_tmp660,1,_tmp53F,_tmp661,_tmp65F);
e->r=_tmp672->r;
e->topt=_tmp672->topt;
goto _LL2CA;}_LL2D3: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp663=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp652;if(_tmp663->tag != 23)goto _LL2D5;else{_tmp664=_tmp663->f1;}}_LL2D4:
# 3223
 e->r=(Cyc_Toc_init_tuple(nv,1,_tmp53F,_tmp664))->r;
goto _LL2CA;_LL2D5:;_LL2D6: {
# 3230
void*old_elt_typ=(void*)_check_null(_tmp540->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
# 3233
struct _tuple1*_tmp673=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp674=Cyc_Absyn_var_exp(_tmp673,0);
struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp674,0),0);
struct Cyc_Absyn_Exp*inner_mexp=mexp;
if(_tmp53F == 0  || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
# 3240
struct Cyc_Absyn_Exp*r=_tmp53F;
Cyc_Toc_exp_to_c(nv,r);
mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{
# 3248
int done=0;
{void*_tmp675=_tmp540->r;void*_tmp676=_tmp675;void*_tmp678;struct Cyc_Absyn_Exp*_tmp679;_LL2DD: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp677=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp676;if(_tmp677->tag != 13)goto _LL2DF;else{_tmp678=(void*)_tmp677->f1;_tmp679=_tmp677->f2;}}_LL2DE:
# 3251
{struct _tuple29 _tmpCD4;struct _tuple29 _tmp67A=(_tmpCD4.f1=Cyc_Tcutil_compress(_tmp678),((_tmpCD4.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp679->topt)),_tmpCD4)));struct _tuple29 _tmp67B=_tmp67A;void*_tmp67D;union Cyc_Absyn_Constraint*_tmp67E;union Cyc_Absyn_Constraint*_tmp680;_LL2E2:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp67C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp67B.f1;if(_tmp67C->tag != 5)goto _LL2E4;else{_tmp67D=(_tmp67C->f1).elt_typ;_tmp67E=((_tmp67C->f1).ptr_atts).bounds;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp67F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp67B.f2;if(_tmp67F->tag != 5)goto _LL2E4;else{_tmp680=((_tmp67F->f1).ptr_atts).bounds;}};_LL2E3:
# 3254
{struct _tuple29 _tmpCD5;struct _tuple29 _tmp681=(_tmpCD5.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp67E),((_tmpCD5.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp680),_tmpCD5)));struct _tuple29 _tmp682=_tmp681;struct Cyc_Absyn_Exp*_tmp685;_LL2E7:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp683=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp682.f1;if(_tmp683->tag != 0)goto _LL2E9;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp684=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp682.f2;if(_tmp684->tag != 1)goto _LL2E9;else{_tmp685=_tmp684->f1;}};_LL2E8:
# 3256
 Cyc_Toc_exp_to_c(nv,_tmp679);
inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);
done=1;{
struct Cyc_Absyn_Exp*_tmp686=Cyc_Toc__init_dyneither_ptr_e;
{struct Cyc_Absyn_Exp*_tmpCD6[4];e->r=Cyc_Toc_fncall_exp_r(_tmp686,(
(_tmpCD6[3]=_tmp685,((_tmpCD6[2]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp67D),0),((_tmpCD6[1]=_tmp679,((_tmpCD6[0]=mexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCD6,sizeof(struct Cyc_Absyn_Exp*),4)))))))))));}
# 3264
goto _LL2E6;};_LL2E9:;_LL2EA:
 goto _LL2E6;_LL2E6:;}
# 3267
goto _LL2E1;_LL2E4:;_LL2E5:
 goto _LL2E1;_LL2E1:;}
# 3270
goto _LL2DC;_LL2DF:;_LL2E0:
 goto _LL2DC;_LL2DC:;}
# 3273
if(!done){
struct Cyc_Absyn_Stmt*_tmp68A=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp674),0);
struct Cyc_Absyn_Exp*_tmp68B=Cyc_Absyn_signed_int_exp(0,0);
Cyc_Toc_exp_to_c(nv,_tmp540);
_tmp68A=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp674,_tmp68B,0),_tmp540,0),_tmp68A,0);{
# 3279
void*_tmp68C=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp673,_tmp68C,mexp,_tmp68A,0));};}
# 3282
goto _LL2CA;};}_LL2CA:;}
# 3284
goto _LL215;};_LL23A: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp541=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp541->tag != 17)goto _LL23C;else{_tmp542=_tmp541->f1;}}_LL23B: {
# 3287
int _tmp68D=Cyc_Toc_set_in_sizeof(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp542);
Cyc_Toc_set_in_sizeof(nv,_tmp68D);
goto _LL215;}_LL23C: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp543=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp543->tag != 16)goto _LL23E;else{_tmp544=(void*)_tmp543->f1;}}_LL23D:
{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpCD9;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpCD8;e->r=(void*)((_tmpCD8=_cycalloc(sizeof(*_tmpCD8)),((_tmpCD8[0]=((_tmpCD9.tag=16,((_tmpCD9.f1=Cyc_Toc_typ_to_c(_tmp544),_tmpCD9)))),_tmpCD8))));}goto _LL215;_LL23E: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp545=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp545->tag != 18)goto _LL240;else{_tmp546=(void*)_tmp545->f1;_tmp547=_tmp545->f2;}}_LL23F: {
# 3293
void*_tmp690=_tmp546;
struct Cyc_List_List*_tmp691=_tmp547;
for(0;_tmp691 != 0;_tmp691=_tmp691->tl){
void*_tmp692=(void*)_tmp691->hd;void*_tmp693=_tmp692;struct _dyneither_ptr*_tmp695;unsigned int _tmp697;_LL2EC: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp694=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp693;if(_tmp694->tag != 0)goto _LL2EE;else{_tmp695=_tmp694->f1;}}_LL2ED:
 goto _LL2EB;_LL2EE: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp696=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp693;if(_tmp696->tag != 1)goto _LL2EB;else{_tmp697=_tmp696->f1;}}_LL2EF:
# 3299
{void*_tmp698=Cyc_Tcutil_compress(_tmp690);void*_tmp699=_tmp698;union Cyc_Absyn_AggrInfoU _tmp69B;struct Cyc_List_List*_tmp69D;struct Cyc_List_List*_tmp69F;struct Cyc_Absyn_Datatypefield*_tmp6A1;_LL2F1: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp69A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp699;if(_tmp69A->tag != 11)goto _LL2F3;else{_tmp69B=(_tmp69A->f1).aggr_info;}}_LL2F2: {
# 3301
struct Cyc_Absyn_Aggrdecl*_tmp6A2=Cyc_Absyn_get_known_aggrdecl(_tmp69B);
if(_tmp6A2->impl == 0){
const char*_tmpCDC;void*_tmpCDB;(_tmpCDB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCDC="struct fields must be known",_tag_dyneither(_tmpCDC,sizeof(char),28))),_tag_dyneither(_tmpCDB,sizeof(void*),0)));}
_tmp69D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6A2->impl))->fields;goto _LL2F4;}_LL2F3: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp69C=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp699;if(_tmp69C->tag != 12)goto _LL2F5;else{_tmp69D=_tmp69C->f2;}}_LL2F4: {
# 3306
struct Cyc_Absyn_Aggrfield*_tmp6A5=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp69D,(int)_tmp697);
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpCDF;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpCDE;_tmp691->hd=(void*)((void*)((_tmpCDE=_cycalloc(sizeof(*_tmpCDE)),((_tmpCDE[0]=((_tmpCDF.tag=0,((_tmpCDF.f1=_tmp6A5->name,_tmpCDF)))),_tmpCDE)))));}
_tmp690=_tmp6A5->type;
goto _LL2F0;}_LL2F5: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp69E=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp699;if(_tmp69E->tag != 10)goto _LL2F7;else{_tmp69F=_tmp69E->f1;}}_LL2F6:
# 3311
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpCE2;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpCE1;_tmp691->hd=(void*)((void*)((_tmpCE1=_cycalloc(sizeof(*_tmpCE1)),((_tmpCE1[0]=((_tmpCE2.tag=0,((_tmpCE2.f1=Cyc_Absyn_fieldname((int)(_tmp697 + 1)),_tmpCE2)))),_tmpCE1)))));}
_tmp690=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp69F,(int)_tmp697)).f2;
goto _LL2F0;_LL2F7: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp6A0=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp699;if(_tmp6A0->tag != 4)goto _LL2F9;else{if((((_tmp6A0->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2F9;_tmp6A1=((struct _tuple2)(((_tmp6A0->f1).field_info).KnownDatatypefield).val).f2;}}_LL2F8:
# 3315
 if(_tmp697 == 0){
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpCE5;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpCE4;_tmp691->hd=(void*)((void*)((_tmpCE4=_cycalloc(sizeof(*_tmpCE4)),((_tmpCE4[0]=((_tmpCE5.tag=0,((_tmpCE5.f1=Cyc_Toc_tag_sp,_tmpCE5)))),_tmpCE4)))));}else{
# 3318
_tmp690=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp6A1->typs,(int)(_tmp697 - 1))).f2;{
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpCE8;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpCE7;_tmp691->hd=(void*)((void*)((_tmpCE7=_cycalloc(sizeof(*_tmpCE7)),((_tmpCE7[0]=((_tmpCE8.tag=0,((_tmpCE8.f1=Cyc_Absyn_fieldname((int)_tmp697),_tmpCE8)))),_tmpCE7)))));};}
# 3321
goto _LL2F0;_LL2F9:;_LL2FA:
{const char*_tmpCEB;void*_tmpCEA;(_tmpCEA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCEB="impossible type for offsetof tuple index",_tag_dyneither(_tmpCEB,sizeof(char),41))),_tag_dyneither(_tmpCEA,sizeof(void*),0)));}
goto _LL2F0;_LL2F0:;}
# 3325
goto _LL2EB;_LL2EB:;}
# 3328
{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpCEE;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpCED;e->r=(void*)((_tmpCED=_cycalloc(sizeof(*_tmpCED)),((_tmpCED[0]=((_tmpCEE.tag=18,((_tmpCEE.f1=Cyc_Toc_typ_to_c(_tmp546),((_tmpCEE.f2=_tmp547,_tmpCEE)))))),_tmpCED))));}
goto _LL215;}_LL240: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp548=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp548->tag != 19)goto _LL242;else{_tmp549=_tmp548->f1;}}_LL241: {
# 3331
int _tmp6B2=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp6B3=Cyc_Tcutil_compress((void*)_check_null(_tmp549->topt));
{void*_tmp6B4=_tmp6B3;void*_tmp6B6;struct Cyc_Absyn_Tqual _tmp6B7;void*_tmp6B8;union Cyc_Absyn_Constraint*_tmp6B9;union Cyc_Absyn_Constraint*_tmp6BA;union Cyc_Absyn_Constraint*_tmp6BB;_LL2FC: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6B5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6B4;if(_tmp6B5->tag != 5)goto _LL2FE;else{_tmp6B6=(_tmp6B5->f1).elt_typ;_tmp6B7=(_tmp6B5->f1).elt_tq;_tmp6B8=((_tmp6B5->f1).ptr_atts).rgn;_tmp6B9=((_tmp6B5->f1).ptr_atts).nullable;_tmp6BA=((_tmp6B5->f1).ptr_atts).bounds;_tmp6BB=((_tmp6B5->f1).ptr_atts).zero_term;}}_LL2FD:
# 3336
{void*_tmp6BC=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6BA);void*_tmp6BD=_tmp6BC;struct Cyc_Absyn_Exp*_tmp6BF;_LL301: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6BE=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6BD;if(_tmp6BE->tag != 1)goto _LL303;else{_tmp6BF=_tmp6BE->f1;}}_LL302: {
# 3338
int do_null_check=!Cyc_Toc_in_sizeof(nv) && Cyc_Toc_need_null_check(_tmp549);
Cyc_Toc_exp_to_c(nv,_tmp549);
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpCF1;void*_tmpCF0;(_tmpCF0=0,Cyc_Tcutil_warn(e->loc,((_tmpCF1="inserted null check due to dereference",_tag_dyneither(_tmpCF1,sizeof(char),39))),_tag_dyneither(_tmpCF0,sizeof(void*),0)));}{
# 3344
struct Cyc_List_List*_tmpCF2;_tmp549->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(_tmp6B3),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpCF2=_cycalloc(sizeof(*_tmpCF2)),((_tmpCF2->hd=Cyc_Absyn_copy_exp(_tmp549),((_tmpCF2->tl=0,_tmpCF2)))))),0));};}
# 3352
if(!Cyc_Toc_in_sizeof(nv) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6BB)){
struct _tuple12 _tmp6C4=Cyc_Evexp_eval_const_uint_exp(_tmp6BF);unsigned int _tmp6C6;int _tmp6C7;struct _tuple12 _tmp6C5=_tmp6C4;_tmp6C6=_tmp6C5.f1;_tmp6C7=_tmp6C5.f2;
# 3358
if(!_tmp6C7  || _tmp6C6 <= 0){
const char*_tmpCF5;void*_tmpCF4;(_tmpCF4=0,Cyc_Tcutil_terr(e->loc,((_tmpCF5="cannot determine dereference is in bounds",_tag_dyneither(_tmpCF5,sizeof(char),42))),_tag_dyneither(_tmpCF4,sizeof(void*),0)));}}
# 3361
goto _LL300;}_LL303: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6C0=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6BD;if(_tmp6C0->tag != 0)goto _LL300;}_LL304: {
# 3364
struct Cyc_Absyn_Exp*_tmp6CA=Cyc_Absyn_uint_exp(0,0);
_tmp6CA->topt=Cyc_Absyn_uint_typ;
e->r=Cyc_Toc_subscript_exp_r(_tmp549,_tmp6CA);
Cyc_Toc_exp_to_c(nv,e);
goto _LL300;}_LL300:;}
# 3370
goto _LL2FB;_LL2FE:;_LL2FF: {
const char*_tmpCF8;void*_tmpCF7;(_tmpCF7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCF8="exp_to_c: Deref: non-pointer",_tag_dyneither(_tmpCF8,sizeof(char),29))),_tag_dyneither(_tmpCF7,sizeof(void*),0)));}_LL2FB:;}
# 3373
Cyc_Toc_set_lhs(nv,_tmp6B2);
goto _LL215;};}_LL242: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp54A=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp54A->tag != 20)goto _LL244;else{_tmp54B=_tmp54A->f1;_tmp54C=_tmp54A->f2;_tmp54D=_tmp54A->f3;_tmp54E=_tmp54A->f4;}}_LL243: {
# 3376
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp54B->topt);
Cyc_Toc_exp_to_c(nv,_tmp54B);
if(_tmp54D  && _tmp54E)
e->r=Cyc_Toc_check_tagged_union(_tmp54B,Cyc_Toc_typ_to_c(e1_cyc_type),e1_cyc_type,_tmp54C,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);
# 3384
if(is_poly)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
goto _LL215;}_LL244: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp54F=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp54F->tag != 21)goto _LL246;else{_tmp550=_tmp54F->f1;_tmp551=_tmp54F->f2;_tmp552=_tmp54F->f3;_tmp553=_tmp54F->f4;}}_LL245: {
# 3388
int _tmp6CD=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp550->topt));
int do_null_check=Cyc_Toc_need_null_check(_tmp550);
Cyc_Toc_exp_to_c(nv,_tmp550);{
int is_poly=Cyc_Toc_is_poly_project(e);
struct Cyc_Absyn_PtrInfo _tmp6CE=Cyc_Toc_get_ptr_type(e1typ);void*_tmp6D0;struct Cyc_Absyn_Tqual _tmp6D1;void*_tmp6D2;union Cyc_Absyn_Constraint*_tmp6D3;union Cyc_Absyn_Constraint*_tmp6D4;union Cyc_Absyn_Constraint*_tmp6D5;struct Cyc_Absyn_PtrInfo _tmp6CF=_tmp6CE;_tmp6D0=_tmp6CF.elt_typ;_tmp6D1=_tmp6CF.elt_tq;_tmp6D2=(_tmp6CF.ptr_atts).rgn;_tmp6D3=(_tmp6CF.ptr_atts).nullable;_tmp6D4=(_tmp6CF.ptr_atts).bounds;_tmp6D5=(_tmp6CF.ptr_atts).zero_term;
{void*_tmp6D6=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6D4);void*_tmp6D7=_tmp6D6;struct Cyc_Absyn_Exp*_tmp6D9;_LL306: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6D8=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6D7;if(_tmp6D8->tag != 1)goto _LL308;else{_tmp6D9=_tmp6D8->f1;}}_LL307: {
# 3399
struct _tuple12 _tmp6DB=Cyc_Evexp_eval_const_uint_exp(_tmp6D9);unsigned int _tmp6DD;int _tmp6DE;struct _tuple12 _tmp6DC=_tmp6DB;_tmp6DD=_tmp6DC.f1;_tmp6DE=_tmp6DC.f2;
if(_tmp6DE){
if(_tmp6DD < 1){
const char*_tmpCFB;void*_tmpCFA;(_tmpCFA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCFB="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpCFB,sizeof(char),44))),_tag_dyneither(_tmpCFA,sizeof(void*),0)));}
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpCFE;void*_tmpCFD;(_tmpCFD=0,Cyc_Tcutil_warn(e->loc,((_tmpCFE="inserted null check due to dereference",_tag_dyneither(_tmpCFE,sizeof(char),39))),_tag_dyneither(_tmpCFD,sizeof(void*),0)));}{
# 3407
struct Cyc_Absyn_Exp*_tmpCFF[1];_tmp550->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpCFF[0]=Cyc_Absyn_new_exp(_tmp550->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCFF,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}}else{
# 3412
if(!Cyc_Evexp_c_can_eval(_tmp6D9)){
const char*_tmpD02;void*_tmpD01;(_tmpD01=0,Cyc_Tcutil_terr(e->loc,((_tmpD02="cannot determine pointer dereference in bounds",_tag_dyneither(_tmpD02,sizeof(char),47))),_tag_dyneither(_tmpD01,sizeof(void*),0)));}{
# 3416
struct Cyc_Absyn_Exp*_tmpD03[4];_tmp550->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpD03[3]=
# 3420
Cyc_Absyn_uint_exp(0,0),((_tmpD03[2]=
# 3419
Cyc_Absyn_sizeoftyp_exp(_tmp6D0,0),((_tmpD03[1]=_tmp6D9,((_tmpD03[0]=
# 3418
Cyc_Absyn_new_exp(_tmp550->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD03,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0));};}
# 3422
goto _LL305;}_LL308: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6DA=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6D7;if(_tmp6DA->tag != 0)goto _LL305;}_LL309: {
# 3425
void*ta1=Cyc_Toc_typ_to_c(_tmp6D0);
{struct Cyc_Absyn_Exp*_tmpD04[3];_tmp550->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp6D1),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_dyneither_subscript_e,(
(_tmpD04[2]=
# 3430
Cyc_Absyn_uint_exp(0,0),((_tmpD04[1]=
# 3429
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpD04[0]=
# 3428
Cyc_Absyn_new_exp(_tmp550->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD04,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}
# 3433
goto _LL305;}_LL305:;}
# 3435
if(_tmp552  && _tmp553)
e->r=Cyc_Toc_check_tagged_union(_tmp550,Cyc_Toc_typ_to_c(e1typ),_tmp6D0,_tmp551,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);
# 3438
if(is_poly  && _tmp553)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
Cyc_Toc_set_lhs(nv,_tmp6CD);
goto _LL215;};};}_LL246: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp554=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp554->tag != 22)goto _LL248;else{_tmp555=_tmp554->f1;_tmp556=_tmp554->f2;}}_LL247: {
# 3443
int _tmp6E8=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp6E9=Cyc_Tcutil_compress((void*)_check_null(_tmp555->topt));
# 3447
{void*_tmp6EA=_tmp6E9;struct Cyc_List_List*_tmp6EC;void*_tmp6EE;struct Cyc_Absyn_Tqual _tmp6EF;void*_tmp6F0;union Cyc_Absyn_Constraint*_tmp6F1;union Cyc_Absyn_Constraint*_tmp6F2;union Cyc_Absyn_Constraint*_tmp6F3;_LL30B: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6EB=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6EA;if(_tmp6EB->tag != 10)goto _LL30D;else{_tmp6EC=_tmp6EB->f1;}}_LL30C:
# 3450
 Cyc_Toc_exp_to_c(nv,_tmp555);
Cyc_Toc_exp_to_c(nv,_tmp556);{
struct _tuple12 _tmp6F4=Cyc_Evexp_eval_const_uint_exp(_tmp556);unsigned int _tmp6F6;int _tmp6F7;struct _tuple12 _tmp6F5=_tmp6F4;_tmp6F6=_tmp6F5.f1;_tmp6F7=_tmp6F5.f2;
if(!_tmp6F7){
const char*_tmpD07;void*_tmpD06;(_tmpD06=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD07="unknown tuple subscript in translation to C",_tag_dyneither(_tmpD07,sizeof(char),44))),_tag_dyneither(_tmpD06,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp555,Cyc_Absyn_fieldname((int)(_tmp6F6 + 1)));
goto _LL30A;};_LL30D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6ED=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6EA;if(_tmp6ED->tag != 5)goto _LL30F;else{_tmp6EE=(_tmp6ED->f1).elt_typ;_tmp6EF=(_tmp6ED->f1).elt_tq;_tmp6F0=((_tmp6ED->f1).ptr_atts).rgn;_tmp6F1=((_tmp6ED->f1).ptr_atts).nullable;_tmp6F2=((_tmp6ED->f1).ptr_atts).bounds;_tmp6F3=((_tmp6ED->f1).ptr_atts).zero_term;}}_LL30E: {
# 3458
struct Cyc_List_List*_tmp6FA=Cyc_Toc_get_relns(_tmp555);
int _tmp6FB=Cyc_Toc_need_null_check(_tmp555);
int _tmp6FC=Cyc_Toc_in_sizeof(nv);
# 3466
int in_bnds=_tmp6FC;
{void*_tmp6FD=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6F2);void*_tmp6FE=_tmp6FD;_LL312:;_LL313:
# 3469
 in_bnds=in_bnds  || Cyc_Toc_check_bounds(_tmp6E9,_tmp6FA,_tmp555,_tmp556);
if(Cyc_Toc_warn_bounds_checks  && !in_bnds){
const char*_tmpD0B;void*_tmpD0A[1];struct Cyc_String_pa_PrintArg_struct _tmpD09;(_tmpD09.tag=0,((_tmpD09.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpD0A[0]=& _tmpD09,Cyc_Tcutil_warn(e->loc,((_tmpD0B="bounds check necessary for %s",_tag_dyneither(_tmpD0B,sizeof(char),30))),_tag_dyneither(_tmpD0A,sizeof(void*),1)))))));}_LL311:;}
# 3476
Cyc_Toc_exp_to_c(nv,_tmp555);
Cyc_Toc_exp_to_c(nv,_tmp556);
++ Cyc_Toc_total_bounds_checks;
{void*_tmp702=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6F2);void*_tmp703=_tmp702;struct Cyc_Absyn_Exp*_tmp705;_LL315: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp704=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp703;if(_tmp704->tag != 1)goto _LL317;else{_tmp705=_tmp704->f1;}}_LL316: {
# 3481
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6F1) && _tmp6FB;
void*ta1=Cyc_Toc_typ_to_c(_tmp6EE);
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp6EF);
if(in_bnds  && !possibly_null)
++ Cyc_Toc_bounds_checks_eliminated;else{
if(in_bnds  && possibly_null){
++ Cyc_Toc_bounds_checks_eliminated;
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpD0E;void*_tmpD0D;(_tmpD0D=0,Cyc_Tcutil_warn(e->loc,((_tmpD0E="inserted null check due to dereference",_tag_dyneither(_tmpD0E,sizeof(char),39))),_tag_dyneither(_tmpD0D,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpD0F[1];_tmp555->r=Cyc_Toc_cast_it_r(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpD0F[0]=Cyc_Absyn_copy_exp(_tmp555),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD0F,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}else{
# 3493
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6F3)){
# 3495
if(!Cyc_Evexp_c_can_eval(_tmp705)){
const char*_tmpD12;void*_tmpD11;(_tmpD11=0,Cyc_Tcutil_terr(e->loc,((_tmpD12="cannot determine subscript is in bounds",_tag_dyneither(_tmpD12,sizeof(char),40))),_tag_dyneither(_tmpD11,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp555);
struct Cyc_Absyn_Exp*_tmpD13[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(function_e,(
(_tmpD13[2]=_tmp556,((_tmpD13[1]=_tmp705,((_tmpD13[0]=_tmp555,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD13,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp705)){
const char*_tmpD16;void*_tmpD15;(_tmpD15=0,Cyc_Tcutil_terr(e->loc,((_tmpD16="cannot determine subscript is in bounds",_tag_dyneither(_tmpD16,sizeof(char),40))),_tag_dyneither(_tmpD15,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpD19;void*_tmpD18;(_tmpD18=0,Cyc_Tcutil_warn(e->loc,((_tmpD19="inserted null check due to dereference",_tag_dyneither(_tmpD19,sizeof(char),39))),_tag_dyneither(_tmpD18,sizeof(void*),0)));}{
# 3507
struct Cyc_Absyn_Exp*_tmpD1A[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpD1A[3]=_tmp556,((_tmpD1A[2]=
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpD1A[1]=_tmp705,((_tmpD1A[0]=_tmp555,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD1A,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{
# 3513
if(!Cyc_Evexp_c_can_eval(_tmp705)){
const char*_tmpD1D;void*_tmpD1C;(_tmpD1C=0,Cyc_Tcutil_terr(e->loc,((_tmpD1D="cannot determine subscript is in bounds",_tag_dyneither(_tmpD1D,sizeof(char),40))),_tag_dyneither(_tmpD1C,sizeof(void*),0)));}{
# 3516
struct Cyc_Absyn_Exp*_tmpD1E[2];_tmp556->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,(
(_tmpD1E[1]=Cyc_Absyn_copy_exp(_tmp556),((_tmpD1E[0]=_tmp705,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD1E,sizeof(struct Cyc_Absyn_Exp*),2)))))));};}}}}
# 3519
goto _LL314;}_LL317: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp706=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp703;if(_tmp706->tag != 0)goto _LL314;}_LL318: {
# 3521
void*ta1=Cyc_Toc_typ_to_c(_tmp6EE);
if(in_bnds){
# 3525
++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6EF),
Cyc_Toc_member_exp(_tmp555,Cyc_Toc_curr_sp,0)),_tmp556);}else{
# 3530
struct Cyc_Absyn_Exp*_tmp715=Cyc_Toc__check_dyneither_subscript_e;
struct Cyc_Absyn_Exp*_tmpD1F[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6EF),
Cyc_Absyn_fncall_exp(_tmp715,(
(_tmpD1F[2]=_tmp556,((_tmpD1F[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpD1F[0]=_tmp555,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD1F,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}
# 3536
goto _LL314;}_LL314:;}
# 3538
goto _LL30A;}_LL30F:;_LL310: {
const char*_tmpD22;void*_tmpD21;(_tmpD21=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD22="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(_tmpD22,sizeof(char),49))),_tag_dyneither(_tmpD21,sizeof(void*),0)));}_LL30A:;}
# 3541
Cyc_Toc_set_lhs(nv,_tmp6E8);
goto _LL215;};}_LL248: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp557=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp557->tag != 23)goto _LL24A;else{_tmp558=_tmp557->f1;}}_LL249:
# 3544
 if(!Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp558))->r;else{
# 3549
struct Cyc_List_List*_tmp719=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp558);
void*_tmp71A=Cyc_Toc_add_tuple_type(_tmp719);
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp558 != 0;(_tmp558=_tmp558->tl,i ++)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp558->hd);{
struct _tuple19*_tmpD25;struct Cyc_List_List*_tmpD24;dles=((_tmpD24=_cycalloc(sizeof(*_tmpD24)),((_tmpD24->hd=((_tmpD25=_cycalloc(sizeof(*_tmpD25)),((_tmpD25->f1=0,((_tmpD25->f2=(struct Cyc_Absyn_Exp*)_tmp558->hd,_tmpD25)))))),((_tmpD24->tl=dles,_tmpD24))))));};}}
# 3556
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}
# 3559
goto _LL215;_LL24A: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp559=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp559->tag != 25)goto _LL24C;else{_tmp55A=_tmp559->f1;}}_LL24B:
# 3563
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp55A);
{struct Cyc_List_List*_tmp71D=_tmp55A;for(0;_tmp71D != 0;_tmp71D=_tmp71D->tl){
struct _tuple19*_tmp71E=(struct _tuple19*)_tmp71D->hd;struct Cyc_Absyn_Exp*_tmp720;struct _tuple19*_tmp71F=_tmp71E;_tmp720=_tmp71F->f2;
Cyc_Toc_exp_to_c(nv,_tmp720);}}
# 3568
goto _LL215;_LL24C: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp55B=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp55B->tag != 26)goto _LL24E;else{_tmp55C=_tmp55B->f1;_tmp55D=_tmp55B->f2;_tmp55E=_tmp55B->f3;_tmp55F=_tmp55B->f4;}}_LL24D: {
# 3572
struct _tuple12 _tmp721=Cyc_Evexp_eval_const_uint_exp(_tmp55D);unsigned int _tmp723;int _tmp724;struct _tuple12 _tmp722=_tmp721;_tmp723=_tmp722.f1;_tmp724=_tmp722.f2;{
void*_tmp725=Cyc_Toc_typ_to_c((void*)_check_null(_tmp55E->topt));
Cyc_Toc_exp_to_c(nv,_tmp55E);{
struct Cyc_List_List*es=0;
# 3577
if(!Cyc_Toc_is_zero(_tmp55E)){
if(!_tmp724){
const char*_tmpD28;void*_tmpD27;(_tmpD27=0,Cyc_Tcutil_terr(_tmp55D->loc,((_tmpD28="cannot determine value of constant",_tag_dyneither(_tmpD28,sizeof(char),35))),_tag_dyneither(_tmpD27,sizeof(void*),0)));}
{unsigned int i=0;for(0;i < _tmp723;++ i){
struct _tuple19*_tmpD2B;struct Cyc_List_List*_tmpD2A;es=((_tmpD2A=_cycalloc(sizeof(*_tmpD2A)),((_tmpD2A->hd=((_tmpD2B=_cycalloc(sizeof(*_tmpD2B)),((_tmpD2B->f1=0,((_tmpD2B->f2=_tmp55E,_tmpD2B)))))),((_tmpD2A->tl=es,_tmpD2A))))));}}
# 3583
if(_tmp55F){
struct Cyc_Absyn_Exp*_tmp72A=Cyc_Toc_cast_it(_tmp725,Cyc_Absyn_uint_exp(0,0));
struct _tuple19*_tmpD2E;struct Cyc_List_List*_tmpD2D;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,((_tmpD2D=_cycalloc(sizeof(*_tmpD2D)),((_tmpD2D->hd=((_tmpD2E=_cycalloc(sizeof(*_tmpD2E)),((_tmpD2E->f1=0,((_tmpD2E->f2=_tmp72A,_tmpD2E)))))),((_tmpD2D->tl=0,_tmpD2D)))))));}}
# 3588
e->r=Cyc_Toc_unresolvedmem_exp_r(0,es);
goto _LL215;};};}_LL24E: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp560=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp560->tag != 27)goto _LL250;else{_tmp561=_tmp560->f1;_tmp562=(void*)_tmp560->f2;_tmp563=_tmp560->f3;}}_LL24F:
# 3593
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,0);
goto _LL215;_LL250: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp564=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp564->tag != 28)goto _LL252;else{_tmp565=_tmp564->f1;_tmp566=_tmp564->f2;_tmp567=_tmp564->f3;_tmp568=_tmp564->f4;}}_LL251:
# 3598
 if(!Cyc_Toc_is_toplevel(nv)){
struct Cyc_Absyn_Exp*_tmp72D=Cyc_Toc_init_struct(nv,old_typ,_tmp566,0,0,_tmp567,_tmp565);
e->r=_tmp72D->r;
e->topt=_tmp72D->topt;}else{
# 3609
if(_tmp568 == 0){
const char*_tmpD31;void*_tmpD30;(_tmpD30=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD31="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpD31,sizeof(char),38))),_tag_dyneither(_tmpD30,sizeof(void*),0)));}{
struct Cyc_Absyn_Aggrdecl*sd2=_tmp568;
# 3613
struct _RegionHandle _tmp730=_new_region("rgn");struct _RegionHandle*rgn=& _tmp730;_push_region(rgn);
{struct Cyc_List_List*_tmp731=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp567,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
# 3617
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp568->impl))->tagged){
# 3619
struct _tuple30*_tmp732=(struct _tuple30*)((struct Cyc_List_List*)_check_null(_tmp731))->hd;struct Cyc_Absyn_Aggrfield*_tmp734;struct Cyc_Absyn_Exp*_tmp735;struct _tuple30*_tmp733=_tmp732;_tmp734=_tmp733->f1;_tmp735=_tmp733->f2;{
void*_tmp736=(void*)_check_null(_tmp735->topt);
void*_tmp737=_tmp734->type;
Cyc_Toc_exp_to_c(nv,_tmp735);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp737) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp736))
_tmp735->r=
Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp735->r,0));{
# 3628
int i=Cyc_Toc_get_member_offset(_tmp568,_tmp734->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct _tuple19*_tmpD36;struct _tuple19*_tmpD35;struct _tuple19*_tmpD34[2];struct Cyc_List_List*_tmp738=(_tmpD34[1]=((_tmpD35=_cycalloc(sizeof(*_tmpD35)),((_tmpD35->f1=0,((_tmpD35->f2=_tmp735,_tmpD35)))))),((_tmpD34[0]=((_tmpD36=_cycalloc(sizeof(*_tmpD36)),((_tmpD36->f1=0,((_tmpD36->f2=field_tag_exp,_tmpD36)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD34,sizeof(struct _tuple19*),2)))));
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp738,0);
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpD3C;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpD3B;void*_tmpD3A[1];struct Cyc_List_List*ds=(_tmpD3A[0]=(void*)((_tmpD3C=_cycalloc(sizeof(*_tmpD3C)),((_tmpD3C[0]=((_tmpD3B.tag=1,((_tmpD3B.f1=_tmp734->name,_tmpD3B)))),_tmpD3C)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD3A,sizeof(void*),1)));
struct _tuple19*_tmpD3F;struct _tuple19*_tmpD3E[1];struct Cyc_List_List*dles=(_tmpD3E[0]=((_tmpD3F=_cycalloc(sizeof(*_tmpD3F)),((_tmpD3F->f1=ds,((_tmpD3F->f2=umem,_tmpD3F)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD3E,sizeof(struct _tuple19*),1)));
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{
# 3637
struct Cyc_List_List*_tmp741=0;
struct Cyc_List_List*_tmp742=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields;
for(0;_tmp742 != 0;_tmp742=_tmp742->tl){
struct Cyc_List_List*_tmp743=_tmp731;for(0;_tmp743 != 0;_tmp743=_tmp743->tl){
if((*((struct _tuple30*)_tmp743->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp742->hd){
struct _tuple30*_tmp744=(struct _tuple30*)_tmp743->hd;struct Cyc_Absyn_Aggrfield*_tmp746;struct Cyc_Absyn_Exp*_tmp747;struct _tuple30*_tmp745=_tmp744;_tmp746=_tmp745->f1;_tmp747=_tmp745->f2;{
void*_tmp748=Cyc_Toc_typ_to_c(_tmp746->type);
void*_tmp749=Cyc_Toc_typ_to_c((void*)_check_null(_tmp747->topt));
Cyc_Toc_exp_to_c(nv,_tmp747);
# 3647
if(!Cyc_Tcutil_unify(_tmp748,_tmp749)){
# 3649
if(!Cyc_Tcutil_is_arithmetic_type(_tmp748) || !
Cyc_Tcutil_is_arithmetic_type(_tmp749))
_tmp747=Cyc_Toc_cast_it(_tmp748,Cyc_Absyn_copy_exp(_tmp747));}
{struct _tuple19*_tmpD42;struct Cyc_List_List*_tmpD41;_tmp741=((_tmpD41=_cycalloc(sizeof(*_tmpD41)),((_tmpD41->hd=((_tmpD42=_cycalloc(sizeof(*_tmpD42)),((_tmpD42->f1=0,((_tmpD42->f2=_tmp747,_tmpD42)))))),((_tmpD41->tl=_tmp741,_tmpD41))))));}
break;};}}}
# 3656
e->r=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp741));}}
# 3614
;_pop_region(rgn);};}
# 3660
goto _LL215;_LL252: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp569=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp569->tag != 29)goto _LL254;else{_tmp56A=(void*)_tmp569->f1;_tmp56B=_tmp569->f2;}}_LL253: {
# 3662
struct Cyc_List_List*fs;
{void*_tmp74C=Cyc_Tcutil_compress(_tmp56A);void*_tmp74D=_tmp74C;struct Cyc_List_List*_tmp74F;_LL31A: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp74E=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp74D;if(_tmp74E->tag != 12)goto _LL31C;else{_tmp74F=_tmp74E->f2;}}_LL31B:
 fs=_tmp74F;goto _LL319;_LL31C:;_LL31D: {
const char*_tmpD46;void*_tmpD45[1];struct Cyc_String_pa_PrintArg_struct _tmpD44;(_tmpD44.tag=0,((_tmpD44.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp56A)),((_tmpD45[0]=& _tmpD44,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD46="anon struct has type %s",_tag_dyneither(_tmpD46,sizeof(char),24))),_tag_dyneither(_tmpD45,sizeof(void*),1)))))));}_LL319:;}{
# 3667
struct _RegionHandle _tmp753=_new_region("rgn");struct _RegionHandle*rgn=& _tmp753;_push_region(rgn);{
struct Cyc_List_List*_tmp754=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp56B,Cyc_Absyn_StructA,fs);
for(0;_tmp754 != 0;_tmp754=_tmp754->tl){
struct _tuple30*_tmp755=(struct _tuple30*)_tmp754->hd;struct Cyc_Absyn_Aggrfield*_tmp757;struct Cyc_Absyn_Exp*_tmp758;struct _tuple30*_tmp756=_tmp755;_tmp757=_tmp756->f1;_tmp758=_tmp756->f2;{
void*_tmp759=(void*)_check_null(_tmp758->topt);
void*_tmp75A=_tmp757->type;
Cyc_Toc_exp_to_c(nv,_tmp758);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp75A) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp759))
_tmp758->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp758->r,0));};}
# 3681
e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp56B);}
# 3683
_npop_handler(0);goto _LL215;
# 3667
;_pop_region(rgn);};}_LL254: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp56C=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp56C->tag != 30)goto _LL256;else{_tmp56D=_tmp56C->f1;_tmp56E=_tmp56C->f2;_tmp56F=_tmp56C->f3;}}_LL255: {
# 3686
void*datatype_ctype;
struct Cyc_Absyn_Exp*tag_exp;
struct _tuple1*_tmp75B=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp75C=Cyc_Absyn_var_exp(_tmp75B,0);
struct Cyc_Absyn_Exp*mem_exp;
datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp56F->name,_tmp56E->name));
tag_exp=_tmp56E->is_extensible?Cyc_Absyn_var_exp(_tmp56F->name,0):
 Cyc_Toc_datatype_tag(_tmp56E,_tmp56F->name);
mem_exp=_tmp75C;{
struct Cyc_List_List*_tmp75D=_tmp56F->typs;
# 3697
if(Cyc_Toc_is_toplevel(nv)){
# 3699
struct Cyc_List_List*dles=0;
for(0;_tmp56D != 0;(_tmp56D=_tmp56D->tl,_tmp75D=_tmp75D->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp56D->hd;
void*_tmp75E=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp75D))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp75E))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct _tuple19*_tmpD49;struct Cyc_List_List*_tmpD48;dles=((_tmpD48=_cycalloc(sizeof(*_tmpD48)),((_tmpD48->hd=((_tmpD49=_cycalloc(sizeof(*_tmpD49)),((_tmpD49->f1=0,((_tmpD49->f2=cur_e,_tmpD49)))))),((_tmpD48->tl=dles,_tmpD48))))));};}
# 3710
{struct _tuple19*_tmpD4C;struct Cyc_List_List*_tmpD4B;dles=((_tmpD4B=_cycalloc(sizeof(*_tmpD4B)),((_tmpD4B->hd=((_tmpD4C=_cycalloc(sizeof(*_tmpD4C)),((_tmpD4C->f1=0,((_tmpD4C->f2=tag_exp,_tmpD4C)))))),((_tmpD4B->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles),_tmpD4B))))));}
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{
# 3716
struct Cyc_List_List*_tmpD4D;struct Cyc_List_List*_tmp763=
(_tmpD4D=_cycalloc(sizeof(*_tmpD4D)),((_tmpD4D->hd=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Toc_tag_sp,0),tag_exp,0),((_tmpD4D->tl=0,_tmpD4D)))));
# 3719
{int i=1;for(0;_tmp56D != 0;(((_tmp56D=_tmp56D->tl,i ++)),_tmp75D=_tmp75D->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp56D->hd;
void*_tmp764=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp75D))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp764))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct Cyc_Absyn_Stmt*_tmp765=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Absyn_fieldname(i),0),cur_e,0);
# 3729
struct Cyc_List_List*_tmpD4E;_tmp763=((_tmpD4E=_cycalloc(sizeof(*_tmpD4E)),((_tmpD4E->hd=_tmp765,((_tmpD4E->tl=_tmp763,_tmpD4E))))));};}}{
# 3731
struct Cyc_Absyn_Stmt*_tmp767=Cyc_Absyn_exp_stmt(_tmp75C,0);
struct Cyc_List_List*_tmpD4F;struct Cyc_Absyn_Stmt*_tmp768=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpD4F=_cycalloc(sizeof(*_tmpD4F)),((_tmpD4F->hd=_tmp767,((_tmpD4F->tl=_tmp763,_tmpD4F))))))),0);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp75B,datatype_ctype,0,_tmp768,0));};}
# 3735
goto _LL215;};}_LL256: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp570=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp570->tag != 31)goto _LL258;}_LL257:
# 3737
 goto _LL259;_LL258: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp571=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp571->tag != 32)goto _LL25A;}_LL259:
 goto _LL215;_LL25A: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp572=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp572->tag != 33)goto _LL25C;else{_tmp573=(_tmp572->f1).is_calloc;_tmp574=(_tmp572->f1).rgn;_tmp575=(_tmp572->f1).elt_type;_tmp576=(_tmp572->f1).num_elts;_tmp577=(_tmp572->f1).fat_result;}}_LL25B: {
# 3741
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp575)));
Cyc_Toc_exp_to_c(nv,_tmp576);
# 3745
if(_tmp577){
struct _tuple1*_tmp76B=Cyc_Toc_temp_var();
struct _tuple1*_tmp76C=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp573){
xexp=_tmp576;
if(_tmp574 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp574;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp76B,0));}else{
# 3756
pexp=Cyc_Toc_calloc_exp(*_tmp575,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp76B,0));}{
# 3758
struct Cyc_Absyn_Exp*_tmpD50[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpD50[2]=
Cyc_Absyn_var_exp(_tmp76B,0),((_tmpD50[1]=
# 3759
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpD50[0]=Cyc_Absyn_var_exp(_tmp76C,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD50,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{
# 3762
if(_tmp574 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp574;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp76B,0));}else{
# 3767
pexp=Cyc_Toc_malloc_exp(*_tmp575,Cyc_Absyn_var_exp(_tmp76B,0));}{
# 3769
struct Cyc_Absyn_Exp*_tmpD51[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((_tmpD51[2]=
Cyc_Absyn_var_exp(_tmp76B,0),((_tmpD51[1]=
# 3769
Cyc_Absyn_uint_exp(1,0),((_tmpD51[0]=Cyc_Absyn_var_exp(_tmp76C,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD51,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}{
# 3772
struct Cyc_Absyn_Stmt*_tmp76F=Cyc_Absyn_declare_stmt(_tmp76B,Cyc_Absyn_uint_typ,_tmp576,
Cyc_Absyn_declare_stmt(_tmp76C,Cyc_Absyn_cstar_typ(t_c,Cyc_Toc_mt_tq),pexp,
Cyc_Absyn_exp_stmt(rexp,0),0),0);
e->r=Cyc_Toc_stmt_exp_r(_tmp76F);};}else{
# 3777
if(_tmp573){
if(_tmp574 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp574;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp576))->r;}else{
# 3783
e->r=(Cyc_Toc_calloc_exp(*_tmp575,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp576))->r;}}else{
# 3786
if(_tmp574 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp574;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rmalloc_exp(rgn,_tmp576))->r;}else{
# 3791
e->r=(Cyc_Toc_malloc_exp(*_tmp575,_tmp576))->r;}}}
# 3795
goto _LL215;}_LL25C: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp578=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp578->tag != 34)goto _LL25E;else{_tmp579=_tmp578->f1;_tmp57A=_tmp578->f2;}}_LL25D: {
# 3804
void*e1_old_typ=(void*)_check_null(_tmp579->topt);
void*e2_old_typ=(void*)_check_null(_tmp57A->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ) && !Cyc_Tcutil_is_pointer_type(e1_old_typ)){
const char*_tmpD54;void*_tmpD53;(_tmpD53=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD54="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",_tag_dyneither(_tmpD54,sizeof(char),57))),_tag_dyneither(_tmpD53,sizeof(void*),0)));}{
# 3810
struct Cyc_Absyn_Exp*swap_fn;
if(Cyc_Tcutil_is_dyneither_ptr(e1_old_typ))
swap_fn=Cyc_Toc__swap_dyneither_e;else{
# 3814
swap_fn=Cyc_Toc__swap_word_e;}{
# 3818
int f1_tag=0;
void*tagged_mem_type1=(void*)& Cyc_Absyn_VoidType_val;
int e1_tmem=Cyc_Toc_is_tagged_union_project(_tmp579,& f1_tag,& tagged_mem_type1,1);
int f2_tag=0;
void*tagged_mem_type2=(void*)& Cyc_Absyn_VoidType_val;
int e2_tmem=Cyc_Toc_is_tagged_union_project(_tmp57A,& f2_tag,& tagged_mem_type2,1);
# 3825
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp579);
Cyc_Toc_exp_to_c(nv,_tmp57A);
Cyc_Toc_set_lhs(nv,0);
# 3831
if(e1_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp579,f1_tag,tagged_mem_type1);else{
# 3835
_tmp579=Cyc_Toc_push_address_exp(_tmp579);}
# 3837
if(e2_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp57A,f2_tag,tagged_mem_type2);else{
# 3841
_tmp57A=Cyc_Toc_push_address_exp(_tmp57A);}
# 3843
{struct Cyc_Absyn_Exp*_tmpD55[2];e->r=Cyc_Toc_fncall_exp_r(swap_fn,((_tmpD55[1]=_tmp57A,((_tmpD55[0]=_tmp579,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD55,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
# 3845
goto _LL215;};};}_LL25E: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp57B=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp57B->tag != 37)goto _LL260;else{_tmp57C=_tmp57B->f1;_tmp57D=_tmp57B->f2;}}_LL25F: {
# 3848
void*_tmp773=Cyc_Tcutil_compress((void*)_check_null(_tmp57C->topt));
Cyc_Toc_exp_to_c(nv,_tmp57C);
{void*_tmp774=_tmp773;struct Cyc_Absyn_Aggrdecl*_tmp776;_LL31F: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp775=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp774;if(_tmp775->tag != 11)goto _LL321;else{if((((_tmp775->f1).aggr_info).KnownAggr).tag != 2)goto _LL321;_tmp776=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp775->f1).aggr_info).KnownAggr).val);}}_LL320: {
# 3852
struct Cyc_Absyn_Exp*_tmp777=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp776,_tmp57D),0);
struct Cyc_Absyn_Exp*_tmp778=Cyc_Toc_member_exp(_tmp57C,_tmp57D,0);
struct Cyc_Absyn_Exp*_tmp779=Cyc_Toc_member_exp(_tmp778,Cyc_Toc_tag_sp,0);
e->r=(Cyc_Absyn_eq_exp(_tmp779,_tmp777,0))->r;
goto _LL31E;}_LL321:;_LL322: {
const char*_tmpD58;void*_tmpD57;(_tmpD57=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD58="non-aggregate type in tagcheck",_tag_dyneither(_tmpD58,sizeof(char),31))),_tag_dyneither(_tmpD57,sizeof(void*),0)));}_LL31E:;}
# 3859
goto _LL215;}_LL260: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp57E=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp57E->tag != 36)goto _LL262;else{_tmp57F=_tmp57E->f1;}}_LL261:
 Cyc_Toc_stmt_to_c(nv,_tmp57F);goto _LL215;_LL262: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp580=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp580->tag != 35)goto _LL264;}_LL263: {
const char*_tmpD5B;void*_tmpD5A;(_tmpD5A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD5B="UnresolvedMem",_tag_dyneither(_tmpD5B,sizeof(char),14))),_tag_dyneither(_tmpD5A,sizeof(void*),0)));}_LL264: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp581=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp581->tag != 24)goto _LL266;}_LL265: {
const char*_tmpD5E;void*_tmpD5D;(_tmpD5D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpD5E="compoundlit",_tag_dyneither(_tmpD5E,sizeof(char),12))),_tag_dyneither(_tmpD5D,sizeof(void*),0)));}_LL266: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp582=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp582->tag != 38)goto _LL268;}_LL267: {
const char*_tmpD61;void*_tmpD60;(_tmpD60=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD61="valueof(-)",_tag_dyneither(_tmpD61,sizeof(char),11))),_tag_dyneither(_tmpD60,sizeof(void*),0)));}_LL268: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp583=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp509;if(_tmp583->tag != 39)goto _LL215;}_LL269: {
const char*_tmpD64;void*_tmpD63;(_tmpD63=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD64="__asm__",_tag_dyneither(_tmpD64,sizeof(char),8))),_tag_dyneither(_tmpD63,sizeof(void*),0)));}_LL215:;};}
# 3896 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){
return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
# 3899
Cyc_Toc_skip_stmt_dl(),0);}
# 3902
static int Cyc_Toc_path_length(struct Cyc_Absyn_Exp*e){
void*_tmp784=e->r;void*_tmp785=_tmp784;struct Cyc_Absyn_Exp*_tmp787;struct Cyc_Absyn_Exp*_tmp789;struct Cyc_Absyn_Exp*_tmp78B;_LL324: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp786=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp785;if(_tmp786->tag != 13)goto _LL326;else{_tmp787=_tmp786->f2;}}_LL325:
 return Cyc_Toc_path_length(_tmp787);_LL326: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp788=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp785;if(_tmp788->tag != 19)goto _LL328;else{_tmp789=_tmp788->f1;}}_LL327:
 return 1 + Cyc_Toc_path_length(_tmp789);_LL328: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp78A=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp785;if(_tmp78A->tag != 20)goto _LL32A;else{_tmp78B=_tmp78A->f1;}}_LL329:
 return Cyc_Toc_path_length(_tmp78B);_LL32A:;_LL32B:
 return 0;_LL323:;}struct _tuple31{struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple32{struct _tuple1*f1;void*f2;};struct _tuple33{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3911
static struct _tuple31 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
# 3916
struct Cyc_Absyn_Stmt*s;
{void*_tmp78C=p->r;void*_tmp78D=_tmp78C;struct _tuple1*_tmp78F;struct _tuple1*_tmp791;struct Cyc_Absyn_Pat*_tmp792;struct _tuple1*_tmp794;struct Cyc_Absyn_Vardecl*_tmp797;struct Cyc_Absyn_Pat*_tmp798;enum Cyc_Absyn_Sign _tmp79B;int _tmp79C;char _tmp79E;struct _dyneither_ptr _tmp7A0;int _tmp7A1;struct Cyc_Absyn_Enumdecl*_tmp7A3;struct Cyc_Absyn_Enumfield*_tmp7A4;void*_tmp7A6;struct Cyc_Absyn_Enumfield*_tmp7A7;struct Cyc_Absyn_Datatypedecl*_tmp7AA;struct Cyc_Absyn_Datatypefield*_tmp7AB;struct Cyc_List_List*_tmp7AC;struct Cyc_List_List*_tmp7AE;struct Cyc_List_List*_tmp7B0;union Cyc_Absyn_AggrInfoU _tmp7B3;struct Cyc_List_List*_tmp7B4;struct Cyc_Absyn_Pat*_tmp7B6;_LL32D: {struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp78E=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp78D;if(_tmp78E->tag != 2)goto _LL32F;else{_tmp78F=(_tmp78E->f2)->name;}}_LL32E: {
# 3919
struct Cyc_Absyn_Pat*_tmp7BA=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0);
_tmp7BA->topt=p->topt;
_tmp791=_tmp78F;_tmp792=_tmp7BA;goto _LL330;}_LL32F: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp790=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp78D;if(_tmp790->tag != 1)goto _LL331;else{_tmp791=(_tmp790->f1)->name;_tmp792=_tmp790->f2;}}_LL330: {
# 3924
struct _tuple1*v=Cyc_Toc_temp_var();
void*_tmp7BB=(void*)_check_null(_tmp792->topt);
{struct _tuple32*_tmpD67;struct Cyc_List_List*_tmpD66;decls=((_tmpD66=_region_malloc(rgn,sizeof(*_tmpD66)),((_tmpD66->hd=((_tmpD67=_region_malloc(rgn,sizeof(*_tmpD67)),((_tmpD67->f1=v,((_tmpD67->f2=Cyc_Toc_typ_to_c_array(_tmp7BB),_tmpD67)))))),((_tmpD66->tl=decls,_tmpD66))))));}{
struct _tuple31 _tmp7BE=Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp791,Cyc_Absyn_var_exp(v,0)),rgn,_tmp7BB,
Cyc_Absyn_var_exp(v,0),path,_tmp792,fail_stmt,decls);
nv=_tmp7BE.f1;
decls=_tmp7BE.f2;{
struct Cyc_Absyn_Stmt*_tmp7BF=_tmp7BE.f3;
struct Cyc_Absyn_Stmt*_tmp7C0=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(v,0),r,0);
s=Cyc_Absyn_seq_stmt(_tmp7C0,_tmp7BF,0);
goto _LL32C;};};}_LL331: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp793=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp78D;if(_tmp793->tag != 4)goto _LL333;else{_tmp794=(_tmp793->f2)->name;}}_LL332: {
# 3937
struct _tuple1*_tmp7C1=Cyc_Toc_temp_var();
void*_tmp7C2=(void*)_check_null(p->topt);
{struct _tuple32*_tmpD6A;struct Cyc_List_List*_tmpD69;decls=((_tmpD69=_region_malloc(rgn,sizeof(*_tmpD69)),((_tmpD69->hd=((_tmpD6A=_region_malloc(rgn,sizeof(*_tmpD6A)),((_tmpD6A->f1=_tmp7C1,((_tmpD6A->f2=Cyc_Toc_typ_to_c_array(_tmp7C2),_tmpD6A)))))),((_tmpD69->tl=decls,_tmpD69))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp794,Cyc_Absyn_var_exp(_tmp7C1,0));
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7C1,0),r,0);
goto _LL32C;}_LL333: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp795=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp78D;if(_tmp795->tag != 0)goto _LL335;}_LL334:
 s=Cyc_Toc_skip_stmt_dl();goto _LL32C;_LL335: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp796=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp78D;if(_tmp796->tag != 3)goto _LL337;else{_tmp797=_tmp796->f1;_tmp798=_tmp796->f2;}}_LL336: {
# 3946
struct _tuple1*_tmp7C5=Cyc_Toc_temp_var();
{struct _tuple32*_tmpD6D;struct Cyc_List_List*_tmpD6C;decls=((_tmpD6C=_region_malloc(rgn,sizeof(*_tmpD6C)),((_tmpD6C->hd=((_tmpD6D=_region_malloc(rgn,sizeof(*_tmpD6D)),((_tmpD6D->f1=_tmp7C5,((_tmpD6D->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),_tmpD6D)))))),((_tmpD6C->tl=decls,_tmpD6C))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp797->name,Cyc_Absyn_var_exp(_tmp7C5,0));
# 3950
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7C5,0),
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(path)),0);{
struct _tuple31 _tmp7C8=Cyc_Toc_xlate_pat(nv,rgn,t,r,path,_tmp798,fail_stmt,decls);
_tmp7C8.f3=Cyc_Absyn_seq_stmt(s,_tmp7C8.f3,0);
return _tmp7C8;};}_LL337: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp799=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp78D;if(_tmp799->tag != 9)goto _LL339;}_LL338:
# 3957
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL32C;_LL339: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp79A=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp78D;if(_tmp79A->tag != 10)goto _LL33B;else{_tmp79B=_tmp79A->f1;_tmp79C=_tmp79A->f2;}}_LL33A:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(_tmp79B,_tmp79C,0),fail_stmt);goto _LL32C;_LL33B: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp79D=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp78D;if(_tmp79D->tag != 11)goto _LL33D;else{_tmp79E=_tmp79D->f1;}}_LL33C:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp79E,0),fail_stmt);goto _LL32C;_LL33D: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp79F=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp78D;if(_tmp79F->tag != 12)goto _LL33F;else{_tmp7A0=_tmp79F->f1;_tmp7A1=_tmp79F->f2;}}_LL33E:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp7A0,_tmp7A1,0),fail_stmt);goto _LL32C;_LL33F: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp7A2=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp78D;if(_tmp7A2->tag != 13)goto _LL341;else{_tmp7A3=_tmp7A2->f1;_tmp7A4=_tmp7A2->f2;}}_LL340:
# 3962
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmpD70;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpD6F;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpD6F=_cycalloc(sizeof(*_tmpD6F)),((_tmpD6F[0]=((_tmpD70.tag=31,((_tmpD70.f1=_tmp7A4->name,((_tmpD70.f2=_tmp7A3,((_tmpD70.f3=_tmp7A4,_tmpD70)))))))),_tmpD6F)))),0),fail_stmt);}
goto _LL32C;_LL341: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp7A5=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp78D;if(_tmp7A5->tag != 14)goto _LL343;else{_tmp7A6=(void*)_tmp7A5->f1;_tmp7A7=_tmp7A5->f2;}}_LL342:
# 3965
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpD73;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpD72;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpD72=_cycalloc(sizeof(*_tmpD72)),((_tmpD72[0]=((_tmpD73.tag=32,((_tmpD73.f1=_tmp7A7->name,((_tmpD73.f2=_tmp7A6,((_tmpD73.f3=_tmp7A7,_tmpD73)))))))),_tmpD72)))),0),fail_stmt);}
goto _LL32C;_LL343: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp7A8=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp78D;if(_tmp7A8->tag != 6)goto _LL345;else{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp7A9=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(_tmp7A8->f1)->r;if(_tmp7A9->tag != 8)goto _LL345;else{_tmp7AA=_tmp7A9->f1;_tmp7AB=_tmp7A9->f2;_tmp7AC=_tmp7A9->f3;}}}_LL344:
# 3975
 s=Cyc_Toc_skip_stmt_dl();{
struct _tuple1*_tmp7CD=Cyc_Toc_temp_var();
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(_tmp7AB->name,_tmp7AA->name);
void*_tmp7CE=Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);
int cnt=1;
struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(_tmp7CE,r);
struct Cyc_List_List*_tmp7CF=_tmp7AB->typs;
for(0;_tmp7AC != 0;(((_tmp7AC=_tmp7AC->tl,_tmp7CF=((struct Cyc_List_List*)_check_null(_tmp7CF))->tl)),++ cnt)){
struct Cyc_Absyn_Pat*_tmp7D0=(struct Cyc_Absyn_Pat*)_tmp7AC->hd;
if(_tmp7D0->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp7D1=(*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp7CF))->hd)).f2;
void*_tmp7D2=(void*)_check_null(_tmp7D0->topt);
void*_tmp7D3=Cyc_Toc_typ_to_c_array(_tmp7D2);
struct Cyc_Absyn_Exp*_tmp7D4=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp7CD,0),Cyc_Absyn_fieldname(cnt),0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp7D1)))
_tmp7D4=Cyc_Toc_cast_it(_tmp7D3,_tmp7D4);{
# 3993
struct _tuple31 _tmp7D5=Cyc_Toc_xlate_pat(nv,rgn,_tmp7D2,_tmp7D4,_tmp7D4,_tmp7D0,fail_stmt,decls);
# 3995
nv=_tmp7D5.f1;
decls=_tmp7D5.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp7D5.f3,0);};};}{
# 3999
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp7CD,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*tag_exp=
_tmp7AA->is_extensible?Cyc_Absyn_var_exp(_tmp7AB->name,0): Cyc_Toc_datatype_tag(_tmp7AA,_tmp7AB->name);
struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,tag_exp,0);
s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);
if(Cyc_Toc_is_nullable((void*)_check_null(p->topt)))
# 4007
s=Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp7CD,0),Cyc_Absyn_uint_exp(0,0),0),fail_stmt,s,0);
# 4010
s=Cyc_Absyn_declare_stmt(_tmp7CD,_tmp7CE,rcast,s,0);
goto _LL32C;};};_LL345: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp7AD=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp78D;if(_tmp7AD->tag != 8)goto _LL347;else{_tmp7AE=_tmp7AD->f3;}}_LL346:
# 4013
 _tmp7B0=_tmp7AE;goto _LL348;_LL347: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp7AF=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp78D;if(_tmp7AF->tag != 5)goto _LL349;else{_tmp7B0=_tmp7AF->f1;}}_LL348:
# 4021
 s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;
for(0;_tmp7B0 != 0;(_tmp7B0=_tmp7B0->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp7D6=(struct Cyc_Absyn_Pat*)_tmp7B0->hd;
if(_tmp7D6->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp7D7=(void*)_check_null(_tmp7D6->topt);
struct _dyneither_ptr*_tmp7D8=Cyc_Absyn_fieldname(cnt);
struct _tuple31 _tmp7D9=Cyc_Toc_xlate_pat(nv,rgn,_tmp7D7,Cyc_Toc_member_exp(r,_tmp7D8,0),
Cyc_Toc_member_exp(path,_tmp7D8,0),_tmp7D6,fail_stmt,decls);
nv=_tmp7D9.f1;
decls=_tmp7D9.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp7D9.f3,0);};}
# 4035
goto _LL32C;};_LL349: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp7B1=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp78D;if(_tmp7B1->tag != 7)goto _LL34B;else{if(_tmp7B1->f1 != 0)goto _LL34B;}}_LL34A: {
# 4037
const char*_tmpD76;void*_tmpD75;(_tmpD75=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD76="unresolved aggregate pattern!",_tag_dyneither(_tmpD76,sizeof(char),30))),_tag_dyneither(_tmpD75,sizeof(void*),0)));}_LL34B: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp7B2=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp78D;if(_tmp7B2->tag != 7)goto _LL34D;else{if(_tmp7B2->f1 == 0)goto _LL34D;_tmp7B3=(_tmp7B2->f1)->aggr_info;_tmp7B4=_tmp7B2->f3;}}_LL34C: {
# 4039
struct Cyc_Absyn_Aggrdecl*_tmp7DC=Cyc_Absyn_get_known_aggrdecl(_tmp7B3);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7DC->impl))->tagged){
# 4043
struct _tuple33*_tmp7DD=(struct _tuple33*)((struct Cyc_List_List*)_check_null(_tmp7B4))->hd;struct Cyc_List_List*_tmp7DF;struct Cyc_Absyn_Pat*_tmp7E0;struct _tuple33*_tmp7DE=_tmp7DD;_tmp7DF=_tmp7DE->f1;_tmp7E0=_tmp7DE->f2;{
struct _dyneither_ptr*f;
{void*_tmp7E1=(void*)((struct Cyc_List_List*)_check_null(_tmp7DF))->hd;void*_tmp7E2=_tmp7E1;struct _dyneither_ptr*_tmp7E4;_LL356: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp7E3=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp7E2;if(_tmp7E3->tag != 1)goto _LL358;else{_tmp7E4=_tmp7E3->f1;}}_LL357:
 f=_tmp7E4;goto _LL355;_LL358:;_LL359: {
const char*_tmpD79;void*_tmpD78;(_tmpD78=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD79="no field name in tagged union pattern",_tag_dyneither(_tmpD79,sizeof(char),38))),_tag_dyneither(_tmpD78,sizeof(void*),0)));}_LL355:;}{
# 4050
void*_tmp7E7=(void*)_check_null(_tmp7E0->topt);
void*_tmp7E8=Cyc_Toc_typ_to_c_array(_tmp7E7);
# 4053
struct Cyc_Absyn_Exp*_tmp7E9=Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(path,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*_tmp7EA=Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp7EA=Cyc_Toc_cast_it(_tmp7E8,_tmp7EA);
_tmp7E9=Cyc_Toc_cast_it(_tmp7E8,_tmp7E9);{
struct _tuple31 _tmp7EB=Cyc_Toc_xlate_pat(nv,rgn,_tmp7E7,_tmp7EA,_tmp7E9,_tmp7E0,fail_stmt,decls);
# 4061
nv=_tmp7EB.f1;
decls=_tmp7EB.f2;{
struct Cyc_Absyn_Stmt*_tmp7EC=_tmp7EB.f3;
struct Cyc_Absyn_Stmt*_tmp7ED=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(r,f,0),Cyc_Toc_tag_sp,0),
Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp7DC,f),0),fail_stmt);
# 4069
s=Cyc_Absyn_seq_stmt(_tmp7ED,_tmp7EC,0);};};};};}else{
# 4072
s=Cyc_Toc_skip_stmt_dl();
for(0;_tmp7B4 != 0;_tmp7B4=_tmp7B4->tl){
struct _tuple33*_tmp7EE=(struct _tuple33*)_tmp7B4->hd;
struct Cyc_Absyn_Pat*_tmp7EF=(*_tmp7EE).f2;
if(_tmp7EF->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*f;
{void*_tmp7F0=(void*)((struct Cyc_List_List*)_check_null((*_tmp7EE).f1))->hd;void*_tmp7F1=_tmp7F0;struct _dyneither_ptr*_tmp7F3;_LL35B: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp7F2=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp7F1;if(_tmp7F2->tag != 1)goto _LL35D;else{_tmp7F3=_tmp7F2->f1;}}_LL35C:
 f=_tmp7F3;goto _LL35A;_LL35D:;_LL35E: {
struct Cyc_Toc_Match_error_exn_struct _tmpD7C;struct Cyc_Toc_Match_error_exn_struct*_tmpD7B;(int)_throw((void*)((_tmpD7B=_cycalloc_atomic(sizeof(*_tmpD7B)),((_tmpD7B[0]=((_tmpD7C.tag=Cyc_Toc_Match_error,_tmpD7C)),_tmpD7B)))));}_LL35A:;}{
# 4083
void*_tmp7F6=(void*)_check_null(_tmp7EF->topt);
void*_tmp7F7=Cyc_Toc_typ_to_c_array(_tmp7F6);
struct Cyc_Absyn_Exp*_tmp7F8=Cyc_Toc_member_exp(r,f,0);
void*_tmp7F9=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7DC->impl))->fields,f)))->type;
struct Cyc_Absyn_Exp*_tmp7FA=Cyc_Toc_member_exp(path,f,0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp7F9))
_tmp7F8=Cyc_Toc_cast_it(_tmp7F7,_tmp7F8);else{
if(!Cyc_Toc_is_array_type(_tmp7F9) && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp7F9))){
# 4093
_tmp7F8=Cyc_Absyn_deref_exp(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(_tmp7F7,Cyc_Toc_mt_tq),
Cyc_Absyn_address_exp(_tmp7F8,0)),0);
# 4096
_tmp7FA=Cyc_Toc_push_address_exp(_tmp7FA);
_tmp7FA=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(_tmp7F7,Cyc_Toc_mt_tq),_tmp7FA);
_tmp7FA=Cyc_Absyn_deref_exp(_tmp7FA,0);}}{
# 4100
struct _tuple31 _tmp7FB=Cyc_Toc_xlate_pat(nv,rgn,_tmp7F6,_tmp7F8,_tmp7FA,_tmp7EF,fail_stmt,decls);
nv=_tmp7FB.f1;
decls=_tmp7FB.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp7FB.f3,0);};};};}}
# 4106
goto _LL32C;}_LL34D: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp7B5=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp78D;if(_tmp7B5->tag != 6)goto _LL34F;else{_tmp7B6=_tmp7B5->f1;}}_LL34E: {
# 4109
void*_tmp7FC=(void*)_check_null(_tmp7B6->topt);
# 4112
struct _tuple31 _tmp7FD=Cyc_Toc_xlate_pat(nv,rgn,_tmp7FC,Cyc_Absyn_deref_exp(r,0),Cyc_Absyn_deref_exp(path,0),_tmp7B6,fail_stmt,decls);
# 4114
nv=_tmp7FD.f1;
decls=_tmp7FD.f2;{
struct Cyc_Absyn_Stmt*_tmp7FE=_tmp7FD.f3;
if(Cyc_Toc_is_nullable(t))
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,
# 4120
Cyc_Toc_skip_stmt_dl(),0),_tmp7FE,0);else{
# 4122
s=_tmp7FE;}
goto _LL32C;};}_LL34F: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp7B7=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp78D;if(_tmp7B7->tag != 15)goto _LL351;}_LL350: {
const char*_tmpD7F;void*_tmpD7E;(_tmpD7E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD7F="unknownid pat",_tag_dyneither(_tmpD7F,sizeof(char),14))),_tag_dyneither(_tmpD7E,sizeof(void*),0)));}_LL351: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp7B8=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp78D;if(_tmp7B8->tag != 16)goto _LL353;}_LL352: {
const char*_tmpD82;void*_tmpD81;(_tmpD81=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD82="unknowncall pat",_tag_dyneither(_tmpD82,sizeof(char),16))),_tag_dyneither(_tmpD81,sizeof(void*),0)));}_LL353: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp7B9=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp78D;if(_tmp7B9->tag != 17)goto _LL32C;}_LL354: {
const char*_tmpD85;void*_tmpD84;(_tmpD84=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD85="exp pat",_tag_dyneither(_tmpD85,sizeof(char),8))),_tag_dyneither(_tmpD84,sizeof(void*),0)));}_LL32C:;}{
# 4128
struct _tuple31 _tmpD86;return(_tmpD86.f1=nv,((_tmpD86.f2=decls,((_tmpD86.f3=s,_tmpD86)))));};}struct _tuple34{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};
# 4165 "toc.cyc"
static struct _tuple34*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 4167
struct _tuple34*_tmpD87;return(_tmpD87=_region_malloc(r,sizeof(*_tmpD87)),((_tmpD87->f1=Cyc_Toc_fresh_label(),((_tmpD87->f2=Cyc_Toc_fresh_label(),((_tmpD87->f3=sc,_tmpD87)))))));}
# 4170
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){
# 4172
Cyc_Toc_exp_to_c(nv,e);{
void*_tmp807=(void*)_check_null(e->topt);
# 4177
int leave_as_switch;
{void*_tmp808=Cyc_Tcutil_compress(_tmp807);void*_tmp809=_tmp808;_LL360: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp80A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp809;if(_tmp80A->tag != 6)goto _LL362;}_LL361:
 goto _LL363;_LL362: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp80B=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp809;if(_tmp80B->tag != 13)goto _LL364;}_LL363:
 leave_as_switch=1;goto _LL35F;_LL364:;_LL365:
 leave_as_switch=0;goto _LL35F;_LL35F:;}
# 4183
{struct Cyc_List_List*_tmp80C=scs;for(0;_tmp80C != 0;_tmp80C=_tmp80C->tl){
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp80C->hd)->pat_vars))->v != 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp80C->hd)->where_clause != 0){
leave_as_switch=0;
break;}}}
# 4188
if(leave_as_switch){
# 4190
struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();
{struct Cyc_List_List*_tmp80D=scs;for(0;_tmp80D != 0;_tmp80D=_tmp80D->tl){
struct Cyc_Absyn_Stmt*_tmp80E=((struct Cyc_Absyn_Switch_clause*)_tmp80D->hd)->body;
((struct Cyc_Absyn_Switch_clause*)_tmp80D->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp80E,0);
next_l=Cyc_Toc_fresh_label();{
struct _RegionHandle _tmp80F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp80F;_push_region(rgn);
Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(rgn,nv,next_l),_tmp80E);;_pop_region(rgn);};}}
# 4199
return;}{
# 4202
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 4207
struct _RegionHandle _tmp810=_new_region("rgn");struct _RegionHandle*rgn=& _tmp810;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp811=Cyc_Toc_share_env(rgn,nv);
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple34*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,rgn,scs);
struct Cyc_List_List*test_stmts=0;
struct Cyc_List_List*nvs=0;
struct Cyc_List_List*decls=0;
# 4218
{struct Cyc_List_List*_tmp812=lscs;for(0;_tmp812 != 0;_tmp812=_tmp812->tl){
struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple34*)_tmp812->hd)).f3;
struct _dyneither_ptr*fail_lab=_tmp812->tl == 0?end_l:(*((struct _tuple34*)((struct Cyc_List_List*)_check_null(_tmp812->tl))->hd)).f1;
struct _tuple31 _tmp813=Cyc_Toc_xlate_pat(_tmp811,rgn,_tmp807,r,path,sc->pattern,
Cyc_Absyn_goto_stmt(fail_lab,0),decls);
# 4221
struct Cyc_Toc_Env*_tmp815;struct Cyc_List_List*_tmp816;struct Cyc_Absyn_Stmt*_tmp817;struct _tuple31 _tmp814=_tmp813;_tmp815=_tmp814.f1;_tmp816=_tmp814.f2;_tmp817=_tmp814.f3;
# 4224
if(sc->where_clause != 0){
struct Cyc_Absyn_Exp*_tmp818=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);
Cyc_Toc_exp_to_c(_tmp815,_tmp818);
_tmp817=Cyc_Absyn_seq_stmt(_tmp817,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp818,0),
Cyc_Absyn_goto_stmt(fail_lab,0),
Cyc_Toc_skip_stmt_dl(),0),0);}
# 4231
decls=_tmp816;
{struct Cyc_List_List*_tmpD88;nvs=((_tmpD88=_region_malloc(rgn,sizeof(*_tmpD88)),((_tmpD88->hd=_tmp815,((_tmpD88->tl=nvs,_tmpD88))))));}{
struct Cyc_List_List*_tmpD89;test_stmts=((_tmpD89=_region_malloc(rgn,sizeof(*_tmpD89)),((_tmpD89->hd=_tmp817,((_tmpD89->tl=test_stmts,_tmpD89))))));};}}
# 4235
nvs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);
test_stmts=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{
struct Cyc_List_List*stmts=0;
# 4240
for(0;lscs != 0;
(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple34*_tmp81B=(struct _tuple34*)lscs->hd;struct _dyneither_ptr*_tmp81D;struct _dyneither_ptr*_tmp81E;struct Cyc_Absyn_Switch_clause*_tmp81F;struct _tuple34*_tmp81C=_tmp81B;_tmp81D=_tmp81C->f1;_tmp81E=_tmp81C->f2;_tmp81F=_tmp81C->f3;{
struct Cyc_Toc_Env*_tmp820=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs))->hd;
struct Cyc_Absyn_Stmt*s=_tmp81F->body;
struct _RegionHandle _tmp821=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp821;_push_region(rgn2);
if(lscs->tl != 0){
struct _tuple34*_tmp822=(struct _tuple34*)((struct Cyc_List_List*)_check_null(lscs->tl))->hd;struct _dyneither_ptr*_tmp824;struct Cyc_Absyn_Switch_clause*_tmp825;struct _tuple34*_tmp823=_tmp822;_tmp824=_tmp823->f2;_tmp825=_tmp823->f3;{
struct Cyc_List_List*_tmp826=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp825->pat_vars))->v)).f1);
_tmp826=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp826);
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(rgn2,_tmp820,end_l,_tmp824,_tmp826,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs->tl))->hd),s);};}else{
# 4255
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn2,_tmp820,end_l),s);}
# 4257
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp81D,(struct Cyc_Absyn_Stmt*)((struct Cyc_List_List*)_check_null(test_stmts))->hd,0),
Cyc_Absyn_label_stmt(_tmp81E,s,0),0);{
struct Cyc_List_List*_tmpD8A;stmts=((_tmpD8A=_region_malloc(rgn,sizeof(*_tmpD8A)),((_tmpD8A->hd=s,((_tmpD8A->tl=stmts,_tmpD8A))))));};
# 4245
;_pop_region(rgn2);};}{
# 4261
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),
Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),0),0);
# 4264
for(decls;decls != 0;decls=decls->tl){
struct _tuple32*_tmp828=(struct _tuple32*)decls->hd;struct _tuple1*_tmp82A;void*_tmp82B;struct _tuple32*_tmp829=_tmp828;_tmp82A=_tmp829->f1;_tmp82B=_tmp829->f2;
res=Cyc_Absyn_declare_stmt(_tmp82A,_tmp82B,0,res,0);}
# 4269
whole_s->r=(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),e,res,0))->r;};};}
# 4208
;_pop_region(rgn);};};}
# 4274
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 4280
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 4282
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
struct Cyc_List_List*_tmpD8B;return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,(
(_tmpD8B=_cycalloc(sizeof(*_tmpD8B)),((_tmpD8B->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0),((_tmpD8B->tl=0,_tmpD8B)))))),0),0);}
# 4286
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt(s->r,0));}struct _tuple35{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 4291
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 4293
while(1){
void*_tmp82D=s->r;void*_tmp82E=_tmp82D;struct Cyc_Absyn_Exp*_tmp831;struct Cyc_Absyn_Stmt*_tmp833;struct Cyc_Absyn_Stmt*_tmp834;struct Cyc_Absyn_Exp*_tmp836;struct Cyc_Absyn_Exp*_tmp838;struct Cyc_Absyn_Stmt*_tmp839;struct Cyc_Absyn_Stmt*_tmp83A;struct Cyc_Absyn_Exp*_tmp83C;struct Cyc_Absyn_Stmt*_tmp83D;struct Cyc_Absyn_Stmt*_tmp83F;struct Cyc_Absyn_Stmt*_tmp841;struct Cyc_Absyn_Stmt*_tmp843;struct Cyc_Absyn_Exp*_tmp845;struct Cyc_Absyn_Exp*_tmp846;struct Cyc_Absyn_Exp*_tmp847;struct Cyc_Absyn_Stmt*_tmp848;struct Cyc_Absyn_Exp*_tmp84A;struct Cyc_List_List*_tmp84B;struct Cyc_List_List*_tmp84D;struct Cyc_Absyn_Switch_clause**_tmp84E;struct Cyc_Absyn_Decl*_tmp850;struct Cyc_Absyn_Stmt*_tmp851;struct _dyneither_ptr*_tmp853;struct Cyc_Absyn_Stmt*_tmp854;struct Cyc_Absyn_Stmt*_tmp856;struct Cyc_Absyn_Exp*_tmp857;struct Cyc_Absyn_Stmt*_tmp859;struct Cyc_List_List*_tmp85A;struct Cyc_Absyn_Exp*_tmp85C;_LL367: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp82F=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp82E;if(_tmp82F->tag != 0)goto _LL369;}_LL368:
# 4296
 return;_LL369: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp830=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp82E;if(_tmp830->tag != 1)goto _LL36B;else{_tmp831=_tmp830->f1;}}_LL36A:
# 4298
 Cyc_Toc_exp_to_c(nv,_tmp831);
return;_LL36B: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp832=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp82E;if(_tmp832->tag != 2)goto _LL36D;else{_tmp833=_tmp832->f1;_tmp834=_tmp832->f2;}}_LL36C:
# 4301
 Cyc_Toc_stmt_to_c(nv,_tmp833);
s=_tmp834;
continue;_LL36D: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp835=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp82E;if(_tmp835->tag != 3)goto _LL36F;else{_tmp836=_tmp835->f1;}}_LL36E: {
# 4305
void*topt=0;
if(_tmp836 != 0){
topt=Cyc_Toc_typ_to_c((void*)_check_null(_tmp836->topt));
Cyc_Toc_exp_to_c(nv,_tmp836);}
# 4311
if(s->try_depth > 0){
if(topt != 0){
struct _tuple1*_tmp85D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp85E=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp85D,0),0);
s->r=(Cyc_Absyn_declare_stmt(_tmp85D,topt,_tmp836,
Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(s->try_depth),_tmp85E,0),0))->r;}else{
# 4319
Cyc_Toc_do_npop_before(s->try_depth,s);}}
# 4321
return;}_LL36F: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp837=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp82E;if(_tmp837->tag != 4)goto _LL371;else{_tmp838=_tmp837->f1;_tmp839=_tmp837->f2;_tmp83A=_tmp837->f3;}}_LL370:
# 4323
 Cyc_Toc_exp_to_c(nv,_tmp838);
Cyc_Toc_stmt_to_c(nv,_tmp839);
s=_tmp83A;
continue;_LL371: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp83B=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp82E;if(_tmp83B->tag != 5)goto _LL373;else{_tmp83C=(_tmp83B->f1).f1;_tmp83D=_tmp83B->f2;}}_LL372:
# 4328
 Cyc_Toc_exp_to_c(nv,_tmp83C);{
struct _RegionHandle _tmp85F=_new_region("temp");struct _RegionHandle*temp=& _tmp85F;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp83D);
# 4332
_npop_handler(0);return;
# 4329
;_pop_region(temp);};_LL373: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp83E=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp82E;if(_tmp83E->tag != 6)goto _LL375;else{_tmp83F=_tmp83E->f1;}}_LL374: {
# 4334
struct _dyneither_ptr**_tmp861;struct Cyc_Toc_Env*_tmp860=nv;_tmp861=_tmp860->break_lab;
if(_tmp861 != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp861,0);{
# 4338
int dest_depth=_tmp83F == 0?0: _tmp83F->try_depth;
Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);
return;};}_LL375: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp840=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp82E;if(_tmp840->tag != 7)goto _LL377;else{_tmp841=_tmp840->f1;}}_LL376: {
# 4342
struct _dyneither_ptr**_tmp863;struct Cyc_Toc_Env*_tmp862=nv;_tmp863=_tmp862->continue_lab;
if(_tmp863 != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp863,0);
_tmp843=_tmp841;goto _LL378;}_LL377: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp842=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp82E;if(_tmp842->tag != 8)goto _LL379;else{_tmp843=_tmp842->f2;}}_LL378:
# 4348
 Cyc_Toc_do_npop_before(s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp843))->try_depth,s);
return;_LL379: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp844=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp82E;if(_tmp844->tag != 9)goto _LL37B;else{_tmp845=_tmp844->f1;_tmp846=(_tmp844->f2).f1;_tmp847=(_tmp844->f3).f1;_tmp848=_tmp844->f4;}}_LL37A:
# 4352
 Cyc_Toc_exp_to_c(nv,_tmp845);Cyc_Toc_exp_to_c(nv,_tmp846);Cyc_Toc_exp_to_c(nv,_tmp847);{
struct _RegionHandle _tmp864=_new_region("temp");struct _RegionHandle*temp=& _tmp864;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp848);
# 4356
_npop_handler(0);return;
# 4353
;_pop_region(temp);};_LL37B: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp849=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp82E;if(_tmp849->tag != 10)goto _LL37D;else{_tmp84A=_tmp849->f1;_tmp84B=_tmp849->f2;}}_LL37C:
# 4358
 Cyc_Toc_xlate_switch(nv,s,_tmp84A,_tmp84B);
return;_LL37D: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp84C=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp82E;if(_tmp84C->tag != 11)goto _LL37F;else{_tmp84D=_tmp84C->f1;_tmp84E=_tmp84C->f2;}}_LL37E: {
# 4361
struct Cyc_Toc_FallthruInfo*_tmp866;struct Cyc_Toc_Env*_tmp865=nv;_tmp866=_tmp865->fallthru_info;
if(_tmp866 == 0){
const char*_tmpD8E;void*_tmpD8D;(_tmpD8D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD8E="fallthru in unexpected place",_tag_dyneither(_tmpD8E,sizeof(char),29))),_tag_dyneither(_tmpD8D,sizeof(void*),0)));}{
struct Cyc_Toc_FallthruInfo _tmp869=*_tmp866;struct _dyneither_ptr*_tmp86B;struct Cyc_List_List*_tmp86C;struct Cyc_Dict_Dict _tmp86D;struct Cyc_Toc_FallthruInfo _tmp86A=_tmp869;_tmp86B=_tmp86A.label;_tmp86C=_tmp86A.binders;_tmp86D=_tmp86A.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp86B,0);
# 4367
Cyc_Toc_do_npop_before(s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp84E)))->body)->try_depth,s2);{
struct Cyc_List_List*_tmp86E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp86C);
struct Cyc_List_List*_tmp86F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp84D);
for(0;_tmp86E != 0;(_tmp86E=_tmp86E->tl,_tmp86F=_tmp86F->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp86F))->hd);
s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp86D,(struct _tuple1*)_tmp86E->hd),(struct Cyc_Absyn_Exp*)_tmp86F->hd,0),s2,0);}
# 4375
s->r=s2->r;
return;};};};}_LL37F: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp84F=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp82E;if(_tmp84F->tag != 12)goto _LL381;else{_tmp850=_tmp84F->f1;_tmp851=_tmp84F->f2;}}_LL380:
# 4381
{void*_tmp870=_tmp850->r;void*_tmp871=_tmp870;struct Cyc_Absyn_Vardecl*_tmp873;struct Cyc_Absyn_Pat*_tmp875;struct Cyc_Absyn_Exp*_tmp876;struct Cyc_List_List*_tmp878;struct Cyc_Absyn_Fndecl*_tmp87A;struct Cyc_Absyn_Tvar*_tmp87C;struct Cyc_Absyn_Vardecl*_tmp87D;int _tmp87E;struct Cyc_Absyn_Exp*_tmp87F;_LL38A: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp872=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp871;if(_tmp872->tag != 0)goto _LL38C;else{_tmp873=_tmp872->f1;}}_LL38B: {
# 4383
struct _RegionHandle _tmp880=_new_region("temp");struct _RegionHandle*temp=& _tmp880;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpD91;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD90;struct Cyc_Toc_Env*_tmp881=Cyc_Toc_add_varmap(temp,nv,_tmp873->name,
Cyc_Absyn_varb_exp(_tmp873->name,(void*)((_tmpD90=_cycalloc(sizeof(*_tmpD90)),((_tmpD90[0]=((_tmpD91.tag=4,((_tmpD91.f1=_tmp873,_tmpD91)))),_tmpD90)))),0));
Cyc_Toc_local_decl_to_c(_tmp881,_tmp881,_tmp873,_tmp851);}
# 4388
_npop_handler(0);goto _LL389;
# 4383
;_pop_region(temp);}_LL38C: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp874=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp871;if(_tmp874->tag != 2)goto _LL38E;else{_tmp875=_tmp874->f1;_tmp876=_tmp874->f3;}}_LL38D:
# 4392
{void*_tmp884=_tmp875->r;void*_tmp885=_tmp884;struct Cyc_Absyn_Vardecl*_tmp887;_LL397: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp886=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp885;if(_tmp886->tag != 1)goto _LL399;else{_tmp887=_tmp886->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp888=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp886->f2)->r;if(_tmp888->tag != 0)goto _LL399;};}}_LL398: {
# 4394
struct _tuple1*old_name=_tmp887->name;
struct _tuple1*new_name=Cyc_Toc_temp_var();
_tmp887->name=new_name;
_tmp887->initializer=_tmp876;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD94;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD93;_tmp850->r=(void*)((_tmpD93=_cycalloc(sizeof(*_tmpD93)),((_tmpD93[0]=((_tmpD94.tag=0,((_tmpD94.f1=_tmp887,_tmpD94)))),_tmpD93))));}{
struct _RegionHandle _tmp88B=_new_region("temp");struct _RegionHandle*temp=& _tmp88B;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpD97;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD96;struct Cyc_Toc_Env*_tmp88C=
Cyc_Toc_add_varmap(temp,nv,old_name,
Cyc_Absyn_varb_exp(new_name,(void*)((_tmpD96=_cycalloc(sizeof(*_tmpD96)),((_tmpD96[0]=((_tmpD97.tag=4,((_tmpD97.f1=_tmp887,_tmpD97)))),_tmpD96)))),0));
Cyc_Toc_local_decl_to_c(_tmp88C,nv,_tmp887,_tmp851);}
# 4405
_npop_handler(0);goto _LL396;
# 4399
;_pop_region(temp);};}_LL399:;_LL39A:
# 4410
 s->r=(Cyc_Toc_letdecl_to_c(nv,_tmp875,(void*)_check_null(_tmp876->topt),_tmp876,_tmp851))->r;
goto _LL396;_LL396:;}
# 4413
goto _LL389;_LL38E: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp877=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp871;if(_tmp877->tag != 3)goto _LL390;else{_tmp878=_tmp877->f1;}}_LL38F: {
# 4426 "toc.cyc"
struct Cyc_List_List*_tmp88F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp878);
if(_tmp88F == 0){
const char*_tmpD9A;void*_tmpD99;(_tmpD99=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD9A="empty Letv_d",_tag_dyneither(_tmpD9A,sizeof(char),13))),_tag_dyneither(_tmpD99,sizeof(void*),0)));}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD9D;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD9C;_tmp850->r=(void*)((_tmpD9C=_cycalloc(sizeof(*_tmpD9C)),((_tmpD9C[0]=((_tmpD9D.tag=0,((_tmpD9D.f1=(struct Cyc_Absyn_Vardecl*)_tmp88F->hd,_tmpD9D)))),_tmpD9C))));}
_tmp88F=_tmp88F->tl;
for(0;_tmp88F != 0;_tmp88F=_tmp88F->tl){
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpDA0;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD9F;struct Cyc_Absyn_Decl*_tmp894=Cyc_Absyn_new_decl((void*)((_tmpD9F=_cycalloc(sizeof(*_tmpD9F)),((_tmpD9F[0]=((_tmpDA0.tag=0,((_tmpDA0.f1=(struct Cyc_Absyn_Vardecl*)_tmp88F->hd,_tmpDA0)))),_tmpD9F)))),0);
s->r=(Cyc_Absyn_decl_stmt(_tmp894,Cyc_Absyn_new_stmt(s->r,0),0))->r;}
# 4435
Cyc_Toc_stmt_to_c(nv,s);
goto _LL389;}_LL390: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp879=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp871;if(_tmp879->tag != 1)goto _LL392;else{_tmp87A=_tmp879->f1;}}_LL391: {
# 4438
struct _tuple1*_tmp897=_tmp87A->name;
struct _RegionHandle _tmp898=_new_region("temp");struct _RegionHandle*temp=& _tmp898;_push_region(temp);{
struct Cyc_Toc_Env*_tmp899=Cyc_Toc_add_varmap(temp,nv,_tmp87A->name,Cyc_Absyn_var_exp(_tmp897,0));
Cyc_Toc_fndecl_to_c(_tmp899,_tmp87A,0);
Cyc_Toc_stmt_to_c(_tmp899,_tmp851);}
# 4444
_npop_handler(0);goto _LL389;
# 4439
;_pop_region(temp);}_LL392: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp87B=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp871;if(_tmp87B->tag != 4)goto _LL394;else{_tmp87C=_tmp87B->f1;_tmp87D=_tmp87B->f2;_tmp87E=_tmp87B->f3;_tmp87F=_tmp87B->f4;}}_LL393: {
# 4446
struct Cyc_Absyn_Stmt*_tmp89A=_tmp851;
# 4454
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple1*rh_var=Cyc_Toc_temp_var();
struct _tuple1*x_var=_tmp87D->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);
# 4461
struct _RegionHandle _tmp89B=_new_region("temp");struct _RegionHandle*temp=& _tmp89B;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(temp,nv,x_var,x_exp),_tmp89A);
# 4464
if(Cyc_Absyn_no_regions)
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_uint_exp(0,0),_tmp89A,0))->r;else{
if((unsigned int)_tmp87F){
Cyc_Toc_exp_to_c(nv,_tmp87F);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(Cyc_Absyn_aggrarrow_exp(_tmp87F,_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"h",sizeof(char),2),0),0);
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,_tmp89A,0))->r;};}else{
# 4472
struct Cyc_Absyn_Exp*_tmpDA3[1];struct Cyc_Absyn_Exp*_tmpDA2[1];struct Cyc_List_List*_tmpDA1;s->r=(
Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,(
(_tmpDA1=_cycalloc(sizeof(*_tmpDA1)),((_tmpDA1->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0),((_tmpDA1->tl=0,_tmpDA1)))))),0),
# 4477
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpDA2[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpDA2,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),
Cyc_Absyn_seq_stmt(_tmp89A,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpDA3[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpDA3,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}}
# 4483
_npop_handler(0);return;
# 4461
;_pop_region(temp);}_LL394:;_LL395: {
# 4484
const char*_tmpDA6;void*_tmpDA5;(_tmpDA5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpDA6="bad nested declaration within function",_tag_dyneither(_tmpDA6,sizeof(char),39))),_tag_dyneither(_tmpDA5,sizeof(void*),0)));}_LL389:;}
# 4486
return;_LL381: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp852=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp82E;if(_tmp852->tag != 13)goto _LL383;else{_tmp853=_tmp852->f1;_tmp854=_tmp852->f2;}}_LL382:
# 4488
 s=_tmp854;continue;_LL383: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp855=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp82E;if(_tmp855->tag != 14)goto _LL385;else{_tmp856=_tmp855->f1;_tmp857=(_tmp855->f2).f1;}}_LL384: {
# 4490
struct _RegionHandle _tmp8A2=_new_region("temp");struct _RegionHandle*temp=& _tmp8A2;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp856);
Cyc_Toc_exp_to_c(nv,_tmp857);
# 4494
_npop_handler(0);return;
# 4490
;_pop_region(temp);}_LL385: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp858=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp82E;if(_tmp858->tag != 15)goto _LL387;else{_tmp859=_tmp858->f1;_tmp85A=_tmp858->f2;}}_LL386: {
# 4511 "toc.cyc"
struct _tuple1*h_var=Cyc_Toc_temp_var();
struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
# 4522
e_exp->topt=e_typ;{
struct _RegionHandle _tmp8A3=_new_region("temp");struct _RegionHandle*temp=& _tmp8A3;_push_region(temp);{
struct Cyc_Toc_Env*_tmp8A4=Cyc_Toc_add_varmap(temp,nv,e_var,e_exp);
# 4526
Cyc_Toc_stmt_to_c(_tmp8A4,_tmp859);{
struct Cyc_Absyn_Stmt*_tmp8A5=Cyc_Absyn_seq_stmt(_tmp859,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
# 4532
struct _tuple1*_tmp8A6=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp8A7=Cyc_Absyn_var_exp(_tmp8A6,0);
struct Cyc_Absyn_Vardecl*_tmp8A8=Cyc_Absyn_new_vardecl(_tmp8A6,Cyc_Absyn_exn_typ(),0);
_tmp8A7->topt=Cyc_Absyn_exn_typ();{
struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpDB0;struct Cyc_Absyn_Pat*_tmpDAF;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpDAE;struct Cyc_Absyn_Pat*_tmpDAD;struct Cyc_Absyn_Pat*_tmp8A9=(_tmpDAD=_cycalloc(sizeof(*_tmpDAD)),((_tmpDAD->r=(void*)((_tmpDB0=_cycalloc(sizeof(*_tmpDB0)),((_tmpDB0[0]=((_tmpDAE.tag=1,((_tmpDAE.f1=_tmp8A8,((_tmpDAE.f2=((_tmpDAF=_cycalloc(sizeof(*_tmpDAF)),((_tmpDAF->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpDAF->topt=Cyc_Absyn_exn_typ(),((_tmpDAF->loc=0,_tmpDAF)))))))),_tmpDAE)))))),_tmpDB0)))),((_tmpDAD->topt=Cyc_Absyn_exn_typ(),((_tmpDAD->loc=0,_tmpDAD)))))));
struct Cyc_Absyn_Exp*_tmp8AA=Cyc_Absyn_rethrow_exp(_tmp8A7,0);
_tmp8AA->topt=(void*)& Cyc_Absyn_VoidType_val;{
struct Cyc_Absyn_Stmt*_tmp8AB=Cyc_Absyn_exp_stmt(_tmp8AA,0);
struct Cyc_Core_Opt*_tmpDBF;struct _tuple35*_tmpDBE;struct Cyc_Absyn_Vardecl**_tmpDBD;struct Cyc_List_List*_tmpDBC;struct Cyc_Absyn_Switch_clause*_tmpDBB;struct Cyc_Absyn_Switch_clause*_tmp8AC=
(_tmpDBB=_cycalloc(sizeof(*_tmpDBB)),((_tmpDBB->pattern=_tmp8A9,((_tmpDBB->pat_vars=((_tmpDBF=_cycalloc(sizeof(*_tmpDBF)),((_tmpDBF->v=((_tmpDBC=_cycalloc(sizeof(*_tmpDBC)),((_tmpDBC->hd=((_tmpDBE=_cycalloc(sizeof(*_tmpDBE)),((_tmpDBE->f1=((_tmpDBD=_cycalloc(sizeof(*_tmpDBD)),((_tmpDBD[0]=_tmp8A8,_tmpDBD)))),((_tmpDBE->f2=_tmp8A7,_tmpDBE)))))),((_tmpDBC->tl=0,_tmpDBC)))))),_tmpDBF)))),((_tmpDBB->where_clause=0,((_tmpDBB->body=_tmp8AB,((_tmpDBB->loc=0,_tmpDBB)))))))))));
# 4543
struct Cyc_List_List*_tmpDC0;struct Cyc_Absyn_Stmt*_tmp8AD=Cyc_Absyn_switch_stmt(e_exp,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp85A,(
(_tmpDC0=_cycalloc(sizeof(*_tmpDC0)),((_tmpDC0->hd=_tmp8AC,((_tmpDC0->tl=0,_tmpDC0))))))),0);
# 4547
Cyc_Toc_stmt_to_c(_tmp8A4,_tmp8AD);{
# 4550
struct Cyc_List_List*_tmpDC1;struct Cyc_Absyn_Exp*_tmp8AE=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,(
(_tmpDC1=_cycalloc(sizeof(*_tmpDC1)),((_tmpDC1->hd=Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0),((_tmpDC1->tl=0,_tmpDC1)))))),0);
# 4554
struct Cyc_List_List*_tmpDC2;struct Cyc_Absyn_Stmt*_tmp8AF=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,(
(_tmpDC2=_cycalloc(sizeof(*_tmpDC2)),((_tmpDC2->hd=Cyc_Absyn_address_exp(h_exp,0),((_tmpDC2->tl=0,_tmpDC2)))))),0),0);
# 4558
struct Cyc_Absyn_Exp*_tmp8B0=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0);
struct Cyc_Absyn_Exp*_tmp8B1=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0);
s->r=(
Cyc_Absyn_declare_stmt(h_var,h_typ,0,
Cyc_Absyn_seq_stmt(_tmp8AF,
Cyc_Absyn_declare_stmt(was_thrown_var,was_thrown_typ,_tmp8B0,
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp8AE,
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp8B1,0),
Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0),_tmp8A5,
# 4569
Cyc_Absyn_declare_stmt(e_var,e_typ,
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp8AD,0),0),0),0),0),0))->r;};};};};}
# 4573
_npop_handler(0);return;
# 4523
;_pop_region(temp);};}_LL387: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp85B=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp82E;if(_tmp85B->tag != 16)goto _LL366;else{_tmp85C=_tmp85B->f1;}}_LL388:
# 4575
 if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{
# 4578
Cyc_Toc_exp_to_c(nv,_tmp85C);{
struct Cyc_List_List*_tmpDC3;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((_tmpDC3=_cycalloc(sizeof(*_tmpDC3)),((_tmpDC3->hd=_tmp85C,((_tmpDC3->tl=0,_tmpDC3)))))),0));};}
# 4581
return;_LL366:;}}
# 4590
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->requires_clause=0;
f->ensures_clause=0;
f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp8BF=_new_region("frgn");struct _RegionHandle*frgn=& _tmp8BF;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp8C0=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp8C1=f->args;for(0;_tmp8C1 != 0;_tmp8C1=_tmp8C1->tl){
struct _tuple1*_tmpDC4;struct _tuple1*_tmp8C2=(_tmpDC4=_cycalloc(sizeof(*_tmpDC4)),((_tmpDC4->f1=Cyc_Absyn_Loc_n,((_tmpDC4->f2=(*((struct _tuple9*)_tmp8C1->hd)).f1,_tmpDC4)))));
(*((struct _tuple9*)_tmp8C1->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp8C1->hd)).f3);
_tmp8C0=Cyc_Toc_add_varmap(frgn,_tmp8C0,_tmp8C2,Cyc_Absyn_var_exp(_tmp8C2,0));}}
# 4607
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}
# 4611
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp8C4=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));struct _dyneither_ptr*_tmp8C6;struct Cyc_Absyn_Tqual _tmp8C7;void*_tmp8C8;int _tmp8C9;struct Cyc_Absyn_VarargInfo _tmp8C5=_tmp8C4;_tmp8C6=_tmp8C5.name;_tmp8C7=_tmp8C5.tq;_tmp8C8=_tmp8C5.type;_tmp8C9=_tmp8C5.inject;{
void*_tmp8CA=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp8C8,(void*)& Cyc_Absyn_HeapRgn_val,_tmp8C7,Cyc_Absyn_false_conref));
struct _tuple1*_tmpDC5;struct _tuple1*_tmp8CB=(_tmpDC5=_cycalloc(sizeof(*_tmpDC5)),((_tmpDC5->f1=Cyc_Absyn_Loc_n,((_tmpDC5->f2=(struct _dyneither_ptr*)_check_null(_tmp8C6),_tmpDC5)))));
{struct _tuple9*_tmpDC8;struct Cyc_List_List*_tmpDC7;f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpDC7=_cycalloc(sizeof(*_tmpDC7)),((_tmpDC7->hd=((_tmpDC8=_cycalloc(sizeof(*_tmpDC8)),((_tmpDC8->f1=_tmp8C6,((_tmpDC8->f2=_tmp8C7,((_tmpDC8->f3=_tmp8CA,_tmpDC8)))))))),((_tmpDC7->tl=0,_tmpDC7)))))));}
_tmp8C0=Cyc_Toc_add_varmap(frgn,_tmp8C0,_tmp8CB,Cyc_Absyn_var_exp(_tmp8CB,0));
f->cyc_varargs=0;};}
# 4620
{struct Cyc_List_List*_tmp8CF=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp8CF != 0;_tmp8CF=_tmp8CF->tl){
((struct Cyc_Absyn_Vardecl*)_tmp8CF->hd)->type=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp8CF->hd)->type);}}
# 4623
Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp8C0),f->body);}
# 4599
;_pop_region(frgn);};}
# 4628
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
switch(s){case Cyc_Absyn_Abstract: _LL39B:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL39C:
 return Cyc_Absyn_Extern;default: _LL39D:
 return s;}}struct _tuple36{struct Cyc_Absyn_Aggrdecl*f1;int f2;};struct _tuple37{struct Cyc_Toc_TocState*f1;struct _tuple36*f2;};
# 4645 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple37*env){
# 4648
struct _tuple37 _tmp8D0=*env;struct Cyc_Toc_TocState*_tmp8D2;struct Cyc_Absyn_Aggrdecl*_tmp8D3;int _tmp8D4;struct _tuple37 _tmp8D1=_tmp8D0;_tmp8D2=_tmp8D1.f1;_tmp8D3=(_tmp8D1.f2)->f1;_tmp8D4=(_tmp8D1.f2)->f2;{
struct _tuple1*_tmp8D5=_tmp8D3->name;
struct Cyc_Toc_TocState _tmp8D6=*_tmp8D2;struct Cyc_Dict_Dict*_tmp8D8;struct Cyc_Toc_TocState _tmp8D7=_tmp8D6;_tmp8D8=_tmp8D7.aggrs_so_far;{
int seen_defn_before;
struct _tuple18**_tmp8D9=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp8D8,_tmp8D5);
if(_tmp8D9 == 0){
seen_defn_before=0;{
struct _tuple18*v;
if(_tmp8D3->kind == Cyc_Absyn_StructA){
struct _tuple18*_tmpDC9;v=((_tmpDC9=_region_malloc(d,sizeof(*_tmpDC9)),((_tmpDC9->f1=_tmp8D3,((_tmpDC9->f2=Cyc_Absyn_strctq(_tmp8D5),_tmpDC9))))));}else{
# 4659
struct _tuple18*_tmpDCA;v=((_tmpDCA=_region_malloc(d,sizeof(*_tmpDCA)),((_tmpDCA->f1=_tmp8D3,((_tmpDCA->f2=Cyc_Absyn_unionq_typ(_tmp8D5),_tmpDCA))))));}
*_tmp8D8=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp8D8,_tmp8D5,v);};}else{
# 4662
struct _tuple18*_tmp8DC=*_tmp8D9;struct Cyc_Absyn_Aggrdecl*_tmp8DE;void*_tmp8DF;struct _tuple18*_tmp8DD=_tmp8DC;_tmp8DE=_tmp8DD->f1;_tmp8DF=_tmp8DD->f2;
if(_tmp8DE->impl == 0){
{struct _tuple18*_tmpDCB;*_tmp8D8=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp8D8,_tmp8D5,((_tmpDCB=_region_malloc(d,sizeof(*_tmpDCB)),((_tmpDCB->f1=_tmp8D3,((_tmpDCB->f2=_tmp8DF,_tmpDCB)))))));}
seen_defn_before=0;}else{
# 4667
seen_defn_before=1;}}{
# 4669
struct Cyc_Absyn_Aggrdecl*_tmpDCC;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpDCC=_cycalloc(sizeof(*_tmpDCC)),((_tmpDCC->kind=_tmp8D3->kind,((_tmpDCC->sc=Cyc_Absyn_Public,((_tmpDCC->name=_tmp8D3->name,((_tmpDCC->tvs=0,((_tmpDCC->impl=0,((_tmpDCC->attributes=_tmp8D3->attributes,_tmpDCC)))))))))))));
# 4675
if(_tmp8D3->impl != 0  && !seen_defn_before){
{struct Cyc_Absyn_AggrdeclImpl*_tmpDCD;new_ad->impl=((_tmpDCD=_cycalloc(sizeof(*_tmpDCD)),((_tmpDCD->exist_vars=0,((_tmpDCD->rgn_po=0,((_tmpDCD->fields=0,((_tmpDCD->tagged=0,_tmpDCD))))))))));}{
# 4680
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp8E2=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp8D3->impl))->fields;for(0;_tmp8E2 != 0;_tmp8E2=_tmp8E2->tl){
# 4684
struct Cyc_Absyn_Aggrfield*_tmp8E3=(struct Cyc_Absyn_Aggrfield*)_tmp8E2->hd;
void*_tmp8E4=_tmp8E3->type;
struct Cyc_List_List*_tmp8E5=_tmp8E3->attributes;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp8E4)) && (
_tmp8D3->kind == Cyc_Absyn_StructA  && _tmp8E2->tl == 0  || _tmp8D3->kind == Cyc_Absyn_UnionA)){
# 4698 "toc.cyc"
void*_tmp8E6=Cyc_Tcutil_compress(_tmp8E4);void*_tmp8E7=_tmp8E6;void*_tmp8E9;struct Cyc_Absyn_Tqual _tmp8EA;union Cyc_Absyn_Constraint*_tmp8EB;unsigned int _tmp8EC;_LL3A0: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8E8=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8E7;if(_tmp8E8->tag != 8)goto _LL3A2;else{_tmp8E9=(_tmp8E8->f1).elt_type;_tmp8EA=(_tmp8E8->f1).tq;_tmp8EB=(_tmp8E8->f1).zero_term;_tmp8EC=(_tmp8E8->f1).zt_loc;}}_LL3A1:
# 4701
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpDD3;struct Cyc_Absyn_ArrayInfo _tmpDD2;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpDD1;_tmp8E4=(void*)((_tmpDD1=_cycalloc(sizeof(*_tmpDD1)),((_tmpDD1[0]=((_tmpDD3.tag=8,((_tmpDD3.f1=((_tmpDD2.elt_type=_tmp8E9,((_tmpDD2.tq=_tmp8EA,((_tmpDD2.num_elts=Cyc_Absyn_uint_exp(0,0),((_tmpDD2.zero_term=_tmp8EB,((_tmpDD2.zt_loc=_tmp8EC,_tmpDD2)))))))))),_tmpDD3)))),_tmpDD1))));}
goto _LL39F;_LL3A2:;_LL3A3:
# 4704
{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpDD9;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpDD8;struct Cyc_List_List*_tmpDD7;_tmp8E5=((_tmpDD7=_cycalloc(sizeof(*_tmpDD7)),((_tmpDD7->hd=(void*)((_tmpDD9=_cycalloc(sizeof(*_tmpDD9)),((_tmpDD9[0]=((_tmpDD8.tag=6,((_tmpDD8.f1=0,_tmpDD8)))),_tmpDD9)))),((_tmpDD7->tl=_tmp8E5,_tmpDD7))))));}{
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpDDF;struct Cyc_Absyn_ArrayInfo _tmpDDE;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpDDD;_tmp8E4=(void*)((_tmpDDD=_cycalloc(sizeof(*_tmpDDD)),((_tmpDDD[0]=((_tmpDDF.tag=8,((_tmpDDF.f1=((_tmpDDE.elt_type=Cyc_Absyn_void_star_typ(),((_tmpDDE.tq=
Cyc_Absyn_empty_tqual(0),((_tmpDDE.num_elts=
Cyc_Absyn_uint_exp(0,0),((_tmpDDE.zero_term=
((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0),((_tmpDDE.zt_loc=0,_tmpDDE)))))))))),_tmpDDF)))),_tmpDDD))));};_LL39F:;}{
# 4711
struct Cyc_Absyn_Aggrfield*_tmpDE0;struct Cyc_Absyn_Aggrfield*_tmp8F6=(_tmpDE0=_cycalloc(sizeof(*_tmpDE0)),((_tmpDE0->name=_tmp8E3->name,((_tmpDE0->tq=Cyc_Toc_mt_tq,((_tmpDE0->type=
# 4713
Cyc_Toc_typ_to_c(_tmp8E4),((_tmpDE0->width=_tmp8E3->width,((_tmpDE0->attributes=_tmp8E5,((_tmpDE0->requires_clause=0,_tmpDE0)))))))))))));
# 4721
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp8D3->impl))->tagged){
void*_tmp8F7=_tmp8F6->type;
struct _dyneither_ptr*_tmp8F8=_tmp8F6->name;
const char*_tmpDE5;void*_tmpDE4[2];struct Cyc_String_pa_PrintArg_struct _tmpDE3;struct Cyc_String_pa_PrintArg_struct _tmpDE2;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpDE2.tag=0,((_tmpDE2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp8F8),((_tmpDE3.tag=0,((_tmpDE3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp8D3->name).f2),((_tmpDE4[0]=& _tmpDE3,((_tmpDE4[1]=& _tmpDE2,Cyc_aprintf(((_tmpDE5="_union_%s_%s",_tag_dyneither(_tmpDE5,sizeof(char),13))),_tag_dyneither(_tmpDE4,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpDE6;struct _dyneither_ptr*str=(_tmpDE6=_cycalloc(sizeof(*_tmpDE6)),((_tmpDE6[0]=s,_tmpDE6)));
struct Cyc_Absyn_Aggrfield*_tmpDE7;struct Cyc_Absyn_Aggrfield*_tmp8F9=(_tmpDE7=_cycalloc(sizeof(*_tmpDE7)),((_tmpDE7->name=Cyc_Toc_val_sp,((_tmpDE7->tq=Cyc_Toc_mt_tq,((_tmpDE7->type=_tmp8F7,((_tmpDE7->width=0,((_tmpDE7->attributes=0,((_tmpDE7->requires_clause=0,_tmpDE7)))))))))))));
struct Cyc_Absyn_Aggrfield*_tmpDE8;struct Cyc_Absyn_Aggrfield*_tmp8FA=(_tmpDE8=_cycalloc(sizeof(*_tmpDE8)),((_tmpDE8->name=Cyc_Toc_tag_sp,((_tmpDE8->tq=Cyc_Toc_mt_tq,((_tmpDE8->type=Cyc_Absyn_sint_typ,((_tmpDE8->width=0,((_tmpDE8->attributes=0,((_tmpDE8->requires_clause=0,_tmpDE8)))))))))))));
struct Cyc_Absyn_Aggrfield*_tmpDE9[2];struct Cyc_List_List*_tmp8FB=(_tmpDE9[1]=_tmp8F9,((_tmpDE9[0]=_tmp8FA,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpDE9,sizeof(struct Cyc_Absyn_Aggrfield*),2)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpDEE;struct _tuple1*_tmpDED;struct Cyc_Absyn_Aggrdecl*_tmpDEC;struct Cyc_Absyn_Aggrdecl*_tmp8FC=(_tmpDEC=_cycalloc(sizeof(*_tmpDEC)),((_tmpDEC->kind=Cyc_Absyn_StructA,((_tmpDEC->sc=Cyc_Absyn_Public,((_tmpDEC->name=(
(_tmpDED=_cycalloc(sizeof(*_tmpDED)),((_tmpDED->f1=Cyc_Absyn_Loc_n,((_tmpDED->f2=str,_tmpDED)))))),((_tmpDEC->tvs=0,((_tmpDEC->impl=(
(_tmpDEE=_cycalloc(sizeof(*_tmpDEE)),((_tmpDEE->exist_vars=0,((_tmpDEE->rgn_po=0,((_tmpDEE->fields=_tmp8FB,((_tmpDEE->tagged=0,_tmpDEE)))))))))),((_tmpDEC->attributes=0,_tmpDEC)))))))))))));
# 4733
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpDF4;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpDF3;struct Cyc_List_List*_tmpDF2;Cyc_Toc_result_decls=((_tmpDF2=_cycalloc(sizeof(*_tmpDF2)),((_tmpDF2->hd=Cyc_Absyn_new_decl((void*)((_tmpDF4=_cycalloc(sizeof(*_tmpDF4)),((_tmpDF4[0]=((_tmpDF3.tag=5,((_tmpDF3.f1=_tmp8FC,_tmpDF3)))),_tmpDF4)))),0),((_tmpDF2->tl=Cyc_Toc_result_decls,_tmpDF2))))));}
_tmp8F6->type=Cyc_Absyn_strct(str);}{
# 4736
struct Cyc_List_List*_tmpDF5;new_fields=((_tmpDF5=_cycalloc(sizeof(*_tmpDF5)),((_tmpDF5->hd=_tmp8F6,((_tmpDF5->tl=new_fields,_tmpDF5))))));};};}}
# 4738
(new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);};}
# 4740
if(_tmp8D4){
struct Cyc_Absyn_Decl*_tmpDFF;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpDFE;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpDFD;struct Cyc_List_List*_tmpDFC;Cyc_Toc_result_decls=((_tmpDFC=_cycalloc(sizeof(*_tmpDFC)),((_tmpDFC->hd=((_tmpDFF=_cycalloc(sizeof(*_tmpDFF)),((_tmpDFF->r=(void*)((_tmpDFD=_cycalloc(sizeof(*_tmpDFD)),((_tmpDFD[0]=((_tmpDFE.tag=5,((_tmpDFE.f1=new_ad,_tmpDFE)))),_tmpDFD)))),((_tmpDFF->loc=0,_tmpDFF)))))),((_tmpDFC->tl=Cyc_Toc_result_decls,_tmpDFC))))));}
return 0;};};};}
# 4745
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple36 _tmpE00;struct _tuple36 p=(_tmpE00.f1=ad,((_tmpE00.f2=add_to_decls,_tmpE00)));
((int(*)(struct _tuple36*arg,int(*f)(struct _RegionHandle*,struct _tuple37*)))Cyc_Toc_use_toc_state)(& p,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple38{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 4774 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple38*env){
# 4777
struct _tuple38 _tmp913=*env;struct Cyc_Set_Set**_tmp915;struct Cyc_Absyn_Datatypedecl*_tmp916;struct _tuple38 _tmp914=_tmp913;_tmp915=(_tmp914.f1)->datatypes_so_far;_tmp916=_tmp914.f2;{
struct _tuple1*_tmp917=_tmp916->name;
if(_tmp916->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*_tmp915,_tmp917))
return 0;
*_tmp915=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*_tmp915,_tmp917);
{struct Cyc_List_List*_tmp918=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp916->fields))->v;for(0;_tmp918 != 0;_tmp918=_tmp918->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp918->hd;
# 4785
struct Cyc_List_List*_tmp919=0;
int i=1;
{struct Cyc_List_List*_tmp91A=f->typs;for(0;_tmp91A != 0;(_tmp91A=_tmp91A->tl,i ++)){
struct _dyneither_ptr*_tmp91B=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmpE01;struct Cyc_Absyn_Aggrfield*_tmp91C=(_tmpE01=_cycalloc(sizeof(*_tmpE01)),((_tmpE01->name=_tmp91B,((_tmpE01->tq=(*((struct _tuple11*)_tmp91A->hd)).f1,((_tmpE01->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp91A->hd)).f2),((_tmpE01->width=0,((_tmpE01->attributes=0,((_tmpE01->requires_clause=0,_tmpE01)))))))))))));
struct Cyc_List_List*_tmpE02;_tmp919=((_tmpE02=_cycalloc(sizeof(*_tmpE02)),((_tmpE02->hd=_tmp91C,((_tmpE02->tl=_tmp919,_tmpE02))))));}}
# 4793
{struct Cyc_Absyn_Aggrfield*_tmpE05;struct Cyc_List_List*_tmpE04;_tmp919=((_tmpE04=_cycalloc(sizeof(*_tmpE04)),((_tmpE04->hd=((_tmpE05=_cycalloc(sizeof(*_tmpE05)),((_tmpE05->name=Cyc_Toc_tag_sp,((_tmpE05->tq=Cyc_Toc_mt_tq,((_tmpE05->type=Cyc_Absyn_sint_typ,((_tmpE05->width=0,((_tmpE05->attributes=0,((_tmpE05->requires_clause=0,_tmpE05)))))))))))))),((_tmpE04->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp919),_tmpE04))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpE08;struct Cyc_Absyn_Aggrdecl*_tmpE07;struct Cyc_Absyn_Aggrdecl*_tmp921=
(_tmpE07=_cycalloc(sizeof(*_tmpE07)),((_tmpE07->kind=Cyc_Absyn_StructA,((_tmpE07->sc=Cyc_Absyn_Public,((_tmpE07->name=
Cyc_Toc_collapse_qvars(f->name,_tmp916->name),((_tmpE07->tvs=0,((_tmpE07->impl=(
# 4799
(_tmpE08=_cycalloc(sizeof(*_tmpE08)),((_tmpE08->exist_vars=0,((_tmpE08->rgn_po=0,((_tmpE08->fields=_tmp919,((_tmpE08->tagged=0,_tmpE08)))))))))),((_tmpE07->attributes=0,_tmpE07)))))))))))));
# 4801
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpE0E;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpE0D;struct Cyc_List_List*_tmpE0C;Cyc_Toc_result_decls=((_tmpE0C=_cycalloc(sizeof(*_tmpE0C)),((_tmpE0C->hd=Cyc_Absyn_new_decl((void*)((_tmpE0E=_cycalloc(sizeof(*_tmpE0E)),((_tmpE0E[0]=((_tmpE0D.tag=5,((_tmpE0D.f1=_tmp921,_tmpE0D)))),_tmpE0E)))),0),((_tmpE0C->tl=Cyc_Toc_result_decls,_tmpE0C))))));};}}
# 4803
return 0;};}
# 4806
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple38*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 4825 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple38*env){
# 4828
struct _tuple38 _tmp927=*env;struct Cyc_Toc_TocState*_tmp929;struct Cyc_Absyn_Datatypedecl*_tmp92A;struct _tuple38 _tmp928=_tmp927;_tmp929=_tmp928.f1;_tmp92A=_tmp928.f2;
if(_tmp92A->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp92B=*_tmp929;struct Cyc_Dict_Dict*_tmp92D;struct Cyc_Toc_TocState _tmp92C=_tmp92B;_tmp92D=_tmp92C.xdatatypes_so_far;{
struct _tuple1*_tmp92E=_tmp92A->name;
{struct Cyc_List_List*_tmp92F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp92A->fields))->v;for(0;_tmp92F != 0;_tmp92F=_tmp92F->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp92F->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp930=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0);
void*_tmp931=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,_tmp930,Cyc_Absyn_false_conref,0);
# 4839
int*_tmp932=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp92D,f->name);int*_tmp933=_tmp932;_LL3A5: if(_tmp933 != 0)goto _LL3A7;_LL3A6: {
# 4841
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0);{
# 4845
struct Cyc_Absyn_Vardecl*_tmp934=Cyc_Absyn_new_vardecl(f->name,_tmp931,initopt);
_tmp934->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpE14;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpE13;struct Cyc_List_List*_tmpE12;Cyc_Toc_result_decls=((_tmpE12=_cycalloc(sizeof(*_tmpE12)),((_tmpE12->hd=Cyc_Absyn_new_decl((void*)((_tmpE14=_cycalloc(sizeof(*_tmpE14)),((_tmpE14[0]=((_tmpE13.tag=0,((_tmpE13.f1=_tmp934,_tmpE13)))),_tmpE14)))),0),((_tmpE12->tl=Cyc_Toc_result_decls,_tmpE12))))));}
*_tmp92D=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp92D,f->name,f->sc != Cyc_Absyn_Extern);{
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp938=f->typs;for(0;_tmp938 != 0;(_tmp938=_tmp938->tl,i ++)){
struct Cyc_Int_pa_PrintArg_struct _tmpE1C;void*_tmpE1B[1];const char*_tmpE1A;struct _dyneither_ptr*_tmpE19;struct _dyneither_ptr*_tmp939=(_tmpE19=_cycalloc(sizeof(*_tmpE19)),((_tmpE19[0]=(struct _dyneither_ptr)((_tmpE1C.tag=1,((_tmpE1C.f1=(unsigned long)i,((_tmpE1B[0]=& _tmpE1C,Cyc_aprintf(((_tmpE1A="f%d",_tag_dyneither(_tmpE1A,sizeof(char),4))),_tag_dyneither(_tmpE1B,sizeof(void*),1)))))))),_tmpE19)));
struct Cyc_Absyn_Aggrfield*_tmpE1D;struct Cyc_Absyn_Aggrfield*_tmp93A=(_tmpE1D=_cycalloc(sizeof(*_tmpE1D)),((_tmpE1D->name=_tmp939,((_tmpE1D->tq=(*((struct _tuple11*)_tmp938->hd)).f1,((_tmpE1D->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp938->hd)).f2),((_tmpE1D->width=0,((_tmpE1D->attributes=0,((_tmpE1D->requires_clause=0,_tmpE1D)))))))))))));
struct Cyc_List_List*_tmpE1E;fields=((_tmpE1E=_cycalloc(sizeof(*_tmpE1E)),((_tmpE1E->hd=_tmp93A,((_tmpE1E->tl=fields,_tmpE1E))))));}}
# 4858
{struct Cyc_Absyn_Aggrfield*_tmpE21;struct Cyc_List_List*_tmpE20;fields=((_tmpE20=_cycalloc(sizeof(*_tmpE20)),((_tmpE20->hd=((_tmpE21=_cycalloc(sizeof(*_tmpE21)),((_tmpE21->name=Cyc_Toc_tag_sp,((_tmpE21->tq=Cyc_Toc_mt_tq,((_tmpE21->type=
Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq),((_tmpE21->width=0,((_tmpE21->attributes=0,((_tmpE21->requires_clause=0,_tmpE21)))))))))))))),((_tmpE20->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmpE20))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpE24;struct Cyc_Absyn_Aggrdecl*_tmpE23;struct Cyc_Absyn_Aggrdecl*_tmp943=
(_tmpE23=_cycalloc(sizeof(*_tmpE23)),((_tmpE23->kind=Cyc_Absyn_StructA,((_tmpE23->sc=Cyc_Absyn_Public,((_tmpE23->name=
Cyc_Toc_collapse_qvars(f->name,_tmp92A->name),((_tmpE23->tvs=0,((_tmpE23->impl=(
# 4865
(_tmpE24=_cycalloc(sizeof(*_tmpE24)),((_tmpE24->exist_vars=0,((_tmpE24->rgn_po=0,((_tmpE24->fields=fields,((_tmpE24->tagged=0,_tmpE24)))))))))),((_tmpE23->attributes=0,_tmpE23)))))))))))));
# 4867
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpE2A;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpE29;struct Cyc_List_List*_tmpE28;Cyc_Toc_result_decls=((_tmpE28=_cycalloc(sizeof(*_tmpE28)),((_tmpE28->hd=Cyc_Absyn_new_decl((void*)((_tmpE2A=_cycalloc(sizeof(*_tmpE2A)),((_tmpE2A[0]=((_tmpE29.tag=5,((_tmpE29.f1=_tmp943,_tmpE29)))),_tmpE2A)))),0),((_tmpE28->tl=Cyc_Toc_result_decls,_tmpE28))))));}
# 4869
goto _LL3A4;};};};}_LL3A7: if(_tmp933 == 0)goto _LL3A9;if(*_tmp933 != 0)goto _LL3A9;_LL3A8:
# 4871
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp949=Cyc_Absyn_string_exp(*fn,0);
struct Cyc_Absyn_Vardecl*_tmp94A=Cyc_Absyn_new_vardecl(f->name,_tmp931,_tmp949);
_tmp94A->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpE30;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpE2F;struct Cyc_List_List*_tmpE2E;Cyc_Toc_result_decls=((_tmpE2E=_cycalloc(sizeof(*_tmpE2E)),((_tmpE2E->hd=Cyc_Absyn_new_decl((void*)((_tmpE30=_cycalloc(sizeof(*_tmpE30)),((_tmpE30[0]=((_tmpE2F.tag=0,((_tmpE2F.f1=_tmp94A,_tmpE2F)))),_tmpE30)))),0),((_tmpE2E->tl=Cyc_Toc_result_decls,_tmpE2E))))));}
*_tmp92D=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp92D,f->name,1);}
# 4878
goto _LL3A4;_LL3A9:;_LL3AA:
 goto _LL3A4;_LL3A4:;}}
# 4882
return 0;};};}
# 4885
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple38*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 4890
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4898
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
vd->type=Cyc_Toc_typ_to_c(old_typ);
# 4902
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(body_nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp94E=init->r;void*_tmp94F=_tmp94E;struct Cyc_Absyn_Vardecl*_tmp951;struct Cyc_Absyn_Exp*_tmp952;struct Cyc_Absyn_Exp*_tmp953;int _tmp954;_LL3AC: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp950=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp94F;if(_tmp950->tag != 26)goto _LL3AE;else{_tmp951=_tmp950->f1;_tmp952=_tmp950->f2;_tmp953=_tmp950->f3;_tmp954=_tmp950->f4;}}_LL3AD:
# 4909
 vd->initializer=0;
s->r=(Cyc_Toc_init_comprehension(init_nv,old_typ,Cyc_Absyn_var_exp(vd->name,0),_tmp951,_tmp952,_tmp953,_tmp954,
Cyc_Absyn_new_stmt(s->r,0),0))->r;
goto _LL3AB;_LL3AE:;_LL3AF:
# 4914
 if(vd->sc == Cyc_Absyn_Static){
# 4918
struct _RegionHandle _tmp955=_new_region("temp");struct _RegionHandle*temp=& _tmp955;_push_region(temp);
{struct Cyc_Toc_Env*_tmp956=Cyc_Toc_set_toplevel(temp,init_nv);
Cyc_Toc_exp_to_c(_tmp956,init);}
# 4919
;_pop_region(temp);}else{
# 4923
Cyc_Toc_exp_to_c(init_nv,init);}
goto _LL3AB;_LL3AB:;}else{
# 4928
void*_tmp957=Cyc_Tcutil_compress(old_typ);void*_tmp958=_tmp957;void*_tmp95A;struct Cyc_Absyn_Exp*_tmp95B;union Cyc_Absyn_Constraint*_tmp95C;_LL3B1:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp959=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp958;if(_tmp959->tag != 8)goto _LL3B3;else{_tmp95A=(_tmp959->f1).elt_type;_tmp95B=(_tmp959->f1).num_elts;_tmp95C=(_tmp959->f1).zero_term;}}if(!
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp95C))goto _LL3B3;_LL3B2:
 if(_tmp95B == 0){
const char*_tmpE33;void*_tmpE32;(_tmpE32=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE33="can't initialize zero-terminated array -- size unknown",_tag_dyneither(_tmpE33,sizeof(char),55))),_tag_dyneither(_tmpE32,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=_tmp95B;
struct Cyc_Absyn_Exp*_tmp95F=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),
Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);
# 4936
struct Cyc_Absyn_Exp*_tmp960=Cyc_Absyn_signed_int_exp(0,0);
s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp95F,_tmp960,0),0),
Cyc_Absyn_new_stmt(s->r,0));
goto _LL3B0;};_LL3B3:;_LL3B4:
 goto _LL3B0;_LL3B0:;}}
# 4950
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*x=Cyc_Toc_temp_var();
struct _RegionHandle _tmp961=_new_region("prgn");struct _RegionHandle*prgn=& _tmp961;_push_region(prgn);{
struct Cyc_Absyn_Stmt*_tmp962=Cyc_Toc_throw_match_stmt();
struct Cyc_Toc_Env*_tmp963=Cyc_Toc_share_env(prgn,nv);
struct _tuple31 _tmp964=
Cyc_Toc_xlate_pat(_tmp963,prgn,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,
Cyc_Toc_throw_match_stmt(),0);
# 4956
struct Cyc_Toc_Env*_tmp966;struct Cyc_List_List*_tmp967;struct Cyc_Absyn_Stmt*_tmp968;struct _tuple31 _tmp965=_tmp964;_tmp966=_tmp965.f1;_tmp967=_tmp965.f2;_tmp968=_tmp965.f3;
# 4959
Cyc_Toc_stmt_to_c(_tmp966,s);
s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),e,Cyc_Absyn_seq_stmt(_tmp968,s,0),0);
for(0;_tmp967 != 0;_tmp967=_tmp967->tl){
struct _tuple32*_tmp969=(struct _tuple32*)_tmp967->hd;struct _tuple1*_tmp96B;void*_tmp96C;struct _tuple32*_tmp96A=_tmp969;_tmp96B=_tmp96A->f1;_tmp96C=_tmp96A->f2;
s=Cyc_Absyn_declare_stmt(_tmp96B,_tmp96C,0,s,0);}}{
# 4966
struct Cyc_Absyn_Stmt*_tmp96D=s;_npop_handler(0);return _tmp96D;};
# 4953
;_pop_region(prgn);};}
# 4972
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp96E=e->r;void*_tmp96F=_tmp96E;struct Cyc_Absyn_Exp*_tmp971;struct Cyc_Absyn_Exp*_tmp973;struct Cyc_Absyn_Exp*_tmp975;struct Cyc_Absyn_Exp*_tmp977;struct Cyc_Absyn_Exp*_tmp979;struct Cyc_Absyn_Exp*_tmp97B;struct Cyc_Absyn_Exp*_tmp97D;struct Cyc_Absyn_Exp*_tmp97F;struct Cyc_List_List*_tmp981;struct Cyc_Absyn_Exp*_tmp983;struct Cyc_Absyn_Exp*_tmp984;struct Cyc_Absyn_Exp*_tmp986;struct Cyc_Absyn_Exp*_tmp987;struct Cyc_Absyn_Exp*_tmp989;struct Cyc_Absyn_Exp*_tmp98A;struct Cyc_Absyn_Exp*_tmp98C;struct Cyc_Absyn_Exp*_tmp98D;struct Cyc_Absyn_Exp*_tmp98F;struct Cyc_Absyn_Exp*_tmp990;struct Cyc_Absyn_Exp*_tmp992;struct Cyc_Absyn_Exp*_tmp993;struct Cyc_Absyn_Exp*_tmp995;struct Cyc_Absyn_Exp*_tmp996;struct Cyc_Absyn_Exp*_tmp997;struct Cyc_Absyn_Exp*_tmp999;struct Cyc_List_List*_tmp99A;void**_tmp99C;struct Cyc_Absyn_Exp*_tmp99D;void**_tmp99F;struct Cyc_List_List*_tmp9A0;struct Cyc_List_List*_tmp9A2;struct Cyc_List_List*_tmp9A4;void**_tmp9A6;void**_tmp9A8;struct Cyc_Absyn_Stmt*_tmp9AA;struct Cyc_Absyn_MallocInfo*_tmp9AC;_LL3B6: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp970=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp970->tag != 19)goto _LL3B8;else{_tmp971=_tmp970->f1;}}_LL3B7:
 _tmp973=_tmp971;goto _LL3B9;_LL3B8: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp972=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp972->tag != 20)goto _LL3BA;else{_tmp973=_tmp972->f1;}}_LL3B9:
 _tmp975=_tmp973;goto _LL3BB;_LL3BA: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp974=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp974->tag != 21)goto _LL3BC;else{_tmp975=_tmp974->f1;}}_LL3BB:
 _tmp977=_tmp975;goto _LL3BD;_LL3BC: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp976=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp976->tag != 14)goto _LL3BE;else{_tmp977=_tmp976->f1;}}_LL3BD:
 _tmp979=_tmp977;goto _LL3BF;_LL3BE: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp978=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp978->tag != 10)goto _LL3C0;else{_tmp979=_tmp978->f1;}}_LL3BF:
 _tmp97B=_tmp979;goto _LL3C1;_LL3C0: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp97A=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp97A->tag != 11)goto _LL3C2;else{_tmp97B=_tmp97A->f1;}}_LL3C1:
 _tmp97D=_tmp97B;goto _LL3C3;_LL3C2: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp97C=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp97C->tag != 17)goto _LL3C4;else{_tmp97D=_tmp97C->f1;}}_LL3C3:
 _tmp97F=_tmp97D;goto _LL3C5;_LL3C4: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp97E=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp97E->tag != 4)goto _LL3C6;else{_tmp97F=_tmp97E->f1;}}_LL3C5:
 Cyc_Toc_exptypes_to_c(_tmp97F);goto _LL3B5;_LL3C6: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp980=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp980->tag != 2)goto _LL3C8;else{_tmp981=_tmp980->f2;}}_LL3C7:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp981);goto _LL3B5;_LL3C8: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp982=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp982->tag != 6)goto _LL3CA;else{_tmp983=_tmp982->f1;_tmp984=_tmp982->f2;}}_LL3C9:
 _tmp986=_tmp983;_tmp987=_tmp984;goto _LL3CB;_LL3CA: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp985=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp985->tag != 7)goto _LL3CC;else{_tmp986=_tmp985->f1;_tmp987=_tmp985->f2;}}_LL3CB:
 _tmp989=_tmp986;_tmp98A=_tmp987;goto _LL3CD;_LL3CC: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp988=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp988->tag != 8)goto _LL3CE;else{_tmp989=_tmp988->f1;_tmp98A=_tmp988->f2;}}_LL3CD:
 _tmp98C=_tmp989;_tmp98D=_tmp98A;goto _LL3CF;_LL3CE: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp98B=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp98B->tag != 22)goto _LL3D0;else{_tmp98C=_tmp98B->f1;_tmp98D=_tmp98B->f2;}}_LL3CF:
 _tmp98F=_tmp98C;_tmp990=_tmp98D;goto _LL3D1;_LL3D0: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp98E=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp98E->tag != 34)goto _LL3D2;else{_tmp98F=_tmp98E->f1;_tmp990=_tmp98E->f2;}}_LL3D1:
 _tmp992=_tmp98F;_tmp993=_tmp990;goto _LL3D3;_LL3D2: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp991=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp991->tag != 3)goto _LL3D4;else{_tmp992=_tmp991->f1;_tmp993=_tmp991->f3;}}_LL3D3:
 Cyc_Toc_exptypes_to_c(_tmp992);Cyc_Toc_exptypes_to_c(_tmp993);goto _LL3B5;_LL3D4: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp994=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp994->tag != 5)goto _LL3D6;else{_tmp995=_tmp994->f1;_tmp996=_tmp994->f2;_tmp997=_tmp994->f3;}}_LL3D5:
# 4990
 Cyc_Toc_exptypes_to_c(_tmp995);Cyc_Toc_exptypes_to_c(_tmp996);Cyc_Toc_exptypes_to_c(_tmp997);goto _LL3B5;_LL3D6: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp998=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp998->tag != 9)goto _LL3D8;else{_tmp999=_tmp998->f1;_tmp99A=_tmp998->f2;}}_LL3D7:
# 4992
 Cyc_Toc_exptypes_to_c(_tmp999);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp99A);goto _LL3B5;_LL3D8: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp99B=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp99B->tag != 13)goto _LL3DA;else{_tmp99C=(void**)((void**)& _tmp99B->f1);_tmp99D=_tmp99B->f2;}}_LL3D9:
*_tmp99C=Cyc_Toc_typ_to_c(*_tmp99C);Cyc_Toc_exptypes_to_c(_tmp99D);goto _LL3B5;_LL3DA: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp99E=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp99E->tag != 24)goto _LL3DC;else{_tmp99F=(void**)&(_tmp99E->f1)->f3;_tmp9A0=_tmp99E->f2;}}_LL3DB:
# 4995
*_tmp99F=Cyc_Toc_typ_to_c(*_tmp99F);
_tmp9A2=_tmp9A0;goto _LL3DD;_LL3DC: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp9A1=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp9A1->tag != 35)goto _LL3DE;else{_tmp9A2=_tmp9A1->f2;}}_LL3DD:
 _tmp9A4=_tmp9A2;goto _LL3DF;_LL3DE: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp9A3=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp9A3->tag != 25)goto _LL3E0;else{_tmp9A4=_tmp9A3->f1;}}_LL3DF:
# 4999
 for(0;_tmp9A4 != 0;_tmp9A4=_tmp9A4->tl){
struct _tuple19 _tmp9BC=*((struct _tuple19*)_tmp9A4->hd);struct Cyc_Absyn_Exp*_tmp9BE;struct _tuple19 _tmp9BD=_tmp9BC;_tmp9BE=_tmp9BD.f2;
Cyc_Toc_exptypes_to_c(_tmp9BE);}
# 5003
goto _LL3B5;_LL3E0: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp9A5=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp9A5->tag != 18)goto _LL3E2;else{_tmp9A6=(void**)((void**)& _tmp9A5->f1);}}_LL3E1:
 _tmp9A8=_tmp9A6;goto _LL3E3;_LL3E2: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp9A7=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp9A7->tag != 16)goto _LL3E4;else{_tmp9A8=(void**)((void**)& _tmp9A7->f1);}}_LL3E3:
*_tmp9A8=Cyc_Toc_typ_to_c(*_tmp9A8);goto _LL3B5;_LL3E4: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp9A9=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp9A9->tag != 36)goto _LL3E6;else{_tmp9AA=_tmp9A9->f1;}}_LL3E5:
 Cyc_Toc_stmttypes_to_c(_tmp9AA);goto _LL3B5;_LL3E6: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp9AB=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp9AB->tag != 33)goto _LL3E8;else{_tmp9AC=(struct Cyc_Absyn_MallocInfo*)& _tmp9AB->f1;}}_LL3E7:
# 5008
 if(_tmp9AC->elt_type != 0){
void**_tmpE34;_tmp9AC->elt_type=((_tmpE34=_cycalloc(sizeof(*_tmpE34)),((_tmpE34[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp9AC->elt_type))),_tmpE34))));}
Cyc_Toc_exptypes_to_c(_tmp9AC->num_elts);
goto _LL3B5;_LL3E8: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp9AD=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp9AD->tag != 0)goto _LL3EA;}_LL3E9:
 goto _LL3EB;_LL3EA: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp9AE=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp9AE->tag != 1)goto _LL3EC;}_LL3EB:
 goto _LL3ED;_LL3EC: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp9AF=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp9AF->tag != 31)goto _LL3EE;}_LL3ED:
 goto _LL3EF;_LL3EE: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp9B0=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp9B0->tag != 39)goto _LL3F0;}_LL3EF:
 goto _LL3F1;_LL3F0: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp9B1=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp9B1->tag != 32)goto _LL3F2;}_LL3F1:
 goto _LL3B5;_LL3F2: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp9B2=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp9B2->tag != 29)goto _LL3F4;}_LL3F3:
# 5018
 goto _LL3F5;_LL3F4: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp9B3=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp9B3->tag != 30)goto _LL3F6;}_LL3F5:
 goto _LL3F7;_LL3F6: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp9B4=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp9B4->tag != 28)goto _LL3F8;}_LL3F7:
 goto _LL3F9;_LL3F8: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp9B5=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp9B5->tag != 26)goto _LL3FA;}_LL3F9:
 goto _LL3FB;_LL3FA: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp9B6=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp9B6->tag != 27)goto _LL3FC;}_LL3FB:
 goto _LL3FD;_LL3FC: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp9B7=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp9B7->tag != 23)goto _LL3FE;}_LL3FD:
 goto _LL3FF;_LL3FE: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp9B8=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp9B8->tag != 12)goto _LL400;}_LL3FF:
 goto _LL401;_LL400: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp9B9=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp9B9->tag != 15)goto _LL402;}_LL401:
 goto _LL403;_LL402: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp9BA=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp9BA->tag != 38)goto _LL404;}_LL403:
 goto _LL405;_LL404: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp9BB=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp96F;if(_tmp9BB->tag != 37)goto _LL3B5;}_LL405:
# 5028
{const char*_tmpE37;void*_tmpE36;(_tmpE36=0,Cyc_Tcutil_terr(e->loc,((_tmpE37="Cyclone expression within C code",_tag_dyneither(_tmpE37,sizeof(char),33))),_tag_dyneither(_tmpE36,sizeof(void*),0)));}
goto _LL3B5;_LL3B5:;}
# 5033
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp9C2=d->r;void*_tmp9C3=_tmp9C2;struct Cyc_Absyn_Vardecl*_tmp9C5;struct Cyc_Absyn_Fndecl*_tmp9C7;struct Cyc_Absyn_Aggrdecl*_tmp9C9;struct Cyc_Absyn_Enumdecl*_tmp9CB;struct Cyc_Absyn_Typedefdecl*_tmp9CD;_LL407: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp9C4=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp9C3;if(_tmp9C4->tag != 0)goto _LL409;else{_tmp9C5=_tmp9C4->f1;}}_LL408:
# 5036
 _tmp9C5->type=Cyc_Toc_typ_to_c(_tmp9C5->type);
if(_tmp9C5->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9C5->initializer));
goto _LL406;_LL409: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp9C6=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp9C3;if(_tmp9C6->tag != 1)goto _LL40B;else{_tmp9C7=_tmp9C6->f1;}}_LL40A:
# 5040
 _tmp9C7->ret_type=Cyc_Toc_typ_to_c(_tmp9C7->ret_type);
{struct Cyc_List_List*_tmp9D8=_tmp9C7->args;for(0;_tmp9D8 != 0;_tmp9D8=_tmp9D8->tl){
(*((struct _tuple9*)_tmp9D8->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp9D8->hd)).f3);}}
# 5044
goto _LL406;_LL40B: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp9C8=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp9C3;if(_tmp9C8->tag != 5)goto _LL40D;else{_tmp9C9=_tmp9C8->f1;}}_LL40C:
 Cyc_Toc_aggrdecl_to_c(_tmp9C9,1);goto _LL406;_LL40D: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp9CA=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp9C3;if(_tmp9CA->tag != 7)goto _LL40F;else{_tmp9CB=_tmp9CA->f1;}}_LL40E:
# 5047
 if(_tmp9CB->fields != 0){
struct Cyc_List_List*_tmp9D9=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp9CB->fields))->v;for(0;_tmp9D9 != 0;_tmp9D9=_tmp9D9->tl){
struct Cyc_Absyn_Enumfield*_tmp9DA=(struct Cyc_Absyn_Enumfield*)_tmp9D9->hd;
if(_tmp9DA->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9DA->tag));}}
# 5052
goto _LL406;_LL40F: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp9CC=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp9C3;if(_tmp9CC->tag != 8)goto _LL411;else{_tmp9CD=_tmp9CC->f1;}}_LL410:
 _tmp9CD->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp9CD->defn));goto _LL406;_LL411: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp9CE=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp9C3;if(_tmp9CE->tag != 2)goto _LL413;}_LL412:
 goto _LL414;_LL413: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp9CF=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp9C3;if(_tmp9CF->tag != 3)goto _LL415;}_LL414:
 goto _LL416;_LL415: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp9D0=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp9C3;if(_tmp9D0->tag != 6)goto _LL417;}_LL416:
 goto _LL418;_LL417: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp9D1=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp9C3;if(_tmp9D1->tag != 9)goto _LL419;}_LL418:
 goto _LL41A;_LL419: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp9D2=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp9C3;if(_tmp9D2->tag != 10)goto _LL41B;}_LL41A:
 goto _LL41C;_LL41B: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp9D3=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp9C3;if(_tmp9D3->tag != 11)goto _LL41D;}_LL41C:
 goto _LL41E;_LL41D: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp9D4=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp9C3;if(_tmp9D4->tag != 12)goto _LL41F;}_LL41E:
 goto _LL420;_LL41F: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp9D5=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp9C3;if(_tmp9D5->tag != 4)goto _LL421;}_LL420:
# 5063
{const char*_tmpE3A;void*_tmpE39;(_tmpE39=0,Cyc_Tcutil_terr(d->loc,((_tmpE3A="Cyclone declaration within C code",_tag_dyneither(_tmpE3A,sizeof(char),34))),_tag_dyneither(_tmpE39,sizeof(void*),0)));}
goto _LL406;_LL421: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp9D6=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp9C3;if(_tmp9D6->tag != 13)goto _LL423;}_LL422:
 goto _LL424;_LL423: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp9D7=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp9C3;if(_tmp9D7->tag != 14)goto _LL406;}_LL424:
# 5067
 goto _LL406;_LL406:;}
# 5071
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp9DD=s->r;void*_tmp9DE=_tmp9DD;struct Cyc_Absyn_Exp*_tmp9E0;struct Cyc_Absyn_Stmt*_tmp9E2;struct Cyc_Absyn_Stmt*_tmp9E3;struct Cyc_Absyn_Exp*_tmp9E5;struct Cyc_Absyn_Exp*_tmp9E7;struct Cyc_Absyn_Stmt*_tmp9E8;struct Cyc_Absyn_Stmt*_tmp9E9;struct Cyc_Absyn_Exp*_tmp9EB;struct Cyc_Absyn_Stmt*_tmp9EC;struct Cyc_Absyn_Exp*_tmp9EE;struct Cyc_Absyn_Exp*_tmp9EF;struct Cyc_Absyn_Exp*_tmp9F0;struct Cyc_Absyn_Stmt*_tmp9F1;struct Cyc_Absyn_Exp*_tmp9F3;struct Cyc_List_List*_tmp9F4;struct Cyc_Absyn_Decl*_tmp9F6;struct Cyc_Absyn_Stmt*_tmp9F7;struct Cyc_Absyn_Stmt*_tmp9F9;struct Cyc_Absyn_Exp*_tmp9FA;_LL426: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp9DF=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp9DE;if(_tmp9DF->tag != 1)goto _LL428;else{_tmp9E0=_tmp9DF->f1;}}_LL427:
 Cyc_Toc_exptypes_to_c(_tmp9E0);goto _LL425;_LL428: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp9E1=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp9DE;if(_tmp9E1->tag != 2)goto _LL42A;else{_tmp9E2=_tmp9E1->f1;_tmp9E3=_tmp9E1->f2;}}_LL429:
 Cyc_Toc_stmttypes_to_c(_tmp9E2);Cyc_Toc_stmttypes_to_c(_tmp9E3);goto _LL425;_LL42A: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp9E4=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp9DE;if(_tmp9E4->tag != 3)goto _LL42C;else{_tmp9E5=_tmp9E4->f1;}}_LL42B:
 if(_tmp9E5 != 0)Cyc_Toc_exptypes_to_c(_tmp9E5);goto _LL425;_LL42C: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp9E6=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp9DE;if(_tmp9E6->tag != 4)goto _LL42E;else{_tmp9E7=_tmp9E6->f1;_tmp9E8=_tmp9E6->f2;_tmp9E9=_tmp9E6->f3;}}_LL42D:
# 5077
 Cyc_Toc_exptypes_to_c(_tmp9E7);Cyc_Toc_stmttypes_to_c(_tmp9E8);Cyc_Toc_stmttypes_to_c(_tmp9E9);goto _LL425;_LL42E: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp9EA=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp9DE;if(_tmp9EA->tag != 5)goto _LL430;else{_tmp9EB=(_tmp9EA->f1).f1;_tmp9EC=_tmp9EA->f2;}}_LL42F:
# 5079
 Cyc_Toc_exptypes_to_c(_tmp9EB);Cyc_Toc_stmttypes_to_c(_tmp9EC);goto _LL425;_LL430: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp9ED=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp9DE;if(_tmp9ED->tag != 9)goto _LL432;else{_tmp9EE=_tmp9ED->f1;_tmp9EF=(_tmp9ED->f2).f1;_tmp9F0=(_tmp9ED->f3).f1;_tmp9F1=_tmp9ED->f4;}}_LL431:
# 5081
 Cyc_Toc_exptypes_to_c(_tmp9EE);Cyc_Toc_exptypes_to_c(_tmp9EF);Cyc_Toc_exptypes_to_c(_tmp9F0);
Cyc_Toc_stmttypes_to_c(_tmp9F1);goto _LL425;_LL432: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp9F2=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp9DE;if(_tmp9F2->tag != 10)goto _LL434;else{_tmp9F3=_tmp9F2->f1;_tmp9F4=_tmp9F2->f2;}}_LL433:
# 5084
 Cyc_Toc_exptypes_to_c(_tmp9F3);
for(0;_tmp9F4 != 0;_tmp9F4=_tmp9F4->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp9F4->hd)->body);}
goto _LL425;_LL434: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp9F5=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp9DE;if(_tmp9F5->tag != 12)goto _LL436;else{_tmp9F6=_tmp9F5->f1;_tmp9F7=_tmp9F5->f2;}}_LL435:
 Cyc_Toc_decltypes_to_c(_tmp9F6);Cyc_Toc_stmttypes_to_c(_tmp9F7);goto _LL425;_LL436: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp9F8=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp9DE;if(_tmp9F8->tag != 14)goto _LL438;else{_tmp9F9=_tmp9F8->f1;_tmp9FA=(_tmp9F8->f2).f1;}}_LL437:
 Cyc_Toc_stmttypes_to_c(_tmp9F9);Cyc_Toc_exptypes_to_c(_tmp9FA);goto _LL425;_LL438: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp9FB=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp9DE;if(_tmp9FB->tag != 0)goto _LL43A;}_LL439:
 goto _LL43B;_LL43A: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp9FC=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp9DE;if(_tmp9FC->tag != 6)goto _LL43C;}_LL43B:
 goto _LL43D;_LL43C: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp9FD=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp9DE;if(_tmp9FD->tag != 7)goto _LL43E;}_LL43D:
 goto _LL43F;_LL43E: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp9FE=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp9DE;if(_tmp9FE->tag != 8)goto _LL440;}_LL43F:
 goto _LL425;_LL440: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp9FF=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp9DE;if(_tmp9FF->tag != 11)goto _LL442;}_LL441:
 goto _LL443;_LL442: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpA00=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp9DE;if(_tmpA00->tag != 13)goto _LL444;}_LL443:
 goto _LL445;_LL444: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmpA01=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp9DE;if(_tmpA01->tag != 15)goto _LL446;}_LL445:
 goto _LL447;_LL446: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmpA02=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp9DE;if(_tmpA02->tag != 16)goto _LL425;}_LL447:
{const char*_tmpE3D;void*_tmpE3C;(_tmpE3C=0,Cyc_Tcutil_terr(s->loc,((_tmpE3D="Cyclone statement in C code",_tag_dyneither(_tmpE3D,sizeof(char),28))),_tag_dyneither(_tmpE3C,sizeof(void*),0)));}
goto _LL425;_LL425:;}
# 5105
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv)){
const char*_tmpE40;void*_tmpE3F;(_tmpE3F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE40="decls_to_c: not at toplevel!",_tag_dyneither(_tmpE40,sizeof(char),29))),_tag_dyneither(_tmpE3F,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmpA07=d->r;void*_tmpA08=_tmpA07;struct Cyc_Absyn_Vardecl*_tmpA0A;struct Cyc_Absyn_Fndecl*_tmpA0C;struct Cyc_Absyn_Aggrdecl*_tmpA11;struct Cyc_Absyn_Datatypedecl*_tmpA13;struct Cyc_Absyn_Enumdecl*_tmpA15;struct Cyc_Absyn_Typedefdecl*_tmpA17;struct Cyc_List_List*_tmpA1B;struct Cyc_List_List*_tmpA1D;struct Cyc_List_List*_tmpA1F;struct Cyc_List_List*_tmpA21;_LL449: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpA09=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpA08;if(_tmpA09->tag != 0)goto _LL44B;else{_tmpA0A=_tmpA09->f1;}}_LL44A: {
# 5112
struct _tuple1*_tmpA22=_tmpA0A->name;
# 5114
if(_tmpA0A->sc == Cyc_Absyn_ExternC){
struct _tuple1*_tmpE41;_tmpA22=((_tmpE41=_cycalloc(sizeof(*_tmpE41)),((_tmpE41->f1=Cyc_Absyn_Rel_n(0),((_tmpE41->f2=(*_tmpA22).f2,_tmpE41))))));}
if(_tmpA0A->initializer != 0){
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmpA0A->initializer));else{
# 5120
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmpA0A->initializer));}}
# 5122
{struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmpE44;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpE43;nv=Cyc_Toc_add_varmap(r,nv,_tmpA0A->name,Cyc_Absyn_varb_exp(_tmpA22,(void*)((_tmpE43=_cycalloc(sizeof(*_tmpE43)),((_tmpE43[0]=((_tmpE44.tag=1,((_tmpE44.f1=_tmpA0A,_tmpE44)))),_tmpE43)))),0));}
_tmpA0A->name=_tmpA22;
_tmpA0A->sc=Cyc_Toc_scope_to_c(_tmpA0A->sc);
_tmpA0A->type=Cyc_Toc_typ_to_c(_tmpA0A->type);
{struct Cyc_List_List*_tmpE45;Cyc_Toc_result_decls=((_tmpE45=_cycalloc(sizeof(*_tmpE45)),((_tmpE45->hd=d,((_tmpE45->tl=Cyc_Toc_result_decls,_tmpE45))))));}
goto _LL448;}_LL44B: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpA0B=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpA08;if(_tmpA0B->tag != 1)goto _LL44D;else{_tmpA0C=_tmpA0B->f1;}}_LL44C: {
# 5129
struct _tuple1*_tmpA27=_tmpA0C->name;
# 5131
if(_tmpA0C->sc == Cyc_Absyn_ExternC){
{struct _tuple1*_tmpE46;_tmpA27=((_tmpE46=_cycalloc(sizeof(*_tmpE46)),((_tmpE46->f1=Cyc_Absyn_Abs_n(0,1),((_tmpE46->f2=(*_tmpA27).f2,_tmpE46))))));}
_tmpA0C->sc=Cyc_Absyn_Public;}
# 5135
nv=Cyc_Toc_add_varmap(r,nv,_tmpA0C->name,Cyc_Absyn_var_exp(_tmpA27,0));
_tmpA0C->name=_tmpA27;
Cyc_Toc_fndecl_to_c(nv,_tmpA0C,cinclude);
{struct Cyc_List_List*_tmpE47;Cyc_Toc_result_decls=((_tmpE47=_cycalloc(sizeof(*_tmpE47)),((_tmpE47->hd=d,((_tmpE47->tl=Cyc_Toc_result_decls,_tmpE47))))));}
goto _LL448;}_LL44D: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmpA0D=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpA08;if(_tmpA0D->tag != 2)goto _LL44F;}_LL44E:
 goto _LL450;_LL44F: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmpA0E=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmpA08;if(_tmpA0E->tag != 3)goto _LL451;}_LL450: {
# 5142
const char*_tmpE4A;void*_tmpE49;(_tmpE49=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE4A="letdecl at toplevel",_tag_dyneither(_tmpE4A,sizeof(char),20))),_tag_dyneither(_tmpE49,sizeof(void*),0)));}_LL451: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmpA0F=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpA08;if(_tmpA0F->tag != 4)goto _LL453;}_LL452: {
# 5144
const char*_tmpE4D;void*_tmpE4C;(_tmpE4C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE4D="region decl at toplevel",_tag_dyneither(_tmpE4D,sizeof(char),24))),_tag_dyneither(_tmpE4C,sizeof(void*),0)));}_LL453: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpA10=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpA08;if(_tmpA10->tag != 5)goto _LL455;else{_tmpA11=_tmpA10->f1;}}_LL454:
# 5148
 Cyc_Toc_aggrdecl_to_c(_tmpA11,1);
goto _LL448;_LL455: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmpA12=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmpA08;if(_tmpA12->tag != 6)goto _LL457;else{_tmpA13=_tmpA12->f1;}}_LL456:
# 5151
 if(_tmpA13->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmpA13);else{
# 5154
Cyc_Toc_datatypedecl_to_c(_tmpA13);}
goto _LL448;_LL457: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmpA14=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpA08;if(_tmpA14->tag != 7)goto _LL459;else{_tmpA15=_tmpA14->f1;}}_LL458:
# 5157
 Cyc_Toc_enumdecl_to_c(_tmpA15);
{struct Cyc_List_List*_tmpE4E;Cyc_Toc_result_decls=((_tmpE4E=_cycalloc(sizeof(*_tmpE4E)),((_tmpE4E->hd=d,((_tmpE4E->tl=Cyc_Toc_result_decls,_tmpE4E))))));}
goto _LL448;_LL459: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmpA16=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpA08;if(_tmpA16->tag != 8)goto _LL45B;else{_tmpA17=_tmpA16->f1;}}_LL45A:
# 5161
 _tmpA17->name=_tmpA17->name;
_tmpA17->tvs=0;
if(_tmpA17->defn != 0)
_tmpA17->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmpA17->defn));else{
# 5166
enum Cyc_Absyn_KindQual _tmpA2F=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmpA17->kind))->v)->kind;switch(_tmpA2F){case Cyc_Absyn_BoxKind: _LL467:
 _tmpA17->defn=Cyc_Absyn_void_star_typ();break;default: _LL468:
 _tmpA17->defn=(void*)& Cyc_Absyn_VoidType_val;break;}}
# 5171
{struct Cyc_List_List*_tmpE4F;Cyc_Toc_result_decls=((_tmpE4F=_cycalloc(sizeof(*_tmpE4F)),((_tmpE4F->hd=d,((_tmpE4F->tl=Cyc_Toc_result_decls,_tmpE4F))))));}
goto _LL448;_LL45B: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmpA18=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmpA08;if(_tmpA18->tag != 13)goto _LL45D;}_LL45C:
 goto _LL45E;_LL45D: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmpA19=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmpA08;if(_tmpA19->tag != 14)goto _LL45F;}_LL45E:
# 5175
 goto _LL448;_LL45F: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpA1A=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpA08;if(_tmpA1A->tag != 9)goto _LL461;else{_tmpA1B=_tmpA1A->f2;}}_LL460:
 _tmpA1D=_tmpA1B;goto _LL462;_LL461: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmpA1C=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpA08;if(_tmpA1C->tag != 10)goto _LL463;else{_tmpA1D=_tmpA1C->f2;}}_LL462:
 _tmpA1F=_tmpA1D;goto _LL464;_LL463: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmpA1E=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmpA08;if(_tmpA1E->tag != 11)goto _LL465;else{_tmpA1F=_tmpA1E->f1;}}_LL464:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmpA1F,top,cinclude);goto _LL448;_LL465: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA20=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmpA08;if(_tmpA20->tag != 12)goto _LL448;else{_tmpA21=_tmpA20->f1;}}_LL466:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmpA21,top,1);goto _LL448;_LL448:;};}
# 5182
return nv;}
# 5186
static void Cyc_Toc_init(){
struct Cyc_Core_NewDynamicRegion _tmpA31=Cyc_Core_new_rckey();struct Cyc_Core_DynamicRegion*_tmpA33;struct Cyc_Core_NewDynamicRegion _tmpA32=_tmpA31;_tmpA33=_tmpA32.key;{
struct Cyc_Toc_TocState*_tmpA34=((struct Cyc_Toc_TocState*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Toc_TocState*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmpA33,0,Cyc_Toc_empty_toc_state);
{struct Cyc_Toc_TocStateWrap*_tmpE50;Cyc_Toc_toc_state=((_tmpE50=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE50)),((_tmpE50->dyn=(struct Cyc_Core_DynamicRegion*)_tmpA33,((_tmpE50->state=(struct Cyc_Toc_TocState*)_tmpA34,_tmpE50))))));}
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0;
Cyc_Toc_bounds_checks_eliminated=0;{
struct _dyneither_ptr**_tmpE51;Cyc_Toc_globals=_tag_dyneither(((_tmpE51=_cycalloc(sizeof(struct _dyneither_ptr*)* 41),((_tmpE51[0]=(struct _dyneither_ptr*)& Cyc_Toc__throw_str,((_tmpE51[1]=(struct _dyneither_ptr*)& Cyc_Toc_setjmp_str,((_tmpE51[2]=(struct _dyneither_ptr*)& Cyc_Toc__push_handler_str,((_tmpE51[3]=(struct _dyneither_ptr*)& Cyc_Toc__pop_handler_str,((_tmpE51[4]=(struct _dyneither_ptr*)& Cyc_Toc__exn_thrown_str,((_tmpE51[5]=(struct _dyneither_ptr*)& Cyc_Toc__npop_handler_str,((_tmpE51[6]=(struct _dyneither_ptr*)& Cyc_Toc__check_null_str,((_tmpE51[7]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_null_str,((_tmpE51[8]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_notnull_str,((_tmpE51[9]=(struct _dyneither_ptr*)& Cyc_Toc__check_dyneither_subscript_str,((_tmpE51[10]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_str,((_tmpE51[11]=(struct _dyneither_ptr*)& Cyc_Toc__tag_dyneither_str,((_tmpE51[12]=(struct _dyneither_ptr*)& Cyc_Toc__init_dyneither_ptr_str,((_tmpE51[13]=(struct _dyneither_ptr*)& Cyc_Toc__untag_dyneither_ptr_str,((_tmpE51[14]=(struct _dyneither_ptr*)& Cyc_Toc__get_dyneither_size_str,((_tmpE51[15]=(struct _dyneither_ptr*)& Cyc_Toc__get_zero_arr_size_str,((_tmpE51[16]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_plus_str,((_tmpE51[17]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_plus_str,((_tmpE51[18]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpE51[19]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_str,((_tmpE51[20]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpE51[21]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_post_str,((_tmpE51[22]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_str,((_tmpE51[23]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_str,((_tmpE51[24]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_atomic_str,((_tmpE51[25]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_atomic_str,((_tmpE51[26]=(struct _dyneither_ptr*)& Cyc_Toc__region_malloc_str,((_tmpE51[27]=(struct _dyneither_ptr*)& Cyc_Toc__region_calloc_str,((_tmpE51[28]=(struct _dyneither_ptr*)& Cyc_Toc__check_times_str,((_tmpE51[29]=(struct _dyneither_ptr*)& Cyc_Toc__new_region_str,((_tmpE51[30]=(struct _dyneither_ptr*)& Cyc_Toc__push_region_str,((_tmpE51[31]=(struct _dyneither_ptr*)& Cyc_Toc__pop_region_str,((_tmpE51[32]=(struct _dyneither_ptr*)& Cyc_Toc__open_dynregion_str,((_tmpE51[33]=(struct _dyneither_ptr*)& Cyc_Toc__push_dynregion_str,((_tmpE51[34]=(struct _dyneither_ptr*)& Cyc_Toc__pop_dynregion_str,((_tmpE51[35]=(struct _dyneither_ptr*)& Cyc_Toc__reset_region_str,((_tmpE51[36]=(struct _dyneither_ptr*)& Cyc_Toc__throw_arraybounds_str,((_tmpE51[37]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_decrease_size_str,((_tmpE51[38]=(struct _dyneither_ptr*)& Cyc_Toc__throw_match_str,((_tmpE51[39]=(struct _dyneither_ptr*)& Cyc_Toc__swap_word_str,((_tmpE51[40]=(struct _dyneither_ptr*)& Cyc_Toc__swap_dyneither_str,_tmpE51)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),sizeof(struct _dyneither_ptr*),41);};};}
# 5242
static int Cyc_Toc_destroy_labels(struct _RegionHandle*d,struct Cyc_Toc_TocState*ts){
struct Cyc_Toc_TocState _tmpA37=*ts;struct Cyc_Xarray_Xarray*_tmpA39;struct Cyc_Toc_TocState _tmpA38=_tmpA37;_tmpA39=_tmpA38.temp_labels;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmpA39);
return 0;}
# 5250
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){
Cyc_Toc_init();{
struct _RegionHandle _tmpA3A=_new_region("start");struct _RegionHandle*start=& _tmpA3A;_push_region(start);
Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(start),ds,1,0);{
# 5259
struct Cyc_Toc_TocStateWrap*ts=0;
_swap_word(& ts,& Cyc_Toc_toc_state);{
struct Cyc_Toc_TocStateWrap _tmpA3B=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Core_DynamicRegion*_tmpA3D;struct Cyc_Toc_TocState*_tmpA3E;struct Cyc_Toc_TocStateWrap _tmpA3C=_tmpA3B;_tmpA3D=_tmpA3C.dyn;_tmpA3E=_tmpA3C.state;
((int(*)(struct Cyc_Core_DynamicRegion*key,struct Cyc_Toc_TocState*arg,int(*body)(struct _RegionHandle*h,struct Cyc_Toc_TocState*arg)))Cyc_Core_open_region)(_tmpA3D,_tmpA3E,Cyc_Toc_destroy_labels);
Cyc_Core_free_rckey(_tmpA3D);
((void(*)(struct _dyneither_ptr ptr))Cyc_Core_ufree)(_tag_dyneither(ts,sizeof(struct Cyc_Toc_TocStateWrap),1));};};{
# 5266
struct Cyc_List_List*_tmpA3F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0);return _tmpA3F;};
# 5252
;_pop_region(start);};}
