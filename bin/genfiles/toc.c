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
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 445 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
# 448
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 461
typedef void*Cyc_Absyn_funcparams_t;
# 464
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 528
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 544
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 546
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign,long long);
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr,int);
# 554
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 561
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 579
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 709 "absyn.h"
extern struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val;struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 744
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
# 772
extern struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 931
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 938
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 948
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 951
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
# 958
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 961
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 976
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 978
extern void*Cyc_Absyn_sint_typ;
# 990
void*Cyc_Absyn_exn_typ();
# 1001
extern void*Cyc_Absyn_bounds_one;
# 1009
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1015
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);
# 1017
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1020
void*Cyc_Absyn_void_star_typ();
# 1022
void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple1*name);
void*Cyc_Absyn_unionq_typ(struct _tuple1*name);
# 1028
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1032
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1034
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1040
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);
# 1045
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
# 1048
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple1*,void*,unsigned int);
# 1052
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1055
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1061
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1064
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1067
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1072
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1076
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1078
struct Cyc_Absyn_Exp*Cyc_Absyn_rethrow_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1081
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
# 1086
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1091
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1097
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned int);
# 1103
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1113
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned int loc);
# 1116
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned int loc);
# 1120
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1122
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc);
# 1127
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1131
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1136
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
# 1179
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1182
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1192
struct _dyneither_ptr*Cyc_Absyn_fieldname(int);
# 1196
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1212
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
struct Cyc_Dict_Dict*_tmpA61;struct Cyc_Dict_Dict*_tmpA60;struct Cyc_Set_Set**_tmpA5F;struct Cyc_List_List**_tmpA5E;struct Cyc_Dict_Dict*_tmpA5D;struct Cyc_List_List**_tmpA5C;struct Cyc_Toc_TocState*_tmpA5B;return(_tmpA5B=_region_malloc(d,sizeof(*_tmpA5B)),((_tmpA5B->tuple_types=(
(_tmpA5C=_region_malloc(d,sizeof(*_tmpA5C)),((_tmpA5C[0]=0,_tmpA5C)))),((_tmpA5B->aggrs_so_far=(
(_tmpA5D=_region_malloc(d,sizeof(*_tmpA5D)),((_tmpA5D[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpA5D)))),((_tmpA5B->abs_struct_types=(
(_tmpA5E=_region_malloc(d,sizeof(*_tmpA5E)),((_tmpA5E[0]=0,_tmpA5E)))),((_tmpA5B->datatypes_so_far=(
(_tmpA5F=_region_malloc(d,sizeof(*_tmpA5F)),((_tmpA5F[0]=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpA5F)))),((_tmpA5B->xdatatypes_so_far=(
(_tmpA60=_region_malloc(d,sizeof(*_tmpA60)),((_tmpA60[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpA60)))),((_tmpA5B->qvar_tags=(
(_tmpA61=_region_malloc(d,sizeof(*_tmpA61)),((_tmpA61[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),_tmpA61)))),((_tmpA5B->temp_labels=
((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d),_tmpA5B)))))))))))))));}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
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
{struct Cyc_Toc_TocStateWrap _tmpA62;*ts=((_tmpA62.dyn=(struct Cyc_Core_DynamicRegion*)_tmp11,((_tmpA62.state=(struct Cyc_Toc_TocState*)_tmp12,_tmpA62))));}
_swap_word(& ts,& Cyc_Toc_toc_state);{
struct _tuple15 _tmpA63;struct _tuple15 _tmp15=(_tmpA63.f1=_tmp12,((_tmpA63.f2=arg,_tmpA63)));
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
struct _tuple16 _tmpA64;struct _tuple16 env=(_tmpA64.f1=q,((_tmpA64.f2=type_maker,_tmpA64)));
return((void*(*)(struct _tuple16*arg,void*(*f)(struct _RegionHandle*,struct _tuple17*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_aggrdecl_type_body);}
# 172
static int Cyc_Toc_tuple_type_counter=0;
static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;char Cyc_Toc_Toc_Unimplemented[18]="Toc_Unimplemented";struct Cyc_Toc_Toc_Unimplemented_exn_struct{char*tag;};char Cyc_Toc_Toc_Impossible[15]="Toc_Impossible";struct Cyc_Toc_Toc_Impossible_exn_struct{char*tag;};
# 179
static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 182
Cyc_vfprintf(Cyc_stderr,fmt,ap);
{const char*_tmpA67;void*_tmpA66;(_tmpA66=0,Cyc_fprintf(Cyc_stderr,((_tmpA67="\n",_tag_dyneither(_tmpA67,sizeof(char),2))),_tag_dyneither(_tmpA66,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Unimplemented_exn_struct _tmpA6A;struct Cyc_Toc_Toc_Unimplemented_exn_struct*_tmpA69;(int)_throw((void*)((_tmpA69=_cycalloc_atomic(sizeof(*_tmpA69)),((_tmpA69[0]=((_tmpA6A.tag=Cyc_Toc_Toc_Unimplemented,_tmpA6A)),_tmpA69)))));};}
# 187
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 190
Cyc_vfprintf(Cyc_stderr,fmt,ap);
{const char*_tmpA6D;void*_tmpA6C;(_tmpA6C=0,Cyc_fprintf(Cyc_stderr,((_tmpA6D="\n",_tag_dyneither(_tmpA6D,sizeof(char),2))),_tag_dyneither(_tmpA6C,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Impossible_exn_struct _tmpA70;struct Cyc_Toc_Toc_Impossible_exn_struct*_tmpA6F;(int)_throw((void*)((_tmpA6F=_cycalloc_atomic(sizeof(*_tmpA6F)),((_tmpA6F[0]=((_tmpA70.tag=Cyc_Toc_Toc_Impossible,_tmpA70)),_tmpA6F)))));};}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_exn_struct{char*tag;};static char _tmp29[5]="curr";
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
static struct _dyneither_ptr Cyc_Toc__rethrow_str={_tmpBB,_tmpBB,_tmpBB + 9};static struct _tuple1 Cyc_Toc__rethrow_pr={{.Loc_n={4,0}},& Cyc_Toc__rethrow_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__rethrow_re={1,& Cyc_Toc__rethrow_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__rethrow_ev={0,(void*)& Cyc_Toc__rethrow_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__rethrow_e=& Cyc_Toc__rethrow_ev;static char _tmpBD[20]="_fast_region_malloc";
static struct _dyneither_ptr Cyc_Toc__fast_region_malloc_str={_tmpBD,_tmpBD,_tmpBD + 20};static struct _tuple1 Cyc_Toc__fast_region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__fast_region_malloc_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fast_region_malloc_re={1,& Cyc_Toc__fast_region_malloc_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__fast_region_malloc_ev={0,(void*)& Cyc_Toc__fast_region_malloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fast_region_malloc_e=& Cyc_Toc__fast_region_malloc_ev;
# 302
static struct Cyc_Absyn_AggrType_Absyn_Type_struct Cyc_Toc_dyneither_ptr_typ_v={11,{{.UnknownAggr={1,{Cyc_Absyn_StructA,& Cyc_Toc__dyneither_ptr_pr,0}}},0}};
# 305
static void*Cyc_Toc_dyneither_ptr_typ=(void*)& Cyc_Toc_dyneither_ptr_typ_v;
# 307
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0};
# 309
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
static struct Cyc_Absyn_Stmt**skip_stmt_opt=0;
if(skip_stmt_opt == 0){
struct Cyc_Absyn_Stmt**_tmpA71;skip_stmt_opt=((_tmpA71=_cycalloc(sizeof(*_tmpA71)),((_tmpA71[0]=Cyc_Absyn_skip_stmt(0),_tmpA71))));}
return*skip_stmt_opt;}
# 318
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}
# 321
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmpA74;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpA73;return(void*)((_tmpA73=_cycalloc(sizeof(*_tmpA73)),((_tmpA73[0]=((_tmpA74.tag=13,((_tmpA74.f1=t,((_tmpA74.f2=e,((_tmpA74.f3=0,((_tmpA74.f4=Cyc_Absyn_No_coercion,_tmpA74)))))))))),_tmpA73))));}
# 324
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmpA77;struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpA76;return(void*)((_tmpA76=_cycalloc(sizeof(*_tmpA76)),((_tmpA76[0]=((_tmpA77.tag=19,((_tmpA77.f1=e,_tmpA77)))),_tmpA76))));}
# 327
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmpA7A;struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpA79;return(void*)((_tmpA79=_cycalloc(sizeof(*_tmpA79)),((_tmpA79[0]=((_tmpA7A.tag=22,((_tmpA7A.f1=e1,((_tmpA7A.f2=e2,_tmpA7A)))))),_tmpA79))));}
# 330
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmpA7D;struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmpA7C;return(void*)((_tmpA7C=_cycalloc(sizeof(*_tmpA7C)),((_tmpA7C[0]=((_tmpA7D.tag=36,((_tmpA7D.f1=s,_tmpA7D)))),_tmpA7C))));}
# 333
static void*Cyc_Toc_sizeoftyp_exp_r(void*t){
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpA80;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpA7F;return(void*)((_tmpA7F=_cycalloc(sizeof(*_tmpA7F)),((_tmpA7F[0]=((_tmpA80.tag=16,((_tmpA80.f1=t,_tmpA80)))),_tmpA7F))));}
# 336
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpA83;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpA82;return(void*)((_tmpA82=_cycalloc(sizeof(*_tmpA82)),((_tmpA82[0]=((_tmpA83.tag=9,((_tmpA83.f1=e,((_tmpA83.f2=es,((_tmpA83.f3=0,((_tmpA83.f4=1,_tmpA83)))))))))),_tmpA82))));}
# 339
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmpA86;struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpA85;return(void*)((_tmpA85=_cycalloc(sizeof(*_tmpA85)),((_tmpA85[0]=((_tmpA86.tag=1,((_tmpA86.f1=e,_tmpA86)))),_tmpA85))));}
# 342
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmpA89;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpA88;return(void*)((_tmpA88=_cycalloc(sizeof(*_tmpA88)),((_tmpA88[0]=((_tmpA89.tag=2,((_tmpA89.f1=s1,((_tmpA89.f2=s2,_tmpA89)))))),_tmpA88))));}
# 345
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmpA8C;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpA8B;return(void*)((_tmpA8B=_cycalloc(sizeof(*_tmpA8B)),((_tmpA8B[0]=((_tmpA8C.tag=5,((_tmpA8C.f1=e1,((_tmpA8C.f2=e2,((_tmpA8C.f3=e3,_tmpA8C)))))))),_tmpA8B))));}
# 348
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpA8F;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpA8E;return(void*)((_tmpA8E=_cycalloc(sizeof(*_tmpA8E)),((_tmpA8E[0]=((_tmpA8F.tag=20,((_tmpA8F.f1=e,((_tmpA8F.f2=n,((_tmpA8F.f3=0,((_tmpA8F.f4=0,_tmpA8F)))))))))),_tmpA8E))));}
# 351
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpA92;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpA91;return(void*)((_tmpA91=_cycalloc(sizeof(*_tmpA91)),((_tmpA91[0]=((_tmpA92.tag=21,((_tmpA92.f1=e,((_tmpA92.f2=n,((_tmpA92.f3=0,((_tmpA92.f4=0,_tmpA92)))))))))),_tmpA91))));}
# 354
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 357
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpA95;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpA94;return(void*)((_tmpA94=_cycalloc(sizeof(*_tmpA94)),((_tmpA94[0]=((_tmpA95.tag=35,((_tmpA95.f1=tdopt,((_tmpA95.f2=ds,_tmpA95)))))),_tmpA94))));}
# 359
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmpA98;struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmpA97;return(void*)((_tmpA97=_cycalloc(sizeof(*_tmpA97)),((_tmpA97[0]=((_tmpA98.tag=8,((_tmpA98.f1=v,((_tmpA98.f2=s,_tmpA98)))))),_tmpA97))));}
# 362
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 366
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,unsigned int loc){
void*_tmpDC=e->r;void*_tmpDD=_tmpDC;struct Cyc_Absyn_Exp*_tmpDF;_LL1: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpDE=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpDE->tag != 19)goto _LL3;else{_tmpDF=_tmpDE->f1;}}_LL2:
 return Cyc_Absyn_aggrarrow_exp(_tmpDF,f,loc);_LL3:;_LL4:
 return Cyc_Absyn_aggrmember_exp(e,f,loc);_LL0:;}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 383
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
# 392
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 401
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
# 410
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
# 421
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
struct Cyc_Absyn_Exp*function;
{void*_tmpE0=Cyc_Tcutil_compress(t);void*_tmpE1=_tmpE0;enum Cyc_Absyn_Size_of _tmpE3;_LL6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpE2=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpE1;if(_tmpE2->tag != 6)goto _LL8;else{_tmpE3=_tmpE2->f2;}}_LL7:
# 425
 switch(_tmpE3){case Cyc_Absyn_Char_sz: _LL12:
 function=fS->fchar;break;case Cyc_Absyn_Short_sz: _LL13:
 function=fS->fshort;break;case Cyc_Absyn_Int_sz: _LL14:
 function=fS->fint;break;default: _LL15: {
struct Cyc_Core_Impossible_exn_struct _tmpA9E;const char*_tmpA9D;struct Cyc_Core_Impossible_exn_struct*_tmpA9C;(int)_throw((void*)((_tmpA9C=_cycalloc(sizeof(*_tmpA9C)),((_tmpA9C[0]=((_tmpA9E.tag=Cyc_Core_Impossible,((_tmpA9E.f1=((_tmpA9D="impossible IntType (not char, short or int)",_tag_dyneither(_tmpA9D,sizeof(char),44))),_tmpA9E)))),_tmpA9C)))));}}
# 431
goto _LL5;_LL8: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpE4=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpE1;if(_tmpE4->tag != 7)goto _LLA;else{if(_tmpE4->f1 != 0)goto _LLA;}}_LL9:
# 433
 function=fS->ffloat;
goto _LL5;_LLA: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpE5=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpE1;if(_tmpE5->tag != 7)goto _LLC;else{if(_tmpE5->f1 != 1)goto _LLC;}}_LLB:
# 436
 function=fS->fdouble;
goto _LL5;_LLC: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpE6=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpE1;if(_tmpE6->tag != 7)goto _LLE;}_LLD:
# 439
 function=fS->flongdouble;
goto _LL5;_LLE: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE1;if(_tmpE7->tag != 5)goto _LL10;}_LLF:
# 442
 function=fS->fvoidstar;
goto _LL5;_LL10:;_LL11: {
# 445
struct Cyc_Core_Impossible_exn_struct _tmpAAB;const char*_tmpAAA;void*_tmpAA9[1];struct Cyc_String_pa_PrintArg_struct _tmpAA8;struct Cyc_Core_Impossible_exn_struct*_tmpAA7;(int)_throw((void*)((_tmpAA7=_cycalloc(sizeof(*_tmpAA7)),((_tmpAA7[0]=((_tmpAAB.tag=Cyc_Core_Impossible,((_tmpAAB.f1=(struct _dyneither_ptr)((_tmpAA8.tag=0,((_tmpAA8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpAA9[0]=& _tmpAA8,Cyc_aprintf(((_tmpAAA="impossible expression type %s (not int, float, double, or pointer)",_tag_dyneither(_tmpAAA,sizeof(char),67))),_tag_dyneither(_tmpAA9,sizeof(void*),1)))))))),_tmpAAB)))),_tmpAA7)))));}_LL5:;}
# 447
return function;}
# 449
struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
return Cyc_Toc_getFunctionType(fS,(void*)_check_null(arr->topt));}
# 452
struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmpF0=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_tmpF1=_tmpF0;void*_tmpF3;_LL18: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpF2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF1;if(_tmpF2->tag != 5)goto _LL1A;else{_tmpF3=(_tmpF2->f1).elt_typ;}}_LL19:
# 455
 return Cyc_Toc_getFunctionType(fS,_tmpF3);_LL1A:;_LL1B: {
struct Cyc_Core_Impossible_exn_struct _tmpAB1;const char*_tmpAB0;struct Cyc_Core_Impossible_exn_struct*_tmpAAF;(int)_throw((void*)((_tmpAAF=_cycalloc(sizeof(*_tmpAAF)),((_tmpAAF[0]=((_tmpAB1.tag=Cyc_Core_Impossible,((_tmpAB1.f1=((_tmpAB0="impossible type (not pointer)",_tag_dyneither(_tmpAB0,sizeof(char),30))),_tmpAB1)))),_tmpAAF)))));}_LL17:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 463
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmpF7=e->r;void*_tmpF8=_tmpF7;char _tmpFA;struct _dyneither_ptr _tmpFC;short _tmpFE;int _tmp100;long long _tmp102;struct Cyc_Absyn_Exp*_tmp105;struct Cyc_List_List*_tmp107;struct Cyc_List_List*_tmp109;struct Cyc_List_List*_tmp10B;struct Cyc_List_List*_tmp10D;struct Cyc_List_List*_tmp10F;_LL1D: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpF9=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF8;if(_tmpF9->tag != 0)goto _LL1F;else{if(((_tmpF9->f1).Char_c).tag != 2)goto _LL1F;_tmpFA=((struct _tuple4)((_tmpF9->f1).Char_c).val).f2;}}_LL1E:
 return _tmpFA == '\000';_LL1F: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpFB=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF8;if(_tmpFB->tag != 0)goto _LL21;else{if(((_tmpFB->f1).Wchar_c).tag != 3)goto _LL21;_tmpFC=(struct _dyneither_ptr)((_tmpFB->f1).Wchar_c).val;}}_LL20: {
# 467
unsigned long _tmp110=Cyc_strlen((struct _dyneither_ptr)_tmpFC);
int i=0;
if(_tmp110 >= 2  && *((const char*)_check_dyneither_subscript(_tmpFC,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmpFC,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmpFC,sizeof(char),1))== 'x'  && _tmp110 >= 3) && *((const char*)_check_dyneither_subscript(_tmpFC,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp110;++ i){
if(*((const char*)_check_dyneither_subscript(_tmpFC,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 477
return 0;}}_LL21: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpFD=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF8;if(_tmpFD->tag != 0)goto _LL23;else{if(((_tmpFD->f1).Short_c).tag != 4)goto _LL23;_tmpFE=((struct _tuple5)((_tmpFD->f1).Short_c).val).f2;}}_LL22:
 return _tmpFE == 0;_LL23: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpFF=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF8;if(_tmpFF->tag != 0)goto _LL25;else{if(((_tmpFF->f1).Int_c).tag != 5)goto _LL25;_tmp100=((struct _tuple6)((_tmpFF->f1).Int_c).val).f2;}}_LL24:
 return _tmp100 == 0;_LL25: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp101=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF8;if(_tmp101->tag != 0)goto _LL27;else{if(((_tmp101->f1).LongLong_c).tag != 6)goto _LL27;_tmp102=((struct _tuple7)((_tmp101->f1).LongLong_c).val).f2;}}_LL26:
 return _tmp102 == 0;_LL27: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp103=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF8;if(_tmp103->tag != 0)goto _LL29;else{if(((_tmp103->f1).Null_c).tag != 1)goto _LL29;}}_LL28:
 return 1;_LL29: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp104=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpF8;if(_tmp104->tag != 13)goto _LL2B;else{_tmp105=_tmp104->f2;}}_LL2A:
 return Cyc_Toc_is_zero(_tmp105);_LL2B: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp106=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpF8;if(_tmp106->tag != 23)goto _LL2D;else{_tmp107=_tmp106->f1;}}_LL2C:
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmp107);_LL2D: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp108=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpF8;if(_tmp108->tag != 25)goto _LL2F;else{_tmp109=_tmp108->f1;}}_LL2E:
 _tmp10B=_tmp109;goto _LL30;_LL2F: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp10A=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpF8;if(_tmp10A->tag != 28)goto _LL31;else{_tmp10B=_tmp10A->f3;}}_LL30:
 _tmp10D=_tmp10B;goto _LL32;_LL31: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp10C=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpF8;if(_tmp10C->tag != 24)goto _LL33;else{_tmp10D=_tmp10C->f2;}}_LL32:
 _tmp10F=_tmp10D;goto _LL34;_LL33: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp10E=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpF8;if(_tmp10E->tag != 35)goto _LL35;else{_tmp10F=_tmp10E->f2;}}_LL34:
# 488
 for(0;_tmp10F != 0;_tmp10F=_tmp10F->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple19*)_tmp10F->hd)).f2))return 0;}
return 1;_LL35:;_LL36:
 return 0;_LL1C:;}
# 496
static int Cyc_Toc_is_nullable(void*t){
void*_tmp111=Cyc_Tcutil_compress(t);void*_tmp112=_tmp111;union Cyc_Absyn_Constraint*_tmp114;_LL38: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp113=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp112;if(_tmp113->tag != 5)goto _LL3A;else{_tmp114=((_tmp113->f1).ptr_atts).nullable;}}_LL39:
# 499
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp114);_LL3A:;_LL3B: {
const char*_tmpAB4;void*_tmpAB3;(_tmpAB3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAB4="is_nullable",_tag_dyneither(_tmpAB4,sizeof(char),12))),_tag_dyneither(_tmpAB3,sizeof(void*),0)));}_LL37:;}
# 505
static struct _dyneither_ptr Cyc_Toc_collapse_qvar(struct _dyneither_ptr s,struct _tuple1*x){
unsigned int slen=Cyc_strlen((struct _dyneither_ptr)s);
unsigned int len=9 + slen;
union Cyc_Absyn_Nmspace _tmp118;struct _dyneither_ptr _tmp119;struct _tuple1*_tmp117=x;_tmp118=_tmp117->f1;_tmp119=*_tmp117->f2;{
unsigned int vlen=Cyc_strlen((struct _dyneither_ptr)_tmp119);
len +=1 + vlen;{
struct Cyc_List_List*nms;
{union Cyc_Absyn_Nmspace _tmp11A=_tmp118;struct Cyc_List_List*_tmp11B;struct Cyc_List_List*_tmp11C;struct Cyc_List_List*_tmp11D;_LL3D: if((_tmp11A.Loc_n).tag != 4)goto _LL3F;_LL3E:
 nms=0;goto _LL3C;_LL3F: if((_tmp11A.Rel_n).tag != 1)goto _LL41;_tmp11B=(struct Cyc_List_List*)(_tmp11A.Rel_n).val;_LL40:
 nms=_tmp11B;goto _LL3C;_LL41: if((_tmp11A.Abs_n).tag != 2)goto _LL43;_tmp11C=(struct Cyc_List_List*)(_tmp11A.Abs_n).val;_LL42:
 nms=_tmp11C;goto _LL3C;_LL43: if((_tmp11A.C_n).tag != 3)goto _LL3C;_tmp11D=(struct Cyc_List_List*)(_tmp11A.C_n).val;_LL44:
 nms=_tmp11D;goto _LL3C;_LL3C:;}
# 518
{struct Cyc_List_List*_tmp11E=nms;for(0;_tmp11E != 0;_tmp11E=_tmp11E->tl){
len +=1 + Cyc_strlen((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp11E->hd));}}{
char*_tmpAB6;unsigned int _tmpAB5;struct _dyneither_ptr buf=(_tmpAB5=len,((_tmpAB6=_cyccalloc_atomic(sizeof(char),_tmpAB5),_tag_dyneither(_tmpAB6,sizeof(char),_tmpAB5))));
struct _dyneither_ptr p=buf;
Cyc_strcpy(p,(struct _dyneither_ptr)s);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)slen);
for(0;nms != 0;nms=nms->tl){
struct _dyneither_ptr*_tmp11F=(struct _dyneither_ptr*)nms->hd;struct _dyneither_ptr _tmp121;struct _dyneither_ptr*_tmp120=_tmp11F;_tmp121=*_tmp120;
{char _tmpAB9;char _tmpAB8;struct _dyneither_ptr _tmpAB7;(_tmpAB7=p,((_tmpAB8=*((char*)_check_dyneither_subscript(_tmpAB7,sizeof(char),0)),((_tmpAB9='_',((_get_dyneither_size(_tmpAB7,sizeof(char))== 1  && (_tmpAB8 == '\000'  && _tmpAB9 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpAB7.curr)=_tmpAB9)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp121);
_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)Cyc_strlen((struct _dyneither_ptr)_tmp121));}
# 529
{char _tmpABC;char _tmpABB;struct _dyneither_ptr _tmpABA;(_tmpABA=p,((_tmpABB=*((char*)_check_dyneither_subscript(_tmpABA,sizeof(char),0)),((_tmpABC='_',((_get_dyneither_size(_tmpABA,sizeof(char))== 1  && (_tmpABB == '\000'  && _tmpABC != '\000')?_throw_arraybounds(): 1,*((char*)_tmpABA.curr)=_tmpABC)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp119);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)vlen);
{const char*_tmpABD;Cyc_strcpy(p,((_tmpABD="_struct",_tag_dyneither(_tmpABD,sizeof(char),8))));}
return(struct _dyneither_ptr)buf;};};};}struct _tuple20{struct Cyc_Toc_TocState*f1;struct _tuple14*f2;};static char _tmp142[8]="*bogus*";
# 541
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple20*env){
# 544
static struct _dyneither_ptr bogus_string={_tmp142,_tmp142,_tmp142 + 8};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
static struct _tuple14 pair={& bogus_qvar,& bogus_qvar};
# 548
struct _tuple20 _tmp12B=*env;struct Cyc_Dict_Dict*_tmp12D;struct _tuple14*_tmp12E;struct _tuple20 _tmp12C=_tmp12B;_tmp12D=(_tmp12C.f1)->qvar_tags;_tmp12E=_tmp12C.f2;{
struct _tuple14 _tmp12F=*_tmp12E;struct _tuple1*_tmp131;struct _tuple1*_tmp132;struct _tuple14 _tmp130=_tmp12F;_tmp131=_tmp130.f1;_tmp132=_tmp130.f2;{
struct _handler_cons _tmp133;_push_handler(& _tmp133);{int _tmp135=0;if(setjmp(_tmp133.handler))_tmp135=1;if(!_tmp135){
{struct _tuple1*_tmp136=((struct _tuple1*(*)(struct Cyc_Dict_Dict d,int(*cmp)(struct _tuple14*,struct _tuple14*),struct _tuple14*k))Cyc_Dict_lookup_other)(*_tmp12D,Cyc_Toc_qvar_tag_cmp,_tmp12E);_npop_handler(0);return _tmp136;};_pop_handler();}else{void*_tmp134=(void*)_exn_thrown;void*_tmp138=_tmp134;void*_tmp13A;_LL46: {struct Cyc_Dict_Absent_exn_struct*_tmp139=(struct Cyc_Dict_Absent_exn_struct*)_tmp138;if(_tmp139->tag != Cyc_Dict_Absent)goto _LL48;}_LL47: {
# 554
struct _tuple14*_tmpABE;struct _tuple14*_tmp13B=(_tmpABE=_cycalloc(sizeof(*_tmpABE)),((_tmpABE->f1=_tmp131,((_tmpABE->f2=_tmp132,_tmpABE)))));
union Cyc_Absyn_Nmspace _tmp13D;struct _dyneither_ptr _tmp13E;struct _tuple1*_tmp13C=_tmp131;_tmp13D=_tmp13C->f1;_tmp13E=*_tmp13C->f2;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmp13E,_tmp132);
struct _dyneither_ptr*_tmpAC1;struct _tuple1*_tmpAC0;struct _tuple1*res=(_tmpAC0=_cycalloc(sizeof(*_tmpAC0)),((_tmpAC0->f1=_tmp13D,((_tmpAC0->f2=((_tmpAC1=_cycalloc(sizeof(*_tmpAC1)),((_tmpAC1[0]=newvar,_tmpAC1)))),_tmpAC0)))));
*_tmp12D=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmp12D,_tmp13B,res);
return res;};}_LL48: _tmp13A=_tmp138;_LL49:(void)_rethrow(_tmp13A);_LL45:;}};};};}
# 563
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple14 _tmpAC2;struct _tuple14 env=(_tmpAC2.f1=fieldname,((_tmpAC2.f2=dtname,_tmpAC2)));
return((struct _tuple1*(*)(struct _tuple14*arg,struct _tuple1*(*f)(struct _RegionHandle*,struct _tuple20*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_collapse_qvars_body);}
# 568
static void*Cyc_Toc_typ_to_c(void*t);struct _tuple21{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple22{void*f1;struct Cyc_List_List*f2;};
# 572
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple21*env){
# 575
struct _tuple21 _tmp144=*env;struct Cyc_List_List**_tmp146;struct Cyc_List_List*_tmp147;struct _tuple21 _tmp145=_tmp144;_tmp146=(_tmp145.f1)->tuple_types;_tmp147=_tmp145.f2;
# 577
{struct Cyc_List_List*_tmp148=*_tmp146;for(0;_tmp148 != 0;_tmp148=_tmp148->tl){
struct _tuple22*_tmp149=(struct _tuple22*)_tmp148->hd;void*_tmp14B;struct Cyc_List_List*_tmp14C;struct _tuple22*_tmp14A=_tmp149;_tmp14B=_tmp14A->f1;_tmp14C=_tmp14A->f2;{
struct Cyc_List_List*_tmp14D=_tmp147;
for(0;_tmp14D != 0  && _tmp14C != 0;(_tmp14D=_tmp14D->tl,_tmp14C=_tmp14C->tl)){
if(!Cyc_Tcutil_unify((*((struct _tuple11*)_tmp14D->hd)).f2,(void*)_tmp14C->hd))
break;}
if(_tmp14D == 0  && _tmp14C == 0)
return _tmp14B;};}}{
# 587
struct Cyc_Int_pa_PrintArg_struct _tmpACA;void*_tmpAC9[1];const char*_tmpAC8;struct _dyneither_ptr*_tmpAC7;struct _dyneither_ptr*xname=(_tmpAC7=_cycalloc(sizeof(*_tmpAC7)),((_tmpAC7[0]=(struct _dyneither_ptr)((_tmpACA.tag=1,((_tmpACA.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpAC9[0]=& _tmpACA,Cyc_aprintf(((_tmpAC8="_tuple%d",_tag_dyneither(_tmpAC8,sizeof(char),9))),_tag_dyneither(_tmpAC9,sizeof(void*),1)))))))),_tmpAC7)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,_tmp147);
struct Cyc_List_List*_tmp14E=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
struct Cyc_Absyn_Aggrfield*_tmpACD;struct Cyc_List_List*_tmpACC;_tmp14E=((_tmpACC=_cycalloc(sizeof(*_tmpACC)),((_tmpACC->hd=((_tmpACD=_cycalloc(sizeof(*_tmpACD)),((_tmpACD->name=Cyc_Absyn_fieldname(i),((_tmpACD->tq=Cyc_Toc_mt_tq,((_tmpACD->type=(void*)ts2->hd,((_tmpACD->width=0,((_tmpACD->attributes=0,((_tmpACD->requires_clause=0,_tmpACD)))))))))))))),((_tmpACC->tl=_tmp14E,_tmpACC))))));}}
_tmp14E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp14E);{
struct Cyc_Absyn_AggrdeclImpl*_tmpAD2;struct _tuple1*_tmpAD1;struct Cyc_Absyn_Aggrdecl*_tmpAD0;struct Cyc_Absyn_Aggrdecl*_tmp151=(_tmpAD0=_cycalloc(sizeof(*_tmpAD0)),((_tmpAD0->kind=Cyc_Absyn_StructA,((_tmpAD0->sc=Cyc_Absyn_Public,((_tmpAD0->name=(
(_tmpAD1=_cycalloc(sizeof(*_tmpAD1)),((_tmpAD1->f1=Cyc_Absyn_Rel_n(0),((_tmpAD1->f2=xname,_tmpAD1)))))),((_tmpAD0->tvs=0,((_tmpAD0->impl=(
(_tmpAD2=_cycalloc(sizeof(*_tmpAD2)),((_tmpAD2->exist_vars=0,((_tmpAD2->rgn_po=0,((_tmpAD2->fields=_tmp14E,((_tmpAD2->tagged=0,_tmpAD2)))))))))),((_tmpAD0->attributes=0,((_tmpAD0->expected_mem_kind=0,_tmpAD0)))))))))))))));
# 602
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpAD8;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpAD7;struct Cyc_List_List*_tmpAD6;Cyc_Toc_result_decls=((_tmpAD6=_cycalloc(sizeof(*_tmpAD6)),((_tmpAD6->hd=Cyc_Absyn_new_decl((void*)((_tmpAD8=_cycalloc(sizeof(*_tmpAD8)),((_tmpAD8[0]=((_tmpAD7.tag=5,((_tmpAD7.f1=_tmp151,_tmpAD7)))),_tmpAD8)))),0),((_tmpAD6->tl=Cyc_Toc_result_decls,_tmpAD6))))));}
{struct _tuple22*_tmpADB;struct Cyc_List_List*_tmpADA;*_tmp146=((_tmpADA=_region_malloc(d,sizeof(*_tmpADA)),((_tmpADA->hd=((_tmpADB=_region_malloc(d,sizeof(*_tmpADB)),((_tmpADB->f1=x,((_tmpADB->f2=ts,_tmpADB)))))),((_tmpADA->tl=*_tmp146,_tmpADA))))));}
return x;};};}
# 607
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return((void*(*)(struct Cyc_List_List*arg,void*(*f)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state)(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple23{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple24{struct Cyc_Toc_TocState*f1;struct _tuple23*f2;};struct _tuple25{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 616
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple24*env){
# 624
struct _tuple24 _tmp15E=*env;struct Cyc_List_List**_tmp160;struct _tuple1*_tmp161;struct Cyc_List_List*_tmp162;struct Cyc_List_List*_tmp163;struct Cyc_List_List*_tmp164;struct _tuple24 _tmp15F=_tmp15E;_tmp160=(_tmp15F.f1)->abs_struct_types;_tmp161=(_tmp15F.f2)->f1;_tmp162=(_tmp15F.f2)->f2;_tmp163=(_tmp15F.f2)->f3;_tmp164=(_tmp15F.f2)->f4;
# 629
{struct Cyc_List_List*_tmp165=*_tmp160;for(0;_tmp165 != 0;_tmp165=_tmp165->tl){
struct _tuple25*_tmp166=(struct _tuple25*)_tmp165->hd;struct _tuple1*_tmp168;struct Cyc_List_List*_tmp169;void*_tmp16A;struct _tuple25*_tmp167=_tmp166;_tmp168=_tmp167->f1;_tmp169=_tmp167->f2;_tmp16A=_tmp167->f3;
# 632
if(Cyc_Absyn_qvar_cmp(_tmp168,_tmp161)== 0  && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp163)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp169)){
int okay=1;
{struct Cyc_List_List*_tmp16B=_tmp163;for(0;_tmp16B != 0;(_tmp16B=_tmp16B->tl,_tmp169=_tmp169->tl)){
void*_tmp16C=(void*)_tmp16B->hd;
void*_tmp16D=(void*)((struct Cyc_List_List*)_check_null(_tmp169))->hd;
# 639
{struct Cyc_Absyn_Kind*_tmp16E=Cyc_Tcutil_typ_kind(_tmp16C);struct Cyc_Absyn_Kind*_tmp16F=_tmp16E;_LL4B: if(_tmp16F->kind != Cyc_Absyn_EffKind)goto _LL4D;_LL4C:
 goto _LL4E;_LL4D: if(_tmp16F->kind != Cyc_Absyn_RgnKind)goto _LL4F;_LL4E:
# 644
 continue;_LL4F:;_LL50:
# 647
 if(Cyc_Tcutil_unify(_tmp16C,_tmp16D) || Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(_tmp16C),Cyc_Toc_typ_to_c(_tmp16D)))
# 649
continue;
# 652
okay=0;
goto _LL4A;_LL4A:;}
# 655
break;}}
# 657
if(okay)
# 659
return _tmp16A;}}}{
# 667
struct Cyc_Int_pa_PrintArg_struct _tmpAE3;void*_tmpAE2[1];const char*_tmpAE1;struct _dyneither_ptr*_tmpAE0;struct _dyneither_ptr*xname=(_tmpAE0=_cycalloc(sizeof(*_tmpAE0)),((_tmpAE0[0]=(struct _dyneither_ptr)((_tmpAE3.tag=1,((_tmpAE3.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpAE2[0]=& _tmpAE3,Cyc_aprintf(((_tmpAE1="_tuple%d",_tag_dyneither(_tmpAE1,sizeof(char),9))),_tag_dyneither(_tmpAE2,sizeof(void*),1)))))))),_tmpAE0)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp170=0;
# 671
{struct _tuple25*_tmpAE6;struct Cyc_List_List*_tmpAE5;*_tmp160=((_tmpAE5=_region_malloc(d,sizeof(*_tmpAE5)),((_tmpAE5->hd=((_tmpAE6=_region_malloc(d,sizeof(*_tmpAE6)),((_tmpAE6->f1=_tmp161,((_tmpAE6->f2=_tmp163,((_tmpAE6->f3=x,_tmpAE6)))))))),((_tmpAE5->tl=*_tmp160,_tmpAE5))))));}{
# 674
struct _RegionHandle _tmp173=_new_region("r");struct _RegionHandle*r=& _tmp173;_push_region(r);
{struct Cyc_List_List*_tmp174=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp162,_tmp163);
for(0;_tmp164 != 0;_tmp164=_tmp164->tl){
struct Cyc_Absyn_Aggrfield*_tmp175=(struct Cyc_Absyn_Aggrfield*)_tmp164->hd;
void*t=_tmp175->type;
struct Cyc_List_List*atts=_tmp175->attributes;
# 682
if(_tmp164->tl == 0  && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(t))){
struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpAEC;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpAEB;struct Cyc_List_List*_tmpAEA;atts=((_tmpAEA=_cycalloc(sizeof(*_tmpAEA)),((_tmpAEA->hd=(void*)((_tmpAEC=_cycalloc(sizeof(*_tmpAEC)),((_tmpAEC[0]=((_tmpAEB.tag=6,((_tmpAEB.f1=0,_tmpAEB)))),_tmpAEC)))),((_tmpAEA->tl=atts,_tmpAEA))))));}
# 685
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp174,t));
# 688
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val)){
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpAF2;struct Cyc_Absyn_ArrayInfo _tmpAF1;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpAF0;t=(void*)((_tmpAF0=_cycalloc(sizeof(*_tmpAF0)),((_tmpAF0[0]=((_tmpAF2.tag=8,((_tmpAF2.f1=((_tmpAF1.elt_type=Cyc_Absyn_void_star_typ(),((_tmpAF1.tq=Cyc_Absyn_empty_tqual(0),((_tmpAF1.num_elts=
Cyc_Absyn_uint_exp(0,0),((_tmpAF1.zero_term=((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0),((_tmpAF1.zt_loc=0,_tmpAF1)))))))))),_tmpAF2)))),_tmpAF0))));}{
# 692
struct Cyc_Absyn_Aggrfield*_tmpAF5;struct Cyc_List_List*_tmpAF4;_tmp170=((_tmpAF4=_cycalloc(sizeof(*_tmpAF4)),((_tmpAF4->hd=((_tmpAF5=_cycalloc(sizeof(*_tmpAF5)),((_tmpAF5->name=_tmp175->name,((_tmpAF5->tq=Cyc_Toc_mt_tq,((_tmpAF5->type=t,((_tmpAF5->width=_tmp175->width,((_tmpAF5->attributes=atts,((_tmpAF5->requires_clause=0,_tmpAF5)))))))))))))),((_tmpAF4->tl=_tmp170,_tmpAF4))))));};}
# 694
_tmp170=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp170);{
struct Cyc_Absyn_AggrdeclImpl*_tmpAFA;struct _tuple1*_tmpAF9;struct Cyc_Absyn_Aggrdecl*_tmpAF8;struct Cyc_Absyn_Aggrdecl*_tmp17E=(_tmpAF8=_cycalloc(sizeof(*_tmpAF8)),((_tmpAF8->kind=Cyc_Absyn_StructA,((_tmpAF8->sc=Cyc_Absyn_Public,((_tmpAF8->name=(
(_tmpAF9=_cycalloc(sizeof(*_tmpAF9)),((_tmpAF9->f1=Cyc_Absyn_Rel_n(0),((_tmpAF9->f2=xname,_tmpAF9)))))),((_tmpAF8->tvs=0,((_tmpAF8->impl=(
(_tmpAFA=_cycalloc(sizeof(*_tmpAFA)),((_tmpAFA->exist_vars=0,((_tmpAFA->rgn_po=0,((_tmpAFA->fields=_tmp170,((_tmpAFA->tagged=0,_tmpAFA)))))))))),((_tmpAF8->attributes=0,((_tmpAF8->expected_mem_kind=0,_tmpAF8)))))))))))))));
# 702
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpB00;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpAFF;struct Cyc_List_List*_tmpAFE;Cyc_Toc_result_decls=((_tmpAFE=_cycalloc(sizeof(*_tmpAFE)),((_tmpAFE->hd=Cyc_Absyn_new_decl((void*)((_tmpB00=_cycalloc(sizeof(*_tmpB00)),((_tmpB00[0]=((_tmpAFF.tag=5,((_tmpAFF.f1=_tmp17E,_tmpAFF)))),_tmpB00)))),0),((_tmpAFE->tl=Cyc_Toc_result_decls,_tmpAFE))))));}{
void*_tmp182=x;_npop_handler(0);return _tmp182;};};}
# 675
;_pop_region(r);};};}
# 706
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 710
struct _tuple23 _tmpB01;struct _tuple23 env=(_tmpB01.f1=struct_name,((_tmpB01.f2=type_vars,((_tmpB01.f3=type_args,((_tmpB01.f4=fields,_tmpB01)))))));
return((void*(*)(struct _tuple23*arg,void*(*f)(struct _RegionHandle*,struct _tuple24*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 717
struct _tuple1*Cyc_Toc_temp_var(){
int _tmp18B=Cyc_Toc_temp_var_counter ++;
struct _dyneither_ptr*_tmpB0E;const char*_tmpB0D;void*_tmpB0C[1];struct Cyc_Int_pa_PrintArg_struct _tmpB0B;struct _tuple1*_tmpB0A;struct _tuple1*res=(_tmpB0A=_cycalloc(sizeof(*_tmpB0A)),((_tmpB0A->f1=Cyc_Absyn_Loc_n,((_tmpB0A->f2=((_tmpB0E=_cycalloc(sizeof(*_tmpB0E)),((_tmpB0E[0]=(struct _dyneither_ptr)((_tmpB0B.tag=1,((_tmpB0B.f1=(unsigned int)_tmp18B,((_tmpB0C[0]=& _tmpB0B,Cyc_aprintf(((_tmpB0D="_tmp%X",_tag_dyneither(_tmpB0D,sizeof(char),7))),_tag_dyneither(_tmpB0C,sizeof(void*),1)))))))),_tmpB0E)))),_tmpB0A)))));
return res;}struct _tuple26{struct Cyc_Toc_TocState*f1;int f2;};
# 725
static struct _dyneither_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple26*env){
struct _tuple26 _tmp191=*env;struct Cyc_Xarray_Xarray*_tmp193;struct _tuple26 _tmp192=_tmp191;_tmp193=(_tmp192.f1)->temp_labels;{
int _tmp194=Cyc_Toc_fresh_label_counter ++;
if(_tmp194 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp193))
return((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp193,_tmp194);{
struct Cyc_Int_pa_PrintArg_struct _tmpB16;void*_tmpB15[1];const char*_tmpB14;struct _dyneither_ptr*_tmpB13;struct _dyneither_ptr*res=(_tmpB13=_cycalloc(sizeof(*_tmpB13)),((_tmpB13[0]=(struct _dyneither_ptr)((_tmpB16.tag=1,((_tmpB16.f1=(unsigned int)_tmp194,((_tmpB15[0]=& _tmpB16,Cyc_aprintf(((_tmpB14="_LL%X",_tag_dyneither(_tmpB14,sizeof(char),6))),_tag_dyneither(_tmpB15,sizeof(void*),1)))))))),_tmpB13)));
if(((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp193,res)!= _tmp194){
const char*_tmpB19;void*_tmpB18;(_tmpB18=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB19="fresh_label: add_ind returned bad index...",_tag_dyneither(_tmpB19,sizeof(char),43))),_tag_dyneither(_tmpB18,sizeof(void*),0)));}
return res;};};}
# 736
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
return((struct _dyneither_ptr*(*)(int arg,struct _dyneither_ptr*(*f)(struct _RegionHandle*,struct _tuple26*)))Cyc_Toc_use_toc_state)(0,Cyc_Toc_fresh_label_body);}
# 743
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
struct Cyc_List_List*_tmp19B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp19B))->hd)->name)!= 0){
++ ans;
_tmp19B=_tmp19B->tl;}
# 750
return Cyc_Absyn_uint_exp((unsigned int)ans,0);}
# 756
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_result_decls);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
# 761
struct _tuple9 _tmp19C=*a;struct _dyneither_ptr*_tmp19E;struct Cyc_Absyn_Tqual _tmp19F;void*_tmp1A0;struct _tuple9 _tmp19D=_tmp19C;_tmp19E=_tmp19D.f1;_tmp19F=_tmp19D.f2;_tmp1A0=_tmp19D.f3;{
struct _tuple9*_tmpB1A;return(_tmpB1A=_cycalloc(sizeof(*_tmpB1A)),((_tmpB1A->f1=_tmp19E,((_tmpB1A->f2=_tmp19F,((_tmpB1A->f3=Cyc_Toc_typ_to_c(_tmp1A0),_tmpB1A)))))));};}
# 764
static struct _tuple11*Cyc_Toc_typ_to_c_f(struct _tuple11*x){
struct _tuple11 _tmp1A2=*x;struct Cyc_Absyn_Tqual _tmp1A4;void*_tmp1A5;struct _tuple11 _tmp1A3=_tmp1A2;_tmp1A4=_tmp1A3.f1;_tmp1A5=_tmp1A3.f2;{
struct _tuple11*_tmpB1B;return(_tmpB1B=_cycalloc(sizeof(*_tmpB1B)),((_tmpB1B->f1=_tmp1A4,((_tmpB1B->f2=Cyc_Toc_typ_to_c(_tmp1A5),_tmpB1B)))));};}
# 783 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp1A7=Cyc_Tcutil_compress(t);void*_tmp1A8=_tmp1A7;void*_tmp1AA;struct Cyc_Absyn_Tqual _tmp1AB;struct Cyc_Absyn_Exp*_tmp1AC;union Cyc_Absyn_Constraint*_tmp1AD;unsigned int _tmp1AE;void*_tmp1B0;_LL52: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1A9=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1A8;if(_tmp1A9->tag != 8)goto _LL54;else{_tmp1AA=(_tmp1A9->f1).elt_type;_tmp1AB=(_tmp1A9->f1).tq;_tmp1AC=(_tmp1A9->f1).num_elts;_tmp1AD=(_tmp1A9->f1).zero_term;_tmp1AE=(_tmp1A9->f1).zt_loc;}}_LL53:
# 786
 return Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(_tmp1AA),_tmp1AB);_LL54:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp1AF=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A8;if(_tmp1AF->tag != 1)goto _LL56;else{_tmp1B0=(void*)_tmp1AF->f2;}}if(!(_tmp1B0 != 0))goto _LL56;_LL55:
 return Cyc_Toc_typ_to_c_array(_tmp1B0);_LL56:;_LL57:
 return Cyc_Toc_typ_to_c(t);_LL51:;}
# 792
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){
# 794
struct Cyc_Absyn_Aggrfield*_tmpB1C;return(_tmpB1C=_cycalloc(sizeof(*_tmpB1C)),((_tmpB1C->name=f->name,((_tmpB1C->tq=Cyc_Toc_mt_tq,((_tmpB1C->type=
# 796
Cyc_Toc_typ_to_c(f->type),((_tmpB1C->width=f->width,((_tmpB1C->attributes=f->attributes,((_tmpB1C->requires_clause=0,_tmpB1C)))))))))))));}
# 801
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 803
return;}
# 806
static void*Cyc_Toc_char_star_typ(){
static void**cs=0;
if(cs == 0){
void**_tmpB1D;cs=((_tmpB1D=_cycalloc(sizeof(*_tmpB1D)),((_tmpB1D[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpB1D))));}
# 811
return*cs;}
# 813
static void*Cyc_Toc_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpB1E;r=((_tmpB1E=_cycalloc(sizeof(*_tmpB1E)),((_tmpB1E[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),_tmpB1E))));}
# 818
return*r;}
# 820
static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpB1F;r=((_tmpB1F=_cycalloc(sizeof(*_tmpB1F)),((_tmpB1F[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),_tmpB1F))));}
# 825
return*r;}
# 827
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp1B5=Cyc_Tcutil_compress(t);void*_tmp1B6=_tmp1B5;struct Cyc_Absyn_Tvar*_tmp1B8;_LL59: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1B7=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1B6;if(_tmp1B7->tag != 2)goto _LL5B;else{_tmp1B8=_tmp1B7->f1;}}_LL5A:
# 830
 return Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tbk);_LL5B:;_LL5C:
 return 0;_LL58:;}
# 834
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp1B9=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp1BA=_tmp1B9;_LL5E: if(_tmp1BA->kind != Cyc_Absyn_AnyKind)goto _LL60;_LL5F:
 return 1;_LL60:;_LL61:
 return 0;_LL5D:;}
# 841
static void*Cyc_Toc_typ_to_c(void*t){
void*_tmp1BB=t;void**_tmp1BE;struct Cyc_Absyn_Tvar*_tmp1C0;struct Cyc_Absyn_Datatypedecl*_tmp1C3;struct Cyc_Absyn_Datatypefield*_tmp1C4;void*_tmp1C7;struct Cyc_Absyn_Tqual _tmp1C8;union Cyc_Absyn_Constraint*_tmp1C9;void*_tmp1CD;struct Cyc_Absyn_Tqual _tmp1CE;struct Cyc_Absyn_Exp*_tmp1CF;unsigned int _tmp1D0;struct Cyc_Absyn_Tqual _tmp1D2;void*_tmp1D3;struct Cyc_List_List*_tmp1D4;int _tmp1D5;struct Cyc_Absyn_VarargInfo*_tmp1D6;struct Cyc_List_List*_tmp1D7;struct Cyc_List_List*_tmp1D9;enum Cyc_Absyn_AggrKind _tmp1DB;struct Cyc_List_List*_tmp1DC;union Cyc_Absyn_AggrInfoU _tmp1DE;struct Cyc_List_List*_tmp1DF;struct _tuple1*_tmp1E1;struct Cyc_List_List*_tmp1E3;struct _tuple1*_tmp1E5;struct Cyc_List_List*_tmp1E6;struct Cyc_Absyn_Typedefdecl*_tmp1E7;void*_tmp1E8;void*_tmp1EB;struct Cyc_Absyn_Exp*_tmp1F4;struct Cyc_Absyn_Exp*_tmp1F6;struct Cyc_Absyn_Aggrdecl*_tmp1FA;struct Cyc_Absyn_Enumdecl*_tmp1FD;struct Cyc_Absyn_Datatypedecl*_tmp200;void**_tmp201;_LL63: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp1BC=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp1BB;if(_tmp1BC->tag != 0)goto _LL65;}_LL64:
 return t;_LL65: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp1BD=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1BB;if(_tmp1BD->tag != 1)goto _LL67;else{_tmp1BE=(void**)((void**)& _tmp1BD->f2);}}_LL66:
# 845
 if(*_tmp1BE == 0){
*_tmp1BE=Cyc_Absyn_sint_typ;
return Cyc_Absyn_sint_typ;}
# 849
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp1BE));_LL67: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1BF=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1BB;if(_tmp1BF->tag != 2)goto _LL69;else{_tmp1C0=_tmp1BF->f1;}}_LL68:
# 851
 if((Cyc_Tcutil_tvar_kind(_tmp1C0,& Cyc_Tcutil_bk))->kind == Cyc_Absyn_AnyKind)
# 854
return(void*)& Cyc_Absyn_VoidType_val;else{
# 856
return Cyc_Absyn_void_star_typ();}_LL69: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp1C1=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1BB;if(_tmp1C1->tag != 3)goto _LL6B;}_LL6A:
# 858
 return(void*)& Cyc_Absyn_VoidType_val;_LL6B: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp1C2=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1BB;if(_tmp1C2->tag != 4)goto _LL6D;else{if((((_tmp1C2->f1).field_info).KnownDatatypefield).tag != 2)goto _LL6D;_tmp1C3=((struct _tuple2)(((_tmp1C2->f1).field_info).KnownDatatypefield).val).f1;_tmp1C4=((struct _tuple2)(((_tmp1C2->f1).field_info).KnownDatatypefield).val).f2;}}_LL6C:
# 860
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp1C4->name,_tmp1C3->name));_LL6D: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp1C5=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1BB;if(_tmp1C5->tag != 4)goto _LL6F;}_LL6E: {
const char*_tmpB22;void*_tmpB21;(_tmpB21=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB22="unresolved DatatypeFieldType",_tag_dyneither(_tmpB22,sizeof(char),29))),_tag_dyneither(_tmpB21,sizeof(void*),0)));}_LL6F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1C6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BB;if(_tmp1C6->tag != 5)goto _LL71;else{_tmp1C7=(_tmp1C6->f1).elt_typ;_tmp1C8=(_tmp1C6->f1).elt_tq;_tmp1C9=((_tmp1C6->f1).ptr_atts).bounds;}}_LL70:
# 865
 _tmp1C7=Cyc_Toc_typ_to_c(_tmp1C7);{
void*_tmp204=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp1C9);void*_tmp205=_tmp204;_LLA4: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp206=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp205;if(_tmp206->tag != 0)goto _LLA6;}_LLA5:
 return Cyc_Toc_dyneither_ptr_typ;_LLA6:;_LLA7:
 return Cyc_Absyn_star_typ(_tmp1C7,(void*)& Cyc_Absyn_HeapRgn_val,_tmp1C8,Cyc_Absyn_false_conref);_LLA3:;};_LL71: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp1CA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1BB;if(_tmp1CA->tag != 6)goto _LL73;}_LL72:
# 870
 goto _LL74;_LL73: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp1CB=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp1BB;if(_tmp1CB->tag != 7)goto _LL75;}_LL74:
 return t;_LL75: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1CC=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1BB;if(_tmp1CC->tag != 8)goto _LL77;else{_tmp1CD=(_tmp1CC->f1).elt_type;_tmp1CE=(_tmp1CC->f1).tq;_tmp1CF=(_tmp1CC->f1).num_elts;_tmp1D0=(_tmp1CC->f1).zt_loc;}}_LL76:
# 873
 return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c(_tmp1CD),_tmp1CE,_tmp1CF,Cyc_Absyn_false_conref,_tmp1D0);_LL77: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1D1=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BB;if(_tmp1D1->tag != 9)goto _LL79;else{_tmp1D2=(_tmp1D1->f1).ret_tqual;_tmp1D3=(_tmp1D1->f1).ret_typ;_tmp1D4=(_tmp1D1->f1).args;_tmp1D5=(_tmp1D1->f1).c_varargs;_tmp1D6=(_tmp1D1->f1).cyc_varargs;_tmp1D7=(_tmp1D1->f1).attributes;}}_LL78: {
# 879
struct Cyc_List_List*_tmp207=0;
for(0;_tmp1D7 != 0;_tmp1D7=_tmp1D7->tl){
void*_tmp208=(void*)_tmp1D7->hd;void*_tmp209=_tmp208;_LLA9: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp20A=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp209;if(_tmp20A->tag != 4)goto _LLAB;}_LLAA:
 goto _LLAC;_LLAB: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp20B=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp209;if(_tmp20B->tag != 5)goto _LLAD;}_LLAC:
 goto _LLAE;_LLAD: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp20C=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp209;if(_tmp20C->tag != 19)goto _LLAF;}_LLAE:
 continue;_LLAF: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp20D=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp209;if(_tmp20D->tag != 22)goto _LLB1;}_LLB0:
 continue;_LLB1: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp20E=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp209;if(_tmp20E->tag != 21)goto _LLB3;}_LLB2:
 continue;_LLB3: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp20F=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp209;if(_tmp20F->tag != 20)goto _LLB5;}_LLB4:
 continue;_LLB5:;_LLB6:
{struct Cyc_List_List*_tmpB23;_tmp207=((_tmpB23=_cycalloc(sizeof(*_tmpB23)),((_tmpB23->hd=(void*)_tmp1D7->hd,((_tmpB23->tl=_tmp207,_tmpB23))))));}goto _LLA8;_LLA8:;}{
# 890
struct Cyc_List_List*_tmp211=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp1D4);
if(_tmp1D6 != 0){
# 893
void*_tmp212=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp1D6->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple9*_tmpB24;struct _tuple9*_tmp213=(_tmpB24=_cycalloc(sizeof(*_tmpB24)),((_tmpB24->f1=_tmp1D6->name,((_tmpB24->f2=_tmp1D6->tq,((_tmpB24->f3=_tmp212,_tmpB24)))))));
struct Cyc_List_List*_tmpB25;_tmp211=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp211,((_tmpB25=_cycalloc(sizeof(*_tmpB25)),((_tmpB25->hd=_tmp213,((_tmpB25->tl=0,_tmpB25)))))));}{
# 897
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpB2B;struct Cyc_Absyn_FnInfo _tmpB2A;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpB29;return(void*)((_tmpB29=_cycalloc(sizeof(*_tmpB29)),((_tmpB29[0]=((_tmpB2B.tag=9,((_tmpB2B.f1=((_tmpB2A.tvars=0,((_tmpB2A.effect=0,((_tmpB2A.ret_tqual=_tmp1D2,((_tmpB2A.ret_typ=Cyc_Toc_typ_to_c(_tmp1D3),((_tmpB2A.args=_tmp211,((_tmpB2A.c_varargs=_tmp1D5,((_tmpB2A.cyc_varargs=0,((_tmpB2A.rgn_po=0,((_tmpB2A.attributes=_tmp207,((_tmpB2A.requires_clause=0,((_tmpB2A.requires_relns=0,((_tmpB2A.ensures_clause=0,((_tmpB2A.ensures_relns=0,_tmpB2A)))))))))))))))))))))))))),_tmpB2B)))),_tmpB29))));};};}_LL79: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp1D8=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1BB;if(_tmp1D8->tag != 10)goto _LL7B;else{_tmp1D9=_tmp1D8->f1;}}_LL7A:
# 902
 _tmp1D9=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp1D9);
return Cyc_Toc_add_tuple_type(_tmp1D9);_LL7B: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1DA=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1BB;if(_tmp1DA->tag != 12)goto _LL7D;else{_tmp1DB=_tmp1DA->f1;_tmp1DC=_tmp1DA->f2;}}_LL7C: {
# 907
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpB2E;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpB2D;return(void*)((_tmpB2D=_cycalloc(sizeof(*_tmpB2D)),((_tmpB2D[0]=((_tmpB2E.tag=12,((_tmpB2E.f1=_tmp1DB,((_tmpB2E.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp1DC),_tmpB2E)))))),_tmpB2D))));}_LL7D: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1DD=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1BB;if(_tmp1DD->tag != 11)goto _LL7F;else{_tmp1DE=(_tmp1DD->f1).aggr_info;_tmp1DF=(_tmp1DD->f1).targs;}}_LL7E:
# 911
{union Cyc_Absyn_AggrInfoU _tmp21B=_tmp1DE;_LLB8: if((_tmp21B.UnknownAggr).tag != 1)goto _LLBA;_LLB9:
 return t;_LLBA:;_LLBB:
 goto _LLB7;_LLB7:;}{
# 915
struct Cyc_Absyn_Aggrdecl*_tmp21C=Cyc_Absyn_get_known_aggrdecl(_tmp1DE);
if(_tmp21C->expected_mem_kind){
# 918
if(_tmp21C->impl == 0){
const char*_tmpB37;void*_tmpB36[2];const char*_tmpB35;const char*_tmpB34;struct Cyc_String_pa_PrintArg_struct _tmpB33;struct Cyc_String_pa_PrintArg_struct _tmpB32;(_tmpB32.tag=0,((_tmpB32.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 921
Cyc_Absynpp_qvar2string(_tmp21C->name)),((_tmpB33.tag=0,((_tmpB33.f1=(struct _dyneither_ptr)(
# 920
_tmp21C->kind == Cyc_Absyn_UnionA?(_tmpB34="union",_tag_dyneither(_tmpB34,sizeof(char),6)):((_tmpB35="struct",_tag_dyneither(_tmpB35,sizeof(char),7)))),((_tmpB36[0]=& _tmpB33,((_tmpB36[1]=& _tmpB32,Cyc_Tcutil_warn(0,((_tmpB37="%s %s was never defined.",_tag_dyneither(_tmpB37,sizeof(char),25))),_tag_dyneither(_tmpB36,sizeof(void*),2)))))))))))));}}
# 926
if(_tmp21C->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp21C->name,Cyc_Absyn_unionq_typ);{
struct Cyc_List_List*_tmp223=_tmp21C->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp21C->impl))->fields;
if(_tmp223 == 0)return Cyc_Toc_aggrdecl_type(_tmp21C->name,Cyc_Absyn_strctq);
for(0;_tmp223->tl != 0;_tmp223=_tmp223->tl){;}{
void*_tmp224=((struct Cyc_Absyn_Aggrfield*)_tmp223->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp224))){
if(_tmp21C->expected_mem_kind){
const char*_tmpB3B;void*_tmpB3A[1];struct Cyc_String_pa_PrintArg_struct _tmpB39;(_tmpB39.tag=0,((_tmpB39.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp21C->name)),((_tmpB3A[0]=& _tmpB39,Cyc_Tcutil_warn(0,((_tmpB3B="struct %s ended up being abstract.",_tag_dyneither(_tmpB3B,sizeof(char),35))),_tag_dyneither(_tmpB3A,sizeof(void*),1)))))));}{
# 940
struct _RegionHandle _tmp228=_new_region("r");struct _RegionHandle*r=& _tmp228;_push_region(r);
{struct Cyc_List_List*_tmp229=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp21C->tvs,_tmp1DF);
void*_tmp22A=Cyc_Tcutil_rsubstitute(r,_tmp229,_tmp224);
if(Cyc_Toc_is_abstract_type(_tmp22A)){void*_tmp22B=Cyc_Toc_aggrdecl_type(_tmp21C->name,Cyc_Absyn_strctq);_npop_handler(0);return _tmp22B;}{
void*_tmp22C=Cyc_Toc_add_struct_type(_tmp21C->name,_tmp21C->tvs,_tmp1DF,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp21C->impl))->fields);_npop_handler(0);return _tmp22C;};}
# 941
;_pop_region(r);};}
# 946
return Cyc_Toc_aggrdecl_type(_tmp21C->name,Cyc_Absyn_strctq);};};};_LL7F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp1E0=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp1BB;if(_tmp1E0->tag != 13)goto _LL81;else{_tmp1E1=_tmp1E0->f1;}}_LL80:
 return t;_LL81: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp1E2=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1BB;if(_tmp1E2->tag != 14)goto _LL83;else{_tmp1E3=_tmp1E2->f1;}}_LL82:
 Cyc_Toc_enumfields_to_c(_tmp1E3);return t;_LL83: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp1E4=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1BB;if(_tmp1E4->tag != 17)goto _LL85;else{_tmp1E5=_tmp1E4->f1;_tmp1E6=_tmp1E4->f2;_tmp1E7=_tmp1E4->f3;_tmp1E8=(void*)_tmp1E4->f4;}}_LL84:
# 950
 if(_tmp1E8 == 0  || Cyc_noexpand_r){
if(_tmp1E6 != 0){
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpB3E;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpB3D;return(void*)((_tmpB3D=_cycalloc(sizeof(*_tmpB3D)),((_tmpB3D[0]=((_tmpB3E.tag=17,((_tmpB3E.f1=_tmp1E5,((_tmpB3E.f2=0,((_tmpB3E.f3=_tmp1E7,((_tmpB3E.f4=0,_tmpB3E)))))))))),_tmpB3D))));}else{
return t;}}else{
# 955
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpB41;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpB40;return(void*)((_tmpB40=_cycalloc(sizeof(*_tmpB40)),((_tmpB40[0]=((_tmpB41.tag=17,((_tmpB41.f1=_tmp1E5,((_tmpB41.f2=0,((_tmpB41.f3=_tmp1E7,((_tmpB41.f4=Cyc_Toc_typ_to_c(_tmp1E8),_tmpB41)))))))))),_tmpB40))));}_LL85: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp1E9=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp1BB;if(_tmp1E9->tag != 19)goto _LL87;}_LL86:
 return Cyc_Absyn_uint_typ;_LL87: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp1EA=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1BB;if(_tmp1EA->tag != 15)goto _LL89;else{_tmp1EB=(void*)_tmp1EA->f1;}}_LL88:
 return Cyc_Toc_rgn_typ();_LL89: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp1EC=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp1BB;if(_tmp1EC->tag != 16)goto _LL8B;}_LL8A:
 return Cyc_Toc_dyn_rgn_typ();_LL8B: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp1ED=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp1BB;if(_tmp1ED->tag != 20)goto _LL8D;}_LL8C:
# 961
 goto _LL8E;_LL8D: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp1EE=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp1BB;if(_tmp1EE->tag != 21)goto _LL8F;}_LL8E:
 goto _LL90;_LL8F: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp1EF=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp1BB;if(_tmp1EF->tag != 22)goto _LL91;}_LL90:
 goto _LL92;_LL91: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1F0=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp1BB;if(_tmp1F0->tag != 23)goto _LL93;}_LL92:
 goto _LL94;_LL93: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1F1=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp1BB;if(_tmp1F1->tag != 24)goto _LL95;}_LL94:
 goto _LL96;_LL95: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1F2=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp1BB;if(_tmp1F2->tag != 25)goto _LL97;}_LL96:
 return Cyc_Absyn_void_star_typ();_LL97: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1F3=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp1BB;if(_tmp1F3->tag != 18)goto _LL99;else{_tmp1F4=_tmp1F3->f1;}}_LL98:
# 971
 return t;_LL99: {struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp1F5=(struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp1BB;if(_tmp1F5->tag != 27)goto _LL9B;else{_tmp1F6=_tmp1F5->f1;}}_LL9A:
# 973
 return t;_LL9B: {struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*_tmp1F7=(struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp1BB;if(_tmp1F7->tag != 28)goto _LL9D;}_LL9C:
 return t;_LL9D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1F8=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1BB;if(_tmp1F8->tag != 26)goto _LL9F;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp1F9=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp1F8->f1)->r;if(_tmp1F9->tag != 0)goto _LL9F;else{_tmp1FA=_tmp1F9->f1;}}}_LL9E:
# 976
 Cyc_Toc_aggrdecl_to_c(_tmp1FA,1);
if(_tmp1FA->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp1FA->name,Cyc_Absyn_unionq_typ);else{
return Cyc_Toc_aggrdecl_type(_tmp1FA->name,Cyc_Absyn_strctq);}_LL9F: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1FB=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1BB;if(_tmp1FB->tag != 26)goto _LLA1;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp1FC=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp1FB->f1)->r;if(_tmp1FC->tag != 1)goto _LLA1;else{_tmp1FD=_tmp1FC->f1;}}}_LLA0:
# 981
 Cyc_Toc_enumdecl_to_c(_tmp1FD);
return t;_LLA1: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1FE=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1BB;if(_tmp1FE->tag != 26)goto _LL62;else{{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp1FF=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp1FE->f1)->r;if(_tmp1FF->tag != 2)goto _LL62;else{_tmp200=_tmp1FF->f1;}}_tmp201=_tmp1FE->f2;}}_LLA2:
# 984
 Cyc_Toc_datatypedecl_to_c(_tmp200);
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp201)));_LL62:;}
# 989
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned int l){
void*_tmp231=t;void*_tmp233;struct Cyc_Absyn_Tqual _tmp234;_LLBD: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp232=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp231;if(_tmp232->tag != 8)goto _LLBF;else{_tmp233=(_tmp232->f1).elt_type;_tmp234=(_tmp232->f1).tq;}}_LLBE:
# 992
 return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp233,(void*)& Cyc_Absyn_HeapRgn_val,_tmp234,Cyc_Absyn_false_conref),e);_LLBF:;_LLC0:
 return Cyc_Toc_cast_it(t,e);_LLBC:;}
# 999
static int Cyc_Toc_atomic_typ(void*t){
void*_tmp235=Cyc_Tcutil_compress(t);void*_tmp236=_tmp235;void*_tmp240;union Cyc_Absyn_AggrInfoU _tmp242;struct Cyc_List_List*_tmp244;struct Cyc_Absyn_Datatypedecl*_tmp246;struct Cyc_Absyn_Datatypefield*_tmp247;struct Cyc_List_List*_tmp249;_LLC2: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp237=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp236;if(_tmp237->tag != 0)goto _LLC4;}_LLC3:
 return 1;_LLC4: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp238=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp236;if(_tmp238->tag != 2)goto _LLC6;}_LLC5:
 return 0;_LLC6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp239=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp236;if(_tmp239->tag != 6)goto _LLC8;}_LLC7:
 goto _LLC9;_LLC8: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp23A=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp236;if(_tmp23A->tag != 13)goto _LLCA;}_LLC9:
 goto _LLCB;_LLCA: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp23B=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp236;if(_tmp23B->tag != 14)goto _LLCC;}_LLCB:
 goto _LLCD;_LLCC: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp23C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp236;if(_tmp23C->tag != 7)goto _LLCE;}_LLCD:
 goto _LLCF;_LLCE: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp23D=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp236;if(_tmp23D->tag != 9)goto _LLD0;}_LLCF:
 goto _LLD1;_LLD0: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp23E=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp236;if(_tmp23E->tag != 19)goto _LLD2;}_LLD1:
 return 1;_LLD2: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp23F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp236;if(_tmp23F->tag != 8)goto _LLD4;else{_tmp240=(_tmp23F->f1).elt_type;}}_LLD3:
 return Cyc_Toc_atomic_typ(_tmp240);_LLD4: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp241=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp236;if(_tmp241->tag != 11)goto _LLD6;else{_tmp242=(_tmp241->f1).aggr_info;}}_LLD5:
# 1016
{union Cyc_Absyn_AggrInfoU _tmp24E=_tmp242;_LLE7: if((_tmp24E.UnknownAggr).tag != 1)goto _LLE9;_LLE8:
 return 0;_LLE9:;_LLEA:
 goto _LLE6;_LLE6:;}{
# 1020
struct Cyc_Absyn_Aggrdecl*_tmp24F=Cyc_Absyn_get_known_aggrdecl(_tmp242);
if(_tmp24F->impl == 0)
return 0;
{struct Cyc_List_List*_tmp250=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp24F->impl))->fields;for(0;_tmp250 != 0;_tmp250=_tmp250->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp250->hd)->type))return 0;}}
return 1;};_LLD6: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp243=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp236;if(_tmp243->tag != 12)goto _LLD8;else{_tmp244=_tmp243->f2;}}_LLD7:
# 1027
 for(0;_tmp244 != 0;_tmp244=_tmp244->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp244->hd)->type))return 0;}
return 1;_LLD8: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp245=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp236;if(_tmp245->tag != 4)goto _LLDA;else{if((((_tmp245->f1).field_info).KnownDatatypefield).tag != 2)goto _LLDA;_tmp246=((struct _tuple2)(((_tmp245->f1).field_info).KnownDatatypefield).val).f1;_tmp247=((struct _tuple2)(((_tmp245->f1).field_info).KnownDatatypefield).val).f2;}}_LLD9:
# 1031
 _tmp249=_tmp247->typs;goto _LLDB;_LLDA: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp248=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp236;if(_tmp248->tag != 10)goto _LLDC;else{_tmp249=_tmp248->f1;}}_LLDB:
# 1033
 for(0;_tmp249 != 0;_tmp249=_tmp249->tl){
if(!Cyc_Toc_atomic_typ((*((struct _tuple11*)_tmp249->hd)).f2))return 0;}
return 1;_LLDC: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp24A=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp236;if(_tmp24A->tag != 3)goto _LLDE;}_LLDD:
# 1038
 goto _LLDF;_LLDE: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp24B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp236;if(_tmp24B->tag != 5)goto _LLE0;}_LLDF:
 goto _LLE1;_LLE0: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp24C=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp236;if(_tmp24C->tag != 16)goto _LLE2;}_LLE1:
 goto _LLE3;_LLE2: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp24D=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp236;if(_tmp24D->tag != 15)goto _LLE4;}_LLE3:
 return 0;_LLE4:;_LLE5: {
const char*_tmpB45;void*_tmpB44[1];struct Cyc_String_pa_PrintArg_struct _tmpB43;(_tmpB43.tag=0,((_tmpB43.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpB44[0]=& _tmpB43,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB45="atomic_typ:  bad type %s",_tag_dyneither(_tmpB45,sizeof(char),25))),_tag_dyneither(_tmpB44,sizeof(void*),1)))))));}_LLC1:;}
# 1046
static int Cyc_Toc_is_void_star(void*t){
void*_tmp254=Cyc_Tcutil_compress(t);void*_tmp255=_tmp254;void*_tmp257;_LLEC: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp256=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp255;if(_tmp256->tag != 5)goto _LLEE;else{_tmp257=(_tmp256->f1).elt_typ;}}_LLED: {
# 1049
void*_tmp258=Cyc_Tcutil_compress(_tmp257);void*_tmp259=_tmp258;_LLF1: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp25A=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp259;if(_tmp25A->tag != 0)goto _LLF3;}_LLF2:
 return 1;_LLF3:;_LLF4:
 return 0;_LLF0:;}_LLEE:;_LLEF:
# 1053
 return 0;_LLEB:;}
# 1057
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1061
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1066
static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){
void*_tmp25B=Cyc_Tcutil_compress(t);void*_tmp25C=_tmp25B;union Cyc_Absyn_AggrInfoU _tmp25E;struct Cyc_List_List*_tmp260;_LLF6: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp25D=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp25C;if(_tmp25D->tag != 11)goto _LLF8;else{_tmp25E=(_tmp25D->f1).aggr_info;}}_LLF7: {
# 1069
struct Cyc_Absyn_Aggrdecl*_tmp261=Cyc_Absyn_get_known_aggrdecl(_tmp25E);
if(_tmp261->impl == 0){
const char*_tmpB48;void*_tmpB47;(_tmpB47=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB48="is_poly_field: type missing fields",_tag_dyneither(_tmpB48,sizeof(char),35))),_tag_dyneither(_tmpB47,sizeof(void*),0)));}
_tmp260=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp261->impl))->fields;goto _LLF9;}_LLF8: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp25F=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp25C;if(_tmp25F->tag != 12)goto _LLFA;else{_tmp260=_tmp25F->f2;}}_LLF9: {
# 1074
struct Cyc_Absyn_Aggrfield*_tmp264=Cyc_Absyn_lookup_field(_tmp260,f);
if(_tmp264 == 0){
const char*_tmpB4C;void*_tmpB4B[1];struct Cyc_String_pa_PrintArg_struct _tmpB4A;(_tmpB4A.tag=0,((_tmpB4A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpB4B[0]=& _tmpB4A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB4C="is_poly_field: bad field %s",_tag_dyneither(_tmpB4C,sizeof(char),28))),_tag_dyneither(_tmpB4B,sizeof(void*),1)))))));}
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp264->type);}_LLFA:;_LLFB: {
const char*_tmpB50;void*_tmpB4F[1];struct Cyc_String_pa_PrintArg_struct _tmpB4E;(_tmpB4E.tag=0,((_tmpB4E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpB4F[0]=& _tmpB4E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB50="is_poly_field: bad type %s",_tag_dyneither(_tmpB50,sizeof(char),27))),_tag_dyneither(_tmpB4F,sizeof(void*),1)))))));}_LLF5:;}
# 1085
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp26B=e->r;void*_tmp26C=_tmp26B;struct Cyc_Absyn_Exp*_tmp26E;struct _dyneither_ptr*_tmp26F;struct Cyc_Absyn_Exp*_tmp271;struct _dyneither_ptr*_tmp272;_LLFD: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp26D=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp26D->tag != 20)goto _LLFF;else{_tmp26E=_tmp26D->f1;_tmp26F=_tmp26D->f2;}}_LLFE:
# 1088
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp26E->topt),_tmp26F) && !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));_LLFF: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp270=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp26C;if(_tmp270->tag != 21)goto _LL101;else{_tmp271=_tmp270->f1;_tmp272=_tmp270->f2;}}_LL100: {
# 1091
void*_tmp273=Cyc_Tcutil_compress((void*)_check_null(_tmp271->topt));void*_tmp274=_tmp273;void*_tmp276;_LL104: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp275=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp274;if(_tmp275->tag != 5)goto _LL106;else{_tmp276=(_tmp275->f1).elt_typ;}}_LL105:
# 1093
 return Cyc_Toc_is_poly_field(_tmp276,_tmp272) && !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));_LL106:;_LL107: {
const char*_tmpB54;void*_tmpB53[1];struct Cyc_String_pa_PrintArg_struct _tmpB52;(_tmpB52.tag=0,((_tmpB52.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp271->topt))),((_tmpB53[0]=& _tmpB52,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB54="is_poly_project: bad type %s",_tag_dyneither(_tmpB54,sizeof(char),29))),_tag_dyneither(_tmpB53,sizeof(void*),1)))))));}_LL103:;}_LL101:;_LL102:
# 1096
 return 0;_LLFC:;}
# 1101
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpB55;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((_tmpB55=_cycalloc(sizeof(*_tmpB55)),((_tmpB55->hd=s,((_tmpB55->tl=0,_tmpB55)))))),0);}
# 1105
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpB56;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((_tmpB56=_cycalloc(sizeof(*_tmpB56)),((_tmpB56->hd=s,((_tmpB56->tl=0,_tmpB56)))))),0);}
# 1109
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1115
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*_tmpB57[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmpB57[1]=s,((_tmpB57[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB57,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
# 1119
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*_tmpB58[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__fast_region_malloc_e,((_tmpB58[1]=s,((_tmpB58[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB58,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
# 1123
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type)){
struct Cyc_Absyn_Exp*_tmpB59[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpB59[1]=n,((_tmpB59[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB59,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{
# 1127
struct Cyc_Absyn_Exp*_tmpB5A[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpB5A[1]=n,((_tmpB5A[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB5A,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}
# 1130
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*_tmpB5B[3];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,((_tmpB5B[2]=n,((_tmpB5B[1]=s,((_tmpB5B[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB5B,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
# 1135
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
struct Cyc_List_List*_tmpB5C;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((_tmpB5C=_cycalloc(sizeof(*_tmpB5C)),((_tmpB5C->hd=e,((_tmpB5C->tl=0,_tmpB5C)))))),0);}
# 1138
static struct Cyc_Absyn_Exp*Cyc_Toc_newrethrow_exp(struct Cyc_Absyn_Exp*e){
struct Cyc_List_List*_tmpB5D;return Cyc_Absyn_fncall_exp(Cyc_Toc__rethrow_e,((_tmpB5D=_cycalloc(sizeof(*_tmpB5D)),((_tmpB5D->hd=e,((_tmpB5D->tl=0,_tmpB5D)))))),0);}
# 1142
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
struct Cyc_Absyn_Exp*_tmpB5E[0];return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB5E,sizeof(struct Cyc_Absyn_Exp*),0)),0),0);}
# 1148
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1156
int is_string=0;
{void*_tmp284=e->r;void*_tmp285=_tmp284;_LL109: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp286=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp285;if(_tmp286->tag != 0)goto _LL10B;else{if(((_tmp286->f1).String_c).tag != 8)goto _LL10B;}}_LL10A:
 is_string=1;goto _LL108;_LL10B: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp287=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp285;if(_tmp287->tag != 0)goto _LL10D;else{if(((_tmp287->f1).Wstring_c).tag != 9)goto _LL10D;}}_LL10C:
 is_string=1;goto _LL108;_LL10D:;_LL10E:
 goto _LL108;_LL108:;}{
# 1162
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
if(is_string){
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_conref,0);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB64;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB63;struct Cyc_List_List*_tmpB62;Cyc_Toc_result_decls=((_tmpB62=_cycalloc(sizeof(*_tmpB62)),((_tmpB62->hd=Cyc_Absyn_new_decl((void*)((_tmpB64=_cycalloc(sizeof(*_tmpB64)),((_tmpB64[0]=((_tmpB63.tag=0,((_tmpB63.f1=vd,_tmpB63)))),_tmpB64)))),0),((_tmpB62->tl=Cyc_Toc_result_decls,_tmpB62))))));}
xexp=Cyc_Absyn_var_exp(x,0);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{
# 1172
xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),e);
# 1174
xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
# 1176
struct Cyc_Absyn_Exp*urm_exp;
{struct _tuple19*_tmpB6B;struct _tuple19*_tmpB6A;struct _tuple19*_tmpB69;struct _tuple19*_tmpB68[3];urm_exp=Cyc_Absyn_unresolvedmem_exp(0,((_tmpB68[2]=(
# 1179
(_tmpB69=_cycalloc(sizeof(*_tmpB69)),((_tmpB69->f1=0,((_tmpB69->f2=xplussz,_tmpB69)))))),((_tmpB68[1]=(
# 1178
(_tmpB6A=_cycalloc(sizeof(*_tmpB6A)),((_tmpB6A->f1=0,((_tmpB6A->f2=xexp,_tmpB6A)))))),((_tmpB68[0]=(
# 1177
(_tmpB6B=_cycalloc(sizeof(*_tmpB6B)),((_tmpB6B->f1=0,((_tmpB6B->f2=xexp,_tmpB6B)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB68,sizeof(struct _tuple19*),3)))))))),0);}
# 1180
return urm_exp;};};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_StructInfo{struct Cyc_Absyn_Exp**varsizeexp;struct Cyc_Absyn_Exp*lhs_exp;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;int*in_lhs;struct Cyc_Toc_StructInfo struct_info;int*in_sizeof;struct _RegionHandle*rgn;};
# 1223 "toc.cyc"
typedef struct Cyc_Toc_Env*Cyc_Toc_env_t;
# 1225
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
int _tmp290;struct Cyc_Toc_Env*_tmp28F=nv;_tmp290=_tmp28F->toplevel;
return _tmp290;}
# 1229
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
int*_tmp292;struct Cyc_Toc_Env*_tmp291=nv;_tmp292=_tmp291->in_lhs;
return*_tmp292;}
# 1234
static struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple1*x){
struct Cyc_Dict_Dict _tmp294;struct Cyc_Toc_Env*_tmp293=nv;_tmp294=_tmp293->varmap;
return((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp294,x);}
# 1240
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
int*_tmpB73;struct Cyc_Absyn_Exp**_tmpB72;int*_tmpB71;struct Cyc_Toc_Env*_tmpB70;return(_tmpB70=_region_malloc(r,sizeof(*_tmpB70)),((_tmpB70->break_lab=(struct _dyneither_ptr**)0,((_tmpB70->continue_lab=(struct _dyneither_ptr**)0,((_tmpB70->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpB70->varmap=(struct Cyc_Dict_Dict)
# 1244
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((_tmpB70->toplevel=(int)1,((_tmpB70->in_lhs=(int*)(
# 1246
(_tmpB71=_region_malloc(r,sizeof(*_tmpB71)),((_tmpB71[0]=0,_tmpB71)))),((_tmpB70->struct_info=(struct Cyc_Toc_StructInfo)(
((_tmpB70->struct_info).varsizeexp=((_tmpB72=_cycalloc(sizeof(*_tmpB72)),((_tmpB72[0]=0,_tmpB72)))),(((_tmpB70->struct_info).lhs_exp=0,_tmpB70->struct_info)))),((_tmpB70->in_sizeof=(int*)(
(_tmpB73=_region_malloc(r,sizeof(*_tmpB73)),((_tmpB73[0]=0,_tmpB73)))),((_tmpB70->rgn=(struct _RegionHandle*)r,_tmpB70)))))))))))))))))));}
# 1252
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp29A;struct _dyneither_ptr**_tmp29B;struct Cyc_Toc_FallthruInfo*_tmp29C;struct Cyc_Dict_Dict _tmp29D;int _tmp29E;int*_tmp29F;struct Cyc_Toc_StructInfo _tmp2A0;int*_tmp2A1;struct Cyc_Toc_Env*_tmp299=e;_tmp29A=_tmp299->break_lab;_tmp29B=_tmp299->continue_lab;_tmp29C=_tmp299->fallthru_info;_tmp29D=_tmp299->varmap;_tmp29E=_tmp299->toplevel;_tmp29F=_tmp299->in_lhs;_tmp2A0=_tmp299->struct_info;_tmp2A1=_tmp299->in_sizeof;{
struct Cyc_Toc_Env*_tmpB74;return(_tmpB74=_region_malloc(r,sizeof(*_tmpB74)),((_tmpB74->break_lab=(struct _dyneither_ptr**)_tmp29A,((_tmpB74->continue_lab=(struct _dyneither_ptr**)_tmp29B,((_tmpB74->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp29C,((_tmpB74->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp29D),((_tmpB74->toplevel=(int)_tmp29E,((_tmpB74->in_lhs=(int*)_tmp29F,((_tmpB74->struct_info=(struct Cyc_Toc_StructInfo)_tmp2A0,((_tmpB74->in_sizeof=(int*)_tmp2A1,((_tmpB74->rgn=(struct _RegionHandle*)r,_tmpB74)))))))))))))))))));};}
# 1257
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp2A4;struct _dyneither_ptr**_tmp2A5;struct Cyc_Toc_FallthruInfo*_tmp2A6;struct Cyc_Dict_Dict _tmp2A7;int _tmp2A8;int*_tmp2A9;struct Cyc_Toc_StructInfo _tmp2AA;int*_tmp2AB;struct Cyc_Toc_Env*_tmp2A3=e;_tmp2A4=_tmp2A3->break_lab;_tmp2A5=_tmp2A3->continue_lab;_tmp2A6=_tmp2A3->fallthru_info;_tmp2A7=_tmp2A3->varmap;_tmp2A8=_tmp2A3->toplevel;_tmp2A9=_tmp2A3->in_lhs;_tmp2AA=_tmp2A3->struct_info;_tmp2AB=_tmp2A3->in_sizeof;{
struct Cyc_Toc_Env*_tmpB75;return(_tmpB75=_region_malloc(r,sizeof(*_tmpB75)),((_tmpB75->break_lab=(struct _dyneither_ptr**)_tmp2A4,((_tmpB75->continue_lab=(struct _dyneither_ptr**)_tmp2A5,((_tmpB75->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2A6,((_tmpB75->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2A7),((_tmpB75->toplevel=(int)0,((_tmpB75->in_lhs=(int*)_tmp2A9,((_tmpB75->struct_info=(struct Cyc_Toc_StructInfo)_tmp2AA,((_tmpB75->in_sizeof=(int*)_tmp2AB,((_tmpB75->rgn=(struct _RegionHandle*)r,_tmpB75)))))))))))))))))));};}
# 1261
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp2AE;struct _dyneither_ptr**_tmp2AF;struct Cyc_Toc_FallthruInfo*_tmp2B0;struct Cyc_Dict_Dict _tmp2B1;int _tmp2B2;int*_tmp2B3;struct Cyc_Toc_StructInfo _tmp2B4;int*_tmp2B5;struct Cyc_Toc_Env*_tmp2AD=e;_tmp2AE=_tmp2AD->break_lab;_tmp2AF=_tmp2AD->continue_lab;_tmp2B0=_tmp2AD->fallthru_info;_tmp2B1=_tmp2AD->varmap;_tmp2B2=_tmp2AD->toplevel;_tmp2B3=_tmp2AD->in_lhs;_tmp2B4=_tmp2AD->struct_info;_tmp2B5=_tmp2AD->in_sizeof;{
struct Cyc_Toc_Env*_tmpB76;return(_tmpB76=_region_malloc(r,sizeof(*_tmpB76)),((_tmpB76->break_lab=(struct _dyneither_ptr**)_tmp2AE,((_tmpB76->continue_lab=(struct _dyneither_ptr**)_tmp2AF,((_tmpB76->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2B0,((_tmpB76->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2B1),((_tmpB76->toplevel=(int)1,((_tmpB76->in_lhs=(int*)_tmp2B3,((_tmpB76->struct_info=(struct Cyc_Toc_StructInfo)_tmp2B4,((_tmpB76->in_sizeof=(int*)_tmp2B5,((_tmpB76->rgn=(struct _RegionHandle*)r,_tmpB76)))))))))))))))))));};}
# 1265
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
int*_tmp2B8;struct Cyc_Toc_Env*_tmp2B7=e;_tmp2B8=_tmp2B7->in_lhs;
*_tmp2B8=b;}
# 1269
static int Cyc_Toc_set_in_sizeof(struct Cyc_Toc_Env*e,int b){
int*_tmp2BA;struct Cyc_Toc_Env*_tmp2B9=e;_tmp2BA=_tmp2B9->in_sizeof;{
int _tmp2BB=*_tmp2BA;
*_tmp2BA=b;
return _tmp2BB;};}
# 1275
static int Cyc_Toc_in_sizeof(struct Cyc_Toc_Env*e){
int*_tmp2BD;struct Cyc_Toc_Env*_tmp2BC=e;_tmp2BD=_tmp2BC->in_sizeof;
return*_tmp2BD;}
# 1280
static struct Cyc_Toc_Env*Cyc_Toc_set_lhs_exp(struct _RegionHandle*r,struct Cyc_Absyn_Exp*exp,struct Cyc_Toc_Env*e){
# 1282
struct _dyneither_ptr**_tmp2BF;struct _dyneither_ptr**_tmp2C0;struct Cyc_Toc_FallthruInfo*_tmp2C1;struct Cyc_Dict_Dict _tmp2C2;int _tmp2C3;int*_tmp2C4;struct Cyc_Absyn_Exp**_tmp2C5;int*_tmp2C6;struct Cyc_Toc_Env*_tmp2BE=e;_tmp2BF=_tmp2BE->break_lab;_tmp2C0=_tmp2BE->continue_lab;_tmp2C1=_tmp2BE->fallthru_info;_tmp2C2=_tmp2BE->varmap;_tmp2C3=_tmp2BE->toplevel;_tmp2C4=_tmp2BE->in_lhs;_tmp2C5=(_tmp2BE->struct_info).varsizeexp;_tmp2C6=_tmp2BE->in_sizeof;{
struct Cyc_Toc_Env*_tmpB77;return(_tmpB77=_region_malloc(r,sizeof(*_tmpB77)),((_tmpB77->break_lab=(struct _dyneither_ptr**)_tmp2BF,((_tmpB77->continue_lab=(struct _dyneither_ptr**)_tmp2C0,((_tmpB77->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2C1,((_tmpB77->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2C2),((_tmpB77->toplevel=(int)_tmp2C3,((_tmpB77->in_lhs=(int*)_tmp2C4,((_tmpB77->struct_info=(struct Cyc_Toc_StructInfo)(((_tmpB77->struct_info).varsizeexp=_tmp2C5,(((_tmpB77->struct_info).lhs_exp=exp,_tmpB77->struct_info)))),((_tmpB77->in_sizeof=(int*)_tmp2C6,((_tmpB77->rgn=(struct _RegionHandle*)r,_tmpB77)))))))))))))))))));};}
# 1288
static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple1*x,struct Cyc_Absyn_Exp*y){
{union Cyc_Absyn_Nmspace _tmp2C8=(*x).f1;union Cyc_Absyn_Nmspace _tmp2C9=_tmp2C8;_LL110: if((_tmp2C9.Rel_n).tag != 1)goto _LL112;_LL111: {
# 1291
const char*_tmpB7B;void*_tmpB7A[1];struct Cyc_String_pa_PrintArg_struct _tmpB79;(_tmpB79.tag=0,((_tmpB79.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(x)),((_tmpB7A[0]=& _tmpB79,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB7B="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(_tmpB7B,sizeof(char),30))),_tag_dyneither(_tmpB7A,sizeof(void*),1)))))));}_LL112:;_LL113:
 goto _LL10F;_LL10F:;}{
# 1294
struct _dyneither_ptr**_tmp2CE;struct _dyneither_ptr**_tmp2CF;struct Cyc_Toc_FallthruInfo*_tmp2D0;struct Cyc_Dict_Dict _tmp2D1;int _tmp2D2;int*_tmp2D3;struct Cyc_Toc_StructInfo _tmp2D4;int*_tmp2D5;struct Cyc_Toc_Env*_tmp2CD=e;_tmp2CE=_tmp2CD->break_lab;_tmp2CF=_tmp2CD->continue_lab;_tmp2D0=_tmp2CD->fallthru_info;_tmp2D1=_tmp2CD->varmap;_tmp2D2=_tmp2CD->toplevel;_tmp2D3=_tmp2CD->in_lhs;_tmp2D4=_tmp2CD->struct_info;_tmp2D5=_tmp2CD->in_sizeof;{
struct Cyc_Dict_Dict _tmp2D6=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2D1),x,y);
struct Cyc_Toc_Env*_tmpB7C;return(_tmpB7C=_region_malloc(r,sizeof(*_tmpB7C)),((_tmpB7C->break_lab=(struct _dyneither_ptr**)_tmp2CE,((_tmpB7C->continue_lab=(struct _dyneither_ptr**)_tmp2CF,((_tmpB7C->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2D0,((_tmpB7C->varmap=(struct Cyc_Dict_Dict)_tmp2D6,((_tmpB7C->toplevel=(int)_tmp2D2,((_tmpB7C->in_lhs=(int*)_tmp2D3,((_tmpB7C->struct_info=(struct Cyc_Toc_StructInfo)_tmp2D4,((_tmpB7C->in_sizeof=(int*)_tmp2D5,((_tmpB7C->rgn=(struct _RegionHandle*)r,_tmpB7C)))))))))))))))))));};};}
# 1301
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp2D9;struct _dyneither_ptr**_tmp2DA;struct Cyc_Toc_FallthruInfo*_tmp2DB;struct Cyc_Dict_Dict _tmp2DC;int _tmp2DD;int*_tmp2DE;struct Cyc_Toc_StructInfo _tmp2DF;int*_tmp2E0;struct Cyc_Toc_Env*_tmp2D8=e;_tmp2D9=_tmp2D8->break_lab;_tmp2DA=_tmp2D8->continue_lab;_tmp2DB=_tmp2D8->fallthru_info;_tmp2DC=_tmp2D8->varmap;_tmp2DD=_tmp2D8->toplevel;_tmp2DE=_tmp2D8->in_lhs;_tmp2DF=_tmp2D8->struct_info;_tmp2E0=_tmp2D8->in_sizeof;{
struct Cyc_Toc_Env*_tmpB7D;return(_tmpB7D=_region_malloc(r,sizeof(*_tmpB7D)),((_tmpB7D->break_lab=(struct _dyneither_ptr**)0,((_tmpB7D->continue_lab=(struct _dyneither_ptr**)0,((_tmpB7D->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2DB,((_tmpB7D->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2DC),((_tmpB7D->toplevel=(int)_tmp2DD,((_tmpB7D->in_lhs=(int*)_tmp2DE,((_tmpB7D->struct_info=(struct Cyc_Toc_StructInfo)_tmp2DF,((_tmpB7D->in_sizeof=(int*)_tmp2E0,((_tmpB7D->rgn=(struct _RegionHandle*)r,_tmpB7D)))))))))))))))))));};}
# 1307
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*next_case_env){
# 1312
struct Cyc_List_List*fallthru_vars=0;
for(0;fallthru_binders != 0;fallthru_binders=fallthru_binders->tl){
struct Cyc_List_List*_tmpB7E;fallthru_vars=((_tmpB7E=_region_malloc(r,sizeof(*_tmpB7E)),((_tmpB7E->hd=((struct Cyc_Absyn_Vardecl*)fallthru_binders->hd)->name,((_tmpB7E->tl=fallthru_vars,_tmpB7E))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fallthru_vars);{
struct _dyneither_ptr**_tmp2E4;struct _dyneither_ptr**_tmp2E5;struct Cyc_Toc_FallthruInfo*_tmp2E6;struct Cyc_Dict_Dict _tmp2E7;int _tmp2E8;int*_tmp2E9;struct Cyc_Toc_StructInfo _tmp2EA;int*_tmp2EB;struct Cyc_Toc_Env*_tmp2E3=e;_tmp2E4=_tmp2E3->break_lab;_tmp2E5=_tmp2E3->continue_lab;_tmp2E6=_tmp2E3->fallthru_info;_tmp2E7=_tmp2E3->varmap;_tmp2E8=_tmp2E3->toplevel;_tmp2E9=_tmp2E3->in_lhs;_tmp2EA=_tmp2E3->struct_info;_tmp2EB=_tmp2E3->in_sizeof;{
struct Cyc_Dict_Dict _tmp2ED;struct Cyc_Toc_Env*_tmp2EC=next_case_env;_tmp2ED=_tmp2EC->varmap;{
struct Cyc_Toc_FallthruInfo*_tmpB7F;struct Cyc_Toc_FallthruInfo*fi=
(_tmpB7F=_region_malloc(r,sizeof(*_tmpB7F)),((_tmpB7F->label=fallthru_l,((_tmpB7F->binders=fallthru_vars,((_tmpB7F->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2ED),_tmpB7F)))))));
struct _dyneither_ptr**_tmpB82;struct Cyc_Toc_Env*_tmpB81;return(_tmpB81=_region_malloc(r,sizeof(*_tmpB81)),((_tmpB81->break_lab=(struct _dyneither_ptr**)((_tmpB82=_region_malloc(r,sizeof(*_tmpB82)),((_tmpB82[0]=break_l,_tmpB82)))),((_tmpB81->continue_lab=(struct _dyneither_ptr**)_tmp2E5,((_tmpB81->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi,((_tmpB81->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2E7),((_tmpB81->toplevel=(int)_tmp2E8,((_tmpB81->in_lhs=(int*)_tmp2E9,((_tmpB81->struct_info=(struct Cyc_Toc_StructInfo)_tmp2EA,((_tmpB81->in_sizeof=(int*)_tmp2EB,((_tmpB81->rgn=(struct _RegionHandle*)r,_tmpB81)))))))))))))))))));};};};}
# 1324
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1327
struct _dyneither_ptr**_tmp2F2;struct _dyneither_ptr**_tmp2F3;struct Cyc_Toc_FallthruInfo*_tmp2F4;struct Cyc_Dict_Dict _tmp2F5;int _tmp2F6;int*_tmp2F7;struct Cyc_Toc_StructInfo _tmp2F8;int*_tmp2F9;struct Cyc_Toc_Env*_tmp2F1=e;_tmp2F2=_tmp2F1->break_lab;_tmp2F3=_tmp2F1->continue_lab;_tmp2F4=_tmp2F1->fallthru_info;_tmp2F5=_tmp2F1->varmap;_tmp2F6=_tmp2F1->toplevel;_tmp2F7=_tmp2F1->in_lhs;_tmp2F8=_tmp2F1->struct_info;_tmp2F9=_tmp2F1->in_sizeof;{
struct _dyneither_ptr**_tmpB85;struct Cyc_Toc_Env*_tmpB84;return(_tmpB84=_region_malloc(r,sizeof(*_tmpB84)),((_tmpB84->break_lab=(struct _dyneither_ptr**)((_tmpB85=_region_malloc(r,sizeof(*_tmpB85)),((_tmpB85[0]=break_l,_tmpB85)))),((_tmpB84->continue_lab=(struct _dyneither_ptr**)_tmp2F3,((_tmpB84->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpB84->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2F5),((_tmpB84->toplevel=(int)_tmp2F6,((_tmpB84->in_lhs=(int*)_tmp2F7,((_tmpB84->struct_info=(struct Cyc_Toc_StructInfo)_tmp2F8,((_tmpB84->in_sizeof=(int*)_tmp2F9,((_tmpB84->rgn=(struct _RegionHandle*)r,_tmpB84)))))))))))))))))));};}
# 1334
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1337
struct _dyneither_ptr**_tmp2FD;struct _dyneither_ptr**_tmp2FE;struct Cyc_Toc_FallthruInfo*_tmp2FF;struct Cyc_Dict_Dict _tmp300;int _tmp301;int*_tmp302;struct Cyc_Toc_StructInfo _tmp303;int*_tmp304;struct Cyc_Toc_Env*_tmp2FC=e;_tmp2FD=_tmp2FC->break_lab;_tmp2FE=_tmp2FC->continue_lab;_tmp2FF=_tmp2FC->fallthru_info;_tmp300=_tmp2FC->varmap;_tmp301=_tmp2FC->toplevel;_tmp302=_tmp2FC->in_lhs;_tmp303=_tmp2FC->struct_info;_tmp304=_tmp2FC->in_sizeof;{
struct Cyc_Toc_FallthruInfo*_tmpB88;struct Cyc_Toc_Env*_tmpB87;return(_tmpB87=_region_malloc(r,sizeof(*_tmpB87)),((_tmpB87->break_lab=(struct _dyneither_ptr**)0,((_tmpB87->continue_lab=(struct _dyneither_ptr**)_tmp2FE,((_tmpB87->fallthru_info=(struct Cyc_Toc_FallthruInfo*)((_tmpB88=_region_malloc(r,sizeof(*_tmpB88)),((_tmpB88->label=next_l,((_tmpB88->binders=0,((_tmpB88->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),_tmpB88)))))))),((_tmpB87->varmap=(struct Cyc_Dict_Dict)
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp300),((_tmpB87->toplevel=(int)_tmp301,((_tmpB87->in_lhs=(int*)_tmp302,((_tmpB87->struct_info=(struct Cyc_Toc_StructInfo)_tmp303,((_tmpB87->in_sizeof=(int*)_tmp304,((_tmpB87->rgn=(struct _RegionHandle*)r,_tmpB87)))))))))))))))))));};}
# 1352 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1355
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
# 1357
void*_tmp307=e->annot;void*_tmp308=_tmp307;_LL115: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp309=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp308;if(_tmp309->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL117;}_LL116:
# 1360
 return Cyc_Toc_is_nullable((void*)_check_null(e->topt));_LL117: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp30A=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp308;if(_tmp30A->tag != Cyc_CfFlowInfo_NotZero)goto _LL119;}_LL118:
# 1363
 return 0;_LL119: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp30B=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp308;if(_tmp30B->tag != Cyc_CfFlowInfo_IsZero)goto _LL11B;}_LL11A:
# 1365
{const char*_tmpB8B;void*_tmpB8A;(_tmpB8A=0,Cyc_Tcutil_terr(e->loc,((_tmpB8B="dereference of NULL pointer",_tag_dyneither(_tmpB8B,sizeof(char),28))),_tag_dyneither(_tmpB8A,sizeof(void*),0)));}
return 0;_LL11B: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp30C=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp308;if(_tmp30C->tag != Cyc_Absyn_EmptyAnnot)goto _LL11D;}_LL11C:
# 1371
 return 1;_LL11D:;_LL11E: {
const char*_tmpB8E;void*_tmpB8D;(_tmpB8D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB8E="need_null_check",_tag_dyneither(_tmpB8E,sizeof(char),16))),_tag_dyneither(_tmpB8D,sizeof(void*),0)));}_LL114:;}
# 1376
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp311=e->annot;void*_tmp312=_tmp311;struct Cyc_List_List*_tmp314;struct Cyc_List_List*_tmp316;_LL120: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp313=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp312;if(_tmp313->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL122;else{_tmp314=_tmp313->f1;}}_LL121:
 return _tmp314;_LL122: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp315=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp312;if(_tmp315->tag != Cyc_CfFlowInfo_NotZero)goto _LL124;else{_tmp316=_tmp315->f1;}}_LL123:
 return _tmp316;_LL124: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp317=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp312;if(_tmp317->tag != Cyc_CfFlowInfo_IsZero)goto _LL126;}_LL125:
# 1381
{const char*_tmpB91;void*_tmpB90;(_tmpB90=0,Cyc_Tcutil_terr(e->loc,((_tmpB91="dereference of NULL pointer",_tag_dyneither(_tmpB91,sizeof(char),28))),_tag_dyneither(_tmpB90,sizeof(void*),0)));}
return 0;_LL126: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp318=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp312;if(_tmp318->tag != Cyc_Absyn_EmptyAnnot)goto _LL128;}_LL127:
 return 0;_LL128:;_LL129: {
const char*_tmpB94;void*_tmpB93;(_tmpB93=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB94="get_relns",_tag_dyneither(_tmpB94,sizeof(char),10))),_tag_dyneither(_tmpB93,sizeof(void*),0)));}_LL11F:;}static char _tmp322[8]="*bogus*";
# 1391
static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
# 1401 "toc.cyc"
union Cyc_Relations_RelnOp rop_i=Cyc_Relations_RConst(0);
int valid_rop_i=Cyc_Relations_exp2relnop(i,& rop_i);
# 1404
struct Cyc_Absyn_Vardecl*x;
{void*_tmp31D=a->r;void*_tmp31E=_tmp31D;void*_tmp320;_LL12B: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp31F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp31E;if(_tmp31F->tag != 1)goto _LL12D;else{_tmp320=(void*)_tmp31F->f2;}}_LL12C: {
# 1407
struct Cyc_Absyn_Vardecl*_tmp321=Cyc_Tcutil_nonesc_vardecl(_tmp320);
if(_tmp321 == 0)goto _LL12E;
x=_tmp321;
goto _LL12A;}_LL12D:;_LL12E: {
# 1413
static struct _dyneither_ptr bogus_string={_tmp322,_tmp322,_tmp322 + 8};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
# 1416
static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};
# 1422
x=& bogus_vardecl;
x->type=a_typ;}_LL12A:;}{
# 1425
void*_tmp323=a_typ;
union Cyc_Relations_RelnOp rop_a=Cyc_Relations_RNumelts(x);
# 1430
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound(_tmp323);
if(bound != 0){
union Cyc_Relations_RelnOp rop_bound=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(bound,& rop_bound))
relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_bound,Cyc_Relations_Rlte,rop_a,relns);}
# 1439
if(valid_rop_i){
struct Cyc_List_List*_tmp324=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlte,rop_i,relns);
# 1447
if(!Cyc_Relations_consistent_relations(_tmp324))return 1;}
# 1450
inner_loop: {
void*_tmp325=i->r;void*_tmp326=_tmp325;void*_tmp328;struct Cyc_Absyn_Exp*_tmp329;struct Cyc_Absyn_Exp*_tmp32B;struct Cyc_Absyn_Exp*_tmp32C;_LL130: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp327=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp326;if(_tmp327->tag != 13)goto _LL132;else{_tmp328=(void*)_tmp327->f1;_tmp329=_tmp327->f2;}}_LL131:
 i=_tmp329;goto inner_loop;_LL132: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp32A=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp326;if(_tmp32A->tag != 2)goto _LL134;else{if(_tmp32A->f1 != Cyc_Absyn_Mod)goto _LL134;if(_tmp32A->f2 == 0)goto _LL134;_tmp32B=(struct Cyc_Absyn_Exp*)(_tmp32A->f2)->hd;if((_tmp32A->f2)->tl == 0)goto _LL134;_tmp32C=(struct Cyc_Absyn_Exp*)((_tmp32A->f2)->tl)->hd;}}_LL133: {
# 1456
union Cyc_Relations_RelnOp rop_z=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmp32C,& rop_z)){
# 1459
struct Cyc_List_List*_tmp32D=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlt,rop_z,relns);
# 1461
return !Cyc_Relations_consistent_relations(_tmp32D);}
# 1463
goto _LL12F;}_LL134:;_LL135:
 goto _LL12F;_LL12F:;}
# 1466
return 0;};}
# 1469
static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0){const char*_tmpB97;void*_tmpB96;(_tmpB96=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB97="Missing type in primop ",_tag_dyneither(_tmpB97,sizeof(char),24))),_tag_dyneither(_tmpB96,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));}
# 1473
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0){const char*_tmpB9A;void*_tmpB99;(_tmpB99=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB9A="Missing type in primop ",_tag_dyneither(_tmpB9A,sizeof(char),24))),_tag_dyneither(_tmpB99,sizeof(void*),0)));}
return(void*)_check_null(e->topt);}
# 1477
static struct _tuple11*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
struct _tuple11*_tmpB9B;return(_tmpB9B=_cycalloc(sizeof(*_tmpB9B)),((_tmpB9B->f1=Cyc_Toc_mt_tq,((_tmpB9B->f2=Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmpB9B)))));}
# 1480
static struct _tuple19*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple19*_tmpB9C;return(_tmpB9C=_cycalloc(sizeof(*_tmpB9C)),((_tmpB9C->f1=0,((_tmpB9C->f2=e,_tmpB9C)))));};}
# 1487
static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic,int do_declare){
# 1490
struct Cyc_Absyn_Exp*eo;
void*t;
if(pointer){
struct Cyc_Absyn_Exp*_tmp334=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(x,0),0);
if(*(nv->struct_info).varsizeexp != 0){
_tmp334=Cyc_Absyn_add_exp(_tmp334,(struct Cyc_Absyn_Exp*)_check_null(*(nv->struct_info).varsizeexp),0);
*(nv->struct_info).varsizeexp=0;}
# 1498
t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);
if(rgnopt == 0  || Cyc_Absyn_no_regions)
eo=is_atomic?Cyc_Toc_malloc_atomic(_tmp334): Cyc_Toc_malloc_ptr(_tmp334);else{
# 1502
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);
eo=Cyc_Toc_rmalloc_exp(r,_tmp334);}}else{
# 1507
t=struct_typ;
eo=0;}
# 1510
if(do_declare){
void*_tmp335=x->r;void*_tmp336=_tmp335;struct _tuple1*_tmp338;_LL137: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp337=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp336;if(_tmp337->tag != 1)goto _LL139;else{_tmp338=_tmp337->f1;}}_LL138:
# 1513
 return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp338,t,eo,s,0),0);_LL139:;_LL13A: {
# 1515
const char*_tmpB9F;void*_tmpB9E;(_tmpB9E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB9F="bogus x to make_struct",_tag_dyneither(_tmpB9F,sizeof(char),23))),_tag_dyneither(_tmpB9E,sizeof(void*),0)));}_LL136:;}else{
# 1519
if(eo != 0)
return Cyc_Absyn_stmt_exp(Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(x,eo,0),0),s,0),0);else{
# 1522
return Cyc_Absyn_stmt_exp(s,0);}}}
# 1526
static int Cyc_Toc_abstract_aggr(void*typ){
void*_tmp33B=Cyc_Tcutil_compress(typ);void*_tmp33C=_tmp33B;union Cyc_Absyn_AggrInfoU _tmp33E;struct Cyc_List_List*_tmp33F;struct Cyc_List_List*_tmp341;_LL13C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp33D=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp33C;if(_tmp33D->tag != 11)goto _LL13E;else{_tmp33E=(_tmp33D->f1).aggr_info;_tmp33F=(_tmp33D->f1).targs;}}_LL13D: {
# 1529
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(_tmp33E);
if(ad->impl == 0)return 0;{
struct Cyc_List_List*aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
_tmp341=aggrfields;goto _LL13F;};}_LL13E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp340=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp33C;if(_tmp340->tag != 12)goto _LL140;else{_tmp341=_tmp340->f2;}}_LL13F:
# 1534
 if(_tmp341 == 0)return 0;
return 1;_LL140:;_LL141:
# 1540
 return 0;_LL13B:;}
# 1544
static void*Cyc_Toc_array_elt_type(void*t){
void*_tmp342=Cyc_Tcutil_compress(t);void*_tmp343=_tmp342;void*_tmp345;_LL143: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp344=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp343;if(_tmp344->tag != 8)goto _LL145;else{_tmp345=(_tmp344->f1).elt_type;}}_LL144:
 return _tmp345;_LL145:;_LL146: {
const char*_tmpBA3;void*_tmpBA2[1];struct Cyc_String_pa_PrintArg_struct _tmpBA1;(_tmpBA1.tag=0,((_tmpBA1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpBA2[0]=& _tmpBA1,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA3="array_elt_type called on %s",_tag_dyneither(_tmpBA3,sizeof(char),28))),_tag_dyneither(_tmpBA2,sizeof(void*),1)))))));}_LL142:;}
# 1551
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated);
# 1557
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s);
# 1562
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,void*array_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){
# 1567
void*orig_elt_type=Cyc_Toc_array_elt_type(array_type);
int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;
{struct Cyc_List_List*_tmp349=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;_tmp349 != 0;_tmp349=_tmp349->tl){
# 1571
struct _tuple19*_tmp34A=(struct _tuple19*)_tmp349->hd;struct Cyc_List_List*_tmp34C;struct Cyc_Absyn_Exp*_tmp34D;struct _tuple19*_tmp34B=_tmp34A;_tmp34C=_tmp34B->f1;_tmp34D=_tmp34B->f2;{
# 1575
struct Cyc_Absyn_Exp*e_index;
if(_tmp34C == 0)
e_index=Cyc_Absyn_signed_int_exp(count --,0);else{
# 1579
if(_tmp34C->tl != 0){const char*_tmpBA6;void*_tmpBA5;(_tmpBA5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBA6="multiple designators in array",_tag_dyneither(_tmpBA6,sizeof(char),30))),_tag_dyneither(_tmpBA5,sizeof(void*),0)));}{
void*_tmp350=(void*)_tmp34C->hd;
void*_tmp351=_tmp350;struct Cyc_Absyn_Exp*_tmp353;_LL148: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp352=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp351;if(_tmp352->tag != 0)goto _LL14A;else{_tmp353=_tmp352->f1;}}_LL149:
# 1583
 Cyc_Toc_exp_to_c(nv,_tmp353);
e_index=_tmp353;
goto _LL147;_LL14A: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp354=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp351;if(_tmp354->tag != 1)goto _LL147;}_LL14B: {
const char*_tmpBA9;void*_tmpBA8;(_tmpBA8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBA9="field name designators in array",_tag_dyneither(_tmpBA9,sizeof(char),32))),_tag_dyneither(_tmpBA8,sizeof(void*),0)));}_LL147:;};}{
# 1589
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);
void*_tmp357=_tmp34D->r;void*_tmp358=_tmp357;struct Cyc_List_List*_tmp35A;struct Cyc_Absyn_Vardecl*_tmp35C;struct Cyc_Absyn_Exp*_tmp35D;struct Cyc_Absyn_Exp*_tmp35E;int _tmp35F;void*_tmp361;struct Cyc_List_List*_tmp362;_LL14D: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp359=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp358;if(_tmp359->tag != 25)goto _LL14F;else{_tmp35A=_tmp359->f1;}}_LL14E:
# 1592
 s=Cyc_Toc_init_array(nv,orig_elt_type,lval,_tmp35A,s);
goto _LL14C;_LL14F: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp35B=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp358;if(_tmp35B->tag != 26)goto _LL151;else{_tmp35C=_tmp35B->f1;_tmp35D=_tmp35B->f2;_tmp35E=_tmp35B->f3;_tmp35F=_tmp35B->f4;}}_LL150:
# 1595
 s=Cyc_Toc_init_comprehension(nv,orig_elt_type,lval,_tmp35C,_tmp35D,_tmp35E,_tmp35F,s,0);
# 1597
goto _LL14C;_LL151: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp360=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp358;if(_tmp360->tag != 29)goto _LL153;else{_tmp361=(void*)_tmp360->f1;_tmp362=_tmp360->f2;}}_LL152:
# 1599
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp361,_tmp362,s);
goto _LL14C;_LL153: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp363=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp358;if(_tmp363->tag != 27)goto _LL155;}_LL154:
# 1602
 goto _LL14C;_LL155:;_LL156:
# 1604
 Cyc_Toc_exp_to_c(nv,_tmp34D);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(orig_elt_type),_tmp34D),0),s,0);
goto _LL14C;_LL14C:;};};}}
# 1610
return s;}
# 1615
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated){
# 1620
struct _tuple1*_tmp364=vd->name;
void*expected_elt_type;
{void*_tmp365=Cyc_Tcutil_compress(comprehension_type);void*_tmp366=_tmp365;void*_tmp368;void*_tmp36A;_LL158: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp367=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp366;if(_tmp367->tag != 8)goto _LL15A;else{_tmp368=(_tmp367->f1).elt_type;}}_LL159:
# 1624
 expected_elt_type=_tmp368;goto _LL157;_LL15A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp369=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp366;if(_tmp369->tag != 5)goto _LL15C;else{_tmp36A=(_tmp369->f1).elt_typ;}}_LL15B:
# 1626
 expected_elt_type=_tmp36A;goto _LL157;_LL15C:;_LL15D: {
const char*_tmpBAC;void*_tmpBAB;(_tmpBAB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBAC="init_comprehension passed a bad type!",_tag_dyneither(_tmpBAC,sizeof(char),38))),_tag_dyneither(_tmpBAB,sizeof(void*),0)));}_LL157:;}{
# 1629
void*_tmp36D=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));
if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);
# 1633
{void*_tmp36E=e2->r;void*_tmp36F=_tmp36E;_LL15F: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp370=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp36F;if(_tmp370->tag != 27)goto _LL161;}_LL160:
 return Cyc_Absyn_skip_stmt(0);_LL161:;_LL162:
 goto _LL15E;_LL15E:;}{
# 1637
struct _RegionHandle _tmp371=_new_region("r2");struct _RegionHandle*r2=& _tmp371;_push_region(r2);
{struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpBAF;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpBAE;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(r2,nv,_tmp364,Cyc_Absyn_varb_exp(_tmp364,(void*)((_tmpBAE=_cycalloc(sizeof(*_tmpBAE)),((_tmpBAE[0]=((_tmpBAF.tag=4,((_tmpBAF.f1=vd,_tmpBAF)))),_tmpBAE)))),0));
struct _tuple1*max=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp364,0),Cyc_Absyn_signed_int_exp(0,0),0);
struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp364,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp364,0),0);
# 1645
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp364,0),0);
struct Cyc_Absyn_Stmt*body;
# 1649
{void*_tmp372=e2->r;void*_tmp373=_tmp372;struct Cyc_List_List*_tmp375;struct Cyc_Absyn_Vardecl*_tmp377;struct Cyc_Absyn_Exp*_tmp378;struct Cyc_Absyn_Exp*_tmp379;int _tmp37A;void*_tmp37C;struct Cyc_List_List*_tmp37D;_LL164: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp374=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp373;if(_tmp374->tag != 25)goto _LL166;else{_tmp375=_tmp374->f1;}}_LL165:
# 1651
 body=Cyc_Toc_init_array(nv2,expected_elt_type,lval,_tmp375,Cyc_Toc_skip_stmt_dl());
goto _LL163;_LL166: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp376=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp373;if(_tmp376->tag != 26)goto _LL168;else{_tmp377=_tmp376->f1;_tmp378=_tmp376->f2;_tmp379=_tmp376->f3;_tmp37A=_tmp376->f4;}}_LL167:
# 1654
 body=Cyc_Toc_init_comprehension(nv2,expected_elt_type,lval,_tmp377,_tmp378,_tmp379,_tmp37A,Cyc_Toc_skip_stmt_dl(),0);
goto _LL163;_LL168: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp37B=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp373;if(_tmp37B->tag != 29)goto _LL16A;else{_tmp37C=(void*)_tmp37B->f1;_tmp37D=_tmp37B->f2;}}_LL169:
# 1657
 body=Cyc_Toc_init_anon_struct(nv,lval,_tmp37C,_tmp37D,Cyc_Toc_skip_stmt_dl());
goto _LL163;_LL16A:;_LL16B:
# 1660
 Cyc_Toc_exp_to_c(nv2,e2);
body=Cyc_Absyn_assign_stmt(lval,Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(expected_elt_type),e2),0);
goto _LL163;_LL163:;}{
# 1664
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0);
# 1666
if(zero_term){
# 1671
struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(_tmp36D,Cyc_Absyn_uint_exp(0,0)),0);
s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}{
# 1676
struct Cyc_Absyn_Stmt*_tmp37E=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,e1,
Cyc_Absyn_declare_stmt(_tmp364,Cyc_Absyn_uint_typ,0,s2,0),0),s,0);_npop_handler(0);return _tmp37E;};};}
# 1638
;_pop_region(r2);};};}
# 1682
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s){
# 1685
{struct Cyc_List_List*_tmp381=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles);for(0;_tmp381 != 0;_tmp381=_tmp381->tl){
struct _tuple19*_tmp382=(struct _tuple19*)_tmp381->hd;struct Cyc_List_List*_tmp384;struct Cyc_Absyn_Exp*_tmp385;struct _tuple19*_tmp383=_tmp382;_tmp384=_tmp383->f1;_tmp385=_tmp383->f2;
if(_tmp384 == 0){
const char*_tmpBB2;void*_tmpBB1;(_tmpBB1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBB2="empty designator list",_tag_dyneither(_tmpBB2,sizeof(char),22))),_tag_dyneither(_tmpBB1,sizeof(void*),0)));}
if(_tmp384->tl != 0){
const char*_tmpBB5;void*_tmpBB4;(_tmpBB4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBB5="too many designators in anonymous struct",_tag_dyneither(_tmpBB5,sizeof(char),41))),_tag_dyneither(_tmpBB4,sizeof(void*),0)));}{
void*_tmp38A=(void*)_tmp384->hd;void*_tmp38B=_tmp38A;struct _dyneither_ptr*_tmp38D;_LL16D: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp38C=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp38B;if(_tmp38C->tag != 1)goto _LL16F;else{_tmp38D=_tmp38C->f1;}}_LL16E: {
# 1693
struct Cyc_Absyn_Exp*lval=Cyc_Toc_member_exp(lhs,_tmp38D,0);
{void*_tmp38E=_tmp385->r;void*_tmp38F=_tmp38E;struct Cyc_List_List*_tmp391;struct Cyc_Absyn_Vardecl*_tmp393;struct Cyc_Absyn_Exp*_tmp394;struct Cyc_Absyn_Exp*_tmp395;int _tmp396;void*_tmp398;struct Cyc_List_List*_tmp399;_LL172: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp390=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp38F;if(_tmp390->tag != 25)goto _LL174;else{_tmp391=_tmp390->f1;}}_LL173:
# 1696
 s=Cyc_Toc_init_array(nv,(void*)_check_null(_tmp385->topt),lval,_tmp391,s);goto _LL171;_LL174: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp392=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp38F;if(_tmp392->tag != 26)goto _LL176;else{_tmp393=_tmp392->f1;_tmp394=_tmp392->f2;_tmp395=_tmp392->f3;_tmp396=_tmp392->f4;}}_LL175:
# 1698
 s=Cyc_Toc_init_comprehension(nv,(void*)_check_null(_tmp385->topt),lval,_tmp393,_tmp394,_tmp395,_tmp396,s,0);
goto _LL171;_LL176: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp397=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp38F;if(_tmp397->tag != 29)goto _LL178;else{_tmp398=(void*)_tmp397->f1;_tmp399=_tmp397->f2;}}_LL177:
# 1701
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp398,_tmp399,s);goto _LL171;_LL178: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp39A=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp38F;if(_tmp39A->tag != 27)goto _LL17A;}_LL179:
# 1703
 goto _LL171;_LL17A:;_LL17B:
# 1705
 Cyc_Toc_exp_to_c(nv,_tmp385);
# 1707
if(Cyc_Toc_is_poly_field(struct_type,_tmp38D))
_tmp385=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp385);
# 1710
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp385,0),0),s,0);
goto _LL171;_LL171:;}
# 1713
goto _LL16C;}_LL16F:;_LL170: {
const char*_tmpBB8;void*_tmpBB7;(_tmpBB7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBB8="array designator in struct",_tag_dyneither(_tmpBB8,sizeof(char),27))),_tag_dyneither(_tmpBB7,sizeof(void*),0)));}_LL16C:;};}}
# 1717
return s;}
# 1722
static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){
# 1726
struct _RegionHandle _tmp39D=_new_region("r");struct _RegionHandle*r=& _tmp39D;_push_region(r);
{struct Cyc_List_List*_tmp39E=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);
void*_tmp39F=Cyc_Toc_add_tuple_type(_tmp39E);
# 1730
struct _tuple1*_tmp3A0=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3A1=Cyc_Absyn_var_exp(_tmp3A0,0);
struct Cyc_Absyn_Stmt*_tmp3A2=Cyc_Absyn_exp_stmt(_tmp3A1,0);
# 1734
struct Cyc_Absyn_Exp*(*_tmp3A3)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
# 1736
int is_atomic=1;
struct Cyc_List_List*_tmp3A4=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);
{int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp3A4);for(0;_tmp3A4 != 0;(_tmp3A4=_tmp3A4->tl,-- i)){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp3A4->hd;
struct Cyc_Absyn_Exp*lval=_tmp3A3(_tmp3A1,Cyc_Absyn_fieldname(i),0);
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(e->topt));{
void*_tmp3A5=e->r;void*_tmp3A6=_tmp3A5;struct Cyc_List_List*_tmp3A8;struct Cyc_Absyn_Vardecl*_tmp3AA;struct Cyc_Absyn_Exp*_tmp3AB;struct Cyc_Absyn_Exp*_tmp3AC;int _tmp3AD;_LL17D: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp3A7=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3A6;if(_tmp3A7->tag != 25)goto _LL17F;else{_tmp3A8=_tmp3A7->f1;}}_LL17E:
# 1744
 _tmp3A2=Cyc_Toc_init_array(nv,(void*)_check_null(e->topt),lval,_tmp3A8,_tmp3A2);
goto _LL17C;_LL17F: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp3A9=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3A6;if(_tmp3A9->tag != 26)goto _LL181;else{_tmp3AA=_tmp3A9->f1;_tmp3AB=_tmp3A9->f2;_tmp3AC=_tmp3A9->f3;_tmp3AD=_tmp3A9->f4;}}_LL180:
# 1747
 _tmp3A2=Cyc_Toc_init_comprehension(nv,(void*)_check_null(e->topt),lval,_tmp3AA,_tmp3AB,_tmp3AC,_tmp3AD,_tmp3A2,0);
# 1749
goto _LL17C;_LL181: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp3AE=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp3A6;if(_tmp3AE->tag != 27)goto _LL183;}_LL182:
# 1751
 goto _LL17C;_LL183:;_LL184:
# 1754
 Cyc_Toc_exp_to_c(nv,e);
_tmp3A2=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3A3(_tmp3A1,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp3A2,0);
# 1757
goto _LL17C;_LL17C:;};}}{
# 1760
struct Cyc_Absyn_Exp*_tmp3AF=Cyc_Toc_make_struct(nv,Cyc_Absyn_var_exp(_tmp3A0,0),_tmp39F,_tmp3A2,pointer,rgnopt,is_atomic,1);_npop_handler(0);return _tmp3AF;};}
# 1727
;_pop_region(r);}
# 1764
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp3B0=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp3B0 != 0;_tmp3B0=_tmp3B0->tl){
struct Cyc_Absyn_Aggrfield*_tmp3B1=(struct Cyc_Absyn_Aggrfield*)_tmp3B0->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp3B1->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}{
# 1771
struct Cyc_String_pa_PrintArg_struct _tmpBC0;void*_tmpBBF[1];const char*_tmpBBE;void*_tmpBBD;(_tmpBBD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((struct _dyneither_ptr)((_tmpBC0.tag=0,((_tmpBC0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpBBF[0]=& _tmpBC0,Cyc_aprintf(((_tmpBBE="get_member_offset %s failed",_tag_dyneither(_tmpBBE,sizeof(char),28))),_tag_dyneither(_tmpBBF,sizeof(void*),1)))))))),_tag_dyneither(_tmpBBD,sizeof(void*),0)));};}
# 1774
static int Cyc_Toc_init_variable_array(void**typ){
{void*_tmp3B6=Cyc_Tcutil_compress(*typ);void*_tmp3B7=_tmp3B6;void*_tmp3B9;struct Cyc_Absyn_Tqual _tmp3BA;struct Cyc_Absyn_Exp*_tmp3BB;union Cyc_Absyn_Constraint*_tmp3BC;unsigned int _tmp3BD;_LL186: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp3B8=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B7;if(_tmp3B8->tag != 8)goto _LL188;else{_tmp3B9=(_tmp3B8->f1).elt_type;_tmp3BA=(_tmp3B8->f1).tq;_tmp3BB=(_tmp3B8->f1).num_elts;_tmp3BC=(_tmp3B8->f1).zero_term;_tmp3BD=(_tmp3B8->f1).zt_loc;}}_LL187:
# 1779
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp3BB))){
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpBC6;struct Cyc_Absyn_ArrayInfo _tmpBC5;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpBC4;*typ=(void*)((_tmpBC4=_cycalloc(sizeof(*_tmpBC4)),((_tmpBC4[0]=((_tmpBC6.tag=8,((_tmpBC6.f1=((_tmpBC5.elt_type=_tmp3B9,((_tmpBC5.tq=_tmp3BA,((_tmpBC5.num_elts=Cyc_Absyn_uint_exp(0,0),((_tmpBC5.zero_term=_tmp3BC,((_tmpBC5.zt_loc=_tmp3BD,_tmpBC5)))))))))),_tmpBC6)))),_tmpBC4))));}
return 1;}
# 1783
goto _LL185;_LL188:;_LL189:
 goto _LL185;_LL185:;}
# 1786
return 0;}
# 1789
static int Cyc_Toc_is_array_type(void*t){
void*_tmp3C1=Cyc_Tcutil_compress(t);void*_tmp3C2=_tmp3C1;_LL18B: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp3C3=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3C2;if(_tmp3C3->tag != 8)goto _LL18D;}_LL18C:
 return 1;_LL18D:;_LL18E:
 return 0;_LL18A:;}
# 1796
struct Cyc_Absyn_Aggrdecl*Cyc_Toc_update_aggr_type(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*fn,void*new_field_type){
# 1798
struct Cyc_List_List*new_fields=0;
struct Cyc_List_List*fields;
for(fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;fields != 0;fields=fields->tl){
# 1802
if(!Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,fn)){
struct Cyc_Absyn_Aggrfield*_tmp3C4=(struct Cyc_Absyn_Aggrfield*)fields->hd;
struct Cyc_List_List*_tmp3C5=_tmp3C4->attributes;
void*_tmp3C6=_tmp3C4->type;
if((!Cyc_Toc_is_array_type(_tmp3C6),Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp3C6)))){
struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpBCC;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpBCB;struct Cyc_List_List*_tmpBCA;_tmp3C5=((_tmpBCA=_cycalloc(sizeof(*_tmpBCA)),((_tmpBCA->hd=(void*)((_tmpBCC=_cycalloc(sizeof(*_tmpBCC)),((_tmpBCC[0]=((_tmpBCB.tag=6,((_tmpBCB.f1=0,_tmpBCB)))),_tmpBCC)))),((_tmpBCA->tl=_tmp3C5,_tmpBCA))))));}{
struct Cyc_Absyn_Aggrfield*_tmpBCD;struct Cyc_Absyn_Aggrfield*_tmp3CA=(_tmpBCD=_cycalloc(sizeof(*_tmpBCD)),((_tmpBCD->name=_tmp3C4->name,((_tmpBCD->tq=Cyc_Toc_mt_tq,((_tmpBCD->type=new_field_type,((_tmpBCD->width=_tmp3C4->width,((_tmpBCD->attributes=_tmp3C5,((_tmpBCD->requires_clause=0,_tmpBCD)))))))))))));
# 1814
struct Cyc_List_List*_tmpBCE;new_fields=((_tmpBCE=_cycalloc(sizeof(*_tmpBCE)),((_tmpBCE->hd=_tmp3CA,((_tmpBCE->tl=new_fields,_tmpBCE))))));};}else{
# 1816
struct Cyc_List_List*_tmpBCF;new_fields=((_tmpBCF=_cycalloc(sizeof(*_tmpBCF)),((_tmpBCF->hd=(struct Cyc_Absyn_Aggrfield*)fields->hd,((_tmpBCF->tl=new_fields,_tmpBCF))))));}}{
# 1818
struct Cyc_List_List*_tmp3CE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);
struct Cyc_Int_pa_PrintArg_struct _tmpBD7;void*_tmpBD6[1];const char*_tmpBD5;struct _dyneither_ptr*_tmpBD4;struct _dyneither_ptr*name=
(_tmpBD4=_cycalloc(sizeof(*_tmpBD4)),((_tmpBD4[0]=(struct _dyneither_ptr)((_tmpBD7.tag=1,((_tmpBD7.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpBD6[0]=& _tmpBD7,Cyc_aprintf(((_tmpBD5="_genStruct%d",_tag_dyneither(_tmpBD5,sizeof(char),13))),_tag_dyneither(_tmpBD6,sizeof(void*),1)))))))),_tmpBD4)));
struct _tuple1*_tmpBD8;struct _tuple1*qv=(_tmpBD8=_cycalloc(sizeof(*_tmpBD8)),((_tmpBD8->f1=Cyc_Absyn_Abs_n(0,1),((_tmpBD8->f2=name,_tmpBD8)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpBD9;struct Cyc_Absyn_AggrdeclImpl*_tmp3CF=(_tmpBD9=_cycalloc(sizeof(*_tmpBD9)),((_tmpBD9->exist_vars=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,((_tmpBD9->rgn_po=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po,((_tmpBD9->fields=_tmp3CE,((_tmpBD9->tagged=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged,_tmpBD9)))))))));
# 1826
struct Cyc_Absyn_Aggrdecl*_tmpBDA;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpBDA=_cycalloc(sizeof(*_tmpBDA)),((_tmpBDA->kind=ad->kind,((_tmpBDA->sc=ad->sc,((_tmpBDA->name=qv,((_tmpBDA->tvs=ad->tvs,((_tmpBDA->impl=_tmp3CF,((_tmpBDA->expected_mem_kind=0,((_tmpBDA->attributes=ad->attributes,_tmpBDA)))))))))))))));
# 1833
Cyc_Toc_aggrdecl_to_c(new_ad,1);
return new_ad;};}
# 1837
static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*nv,void*struct_type,struct Cyc_List_List*exist_types,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*dles,struct _tuple1*tdn){
# 1843
int do_declare=0;
struct Cyc_Absyn_Exp*xexp;
if((nv->struct_info).lhs_exp == 0){
struct _tuple1*_tmp3D7=Cyc_Toc_temp_var();
xexp=Cyc_Absyn_var_exp(_tmp3D7,0);
do_declare=1;}else{
# 1851
xexp=(struct Cyc_Absyn_Exp*)_check_null((nv->struct_info).lhs_exp);
nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);}{
# 1854
struct Cyc_Absyn_Stmt*_tmp3D8=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(xexp),0);
# 1858
struct Cyc_Absyn_Exp**varexp;
{void*_tmp3D9=_tmp3D8->r;void*_tmp3DA=_tmp3D9;struct Cyc_Absyn_Exp**_tmp3DC;_LL190: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp3DB=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3DA;if(_tmp3DB->tag != 1)goto _LL192;else{_tmp3DC=(struct Cyc_Absyn_Exp**)& _tmp3DB->f1;}}_LL191:
 varexp=_tmp3DC;goto _LL18F;_LL192:;_LL193: {
const char*_tmpBDD;void*_tmpBDC;(_tmpBDC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBDD="stmt not an expression!",_tag_dyneither(_tmpBDD,sizeof(char),24))),_tag_dyneither(_tmpBDC,sizeof(void*),0)));}_LL18F:;}{
# 1864
struct Cyc_Absyn_Exp*(*_tmp3DF)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
void*aggr_typ=Cyc_Toc_typ_to_c(struct_type);
void*orig_typ=aggr_typ;
# 1868
int is_atomic=1;
struct Cyc_List_List*forall_types;
struct Cyc_Absyn_Aggrdecl*ad;
struct Cyc_List_List*aggrfields;struct Cyc_List_List*orig_aggrfields;
struct _dyneither_ptr**index=0;
{void*_tmp3E0=Cyc_Tcutil_compress(struct_type);void*_tmp3E1=_tmp3E0;union Cyc_Absyn_AggrInfoU _tmp3E3;struct Cyc_List_List*_tmp3E4;_LL195: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3E2=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3E1;if(_tmp3E2->tag != 11)goto _LL197;else{_tmp3E3=(_tmp3E2->f1).aggr_info;_tmp3E4=(_tmp3E2->f1).targs;}}_LL196:
# 1875
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp3E3);
orig_aggrfields=(aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
forall_types=_tmp3E4;
goto _LL194;_LL197:;_LL198: {
const char*_tmpBE0;void*_tmpBDF;(_tmpBDF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBE0="init_struct: bad struct type",_tag_dyneither(_tmpBE0,sizeof(char),29))),_tag_dyneither(_tmpBDF,sizeof(void*),0)));}_LL194:;}
# 1882
if(exist_types != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields != 0){
struct Cyc_List_List*_tmp3E7=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
for(0;_tmp3E7->tl != 0;_tmp3E7=_tmp3E7->tl){;}{
struct Cyc_Absyn_Aggrfield*_tmp3E8=(struct Cyc_Absyn_Aggrfield*)_tmp3E7->hd;
struct _RegionHandle _tmp3E9=_new_region("temp");struct _RegionHandle*temp=& _tmp3E9;_push_region(temp);
{struct Cyc_List_List*_tmp3EA=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,forall_types);
# 1891
if(Cyc_Toc_is_abstract_type(Cyc_Tcutil_rsubstitute(temp,_tmp3EA,_tmp3E8->type))){
struct Cyc_List_List*_tmp3EB=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,exist_types);
struct Cyc_List_List*_tmp3EC=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(temp,_tmp3EA,_tmp3EB);
# 1895
struct Cyc_List_List*new_fields=0;
for(_tmp3E7=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;_tmp3E7 != 0;_tmp3E7=_tmp3E7->tl){
struct Cyc_Absyn_Aggrfield*_tmp3ED=(struct Cyc_Absyn_Aggrfield*)_tmp3E7->hd;
struct Cyc_Absyn_Aggrfield*_tmpBE1;struct Cyc_Absyn_Aggrfield*_tmp3EE=(_tmpBE1=_cycalloc(sizeof(*_tmpBE1)),((_tmpBE1->name=_tmp3ED->name,((_tmpBE1->tq=Cyc_Toc_mt_tq,((_tmpBE1->type=
# 1900
Cyc_Tcutil_rsubstitute(temp,_tmp3EC,_tmp3ED->type),((_tmpBE1->width=_tmp3ED->width,((_tmpBE1->attributes=_tmp3ED->attributes,((_tmpBE1->requires_clause=0,_tmpBE1)))))))))))));
# 1908
if(_tmp3E7->tl == 0){
if(Cyc_Toc_init_variable_array(& _tmp3EE->type)){
struct _dyneither_ptr**_tmpBE2;index=((_tmpBE2=_cycalloc(sizeof(*_tmpBE2)),((_tmpBE2[0]=_tmp3EE->name,_tmpBE2))));}}{
# 1915
struct Cyc_List_List*_tmpBE3;new_fields=((_tmpBE3=_cycalloc(sizeof(*_tmpBE3)),((_tmpBE3->hd=_tmp3EE,((_tmpBE3->tl=new_fields,_tmpBE3))))));};}
# 1917
exist_types=0;
aggrfields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);{
struct Cyc_Int_pa_PrintArg_struct _tmpBEB;void*_tmpBEA[1];const char*_tmpBE9;struct _dyneither_ptr*_tmpBE8;struct _dyneither_ptr*name=
(_tmpBE8=_cycalloc(sizeof(*_tmpBE8)),((_tmpBE8[0]=(struct _dyneither_ptr)((_tmpBEB.tag=1,((_tmpBEB.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpBEA[0]=& _tmpBEB,Cyc_aprintf(((_tmpBE9="_genStruct%d",_tag_dyneither(_tmpBE9,sizeof(char),13))),_tag_dyneither(_tmpBEA,sizeof(void*),1)))))))),_tmpBE8)));
struct _tuple1*_tmpBEC;struct _tuple1*qv=(_tmpBEC=_cycalloc(sizeof(*_tmpBEC)),((_tmpBEC->f1=Cyc_Absyn_Abs_n(0,1),((_tmpBEC->f2=name,_tmpBEC)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpBED;struct Cyc_Absyn_AggrdeclImpl*_tmp3F2=(_tmpBED=_cycalloc(sizeof(*_tmpBED)),((_tmpBED->exist_vars=0,((_tmpBED->rgn_po=0,((_tmpBED->fields=aggrfields,((_tmpBED->tagged=0,_tmpBED)))))))));
# 1926
struct Cyc_Absyn_Aggrdecl*_tmpBEE;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpBEE=_cycalloc(sizeof(*_tmpBEE)),((_tmpBEE->kind=Cyc_Absyn_StructA,((_tmpBEE->sc=Cyc_Absyn_Public,((_tmpBEE->name=qv,((_tmpBEE->tvs=0,((_tmpBEE->expected_mem_kind=0,((_tmpBEE->impl=_tmp3F2,((_tmpBEE->attributes=0,_tmpBEE)))))))))))))));
# 1933
Cyc_Toc_aggrdecl_to_c(new_ad,1);
ad=new_ad;
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpBF8;struct Cyc_Absyn_Aggrdecl**_tmpBF7;struct Cyc_Absyn_AggrInfo _tmpBF6;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpBF5;struct_type=(void*)((_tmpBF5=_cycalloc(sizeof(*_tmpBF5)),((_tmpBF5[0]=((_tmpBF8.tag=11,((_tmpBF8.f1=((_tmpBF6.aggr_info=Cyc_Absyn_KnownAggr(((_tmpBF7=_cycalloc(sizeof(*_tmpBF7)),((_tmpBF7[0]=new_ad,_tmpBF7))))),((_tmpBF6.targs=0,_tmpBF6)))),_tmpBF8)))),_tmpBF5))));}
# 1937
aggr_typ=Cyc_Toc_typ_to_c(struct_type);};}}
# 1887
;_pop_region(temp);};}{
# 1941
int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct _RegionHandle _tmp3FE=_new_region("r");struct _RegionHandle*r=& _tmp3FE;_push_region(r);
{struct Cyc_List_List*_tmp3FF=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp3FF != 0;_tmp3FF=_tmp3FF->tl){
struct _tuple19*_tmp400=(struct _tuple19*)_tmp3FF->hd;struct Cyc_List_List*_tmp402;struct Cyc_Absyn_Exp*_tmp403;struct _tuple19*_tmp401=_tmp400;_tmp402=_tmp401->f1;_tmp403=_tmp401->f2;
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(_tmp403->topt));
if(_tmp402 == 0){
const char*_tmpBFB;void*_tmpBFA;(_tmpBFA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBFB="empty designator list",_tag_dyneither(_tmpBFB,sizeof(char),22))),_tag_dyneither(_tmpBFA,sizeof(void*),0)));}
if(_tmp402->tl != 0){
# 1951
struct _tuple1*_tmp406=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp407=Cyc_Absyn_var_exp(_tmp406,0);
for(0;_tmp402 != 0;_tmp402=_tmp402->tl){
void*_tmp408=(void*)_tmp402->hd;void*_tmp409=_tmp408;struct _dyneither_ptr*_tmp40B;_LL19A: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp40A=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp409;if(_tmp40A->tag != 1)goto _LL19C;else{_tmp40B=_tmp40A->f1;}}_LL19B:
# 1957
 if(Cyc_Toc_is_poly_field(struct_type,_tmp40B))
_tmp407=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp407);
_tmp3D8=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3DF(xexp,_tmp40B,0),_tmp407,0),0),_tmp3D8,0);
# 1961
goto _LL199;_LL19C:;_LL19D: {
const char*_tmpBFE;void*_tmpBFD;(_tmpBFD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBFE="array designator in struct",_tag_dyneither(_tmpBFE,sizeof(char),27))),_tag_dyneither(_tmpBFD,sizeof(void*),0)));}_LL199:;}
# 1965
Cyc_Toc_exp_to_c(nv,_tmp403);
_tmp3D8=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp407,_tmp403,0),0),_tmp3D8,0);}else{
# 1968
void*_tmp40E=(void*)_tmp402->hd;void*_tmp40F=_tmp40E;struct _dyneither_ptr*_tmp411;_LL19F: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp410=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp40F;if(_tmp410->tag != 1)goto _LL1A1;else{_tmp411=_tmp410->f1;}}_LL1A0: {
# 1970
struct Cyc_Absyn_Exp*lval=_tmp3DF(xexp,_tmp411,0);
if(is_tagged_union){
int i=Cyc_Toc_get_member_offset(ad,_tmp411);
struct Cyc_Absyn_Exp*f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct Cyc_Absyn_Exp*lhs=Cyc_Toc_member_exp(lval,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,f_tag_exp,0);
_tmp3D8=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp3D8,0);
lval=Cyc_Toc_member_exp(lval,Cyc_Toc_val_sp,0);}{
# 1979
int e1_translated=0;
{void*_tmp412=_tmp403->r;void*_tmp413=_tmp412;struct Cyc_List_List*_tmp415;struct Cyc_Absyn_Vardecl*_tmp417;struct Cyc_Absyn_Exp*_tmp418;struct Cyc_Absyn_Exp*_tmp419;int _tmp41A;struct Cyc_Absyn_Exp*_tmp41C;void*_tmp41D;void*_tmp41F;struct Cyc_List_List*_tmp420;_LL1A4: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp414=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp413;if(_tmp414->tag != 25)goto _LL1A6;else{_tmp415=_tmp414->f1;}}_LL1A5:
# 1982
 _tmp3D8=Cyc_Toc_init_array(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(aggrfields,_tmp411)))->type,lval,_tmp415,_tmp3D8);
# 1984
goto _LL1A3;_LL1A6: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp416=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp413;if(_tmp416->tag != 26)goto _LL1A8;else{_tmp417=_tmp416->f1;_tmp418=_tmp416->f2;_tmp419=_tmp416->f3;_tmp41A=_tmp416->f4;}}_LL1A7:
# 1986
 _tmp3D8=Cyc_Toc_init_comprehension(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(do_declare?aggrfields: orig_aggrfields,_tmp411)))->type,lval,_tmp417,_tmp418,_tmp419,_tmp41A,_tmp3D8,0);
# 1990
e1_translated=1;
_tmp41C=_tmp418;_tmp41D=(void*)_check_null(_tmp419->topt);goto _LL1A9;_LL1A8: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp41B=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp413;if(_tmp41B->tag != 27)goto _LL1AA;else{_tmp41C=_tmp41B->f1;_tmp41D=(void*)_tmp41B->f2;}}_LL1A9:
# 1995
 if(index != 0  && !Cyc_strptrcmp(*index,_tmp411)){
if(!e1_translated)Cyc_Toc_exp_to_c(nv,_tmp41C);{
void*_tmp421=Cyc_Toc_typ_to_c(_tmp41D);
struct _tuple6 _tmpC02;union Cyc_Absyn_Cnst _tmpC01;struct Cyc_Absyn_Exp*_tmpBFF[2];*(nv->struct_info).varsizeexp=
Cyc_Absyn_add_exp(Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpBFF[1]=Cyc_Absyn_sizeoftyp_exp(_tmp421,0),((_tmpBFF[0]=_tmp41C,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBFF,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),
Cyc_Absyn_const_exp((((_tmpC01.Int_c).val=((_tmpC02.f1=Cyc_Absyn_Unsigned,((_tmpC02.f2=(int)sizeof(double),_tmpC02)))),(((_tmpC01.Int_c).tag=5,_tmpC01)))),0),0);};}
# 2003
goto _LL1A3;_LL1AA: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp41E=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp413;if(_tmp41E->tag != 29)goto _LL1AC;else{_tmp41F=(void*)_tmp41E->f1;_tmp420=_tmp41E->f2;}}_LL1AB:
# 2005
 _tmp3D8=Cyc_Toc_init_anon_struct(nv,lval,_tmp41F,_tmp420,_tmp3D8);goto _LL1A3;_LL1AC:;_LL1AD: {
# 2007
void*old_e_typ=(void*)_check_null(_tmp403->topt);
int was_ptr_type=Cyc_Toc_is_pointer_or_boxed_tvar(old_e_typ);
Cyc_Toc_exp_to_c(Cyc_Toc_set_lhs_exp(nv->rgn,lval,nv),_tmp403);
{void*_tmp425=old_e_typ;_LL1AF: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp426=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp425;if(_tmp426->tag != 11)goto _LL1B1;}_LL1B0:
# 2014
 if(old_e_typ != _tmp403->topt  && !Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(old_e_typ),(void*)_check_null(_tmp403->topt))){
# 2018
ad=Cyc_Toc_update_aggr_type(ad,_tmp411,(void*)_check_null(_tmp403->topt));
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpC0C;struct Cyc_Absyn_Aggrdecl**_tmpC0B;struct Cyc_Absyn_AggrInfo _tmpC0A;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC09;struct_type=(void*)(
(_tmpC09=_cycalloc(sizeof(*_tmpC09)),((_tmpC09[0]=((_tmpC0C.tag=11,((_tmpC0C.f1=((_tmpC0A.aggr_info=Cyc_Absyn_KnownAggr(((_tmpC0B=_cycalloc(sizeof(*_tmpC0B)),((_tmpC0B[0]=ad,_tmpC0B))))),((_tmpC0A.targs=0,_tmpC0A)))),_tmpC0C)))),_tmpC09))));}
# 2022
aggr_typ=Cyc_Toc_typ_to_c(struct_type);}
# 2024
goto _LL1AE;_LL1B1:;_LL1B2:
 goto _LL1AE;_LL1AE:;}{
# 2027
struct Cyc_Absyn_Aggrfield*_tmp42B=Cyc_Absyn_lookup_field(aggrfields,_tmp411);
# 2029
if(Cyc_Toc_is_poly_field(struct_type,_tmp411) && !was_ptr_type)
_tmp403=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp403);
# 2032
if(exist_types != 0)
_tmp403=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp42B))->type),_tmp403);
# 2035
_tmp3D8=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp403,0),0),_tmp3D8,0);
goto _LL1A3;};}_LL1A3:;}
# 2038
goto _LL19E;};}_LL1A1:;_LL1A2: {
const char*_tmpC0F;void*_tmpC0E;(_tmpC0E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC0F="array designator in struct",_tag_dyneither(_tmpC0F,sizeof(char),27))),_tag_dyneither(_tmpC0E,sizeof(void*),0)));}_LL19E:;}}}
# 2045
if(aggr_typ != orig_typ  && pointer){
void*cast_type=Cyc_Absyn_cstar_typ(orig_typ,Cyc_Toc_mt_tq);
*varexp=Cyc_Toc_cast_it(cast_type,*varexp);}{
# 2050
struct Cyc_Absyn_Exp*_tmp42E=Cyc_Toc_make_struct(nv,xexp,aggr_typ,_tmp3D8,pointer,rgnopt,is_atomic,do_declare);
# 2052
_tmp42E->topt=struct_type;{
struct Cyc_Absyn_Exp*_tmp42F=_tmp42E;_npop_handler(0);return _tmp42F;};};
# 1943
;_pop_region(r);};};};}struct _tuple27{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 2058
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple27*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
# 2061
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0);}
# 2064
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmpC12;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmpC11;return Cyc_Absyn_new_exp((void*)((_tmpC11=_cycalloc(sizeof(*_tmpC11)),((_tmpC11[0]=((_tmpC12.tag=4,((_tmpC12.f1=e1,((_tmpC12.f2=incr,_tmpC12)))))),_tmpC11)))),0);}
# 2068
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 2077
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 2079
void*_tmp432=e1->r;void*_tmp433=_tmp432;struct Cyc_Absyn_Stmt*_tmp435;void*_tmp437;struct Cyc_Absyn_Exp*_tmp438;struct Cyc_Absyn_Exp*_tmp43A;struct _dyneither_ptr*_tmp43B;int _tmp43C;int _tmp43D;_LL1B4: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp434=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp433;if(_tmp434->tag != 36)goto _LL1B6;else{_tmp435=_tmp434->f1;}}_LL1B5:
 Cyc_Toc_lvalue_assign_stmt(_tmp435,fs,f,f_env);goto _LL1B3;_LL1B6: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp436=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp433;if(_tmp436->tag != 13)goto _LL1B8;else{_tmp437=(void*)_tmp436->f1;_tmp438=_tmp436->f2;}}_LL1B7:
 Cyc_Toc_lvalue_assign(_tmp438,fs,f,f_env);goto _LL1B3;_LL1B8: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp439=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp433;if(_tmp439->tag != 20)goto _LL1BA;else{_tmp43A=_tmp439->f1;_tmp43B=_tmp439->f2;_tmp43C=_tmp439->f3;_tmp43D=_tmp439->f4;}}_LL1B9:
# 2084
 e1->r=_tmp43A->r;
{struct Cyc_List_List*_tmpC13;Cyc_Toc_lvalue_assign(e1,((_tmpC13=_cycalloc(sizeof(*_tmpC13)),((_tmpC13->hd=_tmp43B,((_tmpC13->tl=fs,_tmpC13)))))),f,f_env);}
goto _LL1B3;_LL1BA:;_LL1BB: {
# 2092
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 2094
for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;
goto _LL1B3;}_LL1B3:;}
# 2100
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 2102
void*_tmp43F=s->r;void*_tmp440=_tmp43F;struct Cyc_Absyn_Exp*_tmp442;struct Cyc_Absyn_Decl*_tmp444;struct Cyc_Absyn_Stmt*_tmp445;struct Cyc_Absyn_Stmt*_tmp447;_LL1BD: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp441=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp440;if(_tmp441->tag != 1)goto _LL1BF;else{_tmp442=_tmp441->f1;}}_LL1BE:
 Cyc_Toc_lvalue_assign(_tmp442,fs,f,f_env);goto _LL1BC;_LL1BF: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp443=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp440;if(_tmp443->tag != 12)goto _LL1C1;else{_tmp444=_tmp443->f1;_tmp445=_tmp443->f2;}}_LL1C0:
# 2105
 Cyc_Toc_lvalue_assign_stmt(_tmp445,fs,f,f_env);goto _LL1BC;_LL1C1: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp446=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp440;if(_tmp446->tag != 2)goto _LL1C3;else{_tmp447=_tmp446->f2;}}_LL1C2:
 Cyc_Toc_lvalue_assign_stmt(_tmp447,fs,f,f_env);goto _LL1BC;_LL1C3:;_LL1C4: {
const char*_tmpC17;void*_tmpC16[1];struct Cyc_String_pa_PrintArg_struct _tmpC15;(_tmpC15.tag=0,((_tmpC15.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpC16[0]=& _tmpC15,Cyc_Toc_toc_impos(((_tmpC17="lvalue_assign_stmt: %s",_tag_dyneither(_tmpC17,sizeof(char),23))),_tag_dyneither(_tmpC16,sizeof(void*),1)))))));}_LL1BC:;}
# 2111
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 2115
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp44B=e->r;void*_tmp44C=_tmp44B;void**_tmp44E;struct Cyc_Absyn_Exp**_tmp44F;struct Cyc_Absyn_Exp*_tmp451;struct Cyc_Absyn_Stmt*_tmp453;_LL1C6: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp44D=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp44C;if(_tmp44D->tag != 13)goto _LL1C8;else{_tmp44E=(void**)((void**)& _tmp44D->f1);_tmp44F=(struct Cyc_Absyn_Exp**)& _tmp44D->f2;}}_LL1C7:
# 2118
*_tmp44F=Cyc_Toc_push_address_exp(*_tmp44F);
*_tmp44E=Cyc_Absyn_cstar_typ(*_tmp44E,Cyc_Toc_mt_tq);
return e;_LL1C8: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp450=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp44C;if(_tmp450->tag != 19)goto _LL1CA;else{_tmp451=_tmp450->f1;}}_LL1C9:
# 2122
 return _tmp451;_LL1CA: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp452=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp44C;if(_tmp452->tag != 36)goto _LL1CC;else{_tmp453=_tmp452->f1;}}_LL1CB:
# 2126
 Cyc_Toc_push_address_stmt(_tmp453);
return e;_LL1CC:;_LL1CD:
# 2129
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{
const char*_tmpC1B;void*_tmpC1A[1];struct Cyc_String_pa_PrintArg_struct _tmpC19;(_tmpC19.tag=0,((_tmpC19.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC1A[0]=& _tmpC19,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC1B="can't take & of exp %s",_tag_dyneither(_tmpC1B,sizeof(char),23))),_tag_dyneither(_tmpC1A,sizeof(void*),1)))))));};_LL1C5:;}
# 2134
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp457=s->r;void*_tmp458=_tmp457;struct Cyc_Absyn_Stmt*_tmp45A;struct Cyc_Absyn_Stmt*_tmp45C;struct Cyc_Absyn_Exp**_tmp45E;_LL1CF: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp459=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp458;if(_tmp459->tag != 2)goto _LL1D1;else{_tmp45A=_tmp459->f2;}}_LL1D0:
 _tmp45C=_tmp45A;goto _LL1D2;_LL1D1: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp45B=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp458;if(_tmp45B->tag != 12)goto _LL1D3;else{_tmp45C=_tmp45B->f2;}}_LL1D2:
 Cyc_Toc_push_address_stmt(_tmp45C);goto _LL1CE;_LL1D3: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp45D=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp458;if(_tmp45D->tag != 1)goto _LL1D5;else{_tmp45E=(struct Cyc_Absyn_Exp**)& _tmp45D->f1;}}_LL1D4:
*_tmp45E=Cyc_Toc_push_address_exp(*_tmp45E);goto _LL1CE;_LL1D5:;_LL1D6: {
# 2140
const char*_tmpC1F;void*_tmpC1E[1];struct Cyc_String_pa_PrintArg_struct _tmpC1D;(_tmpC1D.tag=0,((_tmpC1D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpC1E[0]=& _tmpC1D,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC1F="can't take & of stmt %s",_tag_dyneither(_tmpC1F,sizeof(char),24))),_tag_dyneither(_tmpC1E,sizeof(void*),1)))))));}_LL1CE:;}
# 2145
static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 2147
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 2149
if(x == 0)return 0;
{struct Cyc_List_List*_tmpC20;result=((_tmpC20=_region_malloc(r2,sizeof(*_tmpC20)),((_tmpC20->hd=f(x->hd,env),((_tmpC20->tl=0,_tmpC20))))));}
prev=result;
for(x=x->tl;x != 0;x=x->tl){
{struct Cyc_List_List*_tmpC21;((struct Cyc_List_List*)_check_null(prev))->tl=((_tmpC21=_region_malloc(r2,sizeof(*_tmpC21)),((_tmpC21->hd=f(x->hd,env),((_tmpC21->tl=0,_tmpC21))))));}
prev=prev->tl;}
# 2156
return result;}
# 2158
static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,x);}
# 2162
static struct _tuple19*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){
struct _tuple19*_tmpC22;return(_tmpC22=_cycalloc(sizeof(*_tmpC22)),((_tmpC22->f1=0,((_tmpC22->f2=e,_tmpC22)))));}
# 2166
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_tmp465=Cyc_Tcutil_compress(t);void*_tmp466=_tmp465;struct Cyc_Absyn_PtrInfo _tmp468;_LL1D8: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp467=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp466;if(_tmp467->tag != 5)goto _LL1DA;else{_tmp468=_tmp467->f1;}}_LL1D9:
 return _tmp468;_LL1DA:;_LL1DB: {
const char*_tmpC25;void*_tmpC24;(_tmpC24=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC25="get_ptr_typ: not a pointer!",_tag_dyneither(_tmpC25,sizeof(char),28))),_tag_dyneither(_tmpC24,sizeof(void*),0)));}_LL1D7:;}
# 2176
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp46B=Cyc_Tcutil_compress(t);void*_tmp46C=_tmp46B;enum Cyc_Absyn_Sign _tmp46F;enum Cyc_Absyn_Sign _tmp471;enum Cyc_Absyn_Sign _tmp475;enum Cyc_Absyn_Sign _tmp477;enum Cyc_Absyn_Sign _tmp479;int _tmp47D;_LL1DD: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp46D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46C;if(_tmp46D->tag != 5)goto _LL1DF;}_LL1DE:
 res=Cyc_Absyn_null_exp(0);goto _LL1DC;_LL1DF: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp46E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46C;if(_tmp46E->tag != 6)goto _LL1E1;else{_tmp46F=_tmp46E->f1;if(_tmp46E->f2 != Cyc_Absyn_Char_sz)goto _LL1E1;}}_LL1E0:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp46F,'\000'),0);goto _LL1DC;_LL1E1: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp470=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46C;if(_tmp470->tag != 6)goto _LL1E3;else{_tmp471=_tmp470->f1;if(_tmp470->f2 != Cyc_Absyn_Short_sz)goto _LL1E3;}}_LL1E2:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp471,0),0);goto _LL1DC;_LL1E3: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp472=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp46C;if(_tmp472->tag != 13)goto _LL1E5;}_LL1E4:
 goto _LL1E6;_LL1E5: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp473=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp46C;if(_tmp473->tag != 14)goto _LL1E7;}_LL1E6:
 _tmp475=Cyc_Absyn_Unsigned;goto _LL1E8;_LL1E7: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp474=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46C;if(_tmp474->tag != 6)goto _LL1E9;else{_tmp475=_tmp474->f1;if(_tmp474->f2 != Cyc_Absyn_Int_sz)goto _LL1E9;}}_LL1E8:
 _tmp477=_tmp475;goto _LL1EA;_LL1E9: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp476=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46C;if(_tmp476->tag != 6)goto _LL1EB;else{_tmp477=_tmp476->f1;if(_tmp476->f2 != Cyc_Absyn_Long_sz)goto _LL1EB;}}_LL1EA:
# 2186
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp477,0),0);goto _LL1DC;_LL1EB: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp478=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46C;if(_tmp478->tag != 6)goto _LL1ED;else{_tmp479=_tmp478->f1;if(_tmp478->f2 != Cyc_Absyn_LongLong_sz)goto _LL1ED;}}_LL1EC:
# 2188
 res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp479,(long long)0),0);goto _LL1DC;_LL1ED: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp47A=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp46C;if(_tmp47A->tag != 7)goto _LL1EF;else{if(_tmp47A->f1 != 0)goto _LL1EF;}}_LL1EE:
{const char*_tmpC26;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpC26="0.0F",_tag_dyneither(_tmpC26,sizeof(char),5))),0),0);}goto _LL1DC;_LL1EF: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp47B=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp46C;if(_tmp47B->tag != 7)goto _LL1F1;else{if(_tmp47B->f1 != 1)goto _LL1F1;}}_LL1F0:
{const char*_tmpC27;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpC27="0.0",_tag_dyneither(_tmpC27,sizeof(char),4))),1),0);}goto _LL1DC;_LL1F1: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp47C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp46C;if(_tmp47C->tag != 7)goto _LL1F3;else{_tmp47D=_tmp47C->f1;}}_LL1F2:
{const char*_tmpC28;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpC28="0.0L",_tag_dyneither(_tmpC28,sizeof(char),5))),_tmp47D),0);}goto _LL1DC;_LL1F3:;_LL1F4: {
# 2193
const char*_tmpC2C;void*_tmpC2B[1];struct Cyc_String_pa_PrintArg_struct _tmpC2A;(_tmpC2A.tag=0,((_tmpC2A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpC2B[0]=& _tmpC2A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC2C="found non-zero type %s in generate_zero",_tag_dyneither(_tmpC2C,sizeof(char),40))),_tag_dyneither(_tmpC2B,sizeof(void*),1)))))));}_LL1DC:;}
# 2195
res->topt=t;
return res;}
# 2202
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 2215 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp484=Cyc_Toc_typ_to_c(elt_type);
void*_tmp485=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp486=Cyc_Absyn_cstar_typ(_tmp484,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmpC2D;struct Cyc_Core_Opt*_tmp487=(_tmpC2D=_cycalloc(sizeof(*_tmpC2D)),((_tmpC2D->v=_tmp486,_tmpC2D)));
struct Cyc_Absyn_Exp*xinit;
{void*_tmp488=e1->r;void*_tmp489=_tmp488;struct Cyc_Absyn_Exp*_tmp48B;struct Cyc_Absyn_Exp*_tmp48D;struct Cyc_Absyn_Exp*_tmp48E;_LL1F6: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp48A=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp489;if(_tmp48A->tag != 19)goto _LL1F8;else{_tmp48B=_tmp48A->f1;}}_LL1F7:
# 2223
 if(!is_dyneither){
_tmp48B=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp48B,0,Cyc_Absyn_Other_coercion,0);
_tmp48B->topt=fat_ptr_type;}
# 2227
Cyc_Toc_exp_to_c(nv,_tmp48B);xinit=_tmp48B;goto _LL1F5;_LL1F8: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp48C=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp489;if(_tmp48C->tag != 22)goto _LL1FA;else{_tmp48D=_tmp48C->f1;_tmp48E=_tmp48C->f2;}}_LL1F9:
# 2229
 if(!is_dyneither){
_tmp48D=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp48D,0,Cyc_Absyn_Other_coercion,0);
_tmp48D->topt=fat_ptr_type;}
# 2233
Cyc_Toc_exp_to_c(nv,_tmp48D);Cyc_Toc_exp_to_c(nv,_tmp48E);
{struct Cyc_Absyn_Exp*_tmpC2E[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpC2E[2]=_tmp48E,((_tmpC2E[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpC2E[0]=_tmp48D,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC2E,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL1F5;_LL1FA:;_LL1FB: {
const char*_tmpC31;void*_tmpC30;(_tmpC30=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC31="found bad lhs for zero-terminated pointer assignment",_tag_dyneither(_tmpC31,sizeof(char),53))),_tag_dyneither(_tmpC30,sizeof(void*),0)));}_LL1F5:;}{
# 2239
struct _tuple1*_tmp492=Cyc_Toc_temp_var();
struct _RegionHandle _tmp493=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp493;_push_region(rgn2);
{struct Cyc_Toc_Env*_tmp494=Cyc_Toc_add_varmap(rgn2,nv,_tmp492,Cyc_Absyn_var_exp(_tmp492,0));
struct Cyc_Absyn_Vardecl*_tmpC32;struct Cyc_Absyn_Vardecl*_tmp495=(_tmpC32=_cycalloc(sizeof(*_tmpC32)),((_tmpC32->sc=Cyc_Absyn_Public,((_tmpC32->name=_tmp492,((_tmpC32->tq=Cyc_Toc_mt_tq,((_tmpC32->type=_tmp485,((_tmpC32->initializer=xinit,((_tmpC32->rgn=0,((_tmpC32->attributes=0,((_tmpC32->escapes=0,_tmpC32)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpC35;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpC34;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp496=(_tmpC34=_cycalloc(sizeof(*_tmpC34)),((_tmpC34[0]=((_tmpC35.tag=4,((_tmpC35.f1=_tmp495,_tmpC35)))),_tmpC34)));
struct Cyc_Absyn_Exp*_tmp497=Cyc_Absyn_varb_exp(_tmp492,(void*)_tmp496,0);
_tmp497->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp498=Cyc_Absyn_deref_exp(_tmp497,0);
_tmp498->topt=elt_type;
Cyc_Toc_exp_to_c(_tmp494,_tmp498);{
struct _tuple1*_tmp499=Cyc_Toc_temp_var();
_tmp494=Cyc_Toc_add_varmap(rgn2,_tmp494,_tmp499,Cyc_Absyn_var_exp(_tmp499,0));{
struct Cyc_Absyn_Vardecl*_tmpC36;struct Cyc_Absyn_Vardecl*_tmp49A=(_tmpC36=_cycalloc(sizeof(*_tmpC36)),((_tmpC36->sc=Cyc_Absyn_Public,((_tmpC36->name=_tmp499,((_tmpC36->tq=Cyc_Toc_mt_tq,((_tmpC36->type=_tmp484,((_tmpC36->initializer=_tmp498,((_tmpC36->rgn=0,((_tmpC36->attributes=0,((_tmpC36->escapes=0,_tmpC36)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpC39;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpC38;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp49B=(_tmpC38=_cycalloc(sizeof(*_tmpC38)),((_tmpC38[0]=((_tmpC39.tag=4,((_tmpC39.f1=_tmp49A,_tmpC39)))),_tmpC38)));
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp49C=Cyc_Absyn_varb_exp(_tmp499,(void*)_tmp49B,0);
_tmp49C->topt=_tmp498->topt;
z_init=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp49C,e2,0);
z_init->topt=_tmp49C->topt;}
# 2260
Cyc_Toc_exp_to_c(_tmp494,z_init);{
struct _tuple1*_tmp49D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmpC3A;struct Cyc_Absyn_Vardecl*_tmp49E=(_tmpC3A=_cycalloc(sizeof(*_tmpC3A)),((_tmpC3A->sc=Cyc_Absyn_Public,((_tmpC3A->name=_tmp49D,((_tmpC3A->tq=Cyc_Toc_mt_tq,((_tmpC3A->type=_tmp484,((_tmpC3A->initializer=z_init,((_tmpC3A->rgn=0,((_tmpC3A->attributes=0,((_tmpC3A->escapes=0,_tmpC3A)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpC3D;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpC3C;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp49F=(_tmpC3C=_cycalloc(sizeof(*_tmpC3C)),((_tmpC3C[0]=((_tmpC3D.tag=4,((_tmpC3D.f1=_tmp49E,_tmpC3D)))),_tmpC3C)));
_tmp494=Cyc_Toc_add_varmap(rgn2,_tmp494,_tmp49D,Cyc_Absyn_var_exp(_tmp49D,0));{
# 2267
struct Cyc_Absyn_Exp*_tmp4A0=Cyc_Absyn_varb_exp(_tmp499,(void*)_tmp49B,0);_tmp4A0->topt=_tmp498->topt;{
struct Cyc_Absyn_Exp*_tmp4A1=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp4A2=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp4A0,_tmp4A1,0);
_tmp4A2->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp494,_tmp4A2);{
# 2273
struct Cyc_Absyn_Exp*_tmp4A3=Cyc_Absyn_varb_exp(_tmp49D,(void*)_tmp49F,0);_tmp4A3->topt=_tmp498->topt;{
struct Cyc_Absyn_Exp*_tmp4A4=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp4A5=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp4A3,_tmp4A4,0);
_tmp4A5->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp494,_tmp4A5);{
# 2279
struct Cyc_Absyn_Exp*_tmpC3E[2];struct Cyc_List_List*_tmp4A6=(_tmpC3E[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpC3E[0]=
# 2279
Cyc_Absyn_varb_exp(_tmp492,(void*)_tmp496,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC3E,sizeof(struct Cyc_Absyn_Exp*),2)))));
# 2281
struct Cyc_Absyn_Exp*_tmp4A7=Cyc_Absyn_uint_exp(1,0);
struct Cyc_Absyn_Exp*xsize;
xsize=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp4A6,0),_tmp4A7,0);{
# 2286
struct Cyc_Absyn_Exp*_tmp4A8=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp4A2,_tmp4A5,0),0);
struct Cyc_Absyn_Stmt*_tmp4A9=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0),0);
struct Cyc_Absyn_Exp*_tmp4AA=Cyc_Toc_member_exp(Cyc_Absyn_varb_exp(_tmp492,(void*)_tmp496,0),Cyc_Toc_curr_sp,0);
_tmp4AA=Cyc_Toc_cast_it(_tmp486,_tmp4AA);{
struct Cyc_Absyn_Exp*_tmp4AB=Cyc_Absyn_deref_exp(_tmp4AA,0);
struct Cyc_Absyn_Exp*_tmp4AC=Cyc_Absyn_assign_exp(_tmp4AB,Cyc_Absyn_var_exp(_tmp49D,0),0);
struct Cyc_Absyn_Stmt*_tmp4AD=Cyc_Absyn_exp_stmt(_tmp4AC,0);
_tmp4AD=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp4A8,_tmp4A9,Cyc_Absyn_skip_stmt(0),0),_tmp4AD,0);
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpC44;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpC43;struct Cyc_Absyn_Decl*_tmpC42;_tmp4AD=Cyc_Absyn_decl_stmt(((_tmpC42=_cycalloc(sizeof(*_tmpC42)),((_tmpC42->r=(void*)((_tmpC44=_cycalloc(sizeof(*_tmpC44)),((_tmpC44[0]=((_tmpC43.tag=0,((_tmpC43.f1=_tmp49E,_tmpC43)))),_tmpC44)))),((_tmpC42->loc=0,_tmpC42)))))),_tmp4AD,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpC4A;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpC49;struct Cyc_Absyn_Decl*_tmpC48;_tmp4AD=Cyc_Absyn_decl_stmt(((_tmpC48=_cycalloc(sizeof(*_tmpC48)),((_tmpC48->r=(void*)((_tmpC4A=_cycalloc(sizeof(*_tmpC4A)),((_tmpC4A[0]=((_tmpC49.tag=0,((_tmpC49.f1=_tmp49A,_tmpC49)))),_tmpC4A)))),((_tmpC48->loc=0,_tmpC48)))))),_tmp4AD,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpC50;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpC4F;struct Cyc_Absyn_Decl*_tmpC4E;_tmp4AD=Cyc_Absyn_decl_stmt(((_tmpC4E=_cycalloc(sizeof(*_tmpC4E)),((_tmpC4E->r=(void*)((_tmpC50=_cycalloc(sizeof(*_tmpC50)),((_tmpC50[0]=((_tmpC4F.tag=0,((_tmpC4F.f1=_tmp495,_tmpC4F)))),_tmpC50)))),((_tmpC4E->loc=0,_tmpC4E)))))),_tmp4AD,0);}
e->r=Cyc_Toc_stmt_exp_r(_tmp4AD);};};};};};};};};};};};}
# 2241
;_pop_region(rgn2);};}
# 2312 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 2316
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp4C2=Cyc_Tcutil_compress(aggrtype);void*_tmp4C3=_tmp4C2;union Cyc_Absyn_AggrInfoU _tmp4C5;_LL1FD: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4C4=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C3;if(_tmp4C4->tag != 11)goto _LL1FF;else{_tmp4C5=(_tmp4C4->f1).aggr_info;}}_LL1FE:
# 2319
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp4C5);goto _LL1FC;_LL1FF:;_LL200: {
struct Cyc_String_pa_PrintArg_struct _tmpC58;void*_tmpC57[1];const char*_tmpC56;void*_tmpC55;(_tmpC55=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmpC58.tag=0,((_tmpC58.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpC57[0]=& _tmpC58,Cyc_aprintf(((_tmpC56="expecting union but found %s in check_tagged_union",_tag_dyneither(_tmpC56,sizeof(char),51))),_tag_dyneither(_tmpC57,sizeof(void*),1)))))))),_tag_dyneither(_tmpC55,sizeof(void*),0)));}_LL1FC:;}{
# 2323
struct _tuple1*_tmp4CA=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4CB=Cyc_Absyn_var_exp(_tmp4CA,0);
struct Cyc_Absyn_Exp*_tmp4CC=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp4CD=Cyc_Absyn_aggrarrow_exp(_tmp4CB,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp4CE=Cyc_Absyn_neq_exp(_tmp4CD,_tmp4CC,0);
struct Cyc_Absyn_Exp*_tmp4CF=Cyc_Absyn_aggrarrow_exp(_tmp4CB,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp4D0=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp4CF,0),0);
struct Cyc_Absyn_Stmt*_tmp4D1=Cyc_Absyn_ifthenelse_stmt(_tmp4CE,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
void*_tmp4D2=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp4D3=Cyc_Absyn_address_exp(aggrproj(e1,f,0),0);
struct Cyc_Absyn_Stmt*_tmp4D4=Cyc_Absyn_declare_stmt(_tmp4CA,_tmp4D2,_tmp4D3,Cyc_Absyn_seq_stmt(_tmp4D1,_tmp4D0,0),0);
return Cyc_Toc_stmt_exp_r(_tmp4D4);}else{
# 2337
struct Cyc_Absyn_Exp*_tmp4D5=Cyc_Toc_member_exp(aggrproj(_tmp4CB,f,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp4D6=Cyc_Absyn_neq_exp(_tmp4D5,_tmp4CC,0);
struct Cyc_Absyn_Exp*_tmp4D7=Cyc_Toc_member_exp(aggrproj(_tmp4CB,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp4D8=Cyc_Absyn_exp_stmt(_tmp4D7,0);
struct Cyc_Absyn_Stmt*_tmp4D9=Cyc_Absyn_ifthenelse_stmt(_tmp4D6,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*_tmp4DA=Cyc_Absyn_declare_stmt(_tmp4CA,e1_c_type,e1,Cyc_Absyn_seq_stmt(_tmp4D9,_tmp4D8,0),0);
return Cyc_Toc_stmt_exp_r(_tmp4DA);}};}
# 2347
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _dyneither_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 2350
void*_tmp4DB=Cyc_Tcutil_compress(t);void*_tmp4DC=_tmp4DB;union Cyc_Absyn_AggrInfoU _tmp4DE;_LL202: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4DD=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4DC;if(_tmp4DD->tag != 11)goto _LL204;else{_tmp4DE=(_tmp4DD->f1).aggr_info;}}_LL203: {
# 2352
struct Cyc_Absyn_Aggrdecl*_tmp4DF=Cyc_Absyn_get_known_aggrdecl(_tmp4DE);
*f_tag=Cyc_Toc_get_member_offset(_tmp4DF,f);{
# 2355
const char*_tmpC5D;void*_tmpC5C[2];struct Cyc_String_pa_PrintArg_struct _tmpC5B;struct Cyc_String_pa_PrintArg_struct _tmpC5A;struct _dyneither_ptr str=(struct _dyneither_ptr)((_tmpC5A.tag=0,((_tmpC5A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpC5B.tag=0,((_tmpC5B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp4DF->name).f2),((_tmpC5C[0]=& _tmpC5B,((_tmpC5C[1]=& _tmpC5A,Cyc_aprintf(((_tmpC5D="_union_%s_%s",_tag_dyneither(_tmpC5D,sizeof(char),13))),_tag_dyneither(_tmpC5C,sizeof(void*),2))))))))))))));
{struct _dyneither_ptr*_tmpC5E;*tagged_member_type=Cyc_Absyn_strct(((_tmpC5E=_cycalloc(sizeof(*_tmpC5E)),((_tmpC5E[0]=str,_tmpC5E)))));}
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4DF->impl))->tagged;};}_LL204:;_LL205:
 return 0;_LL201:;}
# 2366
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 2369
void*_tmp4E5=e->r;void*_tmp4E6=_tmp4E5;struct Cyc_Absyn_Exp*_tmp4E8;struct Cyc_Absyn_Exp*_tmp4EA;struct _dyneither_ptr*_tmp4EB;int*_tmp4EC;struct Cyc_Absyn_Exp*_tmp4EE;struct _dyneither_ptr*_tmp4EF;int*_tmp4F0;_LL207: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4E7=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp4E7->tag != 13)goto _LL209;else{_tmp4E8=_tmp4E7->f2;}}_LL208: {
const char*_tmpC61;void*_tmpC60;(_tmpC60=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC61="cast on lhs!",_tag_dyneither(_tmpC61,sizeof(char),13))),_tag_dyneither(_tmpC60,sizeof(void*),0)));}_LL209: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp4E9=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp4E9->tag != 20)goto _LL20B;else{_tmp4EA=_tmp4E9->f1;_tmp4EB=_tmp4E9->f2;_tmp4EC=(int*)& _tmp4E9->f4;}}_LL20A:
# 2372
 return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(_tmp4EA->topt),_tmp4EB,f_tag,tagged_member_type,clear_read,_tmp4EC);_LL20B: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp4ED=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4E6;if(_tmp4ED->tag != 21)goto _LL20D;else{_tmp4EE=_tmp4ED->f1;_tmp4EF=_tmp4ED->f2;_tmp4F0=(int*)& _tmp4ED->f4;}}_LL20C: {
# 2375
void*_tmp4F3=Cyc_Tcutil_compress((void*)_check_null(_tmp4EE->topt));void*_tmp4F4=_tmp4F3;void*_tmp4F6;_LL210: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4F5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4F4;if(_tmp4F5->tag != 5)goto _LL212;else{_tmp4F6=(_tmp4F5->f1).elt_typ;}}_LL211:
# 2377
 return Cyc_Toc_is_tagged_union_project_impl(_tmp4F6,_tmp4EF,f_tag,tagged_member_type,clear_read,_tmp4F0);_LL212:;_LL213:
# 2379
 return 0;_LL20F:;}_LL20D:;_LL20E:
# 2381
 return 0;_LL206:;}
# 2393 "toc.cyc"
void Cyc_Toc_add_tagged_union_check_for_swap(struct Cyc_Absyn_Exp*e,int tag,void*mem_type){
struct _tuple1*_tmp4F7=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp4F7,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_val,0),0);
struct Cyc_Absyn_Exp*_tmp4F8=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_ifthenelse_stmt(_tmp4F8,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp4F7,Cyc_Absyn_cstar_typ(mem_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(e)),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
e->r=Cyc_Toc_stmt_exp_r(s1);}
# 2417 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 2421
struct _tuple1*_tmp4F9=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp4F9,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{
# 2431
struct Cyc_Absyn_Exp*_tmp4FA=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
s2=Cyc_Absyn_ifthenelse_stmt(_tmp4FA,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{
# 2434
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp4F9,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(e1),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple28{struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};
# 2440
static void Cyc_Toc_new_comprehension_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,void*new_e_type,struct Cyc_Absyn_Exp*rgnopt,void*old_typ,struct Cyc_Absyn_Exp*e1,void*t1,int zero_term,struct Cyc_Absyn_Exp*init_exp,struct Cyc_Absyn_Vardecl*vd){
# 2455 "toc.cyc"
struct _tuple1*max=Cyc_Toc_temp_var();
struct _tuple1*a=Cyc_Toc_temp_var();
void*old_elt_typ=t1;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c(nv,e1);{
struct Cyc_Absyn_Exp*_tmp4FB=Cyc_Absyn_var_exp(max,0);
# 2463
struct Cyc_Absyn_Stmt*s;
if(zero_term)
_tmp4FB=Cyc_Absyn_add_exp(_tmp4FB,Cyc_Absyn_uint_exp(1,0),0);
if(init_exp != 0)
s=Cyc_Toc_init_comprehension(nv,new_e_type,
Cyc_Absyn_var_exp(a,0),(struct Cyc_Absyn_Vardecl*)_check_null(vd),Cyc_Absyn_var_exp(max,0),init_exp,zero_term,
Cyc_Toc_skip_stmt_dl(),1);else{
if(zero_term)
s=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(a,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(elt_typ,Cyc_Absyn_uint_exp(0,0)),0),0);else{
# 2474
s=Cyc_Absyn_skip_stmt(0);}}{
# 2476
struct _RegionHandle _tmp4FC=_new_region("r");struct _RegionHandle*r=& _tmp4FC;_push_region(r);
{struct _tuple28*_tmpC64;struct Cyc_List_List*_tmpC63;struct Cyc_List_List*decls=
(_tmpC63=_region_malloc(r,sizeof(*_tmpC63)),((_tmpC63->hd=((_tmpC64=_region_malloc(r,sizeof(*_tmpC64)),((_tmpC64->f1=max,((_tmpC64->f2=Cyc_Absyn_uint_typ,((_tmpC64->f3=e1,_tmpC64)))))))),((_tmpC63->tl=0,_tmpC63)))));
struct Cyc_Absyn_Exp*ai;
if(rgnopt == 0  || Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*_tmpC65[2];ai=Cyc_Toc_malloc_exp(old_elt_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpC65[1]=_tmp4FB,((_tmpC65[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC65,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));}else{
# 2486
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);{
struct Cyc_Absyn_Exp*_tmpC66[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpC66[1]=_tmp4FB,((_tmpC66[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC66,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{
# 2492
struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);
{struct _tuple28*_tmpC69;struct Cyc_List_List*_tmpC68;decls=((_tmpC68=_region_malloc(r,sizeof(*_tmpC68)),((_tmpC68->hd=((_tmpC69=_region_malloc(r,sizeof(*_tmpC69)),((_tmpC69->f1=a,((_tmpC69->f2=ptr_typ,((_tmpC69->f3=ainit,_tmpC69)))))))),((_tmpC68->tl=decls,_tmpC68))))));}
if(!Cyc_Tcutil_is_pointer_type(old_typ)){
const char*_tmpC6C;void*_tmpC6B;(_tmpC6B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC6C="new_comprehension_to_c: comprehension is not an array type",_tag_dyneither(_tmpC6C,sizeof(char),59))),_tag_dyneither(_tmpC6B,sizeof(void*),0)));}
if(Cyc_Tcutil_is_dyneither_ptr(old_typ)){
struct _tuple1*_tmp503=Cyc_Toc_temp_var();
void*_tmp504=Cyc_Toc_typ_to_c(old_typ);
struct Cyc_Absyn_Exp*_tmp505=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpC6D[3];struct Cyc_Absyn_Exp*_tmp506=
Cyc_Absyn_fncall_exp(_tmp505,(
(_tmpC6D[2]=_tmp4FB,((_tmpC6D[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((_tmpC6D[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC6D,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 2504
{struct _tuple28*_tmpC70;struct Cyc_List_List*_tmpC6F;decls=((_tmpC6F=_region_malloc(r,sizeof(*_tmpC6F)),((_tmpC6F->hd=((_tmpC70=_region_malloc(r,sizeof(*_tmpC70)),((_tmpC70->f1=_tmp503,((_tmpC70->f2=_tmp504,((_tmpC70->f3=_tmp506,_tmpC70)))))))),((_tmpC6F->tl=decls,_tmpC6F))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp503,0),0),0);}else{
# 2507
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}
{struct Cyc_List_List*_tmp50A=decls;for(0;_tmp50A != 0;_tmp50A=_tmp50A->tl){
struct _tuple28 _tmp50B=*((struct _tuple28*)_tmp50A->hd);struct _tuple1*_tmp50D;void*_tmp50E;struct Cyc_Absyn_Exp*_tmp50F;struct _tuple28 _tmp50C=_tmp50B;_tmp50D=_tmp50C.f1;_tmp50E=_tmp50C.f2;_tmp50F=_tmp50C.f3;
s=Cyc_Absyn_declare_stmt(_tmp50D,_tmp50E,_tmp50F,s,0);}}
# 2512
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2477
;_pop_region(r);};};}struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static void _tmpCBF(unsigned int*_tmpCBE,unsigned int*_tmpCBD,struct _tuple1***_tmpCBB){for(*_tmpCBE=0;*_tmpCBE < *_tmpCBD;(*_tmpCBE)++){(*_tmpCBB)[*_tmpCBE]=(struct _tuple1*)
# 2887 "toc.cyc"
Cyc_Toc_temp_var();}}
# 2516 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
void*_tmp512=e->r;
if(e->topt == 0){
const char*_tmpC74;void*_tmpC73[1];struct Cyc_String_pa_PrintArg_struct _tmpC72;(_tmpC72.tag=0,((_tmpC72.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC73[0]=& _tmpC72,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC74="exp_to_c: no type for %s",_tag_dyneither(_tmpC74,sizeof(char),25))),_tag_dyneither(_tmpC73,sizeof(void*),1)))))));}
# 2522
if((nv->struct_info).lhs_exp != 0){
void*_tmp516=_tmp512;_LL215: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp517=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp516;if(_tmp517->tag != 28)goto _LL217;}_LL216:
 goto _LL214;_LL217:;_LL218:
 nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);_LL214:;}{
# 2530
void*old_typ=(void*)_check_null(e->topt);
void*_tmp518=_tmp512;struct _tuple1*_tmp51C;void*_tmp51D;enum Cyc_Absyn_Primop _tmp51F;struct Cyc_List_List*_tmp520;struct Cyc_Absyn_Exp*_tmp522;enum Cyc_Absyn_Incrementor _tmp523;struct Cyc_Absyn_Exp*_tmp525;struct Cyc_Core_Opt*_tmp526;struct Cyc_Absyn_Exp*_tmp527;struct Cyc_Absyn_Exp*_tmp529;struct Cyc_Absyn_Exp*_tmp52A;struct Cyc_Absyn_Exp*_tmp52B;struct Cyc_Absyn_Exp*_tmp52D;struct Cyc_Absyn_Exp*_tmp52E;struct Cyc_Absyn_Exp*_tmp530;struct Cyc_Absyn_Exp*_tmp531;struct Cyc_Absyn_Exp*_tmp533;struct Cyc_Absyn_Exp*_tmp534;struct Cyc_Absyn_Exp*_tmp536;struct Cyc_List_List*_tmp537;struct Cyc_Absyn_Exp*_tmp539;struct Cyc_List_List*_tmp53A;int _tmp53B;struct Cyc_List_List*_tmp53C;struct Cyc_Absyn_VarargInfo*_tmp53D;struct Cyc_Absyn_Exp*_tmp53F;int _tmp540;struct Cyc_Absyn_Exp*_tmp542;struct Cyc_Absyn_Exp*_tmp544;struct Cyc_List_List*_tmp545;void**_tmp547;struct Cyc_Absyn_Exp*_tmp548;int _tmp549;enum Cyc_Absyn_Coercion _tmp54A;struct Cyc_Absyn_Exp*_tmp54C;struct Cyc_Absyn_Exp*_tmp54E;struct Cyc_Absyn_Exp*_tmp54F;struct Cyc_Absyn_Exp*_tmp551;void*_tmp553;void*_tmp555;struct Cyc_List_List*_tmp556;struct Cyc_Absyn_Exp*_tmp558;struct Cyc_Absyn_Exp*_tmp55A;struct _dyneither_ptr*_tmp55B;int _tmp55C;int _tmp55D;struct Cyc_Absyn_Exp*_tmp55F;struct _dyneither_ptr*_tmp560;int _tmp561;int _tmp562;struct Cyc_Absyn_Exp*_tmp564;struct Cyc_Absyn_Exp*_tmp565;struct Cyc_List_List*_tmp567;struct Cyc_List_List*_tmp569;struct Cyc_Absyn_Vardecl*_tmp56B;struct Cyc_Absyn_Exp*_tmp56C;struct Cyc_Absyn_Exp*_tmp56D;int _tmp56E;struct Cyc_Absyn_Exp*_tmp570;void*_tmp571;int _tmp572;struct _tuple1*_tmp574;struct Cyc_List_List*_tmp575;struct Cyc_List_List*_tmp576;struct Cyc_Absyn_Aggrdecl*_tmp577;void*_tmp579;struct Cyc_List_List*_tmp57A;struct Cyc_List_List*_tmp57C;struct Cyc_Absyn_Datatypedecl*_tmp57D;struct Cyc_Absyn_Datatypefield*_tmp57E;int _tmp582;struct Cyc_Absyn_Exp*_tmp583;void**_tmp584;struct Cyc_Absyn_Exp*_tmp585;int _tmp586;int _tmp587;struct Cyc_Absyn_Exp*_tmp589;struct Cyc_Absyn_Exp*_tmp58A;struct Cyc_Absyn_Exp*_tmp58C;struct _dyneither_ptr*_tmp58D;struct Cyc_Absyn_Stmt*_tmp58F;_LL21A: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp519=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp519->tag != 0)goto _LL21C;else{if(((_tmp519->f1).Null_c).tag != 1)goto _LL21C;}}_LL21B: {
# 2537
struct Cyc_Absyn_Exp*_tmp594=Cyc_Absyn_uint_exp(0,0);
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp594,_tmp594))->r;else{
# 2542
struct Cyc_Absyn_Exp*_tmpC75[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((_tmpC75[2]=_tmp594,((_tmpC75[1]=_tmp594,((_tmpC75[0]=_tmp594,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC75,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}else{
# 2544
e->r=(void*)& Cyc_Toc_zero_exp;}
# 2546
goto _LL219;}_LL21C: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp51A=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp51A->tag != 0)goto _LL21E;}_LL21D:
 goto _LL219;_LL21E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp51B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp51B->tag != 1)goto _LL220;else{_tmp51C=_tmp51B->f1;_tmp51D=(void*)_tmp51B->f2;}}_LL21F:
# 2549
{struct _handler_cons _tmp596;_push_handler(& _tmp596);{int _tmp598=0;if(setjmp(_tmp596.handler))_tmp598=1;if(!_tmp598){e->r=(Cyc_Toc_lookup_varmap(nv,_tmp51C))->r;;_pop_handler();}else{void*_tmp597=(void*)_exn_thrown;void*_tmp59A=_tmp597;void*_tmp59C;_LL26F: {struct Cyc_Dict_Absent_exn_struct*_tmp59B=(struct Cyc_Dict_Absent_exn_struct*)_tmp59A;if(_tmp59B->tag != Cyc_Dict_Absent)goto _LL271;}_LL270: {
# 2551
const char*_tmpC79;void*_tmpC78[1];struct Cyc_String_pa_PrintArg_struct _tmpC77;(_tmpC77.tag=0,((_tmpC77.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp51C)),((_tmpC78[0]=& _tmpC77,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC79="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpC79,sizeof(char),32))),_tag_dyneither(_tmpC78,sizeof(void*),1)))))));}_LL271: _tmp59C=_tmp59A;_LL272:(void)_rethrow(_tmp59C);_LL26E:;}};}
# 2553
goto _LL219;_LL220: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp51E=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp51E->tag != 2)goto _LL222;else{_tmp51F=_tmp51E->f1;_tmp520=_tmp51E->f2;}}_LL221: {
# 2556
struct Cyc_List_List*_tmp5A0=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp520);
# 2558
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp520);
switch(_tmp51F){case Cyc_Absyn_Numelts: _LL273: {
# 2561
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp520))->hd;
{void*_tmp5A1=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_tmp5A2=_tmp5A1;struct Cyc_Absyn_Exp*_tmp5A4;void*_tmp5A6;union Cyc_Absyn_Constraint*_tmp5A7;union Cyc_Absyn_Constraint*_tmp5A8;union Cyc_Absyn_Constraint*_tmp5A9;_LL276: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5A3=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A2;if(_tmp5A3->tag != 8)goto _LL278;else{_tmp5A4=(_tmp5A3->f1).num_elts;}}_LL277:
# 2565
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp5A4))){
const char*_tmpC7C;void*_tmpC7B;(_tmpC7B=0,Cyc_Tcutil_terr(e->loc,((_tmpC7C="can't calculate numelts",_tag_dyneither(_tmpC7C,sizeof(char),24))),_tag_dyneither(_tmpC7B,sizeof(void*),0)));}
e->r=_tmp5A4->r;goto _LL275;_LL278: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5A5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A2;if(_tmp5A5->tag != 5)goto _LL27A;else{_tmp5A6=(_tmp5A5->f1).elt_typ;_tmp5A7=((_tmp5A5->f1).ptr_atts).nullable;_tmp5A8=((_tmp5A5->f1).ptr_atts).bounds;_tmp5A9=((_tmp5A5->f1).ptr_atts).zero_term;}}_LL279:
# 2569
{void*_tmp5AC=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5A8);void*_tmp5AD=_tmp5AC;struct Cyc_Absyn_Exp*_tmp5B0;_LL27D: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5AE=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5AD;if(_tmp5AE->tag != 0)goto _LL27F;}_LL27E:
# 2571
{struct Cyc_Absyn_Exp*_tmpC7D[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,(
(_tmpC7D[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5A6),0),((_tmpC7D[0]=(struct Cyc_Absyn_Exp*)_tmp520->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC7D,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL27C;_LL27F: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5AF=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5AD;if(_tmp5AF->tag != 1)goto _LL27C;else{_tmp5B0=_tmp5AF->f1;}}_LL280:
# 2577
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5A9)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp520->hd);
# 2581
struct Cyc_Absyn_Exp*_tmpC7E[2];e->r=Cyc_Toc_fncall_exp_r(function_e,((_tmpC7E[1]=_tmp5B0,((_tmpC7E[0]=(struct Cyc_Absyn_Exp*)_tmp520->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC7E,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5A7)){
if(!Cyc_Evexp_c_can_eval(_tmp5B0)){
const char*_tmpC81;void*_tmpC80;(_tmpC80=0,Cyc_Tcutil_terr(e->loc,((_tmpC81="can't calculate numelts",_tag_dyneither(_tmpC81,sizeof(char),24))),_tag_dyneither(_tmpC80,sizeof(void*),0)));}
# 2586
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp5B0,Cyc_Absyn_uint_exp(0,0));}else{
# 2588
e->r=_tmp5B0->r;goto _LL27C;}}
# 2590
goto _LL27C;_LL27C:;}
# 2592
goto _LL275;_LL27A:;_LL27B: {
# 2594
const char*_tmpC86;void*_tmpC85[2];struct Cyc_String_pa_PrintArg_struct _tmpC84;struct Cyc_String_pa_PrintArg_struct _tmpC83;(_tmpC83.tag=0,((_tmpC83.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpC84.tag=0,((_tmpC84.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpC85[0]=& _tmpC84,((_tmpC85[1]=& _tmpC83,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC86="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpC86,sizeof(char),41))),_tag_dyneither(_tmpC85,sizeof(void*),2)))))))))))));}_LL275:;}
# 2597
break;}case Cyc_Absyn_Plus: _LL274:
# 2602
{void*_tmp5B9=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp5A0))->hd);void*_tmp5BA=_tmp5B9;void*_tmp5BC;union Cyc_Absyn_Constraint*_tmp5BD;union Cyc_Absyn_Constraint*_tmp5BE;_LL283: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5BB=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5BA;if(_tmp5BB->tag != 5)goto _LL285;else{_tmp5BC=(_tmp5BB->f1).elt_typ;_tmp5BD=((_tmp5BB->f1).ptr_atts).bounds;_tmp5BE=((_tmp5BB->f1).ptr_atts).zero_term;}}_LL284:
# 2604
{void*_tmp5BF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5BD);void*_tmp5C0=_tmp5BF;struct Cyc_Absyn_Exp*_tmp5C3;_LL288: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5C1=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5C0;if(_tmp5C1->tag != 0)goto _LL28A;}_LL289: {
# 2606
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp520))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp520->tl))->hd;
{struct Cyc_Absyn_Exp*_tmpC87[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpC87[2]=e2,((_tmpC87[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5BC),0),((_tmpC87[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC87,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}
goto _LL287;}_LL28A: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5C2=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5C0;if(_tmp5C2->tag != 1)goto _LL287;else{_tmp5C3=_tmp5C2->f1;}}_LL28B:
# 2612
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5BE)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp520))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp520->tl))->hd;
struct Cyc_Absyn_Exp*_tmpC88[3];e->r=(Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1),((_tmpC88[2]=e2,((_tmpC88[1]=_tmp5C3,((_tmpC88[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC88,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0))->r;}
# 2617
goto _LL287;_LL287:;}
# 2619
goto _LL282;_LL285:;_LL286:
# 2621
 goto _LL282;_LL282:;}
# 2623
break;case Cyc_Absyn_Minus: _LL281: {
# 2628
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(_tmp5A0))->hd,& elt_typ)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp520))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp520->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(_tmp5A0->tl))->hd)){
e1->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);
e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp);
e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{
# 2638
struct Cyc_Absyn_Exp*_tmpC89[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpC89[2]=
Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0),((_tmpC89[1]=
# 2639
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpC89[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC89,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 2643
break;}case Cyc_Absyn_Eq: _LL28C:
 goto _LL28D;case Cyc_Absyn_Neq: _LL28D:
 goto _LL28E;case Cyc_Absyn_Gt: _LL28E:
 goto _LL28F;case Cyc_Absyn_Gte: _LL28F:
 goto _LL290;case Cyc_Absyn_Lt: _LL290:
 goto _LL291;case Cyc_Absyn_Lte: _LL291: {
# 2651
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp520))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp520->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp5A0))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp5A0->tl))->hd;
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& elt_typ))
e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,0));
if(Cyc_Tcutil_is_tagged_pointer_typ(t2))
e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp,0));
break;}default: _LL292:
 break;}
# 2663
goto _LL219;}_LL222: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp521=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp521->tag != 4)goto _LL224;else{_tmp522=_tmp521->f1;_tmp523=_tmp521->f2;}}_LL223: {
# 2665
void*e2_cyc_typ=(void*)_check_null(_tmp522->topt);
# 2674 "toc.cyc"
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
const char*_tmpC8A;struct _dyneither_ptr incr_str=(_tmpC8A="increment",_tag_dyneither(_tmpC8A,sizeof(char),10));
if(_tmp523 == Cyc_Absyn_PreDec  || _tmp523 == Cyc_Absyn_PostDec){const char*_tmpC8B;incr_str=((_tmpC8B="decrement",_tag_dyneither(_tmpC8B,sizeof(char),10)));}
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp522,& ptr_type,& is_dyneither,& elt_type)){
{const char*_tmpC8F;void*_tmpC8E[1];struct Cyc_String_pa_PrintArg_struct _tmpC8D;(_tmpC8D.tag=0,((_tmpC8D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC8E[0]=& _tmpC8D,Cyc_Tcutil_terr(e->loc,((_tmpC8F="in-place %s is not supported when dereferencing a zero-terminated pointer",_tag_dyneither(_tmpC8F,sizeof(char),74))),_tag_dyneither(_tmpC8E,sizeof(void*),1)))))));}{
const char*_tmpC92;void*_tmpC91;(_tmpC91=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC92="in-place inc/dec on zero-term",_tag_dyneither(_tmpC92,sizeof(char),30))),_tag_dyneither(_tmpC91,sizeof(void*),0)));};}{
# 2683
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
int f_tag=0;
if(Cyc_Toc_is_tagged_union_project(_tmp522,& f_tag,& tunion_member_type,1)){
struct Cyc_Absyn_Exp*_tmp5CD=Cyc_Absyn_signed_int_exp(1,0);
_tmp5CD->topt=Cyc_Absyn_sint_typ;
switch(_tmp523){case Cyc_Absyn_PreInc: _LL294:
# 2690
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpC98;struct Cyc_Core_Opt*_tmpC97;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpC96;e->r=(void*)((_tmpC96=_cycalloc(sizeof(*_tmpC96)),((_tmpC96[0]=((_tmpC98.tag=3,((_tmpC98.f1=_tmp522,((_tmpC98.f2=((_tmpC97=_cycalloc_atomic(sizeof(*_tmpC97)),((_tmpC97->v=(void*)Cyc_Absyn_Plus,_tmpC97)))),((_tmpC98.f3=_tmp5CD,_tmpC98)))))))),_tmpC96))));}
Cyc_Toc_exp_to_c(nv,e);
return;case Cyc_Absyn_PreDec: _LL295:
# 2694
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpC9E;struct Cyc_Core_Opt*_tmpC9D;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpC9C;e->r=(void*)((_tmpC9C=_cycalloc(sizeof(*_tmpC9C)),((_tmpC9C[0]=((_tmpC9E.tag=3,((_tmpC9E.f1=_tmp522,((_tmpC9E.f2=((_tmpC9D=_cycalloc_atomic(sizeof(*_tmpC9D)),((_tmpC9D->v=(void*)Cyc_Absyn_Minus,_tmpC9D)))),((_tmpC9E.f3=_tmp5CD,_tmpC9E)))))))),_tmpC9C))));}
Cyc_Toc_exp_to_c(nv,e);
return;default: _LL296:
# 2698
{const char*_tmpCA2;void*_tmpCA1[1];struct Cyc_String_pa_PrintArg_struct _tmpCA0;(_tmpCA0.tag=0,((_tmpCA0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpCA1[0]=& _tmpCA0,Cyc_Tcutil_terr(e->loc,((_tmpCA2="in-place post-%s is not supported on @tagged union members",_tag_dyneither(_tmpCA2,sizeof(char),59))),_tag_dyneither(_tmpCA1,sizeof(void*),1)))))));}{
# 2700
const char*_tmpCA5;void*_tmpCA4;(_tmpCA4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCA5="in-place inc/dec on @tagged union",_tag_dyneither(_tmpCA5,sizeof(char),34))),_tag_dyneither(_tmpCA4,sizeof(void*),0)));};}}
# 2703
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp522);
Cyc_Toc_set_lhs(nv,0);{
# 2708
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp523 == Cyc_Absyn_PostInc  || _tmp523 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2714
if(_tmp523 == Cyc_Absyn_PreDec  || _tmp523 == Cyc_Absyn_PostDec)
change=-1;{
struct Cyc_Absyn_Exp*_tmpCA6[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpCA6[2]=
# 2718
Cyc_Absyn_signed_int_exp(change,0),((_tmpCA6[1]=
# 2717
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpCA6[0]=
# 2716
Cyc_Toc_push_address_exp(_tmp522),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCA6,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2719
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2723
struct Cyc_Toc_functionSet*_tmp5DA=_tmp523 != Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
# 2725
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp5DA,_tmp522);
struct Cyc_Absyn_Exp*_tmpCA7[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpCA7[1]=
Cyc_Absyn_signed_int_exp(1,0),((_tmpCA7[0]=
# 2726
Cyc_Toc_push_address_exp(_tmp522),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCA7,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
# 2728
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp522)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp522,0,Cyc_Toc_incr_lvalue,_tmp523);
e->r=_tmp522->r;}}}
# 2732
goto _LL219;};};}_LL224: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp524=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp524->tag != 3)goto _LL226;else{_tmp525=_tmp524->f1;_tmp526=_tmp524->f2;_tmp527=_tmp524->f3;}}_LL225: {
# 2751 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp525->topt);
void*e2_old_typ=(void*)_check_null(_tmp527->topt);
int f_tag=0;
void*tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Toc_is_tagged_union_project(_tmp525,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp525);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp527);
e->r=Cyc_Toc_tagged_union_assignop(_tmp525,e1_old_typ,_tmp526,_tmp527,e2_old_typ,f_tag,tagged_member_struct_type);
# 2762
return;}{
# 2764
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp525,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp525,_tmp526,_tmp527,ptr_type,is_dyneither,elt_type);
# 2770
return;}{
# 2774
int e1_poly=Cyc_Toc_is_poly_project(_tmp525);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp525);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp527);{
# 2780
int done=0;
if(_tmp526 != 0){
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp5DD=(enum Cyc_Absyn_Primop)_tmp526->v;switch(_tmp5DD){case Cyc_Absyn_Plus: _LL298:
 change=_tmp527;break;case Cyc_Absyn_Minus: _LL299:
# 2788
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp527,0);break;default: _LL29A: {
const char*_tmpCAA;void*_tmpCA9;(_tmpCA9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCAA="bad t ? pointer arithmetic",_tag_dyneither(_tmpCAA,sizeof(char),27))),_tag_dyneither(_tmpCA9,sizeof(void*),0)));}}}
# 2791
done=1;{
# 2793
struct Cyc_Absyn_Exp*_tmp5E0=Cyc_Toc__dyneither_ptr_inplace_plus_e;
struct Cyc_Absyn_Exp*_tmpCAB[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5E0,((_tmpCAB[2]=change,((_tmpCAB[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpCAB[0]=
# 2794
Cyc_Toc_push_address_exp(_tmp525),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCAB,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2797
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2800
enum Cyc_Absyn_Primop _tmp5E2=(enum Cyc_Absyn_Primop)_tmp526->v;switch(_tmp5E2){case Cyc_Absyn_Plus: _LL29C:
# 2802
 done=1;
{struct Cyc_Absyn_Exp*_tmpCAC[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp525),((_tmpCAC[1]=_tmp527,((_tmpCAC[0]=_tmp525,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCAC,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
break;default: _LL29D: {
const char*_tmpCAF;void*_tmpCAE;(_tmpCAE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCAF="bad zero-terminated pointer arithmetic",_tag_dyneither(_tmpCAF,sizeof(char),39))),_tag_dyneither(_tmpCAE,sizeof(void*),0)));}}}}}
# 2809
if(!done){
# 2811
if(e1_poly)
_tmp527->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp527->r,0));
# 2817
if(!Cyc_Absyn_is_lvalue(_tmp525)){
{struct _tuple27 _tmpCB2;struct _tuple27*_tmpCB1;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple27*),struct _tuple27*f_env))Cyc_Toc_lvalue_assign)(_tmp525,0,Cyc_Toc_assignop_lvalue,((_tmpCB1=_cycalloc(sizeof(struct _tuple27)* 1),((_tmpCB1[0]=(struct _tuple27)((_tmpCB2.f1=_tmp526,((_tmpCB2.f2=_tmp527,_tmpCB2)))),_tmpCB1)))));}
e->r=_tmp525->r;}}
# 2822
goto _LL219;};};};}_LL226: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp528=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp528->tag != 5)goto _LL228;else{_tmp529=_tmp528->f1;_tmp52A=_tmp528->f2;_tmp52B=_tmp528->f3;}}_LL227:
# 2824
 Cyc_Toc_exp_to_c(nv,_tmp529);
Cyc_Toc_exp_to_c(nv,_tmp52A);
Cyc_Toc_exp_to_c(nv,_tmp52B);
goto _LL219;_LL228: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp52C=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp52C->tag != 6)goto _LL22A;else{_tmp52D=_tmp52C->f1;_tmp52E=_tmp52C->f2;}}_LL229:
# 2829
 Cyc_Toc_exp_to_c(nv,_tmp52D);
Cyc_Toc_exp_to_c(nv,_tmp52E);
goto _LL219;_LL22A: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp52F=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp52F->tag != 7)goto _LL22C;else{_tmp530=_tmp52F->f1;_tmp531=_tmp52F->f2;}}_LL22B:
# 2833
 Cyc_Toc_exp_to_c(nv,_tmp530);
Cyc_Toc_exp_to_c(nv,_tmp531);
goto _LL219;_LL22C: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp532=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp532->tag != 8)goto _LL22E;else{_tmp533=_tmp532->f1;_tmp534=_tmp532->f2;}}_LL22D:
# 2837
 Cyc_Toc_exp_to_c(nv,_tmp533);
Cyc_Toc_exp_to_c(nv,_tmp534);
goto _LL219;_LL22E: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp535=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp535->tag != 9)goto _LL230;else{_tmp536=_tmp535->f1;_tmp537=_tmp535->f2;if(_tmp535->f3 != 0)goto _LL230;}}_LL22F:
# 2841
 Cyc_Toc_exp_to_c(nv,_tmp536);
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp537);
goto _LL219;_LL230: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp538=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp538->tag != 9)goto _LL232;else{_tmp539=_tmp538->f1;_tmp53A=_tmp538->f2;if(_tmp538->f3 == 0)goto _LL232;_tmp53B=(_tmp538->f3)->num_varargs;_tmp53C=(_tmp538->f3)->injectors;_tmp53D=(_tmp538->f3)->vai;}}_LL231: {
# 2853 "toc.cyc"
struct _RegionHandle _tmp5E8=_new_region("r");struct _RegionHandle*r=& _tmp5E8;_push_region(r);{
struct _tuple1*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp53B,0);
void*cva_type=Cyc_Toc_typ_to_c(_tmp53D->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_conref,0);
# 2861
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp53A);
int num_normargs=num_args - _tmp53B;
# 2865
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp53A=_tmp53A->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp53A))->hd);{
struct Cyc_List_List*_tmpCB3;new_args=((_tmpCB3=_cycalloc(sizeof(*_tmpCB3)),((_tmpCB3->hd=(struct Cyc_Absyn_Exp*)_tmp53A->hd,((_tmpCB3->tl=new_args,_tmpCB3))))));};}}
# 2870
{struct Cyc_Absyn_Exp*_tmpCB6[3];struct Cyc_List_List*_tmpCB5;new_args=((_tmpCB5=_cycalloc(sizeof(*_tmpCB5)),((_tmpCB5->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpCB6[2]=num_varargs_exp,((_tmpCB6[1]=
Cyc_Absyn_sizeoftyp_exp(cva_type,0),((_tmpCB6[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCB6,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpCB5->tl=new_args,_tmpCB5))))));}
# 2875
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);
# 2877
Cyc_Toc_exp_to_c(nv,_tmp539);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp539,new_args,0),0);
# 2881
if(_tmp53D->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp5EC=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp53D->type));void*_tmp5ED=_tmp5EC;struct Cyc_Absyn_Datatypedecl*_tmp5EF;_LL2A0: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5EE=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5ED;if(_tmp5EE->tag != 3)goto _LL2A2;else{if((((_tmp5EE->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2A2;_tmp5EF=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp5EE->f1).datatype_info).KnownDatatype).val);}}_LL2A1:
 tud=_tmp5EF;goto _LL29F;_LL2A2:;_LL2A3: {
const char*_tmpCB9;void*_tmpCB8;(_tmpCB8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCB9="toc: unknown datatype in vararg with inject",_tag_dyneither(_tmpCB9,sizeof(char),44))),_tag_dyneither(_tmpCB8,sizeof(void*),0)));}_LL29F:;}{
# 2887
unsigned int _tmpCBE;unsigned int _tmpCBD;struct _dyneither_ptr _tmpCBC;struct _tuple1**_tmpCBB;unsigned int _tmpCBA;struct _dyneither_ptr vs=(_tmpCBA=(unsigned int)_tmp53B,((_tmpCBB=(struct _tuple1**)_region_malloc(r,_check_times(sizeof(struct _tuple1*),_tmpCBA)),((_tmpCBC=_tag_dyneither(_tmpCBB,sizeof(struct _tuple1*),_tmpCBA),((((_tmpCBD=_tmpCBA,_tmpCBF(& _tmpCBE,& _tmpCBD,& _tmpCBB))),_tmpCBC)))))));
# 2889
if(_tmp53B != 0){
# 2891
struct Cyc_List_List*_tmp5F2=0;
{int i=_tmp53B - 1;for(0;i >= 0;-- i){
struct Cyc_List_List*_tmpCC0;_tmp5F2=((_tmpCC0=_cycalloc(sizeof(*_tmpCC0)),((_tmpCC0->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0),0)),((_tmpCC0->tl=_tmp5F2,_tmpCC0))))));}}
# 2895
s=Cyc_Absyn_declare_stmt(argv,arr_type,Cyc_Absyn_unresolvedmem_exp(0,_tmp5F2,0),s,0);{
# 2898
int i=0;for(0;_tmp53A != 0;(((_tmp53A=_tmp53A->tl,_tmp53C=_tmp53C->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp53A->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0);
struct Cyc_Absyn_Datatypefield*_tmp5F4=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp53C))->hd;struct _tuple1*_tmp5F6;struct Cyc_List_List*_tmp5F7;struct Cyc_Absyn_Datatypefield*_tmp5F5=_tmp5F4;_tmp5F6=_tmp5F5->name;_tmp5F7=_tmp5F5->typs;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp5F7))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);
# 2913
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);
# 2916
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Toc_tag_sp,0),
Cyc_Toc_datatype_tag(tud,_tmp5F6),0),s,0);
# 2919
s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp5F6,tud->name)),0,s,0);};}};}else{
# 2926
struct _tuple19*_tmpCC1[3];struct Cyc_List_List*_tmp5F8=(_tmpCC1[2]=
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpCC1[1]=
# 2926
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpCC1[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCC1,sizeof(struct _tuple19*),3)))))));
# 2928
s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),Cyc_Absyn_uint_exp(0,0),s,0);}};}else{
# 2938
{int i=0;for(0;_tmp53A != 0;(_tmp53A=_tmp53A->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp53A->hd);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp53A->hd,0),s,0);}}
# 2944
s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}
# 2957 "toc.cyc"
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2959
_npop_handler(0);goto _LL219;
# 2853 "toc.cyc"
;_pop_region(r);}_LL232: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp53E=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp53E->tag != 10)goto _LL234;else{_tmp53F=_tmp53E->f1;_tmp540=_tmp53E->f2;}}_LL233:
# 2962 "toc.cyc"
 Cyc_Toc_exp_to_c(nv,_tmp53F);{
struct Cyc_Absyn_Exp*_tmp5FE=_tmp540?Cyc_Toc_newrethrow_exp(_tmp53F): Cyc_Toc_newthrow_exp(_tmp53F);
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),_tmp5FE,0))->r;
goto _LL219;};_LL234: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp541=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp541->tag != 11)goto _LL236;else{_tmp542=_tmp541->f1;}}_LL235:
 Cyc_Toc_exp_to_c(nv,_tmp542);goto _LL219;_LL236: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp543=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp543->tag != 12)goto _LL238;else{_tmp544=_tmp543->f1;_tmp545=_tmp543->f2;}}_LL237:
# 2968
 Cyc_Toc_exp_to_c(nv,_tmp544);
# 2977 "toc.cyc"
for(0;_tmp545 != 0;_tmp545=_tmp545->tl){
enum Cyc_Absyn_KindQual _tmp5FF=(Cyc_Tcutil_typ_kind((void*)_tmp545->hd))->kind;
if(_tmp5FF != Cyc_Absyn_EffKind  && _tmp5FF != Cyc_Absyn_RgnKind){
{void*_tmp600=Cyc_Tcutil_compress((void*)_tmp545->hd);void*_tmp601=_tmp600;_LL2A5: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp602=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp601;if(_tmp602->tag != 2)goto _LL2A7;}_LL2A6:
 goto _LL2A8;_LL2A7: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp603=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp601;if(_tmp603->tag != 3)goto _LL2A9;}_LL2A8:
 continue;_LL2A9:;_LL2AA:
# 2984
 e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp544,0))->r;
goto _LL2A4;_LL2A4:;}
# 2987
break;}}
# 2990
goto _LL219;_LL238: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp546=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp546->tag != 13)goto _LL23A;else{_tmp547=(void**)((void**)& _tmp546->f1);_tmp548=_tmp546->f2;_tmp549=_tmp546->f3;_tmp54A=_tmp546->f4;}}_LL239: {
# 2992
void*old_t2=(void*)_check_null(_tmp548->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp547;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp547=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp548);
# 2999
{struct _tuple29 _tmpCC2;struct _tuple29 _tmp604=(_tmpCC2.f1=Cyc_Tcutil_compress(old_t2),((_tmpCC2.f2=Cyc_Tcutil_compress(new_typ),_tmpCC2)));struct _tuple29 _tmp605=_tmp604;struct Cyc_Absyn_PtrInfo _tmp607;struct Cyc_Absyn_PtrInfo _tmp609;struct Cyc_Absyn_PtrInfo _tmp60B;_LL2AC:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp606=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp605.f1;if(_tmp606->tag != 5)goto _LL2AE;else{_tmp607=_tmp606->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp608=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp605.f2;if(_tmp608->tag != 5)goto _LL2AE;else{_tmp609=_tmp608->f1;}};_LL2AD: {
# 3001
int _tmp60D=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp607.ptr_atts).nullable);
int _tmp60E=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp609.ptr_atts).nullable);
void*_tmp60F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp607.ptr_atts).bounds);
void*_tmp610=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp609.ptr_atts).bounds);
int _tmp611=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp607.ptr_atts).zero_term);
int _tmp612=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp609.ptr_atts).zero_term);
{struct _tuple29 _tmpCC3;struct _tuple29 _tmp613=(_tmpCC3.f1=_tmp60F,((_tmpCC3.f2=_tmp610,_tmpCC3)));struct _tuple29 _tmp614=_tmp613;struct Cyc_Absyn_Exp*_tmp616;struct Cyc_Absyn_Exp*_tmp618;struct Cyc_Absyn_Exp*_tmp61A;struct Cyc_Absyn_Exp*_tmp61E;_LL2B3:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp615=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp614.f1;if(_tmp615->tag != 1)goto _LL2B5;else{_tmp616=_tmp615->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp617=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp614.f2;if(_tmp617->tag != 1)goto _LL2B5;else{_tmp618=_tmp617->f1;}};_LL2B4:
# 3009
 if((!Cyc_Evexp_c_can_eval(_tmp616) || !Cyc_Evexp_c_can_eval(_tmp618)) && !
Cyc_Evexp_same_const_exp(_tmp616,_tmp618)){
const char*_tmpCC6;void*_tmpCC5;(_tmpCC5=0,Cyc_Tcutil_terr(e->loc,((_tmpCC6="can't validate cast due to potential size differences",_tag_dyneither(_tmpCC6,sizeof(char),54))),_tag_dyneither(_tmpCC5,sizeof(void*),0)));}
if(_tmp60D  && !_tmp60E){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpCC9;void*_tmpCC8;(_tmpCC8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCC9="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpCC9,sizeof(char),44))),_tag_dyneither(_tmpCC8,sizeof(void*),0)));}
# 3018
if(_tmp54A != Cyc_Absyn_NonNull_to_Null){
const char*_tmpCCD;void*_tmpCCC[1];struct Cyc_String_pa_PrintArg_struct _tmpCCB;(_tmpCCB.tag=0,((_tmpCCB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpCCC[0]=& _tmpCCB,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCCD="null-check conversion mis-classified: %s",_tag_dyneither(_tmpCCD,sizeof(char),41))),_tag_dyneither(_tmpCCC,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp548);
if(do_null_check){
if(!_tmp549){
const char*_tmpCD0;void*_tmpCCF;(_tmpCCF=0,Cyc_Tcutil_warn(e->loc,((_tmpCD0="inserted null check due to implicit cast from * to @ type",_tag_dyneither(_tmpCD0,sizeof(char),58))),_tag_dyneither(_tmpCCF,sizeof(void*),0)));}{
# 3025
struct Cyc_List_List*_tmpCD1;e->r=Cyc_Toc_cast_it_r(*_tmp547,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpCD1=_cycalloc(sizeof(*_tmpCD1)),((_tmpCD1->hd=_tmp548,((_tmpCD1->tl=0,_tmpCD1)))))),0));};}else{
# 3029
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp548->r;}};}else{
# 3034
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp548->r;}
# 3041
goto _LL2B2;_LL2B5:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp619=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp614.f1;if(_tmp619->tag != 1)goto _LL2B7;else{_tmp61A=_tmp619->f1;}}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp61B=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp614.f2;if(_tmp61B->tag != 0)goto _LL2B7;};_LL2B6:
# 3043
 if(!Cyc_Evexp_c_can_eval(_tmp61A)){
const char*_tmpCD4;void*_tmpCD3;(_tmpCD3=0,Cyc_Tcutil_terr(e->loc,((_tmpCD4="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpCD4,sizeof(char),71))),_tag_dyneither(_tmpCD3,sizeof(void*),0)));}
# 3046
if(_tmp54A == Cyc_Absyn_NonNull_to_Null){
const char*_tmpCD8;void*_tmpCD7[1];struct Cyc_String_pa_PrintArg_struct _tmpCD6;(_tmpCD6.tag=0,((_tmpCD6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpCD7[0]=& _tmpCD6,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCD8="conversion mis-classified as null-check: %s",_tag_dyneither(_tmpCD8,sizeof(char),44))),_tag_dyneither(_tmpCD7,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){
# 3050
if((_tmp611  && !(_tmp609.elt_tq).real_const) && !_tmp612)
# 3053
_tmp61A=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp61A,Cyc_Absyn_uint_exp(1,0),0);
# 3055
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp61A,_tmp548))->r;}else{
# 3057
struct Cyc_Absyn_Exp*_tmp630=Cyc_Toc__tag_dyneither_e;
# 3059
if(_tmp611){
# 3064
struct _tuple1*_tmp631=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp632=Cyc_Absyn_var_exp(_tmp631,0);
struct Cyc_Absyn_Exp*arg3;
# 3069
{void*_tmp633=_tmp548->r;void*_tmp634=_tmp633;_LL2BC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp635=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp634;if(_tmp635->tag != 0)goto _LL2BE;else{if(((_tmp635->f1).String_c).tag != 8)goto _LL2BE;}}_LL2BD:
# 3071
 arg3=_tmp61A;goto _LL2BB;_LL2BE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp636=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp634;if(_tmp636->tag != 0)goto _LL2C0;else{if(((_tmp636->f1).Wstring_c).tag != 9)goto _LL2C0;}}_LL2BF:
# 3073
 arg3=_tmp61A;goto _LL2BB;_LL2C0:;_LL2C1:
# 3075
{struct Cyc_Absyn_Exp*_tmpCD9[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp548),(
# 3077
(_tmpCD9[1]=_tmp61A,((_tmpCD9[0]=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp632),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCD9,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
goto _LL2BB;_LL2BB:;}
# 3080
if(!_tmp612  && !(_tmp609.elt_tq).real_const)
# 3083
arg3=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{
# 3085
struct Cyc_Absyn_Exp*_tmp638=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp609.elt_typ),0);
struct Cyc_Absyn_Exp*_tmpCDA[3];struct Cyc_Absyn_Exp*_tmp639=Cyc_Absyn_fncall_exp(_tmp630,((_tmpCDA[2]=arg3,((_tmpCDA[1]=_tmp638,((_tmpCDA[0]=_tmp632,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCDA,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
struct Cyc_Absyn_Stmt*_tmp63A=Cyc_Absyn_exp_stmt(_tmp639,0);
_tmp63A=Cyc_Absyn_declare_stmt(_tmp631,Cyc_Toc_typ_to_c(old_t2),_tmp548,_tmp63A,0);
e->r=Cyc_Toc_stmt_exp_r(_tmp63A);};}else{
# 3092
struct Cyc_Absyn_Exp*_tmpCDB[3];e->r=Cyc_Toc_fncall_exp_r(_tmp630,(
(_tmpCDB[2]=_tmp61A,((_tmpCDB[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp609.elt_typ),0),((_tmpCDB[0]=_tmp548,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCDB,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 3098
goto _LL2B2;_LL2B7:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp61C=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp614.f1;if(_tmp61C->tag != 0)goto _LL2B9;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp61D=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp614.f2;if(_tmp61D->tag != 1)goto _LL2B9;else{_tmp61E=_tmp61D->f1;}};_LL2B8:
# 3100
 if(!Cyc_Evexp_c_can_eval(_tmp61E)){
const char*_tmpCDE;void*_tmpCDD;(_tmpCDD=0,Cyc_Tcutil_terr(e->loc,((_tmpCDE="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpCDE,sizeof(char),71))),_tag_dyneither(_tmpCDD,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpCE1;void*_tmpCE0;(_tmpCE0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCE1="can't coerce t? to t* or t@ at the top-level",_tag_dyneither(_tmpCE1,sizeof(char),45))),_tag_dyneither(_tmpCE0,sizeof(void*),0)));}{
# 3112 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp641=_tmp61E;
if(_tmp611  && !_tmp612)
_tmp641=Cyc_Absyn_add_exp(_tmp61E,Cyc_Absyn_uint_exp(1,0),0);{
# 3119
struct Cyc_Absyn_Exp*_tmp642=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpCE2[3];struct Cyc_Absyn_Exp*_tmp643=Cyc_Absyn_fncall_exp(_tmp642,(
(_tmpCE2[2]=_tmp641,((_tmpCE2[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp607.elt_typ),0),((_tmpCE2[0]=_tmp548,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCE2,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 3124
if(_tmp60E){
struct Cyc_List_List*_tmpCE3;_tmp643->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,(
(_tmpCE3=_cycalloc(sizeof(*_tmpCE3)),((_tmpCE3->hd=Cyc_Absyn_copy_exp(_tmp643),((_tmpCE3->tl=0,_tmpCE3)))))));}
e->r=Cyc_Toc_cast_it_r(*_tmp547,_tmp643);
goto _LL2B2;};};_LL2B9:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp61F=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp614.f1;if(_tmp61F->tag != 0)goto _LL2B2;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp620=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp614.f2;if(_tmp620->tag != 0)goto _LL2B2;};_LL2BA:
# 3132
 DynCast:
 if((_tmp611  && !_tmp612) && !(_tmp609.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpCE6;void*_tmpCE5;(_tmpCE5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCE6="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",_tag_dyneither(_tmpCE6,sizeof(char),70))),_tag_dyneither(_tmpCE5,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp648=Cyc_Toc__dyneither_ptr_decrease_size_e;
struct Cyc_Absyn_Exp*_tmpCE7[3];e->r=Cyc_Toc_fncall_exp_r(_tmp648,(
(_tmpCE7[2]=
Cyc_Absyn_uint_exp(1,0),((_tmpCE7[1]=
# 3138
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp607.elt_typ),0),((_tmpCE7[0]=_tmp548,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCE7,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}
# 3141
goto _LL2B2;_LL2B2:;}
# 3143
goto _LL2AB;}_LL2AE:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp60A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp605.f1;if(_tmp60A->tag != 5)goto _LL2B0;else{_tmp60B=_tmp60A->f1;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp60C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp605.f2;if(_tmp60C->tag != 6)goto _LL2B0;};_LL2AF:
# 3145
{void*_tmp64B=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp60B.ptr_atts).bounds);void*_tmp64C=_tmp64B;_LL2C3: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp64D=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp64C;if(_tmp64D->tag != 0)goto _LL2C5;}_LL2C4:
# 3147
 _tmp548->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp548->r,_tmp548->loc),Cyc_Toc_curr_sp);goto _LL2C2;_LL2C5:;_LL2C6:
 goto _LL2C2;_LL2C2:;}
# 3150
goto _LL2AB;_LL2B0:;_LL2B1:
# 3152
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp548->r;
goto _LL2AB;_LL2AB:;}
# 3156
goto _LL219;}_LL23A: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp54B=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp54B->tag != 14)goto _LL23C;else{_tmp54C=_tmp54B->f1;}}_LL23B:
# 3160
{void*_tmp64F=_tmp54C->r;void*_tmp650=_tmp64F;struct _tuple1*_tmp652;struct Cyc_List_List*_tmp653;struct Cyc_List_List*_tmp654;struct Cyc_List_List*_tmp656;_LL2C8: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp651=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp650;if(_tmp651->tag != 28)goto _LL2CA;else{_tmp652=_tmp651->f1;_tmp653=_tmp651->f2;_tmp654=_tmp651->f3;}}_LL2C9:
# 3162
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpCEB;void*_tmpCEA[1];struct Cyc_String_pa_PrintArg_struct _tmpCE9;(_tmpCE9.tag=0,((_tmpCE9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp54C->loc)),((_tmpCEA[0]=& _tmpCE9,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCEB="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpCEB,sizeof(char),42))),_tag_dyneither(_tmpCEA,sizeof(void*),1)))))));}{
struct Cyc_Absyn_Exp*_tmp65A=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp54C->topt),_tmp653,1,0,_tmp654,_tmp652);
e->r=_tmp65A->r;
e->topt=_tmp65A->topt;
goto _LL2C7;};_LL2CA: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp655=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp650;if(_tmp655->tag != 23)goto _LL2CC;else{_tmp656=_tmp655->f1;}}_LL2CB:
# 3170
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpCEF;void*_tmpCEE[1];struct Cyc_String_pa_PrintArg_struct _tmpCED;(_tmpCED.tag=0,((_tmpCED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp54C->loc)),((_tmpCEE[0]=& _tmpCED,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCEF="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpCEF,sizeof(char),42))),_tag_dyneither(_tmpCEE,sizeof(void*),1)))))));}
e->r=(Cyc_Toc_init_tuple(nv,1,0,_tmp656))->r;
goto _LL2C7;_LL2CC:;_LL2CD:
# 3176
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp54C);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp54C)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp54C,0,Cyc_Toc_address_lvalue,1);
# 3182
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp54C);}else{
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind((void*)_check_null(_tmp54C->topt))))
# 3186
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp54C);}
# 3188
goto _LL2C7;_LL2C7:;}
# 3190
goto _LL219;_LL23C: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp54D=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp54D->tag != 15)goto _LL23E;else{_tmp54E=_tmp54D->f1;_tmp54F=_tmp54D->f2;}}_LL23D:
# 3193
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpCF3;void*_tmpCF2[1];struct Cyc_String_pa_PrintArg_struct _tmpCF1;(_tmpCF1.tag=0,((_tmpCF1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp54F->loc)),((_tmpCF2[0]=& _tmpCF1,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCF3="%s: new at top-level",_tag_dyneither(_tmpCF3,sizeof(char),21))),_tag_dyneither(_tmpCF2,sizeof(void*),1)))))));}{
void*new_e_type=(void*)_check_null(_tmp54F->topt);
{void*_tmp661=_tmp54F->r;void*_tmp662=_tmp661;struct Cyc_List_List*_tmp664;struct Cyc_Absyn_Vardecl*_tmp666;struct Cyc_Absyn_Exp*_tmp667;struct Cyc_Absyn_Exp*_tmp668;int _tmp669;struct Cyc_Absyn_Exp*_tmp66B;void*_tmp66C;int _tmp66D;struct _tuple1*_tmp66F;struct Cyc_List_List*_tmp670;struct Cyc_List_List*_tmp671;struct Cyc_Absyn_Aggrdecl*_tmp672;struct Cyc_List_List*_tmp674;_LL2CF: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp663=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp662;if(_tmp663->tag != 25)goto _LL2D1;else{_tmp664=_tmp663->f1;}}_LL2D0: {
# 3201
struct _tuple1*_tmp675=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp676=Cyc_Absyn_var_exp(_tmp675,0);
struct Cyc_Absyn_Stmt*_tmp677=Cyc_Toc_init_array(nv,new_e_type,_tmp676,_tmp664,Cyc_Absyn_exp_stmt(_tmp676,0));
void*old_elt_typ;
{void*_tmp678=Cyc_Tcutil_compress(old_typ);void*_tmp679=_tmp678;void*_tmp67B;struct Cyc_Absyn_Tqual _tmp67C;union Cyc_Absyn_Constraint*_tmp67D;_LL2DC: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp67A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp679;if(_tmp67A->tag != 5)goto _LL2DE;else{_tmp67B=(_tmp67A->f1).elt_typ;_tmp67C=(_tmp67A->f1).elt_tq;_tmp67D=((_tmp67A->f1).ptr_atts).zero_term;}}_LL2DD:
# 3207
 old_elt_typ=_tmp67B;goto _LL2DB;_LL2DE:;_LL2DF: {
# 3209
const char*_tmpCF6;void*_tmpCF5;old_elt_typ=(
(_tmpCF5=0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCF6="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(_tmpCF6,sizeof(char),52))),_tag_dyneither(_tmpCF5,sizeof(void*),0))));}_LL2DB:;}{
# 3212
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*_tmp680=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp681=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),
Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp664),0),0);
struct Cyc_Absyn_Exp*e1;
if(_tmp54E == 0  || Cyc_Absyn_no_regions)
e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp681);else{
# 3220
struct Cyc_Absyn_Exp*r=_tmp54E;
Cyc_Toc_exp_to_c(nv,r);
e1=Cyc_Toc_rmalloc_exp(r,_tmp681);}
# 3225
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp675,_tmp680,e1,_tmp677,0));
goto _LL2CE;};}_LL2D1: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp665=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp662;if(_tmp665->tag != 26)goto _LL2D3;else{_tmp666=_tmp665->f1;_tmp667=_tmp665->f2;_tmp668=_tmp665->f3;_tmp669=_tmp665->f4;}}_LL2D2:
# 3229
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp54E,old_typ,_tmp667,(void*)_check_null(_tmp668->topt),_tmp669,_tmp668,_tmp666);
goto _LL2CE;_LL2D3: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp66A=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp662;if(_tmp66A->tag != 27)goto _LL2D5;else{_tmp66B=_tmp66A->f1;_tmp66C=(void*)_tmp66A->f2;_tmp66D=_tmp66A->f3;}}_LL2D4:
# 3233
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp54E,old_typ,_tmp66B,_tmp66C,_tmp66D,0,0);
goto _LL2CE;_LL2D5: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp66E=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp662;if(_tmp66E->tag != 28)goto _LL2D7;else{_tmp66F=_tmp66E->f1;_tmp670=_tmp66E->f2;_tmp671=_tmp66E->f3;_tmp672=_tmp66E->f4;}}_LL2D6: {
# 3238
struct Cyc_Absyn_Exp*_tmp682=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp54F->topt),_tmp670,1,_tmp54E,_tmp671,_tmp66F);
e->r=_tmp682->r;
e->topt=_tmp682->topt;
goto _LL2CE;}_LL2D7: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp673=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp662;if(_tmp673->tag != 23)goto _LL2D9;else{_tmp674=_tmp673->f1;}}_LL2D8:
# 3244
 e->r=(Cyc_Toc_init_tuple(nv,1,_tmp54E,_tmp674))->r;
goto _LL2CE;_LL2D9:;_LL2DA: {
# 3251
void*old_elt_typ=(void*)_check_null(_tmp54F->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
# 3254
struct _tuple1*_tmp683=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp684=Cyc_Absyn_var_exp(_tmp683,0);
struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp684,0),0);
struct Cyc_Absyn_Exp*inner_mexp=mexp;
if(_tmp54E == 0  || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
# 3261
struct Cyc_Absyn_Exp*r=_tmp54E;
Cyc_Toc_exp_to_c(nv,r);
mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{
# 3269
int done=0;
{void*_tmp685=_tmp54F->r;void*_tmp686=_tmp685;void*_tmp688;struct Cyc_Absyn_Exp*_tmp689;_LL2E1: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp687=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp686;if(_tmp687->tag != 13)goto _LL2E3;else{_tmp688=(void*)_tmp687->f1;_tmp689=_tmp687->f2;}}_LL2E2:
# 3272
{struct _tuple29 _tmpCF7;struct _tuple29 _tmp68A=(_tmpCF7.f1=Cyc_Tcutil_compress(_tmp688),((_tmpCF7.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp689->topt)),_tmpCF7)));struct _tuple29 _tmp68B=_tmp68A;void*_tmp68D;union Cyc_Absyn_Constraint*_tmp68E;union Cyc_Absyn_Constraint*_tmp690;_LL2E6:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp68C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp68B.f1;if(_tmp68C->tag != 5)goto _LL2E8;else{_tmp68D=(_tmp68C->f1).elt_typ;_tmp68E=((_tmp68C->f1).ptr_atts).bounds;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp68F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp68B.f2;if(_tmp68F->tag != 5)goto _LL2E8;else{_tmp690=((_tmp68F->f1).ptr_atts).bounds;}};_LL2E7:
# 3275
{struct _tuple29 _tmpCF8;struct _tuple29 _tmp691=(_tmpCF8.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp68E),((_tmpCF8.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp690),_tmpCF8)));struct _tuple29 _tmp692=_tmp691;struct Cyc_Absyn_Exp*_tmp695;_LL2EB:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp693=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp692.f1;if(_tmp693->tag != 0)goto _LL2ED;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp694=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp692.f2;if(_tmp694->tag != 1)goto _LL2ED;else{_tmp695=_tmp694->f1;}};_LL2EC:
# 3277
 Cyc_Toc_exp_to_c(nv,_tmp689);
inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);
done=1;{
struct Cyc_Absyn_Exp*_tmp696=Cyc_Toc__init_dyneither_ptr_e;
{struct Cyc_Absyn_Exp*_tmpCF9[4];e->r=Cyc_Toc_fncall_exp_r(_tmp696,(
(_tmpCF9[3]=_tmp695,((_tmpCF9[2]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp68D),0),((_tmpCF9[1]=_tmp689,((_tmpCF9[0]=mexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCF9,sizeof(struct Cyc_Absyn_Exp*),4)))))))))));}
# 3285
goto _LL2EA;};_LL2ED:;_LL2EE:
 goto _LL2EA;_LL2EA:;}
# 3288
goto _LL2E5;_LL2E8:;_LL2E9:
 goto _LL2E5;_LL2E5:;}
# 3291
goto _LL2E0;_LL2E3:;_LL2E4:
 goto _LL2E0;_LL2E0:;}
# 3294
if(!done){
struct Cyc_Absyn_Stmt*_tmp69A=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp684),0);
struct Cyc_Absyn_Exp*_tmp69B=Cyc_Absyn_signed_int_exp(0,0);
Cyc_Toc_exp_to_c(nv,_tmp54F);
_tmp69A=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp684,_tmp69B,0),_tmp54F,0),_tmp69A,0);{
# 3300
void*_tmp69C=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp683,_tmp69C,mexp,_tmp69A,0));};}
# 3303
goto _LL2CE;};}_LL2CE:;}
# 3305
goto _LL219;};_LL23E: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp550=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp550->tag != 17)goto _LL240;else{_tmp551=_tmp550->f1;}}_LL23F: {
# 3308
int _tmp69D=Cyc_Toc_set_in_sizeof(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp551);
Cyc_Toc_set_in_sizeof(nv,_tmp69D);
goto _LL219;}_LL240: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp552=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp552->tag != 16)goto _LL242;else{_tmp553=(void*)_tmp552->f1;}}_LL241:
{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpCFC;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpCFB;e->r=(void*)((_tmpCFB=_cycalloc(sizeof(*_tmpCFB)),((_tmpCFB[0]=((_tmpCFC.tag=16,((_tmpCFC.f1=Cyc_Toc_typ_to_c(_tmp553),_tmpCFC)))),_tmpCFB))));}goto _LL219;_LL242: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp554=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp554->tag != 18)goto _LL244;else{_tmp555=(void*)_tmp554->f1;_tmp556=_tmp554->f2;}}_LL243: {
# 3314
void*_tmp6A0=_tmp555;
struct Cyc_List_List*_tmp6A1=_tmp556;
for(0;_tmp6A1 != 0;_tmp6A1=_tmp6A1->tl){
void*_tmp6A2=(void*)_tmp6A1->hd;void*_tmp6A3=_tmp6A2;struct _dyneither_ptr*_tmp6A5;unsigned int _tmp6A7;_LL2F0: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp6A4=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp6A3;if(_tmp6A4->tag != 0)goto _LL2F2;else{_tmp6A5=_tmp6A4->f1;}}_LL2F1:
 goto _LL2EF;_LL2F2: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp6A6=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp6A3;if(_tmp6A6->tag != 1)goto _LL2EF;else{_tmp6A7=_tmp6A6->f1;}}_LL2F3:
# 3320
{void*_tmp6A8=Cyc_Tcutil_compress(_tmp6A0);void*_tmp6A9=_tmp6A8;union Cyc_Absyn_AggrInfoU _tmp6AB;struct Cyc_List_List*_tmp6AD;struct Cyc_List_List*_tmp6AF;struct Cyc_Absyn_Datatypefield*_tmp6B1;_LL2F5: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp6AA=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp6A9;if(_tmp6AA->tag != 11)goto _LL2F7;else{_tmp6AB=(_tmp6AA->f1).aggr_info;}}_LL2F6: {
# 3322
struct Cyc_Absyn_Aggrdecl*_tmp6B2=Cyc_Absyn_get_known_aggrdecl(_tmp6AB);
if(_tmp6B2->impl == 0){
const char*_tmpCFF;void*_tmpCFE;(_tmpCFE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCFF="struct fields must be known",_tag_dyneither(_tmpCFF,sizeof(char),28))),_tag_dyneither(_tmpCFE,sizeof(void*),0)));}
_tmp6AD=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6B2->impl))->fields;goto _LL2F8;}_LL2F7: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp6AC=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6A9;if(_tmp6AC->tag != 12)goto _LL2F9;else{_tmp6AD=_tmp6AC->f2;}}_LL2F8: {
# 3327
struct Cyc_Absyn_Aggrfield*_tmp6B5=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp6AD,(int)_tmp6A7);
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpD02;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpD01;_tmp6A1->hd=(void*)((void*)((_tmpD01=_cycalloc(sizeof(*_tmpD01)),((_tmpD01[0]=((_tmpD02.tag=0,((_tmpD02.f1=_tmp6B5->name,_tmpD02)))),_tmpD01)))));}
_tmp6A0=_tmp6B5->type;
goto _LL2F4;}_LL2F9: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6AE=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6A9;if(_tmp6AE->tag != 10)goto _LL2FB;else{_tmp6AF=_tmp6AE->f1;}}_LL2FA:
# 3332
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpD05;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpD04;_tmp6A1->hd=(void*)((void*)((_tmpD04=_cycalloc(sizeof(*_tmpD04)),((_tmpD04[0]=((_tmpD05.tag=0,((_tmpD05.f1=Cyc_Absyn_fieldname((int)(_tmp6A7 + 1)),_tmpD05)))),_tmpD04)))));}
_tmp6A0=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp6AF,(int)_tmp6A7)).f2;
goto _LL2F4;_LL2FB: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp6B0=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp6A9;if(_tmp6B0->tag != 4)goto _LL2FD;else{if((((_tmp6B0->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2FD;_tmp6B1=((struct _tuple2)(((_tmp6B0->f1).field_info).KnownDatatypefield).val).f2;}}_LL2FC:
# 3336
 if(_tmp6A7 == 0){
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpD08;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpD07;_tmp6A1->hd=(void*)((void*)((_tmpD07=_cycalloc(sizeof(*_tmpD07)),((_tmpD07[0]=((_tmpD08.tag=0,((_tmpD08.f1=Cyc_Toc_tag_sp,_tmpD08)))),_tmpD07)))));}else{
# 3339
_tmp6A0=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp6B1->typs,(int)(_tmp6A7 - 1))).f2;{
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpD0B;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpD0A;_tmp6A1->hd=(void*)((void*)((_tmpD0A=_cycalloc(sizeof(*_tmpD0A)),((_tmpD0A[0]=((_tmpD0B.tag=0,((_tmpD0B.f1=Cyc_Absyn_fieldname((int)_tmp6A7),_tmpD0B)))),_tmpD0A)))));};}
# 3342
goto _LL2F4;_LL2FD:;_LL2FE:
{const char*_tmpD0E;void*_tmpD0D;(_tmpD0D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD0E="impossible type for offsetof tuple index",_tag_dyneither(_tmpD0E,sizeof(char),41))),_tag_dyneither(_tmpD0D,sizeof(void*),0)));}
goto _LL2F4;_LL2F4:;}
# 3346
goto _LL2EF;_LL2EF:;}
# 3349
{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpD11;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpD10;e->r=(void*)((_tmpD10=_cycalloc(sizeof(*_tmpD10)),((_tmpD10[0]=((_tmpD11.tag=18,((_tmpD11.f1=Cyc_Toc_typ_to_c(_tmp555),((_tmpD11.f2=_tmp556,_tmpD11)))))),_tmpD10))));}
goto _LL219;}_LL244: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp557=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp557->tag != 19)goto _LL246;else{_tmp558=_tmp557->f1;}}_LL245: {
# 3352
int _tmp6C2=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp6C3=Cyc_Tcutil_compress((void*)_check_null(_tmp558->topt));
{void*_tmp6C4=_tmp6C3;void*_tmp6C6;struct Cyc_Absyn_Tqual _tmp6C7;void*_tmp6C8;union Cyc_Absyn_Constraint*_tmp6C9;union Cyc_Absyn_Constraint*_tmp6CA;union Cyc_Absyn_Constraint*_tmp6CB;_LL300: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6C5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6C4;if(_tmp6C5->tag != 5)goto _LL302;else{_tmp6C6=(_tmp6C5->f1).elt_typ;_tmp6C7=(_tmp6C5->f1).elt_tq;_tmp6C8=((_tmp6C5->f1).ptr_atts).rgn;_tmp6C9=((_tmp6C5->f1).ptr_atts).nullable;_tmp6CA=((_tmp6C5->f1).ptr_atts).bounds;_tmp6CB=((_tmp6C5->f1).ptr_atts).zero_term;}}_LL301:
# 3357
{void*_tmp6CC=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6CA);void*_tmp6CD=_tmp6CC;struct Cyc_Absyn_Exp*_tmp6CF;_LL305: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6CE=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6CD;if(_tmp6CE->tag != 1)goto _LL307;else{_tmp6CF=_tmp6CE->f1;}}_LL306: {
# 3359
int do_null_check=!Cyc_Toc_in_sizeof(nv) && Cyc_Toc_need_null_check(_tmp558);
Cyc_Toc_exp_to_c(nv,_tmp558);
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpD14;void*_tmpD13;(_tmpD13=0,Cyc_Tcutil_warn(e->loc,((_tmpD14="inserted null check due to dereference",_tag_dyneither(_tmpD14,sizeof(char),39))),_tag_dyneither(_tmpD13,sizeof(void*),0)));}{
# 3365
struct Cyc_List_List*_tmpD15;_tmp558->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(_tmp6C3),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpD15=_cycalloc(sizeof(*_tmpD15)),((_tmpD15->hd=Cyc_Absyn_copy_exp(_tmp558),((_tmpD15->tl=0,_tmpD15)))))),0));};}
# 3373
if(!Cyc_Toc_in_sizeof(nv) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6CB)){
struct _tuple12 _tmp6D4=Cyc_Evexp_eval_const_uint_exp(_tmp6CF);unsigned int _tmp6D6;int _tmp6D7;struct _tuple12 _tmp6D5=_tmp6D4;_tmp6D6=_tmp6D5.f1;_tmp6D7=_tmp6D5.f2;
# 3379
if(!_tmp6D7  || _tmp6D6 <= 0){
const char*_tmpD18;void*_tmpD17;(_tmpD17=0,Cyc_Tcutil_terr(e->loc,((_tmpD18="cannot determine dereference is in bounds",_tag_dyneither(_tmpD18,sizeof(char),42))),_tag_dyneither(_tmpD17,sizeof(void*),0)));}}
# 3382
goto _LL304;}_LL307: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6D0=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6CD;if(_tmp6D0->tag != 0)goto _LL304;}_LL308: {
# 3385
struct Cyc_Absyn_Exp*_tmp6DA=Cyc_Absyn_uint_exp(0,0);
_tmp6DA->topt=Cyc_Absyn_uint_typ;
e->r=Cyc_Toc_subscript_exp_r(_tmp558,_tmp6DA);
Cyc_Toc_exp_to_c(nv,e);
goto _LL304;}_LL304:;}
# 3391
goto _LL2FF;_LL302:;_LL303: {
const char*_tmpD1B;void*_tmpD1A;(_tmpD1A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD1B="exp_to_c: Deref: non-pointer",_tag_dyneither(_tmpD1B,sizeof(char),29))),_tag_dyneither(_tmpD1A,sizeof(void*),0)));}_LL2FF:;}
# 3394
Cyc_Toc_set_lhs(nv,_tmp6C2);
goto _LL219;};}_LL246: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp559=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp559->tag != 20)goto _LL248;else{_tmp55A=_tmp559->f1;_tmp55B=_tmp559->f2;_tmp55C=_tmp559->f3;_tmp55D=_tmp559->f4;}}_LL247: {
# 3397
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp55A->topt);
Cyc_Toc_exp_to_c(nv,_tmp55A);
if(_tmp55C  && _tmp55D)
e->r=Cyc_Toc_check_tagged_union(_tmp55A,Cyc_Toc_typ_to_c(e1_cyc_type),e1_cyc_type,_tmp55B,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);
# 3405
if(is_poly)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
goto _LL219;}_LL248: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp55E=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp55E->tag != 21)goto _LL24A;else{_tmp55F=_tmp55E->f1;_tmp560=_tmp55E->f2;_tmp561=_tmp55E->f3;_tmp562=_tmp55E->f4;}}_LL249: {
# 3409
int _tmp6DD=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp55F->topt));
int do_null_check=Cyc_Toc_need_null_check(_tmp55F);
Cyc_Toc_exp_to_c(nv,_tmp55F);{
int is_poly=Cyc_Toc_is_poly_project(e);
struct Cyc_Absyn_PtrInfo _tmp6DE=Cyc_Toc_get_ptr_type(e1typ);void*_tmp6E0;struct Cyc_Absyn_Tqual _tmp6E1;void*_tmp6E2;union Cyc_Absyn_Constraint*_tmp6E3;union Cyc_Absyn_Constraint*_tmp6E4;union Cyc_Absyn_Constraint*_tmp6E5;struct Cyc_Absyn_PtrInfo _tmp6DF=_tmp6DE;_tmp6E0=_tmp6DF.elt_typ;_tmp6E1=_tmp6DF.elt_tq;_tmp6E2=(_tmp6DF.ptr_atts).rgn;_tmp6E3=(_tmp6DF.ptr_atts).nullable;_tmp6E4=(_tmp6DF.ptr_atts).bounds;_tmp6E5=(_tmp6DF.ptr_atts).zero_term;
{void*_tmp6E6=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6E4);void*_tmp6E7=_tmp6E6;struct Cyc_Absyn_Exp*_tmp6E9;_LL30A: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6E8=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6E7;if(_tmp6E8->tag != 1)goto _LL30C;else{_tmp6E9=_tmp6E8->f1;}}_LL30B: {
# 3420
struct _tuple12 _tmp6EB=Cyc_Evexp_eval_const_uint_exp(_tmp6E9);unsigned int _tmp6ED;int _tmp6EE;struct _tuple12 _tmp6EC=_tmp6EB;_tmp6ED=_tmp6EC.f1;_tmp6EE=_tmp6EC.f2;
if(_tmp6EE){
if(_tmp6ED < 1){
const char*_tmpD1E;void*_tmpD1D;(_tmpD1D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD1E="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpD1E,sizeof(char),44))),_tag_dyneither(_tmpD1D,sizeof(void*),0)));}
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpD21;void*_tmpD20;(_tmpD20=0,Cyc_Tcutil_warn(e->loc,((_tmpD21="inserted null check due to dereference",_tag_dyneither(_tmpD21,sizeof(char),39))),_tag_dyneither(_tmpD20,sizeof(void*),0)));}{
# 3428
struct Cyc_Absyn_Exp*_tmpD22[1];_tmp55F->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpD22[0]=Cyc_Absyn_new_exp(_tmp55F->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD22,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}}else{
# 3433
if(!Cyc_Evexp_c_can_eval(_tmp6E9)){
const char*_tmpD25;void*_tmpD24;(_tmpD24=0,Cyc_Tcutil_terr(e->loc,((_tmpD25="cannot determine pointer dereference in bounds",_tag_dyneither(_tmpD25,sizeof(char),47))),_tag_dyneither(_tmpD24,sizeof(void*),0)));}{
# 3437
struct Cyc_Absyn_Exp*_tmpD26[4];_tmp55F->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpD26[3]=
# 3441
Cyc_Absyn_uint_exp(0,0),((_tmpD26[2]=
# 3440
Cyc_Absyn_sizeoftyp_exp(_tmp6E0,0),((_tmpD26[1]=_tmp6E9,((_tmpD26[0]=
# 3439
Cyc_Absyn_new_exp(_tmp55F->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD26,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0));};}
# 3443
goto _LL309;}_LL30C: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6EA=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6E7;if(_tmp6EA->tag != 0)goto _LL309;}_LL30D: {
# 3446
void*ta1=Cyc_Toc_typ_to_c(_tmp6E0);
{struct Cyc_Absyn_Exp*_tmpD27[3];_tmp55F->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp6E1),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_dyneither_subscript_e,(
(_tmpD27[2]=
# 3451
Cyc_Absyn_uint_exp(0,0),((_tmpD27[1]=
# 3450
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpD27[0]=
# 3449
Cyc_Absyn_new_exp(_tmp55F->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD27,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}
# 3454
goto _LL309;}_LL309:;}
# 3456
if(_tmp561  && _tmp562)
e->r=Cyc_Toc_check_tagged_union(_tmp55F,Cyc_Toc_typ_to_c(e1typ),_tmp6E0,_tmp560,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);
# 3459
if(is_poly  && _tmp562)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
Cyc_Toc_set_lhs(nv,_tmp6DD);
goto _LL219;};};}_LL24A: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp563=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp563->tag != 22)goto _LL24C;else{_tmp564=_tmp563->f1;_tmp565=_tmp563->f2;}}_LL24B: {
# 3464
int _tmp6F8=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp6F9=Cyc_Tcutil_compress((void*)_check_null(_tmp564->topt));
# 3468
{void*_tmp6FA=_tmp6F9;struct Cyc_List_List*_tmp6FC;void*_tmp6FE;struct Cyc_Absyn_Tqual _tmp6FF;void*_tmp700;union Cyc_Absyn_Constraint*_tmp701;union Cyc_Absyn_Constraint*_tmp702;union Cyc_Absyn_Constraint*_tmp703;_LL30F: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6FB=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6FA;if(_tmp6FB->tag != 10)goto _LL311;else{_tmp6FC=_tmp6FB->f1;}}_LL310:
# 3471
 Cyc_Toc_exp_to_c(nv,_tmp564);
Cyc_Toc_exp_to_c(nv,_tmp565);{
struct _tuple12 _tmp704=Cyc_Evexp_eval_const_uint_exp(_tmp565);unsigned int _tmp706;int _tmp707;struct _tuple12 _tmp705=_tmp704;_tmp706=_tmp705.f1;_tmp707=_tmp705.f2;
if(!_tmp707){
const char*_tmpD2A;void*_tmpD29;(_tmpD29=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD2A="unknown tuple subscript in translation to C",_tag_dyneither(_tmpD2A,sizeof(char),44))),_tag_dyneither(_tmpD29,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp564,Cyc_Absyn_fieldname((int)(_tmp706 + 1)));
goto _LL30E;};_LL311: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6FD=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6FA;if(_tmp6FD->tag != 5)goto _LL313;else{_tmp6FE=(_tmp6FD->f1).elt_typ;_tmp6FF=(_tmp6FD->f1).elt_tq;_tmp700=((_tmp6FD->f1).ptr_atts).rgn;_tmp701=((_tmp6FD->f1).ptr_atts).nullable;_tmp702=((_tmp6FD->f1).ptr_atts).bounds;_tmp703=((_tmp6FD->f1).ptr_atts).zero_term;}}_LL312: {
# 3479
struct Cyc_List_List*_tmp70A=Cyc_Toc_get_relns(_tmp564);
int _tmp70B=Cyc_Toc_need_null_check(_tmp564);
int _tmp70C=Cyc_Toc_in_sizeof(nv);
# 3487
int in_bnds=_tmp70C;
{void*_tmp70D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp702);void*_tmp70E=_tmp70D;_LL316:;_LL317:
# 3490
 in_bnds=in_bnds  || Cyc_Toc_check_bounds(_tmp6F9,_tmp70A,_tmp564,_tmp565);
if(Cyc_Toc_warn_bounds_checks  && !in_bnds){
const char*_tmpD2E;void*_tmpD2D[1];struct Cyc_String_pa_PrintArg_struct _tmpD2C;(_tmpD2C.tag=0,((_tmpD2C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpD2D[0]=& _tmpD2C,Cyc_Tcutil_warn(e->loc,((_tmpD2E="bounds check necessary for %s",_tag_dyneither(_tmpD2E,sizeof(char),30))),_tag_dyneither(_tmpD2D,sizeof(void*),1)))))));}_LL315:;}
# 3497
Cyc_Toc_exp_to_c(nv,_tmp564);
Cyc_Toc_exp_to_c(nv,_tmp565);
++ Cyc_Toc_total_bounds_checks;
{void*_tmp712=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp702);void*_tmp713=_tmp712;struct Cyc_Absyn_Exp*_tmp715;_LL319: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp714=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp713;if(_tmp714->tag != 1)goto _LL31B;else{_tmp715=_tmp714->f1;}}_LL31A: {
# 3502
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp701) && _tmp70B;
void*ta1=Cyc_Toc_typ_to_c(_tmp6FE);
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp6FF);
if(in_bnds  && !possibly_null)
++ Cyc_Toc_bounds_checks_eliminated;else{
if(in_bnds  && possibly_null){
++ Cyc_Toc_bounds_checks_eliminated;
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpD31;void*_tmpD30;(_tmpD30=0,Cyc_Tcutil_warn(e->loc,((_tmpD31="inserted null check due to dereference",_tag_dyneither(_tmpD31,sizeof(char),39))),_tag_dyneither(_tmpD30,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpD32[1];_tmp564->r=Cyc_Toc_cast_it_r(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpD32[0]=Cyc_Absyn_copy_exp(_tmp564),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD32,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}else{
# 3514
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp703)){
# 3516
if(!Cyc_Evexp_c_can_eval(_tmp715)){
const char*_tmpD35;void*_tmpD34;(_tmpD34=0,Cyc_Tcutil_terr(e->loc,((_tmpD35="cannot determine subscript is in bounds",_tag_dyneither(_tmpD35,sizeof(char),40))),_tag_dyneither(_tmpD34,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp564);
struct Cyc_Absyn_Exp*_tmpD36[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(function_e,(
(_tmpD36[2]=_tmp565,((_tmpD36[1]=_tmp715,((_tmpD36[0]=_tmp564,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD36,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp715)){
const char*_tmpD39;void*_tmpD38;(_tmpD38=0,Cyc_Tcutil_terr(e->loc,((_tmpD39="cannot determine subscript is in bounds",_tag_dyneither(_tmpD39,sizeof(char),40))),_tag_dyneither(_tmpD38,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpD3C;void*_tmpD3B;(_tmpD3B=0,Cyc_Tcutil_warn(e->loc,((_tmpD3C="inserted null check due to dereference",_tag_dyneither(_tmpD3C,sizeof(char),39))),_tag_dyneither(_tmpD3B,sizeof(void*),0)));}{
# 3528
struct Cyc_Absyn_Exp*_tmpD3D[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpD3D[3]=_tmp565,((_tmpD3D[2]=
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpD3D[1]=_tmp715,((_tmpD3D[0]=_tmp564,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD3D,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{
# 3534
if(!Cyc_Evexp_c_can_eval(_tmp715)){
const char*_tmpD40;void*_tmpD3F;(_tmpD3F=0,Cyc_Tcutil_terr(e->loc,((_tmpD40="cannot determine subscript is in bounds",_tag_dyneither(_tmpD40,sizeof(char),40))),_tag_dyneither(_tmpD3F,sizeof(void*),0)));}{
# 3537
struct Cyc_Absyn_Exp*_tmpD41[2];_tmp565->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,(
(_tmpD41[1]=Cyc_Absyn_copy_exp(_tmp565),((_tmpD41[0]=_tmp715,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD41,sizeof(struct Cyc_Absyn_Exp*),2)))))));};}}}}
# 3540
goto _LL318;}_LL31B: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp716=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp713;if(_tmp716->tag != 0)goto _LL318;}_LL31C: {
# 3542
void*ta1=Cyc_Toc_typ_to_c(_tmp6FE);
if(in_bnds){
# 3546
++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6FF),
Cyc_Toc_member_exp(_tmp564,Cyc_Toc_curr_sp,0)),_tmp565);}else{
# 3551
struct Cyc_Absyn_Exp*_tmp725=Cyc_Toc__check_dyneither_subscript_e;
struct Cyc_Absyn_Exp*_tmpD42[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6FF),
Cyc_Absyn_fncall_exp(_tmp725,(
(_tmpD42[2]=_tmp565,((_tmpD42[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpD42[0]=_tmp564,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD42,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}
# 3557
goto _LL318;}_LL318:;}
# 3559
goto _LL30E;}_LL313:;_LL314: {
const char*_tmpD45;void*_tmpD44;(_tmpD44=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD45="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(_tmpD45,sizeof(char),49))),_tag_dyneither(_tmpD44,sizeof(void*),0)));}_LL30E:;}
# 3562
Cyc_Toc_set_lhs(nv,_tmp6F8);
goto _LL219;};}_LL24C: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp566=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp566->tag != 23)goto _LL24E;else{_tmp567=_tmp566->f1;}}_LL24D:
# 3565
 if(!Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp567))->r;else{
# 3570
struct Cyc_List_List*_tmp729=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp567);
void*_tmp72A=Cyc_Toc_add_tuple_type(_tmp729);
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp567 != 0;(_tmp567=_tmp567->tl,i ++)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp567->hd);{
struct _tuple19*_tmpD48;struct Cyc_List_List*_tmpD47;dles=((_tmpD47=_cycalloc(sizeof(*_tmpD47)),((_tmpD47->hd=((_tmpD48=_cycalloc(sizeof(*_tmpD48)),((_tmpD48->f1=0,((_tmpD48->f2=(struct Cyc_Absyn_Exp*)_tmp567->hd,_tmpD48)))))),((_tmpD47->tl=dles,_tmpD47))))));};}}
# 3577
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}
# 3580
goto _LL219;_LL24E: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp568=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp568->tag != 25)goto _LL250;else{_tmp569=_tmp568->f1;}}_LL24F:
# 3584
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp569);
{struct Cyc_List_List*_tmp72D=_tmp569;for(0;_tmp72D != 0;_tmp72D=_tmp72D->tl){
struct _tuple19*_tmp72E=(struct _tuple19*)_tmp72D->hd;struct Cyc_Absyn_Exp*_tmp730;struct _tuple19*_tmp72F=_tmp72E;_tmp730=_tmp72F->f2;
Cyc_Toc_exp_to_c(nv,_tmp730);}}
# 3589
goto _LL219;_LL250: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp56A=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp56A->tag != 26)goto _LL252;else{_tmp56B=_tmp56A->f1;_tmp56C=_tmp56A->f2;_tmp56D=_tmp56A->f3;_tmp56E=_tmp56A->f4;}}_LL251: {
# 3593
struct _tuple12 _tmp731=Cyc_Evexp_eval_const_uint_exp(_tmp56C);unsigned int _tmp733;int _tmp734;struct _tuple12 _tmp732=_tmp731;_tmp733=_tmp732.f1;_tmp734=_tmp732.f2;{
void*_tmp735=Cyc_Toc_typ_to_c((void*)_check_null(_tmp56D->topt));
Cyc_Toc_exp_to_c(nv,_tmp56D);{
struct Cyc_List_List*es=0;
# 3598
if(!Cyc_Toc_is_zero(_tmp56D)){
if(!_tmp734){
const char*_tmpD4B;void*_tmpD4A;(_tmpD4A=0,Cyc_Tcutil_terr(_tmp56C->loc,((_tmpD4B="cannot determine value of constant",_tag_dyneither(_tmpD4B,sizeof(char),35))),_tag_dyneither(_tmpD4A,sizeof(void*),0)));}
{unsigned int i=0;for(0;i < _tmp733;++ i){
struct _tuple19*_tmpD4E;struct Cyc_List_List*_tmpD4D;es=((_tmpD4D=_cycalloc(sizeof(*_tmpD4D)),((_tmpD4D->hd=((_tmpD4E=_cycalloc(sizeof(*_tmpD4E)),((_tmpD4E->f1=0,((_tmpD4E->f2=_tmp56D,_tmpD4E)))))),((_tmpD4D->tl=es,_tmpD4D))))));}}
# 3604
if(_tmp56E){
struct Cyc_Absyn_Exp*_tmp73A=Cyc_Toc_cast_it(_tmp735,Cyc_Absyn_uint_exp(0,0));
struct _tuple19*_tmpD51;struct Cyc_List_List*_tmpD50;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,((_tmpD50=_cycalloc(sizeof(*_tmpD50)),((_tmpD50->hd=((_tmpD51=_cycalloc(sizeof(*_tmpD51)),((_tmpD51->f1=0,((_tmpD51->f2=_tmp73A,_tmpD51)))))),((_tmpD50->tl=0,_tmpD50)))))));}}
# 3609
e->r=Cyc_Toc_unresolvedmem_exp_r(0,es);
goto _LL219;};};}_LL252: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp56F=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp56F->tag != 27)goto _LL254;else{_tmp570=_tmp56F->f1;_tmp571=(void*)_tmp56F->f2;_tmp572=_tmp56F->f3;}}_LL253:
# 3614
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,0);
goto _LL219;_LL254: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp573=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp573->tag != 28)goto _LL256;else{_tmp574=_tmp573->f1;_tmp575=_tmp573->f2;_tmp576=_tmp573->f3;_tmp577=_tmp573->f4;}}_LL255:
# 3619
 if(!Cyc_Toc_is_toplevel(nv)){
struct Cyc_Absyn_Exp*_tmp73D=Cyc_Toc_init_struct(nv,old_typ,_tmp575,0,0,_tmp576,_tmp574);
e->r=_tmp73D->r;
e->topt=_tmp73D->topt;}else{
# 3630
if(_tmp577 == 0){
const char*_tmpD54;void*_tmpD53;(_tmpD53=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD54="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpD54,sizeof(char),38))),_tag_dyneither(_tmpD53,sizeof(void*),0)));}{
struct Cyc_Absyn_Aggrdecl*sd2=_tmp577;
# 3634
struct _RegionHandle _tmp740=_new_region("rgn");struct _RegionHandle*rgn=& _tmp740;_push_region(rgn);
{struct Cyc_List_List*_tmp741=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp576,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
# 3638
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp577->impl))->tagged){
# 3640
struct _tuple30*_tmp742=(struct _tuple30*)((struct Cyc_List_List*)_check_null(_tmp741))->hd;struct Cyc_Absyn_Aggrfield*_tmp744;struct Cyc_Absyn_Exp*_tmp745;struct _tuple30*_tmp743=_tmp742;_tmp744=_tmp743->f1;_tmp745=_tmp743->f2;{
void*_tmp746=(void*)_check_null(_tmp745->topt);
void*_tmp747=_tmp744->type;
Cyc_Toc_exp_to_c(nv,_tmp745);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp747) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp746))
_tmp745->r=
Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp745->r,0));{
# 3649
int i=Cyc_Toc_get_member_offset(_tmp577,_tmp744->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct _tuple19*_tmpD59;struct _tuple19*_tmpD58;struct _tuple19*_tmpD57[2];struct Cyc_List_List*_tmp748=(_tmpD57[1]=((_tmpD58=_cycalloc(sizeof(*_tmpD58)),((_tmpD58->f1=0,((_tmpD58->f2=_tmp745,_tmpD58)))))),((_tmpD57[0]=((_tmpD59=_cycalloc(sizeof(*_tmpD59)),((_tmpD59->f1=0,((_tmpD59->f2=field_tag_exp,_tmpD59)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD57,sizeof(struct _tuple19*),2)))));
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp748,0);
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpD5F;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpD5E;void*_tmpD5D[1];struct Cyc_List_List*ds=(_tmpD5D[0]=(void*)((_tmpD5F=_cycalloc(sizeof(*_tmpD5F)),((_tmpD5F[0]=((_tmpD5E.tag=1,((_tmpD5E.f1=_tmp744->name,_tmpD5E)))),_tmpD5F)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD5D,sizeof(void*),1)));
struct _tuple19*_tmpD62;struct _tuple19*_tmpD61[1];struct Cyc_List_List*dles=(_tmpD61[0]=((_tmpD62=_cycalloc(sizeof(*_tmpD62)),((_tmpD62->f1=ds,((_tmpD62->f2=umem,_tmpD62)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD61,sizeof(struct _tuple19*),1)));
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{
# 3658
struct Cyc_List_List*_tmp751=0;
struct Cyc_List_List*_tmp752=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields;
for(0;_tmp752 != 0;_tmp752=_tmp752->tl){
struct Cyc_List_List*_tmp753=_tmp741;for(0;_tmp753 != 0;_tmp753=_tmp753->tl){
if((*((struct _tuple30*)_tmp753->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp752->hd){
struct _tuple30*_tmp754=(struct _tuple30*)_tmp753->hd;struct Cyc_Absyn_Aggrfield*_tmp756;struct Cyc_Absyn_Exp*_tmp757;struct _tuple30*_tmp755=_tmp754;_tmp756=_tmp755->f1;_tmp757=_tmp755->f2;{
void*_tmp758=Cyc_Toc_typ_to_c(_tmp756->type);
void*_tmp759=Cyc_Toc_typ_to_c((void*)_check_null(_tmp757->topt));
Cyc_Toc_exp_to_c(nv,_tmp757);
# 3668
if(!Cyc_Tcutil_unify(_tmp758,_tmp759)){
# 3670
if(!Cyc_Tcutil_is_arithmetic_type(_tmp758) || !
Cyc_Tcutil_is_arithmetic_type(_tmp759))
_tmp757=Cyc_Toc_cast_it(_tmp758,Cyc_Absyn_copy_exp(_tmp757));}
{struct _tuple19*_tmpD65;struct Cyc_List_List*_tmpD64;_tmp751=((_tmpD64=_cycalloc(sizeof(*_tmpD64)),((_tmpD64->hd=((_tmpD65=_cycalloc(sizeof(*_tmpD65)),((_tmpD65->f1=0,((_tmpD65->f2=_tmp757,_tmpD65)))))),((_tmpD64->tl=_tmp751,_tmpD64))))));}
break;};}}}
# 3677
e->r=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp751));}}
# 3635
;_pop_region(rgn);};}
# 3681
goto _LL219;_LL256: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp578=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp578->tag != 29)goto _LL258;else{_tmp579=(void*)_tmp578->f1;_tmp57A=_tmp578->f2;}}_LL257: {
# 3683
struct Cyc_List_List*fs;
{void*_tmp75C=Cyc_Tcutil_compress(_tmp579);void*_tmp75D=_tmp75C;struct Cyc_List_List*_tmp75F;_LL31E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp75E=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp75D;if(_tmp75E->tag != 12)goto _LL320;else{_tmp75F=_tmp75E->f2;}}_LL31F:
 fs=_tmp75F;goto _LL31D;_LL320:;_LL321: {
const char*_tmpD69;void*_tmpD68[1];struct Cyc_String_pa_PrintArg_struct _tmpD67;(_tmpD67.tag=0,((_tmpD67.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp579)),((_tmpD68[0]=& _tmpD67,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD69="anon struct has type %s",_tag_dyneither(_tmpD69,sizeof(char),24))),_tag_dyneither(_tmpD68,sizeof(void*),1)))))));}_LL31D:;}{
# 3688
struct _RegionHandle _tmp763=_new_region("rgn");struct _RegionHandle*rgn=& _tmp763;_push_region(rgn);{
struct Cyc_List_List*_tmp764=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp57A,Cyc_Absyn_StructA,fs);
for(0;_tmp764 != 0;_tmp764=_tmp764->tl){
struct _tuple30*_tmp765=(struct _tuple30*)_tmp764->hd;struct Cyc_Absyn_Aggrfield*_tmp767;struct Cyc_Absyn_Exp*_tmp768;struct _tuple30*_tmp766=_tmp765;_tmp767=_tmp766->f1;_tmp768=_tmp766->f2;{
void*_tmp769=(void*)_check_null(_tmp768->topt);
void*_tmp76A=_tmp767->type;
Cyc_Toc_exp_to_c(nv,_tmp768);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp76A) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp769))
_tmp768->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp768->r,0));};}
# 3702
e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp57A);}
# 3704
_npop_handler(0);goto _LL219;
# 3688
;_pop_region(rgn);};}_LL258: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp57B=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp57B->tag != 30)goto _LL25A;else{_tmp57C=_tmp57B->f1;_tmp57D=_tmp57B->f2;_tmp57E=_tmp57B->f3;}}_LL259: {
# 3707
void*datatype_ctype;
struct Cyc_Absyn_Exp*tag_exp;
struct _tuple1*_tmp76B=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp76C=Cyc_Absyn_var_exp(_tmp76B,0);
struct Cyc_Absyn_Exp*mem_exp;
datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp57E->name,_tmp57D->name));
tag_exp=_tmp57D->is_extensible?Cyc_Absyn_var_exp(_tmp57E->name,0):
 Cyc_Toc_datatype_tag(_tmp57D,_tmp57E->name);
mem_exp=_tmp76C;{
struct Cyc_List_List*_tmp76D=_tmp57E->typs;
# 3718
if(Cyc_Toc_is_toplevel(nv)){
# 3720
struct Cyc_List_List*dles=0;
for(0;_tmp57C != 0;(_tmp57C=_tmp57C->tl,_tmp76D=_tmp76D->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp57C->hd;
void*_tmp76E=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp76D))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp76E))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct _tuple19*_tmpD6C;struct Cyc_List_List*_tmpD6B;dles=((_tmpD6B=_cycalloc(sizeof(*_tmpD6B)),((_tmpD6B->hd=((_tmpD6C=_cycalloc(sizeof(*_tmpD6C)),((_tmpD6C->f1=0,((_tmpD6C->f2=cur_e,_tmpD6C)))))),((_tmpD6B->tl=dles,_tmpD6B))))));};}
# 3731
{struct _tuple19*_tmpD6F;struct Cyc_List_List*_tmpD6E;dles=((_tmpD6E=_cycalloc(sizeof(*_tmpD6E)),((_tmpD6E->hd=((_tmpD6F=_cycalloc(sizeof(*_tmpD6F)),((_tmpD6F->f1=0,((_tmpD6F->f2=tag_exp,_tmpD6F)))))),((_tmpD6E->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles),_tmpD6E))))));}
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{
# 3737
struct Cyc_List_List*_tmpD70;struct Cyc_List_List*_tmp773=
(_tmpD70=_cycalloc(sizeof(*_tmpD70)),((_tmpD70->hd=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Toc_tag_sp,0),tag_exp,0),((_tmpD70->tl=0,_tmpD70)))));
# 3740
{int i=1;for(0;_tmp57C != 0;(((_tmp57C=_tmp57C->tl,i ++)),_tmp76D=_tmp76D->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp57C->hd;
void*_tmp774=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp76D))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp774))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct Cyc_Absyn_Stmt*_tmp775=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Absyn_fieldname(i),0),cur_e,0);
# 3750
struct Cyc_List_List*_tmpD71;_tmp773=((_tmpD71=_cycalloc(sizeof(*_tmpD71)),((_tmpD71->hd=_tmp775,((_tmpD71->tl=_tmp773,_tmpD71))))));};}}{
# 3752
struct Cyc_Absyn_Stmt*_tmp777=Cyc_Absyn_exp_stmt(_tmp76C,0);
struct Cyc_List_List*_tmpD72;struct Cyc_Absyn_Stmt*_tmp778=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpD72=_cycalloc(sizeof(*_tmpD72)),((_tmpD72->hd=_tmp777,((_tmpD72->tl=_tmp773,_tmpD72))))))),0);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp76B,datatype_ctype,0,_tmp778,0));};}
# 3756
goto _LL219;};}_LL25A: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp57F=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp57F->tag != 31)goto _LL25C;}_LL25B:
# 3758
 goto _LL25D;_LL25C: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp580=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp580->tag != 32)goto _LL25E;}_LL25D:
 goto _LL219;_LL25E: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp581=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp581->tag != 33)goto _LL260;else{_tmp582=(_tmp581->f1).is_calloc;_tmp583=(_tmp581->f1).rgn;_tmp584=(_tmp581->f1).elt_type;_tmp585=(_tmp581->f1).num_elts;_tmp586=(_tmp581->f1).fat_result;_tmp587=(_tmp581->f1).inline_call;}}_LL25F: {
# 3762
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp584)));
Cyc_Toc_exp_to_c(nv,_tmp585);
# 3766
if(_tmp586){
struct _tuple1*_tmp77B=Cyc_Toc_temp_var();
struct _tuple1*_tmp77C=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp582){
xexp=_tmp585;
if(_tmp583 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp583;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp77B,0));}else{
# 3777
pexp=Cyc_Toc_calloc_exp(*_tmp584,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp77B,0));}{
# 3779
struct Cyc_Absyn_Exp*_tmpD73[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpD73[2]=
Cyc_Absyn_var_exp(_tmp77B,0),((_tmpD73[1]=
# 3780
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpD73[0]=Cyc_Absyn_var_exp(_tmp77C,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD73,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{
# 3783
if(_tmp583 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp583;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp587)
pexp=Cyc_Toc_rmalloc_inline_exp(rgn,Cyc_Absyn_var_exp(_tmp77B,0));else{
# 3789
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp77B,0));}}else{
# 3791
pexp=Cyc_Toc_malloc_exp(*_tmp584,Cyc_Absyn_var_exp(_tmp77B,0));}{
# 3793
struct Cyc_Absyn_Exp*_tmpD74[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((_tmpD74[2]=
Cyc_Absyn_var_exp(_tmp77B,0),((_tmpD74[1]=
# 3793
Cyc_Absyn_uint_exp(1,0),((_tmpD74[0]=Cyc_Absyn_var_exp(_tmp77C,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD74,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}{
# 3796
struct Cyc_Absyn_Stmt*_tmp77F=Cyc_Absyn_declare_stmt(_tmp77B,Cyc_Absyn_uint_typ,_tmp585,
Cyc_Absyn_declare_stmt(_tmp77C,Cyc_Absyn_cstar_typ(t_c,Cyc_Toc_mt_tq),pexp,
Cyc_Absyn_exp_stmt(rexp,0),0),0);
e->r=Cyc_Toc_stmt_exp_r(_tmp77F);};}else{
# 3801
if(_tmp582){
if(_tmp583 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp583;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp585))->r;}else{
# 3807
e->r=(Cyc_Toc_calloc_exp(*_tmp584,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp585))->r;}}else{
# 3810
if(_tmp583 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp583;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp587)
e->r=(Cyc_Toc_rmalloc_inline_exp(rgn,_tmp585))->r;else{
# 3816
e->r=(Cyc_Toc_rmalloc_exp(rgn,_tmp585))->r;}}else{
# 3818
e->r=(Cyc_Toc_malloc_exp(*_tmp584,_tmp585))->r;}}}
# 3822
goto _LL219;}_LL260: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp588=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp588->tag != 34)goto _LL262;else{_tmp589=_tmp588->f1;_tmp58A=_tmp588->f2;}}_LL261: {
# 3831
void*e1_old_typ=(void*)_check_null(_tmp589->topt);
void*e2_old_typ=(void*)_check_null(_tmp58A->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ) && !Cyc_Tcutil_is_pointer_type(e1_old_typ)){
const char*_tmpD77;void*_tmpD76;(_tmpD76=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD77="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",_tag_dyneither(_tmpD77,sizeof(char),57))),_tag_dyneither(_tmpD76,sizeof(void*),0)));}{
# 3837
struct Cyc_Absyn_Exp*swap_fn;
if(Cyc_Tcutil_is_dyneither_ptr(e1_old_typ))
swap_fn=Cyc_Toc__swap_dyneither_e;else{
# 3841
swap_fn=Cyc_Toc__swap_word_e;}{
# 3845
int f1_tag=0;
void*tagged_mem_type1=(void*)& Cyc_Absyn_VoidType_val;
int e1_tmem=Cyc_Toc_is_tagged_union_project(_tmp589,& f1_tag,& tagged_mem_type1,1);
int f2_tag=0;
void*tagged_mem_type2=(void*)& Cyc_Absyn_VoidType_val;
int e2_tmem=Cyc_Toc_is_tagged_union_project(_tmp58A,& f2_tag,& tagged_mem_type2,1);
# 3852
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp589);
Cyc_Toc_exp_to_c(nv,_tmp58A);
Cyc_Toc_set_lhs(nv,0);
# 3858
if(e1_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp589,f1_tag,tagged_mem_type1);else{
# 3862
_tmp589=Cyc_Toc_push_address_exp(_tmp589);}
# 3864
if(e2_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp58A,f2_tag,tagged_mem_type2);else{
# 3868
_tmp58A=Cyc_Toc_push_address_exp(_tmp58A);}
# 3870
{struct Cyc_Absyn_Exp*_tmpD78[2];e->r=Cyc_Toc_fncall_exp_r(swap_fn,((_tmpD78[1]=_tmp58A,((_tmpD78[0]=_tmp589,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD78,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
# 3872
goto _LL219;};};}_LL262: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp58B=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp58B->tag != 37)goto _LL264;else{_tmp58C=_tmp58B->f1;_tmp58D=_tmp58B->f2;}}_LL263: {
# 3875
void*_tmp783=Cyc_Tcutil_compress((void*)_check_null(_tmp58C->topt));
Cyc_Toc_exp_to_c(nv,_tmp58C);
{void*_tmp784=_tmp783;struct Cyc_Absyn_Aggrdecl*_tmp786;_LL323: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp785=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp784;if(_tmp785->tag != 11)goto _LL325;else{if((((_tmp785->f1).aggr_info).KnownAggr).tag != 2)goto _LL325;_tmp786=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp785->f1).aggr_info).KnownAggr).val);}}_LL324: {
# 3879
struct Cyc_Absyn_Exp*_tmp787=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp786,_tmp58D),0);
struct Cyc_Absyn_Exp*_tmp788=Cyc_Toc_member_exp(_tmp58C,_tmp58D,0);
struct Cyc_Absyn_Exp*_tmp789=Cyc_Toc_member_exp(_tmp788,Cyc_Toc_tag_sp,0);
e->r=(Cyc_Absyn_eq_exp(_tmp789,_tmp787,0))->r;
goto _LL322;}_LL325:;_LL326: {
const char*_tmpD7B;void*_tmpD7A;(_tmpD7A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD7B="non-aggregate type in tagcheck",_tag_dyneither(_tmpD7B,sizeof(char),31))),_tag_dyneither(_tmpD7A,sizeof(void*),0)));}_LL322:;}
# 3886
goto _LL219;}_LL264: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp58E=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp58E->tag != 36)goto _LL266;else{_tmp58F=_tmp58E->f1;}}_LL265:
 Cyc_Toc_stmt_to_c(nv,_tmp58F);goto _LL219;_LL266: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp590=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp590->tag != 35)goto _LL268;}_LL267: {
const char*_tmpD7E;void*_tmpD7D;(_tmpD7D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD7E="UnresolvedMem",_tag_dyneither(_tmpD7E,sizeof(char),14))),_tag_dyneither(_tmpD7D,sizeof(void*),0)));}_LL268: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp591=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp591->tag != 24)goto _LL26A;}_LL269: {
const char*_tmpD81;void*_tmpD80;(_tmpD80=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpD81="compoundlit",_tag_dyneither(_tmpD81,sizeof(char),12))),_tag_dyneither(_tmpD80,sizeof(void*),0)));}_LL26A: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp592=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp592->tag != 38)goto _LL26C;}_LL26B: {
const char*_tmpD84;void*_tmpD83;(_tmpD83=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD84="valueof(-)",_tag_dyneither(_tmpD84,sizeof(char),11))),_tag_dyneither(_tmpD83,sizeof(void*),0)));}_LL26C: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp593=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp518;if(_tmp593->tag != 39)goto _LL219;}_LL26D: {
const char*_tmpD87;void*_tmpD86;(_tmpD86=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD87="__asm__",_tag_dyneither(_tmpD87,sizeof(char),8))),_tag_dyneither(_tmpD86,sizeof(void*),0)));}_LL219:;};}
# 3923 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){
return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
# 3926
Cyc_Toc_skip_stmt_dl(),0);}
# 3929
static int Cyc_Toc_path_length(struct Cyc_Absyn_Exp*e){
void*_tmp794=e->r;void*_tmp795=_tmp794;struct Cyc_Absyn_Exp*_tmp797;struct Cyc_Absyn_Exp*_tmp799;struct Cyc_Absyn_Exp*_tmp79B;_LL328: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp796=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp795;if(_tmp796->tag != 13)goto _LL32A;else{_tmp797=_tmp796->f2;}}_LL329:
 return Cyc_Toc_path_length(_tmp797);_LL32A: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp798=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp795;if(_tmp798->tag != 19)goto _LL32C;else{_tmp799=_tmp798->f1;}}_LL32B:
 return 1 + Cyc_Toc_path_length(_tmp799);_LL32C: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp79A=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp795;if(_tmp79A->tag != 20)goto _LL32E;else{_tmp79B=_tmp79A->f1;}}_LL32D:
 return Cyc_Toc_path_length(_tmp79B);_LL32E:;_LL32F:
 return 0;_LL327:;}struct _tuple31{struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple32{struct _tuple1*f1;void*f2;};struct _tuple33{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3938
static struct _tuple31 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
# 3943
struct Cyc_Absyn_Stmt*s;
{void*_tmp79C=p->r;void*_tmp79D=_tmp79C;struct _tuple1*_tmp79F;struct _tuple1*_tmp7A1;struct Cyc_Absyn_Pat*_tmp7A2;struct _tuple1*_tmp7A4;struct Cyc_Absyn_Vardecl*_tmp7A7;struct Cyc_Absyn_Pat*_tmp7A8;enum Cyc_Absyn_Sign _tmp7AB;int _tmp7AC;char _tmp7AE;struct _dyneither_ptr _tmp7B0;int _tmp7B1;struct Cyc_Absyn_Enumdecl*_tmp7B3;struct Cyc_Absyn_Enumfield*_tmp7B4;void*_tmp7B6;struct Cyc_Absyn_Enumfield*_tmp7B7;struct Cyc_Absyn_Datatypedecl*_tmp7BA;struct Cyc_Absyn_Datatypefield*_tmp7BB;struct Cyc_List_List*_tmp7BC;struct Cyc_List_List*_tmp7BE;struct Cyc_List_List*_tmp7C0;union Cyc_Absyn_AggrInfoU _tmp7C3;struct Cyc_List_List*_tmp7C4;struct Cyc_Absyn_Pat*_tmp7C6;_LL331: {struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp79E=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp79D;if(_tmp79E->tag != 2)goto _LL333;else{_tmp79F=(_tmp79E->f2)->name;}}_LL332: {
# 3946
struct Cyc_Absyn_Pat*_tmp7CA=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0);
_tmp7CA->topt=p->topt;
_tmp7A1=_tmp79F;_tmp7A2=_tmp7CA;goto _LL334;}_LL333: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp7A0=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp79D;if(_tmp7A0->tag != 1)goto _LL335;else{_tmp7A1=(_tmp7A0->f1)->name;_tmp7A2=_tmp7A0->f2;}}_LL334: {
# 3951
struct _tuple1*v=Cyc_Toc_temp_var();
void*_tmp7CB=(void*)_check_null(_tmp7A2->topt);
{struct _tuple32*_tmpD8A;struct Cyc_List_List*_tmpD89;decls=((_tmpD89=_region_malloc(rgn,sizeof(*_tmpD89)),((_tmpD89->hd=((_tmpD8A=_region_malloc(rgn,sizeof(*_tmpD8A)),((_tmpD8A->f1=v,((_tmpD8A->f2=Cyc_Toc_typ_to_c_array(_tmp7CB),_tmpD8A)))))),((_tmpD89->tl=decls,_tmpD89))))));}{
struct _tuple31 _tmp7CE=Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp7A1,Cyc_Absyn_var_exp(v,0)),rgn,_tmp7CB,
Cyc_Absyn_var_exp(v,0),path,_tmp7A2,fail_stmt,decls);
nv=_tmp7CE.f1;
decls=_tmp7CE.f2;{
struct Cyc_Absyn_Stmt*_tmp7CF=_tmp7CE.f3;
struct Cyc_Absyn_Stmt*_tmp7D0=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(v,0),r,0);
s=Cyc_Absyn_seq_stmt(_tmp7D0,_tmp7CF,0);
goto _LL330;};};}_LL335: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp7A3=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp79D;if(_tmp7A3->tag != 4)goto _LL337;else{_tmp7A4=(_tmp7A3->f2)->name;}}_LL336: {
# 3964
struct _tuple1*_tmp7D1=Cyc_Toc_temp_var();
void*_tmp7D2=(void*)_check_null(p->topt);
{struct _tuple32*_tmpD8D;struct Cyc_List_List*_tmpD8C;decls=((_tmpD8C=_region_malloc(rgn,sizeof(*_tmpD8C)),((_tmpD8C->hd=((_tmpD8D=_region_malloc(rgn,sizeof(*_tmpD8D)),((_tmpD8D->f1=_tmp7D1,((_tmpD8D->f2=Cyc_Toc_typ_to_c_array(_tmp7D2),_tmpD8D)))))),((_tmpD8C->tl=decls,_tmpD8C))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp7A4,Cyc_Absyn_var_exp(_tmp7D1,0));
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7D1,0),r,0);
goto _LL330;}_LL337: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp7A5=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp79D;if(_tmp7A5->tag != 0)goto _LL339;}_LL338:
 s=Cyc_Toc_skip_stmt_dl();goto _LL330;_LL339: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp7A6=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp79D;if(_tmp7A6->tag != 3)goto _LL33B;else{_tmp7A7=_tmp7A6->f1;_tmp7A8=_tmp7A6->f2;}}_LL33A: {
# 3973
struct _tuple1*_tmp7D5=Cyc_Toc_temp_var();
{struct _tuple32*_tmpD90;struct Cyc_List_List*_tmpD8F;decls=((_tmpD8F=_region_malloc(rgn,sizeof(*_tmpD8F)),((_tmpD8F->hd=((_tmpD90=_region_malloc(rgn,sizeof(*_tmpD90)),((_tmpD90->f1=_tmp7D5,((_tmpD90->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),_tmpD90)))))),((_tmpD8F->tl=decls,_tmpD8F))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp7A7->name,Cyc_Absyn_var_exp(_tmp7D5,0));
# 3977
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7D5,0),
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(path)),0);{
struct _tuple31 _tmp7D8=Cyc_Toc_xlate_pat(nv,rgn,t,r,path,_tmp7A8,fail_stmt,decls);
_tmp7D8.f3=Cyc_Absyn_seq_stmt(s,_tmp7D8.f3,0);
return _tmp7D8;};}_LL33B: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp7A9=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp79D;if(_tmp7A9->tag != 9)goto _LL33D;}_LL33C:
# 3984
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL330;_LL33D: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp7AA=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp79D;if(_tmp7AA->tag != 10)goto _LL33F;else{_tmp7AB=_tmp7AA->f1;_tmp7AC=_tmp7AA->f2;}}_LL33E:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(_tmp7AB,_tmp7AC,0),fail_stmt);goto _LL330;_LL33F: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp7AD=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp79D;if(_tmp7AD->tag != 11)goto _LL341;else{_tmp7AE=_tmp7AD->f1;}}_LL340:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp7AE,0),fail_stmt);goto _LL330;_LL341: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp7AF=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp79D;if(_tmp7AF->tag != 12)goto _LL343;else{_tmp7B0=_tmp7AF->f1;_tmp7B1=_tmp7AF->f2;}}_LL342:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp7B0,_tmp7B1,0),fail_stmt);goto _LL330;_LL343: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp7B2=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp79D;if(_tmp7B2->tag != 13)goto _LL345;else{_tmp7B3=_tmp7B2->f1;_tmp7B4=_tmp7B2->f2;}}_LL344:
# 3989
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmpD93;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpD92;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpD92=_cycalloc(sizeof(*_tmpD92)),((_tmpD92[0]=((_tmpD93.tag=31,((_tmpD93.f1=_tmp7B4->name,((_tmpD93.f2=_tmp7B3,((_tmpD93.f3=_tmp7B4,_tmpD93)))))))),_tmpD92)))),0),fail_stmt);}
goto _LL330;_LL345: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp7B5=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp79D;if(_tmp7B5->tag != 14)goto _LL347;else{_tmp7B6=(void*)_tmp7B5->f1;_tmp7B7=_tmp7B5->f2;}}_LL346:
# 3992
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpD96;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpD95;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpD95=_cycalloc(sizeof(*_tmpD95)),((_tmpD95[0]=((_tmpD96.tag=32,((_tmpD96.f1=_tmp7B7->name,((_tmpD96.f2=_tmp7B6,((_tmpD96.f3=_tmp7B7,_tmpD96)))))))),_tmpD95)))),0),fail_stmt);}
goto _LL330;_LL347: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp7B8=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp79D;if(_tmp7B8->tag != 6)goto _LL349;else{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp7B9=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(_tmp7B8->f1)->r;if(_tmp7B9->tag != 8)goto _LL349;else{_tmp7BA=_tmp7B9->f1;_tmp7BB=_tmp7B9->f2;_tmp7BC=_tmp7B9->f3;}}}_LL348:
# 4002
 s=Cyc_Toc_skip_stmt_dl();{
struct _tuple1*_tmp7DD=Cyc_Toc_temp_var();
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(_tmp7BB->name,_tmp7BA->name);
void*_tmp7DE=Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);
int cnt=1;
struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(_tmp7DE,r);
struct Cyc_List_List*_tmp7DF=_tmp7BB->typs;
for(0;_tmp7BC != 0;(((_tmp7BC=_tmp7BC->tl,_tmp7DF=((struct Cyc_List_List*)_check_null(_tmp7DF))->tl)),++ cnt)){
struct Cyc_Absyn_Pat*_tmp7E0=(struct Cyc_Absyn_Pat*)_tmp7BC->hd;
if(_tmp7E0->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp7E1=(*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp7DF))->hd)).f2;
void*_tmp7E2=(void*)_check_null(_tmp7E0->topt);
void*_tmp7E3=Cyc_Toc_typ_to_c_array(_tmp7E2);
struct Cyc_Absyn_Exp*_tmp7E4=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp7DD,0),Cyc_Absyn_fieldname(cnt),0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp7E1)))
_tmp7E4=Cyc_Toc_cast_it(_tmp7E3,_tmp7E4);{
# 4020
struct _tuple31 _tmp7E5=Cyc_Toc_xlate_pat(nv,rgn,_tmp7E2,_tmp7E4,_tmp7E4,_tmp7E0,fail_stmt,decls);
# 4022
nv=_tmp7E5.f1;
decls=_tmp7E5.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp7E5.f3,0);};};}{
# 4026
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp7DD,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*tag_exp=
_tmp7BA->is_extensible?Cyc_Absyn_var_exp(_tmp7BB->name,0): Cyc_Toc_datatype_tag(_tmp7BA,_tmp7BB->name);
struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,tag_exp,0);
s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);
if(Cyc_Toc_is_nullable((void*)_check_null(p->topt)))
# 4034
s=Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp7DD,0),Cyc_Absyn_uint_exp(0,0),0),fail_stmt,s,0);
# 4037
s=Cyc_Absyn_declare_stmt(_tmp7DD,_tmp7DE,rcast,s,0);
goto _LL330;};};_LL349: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp7BD=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp79D;if(_tmp7BD->tag != 8)goto _LL34B;else{_tmp7BE=_tmp7BD->f3;}}_LL34A:
# 4040
 _tmp7C0=_tmp7BE;goto _LL34C;_LL34B: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp7BF=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp79D;if(_tmp7BF->tag != 5)goto _LL34D;else{_tmp7C0=_tmp7BF->f1;}}_LL34C:
# 4048
 s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;
for(0;_tmp7C0 != 0;(_tmp7C0=_tmp7C0->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp7E6=(struct Cyc_Absyn_Pat*)_tmp7C0->hd;
if(_tmp7E6->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp7E7=(void*)_check_null(_tmp7E6->topt);
struct _dyneither_ptr*_tmp7E8=Cyc_Absyn_fieldname(cnt);
struct _tuple31 _tmp7E9=Cyc_Toc_xlate_pat(nv,rgn,_tmp7E7,Cyc_Toc_member_exp(r,_tmp7E8,0),
Cyc_Toc_member_exp(path,_tmp7E8,0),_tmp7E6,fail_stmt,decls);
nv=_tmp7E9.f1;
decls=_tmp7E9.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp7E9.f3,0);};}
# 4062
goto _LL330;};_LL34D: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp7C1=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp79D;if(_tmp7C1->tag != 7)goto _LL34F;else{if(_tmp7C1->f1 != 0)goto _LL34F;}}_LL34E: {
# 4064
const char*_tmpD99;void*_tmpD98;(_tmpD98=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD99="unresolved aggregate pattern!",_tag_dyneither(_tmpD99,sizeof(char),30))),_tag_dyneither(_tmpD98,sizeof(void*),0)));}_LL34F: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp7C2=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp79D;if(_tmp7C2->tag != 7)goto _LL351;else{if(_tmp7C2->f1 == 0)goto _LL351;_tmp7C3=(_tmp7C2->f1)->aggr_info;_tmp7C4=_tmp7C2->f3;}}_LL350: {
# 4066
struct Cyc_Absyn_Aggrdecl*_tmp7EC=Cyc_Absyn_get_known_aggrdecl(_tmp7C3);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7EC->impl))->tagged){
# 4070
struct _tuple33*_tmp7ED=(struct _tuple33*)((struct Cyc_List_List*)_check_null(_tmp7C4))->hd;struct Cyc_List_List*_tmp7EF;struct Cyc_Absyn_Pat*_tmp7F0;struct _tuple33*_tmp7EE=_tmp7ED;_tmp7EF=_tmp7EE->f1;_tmp7F0=_tmp7EE->f2;{
struct _dyneither_ptr*f;
{void*_tmp7F1=(void*)((struct Cyc_List_List*)_check_null(_tmp7EF))->hd;void*_tmp7F2=_tmp7F1;struct _dyneither_ptr*_tmp7F4;_LL35A: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp7F3=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp7F2;if(_tmp7F3->tag != 1)goto _LL35C;else{_tmp7F4=_tmp7F3->f1;}}_LL35B:
 f=_tmp7F4;goto _LL359;_LL35C:;_LL35D: {
const char*_tmpD9C;void*_tmpD9B;(_tmpD9B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD9C="no field name in tagged union pattern",_tag_dyneither(_tmpD9C,sizeof(char),38))),_tag_dyneither(_tmpD9B,sizeof(void*),0)));}_LL359:;}{
# 4077
void*_tmp7F7=(void*)_check_null(_tmp7F0->topt);
void*_tmp7F8=Cyc_Toc_typ_to_c_array(_tmp7F7);
# 4080
struct Cyc_Absyn_Exp*_tmp7F9=Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(path,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*_tmp7FA=Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp7FA=Cyc_Toc_cast_it(_tmp7F8,_tmp7FA);
_tmp7F9=Cyc_Toc_cast_it(_tmp7F8,_tmp7F9);{
struct _tuple31 _tmp7FB=Cyc_Toc_xlate_pat(nv,rgn,_tmp7F7,_tmp7FA,_tmp7F9,_tmp7F0,fail_stmt,decls);
# 4088
nv=_tmp7FB.f1;
decls=_tmp7FB.f2;{
struct Cyc_Absyn_Stmt*_tmp7FC=_tmp7FB.f3;
struct Cyc_Absyn_Stmt*_tmp7FD=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(r,f,0),Cyc_Toc_tag_sp,0),
Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp7EC,f),0),fail_stmt);
# 4096
s=Cyc_Absyn_seq_stmt(_tmp7FD,_tmp7FC,0);};};};};}else{
# 4099
s=Cyc_Toc_skip_stmt_dl();
for(0;_tmp7C4 != 0;_tmp7C4=_tmp7C4->tl){
struct _tuple33*_tmp7FE=(struct _tuple33*)_tmp7C4->hd;
struct Cyc_Absyn_Pat*_tmp7FF=(*_tmp7FE).f2;
if(_tmp7FF->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*f;
{void*_tmp800=(void*)((struct Cyc_List_List*)_check_null((*_tmp7FE).f1))->hd;void*_tmp801=_tmp800;struct _dyneither_ptr*_tmp803;_LL35F: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp802=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp801;if(_tmp802->tag != 1)goto _LL361;else{_tmp803=_tmp802->f1;}}_LL360:
 f=_tmp803;goto _LL35E;_LL361:;_LL362: {
struct Cyc_Toc_Match_error_exn_struct _tmpD9F;struct Cyc_Toc_Match_error_exn_struct*_tmpD9E;(int)_throw((void*)((_tmpD9E=_cycalloc_atomic(sizeof(*_tmpD9E)),((_tmpD9E[0]=((_tmpD9F.tag=Cyc_Toc_Match_error,_tmpD9F)),_tmpD9E)))));}_LL35E:;}{
# 4110
void*_tmp806=(void*)_check_null(_tmp7FF->topt);
void*_tmp807=Cyc_Toc_typ_to_c_array(_tmp806);
struct Cyc_Absyn_Exp*_tmp808=Cyc_Toc_member_exp(r,f,0);
void*_tmp809=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7EC->impl))->fields,f)))->type;
struct Cyc_Absyn_Exp*_tmp80A=Cyc_Toc_member_exp(path,f,0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp809))
_tmp808=Cyc_Toc_cast_it(_tmp807,_tmp808);else{
if(!Cyc_Toc_is_array_type(_tmp809) && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp809))){
# 4120
_tmp808=Cyc_Absyn_deref_exp(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(_tmp807,Cyc_Toc_mt_tq),
Cyc_Absyn_address_exp(_tmp808,0)),0);
# 4123
_tmp80A=Cyc_Toc_push_address_exp(_tmp80A);
_tmp80A=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(_tmp807,Cyc_Toc_mt_tq),_tmp80A);
_tmp80A=Cyc_Absyn_deref_exp(_tmp80A,0);}}{
# 4127
struct _tuple31 _tmp80B=Cyc_Toc_xlate_pat(nv,rgn,_tmp806,_tmp808,_tmp80A,_tmp7FF,fail_stmt,decls);
nv=_tmp80B.f1;
decls=_tmp80B.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp80B.f3,0);};};};}}
# 4133
goto _LL330;}_LL351: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp7C5=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp79D;if(_tmp7C5->tag != 6)goto _LL353;else{_tmp7C6=_tmp7C5->f1;}}_LL352: {
# 4136
void*_tmp80C=(void*)_check_null(_tmp7C6->topt);
# 4139
struct _tuple31 _tmp80D=Cyc_Toc_xlate_pat(nv,rgn,_tmp80C,Cyc_Absyn_deref_exp(r,0),Cyc_Absyn_deref_exp(path,0),_tmp7C6,fail_stmt,decls);
# 4141
nv=_tmp80D.f1;
decls=_tmp80D.f2;{
struct Cyc_Absyn_Stmt*_tmp80E=_tmp80D.f3;
if(Cyc_Toc_is_nullable(t))
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,
# 4147
Cyc_Toc_skip_stmt_dl(),0),_tmp80E,0);else{
# 4149
s=_tmp80E;}
goto _LL330;};}_LL353: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp7C7=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp79D;if(_tmp7C7->tag != 15)goto _LL355;}_LL354: {
const char*_tmpDA2;void*_tmpDA1;(_tmpDA1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDA2="unknownid pat",_tag_dyneither(_tmpDA2,sizeof(char),14))),_tag_dyneither(_tmpDA1,sizeof(void*),0)));}_LL355: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp7C8=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp79D;if(_tmp7C8->tag != 16)goto _LL357;}_LL356: {
const char*_tmpDA5;void*_tmpDA4;(_tmpDA4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDA5="unknowncall pat",_tag_dyneither(_tmpDA5,sizeof(char),16))),_tag_dyneither(_tmpDA4,sizeof(void*),0)));}_LL357: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp7C9=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp79D;if(_tmp7C9->tag != 17)goto _LL330;}_LL358: {
const char*_tmpDA8;void*_tmpDA7;(_tmpDA7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDA8="exp pat",_tag_dyneither(_tmpDA8,sizeof(char),8))),_tag_dyneither(_tmpDA7,sizeof(void*),0)));}_LL330:;}{
# 4155
struct _tuple31 _tmpDA9;return(_tmpDA9.f1=nv,((_tmpDA9.f2=decls,((_tmpDA9.f3=s,_tmpDA9)))));};}struct _tuple34{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};
# 4192 "toc.cyc"
static struct _tuple34*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 4194
struct _tuple34*_tmpDAA;return(_tmpDAA=_region_malloc(r,sizeof(*_tmpDAA)),((_tmpDAA->f1=Cyc_Toc_fresh_label(),((_tmpDAA->f2=Cyc_Toc_fresh_label(),((_tmpDAA->f3=sc,_tmpDAA)))))));}
# 4197
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){
# 4199
Cyc_Toc_exp_to_c(nv,e);{
void*_tmp817=(void*)_check_null(e->topt);
# 4204
int leave_as_switch;
{void*_tmp818=Cyc_Tcutil_compress(_tmp817);void*_tmp819=_tmp818;_LL364: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp81A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp819;if(_tmp81A->tag != 6)goto _LL366;}_LL365:
 goto _LL367;_LL366: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp81B=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp819;if(_tmp81B->tag != 13)goto _LL368;}_LL367:
 leave_as_switch=1;goto _LL363;_LL368:;_LL369:
 leave_as_switch=0;goto _LL363;_LL363:;}
# 4210
{struct Cyc_List_List*_tmp81C=scs;for(0;_tmp81C != 0;_tmp81C=_tmp81C->tl){
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp81C->hd)->pat_vars))->v != 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp81C->hd)->where_clause != 0){
leave_as_switch=0;
break;}}}
# 4215
if(leave_as_switch){
# 4217
struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();
{struct Cyc_List_List*_tmp81D=scs;for(0;_tmp81D != 0;_tmp81D=_tmp81D->tl){
struct Cyc_Absyn_Stmt*_tmp81E=((struct Cyc_Absyn_Switch_clause*)_tmp81D->hd)->body;
((struct Cyc_Absyn_Switch_clause*)_tmp81D->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp81E,0);
next_l=Cyc_Toc_fresh_label();{
struct _RegionHandle _tmp81F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp81F;_push_region(rgn);
Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(rgn,nv,next_l),_tmp81E);;_pop_region(rgn);};}}
# 4226
return;}{
# 4229
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 4234
struct _RegionHandle _tmp820=_new_region("rgn");struct _RegionHandle*rgn=& _tmp820;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp821=Cyc_Toc_share_env(rgn,nv);
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple34*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,rgn,scs);
struct Cyc_List_List*test_stmts=0;
struct Cyc_List_List*nvs=0;
struct Cyc_List_List*decls=0;
# 4245
{struct Cyc_List_List*_tmp822=lscs;for(0;_tmp822 != 0;_tmp822=_tmp822->tl){
struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple34*)_tmp822->hd)).f3;
struct _dyneither_ptr*fail_lab=_tmp822->tl == 0?end_l:(*((struct _tuple34*)((struct Cyc_List_List*)_check_null(_tmp822->tl))->hd)).f1;
struct _tuple31 _tmp823=Cyc_Toc_xlate_pat(_tmp821,rgn,_tmp817,r,path,sc->pattern,
Cyc_Absyn_goto_stmt(fail_lab,0),decls);
# 4248
struct Cyc_Toc_Env*_tmp825;struct Cyc_List_List*_tmp826;struct Cyc_Absyn_Stmt*_tmp827;struct _tuple31 _tmp824=_tmp823;_tmp825=_tmp824.f1;_tmp826=_tmp824.f2;_tmp827=_tmp824.f3;
# 4251
if(sc->where_clause != 0){
struct Cyc_Absyn_Exp*_tmp828=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);
Cyc_Toc_exp_to_c(_tmp825,_tmp828);
_tmp827=Cyc_Absyn_seq_stmt(_tmp827,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp828,0),
Cyc_Absyn_goto_stmt(fail_lab,0),
Cyc_Toc_skip_stmt_dl(),0),0);}
# 4258
decls=_tmp826;
{struct Cyc_List_List*_tmpDAB;nvs=((_tmpDAB=_region_malloc(rgn,sizeof(*_tmpDAB)),((_tmpDAB->hd=_tmp825,((_tmpDAB->tl=nvs,_tmpDAB))))));}{
struct Cyc_List_List*_tmpDAC;test_stmts=((_tmpDAC=_region_malloc(rgn,sizeof(*_tmpDAC)),((_tmpDAC->hd=_tmp827,((_tmpDAC->tl=test_stmts,_tmpDAC))))));};}}
# 4262
nvs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);
test_stmts=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{
struct Cyc_List_List*stmts=0;
# 4267
for(0;lscs != 0;
(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple34*_tmp82B=(struct _tuple34*)lscs->hd;struct _dyneither_ptr*_tmp82D;struct _dyneither_ptr*_tmp82E;struct Cyc_Absyn_Switch_clause*_tmp82F;struct _tuple34*_tmp82C=_tmp82B;_tmp82D=_tmp82C->f1;_tmp82E=_tmp82C->f2;_tmp82F=_tmp82C->f3;{
struct Cyc_Toc_Env*_tmp830=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs))->hd;
struct Cyc_Absyn_Stmt*s=_tmp82F->body;
struct _RegionHandle _tmp831=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp831;_push_region(rgn2);
if(lscs->tl != 0){
struct _tuple34*_tmp832=(struct _tuple34*)((struct Cyc_List_List*)_check_null(lscs->tl))->hd;struct _dyneither_ptr*_tmp834;struct Cyc_Absyn_Switch_clause*_tmp835;struct _tuple34*_tmp833=_tmp832;_tmp834=_tmp833->f2;_tmp835=_tmp833->f3;{
struct Cyc_List_List*_tmp836=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp835->pat_vars))->v)).f1);
_tmp836=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp836);
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(rgn2,_tmp830,end_l,_tmp834,_tmp836,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs->tl))->hd),s);};}else{
# 4282
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn2,_tmp830,end_l),s);}
# 4284
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp82D,(struct Cyc_Absyn_Stmt*)((struct Cyc_List_List*)_check_null(test_stmts))->hd,0),
Cyc_Absyn_label_stmt(_tmp82E,s,0),0);{
struct Cyc_List_List*_tmpDAD;stmts=((_tmpDAD=_region_malloc(rgn,sizeof(*_tmpDAD)),((_tmpDAD->hd=s,((_tmpDAD->tl=stmts,_tmpDAD))))));};
# 4272
;_pop_region(rgn2);};}{
# 4288
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),
Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),0),0);
# 4291
for(decls;decls != 0;decls=decls->tl){
struct _tuple32*_tmp838=(struct _tuple32*)decls->hd;struct _tuple1*_tmp83A;void*_tmp83B;struct _tuple32*_tmp839=_tmp838;_tmp83A=_tmp839->f1;_tmp83B=_tmp839->f2;
res=Cyc_Absyn_declare_stmt(_tmp83A,_tmp83B,0,res,0);}
# 4296
whole_s->r=(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),e,res,0))->r;};};}
# 4235
;_pop_region(rgn);};};}
# 4301
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 4307
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 4309
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
struct Cyc_List_List*_tmpDAE;return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,(
(_tmpDAE=_cycalloc(sizeof(*_tmpDAE)),((_tmpDAE->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0),((_tmpDAE->tl=0,_tmpDAE)))))),0),0);}
# 4313
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt(s->r,0));}struct _tuple35{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 4318
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 4320
while(1){
void*_tmp83D=s->r;void*_tmp83E=_tmp83D;struct Cyc_Absyn_Exp*_tmp841;struct Cyc_Absyn_Stmt*_tmp843;struct Cyc_Absyn_Stmt*_tmp844;struct Cyc_Absyn_Exp*_tmp846;struct Cyc_Absyn_Exp*_tmp848;struct Cyc_Absyn_Stmt*_tmp849;struct Cyc_Absyn_Stmt*_tmp84A;struct Cyc_Absyn_Exp*_tmp84C;struct Cyc_Absyn_Stmt*_tmp84D;struct Cyc_Absyn_Stmt*_tmp84F;struct Cyc_Absyn_Stmt*_tmp851;struct Cyc_Absyn_Stmt*_tmp853;struct Cyc_Absyn_Exp*_tmp855;struct Cyc_Absyn_Exp*_tmp856;struct Cyc_Absyn_Exp*_tmp857;struct Cyc_Absyn_Stmt*_tmp858;struct Cyc_Absyn_Exp*_tmp85A;struct Cyc_List_List*_tmp85B;struct Cyc_List_List*_tmp85D;struct Cyc_Absyn_Switch_clause**_tmp85E;struct Cyc_Absyn_Decl*_tmp860;struct Cyc_Absyn_Stmt*_tmp861;struct _dyneither_ptr*_tmp863;struct Cyc_Absyn_Stmt*_tmp864;struct Cyc_Absyn_Stmt*_tmp866;struct Cyc_Absyn_Exp*_tmp867;struct Cyc_Absyn_Stmt*_tmp869;struct Cyc_List_List*_tmp86A;struct Cyc_Absyn_Exp*_tmp86C;_LL36B: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp83F=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp83E;if(_tmp83F->tag != 0)goto _LL36D;}_LL36C:
# 4323
 return;_LL36D: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp840=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp83E;if(_tmp840->tag != 1)goto _LL36F;else{_tmp841=_tmp840->f1;}}_LL36E:
# 4325
 Cyc_Toc_exp_to_c(nv,_tmp841);
return;_LL36F: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp842=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp83E;if(_tmp842->tag != 2)goto _LL371;else{_tmp843=_tmp842->f1;_tmp844=_tmp842->f2;}}_LL370:
# 4328
 Cyc_Toc_stmt_to_c(nv,_tmp843);
s=_tmp844;
continue;_LL371: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp845=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp83E;if(_tmp845->tag != 3)goto _LL373;else{_tmp846=_tmp845->f1;}}_LL372: {
# 4332
void*topt=0;
if(_tmp846 != 0){
topt=Cyc_Toc_typ_to_c((void*)_check_null(_tmp846->topt));
Cyc_Toc_exp_to_c(nv,_tmp846);}
# 4338
if(s->try_depth > 0){
if(topt != 0){
struct _tuple1*_tmp86D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp86E=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp86D,0),0);
s->r=(Cyc_Absyn_declare_stmt(_tmp86D,topt,_tmp846,
Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(s->try_depth),_tmp86E,0),0))->r;}else{
# 4346
Cyc_Toc_do_npop_before(s->try_depth,s);}}
# 4348
return;}_LL373: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp847=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp83E;if(_tmp847->tag != 4)goto _LL375;else{_tmp848=_tmp847->f1;_tmp849=_tmp847->f2;_tmp84A=_tmp847->f3;}}_LL374:
# 4350
 Cyc_Toc_exp_to_c(nv,_tmp848);
Cyc_Toc_stmt_to_c(nv,_tmp849);
s=_tmp84A;
continue;_LL375: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp84B=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp83E;if(_tmp84B->tag != 5)goto _LL377;else{_tmp84C=(_tmp84B->f1).f1;_tmp84D=_tmp84B->f2;}}_LL376:
# 4355
 Cyc_Toc_exp_to_c(nv,_tmp84C);{
struct _RegionHandle _tmp86F=_new_region("temp");struct _RegionHandle*temp=& _tmp86F;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp84D);
# 4359
_npop_handler(0);return;
# 4356
;_pop_region(temp);};_LL377: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp84E=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp83E;if(_tmp84E->tag != 6)goto _LL379;else{_tmp84F=_tmp84E->f1;}}_LL378: {
# 4361
struct _dyneither_ptr**_tmp871;struct Cyc_Toc_Env*_tmp870=nv;_tmp871=_tmp870->break_lab;
if(_tmp871 != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp871,0);{
# 4365
int dest_depth=_tmp84F == 0?0: _tmp84F->try_depth;
Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);
return;};}_LL379: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp850=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp83E;if(_tmp850->tag != 7)goto _LL37B;else{_tmp851=_tmp850->f1;}}_LL37A: {
# 4369
struct _dyneither_ptr**_tmp873;struct Cyc_Toc_Env*_tmp872=nv;_tmp873=_tmp872->continue_lab;
if(_tmp873 != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp873,0);
_tmp853=_tmp851;goto _LL37C;}_LL37B: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp852=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp83E;if(_tmp852->tag != 8)goto _LL37D;else{_tmp853=_tmp852->f2;}}_LL37C:
# 4375
 Cyc_Toc_do_npop_before(s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp853))->try_depth,s);
return;_LL37D: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp854=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp83E;if(_tmp854->tag != 9)goto _LL37F;else{_tmp855=_tmp854->f1;_tmp856=(_tmp854->f2).f1;_tmp857=(_tmp854->f3).f1;_tmp858=_tmp854->f4;}}_LL37E:
# 4379
 Cyc_Toc_exp_to_c(nv,_tmp855);Cyc_Toc_exp_to_c(nv,_tmp856);Cyc_Toc_exp_to_c(nv,_tmp857);{
struct _RegionHandle _tmp874=_new_region("temp");struct _RegionHandle*temp=& _tmp874;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp858);
# 4383
_npop_handler(0);return;
# 4380
;_pop_region(temp);};_LL37F: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp859=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp83E;if(_tmp859->tag != 10)goto _LL381;else{_tmp85A=_tmp859->f1;_tmp85B=_tmp859->f2;}}_LL380:
# 4385
 Cyc_Toc_xlate_switch(nv,s,_tmp85A,_tmp85B);
return;_LL381: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp85C=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp83E;if(_tmp85C->tag != 11)goto _LL383;else{_tmp85D=_tmp85C->f1;_tmp85E=_tmp85C->f2;}}_LL382: {
# 4388
struct Cyc_Toc_FallthruInfo*_tmp876;struct Cyc_Toc_Env*_tmp875=nv;_tmp876=_tmp875->fallthru_info;
if(_tmp876 == 0){
const char*_tmpDB1;void*_tmpDB0;(_tmpDB0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDB1="fallthru in unexpected place",_tag_dyneither(_tmpDB1,sizeof(char),29))),_tag_dyneither(_tmpDB0,sizeof(void*),0)));}{
struct Cyc_Toc_FallthruInfo _tmp879=*_tmp876;struct _dyneither_ptr*_tmp87B;struct Cyc_List_List*_tmp87C;struct Cyc_Dict_Dict _tmp87D;struct Cyc_Toc_FallthruInfo _tmp87A=_tmp879;_tmp87B=_tmp87A.label;_tmp87C=_tmp87A.binders;_tmp87D=_tmp87A.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp87B,0);
# 4394
Cyc_Toc_do_npop_before(s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp85E)))->body)->try_depth,s2);{
struct Cyc_List_List*_tmp87E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp87C);
struct Cyc_List_List*_tmp87F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp85D);
for(0;_tmp87E != 0;(_tmp87E=_tmp87E->tl,_tmp87F=_tmp87F->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp87F))->hd);
s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp87D,(struct _tuple1*)_tmp87E->hd),(struct Cyc_Absyn_Exp*)_tmp87F->hd,0),s2,0);}
# 4402
s->r=s2->r;
return;};};};}_LL383: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp85F=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp83E;if(_tmp85F->tag != 12)goto _LL385;else{_tmp860=_tmp85F->f1;_tmp861=_tmp85F->f2;}}_LL384:
# 4408
{void*_tmp880=_tmp860->r;void*_tmp881=_tmp880;struct Cyc_Absyn_Vardecl*_tmp883;struct Cyc_Absyn_Pat*_tmp885;struct Cyc_Absyn_Exp*_tmp886;struct Cyc_List_List*_tmp888;struct Cyc_Absyn_Fndecl*_tmp88A;struct Cyc_Absyn_Tvar*_tmp88C;struct Cyc_Absyn_Vardecl*_tmp88D;int _tmp88E;struct Cyc_Absyn_Exp*_tmp88F;_LL38E: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp882=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp881;if(_tmp882->tag != 0)goto _LL390;else{_tmp883=_tmp882->f1;}}_LL38F: {
# 4410
struct _RegionHandle _tmp890=_new_region("temp");struct _RegionHandle*temp=& _tmp890;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpDB4;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpDB3;struct Cyc_Toc_Env*_tmp891=Cyc_Toc_add_varmap(temp,nv,_tmp883->name,
Cyc_Absyn_varb_exp(_tmp883->name,(void*)((_tmpDB3=_cycalloc(sizeof(*_tmpDB3)),((_tmpDB3[0]=((_tmpDB4.tag=4,((_tmpDB4.f1=_tmp883,_tmpDB4)))),_tmpDB3)))),0));
Cyc_Toc_local_decl_to_c(_tmp891,_tmp891,_tmp883,_tmp861);}
# 4415
_npop_handler(0);goto _LL38D;
# 4410
;_pop_region(temp);}_LL390: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp884=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp881;if(_tmp884->tag != 2)goto _LL392;else{_tmp885=_tmp884->f1;_tmp886=_tmp884->f3;}}_LL391:
# 4419
{void*_tmp894=_tmp885->r;void*_tmp895=_tmp894;struct Cyc_Absyn_Vardecl*_tmp897;_LL39B: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp896=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp895;if(_tmp896->tag != 1)goto _LL39D;else{_tmp897=_tmp896->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp898=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp896->f2)->r;if(_tmp898->tag != 0)goto _LL39D;};}}_LL39C: {
# 4421
struct _tuple1*old_name=_tmp897->name;
struct _tuple1*new_name=Cyc_Toc_temp_var();
_tmp897->name=new_name;
_tmp897->initializer=_tmp886;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpDB7;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpDB6;_tmp860->r=(void*)((_tmpDB6=_cycalloc(sizeof(*_tmpDB6)),((_tmpDB6[0]=((_tmpDB7.tag=0,((_tmpDB7.f1=_tmp897,_tmpDB7)))),_tmpDB6))));}{
struct _RegionHandle _tmp89B=_new_region("temp");struct _RegionHandle*temp=& _tmp89B;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpDBA;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpDB9;struct Cyc_Toc_Env*_tmp89C=
Cyc_Toc_add_varmap(temp,nv,old_name,
Cyc_Absyn_varb_exp(new_name,(void*)((_tmpDB9=_cycalloc(sizeof(*_tmpDB9)),((_tmpDB9[0]=((_tmpDBA.tag=4,((_tmpDBA.f1=_tmp897,_tmpDBA)))),_tmpDB9)))),0));
Cyc_Toc_local_decl_to_c(_tmp89C,nv,_tmp897,_tmp861);}
# 4432
_npop_handler(0);goto _LL39A;
# 4426
;_pop_region(temp);};}_LL39D:;_LL39E:
# 4437
 s->r=(Cyc_Toc_letdecl_to_c(nv,_tmp885,(void*)_check_null(_tmp886->topt),_tmp886,_tmp861))->r;
goto _LL39A;_LL39A:;}
# 4440
goto _LL38D;_LL392: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp887=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp881;if(_tmp887->tag != 3)goto _LL394;else{_tmp888=_tmp887->f1;}}_LL393: {
# 4453 "toc.cyc"
struct Cyc_List_List*_tmp89F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp888);
if(_tmp89F == 0){
const char*_tmpDBD;void*_tmpDBC;(_tmpDBC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpDBD="empty Letv_d",_tag_dyneither(_tmpDBD,sizeof(char),13))),_tag_dyneither(_tmpDBC,sizeof(void*),0)));}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpDC0;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpDBF;_tmp860->r=(void*)((_tmpDBF=_cycalloc(sizeof(*_tmpDBF)),((_tmpDBF[0]=((_tmpDC0.tag=0,((_tmpDC0.f1=(struct Cyc_Absyn_Vardecl*)_tmp89F->hd,_tmpDC0)))),_tmpDBF))));}
_tmp89F=_tmp89F->tl;
for(0;_tmp89F != 0;_tmp89F=_tmp89F->tl){
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpDC3;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpDC2;struct Cyc_Absyn_Decl*_tmp8A4=Cyc_Absyn_new_decl((void*)((_tmpDC2=_cycalloc(sizeof(*_tmpDC2)),((_tmpDC2[0]=((_tmpDC3.tag=0,((_tmpDC3.f1=(struct Cyc_Absyn_Vardecl*)_tmp89F->hd,_tmpDC3)))),_tmpDC2)))),0);
s->r=(Cyc_Absyn_decl_stmt(_tmp8A4,Cyc_Absyn_new_stmt(s->r,0),0))->r;}
# 4462
Cyc_Toc_stmt_to_c(nv,s);
goto _LL38D;}_LL394: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp889=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp881;if(_tmp889->tag != 1)goto _LL396;else{_tmp88A=_tmp889->f1;}}_LL395: {
# 4465
struct _tuple1*_tmp8A7=_tmp88A->name;
struct _RegionHandle _tmp8A8=_new_region("temp");struct _RegionHandle*temp=& _tmp8A8;_push_region(temp);{
struct Cyc_Toc_Env*_tmp8A9=Cyc_Toc_add_varmap(temp,nv,_tmp88A->name,Cyc_Absyn_var_exp(_tmp8A7,0));
Cyc_Toc_fndecl_to_c(_tmp8A9,_tmp88A,0);
Cyc_Toc_stmt_to_c(_tmp8A9,_tmp861);}
# 4471
_npop_handler(0);goto _LL38D;
# 4466
;_pop_region(temp);}_LL396: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp88B=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp881;if(_tmp88B->tag != 4)goto _LL398;else{_tmp88C=_tmp88B->f1;_tmp88D=_tmp88B->f2;_tmp88E=_tmp88B->f3;_tmp88F=_tmp88B->f4;}}_LL397: {
# 4473
struct Cyc_Absyn_Stmt*_tmp8AA=_tmp861;
# 4481
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple1*rh_var=Cyc_Toc_temp_var();
struct _tuple1*x_var=_tmp88D->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);
# 4488
struct _RegionHandle _tmp8AB=_new_region("temp");struct _RegionHandle*temp=& _tmp8AB;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(temp,nv,x_var,x_exp),_tmp8AA);
# 4491
if(Cyc_Absyn_no_regions)
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_uint_exp(0,0),_tmp8AA,0))->r;else{
if((unsigned int)_tmp88F){
Cyc_Toc_exp_to_c(nv,_tmp88F);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(Cyc_Absyn_aggrarrow_exp(_tmp88F,_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"h",sizeof(char),2),0),0);
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,_tmp8AA,0))->r;};}else{
# 4499
struct Cyc_Absyn_Exp*_tmpDC6[1];struct Cyc_Absyn_Exp*_tmpDC5[1];struct Cyc_List_List*_tmpDC4;s->r=(
Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,(
(_tmpDC4=_cycalloc(sizeof(*_tmpDC4)),((_tmpDC4->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0),((_tmpDC4->tl=0,_tmpDC4)))))),0),
# 4504
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpDC5[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpDC5,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),
Cyc_Absyn_seq_stmt(_tmp8AA,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpDC6[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpDC6,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}}
# 4510
_npop_handler(0);return;
# 4488
;_pop_region(temp);}_LL398:;_LL399: {
# 4511
const char*_tmpDC9;void*_tmpDC8;(_tmpDC8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpDC9="bad nested declaration within function",_tag_dyneither(_tmpDC9,sizeof(char),39))),_tag_dyneither(_tmpDC8,sizeof(void*),0)));}_LL38D:;}
# 4513
return;_LL385: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp862=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp83E;if(_tmp862->tag != 13)goto _LL387;else{_tmp863=_tmp862->f1;_tmp864=_tmp862->f2;}}_LL386:
# 4515
 s=_tmp864;continue;_LL387: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp865=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp83E;if(_tmp865->tag != 14)goto _LL389;else{_tmp866=_tmp865->f1;_tmp867=(_tmp865->f2).f1;}}_LL388: {
# 4517
struct _RegionHandle _tmp8B2=_new_region("temp");struct _RegionHandle*temp=& _tmp8B2;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp866);
Cyc_Toc_exp_to_c(nv,_tmp867);
# 4521
_npop_handler(0);return;
# 4517
;_pop_region(temp);}_LL389: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp868=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp83E;if(_tmp868->tag != 15)goto _LL38B;else{_tmp869=_tmp868->f1;_tmp86A=_tmp868->f2;}}_LL38A: {
# 4538 "toc.cyc"
struct _tuple1*h_var=Cyc_Toc_temp_var();
struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
# 4549
e_exp->topt=e_typ;{
struct _RegionHandle _tmp8B3=_new_region("temp");struct _RegionHandle*temp=& _tmp8B3;_push_region(temp);{
struct Cyc_Toc_Env*_tmp8B4=Cyc_Toc_add_varmap(temp,nv,e_var,e_exp);
# 4553
Cyc_Toc_stmt_to_c(_tmp8B4,_tmp869);{
struct Cyc_Absyn_Stmt*_tmp8B5=Cyc_Absyn_seq_stmt(_tmp869,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
# 4559
struct _tuple1*_tmp8B6=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp8B7=Cyc_Absyn_var_exp(_tmp8B6,0);
struct Cyc_Absyn_Vardecl*_tmp8B8=Cyc_Absyn_new_vardecl(_tmp8B6,Cyc_Absyn_exn_typ(),0);
_tmp8B7->topt=Cyc_Absyn_exn_typ();{
struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpDD3;struct Cyc_Absyn_Pat*_tmpDD2;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpDD1;struct Cyc_Absyn_Pat*_tmpDD0;struct Cyc_Absyn_Pat*_tmp8B9=(_tmpDD0=_cycalloc(sizeof(*_tmpDD0)),((_tmpDD0->r=(void*)((_tmpDD3=_cycalloc(sizeof(*_tmpDD3)),((_tmpDD3[0]=((_tmpDD1.tag=1,((_tmpDD1.f1=_tmp8B8,((_tmpDD1.f2=((_tmpDD2=_cycalloc(sizeof(*_tmpDD2)),((_tmpDD2->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpDD2->topt=Cyc_Absyn_exn_typ(),((_tmpDD2->loc=0,_tmpDD2)))))))),_tmpDD1)))))),_tmpDD3)))),((_tmpDD0->topt=Cyc_Absyn_exn_typ(),((_tmpDD0->loc=0,_tmpDD0)))))));
struct Cyc_Absyn_Exp*_tmp8BA=Cyc_Absyn_rethrow_exp(_tmp8B7,0);
_tmp8BA->topt=(void*)& Cyc_Absyn_VoidType_val;{
struct Cyc_Absyn_Stmt*_tmp8BB=Cyc_Absyn_exp_stmt(_tmp8BA,0);
struct Cyc_Core_Opt*_tmpDE2;struct _tuple35*_tmpDE1;struct Cyc_Absyn_Vardecl**_tmpDE0;struct Cyc_List_List*_tmpDDF;struct Cyc_Absyn_Switch_clause*_tmpDDE;struct Cyc_Absyn_Switch_clause*_tmp8BC=
(_tmpDDE=_cycalloc(sizeof(*_tmpDDE)),((_tmpDDE->pattern=_tmp8B9,((_tmpDDE->pat_vars=((_tmpDE2=_cycalloc(sizeof(*_tmpDE2)),((_tmpDE2->v=((_tmpDDF=_cycalloc(sizeof(*_tmpDDF)),((_tmpDDF->hd=((_tmpDE1=_cycalloc(sizeof(*_tmpDE1)),((_tmpDE1->f1=((_tmpDE0=_cycalloc(sizeof(*_tmpDE0)),((_tmpDE0[0]=_tmp8B8,_tmpDE0)))),((_tmpDE1->f2=_tmp8B7,_tmpDE1)))))),((_tmpDDF->tl=0,_tmpDDF)))))),_tmpDE2)))),((_tmpDDE->where_clause=0,((_tmpDDE->body=_tmp8BB,((_tmpDDE->loc=0,_tmpDDE)))))))))));
# 4570
struct Cyc_List_List*_tmpDE3;struct Cyc_Absyn_Stmt*_tmp8BD=Cyc_Absyn_switch_stmt(e_exp,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp86A,(
(_tmpDE3=_cycalloc(sizeof(*_tmpDE3)),((_tmpDE3->hd=_tmp8BC,((_tmpDE3->tl=0,_tmpDE3))))))),0);
# 4574
Cyc_Toc_stmt_to_c(_tmp8B4,_tmp8BD);{
# 4577
struct Cyc_List_List*_tmpDE4;struct Cyc_Absyn_Exp*_tmp8BE=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,(
(_tmpDE4=_cycalloc(sizeof(*_tmpDE4)),((_tmpDE4->hd=Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0),((_tmpDE4->tl=0,_tmpDE4)))))),0);
# 4581
struct Cyc_List_List*_tmpDE5;struct Cyc_Absyn_Stmt*_tmp8BF=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,(
(_tmpDE5=_cycalloc(sizeof(*_tmpDE5)),((_tmpDE5->hd=Cyc_Absyn_address_exp(h_exp,0),((_tmpDE5->tl=0,_tmpDE5)))))),0),0);
# 4585
struct Cyc_Absyn_Exp*_tmp8C0=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0);
struct Cyc_Absyn_Exp*_tmp8C1=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0);
s->r=(
Cyc_Absyn_declare_stmt(h_var,h_typ,0,
Cyc_Absyn_seq_stmt(_tmp8BF,
Cyc_Absyn_declare_stmt(was_thrown_var,was_thrown_typ,_tmp8C0,
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp8BE,
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp8C1,0),
Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0),_tmp8B5,
# 4596
Cyc_Absyn_declare_stmt(e_var,e_typ,
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp8BD,0),0),0),0),0),0))->r;};};};};}
# 4600
_npop_handler(0);return;
# 4550
;_pop_region(temp);};}_LL38B: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp86B=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp83E;if(_tmp86B->tag != 16)goto _LL36A;else{_tmp86C=_tmp86B->f1;}}_LL38C:
# 4602
 if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{
# 4605
Cyc_Toc_exp_to_c(nv,_tmp86C);{
struct Cyc_List_List*_tmpDE6;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((_tmpDE6=_cycalloc(sizeof(*_tmpDE6)),((_tmpDE6->hd=_tmp86C,((_tmpDE6->tl=0,_tmpDE6)))))),0));};}
# 4608
return;_LL36A:;}}
# 4617
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->requires_clause=0;
f->ensures_clause=0;
f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp8CF=_new_region("frgn");struct _RegionHandle*frgn=& _tmp8CF;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp8D0=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp8D1=f->args;for(0;_tmp8D1 != 0;_tmp8D1=_tmp8D1->tl){
struct _tuple1*_tmpDE7;struct _tuple1*_tmp8D2=(_tmpDE7=_cycalloc(sizeof(*_tmpDE7)),((_tmpDE7->f1=Cyc_Absyn_Loc_n,((_tmpDE7->f2=(*((struct _tuple9*)_tmp8D1->hd)).f1,_tmpDE7)))));
(*((struct _tuple9*)_tmp8D1->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp8D1->hd)).f3);
_tmp8D0=Cyc_Toc_add_varmap(frgn,_tmp8D0,_tmp8D2,Cyc_Absyn_var_exp(_tmp8D2,0));}}
# 4634
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}
# 4638
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp8D4=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));struct _dyneither_ptr*_tmp8D6;struct Cyc_Absyn_Tqual _tmp8D7;void*_tmp8D8;int _tmp8D9;struct Cyc_Absyn_VarargInfo _tmp8D5=_tmp8D4;_tmp8D6=_tmp8D5.name;_tmp8D7=_tmp8D5.tq;_tmp8D8=_tmp8D5.type;_tmp8D9=_tmp8D5.inject;{
void*_tmp8DA=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp8D8,(void*)& Cyc_Absyn_HeapRgn_val,_tmp8D7,Cyc_Absyn_false_conref));
struct _tuple1*_tmpDE8;struct _tuple1*_tmp8DB=(_tmpDE8=_cycalloc(sizeof(*_tmpDE8)),((_tmpDE8->f1=Cyc_Absyn_Loc_n,((_tmpDE8->f2=(struct _dyneither_ptr*)_check_null(_tmp8D6),_tmpDE8)))));
{struct _tuple9*_tmpDEB;struct Cyc_List_List*_tmpDEA;f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpDEA=_cycalloc(sizeof(*_tmpDEA)),((_tmpDEA->hd=((_tmpDEB=_cycalloc(sizeof(*_tmpDEB)),((_tmpDEB->f1=_tmp8D6,((_tmpDEB->f2=_tmp8D7,((_tmpDEB->f3=_tmp8DA,_tmpDEB)))))))),((_tmpDEA->tl=0,_tmpDEA)))))));}
_tmp8D0=Cyc_Toc_add_varmap(frgn,_tmp8D0,_tmp8DB,Cyc_Absyn_var_exp(_tmp8DB,0));
f->cyc_varargs=0;};}
# 4647
{struct Cyc_List_List*_tmp8DF=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp8DF != 0;_tmp8DF=_tmp8DF->tl){
((struct Cyc_Absyn_Vardecl*)_tmp8DF->hd)->type=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp8DF->hd)->type);}}
# 4650
Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp8D0),f->body);}
# 4626
;_pop_region(frgn);};}
# 4655
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
switch(s){case Cyc_Absyn_Abstract: _LL39F:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3A0:
 return Cyc_Absyn_Extern;default: _LL3A1:
 return s;}}struct _tuple36{struct Cyc_Absyn_Aggrdecl*f1;int f2;};struct _tuple37{struct Cyc_Toc_TocState*f1;struct _tuple36*f2;};
# 4672 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple37*env){
# 4675
struct _tuple37 _tmp8E0=*env;struct Cyc_Toc_TocState*_tmp8E2;struct Cyc_Absyn_Aggrdecl*_tmp8E3;int _tmp8E4;struct _tuple37 _tmp8E1=_tmp8E0;_tmp8E2=_tmp8E1.f1;_tmp8E3=(_tmp8E1.f2)->f1;_tmp8E4=(_tmp8E1.f2)->f2;{
struct _tuple1*_tmp8E5=_tmp8E3->name;
struct Cyc_Toc_TocState _tmp8E6=*_tmp8E2;struct Cyc_Dict_Dict*_tmp8E8;struct Cyc_Toc_TocState _tmp8E7=_tmp8E6;_tmp8E8=_tmp8E7.aggrs_so_far;{
int seen_defn_before;
struct _tuple18**_tmp8E9=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp8E8,_tmp8E5);
if(_tmp8E9 == 0){
seen_defn_before=0;{
struct _tuple18*v;
if(_tmp8E3->kind == Cyc_Absyn_StructA){
struct _tuple18*_tmpDEC;v=((_tmpDEC=_region_malloc(d,sizeof(*_tmpDEC)),((_tmpDEC->f1=_tmp8E3,((_tmpDEC->f2=Cyc_Absyn_strctq(_tmp8E5),_tmpDEC))))));}else{
# 4686
struct _tuple18*_tmpDED;v=((_tmpDED=_region_malloc(d,sizeof(*_tmpDED)),((_tmpDED->f1=_tmp8E3,((_tmpDED->f2=Cyc_Absyn_unionq_typ(_tmp8E5),_tmpDED))))));}
*_tmp8E8=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp8E8,_tmp8E5,v);};}else{
# 4689
struct _tuple18*_tmp8EC=*_tmp8E9;struct Cyc_Absyn_Aggrdecl*_tmp8EE;void*_tmp8EF;struct _tuple18*_tmp8ED=_tmp8EC;_tmp8EE=_tmp8ED->f1;_tmp8EF=_tmp8ED->f2;
if(_tmp8EE->impl == 0){
{struct _tuple18*_tmpDEE;*_tmp8E8=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp8E8,_tmp8E5,((_tmpDEE=_region_malloc(d,sizeof(*_tmpDEE)),((_tmpDEE->f1=_tmp8E3,((_tmpDEE->f2=_tmp8EF,_tmpDEE)))))));}
seen_defn_before=0;}else{
# 4694
seen_defn_before=1;}}{
# 4696
struct Cyc_Absyn_Aggrdecl*_tmpDEF;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpDEF=_cycalloc(sizeof(*_tmpDEF)),((_tmpDEF->kind=_tmp8E3->kind,((_tmpDEF->sc=Cyc_Absyn_Public,((_tmpDEF->name=_tmp8E3->name,((_tmpDEF->tvs=0,((_tmpDEF->impl=0,((_tmpDEF->expected_mem_kind=0,((_tmpDEF->attributes=_tmp8E3->attributes,_tmpDEF)))))))))))))));
# 4703
if(_tmp8E3->impl != 0  && !seen_defn_before){
{struct Cyc_Absyn_AggrdeclImpl*_tmpDF0;new_ad->impl=((_tmpDF0=_cycalloc(sizeof(*_tmpDF0)),((_tmpDF0->exist_vars=0,((_tmpDF0->rgn_po=0,((_tmpDF0->fields=0,((_tmpDF0->tagged=0,_tmpDF0))))))))));}{
# 4708
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp8F2=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp8E3->impl))->fields;for(0;_tmp8F2 != 0;_tmp8F2=_tmp8F2->tl){
# 4712
struct Cyc_Absyn_Aggrfield*_tmp8F3=(struct Cyc_Absyn_Aggrfield*)_tmp8F2->hd;
void*_tmp8F4=_tmp8F3->type;
struct Cyc_List_List*_tmp8F5=_tmp8F3->attributes;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp8F4)) && (
_tmp8E3->kind == Cyc_Absyn_StructA  && _tmp8F2->tl == 0  || _tmp8E3->kind == Cyc_Absyn_UnionA)){
# 4726 "toc.cyc"
void*_tmp8F6=Cyc_Tcutil_compress(_tmp8F4);void*_tmp8F7=_tmp8F6;void*_tmp8F9;struct Cyc_Absyn_Tqual _tmp8FA;union Cyc_Absyn_Constraint*_tmp8FB;unsigned int _tmp8FC;_LL3A4: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8F8=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8F7;if(_tmp8F8->tag != 8)goto _LL3A6;else{_tmp8F9=(_tmp8F8->f1).elt_type;_tmp8FA=(_tmp8F8->f1).tq;_tmp8FB=(_tmp8F8->f1).zero_term;_tmp8FC=(_tmp8F8->f1).zt_loc;}}_LL3A5:
# 4729
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpDF6;struct Cyc_Absyn_ArrayInfo _tmpDF5;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpDF4;_tmp8F4=(void*)((_tmpDF4=_cycalloc(sizeof(*_tmpDF4)),((_tmpDF4[0]=((_tmpDF6.tag=8,((_tmpDF6.f1=((_tmpDF5.elt_type=_tmp8F9,((_tmpDF5.tq=_tmp8FA,((_tmpDF5.num_elts=Cyc_Absyn_uint_exp(0,0),((_tmpDF5.zero_term=_tmp8FB,((_tmpDF5.zt_loc=_tmp8FC,_tmpDF5)))))))))),_tmpDF6)))),_tmpDF4))));}
goto _LL3A3;_LL3A6:;_LL3A7:
# 4732
{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpDFC;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpDFB;struct Cyc_List_List*_tmpDFA;_tmp8F5=((_tmpDFA=_cycalloc(sizeof(*_tmpDFA)),((_tmpDFA->hd=(void*)((_tmpDFC=_cycalloc(sizeof(*_tmpDFC)),((_tmpDFC[0]=((_tmpDFB.tag=6,((_tmpDFB.f1=0,_tmpDFB)))),_tmpDFC)))),((_tmpDFA->tl=_tmp8F5,_tmpDFA))))));}{
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpE02;struct Cyc_Absyn_ArrayInfo _tmpE01;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpE00;_tmp8F4=(void*)((_tmpE00=_cycalloc(sizeof(*_tmpE00)),((_tmpE00[0]=((_tmpE02.tag=8,((_tmpE02.f1=((_tmpE01.elt_type=Cyc_Absyn_void_star_typ(),((_tmpE01.tq=
Cyc_Absyn_empty_tqual(0),((_tmpE01.num_elts=
Cyc_Absyn_uint_exp(0,0),((_tmpE01.zero_term=
((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0),((_tmpE01.zt_loc=0,_tmpE01)))))))))),_tmpE02)))),_tmpE00))));};_LL3A3:;}{
# 4739
struct Cyc_Absyn_Aggrfield*_tmpE03;struct Cyc_Absyn_Aggrfield*_tmp906=(_tmpE03=_cycalloc(sizeof(*_tmpE03)),((_tmpE03->name=_tmp8F3->name,((_tmpE03->tq=Cyc_Toc_mt_tq,((_tmpE03->type=
# 4741
Cyc_Toc_typ_to_c(_tmp8F4),((_tmpE03->width=_tmp8F3->width,((_tmpE03->attributes=_tmp8F5,((_tmpE03->requires_clause=0,_tmpE03)))))))))))));
# 4749
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp8E3->impl))->tagged){
void*_tmp907=_tmp906->type;
struct _dyneither_ptr*_tmp908=_tmp906->name;
const char*_tmpE08;void*_tmpE07[2];struct Cyc_String_pa_PrintArg_struct _tmpE06;struct Cyc_String_pa_PrintArg_struct _tmpE05;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpE05.tag=0,((_tmpE05.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp908),((_tmpE06.tag=0,((_tmpE06.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp8E3->name).f2),((_tmpE07[0]=& _tmpE06,((_tmpE07[1]=& _tmpE05,Cyc_aprintf(((_tmpE08="_union_%s_%s",_tag_dyneither(_tmpE08,sizeof(char),13))),_tag_dyneither(_tmpE07,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpE09;struct _dyneither_ptr*str=(_tmpE09=_cycalloc(sizeof(*_tmpE09)),((_tmpE09[0]=s,_tmpE09)));
struct Cyc_Absyn_Aggrfield*_tmpE0A;struct Cyc_Absyn_Aggrfield*_tmp909=(_tmpE0A=_cycalloc(sizeof(*_tmpE0A)),((_tmpE0A->name=Cyc_Toc_val_sp,((_tmpE0A->tq=Cyc_Toc_mt_tq,((_tmpE0A->type=_tmp907,((_tmpE0A->width=0,((_tmpE0A->attributes=0,((_tmpE0A->requires_clause=0,_tmpE0A)))))))))))));
struct Cyc_Absyn_Aggrfield*_tmpE0B;struct Cyc_Absyn_Aggrfield*_tmp90A=(_tmpE0B=_cycalloc(sizeof(*_tmpE0B)),((_tmpE0B->name=Cyc_Toc_tag_sp,((_tmpE0B->tq=Cyc_Toc_mt_tq,((_tmpE0B->type=Cyc_Absyn_sint_typ,((_tmpE0B->width=0,((_tmpE0B->attributes=0,((_tmpE0B->requires_clause=0,_tmpE0B)))))))))))));
struct Cyc_Absyn_Aggrfield*_tmpE0C[2];struct Cyc_List_List*_tmp90B=(_tmpE0C[1]=_tmp909,((_tmpE0C[0]=_tmp90A,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpE0C,sizeof(struct Cyc_Absyn_Aggrfield*),2)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpE11;struct _tuple1*_tmpE10;struct Cyc_Absyn_Aggrdecl*_tmpE0F;struct Cyc_Absyn_Aggrdecl*_tmp90C=(_tmpE0F=_cycalloc(sizeof(*_tmpE0F)),((_tmpE0F->kind=Cyc_Absyn_StructA,((_tmpE0F->sc=Cyc_Absyn_Public,((_tmpE0F->name=(
(_tmpE10=_cycalloc(sizeof(*_tmpE10)),((_tmpE10->f1=Cyc_Absyn_Loc_n,((_tmpE10->f2=str,_tmpE10)))))),((_tmpE0F->tvs=0,((_tmpE0F->impl=(
(_tmpE11=_cycalloc(sizeof(*_tmpE11)),((_tmpE11->exist_vars=0,((_tmpE11->rgn_po=0,((_tmpE11->fields=_tmp90B,((_tmpE11->tagged=0,_tmpE11)))))))))),((_tmpE0F->expected_mem_kind=0,((_tmpE0F->attributes=0,_tmpE0F)))))))))))))));
# 4762
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpE17;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpE16;struct Cyc_List_List*_tmpE15;Cyc_Toc_result_decls=((_tmpE15=_cycalloc(sizeof(*_tmpE15)),((_tmpE15->hd=Cyc_Absyn_new_decl((void*)((_tmpE17=_cycalloc(sizeof(*_tmpE17)),((_tmpE17[0]=((_tmpE16.tag=5,((_tmpE16.f1=_tmp90C,_tmpE16)))),_tmpE17)))),0),((_tmpE15->tl=Cyc_Toc_result_decls,_tmpE15))))));}
_tmp906->type=Cyc_Absyn_strct(str);}{
# 4765
struct Cyc_List_List*_tmpE18;new_fields=((_tmpE18=_cycalloc(sizeof(*_tmpE18)),((_tmpE18->hd=_tmp906,((_tmpE18->tl=new_fields,_tmpE18))))));};};}}
# 4767
(new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);};}
# 4769
if(_tmp8E4){
struct Cyc_Absyn_Decl*_tmpE22;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpE21;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpE20;struct Cyc_List_List*_tmpE1F;Cyc_Toc_result_decls=((_tmpE1F=_cycalloc(sizeof(*_tmpE1F)),((_tmpE1F->hd=((_tmpE22=_cycalloc(sizeof(*_tmpE22)),((_tmpE22->r=(void*)((_tmpE20=_cycalloc(sizeof(*_tmpE20)),((_tmpE20[0]=((_tmpE21.tag=5,((_tmpE21.f1=new_ad,_tmpE21)))),_tmpE20)))),((_tmpE22->loc=0,_tmpE22)))))),((_tmpE1F->tl=Cyc_Toc_result_decls,_tmpE1F))))));}
return 0;};};};}
# 4774
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple36 _tmpE23;struct _tuple36 p=(_tmpE23.f1=ad,((_tmpE23.f2=add_to_decls,_tmpE23)));
((int(*)(struct _tuple36*arg,int(*f)(struct _RegionHandle*,struct _tuple37*)))Cyc_Toc_use_toc_state)(& p,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple38{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 4803 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple38*env){
# 4806
struct _tuple38 _tmp923=*env;struct Cyc_Set_Set**_tmp925;struct Cyc_Absyn_Datatypedecl*_tmp926;struct _tuple38 _tmp924=_tmp923;_tmp925=(_tmp924.f1)->datatypes_so_far;_tmp926=_tmp924.f2;{
struct _tuple1*_tmp927=_tmp926->name;
if(_tmp926->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*_tmp925,_tmp927))
return 0;
*_tmp925=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*_tmp925,_tmp927);
{struct Cyc_List_List*_tmp928=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp926->fields))->v;for(0;_tmp928 != 0;_tmp928=_tmp928->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp928->hd;
# 4814
struct Cyc_List_List*_tmp929=0;
int i=1;
{struct Cyc_List_List*_tmp92A=f->typs;for(0;_tmp92A != 0;(_tmp92A=_tmp92A->tl,i ++)){
struct _dyneither_ptr*_tmp92B=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmpE24;struct Cyc_Absyn_Aggrfield*_tmp92C=(_tmpE24=_cycalloc(sizeof(*_tmpE24)),((_tmpE24->name=_tmp92B,((_tmpE24->tq=(*((struct _tuple11*)_tmp92A->hd)).f1,((_tmpE24->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp92A->hd)).f2),((_tmpE24->width=0,((_tmpE24->attributes=0,((_tmpE24->requires_clause=0,_tmpE24)))))))))))));
struct Cyc_List_List*_tmpE25;_tmp929=((_tmpE25=_cycalloc(sizeof(*_tmpE25)),((_tmpE25->hd=_tmp92C,((_tmpE25->tl=_tmp929,_tmpE25))))));}}
# 4822
{struct Cyc_Absyn_Aggrfield*_tmpE28;struct Cyc_List_List*_tmpE27;_tmp929=((_tmpE27=_cycalloc(sizeof(*_tmpE27)),((_tmpE27->hd=((_tmpE28=_cycalloc(sizeof(*_tmpE28)),((_tmpE28->name=Cyc_Toc_tag_sp,((_tmpE28->tq=Cyc_Toc_mt_tq,((_tmpE28->type=Cyc_Absyn_sint_typ,((_tmpE28->width=0,((_tmpE28->attributes=0,((_tmpE28->requires_clause=0,_tmpE28)))))))))))))),((_tmpE27->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp929),_tmpE27))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpE2B;struct Cyc_Absyn_Aggrdecl*_tmpE2A;struct Cyc_Absyn_Aggrdecl*_tmp931=
(_tmpE2A=_cycalloc(sizeof(*_tmpE2A)),((_tmpE2A->kind=Cyc_Absyn_StructA,((_tmpE2A->sc=Cyc_Absyn_Public,((_tmpE2A->name=
Cyc_Toc_collapse_qvars(f->name,_tmp926->name),((_tmpE2A->tvs=0,((_tmpE2A->expected_mem_kind=0,((_tmpE2A->impl=(
# 4828
(_tmpE2B=_cycalloc(sizeof(*_tmpE2B)),((_tmpE2B->exist_vars=0,((_tmpE2B->rgn_po=0,((_tmpE2B->fields=_tmp929,((_tmpE2B->tagged=0,_tmpE2B)))))))))),((_tmpE2A->attributes=0,_tmpE2A)))))))))))))));
# 4830
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpE31;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpE30;struct Cyc_List_List*_tmpE2F;Cyc_Toc_result_decls=((_tmpE2F=_cycalloc(sizeof(*_tmpE2F)),((_tmpE2F->hd=Cyc_Absyn_new_decl((void*)((_tmpE31=_cycalloc(sizeof(*_tmpE31)),((_tmpE31[0]=((_tmpE30.tag=5,((_tmpE30.f1=_tmp931,_tmpE30)))),_tmpE31)))),0),((_tmpE2F->tl=Cyc_Toc_result_decls,_tmpE2F))))));};}}
# 4832
return 0;};}
# 4835
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple38*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 4854 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple38*env){
# 4857
struct _tuple38 _tmp937=*env;struct Cyc_Toc_TocState*_tmp939;struct Cyc_Absyn_Datatypedecl*_tmp93A;struct _tuple38 _tmp938=_tmp937;_tmp939=_tmp938.f1;_tmp93A=_tmp938.f2;
if(_tmp93A->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp93B=*_tmp939;struct Cyc_Dict_Dict*_tmp93D;struct Cyc_Toc_TocState _tmp93C=_tmp93B;_tmp93D=_tmp93C.xdatatypes_so_far;{
struct _tuple1*_tmp93E=_tmp93A->name;
{struct Cyc_List_List*_tmp93F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp93A->fields))->v;for(0;_tmp93F != 0;_tmp93F=_tmp93F->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp93F->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp940=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0);
void*_tmp941=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,_tmp940,Cyc_Absyn_false_conref,0);
# 4868
int*_tmp942=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp93D,f->name);int*_tmp943=_tmp942;_LL3A9: if(_tmp943 != 0)goto _LL3AB;_LL3AA: {
# 4870
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0);{
# 4874
struct Cyc_Absyn_Vardecl*_tmp944=Cyc_Absyn_new_vardecl(f->name,_tmp941,initopt);
_tmp944->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpE37;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpE36;struct Cyc_List_List*_tmpE35;Cyc_Toc_result_decls=((_tmpE35=_cycalloc(sizeof(*_tmpE35)),((_tmpE35->hd=Cyc_Absyn_new_decl((void*)((_tmpE37=_cycalloc(sizeof(*_tmpE37)),((_tmpE37[0]=((_tmpE36.tag=0,((_tmpE36.f1=_tmp944,_tmpE36)))),_tmpE37)))),0),((_tmpE35->tl=Cyc_Toc_result_decls,_tmpE35))))));}
*_tmp93D=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp93D,f->name,f->sc != Cyc_Absyn_Extern);{
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp948=f->typs;for(0;_tmp948 != 0;(_tmp948=_tmp948->tl,i ++)){
struct Cyc_Int_pa_PrintArg_struct _tmpE3F;void*_tmpE3E[1];const char*_tmpE3D;struct _dyneither_ptr*_tmpE3C;struct _dyneither_ptr*_tmp949=(_tmpE3C=_cycalloc(sizeof(*_tmpE3C)),((_tmpE3C[0]=(struct _dyneither_ptr)((_tmpE3F.tag=1,((_tmpE3F.f1=(unsigned long)i,((_tmpE3E[0]=& _tmpE3F,Cyc_aprintf(((_tmpE3D="f%d",_tag_dyneither(_tmpE3D,sizeof(char),4))),_tag_dyneither(_tmpE3E,sizeof(void*),1)))))))),_tmpE3C)));
struct Cyc_Absyn_Aggrfield*_tmpE40;struct Cyc_Absyn_Aggrfield*_tmp94A=(_tmpE40=_cycalloc(sizeof(*_tmpE40)),((_tmpE40->name=_tmp949,((_tmpE40->tq=(*((struct _tuple11*)_tmp948->hd)).f1,((_tmpE40->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp948->hd)).f2),((_tmpE40->width=0,((_tmpE40->attributes=0,((_tmpE40->requires_clause=0,_tmpE40)))))))))))));
struct Cyc_List_List*_tmpE41;fields=((_tmpE41=_cycalloc(sizeof(*_tmpE41)),((_tmpE41->hd=_tmp94A,((_tmpE41->tl=fields,_tmpE41))))));}}
# 4887
{struct Cyc_Absyn_Aggrfield*_tmpE44;struct Cyc_List_List*_tmpE43;fields=((_tmpE43=_cycalloc(sizeof(*_tmpE43)),((_tmpE43->hd=((_tmpE44=_cycalloc(sizeof(*_tmpE44)),((_tmpE44->name=Cyc_Toc_tag_sp,((_tmpE44->tq=Cyc_Toc_mt_tq,((_tmpE44->type=
Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq),((_tmpE44->width=0,((_tmpE44->attributes=0,((_tmpE44->requires_clause=0,_tmpE44)))))))))))))),((_tmpE43->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmpE43))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpE47;struct Cyc_Absyn_Aggrdecl*_tmpE46;struct Cyc_Absyn_Aggrdecl*_tmp953=
(_tmpE46=_cycalloc(sizeof(*_tmpE46)),((_tmpE46->kind=Cyc_Absyn_StructA,((_tmpE46->sc=Cyc_Absyn_Public,((_tmpE46->name=
Cyc_Toc_collapse_qvars(f->name,_tmp93A->name),((_tmpE46->tvs=0,((_tmpE46->expected_mem_kind=0,((_tmpE46->impl=(
# 4894
(_tmpE47=_cycalloc(sizeof(*_tmpE47)),((_tmpE47->exist_vars=0,((_tmpE47->rgn_po=0,((_tmpE47->fields=fields,((_tmpE47->tagged=0,_tmpE47)))))))))),((_tmpE46->attributes=0,_tmpE46)))))))))))))));
# 4896
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpE4D;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpE4C;struct Cyc_List_List*_tmpE4B;Cyc_Toc_result_decls=((_tmpE4B=_cycalloc(sizeof(*_tmpE4B)),((_tmpE4B->hd=Cyc_Absyn_new_decl((void*)((_tmpE4D=_cycalloc(sizeof(*_tmpE4D)),((_tmpE4D[0]=((_tmpE4C.tag=5,((_tmpE4C.f1=_tmp953,_tmpE4C)))),_tmpE4D)))),0),((_tmpE4B->tl=Cyc_Toc_result_decls,_tmpE4B))))));}
# 4898
goto _LL3A8;};};};}_LL3AB: if(_tmp943 == 0)goto _LL3AD;if(*_tmp943 != 0)goto _LL3AD;_LL3AC:
# 4900
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp959=Cyc_Absyn_string_exp(*fn,0);
struct Cyc_Absyn_Vardecl*_tmp95A=Cyc_Absyn_new_vardecl(f->name,_tmp941,_tmp959);
_tmp95A->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpE53;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpE52;struct Cyc_List_List*_tmpE51;Cyc_Toc_result_decls=((_tmpE51=_cycalloc(sizeof(*_tmpE51)),((_tmpE51->hd=Cyc_Absyn_new_decl((void*)((_tmpE53=_cycalloc(sizeof(*_tmpE53)),((_tmpE53[0]=((_tmpE52.tag=0,((_tmpE52.f1=_tmp95A,_tmpE52)))),_tmpE53)))),0),((_tmpE51->tl=Cyc_Toc_result_decls,_tmpE51))))));}
*_tmp93D=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp93D,f->name,1);}
# 4907
goto _LL3A8;_LL3AD:;_LL3AE:
 goto _LL3A8;_LL3A8:;}}
# 4911
return 0;};};}
# 4914
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple38*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 4919
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4927
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
vd->type=Cyc_Toc_typ_to_c(old_typ);
# 4931
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(body_nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp95E=init->r;void*_tmp95F=_tmp95E;struct Cyc_Absyn_Vardecl*_tmp961;struct Cyc_Absyn_Exp*_tmp962;struct Cyc_Absyn_Exp*_tmp963;int _tmp964;_LL3B0: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp960=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp95F;if(_tmp960->tag != 26)goto _LL3B2;else{_tmp961=_tmp960->f1;_tmp962=_tmp960->f2;_tmp963=_tmp960->f3;_tmp964=_tmp960->f4;}}_LL3B1:
# 4938
 vd->initializer=0;
s->r=(Cyc_Toc_init_comprehension(init_nv,old_typ,Cyc_Absyn_var_exp(vd->name,0),_tmp961,_tmp962,_tmp963,_tmp964,
Cyc_Absyn_new_stmt(s->r,0),0))->r;
goto _LL3AF;_LL3B2:;_LL3B3:
# 4943
 if(vd->sc == Cyc_Absyn_Static){
# 4947
struct _RegionHandle _tmp965=_new_region("temp");struct _RegionHandle*temp=& _tmp965;_push_region(temp);
{struct Cyc_Toc_Env*_tmp966=Cyc_Toc_set_toplevel(temp,init_nv);
Cyc_Toc_exp_to_c(_tmp966,init);}
# 4948
;_pop_region(temp);}else{
# 4952
Cyc_Toc_exp_to_c(init_nv,init);}
goto _LL3AF;_LL3AF:;}else{
# 4957
void*_tmp967=Cyc_Tcutil_compress(old_typ);void*_tmp968=_tmp967;void*_tmp96A;struct Cyc_Absyn_Exp*_tmp96B;union Cyc_Absyn_Constraint*_tmp96C;_LL3B5:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp969=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp968;if(_tmp969->tag != 8)goto _LL3B7;else{_tmp96A=(_tmp969->f1).elt_type;_tmp96B=(_tmp969->f1).num_elts;_tmp96C=(_tmp969->f1).zero_term;}}if(!
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp96C))goto _LL3B7;_LL3B6:
 if(_tmp96B == 0){
const char*_tmpE56;void*_tmpE55;(_tmpE55=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE56="can't initialize zero-terminated array -- size unknown",_tag_dyneither(_tmpE56,sizeof(char),55))),_tag_dyneither(_tmpE55,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=_tmp96B;
struct Cyc_Absyn_Exp*_tmp96F=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),
Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);
# 4965
struct Cyc_Absyn_Exp*_tmp970=Cyc_Absyn_signed_int_exp(0,0);
s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp96F,_tmp970,0),0),
Cyc_Absyn_new_stmt(s->r,0));
goto _LL3B4;};_LL3B7:;_LL3B8:
 goto _LL3B4;_LL3B4:;}}
# 4979
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*x=Cyc_Toc_temp_var();
struct _RegionHandle _tmp971=_new_region("prgn");struct _RegionHandle*prgn=& _tmp971;_push_region(prgn);{
struct Cyc_Absyn_Stmt*_tmp972=Cyc_Toc_throw_match_stmt();
struct Cyc_Toc_Env*_tmp973=Cyc_Toc_share_env(prgn,nv);
struct _tuple31 _tmp974=
Cyc_Toc_xlate_pat(_tmp973,prgn,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,
Cyc_Toc_throw_match_stmt(),0);
# 4985
struct Cyc_Toc_Env*_tmp976;struct Cyc_List_List*_tmp977;struct Cyc_Absyn_Stmt*_tmp978;struct _tuple31 _tmp975=_tmp974;_tmp976=_tmp975.f1;_tmp977=_tmp975.f2;_tmp978=_tmp975.f3;
# 4988
Cyc_Toc_stmt_to_c(_tmp976,s);
s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),e,Cyc_Absyn_seq_stmt(_tmp978,s,0),0);
for(0;_tmp977 != 0;_tmp977=_tmp977->tl){
struct _tuple32*_tmp979=(struct _tuple32*)_tmp977->hd;struct _tuple1*_tmp97B;void*_tmp97C;struct _tuple32*_tmp97A=_tmp979;_tmp97B=_tmp97A->f1;_tmp97C=_tmp97A->f2;
s=Cyc_Absyn_declare_stmt(_tmp97B,_tmp97C,0,s,0);}}{
# 4995
struct Cyc_Absyn_Stmt*_tmp97D=s;_npop_handler(0);return _tmp97D;};
# 4982
;_pop_region(prgn);};}
# 5001
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp97E=e->r;void*_tmp97F=_tmp97E;struct Cyc_Absyn_Exp*_tmp981;struct Cyc_Absyn_Exp*_tmp983;struct Cyc_Absyn_Exp*_tmp985;struct Cyc_Absyn_Exp*_tmp987;struct Cyc_Absyn_Exp*_tmp989;struct Cyc_Absyn_Exp*_tmp98B;struct Cyc_Absyn_Exp*_tmp98D;struct Cyc_Absyn_Exp*_tmp98F;struct Cyc_List_List*_tmp991;struct Cyc_Absyn_Exp*_tmp993;struct Cyc_Absyn_Exp*_tmp994;struct Cyc_Absyn_Exp*_tmp996;struct Cyc_Absyn_Exp*_tmp997;struct Cyc_Absyn_Exp*_tmp999;struct Cyc_Absyn_Exp*_tmp99A;struct Cyc_Absyn_Exp*_tmp99C;struct Cyc_Absyn_Exp*_tmp99D;struct Cyc_Absyn_Exp*_tmp99F;struct Cyc_Absyn_Exp*_tmp9A0;struct Cyc_Absyn_Exp*_tmp9A2;struct Cyc_Absyn_Exp*_tmp9A3;struct Cyc_Absyn_Exp*_tmp9A5;struct Cyc_Absyn_Exp*_tmp9A6;struct Cyc_Absyn_Exp*_tmp9A7;struct Cyc_Absyn_Exp*_tmp9A9;struct Cyc_List_List*_tmp9AA;void**_tmp9AC;struct Cyc_Absyn_Exp*_tmp9AD;void**_tmp9AF;struct Cyc_List_List*_tmp9B0;struct Cyc_List_List*_tmp9B2;struct Cyc_List_List*_tmp9B4;void**_tmp9B6;void**_tmp9B8;struct Cyc_Absyn_Stmt*_tmp9BA;struct Cyc_Absyn_MallocInfo*_tmp9BC;_LL3BA: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp980=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp980->tag != 19)goto _LL3BC;else{_tmp981=_tmp980->f1;}}_LL3BB:
 _tmp983=_tmp981;goto _LL3BD;_LL3BC: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp982=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp982->tag != 20)goto _LL3BE;else{_tmp983=_tmp982->f1;}}_LL3BD:
 _tmp985=_tmp983;goto _LL3BF;_LL3BE: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp984=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp984->tag != 21)goto _LL3C0;else{_tmp985=_tmp984->f1;}}_LL3BF:
 _tmp987=_tmp985;goto _LL3C1;_LL3C0: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp986=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp986->tag != 14)goto _LL3C2;else{_tmp987=_tmp986->f1;}}_LL3C1:
 _tmp989=_tmp987;goto _LL3C3;_LL3C2: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp988=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp988->tag != 10)goto _LL3C4;else{_tmp989=_tmp988->f1;}}_LL3C3:
 _tmp98B=_tmp989;goto _LL3C5;_LL3C4: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp98A=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp98A->tag != 11)goto _LL3C6;else{_tmp98B=_tmp98A->f1;}}_LL3C5:
 _tmp98D=_tmp98B;goto _LL3C7;_LL3C6: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp98C=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp98C->tag != 17)goto _LL3C8;else{_tmp98D=_tmp98C->f1;}}_LL3C7:
 _tmp98F=_tmp98D;goto _LL3C9;_LL3C8: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp98E=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp98E->tag != 4)goto _LL3CA;else{_tmp98F=_tmp98E->f1;}}_LL3C9:
 Cyc_Toc_exptypes_to_c(_tmp98F);goto _LL3B9;_LL3CA: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp990=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp990->tag != 2)goto _LL3CC;else{_tmp991=_tmp990->f2;}}_LL3CB:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp991);goto _LL3B9;_LL3CC: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp992=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp992->tag != 6)goto _LL3CE;else{_tmp993=_tmp992->f1;_tmp994=_tmp992->f2;}}_LL3CD:
 _tmp996=_tmp993;_tmp997=_tmp994;goto _LL3CF;_LL3CE: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp995=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp995->tag != 7)goto _LL3D0;else{_tmp996=_tmp995->f1;_tmp997=_tmp995->f2;}}_LL3CF:
 _tmp999=_tmp996;_tmp99A=_tmp997;goto _LL3D1;_LL3D0: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp998=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp998->tag != 8)goto _LL3D2;else{_tmp999=_tmp998->f1;_tmp99A=_tmp998->f2;}}_LL3D1:
 _tmp99C=_tmp999;_tmp99D=_tmp99A;goto _LL3D3;_LL3D2: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp99B=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp99B->tag != 22)goto _LL3D4;else{_tmp99C=_tmp99B->f1;_tmp99D=_tmp99B->f2;}}_LL3D3:
 _tmp99F=_tmp99C;_tmp9A0=_tmp99D;goto _LL3D5;_LL3D4: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp99E=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp99E->tag != 34)goto _LL3D6;else{_tmp99F=_tmp99E->f1;_tmp9A0=_tmp99E->f2;}}_LL3D5:
 _tmp9A2=_tmp99F;_tmp9A3=_tmp9A0;goto _LL3D7;_LL3D6: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp9A1=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp9A1->tag != 3)goto _LL3D8;else{_tmp9A2=_tmp9A1->f1;_tmp9A3=_tmp9A1->f3;}}_LL3D7:
 Cyc_Toc_exptypes_to_c(_tmp9A2);Cyc_Toc_exptypes_to_c(_tmp9A3);goto _LL3B9;_LL3D8: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp9A4=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp9A4->tag != 5)goto _LL3DA;else{_tmp9A5=_tmp9A4->f1;_tmp9A6=_tmp9A4->f2;_tmp9A7=_tmp9A4->f3;}}_LL3D9:
# 5019
 Cyc_Toc_exptypes_to_c(_tmp9A5);Cyc_Toc_exptypes_to_c(_tmp9A6);Cyc_Toc_exptypes_to_c(_tmp9A7);goto _LL3B9;_LL3DA: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp9A8=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp9A8->tag != 9)goto _LL3DC;else{_tmp9A9=_tmp9A8->f1;_tmp9AA=_tmp9A8->f2;}}_LL3DB:
# 5021
 Cyc_Toc_exptypes_to_c(_tmp9A9);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp9AA);goto _LL3B9;_LL3DC: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp9AB=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp9AB->tag != 13)goto _LL3DE;else{_tmp9AC=(void**)((void**)& _tmp9AB->f1);_tmp9AD=_tmp9AB->f2;}}_LL3DD:
*_tmp9AC=Cyc_Toc_typ_to_c(*_tmp9AC);Cyc_Toc_exptypes_to_c(_tmp9AD);goto _LL3B9;_LL3DE: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp9AE=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp9AE->tag != 24)goto _LL3E0;else{_tmp9AF=(void**)&(_tmp9AE->f1)->f3;_tmp9B0=_tmp9AE->f2;}}_LL3DF:
# 5024
*_tmp9AF=Cyc_Toc_typ_to_c(*_tmp9AF);
_tmp9B2=_tmp9B0;goto _LL3E1;_LL3E0: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp9B1=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp9B1->tag != 35)goto _LL3E2;else{_tmp9B2=_tmp9B1->f2;}}_LL3E1:
 _tmp9B4=_tmp9B2;goto _LL3E3;_LL3E2: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp9B3=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp9B3->tag != 25)goto _LL3E4;else{_tmp9B4=_tmp9B3->f1;}}_LL3E3:
# 5028
 for(0;_tmp9B4 != 0;_tmp9B4=_tmp9B4->tl){
struct _tuple19 _tmp9CC=*((struct _tuple19*)_tmp9B4->hd);struct Cyc_Absyn_Exp*_tmp9CE;struct _tuple19 _tmp9CD=_tmp9CC;_tmp9CE=_tmp9CD.f2;
Cyc_Toc_exptypes_to_c(_tmp9CE);}
# 5032
goto _LL3B9;_LL3E4: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp9B5=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp9B5->tag != 18)goto _LL3E6;else{_tmp9B6=(void**)((void**)& _tmp9B5->f1);}}_LL3E5:
 _tmp9B8=_tmp9B6;goto _LL3E7;_LL3E6: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp9B7=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp9B7->tag != 16)goto _LL3E8;else{_tmp9B8=(void**)((void**)& _tmp9B7->f1);}}_LL3E7:
*_tmp9B8=Cyc_Toc_typ_to_c(*_tmp9B8);goto _LL3B9;_LL3E8: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp9B9=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp9B9->tag != 36)goto _LL3EA;else{_tmp9BA=_tmp9B9->f1;}}_LL3E9:
 Cyc_Toc_stmttypes_to_c(_tmp9BA);goto _LL3B9;_LL3EA: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp9BB=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp9BB->tag != 33)goto _LL3EC;else{_tmp9BC=(struct Cyc_Absyn_MallocInfo*)& _tmp9BB->f1;}}_LL3EB:
# 5037
 if(_tmp9BC->elt_type != 0){
void**_tmpE57;_tmp9BC->elt_type=((_tmpE57=_cycalloc(sizeof(*_tmpE57)),((_tmpE57[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp9BC->elt_type))),_tmpE57))));}
Cyc_Toc_exptypes_to_c(_tmp9BC->num_elts);
goto _LL3B9;_LL3EC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp9BD=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp9BD->tag != 0)goto _LL3EE;}_LL3ED:
 goto _LL3EF;_LL3EE: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp9BE=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp9BE->tag != 1)goto _LL3F0;}_LL3EF:
 goto _LL3F1;_LL3F0: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp9BF=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp9BF->tag != 31)goto _LL3F2;}_LL3F1:
 goto _LL3F3;_LL3F2: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp9C0=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp9C0->tag != 39)goto _LL3F4;}_LL3F3:
 goto _LL3F5;_LL3F4: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp9C1=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp9C1->tag != 32)goto _LL3F6;}_LL3F5:
 goto _LL3B9;_LL3F6: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp9C2=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp9C2->tag != 29)goto _LL3F8;}_LL3F7:
# 5047
 goto _LL3F9;_LL3F8: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp9C3=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp9C3->tag != 30)goto _LL3FA;}_LL3F9:
 goto _LL3FB;_LL3FA: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp9C4=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp9C4->tag != 28)goto _LL3FC;}_LL3FB:
 goto _LL3FD;_LL3FC: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp9C5=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp9C5->tag != 26)goto _LL3FE;}_LL3FD:
 goto _LL3FF;_LL3FE: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp9C6=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp9C6->tag != 27)goto _LL400;}_LL3FF:
 goto _LL401;_LL400: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp9C7=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp9C7->tag != 23)goto _LL402;}_LL401:
 goto _LL403;_LL402: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp9C8=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp9C8->tag != 12)goto _LL404;}_LL403:
 goto _LL405;_LL404: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp9C9=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp9C9->tag != 15)goto _LL406;}_LL405:
 goto _LL407;_LL406: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp9CA=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp9CA->tag != 38)goto _LL408;}_LL407:
 goto _LL409;_LL408: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp9CB=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp97F;if(_tmp9CB->tag != 37)goto _LL3B9;}_LL409:
# 5057
{const char*_tmpE5A;void*_tmpE59;(_tmpE59=0,Cyc_Tcutil_terr(e->loc,((_tmpE5A="Cyclone expression within C code",_tag_dyneither(_tmpE5A,sizeof(char),33))),_tag_dyneither(_tmpE59,sizeof(void*),0)));}
goto _LL3B9;_LL3B9:;}
# 5062
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp9D2=d->r;void*_tmp9D3=_tmp9D2;struct Cyc_Absyn_Vardecl*_tmp9D5;struct Cyc_Absyn_Fndecl*_tmp9D7;struct Cyc_Absyn_Aggrdecl*_tmp9D9;struct Cyc_Absyn_Enumdecl*_tmp9DB;struct Cyc_Absyn_Typedefdecl*_tmp9DD;_LL40B: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp9D4=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp9D3;if(_tmp9D4->tag != 0)goto _LL40D;else{_tmp9D5=_tmp9D4->f1;}}_LL40C:
# 5065
 _tmp9D5->type=Cyc_Toc_typ_to_c(_tmp9D5->type);
if(_tmp9D5->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9D5->initializer));
goto _LL40A;_LL40D: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp9D6=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp9D3;if(_tmp9D6->tag != 1)goto _LL40F;else{_tmp9D7=_tmp9D6->f1;}}_LL40E:
# 5069
 _tmp9D7->ret_type=Cyc_Toc_typ_to_c(_tmp9D7->ret_type);
{struct Cyc_List_List*_tmp9E8=_tmp9D7->args;for(0;_tmp9E8 != 0;_tmp9E8=_tmp9E8->tl){
(*((struct _tuple9*)_tmp9E8->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp9E8->hd)).f3);}}
# 5073
goto _LL40A;_LL40F: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp9D8=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp9D3;if(_tmp9D8->tag != 5)goto _LL411;else{_tmp9D9=_tmp9D8->f1;}}_LL410:
 Cyc_Toc_aggrdecl_to_c(_tmp9D9,1);goto _LL40A;_LL411: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp9DA=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp9D3;if(_tmp9DA->tag != 7)goto _LL413;else{_tmp9DB=_tmp9DA->f1;}}_LL412:
# 5076
 if(_tmp9DB->fields != 0){
struct Cyc_List_List*_tmp9E9=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp9DB->fields))->v;for(0;_tmp9E9 != 0;_tmp9E9=_tmp9E9->tl){
struct Cyc_Absyn_Enumfield*_tmp9EA=(struct Cyc_Absyn_Enumfield*)_tmp9E9->hd;
if(_tmp9EA->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9EA->tag));}}
# 5081
goto _LL40A;_LL413: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp9DC=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp9D3;if(_tmp9DC->tag != 8)goto _LL415;else{_tmp9DD=_tmp9DC->f1;}}_LL414:
 _tmp9DD->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp9DD->defn));goto _LL40A;_LL415: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp9DE=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp9D3;if(_tmp9DE->tag != 2)goto _LL417;}_LL416:
 goto _LL418;_LL417: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp9DF=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp9D3;if(_tmp9DF->tag != 3)goto _LL419;}_LL418:
 goto _LL41A;_LL419: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp9E0=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp9D3;if(_tmp9E0->tag != 6)goto _LL41B;}_LL41A:
 goto _LL41C;_LL41B: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp9E1=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp9D3;if(_tmp9E1->tag != 9)goto _LL41D;}_LL41C:
 goto _LL41E;_LL41D: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp9E2=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp9D3;if(_tmp9E2->tag != 10)goto _LL41F;}_LL41E:
 goto _LL420;_LL41F: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp9E3=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp9D3;if(_tmp9E3->tag != 11)goto _LL421;}_LL420:
 goto _LL422;_LL421: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp9E4=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp9D3;if(_tmp9E4->tag != 12)goto _LL423;}_LL422:
 goto _LL424;_LL423: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp9E5=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp9D3;if(_tmp9E5->tag != 4)goto _LL425;}_LL424:
# 5092
{const char*_tmpE5D;void*_tmpE5C;(_tmpE5C=0,Cyc_Tcutil_terr(d->loc,((_tmpE5D="Cyclone declaration within C code",_tag_dyneither(_tmpE5D,sizeof(char),34))),_tag_dyneither(_tmpE5C,sizeof(void*),0)));}
goto _LL40A;_LL425: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp9E6=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp9D3;if(_tmp9E6->tag != 13)goto _LL427;}_LL426:
 goto _LL428;_LL427: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp9E7=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp9D3;if(_tmp9E7->tag != 14)goto _LL40A;}_LL428:
# 5096
 goto _LL40A;_LL40A:;}
# 5100
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp9ED=s->r;void*_tmp9EE=_tmp9ED;struct Cyc_Absyn_Exp*_tmp9F0;struct Cyc_Absyn_Stmt*_tmp9F2;struct Cyc_Absyn_Stmt*_tmp9F3;struct Cyc_Absyn_Exp*_tmp9F5;struct Cyc_Absyn_Exp*_tmp9F7;struct Cyc_Absyn_Stmt*_tmp9F8;struct Cyc_Absyn_Stmt*_tmp9F9;struct Cyc_Absyn_Exp*_tmp9FB;struct Cyc_Absyn_Stmt*_tmp9FC;struct Cyc_Absyn_Exp*_tmp9FE;struct Cyc_Absyn_Exp*_tmp9FF;struct Cyc_Absyn_Exp*_tmpA00;struct Cyc_Absyn_Stmt*_tmpA01;struct Cyc_Absyn_Exp*_tmpA03;struct Cyc_List_List*_tmpA04;struct Cyc_Absyn_Decl*_tmpA06;struct Cyc_Absyn_Stmt*_tmpA07;struct Cyc_Absyn_Stmt*_tmpA09;struct Cyc_Absyn_Exp*_tmpA0A;struct Cyc_Absyn_Stmt*_tmpA0C;_LL42A: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp9EF=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp9EE;if(_tmp9EF->tag != 1)goto _LL42C;else{_tmp9F0=_tmp9EF->f1;}}_LL42B:
 Cyc_Toc_exptypes_to_c(_tmp9F0);goto _LL429;_LL42C: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp9F1=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp9EE;if(_tmp9F1->tag != 2)goto _LL42E;else{_tmp9F2=_tmp9F1->f1;_tmp9F3=_tmp9F1->f2;}}_LL42D:
 Cyc_Toc_stmttypes_to_c(_tmp9F2);Cyc_Toc_stmttypes_to_c(_tmp9F3);goto _LL429;_LL42E: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp9F4=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp9EE;if(_tmp9F4->tag != 3)goto _LL430;else{_tmp9F5=_tmp9F4->f1;}}_LL42F:
 if(_tmp9F5 != 0)Cyc_Toc_exptypes_to_c(_tmp9F5);goto _LL429;_LL430: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp9F6=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp9EE;if(_tmp9F6->tag != 4)goto _LL432;else{_tmp9F7=_tmp9F6->f1;_tmp9F8=_tmp9F6->f2;_tmp9F9=_tmp9F6->f3;}}_LL431:
# 5106
 Cyc_Toc_exptypes_to_c(_tmp9F7);Cyc_Toc_stmttypes_to_c(_tmp9F8);Cyc_Toc_stmttypes_to_c(_tmp9F9);goto _LL429;_LL432: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp9FA=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp9EE;if(_tmp9FA->tag != 5)goto _LL434;else{_tmp9FB=(_tmp9FA->f1).f1;_tmp9FC=_tmp9FA->f2;}}_LL433:
# 5108
 Cyc_Toc_exptypes_to_c(_tmp9FB);Cyc_Toc_stmttypes_to_c(_tmp9FC);goto _LL429;_LL434: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp9FD=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp9EE;if(_tmp9FD->tag != 9)goto _LL436;else{_tmp9FE=_tmp9FD->f1;_tmp9FF=(_tmp9FD->f2).f1;_tmpA00=(_tmp9FD->f3).f1;_tmpA01=_tmp9FD->f4;}}_LL435:
# 5110
 Cyc_Toc_exptypes_to_c(_tmp9FE);Cyc_Toc_exptypes_to_c(_tmp9FF);Cyc_Toc_exptypes_to_c(_tmpA00);
Cyc_Toc_stmttypes_to_c(_tmpA01);goto _LL429;_LL436: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmpA02=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp9EE;if(_tmpA02->tag != 10)goto _LL438;else{_tmpA03=_tmpA02->f1;_tmpA04=_tmpA02->f2;}}_LL437:
# 5113
 Cyc_Toc_exptypes_to_c(_tmpA03);
for(0;_tmpA04 != 0;_tmpA04=_tmpA04->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmpA04->hd)->body);}
goto _LL429;_LL438: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpA05=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp9EE;if(_tmpA05->tag != 12)goto _LL43A;else{_tmpA06=_tmpA05->f1;_tmpA07=_tmpA05->f2;}}_LL439:
 Cyc_Toc_decltypes_to_c(_tmpA06);Cyc_Toc_stmttypes_to_c(_tmpA07);goto _LL429;_LL43A: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmpA08=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp9EE;if(_tmpA08->tag != 14)goto _LL43C;else{_tmpA09=_tmpA08->f1;_tmpA0A=(_tmpA08->f2).f1;}}_LL43B:
 Cyc_Toc_stmttypes_to_c(_tmpA09);Cyc_Toc_exptypes_to_c(_tmpA0A);goto _LL429;_LL43C: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpA0B=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp9EE;if(_tmpA0B->tag != 13)goto _LL43E;else{_tmpA0C=_tmpA0B->f2;}}_LL43D:
 Cyc_Toc_stmttypes_to_c(_tmpA0C);goto _LL429;_LL43E: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmpA0D=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp9EE;if(_tmpA0D->tag != 0)goto _LL440;}_LL43F:
 goto _LL441;_LL440: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmpA0E=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp9EE;if(_tmpA0E->tag != 6)goto _LL442;}_LL441:
 goto _LL443;_LL442: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmpA0F=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp9EE;if(_tmpA0F->tag != 7)goto _LL444;}_LL443:
 goto _LL445;_LL444: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmpA10=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp9EE;if(_tmpA10->tag != 8)goto _LL446;}_LL445:
# 5123
 goto _LL429;_LL446: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmpA11=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp9EE;if(_tmpA11->tag != 11)goto _LL448;}_LL447:
# 5126
{struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct _tmpE60;struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmpE5F;s->r=(void*)((_tmpE5F=_cycalloc_atomic(sizeof(*_tmpE5F)),((_tmpE5F[0]=((_tmpE60.tag=0,_tmpE60)),_tmpE5F))));}
goto _LL429;_LL448: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmpA12=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp9EE;if(_tmpA12->tag != 15)goto _LL44A;}_LL449:
 goto _LL44B;_LL44A: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmpA13=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp9EE;if(_tmpA13->tag != 16)goto _LL429;}_LL44B:
# 5130
{struct Cyc_String_pa_PrintArg_struct _tmpE68;void*_tmpE67[1];const char*_tmpE66;void*_tmpE65;(_tmpE65=0,Cyc_Tcutil_terr(s->loc,(struct _dyneither_ptr)((_tmpE68.tag=0,((_tmpE68.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpE67[0]=& _tmpE68,Cyc_aprintf(((_tmpE66="Cyclone statement in C code: %s",_tag_dyneither(_tmpE66,sizeof(char),32))),_tag_dyneither(_tmpE67,sizeof(void*),1)))))))),_tag_dyneither(_tmpE65,sizeof(void*),0)));}
goto _LL429;_LL429:;}
# 5139
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv)){
const char*_tmpE6B;void*_tmpE6A;(_tmpE6A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE6B="decls_to_c: not at toplevel!",_tag_dyneither(_tmpE6B,sizeof(char),29))),_tag_dyneither(_tmpE6A,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmpA1C=d->r;void*_tmpA1D=_tmpA1C;struct Cyc_Absyn_Vardecl*_tmpA1F;struct Cyc_Absyn_Fndecl*_tmpA21;struct Cyc_Absyn_Aggrdecl*_tmpA26;struct Cyc_Absyn_Datatypedecl*_tmpA28;struct Cyc_Absyn_Enumdecl*_tmpA2A;struct Cyc_Absyn_Typedefdecl*_tmpA2C;struct Cyc_List_List*_tmpA30;struct Cyc_List_List*_tmpA32;struct Cyc_List_List*_tmpA34;struct Cyc_List_List*_tmpA36;_LL44D: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpA1E=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpA1D;if(_tmpA1E->tag != 0)goto _LL44F;else{_tmpA1F=_tmpA1E->f1;}}_LL44E: {
# 5146
struct _tuple1*_tmpA37=_tmpA1F->name;
# 5148
if(_tmpA1F->sc == Cyc_Absyn_ExternC){
struct _tuple1*_tmpE6C;_tmpA37=((_tmpE6C=_cycalloc(sizeof(*_tmpE6C)),((_tmpE6C->f1=Cyc_Absyn_Rel_n(0),((_tmpE6C->f2=(*_tmpA37).f2,_tmpE6C))))));}
if(_tmpA1F->initializer != 0){
if(_tmpA1F->sc == Cyc_Absyn_ExternC)_tmpA1F->sc=Cyc_Absyn_Public;
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmpA1F->initializer));else{
# 5155
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmpA1F->initializer));}}
# 5157
{struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmpE6F;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpE6E;nv=Cyc_Toc_add_varmap(r,nv,_tmpA1F->name,Cyc_Absyn_varb_exp(_tmpA37,(void*)((_tmpE6E=_cycalloc(sizeof(*_tmpE6E)),((_tmpE6E[0]=((_tmpE6F.tag=1,((_tmpE6F.f1=_tmpA1F,_tmpE6F)))),_tmpE6E)))),0));}
_tmpA1F->name=_tmpA37;
_tmpA1F->sc=Cyc_Toc_scope_to_c(_tmpA1F->sc);
_tmpA1F->type=Cyc_Toc_typ_to_c(_tmpA1F->type);
{struct Cyc_List_List*_tmpE70;Cyc_Toc_result_decls=((_tmpE70=_cycalloc(sizeof(*_tmpE70)),((_tmpE70->hd=d,((_tmpE70->tl=Cyc_Toc_result_decls,_tmpE70))))));}
goto _LL44C;}_LL44F: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpA20=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpA1D;if(_tmpA20->tag != 1)goto _LL451;else{_tmpA21=_tmpA20->f1;}}_LL450: {
# 5164
struct _tuple1*_tmpA3C=_tmpA21->name;
# 5166
if(_tmpA21->sc == Cyc_Absyn_ExternC){
{struct _tuple1*_tmpE71;_tmpA3C=((_tmpE71=_cycalloc(sizeof(*_tmpE71)),((_tmpE71->f1=Cyc_Absyn_Abs_n(0,1),((_tmpE71->f2=(*_tmpA3C).f2,_tmpE71))))));}
_tmpA21->sc=Cyc_Absyn_Public;}
# 5170
nv=Cyc_Toc_add_varmap(r,nv,_tmpA21->name,Cyc_Absyn_var_exp(_tmpA3C,0));
_tmpA21->name=_tmpA3C;
Cyc_Toc_fndecl_to_c(nv,_tmpA21,cinclude);
{struct Cyc_List_List*_tmpE72;Cyc_Toc_result_decls=((_tmpE72=_cycalloc(sizeof(*_tmpE72)),((_tmpE72->hd=d,((_tmpE72->tl=Cyc_Toc_result_decls,_tmpE72))))));}
goto _LL44C;}_LL451: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmpA22=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpA1D;if(_tmpA22->tag != 2)goto _LL453;}_LL452:
 goto _LL454;_LL453: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmpA23=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmpA1D;if(_tmpA23->tag != 3)goto _LL455;}_LL454: {
# 5177
const char*_tmpE75;void*_tmpE74;(_tmpE74=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE75="letdecl at toplevel",_tag_dyneither(_tmpE75,sizeof(char),20))),_tag_dyneither(_tmpE74,sizeof(void*),0)));}_LL455: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmpA24=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpA1D;if(_tmpA24->tag != 4)goto _LL457;}_LL456: {
# 5179
const char*_tmpE78;void*_tmpE77;(_tmpE77=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE78="region decl at toplevel",_tag_dyneither(_tmpE78,sizeof(char),24))),_tag_dyneither(_tmpE77,sizeof(void*),0)));}_LL457: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpA25=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpA1D;if(_tmpA25->tag != 5)goto _LL459;else{_tmpA26=_tmpA25->f1;}}_LL458:
# 5183
 Cyc_Toc_aggrdecl_to_c(_tmpA26,1);
goto _LL44C;_LL459: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmpA27=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmpA1D;if(_tmpA27->tag != 6)goto _LL45B;else{_tmpA28=_tmpA27->f1;}}_LL45A:
# 5186
 if(_tmpA28->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmpA28);else{
# 5189
Cyc_Toc_datatypedecl_to_c(_tmpA28);}
goto _LL44C;_LL45B: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmpA29=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpA1D;if(_tmpA29->tag != 7)goto _LL45D;else{_tmpA2A=_tmpA29->f1;}}_LL45C:
# 5192
 Cyc_Toc_enumdecl_to_c(_tmpA2A);
{struct Cyc_List_List*_tmpE79;Cyc_Toc_result_decls=((_tmpE79=_cycalloc(sizeof(*_tmpE79)),((_tmpE79->hd=d,((_tmpE79->tl=Cyc_Toc_result_decls,_tmpE79))))));}
goto _LL44C;_LL45D: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmpA2B=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpA1D;if(_tmpA2B->tag != 8)goto _LL45F;else{_tmpA2C=_tmpA2B->f1;}}_LL45E:
# 5196
 _tmpA2C->tvs=0;
if(_tmpA2C->defn != 0)
_tmpA2C->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmpA2C->defn));else{
# 5200
enum Cyc_Absyn_KindQual _tmpA44=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmpA2C->kind))->v)->kind;switch(_tmpA44){case Cyc_Absyn_BoxKind: _LL46B:
 _tmpA2C->defn=Cyc_Absyn_void_star_typ();break;default: _LL46C:
 _tmpA2C->defn=(void*)& Cyc_Absyn_VoidType_val;break;}}
# 5205
{struct Cyc_List_List*_tmpE7A;Cyc_Toc_result_decls=((_tmpE7A=_cycalloc(sizeof(*_tmpE7A)),((_tmpE7A->hd=d,((_tmpE7A->tl=Cyc_Toc_result_decls,_tmpE7A))))));}
goto _LL44C;_LL45F: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmpA2D=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmpA1D;if(_tmpA2D->tag != 13)goto _LL461;}_LL460:
 goto _LL462;_LL461: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmpA2E=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmpA1D;if(_tmpA2E->tag != 14)goto _LL463;}_LL462:
# 5209
 goto _LL44C;_LL463: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpA2F=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpA1D;if(_tmpA2F->tag != 9)goto _LL465;else{_tmpA30=_tmpA2F->f2;}}_LL464:
 _tmpA32=_tmpA30;goto _LL466;_LL465: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmpA31=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpA1D;if(_tmpA31->tag != 10)goto _LL467;else{_tmpA32=_tmpA31->f2;}}_LL466:
 _tmpA34=_tmpA32;goto _LL468;_LL467: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmpA33=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmpA1D;if(_tmpA33->tag != 11)goto _LL469;else{_tmpA34=_tmpA33->f1;}}_LL468:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmpA34,top,cinclude);goto _LL44C;_LL469: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA35=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmpA1D;if(_tmpA35->tag != 12)goto _LL44C;else{_tmpA36=_tmpA35->f1;}}_LL46A:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmpA36,top,1);goto _LL44C;_LL44C:;};}
# 5216
return nv;}
# 5220
static void Cyc_Toc_init(){
struct Cyc_Core_NewDynamicRegion _tmpA46=Cyc_Core_new_rckey();struct Cyc_Core_DynamicRegion*_tmpA48;struct Cyc_Core_NewDynamicRegion _tmpA47=_tmpA46;_tmpA48=_tmpA47.key;{
struct Cyc_Toc_TocState*_tmpA49=((struct Cyc_Toc_TocState*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Toc_TocState*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmpA48,0,Cyc_Toc_empty_toc_state);
{struct Cyc_Toc_TocStateWrap*_tmpE7B;Cyc_Toc_toc_state=((_tmpE7B=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE7B)),((_tmpE7B->dyn=(struct Cyc_Core_DynamicRegion*)_tmpA48,((_tmpE7B->state=(struct Cyc_Toc_TocState*)_tmpA49,_tmpE7B))))));}
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0;
Cyc_Toc_bounds_checks_eliminated=0;{
struct _dyneither_ptr**_tmpE7C;Cyc_Toc_globals=_tag_dyneither(((_tmpE7C=_cycalloc(sizeof(struct _dyneither_ptr*)* 42),((_tmpE7C[0]=(struct _dyneither_ptr*)& Cyc_Toc__throw_str,((_tmpE7C[1]=(struct _dyneither_ptr*)& Cyc_Toc_setjmp_str,((_tmpE7C[2]=(struct _dyneither_ptr*)& Cyc_Toc__push_handler_str,((_tmpE7C[3]=(struct _dyneither_ptr*)& Cyc_Toc__pop_handler_str,((_tmpE7C[4]=(struct _dyneither_ptr*)& Cyc_Toc__exn_thrown_str,((_tmpE7C[5]=(struct _dyneither_ptr*)& Cyc_Toc__npop_handler_str,((_tmpE7C[6]=(struct _dyneither_ptr*)& Cyc_Toc__check_null_str,((_tmpE7C[7]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_null_str,((_tmpE7C[8]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_notnull_str,((_tmpE7C[9]=(struct _dyneither_ptr*)& Cyc_Toc__check_dyneither_subscript_str,((_tmpE7C[10]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_str,((_tmpE7C[11]=(struct _dyneither_ptr*)& Cyc_Toc__tag_dyneither_str,((_tmpE7C[12]=(struct _dyneither_ptr*)& Cyc_Toc__init_dyneither_ptr_str,((_tmpE7C[13]=(struct _dyneither_ptr*)& Cyc_Toc__untag_dyneither_ptr_str,((_tmpE7C[14]=(struct _dyneither_ptr*)& Cyc_Toc__get_dyneither_size_str,((_tmpE7C[15]=(struct _dyneither_ptr*)& Cyc_Toc__get_zero_arr_size_str,((_tmpE7C[16]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_plus_str,((_tmpE7C[17]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_plus_str,((_tmpE7C[18]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpE7C[19]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_str,((_tmpE7C[20]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpE7C[21]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_post_str,((_tmpE7C[22]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_str,((_tmpE7C[23]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_str,((_tmpE7C[24]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_atomic_str,((_tmpE7C[25]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_atomic_str,((_tmpE7C[26]=(struct _dyneither_ptr*)& Cyc_Toc__region_malloc_str,((_tmpE7C[27]=(struct _dyneither_ptr*)& Cyc_Toc__region_calloc_str,((_tmpE7C[28]=(struct _dyneither_ptr*)& Cyc_Toc__check_times_str,((_tmpE7C[29]=(struct _dyneither_ptr*)& Cyc_Toc__new_region_str,((_tmpE7C[30]=(struct _dyneither_ptr*)& Cyc_Toc__push_region_str,((_tmpE7C[31]=(struct _dyneither_ptr*)& Cyc_Toc__pop_region_str,((_tmpE7C[32]=(struct _dyneither_ptr*)& Cyc_Toc__open_dynregion_str,((_tmpE7C[33]=(struct _dyneither_ptr*)& Cyc_Toc__push_dynregion_str,((_tmpE7C[34]=(struct _dyneither_ptr*)& Cyc_Toc__pop_dynregion_str,((_tmpE7C[35]=(struct _dyneither_ptr*)& Cyc_Toc__reset_region_str,((_tmpE7C[36]=(struct _dyneither_ptr*)& Cyc_Toc__throw_arraybounds_str,((_tmpE7C[37]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_decrease_size_str,((_tmpE7C[38]=(struct _dyneither_ptr*)& Cyc_Toc__throw_match_str,((_tmpE7C[39]=(struct _dyneither_ptr*)& Cyc_Toc__swap_word_str,((_tmpE7C[40]=(struct _dyneither_ptr*)& Cyc_Toc__swap_dyneither_str,((_tmpE7C[41]=(struct _dyneither_ptr*)& Cyc_Toc__fast_region_malloc_str,_tmpE7C)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),sizeof(struct _dyneither_ptr*),42);};};}
# 5277
static int Cyc_Toc_destroy_labels(struct _RegionHandle*d,struct Cyc_Toc_TocState*ts){
struct Cyc_Toc_TocState _tmpA4C=*ts;struct Cyc_Xarray_Xarray*_tmpA4E;struct Cyc_Toc_TocState _tmpA4D=_tmpA4C;_tmpA4E=_tmpA4D.temp_labels;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmpA4E);
return 0;}
# 5285
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){
Cyc_Toc_init();{
struct _RegionHandle _tmpA4F=_new_region("start");struct _RegionHandle*start=& _tmpA4F;_push_region(start);
Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(start),ds,1,0);{
# 5294
struct Cyc_Toc_TocStateWrap*ts=0;
_swap_word(& ts,& Cyc_Toc_toc_state);{
struct Cyc_Toc_TocStateWrap _tmpA50=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Core_DynamicRegion*_tmpA52;struct Cyc_Toc_TocState*_tmpA53;struct Cyc_Toc_TocStateWrap _tmpA51=_tmpA50;_tmpA52=_tmpA51.dyn;_tmpA53=_tmpA51.state;
((int(*)(struct Cyc_Core_DynamicRegion*key,struct Cyc_Toc_TocState*arg,int(*body)(struct _RegionHandle*h,struct Cyc_Toc_TocState*arg)))Cyc_Core_open_region)(_tmpA52,_tmpA53,Cyc_Toc_destroy_labels);
Cyc_Core_free_rckey(_tmpA52);
((void(*)(struct _dyneither_ptr ptr))Cyc_Core_ufree)(_tag_dyneither(ts,sizeof(struct Cyc_Toc_TocStateWrap),1));};};{
# 5301
struct Cyc_List_List*_tmpA54=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0);return _tmpA54;};
# 5287
;_pop_region(start);};}
