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
struct _tuple14 _tmp0=*x;struct _tuple14 _tmp1=_tmp0;struct _tuple1*_tmp2;struct _tuple1*_tmp3;_LL1: _tmp2=_tmp1.f1;_tmp3=_tmp1.f2;_LL2: {
struct _tuple14 _tmp4=*y;struct _tuple14 _tmp5=_tmp4;struct _tuple1*_tmp6;struct _tuple1*_tmp7;_LL6: _tmp6=_tmp5.f1;_tmp7=_tmp5.f2;_LL7: {
int i=Cyc_Absyn_qvar_cmp(_tmp2,_tmp6);
if(i != 0)return i;
return Cyc_Absyn_qvar_cmp(_tmp3,_tmp7);}_LL5:;}_LL0:;}
# 116
static struct Cyc_Toc_TocState*Cyc_Toc_empty_toc_state(struct _RegionHandle*d,int unused){
struct Cyc_Dict_Dict*_tmp919;struct Cyc_Dict_Dict*_tmp918;struct Cyc_Set_Set**_tmp917;struct Cyc_List_List**_tmp916;struct Cyc_Dict_Dict*_tmp915;struct Cyc_List_List**_tmp914;struct Cyc_Toc_TocState*_tmp913;return(_tmp913=_region_malloc(d,sizeof(*_tmp913)),((_tmp913->tuple_types=(
(_tmp914=_region_malloc(d,sizeof(*_tmp914)),((_tmp914[0]=0,_tmp914)))),((_tmp913->aggrs_so_far=(
(_tmp915=_region_malloc(d,sizeof(*_tmp915)),((_tmp915[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmp915)))),((_tmp913->abs_struct_types=(
(_tmp916=_region_malloc(d,sizeof(*_tmp916)),((_tmp916[0]=0,_tmp916)))),((_tmp913->datatypes_so_far=(
(_tmp917=_region_malloc(d,sizeof(*_tmp917)),((_tmp917[0]=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),_tmp917)))),((_tmp913->xdatatypes_so_far=(
(_tmp918=_region_malloc(d,sizeof(*_tmp918)),((_tmp918[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmp918)))),((_tmp913->qvar_tags=(
(_tmp919=_region_malloc(d,sizeof(*_tmp919)),((_tmp919[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),_tmp919)))),((_tmp913->temp_labels=
((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d),_tmp913)))))))))))))));}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 133
typedef struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state_t;
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple15{struct Cyc_Toc_TocState*f1;void*f2;};
# 140
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple15*)){
# 143
struct Cyc_Toc_TocStateWrap*ts=0;
_swap_word(& ts,& Cyc_Toc_toc_state);{
struct Cyc_Toc_TocStateWrap _tmpF=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp10=_tmpF;struct Cyc_Core_DynamicRegion*_tmp11;struct Cyc_Toc_TocState*_tmp12;_LLB: _tmp11=_tmp10.dyn;_tmp12=_tmp10.state;_LLC: {
struct _dyneither_ptr _tmp13=((struct _dyneither_ptr(*)(struct _dyneither_ptr ptr))Cyc_Core_alias_refptr)(_tag_dyneither(_tmp11,sizeof(struct Cyc_Core_DynamicRegion),1));
{struct Cyc_Toc_TocStateWrap _tmp91A;*ts=((_tmp91A.dyn=(struct Cyc_Core_DynamicRegion*)_tmp11,((_tmp91A.state=(struct Cyc_Toc_TocState*)_tmp12,_tmp91A))));}
_swap_word(& ts,& Cyc_Toc_toc_state);{
struct _tuple15 _tmp91B;struct _tuple15 _tmp15=(_tmp91B.f1=_tmp12,((_tmp91B.f2=arg,_tmp91B)));
void*_tmp16=((void*(*)(struct Cyc_Core_DynamicRegion*key,struct _tuple15*arg,void*(*body)(struct _RegionHandle*h,struct _tuple15*arg)))Cyc_Core_open_region)((struct Cyc_Core_DynamicRegion*)_untag_dyneither_ptr(_tmp13,sizeof(struct Cyc_Core_DynamicRegion),1),& _tmp15,f);
Cyc_Core_free_rckey((struct Cyc_Core_DynamicRegion*)_untag_dyneither_ptr(_tmp13,sizeof(struct Cyc_Core_DynamicRegion),1));
return _tmp16;};}_LLA:;};}struct _tuple16{struct _tuple1*f1;void*(*f2)(struct _tuple1*);};struct _tuple17{struct Cyc_Toc_TocState*f1;struct _tuple16*f2;};struct _tuple18{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
# 155
static void*Cyc_Toc_aggrdecl_type_body(struct _RegionHandle*d,struct _tuple17*env){
# 158
struct _tuple17 _tmp18=*env;struct _tuple17 _tmp19=_tmp18;struct Cyc_Toc_TocState*_tmp1A;struct _tuple1*_tmp1B;void*(*_tmp1C)(struct _tuple1*);_LL10: _tmp1A=_tmp19.f1;_tmp1B=(_tmp19.f2)->f1;_tmp1C=(_tmp19.f2)->f2;_LL11: {
struct _tuple18**v=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp1A->aggrs_so_far,_tmp1B);
if(v == 0)
return _tmp1C(_tmp1B);else{
# 163
struct _tuple18*_tmp1D=*v;struct _tuple18*_tmp1E=_tmp1D;void*_tmp1F;_LL15: _tmp1F=_tmp1E->f2;_LL16:
 return _tmp1F;_LL14:;}}_LLF:;}
# 168
void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){
struct _tuple16 _tmp91C;struct _tuple16 env=(_tmp91C.f1=q,((_tmp91C.f2=type_maker,_tmp91C)));
return((void*(*)(struct _tuple16*arg,void*(*f)(struct _RegionHandle*,struct _tuple17*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_aggrdecl_type_body);}
# 173
static int Cyc_Toc_tuple_type_counter=0;
static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;char Cyc_Toc_Toc_Unimplemented[18]="Toc_Unimplemented";struct Cyc_Toc_Toc_Unimplemented_exn_struct{char*tag;};char Cyc_Toc_Toc_Impossible[15]="Toc_Impossible";struct Cyc_Toc_Toc_Impossible_exn_struct{char*tag;};
# 180
static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 183
Cyc_vfprintf(Cyc_stderr,fmt,ap);
{const char*_tmp91F;void*_tmp91E;(_tmp91E=0,Cyc_fprintf(Cyc_stderr,((_tmp91F="\n",_tag_dyneither(_tmp91F,sizeof(char),2))),_tag_dyneither(_tmp91E,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Unimplemented_exn_struct _tmp922;struct Cyc_Toc_Toc_Unimplemented_exn_struct*_tmp921;(int)_throw((void*)((_tmp921=_cycalloc_atomic(sizeof(*_tmp921)),((_tmp921[0]=((_tmp922.tag=Cyc_Toc_Toc_Unimplemented,_tmp922)),_tmp921)))));};}
# 188
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 191
Cyc_vfprintf(Cyc_stderr,fmt,ap);
{const char*_tmp925;void*_tmp924;(_tmp924=0,Cyc_fprintf(Cyc_stderr,((_tmp925="\n",_tag_dyneither(_tmp925,sizeof(char),2))),_tag_dyneither(_tmp924,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Impossible_exn_struct _tmp928;struct Cyc_Toc_Toc_Impossible_exn_struct*_tmp927;(int)_throw((void*)((_tmp927=_cycalloc_atomic(sizeof(*_tmp927)),((_tmp927[0]=((_tmp928.tag=Cyc_Toc_Toc_Impossible,_tmp928)),_tmp927)))));};}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_exn_struct{char*tag;};static char _tmp29[5]="curr";
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
return Cyc_Absyn_skip_stmt(0);}
# 316
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
void*_tmpC0=e->r;void*_tmpC1=_tmpC0;struct Cyc_Absyn_Exp*_tmpC2;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpC1)->tag == 13){if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpC1)->f4 == Cyc_Absyn_No_coercion){_LL1A: _tmpC2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpC1)->f2;_LL1B:
 return Cyc_Toc_cast_it(t,_tmpC2);}else{goto _LL1C;}}else{_LL1C: _LL1D:
 return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}_LL19:;}
# 322
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp92B;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp92A;return(void*)((_tmp92A=_cycalloc(sizeof(*_tmp92A)),((_tmp92A[0]=((_tmp92B.tag=13,((_tmp92B.f1=t,((_tmp92B.f2=e,((_tmp92B.f3=0,((_tmp92B.f4=Cyc_Absyn_No_coercion,_tmp92B)))))))))),_tmp92A))));}
# 325
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmp92E;struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp92D;return(void*)((_tmp92D=_cycalloc(sizeof(*_tmp92D)),((_tmp92D[0]=((_tmp92E.tag=19,((_tmp92E.f1=e,_tmp92E)))),_tmp92D))));}
# 328
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmp931;struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp930;return(void*)((_tmp930=_cycalloc(sizeof(*_tmp930)),((_tmp930[0]=((_tmp931.tag=22,((_tmp931.f1=e1,((_tmp931.f2=e2,_tmp931)))))),_tmp930))));}
# 331
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmp934;struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp933;return(void*)((_tmp933=_cycalloc(sizeof(*_tmp933)),((_tmp933[0]=((_tmp934.tag=36,((_tmp934.f1=s,_tmp934)))),_tmp933))));}
# 334
static void*Cyc_Toc_sizeoftyp_exp_r(void*t){
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp937;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp936;return(void*)((_tmp936=_cycalloc(sizeof(*_tmp936)),((_tmp936[0]=((_tmp937.tag=16,((_tmp937.f1=t,_tmp937)))),_tmp936))));}
# 337
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp93A;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp939;return(void*)((_tmp939=_cycalloc(sizeof(*_tmp939)),((_tmp939[0]=((_tmp93A.tag=9,((_tmp93A.f1=e,((_tmp93A.f2=es,((_tmp93A.f3=0,((_tmp93A.f4=1,_tmp93A)))))))))),_tmp939))));}
# 340
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmp93D;struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp93C;return(void*)((_tmp93C=_cycalloc(sizeof(*_tmp93C)),((_tmp93C[0]=((_tmp93D.tag=1,((_tmp93D.f1=e,_tmp93D)))),_tmp93C))));}
# 343
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp940;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp93F;return(void*)((_tmp93F=_cycalloc(sizeof(*_tmp93F)),((_tmp93F[0]=((_tmp940.tag=2,((_tmp940.f1=s1,((_tmp940.f2=s2,_tmp940)))))),_tmp93F))));}
# 346
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp943;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp942;return(void*)((_tmp942=_cycalloc(sizeof(*_tmp942)),((_tmp942[0]=((_tmp943.tag=5,((_tmp943.f1=e1,((_tmp943.f2=e2,((_tmp943.f3=e3,_tmp943)))))))),_tmp942))));}
# 349
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmp946;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp945;return(void*)((_tmp945=_cycalloc(sizeof(*_tmp945)),((_tmp945[0]=((_tmp946.tag=20,((_tmp946.f1=e,((_tmp946.f2=n,((_tmp946.f3=0,((_tmp946.f4=0,_tmp946)))))))))),_tmp945))));}
# 352
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmp949;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp948;return(void*)((_tmp948=_cycalloc(sizeof(*_tmp948)),((_tmp948[0]=((_tmp949.tag=21,((_tmp949.f1=e,((_tmp949.f2=n,((_tmp949.f3=0,((_tmp949.f4=0,_tmp949)))))))))),_tmp948))));}
# 355
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 358
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp94C;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp94B;return(void*)((_tmp94B=_cycalloc(sizeof(*_tmp94B)),((_tmp94B[0]=((_tmp94C.tag=35,((_tmp94C.f1=tdopt,((_tmp94C.f2=ds,_tmp94C)))))),_tmp94B))));}
# 360
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmp94F;struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp94E;return(void*)((_tmp94E=_cycalloc(sizeof(*_tmp94E)),((_tmp94E[0]=((_tmp94F.tag=8,((_tmp94F.f1=v,((_tmp94F.f2=s,_tmp94F)))))),_tmp94E))));}
# 363
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 367
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,unsigned int loc){
void*_tmpDE=e->r;void*_tmpDF=_tmpDE;struct Cyc_Absyn_Exp*_tmpE0;if(((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpDF)->tag == 19){_LL1F: _tmpE0=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_LL20:
 return Cyc_Absyn_aggrarrow_exp(_tmpE0,f,loc);}else{_LL21: _LL22:
 return Cyc_Absyn_aggrmember_exp(e,f,loc);}_LL1E:;}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 384
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
# 393
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 402
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
# 411
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
# 422
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
struct Cyc_Absyn_Exp*function;
{void*_tmpE1=Cyc_Tcutil_compress(t);void*_tmpE2=_tmpE1;enum Cyc_Absyn_Size_of _tmpE3;switch(*((int*)_tmpE2)){case 6: _LL24: _tmpE3=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpE2)->f2;_LL25:
# 426
{enum Cyc_Absyn_Size_of _tmpE4=_tmpE3;switch(_tmpE4){case Cyc_Absyn_Char_sz: _LL31: _LL32:
 function=fS->fchar;goto _LL30;case Cyc_Absyn_Short_sz: _LL33: _LL34:
 function=fS->fshort;goto _LL30;case Cyc_Absyn_Int_sz: _LL35: _LL36:
 function=fS->fint;goto _LL30;default: _LL37: _LL38: {
struct Cyc_Core_Impossible_exn_struct _tmp955;const char*_tmp954;struct Cyc_Core_Impossible_exn_struct*_tmp953;(int)_throw((void*)((_tmp953=_cycalloc(sizeof(*_tmp953)),((_tmp953[0]=((_tmp955.tag=Cyc_Core_Impossible,((_tmp955.f1=((_tmp954="impossible IntType (not char, short or int)",_tag_dyneither(_tmp954,sizeof(char),44))),_tmp955)))),_tmp953)))));}}_LL30:;}
# 432
goto _LL23;case 7: switch(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpE2)->f1){case 0: _LL26: _LL27:
# 434
 function=fS->ffloat;
goto _LL23;case 1: _LL28: _LL29:
# 437
 function=fS->fdouble;
goto _LL23;default: _LL2A: _LL2B:
# 440
 function=fS->flongdouble;
goto _LL23;}case 5: _LL2C: _LL2D:
# 443
 function=fS->fvoidstar;
goto _LL23;default: _LL2E: _LL2F: {
# 446
struct Cyc_Core_Impossible_exn_struct _tmp962;const char*_tmp961;void*_tmp960[1];struct Cyc_String_pa_PrintArg_struct _tmp95F;struct Cyc_Core_Impossible_exn_struct*_tmp95E;(int)_throw((void*)((_tmp95E=_cycalloc(sizeof(*_tmp95E)),((_tmp95E[0]=((_tmp962.tag=Cyc_Core_Impossible,((_tmp962.f1=(struct _dyneither_ptr)((_tmp95F.tag=0,((_tmp95F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp960[0]=& _tmp95F,Cyc_aprintf(((_tmp961="impossible expression type %s (not int, float, double, or pointer)",_tag_dyneither(_tmp961,sizeof(char),67))),_tag_dyneither(_tmp960,sizeof(void*),1)))))))),_tmp962)))),_tmp95E)))));}}_LL23:;}
# 448
return function;}
# 450
struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
return Cyc_Toc_getFunctionType(fS,(void*)_check_null(arr->topt));}
# 453
struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmpED=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_tmpEE=_tmpED;void*_tmpEF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpEE)->tag == 5){_LL3A: _tmpEF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpEE)->f1).elt_typ;_LL3B:
# 456
 return Cyc_Toc_getFunctionType(fS,_tmpEF);}else{_LL3C: _LL3D: {
struct Cyc_Core_Impossible_exn_struct _tmp968;const char*_tmp967;struct Cyc_Core_Impossible_exn_struct*_tmp966;(int)_throw((void*)((_tmp966=_cycalloc(sizeof(*_tmp966)),((_tmp966[0]=((_tmp968.tag=Cyc_Core_Impossible,((_tmp968.f1=((_tmp967="impossible type (not pointer)",_tag_dyneither(_tmp967,sizeof(char),30))),_tmp968)))),_tmp966)))));}}_LL39:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 464
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmpF3=e->r;void*_tmpF4=_tmpF3;struct Cyc_List_List*_tmpF5;struct Cyc_List_List*_tmpF6;struct Cyc_List_List*_tmpF7;struct Cyc_List_List*_tmpF8;struct Cyc_List_List*_tmpF9;struct Cyc_Absyn_Exp*_tmpFA;long long _tmpFB;int _tmpFC;short _tmpFD;struct _dyneither_ptr _tmpFE;char _tmpFF;switch(*((int*)_tmpF4)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF4)->f1).Null_c).tag){case 2: _LL3F: _tmpFF=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF4)->f1).Char_c).val).f2;_LL40:
 return _tmpFF == '\000';case 3: _LL41: _tmpFE=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF4)->f1).Wchar_c).val;_LL42: {
# 468
unsigned long _tmp100=Cyc_strlen((struct _dyneither_ptr)_tmpFE);
int i=0;
if(_tmp100 >= 2  && *((const char*)_check_dyneither_subscript(_tmpFE,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmpFE,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmpFE,sizeof(char),1))== 'x'  && _tmp100 >= 3) && *((const char*)_check_dyneither_subscript(_tmpFE,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp100;++ i){
if(*((const char*)_check_dyneither_subscript(_tmpFE,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 478
return 0;}}case 4: _LL43: _tmpFD=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF4)->f1).Short_c).val).f2;_LL44:
 return _tmpFD == 0;case 5: _LL45: _tmpFC=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF4)->f1).Int_c).val).f2;_LL46:
 return _tmpFC == 0;case 6: _LL47: _tmpFB=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF4)->f1).LongLong_c).val).f2;_LL48:
 return _tmpFB == 0;case 1: _LL49: _LL4A:
 return 1;default: goto _LL57;}case 13: _LL4B: _tmpFA=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpF4)->f2;_LL4C:
 return Cyc_Toc_is_zero(_tmpFA);case 23: _LL4D: _tmpF9=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpF4)->f1;_LL4E:
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmpF9);case 25: _LL4F: _tmpF8=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpF4)->f1;_LL50:
 _tmpF7=_tmpF8;goto _LL52;case 28: _LL51: _tmpF7=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpF4)->f3;_LL52:
 _tmpF6=_tmpF7;goto _LL54;case 24: _LL53: _tmpF6=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpF4)->f2;_LL54:
 _tmpF5=_tmpF6;goto _LL56;case 35: _LL55: _tmpF5=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpF4)->f2;_LL56:
# 489
 for(0;_tmpF5 != 0;_tmpF5=_tmpF5->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple19*)_tmpF5->hd)).f2))return 0;}
return 1;default: _LL57: _LL58:
 return 0;}_LL3E:;}
# 497
static int Cyc_Toc_is_nullable(void*t){
void*_tmp101=Cyc_Tcutil_compress(t);void*_tmp102=_tmp101;union Cyc_Absyn_Constraint*_tmp103;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp102)->tag == 5){_LL5A: _tmp103=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp102)->f1).ptr_atts).nullable;_LL5B:
# 500
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp103);}else{_LL5C: _LL5D: {
const char*_tmp96B;void*_tmp96A;(_tmp96A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp96B="is_nullable",_tag_dyneither(_tmp96B,sizeof(char),12))),_tag_dyneither(_tmp96A,sizeof(void*),0)));}}_LL59:;}
# 506
static struct _dyneither_ptr Cyc_Toc_collapse_qvar(struct _dyneither_ptr s,struct _tuple1*x){
unsigned int slen=Cyc_strlen((struct _dyneither_ptr)s);
unsigned int len=9 + slen;
struct _tuple1*_tmp106=x;union Cyc_Absyn_Nmspace _tmp107;struct _dyneither_ptr _tmp108;_LL5F: _tmp107=_tmp106->f1;_tmp108=*_tmp106->f2;_LL60: {
unsigned int vlen=Cyc_strlen((struct _dyneither_ptr)_tmp108);
len +=1 + vlen;{
struct Cyc_List_List*nms;
{union Cyc_Absyn_Nmspace _tmp109=_tmp107;struct Cyc_List_List*_tmp10A;struct Cyc_List_List*_tmp10B;struct Cyc_List_List*_tmp10C;switch((_tmp109.Abs_n).tag){case 4: _LL64: _LL65:
 nms=0;goto _LL63;case 1: _LL66: _tmp10C=(_tmp109.Rel_n).val;_LL67:
 nms=_tmp10C;goto _LL63;case 2: _LL68: _tmp10B=(_tmp109.Abs_n).val;_LL69:
 nms=_tmp10B;goto _LL63;default: _LL6A: _tmp10A=(_tmp109.C_n).val;_LL6B:
 nms=_tmp10A;goto _LL63;}_LL63:;}
# 519
{struct Cyc_List_List*_tmp10D=nms;for(0;_tmp10D != 0;_tmp10D=_tmp10D->tl){
len +=1 + Cyc_strlen((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp10D->hd));}}{
char*_tmp96D;unsigned int _tmp96C;struct _dyneither_ptr buf=(_tmp96C=len,((_tmp96D=_cyccalloc_atomic(sizeof(char),_tmp96C),_tag_dyneither(_tmp96D,sizeof(char),_tmp96C))));
struct _dyneither_ptr p=buf;
Cyc_strcpy(p,(struct _dyneither_ptr)s);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)slen);
for(0;nms != 0;nms=nms->tl){
struct _dyneither_ptr*_tmp10E=(struct _dyneither_ptr*)nms->hd;struct _dyneither_ptr*_tmp10F=_tmp10E;struct _dyneither_ptr _tmp110;_LL6D: _tmp110=*_tmp10F;_LL6E:
{char _tmp970;char _tmp96F;struct _dyneither_ptr _tmp96E;(_tmp96E=p,((_tmp96F=*((char*)_check_dyneither_subscript(_tmp96E,sizeof(char),0)),((_tmp970='_',((_get_dyneither_size(_tmp96E,sizeof(char))== 1  && (_tmp96F == '\000'  && _tmp970 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp96E.curr)=_tmp970)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp110);
_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)Cyc_strlen((struct _dyneither_ptr)_tmp110));_LL6C:;}
# 530
{char _tmp973;char _tmp972;struct _dyneither_ptr _tmp971;(_tmp971=p,((_tmp972=*((char*)_check_dyneither_subscript(_tmp971,sizeof(char),0)),((_tmp973='_',((_get_dyneither_size(_tmp971,sizeof(char))== 1  && (_tmp972 == '\000'  && _tmp973 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp971.curr)=_tmp973)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp108);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)vlen);
{const char*_tmp974;Cyc_strcpy(p,((_tmp974="_struct",_tag_dyneither(_tmp974,sizeof(char),8))));}
return(struct _dyneither_ptr)buf;};};}_LL5E:;}struct _tuple20{struct Cyc_Toc_TocState*f1;struct _tuple14*f2;};static char _tmp12F[8]="*bogus*";
# 542
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple20*env){
# 545
static struct _dyneither_ptr bogus_string={_tmp12F,_tmp12F,_tmp12F + 8};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
static struct _tuple14 pair={& bogus_qvar,& bogus_qvar};
# 549
struct _tuple20 _tmp11A=*env;struct _tuple20 _tmp11B=_tmp11A;struct Cyc_Dict_Dict*_tmp11C;struct _tuple14*_tmp11D;_LL72: _tmp11C=(_tmp11B.f1)->qvar_tags;_tmp11D=_tmp11B.f2;_LL73: {
struct _tuple14 _tmp11E=*_tmp11D;struct _tuple14 _tmp11F=_tmp11E;struct _tuple1*_tmp120;struct _tuple1*_tmp121;_LL77: _tmp120=_tmp11F.f1;_tmp121=_tmp11F.f2;_LL78: {
struct _handler_cons _tmp122;_push_handler(& _tmp122);{int _tmp124=0;if(setjmp(_tmp122.handler))_tmp124=1;if(!_tmp124){
{struct _tuple1*_tmp125=((struct _tuple1*(*)(struct Cyc_Dict_Dict d,int(*cmp)(struct _tuple14*,struct _tuple14*),struct _tuple14*k))Cyc_Dict_lookup_other)(*_tmp11C,Cyc_Toc_qvar_tag_cmp,_tmp11D);_npop_handler(0);return _tmp125;};_pop_handler();}else{void*_tmp123=(void*)_exn_thrown;void*_tmp126=_tmp123;void*_tmp127;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp126)->tag == Cyc_Dict_Absent){_LL7C: _LL7D: {
# 555
struct _tuple14*_tmp975;struct _tuple14*_tmp128=(_tmp975=_cycalloc(sizeof(*_tmp975)),((_tmp975->f1=_tmp120,((_tmp975->f2=_tmp121,_tmp975)))));
struct _tuple1*_tmp129=_tmp120;union Cyc_Absyn_Nmspace _tmp12A;struct _dyneither_ptr _tmp12B;_LL81: _tmp12A=_tmp129->f1;_tmp12B=*_tmp129->f2;_LL82: {
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmp12B,_tmp121);
struct _dyneither_ptr*_tmp978;struct _tuple1*_tmp977;struct _tuple1*res=(_tmp977=_cycalloc(sizeof(*_tmp977)),((_tmp977->f1=_tmp12A,((_tmp977->f2=((_tmp978=_cycalloc(sizeof(*_tmp978)),((_tmp978[0]=newvar,_tmp978)))),_tmp977)))));
*_tmp11C=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmp11C,_tmp128,res);
return res;}_LL80:;}}else{_LL7E: _tmp127=_tmp126;_LL7F:(int)_rethrow(_tmp127);}_LL7B:;}};}_LL76:;}_LL71:;}
# 564
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple14 _tmp979;struct _tuple14 env=(_tmp979.f1=fieldname,((_tmp979.f2=dtname,_tmp979)));
return((struct _tuple1*(*)(struct _tuple14*arg,struct _tuple1*(*f)(struct _RegionHandle*,struct _tuple20*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_collapse_qvars_body);}
# 569
static void*Cyc_Toc_typ_to_c(void*t);struct _tuple21{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple22{void*f1;struct Cyc_List_List*f2;};
# 573
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple21*env){
# 576
struct _tuple21 _tmp131=*env;struct _tuple21 _tmp132=_tmp131;struct Cyc_List_List**_tmp133;struct Cyc_List_List*_tmp134;_LL86: _tmp133=(_tmp132.f1)->tuple_types;_tmp134=_tmp132.f2;_LL87:
# 578
{struct Cyc_List_List*_tmp135=*_tmp133;for(0;_tmp135 != 0;_tmp135=_tmp135->tl){
struct _tuple22*_tmp136=(struct _tuple22*)_tmp135->hd;struct _tuple22*_tmp137=_tmp136;void*_tmp138;struct Cyc_List_List*_tmp139;_LL8B: _tmp138=_tmp137->f1;_tmp139=_tmp137->f2;_LL8C: {
struct Cyc_List_List*_tmp13A=_tmp134;
for(0;_tmp13A != 0  && _tmp139 != 0;(_tmp13A=_tmp13A->tl,_tmp139=_tmp139->tl)){
if(!Cyc_Tcutil_unify((*((struct _tuple11*)_tmp13A->hd)).f2,(void*)_tmp139->hd))
break;}
if(_tmp13A == 0  && _tmp139 == 0)
return _tmp138;}_LL8A:;}}{
# 588
struct Cyc_Int_pa_PrintArg_struct _tmp981;void*_tmp980[1];const char*_tmp97F;struct _dyneither_ptr*_tmp97E;struct _dyneither_ptr*xname=(_tmp97E=_cycalloc(sizeof(*_tmp97E)),((_tmp97E[0]=(struct _dyneither_ptr)((_tmp981.tag=1,((_tmp981.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmp980[0]=& _tmp981,Cyc_aprintf(((_tmp97F="_tuple%d",_tag_dyneither(_tmp97F,sizeof(char),9))),_tag_dyneither(_tmp980,sizeof(void*),1)))))))),_tmp97E)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,_tmp134);
struct Cyc_List_List*_tmp13B=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
struct Cyc_Absyn_Aggrfield*_tmp984;struct Cyc_List_List*_tmp983;_tmp13B=((_tmp983=_cycalloc(sizeof(*_tmp983)),((_tmp983->hd=((_tmp984=_cycalloc(sizeof(*_tmp984)),((_tmp984->name=Cyc_Absyn_fieldname(i),((_tmp984->tq=Cyc_Toc_mt_tq,((_tmp984->type=(void*)ts2->hd,((_tmp984->width=0,((_tmp984->attributes=0,((_tmp984->requires_clause=0,_tmp984)))))))))))))),((_tmp983->tl=_tmp13B,_tmp983))))));}}
_tmp13B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp13B);{
struct Cyc_Absyn_AggrdeclImpl*_tmp989;struct _tuple1*_tmp988;struct Cyc_Absyn_Aggrdecl*_tmp987;struct Cyc_Absyn_Aggrdecl*_tmp13E=(_tmp987=_cycalloc(sizeof(*_tmp987)),((_tmp987->kind=Cyc_Absyn_StructA,((_tmp987->sc=Cyc_Absyn_Public,((_tmp987->name=(
(_tmp988=_cycalloc(sizeof(*_tmp988)),((_tmp988->f1=Cyc_Absyn_Rel_n(0),((_tmp988->f2=xname,_tmp988)))))),((_tmp987->tvs=0,((_tmp987->impl=(
(_tmp989=_cycalloc(sizeof(*_tmp989)),((_tmp989->exist_vars=0,((_tmp989->rgn_po=0,((_tmp989->fields=_tmp13B,((_tmp989->tagged=0,_tmp989)))))))))),((_tmp987->attributes=0,((_tmp987->expected_mem_kind=0,_tmp987)))))))))))))));
# 603
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp98F;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp98E;struct Cyc_List_List*_tmp98D;Cyc_Toc_result_decls=((_tmp98D=_cycalloc(sizeof(*_tmp98D)),((_tmp98D->hd=Cyc_Absyn_new_decl((void*)((_tmp98F=_cycalloc(sizeof(*_tmp98F)),((_tmp98F[0]=((_tmp98E.tag=5,((_tmp98E.f1=_tmp13E,_tmp98E)))),_tmp98F)))),0),((_tmp98D->tl=Cyc_Toc_result_decls,_tmp98D))))));}
{struct _tuple22*_tmp992;struct Cyc_List_List*_tmp991;*_tmp133=((_tmp991=_region_malloc(d,sizeof(*_tmp991)),((_tmp991->hd=((_tmp992=_region_malloc(d,sizeof(*_tmp992)),((_tmp992->f1=x,((_tmp992->f2=ts,_tmp992)))))),((_tmp991->tl=*_tmp133,_tmp991))))));}
return x;};};_LL85:;}
# 608
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return((void*(*)(struct Cyc_List_List*arg,void*(*f)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state)(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple23{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple24{struct Cyc_Toc_TocState*f1;struct _tuple23*f2;};struct _tuple25{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 617
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple24*env){
# 625
struct _tuple24 _tmp14B=*env;struct _tuple24 _tmp14C=_tmp14B;struct Cyc_List_List**_tmp14D;struct _tuple1*_tmp14E;struct Cyc_List_List*_tmp14F;struct Cyc_List_List*_tmp150;struct Cyc_List_List*_tmp151;_LL90: _tmp14D=(_tmp14C.f1)->abs_struct_types;_tmp14E=(_tmp14C.f2)->f1;_tmp14F=(_tmp14C.f2)->f2;_tmp150=(_tmp14C.f2)->f3;_tmp151=(_tmp14C.f2)->f4;_LL91:
# 630
{struct Cyc_List_List*_tmp152=*_tmp14D;for(0;_tmp152 != 0;_tmp152=_tmp152->tl){
struct _tuple25*_tmp153=(struct _tuple25*)_tmp152->hd;struct _tuple25*_tmp154=_tmp153;struct _tuple1*_tmp155;struct Cyc_List_List*_tmp156;void*_tmp157;_LL95: _tmp155=_tmp154->f1;_tmp156=_tmp154->f2;_tmp157=_tmp154->f3;_LL96:
# 633
 if(Cyc_Absyn_qvar_cmp(_tmp155,_tmp14E)== 0  && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp150)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp156)){
int okay=1;
{struct Cyc_List_List*_tmp158=_tmp150;for(0;_tmp158 != 0;(_tmp158=_tmp158->tl,_tmp156=_tmp156->tl)){
void*_tmp159=(void*)_tmp158->hd;
void*_tmp15A=(void*)((struct Cyc_List_List*)_check_null(_tmp156))->hd;
# 640
{struct Cyc_Absyn_Kind*_tmp15B=Cyc_Tcutil_typ_kind(_tmp159);struct Cyc_Absyn_Kind*_tmp15C=_tmp15B;switch(((struct Cyc_Absyn_Kind*)_tmp15C)->kind){case Cyc_Absyn_EffKind: _LL9A: _LL9B:
 goto _LL9D;case Cyc_Absyn_RgnKind: _LL9C: _LL9D:
# 645
 continue;default: _LL9E: _LL9F:
# 648
 if(Cyc_Tcutil_unify(_tmp159,_tmp15A) || Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(_tmp159),Cyc_Toc_typ_to_c(_tmp15A)))
# 650
continue;
# 653
okay=0;
goto _LL99;}_LL99:;}
# 656
break;}}
# 658
if(okay)
# 660
return _tmp157;}_LL94:;}}{
# 668
struct Cyc_Int_pa_PrintArg_struct _tmp99A;void*_tmp999[1];const char*_tmp998;struct _dyneither_ptr*_tmp997;struct _dyneither_ptr*xname=(_tmp997=_cycalloc(sizeof(*_tmp997)),((_tmp997[0]=(struct _dyneither_ptr)((_tmp99A.tag=1,((_tmp99A.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmp999[0]=& _tmp99A,Cyc_aprintf(((_tmp998="_tuple%d",_tag_dyneither(_tmp998,sizeof(char),9))),_tag_dyneither(_tmp999,sizeof(void*),1)))))))),_tmp997)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp15D=0;
# 672
{struct _tuple25*_tmp99D;struct Cyc_List_List*_tmp99C;*_tmp14D=((_tmp99C=_region_malloc(d,sizeof(*_tmp99C)),((_tmp99C->hd=((_tmp99D=_region_malloc(d,sizeof(*_tmp99D)),((_tmp99D->f1=_tmp14E,((_tmp99D->f2=_tmp150,((_tmp99D->f3=x,_tmp99D)))))))),((_tmp99C->tl=*_tmp14D,_tmp99C))))));}{
# 675
struct _RegionHandle _tmp160=_new_region("r");struct _RegionHandle*r=& _tmp160;_push_region(r);
{struct Cyc_List_List*_tmp161=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp14F,_tmp150);
for(0;_tmp151 != 0;_tmp151=_tmp151->tl){
struct Cyc_Absyn_Aggrfield*_tmp162=(struct Cyc_Absyn_Aggrfield*)_tmp151->hd;
void*t=_tmp162->type;
struct Cyc_List_List*atts=_tmp162->attributes;
# 683
if(_tmp151->tl == 0  && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(t))){
struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp9A3;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmp9A2;struct Cyc_List_List*_tmp9A1;atts=((_tmp9A1=_cycalloc(sizeof(*_tmp9A1)),((_tmp9A1->hd=(void*)((_tmp9A3=_cycalloc(sizeof(*_tmp9A3)),((_tmp9A3[0]=((_tmp9A2.tag=6,((_tmp9A2.f1=0,_tmp9A2)))),_tmp9A3)))),((_tmp9A1->tl=atts,_tmp9A1))))));}
# 686
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp161,t));
# 689
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val)){
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp9A9;struct Cyc_Absyn_ArrayInfo _tmp9A8;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp9A7;t=(void*)((_tmp9A7=_cycalloc(sizeof(*_tmp9A7)),((_tmp9A7[0]=((_tmp9A9.tag=8,((_tmp9A9.f1=((_tmp9A8.elt_type=Cyc_Absyn_void_star_typ(),((_tmp9A8.tq=Cyc_Absyn_empty_tqual(0),((_tmp9A8.num_elts=
Cyc_Absyn_uint_exp(0,0),((_tmp9A8.zero_term=((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0),((_tmp9A8.zt_loc=0,_tmp9A8)))))))))),_tmp9A9)))),_tmp9A7))));}{
# 693
struct Cyc_Absyn_Aggrfield*_tmp9AC;struct Cyc_List_List*_tmp9AB;_tmp15D=((_tmp9AB=_cycalloc(sizeof(*_tmp9AB)),((_tmp9AB->hd=((_tmp9AC=_cycalloc(sizeof(*_tmp9AC)),((_tmp9AC->name=_tmp162->name,((_tmp9AC->tq=Cyc_Toc_mt_tq,((_tmp9AC->type=t,((_tmp9AC->width=_tmp162->width,((_tmp9AC->attributes=atts,((_tmp9AC->requires_clause=0,_tmp9AC)))))))))))))),((_tmp9AB->tl=_tmp15D,_tmp9AB))))));};}
# 695
_tmp15D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp15D);{
struct Cyc_Absyn_AggrdeclImpl*_tmp9B1;struct _tuple1*_tmp9B0;struct Cyc_Absyn_Aggrdecl*_tmp9AF;struct Cyc_Absyn_Aggrdecl*_tmp16B=(_tmp9AF=_cycalloc(sizeof(*_tmp9AF)),((_tmp9AF->kind=Cyc_Absyn_StructA,((_tmp9AF->sc=Cyc_Absyn_Public,((_tmp9AF->name=(
(_tmp9B0=_cycalloc(sizeof(*_tmp9B0)),((_tmp9B0->f1=Cyc_Absyn_Rel_n(0),((_tmp9B0->f2=xname,_tmp9B0)))))),((_tmp9AF->tvs=0,((_tmp9AF->impl=(
(_tmp9B1=_cycalloc(sizeof(*_tmp9B1)),((_tmp9B1->exist_vars=0,((_tmp9B1->rgn_po=0,((_tmp9B1->fields=_tmp15D,((_tmp9B1->tagged=0,_tmp9B1)))))))))),((_tmp9AF->attributes=0,((_tmp9AF->expected_mem_kind=0,_tmp9AF)))))))))))))));
# 703
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp9B7;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp9B6;struct Cyc_List_List*_tmp9B5;Cyc_Toc_result_decls=((_tmp9B5=_cycalloc(sizeof(*_tmp9B5)),((_tmp9B5->hd=Cyc_Absyn_new_decl((void*)((_tmp9B7=_cycalloc(sizeof(*_tmp9B7)),((_tmp9B7[0]=((_tmp9B6.tag=5,((_tmp9B6.f1=_tmp16B,_tmp9B6)))),_tmp9B7)))),0),((_tmp9B5->tl=Cyc_Toc_result_decls,_tmp9B5))))));}{
void*_tmp16F=x;_npop_handler(0);return _tmp16F;};};}
# 676
;_pop_region(r);};};_LL8F:;}
# 707
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 711
struct _tuple23 _tmp9B8;struct _tuple23 env=(_tmp9B8.f1=struct_name,((_tmp9B8.f2=type_vars,((_tmp9B8.f3=type_args,((_tmp9B8.f4=fields,_tmp9B8)))))));
return((void*(*)(struct _tuple23*arg,void*(*f)(struct _RegionHandle*,struct _tuple24*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 718
struct _tuple1*Cyc_Toc_temp_var(){
int _tmp178=Cyc_Toc_temp_var_counter ++;
struct _dyneither_ptr*_tmp9C5;const char*_tmp9C4;void*_tmp9C3[1];struct Cyc_Int_pa_PrintArg_struct _tmp9C2;struct _tuple1*_tmp9C1;struct _tuple1*res=(_tmp9C1=_cycalloc(sizeof(*_tmp9C1)),((_tmp9C1->f1=Cyc_Absyn_Loc_n,((_tmp9C1->f2=((_tmp9C5=_cycalloc(sizeof(*_tmp9C5)),((_tmp9C5[0]=(struct _dyneither_ptr)((_tmp9C2.tag=1,((_tmp9C2.f1=(unsigned int)_tmp178,((_tmp9C3[0]=& _tmp9C2,Cyc_aprintf(((_tmp9C4="_tmp%X",_tag_dyneither(_tmp9C4,sizeof(char),7))),_tag_dyneither(_tmp9C3,sizeof(void*),1)))))))),_tmp9C5)))),_tmp9C1)))));
return res;}struct _tuple26{struct Cyc_Toc_TocState*f1;int f2;};
# 726
static struct _dyneither_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple26*env){
struct _tuple26 _tmp17E=*env;struct _tuple26 _tmp17F=_tmp17E;struct Cyc_Xarray_Xarray*_tmp180;_LLA1: _tmp180=(_tmp17F.f1)->temp_labels;_LLA2: {
int _tmp181=Cyc_Toc_fresh_label_counter ++;
if(_tmp181 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp180))
return((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp180,_tmp181);{
struct Cyc_Int_pa_PrintArg_struct _tmp9CD;void*_tmp9CC[1];const char*_tmp9CB;struct _dyneither_ptr*_tmp9CA;struct _dyneither_ptr*res=(_tmp9CA=_cycalloc(sizeof(*_tmp9CA)),((_tmp9CA[0]=(struct _dyneither_ptr)((_tmp9CD.tag=1,((_tmp9CD.f1=(unsigned int)_tmp181,((_tmp9CC[0]=& _tmp9CD,Cyc_aprintf(((_tmp9CB="_LL%X",_tag_dyneither(_tmp9CB,sizeof(char),6))),_tag_dyneither(_tmp9CC,sizeof(void*),1)))))))),_tmp9CA)));
if(((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp180,res)!= _tmp181){
const char*_tmp9D0;void*_tmp9CF;(_tmp9CF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp9D0="fresh_label: add_ind returned bad index...",_tag_dyneither(_tmp9D0,sizeof(char),43))),_tag_dyneither(_tmp9CF,sizeof(void*),0)));}
return res;};}_LLA0:;}
# 737
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
return((struct _dyneither_ptr*(*)(int arg,struct _dyneither_ptr*(*f)(struct _RegionHandle*,struct _tuple26*)))Cyc_Toc_use_toc_state)(0,Cyc_Toc_fresh_label_body);}
# 744
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
struct Cyc_List_List*_tmp188=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp188))->hd)->name)!= 0){
++ ans;
_tmp188=_tmp188->tl;}
# 751
return Cyc_Absyn_uint_exp((unsigned int)ans,0);}
# 757
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_result_decls);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
# 762
struct _tuple9 _tmp189=*a;struct _tuple9 _tmp18A=_tmp189;struct _dyneither_ptr*_tmp18B;struct Cyc_Absyn_Tqual _tmp18C;void*_tmp18D;_LLA6: _tmp18B=_tmp18A.f1;_tmp18C=_tmp18A.f2;_tmp18D=_tmp18A.f3;_LLA7: {
struct _tuple9*_tmp9D1;return(_tmp9D1=_cycalloc(sizeof(*_tmp9D1)),((_tmp9D1->f1=_tmp18B,((_tmp9D1->f2=_tmp18C,((_tmp9D1->f3=Cyc_Toc_typ_to_c(_tmp18D),_tmp9D1)))))));}_LLA5:;}
# 765
static struct _tuple11*Cyc_Toc_typ_to_c_f(struct _tuple11*x){
struct _tuple11 _tmp18F=*x;struct _tuple11 _tmp190=_tmp18F;struct Cyc_Absyn_Tqual _tmp191;void*_tmp192;_LLAB: _tmp191=_tmp190.f1;_tmp192=_tmp190.f2;_LLAC: {
struct _tuple11*_tmp9D2;return(_tmp9D2=_cycalloc(sizeof(*_tmp9D2)),((_tmp9D2->f1=_tmp191,((_tmp9D2->f2=Cyc_Toc_typ_to_c(_tmp192),_tmp9D2)))));}_LLAA:;}
# 784 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp194=Cyc_Tcutil_compress(t);void*_tmp195=_tmp194;void*_tmp196;void*_tmp197;struct Cyc_Absyn_Tqual _tmp198;struct Cyc_Absyn_Exp*_tmp199;union Cyc_Absyn_Constraint*_tmp19A;unsigned int _tmp19B;switch(*((int*)_tmp195)){case 8: _LLB0: _tmp197=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp195)->f1).elt_type;_tmp198=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp195)->f1).tq;_tmp199=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp195)->f1).num_elts;_tmp19A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp195)->f1).zero_term;_tmp19B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp195)->f1).zt_loc;_LLB1:
# 787
 return Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(_tmp197),_tmp198);case 1: _LLB2: _tmp196=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp195)->f2;if(_tmp196 != 0){_LLB3:
 return Cyc_Toc_typ_to_c_array(_tmp196);}else{goto _LLB4;}default: _LLB4: _LLB5:
 return Cyc_Toc_typ_to_c(t);}_LLAF:;}
# 793
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){
# 795
struct Cyc_Absyn_Aggrfield*_tmp9D3;return(_tmp9D3=_cycalloc(sizeof(*_tmp9D3)),((_tmp9D3->name=f->name,((_tmp9D3->tq=Cyc_Toc_mt_tq,((_tmp9D3->type=
# 797
Cyc_Toc_typ_to_c(f->type),((_tmp9D3->width=f->width,((_tmp9D3->attributes=f->attributes,((_tmp9D3->requires_clause=0,_tmp9D3)))))))))))));}
# 802
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 804
return;}
# 807
static void*Cyc_Toc_char_star_typ(){
static void**cs=0;
if(cs == 0){
void**_tmp9D4;cs=((_tmp9D4=_cycalloc(sizeof(*_tmp9D4)),((_tmp9D4[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmp9D4))));}
# 812
return*cs;}
# 814
static void*Cyc_Toc_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmp9D5;r=((_tmp9D5=_cycalloc(sizeof(*_tmp9D5)),((_tmp9D5[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),_tmp9D5))));}
# 819
return*r;}
# 821
static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmp9D6;r=((_tmp9D6=_cycalloc(sizeof(*_tmp9D6)),((_tmp9D6[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),_tmp9D6))));}
# 826
return*r;}
# 828
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp1A0=Cyc_Tcutil_compress(t);void*_tmp1A1=_tmp1A0;struct Cyc_Absyn_Tvar*_tmp1A2;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A1)->tag == 2){_LLB7: _tmp1A2=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A1)->f1;_LLB8:
# 831
 return Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tbk);}else{_LLB9: _LLBA:
 return 0;}_LLB6:;}
# 835
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp1A3=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp1A4=_tmp1A3;if(((struct Cyc_Absyn_Kind*)_tmp1A4)->kind == Cyc_Absyn_AnyKind){_LLBC: _LLBD:
 return 1;}else{_LLBE: _LLBF:
 return 0;}_LLBB:;}
# 842
static void*Cyc_Toc_typ_to_c(void*t){
void*_tmp1A5=t;struct Cyc_Absyn_Datatypedecl*_tmp1A6;void**_tmp1A7;struct Cyc_Absyn_Enumdecl*_tmp1A8;struct Cyc_Absyn_Aggrdecl*_tmp1A9;struct Cyc_Absyn_Exp*_tmp1AA;struct Cyc_Absyn_Exp*_tmp1AB;void*_tmp1AC;struct _tuple1*_tmp1AD;struct Cyc_List_List*_tmp1AE;struct Cyc_Absyn_Typedefdecl*_tmp1AF;void*_tmp1B0;struct Cyc_List_List*_tmp1B1;struct _tuple1*_tmp1B2;union Cyc_Absyn_AggrInfoU _tmp1B3;struct Cyc_List_List*_tmp1B4;enum Cyc_Absyn_AggrKind _tmp1B5;struct Cyc_List_List*_tmp1B6;struct Cyc_List_List*_tmp1B7;struct Cyc_Absyn_Tqual _tmp1B8;void*_tmp1B9;struct Cyc_List_List*_tmp1BA;int _tmp1BB;struct Cyc_Absyn_VarargInfo*_tmp1BC;struct Cyc_List_List*_tmp1BD;void*_tmp1BE;struct Cyc_Absyn_Tqual _tmp1BF;struct Cyc_Absyn_Exp*_tmp1C0;unsigned int _tmp1C1;void*_tmp1C2;struct Cyc_Absyn_Tqual _tmp1C3;union Cyc_Absyn_Constraint*_tmp1C4;struct Cyc_Absyn_Datatypedecl*_tmp1C5;struct Cyc_Absyn_Datatypefield*_tmp1C6;struct Cyc_Absyn_Tvar*_tmp1C7;void**_tmp1C8;switch(*((int*)_tmp1A5)){case 0: _LLC1: _LLC2:
 return t;case 1: _LLC3: _tmp1C8=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A5)->f2;_LLC4:
# 846
 if(*_tmp1C8 == 0){
*_tmp1C8=Cyc_Absyn_sint_typ;
return Cyc_Absyn_sint_typ;}
# 850
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp1C8));case 2: _LLC5: _tmp1C7=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A5)->f1;_LLC6:
# 852
 if((Cyc_Tcutil_tvar_kind(_tmp1C7,& Cyc_Tcutil_bk))->kind == Cyc_Absyn_AnyKind)
# 855
return(void*)& Cyc_Absyn_VoidType_val;else{
# 857
return Cyc_Absyn_void_star_typ();}case 3: _LLC7: _LLC8:
# 859
 return(void*)& Cyc_Absyn_VoidType_val;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1A5)->f1).field_info).KnownDatatypefield).tag == 2){_LLC9: _tmp1C5=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1A5)->f1).field_info).KnownDatatypefield).val).f1;_tmp1C6=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1A5)->f1).field_info).KnownDatatypefield).val).f2;_LLCA:
# 861
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp1C6->name,_tmp1C5->name));}else{_LLCB: _LLCC: {
const char*_tmp9D9;void*_tmp9D8;(_tmp9D8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp9D9="unresolved DatatypeFieldType",_tag_dyneither(_tmp9D9,sizeof(char),29))),_tag_dyneither(_tmp9D8,sizeof(void*),0)));}}case 5: _LLCD: _tmp1C2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A5)->f1).elt_typ;_tmp1C3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A5)->f1).elt_tq;_tmp1C4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A5)->f1).ptr_atts).bounds;_LLCE:
# 866
 _tmp1C2=Cyc_Toc_typ_to_c(_tmp1C2);{
void*_tmp1CB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp1C4);void*_tmp1CC=_tmp1CB;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1CC)->tag == 0){_LL102: _LL103:
 return Cyc_Toc_dyneither_ptr_typ;}else{_LL104: _LL105:
 return Cyc_Absyn_star_typ(_tmp1C2,(void*)& Cyc_Absyn_HeapRgn_val,_tmp1C3,Cyc_Absyn_false_conref);}_LL101:;};case 6: _LLCF: _LLD0:
# 871
 goto _LLD2;case 7: _LLD1: _LLD2:
 return t;case 8: _LLD3: _tmp1BE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1A5)->f1).elt_type;_tmp1BF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1A5)->f1).tq;_tmp1C0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1A5)->f1).num_elts;_tmp1C1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1A5)->f1).zt_loc;_LLD4:
# 874
 return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c(_tmp1BE),_tmp1BF,_tmp1C0,Cyc_Absyn_false_conref,_tmp1C1);case 9: _LLD5: _tmp1B8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1A5)->f1).ret_tqual;_tmp1B9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1A5)->f1).ret_typ;_tmp1BA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1A5)->f1).args;_tmp1BB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1A5)->f1).c_varargs;_tmp1BC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1A5)->f1).cyc_varargs;_tmp1BD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1A5)->f1).attributes;_LLD6: {
# 880
struct Cyc_List_List*_tmp1CD=0;
for(0;_tmp1BD != 0;_tmp1BD=_tmp1BD->tl){
void*_tmp1CE=(void*)_tmp1BD->hd;void*_tmp1CF=_tmp1CE;switch(*((int*)_tmp1CF)){case 4: _LL107: _LL108:
 goto _LL10A;case 5: _LL109: _LL10A:
 goto _LL10C;case 19: _LL10B: _LL10C:
 continue;case 22: _LL10D: _LL10E:
 continue;case 21: _LL10F: _LL110:
 continue;case 20: _LL111: _LL112:
 continue;default: _LL113: _LL114:
{struct Cyc_List_List*_tmp9DA;_tmp1CD=((_tmp9DA=_cycalloc(sizeof(*_tmp9DA)),((_tmp9DA->hd=(void*)_tmp1BD->hd,((_tmp9DA->tl=_tmp1CD,_tmp9DA))))));}goto _LL106;}_LL106:;}{
# 891
struct Cyc_List_List*_tmp1D1=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp1BA);
if(_tmp1BC != 0){
# 894
void*_tmp1D2=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp1BC->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple9*_tmp9DB;struct _tuple9*_tmp1D3=(_tmp9DB=_cycalloc(sizeof(*_tmp9DB)),((_tmp9DB->f1=_tmp1BC->name,((_tmp9DB->f2=_tmp1BC->tq,((_tmp9DB->f3=_tmp1D2,_tmp9DB)))))));
struct Cyc_List_List*_tmp9DC;_tmp1D1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1D1,((_tmp9DC=_cycalloc(sizeof(*_tmp9DC)),((_tmp9DC->hd=_tmp1D3,((_tmp9DC->tl=0,_tmp9DC)))))));}{
# 898
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp9E2;struct Cyc_Absyn_FnInfo _tmp9E1;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp9E0;return(void*)((_tmp9E0=_cycalloc(sizeof(*_tmp9E0)),((_tmp9E0[0]=((_tmp9E2.tag=9,((_tmp9E2.f1=((_tmp9E1.tvars=0,((_tmp9E1.effect=0,((_tmp9E1.ret_tqual=_tmp1B8,((_tmp9E1.ret_typ=Cyc_Toc_typ_to_c(_tmp1B9),((_tmp9E1.args=_tmp1D1,((_tmp9E1.c_varargs=_tmp1BB,((_tmp9E1.cyc_varargs=0,((_tmp9E1.rgn_po=0,((_tmp9E1.attributes=_tmp1CD,((_tmp9E1.requires_clause=0,((_tmp9E1.requires_relns=0,((_tmp9E1.ensures_clause=0,((_tmp9E1.ensures_relns=0,_tmp9E1)))))))))))))))))))))))))),_tmp9E2)))),_tmp9E0))));};};}case 10: _LLD7: _tmp1B7=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1A5)->f1;_LLD8:
# 903
 _tmp1B7=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp1B7);
return Cyc_Toc_add_tuple_type(_tmp1B7);case 12: _LLD9: _tmp1B5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1A5)->f1;_tmp1B6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1A5)->f2;_LLDA: {
# 908
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp9E5;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp9E4;return(void*)((_tmp9E4=_cycalloc(sizeof(*_tmp9E4)),((_tmp9E4[0]=((_tmp9E5.tag=12,((_tmp9E5.f1=_tmp1B5,((_tmp9E5.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp1B6),_tmp9E5)))))),_tmp9E4))));}case 11: _LLDB: _tmp1B3=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A5)->f1).aggr_info;_tmp1B4=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A5)->f1).targs;_LLDC:
# 912
{union Cyc_Absyn_AggrInfoU _tmp1DB=_tmp1B3;if((_tmp1DB.UnknownAggr).tag == 1){_LL116: _LL117:
 return t;}else{_LL118: _LL119:
 goto _LL115;}_LL115:;}{
# 916
struct Cyc_Absyn_Aggrdecl*_tmp1DC=Cyc_Absyn_get_known_aggrdecl(_tmp1B3);
if(_tmp1DC->expected_mem_kind){
# 919
if(_tmp1DC->impl == 0){
const char*_tmp9EE;void*_tmp9ED[2];const char*_tmp9EC;const char*_tmp9EB;struct Cyc_String_pa_PrintArg_struct _tmp9EA;struct Cyc_String_pa_PrintArg_struct _tmp9E9;(_tmp9E9.tag=0,((_tmp9E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 922
Cyc_Absynpp_qvar2string(_tmp1DC->name)),((_tmp9EA.tag=0,((_tmp9EA.f1=(struct _dyneither_ptr)(
# 921
_tmp1DC->kind == Cyc_Absyn_UnionA?(_tmp9EB="union",_tag_dyneither(_tmp9EB,sizeof(char),6)):((_tmp9EC="struct",_tag_dyneither(_tmp9EC,sizeof(char),7)))),((_tmp9ED[0]=& _tmp9EA,((_tmp9ED[1]=& _tmp9E9,Cyc_Tcutil_warn(0,((_tmp9EE="%s %s was never defined.",_tag_dyneither(_tmp9EE,sizeof(char),25))),_tag_dyneither(_tmp9ED,sizeof(void*),2)))))))))))));}}
# 927
if(_tmp1DC->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp1DC->name,Cyc_Absyn_unionq_typ);{
struct Cyc_List_List*_tmp1E3=_tmp1DC->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1DC->impl))->fields;
if(_tmp1E3 == 0)return Cyc_Toc_aggrdecl_type(_tmp1DC->name,Cyc_Absyn_strctq);
for(0;_tmp1E3->tl != 0;_tmp1E3=_tmp1E3->tl){;}{
void*_tmp1E4=((struct Cyc_Absyn_Aggrfield*)_tmp1E3->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp1E4))){
if(_tmp1DC->expected_mem_kind){
const char*_tmp9F2;void*_tmp9F1[1];struct Cyc_String_pa_PrintArg_struct _tmp9F0;(_tmp9F0.tag=0,((_tmp9F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp1DC->name)),((_tmp9F1[0]=& _tmp9F0,Cyc_Tcutil_warn(0,((_tmp9F2="struct %s ended up being abstract.",_tag_dyneither(_tmp9F2,sizeof(char),35))),_tag_dyneither(_tmp9F1,sizeof(void*),1)))))));}{
# 941
struct _RegionHandle _tmp1E8=_new_region("r");struct _RegionHandle*r=& _tmp1E8;_push_region(r);
{struct Cyc_List_List*_tmp1E9=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp1DC->tvs,_tmp1B4);
void*_tmp1EA=Cyc_Tcutil_rsubstitute(r,_tmp1E9,_tmp1E4);
if(Cyc_Toc_is_abstract_type(_tmp1EA)){void*_tmp1EB=Cyc_Toc_aggrdecl_type(_tmp1DC->name,Cyc_Absyn_strctq);_npop_handler(0);return _tmp1EB;}{
void*_tmp1EC=Cyc_Toc_add_struct_type(_tmp1DC->name,_tmp1DC->tvs,_tmp1B4,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1DC->impl))->fields);_npop_handler(0);return _tmp1EC;};}
# 942
;_pop_region(r);};}
# 947
return Cyc_Toc_aggrdecl_type(_tmp1DC->name,Cyc_Absyn_strctq);};};};case 13: _LLDD: _tmp1B2=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp1A5)->f1;_LLDE:
 return t;case 14: _LLDF: _tmp1B1=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1A5)->f1;_LLE0:
 Cyc_Toc_enumfields_to_c(_tmp1B1);return t;case 17: _LLE1: _tmp1AD=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1A5)->f1;_tmp1AE=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1A5)->f2;_tmp1AF=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1A5)->f3;_tmp1B0=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1A5)->f4;_LLE2:
# 951
 if(_tmp1B0 == 0  || Cyc_noexpand_r){
if(_tmp1AE != 0){
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp9F5;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp9F4;return(void*)((_tmp9F4=_cycalloc(sizeof(*_tmp9F4)),((_tmp9F4[0]=((_tmp9F5.tag=17,((_tmp9F5.f1=_tmp1AD,((_tmp9F5.f2=0,((_tmp9F5.f3=_tmp1AF,((_tmp9F5.f4=0,_tmp9F5)))))))))),_tmp9F4))));}else{
return t;}}else{
# 956
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp9F8;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp9F7;return(void*)((_tmp9F7=_cycalloc(sizeof(*_tmp9F7)),((_tmp9F7[0]=((_tmp9F8.tag=17,((_tmp9F8.f1=_tmp1AD,((_tmp9F8.f2=0,((_tmp9F8.f3=_tmp1AF,((_tmp9F8.f4=Cyc_Toc_typ_to_c(_tmp1B0),_tmp9F8)))))))))),_tmp9F7))));}case 19: _LLE3: _LLE4:
 return Cyc_Absyn_uint_typ;case 15: _LLE5: _tmp1AC=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1A5)->f1;_LLE6:
 return Cyc_Toc_rgn_typ();case 16: _LLE7: _LLE8:
 return Cyc_Toc_dyn_rgn_typ();case 20: _LLE9: _LLEA:
# 962
 goto _LLEC;case 21: _LLEB: _LLEC:
 goto _LLEE;case 22: _LLED: _LLEE:
 goto _LLF0;case 23: _LLEF: _LLF0:
 goto _LLF2;case 24: _LLF1: _LLF2:
 goto _LLF4;case 25: _LLF3: _LLF4:
 return Cyc_Absyn_void_star_typ();case 18: _LLF5: _tmp1AB=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp1A5)->f1;_LLF6:
# 972
 return t;case 27: _LLF7: _tmp1AA=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp1A5)->f1;_LLF8:
# 974
 return t;case 28: _LLF9: _LLFA:
 return t;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1A5)->f1)->r)){case 0: _LLFB: _tmp1A9=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1A5)->f1)->r)->f1;_LLFC:
# 977
 Cyc_Toc_aggrdecl_to_c(_tmp1A9,1);
if(_tmp1A9->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp1A9->name,Cyc_Absyn_unionq_typ);else{
return Cyc_Toc_aggrdecl_type(_tmp1A9->name,Cyc_Absyn_strctq);}case 1: _LLFD: _tmp1A8=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1A5)->f1)->r)->f1;_LLFE:
# 982
 Cyc_Toc_enumdecl_to_c(_tmp1A8);
return t;default: _LLFF: _tmp1A6=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1A5)->f1)->r)->f1;_tmp1A7=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1A5)->f2;_LL100:
# 985
 Cyc_Toc_datatypedecl_to_c(_tmp1A6);
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp1A7)));}}_LLC0:;}
# 990
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned int l){
void*_tmp1F1=t;void*_tmp1F2;struct Cyc_Absyn_Tqual _tmp1F3;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F1)->tag == 8){_LL11B: _tmp1F2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F1)->f1).elt_type;_tmp1F3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F1)->f1).tq;_LL11C:
# 993
 return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp1F2,(void*)& Cyc_Absyn_HeapRgn_val,_tmp1F3,Cyc_Absyn_false_conref),e);}else{_LL11D: _LL11E:
 return Cyc_Toc_cast_it(t,e);}_LL11A:;}
# 1000
static int Cyc_Toc_atomic_typ(void*t){
void*_tmp1F4=Cyc_Tcutil_compress(t);void*_tmp1F5=_tmp1F4;struct Cyc_List_List*_tmp1F6;struct Cyc_Absyn_Datatypedecl*_tmp1F7;struct Cyc_Absyn_Datatypefield*_tmp1F8;struct Cyc_List_List*_tmp1F9;union Cyc_Absyn_AggrInfoU _tmp1FA;void*_tmp1FB;switch(*((int*)_tmp1F5)){case 0: _LL120: _LL121:
 return 1;case 2: _LL122: _LL123:
 return 0;case 6: _LL124: _LL125:
 goto _LL127;case 13: _LL126: _LL127:
 goto _LL129;case 14: _LL128: _LL129:
 goto _LL12B;case 7: _LL12A: _LL12B:
 goto _LL12D;case 9: _LL12C: _LL12D:
 goto _LL12F;case 19: _LL12E: _LL12F:
 return 1;case 8: _LL130: _tmp1FB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F5)->f1).elt_type;_LL131:
 return Cyc_Toc_atomic_typ(_tmp1FB);case 11: _LL132: _tmp1FA=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1F5)->f1).aggr_info;_LL133:
# 1017
{union Cyc_Absyn_AggrInfoU _tmp1FC=_tmp1FA;if((_tmp1FC.UnknownAggr).tag == 1){_LL145: _LL146:
 return 0;}else{_LL147: _LL148:
 goto _LL144;}_LL144:;}{
# 1021
struct Cyc_Absyn_Aggrdecl*_tmp1FD=Cyc_Absyn_get_known_aggrdecl(_tmp1FA);
if(_tmp1FD->impl == 0)
return 0;
{struct Cyc_List_List*_tmp1FE=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1FD->impl))->fields;for(0;_tmp1FE != 0;_tmp1FE=_tmp1FE->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp1FE->hd)->type))return 0;}}
return 1;};case 12: _LL134: _tmp1F9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1F5)->f2;_LL135:
# 1028
 for(0;_tmp1F9 != 0;_tmp1F9=_tmp1F9->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp1F9->hd)->type))return 0;}
return 1;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1F5)->f1).field_info).KnownDatatypefield).tag == 2){_LL136: _tmp1F7=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1F5)->f1).field_info).KnownDatatypefield).val).f1;_tmp1F8=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1F5)->f1).field_info).KnownDatatypefield).val).f2;_LL137:
# 1032
 _tmp1F6=_tmp1F8->typs;goto _LL139;}else{goto _LL142;}case 10: _LL138: _tmp1F6=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1F5)->f1;_LL139:
# 1034
 for(0;_tmp1F6 != 0;_tmp1F6=_tmp1F6->tl){
if(!Cyc_Toc_atomic_typ((*((struct _tuple11*)_tmp1F6->hd)).f2))return 0;}
return 1;case 3: _LL13A: _LL13B:
# 1039
 goto _LL13D;case 5: _LL13C: _LL13D:
 goto _LL13F;case 16: _LL13E: _LL13F:
 goto _LL141;case 15: _LL140: _LL141:
 return 0;default: _LL142: _LL143: {
const char*_tmp9FC;void*_tmp9FB[1];struct Cyc_String_pa_PrintArg_struct _tmp9FA;(_tmp9FA.tag=0,((_tmp9FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp9FB[0]=& _tmp9FA,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp9FC="atomic_typ:  bad type %s",_tag_dyneither(_tmp9FC,sizeof(char),25))),_tag_dyneither(_tmp9FB,sizeof(void*),1)))))));}}_LL11F:;}
# 1047
static int Cyc_Toc_is_void_star(void*t){
void*_tmp202=Cyc_Tcutil_compress(t);void*_tmp203=_tmp202;void*_tmp204;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp203)->tag == 5){_LL14A: _tmp204=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp203)->f1).elt_typ;_LL14B: {
# 1050
void*_tmp205=Cyc_Tcutil_compress(_tmp204);void*_tmp206=_tmp205;if(((struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp206)->tag == 0){_LL14F: _LL150:
 return 1;}else{_LL151: _LL152:
 return 0;}_LL14E:;}}else{_LL14C: _LL14D:
# 1054
 return 0;}_LL149:;}
# 1058
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1062
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1067
static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){
void*_tmp207=Cyc_Tcutil_compress(t);void*_tmp208=_tmp207;struct Cyc_List_List*_tmp209;union Cyc_Absyn_AggrInfoU _tmp20A;switch(*((int*)_tmp208)){case 11: _LL154: _tmp20A=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp208)->f1).aggr_info;_LL155: {
# 1070
struct Cyc_Absyn_Aggrdecl*_tmp20B=Cyc_Absyn_get_known_aggrdecl(_tmp20A);
if(_tmp20B->impl == 0){
const char*_tmp9FF;void*_tmp9FE;(_tmp9FE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp9FF="is_poly_field: type missing fields",_tag_dyneither(_tmp9FF,sizeof(char),35))),_tag_dyneither(_tmp9FE,sizeof(void*),0)));}
_tmp209=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp20B->impl))->fields;goto _LL157;}case 12: _LL156: _tmp209=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp208)->f2;_LL157: {
# 1075
struct Cyc_Absyn_Aggrfield*_tmp20E=Cyc_Absyn_lookup_field(_tmp209,f);
if(_tmp20E == 0){
const char*_tmpA03;void*_tmpA02[1];struct Cyc_String_pa_PrintArg_struct _tmpA01;(_tmpA01.tag=0,((_tmpA01.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpA02[0]=& _tmpA01,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA03="is_poly_field: bad field %s",_tag_dyneither(_tmpA03,sizeof(char),28))),_tag_dyneither(_tmpA02,sizeof(void*),1)))))));}
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp20E->type);}default: _LL158: _LL159: {
const char*_tmpA07;void*_tmpA06[1];struct Cyc_String_pa_PrintArg_struct _tmpA05;(_tmpA05.tag=0,((_tmpA05.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA06[0]=& _tmpA05,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA07="is_poly_field: bad type %s",_tag_dyneither(_tmpA07,sizeof(char),27))),_tag_dyneither(_tmpA06,sizeof(void*),1)))))));}}_LL153:;}
# 1086
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp215=e->r;void*_tmp216=_tmp215;struct Cyc_Absyn_Exp*_tmp217;struct _dyneither_ptr*_tmp218;struct Cyc_Absyn_Exp*_tmp219;struct _dyneither_ptr*_tmp21A;switch(*((int*)_tmp216)){case 20: _LL15B: _tmp219=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp216)->f1;_tmp21A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp216)->f2;_LL15C:
# 1089
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp219->topt),_tmp21A) && !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));case 21: _LL15D: _tmp217=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp216)->f1;_tmp218=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp216)->f2;_LL15E: {
# 1092
void*_tmp21B=Cyc_Tcutil_compress((void*)_check_null(_tmp217->topt));void*_tmp21C=_tmp21B;void*_tmp21D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21C)->tag == 5){_LL162: _tmp21D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21C)->f1).elt_typ;_LL163:
# 1094
 return Cyc_Toc_is_poly_field(_tmp21D,_tmp218) && !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}else{_LL164: _LL165: {
const char*_tmpA0B;void*_tmpA0A[1];struct Cyc_String_pa_PrintArg_struct _tmpA09;(_tmpA09.tag=0,((_tmpA09.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp217->topt))),((_tmpA0A[0]=& _tmpA09,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA0B="is_poly_project: bad type %s",_tag_dyneither(_tmpA0B,sizeof(char),29))),_tag_dyneither(_tmpA0A,sizeof(void*),1)))))));}}_LL161:;}default: _LL15F: _LL160:
# 1097
 return 0;}_LL15A:;}
# 1102
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpA0C;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((_tmpA0C=_cycalloc(sizeof(*_tmpA0C)),((_tmpA0C->hd=s,((_tmpA0C->tl=0,_tmpA0C)))))),0);}
# 1106
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpA0D;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((_tmpA0D=_cycalloc(sizeof(*_tmpA0D)),((_tmpA0D->hd=s,((_tmpA0D->tl=0,_tmpA0D)))))),0);}
# 1110
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1116
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*_tmpA0E[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmpA0E[1]=s,((_tmpA0E[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA0E,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
# 1120
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*_tmpA0F[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__fast_region_malloc_e,((_tmpA0F[1]=s,((_tmpA0F[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA0F,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
# 1124
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type)){
struct Cyc_Absyn_Exp*_tmpA10[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpA10[1]=n,((_tmpA10[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA10,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{
# 1128
struct Cyc_Absyn_Exp*_tmpA11[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpA11[1]=n,((_tmpA11[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA11,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}
# 1131
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*_tmpA12[3];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,((_tmpA12[2]=n,((_tmpA12[1]=s,((_tmpA12[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA12,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
# 1136
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
struct Cyc_List_List*_tmpA13;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((_tmpA13=_cycalloc(sizeof(*_tmpA13)),((_tmpA13->hd=e,((_tmpA13->tl=0,_tmpA13)))))),0);}
# 1139
static struct Cyc_Absyn_Exp*Cyc_Toc_newrethrow_exp(struct Cyc_Absyn_Exp*e){
struct Cyc_List_List*_tmpA14;return Cyc_Absyn_fncall_exp(Cyc_Toc__rethrow_e,((_tmpA14=_cycalloc(sizeof(*_tmpA14)),((_tmpA14->hd=e,((_tmpA14->tl=0,_tmpA14)))))),0);}
# 1143
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
struct Cyc_Absyn_Exp*_tmpA15[0];return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA15,sizeof(struct Cyc_Absyn_Exp*),0)),0),0);}
# 1149
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1157
int is_string=0;
{void*_tmp22B=e->r;void*_tmp22C=_tmp22B;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp22C)->tag == 0)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp22C)->f1).Wstring_c).tag){case 8: _LL167: _LL168:
 is_string=1;goto _LL166;case 9: _LL169: _LL16A:
 is_string=1;goto _LL166;default: goto _LL16B;}else{_LL16B: _LL16C:
 goto _LL166;}_LL166:;}{
# 1163
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
if(is_string){
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_conref,0);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpA1B;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpA1A;struct Cyc_List_List*_tmpA19;Cyc_Toc_result_decls=((_tmpA19=_cycalloc(sizeof(*_tmpA19)),((_tmpA19->hd=Cyc_Absyn_new_decl((void*)((_tmpA1B=_cycalloc(sizeof(*_tmpA1B)),((_tmpA1B[0]=((_tmpA1A.tag=0,((_tmpA1A.f1=vd,_tmpA1A)))),_tmpA1B)))),0),((_tmpA19->tl=Cyc_Toc_result_decls,_tmpA19))))));}
xexp=Cyc_Absyn_var_exp(x,0);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{
# 1173
xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),e);
# 1175
xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
# 1177
struct Cyc_Absyn_Exp*urm_exp;
{struct _tuple19*_tmpA22;struct _tuple19*_tmpA21;struct _tuple19*_tmpA20;struct _tuple19*_tmpA1F[3];urm_exp=Cyc_Absyn_unresolvedmem_exp(0,((_tmpA1F[2]=(
# 1180
(_tmpA20=_cycalloc(sizeof(*_tmpA20)),((_tmpA20->f1=0,((_tmpA20->f2=xplussz,_tmpA20)))))),((_tmpA1F[1]=(
# 1179
(_tmpA21=_cycalloc(sizeof(*_tmpA21)),((_tmpA21->f1=0,((_tmpA21->f2=xexp,_tmpA21)))))),((_tmpA1F[0]=(
# 1178
(_tmpA22=_cycalloc(sizeof(*_tmpA22)),((_tmpA22->f1=0,((_tmpA22->f2=xexp,_tmpA22)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA1F,sizeof(struct _tuple19*),3)))))))),0);}
# 1181
return urm_exp;};};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_StructInfo{struct Cyc_Absyn_Exp**varsizeexp;struct Cyc_Absyn_Exp*lhs_exp;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;int*in_lhs;struct Cyc_Toc_StructInfo struct_info;int*in_sizeof;struct _RegionHandle*rgn;};
# 1224 "toc.cyc"
typedef struct Cyc_Toc_Env*Cyc_Toc_env_t;
# 1226
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp234=nv;int _tmp235;_LL16E: _tmp235=_tmp234->toplevel;_LL16F:
 return _tmp235;_LL16D:;}
# 1230
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp236=nv;int*_tmp237;_LL173: _tmp237=_tmp236->in_lhs;_LL174:
 return*_tmp237;_LL172:;}
# 1235
static struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple1*x){
struct Cyc_Toc_Env*_tmp238=nv;struct Cyc_Dict_Dict _tmp239;_LL178: _tmp239=_tmp238->varmap;_LL179:
 return((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp239,x);_LL177:;}
# 1241
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
int*_tmpA2A;struct Cyc_Absyn_Exp**_tmpA29;int*_tmpA28;struct Cyc_Toc_Env*_tmpA27;return(_tmpA27=_region_malloc(r,sizeof(*_tmpA27)),((_tmpA27->break_lab=(struct _dyneither_ptr**)0,((_tmpA27->continue_lab=(struct _dyneither_ptr**)0,((_tmpA27->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpA27->varmap=(struct Cyc_Dict_Dict)
# 1245
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((_tmpA27->toplevel=(int)1,((_tmpA27->in_lhs=(int*)(
# 1247
(_tmpA28=_region_malloc(r,sizeof(*_tmpA28)),((_tmpA28[0]=0,_tmpA28)))),((_tmpA27->struct_info=(struct Cyc_Toc_StructInfo)(
((_tmpA27->struct_info).varsizeexp=((_tmpA29=_cycalloc(sizeof(*_tmpA29)),((_tmpA29[0]=0,_tmpA29)))),(((_tmpA27->struct_info).lhs_exp=0,_tmpA27->struct_info)))),((_tmpA27->in_sizeof=(int*)(
(_tmpA2A=_region_malloc(r,sizeof(*_tmpA2A)),((_tmpA2A[0]=0,_tmpA2A)))),((_tmpA27->rgn=(struct _RegionHandle*)r,_tmpA27)))))))))))))))))));}
# 1253
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp23E=e;struct _dyneither_ptr**_tmp23F;struct _dyneither_ptr**_tmp240;struct Cyc_Toc_FallthruInfo*_tmp241;struct Cyc_Dict_Dict _tmp242;int _tmp243;int*_tmp244;struct Cyc_Toc_StructInfo _tmp245;int*_tmp246;_LL17D: _tmp23F=_tmp23E->break_lab;_tmp240=_tmp23E->continue_lab;_tmp241=_tmp23E->fallthru_info;_tmp242=_tmp23E->varmap;_tmp243=_tmp23E->toplevel;_tmp244=_tmp23E->in_lhs;_tmp245=_tmp23E->struct_info;_tmp246=_tmp23E->in_sizeof;_LL17E: {
struct Cyc_Toc_Env*_tmpA2B;return(_tmpA2B=_region_malloc(r,sizeof(*_tmpA2B)),((_tmpA2B->break_lab=(struct _dyneither_ptr**)_tmp23F,((_tmpA2B->continue_lab=(struct _dyneither_ptr**)_tmp240,((_tmpA2B->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp241,((_tmpA2B->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp242),((_tmpA2B->toplevel=(int)_tmp243,((_tmpA2B->in_lhs=(int*)_tmp244,((_tmpA2B->struct_info=(struct Cyc_Toc_StructInfo)_tmp245,((_tmpA2B->in_sizeof=(int*)_tmp246,((_tmpA2B->rgn=(struct _RegionHandle*)r,_tmpA2B)))))))))))))))))));}_LL17C:;}
# 1258
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp248=e;struct _dyneither_ptr**_tmp249;struct _dyneither_ptr**_tmp24A;struct Cyc_Toc_FallthruInfo*_tmp24B;struct Cyc_Dict_Dict _tmp24C;int _tmp24D;int*_tmp24E;struct Cyc_Toc_StructInfo _tmp24F;int*_tmp250;_LL182: _tmp249=_tmp248->break_lab;_tmp24A=_tmp248->continue_lab;_tmp24B=_tmp248->fallthru_info;_tmp24C=_tmp248->varmap;_tmp24D=_tmp248->toplevel;_tmp24E=_tmp248->in_lhs;_tmp24F=_tmp248->struct_info;_tmp250=_tmp248->in_sizeof;_LL183: {
struct Cyc_Toc_Env*_tmpA2C;return(_tmpA2C=_region_malloc(r,sizeof(*_tmpA2C)),((_tmpA2C->break_lab=(struct _dyneither_ptr**)_tmp249,((_tmpA2C->continue_lab=(struct _dyneither_ptr**)_tmp24A,((_tmpA2C->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp24B,((_tmpA2C->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp24C),((_tmpA2C->toplevel=(int)0,((_tmpA2C->in_lhs=(int*)_tmp24E,((_tmpA2C->struct_info=(struct Cyc_Toc_StructInfo)_tmp24F,((_tmpA2C->in_sizeof=(int*)_tmp250,((_tmpA2C->rgn=(struct _RegionHandle*)r,_tmpA2C)))))))))))))))))));}_LL181:;}
# 1262
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp252=e;struct _dyneither_ptr**_tmp253;struct _dyneither_ptr**_tmp254;struct Cyc_Toc_FallthruInfo*_tmp255;struct Cyc_Dict_Dict _tmp256;int _tmp257;int*_tmp258;struct Cyc_Toc_StructInfo _tmp259;int*_tmp25A;_LL187: _tmp253=_tmp252->break_lab;_tmp254=_tmp252->continue_lab;_tmp255=_tmp252->fallthru_info;_tmp256=_tmp252->varmap;_tmp257=_tmp252->toplevel;_tmp258=_tmp252->in_lhs;_tmp259=_tmp252->struct_info;_tmp25A=_tmp252->in_sizeof;_LL188: {
struct Cyc_Toc_Env*_tmpA2D;return(_tmpA2D=_region_malloc(r,sizeof(*_tmpA2D)),((_tmpA2D->break_lab=(struct _dyneither_ptr**)_tmp253,((_tmpA2D->continue_lab=(struct _dyneither_ptr**)_tmp254,((_tmpA2D->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp255,((_tmpA2D->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp256),((_tmpA2D->toplevel=(int)1,((_tmpA2D->in_lhs=(int*)_tmp258,((_tmpA2D->struct_info=(struct Cyc_Toc_StructInfo)_tmp259,((_tmpA2D->in_sizeof=(int*)_tmp25A,((_tmpA2D->rgn=(struct _RegionHandle*)r,_tmpA2D)))))))))))))))))));}_LL186:;}
# 1266
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp25C=e;int*_tmp25D;_LL18C: _tmp25D=_tmp25C->in_lhs;_LL18D:
*_tmp25D=b;_LL18B:;}
# 1270
static int Cyc_Toc_set_in_sizeof(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp25E=e;int*_tmp25F;_LL191: _tmp25F=_tmp25E->in_sizeof;_LL192: {
int _tmp260=*_tmp25F;
*_tmp25F=b;
return _tmp260;}_LL190:;}
# 1276
static int Cyc_Toc_in_sizeof(struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp261=e;int*_tmp262;_LL196: _tmp262=_tmp261->in_sizeof;_LL197:
 return*_tmp262;_LL195:;}
# 1281
static struct Cyc_Toc_Env*Cyc_Toc_set_lhs_exp(struct _RegionHandle*r,struct Cyc_Absyn_Exp*exp,struct Cyc_Toc_Env*e){
# 1283
struct Cyc_Toc_Env*_tmp263=e;struct _dyneither_ptr**_tmp264;struct _dyneither_ptr**_tmp265;struct Cyc_Toc_FallthruInfo*_tmp266;struct Cyc_Dict_Dict _tmp267;int _tmp268;int*_tmp269;struct Cyc_Absyn_Exp**_tmp26A;int*_tmp26B;_LL19B: _tmp264=_tmp263->break_lab;_tmp265=_tmp263->continue_lab;_tmp266=_tmp263->fallthru_info;_tmp267=_tmp263->varmap;_tmp268=_tmp263->toplevel;_tmp269=_tmp263->in_lhs;_tmp26A=(_tmp263->struct_info).varsizeexp;_tmp26B=_tmp263->in_sizeof;_LL19C: {
struct Cyc_Toc_Env*_tmpA2E;return(_tmpA2E=_region_malloc(r,sizeof(*_tmpA2E)),((_tmpA2E->break_lab=(struct _dyneither_ptr**)_tmp264,((_tmpA2E->continue_lab=(struct _dyneither_ptr**)_tmp265,((_tmpA2E->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp266,((_tmpA2E->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp267),((_tmpA2E->toplevel=(int)_tmp268,((_tmpA2E->in_lhs=(int*)_tmp269,((_tmpA2E->struct_info=(struct Cyc_Toc_StructInfo)(((_tmpA2E->struct_info).varsizeexp=_tmp26A,(((_tmpA2E->struct_info).lhs_exp=exp,_tmpA2E->struct_info)))),((_tmpA2E->in_sizeof=(int*)_tmp26B,((_tmpA2E->rgn=(struct _RegionHandle*)r,_tmpA2E)))))))))))))))))));}_LL19A:;}
# 1289
static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple1*x,struct Cyc_Absyn_Exp*y){
{union Cyc_Absyn_Nmspace _tmp26D=(*x).f1;union Cyc_Absyn_Nmspace _tmp26E=_tmp26D;if((_tmp26E.Rel_n).tag == 1){_LL1A0: _LL1A1: {
# 1292
const char*_tmpA32;void*_tmpA31[1];struct Cyc_String_pa_PrintArg_struct _tmpA30;(_tmpA30.tag=0,((_tmpA30.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(x)),((_tmpA31[0]=& _tmpA30,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA32="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(_tmpA32,sizeof(char),30))),_tag_dyneither(_tmpA31,sizeof(void*),1)))))));}}else{_LL1A2: _LL1A3:
 goto _LL19F;}_LL19F:;}{
# 1295
struct Cyc_Toc_Env*_tmp272=e;struct _dyneither_ptr**_tmp273;struct _dyneither_ptr**_tmp274;struct Cyc_Toc_FallthruInfo*_tmp275;struct Cyc_Dict_Dict _tmp276;int _tmp277;int*_tmp278;struct Cyc_Toc_StructInfo _tmp279;int*_tmp27A;_LL1A5: _tmp273=_tmp272->break_lab;_tmp274=_tmp272->continue_lab;_tmp275=_tmp272->fallthru_info;_tmp276=_tmp272->varmap;_tmp277=_tmp272->toplevel;_tmp278=_tmp272->in_lhs;_tmp279=_tmp272->struct_info;_tmp27A=_tmp272->in_sizeof;_LL1A6: {
struct Cyc_Dict_Dict _tmp27B=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp276),x,y);
struct Cyc_Toc_Env*_tmpA33;return(_tmpA33=_region_malloc(r,sizeof(*_tmpA33)),((_tmpA33->break_lab=(struct _dyneither_ptr**)_tmp273,((_tmpA33->continue_lab=(struct _dyneither_ptr**)_tmp274,((_tmpA33->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp275,((_tmpA33->varmap=(struct Cyc_Dict_Dict)_tmp27B,((_tmpA33->toplevel=(int)_tmp277,((_tmpA33->in_lhs=(int*)_tmp278,((_tmpA33->struct_info=(struct Cyc_Toc_StructInfo)_tmp279,((_tmpA33->in_sizeof=(int*)_tmp27A,((_tmpA33->rgn=(struct _RegionHandle*)r,_tmpA33)))))))))))))))))));}_LL1A4:;};}
# 1302
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp27D=e;struct _dyneither_ptr**_tmp27E;struct _dyneither_ptr**_tmp27F;struct Cyc_Toc_FallthruInfo*_tmp280;struct Cyc_Dict_Dict _tmp281;int _tmp282;int*_tmp283;struct Cyc_Toc_StructInfo _tmp284;int*_tmp285;_LL1AA: _tmp27E=_tmp27D->break_lab;_tmp27F=_tmp27D->continue_lab;_tmp280=_tmp27D->fallthru_info;_tmp281=_tmp27D->varmap;_tmp282=_tmp27D->toplevel;_tmp283=_tmp27D->in_lhs;_tmp284=_tmp27D->struct_info;_tmp285=_tmp27D->in_sizeof;_LL1AB: {
struct Cyc_Toc_Env*_tmpA34;return(_tmpA34=_region_malloc(r,sizeof(*_tmpA34)),((_tmpA34->break_lab=(struct _dyneither_ptr**)0,((_tmpA34->continue_lab=(struct _dyneither_ptr**)0,((_tmpA34->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp280,((_tmpA34->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp281),((_tmpA34->toplevel=(int)_tmp282,((_tmpA34->in_lhs=(int*)_tmp283,((_tmpA34->struct_info=(struct Cyc_Toc_StructInfo)_tmp284,((_tmpA34->in_sizeof=(int*)_tmp285,((_tmpA34->rgn=(struct _RegionHandle*)r,_tmpA34)))))))))))))))))));}_LL1A9:;}
# 1308
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*next_case_env){
# 1313
struct Cyc_List_List*fallthru_vars=0;
for(0;fallthru_binders != 0;fallthru_binders=fallthru_binders->tl){
struct Cyc_List_List*_tmpA35;fallthru_vars=((_tmpA35=_region_malloc(r,sizeof(*_tmpA35)),((_tmpA35->hd=((struct Cyc_Absyn_Vardecl*)fallthru_binders->hd)->name,((_tmpA35->tl=fallthru_vars,_tmpA35))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fallthru_vars);{
struct Cyc_Toc_Env*_tmp288=e;struct _dyneither_ptr**_tmp289;struct _dyneither_ptr**_tmp28A;struct Cyc_Toc_FallthruInfo*_tmp28B;struct Cyc_Dict_Dict _tmp28C;int _tmp28D;int*_tmp28E;struct Cyc_Toc_StructInfo _tmp28F;int*_tmp290;_LL1AF: _tmp289=_tmp288->break_lab;_tmp28A=_tmp288->continue_lab;_tmp28B=_tmp288->fallthru_info;_tmp28C=_tmp288->varmap;_tmp28D=_tmp288->toplevel;_tmp28E=_tmp288->in_lhs;_tmp28F=_tmp288->struct_info;_tmp290=_tmp288->in_sizeof;_LL1B0: {
struct Cyc_Toc_Env*_tmp291=next_case_env;struct Cyc_Dict_Dict _tmp292;_LL1B4: _tmp292=_tmp291->varmap;_LL1B5: {
struct Cyc_Toc_FallthruInfo*_tmpA36;struct Cyc_Toc_FallthruInfo*fi=
(_tmpA36=_region_malloc(r,sizeof(*_tmpA36)),((_tmpA36->label=fallthru_l,((_tmpA36->binders=fallthru_vars,((_tmpA36->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp292),_tmpA36)))))));
struct _dyneither_ptr**_tmpA39;struct Cyc_Toc_Env*_tmpA38;return(_tmpA38=_region_malloc(r,sizeof(*_tmpA38)),((_tmpA38->break_lab=(struct _dyneither_ptr**)((_tmpA39=_region_malloc(r,sizeof(*_tmpA39)),((_tmpA39[0]=break_l,_tmpA39)))),((_tmpA38->continue_lab=(struct _dyneither_ptr**)_tmp28A,((_tmpA38->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi,((_tmpA38->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp28C),((_tmpA38->toplevel=(int)_tmp28D,((_tmpA38->in_lhs=(int*)_tmp28E,((_tmpA38->struct_info=(struct Cyc_Toc_StructInfo)_tmp28F,((_tmpA38->in_sizeof=(int*)_tmp290,((_tmpA38->rgn=(struct _RegionHandle*)r,_tmpA38)))))))))))))))))));}_LL1B3:;}_LL1AE:;};}
# 1325
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1328
struct Cyc_Toc_Env*_tmp296=e;struct _dyneither_ptr**_tmp297;struct _dyneither_ptr**_tmp298;struct Cyc_Toc_FallthruInfo*_tmp299;struct Cyc_Dict_Dict _tmp29A;int _tmp29B;int*_tmp29C;struct Cyc_Toc_StructInfo _tmp29D;int*_tmp29E;_LL1B9: _tmp297=_tmp296->break_lab;_tmp298=_tmp296->continue_lab;_tmp299=_tmp296->fallthru_info;_tmp29A=_tmp296->varmap;_tmp29B=_tmp296->toplevel;_tmp29C=_tmp296->in_lhs;_tmp29D=_tmp296->struct_info;_tmp29E=_tmp296->in_sizeof;_LL1BA: {
struct _dyneither_ptr**_tmpA3C;struct Cyc_Toc_Env*_tmpA3B;return(_tmpA3B=_region_malloc(r,sizeof(*_tmpA3B)),((_tmpA3B->break_lab=(struct _dyneither_ptr**)((_tmpA3C=_region_malloc(r,sizeof(*_tmpA3C)),((_tmpA3C[0]=break_l,_tmpA3C)))),((_tmpA3B->continue_lab=(struct _dyneither_ptr**)_tmp298,((_tmpA3B->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpA3B->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp29A),((_tmpA3B->toplevel=(int)_tmp29B,((_tmpA3B->in_lhs=(int*)_tmp29C,((_tmpA3B->struct_info=(struct Cyc_Toc_StructInfo)_tmp29D,((_tmpA3B->in_sizeof=(int*)_tmp29E,((_tmpA3B->rgn=(struct _RegionHandle*)r,_tmpA3B)))))))))))))))))));}_LL1B8:;}
# 1335
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1338
struct Cyc_Toc_Env*_tmp2A1=e;struct _dyneither_ptr**_tmp2A2;struct _dyneither_ptr**_tmp2A3;struct Cyc_Toc_FallthruInfo*_tmp2A4;struct Cyc_Dict_Dict _tmp2A5;int _tmp2A6;int*_tmp2A7;struct Cyc_Toc_StructInfo _tmp2A8;int*_tmp2A9;_LL1BE: _tmp2A2=_tmp2A1->break_lab;_tmp2A3=_tmp2A1->continue_lab;_tmp2A4=_tmp2A1->fallthru_info;_tmp2A5=_tmp2A1->varmap;_tmp2A6=_tmp2A1->toplevel;_tmp2A7=_tmp2A1->in_lhs;_tmp2A8=_tmp2A1->struct_info;_tmp2A9=_tmp2A1->in_sizeof;_LL1BF: {
struct Cyc_Toc_FallthruInfo*_tmpA3F;struct Cyc_Toc_Env*_tmpA3E;return(_tmpA3E=_region_malloc(r,sizeof(*_tmpA3E)),((_tmpA3E->break_lab=(struct _dyneither_ptr**)0,((_tmpA3E->continue_lab=(struct _dyneither_ptr**)_tmp2A3,((_tmpA3E->fallthru_info=(struct Cyc_Toc_FallthruInfo*)((_tmpA3F=_region_malloc(r,sizeof(*_tmpA3F)),((_tmpA3F->label=next_l,((_tmpA3F->binders=0,((_tmpA3F->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),_tmpA3F)))))))),((_tmpA3E->varmap=(struct Cyc_Dict_Dict)
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2A5),((_tmpA3E->toplevel=(int)_tmp2A6,((_tmpA3E->in_lhs=(int*)_tmp2A7,((_tmpA3E->struct_info=(struct Cyc_Toc_StructInfo)_tmp2A8,((_tmpA3E->in_sizeof=(int*)_tmp2A9,((_tmpA3E->rgn=(struct _RegionHandle*)r,_tmpA3E)))))))))))))))))));}_LL1BD:;}
# 1353 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1356
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
# 1358
void*_tmp2AC=e->annot;void*_tmp2AD=_tmp2AC;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2AD)->tag == Cyc_CfFlowInfo_UnknownZ){_LL1C3: _LL1C4:
# 1361
 return Cyc_Toc_is_nullable((void*)_check_null(e->topt));}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2AD)->tag == Cyc_CfFlowInfo_NotZero){_LL1C5: _LL1C6:
# 1364
 return 0;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2AD)->tag == Cyc_CfFlowInfo_IsZero){_LL1C7: _LL1C8:
# 1366
{const char*_tmpA42;void*_tmpA41;(_tmpA41=0,Cyc_Tcutil_terr(e->loc,((_tmpA42="dereference of NULL pointer",_tag_dyneither(_tmpA42,sizeof(char),28))),_tag_dyneither(_tmpA41,sizeof(void*),0)));}
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2AD)->tag == Cyc_Absyn_EmptyAnnot){_LL1C9: _LL1CA:
# 1372
 return 1;}else{_LL1CB: _LL1CC: {
const char*_tmpA45;void*_tmpA44;(_tmpA44=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA45="need_null_check",_tag_dyneither(_tmpA45,sizeof(char),16))),_tag_dyneither(_tmpA44,sizeof(void*),0)));}}}}}_LL1C2:;}
# 1377
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp2B2=e->annot;void*_tmp2B3=_tmp2B2;struct Cyc_List_List*_tmp2B4;struct Cyc_List_List*_tmp2B5;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2B3)->tag == Cyc_CfFlowInfo_UnknownZ){_LL1CE: _tmp2B5=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2B3)->f1;_LL1CF:
 return _tmp2B5;}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2B3)->tag == Cyc_CfFlowInfo_NotZero){_LL1D0: _tmp2B4=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2B3)->f1;_LL1D1:
 return _tmp2B4;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2B3)->tag == Cyc_CfFlowInfo_IsZero){_LL1D2: _LL1D3:
# 1382
{const char*_tmpA48;void*_tmpA47;(_tmpA47=0,Cyc_Tcutil_terr(e->loc,((_tmpA48="dereference of NULL pointer",_tag_dyneither(_tmpA48,sizeof(char),28))),_tag_dyneither(_tmpA47,sizeof(void*),0)));}
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2B3)->tag == Cyc_Absyn_EmptyAnnot){_LL1D4: _LL1D5:
 return 0;}else{_LL1D6: _LL1D7: {
const char*_tmpA4B;void*_tmpA4A;(_tmpA4A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA4B="get_relns",_tag_dyneither(_tmpA4B,sizeof(char),10))),_tag_dyneither(_tmpA4A,sizeof(void*),0)));}}}}}_LL1CD:;}static char _tmp2BE[8]="*bogus*";
# 1392
static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
# 1402 "toc.cyc"
union Cyc_Relations_RelnOp rop_i=Cyc_Relations_RConst(0);
int valid_rop_i=Cyc_Relations_exp2relnop(i,& rop_i);
# 1405
struct Cyc_Absyn_Vardecl*x;
{void*_tmp2BA=a->r;void*_tmp2BB=_tmp2BA;void*_tmp2BC;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2BB)->tag == 1){_LL1D9: _tmp2BC=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2BB)->f2;_LL1DA: {
# 1408
struct Cyc_Absyn_Vardecl*_tmp2BD=Cyc_Tcutil_nonesc_vardecl(_tmp2BC);
if(_tmp2BD == 0)goto _LL1DC;
x=_tmp2BD;
goto _LL1D8;}}else{_LL1DB: _LL1DC: {
# 1414
static struct _dyneither_ptr bogus_string={_tmp2BE,_tmp2BE,_tmp2BE + 8};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
# 1417
static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};
# 1423
x=& bogus_vardecl;
x->type=a_typ;}}_LL1D8:;}{
# 1426
void*_tmp2BF=a_typ;
union Cyc_Relations_RelnOp rop_a=Cyc_Relations_RNumelts(x);
# 1431
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound(_tmp2BF);
if(bound != 0){
union Cyc_Relations_RelnOp rop_bound=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(bound,& rop_bound))
relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_bound,Cyc_Relations_Rlte,rop_a,relns);}
# 1440
if(valid_rop_i){
struct Cyc_List_List*_tmp2C0=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlte,rop_i,relns);
# 1448
if(!Cyc_Relations_consistent_relations(_tmp2C0))return 1;}
# 1451
inner_loop: {
void*_tmp2C1=i->r;void*_tmp2C2=_tmp2C1;struct Cyc_Absyn_Exp*_tmp2C3;struct Cyc_Absyn_Exp*_tmp2C4;void*_tmp2C5;struct Cyc_Absyn_Exp*_tmp2C6;switch(*((int*)_tmp2C2)){case 13: _LL1DE: _tmp2C5=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2C2)->f1;_tmp2C6=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2C2)->f2;_LL1DF:
 i=_tmp2C6;goto inner_loop;case 2: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2C2)->f1 == Cyc_Absyn_Mod){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2C2)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2C2)->f2)->tl != 0){_LL1E0: _tmp2C3=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2C2)->f2)->hd;_tmp2C4=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2C2)->f2)->tl)->hd;_LL1E1: {
# 1457
union Cyc_Relations_RelnOp rop_z=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmp2C4,& rop_z)){
# 1460
struct Cyc_List_List*_tmp2C7=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlt,rop_z,relns);
# 1462
return !Cyc_Relations_consistent_relations(_tmp2C7);}
# 1464
goto _LL1DD;}}else{goto _LL1E2;}}else{goto _LL1E2;}}else{goto _LL1E2;}default: _LL1E2: _LL1E3:
 goto _LL1DD;}_LL1DD:;}
# 1467
return 0;};}
# 1470
static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0){const char*_tmpA4E;void*_tmpA4D;(_tmpA4D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA4E="Missing type in primop ",_tag_dyneither(_tmpA4E,sizeof(char),24))),_tag_dyneither(_tmpA4D,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));}
# 1474
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0){const char*_tmpA51;void*_tmpA50;(_tmpA50=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA51="Missing type in primop ",_tag_dyneither(_tmpA51,sizeof(char),24))),_tag_dyneither(_tmpA50,sizeof(void*),0)));}
return(void*)_check_null(e->topt);}
# 1478
static struct _tuple11*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
struct _tuple11*_tmpA52;return(_tmpA52=_cycalloc(sizeof(*_tmpA52)),((_tmpA52->f1=Cyc_Toc_mt_tq,((_tmpA52->f2=Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmpA52)))));}
# 1481
static struct _tuple19*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple19*_tmpA53;return(_tmpA53=_cycalloc(sizeof(*_tmpA53)),((_tmpA53->f1=0,((_tmpA53->f2=e,_tmpA53)))));};}
# 1488
static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic,int do_declare){
# 1491
struct Cyc_Absyn_Exp*eo;
void*t;
if(pointer){
struct Cyc_Absyn_Exp*_tmp2CE=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(x,0),0);
if(*(nv->struct_info).varsizeexp != 0){
_tmp2CE=Cyc_Absyn_add_exp(_tmp2CE,(struct Cyc_Absyn_Exp*)_check_null(*(nv->struct_info).varsizeexp),0);
*(nv->struct_info).varsizeexp=0;}
# 1499
t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);
if(rgnopt == 0  || Cyc_Absyn_no_regions)
eo=is_atomic?Cyc_Toc_malloc_atomic(_tmp2CE): Cyc_Toc_malloc_ptr(_tmp2CE);else{
# 1503
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);
eo=Cyc_Toc_rmalloc_exp(r,_tmp2CE);}}else{
# 1508
t=struct_typ;
eo=0;}
# 1511
if(do_declare){
void*_tmp2CF=x->r;void*_tmp2D0=_tmp2CF;struct _tuple1*_tmp2D1;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D0)->tag == 1){_LL1E5: _tmp2D1=((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D0)->f1;_LL1E6:
# 1514
 return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp2D1,t,eo,s,0),0);}else{_LL1E7: _LL1E8: {
# 1516
const char*_tmpA56;void*_tmpA55;(_tmpA55=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA56="bogus x to make_struct",_tag_dyneither(_tmpA56,sizeof(char),23))),_tag_dyneither(_tmpA55,sizeof(void*),0)));}}_LL1E4:;}else{
# 1520
if(eo != 0)
return Cyc_Absyn_stmt_exp(Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(x,eo,0),0),s,0),0);else{
# 1523
return Cyc_Absyn_stmt_exp(s,0);}}}
# 1527
static int Cyc_Toc_abstract_aggr(void*typ){
void*_tmp2D4=Cyc_Tcutil_compress(typ);void*_tmp2D5=_tmp2D4;struct Cyc_List_List*_tmp2D6;union Cyc_Absyn_AggrInfoU _tmp2D7;struct Cyc_List_List*_tmp2D8;switch(*((int*)_tmp2D5)){case 11: _LL1EA: _tmp2D7=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2D5)->f1).aggr_info;_tmp2D8=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2D5)->f1).targs;_LL1EB: {
# 1530
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(_tmp2D7);
if(ad->impl == 0)return 0;{
struct Cyc_List_List*aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
_tmp2D6=aggrfields;goto _LL1ED;};}case 12: _LL1EC: _tmp2D6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D5)->f2;_LL1ED:
# 1535
 if(_tmp2D6 == 0)return 0;
return 1;default: _LL1EE: _LL1EF:
# 1541
 return 0;}_LL1E9:;}
# 1545
static void*Cyc_Toc_array_elt_type(void*t){
void*_tmp2D9=Cyc_Tcutil_compress(t);void*_tmp2DA=_tmp2D9;void*_tmp2DB;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2DA)->tag == 8){_LL1F1: _tmp2DB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2DA)->f1).elt_type;_LL1F2:
 return _tmp2DB;}else{_LL1F3: _LL1F4: {
const char*_tmpA5A;void*_tmpA59[1];struct Cyc_String_pa_PrintArg_struct _tmpA58;(_tmpA58.tag=0,((_tmpA58.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA59[0]=& _tmpA58,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA5A="array_elt_type called on %s",_tag_dyneither(_tmpA5A,sizeof(char),28))),_tag_dyneither(_tmpA59,sizeof(void*),1)))))));}}_LL1F0:;}
# 1552
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated);
# 1558
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s);
# 1563
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,void*array_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){
# 1568
void*orig_elt_type=Cyc_Toc_array_elt_type(array_type);
int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;
{struct Cyc_List_List*_tmp2DF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;_tmp2DF != 0;_tmp2DF=_tmp2DF->tl){
# 1572
struct _tuple19*_tmp2E0=(struct _tuple19*)_tmp2DF->hd;struct _tuple19*_tmp2E1=_tmp2E0;struct Cyc_List_List*_tmp2E2;struct Cyc_Absyn_Exp*_tmp2E3;_LL1F6: _tmp2E2=_tmp2E1->f1;_tmp2E3=_tmp2E1->f2;_LL1F7: {
# 1576
struct Cyc_Absyn_Exp*e_index;
if(_tmp2E2 == 0)
e_index=Cyc_Absyn_signed_int_exp(count --,0);else{
# 1580
if(_tmp2E2->tl != 0){const char*_tmpA5D;void*_tmpA5C;(_tmpA5C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpA5D="multiple designators in array",_tag_dyneither(_tmpA5D,sizeof(char),30))),_tag_dyneither(_tmpA5C,sizeof(void*),0)));}{
void*_tmp2E6=(void*)_tmp2E2->hd;
void*_tmp2E7=_tmp2E6;struct Cyc_Absyn_Exp*_tmp2E8;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2E7)->tag == 0){_LL1FB: _tmp2E8=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2E7)->f1;_LL1FC:
# 1584
 Cyc_Toc_exp_to_c(nv,_tmp2E8);
e_index=_tmp2E8;
goto _LL1FA;}else{_LL1FD: _LL1FE: {
const char*_tmpA60;void*_tmpA5F;(_tmpA5F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpA60="field name designators in array",_tag_dyneither(_tmpA60,sizeof(char),32))),_tag_dyneither(_tmpA5F,sizeof(void*),0)));}}_LL1FA:;};}{
# 1590
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);
void*_tmp2EB=_tmp2E3->r;void*_tmp2EC=_tmp2EB;void*_tmp2ED;struct Cyc_List_List*_tmp2EE;struct Cyc_Absyn_Vardecl*_tmp2EF;struct Cyc_Absyn_Exp*_tmp2F0;struct Cyc_Absyn_Exp*_tmp2F1;int _tmp2F2;struct Cyc_List_List*_tmp2F3;switch(*((int*)_tmp2EC)){case 25: _LL200: _tmp2F3=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp2EC)->f1;_LL201:
# 1593
 s=Cyc_Toc_init_array(nv,orig_elt_type,lval,_tmp2F3,s);
goto _LL1FF;case 26: _LL202: _tmp2EF=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2EC)->f1;_tmp2F0=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2EC)->f2;_tmp2F1=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2EC)->f3;_tmp2F2=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2EC)->f4;_LL203:
# 1596
 s=Cyc_Toc_init_comprehension(nv,orig_elt_type,lval,_tmp2EF,_tmp2F0,_tmp2F1,_tmp2F2,s,0);
# 1598
goto _LL1FF;case 29: _LL204: _tmp2ED=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2EC)->f1;_tmp2EE=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2EC)->f2;_LL205:
# 1600
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp2ED,_tmp2EE,s);
goto _LL1FF;case 27: _LL206: _LL207:
# 1603
 goto _LL1FF;default: _LL208: _LL209:
# 1605
 Cyc_Toc_exp_to_c(nv,_tmp2E3);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(orig_elt_type),_tmp2E3),0),s,0);
goto _LL1FF;}_LL1FF:;};}_LL1F5:;}}
# 1611
return s;}
# 1616
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated){
# 1621
struct _tuple1*_tmp2F4=vd->name;
void*expected_elt_type;
{void*_tmp2F5=Cyc_Tcutil_compress(comprehension_type);void*_tmp2F6=_tmp2F5;void*_tmp2F7;void*_tmp2F8;switch(*((int*)_tmp2F6)){case 8: _LL20B: _tmp2F8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F6)->f1).elt_type;_LL20C:
# 1625
 expected_elt_type=_tmp2F8;goto _LL20A;case 5: _LL20D: _tmp2F7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F6)->f1).elt_typ;_LL20E:
# 1627
 expected_elt_type=_tmp2F7;goto _LL20A;default: _LL20F: _LL210: {
const char*_tmpA63;void*_tmpA62;(_tmpA62=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA63="init_comprehension passed a bad type!",_tag_dyneither(_tmpA63,sizeof(char),38))),_tag_dyneither(_tmpA62,sizeof(void*),0)));}}_LL20A:;}{
# 1630
void*_tmp2FB=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));
if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);
# 1634
{void*_tmp2FC=e2->r;void*_tmp2FD=_tmp2FC;if(((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2FD)->tag == 27){_LL212: _LL213:
 return Cyc_Absyn_skip_stmt(0);}else{_LL214: _LL215:
 goto _LL211;}_LL211:;}{
# 1638
struct _RegionHandle _tmp2FE=_new_region("r2");struct _RegionHandle*r2=& _tmp2FE;_push_region(r2);
{struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpA66;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpA65;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(r2,nv,_tmp2F4,Cyc_Absyn_varb_exp(_tmp2F4,(void*)((_tmpA65=_cycalloc(sizeof(*_tmpA65)),((_tmpA65[0]=((_tmpA66.tag=4,((_tmpA66.f1=vd,_tmpA66)))),_tmpA65)))),0));
struct _tuple1*max=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp2F4,0),Cyc_Absyn_signed_int_exp(0,0),0);
struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp2F4,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp2F4,0),0);
# 1646
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp2F4,0),0);
struct Cyc_Absyn_Stmt*body;
# 1650
{void*_tmp2FF=e2->r;void*_tmp300=_tmp2FF;void*_tmp301;struct Cyc_List_List*_tmp302;struct Cyc_Absyn_Vardecl*_tmp303;struct Cyc_Absyn_Exp*_tmp304;struct Cyc_Absyn_Exp*_tmp305;int _tmp306;struct Cyc_List_List*_tmp307;switch(*((int*)_tmp300)){case 25: _LL217: _tmp307=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_LL218:
# 1652
 body=Cyc_Toc_init_array(nv2,expected_elt_type,lval,_tmp307,Cyc_Toc_skip_stmt_dl());
goto _LL216;case 26: _LL219: _tmp303=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_tmp304=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp300)->f2;_tmp305=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp300)->f3;_tmp306=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp300)->f4;_LL21A:
# 1655
 body=Cyc_Toc_init_comprehension(nv2,expected_elt_type,lval,_tmp303,_tmp304,_tmp305,_tmp306,Cyc_Toc_skip_stmt_dl(),0);
goto _LL216;case 29: _LL21B: _tmp301=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_tmp302=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp300)->f2;_LL21C:
# 1658
 body=Cyc_Toc_init_anon_struct(nv,lval,_tmp301,_tmp302,Cyc_Toc_skip_stmt_dl());
goto _LL216;default: _LL21D: _LL21E:
# 1661
 Cyc_Toc_exp_to_c(nv2,e2);
body=Cyc_Absyn_assign_stmt(lval,Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(expected_elt_type),e2),0);
goto _LL216;}_LL216:;}{
# 1665
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0);
# 1667
if(zero_term){
# 1672
struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(_tmp2FB,Cyc_Absyn_uint_exp(0,0)),0);
s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}{
# 1677
struct Cyc_Absyn_Stmt*_tmp308=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,e1,
Cyc_Absyn_declare_stmt(_tmp2F4,Cyc_Absyn_uint_typ,0,s2,0),0),s,0);_npop_handler(0);return _tmp308;};};}
# 1639
;_pop_region(r2);};};}
# 1683
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s){
# 1686
{struct Cyc_List_List*_tmp30B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles);for(0;_tmp30B != 0;_tmp30B=_tmp30B->tl){
struct _tuple19*_tmp30C=(struct _tuple19*)_tmp30B->hd;struct _tuple19*_tmp30D=_tmp30C;struct Cyc_List_List*_tmp30E;struct Cyc_Absyn_Exp*_tmp30F;_LL220: _tmp30E=_tmp30D->f1;_tmp30F=_tmp30D->f2;_LL221:
 if(_tmp30E == 0){
const char*_tmpA69;void*_tmpA68;(_tmpA68=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA69="empty designator list",_tag_dyneither(_tmpA69,sizeof(char),22))),_tag_dyneither(_tmpA68,sizeof(void*),0)));}
if(_tmp30E->tl != 0){
const char*_tmpA6C;void*_tmpA6B;(_tmpA6B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA6C="too many designators in anonymous struct",_tag_dyneither(_tmpA6C,sizeof(char),41))),_tag_dyneither(_tmpA6B,sizeof(void*),0)));}{
void*_tmp314=(void*)_tmp30E->hd;void*_tmp315=_tmp314;struct _dyneither_ptr*_tmp316;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp315)->tag == 1){_LL225: _tmp316=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp315)->f1;_LL226: {
# 1694
struct Cyc_Absyn_Exp*lval=Cyc_Toc_member_exp(lhs,_tmp316,0);
{void*_tmp317=_tmp30F->r;void*_tmp318=_tmp317;void*_tmp319;struct Cyc_List_List*_tmp31A;struct Cyc_Absyn_Vardecl*_tmp31B;struct Cyc_Absyn_Exp*_tmp31C;struct Cyc_Absyn_Exp*_tmp31D;int _tmp31E;struct Cyc_List_List*_tmp31F;switch(*((int*)_tmp318)){case 25: _LL22A: _tmp31F=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp318)->f1;_LL22B:
# 1697
 s=Cyc_Toc_init_array(nv,(void*)_check_null(_tmp30F->topt),lval,_tmp31F,s);goto _LL229;case 26: _LL22C: _tmp31B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp318)->f1;_tmp31C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp318)->f2;_tmp31D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp318)->f3;_tmp31E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp318)->f4;_LL22D:
# 1699
 s=Cyc_Toc_init_comprehension(nv,(void*)_check_null(_tmp30F->topt),lval,_tmp31B,_tmp31C,_tmp31D,_tmp31E,s,0);
goto _LL229;case 29: _LL22E: _tmp319=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp318)->f1;_tmp31A=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp318)->f2;_LL22F:
# 1702
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp319,_tmp31A,s);goto _LL229;case 27: _LL230: _LL231:
# 1704
 goto _LL229;default: _LL232: _LL233:
# 1706
 Cyc_Toc_exp_to_c(nv,_tmp30F);
# 1708
if(Cyc_Toc_is_poly_field(struct_type,_tmp316))
_tmp30F=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp30F);
# 1711
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp30F,0),0),s,0);
goto _LL229;}_LL229:;}
# 1714
goto _LL224;}}else{_LL227: _LL228: {
const char*_tmpA6F;void*_tmpA6E;(_tmpA6E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA6F="array designator in struct",_tag_dyneither(_tmpA6F,sizeof(char),27))),_tag_dyneither(_tmpA6E,sizeof(void*),0)));}}_LL224:;};_LL21F:;}}
# 1718
return s;}
# 1723
static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){
# 1727
struct _RegionHandle _tmp322=_new_region("r");struct _RegionHandle*r=& _tmp322;_push_region(r);
{struct Cyc_List_List*_tmp323=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);
void*_tmp324=Cyc_Toc_add_tuple_type(_tmp323);
# 1731
struct _tuple1*_tmp325=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp326=Cyc_Absyn_var_exp(_tmp325,0);
struct Cyc_Absyn_Stmt*_tmp327=Cyc_Absyn_exp_stmt(_tmp326,0);
# 1735
struct Cyc_Absyn_Exp*(*_tmp328)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
# 1737
int is_atomic=1;
struct Cyc_List_List*_tmp329=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);
{int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp329);for(0;_tmp329 != 0;(_tmp329=_tmp329->tl,-- i)){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp329->hd;
struct Cyc_Absyn_Exp*lval=_tmp328(_tmp326,Cyc_Absyn_fieldname(i),0);
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(e->topt));{
void*_tmp32A=e->r;void*_tmp32B=_tmp32A;struct Cyc_Absyn_Vardecl*_tmp32C;struct Cyc_Absyn_Exp*_tmp32D;struct Cyc_Absyn_Exp*_tmp32E;int _tmp32F;struct Cyc_List_List*_tmp330;switch(*((int*)_tmp32B)){case 25: _LL235: _tmp330=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp32B)->f1;_LL236:
# 1745
 _tmp327=Cyc_Toc_init_array(nv,(void*)_check_null(e->topt),lval,_tmp330,_tmp327);
goto _LL234;case 26: _LL237: _tmp32C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp32B)->f1;_tmp32D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp32B)->f2;_tmp32E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp32B)->f3;_tmp32F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp32B)->f4;_LL238:
# 1748
 _tmp327=Cyc_Toc_init_comprehension(nv,(void*)_check_null(e->topt),lval,_tmp32C,_tmp32D,_tmp32E,_tmp32F,_tmp327,0);
# 1750
goto _LL234;case 27: _LL239: _LL23A:
# 1752
 goto _LL234;default: _LL23B: _LL23C:
# 1755
 Cyc_Toc_exp_to_c(nv,e);
_tmp327=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp328(_tmp326,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp327,0);
# 1758
goto _LL234;}_LL234:;};}}{
# 1761
struct Cyc_Absyn_Exp*_tmp331=Cyc_Toc_make_struct(nv,Cyc_Absyn_var_exp(_tmp325,0),_tmp324,_tmp327,pointer,rgnopt,is_atomic,1);_npop_handler(0);return _tmp331;};}
# 1728
;_pop_region(r);}
# 1765
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp332=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp332 != 0;_tmp332=_tmp332->tl){
struct Cyc_Absyn_Aggrfield*_tmp333=(struct Cyc_Absyn_Aggrfield*)_tmp332->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp333->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}{
# 1772
struct Cyc_String_pa_PrintArg_struct _tmpA77;void*_tmpA76[1];const char*_tmpA75;void*_tmpA74;(_tmpA74=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((struct _dyneither_ptr)((_tmpA77.tag=0,((_tmpA77.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpA76[0]=& _tmpA77,Cyc_aprintf(((_tmpA75="get_member_offset %s failed",_tag_dyneither(_tmpA75,sizeof(char),28))),_tag_dyneither(_tmpA76,sizeof(void*),1)))))))),_tag_dyneither(_tmpA74,sizeof(void*),0)));};}
# 1775
static int Cyc_Toc_init_variable_array(void**typ){
{void*_tmp338=Cyc_Tcutil_compress(*typ);void*_tmp339=_tmp338;void*_tmp33A;struct Cyc_Absyn_Tqual _tmp33B;struct Cyc_Absyn_Exp*_tmp33C;union Cyc_Absyn_Constraint*_tmp33D;unsigned int _tmp33E;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp339)->tag == 8){_LL23E: _tmp33A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp339)->f1).elt_type;_tmp33B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp339)->f1).tq;_tmp33C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp339)->f1).num_elts;_tmp33D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp339)->f1).zero_term;_tmp33E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp339)->f1).zt_loc;_LL23F:
# 1780
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp33C))){
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpA7D;struct Cyc_Absyn_ArrayInfo _tmpA7C;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpA7B;*typ=(void*)((_tmpA7B=_cycalloc(sizeof(*_tmpA7B)),((_tmpA7B[0]=((_tmpA7D.tag=8,((_tmpA7D.f1=((_tmpA7C.elt_type=_tmp33A,((_tmpA7C.tq=_tmp33B,((_tmpA7C.num_elts=Cyc_Absyn_uint_exp(0,0),((_tmpA7C.zero_term=_tmp33D,((_tmpA7C.zt_loc=_tmp33E,_tmpA7C)))))))))),_tmpA7D)))),_tmpA7B))));}
return 1;}
# 1784
goto _LL23D;}else{_LL240: _LL241:
 goto _LL23D;}_LL23D:;}
# 1787
return 0;}
# 1790
static int Cyc_Toc_is_array_type(void*t){
void*_tmp342=Cyc_Tcutil_compress(t);void*_tmp343=_tmp342;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp343)->tag == 8){_LL243: _LL244:
 return 1;}else{_LL245: _LL246:
 return 0;}_LL242:;}
# 1797
struct Cyc_Absyn_Aggrdecl*Cyc_Toc_update_aggr_type(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*fn,void*new_field_type){
# 1799
struct Cyc_List_List*new_fields=0;
struct Cyc_List_List*fields;
for(fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;fields != 0;fields=fields->tl){
# 1803
if(!Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,fn)){
struct Cyc_Absyn_Aggrfield*_tmp344=(struct Cyc_Absyn_Aggrfield*)fields->hd;
struct Cyc_List_List*_tmp345=_tmp344->attributes;
void*_tmp346=_tmp344->type;
if((!Cyc_Toc_is_array_type(_tmp346),Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp346)))){
struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpA83;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpA82;struct Cyc_List_List*_tmpA81;_tmp345=((_tmpA81=_cycalloc(sizeof(*_tmpA81)),((_tmpA81->hd=(void*)((_tmpA83=_cycalloc(sizeof(*_tmpA83)),((_tmpA83[0]=((_tmpA82.tag=6,((_tmpA82.f1=0,_tmpA82)))),_tmpA83)))),((_tmpA81->tl=_tmp345,_tmpA81))))));}{
struct Cyc_Absyn_Aggrfield*_tmpA84;struct Cyc_Absyn_Aggrfield*_tmp34A=(_tmpA84=_cycalloc(sizeof(*_tmpA84)),((_tmpA84->name=_tmp344->name,((_tmpA84->tq=Cyc_Toc_mt_tq,((_tmpA84->type=new_field_type,((_tmpA84->width=_tmp344->width,((_tmpA84->attributes=_tmp345,((_tmpA84->requires_clause=0,_tmpA84)))))))))))));
# 1815
struct Cyc_List_List*_tmpA85;new_fields=((_tmpA85=_cycalloc(sizeof(*_tmpA85)),((_tmpA85->hd=_tmp34A,((_tmpA85->tl=new_fields,_tmpA85))))));};}else{
# 1817
struct Cyc_List_List*_tmpA86;new_fields=((_tmpA86=_cycalloc(sizeof(*_tmpA86)),((_tmpA86->hd=(struct Cyc_Absyn_Aggrfield*)fields->hd,((_tmpA86->tl=new_fields,_tmpA86))))));}}{
# 1819
struct Cyc_List_List*_tmp34E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);
struct Cyc_Int_pa_PrintArg_struct _tmpA8E;void*_tmpA8D[1];const char*_tmpA8C;struct _dyneither_ptr*_tmpA8B;struct _dyneither_ptr*name=
(_tmpA8B=_cycalloc(sizeof(*_tmpA8B)),((_tmpA8B[0]=(struct _dyneither_ptr)((_tmpA8E.tag=1,((_tmpA8E.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpA8D[0]=& _tmpA8E,Cyc_aprintf(((_tmpA8C="_genStruct%d",_tag_dyneither(_tmpA8C,sizeof(char),13))),_tag_dyneither(_tmpA8D,sizeof(void*),1)))))))),_tmpA8B)));
struct _tuple1*_tmpA8F;struct _tuple1*qv=(_tmpA8F=_cycalloc(sizeof(*_tmpA8F)),((_tmpA8F->f1=Cyc_Absyn_Abs_n(0,1),((_tmpA8F->f2=name,_tmpA8F)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpA90;struct Cyc_Absyn_AggrdeclImpl*_tmp34F=(_tmpA90=_cycalloc(sizeof(*_tmpA90)),((_tmpA90->exist_vars=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,((_tmpA90->rgn_po=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po,((_tmpA90->fields=_tmp34E,((_tmpA90->tagged=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged,_tmpA90)))))))));
# 1827
struct Cyc_Absyn_Aggrdecl*_tmpA91;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpA91=_cycalloc(sizeof(*_tmpA91)),((_tmpA91->kind=ad->kind,((_tmpA91->sc=ad->sc,((_tmpA91->name=qv,((_tmpA91->tvs=ad->tvs,((_tmpA91->impl=_tmp34F,((_tmpA91->expected_mem_kind=0,((_tmpA91->attributes=ad->attributes,_tmpA91)))))))))))))));
# 1834
Cyc_Toc_aggrdecl_to_c(new_ad,1);
return new_ad;};}
# 1838
static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*nv,void*struct_type,struct Cyc_List_List*exist_types,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*dles,struct _tuple1*tdn){
# 1844
int do_declare=0;
struct Cyc_Absyn_Exp*xexp;
if((nv->struct_info).lhs_exp == 0){
struct _tuple1*_tmp357=Cyc_Toc_temp_var();
xexp=Cyc_Absyn_var_exp(_tmp357,0);
do_declare=1;}else{
# 1852
xexp=(struct Cyc_Absyn_Exp*)_check_null((nv->struct_info).lhs_exp);
nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);}{
# 1855
struct Cyc_Absyn_Stmt*_tmp358=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(xexp),0);
# 1859
struct Cyc_Absyn_Exp**varexp;
{void*_tmp359=_tmp358->r;void*_tmp35A=_tmp359;struct Cyc_Absyn_Exp**_tmp35B;if(((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp35A)->tag == 1){_LL248: _tmp35B=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp35A)->f1;_LL249:
 varexp=_tmp35B;goto _LL247;}else{_LL24A: _LL24B: {
const char*_tmpA94;void*_tmpA93;(_tmpA93=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA94="stmt not an expression!",_tag_dyneither(_tmpA94,sizeof(char),24))),_tag_dyneither(_tmpA93,sizeof(void*),0)));}}_LL247:;}{
# 1865
struct Cyc_Absyn_Exp*(*_tmp35E)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
void*aggr_typ=Cyc_Toc_typ_to_c(struct_type);
void*orig_typ=aggr_typ;
# 1869
int is_atomic=1;
struct Cyc_List_List*forall_types;
struct Cyc_Absyn_Aggrdecl*ad;
struct Cyc_List_List*aggrfields;struct Cyc_List_List*orig_aggrfields;
struct _dyneither_ptr**index=0;
{void*_tmp35F=Cyc_Tcutil_compress(struct_type);void*_tmp360=_tmp35F;union Cyc_Absyn_AggrInfoU _tmp361;struct Cyc_List_List*_tmp362;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp360)->tag == 11){_LL24D: _tmp361=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp360)->f1).aggr_info;_tmp362=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp360)->f1).targs;_LL24E:
# 1876
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp361);
orig_aggrfields=(aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
forall_types=_tmp362;
goto _LL24C;}else{_LL24F: _LL250: {
const char*_tmpA97;void*_tmpA96;(_tmpA96=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA97="init_struct: bad struct type",_tag_dyneither(_tmpA97,sizeof(char),29))),_tag_dyneither(_tmpA96,sizeof(void*),0)));}}_LL24C:;}
# 1883
if(exist_types != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields != 0){
struct Cyc_List_List*_tmp365=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
for(0;_tmp365->tl != 0;_tmp365=_tmp365->tl){;}{
struct Cyc_Absyn_Aggrfield*_tmp366=(struct Cyc_Absyn_Aggrfield*)_tmp365->hd;
struct _RegionHandle _tmp367=_new_region("temp");struct _RegionHandle*temp=& _tmp367;_push_region(temp);
{struct Cyc_List_List*_tmp368=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,forall_types);
# 1892
if(Cyc_Toc_is_abstract_type(Cyc_Tcutil_rsubstitute(temp,_tmp368,_tmp366->type))){
struct Cyc_List_List*_tmp369=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,exist_types);
struct Cyc_List_List*_tmp36A=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(temp,_tmp368,_tmp369);
# 1896
struct Cyc_List_List*new_fields=0;
for(_tmp365=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;_tmp365 != 0;_tmp365=_tmp365->tl){
struct Cyc_Absyn_Aggrfield*_tmp36B=(struct Cyc_Absyn_Aggrfield*)_tmp365->hd;
struct Cyc_Absyn_Aggrfield*_tmpA98;struct Cyc_Absyn_Aggrfield*_tmp36C=(_tmpA98=_cycalloc(sizeof(*_tmpA98)),((_tmpA98->name=_tmp36B->name,((_tmpA98->tq=Cyc_Toc_mt_tq,((_tmpA98->type=
# 1901
Cyc_Tcutil_rsubstitute(temp,_tmp36A,_tmp36B->type),((_tmpA98->width=_tmp36B->width,((_tmpA98->attributes=_tmp36B->attributes,((_tmpA98->requires_clause=0,_tmpA98)))))))))))));
# 1909
if(_tmp365->tl == 0){
if(Cyc_Toc_init_variable_array(& _tmp36C->type)){
struct _dyneither_ptr**_tmpA99;index=((_tmpA99=_cycalloc(sizeof(*_tmpA99)),((_tmpA99[0]=_tmp36C->name,_tmpA99))));}}{
# 1916
struct Cyc_List_List*_tmpA9A;new_fields=((_tmpA9A=_cycalloc(sizeof(*_tmpA9A)),((_tmpA9A->hd=_tmp36C,((_tmpA9A->tl=new_fields,_tmpA9A))))));};}
# 1918
exist_types=0;
aggrfields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);{
struct Cyc_Int_pa_PrintArg_struct _tmpAA2;void*_tmpAA1[1];const char*_tmpAA0;struct _dyneither_ptr*_tmpA9F;struct _dyneither_ptr*name=
(_tmpA9F=_cycalloc(sizeof(*_tmpA9F)),((_tmpA9F[0]=(struct _dyneither_ptr)((_tmpAA2.tag=1,((_tmpAA2.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpAA1[0]=& _tmpAA2,Cyc_aprintf(((_tmpAA0="_genStruct%d",_tag_dyneither(_tmpAA0,sizeof(char),13))),_tag_dyneither(_tmpAA1,sizeof(void*),1)))))))),_tmpA9F)));
struct _tuple1*_tmpAA3;struct _tuple1*qv=(_tmpAA3=_cycalloc(sizeof(*_tmpAA3)),((_tmpAA3->f1=Cyc_Absyn_Abs_n(0,1),((_tmpAA3->f2=name,_tmpAA3)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpAA4;struct Cyc_Absyn_AggrdeclImpl*_tmp370=(_tmpAA4=_cycalloc(sizeof(*_tmpAA4)),((_tmpAA4->exist_vars=0,((_tmpAA4->rgn_po=0,((_tmpAA4->fields=aggrfields,((_tmpAA4->tagged=0,_tmpAA4)))))))));
# 1927
struct Cyc_Absyn_Aggrdecl*_tmpAA5;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpAA5=_cycalloc(sizeof(*_tmpAA5)),((_tmpAA5->kind=Cyc_Absyn_StructA,((_tmpAA5->sc=Cyc_Absyn_Public,((_tmpAA5->name=qv,((_tmpAA5->tvs=0,((_tmpAA5->expected_mem_kind=0,((_tmpAA5->impl=_tmp370,((_tmpAA5->attributes=0,_tmpAA5)))))))))))))));
# 1934
Cyc_Toc_aggrdecl_to_c(new_ad,1);
ad=new_ad;
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpAAF;struct Cyc_Absyn_Aggrdecl**_tmpAAE;struct Cyc_Absyn_AggrInfo _tmpAAD;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpAAC;struct_type=(void*)((_tmpAAC=_cycalloc(sizeof(*_tmpAAC)),((_tmpAAC[0]=((_tmpAAF.tag=11,((_tmpAAF.f1=((_tmpAAD.aggr_info=Cyc_Absyn_KnownAggr(((_tmpAAE=_cycalloc(sizeof(*_tmpAAE)),((_tmpAAE[0]=new_ad,_tmpAAE))))),((_tmpAAD.targs=0,_tmpAAD)))),_tmpAAF)))),_tmpAAC))));}
# 1938
aggr_typ=Cyc_Toc_typ_to_c(struct_type);};}}
# 1888
;_pop_region(temp);};}{
# 1942
int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct _RegionHandle _tmp37C=_new_region("r");struct _RegionHandle*r=& _tmp37C;_push_region(r);
{struct Cyc_List_List*_tmp37D=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp37D != 0;_tmp37D=_tmp37D->tl){
struct _tuple19*_tmp37E=(struct _tuple19*)_tmp37D->hd;struct _tuple19*_tmp37F=_tmp37E;struct Cyc_List_List*_tmp380;struct Cyc_Absyn_Exp*_tmp381;_LL252: _tmp380=_tmp37F->f1;_tmp381=_tmp37F->f2;_LL253:
 is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(_tmp381->topt));
if(_tmp380 == 0){
const char*_tmpAB2;void*_tmpAB1;(_tmpAB1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAB2="empty designator list",_tag_dyneither(_tmpAB2,sizeof(char),22))),_tag_dyneither(_tmpAB1,sizeof(void*),0)));}
if(_tmp380->tl != 0){
# 1952
struct _tuple1*_tmp384=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp385=Cyc_Absyn_var_exp(_tmp384,0);
for(0;_tmp380 != 0;_tmp380=_tmp380->tl){
void*_tmp386=(void*)_tmp380->hd;void*_tmp387=_tmp386;struct _dyneither_ptr*_tmp388;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp387)->tag == 1){_LL257: _tmp388=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp387)->f1;_LL258:
# 1958
 if(Cyc_Toc_is_poly_field(struct_type,_tmp388))
_tmp385=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp385);
_tmp358=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp35E(xexp,_tmp388,0),_tmp385,0),0),_tmp358,0);
# 1962
goto _LL256;}else{_LL259: _LL25A: {
const char*_tmpAB5;void*_tmpAB4;(_tmpAB4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAB5="array designator in struct",_tag_dyneither(_tmpAB5,sizeof(char),27))),_tag_dyneither(_tmpAB4,sizeof(void*),0)));}}_LL256:;}
# 1966
Cyc_Toc_exp_to_c(nv,_tmp381);
_tmp358=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp385,_tmp381,0),0),_tmp358,0);}else{
# 1969
void*_tmp38B=(void*)_tmp380->hd;void*_tmp38C=_tmp38B;struct _dyneither_ptr*_tmp38D;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp38C)->tag == 1){_LL25C: _tmp38D=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp38C)->f1;_LL25D: {
# 1971
struct Cyc_Absyn_Exp*lval=_tmp35E(xexp,_tmp38D,0);
if(is_tagged_union){
int i=Cyc_Toc_get_member_offset(ad,_tmp38D);
struct Cyc_Absyn_Exp*f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct Cyc_Absyn_Exp*lhs=Cyc_Toc_member_exp(lval,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,f_tag_exp,0);
_tmp358=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp358,0);
lval=Cyc_Toc_member_exp(lval,Cyc_Toc_val_sp,0);}{
# 1980
int e1_translated=0;
{void*_tmp38E=_tmp381->r;void*_tmp38F=_tmp38E;void*_tmp390;struct Cyc_List_List*_tmp391;struct Cyc_Absyn_Exp*_tmp392;void*_tmp393;struct Cyc_Absyn_Vardecl*_tmp394;struct Cyc_Absyn_Exp*_tmp395;struct Cyc_Absyn_Exp*_tmp396;int _tmp397;struct Cyc_List_List*_tmp398;switch(*((int*)_tmp38F)){case 25: _LL261: _tmp398=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp38F)->f1;_LL262:
# 1983
 _tmp358=Cyc_Toc_init_array(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(aggrfields,_tmp38D)))->type,lval,_tmp398,_tmp358);
# 1985
goto _LL260;case 26: _LL263: _tmp394=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp38F)->f1;_tmp395=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp38F)->f2;_tmp396=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp38F)->f3;_tmp397=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp38F)->f4;_LL264:
# 1987
 _tmp358=Cyc_Toc_init_comprehension(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(do_declare?aggrfields: orig_aggrfields,_tmp38D)))->type,lval,_tmp394,_tmp395,_tmp396,_tmp397,_tmp358,0);
# 1991
e1_translated=1;
_tmp392=_tmp395;_tmp393=(void*)_check_null(_tmp396->topt);goto _LL266;case 27: _LL265: _tmp392=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp38F)->f1;_tmp393=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp38F)->f2;_LL266:
# 1996
 if(index != 0  && !Cyc_strptrcmp(*index,_tmp38D)){
if(!e1_translated)Cyc_Toc_exp_to_c(nv,_tmp392);{
void*_tmp399=Cyc_Toc_typ_to_c(_tmp393);
struct _tuple6 _tmpAB9;union Cyc_Absyn_Cnst _tmpAB8;struct Cyc_Absyn_Exp*_tmpAB6[2];*(nv->struct_info).varsizeexp=
Cyc_Absyn_add_exp(Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpAB6[1]=Cyc_Absyn_sizeoftyp_exp(_tmp399,0),((_tmpAB6[0]=_tmp392,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAB6,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),
Cyc_Absyn_const_exp((((_tmpAB8.Int_c).val=((_tmpAB9.f1=Cyc_Absyn_Unsigned,((_tmpAB9.f2=(int)sizeof(double),_tmpAB9)))),(((_tmpAB8.Int_c).tag=5,_tmpAB8)))),0),0);};}
# 2004
goto _LL260;case 29: _LL267: _tmp390=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp38F)->f1;_tmp391=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp38F)->f2;_LL268:
# 2006
 _tmp358=Cyc_Toc_init_anon_struct(nv,lval,_tmp390,_tmp391,_tmp358);goto _LL260;default: _LL269: _LL26A: {
# 2008
void*old_e_typ=(void*)_check_null(_tmp381->topt);
int was_ptr_type=Cyc_Toc_is_pointer_or_boxed_tvar(old_e_typ);
Cyc_Toc_exp_to_c(Cyc_Toc_set_lhs_exp(nv->rgn,lval,nv),_tmp381);
{void*_tmp39D=old_e_typ;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp39D)->tag == 11){_LL26C: _LL26D:
# 2015
 if(old_e_typ != _tmp381->topt  && !Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(old_e_typ),(void*)_check_null(_tmp381->topt))){
# 2019
ad=Cyc_Toc_update_aggr_type(ad,_tmp38D,(void*)_check_null(_tmp381->topt));
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpAC3;struct Cyc_Absyn_Aggrdecl**_tmpAC2;struct Cyc_Absyn_AggrInfo _tmpAC1;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpAC0;struct_type=(void*)(
(_tmpAC0=_cycalloc(sizeof(*_tmpAC0)),((_tmpAC0[0]=((_tmpAC3.tag=11,((_tmpAC3.f1=((_tmpAC1.aggr_info=Cyc_Absyn_KnownAggr(((_tmpAC2=_cycalloc(sizeof(*_tmpAC2)),((_tmpAC2[0]=ad,_tmpAC2))))),((_tmpAC1.targs=0,_tmpAC1)))),_tmpAC3)))),_tmpAC0))));}
# 2023
aggr_typ=Cyc_Toc_typ_to_c(struct_type);}
# 2025
goto _LL26B;}else{_LL26E: _LL26F:
 goto _LL26B;}_LL26B:;}{
# 2028
struct Cyc_Absyn_Aggrfield*_tmp3A2=Cyc_Absyn_lookup_field(aggrfields,_tmp38D);
# 2030
if(Cyc_Toc_is_poly_field(struct_type,_tmp38D) && !was_ptr_type)
_tmp381=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp381);
# 2033
if(exist_types != 0)
_tmp381=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3A2))->type),_tmp381);
# 2036
_tmp358=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp381,0),0),_tmp358,0);
goto _LL260;};}}_LL260:;}
# 2039
goto _LL25B;};}}else{_LL25E: _LL25F: {
const char*_tmpAC6;void*_tmpAC5;(_tmpAC5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAC6="array designator in struct",_tag_dyneither(_tmpAC6,sizeof(char),27))),_tag_dyneither(_tmpAC5,sizeof(void*),0)));}}_LL25B:;}_LL251:;}}
# 2046
if(aggr_typ != orig_typ  && pointer){
void*cast_type=Cyc_Absyn_cstar_typ(orig_typ,Cyc_Toc_mt_tq);
*varexp=Cyc_Toc_cast_it(cast_type,*varexp);}{
# 2051
struct Cyc_Absyn_Exp*_tmp3A5=Cyc_Toc_make_struct(nv,xexp,aggr_typ,_tmp358,pointer,rgnopt,is_atomic,do_declare);
# 2053
_tmp3A5->topt=struct_type;{
struct Cyc_Absyn_Exp*_tmp3A6=_tmp3A5;_npop_handler(0);return _tmp3A6;};};
# 1944
;_pop_region(r);};};};}struct _tuple27{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 2059
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple27*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
# 2062
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0);}
# 2065
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmpAC9;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmpAC8;return Cyc_Absyn_new_exp((void*)((_tmpAC8=_cycalloc(sizeof(*_tmpAC8)),((_tmpAC8[0]=((_tmpAC9.tag=4,((_tmpAC9.f1=e1,((_tmpAC9.f2=incr,_tmpAC9)))))),_tmpAC8)))),0);}
# 2069
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 2078
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 2080
void*_tmp3A9=e1->r;void*_tmp3AA=_tmp3A9;struct Cyc_Absyn_Exp*_tmp3AB;struct _dyneither_ptr*_tmp3AC;int _tmp3AD;int _tmp3AE;void*_tmp3AF;struct Cyc_Absyn_Exp*_tmp3B0;struct Cyc_Absyn_Stmt*_tmp3B1;switch(*((int*)_tmp3AA)){case 36: _LL271: _tmp3B1=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3AA)->f1;_LL272:
 Cyc_Toc_lvalue_assign_stmt(_tmp3B1,fs,f,f_env);goto _LL270;case 13: _LL273: _tmp3AF=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3AA)->f1;_tmp3B0=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3AA)->f2;_LL274:
 Cyc_Toc_lvalue_assign(_tmp3B0,fs,f,f_env);goto _LL270;case 20: _LL275: _tmp3AB=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3AA)->f1;_tmp3AC=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3AA)->f2;_tmp3AD=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3AA)->f3;_tmp3AE=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3AA)->f4;_LL276:
# 2085
 e1->r=_tmp3AB->r;
{struct Cyc_List_List*_tmpACA;Cyc_Toc_lvalue_assign(e1,((_tmpACA=_cycalloc(sizeof(*_tmpACA)),((_tmpACA->hd=_tmp3AC,((_tmpACA->tl=fs,_tmpACA)))))),f,f_env);}
goto _LL270;default: _LL277: _LL278: {
# 2093
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 2095
for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;
goto _LL270;}}_LL270:;}
# 2101
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 2103
void*_tmp3B3=s->r;void*_tmp3B4=_tmp3B3;struct Cyc_Absyn_Stmt*_tmp3B5;struct Cyc_Absyn_Decl*_tmp3B6;struct Cyc_Absyn_Stmt*_tmp3B7;struct Cyc_Absyn_Exp*_tmp3B8;switch(*((int*)_tmp3B4)){case 1: _LL27A: _tmp3B8=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3B4)->f1;_LL27B:
 Cyc_Toc_lvalue_assign(_tmp3B8,fs,f,f_env);goto _LL279;case 12: _LL27C: _tmp3B6=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3B4)->f1;_tmp3B7=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3B4)->f2;_LL27D:
# 2106
 Cyc_Toc_lvalue_assign_stmt(_tmp3B7,fs,f,f_env);goto _LL279;case 2: _LL27E: _tmp3B5=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3B4)->f2;_LL27F:
 Cyc_Toc_lvalue_assign_stmt(_tmp3B5,fs,f,f_env);goto _LL279;default: _LL280: _LL281: {
const char*_tmpACE;void*_tmpACD[1];struct Cyc_String_pa_PrintArg_struct _tmpACC;(_tmpACC.tag=0,((_tmpACC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpACD[0]=& _tmpACC,Cyc_Toc_toc_impos(((_tmpACE="lvalue_assign_stmt: %s",_tag_dyneither(_tmpACE,sizeof(char),23))),_tag_dyneither(_tmpACD,sizeof(void*),1)))))));}}_LL279:;}
# 2112
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 2116
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp3BC=e->r;void*_tmp3BD=_tmp3BC;struct Cyc_Absyn_Stmt*_tmp3BE;struct Cyc_Absyn_Exp*_tmp3BF;void**_tmp3C0;struct Cyc_Absyn_Exp**_tmp3C1;switch(*((int*)_tmp3BD)){case 13: _LL283: _tmp3C0=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3BD)->f1;_tmp3C1=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3BD)->f2;_LL284:
# 2119
*_tmp3C1=Cyc_Toc_push_address_exp(*_tmp3C1);
*_tmp3C0=Cyc_Absyn_cstar_typ(*_tmp3C0,Cyc_Toc_mt_tq);
return e;case 19: _LL285: _tmp3BF=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3BD)->f1;_LL286:
# 2123
 return _tmp3BF;case 36: _LL287: _tmp3BE=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3BD)->f1;_LL288:
# 2127
 Cyc_Toc_push_address_stmt(_tmp3BE);
return e;default: _LL289: _LL28A:
# 2130
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{
const char*_tmpAD2;void*_tmpAD1[1];struct Cyc_String_pa_PrintArg_struct _tmpAD0;(_tmpAD0.tag=0,((_tmpAD0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpAD1[0]=& _tmpAD0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAD2="can't take & of exp %s",_tag_dyneither(_tmpAD2,sizeof(char),23))),_tag_dyneither(_tmpAD1,sizeof(void*),1)))))));};}_LL282:;}
# 2135
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp3C5=s->r;void*_tmp3C6=_tmp3C5;struct Cyc_Absyn_Exp**_tmp3C7;struct Cyc_Absyn_Stmt*_tmp3C8;struct Cyc_Absyn_Stmt*_tmp3C9;switch(*((int*)_tmp3C6)){case 2: _LL28C: _tmp3C9=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3C6)->f2;_LL28D:
 _tmp3C8=_tmp3C9;goto _LL28F;case 12: _LL28E: _tmp3C8=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3C6)->f2;_LL28F:
 Cyc_Toc_push_address_stmt(_tmp3C8);goto _LL28B;case 1: _LL290: _tmp3C7=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3C6)->f1;_LL291:
*_tmp3C7=Cyc_Toc_push_address_exp(*_tmp3C7);goto _LL28B;default: _LL292: _LL293: {
# 2141
const char*_tmpAD6;void*_tmpAD5[1];struct Cyc_String_pa_PrintArg_struct _tmpAD4;(_tmpAD4.tag=0,((_tmpAD4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpAD5[0]=& _tmpAD4,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAD6="can't take & of stmt %s",_tag_dyneither(_tmpAD6,sizeof(char),24))),_tag_dyneither(_tmpAD5,sizeof(void*),1)))))));}}_LL28B:;}
# 2146
static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 2148
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 2150
if(x == 0)return 0;
{struct Cyc_List_List*_tmpAD7;result=((_tmpAD7=_region_malloc(r2,sizeof(*_tmpAD7)),((_tmpAD7->hd=f(x->hd,env),((_tmpAD7->tl=0,_tmpAD7))))));}
prev=result;
for(x=x->tl;x != 0;x=x->tl){
{struct Cyc_List_List*_tmpAD8;((struct Cyc_List_List*)_check_null(prev))->tl=((_tmpAD8=_region_malloc(r2,sizeof(*_tmpAD8)),((_tmpAD8->hd=f(x->hd,env),((_tmpAD8->tl=0,_tmpAD8))))));}
prev=prev->tl;}
# 2157
return result;}
# 2159
static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,x);}
# 2163
static struct _tuple19*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){
struct _tuple19*_tmpAD9;return(_tmpAD9=_cycalloc(sizeof(*_tmpAD9)),((_tmpAD9->f1=0,((_tmpAD9->f2=e,_tmpAD9)))));}
# 2167
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_tmp3D0=Cyc_Tcutil_compress(t);void*_tmp3D1=_tmp3D0;struct Cyc_Absyn_PtrInfo _tmp3D2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D1)->tag == 5){_LL295: _tmp3D2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D1)->f1;_LL296:
 return _tmp3D2;}else{_LL297: _LL298: {
const char*_tmpADC;void*_tmpADB;(_tmpADB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpADC="get_ptr_typ: not a pointer!",_tag_dyneither(_tmpADC,sizeof(char),28))),_tag_dyneither(_tmpADB,sizeof(void*),0)));}}_LL294:;}
# 2177
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp3D5=Cyc_Tcutil_compress(t);void*_tmp3D6=_tmp3D5;int _tmp3D7;enum Cyc_Absyn_Sign _tmp3D8;enum Cyc_Absyn_Sign _tmp3D9;enum Cyc_Absyn_Sign _tmp3DA;enum Cyc_Absyn_Sign _tmp3DB;enum Cyc_Absyn_Sign _tmp3DC;switch(*((int*)_tmp3D6)){case 5: _LL29A: _LL29B:
 res=Cyc_Absyn_null_exp(0);goto _LL299;case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3D6)->f2){case Cyc_Absyn_Char_sz: _LL29C: _tmp3DC=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3D6)->f1;_LL29D:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp3DC,'\000'),0);goto _LL299;case Cyc_Absyn_Short_sz: _LL29E: _tmp3DB=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3D6)->f1;_LL29F:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp3DB,0),0);goto _LL299;case Cyc_Absyn_Int_sz: _LL2A4: _tmp3DA=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3D6)->f1;_LL2A5:
# 2185
 _tmp3D9=_tmp3DA;goto _LL2A7;case Cyc_Absyn_Long_sz: _LL2A6: _tmp3D9=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3D6)->f1;_LL2A7:
# 2187
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp3D9,0),0);goto _LL299;default: _LL2A8: _tmp3D8=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3D6)->f1;_LL2A9:
# 2189
 res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp3D8,(long long)0),0);goto _LL299;}case 13: _LL2A0: _LL2A1:
# 2183
 goto _LL2A3;case 14: _LL2A2: _LL2A3:
 _tmp3DA=Cyc_Absyn_Unsigned;goto _LL2A5;case 7: switch(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3D6)->f1){case 0: _LL2AA: _LL2AB:
# 2190
{const char*_tmpADD;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpADD="0.0F",_tag_dyneither(_tmpADD,sizeof(char),5))),0),0);}goto _LL299;case 1: _LL2AC: _LL2AD:
{const char*_tmpADE;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpADE="0.0",_tag_dyneither(_tmpADE,sizeof(char),4))),1),0);}goto _LL299;default: _LL2AE: _tmp3D7=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3D6)->f1;_LL2AF:
{const char*_tmpADF;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpADF="0.0L",_tag_dyneither(_tmpADF,sizeof(char),5))),_tmp3D7),0);}goto _LL299;}default: _LL2B0: _LL2B1: {
# 2194
const char*_tmpAE3;void*_tmpAE2[1];struct Cyc_String_pa_PrintArg_struct _tmpAE1;(_tmpAE1.tag=0,((_tmpAE1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpAE2[0]=& _tmpAE1,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAE3="found non-zero type %s in generate_zero",_tag_dyneither(_tmpAE3,sizeof(char),40))),_tag_dyneither(_tmpAE2,sizeof(void*),1)))))));}}_LL299:;}
# 2196
res->topt=t;
return res;}
# 2203
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 2216 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp3E3=Cyc_Toc_typ_to_c(elt_type);
void*_tmp3E4=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp3E5=Cyc_Absyn_cstar_typ(_tmp3E3,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmpAE4;struct Cyc_Core_Opt*_tmp3E6=(_tmpAE4=_cycalloc(sizeof(*_tmpAE4)),((_tmpAE4->v=_tmp3E5,_tmpAE4)));
struct Cyc_Absyn_Exp*xinit;
{void*_tmp3E7=e1->r;void*_tmp3E8=_tmp3E7;struct Cyc_Absyn_Exp*_tmp3E9;struct Cyc_Absyn_Exp*_tmp3EA;struct Cyc_Absyn_Exp*_tmp3EB;switch(*((int*)_tmp3E8)){case 19: _LL2B3: _tmp3EB=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3E8)->f1;_LL2B4:
# 2224
 if(!is_dyneither){
_tmp3EB=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp3EB,0,Cyc_Absyn_Other_coercion,0);
_tmp3EB->topt=fat_ptr_type;}
# 2228
Cyc_Toc_exp_to_c(nv,_tmp3EB);xinit=_tmp3EB;goto _LL2B2;case 22: _LL2B5: _tmp3E9=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3E8)->f1;_tmp3EA=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3E8)->f2;_LL2B6:
# 2230
 if(!is_dyneither){
_tmp3E9=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp3E9,0,Cyc_Absyn_Other_coercion,0);
_tmp3E9->topt=fat_ptr_type;}
# 2234
Cyc_Toc_exp_to_c(nv,_tmp3E9);Cyc_Toc_exp_to_c(nv,_tmp3EA);
{struct Cyc_Absyn_Exp*_tmpAE5[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpAE5[2]=_tmp3EA,((_tmpAE5[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpAE5[0]=_tmp3E9,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAE5,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL2B2;default: _LL2B7: _LL2B8: {
const char*_tmpAE8;void*_tmpAE7;(_tmpAE7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAE8="found bad lhs for zero-terminated pointer assignment",_tag_dyneither(_tmpAE8,sizeof(char),53))),_tag_dyneither(_tmpAE7,sizeof(void*),0)));}}_LL2B2:;}{
# 2240
struct _tuple1*_tmp3EF=Cyc_Toc_temp_var();
struct _RegionHandle _tmp3F0=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp3F0;_push_region(rgn2);
{struct Cyc_Toc_Env*_tmp3F1=Cyc_Toc_add_varmap(rgn2,nv,_tmp3EF,Cyc_Absyn_var_exp(_tmp3EF,0));
struct Cyc_Absyn_Vardecl*_tmpAE9;struct Cyc_Absyn_Vardecl*_tmp3F2=(_tmpAE9=_cycalloc(sizeof(*_tmpAE9)),((_tmpAE9->sc=Cyc_Absyn_Public,((_tmpAE9->name=_tmp3EF,((_tmpAE9->tq=Cyc_Toc_mt_tq,((_tmpAE9->type=_tmp3E4,((_tmpAE9->initializer=xinit,((_tmpAE9->rgn=0,((_tmpAE9->attributes=0,((_tmpAE9->escapes=0,_tmpAE9)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpAEC;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpAEB;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp3F3=(_tmpAEB=_cycalloc(sizeof(*_tmpAEB)),((_tmpAEB[0]=((_tmpAEC.tag=4,((_tmpAEC.f1=_tmp3F2,_tmpAEC)))),_tmpAEB)));
struct Cyc_Absyn_Exp*_tmp3F4=Cyc_Absyn_varb_exp(_tmp3EF,(void*)_tmp3F3,0);
_tmp3F4->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp3F5=Cyc_Absyn_deref_exp(_tmp3F4,0);
_tmp3F5->topt=elt_type;
Cyc_Toc_exp_to_c(_tmp3F1,_tmp3F5);{
struct _tuple1*_tmp3F6=Cyc_Toc_temp_var();
_tmp3F1=Cyc_Toc_add_varmap(rgn2,_tmp3F1,_tmp3F6,Cyc_Absyn_var_exp(_tmp3F6,0));{
struct Cyc_Absyn_Vardecl*_tmpAED;struct Cyc_Absyn_Vardecl*_tmp3F7=(_tmpAED=_cycalloc(sizeof(*_tmpAED)),((_tmpAED->sc=Cyc_Absyn_Public,((_tmpAED->name=_tmp3F6,((_tmpAED->tq=Cyc_Toc_mt_tq,((_tmpAED->type=_tmp3E3,((_tmpAED->initializer=_tmp3F5,((_tmpAED->rgn=0,((_tmpAED->attributes=0,((_tmpAED->escapes=0,_tmpAED)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpAF0;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpAEF;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp3F8=(_tmpAEF=_cycalloc(sizeof(*_tmpAEF)),((_tmpAEF[0]=((_tmpAF0.tag=4,((_tmpAF0.f1=_tmp3F7,_tmpAF0)))),_tmpAEF)));
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp3F9=Cyc_Absyn_varb_exp(_tmp3F6,(void*)_tmp3F8,0);
_tmp3F9->topt=_tmp3F5->topt;
z_init=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp3F9,e2,0);
z_init->topt=_tmp3F9->topt;}
# 2261
Cyc_Toc_exp_to_c(_tmp3F1,z_init);{
struct _tuple1*_tmp3FA=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmpAF1;struct Cyc_Absyn_Vardecl*_tmp3FB=(_tmpAF1=_cycalloc(sizeof(*_tmpAF1)),((_tmpAF1->sc=Cyc_Absyn_Public,((_tmpAF1->name=_tmp3FA,((_tmpAF1->tq=Cyc_Toc_mt_tq,((_tmpAF1->type=_tmp3E3,((_tmpAF1->initializer=z_init,((_tmpAF1->rgn=0,((_tmpAF1->attributes=0,((_tmpAF1->escapes=0,_tmpAF1)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpAF4;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpAF3;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp3FC=(_tmpAF3=_cycalloc(sizeof(*_tmpAF3)),((_tmpAF3[0]=((_tmpAF4.tag=4,((_tmpAF4.f1=_tmp3FB,_tmpAF4)))),_tmpAF3)));
_tmp3F1=Cyc_Toc_add_varmap(rgn2,_tmp3F1,_tmp3FA,Cyc_Absyn_var_exp(_tmp3FA,0));{
# 2268
struct Cyc_Absyn_Exp*_tmp3FD=Cyc_Absyn_varb_exp(_tmp3F6,(void*)_tmp3F8,0);_tmp3FD->topt=_tmp3F5->topt;{
struct Cyc_Absyn_Exp*_tmp3FE=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp3FF=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp3FD,_tmp3FE,0);
_tmp3FF->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp3F1,_tmp3FF);{
# 2274
struct Cyc_Absyn_Exp*_tmp400=Cyc_Absyn_varb_exp(_tmp3FA,(void*)_tmp3FC,0);_tmp400->topt=_tmp3F5->topt;{
struct Cyc_Absyn_Exp*_tmp401=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp402=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp400,_tmp401,0);
_tmp402->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp3F1,_tmp402);{
# 2280
struct Cyc_Absyn_Exp*_tmpAF5[2];struct Cyc_List_List*_tmp403=(_tmpAF5[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpAF5[0]=
# 2280
Cyc_Absyn_varb_exp(_tmp3EF,(void*)_tmp3F3,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAF5,sizeof(struct Cyc_Absyn_Exp*),2)))));
# 2282
struct Cyc_Absyn_Exp*_tmp404=Cyc_Absyn_uint_exp(1,0);
struct Cyc_Absyn_Exp*xsize;
xsize=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp403,0),_tmp404,0);{
# 2287
struct Cyc_Absyn_Exp*_tmp405=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp3FF,_tmp402,0),0);
struct Cyc_Absyn_Stmt*_tmp406=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0),0);
struct Cyc_Absyn_Exp*_tmp407=Cyc_Toc_member_exp(Cyc_Absyn_varb_exp(_tmp3EF,(void*)_tmp3F3,0),Cyc_Toc_curr_sp,0);
_tmp407=Cyc_Toc_cast_it(_tmp3E5,_tmp407);{
struct Cyc_Absyn_Exp*_tmp408=Cyc_Absyn_deref_exp(_tmp407,0);
struct Cyc_Absyn_Exp*_tmp409=Cyc_Absyn_assign_exp(_tmp408,Cyc_Absyn_var_exp(_tmp3FA,0),0);
struct Cyc_Absyn_Stmt*_tmp40A=Cyc_Absyn_exp_stmt(_tmp409,0);
_tmp40A=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp405,_tmp406,Cyc_Absyn_skip_stmt(0),0),_tmp40A,0);
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpAFB;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpAFA;struct Cyc_Absyn_Decl*_tmpAF9;_tmp40A=Cyc_Absyn_decl_stmt(((_tmpAF9=_cycalloc(sizeof(*_tmpAF9)),((_tmpAF9->r=(void*)((_tmpAFB=_cycalloc(sizeof(*_tmpAFB)),((_tmpAFB[0]=((_tmpAFA.tag=0,((_tmpAFA.f1=_tmp3FB,_tmpAFA)))),_tmpAFB)))),((_tmpAF9->loc=0,_tmpAF9)))))),_tmp40A,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB01;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB00;struct Cyc_Absyn_Decl*_tmpAFF;_tmp40A=Cyc_Absyn_decl_stmt(((_tmpAFF=_cycalloc(sizeof(*_tmpAFF)),((_tmpAFF->r=(void*)((_tmpB01=_cycalloc(sizeof(*_tmpB01)),((_tmpB01[0]=((_tmpB00.tag=0,((_tmpB00.f1=_tmp3F7,_tmpB00)))),_tmpB01)))),((_tmpAFF->loc=0,_tmpAFF)))))),_tmp40A,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB07;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB06;struct Cyc_Absyn_Decl*_tmpB05;_tmp40A=Cyc_Absyn_decl_stmt(((_tmpB05=_cycalloc(sizeof(*_tmpB05)),((_tmpB05->r=(void*)((_tmpB07=_cycalloc(sizeof(*_tmpB07)),((_tmpB07[0]=((_tmpB06.tag=0,((_tmpB06.f1=_tmp3F2,_tmpB06)))),_tmpB07)))),((_tmpB05->loc=0,_tmpB05)))))),_tmp40A,0);}
e->r=Cyc_Toc_stmt_exp_r(_tmp40A);};};};};};};};};};};};}
# 2242
;_pop_region(rgn2);};}
# 2313 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 2317
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp41F=Cyc_Tcutil_compress(aggrtype);void*_tmp420=_tmp41F;union Cyc_Absyn_AggrInfoU _tmp421;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp420)->tag == 11){_LL2BA: _tmp421=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp420)->f1).aggr_info;_LL2BB:
# 2320
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp421);goto _LL2B9;}else{_LL2BC: _LL2BD: {
struct Cyc_String_pa_PrintArg_struct _tmpB0F;void*_tmpB0E[1];const char*_tmpB0D;void*_tmpB0C;(_tmpB0C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmpB0F.tag=0,((_tmpB0F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpB0E[0]=& _tmpB0F,Cyc_aprintf(((_tmpB0D="expecting union but found %s in check_tagged_union",_tag_dyneither(_tmpB0D,sizeof(char),51))),_tag_dyneither(_tmpB0E,sizeof(void*),1)))))))),_tag_dyneither(_tmpB0C,sizeof(void*),0)));}}_LL2B9:;}{
# 2324
struct _tuple1*_tmp426=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp427=Cyc_Absyn_var_exp(_tmp426,0);
struct Cyc_Absyn_Exp*_tmp428=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp429=Cyc_Absyn_aggrarrow_exp(_tmp427,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp42A=Cyc_Absyn_neq_exp(_tmp429,_tmp428,0);
struct Cyc_Absyn_Exp*_tmp42B=Cyc_Absyn_aggrarrow_exp(_tmp427,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp42C=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp42B,0),0);
struct Cyc_Absyn_Stmt*_tmp42D=Cyc_Absyn_ifthenelse_stmt(_tmp42A,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
void*_tmp42E=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp42F=Cyc_Absyn_address_exp(aggrproj(e1,f,0),0);
struct Cyc_Absyn_Stmt*_tmp430=Cyc_Absyn_declare_stmt(_tmp426,_tmp42E,_tmp42F,Cyc_Absyn_seq_stmt(_tmp42D,_tmp42C,0),0);
return Cyc_Toc_stmt_exp_r(_tmp430);}else{
# 2338
struct Cyc_Absyn_Exp*_tmp431=Cyc_Toc_member_exp(aggrproj(_tmp427,f,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp432=Cyc_Absyn_neq_exp(_tmp431,_tmp428,0);
struct Cyc_Absyn_Exp*_tmp433=Cyc_Toc_member_exp(aggrproj(_tmp427,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp434=Cyc_Absyn_exp_stmt(_tmp433,0);
struct Cyc_Absyn_Stmt*_tmp435=Cyc_Absyn_ifthenelse_stmt(_tmp432,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*_tmp436=Cyc_Absyn_declare_stmt(_tmp426,e1_c_type,e1,Cyc_Absyn_seq_stmt(_tmp435,_tmp434,0),0);
return Cyc_Toc_stmt_exp_r(_tmp436);}};}
# 2348
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _dyneither_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 2351
void*_tmp437=Cyc_Tcutil_compress(t);void*_tmp438=_tmp437;union Cyc_Absyn_AggrInfoU _tmp439;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp438)->tag == 11){_LL2BF: _tmp439=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp438)->f1).aggr_info;_LL2C0: {
# 2353
struct Cyc_Absyn_Aggrdecl*_tmp43A=Cyc_Absyn_get_known_aggrdecl(_tmp439);
*f_tag=Cyc_Toc_get_member_offset(_tmp43A,f);{
# 2356
const char*_tmpB14;void*_tmpB13[2];struct Cyc_String_pa_PrintArg_struct _tmpB12;struct Cyc_String_pa_PrintArg_struct _tmpB11;struct _dyneither_ptr str=(struct _dyneither_ptr)((_tmpB11.tag=0,((_tmpB11.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpB12.tag=0,((_tmpB12.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp43A->name).f2),((_tmpB13[0]=& _tmpB12,((_tmpB13[1]=& _tmpB11,Cyc_aprintf(((_tmpB14="_union_%s_%s",_tag_dyneither(_tmpB14,sizeof(char),13))),_tag_dyneither(_tmpB13,sizeof(void*),2))))))))))))));
{struct _dyneither_ptr*_tmpB15;*tagged_member_type=Cyc_Absyn_strct(((_tmpB15=_cycalloc(sizeof(*_tmpB15)),((_tmpB15[0]=str,_tmpB15)))));}
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp43A->impl))->tagged;};}}else{_LL2C1: _LL2C2:
 return 0;}_LL2BE:;}
# 2367
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 2370
void*_tmp440=e->r;void*_tmp441=_tmp440;struct Cyc_Absyn_Exp*_tmp442;struct _dyneither_ptr*_tmp443;int*_tmp444;struct Cyc_Absyn_Exp*_tmp445;struct _dyneither_ptr*_tmp446;int*_tmp447;struct Cyc_Absyn_Exp*_tmp448;switch(*((int*)_tmp441)){case 13: _LL2C4: _tmp448=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp441)->f2;_LL2C5: {
const char*_tmpB18;void*_tmpB17;(_tmpB17=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB18="cast on lhs!",_tag_dyneither(_tmpB18,sizeof(char),13))),_tag_dyneither(_tmpB17,sizeof(void*),0)));}case 20: _LL2C6: _tmp445=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp441)->f1;_tmp446=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp441)->f2;_tmp447=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp441)->f4;_LL2C7:
# 2373
 return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(_tmp445->topt),_tmp446,f_tag,tagged_member_type,clear_read,_tmp447);case 21: _LL2C8: _tmp442=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp441)->f1;_tmp443=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp441)->f2;_tmp444=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp441)->f4;_LL2C9: {
# 2376
void*_tmp44B=Cyc_Tcutil_compress((void*)_check_null(_tmp442->topt));void*_tmp44C=_tmp44B;void*_tmp44D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44C)->tag == 5){_LL2CD: _tmp44D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44C)->f1).elt_typ;_LL2CE:
# 2378
 return Cyc_Toc_is_tagged_union_project_impl(_tmp44D,_tmp443,f_tag,tagged_member_type,clear_read,_tmp444);}else{_LL2CF: _LL2D0:
# 2380
 return 0;}_LL2CC:;}default: _LL2CA: _LL2CB:
# 2382
 return 0;}_LL2C3:;}
# 2394 "toc.cyc"
void Cyc_Toc_add_tagged_union_check_for_swap(struct Cyc_Absyn_Exp*e,int tag,void*mem_type){
struct _tuple1*_tmp44E=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp44E,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_val,0),0);
struct Cyc_Absyn_Exp*_tmp44F=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_ifthenelse_stmt(_tmp44F,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp44E,Cyc_Absyn_cstar_typ(mem_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(e)),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
e->r=Cyc_Toc_stmt_exp_r(s1);}
# 2418 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 2422
struct _tuple1*_tmp450=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp450,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{
# 2432
struct Cyc_Absyn_Exp*_tmp451=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
s2=Cyc_Absyn_ifthenelse_stmt(_tmp451,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{
# 2435
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp450,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(e1),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple28{struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};
# 2441
static void Cyc_Toc_new_comprehension_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,void*new_e_type,struct Cyc_Absyn_Exp*rgnopt,void*old_typ,struct Cyc_Absyn_Exp*e1,void*t1,int zero_term,struct Cyc_Absyn_Exp*init_exp,struct Cyc_Absyn_Vardecl*vd){
# 2456 "toc.cyc"
struct _tuple1*max=Cyc_Toc_temp_var();
struct _tuple1*a=Cyc_Toc_temp_var();
void*old_elt_typ=t1;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c(nv,e1);{
struct Cyc_Absyn_Exp*_tmp452=Cyc_Absyn_var_exp(max,0);
# 2464
struct Cyc_Absyn_Stmt*s;
if(zero_term)
_tmp452=Cyc_Absyn_add_exp(_tmp452,Cyc_Absyn_uint_exp(1,0),0);
if(init_exp != 0)
s=Cyc_Toc_init_comprehension(nv,new_e_type,
Cyc_Absyn_var_exp(a,0),(struct Cyc_Absyn_Vardecl*)_check_null(vd),Cyc_Absyn_var_exp(max,0),init_exp,zero_term,
Cyc_Toc_skip_stmt_dl(),1);else{
if(zero_term)
s=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(a,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(elt_typ,Cyc_Absyn_uint_exp(0,0)),0),0);else{
# 2475
s=Cyc_Absyn_skip_stmt(0);}}{
# 2477
struct _RegionHandle _tmp453=_new_region("r");struct _RegionHandle*r=& _tmp453;_push_region(r);
{struct _tuple28*_tmpB1B;struct Cyc_List_List*_tmpB1A;struct Cyc_List_List*decls=
(_tmpB1A=_region_malloc(r,sizeof(*_tmpB1A)),((_tmpB1A->hd=((_tmpB1B=_region_malloc(r,sizeof(*_tmpB1B)),((_tmpB1B->f1=max,((_tmpB1B->f2=Cyc_Absyn_uint_typ,((_tmpB1B->f3=e1,_tmpB1B)))))))),((_tmpB1A->tl=0,_tmpB1A)))));
struct Cyc_Absyn_Exp*ai;
if(rgnopt == 0  || Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*_tmpB1C[2];ai=Cyc_Toc_malloc_exp(old_elt_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpB1C[1]=_tmp452,((_tmpB1C[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB1C,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));}else{
# 2487
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);{
struct Cyc_Absyn_Exp*_tmpB1D[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpB1D[1]=_tmp452,((_tmpB1D[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB1D,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{
# 2493
struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);
{struct _tuple28*_tmpB20;struct Cyc_List_List*_tmpB1F;decls=((_tmpB1F=_region_malloc(r,sizeof(*_tmpB1F)),((_tmpB1F->hd=((_tmpB20=_region_malloc(r,sizeof(*_tmpB20)),((_tmpB20->f1=a,((_tmpB20->f2=ptr_typ,((_tmpB20->f3=ainit,_tmpB20)))))))),((_tmpB1F->tl=decls,_tmpB1F))))));}
if(!Cyc_Tcutil_is_pointer_type(old_typ)){
const char*_tmpB23;void*_tmpB22;(_tmpB22=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB23="new_comprehension_to_c: comprehension is not an array type",_tag_dyneither(_tmpB23,sizeof(char),59))),_tag_dyneither(_tmpB22,sizeof(void*),0)));}
if(Cyc_Tcutil_is_dyneither_ptr(old_typ)){
struct _tuple1*_tmp45A=Cyc_Toc_temp_var();
void*_tmp45B=Cyc_Toc_typ_to_c(old_typ);
struct Cyc_Absyn_Exp*_tmp45C=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpB24[3];struct Cyc_Absyn_Exp*_tmp45D=
Cyc_Absyn_fncall_exp(_tmp45C,(
(_tmpB24[2]=_tmp452,((_tmpB24[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((_tmpB24[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB24,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 2505
{struct _tuple28*_tmpB27;struct Cyc_List_List*_tmpB26;decls=((_tmpB26=_region_malloc(r,sizeof(*_tmpB26)),((_tmpB26->hd=((_tmpB27=_region_malloc(r,sizeof(*_tmpB27)),((_tmpB27->f1=_tmp45A,((_tmpB27->f2=_tmp45B,((_tmpB27->f3=_tmp45D,_tmpB27)))))))),((_tmpB26->tl=decls,_tmpB26))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp45A,0),0),0);}else{
# 2508
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}
{struct Cyc_List_List*_tmp461=decls;for(0;_tmp461 != 0;_tmp461=_tmp461->tl){
struct _tuple28 _tmp462=*((struct _tuple28*)_tmp461->hd);struct _tuple28 _tmp463=_tmp462;struct _tuple1*_tmp464;void*_tmp465;struct Cyc_Absyn_Exp*_tmp466;_LL2D2: _tmp464=_tmp463.f1;_tmp465=_tmp463.f2;_tmp466=_tmp463.f3;_LL2D3:
 s=Cyc_Absyn_declare_stmt(_tmp464,_tmp465,_tmp466,s,0);_LL2D1:;}}
# 2513
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2478
;_pop_region(r);};};}struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static void _tmpB76(unsigned int*_tmpB75,unsigned int*_tmpB74,struct _tuple1***_tmpB72){for(*_tmpB75=0;*_tmpB75 < *_tmpB74;(*_tmpB75)++){(*_tmpB72)[*_tmpB75]=(struct _tuple1*)
# 2888 "toc.cyc"
Cyc_Toc_temp_var();}}
# 2517 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
void*_tmp469=e->r;
if(e->topt == 0){
const char*_tmpB2B;void*_tmpB2A[1];struct Cyc_String_pa_PrintArg_struct _tmpB29;(_tmpB29.tag=0,((_tmpB29.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpB2A[0]=& _tmpB29,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB2B="exp_to_c: no type for %s",_tag_dyneither(_tmpB2B,sizeof(char),25))),_tag_dyneither(_tmpB2A,sizeof(void*),1)))))));}
# 2523
if((nv->struct_info).lhs_exp != 0){
void*_tmp46D=_tmp469;if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp46D)->tag == 28){_LL2D7: _LL2D8:
 goto _LL2D6;}else{_LL2D9: _LL2DA:
 nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);}_LL2D6:;}{
# 2531
void*old_typ=(void*)_check_null(e->topt);
void*_tmp46E=_tmp469;struct Cyc_Absyn_Stmt*_tmp46F;struct Cyc_Absyn_Exp*_tmp470;struct _dyneither_ptr*_tmp471;struct Cyc_Absyn_Exp*_tmp472;struct Cyc_Absyn_Exp*_tmp473;int _tmp474;struct Cyc_Absyn_Exp*_tmp475;void**_tmp476;struct Cyc_Absyn_Exp*_tmp477;int _tmp478;int _tmp479;struct Cyc_List_List*_tmp47A;struct Cyc_Absyn_Datatypedecl*_tmp47B;struct Cyc_Absyn_Datatypefield*_tmp47C;void*_tmp47D;struct Cyc_List_List*_tmp47E;struct _tuple1*_tmp47F;struct Cyc_List_List*_tmp480;struct Cyc_List_List*_tmp481;struct Cyc_Absyn_Aggrdecl*_tmp482;struct Cyc_Absyn_Exp*_tmp483;void*_tmp484;int _tmp485;struct Cyc_Absyn_Vardecl*_tmp486;struct Cyc_Absyn_Exp*_tmp487;struct Cyc_Absyn_Exp*_tmp488;int _tmp489;struct Cyc_List_List*_tmp48A;struct Cyc_List_List*_tmp48B;struct Cyc_Absyn_Exp*_tmp48C;struct Cyc_Absyn_Exp*_tmp48D;struct Cyc_Absyn_Exp*_tmp48E;struct _dyneither_ptr*_tmp48F;int _tmp490;int _tmp491;struct Cyc_Absyn_Exp*_tmp492;struct _dyneither_ptr*_tmp493;int _tmp494;int _tmp495;struct Cyc_Absyn_Exp*_tmp496;void*_tmp497;struct Cyc_List_List*_tmp498;void*_tmp499;struct Cyc_Absyn_Exp*_tmp49A;struct Cyc_Absyn_Exp*_tmp49B;struct Cyc_Absyn_Exp*_tmp49C;struct Cyc_Absyn_Exp*_tmp49D;void**_tmp49E;struct Cyc_Absyn_Exp*_tmp49F;int _tmp4A0;enum Cyc_Absyn_Coercion _tmp4A1;struct Cyc_Absyn_Exp*_tmp4A2;struct Cyc_List_List*_tmp4A3;struct Cyc_Absyn_Exp*_tmp4A4;struct Cyc_Absyn_Exp*_tmp4A5;int _tmp4A6;struct Cyc_Absyn_Exp*_tmp4A7;struct Cyc_List_List*_tmp4A8;int _tmp4A9;struct Cyc_List_List*_tmp4AA;struct Cyc_Absyn_VarargInfo*_tmp4AB;struct Cyc_Absyn_Exp*_tmp4AC;struct Cyc_List_List*_tmp4AD;struct Cyc_Absyn_Exp*_tmp4AE;struct Cyc_Absyn_Exp*_tmp4AF;struct Cyc_Absyn_Exp*_tmp4B0;struct Cyc_Absyn_Exp*_tmp4B1;struct Cyc_Absyn_Exp*_tmp4B2;struct Cyc_Absyn_Exp*_tmp4B3;struct Cyc_Absyn_Exp*_tmp4B4;struct Cyc_Absyn_Exp*_tmp4B5;struct Cyc_Absyn_Exp*_tmp4B6;struct Cyc_Absyn_Exp*_tmp4B7;struct Cyc_Core_Opt*_tmp4B8;struct Cyc_Absyn_Exp*_tmp4B9;struct Cyc_Absyn_Exp*_tmp4BA;enum Cyc_Absyn_Incrementor _tmp4BB;enum Cyc_Absyn_Primop _tmp4BC;struct Cyc_List_List*_tmp4BD;struct _tuple1*_tmp4BE;void*_tmp4BF;switch(*((int*)_tmp46E)){case 0: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp46E)->f1).Null_c).tag == 1){_LL2DC: _LL2DD: {
# 2538
struct Cyc_Absyn_Exp*_tmp4C0=Cyc_Absyn_uint_exp(0,0);
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp4C0,_tmp4C0))->r;else{
# 2543
struct Cyc_Absyn_Exp*_tmpB2C[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((_tmpB2C[2]=_tmp4C0,((_tmpB2C[1]=_tmp4C0,((_tmpB2C[0]=_tmp4C0,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB2C,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}else{
# 2545
e->r=(void*)& Cyc_Toc_zero_exp;}
# 2547
goto _LL2DB;}}else{_LL2DE: _LL2DF:
 goto _LL2DB;}case 1: _LL2E0: _tmp4BE=((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp4BF=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL2E1:
# 2550
{struct _handler_cons _tmp4C2;_push_handler(& _tmp4C2);{int _tmp4C4=0;if(setjmp(_tmp4C2.handler))_tmp4C4=1;if(!_tmp4C4){e->r=(Cyc_Toc_lookup_varmap(nv,_tmp4BE))->r;;_pop_handler();}else{void*_tmp4C3=(void*)_exn_thrown;void*_tmp4C5=_tmp4C3;void*_tmp4C6;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp4C5)->tag == Cyc_Dict_Absent){_LL331: _LL332: {
# 2552
const char*_tmpB30;void*_tmpB2F[1];struct Cyc_String_pa_PrintArg_struct _tmpB2E;(_tmpB2E.tag=0,((_tmpB2E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4BE)),((_tmpB2F[0]=& _tmpB2E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB30="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpB30,sizeof(char),32))),_tag_dyneither(_tmpB2F,sizeof(void*),1)))))));}}else{_LL333: _tmp4C6=_tmp4C5;_LL334:(int)_rethrow(_tmp4C6);}_LL330:;}};}
# 2554
goto _LL2DB;case 2: _LL2E2: _tmp4BC=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp4BD=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL2E3: {
# 2557
struct Cyc_List_List*_tmp4CA=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp4BD);
# 2559
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4BD);
{enum Cyc_Absyn_Primop _tmp4CB=_tmp4BC;switch(_tmp4CB){case Cyc_Absyn_Numelts: _LL336: _LL337: {
# 2562
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4BD))->hd;
{void*_tmp4CC=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_tmp4CD=_tmp4CC;void*_tmp4CE;union Cyc_Absyn_Constraint*_tmp4CF;union Cyc_Absyn_Constraint*_tmp4D0;union Cyc_Absyn_Constraint*_tmp4D1;struct Cyc_Absyn_Exp*_tmp4D2;switch(*((int*)_tmp4CD)){case 8: _LL34B: _tmp4D2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4CD)->f1).num_elts;_LL34C:
# 2566
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp4D2))){
const char*_tmpB33;void*_tmpB32;(_tmpB32=0,Cyc_Tcutil_terr(e->loc,((_tmpB33="can't calculate numelts",_tag_dyneither(_tmpB33,sizeof(char),24))),_tag_dyneither(_tmpB32,sizeof(void*),0)));}
e->r=_tmp4D2->r;goto _LL34A;case 5: _LL34D: _tmp4CE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CD)->f1).elt_typ;_tmp4CF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CD)->f1).ptr_atts).nullable;_tmp4D0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CD)->f1).ptr_atts).bounds;_tmp4D1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CD)->f1).ptr_atts).zero_term;_LL34E:
# 2570
{void*_tmp4D5=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp4D0);void*_tmp4D6=_tmp4D5;struct Cyc_Absyn_Exp*_tmp4D7;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp4D6)->tag == 0){_LL352: _LL353:
# 2572
{struct Cyc_Absyn_Exp*_tmpB34[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,(
(_tmpB34[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp4CE),0),((_tmpB34[0]=(struct Cyc_Absyn_Exp*)_tmp4BD->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB34,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL351;}else{_LL354: _tmp4D7=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4D6)->f1;_LL355:
# 2578
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4D1)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp4BD->hd);
# 2582
struct Cyc_Absyn_Exp*_tmpB35[2];e->r=Cyc_Toc_fncall_exp_r(function_e,((_tmpB35[1]=_tmp4D7,((_tmpB35[0]=(struct Cyc_Absyn_Exp*)_tmp4BD->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB35,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4CF)){
if(!Cyc_Evexp_c_can_eval(_tmp4D7)){
const char*_tmpB38;void*_tmpB37;(_tmpB37=0,Cyc_Tcutil_terr(e->loc,((_tmpB38="can't calculate numelts",_tag_dyneither(_tmpB38,sizeof(char),24))),_tag_dyneither(_tmpB37,sizeof(void*),0)));}
# 2587
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp4D7,Cyc_Absyn_uint_exp(0,0));}else{
# 2589
e->r=_tmp4D7->r;goto _LL351;}}
# 2591
goto _LL351;}_LL351:;}
# 2593
goto _LL34A;default: _LL34F: _LL350: {
# 2595
const char*_tmpB3D;void*_tmpB3C[2];struct Cyc_String_pa_PrintArg_struct _tmpB3B;struct Cyc_String_pa_PrintArg_struct _tmpB3A;(_tmpB3A.tag=0,((_tmpB3A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpB3B.tag=0,((_tmpB3B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpB3C[0]=& _tmpB3B,((_tmpB3C[1]=& _tmpB3A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB3D="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpB3D,sizeof(char),41))),_tag_dyneither(_tmpB3C,sizeof(void*),2)))))))))))));}}_LL34A:;}
# 2598
goto _LL335;}case Cyc_Absyn_Plus: _LL338: _LL339:
# 2603
{void*_tmp4E0=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp4CA))->hd);void*_tmp4E1=_tmp4E0;void*_tmp4E2;union Cyc_Absyn_Constraint*_tmp4E3;union Cyc_Absyn_Constraint*_tmp4E4;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E1)->tag == 5){_LL357: _tmp4E2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E1)->f1).elt_typ;_tmp4E3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E1)->f1).ptr_atts).bounds;_tmp4E4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E1)->f1).ptr_atts).zero_term;_LL358:
# 2605
{void*_tmp4E5=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp4E3);void*_tmp4E6=_tmp4E5;struct Cyc_Absyn_Exp*_tmp4E7;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp4E6)->tag == 0){_LL35C: _LL35D: {
# 2607
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4BD))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4BD->tl))->hd;
{struct Cyc_Absyn_Exp*_tmpB3E[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpB3E[2]=e2,((_tmpB3E[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp4E2),0),((_tmpB3E[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB3E,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}
goto _LL35B;}}else{_LL35E: _tmp4E7=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4E6)->f1;_LL35F:
# 2613
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4E4)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4BD))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4BD->tl))->hd;
struct Cyc_Absyn_Exp*_tmpB3F[3];e->r=(Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1),((_tmpB3F[2]=e2,((_tmpB3F[1]=_tmp4E7,((_tmpB3F[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB3F,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0))->r;}
# 2618
goto _LL35B;}_LL35B:;}
# 2620
goto _LL356;}else{_LL359: _LL35A:
# 2622
 goto _LL356;}_LL356:;}
# 2624
goto _LL335;case Cyc_Absyn_Minus: _LL33A: _LL33B: {
# 2629
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(_tmp4CA))->hd,& elt_typ)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4BD))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4BD->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(_tmp4CA->tl))->hd)){
e1->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);
e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp);
e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{
# 2639
struct Cyc_Absyn_Exp*_tmpB40[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpB40[2]=
Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0),((_tmpB40[1]=
# 2640
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpB40[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB40,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 2644
goto _LL335;}case Cyc_Absyn_Eq: _LL33C: _LL33D:
 goto _LL33F;case Cyc_Absyn_Neq: _LL33E: _LL33F:
 goto _LL341;case Cyc_Absyn_Gt: _LL340: _LL341:
 goto _LL343;case Cyc_Absyn_Gte: _LL342: _LL343:
 goto _LL345;case Cyc_Absyn_Lt: _LL344: _LL345:
 goto _LL347;case Cyc_Absyn_Lte: _LL346: _LL347: {
# 2652
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4BD))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4BD->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp4CA))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp4CA->tl))->hd;
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& elt_typ))
e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,0));
if(Cyc_Tcutil_is_tagged_pointer_typ(t2))
e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp,0));
goto _LL335;}default: _LL348: _LL349:
 goto _LL335;}_LL335:;}
# 2664
goto _LL2DB;}case 4: _LL2E4: _tmp4BA=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp4BB=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL2E5: {
# 2666
void*e2_cyc_typ=(void*)_check_null(_tmp4BA->topt);
# 2675 "toc.cyc"
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
const char*_tmpB41;struct _dyneither_ptr incr_str=(_tmpB41="increment",_tag_dyneither(_tmpB41,sizeof(char),10));
if(_tmp4BB == Cyc_Absyn_PreDec  || _tmp4BB == Cyc_Absyn_PostDec){const char*_tmpB42;incr_str=((_tmpB42="decrement",_tag_dyneither(_tmpB42,sizeof(char),10)));}
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4BA,& ptr_type,& is_dyneither,& elt_type)){
{const char*_tmpB46;void*_tmpB45[1];struct Cyc_String_pa_PrintArg_struct _tmpB44;(_tmpB44.tag=0,((_tmpB44.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpB45[0]=& _tmpB44,Cyc_Tcutil_terr(e->loc,((_tmpB46="in-place %s is not supported when dereferencing a zero-terminated pointer",_tag_dyneither(_tmpB46,sizeof(char),74))),_tag_dyneither(_tmpB45,sizeof(void*),1)))))));}{
const char*_tmpB49;void*_tmpB48;(_tmpB48=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB49="in-place inc/dec on zero-term",_tag_dyneither(_tmpB49,sizeof(char),30))),_tag_dyneither(_tmpB48,sizeof(void*),0)));};}{
# 2684
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
int f_tag=0;
if(Cyc_Toc_is_tagged_union_project(_tmp4BA,& f_tag,& tunion_member_type,1)){
struct Cyc_Absyn_Exp*_tmp4F1=Cyc_Absyn_signed_int_exp(1,0);
_tmp4F1->topt=Cyc_Absyn_sint_typ;{
enum Cyc_Absyn_Incrementor _tmp4F2=_tmp4BB;switch(_tmp4F2){case Cyc_Absyn_PreInc: _LL361: _LL362:
# 2691
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpB4F;struct Cyc_Core_Opt*_tmpB4E;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpB4D;e->r=(void*)((_tmpB4D=_cycalloc(sizeof(*_tmpB4D)),((_tmpB4D[0]=((_tmpB4F.tag=3,((_tmpB4F.f1=_tmp4BA,((_tmpB4F.f2=((_tmpB4E=_cycalloc_atomic(sizeof(*_tmpB4E)),((_tmpB4E->v=(void*)Cyc_Absyn_Plus,_tmpB4E)))),((_tmpB4F.f3=_tmp4F1,_tmpB4F)))))))),_tmpB4D))));}
Cyc_Toc_exp_to_c(nv,e);
return;case Cyc_Absyn_PreDec: _LL363: _LL364:
# 2695
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpB55;struct Cyc_Core_Opt*_tmpB54;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpB53;e->r=(void*)((_tmpB53=_cycalloc(sizeof(*_tmpB53)),((_tmpB53[0]=((_tmpB55.tag=3,((_tmpB55.f1=_tmp4BA,((_tmpB55.f2=((_tmpB54=_cycalloc_atomic(sizeof(*_tmpB54)),((_tmpB54->v=(void*)Cyc_Absyn_Minus,_tmpB54)))),((_tmpB55.f3=_tmp4F1,_tmpB55)))))))),_tmpB53))));}
Cyc_Toc_exp_to_c(nv,e);
return;default: _LL365: _LL366:
# 2699
{const char*_tmpB59;void*_tmpB58[1];struct Cyc_String_pa_PrintArg_struct _tmpB57;(_tmpB57.tag=0,((_tmpB57.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpB58[0]=& _tmpB57,Cyc_Tcutil_terr(e->loc,((_tmpB59="in-place post-%s is not supported on @tagged union members",_tag_dyneither(_tmpB59,sizeof(char),59))),_tag_dyneither(_tmpB58,sizeof(void*),1)))))));}{
# 2701
const char*_tmpB5C;void*_tmpB5B;(_tmpB5B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB5C="in-place inc/dec on @tagged union",_tag_dyneither(_tmpB5C,sizeof(char),34))),_tag_dyneither(_tmpB5B,sizeof(void*),0)));};}_LL360:;};}
# 2704
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4BA);
Cyc_Toc_set_lhs(nv,0);{
# 2709
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp4BB == Cyc_Absyn_PostInc  || _tmp4BB == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2715
if(_tmp4BB == Cyc_Absyn_PreDec  || _tmp4BB == Cyc_Absyn_PostDec)
change=-1;{
struct Cyc_Absyn_Exp*_tmpB5D[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpB5D[2]=
# 2719
Cyc_Absyn_signed_int_exp(change,0),((_tmpB5D[1]=
# 2718
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpB5D[0]=
# 2717
Cyc_Toc_push_address_exp(_tmp4BA),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB5D,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2720
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2724
struct Cyc_Toc_functionSet*_tmp4FF=_tmp4BB != Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
# 2726
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp4FF,_tmp4BA);
struct Cyc_Absyn_Exp*_tmpB5E[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpB5E[1]=
Cyc_Absyn_signed_int_exp(1,0),((_tmpB5E[0]=
# 2727
Cyc_Toc_push_address_exp(_tmp4BA),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB5E,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
# 2729
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp4BA)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp4BA,0,Cyc_Toc_incr_lvalue,_tmp4BB);
e->r=_tmp4BA->r;}}}
# 2733
goto _LL2DB;};};}case 3: _LL2E6: _tmp4B7=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp4B8=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_tmp4B9=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp46E)->f3;_LL2E7: {
# 2752 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp4B7->topt);
void*e2_old_typ=(void*)_check_null(_tmp4B9->topt);
int f_tag=0;
void*tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Toc_is_tagged_union_project(_tmp4B7,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4B7);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4B9);
e->r=Cyc_Toc_tagged_union_assignop(_tmp4B7,e1_old_typ,_tmp4B8,_tmp4B9,e2_old_typ,f_tag,tagged_member_struct_type);
# 2763
return;}{
# 2765
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4B7,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp4B7,_tmp4B8,_tmp4B9,ptr_type,is_dyneither,elt_type);
# 2771
return;}{
# 2775
int e1_poly=Cyc_Toc_is_poly_project(_tmp4B7);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4B7);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4B9);{
# 2781
int done=0;
if(_tmp4B8 != 0){
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp502=(enum Cyc_Absyn_Primop)_tmp4B8->v;enum Cyc_Absyn_Primop _tmp503=_tmp502;switch(_tmp503){case Cyc_Absyn_Plus: _LL368: _LL369:
 change=_tmp4B9;goto _LL367;case Cyc_Absyn_Minus: _LL36A: _LL36B:
# 2789
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp4B9,0);goto _LL367;default: _LL36C: _LL36D: {
const char*_tmpB61;void*_tmpB60;(_tmpB60=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB61="bad t ? pointer arithmetic",_tag_dyneither(_tmpB61,sizeof(char),27))),_tag_dyneither(_tmpB60,sizeof(void*),0)));}}_LL367:;}
# 2792
done=1;{
# 2794
struct Cyc_Absyn_Exp*_tmp506=Cyc_Toc__dyneither_ptr_inplace_plus_e;
struct Cyc_Absyn_Exp*_tmpB62[3];e->r=Cyc_Toc_fncall_exp_r(_tmp506,((_tmpB62[2]=change,((_tmpB62[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpB62[0]=
# 2795
Cyc_Toc_push_address_exp(_tmp4B7),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB62,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2798
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2801
enum Cyc_Absyn_Primop _tmp508=(enum Cyc_Absyn_Primop)_tmp4B8->v;enum Cyc_Absyn_Primop _tmp509=_tmp508;if(_tmp509 == Cyc_Absyn_Plus){_LL36F: _LL370:
# 2803
 done=1;
{struct Cyc_Absyn_Exp*_tmpB63[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp4B7),((_tmpB63[1]=_tmp4B9,((_tmpB63[0]=_tmp4B7,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB63,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL36E;}else{_LL371: _LL372: {
const char*_tmpB66;void*_tmpB65;(_tmpB65=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB66="bad zero-terminated pointer arithmetic",_tag_dyneither(_tmpB66,sizeof(char),39))),_tag_dyneither(_tmpB65,sizeof(void*),0)));}}_LL36E:;}}}
# 2810
if(!done){
# 2812
if(e1_poly)
_tmp4B9->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp4B9->r,0));
# 2818
if(!Cyc_Absyn_is_lvalue(_tmp4B7)){
{struct _tuple27 _tmpB69;struct _tuple27*_tmpB68;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple27*),struct _tuple27*f_env))Cyc_Toc_lvalue_assign)(_tmp4B7,0,Cyc_Toc_assignop_lvalue,((_tmpB68=_cycalloc(sizeof(struct _tuple27)* 1),((_tmpB68[0]=(struct _tuple27)((_tmpB69.f1=_tmp4B8,((_tmpB69.f2=_tmp4B9,_tmpB69)))),_tmpB68)))));}
e->r=_tmp4B7->r;}}
# 2823
goto _LL2DB;};};};}case 5: _LL2E8: _tmp4B4=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp4B5=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_tmp4B6=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp46E)->f3;_LL2E9:
# 2825
 Cyc_Toc_exp_to_c(nv,_tmp4B4);
Cyc_Toc_exp_to_c(nv,_tmp4B5);
Cyc_Toc_exp_to_c(nv,_tmp4B6);
goto _LL2DB;case 6: _LL2EA: _tmp4B2=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp4B3=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL2EB:
# 2830
 Cyc_Toc_exp_to_c(nv,_tmp4B2);
Cyc_Toc_exp_to_c(nv,_tmp4B3);
goto _LL2DB;case 7: _LL2EC: _tmp4B0=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp4B1=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL2ED:
# 2834
 Cyc_Toc_exp_to_c(nv,_tmp4B0);
Cyc_Toc_exp_to_c(nv,_tmp4B1);
goto _LL2DB;case 8: _LL2EE: _tmp4AE=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp4AF=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL2EF:
# 2838
 Cyc_Toc_exp_to_c(nv,_tmp4AE);
Cyc_Toc_exp_to_c(nv,_tmp4AF);
goto _LL2DB;case 9: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46E)->f3 == 0){_LL2F0: _tmp4AC=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp4AD=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL2F1:
# 2842
 Cyc_Toc_exp_to_c(nv,_tmp4AC);
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4AD);
goto _LL2DB;}else{_LL2F2: _tmp4A7=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp4A8=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_tmp4A9=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46E)->f3)->num_varargs;_tmp4AA=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46E)->f3)->injectors;_tmp4AB=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46E)->f3)->vai;_LL2F3: {
# 2854 "toc.cyc"
struct _RegionHandle _tmp50F=_new_region("r");struct _RegionHandle*r=& _tmp50F;_push_region(r);{
struct _tuple1*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp4A9,0);
void*cva_type=Cyc_Toc_typ_to_c(_tmp4AB->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_conref,0);
# 2862
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4A8);
int num_normargs=num_args - _tmp4A9;
# 2866
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp4A8=_tmp4A8->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4A8))->hd);{
struct Cyc_List_List*_tmpB6A;new_args=((_tmpB6A=_cycalloc(sizeof(*_tmpB6A)),((_tmpB6A->hd=(struct Cyc_Absyn_Exp*)_tmp4A8->hd,((_tmpB6A->tl=new_args,_tmpB6A))))));};}}
# 2871
{struct Cyc_Absyn_Exp*_tmpB6D[3];struct Cyc_List_List*_tmpB6C;new_args=((_tmpB6C=_cycalloc(sizeof(*_tmpB6C)),((_tmpB6C->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpB6D[2]=num_varargs_exp,((_tmpB6D[1]=
Cyc_Absyn_sizeoftyp_exp(cva_type,0),((_tmpB6D[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB6D,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpB6C->tl=new_args,_tmpB6C))))));}
# 2876
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);
# 2878
Cyc_Toc_exp_to_c(nv,_tmp4A7);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4A7,new_args,0),0);
# 2882
if(_tmp4AB->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp513=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp4AB->type));void*_tmp514=_tmp513;struct Cyc_Absyn_Datatypedecl*_tmp515;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp514)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp514)->f1).datatype_info).KnownDatatype).tag == 2){_LL374: _tmp515=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp514)->f1).datatype_info).KnownDatatype).val;_LL375:
 tud=_tmp515;goto _LL373;}else{goto _LL376;}}else{_LL376: _LL377: {
const char*_tmpB70;void*_tmpB6F;(_tmpB6F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB70="toc: unknown datatype in vararg with inject",_tag_dyneither(_tmpB70,sizeof(char),44))),_tag_dyneither(_tmpB6F,sizeof(void*),0)));}}_LL373:;}{
# 2888
unsigned int _tmpB75;unsigned int _tmpB74;struct _dyneither_ptr _tmpB73;struct _tuple1**_tmpB72;unsigned int _tmpB71;struct _dyneither_ptr vs=(_tmpB71=(unsigned int)_tmp4A9,((_tmpB72=(struct _tuple1**)_region_malloc(r,_check_times(sizeof(struct _tuple1*),_tmpB71)),((_tmpB73=_tag_dyneither(_tmpB72,sizeof(struct _tuple1*),_tmpB71),((((_tmpB74=_tmpB71,_tmpB76(& _tmpB75,& _tmpB74,& _tmpB72))),_tmpB73)))))));
# 2890
if(_tmp4A9 != 0){
# 2892
struct Cyc_List_List*_tmp518=0;
{int i=_tmp4A9 - 1;for(0;i >= 0;-- i){
struct Cyc_List_List*_tmpB77;_tmp518=((_tmpB77=_cycalloc(sizeof(*_tmpB77)),((_tmpB77->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0),0)),((_tmpB77->tl=_tmp518,_tmpB77))))));}}
# 2896
s=Cyc_Absyn_declare_stmt(argv,arr_type,Cyc_Absyn_unresolvedmem_exp(0,_tmp518,0),s,0);{
# 2899
int i=0;for(0;_tmp4A8 != 0;(((_tmp4A8=_tmp4A8->tl,_tmp4AA=_tmp4AA->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp4A8->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0);
struct Cyc_Absyn_Datatypefield*_tmp51A=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp4AA))->hd;struct Cyc_Absyn_Datatypefield*_tmp51B=_tmp51A;struct _tuple1*_tmp51C;struct Cyc_List_List*_tmp51D;_LL379: _tmp51C=_tmp51B->name;_tmp51D=_tmp51B->typs;_LL37A: {
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp51D))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);
# 2914
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);
# 2917
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Toc_tag_sp,0),
Cyc_Toc_datatype_tag(tud,_tmp51C),0),s,0);
# 2920
s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp51C,tud->name)),0,s,0);}_LL378:;}};}else{
# 2927
struct _tuple19*_tmpB78[3];struct Cyc_List_List*_tmp51E=(_tmpB78[2]=
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpB78[1]=
# 2927
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpB78[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB78,sizeof(struct _tuple19*),3)))))));
# 2929
s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),Cyc_Absyn_uint_exp(0,0),s,0);}};}else{
# 2939
{int i=0;for(0;_tmp4A8 != 0;(_tmp4A8=_tmp4A8->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4A8->hd);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp4A8->hd,0),s,0);}}
# 2945
s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}
# 2958 "toc.cyc"
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2960
_npop_handler(0);goto _LL2DB;
# 2854 "toc.cyc"
;_pop_region(r);}}case 10: _LL2F4: _tmp4A5=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp4A6=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL2F5:
# 2963 "toc.cyc"
 Cyc_Toc_exp_to_c(nv,_tmp4A5);{
struct Cyc_Absyn_Exp*_tmp524=_tmp4A6?Cyc_Toc_newrethrow_exp(_tmp4A5): Cyc_Toc_newthrow_exp(_tmp4A5);
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),_tmp524,0))->r;
goto _LL2DB;};case 11: _LL2F6: _tmp4A4=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_LL2F7:
 Cyc_Toc_exp_to_c(nv,_tmp4A4);goto _LL2DB;case 12: _LL2F8: _tmp4A2=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp4A3=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL2F9:
# 2969
 Cyc_Toc_exp_to_c(nv,_tmp4A2);
# 2978 "toc.cyc"
for(0;_tmp4A3 != 0;_tmp4A3=_tmp4A3->tl){
enum Cyc_Absyn_KindQual _tmp525=(Cyc_Tcutil_typ_kind((void*)_tmp4A3->hd))->kind;
if(_tmp525 != Cyc_Absyn_EffKind  && _tmp525 != Cyc_Absyn_RgnKind){
{void*_tmp526=Cyc_Tcutil_compress((void*)_tmp4A3->hd);void*_tmp527=_tmp526;switch(*((int*)_tmp527)){case 2: _LL37E: _LL37F:
 goto _LL381;case 3: _LL380: _LL381:
 continue;default: _LL382: _LL383:
# 2985
 e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp4A2,0))->r;
goto _LL37D;}_LL37D:;}
# 2988
break;}}
# 2991
goto _LL2DB;case 13: _LL2FA: _tmp49E=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp49F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_tmp4A0=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp46E)->f3;_tmp4A1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp46E)->f4;_LL2FB: {
# 2993
void*old_t2=(void*)_check_null(_tmp49F->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp49E;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp49E=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp49F);
# 3000
{struct _tuple29 _tmpB79;struct _tuple29 _tmp528=(_tmpB79.f1=Cyc_Tcutil_compress(old_t2),((_tmpB79.f2=Cyc_Tcutil_compress(new_typ),_tmpB79)));struct _tuple29 _tmp529=_tmp528;struct Cyc_Absyn_PtrInfo _tmp52A;struct Cyc_Absyn_PtrInfo _tmp52B;struct Cyc_Absyn_PtrInfo _tmp52C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp529.f1)->tag == 5)switch(*((int*)_tmp529.f2)){case 5: _LL385: _tmp52B=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp529.f1)->f1;_tmp52C=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp529.f2)->f1;_LL386: {
# 3002
int _tmp52D=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp52B.ptr_atts).nullable);
int _tmp52E=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp52C.ptr_atts).nullable);
void*_tmp52F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp52B.ptr_atts).bounds);
void*_tmp530=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp52C.ptr_atts).bounds);
int _tmp531=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp52B.ptr_atts).zero_term);
int _tmp532=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp52C.ptr_atts).zero_term);
{struct _tuple29 _tmpB7A;struct _tuple29 _tmp533=(_tmpB7A.f1=_tmp52F,((_tmpB7A.f2=_tmp530,_tmpB7A)));struct _tuple29 _tmp534=_tmp533;struct Cyc_Absyn_Exp*_tmp535;struct Cyc_Absyn_Exp*_tmp536;struct Cyc_Absyn_Exp*_tmp537;struct Cyc_Absyn_Exp*_tmp538;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp534.f1)->tag == 1){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp534.f2)->tag == 1){_LL38C: _tmp537=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp534.f1)->f1;_tmp538=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp534.f2)->f1;_LL38D:
# 3010
 if((!Cyc_Evexp_c_can_eval(_tmp537) || !Cyc_Evexp_c_can_eval(_tmp538)) && !
Cyc_Evexp_same_const_exp(_tmp537,_tmp538)){
const char*_tmpB7D;void*_tmpB7C;(_tmpB7C=0,Cyc_Tcutil_terr(e->loc,((_tmpB7D="can't validate cast due to potential size differences",_tag_dyneither(_tmpB7D,sizeof(char),54))),_tag_dyneither(_tmpB7C,sizeof(void*),0)));}
if(_tmp52D  && !_tmp52E){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpB80;void*_tmpB7F;(_tmpB7F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB80="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpB80,sizeof(char),44))),_tag_dyneither(_tmpB7F,sizeof(void*),0)));}
# 3019
if(_tmp4A1 != Cyc_Absyn_NonNull_to_Null){
const char*_tmpB84;void*_tmpB83[1];struct Cyc_String_pa_PrintArg_struct _tmpB82;(_tmpB82.tag=0,((_tmpB82.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpB83[0]=& _tmpB82,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpB84="null-check conversion mis-classified: %s",_tag_dyneither(_tmpB84,sizeof(char),41))),_tag_dyneither(_tmpB83,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp49F);
if(do_null_check){
if(!_tmp4A0){
const char*_tmpB87;void*_tmpB86;(_tmpB86=0,Cyc_Tcutil_warn(e->loc,((_tmpB87="inserted null check due to implicit cast from * to @ type",_tag_dyneither(_tmpB87,sizeof(char),58))),_tag_dyneither(_tmpB86,sizeof(void*),0)));}{
# 3026
struct Cyc_List_List*_tmpB88;e->r=Cyc_Toc_cast_it_r(*_tmp49E,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpB88=_cycalloc(sizeof(*_tmpB88)),((_tmpB88->hd=_tmp49F,((_tmpB88->tl=0,_tmpB88)))))),0));};}else{
# 3030
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp49F->r;}};}else{
# 3035
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp49F->r;}
# 3042
goto _LL38B;}else{_LL38E: _tmp536=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp534.f1)->f1;_LL38F:
# 3044
 if(!Cyc_Evexp_c_can_eval(_tmp536)){
const char*_tmpB8B;void*_tmpB8A;(_tmpB8A=0,Cyc_Tcutil_terr(e->loc,((_tmpB8B="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpB8B,sizeof(char),71))),_tag_dyneither(_tmpB8A,sizeof(void*),0)));}
# 3047
if(_tmp4A1 == Cyc_Absyn_NonNull_to_Null){
const char*_tmpB8F;void*_tmpB8E[1];struct Cyc_String_pa_PrintArg_struct _tmpB8D;(_tmpB8D.tag=0,((_tmpB8D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpB8E[0]=& _tmpB8D,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpB8F="conversion mis-classified as null-check: %s",_tag_dyneither(_tmpB8F,sizeof(char),44))),_tag_dyneither(_tmpB8E,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){
# 3051
if((_tmp531  && !(_tmp52C.elt_tq).real_const) && !_tmp532)
# 3054
_tmp536=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp536,Cyc_Absyn_uint_exp(1,0),0);
# 3056
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp536,_tmp49F))->r;}else{
# 3058
struct Cyc_Absyn_Exp*_tmp548=Cyc_Toc__tag_dyneither_e;
# 3060
if(_tmp531){
# 3065
struct _tuple1*_tmp549=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp54A=Cyc_Absyn_var_exp(_tmp549,0);
struct Cyc_Absyn_Exp*arg3;
# 3070
{void*_tmp54B=_tmp49F->r;void*_tmp54C=_tmp54B;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp54C)->tag == 0)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp54C)->f1).Wstring_c).tag){case 8: _LL395: _LL396:
# 3072
 arg3=_tmp536;goto _LL394;case 9: _LL397: _LL398:
# 3074
 arg3=_tmp536;goto _LL394;default: goto _LL399;}else{_LL399: _LL39A:
# 3076
{struct Cyc_Absyn_Exp*_tmpB90[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp49F),(
# 3078
(_tmpB90[1]=_tmp536,((_tmpB90[0]=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp54A),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB90,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
goto _LL394;}_LL394:;}
# 3081
if(!_tmp532  && !(_tmp52C.elt_tq).real_const)
# 3084
arg3=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{
# 3086
struct Cyc_Absyn_Exp*_tmp54E=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp52C.elt_typ),0);
struct Cyc_Absyn_Exp*_tmpB91[3];struct Cyc_Absyn_Exp*_tmp54F=Cyc_Absyn_fncall_exp(_tmp548,((_tmpB91[2]=arg3,((_tmpB91[1]=_tmp54E,((_tmpB91[0]=_tmp54A,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB91,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
struct Cyc_Absyn_Stmt*_tmp550=Cyc_Absyn_exp_stmt(_tmp54F,0);
_tmp550=Cyc_Absyn_declare_stmt(_tmp549,Cyc_Toc_typ_to_c(old_t2),_tmp49F,_tmp550,0);
e->r=Cyc_Toc_stmt_exp_r(_tmp550);};}else{
# 3093
struct Cyc_Absyn_Exp*_tmpB92[3];e->r=Cyc_Toc_fncall_exp_r(_tmp548,(
(_tmpB92[2]=_tmp536,((_tmpB92[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp52C.elt_typ),0),((_tmpB92[0]=_tmp49F,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB92,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 3099
goto _LL38B;}}else{if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp534.f2)->tag == 1){_LL390: _tmp535=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp534.f2)->f1;_LL391:
# 3101
 if(!Cyc_Evexp_c_can_eval(_tmp535)){
const char*_tmpB95;void*_tmpB94;(_tmpB94=0,Cyc_Tcutil_terr(e->loc,((_tmpB95="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpB95,sizeof(char),71))),_tag_dyneither(_tmpB94,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpB98;void*_tmpB97;(_tmpB97=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB98="can't coerce t? to t* or t@ at the top-level",_tag_dyneither(_tmpB98,sizeof(char),45))),_tag_dyneither(_tmpB97,sizeof(void*),0)));}{
# 3113 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp557=_tmp535;
if(_tmp531  && !_tmp532)
_tmp557=Cyc_Absyn_add_exp(_tmp535,Cyc_Absyn_uint_exp(1,0),0);{
# 3120
struct Cyc_Absyn_Exp*_tmp558=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpB99[3];struct Cyc_Absyn_Exp*_tmp559=Cyc_Absyn_fncall_exp(_tmp558,(
(_tmpB99[2]=_tmp557,((_tmpB99[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp52B.elt_typ),0),((_tmpB99[0]=_tmp49F,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB99,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 3125
if(_tmp52E){
struct Cyc_List_List*_tmpB9A;_tmp559->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,(
(_tmpB9A=_cycalloc(sizeof(*_tmpB9A)),((_tmpB9A->hd=Cyc_Absyn_copy_exp(_tmp559),((_tmpB9A->tl=0,_tmpB9A)))))));}
e->r=Cyc_Toc_cast_it_r(*_tmp49E,_tmp559);
goto _LL38B;};};}else{_LL392: _LL393:
# 3133
 DynCast:
 if((_tmp531  && !_tmp532) && !(_tmp52C.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpB9D;void*_tmpB9C;(_tmpB9C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB9D="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",_tag_dyneither(_tmpB9D,sizeof(char),70))),_tag_dyneither(_tmpB9C,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp55E=Cyc_Toc__dyneither_ptr_decrease_size_e;
struct Cyc_Absyn_Exp*_tmpB9E[3];e->r=Cyc_Toc_fncall_exp_r(_tmp55E,(
(_tmpB9E[2]=
Cyc_Absyn_uint_exp(1,0),((_tmpB9E[1]=
# 3139
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp52B.elt_typ),0),((_tmpB9E[0]=_tmp49F,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB9E,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}
# 3142
goto _LL38B;}}_LL38B:;}
# 3144
goto _LL384;}case 6: _LL387: _tmp52A=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp529.f1)->f1;_LL388:
# 3146
{void*_tmp561=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp52A.ptr_atts).bounds);void*_tmp562=_tmp561;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp562)->tag == 0){_LL39C: _LL39D:
# 3148
 _tmp49F->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp49F->r,_tmp49F->loc),Cyc_Toc_curr_sp);goto _LL39B;}else{_LL39E: _LL39F:
 goto _LL39B;}_LL39B:;}
# 3151
goto _LL384;default: goto _LL389;}else{_LL389: _LL38A:
# 3153
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp49F->r;
goto _LL384;}_LL384:;}
# 3157
goto _LL2DB;}case 14: _LL2FC: _tmp49D=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_LL2FD:
# 3161
{void*_tmp564=_tmp49D->r;void*_tmp565=_tmp564;struct Cyc_List_List*_tmp566;struct _tuple1*_tmp567;struct Cyc_List_List*_tmp568;struct Cyc_List_List*_tmp569;switch(*((int*)_tmp565)){case 28: _LL3A1: _tmp567=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp565)->f1;_tmp568=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp565)->f2;_tmp569=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp565)->f3;_LL3A2:
# 3163
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpBA2;void*_tmpBA1[1];struct Cyc_String_pa_PrintArg_struct _tmpBA0;(_tmpBA0.tag=0,((_tmpBA0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp49D->loc)),((_tmpBA1[0]=& _tmpBA0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBA2="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpBA2,sizeof(char),42))),_tag_dyneither(_tmpBA1,sizeof(void*),1)))))));}{
struct Cyc_Absyn_Exp*_tmp56D=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp49D->topt),_tmp568,1,0,_tmp569,_tmp567);
e->r=_tmp56D->r;
e->topt=_tmp56D->topt;
goto _LL3A0;};case 23: _LL3A3: _tmp566=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp565)->f1;_LL3A4:
# 3171
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpBA6;void*_tmpBA5[1];struct Cyc_String_pa_PrintArg_struct _tmpBA4;(_tmpBA4.tag=0,((_tmpBA4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp49D->loc)),((_tmpBA5[0]=& _tmpBA4,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBA6="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpBA6,sizeof(char),42))),_tag_dyneither(_tmpBA5,sizeof(void*),1)))))));}
e->r=(Cyc_Toc_init_tuple(nv,1,0,_tmp566))->r;
goto _LL3A0;default: _LL3A5: _LL3A6:
# 3177
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp49D);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp49D)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp49D,0,Cyc_Toc_address_lvalue,1);
# 3183
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp49D);}else{
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind((void*)_check_null(_tmp49D->topt))))
# 3187
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp49D);}
# 3189
goto _LL3A0;}_LL3A0:;}
# 3191
goto _LL2DB;case 15: _LL2FE: _tmp49B=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp49C=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL2FF:
# 3194
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpBAA;void*_tmpBA9[1];struct Cyc_String_pa_PrintArg_struct _tmpBA8;(_tmpBA8.tag=0,((_tmpBA8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp49C->loc)),((_tmpBA9[0]=& _tmpBA8,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBAA="%s: new at top-level",_tag_dyneither(_tmpBAA,sizeof(char),21))),_tag_dyneither(_tmpBA9,sizeof(void*),1)))))));}{
void*new_e_type=(void*)_check_null(_tmp49C->topt);
{void*_tmp574=_tmp49C->r;void*_tmp575=_tmp574;struct Cyc_List_List*_tmp576;struct _tuple1*_tmp577;struct Cyc_List_List*_tmp578;struct Cyc_List_List*_tmp579;struct Cyc_Absyn_Aggrdecl*_tmp57A;struct Cyc_Absyn_Exp*_tmp57B;void*_tmp57C;int _tmp57D;struct Cyc_Absyn_Vardecl*_tmp57E;struct Cyc_Absyn_Exp*_tmp57F;struct Cyc_Absyn_Exp*_tmp580;int _tmp581;struct Cyc_List_List*_tmp582;switch(*((int*)_tmp575)){case 25: _LL3A8: _tmp582=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp575)->f1;_LL3A9: {
# 3202
struct _tuple1*_tmp583=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp584=Cyc_Absyn_var_exp(_tmp583,0);
struct Cyc_Absyn_Stmt*_tmp585=Cyc_Toc_init_array(nv,new_e_type,_tmp584,_tmp582,Cyc_Absyn_exp_stmt(_tmp584,0));
void*old_elt_typ;
{void*_tmp586=Cyc_Tcutil_compress(old_typ);void*_tmp587=_tmp586;void*_tmp588;struct Cyc_Absyn_Tqual _tmp589;union Cyc_Absyn_Constraint*_tmp58A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp587)->tag == 5){_LL3B5: _tmp588=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp587)->f1).elt_typ;_tmp589=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp587)->f1).elt_tq;_tmp58A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp587)->f1).ptr_atts).zero_term;_LL3B6:
# 3208
 old_elt_typ=_tmp588;goto _LL3B4;}else{_LL3B7: _LL3B8: {
# 3210
const char*_tmpBAD;void*_tmpBAC;old_elt_typ=(
(_tmpBAC=0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBAD="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(_tmpBAD,sizeof(char),52))),_tag_dyneither(_tmpBAC,sizeof(void*),0))));}}_LL3B4:;}{
# 3213
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*_tmp58D=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp58E=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),
Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp582),0),0);
struct Cyc_Absyn_Exp*e1;
if(_tmp49B == 0  || Cyc_Absyn_no_regions)
e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp58E);else{
# 3221
struct Cyc_Absyn_Exp*r=_tmp49B;
Cyc_Toc_exp_to_c(nv,r);
e1=Cyc_Toc_rmalloc_exp(r,_tmp58E);}
# 3226
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp583,_tmp58D,e1,_tmp585,0));
goto _LL3A7;};}case 26: _LL3AA: _tmp57E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp575)->f1;_tmp57F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp575)->f2;_tmp580=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp575)->f3;_tmp581=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp575)->f4;_LL3AB:
# 3230
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp49B,old_typ,_tmp57F,(void*)_check_null(_tmp580->topt),_tmp581,_tmp580,_tmp57E);
goto _LL3A7;case 27: _LL3AC: _tmp57B=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp575)->f1;_tmp57C=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp575)->f2;_tmp57D=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp575)->f3;_LL3AD:
# 3234
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp49B,old_typ,_tmp57B,_tmp57C,_tmp57D,0,0);
goto _LL3A7;case 28: _LL3AE: _tmp577=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp575)->f1;_tmp578=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp575)->f2;_tmp579=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp575)->f3;_tmp57A=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp575)->f4;_LL3AF: {
# 3239
struct Cyc_Absyn_Exp*_tmp58F=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp49C->topt),_tmp578,1,_tmp49B,_tmp579,_tmp577);
e->r=_tmp58F->r;
e->topt=_tmp58F->topt;
goto _LL3A7;}case 23: _LL3B0: _tmp576=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp575)->f1;_LL3B1:
# 3245
 e->r=(Cyc_Toc_init_tuple(nv,1,_tmp49B,_tmp576))->r;
goto _LL3A7;default: _LL3B2: _LL3B3: {
# 3252
void*old_elt_typ=(void*)_check_null(_tmp49C->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
# 3255
struct _tuple1*_tmp590=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp591=Cyc_Absyn_var_exp(_tmp590,0);
struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp591,0),0);
struct Cyc_Absyn_Exp*inner_mexp=mexp;
if(_tmp49B == 0  || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
# 3262
struct Cyc_Absyn_Exp*r=_tmp49B;
Cyc_Toc_exp_to_c(nv,r);
mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{
# 3270
int done=0;
{void*_tmp592=_tmp49C->r;void*_tmp593=_tmp592;void*_tmp594;struct Cyc_Absyn_Exp*_tmp595;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp593)->tag == 13){_LL3BA: _tmp594=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp593)->f1;_tmp595=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp593)->f2;_LL3BB:
# 3273
{struct _tuple29 _tmpBAE;struct _tuple29 _tmp596=(_tmpBAE.f1=Cyc_Tcutil_compress(_tmp594),((_tmpBAE.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp595->topt)),_tmpBAE)));struct _tuple29 _tmp597=_tmp596;void*_tmp598;union Cyc_Absyn_Constraint*_tmp599;union Cyc_Absyn_Constraint*_tmp59A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp597.f1)->tag == 5){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp597.f2)->tag == 5){_LL3BF: _tmp598=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp597.f1)->f1).elt_typ;_tmp599=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp597.f1)->f1).ptr_atts).bounds;_tmp59A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp597.f2)->f1).ptr_atts).bounds;_LL3C0:
# 3276
{struct _tuple29 _tmpBAF;struct _tuple29 _tmp59B=(_tmpBAF.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp599),((_tmpBAF.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp59A),_tmpBAF)));struct _tuple29 _tmp59C=_tmp59B;struct Cyc_Absyn_Exp*_tmp59D;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp59C.f1)->tag == 0){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp59C.f2)->tag == 1){_LL3C4: _tmp59D=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp59C.f2)->f1;_LL3C5:
# 3278
 Cyc_Toc_exp_to_c(nv,_tmp595);
inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);
done=1;{
struct Cyc_Absyn_Exp*_tmp59E=Cyc_Toc__init_dyneither_ptr_e;
{struct Cyc_Absyn_Exp*_tmpBB0[4];e->r=Cyc_Toc_fncall_exp_r(_tmp59E,(
(_tmpBB0[3]=_tmp59D,((_tmpBB0[2]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp598),0),((_tmpBB0[1]=_tmp595,((_tmpBB0[0]=mexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBB0,sizeof(struct Cyc_Absyn_Exp*),4)))))))))));}
# 3286
goto _LL3C3;};}else{goto _LL3C6;}}else{_LL3C6: _LL3C7:
 goto _LL3C3;}_LL3C3:;}
# 3289
goto _LL3BE;}else{goto _LL3C1;}}else{_LL3C1: _LL3C2:
 goto _LL3BE;}_LL3BE:;}
# 3292
goto _LL3B9;}else{_LL3BC: _LL3BD:
 goto _LL3B9;}_LL3B9:;}
# 3295
if(!done){
struct Cyc_Absyn_Stmt*_tmp5A2=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp591),0);
struct Cyc_Absyn_Exp*_tmp5A3=Cyc_Absyn_signed_int_exp(0,0);
Cyc_Toc_exp_to_c(nv,_tmp49C);
_tmp5A2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp591,_tmp5A3,0),_tmp49C,0),_tmp5A2,0);{
# 3301
void*_tmp5A4=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp590,_tmp5A4,mexp,_tmp5A2,0));};}
# 3304
goto _LL3A7;};}}_LL3A7:;}
# 3306
goto _LL2DB;};case 17: _LL300: _tmp49A=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_LL301: {
# 3309
int _tmp5A5=Cyc_Toc_set_in_sizeof(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp49A);
Cyc_Toc_set_in_sizeof(nv,_tmp5A5);
goto _LL2DB;}case 16: _LL302: _tmp499=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_LL303:
{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpBB3;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpBB2;e->r=(void*)((_tmpBB2=_cycalloc(sizeof(*_tmpBB2)),((_tmpBB2[0]=((_tmpBB3.tag=16,((_tmpBB3.f1=Cyc_Toc_typ_to_c(_tmp499),_tmpBB3)))),_tmpBB2))));}goto _LL2DB;case 18: _LL304: _tmp497=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp498=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL305: {
# 3315
void*_tmp5A8=_tmp497;
struct Cyc_List_List*_tmp5A9=_tmp498;
for(0;_tmp5A9 != 0;_tmp5A9=_tmp5A9->tl){
void*_tmp5AA=(void*)_tmp5A9->hd;void*_tmp5AB=_tmp5AA;unsigned int _tmp5AC;struct _dyneither_ptr*_tmp5AD;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp5AB)->tag == 0){_LL3C9: _tmp5AD=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp5AB)->f1;_LL3CA:
 goto _LL3C8;}else{_LL3CB: _tmp5AC=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp5AB)->f1;_LL3CC:
# 3321
{void*_tmp5AE=Cyc_Tcutil_compress(_tmp5A8);void*_tmp5AF=_tmp5AE;struct Cyc_Absyn_Datatypefield*_tmp5B0;struct Cyc_List_List*_tmp5B1;struct Cyc_List_List*_tmp5B2;union Cyc_Absyn_AggrInfoU _tmp5B3;switch(*((int*)_tmp5AF)){case 11: _LL3CE: _tmp5B3=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5AF)->f1).aggr_info;_LL3CF: {
# 3323
struct Cyc_Absyn_Aggrdecl*_tmp5B4=Cyc_Absyn_get_known_aggrdecl(_tmp5B3);
if(_tmp5B4->impl == 0){
const char*_tmpBB6;void*_tmpBB5;(_tmpBB5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBB6="struct fields must be known",_tag_dyneither(_tmpBB6,sizeof(char),28))),_tag_dyneither(_tmpBB5,sizeof(void*),0)));}
_tmp5B2=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp5B4->impl))->fields;goto _LL3D1;}case 12: _LL3D0: _tmp5B2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5AF)->f2;_LL3D1: {
# 3328
struct Cyc_Absyn_Aggrfield*_tmp5B7=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp5B2,(int)_tmp5AC);
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpBB9;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpBB8;_tmp5A9->hd=(void*)((void*)((_tmpBB8=_cycalloc(sizeof(*_tmpBB8)),((_tmpBB8[0]=((_tmpBB9.tag=0,((_tmpBB9.f1=_tmp5B7->name,_tmpBB9)))),_tmpBB8)))));}
_tmp5A8=_tmp5B7->type;
goto _LL3CD;}case 10: _LL3D2: _tmp5B1=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5AF)->f1;_LL3D3:
# 3333
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpBBC;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpBBB;_tmp5A9->hd=(void*)((void*)((_tmpBBB=_cycalloc(sizeof(*_tmpBBB)),((_tmpBBB[0]=((_tmpBBC.tag=0,((_tmpBBC.f1=Cyc_Absyn_fieldname((int)(_tmp5AC + 1)),_tmpBBC)))),_tmpBBB)))));}
_tmp5A8=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp5B1,(int)_tmp5AC)).f2;
goto _LL3CD;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp5AF)->f1).field_info).KnownDatatypefield).tag == 2){_LL3D4: _tmp5B0=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp5AF)->f1).field_info).KnownDatatypefield).val).f2;_LL3D5:
# 3337
 if(_tmp5AC == 0){
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpBBF;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpBBE;_tmp5A9->hd=(void*)((void*)((_tmpBBE=_cycalloc(sizeof(*_tmpBBE)),((_tmpBBE[0]=((_tmpBBF.tag=0,((_tmpBBF.f1=Cyc_Toc_tag_sp,_tmpBBF)))),_tmpBBE)))));}else{
# 3340
_tmp5A8=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp5B0->typs,(int)(_tmp5AC - 1))).f2;{
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpBC2;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpBC1;_tmp5A9->hd=(void*)((void*)((_tmpBC1=_cycalloc(sizeof(*_tmpBC1)),((_tmpBC1[0]=((_tmpBC2.tag=0,((_tmpBC2.f1=Cyc_Absyn_fieldname((int)_tmp5AC),_tmpBC2)))),_tmpBC1)))));};}
# 3343
goto _LL3CD;}else{goto _LL3D6;}default: _LL3D6: _LL3D7:
{const char*_tmpBC5;void*_tmpBC4;(_tmpBC4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBC5="impossible type for offsetof tuple index",_tag_dyneither(_tmpBC5,sizeof(char),41))),_tag_dyneither(_tmpBC4,sizeof(void*),0)));}
goto _LL3CD;}_LL3CD:;}
# 3347
goto _LL3C8;}_LL3C8:;}
# 3350
{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpBC8;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpBC7;e->r=(void*)((_tmpBC7=_cycalloc(sizeof(*_tmpBC7)),((_tmpBC7[0]=((_tmpBC8.tag=18,((_tmpBC8.f1=Cyc_Toc_typ_to_c(_tmp497),((_tmpBC8.f2=_tmp498,_tmpBC8)))))),_tmpBC7))));}
goto _LL2DB;}case 19: _LL306: _tmp496=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_LL307: {
# 3353
int _tmp5C4=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp5C5=Cyc_Tcutil_compress((void*)_check_null(_tmp496->topt));
{void*_tmp5C6=_tmp5C5;void*_tmp5C7;struct Cyc_Absyn_Tqual _tmp5C8;void*_tmp5C9;union Cyc_Absyn_Constraint*_tmp5CA;union Cyc_Absyn_Constraint*_tmp5CB;union Cyc_Absyn_Constraint*_tmp5CC;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C6)->tag == 5){_LL3D9: _tmp5C7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C6)->f1).elt_typ;_tmp5C8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C6)->f1).elt_tq;_tmp5C9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C6)->f1).ptr_atts).rgn;_tmp5CA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C6)->f1).ptr_atts).nullable;_tmp5CB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C6)->f1).ptr_atts).bounds;_tmp5CC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C6)->f1).ptr_atts).zero_term;_LL3DA:
# 3358
{void*_tmp5CD=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5CB);void*_tmp5CE=_tmp5CD;struct Cyc_Absyn_Exp*_tmp5CF;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5CE)->tag == 1){_LL3DE: _tmp5CF=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5CE)->f1;_LL3DF: {
# 3360
int do_null_check=!Cyc_Toc_in_sizeof(nv) && Cyc_Toc_need_null_check(_tmp496);
Cyc_Toc_exp_to_c(nv,_tmp496);
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpBCB;void*_tmpBCA;(_tmpBCA=0,Cyc_Tcutil_warn(e->loc,((_tmpBCB="inserted null check due to dereference",_tag_dyneither(_tmpBCB,sizeof(char),39))),_tag_dyneither(_tmpBCA,sizeof(void*),0)));}{
# 3366
struct Cyc_List_List*_tmpBCC;_tmp496->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(_tmp5C5),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpBCC=_cycalloc(sizeof(*_tmpBCC)),((_tmpBCC->hd=Cyc_Absyn_copy_exp(_tmp496),((_tmpBCC->tl=0,_tmpBCC)))))),0));};}
# 3374
if(!Cyc_Toc_in_sizeof(nv) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5CC)){
struct _tuple12 _tmp5D3=Cyc_Evexp_eval_const_uint_exp(_tmp5CF);struct _tuple12 _tmp5D4=_tmp5D3;unsigned int _tmp5D5;int _tmp5D6;_LL3E3: _tmp5D5=_tmp5D4.f1;_tmp5D6=_tmp5D4.f2;_LL3E4:
# 3380
 if(!_tmp5D6  || _tmp5D5 <= 0){
const char*_tmpBCF;void*_tmpBCE;(_tmpBCE=0,Cyc_Tcutil_terr(e->loc,((_tmpBCF="cannot determine dereference is in bounds",_tag_dyneither(_tmpBCF,sizeof(char),42))),_tag_dyneither(_tmpBCE,sizeof(void*),0)));}_LL3E2:;}
# 3383
goto _LL3DD;}}else{_LL3E0: _LL3E1: {
# 3386
struct Cyc_Absyn_Exp*_tmp5D9=Cyc_Absyn_uint_exp(0,0);
_tmp5D9->topt=Cyc_Absyn_uint_typ;
e->r=Cyc_Toc_subscript_exp_r(_tmp496,_tmp5D9);
Cyc_Toc_exp_to_c(nv,e);
goto _LL3DD;}}_LL3DD:;}
# 3392
goto _LL3D8;}else{_LL3DB: _LL3DC: {
const char*_tmpBD2;void*_tmpBD1;(_tmpBD1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBD2="exp_to_c: Deref: non-pointer",_tag_dyneither(_tmpBD2,sizeof(char),29))),_tag_dyneither(_tmpBD1,sizeof(void*),0)));}}_LL3D8:;}
# 3395
Cyc_Toc_set_lhs(nv,_tmp5C4);
goto _LL2DB;};}case 20: _LL308: _tmp492=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp493=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_tmp494=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp46E)->f3;_tmp495=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp46E)->f4;_LL309: {
# 3398
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp492->topt);
Cyc_Toc_exp_to_c(nv,_tmp492);
if(_tmp494  && _tmp495)
e->r=Cyc_Toc_check_tagged_union(_tmp492,Cyc_Toc_typ_to_c(e1_cyc_type),e1_cyc_type,_tmp493,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);
# 3406
if(is_poly)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
goto _LL2DB;}case 21: _LL30A: _tmp48E=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp48F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_tmp490=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp46E)->f3;_tmp491=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp46E)->f4;_LL30B: {
# 3410
int _tmp5DC=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp48E->topt));
int do_null_check=Cyc_Toc_need_null_check(_tmp48E);
Cyc_Toc_exp_to_c(nv,_tmp48E);{
int is_poly=Cyc_Toc_is_poly_project(e);
struct Cyc_Absyn_PtrInfo _tmp5DD=Cyc_Toc_get_ptr_type(e1typ);struct Cyc_Absyn_PtrInfo _tmp5DE=_tmp5DD;void*_tmp5DF;struct Cyc_Absyn_Tqual _tmp5E0;void*_tmp5E1;union Cyc_Absyn_Constraint*_tmp5E2;union Cyc_Absyn_Constraint*_tmp5E3;union Cyc_Absyn_Constraint*_tmp5E4;_LL3E8: _tmp5DF=_tmp5DE.elt_typ;_tmp5E0=_tmp5DE.elt_tq;_tmp5E1=(_tmp5DE.ptr_atts).rgn;_tmp5E2=(_tmp5DE.ptr_atts).nullable;_tmp5E3=(_tmp5DE.ptr_atts).bounds;_tmp5E4=(_tmp5DE.ptr_atts).zero_term;_LL3E9:
{void*_tmp5E5=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5E3);void*_tmp5E6=_tmp5E5;struct Cyc_Absyn_Exp*_tmp5E7;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5E6)->tag == 1){_LL3ED: _tmp5E7=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5E6)->f1;_LL3EE: {
# 3421
struct _tuple12 _tmp5E8=Cyc_Evexp_eval_const_uint_exp(_tmp5E7);struct _tuple12 _tmp5E9=_tmp5E8;unsigned int _tmp5EA;int _tmp5EB;_LL3F2: _tmp5EA=_tmp5E9.f1;_tmp5EB=_tmp5E9.f2;_LL3F3:
 if(_tmp5EB){
if(_tmp5EA < 1){
const char*_tmpBD5;void*_tmpBD4;(_tmpBD4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpBD5="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpBD5,sizeof(char),44))),_tag_dyneither(_tmpBD4,sizeof(void*),0)));}
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpBD8;void*_tmpBD7;(_tmpBD7=0,Cyc_Tcutil_warn(e->loc,((_tmpBD8="inserted null check due to dereference",_tag_dyneither(_tmpBD8,sizeof(char),39))),_tag_dyneither(_tmpBD7,sizeof(void*),0)));}{
# 3429
struct Cyc_Absyn_Exp*_tmpBD9[1];_tmp48E->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpBD9[0]=Cyc_Absyn_new_exp(_tmp48E->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBD9,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}}else{
# 3434
if(!Cyc_Evexp_c_can_eval(_tmp5E7)){
const char*_tmpBDC;void*_tmpBDB;(_tmpBDB=0,Cyc_Tcutil_terr(e->loc,((_tmpBDC="cannot determine pointer dereference in bounds",_tag_dyneither(_tmpBDC,sizeof(char),47))),_tag_dyneither(_tmpBDB,sizeof(void*),0)));}{
# 3438
struct Cyc_Absyn_Exp*_tmpBDD[4];_tmp48E->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpBDD[3]=
# 3442
Cyc_Absyn_uint_exp(0,0),((_tmpBDD[2]=
# 3441
Cyc_Absyn_sizeoftyp_exp(_tmp5DF,0),((_tmpBDD[1]=_tmp5E7,((_tmpBDD[0]=
# 3440
Cyc_Absyn_new_exp(_tmp48E->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBDD,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0));};}
# 3444
goto _LL3EC;_LL3F1:;}}else{_LL3EF: _LL3F0: {
# 3447
void*ta1=Cyc_Toc_typ_to_c(_tmp5DF);
{struct Cyc_Absyn_Exp*_tmpBDE[3];_tmp48E->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp5E0),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_dyneither_subscript_e,(
(_tmpBDE[2]=
# 3452
Cyc_Absyn_uint_exp(0,0),((_tmpBDE[1]=
# 3451
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpBDE[0]=
# 3450
Cyc_Absyn_new_exp(_tmp48E->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBDE,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}
# 3455
goto _LL3EC;}}_LL3EC:;}
# 3457
if(_tmp490  && _tmp491)
e->r=Cyc_Toc_check_tagged_union(_tmp48E,Cyc_Toc_typ_to_c(e1typ),_tmp5DF,_tmp48F,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);
# 3460
if(is_poly  && _tmp491)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
Cyc_Toc_set_lhs(nv,_tmp5DC);
goto _LL2DB;_LL3E7:;};};}case 22: _LL30C: _tmp48C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp48D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL30D: {
# 3465
int _tmp5F5=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp5F6=Cyc_Tcutil_compress((void*)_check_null(_tmp48C->topt));
# 3469
{void*_tmp5F7=_tmp5F6;void*_tmp5F8;struct Cyc_Absyn_Tqual _tmp5F9;void*_tmp5FA;union Cyc_Absyn_Constraint*_tmp5FB;union Cyc_Absyn_Constraint*_tmp5FC;union Cyc_Absyn_Constraint*_tmp5FD;struct Cyc_List_List*_tmp5FE;switch(*((int*)_tmp5F7)){case 10: _LL3F7: _tmp5FE=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5F7)->f1;_LL3F8:
# 3472
 Cyc_Toc_exp_to_c(nv,_tmp48C);
Cyc_Toc_exp_to_c(nv,_tmp48D);{
struct _tuple12 _tmp5FF=Cyc_Evexp_eval_const_uint_exp(_tmp48D);struct _tuple12 _tmp600=_tmp5FF;unsigned int _tmp601;int _tmp602;_LL3FE: _tmp601=_tmp600.f1;_tmp602=_tmp600.f2;_LL3FF:
 if(!_tmp602){
const char*_tmpBE1;void*_tmpBE0;(_tmpBE0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpBE1="unknown tuple subscript in translation to C",_tag_dyneither(_tmpBE1,sizeof(char),44))),_tag_dyneither(_tmpBE0,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp48C,Cyc_Absyn_fieldname((int)(_tmp601 + 1)));
goto _LL3F6;_LL3FD:;};case 5: _LL3F9: _tmp5F8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F7)->f1).elt_typ;_tmp5F9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F7)->f1).elt_tq;_tmp5FA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F7)->f1).ptr_atts).rgn;_tmp5FB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F7)->f1).ptr_atts).nullable;_tmp5FC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F7)->f1).ptr_atts).bounds;_tmp5FD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F7)->f1).ptr_atts).zero_term;_LL3FA: {
# 3480
struct Cyc_List_List*_tmp605=Cyc_Toc_get_relns(_tmp48C);
int _tmp606=Cyc_Toc_need_null_check(_tmp48C);
int _tmp607=Cyc_Toc_in_sizeof(nv);
# 3488
int in_bnds=_tmp607;
{void*_tmp608=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5FC);void*_tmp609=_tmp608;_LL403: _LL404:
# 3491
 in_bnds=in_bnds  || Cyc_Toc_check_bounds(_tmp5F6,_tmp605,_tmp48C,_tmp48D);
if(Cyc_Toc_warn_bounds_checks  && !in_bnds){
const char*_tmpBE5;void*_tmpBE4[1];struct Cyc_String_pa_PrintArg_struct _tmpBE3;(_tmpBE3.tag=0,((_tmpBE3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpBE4[0]=& _tmpBE3,Cyc_Tcutil_warn(e->loc,((_tmpBE5="bounds check necessary for %s",_tag_dyneither(_tmpBE5,sizeof(char),30))),_tag_dyneither(_tmpBE4,sizeof(void*),1)))))));}_LL402:;}
# 3498
Cyc_Toc_exp_to_c(nv,_tmp48C);
Cyc_Toc_exp_to_c(nv,_tmp48D);
++ Cyc_Toc_total_bounds_checks;
{void*_tmp60D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5FC);void*_tmp60E=_tmp60D;struct Cyc_Absyn_Exp*_tmp60F;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp60E)->tag == 1){_LL406: _tmp60F=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp60E)->f1;_LL407: {
# 3503
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5FB) && _tmp606;
void*ta1=Cyc_Toc_typ_to_c(_tmp5F8);
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp5F9);
if(in_bnds  && !possibly_null)
++ Cyc_Toc_bounds_checks_eliminated;else{
if(in_bnds  && possibly_null){
++ Cyc_Toc_bounds_checks_eliminated;
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpBE8;void*_tmpBE7;(_tmpBE7=0,Cyc_Tcutil_warn(e->loc,((_tmpBE8="inserted null check due to dereference",_tag_dyneither(_tmpBE8,sizeof(char),39))),_tag_dyneither(_tmpBE7,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpBE9[1];_tmp48C->r=Cyc_Toc_cast_it_r(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpBE9[0]=Cyc_Absyn_copy_exp(_tmp48C),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBE9,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}else{
# 3515
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5FD)){
# 3517
if(!Cyc_Evexp_c_can_eval(_tmp60F)){
const char*_tmpBEC;void*_tmpBEB;(_tmpBEB=0,Cyc_Tcutil_terr(e->loc,((_tmpBEC="cannot determine subscript is in bounds",_tag_dyneither(_tmpBEC,sizeof(char),40))),_tag_dyneither(_tmpBEB,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp48C);
struct Cyc_Absyn_Exp*_tmpBED[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(function_e,(
(_tmpBED[2]=_tmp48D,((_tmpBED[1]=_tmp60F,((_tmpBED[0]=_tmp48C,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBED,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp60F)){
const char*_tmpBF0;void*_tmpBEF;(_tmpBEF=0,Cyc_Tcutil_terr(e->loc,((_tmpBF0="cannot determine subscript is in bounds",_tag_dyneither(_tmpBF0,sizeof(char),40))),_tag_dyneither(_tmpBEF,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpBF3;void*_tmpBF2;(_tmpBF2=0,Cyc_Tcutil_warn(e->loc,((_tmpBF3="inserted null check due to dereference",_tag_dyneither(_tmpBF3,sizeof(char),39))),_tag_dyneither(_tmpBF2,sizeof(void*),0)));}{
# 3529
struct Cyc_Absyn_Exp*_tmpBF4[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpBF4[3]=_tmp48D,((_tmpBF4[2]=
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpBF4[1]=_tmp60F,((_tmpBF4[0]=_tmp48C,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBF4,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{
# 3535
if(!Cyc_Evexp_c_can_eval(_tmp60F)){
const char*_tmpBF7;void*_tmpBF6;(_tmpBF6=0,Cyc_Tcutil_terr(e->loc,((_tmpBF7="cannot determine subscript is in bounds",_tag_dyneither(_tmpBF7,sizeof(char),40))),_tag_dyneither(_tmpBF6,sizeof(void*),0)));}{
# 3538
struct Cyc_Absyn_Exp*_tmpBF8[2];_tmp48D->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,(
(_tmpBF8[1]=Cyc_Absyn_copy_exp(_tmp48D),((_tmpBF8[0]=_tmp60F,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBF8,sizeof(struct Cyc_Absyn_Exp*),2)))))));};}}}}
# 3541
goto _LL405;}}else{_LL408: _LL409: {
# 3543
void*ta1=Cyc_Toc_typ_to_c(_tmp5F8);
if(in_bnds){
# 3547
++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp5F9),
Cyc_Toc_member_exp(_tmp48C,Cyc_Toc_curr_sp,0)),_tmp48D);}else{
# 3552
struct Cyc_Absyn_Exp*_tmp61E=Cyc_Toc__check_dyneither_subscript_e;
struct Cyc_Absyn_Exp*_tmpBF9[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp5F9),
Cyc_Absyn_fncall_exp(_tmp61E,(
(_tmpBF9[2]=_tmp48D,((_tmpBF9[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpBF9[0]=_tmp48C,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBF9,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}
# 3558
goto _LL405;}}_LL405:;}
# 3560
goto _LL3F6;}default: _LL3FB: _LL3FC: {
const char*_tmpBFC;void*_tmpBFB;(_tmpBFB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBFC="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(_tmpBFC,sizeof(char),49))),_tag_dyneither(_tmpBFB,sizeof(void*),0)));}}_LL3F6:;}
# 3563
Cyc_Toc_set_lhs(nv,_tmp5F5);
goto _LL2DB;};}case 23: _LL30E: _tmp48B=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_LL30F:
# 3566
 if(!Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp48B))->r;else{
# 3571
struct Cyc_List_List*_tmp622=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp48B);
void*_tmp623=Cyc_Toc_add_tuple_type(_tmp622);
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp48B != 0;(_tmp48B=_tmp48B->tl,i ++)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp48B->hd);{
struct _tuple19*_tmpBFF;struct Cyc_List_List*_tmpBFE;dles=((_tmpBFE=_cycalloc(sizeof(*_tmpBFE)),((_tmpBFE->hd=((_tmpBFF=_cycalloc(sizeof(*_tmpBFF)),((_tmpBFF->f1=0,((_tmpBFF->f2=(struct Cyc_Absyn_Exp*)_tmp48B->hd,_tmpBFF)))))),((_tmpBFE->tl=dles,_tmpBFE))))));};}}
# 3578
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}
# 3581
goto _LL2DB;case 25: _LL310: _tmp48A=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_LL311:
# 3585
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp48A);
{struct Cyc_List_List*_tmp626=_tmp48A;for(0;_tmp626 != 0;_tmp626=_tmp626->tl){
struct _tuple19*_tmp627=(struct _tuple19*)_tmp626->hd;struct _tuple19*_tmp628=_tmp627;struct Cyc_Absyn_Exp*_tmp629;_LL40B: _tmp629=_tmp628->f2;_LL40C:
 Cyc_Toc_exp_to_c(nv,_tmp629);_LL40A:;}}
# 3590
goto _LL2DB;case 26: _LL312: _tmp486=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp487=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_tmp488=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp46E)->f3;_tmp489=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp46E)->f4;_LL313: {
# 3594
struct _tuple12 _tmp62A=Cyc_Evexp_eval_const_uint_exp(_tmp487);struct _tuple12 _tmp62B=_tmp62A;unsigned int _tmp62C;int _tmp62D;_LL410: _tmp62C=_tmp62B.f1;_tmp62D=_tmp62B.f2;_LL411: {
void*_tmp62E=Cyc_Toc_typ_to_c((void*)_check_null(_tmp488->topt));
Cyc_Toc_exp_to_c(nv,_tmp488);{
struct Cyc_List_List*es=0;
# 3599
if(!Cyc_Toc_is_zero(_tmp488)){
if(!_tmp62D){
const char*_tmpC02;void*_tmpC01;(_tmpC01=0,Cyc_Tcutil_terr(_tmp487->loc,((_tmpC02="cannot determine value of constant",_tag_dyneither(_tmpC02,sizeof(char),35))),_tag_dyneither(_tmpC01,sizeof(void*),0)));}
{unsigned int i=0;for(0;i < _tmp62C;++ i){
struct _tuple19*_tmpC05;struct Cyc_List_List*_tmpC04;es=((_tmpC04=_cycalloc(sizeof(*_tmpC04)),((_tmpC04->hd=((_tmpC05=_cycalloc(sizeof(*_tmpC05)),((_tmpC05->f1=0,((_tmpC05->f2=_tmp488,_tmpC05)))))),((_tmpC04->tl=es,_tmpC04))))));}}
# 3605
if(_tmp489){
struct Cyc_Absyn_Exp*_tmp633=Cyc_Toc_cast_it(_tmp62E,Cyc_Absyn_uint_exp(0,0));
struct _tuple19*_tmpC08;struct Cyc_List_List*_tmpC07;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,((_tmpC07=_cycalloc(sizeof(*_tmpC07)),((_tmpC07->hd=((_tmpC08=_cycalloc(sizeof(*_tmpC08)),((_tmpC08->f1=0,((_tmpC08->f2=_tmp633,_tmpC08)))))),((_tmpC07->tl=0,_tmpC07)))))));}}
# 3610
e->r=Cyc_Toc_unresolvedmem_exp_r(0,es);
goto _LL2DB;};}_LL40F:;}case 27: _LL314: _tmp483=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp484=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_tmp485=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp46E)->f3;_LL315:
# 3615
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,0);
goto _LL2DB;case 28: _LL316: _tmp47F=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp480=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_tmp481=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp46E)->f3;_tmp482=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp46E)->f4;_LL317:
# 3620
 if(!Cyc_Toc_is_toplevel(nv)){
struct Cyc_Absyn_Exp*_tmp636=Cyc_Toc_init_struct(nv,old_typ,_tmp480,0,0,_tmp481,_tmp47F);
e->r=_tmp636->r;
e->topt=_tmp636->topt;}else{
# 3631
if(_tmp482 == 0){
const char*_tmpC0B;void*_tmpC0A;(_tmpC0A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC0B="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpC0B,sizeof(char),38))),_tag_dyneither(_tmpC0A,sizeof(void*),0)));}{
struct Cyc_Absyn_Aggrdecl*sd2=_tmp482;
# 3635
struct _RegionHandle _tmp639=_new_region("rgn");struct _RegionHandle*rgn=& _tmp639;_push_region(rgn);
{struct Cyc_List_List*_tmp63A=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp481,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
# 3639
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp482->impl))->tagged){
# 3641
struct _tuple30*_tmp63B=(struct _tuple30*)((struct Cyc_List_List*)_check_null(_tmp63A))->hd;struct _tuple30*_tmp63C=_tmp63B;struct Cyc_Absyn_Aggrfield*_tmp63D;struct Cyc_Absyn_Exp*_tmp63E;_LL415: _tmp63D=_tmp63C->f1;_tmp63E=_tmp63C->f2;_LL416: {
void*_tmp63F=(void*)_check_null(_tmp63E->topt);
void*_tmp640=_tmp63D->type;
Cyc_Toc_exp_to_c(nv,_tmp63E);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp640) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp63F))
_tmp63E->r=
Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp63E->r,0));{
# 3650
int i=Cyc_Toc_get_member_offset(_tmp482,_tmp63D->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct _tuple19*_tmpC10;struct _tuple19*_tmpC0F;struct _tuple19*_tmpC0E[2];struct Cyc_List_List*_tmp641=(_tmpC0E[1]=((_tmpC0F=_cycalloc(sizeof(*_tmpC0F)),((_tmpC0F->f1=0,((_tmpC0F->f2=_tmp63E,_tmpC0F)))))),((_tmpC0E[0]=((_tmpC10=_cycalloc(sizeof(*_tmpC10)),((_tmpC10->f1=0,((_tmpC10->f2=field_tag_exp,_tmpC10)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC0E,sizeof(struct _tuple19*),2)))));
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp641,0);
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpC16;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpC15;void*_tmpC14[1];struct Cyc_List_List*ds=(_tmpC14[0]=(void*)((_tmpC16=_cycalloc(sizeof(*_tmpC16)),((_tmpC16[0]=((_tmpC15.tag=1,((_tmpC15.f1=_tmp63D->name,_tmpC15)))),_tmpC16)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC14,sizeof(void*),1)));
struct _tuple19*_tmpC19;struct _tuple19*_tmpC18[1];struct Cyc_List_List*dles=(_tmpC18[0]=((_tmpC19=_cycalloc(sizeof(*_tmpC19)),((_tmpC19->f1=ds,((_tmpC19->f2=umem,_tmpC19)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC18,sizeof(struct _tuple19*),1)));
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};}_LL414:;}else{
# 3659
struct Cyc_List_List*_tmp64A=0;
struct Cyc_List_List*_tmp64B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields;
for(0;_tmp64B != 0;_tmp64B=_tmp64B->tl){
struct Cyc_List_List*_tmp64C=_tmp63A;for(0;_tmp64C != 0;_tmp64C=_tmp64C->tl){
if((*((struct _tuple30*)_tmp64C->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp64B->hd){
struct _tuple30*_tmp64D=(struct _tuple30*)_tmp64C->hd;struct _tuple30*_tmp64E=_tmp64D;struct Cyc_Absyn_Aggrfield*_tmp64F;struct Cyc_Absyn_Exp*_tmp650;_LL41A: _tmp64F=_tmp64E->f1;_tmp650=_tmp64E->f2;_LL41B: {
void*_tmp651=Cyc_Toc_typ_to_c(_tmp64F->type);
void*_tmp652=Cyc_Toc_typ_to_c((void*)_check_null(_tmp650->topt));
Cyc_Toc_exp_to_c(nv,_tmp650);
# 3669
if(!Cyc_Tcutil_unify(_tmp651,_tmp652)){
# 3671
if(!Cyc_Tcutil_is_arithmetic_type(_tmp651) || !
Cyc_Tcutil_is_arithmetic_type(_tmp652))
_tmp650=Cyc_Toc_cast_it(_tmp651,Cyc_Absyn_copy_exp(_tmp650));}
{struct _tuple19*_tmpC1C;struct Cyc_List_List*_tmpC1B;_tmp64A=((_tmpC1B=_cycalloc(sizeof(*_tmpC1B)),((_tmpC1B->hd=((_tmpC1C=_cycalloc(sizeof(*_tmpC1C)),((_tmpC1C->f1=0,((_tmpC1C->f2=_tmp650,_tmpC1C)))))),((_tmpC1B->tl=_tmp64A,_tmpC1B))))));}
break;}_LL419:;}}}
# 3678
e->r=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp64A));}}
# 3636
;_pop_region(rgn);};}
# 3682
goto _LL2DB;case 29: _LL318: _tmp47D=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp47E=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL319: {
# 3684
struct Cyc_List_List*fs;
{void*_tmp655=Cyc_Tcutil_compress(_tmp47D);void*_tmp656=_tmp655;struct Cyc_List_List*_tmp657;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp656)->tag == 12){_LL41F: _tmp657=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp656)->f2;_LL420:
 fs=_tmp657;goto _LL41E;}else{_LL421: _LL422: {
const char*_tmpC20;void*_tmpC1F[1];struct Cyc_String_pa_PrintArg_struct _tmpC1E;(_tmpC1E.tag=0,((_tmpC1E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp47D)),((_tmpC1F[0]=& _tmpC1E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC20="anon struct has type %s",_tag_dyneither(_tmpC20,sizeof(char),24))),_tag_dyneither(_tmpC1F,sizeof(void*),1)))))));}}_LL41E:;}{
# 3689
struct _RegionHandle _tmp65B=_new_region("rgn");struct _RegionHandle*rgn=& _tmp65B;_push_region(rgn);{
struct Cyc_List_List*_tmp65C=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp47E,Cyc_Absyn_StructA,fs);
for(0;_tmp65C != 0;_tmp65C=_tmp65C->tl){
struct _tuple30*_tmp65D=(struct _tuple30*)_tmp65C->hd;struct _tuple30*_tmp65E=_tmp65D;struct Cyc_Absyn_Aggrfield*_tmp65F;struct Cyc_Absyn_Exp*_tmp660;_LL424: _tmp65F=_tmp65E->f1;_tmp660=_tmp65E->f2;_LL425: {
void*_tmp661=(void*)_check_null(_tmp660->topt);
void*_tmp662=_tmp65F->type;
Cyc_Toc_exp_to_c(nv,_tmp660);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp662) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp661))
_tmp660->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp660->r,0));}_LL423:;}
# 3703
e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp47E);}
# 3705
_npop_handler(0);goto _LL2DB;
# 3689
;_pop_region(rgn);};}case 30: _LL31A: _tmp47A=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp47B=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_tmp47C=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp46E)->f3;_LL31B: {
# 3708
void*datatype_ctype;
struct Cyc_Absyn_Exp*tag_exp;
struct _tuple1*_tmp663=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp664=Cyc_Absyn_var_exp(_tmp663,0);
struct Cyc_Absyn_Exp*mem_exp;
datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp47C->name,_tmp47B->name));
tag_exp=_tmp47B->is_extensible?Cyc_Absyn_var_exp(_tmp47C->name,0):
 Cyc_Toc_datatype_tag(_tmp47B,_tmp47C->name);
mem_exp=_tmp664;{
struct Cyc_List_List*_tmp665=_tmp47C->typs;
# 3719
if(Cyc_Toc_is_toplevel(nv)){
# 3721
struct Cyc_List_List*dles=0;
for(0;_tmp47A != 0;(_tmp47A=_tmp47A->tl,_tmp665=_tmp665->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp47A->hd;
void*_tmp666=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp665))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp666))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct _tuple19*_tmpC23;struct Cyc_List_List*_tmpC22;dles=((_tmpC22=_cycalloc(sizeof(*_tmpC22)),((_tmpC22->hd=((_tmpC23=_cycalloc(sizeof(*_tmpC23)),((_tmpC23->f1=0,((_tmpC23->f2=cur_e,_tmpC23)))))),((_tmpC22->tl=dles,_tmpC22))))));};}
# 3732
{struct _tuple19*_tmpC26;struct Cyc_List_List*_tmpC25;dles=((_tmpC25=_cycalloc(sizeof(*_tmpC25)),((_tmpC25->hd=((_tmpC26=_cycalloc(sizeof(*_tmpC26)),((_tmpC26->f1=0,((_tmpC26->f2=tag_exp,_tmpC26)))))),((_tmpC25->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles),_tmpC25))))));}
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{
# 3738
struct Cyc_List_List*_tmpC27;struct Cyc_List_List*_tmp66B=
(_tmpC27=_cycalloc(sizeof(*_tmpC27)),((_tmpC27->hd=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Toc_tag_sp,0),tag_exp,0),((_tmpC27->tl=0,_tmpC27)))));
# 3741
{int i=1;for(0;_tmp47A != 0;(((_tmp47A=_tmp47A->tl,i ++)),_tmp665=_tmp665->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp47A->hd;
void*_tmp66C=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp665))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp66C))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct Cyc_Absyn_Stmt*_tmp66D=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Absyn_fieldname(i),0),cur_e,0);
# 3751
struct Cyc_List_List*_tmpC28;_tmp66B=((_tmpC28=_cycalloc(sizeof(*_tmpC28)),((_tmpC28->hd=_tmp66D,((_tmpC28->tl=_tmp66B,_tmpC28))))));};}}{
# 3753
struct Cyc_Absyn_Stmt*_tmp66F=Cyc_Absyn_exp_stmt(_tmp664,0);
struct Cyc_List_List*_tmpC29;struct Cyc_Absyn_Stmt*_tmp670=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpC29=_cycalloc(sizeof(*_tmpC29)),((_tmpC29->hd=_tmp66F,((_tmpC29->tl=_tmp66B,_tmpC29))))))),0);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp663,datatype_ctype,0,_tmp670,0));};}
# 3757
goto _LL2DB;};}case 31: _LL31C: _LL31D:
# 3759
 goto _LL31F;case 32: _LL31E: _LL31F:
 goto _LL2DB;case 33: _LL320: _tmp474=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46E)->f1).is_calloc;_tmp475=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46E)->f1).rgn;_tmp476=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46E)->f1).elt_type;_tmp477=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46E)->f1).num_elts;_tmp478=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46E)->f1).fat_result;_tmp479=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46E)->f1).inline_call;_LL321: {
# 3763
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp476)));
Cyc_Toc_exp_to_c(nv,_tmp477);
# 3767
if(_tmp478){
struct _tuple1*_tmp673=Cyc_Toc_temp_var();
struct _tuple1*_tmp674=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp474){
xexp=_tmp477;
if(_tmp475 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp475;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp673,0));}else{
# 3778
pexp=Cyc_Toc_calloc_exp(*_tmp476,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp673,0));}{
# 3780
struct Cyc_Absyn_Exp*_tmpC2A[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpC2A[2]=
Cyc_Absyn_var_exp(_tmp673,0),((_tmpC2A[1]=
# 3781
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpC2A[0]=Cyc_Absyn_var_exp(_tmp674,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC2A,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{
# 3784
if(_tmp475 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp475;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp479)
pexp=Cyc_Toc_rmalloc_inline_exp(rgn,Cyc_Absyn_var_exp(_tmp673,0));else{
# 3790
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp673,0));}}else{
# 3792
pexp=Cyc_Toc_malloc_exp(*_tmp476,Cyc_Absyn_var_exp(_tmp673,0));}{
# 3794
struct Cyc_Absyn_Exp*_tmpC2B[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((_tmpC2B[2]=
Cyc_Absyn_var_exp(_tmp673,0),((_tmpC2B[1]=
# 3794
Cyc_Absyn_uint_exp(1,0),((_tmpC2B[0]=Cyc_Absyn_var_exp(_tmp674,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC2B,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}{
# 3797
struct Cyc_Absyn_Stmt*_tmp677=Cyc_Absyn_declare_stmt(_tmp673,Cyc_Absyn_uint_typ,_tmp477,
Cyc_Absyn_declare_stmt(_tmp674,Cyc_Absyn_cstar_typ(t_c,Cyc_Toc_mt_tq),pexp,
Cyc_Absyn_exp_stmt(rexp,0),0),0);
e->r=Cyc_Toc_stmt_exp_r(_tmp677);};}else{
# 3802
if(_tmp474){
if(_tmp475 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp475;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp477))->r;}else{
# 3808
e->r=(Cyc_Toc_calloc_exp(*_tmp476,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp477))->r;}}else{
# 3811
if(_tmp475 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp475;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp479)
e->r=(Cyc_Toc_rmalloc_inline_exp(rgn,_tmp477))->r;else{
# 3817
e->r=(Cyc_Toc_rmalloc_exp(rgn,_tmp477))->r;}}else{
# 3819
e->r=(Cyc_Toc_malloc_exp(*_tmp476,_tmp477))->r;}}}
# 3823
goto _LL2DB;}case 34: _LL322: _tmp472=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp473=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL323: {
# 3832
void*e1_old_typ=(void*)_check_null(_tmp472->topt);
void*e2_old_typ=(void*)_check_null(_tmp473->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ) && !Cyc_Tcutil_is_pointer_type(e1_old_typ)){
const char*_tmpC2E;void*_tmpC2D;(_tmpC2D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC2E="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",_tag_dyneither(_tmpC2E,sizeof(char),57))),_tag_dyneither(_tmpC2D,sizeof(void*),0)));}{
# 3838
struct Cyc_Absyn_Exp*swap_fn;
if(Cyc_Tcutil_is_dyneither_ptr(e1_old_typ))
swap_fn=Cyc_Toc__swap_dyneither_e;else{
# 3842
swap_fn=Cyc_Toc__swap_word_e;}{
# 3846
int f1_tag=0;
void*tagged_mem_type1=(void*)& Cyc_Absyn_VoidType_val;
int e1_tmem=Cyc_Toc_is_tagged_union_project(_tmp472,& f1_tag,& tagged_mem_type1,1);
int f2_tag=0;
void*tagged_mem_type2=(void*)& Cyc_Absyn_VoidType_val;
int e2_tmem=Cyc_Toc_is_tagged_union_project(_tmp473,& f2_tag,& tagged_mem_type2,1);
# 3853
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp472);
Cyc_Toc_exp_to_c(nv,_tmp473);
Cyc_Toc_set_lhs(nv,0);
# 3859
if(e1_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp472,f1_tag,tagged_mem_type1);else{
# 3863
_tmp472=Cyc_Toc_push_address_exp(_tmp472);}
# 3865
if(e2_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp473,f2_tag,tagged_mem_type2);else{
# 3869
_tmp473=Cyc_Toc_push_address_exp(_tmp473);}
# 3871
{struct Cyc_Absyn_Exp*_tmpC2F[2];e->r=Cyc_Toc_fncall_exp_r(swap_fn,((_tmpC2F[1]=_tmp473,((_tmpC2F[0]=_tmp472,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC2F,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
# 3873
goto _LL2DB;};};}case 37: _LL324: _tmp470=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp471=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL325: {
# 3876
void*_tmp67B=Cyc_Tcutil_compress((void*)_check_null(_tmp470->topt));
Cyc_Toc_exp_to_c(nv,_tmp470);
{void*_tmp67C=_tmp67B;struct Cyc_Absyn_Aggrdecl*_tmp67D;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp67C)->tag == 11){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp67C)->f1).aggr_info).KnownAggr).tag == 2){_LL429: _tmp67D=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp67C)->f1).aggr_info).KnownAggr).val;_LL42A: {
# 3880
struct Cyc_Absyn_Exp*_tmp67E=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp67D,_tmp471),0);
struct Cyc_Absyn_Exp*_tmp67F=Cyc_Toc_member_exp(_tmp470,_tmp471,0);
struct Cyc_Absyn_Exp*_tmp680=Cyc_Toc_member_exp(_tmp67F,Cyc_Toc_tag_sp,0);
e->r=(Cyc_Absyn_eq_exp(_tmp680,_tmp67E,0))->r;
goto _LL428;}}else{goto _LL42B;}}else{_LL42B: _LL42C: {
const char*_tmpC32;void*_tmpC31;(_tmpC31=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC32="non-aggregate type in tagcheck",_tag_dyneither(_tmpC32,sizeof(char),31))),_tag_dyneither(_tmpC31,sizeof(void*),0)));}}_LL428:;}
# 3887
goto _LL2DB;}case 36: _LL326: _tmp46F=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_LL327:
 Cyc_Toc_stmt_to_c(nv,_tmp46F);goto _LL2DB;case 35: _LL328: _LL329: {
const char*_tmpC35;void*_tmpC34;(_tmpC34=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC35="UnresolvedMem",_tag_dyneither(_tmpC35,sizeof(char),14))),_tag_dyneither(_tmpC34,sizeof(void*),0)));}case 24: _LL32A: _LL32B: {
const char*_tmpC38;void*_tmpC37;(_tmpC37=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC38="compoundlit",_tag_dyneither(_tmpC38,sizeof(char),12))),_tag_dyneither(_tmpC37,sizeof(void*),0)));}case 38: _LL32C: _LL32D: {
const char*_tmpC3B;void*_tmpC3A;(_tmpC3A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC3B="valueof(-)",_tag_dyneither(_tmpC3B,sizeof(char),11))),_tag_dyneither(_tmpC3A,sizeof(void*),0)));}default: _LL32E: _LL32F: {
const char*_tmpC3E;void*_tmpC3D;(_tmpC3D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC3E="__asm__",_tag_dyneither(_tmpC3E,sizeof(char),8))),_tag_dyneither(_tmpC3D,sizeof(void*),0)));}}_LL2DB:;};}struct _tuple31{int f1;struct _dyneither_ptr*f2;struct _dyneither_ptr*f3;struct Cyc_Absyn_Switch_clause*f4;};
# 3920 "toc.cyc"
static struct _tuple31*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 3922
struct _tuple31*_tmpC3F;return(_tmpC3F=_region_malloc(r,sizeof(*_tmpC3F)),((_tmpC3F->f1=0,((_tmpC3F->f2=Cyc_Toc_fresh_label(),((_tmpC3F->f3=Cyc_Toc_fresh_label(),((_tmpC3F->f4=sc,_tmpC3F)))))))));}
# 3927
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(0;ps != 0;ps=((struct Cyc_List_List*)_check_null(ps))->tl){
struct Cyc_Tcpat_PathNode*_tmp68C=(struct Cyc_Tcpat_PathNode*)ps->hd;
# 3933
if((int)(((_tmp68C->orig_pat).pattern).tag == 1)){
union Cyc_Tcpat_PatOrWhere _tmpC40;void*t=(void*)_check_null(((_tmpC40=_tmp68C->orig_pat,(((_tmpC40.pattern).tag != 1?_throw_match(): 1,(_tmpC40.pattern).val))))->topt);
void*_tmp68D=Cyc_Tcutil_compress(Cyc_Toc_typ_to_c_array(t));
void*_tmp68E=_tmp68D;switch(*((int*)_tmp68E)){case 0: _LL42E: _LL42F:
# 3938
 goto _LL431;case 11: _LL430: _LL431:
 goto _LL433;case 12: _LL432: _LL433:
 goto _LL42D;default: _LL434: _LL435:
 v=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c_array(t),v);goto _LL42D;}_LL42D:;}{
# 3944
void*_tmp690=_tmp68C->access;void*_tmp691=_tmp690;struct Cyc_Absyn_Datatypedecl*_tmp692;struct Cyc_Absyn_Datatypefield*_tmp693;unsigned int _tmp694;int _tmp695;struct _dyneither_ptr*_tmp696;unsigned int _tmp697;switch(*((int*)_tmp691)){case 0: _LL437: _LL438:
# 3949
 goto _LL436;case 1: _LL439: _LL43A:
# 3954
 if(ps->tl != 0){
void*_tmp698=((struct Cyc_Tcpat_PathNode*)((struct Cyc_List_List*)_check_null(ps->tl))->hd)->access;void*_tmp699=_tmp698;struct Cyc_Absyn_Datatypedecl*_tmp69A;struct Cyc_Absyn_Datatypefield*_tmp69B;unsigned int _tmp69C;if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp699)->tag == 3){_LL442: _tmp69A=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp699)->f1;_tmp69B=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp699)->f2;_tmp69C=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp699)->f3;_LL443:
# 3957
 ps=ps->tl;
v=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp69B->name,_tmp69A->name)),Cyc_Toc_mt_tq),v);
v=Cyc_Absyn_aggrarrow_exp(v,Cyc_Absyn_fieldname((int)(_tmp69C + 1)),0);
continue;}else{_LL444: _LL445:
# 3962
 goto _LL441;}_LL441:;}
# 3965
v=Cyc_Absyn_deref_exp(v,0);
goto _LL436;case 2: _LL43B: _tmp697=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp691)->f1;_LL43C:
 v=Cyc_Toc_member_exp(v,Cyc_Absyn_fieldname((int)(_tmp697 + 1)),0);goto _LL436;case 4: _LL43D: _tmp695=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp691)->f1;_tmp696=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp691)->f2;_LL43E:
# 3969
 v=Cyc_Toc_member_exp(v,_tmp696,0);
if(_tmp695)
v=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0);
goto _LL436;default: _LL43F: _tmp692=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp691)->f1;_tmp693=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp691)->f2;_tmp694=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp691)->f3;_LL440:
# 3974
 v=Cyc_Toc_member_exp(v,Cyc_Absyn_fieldname((int)(_tmp694 + 1)),0);
goto _LL436;}_LL436:;};}
# 3978
return v;}
# 3983
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
void*_tmp69D=t;struct Cyc_Absyn_Datatypedecl*_tmp69E;struct Cyc_Absyn_Datatypefield*_tmp69F;struct _dyneither_ptr*_tmp6A0;int _tmp6A1;int _tmp6A2;struct Cyc_Absyn_Datatypedecl*_tmp6A3;struct Cyc_Absyn_Datatypefield*_tmp6A4;unsigned int _tmp6A5;struct _dyneither_ptr _tmp6A6;int _tmp6A7;void*_tmp6A8;struct Cyc_Absyn_Enumfield*_tmp6A9;struct Cyc_Absyn_Enumdecl*_tmp6AA;struct Cyc_Absyn_Enumfield*_tmp6AB;struct Cyc_Absyn_Exp*_tmp6AC;switch(*((int*)_tmp69D)){case 0: _LL447: _tmp6AC=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp69D)->f1;_LL448:
# 3987
 if(_tmp6AC == 0)return v;else{return _tmp6AC;}case 1: _LL449: _LL44A:
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_signed_int_exp(0,0),0);case 2: _LL44B: _LL44C:
 return Cyc_Absyn_neq_exp(v,Cyc_Absyn_signed_int_exp(0,0),0);case 3: _LL44D: _tmp6AA=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp69D)->f1;_tmp6AB=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp69D)->f2;_LL44E: {
# 3991
struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmpC43;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpC42;return Cyc_Absyn_eq_exp(v,Cyc_Absyn_new_exp((void*)((_tmpC42=_cycalloc(sizeof(*_tmpC42)),((_tmpC42[0]=((_tmpC43.tag=31,((_tmpC43.f1=_tmp6AB->name,((_tmpC43.f2=_tmp6AA,((_tmpC43.f3=_tmp6AB,_tmpC43)))))))),_tmpC42)))),0),0);}case 4: _LL44F: _tmp6A8=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp69D)->f1;_tmp6A9=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp69D)->f2;_LL450: {
# 3993
struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpC46;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpC45;return Cyc_Absyn_eq_exp(v,Cyc_Absyn_new_exp((void*)((_tmpC45=_cycalloc(sizeof(*_tmpC45)),((_tmpC45[0]=((_tmpC46.tag=32,((_tmpC46.f1=_tmp6A9->name,((_tmpC46.f2=_tmp6A8,((_tmpC46.f3=_tmp6A9,_tmpC46)))))))),_tmpC45)))),0),0);}case 5: _LL451: _tmp6A6=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp69D)->f1;_tmp6A7=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp69D)->f2;_LL452:
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_float_exp(_tmp6A6,_tmp6A7,0),0);case 6: _LL453: _tmp6A5=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp69D)->f1;_LL454:
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_uint_exp(_tmp6A5,0),0);case 7: _LL455: _tmp6A2=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp69D)->f1;_tmp6A3=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp69D)->f2;_tmp6A4=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp69D)->f3;_LL456:
# 3999
 LOOP1: {
void*_tmp6B1=v->r;void*_tmp6B2=_tmp6B1;struct Cyc_Absyn_Exp*_tmp6B3;struct Cyc_Absyn_Exp*_tmp6B4;switch(*((int*)_tmp6B2)){case 13: _LL45C: _tmp6B4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6B2)->f2;_LL45D:
 v=_tmp6B4;goto LOOP1;case 19: _LL45E: _tmp6B3=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6B2)->f1;_LL45F:
 v=_tmp6B3;goto _LL45B;default: _LL460: _LL461:
 goto _LL45B;}_LL45B:;}
# 4006
v=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp6A4->name,_tmp6A3->name)),Cyc_Toc_mt_tq),v);
return Cyc_Absyn_eq_exp(Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0),Cyc_Absyn_uint_exp((unsigned int)_tmp6A2,0),0);case 8: _LL457: _tmp6A0=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp69D)->f1;_tmp6A1=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp69D)->f2;_LL458:
# 4009
 v=Cyc_Toc_member_exp(Cyc_Toc_member_exp(v,_tmp6A0,0),Cyc_Toc_tag_sp,0);
return Cyc_Absyn_eq_exp(v,Cyc_Absyn_uint_exp((unsigned int)_tmp6A1,0),0);default: _LL459: _tmp69E=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp69D)->f1;_tmp69F=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp69D)->f2;_LL45A:
# 4013
 LOOP2: {
void*_tmp6B5=v->r;void*_tmp6B6=_tmp6B5;struct Cyc_Absyn_Exp*_tmp6B7;struct Cyc_Absyn_Exp*_tmp6B8;switch(*((int*)_tmp6B6)){case 13: _LL463: _tmp6B8=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6B6)->f2;_LL464:
 v=_tmp6B8;goto LOOP2;case 19: _LL465: _tmp6B7=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6B6)->f1;_LL466:
 v=_tmp6B7;goto _LL462;default: _LL467: _LL468:
 goto _LL462;}_LL462:;}
# 4020
v=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp69F->name,_tmp69E->name)),Cyc_Toc_mt_tq),v);
return Cyc_Absyn_eq_exp(Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0),Cyc_Absyn_var_exp(_tmp69F->name,0),0);}_LL446:;}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _dyneither_ptr*f1;};
# 4032
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1};struct _tuple32{int f1;void*f2;};
# 4040
static struct _tuple32 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 4042
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(0;ss != 0;(ss=ss->tl,c=c + 1)){
struct _tuple29 _tmp6BB=*((struct _tuple29*)ss->hd);struct _tuple29 _tmp6BC=_tmp6BB;void*_tmp6BD;_LL46A: _tmp6BD=_tmp6BC.f1;_LL46B: {
void*_tmp6BE=_tmp6BD;struct Cyc_Absyn_Exp*_tmp6BF;struct _dyneither_ptr*_tmp6C0;switch(*((int*)_tmp6BE)){case 3: _LL46F: _LL470:
# 4048
 goto _LL472;case 4: _LL471: _LL472:
 goto _LL474;case 6: _LL473: _LL474:
# 4051
 continue;case 8: _LL475: _tmp6C0=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp6BE)->f1;_LL476:
# 4053
 if(k == (void*)& Cyc_Toc_OtherTest_val){
struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct _tmpC49;struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_tmpC48;k=(void*)((_tmpC48=_cycalloc(sizeof(*_tmpC48)),((_tmpC48[0]=((_tmpC49.tag=3,((_tmpC49.f1=_tmp6C0,_tmpC49)))),_tmpC48))));}
continue;case 7: _LL477: _LL478:
# 4057
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;
continue;case 0: _LL479: _tmp6BF=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp6BE)->f1;if(_tmp6BF != 0){_LL47A:
# 4061
{struct Cyc_Toc_WhereTest_Toc_TestKind_struct _tmpC4C;struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_tmpC4B;k=(void*)((_tmpC4B=_cycalloc(sizeof(*_tmpC4B)),((_tmpC4B[0]=((_tmpC4C.tag=2,((_tmpC4C.f1=_tmp6BF,_tmpC4C)))),_tmpC4B))));}{
struct _tuple32 _tmpC4D;return(_tmpC4D.f1=0,((_tmpC4D.f2=k,_tmpC4D)));};}else{_LL47B: _LL47C:
 goto _LL47E;}case 1: _LL47D: _LL47E:
 goto _LL480;case 2: _LL47F: _LL480:
 goto _LL482;case 5: _LL481: _LL482:
 goto _LL484;default: _LL483: _LL484: {
struct _tuple32 _tmpC4E;return(_tmpC4E.f1=0,((_tmpC4E.f2=k,_tmpC4E)));}}_LL46E:;}_LL469:;}{
# 4070
struct _tuple32 _tmpC4F;return(_tmpC4F.f1=c,((_tmpC4F.f2=k,_tmpC4F)));};}
# 4075
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{void*_tmp6C8=p;int _tmp6C9;int _tmp6CA;unsigned int _tmp6CB;void*_tmp6CC;struct Cyc_Absyn_Enumfield*_tmp6CD;struct Cyc_Absyn_Enumdecl*_tmp6CE;struct Cyc_Absyn_Enumfield*_tmp6CF;switch(*((int*)_tmp6C8)){case 3: _LL486: _tmp6CE=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp6C8)->f1;_tmp6CF=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp6C8)->f2;_LL487:
# 4079
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmpC52;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpC51;e=Cyc_Absyn_new_exp((void*)((_tmpC51=_cycalloc(sizeof(*_tmpC51)),((_tmpC51[0]=((_tmpC52.tag=31,((_tmpC52.f1=_tmp6CF->name,((_tmpC52.f2=_tmp6CE,((_tmpC52.f3=_tmp6CF,_tmpC52)))))))),_tmpC51)))),0);}goto _LL485;case 4: _LL488: _tmp6CC=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp6C8)->f1;_tmp6CD=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp6C8)->f2;_LL489:
# 4081
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpC55;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpC54;e=Cyc_Absyn_new_exp((void*)((_tmpC54=_cycalloc(sizeof(*_tmpC54)),((_tmpC54[0]=((_tmpC55.tag=32,((_tmpC55.f1=_tmp6CD->name,((_tmpC55.f2=_tmp6CC,((_tmpC55.f3=_tmp6CD,_tmpC55)))))))),_tmpC54)))),0);}goto _LL485;case 6: _LL48A: _tmp6CB=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp6C8)->f1;_LL48B:
 _tmp6CA=(int)_tmp6CB;goto _LL48D;case 7: _LL48C: _tmp6CA=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp6C8)->f1;_LL48D:
 _tmp6C9=_tmp6CA;goto _LL48F;case 8: _LL48E: _tmp6C9=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp6C8)->f2;_LL48F:
# 4085
 e=Cyc_Absyn_uint_exp((unsigned int)_tmp6C9,0);
goto _LL485;default: _LL490: _LL491: {
const char*_tmpC58;void*_tmpC57;(_tmpC57=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC58="compile_pat_test_as_case!",_tag_dyneither(_tmpC58,sizeof(char),26))),_tag_dyneither(_tmpC57,sizeof(void*),0)));}}_LL485:;}{
# 4089
struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct _tmpC5B;struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmpC5A;return Cyc_Absyn_new_pat((void*)((_tmpC5A=_cycalloc(sizeof(*_tmpC5A)),((_tmpC5A[0]=((_tmpC5B.tag=17,((_tmpC5B.f1=e,_tmpC5B)))),_tmpC5A)))),0);};}
# 4093
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0);}struct _tuple33{struct _tuple1*f1;void*f2;};struct _tuple34{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 4102
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**decls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 4105
void*t=(void*)_check_null(p->topt);
void*_tmp6D8=p->r;void*_tmp6D9=_tmp6D8;union Cyc_Absyn_AggrInfoU _tmp6DA;struct Cyc_List_List*_tmp6DB;struct Cyc_List_List*_tmp6DC;struct Cyc_List_List*_tmp6DD;struct Cyc_Absyn_Pat*_tmp6DE;struct Cyc_Absyn_Datatypedecl*_tmp6DF;struct Cyc_Absyn_Datatypefield*_tmp6E0;struct Cyc_List_List*_tmp6E1;struct Cyc_Absyn_Vardecl*_tmp6E2;struct Cyc_Absyn_Pat*_tmp6E3;struct _tuple1*_tmp6E4;struct _tuple1*_tmp6E5;struct Cyc_Absyn_Pat*_tmp6E6;struct _tuple1*_tmp6E7;switch(*((int*)_tmp6D9)){case 2: _LL493: _tmp6E7=(((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp6D9)->f2)->name;_LL494: {
# 4108
struct Cyc_Absyn_Pat*_tmp6E8=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0);
_tmp6E8->topt=p->topt;
_tmp6E5=_tmp6E7;_tmp6E6=_tmp6E8;goto _LL496;}case 1: _LL495: _tmp6E5=(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp6D9)->f1)->name;_tmp6E6=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp6D9)->f2;_LL496: {
# 4113
struct _tuple1*v=Cyc_Toc_temp_var();
void*_tmp6E9=(void*)_check_null(_tmp6E6->topt);
{struct _tuple33*_tmpC5E;struct Cyc_List_List*_tmpC5D;*decls=((_tmpC5D=_region_malloc(rgn,sizeof(*_tmpC5D)),((_tmpC5D->hd=((_tmpC5E=_region_malloc(rgn,sizeof(*_tmpC5E)),((_tmpC5E->f1=v,((_tmpC5E->f2=Cyc_Toc_typ_to_c_array(_tmp6E9),_tmpC5E)))))),((_tmpC5D->tl=*decls,_tmpC5D))))));}
*nv=Cyc_Toc_add_varmap(rgn,*nv,_tmp6E5,Cyc_Absyn_var_exp(v,0));{
struct Cyc_Absyn_Stmt*_tmp6EC=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(v,0),Cyc_Absyn_copy_exp(path),0);
return Cyc_Toc_seq_stmt_opt(_tmp6EC,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp6E6));};}case 4: _LL497: _tmp6E4=(((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp6D9)->f2)->name;_LL498: {
# 4121
struct _tuple1*_tmp6ED=Cyc_Toc_temp_var();
void*_tmp6EE=(void*)_check_null(p->topt);
{struct _tuple33*_tmpC61;struct Cyc_List_List*_tmpC60;*decls=((_tmpC60=_region_malloc(rgn,sizeof(*_tmpC60)),((_tmpC60->hd=((_tmpC61=_region_malloc(rgn,sizeof(*_tmpC61)),((_tmpC61->f1=_tmp6ED,((_tmpC61->f2=Cyc_Toc_typ_to_c_array(_tmp6EE),_tmpC61)))))),((_tmpC60->tl=*decls,_tmpC60))))));}
*nv=Cyc_Toc_add_varmap(rgn,*nv,_tmp6E4,Cyc_Absyn_var_exp(_tmp6ED,0));
return Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp6ED,0),Cyc_Absyn_copy_exp(path),0);}case 0: _LL499: _LL49A:
 return 0;case 3: _LL49B: _tmp6E2=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp6D9)->f1;_tmp6E3=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp6D9)->f2;_LL49C: {
# 4129
struct _tuple1*_tmp6F1=Cyc_Toc_temp_var();
{struct _tuple33*_tmpC64;struct Cyc_List_List*_tmpC63;*decls=((_tmpC63=_region_malloc(rgn,sizeof(*_tmpC63)),((_tmpC63->hd=((_tmpC64=_region_malloc(rgn,sizeof(*_tmpC64)),((_tmpC64->f1=_tmp6F1,((_tmpC64->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),_tmpC64)))))),((_tmpC63->tl=*decls,_tmpC63))))));}
*nv=Cyc_Toc_add_varmap(rgn,*nv,_tmp6E2->name,Cyc_Absyn_var_exp(_tmp6F1,0));{
# 4133
struct Cyc_Absyn_Stmt*_tmp6F4=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp6F1,0),
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path))),0);
return Cyc_Toc_seq_stmt_opt(_tmp6F4,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp6E3));};}case 9: _LL49D: _LL49E:
# 4138
 goto _LL4A0;case 10: _LL49F: _LL4A0:
 goto _LL4A2;case 11: _LL4A1: _LL4A2:
 goto _LL4A4;case 12: _LL4A3: _LL4A4:
 goto _LL4A6;case 13: _LL4A5: _LL4A6:
 goto _LL4A8;case 14: _LL4A7: _LL4A8:
 return 0;case 6: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6D9)->f1)->r)->tag == 8){_LL4A9: _tmp6DF=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6D9)->f1)->r)->f1;_tmp6E0=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6D9)->f1)->r)->f2;_tmp6E1=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6D9)->f1)->r)->f3;_LL4AA:
# 4147
 if(_tmp6E1 == 0)return 0;{
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(_tmp6E0->name,_tmp6DF->name);
void*_tmp6F5=Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);
path=Cyc_Toc_cast_it(_tmp6F5,path);{
int cnt=1;
struct Cyc_List_List*_tmp6F6=_tmp6E0->typs;
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp6E1 != 0;(((_tmp6E1=_tmp6E1->tl,_tmp6F6=((struct Cyc_List_List*)_check_null(_tmp6F6))->tl)),++ cnt)){
struct Cyc_Absyn_Pat*_tmp6F7=(struct Cyc_Absyn_Pat*)_tmp6E1->hd;
if(_tmp6F7->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
void*_tmp6F8=(*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp6F6))->hd)).f2;
void*_tmp6F9=(void*)_check_null(_tmp6F7->topt);
void*_tmp6FA=Cyc_Toc_typ_to_c_array(_tmp6F9);
struct Cyc_Absyn_Exp*_tmp6FB=Cyc_Absyn_aggrarrow_exp(path,Cyc_Absyn_fieldname(cnt),0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp6F8)))
_tmp6FB=Cyc_Toc_cast_it(_tmp6FA,_tmp6FB);
s=Cyc_Toc_seq_stmt_opt(s,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp6FB,_tmp6F7));};}
# 4165
return s;};};}else{_LL4B3: _tmp6DE=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6D9)->f1;_LL4B4:
# 4214
 return Cyc_Toc_extract_pattern_vars(rgn,nv,decls,Cyc_Absyn_deref_exp(path,0),_tmp6DE);}case 8: _LL4AB: _tmp6DD=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp6D9)->f3;_LL4AC:
# 4167
 _tmp6DC=_tmp6DD;goto _LL4AE;case 5: _LL4AD: _tmp6DC=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp6D9)->f1;_LL4AE: {
# 4169
struct Cyc_Absyn_Stmt*s=0;
int cnt=1;
for(0;_tmp6DC != 0;(_tmp6DC=_tmp6DC->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp6FC=(struct Cyc_Absyn_Pat*)_tmp6DC->hd;
if(_tmp6FC->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp6FD=(void*)_check_null(_tmp6FC->topt);
struct _dyneither_ptr*_tmp6FE=Cyc_Absyn_fieldname(cnt);
s=Cyc_Toc_seq_stmt_opt(s,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,Cyc_Toc_member_exp(path,_tmp6FE,0),_tmp6FC));};}
# 4179
return s;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp6D9)->f1 == 0){_LL4AF: _LL4B0: {
const char*_tmpC67;void*_tmpC66;(_tmpC66=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC67="unresolved aggregate pattern!",_tag_dyneither(_tmpC67,sizeof(char),30))),_tag_dyneither(_tmpC66,sizeof(void*),0)));}}else{_LL4B1: _tmp6DA=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp6D9)->f1)->aggr_info;_tmp6DB=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp6D9)->f3;_LL4B2: {
# 4183
struct Cyc_Absyn_Aggrdecl*_tmp701=Cyc_Absyn_get_known_aggrdecl(_tmp6DA);
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp6DB != 0;_tmp6DB=_tmp6DB->tl){
struct _tuple34*_tmp702=(struct _tuple34*)_tmp6DB->hd;
struct Cyc_Absyn_Pat*_tmp703=(*_tmp702).f2;
if(_tmp703->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*f;
{void*_tmp704=(void*)((struct Cyc_List_List*)_check_null((*_tmp702).f1))->hd;void*_tmp705=_tmp704;struct _dyneither_ptr*_tmp706;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp705)->tag == 1){_LL4BC: _tmp706=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp705)->f1;_LL4BD:
 f=_tmp706;goto _LL4BB;}else{_LL4BE: _LL4BF: {
struct Cyc_Toc_Match_error_exn_struct _tmpC6A;struct Cyc_Toc_Match_error_exn_struct*_tmpC69;(int)_throw((void*)((_tmpC69=_cycalloc_atomic(sizeof(*_tmpC69)),((_tmpC69[0]=((_tmpC6A.tag=Cyc_Toc_Match_error,_tmpC6A)),_tmpC69)))));}}_LL4BB:;}{
# 4195
void*_tmp709=(void*)_check_null(_tmp703->topt);
void*_tmp70A=Cyc_Toc_typ_to_c_array(_tmp709);
struct Cyc_Absyn_Exp*_tmp70B=Cyc_Toc_member_exp(path,f,0);
# 4199
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp701->impl))->tagged)_tmp70B=Cyc_Toc_member_exp(_tmp70B,Cyc_Toc_val_sp,0);{
void*_tmp70C=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp701->impl))->fields,f)))->type;
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp70C))
_tmp70B=Cyc_Toc_cast_it(_tmp70A,_tmp70B);else{
if(!Cyc_Toc_is_array_type(_tmp70C) && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp70C)))
# 4206
_tmp70B=Cyc_Absyn_deref_exp(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(_tmp70A,Cyc_Toc_mt_tq),
Cyc_Absyn_address_exp(_tmp70B,0)),0);}
# 4209
s=Cyc_Toc_seq_stmt_opt(s,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp70B,_tmp703));};};};}
# 4211
return s;}}case 15: _LL4B5: _LL4B6: {
# 4216
const char*_tmpC6D;void*_tmpC6C;(_tmpC6C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC6D="unknownid pat",_tag_dyneither(_tmpC6D,sizeof(char),14))),_tag_dyneither(_tmpC6C,sizeof(void*),0)));}case 16: _LL4B7: _LL4B8: {
const char*_tmpC70;void*_tmpC6F;(_tmpC6F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC70="unknowncall pat",_tag_dyneither(_tmpC70,sizeof(char),16))),_tag_dyneither(_tmpC6F,sizeof(void*),0)));}default: _LL4B9: _LL4BA: {
const char*_tmpC73;void*_tmpC72;(_tmpC72=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC73="exp pat",_tag_dyneither(_tmpC73,sizeof(char),8))),_tag_dyneither(_tmpC72,sizeof(void*),0)));}}_LL492:;}struct _tuple35{struct Cyc_Toc_Env*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Stmt*f3;};
# 4227
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 4235
void*_tmp713=dopt;struct Cyc_List_List*_tmp714;struct Cyc_List_List*_tmp715;void*_tmp716;struct Cyc_Tcpat_Rhs*_tmp717;if(_tmp713 == 0){_LL4C1: _LL4C2:
# 4237
 goto _LL4C4;}else{switch(*((int*)_tmp713)){case 0: _LL4C3: _LL4C4:
 return Cyc_Absyn_skip_stmt(0);case 1: _LL4C5: _tmp717=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp713)->f1;_LL4C6:
# 4242
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple31*_tmp718=(struct _tuple31*)lscs->hd;struct _tuple31*_tmp719=_tmp718;int*_tmp71A;struct _dyneither_ptr*_tmp71B;struct _dyneither_ptr*_tmp71C;struct Cyc_Absyn_Switch_clause*_tmp71D;_LL4CA: _tmp71A=(int*)& _tmp719->f1;_tmp71B=_tmp719->f2;_tmp71C=_tmp719->f3;_tmp71D=_tmp719->f4;_LL4CB: {
struct Cyc_Absyn_Stmt*_tmp71E=_tmp71D->body;
if(_tmp71E == _tmp717->rhs){
# 4247
if(*_tmp71A)
return Cyc_Absyn_goto_stmt(_tmp71B,0);
*_tmp71A=1;{
# 4251
struct Cyc_Absyn_Stmt*init_opt=Cyc_Toc_extract_pattern_vars(rgn,& nv,decls,Cyc_Absyn_var_exp(v,0),_tmp71D->pattern);
# 4254
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_label_stmt(_tmp71C,_tmp71D->body,0);
if(init_opt != 0)
res=Cyc_Absyn_seq_stmt(init_opt,res,0);
res=Cyc_Absyn_label_stmt(_tmp71B,res,0);
{struct _tuple35*_tmpC76;struct Cyc_List_List*_tmpC75;*bodies=((_tmpC75=_region_malloc(rgn,sizeof(*_tmpC75)),((_tmpC75->hd=((_tmpC76=_region_malloc(rgn,sizeof(*_tmpC76)),((_tmpC76->f1=nv,((_tmpC76->f2=_tmp71C,((_tmpC76->f3=_tmp71E,_tmpC76)))))))),((_tmpC75->tl=*bodies,_tmpC75))))));}
return res;};}}_LL4C9:;}{
# 4262
const char*_tmpC79;void*_tmpC78;(_tmpC78=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC79="couldn't find rhs!",_tag_dyneither(_tmpC79,sizeof(char),19))),_tag_dyneither(_tmpC78,sizeof(void*),0)));};default: _LL4C7: _tmp714=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp713)->f1;_tmp715=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp713)->f2;_tmp716=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp713)->f3;_LL4C8: {
# 4265
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp716,lscs,v);
# 4267
struct Cyc_Absyn_Exp*_tmp723=Cyc_Toc_compile_path(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp714),Cyc_Absyn_var_exp(v,0));
struct Cyc_List_List*_tmp724=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp715);
# 4270
struct _tuple32 _tmp725=Cyc_Toc_admits_switch(_tmp724);struct _tuple32 _tmp726=_tmp725;int _tmp727;void*_tmp728;_LL4CF: _tmp727=_tmp726.f1;_tmp728=_tmp726.f2;_LL4D0:
 if(_tmp727 > 1){
# 4274
struct Cyc_Absyn_Switch_clause*_tmpC7C;struct Cyc_List_List*_tmpC7B;struct Cyc_List_List*new_lscs=
(_tmpC7B=_cycalloc(sizeof(*_tmpC7B)),((_tmpC7B->hd=((_tmpC7C=_cycalloc(sizeof(*_tmpC7C)),((_tmpC7C->pattern=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0),((_tmpC7C->pat_vars=0,((_tmpC7C->where_clause=0,((_tmpC7C->body=res,((_tmpC7C->loc=0,_tmpC7C)))))))))))),((_tmpC7B->tl=0,_tmpC7B)))));
# 4277
for(0;_tmp724 != 0;_tmp724=_tmp724->tl){
struct _tuple29 _tmp729=*((struct _tuple29*)_tmp724->hd);struct _tuple29 _tmp72A=_tmp729;void*_tmp72B;void*_tmp72C;_LL4D4: _tmp72B=_tmp72A.f1;_tmp72C=_tmp72A.f2;_LL4D5: {
# 4280
struct Cyc_Absyn_Pat*_tmp72D=Cyc_Toc_compile_pat_test_as_case(_tmp72B);
# 4282
struct Cyc_Absyn_Stmt*_tmp72E=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp72C,lscs,v);
# 4284
struct Cyc_Absyn_Switch_clause*_tmpC7F;struct Cyc_List_List*_tmpC7E;new_lscs=((_tmpC7E=_cycalloc(sizeof(*_tmpC7E)),((_tmpC7E->hd=((_tmpC7F=_cycalloc(sizeof(*_tmpC7F)),((_tmpC7F->pattern=_tmp72D,((_tmpC7F->pat_vars=0,((_tmpC7F->where_clause=0,((_tmpC7F->body=_tmp72E,((_tmpC7F->loc=0,_tmpC7F)))))))))))),((_tmpC7E->tl=new_lscs,_tmpC7E))))));}_LL4D3:;}
# 4286
{void*_tmp731=_tmp728;struct _dyneither_ptr*_tmp732;switch(*((int*)_tmp731)){case 1: _LL4D9: _LL4DA:
# 4289
 LOOP1: {
void*_tmp733=_tmp723->r;void*_tmp734=_tmp733;struct Cyc_Absyn_Exp*_tmp735;struct Cyc_Absyn_Exp*_tmp736;switch(*((int*)_tmp734)){case 13: _LL4E2: _tmp736=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp734)->f2;_LL4E3:
 _tmp723=_tmp736;goto LOOP1;case 19: _LL4E4: _tmp735=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp734)->f1;_LL4E5:
 _tmp723=_tmp735;goto _LL4E1;default: _LL4E6: _LL4E7:
 goto _LL4E1;}_LL4E1:;}
# 4295
_tmp723=Cyc_Absyn_deref_exp(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,Cyc_Toc_mt_tq),_tmp723),0);goto _LL4D8;case 3: _LL4DB: _tmp732=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_tmp731)->f1;_LL4DC:
# 4298
 _tmp723=Cyc_Toc_member_exp(Cyc_Toc_member_exp(_tmp723,_tmp732,0),Cyc_Toc_tag_sp,0);goto _LL4D8;case 2: _LL4DD: _LL4DE:
# 4300
 goto _LL4D8;default: _LL4DF: _LL4E0:
 goto _LL4D8;}_LL4D8:;}{
# 4303
struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct _tmpC82;struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmpC81;res=Cyc_Absyn_new_stmt((void*)((_tmpC81=_cycalloc(sizeof(*_tmpC81)),((_tmpC81[0]=((_tmpC82.tag=10,((_tmpC82.f1=_tmp723,((_tmpC82.f2=new_lscs,((_tmpC82.f3=0,_tmpC82)))))))),_tmpC81)))),0);};}else{
# 4307
void*_tmp73B=_tmp728;struct Cyc_Absyn_Exp*_tmp73C;if(((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp73B)->tag == 2){_LL4E9: _tmp73C=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp73B)->f1;_LL4EA: {
# 4309
struct Cyc_List_List*_tmp73D=_tmp724;void*_tmp73E;struct Cyc_Tcpat_Rhs*_tmp73F;if(_tmp73D != 0){if(((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple29*)((struct Cyc_List_List*)_tmp73D)->hd)->f2)->tag == 1){if(((struct Cyc_List_List*)_tmp73D)->tl == 0){_LL4EE: _tmp73E=((struct _tuple29*)_tmp73D->hd)->f1;_tmp73F=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple29*)_tmp73D->hd)->f2)->f1;_LL4EF:
# 4315
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple31*_tmp740=(struct _tuple31*)lscs->hd;struct _tuple31*_tmp741=_tmp740;int*_tmp742;struct _dyneither_ptr*_tmp743;struct _dyneither_ptr*_tmp744;struct Cyc_Absyn_Switch_clause*_tmp745;_LL4F3: _tmp742=(int*)& _tmp741->f1;_tmp743=_tmp741->f2;_tmp744=_tmp741->f3;_tmp745=_tmp741->f4;_LL4F4: {
struct Cyc_Absyn_Stmt*_tmp746=_tmp745->body;
if(_tmp746 == _tmp73F->rhs){
# 4320
if(*_tmp742)
return Cyc_Absyn_goto_stmt(_tmp743,0);
*_tmp742=1;{
# 4324
struct Cyc_Absyn_Stmt*init_opt=Cyc_Toc_extract_pattern_vars(rgn,& nv,decls,Cyc_Absyn_var_exp(v,0),_tmp745->pattern);
# 4327
Cyc_Toc_exp_to_c(nv,_tmp73C);{
# 4330
struct Cyc_Absyn_Stmt*r=Cyc_Absyn_label_stmt(_tmp744,_tmp745->body,0);
r=Cyc_Absyn_ifthenelse_stmt(_tmp73C,r,res,0);
if(init_opt != 0)
r=Cyc_Absyn_seq_stmt(init_opt,r,0);
r=Cyc_Absyn_label_stmt(_tmp743,r,0);
{struct _tuple35*_tmpC85;struct Cyc_List_List*_tmpC84;*bodies=((_tmpC84=_region_malloc(rgn,sizeof(*_tmpC84)),((_tmpC84->hd=((_tmpC85=_region_malloc(rgn,sizeof(*_tmpC85)),((_tmpC85->f1=nv,((_tmpC85->f2=_tmp744,((_tmpC85->f3=_tmp746,_tmpC85)))))))),((_tmpC84->tl=*bodies,_tmpC84))))));}
return r;};};}}_LL4F2:;}{
# 4339
const char*_tmpC88;void*_tmpC87;(_tmpC87=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC88="couldn't find rhs!",_tag_dyneither(_tmpC88,sizeof(char),19))),_tag_dyneither(_tmpC87,sizeof(void*),0)));};}else{goto _LL4F0;}}else{goto _LL4F0;}}else{_LL4F0: _LL4F1: {
const char*_tmpC8B;void*_tmpC8A;(_tmpC8A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC8B="bad where clause in match compiler",_tag_dyneither(_tmpC8B,sizeof(char),35))),_tag_dyneither(_tmpC8A,sizeof(void*),0)));}}_LL4ED:;}}else{_LL4EB: _LL4EC:
# 4344
 for(0;_tmp724 != 0;_tmp724=_tmp724->tl){
struct _tuple29 _tmp74D=*((struct _tuple29*)_tmp724->hd);struct _tuple29 _tmp74E=_tmp74D;void*_tmp74F;void*_tmp750;_LL4F8: _tmp74F=_tmp74E.f1;_tmp750=_tmp74E.f2;_LL4F9: {
struct Cyc_Absyn_Exp*_tmp751=Cyc_Toc_compile_pat_test(_tmp723,_tmp74F);
struct Cyc_Absyn_Stmt*_tmp752=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp750,lscs,v);
res=Cyc_Absyn_ifthenelse_stmt(_tmp751,_tmp752,res,0);}_LL4F7:;}}_LL4E8:;}
# 4352
return res;_LL4CE:;}}}_LL4C0:;}
# 4362
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 4364
for(0;bodies != 0;bodies=bodies->tl){
struct _tuple35*_tmp753=(struct _tuple35*)bodies->hd;struct _tuple35*_tmp754=_tmp753;struct Cyc_Toc_Env**_tmp755;struct Cyc_Absyn_Stmt*_tmp756;_LL4FD: _tmp755=(struct Cyc_Toc_Env**)& _tmp754->f1;_tmp756=_tmp754->f3;_LL4FE:
 if(_tmp756 == s)return _tmp755;_LL4FC:;}
# 4370
return 0;}
# 4374
static void Cyc_Toc_xlate_switch(int nested_fallthru,struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 4377
Cyc_Toc_exp_to_c(nv,e);{
void*_tmp757=(void*)_check_null(e->topt);
# 4380
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);
r->topt=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));{
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp758=_new_region("rgn");struct _RegionHandle*rgn=& _tmp758;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp759=Cyc_Toc_share_env(rgn,nv);
# 4389
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple31*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 4394
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp759,& mydecls,& mybodies,dopt,lscs,v);
# 4402
{struct Cyc_List_List*_tmp75A=lscs;for(0;_tmp75A != 0;_tmp75A=_tmp75A->tl){
struct _tuple31*_tmp75B=(struct _tuple31*)_tmp75A->hd;struct _tuple31*_tmp75C=_tmp75B;struct _dyneither_ptr*_tmp75D;struct Cyc_Absyn_Switch_clause*_tmp75E;_LL502: _tmp75D=_tmp75C->f3;_tmp75E=_tmp75C->f4;_LL503: {
struct Cyc_Absyn_Stmt*s=_tmp75E->body;
# 4406
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 4409
if(envp == 0)continue;{
struct Cyc_Toc_Env*_tmp75F=*envp;
if(!nested_fallthru)
# 4414
Cyc_Toc_stmt_to_c(_tmp75F,s);else{
# 4417
if(_tmp75A->tl != 0){
struct _tuple31*_tmp760=(struct _tuple31*)((struct Cyc_List_List*)_check_null(_tmp75A->tl))->hd;struct _tuple31*_tmp761=_tmp760;struct _dyneither_ptr*_tmp762;struct Cyc_Absyn_Switch_clause*_tmp763;_LL507: _tmp762=_tmp761->f3;_tmp763=_tmp761->f4;_LL508: {
struct Cyc_Toc_Env**_tmp764=Cyc_Toc_find_case_env(mybodies,_tmp763->body);
# 4424
if(_tmp764 == 0)
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn,_tmp75F,end_l),s);else{
# 4428
struct Cyc_List_List*vs=0;
if(_tmp763->pat_vars != 0){
vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp763->pat_vars))->v)).f1);
vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vs);}
# 4433
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(rgn,_tmp75F,end_l,_tmp762,vs,*_tmp764),s);}}_LL506:;}else{
# 4437
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn,_tmp75F,end_l),s);}}};}_LL501:;}}{
# 4441
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(test_tree,Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),0),0);
# 4443
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct _tuple33*_tmp765=(struct _tuple33*)((struct Cyc_List_List*)_check_null(mydecls))->hd;struct _tuple33*_tmp766=_tmp765;struct _tuple1*_tmp767;void*_tmp768;_LL50C: _tmp767=_tmp766->f1;_tmp768=_tmp766->f2;_LL50D:
 res=Cyc_Absyn_declare_stmt(_tmp767,_tmp768,0,res,0);_LL50B:;}
# 4448
whole_s->r=(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),e,res,0))->r;
_npop_handler(0);return;};}
# 4386
;_pop_region(rgn);};};}
# 4454
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
# 4456
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 4461
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 4463
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
struct Cyc_List_List*_tmpC8C;return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,(
(_tmpC8C=_cycalloc(sizeof(*_tmpC8C)),((_tmpC8C->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0),((_tmpC8C->tl=0,_tmpC8C)))))),0),0);}
# 4467
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt(s->r,0));}
# 4472
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 4474
while(1){
void*_tmp76A=s->r;void*_tmp76B=_tmp76A;struct Cyc_Absyn_Exp*_tmp76C;struct Cyc_Absyn_Stmt*_tmp76D;struct Cyc_List_List*_tmp76E;void*_tmp76F;struct Cyc_Absyn_Stmt*_tmp770;struct Cyc_Absyn_Exp*_tmp771;struct _dyneither_ptr*_tmp772;struct Cyc_Absyn_Stmt*_tmp773;struct Cyc_Absyn_Decl*_tmp774;struct Cyc_Absyn_Stmt*_tmp775;struct Cyc_List_List*_tmp776;struct Cyc_Absyn_Switch_clause**_tmp777;struct Cyc_Absyn_Exp*_tmp778;struct Cyc_List_List*_tmp779;void*_tmp77A;struct Cyc_Absyn_Exp*_tmp77B;struct Cyc_Absyn_Exp*_tmp77C;struct Cyc_Absyn_Exp*_tmp77D;struct Cyc_Absyn_Stmt*_tmp77E;struct Cyc_Absyn_Stmt*_tmp77F;struct Cyc_Absyn_Stmt*_tmp780;struct Cyc_Absyn_Stmt*_tmp781;struct Cyc_Absyn_Exp*_tmp782;struct Cyc_Absyn_Stmt*_tmp783;struct Cyc_Absyn_Exp*_tmp784;struct Cyc_Absyn_Stmt*_tmp785;struct Cyc_Absyn_Stmt*_tmp786;struct Cyc_Absyn_Exp*_tmp787;struct Cyc_Absyn_Stmt*_tmp788;struct Cyc_Absyn_Stmt*_tmp789;struct Cyc_Absyn_Exp*_tmp78A;switch(*((int*)_tmp76B)){case 0: _LL511: _LL512:
# 4477
 return;case 1: _LL513: _tmp78A=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_LL514:
# 4479
 Cyc_Toc_exp_to_c(nv,_tmp78A);
return;case 2: _LL515: _tmp788=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_tmp789=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp76B)->f2;_LL516:
# 4482
 Cyc_Toc_stmt_to_c(nv,_tmp788);
s=_tmp789;
continue;case 3: _LL517: _tmp787=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_LL518: {
# 4486
void*topt=0;
if(_tmp787 != 0){
topt=Cyc_Toc_typ_to_c((void*)_check_null(_tmp787->topt));
Cyc_Toc_exp_to_c(nv,_tmp787);}
# 4492
if(s->try_depth > 0){
if(topt != 0){
struct _tuple1*_tmp78B=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp78C=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp78B,0),0);
s->r=(Cyc_Absyn_declare_stmt(_tmp78B,topt,_tmp787,
Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(s->try_depth),_tmp78C,0),0))->r;}else{
# 4500
Cyc_Toc_do_npop_before(s->try_depth,s);}}
# 4502
return;}case 4: _LL519: _tmp784=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_tmp785=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp76B)->f2;_tmp786=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp76B)->f3;_LL51A:
# 4504
 Cyc_Toc_exp_to_c(nv,_tmp784);
Cyc_Toc_stmt_to_c(nv,_tmp785);
s=_tmp786;
continue;case 5: _LL51B: _tmp782=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1).f1;_tmp783=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp76B)->f2;_LL51C:
# 4509
 Cyc_Toc_exp_to_c(nv,_tmp782);{
struct _RegionHandle _tmp78D=_new_region("temp");struct _RegionHandle*temp=& _tmp78D;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp783);
# 4513
_npop_handler(0);return;
# 4510
;_pop_region(temp);};case 6: _LL51D: _tmp781=((struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_LL51E: {
# 4515
struct Cyc_Toc_Env*_tmp78E=nv;struct _dyneither_ptr**_tmp78F;_LL534: _tmp78F=_tmp78E->break_lab;_LL535:
 if(_tmp78F != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp78F,0);{
# 4519
int dest_depth=_tmp781 == 0?0: _tmp781->try_depth;
Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);
return;};_LL533:;}case 7: _LL51F: _tmp780=((struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_LL520: {
# 4523
struct Cyc_Toc_Env*_tmp790=nv;struct _dyneither_ptr**_tmp791;_LL539: _tmp791=_tmp790->continue_lab;_LL53A:
 if(_tmp791 != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp791,0);
_tmp77F=_tmp780;goto _LL522;_LL538:;}case 8: _LL521: _tmp77F=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp76B)->f2;_LL522:
# 4529
 Cyc_Toc_do_npop_before(s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp77F))->try_depth,s);
return;case 9: _LL523: _tmp77B=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_tmp77C=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp76B)->f2).f1;_tmp77D=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp76B)->f3).f1;_tmp77E=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp76B)->f4;_LL524:
# 4533
 Cyc_Toc_exp_to_c(nv,_tmp77B);Cyc_Toc_exp_to_c(nv,_tmp77C);Cyc_Toc_exp_to_c(nv,_tmp77D);{
struct _RegionHandle _tmp792=_new_region("temp");struct _RegionHandle*temp=& _tmp792;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp77E);
# 4537
_npop_handler(0);return;
# 4534
;_pop_region(temp);};case 10: _LL525: _tmp778=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_tmp779=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp76B)->f2;_tmp77A=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp76B)->f3;_LL526:
# 4539
 Cyc_Toc_xlate_switch(1,nv,s,_tmp778,_tmp779,_tmp77A);
return;case 11: _LL527: _tmp776=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_tmp777=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp76B)->f2;_LL528: {
# 4542
struct Cyc_Toc_Env*_tmp793=nv;struct Cyc_Toc_FallthruInfo*_tmp794;_LL53E: _tmp794=_tmp793->fallthru_info;_LL53F:
 if(_tmp794 == 0){
const char*_tmpC8F;void*_tmpC8E;(_tmpC8E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC8F="fallthru in unexpected place",_tag_dyneither(_tmpC8F,sizeof(char),29))),_tag_dyneither(_tmpC8E,sizeof(void*),0)));}{
struct Cyc_Toc_FallthruInfo _tmp797=*_tmp794;struct Cyc_Toc_FallthruInfo _tmp798=_tmp797;struct _dyneither_ptr*_tmp799;struct Cyc_List_List*_tmp79A;struct Cyc_Dict_Dict _tmp79B;_LL543: _tmp799=_tmp798.label;_tmp79A=_tmp798.binders;_tmp79B=_tmp798.next_case_env;_LL544: {
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp799,0);
# 4548
Cyc_Toc_do_npop_before(s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp777)))->body)->try_depth,s2);{
struct Cyc_List_List*_tmp79C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp79A);
struct Cyc_List_List*_tmp79D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp776);
for(0;_tmp79C != 0;(_tmp79C=_tmp79C->tl,_tmp79D=_tmp79D->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp79D))->hd);
s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp79B,(struct _tuple1*)_tmp79C->hd),(struct Cyc_Absyn_Exp*)_tmp79D->hd,0),s2,0);}
# 4556
s->r=s2->r;
return;};}_LL542:;};_LL53D:;}case 12: _LL529: _tmp774=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_tmp775=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp76B)->f2;_LL52A:
# 4562
{void*_tmp79E=_tmp774->r;void*_tmp79F=_tmp79E;struct Cyc_Absyn_Tvar*_tmp7A0;struct Cyc_Absyn_Vardecl*_tmp7A1;int _tmp7A2;struct Cyc_Absyn_Exp*_tmp7A3;struct Cyc_Absyn_Fndecl*_tmp7A4;struct Cyc_List_List*_tmp7A5;struct Cyc_Absyn_Pat*_tmp7A6;struct Cyc_Absyn_Exp*_tmp7A7;void*_tmp7A8;struct Cyc_Absyn_Vardecl*_tmp7A9;switch(*((int*)_tmp79F)){case 0: _LL548: _tmp7A9=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp79F)->f1;_LL549: {
# 4564
struct _RegionHandle _tmp7AA=_new_region("temp");struct _RegionHandle*temp=& _tmp7AA;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpC92;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpC91;struct Cyc_Toc_Env*_tmp7AB=Cyc_Toc_add_varmap(temp,nv,_tmp7A9->name,
Cyc_Absyn_varb_exp(_tmp7A9->name,(void*)((_tmpC91=_cycalloc(sizeof(*_tmpC91)),((_tmpC91[0]=((_tmpC92.tag=4,((_tmpC92.f1=_tmp7A9,_tmpC92)))),_tmpC91)))),0));
Cyc_Toc_local_decl_to_c(_tmp7AB,_tmp7AB,_tmp7A9,_tmp775);}
# 4569
_npop_handler(0);goto _LL547;
# 4564
;_pop_region(temp);}case 2: _LL54A: _tmp7A6=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp79F)->f1;_tmp7A7=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp79F)->f3;_tmp7A8=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp79F)->f4;_LL54B:
# 4573
{void*_tmp7AE=_tmp7A6->r;void*_tmp7AF=_tmp7AE;struct Cyc_Absyn_Vardecl*_tmp7B0;if(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp7AF)->tag == 1){if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp7AF)->f2)->r)->tag == 0){_LL555: _tmp7B0=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp7AF)->f1;_LL556: {
# 4575
struct _tuple1*old_name=_tmp7B0->name;
struct _tuple1*new_name=Cyc_Toc_temp_var();
_tmp7B0->name=new_name;
_tmp7B0->initializer=_tmp7A7;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpC95;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpC94;_tmp774->r=(void*)((_tmpC94=_cycalloc(sizeof(*_tmpC94)),((_tmpC94[0]=((_tmpC95.tag=0,((_tmpC95.f1=_tmp7B0,_tmpC95)))),_tmpC94))));}{
struct _RegionHandle _tmp7B3=_new_region("temp");struct _RegionHandle*temp=& _tmp7B3;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpC98;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpC97;struct Cyc_Toc_Env*_tmp7B4=
Cyc_Toc_add_varmap(temp,nv,old_name,
Cyc_Absyn_varb_exp(new_name,(void*)((_tmpC97=_cycalloc(sizeof(*_tmpC97)),((_tmpC97[0]=((_tmpC98.tag=4,((_tmpC98.f1=_tmp7B0,_tmpC98)))),_tmpC97)))),0));
Cyc_Toc_local_decl_to_c(_tmp7B4,nv,_tmp7B0,_tmp775);}
# 4586
_npop_handler(0);goto _LL554;
# 4580
;_pop_region(temp);};}}else{goto _LL557;}}else{_LL557: _LL558:
# 4591
 s->r=(Cyc_Toc_letdecl_to_c(nv,_tmp7A6,_tmp7A8,(void*)_check_null(_tmp7A7->topt),_tmp7A7,_tmp775))->r;
goto _LL554;}_LL554:;}
# 4594
goto _LL547;case 3: _LL54C: _tmp7A5=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp79F)->f1;_LL54D: {
# 4607 "toc.cyc"
struct Cyc_List_List*_tmp7B7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7A5);
if(_tmp7B7 == 0){
const char*_tmpC9B;void*_tmpC9A;(_tmpC9A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC9B="empty Letv_d",_tag_dyneither(_tmpC9B,sizeof(char),13))),_tag_dyneither(_tmpC9A,sizeof(void*),0)));}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpC9E;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpC9D;_tmp774->r=(void*)((_tmpC9D=_cycalloc(sizeof(*_tmpC9D)),((_tmpC9D[0]=((_tmpC9E.tag=0,((_tmpC9E.f1=(struct Cyc_Absyn_Vardecl*)_tmp7B7->hd,_tmpC9E)))),_tmpC9D))));}
_tmp7B7=_tmp7B7->tl;
for(0;_tmp7B7 != 0;_tmp7B7=_tmp7B7->tl){
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpCA1;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpCA0;struct Cyc_Absyn_Decl*_tmp7BC=Cyc_Absyn_new_decl((void*)((_tmpCA0=_cycalloc(sizeof(*_tmpCA0)),((_tmpCA0[0]=((_tmpCA1.tag=0,((_tmpCA1.f1=(struct Cyc_Absyn_Vardecl*)_tmp7B7->hd,_tmpCA1)))),_tmpCA0)))),0);
s->r=(Cyc_Absyn_decl_stmt(_tmp7BC,Cyc_Absyn_new_stmt(s->r,0),0))->r;}
# 4616
Cyc_Toc_stmt_to_c(nv,s);
goto _LL547;}case 1: _LL54E: _tmp7A4=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp79F)->f1;_LL54F: {
# 4619
struct _tuple1*_tmp7BF=_tmp7A4->name;
struct _RegionHandle _tmp7C0=_new_region("temp");struct _RegionHandle*temp=& _tmp7C0;_push_region(temp);{
struct Cyc_Toc_Env*_tmp7C1=Cyc_Toc_add_varmap(temp,nv,_tmp7A4->name,Cyc_Absyn_var_exp(_tmp7BF,0));
Cyc_Toc_fndecl_to_c(_tmp7C1,_tmp7A4,0);
Cyc_Toc_stmt_to_c(_tmp7C1,_tmp775);}
# 4625
_npop_handler(0);goto _LL547;
# 4620
;_pop_region(temp);}case 4: _LL550: _tmp7A0=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp79F)->f1;_tmp7A1=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp79F)->f2;_tmp7A2=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp79F)->f3;_tmp7A3=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp79F)->f4;_LL551: {
# 4627
struct Cyc_Absyn_Stmt*_tmp7C2=_tmp775;
# 4635
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple1*rh_var=Cyc_Toc_temp_var();
struct _tuple1*x_var=_tmp7A1->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);
# 4642
struct _RegionHandle _tmp7C3=_new_region("temp");struct _RegionHandle*temp=& _tmp7C3;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(temp,nv,x_var,x_exp),_tmp7C2);
# 4645
if(Cyc_Absyn_no_regions)
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_uint_exp(0,0),_tmp7C2,0))->r;else{
if((unsigned int)_tmp7A3){
Cyc_Toc_exp_to_c(nv,_tmp7A3);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(Cyc_Absyn_aggrarrow_exp(_tmp7A3,_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"h",sizeof(char),2),0),0);
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,_tmp7C2,0))->r;};}else{
# 4653
struct Cyc_Absyn_Exp*_tmpCA4[1];struct Cyc_Absyn_Exp*_tmpCA3[1];struct Cyc_List_List*_tmpCA2;s->r=(
Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,(
(_tmpCA2=_cycalloc(sizeof(*_tmpCA2)),((_tmpCA2->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0),((_tmpCA2->tl=0,_tmpCA2)))))),0),
# 4658
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpCA3[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCA3,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),
Cyc_Absyn_seq_stmt(_tmp7C2,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpCA4[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCA4,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}}
# 4664
_npop_handler(0);return;
# 4642
;_pop_region(temp);}default: _LL552: _LL553: {
# 4665
const char*_tmpCA7;void*_tmpCA6;(_tmpCA6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCA7="bad nested declaration within function",_tag_dyneither(_tmpCA7,sizeof(char),39))),_tag_dyneither(_tmpCA6,sizeof(void*),0)));}}_LL547:;}
# 4667
return;case 13: _LL52B: _tmp772=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_tmp773=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp76B)->f2;_LL52C:
# 4669
 s=_tmp773;continue;case 14: _LL52D: _tmp770=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_tmp771=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp76B)->f2).f1;_LL52E: {
# 4671
struct _RegionHandle _tmp7CA=_new_region("temp");struct _RegionHandle*temp=& _tmp7CA;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp770);
Cyc_Toc_exp_to_c(nv,_tmp771);
# 4675
_npop_handler(0);return;
# 4671
;_pop_region(temp);}case 15: _LL52F: _tmp76D=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_tmp76E=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp76B)->f2;_tmp76F=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp76B)->f3;_LL530: {
# 4689 "toc.cyc"
struct _tuple1*h_var=Cyc_Toc_temp_var();
struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
# 4700
e_exp->topt=e_typ;{
struct _RegionHandle _tmp7CB=_new_region("temp");struct _RegionHandle*temp=& _tmp7CB;_push_region(temp);{
struct Cyc_Toc_Env*_tmp7CC=Cyc_Toc_add_varmap(temp,nv,e_var,e_exp);
# 4704
Cyc_Toc_stmt_to_c(_tmp7CC,_tmp76D);{
struct Cyc_Absyn_Stmt*_tmp7CD=Cyc_Absyn_seq_stmt(_tmp76D,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
# 4709
struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct _tmpCAA;struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmpCA9;struct Cyc_Absyn_Stmt*_tmp7CE=Cyc_Absyn_new_stmt((void*)((_tmpCA9=_cycalloc(sizeof(*_tmpCA9)),((_tmpCA9[0]=((_tmpCAA.tag=10,((_tmpCAA.f1=e_exp,((_tmpCAA.f2=_tmp76E,((_tmpCAA.f3=_tmp76F,_tmpCAA)))))))),_tmpCA9)))),0);
# 4711
Cyc_Toc_stmt_to_c(_tmp7CC,_tmp7CE);{
# 4714
struct Cyc_List_List*_tmpCAB;struct Cyc_Absyn_Exp*_tmp7CF=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,(
(_tmpCAB=_cycalloc(sizeof(*_tmpCAB)),((_tmpCAB->hd=Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0),((_tmpCAB->tl=0,_tmpCAB)))))),0);
# 4718
struct Cyc_List_List*_tmpCAC;struct Cyc_Absyn_Stmt*_tmp7D0=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,(
(_tmpCAC=_cycalloc(sizeof(*_tmpCAC)),((_tmpCAC->hd=Cyc_Absyn_address_exp(h_exp,0),((_tmpCAC->tl=0,_tmpCAC)))))),0),0);
# 4722
struct Cyc_Absyn_Exp*_tmp7D1=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0);
struct Cyc_Absyn_Exp*_tmp7D2=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0);
s->r=(
Cyc_Absyn_declare_stmt(h_var,h_typ,0,
Cyc_Absyn_seq_stmt(_tmp7D0,
Cyc_Absyn_declare_stmt(was_thrown_var,was_thrown_typ,_tmp7D1,
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp7CF,
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp7D2,0),
Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0),_tmp7CD,
# 4733
Cyc_Absyn_declare_stmt(e_var,e_typ,
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp7CE,0),0),0),0),0),0))->r;};};}
# 4737
_npop_handler(0);return;
# 4701
;_pop_region(temp);};}default: _LL531: _tmp76C=((struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_LL532:
# 4739
 if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{
# 4742
Cyc_Toc_exp_to_c(nv,_tmp76C);{
struct Cyc_List_List*_tmpCAD;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((_tmpCAD=_cycalloc(sizeof(*_tmpCAD)),((_tmpCAD->hd=_tmp76C,((_tmpCAD->tl=0,_tmpCAD)))))),0));};}
# 4745
return;}_LL510:;}}
# 4754
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->requires_clause=0;
f->ensures_clause=0;
f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp7D8=_new_region("frgn");struct _RegionHandle*frgn=& _tmp7D8;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp7D9=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp7DA=f->args;for(0;_tmp7DA != 0;_tmp7DA=_tmp7DA->tl){
struct _tuple1*_tmpCAE;struct _tuple1*_tmp7DB=(_tmpCAE=_cycalloc(sizeof(*_tmpCAE)),((_tmpCAE->f1=Cyc_Absyn_Loc_n,((_tmpCAE->f2=(*((struct _tuple9*)_tmp7DA->hd)).f1,_tmpCAE)))));
(*((struct _tuple9*)_tmp7DA->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp7DA->hd)).f3);
_tmp7D9=Cyc_Toc_add_varmap(frgn,_tmp7D9,_tmp7DB,Cyc_Absyn_var_exp(_tmp7DB,0));}}
# 4771
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}
# 4775
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp7DD=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp7DE=_tmp7DD;struct _dyneither_ptr*_tmp7DF;struct Cyc_Absyn_Tqual _tmp7E0;void*_tmp7E1;int _tmp7E2;_LL55A: _tmp7DF=_tmp7DE.name;_tmp7E0=_tmp7DE.tq;_tmp7E1=_tmp7DE.type;_tmp7E2=_tmp7DE.inject;_LL55B: {
void*_tmp7E3=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp7E1,(void*)& Cyc_Absyn_HeapRgn_val,_tmp7E0,Cyc_Absyn_false_conref));
struct _tuple1*_tmpCAF;struct _tuple1*_tmp7E4=(_tmpCAF=_cycalloc(sizeof(*_tmpCAF)),((_tmpCAF->f1=Cyc_Absyn_Loc_n,((_tmpCAF->f2=(struct _dyneither_ptr*)_check_null(_tmp7DF),_tmpCAF)))));
{struct _tuple9*_tmpCB2;struct Cyc_List_List*_tmpCB1;f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpCB1=_cycalloc(sizeof(*_tmpCB1)),((_tmpCB1->hd=((_tmpCB2=_cycalloc(sizeof(*_tmpCB2)),((_tmpCB2->f1=_tmp7DF,((_tmpCB2->f2=_tmp7E0,((_tmpCB2->f3=_tmp7E3,_tmpCB2)))))))),((_tmpCB1->tl=0,_tmpCB1)))))));}
_tmp7D9=Cyc_Toc_add_varmap(frgn,_tmp7D9,_tmp7E4,Cyc_Absyn_var_exp(_tmp7E4,0));
f->cyc_varargs=0;}_LL559:;}
# 4784
{struct Cyc_List_List*_tmp7E8=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp7E8 != 0;_tmp7E8=_tmp7E8->tl){
((struct Cyc_Absyn_Vardecl*)_tmp7E8->hd)->type=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp7E8->hd)->type);}}
# 4787
Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp7D9),f->body);}
# 4763
;_pop_region(frgn);};}
# 4792
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
enum Cyc_Absyn_Scope _tmp7E9=s;switch(_tmp7E9){case Cyc_Absyn_Abstract: _LL55F: _LL560:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL561: _LL562:
 return Cyc_Absyn_Extern;default: _LL563: _LL564:
 return s;}_LL55E:;}struct _tuple36{struct Cyc_Absyn_Aggrdecl*f1;int f2;};struct _tuple37{struct Cyc_Toc_TocState*f1;struct _tuple36*f2;};
# 4809 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple37*env){
# 4812
struct _tuple37 _tmp7EA=*env;struct _tuple37 _tmp7EB=_tmp7EA;struct Cyc_Toc_TocState*_tmp7EC;struct Cyc_Absyn_Aggrdecl*_tmp7ED;int _tmp7EE;_LL566: _tmp7EC=_tmp7EB.f1;_tmp7ED=(_tmp7EB.f2)->f1;_tmp7EE=(_tmp7EB.f2)->f2;_LL567: {
struct _tuple1*_tmp7EF=_tmp7ED->name;
struct Cyc_Toc_TocState _tmp7F0=*_tmp7EC;struct Cyc_Toc_TocState _tmp7F1=_tmp7F0;struct Cyc_Dict_Dict*_tmp7F2;_LL56B: _tmp7F2=_tmp7F1.aggrs_so_far;_LL56C: {
int seen_defn_before;
struct _tuple18**_tmp7F3=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp7F2,_tmp7EF);
if(_tmp7F3 == 0){
seen_defn_before=0;{
struct _tuple18*v;
if(_tmp7ED->kind == Cyc_Absyn_StructA){
struct _tuple18*_tmpCB3;v=((_tmpCB3=_region_malloc(d,sizeof(*_tmpCB3)),((_tmpCB3->f1=_tmp7ED,((_tmpCB3->f2=Cyc_Absyn_strctq(_tmp7EF),_tmpCB3))))));}else{
# 4823
struct _tuple18*_tmpCB4;v=((_tmpCB4=_region_malloc(d,sizeof(*_tmpCB4)),((_tmpCB4->f1=_tmp7ED,((_tmpCB4->f2=Cyc_Absyn_unionq_typ(_tmp7EF),_tmpCB4))))));}
*_tmp7F2=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp7F2,_tmp7EF,v);};}else{
# 4826
struct _tuple18*_tmp7F6=*_tmp7F3;struct _tuple18*_tmp7F7=_tmp7F6;struct Cyc_Absyn_Aggrdecl*_tmp7F8;void*_tmp7F9;_LL570: _tmp7F8=_tmp7F7->f1;_tmp7F9=_tmp7F7->f2;_LL571:
 if(_tmp7F8->impl == 0){
{struct _tuple18*_tmpCB5;*_tmp7F2=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp7F2,_tmp7EF,((_tmpCB5=_region_malloc(d,sizeof(*_tmpCB5)),((_tmpCB5->f1=_tmp7ED,((_tmpCB5->f2=_tmp7F9,_tmpCB5)))))));}
seen_defn_before=0;}else{
# 4831
seen_defn_before=1;}_LL56F:;}{
# 4833
struct Cyc_Absyn_Aggrdecl*_tmpCB6;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpCB6=_cycalloc(sizeof(*_tmpCB6)),((_tmpCB6->kind=_tmp7ED->kind,((_tmpCB6->sc=Cyc_Absyn_Public,((_tmpCB6->name=_tmp7ED->name,((_tmpCB6->tvs=0,((_tmpCB6->impl=0,((_tmpCB6->expected_mem_kind=0,((_tmpCB6->attributes=_tmp7ED->attributes,_tmpCB6)))))))))))))));
# 4840
if(_tmp7ED->impl != 0  && !seen_defn_before){
{struct Cyc_Absyn_AggrdeclImpl*_tmpCB7;new_ad->impl=((_tmpCB7=_cycalloc(sizeof(*_tmpCB7)),((_tmpCB7->exist_vars=0,((_tmpCB7->rgn_po=0,((_tmpCB7->fields=0,((_tmpCB7->tagged=0,_tmpCB7))))))))));}{
# 4845
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp7FC=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7ED->impl))->fields;for(0;_tmp7FC != 0;_tmp7FC=_tmp7FC->tl){
# 4849
struct Cyc_Absyn_Aggrfield*_tmp7FD=(struct Cyc_Absyn_Aggrfield*)_tmp7FC->hd;
void*_tmp7FE=_tmp7FD->type;
struct Cyc_List_List*_tmp7FF=_tmp7FD->attributes;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp7FE)) && (
_tmp7ED->kind == Cyc_Absyn_StructA  && _tmp7FC->tl == 0  || _tmp7ED->kind == Cyc_Absyn_UnionA)){
# 4863 "toc.cyc"
void*_tmp800=Cyc_Tcutil_compress(_tmp7FE);void*_tmp801=_tmp800;void*_tmp802;struct Cyc_Absyn_Tqual _tmp803;union Cyc_Absyn_Constraint*_tmp804;unsigned int _tmp805;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp801)->tag == 8){_LL575: _tmp802=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp801)->f1).elt_type;_tmp803=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp801)->f1).tq;_tmp804=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp801)->f1).zero_term;_tmp805=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp801)->f1).zt_loc;_LL576:
# 4866
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpCBD;struct Cyc_Absyn_ArrayInfo _tmpCBC;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCBB;_tmp7FE=(void*)((_tmpCBB=_cycalloc(sizeof(*_tmpCBB)),((_tmpCBB[0]=((_tmpCBD.tag=8,((_tmpCBD.f1=((_tmpCBC.elt_type=_tmp802,((_tmpCBC.tq=_tmp803,((_tmpCBC.num_elts=Cyc_Absyn_uint_exp(0,0),((_tmpCBC.zero_term=_tmp804,((_tmpCBC.zt_loc=_tmp805,_tmpCBC)))))))))),_tmpCBD)))),_tmpCBB))));}
goto _LL574;}else{_LL577: _LL578:
# 4869
{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpCC3;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpCC2;struct Cyc_List_List*_tmpCC1;_tmp7FF=((_tmpCC1=_cycalloc(sizeof(*_tmpCC1)),((_tmpCC1->hd=(void*)((_tmpCC3=_cycalloc(sizeof(*_tmpCC3)),((_tmpCC3[0]=((_tmpCC2.tag=6,((_tmpCC2.f1=0,_tmpCC2)))),_tmpCC3)))),((_tmpCC1->tl=_tmp7FF,_tmpCC1))))));}{
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpCC9;struct Cyc_Absyn_ArrayInfo _tmpCC8;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCC7;_tmp7FE=(void*)((_tmpCC7=_cycalloc(sizeof(*_tmpCC7)),((_tmpCC7[0]=((_tmpCC9.tag=8,((_tmpCC9.f1=((_tmpCC8.elt_type=Cyc_Absyn_void_star_typ(),((_tmpCC8.tq=
Cyc_Absyn_empty_tqual(0),((_tmpCC8.num_elts=
Cyc_Absyn_uint_exp(0,0),((_tmpCC8.zero_term=
((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0),((_tmpCC8.zt_loc=0,_tmpCC8)))))))))),_tmpCC9)))),_tmpCC7))));};}_LL574:;}{
# 4876
struct Cyc_Absyn_Aggrfield*_tmpCCA;struct Cyc_Absyn_Aggrfield*_tmp80F=(_tmpCCA=_cycalloc(sizeof(*_tmpCCA)),((_tmpCCA->name=_tmp7FD->name,((_tmpCCA->tq=Cyc_Toc_mt_tq,((_tmpCCA->type=
# 4878
Cyc_Toc_typ_to_c(_tmp7FE),((_tmpCCA->width=_tmp7FD->width,((_tmpCCA->attributes=_tmp7FF,((_tmpCCA->requires_clause=0,_tmpCCA)))))))))))));
# 4886
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7ED->impl))->tagged){
void*_tmp810=_tmp80F->type;
struct _dyneither_ptr*_tmp811=_tmp80F->name;
const char*_tmpCCF;void*_tmpCCE[2];struct Cyc_String_pa_PrintArg_struct _tmpCCD;struct Cyc_String_pa_PrintArg_struct _tmpCCC;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpCCC.tag=0,((_tmpCCC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp811),((_tmpCCD.tag=0,((_tmpCCD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp7ED->name).f2),((_tmpCCE[0]=& _tmpCCD,((_tmpCCE[1]=& _tmpCCC,Cyc_aprintf(((_tmpCCF="_union_%s_%s",_tag_dyneither(_tmpCCF,sizeof(char),13))),_tag_dyneither(_tmpCCE,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpCD0;struct _dyneither_ptr*str=(_tmpCD0=_cycalloc(sizeof(*_tmpCD0)),((_tmpCD0[0]=s,_tmpCD0)));
struct Cyc_Absyn_Aggrfield*_tmpCD1;struct Cyc_Absyn_Aggrfield*_tmp812=(_tmpCD1=_cycalloc(sizeof(*_tmpCD1)),((_tmpCD1->name=Cyc_Toc_val_sp,((_tmpCD1->tq=Cyc_Toc_mt_tq,((_tmpCD1->type=_tmp810,((_tmpCD1->width=0,((_tmpCD1->attributes=0,((_tmpCD1->requires_clause=0,_tmpCD1)))))))))))));
struct Cyc_Absyn_Aggrfield*_tmpCD2;struct Cyc_Absyn_Aggrfield*_tmp813=(_tmpCD2=_cycalloc(sizeof(*_tmpCD2)),((_tmpCD2->name=Cyc_Toc_tag_sp,((_tmpCD2->tq=Cyc_Toc_mt_tq,((_tmpCD2->type=Cyc_Absyn_sint_typ,((_tmpCD2->width=0,((_tmpCD2->attributes=0,((_tmpCD2->requires_clause=0,_tmpCD2)))))))))))));
struct Cyc_Absyn_Aggrfield*_tmpCD3[2];struct Cyc_List_List*_tmp814=(_tmpCD3[1]=_tmp812,((_tmpCD3[0]=_tmp813,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCD3,sizeof(struct Cyc_Absyn_Aggrfield*),2)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpCD8;struct _tuple1*_tmpCD7;struct Cyc_Absyn_Aggrdecl*_tmpCD6;struct Cyc_Absyn_Aggrdecl*_tmp815=(_tmpCD6=_cycalloc(sizeof(*_tmpCD6)),((_tmpCD6->kind=Cyc_Absyn_StructA,((_tmpCD6->sc=Cyc_Absyn_Public,((_tmpCD6->name=(
(_tmpCD7=_cycalloc(sizeof(*_tmpCD7)),((_tmpCD7->f1=Cyc_Absyn_Loc_n,((_tmpCD7->f2=str,_tmpCD7)))))),((_tmpCD6->tvs=0,((_tmpCD6->impl=(
(_tmpCD8=_cycalloc(sizeof(*_tmpCD8)),((_tmpCD8->exist_vars=0,((_tmpCD8->rgn_po=0,((_tmpCD8->fields=_tmp814,((_tmpCD8->tagged=0,_tmpCD8)))))))))),((_tmpCD6->expected_mem_kind=0,((_tmpCD6->attributes=0,_tmpCD6)))))))))))))));
# 4899
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpCDE;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpCDD;struct Cyc_List_List*_tmpCDC;Cyc_Toc_result_decls=((_tmpCDC=_cycalloc(sizeof(*_tmpCDC)),((_tmpCDC->hd=Cyc_Absyn_new_decl((void*)((_tmpCDE=_cycalloc(sizeof(*_tmpCDE)),((_tmpCDE[0]=((_tmpCDD.tag=5,((_tmpCDD.f1=_tmp815,_tmpCDD)))),_tmpCDE)))),0),((_tmpCDC->tl=Cyc_Toc_result_decls,_tmpCDC))))));}
_tmp80F->type=Cyc_Absyn_strct(str);}{
# 4902
struct Cyc_List_List*_tmpCDF;new_fields=((_tmpCDF=_cycalloc(sizeof(*_tmpCDF)),((_tmpCDF->hd=_tmp80F,((_tmpCDF->tl=new_fields,_tmpCDF))))));};};}}
# 4904
(new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);};}
# 4906
if(_tmp7EE){
struct Cyc_Absyn_Decl*_tmpCE9;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpCE8;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpCE7;struct Cyc_List_List*_tmpCE6;Cyc_Toc_result_decls=((_tmpCE6=_cycalloc(sizeof(*_tmpCE6)),((_tmpCE6->hd=((_tmpCE9=_cycalloc(sizeof(*_tmpCE9)),((_tmpCE9->r=(void*)((_tmpCE7=_cycalloc(sizeof(*_tmpCE7)),((_tmpCE7[0]=((_tmpCE8.tag=5,((_tmpCE8.f1=new_ad,_tmpCE8)))),_tmpCE7)))),((_tmpCE9->loc=0,_tmpCE9)))))),((_tmpCE6->tl=Cyc_Toc_result_decls,_tmpCE6))))));}
return 0;};}_LL56A:;}_LL565:;}
# 4911
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple36 _tmpCEA;struct _tuple36 p=(_tmpCEA.f1=ad,((_tmpCEA.f2=add_to_decls,_tmpCEA)));
((int(*)(struct _tuple36*arg,int(*f)(struct _RegionHandle*,struct _tuple37*)))Cyc_Toc_use_toc_state)(& p,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple38{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 4940 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple38*env){
# 4943
struct _tuple38 _tmp82C=*env;struct _tuple38 _tmp82D=_tmp82C;struct Cyc_Set_Set**_tmp82E;struct Cyc_Absyn_Datatypedecl*_tmp82F;_LL57A: _tmp82E=(_tmp82D.f1)->datatypes_so_far;_tmp82F=_tmp82D.f2;_LL57B: {
struct _tuple1*_tmp830=_tmp82F->name;
if(_tmp82F->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*_tmp82E,_tmp830))
return 0;
*_tmp82E=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*_tmp82E,_tmp830);
{struct Cyc_List_List*_tmp831=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp82F->fields))->v;for(0;_tmp831 != 0;_tmp831=_tmp831->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp831->hd;
# 4951
struct Cyc_List_List*_tmp832=0;
int i=1;
{struct Cyc_List_List*_tmp833=f->typs;for(0;_tmp833 != 0;(_tmp833=_tmp833->tl,i ++)){
struct _dyneither_ptr*_tmp834=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmpCEB;struct Cyc_Absyn_Aggrfield*_tmp835=(_tmpCEB=_cycalloc(sizeof(*_tmpCEB)),((_tmpCEB->name=_tmp834,((_tmpCEB->tq=(*((struct _tuple11*)_tmp833->hd)).f1,((_tmpCEB->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp833->hd)).f2),((_tmpCEB->width=0,((_tmpCEB->attributes=0,((_tmpCEB->requires_clause=0,_tmpCEB)))))))))))));
struct Cyc_List_List*_tmpCEC;_tmp832=((_tmpCEC=_cycalloc(sizeof(*_tmpCEC)),((_tmpCEC->hd=_tmp835,((_tmpCEC->tl=_tmp832,_tmpCEC))))));}}
# 4959
{struct Cyc_Absyn_Aggrfield*_tmpCEF;struct Cyc_List_List*_tmpCEE;_tmp832=((_tmpCEE=_cycalloc(sizeof(*_tmpCEE)),((_tmpCEE->hd=((_tmpCEF=_cycalloc(sizeof(*_tmpCEF)),((_tmpCEF->name=Cyc_Toc_tag_sp,((_tmpCEF->tq=Cyc_Toc_mt_tq,((_tmpCEF->type=Cyc_Absyn_sint_typ,((_tmpCEF->width=0,((_tmpCEF->attributes=0,((_tmpCEF->requires_clause=0,_tmpCEF)))))))))))))),((_tmpCEE->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp832),_tmpCEE))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpCF2;struct Cyc_Absyn_Aggrdecl*_tmpCF1;struct Cyc_Absyn_Aggrdecl*_tmp83A=
(_tmpCF1=_cycalloc(sizeof(*_tmpCF1)),((_tmpCF1->kind=Cyc_Absyn_StructA,((_tmpCF1->sc=Cyc_Absyn_Public,((_tmpCF1->name=
Cyc_Toc_collapse_qvars(f->name,_tmp82F->name),((_tmpCF1->tvs=0,((_tmpCF1->expected_mem_kind=0,((_tmpCF1->impl=(
# 4965
(_tmpCF2=_cycalloc(sizeof(*_tmpCF2)),((_tmpCF2->exist_vars=0,((_tmpCF2->rgn_po=0,((_tmpCF2->fields=_tmp832,((_tmpCF2->tagged=0,_tmpCF2)))))))))),((_tmpCF1->attributes=0,_tmpCF1)))))))))))))));
# 4967
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpCF8;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpCF7;struct Cyc_List_List*_tmpCF6;Cyc_Toc_result_decls=((_tmpCF6=_cycalloc(sizeof(*_tmpCF6)),((_tmpCF6->hd=Cyc_Absyn_new_decl((void*)((_tmpCF8=_cycalloc(sizeof(*_tmpCF8)),((_tmpCF8[0]=((_tmpCF7.tag=5,((_tmpCF7.f1=_tmp83A,_tmpCF7)))),_tmpCF8)))),0),((_tmpCF6->tl=Cyc_Toc_result_decls,_tmpCF6))))));};}}
# 4969
return 0;}_LL579:;}
# 4972
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple38*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 4991 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple38*env){
# 4994
struct _tuple38 _tmp840=*env;struct _tuple38 _tmp841=_tmp840;struct Cyc_Toc_TocState*_tmp842;struct Cyc_Absyn_Datatypedecl*_tmp843;_LL57F: _tmp842=_tmp841.f1;_tmp843=_tmp841.f2;_LL580:
 if(_tmp843->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp844=*_tmp842;struct Cyc_Toc_TocState _tmp845=_tmp844;struct Cyc_Dict_Dict*_tmp846;_LL584: _tmp846=_tmp845.xdatatypes_so_far;_LL585: {
struct _tuple1*_tmp847=_tmp843->name;
{struct Cyc_List_List*_tmp848=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp843->fields))->v;for(0;_tmp848 != 0;_tmp848=_tmp848->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp848->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp849=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0);
void*_tmp84A=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,_tmp849,Cyc_Absyn_false_conref,0);
# 5005
int*_tmp84B=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp846,f->name);int*_tmp84C=_tmp84B;if(_tmp84C == 0){_LL589: _LL58A: {
# 5007
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0);{
# 5011
struct Cyc_Absyn_Vardecl*_tmp84D=Cyc_Absyn_new_vardecl(f->name,_tmp84A,initopt);
_tmp84D->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpCFE;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpCFD;struct Cyc_List_List*_tmpCFC;Cyc_Toc_result_decls=((_tmpCFC=_cycalloc(sizeof(*_tmpCFC)),((_tmpCFC->hd=Cyc_Absyn_new_decl((void*)((_tmpCFE=_cycalloc(sizeof(*_tmpCFE)),((_tmpCFE[0]=((_tmpCFD.tag=0,((_tmpCFD.f1=_tmp84D,_tmpCFD)))),_tmpCFE)))),0),((_tmpCFC->tl=Cyc_Toc_result_decls,_tmpCFC))))));}
*_tmp846=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp846,f->name,f->sc != Cyc_Absyn_Extern);{
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp851=f->typs;for(0;_tmp851 != 0;(_tmp851=_tmp851->tl,i ++)){
struct Cyc_Int_pa_PrintArg_struct _tmpD06;void*_tmpD05[1];const char*_tmpD04;struct _dyneither_ptr*_tmpD03;struct _dyneither_ptr*_tmp852=(_tmpD03=_cycalloc(sizeof(*_tmpD03)),((_tmpD03[0]=(struct _dyneither_ptr)((_tmpD06.tag=1,((_tmpD06.f1=(unsigned long)i,((_tmpD05[0]=& _tmpD06,Cyc_aprintf(((_tmpD04="f%d",_tag_dyneither(_tmpD04,sizeof(char),4))),_tag_dyneither(_tmpD05,sizeof(void*),1)))))))),_tmpD03)));
struct Cyc_Absyn_Aggrfield*_tmpD07;struct Cyc_Absyn_Aggrfield*_tmp853=(_tmpD07=_cycalloc(sizeof(*_tmpD07)),((_tmpD07->name=_tmp852,((_tmpD07->tq=(*((struct _tuple11*)_tmp851->hd)).f1,((_tmpD07->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp851->hd)).f2),((_tmpD07->width=0,((_tmpD07->attributes=0,((_tmpD07->requires_clause=0,_tmpD07)))))))))))));
struct Cyc_List_List*_tmpD08;fields=((_tmpD08=_cycalloc(sizeof(*_tmpD08)),((_tmpD08->hd=_tmp853,((_tmpD08->tl=fields,_tmpD08))))));}}
# 5024
{struct Cyc_Absyn_Aggrfield*_tmpD0B;struct Cyc_List_List*_tmpD0A;fields=((_tmpD0A=_cycalloc(sizeof(*_tmpD0A)),((_tmpD0A->hd=((_tmpD0B=_cycalloc(sizeof(*_tmpD0B)),((_tmpD0B->name=Cyc_Toc_tag_sp,((_tmpD0B->tq=Cyc_Toc_mt_tq,((_tmpD0B->type=
Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq),((_tmpD0B->width=0,((_tmpD0B->attributes=0,((_tmpD0B->requires_clause=0,_tmpD0B)))))))))))))),((_tmpD0A->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmpD0A))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpD0E;struct Cyc_Absyn_Aggrdecl*_tmpD0D;struct Cyc_Absyn_Aggrdecl*_tmp85C=
(_tmpD0D=_cycalloc(sizeof(*_tmpD0D)),((_tmpD0D->kind=Cyc_Absyn_StructA,((_tmpD0D->sc=Cyc_Absyn_Public,((_tmpD0D->name=
Cyc_Toc_collapse_qvars(f->name,_tmp843->name),((_tmpD0D->tvs=0,((_tmpD0D->expected_mem_kind=0,((_tmpD0D->impl=(
# 5031
(_tmpD0E=_cycalloc(sizeof(*_tmpD0E)),((_tmpD0E->exist_vars=0,((_tmpD0E->rgn_po=0,((_tmpD0E->fields=fields,((_tmpD0E->tagged=0,_tmpD0E)))))))))),((_tmpD0D->attributes=0,_tmpD0D)))))))))))))));
# 5033
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD14;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD13;struct Cyc_List_List*_tmpD12;Cyc_Toc_result_decls=((_tmpD12=_cycalloc(sizeof(*_tmpD12)),((_tmpD12->hd=Cyc_Absyn_new_decl((void*)((_tmpD14=_cycalloc(sizeof(*_tmpD14)),((_tmpD14[0]=((_tmpD13.tag=5,((_tmpD13.f1=_tmp85C,_tmpD13)))),_tmpD14)))),0),((_tmpD12->tl=Cyc_Toc_result_decls,_tmpD12))))));}
# 5035
goto _LL588;};};};}}else{if(*((int*)_tmp84C)== 0){_LL58B: _LL58C:
# 5037
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp862=Cyc_Absyn_string_exp(*fn,0);
struct Cyc_Absyn_Vardecl*_tmp863=Cyc_Absyn_new_vardecl(f->name,_tmp84A,_tmp862);
_tmp863->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD1A;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD19;struct Cyc_List_List*_tmpD18;Cyc_Toc_result_decls=((_tmpD18=_cycalloc(sizeof(*_tmpD18)),((_tmpD18->hd=Cyc_Absyn_new_decl((void*)((_tmpD1A=_cycalloc(sizeof(*_tmpD1A)),((_tmpD1A[0]=((_tmpD19.tag=0,((_tmpD19.f1=_tmp863,_tmpD19)))),_tmpD1A)))),0),((_tmpD18->tl=Cyc_Toc_result_decls,_tmpD18))))));}
*_tmp846=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp846,f->name,1);}
# 5044
goto _LL588;}else{_LL58D: _LL58E:
 goto _LL588;}}_LL588:;}}
# 5048
return 0;}_LL583:;};_LL57E:;}
# 5051
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple38*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 5056
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 5064
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
vd->type=Cyc_Toc_typ_to_c(old_typ);
# 5068
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(body_nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp867=init->r;void*_tmp868=_tmp867;struct Cyc_Absyn_Vardecl*_tmp869;struct Cyc_Absyn_Exp*_tmp86A;struct Cyc_Absyn_Exp*_tmp86B;int _tmp86C;if(((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp868)->tag == 26){_LL590: _tmp869=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp868)->f1;_tmp86A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp868)->f2;_tmp86B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp868)->f3;_tmp86C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp868)->f4;_LL591:
# 5075
 vd->initializer=0;
s->r=(Cyc_Toc_init_comprehension(init_nv,old_typ,Cyc_Absyn_var_exp(vd->name,0),_tmp869,_tmp86A,_tmp86B,_tmp86C,
Cyc_Absyn_new_stmt(s->r,0),0))->r;
goto _LL58F;}else{_LL592: _LL593:
# 5080
 if(vd->sc == Cyc_Absyn_Static){
# 5084
struct _RegionHandle _tmp86D=_new_region("temp");struct _RegionHandle*temp=& _tmp86D;_push_region(temp);
{struct Cyc_Toc_Env*_tmp86E=Cyc_Toc_set_toplevel(temp,init_nv);
Cyc_Toc_exp_to_c(_tmp86E,init);}
# 5085
;_pop_region(temp);}else{
# 5089
Cyc_Toc_exp_to_c(init_nv,init);}
goto _LL58F;}_LL58F:;}else{
# 5094
void*_tmp86F=Cyc_Tcutil_compress(old_typ);void*_tmp870=_tmp86F;void*_tmp871;struct Cyc_Absyn_Exp*_tmp872;union Cyc_Absyn_Constraint*_tmp873;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp870)->tag == 8){_LL595: _tmp871=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp870)->f1).elt_type;_tmp872=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp870)->f1).num_elts;_tmp873=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp870)->f1).zero_term;if(
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp873)){_LL596:
 if(_tmp872 == 0){
const char*_tmpD1D;void*_tmpD1C;(_tmpD1C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD1D="can't initialize zero-terminated array -- size unknown",_tag_dyneither(_tmpD1D,sizeof(char),55))),_tag_dyneither(_tmpD1C,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=_tmp872;
struct Cyc_Absyn_Exp*_tmp876=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),
Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);
# 5102
struct Cyc_Absyn_Exp*_tmp877=Cyc_Absyn_signed_int_exp(0,0);
s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp876,_tmp877,0),0),
Cyc_Absyn_new_stmt(s->r,0));
goto _LL594;};}else{goto _LL597;}}else{_LL597: _LL598:
 goto _LL594;}_LL594:;}}
# 5113
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success,struct Cyc_Absyn_Stmt*fail){
# 5116
void*_tmp878=d;struct Cyc_List_List*_tmp879;struct Cyc_List_List*_tmp87A;void**_tmp87B;struct Cyc_Tcpat_Rhs*_tmp87C;switch(*((int*)_tmp878)){case 0: _LL59A: _LL59B: {
# 5118
struct Cyc_Tcpat_Success_Tcpat_Decision_struct _tmpD23;struct Cyc_Tcpat_Rhs*_tmpD22;struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmpD21;return(void*)((_tmpD21=_cycalloc(sizeof(*_tmpD21)),((_tmpD21[0]=((_tmpD23.tag=1,((_tmpD23.f1=((_tmpD22=_cycalloc(sizeof(*_tmpD22)),((_tmpD22->used=1,((_tmpD22->pat_loc=0,((_tmpD22->rhs=fail,_tmpD22)))))))),_tmpD23)))),_tmpD21))));}case 1: _LL59C: _tmp87C=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp878)->f1;_LL59D:
 _tmp87C->rhs=success;return d;default: _LL59E: _tmp879=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp878)->f1;_tmp87A=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp878)->f2;_tmp87B=(void**)&((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp878)->f3;_LL59F:
# 5121
*_tmp87B=Cyc_Toc_rewrite_decision(*_tmp87B,success,fail);
for(0;_tmp87A != 0;_tmp87A=_tmp87A->tl){
struct _tuple29*_tmp880=(struct _tuple29*)_tmp87A->hd;struct _tuple29*_tmp881=_tmp880;void**_tmp882;_LL5A1: _tmp882=(void**)& _tmp881->f2;_LL5A2:
*_tmp882=Cyc_Toc_rewrite_decision(*_tmp882,success,fail);_LL5A0:;}
# 5126
return d;}_LL599:;}
# 5135
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 5139
struct Cyc_Absyn_Stmt*_tmp883=Cyc_Toc_skip_stmt_dl();
struct Cyc_Absyn_Pat*_tmp884=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0);
_tmp884->topt=p->topt;{
struct Cyc_Absyn_Switch_clause*_tmpD24;struct Cyc_Absyn_Switch_clause*_tmp885=(_tmpD24=_cycalloc(sizeof(*_tmpD24)),((_tmpD24->pattern=p,((_tmpD24->pat_vars=0,((_tmpD24->where_clause=0,((_tmpD24->body=s,((_tmpD24->loc=0,_tmpD24)))))))))));
struct Cyc_Absyn_Switch_clause*_tmpD25;struct Cyc_Absyn_Switch_clause*_tmp886=(_tmpD25=_cycalloc(sizeof(*_tmpD25)),((_tmpD25->pattern=_tmp884,((_tmpD25->pat_vars=0,((_tmpD25->where_clause=0,((_tmpD25->body=_tmp883,((_tmpD25->loc=0,_tmpD25)))))))))));
struct Cyc_Absyn_Switch_clause*_tmpD26[2];struct Cyc_List_List*swcs=(_tmpD26[1]=_tmp886,((_tmpD26[0]=_tmp885,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD26,sizeof(struct Cyc_Absyn_Switch_clause*),2)))));
# 5147
if(dopt != 0)
dopt=Cyc_Toc_rewrite_decision(dopt,s,_tmp883);{
# 5150
struct Cyc_Absyn_Stmt*_tmp887=Cyc_Toc_skip_stmt_dl();
Cyc_Toc_xlate_switch(0,nv,_tmp887,e,swcs,dopt);
# 5153
_tmp883->r=(Cyc_Toc_throw_match_stmt())->r;
return _tmp887;};};}
# 5160
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp88B=e->r;void*_tmp88C=_tmp88B;struct Cyc_Absyn_MallocInfo*_tmp88D;struct Cyc_Absyn_Stmt*_tmp88E;void**_tmp88F;void**_tmp890;struct Cyc_List_List*_tmp891;struct Cyc_List_List*_tmp892;void**_tmp893;struct Cyc_List_List*_tmp894;void**_tmp895;struct Cyc_Absyn_Exp*_tmp896;struct Cyc_Absyn_Exp*_tmp897;struct Cyc_List_List*_tmp898;struct Cyc_Absyn_Exp*_tmp899;struct Cyc_Absyn_Exp*_tmp89A;struct Cyc_Absyn_Exp*_tmp89B;struct Cyc_Absyn_Exp*_tmp89C;struct Cyc_Absyn_Exp*_tmp89D;struct Cyc_Absyn_Exp*_tmp89E;struct Cyc_Absyn_Exp*_tmp89F;struct Cyc_Absyn_Exp*_tmp8A0;struct Cyc_Absyn_Exp*_tmp8A1;struct Cyc_Absyn_Exp*_tmp8A2;struct Cyc_Absyn_Exp*_tmp8A3;struct Cyc_Absyn_Exp*_tmp8A4;struct Cyc_Absyn_Exp*_tmp8A5;struct Cyc_Absyn_Exp*_tmp8A6;struct Cyc_Absyn_Exp*_tmp8A7;struct Cyc_List_List*_tmp8A8;struct Cyc_Absyn_Exp*_tmp8A9;struct Cyc_Absyn_Exp*_tmp8AA;struct Cyc_Absyn_Exp*_tmp8AB;struct Cyc_Absyn_Exp*_tmp8AC;struct Cyc_Absyn_Exp*_tmp8AD;struct Cyc_Absyn_Exp*_tmp8AE;struct Cyc_Absyn_Exp*_tmp8AF;struct Cyc_Absyn_Exp*_tmp8B0;switch(*((int*)_tmp88C)){case 19: _LL5A6: _tmp8B0=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp88C)->f1;_LL5A7:
 _tmp8AF=_tmp8B0;goto _LL5A9;case 20: _LL5A8: _tmp8AF=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp88C)->f1;_LL5A9:
 _tmp8AE=_tmp8AF;goto _LL5AB;case 21: _LL5AA: _tmp8AE=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp88C)->f1;_LL5AB:
 _tmp8AD=_tmp8AE;goto _LL5AD;case 14: _LL5AC: _tmp8AD=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp88C)->f1;_LL5AD:
 _tmp8AC=_tmp8AD;goto _LL5AF;case 10: _LL5AE: _tmp8AC=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp88C)->f1;_LL5AF:
 _tmp8AB=_tmp8AC;goto _LL5B1;case 11: _LL5B0: _tmp8AB=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp88C)->f1;_LL5B1:
 _tmp8AA=_tmp8AB;goto _LL5B3;case 17: _LL5B2: _tmp8AA=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp88C)->f1;_LL5B3:
 _tmp8A9=_tmp8AA;goto _LL5B5;case 4: _LL5B4: _tmp8A9=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp88C)->f1;_LL5B5:
 Cyc_Toc_exptypes_to_c(_tmp8A9);goto _LL5A5;case 2: _LL5B6: _tmp8A8=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp88C)->f2;_LL5B7:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp8A8);goto _LL5A5;case 6: _LL5B8: _tmp8A6=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp88C)->f1;_tmp8A7=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp88C)->f2;_LL5B9:
 _tmp8A4=_tmp8A6;_tmp8A5=_tmp8A7;goto _LL5BB;case 7: _LL5BA: _tmp8A4=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp88C)->f1;_tmp8A5=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp88C)->f2;_LL5BB:
 _tmp8A2=_tmp8A4;_tmp8A3=_tmp8A5;goto _LL5BD;case 8: _LL5BC: _tmp8A2=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp88C)->f1;_tmp8A3=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp88C)->f2;_LL5BD:
 _tmp8A0=_tmp8A2;_tmp8A1=_tmp8A3;goto _LL5BF;case 22: _LL5BE: _tmp8A0=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp88C)->f1;_tmp8A1=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp88C)->f2;_LL5BF:
 _tmp89E=_tmp8A0;_tmp89F=_tmp8A1;goto _LL5C1;case 34: _LL5C0: _tmp89E=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp88C)->f1;_tmp89F=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp88C)->f2;_LL5C1:
 _tmp89C=_tmp89E;_tmp89D=_tmp89F;goto _LL5C3;case 3: _LL5C2: _tmp89C=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp88C)->f1;_tmp89D=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp88C)->f3;_LL5C3:
 Cyc_Toc_exptypes_to_c(_tmp89C);Cyc_Toc_exptypes_to_c(_tmp89D);goto _LL5A5;case 5: _LL5C4: _tmp899=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp88C)->f1;_tmp89A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp88C)->f2;_tmp89B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp88C)->f3;_LL5C5:
# 5178
 Cyc_Toc_exptypes_to_c(_tmp899);Cyc_Toc_exptypes_to_c(_tmp89A);Cyc_Toc_exptypes_to_c(_tmp89B);goto _LL5A5;case 9: _LL5C6: _tmp897=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp88C)->f1;_tmp898=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp88C)->f2;_LL5C7:
# 5180
 Cyc_Toc_exptypes_to_c(_tmp897);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp898);goto _LL5A5;case 13: _LL5C8: _tmp895=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp88C)->f1;_tmp896=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp88C)->f2;_LL5C9:
*_tmp895=Cyc_Toc_typ_to_c(*_tmp895);Cyc_Toc_exptypes_to_c(_tmp896);goto _LL5A5;case 24: _LL5CA: _tmp893=(void**)&(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp88C)->f1)->f3;_tmp894=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp88C)->f2;_LL5CB:
# 5183
*_tmp893=Cyc_Toc_typ_to_c(*_tmp893);
_tmp892=_tmp894;goto _LL5CD;case 35: _LL5CC: _tmp892=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp88C)->f2;_LL5CD:
 _tmp891=_tmp892;goto _LL5CF;case 25: _LL5CE: _tmp891=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp88C)->f1;_LL5CF:
# 5187
 for(0;_tmp891 != 0;_tmp891=_tmp891->tl){
struct _tuple19 _tmp8B1=*((struct _tuple19*)_tmp891->hd);struct _tuple19 _tmp8B2=_tmp8B1;struct Cyc_Absyn_Exp*_tmp8B3;_LL5F7: _tmp8B3=_tmp8B2.f2;_LL5F8:
 Cyc_Toc_exptypes_to_c(_tmp8B3);_LL5F6:;}
# 5191
goto _LL5A5;case 18: _LL5D0: _tmp890=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp88C)->f1;_LL5D1:
 _tmp88F=_tmp890;goto _LL5D3;case 16: _LL5D2: _tmp88F=(void**)&((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp88C)->f1;_LL5D3:
*_tmp88F=Cyc_Toc_typ_to_c(*_tmp88F);goto _LL5A5;case 36: _LL5D4: _tmp88E=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp88C)->f1;_LL5D5:
 Cyc_Toc_stmttypes_to_c(_tmp88E);goto _LL5A5;case 33: _LL5D6: _tmp88D=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp88C)->f1;_LL5D7:
# 5196
 if(_tmp88D->elt_type != 0){
void**_tmpD27;_tmp88D->elt_type=((_tmpD27=_cycalloc(sizeof(*_tmpD27)),((_tmpD27[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp88D->elt_type))),_tmpD27))));}
Cyc_Toc_exptypes_to_c(_tmp88D->num_elts);
goto _LL5A5;case 0: _LL5D8: _LL5D9:
 goto _LL5DB;case 1: _LL5DA: _LL5DB:
 goto _LL5DD;case 31: _LL5DC: _LL5DD:
 goto _LL5DF;case 39: _LL5DE: _LL5DF:
 goto _LL5E1;case 32: _LL5E0: _LL5E1:
 goto _LL5A5;case 29: _LL5E2: _LL5E3:
# 5206
 goto _LL5E5;case 30: _LL5E4: _LL5E5:
 goto _LL5E7;case 28: _LL5E6: _LL5E7:
 goto _LL5E9;case 26: _LL5E8: _LL5E9:
 goto _LL5EB;case 27: _LL5EA: _LL5EB:
 goto _LL5ED;case 23: _LL5EC: _LL5ED:
 goto _LL5EF;case 12: _LL5EE: _LL5EF:
 goto _LL5F1;case 15: _LL5F0: _LL5F1:
 goto _LL5F3;case 38: _LL5F2: _LL5F3:
 goto _LL5F5;default: _LL5F4: _LL5F5:
# 5216
{const char*_tmpD2A;void*_tmpD29;(_tmpD29=0,Cyc_Tcutil_terr(e->loc,((_tmpD2A="Cyclone expression within C code",_tag_dyneither(_tmpD2A,sizeof(char),33))),_tag_dyneither(_tmpD29,sizeof(void*),0)));}
goto _LL5A5;}_LL5A5:;}
# 5221
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp8B7=d->r;void*_tmp8B8=_tmp8B7;struct Cyc_Absyn_Typedefdecl*_tmp8B9;struct Cyc_Absyn_Enumdecl*_tmp8BA;struct Cyc_Absyn_Aggrdecl*_tmp8BB;struct Cyc_Absyn_Fndecl*_tmp8BC;struct Cyc_Absyn_Vardecl*_tmp8BD;switch(*((int*)_tmp8B8)){case 0: _LL5FC: _tmp8BD=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8B8)->f1;_LL5FD:
# 5224
 _tmp8BD->type=Cyc_Toc_typ_to_c(_tmp8BD->type);
if(_tmp8BD->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp8BD->initializer));
goto _LL5FB;case 1: _LL5FE: _tmp8BC=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp8B8)->f1;_LL5FF:
# 5228
 _tmp8BC->ret_type=Cyc_Toc_typ_to_c(_tmp8BC->ret_type);
{struct Cyc_List_List*_tmp8BE=_tmp8BC->args;for(0;_tmp8BE != 0;_tmp8BE=_tmp8BE->tl){
(*((struct _tuple9*)_tmp8BE->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp8BE->hd)).f3);}}
# 5232
goto _LL5FB;case 5: _LL600: _tmp8BB=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp8B8)->f1;_LL601:
 Cyc_Toc_aggrdecl_to_c(_tmp8BB,1);goto _LL5FB;case 7: _LL602: _tmp8BA=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp8B8)->f1;_LL603:
# 5235
 if(_tmp8BA->fields != 0){
struct Cyc_List_List*_tmp8BF=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp8BA->fields))->v;for(0;_tmp8BF != 0;_tmp8BF=_tmp8BF->tl){
struct Cyc_Absyn_Enumfield*_tmp8C0=(struct Cyc_Absyn_Enumfield*)_tmp8BF->hd;
if(_tmp8C0->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp8C0->tag));}}
# 5240
goto _LL5FB;case 8: _LL604: _tmp8B9=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp8B8)->f1;_LL605:
 _tmp8B9->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp8B9->defn));goto _LL5FB;case 2: _LL606: _LL607:
 goto _LL609;case 3: _LL608: _LL609:
 goto _LL60B;case 6: _LL60A: _LL60B:
 goto _LL60D;case 9: _LL60C: _LL60D:
 goto _LL60F;case 10: _LL60E: _LL60F:
 goto _LL611;case 11: _LL610: _LL611:
 goto _LL613;case 12: _LL612: _LL613:
 goto _LL615;case 4: _LL614: _LL615:
# 5251
{const char*_tmpD2D;void*_tmpD2C;(_tmpD2C=0,Cyc_Tcutil_terr(d->loc,((_tmpD2D="Cyclone declaration within C code",_tag_dyneither(_tmpD2D,sizeof(char),34))),_tag_dyneither(_tmpD2C,sizeof(void*),0)));}
goto _LL5FB;case 13: _LL616: _LL617:
 goto _LL619;default: _LL618: _LL619:
# 5255
 goto _LL5FB;}_LL5FB:;}
# 5259
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp8C3=s->r;void*_tmp8C4=_tmp8C3;struct Cyc_Absyn_Stmt*_tmp8C5;struct Cyc_Absyn_Stmt*_tmp8C6;struct Cyc_Absyn_Exp*_tmp8C7;struct Cyc_Absyn_Decl*_tmp8C8;struct Cyc_Absyn_Stmt*_tmp8C9;struct Cyc_Absyn_Exp*_tmp8CA;struct Cyc_List_List*_tmp8CB;void*_tmp8CC;struct Cyc_Absyn_Exp*_tmp8CD;struct Cyc_Absyn_Exp*_tmp8CE;struct Cyc_Absyn_Exp*_tmp8CF;struct Cyc_Absyn_Stmt*_tmp8D0;struct Cyc_Absyn_Exp*_tmp8D1;struct Cyc_Absyn_Stmt*_tmp8D2;struct Cyc_Absyn_Exp*_tmp8D3;struct Cyc_Absyn_Stmt*_tmp8D4;struct Cyc_Absyn_Stmt*_tmp8D5;struct Cyc_Absyn_Exp*_tmp8D6;struct Cyc_Absyn_Stmt*_tmp8D7;struct Cyc_Absyn_Stmt*_tmp8D8;struct Cyc_Absyn_Exp*_tmp8D9;switch(*((int*)_tmp8C4)){case 1: _LL61B: _tmp8D9=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp8C4)->f1;_LL61C:
 Cyc_Toc_exptypes_to_c(_tmp8D9);goto _LL61A;case 2: _LL61D: _tmp8D7=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8C4)->f1;_tmp8D8=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8C4)->f2;_LL61E:
 Cyc_Toc_stmttypes_to_c(_tmp8D7);Cyc_Toc_stmttypes_to_c(_tmp8D8);goto _LL61A;case 3: _LL61F: _tmp8D6=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp8C4)->f1;_LL620:
 if(_tmp8D6 != 0)Cyc_Toc_exptypes_to_c(_tmp8D6);goto _LL61A;case 4: _LL621: _tmp8D3=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8C4)->f1;_tmp8D4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8C4)->f2;_tmp8D5=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8C4)->f3;_LL622:
# 5265
 Cyc_Toc_exptypes_to_c(_tmp8D3);Cyc_Toc_stmttypes_to_c(_tmp8D4);Cyc_Toc_stmttypes_to_c(_tmp8D5);goto _LL61A;case 5: _LL623: _tmp8D1=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp8C4)->f1).f1;_tmp8D2=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp8C4)->f2;_LL624:
# 5267
 Cyc_Toc_exptypes_to_c(_tmp8D1);Cyc_Toc_stmttypes_to_c(_tmp8D2);goto _LL61A;case 9: _LL625: _tmp8CD=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8C4)->f1;_tmp8CE=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8C4)->f2).f1;_tmp8CF=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8C4)->f3).f1;_tmp8D0=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8C4)->f4;_LL626:
# 5269
 Cyc_Toc_exptypes_to_c(_tmp8CD);Cyc_Toc_exptypes_to_c(_tmp8CE);Cyc_Toc_exptypes_to_c(_tmp8CF);
Cyc_Toc_stmttypes_to_c(_tmp8D0);goto _LL61A;case 10: _LL627: _tmp8CA=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8C4)->f1;_tmp8CB=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8C4)->f2;_tmp8CC=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8C4)->f3;_LL628:
# 5272
 Cyc_Toc_exptypes_to_c(_tmp8CA);
for(0;_tmp8CB != 0;_tmp8CB=_tmp8CB->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp8CB->hd)->body);}
goto _LL61A;case 12: _LL629: _tmp8C8=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8C4)->f1;_tmp8C9=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8C4)->f2;_LL62A:
 Cyc_Toc_decltypes_to_c(_tmp8C8);Cyc_Toc_stmttypes_to_c(_tmp8C9);goto _LL61A;case 14: _LL62B: _tmp8C6=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp8C4)->f1;_tmp8C7=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp8C4)->f2).f1;_LL62C:
 Cyc_Toc_stmttypes_to_c(_tmp8C6);Cyc_Toc_exptypes_to_c(_tmp8C7);goto _LL61A;case 13: _LL62D: _tmp8C5=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp8C4)->f2;_LL62E:
 Cyc_Toc_stmttypes_to_c(_tmp8C5);goto _LL61A;case 0: _LL62F: _LL630:
 goto _LL632;case 6: _LL631: _LL632:
 goto _LL634;case 7: _LL633: _LL634:
 goto _LL636;case 8: _LL635: _LL636:
# 5282
 goto _LL61A;case 11: _LL637: _LL638:
# 5285
{struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct _tmpD30;struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmpD2F;s->r=(void*)((_tmpD2F=_cycalloc_atomic(sizeof(*_tmpD2F)),((_tmpD2F[0]=((_tmpD30.tag=0,_tmpD30)),_tmpD2F))));}
goto _LL61A;case 15: _LL639: _LL63A:
 goto _LL63C;default: _LL63B: _LL63C:
# 5289
{struct Cyc_String_pa_PrintArg_struct _tmpD38;void*_tmpD37[1];const char*_tmpD36;void*_tmpD35;(_tmpD35=0,Cyc_Tcutil_terr(s->loc,(struct _dyneither_ptr)((_tmpD38.tag=0,((_tmpD38.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpD37[0]=& _tmpD38,Cyc_aprintf(((_tmpD36="Cyclone statement in C code: %s",_tag_dyneither(_tmpD36,sizeof(char),32))),_tag_dyneither(_tmpD37,sizeof(void*),1)))))))),_tag_dyneither(_tmpD35,sizeof(void*),0)));}
goto _LL61A;}_LL61A:;}
# 5298
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv)){
const char*_tmpD3B;void*_tmpD3A;(_tmpD3A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD3B="decls_to_c: not at toplevel!",_tag_dyneither(_tmpD3B,sizeof(char),29))),_tag_dyneither(_tmpD3A,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp8E2=d->r;void*_tmp8E3=_tmp8E2;struct Cyc_List_List*_tmp8E4;struct Cyc_List_List*_tmp8E5;struct Cyc_List_List*_tmp8E6;struct Cyc_List_List*_tmp8E7;struct Cyc_Absyn_Typedefdecl*_tmp8E8;struct Cyc_Absyn_Enumdecl*_tmp8E9;struct Cyc_Absyn_Datatypedecl*_tmp8EA;struct Cyc_Absyn_Aggrdecl*_tmp8EB;struct Cyc_Absyn_Fndecl*_tmp8EC;struct Cyc_Absyn_Vardecl*_tmp8ED;switch(*((int*)_tmp8E3)){case 0: _LL63E: _tmp8ED=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8E3)->f1;_LL63F: {
# 5305
struct _tuple1*_tmp8EE=_tmp8ED->name;
# 5307
if(_tmp8ED->sc == Cyc_Absyn_ExternC){
struct _tuple1*_tmpD3C;_tmp8EE=((_tmpD3C=_cycalloc(sizeof(*_tmpD3C)),((_tmpD3C->f1=Cyc_Absyn_Rel_n(0),((_tmpD3C->f2=(*_tmp8EE).f2,_tmpD3C))))));}
if(_tmp8ED->initializer != 0){
if(_tmp8ED->sc == Cyc_Absyn_ExternC)_tmp8ED->sc=Cyc_Absyn_Public;
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp8ED->initializer));else{
# 5314
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp8ED->initializer));}}
# 5316
{struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmpD3F;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpD3E;nv=Cyc_Toc_add_varmap(r,nv,_tmp8ED->name,Cyc_Absyn_varb_exp(_tmp8EE,(void*)((_tmpD3E=_cycalloc(sizeof(*_tmpD3E)),((_tmpD3E[0]=((_tmpD3F.tag=1,((_tmpD3F.f1=_tmp8ED,_tmpD3F)))),_tmpD3E)))),0));}
_tmp8ED->name=_tmp8EE;
_tmp8ED->sc=Cyc_Toc_scope_to_c(_tmp8ED->sc);
_tmp8ED->type=Cyc_Toc_typ_to_c(_tmp8ED->type);
{struct Cyc_List_List*_tmpD40;Cyc_Toc_result_decls=((_tmpD40=_cycalloc(sizeof(*_tmpD40)),((_tmpD40->hd=d,((_tmpD40->tl=Cyc_Toc_result_decls,_tmpD40))))));}
goto _LL63D;}case 1: _LL640: _tmp8EC=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp8E3)->f1;_LL641: {
# 5323
struct _tuple1*_tmp8F3=_tmp8EC->name;
# 5325
if(_tmp8EC->sc == Cyc_Absyn_ExternC){
{struct _tuple1*_tmpD41;_tmp8F3=((_tmpD41=_cycalloc(sizeof(*_tmpD41)),((_tmpD41->f1=Cyc_Absyn_Abs_n(0,1),((_tmpD41->f2=(*_tmp8F3).f2,_tmpD41))))));}
_tmp8EC->sc=Cyc_Absyn_Public;}
# 5329
nv=Cyc_Toc_add_varmap(r,nv,_tmp8EC->name,Cyc_Absyn_var_exp(_tmp8F3,0));
_tmp8EC->name=_tmp8F3;
Cyc_Toc_fndecl_to_c(nv,_tmp8EC,cinclude);
{struct Cyc_List_List*_tmpD42;Cyc_Toc_result_decls=((_tmpD42=_cycalloc(sizeof(*_tmpD42)),((_tmpD42->hd=d,((_tmpD42->tl=Cyc_Toc_result_decls,_tmpD42))))));}
goto _LL63D;}case 2: _LL642: _LL643:
 goto _LL645;case 3: _LL644: _LL645: {
# 5336
const char*_tmpD45;void*_tmpD44;(_tmpD44=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD45="letdecl at toplevel",_tag_dyneither(_tmpD45,sizeof(char),20))),_tag_dyneither(_tmpD44,sizeof(void*),0)));}case 4: _LL646: _LL647: {
# 5338
const char*_tmpD48;void*_tmpD47;(_tmpD47=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD48="region decl at toplevel",_tag_dyneither(_tmpD48,sizeof(char),24))),_tag_dyneither(_tmpD47,sizeof(void*),0)));}case 5: _LL648: _tmp8EB=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp8E3)->f1;_LL649:
# 5342
 Cyc_Toc_aggrdecl_to_c(_tmp8EB,1);
goto _LL63D;case 6: _LL64A: _tmp8EA=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp8E3)->f1;_LL64B:
# 5345
 if(_tmp8EA->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp8EA);else{
# 5348
Cyc_Toc_datatypedecl_to_c(_tmp8EA);}
goto _LL63D;case 7: _LL64C: _tmp8E9=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp8E3)->f1;_LL64D:
# 5351
 Cyc_Toc_enumdecl_to_c(_tmp8E9);
{struct Cyc_List_List*_tmpD49;Cyc_Toc_result_decls=((_tmpD49=_cycalloc(sizeof(*_tmpD49)),((_tmpD49->hd=d,((_tmpD49->tl=Cyc_Toc_result_decls,_tmpD49))))));}
goto _LL63D;case 8: _LL64E: _tmp8E8=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp8E3)->f1;_LL64F:
# 5355
 _tmp8E8->tvs=0;
if(_tmp8E8->defn != 0)
_tmp8E8->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp8E8->defn));else{
# 5359
enum Cyc_Absyn_KindQual _tmp8FB=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp8E8->kind))->v)->kind;enum Cyc_Absyn_KindQual _tmp8FC=_tmp8FB;if(_tmp8FC == Cyc_Absyn_BoxKind){_LL65D: _LL65E:
 _tmp8E8->defn=Cyc_Absyn_void_star_typ();goto _LL65C;}else{_LL65F: _LL660:
 _tmp8E8->defn=(void*)& Cyc_Absyn_VoidType_val;goto _LL65C;}_LL65C:;}
# 5364
{struct Cyc_List_List*_tmpD4A;Cyc_Toc_result_decls=((_tmpD4A=_cycalloc(sizeof(*_tmpD4A)),((_tmpD4A->hd=d,((_tmpD4A->tl=Cyc_Toc_result_decls,_tmpD4A))))));}
goto _LL63D;case 13: _LL650: _LL651:
 goto _LL653;case 14: _LL652: _LL653:
# 5368
 goto _LL63D;case 9: _LL654: _tmp8E7=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp8E3)->f2;_LL655:
 _tmp8E6=_tmp8E7;goto _LL657;case 10: _LL656: _tmp8E6=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp8E3)->f2;_LL657:
 _tmp8E5=_tmp8E6;goto _LL659;case 11: _LL658: _tmp8E5=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp8E3)->f1;_LL659:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp8E5,top,cinclude);goto _LL63D;default: _LL65A: _tmp8E4=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp8E3)->f1;_LL65B:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp8E4,top,1);goto _LL63D;}_LL63D:;};}
# 5375
return nv;}
# 5379
static void Cyc_Toc_init(){
struct Cyc_Core_NewDynamicRegion _tmp8FE=Cyc_Core_new_rckey();struct Cyc_Core_NewDynamicRegion _tmp8FF=_tmp8FE;struct Cyc_Core_DynamicRegion*_tmp900;_LL662: _tmp900=_tmp8FF.key;_LL663: {
struct Cyc_Toc_TocState*_tmp901=((struct Cyc_Toc_TocState*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Toc_TocState*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmp900,0,Cyc_Toc_empty_toc_state);
{struct Cyc_Toc_TocStateWrap*_tmpD4B;Cyc_Toc_toc_state=((_tmpD4B=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD4B)),((_tmpD4B->dyn=(struct Cyc_Core_DynamicRegion*)_tmp900,((_tmpD4B->state=(struct Cyc_Toc_TocState*)_tmp901,_tmpD4B))))));}
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0;
Cyc_Toc_bounds_checks_eliminated=0;{
struct _dyneither_ptr**_tmpD4C;Cyc_Toc_globals=_tag_dyneither(((_tmpD4C=_cycalloc(sizeof(struct _dyneither_ptr*)* 42),((_tmpD4C[0]=(struct _dyneither_ptr*)& Cyc_Toc__throw_str,((_tmpD4C[1]=(struct _dyneither_ptr*)& Cyc_Toc_setjmp_str,((_tmpD4C[2]=(struct _dyneither_ptr*)& Cyc_Toc__push_handler_str,((_tmpD4C[3]=(struct _dyneither_ptr*)& Cyc_Toc__pop_handler_str,((_tmpD4C[4]=(struct _dyneither_ptr*)& Cyc_Toc__exn_thrown_str,((_tmpD4C[5]=(struct _dyneither_ptr*)& Cyc_Toc__npop_handler_str,((_tmpD4C[6]=(struct _dyneither_ptr*)& Cyc_Toc__check_null_str,((_tmpD4C[7]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_null_str,((_tmpD4C[8]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_notnull_str,((_tmpD4C[9]=(struct _dyneither_ptr*)& Cyc_Toc__check_dyneither_subscript_str,((_tmpD4C[10]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_str,((_tmpD4C[11]=(struct _dyneither_ptr*)& Cyc_Toc__tag_dyneither_str,((_tmpD4C[12]=(struct _dyneither_ptr*)& Cyc_Toc__init_dyneither_ptr_str,((_tmpD4C[13]=(struct _dyneither_ptr*)& Cyc_Toc__untag_dyneither_ptr_str,((_tmpD4C[14]=(struct _dyneither_ptr*)& Cyc_Toc__get_dyneither_size_str,((_tmpD4C[15]=(struct _dyneither_ptr*)& Cyc_Toc__get_zero_arr_size_str,((_tmpD4C[16]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_plus_str,((_tmpD4C[17]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_plus_str,((_tmpD4C[18]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpD4C[19]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_str,((_tmpD4C[20]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpD4C[21]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_post_str,((_tmpD4C[22]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_str,((_tmpD4C[23]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_str,((_tmpD4C[24]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_atomic_str,((_tmpD4C[25]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_atomic_str,((_tmpD4C[26]=(struct _dyneither_ptr*)& Cyc_Toc__region_malloc_str,((_tmpD4C[27]=(struct _dyneither_ptr*)& Cyc_Toc__region_calloc_str,((_tmpD4C[28]=(struct _dyneither_ptr*)& Cyc_Toc__check_times_str,((_tmpD4C[29]=(struct _dyneither_ptr*)& Cyc_Toc__new_region_str,((_tmpD4C[30]=(struct _dyneither_ptr*)& Cyc_Toc__push_region_str,((_tmpD4C[31]=(struct _dyneither_ptr*)& Cyc_Toc__pop_region_str,((_tmpD4C[32]=(struct _dyneither_ptr*)& Cyc_Toc__open_dynregion_str,((_tmpD4C[33]=(struct _dyneither_ptr*)& Cyc_Toc__push_dynregion_str,((_tmpD4C[34]=(struct _dyneither_ptr*)& Cyc_Toc__pop_dynregion_str,((_tmpD4C[35]=(struct _dyneither_ptr*)& Cyc_Toc__reset_region_str,((_tmpD4C[36]=(struct _dyneither_ptr*)& Cyc_Toc__throw_arraybounds_str,((_tmpD4C[37]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_decrease_size_str,((_tmpD4C[38]=(struct _dyneither_ptr*)& Cyc_Toc__throw_match_str,((_tmpD4C[39]=(struct _dyneither_ptr*)& Cyc_Toc__swap_word_str,((_tmpD4C[40]=(struct _dyneither_ptr*)& Cyc_Toc__swap_dyneither_str,((_tmpD4C[41]=(struct _dyneither_ptr*)& Cyc_Toc__fast_region_malloc_str,_tmpD4C)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),sizeof(struct _dyneither_ptr*),42);};}_LL661:;}
# 5436
static int Cyc_Toc_destroy_labels(struct _RegionHandle*d,struct Cyc_Toc_TocState*ts){
struct Cyc_Toc_TocState _tmp904=*ts;struct Cyc_Toc_TocState _tmp905=_tmp904;struct Cyc_Xarray_Xarray*_tmp906;_LL667: _tmp906=_tmp905.temp_labels;_LL668:
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp906);
return 0;_LL666:;}
# 5444
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){
Cyc_Toc_init();{
struct _RegionHandle _tmp907=_new_region("start");struct _RegionHandle*start=& _tmp907;_push_region(start);
Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(start),ds,1,0);{
# 5453
struct Cyc_Toc_TocStateWrap*ts=0;
_swap_word(& ts,& Cyc_Toc_toc_state);{
struct Cyc_Toc_TocStateWrap _tmp908=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp909=_tmp908;struct Cyc_Core_DynamicRegion*_tmp90A;struct Cyc_Toc_TocState*_tmp90B;_LL66C: _tmp90A=_tmp909.dyn;_tmp90B=_tmp909.state;_LL66D:
((int(*)(struct Cyc_Core_DynamicRegion*key,struct Cyc_Toc_TocState*arg,int(*body)(struct _RegionHandle*h,struct Cyc_Toc_TocState*arg)))Cyc_Core_open_region)(_tmp90A,_tmp90B,Cyc_Toc_destroy_labels);
Cyc_Core_free_rckey(_tmp90A);
((void(*)(struct _dyneither_ptr ptr))Cyc_Core_ufree)(_tag_dyneither(ts,sizeof(struct Cyc_Toc_TocStateWrap),1));_LL66B:;};};{
# 5460
struct Cyc_List_List*_tmp90C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0);return _tmp90C;};
# 5446
;_pop_region(start);};}
