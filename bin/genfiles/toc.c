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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 110
typedef struct _tuple1*Cyc_Absyn_qvar_t;typedef struct _tuple1*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_opt_t;
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
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 370
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 390
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 395
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 450 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
# 453
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 466
typedef void*Cyc_Absyn_funcparams_t;
# 469
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 533
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 549
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 551
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign,long long);
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr,int);
# 559
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 566
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 584
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 715 "absyn.h"
extern struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val;struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 750
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
# 778
extern struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 937
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 944
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 954
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 957
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
# 964
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 967
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 982
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 984
extern void*Cyc_Absyn_sint_typ;
# 996
void*Cyc_Absyn_exn_typ();
# 1007
extern void*Cyc_Absyn_bounds_one;
# 1015
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1021
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);
# 1023
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1026
void*Cyc_Absyn_void_star_typ();
# 1028
void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple1*name);
void*Cyc_Absyn_unionq_typ(struct _tuple1*name);
# 1034
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1038
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1040
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1046
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);
# 1051
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
# 1054
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple1*,void*,unsigned int);
# 1058
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1061
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1067
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1070
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1073
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1078
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1082
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1087
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
# 1092
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1097
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1103
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned int);
# 1109
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1119
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1122
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned int loc);
# 1126
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1128
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc);
# 1133
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1137
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1142
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
# 1185
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1188
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1198
struct _dyneither_ptr*Cyc_Absyn_fieldname(int);
# 1202
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1218
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
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);struct _tuple12{unsigned int f1;int f2;};
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
extern struct _dyneither_ptr Cyc_Toc_globals;struct Cyc_Tcpat_TcPatResult{struct _tuple0*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};
# 54 "tcpat.h"
typedef struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcpat_result_t;struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};
# 75
typedef void*Cyc_Tcpat_pat_test_t;struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned int f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _dyneither_ptr*f2;};
# 83
typedef void*Cyc_Tcpat_access_t;struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};
# 93
typedef struct Cyc_Tcpat_PathNode*Cyc_Tcpat_path_node_t;
# 95
typedef void*Cyc_Tcpat_term_desc_t;
typedef struct Cyc_List_List*Cyc_Tcpat_path_t;struct Cyc_Tcpat_Rhs{int used;unsigned int pat_loc;struct Cyc_Absyn_Stmt*rhs;};
# 103
typedef struct Cyc_Tcpat_Rhs*Cyc_Tcpat_rhs_t;
# 105
typedef void*Cyc_Tcpat_decision_t;struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};
# 67 "toc.cyc"
extern int Cyc_noexpand_r;
# 78 "toc.cyc"
int Cyc_Toc_warn_bounds_checks=0;
int Cyc_Toc_warn_all_null_deref=0;
unsigned int Cyc_Toc_total_bounds_checks=0;
unsigned int Cyc_Toc_bounds_checks_eliminated=0;
# 87
static struct Cyc_List_List*Cyc_Toc_result_decls=0;struct Cyc_Toc_TocState{struct Cyc_List_List**tuple_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_List_List**abs_struct_types;struct Cyc_Set_Set**datatypes_so_far;struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*temp_labels;};struct _tuple14{struct _tuple1*f1;struct _tuple1*f2;};
# 107
int Cyc_Toc_qvar_tag_cmp(struct _tuple14*x,struct _tuple14*y){
struct _tuple14 _tmp0=*x;struct _tuple1*_tmp2;struct _tuple1*_tmp3;struct _tuple14 _tmp1=_tmp0;_tmp2=_tmp1.f1;_tmp3=_tmp1.f2;{
struct _tuple14 _tmp4=*y;struct _tuple1*_tmp6;struct _tuple1*_tmp7;struct _tuple14 _tmp5=_tmp4;_tmp6=_tmp5.f1;_tmp7=_tmp5.f2;{
int i=Cyc_Absyn_qvar_cmp(_tmp2,_tmp6);
if(i != 0)return i;
return Cyc_Absyn_qvar_cmp(_tmp3,_tmp7);};};}
# 116
static struct Cyc_Toc_TocState*Cyc_Toc_empty_toc_state(struct _RegionHandle*d,int unused){
struct Cyc_Dict_Dict*_tmp97D;struct Cyc_Dict_Dict*_tmp97C;struct Cyc_Set_Set**_tmp97B;struct Cyc_List_List**_tmp97A;struct Cyc_Dict_Dict*_tmp979;struct Cyc_List_List**_tmp978;struct Cyc_Toc_TocState*_tmp977;return(_tmp977=_region_malloc(d,sizeof(*_tmp977)),((_tmp977->tuple_types=(
(_tmp978=_region_malloc(d,sizeof(*_tmp978)),((_tmp978[0]=0,_tmp978)))),((_tmp977->aggrs_so_far=(
(_tmp979=_region_malloc(d,sizeof(*_tmp979)),((_tmp979[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmp979)))),((_tmp977->abs_struct_types=(
(_tmp97A=_region_malloc(d,sizeof(*_tmp97A)),((_tmp97A[0]=0,_tmp97A)))),((_tmp977->datatypes_so_far=(
(_tmp97B=_region_malloc(d,sizeof(*_tmp97B)),((_tmp97B[0]=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),_tmp97B)))),((_tmp977->xdatatypes_so_far=(
(_tmp97C=_region_malloc(d,sizeof(*_tmp97C)),((_tmp97C[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmp97C)))),((_tmp977->qvar_tags=(
(_tmp97D=_region_malloc(d,sizeof(*_tmp97D)),((_tmp97D[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),_tmp97D)))),((_tmp977->temp_labels=
((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d),_tmp977)))))))))))))));}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 133
typedef struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state_t;
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple15{struct Cyc_Toc_TocState*f1;void*f2;};
# 140
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple15*)){
# 143
struct Cyc_Toc_TocStateWrap*ts=0;
_swap_word(& ts,& Cyc_Toc_toc_state);{
struct Cyc_Toc_TocStateWrap _tmpF=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Core_DynamicRegion*_tmp11;struct Cyc_Toc_TocState*_tmp12;struct Cyc_Toc_TocStateWrap _tmp10=_tmpF;_tmp11=_tmp10.dyn;_tmp12=_tmp10.state;{
struct _dyneither_ptr _tmp13=((struct _dyneither_ptr(*)(struct _dyneither_ptr ptr))Cyc_Core_alias_refptr)(_tag_dyneither(_tmp11,sizeof(struct Cyc_Core_DynamicRegion),1));
{struct Cyc_Toc_TocStateWrap _tmp97E;*ts=((_tmp97E.dyn=(struct Cyc_Core_DynamicRegion*)_tmp11,((_tmp97E.state=(struct Cyc_Toc_TocState*)_tmp12,_tmp97E))));}
_swap_word(& ts,& Cyc_Toc_toc_state);{
struct _tuple15 _tmp97F;struct _tuple15 _tmp15=(_tmp97F.f1=_tmp12,((_tmp97F.f2=arg,_tmp97F)));
void*_tmp16=((void*(*)(struct Cyc_Core_DynamicRegion*key,struct _tuple15*arg,void*(*body)(struct _RegionHandle*h,struct _tuple15*arg)))Cyc_Core_open_region)((struct Cyc_Core_DynamicRegion*)_untag_dyneither_ptr(_tmp13,sizeof(struct Cyc_Core_DynamicRegion),1),& _tmp15,f);
Cyc_Core_free_rckey((struct Cyc_Core_DynamicRegion*)_untag_dyneither_ptr(_tmp13,sizeof(struct Cyc_Core_DynamicRegion),1));
return _tmp16;};};};}struct _tuple16{struct _tuple1*f1;void*(*f2)(struct _tuple1*);};struct _tuple17{struct Cyc_Toc_TocState*f1;struct _tuple16*f2;};struct _tuple18{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
# 155
static void*Cyc_Toc_aggrdecl_type_body(struct _RegionHandle*d,struct _tuple17*env){
# 158
struct _tuple17 _tmp18=*env;struct Cyc_Toc_TocState*_tmp1A;struct _tuple1*_tmp1B;void*(*_tmp1C)(struct _tuple1*);struct _tuple17 _tmp19=_tmp18;_tmp1A=_tmp19.f1;_tmp1B=(_tmp19.f2)->f1;_tmp1C=(_tmp19.f2)->f2;{
struct _tuple18**v=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp1A->aggrs_so_far,_tmp1B);
if(v == 0)
return _tmp1C(_tmp1B);else{
# 163
struct _tuple18*_tmp1D=*v;void*_tmp1F;struct _tuple18*_tmp1E=_tmp1D;_tmp1F=_tmp1E->f2;
return _tmp1F;}};}
# 168
void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){
struct _tuple16 _tmp980;struct _tuple16 env=(_tmp980.f1=q,((_tmp980.f2=type_maker,_tmp980)));
return((void*(*)(struct _tuple16*arg,void*(*f)(struct _RegionHandle*,struct _tuple17*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_aggrdecl_type_body);}
# 173
static int Cyc_Toc_tuple_type_counter=0;
static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;char Cyc_Toc_Toc_Unimplemented[18]="Toc_Unimplemented";struct Cyc_Toc_Toc_Unimplemented_exn_struct{char*tag;};char Cyc_Toc_Toc_Impossible[15]="Toc_Impossible";struct Cyc_Toc_Toc_Impossible_exn_struct{char*tag;};
# 180
static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 183
Cyc_vfprintf(Cyc_stderr,fmt,ap);
{const char*_tmp983;void*_tmp982;(_tmp982=0,Cyc_fprintf(Cyc_stderr,((_tmp983="\n",_tag_dyneither(_tmp983,sizeof(char),2))),_tag_dyneither(_tmp982,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Unimplemented_exn_struct _tmp986;struct Cyc_Toc_Toc_Unimplemented_exn_struct*_tmp985;(int)_throw((void*)((_tmp985=_cycalloc_atomic(sizeof(*_tmp985)),((_tmp985[0]=((_tmp986.tag=Cyc_Toc_Toc_Unimplemented,_tmp986)),_tmp985)))));};}
# 188
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 191
Cyc_vfprintf(Cyc_stderr,fmt,ap);
{const char*_tmp989;void*_tmp988;(_tmp988=0,Cyc_fprintf(Cyc_stderr,((_tmp989="\n",_tag_dyneither(_tmp989,sizeof(char),2))),_tag_dyneither(_tmp988,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Impossible_exn_struct _tmp98C;struct Cyc_Toc_Toc_Impossible_exn_struct*_tmp98B;(int)_throw((void*)((_tmp98B=_cycalloc_atomic(sizeof(*_tmp98B)),((_tmp98B[0]=((_tmp98C.tag=Cyc_Toc_Toc_Impossible,_tmp98C)),_tmp98B)))));};}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_exn_struct{char*tag;};static char _tmp29[5]="curr";
# 206 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc_curr_string={_tmp29,_tmp29,_tmp29 + 5};static struct _dyneither_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp2A[4]="tag";
static struct _dyneither_ptr Cyc_Toc_tag_string={_tmp2A,_tmp2A,_tmp2A + 4};static struct _dyneither_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp2B[4]="val";
static struct _dyneither_ptr Cyc_Toc_val_string={_tmp2B,_tmp2B,_tmp2B + 4};static struct _dyneither_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp2C[14]="_handler_cons";
static struct _dyneither_ptr Cyc_Toc__handler_cons_string={_tmp2C,_tmp2C,_tmp2C + 14};static struct _dyneither_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;static char _tmp2D[8]="handler";
static struct _dyneither_ptr Cyc_Toc_handler_string={_tmp2D,_tmp2D,_tmp2D + 8};static struct _dyneither_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp2E[14]="_RegionHandle";
static struct _dyneither_ptr Cyc_Toc__RegionHandle_string={_tmp2E,_tmp2E,_tmp2E + 14};static struct _dyneither_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;static char _tmp2F[17]="_DynRegionHandle";
static struct _dyneither_ptr Cyc_Toc__DynRegionHandle_string={_tmp2F,_tmp2F,_tmp2F + 17};static struct _dyneither_ptr*Cyc_Toc__DynRegionHandle_sp=& Cyc_Toc__DynRegionHandle_string;static char _tmp30[16]="_DynRegionFrame";
static struct _dyneither_ptr Cyc_Toc__DynRegionFrame_string={_tmp30,_tmp30,_tmp30 + 16};static struct _dyneither_ptr*Cyc_Toc__DynRegionFrame_sp=& Cyc_Toc__DynRegionFrame_string;
# 219
struct _dyneither_ptr Cyc_Toc_globals={(void*)0,(void*)0,(void*)(0 + 0)};static char _tmp31[7]="_throw";
# 228 "toc.cyc"
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
static struct _dyneither_ptr Cyc_Toc__rethrow_str={_tmpBB,_tmpBB,_tmpBB + 9};static struct _tuple1 Cyc_Toc__rethrow_pr={{.Loc_n={4,0}},& Cyc_Toc__rethrow_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__rethrow_re={1,& Cyc_Toc__rethrow_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__rethrow_ev={0,(void*)& Cyc_Toc__rethrow_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__rethrow_e=& Cyc_Toc__rethrow_ev;static char _tmpBD[20]="_fast_region_malloc";
static struct _dyneither_ptr Cyc_Toc__fast_region_malloc_str={_tmpBD,_tmpBD,_tmpBD + 20};static struct _tuple1 Cyc_Toc__fast_region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__fast_region_malloc_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fast_region_malloc_re={1,& Cyc_Toc__fast_region_malloc_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__fast_region_malloc_ev={0,(void*)& Cyc_Toc__fast_region_malloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fast_region_malloc_e=& Cyc_Toc__fast_region_malloc_ev;
# 303
static struct Cyc_Absyn_AggrType_Absyn_Type_struct Cyc_Toc_dyneither_ptr_typ_v={11,{{.UnknownAggr={1,{Cyc_Absyn_StructA,& Cyc_Toc__dyneither_ptr_pr,0}}},0}};
# 306
static void*Cyc_Toc_dyneither_ptr_typ=(void*)& Cyc_Toc_dyneither_ptr_typ_v;
# 308
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0};
# 310
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
static struct Cyc_Absyn_Stmt**skip_stmt_opt=0;
if(skip_stmt_opt == 0){
struct Cyc_Absyn_Stmt**_tmp98D;skip_stmt_opt=((_tmp98D=_cycalloc(sizeof(*_tmp98D)),((_tmp98D[0]=Cyc_Absyn_skip_stmt(0),_tmp98D))));}
return*skip_stmt_opt;}
# 319
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
void*_tmpC1=e->r;void*_tmpC2=_tmpC1;struct Cyc_Absyn_Exp*_tmpC3;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpC2)->tag == 13){if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpC2)->f4 == Cyc_Absyn_No_coercion){_LL1: _tmpC3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpC2)->f2;_LL2:
 return Cyc_Toc_cast_it(t,_tmpC3);}else{goto _LL3;}}else{_LL3: _LL4:
 return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}_LL0:;}
# 325
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp990;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp98F;return(void*)((_tmp98F=_cycalloc(sizeof(*_tmp98F)),((_tmp98F[0]=((_tmp990.tag=13,((_tmp990.f1=t,((_tmp990.f2=e,((_tmp990.f3=0,((_tmp990.f4=Cyc_Absyn_No_coercion,_tmp990)))))))))),_tmp98F))));}
# 328
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmp993;struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp992;return(void*)((_tmp992=_cycalloc(sizeof(*_tmp992)),((_tmp992[0]=((_tmp993.tag=19,((_tmp993.f1=e,_tmp993)))),_tmp992))));}
# 331
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmp996;struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp995;return(void*)((_tmp995=_cycalloc(sizeof(*_tmp995)),((_tmp995[0]=((_tmp996.tag=22,((_tmp996.f1=e1,((_tmp996.f2=e2,_tmp996)))))),_tmp995))));}
# 334
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmp999;struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp998;return(void*)((_tmp998=_cycalloc(sizeof(*_tmp998)),((_tmp998[0]=((_tmp999.tag=36,((_tmp999.f1=s,_tmp999)))),_tmp998))));}
# 337
static void*Cyc_Toc_sizeoftyp_exp_r(void*t){
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp99C;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp99B;return(void*)((_tmp99B=_cycalloc(sizeof(*_tmp99B)),((_tmp99B[0]=((_tmp99C.tag=16,((_tmp99C.f1=t,_tmp99C)))),_tmp99B))));}
# 340
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp99F;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp99E;return(void*)((_tmp99E=_cycalloc(sizeof(*_tmp99E)),((_tmp99E[0]=((_tmp99F.tag=9,((_tmp99F.f1=e,((_tmp99F.f2=es,((_tmp99F.f3=0,((_tmp99F.f4=1,_tmp99F)))))))))),_tmp99E))));}
# 343
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmp9A2;struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp9A1;return(void*)((_tmp9A1=_cycalloc(sizeof(*_tmp9A1)),((_tmp9A1[0]=((_tmp9A2.tag=1,((_tmp9A2.f1=e,_tmp9A2)))),_tmp9A1))));}
# 346
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp9A5;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp9A4;return(void*)((_tmp9A4=_cycalloc(sizeof(*_tmp9A4)),((_tmp9A4[0]=((_tmp9A5.tag=2,((_tmp9A5.f1=s1,((_tmp9A5.f2=s2,_tmp9A5)))))),_tmp9A4))));}
# 349
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp9A8;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp9A7;return(void*)((_tmp9A7=_cycalloc(sizeof(*_tmp9A7)),((_tmp9A7[0]=((_tmp9A8.tag=5,((_tmp9A8.f1=e1,((_tmp9A8.f2=e2,((_tmp9A8.f3=e3,_tmp9A8)))))))),_tmp9A7))));}
# 352
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmp9AB;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp9AA;return(void*)((_tmp9AA=_cycalloc(sizeof(*_tmp9AA)),((_tmp9AA[0]=((_tmp9AB.tag=20,((_tmp9AB.f1=e,((_tmp9AB.f2=n,((_tmp9AB.f3=0,((_tmp9AB.f4=0,_tmp9AB)))))))))),_tmp9AA))));}
# 355
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmp9AE;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp9AD;return(void*)((_tmp9AD=_cycalloc(sizeof(*_tmp9AD)),((_tmp9AD[0]=((_tmp9AE.tag=21,((_tmp9AE.f1=e,((_tmp9AE.f2=n,((_tmp9AE.f3=0,((_tmp9AE.f4=0,_tmp9AE)))))))))),_tmp9AD))));}
# 358
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 361
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp9B1;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp9B0;return(void*)((_tmp9B0=_cycalloc(sizeof(*_tmp9B0)),((_tmp9B0[0]=((_tmp9B1.tag=35,((_tmp9B1.f1=tdopt,((_tmp9B1.f2=ds,_tmp9B1)))))),_tmp9B0))));}
# 363
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmp9B4;struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp9B3;return(void*)((_tmp9B3=_cycalloc(sizeof(*_tmp9B3)),((_tmp9B3[0]=((_tmp9B4.tag=8,((_tmp9B4.f1=v,((_tmp9B4.f2=s,_tmp9B4)))))),_tmp9B3))));}
# 366
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 370
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,unsigned int loc){
void*_tmpDF=e->r;void*_tmpE0=_tmpDF;struct Cyc_Absyn_Exp*_tmpE1;if(((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpE0)->tag == 19){_LL6: _tmpE1=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_LL7:
 return Cyc_Absyn_aggrarrow_exp(_tmpE1,f,loc);}else{_LL8: _LL9:
 return Cyc_Absyn_aggrmember_exp(e,f,loc);}_LL5:;}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 387
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
# 396
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 405
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
# 414
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
# 425
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
struct Cyc_Absyn_Exp*function;
{void*_tmpE2=Cyc_Tcutil_compress(t);void*_tmpE3=_tmpE2;enum Cyc_Absyn_Size_of _tmpE4;switch(*((int*)_tmpE3)){case 6: _LLB: _tmpE4=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpE3)->f2;_LLC:
# 429
{enum Cyc_Absyn_Size_of _tmpE5=_tmpE4;switch(_tmpE5){case Cyc_Absyn_Char_sz: _LL18: _LL19:
 function=fS->fchar;goto _LL17;case Cyc_Absyn_Short_sz: _LL1A: _LL1B:
 function=fS->fshort;goto _LL17;case Cyc_Absyn_Int_sz: _LL1C: _LL1D:
 function=fS->fint;goto _LL17;default: _LL1E: _LL1F: {
struct Cyc_Core_Impossible_exn_struct _tmp9BA;const char*_tmp9B9;struct Cyc_Core_Impossible_exn_struct*_tmp9B8;(int)_throw((void*)((_tmp9B8=_cycalloc(sizeof(*_tmp9B8)),((_tmp9B8[0]=((_tmp9BA.tag=Cyc_Core_Impossible,((_tmp9BA.f1=((_tmp9B9="impossible IntType (not char, short or int)",_tag_dyneither(_tmp9B9,sizeof(char),44))),_tmp9BA)))),_tmp9B8)))));}}_LL17:;}
# 435
goto _LLA;case 7: switch(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpE3)->f1){case 0: _LLD: _LLE:
# 437
 function=fS->ffloat;
goto _LLA;case 1: _LLF: _LL10:
# 440
 function=fS->fdouble;
goto _LLA;default: _LL11: _LL12:
# 443
 function=fS->flongdouble;
goto _LLA;}case 5: _LL13: _LL14:
# 446
 function=fS->fvoidstar;
goto _LLA;default: _LL15: _LL16: {
# 449
struct Cyc_Core_Impossible_exn_struct _tmp9C7;const char*_tmp9C6;void*_tmp9C5[1];struct Cyc_String_pa_PrintArg_struct _tmp9C4;struct Cyc_Core_Impossible_exn_struct*_tmp9C3;(int)_throw((void*)((_tmp9C3=_cycalloc(sizeof(*_tmp9C3)),((_tmp9C3[0]=((_tmp9C7.tag=Cyc_Core_Impossible,((_tmp9C7.f1=(struct _dyneither_ptr)((_tmp9C4.tag=0,((_tmp9C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp9C5[0]=& _tmp9C4,Cyc_aprintf(((_tmp9C6="impossible expression type %s (not int, float, double, or pointer)",_tag_dyneither(_tmp9C6,sizeof(char),67))),_tag_dyneither(_tmp9C5,sizeof(void*),1)))))))),_tmp9C7)))),_tmp9C3)))));}}_LLA:;}
# 451
return function;}
# 453
struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
return Cyc_Toc_getFunctionType(fS,(void*)_check_null(arr->topt));}
# 456
struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmpEE=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_tmpEF=_tmpEE;void*_tmpF0;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpEF)->tag == 5){_LL21: _tmpF0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpEF)->f1).elt_typ;_LL22:
# 459
 return Cyc_Toc_getFunctionType(fS,_tmpF0);}else{_LL23: _LL24: {
struct Cyc_Core_Impossible_exn_struct _tmp9CD;const char*_tmp9CC;struct Cyc_Core_Impossible_exn_struct*_tmp9CB;(int)_throw((void*)((_tmp9CB=_cycalloc(sizeof(*_tmp9CB)),((_tmp9CB[0]=((_tmp9CD.tag=Cyc_Core_Impossible,((_tmp9CD.f1=((_tmp9CC="impossible type (not pointer)",_tag_dyneither(_tmp9CC,sizeof(char),30))),_tmp9CD)))),_tmp9CB)))));}}_LL20:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 467
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmpF4=e->r;void*_tmpF5=_tmpF4;struct Cyc_List_List*_tmpF6;struct Cyc_List_List*_tmpF7;struct Cyc_List_List*_tmpF8;struct Cyc_List_List*_tmpF9;struct Cyc_List_List*_tmpFA;struct Cyc_Absyn_Exp*_tmpFB;long long _tmpFC;int _tmpFD;short _tmpFE;struct _dyneither_ptr _tmpFF;char _tmp100;switch(*((int*)_tmpF5)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF5)->f1).Null_c).tag){case 2: _LL26: _tmp100=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF5)->f1).Char_c).val).f2;_LL27:
 return _tmp100 == '\000';case 3: _LL28: _tmpFF=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF5)->f1).Wchar_c).val;_LL29: {
# 471
unsigned long _tmp101=Cyc_strlen((struct _dyneither_ptr)_tmpFF);
int i=0;
if(_tmp101 >= 2  && *((const char*)_check_dyneither_subscript(_tmpFF,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmpFF,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmpFF,sizeof(char),1))== 'x'  && _tmp101 >= 3) && *((const char*)_check_dyneither_subscript(_tmpFF,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp101;++ i){
if(*((const char*)_check_dyneither_subscript(_tmpFF,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 481
return 0;}}case 4: _LL2A: _tmpFE=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF5)->f1).Short_c).val).f2;_LL2B:
 return _tmpFE == 0;case 5: _LL2C: _tmpFD=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF5)->f1).Int_c).val).f2;_LL2D:
 return _tmpFD == 0;case 6: _LL2E: _tmpFC=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF5)->f1).LongLong_c).val).f2;_LL2F:
 return _tmpFC == 0;case 1: _LL30: _LL31:
 return 1;default: goto _LL3E;}case 13: _LL32: _tmpFB=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpF5)->f2;_LL33:
 return Cyc_Toc_is_zero(_tmpFB);case 23: _LL34: _tmpFA=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpF5)->f1;_LL35:
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmpFA);case 25: _LL36: _tmpF9=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpF5)->f1;_LL37:
 _tmpF8=_tmpF9;goto _LL39;case 28: _LL38: _tmpF8=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpF5)->f3;_LL39:
 _tmpF7=_tmpF8;goto _LL3B;case 24: _LL3A: _tmpF7=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpF5)->f2;_LL3B:
 _tmpF6=_tmpF7;goto _LL3D;case 35: _LL3C: _tmpF6=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpF5)->f2;_LL3D:
# 492
 for(0;_tmpF6 != 0;_tmpF6=_tmpF6->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple19*)_tmpF6->hd)).f2))return 0;}
return 1;default: _LL3E: _LL3F:
 return 0;}_LL25:;}
# 500
static int Cyc_Toc_is_nullable(void*t){
void*_tmp102=Cyc_Tcutil_compress(t);void*_tmp103=_tmp102;union Cyc_Absyn_Constraint*_tmp104;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp103)->tag == 5){_LL41: _tmp104=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp103)->f1).ptr_atts).nullable;_LL42:
# 503
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp104);}else{_LL43: _LL44: {
const char*_tmp9D0;void*_tmp9CF;(_tmp9CF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp9D0="is_nullable",_tag_dyneither(_tmp9D0,sizeof(char),12))),_tag_dyneither(_tmp9CF,sizeof(void*),0)));}}_LL40:;}
# 509
static struct _dyneither_ptr Cyc_Toc_collapse_qvar(struct _dyneither_ptr s,struct _tuple1*x){
unsigned int slen=Cyc_strlen((struct _dyneither_ptr)s);
unsigned int len=9 + slen;
union Cyc_Absyn_Nmspace _tmp108;struct _dyneither_ptr _tmp109;struct _tuple1*_tmp107=x;_tmp108=_tmp107->f1;_tmp109=*_tmp107->f2;{
unsigned int vlen=Cyc_strlen((struct _dyneither_ptr)_tmp109);
len +=1 + vlen;{
struct Cyc_List_List*nms;
{union Cyc_Absyn_Nmspace _tmp10A=_tmp108;struct Cyc_List_List*_tmp10B;struct Cyc_List_List*_tmp10C;struct Cyc_List_List*_tmp10D;switch((_tmp10A.Abs_n).tag){case 4: _LL46: _LL47:
 nms=0;goto _LL45;case 1: _LL48: _tmp10D=(_tmp10A.Rel_n).val;_LL49:
 nms=_tmp10D;goto _LL45;case 2: _LL4A: _tmp10C=(_tmp10A.Abs_n).val;_LL4B:
 nms=_tmp10C;goto _LL45;default: _LL4C: _tmp10B=(_tmp10A.C_n).val;_LL4D:
 nms=_tmp10B;goto _LL45;}_LL45:;}
# 522
{struct Cyc_List_List*_tmp10E=nms;for(0;_tmp10E != 0;_tmp10E=_tmp10E->tl){
len +=1 + Cyc_strlen((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp10E->hd));}}{
char*_tmp9D2;unsigned int _tmp9D1;struct _dyneither_ptr buf=(_tmp9D1=len,((_tmp9D2=_cyccalloc_atomic(sizeof(char),_tmp9D1),_tag_dyneither(_tmp9D2,sizeof(char),_tmp9D1))));
struct _dyneither_ptr p=buf;
Cyc_strcpy(p,(struct _dyneither_ptr)s);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)slen);
for(0;nms != 0;nms=nms->tl){
struct _dyneither_ptr*_tmp10F=(struct _dyneither_ptr*)nms->hd;struct _dyneither_ptr _tmp111;struct _dyneither_ptr*_tmp110=_tmp10F;_tmp111=*_tmp110;
{char _tmp9D5;char _tmp9D4;struct _dyneither_ptr _tmp9D3;(_tmp9D3=p,((_tmp9D4=*((char*)_check_dyneither_subscript(_tmp9D3,sizeof(char),0)),((_tmp9D5='_',((_get_dyneither_size(_tmp9D3,sizeof(char))== 1  && (_tmp9D4 == '\000'  && _tmp9D5 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp9D3.curr)=_tmp9D5)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp111);
_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)Cyc_strlen((struct _dyneither_ptr)_tmp111));}
# 533
{char _tmp9D8;char _tmp9D7;struct _dyneither_ptr _tmp9D6;(_tmp9D6=p,((_tmp9D7=*((char*)_check_dyneither_subscript(_tmp9D6,sizeof(char),0)),((_tmp9D8='_',((_get_dyneither_size(_tmp9D6,sizeof(char))== 1  && (_tmp9D7 == '\000'  && _tmp9D8 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp9D6.curr)=_tmp9D8)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp109);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)vlen);
{const char*_tmp9D9;Cyc_strcpy(p,((_tmp9D9="_struct",_tag_dyneither(_tmp9D9,sizeof(char),8))));}
return(struct _dyneither_ptr)buf;};};};}struct _tuple20{struct Cyc_Toc_TocState*f1;struct _tuple14*f2;};static char _tmp130[8]="*bogus*";
# 545
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple20*env){
# 548
static struct _dyneither_ptr bogus_string={_tmp130,_tmp130,_tmp130 + 8};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
static struct _tuple14 pair={& bogus_qvar,& bogus_qvar};
# 552
struct _tuple20 _tmp11B=*env;struct Cyc_Dict_Dict*_tmp11D;struct _tuple14*_tmp11E;struct _tuple20 _tmp11C=_tmp11B;_tmp11D=(_tmp11C.f1)->qvar_tags;_tmp11E=_tmp11C.f2;{
struct _tuple14 _tmp11F=*_tmp11E;struct _tuple1*_tmp121;struct _tuple1*_tmp122;struct _tuple14 _tmp120=_tmp11F;_tmp121=_tmp120.f1;_tmp122=_tmp120.f2;{
struct _handler_cons _tmp123;_push_handler(& _tmp123);{int _tmp125=0;if(setjmp(_tmp123.handler))_tmp125=1;if(!_tmp125){
{struct _tuple1*_tmp126=((struct _tuple1*(*)(struct Cyc_Dict_Dict d,int(*cmp)(struct _tuple14*,struct _tuple14*),struct _tuple14*k))Cyc_Dict_lookup_other)(*_tmp11D,Cyc_Toc_qvar_tag_cmp,_tmp11E);_npop_handler(0);return _tmp126;};_pop_handler();}else{void*_tmp124=(void*)_exn_thrown;void*_tmp127=_tmp124;void*_tmp128;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp127)->tag == Cyc_Dict_Absent){_LL4F: _LL50: {
# 558
struct _tuple14*_tmp9DA;struct _tuple14*_tmp129=(_tmp9DA=_cycalloc(sizeof(*_tmp9DA)),((_tmp9DA->f1=_tmp121,((_tmp9DA->f2=_tmp122,_tmp9DA)))));
union Cyc_Absyn_Nmspace _tmp12B;struct _dyneither_ptr _tmp12C;struct _tuple1*_tmp12A=_tmp121;_tmp12B=_tmp12A->f1;_tmp12C=*_tmp12A->f2;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmp12C,_tmp122);
struct _dyneither_ptr*_tmp9DD;struct _tuple1*_tmp9DC;struct _tuple1*res=(_tmp9DC=_cycalloc(sizeof(*_tmp9DC)),((_tmp9DC->f1=_tmp12B,((_tmp9DC->f2=((_tmp9DD=_cycalloc(sizeof(*_tmp9DD)),((_tmp9DD[0]=newvar,_tmp9DD)))),_tmp9DC)))));
*_tmp11D=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmp11D,_tmp129,res);
return res;};}}else{_LL51: _tmp128=_tmp127;_LL52:(int)_rethrow(_tmp128);}_LL4E:;}};};};}
# 567
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple14 _tmp9DE;struct _tuple14 env=(_tmp9DE.f1=fieldname,((_tmp9DE.f2=dtname,_tmp9DE)));
return((struct _tuple1*(*)(struct _tuple14*arg,struct _tuple1*(*f)(struct _RegionHandle*,struct _tuple20*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_collapse_qvars_body);}
# 572
static void*Cyc_Toc_typ_to_c(void*t);struct _tuple21{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple22{void*f1;struct Cyc_List_List*f2;};
# 576
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple21*env){
# 579
struct _tuple21 _tmp132=*env;struct Cyc_List_List**_tmp134;struct Cyc_List_List*_tmp135;struct _tuple21 _tmp133=_tmp132;_tmp134=(_tmp133.f1)->tuple_types;_tmp135=_tmp133.f2;
# 581
{struct Cyc_List_List*_tmp136=*_tmp134;for(0;_tmp136 != 0;_tmp136=_tmp136->tl){
struct _tuple22*_tmp137=(struct _tuple22*)_tmp136->hd;void*_tmp139;struct Cyc_List_List*_tmp13A;struct _tuple22*_tmp138=_tmp137;_tmp139=_tmp138->f1;_tmp13A=_tmp138->f2;{
struct Cyc_List_List*_tmp13B=_tmp135;
for(0;_tmp13B != 0  && _tmp13A != 0;(_tmp13B=_tmp13B->tl,_tmp13A=_tmp13A->tl)){
if(!Cyc_Tcutil_unify((*((struct _tuple11*)_tmp13B->hd)).f2,(void*)_tmp13A->hd))
break;}
if(_tmp13B == 0  && _tmp13A == 0)
return _tmp139;};}}{
# 591
struct Cyc_Int_pa_PrintArg_struct _tmp9E6;void*_tmp9E5[1];const char*_tmp9E4;struct _dyneither_ptr*_tmp9E3;struct _dyneither_ptr*xname=(_tmp9E3=_cycalloc(sizeof(*_tmp9E3)),((_tmp9E3[0]=(struct _dyneither_ptr)((_tmp9E6.tag=1,((_tmp9E6.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmp9E5[0]=& _tmp9E6,Cyc_aprintf(((_tmp9E4="_tuple%d",_tag_dyneither(_tmp9E4,sizeof(char),9))),_tag_dyneither(_tmp9E5,sizeof(void*),1)))))))),_tmp9E3)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,_tmp135);
struct Cyc_List_List*_tmp13C=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
struct Cyc_Absyn_Aggrfield*_tmp9E9;struct Cyc_List_List*_tmp9E8;_tmp13C=((_tmp9E8=_cycalloc(sizeof(*_tmp9E8)),((_tmp9E8->hd=((_tmp9E9=_cycalloc(sizeof(*_tmp9E9)),((_tmp9E9->name=Cyc_Absyn_fieldname(i),((_tmp9E9->tq=Cyc_Toc_mt_tq,((_tmp9E9->type=(void*)ts2->hd,((_tmp9E9->width=0,((_tmp9E9->attributes=0,((_tmp9E9->requires_clause=0,_tmp9E9)))))))))))))),((_tmp9E8->tl=_tmp13C,_tmp9E8))))));}}
_tmp13C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp13C);{
struct Cyc_Absyn_AggrdeclImpl*_tmp9EE;struct _tuple1*_tmp9ED;struct Cyc_Absyn_Aggrdecl*_tmp9EC;struct Cyc_Absyn_Aggrdecl*_tmp13F=(_tmp9EC=_cycalloc(sizeof(*_tmp9EC)),((_tmp9EC->kind=Cyc_Absyn_StructA,((_tmp9EC->sc=Cyc_Absyn_Public,((_tmp9EC->name=(
(_tmp9ED=_cycalloc(sizeof(*_tmp9ED)),((_tmp9ED->f1=Cyc_Absyn_Rel_n(0),((_tmp9ED->f2=xname,_tmp9ED)))))),((_tmp9EC->tvs=0,((_tmp9EC->impl=(
(_tmp9EE=_cycalloc(sizeof(*_tmp9EE)),((_tmp9EE->exist_vars=0,((_tmp9EE->rgn_po=0,((_tmp9EE->fields=_tmp13C,((_tmp9EE->tagged=0,_tmp9EE)))))))))),((_tmp9EC->attributes=0,((_tmp9EC->expected_mem_kind=0,_tmp9EC)))))))))))))));
# 606
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp9F4;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp9F3;struct Cyc_List_List*_tmp9F2;Cyc_Toc_result_decls=((_tmp9F2=_cycalloc(sizeof(*_tmp9F2)),((_tmp9F2->hd=Cyc_Absyn_new_decl((void*)((_tmp9F4=_cycalloc(sizeof(*_tmp9F4)),((_tmp9F4[0]=((_tmp9F3.tag=5,((_tmp9F3.f1=_tmp13F,_tmp9F3)))),_tmp9F4)))),0),((_tmp9F2->tl=Cyc_Toc_result_decls,_tmp9F2))))));}
{struct _tuple22*_tmp9F7;struct Cyc_List_List*_tmp9F6;*_tmp134=((_tmp9F6=_region_malloc(d,sizeof(*_tmp9F6)),((_tmp9F6->hd=((_tmp9F7=_region_malloc(d,sizeof(*_tmp9F7)),((_tmp9F7->f1=x,((_tmp9F7->f2=ts,_tmp9F7)))))),((_tmp9F6->tl=*_tmp134,_tmp9F6))))));}
return x;};};}
# 611
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return((void*(*)(struct Cyc_List_List*arg,void*(*f)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state)(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple23{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple24{struct Cyc_Toc_TocState*f1;struct _tuple23*f2;};struct _tuple25{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 620
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple24*env){
# 628
struct _tuple24 _tmp14C=*env;struct Cyc_List_List**_tmp14E;struct _tuple1*_tmp14F;struct Cyc_List_List*_tmp150;struct Cyc_List_List*_tmp151;struct Cyc_List_List*_tmp152;struct _tuple24 _tmp14D=_tmp14C;_tmp14E=(_tmp14D.f1)->abs_struct_types;_tmp14F=(_tmp14D.f2)->f1;_tmp150=(_tmp14D.f2)->f2;_tmp151=(_tmp14D.f2)->f3;_tmp152=(_tmp14D.f2)->f4;
# 633
{struct Cyc_List_List*_tmp153=*_tmp14E;for(0;_tmp153 != 0;_tmp153=_tmp153->tl){
struct _tuple25*_tmp154=(struct _tuple25*)_tmp153->hd;struct _tuple1*_tmp156;struct Cyc_List_List*_tmp157;void*_tmp158;struct _tuple25*_tmp155=_tmp154;_tmp156=_tmp155->f1;_tmp157=_tmp155->f2;_tmp158=_tmp155->f3;
# 636
if(Cyc_Absyn_qvar_cmp(_tmp156,_tmp14F)== 0  && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp151)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp157)){
int okay=1;
{struct Cyc_List_List*_tmp159=_tmp151;for(0;_tmp159 != 0;(_tmp159=_tmp159->tl,_tmp157=_tmp157->tl)){
void*_tmp15A=(void*)_tmp159->hd;
void*_tmp15B=(void*)((struct Cyc_List_List*)_check_null(_tmp157))->hd;
# 643
{struct Cyc_Absyn_Kind*_tmp15C=Cyc_Tcutil_typ_kind(_tmp15A);struct Cyc_Absyn_Kind*_tmp15D=_tmp15C;switch(((struct Cyc_Absyn_Kind*)_tmp15D)->kind){case Cyc_Absyn_EffKind: _LL54: _LL55:
 goto _LL57;case Cyc_Absyn_RgnKind: _LL56: _LL57:
# 648
 continue;default: _LL58: _LL59:
# 651
 if(Cyc_Tcutil_unify(_tmp15A,_tmp15B) || Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(_tmp15A),Cyc_Toc_typ_to_c(_tmp15B)))
# 653
continue;
# 656
okay=0;
goto _LL53;}_LL53:;}
# 659
break;}}
# 661
if(okay)
# 663
return _tmp158;}}}{
# 671
struct Cyc_Int_pa_PrintArg_struct _tmp9FF;void*_tmp9FE[1];const char*_tmp9FD;struct _dyneither_ptr*_tmp9FC;struct _dyneither_ptr*xname=(_tmp9FC=_cycalloc(sizeof(*_tmp9FC)),((_tmp9FC[0]=(struct _dyneither_ptr)((_tmp9FF.tag=1,((_tmp9FF.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmp9FE[0]=& _tmp9FF,Cyc_aprintf(((_tmp9FD="_tuple%d",_tag_dyneither(_tmp9FD,sizeof(char),9))),_tag_dyneither(_tmp9FE,sizeof(void*),1)))))))),_tmp9FC)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp15E=0;
# 675
{struct _tuple25*_tmpA02;struct Cyc_List_List*_tmpA01;*_tmp14E=((_tmpA01=_region_malloc(d,sizeof(*_tmpA01)),((_tmpA01->hd=((_tmpA02=_region_malloc(d,sizeof(*_tmpA02)),((_tmpA02->f1=_tmp14F,((_tmpA02->f2=_tmp151,((_tmpA02->f3=x,_tmpA02)))))))),((_tmpA01->tl=*_tmp14E,_tmpA01))))));}{
# 678
struct _RegionHandle _tmp161=_new_region("r");struct _RegionHandle*r=& _tmp161;_push_region(r);
{struct Cyc_List_List*_tmp162=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp150,_tmp151);
for(0;_tmp152 != 0;_tmp152=_tmp152->tl){
struct Cyc_Absyn_Aggrfield*_tmp163=(struct Cyc_Absyn_Aggrfield*)_tmp152->hd;
void*t=_tmp163->type;
struct Cyc_List_List*atts=_tmp163->attributes;
# 686
if(_tmp152->tl == 0  && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(t))){
struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpA08;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpA07;struct Cyc_List_List*_tmpA06;atts=((_tmpA06=_cycalloc(sizeof(*_tmpA06)),((_tmpA06->hd=(void*)((_tmpA08=_cycalloc(sizeof(*_tmpA08)),((_tmpA08[0]=((_tmpA07.tag=6,((_tmpA07.f1=0,_tmpA07)))),_tmpA08)))),((_tmpA06->tl=atts,_tmpA06))))));}
# 689
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp162,t));
# 692
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val)){
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpA0E;struct Cyc_Absyn_ArrayInfo _tmpA0D;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpA0C;t=(void*)((_tmpA0C=_cycalloc(sizeof(*_tmpA0C)),((_tmpA0C[0]=((_tmpA0E.tag=8,((_tmpA0E.f1=((_tmpA0D.elt_type=Cyc_Absyn_void_star_typ(),((_tmpA0D.tq=Cyc_Absyn_empty_tqual(0),((_tmpA0D.num_elts=
Cyc_Absyn_uint_exp(0,0),((_tmpA0D.zero_term=((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0),((_tmpA0D.zt_loc=0,_tmpA0D)))))))))),_tmpA0E)))),_tmpA0C))));}{
# 696
struct Cyc_Absyn_Aggrfield*_tmpA11;struct Cyc_List_List*_tmpA10;_tmp15E=((_tmpA10=_cycalloc(sizeof(*_tmpA10)),((_tmpA10->hd=((_tmpA11=_cycalloc(sizeof(*_tmpA11)),((_tmpA11->name=_tmp163->name,((_tmpA11->tq=Cyc_Toc_mt_tq,((_tmpA11->type=t,((_tmpA11->width=_tmp163->width,((_tmpA11->attributes=atts,((_tmpA11->requires_clause=0,_tmpA11)))))))))))))),((_tmpA10->tl=_tmp15E,_tmpA10))))));};}
# 698
_tmp15E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp15E);{
struct Cyc_Absyn_AggrdeclImpl*_tmpA16;struct _tuple1*_tmpA15;struct Cyc_Absyn_Aggrdecl*_tmpA14;struct Cyc_Absyn_Aggrdecl*_tmp16C=(_tmpA14=_cycalloc(sizeof(*_tmpA14)),((_tmpA14->kind=Cyc_Absyn_StructA,((_tmpA14->sc=Cyc_Absyn_Public,((_tmpA14->name=(
(_tmpA15=_cycalloc(sizeof(*_tmpA15)),((_tmpA15->f1=Cyc_Absyn_Rel_n(0),((_tmpA15->f2=xname,_tmpA15)))))),((_tmpA14->tvs=0,((_tmpA14->impl=(
(_tmpA16=_cycalloc(sizeof(*_tmpA16)),((_tmpA16->exist_vars=0,((_tmpA16->rgn_po=0,((_tmpA16->fields=_tmp15E,((_tmpA16->tagged=0,_tmpA16)))))))))),((_tmpA14->attributes=0,((_tmpA14->expected_mem_kind=0,_tmpA14)))))))))))))));
# 706
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpA1C;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpA1B;struct Cyc_List_List*_tmpA1A;Cyc_Toc_result_decls=((_tmpA1A=_cycalloc(sizeof(*_tmpA1A)),((_tmpA1A->hd=Cyc_Absyn_new_decl((void*)((_tmpA1C=_cycalloc(sizeof(*_tmpA1C)),((_tmpA1C[0]=((_tmpA1B.tag=5,((_tmpA1B.f1=_tmp16C,_tmpA1B)))),_tmpA1C)))),0),((_tmpA1A->tl=Cyc_Toc_result_decls,_tmpA1A))))));}{
void*_tmp170=x;_npop_handler(0);return _tmp170;};};}
# 679
;_pop_region(r);};};}
# 710
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 714
struct _tuple23 _tmpA1D;struct _tuple23 env=(_tmpA1D.f1=struct_name,((_tmpA1D.f2=type_vars,((_tmpA1D.f3=type_args,((_tmpA1D.f4=fields,_tmpA1D)))))));
return((void*(*)(struct _tuple23*arg,void*(*f)(struct _RegionHandle*,struct _tuple24*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 721
struct _tuple1*Cyc_Toc_temp_var(){
int _tmp179=Cyc_Toc_temp_var_counter ++;
struct _dyneither_ptr*_tmpA2A;const char*_tmpA29;void*_tmpA28[1];struct Cyc_Int_pa_PrintArg_struct _tmpA27;struct _tuple1*_tmpA26;struct _tuple1*res=(_tmpA26=_cycalloc(sizeof(*_tmpA26)),((_tmpA26->f1=Cyc_Absyn_Loc_n,((_tmpA26->f2=((_tmpA2A=_cycalloc(sizeof(*_tmpA2A)),((_tmpA2A[0]=(struct _dyneither_ptr)((_tmpA27.tag=1,((_tmpA27.f1=(unsigned int)_tmp179,((_tmpA28[0]=& _tmpA27,Cyc_aprintf(((_tmpA29="_tmp%X",_tag_dyneither(_tmpA29,sizeof(char),7))),_tag_dyneither(_tmpA28,sizeof(void*),1)))))))),_tmpA2A)))),_tmpA26)))));
return res;}struct _tuple26{struct Cyc_Toc_TocState*f1;int f2;};
# 729
static struct _dyneither_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple26*env){
struct _tuple26 _tmp17F=*env;struct Cyc_Xarray_Xarray*_tmp181;struct _tuple26 _tmp180=_tmp17F;_tmp181=(_tmp180.f1)->temp_labels;{
int _tmp182=Cyc_Toc_fresh_label_counter ++;
if(_tmp182 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp181))
return((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp181,_tmp182);{
struct Cyc_Int_pa_PrintArg_struct _tmpA32;void*_tmpA31[1];const char*_tmpA30;struct _dyneither_ptr*_tmpA2F;struct _dyneither_ptr*res=(_tmpA2F=_cycalloc(sizeof(*_tmpA2F)),((_tmpA2F[0]=(struct _dyneither_ptr)((_tmpA32.tag=1,((_tmpA32.f1=(unsigned int)_tmp182,((_tmpA31[0]=& _tmpA32,Cyc_aprintf(((_tmpA30="_LL%X",_tag_dyneither(_tmpA30,sizeof(char),6))),_tag_dyneither(_tmpA31,sizeof(void*),1)))))))),_tmpA2F)));
if(((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp181,res)!= _tmp182){
const char*_tmpA35;void*_tmpA34;(_tmpA34=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA35="fresh_label: add_ind returned bad index...",_tag_dyneither(_tmpA35,sizeof(char),43))),_tag_dyneither(_tmpA34,sizeof(void*),0)));}
return res;};};}
# 740
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
return((struct _dyneither_ptr*(*)(int arg,struct _dyneither_ptr*(*f)(struct _RegionHandle*,struct _tuple26*)))Cyc_Toc_use_toc_state)(0,Cyc_Toc_fresh_label_body);}
# 747
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
struct Cyc_List_List*_tmp189=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp189))->hd)->name)!= 0){
++ ans;
_tmp189=_tmp189->tl;}
# 754
return Cyc_Absyn_uint_exp((unsigned int)ans,0);}
# 760
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_result_decls);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
# 765
struct _tuple9 _tmp18A=*a;struct _dyneither_ptr*_tmp18C;struct Cyc_Absyn_Tqual _tmp18D;void*_tmp18E;struct _tuple9 _tmp18B=_tmp18A;_tmp18C=_tmp18B.f1;_tmp18D=_tmp18B.f2;_tmp18E=_tmp18B.f3;{
struct _tuple9*_tmpA36;return(_tmpA36=_cycalloc(sizeof(*_tmpA36)),((_tmpA36->f1=_tmp18C,((_tmpA36->f2=_tmp18D,((_tmpA36->f3=Cyc_Toc_typ_to_c(_tmp18E),_tmpA36)))))));};}
# 768
static struct _tuple11*Cyc_Toc_typ_to_c_f(struct _tuple11*x){
struct _tuple11 _tmp190=*x;struct Cyc_Absyn_Tqual _tmp192;void*_tmp193;struct _tuple11 _tmp191=_tmp190;_tmp192=_tmp191.f1;_tmp193=_tmp191.f2;{
struct _tuple11*_tmpA37;return(_tmpA37=_cycalloc(sizeof(*_tmpA37)),((_tmpA37->f1=_tmp192,((_tmpA37->f2=Cyc_Toc_typ_to_c(_tmp193),_tmpA37)))));};}
# 787 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp195=Cyc_Tcutil_compress(t);void*_tmp196=_tmp195;void*_tmp197;void*_tmp198;struct Cyc_Absyn_Tqual _tmp199;struct Cyc_Absyn_Exp*_tmp19A;union Cyc_Absyn_Constraint*_tmp19B;unsigned int _tmp19C;switch(*((int*)_tmp196)){case 8: _LL5B: _tmp198=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp196)->f1).elt_type;_tmp199=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp196)->f1).tq;_tmp19A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp196)->f1).num_elts;_tmp19B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp196)->f1).zero_term;_tmp19C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp196)->f1).zt_loc;_LL5C:
# 790
 return Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(_tmp198),_tmp199);case 1: _LL5D: _tmp197=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp196)->f2;if(_tmp197 != 0){_LL5E:
 return Cyc_Toc_typ_to_c_array(_tmp197);}else{goto _LL5F;}default: _LL5F: _LL60:
 return Cyc_Toc_typ_to_c(t);}_LL5A:;}
# 796
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){
# 798
struct Cyc_Absyn_Aggrfield*_tmpA38;return(_tmpA38=_cycalloc(sizeof(*_tmpA38)),((_tmpA38->name=f->name,((_tmpA38->tq=Cyc_Toc_mt_tq,((_tmpA38->type=
# 800
Cyc_Toc_typ_to_c(f->type),((_tmpA38->width=f->width,((_tmpA38->attributes=f->attributes,((_tmpA38->requires_clause=0,_tmpA38)))))))))))));}
# 805
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 807
return;}
# 810
static void*Cyc_Toc_char_star_typ(){
static void**cs=0;
if(cs == 0){
void**_tmpA39;cs=((_tmpA39=_cycalloc(sizeof(*_tmpA39)),((_tmpA39[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpA39))));}
# 815
return*cs;}
# 817
static void*Cyc_Toc_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpA3A;r=((_tmpA3A=_cycalloc(sizeof(*_tmpA3A)),((_tmpA3A[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),_tmpA3A))));}
# 822
return*r;}
# 824
static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpA3B;r=((_tmpA3B=_cycalloc(sizeof(*_tmpA3B)),((_tmpA3B[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),_tmpA3B))));}
# 829
return*r;}
# 831
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp1A1=Cyc_Tcutil_compress(t);void*_tmp1A2=_tmp1A1;struct Cyc_Absyn_Tvar*_tmp1A3;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A2)->tag == 2){_LL62: _tmp1A3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A2)->f1;_LL63:
# 834
 return Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tbk);}else{_LL64: _LL65:
 return 0;}_LL61:;}
# 838
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp1A4=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp1A5=_tmp1A4;if(((struct Cyc_Absyn_Kind*)_tmp1A5)->kind == Cyc_Absyn_AnyKind){_LL67: _LL68:
 return 1;}else{_LL69: _LL6A:
 return 0;}_LL66:;}
# 845
static void*Cyc_Toc_typ_to_c(void*t){
void*_tmp1A6=t;struct Cyc_Absyn_Datatypedecl*_tmp1A7;void**_tmp1A8;struct Cyc_Absyn_Enumdecl*_tmp1A9;struct Cyc_Absyn_Aggrdecl*_tmp1AA;struct Cyc_Absyn_Exp*_tmp1AB;struct Cyc_Absyn_Exp*_tmp1AC;void*_tmp1AD;struct _tuple1*_tmp1AE;struct Cyc_List_List*_tmp1AF;struct Cyc_Absyn_Typedefdecl*_tmp1B0;void*_tmp1B1;struct Cyc_List_List*_tmp1B2;struct _tuple1*_tmp1B3;union Cyc_Absyn_AggrInfoU _tmp1B4;struct Cyc_List_List*_tmp1B5;enum Cyc_Absyn_AggrKind _tmp1B6;struct Cyc_List_List*_tmp1B7;struct Cyc_List_List*_tmp1B8;struct Cyc_Absyn_Tqual _tmp1B9;void*_tmp1BA;struct Cyc_List_List*_tmp1BB;int _tmp1BC;struct Cyc_Absyn_VarargInfo*_tmp1BD;struct Cyc_List_List*_tmp1BE;void*_tmp1BF;struct Cyc_Absyn_Tqual _tmp1C0;struct Cyc_Absyn_Exp*_tmp1C1;unsigned int _tmp1C2;void*_tmp1C3;struct Cyc_Absyn_Tqual _tmp1C4;union Cyc_Absyn_Constraint*_tmp1C5;struct Cyc_Absyn_Datatypedecl*_tmp1C6;struct Cyc_Absyn_Datatypefield*_tmp1C7;struct Cyc_Absyn_Tvar*_tmp1C8;void**_tmp1C9;switch(*((int*)_tmp1A6)){case 0: _LL6C: _LL6D:
 return t;case 1: _LL6E: _tmp1C9=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A6)->f2;_LL6F:
# 849
 if(*_tmp1C9 == 0){
*_tmp1C9=Cyc_Absyn_sint_typ;
return Cyc_Absyn_sint_typ;}
# 853
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp1C9));case 2: _LL70: _tmp1C8=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A6)->f1;_LL71:
# 855
 if((Cyc_Tcutil_tvar_kind(_tmp1C8,& Cyc_Tcutil_bk))->kind == Cyc_Absyn_AnyKind)
# 858
return(void*)& Cyc_Absyn_VoidType_val;else{
# 860
return Cyc_Absyn_void_star_typ();}case 3: _LL72: _LL73:
# 862
 return(void*)& Cyc_Absyn_VoidType_val;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1A6)->f1).field_info).KnownDatatypefield).tag == 2){_LL74: _tmp1C6=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1A6)->f1).field_info).KnownDatatypefield).val).f1;_tmp1C7=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1A6)->f1).field_info).KnownDatatypefield).val).f2;_LL75:
# 864
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp1C7->name,_tmp1C6->name));}else{_LL76: _LL77: {
const char*_tmpA3E;void*_tmpA3D;(_tmpA3D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA3E="unresolved DatatypeFieldType",_tag_dyneither(_tmpA3E,sizeof(char),29))),_tag_dyneither(_tmpA3D,sizeof(void*),0)));}}case 5: _LL78: _tmp1C3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A6)->f1).elt_typ;_tmp1C4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A6)->f1).elt_tq;_tmp1C5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A6)->f1).ptr_atts).bounds;_LL79:
# 869
 _tmp1C3=Cyc_Toc_typ_to_c(_tmp1C3);{
void*_tmp1CC=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp1C5);void*_tmp1CD=_tmp1CC;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1CD)->tag == 0){_LLAD: _LLAE:
 return Cyc_Toc_dyneither_ptr_typ;}else{_LLAF: _LLB0:
 return Cyc_Absyn_star_typ(_tmp1C3,(void*)& Cyc_Absyn_HeapRgn_val,_tmp1C4,Cyc_Absyn_false_conref);}_LLAC:;};case 6: _LL7A: _LL7B:
# 874
 goto _LL7D;case 7: _LL7C: _LL7D:
 return t;case 8: _LL7E: _tmp1BF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1A6)->f1).elt_type;_tmp1C0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1A6)->f1).tq;_tmp1C1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1A6)->f1).num_elts;_tmp1C2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1A6)->f1).zt_loc;_LL7F:
# 877
 return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c(_tmp1BF),_tmp1C0,_tmp1C1,Cyc_Absyn_false_conref,_tmp1C2);case 9: _LL80: _tmp1B9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1A6)->f1).ret_tqual;_tmp1BA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1A6)->f1).ret_typ;_tmp1BB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1A6)->f1).args;_tmp1BC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1A6)->f1).c_varargs;_tmp1BD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1A6)->f1).cyc_varargs;_tmp1BE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1A6)->f1).attributes;_LL81: {
# 883
struct Cyc_List_List*_tmp1CE=0;
for(0;_tmp1BE != 0;_tmp1BE=_tmp1BE->tl){
void*_tmp1CF=(void*)_tmp1BE->hd;void*_tmp1D0=_tmp1CF;switch(*((int*)_tmp1D0)){case 4: _LLB2: _LLB3:
 goto _LLB5;case 5: _LLB4: _LLB5:
 goto _LLB7;case 19: _LLB6: _LLB7:
 continue;case 22: _LLB8: _LLB9:
 continue;case 21: _LLBA: _LLBB:
 continue;case 20: _LLBC: _LLBD:
 continue;default: _LLBE: _LLBF:
{struct Cyc_List_List*_tmpA3F;_tmp1CE=((_tmpA3F=_cycalloc(sizeof(*_tmpA3F)),((_tmpA3F->hd=(void*)_tmp1BE->hd,((_tmpA3F->tl=_tmp1CE,_tmpA3F))))));}goto _LLB1;}_LLB1:;}{
# 894
struct Cyc_List_List*_tmp1D2=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp1BB);
if(_tmp1BD != 0){
# 897
void*_tmp1D3=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp1BD->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple9*_tmpA40;struct _tuple9*_tmp1D4=(_tmpA40=_cycalloc(sizeof(*_tmpA40)),((_tmpA40->f1=_tmp1BD->name,((_tmpA40->f2=_tmp1BD->tq,((_tmpA40->f3=_tmp1D3,_tmpA40)))))));
struct Cyc_List_List*_tmpA41;_tmp1D2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1D2,((_tmpA41=_cycalloc(sizeof(*_tmpA41)),((_tmpA41->hd=_tmp1D4,((_tmpA41->tl=0,_tmpA41)))))));}{
# 901
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpA47;struct Cyc_Absyn_FnInfo _tmpA46;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpA45;return(void*)((_tmpA45=_cycalloc(sizeof(*_tmpA45)),((_tmpA45[0]=((_tmpA47.tag=9,((_tmpA47.f1=((_tmpA46.tvars=0,((_tmpA46.effect=0,((_tmpA46.ret_tqual=_tmp1B9,((_tmpA46.ret_typ=Cyc_Toc_typ_to_c(_tmp1BA),((_tmpA46.args=_tmp1D2,((_tmpA46.c_varargs=_tmp1BC,((_tmpA46.cyc_varargs=0,((_tmpA46.rgn_po=0,((_tmpA46.attributes=_tmp1CE,((_tmpA46.requires_clause=0,((_tmpA46.requires_relns=0,((_tmpA46.ensures_clause=0,((_tmpA46.ensures_relns=0,_tmpA46)))))))))))))))))))))))))),_tmpA47)))),_tmpA45))));};};}case 10: _LL82: _tmp1B8=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1A6)->f1;_LL83:
# 906
 _tmp1B8=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp1B8);
return Cyc_Toc_add_tuple_type(_tmp1B8);case 12: _LL84: _tmp1B6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1A6)->f1;_tmp1B7=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1A6)->f2;_LL85: {
# 911
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpA4A;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpA49;return(void*)((_tmpA49=_cycalloc(sizeof(*_tmpA49)),((_tmpA49[0]=((_tmpA4A.tag=12,((_tmpA4A.f1=_tmp1B6,((_tmpA4A.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp1B7),_tmpA4A)))))),_tmpA49))));}case 11: _LL86: _tmp1B4=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A6)->f1).aggr_info;_tmp1B5=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A6)->f1).targs;_LL87:
# 915
{union Cyc_Absyn_AggrInfoU _tmp1DC=_tmp1B4;if((_tmp1DC.UnknownAggr).tag == 1){_LLC1: _LLC2:
 return t;}else{_LLC3: _LLC4:
 goto _LLC0;}_LLC0:;}{
# 919
struct Cyc_Absyn_Aggrdecl*_tmp1DD=Cyc_Absyn_get_known_aggrdecl(_tmp1B4);
if(_tmp1DD->expected_mem_kind){
# 922
if(_tmp1DD->impl == 0){
const char*_tmpA53;void*_tmpA52[2];const char*_tmpA51;const char*_tmpA50;struct Cyc_String_pa_PrintArg_struct _tmpA4F;struct Cyc_String_pa_PrintArg_struct _tmpA4E;(_tmpA4E.tag=0,((_tmpA4E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 925
Cyc_Absynpp_qvar2string(_tmp1DD->name)),((_tmpA4F.tag=0,((_tmpA4F.f1=(struct _dyneither_ptr)(
# 924
_tmp1DD->kind == Cyc_Absyn_UnionA?(_tmpA50="union",_tag_dyneither(_tmpA50,sizeof(char),6)):((_tmpA51="struct",_tag_dyneither(_tmpA51,sizeof(char),7)))),((_tmpA52[0]=& _tmpA4F,((_tmpA52[1]=& _tmpA4E,Cyc_Tcutil_warn(0,((_tmpA53="%s %s was never defined.",_tag_dyneither(_tmpA53,sizeof(char),25))),_tag_dyneither(_tmpA52,sizeof(void*),2)))))))))))));}}
# 930
if(_tmp1DD->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp1DD->name,Cyc_Absyn_unionq_typ);{
struct Cyc_List_List*_tmp1E4=_tmp1DD->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1DD->impl))->fields;
if(_tmp1E4 == 0)return Cyc_Toc_aggrdecl_type(_tmp1DD->name,Cyc_Absyn_strctq);
for(0;_tmp1E4->tl != 0;_tmp1E4=_tmp1E4->tl){;}{
void*_tmp1E5=((struct Cyc_Absyn_Aggrfield*)_tmp1E4->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp1E5))){
if(_tmp1DD->expected_mem_kind){
const char*_tmpA57;void*_tmpA56[1];struct Cyc_String_pa_PrintArg_struct _tmpA55;(_tmpA55.tag=0,((_tmpA55.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp1DD->name)),((_tmpA56[0]=& _tmpA55,Cyc_Tcutil_warn(0,((_tmpA57="struct %s ended up being abstract.",_tag_dyneither(_tmpA57,sizeof(char),35))),_tag_dyneither(_tmpA56,sizeof(void*),1)))))));}{
# 944
struct _RegionHandle _tmp1E9=_new_region("r");struct _RegionHandle*r=& _tmp1E9;_push_region(r);
{struct Cyc_List_List*_tmp1EA=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp1DD->tvs,_tmp1B5);
void*_tmp1EB=Cyc_Tcutil_rsubstitute(r,_tmp1EA,_tmp1E5);
if(Cyc_Toc_is_abstract_type(_tmp1EB)){void*_tmp1EC=Cyc_Toc_aggrdecl_type(_tmp1DD->name,Cyc_Absyn_strctq);_npop_handler(0);return _tmp1EC;}{
void*_tmp1ED=Cyc_Toc_add_struct_type(_tmp1DD->name,_tmp1DD->tvs,_tmp1B5,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1DD->impl))->fields);_npop_handler(0);return _tmp1ED;};}
# 945
;_pop_region(r);};}
# 950
return Cyc_Toc_aggrdecl_type(_tmp1DD->name,Cyc_Absyn_strctq);};};};case 13: _LL88: _tmp1B3=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp1A6)->f1;_LL89:
 return t;case 14: _LL8A: _tmp1B2=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1A6)->f1;_LL8B:
 Cyc_Toc_enumfields_to_c(_tmp1B2);return t;case 17: _LL8C: _tmp1AE=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1A6)->f1;_tmp1AF=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1A6)->f2;_tmp1B0=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1A6)->f3;_tmp1B1=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1A6)->f4;_LL8D:
# 954
 if(_tmp1B1 == 0  || Cyc_noexpand_r){
if(_tmp1AF != 0){
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpA5A;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA59;return(void*)((_tmpA59=_cycalloc(sizeof(*_tmpA59)),((_tmpA59[0]=((_tmpA5A.tag=17,((_tmpA5A.f1=_tmp1AE,((_tmpA5A.f2=0,((_tmpA5A.f3=_tmp1B0,((_tmpA5A.f4=0,_tmpA5A)))))))))),_tmpA59))));}else{
return t;}}else{
# 959
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpA5D;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA5C;return(void*)((_tmpA5C=_cycalloc(sizeof(*_tmpA5C)),((_tmpA5C[0]=((_tmpA5D.tag=17,((_tmpA5D.f1=_tmp1AE,((_tmpA5D.f2=0,((_tmpA5D.f3=_tmp1B0,((_tmpA5D.f4=Cyc_Toc_typ_to_c(_tmp1B1),_tmpA5D)))))))))),_tmpA5C))));}case 19: _LL8E: _LL8F:
 return Cyc_Absyn_uint_typ;case 15: _LL90: _tmp1AD=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1A6)->f1;_LL91:
 return Cyc_Toc_rgn_typ();case 16: _LL92: _LL93:
 return Cyc_Toc_dyn_rgn_typ();case 20: _LL94: _LL95:
# 965
 goto _LL97;case 21: _LL96: _LL97:
 goto _LL99;case 22: _LL98: _LL99:
 goto _LL9B;case 23: _LL9A: _LL9B:
 goto _LL9D;case 24: _LL9C: _LL9D:
 goto _LL9F;case 25: _LL9E: _LL9F:
 return Cyc_Absyn_void_star_typ();case 18: _LLA0: _tmp1AC=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp1A6)->f1;_LLA1:
# 975
 return t;case 27: _LLA2: _tmp1AB=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp1A6)->f1;_LLA3:
# 977
 return t;case 28: _LLA4: _LLA5:
 return t;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1A6)->f1)->r)){case 0: _LLA6: _tmp1AA=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1A6)->f1)->r)->f1;_LLA7:
# 980
 Cyc_Toc_aggrdecl_to_c(_tmp1AA,1);
if(_tmp1AA->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp1AA->name,Cyc_Absyn_unionq_typ);else{
return Cyc_Toc_aggrdecl_type(_tmp1AA->name,Cyc_Absyn_strctq);}case 1: _LLA8: _tmp1A9=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1A6)->f1)->r)->f1;_LLA9:
# 985
 Cyc_Toc_enumdecl_to_c(_tmp1A9);
return t;default: _LLAA: _tmp1A7=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1A6)->f1)->r)->f1;_tmp1A8=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1A6)->f2;_LLAB:
# 988
 Cyc_Toc_datatypedecl_to_c(_tmp1A7);
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp1A8)));}}_LL6B:;}
# 993
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned int l){
void*_tmp1F2=t;void*_tmp1F3;struct Cyc_Absyn_Tqual _tmp1F4;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F2)->tag == 8){_LLC6: _tmp1F3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F2)->f1).elt_type;_tmp1F4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F2)->f1).tq;_LLC7:
# 996
 return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp1F3,(void*)& Cyc_Absyn_HeapRgn_val,_tmp1F4,Cyc_Absyn_false_conref),e);}else{_LLC8: _LLC9:
 return Cyc_Toc_cast_it(t,e);}_LLC5:;}
# 1003
static int Cyc_Toc_atomic_typ(void*t){
void*_tmp1F5=Cyc_Tcutil_compress(t);void*_tmp1F6=_tmp1F5;struct Cyc_List_List*_tmp1F7;struct Cyc_Absyn_Datatypedecl*_tmp1F8;struct Cyc_Absyn_Datatypefield*_tmp1F9;struct Cyc_List_List*_tmp1FA;union Cyc_Absyn_AggrInfoU _tmp1FB;void*_tmp1FC;switch(*((int*)_tmp1F6)){case 0: _LLCB: _LLCC:
 return 1;case 2: _LLCD: _LLCE:
 return 0;case 6: _LLCF: _LLD0:
 goto _LLD2;case 13: _LLD1: _LLD2:
 goto _LLD4;case 14: _LLD3: _LLD4:
 goto _LLD6;case 7: _LLD5: _LLD6:
 goto _LLD8;case 9: _LLD7: _LLD8:
 goto _LLDA;case 19: _LLD9: _LLDA:
 return 1;case 8: _LLDB: _tmp1FC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F6)->f1).elt_type;_LLDC:
 return Cyc_Toc_atomic_typ(_tmp1FC);case 11: _LLDD: _tmp1FB=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1F6)->f1).aggr_info;_LLDE:
# 1020
{union Cyc_Absyn_AggrInfoU _tmp1FD=_tmp1FB;if((_tmp1FD.UnknownAggr).tag == 1){_LLF0: _LLF1:
 return 0;}else{_LLF2: _LLF3:
 goto _LLEF;}_LLEF:;}{
# 1024
struct Cyc_Absyn_Aggrdecl*_tmp1FE=Cyc_Absyn_get_known_aggrdecl(_tmp1FB);
if(_tmp1FE->impl == 0)
return 0;
{struct Cyc_List_List*_tmp1FF=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1FE->impl))->fields;for(0;_tmp1FF != 0;_tmp1FF=_tmp1FF->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp1FF->hd)->type))return 0;}}
return 1;};case 12: _LLDF: _tmp1FA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1F6)->f2;_LLE0:
# 1031
 for(0;_tmp1FA != 0;_tmp1FA=_tmp1FA->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp1FA->hd)->type))return 0;}
return 1;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1F6)->f1).field_info).KnownDatatypefield).tag == 2){_LLE1: _tmp1F8=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1F6)->f1).field_info).KnownDatatypefield).val).f1;_tmp1F9=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1F6)->f1).field_info).KnownDatatypefield).val).f2;_LLE2:
# 1035
 _tmp1F7=_tmp1F9->typs;goto _LLE4;}else{goto _LLED;}case 10: _LLE3: _tmp1F7=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1F6)->f1;_LLE4:
# 1037
 for(0;_tmp1F7 != 0;_tmp1F7=_tmp1F7->tl){
if(!Cyc_Toc_atomic_typ((*((struct _tuple11*)_tmp1F7->hd)).f2))return 0;}
return 1;case 3: _LLE5: _LLE6:
# 1042
 goto _LLE8;case 5: _LLE7: _LLE8:
 goto _LLEA;case 16: _LLE9: _LLEA:
 goto _LLEC;case 15: _LLEB: _LLEC:
 return 0;default: _LLED: _LLEE: {
const char*_tmpA61;void*_tmpA60[1];struct Cyc_String_pa_PrintArg_struct _tmpA5F;(_tmpA5F.tag=0,((_tmpA5F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA60[0]=& _tmpA5F,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA61="atomic_typ:  bad type %s",_tag_dyneither(_tmpA61,sizeof(char),25))),_tag_dyneither(_tmpA60,sizeof(void*),1)))))));}}_LLCA:;}
# 1050
static int Cyc_Toc_is_void_star(void*t){
void*_tmp203=Cyc_Tcutil_compress(t);void*_tmp204=_tmp203;void*_tmp205;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp204)->tag == 5){_LLF5: _tmp205=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp204)->f1).elt_typ;_LLF6: {
# 1053
void*_tmp206=Cyc_Tcutil_compress(_tmp205);void*_tmp207=_tmp206;if(((struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp207)->tag == 0){_LLFA: _LLFB:
 return 1;}else{_LLFC: _LLFD:
 return 0;}_LLF9:;}}else{_LLF7: _LLF8:
# 1057
 return 0;}_LLF4:;}
# 1061
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1065
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1070
static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){
void*_tmp208=Cyc_Tcutil_compress(t);void*_tmp209=_tmp208;struct Cyc_List_List*_tmp20A;union Cyc_Absyn_AggrInfoU _tmp20B;switch(*((int*)_tmp209)){case 11: _LLFF: _tmp20B=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp209)->f1).aggr_info;_LL100: {
# 1073
struct Cyc_Absyn_Aggrdecl*_tmp20C=Cyc_Absyn_get_known_aggrdecl(_tmp20B);
if(_tmp20C->impl == 0){
const char*_tmpA64;void*_tmpA63;(_tmpA63=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA64="is_poly_field: type missing fields",_tag_dyneither(_tmpA64,sizeof(char),35))),_tag_dyneither(_tmpA63,sizeof(void*),0)));}
_tmp20A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp20C->impl))->fields;goto _LL102;}case 12: _LL101: _tmp20A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp209)->f2;_LL102: {
# 1078
struct Cyc_Absyn_Aggrfield*_tmp20F=Cyc_Absyn_lookup_field(_tmp20A,f);
if(_tmp20F == 0){
const char*_tmpA68;void*_tmpA67[1];struct Cyc_String_pa_PrintArg_struct _tmpA66;(_tmpA66.tag=0,((_tmpA66.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpA67[0]=& _tmpA66,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA68="is_poly_field: bad field %s",_tag_dyneither(_tmpA68,sizeof(char),28))),_tag_dyneither(_tmpA67,sizeof(void*),1)))))));}
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp20F->type);}default: _LL103: _LL104: {
const char*_tmpA6C;void*_tmpA6B[1];struct Cyc_String_pa_PrintArg_struct _tmpA6A;(_tmpA6A.tag=0,((_tmpA6A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA6B[0]=& _tmpA6A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA6C="is_poly_field: bad type %s",_tag_dyneither(_tmpA6C,sizeof(char),27))),_tag_dyneither(_tmpA6B,sizeof(void*),1)))))));}}_LLFE:;}
# 1089
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp216=e->r;void*_tmp217=_tmp216;struct Cyc_Absyn_Exp*_tmp218;struct _dyneither_ptr*_tmp219;struct Cyc_Absyn_Exp*_tmp21A;struct _dyneither_ptr*_tmp21B;switch(*((int*)_tmp217)){case 20: _LL106: _tmp21A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp217)->f1;_tmp21B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp217)->f2;_LL107:
# 1092
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp21A->topt),_tmp21B) && !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));case 21: _LL108: _tmp218=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp217)->f1;_tmp219=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp217)->f2;_LL109: {
# 1095
void*_tmp21C=Cyc_Tcutil_compress((void*)_check_null(_tmp218->topt));void*_tmp21D=_tmp21C;void*_tmp21E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21D)->tag == 5){_LL10D: _tmp21E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21D)->f1).elt_typ;_LL10E:
# 1097
 return Cyc_Toc_is_poly_field(_tmp21E,_tmp219) && !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}else{_LL10F: _LL110: {
const char*_tmpA70;void*_tmpA6F[1];struct Cyc_String_pa_PrintArg_struct _tmpA6E;(_tmpA6E.tag=0,((_tmpA6E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp218->topt))),((_tmpA6F[0]=& _tmpA6E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA70="is_poly_project: bad type %s",_tag_dyneither(_tmpA70,sizeof(char),29))),_tag_dyneither(_tmpA6F,sizeof(void*),1)))))));}}_LL10C:;}default: _LL10A: _LL10B:
# 1100
 return 0;}_LL105:;}
# 1105
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpA71;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((_tmpA71=_cycalloc(sizeof(*_tmpA71)),((_tmpA71->hd=s,((_tmpA71->tl=0,_tmpA71)))))),0);}
# 1109
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpA72;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((_tmpA72=_cycalloc(sizeof(*_tmpA72)),((_tmpA72->hd=s,((_tmpA72->tl=0,_tmpA72)))))),0);}
# 1113
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1119
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*_tmpA73[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmpA73[1]=s,((_tmpA73[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA73,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
# 1123
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*_tmpA74[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__fast_region_malloc_e,((_tmpA74[1]=s,((_tmpA74[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA74,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
# 1127
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type)){
struct Cyc_Absyn_Exp*_tmpA75[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpA75[1]=n,((_tmpA75[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA75,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{
# 1131
struct Cyc_Absyn_Exp*_tmpA76[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpA76[1]=n,((_tmpA76[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA76,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}
# 1134
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*_tmpA77[3];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,((_tmpA77[2]=n,((_tmpA77[1]=s,((_tmpA77[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA77,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
# 1139
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
struct Cyc_List_List*_tmpA78;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((_tmpA78=_cycalloc(sizeof(*_tmpA78)),((_tmpA78->hd=e,((_tmpA78->tl=0,_tmpA78)))))),0);}
# 1142
static struct Cyc_Absyn_Exp*Cyc_Toc_newrethrow_exp(struct Cyc_Absyn_Exp*e){
struct Cyc_List_List*_tmpA79;return Cyc_Absyn_fncall_exp(Cyc_Toc__rethrow_e,((_tmpA79=_cycalloc(sizeof(*_tmpA79)),((_tmpA79->hd=e,((_tmpA79->tl=0,_tmpA79)))))),0);}
# 1146
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
struct Cyc_Absyn_Exp*_tmpA7A[0];return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA7A,sizeof(struct Cyc_Absyn_Exp*),0)),0),0);}
# 1152
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1160
int is_string=0;
{void*_tmp22C=e->r;void*_tmp22D=_tmp22C;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp22D)->tag == 0)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp22D)->f1).Wstring_c).tag){case 8: _LL112: _LL113:
 is_string=1;goto _LL111;case 9: _LL114: _LL115:
 is_string=1;goto _LL111;default: goto _LL116;}else{_LL116: _LL117:
 goto _LL111;}_LL111:;}{
# 1166
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
if(is_string){
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_conref,0);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpA80;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpA7F;struct Cyc_List_List*_tmpA7E;Cyc_Toc_result_decls=((_tmpA7E=_cycalloc(sizeof(*_tmpA7E)),((_tmpA7E->hd=Cyc_Absyn_new_decl((void*)((_tmpA80=_cycalloc(sizeof(*_tmpA80)),((_tmpA80[0]=((_tmpA7F.tag=0,((_tmpA7F.f1=vd,_tmpA7F)))),_tmpA80)))),0),((_tmpA7E->tl=Cyc_Toc_result_decls,_tmpA7E))))));}
xexp=Cyc_Absyn_var_exp(x,0);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{
# 1176
xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),e);
# 1178
xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
# 1180
struct Cyc_Absyn_Exp*urm_exp;
{struct _tuple19*_tmpA87;struct _tuple19*_tmpA86;struct _tuple19*_tmpA85;struct _tuple19*_tmpA84[3];urm_exp=Cyc_Absyn_unresolvedmem_exp(0,((_tmpA84[2]=(
# 1183
(_tmpA85=_cycalloc(sizeof(*_tmpA85)),((_tmpA85->f1=0,((_tmpA85->f2=xplussz,_tmpA85)))))),((_tmpA84[1]=(
# 1182
(_tmpA86=_cycalloc(sizeof(*_tmpA86)),((_tmpA86->f1=0,((_tmpA86->f2=xexp,_tmpA86)))))),((_tmpA84[0]=(
# 1181
(_tmpA87=_cycalloc(sizeof(*_tmpA87)),((_tmpA87->f1=0,((_tmpA87->f2=xexp,_tmpA87)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA84,sizeof(struct _tuple19*),3)))))))),0);}
# 1184
return urm_exp;};};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_StructInfo{struct Cyc_Absyn_Exp**varsizeexp;struct Cyc_Absyn_Exp*lhs_exp;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;int*in_lhs;struct Cyc_Toc_StructInfo struct_info;int*in_sizeof;struct _RegionHandle*rgn;};
# 1227 "toc.cyc"
typedef struct Cyc_Toc_Env*Cyc_Toc_env_t;
# 1229
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
int _tmp236;struct Cyc_Toc_Env*_tmp235=nv;_tmp236=_tmp235->toplevel;
return _tmp236;}
# 1233
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
int*_tmp238;struct Cyc_Toc_Env*_tmp237=nv;_tmp238=_tmp237->in_lhs;
return*_tmp238;}
# 1238
static struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple1*x){
struct Cyc_Dict_Dict _tmp23A;struct Cyc_Toc_Env*_tmp239=nv;_tmp23A=_tmp239->varmap;
return((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp23A,x);}
# 1244
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
int*_tmpA8F;struct Cyc_Absyn_Exp**_tmpA8E;int*_tmpA8D;struct Cyc_Toc_Env*_tmpA8C;return(_tmpA8C=_region_malloc(r,sizeof(*_tmpA8C)),((_tmpA8C->break_lab=(struct _dyneither_ptr**)0,((_tmpA8C->continue_lab=(struct _dyneither_ptr**)0,((_tmpA8C->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpA8C->varmap=(struct Cyc_Dict_Dict)
# 1248
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((_tmpA8C->toplevel=(int)1,((_tmpA8C->in_lhs=(int*)(
# 1250
(_tmpA8D=_region_malloc(r,sizeof(*_tmpA8D)),((_tmpA8D[0]=0,_tmpA8D)))),((_tmpA8C->struct_info=(struct Cyc_Toc_StructInfo)(
((_tmpA8C->struct_info).varsizeexp=((_tmpA8E=_cycalloc(sizeof(*_tmpA8E)),((_tmpA8E[0]=0,_tmpA8E)))),(((_tmpA8C->struct_info).lhs_exp=0,_tmpA8C->struct_info)))),((_tmpA8C->in_sizeof=(int*)(
(_tmpA8F=_region_malloc(r,sizeof(*_tmpA8F)),((_tmpA8F[0]=0,_tmpA8F)))),((_tmpA8C->rgn=(struct _RegionHandle*)r,_tmpA8C)))))))))))))))))));}
# 1256
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp240;struct _dyneither_ptr**_tmp241;struct Cyc_Toc_FallthruInfo*_tmp242;struct Cyc_Dict_Dict _tmp243;int _tmp244;int*_tmp245;struct Cyc_Toc_StructInfo _tmp246;int*_tmp247;struct Cyc_Toc_Env*_tmp23F=e;_tmp240=_tmp23F->break_lab;_tmp241=_tmp23F->continue_lab;_tmp242=_tmp23F->fallthru_info;_tmp243=_tmp23F->varmap;_tmp244=_tmp23F->toplevel;_tmp245=_tmp23F->in_lhs;_tmp246=_tmp23F->struct_info;_tmp247=_tmp23F->in_sizeof;{
struct Cyc_Toc_Env*_tmpA90;return(_tmpA90=_region_malloc(r,sizeof(*_tmpA90)),((_tmpA90->break_lab=(struct _dyneither_ptr**)_tmp240,((_tmpA90->continue_lab=(struct _dyneither_ptr**)_tmp241,((_tmpA90->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp242,((_tmpA90->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp243),((_tmpA90->toplevel=(int)_tmp244,((_tmpA90->in_lhs=(int*)_tmp245,((_tmpA90->struct_info=(struct Cyc_Toc_StructInfo)_tmp246,((_tmpA90->in_sizeof=(int*)_tmp247,((_tmpA90->rgn=(struct _RegionHandle*)r,_tmpA90)))))))))))))))))));};}
# 1261
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp24A;struct _dyneither_ptr**_tmp24B;struct Cyc_Toc_FallthruInfo*_tmp24C;struct Cyc_Dict_Dict _tmp24D;int _tmp24E;int*_tmp24F;struct Cyc_Toc_StructInfo _tmp250;int*_tmp251;struct Cyc_Toc_Env*_tmp249=e;_tmp24A=_tmp249->break_lab;_tmp24B=_tmp249->continue_lab;_tmp24C=_tmp249->fallthru_info;_tmp24D=_tmp249->varmap;_tmp24E=_tmp249->toplevel;_tmp24F=_tmp249->in_lhs;_tmp250=_tmp249->struct_info;_tmp251=_tmp249->in_sizeof;{
struct Cyc_Toc_Env*_tmpA91;return(_tmpA91=_region_malloc(r,sizeof(*_tmpA91)),((_tmpA91->break_lab=(struct _dyneither_ptr**)_tmp24A,((_tmpA91->continue_lab=(struct _dyneither_ptr**)_tmp24B,((_tmpA91->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp24C,((_tmpA91->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp24D),((_tmpA91->toplevel=(int)0,((_tmpA91->in_lhs=(int*)_tmp24F,((_tmpA91->struct_info=(struct Cyc_Toc_StructInfo)_tmp250,((_tmpA91->in_sizeof=(int*)_tmp251,((_tmpA91->rgn=(struct _RegionHandle*)r,_tmpA91)))))))))))))))))));};}
# 1265
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp254;struct _dyneither_ptr**_tmp255;struct Cyc_Toc_FallthruInfo*_tmp256;struct Cyc_Dict_Dict _tmp257;int _tmp258;int*_tmp259;struct Cyc_Toc_StructInfo _tmp25A;int*_tmp25B;struct Cyc_Toc_Env*_tmp253=e;_tmp254=_tmp253->break_lab;_tmp255=_tmp253->continue_lab;_tmp256=_tmp253->fallthru_info;_tmp257=_tmp253->varmap;_tmp258=_tmp253->toplevel;_tmp259=_tmp253->in_lhs;_tmp25A=_tmp253->struct_info;_tmp25B=_tmp253->in_sizeof;{
struct Cyc_Toc_Env*_tmpA92;return(_tmpA92=_region_malloc(r,sizeof(*_tmpA92)),((_tmpA92->break_lab=(struct _dyneither_ptr**)_tmp254,((_tmpA92->continue_lab=(struct _dyneither_ptr**)_tmp255,((_tmpA92->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp256,((_tmpA92->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp257),((_tmpA92->toplevel=(int)1,((_tmpA92->in_lhs=(int*)_tmp259,((_tmpA92->struct_info=(struct Cyc_Toc_StructInfo)_tmp25A,((_tmpA92->in_sizeof=(int*)_tmp25B,((_tmpA92->rgn=(struct _RegionHandle*)r,_tmpA92)))))))))))))))))));};}
# 1269
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
int*_tmp25E;struct Cyc_Toc_Env*_tmp25D=e;_tmp25E=_tmp25D->in_lhs;
*_tmp25E=b;}
# 1273
static int Cyc_Toc_set_in_sizeof(struct Cyc_Toc_Env*e,int b){
int*_tmp260;struct Cyc_Toc_Env*_tmp25F=e;_tmp260=_tmp25F->in_sizeof;{
int _tmp261=*_tmp260;
*_tmp260=b;
return _tmp261;};}
# 1279
static int Cyc_Toc_in_sizeof(struct Cyc_Toc_Env*e){
int*_tmp263;struct Cyc_Toc_Env*_tmp262=e;_tmp263=_tmp262->in_sizeof;
return*_tmp263;}
# 1284
static struct Cyc_Toc_Env*Cyc_Toc_set_lhs_exp(struct _RegionHandle*r,struct Cyc_Absyn_Exp*exp,struct Cyc_Toc_Env*e){
# 1286
struct _dyneither_ptr**_tmp265;struct _dyneither_ptr**_tmp266;struct Cyc_Toc_FallthruInfo*_tmp267;struct Cyc_Dict_Dict _tmp268;int _tmp269;int*_tmp26A;struct Cyc_Absyn_Exp**_tmp26B;int*_tmp26C;struct Cyc_Toc_Env*_tmp264=e;_tmp265=_tmp264->break_lab;_tmp266=_tmp264->continue_lab;_tmp267=_tmp264->fallthru_info;_tmp268=_tmp264->varmap;_tmp269=_tmp264->toplevel;_tmp26A=_tmp264->in_lhs;_tmp26B=(_tmp264->struct_info).varsizeexp;_tmp26C=_tmp264->in_sizeof;{
struct Cyc_Toc_Env*_tmpA93;return(_tmpA93=_region_malloc(r,sizeof(*_tmpA93)),((_tmpA93->break_lab=(struct _dyneither_ptr**)_tmp265,((_tmpA93->continue_lab=(struct _dyneither_ptr**)_tmp266,((_tmpA93->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp267,((_tmpA93->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp268),((_tmpA93->toplevel=(int)_tmp269,((_tmpA93->in_lhs=(int*)_tmp26A,((_tmpA93->struct_info=(struct Cyc_Toc_StructInfo)(((_tmpA93->struct_info).varsizeexp=_tmp26B,(((_tmpA93->struct_info).lhs_exp=exp,_tmpA93->struct_info)))),((_tmpA93->in_sizeof=(int*)_tmp26C,((_tmpA93->rgn=(struct _RegionHandle*)r,_tmpA93)))))))))))))))))));};}
# 1292
static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple1*x,struct Cyc_Absyn_Exp*y){
{union Cyc_Absyn_Nmspace _tmp26E=(*x).f1;union Cyc_Absyn_Nmspace _tmp26F=_tmp26E;if((_tmp26F.Rel_n).tag == 1){_LL119: _LL11A: {
# 1295
const char*_tmpA97;void*_tmpA96[1];struct Cyc_String_pa_PrintArg_struct _tmpA95;(_tmpA95.tag=0,((_tmpA95.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(x)),((_tmpA96[0]=& _tmpA95,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA97="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(_tmpA97,sizeof(char),30))),_tag_dyneither(_tmpA96,sizeof(void*),1)))))));}}else{_LL11B: _LL11C:
 goto _LL118;}_LL118:;}{
# 1298
struct _dyneither_ptr**_tmp274;struct _dyneither_ptr**_tmp275;struct Cyc_Toc_FallthruInfo*_tmp276;struct Cyc_Dict_Dict _tmp277;int _tmp278;int*_tmp279;struct Cyc_Toc_StructInfo _tmp27A;int*_tmp27B;struct Cyc_Toc_Env*_tmp273=e;_tmp274=_tmp273->break_lab;_tmp275=_tmp273->continue_lab;_tmp276=_tmp273->fallthru_info;_tmp277=_tmp273->varmap;_tmp278=_tmp273->toplevel;_tmp279=_tmp273->in_lhs;_tmp27A=_tmp273->struct_info;_tmp27B=_tmp273->in_sizeof;{
struct Cyc_Dict_Dict _tmp27C=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp277),x,y);
struct Cyc_Toc_Env*_tmpA98;return(_tmpA98=_region_malloc(r,sizeof(*_tmpA98)),((_tmpA98->break_lab=(struct _dyneither_ptr**)_tmp274,((_tmpA98->continue_lab=(struct _dyneither_ptr**)_tmp275,((_tmpA98->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp276,((_tmpA98->varmap=(struct Cyc_Dict_Dict)_tmp27C,((_tmpA98->toplevel=(int)_tmp278,((_tmpA98->in_lhs=(int*)_tmp279,((_tmpA98->struct_info=(struct Cyc_Toc_StructInfo)_tmp27A,((_tmpA98->in_sizeof=(int*)_tmp27B,((_tmpA98->rgn=(struct _RegionHandle*)r,_tmpA98)))))))))))))))))));};};}
# 1305
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp27F;struct _dyneither_ptr**_tmp280;struct Cyc_Toc_FallthruInfo*_tmp281;struct Cyc_Dict_Dict _tmp282;int _tmp283;int*_tmp284;struct Cyc_Toc_StructInfo _tmp285;int*_tmp286;struct Cyc_Toc_Env*_tmp27E=e;_tmp27F=_tmp27E->break_lab;_tmp280=_tmp27E->continue_lab;_tmp281=_tmp27E->fallthru_info;_tmp282=_tmp27E->varmap;_tmp283=_tmp27E->toplevel;_tmp284=_tmp27E->in_lhs;_tmp285=_tmp27E->struct_info;_tmp286=_tmp27E->in_sizeof;{
struct Cyc_Toc_Env*_tmpA99;return(_tmpA99=_region_malloc(r,sizeof(*_tmpA99)),((_tmpA99->break_lab=(struct _dyneither_ptr**)0,((_tmpA99->continue_lab=(struct _dyneither_ptr**)0,((_tmpA99->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp281,((_tmpA99->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp282),((_tmpA99->toplevel=(int)_tmp283,((_tmpA99->in_lhs=(int*)_tmp284,((_tmpA99->struct_info=(struct Cyc_Toc_StructInfo)_tmp285,((_tmpA99->in_sizeof=(int*)_tmp286,((_tmpA99->rgn=(struct _RegionHandle*)r,_tmpA99)))))))))))))))))));};}
# 1311
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*next_case_env){
# 1316
struct Cyc_List_List*fallthru_vars=0;
for(0;fallthru_binders != 0;fallthru_binders=fallthru_binders->tl){
struct Cyc_List_List*_tmpA9A;fallthru_vars=((_tmpA9A=_region_malloc(r,sizeof(*_tmpA9A)),((_tmpA9A->hd=((struct Cyc_Absyn_Vardecl*)fallthru_binders->hd)->name,((_tmpA9A->tl=fallthru_vars,_tmpA9A))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fallthru_vars);{
struct _dyneither_ptr**_tmp28A;struct _dyneither_ptr**_tmp28B;struct Cyc_Toc_FallthruInfo*_tmp28C;struct Cyc_Dict_Dict _tmp28D;int _tmp28E;int*_tmp28F;struct Cyc_Toc_StructInfo _tmp290;int*_tmp291;struct Cyc_Toc_Env*_tmp289=e;_tmp28A=_tmp289->break_lab;_tmp28B=_tmp289->continue_lab;_tmp28C=_tmp289->fallthru_info;_tmp28D=_tmp289->varmap;_tmp28E=_tmp289->toplevel;_tmp28F=_tmp289->in_lhs;_tmp290=_tmp289->struct_info;_tmp291=_tmp289->in_sizeof;{
struct Cyc_Dict_Dict _tmp293;struct Cyc_Toc_Env*_tmp292=next_case_env;_tmp293=_tmp292->varmap;{
struct Cyc_Toc_FallthruInfo*_tmpA9B;struct Cyc_Toc_FallthruInfo*fi=
(_tmpA9B=_region_malloc(r,sizeof(*_tmpA9B)),((_tmpA9B->label=fallthru_l,((_tmpA9B->binders=fallthru_vars,((_tmpA9B->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp293),_tmpA9B)))))));
struct _dyneither_ptr**_tmpA9E;struct Cyc_Toc_Env*_tmpA9D;return(_tmpA9D=_region_malloc(r,sizeof(*_tmpA9D)),((_tmpA9D->break_lab=(struct _dyneither_ptr**)((_tmpA9E=_region_malloc(r,sizeof(*_tmpA9E)),((_tmpA9E[0]=break_l,_tmpA9E)))),((_tmpA9D->continue_lab=(struct _dyneither_ptr**)_tmp28B,((_tmpA9D->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi,((_tmpA9D->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp28D),((_tmpA9D->toplevel=(int)_tmp28E,((_tmpA9D->in_lhs=(int*)_tmp28F,((_tmpA9D->struct_info=(struct Cyc_Toc_StructInfo)_tmp290,((_tmpA9D->in_sizeof=(int*)_tmp291,((_tmpA9D->rgn=(struct _RegionHandle*)r,_tmpA9D)))))))))))))))))));};};};}
# 1328
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1331
struct _dyneither_ptr**_tmp298;struct _dyneither_ptr**_tmp299;struct Cyc_Toc_FallthruInfo*_tmp29A;struct Cyc_Dict_Dict _tmp29B;int _tmp29C;int*_tmp29D;struct Cyc_Toc_StructInfo _tmp29E;int*_tmp29F;struct Cyc_Toc_Env*_tmp297=e;_tmp298=_tmp297->break_lab;_tmp299=_tmp297->continue_lab;_tmp29A=_tmp297->fallthru_info;_tmp29B=_tmp297->varmap;_tmp29C=_tmp297->toplevel;_tmp29D=_tmp297->in_lhs;_tmp29E=_tmp297->struct_info;_tmp29F=_tmp297->in_sizeof;{
struct _dyneither_ptr**_tmpAA1;struct Cyc_Toc_Env*_tmpAA0;return(_tmpAA0=_region_malloc(r,sizeof(*_tmpAA0)),((_tmpAA0->break_lab=(struct _dyneither_ptr**)((_tmpAA1=_region_malloc(r,sizeof(*_tmpAA1)),((_tmpAA1[0]=break_l,_tmpAA1)))),((_tmpAA0->continue_lab=(struct _dyneither_ptr**)_tmp299,((_tmpAA0->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpAA0->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp29B),((_tmpAA0->toplevel=(int)_tmp29C,((_tmpAA0->in_lhs=(int*)_tmp29D,((_tmpAA0->struct_info=(struct Cyc_Toc_StructInfo)_tmp29E,((_tmpAA0->in_sizeof=(int*)_tmp29F,((_tmpAA0->rgn=(struct _RegionHandle*)r,_tmpAA0)))))))))))))))))));};}
# 1338
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1341
struct _dyneither_ptr**_tmp2A3;struct _dyneither_ptr**_tmp2A4;struct Cyc_Toc_FallthruInfo*_tmp2A5;struct Cyc_Dict_Dict _tmp2A6;int _tmp2A7;int*_tmp2A8;struct Cyc_Toc_StructInfo _tmp2A9;int*_tmp2AA;struct Cyc_Toc_Env*_tmp2A2=e;_tmp2A3=_tmp2A2->break_lab;_tmp2A4=_tmp2A2->continue_lab;_tmp2A5=_tmp2A2->fallthru_info;_tmp2A6=_tmp2A2->varmap;_tmp2A7=_tmp2A2->toplevel;_tmp2A8=_tmp2A2->in_lhs;_tmp2A9=_tmp2A2->struct_info;_tmp2AA=_tmp2A2->in_sizeof;{
struct Cyc_Toc_FallthruInfo*_tmpAA4;struct Cyc_Toc_Env*_tmpAA3;return(_tmpAA3=_region_malloc(r,sizeof(*_tmpAA3)),((_tmpAA3->break_lab=(struct _dyneither_ptr**)0,((_tmpAA3->continue_lab=(struct _dyneither_ptr**)_tmp2A4,((_tmpAA3->fallthru_info=(struct Cyc_Toc_FallthruInfo*)((_tmpAA4=_region_malloc(r,sizeof(*_tmpAA4)),((_tmpAA4->label=next_l,((_tmpAA4->binders=0,((_tmpAA4->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),_tmpAA4)))))))),((_tmpAA3->varmap=(struct Cyc_Dict_Dict)
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2A6),((_tmpAA3->toplevel=(int)_tmp2A7,((_tmpAA3->in_lhs=(int*)_tmp2A8,((_tmpAA3->struct_info=(struct Cyc_Toc_StructInfo)_tmp2A9,((_tmpAA3->in_sizeof=(int*)_tmp2AA,((_tmpAA3->rgn=(struct _RegionHandle*)r,_tmpAA3)))))))))))))))))));};}
# 1356 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1359
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
# 1361
void*_tmp2AD=e->annot;void*_tmp2AE=_tmp2AD;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2AE)->tag == Cyc_CfFlowInfo_UnknownZ){_LL11E: _LL11F:
# 1364
 return Cyc_Toc_is_nullable((void*)_check_null(e->topt));}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2AE)->tag == Cyc_CfFlowInfo_NotZero){_LL120: _LL121:
# 1367
 return 0;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2AE)->tag == Cyc_CfFlowInfo_IsZero){_LL122: _LL123:
# 1369
{const char*_tmpAA7;void*_tmpAA6;(_tmpAA6=0,Cyc_Tcutil_terr(e->loc,((_tmpAA7="dereference of NULL pointer",_tag_dyneither(_tmpAA7,sizeof(char),28))),_tag_dyneither(_tmpAA6,sizeof(void*),0)));}
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2AE)->tag == Cyc_Absyn_EmptyAnnot){_LL124: _LL125:
# 1375
 return 1;}else{_LL126: _LL127: {
const char*_tmpAAA;void*_tmpAA9;(_tmpAA9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAAA="need_null_check",_tag_dyneither(_tmpAAA,sizeof(char),16))),_tag_dyneither(_tmpAA9,sizeof(void*),0)));}}}}}_LL11D:;}
# 1380
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp2B3=e->annot;void*_tmp2B4=_tmp2B3;struct Cyc_List_List*_tmp2B5;struct Cyc_List_List*_tmp2B6;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2B4)->tag == Cyc_CfFlowInfo_UnknownZ){_LL129: _tmp2B6=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2B4)->f1;_LL12A:
 return _tmp2B6;}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2B4)->tag == Cyc_CfFlowInfo_NotZero){_LL12B: _tmp2B5=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2B4)->f1;_LL12C:
 return _tmp2B5;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2B4)->tag == Cyc_CfFlowInfo_IsZero){_LL12D: _LL12E:
# 1385
{const char*_tmpAAD;void*_tmpAAC;(_tmpAAC=0,Cyc_Tcutil_terr(e->loc,((_tmpAAD="dereference of NULL pointer",_tag_dyneither(_tmpAAD,sizeof(char),28))),_tag_dyneither(_tmpAAC,sizeof(void*),0)));}
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2B4)->tag == Cyc_Absyn_EmptyAnnot){_LL12F: _LL130:
 return 0;}else{_LL131: _LL132: {
const char*_tmpAB0;void*_tmpAAF;(_tmpAAF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAB0="get_relns",_tag_dyneither(_tmpAB0,sizeof(char),10))),_tag_dyneither(_tmpAAF,sizeof(void*),0)));}}}}}_LL128:;}static char _tmp2BF[8]="*bogus*";
# 1395
static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
# 1405 "toc.cyc"
union Cyc_Relations_RelnOp rop_i=Cyc_Relations_RConst(0);
int valid_rop_i=Cyc_Relations_exp2relnop(i,& rop_i);
# 1408
struct Cyc_Absyn_Vardecl*x;
{void*_tmp2BB=a->r;void*_tmp2BC=_tmp2BB;void*_tmp2BD;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2BC)->tag == 1){_LL134: _tmp2BD=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2BC)->f2;_LL135: {
# 1411
struct Cyc_Absyn_Vardecl*_tmp2BE=Cyc_Tcutil_nonesc_vardecl(_tmp2BD);
if(_tmp2BE == 0)goto _LL137;
x=_tmp2BE;
goto _LL133;}}else{_LL136: _LL137: {
# 1417
static struct _dyneither_ptr bogus_string={_tmp2BF,_tmp2BF,_tmp2BF + 8};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
# 1420
static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};
# 1426
x=& bogus_vardecl;
x->type=a_typ;}}_LL133:;}{
# 1429
void*_tmp2C0=a_typ;
union Cyc_Relations_RelnOp rop_a=Cyc_Relations_RNumelts(x);
# 1434
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound(_tmp2C0);
if(bound != 0){
union Cyc_Relations_RelnOp rop_bound=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(bound,& rop_bound))
relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_bound,Cyc_Relations_Rlte,rop_a,relns);}
# 1443
if(valid_rop_i){
struct Cyc_List_List*_tmp2C1=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlte,rop_i,relns);
# 1451
if(!Cyc_Relations_consistent_relations(_tmp2C1))return 1;}
# 1454
inner_loop: {
void*_tmp2C2=i->r;void*_tmp2C3=_tmp2C2;struct Cyc_Absyn_Exp*_tmp2C4;struct Cyc_Absyn_Exp*_tmp2C5;void*_tmp2C6;struct Cyc_Absyn_Exp*_tmp2C7;switch(*((int*)_tmp2C3)){case 13: _LL139: _tmp2C6=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2C3)->f1;_tmp2C7=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2C3)->f2;_LL13A:
 i=_tmp2C7;goto inner_loop;case 2: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2C3)->f1 == Cyc_Absyn_Mod){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2C3)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2C3)->f2)->tl != 0){_LL13B: _tmp2C4=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2C3)->f2)->hd;_tmp2C5=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2C3)->f2)->tl)->hd;_LL13C: {
# 1460
union Cyc_Relations_RelnOp rop_z=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmp2C5,& rop_z)){
# 1463
struct Cyc_List_List*_tmp2C8=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlt,rop_z,relns);
# 1465
return !Cyc_Relations_consistent_relations(_tmp2C8);}
# 1467
goto _LL138;}}else{goto _LL13D;}}else{goto _LL13D;}}else{goto _LL13D;}default: _LL13D: _LL13E:
 goto _LL138;}_LL138:;}
# 1470
return 0;};}
# 1473
static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0){const char*_tmpAB3;void*_tmpAB2;(_tmpAB2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAB3="Missing type in primop ",_tag_dyneither(_tmpAB3,sizeof(char),24))),_tag_dyneither(_tmpAB2,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));}
# 1477
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0){const char*_tmpAB6;void*_tmpAB5;(_tmpAB5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAB6="Missing type in primop ",_tag_dyneither(_tmpAB6,sizeof(char),24))),_tag_dyneither(_tmpAB5,sizeof(void*),0)));}
return(void*)_check_null(e->topt);}
# 1481
static struct _tuple11*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
struct _tuple11*_tmpAB7;return(_tmpAB7=_cycalloc(sizeof(*_tmpAB7)),((_tmpAB7->f1=Cyc_Toc_mt_tq,((_tmpAB7->f2=Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmpAB7)))));}
# 1484
static struct _tuple19*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple19*_tmpAB8;return(_tmpAB8=_cycalloc(sizeof(*_tmpAB8)),((_tmpAB8->f1=0,((_tmpAB8->f2=e,_tmpAB8)))));};}
# 1491
static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic,int do_declare){
# 1494
struct Cyc_Absyn_Exp*eo;
void*t;
if(pointer){
struct Cyc_Absyn_Exp*_tmp2CF=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(x,0),0);
if(*(nv->struct_info).varsizeexp != 0){
_tmp2CF=Cyc_Absyn_add_exp(_tmp2CF,(struct Cyc_Absyn_Exp*)_check_null(*(nv->struct_info).varsizeexp),0);
*(nv->struct_info).varsizeexp=0;}
# 1502
t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);
if(rgnopt == 0  || Cyc_Absyn_no_regions)
eo=is_atomic?Cyc_Toc_malloc_atomic(_tmp2CF): Cyc_Toc_malloc_ptr(_tmp2CF);else{
# 1506
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);
eo=Cyc_Toc_rmalloc_exp(r,_tmp2CF);}}else{
# 1511
t=struct_typ;
eo=0;}
# 1514
if(do_declare){
void*_tmp2D0=x->r;void*_tmp2D1=_tmp2D0;struct _tuple1*_tmp2D2;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D1)->tag == 1){_LL140: _tmp2D2=((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D1)->f1;_LL141:
# 1517
 return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp2D2,t,eo,s,0),0);}else{_LL142: _LL143: {
# 1519
const char*_tmpABB;void*_tmpABA;(_tmpABA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpABB="bogus x to make_struct",_tag_dyneither(_tmpABB,sizeof(char),23))),_tag_dyneither(_tmpABA,sizeof(void*),0)));}}_LL13F:;}else{
# 1523
if(eo != 0)
return Cyc_Absyn_stmt_exp(Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(x,eo,0),0),s,0),0);else{
# 1526
return Cyc_Absyn_stmt_exp(s,0);}}}
# 1530
static int Cyc_Toc_abstract_aggr(void*typ){
void*_tmp2D5=Cyc_Tcutil_compress(typ);void*_tmp2D6=_tmp2D5;struct Cyc_List_List*_tmp2D7;union Cyc_Absyn_AggrInfoU _tmp2D8;struct Cyc_List_List*_tmp2D9;switch(*((int*)_tmp2D6)){case 11: _LL145: _tmp2D8=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2D6)->f1).aggr_info;_tmp2D9=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2D6)->f1).targs;_LL146: {
# 1533
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(_tmp2D8);
if(ad->impl == 0)return 0;{
struct Cyc_List_List*aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
_tmp2D7=aggrfields;goto _LL148;};}case 12: _LL147: _tmp2D7=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D6)->f2;_LL148:
# 1538
 if(_tmp2D7 == 0)return 0;
return 1;default: _LL149: _LL14A:
# 1544
 return 0;}_LL144:;}
# 1548
static void*Cyc_Toc_array_elt_type(void*t){
void*_tmp2DA=Cyc_Tcutil_compress(t);void*_tmp2DB=_tmp2DA;void*_tmp2DC;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2DB)->tag == 8){_LL14C: _tmp2DC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2DB)->f1).elt_type;_LL14D:
 return _tmp2DC;}else{_LL14E: _LL14F: {
const char*_tmpABF;void*_tmpABE[1];struct Cyc_String_pa_PrintArg_struct _tmpABD;(_tmpABD.tag=0,((_tmpABD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpABE[0]=& _tmpABD,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpABF="array_elt_type called on %s",_tag_dyneither(_tmpABF,sizeof(char),28))),_tag_dyneither(_tmpABE,sizeof(void*),1)))))));}}_LL14B:;}
# 1555
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated);
# 1561
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s);
# 1566
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,void*array_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){
# 1571
void*orig_elt_type=Cyc_Toc_array_elt_type(array_type);
int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;
{struct Cyc_List_List*_tmp2E0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;_tmp2E0 != 0;_tmp2E0=_tmp2E0->tl){
# 1575
struct _tuple19*_tmp2E1=(struct _tuple19*)_tmp2E0->hd;struct Cyc_List_List*_tmp2E3;struct Cyc_Absyn_Exp*_tmp2E4;struct _tuple19*_tmp2E2=_tmp2E1;_tmp2E3=_tmp2E2->f1;_tmp2E4=_tmp2E2->f2;{
# 1579
struct Cyc_Absyn_Exp*e_index;
if(_tmp2E3 == 0)
e_index=Cyc_Absyn_signed_int_exp(count --,0);else{
# 1583
if(_tmp2E3->tl != 0){const char*_tmpAC2;void*_tmpAC1;(_tmpAC1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpAC2="multiple designators in array",_tag_dyneither(_tmpAC2,sizeof(char),30))),_tag_dyneither(_tmpAC1,sizeof(void*),0)));}{
void*_tmp2E7=(void*)_tmp2E3->hd;
void*_tmp2E8=_tmp2E7;struct Cyc_Absyn_Exp*_tmp2E9;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2E8)->tag == 0){_LL151: _tmp2E9=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2E8)->f1;_LL152:
# 1587
 Cyc_Toc_exp_to_c(nv,_tmp2E9);
e_index=_tmp2E9;
goto _LL150;}else{_LL153: _LL154: {
const char*_tmpAC5;void*_tmpAC4;(_tmpAC4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpAC5="field name designators in array",_tag_dyneither(_tmpAC5,sizeof(char),32))),_tag_dyneither(_tmpAC4,sizeof(void*),0)));}}_LL150:;};}{
# 1593
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);
void*_tmp2EC=_tmp2E4->r;void*_tmp2ED=_tmp2EC;void*_tmp2EE;struct Cyc_List_List*_tmp2EF;struct Cyc_Absyn_Vardecl*_tmp2F0;struct Cyc_Absyn_Exp*_tmp2F1;struct Cyc_Absyn_Exp*_tmp2F2;int _tmp2F3;struct Cyc_List_List*_tmp2F4;switch(*((int*)_tmp2ED)){case 25: _LL156: _tmp2F4=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp2ED)->f1;_LL157:
# 1596
 s=Cyc_Toc_init_array(nv,orig_elt_type,lval,_tmp2F4,s);
goto _LL155;case 26: _LL158: _tmp2F0=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2ED)->f1;_tmp2F1=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2ED)->f2;_tmp2F2=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2ED)->f3;_tmp2F3=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2ED)->f4;_LL159:
# 1599
 s=Cyc_Toc_init_comprehension(nv,orig_elt_type,lval,_tmp2F0,_tmp2F1,_tmp2F2,_tmp2F3,s,0);
# 1601
goto _LL155;case 29: _LL15A: _tmp2EE=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2ED)->f1;_tmp2EF=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2ED)->f2;_LL15B:
# 1603
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp2EE,_tmp2EF,s);
goto _LL155;case 27: _LL15C: _LL15D:
# 1606
 goto _LL155;default: _LL15E: _LL15F:
# 1608
 Cyc_Toc_exp_to_c(nv,_tmp2E4);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(orig_elt_type),_tmp2E4),0),s,0);
goto _LL155;}_LL155:;};};}}
# 1614
return s;}
# 1619
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated){
# 1624
struct _tuple1*_tmp2F5=vd->name;
void*expected_elt_type;
{void*_tmp2F6=Cyc_Tcutil_compress(comprehension_type);void*_tmp2F7=_tmp2F6;void*_tmp2F8;void*_tmp2F9;switch(*((int*)_tmp2F7)){case 8: _LL161: _tmp2F9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F7)->f1).elt_type;_LL162:
# 1628
 expected_elt_type=_tmp2F9;goto _LL160;case 5: _LL163: _tmp2F8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F7)->f1).elt_typ;_LL164:
# 1630
 expected_elt_type=_tmp2F8;goto _LL160;default: _LL165: _LL166: {
const char*_tmpAC8;void*_tmpAC7;(_tmpAC7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAC8="init_comprehension passed a bad type!",_tag_dyneither(_tmpAC8,sizeof(char),38))),_tag_dyneither(_tmpAC7,sizeof(void*),0)));}}_LL160:;}{
# 1633
void*_tmp2FC=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));
if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);
# 1637
{void*_tmp2FD=e2->r;void*_tmp2FE=_tmp2FD;if(((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2FE)->tag == 27){_LL168: _LL169:
 return Cyc_Absyn_skip_stmt(0);}else{_LL16A: _LL16B:
 goto _LL167;}_LL167:;}{
# 1641
struct _RegionHandle _tmp2FF=_new_region("r2");struct _RegionHandle*r2=& _tmp2FF;_push_region(r2);
{struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpACB;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpACA;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(r2,nv,_tmp2F5,Cyc_Absyn_varb_exp(_tmp2F5,(void*)((_tmpACA=_cycalloc(sizeof(*_tmpACA)),((_tmpACA[0]=((_tmpACB.tag=4,((_tmpACB.f1=vd,_tmpACB)))),_tmpACA)))),0));
struct _tuple1*max=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp2F5,0),Cyc_Absyn_signed_int_exp(0,0),0);
struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp2F5,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp2F5,0),0);
# 1649
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp2F5,0),0);
struct Cyc_Absyn_Stmt*body;
# 1653
{void*_tmp300=e2->r;void*_tmp301=_tmp300;void*_tmp302;struct Cyc_List_List*_tmp303;struct Cyc_Absyn_Vardecl*_tmp304;struct Cyc_Absyn_Exp*_tmp305;struct Cyc_Absyn_Exp*_tmp306;int _tmp307;struct Cyc_List_List*_tmp308;switch(*((int*)_tmp301)){case 25: _LL16D: _tmp308=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp301)->f1;_LL16E:
# 1655
 body=Cyc_Toc_init_array(nv2,expected_elt_type,lval,_tmp308,Cyc_Toc_skip_stmt_dl());
goto _LL16C;case 26: _LL16F: _tmp304=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp301)->f1;_tmp305=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp301)->f2;_tmp306=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp301)->f3;_tmp307=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp301)->f4;_LL170:
# 1658
 body=Cyc_Toc_init_comprehension(nv2,expected_elt_type,lval,_tmp304,_tmp305,_tmp306,_tmp307,Cyc_Toc_skip_stmt_dl(),0);
goto _LL16C;case 29: _LL171: _tmp302=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp301)->f1;_tmp303=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp301)->f2;_LL172:
# 1661
 body=Cyc_Toc_init_anon_struct(nv,lval,_tmp302,_tmp303,Cyc_Toc_skip_stmt_dl());
goto _LL16C;default: _LL173: _LL174:
# 1664
 Cyc_Toc_exp_to_c(nv2,e2);
body=Cyc_Absyn_assign_stmt(lval,Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(expected_elt_type),e2),0);
goto _LL16C;}_LL16C:;}{
# 1668
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0);
# 1670
if(zero_term){
# 1675
struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(_tmp2FC,Cyc_Absyn_uint_exp(0,0)),0);
s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}{
# 1680
struct Cyc_Absyn_Stmt*_tmp309=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,e1,
Cyc_Absyn_declare_stmt(_tmp2F5,Cyc_Absyn_uint_typ,0,s2,0),0),s,0);_npop_handler(0);return _tmp309;};};}
# 1642
;_pop_region(r2);};};}
# 1686
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s){
# 1689
{struct Cyc_List_List*_tmp30C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles);for(0;_tmp30C != 0;_tmp30C=_tmp30C->tl){
struct _tuple19*_tmp30D=(struct _tuple19*)_tmp30C->hd;struct Cyc_List_List*_tmp30F;struct Cyc_Absyn_Exp*_tmp310;struct _tuple19*_tmp30E=_tmp30D;_tmp30F=_tmp30E->f1;_tmp310=_tmp30E->f2;
if(_tmp30F == 0){
const char*_tmpACE;void*_tmpACD;(_tmpACD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpACE="empty designator list",_tag_dyneither(_tmpACE,sizeof(char),22))),_tag_dyneither(_tmpACD,sizeof(void*),0)));}
if(_tmp30F->tl != 0){
const char*_tmpAD1;void*_tmpAD0;(_tmpAD0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAD1="too many designators in anonymous struct",_tag_dyneither(_tmpAD1,sizeof(char),41))),_tag_dyneither(_tmpAD0,sizeof(void*),0)));}{
void*_tmp315=(void*)_tmp30F->hd;void*_tmp316=_tmp315;struct _dyneither_ptr*_tmp317;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp316)->tag == 1){_LL176: _tmp317=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp316)->f1;_LL177: {
# 1697
struct Cyc_Absyn_Exp*lval=Cyc_Toc_member_exp(lhs,_tmp317,0);
{void*_tmp318=_tmp310->r;void*_tmp319=_tmp318;void*_tmp31A;struct Cyc_List_List*_tmp31B;struct Cyc_Absyn_Vardecl*_tmp31C;struct Cyc_Absyn_Exp*_tmp31D;struct Cyc_Absyn_Exp*_tmp31E;int _tmp31F;struct Cyc_List_List*_tmp320;switch(*((int*)_tmp319)){case 25: _LL17B: _tmp320=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp319)->f1;_LL17C:
# 1700
 s=Cyc_Toc_init_array(nv,(void*)_check_null(_tmp310->topt),lval,_tmp320,s);goto _LL17A;case 26: _LL17D: _tmp31C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp319)->f1;_tmp31D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp319)->f2;_tmp31E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp319)->f3;_tmp31F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp319)->f4;_LL17E:
# 1702
 s=Cyc_Toc_init_comprehension(nv,(void*)_check_null(_tmp310->topt),lval,_tmp31C,_tmp31D,_tmp31E,_tmp31F,s,0);
goto _LL17A;case 29: _LL17F: _tmp31A=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp319)->f1;_tmp31B=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp319)->f2;_LL180:
# 1705
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp31A,_tmp31B,s);goto _LL17A;case 27: _LL181: _LL182:
# 1707
 goto _LL17A;default: _LL183: _LL184:
# 1709
 Cyc_Toc_exp_to_c(nv,_tmp310);
# 1711
if(Cyc_Toc_is_poly_field(struct_type,_tmp317))
_tmp310=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp310);
# 1714
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp310,0),0),s,0);
goto _LL17A;}_LL17A:;}
# 1717
goto _LL175;}}else{_LL178: _LL179: {
const char*_tmpAD4;void*_tmpAD3;(_tmpAD3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAD4="array designator in struct",_tag_dyneither(_tmpAD4,sizeof(char),27))),_tag_dyneither(_tmpAD3,sizeof(void*),0)));}}_LL175:;};}}
# 1721
return s;}
# 1726
static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){
# 1730
struct _RegionHandle _tmp323=_new_region("r");struct _RegionHandle*r=& _tmp323;_push_region(r);
{struct Cyc_List_List*_tmp324=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);
void*_tmp325=Cyc_Toc_add_tuple_type(_tmp324);
# 1734
struct _tuple1*_tmp326=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp327=Cyc_Absyn_var_exp(_tmp326,0);
struct Cyc_Absyn_Stmt*_tmp328=Cyc_Absyn_exp_stmt(_tmp327,0);
# 1738
struct Cyc_Absyn_Exp*(*_tmp329)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
# 1740
int is_atomic=1;
struct Cyc_List_List*_tmp32A=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);
{int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp32A);for(0;_tmp32A != 0;(_tmp32A=_tmp32A->tl,-- i)){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp32A->hd;
struct Cyc_Absyn_Exp*lval=_tmp329(_tmp327,Cyc_Absyn_fieldname(i),0);
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(e->topt));{
void*_tmp32B=e->r;void*_tmp32C=_tmp32B;struct Cyc_Absyn_Vardecl*_tmp32D;struct Cyc_Absyn_Exp*_tmp32E;struct Cyc_Absyn_Exp*_tmp32F;int _tmp330;struct Cyc_List_List*_tmp331;switch(*((int*)_tmp32C)){case 25: _LL186: _tmp331=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp32C)->f1;_LL187:
# 1748
 _tmp328=Cyc_Toc_init_array(nv,(void*)_check_null(e->topt),lval,_tmp331,_tmp328);
goto _LL185;case 26: _LL188: _tmp32D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp32C)->f1;_tmp32E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp32C)->f2;_tmp32F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp32C)->f3;_tmp330=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp32C)->f4;_LL189:
# 1751
 _tmp328=Cyc_Toc_init_comprehension(nv,(void*)_check_null(e->topt),lval,_tmp32D,_tmp32E,_tmp32F,_tmp330,_tmp328,0);
# 1753
goto _LL185;case 27: _LL18A: _LL18B:
# 1755
 goto _LL185;default: _LL18C: _LL18D:
# 1758
 Cyc_Toc_exp_to_c(nv,e);
_tmp328=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp329(_tmp327,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp328,0);
# 1761
goto _LL185;}_LL185:;};}}{
# 1764
struct Cyc_Absyn_Exp*_tmp332=Cyc_Toc_make_struct(nv,Cyc_Absyn_var_exp(_tmp326,0),_tmp325,_tmp328,pointer,rgnopt,is_atomic,1);_npop_handler(0);return _tmp332;};}
# 1731
;_pop_region(r);}
# 1768
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp333=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp333 != 0;_tmp333=_tmp333->tl){
struct Cyc_Absyn_Aggrfield*_tmp334=(struct Cyc_Absyn_Aggrfield*)_tmp333->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp334->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}{
# 1775
struct Cyc_String_pa_PrintArg_struct _tmpADC;void*_tmpADB[1];const char*_tmpADA;void*_tmpAD9;(_tmpAD9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((struct _dyneither_ptr)((_tmpADC.tag=0,((_tmpADC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpADB[0]=& _tmpADC,Cyc_aprintf(((_tmpADA="get_member_offset %s failed",_tag_dyneither(_tmpADA,sizeof(char),28))),_tag_dyneither(_tmpADB,sizeof(void*),1)))))))),_tag_dyneither(_tmpAD9,sizeof(void*),0)));};}
# 1778
static int Cyc_Toc_init_variable_array(void**typ){
{void*_tmp339=Cyc_Tcutil_compress(*typ);void*_tmp33A=_tmp339;void*_tmp33B;struct Cyc_Absyn_Tqual _tmp33C;struct Cyc_Absyn_Exp*_tmp33D;union Cyc_Absyn_Constraint*_tmp33E;unsigned int _tmp33F;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp33A)->tag == 8){_LL18F: _tmp33B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp33A)->f1).elt_type;_tmp33C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp33A)->f1).tq;_tmp33D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp33A)->f1).num_elts;_tmp33E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp33A)->f1).zero_term;_tmp33F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp33A)->f1).zt_loc;_LL190:
# 1783
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp33D))){
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpAE2;struct Cyc_Absyn_ArrayInfo _tmpAE1;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpAE0;*typ=(void*)((_tmpAE0=_cycalloc(sizeof(*_tmpAE0)),((_tmpAE0[0]=((_tmpAE2.tag=8,((_tmpAE2.f1=((_tmpAE1.elt_type=_tmp33B,((_tmpAE1.tq=_tmp33C,((_tmpAE1.num_elts=Cyc_Absyn_uint_exp(0,0),((_tmpAE1.zero_term=_tmp33E,((_tmpAE1.zt_loc=_tmp33F,_tmpAE1)))))))))),_tmpAE2)))),_tmpAE0))));}
return 1;}
# 1787
goto _LL18E;}else{_LL191: _LL192:
 goto _LL18E;}_LL18E:;}
# 1790
return 0;}
# 1793
static int Cyc_Toc_is_array_type(void*t){
void*_tmp343=Cyc_Tcutil_compress(t);void*_tmp344=_tmp343;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp344)->tag == 8){_LL194: _LL195:
 return 1;}else{_LL196: _LL197:
 return 0;}_LL193:;}
# 1800
struct Cyc_Absyn_Aggrdecl*Cyc_Toc_update_aggr_type(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*fn,void*new_field_type){
# 1802
struct Cyc_List_List*new_fields=0;
struct Cyc_List_List*fields;
for(fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;fields != 0;fields=fields->tl){
# 1806
if(!Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,fn)){
struct Cyc_Absyn_Aggrfield*_tmp345=(struct Cyc_Absyn_Aggrfield*)fields->hd;
struct Cyc_List_List*_tmp346=_tmp345->attributes;
void*_tmp347=_tmp345->type;
if((!Cyc_Toc_is_array_type(_tmp347),Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp347)))){
struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpAE8;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpAE7;struct Cyc_List_List*_tmpAE6;_tmp346=((_tmpAE6=_cycalloc(sizeof(*_tmpAE6)),((_tmpAE6->hd=(void*)((_tmpAE8=_cycalloc(sizeof(*_tmpAE8)),((_tmpAE8[0]=((_tmpAE7.tag=6,((_tmpAE7.f1=0,_tmpAE7)))),_tmpAE8)))),((_tmpAE6->tl=_tmp346,_tmpAE6))))));}{
struct Cyc_Absyn_Aggrfield*_tmpAE9;struct Cyc_Absyn_Aggrfield*_tmp34B=(_tmpAE9=_cycalloc(sizeof(*_tmpAE9)),((_tmpAE9->name=_tmp345->name,((_tmpAE9->tq=Cyc_Toc_mt_tq,((_tmpAE9->type=new_field_type,((_tmpAE9->width=_tmp345->width,((_tmpAE9->attributes=_tmp346,((_tmpAE9->requires_clause=0,_tmpAE9)))))))))))));
# 1818
struct Cyc_List_List*_tmpAEA;new_fields=((_tmpAEA=_cycalloc(sizeof(*_tmpAEA)),((_tmpAEA->hd=_tmp34B,((_tmpAEA->tl=new_fields,_tmpAEA))))));};}else{
# 1820
struct Cyc_List_List*_tmpAEB;new_fields=((_tmpAEB=_cycalloc(sizeof(*_tmpAEB)),((_tmpAEB->hd=(struct Cyc_Absyn_Aggrfield*)fields->hd,((_tmpAEB->tl=new_fields,_tmpAEB))))));}}{
# 1822
struct Cyc_List_List*_tmp34F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);
struct Cyc_Int_pa_PrintArg_struct _tmpAF3;void*_tmpAF2[1];const char*_tmpAF1;struct _dyneither_ptr*_tmpAF0;struct _dyneither_ptr*name=
(_tmpAF0=_cycalloc(sizeof(*_tmpAF0)),((_tmpAF0[0]=(struct _dyneither_ptr)((_tmpAF3.tag=1,((_tmpAF3.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpAF2[0]=& _tmpAF3,Cyc_aprintf(((_tmpAF1="_genStruct%d",_tag_dyneither(_tmpAF1,sizeof(char),13))),_tag_dyneither(_tmpAF2,sizeof(void*),1)))))))),_tmpAF0)));
struct _tuple1*_tmpAF4;struct _tuple1*qv=(_tmpAF4=_cycalloc(sizeof(*_tmpAF4)),((_tmpAF4->f1=Cyc_Absyn_Abs_n(0,1),((_tmpAF4->f2=name,_tmpAF4)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpAF5;struct Cyc_Absyn_AggrdeclImpl*_tmp350=(_tmpAF5=_cycalloc(sizeof(*_tmpAF5)),((_tmpAF5->exist_vars=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,((_tmpAF5->rgn_po=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po,((_tmpAF5->fields=_tmp34F,((_tmpAF5->tagged=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged,_tmpAF5)))))))));
# 1830
struct Cyc_Absyn_Aggrdecl*_tmpAF6;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpAF6=_cycalloc(sizeof(*_tmpAF6)),((_tmpAF6->kind=ad->kind,((_tmpAF6->sc=ad->sc,((_tmpAF6->name=qv,((_tmpAF6->tvs=ad->tvs,((_tmpAF6->impl=_tmp350,((_tmpAF6->expected_mem_kind=0,((_tmpAF6->attributes=ad->attributes,_tmpAF6)))))))))))))));
# 1837
Cyc_Toc_aggrdecl_to_c(new_ad,1);
return new_ad;};}
# 1841
static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*nv,void*struct_type,struct Cyc_List_List*exist_types,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*dles,struct _tuple1*tdn){
# 1847
int do_declare=0;
struct Cyc_Absyn_Exp*xexp;
if((nv->struct_info).lhs_exp == 0){
struct _tuple1*_tmp358=Cyc_Toc_temp_var();
xexp=Cyc_Absyn_var_exp(_tmp358,0);
do_declare=1;}else{
# 1855
xexp=(struct Cyc_Absyn_Exp*)_check_null((nv->struct_info).lhs_exp);
nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);}{
# 1858
struct Cyc_Absyn_Stmt*_tmp359=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(xexp),0);
# 1862
struct Cyc_Absyn_Exp**varexp;
{void*_tmp35A=_tmp359->r;void*_tmp35B=_tmp35A;struct Cyc_Absyn_Exp**_tmp35C;if(((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp35B)->tag == 1){_LL199: _tmp35C=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp35B)->f1;_LL19A:
 varexp=_tmp35C;goto _LL198;}else{_LL19B: _LL19C: {
const char*_tmpAF9;void*_tmpAF8;(_tmpAF8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAF9="stmt not an expression!",_tag_dyneither(_tmpAF9,sizeof(char),24))),_tag_dyneither(_tmpAF8,sizeof(void*),0)));}}_LL198:;}{
# 1868
struct Cyc_Absyn_Exp*(*_tmp35F)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
void*aggr_typ=Cyc_Toc_typ_to_c(struct_type);
void*orig_typ=aggr_typ;
# 1872
int is_atomic=1;
struct Cyc_List_List*forall_types;
struct Cyc_Absyn_Aggrdecl*ad;
struct Cyc_List_List*aggrfields;struct Cyc_List_List*orig_aggrfields;
struct _dyneither_ptr**index=0;
{void*_tmp360=Cyc_Tcutil_compress(struct_type);void*_tmp361=_tmp360;union Cyc_Absyn_AggrInfoU _tmp362;struct Cyc_List_List*_tmp363;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp361)->tag == 11){_LL19E: _tmp362=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp361)->f1).aggr_info;_tmp363=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp361)->f1).targs;_LL19F:
# 1879
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp362);
orig_aggrfields=(aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
forall_types=_tmp363;
goto _LL19D;}else{_LL1A0: _LL1A1: {
const char*_tmpAFC;void*_tmpAFB;(_tmpAFB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAFC="init_struct: bad struct type",_tag_dyneither(_tmpAFC,sizeof(char),29))),_tag_dyneither(_tmpAFB,sizeof(void*),0)));}}_LL19D:;}
# 1886
if(exist_types != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields != 0){
struct Cyc_List_List*_tmp366=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
for(0;_tmp366->tl != 0;_tmp366=_tmp366->tl){;}{
struct Cyc_Absyn_Aggrfield*_tmp367=(struct Cyc_Absyn_Aggrfield*)_tmp366->hd;
struct _RegionHandle _tmp368=_new_region("temp");struct _RegionHandle*temp=& _tmp368;_push_region(temp);
{struct Cyc_List_List*_tmp369=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,forall_types);
# 1895
if(Cyc_Toc_is_abstract_type(Cyc_Tcutil_rsubstitute(temp,_tmp369,_tmp367->type))){
struct Cyc_List_List*_tmp36A=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,exist_types);
struct Cyc_List_List*_tmp36B=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(temp,_tmp369,_tmp36A);
# 1899
struct Cyc_List_List*new_fields=0;
for(_tmp366=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;_tmp366 != 0;_tmp366=_tmp366->tl){
struct Cyc_Absyn_Aggrfield*_tmp36C=(struct Cyc_Absyn_Aggrfield*)_tmp366->hd;
struct Cyc_Absyn_Aggrfield*_tmpAFD;struct Cyc_Absyn_Aggrfield*_tmp36D=(_tmpAFD=_cycalloc(sizeof(*_tmpAFD)),((_tmpAFD->name=_tmp36C->name,((_tmpAFD->tq=Cyc_Toc_mt_tq,((_tmpAFD->type=
# 1904
Cyc_Tcutil_rsubstitute(temp,_tmp36B,_tmp36C->type),((_tmpAFD->width=_tmp36C->width,((_tmpAFD->attributes=_tmp36C->attributes,((_tmpAFD->requires_clause=0,_tmpAFD)))))))))))));
# 1912
if(_tmp366->tl == 0){
if(Cyc_Toc_init_variable_array(& _tmp36D->type)){
struct _dyneither_ptr**_tmpAFE;index=((_tmpAFE=_cycalloc(sizeof(*_tmpAFE)),((_tmpAFE[0]=_tmp36D->name,_tmpAFE))));}}{
# 1919
struct Cyc_List_List*_tmpAFF;new_fields=((_tmpAFF=_cycalloc(sizeof(*_tmpAFF)),((_tmpAFF->hd=_tmp36D,((_tmpAFF->tl=new_fields,_tmpAFF))))));};}
# 1921
exist_types=0;
aggrfields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);{
struct Cyc_Int_pa_PrintArg_struct _tmpB07;void*_tmpB06[1];const char*_tmpB05;struct _dyneither_ptr*_tmpB04;struct _dyneither_ptr*name=
(_tmpB04=_cycalloc(sizeof(*_tmpB04)),((_tmpB04[0]=(struct _dyneither_ptr)((_tmpB07.tag=1,((_tmpB07.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpB06[0]=& _tmpB07,Cyc_aprintf(((_tmpB05="_genStruct%d",_tag_dyneither(_tmpB05,sizeof(char),13))),_tag_dyneither(_tmpB06,sizeof(void*),1)))))))),_tmpB04)));
struct _tuple1*_tmpB08;struct _tuple1*qv=(_tmpB08=_cycalloc(sizeof(*_tmpB08)),((_tmpB08->f1=Cyc_Absyn_Abs_n(0,1),((_tmpB08->f2=name,_tmpB08)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpB09;struct Cyc_Absyn_AggrdeclImpl*_tmp371=(_tmpB09=_cycalloc(sizeof(*_tmpB09)),((_tmpB09->exist_vars=0,((_tmpB09->rgn_po=0,((_tmpB09->fields=aggrfields,((_tmpB09->tagged=0,_tmpB09)))))))));
# 1930
struct Cyc_Absyn_Aggrdecl*_tmpB0A;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpB0A=_cycalloc(sizeof(*_tmpB0A)),((_tmpB0A->kind=Cyc_Absyn_StructA,((_tmpB0A->sc=Cyc_Absyn_Public,((_tmpB0A->name=qv,((_tmpB0A->tvs=0,((_tmpB0A->expected_mem_kind=0,((_tmpB0A->impl=_tmp371,((_tmpB0A->attributes=0,_tmpB0A)))))))))))))));
# 1937
Cyc_Toc_aggrdecl_to_c(new_ad,1);
ad=new_ad;
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB14;struct Cyc_Absyn_Aggrdecl**_tmpB13;struct Cyc_Absyn_AggrInfo _tmpB12;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB11;struct_type=(void*)((_tmpB11=_cycalloc(sizeof(*_tmpB11)),((_tmpB11[0]=((_tmpB14.tag=11,((_tmpB14.f1=((_tmpB12.aggr_info=Cyc_Absyn_KnownAggr(((_tmpB13=_cycalloc(sizeof(*_tmpB13)),((_tmpB13[0]=new_ad,_tmpB13))))),((_tmpB12.targs=0,_tmpB12)))),_tmpB14)))),_tmpB11))));}
# 1941
aggr_typ=Cyc_Toc_typ_to_c(struct_type);};}}
# 1891
;_pop_region(temp);};}{
# 1945
int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct _RegionHandle _tmp37D=_new_region("r");struct _RegionHandle*r=& _tmp37D;_push_region(r);
{struct Cyc_List_List*_tmp37E=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp37E != 0;_tmp37E=_tmp37E->tl){
struct _tuple19*_tmp37F=(struct _tuple19*)_tmp37E->hd;struct Cyc_List_List*_tmp381;struct Cyc_Absyn_Exp*_tmp382;struct _tuple19*_tmp380=_tmp37F;_tmp381=_tmp380->f1;_tmp382=_tmp380->f2;
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(_tmp382->topt));
if(_tmp381 == 0){
const char*_tmpB17;void*_tmpB16;(_tmpB16=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB17="empty designator list",_tag_dyneither(_tmpB17,sizeof(char),22))),_tag_dyneither(_tmpB16,sizeof(void*),0)));}
if(_tmp381->tl != 0){
# 1955
struct _tuple1*_tmp385=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp386=Cyc_Absyn_var_exp(_tmp385,0);
for(0;_tmp381 != 0;_tmp381=_tmp381->tl){
void*_tmp387=(void*)_tmp381->hd;void*_tmp388=_tmp387;struct _dyneither_ptr*_tmp389;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp388)->tag == 1){_LL1A3: _tmp389=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp388)->f1;_LL1A4:
# 1961
 if(Cyc_Toc_is_poly_field(struct_type,_tmp389))
_tmp386=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp386);
_tmp359=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp35F(xexp,_tmp389,0),_tmp386,0),0),_tmp359,0);
# 1965
goto _LL1A2;}else{_LL1A5: _LL1A6: {
const char*_tmpB1A;void*_tmpB19;(_tmpB19=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB1A="array designator in struct",_tag_dyneither(_tmpB1A,sizeof(char),27))),_tag_dyneither(_tmpB19,sizeof(void*),0)));}}_LL1A2:;}
# 1969
Cyc_Toc_exp_to_c(nv,_tmp382);
_tmp359=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp386,_tmp382,0),0),_tmp359,0);}else{
# 1972
void*_tmp38C=(void*)_tmp381->hd;void*_tmp38D=_tmp38C;struct _dyneither_ptr*_tmp38E;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp38D)->tag == 1){_LL1A8: _tmp38E=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp38D)->f1;_LL1A9: {
# 1974
struct Cyc_Absyn_Exp*lval=_tmp35F(xexp,_tmp38E,0);
if(is_tagged_union){
int i=Cyc_Toc_get_member_offset(ad,_tmp38E);
struct Cyc_Absyn_Exp*f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct Cyc_Absyn_Exp*lhs=Cyc_Toc_member_exp(lval,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,f_tag_exp,0);
_tmp359=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp359,0);
lval=Cyc_Toc_member_exp(lval,Cyc_Toc_val_sp,0);}{
# 1983
int e1_translated=0;
{void*_tmp38F=_tmp382->r;void*_tmp390=_tmp38F;void*_tmp391;struct Cyc_List_List*_tmp392;struct Cyc_Absyn_Exp*_tmp393;void*_tmp394;struct Cyc_Absyn_Vardecl*_tmp395;struct Cyc_Absyn_Exp*_tmp396;struct Cyc_Absyn_Exp*_tmp397;int _tmp398;struct Cyc_List_List*_tmp399;switch(*((int*)_tmp390)){case 25: _LL1AD: _tmp399=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp390)->f1;_LL1AE:
# 1986
 _tmp359=Cyc_Toc_init_array(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(aggrfields,_tmp38E)))->type,lval,_tmp399,_tmp359);
# 1988
goto _LL1AC;case 26: _LL1AF: _tmp395=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp390)->f1;_tmp396=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp390)->f2;_tmp397=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp390)->f3;_tmp398=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp390)->f4;_LL1B0:
# 1990
 _tmp359=Cyc_Toc_init_comprehension(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(do_declare?aggrfields: orig_aggrfields,_tmp38E)))->type,lval,_tmp395,_tmp396,_tmp397,_tmp398,_tmp359,0);
# 1994
e1_translated=1;
_tmp393=_tmp396;_tmp394=(void*)_check_null(_tmp397->topt);goto _LL1B2;case 27: _LL1B1: _tmp393=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp390)->f1;_tmp394=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp390)->f2;_LL1B2:
# 1999
 if(index != 0  && !Cyc_strptrcmp(*index,_tmp38E)){
if(!e1_translated)Cyc_Toc_exp_to_c(nv,_tmp393);{
void*_tmp39A=Cyc_Toc_typ_to_c(_tmp394);
struct _tuple6 _tmpB1E;union Cyc_Absyn_Cnst _tmpB1D;struct Cyc_Absyn_Exp*_tmpB1B[2];*(nv->struct_info).varsizeexp=
Cyc_Absyn_add_exp(Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpB1B[1]=Cyc_Absyn_sizeoftyp_exp(_tmp39A,0),((_tmpB1B[0]=_tmp393,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB1B,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),
Cyc_Absyn_const_exp((((_tmpB1D.Int_c).val=((_tmpB1E.f1=Cyc_Absyn_Unsigned,((_tmpB1E.f2=(int)sizeof(double),_tmpB1E)))),(((_tmpB1D.Int_c).tag=5,_tmpB1D)))),0),0);};}
# 2007
goto _LL1AC;case 29: _LL1B3: _tmp391=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp390)->f1;_tmp392=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp390)->f2;_LL1B4:
# 2009
 _tmp359=Cyc_Toc_init_anon_struct(nv,lval,_tmp391,_tmp392,_tmp359);goto _LL1AC;default: _LL1B5: _LL1B6: {
# 2011
void*old_e_typ=(void*)_check_null(_tmp382->topt);
int was_ptr_type=Cyc_Toc_is_pointer_or_boxed_tvar(old_e_typ);
Cyc_Toc_exp_to_c(Cyc_Toc_set_lhs_exp(nv->rgn,lval,nv),_tmp382);
{void*_tmp39E=old_e_typ;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp39E)->tag == 11){_LL1B8: _LL1B9:
# 2018
 if(old_e_typ != _tmp382->topt  && !Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(old_e_typ),(void*)_check_null(_tmp382->topt))){
# 2022
ad=Cyc_Toc_update_aggr_type(ad,_tmp38E,(void*)_check_null(_tmp382->topt));
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB28;struct Cyc_Absyn_Aggrdecl**_tmpB27;struct Cyc_Absyn_AggrInfo _tmpB26;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB25;struct_type=(void*)(
(_tmpB25=_cycalloc(sizeof(*_tmpB25)),((_tmpB25[0]=((_tmpB28.tag=11,((_tmpB28.f1=((_tmpB26.aggr_info=Cyc_Absyn_KnownAggr(((_tmpB27=_cycalloc(sizeof(*_tmpB27)),((_tmpB27[0]=ad,_tmpB27))))),((_tmpB26.targs=0,_tmpB26)))),_tmpB28)))),_tmpB25))));}
# 2026
aggr_typ=Cyc_Toc_typ_to_c(struct_type);}
# 2028
goto _LL1B7;}else{_LL1BA: _LL1BB:
 goto _LL1B7;}_LL1B7:;}{
# 2031
struct Cyc_Absyn_Aggrfield*_tmp3A3=Cyc_Absyn_lookup_field(aggrfields,_tmp38E);
# 2033
if(Cyc_Toc_is_poly_field(struct_type,_tmp38E) && !was_ptr_type)
_tmp382=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp382);
# 2036
if(exist_types != 0)
_tmp382=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3A3))->type),_tmp382);
# 2039
_tmp359=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp382,0),0),_tmp359,0);
goto _LL1AC;};}}_LL1AC:;}
# 2042
goto _LL1A7;};}}else{_LL1AA: _LL1AB: {
const char*_tmpB2B;void*_tmpB2A;(_tmpB2A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB2B="array designator in struct",_tag_dyneither(_tmpB2B,sizeof(char),27))),_tag_dyneither(_tmpB2A,sizeof(void*),0)));}}_LL1A7:;}}}
# 2049
if(aggr_typ != orig_typ  && pointer){
void*cast_type=Cyc_Absyn_cstar_typ(orig_typ,Cyc_Toc_mt_tq);
*varexp=Cyc_Toc_cast_it(cast_type,*varexp);}{
# 2054
struct Cyc_Absyn_Exp*_tmp3A6=Cyc_Toc_make_struct(nv,xexp,aggr_typ,_tmp359,pointer,rgnopt,is_atomic,do_declare);
# 2056
_tmp3A6->topt=struct_type;{
struct Cyc_Absyn_Exp*_tmp3A7=_tmp3A6;_npop_handler(0);return _tmp3A7;};};
# 1947
;_pop_region(r);};};};}struct _tuple27{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 2062
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple27*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
# 2065
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0);}
# 2068
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmpB2E;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmpB2D;return Cyc_Absyn_new_exp((void*)((_tmpB2D=_cycalloc(sizeof(*_tmpB2D)),((_tmpB2D[0]=((_tmpB2E.tag=4,((_tmpB2E.f1=e1,((_tmpB2E.f2=incr,_tmpB2E)))))),_tmpB2D)))),0);}
# 2072
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 2081
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 2083
void*_tmp3AA=e1->r;void*_tmp3AB=_tmp3AA;struct Cyc_Absyn_Exp*_tmp3AC;struct _dyneither_ptr*_tmp3AD;int _tmp3AE;int _tmp3AF;void*_tmp3B0;struct Cyc_Absyn_Exp*_tmp3B1;struct Cyc_Absyn_Stmt*_tmp3B2;switch(*((int*)_tmp3AB)){case 36: _LL1BD: _tmp3B2=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3AB)->f1;_LL1BE:
 Cyc_Toc_lvalue_assign_stmt(_tmp3B2,fs,f,f_env);goto _LL1BC;case 13: _LL1BF: _tmp3B0=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3AB)->f1;_tmp3B1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3AB)->f2;_LL1C0:
 Cyc_Toc_lvalue_assign(_tmp3B1,fs,f,f_env);goto _LL1BC;case 20: _LL1C1: _tmp3AC=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3AB)->f1;_tmp3AD=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3AB)->f2;_tmp3AE=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3AB)->f3;_tmp3AF=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3AB)->f4;_LL1C2:
# 2088
 e1->r=_tmp3AC->r;
{struct Cyc_List_List*_tmpB2F;Cyc_Toc_lvalue_assign(e1,((_tmpB2F=_cycalloc(sizeof(*_tmpB2F)),((_tmpB2F->hd=_tmp3AD,((_tmpB2F->tl=fs,_tmpB2F)))))),f,f_env);}
goto _LL1BC;default: _LL1C3: _LL1C4: {
# 2096
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 2098
for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;
goto _LL1BC;}}_LL1BC:;}
# 2104
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 2106
void*_tmp3B4=s->r;void*_tmp3B5=_tmp3B4;struct Cyc_Absyn_Stmt*_tmp3B6;struct Cyc_Absyn_Decl*_tmp3B7;struct Cyc_Absyn_Stmt*_tmp3B8;struct Cyc_Absyn_Exp*_tmp3B9;switch(*((int*)_tmp3B5)){case 1: _LL1C6: _tmp3B9=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3B5)->f1;_LL1C7:
 Cyc_Toc_lvalue_assign(_tmp3B9,fs,f,f_env);goto _LL1C5;case 12: _LL1C8: _tmp3B7=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3B5)->f1;_tmp3B8=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3B5)->f2;_LL1C9:
# 2109
 Cyc_Toc_lvalue_assign_stmt(_tmp3B8,fs,f,f_env);goto _LL1C5;case 2: _LL1CA: _tmp3B6=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3B5)->f2;_LL1CB:
 Cyc_Toc_lvalue_assign_stmt(_tmp3B6,fs,f,f_env);goto _LL1C5;default: _LL1CC: _LL1CD: {
const char*_tmpB33;void*_tmpB32[1];struct Cyc_String_pa_PrintArg_struct _tmpB31;(_tmpB31.tag=0,((_tmpB31.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpB32[0]=& _tmpB31,Cyc_Toc_toc_impos(((_tmpB33="lvalue_assign_stmt: %s",_tag_dyneither(_tmpB33,sizeof(char),23))),_tag_dyneither(_tmpB32,sizeof(void*),1)))))));}}_LL1C5:;}
# 2115
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 2119
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp3BD=e->r;void*_tmp3BE=_tmp3BD;struct Cyc_Absyn_Stmt*_tmp3BF;struct Cyc_Absyn_Exp*_tmp3C0;void**_tmp3C1;struct Cyc_Absyn_Exp**_tmp3C2;switch(*((int*)_tmp3BE)){case 13: _LL1CF: _tmp3C1=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3BE)->f1;_tmp3C2=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3BE)->f2;_LL1D0:
# 2122
*_tmp3C2=Cyc_Toc_push_address_exp(*_tmp3C2);
*_tmp3C1=Cyc_Absyn_cstar_typ(*_tmp3C1,Cyc_Toc_mt_tq);
return e;case 19: _LL1D1: _tmp3C0=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3BE)->f1;_LL1D2:
# 2126
 return _tmp3C0;case 36: _LL1D3: _tmp3BF=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3BE)->f1;_LL1D4:
# 2130
 Cyc_Toc_push_address_stmt(_tmp3BF);
return e;default: _LL1D5: _LL1D6:
# 2133
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{
const char*_tmpB37;void*_tmpB36[1];struct Cyc_String_pa_PrintArg_struct _tmpB35;(_tmpB35.tag=0,((_tmpB35.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpB36[0]=& _tmpB35,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB37="can't take & of exp %s",_tag_dyneither(_tmpB37,sizeof(char),23))),_tag_dyneither(_tmpB36,sizeof(void*),1)))))));};}_LL1CE:;}
# 2138
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp3C6=s->r;void*_tmp3C7=_tmp3C6;struct Cyc_Absyn_Exp**_tmp3C8;struct Cyc_Absyn_Stmt*_tmp3C9;struct Cyc_Absyn_Stmt*_tmp3CA;switch(*((int*)_tmp3C7)){case 2: _LL1D8: _tmp3CA=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3C7)->f2;_LL1D9:
 _tmp3C9=_tmp3CA;goto _LL1DB;case 12: _LL1DA: _tmp3C9=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3C7)->f2;_LL1DB:
 Cyc_Toc_push_address_stmt(_tmp3C9);goto _LL1D7;case 1: _LL1DC: _tmp3C8=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3C7)->f1;_LL1DD:
*_tmp3C8=Cyc_Toc_push_address_exp(*_tmp3C8);goto _LL1D7;default: _LL1DE: _LL1DF: {
# 2144
const char*_tmpB3B;void*_tmpB3A[1];struct Cyc_String_pa_PrintArg_struct _tmpB39;(_tmpB39.tag=0,((_tmpB39.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpB3A[0]=& _tmpB39,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB3B="can't take & of stmt %s",_tag_dyneither(_tmpB3B,sizeof(char),24))),_tag_dyneither(_tmpB3A,sizeof(void*),1)))))));}}_LL1D7:;}
# 2149
static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 2151
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 2153
if(x == 0)return 0;
{struct Cyc_List_List*_tmpB3C;result=((_tmpB3C=_region_malloc(r2,sizeof(*_tmpB3C)),((_tmpB3C->hd=f(x->hd,env),((_tmpB3C->tl=0,_tmpB3C))))));}
prev=result;
for(x=x->tl;x != 0;x=x->tl){
{struct Cyc_List_List*_tmpB3D;((struct Cyc_List_List*)_check_null(prev))->tl=((_tmpB3D=_region_malloc(r2,sizeof(*_tmpB3D)),((_tmpB3D->hd=f(x->hd,env),((_tmpB3D->tl=0,_tmpB3D))))));}
prev=prev->tl;}
# 2160
return result;}
# 2162
static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,x);}
# 2166
static struct _tuple19*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){
struct _tuple19*_tmpB3E;return(_tmpB3E=_cycalloc(sizeof(*_tmpB3E)),((_tmpB3E->f1=0,((_tmpB3E->f2=e,_tmpB3E)))));}
# 2170
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_tmp3D1=Cyc_Tcutil_compress(t);void*_tmp3D2=_tmp3D1;struct Cyc_Absyn_PtrInfo _tmp3D3;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D2)->tag == 5){_LL1E1: _tmp3D3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D2)->f1;_LL1E2:
 return _tmp3D3;}else{_LL1E3: _LL1E4: {
const char*_tmpB41;void*_tmpB40;(_tmpB40=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB41="get_ptr_typ: not a pointer!",_tag_dyneither(_tmpB41,sizeof(char),28))),_tag_dyneither(_tmpB40,sizeof(void*),0)));}}_LL1E0:;}
# 2180
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp3D6=Cyc_Tcutil_compress(t);void*_tmp3D7=_tmp3D6;int _tmp3D8;enum Cyc_Absyn_Sign _tmp3D9;enum Cyc_Absyn_Sign _tmp3DA;enum Cyc_Absyn_Sign _tmp3DB;enum Cyc_Absyn_Sign _tmp3DC;enum Cyc_Absyn_Sign _tmp3DD;switch(*((int*)_tmp3D7)){case 5: _LL1E6: _LL1E7:
 res=Cyc_Absyn_null_exp(0);goto _LL1E5;case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3D7)->f2){case Cyc_Absyn_Char_sz: _LL1E8: _tmp3DD=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3D7)->f1;_LL1E9:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp3DD,'\000'),0);goto _LL1E5;case Cyc_Absyn_Short_sz: _LL1EA: _tmp3DC=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3D7)->f1;_LL1EB:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp3DC,0),0);goto _LL1E5;case Cyc_Absyn_Int_sz: _LL1F0: _tmp3DB=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3D7)->f1;_LL1F1:
# 2188
 _tmp3DA=_tmp3DB;goto _LL1F3;case Cyc_Absyn_Long_sz: _LL1F2: _tmp3DA=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3D7)->f1;_LL1F3:
# 2190
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp3DA,0),0);goto _LL1E5;default: _LL1F4: _tmp3D9=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3D7)->f1;_LL1F5:
# 2192
 res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp3D9,(long long)0),0);goto _LL1E5;}case 13: _LL1EC: _LL1ED:
# 2186
 goto _LL1EF;case 14: _LL1EE: _LL1EF:
 _tmp3DB=Cyc_Absyn_Unsigned;goto _LL1F1;case 7: switch(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3D7)->f1){case 0: _LL1F6: _LL1F7:
# 2193
{const char*_tmpB42;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpB42="0.0F",_tag_dyneither(_tmpB42,sizeof(char),5))),0),0);}goto _LL1E5;case 1: _LL1F8: _LL1F9:
{const char*_tmpB43;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpB43="0.0",_tag_dyneither(_tmpB43,sizeof(char),4))),1),0);}goto _LL1E5;default: _LL1FA: _tmp3D8=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3D7)->f1;_LL1FB:
{const char*_tmpB44;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpB44="0.0L",_tag_dyneither(_tmpB44,sizeof(char),5))),_tmp3D8),0);}goto _LL1E5;}default: _LL1FC: _LL1FD: {
# 2197
const char*_tmpB48;void*_tmpB47[1];struct Cyc_String_pa_PrintArg_struct _tmpB46;(_tmpB46.tag=0,((_tmpB46.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpB47[0]=& _tmpB46,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB48="found non-zero type %s in generate_zero",_tag_dyneither(_tmpB48,sizeof(char),40))),_tag_dyneither(_tmpB47,sizeof(void*),1)))))));}}_LL1E5:;}
# 2199
res->topt=t;
return res;}
# 2206
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 2219 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp3E4=Cyc_Toc_typ_to_c(elt_type);
void*_tmp3E5=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp3E6=Cyc_Absyn_cstar_typ(_tmp3E4,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmpB49;struct Cyc_Core_Opt*_tmp3E7=(_tmpB49=_cycalloc(sizeof(*_tmpB49)),((_tmpB49->v=_tmp3E6,_tmpB49)));
struct Cyc_Absyn_Exp*xinit;
{void*_tmp3E8=e1->r;void*_tmp3E9=_tmp3E8;struct Cyc_Absyn_Exp*_tmp3EA;struct Cyc_Absyn_Exp*_tmp3EB;struct Cyc_Absyn_Exp*_tmp3EC;switch(*((int*)_tmp3E9)){case 19: _LL1FF: _tmp3EC=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3E9)->f1;_LL200:
# 2227
 if(!is_dyneither){
_tmp3EC=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp3EC,0,Cyc_Absyn_Other_coercion,0);
_tmp3EC->topt=fat_ptr_type;}
# 2231
Cyc_Toc_exp_to_c(nv,_tmp3EC);xinit=_tmp3EC;goto _LL1FE;case 22: _LL201: _tmp3EA=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3E9)->f1;_tmp3EB=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3E9)->f2;_LL202:
# 2233
 if(!is_dyneither){
_tmp3EA=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp3EA,0,Cyc_Absyn_Other_coercion,0);
_tmp3EA->topt=fat_ptr_type;}
# 2237
Cyc_Toc_exp_to_c(nv,_tmp3EA);Cyc_Toc_exp_to_c(nv,_tmp3EB);
{struct Cyc_Absyn_Exp*_tmpB4A[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpB4A[2]=_tmp3EB,((_tmpB4A[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpB4A[0]=_tmp3EA,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB4A,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL1FE;default: _LL203: _LL204: {
const char*_tmpB4D;void*_tmpB4C;(_tmpB4C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB4D="found bad lhs for zero-terminated pointer assignment",_tag_dyneither(_tmpB4D,sizeof(char),53))),_tag_dyneither(_tmpB4C,sizeof(void*),0)));}}_LL1FE:;}{
# 2243
struct _tuple1*_tmp3F0=Cyc_Toc_temp_var();
struct _RegionHandle _tmp3F1=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp3F1;_push_region(rgn2);
{struct Cyc_Toc_Env*_tmp3F2=Cyc_Toc_add_varmap(rgn2,nv,_tmp3F0,Cyc_Absyn_var_exp(_tmp3F0,0));
struct Cyc_Absyn_Vardecl*_tmpB4E;struct Cyc_Absyn_Vardecl*_tmp3F3=(_tmpB4E=_cycalloc(sizeof(*_tmpB4E)),((_tmpB4E->sc=Cyc_Absyn_Public,((_tmpB4E->name=_tmp3F0,((_tmpB4E->tq=Cyc_Toc_mt_tq,((_tmpB4E->type=_tmp3E5,((_tmpB4E->initializer=xinit,((_tmpB4E->rgn=0,((_tmpB4E->attributes=0,((_tmpB4E->escapes=0,_tmpB4E)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpB51;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB50;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp3F4=(_tmpB50=_cycalloc(sizeof(*_tmpB50)),((_tmpB50[0]=((_tmpB51.tag=4,((_tmpB51.f1=_tmp3F3,_tmpB51)))),_tmpB50)));
struct Cyc_Absyn_Exp*_tmp3F5=Cyc_Absyn_varb_exp(_tmp3F0,(void*)_tmp3F4,0);
_tmp3F5->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp3F6=Cyc_Absyn_deref_exp(_tmp3F5,0);
_tmp3F6->topt=elt_type;
Cyc_Toc_exp_to_c(_tmp3F2,_tmp3F6);{
struct _tuple1*_tmp3F7=Cyc_Toc_temp_var();
_tmp3F2=Cyc_Toc_add_varmap(rgn2,_tmp3F2,_tmp3F7,Cyc_Absyn_var_exp(_tmp3F7,0));{
struct Cyc_Absyn_Vardecl*_tmpB52;struct Cyc_Absyn_Vardecl*_tmp3F8=(_tmpB52=_cycalloc(sizeof(*_tmpB52)),((_tmpB52->sc=Cyc_Absyn_Public,((_tmpB52->name=_tmp3F7,((_tmpB52->tq=Cyc_Toc_mt_tq,((_tmpB52->type=_tmp3E4,((_tmpB52->initializer=_tmp3F6,((_tmpB52->rgn=0,((_tmpB52->attributes=0,((_tmpB52->escapes=0,_tmpB52)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpB55;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB54;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp3F9=(_tmpB54=_cycalloc(sizeof(*_tmpB54)),((_tmpB54[0]=((_tmpB55.tag=4,((_tmpB55.f1=_tmp3F8,_tmpB55)))),_tmpB54)));
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp3FA=Cyc_Absyn_varb_exp(_tmp3F7,(void*)_tmp3F9,0);
_tmp3FA->topt=_tmp3F6->topt;
z_init=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp3FA,e2,0);
z_init->topt=_tmp3FA->topt;}
# 2264
Cyc_Toc_exp_to_c(_tmp3F2,z_init);{
struct _tuple1*_tmp3FB=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmpB56;struct Cyc_Absyn_Vardecl*_tmp3FC=(_tmpB56=_cycalloc(sizeof(*_tmpB56)),((_tmpB56->sc=Cyc_Absyn_Public,((_tmpB56->name=_tmp3FB,((_tmpB56->tq=Cyc_Toc_mt_tq,((_tmpB56->type=_tmp3E4,((_tmpB56->initializer=z_init,((_tmpB56->rgn=0,((_tmpB56->attributes=0,((_tmpB56->escapes=0,_tmpB56)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpB59;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB58;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp3FD=(_tmpB58=_cycalloc(sizeof(*_tmpB58)),((_tmpB58[0]=((_tmpB59.tag=4,((_tmpB59.f1=_tmp3FC,_tmpB59)))),_tmpB58)));
_tmp3F2=Cyc_Toc_add_varmap(rgn2,_tmp3F2,_tmp3FB,Cyc_Absyn_var_exp(_tmp3FB,0));{
# 2271
struct Cyc_Absyn_Exp*_tmp3FE=Cyc_Absyn_varb_exp(_tmp3F7,(void*)_tmp3F9,0);_tmp3FE->topt=_tmp3F6->topt;{
struct Cyc_Absyn_Exp*_tmp3FF=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp400=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp3FE,_tmp3FF,0);
_tmp400->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp3F2,_tmp400);{
# 2277
struct Cyc_Absyn_Exp*_tmp401=Cyc_Absyn_varb_exp(_tmp3FB,(void*)_tmp3FD,0);_tmp401->topt=_tmp3F6->topt;{
struct Cyc_Absyn_Exp*_tmp402=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp403=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp401,_tmp402,0);
_tmp403->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp3F2,_tmp403);{
# 2283
struct Cyc_Absyn_Exp*_tmpB5A[2];struct Cyc_List_List*_tmp404=(_tmpB5A[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpB5A[0]=
# 2283
Cyc_Absyn_varb_exp(_tmp3F0,(void*)_tmp3F4,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB5A,sizeof(struct Cyc_Absyn_Exp*),2)))));
# 2285
struct Cyc_Absyn_Exp*_tmp405=Cyc_Absyn_uint_exp(1,0);
struct Cyc_Absyn_Exp*xsize;
xsize=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp404,0),_tmp405,0);{
# 2290
struct Cyc_Absyn_Exp*_tmp406=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp400,_tmp403,0),0);
struct Cyc_Absyn_Stmt*_tmp407=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0),0);
struct Cyc_Absyn_Exp*_tmp408=Cyc_Toc_member_exp(Cyc_Absyn_varb_exp(_tmp3F0,(void*)_tmp3F4,0),Cyc_Toc_curr_sp,0);
_tmp408=Cyc_Toc_cast_it(_tmp3E6,_tmp408);{
struct Cyc_Absyn_Exp*_tmp409=Cyc_Absyn_deref_exp(_tmp408,0);
struct Cyc_Absyn_Exp*_tmp40A=Cyc_Absyn_assign_exp(_tmp409,Cyc_Absyn_var_exp(_tmp3FB,0),0);
struct Cyc_Absyn_Stmt*_tmp40B=Cyc_Absyn_exp_stmt(_tmp40A,0);
_tmp40B=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp406,_tmp407,Cyc_Absyn_skip_stmt(0),0),_tmp40B,0);
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB60;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB5F;struct Cyc_Absyn_Decl*_tmpB5E;_tmp40B=Cyc_Absyn_decl_stmt(((_tmpB5E=_cycalloc(sizeof(*_tmpB5E)),((_tmpB5E->r=(void*)((_tmpB60=_cycalloc(sizeof(*_tmpB60)),((_tmpB60[0]=((_tmpB5F.tag=0,((_tmpB5F.f1=_tmp3FC,_tmpB5F)))),_tmpB60)))),((_tmpB5E->loc=0,_tmpB5E)))))),_tmp40B,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB66;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB65;struct Cyc_Absyn_Decl*_tmpB64;_tmp40B=Cyc_Absyn_decl_stmt(((_tmpB64=_cycalloc(sizeof(*_tmpB64)),((_tmpB64->r=(void*)((_tmpB66=_cycalloc(sizeof(*_tmpB66)),((_tmpB66[0]=((_tmpB65.tag=0,((_tmpB65.f1=_tmp3F8,_tmpB65)))),_tmpB66)))),((_tmpB64->loc=0,_tmpB64)))))),_tmp40B,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB6C;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB6B;struct Cyc_Absyn_Decl*_tmpB6A;_tmp40B=Cyc_Absyn_decl_stmt(((_tmpB6A=_cycalloc(sizeof(*_tmpB6A)),((_tmpB6A->r=(void*)((_tmpB6C=_cycalloc(sizeof(*_tmpB6C)),((_tmpB6C[0]=((_tmpB6B.tag=0,((_tmpB6B.f1=_tmp3F3,_tmpB6B)))),_tmpB6C)))),((_tmpB6A->loc=0,_tmpB6A)))))),_tmp40B,0);}
e->r=Cyc_Toc_stmt_exp_r(_tmp40B);};};};};};};};};};};};}
# 2245
;_pop_region(rgn2);};}
# 2316 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 2320
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp420=Cyc_Tcutil_compress(aggrtype);void*_tmp421=_tmp420;union Cyc_Absyn_AggrInfoU _tmp422;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp421)->tag == 11){_LL206: _tmp422=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp421)->f1).aggr_info;_LL207:
# 2323
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp422);goto _LL205;}else{_LL208: _LL209: {
struct Cyc_String_pa_PrintArg_struct _tmpB74;void*_tmpB73[1];const char*_tmpB72;void*_tmpB71;(_tmpB71=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmpB74.tag=0,((_tmpB74.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpB73[0]=& _tmpB74,Cyc_aprintf(((_tmpB72="expecting union but found %s in check_tagged_union",_tag_dyneither(_tmpB72,sizeof(char),51))),_tag_dyneither(_tmpB73,sizeof(void*),1)))))))),_tag_dyneither(_tmpB71,sizeof(void*),0)));}}_LL205:;}{
# 2327
struct _tuple1*_tmp427=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp428=Cyc_Absyn_var_exp(_tmp427,0);
struct Cyc_Absyn_Exp*_tmp429=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp42A=Cyc_Absyn_aggrarrow_exp(_tmp428,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp42B=Cyc_Absyn_neq_exp(_tmp42A,_tmp429,0);
struct Cyc_Absyn_Exp*_tmp42C=Cyc_Absyn_aggrarrow_exp(_tmp428,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp42D=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp42C,0),0);
struct Cyc_Absyn_Stmt*_tmp42E=Cyc_Absyn_ifthenelse_stmt(_tmp42B,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
void*_tmp42F=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp430=Cyc_Absyn_address_exp(aggrproj(e1,f,0),0);
struct Cyc_Absyn_Stmt*_tmp431=Cyc_Absyn_declare_stmt(_tmp427,_tmp42F,_tmp430,Cyc_Absyn_seq_stmt(_tmp42E,_tmp42D,0),0);
return Cyc_Toc_stmt_exp_r(_tmp431);}else{
# 2341
struct Cyc_Absyn_Exp*_tmp432=Cyc_Toc_member_exp(aggrproj(_tmp428,f,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp433=Cyc_Absyn_neq_exp(_tmp432,_tmp429,0);
struct Cyc_Absyn_Exp*_tmp434=Cyc_Toc_member_exp(aggrproj(_tmp428,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp435=Cyc_Absyn_exp_stmt(_tmp434,0);
struct Cyc_Absyn_Stmt*_tmp436=Cyc_Absyn_ifthenelse_stmt(_tmp433,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*_tmp437=Cyc_Absyn_declare_stmt(_tmp427,e1_c_type,e1,Cyc_Absyn_seq_stmt(_tmp436,_tmp435,0),0);
return Cyc_Toc_stmt_exp_r(_tmp437);}};}
# 2351
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _dyneither_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 2354
void*_tmp438=Cyc_Tcutil_compress(t);void*_tmp439=_tmp438;union Cyc_Absyn_AggrInfoU _tmp43A;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp439)->tag == 11){_LL20B: _tmp43A=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp439)->f1).aggr_info;_LL20C: {
# 2356
struct Cyc_Absyn_Aggrdecl*_tmp43B=Cyc_Absyn_get_known_aggrdecl(_tmp43A);
*f_tag=Cyc_Toc_get_member_offset(_tmp43B,f);{
# 2359
const char*_tmpB79;void*_tmpB78[2];struct Cyc_String_pa_PrintArg_struct _tmpB77;struct Cyc_String_pa_PrintArg_struct _tmpB76;struct _dyneither_ptr str=(struct _dyneither_ptr)((_tmpB76.tag=0,((_tmpB76.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpB77.tag=0,((_tmpB77.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp43B->name).f2),((_tmpB78[0]=& _tmpB77,((_tmpB78[1]=& _tmpB76,Cyc_aprintf(((_tmpB79="_union_%s_%s",_tag_dyneither(_tmpB79,sizeof(char),13))),_tag_dyneither(_tmpB78,sizeof(void*),2))))))))))))));
{struct _dyneither_ptr*_tmpB7A;*tagged_member_type=Cyc_Absyn_strct(((_tmpB7A=_cycalloc(sizeof(*_tmpB7A)),((_tmpB7A[0]=str,_tmpB7A)))));}
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp43B->impl))->tagged;};}}else{_LL20D: _LL20E:
 return 0;}_LL20A:;}
# 2370
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 2373
void*_tmp441=e->r;void*_tmp442=_tmp441;struct Cyc_Absyn_Exp*_tmp443;struct _dyneither_ptr*_tmp444;int*_tmp445;struct Cyc_Absyn_Exp*_tmp446;struct _dyneither_ptr*_tmp447;int*_tmp448;struct Cyc_Absyn_Exp*_tmp449;switch(*((int*)_tmp442)){case 13: _LL210: _tmp449=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp442)->f2;_LL211: {
const char*_tmpB7D;void*_tmpB7C;(_tmpB7C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB7D="cast on lhs!",_tag_dyneither(_tmpB7D,sizeof(char),13))),_tag_dyneither(_tmpB7C,sizeof(void*),0)));}case 20: _LL212: _tmp446=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp442)->f1;_tmp447=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp442)->f2;_tmp448=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp442)->f4;_LL213:
# 2376
 return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(_tmp446->topt),_tmp447,f_tag,tagged_member_type,clear_read,_tmp448);case 21: _LL214: _tmp443=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp442)->f1;_tmp444=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp442)->f2;_tmp445=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp442)->f4;_LL215: {
# 2379
void*_tmp44C=Cyc_Tcutil_compress((void*)_check_null(_tmp443->topt));void*_tmp44D=_tmp44C;void*_tmp44E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44D)->tag == 5){_LL219: _tmp44E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44D)->f1).elt_typ;_LL21A:
# 2381
 return Cyc_Toc_is_tagged_union_project_impl(_tmp44E,_tmp444,f_tag,tagged_member_type,clear_read,_tmp445);}else{_LL21B: _LL21C:
# 2383
 return 0;}_LL218:;}default: _LL216: _LL217:
# 2385
 return 0;}_LL20F:;}
# 2397 "toc.cyc"
void Cyc_Toc_add_tagged_union_check_for_swap(struct Cyc_Absyn_Exp*e,int tag,void*mem_type){
struct _tuple1*_tmp44F=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp44F,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_val,0),0);
struct Cyc_Absyn_Exp*_tmp450=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_ifthenelse_stmt(_tmp450,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp44F,Cyc_Absyn_cstar_typ(mem_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(e)),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
e->r=Cyc_Toc_stmt_exp_r(s1);}
# 2421 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 2425
struct _tuple1*_tmp451=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp451,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{
# 2435
struct Cyc_Absyn_Exp*_tmp452=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
s2=Cyc_Absyn_ifthenelse_stmt(_tmp452,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{
# 2438
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp451,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(e1),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple28{struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};
# 2444
static void Cyc_Toc_new_comprehension_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,void*new_e_type,struct Cyc_Absyn_Exp*rgnopt,void*old_typ,struct Cyc_Absyn_Exp*e1,void*t1,int zero_term,struct Cyc_Absyn_Exp*init_exp,struct Cyc_Absyn_Vardecl*vd){
# 2459 "toc.cyc"
struct _tuple1*max=Cyc_Toc_temp_var();
struct _tuple1*a=Cyc_Toc_temp_var();
void*old_elt_typ=t1;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c(nv,e1);{
struct Cyc_Absyn_Exp*_tmp453=Cyc_Absyn_var_exp(max,0);
# 2467
struct Cyc_Absyn_Stmt*s;
if(zero_term)
_tmp453=Cyc_Absyn_add_exp(_tmp453,Cyc_Absyn_uint_exp(1,0),0);
if(init_exp != 0)
s=Cyc_Toc_init_comprehension(nv,new_e_type,
Cyc_Absyn_var_exp(a,0),(struct Cyc_Absyn_Vardecl*)_check_null(vd),Cyc_Absyn_var_exp(max,0),init_exp,zero_term,
Cyc_Toc_skip_stmt_dl(),1);else{
if(zero_term)
s=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(a,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(elt_typ,Cyc_Absyn_uint_exp(0,0)),0),0);else{
# 2478
s=Cyc_Absyn_skip_stmt(0);}}{
# 2480
struct _RegionHandle _tmp454=_new_region("r");struct _RegionHandle*r=& _tmp454;_push_region(r);
{struct _tuple28*_tmpB80;struct Cyc_List_List*_tmpB7F;struct Cyc_List_List*decls=
(_tmpB7F=_region_malloc(r,sizeof(*_tmpB7F)),((_tmpB7F->hd=((_tmpB80=_region_malloc(r,sizeof(*_tmpB80)),((_tmpB80->f1=max,((_tmpB80->f2=Cyc_Absyn_uint_typ,((_tmpB80->f3=e1,_tmpB80)))))))),((_tmpB7F->tl=0,_tmpB7F)))));
struct Cyc_Absyn_Exp*ai;
if(rgnopt == 0  || Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*_tmpB81[2];ai=Cyc_Toc_malloc_exp(old_elt_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpB81[1]=_tmp453,((_tmpB81[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB81,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));}else{
# 2490
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);{
struct Cyc_Absyn_Exp*_tmpB82[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpB82[1]=_tmp453,((_tmpB82[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB82,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{
# 2496
struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);
{struct _tuple28*_tmpB85;struct Cyc_List_List*_tmpB84;decls=((_tmpB84=_region_malloc(r,sizeof(*_tmpB84)),((_tmpB84->hd=((_tmpB85=_region_malloc(r,sizeof(*_tmpB85)),((_tmpB85->f1=a,((_tmpB85->f2=ptr_typ,((_tmpB85->f3=ainit,_tmpB85)))))))),((_tmpB84->tl=decls,_tmpB84))))));}
if(!Cyc_Tcutil_is_pointer_type(old_typ)){
const char*_tmpB88;void*_tmpB87;(_tmpB87=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB88="new_comprehension_to_c: comprehension is not an array type",_tag_dyneither(_tmpB88,sizeof(char),59))),_tag_dyneither(_tmpB87,sizeof(void*),0)));}
if(Cyc_Tcutil_is_dyneither_ptr(old_typ)){
struct _tuple1*_tmp45B=Cyc_Toc_temp_var();
void*_tmp45C=Cyc_Toc_typ_to_c(old_typ);
struct Cyc_Absyn_Exp*_tmp45D=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpB89[3];struct Cyc_Absyn_Exp*_tmp45E=
Cyc_Absyn_fncall_exp(_tmp45D,(
(_tmpB89[2]=_tmp453,((_tmpB89[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((_tmpB89[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB89,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 2508
{struct _tuple28*_tmpB8C;struct Cyc_List_List*_tmpB8B;decls=((_tmpB8B=_region_malloc(r,sizeof(*_tmpB8B)),((_tmpB8B->hd=((_tmpB8C=_region_malloc(r,sizeof(*_tmpB8C)),((_tmpB8C->f1=_tmp45B,((_tmpB8C->f2=_tmp45C,((_tmpB8C->f3=_tmp45E,_tmpB8C)))))))),((_tmpB8B->tl=decls,_tmpB8B))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp45B,0),0),0);}else{
# 2511
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}
{struct Cyc_List_List*_tmp462=decls;for(0;_tmp462 != 0;_tmp462=_tmp462->tl){
struct _tuple28 _tmp463=*((struct _tuple28*)_tmp462->hd);struct _tuple1*_tmp465;void*_tmp466;struct Cyc_Absyn_Exp*_tmp467;struct _tuple28 _tmp464=_tmp463;_tmp465=_tmp464.f1;_tmp466=_tmp464.f2;_tmp467=_tmp464.f3;
s=Cyc_Absyn_declare_stmt(_tmp465,_tmp466,_tmp467,s,0);}}
# 2516
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2481
;_pop_region(r);};};}struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static void _tmpBDB(unsigned int*_tmpBDA,unsigned int*_tmpBD9,struct _tuple1***_tmpBD7){for(*_tmpBDA=0;*_tmpBDA < *_tmpBD9;(*_tmpBDA)++){(*_tmpBD7)[*_tmpBDA]=(struct _tuple1*)
# 2891 "toc.cyc"
Cyc_Toc_temp_var();}}
# 2520 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
void*_tmp46A=e->r;
if(e->topt == 0){
const char*_tmpB90;void*_tmpB8F[1];struct Cyc_String_pa_PrintArg_struct _tmpB8E;(_tmpB8E.tag=0,((_tmpB8E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpB8F[0]=& _tmpB8E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB90="exp_to_c: no type for %s",_tag_dyneither(_tmpB90,sizeof(char),25))),_tag_dyneither(_tmpB8F,sizeof(void*),1)))))));}
# 2526
if((nv->struct_info).lhs_exp != 0){
void*_tmp46E=_tmp46A;if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp46E)->tag == 28){_LL21E: _LL21F:
 goto _LL21D;}else{_LL220: _LL221:
 nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);}_LL21D:;}{
# 2534
void*old_typ=(void*)_check_null(e->topt);
void*_tmp46F=_tmp46A;struct Cyc_Absyn_Stmt*_tmp470;struct Cyc_Absyn_Exp*_tmp471;struct _dyneither_ptr*_tmp472;struct Cyc_Absyn_Exp*_tmp473;struct Cyc_Absyn_Exp*_tmp474;int _tmp475;struct Cyc_Absyn_Exp*_tmp476;void**_tmp477;struct Cyc_Absyn_Exp*_tmp478;int _tmp479;int _tmp47A;struct Cyc_List_List*_tmp47B;struct Cyc_Absyn_Datatypedecl*_tmp47C;struct Cyc_Absyn_Datatypefield*_tmp47D;void*_tmp47E;struct Cyc_List_List*_tmp47F;struct _tuple1*_tmp480;struct Cyc_List_List*_tmp481;struct Cyc_List_List*_tmp482;struct Cyc_Absyn_Aggrdecl*_tmp483;struct Cyc_Absyn_Exp*_tmp484;void*_tmp485;int _tmp486;struct Cyc_Absyn_Vardecl*_tmp487;struct Cyc_Absyn_Exp*_tmp488;struct Cyc_Absyn_Exp*_tmp489;int _tmp48A;struct Cyc_List_List*_tmp48B;struct Cyc_List_List*_tmp48C;struct Cyc_Absyn_Exp*_tmp48D;struct Cyc_Absyn_Exp*_tmp48E;struct Cyc_Absyn_Exp*_tmp48F;struct _dyneither_ptr*_tmp490;int _tmp491;int _tmp492;struct Cyc_Absyn_Exp*_tmp493;struct _dyneither_ptr*_tmp494;int _tmp495;int _tmp496;struct Cyc_Absyn_Exp*_tmp497;void*_tmp498;struct Cyc_List_List*_tmp499;void*_tmp49A;struct Cyc_Absyn_Exp*_tmp49B;struct Cyc_Absyn_Exp*_tmp49C;struct Cyc_Absyn_Exp*_tmp49D;struct Cyc_Absyn_Exp*_tmp49E;void**_tmp49F;struct Cyc_Absyn_Exp*_tmp4A0;int _tmp4A1;enum Cyc_Absyn_Coercion _tmp4A2;struct Cyc_Absyn_Exp*_tmp4A3;struct Cyc_List_List*_tmp4A4;struct Cyc_Absyn_Exp*_tmp4A5;struct Cyc_Absyn_Exp*_tmp4A6;int _tmp4A7;struct Cyc_Absyn_Exp*_tmp4A8;struct Cyc_List_List*_tmp4A9;int _tmp4AA;struct Cyc_List_List*_tmp4AB;struct Cyc_Absyn_VarargInfo*_tmp4AC;struct Cyc_Absyn_Exp*_tmp4AD;struct Cyc_List_List*_tmp4AE;struct Cyc_Absyn_Exp*_tmp4AF;struct Cyc_Absyn_Exp*_tmp4B0;struct Cyc_Absyn_Exp*_tmp4B1;struct Cyc_Absyn_Exp*_tmp4B2;struct Cyc_Absyn_Exp*_tmp4B3;struct Cyc_Absyn_Exp*_tmp4B4;struct Cyc_Absyn_Exp*_tmp4B5;struct Cyc_Absyn_Exp*_tmp4B6;struct Cyc_Absyn_Exp*_tmp4B7;struct Cyc_Absyn_Exp*_tmp4B8;struct Cyc_Core_Opt*_tmp4B9;struct Cyc_Absyn_Exp*_tmp4BA;struct Cyc_Absyn_Exp*_tmp4BB;enum Cyc_Absyn_Incrementor _tmp4BC;enum Cyc_Absyn_Primop _tmp4BD;struct Cyc_List_List*_tmp4BE;struct _tuple1*_tmp4BF;void*_tmp4C0;switch(*((int*)_tmp46F)){case 0: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp46F)->f1).Null_c).tag == 1){_LL223: _LL224: {
# 2541
struct Cyc_Absyn_Exp*_tmp4C1=Cyc_Absyn_uint_exp(0,0);
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp4C1,_tmp4C1))->r;else{
# 2546
struct Cyc_Absyn_Exp*_tmpB91[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((_tmpB91[2]=_tmp4C1,((_tmpB91[1]=_tmp4C1,((_tmpB91[0]=_tmp4C1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB91,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}else{
# 2548
e->r=(void*)& Cyc_Toc_zero_exp;}
# 2550
goto _LL222;}}else{_LL225: _LL226:
 goto _LL222;}case 1: _LL227: _tmp4BF=((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46F)->f1;_tmp4C0=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46F)->f2;_LL228:
# 2553
{struct _handler_cons _tmp4C3;_push_handler(& _tmp4C3);{int _tmp4C5=0;if(setjmp(_tmp4C3.handler))_tmp4C5=1;if(!_tmp4C5){e->r=(Cyc_Toc_lookup_varmap(nv,_tmp4BF))->r;;_pop_handler();}else{void*_tmp4C4=(void*)_exn_thrown;void*_tmp4C6=_tmp4C4;void*_tmp4C7;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp4C6)->tag == Cyc_Dict_Absent){_LL278: _LL279: {
# 2555
const char*_tmpB95;void*_tmpB94[1];struct Cyc_String_pa_PrintArg_struct _tmpB93;(_tmpB93.tag=0,((_tmpB93.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4BF)),((_tmpB94[0]=& _tmpB93,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB95="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpB95,sizeof(char),32))),_tag_dyneither(_tmpB94,sizeof(void*),1)))))));}}else{_LL27A: _tmp4C7=_tmp4C6;_LL27B:(int)_rethrow(_tmp4C7);}_LL277:;}};}
# 2557
goto _LL222;case 2: _LL229: _tmp4BD=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp46F)->f1;_tmp4BE=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp46F)->f2;_LL22A: {
# 2560
struct Cyc_List_List*_tmp4CB=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp4BE);
# 2562
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4BE);
{enum Cyc_Absyn_Primop _tmp4CC=_tmp4BD;switch(_tmp4CC){case Cyc_Absyn_Numelts: _LL27D: _LL27E: {
# 2565
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4BE))->hd;
{void*_tmp4CD=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_tmp4CE=_tmp4CD;void*_tmp4CF;union Cyc_Absyn_Constraint*_tmp4D0;union Cyc_Absyn_Constraint*_tmp4D1;union Cyc_Absyn_Constraint*_tmp4D2;struct Cyc_Absyn_Exp*_tmp4D3;switch(*((int*)_tmp4CE)){case 8: _LL292: _tmp4D3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4CE)->f1).num_elts;_LL293:
# 2569
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp4D3))){
const char*_tmpB98;void*_tmpB97;(_tmpB97=0,Cyc_Tcutil_terr(e->loc,((_tmpB98="can't calculate numelts",_tag_dyneither(_tmpB98,sizeof(char),24))),_tag_dyneither(_tmpB97,sizeof(void*),0)));}
e->r=_tmp4D3->r;goto _LL291;case 5: _LL294: _tmp4CF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CE)->f1).elt_typ;_tmp4D0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CE)->f1).ptr_atts).nullable;_tmp4D1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CE)->f1).ptr_atts).bounds;_tmp4D2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CE)->f1).ptr_atts).zero_term;_LL295:
# 2573
{void*_tmp4D6=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp4D1);void*_tmp4D7=_tmp4D6;struct Cyc_Absyn_Exp*_tmp4D8;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp4D7)->tag == 0){_LL299: _LL29A:
# 2575
{struct Cyc_Absyn_Exp*_tmpB99[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,(
(_tmpB99[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp4CF),0),((_tmpB99[0]=(struct Cyc_Absyn_Exp*)_tmp4BE->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB99,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL298;}else{_LL29B: _tmp4D8=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4D7)->f1;_LL29C:
# 2581
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4D2)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp4BE->hd);
# 2585
struct Cyc_Absyn_Exp*_tmpB9A[2];e->r=Cyc_Toc_fncall_exp_r(function_e,((_tmpB9A[1]=_tmp4D8,((_tmpB9A[0]=(struct Cyc_Absyn_Exp*)_tmp4BE->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB9A,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4D0)){
if(!Cyc_Evexp_c_can_eval(_tmp4D8)){
const char*_tmpB9D;void*_tmpB9C;(_tmpB9C=0,Cyc_Tcutil_terr(e->loc,((_tmpB9D="can't calculate numelts",_tag_dyneither(_tmpB9D,sizeof(char),24))),_tag_dyneither(_tmpB9C,sizeof(void*),0)));}
# 2590
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp4D8,Cyc_Absyn_uint_exp(0,0));}else{
# 2592
e->r=_tmp4D8->r;goto _LL298;}}
# 2594
goto _LL298;}_LL298:;}
# 2596
goto _LL291;default: _LL296: _LL297: {
# 2598
const char*_tmpBA2;void*_tmpBA1[2];struct Cyc_String_pa_PrintArg_struct _tmpBA0;struct Cyc_String_pa_PrintArg_struct _tmpB9F;(_tmpB9F.tag=0,((_tmpB9F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpBA0.tag=0,((_tmpBA0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpBA1[0]=& _tmpBA0,((_tmpBA1[1]=& _tmpB9F,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA2="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpBA2,sizeof(char),41))),_tag_dyneither(_tmpBA1,sizeof(void*),2)))))))))))));}}_LL291:;}
# 2601
goto _LL27C;}case Cyc_Absyn_Plus: _LL27F: _LL280:
# 2606
{void*_tmp4E1=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp4CB))->hd);void*_tmp4E2=_tmp4E1;void*_tmp4E3;union Cyc_Absyn_Constraint*_tmp4E4;union Cyc_Absyn_Constraint*_tmp4E5;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E2)->tag == 5){_LL29E: _tmp4E3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E2)->f1).elt_typ;_tmp4E4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E2)->f1).ptr_atts).bounds;_tmp4E5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E2)->f1).ptr_atts).zero_term;_LL29F:
# 2608
{void*_tmp4E6=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp4E4);void*_tmp4E7=_tmp4E6;struct Cyc_Absyn_Exp*_tmp4E8;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp4E7)->tag == 0){_LL2A3: _LL2A4: {
# 2610
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4BE))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4BE->tl))->hd;
{struct Cyc_Absyn_Exp*_tmpBA3[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpBA3[2]=e2,((_tmpBA3[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp4E3),0),((_tmpBA3[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBA3,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}
goto _LL2A2;}}else{_LL2A5: _tmp4E8=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4E7)->f1;_LL2A6:
# 2616
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4E5)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4BE))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4BE->tl))->hd;
struct Cyc_Absyn_Exp*_tmpBA4[3];e->r=(Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1),((_tmpBA4[2]=e2,((_tmpBA4[1]=_tmp4E8,((_tmpBA4[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBA4,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0))->r;}
# 2621
goto _LL2A2;}_LL2A2:;}
# 2623
goto _LL29D;}else{_LL2A0: _LL2A1:
# 2625
 goto _LL29D;}_LL29D:;}
# 2627
goto _LL27C;case Cyc_Absyn_Minus: _LL281: _LL282: {
# 2632
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(_tmp4CB))->hd,& elt_typ)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4BE))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4BE->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(_tmp4CB->tl))->hd)){
e1->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);
e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp);
e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{
# 2642
struct Cyc_Absyn_Exp*_tmpBA5[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpBA5[2]=
Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0),((_tmpBA5[1]=
# 2643
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpBA5[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBA5,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 2647
goto _LL27C;}case Cyc_Absyn_Eq: _LL283: _LL284:
 goto _LL286;case Cyc_Absyn_Neq: _LL285: _LL286:
 goto _LL288;case Cyc_Absyn_Gt: _LL287: _LL288:
 goto _LL28A;case Cyc_Absyn_Gte: _LL289: _LL28A:
 goto _LL28C;case Cyc_Absyn_Lt: _LL28B: _LL28C:
 goto _LL28E;case Cyc_Absyn_Lte: _LL28D: _LL28E: {
# 2655
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4BE))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4BE->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp4CB))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp4CB->tl))->hd;
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& elt_typ))
e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,0));
if(Cyc_Tcutil_is_tagged_pointer_typ(t2))
e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp,0));
goto _LL27C;}default: _LL28F: _LL290:
 goto _LL27C;}_LL27C:;}
# 2667
goto _LL222;}case 4: _LL22B: _tmp4BB=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp46F)->f1;_tmp4BC=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp46F)->f2;_LL22C: {
# 2669
void*e2_cyc_typ=(void*)_check_null(_tmp4BB->topt);
# 2678 "toc.cyc"
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
const char*_tmpBA6;struct _dyneither_ptr incr_str=(_tmpBA6="increment",_tag_dyneither(_tmpBA6,sizeof(char),10));
if(_tmp4BC == Cyc_Absyn_PreDec  || _tmp4BC == Cyc_Absyn_PostDec){const char*_tmpBA7;incr_str=((_tmpBA7="decrement",_tag_dyneither(_tmpBA7,sizeof(char),10)));}
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4BB,& ptr_type,& is_dyneither,& elt_type)){
{const char*_tmpBAB;void*_tmpBAA[1];struct Cyc_String_pa_PrintArg_struct _tmpBA9;(_tmpBA9.tag=0,((_tmpBA9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBAA[0]=& _tmpBA9,Cyc_Tcutil_terr(e->loc,((_tmpBAB="in-place %s is not supported when dereferencing a zero-terminated pointer",_tag_dyneither(_tmpBAB,sizeof(char),74))),_tag_dyneither(_tmpBAA,sizeof(void*),1)))))));}{
const char*_tmpBAE;void*_tmpBAD;(_tmpBAD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBAE="in-place inc/dec on zero-term",_tag_dyneither(_tmpBAE,sizeof(char),30))),_tag_dyneither(_tmpBAD,sizeof(void*),0)));};}{
# 2687
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
int f_tag=0;
if(Cyc_Toc_is_tagged_union_project(_tmp4BB,& f_tag,& tunion_member_type,1)){
struct Cyc_Absyn_Exp*_tmp4F2=Cyc_Absyn_signed_int_exp(1,0);
_tmp4F2->topt=Cyc_Absyn_sint_typ;{
enum Cyc_Absyn_Incrementor _tmp4F3=_tmp4BC;switch(_tmp4F3){case Cyc_Absyn_PreInc: _LL2A8: _LL2A9:
# 2694
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpBB4;struct Cyc_Core_Opt*_tmpBB3;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpBB2;e->r=(void*)((_tmpBB2=_cycalloc(sizeof(*_tmpBB2)),((_tmpBB2[0]=((_tmpBB4.tag=3,((_tmpBB4.f1=_tmp4BB,((_tmpBB4.f2=((_tmpBB3=_cycalloc_atomic(sizeof(*_tmpBB3)),((_tmpBB3->v=(void*)Cyc_Absyn_Plus,_tmpBB3)))),((_tmpBB4.f3=_tmp4F2,_tmpBB4)))))))),_tmpBB2))));}
Cyc_Toc_exp_to_c(nv,e);
return;case Cyc_Absyn_PreDec: _LL2AA: _LL2AB:
# 2698
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpBBA;struct Cyc_Core_Opt*_tmpBB9;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpBB8;e->r=(void*)((_tmpBB8=_cycalloc(sizeof(*_tmpBB8)),((_tmpBB8[0]=((_tmpBBA.tag=3,((_tmpBBA.f1=_tmp4BB,((_tmpBBA.f2=((_tmpBB9=_cycalloc_atomic(sizeof(*_tmpBB9)),((_tmpBB9->v=(void*)Cyc_Absyn_Minus,_tmpBB9)))),((_tmpBBA.f3=_tmp4F2,_tmpBBA)))))))),_tmpBB8))));}
Cyc_Toc_exp_to_c(nv,e);
return;default: _LL2AC: _LL2AD:
# 2702
{const char*_tmpBBE;void*_tmpBBD[1];struct Cyc_String_pa_PrintArg_struct _tmpBBC;(_tmpBBC.tag=0,((_tmpBBC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBBD[0]=& _tmpBBC,Cyc_Tcutil_terr(e->loc,((_tmpBBE="in-place post-%s is not supported on @tagged union members",_tag_dyneither(_tmpBBE,sizeof(char),59))),_tag_dyneither(_tmpBBD,sizeof(void*),1)))))));}{
# 2704
const char*_tmpBC1;void*_tmpBC0;(_tmpBC0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBC1="in-place inc/dec on @tagged union",_tag_dyneither(_tmpBC1,sizeof(char),34))),_tag_dyneither(_tmpBC0,sizeof(void*),0)));};}_LL2A7:;};}
# 2707
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4BB);
Cyc_Toc_set_lhs(nv,0);{
# 2712
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp4BC == Cyc_Absyn_PostInc  || _tmp4BC == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2718
if(_tmp4BC == Cyc_Absyn_PreDec  || _tmp4BC == Cyc_Absyn_PostDec)
change=-1;{
struct Cyc_Absyn_Exp*_tmpBC2[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpBC2[2]=
# 2722
Cyc_Absyn_signed_int_exp(change,0),((_tmpBC2[1]=
# 2721
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpBC2[0]=
# 2720
Cyc_Toc_push_address_exp(_tmp4BB),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBC2,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2723
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2727
struct Cyc_Toc_functionSet*_tmp500=_tmp4BC != Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
# 2729
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp500,_tmp4BB);
struct Cyc_Absyn_Exp*_tmpBC3[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpBC3[1]=
Cyc_Absyn_signed_int_exp(1,0),((_tmpBC3[0]=
# 2730
Cyc_Toc_push_address_exp(_tmp4BB),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBC3,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
# 2732
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp4BB)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp4BB,0,Cyc_Toc_incr_lvalue,_tmp4BC);
e->r=_tmp4BB->r;}}}
# 2736
goto _LL222;};};}case 3: _LL22D: _tmp4B8=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp46F)->f1;_tmp4B9=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp46F)->f2;_tmp4BA=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp46F)->f3;_LL22E: {
# 2755 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp4B8->topt);
void*e2_old_typ=(void*)_check_null(_tmp4BA->topt);
int f_tag=0;
void*tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Toc_is_tagged_union_project(_tmp4B8,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4B8);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4BA);
e->r=Cyc_Toc_tagged_union_assignop(_tmp4B8,e1_old_typ,_tmp4B9,_tmp4BA,e2_old_typ,f_tag,tagged_member_struct_type);
# 2766
return;}{
# 2768
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4B8,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp4B8,_tmp4B9,_tmp4BA,ptr_type,is_dyneither,elt_type);
# 2774
return;}{
# 2778
int e1_poly=Cyc_Toc_is_poly_project(_tmp4B8);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4B8);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4BA);{
# 2784
int done=0;
if(_tmp4B9 != 0){
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp503=(enum Cyc_Absyn_Primop)_tmp4B9->v;enum Cyc_Absyn_Primop _tmp504=_tmp503;switch(_tmp504){case Cyc_Absyn_Plus: _LL2AF: _LL2B0:
 change=_tmp4BA;goto _LL2AE;case Cyc_Absyn_Minus: _LL2B1: _LL2B2:
# 2792
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp4BA,0);goto _LL2AE;default: _LL2B3: _LL2B4: {
const char*_tmpBC6;void*_tmpBC5;(_tmpBC5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBC6="bad t ? pointer arithmetic",_tag_dyneither(_tmpBC6,sizeof(char),27))),_tag_dyneither(_tmpBC5,sizeof(void*),0)));}}_LL2AE:;}
# 2795
done=1;{
# 2797
struct Cyc_Absyn_Exp*_tmp507=Cyc_Toc__dyneither_ptr_inplace_plus_e;
struct Cyc_Absyn_Exp*_tmpBC7[3];e->r=Cyc_Toc_fncall_exp_r(_tmp507,((_tmpBC7[2]=change,((_tmpBC7[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpBC7[0]=
# 2798
Cyc_Toc_push_address_exp(_tmp4B8),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBC7,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2801
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2804
enum Cyc_Absyn_Primop _tmp509=(enum Cyc_Absyn_Primop)_tmp4B9->v;enum Cyc_Absyn_Primop _tmp50A=_tmp509;if(_tmp50A == Cyc_Absyn_Plus){_LL2B6: _LL2B7:
# 2806
 done=1;
{struct Cyc_Absyn_Exp*_tmpBC8[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp4B8),((_tmpBC8[1]=_tmp4BA,((_tmpBC8[0]=_tmp4B8,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBC8,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL2B5;}else{_LL2B8: _LL2B9: {
const char*_tmpBCB;void*_tmpBCA;(_tmpBCA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBCB="bad zero-terminated pointer arithmetic",_tag_dyneither(_tmpBCB,sizeof(char),39))),_tag_dyneither(_tmpBCA,sizeof(void*),0)));}}_LL2B5:;}}}
# 2813
if(!done){
# 2815
if(e1_poly)
_tmp4BA->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp4BA->r,0));
# 2821
if(!Cyc_Absyn_is_lvalue(_tmp4B8)){
{struct _tuple27 _tmpBCE;struct _tuple27*_tmpBCD;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple27*),struct _tuple27*f_env))Cyc_Toc_lvalue_assign)(_tmp4B8,0,Cyc_Toc_assignop_lvalue,((_tmpBCD=_cycalloc(sizeof(struct _tuple27)* 1),((_tmpBCD[0]=(struct _tuple27)((_tmpBCE.f1=_tmp4B9,((_tmpBCE.f2=_tmp4BA,_tmpBCE)))),_tmpBCD)))));}
e->r=_tmp4B8->r;}}
# 2826
goto _LL222;};};};}case 5: _LL22F: _tmp4B5=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp46F)->f1;_tmp4B6=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp46F)->f2;_tmp4B7=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp46F)->f3;_LL230:
# 2828
 Cyc_Toc_exp_to_c(nv,_tmp4B5);
Cyc_Toc_exp_to_c(nv,_tmp4B6);
Cyc_Toc_exp_to_c(nv,_tmp4B7);
goto _LL222;case 6: _LL231: _tmp4B3=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp46F)->f1;_tmp4B4=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp46F)->f2;_LL232:
# 2833
 Cyc_Toc_exp_to_c(nv,_tmp4B3);
Cyc_Toc_exp_to_c(nv,_tmp4B4);
goto _LL222;case 7: _LL233: _tmp4B1=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp46F)->f1;_tmp4B2=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp46F)->f2;_LL234:
# 2837
 Cyc_Toc_exp_to_c(nv,_tmp4B1);
Cyc_Toc_exp_to_c(nv,_tmp4B2);
goto _LL222;case 8: _LL235: _tmp4AF=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp46F)->f1;_tmp4B0=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp46F)->f2;_LL236:
# 2841
 Cyc_Toc_exp_to_c(nv,_tmp4AF);
Cyc_Toc_exp_to_c(nv,_tmp4B0);
goto _LL222;case 9: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46F)->f3 == 0){_LL237: _tmp4AD=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46F)->f1;_tmp4AE=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46F)->f2;_LL238:
# 2845
 Cyc_Toc_exp_to_c(nv,_tmp4AD);
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4AE);
goto _LL222;}else{_LL239: _tmp4A8=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46F)->f1;_tmp4A9=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46F)->f2;_tmp4AA=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46F)->f3)->num_varargs;_tmp4AB=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46F)->f3)->injectors;_tmp4AC=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46F)->f3)->vai;_LL23A: {
# 2857 "toc.cyc"
struct _RegionHandle _tmp510=_new_region("r");struct _RegionHandle*r=& _tmp510;_push_region(r);{
struct _tuple1*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp4AA,0);
void*cva_type=Cyc_Toc_typ_to_c(_tmp4AC->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_conref,0);
# 2865
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4A9);
int num_normargs=num_args - _tmp4AA;
# 2869
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp4A9=_tmp4A9->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4A9))->hd);{
struct Cyc_List_List*_tmpBCF;new_args=((_tmpBCF=_cycalloc(sizeof(*_tmpBCF)),((_tmpBCF->hd=(struct Cyc_Absyn_Exp*)_tmp4A9->hd,((_tmpBCF->tl=new_args,_tmpBCF))))));};}}
# 2874
{struct Cyc_Absyn_Exp*_tmpBD2[3];struct Cyc_List_List*_tmpBD1;new_args=((_tmpBD1=_cycalloc(sizeof(*_tmpBD1)),((_tmpBD1->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpBD2[2]=num_varargs_exp,((_tmpBD2[1]=
Cyc_Absyn_sizeoftyp_exp(cva_type,0),((_tmpBD2[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBD2,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpBD1->tl=new_args,_tmpBD1))))));}
# 2879
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);
# 2881
Cyc_Toc_exp_to_c(nv,_tmp4A8);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4A8,new_args,0),0);
# 2885
if(_tmp4AC->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp514=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp4AC->type));void*_tmp515=_tmp514;struct Cyc_Absyn_Datatypedecl*_tmp516;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp515)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp515)->f1).datatype_info).KnownDatatype).tag == 2){_LL2BB: _tmp516=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp515)->f1).datatype_info).KnownDatatype).val;_LL2BC:
 tud=_tmp516;goto _LL2BA;}else{goto _LL2BD;}}else{_LL2BD: _LL2BE: {
const char*_tmpBD5;void*_tmpBD4;(_tmpBD4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBD5="toc: unknown datatype in vararg with inject",_tag_dyneither(_tmpBD5,sizeof(char),44))),_tag_dyneither(_tmpBD4,sizeof(void*),0)));}}_LL2BA:;}{
# 2891
unsigned int _tmpBDA;unsigned int _tmpBD9;struct _dyneither_ptr _tmpBD8;struct _tuple1**_tmpBD7;unsigned int _tmpBD6;struct _dyneither_ptr vs=(_tmpBD6=(unsigned int)_tmp4AA,((_tmpBD7=(struct _tuple1**)_region_malloc(r,_check_times(sizeof(struct _tuple1*),_tmpBD6)),((_tmpBD8=_tag_dyneither(_tmpBD7,sizeof(struct _tuple1*),_tmpBD6),((((_tmpBD9=_tmpBD6,_tmpBDB(& _tmpBDA,& _tmpBD9,& _tmpBD7))),_tmpBD8)))))));
# 2893
if(_tmp4AA != 0){
# 2895
struct Cyc_List_List*_tmp519=0;
{int i=_tmp4AA - 1;for(0;i >= 0;-- i){
struct Cyc_List_List*_tmpBDC;_tmp519=((_tmpBDC=_cycalloc(sizeof(*_tmpBDC)),((_tmpBDC->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0),0)),((_tmpBDC->tl=_tmp519,_tmpBDC))))));}}
# 2899
s=Cyc_Absyn_declare_stmt(argv,arr_type,Cyc_Absyn_unresolvedmem_exp(0,_tmp519,0),s,0);{
# 2902
int i=0;for(0;_tmp4A9 != 0;(((_tmp4A9=_tmp4A9->tl,_tmp4AB=_tmp4AB->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp4A9->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0);
struct Cyc_Absyn_Datatypefield*_tmp51B=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp4AB))->hd;struct _tuple1*_tmp51D;struct Cyc_List_List*_tmp51E;struct Cyc_Absyn_Datatypefield*_tmp51C=_tmp51B;_tmp51D=_tmp51C->name;_tmp51E=_tmp51C->typs;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp51E))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);
# 2917
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);
# 2920
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Toc_tag_sp,0),
Cyc_Toc_datatype_tag(tud,_tmp51D),0),s,0);
# 2923
s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp51D,tud->name)),0,s,0);};}};}else{
# 2930
struct _tuple19*_tmpBDD[3];struct Cyc_List_List*_tmp51F=(_tmpBDD[2]=
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpBDD[1]=
# 2930
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpBDD[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBDD,sizeof(struct _tuple19*),3)))))));
# 2932
s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),Cyc_Absyn_uint_exp(0,0),s,0);}};}else{
# 2942
{int i=0;for(0;_tmp4A9 != 0;(_tmp4A9=_tmp4A9->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4A9->hd);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp4A9->hd,0),s,0);}}
# 2948
s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}
# 2961 "toc.cyc"
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2963
_npop_handler(0);goto _LL222;
# 2857 "toc.cyc"
;_pop_region(r);}}case 10: _LL23B: _tmp4A6=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp46F)->f1;_tmp4A7=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp46F)->f2;_LL23C:
# 2966 "toc.cyc"
 Cyc_Toc_exp_to_c(nv,_tmp4A6);{
struct Cyc_Absyn_Exp*_tmp525=_tmp4A7?Cyc_Toc_newrethrow_exp(_tmp4A6): Cyc_Toc_newthrow_exp(_tmp4A6);
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),_tmp525,0))->r;
goto _LL222;};case 11: _LL23D: _tmp4A5=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp46F)->f1;_LL23E:
 Cyc_Toc_exp_to_c(nv,_tmp4A5);goto _LL222;case 12: _LL23F: _tmp4A3=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp46F)->f1;_tmp4A4=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp46F)->f2;_LL240:
# 2972
 Cyc_Toc_exp_to_c(nv,_tmp4A3);
# 2981 "toc.cyc"
for(0;_tmp4A4 != 0;_tmp4A4=_tmp4A4->tl){
enum Cyc_Absyn_KindQual _tmp526=(Cyc_Tcutil_typ_kind((void*)_tmp4A4->hd))->kind;
if(_tmp526 != Cyc_Absyn_EffKind  && _tmp526 != Cyc_Absyn_RgnKind){
{void*_tmp527=Cyc_Tcutil_compress((void*)_tmp4A4->hd);void*_tmp528=_tmp527;switch(*((int*)_tmp528)){case 2: _LL2C0: _LL2C1:
 goto _LL2C3;case 3: _LL2C2: _LL2C3:
 continue;default: _LL2C4: _LL2C5:
# 2988
 e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp4A3,0))->r;
goto _LL2BF;}_LL2BF:;}
# 2991
break;}}
# 2994
goto _LL222;case 13: _LL241: _tmp49F=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp46F)->f1;_tmp4A0=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp46F)->f2;_tmp4A1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp46F)->f3;_tmp4A2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp46F)->f4;_LL242: {
# 2996
void*old_t2=(void*)_check_null(_tmp4A0->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp49F;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp49F=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp4A0);
# 3003
{struct _tuple29 _tmpBDE;struct _tuple29 _tmp529=(_tmpBDE.f1=Cyc_Tcutil_compress(old_t2),((_tmpBDE.f2=Cyc_Tcutil_compress(new_typ),_tmpBDE)));struct _tuple29 _tmp52A=_tmp529;struct Cyc_Absyn_PtrInfo _tmp52B;struct Cyc_Absyn_PtrInfo _tmp52C;struct Cyc_Absyn_PtrInfo _tmp52D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp52A.f1)->tag == 5)switch(*((int*)_tmp52A.f2)){case 5: _LL2C7: _tmp52C=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp52A.f1)->f1;_tmp52D=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp52A.f2)->f1;_LL2C8: {
# 3005
int _tmp52E=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp52C.ptr_atts).nullable);
int _tmp52F=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp52D.ptr_atts).nullable);
void*_tmp530=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp52C.ptr_atts).bounds);
void*_tmp531=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp52D.ptr_atts).bounds);
int _tmp532=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp52C.ptr_atts).zero_term);
int _tmp533=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp52D.ptr_atts).zero_term);
{struct _tuple29 _tmpBDF;struct _tuple29 _tmp534=(_tmpBDF.f1=_tmp530,((_tmpBDF.f2=_tmp531,_tmpBDF)));struct _tuple29 _tmp535=_tmp534;struct Cyc_Absyn_Exp*_tmp536;struct Cyc_Absyn_Exp*_tmp537;struct Cyc_Absyn_Exp*_tmp538;struct Cyc_Absyn_Exp*_tmp539;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp535.f1)->tag == 1){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp535.f2)->tag == 1){_LL2CE: _tmp538=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp535.f1)->f1;_tmp539=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp535.f2)->f1;_LL2CF:
# 3013
 if((!Cyc_Evexp_c_can_eval(_tmp538) || !Cyc_Evexp_c_can_eval(_tmp539)) && !
Cyc_Evexp_same_const_exp(_tmp538,_tmp539)){
const char*_tmpBE2;void*_tmpBE1;(_tmpBE1=0,Cyc_Tcutil_terr(e->loc,((_tmpBE2="can't validate cast due to potential size differences",_tag_dyneither(_tmpBE2,sizeof(char),54))),_tag_dyneither(_tmpBE1,sizeof(void*),0)));}
if(_tmp52E  && !_tmp52F){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpBE5;void*_tmpBE4;(_tmpBE4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBE5="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpBE5,sizeof(char),44))),_tag_dyneither(_tmpBE4,sizeof(void*),0)));}
# 3022
if(_tmp4A2 != Cyc_Absyn_NonNull_to_Null){
const char*_tmpBE9;void*_tmpBE8[1];struct Cyc_String_pa_PrintArg_struct _tmpBE7;(_tmpBE7.tag=0,((_tmpBE7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpBE8[0]=& _tmpBE7,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpBE9="null-check conversion mis-classified: %s",_tag_dyneither(_tmpBE9,sizeof(char),41))),_tag_dyneither(_tmpBE8,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp4A0);
if(do_null_check){
if(!_tmp4A1){
const char*_tmpBEC;void*_tmpBEB;(_tmpBEB=0,Cyc_Tcutil_warn(e->loc,((_tmpBEC="inserted null check due to implicit cast from * to @ type",_tag_dyneither(_tmpBEC,sizeof(char),58))),_tag_dyneither(_tmpBEB,sizeof(void*),0)));}{
# 3029
struct Cyc_List_List*_tmpBED;e->r=Cyc_Toc_cast_it_r(*_tmp49F,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpBED=_cycalloc(sizeof(*_tmpBED)),((_tmpBED->hd=_tmp4A0,((_tmpBED->tl=0,_tmpBED)))))),0));};}else{
# 3033
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4A0->r;}};}else{
# 3038
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4A0->r;}
# 3045
goto _LL2CD;}else{_LL2D0: _tmp537=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp535.f1)->f1;_LL2D1:
# 3047
 if(!Cyc_Evexp_c_can_eval(_tmp537)){
const char*_tmpBF0;void*_tmpBEF;(_tmpBEF=0,Cyc_Tcutil_terr(e->loc,((_tmpBF0="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpBF0,sizeof(char),71))),_tag_dyneither(_tmpBEF,sizeof(void*),0)));}
# 3050
if(_tmp4A2 == Cyc_Absyn_NonNull_to_Null){
const char*_tmpBF4;void*_tmpBF3[1];struct Cyc_String_pa_PrintArg_struct _tmpBF2;(_tmpBF2.tag=0,((_tmpBF2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpBF3[0]=& _tmpBF2,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpBF4="conversion mis-classified as null-check: %s",_tag_dyneither(_tmpBF4,sizeof(char),44))),_tag_dyneither(_tmpBF3,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){
# 3054
if((_tmp532  && !(_tmp52D.elt_tq).real_const) && !_tmp533)
# 3057
_tmp537=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp537,Cyc_Absyn_uint_exp(1,0),0);
# 3059
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp537,_tmp4A0))->r;}else{
# 3061
struct Cyc_Absyn_Exp*_tmp549=Cyc_Toc__tag_dyneither_e;
# 3063
if(_tmp532){
# 3068
struct _tuple1*_tmp54A=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp54B=Cyc_Absyn_var_exp(_tmp54A,0);
struct Cyc_Absyn_Exp*arg3;
# 3073
{void*_tmp54C=_tmp4A0->r;void*_tmp54D=_tmp54C;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp54D)->tag == 0)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp54D)->f1).Wstring_c).tag){case 8: _LL2D7: _LL2D8:
# 3075
 arg3=_tmp537;goto _LL2D6;case 9: _LL2D9: _LL2DA:
# 3077
 arg3=_tmp537;goto _LL2D6;default: goto _LL2DB;}else{_LL2DB: _LL2DC:
# 3079
{struct Cyc_Absyn_Exp*_tmpBF5[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp4A0),(
# 3081
(_tmpBF5[1]=_tmp537,((_tmpBF5[0]=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp54B),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBF5,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
goto _LL2D6;}_LL2D6:;}
# 3084
if(!_tmp533  && !(_tmp52D.elt_tq).real_const)
# 3087
arg3=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{
# 3089
struct Cyc_Absyn_Exp*_tmp54F=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp52D.elt_typ),0);
struct Cyc_Absyn_Exp*_tmpBF6[3];struct Cyc_Absyn_Exp*_tmp550=Cyc_Absyn_fncall_exp(_tmp549,((_tmpBF6[2]=arg3,((_tmpBF6[1]=_tmp54F,((_tmpBF6[0]=_tmp54B,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBF6,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
struct Cyc_Absyn_Stmt*_tmp551=Cyc_Absyn_exp_stmt(_tmp550,0);
_tmp551=Cyc_Absyn_declare_stmt(_tmp54A,Cyc_Toc_typ_to_c(old_t2),_tmp4A0,_tmp551,0);
e->r=Cyc_Toc_stmt_exp_r(_tmp551);};}else{
# 3096
struct Cyc_Absyn_Exp*_tmpBF7[3];e->r=Cyc_Toc_fncall_exp_r(_tmp549,(
(_tmpBF7[2]=_tmp537,((_tmpBF7[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp52D.elt_typ),0),((_tmpBF7[0]=_tmp4A0,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBF7,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 3102
goto _LL2CD;}}else{if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp535.f2)->tag == 1){_LL2D2: _tmp536=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp535.f2)->f1;_LL2D3:
# 3104
 if(!Cyc_Evexp_c_can_eval(_tmp536)){
const char*_tmpBFA;void*_tmpBF9;(_tmpBF9=0,Cyc_Tcutil_terr(e->loc,((_tmpBFA="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpBFA,sizeof(char),71))),_tag_dyneither(_tmpBF9,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpBFD;void*_tmpBFC;(_tmpBFC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBFD="can't coerce t? to t* or t@ at the top-level",_tag_dyneither(_tmpBFD,sizeof(char),45))),_tag_dyneither(_tmpBFC,sizeof(void*),0)));}{
# 3116 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp558=_tmp536;
if(_tmp532  && !_tmp533)
_tmp558=Cyc_Absyn_add_exp(_tmp536,Cyc_Absyn_uint_exp(1,0),0);{
# 3123
struct Cyc_Absyn_Exp*_tmp559=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpBFE[3];struct Cyc_Absyn_Exp*_tmp55A=Cyc_Absyn_fncall_exp(_tmp559,(
(_tmpBFE[2]=_tmp558,((_tmpBFE[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp52C.elt_typ),0),((_tmpBFE[0]=_tmp4A0,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBFE,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 3128
if(_tmp52F){
struct Cyc_List_List*_tmpBFF;_tmp55A->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,(
(_tmpBFF=_cycalloc(sizeof(*_tmpBFF)),((_tmpBFF->hd=Cyc_Absyn_copy_exp(_tmp55A),((_tmpBFF->tl=0,_tmpBFF)))))));}
e->r=Cyc_Toc_cast_it_r(*_tmp49F,_tmp55A);
goto _LL2CD;};};}else{_LL2D4: _LL2D5:
# 3136
 DynCast:
 if((_tmp532  && !_tmp533) && !(_tmp52D.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC02;void*_tmpC01;(_tmpC01=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC02="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",_tag_dyneither(_tmpC02,sizeof(char),70))),_tag_dyneither(_tmpC01,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp55F=Cyc_Toc__dyneither_ptr_decrease_size_e;
struct Cyc_Absyn_Exp*_tmpC03[3];e->r=Cyc_Toc_fncall_exp_r(_tmp55F,(
(_tmpC03[2]=
Cyc_Absyn_uint_exp(1,0),((_tmpC03[1]=
# 3142
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp52C.elt_typ),0),((_tmpC03[0]=_tmp4A0,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC03,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}
# 3145
goto _LL2CD;}}_LL2CD:;}
# 3147
goto _LL2C6;}case 6: _LL2C9: _tmp52B=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp52A.f1)->f1;_LL2CA:
# 3149
{void*_tmp562=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp52B.ptr_atts).bounds);void*_tmp563=_tmp562;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp563)->tag == 0){_LL2DE: _LL2DF:
# 3151
 _tmp4A0->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp4A0->r,_tmp4A0->loc),Cyc_Toc_curr_sp);goto _LL2DD;}else{_LL2E0: _LL2E1:
 goto _LL2DD;}_LL2DD:;}
# 3154
goto _LL2C6;default: goto _LL2CB;}else{_LL2CB: _LL2CC:
# 3156
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4A0->r;
goto _LL2C6;}_LL2C6:;}
# 3160
goto _LL222;}case 14: _LL243: _tmp49E=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp46F)->f1;_LL244:
# 3164
{void*_tmp565=_tmp49E->r;void*_tmp566=_tmp565;struct Cyc_List_List*_tmp567;struct _tuple1*_tmp568;struct Cyc_List_List*_tmp569;struct Cyc_List_List*_tmp56A;switch(*((int*)_tmp566)){case 28: _LL2E3: _tmp568=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp566)->f1;_tmp569=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp566)->f2;_tmp56A=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp566)->f3;_LL2E4:
# 3166
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC07;void*_tmpC06[1];struct Cyc_String_pa_PrintArg_struct _tmpC05;(_tmpC05.tag=0,((_tmpC05.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp49E->loc)),((_tmpC06[0]=& _tmpC05,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC07="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC07,sizeof(char),42))),_tag_dyneither(_tmpC06,sizeof(void*),1)))))));}{
struct Cyc_Absyn_Exp*_tmp56E=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp49E->topt),_tmp569,1,0,_tmp56A,_tmp568);
e->r=_tmp56E->r;
e->topt=_tmp56E->topt;
goto _LL2E2;};case 23: _LL2E5: _tmp567=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp566)->f1;_LL2E6:
# 3174
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC0B;void*_tmpC0A[1];struct Cyc_String_pa_PrintArg_struct _tmpC09;(_tmpC09.tag=0,((_tmpC09.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp49E->loc)),((_tmpC0A[0]=& _tmpC09,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC0B="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC0B,sizeof(char),42))),_tag_dyneither(_tmpC0A,sizeof(void*),1)))))));}
e->r=(Cyc_Toc_init_tuple(nv,1,0,_tmp567))->r;
goto _LL2E2;default: _LL2E7: _LL2E8:
# 3180
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp49E);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp49E)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp49E,0,Cyc_Toc_address_lvalue,1);
# 3186
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp49E);}else{
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind((void*)_check_null(_tmp49E->topt))))
# 3190
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp49E);}
# 3192
goto _LL2E2;}_LL2E2:;}
# 3194
goto _LL222;case 15: _LL245: _tmp49C=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp46F)->f1;_tmp49D=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp46F)->f2;_LL246:
# 3197
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC0F;void*_tmpC0E[1];struct Cyc_String_pa_PrintArg_struct _tmpC0D;(_tmpC0D.tag=0,((_tmpC0D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp49D->loc)),((_tmpC0E[0]=& _tmpC0D,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC0F="%s: new at top-level",_tag_dyneither(_tmpC0F,sizeof(char),21))),_tag_dyneither(_tmpC0E,sizeof(void*),1)))))));}{
void*new_e_type=(void*)_check_null(_tmp49D->topt);
{void*_tmp575=_tmp49D->r;void*_tmp576=_tmp575;struct Cyc_List_List*_tmp577;struct _tuple1*_tmp578;struct Cyc_List_List*_tmp579;struct Cyc_List_List*_tmp57A;struct Cyc_Absyn_Aggrdecl*_tmp57B;struct Cyc_Absyn_Exp*_tmp57C;void*_tmp57D;int _tmp57E;struct Cyc_Absyn_Vardecl*_tmp57F;struct Cyc_Absyn_Exp*_tmp580;struct Cyc_Absyn_Exp*_tmp581;int _tmp582;struct Cyc_List_List*_tmp583;switch(*((int*)_tmp576)){case 25: _LL2EA: _tmp583=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp576)->f1;_LL2EB: {
# 3205
struct _tuple1*_tmp584=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp585=Cyc_Absyn_var_exp(_tmp584,0);
struct Cyc_Absyn_Stmt*_tmp586=Cyc_Toc_init_array(nv,new_e_type,_tmp585,_tmp583,Cyc_Absyn_exp_stmt(_tmp585,0));
void*old_elt_typ;
{void*_tmp587=Cyc_Tcutil_compress(old_typ);void*_tmp588=_tmp587;void*_tmp589;struct Cyc_Absyn_Tqual _tmp58A;union Cyc_Absyn_Constraint*_tmp58B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp588)->tag == 5){_LL2F7: _tmp589=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp588)->f1).elt_typ;_tmp58A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp588)->f1).elt_tq;_tmp58B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp588)->f1).ptr_atts).zero_term;_LL2F8:
# 3211
 old_elt_typ=_tmp589;goto _LL2F6;}else{_LL2F9: _LL2FA: {
# 3213
const char*_tmpC12;void*_tmpC11;old_elt_typ=(
(_tmpC11=0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC12="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(_tmpC12,sizeof(char),52))),_tag_dyneither(_tmpC11,sizeof(void*),0))));}}_LL2F6:;}{
# 3216
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*_tmp58E=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp58F=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),
Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp583),0),0);
struct Cyc_Absyn_Exp*e1;
if(_tmp49C == 0  || Cyc_Absyn_no_regions)
e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp58F);else{
# 3224
struct Cyc_Absyn_Exp*r=_tmp49C;
Cyc_Toc_exp_to_c(nv,r);
e1=Cyc_Toc_rmalloc_exp(r,_tmp58F);}
# 3229
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp584,_tmp58E,e1,_tmp586,0));
goto _LL2E9;};}case 26: _LL2EC: _tmp57F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp576)->f1;_tmp580=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp576)->f2;_tmp581=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp576)->f3;_tmp582=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp576)->f4;_LL2ED:
# 3233
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp49C,old_typ,_tmp580,(void*)_check_null(_tmp581->topt),_tmp582,_tmp581,_tmp57F);
goto _LL2E9;case 27: _LL2EE: _tmp57C=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp576)->f1;_tmp57D=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp576)->f2;_tmp57E=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp576)->f3;_LL2EF:
# 3237
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp49C,old_typ,_tmp57C,_tmp57D,_tmp57E,0,0);
goto _LL2E9;case 28: _LL2F0: _tmp578=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp576)->f1;_tmp579=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp576)->f2;_tmp57A=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp576)->f3;_tmp57B=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp576)->f4;_LL2F1: {
# 3242
struct Cyc_Absyn_Exp*_tmp590=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp49D->topt),_tmp579,1,_tmp49C,_tmp57A,_tmp578);
e->r=_tmp590->r;
e->topt=_tmp590->topt;
goto _LL2E9;}case 23: _LL2F2: _tmp577=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp576)->f1;_LL2F3:
# 3248
 e->r=(Cyc_Toc_init_tuple(nv,1,_tmp49C,_tmp577))->r;
goto _LL2E9;default: _LL2F4: _LL2F5: {
# 3255
void*old_elt_typ=(void*)_check_null(_tmp49D->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
# 3258
struct _tuple1*_tmp591=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp592=Cyc_Absyn_var_exp(_tmp591,0);
struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp592,0),0);
struct Cyc_Absyn_Exp*inner_mexp=mexp;
if(_tmp49C == 0  || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
# 3265
struct Cyc_Absyn_Exp*r=_tmp49C;
Cyc_Toc_exp_to_c(nv,r);
mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{
# 3273
int done=0;
{void*_tmp593=_tmp49D->r;void*_tmp594=_tmp593;void*_tmp595;struct Cyc_Absyn_Exp*_tmp596;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp594)->tag == 13){_LL2FC: _tmp595=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp594)->f1;_tmp596=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp594)->f2;_LL2FD:
# 3276
{struct _tuple29 _tmpC13;struct _tuple29 _tmp597=(_tmpC13.f1=Cyc_Tcutil_compress(_tmp595),((_tmpC13.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp596->topt)),_tmpC13)));struct _tuple29 _tmp598=_tmp597;void*_tmp599;union Cyc_Absyn_Constraint*_tmp59A;union Cyc_Absyn_Constraint*_tmp59B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp598.f1)->tag == 5){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp598.f2)->tag == 5){_LL301: _tmp599=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp598.f1)->f1).elt_typ;_tmp59A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp598.f1)->f1).ptr_atts).bounds;_tmp59B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp598.f2)->f1).ptr_atts).bounds;_LL302:
# 3279
{struct _tuple29 _tmpC14;struct _tuple29 _tmp59C=(_tmpC14.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp59A),((_tmpC14.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp59B),_tmpC14)));struct _tuple29 _tmp59D=_tmp59C;struct Cyc_Absyn_Exp*_tmp59E;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp59D.f1)->tag == 0){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp59D.f2)->tag == 1){_LL306: _tmp59E=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp59D.f2)->f1;_LL307:
# 3281
 Cyc_Toc_exp_to_c(nv,_tmp596);
inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);
done=1;{
struct Cyc_Absyn_Exp*_tmp59F=Cyc_Toc__init_dyneither_ptr_e;
{struct Cyc_Absyn_Exp*_tmpC15[4];e->r=Cyc_Toc_fncall_exp_r(_tmp59F,(
(_tmpC15[3]=_tmp59E,((_tmpC15[2]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp599),0),((_tmpC15[1]=_tmp596,((_tmpC15[0]=mexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC15,sizeof(struct Cyc_Absyn_Exp*),4)))))))))));}
# 3289
goto _LL305;};}else{goto _LL308;}}else{_LL308: _LL309:
 goto _LL305;}_LL305:;}
# 3292
goto _LL300;}else{goto _LL303;}}else{_LL303: _LL304:
 goto _LL300;}_LL300:;}
# 3295
goto _LL2FB;}else{_LL2FE: _LL2FF:
 goto _LL2FB;}_LL2FB:;}
# 3298
if(!done){
struct Cyc_Absyn_Stmt*_tmp5A3=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp592),0);
struct Cyc_Absyn_Exp*_tmp5A4=Cyc_Absyn_signed_int_exp(0,0);
Cyc_Toc_exp_to_c(nv,_tmp49D);
_tmp5A3=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp592,_tmp5A4,0),_tmp49D,0),_tmp5A3,0);{
# 3304
void*_tmp5A5=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp591,_tmp5A5,mexp,_tmp5A3,0));};}
# 3307
goto _LL2E9;};}}_LL2E9:;}
# 3309
goto _LL222;};case 17: _LL247: _tmp49B=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp46F)->f1;_LL248: {
# 3312
int _tmp5A6=Cyc_Toc_set_in_sizeof(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp49B);
Cyc_Toc_set_in_sizeof(nv,_tmp5A6);
goto _LL222;}case 16: _LL249: _tmp49A=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp46F)->f1;_LL24A:
{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpC18;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpC17;e->r=(void*)((_tmpC17=_cycalloc(sizeof(*_tmpC17)),((_tmpC17[0]=((_tmpC18.tag=16,((_tmpC18.f1=Cyc_Toc_typ_to_c(_tmp49A),_tmpC18)))),_tmpC17))));}goto _LL222;case 18: _LL24B: _tmp498=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp46F)->f1;_tmp499=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp46F)->f2;_LL24C: {
# 3318
void*_tmp5A9=_tmp498;
struct Cyc_List_List*_tmp5AA=_tmp499;
for(0;_tmp5AA != 0;_tmp5AA=_tmp5AA->tl){
void*_tmp5AB=(void*)_tmp5AA->hd;void*_tmp5AC=_tmp5AB;unsigned int _tmp5AD;struct _dyneither_ptr*_tmp5AE;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp5AC)->tag == 0){_LL30B: _tmp5AE=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp5AC)->f1;_LL30C:
 goto _LL30A;}else{_LL30D: _tmp5AD=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp5AC)->f1;_LL30E:
# 3324
{void*_tmp5AF=Cyc_Tcutil_compress(_tmp5A9);void*_tmp5B0=_tmp5AF;struct Cyc_Absyn_Datatypefield*_tmp5B1;struct Cyc_List_List*_tmp5B2;struct Cyc_List_List*_tmp5B3;union Cyc_Absyn_AggrInfoU _tmp5B4;switch(*((int*)_tmp5B0)){case 11: _LL310: _tmp5B4=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5B0)->f1).aggr_info;_LL311: {
# 3326
struct Cyc_Absyn_Aggrdecl*_tmp5B5=Cyc_Absyn_get_known_aggrdecl(_tmp5B4);
if(_tmp5B5->impl == 0){
const char*_tmpC1B;void*_tmpC1A;(_tmpC1A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC1B="struct fields must be known",_tag_dyneither(_tmpC1B,sizeof(char),28))),_tag_dyneither(_tmpC1A,sizeof(void*),0)));}
_tmp5B3=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp5B5->impl))->fields;goto _LL313;}case 12: _LL312: _tmp5B3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5B0)->f2;_LL313: {
# 3331
struct Cyc_Absyn_Aggrfield*_tmp5B8=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp5B3,(int)_tmp5AD);
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpC1E;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpC1D;_tmp5AA->hd=(void*)((void*)((_tmpC1D=_cycalloc(sizeof(*_tmpC1D)),((_tmpC1D[0]=((_tmpC1E.tag=0,((_tmpC1E.f1=_tmp5B8->name,_tmpC1E)))),_tmpC1D)))));}
_tmp5A9=_tmp5B8->type;
goto _LL30F;}case 10: _LL314: _tmp5B2=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5B0)->f1;_LL315:
# 3336
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpC21;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpC20;_tmp5AA->hd=(void*)((void*)((_tmpC20=_cycalloc(sizeof(*_tmpC20)),((_tmpC20[0]=((_tmpC21.tag=0,((_tmpC21.f1=Cyc_Absyn_fieldname((int)(_tmp5AD + 1)),_tmpC21)))),_tmpC20)))));}
_tmp5A9=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp5B2,(int)_tmp5AD)).f2;
goto _LL30F;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp5B0)->f1).field_info).KnownDatatypefield).tag == 2){_LL316: _tmp5B1=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp5B0)->f1).field_info).KnownDatatypefield).val).f2;_LL317:
# 3340
 if(_tmp5AD == 0){
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpC24;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpC23;_tmp5AA->hd=(void*)((void*)((_tmpC23=_cycalloc(sizeof(*_tmpC23)),((_tmpC23[0]=((_tmpC24.tag=0,((_tmpC24.f1=Cyc_Toc_tag_sp,_tmpC24)))),_tmpC23)))));}else{
# 3343
_tmp5A9=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp5B1->typs,(int)(_tmp5AD - 1))).f2;{
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpC27;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpC26;_tmp5AA->hd=(void*)((void*)((_tmpC26=_cycalloc(sizeof(*_tmpC26)),((_tmpC26[0]=((_tmpC27.tag=0,((_tmpC27.f1=Cyc_Absyn_fieldname((int)_tmp5AD),_tmpC27)))),_tmpC26)))));};}
# 3346
goto _LL30F;}else{goto _LL318;}default: _LL318: _LL319:
{const char*_tmpC2A;void*_tmpC29;(_tmpC29=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC2A="impossible type for offsetof tuple index",_tag_dyneither(_tmpC2A,sizeof(char),41))),_tag_dyneither(_tmpC29,sizeof(void*),0)));}
goto _LL30F;}_LL30F:;}
# 3350
goto _LL30A;}_LL30A:;}
# 3353
{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpC2D;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpC2C;e->r=(void*)((_tmpC2C=_cycalloc(sizeof(*_tmpC2C)),((_tmpC2C[0]=((_tmpC2D.tag=18,((_tmpC2D.f1=Cyc_Toc_typ_to_c(_tmp498),((_tmpC2D.f2=_tmp499,_tmpC2D)))))),_tmpC2C))));}
goto _LL222;}case 19: _LL24D: _tmp497=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp46F)->f1;_LL24E: {
# 3356
int _tmp5C5=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp5C6=Cyc_Tcutil_compress((void*)_check_null(_tmp497->topt));
{void*_tmp5C7=_tmp5C6;void*_tmp5C8;struct Cyc_Absyn_Tqual _tmp5C9;void*_tmp5CA;union Cyc_Absyn_Constraint*_tmp5CB;union Cyc_Absyn_Constraint*_tmp5CC;union Cyc_Absyn_Constraint*_tmp5CD;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C7)->tag == 5){_LL31B: _tmp5C8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C7)->f1).elt_typ;_tmp5C9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C7)->f1).elt_tq;_tmp5CA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C7)->f1).ptr_atts).rgn;_tmp5CB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C7)->f1).ptr_atts).nullable;_tmp5CC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C7)->f1).ptr_atts).bounds;_tmp5CD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C7)->f1).ptr_atts).zero_term;_LL31C:
# 3361
{void*_tmp5CE=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5CC);void*_tmp5CF=_tmp5CE;struct Cyc_Absyn_Exp*_tmp5D0;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5CF)->tag == 1){_LL320: _tmp5D0=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5CF)->f1;_LL321: {
# 3363
int do_null_check=!Cyc_Toc_in_sizeof(nv) && Cyc_Toc_need_null_check(_tmp497);
Cyc_Toc_exp_to_c(nv,_tmp497);
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC30;void*_tmpC2F;(_tmpC2F=0,Cyc_Tcutil_warn(e->loc,((_tmpC30="inserted null check due to dereference",_tag_dyneither(_tmpC30,sizeof(char),39))),_tag_dyneither(_tmpC2F,sizeof(void*),0)));}{
# 3369
struct Cyc_List_List*_tmpC31;_tmp497->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(_tmp5C6),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpC31=_cycalloc(sizeof(*_tmpC31)),((_tmpC31->hd=Cyc_Absyn_copy_exp(_tmp497),((_tmpC31->tl=0,_tmpC31)))))),0));};}
# 3377
if(!Cyc_Toc_in_sizeof(nv) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5CD)){
struct _tuple12 _tmp5D4=Cyc_Evexp_eval_const_uint_exp(_tmp5D0);unsigned int _tmp5D6;int _tmp5D7;struct _tuple12 _tmp5D5=_tmp5D4;_tmp5D6=_tmp5D5.f1;_tmp5D7=_tmp5D5.f2;
# 3383
if(!_tmp5D7  || _tmp5D6 <= 0){
const char*_tmpC34;void*_tmpC33;(_tmpC33=0,Cyc_Tcutil_terr(e->loc,((_tmpC34="cannot determine dereference is in bounds",_tag_dyneither(_tmpC34,sizeof(char),42))),_tag_dyneither(_tmpC33,sizeof(void*),0)));}}
# 3386
goto _LL31F;}}else{_LL322: _LL323: {
# 3389
struct Cyc_Absyn_Exp*_tmp5DA=Cyc_Absyn_uint_exp(0,0);
_tmp5DA->topt=Cyc_Absyn_uint_typ;
e->r=Cyc_Toc_subscript_exp_r(_tmp497,_tmp5DA);
Cyc_Toc_exp_to_c(nv,e);
goto _LL31F;}}_LL31F:;}
# 3395
goto _LL31A;}else{_LL31D: _LL31E: {
const char*_tmpC37;void*_tmpC36;(_tmpC36=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC37="exp_to_c: Deref: non-pointer",_tag_dyneither(_tmpC37,sizeof(char),29))),_tag_dyneither(_tmpC36,sizeof(void*),0)));}}_LL31A:;}
# 3398
Cyc_Toc_set_lhs(nv,_tmp5C5);
goto _LL222;};}case 20: _LL24F: _tmp493=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp46F)->f1;_tmp494=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp46F)->f2;_tmp495=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp46F)->f3;_tmp496=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp46F)->f4;_LL250: {
# 3401
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp493->topt);
Cyc_Toc_exp_to_c(nv,_tmp493);
if(_tmp495  && _tmp496)
e->r=Cyc_Toc_check_tagged_union(_tmp493,Cyc_Toc_typ_to_c(e1_cyc_type),e1_cyc_type,_tmp494,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);
# 3409
if(is_poly)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
goto _LL222;}case 21: _LL251: _tmp48F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp46F)->f1;_tmp490=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp46F)->f2;_tmp491=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp46F)->f3;_tmp492=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp46F)->f4;_LL252: {
# 3413
int _tmp5DD=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp48F->topt));
int do_null_check=Cyc_Toc_need_null_check(_tmp48F);
Cyc_Toc_exp_to_c(nv,_tmp48F);{
int is_poly=Cyc_Toc_is_poly_project(e);
struct Cyc_Absyn_PtrInfo _tmp5DE=Cyc_Toc_get_ptr_type(e1typ);void*_tmp5E0;struct Cyc_Absyn_Tqual _tmp5E1;void*_tmp5E2;union Cyc_Absyn_Constraint*_tmp5E3;union Cyc_Absyn_Constraint*_tmp5E4;union Cyc_Absyn_Constraint*_tmp5E5;struct Cyc_Absyn_PtrInfo _tmp5DF=_tmp5DE;_tmp5E0=_tmp5DF.elt_typ;_tmp5E1=_tmp5DF.elt_tq;_tmp5E2=(_tmp5DF.ptr_atts).rgn;_tmp5E3=(_tmp5DF.ptr_atts).nullable;_tmp5E4=(_tmp5DF.ptr_atts).bounds;_tmp5E5=(_tmp5DF.ptr_atts).zero_term;
{void*_tmp5E6=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5E4);void*_tmp5E7=_tmp5E6;struct Cyc_Absyn_Exp*_tmp5E8;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5E7)->tag == 1){_LL325: _tmp5E8=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5E7)->f1;_LL326: {
# 3424
struct _tuple12 _tmp5E9=Cyc_Evexp_eval_const_uint_exp(_tmp5E8);unsigned int _tmp5EB;int _tmp5EC;struct _tuple12 _tmp5EA=_tmp5E9;_tmp5EB=_tmp5EA.f1;_tmp5EC=_tmp5EA.f2;
if(_tmp5EC){
if(_tmp5EB < 1){
const char*_tmpC3A;void*_tmpC39;(_tmpC39=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC3A="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpC3A,sizeof(char),44))),_tag_dyneither(_tmpC39,sizeof(void*),0)));}
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC3D;void*_tmpC3C;(_tmpC3C=0,Cyc_Tcutil_warn(e->loc,((_tmpC3D="inserted null check due to dereference",_tag_dyneither(_tmpC3D,sizeof(char),39))),_tag_dyneither(_tmpC3C,sizeof(void*),0)));}{
# 3432
struct Cyc_Absyn_Exp*_tmpC3E[1];_tmp48F->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpC3E[0]=Cyc_Absyn_new_exp(_tmp48F->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC3E,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}}else{
# 3437
if(!Cyc_Evexp_c_can_eval(_tmp5E8)){
const char*_tmpC41;void*_tmpC40;(_tmpC40=0,Cyc_Tcutil_terr(e->loc,((_tmpC41="cannot determine pointer dereference in bounds",_tag_dyneither(_tmpC41,sizeof(char),47))),_tag_dyneither(_tmpC40,sizeof(void*),0)));}{
# 3441
struct Cyc_Absyn_Exp*_tmpC42[4];_tmp48F->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpC42[3]=
# 3445
Cyc_Absyn_uint_exp(0,0),((_tmpC42[2]=
# 3444
Cyc_Absyn_sizeoftyp_exp(_tmp5E0,0),((_tmpC42[1]=_tmp5E8,((_tmpC42[0]=
# 3443
Cyc_Absyn_new_exp(_tmp48F->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC42,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0));};}
# 3447
goto _LL324;}}else{_LL327: _LL328: {
# 3450
void*ta1=Cyc_Toc_typ_to_c(_tmp5E0);
{struct Cyc_Absyn_Exp*_tmpC43[3];_tmp48F->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp5E1),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_dyneither_subscript_e,(
(_tmpC43[2]=
# 3455
Cyc_Absyn_uint_exp(0,0),((_tmpC43[1]=
# 3454
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC43[0]=
# 3453
Cyc_Absyn_new_exp(_tmp48F->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC43,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}
# 3458
goto _LL324;}}_LL324:;}
# 3460
if(_tmp491  && _tmp492)
e->r=Cyc_Toc_check_tagged_union(_tmp48F,Cyc_Toc_typ_to_c(e1typ),_tmp5E0,_tmp490,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);
# 3463
if(is_poly  && _tmp492)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
Cyc_Toc_set_lhs(nv,_tmp5DD);
goto _LL222;};};}case 22: _LL253: _tmp48D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp46F)->f1;_tmp48E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp46F)->f2;_LL254: {
# 3468
int _tmp5F6=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp5F7=Cyc_Tcutil_compress((void*)_check_null(_tmp48D->topt));
# 3472
{void*_tmp5F8=_tmp5F7;void*_tmp5F9;struct Cyc_Absyn_Tqual _tmp5FA;void*_tmp5FB;union Cyc_Absyn_Constraint*_tmp5FC;union Cyc_Absyn_Constraint*_tmp5FD;union Cyc_Absyn_Constraint*_tmp5FE;struct Cyc_List_List*_tmp5FF;switch(*((int*)_tmp5F8)){case 10: _LL32A: _tmp5FF=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5F8)->f1;_LL32B:
# 3475
 Cyc_Toc_exp_to_c(nv,_tmp48D);
Cyc_Toc_exp_to_c(nv,_tmp48E);{
struct _tuple12 _tmp600=Cyc_Evexp_eval_const_uint_exp(_tmp48E);unsigned int _tmp602;int _tmp603;struct _tuple12 _tmp601=_tmp600;_tmp602=_tmp601.f1;_tmp603=_tmp601.f2;
if(!_tmp603){
const char*_tmpC46;void*_tmpC45;(_tmpC45=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC46="unknown tuple subscript in translation to C",_tag_dyneither(_tmpC46,sizeof(char),44))),_tag_dyneither(_tmpC45,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp48D,Cyc_Absyn_fieldname((int)(_tmp602 + 1)));
goto _LL329;};case 5: _LL32C: _tmp5F9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F8)->f1).elt_typ;_tmp5FA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F8)->f1).elt_tq;_tmp5FB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F8)->f1).ptr_atts).rgn;_tmp5FC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F8)->f1).ptr_atts).nullable;_tmp5FD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F8)->f1).ptr_atts).bounds;_tmp5FE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F8)->f1).ptr_atts).zero_term;_LL32D: {
# 3483
struct Cyc_List_List*_tmp606=Cyc_Toc_get_relns(_tmp48D);
int _tmp607=Cyc_Toc_need_null_check(_tmp48D);
int _tmp608=Cyc_Toc_in_sizeof(nv);
# 3491
int in_bnds=_tmp608;
{void*_tmp609=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5FD);void*_tmp60A=_tmp609;_LL331: _LL332:
# 3494
 in_bnds=in_bnds  || Cyc_Toc_check_bounds(_tmp5F7,_tmp606,_tmp48D,_tmp48E);
if(Cyc_Toc_warn_bounds_checks  && !in_bnds){
const char*_tmpC4A;void*_tmpC49[1];struct Cyc_String_pa_PrintArg_struct _tmpC48;(_tmpC48.tag=0,((_tmpC48.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC49[0]=& _tmpC48,Cyc_Tcutil_warn(e->loc,((_tmpC4A="bounds check necessary for %s",_tag_dyneither(_tmpC4A,sizeof(char),30))),_tag_dyneither(_tmpC49,sizeof(void*),1)))))));}_LL330:;}
# 3501
Cyc_Toc_exp_to_c(nv,_tmp48D);
Cyc_Toc_exp_to_c(nv,_tmp48E);
++ Cyc_Toc_total_bounds_checks;
{void*_tmp60E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5FD);void*_tmp60F=_tmp60E;struct Cyc_Absyn_Exp*_tmp610;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp60F)->tag == 1){_LL334: _tmp610=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp60F)->f1;_LL335: {
# 3506
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5FC) && _tmp607;
void*ta1=Cyc_Toc_typ_to_c(_tmp5F9);
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp5FA);
if(in_bnds  && !possibly_null)
++ Cyc_Toc_bounds_checks_eliminated;else{
if(in_bnds  && possibly_null){
++ Cyc_Toc_bounds_checks_eliminated;
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC4D;void*_tmpC4C;(_tmpC4C=0,Cyc_Tcutil_warn(e->loc,((_tmpC4D="inserted null check due to dereference",_tag_dyneither(_tmpC4D,sizeof(char),39))),_tag_dyneither(_tmpC4C,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpC4E[1];_tmp48D->r=Cyc_Toc_cast_it_r(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpC4E[0]=Cyc_Absyn_copy_exp(_tmp48D),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC4E,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}else{
# 3518
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5FE)){
# 3520
if(!Cyc_Evexp_c_can_eval(_tmp610)){
const char*_tmpC51;void*_tmpC50;(_tmpC50=0,Cyc_Tcutil_terr(e->loc,((_tmpC51="cannot determine subscript is in bounds",_tag_dyneither(_tmpC51,sizeof(char),40))),_tag_dyneither(_tmpC50,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp48D);
struct Cyc_Absyn_Exp*_tmpC52[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(function_e,(
(_tmpC52[2]=_tmp48E,((_tmpC52[1]=_tmp610,((_tmpC52[0]=_tmp48D,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC52,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp610)){
const char*_tmpC55;void*_tmpC54;(_tmpC54=0,Cyc_Tcutil_terr(e->loc,((_tmpC55="cannot determine subscript is in bounds",_tag_dyneither(_tmpC55,sizeof(char),40))),_tag_dyneither(_tmpC54,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC58;void*_tmpC57;(_tmpC57=0,Cyc_Tcutil_warn(e->loc,((_tmpC58="inserted null check due to dereference",_tag_dyneither(_tmpC58,sizeof(char),39))),_tag_dyneither(_tmpC57,sizeof(void*),0)));}{
# 3532
struct Cyc_Absyn_Exp*_tmpC59[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpC59[3]=_tmp48E,((_tmpC59[2]=
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC59[1]=_tmp610,((_tmpC59[0]=_tmp48D,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC59,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{
# 3538
if(!Cyc_Evexp_c_can_eval(_tmp610)){
const char*_tmpC5C;void*_tmpC5B;(_tmpC5B=0,Cyc_Tcutil_terr(e->loc,((_tmpC5C="cannot determine subscript is in bounds",_tag_dyneither(_tmpC5C,sizeof(char),40))),_tag_dyneither(_tmpC5B,sizeof(void*),0)));}{
# 3541
struct Cyc_Absyn_Exp*_tmpC5D[2];_tmp48E->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,(
(_tmpC5D[1]=Cyc_Absyn_copy_exp(_tmp48E),((_tmpC5D[0]=_tmp610,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC5D,sizeof(struct Cyc_Absyn_Exp*),2)))))));};}}}}
# 3544
goto _LL333;}}else{_LL336: _LL337: {
# 3546
void*ta1=Cyc_Toc_typ_to_c(_tmp5F9);
if(in_bnds){
# 3550
++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp5FA),
Cyc_Toc_member_exp(_tmp48D,Cyc_Toc_curr_sp,0)),_tmp48E);}else{
# 3555
struct Cyc_Absyn_Exp*_tmp61F=Cyc_Toc__check_dyneither_subscript_e;
struct Cyc_Absyn_Exp*_tmpC5E[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp5FA),
Cyc_Absyn_fncall_exp(_tmp61F,(
(_tmpC5E[2]=_tmp48E,((_tmpC5E[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC5E[0]=_tmp48D,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC5E,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}
# 3561
goto _LL333;}}_LL333:;}
# 3563
goto _LL329;}default: _LL32E: _LL32F: {
const char*_tmpC61;void*_tmpC60;(_tmpC60=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC61="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(_tmpC61,sizeof(char),49))),_tag_dyneither(_tmpC60,sizeof(void*),0)));}}_LL329:;}
# 3566
Cyc_Toc_set_lhs(nv,_tmp5F6);
goto _LL222;};}case 23: _LL255: _tmp48C=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp46F)->f1;_LL256:
# 3569
 if(!Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp48C))->r;else{
# 3574
struct Cyc_List_List*_tmp623=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp48C);
void*_tmp624=Cyc_Toc_add_tuple_type(_tmp623);
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp48C != 0;(_tmp48C=_tmp48C->tl,i ++)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp48C->hd);{
struct _tuple19*_tmpC64;struct Cyc_List_List*_tmpC63;dles=((_tmpC63=_cycalloc(sizeof(*_tmpC63)),((_tmpC63->hd=((_tmpC64=_cycalloc(sizeof(*_tmpC64)),((_tmpC64->f1=0,((_tmpC64->f2=(struct Cyc_Absyn_Exp*)_tmp48C->hd,_tmpC64)))))),((_tmpC63->tl=dles,_tmpC63))))));};}}
# 3581
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}
# 3584
goto _LL222;case 25: _LL257: _tmp48B=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp46F)->f1;_LL258:
# 3588
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp48B);
{struct Cyc_List_List*_tmp627=_tmp48B;for(0;_tmp627 != 0;_tmp627=_tmp627->tl){
struct _tuple19*_tmp628=(struct _tuple19*)_tmp627->hd;struct Cyc_Absyn_Exp*_tmp62A;struct _tuple19*_tmp629=_tmp628;_tmp62A=_tmp629->f2;
Cyc_Toc_exp_to_c(nv,_tmp62A);}}
# 3593
goto _LL222;case 26: _LL259: _tmp487=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp46F)->f1;_tmp488=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp46F)->f2;_tmp489=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp46F)->f3;_tmp48A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp46F)->f4;_LL25A: {
# 3597
struct _tuple12 _tmp62B=Cyc_Evexp_eval_const_uint_exp(_tmp488);unsigned int _tmp62D;int _tmp62E;struct _tuple12 _tmp62C=_tmp62B;_tmp62D=_tmp62C.f1;_tmp62E=_tmp62C.f2;{
void*_tmp62F=Cyc_Toc_typ_to_c((void*)_check_null(_tmp489->topt));
Cyc_Toc_exp_to_c(nv,_tmp489);{
struct Cyc_List_List*es=0;
# 3602
if(!Cyc_Toc_is_zero(_tmp489)){
if(!_tmp62E){
const char*_tmpC67;void*_tmpC66;(_tmpC66=0,Cyc_Tcutil_terr(_tmp488->loc,((_tmpC67="cannot determine value of constant",_tag_dyneither(_tmpC67,sizeof(char),35))),_tag_dyneither(_tmpC66,sizeof(void*),0)));}
{unsigned int i=0;for(0;i < _tmp62D;++ i){
struct _tuple19*_tmpC6A;struct Cyc_List_List*_tmpC69;es=((_tmpC69=_cycalloc(sizeof(*_tmpC69)),((_tmpC69->hd=((_tmpC6A=_cycalloc(sizeof(*_tmpC6A)),((_tmpC6A->f1=0,((_tmpC6A->f2=_tmp489,_tmpC6A)))))),((_tmpC69->tl=es,_tmpC69))))));}}
# 3608
if(_tmp48A){
struct Cyc_Absyn_Exp*_tmp634=Cyc_Toc_cast_it(_tmp62F,Cyc_Absyn_uint_exp(0,0));
struct _tuple19*_tmpC6D;struct Cyc_List_List*_tmpC6C;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,((_tmpC6C=_cycalloc(sizeof(*_tmpC6C)),((_tmpC6C->hd=((_tmpC6D=_cycalloc(sizeof(*_tmpC6D)),((_tmpC6D->f1=0,((_tmpC6D->f2=_tmp634,_tmpC6D)))))),((_tmpC6C->tl=0,_tmpC6C)))))));}}
# 3613
e->r=Cyc_Toc_unresolvedmem_exp_r(0,es);
goto _LL222;};};}case 27: _LL25B: _tmp484=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp46F)->f1;_tmp485=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp46F)->f2;_tmp486=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp46F)->f3;_LL25C:
# 3618
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,0);
goto _LL222;case 28: _LL25D: _tmp480=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp46F)->f1;_tmp481=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp46F)->f2;_tmp482=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp46F)->f3;_tmp483=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp46F)->f4;_LL25E:
# 3623
 if(!Cyc_Toc_is_toplevel(nv)){
struct Cyc_Absyn_Exp*_tmp637=Cyc_Toc_init_struct(nv,old_typ,_tmp481,0,0,_tmp482,_tmp480);
e->r=_tmp637->r;
e->topt=_tmp637->topt;}else{
# 3634
if(_tmp483 == 0){
const char*_tmpC70;void*_tmpC6F;(_tmpC6F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC70="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpC70,sizeof(char),38))),_tag_dyneither(_tmpC6F,sizeof(void*),0)));}{
struct Cyc_Absyn_Aggrdecl*sd2=_tmp483;
# 3638
struct _RegionHandle _tmp63A=_new_region("rgn");struct _RegionHandle*rgn=& _tmp63A;_push_region(rgn);
{struct Cyc_List_List*_tmp63B=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp482,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
# 3642
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp483->impl))->tagged){
# 3644
struct _tuple30*_tmp63C=(struct _tuple30*)((struct Cyc_List_List*)_check_null(_tmp63B))->hd;struct Cyc_Absyn_Aggrfield*_tmp63E;struct Cyc_Absyn_Exp*_tmp63F;struct _tuple30*_tmp63D=_tmp63C;_tmp63E=_tmp63D->f1;_tmp63F=_tmp63D->f2;{
void*_tmp640=(void*)_check_null(_tmp63F->topt);
void*_tmp641=_tmp63E->type;
Cyc_Toc_exp_to_c(nv,_tmp63F);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp641) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp640))
_tmp63F->r=
Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp63F->r,0));{
# 3653
int i=Cyc_Toc_get_member_offset(_tmp483,_tmp63E->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct _tuple19*_tmpC75;struct _tuple19*_tmpC74;struct _tuple19*_tmpC73[2];struct Cyc_List_List*_tmp642=(_tmpC73[1]=((_tmpC74=_cycalloc(sizeof(*_tmpC74)),((_tmpC74->f1=0,((_tmpC74->f2=_tmp63F,_tmpC74)))))),((_tmpC73[0]=((_tmpC75=_cycalloc(sizeof(*_tmpC75)),((_tmpC75->f1=0,((_tmpC75->f2=field_tag_exp,_tmpC75)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC73,sizeof(struct _tuple19*),2)))));
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp642,0);
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpC7B;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpC7A;void*_tmpC79[1];struct Cyc_List_List*ds=(_tmpC79[0]=(void*)((_tmpC7B=_cycalloc(sizeof(*_tmpC7B)),((_tmpC7B[0]=((_tmpC7A.tag=1,((_tmpC7A.f1=_tmp63E->name,_tmpC7A)))),_tmpC7B)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC79,sizeof(void*),1)));
struct _tuple19*_tmpC7E;struct _tuple19*_tmpC7D[1];struct Cyc_List_List*dles=(_tmpC7D[0]=((_tmpC7E=_cycalloc(sizeof(*_tmpC7E)),((_tmpC7E->f1=ds,((_tmpC7E->f2=umem,_tmpC7E)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC7D,sizeof(struct _tuple19*),1)));
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{
# 3662
struct Cyc_List_List*_tmp64B=0;
struct Cyc_List_List*_tmp64C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields;
for(0;_tmp64C != 0;_tmp64C=_tmp64C->tl){
struct Cyc_List_List*_tmp64D=_tmp63B;for(0;_tmp64D != 0;_tmp64D=_tmp64D->tl){
if((*((struct _tuple30*)_tmp64D->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp64C->hd){
struct _tuple30*_tmp64E=(struct _tuple30*)_tmp64D->hd;struct Cyc_Absyn_Aggrfield*_tmp650;struct Cyc_Absyn_Exp*_tmp651;struct _tuple30*_tmp64F=_tmp64E;_tmp650=_tmp64F->f1;_tmp651=_tmp64F->f2;{
void*_tmp652=Cyc_Toc_typ_to_c(_tmp650->type);
void*_tmp653=Cyc_Toc_typ_to_c((void*)_check_null(_tmp651->topt));
Cyc_Toc_exp_to_c(nv,_tmp651);
# 3672
if(!Cyc_Tcutil_unify(_tmp652,_tmp653)){
# 3674
if(!Cyc_Tcutil_is_arithmetic_type(_tmp652) || !
Cyc_Tcutil_is_arithmetic_type(_tmp653))
_tmp651=Cyc_Toc_cast_it(_tmp652,Cyc_Absyn_copy_exp(_tmp651));}
{struct _tuple19*_tmpC81;struct Cyc_List_List*_tmpC80;_tmp64B=((_tmpC80=_cycalloc(sizeof(*_tmpC80)),((_tmpC80->hd=((_tmpC81=_cycalloc(sizeof(*_tmpC81)),((_tmpC81->f1=0,((_tmpC81->f2=_tmp651,_tmpC81)))))),((_tmpC80->tl=_tmp64B,_tmpC80))))));}
break;};}}}
# 3681
e->r=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp64B));}}
# 3639
;_pop_region(rgn);};}
# 3685
goto _LL222;case 29: _LL25F: _tmp47E=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp46F)->f1;_tmp47F=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp46F)->f2;_LL260: {
# 3687
struct Cyc_List_List*fs;
{void*_tmp656=Cyc_Tcutil_compress(_tmp47E);void*_tmp657=_tmp656;struct Cyc_List_List*_tmp658;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp657)->tag == 12){_LL339: _tmp658=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp657)->f2;_LL33A:
 fs=_tmp658;goto _LL338;}else{_LL33B: _LL33C: {
const char*_tmpC85;void*_tmpC84[1];struct Cyc_String_pa_PrintArg_struct _tmpC83;(_tmpC83.tag=0,((_tmpC83.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp47E)),((_tmpC84[0]=& _tmpC83,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC85="anon struct has type %s",_tag_dyneither(_tmpC85,sizeof(char),24))),_tag_dyneither(_tmpC84,sizeof(void*),1)))))));}}_LL338:;}{
# 3692
struct _RegionHandle _tmp65C=_new_region("rgn");struct _RegionHandle*rgn=& _tmp65C;_push_region(rgn);{
struct Cyc_List_List*_tmp65D=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp47F,Cyc_Absyn_StructA,fs);
for(0;_tmp65D != 0;_tmp65D=_tmp65D->tl){
struct _tuple30*_tmp65E=(struct _tuple30*)_tmp65D->hd;struct Cyc_Absyn_Aggrfield*_tmp660;struct Cyc_Absyn_Exp*_tmp661;struct _tuple30*_tmp65F=_tmp65E;_tmp660=_tmp65F->f1;_tmp661=_tmp65F->f2;{
void*_tmp662=(void*)_check_null(_tmp661->topt);
void*_tmp663=_tmp660->type;
Cyc_Toc_exp_to_c(nv,_tmp661);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp663) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp662))
_tmp661->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp661->r,0));};}
# 3706
e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp47F);}
# 3708
_npop_handler(0);goto _LL222;
# 3692
;_pop_region(rgn);};}case 30: _LL261: _tmp47B=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp46F)->f1;_tmp47C=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp46F)->f2;_tmp47D=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp46F)->f3;_LL262: {
# 3711
void*datatype_ctype;
struct Cyc_Absyn_Exp*tag_exp;
struct _tuple1*_tmp664=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp665=Cyc_Absyn_var_exp(_tmp664,0);
struct Cyc_Absyn_Exp*mem_exp;
datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp47D->name,_tmp47C->name));
tag_exp=_tmp47C->is_extensible?Cyc_Absyn_var_exp(_tmp47D->name,0):
 Cyc_Toc_datatype_tag(_tmp47C,_tmp47D->name);
mem_exp=_tmp665;{
struct Cyc_List_List*_tmp666=_tmp47D->typs;
# 3722
if(Cyc_Toc_is_toplevel(nv)){
# 3724
struct Cyc_List_List*dles=0;
for(0;_tmp47B != 0;(_tmp47B=_tmp47B->tl,_tmp666=_tmp666->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp47B->hd;
void*_tmp667=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp666))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp667))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct _tuple19*_tmpC88;struct Cyc_List_List*_tmpC87;dles=((_tmpC87=_cycalloc(sizeof(*_tmpC87)),((_tmpC87->hd=((_tmpC88=_cycalloc(sizeof(*_tmpC88)),((_tmpC88->f1=0,((_tmpC88->f2=cur_e,_tmpC88)))))),((_tmpC87->tl=dles,_tmpC87))))));};}
# 3735
{struct _tuple19*_tmpC8B;struct Cyc_List_List*_tmpC8A;dles=((_tmpC8A=_cycalloc(sizeof(*_tmpC8A)),((_tmpC8A->hd=((_tmpC8B=_cycalloc(sizeof(*_tmpC8B)),((_tmpC8B->f1=0,((_tmpC8B->f2=tag_exp,_tmpC8B)))))),((_tmpC8A->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles),_tmpC8A))))));}
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{
# 3741
struct Cyc_List_List*_tmpC8C;struct Cyc_List_List*_tmp66C=
(_tmpC8C=_cycalloc(sizeof(*_tmpC8C)),((_tmpC8C->hd=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Toc_tag_sp,0),tag_exp,0),((_tmpC8C->tl=0,_tmpC8C)))));
# 3744
{int i=1;for(0;_tmp47B != 0;(((_tmp47B=_tmp47B->tl,i ++)),_tmp666=_tmp666->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp47B->hd;
void*_tmp66D=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp666))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp66D))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct Cyc_Absyn_Stmt*_tmp66E=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Absyn_fieldname(i),0),cur_e,0);
# 3754
struct Cyc_List_List*_tmpC8D;_tmp66C=((_tmpC8D=_cycalloc(sizeof(*_tmpC8D)),((_tmpC8D->hd=_tmp66E,((_tmpC8D->tl=_tmp66C,_tmpC8D))))));};}}{
# 3756
struct Cyc_Absyn_Stmt*_tmp670=Cyc_Absyn_exp_stmt(_tmp665,0);
struct Cyc_List_List*_tmpC8E;struct Cyc_Absyn_Stmt*_tmp671=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpC8E=_cycalloc(sizeof(*_tmpC8E)),((_tmpC8E->hd=_tmp670,((_tmpC8E->tl=_tmp66C,_tmpC8E))))))),0);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp664,datatype_ctype,0,_tmp671,0));};}
# 3760
goto _LL222;};}case 31: _LL263: _LL264:
# 3762
 goto _LL266;case 32: _LL265: _LL266:
 goto _LL222;case 33: _LL267: _tmp475=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46F)->f1).is_calloc;_tmp476=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46F)->f1).rgn;_tmp477=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46F)->f1).elt_type;_tmp478=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46F)->f1).num_elts;_tmp479=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46F)->f1).fat_result;_tmp47A=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46F)->f1).inline_call;_LL268: {
# 3766
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp477)));
Cyc_Toc_exp_to_c(nv,_tmp478);
# 3770
if(_tmp479){
struct _tuple1*_tmp674=Cyc_Toc_temp_var();
struct _tuple1*_tmp675=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp475){
xexp=_tmp478;
if(_tmp476 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp476;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp674,0));}else{
# 3781
pexp=Cyc_Toc_calloc_exp(*_tmp477,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp674,0));}{
# 3783
struct Cyc_Absyn_Exp*_tmpC8F[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpC8F[2]=
Cyc_Absyn_var_exp(_tmp674,0),((_tmpC8F[1]=
# 3784
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpC8F[0]=Cyc_Absyn_var_exp(_tmp675,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC8F,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{
# 3787
if(_tmp476 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp476;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp47A)
pexp=Cyc_Toc_rmalloc_inline_exp(rgn,Cyc_Absyn_var_exp(_tmp674,0));else{
# 3793
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp674,0));}}else{
# 3795
pexp=Cyc_Toc_malloc_exp(*_tmp477,Cyc_Absyn_var_exp(_tmp674,0));}{
# 3797
struct Cyc_Absyn_Exp*_tmpC90[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((_tmpC90[2]=
Cyc_Absyn_var_exp(_tmp674,0),((_tmpC90[1]=
# 3797
Cyc_Absyn_uint_exp(1,0),((_tmpC90[0]=Cyc_Absyn_var_exp(_tmp675,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC90,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}{
# 3800
struct Cyc_Absyn_Stmt*_tmp678=Cyc_Absyn_declare_stmt(_tmp674,Cyc_Absyn_uint_typ,_tmp478,
Cyc_Absyn_declare_stmt(_tmp675,Cyc_Absyn_cstar_typ(t_c,Cyc_Toc_mt_tq),pexp,
Cyc_Absyn_exp_stmt(rexp,0),0),0);
e->r=Cyc_Toc_stmt_exp_r(_tmp678);};}else{
# 3805
if(_tmp475){
if(_tmp476 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp476;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp478))->r;}else{
# 3811
e->r=(Cyc_Toc_calloc_exp(*_tmp477,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp478))->r;}}else{
# 3814
if(_tmp476 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp476;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp47A)
e->r=(Cyc_Toc_rmalloc_inline_exp(rgn,_tmp478))->r;else{
# 3820
e->r=(Cyc_Toc_rmalloc_exp(rgn,_tmp478))->r;}}else{
# 3822
e->r=(Cyc_Toc_malloc_exp(*_tmp477,_tmp478))->r;}}}
# 3826
goto _LL222;}case 34: _LL269: _tmp473=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp46F)->f1;_tmp474=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp46F)->f2;_LL26A: {
# 3835
void*e1_old_typ=(void*)_check_null(_tmp473->topt);
void*e2_old_typ=(void*)_check_null(_tmp474->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ) && !Cyc_Tcutil_is_pointer_type(e1_old_typ)){
const char*_tmpC93;void*_tmpC92;(_tmpC92=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC93="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",_tag_dyneither(_tmpC93,sizeof(char),57))),_tag_dyneither(_tmpC92,sizeof(void*),0)));}{
# 3841
struct Cyc_Absyn_Exp*swap_fn;
if(Cyc_Tcutil_is_dyneither_ptr(e1_old_typ))
swap_fn=Cyc_Toc__swap_dyneither_e;else{
# 3845
swap_fn=Cyc_Toc__swap_word_e;}{
# 3849
int f1_tag=0;
void*tagged_mem_type1=(void*)& Cyc_Absyn_VoidType_val;
int e1_tmem=Cyc_Toc_is_tagged_union_project(_tmp473,& f1_tag,& tagged_mem_type1,1);
int f2_tag=0;
void*tagged_mem_type2=(void*)& Cyc_Absyn_VoidType_val;
int e2_tmem=Cyc_Toc_is_tagged_union_project(_tmp474,& f2_tag,& tagged_mem_type2,1);
# 3856
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp473);
Cyc_Toc_exp_to_c(nv,_tmp474);
Cyc_Toc_set_lhs(nv,0);
# 3862
if(e1_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp473,f1_tag,tagged_mem_type1);else{
# 3866
_tmp473=Cyc_Toc_push_address_exp(_tmp473);}
# 3868
if(e2_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp474,f2_tag,tagged_mem_type2);else{
# 3872
_tmp474=Cyc_Toc_push_address_exp(_tmp474);}
# 3874
{struct Cyc_Absyn_Exp*_tmpC94[2];e->r=Cyc_Toc_fncall_exp_r(swap_fn,((_tmpC94[1]=_tmp474,((_tmpC94[0]=_tmp473,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC94,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
# 3876
goto _LL222;};};}case 37: _LL26B: _tmp471=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp46F)->f1;_tmp472=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp46F)->f2;_LL26C: {
# 3879
void*_tmp67C=Cyc_Tcutil_compress((void*)_check_null(_tmp471->topt));
Cyc_Toc_exp_to_c(nv,_tmp471);
{void*_tmp67D=_tmp67C;struct Cyc_Absyn_Aggrdecl*_tmp67E;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp67D)->tag == 11){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp67D)->f1).aggr_info).KnownAggr).tag == 2){_LL33E: _tmp67E=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp67D)->f1).aggr_info).KnownAggr).val;_LL33F: {
# 3883
struct Cyc_Absyn_Exp*_tmp67F=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp67E,_tmp472),0);
struct Cyc_Absyn_Exp*_tmp680=Cyc_Toc_member_exp(_tmp471,_tmp472,0);
struct Cyc_Absyn_Exp*_tmp681=Cyc_Toc_member_exp(_tmp680,Cyc_Toc_tag_sp,0);
e->r=(Cyc_Absyn_eq_exp(_tmp681,_tmp67F,0))->r;
goto _LL33D;}}else{goto _LL340;}}else{_LL340: _LL341: {
const char*_tmpC97;void*_tmpC96;(_tmpC96=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC97="non-aggregate type in tagcheck",_tag_dyneither(_tmpC97,sizeof(char),31))),_tag_dyneither(_tmpC96,sizeof(void*),0)));}}_LL33D:;}
# 3890
goto _LL222;}case 36: _LL26D: _tmp470=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp46F)->f1;_LL26E:
 Cyc_Toc_stmt_to_c(nv,_tmp470);goto _LL222;case 35: _LL26F: _LL270: {
const char*_tmpC9A;void*_tmpC99;(_tmpC99=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC9A="UnresolvedMem",_tag_dyneither(_tmpC9A,sizeof(char),14))),_tag_dyneither(_tmpC99,sizeof(void*),0)));}case 24: _LL271: _LL272: {
const char*_tmpC9D;void*_tmpC9C;(_tmpC9C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC9D="compoundlit",_tag_dyneither(_tmpC9D,sizeof(char),12))),_tag_dyneither(_tmpC9C,sizeof(void*),0)));}case 38: _LL273: _LL274: {
const char*_tmpCA0;void*_tmpC9F;(_tmpC9F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCA0="valueof(-)",_tag_dyneither(_tmpCA0,sizeof(char),11))),_tag_dyneither(_tmpC9F,sizeof(void*),0)));}default: _LL275: _LL276: {
const char*_tmpCA3;void*_tmpCA2;(_tmpCA2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCA3="__asm__",_tag_dyneither(_tmpCA3,sizeof(char),8))),_tag_dyneither(_tmpCA2,sizeof(void*),0)));}}_LL222:;};}
# 3927 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){
return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
# 3930
Cyc_Toc_skip_stmt_dl(),0);}
# 3933
static int Cyc_Toc_path_length(struct Cyc_Absyn_Exp*e){
void*_tmp68C=e->r;void*_tmp68D=_tmp68C;struct Cyc_Absyn_Exp*_tmp68E;struct Cyc_Absyn_Exp*_tmp68F;struct Cyc_Absyn_Exp*_tmp690;switch(*((int*)_tmp68D)){case 13: _LL343: _tmp690=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp68D)->f2;_LL344:
 return Cyc_Toc_path_length(_tmp690);case 19: _LL345: _tmp68F=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp68D)->f1;_LL346:
 return 1 + Cyc_Toc_path_length(_tmp68F);case 20: _LL347: _tmp68E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp68D)->f1;_LL348:
 return Cyc_Toc_path_length(_tmp68E);default: _LL349: _LL34A:
 return 0;}_LL342:;}struct _tuple31{struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple32{struct _tuple1*f1;void*f2;};struct _tuple33{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3942
static struct _tuple31 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
# 3947
struct Cyc_Absyn_Stmt*s;
{void*_tmp691=p->r;void*_tmp692=_tmp691;union Cyc_Absyn_AggrInfoU _tmp693;struct Cyc_List_List*_tmp694;struct Cyc_List_List*_tmp695;struct Cyc_List_List*_tmp696;struct Cyc_Absyn_Pat*_tmp697;struct Cyc_Absyn_Datatypedecl*_tmp698;struct Cyc_Absyn_Datatypefield*_tmp699;struct Cyc_List_List*_tmp69A;void*_tmp69B;struct Cyc_Absyn_Enumfield*_tmp69C;struct Cyc_Absyn_Enumdecl*_tmp69D;struct Cyc_Absyn_Enumfield*_tmp69E;struct _dyneither_ptr _tmp69F;int _tmp6A0;char _tmp6A1;enum Cyc_Absyn_Sign _tmp6A2;int _tmp6A3;struct Cyc_Absyn_Vardecl*_tmp6A4;struct Cyc_Absyn_Pat*_tmp6A5;struct _tuple1*_tmp6A6;struct _tuple1*_tmp6A7;struct Cyc_Absyn_Pat*_tmp6A8;struct _tuple1*_tmp6A9;switch(*((int*)_tmp692)){case 2: _LL34C: _tmp6A9=(((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp692)->f2)->name;_LL34D: {
# 3950
struct Cyc_Absyn_Pat*_tmp6AA=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0);
_tmp6AA->topt=p->topt;
_tmp6A7=_tmp6A9;_tmp6A8=_tmp6AA;goto _LL34F;}case 1: _LL34E: _tmp6A7=(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp692)->f1)->name;_tmp6A8=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp692)->f2;_LL34F: {
# 3955
struct _tuple1*v=Cyc_Toc_temp_var();
void*_tmp6AB=(void*)_check_null(_tmp6A8->topt);
{struct _tuple32*_tmpCA6;struct Cyc_List_List*_tmpCA5;decls=((_tmpCA5=_region_malloc(rgn,sizeof(*_tmpCA5)),((_tmpCA5->hd=((_tmpCA6=_region_malloc(rgn,sizeof(*_tmpCA6)),((_tmpCA6->f1=v,((_tmpCA6->f2=Cyc_Toc_typ_to_c_array(_tmp6AB),_tmpCA6)))))),((_tmpCA5->tl=decls,_tmpCA5))))));}{
struct _tuple31 _tmp6AE=Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp6A7,Cyc_Absyn_var_exp(v,0)),rgn,_tmp6AB,
Cyc_Absyn_var_exp(v,0),path,_tmp6A8,fail_stmt,decls);
nv=_tmp6AE.f1;
decls=_tmp6AE.f2;{
struct Cyc_Absyn_Stmt*_tmp6AF=_tmp6AE.f3;
struct Cyc_Absyn_Stmt*_tmp6B0=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(v,0),r,0);
s=Cyc_Absyn_seq_stmt(_tmp6B0,_tmp6AF,0);
goto _LL34B;};};}case 4: _LL350: _tmp6A6=(((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp692)->f2)->name;_LL351: {
# 3968
struct _tuple1*_tmp6B1=Cyc_Toc_temp_var();
void*_tmp6B2=(void*)_check_null(p->topt);
{struct _tuple32*_tmpCA9;struct Cyc_List_List*_tmpCA8;decls=((_tmpCA8=_region_malloc(rgn,sizeof(*_tmpCA8)),((_tmpCA8->hd=((_tmpCA9=_region_malloc(rgn,sizeof(*_tmpCA9)),((_tmpCA9->f1=_tmp6B1,((_tmpCA9->f2=Cyc_Toc_typ_to_c_array(_tmp6B2),_tmpCA9)))))),((_tmpCA8->tl=decls,_tmpCA8))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp6A6,Cyc_Absyn_var_exp(_tmp6B1,0));
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp6B1,0),r,0);
goto _LL34B;}case 0: _LL352: _LL353:
 s=Cyc_Toc_skip_stmt_dl();goto _LL34B;case 3: _LL354: _tmp6A4=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp692)->f1;_tmp6A5=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp692)->f2;_LL355: {
# 3977
struct _tuple1*_tmp6B5=Cyc_Toc_temp_var();
{struct _tuple32*_tmpCAC;struct Cyc_List_List*_tmpCAB;decls=((_tmpCAB=_region_malloc(rgn,sizeof(*_tmpCAB)),((_tmpCAB->hd=((_tmpCAC=_region_malloc(rgn,sizeof(*_tmpCAC)),((_tmpCAC->f1=_tmp6B5,((_tmpCAC->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),_tmpCAC)))))),((_tmpCAB->tl=decls,_tmpCAB))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp6A4->name,Cyc_Absyn_var_exp(_tmp6B5,0));
# 3981
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp6B5,0),
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(path)),0);{
struct _tuple31 _tmp6B8=Cyc_Toc_xlate_pat(nv,rgn,t,r,path,_tmp6A5,fail_stmt,decls);
_tmp6B8.f3=Cyc_Absyn_seq_stmt(s,_tmp6B8.f3,0);
return _tmp6B8;};}case 9: _LL356: _LL357:
# 3988
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL34B;case 10: _LL358: _tmp6A2=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp692)->f1;_tmp6A3=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp692)->f2;_LL359:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(_tmp6A2,_tmp6A3,0),fail_stmt);goto _LL34B;case 11: _LL35A: _tmp6A1=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp692)->f1;_LL35B:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp6A1,0),fail_stmt);goto _LL34B;case 12: _LL35C: _tmp69F=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp692)->f1;_tmp6A0=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp692)->f2;_LL35D:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp69F,_tmp6A0,0),fail_stmt);goto _LL34B;case 13: _LL35E: _tmp69D=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp692)->f1;_tmp69E=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp692)->f2;_LL35F:
# 3993
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmpCAF;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpCAE;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpCAE=_cycalloc(sizeof(*_tmpCAE)),((_tmpCAE[0]=((_tmpCAF.tag=31,((_tmpCAF.f1=_tmp69E->name,((_tmpCAF.f2=_tmp69D,((_tmpCAF.f3=_tmp69E,_tmpCAF)))))))),_tmpCAE)))),0),fail_stmt);}
goto _LL34B;case 14: _LL360: _tmp69B=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp692)->f1;_tmp69C=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp692)->f2;_LL361:
# 3996
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpCB2;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpCB1;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpCB1=_cycalloc(sizeof(*_tmpCB1)),((_tmpCB1[0]=((_tmpCB2.tag=32,((_tmpCB2.f1=_tmp69C->name,((_tmpCB2.f2=_tmp69B,((_tmpCB2.f3=_tmp69C,_tmpCB2)))))))),_tmpCB1)))),0),fail_stmt);}
goto _LL34B;case 6: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp692)->f1)->r)->tag == 8){_LL362: _tmp698=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp692)->f1)->r)->f1;_tmp699=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp692)->f1)->r)->f2;_tmp69A=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp692)->f1)->r)->f3;_LL363:
# 4006
 s=Cyc_Toc_skip_stmt_dl();{
struct _tuple1*_tmp6BD=Cyc_Toc_temp_var();
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(_tmp699->name,_tmp698->name);
void*_tmp6BE=Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);
int cnt=1;
struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(_tmp6BE,r);
struct Cyc_List_List*_tmp6BF=_tmp699->typs;
for(0;_tmp69A != 0;(((_tmp69A=_tmp69A->tl,_tmp6BF=((struct Cyc_List_List*)_check_null(_tmp6BF))->tl)),++ cnt)){
struct Cyc_Absyn_Pat*_tmp6C0=(struct Cyc_Absyn_Pat*)_tmp69A->hd;
if(_tmp6C0->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp6C1=(*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp6BF))->hd)).f2;
void*_tmp6C2=(void*)_check_null(_tmp6C0->topt);
void*_tmp6C3=Cyc_Toc_typ_to_c_array(_tmp6C2);
struct Cyc_Absyn_Exp*_tmp6C4=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp6BD,0),Cyc_Absyn_fieldname(cnt),0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp6C1)))
_tmp6C4=Cyc_Toc_cast_it(_tmp6C3,_tmp6C4);{
# 4024
struct _tuple31 _tmp6C5=Cyc_Toc_xlate_pat(nv,rgn,_tmp6C2,_tmp6C4,_tmp6C4,_tmp6C0,fail_stmt,decls);
# 4026
nv=_tmp6C5.f1;
decls=_tmp6C5.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp6C5.f3,0);};};}{
# 4030
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp6BD,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*tag_exp=
_tmp698->is_extensible?Cyc_Absyn_var_exp(_tmp699->name,0): Cyc_Toc_datatype_tag(_tmp698,_tmp699->name);
struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,tag_exp,0);
s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);
if(Cyc_Toc_is_nullable((void*)_check_null(p->topt)))
# 4038
s=Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp6BD,0),Cyc_Absyn_uint_exp(0,0),0),fail_stmt,s,0);
# 4041
s=Cyc_Absyn_declare_stmt(_tmp6BD,_tmp6BE,rcast,s,0);
goto _LL34B;};};}else{_LL36C: _tmp697=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp692)->f1;_LL36D: {
# 4140
void*_tmp6EA=(void*)_check_null(_tmp697->topt);
# 4143
struct _tuple31 _tmp6EB=Cyc_Toc_xlate_pat(nv,rgn,_tmp6EA,Cyc_Absyn_deref_exp(r,0),Cyc_Absyn_deref_exp(path,0),_tmp697,fail_stmt,decls);
# 4145
nv=_tmp6EB.f1;
decls=_tmp6EB.f2;{
struct Cyc_Absyn_Stmt*_tmp6EC=_tmp6EB.f3;
if(Cyc_Toc_is_nullable(t))
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,
# 4151
Cyc_Toc_skip_stmt_dl(),0),_tmp6EC,0);else{
# 4153
s=_tmp6EC;}
goto _LL34B;};}}case 8: _LL364: _tmp696=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp692)->f3;_LL365:
# 4044
 _tmp695=_tmp696;goto _LL367;case 5: _LL366: _tmp695=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp692)->f1;_LL367:
# 4052
 s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;
for(0;_tmp695 != 0;(_tmp695=_tmp695->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp6C6=(struct Cyc_Absyn_Pat*)_tmp695->hd;
if(_tmp6C6->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp6C7=(void*)_check_null(_tmp6C6->topt);
struct _dyneither_ptr*_tmp6C8=Cyc_Absyn_fieldname(cnt);
struct _tuple31 _tmp6C9=Cyc_Toc_xlate_pat(nv,rgn,_tmp6C7,Cyc_Toc_member_exp(r,_tmp6C8,0),
Cyc_Toc_member_exp(path,_tmp6C8,0),_tmp6C6,fail_stmt,decls);
nv=_tmp6C9.f1;
decls=_tmp6C9.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp6C9.f3,0);};}
# 4066
goto _LL34B;};case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp692)->f1 == 0){_LL368: _LL369: {
# 4068
const char*_tmpCB5;void*_tmpCB4;(_tmpCB4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCB5="unresolved aggregate pattern!",_tag_dyneither(_tmpCB5,sizeof(char),30))),_tag_dyneither(_tmpCB4,sizeof(void*),0)));}}else{_LL36A: _tmp693=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp692)->f1)->aggr_info;_tmp694=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp692)->f3;_LL36B: {
# 4070
struct Cyc_Absyn_Aggrdecl*_tmp6CC=Cyc_Absyn_get_known_aggrdecl(_tmp693);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6CC->impl))->tagged){
# 4074
struct _tuple33*_tmp6CD=(struct _tuple33*)((struct Cyc_List_List*)_check_null(_tmp694))->hd;struct Cyc_List_List*_tmp6CF;struct Cyc_Absyn_Pat*_tmp6D0;struct _tuple33*_tmp6CE=_tmp6CD;_tmp6CF=_tmp6CE->f1;_tmp6D0=_tmp6CE->f2;{
struct _dyneither_ptr*f;
{void*_tmp6D1=(void*)((struct Cyc_List_List*)_check_null(_tmp6CF))->hd;void*_tmp6D2=_tmp6D1;struct _dyneither_ptr*_tmp6D3;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp6D2)->tag == 1){_LL375: _tmp6D3=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp6D2)->f1;_LL376:
 f=_tmp6D3;goto _LL374;}else{_LL377: _LL378: {
const char*_tmpCB8;void*_tmpCB7;(_tmpCB7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCB8="no field name in tagged union pattern",_tag_dyneither(_tmpCB8,sizeof(char),38))),_tag_dyneither(_tmpCB7,sizeof(void*),0)));}}_LL374:;}{
# 4081
void*_tmp6D6=(void*)_check_null(_tmp6D0->topt);
void*_tmp6D7=Cyc_Toc_typ_to_c_array(_tmp6D6);
# 4084
struct Cyc_Absyn_Exp*_tmp6D8=Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(path,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*_tmp6D9=Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp6D9=Cyc_Toc_cast_it(_tmp6D7,_tmp6D9);
_tmp6D8=Cyc_Toc_cast_it(_tmp6D7,_tmp6D8);{
struct _tuple31 _tmp6DA=Cyc_Toc_xlate_pat(nv,rgn,_tmp6D6,_tmp6D9,_tmp6D8,_tmp6D0,fail_stmt,decls);
# 4092
nv=_tmp6DA.f1;
decls=_tmp6DA.f2;{
struct Cyc_Absyn_Stmt*_tmp6DB=_tmp6DA.f3;
struct Cyc_Absyn_Stmt*_tmp6DC=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(r,f,0),Cyc_Toc_tag_sp,0),
Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp6CC,f),0),fail_stmt);
# 4100
s=Cyc_Absyn_seq_stmt(_tmp6DC,_tmp6DB,0);};};};};}else{
# 4103
s=Cyc_Toc_skip_stmt_dl();
for(0;_tmp694 != 0;_tmp694=_tmp694->tl){
struct _tuple33*_tmp6DD=(struct _tuple33*)_tmp694->hd;
struct Cyc_Absyn_Pat*_tmp6DE=(*_tmp6DD).f2;
if(_tmp6DE->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*f;
{void*_tmp6DF=(void*)((struct Cyc_List_List*)_check_null((*_tmp6DD).f1))->hd;void*_tmp6E0=_tmp6DF;struct _dyneither_ptr*_tmp6E1;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp6E0)->tag == 1){_LL37A: _tmp6E1=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp6E0)->f1;_LL37B:
 f=_tmp6E1;goto _LL379;}else{_LL37C: _LL37D: {
struct Cyc_Toc_Match_error_exn_struct _tmpCBB;struct Cyc_Toc_Match_error_exn_struct*_tmpCBA;(int)_throw((void*)((_tmpCBA=_cycalloc_atomic(sizeof(*_tmpCBA)),((_tmpCBA[0]=((_tmpCBB.tag=Cyc_Toc_Match_error,_tmpCBB)),_tmpCBA)))));}}_LL379:;}{
# 4114
void*_tmp6E4=(void*)_check_null(_tmp6DE->topt);
void*_tmp6E5=Cyc_Toc_typ_to_c_array(_tmp6E4);
struct Cyc_Absyn_Exp*_tmp6E6=Cyc_Toc_member_exp(r,f,0);
void*_tmp6E7=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6CC->impl))->fields,f)))->type;
struct Cyc_Absyn_Exp*_tmp6E8=Cyc_Toc_member_exp(path,f,0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6E7))
_tmp6E6=Cyc_Toc_cast_it(_tmp6E5,_tmp6E6);else{
if(!Cyc_Toc_is_array_type(_tmp6E7) && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp6E7))){
# 4124
_tmp6E6=Cyc_Absyn_deref_exp(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(_tmp6E5,Cyc_Toc_mt_tq),
Cyc_Absyn_address_exp(_tmp6E6,0)),0);
# 4127
_tmp6E8=Cyc_Toc_push_address_exp(_tmp6E8);
_tmp6E8=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(_tmp6E5,Cyc_Toc_mt_tq),_tmp6E8);
_tmp6E8=Cyc_Absyn_deref_exp(_tmp6E8,0);}}{
# 4131
struct _tuple31 _tmp6E9=Cyc_Toc_xlate_pat(nv,rgn,_tmp6E4,_tmp6E6,_tmp6E8,_tmp6DE,fail_stmt,decls);
nv=_tmp6E9.f1;
decls=_tmp6E9.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp6E9.f3,0);};};};}}
# 4137
goto _LL34B;}}case 15: _LL36E: _LL36F: {
# 4155
const char*_tmpCBE;void*_tmpCBD;(_tmpCBD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCBE="unknownid pat",_tag_dyneither(_tmpCBE,sizeof(char),14))),_tag_dyneither(_tmpCBD,sizeof(void*),0)));}case 16: _LL370: _LL371: {
const char*_tmpCC1;void*_tmpCC0;(_tmpCC0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCC1="unknowncall pat",_tag_dyneither(_tmpCC1,sizeof(char),16))),_tag_dyneither(_tmpCC0,sizeof(void*),0)));}default: _LL372: _LL373: {
const char*_tmpCC4;void*_tmpCC3;(_tmpCC3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCC4="exp pat",_tag_dyneither(_tmpCC4,sizeof(char),8))),_tag_dyneither(_tmpCC3,sizeof(void*),0)));}}_LL34B:;}{
# 4159
struct _tuple31 _tmpCC5;return(_tmpCC5.f1=nv,((_tmpCC5.f2=decls,((_tmpCC5.f3=s,_tmpCC5)))));};}struct _tuple34{int f1;struct _dyneither_ptr*f2;struct _dyneither_ptr*f3;struct Cyc_Absyn_Switch_clause*f4;};
# 4186 "toc.cyc"
static struct _tuple34*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 4188
struct _tuple34*_tmpCC6;return(_tmpCC6=_region_malloc(r,sizeof(*_tmpCC6)),((_tmpCC6->f1=0,((_tmpCC6->f2=Cyc_Toc_fresh_label(),((_tmpCC6->f3=Cyc_Toc_fresh_label(),((_tmpCC6->f4=sc,_tmpCC6)))))))));}
# 4193
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(0;ps != 0;ps=((struct Cyc_List_List*)_check_null(ps))->tl){
struct Cyc_Tcpat_PathNode*_tmp6F5=(struct Cyc_Tcpat_PathNode*)ps->hd;
# 4199
if((int)(((_tmp6F5->orig_pat).pattern).tag == 1)){
union Cyc_Tcpat_PatOrWhere _tmpCC7;void*t=(void*)_check_null(((_tmpCC7=_tmp6F5->orig_pat,(((_tmpCC7.pattern).tag != 1?_throw_match(): 1,(_tmpCC7.pattern).val))))->topt);
void*_tmp6F6=Cyc_Tcutil_compress(Cyc_Toc_typ_to_c_array(t));
void*_tmp6F7=_tmp6F6;switch(*((int*)_tmp6F7)){case 0: _LL37F: _LL380:
# 4204
 goto _LL382;case 11: _LL381: _LL382:
 goto _LL384;case 12: _LL383: _LL384:
 goto _LL37E;default: _LL385: _LL386:
 v=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c_array(t),v);goto _LL37E;}_LL37E:;}{
# 4210
void*_tmp6F9=_tmp6F5->access;void*_tmp6FA=_tmp6F9;struct Cyc_Absyn_Datatypedecl*_tmp6FB;struct Cyc_Absyn_Datatypefield*_tmp6FC;unsigned int _tmp6FD;int _tmp6FE;struct _dyneither_ptr*_tmp6FF;unsigned int _tmp700;switch(*((int*)_tmp6FA)){case 0: _LL388: _LL389:
# 4215
 goto _LL387;case 1: _LL38A: _LL38B:
# 4220
 if(ps->tl != 0){
void*_tmp701=((struct Cyc_Tcpat_PathNode*)((struct Cyc_List_List*)_check_null(ps->tl))->hd)->access;void*_tmp702=_tmp701;struct Cyc_Absyn_Datatypedecl*_tmp703;struct Cyc_Absyn_Datatypefield*_tmp704;unsigned int _tmp705;if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp702)->tag == 3){_LL393: _tmp703=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp702)->f1;_tmp704=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp702)->f2;_tmp705=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp702)->f3;_LL394:
# 4223
 ps=ps->tl;
v=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp704->name,_tmp703->name)),Cyc_Toc_mt_tq),v);
v=Cyc_Absyn_aggrarrow_exp(v,Cyc_Absyn_fieldname((int)(_tmp705 + 1)),0);
continue;}else{_LL395: _LL396:
# 4228
 goto _LL392;}_LL392:;}
# 4231
v=Cyc_Absyn_deref_exp(v,0);
goto _LL387;case 2: _LL38C: _tmp700=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp6FA)->f1;_LL38D:
 v=Cyc_Toc_member_exp(v,Cyc_Absyn_fieldname((int)(_tmp700 + 1)),0);goto _LL387;case 4: _LL38E: _tmp6FE=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp6FA)->f1;_tmp6FF=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp6FA)->f2;_LL38F:
# 4235
 v=Cyc_Toc_member_exp(v,_tmp6FF,0);
if(_tmp6FE)
v=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0);
goto _LL387;default: _LL390: _tmp6FB=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6FA)->f1;_tmp6FC=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6FA)->f2;_tmp6FD=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6FA)->f3;_LL391:
# 4240
 v=Cyc_Toc_member_exp(v,Cyc_Absyn_fieldname((int)(_tmp6FD + 1)),0);
goto _LL387;}_LL387:;};}
# 4244
return v;}
# 4249
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
void*_tmp706=t;struct Cyc_Absyn_Datatypedecl*_tmp707;struct Cyc_Absyn_Datatypefield*_tmp708;struct _dyneither_ptr*_tmp709;int _tmp70A;int _tmp70B;struct Cyc_Absyn_Datatypedecl*_tmp70C;struct Cyc_Absyn_Datatypefield*_tmp70D;unsigned int _tmp70E;struct _dyneither_ptr _tmp70F;int _tmp710;void*_tmp711;struct Cyc_Absyn_Enumfield*_tmp712;struct Cyc_Absyn_Enumdecl*_tmp713;struct Cyc_Absyn_Enumfield*_tmp714;struct Cyc_Absyn_Exp*_tmp715;switch(*((int*)_tmp706)){case 0: _LL398: _tmp715=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp706)->f1;_LL399:
# 4253
 if(_tmp715 == 0)return v;else{return _tmp715;}case 1: _LL39A: _LL39B:
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_signed_int_exp(0,0),0);case 2: _LL39C: _LL39D:
 return Cyc_Absyn_neq_exp(v,Cyc_Absyn_signed_int_exp(0,0),0);case 3: _LL39E: _tmp713=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp706)->f1;_tmp714=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp706)->f2;_LL39F: {
# 4257
struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmpCCA;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpCC9;return Cyc_Absyn_eq_exp(v,Cyc_Absyn_new_exp((void*)((_tmpCC9=_cycalloc(sizeof(*_tmpCC9)),((_tmpCC9[0]=((_tmpCCA.tag=31,((_tmpCCA.f1=_tmp714->name,((_tmpCCA.f2=_tmp713,((_tmpCCA.f3=_tmp714,_tmpCCA)))))))),_tmpCC9)))),0),0);}case 4: _LL3A0: _tmp711=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp706)->f1;_tmp712=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp706)->f2;_LL3A1: {
# 4259
struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpCCD;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpCCC;return Cyc_Absyn_eq_exp(v,Cyc_Absyn_new_exp((void*)((_tmpCCC=_cycalloc(sizeof(*_tmpCCC)),((_tmpCCC[0]=((_tmpCCD.tag=32,((_tmpCCD.f1=_tmp712->name,((_tmpCCD.f2=_tmp711,((_tmpCCD.f3=_tmp712,_tmpCCD)))))))),_tmpCCC)))),0),0);}case 5: _LL3A2: _tmp70F=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp706)->f1;_tmp710=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp706)->f2;_LL3A3:
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_float_exp(_tmp70F,_tmp710,0),0);case 6: _LL3A4: _tmp70E=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp706)->f1;_LL3A5:
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_uint_exp(_tmp70E,0),0);case 7: _LL3A6: _tmp70B=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp706)->f1;_tmp70C=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp706)->f2;_tmp70D=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp706)->f3;_LL3A7:
# 4265
 LOOP1: {
void*_tmp71A=v->r;void*_tmp71B=_tmp71A;struct Cyc_Absyn_Exp*_tmp71C;struct Cyc_Absyn_Exp*_tmp71D;switch(*((int*)_tmp71B)){case 13: _LL3AD: _tmp71D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp71B)->f2;_LL3AE:
 v=_tmp71D;goto LOOP1;case 19: _LL3AF: _tmp71C=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp71B)->f1;_LL3B0:
 v=_tmp71C;goto _LL3AC;default: _LL3B1: _LL3B2:
 goto _LL3AC;}_LL3AC:;}
# 4272
v=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp70D->name,_tmp70C->name)),Cyc_Toc_mt_tq),v);
return Cyc_Absyn_eq_exp(Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0),Cyc_Absyn_uint_exp((unsigned int)_tmp70B,0),0);case 8: _LL3A8: _tmp709=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp706)->f1;_tmp70A=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp706)->f2;_LL3A9:
# 4275
 v=Cyc_Toc_member_exp(Cyc_Toc_member_exp(v,_tmp709,0),Cyc_Toc_tag_sp,0);
return Cyc_Absyn_eq_exp(v,Cyc_Absyn_uint_exp((unsigned int)_tmp70A,0),0);default: _LL3AA: _tmp707=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp706)->f1;_tmp708=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp706)->f2;_LL3AB:
# 4279
 LOOP2: {
void*_tmp71E=v->r;void*_tmp71F=_tmp71E;struct Cyc_Absyn_Exp*_tmp720;struct Cyc_Absyn_Exp*_tmp721;switch(*((int*)_tmp71F)){case 13: _LL3B4: _tmp721=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp71F)->f2;_LL3B5:
 v=_tmp721;goto LOOP2;case 19: _LL3B6: _tmp720=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp71F)->f1;_LL3B7:
 v=_tmp720;goto _LL3B3;default: _LL3B8: _LL3B9:
 goto _LL3B3;}_LL3B3:;}
# 4286
v=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp708->name,_tmp707->name)),Cyc_Toc_mt_tq),v);
return Cyc_Absyn_eq_exp(Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0),Cyc_Absyn_var_exp(_tmp708->name,0),0);}_LL397:;}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _dyneither_ptr*f1;};
# 4298
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1};struct _tuple35{int f1;void*f2;};
# 4306
static struct _tuple35 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 4308
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(0;ss != 0;(ss=ss->tl,c=c + 1)){
struct _tuple29 _tmp724=*((struct _tuple29*)ss->hd);void*_tmp726;struct _tuple29 _tmp725=_tmp724;_tmp726=_tmp725.f1;{
void*_tmp727=_tmp726;struct Cyc_Absyn_Exp*_tmp728;struct _dyneither_ptr*_tmp729;switch(*((int*)_tmp727)){case 3: _LL3BB: _LL3BC:
# 4314
 goto _LL3BE;case 4: _LL3BD: _LL3BE:
 goto _LL3C0;case 6: _LL3BF: _LL3C0:
# 4317
 continue;case 8: _LL3C1: _tmp729=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp727)->f1;_LL3C2:
# 4319
 if(k == (void*)& Cyc_Toc_OtherTest_val){
struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct _tmpCD0;struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_tmpCCF;k=(void*)((_tmpCCF=_cycalloc(sizeof(*_tmpCCF)),((_tmpCCF[0]=((_tmpCD0.tag=3,((_tmpCD0.f1=_tmp729,_tmpCD0)))),_tmpCCF))));}
continue;case 7: _LL3C3: _LL3C4:
# 4323
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;
continue;case 0: _LL3C5: _tmp728=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp727)->f1;if(_tmp728 != 0){_LL3C6:
# 4327
{struct Cyc_Toc_WhereTest_Toc_TestKind_struct _tmpCD3;struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_tmpCD2;k=(void*)((_tmpCD2=_cycalloc(sizeof(*_tmpCD2)),((_tmpCD2[0]=((_tmpCD3.tag=2,((_tmpCD3.f1=_tmp728,_tmpCD3)))),_tmpCD2))));}{
struct _tuple35 _tmpCD4;return(_tmpCD4.f1=0,((_tmpCD4.f2=k,_tmpCD4)));};}else{_LL3C7: _LL3C8:
 goto _LL3CA;}case 1: _LL3C9: _LL3CA:
 goto _LL3CC;case 2: _LL3CB: _LL3CC:
 goto _LL3CE;case 5: _LL3CD: _LL3CE:
 goto _LL3D0;default: _LL3CF: _LL3D0: {
struct _tuple35 _tmpCD5;return(_tmpCD5.f1=0,((_tmpCD5.f2=k,_tmpCD5)));}}_LL3BA:;};}{
# 4336
struct _tuple35 _tmpCD6;return(_tmpCD6.f1=c,((_tmpCD6.f2=k,_tmpCD6)));};}
# 4341
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{void*_tmp731=p;int _tmp732;int _tmp733;unsigned int _tmp734;void*_tmp735;struct Cyc_Absyn_Enumfield*_tmp736;struct Cyc_Absyn_Enumdecl*_tmp737;struct Cyc_Absyn_Enumfield*_tmp738;switch(*((int*)_tmp731)){case 3: _LL3D2: _tmp737=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp731)->f1;_tmp738=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp731)->f2;_LL3D3:
# 4345
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmpCD9;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpCD8;e=Cyc_Absyn_new_exp((void*)((_tmpCD8=_cycalloc(sizeof(*_tmpCD8)),((_tmpCD8[0]=((_tmpCD9.tag=31,((_tmpCD9.f1=_tmp738->name,((_tmpCD9.f2=_tmp737,((_tmpCD9.f3=_tmp738,_tmpCD9)))))))),_tmpCD8)))),0);}goto _LL3D1;case 4: _LL3D4: _tmp735=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp731)->f1;_tmp736=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp731)->f2;_LL3D5:
# 4347
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpCDC;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpCDB;e=Cyc_Absyn_new_exp((void*)((_tmpCDB=_cycalloc(sizeof(*_tmpCDB)),((_tmpCDB[0]=((_tmpCDC.tag=32,((_tmpCDC.f1=_tmp736->name,((_tmpCDC.f2=_tmp735,((_tmpCDC.f3=_tmp736,_tmpCDC)))))))),_tmpCDB)))),0);}goto _LL3D1;case 6: _LL3D6: _tmp734=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp731)->f1;_LL3D7:
 _tmp733=(int)_tmp734;goto _LL3D9;case 7: _LL3D8: _tmp733=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp731)->f1;_LL3D9:
 _tmp732=_tmp733;goto _LL3DB;case 8: _LL3DA: _tmp732=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp731)->f2;_LL3DB:
# 4351
 e=Cyc_Absyn_uint_exp((unsigned int)_tmp732,0);
goto _LL3D1;default: _LL3DC: _LL3DD: {
const char*_tmpCDF;void*_tmpCDE;(_tmpCDE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCDF="compile_pat_test_as_case!",_tag_dyneither(_tmpCDF,sizeof(char),26))),_tag_dyneither(_tmpCDE,sizeof(void*),0)));}}_LL3D1:;}{
# 4355
struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct _tmpCE2;struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmpCE1;return Cyc_Absyn_new_pat((void*)((_tmpCE1=_cycalloc(sizeof(*_tmpCE1)),((_tmpCE1[0]=((_tmpCE2.tag=17,((_tmpCE2.f1=e,_tmpCE2)))),_tmpCE1)))),0);};}
# 4359
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0);}
# 4369
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**decls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 4372
void*t=(void*)_check_null(p->topt);
void*_tmp741=p->r;void*_tmp742=_tmp741;union Cyc_Absyn_AggrInfoU _tmp743;struct Cyc_List_List*_tmp744;struct Cyc_List_List*_tmp745;struct Cyc_List_List*_tmp746;struct Cyc_Absyn_Pat*_tmp747;struct Cyc_Absyn_Datatypedecl*_tmp748;struct Cyc_Absyn_Datatypefield*_tmp749;struct Cyc_List_List*_tmp74A;struct Cyc_Absyn_Vardecl*_tmp74B;struct Cyc_Absyn_Pat*_tmp74C;struct _tuple1*_tmp74D;struct _tuple1*_tmp74E;struct Cyc_Absyn_Pat*_tmp74F;struct _tuple1*_tmp750;switch(*((int*)_tmp742)){case 2: _LL3DF: _tmp750=(((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp742)->f2)->name;_LL3E0: {
# 4375
struct Cyc_Absyn_Pat*_tmp751=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0);
_tmp751->topt=p->topt;
_tmp74E=_tmp750;_tmp74F=_tmp751;goto _LL3E2;}case 1: _LL3E1: _tmp74E=(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp742)->f1)->name;_tmp74F=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp742)->f2;_LL3E2: {
# 4380
struct _tuple1*v=Cyc_Toc_temp_var();
void*_tmp752=(void*)_check_null(_tmp74F->topt);
{struct _tuple32*_tmpCE5;struct Cyc_List_List*_tmpCE4;*decls=((_tmpCE4=_region_malloc(rgn,sizeof(*_tmpCE4)),((_tmpCE4->hd=((_tmpCE5=_region_malloc(rgn,sizeof(*_tmpCE5)),((_tmpCE5->f1=v,((_tmpCE5->f2=Cyc_Toc_typ_to_c_array(_tmp752),_tmpCE5)))))),((_tmpCE4->tl=*decls,_tmpCE4))))));}
*nv=Cyc_Toc_add_varmap(rgn,*nv,_tmp74E,Cyc_Absyn_var_exp(v,0));{
struct Cyc_Absyn_Stmt*_tmp755=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(v,0),Cyc_Absyn_copy_exp(path),0);
return Cyc_Toc_seq_stmt_opt(_tmp755,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp74F));};}case 4: _LL3E3: _tmp74D=(((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp742)->f2)->name;_LL3E4: {
# 4388
struct _tuple1*_tmp756=Cyc_Toc_temp_var();
void*_tmp757=(void*)_check_null(p->topt);
{struct _tuple32*_tmpCE8;struct Cyc_List_List*_tmpCE7;*decls=((_tmpCE7=_region_malloc(rgn,sizeof(*_tmpCE7)),((_tmpCE7->hd=((_tmpCE8=_region_malloc(rgn,sizeof(*_tmpCE8)),((_tmpCE8->f1=_tmp756,((_tmpCE8->f2=Cyc_Toc_typ_to_c_array(_tmp757),_tmpCE8)))))),((_tmpCE7->tl=*decls,_tmpCE7))))));}
*nv=Cyc_Toc_add_varmap(rgn,*nv,_tmp74D,Cyc_Absyn_var_exp(_tmp756,0));
return Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp756,0),Cyc_Absyn_copy_exp(path),0);}case 0: _LL3E5: _LL3E6:
 return 0;case 3: _LL3E7: _tmp74B=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp742)->f1;_tmp74C=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp742)->f2;_LL3E8: {
# 4396
struct _tuple1*_tmp75A=Cyc_Toc_temp_var();
{struct _tuple32*_tmpCEB;struct Cyc_List_List*_tmpCEA;*decls=((_tmpCEA=_region_malloc(rgn,sizeof(*_tmpCEA)),((_tmpCEA->hd=((_tmpCEB=_region_malloc(rgn,sizeof(*_tmpCEB)),((_tmpCEB->f1=_tmp75A,((_tmpCEB->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),_tmpCEB)))))),((_tmpCEA->tl=*decls,_tmpCEA))))));}
*nv=Cyc_Toc_add_varmap(rgn,*nv,_tmp74B->name,Cyc_Absyn_var_exp(_tmp75A,0));{
# 4400
struct Cyc_Absyn_Stmt*_tmp75D=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp75A,0),
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path))),0);
return Cyc_Toc_seq_stmt_opt(_tmp75D,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp74C));};}case 9: _LL3E9: _LL3EA:
# 4405
 goto _LL3EC;case 10: _LL3EB: _LL3EC:
 goto _LL3EE;case 11: _LL3ED: _LL3EE:
 goto _LL3F0;case 12: _LL3EF: _LL3F0:
 goto _LL3F2;case 13: _LL3F1: _LL3F2:
 goto _LL3F4;case 14: _LL3F3: _LL3F4:
 return 0;case 6: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp742)->f1)->r)->tag == 8){_LL3F5: _tmp748=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp742)->f1)->r)->f1;_tmp749=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp742)->f1)->r)->f2;_tmp74A=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp742)->f1)->r)->f3;_LL3F6: {
# 4414
struct Cyc_Absyn_Stmt*s=0;
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(_tmp749->name,_tmp748->name);
void*_tmp75E=Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);
int cnt=1;
struct Cyc_List_List*_tmp75F=_tmp749->typs;
path=Cyc_Toc_cast_it(_tmp75E,path);
for(0;_tmp74A != 0;(((_tmp74A=_tmp74A->tl,_tmp75F=((struct Cyc_List_List*)_check_null(_tmp75F))->tl)),++ cnt)){
struct Cyc_Absyn_Pat*_tmp760=(struct Cyc_Absyn_Pat*)_tmp74A->hd;
if(_tmp760->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
void*_tmp761=(*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp75F))->hd)).f2;
void*_tmp762=(void*)_check_null(_tmp760->topt);
void*_tmp763=Cyc_Toc_typ_to_c_array(_tmp762);
struct Cyc_Absyn_Exp*_tmp764=Cyc_Absyn_aggrarrow_exp(path,Cyc_Absyn_fieldname(cnt),0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp761)))
_tmp764=Cyc_Toc_cast_it(_tmp763,_tmp764);
s=Cyc_Toc_seq_stmt_opt(s,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp764,_tmp760));};}
# 4431
return s;}}else{_LL3FF: _tmp747=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp742)->f1;_LL400:
# 4479
 return Cyc_Toc_extract_pattern_vars(rgn,nv,decls,Cyc_Absyn_deref_exp(path,0),_tmp747);}case 8: _LL3F7: _tmp746=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp742)->f3;_LL3F8:
# 4433
 _tmp745=_tmp746;goto _LL3FA;case 5: _LL3F9: _tmp745=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp742)->f1;_LL3FA: {
# 4435
struct Cyc_Absyn_Stmt*s=0;
int cnt=1;
for(0;_tmp745 != 0;(_tmp745=_tmp745->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp765=(struct Cyc_Absyn_Pat*)_tmp745->hd;
if(_tmp765->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp766=(void*)_check_null(_tmp765->topt);
struct _dyneither_ptr*_tmp767=Cyc_Absyn_fieldname(cnt);
s=Cyc_Toc_seq_stmt_opt(s,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,Cyc_Toc_member_exp(path,_tmp767,0),_tmp765));};}
# 4445
return s;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp742)->f1 == 0){_LL3FB: _LL3FC: {
const char*_tmpCEE;void*_tmpCED;(_tmpCED=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCEE="unresolved aggregate pattern!",_tag_dyneither(_tmpCEE,sizeof(char),30))),_tag_dyneither(_tmpCED,sizeof(void*),0)));}}else{_LL3FD: _tmp743=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp742)->f1)->aggr_info;_tmp744=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp742)->f3;_LL3FE: {
# 4449
struct Cyc_Absyn_Aggrdecl*_tmp76A=Cyc_Absyn_get_known_aggrdecl(_tmp743);
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp744 != 0;_tmp744=_tmp744->tl){
struct _tuple33*_tmp76B=(struct _tuple33*)_tmp744->hd;
struct Cyc_Absyn_Pat*_tmp76C=(*_tmp76B).f2;
if(_tmp76C->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*f;
{void*_tmp76D=(void*)((struct Cyc_List_List*)_check_null((*_tmp76B).f1))->hd;void*_tmp76E=_tmp76D;struct _dyneither_ptr*_tmp76F;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp76E)->tag == 1){_LL408: _tmp76F=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp76E)->f1;_LL409:
 f=_tmp76F;goto _LL407;}else{_LL40A: _LL40B: {
struct Cyc_Toc_Match_error_exn_struct _tmpCF1;struct Cyc_Toc_Match_error_exn_struct*_tmpCF0;(int)_throw((void*)((_tmpCF0=_cycalloc_atomic(sizeof(*_tmpCF0)),((_tmpCF0[0]=((_tmpCF1.tag=Cyc_Toc_Match_error,_tmpCF1)),_tmpCF0)))));}}_LL407:;}{
# 4461
void*_tmp772=(void*)_check_null(_tmp76C->topt);
void*_tmp773=Cyc_Toc_typ_to_c_array(_tmp772);
struct Cyc_Absyn_Exp*_tmp774=Cyc_Toc_member_exp(path,f,0);
# 4465
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp76A->impl))->tagged)_tmp774=Cyc_Toc_member_exp(_tmp774,Cyc_Toc_val_sp,0);{
void*_tmp775=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp76A->impl))->fields,f)))->type;
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp775))
_tmp774=Cyc_Toc_cast_it(_tmp773,_tmp774);else{
if(!Cyc_Toc_is_array_type(_tmp775) && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp775)))
# 4472
_tmp774=Cyc_Absyn_deref_exp(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(_tmp773,Cyc_Toc_mt_tq),
Cyc_Absyn_address_exp(_tmp774,0)),0);}
# 4475
s=Cyc_Toc_seq_stmt_opt(s,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp774,_tmp76C));};};};}
# 4477
return s;}}case 15: _LL401: _LL402: {
# 4481
const char*_tmpCF4;void*_tmpCF3;(_tmpCF3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCF4="unknownid pat",_tag_dyneither(_tmpCF4,sizeof(char),14))),_tag_dyneither(_tmpCF3,sizeof(void*),0)));}case 16: _LL403: _LL404: {
const char*_tmpCF7;void*_tmpCF6;(_tmpCF6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCF7="unknowncall pat",_tag_dyneither(_tmpCF7,sizeof(char),16))),_tag_dyneither(_tmpCF6,sizeof(void*),0)));}default: _LL405: _LL406: {
const char*_tmpCFA;void*_tmpCF9;(_tmpCF9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCFA="exp pat",_tag_dyneither(_tmpCFA,sizeof(char),8))),_tag_dyneither(_tmpCF9,sizeof(void*),0)));}}_LL3DE:;}struct _tuple36{struct Cyc_Toc_Env*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Stmt*f3;};
# 4492
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 4500
void*_tmp77C=dopt;struct Cyc_List_List*_tmp77D;struct Cyc_List_List*_tmp77E;void*_tmp77F;struct Cyc_Tcpat_Rhs*_tmp780;if(_tmp77C == 0){_LL40D: _LL40E:
# 4502
 goto _LL410;}else{switch(*((int*)_tmp77C)){case 0: _LL40F: _LL410:
 return Cyc_Absyn_skip_stmt(0);case 1: _LL411: _tmp780=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp77C)->f1;_LL412:
# 4507
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple34*_tmp781=(struct _tuple34*)lscs->hd;int*_tmp783;struct _dyneither_ptr*_tmp784;struct _dyneither_ptr*_tmp785;struct Cyc_Absyn_Switch_clause*_tmp786;struct _tuple34*_tmp782=_tmp781;_tmp783=(int*)& _tmp782->f1;_tmp784=_tmp782->f2;_tmp785=_tmp782->f3;_tmp786=_tmp782->f4;{
struct Cyc_Absyn_Stmt*_tmp787=_tmp786->body;
if(_tmp787 == _tmp780->rhs){
# 4512
if(*_tmp783)
return Cyc_Absyn_goto_stmt(_tmp784,0);
*_tmp783=1;{
# 4516
struct Cyc_Absyn_Stmt*init_opt=Cyc_Toc_extract_pattern_vars(rgn,& nv,decls,Cyc_Absyn_var_exp(v,0),_tmp786->pattern);
# 4519
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_label_stmt(_tmp785,_tmp786->body,0);
if(init_opt != 0)
res=Cyc_Absyn_seq_stmt(init_opt,res,0);
res=Cyc_Absyn_label_stmt(_tmp784,res,0);
{struct _tuple36*_tmpCFD;struct Cyc_List_List*_tmpCFC;*bodies=((_tmpCFC=_region_malloc(rgn,sizeof(*_tmpCFC)),((_tmpCFC->hd=((_tmpCFD=_region_malloc(rgn,sizeof(*_tmpCFD)),((_tmpCFD->f1=nv,((_tmpCFD->f2=_tmp785,((_tmpCFD->f3=_tmp787,_tmpCFD)))))))),((_tmpCFC->tl=*bodies,_tmpCFC))))));}
return res;};}};}{
# 4527
const char*_tmpD00;void*_tmpCFF;(_tmpCFF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD00="couldn't find rhs!",_tag_dyneither(_tmpD00,sizeof(char),19))),_tag_dyneither(_tmpCFF,sizeof(void*),0)));};default: _LL413: _tmp77D=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp77C)->f1;_tmp77E=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp77C)->f2;_tmp77F=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp77C)->f3;_LL414: {
# 4530
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp77F,lscs,v);
# 4532
struct Cyc_Absyn_Exp*_tmp78C=Cyc_Toc_compile_path(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp77D),Cyc_Absyn_var_exp(v,0));
struct Cyc_List_List*_tmp78D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp77E);
# 4535
struct _tuple35 _tmp78E=Cyc_Toc_admits_switch(_tmp78D);int _tmp790;void*_tmp791;struct _tuple35 _tmp78F=_tmp78E;_tmp790=_tmp78F.f1;_tmp791=_tmp78F.f2;
if(_tmp790 > 1){
# 4539
struct Cyc_Absyn_Switch_clause*_tmpD03;struct Cyc_List_List*_tmpD02;struct Cyc_List_List*new_lscs=
(_tmpD02=_cycalloc(sizeof(*_tmpD02)),((_tmpD02->hd=((_tmpD03=_cycalloc(sizeof(*_tmpD03)),((_tmpD03->pattern=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0),((_tmpD03->pat_vars=0,((_tmpD03->where_clause=0,((_tmpD03->body=res,((_tmpD03->loc=0,_tmpD03)))))))))))),((_tmpD02->tl=0,_tmpD02)))));
# 4542
for(0;_tmp78D != 0;_tmp78D=_tmp78D->tl){
struct _tuple29 _tmp792=*((struct _tuple29*)_tmp78D->hd);void*_tmp794;void*_tmp795;struct _tuple29 _tmp793=_tmp792;_tmp794=_tmp793.f1;_tmp795=_tmp793.f2;{
# 4545
struct Cyc_Absyn_Pat*_tmp796=Cyc_Toc_compile_pat_test_as_case(_tmp794);
# 4547
struct Cyc_Absyn_Stmt*_tmp797=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp795,lscs,v);
# 4549
struct Cyc_Absyn_Switch_clause*_tmpD06;struct Cyc_List_List*_tmpD05;new_lscs=((_tmpD05=_cycalloc(sizeof(*_tmpD05)),((_tmpD05->hd=((_tmpD06=_cycalloc(sizeof(*_tmpD06)),((_tmpD06->pattern=_tmp796,((_tmpD06->pat_vars=0,((_tmpD06->where_clause=0,((_tmpD06->body=_tmp797,((_tmpD06->loc=0,_tmpD06)))))))))))),((_tmpD05->tl=new_lscs,_tmpD05))))));};}
# 4551
{void*_tmp79A=_tmp791;struct _dyneither_ptr*_tmp79B;switch(*((int*)_tmp79A)){case 1: _LL416: _LL417:
# 4554
 LOOP1: {
void*_tmp79C=_tmp78C->r;void*_tmp79D=_tmp79C;struct Cyc_Absyn_Exp*_tmp79E;struct Cyc_Absyn_Exp*_tmp79F;switch(*((int*)_tmp79D)){case 13: _LL41F: _tmp79F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp79D)->f2;_LL420:
 _tmp78C=_tmp79F;goto LOOP1;case 19: _LL421: _tmp79E=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp79D)->f1;_LL422:
 _tmp78C=_tmp79E;goto _LL41E;default: _LL423: _LL424:
 goto _LL41E;}_LL41E:;}
# 4560
_tmp78C=Cyc_Absyn_deref_exp(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,Cyc_Toc_mt_tq),_tmp78C),0);goto _LL415;case 3: _LL418: _tmp79B=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_tmp79A)->f1;_LL419:
# 4563
 _tmp78C=Cyc_Toc_member_exp(Cyc_Toc_member_exp(_tmp78C,_tmp79B,0),Cyc_Toc_tag_sp,0);goto _LL415;case 2: _LL41A: _LL41B:
# 4565
 goto _LL415;default: _LL41C: _LL41D:
 goto _LL415;}_LL415:;}{
# 4568
struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct _tmpD09;struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmpD08;res=Cyc_Absyn_new_stmt((void*)((_tmpD08=_cycalloc(sizeof(*_tmpD08)),((_tmpD08[0]=((_tmpD09.tag=10,((_tmpD09.f1=_tmp78C,((_tmpD09.f2=new_lscs,((_tmpD09.f3=0,_tmpD09)))))))),_tmpD08)))),0);};}else{
# 4572
void*_tmp7A4=_tmp791;struct Cyc_Absyn_Exp*_tmp7A5;if(((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp7A4)->tag == 2){_LL426: _tmp7A5=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp7A4)->f1;_LL427: {
# 4574
struct Cyc_List_List*_tmp7A6=_tmp78D;void*_tmp7A7;struct Cyc_Tcpat_Rhs*_tmp7A8;if(_tmp7A6 != 0){if(((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple29*)((struct Cyc_List_List*)_tmp7A6)->hd)->f2)->tag == 1){if(((struct Cyc_List_List*)_tmp7A6)->tl == 0){_LL42B: _tmp7A7=((struct _tuple29*)_tmp7A6->hd)->f1;_tmp7A8=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple29*)_tmp7A6->hd)->f2)->f1;_LL42C:
# 4580
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple34*_tmp7A9=(struct _tuple34*)lscs->hd;int*_tmp7AB;struct _dyneither_ptr*_tmp7AC;struct _dyneither_ptr*_tmp7AD;struct Cyc_Absyn_Switch_clause*_tmp7AE;struct _tuple34*_tmp7AA=_tmp7A9;_tmp7AB=(int*)& _tmp7AA->f1;_tmp7AC=_tmp7AA->f2;_tmp7AD=_tmp7AA->f3;_tmp7AE=_tmp7AA->f4;{
struct Cyc_Absyn_Stmt*_tmp7AF=_tmp7AE->body;
if(_tmp7AF == _tmp7A8->rhs){
# 4585
if(*_tmp7AB)
return Cyc_Absyn_goto_stmt(_tmp7AC,0);
*_tmp7AB=1;{
# 4589
struct Cyc_Absyn_Stmt*init_opt=Cyc_Toc_extract_pattern_vars(rgn,& nv,decls,Cyc_Absyn_var_exp(v,0),_tmp7AE->pattern);
# 4592
Cyc_Toc_exp_to_c(nv,_tmp7A5);{
# 4595
struct Cyc_Absyn_Stmt*r=Cyc_Absyn_label_stmt(_tmp7AD,_tmp7AE->body,0);
r=Cyc_Absyn_ifthenelse_stmt(_tmp7A5,r,res,0);
if(init_opt != 0)
r=Cyc_Absyn_seq_stmt(init_opt,r,0);
r=Cyc_Absyn_label_stmt(_tmp7AC,r,0);
{struct _tuple36*_tmpD0C;struct Cyc_List_List*_tmpD0B;*bodies=((_tmpD0B=_region_malloc(rgn,sizeof(*_tmpD0B)),((_tmpD0B->hd=((_tmpD0C=_region_malloc(rgn,sizeof(*_tmpD0C)),((_tmpD0C->f1=nv,((_tmpD0C->f2=_tmp7AD,((_tmpD0C->f3=_tmp7AF,_tmpD0C)))))))),((_tmpD0B->tl=*bodies,_tmpD0B))))));}
return r;};};}};}{
# 4604
const char*_tmpD0F;void*_tmpD0E;(_tmpD0E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD0F="couldn't find rhs!",_tag_dyneither(_tmpD0F,sizeof(char),19))),_tag_dyneither(_tmpD0E,sizeof(void*),0)));};}else{goto _LL42D;}}else{goto _LL42D;}}else{_LL42D: _LL42E: {
const char*_tmpD12;void*_tmpD11;(_tmpD11=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD12="bad where clause in match compiler",_tag_dyneither(_tmpD12,sizeof(char),35))),_tag_dyneither(_tmpD11,sizeof(void*),0)));}}_LL42A:;}}else{_LL428: _LL429:
# 4609
 for(0;_tmp78D != 0;_tmp78D=_tmp78D->tl){
struct _tuple29 _tmp7B6=*((struct _tuple29*)_tmp78D->hd);void*_tmp7B8;void*_tmp7B9;struct _tuple29 _tmp7B7=_tmp7B6;_tmp7B8=_tmp7B7.f1;_tmp7B9=_tmp7B7.f2;{
struct Cyc_Absyn_Exp*_tmp7BA=Cyc_Toc_compile_pat_test(_tmp78C,_tmp7B8);
struct Cyc_Absyn_Stmt*_tmp7BB=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp7B9,lscs,v);
res=Cyc_Absyn_ifthenelse_stmt(_tmp7BA,_tmp7BB,res,0);};}}_LL425:;}
# 4617
return res;}}}_LL40C:;}
# 4627
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 4629
for(0;bodies != 0;bodies=bodies->tl){
struct _tuple36*_tmp7BC=(struct _tuple36*)bodies->hd;struct Cyc_Toc_Env**_tmp7BE;struct Cyc_Absyn_Stmt*_tmp7BF;struct _tuple36*_tmp7BD=_tmp7BC;_tmp7BE=(struct Cyc_Toc_Env**)& _tmp7BD->f1;_tmp7BF=_tmp7BD->f3;
if(_tmp7BF == s)return _tmp7BE;}
# 4635
return 0;}
# 4639
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 4642
Cyc_Toc_exp_to_c(nv,e);{
void*_tmp7C0=(void*)_check_null(e->topt);
# 4645
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);
r->topt=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));{
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp7C1=_new_region("rgn");struct _RegionHandle*rgn=& _tmp7C1;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp7C2=Cyc_Toc_share_env(rgn,nv);
# 4654
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple34*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 4659
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp7C2,& mydecls,& mybodies,dopt,lscs,v);
# 4667
{struct Cyc_List_List*_tmp7C3=lscs;for(0;_tmp7C3 != 0;_tmp7C3=_tmp7C3->tl){
struct _tuple34*_tmp7C4=(struct _tuple34*)_tmp7C3->hd;struct _dyneither_ptr*_tmp7C6;struct Cyc_Absyn_Switch_clause*_tmp7C7;struct _tuple34*_tmp7C5=_tmp7C4;_tmp7C6=_tmp7C5->f3;_tmp7C7=_tmp7C5->f4;{
struct Cyc_Absyn_Stmt*s=_tmp7C7->body;
# 4671
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 4674
if(envp == 0)continue;{
struct Cyc_Toc_Env*_tmp7C8=*envp;
# 4677
if(_tmp7C3->tl != 0){
struct _tuple34*_tmp7C9=(struct _tuple34*)((struct Cyc_List_List*)_check_null(_tmp7C3->tl))->hd;struct _dyneither_ptr*_tmp7CB;struct Cyc_Absyn_Switch_clause*_tmp7CC;struct _tuple34*_tmp7CA=_tmp7C9;_tmp7CB=_tmp7CA->f3;_tmp7CC=_tmp7CA->f4;{
struct Cyc_Toc_Env**_tmp7CD=Cyc_Toc_find_case_env(mybodies,_tmp7CC->body);
# 4684
if(_tmp7CD == 0)
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn,_tmp7C8,end_l),s);else{
# 4688
struct Cyc_List_List*_tmp7CE=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp7CC->pat_vars))->v)).f1);
_tmp7CE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp7CE);
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(rgn,_tmp7C8,end_l,_tmp7CB,_tmp7CE,*_tmp7CD),s);}};}else{
# 4694
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn,_tmp7C8,end_l),s);}};};}}{
# 4698
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(test_tree,Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),0),0);
# 4700
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct _tuple32*_tmp7CF=(struct _tuple32*)((struct Cyc_List_List*)_check_null(mydecls))->hd;struct _tuple1*_tmp7D1;void*_tmp7D2;struct _tuple32*_tmp7D0=_tmp7CF;_tmp7D1=_tmp7D0->f1;_tmp7D2=_tmp7D0->f2;
res=Cyc_Absyn_declare_stmt(_tmp7D1,_tmp7D2,0,res,0);}
# 4705
whole_s->r=(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),e,res,0))->r;
_npop_handler(0);return;};}
# 4651
;_pop_region(rgn);};};}
# 4711
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 4717
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 4719
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
struct Cyc_List_List*_tmpD13;return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,(
(_tmpD13=_cycalloc(sizeof(*_tmpD13)),((_tmpD13->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0),((_tmpD13->tl=0,_tmpD13)))))),0),0);}
# 4723
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt(s->r,0));}
# 4728
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 4730
while(1){
void*_tmp7D4=s->r;void*_tmp7D5=_tmp7D4;struct Cyc_Absyn_Exp*_tmp7D6;struct Cyc_Absyn_Stmt*_tmp7D7;struct Cyc_List_List*_tmp7D8;void*_tmp7D9;struct Cyc_Absyn_Stmt*_tmp7DA;struct Cyc_Absyn_Exp*_tmp7DB;struct _dyneither_ptr*_tmp7DC;struct Cyc_Absyn_Stmt*_tmp7DD;struct Cyc_Absyn_Decl*_tmp7DE;struct Cyc_Absyn_Stmt*_tmp7DF;struct Cyc_List_List*_tmp7E0;struct Cyc_Absyn_Switch_clause**_tmp7E1;struct Cyc_Absyn_Exp*_tmp7E2;struct Cyc_List_List*_tmp7E3;void*_tmp7E4;struct Cyc_Absyn_Exp*_tmp7E5;struct Cyc_Absyn_Exp*_tmp7E6;struct Cyc_Absyn_Exp*_tmp7E7;struct Cyc_Absyn_Stmt*_tmp7E8;struct Cyc_Absyn_Stmt*_tmp7E9;struct Cyc_Absyn_Stmt*_tmp7EA;struct Cyc_Absyn_Stmt*_tmp7EB;struct Cyc_Absyn_Exp*_tmp7EC;struct Cyc_Absyn_Stmt*_tmp7ED;struct Cyc_Absyn_Exp*_tmp7EE;struct Cyc_Absyn_Stmt*_tmp7EF;struct Cyc_Absyn_Stmt*_tmp7F0;struct Cyc_Absyn_Exp*_tmp7F1;struct Cyc_Absyn_Stmt*_tmp7F2;struct Cyc_Absyn_Stmt*_tmp7F3;struct Cyc_Absyn_Exp*_tmp7F4;switch(*((int*)_tmp7D5)){case 0: _LL430: _LL431:
# 4733
 return;case 1: _LL432: _tmp7F4=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp7D5)->f1;_LL433:
# 4735
 Cyc_Toc_exp_to_c(nv,_tmp7F4);
return;case 2: _LL434: _tmp7F2=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp7D5)->f1;_tmp7F3=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp7D5)->f2;_LL435:
# 4738
 Cyc_Toc_stmt_to_c(nv,_tmp7F2);
s=_tmp7F3;
continue;case 3: _LL436: _tmp7F1=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp7D5)->f1;_LL437: {
# 4742
void*topt=0;
if(_tmp7F1 != 0){
topt=Cyc_Toc_typ_to_c((void*)_check_null(_tmp7F1->topt));
Cyc_Toc_exp_to_c(nv,_tmp7F1);}
# 4748
if(s->try_depth > 0){
if(topt != 0){
struct _tuple1*_tmp7F5=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp7F6=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp7F5,0),0);
s->r=(Cyc_Absyn_declare_stmt(_tmp7F5,topt,_tmp7F1,
Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(s->try_depth),_tmp7F6,0),0))->r;}else{
# 4756
Cyc_Toc_do_npop_before(s->try_depth,s);}}
# 4758
return;}case 4: _LL438: _tmp7EE=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp7D5)->f1;_tmp7EF=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp7D5)->f2;_tmp7F0=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp7D5)->f3;_LL439:
# 4760
 Cyc_Toc_exp_to_c(nv,_tmp7EE);
Cyc_Toc_stmt_to_c(nv,_tmp7EF);
s=_tmp7F0;
continue;case 5: _LL43A: _tmp7EC=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp7D5)->f1).f1;_tmp7ED=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp7D5)->f2;_LL43B:
# 4765
 Cyc_Toc_exp_to_c(nv,_tmp7EC);{
struct _RegionHandle _tmp7F7=_new_region("temp");struct _RegionHandle*temp=& _tmp7F7;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7ED);
# 4769
_npop_handler(0);return;
# 4766
;_pop_region(temp);};case 6: _LL43C: _tmp7EB=((struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp7D5)->f1;_LL43D: {
# 4771
struct _dyneither_ptr**_tmp7F9;struct Cyc_Toc_Env*_tmp7F8=nv;_tmp7F9=_tmp7F8->break_lab;
if(_tmp7F9 != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp7F9,0);{
# 4775
int dest_depth=_tmp7EB == 0?0: _tmp7EB->try_depth;
Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);
return;};}case 7: _LL43E: _tmp7EA=((struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp7D5)->f1;_LL43F: {
# 4779
struct _dyneither_ptr**_tmp7FB;struct Cyc_Toc_Env*_tmp7FA=nv;_tmp7FB=_tmp7FA->continue_lab;
if(_tmp7FB != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp7FB,0);
_tmp7E9=_tmp7EA;goto _LL441;}case 8: _LL440: _tmp7E9=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp7D5)->f2;_LL441:
# 4785
 Cyc_Toc_do_npop_before(s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp7E9))->try_depth,s);
return;case 9: _LL442: _tmp7E5=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp7D5)->f1;_tmp7E6=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp7D5)->f2).f1;_tmp7E7=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp7D5)->f3).f1;_tmp7E8=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp7D5)->f4;_LL443:
# 4789
 Cyc_Toc_exp_to_c(nv,_tmp7E5);Cyc_Toc_exp_to_c(nv,_tmp7E6);Cyc_Toc_exp_to_c(nv,_tmp7E7);{
struct _RegionHandle _tmp7FC=_new_region("temp");struct _RegionHandle*temp=& _tmp7FC;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7E8);
# 4793
_npop_handler(0);return;
# 4790
;_pop_region(temp);};case 10: _LL444: _tmp7E2=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp7D5)->f1;_tmp7E3=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp7D5)->f2;_tmp7E4=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp7D5)->f3;_LL445:
# 4795
 Cyc_Toc_xlate_switch(nv,s,_tmp7E2,_tmp7E3,_tmp7E4);
return;case 11: _LL446: _tmp7E0=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp7D5)->f1;_tmp7E1=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp7D5)->f2;_LL447: {
# 4798
struct Cyc_Toc_FallthruInfo*_tmp7FE;struct Cyc_Toc_Env*_tmp7FD=nv;_tmp7FE=_tmp7FD->fallthru_info;
if(_tmp7FE == 0){
const char*_tmpD16;void*_tmpD15;(_tmpD15=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD16="fallthru in unexpected place",_tag_dyneither(_tmpD16,sizeof(char),29))),_tag_dyneither(_tmpD15,sizeof(void*),0)));}{
struct Cyc_Toc_FallthruInfo _tmp801=*_tmp7FE;struct _dyneither_ptr*_tmp803;struct Cyc_List_List*_tmp804;struct Cyc_Dict_Dict _tmp805;struct Cyc_Toc_FallthruInfo _tmp802=_tmp801;_tmp803=_tmp802.label;_tmp804=_tmp802.binders;_tmp805=_tmp802.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp803,0);
# 4804
Cyc_Toc_do_npop_before(s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp7E1)))->body)->try_depth,s2);{
struct Cyc_List_List*_tmp806=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp804);
struct Cyc_List_List*_tmp807=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7E0);
for(0;_tmp806 != 0;(_tmp806=_tmp806->tl,_tmp807=_tmp807->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp807))->hd);
s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp805,(struct _tuple1*)_tmp806->hd),(struct Cyc_Absyn_Exp*)_tmp807->hd,0),s2,0);}
# 4812
s->r=s2->r;
return;};};};}case 12: _LL448: _tmp7DE=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp7D5)->f1;_tmp7DF=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp7D5)->f2;_LL449:
# 4818
{void*_tmp808=_tmp7DE->r;void*_tmp809=_tmp808;struct Cyc_Absyn_Tvar*_tmp80A;struct Cyc_Absyn_Vardecl*_tmp80B;int _tmp80C;struct Cyc_Absyn_Exp*_tmp80D;struct Cyc_Absyn_Fndecl*_tmp80E;struct Cyc_List_List*_tmp80F;struct Cyc_Absyn_Pat*_tmp810;struct Cyc_Absyn_Exp*_tmp811;void*_tmp812;struct Cyc_Absyn_Vardecl*_tmp813;switch(*((int*)_tmp809)){case 0: _LL453: _tmp813=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp809)->f1;_LL454: {
# 4820
struct _RegionHandle _tmp814=_new_region("temp");struct _RegionHandle*temp=& _tmp814;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpD19;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD18;struct Cyc_Toc_Env*_tmp815=Cyc_Toc_add_varmap(temp,nv,_tmp813->name,
Cyc_Absyn_varb_exp(_tmp813->name,(void*)((_tmpD18=_cycalloc(sizeof(*_tmpD18)),((_tmpD18[0]=((_tmpD19.tag=4,((_tmpD19.f1=_tmp813,_tmpD19)))),_tmpD18)))),0));
Cyc_Toc_local_decl_to_c(_tmp815,_tmp815,_tmp813,_tmp7DF);}
# 4825
_npop_handler(0);goto _LL452;
# 4820
;_pop_region(temp);}case 2: _LL455: _tmp810=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp809)->f1;_tmp811=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp809)->f3;_tmp812=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp809)->f4;_LL456:
# 4829
{void*_tmp818=_tmp810->r;void*_tmp819=_tmp818;struct Cyc_Absyn_Vardecl*_tmp81A;if(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp819)->tag == 1){if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp819)->f2)->r)->tag == 0){_LL460: _tmp81A=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp819)->f1;_LL461: {
# 4831
struct _tuple1*old_name=_tmp81A->name;
struct _tuple1*new_name=Cyc_Toc_temp_var();
_tmp81A->name=new_name;
_tmp81A->initializer=_tmp811;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD1C;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD1B;_tmp7DE->r=(void*)((_tmpD1B=_cycalloc(sizeof(*_tmpD1B)),((_tmpD1B[0]=((_tmpD1C.tag=0,((_tmpD1C.f1=_tmp81A,_tmpD1C)))),_tmpD1B))));}{
struct _RegionHandle _tmp81D=_new_region("temp");struct _RegionHandle*temp=& _tmp81D;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpD1F;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD1E;struct Cyc_Toc_Env*_tmp81E=
Cyc_Toc_add_varmap(temp,nv,old_name,
Cyc_Absyn_varb_exp(new_name,(void*)((_tmpD1E=_cycalloc(sizeof(*_tmpD1E)),((_tmpD1E[0]=((_tmpD1F.tag=4,((_tmpD1F.f1=_tmp81A,_tmpD1F)))),_tmpD1E)))),0));
Cyc_Toc_local_decl_to_c(_tmp81E,nv,_tmp81A,_tmp7DF);}
# 4842
_npop_handler(0);goto _LL45F;
# 4836
;_pop_region(temp);};}}else{goto _LL462;}}else{_LL462: _LL463:
# 4847
 s->r=(Cyc_Toc_letdecl_to_c(nv,_tmp810,(void*)_check_null(_tmp811->topt),_tmp811,_tmp7DF))->r;
goto _LL45F;}_LL45F:;}
# 4850
goto _LL452;case 3: _LL457: _tmp80F=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp809)->f1;_LL458: {
# 4863 "toc.cyc"
struct Cyc_List_List*_tmp821=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp80F);
if(_tmp821 == 0){
const char*_tmpD22;void*_tmpD21;(_tmpD21=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD22="empty Letv_d",_tag_dyneither(_tmpD22,sizeof(char),13))),_tag_dyneither(_tmpD21,sizeof(void*),0)));}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD25;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD24;_tmp7DE->r=(void*)((_tmpD24=_cycalloc(sizeof(*_tmpD24)),((_tmpD24[0]=((_tmpD25.tag=0,((_tmpD25.f1=(struct Cyc_Absyn_Vardecl*)_tmp821->hd,_tmpD25)))),_tmpD24))));}
_tmp821=_tmp821->tl;
for(0;_tmp821 != 0;_tmp821=_tmp821->tl){
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD28;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD27;struct Cyc_Absyn_Decl*_tmp826=Cyc_Absyn_new_decl((void*)((_tmpD27=_cycalloc(sizeof(*_tmpD27)),((_tmpD27[0]=((_tmpD28.tag=0,((_tmpD28.f1=(struct Cyc_Absyn_Vardecl*)_tmp821->hd,_tmpD28)))),_tmpD27)))),0);
s->r=(Cyc_Absyn_decl_stmt(_tmp826,Cyc_Absyn_new_stmt(s->r,0),0))->r;}
# 4872
Cyc_Toc_stmt_to_c(nv,s);
goto _LL452;}case 1: _LL459: _tmp80E=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp809)->f1;_LL45A: {
# 4875
struct _tuple1*_tmp829=_tmp80E->name;
struct _RegionHandle _tmp82A=_new_region("temp");struct _RegionHandle*temp=& _tmp82A;_push_region(temp);{
struct Cyc_Toc_Env*_tmp82B=Cyc_Toc_add_varmap(temp,nv,_tmp80E->name,Cyc_Absyn_var_exp(_tmp829,0));
Cyc_Toc_fndecl_to_c(_tmp82B,_tmp80E,0);
Cyc_Toc_stmt_to_c(_tmp82B,_tmp7DF);}
# 4881
_npop_handler(0);goto _LL452;
# 4876
;_pop_region(temp);}case 4: _LL45B: _tmp80A=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp809)->f1;_tmp80B=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp809)->f2;_tmp80C=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp809)->f3;_tmp80D=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp809)->f4;_LL45C: {
# 4883
struct Cyc_Absyn_Stmt*_tmp82C=_tmp7DF;
# 4891
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple1*rh_var=Cyc_Toc_temp_var();
struct _tuple1*x_var=_tmp80B->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);
# 4898
struct _RegionHandle _tmp82D=_new_region("temp");struct _RegionHandle*temp=& _tmp82D;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(temp,nv,x_var,x_exp),_tmp82C);
# 4901
if(Cyc_Absyn_no_regions)
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_uint_exp(0,0),_tmp82C,0))->r;else{
if((unsigned int)_tmp80D){
Cyc_Toc_exp_to_c(nv,_tmp80D);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(Cyc_Absyn_aggrarrow_exp(_tmp80D,_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"h",sizeof(char),2),0),0);
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,_tmp82C,0))->r;};}else{
# 4909
struct Cyc_Absyn_Exp*_tmpD2B[1];struct Cyc_Absyn_Exp*_tmpD2A[1];struct Cyc_List_List*_tmpD29;s->r=(
Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,(
(_tmpD29=_cycalloc(sizeof(*_tmpD29)),((_tmpD29->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0),((_tmpD29->tl=0,_tmpD29)))))),0),
# 4914
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpD2A[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD2A,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),
Cyc_Absyn_seq_stmt(_tmp82C,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpD2B[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD2B,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}}
# 4920
_npop_handler(0);return;
# 4898
;_pop_region(temp);}default: _LL45D: _LL45E: {
# 4921
const char*_tmpD2E;void*_tmpD2D;(_tmpD2D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD2E="bad nested declaration within function",_tag_dyneither(_tmpD2E,sizeof(char),39))),_tag_dyneither(_tmpD2D,sizeof(void*),0)));}}_LL452:;}
# 4923
return;case 13: _LL44A: _tmp7DC=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp7D5)->f1;_tmp7DD=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp7D5)->f2;_LL44B:
# 4925
 s=_tmp7DD;continue;case 14: _LL44C: _tmp7DA=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp7D5)->f1;_tmp7DB=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp7D5)->f2).f1;_LL44D: {
# 4927
struct _RegionHandle _tmp834=_new_region("temp");struct _RegionHandle*temp=& _tmp834;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7DA);
Cyc_Toc_exp_to_c(nv,_tmp7DB);
# 4931
_npop_handler(0);return;
# 4927
;_pop_region(temp);}case 15: _LL44E: _tmp7D7=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp7D5)->f1;_tmp7D8=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp7D5)->f2;_tmp7D9=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp7D5)->f3;_LL44F: {
# 4945 "toc.cyc"
struct _tuple1*h_var=Cyc_Toc_temp_var();
struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
# 4956
e_exp->topt=e_typ;{
struct _RegionHandle _tmp835=_new_region("temp");struct _RegionHandle*temp=& _tmp835;_push_region(temp);{
struct Cyc_Toc_Env*_tmp836=Cyc_Toc_add_varmap(temp,nv,e_var,e_exp);
# 4960
Cyc_Toc_stmt_to_c(_tmp836,_tmp7D7);{
struct Cyc_Absyn_Stmt*_tmp837=Cyc_Absyn_seq_stmt(_tmp7D7,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
# 4965
struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct _tmpD31;struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmpD30;struct Cyc_Absyn_Stmt*_tmp838=Cyc_Absyn_new_stmt((void*)((_tmpD30=_cycalloc(sizeof(*_tmpD30)),((_tmpD30[0]=((_tmpD31.tag=10,((_tmpD31.f1=e_exp,((_tmpD31.f2=_tmp7D8,((_tmpD31.f3=_tmp7D9,_tmpD31)))))))),_tmpD30)))),0);
# 4967
Cyc_Toc_stmt_to_c(_tmp836,_tmp838);{
# 4970
struct Cyc_List_List*_tmpD32;struct Cyc_Absyn_Exp*_tmp839=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,(
(_tmpD32=_cycalloc(sizeof(*_tmpD32)),((_tmpD32->hd=Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0),((_tmpD32->tl=0,_tmpD32)))))),0);
# 4974
struct Cyc_List_List*_tmpD33;struct Cyc_Absyn_Stmt*_tmp83A=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,(
(_tmpD33=_cycalloc(sizeof(*_tmpD33)),((_tmpD33->hd=Cyc_Absyn_address_exp(h_exp,0),((_tmpD33->tl=0,_tmpD33)))))),0),0);
# 4978
struct Cyc_Absyn_Exp*_tmp83B=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0);
struct Cyc_Absyn_Exp*_tmp83C=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0);
s->r=(
Cyc_Absyn_declare_stmt(h_var,h_typ,0,
Cyc_Absyn_seq_stmt(_tmp83A,
Cyc_Absyn_declare_stmt(was_thrown_var,was_thrown_typ,_tmp83B,
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp839,
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp83C,0),
Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0),_tmp837,
# 4989
Cyc_Absyn_declare_stmt(e_var,e_typ,
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp838,0),0),0),0),0),0))->r;};};}
# 4993
_npop_handler(0);return;
# 4957
;_pop_region(temp);};}default: _LL450: _tmp7D6=((struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp7D5)->f1;_LL451:
# 4995
 if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{
# 4998
Cyc_Toc_exp_to_c(nv,_tmp7D6);{
struct Cyc_List_List*_tmpD34;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((_tmpD34=_cycalloc(sizeof(*_tmpD34)),((_tmpD34->hd=_tmp7D6,((_tmpD34->tl=0,_tmpD34)))))),0));};}
# 5001
return;}_LL42F:;}}
# 5010
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->requires_clause=0;
f->ensures_clause=0;
f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp842=_new_region("frgn");struct _RegionHandle*frgn=& _tmp842;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp843=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp844=f->args;for(0;_tmp844 != 0;_tmp844=_tmp844->tl){
struct _tuple1*_tmpD35;struct _tuple1*_tmp845=(_tmpD35=_cycalloc(sizeof(*_tmpD35)),((_tmpD35->f1=Cyc_Absyn_Loc_n,((_tmpD35->f2=(*((struct _tuple9*)_tmp844->hd)).f1,_tmpD35)))));
(*((struct _tuple9*)_tmp844->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp844->hd)).f3);
_tmp843=Cyc_Toc_add_varmap(frgn,_tmp843,_tmp845,Cyc_Absyn_var_exp(_tmp845,0));}}
# 5027
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}
# 5031
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp847=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));struct _dyneither_ptr*_tmp849;struct Cyc_Absyn_Tqual _tmp84A;void*_tmp84B;int _tmp84C;struct Cyc_Absyn_VarargInfo _tmp848=_tmp847;_tmp849=_tmp848.name;_tmp84A=_tmp848.tq;_tmp84B=_tmp848.type;_tmp84C=_tmp848.inject;{
void*_tmp84D=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp84B,(void*)& Cyc_Absyn_HeapRgn_val,_tmp84A,Cyc_Absyn_false_conref));
struct _tuple1*_tmpD36;struct _tuple1*_tmp84E=(_tmpD36=_cycalloc(sizeof(*_tmpD36)),((_tmpD36->f1=Cyc_Absyn_Loc_n,((_tmpD36->f2=(struct _dyneither_ptr*)_check_null(_tmp849),_tmpD36)))));
{struct _tuple9*_tmpD39;struct Cyc_List_List*_tmpD38;f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpD38=_cycalloc(sizeof(*_tmpD38)),((_tmpD38->hd=((_tmpD39=_cycalloc(sizeof(*_tmpD39)),((_tmpD39->f1=_tmp849,((_tmpD39->f2=_tmp84A,((_tmpD39->f3=_tmp84D,_tmpD39)))))))),((_tmpD38->tl=0,_tmpD38)))))));}
_tmp843=Cyc_Toc_add_varmap(frgn,_tmp843,_tmp84E,Cyc_Absyn_var_exp(_tmp84E,0));
f->cyc_varargs=0;};}
# 5040
{struct Cyc_List_List*_tmp852=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp852 != 0;_tmp852=_tmp852->tl){
((struct Cyc_Absyn_Vardecl*)_tmp852->hd)->type=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp852->hd)->type);}}
# 5043
Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp843),f->body);}
# 5019
;_pop_region(frgn);};}
# 5048
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
enum Cyc_Absyn_Scope _tmp853=s;switch(_tmp853){case Cyc_Absyn_Abstract: _LL465: _LL466:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL467: _LL468:
 return Cyc_Absyn_Extern;default: _LL469: _LL46A:
 return s;}_LL464:;}struct _tuple37{struct Cyc_Absyn_Aggrdecl*f1;int f2;};struct _tuple38{struct Cyc_Toc_TocState*f1;struct _tuple37*f2;};
# 5065 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple38*env){
# 5068
struct _tuple38 _tmp854=*env;struct Cyc_Toc_TocState*_tmp856;struct Cyc_Absyn_Aggrdecl*_tmp857;int _tmp858;struct _tuple38 _tmp855=_tmp854;_tmp856=_tmp855.f1;_tmp857=(_tmp855.f2)->f1;_tmp858=(_tmp855.f2)->f2;{
struct _tuple1*_tmp859=_tmp857->name;
struct Cyc_Toc_TocState _tmp85A=*_tmp856;struct Cyc_Dict_Dict*_tmp85C;struct Cyc_Toc_TocState _tmp85B=_tmp85A;_tmp85C=_tmp85B.aggrs_so_far;{
int seen_defn_before;
struct _tuple18**_tmp85D=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp85C,_tmp859);
if(_tmp85D == 0){
seen_defn_before=0;{
struct _tuple18*v;
if(_tmp857->kind == Cyc_Absyn_StructA){
struct _tuple18*_tmpD3A;v=((_tmpD3A=_region_malloc(d,sizeof(*_tmpD3A)),((_tmpD3A->f1=_tmp857,((_tmpD3A->f2=Cyc_Absyn_strctq(_tmp859),_tmpD3A))))));}else{
# 5079
struct _tuple18*_tmpD3B;v=((_tmpD3B=_region_malloc(d,sizeof(*_tmpD3B)),((_tmpD3B->f1=_tmp857,((_tmpD3B->f2=Cyc_Absyn_unionq_typ(_tmp859),_tmpD3B))))));}
*_tmp85C=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp85C,_tmp859,v);};}else{
# 5082
struct _tuple18*_tmp860=*_tmp85D;struct Cyc_Absyn_Aggrdecl*_tmp862;void*_tmp863;struct _tuple18*_tmp861=_tmp860;_tmp862=_tmp861->f1;_tmp863=_tmp861->f2;
if(_tmp862->impl == 0){
{struct _tuple18*_tmpD3C;*_tmp85C=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp85C,_tmp859,((_tmpD3C=_region_malloc(d,sizeof(*_tmpD3C)),((_tmpD3C->f1=_tmp857,((_tmpD3C->f2=_tmp863,_tmpD3C)))))));}
seen_defn_before=0;}else{
# 5087
seen_defn_before=1;}}{
# 5089
struct Cyc_Absyn_Aggrdecl*_tmpD3D;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpD3D=_cycalloc(sizeof(*_tmpD3D)),((_tmpD3D->kind=_tmp857->kind,((_tmpD3D->sc=Cyc_Absyn_Public,((_tmpD3D->name=_tmp857->name,((_tmpD3D->tvs=0,((_tmpD3D->impl=0,((_tmpD3D->expected_mem_kind=0,((_tmpD3D->attributes=_tmp857->attributes,_tmpD3D)))))))))))))));
# 5096
if(_tmp857->impl != 0  && !seen_defn_before){
{struct Cyc_Absyn_AggrdeclImpl*_tmpD3E;new_ad->impl=((_tmpD3E=_cycalloc(sizeof(*_tmpD3E)),((_tmpD3E->exist_vars=0,((_tmpD3E->rgn_po=0,((_tmpD3E->fields=0,((_tmpD3E->tagged=0,_tmpD3E))))))))));}{
# 5101
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp866=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp857->impl))->fields;for(0;_tmp866 != 0;_tmp866=_tmp866->tl){
# 5105
struct Cyc_Absyn_Aggrfield*_tmp867=(struct Cyc_Absyn_Aggrfield*)_tmp866->hd;
void*_tmp868=_tmp867->type;
struct Cyc_List_List*_tmp869=_tmp867->attributes;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp868)) && (
_tmp857->kind == Cyc_Absyn_StructA  && _tmp866->tl == 0  || _tmp857->kind == Cyc_Absyn_UnionA)){
# 5119 "toc.cyc"
void*_tmp86A=Cyc_Tcutil_compress(_tmp868);void*_tmp86B=_tmp86A;void*_tmp86C;struct Cyc_Absyn_Tqual _tmp86D;union Cyc_Absyn_Constraint*_tmp86E;unsigned int _tmp86F;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp86B)->tag == 8){_LL46C: _tmp86C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp86B)->f1).elt_type;_tmp86D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp86B)->f1).tq;_tmp86E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp86B)->f1).zero_term;_tmp86F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp86B)->f1).zt_loc;_LL46D:
# 5122
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpD44;struct Cyc_Absyn_ArrayInfo _tmpD43;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD42;_tmp868=(void*)((_tmpD42=_cycalloc(sizeof(*_tmpD42)),((_tmpD42[0]=((_tmpD44.tag=8,((_tmpD44.f1=((_tmpD43.elt_type=_tmp86C,((_tmpD43.tq=_tmp86D,((_tmpD43.num_elts=Cyc_Absyn_uint_exp(0,0),((_tmpD43.zero_term=_tmp86E,((_tmpD43.zt_loc=_tmp86F,_tmpD43)))))))))),_tmpD44)))),_tmpD42))));}
goto _LL46B;}else{_LL46E: _LL46F:
# 5125
{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpD4A;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpD49;struct Cyc_List_List*_tmpD48;_tmp869=((_tmpD48=_cycalloc(sizeof(*_tmpD48)),((_tmpD48->hd=(void*)((_tmpD4A=_cycalloc(sizeof(*_tmpD4A)),((_tmpD4A[0]=((_tmpD49.tag=6,((_tmpD49.f1=0,_tmpD49)))),_tmpD4A)))),((_tmpD48->tl=_tmp869,_tmpD48))))));}{
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpD50;struct Cyc_Absyn_ArrayInfo _tmpD4F;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD4E;_tmp868=(void*)((_tmpD4E=_cycalloc(sizeof(*_tmpD4E)),((_tmpD4E[0]=((_tmpD50.tag=8,((_tmpD50.f1=((_tmpD4F.elt_type=Cyc_Absyn_void_star_typ(),((_tmpD4F.tq=
Cyc_Absyn_empty_tqual(0),((_tmpD4F.num_elts=
Cyc_Absyn_uint_exp(0,0),((_tmpD4F.zero_term=
((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0),((_tmpD4F.zt_loc=0,_tmpD4F)))))))))),_tmpD50)))),_tmpD4E))));};}_LL46B:;}{
# 5132
struct Cyc_Absyn_Aggrfield*_tmpD51;struct Cyc_Absyn_Aggrfield*_tmp879=(_tmpD51=_cycalloc(sizeof(*_tmpD51)),((_tmpD51->name=_tmp867->name,((_tmpD51->tq=Cyc_Toc_mt_tq,((_tmpD51->type=
# 5134
Cyc_Toc_typ_to_c(_tmp868),((_tmpD51->width=_tmp867->width,((_tmpD51->attributes=_tmp869,((_tmpD51->requires_clause=0,_tmpD51)))))))))))));
# 5142
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp857->impl))->tagged){
void*_tmp87A=_tmp879->type;
struct _dyneither_ptr*_tmp87B=_tmp879->name;
const char*_tmpD56;void*_tmpD55[2];struct Cyc_String_pa_PrintArg_struct _tmpD54;struct Cyc_String_pa_PrintArg_struct _tmpD53;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpD53.tag=0,((_tmpD53.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp87B),((_tmpD54.tag=0,((_tmpD54.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp857->name).f2),((_tmpD55[0]=& _tmpD54,((_tmpD55[1]=& _tmpD53,Cyc_aprintf(((_tmpD56="_union_%s_%s",_tag_dyneither(_tmpD56,sizeof(char),13))),_tag_dyneither(_tmpD55,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpD57;struct _dyneither_ptr*str=(_tmpD57=_cycalloc(sizeof(*_tmpD57)),((_tmpD57[0]=s,_tmpD57)));
struct Cyc_Absyn_Aggrfield*_tmpD58;struct Cyc_Absyn_Aggrfield*_tmp87C=(_tmpD58=_cycalloc(sizeof(*_tmpD58)),((_tmpD58->name=Cyc_Toc_val_sp,((_tmpD58->tq=Cyc_Toc_mt_tq,((_tmpD58->type=_tmp87A,((_tmpD58->width=0,((_tmpD58->attributes=0,((_tmpD58->requires_clause=0,_tmpD58)))))))))))));
struct Cyc_Absyn_Aggrfield*_tmpD59;struct Cyc_Absyn_Aggrfield*_tmp87D=(_tmpD59=_cycalloc(sizeof(*_tmpD59)),((_tmpD59->name=Cyc_Toc_tag_sp,((_tmpD59->tq=Cyc_Toc_mt_tq,((_tmpD59->type=Cyc_Absyn_sint_typ,((_tmpD59->width=0,((_tmpD59->attributes=0,((_tmpD59->requires_clause=0,_tmpD59)))))))))))));
struct Cyc_Absyn_Aggrfield*_tmpD5A[2];struct Cyc_List_List*_tmp87E=(_tmpD5A[1]=_tmp87C,((_tmpD5A[0]=_tmp87D,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD5A,sizeof(struct Cyc_Absyn_Aggrfield*),2)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpD5F;struct _tuple1*_tmpD5E;struct Cyc_Absyn_Aggrdecl*_tmpD5D;struct Cyc_Absyn_Aggrdecl*_tmp87F=(_tmpD5D=_cycalloc(sizeof(*_tmpD5D)),((_tmpD5D->kind=Cyc_Absyn_StructA,((_tmpD5D->sc=Cyc_Absyn_Public,((_tmpD5D->name=(
(_tmpD5E=_cycalloc(sizeof(*_tmpD5E)),((_tmpD5E->f1=Cyc_Absyn_Loc_n,((_tmpD5E->f2=str,_tmpD5E)))))),((_tmpD5D->tvs=0,((_tmpD5D->impl=(
(_tmpD5F=_cycalloc(sizeof(*_tmpD5F)),((_tmpD5F->exist_vars=0,((_tmpD5F->rgn_po=0,((_tmpD5F->fields=_tmp87E,((_tmpD5F->tagged=0,_tmpD5F)))))))))),((_tmpD5D->expected_mem_kind=0,((_tmpD5D->attributes=0,_tmpD5D)))))))))))))));
# 5155
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD65;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD64;struct Cyc_List_List*_tmpD63;Cyc_Toc_result_decls=((_tmpD63=_cycalloc(sizeof(*_tmpD63)),((_tmpD63->hd=Cyc_Absyn_new_decl((void*)((_tmpD65=_cycalloc(sizeof(*_tmpD65)),((_tmpD65[0]=((_tmpD64.tag=5,((_tmpD64.f1=_tmp87F,_tmpD64)))),_tmpD65)))),0),((_tmpD63->tl=Cyc_Toc_result_decls,_tmpD63))))));}
_tmp879->type=Cyc_Absyn_strct(str);}{
# 5158
struct Cyc_List_List*_tmpD66;new_fields=((_tmpD66=_cycalloc(sizeof(*_tmpD66)),((_tmpD66->hd=_tmp879,((_tmpD66->tl=new_fields,_tmpD66))))));};};}}
# 5160
(new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);};}
# 5162
if(_tmp858){
struct Cyc_Absyn_Decl*_tmpD70;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD6F;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD6E;struct Cyc_List_List*_tmpD6D;Cyc_Toc_result_decls=((_tmpD6D=_cycalloc(sizeof(*_tmpD6D)),((_tmpD6D->hd=((_tmpD70=_cycalloc(sizeof(*_tmpD70)),((_tmpD70->r=(void*)((_tmpD6E=_cycalloc(sizeof(*_tmpD6E)),((_tmpD6E[0]=((_tmpD6F.tag=5,((_tmpD6F.f1=new_ad,_tmpD6F)))),_tmpD6E)))),((_tmpD70->loc=0,_tmpD70)))))),((_tmpD6D->tl=Cyc_Toc_result_decls,_tmpD6D))))));}
return 0;};};};}
# 5167
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple37 _tmpD71;struct _tuple37 p=(_tmpD71.f1=ad,((_tmpD71.f2=add_to_decls,_tmpD71)));
((int(*)(struct _tuple37*arg,int(*f)(struct _RegionHandle*,struct _tuple38*)))Cyc_Toc_use_toc_state)(& p,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple39{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 5196 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple39*env){
# 5199
struct _tuple39 _tmp896=*env;struct Cyc_Set_Set**_tmp898;struct Cyc_Absyn_Datatypedecl*_tmp899;struct _tuple39 _tmp897=_tmp896;_tmp898=(_tmp897.f1)->datatypes_so_far;_tmp899=_tmp897.f2;{
struct _tuple1*_tmp89A=_tmp899->name;
if(_tmp899->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*_tmp898,_tmp89A))
return 0;
*_tmp898=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*_tmp898,_tmp89A);
{struct Cyc_List_List*_tmp89B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp899->fields))->v;for(0;_tmp89B != 0;_tmp89B=_tmp89B->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp89B->hd;
# 5207
struct Cyc_List_List*_tmp89C=0;
int i=1;
{struct Cyc_List_List*_tmp89D=f->typs;for(0;_tmp89D != 0;(_tmp89D=_tmp89D->tl,i ++)){
struct _dyneither_ptr*_tmp89E=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmpD72;struct Cyc_Absyn_Aggrfield*_tmp89F=(_tmpD72=_cycalloc(sizeof(*_tmpD72)),((_tmpD72->name=_tmp89E,((_tmpD72->tq=(*((struct _tuple11*)_tmp89D->hd)).f1,((_tmpD72->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp89D->hd)).f2),((_tmpD72->width=0,((_tmpD72->attributes=0,((_tmpD72->requires_clause=0,_tmpD72)))))))))))));
struct Cyc_List_List*_tmpD73;_tmp89C=((_tmpD73=_cycalloc(sizeof(*_tmpD73)),((_tmpD73->hd=_tmp89F,((_tmpD73->tl=_tmp89C,_tmpD73))))));}}
# 5215
{struct Cyc_Absyn_Aggrfield*_tmpD76;struct Cyc_List_List*_tmpD75;_tmp89C=((_tmpD75=_cycalloc(sizeof(*_tmpD75)),((_tmpD75->hd=((_tmpD76=_cycalloc(sizeof(*_tmpD76)),((_tmpD76->name=Cyc_Toc_tag_sp,((_tmpD76->tq=Cyc_Toc_mt_tq,((_tmpD76->type=Cyc_Absyn_sint_typ,((_tmpD76->width=0,((_tmpD76->attributes=0,((_tmpD76->requires_clause=0,_tmpD76)))))))))))))),((_tmpD75->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp89C),_tmpD75))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpD79;struct Cyc_Absyn_Aggrdecl*_tmpD78;struct Cyc_Absyn_Aggrdecl*_tmp8A4=
(_tmpD78=_cycalloc(sizeof(*_tmpD78)),((_tmpD78->kind=Cyc_Absyn_StructA,((_tmpD78->sc=Cyc_Absyn_Public,((_tmpD78->name=
Cyc_Toc_collapse_qvars(f->name,_tmp899->name),((_tmpD78->tvs=0,((_tmpD78->expected_mem_kind=0,((_tmpD78->impl=(
# 5221
(_tmpD79=_cycalloc(sizeof(*_tmpD79)),((_tmpD79->exist_vars=0,((_tmpD79->rgn_po=0,((_tmpD79->fields=_tmp89C,((_tmpD79->tagged=0,_tmpD79)))))))))),((_tmpD78->attributes=0,_tmpD78)))))))))))))));
# 5223
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD7F;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD7E;struct Cyc_List_List*_tmpD7D;Cyc_Toc_result_decls=((_tmpD7D=_cycalloc(sizeof(*_tmpD7D)),((_tmpD7D->hd=Cyc_Absyn_new_decl((void*)((_tmpD7F=_cycalloc(sizeof(*_tmpD7F)),((_tmpD7F[0]=((_tmpD7E.tag=5,((_tmpD7E.f1=_tmp8A4,_tmpD7E)))),_tmpD7F)))),0),((_tmpD7D->tl=Cyc_Toc_result_decls,_tmpD7D))))));};}}
# 5225
return 0;};}
# 5228
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple39*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 5247 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple39*env){
# 5250
struct _tuple39 _tmp8AA=*env;struct Cyc_Toc_TocState*_tmp8AC;struct Cyc_Absyn_Datatypedecl*_tmp8AD;struct _tuple39 _tmp8AB=_tmp8AA;_tmp8AC=_tmp8AB.f1;_tmp8AD=_tmp8AB.f2;
if(_tmp8AD->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp8AE=*_tmp8AC;struct Cyc_Dict_Dict*_tmp8B0;struct Cyc_Toc_TocState _tmp8AF=_tmp8AE;_tmp8B0=_tmp8AF.xdatatypes_so_far;{
struct _tuple1*_tmp8B1=_tmp8AD->name;
{struct Cyc_List_List*_tmp8B2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp8AD->fields))->v;for(0;_tmp8B2 != 0;_tmp8B2=_tmp8B2->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp8B2->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp8B3=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0);
void*_tmp8B4=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,_tmp8B3,Cyc_Absyn_false_conref,0);
# 5261
int*_tmp8B5=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp8B0,f->name);int*_tmp8B6=_tmp8B5;if(_tmp8B6 == 0){_LL471: _LL472: {
# 5263
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0);{
# 5267
struct Cyc_Absyn_Vardecl*_tmp8B7=Cyc_Absyn_new_vardecl(f->name,_tmp8B4,initopt);
_tmp8B7->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD85;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD84;struct Cyc_List_List*_tmpD83;Cyc_Toc_result_decls=((_tmpD83=_cycalloc(sizeof(*_tmpD83)),((_tmpD83->hd=Cyc_Absyn_new_decl((void*)((_tmpD85=_cycalloc(sizeof(*_tmpD85)),((_tmpD85[0]=((_tmpD84.tag=0,((_tmpD84.f1=_tmp8B7,_tmpD84)))),_tmpD85)))),0),((_tmpD83->tl=Cyc_Toc_result_decls,_tmpD83))))));}
*_tmp8B0=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp8B0,f->name,f->sc != Cyc_Absyn_Extern);{
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp8BB=f->typs;for(0;_tmp8BB != 0;(_tmp8BB=_tmp8BB->tl,i ++)){
struct Cyc_Int_pa_PrintArg_struct _tmpD8D;void*_tmpD8C[1];const char*_tmpD8B;struct _dyneither_ptr*_tmpD8A;struct _dyneither_ptr*_tmp8BC=(_tmpD8A=_cycalloc(sizeof(*_tmpD8A)),((_tmpD8A[0]=(struct _dyneither_ptr)((_tmpD8D.tag=1,((_tmpD8D.f1=(unsigned long)i,((_tmpD8C[0]=& _tmpD8D,Cyc_aprintf(((_tmpD8B="f%d",_tag_dyneither(_tmpD8B,sizeof(char),4))),_tag_dyneither(_tmpD8C,sizeof(void*),1)))))))),_tmpD8A)));
struct Cyc_Absyn_Aggrfield*_tmpD8E;struct Cyc_Absyn_Aggrfield*_tmp8BD=(_tmpD8E=_cycalloc(sizeof(*_tmpD8E)),((_tmpD8E->name=_tmp8BC,((_tmpD8E->tq=(*((struct _tuple11*)_tmp8BB->hd)).f1,((_tmpD8E->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp8BB->hd)).f2),((_tmpD8E->width=0,((_tmpD8E->attributes=0,((_tmpD8E->requires_clause=0,_tmpD8E)))))))))))));
struct Cyc_List_List*_tmpD8F;fields=((_tmpD8F=_cycalloc(sizeof(*_tmpD8F)),((_tmpD8F->hd=_tmp8BD,((_tmpD8F->tl=fields,_tmpD8F))))));}}
# 5280
{struct Cyc_Absyn_Aggrfield*_tmpD92;struct Cyc_List_List*_tmpD91;fields=((_tmpD91=_cycalloc(sizeof(*_tmpD91)),((_tmpD91->hd=((_tmpD92=_cycalloc(sizeof(*_tmpD92)),((_tmpD92->name=Cyc_Toc_tag_sp,((_tmpD92->tq=Cyc_Toc_mt_tq,((_tmpD92->type=
Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq),((_tmpD92->width=0,((_tmpD92->attributes=0,((_tmpD92->requires_clause=0,_tmpD92)))))))))))))),((_tmpD91->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmpD91))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpD95;struct Cyc_Absyn_Aggrdecl*_tmpD94;struct Cyc_Absyn_Aggrdecl*_tmp8C6=
(_tmpD94=_cycalloc(sizeof(*_tmpD94)),((_tmpD94->kind=Cyc_Absyn_StructA,((_tmpD94->sc=Cyc_Absyn_Public,((_tmpD94->name=
Cyc_Toc_collapse_qvars(f->name,_tmp8AD->name),((_tmpD94->tvs=0,((_tmpD94->expected_mem_kind=0,((_tmpD94->impl=(
# 5287
(_tmpD95=_cycalloc(sizeof(*_tmpD95)),((_tmpD95->exist_vars=0,((_tmpD95->rgn_po=0,((_tmpD95->fields=fields,((_tmpD95->tagged=0,_tmpD95)))))))))),((_tmpD94->attributes=0,_tmpD94)))))))))))))));
# 5289
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD9B;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD9A;struct Cyc_List_List*_tmpD99;Cyc_Toc_result_decls=((_tmpD99=_cycalloc(sizeof(*_tmpD99)),((_tmpD99->hd=Cyc_Absyn_new_decl((void*)((_tmpD9B=_cycalloc(sizeof(*_tmpD9B)),((_tmpD9B[0]=((_tmpD9A.tag=5,((_tmpD9A.f1=_tmp8C6,_tmpD9A)))),_tmpD9B)))),0),((_tmpD99->tl=Cyc_Toc_result_decls,_tmpD99))))));}
# 5291
goto _LL470;};};};}}else{if(*((int*)_tmp8B6)== 0){_LL473: _LL474:
# 5293
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp8CC=Cyc_Absyn_string_exp(*fn,0);
struct Cyc_Absyn_Vardecl*_tmp8CD=Cyc_Absyn_new_vardecl(f->name,_tmp8B4,_tmp8CC);
_tmp8CD->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpDA1;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpDA0;struct Cyc_List_List*_tmpD9F;Cyc_Toc_result_decls=((_tmpD9F=_cycalloc(sizeof(*_tmpD9F)),((_tmpD9F->hd=Cyc_Absyn_new_decl((void*)((_tmpDA1=_cycalloc(sizeof(*_tmpDA1)),((_tmpDA1[0]=((_tmpDA0.tag=0,((_tmpDA0.f1=_tmp8CD,_tmpDA0)))),_tmpDA1)))),0),((_tmpD9F->tl=Cyc_Toc_result_decls,_tmpD9F))))));}
*_tmp8B0=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp8B0,f->name,1);}
# 5300
goto _LL470;}else{_LL475: _LL476:
 goto _LL470;}}_LL470:;}}
# 5304
return 0;};};}
# 5307
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple39*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 5312
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 5320
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
vd->type=Cyc_Toc_typ_to_c(old_typ);
# 5324
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(body_nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp8D1=init->r;void*_tmp8D2=_tmp8D1;struct Cyc_Absyn_Vardecl*_tmp8D3;struct Cyc_Absyn_Exp*_tmp8D4;struct Cyc_Absyn_Exp*_tmp8D5;int _tmp8D6;if(((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp8D2)->tag == 26){_LL478: _tmp8D3=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp8D2)->f1;_tmp8D4=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp8D2)->f2;_tmp8D5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp8D2)->f3;_tmp8D6=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp8D2)->f4;_LL479:
# 5331
 vd->initializer=0;
s->r=(Cyc_Toc_init_comprehension(init_nv,old_typ,Cyc_Absyn_var_exp(vd->name,0),_tmp8D3,_tmp8D4,_tmp8D5,_tmp8D6,
Cyc_Absyn_new_stmt(s->r,0),0))->r;
goto _LL477;}else{_LL47A: _LL47B:
# 5336
 if(vd->sc == Cyc_Absyn_Static){
# 5340
struct _RegionHandle _tmp8D7=_new_region("temp");struct _RegionHandle*temp=& _tmp8D7;_push_region(temp);
{struct Cyc_Toc_Env*_tmp8D8=Cyc_Toc_set_toplevel(temp,init_nv);
Cyc_Toc_exp_to_c(_tmp8D8,init);}
# 5341
;_pop_region(temp);}else{
# 5345
Cyc_Toc_exp_to_c(init_nv,init);}
goto _LL477;}_LL477:;}else{
# 5350
void*_tmp8D9=Cyc_Tcutil_compress(old_typ);void*_tmp8DA=_tmp8D9;void*_tmp8DB;struct Cyc_Absyn_Exp*_tmp8DC;union Cyc_Absyn_Constraint*_tmp8DD;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8DA)->tag == 8){_LL47D: _tmp8DB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8DA)->f1).elt_type;_tmp8DC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8DA)->f1).num_elts;_tmp8DD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8DA)->f1).zero_term;if(
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8DD)){_LL47E:
 if(_tmp8DC == 0){
const char*_tmpDA4;void*_tmpDA3;(_tmpDA3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDA4="can't initialize zero-terminated array -- size unknown",_tag_dyneither(_tmpDA4,sizeof(char),55))),_tag_dyneither(_tmpDA3,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=_tmp8DC;
struct Cyc_Absyn_Exp*_tmp8E0=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),
Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);
# 5358
struct Cyc_Absyn_Exp*_tmp8E1=Cyc_Absyn_signed_int_exp(0,0);
s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp8E0,_tmp8E1,0),0),
Cyc_Absyn_new_stmt(s->r,0));
goto _LL47C;};}else{goto _LL47F;}}else{_LL47F: _LL480:
 goto _LL47C;}_LL47C:;}}
# 5372
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*x=Cyc_Toc_temp_var();
struct _RegionHandle _tmp8E2=_new_region("prgn");struct _RegionHandle*prgn=& _tmp8E2;_push_region(prgn);{
struct Cyc_Absyn_Stmt*_tmp8E3=Cyc_Toc_throw_match_stmt();
struct Cyc_Toc_Env*_tmp8E4=Cyc_Toc_share_env(prgn,nv);
struct _tuple31 _tmp8E5=
Cyc_Toc_xlate_pat(_tmp8E4,prgn,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,
Cyc_Toc_throw_match_stmt(),0);
# 5378
struct Cyc_Toc_Env*_tmp8E7;struct Cyc_List_List*_tmp8E8;struct Cyc_Absyn_Stmt*_tmp8E9;struct _tuple31 _tmp8E6=_tmp8E5;_tmp8E7=_tmp8E6.f1;_tmp8E8=_tmp8E6.f2;_tmp8E9=_tmp8E6.f3;
# 5381
Cyc_Toc_stmt_to_c(_tmp8E7,s);
s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),e,Cyc_Absyn_seq_stmt(_tmp8E9,s,0),0);
for(0;_tmp8E8 != 0;_tmp8E8=_tmp8E8->tl){
struct _tuple32*_tmp8EA=(struct _tuple32*)_tmp8E8->hd;struct _tuple1*_tmp8EC;void*_tmp8ED;struct _tuple32*_tmp8EB=_tmp8EA;_tmp8EC=_tmp8EB->f1;_tmp8ED=_tmp8EB->f2;
s=Cyc_Absyn_declare_stmt(_tmp8EC,_tmp8ED,0,s,0);}}{
# 5388
struct Cyc_Absyn_Stmt*_tmp8EE=s;_npop_handler(0);return _tmp8EE;};
# 5375
;_pop_region(prgn);};}
# 5394
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp8EF=e->r;void*_tmp8F0=_tmp8EF;struct Cyc_Absyn_MallocInfo*_tmp8F1;struct Cyc_Absyn_Stmt*_tmp8F2;void**_tmp8F3;void**_tmp8F4;struct Cyc_List_List*_tmp8F5;struct Cyc_List_List*_tmp8F6;void**_tmp8F7;struct Cyc_List_List*_tmp8F8;void**_tmp8F9;struct Cyc_Absyn_Exp*_tmp8FA;struct Cyc_Absyn_Exp*_tmp8FB;struct Cyc_List_List*_tmp8FC;struct Cyc_Absyn_Exp*_tmp8FD;struct Cyc_Absyn_Exp*_tmp8FE;struct Cyc_Absyn_Exp*_tmp8FF;struct Cyc_Absyn_Exp*_tmp900;struct Cyc_Absyn_Exp*_tmp901;struct Cyc_Absyn_Exp*_tmp902;struct Cyc_Absyn_Exp*_tmp903;struct Cyc_Absyn_Exp*_tmp904;struct Cyc_Absyn_Exp*_tmp905;struct Cyc_Absyn_Exp*_tmp906;struct Cyc_Absyn_Exp*_tmp907;struct Cyc_Absyn_Exp*_tmp908;struct Cyc_Absyn_Exp*_tmp909;struct Cyc_Absyn_Exp*_tmp90A;struct Cyc_Absyn_Exp*_tmp90B;struct Cyc_List_List*_tmp90C;struct Cyc_Absyn_Exp*_tmp90D;struct Cyc_Absyn_Exp*_tmp90E;struct Cyc_Absyn_Exp*_tmp90F;struct Cyc_Absyn_Exp*_tmp910;struct Cyc_Absyn_Exp*_tmp911;struct Cyc_Absyn_Exp*_tmp912;struct Cyc_Absyn_Exp*_tmp913;struct Cyc_Absyn_Exp*_tmp914;switch(*((int*)_tmp8F0)){case 19: _LL482: _tmp914=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp8F0)->f1;_LL483:
 _tmp913=_tmp914;goto _LL485;case 20: _LL484: _tmp913=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8F0)->f1;_LL485:
 _tmp912=_tmp913;goto _LL487;case 21: _LL486: _tmp912=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp8F0)->f1;_LL487:
 _tmp911=_tmp912;goto _LL489;case 14: _LL488: _tmp911=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp8F0)->f1;_LL489:
 _tmp910=_tmp911;goto _LL48B;case 10: _LL48A: _tmp910=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp8F0)->f1;_LL48B:
 _tmp90F=_tmp910;goto _LL48D;case 11: _LL48C: _tmp90F=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp8F0)->f1;_LL48D:
 _tmp90E=_tmp90F;goto _LL48F;case 17: _LL48E: _tmp90E=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp8F0)->f1;_LL48F:
 _tmp90D=_tmp90E;goto _LL491;case 4: _LL490: _tmp90D=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp8F0)->f1;_LL491:
 Cyc_Toc_exptypes_to_c(_tmp90D);goto _LL481;case 2: _LL492: _tmp90C=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp8F0)->f2;_LL493:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp90C);goto _LL481;case 6: _LL494: _tmp90A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp8F0)->f1;_tmp90B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp8F0)->f2;_LL495:
 _tmp908=_tmp90A;_tmp909=_tmp90B;goto _LL497;case 7: _LL496: _tmp908=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp8F0)->f1;_tmp909=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp8F0)->f2;_LL497:
 _tmp906=_tmp908;_tmp907=_tmp909;goto _LL499;case 8: _LL498: _tmp906=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp8F0)->f1;_tmp907=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp8F0)->f2;_LL499:
 _tmp904=_tmp906;_tmp905=_tmp907;goto _LL49B;case 22: _LL49A: _tmp904=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8F0)->f1;_tmp905=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8F0)->f2;_LL49B:
 _tmp902=_tmp904;_tmp903=_tmp905;goto _LL49D;case 34: _LL49C: _tmp902=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp8F0)->f1;_tmp903=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp8F0)->f2;_LL49D:
 _tmp900=_tmp902;_tmp901=_tmp903;goto _LL49F;case 3: _LL49E: _tmp900=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp8F0)->f1;_tmp901=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp8F0)->f3;_LL49F:
 Cyc_Toc_exptypes_to_c(_tmp900);Cyc_Toc_exptypes_to_c(_tmp901);goto _LL481;case 5: _LL4A0: _tmp8FD=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8F0)->f1;_tmp8FE=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8F0)->f2;_tmp8FF=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8F0)->f3;_LL4A1:
# 5412
 Cyc_Toc_exptypes_to_c(_tmp8FD);Cyc_Toc_exptypes_to_c(_tmp8FE);Cyc_Toc_exptypes_to_c(_tmp8FF);goto _LL481;case 9: _LL4A2: _tmp8FB=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp8F0)->f1;_tmp8FC=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp8F0)->f2;_LL4A3:
# 5414
 Cyc_Toc_exptypes_to_c(_tmp8FB);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp8FC);goto _LL481;case 13: _LL4A4: _tmp8F9=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp8F0)->f1;_tmp8FA=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp8F0)->f2;_LL4A5:
*_tmp8F9=Cyc_Toc_typ_to_c(*_tmp8F9);Cyc_Toc_exptypes_to_c(_tmp8FA);goto _LL481;case 24: _LL4A6: _tmp8F7=(void**)&(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp8F0)->f1)->f3;_tmp8F8=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp8F0)->f2;_LL4A7:
# 5417
*_tmp8F7=Cyc_Toc_typ_to_c(*_tmp8F7);
_tmp8F6=_tmp8F8;goto _LL4A9;case 35: _LL4A8: _tmp8F6=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp8F0)->f2;_LL4A9:
 _tmp8F5=_tmp8F6;goto _LL4AB;case 25: _LL4AA: _tmp8F5=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp8F0)->f1;_LL4AB:
# 5421
 for(0;_tmp8F5 != 0;_tmp8F5=_tmp8F5->tl){
struct _tuple19 _tmp915=*((struct _tuple19*)_tmp8F5->hd);struct Cyc_Absyn_Exp*_tmp917;struct _tuple19 _tmp916=_tmp915;_tmp917=_tmp916.f2;
Cyc_Toc_exptypes_to_c(_tmp917);}
# 5425
goto _LL481;case 18: _LL4AC: _tmp8F4=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp8F0)->f1;_LL4AD:
 _tmp8F3=_tmp8F4;goto _LL4AF;case 16: _LL4AE: _tmp8F3=(void**)&((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp8F0)->f1;_LL4AF:
*_tmp8F3=Cyc_Toc_typ_to_c(*_tmp8F3);goto _LL481;case 36: _LL4B0: _tmp8F2=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp8F0)->f1;_LL4B1:
 Cyc_Toc_stmttypes_to_c(_tmp8F2);goto _LL481;case 33: _LL4B2: _tmp8F1=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp8F0)->f1;_LL4B3:
# 5430
 if(_tmp8F1->elt_type != 0){
void**_tmpDA5;_tmp8F1->elt_type=((_tmpDA5=_cycalloc(sizeof(*_tmpDA5)),((_tmpDA5[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp8F1->elt_type))),_tmpDA5))));}
Cyc_Toc_exptypes_to_c(_tmp8F1->num_elts);
goto _LL481;case 0: _LL4B4: _LL4B5:
 goto _LL4B7;case 1: _LL4B6: _LL4B7:
 goto _LL4B9;case 31: _LL4B8: _LL4B9:
 goto _LL4BB;case 39: _LL4BA: _LL4BB:
 goto _LL4BD;case 32: _LL4BC: _LL4BD:
 goto _LL481;case 29: _LL4BE: _LL4BF:
# 5440
 goto _LL4C1;case 30: _LL4C0: _LL4C1:
 goto _LL4C3;case 28: _LL4C2: _LL4C3:
 goto _LL4C5;case 26: _LL4C4: _LL4C5:
 goto _LL4C7;case 27: _LL4C6: _LL4C7:
 goto _LL4C9;case 23: _LL4C8: _LL4C9:
 goto _LL4CB;case 12: _LL4CA: _LL4CB:
 goto _LL4CD;case 15: _LL4CC: _LL4CD:
 goto _LL4CF;case 38: _LL4CE: _LL4CF:
 goto _LL4D1;default: _LL4D0: _LL4D1:
# 5450
{const char*_tmpDA8;void*_tmpDA7;(_tmpDA7=0,Cyc_Tcutil_terr(e->loc,((_tmpDA8="Cyclone expression within C code",_tag_dyneither(_tmpDA8,sizeof(char),33))),_tag_dyneither(_tmpDA7,sizeof(void*),0)));}
goto _LL481;}_LL481:;}
# 5455
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp91B=d->r;void*_tmp91C=_tmp91B;struct Cyc_Absyn_Typedefdecl*_tmp91D;struct Cyc_Absyn_Enumdecl*_tmp91E;struct Cyc_Absyn_Aggrdecl*_tmp91F;struct Cyc_Absyn_Fndecl*_tmp920;struct Cyc_Absyn_Vardecl*_tmp921;switch(*((int*)_tmp91C)){case 0: _LL4D3: _tmp921=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp91C)->f1;_LL4D4:
# 5458
 _tmp921->type=Cyc_Toc_typ_to_c(_tmp921->type);
if(_tmp921->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp921->initializer));
goto _LL4D2;case 1: _LL4D5: _tmp920=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp91C)->f1;_LL4D6:
# 5462
 _tmp920->ret_type=Cyc_Toc_typ_to_c(_tmp920->ret_type);
{struct Cyc_List_List*_tmp922=_tmp920->args;for(0;_tmp922 != 0;_tmp922=_tmp922->tl){
(*((struct _tuple9*)_tmp922->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp922->hd)).f3);}}
# 5466
goto _LL4D2;case 5: _LL4D7: _tmp91F=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp91C)->f1;_LL4D8:
 Cyc_Toc_aggrdecl_to_c(_tmp91F,1);goto _LL4D2;case 7: _LL4D9: _tmp91E=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp91C)->f1;_LL4DA:
# 5469
 if(_tmp91E->fields != 0){
struct Cyc_List_List*_tmp923=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp91E->fields))->v;for(0;_tmp923 != 0;_tmp923=_tmp923->tl){
struct Cyc_Absyn_Enumfield*_tmp924=(struct Cyc_Absyn_Enumfield*)_tmp923->hd;
if(_tmp924->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp924->tag));}}
# 5474
goto _LL4D2;case 8: _LL4DB: _tmp91D=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp91C)->f1;_LL4DC:
 _tmp91D->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp91D->defn));goto _LL4D2;case 2: _LL4DD: _LL4DE:
 goto _LL4E0;case 3: _LL4DF: _LL4E0:
 goto _LL4E2;case 6: _LL4E1: _LL4E2:
 goto _LL4E4;case 9: _LL4E3: _LL4E4:
 goto _LL4E6;case 10: _LL4E5: _LL4E6:
 goto _LL4E8;case 11: _LL4E7: _LL4E8:
 goto _LL4EA;case 12: _LL4E9: _LL4EA:
 goto _LL4EC;case 4: _LL4EB: _LL4EC:
# 5485
{const char*_tmpDAB;void*_tmpDAA;(_tmpDAA=0,Cyc_Tcutil_terr(d->loc,((_tmpDAB="Cyclone declaration within C code",_tag_dyneither(_tmpDAB,sizeof(char),34))),_tag_dyneither(_tmpDAA,sizeof(void*),0)));}
goto _LL4D2;case 13: _LL4ED: _LL4EE:
 goto _LL4F0;default: _LL4EF: _LL4F0:
# 5489
 goto _LL4D2;}_LL4D2:;}
# 5493
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp927=s->r;void*_tmp928=_tmp927;struct Cyc_Absyn_Stmt*_tmp929;struct Cyc_Absyn_Stmt*_tmp92A;struct Cyc_Absyn_Exp*_tmp92B;struct Cyc_Absyn_Decl*_tmp92C;struct Cyc_Absyn_Stmt*_tmp92D;struct Cyc_Absyn_Exp*_tmp92E;struct Cyc_List_List*_tmp92F;void*_tmp930;struct Cyc_Absyn_Exp*_tmp931;struct Cyc_Absyn_Exp*_tmp932;struct Cyc_Absyn_Exp*_tmp933;struct Cyc_Absyn_Stmt*_tmp934;struct Cyc_Absyn_Exp*_tmp935;struct Cyc_Absyn_Stmt*_tmp936;struct Cyc_Absyn_Exp*_tmp937;struct Cyc_Absyn_Stmt*_tmp938;struct Cyc_Absyn_Stmt*_tmp939;struct Cyc_Absyn_Exp*_tmp93A;struct Cyc_Absyn_Stmt*_tmp93B;struct Cyc_Absyn_Stmt*_tmp93C;struct Cyc_Absyn_Exp*_tmp93D;switch(*((int*)_tmp928)){case 1: _LL4F2: _tmp93D=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp928)->f1;_LL4F3:
 Cyc_Toc_exptypes_to_c(_tmp93D);goto _LL4F1;case 2: _LL4F4: _tmp93B=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp928)->f1;_tmp93C=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp928)->f2;_LL4F5:
 Cyc_Toc_stmttypes_to_c(_tmp93B);Cyc_Toc_stmttypes_to_c(_tmp93C);goto _LL4F1;case 3: _LL4F6: _tmp93A=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp928)->f1;_LL4F7:
 if(_tmp93A != 0)Cyc_Toc_exptypes_to_c(_tmp93A);goto _LL4F1;case 4: _LL4F8: _tmp937=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp928)->f1;_tmp938=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp928)->f2;_tmp939=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp928)->f3;_LL4F9:
# 5499
 Cyc_Toc_exptypes_to_c(_tmp937);Cyc_Toc_stmttypes_to_c(_tmp938);Cyc_Toc_stmttypes_to_c(_tmp939);goto _LL4F1;case 5: _LL4FA: _tmp935=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp928)->f1).f1;_tmp936=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp928)->f2;_LL4FB:
# 5501
 Cyc_Toc_exptypes_to_c(_tmp935);Cyc_Toc_stmttypes_to_c(_tmp936);goto _LL4F1;case 9: _LL4FC: _tmp931=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp928)->f1;_tmp932=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp928)->f2).f1;_tmp933=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp928)->f3).f1;_tmp934=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp928)->f4;_LL4FD:
# 5503
 Cyc_Toc_exptypes_to_c(_tmp931);Cyc_Toc_exptypes_to_c(_tmp932);Cyc_Toc_exptypes_to_c(_tmp933);
Cyc_Toc_stmttypes_to_c(_tmp934);goto _LL4F1;case 10: _LL4FE: _tmp92E=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp928)->f1;_tmp92F=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp928)->f2;_tmp930=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp928)->f3;_LL4FF:
# 5506
 Cyc_Toc_exptypes_to_c(_tmp92E);
for(0;_tmp92F != 0;_tmp92F=_tmp92F->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp92F->hd)->body);}
goto _LL4F1;case 12: _LL500: _tmp92C=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp928)->f1;_tmp92D=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp928)->f2;_LL501:
 Cyc_Toc_decltypes_to_c(_tmp92C);Cyc_Toc_stmttypes_to_c(_tmp92D);goto _LL4F1;case 14: _LL502: _tmp92A=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp928)->f1;_tmp92B=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp928)->f2).f1;_LL503:
 Cyc_Toc_stmttypes_to_c(_tmp92A);Cyc_Toc_exptypes_to_c(_tmp92B);goto _LL4F1;case 13: _LL504: _tmp929=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp928)->f2;_LL505:
 Cyc_Toc_stmttypes_to_c(_tmp929);goto _LL4F1;case 0: _LL506: _LL507:
 goto _LL509;case 6: _LL508: _LL509:
 goto _LL50B;case 7: _LL50A: _LL50B:
 goto _LL50D;case 8: _LL50C: _LL50D:
# 5516
 goto _LL4F1;case 11: _LL50E: _LL50F:
# 5519
{struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct _tmpDAE;struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmpDAD;s->r=(void*)((_tmpDAD=_cycalloc_atomic(sizeof(*_tmpDAD)),((_tmpDAD[0]=((_tmpDAE.tag=0,_tmpDAE)),_tmpDAD))));}
goto _LL4F1;case 15: _LL510: _LL511:
 goto _LL513;default: _LL512: _LL513:
# 5523
{struct Cyc_String_pa_PrintArg_struct _tmpDB6;void*_tmpDB5[1];const char*_tmpDB4;void*_tmpDB3;(_tmpDB3=0,Cyc_Tcutil_terr(s->loc,(struct _dyneither_ptr)((_tmpDB6.tag=0,((_tmpDB6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpDB5[0]=& _tmpDB6,Cyc_aprintf(((_tmpDB4="Cyclone statement in C code: %s",_tag_dyneither(_tmpDB4,sizeof(char),32))),_tag_dyneither(_tmpDB5,sizeof(void*),1)))))))),_tag_dyneither(_tmpDB3,sizeof(void*),0)));}
goto _LL4F1;}_LL4F1:;}
# 5532
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv)){
const char*_tmpDB9;void*_tmpDB8;(_tmpDB8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpDB9="decls_to_c: not at toplevel!",_tag_dyneither(_tmpDB9,sizeof(char),29))),_tag_dyneither(_tmpDB8,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp946=d->r;void*_tmp947=_tmp946;struct Cyc_List_List*_tmp948;struct Cyc_List_List*_tmp949;struct Cyc_List_List*_tmp94A;struct Cyc_List_List*_tmp94B;struct Cyc_Absyn_Typedefdecl*_tmp94C;struct Cyc_Absyn_Enumdecl*_tmp94D;struct Cyc_Absyn_Datatypedecl*_tmp94E;struct Cyc_Absyn_Aggrdecl*_tmp94F;struct Cyc_Absyn_Fndecl*_tmp950;struct Cyc_Absyn_Vardecl*_tmp951;switch(*((int*)_tmp947)){case 0: _LL515: _tmp951=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp947)->f1;_LL516: {
# 5539
struct _tuple1*_tmp952=_tmp951->name;
# 5541
if(_tmp951->sc == Cyc_Absyn_ExternC){
struct _tuple1*_tmpDBA;_tmp952=((_tmpDBA=_cycalloc(sizeof(*_tmpDBA)),((_tmpDBA->f1=Cyc_Absyn_Rel_n(0),((_tmpDBA->f2=(*_tmp952).f2,_tmpDBA))))));}
if(_tmp951->initializer != 0){
if(_tmp951->sc == Cyc_Absyn_ExternC)_tmp951->sc=Cyc_Absyn_Public;
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp951->initializer));else{
# 5548
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp951->initializer));}}
# 5550
{struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmpDBD;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpDBC;nv=Cyc_Toc_add_varmap(r,nv,_tmp951->name,Cyc_Absyn_varb_exp(_tmp952,(void*)((_tmpDBC=_cycalloc(sizeof(*_tmpDBC)),((_tmpDBC[0]=((_tmpDBD.tag=1,((_tmpDBD.f1=_tmp951,_tmpDBD)))),_tmpDBC)))),0));}
_tmp951->name=_tmp952;
_tmp951->sc=Cyc_Toc_scope_to_c(_tmp951->sc);
_tmp951->type=Cyc_Toc_typ_to_c(_tmp951->type);
{struct Cyc_List_List*_tmpDBE;Cyc_Toc_result_decls=((_tmpDBE=_cycalloc(sizeof(*_tmpDBE)),((_tmpDBE->hd=d,((_tmpDBE->tl=Cyc_Toc_result_decls,_tmpDBE))))));}
goto _LL514;}case 1: _LL517: _tmp950=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp947)->f1;_LL518: {
# 5557
struct _tuple1*_tmp957=_tmp950->name;
# 5559
if(_tmp950->sc == Cyc_Absyn_ExternC){
{struct _tuple1*_tmpDBF;_tmp957=((_tmpDBF=_cycalloc(sizeof(*_tmpDBF)),((_tmpDBF->f1=Cyc_Absyn_Abs_n(0,1),((_tmpDBF->f2=(*_tmp957).f2,_tmpDBF))))));}
_tmp950->sc=Cyc_Absyn_Public;}
# 5563
nv=Cyc_Toc_add_varmap(r,nv,_tmp950->name,Cyc_Absyn_var_exp(_tmp957,0));
_tmp950->name=_tmp957;
Cyc_Toc_fndecl_to_c(nv,_tmp950,cinclude);
{struct Cyc_List_List*_tmpDC0;Cyc_Toc_result_decls=((_tmpDC0=_cycalloc(sizeof(*_tmpDC0)),((_tmpDC0->hd=d,((_tmpDC0->tl=Cyc_Toc_result_decls,_tmpDC0))))));}
goto _LL514;}case 2: _LL519: _LL51A:
 goto _LL51C;case 3: _LL51B: _LL51C: {
# 5570
const char*_tmpDC3;void*_tmpDC2;(_tmpDC2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDC3="letdecl at toplevel",_tag_dyneither(_tmpDC3,sizeof(char),20))),_tag_dyneither(_tmpDC2,sizeof(void*),0)));}case 4: _LL51D: _LL51E: {
# 5572
const char*_tmpDC6;void*_tmpDC5;(_tmpDC5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDC6="region decl at toplevel",_tag_dyneither(_tmpDC6,sizeof(char),24))),_tag_dyneither(_tmpDC5,sizeof(void*),0)));}case 5: _LL51F: _tmp94F=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp947)->f1;_LL520:
# 5576
 Cyc_Toc_aggrdecl_to_c(_tmp94F,1);
goto _LL514;case 6: _LL521: _tmp94E=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp947)->f1;_LL522:
# 5579
 if(_tmp94E->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp94E);else{
# 5582
Cyc_Toc_datatypedecl_to_c(_tmp94E);}
goto _LL514;case 7: _LL523: _tmp94D=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp947)->f1;_LL524:
# 5585
 Cyc_Toc_enumdecl_to_c(_tmp94D);
{struct Cyc_List_List*_tmpDC7;Cyc_Toc_result_decls=((_tmpDC7=_cycalloc(sizeof(*_tmpDC7)),((_tmpDC7->hd=d,((_tmpDC7->tl=Cyc_Toc_result_decls,_tmpDC7))))));}
goto _LL514;case 8: _LL525: _tmp94C=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp947)->f1;_LL526:
# 5589
 _tmp94C->tvs=0;
if(_tmp94C->defn != 0)
_tmp94C->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp94C->defn));else{
# 5593
enum Cyc_Absyn_KindQual _tmp95F=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp94C->kind))->v)->kind;enum Cyc_Absyn_KindQual _tmp960=_tmp95F;if(_tmp960 == Cyc_Absyn_BoxKind){_LL534: _LL535:
 _tmp94C->defn=Cyc_Absyn_void_star_typ();goto _LL533;}else{_LL536: _LL537:
 _tmp94C->defn=(void*)& Cyc_Absyn_VoidType_val;goto _LL533;}_LL533:;}
# 5598
{struct Cyc_List_List*_tmpDC8;Cyc_Toc_result_decls=((_tmpDC8=_cycalloc(sizeof(*_tmpDC8)),((_tmpDC8->hd=d,((_tmpDC8->tl=Cyc_Toc_result_decls,_tmpDC8))))));}
goto _LL514;case 13: _LL527: _LL528:
 goto _LL52A;case 14: _LL529: _LL52A:
# 5602
 goto _LL514;case 9: _LL52B: _tmp94B=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp947)->f2;_LL52C:
 _tmp94A=_tmp94B;goto _LL52E;case 10: _LL52D: _tmp94A=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp947)->f2;_LL52E:
 _tmp949=_tmp94A;goto _LL530;case 11: _LL52F: _tmp949=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp947)->f1;_LL530:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp949,top,cinclude);goto _LL514;default: _LL531: _tmp948=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp947)->f1;_LL532:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp948,top,1);goto _LL514;}_LL514:;};}
# 5609
return nv;}
# 5613
static void Cyc_Toc_init(){
struct Cyc_Core_NewDynamicRegion _tmp962=Cyc_Core_new_rckey();struct Cyc_Core_DynamicRegion*_tmp964;struct Cyc_Core_NewDynamicRegion _tmp963=_tmp962;_tmp964=_tmp963.key;{
struct Cyc_Toc_TocState*_tmp965=((struct Cyc_Toc_TocState*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Toc_TocState*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmp964,0,Cyc_Toc_empty_toc_state);
{struct Cyc_Toc_TocStateWrap*_tmpDC9;Cyc_Toc_toc_state=((_tmpDC9=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpDC9)),((_tmpDC9->dyn=(struct Cyc_Core_DynamicRegion*)_tmp964,((_tmpDC9->state=(struct Cyc_Toc_TocState*)_tmp965,_tmpDC9))))));}
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0;
Cyc_Toc_bounds_checks_eliminated=0;{
struct _dyneither_ptr**_tmpDCA;Cyc_Toc_globals=_tag_dyneither(((_tmpDCA=_cycalloc(sizeof(struct _dyneither_ptr*)* 42),((_tmpDCA[0]=(struct _dyneither_ptr*)& Cyc_Toc__throw_str,((_tmpDCA[1]=(struct _dyneither_ptr*)& Cyc_Toc_setjmp_str,((_tmpDCA[2]=(struct _dyneither_ptr*)& Cyc_Toc__push_handler_str,((_tmpDCA[3]=(struct _dyneither_ptr*)& Cyc_Toc__pop_handler_str,((_tmpDCA[4]=(struct _dyneither_ptr*)& Cyc_Toc__exn_thrown_str,((_tmpDCA[5]=(struct _dyneither_ptr*)& Cyc_Toc__npop_handler_str,((_tmpDCA[6]=(struct _dyneither_ptr*)& Cyc_Toc__check_null_str,((_tmpDCA[7]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_null_str,((_tmpDCA[8]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_notnull_str,((_tmpDCA[9]=(struct _dyneither_ptr*)& Cyc_Toc__check_dyneither_subscript_str,((_tmpDCA[10]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_str,((_tmpDCA[11]=(struct _dyneither_ptr*)& Cyc_Toc__tag_dyneither_str,((_tmpDCA[12]=(struct _dyneither_ptr*)& Cyc_Toc__init_dyneither_ptr_str,((_tmpDCA[13]=(struct _dyneither_ptr*)& Cyc_Toc__untag_dyneither_ptr_str,((_tmpDCA[14]=(struct _dyneither_ptr*)& Cyc_Toc__get_dyneither_size_str,((_tmpDCA[15]=(struct _dyneither_ptr*)& Cyc_Toc__get_zero_arr_size_str,((_tmpDCA[16]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_plus_str,((_tmpDCA[17]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_plus_str,((_tmpDCA[18]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpDCA[19]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_str,((_tmpDCA[20]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpDCA[21]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_post_str,((_tmpDCA[22]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_str,((_tmpDCA[23]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_str,((_tmpDCA[24]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_atomic_str,((_tmpDCA[25]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_atomic_str,((_tmpDCA[26]=(struct _dyneither_ptr*)& Cyc_Toc__region_malloc_str,((_tmpDCA[27]=(struct _dyneither_ptr*)& Cyc_Toc__region_calloc_str,((_tmpDCA[28]=(struct _dyneither_ptr*)& Cyc_Toc__check_times_str,((_tmpDCA[29]=(struct _dyneither_ptr*)& Cyc_Toc__new_region_str,((_tmpDCA[30]=(struct _dyneither_ptr*)& Cyc_Toc__push_region_str,((_tmpDCA[31]=(struct _dyneither_ptr*)& Cyc_Toc__pop_region_str,((_tmpDCA[32]=(struct _dyneither_ptr*)& Cyc_Toc__open_dynregion_str,((_tmpDCA[33]=(struct _dyneither_ptr*)& Cyc_Toc__push_dynregion_str,((_tmpDCA[34]=(struct _dyneither_ptr*)& Cyc_Toc__pop_dynregion_str,((_tmpDCA[35]=(struct _dyneither_ptr*)& Cyc_Toc__reset_region_str,((_tmpDCA[36]=(struct _dyneither_ptr*)& Cyc_Toc__throw_arraybounds_str,((_tmpDCA[37]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_decrease_size_str,((_tmpDCA[38]=(struct _dyneither_ptr*)& Cyc_Toc__throw_match_str,((_tmpDCA[39]=(struct _dyneither_ptr*)& Cyc_Toc__swap_word_str,((_tmpDCA[40]=(struct _dyneither_ptr*)& Cyc_Toc__swap_dyneither_str,((_tmpDCA[41]=(struct _dyneither_ptr*)& Cyc_Toc__fast_region_malloc_str,_tmpDCA)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),sizeof(struct _dyneither_ptr*),42);};};}
# 5670
static int Cyc_Toc_destroy_labels(struct _RegionHandle*d,struct Cyc_Toc_TocState*ts){
struct Cyc_Toc_TocState _tmp968=*ts;struct Cyc_Xarray_Xarray*_tmp96A;struct Cyc_Toc_TocState _tmp969=_tmp968;_tmp96A=_tmp969.temp_labels;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp96A);
return 0;}
# 5678
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){
Cyc_Toc_init();{
struct _RegionHandle _tmp96B=_new_region("start");struct _RegionHandle*start=& _tmp96B;_push_region(start);
Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(start),ds,1,0);{
# 5687
struct Cyc_Toc_TocStateWrap*ts=0;
_swap_word(& ts,& Cyc_Toc_toc_state);{
struct Cyc_Toc_TocStateWrap _tmp96C=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Core_DynamicRegion*_tmp96E;struct Cyc_Toc_TocState*_tmp96F;struct Cyc_Toc_TocStateWrap _tmp96D=_tmp96C;_tmp96E=_tmp96D.dyn;_tmp96F=_tmp96D.state;
((int(*)(struct Cyc_Core_DynamicRegion*key,struct Cyc_Toc_TocState*arg,int(*body)(struct _RegionHandle*h,struct Cyc_Toc_TocState*arg)))Cyc_Core_open_region)(_tmp96E,_tmp96F,Cyc_Toc_destroy_labels);
Cyc_Core_free_rckey(_tmp96E);
((void(*)(struct _dyneither_ptr ptr))Cyc_Core_ufree)(_tag_dyneither(ts,sizeof(struct Cyc_Toc_TocStateWrap),1));};};{
# 5694
struct Cyc_List_List*_tmp970=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0);return _tmp970;};
# 5680
;_pop_region(start);};}
