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
# 57
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _dyneither_ptr);
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
extern struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val;struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 749
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
# 777
extern struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 936
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 943
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 953
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 956
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
# 963
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 966
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 981
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 983
extern void*Cyc_Absyn_sint_typ;
# 995
void*Cyc_Absyn_exn_typ();
# 1006
extern void*Cyc_Absyn_bounds_one;
# 1014
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1020
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);
# 1022
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1025
void*Cyc_Absyn_void_star_typ();
# 1027
void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple1*name);
void*Cyc_Absyn_unionq_typ(struct _tuple1*name);
# 1033
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1037
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1039
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1045
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1050
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
# 1053
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple1*,void*,unsigned int);
# 1057
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1060
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1066
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1069
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1072
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1077
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1081
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1086
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
# 1091
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1096
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1102
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned int);
# 1108
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1118
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1121
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned int loc);
# 1125
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1127
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc);
# 1132
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1136
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1141
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
# 1184
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1187
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1197
struct _dyneither_ptr*Cyc_Absyn_fieldname(int);
# 1201
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1217
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
typedef void*Cyc_CfFlowInfo_killrgn_t;struct Cyc_Hashtable_Table;
# 35 "hashtable.h"
typedef struct Cyc_Hashtable_Table*Cyc_Hashtable_table_t;
# 52
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 32 "toc.h"
typedef struct Cyc_Hashtable_Table*Cyc_Toc_table_t;
# 35
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds);
# 39
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
# 84
struct Cyc_Hashtable_Table**Cyc_Toc_gpop_tables=0;
struct Cyc_Hashtable_Table**Cyc_Toc_fn_pop_table=0;
static int Cyc_Toc_get_npop(struct Cyc_Absyn_Stmt*s){
return((int(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_fn_pop_table)),s);}
# 94
static struct Cyc_List_List*Cyc_Toc_result_decls=0;struct Cyc_Toc_TocState{struct Cyc_List_List**tuple_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_List_List**abs_struct_types;struct Cyc_Set_Set**datatypes_so_far;struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*temp_labels;};struct _tuple14{struct _tuple1*f1;struct _tuple1*f2;};
# 114
int Cyc_Toc_qvar_tag_cmp(struct _tuple14*x,struct _tuple14*y){
struct _tuple14 _tmp0=*x;struct _tuple14 _tmp1=_tmp0;struct _tuple1*_tmp2;struct _tuple1*_tmp3;_LL1: _tmp2=_tmp1.f1;_tmp3=_tmp1.f2;_LL2:;{
struct _tuple14 _tmp4=*y;struct _tuple14 _tmp5=_tmp4;struct _tuple1*_tmp6;struct _tuple1*_tmp7;_LL4: _tmp6=_tmp5.f1;_tmp7=_tmp5.f2;_LL5:;{
int i=Cyc_Absyn_qvar_cmp(_tmp2,_tmp6);
if(i != 0)return i;
return Cyc_Absyn_qvar_cmp(_tmp3,_tmp7);};};}
# 123
static struct Cyc_Toc_TocState*Cyc_Toc_empty_toc_state(struct _RegionHandle*d,int unused){
return({struct Cyc_Toc_TocState*_tmp8=_region_malloc(d,sizeof(*_tmp8));_tmp8->tuple_types=({struct Cyc_List_List**_tmpE=_region_malloc(d,sizeof(*_tmpE));_tmpE[0]=0;_tmpE;});_tmp8->aggrs_so_far=({struct Cyc_Dict_Dict*_tmpD=_region_malloc(d,sizeof(*_tmpD));_tmpD[0]=
# 126
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);_tmpD;});_tmp8->abs_struct_types=({struct Cyc_List_List**_tmpC=_region_malloc(d,sizeof(*_tmpC));_tmpC[0]=0;_tmpC;});_tmp8->datatypes_so_far=({struct Cyc_Set_Set**_tmpB=_region_malloc(d,sizeof(*_tmpB));_tmpB[0]=
# 128
((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp);_tmpB;});_tmp8->xdatatypes_so_far=({struct Cyc_Dict_Dict*_tmpA=_region_malloc(d,sizeof(*_tmpA));_tmpA[0]=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);_tmpA;});_tmp8->qvar_tags=({struct Cyc_Dict_Dict*_tmp9=_region_malloc(d,sizeof(*_tmp9));_tmp9[0]=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp);_tmp9;});_tmp8->temp_labels=
((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d);_tmp8;});}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 140
typedef struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state_t;
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple15{struct Cyc_Toc_TocState*f1;void*f2;};
# 147
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple15*)){
# 150
struct Cyc_Toc_TocStateWrap*ts=0;
_swap_word(& ts,& Cyc_Toc_toc_state);{
struct Cyc_Toc_TocStateWrap _tmpF=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp10=_tmpF;struct Cyc_Core_DynamicRegion*_tmp11;struct Cyc_Toc_TocState*_tmp12;_LL7: _tmp11=_tmp10.dyn;_tmp12=_tmp10.state;_LL8:;{
struct _dyneither_ptr _tmp13=((struct _dyneither_ptr(*)(struct _dyneither_ptr ptr))Cyc_Core_alias_refptr)(_tag_dyneither(_tmp11,sizeof(struct Cyc_Core_DynamicRegion),1));
*ts=({struct Cyc_Toc_TocStateWrap _tmp14;_tmp14.dyn=(struct Cyc_Core_DynamicRegion*)_tmp11;_tmp14.state=(struct Cyc_Toc_TocState*)_tmp12;_tmp14;});
_swap_word(& ts,& Cyc_Toc_toc_state);{
struct _tuple15 _tmp15=({struct _tuple15 _tmp17;_tmp17.f1=_tmp12;_tmp17.f2=arg;_tmp17;});
void*_tmp16=((void*(*)(struct Cyc_Core_DynamicRegion*key,struct _tuple15*arg,void*(*body)(struct _RegionHandle*h,struct _tuple15*arg)))Cyc_Core_open_region)((struct Cyc_Core_DynamicRegion*)_untag_dyneither_ptr(_tmp13,sizeof(struct Cyc_Core_DynamicRegion),1),& _tmp15,f);
Cyc_Core_free_rckey((struct Cyc_Core_DynamicRegion*)_untag_dyneither_ptr(_tmp13,sizeof(struct Cyc_Core_DynamicRegion),1));
return _tmp16;};};};}struct _tuple16{struct _tuple1*f1;void*(*f2)(struct _tuple1*);};struct _tuple17{struct Cyc_Toc_TocState*f1;struct _tuple16*f2;};struct _tuple18{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
# 162
static void*Cyc_Toc_aggrdecl_type_body(struct _RegionHandle*d,struct _tuple17*env){
# 165
struct _tuple17 _tmp18=*env;struct _tuple17 _tmp19=_tmp18;struct Cyc_Toc_TocState*_tmp1A;struct _tuple1*_tmp1B;void*(*_tmp1C)(struct _tuple1*);_LLA: _tmp1A=_tmp19.f1;_tmp1B=(_tmp19.f2)->f1;_tmp1C=(_tmp19.f2)->f2;_LLB:;{
struct _tuple18**v=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp1A->aggrs_so_far,_tmp1B);
if(v == 0)
return _tmp1C(_tmp1B);else{
# 170
struct _tuple18*_tmp1D=*v;struct _tuple18*_tmp1E=_tmp1D;void*_tmp1F;_LLD: _tmp1F=_tmp1E->f2;_LLE:;
return _tmp1F;}};}
# 175
void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){
struct _tuple16 env=({struct _tuple16 _tmp20;_tmp20.f1=q;_tmp20.f2=type_maker;_tmp20;});
return((void*(*)(struct _tuple16*arg,void*(*f)(struct _RegionHandle*,struct _tuple17*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_aggrdecl_type_body);}
# 180
static int Cyc_Toc_tuple_type_counter=0;
static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;char Cyc_Toc_Toc_Unimplemented[18]="Toc_Unimplemented";struct Cyc_Toc_Toc_Unimplemented_exn_struct{char*tag;};char Cyc_Toc_Toc_Impossible[15]="Toc_Impossible";struct Cyc_Toc_Toc_Impossible_exn_struct{char*tag;};
# 187
static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 190
Cyc_vfprintf(Cyc_stderr,fmt,ap);
({void*_tmp21=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp22="\n";_tag_dyneither(_tmp22,sizeof(char),2);}),_tag_dyneither(_tmp21,sizeof(void*),0));});
Cyc_fflush(Cyc_stderr);
(int)_throw((void*)({struct Cyc_Toc_Toc_Unimplemented_exn_struct*_tmp23=_cycalloc_atomic(sizeof(*_tmp23));_tmp23[0]=({struct Cyc_Toc_Toc_Unimplemented_exn_struct _tmp24;_tmp24.tag=Cyc_Toc_Toc_Unimplemented;_tmp24;});_tmp23;}));}
# 195
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 198
Cyc_vfprintf(Cyc_stderr,fmt,ap);
({void*_tmp25=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp26="\n";_tag_dyneither(_tmp26,sizeof(char),2);}),_tag_dyneither(_tmp25,sizeof(void*),0));});
Cyc_fflush(Cyc_stderr);
(int)_throw((void*)({struct Cyc_Toc_Toc_Impossible_exn_struct*_tmp27=_cycalloc_atomic(sizeof(*_tmp27));_tmp27[0]=({struct Cyc_Toc_Toc_Impossible_exn_struct _tmp28;_tmp28.tag=Cyc_Toc_Toc_Impossible;_tmp28;});_tmp27;}));}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_exn_struct{char*tag;};static char _tmp29[5]="curr";
# 213 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc_curr_string={_tmp29,_tmp29,_tmp29 + 5};static struct _dyneither_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp2A[4]="tag";
static struct _dyneither_ptr Cyc_Toc_tag_string={_tmp2A,_tmp2A,_tmp2A + 4};static struct _dyneither_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp2B[4]="val";
static struct _dyneither_ptr Cyc_Toc_val_string={_tmp2B,_tmp2B,_tmp2B + 4};static struct _dyneither_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp2C[14]="_handler_cons";
static struct _dyneither_ptr Cyc_Toc__handler_cons_string={_tmp2C,_tmp2C,_tmp2C + 14};static struct _dyneither_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;static char _tmp2D[8]="handler";
static struct _dyneither_ptr Cyc_Toc_handler_string={_tmp2D,_tmp2D,_tmp2D + 8};static struct _dyneither_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp2E[14]="_RegionHandle";
static struct _dyneither_ptr Cyc_Toc__RegionHandle_string={_tmp2E,_tmp2E,_tmp2E + 14};static struct _dyneither_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;static char _tmp2F[17]="_DynRegionHandle";
static struct _dyneither_ptr Cyc_Toc__DynRegionHandle_string={_tmp2F,_tmp2F,_tmp2F + 17};static struct _dyneither_ptr*Cyc_Toc__DynRegionHandle_sp=& Cyc_Toc__DynRegionHandle_string;static char _tmp30[16]="_DynRegionFrame";
static struct _dyneither_ptr Cyc_Toc__DynRegionFrame_string={_tmp30,_tmp30,_tmp30 + 16};static struct _dyneither_ptr*Cyc_Toc__DynRegionFrame_sp=& Cyc_Toc__DynRegionFrame_string;
# 226
struct _dyneither_ptr Cyc_Toc_globals={(void*)0,(void*)0,(void*)(0 + 0)};static char _tmp31[7]="_throw";
# 235 "toc.cyc"
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
# 310
static struct Cyc_Absyn_AggrType_Absyn_Type_struct Cyc_Toc_dyneither_ptr_typ_v={11,{{.UnknownAggr={1,{Cyc_Absyn_StructA,& Cyc_Toc__dyneither_ptr_pr,0}}},0}};
# 313
static void*Cyc_Toc_dyneither_ptr_typ=(void*)& Cyc_Toc_dyneither_ptr_typ_v;
# 315
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0};
# 317
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
return Cyc_Absyn_skip_stmt(0);}
# 323
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
void*_tmpC0=e->r;void*_tmpC1=_tmpC0;struct Cyc_Absyn_Exp*_tmpC2;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpC1)->tag == 13){if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpC1)->f4 == Cyc_Absyn_No_coercion){_LL10: _tmpC2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpC1)->f2;_LL11:
 return Cyc_Toc_cast_it(t,_tmpC2);}else{goto _LL12;}}else{_LL12: _LL13:
 return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}_LLF:;}
# 329
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3[0]=({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmpC4;_tmpC4.tag=13;_tmpC4.f1=t;_tmpC4.f2=e;_tmpC4.f3=0;_tmpC4.f4=Cyc_Absyn_No_coercion;_tmpC4;});_tmpC3;});}
# 332
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5[0]=({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmpC6;_tmpC6.tag=19;_tmpC6.f1=e;_tmpC6;});_tmpC5;});}
# 335
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7[0]=({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmpC8;_tmpC8.tag=22;_tmpC8.f1=e1;_tmpC8.f2=e2;_tmpC8;});_tmpC7;});}
# 338
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
return(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9[0]=({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmpCA;_tmpCA.tag=36;_tmpCA.f1=s;_tmpCA;});_tmpC9;});}
# 341
static void*Cyc_Toc_sizeoftyp_exp_r(void*t){
return(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB[0]=({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpCC;_tmpCC.tag=16;_tmpCC.f1=t;_tmpCC;});_tmpCB;});}
# 344
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
return(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD[0]=({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpCE;_tmpCE.tag=9;_tmpCE.f1=e;_tmpCE.f2=es;_tmpCE.f3=0;_tmpCE.f4=1;_tmpCE;});_tmpCD;});}
# 347
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF[0]=({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmpD0;_tmpD0.tag=1;_tmpD0.f1=e;_tmpD0;});_tmpCF;});}
# 350
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
return(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpD1=_cycalloc(sizeof(*_tmpD1));_tmpD1[0]=({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmpD2;_tmpD2.tag=2;_tmpD2.f1=s1;_tmpD2.f2=s2;_tmpD2;});_tmpD1;});}
# 353
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
return(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpD3=_cycalloc(sizeof(*_tmpD3));_tmpD3[0]=({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmpD4;_tmpD4.tag=5;_tmpD4.f1=e1;_tmpD4.f2=e2;_tmpD4.f3=e3;_tmpD4;});_tmpD3;});}
# 356
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpD5=_cycalloc(sizeof(*_tmpD5));_tmpD5[0]=({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpD6;_tmpD6.tag=20;_tmpD6.f1=e;_tmpD6.f2=n;_tmpD6.f3=0;_tmpD6.f4=0;_tmpD6;});_tmpD5;});}
# 359
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpD7=_cycalloc(sizeof(*_tmpD7));_tmpD7[0]=({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpD8;_tmpD8.tag=21;_tmpD8.f1=e;_tmpD8.f2=n;_tmpD8.f3=0;_tmpD8.f4=0;_tmpD8;});_tmpD7;});}
# 362
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 365
return(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpD9=_cycalloc(sizeof(*_tmpD9));_tmpD9[0]=({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpDA;_tmpDA.tag=35;_tmpDA.f1=tdopt;_tmpDA.f2=ds;_tmpDA;});_tmpD9;});}
# 367
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
return(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmpDB=_cycalloc(sizeof(*_tmpDB));_tmpDB[0]=({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmpDC;_tmpDC.tag=8;_tmpDC.f1=v;_tmpDC.f2=s;_tmpDC;});_tmpDB;});}
# 370
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 374
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,unsigned int loc){
void*_tmpDE=e->r;void*_tmpDF=_tmpDE;struct Cyc_Absyn_Exp*_tmpE0;if(((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpDF)->tag == 19){_LL15: _tmpE0=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_LL16:
 return Cyc_Absyn_aggrarrow_exp(_tmpE0,f,loc);}else{_LL17: _LL18:
 return Cyc_Absyn_aggrmember_exp(e,f,loc);}_LL14:;}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 391
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
# 400
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 409
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
# 418
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
# 429
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
struct Cyc_Absyn_Exp*function;
{void*_tmpE1=Cyc_Tcutil_compress(t);void*_tmpE2=_tmpE1;enum Cyc_Absyn_Size_of _tmpE3;switch(*((int*)_tmpE2)){case 6: _LL1A: _tmpE3=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpE2)->f2;_LL1B:
# 433
{enum Cyc_Absyn_Size_of _tmpE4=_tmpE3;switch(_tmpE4){case Cyc_Absyn_Char_sz: _LL27: _LL28:
 function=fS->fchar;goto _LL26;case Cyc_Absyn_Short_sz: _LL29: _LL2A:
 function=fS->fshort;goto _LL26;case Cyc_Absyn_Int_sz: _LL2B: _LL2C:
 function=fS->fint;goto _LL26;default: _LL2D: _LL2E:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpE5=_cycalloc(sizeof(*_tmpE5));_tmpE5[0]=({struct Cyc_Core_Impossible_exn_struct _tmpE6;_tmpE6.tag=Cyc_Core_Impossible;_tmpE6.f1=({const char*_tmpE7="impossible IntType (not char, short or int)";_tag_dyneither(_tmpE7,sizeof(char),44);});_tmpE6;});_tmpE5;}));}_LL26:;}
# 439
goto _LL19;case 7: switch(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpE2)->f1){case 0: _LL1C: _LL1D:
# 441
 function=fS->ffloat;
goto _LL19;case 1: _LL1E: _LL1F:
# 444
 function=fS->fdouble;
goto _LL19;default: _LL20: _LL21:
# 447
 function=fS->flongdouble;
goto _LL19;}case 5: _LL22: _LL23:
# 450
 function=fS->fvoidstar;
goto _LL19;default: _LL24: _LL25:
# 453
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpE8=_cycalloc(sizeof(*_tmpE8));_tmpE8[0]=({struct Cyc_Core_Impossible_exn_struct _tmpE9;_tmpE9.tag=Cyc_Core_Impossible;_tmpE9.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpEC;_tmpEC.tag=0;_tmpEC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmpEA[1]={& _tmpEC};Cyc_aprintf(({const char*_tmpEB="impossible expression type %s (not int, float, double, or pointer)";_tag_dyneither(_tmpEB,sizeof(char),67);}),_tag_dyneither(_tmpEA,sizeof(void*),1));});});_tmpE9;});_tmpE8;}));}_LL19:;}
# 455
return function;}
# 457
struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
return Cyc_Toc_getFunctionType(fS,(void*)_check_null(arr->topt));}
# 460
struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmpED=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_tmpEE=_tmpED;void*_tmpEF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpEE)->tag == 5){_LL30: _tmpEF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpEE)->f1).elt_typ;_LL31:
# 463
 return Cyc_Toc_getFunctionType(fS,_tmpEF);}else{_LL32: _LL33:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpF0=_cycalloc(sizeof(*_tmpF0));_tmpF0[0]=({struct Cyc_Core_Impossible_exn_struct _tmpF1;_tmpF1.tag=Cyc_Core_Impossible;_tmpF1.f1=({const char*_tmpF2="impossible type (not pointer)";_tag_dyneither(_tmpF2,sizeof(char),30);});_tmpF1;});_tmpF0;}));}_LL2F:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 471
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmpF3=e->r;void*_tmpF4=_tmpF3;struct Cyc_List_List*_tmpF5;struct Cyc_List_List*_tmpF6;struct Cyc_List_List*_tmpF7;struct Cyc_List_List*_tmpF8;struct Cyc_List_List*_tmpF9;struct Cyc_Absyn_Exp*_tmpFA;long long _tmpFB;int _tmpFC;short _tmpFD;struct _dyneither_ptr _tmpFE;char _tmpFF;switch(*((int*)_tmpF4)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF4)->f1).Null_c).tag){case 2: _LL35: _tmpFF=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF4)->f1).Char_c).val).f2;_LL36:
 return _tmpFF == '\000';case 3: _LL37: _tmpFE=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF4)->f1).Wchar_c).val;_LL38: {
# 475
unsigned long _tmp100=Cyc_strlen((struct _dyneither_ptr)_tmpFE);
int i=0;
if(_tmp100 >= 2  && *((const char*)_check_dyneither_subscript(_tmpFE,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmpFE,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmpFE,sizeof(char),1))== 'x'  && _tmp100 >= 3) && *((const char*)_check_dyneither_subscript(_tmpFE,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp100;++ i){
if(*((const char*)_check_dyneither_subscript(_tmpFE,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 485
return 0;}}case 4: _LL39: _tmpFD=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF4)->f1).Short_c).val).f2;_LL3A:
 return _tmpFD == 0;case 5: _LL3B: _tmpFC=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF4)->f1).Int_c).val).f2;_LL3C:
 return _tmpFC == 0;case 6: _LL3D: _tmpFB=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF4)->f1).LongLong_c).val).f2;_LL3E:
 return _tmpFB == 0;case 1: _LL3F: _LL40:
 return 1;default: goto _LL4D;}case 13: _LL41: _tmpFA=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpF4)->f2;_LL42:
 return Cyc_Toc_is_zero(_tmpFA);case 23: _LL43: _tmpF9=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpF4)->f1;_LL44:
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmpF9);case 25: _LL45: _tmpF8=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpF4)->f1;_LL46:
 _tmpF7=_tmpF8;goto _LL48;case 28: _LL47: _tmpF7=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpF4)->f3;_LL48:
 _tmpF6=_tmpF7;goto _LL4A;case 24: _LL49: _tmpF6=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpF4)->f2;_LL4A:
 _tmpF5=_tmpF6;goto _LL4C;case 35: _LL4B: _tmpF5=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpF4)->f2;_LL4C:
# 496
 for(0;_tmpF5 != 0;_tmpF5=_tmpF5->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple19*)_tmpF5->hd)).f2))return 0;}
return 1;default: _LL4D: _LL4E:
 return 0;}_LL34:;}
# 504
static int Cyc_Toc_is_nullable(void*t){
void*_tmp101=Cyc_Tcutil_compress(t);void*_tmp102=_tmp101;union Cyc_Absyn_Constraint*_tmp103;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp102)->tag == 5){_LL50: _tmp103=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp102)->f1).ptr_atts).nullable;_LL51:
# 507
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp103);}else{_LL52: _LL53:
({void*_tmp104=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp105="is_nullable";_tag_dyneither(_tmp105,sizeof(char),12);}),_tag_dyneither(_tmp104,sizeof(void*),0));});}_LL4F:;}
# 513
static struct _dyneither_ptr Cyc_Toc_collapse_qvar(struct _dyneither_ptr s,struct _tuple1*x){
unsigned int slen=Cyc_strlen((struct _dyneither_ptr)s);
unsigned int len=9 + slen;
struct _tuple1*_tmp106=x;union Cyc_Absyn_Nmspace _tmp107;struct _dyneither_ptr _tmp108;_LL55: _tmp107=_tmp106->f1;_tmp108=*_tmp106->f2;_LL56:;{
unsigned int vlen=Cyc_strlen((struct _dyneither_ptr)_tmp108);
len +=1 + vlen;{
struct Cyc_List_List*nms;
{union Cyc_Absyn_Nmspace _tmp109=_tmp107;struct Cyc_List_List*_tmp10A;struct Cyc_List_List*_tmp10B;struct Cyc_List_List*_tmp10C;switch((_tmp109.Abs_n).tag){case 4: _LL58: _LL59:
 nms=0;goto _LL57;case 1: _LL5A: _tmp10C=(_tmp109.Rel_n).val;_LL5B:
 nms=_tmp10C;goto _LL57;case 2: _LL5C: _tmp10B=(_tmp109.Abs_n).val;_LL5D:
 nms=_tmp10B;goto _LL57;default: _LL5E: _tmp10A=(_tmp109.C_n).val;_LL5F:
 nms=_tmp10A;goto _LL57;}_LL57:;}
# 526
{struct Cyc_List_List*_tmp10D=nms;for(0;_tmp10D != 0;_tmp10D=_tmp10D->tl){
len +=1 + Cyc_strlen((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp10D->hd));}}{
struct _dyneither_ptr buf=({unsigned int _tmp118=len;char*_tmp119=_cyccalloc_atomic(sizeof(char),_tmp118);_tag_dyneither(_tmp119,sizeof(char),_tmp118);});
struct _dyneither_ptr p=buf;
Cyc_strcpy(p,(struct _dyneither_ptr)s);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)slen);
for(0;nms != 0;nms=nms->tl){
struct _dyneither_ptr*_tmp10E=(struct _dyneither_ptr*)nms->hd;struct _dyneither_ptr*_tmp10F=_tmp10E;struct _dyneither_ptr _tmp110;_LL61: _tmp110=*_tmp10F;_LL62:;
({struct _dyneither_ptr _tmp111=p;char _tmp112=*((char*)_check_dyneither_subscript(_tmp111,sizeof(char),0));char _tmp113='_';if(_get_dyneither_size(_tmp111,sizeof(char))== 1  && (_tmp112 == '\000'  && _tmp113 != '\000'))_throw_arraybounds();*((char*)_tmp111.curr)=_tmp113;});_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp110);
_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)Cyc_strlen((struct _dyneither_ptr)_tmp110));}
# 537
({struct _dyneither_ptr _tmp114=p;char _tmp115=*((char*)_check_dyneither_subscript(_tmp114,sizeof(char),0));char _tmp116='_';if(_get_dyneither_size(_tmp114,sizeof(char))== 1  && (_tmp115 == '\000'  && _tmp116 != '\000'))_throw_arraybounds();*((char*)_tmp114.curr)=_tmp116;});_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp108);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)vlen);
Cyc_strcpy(p,({const char*_tmp117="_struct";_tag_dyneither(_tmp117,sizeof(char),8);}));
return(struct _dyneither_ptr)buf;};};};}struct _tuple20{struct Cyc_Toc_TocState*f1;struct _tuple14*f2;};static char _tmp12F[8]="*bogus*";
# 549
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple20*env){
# 552
static struct _dyneither_ptr bogus_string={_tmp12F,_tmp12F,_tmp12F + 8};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
static struct _tuple14 pair={& bogus_qvar,& bogus_qvar};
# 556
struct _tuple20 _tmp11A=*env;struct _tuple20 _tmp11B=_tmp11A;struct Cyc_Dict_Dict*_tmp11C;struct _tuple14*_tmp11D;_LL64: _tmp11C=(_tmp11B.f1)->qvar_tags;_tmp11D=_tmp11B.f2;_LL65:;{
struct _tuple14 _tmp11E=*_tmp11D;struct _tuple14 _tmp11F=_tmp11E;struct _tuple1*_tmp120;struct _tuple1*_tmp121;_LL67: _tmp120=_tmp11F.f1;_tmp121=_tmp11F.f2;_LL68:;{
struct _handler_cons _tmp122;_push_handler(& _tmp122);{int _tmp124=0;if(setjmp(_tmp122.handler))_tmp124=1;if(!_tmp124){
{struct _tuple1*_tmp125=((struct _tuple1*(*)(struct Cyc_Dict_Dict d,int(*cmp)(struct _tuple14*,struct _tuple14*),struct _tuple14*k))Cyc_Dict_lookup_other)(*_tmp11C,Cyc_Toc_qvar_tag_cmp,_tmp11D);_npop_handler(0);return _tmp125;};_pop_handler();}else{void*_tmp123=(void*)_exn_thrown;void*_tmp126=_tmp123;void*_tmp127;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp126)->tag == Cyc_Dict_Absent){_LL6A: _LL6B: {
# 562
struct _tuple14*_tmp128=({struct _tuple14*_tmp12E=_cycalloc(sizeof(*_tmp12E));_tmp12E->f1=_tmp120;_tmp12E->f2=_tmp121;_tmp12E;});
struct _tuple1*_tmp129=_tmp120;union Cyc_Absyn_Nmspace _tmp12A;struct _dyneither_ptr _tmp12B;_LL6F: _tmp12A=_tmp129->f1;_tmp12B=*_tmp129->f2;_LL70:;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmp12B,_tmp121);
struct _tuple1*res=({struct _tuple1*_tmp12C=_cycalloc(sizeof(*_tmp12C));_tmp12C->f1=_tmp12A;_tmp12C->f2=({struct _dyneither_ptr*_tmp12D=_cycalloc(sizeof(*_tmp12D));_tmp12D[0]=newvar;_tmp12D;});_tmp12C;});
*_tmp11C=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmp11C,_tmp128,res);
return res;};}}else{_LL6C: _tmp127=_tmp126;_LL6D:(int)_rethrow(_tmp127);}_LL69:;}};};};}
# 571
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple14 env=({struct _tuple14 _tmp130;_tmp130.f1=fieldname;_tmp130.f2=dtname;_tmp130;});
return((struct _tuple1*(*)(struct _tuple14*arg,struct _tuple1*(*f)(struct _RegionHandle*,struct _tuple20*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_collapse_qvars_body);}
# 576
static void*Cyc_Toc_typ_to_c(void*t);struct _tuple21{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple22{void*f1;struct Cyc_List_List*f2;};
# 580
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple21*env){
# 583
struct _tuple21 _tmp131=*env;struct _tuple21 _tmp132=_tmp131;struct Cyc_List_List**_tmp133;struct Cyc_List_List*_tmp134;_LL72: _tmp133=(_tmp132.f1)->tuple_types;_tmp134=_tmp132.f2;_LL73:;
# 585
{struct Cyc_List_List*_tmp135=*_tmp133;for(0;_tmp135 != 0;_tmp135=_tmp135->tl){
struct _tuple22*_tmp136=(struct _tuple22*)_tmp135->hd;struct _tuple22*_tmp137=_tmp136;void*_tmp138;struct Cyc_List_List*_tmp139;_LL75: _tmp138=_tmp137->f1;_tmp139=_tmp137->f2;_LL76:;{
struct Cyc_List_List*_tmp13A=_tmp134;
for(0;_tmp13A != 0  && _tmp139 != 0;(_tmp13A=_tmp13A->tl,_tmp139=_tmp139->tl)){
if(!Cyc_Tcutil_unify((*((struct _tuple11*)_tmp13A->hd)).f2,(void*)_tmp139->hd))
break;}
if(_tmp13A == 0  && _tmp139 == 0)
return _tmp138;};}}{
# 595
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmp147=_cycalloc(sizeof(*_tmp147));_tmp147[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp14A;_tmp14A.tag=1;_tmp14A.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp148[1]={& _tmp14A};Cyc_aprintf(({const char*_tmp149="_tuple%d";_tag_dyneither(_tmp149,sizeof(char),9);}),_tag_dyneither(_tmp148,sizeof(void*),1));});});_tmp147;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,_tmp134);
struct Cyc_List_List*_tmp13B=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
_tmp13B=({struct Cyc_List_List*_tmp13C=_cycalloc(sizeof(*_tmp13C));_tmp13C->hd=({struct Cyc_Absyn_Aggrfield*_tmp13D=_cycalloc(sizeof(*_tmp13D));_tmp13D->name=Cyc_Absyn_fieldname(i);_tmp13D->tq=Cyc_Toc_mt_tq;_tmp13D->type=(void*)ts2->hd;_tmp13D->width=0;_tmp13D->attributes=0;_tmp13D->requires_clause=0;_tmp13D;});_tmp13C->tl=_tmp13B;_tmp13C;});}}
_tmp13B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp13B);{
struct Cyc_Absyn_Aggrdecl*_tmp13E=({struct Cyc_Absyn_Aggrdecl*_tmp144=_cycalloc(sizeof(*_tmp144));_tmp144->kind=Cyc_Absyn_StructA;_tmp144->sc=Cyc_Absyn_Public;_tmp144->name=({struct _tuple1*_tmp146=_cycalloc(sizeof(*_tmp146));_tmp146->f1=
Cyc_Absyn_Rel_n(0);_tmp146->f2=xname;_tmp146;});_tmp144->tvs=0;_tmp144->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp145=_cycalloc(sizeof(*_tmp145));_tmp145->exist_vars=0;_tmp145->rgn_po=0;_tmp145->fields=_tmp13B;_tmp145->tagged=0;_tmp145;});_tmp144->attributes=0;_tmp144->expected_mem_kind=0;_tmp144;});
# 610
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp13F=_cycalloc(sizeof(*_tmp13F));_tmp13F->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp140=_cycalloc(sizeof(*_tmp140));_tmp140[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp141;_tmp141.tag=5;_tmp141.f1=_tmp13E;_tmp141;});_tmp140;}),0);_tmp13F->tl=Cyc_Toc_result_decls;_tmp13F;});
*_tmp133=({struct Cyc_List_List*_tmp142=_region_malloc(d,sizeof(*_tmp142));_tmp142->hd=({struct _tuple22*_tmp143=_region_malloc(d,sizeof(*_tmp143));_tmp143->f1=x;_tmp143->f2=ts;_tmp143;});_tmp142->tl=*_tmp133;_tmp142;});
return x;};};}
# 615
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return((void*(*)(struct Cyc_List_List*arg,void*(*f)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state)(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple23{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple24{struct Cyc_Toc_TocState*f1;struct _tuple23*f2;};struct _tuple25{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 624
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple24*env){
# 632
struct _tuple24 _tmp14B=*env;struct _tuple24 _tmp14C=_tmp14B;struct Cyc_List_List**_tmp14D;struct _tuple1*_tmp14E;struct Cyc_List_List*_tmp14F;struct Cyc_List_List*_tmp150;struct Cyc_List_List*_tmp151;_LL78: _tmp14D=(_tmp14C.f1)->abs_struct_types;_tmp14E=(_tmp14C.f2)->f1;_tmp14F=(_tmp14C.f2)->f2;_tmp150=(_tmp14C.f2)->f3;_tmp151=(_tmp14C.f2)->f4;_LL79:;
# 637
{struct Cyc_List_List*_tmp152=*_tmp14D;for(0;_tmp152 != 0;_tmp152=_tmp152->tl){
struct _tuple25*_tmp153=(struct _tuple25*)_tmp152->hd;struct _tuple25*_tmp154=_tmp153;struct _tuple1*_tmp155;struct Cyc_List_List*_tmp156;void*_tmp157;_LL7B: _tmp155=_tmp154->f1;_tmp156=_tmp154->f2;_tmp157=_tmp154->f3;_LL7C:;
# 640
if(Cyc_Absyn_qvar_cmp(_tmp155,_tmp14E)== 0  && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp150)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp156)){
int okay=1;
{struct Cyc_List_List*_tmp158=_tmp150;for(0;_tmp158 != 0;(_tmp158=_tmp158->tl,_tmp156=_tmp156->tl)){
void*_tmp159=(void*)_tmp158->hd;
void*_tmp15A=(void*)((struct Cyc_List_List*)_check_null(_tmp156))->hd;
# 647
{struct Cyc_Absyn_Kind*_tmp15B=Cyc_Tcutil_typ_kind(_tmp159);struct Cyc_Absyn_Kind*_tmp15C=_tmp15B;switch(((struct Cyc_Absyn_Kind*)_tmp15C)->kind){case Cyc_Absyn_EffKind: _LL7E: _LL7F:
 goto _LL81;case Cyc_Absyn_RgnKind: _LL80: _LL81:
# 652
 continue;default: _LL82: _LL83:
# 655
 if(Cyc_Tcutil_unify(_tmp159,_tmp15A) || Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(_tmp159),Cyc_Toc_typ_to_c(_tmp15A)))
# 657
continue;
# 660
okay=0;
goto _LL7D;}_LL7D:;}
# 663
break;}}
# 665
if(okay)
# 667
return _tmp157;}}}{
# 675
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmp173=_cycalloc(sizeof(*_tmp173));_tmp173[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp176;_tmp176.tag=1;_tmp176.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp174[1]={& _tmp176};Cyc_aprintf(({const char*_tmp175="_tuple%d";_tag_dyneither(_tmp175,sizeof(char),9);}),_tag_dyneither(_tmp174,sizeof(void*),1));});});_tmp173;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp15D=0;
# 679
*_tmp14D=({struct Cyc_List_List*_tmp15E=_region_malloc(d,sizeof(*_tmp15E));_tmp15E->hd=({struct _tuple25*_tmp15F=_region_malloc(d,sizeof(*_tmp15F));_tmp15F->f1=_tmp14E;_tmp15F->f2=_tmp150;_tmp15F->f3=x;_tmp15F;});_tmp15E->tl=*_tmp14D;_tmp15E;});{
# 682
struct _RegionHandle _tmp160=_new_region("r");struct _RegionHandle*r=& _tmp160;_push_region(r);
{struct Cyc_List_List*_tmp161=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp14F,_tmp150);
for(0;_tmp151 != 0;_tmp151=_tmp151->tl){
struct Cyc_Absyn_Aggrfield*_tmp162=(struct Cyc_Absyn_Aggrfield*)_tmp151->hd;
void*t=_tmp162->type;
struct Cyc_List_List*atts=_tmp162->attributes;
# 690
if(_tmp151->tl == 0  && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(t)))
atts=({struct Cyc_List_List*_tmp163=_cycalloc(sizeof(*_tmp163));_tmp163->hd=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp164=_cycalloc(sizeof(*_tmp164));_tmp164[0]=({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmp165;_tmp165.tag=6;_tmp165.f1=0;_tmp165;});_tmp164;});_tmp163->tl=atts;_tmp163;});
# 693
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp161,t));
# 696
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val))
t=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp166=_cycalloc(sizeof(*_tmp166));_tmp166[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp167;_tmp167.tag=8;_tmp167.f1=({struct Cyc_Absyn_ArrayInfo _tmp168;_tmp168.elt_type=Cyc_Absyn_void_star_typ();_tmp168.tq=Cyc_Absyn_empty_tqual(0);_tmp168.num_elts=
Cyc_Absyn_uint_exp(0,0);_tmp168.zero_term=((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0);_tmp168.zt_loc=0;_tmp168;});_tmp167;});_tmp166;});
# 700
_tmp15D=({struct Cyc_List_List*_tmp169=_cycalloc(sizeof(*_tmp169));_tmp169->hd=({struct Cyc_Absyn_Aggrfield*_tmp16A=_cycalloc(sizeof(*_tmp16A));_tmp16A->name=_tmp162->name;_tmp16A->tq=Cyc_Toc_mt_tq;_tmp16A->type=t;_tmp16A->width=_tmp162->width;_tmp16A->attributes=atts;_tmp16A->requires_clause=0;_tmp16A;});_tmp169->tl=_tmp15D;_tmp169;});}
# 702
_tmp15D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp15D);{
struct Cyc_Absyn_Aggrdecl*_tmp16B=({struct Cyc_Absyn_Aggrdecl*_tmp170=_cycalloc(sizeof(*_tmp170));_tmp170->kind=Cyc_Absyn_StructA;_tmp170->sc=Cyc_Absyn_Public;_tmp170->name=({struct _tuple1*_tmp172=_cycalloc(sizeof(*_tmp172));_tmp172->f1=
Cyc_Absyn_Rel_n(0);_tmp172->f2=xname;_tmp172;});_tmp170->tvs=0;_tmp170->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp171=_cycalloc(sizeof(*_tmp171));_tmp171->exist_vars=0;_tmp171->rgn_po=0;_tmp171->fields=_tmp15D;_tmp171->tagged=0;_tmp171;});_tmp170->attributes=0;_tmp170->expected_mem_kind=0;_tmp170;});
# 710
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp16C=_cycalloc(sizeof(*_tmp16C));_tmp16C->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp16D=_cycalloc(sizeof(*_tmp16D));_tmp16D[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp16E;_tmp16E.tag=5;_tmp16E.f1=_tmp16B;_tmp16E;});_tmp16D;}),0);_tmp16C->tl=Cyc_Toc_result_decls;_tmp16C;});{
void*_tmp16F=x;_npop_handler(0);return _tmp16F;};};}
# 683
;_pop_region(r);};};}
# 714
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 718
struct _tuple23 env=({struct _tuple23 _tmp177;_tmp177.f1=struct_name;_tmp177.f2=type_vars;_tmp177.f3=type_args;_tmp177.f4=fields;_tmp177;});
return((void*(*)(struct _tuple23*arg,void*(*f)(struct _RegionHandle*,struct _tuple24*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 725
struct _tuple1*Cyc_Toc_temp_var(){
int _tmp178=Cyc_Toc_temp_var_counter ++;
struct _tuple1*res=({struct _tuple1*_tmp179=_cycalloc(sizeof(*_tmp179));_tmp179->f1=Cyc_Absyn_Loc_n;_tmp179->f2=({struct _dyneither_ptr*_tmp17A=_cycalloc(sizeof(*_tmp17A));_tmp17A[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp17D;_tmp17D.tag=1;_tmp17D.f1=(unsigned int)_tmp178;({void*_tmp17B[1]={& _tmp17D};Cyc_aprintf(({const char*_tmp17C="_tmp%X";_tag_dyneither(_tmp17C,sizeof(char),7);}),_tag_dyneither(_tmp17B,sizeof(void*),1));});});_tmp17A;});_tmp179;});
return res;}struct _tuple26{struct Cyc_Toc_TocState*f1;int f2;};
# 733
static struct _dyneither_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple26*env){
struct _tuple26 _tmp17E=*env;struct _tuple26 _tmp17F=_tmp17E;struct Cyc_Xarray_Xarray*_tmp180;_LL85: _tmp180=(_tmp17F.f1)->temp_labels;_LL86:;{
int _tmp181=Cyc_Toc_fresh_label_counter ++;
if(_tmp181 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp180))
return((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp180,_tmp181);{
struct _dyneither_ptr*res=({struct _dyneither_ptr*_tmp184=_cycalloc(sizeof(*_tmp184));_tmp184[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp187;_tmp187.tag=1;_tmp187.f1=(unsigned int)_tmp181;({void*_tmp185[1]={& _tmp187};Cyc_aprintf(({const char*_tmp186="_LL%X";_tag_dyneither(_tmp186,sizeof(char),6);}),_tag_dyneither(_tmp185,sizeof(void*),1));});});_tmp184;});
if(((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp180,res)!= _tmp181)
({void*_tmp182=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp183="fresh_label: add_ind returned bad index...";_tag_dyneither(_tmp183,sizeof(char),43);}),_tag_dyneither(_tmp182,sizeof(void*),0));});
return res;};};}
# 744
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
return((struct _dyneither_ptr*(*)(int arg,struct _dyneither_ptr*(*f)(struct _RegionHandle*,struct _tuple26*)))Cyc_Toc_use_toc_state)(0,Cyc_Toc_fresh_label_body);}
# 751
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
struct Cyc_List_List*_tmp188=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp188))->hd)->name)!= 0){
++ ans;
_tmp188=_tmp188->tl;}
# 758
return Cyc_Absyn_uint_exp((unsigned int)ans,0);}
# 764
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_result_decls);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
# 769
struct _tuple9 _tmp189=*a;struct _tuple9 _tmp18A=_tmp189;struct _dyneither_ptr*_tmp18B;struct Cyc_Absyn_Tqual _tmp18C;void*_tmp18D;_LL88: _tmp18B=_tmp18A.f1;_tmp18C=_tmp18A.f2;_tmp18D=_tmp18A.f3;_LL89:;
return({struct _tuple9*_tmp18E=_cycalloc(sizeof(*_tmp18E));_tmp18E->f1=_tmp18B;_tmp18E->f2=_tmp18C;_tmp18E->f3=Cyc_Toc_typ_to_c(_tmp18D);_tmp18E;});}
# 772
static struct _tuple11*Cyc_Toc_typ_to_c_f(struct _tuple11*x){
struct _tuple11 _tmp18F=*x;struct _tuple11 _tmp190=_tmp18F;struct Cyc_Absyn_Tqual _tmp191;void*_tmp192;_LL8B: _tmp191=_tmp190.f1;_tmp192=_tmp190.f2;_LL8C:;
return({struct _tuple11*_tmp193=_cycalloc(sizeof(*_tmp193));_tmp193->f1=_tmp191;_tmp193->f2=Cyc_Toc_typ_to_c(_tmp192);_tmp193;});}
# 791 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp194=Cyc_Tcutil_compress(t);void*_tmp195=_tmp194;void*_tmp196;void*_tmp197;struct Cyc_Absyn_Tqual _tmp198;struct Cyc_Absyn_Exp*_tmp199;union Cyc_Absyn_Constraint*_tmp19A;unsigned int _tmp19B;switch(*((int*)_tmp195)){case 8: _LL8E: _tmp197=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp195)->f1).elt_type;_tmp198=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp195)->f1).tq;_tmp199=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp195)->f1).num_elts;_tmp19A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp195)->f1).zero_term;_tmp19B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp195)->f1).zt_loc;_LL8F:
# 794
 return Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(_tmp197),_tmp198);case 1: _LL90: _tmp196=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp195)->f2;if(_tmp196 != 0){_LL91:
 return Cyc_Toc_typ_to_c_array(_tmp196);}else{goto _LL92;}default: _LL92: _LL93:
 return Cyc_Toc_typ_to_c(t);}_LL8D:;}
# 800
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){
# 802
return({struct Cyc_Absyn_Aggrfield*_tmp19C=_cycalloc(sizeof(*_tmp19C));_tmp19C->name=f->name;_tmp19C->tq=Cyc_Toc_mt_tq;_tmp19C->type=
# 804
Cyc_Toc_typ_to_c(f->type);_tmp19C->width=f->width;_tmp19C->attributes=f->attributes;_tmp19C->requires_clause=0;_tmp19C;});}
# 809
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 811
return;}
# 814
static void*Cyc_Toc_char_star_typ(){
static void**cs=0;
if(cs == 0)
cs=({void**_tmp19D=_cycalloc(sizeof(*_tmp19D));_tmp19D[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref);_tmp19D;});
# 819
return*cs;}
# 821
static void*Cyc_Toc_rgn_typ(){
static void**r=0;
if(r == 0)
r=({void**_tmp19E=_cycalloc(sizeof(*_tmp19E));_tmp19E[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq);_tmp19E;});
# 826
return*r;}
# 828
static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;
if(r == 0)
r=({void**_tmp19F=_cycalloc(sizeof(*_tmp19F));_tmp19F[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq);_tmp19F;});
# 833
return*r;}
# 835
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp1A0=Cyc_Tcutil_compress(t);void*_tmp1A1=_tmp1A0;struct Cyc_Absyn_Tvar*_tmp1A2;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A1)->tag == 2){_LL95: _tmp1A2=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A1)->f1;_LL96:
# 838
 return Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tbk);}else{_LL97: _LL98:
 return 0;}_LL94:;}
# 842
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp1A3=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp1A4=_tmp1A3;if(((struct Cyc_Absyn_Kind*)_tmp1A4)->kind == Cyc_Absyn_AnyKind){_LL9A: _LL9B:
 return 1;}else{_LL9C: _LL9D:
 return 0;}_LL99:;}
# 849
static void*Cyc_Toc_typ_to_c(void*t){
void*_tmp1A5=t;struct Cyc_Absyn_Datatypedecl*_tmp1A6;void**_tmp1A7;struct Cyc_Absyn_Enumdecl*_tmp1A8;struct Cyc_Absyn_Aggrdecl*_tmp1A9;struct Cyc_Absyn_Exp*_tmp1AA;struct Cyc_Absyn_Exp*_tmp1AB;void*_tmp1AC;struct _tuple1*_tmp1AD;struct Cyc_List_List*_tmp1AE;struct Cyc_Absyn_Typedefdecl*_tmp1AF;void*_tmp1B0;struct Cyc_List_List*_tmp1B1;struct _tuple1*_tmp1B2;union Cyc_Absyn_AggrInfoU _tmp1B3;struct Cyc_List_List*_tmp1B4;enum Cyc_Absyn_AggrKind _tmp1B5;struct Cyc_List_List*_tmp1B6;struct Cyc_List_List*_tmp1B7;struct Cyc_Absyn_Tqual _tmp1B8;void*_tmp1B9;struct Cyc_List_List*_tmp1BA;int _tmp1BB;struct Cyc_Absyn_VarargInfo*_tmp1BC;struct Cyc_List_List*_tmp1BD;void*_tmp1BE;struct Cyc_Absyn_Tqual _tmp1BF;struct Cyc_Absyn_Exp*_tmp1C0;unsigned int _tmp1C1;void*_tmp1C2;struct Cyc_Absyn_Tqual _tmp1C3;union Cyc_Absyn_Constraint*_tmp1C4;struct Cyc_Absyn_Datatypedecl*_tmp1C5;struct Cyc_Absyn_Datatypefield*_tmp1C6;struct Cyc_Absyn_Tvar*_tmp1C7;void**_tmp1C8;switch(*((int*)_tmp1A5)){case 0: _LL9F: _LLA0:
 return t;case 1: _LLA1: _tmp1C8=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A5)->f2;_LLA2:
# 853
 if(*_tmp1C8 == 0){
*_tmp1C8=Cyc_Absyn_sint_typ;
return Cyc_Absyn_sint_typ;}
# 857
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp1C8));case 2: _LLA3: _tmp1C7=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A5)->f1;_LLA4:
# 859
 if((Cyc_Tcutil_tvar_kind(_tmp1C7,& Cyc_Tcutil_bk))->kind == Cyc_Absyn_AnyKind)
# 862
return(void*)& Cyc_Absyn_VoidType_val;else{
# 864
return Cyc_Absyn_void_star_typ();}case 3: _LLA5: _LLA6:
# 866
 return(void*)& Cyc_Absyn_VoidType_val;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1A5)->f1).field_info).KnownDatatypefield).tag == 2){_LLA7: _tmp1C5=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1A5)->f1).field_info).KnownDatatypefield).val).f1;_tmp1C6=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1A5)->f1).field_info).KnownDatatypefield).val).f2;_LLA8:
# 868
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp1C6->name,_tmp1C5->name));}else{_LLA9: _LLAA:
({void*_tmp1C9=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1CA="unresolved DatatypeFieldType";_tag_dyneither(_tmp1CA,sizeof(char),29);}),_tag_dyneither(_tmp1C9,sizeof(void*),0));});}case 5: _LLAB: _tmp1C2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A5)->f1).elt_typ;_tmp1C3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A5)->f1).elt_tq;_tmp1C4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A5)->f1).ptr_atts).bounds;_LLAC:
# 873
 _tmp1C2=Cyc_Toc_typ_to_c(_tmp1C2);{
void*_tmp1CB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp1C4);void*_tmp1CC=_tmp1CB;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1CC)->tag == 0){_LLE0: _LLE1:
 return Cyc_Toc_dyneither_ptr_typ;}else{_LLE2: _LLE3:
 return Cyc_Absyn_star_typ(_tmp1C2,(void*)& Cyc_Absyn_HeapRgn_val,_tmp1C3,Cyc_Absyn_false_conref);}_LLDF:;};case 6: _LLAD: _LLAE:
# 878
 goto _LLB0;case 7: _LLAF: _LLB0:
 return t;case 8: _LLB1: _tmp1BE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1A5)->f1).elt_type;_tmp1BF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1A5)->f1).tq;_tmp1C0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1A5)->f1).num_elts;_tmp1C1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1A5)->f1).zt_loc;_LLB2:
# 881
 return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c(_tmp1BE),_tmp1BF,_tmp1C0,Cyc_Absyn_false_conref,_tmp1C1);case 9: _LLB3: _tmp1B8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1A5)->f1).ret_tqual;_tmp1B9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1A5)->f1).ret_typ;_tmp1BA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1A5)->f1).args;_tmp1BB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1A5)->f1).c_varargs;_tmp1BC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1A5)->f1).cyc_varargs;_tmp1BD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1A5)->f1).attributes;_LLB4: {
# 887
struct Cyc_List_List*_tmp1CD=0;
for(0;_tmp1BD != 0;_tmp1BD=_tmp1BD->tl){
void*_tmp1CE=(void*)_tmp1BD->hd;void*_tmp1CF=_tmp1CE;switch(*((int*)_tmp1CF)){case 4: _LLE5: _LLE6:
 goto _LLE8;case 5: _LLE7: _LLE8:
 goto _LLEA;case 19: _LLE9: _LLEA:
 continue;case 22: _LLEB: _LLEC:
 continue;case 21: _LLED: _LLEE:
 continue;case 20: _LLEF: _LLF0:
 continue;default: _LLF1: _LLF2:
 _tmp1CD=({struct Cyc_List_List*_tmp1D0=_cycalloc(sizeof(*_tmp1D0));_tmp1D0->hd=(void*)_tmp1BD->hd;_tmp1D0->tl=_tmp1CD;_tmp1D0;});goto _LLE4;}_LLE4:;}{
# 898
struct Cyc_List_List*_tmp1D1=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp1BA);
if(_tmp1BC != 0){
# 901
void*_tmp1D2=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp1BC->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple9*_tmp1D3=({struct _tuple9*_tmp1D5=_cycalloc(sizeof(*_tmp1D5));_tmp1D5->f1=_tmp1BC->name;_tmp1D5->f2=_tmp1BC->tq;_tmp1D5->f3=_tmp1D2;_tmp1D5;});
_tmp1D1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1D1,({struct Cyc_List_List*_tmp1D4=_cycalloc(sizeof(*_tmp1D4));_tmp1D4->hd=_tmp1D3;_tmp1D4->tl=0;_tmp1D4;}));}
# 905
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1D6=_cycalloc(sizeof(*_tmp1D6));_tmp1D6[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1D7;_tmp1D7.tag=9;_tmp1D7.f1=({struct Cyc_Absyn_FnInfo _tmp1D8;_tmp1D8.tvars=0;_tmp1D8.effect=0;_tmp1D8.ret_tqual=_tmp1B8;_tmp1D8.ret_typ=Cyc_Toc_typ_to_c(_tmp1B9);_tmp1D8.args=_tmp1D1;_tmp1D8.c_varargs=_tmp1BB;_tmp1D8.cyc_varargs=0;_tmp1D8.rgn_po=0;_tmp1D8.attributes=_tmp1CD;_tmp1D8.requires_clause=0;_tmp1D8.requires_relns=0;_tmp1D8.ensures_clause=0;_tmp1D8.ensures_relns=0;_tmp1D8;});_tmp1D7;});_tmp1D6;});};}case 10: _LLB5: _tmp1B7=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1A5)->f1;_LLB6:
# 910
 _tmp1B7=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp1B7);
return Cyc_Toc_add_tuple_type(_tmp1B7);case 12: _LLB7: _tmp1B5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1A5)->f1;_tmp1B6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1A5)->f2;_LLB8:
# 915
 return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1D9=_cycalloc(sizeof(*_tmp1D9));_tmp1D9[0]=({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp1DA;_tmp1DA.tag=12;_tmp1DA.f1=_tmp1B5;_tmp1DA.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp1B6);_tmp1DA;});_tmp1D9;});case 11: _LLB9: _tmp1B3=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A5)->f1).aggr_info;_tmp1B4=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A5)->f1).targs;_LLBA:
# 919
{union Cyc_Absyn_AggrInfoU _tmp1DB=_tmp1B3;if((_tmp1DB.UnknownAggr).tag == 1){_LLF4: _LLF5:
 return t;}else{_LLF6: _LLF7:
 goto _LLF3;}_LLF3:;}{
# 923
struct Cyc_Absyn_Aggrdecl*_tmp1DC=Cyc_Absyn_get_known_aggrdecl(_tmp1B3);
if(_tmp1DC->expected_mem_kind){
# 926
if(_tmp1DC->impl == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1E0;_tmp1E0.tag=0;_tmp1E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 929
Cyc_Absynpp_qvar2string(_tmp1DC->name));({struct Cyc_String_pa_PrintArg_struct _tmp1DF;_tmp1DF.tag=0;_tmp1DF.f1=(struct _dyneither_ptr)(
# 928
_tmp1DC->kind == Cyc_Absyn_UnionA?({const char*_tmp1E1="union";_tag_dyneither(_tmp1E1,sizeof(char),6);}):({const char*_tmp1E2="struct";_tag_dyneither(_tmp1E2,sizeof(char),7);}));({void*_tmp1DD[2]={& _tmp1DF,& _tmp1E0};Cyc_Tcutil_warn(0,({const char*_tmp1DE="%s %s was never defined.";_tag_dyneither(_tmp1DE,sizeof(char),25);}),_tag_dyneither(_tmp1DD,sizeof(void*),2));});});});}
# 934
if(_tmp1DC->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp1DC->name,Cyc_Absyn_unionq_typ);{
struct Cyc_List_List*_tmp1E3=_tmp1DC->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1DC->impl))->fields;
if(_tmp1E3 == 0)return Cyc_Toc_aggrdecl_type(_tmp1DC->name,Cyc_Absyn_strctq);
for(0;_tmp1E3->tl != 0;_tmp1E3=_tmp1E3->tl){;}{
void*_tmp1E4=((struct Cyc_Absyn_Aggrfield*)_tmp1E3->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp1E4))){
if(_tmp1DC->expected_mem_kind)
({struct Cyc_String_pa_PrintArg_struct _tmp1E7;_tmp1E7.tag=0;_tmp1E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp1DC->name));({void*_tmp1E5[1]={& _tmp1E7};Cyc_Tcutil_warn(0,({const char*_tmp1E6="struct %s ended up being abstract.";_tag_dyneither(_tmp1E6,sizeof(char),35);}),_tag_dyneither(_tmp1E5,sizeof(void*),1));});});{
# 948
struct _RegionHandle _tmp1E8=_new_region("r");struct _RegionHandle*r=& _tmp1E8;_push_region(r);
{struct Cyc_List_List*_tmp1E9=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp1DC->tvs,_tmp1B4);
void*_tmp1EA=Cyc_Tcutil_rsubstitute(r,_tmp1E9,_tmp1E4);
if(Cyc_Toc_is_abstract_type(_tmp1EA)){void*_tmp1EB=Cyc_Toc_aggrdecl_type(_tmp1DC->name,Cyc_Absyn_strctq);_npop_handler(0);return _tmp1EB;}{
void*_tmp1EC=Cyc_Toc_add_struct_type(_tmp1DC->name,_tmp1DC->tvs,_tmp1B4,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1DC->impl))->fields);_npop_handler(0);return _tmp1EC;};}
# 949
;_pop_region(r);};}
# 954
return Cyc_Toc_aggrdecl_type(_tmp1DC->name,Cyc_Absyn_strctq);};};};case 13: _LLBB: _tmp1B2=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp1A5)->f1;_LLBC:
 return t;case 14: _LLBD: _tmp1B1=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1A5)->f1;_LLBE:
 Cyc_Toc_enumfields_to_c(_tmp1B1);return t;case 17: _LLBF: _tmp1AD=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1A5)->f1;_tmp1AE=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1A5)->f2;_tmp1AF=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1A5)->f3;_tmp1B0=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1A5)->f4;_LLC0:
# 958
 if(_tmp1B0 == 0  || Cyc_noexpand_r){
if(_tmp1AE != 0)
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp1ED=_cycalloc(sizeof(*_tmp1ED));_tmp1ED[0]=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp1EE;_tmp1EE.tag=17;_tmp1EE.f1=_tmp1AD;_tmp1EE.f2=0;_tmp1EE.f3=_tmp1AF;_tmp1EE.f4=0;_tmp1EE;});_tmp1ED;});else{
return t;}}else{
# 963
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp1EF=_cycalloc(sizeof(*_tmp1EF));_tmp1EF[0]=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp1F0;_tmp1F0.tag=17;_tmp1F0.f1=_tmp1AD;_tmp1F0.f2=0;_tmp1F0.f3=_tmp1AF;_tmp1F0.f4=Cyc_Toc_typ_to_c(_tmp1B0);_tmp1F0;});_tmp1EF;});}case 19: _LLC1: _LLC2:
 return Cyc_Absyn_uint_typ;case 15: _LLC3: _tmp1AC=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1A5)->f1;_LLC4:
 return Cyc_Toc_rgn_typ();case 16: _LLC5: _LLC6:
 return Cyc_Toc_dyn_rgn_typ();case 20: _LLC7: _LLC8:
# 969
 goto _LLCA;case 21: _LLC9: _LLCA:
 goto _LLCC;case 22: _LLCB: _LLCC:
 goto _LLCE;case 23: _LLCD: _LLCE:
 goto _LLD0;case 24: _LLCF: _LLD0:
 goto _LLD2;case 25: _LLD1: _LLD2:
 return Cyc_Absyn_void_star_typ();case 18: _LLD3: _tmp1AB=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp1A5)->f1;_LLD4:
# 979
 return t;case 27: _LLD5: _tmp1AA=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp1A5)->f1;_LLD6:
# 981
 return t;case 28: _LLD7: _LLD8:
 return t;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1A5)->f1)->r)){case 0: _LLD9: _tmp1A9=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1A5)->f1)->r)->f1;_LLDA:
# 984
 Cyc_Toc_aggrdecl_to_c(_tmp1A9,1);
if(_tmp1A9->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp1A9->name,Cyc_Absyn_unionq_typ);else{
return Cyc_Toc_aggrdecl_type(_tmp1A9->name,Cyc_Absyn_strctq);}case 1: _LLDB: _tmp1A8=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1A5)->f1)->r)->f1;_LLDC:
# 989
 Cyc_Toc_enumdecl_to_c(_tmp1A8);
return t;default: _LLDD: _tmp1A6=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1A5)->f1)->r)->f1;_tmp1A7=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1A5)->f2;_LLDE:
# 992
 Cyc_Toc_datatypedecl_to_c(_tmp1A6);
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp1A7)));}}_LL9E:;}
# 997
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned int l){
void*_tmp1F1=t;void*_tmp1F2;struct Cyc_Absyn_Tqual _tmp1F3;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F1)->tag == 8){_LLF9: _tmp1F2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F1)->f1).elt_type;_tmp1F3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F1)->f1).tq;_LLFA:
# 1000
 return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp1F2,(void*)& Cyc_Absyn_HeapRgn_val,_tmp1F3,Cyc_Absyn_false_conref),e);}else{_LLFB: _LLFC:
 return Cyc_Toc_cast_it(t,e);}_LLF8:;}
# 1007
static int Cyc_Toc_atomic_typ(void*t){
void*_tmp1F4=Cyc_Tcutil_compress(t);void*_tmp1F5=_tmp1F4;struct Cyc_List_List*_tmp1F6;struct Cyc_Absyn_Datatypedecl*_tmp1F7;struct Cyc_Absyn_Datatypefield*_tmp1F8;struct Cyc_List_List*_tmp1F9;union Cyc_Absyn_AggrInfoU _tmp1FA;void*_tmp1FB;switch(*((int*)_tmp1F5)){case 0: _LLFE: _LLFF:
 return 1;case 2: _LL100: _LL101:
 return 0;case 6: _LL102: _LL103:
 goto _LL105;case 13: _LL104: _LL105:
 goto _LL107;case 14: _LL106: _LL107:
 goto _LL109;case 7: _LL108: _LL109:
 goto _LL10B;case 9: _LL10A: _LL10B:
 goto _LL10D;case 19: _LL10C: _LL10D:
 return 1;case 8: _LL10E: _tmp1FB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F5)->f1).elt_type;_LL10F:
 return Cyc_Toc_atomic_typ(_tmp1FB);case 11: _LL110: _tmp1FA=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1F5)->f1).aggr_info;_LL111:
# 1024
{union Cyc_Absyn_AggrInfoU _tmp1FC=_tmp1FA;if((_tmp1FC.UnknownAggr).tag == 1){_LL123: _LL124:
 return 0;}else{_LL125: _LL126:
 goto _LL122;}_LL122:;}{
# 1028
struct Cyc_Absyn_Aggrdecl*_tmp1FD=Cyc_Absyn_get_known_aggrdecl(_tmp1FA);
if(_tmp1FD->impl == 0)
return 0;
{struct Cyc_List_List*_tmp1FE=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1FD->impl))->fields;for(0;_tmp1FE != 0;_tmp1FE=_tmp1FE->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp1FE->hd)->type))return 0;}}
return 1;};case 12: _LL112: _tmp1F9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1F5)->f2;_LL113:
# 1035
 for(0;_tmp1F9 != 0;_tmp1F9=_tmp1F9->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp1F9->hd)->type))return 0;}
return 1;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1F5)->f1).field_info).KnownDatatypefield).tag == 2){_LL114: _tmp1F7=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1F5)->f1).field_info).KnownDatatypefield).val).f1;_tmp1F8=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1F5)->f1).field_info).KnownDatatypefield).val).f2;_LL115:
# 1039
 _tmp1F6=_tmp1F8->typs;goto _LL117;}else{goto _LL120;}case 10: _LL116: _tmp1F6=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1F5)->f1;_LL117:
# 1041
 for(0;_tmp1F6 != 0;_tmp1F6=_tmp1F6->tl){
if(!Cyc_Toc_atomic_typ((*((struct _tuple11*)_tmp1F6->hd)).f2))return 0;}
return 1;case 3: _LL118: _LL119:
# 1046
 goto _LL11B;case 5: _LL11A: _LL11B:
 goto _LL11D;case 16: _LL11C: _LL11D:
 goto _LL11F;case 15: _LL11E: _LL11F:
 return 0;default: _LL120: _LL121:
({struct Cyc_String_pa_PrintArg_struct _tmp201;_tmp201.tag=0;_tmp201.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp1FF[1]={& _tmp201};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp200="atomic_typ:  bad type %s";_tag_dyneither(_tmp200,sizeof(char),25);}),_tag_dyneither(_tmp1FF,sizeof(void*),1));});});}_LLFD:;}
# 1054
static int Cyc_Toc_is_void_star(void*t){
void*_tmp202=Cyc_Tcutil_compress(t);void*_tmp203=_tmp202;void*_tmp204;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp203)->tag == 5){_LL128: _tmp204=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp203)->f1).elt_typ;_LL129: {
# 1057
void*_tmp205=Cyc_Tcutil_compress(_tmp204);void*_tmp206=_tmp205;if(((struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp206)->tag == 0){_LL12D: _LL12E:
 return 1;}else{_LL12F: _LL130:
 return 0;}_LL12C:;}}else{_LL12A: _LL12B:
# 1061
 return 0;}_LL127:;}
# 1065
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1069
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1074
static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){
void*_tmp207=Cyc_Tcutil_compress(t);void*_tmp208=_tmp207;struct Cyc_List_List*_tmp209;union Cyc_Absyn_AggrInfoU _tmp20A;switch(*((int*)_tmp208)){case 11: _LL132: _tmp20A=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp208)->f1).aggr_info;_LL133: {
# 1077
struct Cyc_Absyn_Aggrdecl*_tmp20B=Cyc_Absyn_get_known_aggrdecl(_tmp20A);
if(_tmp20B->impl == 0)
({void*_tmp20C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp20D="is_poly_field: type missing fields";_tag_dyneither(_tmp20D,sizeof(char),35);}),_tag_dyneither(_tmp20C,sizeof(void*),0));});
_tmp209=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp20B->impl))->fields;goto _LL135;}case 12: _LL134: _tmp209=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp208)->f2;_LL135: {
# 1082
struct Cyc_Absyn_Aggrfield*_tmp20E=Cyc_Absyn_lookup_field(_tmp209,f);
if(_tmp20E == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp211;_tmp211.tag=0;_tmp211.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp20F[1]={& _tmp211};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp210="is_poly_field: bad field %s";_tag_dyneither(_tmp210,sizeof(char),28);}),_tag_dyneither(_tmp20F,sizeof(void*),1));});});
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp20E->type);}default: _LL136: _LL137:
({struct Cyc_String_pa_PrintArg_struct _tmp214;_tmp214.tag=0;_tmp214.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp212[1]={& _tmp214};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp213="is_poly_field: bad type %s";_tag_dyneither(_tmp213,sizeof(char),27);}),_tag_dyneither(_tmp212,sizeof(void*),1));});});}_LL131:;}
# 1093
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp215=e->r;void*_tmp216=_tmp215;struct Cyc_Absyn_Exp*_tmp217;struct _dyneither_ptr*_tmp218;struct Cyc_Absyn_Exp*_tmp219;struct _dyneither_ptr*_tmp21A;switch(*((int*)_tmp216)){case 20: _LL139: _tmp219=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp216)->f1;_tmp21A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp216)->f2;_LL13A:
# 1096
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp219->topt),_tmp21A) && !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));case 21: _LL13B: _tmp217=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp216)->f1;_tmp218=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp216)->f2;_LL13C: {
# 1099
void*_tmp21B=Cyc_Tcutil_compress((void*)_check_null(_tmp217->topt));void*_tmp21C=_tmp21B;void*_tmp21D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21C)->tag == 5){_LL140: _tmp21D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21C)->f1).elt_typ;_LL141:
# 1101
 return Cyc_Toc_is_poly_field(_tmp21D,_tmp218) && !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}else{_LL142: _LL143:
({struct Cyc_String_pa_PrintArg_struct _tmp220;_tmp220.tag=0;_tmp220.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp217->topt)));({void*_tmp21E[1]={& _tmp220};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp21F="is_poly_project: bad type %s";_tag_dyneither(_tmp21F,sizeof(char),29);}),_tag_dyneither(_tmp21E,sizeof(void*),1));});});}_LL13F:;}default: _LL13D: _LL13E:
# 1104
 return 0;}_LL138:;}
# 1109
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,({struct Cyc_List_List*_tmp221=_cycalloc(sizeof(*_tmp221));_tmp221->hd=s;_tmp221->tl=0;_tmp221;}),0);}
# 1113
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,({struct Cyc_List_List*_tmp222=_cycalloc(sizeof(*_tmp222));_tmp222->hd=s;_tmp222->tl=0;_tmp222;}),0);}
# 1117
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1123
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,({struct Cyc_Absyn_Exp*_tmp223[2];_tmp223[1]=s;_tmp223[0]=rgn;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp223,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}
# 1127
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return Cyc_Absyn_fncall_exp(Cyc_Toc__fast_region_malloc_e,({struct Cyc_Absyn_Exp*_tmp224[2];_tmp224[1]=s;_tmp224[0]=rgn;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp224,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}
# 1131
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type))
return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,({struct Cyc_Absyn_Exp*_tmp225[2];_tmp225[1]=n;_tmp225[0]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp225,sizeof(struct Cyc_Absyn_Exp*),2));}),0);else{
# 1135
return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,({struct Cyc_Absyn_Exp*_tmp226[2];_tmp226[1]=n;_tmp226[0]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp226,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}}
# 1138
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,({struct Cyc_Absyn_Exp*_tmp227[3];_tmp227[2]=n;_tmp227[1]=s;_tmp227[0]=rgn;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp227,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}
# 1143
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,({struct Cyc_List_List*_tmp228=_cycalloc(sizeof(*_tmp228));_tmp228->hd=e;_tmp228->tl=0;_tmp228;}),0);}
# 1146
static struct Cyc_Absyn_Exp*Cyc_Toc_newrethrow_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_fncall_exp(Cyc_Toc__rethrow_e,({struct Cyc_List_List*_tmp229=_cycalloc(sizeof(*_tmp229));_tmp229->hd=e;_tmp229->tl=0;_tmp229;}),0);}
# 1150
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,({struct Cyc_Absyn_Exp*_tmp22A[0];((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp22A,sizeof(struct Cyc_Absyn_Exp*),0));}),0),0);}
# 1156
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1164
int is_string=0;
{void*_tmp22B=e->r;void*_tmp22C=_tmp22B;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp22C)->tag == 0)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp22C)->f1).Wstring_c).tag){case 8: _LL145: _LL146:
 is_string=1;goto _LL144;case 9: _LL147: _LL148:
 is_string=1;goto _LL144;default: goto _LL149;}else{_LL149: _LL14A:
 goto _LL144;}_LL144:;}{
# 1170
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
if(is_string){
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_conref,0);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp22D=_cycalloc(sizeof(*_tmp22D));_tmp22D->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp22E=_cycalloc(sizeof(*_tmp22E));_tmp22E[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp22F;_tmp22F.tag=0;_tmp22F.f1=vd;_tmp22F;});_tmp22E;}),0);_tmp22D->tl=Cyc_Toc_result_decls;_tmp22D;});
xexp=Cyc_Absyn_var_exp(x,0);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{
# 1180
xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),e);
# 1182
xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
# 1184
struct Cyc_Absyn_Exp*urm_exp;
urm_exp=Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple19*_tmp230[3];_tmp230[2]=({struct _tuple19*_tmp233=_cycalloc(sizeof(*_tmp233));_tmp233->f1=0;_tmp233->f2=xplussz;_tmp233;});_tmp230[1]=({struct _tuple19*_tmp232=_cycalloc(sizeof(*_tmp232));_tmp232->f1=0;_tmp232->f2=xexp;_tmp232;});_tmp230[0]=({struct _tuple19*_tmp231=_cycalloc(sizeof(*_tmp231));_tmp231->f1=0;_tmp231->f2=xexp;_tmp231;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp230,sizeof(struct _tuple19*),3));}),0);
# 1188
return urm_exp;};};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_StructInfo{struct Cyc_Absyn_Exp**varsizeexp;struct Cyc_Absyn_Exp*lhs_exp;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;int*in_lhs;struct Cyc_Toc_StructInfo struct_info;int*in_sizeof;struct _RegionHandle*rgn;};
# 1231 "toc.cyc"
typedef struct Cyc_Toc_Env*Cyc_Toc_env_t;
# 1233
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp234=nv;int _tmp235;_LL14C: _tmp235=_tmp234->toplevel;_LL14D:;
return _tmp235;}
# 1237
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp236=nv;int*_tmp237;_LL14F: _tmp237=_tmp236->in_lhs;_LL150:;
return*_tmp237;}
# 1242
static struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple1*x){
struct Cyc_Toc_Env*_tmp238=nv;struct Cyc_Dict_Dict _tmp239;_LL152: _tmp239=_tmp238->varmap;_LL153:;
return((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp239,x);}
# 1248
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
return({struct Cyc_Toc_Env*_tmp23A=_region_malloc(r,sizeof(*_tmp23A));_tmp23A->break_lab=(struct _dyneither_ptr**)0;_tmp23A->continue_lab=(struct _dyneither_ptr**)0;_tmp23A->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0;_tmp23A->varmap=(struct Cyc_Dict_Dict)
# 1252
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp);_tmp23A->toplevel=(int)1;_tmp23A->in_lhs=(int*)({int*_tmp23D=_region_malloc(r,sizeof(*_tmp23D));_tmp23D[0]=0;_tmp23D;});_tmp23A->struct_info=(struct Cyc_Toc_StructInfo)({(_tmp23A->struct_info).varsizeexp=({struct Cyc_Absyn_Exp**_tmp23C=_cycalloc(sizeof(*_tmp23C));_tmp23C[0]=0;_tmp23C;});(_tmp23A->struct_info).lhs_exp=0;_tmp23A->struct_info;});_tmp23A->in_sizeof=(int*)({int*_tmp23B=_region_malloc(r,sizeof(*_tmp23B));_tmp23B[0]=0;_tmp23B;});_tmp23A->rgn=(struct _RegionHandle*)r;_tmp23A;});}
# 1260
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp23E=e;struct _dyneither_ptr**_tmp23F;struct _dyneither_ptr**_tmp240;struct Cyc_Toc_FallthruInfo*_tmp241;struct Cyc_Dict_Dict _tmp242;int _tmp243;int*_tmp244;struct Cyc_Toc_StructInfo _tmp245;int*_tmp246;_LL155: _tmp23F=_tmp23E->break_lab;_tmp240=_tmp23E->continue_lab;_tmp241=_tmp23E->fallthru_info;_tmp242=_tmp23E->varmap;_tmp243=_tmp23E->toplevel;_tmp244=_tmp23E->in_lhs;_tmp245=_tmp23E->struct_info;_tmp246=_tmp23E->in_sizeof;_LL156:;
return({struct Cyc_Toc_Env*_tmp247=_region_malloc(r,sizeof(*_tmp247));_tmp247->break_lab=(struct _dyneither_ptr**)_tmp23F;_tmp247->continue_lab=(struct _dyneither_ptr**)_tmp240;_tmp247->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp241;_tmp247->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp242);_tmp247->toplevel=(int)_tmp243;_tmp247->in_lhs=(int*)_tmp244;_tmp247->struct_info=(struct Cyc_Toc_StructInfo)_tmp245;_tmp247->in_sizeof=(int*)_tmp246;_tmp247->rgn=(struct _RegionHandle*)r;_tmp247;});}
# 1265
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp248=e;struct _dyneither_ptr**_tmp249;struct _dyneither_ptr**_tmp24A;struct Cyc_Toc_FallthruInfo*_tmp24B;struct Cyc_Dict_Dict _tmp24C;int _tmp24D;int*_tmp24E;struct Cyc_Toc_StructInfo _tmp24F;int*_tmp250;_LL158: _tmp249=_tmp248->break_lab;_tmp24A=_tmp248->continue_lab;_tmp24B=_tmp248->fallthru_info;_tmp24C=_tmp248->varmap;_tmp24D=_tmp248->toplevel;_tmp24E=_tmp248->in_lhs;_tmp24F=_tmp248->struct_info;_tmp250=_tmp248->in_sizeof;_LL159:;
return({struct Cyc_Toc_Env*_tmp251=_region_malloc(r,sizeof(*_tmp251));_tmp251->break_lab=(struct _dyneither_ptr**)_tmp249;_tmp251->continue_lab=(struct _dyneither_ptr**)_tmp24A;_tmp251->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp24B;_tmp251->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp24C);_tmp251->toplevel=(int)0;_tmp251->in_lhs=(int*)_tmp24E;_tmp251->struct_info=(struct Cyc_Toc_StructInfo)_tmp24F;_tmp251->in_sizeof=(int*)_tmp250;_tmp251->rgn=(struct _RegionHandle*)r;_tmp251;});}
# 1269
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp252=e;struct _dyneither_ptr**_tmp253;struct _dyneither_ptr**_tmp254;struct Cyc_Toc_FallthruInfo*_tmp255;struct Cyc_Dict_Dict _tmp256;int _tmp257;int*_tmp258;struct Cyc_Toc_StructInfo _tmp259;int*_tmp25A;_LL15B: _tmp253=_tmp252->break_lab;_tmp254=_tmp252->continue_lab;_tmp255=_tmp252->fallthru_info;_tmp256=_tmp252->varmap;_tmp257=_tmp252->toplevel;_tmp258=_tmp252->in_lhs;_tmp259=_tmp252->struct_info;_tmp25A=_tmp252->in_sizeof;_LL15C:;
return({struct Cyc_Toc_Env*_tmp25B=_region_malloc(r,sizeof(*_tmp25B));_tmp25B->break_lab=(struct _dyneither_ptr**)_tmp253;_tmp25B->continue_lab=(struct _dyneither_ptr**)_tmp254;_tmp25B->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp255;_tmp25B->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp256);_tmp25B->toplevel=(int)1;_tmp25B->in_lhs=(int*)_tmp258;_tmp25B->struct_info=(struct Cyc_Toc_StructInfo)_tmp259;_tmp25B->in_sizeof=(int*)_tmp25A;_tmp25B->rgn=(struct _RegionHandle*)r;_tmp25B;});}
# 1273
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp25C=e;int*_tmp25D;_LL15E: _tmp25D=_tmp25C->in_lhs;_LL15F:;
*_tmp25D=b;}
# 1277
static int Cyc_Toc_set_in_sizeof(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp25E=e;int*_tmp25F;_LL161: _tmp25F=_tmp25E->in_sizeof;_LL162:;{
int _tmp260=*_tmp25F;
*_tmp25F=b;
return _tmp260;};}
# 1283
static int Cyc_Toc_in_sizeof(struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp261=e;int*_tmp262;_LL164: _tmp262=_tmp261->in_sizeof;_LL165:;
return*_tmp262;}
# 1288
static struct Cyc_Toc_Env*Cyc_Toc_set_lhs_exp(struct _RegionHandle*r,struct Cyc_Absyn_Exp*exp,struct Cyc_Toc_Env*e){
# 1290
struct Cyc_Toc_Env*_tmp263=e;struct _dyneither_ptr**_tmp264;struct _dyneither_ptr**_tmp265;struct Cyc_Toc_FallthruInfo*_tmp266;struct Cyc_Dict_Dict _tmp267;int _tmp268;int*_tmp269;struct Cyc_Absyn_Exp**_tmp26A;int*_tmp26B;_LL167: _tmp264=_tmp263->break_lab;_tmp265=_tmp263->continue_lab;_tmp266=_tmp263->fallthru_info;_tmp267=_tmp263->varmap;_tmp268=_tmp263->toplevel;_tmp269=_tmp263->in_lhs;_tmp26A=(_tmp263->struct_info).varsizeexp;_tmp26B=_tmp263->in_sizeof;_LL168:;
return({struct Cyc_Toc_Env*_tmp26C=_region_malloc(r,sizeof(*_tmp26C));_tmp26C->break_lab=(struct _dyneither_ptr**)_tmp264;_tmp26C->continue_lab=(struct _dyneither_ptr**)_tmp265;_tmp26C->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp266;_tmp26C->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp267);_tmp26C->toplevel=(int)_tmp268;_tmp26C->in_lhs=(int*)_tmp269;_tmp26C->struct_info=(struct Cyc_Toc_StructInfo)({(_tmp26C->struct_info).varsizeexp=_tmp26A;(_tmp26C->struct_info).lhs_exp=exp;_tmp26C->struct_info;});_tmp26C->in_sizeof=(int*)_tmp26B;_tmp26C->rgn=(struct _RegionHandle*)r;_tmp26C;});}
# 1296
static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple1*x,struct Cyc_Absyn_Exp*y){
{union Cyc_Absyn_Nmspace _tmp26D=(*x).f1;union Cyc_Absyn_Nmspace _tmp26E=_tmp26D;if((_tmp26E.Rel_n).tag == 1){_LL16A: _LL16B:
# 1299
({struct Cyc_String_pa_PrintArg_struct _tmp271;_tmp271.tag=0;_tmp271.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(x));({void*_tmp26F[1]={& _tmp271};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp270="Toc::add_varmap on Rel_n: %s\n";_tag_dyneither(_tmp270,sizeof(char),30);}),_tag_dyneither(_tmp26F,sizeof(void*),1));});});}else{_LL16C: _LL16D:
 goto _LL169;}_LL169:;}{
# 1302
struct Cyc_Toc_Env*_tmp272=e;struct _dyneither_ptr**_tmp273;struct _dyneither_ptr**_tmp274;struct Cyc_Toc_FallthruInfo*_tmp275;struct Cyc_Dict_Dict _tmp276;int _tmp277;int*_tmp278;struct Cyc_Toc_StructInfo _tmp279;int*_tmp27A;_LL16F: _tmp273=_tmp272->break_lab;_tmp274=_tmp272->continue_lab;_tmp275=_tmp272->fallthru_info;_tmp276=_tmp272->varmap;_tmp277=_tmp272->toplevel;_tmp278=_tmp272->in_lhs;_tmp279=_tmp272->struct_info;_tmp27A=_tmp272->in_sizeof;_LL170:;{
struct Cyc_Dict_Dict _tmp27B=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp276),x,y);
return({struct Cyc_Toc_Env*_tmp27C=_region_malloc(r,sizeof(*_tmp27C));_tmp27C->break_lab=(struct _dyneither_ptr**)_tmp273;_tmp27C->continue_lab=(struct _dyneither_ptr**)_tmp274;_tmp27C->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp275;_tmp27C->varmap=(struct Cyc_Dict_Dict)_tmp27B;_tmp27C->toplevel=(int)_tmp277;_tmp27C->in_lhs=(int*)_tmp278;_tmp27C->struct_info=(struct Cyc_Toc_StructInfo)_tmp279;_tmp27C->in_sizeof=(int*)_tmp27A;_tmp27C->rgn=(struct _RegionHandle*)r;_tmp27C;});};};}
# 1309
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp27D=e;struct _dyneither_ptr**_tmp27E;struct _dyneither_ptr**_tmp27F;struct Cyc_Toc_FallthruInfo*_tmp280;struct Cyc_Dict_Dict _tmp281;int _tmp282;int*_tmp283;struct Cyc_Toc_StructInfo _tmp284;int*_tmp285;_LL172: _tmp27E=_tmp27D->break_lab;_tmp27F=_tmp27D->continue_lab;_tmp280=_tmp27D->fallthru_info;_tmp281=_tmp27D->varmap;_tmp282=_tmp27D->toplevel;_tmp283=_tmp27D->in_lhs;_tmp284=_tmp27D->struct_info;_tmp285=_tmp27D->in_sizeof;_LL173:;
return({struct Cyc_Toc_Env*_tmp286=_region_malloc(r,sizeof(*_tmp286));_tmp286->break_lab=(struct _dyneither_ptr**)0;_tmp286->continue_lab=(struct _dyneither_ptr**)0;_tmp286->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp280;_tmp286->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp281);_tmp286->toplevel=(int)_tmp282;_tmp286->in_lhs=(int*)_tmp283;_tmp286->struct_info=(struct Cyc_Toc_StructInfo)_tmp284;_tmp286->in_sizeof=(int*)_tmp285;_tmp286->rgn=(struct _RegionHandle*)r;_tmp286;});}
# 1315
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*next_case_env){
# 1320
struct Cyc_List_List*fallthru_vars=0;
for(0;fallthru_binders != 0;fallthru_binders=fallthru_binders->tl){
fallthru_vars=({struct Cyc_List_List*_tmp287=_region_malloc(r,sizeof(*_tmp287));_tmp287->hd=((struct Cyc_Absyn_Vardecl*)fallthru_binders->hd)->name;_tmp287->tl=fallthru_vars;_tmp287;});}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fallthru_vars);{
struct Cyc_Toc_Env*_tmp288=e;struct _dyneither_ptr**_tmp289;struct _dyneither_ptr**_tmp28A;struct Cyc_Toc_FallthruInfo*_tmp28B;struct Cyc_Dict_Dict _tmp28C;int _tmp28D;int*_tmp28E;struct Cyc_Toc_StructInfo _tmp28F;int*_tmp290;_LL175: _tmp289=_tmp288->break_lab;_tmp28A=_tmp288->continue_lab;_tmp28B=_tmp288->fallthru_info;_tmp28C=_tmp288->varmap;_tmp28D=_tmp288->toplevel;_tmp28E=_tmp288->in_lhs;_tmp28F=_tmp288->struct_info;_tmp290=_tmp288->in_sizeof;_LL176:;{
struct Cyc_Toc_Env*_tmp291=next_case_env;struct Cyc_Dict_Dict _tmp292;_LL178: _tmp292=_tmp291->varmap;_LL179:;{
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp295=_region_malloc(r,sizeof(*_tmp295));_tmp295->label=fallthru_l;_tmp295->binders=fallthru_vars;_tmp295->next_case_env=
# 1328
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp292);_tmp295;});
return({struct Cyc_Toc_Env*_tmp293=_region_malloc(r,sizeof(*_tmp293));_tmp293->break_lab=(struct _dyneither_ptr**)({struct _dyneither_ptr**_tmp294=_region_malloc(r,sizeof(*_tmp294));_tmp294[0]=break_l;_tmp294;});_tmp293->continue_lab=(struct _dyneither_ptr**)_tmp28A;_tmp293->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi;_tmp293->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp28C);_tmp293->toplevel=(int)_tmp28D;_tmp293->in_lhs=(int*)_tmp28E;_tmp293->struct_info=(struct Cyc_Toc_StructInfo)_tmp28F;_tmp293->in_sizeof=(int*)_tmp290;_tmp293->rgn=(struct _RegionHandle*)r;_tmp293;});};};};}
# 1332
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1335
struct Cyc_Toc_Env*_tmp296=e;struct _dyneither_ptr**_tmp297;struct _dyneither_ptr**_tmp298;struct Cyc_Toc_FallthruInfo*_tmp299;struct Cyc_Dict_Dict _tmp29A;int _tmp29B;int*_tmp29C;struct Cyc_Toc_StructInfo _tmp29D;int*_tmp29E;_LL17B: _tmp297=_tmp296->break_lab;_tmp298=_tmp296->continue_lab;_tmp299=_tmp296->fallthru_info;_tmp29A=_tmp296->varmap;_tmp29B=_tmp296->toplevel;_tmp29C=_tmp296->in_lhs;_tmp29D=_tmp296->struct_info;_tmp29E=_tmp296->in_sizeof;_LL17C:;
return({struct Cyc_Toc_Env*_tmp29F=_region_malloc(r,sizeof(*_tmp29F));_tmp29F->break_lab=(struct _dyneither_ptr**)({struct _dyneither_ptr**_tmp2A0=_region_malloc(r,sizeof(*_tmp2A0));_tmp2A0[0]=break_l;_tmp2A0;});_tmp29F->continue_lab=(struct _dyneither_ptr**)_tmp298;_tmp29F->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0;_tmp29F->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp29A);_tmp29F->toplevel=(int)_tmp29B;_tmp29F->in_lhs=(int*)_tmp29C;_tmp29F->struct_info=(struct Cyc_Toc_StructInfo)_tmp29D;_tmp29F->in_sizeof=(int*)_tmp29E;_tmp29F->rgn=(struct _RegionHandle*)r;_tmp29F;});}
# 1342
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1345
struct Cyc_Toc_Env*_tmp2A1=e;struct _dyneither_ptr**_tmp2A2;struct _dyneither_ptr**_tmp2A3;struct Cyc_Toc_FallthruInfo*_tmp2A4;struct Cyc_Dict_Dict _tmp2A5;int _tmp2A6;int*_tmp2A7;struct Cyc_Toc_StructInfo _tmp2A8;int*_tmp2A9;_LL17E: _tmp2A2=_tmp2A1->break_lab;_tmp2A3=_tmp2A1->continue_lab;_tmp2A4=_tmp2A1->fallthru_info;_tmp2A5=_tmp2A1->varmap;_tmp2A6=_tmp2A1->toplevel;_tmp2A7=_tmp2A1->in_lhs;_tmp2A8=_tmp2A1->struct_info;_tmp2A9=_tmp2A1->in_sizeof;_LL17F:;
return({struct Cyc_Toc_Env*_tmp2AA=_region_malloc(r,sizeof(*_tmp2AA));_tmp2AA->break_lab=(struct _dyneither_ptr**)0;_tmp2AA->continue_lab=(struct _dyneither_ptr**)_tmp2A3;_tmp2AA->fallthru_info=(struct Cyc_Toc_FallthruInfo*)({struct Cyc_Toc_FallthruInfo*_tmp2AB=_region_malloc(r,sizeof(*_tmp2AB));_tmp2AB->label=next_l;_tmp2AB->binders=0;_tmp2AB->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp);_tmp2AB;});_tmp2AA->varmap=(struct Cyc_Dict_Dict)
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2A5);_tmp2AA->toplevel=(int)_tmp2A6;_tmp2AA->in_lhs=(int*)_tmp2A7;_tmp2AA->struct_info=(struct Cyc_Toc_StructInfo)_tmp2A8;_tmp2AA->in_sizeof=(int*)_tmp2A9;_tmp2AA->rgn=(struct _RegionHandle*)r;_tmp2AA;});}
# 1360 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1363
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
# 1365
void*_tmp2AC=e->annot;void*_tmp2AD=_tmp2AC;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2AD)->tag == Cyc_CfFlowInfo_UnknownZ){_LL181: _LL182:
# 1368
 return Cyc_Toc_is_nullable((void*)_check_null(e->topt));}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2AD)->tag == Cyc_CfFlowInfo_NotZero){_LL183: _LL184:
# 1371
 return 0;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2AD)->tag == Cyc_CfFlowInfo_IsZero){_LL185: _LL186:
# 1373
({void*_tmp2AE=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp2AF="dereference of NULL pointer";_tag_dyneither(_tmp2AF,sizeof(char),28);}),_tag_dyneither(_tmp2AE,sizeof(void*),0));});
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2AD)->tag == Cyc_Absyn_EmptyAnnot){_LL187: _LL188:
# 1379
 return 1;}else{_LL189: _LL18A:
({void*_tmp2B0=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2B1="need_null_check";_tag_dyneither(_tmp2B1,sizeof(char),16);}),_tag_dyneither(_tmp2B0,sizeof(void*),0));});}}}}_LL180:;}
# 1384
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp2B2=e->annot;void*_tmp2B3=_tmp2B2;struct Cyc_List_List*_tmp2B4;struct Cyc_List_List*_tmp2B5;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2B3)->tag == Cyc_CfFlowInfo_UnknownZ){_LL18C: _tmp2B5=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2B3)->f1;_LL18D:
 return _tmp2B5;}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2B3)->tag == Cyc_CfFlowInfo_NotZero){_LL18E: _tmp2B4=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2B3)->f1;_LL18F:
 return _tmp2B4;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2B3)->tag == Cyc_CfFlowInfo_IsZero){_LL190: _LL191:
# 1389
({void*_tmp2B6=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp2B7="dereference of NULL pointer";_tag_dyneither(_tmp2B7,sizeof(char),28);}),_tag_dyneither(_tmp2B6,sizeof(void*),0));});
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2B3)->tag == Cyc_Absyn_EmptyAnnot){_LL192: _LL193:
 return 0;}else{_LL194: _LL195:
({void*_tmp2B8=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2B9="get_relns";_tag_dyneither(_tmp2B9,sizeof(char),10);}),_tag_dyneither(_tmp2B8,sizeof(void*),0));});}}}}_LL18B:;}static char _tmp2BE[8]="*bogus*";
# 1399
static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
# 1409 "toc.cyc"
union Cyc_Relations_RelnOp rop_i=Cyc_Relations_RConst(0);
int valid_rop_i=Cyc_Relations_exp2relnop(i,& rop_i);
# 1412
struct Cyc_Absyn_Vardecl*x;
{void*_tmp2BA=a->r;void*_tmp2BB=_tmp2BA;void*_tmp2BC;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2BB)->tag == 1){_LL197: _tmp2BC=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2BB)->f2;_LL198: {
# 1415
struct Cyc_Absyn_Vardecl*_tmp2BD=Cyc_Tcutil_nonesc_vardecl(_tmp2BC);
if(_tmp2BD == 0)goto _LL19A;
x=_tmp2BD;
goto _LL196;}}else{_LL199: _LL19A: {
# 1421
static struct _dyneither_ptr bogus_string={_tmp2BE,_tmp2BE,_tmp2BE + 8};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
# 1424
static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};
# 1430
x=& bogus_vardecl;
x->type=a_typ;}}_LL196:;}{
# 1433
void*_tmp2BF=a_typ;
union Cyc_Relations_RelnOp rop_a=Cyc_Relations_RNumelts(x);
# 1438
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound(_tmp2BF);
if(bound != 0){
union Cyc_Relations_RelnOp rop_bound=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(bound,& rop_bound))
relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_bound,Cyc_Relations_Rlte,rop_a,relns);}
# 1447
if(valid_rop_i){
struct Cyc_List_List*_tmp2C0=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlte,rop_i,relns);
# 1455
if(!Cyc_Relations_consistent_relations(_tmp2C0))return 1;}
# 1458
inner_loop: {
void*_tmp2C1=i->r;void*_tmp2C2=_tmp2C1;struct Cyc_Absyn_Exp*_tmp2C3;struct Cyc_Absyn_Exp*_tmp2C4;void*_tmp2C5;struct Cyc_Absyn_Exp*_tmp2C6;switch(*((int*)_tmp2C2)){case 13: _LL19C: _tmp2C5=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2C2)->f1;_tmp2C6=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2C2)->f2;_LL19D:
 i=_tmp2C6;goto inner_loop;case 2: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2C2)->f1 == Cyc_Absyn_Mod){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2C2)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2C2)->f2)->tl != 0){_LL19E: _tmp2C3=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2C2)->f2)->hd;_tmp2C4=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2C2)->f2)->tl)->hd;_LL19F: {
# 1464
union Cyc_Relations_RelnOp rop_z=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmp2C4,& rop_z)){
# 1467
struct Cyc_List_List*_tmp2C7=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlt,rop_z,relns);
# 1469
return !Cyc_Relations_consistent_relations(_tmp2C7);}
# 1471
goto _LL19B;}}else{goto _LL1A0;}}else{goto _LL1A0;}}else{goto _LL1A0;}default: _LL1A0: _LL1A1:
 goto _LL19B;}_LL19B:;}
# 1474
return 0;};}
# 1477
static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp2C8=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2C9="Missing type in primop ";_tag_dyneither(_tmp2C9,sizeof(char),24);}),_tag_dyneither(_tmp2C8,sizeof(void*),0));});
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));}
# 1481
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp2CA=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2CB="Missing type in primop ";_tag_dyneither(_tmp2CB,sizeof(char),24);}),_tag_dyneither(_tmp2CA,sizeof(void*),0));});
return(void*)_check_null(e->topt);}
# 1485
static struct _tuple11*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
return({struct _tuple11*_tmp2CC=_cycalloc(sizeof(*_tmp2CC));_tmp2CC->f1=Cyc_Toc_mt_tq;_tmp2CC->f2=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));_tmp2CC;});}
# 1488
static struct _tuple19*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
Cyc_Toc_exp_to_c(nv,e);
return({struct _tuple19*_tmp2CD=_cycalloc(sizeof(*_tmp2CD));_tmp2CD->f1=0;_tmp2CD->f2=e;_tmp2CD;});}
# 1495
static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic,int do_declare){
# 1498
struct Cyc_Absyn_Exp*eo;
void*t;
if(pointer){
struct Cyc_Absyn_Exp*_tmp2CE=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(x,0),0);
if(*(nv->struct_info).varsizeexp != 0){
_tmp2CE=Cyc_Absyn_add_exp(_tmp2CE,(struct Cyc_Absyn_Exp*)_check_null(*(nv->struct_info).varsizeexp),0);
*(nv->struct_info).varsizeexp=0;}
# 1506
t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);
if(rgnopt == 0  || Cyc_Absyn_no_regions)
eo=is_atomic?Cyc_Toc_malloc_atomic(_tmp2CE): Cyc_Toc_malloc_ptr(_tmp2CE);else{
# 1510
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);
eo=Cyc_Toc_rmalloc_exp(r,_tmp2CE);}}else{
# 1515
t=struct_typ;
eo=0;}
# 1518
if(do_declare){
void*_tmp2CF=x->r;void*_tmp2D0=_tmp2CF;struct _tuple1*_tmp2D1;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D0)->tag == 1){_LL1A3: _tmp2D1=((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D0)->f1;_LL1A4:
# 1521
 return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp2D1,t,eo,s,0),0);}else{_LL1A5: _LL1A6:
# 1523
({void*_tmp2D2=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2D3="bogus x to make_struct";_tag_dyneither(_tmp2D3,sizeof(char),23);}),_tag_dyneither(_tmp2D2,sizeof(void*),0));});}_LL1A2:;}else{
# 1527
if(eo != 0)
return Cyc_Absyn_stmt_exp(Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(x,eo,0),0),s,0),0);else{
# 1530
return Cyc_Absyn_stmt_exp(s,0);}}}
# 1534
static int Cyc_Toc_abstract_aggr(void*typ){
void*_tmp2D4=Cyc_Tcutil_compress(typ);void*_tmp2D5=_tmp2D4;struct Cyc_List_List*_tmp2D6;union Cyc_Absyn_AggrInfoU _tmp2D7;struct Cyc_List_List*_tmp2D8;switch(*((int*)_tmp2D5)){case 11: _LL1A8: _tmp2D7=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2D5)->f1).aggr_info;_tmp2D8=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2D5)->f1).targs;_LL1A9: {
# 1537
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(_tmp2D7);
if(ad->impl == 0)return 0;{
struct Cyc_List_List*aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
_tmp2D6=aggrfields;goto _LL1AB;};}case 12: _LL1AA: _tmp2D6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D5)->f2;_LL1AB:
# 1542
 if(_tmp2D6 == 0)return 0;
return 1;default: _LL1AC: _LL1AD:
# 1548
 return 0;}_LL1A7:;}
# 1552
static void*Cyc_Toc_array_elt_type(void*t){
void*_tmp2D9=Cyc_Tcutil_compress(t);void*_tmp2DA=_tmp2D9;void*_tmp2DB;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2DA)->tag == 8){_LL1AF: _tmp2DB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2DA)->f1).elt_type;_LL1B0:
 return _tmp2DB;}else{_LL1B1: _LL1B2:
({struct Cyc_String_pa_PrintArg_struct _tmp2DE;_tmp2DE.tag=0;_tmp2DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp2DC[1]={& _tmp2DE};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2DD="array_elt_type called on %s";_tag_dyneither(_tmp2DD,sizeof(char),28);}),_tag_dyneither(_tmp2DC,sizeof(void*),1));});});}_LL1AE:;}
# 1559
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated);
# 1565
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s);
# 1570
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,void*array_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){
# 1575
void*orig_elt_type=Cyc_Toc_array_elt_type(array_type);
int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;
{struct Cyc_List_List*_tmp2DF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;_tmp2DF != 0;_tmp2DF=_tmp2DF->tl){
# 1579
struct _tuple19*_tmp2E0=(struct _tuple19*)_tmp2DF->hd;struct _tuple19*_tmp2E1=_tmp2E0;struct Cyc_List_List*_tmp2E2;struct Cyc_Absyn_Exp*_tmp2E3;_LL1B4: _tmp2E2=_tmp2E1->f1;_tmp2E3=_tmp2E1->f2;_LL1B5:;{
# 1583
struct Cyc_Absyn_Exp*e_index;
if(_tmp2E2 == 0)
e_index=Cyc_Absyn_signed_int_exp(count --,0);else{
# 1587
if(_tmp2E2->tl != 0)({void*_tmp2E4=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp2E5="multiple designators in array";_tag_dyneither(_tmp2E5,sizeof(char),30);}),_tag_dyneither(_tmp2E4,sizeof(void*),0));});{
void*_tmp2E6=(void*)_tmp2E2->hd;
void*_tmp2E7=_tmp2E6;struct Cyc_Absyn_Exp*_tmp2E8;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2E7)->tag == 0){_LL1B7: _tmp2E8=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2E7)->f1;_LL1B8:
# 1591
 Cyc_Toc_exp_to_c(nv,_tmp2E8);
e_index=_tmp2E8;
goto _LL1B6;}else{_LL1B9: _LL1BA:
({void*_tmp2E9=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp2EA="field name designators in array";_tag_dyneither(_tmp2EA,sizeof(char),32);}),_tag_dyneither(_tmp2E9,sizeof(void*),0));});}_LL1B6:;};}{
# 1597
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);
void*_tmp2EB=_tmp2E3->r;void*_tmp2EC=_tmp2EB;void*_tmp2ED;struct Cyc_List_List*_tmp2EE;struct Cyc_Absyn_Vardecl*_tmp2EF;struct Cyc_Absyn_Exp*_tmp2F0;struct Cyc_Absyn_Exp*_tmp2F1;int _tmp2F2;struct Cyc_List_List*_tmp2F3;switch(*((int*)_tmp2EC)){case 25: _LL1BC: _tmp2F3=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp2EC)->f1;_LL1BD:
# 1600
 s=Cyc_Toc_init_array(nv,orig_elt_type,lval,_tmp2F3,s);
goto _LL1BB;case 26: _LL1BE: _tmp2EF=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2EC)->f1;_tmp2F0=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2EC)->f2;_tmp2F1=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2EC)->f3;_tmp2F2=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2EC)->f4;_LL1BF:
# 1603
 s=Cyc_Toc_init_comprehension(nv,orig_elt_type,lval,_tmp2EF,_tmp2F0,_tmp2F1,_tmp2F2,s,0);
# 1605
goto _LL1BB;case 29: _LL1C0: _tmp2ED=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2EC)->f1;_tmp2EE=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2EC)->f2;_LL1C1:
# 1607
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp2ED,_tmp2EE,s);
goto _LL1BB;case 27: _LL1C2: _LL1C3:
# 1610
 goto _LL1BB;default: _LL1C4: _LL1C5:
# 1612
 Cyc_Toc_exp_to_c(nv,_tmp2E3);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(orig_elt_type),_tmp2E3),0),s,0);
goto _LL1BB;}_LL1BB:;};};}}
# 1618
return s;}
# 1623
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated){
# 1628
struct _tuple1*_tmp2F4=vd->name;
void*expected_elt_type;
{void*_tmp2F5=Cyc_Tcutil_compress(comprehension_type);void*_tmp2F6=_tmp2F5;void*_tmp2F7;void*_tmp2F8;switch(*((int*)_tmp2F6)){case 8: _LL1C7: _tmp2F8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F6)->f1).elt_type;_LL1C8:
# 1632
 expected_elt_type=_tmp2F8;goto _LL1C6;case 5: _LL1C9: _tmp2F7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F6)->f1).elt_typ;_LL1CA:
# 1634
 expected_elt_type=_tmp2F7;goto _LL1C6;default: _LL1CB: _LL1CC:
({void*_tmp2F9=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2FA="init_comprehension passed a bad type!";_tag_dyneither(_tmp2FA,sizeof(char),38);}),_tag_dyneither(_tmp2F9,sizeof(void*),0));});}_LL1C6:;}{
# 1637
void*_tmp2FB=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));
if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);
# 1641
{void*_tmp2FC=e2->r;void*_tmp2FD=_tmp2FC;if(((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2FD)->tag == 27){_LL1CE: _LL1CF:
 return Cyc_Absyn_skip_stmt(0);}else{_LL1D0: _LL1D1:
 goto _LL1CD;}_LL1CD:;}{
# 1645
struct _RegionHandle _tmp2FE=_new_region("r2");struct _RegionHandle*r2=& _tmp2FE;_push_region(r2);
{struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(r2,nv,_tmp2F4,Cyc_Absyn_varb_exp(_tmp2F4,(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp309=_cycalloc(sizeof(*_tmp309));_tmp309[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp30A;_tmp30A.tag=4;_tmp30A.f1=vd;_tmp30A;});_tmp309;}),0));
struct _tuple1*max=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp2F4,0),Cyc_Absyn_signed_int_exp(0,0),0);
struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp2F4,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp2F4,0),0);
# 1653
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp2F4,0),0);
struct Cyc_Absyn_Stmt*body;
# 1657
{void*_tmp2FF=e2->r;void*_tmp300=_tmp2FF;void*_tmp301;struct Cyc_List_List*_tmp302;struct Cyc_Absyn_Vardecl*_tmp303;struct Cyc_Absyn_Exp*_tmp304;struct Cyc_Absyn_Exp*_tmp305;int _tmp306;struct Cyc_List_List*_tmp307;switch(*((int*)_tmp300)){case 25: _LL1D3: _tmp307=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_LL1D4:
# 1659
 body=Cyc_Toc_init_array(nv2,expected_elt_type,lval,_tmp307,Cyc_Toc_skip_stmt_dl());
goto _LL1D2;case 26: _LL1D5: _tmp303=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_tmp304=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp300)->f2;_tmp305=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp300)->f3;_tmp306=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp300)->f4;_LL1D6:
# 1662
 body=Cyc_Toc_init_comprehension(nv2,expected_elt_type,lval,_tmp303,_tmp304,_tmp305,_tmp306,Cyc_Toc_skip_stmt_dl(),0);
goto _LL1D2;case 29: _LL1D7: _tmp301=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_tmp302=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp300)->f2;_LL1D8:
# 1665
 body=Cyc_Toc_init_anon_struct(nv,lval,_tmp301,_tmp302,Cyc_Toc_skip_stmt_dl());
goto _LL1D2;default: _LL1D9: _LL1DA:
# 1668
 Cyc_Toc_exp_to_c(nv2,e2);
body=Cyc_Absyn_assign_stmt(lval,Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(expected_elt_type),e2),0);
goto _LL1D2;}_LL1D2:;}{
# 1672
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0);
# 1674
if(zero_term){
# 1679
struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(_tmp2FB,Cyc_Absyn_uint_exp(0,0)),0);
s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}{
# 1684
struct Cyc_Absyn_Stmt*_tmp308=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,e1,
Cyc_Absyn_declare_stmt(_tmp2F4,Cyc_Absyn_uint_typ,0,s2,0),0),s,0);_npop_handler(0);return _tmp308;};};}
# 1646
;_pop_region(r2);};};}
# 1690
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s){
# 1693
{struct Cyc_List_List*_tmp30B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles);for(0;_tmp30B != 0;_tmp30B=_tmp30B->tl){
struct _tuple19*_tmp30C=(struct _tuple19*)_tmp30B->hd;struct _tuple19*_tmp30D=_tmp30C;struct Cyc_List_List*_tmp30E;struct Cyc_Absyn_Exp*_tmp30F;_LL1DC: _tmp30E=_tmp30D->f1;_tmp30F=_tmp30D->f2;_LL1DD:;
if(_tmp30E == 0)
({void*_tmp310=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp311="empty designator list";_tag_dyneither(_tmp311,sizeof(char),22);}),_tag_dyneither(_tmp310,sizeof(void*),0));});
if(_tmp30E->tl != 0)
({void*_tmp312=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp313="too many designators in anonymous struct";_tag_dyneither(_tmp313,sizeof(char),41);}),_tag_dyneither(_tmp312,sizeof(void*),0));});{
void*_tmp314=(void*)_tmp30E->hd;void*_tmp315=_tmp314;struct _dyneither_ptr*_tmp316;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp315)->tag == 1){_LL1DF: _tmp316=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp315)->f1;_LL1E0: {
# 1701
struct Cyc_Absyn_Exp*lval=Cyc_Toc_member_exp(lhs,_tmp316,0);
{void*_tmp317=_tmp30F->r;void*_tmp318=_tmp317;void*_tmp319;struct Cyc_List_List*_tmp31A;struct Cyc_Absyn_Vardecl*_tmp31B;struct Cyc_Absyn_Exp*_tmp31C;struct Cyc_Absyn_Exp*_tmp31D;int _tmp31E;struct Cyc_List_List*_tmp31F;switch(*((int*)_tmp318)){case 25: _LL1E4: _tmp31F=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp318)->f1;_LL1E5:
# 1704
 s=Cyc_Toc_init_array(nv,(void*)_check_null(_tmp30F->topt),lval,_tmp31F,s);goto _LL1E3;case 26: _LL1E6: _tmp31B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp318)->f1;_tmp31C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp318)->f2;_tmp31D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp318)->f3;_tmp31E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp318)->f4;_LL1E7:
# 1706
 s=Cyc_Toc_init_comprehension(nv,(void*)_check_null(_tmp30F->topt),lval,_tmp31B,_tmp31C,_tmp31D,_tmp31E,s,0);
goto _LL1E3;case 29: _LL1E8: _tmp319=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp318)->f1;_tmp31A=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp318)->f2;_LL1E9:
# 1709
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp319,_tmp31A,s);goto _LL1E3;case 27: _LL1EA: _LL1EB:
# 1711
 goto _LL1E3;default: _LL1EC: _LL1ED:
# 1713
 Cyc_Toc_exp_to_c(nv,_tmp30F);
# 1715
if(Cyc_Toc_is_poly_field(struct_type,_tmp316))
_tmp30F=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp30F);
# 1718
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp30F,0),0),s,0);
goto _LL1E3;}_LL1E3:;}
# 1721
goto _LL1DE;}}else{_LL1E1: _LL1E2:
({void*_tmp320=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp321="array designator in struct";_tag_dyneither(_tmp321,sizeof(char),27);}),_tag_dyneither(_tmp320,sizeof(void*),0));});}_LL1DE:;};}}
# 1725
return s;}
# 1730
static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){
# 1734
struct _RegionHandle _tmp322=_new_region("r");struct _RegionHandle*r=& _tmp322;_push_region(r);
{struct Cyc_List_List*_tmp323=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);
void*_tmp324=Cyc_Toc_add_tuple_type(_tmp323);
# 1738
struct _tuple1*_tmp325=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp326=Cyc_Absyn_var_exp(_tmp325,0);
struct Cyc_Absyn_Stmt*_tmp327=Cyc_Absyn_exp_stmt(_tmp326,0);
# 1742
struct Cyc_Absyn_Exp*(*_tmp328)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
# 1744
int is_atomic=1;
struct Cyc_List_List*_tmp329=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);
{int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp329);for(0;_tmp329 != 0;(_tmp329=_tmp329->tl,-- i)){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp329->hd;
struct Cyc_Absyn_Exp*lval=_tmp328(_tmp326,Cyc_Absyn_fieldname(i),0);
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(e->topt));{
void*_tmp32A=e->r;void*_tmp32B=_tmp32A;struct Cyc_Absyn_Vardecl*_tmp32C;struct Cyc_Absyn_Exp*_tmp32D;struct Cyc_Absyn_Exp*_tmp32E;int _tmp32F;struct Cyc_List_List*_tmp330;switch(*((int*)_tmp32B)){case 25: _LL1EF: _tmp330=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp32B)->f1;_LL1F0:
# 1752
 _tmp327=Cyc_Toc_init_array(nv,(void*)_check_null(e->topt),lval,_tmp330,_tmp327);
goto _LL1EE;case 26: _LL1F1: _tmp32C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp32B)->f1;_tmp32D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp32B)->f2;_tmp32E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp32B)->f3;_tmp32F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp32B)->f4;_LL1F2:
# 1755
 _tmp327=Cyc_Toc_init_comprehension(nv,(void*)_check_null(e->topt),lval,_tmp32C,_tmp32D,_tmp32E,_tmp32F,_tmp327,0);
# 1757
goto _LL1EE;case 27: _LL1F3: _LL1F4:
# 1759
 goto _LL1EE;default: _LL1F5: _LL1F6:
# 1762
 Cyc_Toc_exp_to_c(nv,e);
_tmp327=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp328(_tmp326,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp327,0);
# 1765
goto _LL1EE;}_LL1EE:;};}}{
# 1768
struct Cyc_Absyn_Exp*_tmp331=Cyc_Toc_make_struct(nv,Cyc_Absyn_var_exp(_tmp325,0),_tmp324,_tmp327,pointer,rgnopt,is_atomic,1);_npop_handler(0);return _tmp331;};}
# 1735
;_pop_region(r);}
# 1772
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp332=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp332 != 0;_tmp332=_tmp332->tl){
struct Cyc_Absyn_Aggrfield*_tmp333=(struct Cyc_Absyn_Aggrfield*)_tmp332->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp333->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}
# 1779
({void*_tmp334=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp337;_tmp337.tag=0;_tmp337.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp335[1]={& _tmp337};Cyc_aprintf(({const char*_tmp336="get_member_offset %s failed";_tag_dyneither(_tmp336,sizeof(char),28);}),_tag_dyneither(_tmp335,sizeof(void*),1));});}),_tag_dyneither(_tmp334,sizeof(void*),0));});}
# 1782
static int Cyc_Toc_init_variable_array(void**typ){
{void*_tmp338=Cyc_Tcutil_compress(*typ);void*_tmp339=_tmp338;void*_tmp33A;struct Cyc_Absyn_Tqual _tmp33B;struct Cyc_Absyn_Exp*_tmp33C;union Cyc_Absyn_Constraint*_tmp33D;unsigned int _tmp33E;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp339)->tag == 8){_LL1F8: _tmp33A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp339)->f1).elt_type;_tmp33B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp339)->f1).tq;_tmp33C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp339)->f1).num_elts;_tmp33D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp339)->f1).zero_term;_tmp33E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp339)->f1).zt_loc;_LL1F9:
# 1787
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp33C))){
*typ=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp33F=_cycalloc(sizeof(*_tmp33F));_tmp33F[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp340;_tmp340.tag=8;_tmp340.f1=({struct Cyc_Absyn_ArrayInfo _tmp341;_tmp341.elt_type=_tmp33A;_tmp341.tq=_tmp33B;_tmp341.num_elts=Cyc_Absyn_uint_exp(0,0);_tmp341.zero_term=_tmp33D;_tmp341.zt_loc=_tmp33E;_tmp341;});_tmp340;});_tmp33F;});
return 1;}
# 1791
goto _LL1F7;}else{_LL1FA: _LL1FB:
 goto _LL1F7;}_LL1F7:;}
# 1794
return 0;}
# 1797
static int Cyc_Toc_is_array_type(void*t){
void*_tmp342=Cyc_Tcutil_compress(t);void*_tmp343=_tmp342;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp343)->tag == 8){_LL1FD: _LL1FE:
 return 1;}else{_LL1FF: _LL200:
 return 0;}_LL1FC:;}
# 1804
struct Cyc_Absyn_Aggrdecl*Cyc_Toc_update_aggr_type(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*fn,void*new_field_type){
# 1806
struct Cyc_List_List*new_fields=0;
struct Cyc_List_List*fields;
for(fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;fields != 0;fields=fields->tl){
# 1810
if(!Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,fn)){
struct Cyc_Absyn_Aggrfield*_tmp344=(struct Cyc_Absyn_Aggrfield*)fields->hd;
struct Cyc_List_List*_tmp345=_tmp344->attributes;
void*_tmp346=_tmp344->type;
if((!Cyc_Toc_is_array_type(_tmp346),Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp346))))
_tmp345=({struct Cyc_List_List*_tmp347=_cycalloc(sizeof(*_tmp347));_tmp347->hd=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp348=_cycalloc(sizeof(*_tmp348));_tmp348[0]=({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmp349;_tmp349.tag=6;_tmp349.f1=0;_tmp349;});_tmp348;});_tmp347->tl=_tmp345;_tmp347;});{
struct Cyc_Absyn_Aggrfield*_tmp34A=({struct Cyc_Absyn_Aggrfield*_tmp34C=_cycalloc(sizeof(*_tmp34C));_tmp34C->name=_tmp344->name;_tmp34C->tq=Cyc_Toc_mt_tq;_tmp34C->type=new_field_type;_tmp34C->width=_tmp344->width;_tmp34C->attributes=_tmp345;_tmp34C->requires_clause=0;_tmp34C;});
# 1822
new_fields=({struct Cyc_List_List*_tmp34B=_cycalloc(sizeof(*_tmp34B));_tmp34B->hd=_tmp34A;_tmp34B->tl=new_fields;_tmp34B;});};}else{
# 1824
new_fields=({struct Cyc_List_List*_tmp34D=_cycalloc(sizeof(*_tmp34D));_tmp34D->hd=(struct Cyc_Absyn_Aggrfield*)fields->hd;_tmp34D->tl=new_fields;_tmp34D;});}}{
# 1826
struct Cyc_List_List*_tmp34E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);
struct _dyneither_ptr*name=({struct _dyneither_ptr*_tmp353=_cycalloc(sizeof(*_tmp353));_tmp353[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp356;_tmp356.tag=1;_tmp356.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp354[1]={& _tmp356};Cyc_aprintf(({const char*_tmp355="_genStruct%d";_tag_dyneither(_tmp355,sizeof(char),13);}),_tag_dyneither(_tmp354,sizeof(void*),1));});});_tmp353;});
# 1829
struct _tuple1*qv=({struct _tuple1*_tmp352=_cycalloc(sizeof(*_tmp352));_tmp352->f1=Cyc_Absyn_Abs_n(0,1);_tmp352->f2=name;_tmp352;});
struct Cyc_Absyn_AggrdeclImpl*_tmp34F=({struct Cyc_Absyn_AggrdeclImpl*_tmp351=_cycalloc(sizeof(*_tmp351));_tmp351->exist_vars=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars;_tmp351->rgn_po=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;_tmp351->fields=_tmp34E;_tmp351->tagged=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;_tmp351;});
# 1834
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp350=_cycalloc(sizeof(*_tmp350));_tmp350->kind=ad->kind;_tmp350->sc=ad->sc;_tmp350->name=qv;_tmp350->tvs=ad->tvs;_tmp350->impl=_tmp34F;_tmp350->expected_mem_kind=0;_tmp350->attributes=ad->attributes;_tmp350;});
# 1841
Cyc_Toc_aggrdecl_to_c(new_ad,1);
return new_ad;};}
# 1845
static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*nv,void*struct_type,struct Cyc_List_List*exist_types,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*dles,struct _tuple1*tdn){
# 1851
int do_declare=0;
struct Cyc_Absyn_Exp*xexp;
if((nv->struct_info).lhs_exp == 0){
struct _tuple1*_tmp357=Cyc_Toc_temp_var();
xexp=Cyc_Absyn_var_exp(_tmp357,0);
do_declare=1;}else{
# 1859
xexp=(struct Cyc_Absyn_Exp*)_check_null((nv->struct_info).lhs_exp);
nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);}{
# 1862
struct Cyc_Absyn_Stmt*_tmp358=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(xexp),0);
# 1866
struct Cyc_Absyn_Exp**varexp;
{void*_tmp359=_tmp358->r;void*_tmp35A=_tmp359;struct Cyc_Absyn_Exp**_tmp35B;if(((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp35A)->tag == 1){_LL202: _tmp35B=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp35A)->f1;_LL203:
 varexp=_tmp35B;goto _LL201;}else{_LL204: _LL205:
({void*_tmp35C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp35D="stmt not an expression!";_tag_dyneither(_tmp35D,sizeof(char),24);}),_tag_dyneither(_tmp35C,sizeof(void*),0));});}_LL201:;}{
# 1872
struct Cyc_Absyn_Exp*(*_tmp35E)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
void*aggr_typ=Cyc_Toc_typ_to_c(struct_type);
void*orig_typ=aggr_typ;
# 1876
int is_atomic=1;
struct Cyc_List_List*forall_types;
struct Cyc_Absyn_Aggrdecl*ad;
struct Cyc_List_List*aggrfields;struct Cyc_List_List*orig_aggrfields;
struct _dyneither_ptr**index=0;
{void*_tmp35F=Cyc_Tcutil_compress(struct_type);void*_tmp360=_tmp35F;union Cyc_Absyn_AggrInfoU _tmp361;struct Cyc_List_List*_tmp362;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp360)->tag == 11){_LL207: _tmp361=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp360)->f1).aggr_info;_tmp362=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp360)->f1).targs;_LL208:
# 1883
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp361);
orig_aggrfields=(aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
forall_types=_tmp362;
goto _LL206;}else{_LL209: _LL20A:
({void*_tmp363=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp364="init_struct: bad struct type";_tag_dyneither(_tmp364,sizeof(char),29);}),_tag_dyneither(_tmp363,sizeof(void*),0));});}_LL206:;}
# 1890
if(exist_types != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields != 0){
struct Cyc_List_List*_tmp365=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
for(0;_tmp365->tl != 0;_tmp365=_tmp365->tl){;}{
struct Cyc_Absyn_Aggrfield*_tmp366=(struct Cyc_Absyn_Aggrfield*)_tmp365->hd;
struct _RegionHandle _tmp367=_new_region("temp");struct _RegionHandle*temp=& _tmp367;_push_region(temp);
{struct Cyc_List_List*_tmp368=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,forall_types);
# 1899
if(Cyc_Toc_is_abstract_type(Cyc_Tcutil_rsubstitute(temp,_tmp368,_tmp366->type))){
struct Cyc_List_List*_tmp369=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,exist_types);
struct Cyc_List_List*_tmp36A=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(temp,_tmp368,_tmp369);
# 1903
struct Cyc_List_List*new_fields=0;
for(_tmp365=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;_tmp365 != 0;_tmp365=_tmp365->tl){
struct Cyc_Absyn_Aggrfield*_tmp36B=(struct Cyc_Absyn_Aggrfield*)_tmp365->hd;
struct Cyc_Absyn_Aggrfield*_tmp36C=({struct Cyc_Absyn_Aggrfield*_tmp36F=_cycalloc(sizeof(*_tmp36F));_tmp36F->name=_tmp36B->name;_tmp36F->tq=Cyc_Toc_mt_tq;_tmp36F->type=
# 1908
Cyc_Tcutil_rsubstitute(temp,_tmp36A,_tmp36B->type);_tmp36F->width=_tmp36B->width;_tmp36F->attributes=_tmp36B->attributes;_tmp36F->requires_clause=0;_tmp36F;});
# 1916
if(_tmp365->tl == 0){
if(Cyc_Toc_init_variable_array(& _tmp36C->type))
index=({struct _dyneither_ptr**_tmp36D=_cycalloc(sizeof(*_tmp36D));_tmp36D[0]=_tmp36C->name;_tmp36D;});}
# 1923
new_fields=({struct Cyc_List_List*_tmp36E=_cycalloc(sizeof(*_tmp36E));_tmp36E->hd=_tmp36C;_tmp36E->tl=new_fields;_tmp36E;});}
# 1925
exist_types=0;
aggrfields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);{
struct _dyneither_ptr*name=({struct _dyneither_ptr*_tmp378=_cycalloc(sizeof(*_tmp378));_tmp378[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp37B;_tmp37B.tag=1;_tmp37B.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp379[1]={& _tmp37B};Cyc_aprintf(({const char*_tmp37A="_genStruct%d";_tag_dyneither(_tmp37A,sizeof(char),13);}),_tag_dyneither(_tmp379,sizeof(void*),1));});});_tmp378;});
# 1929
struct _tuple1*qv=({struct _tuple1*_tmp377=_cycalloc(sizeof(*_tmp377));_tmp377->f1=Cyc_Absyn_Abs_n(0,1);_tmp377->f2=name;_tmp377;});
struct Cyc_Absyn_AggrdeclImpl*_tmp370=({struct Cyc_Absyn_AggrdeclImpl*_tmp376=_cycalloc(sizeof(*_tmp376));_tmp376->exist_vars=0;_tmp376->rgn_po=0;_tmp376->fields=aggrfields;_tmp376->tagged=0;_tmp376;});
# 1934
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp375=_cycalloc(sizeof(*_tmp375));_tmp375->kind=Cyc_Absyn_StructA;_tmp375->sc=Cyc_Absyn_Public;_tmp375->name=qv;_tmp375->tvs=0;_tmp375->expected_mem_kind=0;_tmp375->impl=_tmp370;_tmp375->attributes=0;_tmp375;});
# 1941
Cyc_Toc_aggrdecl_to_c(new_ad,1);
ad=new_ad;
struct_type=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp371=_cycalloc(sizeof(*_tmp371));_tmp371[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp372;_tmp372.tag=11;_tmp372.f1=({struct Cyc_Absyn_AggrInfo _tmp373;_tmp373.aggr_info=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp374=_cycalloc(sizeof(*_tmp374));_tmp374[0]=new_ad;_tmp374;}));_tmp373.targs=0;_tmp373;});_tmp372;});_tmp371;});
# 1945
aggr_typ=Cyc_Toc_typ_to_c(struct_type);};}}
# 1895
;_pop_region(temp);};}{
# 1949
int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct _RegionHandle _tmp37C=_new_region("r");struct _RegionHandle*r=& _tmp37C;_push_region(r);
{struct Cyc_List_List*_tmp37D=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp37D != 0;_tmp37D=_tmp37D->tl){
struct _tuple19*_tmp37E=(struct _tuple19*)_tmp37D->hd;struct _tuple19*_tmp37F=_tmp37E;struct Cyc_List_List*_tmp380;struct Cyc_Absyn_Exp*_tmp381;_LL20C: _tmp380=_tmp37F->f1;_tmp381=_tmp37F->f2;_LL20D:;
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(_tmp381->topt));
if(_tmp380 == 0)
({void*_tmp382=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp383="empty designator list";_tag_dyneither(_tmp383,sizeof(char),22);}),_tag_dyneither(_tmp382,sizeof(void*),0));});
if(_tmp380->tl != 0){
# 1959
struct _tuple1*_tmp384=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp385=Cyc_Absyn_var_exp(_tmp384,0);
for(0;_tmp380 != 0;_tmp380=_tmp380->tl){
void*_tmp386=(void*)_tmp380->hd;void*_tmp387=_tmp386;struct _dyneither_ptr*_tmp388;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp387)->tag == 1){_LL20F: _tmp388=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp387)->f1;_LL210:
# 1965
 if(Cyc_Toc_is_poly_field(struct_type,_tmp388))
_tmp385=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp385);
_tmp358=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp35E(xexp,_tmp388,0),_tmp385,0),0),_tmp358,0);
# 1969
goto _LL20E;}else{_LL211: _LL212:
({void*_tmp389=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp38A="array designator in struct";_tag_dyneither(_tmp38A,sizeof(char),27);}),_tag_dyneither(_tmp389,sizeof(void*),0));});}_LL20E:;}
# 1973
Cyc_Toc_exp_to_c(nv,_tmp381);
_tmp358=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp385,_tmp381,0),0),_tmp358,0);}else{
# 1976
void*_tmp38B=(void*)_tmp380->hd;void*_tmp38C=_tmp38B;struct _dyneither_ptr*_tmp38D;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp38C)->tag == 1){_LL214: _tmp38D=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp38C)->f1;_LL215: {
# 1978
struct Cyc_Absyn_Exp*lval=_tmp35E(xexp,_tmp38D,0);
if(is_tagged_union){
int i=Cyc_Toc_get_member_offset(ad,_tmp38D);
struct Cyc_Absyn_Exp*f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct Cyc_Absyn_Exp*lhs=Cyc_Toc_member_exp(lval,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,f_tag_exp,0);
_tmp358=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp358,0);
lval=Cyc_Toc_member_exp(lval,Cyc_Toc_val_sp,0);}{
# 1987
int e1_translated=0;
{void*_tmp38E=_tmp381->r;void*_tmp38F=_tmp38E;void*_tmp390;struct Cyc_List_List*_tmp391;struct Cyc_Absyn_Exp*_tmp392;void*_tmp393;struct Cyc_Absyn_Vardecl*_tmp394;struct Cyc_Absyn_Exp*_tmp395;struct Cyc_Absyn_Exp*_tmp396;int _tmp397;struct Cyc_List_List*_tmp398;switch(*((int*)_tmp38F)){case 25: _LL219: _tmp398=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp38F)->f1;_LL21A:
# 1990
 _tmp358=Cyc_Toc_init_array(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(aggrfields,_tmp38D)))->type,lval,_tmp398,_tmp358);
# 1992
goto _LL218;case 26: _LL21B: _tmp394=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp38F)->f1;_tmp395=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp38F)->f2;_tmp396=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp38F)->f3;_tmp397=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp38F)->f4;_LL21C:
# 1994
 _tmp358=Cyc_Toc_init_comprehension(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(do_declare?aggrfields: orig_aggrfields,_tmp38D)))->type,lval,_tmp394,_tmp395,_tmp396,_tmp397,_tmp358,0);
# 1998
e1_translated=1;
_tmp392=_tmp395;_tmp393=(void*)_check_null(_tmp396->topt);goto _LL21E;case 27: _LL21D: _tmp392=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp38F)->f1;_tmp393=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp38F)->f2;_LL21E:
# 2003
 if(index != 0  && !Cyc_strptrcmp(*index,_tmp38D)){
if(!e1_translated)Cyc_Toc_exp_to_c(nv,_tmp392);{
void*_tmp399=Cyc_Toc_typ_to_c(_tmp393);
*(nv->struct_info).varsizeexp=
Cyc_Absyn_add_exp(Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp39A[2];_tmp39A[1]=
Cyc_Absyn_sizeoftyp_exp(_tmp399,0);_tmp39A[0]=_tmp392;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp39A,sizeof(struct Cyc_Absyn_Exp*),2));}),0),
Cyc_Absyn_const_exp(({union Cyc_Absyn_Cnst _tmp39B;(_tmp39B.Int_c).val=({struct _tuple6 _tmp39C;_tmp39C.f1=Cyc_Absyn_Unsigned;_tmp39C.f2=(int)sizeof(double);_tmp39C;});(_tmp39B.Int_c).tag=5;_tmp39B;}),0),0);};}
# 2011
goto _LL218;case 29: _LL21F: _tmp390=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp38F)->f1;_tmp391=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp38F)->f2;_LL220:
# 2013
 _tmp358=Cyc_Toc_init_anon_struct(nv,lval,_tmp390,_tmp391,_tmp358);goto _LL218;default: _LL221: _LL222: {
# 2015
void*old_e_typ=(void*)_check_null(_tmp381->topt);
int was_ptr_type=Cyc_Toc_is_pointer_or_boxed_tvar(old_e_typ);
Cyc_Toc_exp_to_c(Cyc_Toc_set_lhs_exp(nv->rgn,lval,nv),_tmp381);
{void*_tmp39D=old_e_typ;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp39D)->tag == 11){_LL224: _LL225:
# 2022
 if(old_e_typ != _tmp381->topt  && !Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(old_e_typ),(void*)_check_null(_tmp381->topt))){
# 2026
ad=Cyc_Toc_update_aggr_type(ad,_tmp38D,(void*)_check_null(_tmp381->topt));
struct_type=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp39E=_cycalloc(sizeof(*_tmp39E));_tmp39E[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp39F;_tmp39F.tag=11;_tmp39F.f1=({struct Cyc_Absyn_AggrInfo _tmp3A0;_tmp3A0.aggr_info=
Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp3A1=_cycalloc(sizeof(*_tmp3A1));_tmp3A1[0]=ad;_tmp3A1;}));_tmp3A0.targs=0;_tmp3A0;});_tmp39F;});_tmp39E;});
# 2030
aggr_typ=Cyc_Toc_typ_to_c(struct_type);}
# 2032
goto _LL223;}else{_LL226: _LL227:
 goto _LL223;}_LL223:;}{
# 2035
struct Cyc_Absyn_Aggrfield*_tmp3A2=Cyc_Absyn_lookup_field(aggrfields,_tmp38D);
# 2037
if(Cyc_Toc_is_poly_field(struct_type,_tmp38D) && !was_ptr_type)
_tmp381=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp381);
# 2040
if(exist_types != 0)
_tmp381=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3A2))->type),_tmp381);
# 2043
_tmp358=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp381,0),0),_tmp358,0);
goto _LL218;};}}_LL218:;}
# 2046
goto _LL213;};}}else{_LL216: _LL217:
({void*_tmp3A3=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3A4="array designator in struct";_tag_dyneither(_tmp3A4,sizeof(char),27);}),_tag_dyneither(_tmp3A3,sizeof(void*),0));});}_LL213:;}}}
# 2053
if(aggr_typ != orig_typ  && pointer){
void*cast_type=Cyc_Absyn_cstar_typ(orig_typ,Cyc_Toc_mt_tq);
*varexp=Cyc_Toc_cast_it(cast_type,*varexp);}{
# 2058
struct Cyc_Absyn_Exp*_tmp3A5=Cyc_Toc_make_struct(nv,xexp,aggr_typ,_tmp358,pointer,rgnopt,is_atomic,do_declare);
# 2060
_tmp3A5->topt=struct_type;{
struct Cyc_Absyn_Exp*_tmp3A6=_tmp3A5;_npop_handler(0);return _tmp3A6;};};
# 1951
;_pop_region(r);};};};}struct _tuple27{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 2066
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple27*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
# 2069
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0);}
# 2072
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp3A7=_cycalloc(sizeof(*_tmp3A7));_tmp3A7[0]=({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmp3A8;_tmp3A8.tag=4;_tmp3A8.f1=e1;_tmp3A8.f2=incr;_tmp3A8;});_tmp3A7;}),0);}
# 2076
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 2085
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 2087
void*_tmp3A9=e1->r;void*_tmp3AA=_tmp3A9;struct Cyc_Absyn_Exp*_tmp3AB;struct _dyneither_ptr*_tmp3AC;int _tmp3AD;int _tmp3AE;void*_tmp3AF;struct Cyc_Absyn_Exp*_tmp3B0;struct Cyc_Absyn_Stmt*_tmp3B1;switch(*((int*)_tmp3AA)){case 36: _LL229: _tmp3B1=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3AA)->f1;_LL22A:
 Cyc_Toc_lvalue_assign_stmt(_tmp3B1,fs,f,f_env);goto _LL228;case 13: _LL22B: _tmp3AF=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3AA)->f1;_tmp3B0=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3AA)->f2;_LL22C:
 Cyc_Toc_lvalue_assign(_tmp3B0,fs,f,f_env);goto _LL228;case 20: _LL22D: _tmp3AB=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3AA)->f1;_tmp3AC=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3AA)->f2;_tmp3AD=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3AA)->f3;_tmp3AE=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3AA)->f4;_LL22E:
# 2092
 e1->r=_tmp3AB->r;
Cyc_Toc_lvalue_assign(e1,({struct Cyc_List_List*_tmp3B2=_cycalloc(sizeof(*_tmp3B2));_tmp3B2->hd=_tmp3AC;_tmp3B2->tl=fs;_tmp3B2;}),f,f_env);
goto _LL228;default: _LL22F: _LL230: {
# 2100
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 2102
for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;
goto _LL228;}}_LL228:;}
# 2108
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 2110
void*_tmp3B3=s->r;void*_tmp3B4=_tmp3B3;struct Cyc_Absyn_Stmt*_tmp3B5;struct Cyc_Absyn_Decl*_tmp3B6;struct Cyc_Absyn_Stmt*_tmp3B7;struct Cyc_Absyn_Exp*_tmp3B8;switch(*((int*)_tmp3B4)){case 1: _LL232: _tmp3B8=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3B4)->f1;_LL233:
 Cyc_Toc_lvalue_assign(_tmp3B8,fs,f,f_env);goto _LL231;case 12: _LL234: _tmp3B6=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3B4)->f1;_tmp3B7=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3B4)->f2;_LL235:
# 2113
 Cyc_Toc_lvalue_assign_stmt(_tmp3B7,fs,f,f_env);goto _LL231;case 2: _LL236: _tmp3B5=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3B4)->f2;_LL237:
 Cyc_Toc_lvalue_assign_stmt(_tmp3B5,fs,f,f_env);goto _LL231;default: _LL238: _LL239:
({struct Cyc_String_pa_PrintArg_struct _tmp3BB;_tmp3BB.tag=0;_tmp3BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));({void*_tmp3B9[1]={& _tmp3BB};Cyc_Toc_toc_impos(({const char*_tmp3BA="lvalue_assign_stmt: %s";_tag_dyneither(_tmp3BA,sizeof(char),23);}),_tag_dyneither(_tmp3B9,sizeof(void*),1));});});}_LL231:;}
# 2119
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 2123
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp3BC=e->r;void*_tmp3BD=_tmp3BC;struct Cyc_Absyn_Stmt*_tmp3BE;struct Cyc_Absyn_Exp*_tmp3BF;void**_tmp3C0;struct Cyc_Absyn_Exp**_tmp3C1;switch(*((int*)_tmp3BD)){case 13: _LL23B: _tmp3C0=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3BD)->f1;_tmp3C1=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3BD)->f2;_LL23C:
# 2126
*_tmp3C1=Cyc_Toc_push_address_exp(*_tmp3C1);
*_tmp3C0=Cyc_Absyn_cstar_typ(*_tmp3C0,Cyc_Toc_mt_tq);
return e;case 19: _LL23D: _tmp3BF=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3BD)->f1;_LL23E:
# 2130
 return _tmp3BF;case 36: _LL23F: _tmp3BE=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3BD)->f1;_LL240:
# 2134
 Cyc_Toc_push_address_stmt(_tmp3BE);
return e;default: _LL241: _LL242:
# 2137
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);
({struct Cyc_String_pa_PrintArg_struct _tmp3C4;_tmp3C4.tag=0;_tmp3C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp3C2[1]={& _tmp3C4};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3C3="can't take & of exp %s";_tag_dyneither(_tmp3C3,sizeof(char),23);}),_tag_dyneither(_tmp3C2,sizeof(void*),1));});});}_LL23A:;}
# 2142
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp3C5=s->r;void*_tmp3C6=_tmp3C5;struct Cyc_Absyn_Exp**_tmp3C7;struct Cyc_Absyn_Stmt*_tmp3C8;struct Cyc_Absyn_Stmt*_tmp3C9;switch(*((int*)_tmp3C6)){case 2: _LL244: _tmp3C9=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3C6)->f2;_LL245:
 _tmp3C8=_tmp3C9;goto _LL247;case 12: _LL246: _tmp3C8=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3C6)->f2;_LL247:
 Cyc_Toc_push_address_stmt(_tmp3C8);goto _LL243;case 1: _LL248: _tmp3C7=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3C6)->f1;_LL249:
*_tmp3C7=Cyc_Toc_push_address_exp(*_tmp3C7);goto _LL243;default: _LL24A: _LL24B:
# 2148
({struct Cyc_String_pa_PrintArg_struct _tmp3CC;_tmp3CC.tag=0;_tmp3CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));({void*_tmp3CA[1]={& _tmp3CC};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3CB="can't take & of stmt %s";_tag_dyneither(_tmp3CB,sizeof(char),24);}),_tag_dyneither(_tmp3CA,sizeof(void*),1));});});}_LL243:;}
# 2153
static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 2155
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 2157
if(x == 0)return 0;
result=({struct Cyc_List_List*_tmp3CD=_region_malloc(r2,sizeof(*_tmp3CD));_tmp3CD->hd=f(x->hd,env);_tmp3CD->tl=0;_tmp3CD;});
prev=result;
for(x=x->tl;x != 0;x=x->tl){
((struct Cyc_List_List*)_check_null(prev))->tl=({struct Cyc_List_List*_tmp3CE=_region_malloc(r2,sizeof(*_tmp3CE));_tmp3CE->hd=f(x->hd,env);_tmp3CE->tl=0;_tmp3CE;});
prev=prev->tl;}
# 2164
return result;}
# 2166
static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,x);}
# 2170
static struct _tuple19*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){
return({struct _tuple19*_tmp3CF=_cycalloc(sizeof(*_tmp3CF));_tmp3CF->f1=0;_tmp3CF->f2=e;_tmp3CF;});}
# 2174
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_tmp3D0=Cyc_Tcutil_compress(t);void*_tmp3D1=_tmp3D0;struct Cyc_Absyn_PtrInfo _tmp3D2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D1)->tag == 5){_LL24D: _tmp3D2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D1)->f1;_LL24E:
 return _tmp3D2;}else{_LL24F: _LL250:
({void*_tmp3D3=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3D4="get_ptr_typ: not a pointer!";_tag_dyneither(_tmp3D4,sizeof(char),28);}),_tag_dyneither(_tmp3D3,sizeof(void*),0));});}_LL24C:;}
# 2184
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp3D5=Cyc_Tcutil_compress(t);void*_tmp3D6=_tmp3D5;int _tmp3D7;enum Cyc_Absyn_Sign _tmp3D8;enum Cyc_Absyn_Sign _tmp3D9;enum Cyc_Absyn_Sign _tmp3DA;enum Cyc_Absyn_Sign _tmp3DB;enum Cyc_Absyn_Sign _tmp3DC;switch(*((int*)_tmp3D6)){case 5: _LL252: _LL253:
 res=Cyc_Absyn_null_exp(0);goto _LL251;case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3D6)->f2){case Cyc_Absyn_Char_sz: _LL254: _tmp3DC=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3D6)->f1;_LL255:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp3DC,'\000'),0);goto _LL251;case Cyc_Absyn_Short_sz: _LL256: _tmp3DB=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3D6)->f1;_LL257:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp3DB,0),0);goto _LL251;case Cyc_Absyn_Int_sz: _LL25C: _tmp3DA=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3D6)->f1;_LL25D:
# 2192
 _tmp3D9=_tmp3DA;goto _LL25F;case Cyc_Absyn_Long_sz: _LL25E: _tmp3D9=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3D6)->f1;_LL25F:
# 2194
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp3D9,0),0);goto _LL251;default: _LL260: _tmp3D8=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3D6)->f1;_LL261:
# 2196
 res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp3D8,(long long)0),0);goto _LL251;}case 13: _LL258: _LL259:
# 2190
 goto _LL25B;case 14: _LL25A: _LL25B:
 _tmp3DA=Cyc_Absyn_Unsigned;goto _LL25D;case 7: switch(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3D6)->f1){case 0: _LL262: _LL263:
# 2197
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp3DD="0.0F";_tag_dyneither(_tmp3DD,sizeof(char),5);}),0),0);goto _LL251;case 1: _LL264: _LL265:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp3DE="0.0";_tag_dyneither(_tmp3DE,sizeof(char),4);}),1),0);goto _LL251;default: _LL266: _tmp3D7=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3D6)->f1;_LL267:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp3DF="0.0L";_tag_dyneither(_tmp3DF,sizeof(char),5);}),_tmp3D7),0);goto _LL251;}default: _LL268: _LL269:
# 2201
({struct Cyc_String_pa_PrintArg_struct _tmp3E2;_tmp3E2.tag=0;_tmp3E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp3E0[1]={& _tmp3E2};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3E1="found non-zero type %s in generate_zero";_tag_dyneither(_tmp3E1,sizeof(char),40);}),_tag_dyneither(_tmp3E0,sizeof(void*),1));});});}_LL251:;}
# 2203
res->topt=t;
return res;}
# 2210
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 2223 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp3E3=Cyc_Toc_typ_to_c(elt_type);
void*_tmp3E4=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp3E5=Cyc_Absyn_cstar_typ(_tmp3E3,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmp3E6=({struct Cyc_Core_Opt*_tmp41E=_cycalloc(sizeof(*_tmp41E));_tmp41E->v=_tmp3E5;_tmp41E;});
struct Cyc_Absyn_Exp*xinit;
{void*_tmp3E7=e1->r;void*_tmp3E8=_tmp3E7;struct Cyc_Absyn_Exp*_tmp3E9;struct Cyc_Absyn_Exp*_tmp3EA;struct Cyc_Absyn_Exp*_tmp3EB;switch(*((int*)_tmp3E8)){case 19: _LL26B: _tmp3EB=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3E8)->f1;_LL26C:
# 2231
 if(!is_dyneither){
_tmp3EB=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp3EB,0,Cyc_Absyn_Other_coercion,0);
_tmp3EB->topt=fat_ptr_type;}
# 2235
Cyc_Toc_exp_to_c(nv,_tmp3EB);xinit=_tmp3EB;goto _LL26A;case 22: _LL26D: _tmp3E9=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3E8)->f1;_tmp3EA=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3E8)->f2;_LL26E:
# 2237
 if(!is_dyneither){
_tmp3E9=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp3E9,0,Cyc_Absyn_Other_coercion,0);
_tmp3E9->topt=fat_ptr_type;}
# 2241
Cyc_Toc_exp_to_c(nv,_tmp3E9);Cyc_Toc_exp_to_c(nv,_tmp3EA);
xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp3EC[3];_tmp3EC[2]=_tmp3EA;_tmp3EC[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0);_tmp3EC[0]=_tmp3E9;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3EC,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
goto _LL26A;default: _LL26F: _LL270:
({void*_tmp3ED=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3EE="found bad lhs for zero-terminated pointer assignment";_tag_dyneither(_tmp3EE,sizeof(char),53);}),_tag_dyneither(_tmp3ED,sizeof(void*),0));});}_LL26A:;}{
# 2247
struct _tuple1*_tmp3EF=Cyc_Toc_temp_var();
struct _RegionHandle _tmp3F0=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp3F0;_push_region(rgn2);
{struct Cyc_Toc_Env*_tmp3F1=Cyc_Toc_add_varmap(rgn2,nv,_tmp3EF,Cyc_Absyn_var_exp(_tmp3EF,0));
struct Cyc_Absyn_Vardecl*_tmp3F2=({struct Cyc_Absyn_Vardecl*_tmp41D=_cycalloc(sizeof(*_tmp41D));_tmp41D->sc=Cyc_Absyn_Public;_tmp41D->name=_tmp3EF;_tmp41D->tq=Cyc_Toc_mt_tq;_tmp41D->type=_tmp3E4;_tmp41D->initializer=xinit;_tmp41D->rgn=0;_tmp41D->attributes=0;_tmp41D->escapes=0;_tmp41D;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp3F3=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp41B=_cycalloc(sizeof(*_tmp41B));_tmp41B[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp41C;_tmp41C.tag=4;_tmp41C.f1=_tmp3F2;_tmp41C;});_tmp41B;});
struct Cyc_Absyn_Exp*_tmp3F4=Cyc_Absyn_varb_exp(_tmp3EF,(void*)_tmp3F3,0);
_tmp3F4->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp3F5=Cyc_Absyn_deref_exp(_tmp3F4,0);
_tmp3F5->topt=elt_type;
Cyc_Toc_exp_to_c(_tmp3F1,_tmp3F5);{
struct _tuple1*_tmp3F6=Cyc_Toc_temp_var();
_tmp3F1=Cyc_Toc_add_varmap(rgn2,_tmp3F1,_tmp3F6,Cyc_Absyn_var_exp(_tmp3F6,0));{
struct Cyc_Absyn_Vardecl*_tmp3F7=({struct Cyc_Absyn_Vardecl*_tmp41A=_cycalloc(sizeof(*_tmp41A));_tmp41A->sc=Cyc_Absyn_Public;_tmp41A->name=_tmp3F6;_tmp41A->tq=Cyc_Toc_mt_tq;_tmp41A->type=_tmp3E3;_tmp41A->initializer=_tmp3F5;_tmp41A->rgn=0;_tmp41A->attributes=0;_tmp41A->escapes=0;_tmp41A;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp3F8=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp418=_cycalloc(sizeof(*_tmp418));_tmp418[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp419;_tmp419.tag=4;_tmp419.f1=_tmp3F7;_tmp419;});_tmp418;});
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp3F9=Cyc_Absyn_varb_exp(_tmp3F6,(void*)_tmp3F8,0);
_tmp3F9->topt=_tmp3F5->topt;
z_init=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp3F9,e2,0);
z_init->topt=_tmp3F9->topt;}
# 2268
Cyc_Toc_exp_to_c(_tmp3F1,z_init);{
struct _tuple1*_tmp3FA=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp3FB=({struct Cyc_Absyn_Vardecl*_tmp417=_cycalloc(sizeof(*_tmp417));_tmp417->sc=Cyc_Absyn_Public;_tmp417->name=_tmp3FA;_tmp417->tq=Cyc_Toc_mt_tq;_tmp417->type=_tmp3E3;_tmp417->initializer=z_init;_tmp417->rgn=0;_tmp417->attributes=0;_tmp417->escapes=0;_tmp417;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp3FC=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp415=_cycalloc(sizeof(*_tmp415));_tmp415[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp416;_tmp416.tag=4;_tmp416.f1=_tmp3FB;_tmp416;});_tmp415;});
_tmp3F1=Cyc_Toc_add_varmap(rgn2,_tmp3F1,_tmp3FA,Cyc_Absyn_var_exp(_tmp3FA,0));{
# 2275
struct Cyc_Absyn_Exp*_tmp3FD=Cyc_Absyn_varb_exp(_tmp3F6,(void*)_tmp3F8,0);_tmp3FD->topt=_tmp3F5->topt;{
struct Cyc_Absyn_Exp*_tmp3FE=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp3FF=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp3FD,_tmp3FE,0);
_tmp3FF->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp3F1,_tmp3FF);{
# 2281
struct Cyc_Absyn_Exp*_tmp400=Cyc_Absyn_varb_exp(_tmp3FA,(void*)_tmp3FC,0);_tmp400->topt=_tmp3F5->topt;{
struct Cyc_Absyn_Exp*_tmp401=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp402=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp400,_tmp401,0);
_tmp402->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp3F1,_tmp402);{
# 2287
struct Cyc_List_List*_tmp403=({struct Cyc_Absyn_Exp*_tmp414[2];_tmp414[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0);_tmp414[0]=
# 2287
Cyc_Absyn_varb_exp(_tmp3EF,(void*)_tmp3F3,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp414,sizeof(struct Cyc_Absyn_Exp*),2));});
# 2289
struct Cyc_Absyn_Exp*_tmp404=Cyc_Absyn_uint_exp(1,0);
struct Cyc_Absyn_Exp*xsize;
xsize=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp403,0),_tmp404,0);{
# 2294
struct Cyc_Absyn_Exp*_tmp405=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp3FF,_tmp402,0),0);
struct Cyc_Absyn_Stmt*_tmp406=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0),0);
struct Cyc_Absyn_Exp*_tmp407=Cyc_Toc_member_exp(Cyc_Absyn_varb_exp(_tmp3EF,(void*)_tmp3F3,0),Cyc_Toc_curr_sp,0);
_tmp407=Cyc_Toc_cast_it(_tmp3E5,_tmp407);{
struct Cyc_Absyn_Exp*_tmp408=Cyc_Absyn_deref_exp(_tmp407,0);
struct Cyc_Absyn_Exp*_tmp409=Cyc_Absyn_assign_exp(_tmp408,Cyc_Absyn_var_exp(_tmp3FA,0),0);
struct Cyc_Absyn_Stmt*_tmp40A=Cyc_Absyn_exp_stmt(_tmp409,0);
_tmp40A=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp405,_tmp406,Cyc_Absyn_skip_stmt(0),0),_tmp40A,0);
_tmp40A=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp40B=_cycalloc(sizeof(*_tmp40B));_tmp40B->r=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp40C=_cycalloc(sizeof(*_tmp40C));_tmp40C[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp40D;_tmp40D.tag=0;_tmp40D.f1=_tmp3FB;_tmp40D;});_tmp40C;});_tmp40B->loc=0;_tmp40B;}),_tmp40A,0);
_tmp40A=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp40E=_cycalloc(sizeof(*_tmp40E));_tmp40E->r=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp40F=_cycalloc(sizeof(*_tmp40F));_tmp40F[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp410;_tmp410.tag=0;_tmp410.f1=_tmp3F7;_tmp410;});_tmp40F;});_tmp40E->loc=0;_tmp40E;}),_tmp40A,0);
_tmp40A=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp411=_cycalloc(sizeof(*_tmp411));_tmp411->r=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp412=_cycalloc(sizeof(*_tmp412));_tmp412[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp413;_tmp413.tag=0;_tmp413.f1=_tmp3F2;_tmp413;});_tmp412;});_tmp411->loc=0;_tmp411;}),_tmp40A,0);
e->r=Cyc_Toc_stmt_exp_r(_tmp40A);};};};};};};};};};};};}
# 2249
;_pop_region(rgn2);};}
# 2320 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 2324
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp41F=Cyc_Tcutil_compress(aggrtype);void*_tmp420=_tmp41F;union Cyc_Absyn_AggrInfoU _tmp421;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp420)->tag == 11){_LL272: _tmp421=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp420)->f1).aggr_info;_LL273:
# 2327
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp421);goto _LL271;}else{_LL274: _LL275:
({void*_tmp422=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp425;_tmp425.tag=0;_tmp425.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype));({void*_tmp423[1]={& _tmp425};Cyc_aprintf(({const char*_tmp424="expecting union but found %s in check_tagged_union";_tag_dyneither(_tmp424,sizeof(char),51);}),_tag_dyneither(_tmp423,sizeof(void*),1));});}),_tag_dyneither(_tmp422,sizeof(void*),0));});}_LL271:;}{
# 2331
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
# 2345
struct Cyc_Absyn_Exp*_tmp431=Cyc_Toc_member_exp(aggrproj(_tmp427,f,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp432=Cyc_Absyn_neq_exp(_tmp431,_tmp428,0);
struct Cyc_Absyn_Exp*_tmp433=Cyc_Toc_member_exp(aggrproj(_tmp427,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp434=Cyc_Absyn_exp_stmt(_tmp433,0);
struct Cyc_Absyn_Stmt*_tmp435=Cyc_Absyn_ifthenelse_stmt(_tmp432,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*_tmp436=Cyc_Absyn_declare_stmt(_tmp426,e1_c_type,e1,Cyc_Absyn_seq_stmt(_tmp435,_tmp434,0),0);
return Cyc_Toc_stmt_exp_r(_tmp436);}};}
# 2355
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _dyneither_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 2358
void*_tmp437=Cyc_Tcutil_compress(t);void*_tmp438=_tmp437;union Cyc_Absyn_AggrInfoU _tmp439;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp438)->tag == 11){_LL277: _tmp439=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp438)->f1).aggr_info;_LL278: {
# 2360
struct Cyc_Absyn_Aggrdecl*_tmp43A=Cyc_Absyn_get_known_aggrdecl(_tmp439);
*f_tag=Cyc_Toc_get_member_offset(_tmp43A,f);{
# 2363
struct _dyneither_ptr str=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp43F;_tmp43F.tag=0;_tmp43F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp43E;_tmp43E.tag=0;_tmp43E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp43A->name).f2);({void*_tmp43C[2]={& _tmp43E,& _tmp43F};Cyc_aprintf(({const char*_tmp43D="_union_%s_%s";_tag_dyneither(_tmp43D,sizeof(char),13);}),_tag_dyneither(_tmp43C,sizeof(void*),2));});});});
*tagged_member_type=Cyc_Absyn_strct(({struct _dyneither_ptr*_tmp43B=_cycalloc(sizeof(*_tmp43B));_tmp43B[0]=str;_tmp43B;}));
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp43A->impl))->tagged;};}}else{_LL279: _LL27A:
 return 0;}_LL276:;}
# 2374
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 2377
void*_tmp440=e->r;void*_tmp441=_tmp440;struct Cyc_Absyn_Exp*_tmp442;struct _dyneither_ptr*_tmp443;int*_tmp444;struct Cyc_Absyn_Exp*_tmp445;struct _dyneither_ptr*_tmp446;int*_tmp447;struct Cyc_Absyn_Exp*_tmp448;switch(*((int*)_tmp441)){case 13: _LL27C: _tmp448=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp441)->f2;_LL27D:
({void*_tmp449=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp44A="cast on lhs!";_tag_dyneither(_tmp44A,sizeof(char),13);}),_tag_dyneither(_tmp449,sizeof(void*),0));});case 20: _LL27E: _tmp445=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp441)->f1;_tmp446=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp441)->f2;_tmp447=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp441)->f4;_LL27F:
# 2380
 return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(_tmp445->topt),_tmp446,f_tag,tagged_member_type,clear_read,_tmp447);case 21: _LL280: _tmp442=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp441)->f1;_tmp443=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp441)->f2;_tmp444=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp441)->f4;_LL281: {
# 2383
void*_tmp44B=Cyc_Tcutil_compress((void*)_check_null(_tmp442->topt));void*_tmp44C=_tmp44B;void*_tmp44D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44C)->tag == 5){_LL285: _tmp44D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44C)->f1).elt_typ;_LL286:
# 2385
 return Cyc_Toc_is_tagged_union_project_impl(_tmp44D,_tmp443,f_tag,tagged_member_type,clear_read,_tmp444);}else{_LL287: _LL288:
# 2387
 return 0;}_LL284:;}default: _LL282: _LL283:
# 2389
 return 0;}_LL27B:;}
# 2401 "toc.cyc"
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
# 2425 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 2429
struct _tuple1*_tmp450=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp450,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{
# 2439
struct Cyc_Absyn_Exp*_tmp451=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
s2=Cyc_Absyn_ifthenelse_stmt(_tmp451,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{
# 2442
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp450,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(e1),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple28{struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};
# 2448
static void Cyc_Toc_new_comprehension_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,void*new_e_type,struct Cyc_Absyn_Exp*rgnopt,void*old_typ,struct Cyc_Absyn_Exp*e1,void*t1,int zero_term,struct Cyc_Absyn_Exp*init_exp,struct Cyc_Absyn_Vardecl*vd){
# 2463 "toc.cyc"
struct _tuple1*max=Cyc_Toc_temp_var();
struct _tuple1*a=Cyc_Toc_temp_var();
void*old_elt_typ=t1;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c(nv,e1);{
struct Cyc_Absyn_Exp*_tmp452=Cyc_Absyn_var_exp(max,0);
# 2471
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
# 2482
s=Cyc_Absyn_skip_stmt(0);}}{
# 2484
struct _RegionHandle _tmp453=_new_region("r");struct _RegionHandle*r=& _tmp453;_push_region(r);
{struct Cyc_List_List*decls=({struct Cyc_List_List*_tmp467=_region_malloc(r,sizeof(*_tmp467));_tmp467->hd=({struct _tuple28*_tmp468=_region_malloc(r,sizeof(*_tmp468));_tmp468->f1=max;_tmp468->f2=Cyc_Absyn_uint_typ;_tmp468->f3=e1;_tmp468;});_tmp467->tl=0;_tmp467;});
# 2487
struct Cyc_Absyn_Exp*ai;
if(rgnopt == 0  || Cyc_Absyn_no_regions)
ai=Cyc_Toc_malloc_exp(old_elt_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp454[2];_tmp454[1]=_tmp452;_tmp454[0]=
Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp454,sizeof(struct Cyc_Absyn_Exp*),2));}),0));else{
# 2494
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);
ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp455[2];_tmp455[1]=_tmp452;_tmp455[0]=
Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp455,sizeof(struct Cyc_Absyn_Exp*),2));}),0));}{
# 2500
struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);
decls=({struct Cyc_List_List*_tmp456=_region_malloc(r,sizeof(*_tmp456));_tmp456->hd=({struct _tuple28*_tmp457=_region_malloc(r,sizeof(*_tmp457));_tmp457->f1=a;_tmp457->f2=ptr_typ;_tmp457->f3=ainit;_tmp457;});_tmp456->tl=decls;_tmp456;});
if(!Cyc_Tcutil_is_pointer_type(old_typ))
({void*_tmp458=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp459="new_comprehension_to_c: comprehension is not an array type";_tag_dyneither(_tmp459,sizeof(char),59);}),_tag_dyneither(_tmp458,sizeof(void*),0));});
if(Cyc_Tcutil_is_dyneither_ptr(old_typ)){
struct _tuple1*_tmp45A=Cyc_Toc_temp_var();
void*_tmp45B=Cyc_Toc_typ_to_c(old_typ);
struct Cyc_Absyn_Exp*_tmp45C=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmp45D=
Cyc_Absyn_fncall_exp(_tmp45C,({struct Cyc_Absyn_Exp*_tmp460[3];_tmp460[2]=_tmp452;_tmp460[1]=
Cyc_Absyn_sizeoftyp_exp(elt_typ,0);_tmp460[0]=Cyc_Absyn_var_exp(a,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp460,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
# 2512
decls=({struct Cyc_List_List*_tmp45E=_region_malloc(r,sizeof(*_tmp45E));_tmp45E->hd=({struct _tuple28*_tmp45F=_region_malloc(r,sizeof(*_tmp45F));_tmp45F->f1=_tmp45A;_tmp45F->f2=_tmp45B;_tmp45F->f3=_tmp45D;_tmp45F;});_tmp45E->tl=decls;_tmp45E;});
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp45A,0),0),0);}else{
# 2515
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}
{struct Cyc_List_List*_tmp461=decls;for(0;_tmp461 != 0;_tmp461=_tmp461->tl){
struct _tuple28 _tmp462=*((struct _tuple28*)_tmp461->hd);struct _tuple28 _tmp463=_tmp462;struct _tuple1*_tmp464;void*_tmp465;struct Cyc_Absyn_Exp*_tmp466;_LL28A: _tmp464=_tmp463.f1;_tmp465=_tmp463.f2;_tmp466=_tmp463.f3;_LL28B:;
s=Cyc_Absyn_declare_stmt(_tmp464,_tmp465,_tmp466,s,0);}}
# 2520
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2485
;_pop_region(r);};};}struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 2524
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
void*_tmp469=e->r;
if(e->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp46C;_tmp46C.tag=0;_tmp46C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp46A[1]={& _tmp46C};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp46B="exp_to_c: no type for %s";_tag_dyneither(_tmp46B,sizeof(char),25);}),_tag_dyneither(_tmp46A,sizeof(void*),1));});});
# 2530
if((nv->struct_info).lhs_exp != 0){
void*_tmp46D=_tmp469;if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp46D)->tag == 28){_LL28D: _LL28E:
 goto _LL28C;}else{_LL28F: _LL290:
 nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);}_LL28C:;}{
# 2538
void*old_typ=(void*)_check_null(e->topt);
void*_tmp46E=_tmp469;struct Cyc_Absyn_Stmt*_tmp46F;struct Cyc_Absyn_Exp*_tmp470;struct _dyneither_ptr*_tmp471;struct Cyc_Absyn_Exp*_tmp472;struct Cyc_Absyn_Exp*_tmp473;int _tmp474;struct Cyc_Absyn_Exp*_tmp475;void**_tmp476;struct Cyc_Absyn_Exp*_tmp477;int _tmp478;int _tmp479;struct Cyc_List_List*_tmp47A;struct Cyc_Absyn_Datatypedecl*_tmp47B;struct Cyc_Absyn_Datatypefield*_tmp47C;void*_tmp47D;struct Cyc_List_List*_tmp47E;struct _tuple1*_tmp47F;struct Cyc_List_List*_tmp480;struct Cyc_List_List*_tmp481;struct Cyc_Absyn_Aggrdecl*_tmp482;struct Cyc_Absyn_Exp*_tmp483;void*_tmp484;int _tmp485;struct Cyc_Absyn_Vardecl*_tmp486;struct Cyc_Absyn_Exp*_tmp487;struct Cyc_Absyn_Exp*_tmp488;int _tmp489;struct Cyc_List_List*_tmp48A;struct Cyc_List_List*_tmp48B;struct Cyc_Absyn_Exp*_tmp48C;struct Cyc_Absyn_Exp*_tmp48D;struct Cyc_Absyn_Exp*_tmp48E;struct _dyneither_ptr*_tmp48F;int _tmp490;int _tmp491;struct Cyc_Absyn_Exp*_tmp492;struct _dyneither_ptr*_tmp493;int _tmp494;int _tmp495;struct Cyc_Absyn_Exp*_tmp496;void*_tmp497;struct Cyc_List_List*_tmp498;void*_tmp499;struct Cyc_Absyn_Exp*_tmp49A;struct Cyc_Absyn_Exp*_tmp49B;struct Cyc_Absyn_Exp*_tmp49C;struct Cyc_Absyn_Exp*_tmp49D;void**_tmp49E;struct Cyc_Absyn_Exp*_tmp49F;int _tmp4A0;enum Cyc_Absyn_Coercion _tmp4A1;struct Cyc_Absyn_Exp*_tmp4A2;struct Cyc_List_List*_tmp4A3;struct Cyc_Absyn_Exp*_tmp4A4;struct Cyc_Absyn_Exp*_tmp4A5;int _tmp4A6;struct Cyc_Absyn_Exp*_tmp4A7;struct Cyc_List_List*_tmp4A8;int _tmp4A9;struct Cyc_List_List*_tmp4AA;struct Cyc_Absyn_VarargInfo*_tmp4AB;struct Cyc_Absyn_Exp*_tmp4AC;struct Cyc_List_List*_tmp4AD;struct Cyc_Absyn_Exp*_tmp4AE;struct Cyc_Absyn_Exp*_tmp4AF;struct Cyc_Absyn_Exp*_tmp4B0;struct Cyc_Absyn_Exp*_tmp4B1;struct Cyc_Absyn_Exp*_tmp4B2;struct Cyc_Absyn_Exp*_tmp4B3;struct Cyc_Absyn_Exp*_tmp4B4;struct Cyc_Absyn_Exp*_tmp4B5;struct Cyc_Absyn_Exp*_tmp4B6;struct Cyc_Absyn_Exp*_tmp4B7;struct Cyc_Core_Opt*_tmp4B8;struct Cyc_Absyn_Exp*_tmp4B9;struct Cyc_Absyn_Exp*_tmp4BA;enum Cyc_Absyn_Incrementor _tmp4BB;enum Cyc_Absyn_Primop _tmp4BC;struct Cyc_List_List*_tmp4BD;struct _tuple1*_tmp4BE;void*_tmp4BF;switch(*((int*)_tmp46E)){case 0: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp46E)->f1).Null_c).tag == 1){_LL292: _LL293: {
# 2545
struct Cyc_Absyn_Exp*_tmp4C0=Cyc_Absyn_uint_exp(0,0);
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp4C0,_tmp4C0))->r;else{
# 2550
e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*_tmp4C1[3];_tmp4C1[2]=_tmp4C0;_tmp4C1[1]=_tmp4C0;_tmp4C1[0]=_tmp4C0;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4C1,sizeof(struct Cyc_Absyn_Exp*),3));}));}}else{
# 2552
e->r=(void*)& Cyc_Toc_zero_exp;}
# 2554
goto _LL291;}}else{_LL294: _LL295:
 goto _LL291;}case 1: _LL296: _tmp4BE=((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp4BF=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL297:
# 2557
{struct _handler_cons _tmp4C2;_push_handler(& _tmp4C2);{int _tmp4C4=0;if(setjmp(_tmp4C2.handler))_tmp4C4=1;if(!_tmp4C4){e->r=(Cyc_Toc_lookup_varmap(nv,_tmp4BE))->r;;_pop_handler();}else{void*_tmp4C3=(void*)_exn_thrown;void*_tmp4C5=_tmp4C3;void*_tmp4C6;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp4C5)->tag == Cyc_Dict_Absent){_LL2E7: _LL2E8:
# 2559
({struct Cyc_String_pa_PrintArg_struct _tmp4C9;_tmp4C9.tag=0;_tmp4C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4BE));({void*_tmp4C7[1]={& _tmp4C9};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4C8="Can't find %s in exp_to_c, Var\n";_tag_dyneither(_tmp4C8,sizeof(char),32);}),_tag_dyneither(_tmp4C7,sizeof(void*),1));});});}else{_LL2E9: _tmp4C6=_tmp4C5;_LL2EA:(int)_rethrow(_tmp4C6);}_LL2E6:;}};}
# 2561
goto _LL291;case 2: _LL298: _tmp4BC=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp4BD=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL299: {
# 2564
struct Cyc_List_List*_tmp4CA=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp4BD);
# 2566
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4BD);
{enum Cyc_Absyn_Primop _tmp4CB=_tmp4BC;switch(_tmp4CB){case Cyc_Absyn_Numelts: _LL2EC: _LL2ED: {
# 2569
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4BD))->hd;
{void*_tmp4CC=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_tmp4CD=_tmp4CC;void*_tmp4CE;union Cyc_Absyn_Constraint*_tmp4CF;union Cyc_Absyn_Constraint*_tmp4D0;union Cyc_Absyn_Constraint*_tmp4D1;struct Cyc_Absyn_Exp*_tmp4D2;switch(*((int*)_tmp4CD)){case 8: _LL301: _tmp4D2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4CD)->f1).num_elts;_LL302:
# 2573
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp4D2)))
({void*_tmp4D3=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp4D4="can't calculate numelts";_tag_dyneither(_tmp4D4,sizeof(char),24);}),_tag_dyneither(_tmp4D3,sizeof(void*),0));});
e->r=_tmp4D2->r;goto _LL300;case 5: _LL303: _tmp4CE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CD)->f1).elt_typ;_tmp4CF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CD)->f1).ptr_atts).nullable;_tmp4D0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CD)->f1).ptr_atts).bounds;_tmp4D1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CD)->f1).ptr_atts).zero_term;_LL304:
# 2577
{void*_tmp4D5=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp4D0);void*_tmp4D6=_tmp4D5;struct Cyc_Absyn_Exp*_tmp4D7;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp4D6)->tag == 0){_LL308: _LL309:
# 2579
 e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,({struct Cyc_Absyn_Exp*_tmp4D8[2];_tmp4D8[1]=
# 2581
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp4CE),0);_tmp4D8[0]=(struct Cyc_Absyn_Exp*)_tmp4BD->hd;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4D8,sizeof(struct Cyc_Absyn_Exp*),2));}));
goto _LL307;}else{_LL30A: _tmp4D7=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4D6)->f1;_LL30B:
# 2585
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4D1)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp4BD->hd);
# 2589
e->r=Cyc_Toc_fncall_exp_r(function_e,({struct Cyc_Absyn_Exp*_tmp4D9[2];_tmp4D9[1]=_tmp4D7;_tmp4D9[0]=(struct Cyc_Absyn_Exp*)_tmp4BD->hd;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4D9,sizeof(struct Cyc_Absyn_Exp*),2));}));}else{
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4CF)){
if(!Cyc_Evexp_c_can_eval(_tmp4D7))
({void*_tmp4DA=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp4DB="can't calculate numelts";_tag_dyneither(_tmp4DB,sizeof(char),24);}),_tag_dyneither(_tmp4DA,sizeof(void*),0));});
# 2594
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp4D7,Cyc_Absyn_uint_exp(0,0));}else{
# 2596
e->r=_tmp4D7->r;goto _LL307;}}
# 2598
goto _LL307;}_LL307:;}
# 2600
goto _LL300;default: _LL305: _LL306:
# 2602
({struct Cyc_String_pa_PrintArg_struct _tmp4DF;_tmp4DF.tag=0;_tmp4DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp4DE;_tmp4DE.tag=0;_tmp4DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));({void*_tmp4DC[2]={& _tmp4DE,& _tmp4DF};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4DD="size primop applied to non-array %s (%s)";_tag_dyneither(_tmp4DD,sizeof(char),41);}),_tag_dyneither(_tmp4DC,sizeof(void*),2));});});});}_LL300:;}
# 2605
goto _LL2EB;}case Cyc_Absyn_Plus: _LL2EE: _LL2EF:
# 2610
{void*_tmp4E0=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp4CA))->hd);void*_tmp4E1=_tmp4E0;void*_tmp4E2;union Cyc_Absyn_Constraint*_tmp4E3;union Cyc_Absyn_Constraint*_tmp4E4;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E1)->tag == 5){_LL30D: _tmp4E2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E1)->f1).elt_typ;_tmp4E3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E1)->f1).ptr_atts).bounds;_tmp4E4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E1)->f1).ptr_atts).zero_term;_LL30E:
# 2612
{void*_tmp4E5=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp4E3);void*_tmp4E6=_tmp4E5;struct Cyc_Absyn_Exp*_tmp4E7;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp4E6)->tag == 0){_LL312: _LL313: {
# 2614
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4BD))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4BD->tl))->hd;
e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp4E8[3];_tmp4E8[2]=e2;_tmp4E8[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp4E2),0);_tmp4E8[0]=e1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4E8,sizeof(struct Cyc_Absyn_Exp*),3));}));
goto _LL311;}}else{_LL314: _tmp4E7=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4E6)->f1;_LL315:
# 2620
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4E4)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4BD))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4BD->tl))->hd;
e->r=(Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1),({struct Cyc_Absyn_Exp*_tmp4E9[3];_tmp4E9[2]=e2;_tmp4E9[1]=_tmp4E7;_tmp4E9[0]=e1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4E9,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r;}
# 2625
goto _LL311;}_LL311:;}
# 2627
goto _LL30C;}else{_LL30F: _LL310:
# 2629
 goto _LL30C;}_LL30C:;}
# 2631
goto _LL2EB;case Cyc_Absyn_Minus: _LL2F0: _LL2F1: {
# 2636
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(_tmp4CA))->hd,& elt_typ)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4BD))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4BD->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(_tmp4CA->tl))->hd)){
e1->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);
e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp);
e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{
# 2646
e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp4EA[3];_tmp4EA[2]=
# 2648
Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0);_tmp4EA[1]=
# 2647
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp4EA[0]=e1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4EA,sizeof(struct Cyc_Absyn_Exp*),3));}));}}
# 2651
goto _LL2EB;}case Cyc_Absyn_Eq: _LL2F2: _LL2F3:
 goto _LL2F5;case Cyc_Absyn_Neq: _LL2F4: _LL2F5:
 goto _LL2F7;case Cyc_Absyn_Gt: _LL2F6: _LL2F7:
 goto _LL2F9;case Cyc_Absyn_Gte: _LL2F8: _LL2F9:
 goto _LL2FB;case Cyc_Absyn_Lt: _LL2FA: _LL2FB:
 goto _LL2FD;case Cyc_Absyn_Lte: _LL2FC: _LL2FD: {
# 2659
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4BD))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4BD->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp4CA))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp4CA->tl))->hd;
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& elt_typ))
e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,0));
if(Cyc_Tcutil_is_tagged_pointer_typ(t2))
e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp,0));
goto _LL2EB;}default: _LL2FE: _LL2FF:
 goto _LL2EB;}_LL2EB:;}
# 2671
goto _LL291;}case 4: _LL29A: _tmp4BA=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp4BB=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL29B: {
# 2673
void*e2_cyc_typ=(void*)_check_null(_tmp4BA->topt);
# 2682 "toc.cyc"
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
struct _dyneither_ptr incr_str=({const char*_tmp501="increment";_tag_dyneither(_tmp501,sizeof(char),10);});
if(_tmp4BB == Cyc_Absyn_PreDec  || _tmp4BB == Cyc_Absyn_PostDec)incr_str=({const char*_tmp4EB="decrement";_tag_dyneither(_tmp4EB,sizeof(char),10);});
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4BA,& ptr_type,& is_dyneither,& elt_type)){
({struct Cyc_String_pa_PrintArg_struct _tmp4EE;_tmp4EE.tag=0;_tmp4EE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);({void*_tmp4EC[1]={& _tmp4EE};Cyc_Tcutil_terr(e->loc,({const char*_tmp4ED="in-place %s is not supported when dereferencing a zero-terminated pointer";_tag_dyneither(_tmp4ED,sizeof(char),74);}),_tag_dyneither(_tmp4EC,sizeof(void*),1));});});
({void*_tmp4EF=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4F0="in-place inc/dec on zero-term";_tag_dyneither(_tmp4F0,sizeof(char),30);}),_tag_dyneither(_tmp4EF,sizeof(void*),0));});}{
# 2691
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
int f_tag=0;
if(Cyc_Toc_is_tagged_union_project(_tmp4BA,& f_tag,& tunion_member_type,1)){
struct Cyc_Absyn_Exp*_tmp4F1=Cyc_Absyn_signed_int_exp(1,0);
_tmp4F1->topt=Cyc_Absyn_sint_typ;{
enum Cyc_Absyn_Incrementor _tmp4F2=_tmp4BB;switch(_tmp4F2){case Cyc_Absyn_PreInc: _LL317: _LL318:
# 2698
 e->r=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp4F3=_cycalloc(sizeof(*_tmp4F3));_tmp4F3[0]=({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmp4F4;_tmp4F4.tag=3;_tmp4F4.f1=_tmp4BA;_tmp4F4.f2=({struct Cyc_Core_Opt*_tmp4F5=_cycalloc_atomic(sizeof(*_tmp4F5));_tmp4F5->v=(void*)Cyc_Absyn_Plus;_tmp4F5;});_tmp4F4.f3=_tmp4F1;_tmp4F4;});_tmp4F3;});
Cyc_Toc_exp_to_c(nv,e);
return;case Cyc_Absyn_PreDec: _LL319: _LL31A:
# 2702
 e->r=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp4F6=_cycalloc(sizeof(*_tmp4F6));_tmp4F6[0]=({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmp4F7;_tmp4F7.tag=3;_tmp4F7.f1=_tmp4BA;_tmp4F7.f2=({struct Cyc_Core_Opt*_tmp4F8=_cycalloc_atomic(sizeof(*_tmp4F8));_tmp4F8->v=(void*)Cyc_Absyn_Minus;_tmp4F8;});_tmp4F7.f3=_tmp4F1;_tmp4F7;});_tmp4F6;});
Cyc_Toc_exp_to_c(nv,e);
return;default: _LL31B: _LL31C:
# 2706
({struct Cyc_String_pa_PrintArg_struct _tmp4FB;_tmp4FB.tag=0;_tmp4FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);({void*_tmp4F9[1]={& _tmp4FB};Cyc_Tcutil_terr(e->loc,({const char*_tmp4FA="in-place post-%s is not supported on @tagged union members";_tag_dyneither(_tmp4FA,sizeof(char),59);}),_tag_dyneither(_tmp4F9,sizeof(void*),1));});});
# 2708
({void*_tmp4FC=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4FD="in-place inc/dec on @tagged union";_tag_dyneither(_tmp4FD,sizeof(char),34);}),_tag_dyneither(_tmp4FC,sizeof(void*),0));});}_LL316:;};}
# 2711
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4BA);
Cyc_Toc_set_lhs(nv,0);{
# 2716
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp4BB == Cyc_Absyn_PostInc  || _tmp4BB == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2722
if(_tmp4BB == Cyc_Absyn_PreDec  || _tmp4BB == Cyc_Absyn_PostDec)
change=-1;
e->r=Cyc_Toc_fncall_exp_r(fn_e,({struct Cyc_Absyn_Exp*_tmp4FE[3];_tmp4FE[2]=
# 2726
Cyc_Absyn_signed_int_exp(change,0);_tmp4FE[1]=
# 2725
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp4FE[0]=
# 2724
Cyc_Toc_push_address_exp(_tmp4BA);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4FE,sizeof(struct Cyc_Absyn_Exp*),3));}));}else{
# 2727
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2731
struct Cyc_Toc_functionSet*_tmp4FF=_tmp4BB != Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
# 2733
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp4FF,_tmp4BA);
e->r=Cyc_Toc_fncall_exp_r(fn_e,({struct Cyc_Absyn_Exp*_tmp500[2];_tmp500[1]=
Cyc_Absyn_signed_int_exp(1,0);_tmp500[0]=
# 2734
Cyc_Toc_push_address_exp(_tmp4BA);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp500,sizeof(struct Cyc_Absyn_Exp*),2));}));}else{
# 2736
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp4BA)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp4BA,0,Cyc_Toc_incr_lvalue,_tmp4BB);
e->r=_tmp4BA->r;}}}
# 2740
goto _LL291;};};}case 3: _LL29C: _tmp4B7=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp4B8=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_tmp4B9=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp46E)->f3;_LL29D: {
# 2759 "toc.cyc"
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
# 2770
return;}{
# 2772
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4B7,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp4B7,_tmp4B8,_tmp4B9,ptr_type,is_dyneither,elt_type);
# 2778
return;}{
# 2782
int e1_poly=Cyc_Toc_is_poly_project(_tmp4B7);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4B7);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4B9);{
# 2788
int done=0;
if(_tmp4B8 != 0){
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp502=(enum Cyc_Absyn_Primop)_tmp4B8->v;enum Cyc_Absyn_Primop _tmp503=_tmp502;switch(_tmp503){case Cyc_Absyn_Plus: _LL31E: _LL31F:
 change=_tmp4B9;goto _LL31D;case Cyc_Absyn_Minus: _LL320: _LL321:
# 2796
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp4B9,0);goto _LL31D;default: _LL322: _LL323:
({void*_tmp504=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp505="bad t ? pointer arithmetic";_tag_dyneither(_tmp505,sizeof(char),27);}),_tag_dyneither(_tmp504,sizeof(void*),0));});}_LL31D:;}
# 2799
done=1;{
# 2801
struct Cyc_Absyn_Exp*_tmp506=Cyc_Toc__dyneither_ptr_inplace_plus_e;
e->r=Cyc_Toc_fncall_exp_r(_tmp506,({struct Cyc_Absyn_Exp*_tmp507[3];_tmp507[2]=change;_tmp507[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp507[0]=
# 2802
Cyc_Toc_push_address_exp(_tmp4B7);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp507,sizeof(struct Cyc_Absyn_Exp*),3));}));};}else{
# 2805
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2808
enum Cyc_Absyn_Primop _tmp508=(enum Cyc_Absyn_Primop)_tmp4B8->v;enum Cyc_Absyn_Primop _tmp509=_tmp508;if(_tmp509 == Cyc_Absyn_Plus){_LL325: _LL326:
# 2810
 done=1;
e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp4B7),({struct Cyc_Absyn_Exp*_tmp50A[2];_tmp50A[1]=_tmp4B9;_tmp50A[0]=_tmp4B7;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp50A,sizeof(struct Cyc_Absyn_Exp*),2));}));
goto _LL324;}else{_LL327: _LL328:
({void*_tmp50B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp50C="bad zero-terminated pointer arithmetic";_tag_dyneither(_tmp50C,sizeof(char),39);}),_tag_dyneither(_tmp50B,sizeof(void*),0));});}_LL324:;}}}
# 2817
if(!done){
# 2819
if(e1_poly)
_tmp4B9->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp4B9->r,0));
# 2825
if(!Cyc_Absyn_is_lvalue(_tmp4B7)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple27*),struct _tuple27*f_env))Cyc_Toc_lvalue_assign)(_tmp4B7,0,Cyc_Toc_assignop_lvalue,({struct _tuple27*_tmp50D=_cycalloc(sizeof(struct _tuple27)* 1);_tmp50D[0]=(struct _tuple27)({struct _tuple27 _tmp50E;_tmp50E.f1=_tmp4B8;_tmp50E.f2=_tmp4B9;_tmp50E;});_tmp50D;}));
e->r=_tmp4B7->r;}}
# 2830
goto _LL291;};};};}case 5: _LL29E: _tmp4B4=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp4B5=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_tmp4B6=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp46E)->f3;_LL29F:
# 2832
 Cyc_Toc_exp_to_c(nv,_tmp4B4);
Cyc_Toc_exp_to_c(nv,_tmp4B5);
Cyc_Toc_exp_to_c(nv,_tmp4B6);
goto _LL291;case 6: _LL2A0: _tmp4B2=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp4B3=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL2A1:
# 2837
 Cyc_Toc_exp_to_c(nv,_tmp4B2);
Cyc_Toc_exp_to_c(nv,_tmp4B3);
goto _LL291;case 7: _LL2A2: _tmp4B0=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp4B1=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL2A3:
# 2841
 Cyc_Toc_exp_to_c(nv,_tmp4B0);
Cyc_Toc_exp_to_c(nv,_tmp4B1);
goto _LL291;case 8: _LL2A4: _tmp4AE=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp4AF=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL2A5:
# 2845
 Cyc_Toc_exp_to_c(nv,_tmp4AE);
Cyc_Toc_exp_to_c(nv,_tmp4AF);
goto _LL291;case 9: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46E)->f3 == 0){_LL2A6: _tmp4AC=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp4AD=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL2A7:
# 2849
 Cyc_Toc_exp_to_c(nv,_tmp4AC);
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4AD);
goto _LL291;}else{_LL2A8: _tmp4A7=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp4A8=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_tmp4A9=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46E)->f3)->num_varargs;_tmp4AA=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46E)->f3)->injectors;_tmp4AB=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46E)->f3)->vai;_LL2A9: {
# 2861 "toc.cyc"
struct _RegionHandle _tmp50F=_new_region("r");struct _RegionHandle*r=& _tmp50F;_push_region(r);{
struct _tuple1*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp4A9,0);
void*cva_type=Cyc_Toc_typ_to_c(_tmp4AB->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_conref,0);
# 2869
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4A8);
int num_normargs=num_args - _tmp4A9;
# 2873
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp4A8=_tmp4A8->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4A8))->hd);
new_args=({struct Cyc_List_List*_tmp510=_cycalloc(sizeof(*_tmp510));_tmp510->hd=(struct Cyc_Absyn_Exp*)_tmp4A8->hd;_tmp510->tl=new_args;_tmp510;});}}
# 2878
new_args=({struct Cyc_List_List*_tmp511=_cycalloc(sizeof(*_tmp511));_tmp511->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*_tmp512[3];_tmp512[2]=num_varargs_exp;_tmp512[1]=
# 2880
Cyc_Absyn_sizeoftyp_exp(cva_type,0);_tmp512[0]=argvexp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp512,sizeof(struct Cyc_Absyn_Exp*),3));}),0);_tmp511->tl=new_args;_tmp511;});
# 2883
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);
# 2885
Cyc_Toc_exp_to_c(nv,_tmp4A7);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4A7,new_args,0),0);
# 2889
if(_tmp4AB->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp513=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp4AB->type));void*_tmp514=_tmp513;struct Cyc_Absyn_Datatypedecl*_tmp515;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp514)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp514)->f1).datatype_info).KnownDatatype).tag == 2){_LL32A: _tmp515=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp514)->f1).datatype_info).KnownDatatype).val;_LL32B:
 tud=_tmp515;goto _LL329;}else{goto _LL32C;}}else{_LL32C: _LL32D:
({void*_tmp516=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp517="toc: unknown datatype in vararg with inject";_tag_dyneither(_tmp517,sizeof(char),44);}),_tag_dyneither(_tmp516,sizeof(void*),0));});}_LL329:;}{
# 2895
struct _dyneither_ptr vs=({unsigned int _tmp520=(unsigned int)_tmp4A9;struct _tuple1**_tmp521=(struct _tuple1**)_region_malloc(r,_check_times(sizeof(struct _tuple1*),_tmp520));struct _dyneither_ptr _tmp523=_tag_dyneither(_tmp521,sizeof(struct _tuple1*),_tmp520);{unsigned int _tmp522=_tmp520;unsigned int i;for(i=0;i < _tmp522;i ++){_tmp521[i]=(struct _tuple1*)Cyc_Toc_temp_var();}}_tmp523;});
# 2897
if(_tmp4A9 != 0){
# 2899
struct Cyc_List_List*_tmp518=0;
{int i=_tmp4A9 - 1;for(0;i >= 0;-- i){
_tmp518=({struct Cyc_List_List*_tmp519=_cycalloc(sizeof(*_tmp519));_tmp519->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0),0));_tmp519->tl=_tmp518;_tmp519;});}}
# 2903
s=Cyc_Absyn_declare_stmt(argv,arr_type,Cyc_Absyn_unresolvedmem_exp(0,_tmp518,0),s,0);{
# 2906
int i=0;for(0;_tmp4A8 != 0;(((_tmp4A8=_tmp4A8->tl,_tmp4AA=_tmp4AA->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp4A8->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0);
struct Cyc_Absyn_Datatypefield*_tmp51A=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp4AA))->hd;struct Cyc_Absyn_Datatypefield*_tmp51B=_tmp51A;struct _tuple1*_tmp51C;struct Cyc_List_List*_tmp51D;_LL32F: _tmp51C=_tmp51B->name;_tmp51D=_tmp51B->typs;_LL330:;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp51D))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);
# 2921
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);
# 2924
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Toc_tag_sp,0),
Cyc_Toc_datatype_tag(tud,_tmp51C),0),s,0);
# 2927
s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp51C,tud->name)),0,s,0);};}};}else{
# 2934
struct Cyc_List_List*_tmp51E=({struct _tuple19*_tmp51F[3];_tmp51F[2]=
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0));_tmp51F[1]=
# 2934
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0));_tmp51F[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0));((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp51F,sizeof(struct _tuple19*),3));});
# 2936
s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),Cyc_Absyn_uint_exp(0,0),s,0);}};}else{
# 2946
{int i=0;for(0;_tmp4A8 != 0;(_tmp4A8=_tmp4A8->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4A8->hd);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp4A8->hd,0),s,0);}}
# 2952
s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}
# 2965 "toc.cyc"
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2967
_npop_handler(0);goto _LL291;
# 2861 "toc.cyc"
;_pop_region(r);}}case 10: _LL2AA: _tmp4A5=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp4A6=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL2AB:
# 2970 "toc.cyc"
 Cyc_Toc_exp_to_c(nv,_tmp4A5);{
struct Cyc_Absyn_Exp*_tmp524=_tmp4A6?Cyc_Toc_newrethrow_exp(_tmp4A5): Cyc_Toc_newthrow_exp(_tmp4A5);
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),_tmp524,0))->r;
goto _LL291;};case 11: _LL2AC: _tmp4A4=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_LL2AD:
 Cyc_Toc_exp_to_c(nv,_tmp4A4);goto _LL291;case 12: _LL2AE: _tmp4A2=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp4A3=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL2AF:
# 2976
 Cyc_Toc_exp_to_c(nv,_tmp4A2);
# 2985 "toc.cyc"
for(0;_tmp4A3 != 0;_tmp4A3=_tmp4A3->tl){
enum Cyc_Absyn_KindQual _tmp525=(Cyc_Tcutil_typ_kind((void*)_tmp4A3->hd))->kind;
if(_tmp525 != Cyc_Absyn_EffKind  && _tmp525 != Cyc_Absyn_RgnKind){
{void*_tmp526=Cyc_Tcutil_compress((void*)_tmp4A3->hd);void*_tmp527=_tmp526;switch(*((int*)_tmp527)){case 2: _LL332: _LL333:
 goto _LL335;case 3: _LL334: _LL335:
 continue;default: _LL336: _LL337:
# 2992
 e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp4A2,0))->r;
goto _LL331;}_LL331:;}
# 2995
break;}}
# 2998
goto _LL291;case 13: _LL2B0: _tmp49E=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp49F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_tmp4A0=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp46E)->f3;_tmp4A1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp46E)->f4;_LL2B1: {
# 3000
void*old_t2=(void*)_check_null(_tmp49F->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp49E;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp49E=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp49F);
# 3007
{struct _tuple29 _tmp528=({struct _tuple29 _tmp563;_tmp563.f1=Cyc_Tcutil_compress(old_t2);_tmp563.f2=Cyc_Tcutil_compress(new_typ);_tmp563;});struct _tuple29 _tmp529=_tmp528;struct Cyc_Absyn_PtrInfo _tmp52A;struct Cyc_Absyn_PtrInfo _tmp52B;struct Cyc_Absyn_PtrInfo _tmp52C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp529.f1)->tag == 5)switch(*((int*)_tmp529.f2)){case 5: _LL339: _tmp52B=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp529.f1)->f1;_tmp52C=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp529.f2)->f1;_LL33A: {
# 3009
int _tmp52D=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp52B.ptr_atts).nullable);
int _tmp52E=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp52C.ptr_atts).nullable);
void*_tmp52F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp52B.ptr_atts).bounds);
void*_tmp530=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp52C.ptr_atts).bounds);
int _tmp531=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp52B.ptr_atts).zero_term);
int _tmp532=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp52C.ptr_atts).zero_term);
{struct _tuple29 _tmp533=({struct _tuple29 _tmp560;_tmp560.f1=_tmp52F;_tmp560.f2=_tmp530;_tmp560;});struct _tuple29 _tmp534=_tmp533;struct Cyc_Absyn_Exp*_tmp535;struct Cyc_Absyn_Exp*_tmp536;struct Cyc_Absyn_Exp*_tmp537;struct Cyc_Absyn_Exp*_tmp538;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp534.f1)->tag == 1){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp534.f2)->tag == 1){_LL340: _tmp537=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp534.f1)->f1;_tmp538=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp534.f2)->f1;_LL341:
# 3017
 if((!Cyc_Evexp_c_can_eval(_tmp537) || !Cyc_Evexp_c_can_eval(_tmp538)) && !
Cyc_Evexp_same_const_exp(_tmp537,_tmp538))
({void*_tmp539=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp53A="can't validate cast due to potential size differences";_tag_dyneither(_tmp53A,sizeof(char),54);}),_tag_dyneither(_tmp539,sizeof(void*),0));});
if(_tmp52D  && !_tmp52E){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp53B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp53C="can't do NULL-check conversion at top-level";_tag_dyneither(_tmp53C,sizeof(char),44);}),_tag_dyneither(_tmp53B,sizeof(void*),0));});
# 3026
if(_tmp4A1 != Cyc_Absyn_NonNull_to_Null)
({struct Cyc_String_pa_PrintArg_struct _tmp53F;_tmp53F.tag=0;_tmp53F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp53D[1]={& _tmp53F};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp53E="null-check conversion mis-classified: %s";_tag_dyneither(_tmp53E,sizeof(char),41);}),_tag_dyneither(_tmp53D,sizeof(void*),1));});});{
int do_null_check=Cyc_Toc_need_null_check(_tmp49F);
if(do_null_check){
if(!_tmp4A0)
({void*_tmp540=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp541="inserted null check due to implicit cast from * to @ type";_tag_dyneither(_tmp541,sizeof(char),58);}),_tag_dyneither(_tmp540,sizeof(void*),0));});
# 3033
e->r=Cyc_Toc_cast_it_r(*_tmp49E,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp542=_cycalloc(sizeof(*_tmp542));_tmp542->hd=_tmp49F;_tmp542->tl=0;_tmp542;}),0));}else{
# 3037
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp49F->r;}};}else{
# 3042
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp49F->r;}
# 3049
goto _LL33F;}else{_LL342: _tmp536=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp534.f1)->f1;_LL343:
# 3051
 if(!Cyc_Evexp_c_can_eval(_tmp536))
({void*_tmp543=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp544="cannot perform coercion since numelts cannot be determined statically.";_tag_dyneither(_tmp544,sizeof(char),71);}),_tag_dyneither(_tmp543,sizeof(void*),0));});
# 3054
if(_tmp4A1 == Cyc_Absyn_NonNull_to_Null)
({struct Cyc_String_pa_PrintArg_struct _tmp547;_tmp547.tag=0;_tmp547.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp545[1]={& _tmp547};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp546="conversion mis-classified as null-check: %s";_tag_dyneither(_tmp546,sizeof(char),44);}),_tag_dyneither(_tmp545,sizeof(void*),1));});});
if(Cyc_Toc_is_toplevel(nv)){
# 3058
if((_tmp531  && !(_tmp52C.elt_tq).real_const) && !_tmp532)
# 3061
_tmp536=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp536,Cyc_Absyn_uint_exp(1,0),0);
# 3063
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp536,_tmp49F))->r;}else{
# 3065
struct Cyc_Absyn_Exp*_tmp548=Cyc_Toc__tag_dyneither_e;
# 3067
if(_tmp531){
# 3072
struct _tuple1*_tmp549=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp54A=Cyc_Absyn_var_exp(_tmp549,0);
struct Cyc_Absyn_Exp*arg3;
# 3077
{void*_tmp54B=_tmp49F->r;void*_tmp54C=_tmp54B;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp54C)->tag == 0)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp54C)->f1).Wstring_c).tag){case 8: _LL349: _LL34A:
# 3079
 arg3=_tmp536;goto _LL348;case 9: _LL34B: _LL34C:
# 3081
 arg3=_tmp536;goto _LL348;default: goto _LL34D;}else{_LL34D: _LL34E:
# 3083
 arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp49F),({struct Cyc_Absyn_Exp*_tmp54D[2];_tmp54D[1]=_tmp536;_tmp54D[0]=
# 3085
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp54A);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp54D,sizeof(struct Cyc_Absyn_Exp*),2));}),0);
goto _LL348;}_LL348:;}
# 3088
if(!_tmp532  && !(_tmp52C.elt_tq).real_const)
# 3091
arg3=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{
# 3093
struct Cyc_Absyn_Exp*_tmp54E=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp52C.elt_typ),0);
struct Cyc_Absyn_Exp*_tmp54F=Cyc_Absyn_fncall_exp(_tmp548,({struct Cyc_Absyn_Exp*_tmp551[3];_tmp551[2]=arg3;_tmp551[1]=_tmp54E;_tmp551[0]=_tmp54A;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp551,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
struct Cyc_Absyn_Stmt*_tmp550=Cyc_Absyn_exp_stmt(_tmp54F,0);
_tmp550=Cyc_Absyn_declare_stmt(_tmp549,Cyc_Toc_typ_to_c(old_t2),_tmp49F,_tmp550,0);
e->r=Cyc_Toc_stmt_exp_r(_tmp550);};}else{
# 3100
e->r=Cyc_Toc_fncall_exp_r(_tmp548,({struct Cyc_Absyn_Exp*_tmp552[3];_tmp552[2]=_tmp536;_tmp552[1]=
# 3102
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp52C.elt_typ),0);_tmp552[0]=_tmp49F;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp552,sizeof(struct Cyc_Absyn_Exp*),3));}));}}
# 3106
goto _LL33F;}}else{if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp534.f2)->tag == 1){_LL344: _tmp535=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp534.f2)->f1;_LL345:
# 3108
 if(!Cyc_Evexp_c_can_eval(_tmp535))
({void*_tmp553=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp554="cannot perform coercion since numelts cannot be determined statically.";_tag_dyneither(_tmp554,sizeof(char),71);}),_tag_dyneither(_tmp553,sizeof(void*),0));});
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp555=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp556="can't coerce t? to t* or t@ at the top-level";_tag_dyneither(_tmp556,sizeof(char),45);}),_tag_dyneither(_tmp555,sizeof(void*),0));});{
# 3120 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp557=_tmp535;
if(_tmp531  && !_tmp532)
_tmp557=Cyc_Absyn_add_exp(_tmp535,Cyc_Absyn_uint_exp(1,0),0);{
# 3127
struct Cyc_Absyn_Exp*_tmp558=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmp559=Cyc_Absyn_fncall_exp(_tmp558,({struct Cyc_Absyn_Exp*_tmp55B[3];_tmp55B[2]=_tmp557;_tmp55B[1]=
# 3130
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp52B.elt_typ),0);_tmp55B[0]=_tmp49F;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp55B,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
# 3132
if(_tmp52E)
_tmp559->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp55A=_cycalloc(sizeof(*_tmp55A));_tmp55A->hd=
Cyc_Absyn_copy_exp(_tmp559);_tmp55A->tl=0;_tmp55A;}));
e->r=Cyc_Toc_cast_it_r(*_tmp49E,_tmp559);
goto _LL33F;};};}else{_LL346: _LL347:
# 3140
 DynCast:
 if((_tmp531  && !_tmp532) && !(_tmp52C.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp55C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp55D="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";_tag_dyneither(_tmp55D,sizeof(char),70);}),_tag_dyneither(_tmp55C,sizeof(void*),0));});{
struct Cyc_Absyn_Exp*_tmp55E=Cyc_Toc__dyneither_ptr_decrease_size_e;
e->r=Cyc_Toc_fncall_exp_r(_tmp55E,({struct Cyc_Absyn_Exp*_tmp55F[3];_tmp55F[2]=
# 3147
Cyc_Absyn_uint_exp(1,0);_tmp55F[1]=
# 3146
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp52B.elt_typ),0);_tmp55F[0]=_tmp49F;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp55F,sizeof(struct Cyc_Absyn_Exp*),3));}));};}
# 3149
goto _LL33F;}}_LL33F:;}
# 3151
goto _LL338;}case 6: _LL33B: _tmp52A=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp529.f1)->f1;_LL33C:
# 3153
{void*_tmp561=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp52A.ptr_atts).bounds);void*_tmp562=_tmp561;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp562)->tag == 0){_LL350: _LL351:
# 3155
 _tmp49F->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp49F->r,_tmp49F->loc),Cyc_Toc_curr_sp);goto _LL34F;}else{_LL352: _LL353:
 goto _LL34F;}_LL34F:;}
# 3158
goto _LL338;default: goto _LL33D;}else{_LL33D: _LL33E:
# 3160
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp49F->r;
goto _LL338;}_LL338:;}
# 3164
goto _LL291;}case 14: _LL2B2: _tmp49D=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_LL2B3:
# 3168
{void*_tmp564=_tmp49D->r;void*_tmp565=_tmp564;struct Cyc_List_List*_tmp566;struct _tuple1*_tmp567;struct Cyc_List_List*_tmp568;struct Cyc_List_List*_tmp569;switch(*((int*)_tmp565)){case 28: _LL355: _tmp567=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp565)->f1;_tmp568=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp565)->f2;_tmp569=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp565)->f3;_LL356:
# 3170
 if(Cyc_Toc_is_toplevel(nv))
({struct Cyc_String_pa_PrintArg_struct _tmp56C;_tmp56C.tag=0;_tmp56C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp49D->loc));({void*_tmp56A[1]={& _tmp56C};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp56B="%s: & on non-identifiers at the top-level";_tag_dyneither(_tmp56B,sizeof(char),42);}),_tag_dyneither(_tmp56A,sizeof(void*),1));});});{
struct Cyc_Absyn_Exp*_tmp56D=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp49D->topt),_tmp568,1,0,_tmp569,_tmp567);
e->r=_tmp56D->r;
e->topt=_tmp56D->topt;
goto _LL354;};case 23: _LL357: _tmp566=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp565)->f1;_LL358:
# 3178
 if(Cyc_Toc_is_toplevel(nv))
({struct Cyc_String_pa_PrintArg_struct _tmp570;_tmp570.tag=0;_tmp570.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp49D->loc));({void*_tmp56E[1]={& _tmp570};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp56F="%s: & on non-identifiers at the top-level";_tag_dyneither(_tmp56F,sizeof(char),42);}),_tag_dyneither(_tmp56E,sizeof(void*),1));});});
e->r=(Cyc_Toc_init_tuple(nv,1,0,_tmp566))->r;
goto _LL354;default: _LL359: _LL35A:
# 3184
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp49D);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp49D)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp49D,0,Cyc_Toc_address_lvalue,1);
# 3190
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp49D);}else{
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind((void*)_check_null(_tmp49D->topt))))
# 3194
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp49D);}
# 3196
goto _LL354;}_LL354:;}
# 3198
goto _LL291;case 15: _LL2B4: _tmp49B=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp49C=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL2B5:
# 3201
 if(Cyc_Toc_is_toplevel(nv))
({struct Cyc_String_pa_PrintArg_struct _tmp573;_tmp573.tag=0;_tmp573.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp49C->loc));({void*_tmp571[1]={& _tmp573};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp572="%s: new at top-level";_tag_dyneither(_tmp572,sizeof(char),21);}),_tag_dyneither(_tmp571,sizeof(void*),1));});});{
void*new_e_type=(void*)_check_null(_tmp49C->topt);
{void*_tmp574=_tmp49C->r;void*_tmp575=_tmp574;struct Cyc_List_List*_tmp576;struct _tuple1*_tmp577;struct Cyc_List_List*_tmp578;struct Cyc_List_List*_tmp579;struct Cyc_Absyn_Aggrdecl*_tmp57A;struct Cyc_Absyn_Exp*_tmp57B;void*_tmp57C;int _tmp57D;struct Cyc_Absyn_Vardecl*_tmp57E;struct Cyc_Absyn_Exp*_tmp57F;struct Cyc_Absyn_Exp*_tmp580;int _tmp581;struct Cyc_List_List*_tmp582;switch(*((int*)_tmp575)){case 25: _LL35C: _tmp582=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp575)->f1;_LL35D: {
# 3209
struct _tuple1*_tmp583=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp584=Cyc_Absyn_var_exp(_tmp583,0);
struct Cyc_Absyn_Stmt*_tmp585=Cyc_Toc_init_array(nv,new_e_type,_tmp584,_tmp582,Cyc_Absyn_exp_stmt(_tmp584,0));
void*old_elt_typ;
{void*_tmp586=Cyc_Tcutil_compress(old_typ);void*_tmp587=_tmp586;void*_tmp588;struct Cyc_Absyn_Tqual _tmp589;union Cyc_Absyn_Constraint*_tmp58A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp587)->tag == 5){_LL369: _tmp588=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp587)->f1).elt_typ;_tmp589=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp587)->f1).elt_tq;_tmp58A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp587)->f1).ptr_atts).zero_term;_LL36A:
# 3215
 old_elt_typ=_tmp588;goto _LL368;}else{_LL36B: _LL36C:
# 3217
 old_elt_typ=({void*_tmp58B=0;((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp58C="exp_to_c:new array expression doesn't have ptr type";_tag_dyneither(_tmp58C,sizeof(char),52);}),_tag_dyneither(_tmp58B,sizeof(void*),0));});}_LL368:;}{
# 3220
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*_tmp58D=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp58E=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),
Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp582),0),0);
struct Cyc_Absyn_Exp*e1;
if(_tmp49B == 0  || Cyc_Absyn_no_regions)
e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp58E);else{
# 3228
struct Cyc_Absyn_Exp*r=_tmp49B;
Cyc_Toc_exp_to_c(nv,r);
e1=Cyc_Toc_rmalloc_exp(r,_tmp58E);}
# 3233
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp583,_tmp58D,e1,_tmp585,0));
goto _LL35B;};}case 26: _LL35E: _tmp57E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp575)->f1;_tmp57F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp575)->f2;_tmp580=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp575)->f3;_tmp581=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp575)->f4;_LL35F:
# 3237
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp49B,old_typ,_tmp57F,(void*)_check_null(_tmp580->topt),_tmp581,_tmp580,_tmp57E);
goto _LL35B;case 27: _LL360: _tmp57B=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp575)->f1;_tmp57C=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp575)->f2;_tmp57D=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp575)->f3;_LL361:
# 3241
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp49B,old_typ,_tmp57B,_tmp57C,_tmp57D,0,0);
goto _LL35B;case 28: _LL362: _tmp577=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp575)->f1;_tmp578=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp575)->f2;_tmp579=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp575)->f3;_tmp57A=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp575)->f4;_LL363: {
# 3246
struct Cyc_Absyn_Exp*_tmp58F=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp49C->topt),_tmp578,1,_tmp49B,_tmp579,_tmp577);
e->r=_tmp58F->r;
e->topt=_tmp58F->topt;
goto _LL35B;}case 23: _LL364: _tmp576=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp575)->f1;_LL365:
# 3252
 e->r=(Cyc_Toc_init_tuple(nv,1,_tmp49B,_tmp576))->r;
goto _LL35B;default: _LL366: _LL367: {
# 3259
void*old_elt_typ=(void*)_check_null(_tmp49C->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
# 3262
struct _tuple1*_tmp590=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp591=Cyc_Absyn_var_exp(_tmp590,0);
struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp591,0),0);
struct Cyc_Absyn_Exp*inner_mexp=mexp;
if(_tmp49B == 0  || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
# 3269
struct Cyc_Absyn_Exp*r=_tmp49B;
Cyc_Toc_exp_to_c(nv,r);
mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{
# 3277
int done=0;
{void*_tmp592=_tmp49C->r;void*_tmp593=_tmp592;void*_tmp594;struct Cyc_Absyn_Exp*_tmp595;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp593)->tag == 13){_LL36E: _tmp594=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp593)->f1;_tmp595=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp593)->f2;_LL36F:
# 3280
{struct _tuple29 _tmp596=({struct _tuple29 _tmp5A1;_tmp5A1.f1=Cyc_Tcutil_compress(_tmp594);_tmp5A1.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp595->topt));_tmp5A1;});struct _tuple29 _tmp597=_tmp596;void*_tmp598;union Cyc_Absyn_Constraint*_tmp599;union Cyc_Absyn_Constraint*_tmp59A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp597.f1)->tag == 5){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp597.f2)->tag == 5){_LL373: _tmp598=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp597.f1)->f1).elt_typ;_tmp599=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp597.f1)->f1).ptr_atts).bounds;_tmp59A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp597.f2)->f1).ptr_atts).bounds;_LL374:
# 3283
{struct _tuple29 _tmp59B=({struct _tuple29 _tmp5A0;_tmp5A0.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp599);_tmp5A0.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp59A);_tmp5A0;});struct _tuple29 _tmp59C=_tmp59B;struct Cyc_Absyn_Exp*_tmp59D;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp59C.f1)->tag == 0){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp59C.f2)->tag == 1){_LL378: _tmp59D=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp59C.f2)->f1;_LL379:
# 3285
 Cyc_Toc_exp_to_c(nv,_tmp595);
inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);
done=1;{
struct Cyc_Absyn_Exp*_tmp59E=Cyc_Toc__init_dyneither_ptr_e;
e->r=Cyc_Toc_fncall_exp_r(_tmp59E,({struct Cyc_Absyn_Exp*_tmp59F[4];_tmp59F[3]=_tmp59D;_tmp59F[2]=
# 3291
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp598),0);_tmp59F[1]=_tmp595;_tmp59F[0]=mexp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp59F,sizeof(struct Cyc_Absyn_Exp*),4));}));
# 3293
goto _LL377;};}else{goto _LL37A;}}else{_LL37A: _LL37B:
 goto _LL377;}_LL377:;}
# 3296
goto _LL372;}else{goto _LL375;}}else{_LL375: _LL376:
 goto _LL372;}_LL372:;}
# 3299
goto _LL36D;}else{_LL370: _LL371:
 goto _LL36D;}_LL36D:;}
# 3302
if(!done){
struct Cyc_Absyn_Stmt*_tmp5A2=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp591),0);
struct Cyc_Absyn_Exp*_tmp5A3=Cyc_Absyn_signed_int_exp(0,0);
Cyc_Toc_exp_to_c(nv,_tmp49C);
_tmp5A2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp591,_tmp5A3,0),_tmp49C,0),_tmp5A2,0);{
# 3308
void*_tmp5A4=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp590,_tmp5A4,mexp,_tmp5A2,0));};}
# 3311
goto _LL35B;};}}_LL35B:;}
# 3313
goto _LL291;};case 17: _LL2B6: _tmp49A=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_LL2B7: {
# 3316
int _tmp5A5=Cyc_Toc_set_in_sizeof(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp49A);
Cyc_Toc_set_in_sizeof(nv,_tmp5A5);
goto _LL291;}case 16: _LL2B8: _tmp499=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_LL2B9:
 e->r=(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp5A6=_cycalloc(sizeof(*_tmp5A6));_tmp5A6[0]=({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp5A7;_tmp5A7.tag=16;_tmp5A7.f1=Cyc_Toc_typ_to_c(_tmp499);_tmp5A7;});_tmp5A6;});goto _LL291;case 18: _LL2BA: _tmp497=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp498=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL2BB: {
# 3322
void*_tmp5A8=_tmp497;
struct Cyc_List_List*_tmp5A9=_tmp498;
for(0;_tmp5A9 != 0;_tmp5A9=_tmp5A9->tl){
void*_tmp5AA=(void*)_tmp5A9->hd;void*_tmp5AB=_tmp5AA;unsigned int _tmp5AC;struct _dyneither_ptr*_tmp5AD;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp5AB)->tag == 0){_LL37D: _tmp5AD=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp5AB)->f1;_LL37E:
 goto _LL37C;}else{_LL37F: _tmp5AC=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp5AB)->f1;_LL380:
# 3328
{void*_tmp5AE=Cyc_Tcutil_compress(_tmp5A8);void*_tmp5AF=_tmp5AE;struct Cyc_Absyn_Datatypefield*_tmp5B0;struct Cyc_List_List*_tmp5B1;struct Cyc_List_List*_tmp5B2;union Cyc_Absyn_AggrInfoU _tmp5B3;switch(*((int*)_tmp5AF)){case 11: _LL382: _tmp5B3=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5AF)->f1).aggr_info;_LL383: {
# 3330
struct Cyc_Absyn_Aggrdecl*_tmp5B4=Cyc_Absyn_get_known_aggrdecl(_tmp5B3);
if(_tmp5B4->impl == 0)
({void*_tmp5B5=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp5B6="struct fields must be known";_tag_dyneither(_tmp5B6,sizeof(char),28);}),_tag_dyneither(_tmp5B5,sizeof(void*),0));});
_tmp5B2=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp5B4->impl))->fields;goto _LL385;}case 12: _LL384: _tmp5B2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5AF)->f2;_LL385: {
# 3335
struct Cyc_Absyn_Aggrfield*_tmp5B7=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp5B2,(int)_tmp5AC);
_tmp5A9->hd=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp5B8=_cycalloc(sizeof(*_tmp5B8));_tmp5B8[0]=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp5B9;_tmp5B9.tag=0;_tmp5B9.f1=_tmp5B7->name;_tmp5B9;});_tmp5B8;}));
_tmp5A8=_tmp5B7->type;
goto _LL381;}case 10: _LL386: _tmp5B1=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5AF)->f1;_LL387:
# 3340
 _tmp5A9->hd=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp5BA=_cycalloc(sizeof(*_tmp5BA));_tmp5BA[0]=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp5BB;_tmp5BB.tag=0;_tmp5BB.f1=Cyc_Absyn_fieldname((int)(_tmp5AC + 1));_tmp5BB;});_tmp5BA;}));
_tmp5A8=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp5B1,(int)_tmp5AC)).f2;
goto _LL381;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp5AF)->f1).field_info).KnownDatatypefield).tag == 2){_LL388: _tmp5B0=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp5AF)->f1).field_info).KnownDatatypefield).val).f2;_LL389:
# 3344
 if(_tmp5AC == 0)
_tmp5A9->hd=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp5BC=_cycalloc(sizeof(*_tmp5BC));_tmp5BC[0]=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp5BD;_tmp5BD.tag=0;_tmp5BD.f1=Cyc_Toc_tag_sp;_tmp5BD;});_tmp5BC;}));else{
# 3347
_tmp5A8=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp5B0->typs,(int)(_tmp5AC - 1))).f2;
_tmp5A9->hd=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp5BE=_cycalloc(sizeof(*_tmp5BE));_tmp5BE[0]=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp5BF;_tmp5BF.tag=0;_tmp5BF.f1=Cyc_Absyn_fieldname((int)_tmp5AC);_tmp5BF;});_tmp5BE;}));}
# 3350
goto _LL381;}else{goto _LL38A;}default: _LL38A: _LL38B:
({void*_tmp5C0=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp5C1="impossible type for offsetof tuple index";_tag_dyneither(_tmp5C1,sizeof(char),41);}),_tag_dyneither(_tmp5C0,sizeof(void*),0));});
goto _LL381;}_LL381:;}
# 3354
goto _LL37C;}_LL37C:;}
# 3357
e->r=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp5C2=_cycalloc(sizeof(*_tmp5C2));_tmp5C2[0]=({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp5C3;_tmp5C3.tag=18;_tmp5C3.f1=Cyc_Toc_typ_to_c(_tmp497);_tmp5C3.f2=_tmp498;_tmp5C3;});_tmp5C2;});
goto _LL291;}case 19: _LL2BC: _tmp496=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_LL2BD: {
# 3360
int _tmp5C4=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp5C5=Cyc_Tcutil_compress((void*)_check_null(_tmp496->topt));
{void*_tmp5C6=_tmp5C5;void*_tmp5C7;struct Cyc_Absyn_Tqual _tmp5C8;void*_tmp5C9;union Cyc_Absyn_Constraint*_tmp5CA;union Cyc_Absyn_Constraint*_tmp5CB;union Cyc_Absyn_Constraint*_tmp5CC;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C6)->tag == 5){_LL38D: _tmp5C7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C6)->f1).elt_typ;_tmp5C8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C6)->f1).elt_tq;_tmp5C9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C6)->f1).ptr_atts).rgn;_tmp5CA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C6)->f1).ptr_atts).nullable;_tmp5CB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C6)->f1).ptr_atts).bounds;_tmp5CC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C6)->f1).ptr_atts).zero_term;_LL38E:
# 3365
{void*_tmp5CD=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5CB);void*_tmp5CE=_tmp5CD;struct Cyc_Absyn_Exp*_tmp5CF;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5CE)->tag == 1){_LL392: _tmp5CF=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5CE)->f1;_LL393: {
# 3367
int do_null_check=!Cyc_Toc_in_sizeof(nv) && Cyc_Toc_need_null_check(_tmp496);
Cyc_Toc_exp_to_c(nv,_tmp496);
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp5D0=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp5D1="inserted null check due to dereference";_tag_dyneither(_tmp5D1,sizeof(char),39);}),_tag_dyneither(_tmp5D0,sizeof(void*),0));});
# 3373
_tmp496->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(_tmp5C5),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp5D2=_cycalloc(sizeof(*_tmp5D2));_tmp5D2->hd=
Cyc_Absyn_copy_exp(_tmp496);_tmp5D2->tl=0;_tmp5D2;}),0));}
# 3381
if(!Cyc_Toc_in_sizeof(nv) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5CC)){
struct _tuple12 _tmp5D3=Cyc_Evexp_eval_const_uint_exp(_tmp5CF);struct _tuple12 _tmp5D4=_tmp5D3;unsigned int _tmp5D5;int _tmp5D6;_LL397: _tmp5D5=_tmp5D4.f1;_tmp5D6=_tmp5D4.f2;_LL398:;
# 3387
if(!_tmp5D6  || _tmp5D5 <= 0)
({void*_tmp5D7=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp5D8="cannot determine dereference is in bounds";_tag_dyneither(_tmp5D8,sizeof(char),42);}),_tag_dyneither(_tmp5D7,sizeof(void*),0));});}
# 3390
goto _LL391;}}else{_LL394: _LL395: {
# 3393
struct Cyc_Absyn_Exp*_tmp5D9=Cyc_Absyn_uint_exp(0,0);
_tmp5D9->topt=Cyc_Absyn_uint_typ;
e->r=Cyc_Toc_subscript_exp_r(_tmp496,_tmp5D9);
Cyc_Toc_exp_to_c(nv,e);
goto _LL391;}}_LL391:;}
# 3399
goto _LL38C;}else{_LL38F: _LL390:
({void*_tmp5DA=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp5DB="exp_to_c: Deref: non-pointer";_tag_dyneither(_tmp5DB,sizeof(char),29);}),_tag_dyneither(_tmp5DA,sizeof(void*),0));});}_LL38C:;}
# 3402
Cyc_Toc_set_lhs(nv,_tmp5C4);
goto _LL291;};}case 20: _LL2BE: _tmp492=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp493=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_tmp494=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp46E)->f3;_tmp495=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp46E)->f4;_LL2BF: {
# 3405
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp492->topt);
Cyc_Toc_exp_to_c(nv,_tmp492);
if(_tmp494  && _tmp495)
e->r=Cyc_Toc_check_tagged_union(_tmp492,Cyc_Toc_typ_to_c(e1_cyc_type),e1_cyc_type,_tmp493,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);
# 3413
if(is_poly)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
goto _LL291;}case 21: _LL2C0: _tmp48E=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp48F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_tmp490=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp46E)->f3;_tmp491=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp46E)->f4;_LL2C1: {
# 3417
int _tmp5DC=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp48E->topt));
int do_null_check=Cyc_Toc_need_null_check(_tmp48E);
Cyc_Toc_exp_to_c(nv,_tmp48E);{
int is_poly=Cyc_Toc_is_poly_project(e);
struct Cyc_Absyn_PtrInfo _tmp5DD=Cyc_Toc_get_ptr_type(e1typ);struct Cyc_Absyn_PtrInfo _tmp5DE=_tmp5DD;void*_tmp5DF;struct Cyc_Absyn_Tqual _tmp5E0;void*_tmp5E1;union Cyc_Absyn_Constraint*_tmp5E2;union Cyc_Absyn_Constraint*_tmp5E3;union Cyc_Absyn_Constraint*_tmp5E4;_LL39A: _tmp5DF=_tmp5DE.elt_typ;_tmp5E0=_tmp5DE.elt_tq;_tmp5E1=(_tmp5DE.ptr_atts).rgn;_tmp5E2=(_tmp5DE.ptr_atts).nullable;_tmp5E3=(_tmp5DE.ptr_atts).bounds;_tmp5E4=(_tmp5DE.ptr_atts).zero_term;_LL39B:;
{void*_tmp5E5=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5E3);void*_tmp5E6=_tmp5E5;struct Cyc_Absyn_Exp*_tmp5E7;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5E6)->tag == 1){_LL39D: _tmp5E7=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5E6)->f1;_LL39E: {
# 3428
struct _tuple12 _tmp5E8=Cyc_Evexp_eval_const_uint_exp(_tmp5E7);struct _tuple12 _tmp5E9=_tmp5E8;unsigned int _tmp5EA;int _tmp5EB;_LL3A2: _tmp5EA=_tmp5E9.f1;_tmp5EB=_tmp5E9.f2;_LL3A3:;
if(_tmp5EB){
if(_tmp5EA < 1)
({void*_tmp5EC=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp5ED="exp_to_c:  AggrArrow_e on pointer of size 0";_tag_dyneither(_tmp5ED,sizeof(char),44);}),_tag_dyneither(_tmp5EC,sizeof(void*),0));});
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp5EE=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp5EF="inserted null check due to dereference";_tag_dyneither(_tmp5EF,sizeof(char),39);}),_tag_dyneither(_tmp5EE,sizeof(void*),0));});
# 3436
_tmp48E->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_Absyn_Exp*_tmp5F0[1];_tmp5F0[0]=
Cyc_Absyn_new_exp(_tmp48E->r,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5F0,sizeof(struct Cyc_Absyn_Exp*),1));}),0));}}else{
# 3441
if(!Cyc_Evexp_c_can_eval(_tmp5E7))
({void*_tmp5F1=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp5F2="cannot determine pointer dereference in bounds";_tag_dyneither(_tmp5F2,sizeof(char),47);}),_tag_dyneither(_tmp5F1,sizeof(void*),0));});
# 3445
_tmp48E->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*_tmp5F3[4];_tmp5F3[3]=
# 3449
Cyc_Absyn_uint_exp(0,0);_tmp5F3[2]=
# 3448
Cyc_Absyn_sizeoftyp_exp(_tmp5DF,0);_tmp5F3[1]=_tmp5E7;_tmp5F3[0]=
# 3447
Cyc_Absyn_new_exp(_tmp48E->r,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5F3,sizeof(struct Cyc_Absyn_Exp*),4));}),0));}
# 3451
goto _LL39C;}}else{_LL39F: _LL3A0: {
# 3454
void*ta1=Cyc_Toc_typ_to_c(_tmp5DF);
_tmp48E->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp5E0),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_dyneither_subscript_e,({struct Cyc_Absyn_Exp*_tmp5F4[3];_tmp5F4[2]=
# 3459
Cyc_Absyn_uint_exp(0,0);_tmp5F4[1]=
# 3458
Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp5F4[0]=
# 3457
Cyc_Absyn_new_exp(_tmp48E->r,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5F4,sizeof(struct Cyc_Absyn_Exp*),3));}),0));
# 3462
goto _LL39C;}}_LL39C:;}
# 3464
if(_tmp490  && _tmp491)
e->r=Cyc_Toc_check_tagged_union(_tmp48E,Cyc_Toc_typ_to_c(e1typ),_tmp5DF,_tmp48F,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);
# 3467
if(is_poly  && _tmp491)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
Cyc_Toc_set_lhs(nv,_tmp5DC);
goto _LL291;};};}case 22: _LL2C2: _tmp48C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp48D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL2C3: {
# 3472
int _tmp5F5=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp5F6=Cyc_Tcutil_compress((void*)_check_null(_tmp48C->topt));
# 3476
{void*_tmp5F7=_tmp5F6;void*_tmp5F8;struct Cyc_Absyn_Tqual _tmp5F9;void*_tmp5FA;union Cyc_Absyn_Constraint*_tmp5FB;union Cyc_Absyn_Constraint*_tmp5FC;union Cyc_Absyn_Constraint*_tmp5FD;struct Cyc_List_List*_tmp5FE;switch(*((int*)_tmp5F7)){case 10: _LL3A5: _tmp5FE=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5F7)->f1;_LL3A6:
# 3479
 Cyc_Toc_exp_to_c(nv,_tmp48C);
Cyc_Toc_exp_to_c(nv,_tmp48D);{
struct _tuple12 _tmp5FF=Cyc_Evexp_eval_const_uint_exp(_tmp48D);struct _tuple12 _tmp600=_tmp5FF;unsigned int _tmp601;int _tmp602;_LL3AC: _tmp601=_tmp600.f1;_tmp602=_tmp600.f2;_LL3AD:;
if(!_tmp602)
({void*_tmp603=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp604="unknown tuple subscript in translation to C";_tag_dyneither(_tmp604,sizeof(char),44);}),_tag_dyneither(_tmp603,sizeof(void*),0));});
e->r=Cyc_Toc_aggrmember_exp_r(_tmp48C,Cyc_Absyn_fieldname((int)(_tmp601 + 1)));
goto _LL3A4;};case 5: _LL3A7: _tmp5F8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F7)->f1).elt_typ;_tmp5F9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F7)->f1).elt_tq;_tmp5FA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F7)->f1).ptr_atts).rgn;_tmp5FB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F7)->f1).ptr_atts).nullable;_tmp5FC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F7)->f1).ptr_atts).bounds;_tmp5FD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F7)->f1).ptr_atts).zero_term;_LL3A8: {
# 3487
struct Cyc_List_List*_tmp605=Cyc_Toc_get_relns(_tmp48C);
int _tmp606=Cyc_Toc_need_null_check(_tmp48C);
int _tmp607=Cyc_Toc_in_sizeof(nv);
# 3495
int in_bnds=_tmp607;
{void*_tmp608=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5FC);void*_tmp609=_tmp608;_LL3AF: _LL3B0:
# 3498
 in_bnds=in_bnds  || Cyc_Toc_check_bounds(_tmp5F6,_tmp605,_tmp48C,_tmp48D);
if(Cyc_Toc_warn_bounds_checks  && !in_bnds)
({struct Cyc_String_pa_PrintArg_struct _tmp60C;_tmp60C.tag=0;_tmp60C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp60A[1]={& _tmp60C};Cyc_Tcutil_warn(e->loc,({const char*_tmp60B="bounds check necessary for %s";_tag_dyneither(_tmp60B,sizeof(char),30);}),_tag_dyneither(_tmp60A,sizeof(void*),1));});});_LL3AE:;}
# 3505
Cyc_Toc_exp_to_c(nv,_tmp48C);
Cyc_Toc_exp_to_c(nv,_tmp48D);
++ Cyc_Toc_total_bounds_checks;
{void*_tmp60D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5FC);void*_tmp60E=_tmp60D;struct Cyc_Absyn_Exp*_tmp60F;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp60E)->tag == 1){_LL3B2: _tmp60F=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp60E)->f1;_LL3B3: {
# 3510
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5FB) && _tmp606;
void*ta1=Cyc_Toc_typ_to_c(_tmp5F8);
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp5F9);
if(in_bnds  && !possibly_null)
++ Cyc_Toc_bounds_checks_eliminated;else{
if(in_bnds  && possibly_null){
++ Cyc_Toc_bounds_checks_eliminated;
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp610=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp611="inserted null check due to dereference";_tag_dyneither(_tmp611,sizeof(char),39);}),_tag_dyneither(_tmp610,sizeof(void*),0));});
_tmp48C->r=Cyc_Toc_cast_it_r(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_Absyn_Exp*_tmp612[1];_tmp612[0]=
Cyc_Absyn_copy_exp(_tmp48C);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp612,sizeof(struct Cyc_Absyn_Exp*),1));}),0));}else{
# 3522
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5FD)){
# 3524
if(!Cyc_Evexp_c_can_eval(_tmp60F))
({void*_tmp613=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp614="cannot determine subscript is in bounds";_tag_dyneither(_tmp614,sizeof(char),40);}),_tag_dyneither(_tmp613,sizeof(void*),0));});{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp48C);
e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(function_e,({struct Cyc_Absyn_Exp*_tmp615[3];_tmp615[2]=_tmp48D;_tmp615[1]=_tmp60F;_tmp615[0]=_tmp48C;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp615,sizeof(struct Cyc_Absyn_Exp*),3));}),0)));};}else{
# 3530
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp60F))
({void*_tmp616=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp617="cannot determine subscript is in bounds";_tag_dyneither(_tmp617,sizeof(char),40);}),_tag_dyneither(_tmp616,sizeof(void*),0));});
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp618=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp619="inserted null check due to dereference";_tag_dyneither(_tmp619,sizeof(char),39);}),_tag_dyneither(_tmp618,sizeof(void*),0));});
# 3536
e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*_tmp61A[4];_tmp61A[3]=_tmp48D;_tmp61A[2]=
# 3539
Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp61A[1]=_tmp60F;_tmp61A[0]=_tmp48C;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp61A,sizeof(struct Cyc_Absyn_Exp*),4));}),0)));}else{
# 3542
if(!Cyc_Evexp_c_can_eval(_tmp60F))
({void*_tmp61B=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp61C="cannot determine subscript is in bounds";_tag_dyneither(_tmp61C,sizeof(char),40);}),_tag_dyneither(_tmp61B,sizeof(void*),0));});
# 3545
_tmp48D->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,({struct Cyc_Absyn_Exp*_tmp61D[2];_tmp61D[1]=
Cyc_Absyn_copy_exp(_tmp48D);_tmp61D[0]=_tmp60F;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp61D,sizeof(struct Cyc_Absyn_Exp*),2));}));}}}}
# 3548
goto _LL3B1;}}else{_LL3B4: _LL3B5: {
# 3550
void*ta1=Cyc_Toc_typ_to_c(_tmp5F8);
if(in_bnds){
# 3554
++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp5F9),
Cyc_Toc_member_exp(_tmp48C,Cyc_Toc_curr_sp,0)),_tmp48D);}else{
# 3559
struct Cyc_Absyn_Exp*_tmp61E=Cyc_Toc__check_dyneither_subscript_e;
e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp5F9),
Cyc_Absyn_fncall_exp(_tmp61E,({struct Cyc_Absyn_Exp*_tmp61F[3];_tmp61F[2]=_tmp48D;_tmp61F[1]=
Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp61F[0]=_tmp48C;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp61F,sizeof(struct Cyc_Absyn_Exp*),3));}),0)));}
# 3565
goto _LL3B1;}}_LL3B1:;}
# 3567
goto _LL3A4;}default: _LL3A9: _LL3AA:
({void*_tmp620=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp621="exp_to_c: Subscript on non-tuple/array/tuple ptr";_tag_dyneither(_tmp621,sizeof(char),49);}),_tag_dyneither(_tmp620,sizeof(void*),0));});}_LL3A4:;}
# 3570
Cyc_Toc_set_lhs(nv,_tmp5F5);
goto _LL291;};}case 23: _LL2C4: _tmp48B=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_LL2C5:
# 3573
 if(!Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp48B))->r;else{
# 3578
struct Cyc_List_List*_tmp622=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp48B);
void*_tmp623=Cyc_Toc_add_tuple_type(_tmp622);
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp48B != 0;(_tmp48B=_tmp48B->tl,i ++)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp48B->hd);
dles=({struct Cyc_List_List*_tmp624=_cycalloc(sizeof(*_tmp624));_tmp624->hd=({struct _tuple19*_tmp625=_cycalloc(sizeof(*_tmp625));_tmp625->f1=0;_tmp625->f2=(struct Cyc_Absyn_Exp*)_tmp48B->hd;_tmp625;});_tmp624->tl=dles;_tmp624;});}}
# 3585
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}
# 3588
goto _LL291;case 25: _LL2C6: _tmp48A=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_LL2C7:
# 3592
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp48A);
{struct Cyc_List_List*_tmp626=_tmp48A;for(0;_tmp626 != 0;_tmp626=_tmp626->tl){
struct _tuple19*_tmp627=(struct _tuple19*)_tmp626->hd;struct _tuple19*_tmp628=_tmp627;struct Cyc_Absyn_Exp*_tmp629;_LL3B7: _tmp629=_tmp628->f2;_LL3B8:;
Cyc_Toc_exp_to_c(nv,_tmp629);}}
# 3597
goto _LL291;case 26: _LL2C8: _tmp486=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp487=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_tmp488=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp46E)->f3;_tmp489=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp46E)->f4;_LL2C9: {
# 3601
struct _tuple12 _tmp62A=Cyc_Evexp_eval_const_uint_exp(_tmp487);struct _tuple12 _tmp62B=_tmp62A;unsigned int _tmp62C;int _tmp62D;_LL3BA: _tmp62C=_tmp62B.f1;_tmp62D=_tmp62B.f2;_LL3BB:;{
void*_tmp62E=Cyc_Toc_typ_to_c((void*)_check_null(_tmp488->topt));
Cyc_Toc_exp_to_c(nv,_tmp488);{
struct Cyc_List_List*es=0;
# 3606
if(!Cyc_Toc_is_zero(_tmp488)){
if(!_tmp62D)
({void*_tmp62F=0;Cyc_Tcutil_terr(_tmp487->loc,({const char*_tmp630="cannot determine value of constant";_tag_dyneither(_tmp630,sizeof(char),35);}),_tag_dyneither(_tmp62F,sizeof(void*),0));});
{unsigned int i=0;for(0;i < _tmp62C;++ i){
es=({struct Cyc_List_List*_tmp631=_cycalloc(sizeof(*_tmp631));_tmp631->hd=({struct _tuple19*_tmp632=_cycalloc(sizeof(*_tmp632));_tmp632->f1=0;_tmp632->f2=_tmp488;_tmp632;});_tmp631->tl=es;_tmp631;});}}
# 3612
if(_tmp489){
struct Cyc_Absyn_Exp*_tmp633=Cyc_Toc_cast_it(_tmp62E,Cyc_Absyn_uint_exp(0,0));
es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,({struct Cyc_List_List*_tmp634=_cycalloc(sizeof(*_tmp634));_tmp634->hd=({struct _tuple19*_tmp635=_cycalloc(sizeof(*_tmp635));_tmp635->f1=0;_tmp635->f2=_tmp633;_tmp635;});_tmp634->tl=0;_tmp634;}));}}
# 3617
e->r=Cyc_Toc_unresolvedmem_exp_r(0,es);
goto _LL291;};};}case 27: _LL2CA: _tmp483=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp484=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_tmp485=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp46E)->f3;_LL2CB:
# 3622
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,0);
goto _LL291;case 28: _LL2CC: _tmp47F=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp480=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_tmp481=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp46E)->f3;_tmp482=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp46E)->f4;_LL2CD:
# 3627
 if(!Cyc_Toc_is_toplevel(nv)){
struct Cyc_Absyn_Exp*_tmp636=Cyc_Toc_init_struct(nv,old_typ,_tmp480,0,0,_tmp481,_tmp47F);
e->r=_tmp636->r;
e->topt=_tmp636->topt;}else{
# 3638
if(_tmp482 == 0)
({void*_tmp637=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp638="Aggregate_e: missing aggrdecl pointer";_tag_dyneither(_tmp638,sizeof(char),38);}),_tag_dyneither(_tmp637,sizeof(void*),0));});{
struct Cyc_Absyn_Aggrdecl*sd2=_tmp482;
# 3642
struct _RegionHandle _tmp639=_new_region("rgn");struct _RegionHandle*rgn=& _tmp639;_push_region(rgn);
{struct Cyc_List_List*_tmp63A=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp481,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
# 3646
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp482->impl))->tagged){
# 3648
struct _tuple30*_tmp63B=(struct _tuple30*)((struct Cyc_List_List*)_check_null(_tmp63A))->hd;struct _tuple30*_tmp63C=_tmp63B;struct Cyc_Absyn_Aggrfield*_tmp63D;struct Cyc_Absyn_Exp*_tmp63E;_LL3BD: _tmp63D=_tmp63C->f1;_tmp63E=_tmp63C->f2;_LL3BE:;{
void*_tmp63F=(void*)_check_null(_tmp63E->topt);
void*_tmp640=_tmp63D->type;
Cyc_Toc_exp_to_c(nv,_tmp63E);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp640) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp63F))
_tmp63E->r=
Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp63E->r,0));{
# 3657
int i=Cyc_Toc_get_member_offset(_tmp482,_tmp63D->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct Cyc_List_List*_tmp641=({struct _tuple19*_tmp647[2];_tmp647[1]=({struct _tuple19*_tmp649=_cycalloc(sizeof(*_tmp649));_tmp649->f1=0;_tmp649->f2=_tmp63E;_tmp649;});_tmp647[0]=({struct _tuple19*_tmp648=_cycalloc(sizeof(*_tmp648));_tmp648->f1=0;_tmp648->f2=field_tag_exp;_tmp648;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp647,sizeof(struct _tuple19*),2));});
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp641,0);
struct Cyc_List_List*ds=({void*_tmp644[1];_tmp644[0]=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp645=_cycalloc(sizeof(*_tmp645));_tmp645[0]=({struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp646;_tmp646.tag=1;_tmp646.f1=_tmp63D->name;_tmp646;});_tmp645;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp644,sizeof(void*),1));});
struct Cyc_List_List*dles=({struct _tuple19*_tmp642[1];_tmp642[0]=({struct _tuple19*_tmp643=_cycalloc(sizeof(*_tmp643));_tmp643->f1=ds;_tmp643->f2=umem;_tmp643;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp642,sizeof(struct _tuple19*),1));});
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{
# 3666
struct Cyc_List_List*_tmp64A=0;
struct Cyc_List_List*_tmp64B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields;
for(0;_tmp64B != 0;_tmp64B=_tmp64B->tl){
struct Cyc_List_List*_tmp64C=_tmp63A;for(0;_tmp64C != 0;_tmp64C=_tmp64C->tl){
if((*((struct _tuple30*)_tmp64C->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp64B->hd){
struct _tuple30*_tmp64D=(struct _tuple30*)_tmp64C->hd;struct _tuple30*_tmp64E=_tmp64D;struct Cyc_Absyn_Aggrfield*_tmp64F;struct Cyc_Absyn_Exp*_tmp650;_LL3C0: _tmp64F=_tmp64E->f1;_tmp650=_tmp64E->f2;_LL3C1:;{
void*_tmp651=Cyc_Toc_typ_to_c(_tmp64F->type);
void*_tmp652=Cyc_Toc_typ_to_c((void*)_check_null(_tmp650->topt));
Cyc_Toc_exp_to_c(nv,_tmp650);
# 3676
if(!Cyc_Tcutil_unify(_tmp651,_tmp652)){
# 3678
if(!Cyc_Tcutil_is_arithmetic_type(_tmp651) || !
Cyc_Tcutil_is_arithmetic_type(_tmp652))
_tmp650=Cyc_Toc_cast_it(_tmp651,Cyc_Absyn_copy_exp(_tmp650));}
_tmp64A=({struct Cyc_List_List*_tmp653=_cycalloc(sizeof(*_tmp653));_tmp653->hd=({struct _tuple19*_tmp654=_cycalloc(sizeof(*_tmp654));_tmp654->f1=0;_tmp654->f2=_tmp650;_tmp654;});_tmp653->tl=_tmp64A;_tmp653;});
break;};}}}
# 3685
e->r=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp64A));}}
# 3643
;_pop_region(rgn);};}
# 3689
goto _LL291;case 29: _LL2CE: _tmp47D=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp47E=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL2CF: {
# 3691
struct Cyc_List_List*fs;
{void*_tmp655=Cyc_Tcutil_compress(_tmp47D);void*_tmp656=_tmp655;struct Cyc_List_List*_tmp657;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp656)->tag == 12){_LL3C3: _tmp657=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp656)->f2;_LL3C4:
 fs=_tmp657;goto _LL3C2;}else{_LL3C5: _LL3C6:
({struct Cyc_String_pa_PrintArg_struct _tmp65A;_tmp65A.tag=0;_tmp65A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp47D));({void*_tmp658[1]={& _tmp65A};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp659="anon struct has type %s";_tag_dyneither(_tmp659,sizeof(char),24);}),_tag_dyneither(_tmp658,sizeof(void*),1));});});}_LL3C2:;}{
# 3696
struct _RegionHandle _tmp65B=_new_region("rgn");struct _RegionHandle*rgn=& _tmp65B;_push_region(rgn);{
struct Cyc_List_List*_tmp65C=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp47E,Cyc_Absyn_StructA,fs);
for(0;_tmp65C != 0;_tmp65C=_tmp65C->tl){
struct _tuple30*_tmp65D=(struct _tuple30*)_tmp65C->hd;struct _tuple30*_tmp65E=_tmp65D;struct Cyc_Absyn_Aggrfield*_tmp65F;struct Cyc_Absyn_Exp*_tmp660;_LL3C8: _tmp65F=_tmp65E->f1;_tmp660=_tmp65E->f2;_LL3C9:;{
void*_tmp661=(void*)_check_null(_tmp660->topt);
void*_tmp662=_tmp65F->type;
Cyc_Toc_exp_to_c(nv,_tmp660);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp662) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp661))
_tmp660->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp660->r,0));};}
# 3710
e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp47E);}
# 3712
_npop_handler(0);goto _LL291;
# 3696
;_pop_region(rgn);};}case 30: _LL2D0: _tmp47A=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp47B=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_tmp47C=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp46E)->f3;_LL2D1: {
# 3715
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
# 3726
if(Cyc_Toc_is_toplevel(nv)){
# 3728
struct Cyc_List_List*dles=0;
for(0;_tmp47A != 0;(_tmp47A=_tmp47A->tl,_tmp665=_tmp665->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp47A->hd;
void*_tmp666=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp665))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp666))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);
dles=({struct Cyc_List_List*_tmp667=_cycalloc(sizeof(*_tmp667));_tmp667->hd=({struct _tuple19*_tmp668=_cycalloc(sizeof(*_tmp668));_tmp668->f1=0;_tmp668->f2=cur_e;_tmp668;});_tmp667->tl=dles;_tmp667;});}
# 3739
dles=({struct Cyc_List_List*_tmp669=_cycalloc(sizeof(*_tmp669));_tmp669->hd=({struct _tuple19*_tmp66A=_cycalloc(sizeof(*_tmp66A));_tmp66A->f1=0;_tmp66A->f2=tag_exp;_tmp66A;});_tmp669->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp669;});
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{
# 3745
struct Cyc_List_List*_tmp66B=({struct Cyc_List_List*_tmp672=_cycalloc(sizeof(*_tmp672));_tmp672->hd=
Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Toc_tag_sp,0),tag_exp,0);_tmp672->tl=0;_tmp672;});
# 3748
{int i=1;for(0;_tmp47A != 0;(((_tmp47A=_tmp47A->tl,i ++)),_tmp665=_tmp665->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp47A->hd;
void*_tmp66C=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp665))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp66C))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct Cyc_Absyn_Stmt*_tmp66D=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Absyn_fieldname(i),0),cur_e,0);
# 3758
_tmp66B=({struct Cyc_List_List*_tmp66E=_cycalloc(sizeof(*_tmp66E));_tmp66E->hd=_tmp66D;_tmp66E->tl=_tmp66B;_tmp66E;});};}}{
# 3760
struct Cyc_Absyn_Stmt*_tmp66F=Cyc_Absyn_exp_stmt(_tmp664,0);
struct Cyc_Absyn_Stmt*_tmp670=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(({struct Cyc_List_List*_tmp671=_cycalloc(sizeof(*_tmp671));_tmp671->hd=_tmp66F;_tmp671->tl=_tmp66B;_tmp671;})),0);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp663,datatype_ctype,0,_tmp670,0));};}
# 3764
goto _LL291;};}case 31: _LL2D2: _LL2D3:
# 3766
 goto _LL2D5;case 32: _LL2D4: _LL2D5:
 goto _LL291;case 33: _LL2D6: _tmp474=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46E)->f1).is_calloc;_tmp475=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46E)->f1).rgn;_tmp476=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46E)->f1).elt_type;_tmp477=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46E)->f1).num_elts;_tmp478=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46E)->f1).fat_result;_tmp479=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46E)->f1).inline_call;_LL2D7: {
# 3770
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp476)));
Cyc_Toc_exp_to_c(nv,_tmp477);
# 3774
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
# 3785
pexp=Cyc_Toc_calloc_exp(*_tmp476,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp673,0));}
# 3787
rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*_tmp675[3];_tmp675[2]=
# 3789
Cyc_Absyn_var_exp(_tmp673,0);_tmp675[1]=
# 3788
Cyc_Absyn_sizeoftyp_exp(t_c,0);_tmp675[0]=Cyc_Absyn_var_exp(_tmp674,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp675,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}else{
# 3791
if(_tmp475 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp475;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp479)
pexp=Cyc_Toc_rmalloc_inline_exp(rgn,Cyc_Absyn_var_exp(_tmp673,0));else{
# 3797
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp673,0));}}else{
# 3799
pexp=Cyc_Toc_malloc_exp(*_tmp476,Cyc_Absyn_var_exp(_tmp673,0));}
# 3801
rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*_tmp676[3];_tmp676[2]=
Cyc_Absyn_var_exp(_tmp673,0);_tmp676[1]=
# 3801
Cyc_Absyn_uint_exp(1,0);_tmp676[0]=Cyc_Absyn_var_exp(_tmp674,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp676,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}{
# 3804
struct Cyc_Absyn_Stmt*_tmp677=Cyc_Absyn_declare_stmt(_tmp673,Cyc_Absyn_uint_typ,_tmp477,
Cyc_Absyn_declare_stmt(_tmp674,Cyc_Absyn_cstar_typ(t_c,Cyc_Toc_mt_tq),pexp,
Cyc_Absyn_exp_stmt(rexp,0),0),0);
e->r=Cyc_Toc_stmt_exp_r(_tmp677);};}else{
# 3809
if(_tmp474){
if(_tmp475 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp475;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp477))->r;}else{
# 3815
e->r=(Cyc_Toc_calloc_exp(*_tmp476,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp477))->r;}}else{
# 3818
if(_tmp475 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp475;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp479)
e->r=(Cyc_Toc_rmalloc_inline_exp(rgn,_tmp477))->r;else{
# 3824
e->r=(Cyc_Toc_rmalloc_exp(rgn,_tmp477))->r;}}else{
# 3826
e->r=(Cyc_Toc_malloc_exp(*_tmp476,_tmp477))->r;}}}
# 3830
goto _LL291;}case 34: _LL2D8: _tmp472=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp473=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL2D9: {
# 3839
void*e1_old_typ=(void*)_check_null(_tmp472->topt);
void*e2_old_typ=(void*)_check_null(_tmp473->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ) && !Cyc_Tcutil_is_pointer_type(e1_old_typ))
({void*_tmp678=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp679="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";_tag_dyneither(_tmp679,sizeof(char),57);}),_tag_dyneither(_tmp678,sizeof(void*),0));});{
# 3845
struct Cyc_Absyn_Exp*swap_fn;
if(Cyc_Tcutil_is_dyneither_ptr(e1_old_typ))
swap_fn=Cyc_Toc__swap_dyneither_e;else{
# 3849
swap_fn=Cyc_Toc__swap_word_e;}{
# 3853
int f1_tag=0;
void*tagged_mem_type1=(void*)& Cyc_Absyn_VoidType_val;
int e1_tmem=Cyc_Toc_is_tagged_union_project(_tmp472,& f1_tag,& tagged_mem_type1,1);
int f2_tag=0;
void*tagged_mem_type2=(void*)& Cyc_Absyn_VoidType_val;
int e2_tmem=Cyc_Toc_is_tagged_union_project(_tmp473,& f2_tag,& tagged_mem_type2,1);
# 3860
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp472);
Cyc_Toc_exp_to_c(nv,_tmp473);
Cyc_Toc_set_lhs(nv,0);
# 3866
if(e1_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp472,f1_tag,tagged_mem_type1);else{
# 3870
_tmp472=Cyc_Toc_push_address_exp(_tmp472);}
# 3872
if(e2_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp473,f2_tag,tagged_mem_type2);else{
# 3876
_tmp473=Cyc_Toc_push_address_exp(_tmp473);}
# 3878
e->r=Cyc_Toc_fncall_exp_r(swap_fn,({struct Cyc_Absyn_Exp*_tmp67A[2];_tmp67A[1]=_tmp473;_tmp67A[0]=_tmp472;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp67A,sizeof(struct Cyc_Absyn_Exp*),2));}));
# 3880
goto _LL291;};};}case 37: _LL2DA: _tmp470=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp471=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL2DB: {
# 3883
void*_tmp67B=Cyc_Tcutil_compress((void*)_check_null(_tmp470->topt));
Cyc_Toc_exp_to_c(nv,_tmp470);
{void*_tmp67C=_tmp67B;struct Cyc_Absyn_Aggrdecl*_tmp67D;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp67C)->tag == 11){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp67C)->f1).aggr_info).KnownAggr).tag == 2){_LL3CB: _tmp67D=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp67C)->f1).aggr_info).KnownAggr).val;_LL3CC: {
# 3887
struct Cyc_Absyn_Exp*_tmp67E=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp67D,_tmp471),0);
struct Cyc_Absyn_Exp*_tmp67F=Cyc_Toc_member_exp(_tmp470,_tmp471,0);
struct Cyc_Absyn_Exp*_tmp680=Cyc_Toc_member_exp(_tmp67F,Cyc_Toc_tag_sp,0);
e->r=(Cyc_Absyn_eq_exp(_tmp680,_tmp67E,0))->r;
goto _LL3CA;}}else{goto _LL3CD;}}else{_LL3CD: _LL3CE:
({void*_tmp681=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp682="non-aggregate type in tagcheck";_tag_dyneither(_tmp682,sizeof(char),31);}),_tag_dyneither(_tmp681,sizeof(void*),0));});}_LL3CA:;}
# 3894
goto _LL291;}case 36: _LL2DC: _tmp46F=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_LL2DD:
 Cyc_Toc_stmt_to_c(nv,_tmp46F);goto _LL291;case 35: _LL2DE: _LL2DF:
({void*_tmp683=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp684="UnresolvedMem";_tag_dyneither(_tmp684,sizeof(char),14);}),_tag_dyneither(_tmp683,sizeof(void*),0));});case 24: _LL2E0: _LL2E1:
({void*_tmp685=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp686="compoundlit";_tag_dyneither(_tmp686,sizeof(char),12);}),_tag_dyneither(_tmp685,sizeof(void*),0));});case 38: _LL2E2: _LL2E3:
({void*_tmp687=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp688="valueof(-)";_tag_dyneither(_tmp688,sizeof(char),11);}),_tag_dyneither(_tmp687,sizeof(void*),0));});default: _LL2E4: _LL2E5:
({void*_tmp689=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp68A="__asm__";_tag_dyneither(_tmp68A,sizeof(char),8);}),_tag_dyneither(_tmp689,sizeof(void*),0));});}_LL291:;};}struct _tuple31{int f1;struct _dyneither_ptr*f2;struct _dyneither_ptr*f3;struct Cyc_Absyn_Switch_clause*f4;};
# 3927 "toc.cyc"
static struct _tuple31*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 3929
return({struct _tuple31*_tmp68B=_region_malloc(r,sizeof(*_tmp68B));_tmp68B->f1=0;_tmp68B->f2=Cyc_Toc_fresh_label();_tmp68B->f3=Cyc_Toc_fresh_label();_tmp68B->f4=sc;_tmp68B;});}
# 3934
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(0;ps != 0;ps=((struct Cyc_List_List*)_check_null(ps))->tl){
struct Cyc_Tcpat_PathNode*_tmp68C=(struct Cyc_Tcpat_PathNode*)ps->hd;
# 3940
if((int)(((_tmp68C->orig_pat).pattern).tag == 1)){
void*t=(void*)_check_null(({union Cyc_Tcpat_PatOrWhere _tmp68F=_tmp68C->orig_pat;if((_tmp68F.pattern).tag != 1)_throw_match();(_tmp68F.pattern).val;})->topt);
void*_tmp68D=Cyc_Tcutil_compress(Cyc_Toc_typ_to_c_array(t));
void*_tmp68E=_tmp68D;switch(*((int*)_tmp68E)){case 0: _LL3D0: _LL3D1:
# 3945
 goto _LL3D3;case 11: _LL3D2: _LL3D3:
 goto _LL3D5;case 12: _LL3D4: _LL3D5:
 goto _LL3CF;default: _LL3D6: _LL3D7:
 v=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c_array(t),v);goto _LL3CF;}_LL3CF:;}{
# 3951
void*_tmp690=_tmp68C->access;void*_tmp691=_tmp690;struct Cyc_Absyn_Datatypedecl*_tmp692;struct Cyc_Absyn_Datatypefield*_tmp693;unsigned int _tmp694;int _tmp695;struct _dyneither_ptr*_tmp696;unsigned int _tmp697;switch(*((int*)_tmp691)){case 0: _LL3D9: _LL3DA:
# 3956
 goto _LL3D8;case 1: _LL3DB: _LL3DC:
# 3961
 if(ps->tl != 0){
void*_tmp698=((struct Cyc_Tcpat_PathNode*)((struct Cyc_List_List*)_check_null(ps->tl))->hd)->access;void*_tmp699=_tmp698;struct Cyc_Absyn_Datatypedecl*_tmp69A;struct Cyc_Absyn_Datatypefield*_tmp69B;unsigned int _tmp69C;if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp699)->tag == 3){_LL3E4: _tmp69A=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp699)->f1;_tmp69B=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp699)->f2;_tmp69C=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp699)->f3;_LL3E5:
# 3964
 ps=ps->tl;
v=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp69B->name,_tmp69A->name)),Cyc_Toc_mt_tq),v);
v=Cyc_Absyn_aggrarrow_exp(v,Cyc_Absyn_fieldname((int)(_tmp69C + 1)),0);
continue;}else{_LL3E6: _LL3E7:
# 3969
 goto _LL3E3;}_LL3E3:;}
# 3972
v=Cyc_Absyn_deref_exp(v,0);
goto _LL3D8;case 2: _LL3DD: _tmp697=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp691)->f1;_LL3DE:
 v=Cyc_Toc_member_exp(v,Cyc_Absyn_fieldname((int)(_tmp697 + 1)),0);goto _LL3D8;case 4: _LL3DF: _tmp695=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp691)->f1;_tmp696=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp691)->f2;_LL3E0:
# 3976
 v=Cyc_Toc_member_exp(v,_tmp696,0);
if(_tmp695)
v=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0);
goto _LL3D8;default: _LL3E1: _tmp692=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp691)->f1;_tmp693=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp691)->f2;_tmp694=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp691)->f3;_LL3E2:
# 3981
 v=Cyc_Toc_member_exp(v,Cyc_Absyn_fieldname((int)(_tmp694 + 1)),0);
goto _LL3D8;}_LL3D8:;};}
# 3985
return v;}
# 3990
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
void*_tmp69D=t;struct Cyc_Absyn_Datatypedecl*_tmp69E;struct Cyc_Absyn_Datatypefield*_tmp69F;struct _dyneither_ptr*_tmp6A0;int _tmp6A1;int _tmp6A2;struct Cyc_Absyn_Datatypedecl*_tmp6A3;struct Cyc_Absyn_Datatypefield*_tmp6A4;unsigned int _tmp6A5;struct _dyneither_ptr _tmp6A6;int _tmp6A7;void*_tmp6A8;struct Cyc_Absyn_Enumfield*_tmp6A9;struct Cyc_Absyn_Enumdecl*_tmp6AA;struct Cyc_Absyn_Enumfield*_tmp6AB;struct Cyc_Absyn_Exp*_tmp6AC;switch(*((int*)_tmp69D)){case 0: _LL3E9: _tmp6AC=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp69D)->f1;_LL3EA:
# 3994
 if(_tmp6AC == 0)return v;else{return _tmp6AC;}case 1: _LL3EB: _LL3EC:
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_signed_int_exp(0,0),0);case 2: _LL3ED: _LL3EE:
 return Cyc_Absyn_neq_exp(v,Cyc_Absyn_signed_int_exp(0,0),0);case 3: _LL3EF: _tmp6AA=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp69D)->f1;_tmp6AB=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp69D)->f2;_LL3F0:
# 3998
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp6AD=_cycalloc(sizeof(*_tmp6AD));_tmp6AD[0]=({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmp6AE;_tmp6AE.tag=31;_tmp6AE.f1=_tmp6AB->name;_tmp6AE.f2=_tmp6AA;_tmp6AE.f3=_tmp6AB;_tmp6AE;});_tmp6AD;}),0),0);case 4: _LL3F1: _tmp6A8=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp69D)->f1;_tmp6A9=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp69D)->f2;_LL3F2:
# 4000
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp6AF=_cycalloc(sizeof(*_tmp6AF));_tmp6AF[0]=({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp6B0;_tmp6B0.tag=32;_tmp6B0.f1=_tmp6A9->name;_tmp6B0.f2=_tmp6A8;_tmp6B0.f3=_tmp6A9;_tmp6B0;});_tmp6AF;}),0),0);case 5: _LL3F3: _tmp6A6=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp69D)->f1;_tmp6A7=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp69D)->f2;_LL3F4:
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_float_exp(_tmp6A6,_tmp6A7,0),0);case 6: _LL3F5: _tmp6A5=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp69D)->f1;_LL3F6:
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_uint_exp(_tmp6A5,0),0);case 7: _LL3F7: _tmp6A2=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp69D)->f1;_tmp6A3=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp69D)->f2;_tmp6A4=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp69D)->f3;_LL3F8:
# 4006
 LOOP1: {
void*_tmp6B1=v->r;void*_tmp6B2=_tmp6B1;struct Cyc_Absyn_Exp*_tmp6B3;struct Cyc_Absyn_Exp*_tmp6B4;switch(*((int*)_tmp6B2)){case 13: _LL3FE: _tmp6B4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6B2)->f2;_LL3FF:
 v=_tmp6B4;goto LOOP1;case 19: _LL400: _tmp6B3=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6B2)->f1;_LL401:
 v=_tmp6B3;goto _LL3FD;default: _LL402: _LL403:
 goto _LL3FD;}_LL3FD:;}
# 4013
v=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp6A4->name,_tmp6A3->name)),Cyc_Toc_mt_tq),v);
return Cyc_Absyn_eq_exp(Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0),Cyc_Absyn_uint_exp((unsigned int)_tmp6A2,0),0);case 8: _LL3F9: _tmp6A0=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp69D)->f1;_tmp6A1=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp69D)->f2;_LL3FA:
# 4016
 v=Cyc_Toc_member_exp(Cyc_Toc_member_exp(v,_tmp6A0,0),Cyc_Toc_tag_sp,0);
return Cyc_Absyn_eq_exp(v,Cyc_Absyn_uint_exp((unsigned int)_tmp6A1,0),0);default: _LL3FB: _tmp69E=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp69D)->f1;_tmp69F=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp69D)->f2;_LL3FC:
# 4020
 LOOP2: {
void*_tmp6B5=v->r;void*_tmp6B6=_tmp6B5;struct Cyc_Absyn_Exp*_tmp6B7;struct Cyc_Absyn_Exp*_tmp6B8;switch(*((int*)_tmp6B6)){case 13: _LL405: _tmp6B8=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6B6)->f2;_LL406:
 v=_tmp6B8;goto LOOP2;case 19: _LL407: _tmp6B7=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6B6)->f1;_LL408:
 v=_tmp6B7;goto _LL404;default: _LL409: _LL40A:
 goto _LL404;}_LL404:;}
# 4027
v=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp69F->name,_tmp69E->name)),Cyc_Toc_mt_tq),v);
return Cyc_Absyn_eq_exp(Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0),Cyc_Absyn_var_exp(_tmp69F->name,0),0);}_LL3E8:;}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _dyneither_ptr*f1;};
# 4039
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1};struct _tuple32{int f1;void*f2;};
# 4047
static struct _tuple32 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 4049
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(0;ss != 0;(ss=ss->tl,c=c + 1)){
struct _tuple29 _tmp6BB=*((struct _tuple29*)ss->hd);struct _tuple29 _tmp6BC=_tmp6BB;void*_tmp6BD;_LL40C: _tmp6BD=_tmp6BC.f1;_LL40D:;{
void*_tmp6BE=_tmp6BD;struct Cyc_Absyn_Exp*_tmp6BF;struct _dyneither_ptr*_tmp6C0;switch(*((int*)_tmp6BE)){case 3: _LL40F: _LL410:
# 4055
 goto _LL412;case 4: _LL411: _LL412:
 goto _LL414;case 6: _LL413: _LL414:
# 4058
 continue;case 8: _LL415: _tmp6C0=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp6BE)->f1;_LL416:
# 4060
 if(k == (void*)& Cyc_Toc_OtherTest_val)
k=(void*)({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_tmp6C1=_cycalloc(sizeof(*_tmp6C1));_tmp6C1[0]=({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct _tmp6C2;_tmp6C2.tag=3;_tmp6C2.f1=_tmp6C0;_tmp6C2;});_tmp6C1;});
continue;case 7: _LL417: _LL418:
# 4064
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;
continue;case 0: _LL419: _tmp6BF=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp6BE)->f1;if(_tmp6BF != 0){_LL41A:
# 4068
 k=(void*)({struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_tmp6C3=_cycalloc(sizeof(*_tmp6C3));_tmp6C3[0]=({struct Cyc_Toc_WhereTest_Toc_TestKind_struct _tmp6C4;_tmp6C4.tag=2;_tmp6C4.f1=_tmp6BF;_tmp6C4;});_tmp6C3;});
return({struct _tuple32 _tmp6C5;_tmp6C5.f1=0;_tmp6C5.f2=k;_tmp6C5;});}else{_LL41B: _LL41C:
 goto _LL41E;}case 1: _LL41D: _LL41E:
 goto _LL420;case 2: _LL41F: _LL420:
 goto _LL422;case 5: _LL421: _LL422:
 goto _LL424;default: _LL423: _LL424:
 return({struct _tuple32 _tmp6C6;_tmp6C6.f1=0;_tmp6C6.f2=k;_tmp6C6;});}_LL40E:;};}
# 4077
return({struct _tuple32 _tmp6C7;_tmp6C7.f1=c;_tmp6C7.f2=k;_tmp6C7;});}
# 4082
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{void*_tmp6C8=p;int _tmp6C9;int _tmp6CA;unsigned int _tmp6CB;void*_tmp6CC;struct Cyc_Absyn_Enumfield*_tmp6CD;struct Cyc_Absyn_Enumdecl*_tmp6CE;struct Cyc_Absyn_Enumfield*_tmp6CF;switch(*((int*)_tmp6C8)){case 3: _LL426: _tmp6CE=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp6C8)->f1;_tmp6CF=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp6C8)->f2;_LL427:
# 4086
 e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp6D0=_cycalloc(sizeof(*_tmp6D0));_tmp6D0[0]=({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmp6D1;_tmp6D1.tag=31;_tmp6D1.f1=_tmp6CF->name;_tmp6D1.f2=_tmp6CE;_tmp6D1.f3=_tmp6CF;_tmp6D1;});_tmp6D0;}),0);goto _LL425;case 4: _LL428: _tmp6CC=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp6C8)->f1;_tmp6CD=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp6C8)->f2;_LL429:
# 4088
 e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp6D2=_cycalloc(sizeof(*_tmp6D2));_tmp6D2[0]=({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp6D3;_tmp6D3.tag=32;_tmp6D3.f1=_tmp6CD->name;_tmp6D3.f2=_tmp6CC;_tmp6D3.f3=_tmp6CD;_tmp6D3;});_tmp6D2;}),0);goto _LL425;case 6: _LL42A: _tmp6CB=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp6C8)->f1;_LL42B:
 _tmp6CA=(int)_tmp6CB;goto _LL42D;case 7: _LL42C: _tmp6CA=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp6C8)->f1;_LL42D:
 _tmp6C9=_tmp6CA;goto _LL42F;case 8: _LL42E: _tmp6C9=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp6C8)->f2;_LL42F:
# 4092
 e=Cyc_Absyn_uint_exp((unsigned int)_tmp6C9,0);
goto _LL425;default: _LL430: _LL431:
({void*_tmp6D4=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp6D5="compile_pat_test_as_case!";_tag_dyneither(_tmp6D5,sizeof(char),26);}),_tag_dyneither(_tmp6D4,sizeof(void*),0));});}_LL425:;}
# 4096
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp6D6=_cycalloc(sizeof(*_tmp6D6));_tmp6D6[0]=({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct _tmp6D7;_tmp6D7.tag=17;_tmp6D7.f1=e;_tmp6D7;});_tmp6D6;}),0);}
# 4100
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0);}struct _tuple33{struct _tuple1*f1;void*f2;};struct _tuple34{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 4109
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**decls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 4112
void*t=(void*)_check_null(p->topt);
void*_tmp6D8=p->r;void*_tmp6D9=_tmp6D8;union Cyc_Absyn_AggrInfoU _tmp6DA;struct Cyc_List_List*_tmp6DB;struct Cyc_List_List*_tmp6DC;struct Cyc_List_List*_tmp6DD;struct Cyc_Absyn_Pat*_tmp6DE;struct Cyc_Absyn_Datatypedecl*_tmp6DF;struct Cyc_Absyn_Datatypefield*_tmp6E0;struct Cyc_List_List*_tmp6E1;struct Cyc_Absyn_Vardecl*_tmp6E2;struct Cyc_Absyn_Pat*_tmp6E3;struct _tuple1*_tmp6E4;struct _tuple1*_tmp6E5;struct Cyc_Absyn_Pat*_tmp6E6;struct _tuple1*_tmp6E7;switch(*((int*)_tmp6D9)){case 2: _LL433: _tmp6E7=(((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp6D9)->f2)->name;_LL434: {
# 4115
struct Cyc_Absyn_Pat*_tmp6E8=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0);
_tmp6E8->topt=p->topt;
_tmp6E5=_tmp6E7;_tmp6E6=_tmp6E8;goto _LL436;}case 1: _LL435: _tmp6E5=(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp6D9)->f1)->name;_tmp6E6=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp6D9)->f2;_LL436: {
# 4120
struct _tuple1*v=Cyc_Toc_temp_var();
void*_tmp6E9=(void*)_check_null(_tmp6E6->topt);
*decls=({struct Cyc_List_List*_tmp6EA=_region_malloc(rgn,sizeof(*_tmp6EA));_tmp6EA->hd=({struct _tuple33*_tmp6EB=_region_malloc(rgn,sizeof(*_tmp6EB));_tmp6EB->f1=v;_tmp6EB->f2=Cyc_Toc_typ_to_c_array(_tmp6E9);_tmp6EB;});_tmp6EA->tl=*decls;_tmp6EA;});
*nv=Cyc_Toc_add_varmap(rgn,*nv,_tmp6E5,Cyc_Absyn_var_exp(v,0));{
struct Cyc_Absyn_Stmt*_tmp6EC=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(v,0),Cyc_Absyn_copy_exp(path),0);
return Cyc_Toc_seq_stmt_opt(_tmp6EC,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp6E6));};}case 4: _LL437: _tmp6E4=(((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp6D9)->f2)->name;_LL438: {
# 4128
struct _tuple1*_tmp6ED=Cyc_Toc_temp_var();
void*_tmp6EE=(void*)_check_null(p->topt);
*decls=({struct Cyc_List_List*_tmp6EF=_region_malloc(rgn,sizeof(*_tmp6EF));_tmp6EF->hd=({struct _tuple33*_tmp6F0=_region_malloc(rgn,sizeof(*_tmp6F0));_tmp6F0->f1=_tmp6ED;_tmp6F0->f2=Cyc_Toc_typ_to_c_array(_tmp6EE);_tmp6F0;});_tmp6EF->tl=*decls;_tmp6EF;});
*nv=Cyc_Toc_add_varmap(rgn,*nv,_tmp6E4,Cyc_Absyn_var_exp(_tmp6ED,0));
return Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp6ED,0),Cyc_Absyn_copy_exp(path),0);}case 0: _LL439: _LL43A:
 return 0;case 3: _LL43B: _tmp6E2=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp6D9)->f1;_tmp6E3=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp6D9)->f2;_LL43C: {
# 4136
struct _tuple1*_tmp6F1=Cyc_Toc_temp_var();
*decls=({struct Cyc_List_List*_tmp6F2=_region_malloc(rgn,sizeof(*_tmp6F2));_tmp6F2->hd=({struct _tuple33*_tmp6F3=_region_malloc(rgn,sizeof(*_tmp6F3));_tmp6F3->f1=_tmp6F1;_tmp6F3->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq);_tmp6F3;});_tmp6F2->tl=*decls;_tmp6F2;});
*nv=Cyc_Toc_add_varmap(rgn,*nv,_tmp6E2->name,Cyc_Absyn_var_exp(_tmp6F1,0));{
# 4140
struct Cyc_Absyn_Stmt*_tmp6F4=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp6F1,0),
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path))),0);
return Cyc_Toc_seq_stmt_opt(_tmp6F4,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp6E3));};}case 9: _LL43D: _LL43E:
# 4145
 goto _LL440;case 10: _LL43F: _LL440:
 goto _LL442;case 11: _LL441: _LL442:
 goto _LL444;case 12: _LL443: _LL444:
 goto _LL446;case 13: _LL445: _LL446:
 goto _LL448;case 14: _LL447: _LL448:
 return 0;case 6: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6D9)->f1)->r)->tag == 8){_LL449: _tmp6DF=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6D9)->f1)->r)->f1;_tmp6E0=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6D9)->f1)->r)->f2;_tmp6E1=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6D9)->f1)->r)->f3;_LL44A:
# 4154
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
# 4172
return s;};};}else{_LL453: _tmp6DE=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6D9)->f1;_LL454:
# 4221
 return Cyc_Toc_extract_pattern_vars(rgn,nv,decls,Cyc_Absyn_deref_exp(path,0),_tmp6DE);}case 8: _LL44B: _tmp6DD=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp6D9)->f3;_LL44C:
# 4174
 _tmp6DC=_tmp6DD;goto _LL44E;case 5: _LL44D: _tmp6DC=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp6D9)->f1;_LL44E: {
# 4176
struct Cyc_Absyn_Stmt*s=0;
int cnt=1;
for(0;_tmp6DC != 0;(_tmp6DC=_tmp6DC->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp6FC=(struct Cyc_Absyn_Pat*)_tmp6DC->hd;
if(_tmp6FC->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp6FD=(void*)_check_null(_tmp6FC->topt);
struct _dyneither_ptr*_tmp6FE=Cyc_Absyn_fieldname(cnt);
s=Cyc_Toc_seq_stmt_opt(s,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,Cyc_Toc_member_exp(path,_tmp6FE,0),_tmp6FC));};}
# 4186
return s;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp6D9)->f1 == 0){_LL44F: _LL450:
({void*_tmp6FF=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp700="unresolved aggregate pattern!";_tag_dyneither(_tmp700,sizeof(char),30);}),_tag_dyneither(_tmp6FF,sizeof(void*),0));});}else{_LL451: _tmp6DA=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp6D9)->f1)->aggr_info;_tmp6DB=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp6D9)->f3;_LL452: {
# 4190
struct Cyc_Absyn_Aggrdecl*_tmp701=Cyc_Absyn_get_known_aggrdecl(_tmp6DA);
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp6DB != 0;_tmp6DB=_tmp6DB->tl){
struct _tuple34*_tmp702=(struct _tuple34*)_tmp6DB->hd;
struct Cyc_Absyn_Pat*_tmp703=(*_tmp702).f2;
if(_tmp703->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*f;
{void*_tmp704=(void*)((struct Cyc_List_List*)_check_null((*_tmp702).f1))->hd;void*_tmp705=_tmp704;struct _dyneither_ptr*_tmp706;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp705)->tag == 1){_LL45C: _tmp706=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp705)->f1;_LL45D:
 f=_tmp706;goto _LL45B;}else{_LL45E: _LL45F:
(int)_throw((void*)({struct Cyc_Toc_Match_error_exn_struct*_tmp707=_cycalloc_atomic(sizeof(*_tmp707));_tmp707[0]=({struct Cyc_Toc_Match_error_exn_struct _tmp708;_tmp708.tag=Cyc_Toc_Match_error;_tmp708;});_tmp707;}));}_LL45B:;}{
# 4202
void*_tmp709=(void*)_check_null(_tmp703->topt);
void*_tmp70A=Cyc_Toc_typ_to_c_array(_tmp709);
struct Cyc_Absyn_Exp*_tmp70B=Cyc_Toc_member_exp(path,f,0);
# 4206
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp701->impl))->tagged)_tmp70B=Cyc_Toc_member_exp(_tmp70B,Cyc_Toc_val_sp,0);{
void*_tmp70C=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp701->impl))->fields,f)))->type;
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp70C))
_tmp70B=Cyc_Toc_cast_it(_tmp70A,_tmp70B);else{
if(!Cyc_Toc_is_array_type(_tmp70C) && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp70C)))
# 4213
_tmp70B=Cyc_Absyn_deref_exp(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(_tmp70A,Cyc_Toc_mt_tq),
Cyc_Absyn_address_exp(_tmp70B,0)),0);}
# 4216
s=Cyc_Toc_seq_stmt_opt(s,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp70B,_tmp703));};};};}
# 4218
return s;}}case 15: _LL455: _LL456:
# 4223
({void*_tmp70D=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp70E="unknownid pat";_tag_dyneither(_tmp70E,sizeof(char),14);}),_tag_dyneither(_tmp70D,sizeof(void*),0));});case 16: _LL457: _LL458:
({void*_tmp70F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp710="unknowncall pat";_tag_dyneither(_tmp710,sizeof(char),16);}),_tag_dyneither(_tmp70F,sizeof(void*),0));});default: _LL459: _LL45A:
({void*_tmp711=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp712="exp pat";_tag_dyneither(_tmp712,sizeof(char),8);}),_tag_dyneither(_tmp711,sizeof(void*),0));});}_LL432:;}struct _tuple35{struct Cyc_Toc_Env*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Stmt*f3;};
# 4234
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 4242
void*_tmp713=dopt;struct Cyc_List_List*_tmp714;struct Cyc_List_List*_tmp715;void*_tmp716;struct Cyc_Tcpat_Rhs*_tmp717;if(_tmp713 == 0){_LL461: _LL462:
# 4244
 return Cyc_Absyn_skip_stmt(0);}else{switch(*((int*)_tmp713)){case 0: _LL463: _LL464:
 return Cyc_Toc_throw_match_stmt();case 1: _LL465: _tmp717=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp713)->f1;_LL466:
# 4249
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple31*_tmp718=(struct _tuple31*)lscs->hd;struct _tuple31*_tmp719=_tmp718;int*_tmp71A;struct _dyneither_ptr*_tmp71B;struct _dyneither_ptr*_tmp71C;struct Cyc_Absyn_Switch_clause*_tmp71D;_LL46A: _tmp71A=(int*)& _tmp719->f1;_tmp71B=_tmp719->f2;_tmp71C=_tmp719->f3;_tmp71D=_tmp719->f4;_LL46B:;{
struct Cyc_Absyn_Stmt*_tmp71E=_tmp71D->body;
if(_tmp71E == _tmp717->rhs){
# 4254
if(*_tmp71A)
return Cyc_Absyn_goto_stmt(_tmp71B,0);
*_tmp71A=1;{
# 4258
struct Cyc_Absyn_Stmt*init_opt=Cyc_Toc_extract_pattern_vars(rgn,& nv,decls,Cyc_Absyn_var_exp(v,0),_tmp71D->pattern);
# 4261
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_label_stmt(_tmp71C,_tmp71D->body,0);
if(init_opt != 0)
res=Cyc_Absyn_seq_stmt(init_opt,res,0);
res=Cyc_Absyn_label_stmt(_tmp71B,res,0);
*bodies=({struct Cyc_List_List*_tmp71F=_region_malloc(rgn,sizeof(*_tmp71F));_tmp71F->hd=({struct _tuple35*_tmp720=_region_malloc(rgn,sizeof(*_tmp720));_tmp720->f1=nv;_tmp720->f2=_tmp71C;_tmp720->f3=_tmp71E;_tmp720;});_tmp71F->tl=*bodies;_tmp71F;});
return res;};}};}
# 4269
({void*_tmp721=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp722="couldn't find rhs!";_tag_dyneither(_tmp722,sizeof(char),19);}),_tag_dyneither(_tmp721,sizeof(void*),0));});default: _LL467: _tmp714=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp713)->f1;_tmp715=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp713)->f2;_tmp716=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp713)->f3;_LL468: {
# 4272
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp716,lscs,v);
# 4274
struct Cyc_Absyn_Exp*_tmp723=Cyc_Toc_compile_path(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp714),Cyc_Absyn_var_exp(v,0));
struct Cyc_List_List*_tmp724=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp715);
# 4277
struct _tuple32 _tmp725=Cyc_Toc_admits_switch(_tmp724);struct _tuple32 _tmp726=_tmp725;int _tmp727;void*_tmp728;_LL46D: _tmp727=_tmp726.f1;_tmp728=_tmp726.f2;_LL46E:;
if(_tmp727 > 1){
# 4281
struct Cyc_List_List*new_lscs=({struct Cyc_List_List*_tmp739=_cycalloc(sizeof(*_tmp739));_tmp739->hd=({struct Cyc_Absyn_Switch_clause*_tmp73A=_cycalloc(sizeof(*_tmp73A));_tmp73A->pattern=
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0);_tmp73A->pat_vars=0;_tmp73A->where_clause=0;_tmp73A->body=res;_tmp73A->loc=0;_tmp73A;});_tmp739->tl=0;_tmp739;});
# 4284
for(0;_tmp724 != 0;_tmp724=_tmp724->tl){
struct _tuple29 _tmp729=*((struct _tuple29*)_tmp724->hd);struct _tuple29 _tmp72A=_tmp729;void*_tmp72B;void*_tmp72C;_LL470: _tmp72B=_tmp72A.f1;_tmp72C=_tmp72A.f2;_LL471:;{
# 4287
struct Cyc_Absyn_Pat*_tmp72D=Cyc_Toc_compile_pat_test_as_case(_tmp72B);
# 4289
struct Cyc_Absyn_Stmt*_tmp72E=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp72C,lscs,v);
# 4291
new_lscs=({struct Cyc_List_List*_tmp72F=_cycalloc(sizeof(*_tmp72F));_tmp72F->hd=({struct Cyc_Absyn_Switch_clause*_tmp730=_cycalloc(sizeof(*_tmp730));_tmp730->pattern=_tmp72D;_tmp730->pat_vars=0;_tmp730->where_clause=0;_tmp730->body=_tmp72E;_tmp730->loc=0;_tmp730;});_tmp72F->tl=new_lscs;_tmp72F;});};}
# 4293
{void*_tmp731=_tmp728;struct _dyneither_ptr*_tmp732;switch(*((int*)_tmp731)){case 1: _LL473: _LL474:
# 4296
 LOOP1: {
void*_tmp733=_tmp723->r;void*_tmp734=_tmp733;struct Cyc_Absyn_Exp*_tmp735;struct Cyc_Absyn_Exp*_tmp736;switch(*((int*)_tmp734)){case 13: _LL47C: _tmp736=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp734)->f2;_LL47D:
 _tmp723=_tmp736;goto LOOP1;case 19: _LL47E: _tmp735=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp734)->f1;_LL47F:
 _tmp723=_tmp735;goto _LL47B;default: _LL480: _LL481:
 goto _LL47B;}_LL47B:;}
# 4302
_tmp723=Cyc_Absyn_deref_exp(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,Cyc_Toc_mt_tq),_tmp723),0);goto _LL472;case 3: _LL475: _tmp732=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_tmp731)->f1;_LL476:
# 4305
 _tmp723=Cyc_Toc_member_exp(Cyc_Toc_member_exp(_tmp723,_tmp732,0),Cyc_Toc_tag_sp,0);goto _LL472;case 2: _LL477: _LL478:
# 4307
 goto _LL472;default: _LL479: _LL47A:
 goto _LL472;}_LL472:;}
# 4310
res=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp737=_cycalloc(sizeof(*_tmp737));_tmp737[0]=({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct _tmp738;_tmp738.tag=10;_tmp738.f1=_tmp723;_tmp738.f2=new_lscs;_tmp738.f3=0;_tmp738;});_tmp737;}),0);}else{
# 4314
void*_tmp73B=_tmp728;struct Cyc_Absyn_Exp*_tmp73C;if(((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp73B)->tag == 2){_LL483: _tmp73C=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp73B)->f1;_LL484: {
# 4316
struct Cyc_List_List*_tmp73D=_tmp724;void*_tmp73E;struct Cyc_Tcpat_Rhs*_tmp73F;if(_tmp73D != 0){if(((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple29*)((struct Cyc_List_List*)_tmp73D)->hd)->f2)->tag == 1){if(((struct Cyc_List_List*)_tmp73D)->tl == 0){_LL488: _tmp73E=((struct _tuple29*)_tmp73D->hd)->f1;_tmp73F=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple29*)_tmp73D->hd)->f2)->f1;_LL489:
# 4322
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple31*_tmp740=(struct _tuple31*)lscs->hd;struct _tuple31*_tmp741=_tmp740;int*_tmp742;struct _dyneither_ptr*_tmp743;struct _dyneither_ptr*_tmp744;struct Cyc_Absyn_Switch_clause*_tmp745;_LL48D: _tmp742=(int*)& _tmp741->f1;_tmp743=_tmp741->f2;_tmp744=_tmp741->f3;_tmp745=_tmp741->f4;_LL48E:;{
struct Cyc_Absyn_Stmt*_tmp746=_tmp745->body;
if(_tmp746 == _tmp73F->rhs){
# 4327
if(*_tmp742)
return Cyc_Absyn_goto_stmt(_tmp743,0);
*_tmp742=1;{
# 4331
struct Cyc_Absyn_Stmt*init_opt=Cyc_Toc_extract_pattern_vars(rgn,& nv,decls,Cyc_Absyn_var_exp(v,0),_tmp745->pattern);
# 4334
Cyc_Toc_exp_to_c(nv,_tmp73C);{
# 4337
struct Cyc_Absyn_Stmt*r=Cyc_Absyn_label_stmt(_tmp744,_tmp745->body,0);
r=Cyc_Absyn_ifthenelse_stmt(_tmp73C,r,res,0);
if(init_opt != 0)
r=Cyc_Absyn_seq_stmt(init_opt,r,0);
r=Cyc_Absyn_label_stmt(_tmp743,r,0);
*bodies=({struct Cyc_List_List*_tmp747=_region_malloc(rgn,sizeof(*_tmp747));_tmp747->hd=({struct _tuple35*_tmp748=_region_malloc(rgn,sizeof(*_tmp748));_tmp748->f1=nv;_tmp748->f2=_tmp744;_tmp748->f3=_tmp746;_tmp748;});_tmp747->tl=*bodies;_tmp747;});
return r;};};}};}
# 4346
({void*_tmp749=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp74A="couldn't find rhs!";_tag_dyneither(_tmp74A,sizeof(char),19);}),_tag_dyneither(_tmp749,sizeof(void*),0));});}else{goto _LL48A;}}else{goto _LL48A;}}else{_LL48A: _LL48B:
({void*_tmp74B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp74C="bad where clause in match compiler";_tag_dyneither(_tmp74C,sizeof(char),35);}),_tag_dyneither(_tmp74B,sizeof(void*),0));});}_LL487:;}}else{_LL485: _LL486:
# 4351
 for(0;_tmp724 != 0;_tmp724=_tmp724->tl){
struct _tuple29 _tmp74D=*((struct _tuple29*)_tmp724->hd);struct _tuple29 _tmp74E=_tmp74D;void*_tmp74F;void*_tmp750;_LL490: _tmp74F=_tmp74E.f1;_tmp750=_tmp74E.f2;_LL491:;{
struct Cyc_Absyn_Exp*_tmp751=Cyc_Toc_compile_pat_test(_tmp723,_tmp74F);
struct Cyc_Absyn_Stmt*_tmp752=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp750,lscs,v);
res=Cyc_Absyn_ifthenelse_stmt(_tmp751,_tmp752,res,0);};}}_LL482:;}
# 4359
return res;}}}_LL460:;}
# 4369
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 4371
for(0;bodies != 0;bodies=bodies->tl){
struct _tuple35*_tmp753=(struct _tuple35*)bodies->hd;struct _tuple35*_tmp754=_tmp753;struct Cyc_Toc_Env**_tmp755;struct Cyc_Absyn_Stmt*_tmp756;_LL493: _tmp755=(struct Cyc_Toc_Env**)& _tmp754->f1;_tmp756=_tmp754->f3;_LL494:;
if(_tmp756 == s)return _tmp755;}
# 4377
return 0;}
# 4381
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 4384
void*_tmp757=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(nv,e);{
# 4387
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp758=_new_region("rgn");struct _RegionHandle*rgn=& _tmp758;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp759=Cyc_Toc_share_env(rgn,nv);
# 4394
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple31*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 4399
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp759,& mydecls,& mybodies,dopt,lscs,v);
# 4407
{struct Cyc_List_List*_tmp75A=lscs;for(0;_tmp75A != 0;_tmp75A=_tmp75A->tl){
struct _tuple31*_tmp75B=(struct _tuple31*)_tmp75A->hd;struct _tuple31*_tmp75C=_tmp75B;struct _dyneither_ptr*_tmp75D;struct Cyc_Absyn_Switch_clause*_tmp75E;_LL496: _tmp75D=_tmp75C->f3;_tmp75E=_tmp75C->f4;_LL497:;{
struct Cyc_Absyn_Stmt*s=_tmp75E->body;
# 4411
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 4414
if(envp == 0)continue;{
struct Cyc_Toc_Env*_tmp75F=*envp;
# 4417
if(_tmp75A->tl != 0){
struct _tuple31*_tmp760=(struct _tuple31*)((struct Cyc_List_List*)_check_null(_tmp75A->tl))->hd;struct _tuple31*_tmp761=_tmp760;struct _dyneither_ptr*_tmp762;struct Cyc_Absyn_Switch_clause*_tmp763;_LL499: _tmp762=_tmp761->f3;_tmp763=_tmp761->f4;_LL49A:;{
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
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(rgn,_tmp75F,end_l,_tmp762,vs,*_tmp764),s);}};}else{
# 4437
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn,_tmp75F,end_l),s);}};};}}{
# 4441
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(test_tree,Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),0),0);
# 4443
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct _tuple33*_tmp765=(struct _tuple33*)((struct Cyc_List_List*)_check_null(mydecls))->hd;struct _tuple33*_tmp766=_tmp765;struct _tuple1*_tmp767;void*_tmp768;_LL49C: _tmp767=_tmp766->f1;_tmp768=_tmp766->f2;_LL49D:;
res=Cyc_Absyn_declare_stmt(_tmp767,_tmp768,0,res,0);}
# 4448
whole_s->r=(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),e,res,0))->r;
_npop_handler(0);return;};}
# 4391
;_pop_region(rgn);};}
# 4454
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
# 4456
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 4461
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 4463
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,({struct Cyc_List_List*_tmp769=_cycalloc(sizeof(*_tmp769));_tmp769->hd=
Cyc_Absyn_uint_exp((unsigned int)(n - 1),0);_tmp769->tl=0;_tmp769;}),0),0);}
# 4467
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt(s->r,0));}
# 4472
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 4474
while(1){
void*_tmp76A=s->r;void*_tmp76B=_tmp76A;struct Cyc_Absyn_Exp*_tmp76C;struct Cyc_Absyn_Stmt*_tmp76D;struct Cyc_List_List*_tmp76E;void*_tmp76F;struct Cyc_Absyn_Stmt*_tmp770;struct Cyc_Absyn_Exp*_tmp771;struct _dyneither_ptr*_tmp772;struct Cyc_Absyn_Stmt*_tmp773;struct Cyc_Absyn_Decl*_tmp774;struct Cyc_Absyn_Stmt*_tmp775;struct Cyc_List_List*_tmp776;struct Cyc_Absyn_Switch_clause**_tmp777;struct Cyc_Absyn_Exp*_tmp778;struct Cyc_List_List*_tmp779;void*_tmp77A;struct Cyc_Absyn_Exp*_tmp77B;struct Cyc_Absyn_Exp*_tmp77C;struct Cyc_Absyn_Exp*_tmp77D;struct Cyc_Absyn_Stmt*_tmp77E;struct Cyc_Absyn_Exp*_tmp77F;struct Cyc_Absyn_Stmt*_tmp780;struct Cyc_Absyn_Exp*_tmp781;struct Cyc_Absyn_Stmt*_tmp782;struct Cyc_Absyn_Stmt*_tmp783;struct Cyc_Absyn_Exp*_tmp784;struct Cyc_Absyn_Stmt*_tmp785;struct Cyc_Absyn_Stmt*_tmp786;struct Cyc_Absyn_Exp*_tmp787;switch(*((int*)_tmp76B)){case 0: _LL49F: _LL4A0:
# 4477
 return;case 1: _LL4A1: _tmp787=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_LL4A2:
# 4479
 Cyc_Toc_exp_to_c(nv,_tmp787);
return;case 2: _LL4A3: _tmp785=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_tmp786=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp76B)->f2;_LL4A4:
# 4482
 Cyc_Toc_stmt_to_c(nv,_tmp785);
s=_tmp786;
continue;case 3: _LL4A5: _tmp784=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_LL4A6: {
# 4486
void*topt=0;
if(_tmp784 != 0){
topt=Cyc_Toc_typ_to_c((void*)_check_null(_tmp784->topt));
Cyc_Toc_exp_to_c(nv,_tmp784);}{
# 4492
int _tmp788=Cyc_Toc_get_npop(s);
if(_tmp788 > 0){
if(topt != 0){
struct _tuple1*_tmp789=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp78A=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp789,0),0);
s->r=(Cyc_Absyn_declare_stmt(_tmp789,topt,_tmp784,
Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(_tmp788),_tmp78A,0),0))->r;}else{
# 4501
Cyc_Toc_do_npop_before(_tmp788,s);}}
# 4503
return;};}case 4: _LL4A7: _tmp781=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_tmp782=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp76B)->f2;_tmp783=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp76B)->f3;_LL4A8:
# 4505
 Cyc_Toc_exp_to_c(nv,_tmp781);
Cyc_Toc_stmt_to_c(nv,_tmp782);
s=_tmp783;
continue;case 5: _LL4A9: _tmp77F=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1).f1;_tmp780=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp76B)->f2;_LL4AA:
# 4510
 Cyc_Toc_exp_to_c(nv,_tmp77F);{
struct _RegionHandle _tmp78B=_new_region("temp");struct _RegionHandle*temp=& _tmp78B;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp780);
# 4514
_npop_handler(0);return;
# 4511
;_pop_region(temp);};case 6: _LL4AB: _LL4AC: {
# 4516
struct Cyc_Toc_Env*_tmp78C=nv;struct _dyneither_ptr**_tmp78D;_LL4C2: _tmp78D=_tmp78C->break_lab;_LL4C3:;
if(_tmp78D != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp78D,0);
# 4520
Cyc_Toc_do_npop_before(Cyc_Toc_get_npop(s),s);
return;}case 7: _LL4AD: _LL4AE: {
# 4523
struct Cyc_Toc_Env*_tmp78E=nv;struct _dyneither_ptr**_tmp78F;_LL4C5: _tmp78F=_tmp78E->continue_lab;_LL4C6:;
if(_tmp78F != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp78F,0);
goto _LL4B0;}case 8: _LL4AF: _LL4B0:
# 4529
 Cyc_Toc_do_npop_before(Cyc_Toc_get_npop(s),s);
return;case 9: _LL4B1: _tmp77B=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_tmp77C=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp76B)->f2).f1;_tmp77D=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp76B)->f3).f1;_tmp77E=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp76B)->f4;_LL4B2:
# 4533
 Cyc_Toc_exp_to_c(nv,_tmp77B);Cyc_Toc_exp_to_c(nv,_tmp77C);Cyc_Toc_exp_to_c(nv,_tmp77D);{
struct _RegionHandle _tmp790=_new_region("temp");struct _RegionHandle*temp=& _tmp790;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp77E);
# 4537
_npop_handler(0);return;
# 4534
;_pop_region(temp);};case 10: _LL4B3: _tmp778=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_tmp779=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp76B)->f2;_tmp77A=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp76B)->f3;_LL4B4:
# 4539
 Cyc_Toc_xlate_switch(nv,s,_tmp778,_tmp779,_tmp77A);
return;case 11: _LL4B5: _tmp776=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_tmp777=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp76B)->f2;_LL4B6: {
# 4542
struct Cyc_Toc_Env*_tmp791=nv;struct Cyc_Toc_FallthruInfo*_tmp792;_LL4C8: _tmp792=_tmp791->fallthru_info;_LL4C9:;
if(_tmp792 == 0)
({void*_tmp793=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp794="fallthru in unexpected place";_tag_dyneither(_tmp794,sizeof(char),29);}),_tag_dyneither(_tmp793,sizeof(void*),0));});{
struct Cyc_Toc_FallthruInfo _tmp795=*_tmp792;struct Cyc_Toc_FallthruInfo _tmp796=_tmp795;struct _dyneither_ptr*_tmp797;struct Cyc_List_List*_tmp798;struct Cyc_Dict_Dict _tmp799;_LL4CB: _tmp797=_tmp796.label;_tmp798=_tmp796.binders;_tmp799=_tmp796.next_case_env;_LL4CC:;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp797,0);
# 4548
Cyc_Toc_do_npop_before(Cyc_Toc_get_npop(s),s2);{
struct Cyc_List_List*_tmp79A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp798);
struct Cyc_List_List*_tmp79B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp776);
for(0;_tmp79A != 0;(_tmp79A=_tmp79A->tl,_tmp79B=_tmp79B->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp79B))->hd);
s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp799,(struct _tuple1*)_tmp79A->hd),(struct Cyc_Absyn_Exp*)_tmp79B->hd,0),s2,0);}
# 4556
s->r=s2->r;
return;};};};}case 12: _LL4B7: _tmp774=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_tmp775=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp76B)->f2;_LL4B8:
# 4562
{void*_tmp79C=_tmp774->r;void*_tmp79D=_tmp79C;struct Cyc_Absyn_Tvar*_tmp79E;struct Cyc_Absyn_Vardecl*_tmp79F;int _tmp7A0;struct Cyc_Absyn_Exp*_tmp7A1;struct Cyc_Absyn_Fndecl*_tmp7A2;struct Cyc_List_List*_tmp7A3;struct Cyc_Absyn_Pat*_tmp7A4;struct Cyc_Absyn_Exp*_tmp7A5;void*_tmp7A6;struct Cyc_Absyn_Vardecl*_tmp7A7;switch(*((int*)_tmp79D)){case 0: _LL4CE: _tmp7A7=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp79D)->f1;_LL4CF: {
# 4564
struct _RegionHandle _tmp7A8=_new_region("temp");struct _RegionHandle*temp=& _tmp7A8;_push_region(temp);{
struct Cyc_Toc_Env*_tmp7A9=Cyc_Toc_add_varmap(temp,nv,_tmp7A7->name,
Cyc_Absyn_varb_exp(_tmp7A7->name,(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp7AA=_cycalloc(sizeof(*_tmp7AA));_tmp7AA[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp7AB;_tmp7AB.tag=4;_tmp7AB.f1=_tmp7A7;_tmp7AB;});_tmp7AA;}),0));
Cyc_Toc_local_decl_to_c(_tmp7A9,_tmp7A9,_tmp7A7,_tmp775);}
# 4569
_npop_handler(0);goto _LL4CD;
# 4564
;_pop_region(temp);}case 2: _LL4D0: _tmp7A4=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp79D)->f1;_tmp7A5=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp79D)->f3;_tmp7A6=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp79D)->f4;_LL4D1:
# 4573
{void*_tmp7AC=_tmp7A4->r;void*_tmp7AD=_tmp7AC;struct Cyc_Absyn_Vardecl*_tmp7AE;if(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp7AD)->tag == 1){if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp7AD)->f2)->r)->tag == 0){_LL4DB: _tmp7AE=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp7AD)->f1;_LL4DC: {
# 4575
struct _tuple1*old_name=_tmp7AE->name;
struct _tuple1*new_name=Cyc_Toc_temp_var();
_tmp7AE->name=new_name;
_tmp7AE->initializer=_tmp7A5;
_tmp774->r=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp7AF=_cycalloc(sizeof(*_tmp7AF));_tmp7AF[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp7B0;_tmp7B0.tag=0;_tmp7B0.f1=_tmp7AE;_tmp7B0;});_tmp7AF;});{
struct _RegionHandle _tmp7B1=_new_region("temp");struct _RegionHandle*temp=& _tmp7B1;_push_region(temp);{
struct Cyc_Toc_Env*_tmp7B2=
Cyc_Toc_add_varmap(temp,nv,old_name,
Cyc_Absyn_varb_exp(new_name,(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp7B3=_cycalloc(sizeof(*_tmp7B3));_tmp7B3[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp7B4;_tmp7B4.tag=4;_tmp7B4.f1=_tmp7AE;_tmp7B4;});_tmp7B3;}),0));
Cyc_Toc_local_decl_to_c(_tmp7B2,nv,_tmp7AE,_tmp775);}
# 4586
_npop_handler(0);goto _LL4DA;
# 4580
;_pop_region(temp);};}}else{goto _LL4DD;}}else{_LL4DD: _LL4DE:
# 4591
 s->r=(Cyc_Toc_letdecl_to_c(nv,_tmp7A4,_tmp7A6,(void*)_check_null(_tmp7A5->topt),_tmp7A5,_tmp775))->r;
goto _LL4DA;}_LL4DA:;}
# 4594
goto _LL4CD;case 3: _LL4D2: _tmp7A3=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp79D)->f1;_LL4D3: {
# 4607 "toc.cyc"
struct Cyc_List_List*_tmp7B5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7A3);
if(_tmp7B5 == 0)
({void*_tmp7B6=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp7B7="empty Letv_d";_tag_dyneither(_tmp7B7,sizeof(char),13);}),_tag_dyneither(_tmp7B6,sizeof(void*),0));});
_tmp774->r=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp7B8=_cycalloc(sizeof(*_tmp7B8));_tmp7B8[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp7B9;_tmp7B9.tag=0;_tmp7B9.f1=(struct Cyc_Absyn_Vardecl*)_tmp7B5->hd;_tmp7B9;});_tmp7B8;});
_tmp7B5=_tmp7B5->tl;
for(0;_tmp7B5 != 0;_tmp7B5=_tmp7B5->tl){
struct Cyc_Absyn_Decl*_tmp7BA=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp7BB=_cycalloc(sizeof(*_tmp7BB));_tmp7BB[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp7BC;_tmp7BC.tag=0;_tmp7BC.f1=(struct Cyc_Absyn_Vardecl*)_tmp7B5->hd;_tmp7BC;});_tmp7BB;}),0);
s->r=(Cyc_Absyn_decl_stmt(_tmp7BA,Cyc_Absyn_new_stmt(s->r,0),0))->r;}
# 4616
Cyc_Toc_stmt_to_c(nv,s);
goto _LL4CD;}case 1: _LL4D4: _tmp7A2=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp79D)->f1;_LL4D5: {
# 4619
struct _tuple1*_tmp7BD=_tmp7A2->name;
struct _RegionHandle _tmp7BE=_new_region("temp");struct _RegionHandle*temp=& _tmp7BE;_push_region(temp);{
struct Cyc_Toc_Env*_tmp7BF=Cyc_Toc_add_varmap(temp,nv,_tmp7A2->name,Cyc_Absyn_var_exp(_tmp7BD,0));
Cyc_Toc_fndecl_to_c(_tmp7BF,_tmp7A2,0);
Cyc_Toc_stmt_to_c(_tmp7BF,_tmp775);}
# 4625
_npop_handler(0);goto _LL4CD;
# 4620
;_pop_region(temp);}case 4: _LL4D6: _tmp79E=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp79D)->f1;_tmp79F=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp79D)->f2;_tmp7A0=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp79D)->f3;_tmp7A1=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp79D)->f4;_LL4D7: {
# 4627
struct Cyc_Absyn_Stmt*_tmp7C0=_tmp775;
# 4635
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple1*rh_var=Cyc_Toc_temp_var();
struct _tuple1*x_var=_tmp79F->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);
# 4642
struct _RegionHandle _tmp7C1=_new_region("temp");struct _RegionHandle*temp=& _tmp7C1;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(temp,nv,x_var,x_exp),_tmp7C0);
# 4645
if(Cyc_Absyn_no_regions)
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_uint_exp(0,0),_tmp7C0,0))->r;else{
if((unsigned int)_tmp7A1){
Cyc_Toc_exp_to_c(nv,_tmp7A1);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(Cyc_Absyn_aggrarrow_exp(_tmp7A1,_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"h",sizeof(char),2),0),0);
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,_tmp7C0,0))->r;};}else{
# 4653
s->r=(
Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,({struct Cyc_List_List*_tmp7C3=_cycalloc(sizeof(*_tmp7C3));_tmp7C3->hd=
Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0);_tmp7C3->tl=0;_tmp7C3;}),0),
# 4658
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,({struct Cyc_Absyn_Exp*_tmp7C4[1];_tmp7C4[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp7C4,sizeof(struct Cyc_Absyn_Exp*),1));}),0),0),
Cyc_Absyn_seq_stmt(_tmp7C0,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,({struct Cyc_Absyn_Exp*_tmp7C5[1];_tmp7C5[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp7C5,sizeof(struct Cyc_Absyn_Exp*),1));}),0),0),0),0),0),0))->r;}}
# 4664
_npop_handler(0);return;
# 4642
;_pop_region(temp);}default: _LL4D8: _LL4D9:
# 4665
({void*_tmp7C6=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp7C7="bad nested declaration within function";_tag_dyneither(_tmp7C7,sizeof(char),39);}),_tag_dyneither(_tmp7C6,sizeof(void*),0));});}_LL4CD:;}
# 4667
return;case 13: _LL4B9: _tmp772=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_tmp773=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp76B)->f2;_LL4BA:
# 4669
 s=_tmp773;continue;case 14: _LL4BB: _tmp770=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_tmp771=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp76B)->f2).f1;_LL4BC: {
# 4671
struct _RegionHandle _tmp7C8=_new_region("temp");struct _RegionHandle*temp=& _tmp7C8;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp770);
Cyc_Toc_exp_to_c(nv,_tmp771);
# 4675
_npop_handler(0);return;
# 4671
;_pop_region(temp);}case 15: _LL4BD: _tmp76D=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_tmp76E=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp76B)->f2;_tmp76F=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp76B)->f3;_LL4BE: {
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
struct _RegionHandle _tmp7C9=_new_region("temp");struct _RegionHandle*temp=& _tmp7C9;_push_region(temp);{
struct Cyc_Toc_Env*_tmp7CA=Cyc_Toc_add_varmap(temp,nv,e_var,e_exp);
# 4704
Cyc_Toc_stmt_to_c(_tmp7CA,_tmp76D);{
struct Cyc_Absyn_Stmt*_tmp7CB=Cyc_Absyn_seq_stmt(_tmp76D,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
# 4709
struct Cyc_Absyn_Stmt*_tmp7CC=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp7D3=_cycalloc(sizeof(*_tmp7D3));_tmp7D3[0]=({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct _tmp7D4;_tmp7D4.tag=10;_tmp7D4.f1=e_exp;_tmp7D4.f2=_tmp76E;_tmp7D4.f3=_tmp76F;_tmp7D4;});_tmp7D3;}),0);
# 4711
Cyc_Toc_stmt_to_c(_tmp7CA,_tmp7CC);{
# 4714
struct Cyc_Absyn_Exp*_tmp7CD=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,({struct Cyc_List_List*_tmp7D2=_cycalloc(sizeof(*_tmp7D2));_tmp7D2->hd=
Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0);_tmp7D2->tl=0;_tmp7D2;}),0);
# 4718
struct Cyc_Absyn_Stmt*_tmp7CE=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,({struct Cyc_List_List*_tmp7D1=_cycalloc(sizeof(*_tmp7D1));_tmp7D1->hd=
Cyc_Absyn_address_exp(h_exp,0);_tmp7D1->tl=0;_tmp7D1;}),0),0);
# 4722
struct Cyc_Absyn_Exp*_tmp7CF=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0);
struct Cyc_Absyn_Exp*_tmp7D0=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0);
s->r=(
Cyc_Absyn_declare_stmt(h_var,h_typ,0,
Cyc_Absyn_seq_stmt(_tmp7CE,
Cyc_Absyn_declare_stmt(was_thrown_var,was_thrown_typ,_tmp7CF,
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp7CD,
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp7D0,0),
Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0),_tmp7CB,
# 4733
Cyc_Absyn_declare_stmt(e_var,e_typ,
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp7CC,0),0),0),0),0),0))->r;};};}
# 4737
_npop_handler(0);return;
# 4701
;_pop_region(temp);};}default: _LL4BF: _tmp76C=((struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_LL4C0:
# 4739
 if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{
# 4742
Cyc_Toc_exp_to_c(nv,_tmp76C);
s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,({struct Cyc_List_List*_tmp7D5=_cycalloc(sizeof(*_tmp7D5));_tmp7D5->hd=_tmp76C;_tmp7D5->tl=0;_tmp7D5;}),0));}
# 4745
return;}_LL49E:;}}
# 4754
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->requires_clause=0;
f->ensures_clause=0;
f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp7D6=_new_region("frgn");struct _RegionHandle*frgn=& _tmp7D6;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp7D7=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp7D8=f->args;for(0;_tmp7D8 != 0;_tmp7D8=_tmp7D8->tl){
struct _tuple1*_tmp7D9=({struct _tuple1*_tmp7DA=_cycalloc(sizeof(*_tmp7DA));_tmp7DA->f1=Cyc_Absyn_Loc_n;_tmp7DA->f2=(*((struct _tuple9*)_tmp7D8->hd)).f1;_tmp7DA;});
(*((struct _tuple9*)_tmp7D8->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp7D8->hd)).f3);
_tmp7D7=Cyc_Toc_add_varmap(frgn,_tmp7D7,_tmp7D9,Cyc_Absyn_var_exp(_tmp7D9,0));}}
# 4771
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}
# 4775
Cyc_Toc_fn_pop_table=({struct Cyc_Hashtable_Table**_tmp7DB=_cycalloc(sizeof(*_tmp7DB));_tmp7DB[0]=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_gpop_tables)),f);_tmp7DB;});
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp7DC=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp7DD=_tmp7DC;struct _dyneither_ptr*_tmp7DE;struct Cyc_Absyn_Tqual _tmp7DF;void*_tmp7E0;int _tmp7E1;_LL4E0: _tmp7DE=_tmp7DD.name;_tmp7DF=_tmp7DD.tq;_tmp7E0=_tmp7DD.type;_tmp7E1=_tmp7DD.inject;_LL4E1:;{
void*_tmp7E2=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp7E0,(void*)& Cyc_Absyn_HeapRgn_val,_tmp7DF,Cyc_Absyn_false_conref));
struct _tuple1*_tmp7E3=({struct _tuple1*_tmp7E6=_cycalloc(sizeof(*_tmp7E6));_tmp7E6->f1=Cyc_Absyn_Loc_n;_tmp7E6->f2=(struct _dyneither_ptr*)_check_null(_tmp7DE);_tmp7E6;});
f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,({struct Cyc_List_List*_tmp7E4=_cycalloc(sizeof(*_tmp7E4));_tmp7E4->hd=({struct _tuple9*_tmp7E5=_cycalloc(sizeof(*_tmp7E5));_tmp7E5->f1=_tmp7DE;_tmp7E5->f2=_tmp7DF;_tmp7E5->f3=_tmp7E2;_tmp7E5;});_tmp7E4->tl=0;_tmp7E4;}));
_tmp7D7=Cyc_Toc_add_varmap(frgn,_tmp7D7,_tmp7E3,Cyc_Absyn_var_exp(_tmp7E3,0));
f->cyc_varargs=0;};}
# 4785
{struct Cyc_List_List*_tmp7E7=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp7E7 != 0;_tmp7E7=_tmp7E7->tl){
((struct Cyc_Absyn_Vardecl*)_tmp7E7->hd)->type=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp7E7->hd)->type);}}
# 4788
Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp7D7),f->body);}
# 4763
;_pop_region(frgn);};}
# 4793
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
enum Cyc_Absyn_Scope _tmp7E8=s;switch(_tmp7E8){case Cyc_Absyn_Abstract: _LL4E3: _LL4E4:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL4E5: _LL4E6:
 return Cyc_Absyn_Extern;default: _LL4E7: _LL4E8:
 return s;}_LL4E2:;}struct _tuple36{struct Cyc_Absyn_Aggrdecl*f1;int f2;};struct _tuple37{struct Cyc_Toc_TocState*f1;struct _tuple36*f2;};
# 4810 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple37*env){
# 4813
struct _tuple37 _tmp7E9=*env;struct _tuple37 _tmp7EA=_tmp7E9;struct Cyc_Toc_TocState*_tmp7EB;struct Cyc_Absyn_Aggrdecl*_tmp7EC;int _tmp7ED;_LL4EA: _tmp7EB=_tmp7EA.f1;_tmp7EC=(_tmp7EA.f2)->f1;_tmp7ED=(_tmp7EA.f2)->f2;_LL4EB:;{
struct _tuple1*_tmp7EE=_tmp7EC->name;
struct Cyc_Toc_TocState _tmp7EF=*_tmp7EB;struct Cyc_Toc_TocState _tmp7F0=_tmp7EF;struct Cyc_Dict_Dict*_tmp7F1;_LL4ED: _tmp7F1=_tmp7F0.aggrs_so_far;_LL4EE:;{
int seen_defn_before;
struct _tuple18**_tmp7F2=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp7F1,_tmp7EE);
if(_tmp7F2 == 0){
seen_defn_before=0;{
struct _tuple18*v;
if(_tmp7EC->kind == Cyc_Absyn_StructA)
v=({struct _tuple18*_tmp7F3=_region_malloc(d,sizeof(*_tmp7F3));_tmp7F3->f1=_tmp7EC;_tmp7F3->f2=Cyc_Absyn_strctq(_tmp7EE);_tmp7F3;});else{
# 4824
v=({struct _tuple18*_tmp7F4=_region_malloc(d,sizeof(*_tmp7F4));_tmp7F4->f1=_tmp7EC;_tmp7F4->f2=Cyc_Absyn_unionq_typ(_tmp7EE);_tmp7F4;});}
*_tmp7F1=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp7F1,_tmp7EE,v);};}else{
# 4827
struct _tuple18*_tmp7F5=*_tmp7F2;struct _tuple18*_tmp7F6=_tmp7F5;struct Cyc_Absyn_Aggrdecl*_tmp7F7;void*_tmp7F8;_LL4F0: _tmp7F7=_tmp7F6->f1;_tmp7F8=_tmp7F6->f2;_LL4F1:;
if(_tmp7F7->impl == 0){
*_tmp7F1=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp7F1,_tmp7EE,({struct _tuple18*_tmp7F9=_region_malloc(d,sizeof(*_tmp7F9));_tmp7F9->f1=_tmp7EC;_tmp7F9->f2=_tmp7F8;_tmp7F9;}));
seen_defn_before=0;}else{
# 4832
seen_defn_before=1;}}{
# 4834
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp829=_cycalloc(sizeof(*_tmp829));_tmp829->kind=_tmp7EC->kind;_tmp829->sc=Cyc_Absyn_Public;_tmp829->name=_tmp7EC->name;_tmp829->tvs=0;_tmp829->impl=0;_tmp829->expected_mem_kind=0;_tmp829->attributes=_tmp7EC->attributes;_tmp829;});
# 4841
if(_tmp7EC->impl != 0  && !seen_defn_before){
new_ad->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp7FA=_cycalloc(sizeof(*_tmp7FA));_tmp7FA->exist_vars=0;_tmp7FA->rgn_po=0;_tmp7FA->fields=0;_tmp7FA->tagged=0;_tmp7FA;});{
# 4846
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp7FB=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7EC->impl))->fields;for(0;_tmp7FB != 0;_tmp7FB=_tmp7FB->tl){
# 4850
struct Cyc_Absyn_Aggrfield*_tmp7FC=(struct Cyc_Absyn_Aggrfield*)_tmp7FB->hd;
void*_tmp7FD=_tmp7FC->type;
struct Cyc_List_List*_tmp7FE=_tmp7FC->attributes;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp7FD)) && (
_tmp7EC->kind == Cyc_Absyn_StructA  && _tmp7FB->tl == 0  || _tmp7EC->kind == Cyc_Absyn_UnionA)){
# 4864 "toc.cyc"
void*_tmp7FF=Cyc_Tcutil_compress(_tmp7FD);void*_tmp800=_tmp7FF;void*_tmp801;struct Cyc_Absyn_Tqual _tmp802;union Cyc_Absyn_Constraint*_tmp803;unsigned int _tmp804;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp800)->tag == 8){_LL4F3: _tmp801=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp800)->f1).elt_type;_tmp802=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp800)->f1).tq;_tmp803=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp800)->f1).zero_term;_tmp804=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp800)->f1).zt_loc;_LL4F4:
# 4867
 _tmp7FD=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp805=_cycalloc(sizeof(*_tmp805));_tmp805[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp806;_tmp806.tag=8;_tmp806.f1=({struct Cyc_Absyn_ArrayInfo _tmp807;_tmp807.elt_type=_tmp801;_tmp807.tq=_tmp802;_tmp807.num_elts=Cyc_Absyn_uint_exp(0,0);_tmp807.zero_term=_tmp803;_tmp807.zt_loc=_tmp804;_tmp807;});_tmp806;});_tmp805;});
goto _LL4F2;}else{_LL4F5: _LL4F6:
# 4870
 _tmp7FE=({struct Cyc_List_List*_tmp808=_cycalloc(sizeof(*_tmp808));_tmp808->hd=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp809=_cycalloc(sizeof(*_tmp809));_tmp809[0]=({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmp80A;_tmp80A.tag=6;_tmp80A.f1=0;_tmp80A;});_tmp809;});_tmp808->tl=_tmp7FE;_tmp808;});
_tmp7FD=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp80B=_cycalloc(sizeof(*_tmp80B));_tmp80B[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp80C;_tmp80C.tag=8;_tmp80C.f1=({struct Cyc_Absyn_ArrayInfo _tmp80D;_tmp80D.elt_type=Cyc_Absyn_void_star_typ();_tmp80D.tq=
Cyc_Absyn_empty_tqual(0);_tmp80D.num_elts=
Cyc_Absyn_uint_exp(0,0);_tmp80D.zero_term=
((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0);_tmp80D.zt_loc=0;_tmp80D;});_tmp80C;});_tmp80B;});}_LL4F2:;}{
# 4877
struct Cyc_Absyn_Aggrfield*_tmp80E=({struct Cyc_Absyn_Aggrfield*_tmp824=_cycalloc(sizeof(*_tmp824));_tmp824->name=_tmp7FC->name;_tmp824->tq=Cyc_Toc_mt_tq;_tmp824->type=
# 4879
Cyc_Toc_typ_to_c(_tmp7FD);_tmp824->width=_tmp7FC->width;_tmp824->attributes=_tmp7FE;_tmp824->requires_clause=0;_tmp824;});
# 4887
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7EC->impl))->tagged){
void*_tmp80F=_tmp80E->type;
struct _dyneither_ptr*_tmp810=_tmp80E->name;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp822;_tmp822.tag=0;_tmp822.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp810);({struct Cyc_String_pa_PrintArg_struct _tmp821;_tmp821.tag=0;_tmp821.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp7EC->name).f2);({void*_tmp81F[2]={& _tmp821,& _tmp822};Cyc_aprintf(({const char*_tmp820="_union_%s_%s";_tag_dyneither(_tmp820,sizeof(char),13);}),_tag_dyneither(_tmp81F,sizeof(void*),2));});});});
struct _dyneither_ptr*str=({struct _dyneither_ptr*_tmp81E=_cycalloc(sizeof(*_tmp81E));_tmp81E[0]=s;_tmp81E;});
struct Cyc_Absyn_Aggrfield*_tmp811=({struct Cyc_Absyn_Aggrfield*_tmp81D=_cycalloc(sizeof(*_tmp81D));_tmp81D->name=Cyc_Toc_val_sp;_tmp81D->tq=Cyc_Toc_mt_tq;_tmp81D->type=_tmp80F;_tmp81D->width=0;_tmp81D->attributes=0;_tmp81D->requires_clause=0;_tmp81D;});
struct Cyc_Absyn_Aggrfield*_tmp812=({struct Cyc_Absyn_Aggrfield*_tmp81C=_cycalloc(sizeof(*_tmp81C));_tmp81C->name=Cyc_Toc_tag_sp;_tmp81C->tq=Cyc_Toc_mt_tq;_tmp81C->type=Cyc_Absyn_sint_typ;_tmp81C->width=0;_tmp81C->attributes=0;_tmp81C->requires_clause=0;_tmp81C;});
struct Cyc_List_List*_tmp813=({struct Cyc_Absyn_Aggrfield*_tmp81B[2];_tmp81B[1]=_tmp811;_tmp81B[0]=_tmp812;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp81B,sizeof(struct Cyc_Absyn_Aggrfield*),2));});
struct Cyc_Absyn_Aggrdecl*_tmp814=({struct Cyc_Absyn_Aggrdecl*_tmp818=_cycalloc(sizeof(*_tmp818));_tmp818->kind=Cyc_Absyn_StructA;_tmp818->sc=Cyc_Absyn_Public;_tmp818->name=({struct _tuple1*_tmp81A=_cycalloc(sizeof(*_tmp81A));_tmp81A->f1=Cyc_Absyn_Loc_n;_tmp81A->f2=str;_tmp81A;});_tmp818->tvs=0;_tmp818->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp819=_cycalloc(sizeof(*_tmp819));_tmp819->exist_vars=0;_tmp819->rgn_po=0;_tmp819->fields=_tmp813;_tmp819->tagged=0;_tmp819;});_tmp818->expected_mem_kind=0;_tmp818->attributes=0;_tmp818;});
# 4900
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp815=_cycalloc(sizeof(*_tmp815));_tmp815->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp816=_cycalloc(sizeof(*_tmp816));_tmp816[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp817;_tmp817.tag=5;_tmp817.f1=_tmp814;_tmp817;});_tmp816;}),0);_tmp815->tl=Cyc_Toc_result_decls;_tmp815;});
_tmp80E->type=Cyc_Absyn_strct(str);}
# 4903
new_fields=({struct Cyc_List_List*_tmp823=_cycalloc(sizeof(*_tmp823));_tmp823->hd=_tmp80E;_tmp823->tl=new_fields;_tmp823;});};}}
# 4905
(new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);};}
# 4907
if(_tmp7ED)
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp825=_cycalloc(sizeof(*_tmp825));_tmp825->hd=({struct Cyc_Absyn_Decl*_tmp826=_cycalloc(sizeof(*_tmp826));_tmp826->r=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp827=_cycalloc(sizeof(*_tmp827));_tmp827[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp828;_tmp828.tag=5;_tmp828.f1=new_ad;_tmp828;});_tmp827;});_tmp826->loc=0;_tmp826;});_tmp825->tl=Cyc_Toc_result_decls;_tmp825;});
return 0;};};};}
# 4912
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple36 p=({struct _tuple36 _tmp82A;_tmp82A.f1=ad;_tmp82A.f2=add_to_decls;_tmp82A;});
((int(*)(struct _tuple36*arg,int(*f)(struct _RegionHandle*,struct _tuple37*)))Cyc_Toc_use_toc_state)(& p,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple38{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 4941 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple38*env){
# 4944
struct _tuple38 _tmp82B=*env;struct _tuple38 _tmp82C=_tmp82B;struct Cyc_Set_Set**_tmp82D;struct Cyc_Absyn_Datatypedecl*_tmp82E;_LL4F8: _tmp82D=(_tmp82C.f1)->datatypes_so_far;_tmp82E=_tmp82C.f2;_LL4F9:;{
struct _tuple1*_tmp82F=_tmp82E->name;
if(_tmp82E->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*_tmp82D,_tmp82F))
return 0;
*_tmp82D=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*_tmp82D,_tmp82F);
{struct Cyc_List_List*_tmp830=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp82E->fields))->v;for(0;_tmp830 != 0;_tmp830=_tmp830->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp830->hd;
# 4952
struct Cyc_List_List*_tmp831=0;
int i=1;
{struct Cyc_List_List*_tmp832=f->typs;for(0;_tmp832 != 0;(_tmp832=_tmp832->tl,i ++)){
struct _dyneither_ptr*_tmp833=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmp834=({struct Cyc_Absyn_Aggrfield*_tmp836=_cycalloc(sizeof(*_tmp836));_tmp836->name=_tmp833;_tmp836->tq=(*((struct _tuple11*)_tmp832->hd)).f1;_tmp836->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp832->hd)).f2);_tmp836->width=0;_tmp836->attributes=0;_tmp836->requires_clause=0;_tmp836;});
_tmp831=({struct Cyc_List_List*_tmp835=_cycalloc(sizeof(*_tmp835));_tmp835->hd=_tmp834;_tmp835->tl=_tmp831;_tmp835;});}}
# 4960
_tmp831=({struct Cyc_List_List*_tmp837=_cycalloc(sizeof(*_tmp837));_tmp837->hd=({struct Cyc_Absyn_Aggrfield*_tmp838=_cycalloc(sizeof(*_tmp838));_tmp838->name=Cyc_Toc_tag_sp;_tmp838->tq=Cyc_Toc_mt_tq;_tmp838->type=Cyc_Absyn_sint_typ;_tmp838->width=0;_tmp838->attributes=0;_tmp838->requires_clause=0;_tmp838;});_tmp837->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp831);_tmp837;});{
struct Cyc_Absyn_Aggrdecl*_tmp839=({struct Cyc_Absyn_Aggrdecl*_tmp83D=_cycalloc(sizeof(*_tmp83D));_tmp83D->kind=Cyc_Absyn_StructA;_tmp83D->sc=Cyc_Absyn_Public;_tmp83D->name=
# 4964
Cyc_Toc_collapse_qvars(f->name,_tmp82E->name);_tmp83D->tvs=0;_tmp83D->expected_mem_kind=0;_tmp83D->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp83E=_cycalloc(sizeof(*_tmp83E));_tmp83E->exist_vars=0;_tmp83E->rgn_po=0;_tmp83E->fields=_tmp831;_tmp83E->tagged=0;_tmp83E;});_tmp83D->attributes=0;_tmp83D;});
# 4968
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp83A=_cycalloc(sizeof(*_tmp83A));_tmp83A->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp83B=_cycalloc(sizeof(*_tmp83B));_tmp83B[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp83C;_tmp83C.tag=5;_tmp83C.f1=_tmp839;_tmp83C;});_tmp83B;}),0);_tmp83A->tl=Cyc_Toc_result_decls;_tmp83A;});};}}
# 4970
return 0;};}
# 4973
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple38*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 4992 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple38*env){
# 4995
struct _tuple38 _tmp83F=*env;struct _tuple38 _tmp840=_tmp83F;struct Cyc_Toc_TocState*_tmp841;struct Cyc_Absyn_Datatypedecl*_tmp842;_LL4FB: _tmp841=_tmp840.f1;_tmp842=_tmp840.f2;_LL4FC:;
if(_tmp842->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp843=*_tmp841;struct Cyc_Toc_TocState _tmp844=_tmp843;struct Cyc_Dict_Dict*_tmp845;_LL4FE: _tmp845=_tmp844.xdatatypes_so_far;_LL4FF:;{
struct _tuple1*_tmp846=_tmp842->name;
{struct Cyc_List_List*_tmp847=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp842->fields))->v;for(0;_tmp847 != 0;_tmp847=_tmp847->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp847->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp848=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0);
void*_tmp849=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,_tmp848,Cyc_Absyn_false_conref,0);
# 5006
int*_tmp84A=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp845,f->name);int*_tmp84B=_tmp84A;if(_tmp84B == 0){_LL501: _LL502: {
# 5008
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0);{
# 5012
struct Cyc_Absyn_Vardecl*_tmp84C=Cyc_Absyn_new_vardecl(f->name,_tmp849,initopt);
_tmp84C->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp84D=_cycalloc(sizeof(*_tmp84D));_tmp84D->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp84E=_cycalloc(sizeof(*_tmp84E));_tmp84E[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp84F;_tmp84F.tag=0;_tmp84F.f1=_tmp84C;_tmp84F;});_tmp84E;}),0);_tmp84D->tl=Cyc_Toc_result_decls;_tmp84D;});
*_tmp845=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp845,f->name,f->sc != Cyc_Absyn_Extern);{
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp850=f->typs;for(0;_tmp850 != 0;(_tmp850=_tmp850->tl,i ++)){
struct _dyneither_ptr*_tmp851=({struct _dyneither_ptr*_tmp855=_cycalloc(sizeof(*_tmp855));_tmp855[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp858;_tmp858.tag=1;_tmp858.f1=(unsigned long)i;({void*_tmp856[1]={& _tmp858};Cyc_aprintf(({const char*_tmp857="f%d";_tag_dyneither(_tmp857,sizeof(char),4);}),_tag_dyneither(_tmp856,sizeof(void*),1));});});_tmp855;});
struct Cyc_Absyn_Aggrfield*_tmp852=({struct Cyc_Absyn_Aggrfield*_tmp854=_cycalloc(sizeof(*_tmp854));_tmp854->name=_tmp851;_tmp854->tq=(*((struct _tuple11*)_tmp850->hd)).f1;_tmp854->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp850->hd)).f2);_tmp854->width=0;_tmp854->attributes=0;_tmp854->requires_clause=0;_tmp854;});
fields=({struct Cyc_List_List*_tmp853=_cycalloc(sizeof(*_tmp853));_tmp853->hd=_tmp852;_tmp853->tl=fields;_tmp853;});}}
# 5025
fields=({struct Cyc_List_List*_tmp859=_cycalloc(sizeof(*_tmp859));_tmp859->hd=({struct Cyc_Absyn_Aggrfield*_tmp85A=_cycalloc(sizeof(*_tmp85A));_tmp85A->name=Cyc_Toc_tag_sp;_tmp85A->tq=Cyc_Toc_mt_tq;_tmp85A->type=
Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq);_tmp85A->width=0;_tmp85A->attributes=0;_tmp85A->requires_clause=0;_tmp85A;});_tmp859->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp859;});{
struct Cyc_Absyn_Aggrdecl*_tmp85B=({struct Cyc_Absyn_Aggrdecl*_tmp85F=_cycalloc(sizeof(*_tmp85F));_tmp85F->kind=Cyc_Absyn_StructA;_tmp85F->sc=Cyc_Absyn_Public;_tmp85F->name=
# 5030
Cyc_Toc_collapse_qvars(f->name,_tmp842->name);_tmp85F->tvs=0;_tmp85F->expected_mem_kind=0;_tmp85F->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp860=_cycalloc(sizeof(*_tmp860));_tmp860->exist_vars=0;_tmp860->rgn_po=0;_tmp860->fields=fields;_tmp860->tagged=0;_tmp860;});_tmp85F->attributes=0;_tmp85F;});
# 5034
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp85C=_cycalloc(sizeof(*_tmp85C));_tmp85C->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp85D=_cycalloc(sizeof(*_tmp85D));_tmp85D[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp85E;_tmp85E.tag=5;_tmp85E.f1=_tmp85B;_tmp85E;});_tmp85D;}),0);_tmp85C->tl=Cyc_Toc_result_decls;_tmp85C;});
# 5036
goto _LL500;};};};}}else{if(*((int*)_tmp84B)== 0){_LL503: _LL504:
# 5038
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp861=Cyc_Absyn_string_exp(*fn,0);
struct Cyc_Absyn_Vardecl*_tmp862=Cyc_Absyn_new_vardecl(f->name,_tmp849,_tmp861);
_tmp862->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp863=_cycalloc(sizeof(*_tmp863));_tmp863->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp864=_cycalloc(sizeof(*_tmp864));_tmp864[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp865;_tmp865.tag=0;_tmp865.f1=_tmp862;_tmp865;});_tmp864;}),0);_tmp863->tl=Cyc_Toc_result_decls;_tmp863;});
*_tmp845=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp845,f->name,1);}
# 5045
goto _LL500;}else{_LL505: _LL506:
 goto _LL500;}}_LL500:;}}
# 5049
return 0;};};}
# 5052
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple38*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 5057
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 5065
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
vd->type=Cyc_Toc_typ_to_c(old_typ);
# 5069
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(body_nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp866=init->r;void*_tmp867=_tmp866;struct Cyc_Absyn_Vardecl*_tmp868;struct Cyc_Absyn_Exp*_tmp869;struct Cyc_Absyn_Exp*_tmp86A;int _tmp86B;if(((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp867)->tag == 26){_LL508: _tmp868=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp867)->f1;_tmp869=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp867)->f2;_tmp86A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp867)->f3;_tmp86B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp867)->f4;_LL509:
# 5076
 vd->initializer=0;
s->r=(Cyc_Toc_init_comprehension(init_nv,old_typ,Cyc_Absyn_var_exp(vd->name,0),_tmp868,_tmp869,_tmp86A,_tmp86B,
Cyc_Absyn_new_stmt(s->r,0),0))->r;
goto _LL507;}else{_LL50A: _LL50B:
# 5081
 if(vd->sc == Cyc_Absyn_Static){
# 5085
struct _RegionHandle _tmp86C=_new_region("temp");struct _RegionHandle*temp=& _tmp86C;_push_region(temp);
{struct Cyc_Toc_Env*_tmp86D=Cyc_Toc_set_toplevel(temp,init_nv);
Cyc_Toc_exp_to_c(_tmp86D,init);}
# 5086
;_pop_region(temp);}else{
# 5090
Cyc_Toc_exp_to_c(init_nv,init);}
goto _LL507;}_LL507:;}else{
# 5095
void*_tmp86E=Cyc_Tcutil_compress(old_typ);void*_tmp86F=_tmp86E;void*_tmp870;struct Cyc_Absyn_Exp*_tmp871;union Cyc_Absyn_Constraint*_tmp872;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp86F)->tag == 8){_LL50D: _tmp870=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp86F)->f1).elt_type;_tmp871=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp86F)->f1).num_elts;_tmp872=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp86F)->f1).zero_term;if(
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp872)){_LL50E:
 if(_tmp871 == 0)
({void*_tmp873=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp874="can't initialize zero-terminated array -- size unknown";_tag_dyneither(_tmp874,sizeof(char),55);}),_tag_dyneither(_tmp873,sizeof(void*),0));});{
struct Cyc_Absyn_Exp*num_elts=_tmp871;
struct Cyc_Absyn_Exp*_tmp875=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),
Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);
# 5103
struct Cyc_Absyn_Exp*_tmp876=Cyc_Absyn_signed_int_exp(0,0);
s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp875,_tmp876,0),0),
Cyc_Absyn_new_stmt(s->r,0));
goto _LL50C;};}else{goto _LL50F;}}else{_LL50F: _LL510:
 goto _LL50C;}_LL50C:;}}
# 5114
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success){
void*_tmp877=d;struct Cyc_List_List*_tmp878;struct Cyc_List_List*_tmp879;void**_tmp87A;struct Cyc_Tcpat_Rhs*_tmp87B;switch(*((int*)_tmp877)){case 0: _LL512: _LL513:
 return d;case 1: _LL514: _tmp87B=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp877)->f1;_LL515:
 _tmp87B->rhs=success;return d;default: _LL516: _tmp878=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp877)->f1;_tmp879=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp877)->f2;_tmp87A=(void**)&((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp877)->f3;_LL517:
# 5119
*_tmp87A=Cyc_Toc_rewrite_decision(*_tmp87A,success);
for(0;_tmp879 != 0;_tmp879=_tmp879->tl){
struct _tuple29*_tmp87C=(struct _tuple29*)_tmp879->hd;struct _tuple29*_tmp87D=_tmp87C;void**_tmp87E;_LL519: _tmp87E=(void**)& _tmp87D->f2;_LL51A:;
*_tmp87E=Cyc_Toc_rewrite_decision(*_tmp87E,success);}
# 5124
return d;}_LL511:;}
# 5135 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 5137
struct _RegionHandle _tmp87F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp87F;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp880=Cyc_Toc_share_env(rgn,nv);
void*_tmp881=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(_tmp880,e);{
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 5147
struct Cyc_Absyn_Stmt*_tmp882=Cyc_Toc_skip_stmt_dl();
if(dopt != 0)
# 5151
dopt=Cyc_Toc_rewrite_decision(dopt,_tmp882);{
# 5153
struct Cyc_Absyn_Switch_clause*_tmp883=({struct Cyc_Absyn_Switch_clause*_tmp891=_cycalloc(sizeof(*_tmp891));_tmp891->pattern=p;_tmp891->pat_vars=0;_tmp891->where_clause=0;_tmp891->body=_tmp882;_tmp891->loc=0;_tmp891;});
struct Cyc_List_List*_tmp884=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple31*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,({struct Cyc_Absyn_Switch_clause*_tmp890[1];_tmp890[0]=_tmp883;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(rgn,_tag_dyneither(_tmp890,sizeof(struct Cyc_Absyn_Switch_clause*),1));}));
# 5156
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
# 5160
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp880,& mydecls,& mybodies,dopt,_tmp884,v);
# 5163
struct Cyc_Toc_Env*senv;
for(0;mybodies != 0;mybodies=((struct Cyc_List_List*)_check_null(mybodies))->tl){
struct _tuple35*_tmp885=(struct _tuple35*)((struct Cyc_List_List*)_check_null(mybodies))->hd;struct _tuple35*_tmp886=_tmp885;struct Cyc_Toc_Env*_tmp887;struct Cyc_Absyn_Stmt*_tmp888;_LL51C: _tmp887=_tmp886->f1;_tmp888=_tmp886->f3;_LL51D:;
if(_tmp888 == _tmp882){senv=_tmp887;goto FOUND_ENV;}}
# 5168
({void*_tmp889=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp88A="letdecl_to_c: couldn't find env!";_tag_dyneither(_tmp88A,sizeof(char),33);}),_tag_dyneither(_tmp889,sizeof(void*),0));});
FOUND_ENV:
# 5172
 Cyc_Toc_stmt_to_c(senv,s);{
# 5174
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(test_tree,s,0);
# 5176
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct _tuple33*_tmp88B=(struct _tuple33*)((struct Cyc_List_List*)_check_null(mydecls))->hd;struct _tuple33*_tmp88C=_tmp88B;struct _tuple1*_tmp88D;void*_tmp88E;_LL51F: _tmp88D=_tmp88C->f1;_tmp88E=_tmp88C->f2;_LL520:;
res=Cyc_Absyn_declare_stmt(_tmp88D,_tmp88E,0,res,0);}
# 5181
res=Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c(_tmp881),e,res,0);{
struct Cyc_Absyn_Stmt*_tmp88F=res;_npop_handler(0);return _tmp88F;};};};};}
# 5138
;_pop_region(rgn);}
# 5189
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp892=e->r;void*_tmp893=_tmp892;struct Cyc_Absyn_MallocInfo*_tmp894;struct Cyc_Absyn_Stmt*_tmp895;void**_tmp896;void**_tmp897;struct Cyc_List_List*_tmp898;struct Cyc_List_List*_tmp899;void**_tmp89A;struct Cyc_List_List*_tmp89B;void**_tmp89C;struct Cyc_Absyn_Exp*_tmp89D;struct Cyc_Absyn_Exp*_tmp89E;struct Cyc_List_List*_tmp89F;struct Cyc_Absyn_Exp*_tmp8A0;struct Cyc_Absyn_Exp*_tmp8A1;struct Cyc_Absyn_Exp*_tmp8A2;struct Cyc_Absyn_Exp*_tmp8A3;struct Cyc_Absyn_Exp*_tmp8A4;struct Cyc_Absyn_Exp*_tmp8A5;struct Cyc_Absyn_Exp*_tmp8A6;struct Cyc_Absyn_Exp*_tmp8A7;struct Cyc_Absyn_Exp*_tmp8A8;struct Cyc_Absyn_Exp*_tmp8A9;struct Cyc_Absyn_Exp*_tmp8AA;struct Cyc_Absyn_Exp*_tmp8AB;struct Cyc_Absyn_Exp*_tmp8AC;struct Cyc_Absyn_Exp*_tmp8AD;struct Cyc_Absyn_Exp*_tmp8AE;struct Cyc_List_List*_tmp8AF;struct Cyc_Absyn_Exp*_tmp8B0;struct Cyc_Absyn_Exp*_tmp8B1;struct Cyc_Absyn_Exp*_tmp8B2;struct Cyc_Absyn_Exp*_tmp8B3;struct Cyc_Absyn_Exp*_tmp8B4;struct Cyc_Absyn_Exp*_tmp8B5;struct Cyc_Absyn_Exp*_tmp8B6;struct Cyc_Absyn_Exp*_tmp8B7;switch(*((int*)_tmp893)){case 19: _LL522: _tmp8B7=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp893)->f1;_LL523:
 _tmp8B6=_tmp8B7;goto _LL525;case 20: _LL524: _tmp8B6=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp893)->f1;_LL525:
 _tmp8B5=_tmp8B6;goto _LL527;case 21: _LL526: _tmp8B5=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp893)->f1;_LL527:
 _tmp8B4=_tmp8B5;goto _LL529;case 14: _LL528: _tmp8B4=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp893)->f1;_LL529:
 _tmp8B3=_tmp8B4;goto _LL52B;case 10: _LL52A: _tmp8B3=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp893)->f1;_LL52B:
 _tmp8B2=_tmp8B3;goto _LL52D;case 11: _LL52C: _tmp8B2=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp893)->f1;_LL52D:
 _tmp8B1=_tmp8B2;goto _LL52F;case 17: _LL52E: _tmp8B1=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp893)->f1;_LL52F:
 _tmp8B0=_tmp8B1;goto _LL531;case 4: _LL530: _tmp8B0=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp893)->f1;_LL531:
 Cyc_Toc_exptypes_to_c(_tmp8B0);goto _LL521;case 2: _LL532: _tmp8AF=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp893)->f2;_LL533:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp8AF);goto _LL521;case 6: _LL534: _tmp8AD=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp893)->f1;_tmp8AE=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp893)->f2;_LL535:
 _tmp8AB=_tmp8AD;_tmp8AC=_tmp8AE;goto _LL537;case 7: _LL536: _tmp8AB=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp893)->f1;_tmp8AC=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp893)->f2;_LL537:
 _tmp8A9=_tmp8AB;_tmp8AA=_tmp8AC;goto _LL539;case 8: _LL538: _tmp8A9=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp893)->f1;_tmp8AA=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp893)->f2;_LL539:
 _tmp8A7=_tmp8A9;_tmp8A8=_tmp8AA;goto _LL53B;case 22: _LL53A: _tmp8A7=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp893)->f1;_tmp8A8=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp893)->f2;_LL53B:
 _tmp8A5=_tmp8A7;_tmp8A6=_tmp8A8;goto _LL53D;case 34: _LL53C: _tmp8A5=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp893)->f1;_tmp8A6=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp893)->f2;_LL53D:
 _tmp8A3=_tmp8A5;_tmp8A4=_tmp8A6;goto _LL53F;case 3: _LL53E: _tmp8A3=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp893)->f1;_tmp8A4=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp893)->f3;_LL53F:
 Cyc_Toc_exptypes_to_c(_tmp8A3);Cyc_Toc_exptypes_to_c(_tmp8A4);goto _LL521;case 5: _LL540: _tmp8A0=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp893)->f1;_tmp8A1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp893)->f2;_tmp8A2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp893)->f3;_LL541:
# 5207
 Cyc_Toc_exptypes_to_c(_tmp8A0);Cyc_Toc_exptypes_to_c(_tmp8A1);Cyc_Toc_exptypes_to_c(_tmp8A2);goto _LL521;case 9: _LL542: _tmp89E=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp893)->f1;_tmp89F=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp893)->f2;_LL543:
# 5209
 Cyc_Toc_exptypes_to_c(_tmp89E);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp89F);goto _LL521;case 13: _LL544: _tmp89C=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp893)->f1;_tmp89D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp893)->f2;_LL545:
*_tmp89C=Cyc_Toc_typ_to_c(*_tmp89C);Cyc_Toc_exptypes_to_c(_tmp89D);goto _LL521;case 24: _LL546: _tmp89A=(void**)&(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp893)->f1)->f3;_tmp89B=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp893)->f2;_LL547:
# 5212
*_tmp89A=Cyc_Toc_typ_to_c(*_tmp89A);
_tmp899=_tmp89B;goto _LL549;case 35: _LL548: _tmp899=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp893)->f2;_LL549:
 _tmp898=_tmp899;goto _LL54B;case 25: _LL54A: _tmp898=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp893)->f1;_LL54B:
# 5216
 for(0;_tmp898 != 0;_tmp898=_tmp898->tl){
struct _tuple19 _tmp8B8=*((struct _tuple19*)_tmp898->hd);struct _tuple19 _tmp8B9=_tmp8B8;struct Cyc_Absyn_Exp*_tmp8BA;_LL573: _tmp8BA=_tmp8B9.f2;_LL574:;
Cyc_Toc_exptypes_to_c(_tmp8BA);}
# 5220
goto _LL521;case 18: _LL54C: _tmp897=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp893)->f1;_LL54D:
 _tmp896=_tmp897;goto _LL54F;case 16: _LL54E: _tmp896=(void**)&((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp893)->f1;_LL54F:
*_tmp896=Cyc_Toc_typ_to_c(*_tmp896);goto _LL521;case 36: _LL550: _tmp895=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp893)->f1;_LL551:
 Cyc_Toc_stmttypes_to_c(_tmp895);goto _LL521;case 33: _LL552: _tmp894=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp893)->f1;_LL553:
# 5225
 if(_tmp894->elt_type != 0)
_tmp894->elt_type=({void**_tmp8BB=_cycalloc(sizeof(*_tmp8BB));_tmp8BB[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp894->elt_type)));_tmp8BB;});
Cyc_Toc_exptypes_to_c(_tmp894->num_elts);
goto _LL521;case 0: _LL554: _LL555:
 goto _LL557;case 1: _LL556: _LL557:
 goto _LL559;case 31: _LL558: _LL559:
 goto _LL55B;case 39: _LL55A: _LL55B:
 goto _LL55D;case 32: _LL55C: _LL55D:
 goto _LL521;case 29: _LL55E: _LL55F:
# 5235
 goto _LL561;case 30: _LL560: _LL561:
 goto _LL563;case 28: _LL562: _LL563:
 goto _LL565;case 26: _LL564: _LL565:
 goto _LL567;case 27: _LL566: _LL567:
 goto _LL569;case 23: _LL568: _LL569:
 goto _LL56B;case 12: _LL56A: _LL56B:
 goto _LL56D;case 15: _LL56C: _LL56D:
 goto _LL56F;case 38: _LL56E: _LL56F:
 goto _LL571;default: _LL570: _LL571:
# 5245
({void*_tmp8BC=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp8BD="Cyclone expression within C code";_tag_dyneither(_tmp8BD,sizeof(char),33);}),_tag_dyneither(_tmp8BC,sizeof(void*),0));});
goto _LL521;}_LL521:;}
# 5250
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp8BE=d->r;void*_tmp8BF=_tmp8BE;struct Cyc_Absyn_Typedefdecl*_tmp8C0;struct Cyc_Absyn_Enumdecl*_tmp8C1;struct Cyc_Absyn_Aggrdecl*_tmp8C2;struct Cyc_Absyn_Fndecl*_tmp8C3;struct Cyc_Absyn_Vardecl*_tmp8C4;switch(*((int*)_tmp8BF)){case 0: _LL576: _tmp8C4=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8BF)->f1;_LL577:
# 5253
 _tmp8C4->type=Cyc_Toc_typ_to_c(_tmp8C4->type);
if(_tmp8C4->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp8C4->initializer));
goto _LL575;case 1: _LL578: _tmp8C3=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp8BF)->f1;_LL579:
# 5257
 _tmp8C3->ret_type=Cyc_Toc_typ_to_c(_tmp8C3->ret_type);
{struct Cyc_List_List*_tmp8C5=_tmp8C3->args;for(0;_tmp8C5 != 0;_tmp8C5=_tmp8C5->tl){
(*((struct _tuple9*)_tmp8C5->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp8C5->hd)).f3);}}
# 5261
goto _LL575;case 5: _LL57A: _tmp8C2=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp8BF)->f1;_LL57B:
 Cyc_Toc_aggrdecl_to_c(_tmp8C2,1);goto _LL575;case 7: _LL57C: _tmp8C1=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp8BF)->f1;_LL57D:
# 5264
 if(_tmp8C1->fields != 0){
struct Cyc_List_List*_tmp8C6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp8C1->fields))->v;for(0;_tmp8C6 != 0;_tmp8C6=_tmp8C6->tl){
struct Cyc_Absyn_Enumfield*_tmp8C7=(struct Cyc_Absyn_Enumfield*)_tmp8C6->hd;
if(_tmp8C7->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp8C7->tag));}}
# 5269
goto _LL575;case 8: _LL57E: _tmp8C0=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp8BF)->f1;_LL57F:
 _tmp8C0->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp8C0->defn));goto _LL575;case 2: _LL580: _LL581:
 goto _LL583;case 3: _LL582: _LL583:
 goto _LL585;case 6: _LL584: _LL585:
 goto _LL587;case 9: _LL586: _LL587:
 goto _LL589;case 10: _LL588: _LL589:
 goto _LL58B;case 11: _LL58A: _LL58B:
 goto _LL58D;case 12: _LL58C: _LL58D:
 goto _LL58F;case 4: _LL58E: _LL58F:
# 5280
({void*_tmp8C8=0;Cyc_Tcutil_terr(d->loc,({const char*_tmp8C9="Cyclone declaration within C code";_tag_dyneither(_tmp8C9,sizeof(char),34);}),_tag_dyneither(_tmp8C8,sizeof(void*),0));});
goto _LL575;case 13: _LL590: _LL591:
 goto _LL593;default: _LL592: _LL593:
# 5284
 goto _LL575;}_LL575:;}
# 5288
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp8CA=s->r;void*_tmp8CB=_tmp8CA;struct Cyc_Absyn_Stmt*_tmp8CC;struct Cyc_Absyn_Stmt*_tmp8CD;struct Cyc_Absyn_Exp*_tmp8CE;struct Cyc_Absyn_Decl*_tmp8CF;struct Cyc_Absyn_Stmt*_tmp8D0;struct Cyc_Absyn_Exp*_tmp8D1;struct Cyc_List_List*_tmp8D2;void*_tmp8D3;struct Cyc_Absyn_Exp*_tmp8D4;struct Cyc_Absyn_Exp*_tmp8D5;struct Cyc_Absyn_Exp*_tmp8D6;struct Cyc_Absyn_Stmt*_tmp8D7;struct Cyc_Absyn_Exp*_tmp8D8;struct Cyc_Absyn_Stmt*_tmp8D9;struct Cyc_Absyn_Exp*_tmp8DA;struct Cyc_Absyn_Stmt*_tmp8DB;struct Cyc_Absyn_Stmt*_tmp8DC;struct Cyc_Absyn_Exp*_tmp8DD;struct Cyc_Absyn_Stmt*_tmp8DE;struct Cyc_Absyn_Stmt*_tmp8DF;struct Cyc_Absyn_Exp*_tmp8E0;switch(*((int*)_tmp8CB)){case 1: _LL595: _tmp8E0=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp8CB)->f1;_LL596:
 Cyc_Toc_exptypes_to_c(_tmp8E0);goto _LL594;case 2: _LL597: _tmp8DE=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8CB)->f1;_tmp8DF=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8CB)->f2;_LL598:
 Cyc_Toc_stmttypes_to_c(_tmp8DE);Cyc_Toc_stmttypes_to_c(_tmp8DF);goto _LL594;case 3: _LL599: _tmp8DD=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp8CB)->f1;_LL59A:
 if(_tmp8DD != 0)Cyc_Toc_exptypes_to_c(_tmp8DD);goto _LL594;case 4: _LL59B: _tmp8DA=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8CB)->f1;_tmp8DB=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8CB)->f2;_tmp8DC=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8CB)->f3;_LL59C:
# 5294
 Cyc_Toc_exptypes_to_c(_tmp8DA);Cyc_Toc_stmttypes_to_c(_tmp8DB);Cyc_Toc_stmttypes_to_c(_tmp8DC);goto _LL594;case 5: _LL59D: _tmp8D8=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp8CB)->f1).f1;_tmp8D9=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp8CB)->f2;_LL59E:
# 5296
 Cyc_Toc_exptypes_to_c(_tmp8D8);Cyc_Toc_stmttypes_to_c(_tmp8D9);goto _LL594;case 9: _LL59F: _tmp8D4=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8CB)->f1;_tmp8D5=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8CB)->f2).f1;_tmp8D6=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8CB)->f3).f1;_tmp8D7=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8CB)->f4;_LL5A0:
# 5298
 Cyc_Toc_exptypes_to_c(_tmp8D4);Cyc_Toc_exptypes_to_c(_tmp8D5);Cyc_Toc_exptypes_to_c(_tmp8D6);
Cyc_Toc_stmttypes_to_c(_tmp8D7);goto _LL594;case 10: _LL5A1: _tmp8D1=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8CB)->f1;_tmp8D2=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8CB)->f2;_tmp8D3=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8CB)->f3;_LL5A2:
# 5301
 Cyc_Toc_exptypes_to_c(_tmp8D1);
for(0;_tmp8D2 != 0;_tmp8D2=_tmp8D2->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp8D2->hd)->body);}
goto _LL594;case 12: _LL5A3: _tmp8CF=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8CB)->f1;_tmp8D0=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8CB)->f2;_LL5A4:
 Cyc_Toc_decltypes_to_c(_tmp8CF);Cyc_Toc_stmttypes_to_c(_tmp8D0);goto _LL594;case 14: _LL5A5: _tmp8CD=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp8CB)->f1;_tmp8CE=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp8CB)->f2).f1;_LL5A6:
 Cyc_Toc_stmttypes_to_c(_tmp8CD);Cyc_Toc_exptypes_to_c(_tmp8CE);goto _LL594;case 13: _LL5A7: _tmp8CC=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp8CB)->f2;_LL5A8:
 Cyc_Toc_stmttypes_to_c(_tmp8CC);goto _LL594;case 0: _LL5A9: _LL5AA:
 goto _LL5AC;case 6: _LL5AB: _LL5AC:
 goto _LL5AE;case 7: _LL5AD: _LL5AE:
 goto _LL5B0;case 8: _LL5AF: _LL5B0:
# 5311
 goto _LL594;case 11: _LL5B1: _LL5B2:
# 5314
 s->r=(void*)({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp8E1=_cycalloc_atomic(sizeof(*_tmp8E1));_tmp8E1[0]=({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct _tmp8E2;_tmp8E2.tag=0;_tmp8E2;});_tmp8E1;});
goto _LL594;case 15: _LL5B3: _LL5B4:
 goto _LL5B6;default: _LL5B5: _LL5B6:
# 5318
({void*_tmp8E3=0;Cyc_Tcutil_terr(s->loc,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp8E6;_tmp8E6.tag=0;_tmp8E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));({void*_tmp8E4[1]={& _tmp8E6};Cyc_aprintf(({const char*_tmp8E5="Cyclone statement in C code: %s";_tag_dyneither(_tmp8E5,sizeof(char),32);}),_tag_dyneither(_tmp8E4,sizeof(void*),1));});}),_tag_dyneither(_tmp8E3,sizeof(void*),0));});
goto _LL594;}_LL594:;}
# 5327
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv))
({void*_tmp8E7=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp8E8="decls_to_c: not at toplevel!";_tag_dyneither(_tmp8E8,sizeof(char),29);}),_tag_dyneither(_tmp8E7,sizeof(void*),0));});{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp8E9=d->r;void*_tmp8EA=_tmp8E9;struct Cyc_List_List*_tmp8EB;struct Cyc_List_List*_tmp8EC;struct Cyc_List_List*_tmp8ED;struct Cyc_List_List*_tmp8EE;struct Cyc_Absyn_Typedefdecl*_tmp8EF;struct Cyc_Absyn_Enumdecl*_tmp8F0;struct Cyc_Absyn_Datatypedecl*_tmp8F1;struct Cyc_Absyn_Aggrdecl*_tmp8F2;struct Cyc_Absyn_Fndecl*_tmp8F3;struct Cyc_Absyn_Vardecl*_tmp8F4;switch(*((int*)_tmp8EA)){case 0: _LL5B8: _tmp8F4=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8EA)->f1;_LL5B9: {
# 5334
struct _tuple1*_tmp8F5=_tmp8F4->name;
# 5336
if(_tmp8F4->sc == Cyc_Absyn_ExternC)
_tmp8F5=({struct _tuple1*_tmp8F6=_cycalloc(sizeof(*_tmp8F6));_tmp8F6->f1=Cyc_Absyn_Rel_n(0);_tmp8F6->f2=(*_tmp8F5).f2;_tmp8F6;});
if(_tmp8F4->initializer != 0){
if(_tmp8F4->sc == Cyc_Absyn_ExternC)_tmp8F4->sc=Cyc_Absyn_Public;
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp8F4->initializer));else{
# 5343
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp8F4->initializer));}}
# 5345
nv=Cyc_Toc_add_varmap(r,nv,_tmp8F4->name,Cyc_Absyn_varb_exp(_tmp8F5,(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp8F7=_cycalloc(sizeof(*_tmp8F7));_tmp8F7[0]=({struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp8F8;_tmp8F8.tag=1;_tmp8F8.f1=_tmp8F4;_tmp8F8;});_tmp8F7;}),0));
_tmp8F4->name=_tmp8F5;
_tmp8F4->sc=Cyc_Toc_scope_to_c(_tmp8F4->sc);
_tmp8F4->type=Cyc_Toc_typ_to_c(_tmp8F4->type);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp8F9=_cycalloc(sizeof(*_tmp8F9));_tmp8F9->hd=d;_tmp8F9->tl=Cyc_Toc_result_decls;_tmp8F9;});
goto _LL5B7;}case 1: _LL5BA: _tmp8F3=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp8EA)->f1;_LL5BB: {
# 5352
struct _tuple1*_tmp8FA=_tmp8F3->name;
# 5354
if(_tmp8F3->sc == Cyc_Absyn_ExternC){
_tmp8FA=({struct _tuple1*_tmp8FB=_cycalloc(sizeof(*_tmp8FB));_tmp8FB->f1=Cyc_Absyn_Abs_n(0,1);_tmp8FB->f2=(*_tmp8FA).f2;_tmp8FB;});
_tmp8F3->sc=Cyc_Absyn_Public;}
# 5358
nv=Cyc_Toc_add_varmap(r,nv,_tmp8F3->name,Cyc_Absyn_var_exp(_tmp8FA,0));
_tmp8F3->name=_tmp8FA;
Cyc_Toc_fndecl_to_c(nv,_tmp8F3,cinclude);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp8FC=_cycalloc(sizeof(*_tmp8FC));_tmp8FC->hd=d;_tmp8FC->tl=Cyc_Toc_result_decls;_tmp8FC;});
goto _LL5B7;}case 2: _LL5BC: _LL5BD:
 goto _LL5BF;case 3: _LL5BE: _LL5BF:
# 5365
({void*_tmp8FD=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp8FE="letdecl at toplevel";_tag_dyneither(_tmp8FE,sizeof(char),20);}),_tag_dyneither(_tmp8FD,sizeof(void*),0));});case 4: _LL5C0: _LL5C1:
# 5367
({void*_tmp8FF=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp900="region decl at toplevel";_tag_dyneither(_tmp900,sizeof(char),24);}),_tag_dyneither(_tmp8FF,sizeof(void*),0));});case 5: _LL5C2: _tmp8F2=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp8EA)->f1;_LL5C3:
# 5371
 Cyc_Toc_aggrdecl_to_c(_tmp8F2,1);
goto _LL5B7;case 6: _LL5C4: _tmp8F1=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp8EA)->f1;_LL5C5:
# 5374
 if(_tmp8F1->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp8F1);else{
# 5377
Cyc_Toc_datatypedecl_to_c(_tmp8F1);}
goto _LL5B7;case 7: _LL5C6: _tmp8F0=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp8EA)->f1;_LL5C7:
# 5380
 Cyc_Toc_enumdecl_to_c(_tmp8F0);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp901=_cycalloc(sizeof(*_tmp901));_tmp901->hd=d;_tmp901->tl=Cyc_Toc_result_decls;_tmp901;});
goto _LL5B7;case 8: _LL5C8: _tmp8EF=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp8EA)->f1;_LL5C9:
# 5384
 _tmp8EF->tvs=0;
if(_tmp8EF->defn != 0)
_tmp8EF->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp8EF->defn));else{
# 5388
enum Cyc_Absyn_KindQual _tmp902=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp8EF->kind))->v)->kind;enum Cyc_Absyn_KindQual _tmp903=_tmp902;if(_tmp903 == Cyc_Absyn_BoxKind){_LL5D7: _LL5D8:
 _tmp8EF->defn=Cyc_Absyn_void_star_typ();goto _LL5D6;}else{_LL5D9: _LL5DA:
 _tmp8EF->defn=(void*)& Cyc_Absyn_VoidType_val;goto _LL5D6;}_LL5D6:;}
# 5393
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp904=_cycalloc(sizeof(*_tmp904));_tmp904->hd=d;_tmp904->tl=Cyc_Toc_result_decls;_tmp904;});
goto _LL5B7;case 13: _LL5CA: _LL5CB:
 goto _LL5CD;case 14: _LL5CC: _LL5CD:
# 5397
 goto _LL5B7;case 9: _LL5CE: _tmp8EE=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp8EA)->f2;_LL5CF:
 _tmp8ED=_tmp8EE;goto _LL5D1;case 10: _LL5D0: _tmp8ED=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp8EA)->f2;_LL5D1:
 _tmp8EC=_tmp8ED;goto _LL5D3;case 11: _LL5D2: _tmp8EC=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp8EA)->f1;_LL5D3:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp8EC,top,cinclude);goto _LL5B7;default: _LL5D4: _tmp8EB=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp8EA)->f1;_LL5D5:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp8EB,top,1);goto _LL5B7;}_LL5B7:;};}
# 5404
return nv;}
# 5408
static void Cyc_Toc_init(){
struct Cyc_Core_NewDynamicRegion _tmp905=Cyc_Core_new_rckey();struct Cyc_Core_NewDynamicRegion _tmp906=_tmp905;struct Cyc_Core_DynamicRegion*_tmp907;_LL5DC: _tmp907=_tmp906.key;_LL5DD:;{
struct Cyc_Toc_TocState*_tmp908=((struct Cyc_Toc_TocState*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Toc_TocState*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmp907,0,Cyc_Toc_empty_toc_state);
Cyc_Toc_toc_state=({struct Cyc_Toc_TocStateWrap*_tmp909=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp909));_tmp909->dyn=(struct Cyc_Core_DynamicRegion*)_tmp907;_tmp909->state=(struct Cyc_Toc_TocState*)_tmp908;_tmp909;});
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0;
Cyc_Toc_bounds_checks_eliminated=0;
Cyc_Toc_globals=_tag_dyneither(({struct _dyneither_ptr**_tmp90A=_cycalloc(sizeof(struct _dyneither_ptr*)* 42);_tmp90A[0]=(struct _dyneither_ptr*)& Cyc_Toc__throw_str;_tmp90A[1]=(struct _dyneither_ptr*)& Cyc_Toc_setjmp_str;_tmp90A[2]=(struct _dyneither_ptr*)& Cyc_Toc__push_handler_str;_tmp90A[3]=(struct _dyneither_ptr*)& Cyc_Toc__pop_handler_str;_tmp90A[4]=(struct _dyneither_ptr*)& Cyc_Toc__exn_thrown_str;_tmp90A[5]=(struct _dyneither_ptr*)& Cyc_Toc__npop_handler_str;_tmp90A[6]=(struct _dyneither_ptr*)& Cyc_Toc__check_null_str;_tmp90A[7]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_null_str;_tmp90A[8]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_notnull_str;_tmp90A[9]=(struct _dyneither_ptr*)& Cyc_Toc__check_dyneither_subscript_str;_tmp90A[10]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_str;_tmp90A[11]=(struct _dyneither_ptr*)& Cyc_Toc__tag_dyneither_str;_tmp90A[12]=(struct _dyneither_ptr*)& Cyc_Toc__init_dyneither_ptr_str;_tmp90A[13]=(struct _dyneither_ptr*)& Cyc_Toc__untag_dyneither_ptr_str;_tmp90A[14]=(struct _dyneither_ptr*)& Cyc_Toc__get_dyneither_size_str;_tmp90A[15]=(struct _dyneither_ptr*)& Cyc_Toc__get_zero_arr_size_str;_tmp90A[16]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_plus_str;_tmp90A[17]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_plus_str;_tmp90A[18]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_str;_tmp90A[19]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_str;_tmp90A[20]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_str;_tmp90A[21]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_post_str;_tmp90A[22]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_str;_tmp90A[23]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_str;_tmp90A[24]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_atomic_str;_tmp90A[25]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_atomic_str;_tmp90A[26]=(struct _dyneither_ptr*)& Cyc_Toc__region_malloc_str;_tmp90A[27]=(struct _dyneither_ptr*)& Cyc_Toc__region_calloc_str;_tmp90A[28]=(struct _dyneither_ptr*)& Cyc_Toc__check_times_str;_tmp90A[29]=(struct _dyneither_ptr*)& Cyc_Toc__new_region_str;_tmp90A[30]=(struct _dyneither_ptr*)& Cyc_Toc__push_region_str;_tmp90A[31]=(struct _dyneither_ptr*)& Cyc_Toc__pop_region_str;_tmp90A[32]=(struct _dyneither_ptr*)& Cyc_Toc__open_dynregion_str;_tmp90A[33]=(struct _dyneither_ptr*)& Cyc_Toc__push_dynregion_str;_tmp90A[34]=(struct _dyneither_ptr*)& Cyc_Toc__pop_dynregion_str;_tmp90A[35]=(struct _dyneither_ptr*)& Cyc_Toc__reset_region_str;_tmp90A[36]=(struct _dyneither_ptr*)& Cyc_Toc__throw_arraybounds_str;_tmp90A[37]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_decrease_size_str;_tmp90A[38]=(struct _dyneither_ptr*)& Cyc_Toc__throw_match_str;_tmp90A[39]=(struct _dyneither_ptr*)& Cyc_Toc__swap_word_str;_tmp90A[40]=(struct _dyneither_ptr*)& Cyc_Toc__swap_dyneither_str;_tmp90A[41]=(struct _dyneither_ptr*)& Cyc_Toc__fast_region_malloc_str;_tmp90A;}),sizeof(struct _dyneither_ptr*),42);};}
# 5465
static int Cyc_Toc_destroy_labels(struct _RegionHandle*d,struct Cyc_Toc_TocState*ts){
struct Cyc_Toc_TocState _tmp90B=*ts;struct Cyc_Toc_TocState _tmp90C=_tmp90B;struct Cyc_Xarray_Xarray*_tmp90D;_LL5DF: _tmp90D=_tmp90C.temp_labels;_LL5E0:;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp90D);
return 0;}
# 5473
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds){
# 5475
Cyc_Toc_gpop_tables=({struct Cyc_Hashtable_Table**_tmp90E=_cycalloc(sizeof(*_tmp90E));_tmp90E[0]=pop_tables;_tmp90E;});
Cyc_Toc_init();{
struct _RegionHandle _tmp90F=_new_region("start");struct _RegionHandle*start=& _tmp90F;_push_region(start);
Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(start),ds,1,0);{
# 5484
struct Cyc_Toc_TocStateWrap*ts=0;
_swap_word(& ts,& Cyc_Toc_toc_state);{
struct Cyc_Toc_TocStateWrap _tmp910=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp911=_tmp910;struct Cyc_Core_DynamicRegion*_tmp912;struct Cyc_Toc_TocState*_tmp913;_LL5E2: _tmp912=_tmp911.dyn;_tmp913=_tmp911.state;_LL5E3:;
((int(*)(struct Cyc_Core_DynamicRegion*key,struct Cyc_Toc_TocState*arg,int(*body)(struct _RegionHandle*h,struct Cyc_Toc_TocState*arg)))Cyc_Core_open_region)(_tmp912,_tmp913,Cyc_Toc_destroy_labels);
Cyc_Core_free_rckey(_tmp912);
((void(*)(struct _dyneither_ptr ptr))Cyc_Core_ufree)(_tag_dyneither(ts,sizeof(struct Cyc_Toc_TocStateWrap),1));};};
# 5491
Cyc_Toc_gpop_tables=0;
Cyc_Toc_fn_pop_table=0;{
struct Cyc_List_List*_tmp914=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0);return _tmp914;};
# 5477
;_pop_region(start);};}
