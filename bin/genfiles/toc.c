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
struct _tuple14 _tmp0=*x;struct _tuple14 _tmp1=_tmp0;struct _tuple1*_tmp2;struct _tuple1*_tmp3;_LL1: _tmp2=_tmp1.f1;_tmp3=_tmp1.f2;_LL2:;{
struct _tuple14 _tmp4=*y;struct _tuple14 _tmp5=_tmp4;struct _tuple1*_tmp6;struct _tuple1*_tmp7;_LL4: _tmp6=_tmp5.f1;_tmp7=_tmp5.f2;_LL5:;{
int i=Cyc_Absyn_qvar_cmp(_tmp2,_tmp6);
if(i != 0)return i;
return Cyc_Absyn_qvar_cmp(_tmp3,_tmp7);};};}
# 116
static struct Cyc_Toc_TocState*Cyc_Toc_empty_toc_state(struct _RegionHandle*d,int unused){
struct Cyc_Dict_Dict*_tmp921;struct Cyc_Dict_Dict*_tmp920;struct Cyc_Set_Set**_tmp91F;struct Cyc_List_List**_tmp91E;struct Cyc_Dict_Dict*_tmp91D;struct Cyc_List_List**_tmp91C;struct Cyc_Toc_TocState*_tmp91B;return(_tmp91B=_region_malloc(d,sizeof(*_tmp91B)),((_tmp91B->tuple_types=(
(_tmp91C=_region_malloc(d,sizeof(*_tmp91C)),((_tmp91C[0]=0,_tmp91C)))),((_tmp91B->aggrs_so_far=(
(_tmp91D=_region_malloc(d,sizeof(*_tmp91D)),((_tmp91D[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmp91D)))),((_tmp91B->abs_struct_types=(
(_tmp91E=_region_malloc(d,sizeof(*_tmp91E)),((_tmp91E[0]=0,_tmp91E)))),((_tmp91B->datatypes_so_far=(
(_tmp91F=_region_malloc(d,sizeof(*_tmp91F)),((_tmp91F[0]=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),_tmp91F)))),((_tmp91B->xdatatypes_so_far=(
(_tmp920=_region_malloc(d,sizeof(*_tmp920)),((_tmp920[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmp920)))),((_tmp91B->qvar_tags=(
(_tmp921=_region_malloc(d,sizeof(*_tmp921)),((_tmp921[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),_tmp921)))),((_tmp91B->temp_labels=
((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d),_tmp91B)))))))))))))));}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 133
typedef struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state_t;
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple15{struct Cyc_Toc_TocState*f1;void*f2;};
# 140
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple15*)){
# 143
struct Cyc_Toc_TocStateWrap*ts=0;
_swap_word(& ts,& Cyc_Toc_toc_state);{
struct Cyc_Toc_TocStateWrap _tmpF=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp10=_tmpF;struct Cyc_Core_DynamicRegion*_tmp11;struct Cyc_Toc_TocState*_tmp12;_LL7: _tmp11=_tmp10.dyn;_tmp12=_tmp10.state;_LL8:;{
struct _dyneither_ptr _tmp13=((struct _dyneither_ptr(*)(struct _dyneither_ptr ptr))Cyc_Core_alias_refptr)(_tag_dyneither(_tmp11,sizeof(struct Cyc_Core_DynamicRegion),1));
{struct Cyc_Toc_TocStateWrap _tmp922;*ts=((_tmp922.dyn=(struct Cyc_Core_DynamicRegion*)_tmp11,((_tmp922.state=(struct Cyc_Toc_TocState*)_tmp12,_tmp922))));}
_swap_word(& ts,& Cyc_Toc_toc_state);{
struct _tuple15 _tmp923;struct _tuple15 _tmp15=(_tmp923.f1=_tmp12,((_tmp923.f2=arg,_tmp923)));
void*_tmp16=((void*(*)(struct Cyc_Core_DynamicRegion*key,struct _tuple15*arg,void*(*body)(struct _RegionHandle*h,struct _tuple15*arg)))Cyc_Core_open_region)((struct Cyc_Core_DynamicRegion*)_untag_dyneither_ptr(_tmp13,sizeof(struct Cyc_Core_DynamicRegion),1),& _tmp15,f);
Cyc_Core_free_rckey((struct Cyc_Core_DynamicRegion*)_untag_dyneither_ptr(_tmp13,sizeof(struct Cyc_Core_DynamicRegion),1));
return _tmp16;};};};}struct _tuple16{struct _tuple1*f1;void*(*f2)(struct _tuple1*);};struct _tuple17{struct Cyc_Toc_TocState*f1;struct _tuple16*f2;};struct _tuple18{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
# 155
static void*Cyc_Toc_aggrdecl_type_body(struct _RegionHandle*d,struct _tuple17*env){
# 158
struct _tuple17 _tmp18=*env;struct _tuple17 _tmp19=_tmp18;struct Cyc_Toc_TocState*_tmp1A;struct _tuple1*_tmp1B;void*(*_tmp1C)(struct _tuple1*);_LLA: _tmp1A=_tmp19.f1;_tmp1B=(_tmp19.f2)->f1;_tmp1C=(_tmp19.f2)->f2;_LLB:;{
struct _tuple18**v=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp1A->aggrs_so_far,_tmp1B);
if(v == 0)
return _tmp1C(_tmp1B);else{
# 163
struct _tuple18*_tmp1D=*v;struct _tuple18*_tmp1E=_tmp1D;void*_tmp1F;_LLD: _tmp1F=_tmp1E->f2;_LLE:;
return _tmp1F;}};}
# 168
void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){
struct _tuple16 _tmp924;struct _tuple16 env=(_tmp924.f1=q,((_tmp924.f2=type_maker,_tmp924)));
return((void*(*)(struct _tuple16*arg,void*(*f)(struct _RegionHandle*,struct _tuple17*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_aggrdecl_type_body);}
# 173
static int Cyc_Toc_tuple_type_counter=0;
static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;char Cyc_Toc_Toc_Unimplemented[18]="Toc_Unimplemented";struct Cyc_Toc_Toc_Unimplemented_exn_struct{char*tag;};char Cyc_Toc_Toc_Impossible[15]="Toc_Impossible";struct Cyc_Toc_Toc_Impossible_exn_struct{char*tag;};
# 180
static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 183
Cyc_vfprintf(Cyc_stderr,fmt,ap);
{const char*_tmp927;void*_tmp926;(_tmp926=0,Cyc_fprintf(Cyc_stderr,((_tmp927="\n",_tag_dyneither(_tmp927,sizeof(char),2))),_tag_dyneither(_tmp926,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Unimplemented_exn_struct _tmp92A;struct Cyc_Toc_Toc_Unimplemented_exn_struct*_tmp929;(int)_throw((void*)((_tmp929=_cycalloc_atomic(sizeof(*_tmp929)),((_tmp929[0]=((_tmp92A.tag=Cyc_Toc_Toc_Unimplemented,_tmp92A)),_tmp929)))));};}
# 188
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 191
Cyc_vfprintf(Cyc_stderr,fmt,ap);
{const char*_tmp92D;void*_tmp92C;(_tmp92C=0,Cyc_fprintf(Cyc_stderr,((_tmp92D="\n",_tag_dyneither(_tmp92D,sizeof(char),2))),_tag_dyneither(_tmp92C,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Impossible_exn_struct _tmp930;struct Cyc_Toc_Toc_Impossible_exn_struct*_tmp92F;(int)_throw((void*)((_tmp92F=_cycalloc_atomic(sizeof(*_tmp92F)),((_tmp92F[0]=((_tmp930.tag=Cyc_Toc_Toc_Impossible,_tmp930)),_tmp92F)))));};}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_exn_struct{char*tag;};static char _tmp29[5]="curr";
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
void*_tmpC0=e->r;void*_tmpC1=_tmpC0;struct Cyc_Absyn_Exp*_tmpC2;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpC1)->tag == 13){if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpC1)->f4 == Cyc_Absyn_No_coercion){_LL10: _tmpC2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpC1)->f2;_LL11:
 return Cyc_Toc_cast_it(t,_tmpC2);}else{goto _LL12;}}else{_LL12: _LL13:
 return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}_LLF:;}
# 322
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp933;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp932;return(void*)((_tmp932=_cycalloc(sizeof(*_tmp932)),((_tmp932[0]=((_tmp933.tag=13,((_tmp933.f1=t,((_tmp933.f2=e,((_tmp933.f3=0,((_tmp933.f4=Cyc_Absyn_No_coercion,_tmp933)))))))))),_tmp932))));}
# 325
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmp936;struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp935;return(void*)((_tmp935=_cycalloc(sizeof(*_tmp935)),((_tmp935[0]=((_tmp936.tag=19,((_tmp936.f1=e,_tmp936)))),_tmp935))));}
# 328
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmp939;struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp938;return(void*)((_tmp938=_cycalloc(sizeof(*_tmp938)),((_tmp938[0]=((_tmp939.tag=22,((_tmp939.f1=e1,((_tmp939.f2=e2,_tmp939)))))),_tmp938))));}
# 331
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmp93C;struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp93B;return(void*)((_tmp93B=_cycalloc(sizeof(*_tmp93B)),((_tmp93B[0]=((_tmp93C.tag=36,((_tmp93C.f1=s,_tmp93C)))),_tmp93B))));}
# 334
static void*Cyc_Toc_sizeoftyp_exp_r(void*t){
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp93F;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp93E;return(void*)((_tmp93E=_cycalloc(sizeof(*_tmp93E)),((_tmp93E[0]=((_tmp93F.tag=16,((_tmp93F.f1=t,_tmp93F)))),_tmp93E))));}
# 337
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp942;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp941;return(void*)((_tmp941=_cycalloc(sizeof(*_tmp941)),((_tmp941[0]=((_tmp942.tag=9,((_tmp942.f1=e,((_tmp942.f2=es,((_tmp942.f3=0,((_tmp942.f4=1,_tmp942)))))))))),_tmp941))));}
# 340
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmp945;struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp944;return(void*)((_tmp944=_cycalloc(sizeof(*_tmp944)),((_tmp944[0]=((_tmp945.tag=1,((_tmp945.f1=e,_tmp945)))),_tmp944))));}
# 343
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp948;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp947;return(void*)((_tmp947=_cycalloc(sizeof(*_tmp947)),((_tmp947[0]=((_tmp948.tag=2,((_tmp948.f1=s1,((_tmp948.f2=s2,_tmp948)))))),_tmp947))));}
# 346
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp94B;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp94A;return(void*)((_tmp94A=_cycalloc(sizeof(*_tmp94A)),((_tmp94A[0]=((_tmp94B.tag=5,((_tmp94B.f1=e1,((_tmp94B.f2=e2,((_tmp94B.f3=e3,_tmp94B)))))))),_tmp94A))));}
# 349
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmp94E;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp94D;return(void*)((_tmp94D=_cycalloc(sizeof(*_tmp94D)),((_tmp94D[0]=((_tmp94E.tag=20,((_tmp94E.f1=e,((_tmp94E.f2=n,((_tmp94E.f3=0,((_tmp94E.f4=0,_tmp94E)))))))))),_tmp94D))));}
# 352
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmp951;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp950;return(void*)((_tmp950=_cycalloc(sizeof(*_tmp950)),((_tmp950[0]=((_tmp951.tag=21,((_tmp951.f1=e,((_tmp951.f2=n,((_tmp951.f3=0,((_tmp951.f4=0,_tmp951)))))))))),_tmp950))));}
# 355
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 358
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp954;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp953;return(void*)((_tmp953=_cycalloc(sizeof(*_tmp953)),((_tmp953[0]=((_tmp954.tag=35,((_tmp954.f1=tdopt,((_tmp954.f2=ds,_tmp954)))))),_tmp953))));}
# 360
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmp957;struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp956;return(void*)((_tmp956=_cycalloc(sizeof(*_tmp956)),((_tmp956[0]=((_tmp957.tag=8,((_tmp957.f1=v,((_tmp957.f2=s,_tmp957)))))),_tmp956))));}
# 363
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 367
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,unsigned int loc){
void*_tmpDE=e->r;void*_tmpDF=_tmpDE;struct Cyc_Absyn_Exp*_tmpE0;if(((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpDF)->tag == 19){_LL15: _tmpE0=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpDF)->f1;_LL16:
 return Cyc_Absyn_aggrarrow_exp(_tmpE0,f,loc);}else{_LL17: _LL18:
 return Cyc_Absyn_aggrmember_exp(e,f,loc);}_LL14:;}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
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
{void*_tmpE1=Cyc_Tcutil_compress(t);void*_tmpE2=_tmpE1;enum Cyc_Absyn_Size_of _tmpE3;switch(*((int*)_tmpE2)){case 6: _LL1A: _tmpE3=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpE2)->f2;_LL1B:
# 426
{enum Cyc_Absyn_Size_of _tmpE4=_tmpE3;switch(_tmpE4){case Cyc_Absyn_Char_sz: _LL27: _LL28:
 function=fS->fchar;goto _LL26;case Cyc_Absyn_Short_sz: _LL29: _LL2A:
 function=fS->fshort;goto _LL26;case Cyc_Absyn_Int_sz: _LL2B: _LL2C:
 function=fS->fint;goto _LL26;default: _LL2D: _LL2E: {
struct Cyc_Core_Impossible_exn_struct _tmp95D;const char*_tmp95C;struct Cyc_Core_Impossible_exn_struct*_tmp95B;(int)_throw((void*)((_tmp95B=_cycalloc(sizeof(*_tmp95B)),((_tmp95B[0]=((_tmp95D.tag=Cyc_Core_Impossible,((_tmp95D.f1=((_tmp95C="impossible IntType (not char, short or int)",_tag_dyneither(_tmp95C,sizeof(char),44))),_tmp95D)))),_tmp95B)))));}}_LL26:;}
# 432
goto _LL19;case 7: switch(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpE2)->f1){case 0: _LL1C: _LL1D:
# 434
 function=fS->ffloat;
goto _LL19;case 1: _LL1E: _LL1F:
# 437
 function=fS->fdouble;
goto _LL19;default: _LL20: _LL21:
# 440
 function=fS->flongdouble;
goto _LL19;}case 5: _LL22: _LL23:
# 443
 function=fS->fvoidstar;
goto _LL19;default: _LL24: _LL25: {
# 446
struct Cyc_Core_Impossible_exn_struct _tmp96A;const char*_tmp969;void*_tmp968[1];struct Cyc_String_pa_PrintArg_struct _tmp967;struct Cyc_Core_Impossible_exn_struct*_tmp966;(int)_throw((void*)((_tmp966=_cycalloc(sizeof(*_tmp966)),((_tmp966[0]=((_tmp96A.tag=Cyc_Core_Impossible,((_tmp96A.f1=(struct _dyneither_ptr)((_tmp967.tag=0,((_tmp967.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp968[0]=& _tmp967,Cyc_aprintf(((_tmp969="impossible expression type %s (not int, float, double, or pointer)",_tag_dyneither(_tmp969,sizeof(char),67))),_tag_dyneither(_tmp968,sizeof(void*),1)))))))),_tmp96A)))),_tmp966)))));}}_LL19:;}
# 448
return function;}
# 450
struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
return Cyc_Toc_getFunctionType(fS,(void*)_check_null(arr->topt));}
# 453
struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmpED=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_tmpEE=_tmpED;void*_tmpEF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpEE)->tag == 5){_LL30: _tmpEF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpEE)->f1).elt_typ;_LL31:
# 456
 return Cyc_Toc_getFunctionType(fS,_tmpEF);}else{_LL32: _LL33: {
struct Cyc_Core_Impossible_exn_struct _tmp970;const char*_tmp96F;struct Cyc_Core_Impossible_exn_struct*_tmp96E;(int)_throw((void*)((_tmp96E=_cycalloc(sizeof(*_tmp96E)),((_tmp96E[0]=((_tmp970.tag=Cyc_Core_Impossible,((_tmp970.f1=((_tmp96F="impossible type (not pointer)",_tag_dyneither(_tmp96F,sizeof(char),30))),_tmp970)))),_tmp96E)))));}}_LL2F:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 464
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmpF3=e->r;void*_tmpF4=_tmpF3;struct Cyc_List_List*_tmpF5;struct Cyc_List_List*_tmpF6;struct Cyc_List_List*_tmpF7;struct Cyc_List_List*_tmpF8;struct Cyc_List_List*_tmpF9;struct Cyc_Absyn_Exp*_tmpFA;long long _tmpFB;int _tmpFC;short _tmpFD;struct _dyneither_ptr _tmpFE;char _tmpFF;switch(*((int*)_tmpF4)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF4)->f1).Null_c).tag){case 2: _LL35: _tmpFF=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF4)->f1).Char_c).val).f2;_LL36:
 return _tmpFF == '\000';case 3: _LL37: _tmpFE=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF4)->f1).Wchar_c).val;_LL38: {
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
# 489
 for(0;_tmpF5 != 0;_tmpF5=_tmpF5->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple19*)_tmpF5->hd)).f2))return 0;}
return 1;default: _LL4D: _LL4E:
 return 0;}_LL34:;}
# 497
static int Cyc_Toc_is_nullable(void*t){
void*_tmp101=Cyc_Tcutil_compress(t);void*_tmp102=_tmp101;union Cyc_Absyn_Constraint*_tmp103;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp102)->tag == 5){_LL50: _tmp103=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp102)->f1).ptr_atts).nullable;_LL51:
# 500
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp103);}else{_LL52: _LL53: {
const char*_tmp973;void*_tmp972;(_tmp972=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp973="is_nullable",_tag_dyneither(_tmp973,sizeof(char),12))),_tag_dyneither(_tmp972,sizeof(void*),0)));}}_LL4F:;}
# 506
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
# 519
{struct Cyc_List_List*_tmp10D=nms;for(0;_tmp10D != 0;_tmp10D=_tmp10D->tl){
len +=1 + Cyc_strlen((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp10D->hd));}}{
char*_tmp975;unsigned int _tmp974;struct _dyneither_ptr buf=(_tmp974=len,((_tmp975=_cyccalloc_atomic(sizeof(char),_tmp974),_tag_dyneither(_tmp975,sizeof(char),_tmp974))));
struct _dyneither_ptr p=buf;
Cyc_strcpy(p,(struct _dyneither_ptr)s);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)slen);
for(0;nms != 0;nms=nms->tl){
struct _dyneither_ptr*_tmp10E=(struct _dyneither_ptr*)nms->hd;struct _dyneither_ptr*_tmp10F=_tmp10E;struct _dyneither_ptr _tmp110;_LL61: _tmp110=*_tmp10F;_LL62:;
{char _tmp978;char _tmp977;struct _dyneither_ptr _tmp976;(_tmp976=p,((_tmp977=*((char*)_check_dyneither_subscript(_tmp976,sizeof(char),0)),((_tmp978='_',((_get_dyneither_size(_tmp976,sizeof(char))== 1  && (_tmp977 == '\000'  && _tmp978 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp976.curr)=_tmp978)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp110);
_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)Cyc_strlen((struct _dyneither_ptr)_tmp110));}
# 530
{char _tmp97B;char _tmp97A;struct _dyneither_ptr _tmp979;(_tmp979=p,((_tmp97A=*((char*)_check_dyneither_subscript(_tmp979,sizeof(char),0)),((_tmp97B='_',((_get_dyneither_size(_tmp979,sizeof(char))== 1  && (_tmp97A == '\000'  && _tmp97B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp979.curr)=_tmp97B)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp108);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)vlen);
{const char*_tmp97C;Cyc_strcpy(p,((_tmp97C="_struct",_tag_dyneither(_tmp97C,sizeof(char),8))));}
return(struct _dyneither_ptr)buf;};};};}struct _tuple20{struct Cyc_Toc_TocState*f1;struct _tuple14*f2;};static char _tmp12F[8]="*bogus*";
# 542
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple20*env){
# 545
static struct _dyneither_ptr bogus_string={_tmp12F,_tmp12F,_tmp12F + 8};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
static struct _tuple14 pair={& bogus_qvar,& bogus_qvar};
# 549
struct _tuple20 _tmp11A=*env;struct _tuple20 _tmp11B=_tmp11A;struct Cyc_Dict_Dict*_tmp11C;struct _tuple14*_tmp11D;_LL64: _tmp11C=(_tmp11B.f1)->qvar_tags;_tmp11D=_tmp11B.f2;_LL65:;{
struct _tuple14 _tmp11E=*_tmp11D;struct _tuple14 _tmp11F=_tmp11E;struct _tuple1*_tmp120;struct _tuple1*_tmp121;_LL67: _tmp120=_tmp11F.f1;_tmp121=_tmp11F.f2;_LL68:;{
struct _handler_cons _tmp122;_push_handler(& _tmp122);{int _tmp124=0;if(setjmp(_tmp122.handler))_tmp124=1;if(!_tmp124){
{struct _tuple1*_tmp125=((struct _tuple1*(*)(struct Cyc_Dict_Dict d,int(*cmp)(struct _tuple14*,struct _tuple14*),struct _tuple14*k))Cyc_Dict_lookup_other)(*_tmp11C,Cyc_Toc_qvar_tag_cmp,_tmp11D);_npop_handler(0);return _tmp125;};_pop_handler();}else{void*_tmp123=(void*)_exn_thrown;void*_tmp126=_tmp123;void*_tmp127;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp126)->tag == Cyc_Dict_Absent){_LL6A: _LL6B: {
# 555
struct _tuple14*_tmp97D;struct _tuple14*_tmp128=(_tmp97D=_cycalloc(sizeof(*_tmp97D)),((_tmp97D->f1=_tmp120,((_tmp97D->f2=_tmp121,_tmp97D)))));
struct _tuple1*_tmp129=_tmp120;union Cyc_Absyn_Nmspace _tmp12A;struct _dyneither_ptr _tmp12B;_LL6F: _tmp12A=_tmp129->f1;_tmp12B=*_tmp129->f2;_LL70:;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmp12B,_tmp121);
struct _dyneither_ptr*_tmp980;struct _tuple1*_tmp97F;struct _tuple1*res=(_tmp97F=_cycalloc(sizeof(*_tmp97F)),((_tmp97F->f1=_tmp12A,((_tmp97F->f2=((_tmp980=_cycalloc(sizeof(*_tmp980)),((_tmp980[0]=newvar,_tmp980)))),_tmp97F)))));
*_tmp11C=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmp11C,_tmp128,res);
return res;};}}else{_LL6C: _tmp127=_tmp126;_LL6D:(int)_rethrow(_tmp127);}_LL69:;}};};};}
# 564
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple14 _tmp981;struct _tuple14 env=(_tmp981.f1=fieldname,((_tmp981.f2=dtname,_tmp981)));
return((struct _tuple1*(*)(struct _tuple14*arg,struct _tuple1*(*f)(struct _RegionHandle*,struct _tuple20*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_collapse_qvars_body);}
# 569
static void*Cyc_Toc_typ_to_c(void*t);struct _tuple21{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple22{void*f1;struct Cyc_List_List*f2;};
# 573
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple21*env){
# 576
struct _tuple21 _tmp131=*env;struct _tuple21 _tmp132=_tmp131;struct Cyc_List_List**_tmp133;struct Cyc_List_List*_tmp134;_LL72: _tmp133=(_tmp132.f1)->tuple_types;_tmp134=_tmp132.f2;_LL73:;
# 578
{struct Cyc_List_List*_tmp135=*_tmp133;for(0;_tmp135 != 0;_tmp135=_tmp135->tl){
struct _tuple22*_tmp136=(struct _tuple22*)_tmp135->hd;struct _tuple22*_tmp137=_tmp136;void*_tmp138;struct Cyc_List_List*_tmp139;_LL75: _tmp138=_tmp137->f1;_tmp139=_tmp137->f2;_LL76:;{
struct Cyc_List_List*_tmp13A=_tmp134;
for(0;_tmp13A != 0  && _tmp139 != 0;(_tmp13A=_tmp13A->tl,_tmp139=_tmp139->tl)){
if(!Cyc_Tcutil_unify((*((struct _tuple11*)_tmp13A->hd)).f2,(void*)_tmp139->hd))
break;}
if(_tmp13A == 0  && _tmp139 == 0)
return _tmp138;};}}{
# 588
struct Cyc_Int_pa_PrintArg_struct _tmp989;void*_tmp988[1];const char*_tmp987;struct _dyneither_ptr*_tmp986;struct _dyneither_ptr*xname=(_tmp986=_cycalloc(sizeof(*_tmp986)),((_tmp986[0]=(struct _dyneither_ptr)((_tmp989.tag=1,((_tmp989.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmp988[0]=& _tmp989,Cyc_aprintf(((_tmp987="_tuple%d",_tag_dyneither(_tmp987,sizeof(char),9))),_tag_dyneither(_tmp988,sizeof(void*),1)))))))),_tmp986)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,_tmp134);
struct Cyc_List_List*_tmp13B=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
struct Cyc_Absyn_Aggrfield*_tmp98C;struct Cyc_List_List*_tmp98B;_tmp13B=((_tmp98B=_cycalloc(sizeof(*_tmp98B)),((_tmp98B->hd=((_tmp98C=_cycalloc(sizeof(*_tmp98C)),((_tmp98C->name=Cyc_Absyn_fieldname(i),((_tmp98C->tq=Cyc_Toc_mt_tq,((_tmp98C->type=(void*)ts2->hd,((_tmp98C->width=0,((_tmp98C->attributes=0,((_tmp98C->requires_clause=0,_tmp98C)))))))))))))),((_tmp98B->tl=_tmp13B,_tmp98B))))));}}
_tmp13B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp13B);{
struct Cyc_Absyn_AggrdeclImpl*_tmp991;struct _tuple1*_tmp990;struct Cyc_Absyn_Aggrdecl*_tmp98F;struct Cyc_Absyn_Aggrdecl*_tmp13E=(_tmp98F=_cycalloc(sizeof(*_tmp98F)),((_tmp98F->kind=Cyc_Absyn_StructA,((_tmp98F->sc=Cyc_Absyn_Public,((_tmp98F->name=(
(_tmp990=_cycalloc(sizeof(*_tmp990)),((_tmp990->f1=Cyc_Absyn_Rel_n(0),((_tmp990->f2=xname,_tmp990)))))),((_tmp98F->tvs=0,((_tmp98F->impl=(
(_tmp991=_cycalloc(sizeof(*_tmp991)),((_tmp991->exist_vars=0,((_tmp991->rgn_po=0,((_tmp991->fields=_tmp13B,((_tmp991->tagged=0,_tmp991)))))))))),((_tmp98F->attributes=0,((_tmp98F->expected_mem_kind=0,_tmp98F)))))))))))))));
# 603
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp997;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp996;struct Cyc_List_List*_tmp995;Cyc_Toc_result_decls=((_tmp995=_cycalloc(sizeof(*_tmp995)),((_tmp995->hd=Cyc_Absyn_new_decl((void*)((_tmp997=_cycalloc(sizeof(*_tmp997)),((_tmp997[0]=((_tmp996.tag=5,((_tmp996.f1=_tmp13E,_tmp996)))),_tmp997)))),0),((_tmp995->tl=Cyc_Toc_result_decls,_tmp995))))));}
{struct _tuple22*_tmp99A;struct Cyc_List_List*_tmp999;*_tmp133=((_tmp999=_region_malloc(d,sizeof(*_tmp999)),((_tmp999->hd=((_tmp99A=_region_malloc(d,sizeof(*_tmp99A)),((_tmp99A->f1=x,((_tmp99A->f2=ts,_tmp99A)))))),((_tmp999->tl=*_tmp133,_tmp999))))));}
return x;};};}
# 608
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return((void*(*)(struct Cyc_List_List*arg,void*(*f)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state)(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple23{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple24{struct Cyc_Toc_TocState*f1;struct _tuple23*f2;};struct _tuple25{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 617
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple24*env){
# 625
struct _tuple24 _tmp14B=*env;struct _tuple24 _tmp14C=_tmp14B;struct Cyc_List_List**_tmp14D;struct _tuple1*_tmp14E;struct Cyc_List_List*_tmp14F;struct Cyc_List_List*_tmp150;struct Cyc_List_List*_tmp151;_LL78: _tmp14D=(_tmp14C.f1)->abs_struct_types;_tmp14E=(_tmp14C.f2)->f1;_tmp14F=(_tmp14C.f2)->f2;_tmp150=(_tmp14C.f2)->f3;_tmp151=(_tmp14C.f2)->f4;_LL79:;
# 630
{struct Cyc_List_List*_tmp152=*_tmp14D;for(0;_tmp152 != 0;_tmp152=_tmp152->tl){
struct _tuple25*_tmp153=(struct _tuple25*)_tmp152->hd;struct _tuple25*_tmp154=_tmp153;struct _tuple1*_tmp155;struct Cyc_List_List*_tmp156;void*_tmp157;_LL7B: _tmp155=_tmp154->f1;_tmp156=_tmp154->f2;_tmp157=_tmp154->f3;_LL7C:;
# 633
if(Cyc_Absyn_qvar_cmp(_tmp155,_tmp14E)== 0  && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp150)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp156)){
int okay=1;
{struct Cyc_List_List*_tmp158=_tmp150;for(0;_tmp158 != 0;(_tmp158=_tmp158->tl,_tmp156=_tmp156->tl)){
void*_tmp159=(void*)_tmp158->hd;
void*_tmp15A=(void*)((struct Cyc_List_List*)_check_null(_tmp156))->hd;
# 640
{struct Cyc_Absyn_Kind*_tmp15B=Cyc_Tcutil_typ_kind(_tmp159);struct Cyc_Absyn_Kind*_tmp15C=_tmp15B;switch(((struct Cyc_Absyn_Kind*)_tmp15C)->kind){case Cyc_Absyn_EffKind: _LL7E: _LL7F:
 goto _LL81;case Cyc_Absyn_RgnKind: _LL80: _LL81:
# 645
 continue;default: _LL82: _LL83:
# 648
 if(Cyc_Tcutil_unify(_tmp159,_tmp15A) || Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(_tmp159),Cyc_Toc_typ_to_c(_tmp15A)))
# 650
continue;
# 653
okay=0;
goto _LL7D;}_LL7D:;}
# 656
break;}}
# 658
if(okay)
# 660
return _tmp157;}}}{
# 668
struct Cyc_Int_pa_PrintArg_struct _tmp9A2;void*_tmp9A1[1];const char*_tmp9A0;struct _dyneither_ptr*_tmp99F;struct _dyneither_ptr*xname=(_tmp99F=_cycalloc(sizeof(*_tmp99F)),((_tmp99F[0]=(struct _dyneither_ptr)((_tmp9A2.tag=1,((_tmp9A2.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmp9A1[0]=& _tmp9A2,Cyc_aprintf(((_tmp9A0="_tuple%d",_tag_dyneither(_tmp9A0,sizeof(char),9))),_tag_dyneither(_tmp9A1,sizeof(void*),1)))))))),_tmp99F)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp15D=0;
# 672
{struct _tuple25*_tmp9A5;struct Cyc_List_List*_tmp9A4;*_tmp14D=((_tmp9A4=_region_malloc(d,sizeof(*_tmp9A4)),((_tmp9A4->hd=((_tmp9A5=_region_malloc(d,sizeof(*_tmp9A5)),((_tmp9A5->f1=_tmp14E,((_tmp9A5->f2=_tmp150,((_tmp9A5->f3=x,_tmp9A5)))))))),((_tmp9A4->tl=*_tmp14D,_tmp9A4))))));}{
# 675
struct _RegionHandle _tmp160=_new_region("r");struct _RegionHandle*r=& _tmp160;_push_region(r);
{struct Cyc_List_List*_tmp161=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp14F,_tmp150);
for(0;_tmp151 != 0;_tmp151=_tmp151->tl){
struct Cyc_Absyn_Aggrfield*_tmp162=(struct Cyc_Absyn_Aggrfield*)_tmp151->hd;
void*t=_tmp162->type;
struct Cyc_List_List*atts=_tmp162->attributes;
# 683
if(_tmp151->tl == 0  && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(t))){
struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp9AB;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmp9AA;struct Cyc_List_List*_tmp9A9;atts=((_tmp9A9=_cycalloc(sizeof(*_tmp9A9)),((_tmp9A9->hd=(void*)((_tmp9AB=_cycalloc(sizeof(*_tmp9AB)),((_tmp9AB[0]=((_tmp9AA.tag=6,((_tmp9AA.f1=0,_tmp9AA)))),_tmp9AB)))),((_tmp9A9->tl=atts,_tmp9A9))))));}
# 686
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp161,t));
# 689
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val)){
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp9B1;struct Cyc_Absyn_ArrayInfo _tmp9B0;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp9AF;t=(void*)((_tmp9AF=_cycalloc(sizeof(*_tmp9AF)),((_tmp9AF[0]=((_tmp9B1.tag=8,((_tmp9B1.f1=((_tmp9B0.elt_type=Cyc_Absyn_void_star_typ(),((_tmp9B0.tq=Cyc_Absyn_empty_tqual(0),((_tmp9B0.num_elts=
Cyc_Absyn_uint_exp(0,0),((_tmp9B0.zero_term=((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0),((_tmp9B0.zt_loc=0,_tmp9B0)))))))))),_tmp9B1)))),_tmp9AF))));}{
# 693
struct Cyc_Absyn_Aggrfield*_tmp9B4;struct Cyc_List_List*_tmp9B3;_tmp15D=((_tmp9B3=_cycalloc(sizeof(*_tmp9B3)),((_tmp9B3->hd=((_tmp9B4=_cycalloc(sizeof(*_tmp9B4)),((_tmp9B4->name=_tmp162->name,((_tmp9B4->tq=Cyc_Toc_mt_tq,((_tmp9B4->type=t,((_tmp9B4->width=_tmp162->width,((_tmp9B4->attributes=atts,((_tmp9B4->requires_clause=0,_tmp9B4)))))))))))))),((_tmp9B3->tl=_tmp15D,_tmp9B3))))));};}
# 695
_tmp15D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp15D);{
struct Cyc_Absyn_AggrdeclImpl*_tmp9B9;struct _tuple1*_tmp9B8;struct Cyc_Absyn_Aggrdecl*_tmp9B7;struct Cyc_Absyn_Aggrdecl*_tmp16B=(_tmp9B7=_cycalloc(sizeof(*_tmp9B7)),((_tmp9B7->kind=Cyc_Absyn_StructA,((_tmp9B7->sc=Cyc_Absyn_Public,((_tmp9B7->name=(
(_tmp9B8=_cycalloc(sizeof(*_tmp9B8)),((_tmp9B8->f1=Cyc_Absyn_Rel_n(0),((_tmp9B8->f2=xname,_tmp9B8)))))),((_tmp9B7->tvs=0,((_tmp9B7->impl=(
(_tmp9B9=_cycalloc(sizeof(*_tmp9B9)),((_tmp9B9->exist_vars=0,((_tmp9B9->rgn_po=0,((_tmp9B9->fields=_tmp15D,((_tmp9B9->tagged=0,_tmp9B9)))))))))),((_tmp9B7->attributes=0,((_tmp9B7->expected_mem_kind=0,_tmp9B7)))))))))))))));
# 703
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp9BF;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp9BE;struct Cyc_List_List*_tmp9BD;Cyc_Toc_result_decls=((_tmp9BD=_cycalloc(sizeof(*_tmp9BD)),((_tmp9BD->hd=Cyc_Absyn_new_decl((void*)((_tmp9BF=_cycalloc(sizeof(*_tmp9BF)),((_tmp9BF[0]=((_tmp9BE.tag=5,((_tmp9BE.f1=_tmp16B,_tmp9BE)))),_tmp9BF)))),0),((_tmp9BD->tl=Cyc_Toc_result_decls,_tmp9BD))))));}{
void*_tmp16F=x;_npop_handler(0);return _tmp16F;};};}
# 676
;_pop_region(r);};};}
# 707
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 711
struct _tuple23 _tmp9C0;struct _tuple23 env=(_tmp9C0.f1=struct_name,((_tmp9C0.f2=type_vars,((_tmp9C0.f3=type_args,((_tmp9C0.f4=fields,_tmp9C0)))))));
return((void*(*)(struct _tuple23*arg,void*(*f)(struct _RegionHandle*,struct _tuple24*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 718
struct _tuple1*Cyc_Toc_temp_var(){
int _tmp178=Cyc_Toc_temp_var_counter ++;
struct _dyneither_ptr*_tmp9CD;const char*_tmp9CC;void*_tmp9CB[1];struct Cyc_Int_pa_PrintArg_struct _tmp9CA;struct _tuple1*_tmp9C9;struct _tuple1*res=(_tmp9C9=_cycalloc(sizeof(*_tmp9C9)),((_tmp9C9->f1=Cyc_Absyn_Loc_n,((_tmp9C9->f2=((_tmp9CD=_cycalloc(sizeof(*_tmp9CD)),((_tmp9CD[0]=(struct _dyneither_ptr)((_tmp9CA.tag=1,((_tmp9CA.f1=(unsigned int)_tmp178,((_tmp9CB[0]=& _tmp9CA,Cyc_aprintf(((_tmp9CC="_tmp%X",_tag_dyneither(_tmp9CC,sizeof(char),7))),_tag_dyneither(_tmp9CB,sizeof(void*),1)))))))),_tmp9CD)))),_tmp9C9)))));
return res;}struct _tuple26{struct Cyc_Toc_TocState*f1;int f2;};
# 726
static struct _dyneither_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple26*env){
struct _tuple26 _tmp17E=*env;struct _tuple26 _tmp17F=_tmp17E;struct Cyc_Xarray_Xarray*_tmp180;_LL85: _tmp180=(_tmp17F.f1)->temp_labels;_LL86:;{
int _tmp181=Cyc_Toc_fresh_label_counter ++;
if(_tmp181 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp180))
return((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp180,_tmp181);{
struct Cyc_Int_pa_PrintArg_struct _tmp9D5;void*_tmp9D4[1];const char*_tmp9D3;struct _dyneither_ptr*_tmp9D2;struct _dyneither_ptr*res=(_tmp9D2=_cycalloc(sizeof(*_tmp9D2)),((_tmp9D2[0]=(struct _dyneither_ptr)((_tmp9D5.tag=1,((_tmp9D5.f1=(unsigned int)_tmp181,((_tmp9D4[0]=& _tmp9D5,Cyc_aprintf(((_tmp9D3="_LL%X",_tag_dyneither(_tmp9D3,sizeof(char),6))),_tag_dyneither(_tmp9D4,sizeof(void*),1)))))))),_tmp9D2)));
if(((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp180,res)!= _tmp181){
const char*_tmp9D8;void*_tmp9D7;(_tmp9D7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp9D8="fresh_label: add_ind returned bad index...",_tag_dyneither(_tmp9D8,sizeof(char),43))),_tag_dyneither(_tmp9D7,sizeof(void*),0)));}
return res;};};}
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
struct _tuple9 _tmp189=*a;struct _tuple9 _tmp18A=_tmp189;struct _dyneither_ptr*_tmp18B;struct Cyc_Absyn_Tqual _tmp18C;void*_tmp18D;_LL88: _tmp18B=_tmp18A.f1;_tmp18C=_tmp18A.f2;_tmp18D=_tmp18A.f3;_LL89:;{
struct _tuple9*_tmp9D9;return(_tmp9D9=_cycalloc(sizeof(*_tmp9D9)),((_tmp9D9->f1=_tmp18B,((_tmp9D9->f2=_tmp18C,((_tmp9D9->f3=Cyc_Toc_typ_to_c(_tmp18D),_tmp9D9)))))));};}
# 765
static struct _tuple11*Cyc_Toc_typ_to_c_f(struct _tuple11*x){
struct _tuple11 _tmp18F=*x;struct _tuple11 _tmp190=_tmp18F;struct Cyc_Absyn_Tqual _tmp191;void*_tmp192;_LL8B: _tmp191=_tmp190.f1;_tmp192=_tmp190.f2;_LL8C:;{
struct _tuple11*_tmp9DA;return(_tmp9DA=_cycalloc(sizeof(*_tmp9DA)),((_tmp9DA->f1=_tmp191,((_tmp9DA->f2=Cyc_Toc_typ_to_c(_tmp192),_tmp9DA)))));};}
# 784 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp194=Cyc_Tcutil_compress(t);void*_tmp195=_tmp194;void*_tmp196;void*_tmp197;struct Cyc_Absyn_Tqual _tmp198;struct Cyc_Absyn_Exp*_tmp199;union Cyc_Absyn_Constraint*_tmp19A;unsigned int _tmp19B;switch(*((int*)_tmp195)){case 8: _LL8E: _tmp197=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp195)->f1).elt_type;_tmp198=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp195)->f1).tq;_tmp199=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp195)->f1).num_elts;_tmp19A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp195)->f1).zero_term;_tmp19B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp195)->f1).zt_loc;_LL8F:
# 787
 return Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(_tmp197),_tmp198);case 1: _LL90: _tmp196=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp195)->f2;if(_tmp196 != 0){_LL91:
 return Cyc_Toc_typ_to_c_array(_tmp196);}else{goto _LL92;}default: _LL92: _LL93:
 return Cyc_Toc_typ_to_c(t);}_LL8D:;}
# 793
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){
# 795
struct Cyc_Absyn_Aggrfield*_tmp9DB;return(_tmp9DB=_cycalloc(sizeof(*_tmp9DB)),((_tmp9DB->name=f->name,((_tmp9DB->tq=Cyc_Toc_mt_tq,((_tmp9DB->type=
# 797
Cyc_Toc_typ_to_c(f->type),((_tmp9DB->width=f->width,((_tmp9DB->attributes=f->attributes,((_tmp9DB->requires_clause=0,_tmp9DB)))))))))))));}
# 802
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 804
return;}
# 807
static void*Cyc_Toc_char_star_typ(){
static void**cs=0;
if(cs == 0){
void**_tmp9DC;cs=((_tmp9DC=_cycalloc(sizeof(*_tmp9DC)),((_tmp9DC[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmp9DC))));}
# 812
return*cs;}
# 814
static void*Cyc_Toc_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmp9DD;r=((_tmp9DD=_cycalloc(sizeof(*_tmp9DD)),((_tmp9DD[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),_tmp9DD))));}
# 819
return*r;}
# 821
static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmp9DE;r=((_tmp9DE=_cycalloc(sizeof(*_tmp9DE)),((_tmp9DE[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),_tmp9DE))));}
# 826
return*r;}
# 828
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp1A0=Cyc_Tcutil_compress(t);void*_tmp1A1=_tmp1A0;struct Cyc_Absyn_Tvar*_tmp1A2;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A1)->tag == 2){_LL95: _tmp1A2=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A1)->f1;_LL96:
# 831
 return Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tbk);}else{_LL97: _LL98:
 return 0;}_LL94:;}
# 835
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp1A3=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp1A4=_tmp1A3;if(((struct Cyc_Absyn_Kind*)_tmp1A4)->kind == Cyc_Absyn_AnyKind){_LL9A: _LL9B:
 return 1;}else{_LL9C: _LL9D:
 return 0;}_LL99:;}
# 842
static void*Cyc_Toc_typ_to_c(void*t){
void*_tmp1A5=t;struct Cyc_Absyn_Datatypedecl*_tmp1A6;void**_tmp1A7;struct Cyc_Absyn_Enumdecl*_tmp1A8;struct Cyc_Absyn_Aggrdecl*_tmp1A9;struct Cyc_Absyn_Exp*_tmp1AA;struct Cyc_Absyn_Exp*_tmp1AB;void*_tmp1AC;struct _tuple1*_tmp1AD;struct Cyc_List_List*_tmp1AE;struct Cyc_Absyn_Typedefdecl*_tmp1AF;void*_tmp1B0;struct Cyc_List_List*_tmp1B1;struct _tuple1*_tmp1B2;union Cyc_Absyn_AggrInfoU _tmp1B3;struct Cyc_List_List*_tmp1B4;enum Cyc_Absyn_AggrKind _tmp1B5;struct Cyc_List_List*_tmp1B6;struct Cyc_List_List*_tmp1B7;struct Cyc_Absyn_Tqual _tmp1B8;void*_tmp1B9;struct Cyc_List_List*_tmp1BA;int _tmp1BB;struct Cyc_Absyn_VarargInfo*_tmp1BC;struct Cyc_List_List*_tmp1BD;void*_tmp1BE;struct Cyc_Absyn_Tqual _tmp1BF;struct Cyc_Absyn_Exp*_tmp1C0;unsigned int _tmp1C1;void*_tmp1C2;struct Cyc_Absyn_Tqual _tmp1C3;union Cyc_Absyn_Constraint*_tmp1C4;struct Cyc_Absyn_Datatypedecl*_tmp1C5;struct Cyc_Absyn_Datatypefield*_tmp1C6;struct Cyc_Absyn_Tvar*_tmp1C7;void**_tmp1C8;switch(*((int*)_tmp1A5)){case 0: _LL9F: _LLA0:
 return t;case 1: _LLA1: _tmp1C8=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A5)->f2;_LLA2:
# 846
 if(*_tmp1C8 == 0){
*_tmp1C8=Cyc_Absyn_sint_typ;
return Cyc_Absyn_sint_typ;}
# 850
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp1C8));case 2: _LLA3: _tmp1C7=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A5)->f1;_LLA4:
# 852
 if((Cyc_Tcutil_tvar_kind(_tmp1C7,& Cyc_Tcutil_bk))->kind == Cyc_Absyn_AnyKind)
# 855
return(void*)& Cyc_Absyn_VoidType_val;else{
# 857
return Cyc_Absyn_void_star_typ();}case 3: _LLA5: _LLA6:
# 859
 return(void*)& Cyc_Absyn_VoidType_val;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1A5)->f1).field_info).KnownDatatypefield).tag == 2){_LLA7: _tmp1C5=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1A5)->f1).field_info).KnownDatatypefield).val).f1;_tmp1C6=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1A5)->f1).field_info).KnownDatatypefield).val).f2;_LLA8:
# 861
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp1C6->name,_tmp1C5->name));}else{_LLA9: _LLAA: {
const char*_tmp9E1;void*_tmp9E0;(_tmp9E0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmp9E1="unresolved DatatypeFieldType",_tag_dyneither(_tmp9E1,sizeof(char),29))),_tag_dyneither(_tmp9E0,sizeof(void*),0)));}}case 5: _LLAB: _tmp1C2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A5)->f1).elt_typ;_tmp1C3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A5)->f1).elt_tq;_tmp1C4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A5)->f1).ptr_atts).bounds;_LLAC:
# 866
 _tmp1C2=Cyc_Toc_typ_to_c(_tmp1C2);{
void*_tmp1CB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp1C4);void*_tmp1CC=_tmp1CB;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1CC)->tag == 0){_LLE0: _LLE1:
 return Cyc_Toc_dyneither_ptr_typ;}else{_LLE2: _LLE3:
 return Cyc_Absyn_star_typ(_tmp1C2,(void*)& Cyc_Absyn_HeapRgn_val,_tmp1C3,Cyc_Absyn_false_conref);}_LLDF:;};case 6: _LLAD: _LLAE:
# 871
 goto _LLB0;case 7: _LLAF: _LLB0:
 return t;case 8: _LLB1: _tmp1BE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1A5)->f1).elt_type;_tmp1BF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1A5)->f1).tq;_tmp1C0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1A5)->f1).num_elts;_tmp1C1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1A5)->f1).zt_loc;_LLB2:
# 874
 return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c(_tmp1BE),_tmp1BF,_tmp1C0,Cyc_Absyn_false_conref,_tmp1C1);case 9: _LLB3: _tmp1B8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1A5)->f1).ret_tqual;_tmp1B9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1A5)->f1).ret_typ;_tmp1BA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1A5)->f1).args;_tmp1BB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1A5)->f1).c_varargs;_tmp1BC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1A5)->f1).cyc_varargs;_tmp1BD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1A5)->f1).attributes;_LLB4: {
# 880
struct Cyc_List_List*_tmp1CD=0;
for(0;_tmp1BD != 0;_tmp1BD=_tmp1BD->tl){
void*_tmp1CE=(void*)_tmp1BD->hd;void*_tmp1CF=_tmp1CE;switch(*((int*)_tmp1CF)){case 4: _LLE5: _LLE6:
 goto _LLE8;case 5: _LLE7: _LLE8:
 goto _LLEA;case 19: _LLE9: _LLEA:
 continue;case 22: _LLEB: _LLEC:
 continue;case 21: _LLED: _LLEE:
 continue;case 20: _LLEF: _LLF0:
 continue;default: _LLF1: _LLF2:
{struct Cyc_List_List*_tmp9E2;_tmp1CD=((_tmp9E2=_cycalloc(sizeof(*_tmp9E2)),((_tmp9E2->hd=(void*)_tmp1BD->hd,((_tmp9E2->tl=_tmp1CD,_tmp9E2))))));}goto _LLE4;}_LLE4:;}{
# 891
struct Cyc_List_List*_tmp1D1=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp1BA);
if(_tmp1BC != 0){
# 894
void*_tmp1D2=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp1BC->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple9*_tmp9E3;struct _tuple9*_tmp1D3=(_tmp9E3=_cycalloc(sizeof(*_tmp9E3)),((_tmp9E3->f1=_tmp1BC->name,((_tmp9E3->f2=_tmp1BC->tq,((_tmp9E3->f3=_tmp1D2,_tmp9E3)))))));
struct Cyc_List_List*_tmp9E4;_tmp1D1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1D1,((_tmp9E4=_cycalloc(sizeof(*_tmp9E4)),((_tmp9E4->hd=_tmp1D3,((_tmp9E4->tl=0,_tmp9E4)))))));}{
# 898
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp9EA;struct Cyc_Absyn_FnInfo _tmp9E9;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp9E8;return(void*)((_tmp9E8=_cycalloc(sizeof(*_tmp9E8)),((_tmp9E8[0]=((_tmp9EA.tag=9,((_tmp9EA.f1=((_tmp9E9.tvars=0,((_tmp9E9.effect=0,((_tmp9E9.ret_tqual=_tmp1B8,((_tmp9E9.ret_typ=Cyc_Toc_typ_to_c(_tmp1B9),((_tmp9E9.args=_tmp1D1,((_tmp9E9.c_varargs=_tmp1BB,((_tmp9E9.cyc_varargs=0,((_tmp9E9.rgn_po=0,((_tmp9E9.attributes=_tmp1CD,((_tmp9E9.requires_clause=0,((_tmp9E9.requires_relns=0,((_tmp9E9.ensures_clause=0,((_tmp9E9.ensures_relns=0,_tmp9E9)))))))))))))))))))))))))),_tmp9EA)))),_tmp9E8))));};};}case 10: _LLB5: _tmp1B7=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1A5)->f1;_LLB6:
# 903
 _tmp1B7=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp1B7);
return Cyc_Toc_add_tuple_type(_tmp1B7);case 12: _LLB7: _tmp1B5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1A5)->f1;_tmp1B6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1A5)->f2;_LLB8: {
# 908
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp9ED;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp9EC;return(void*)((_tmp9EC=_cycalloc(sizeof(*_tmp9EC)),((_tmp9EC[0]=((_tmp9ED.tag=12,((_tmp9ED.f1=_tmp1B5,((_tmp9ED.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp1B6),_tmp9ED)))))),_tmp9EC))));}case 11: _LLB9: _tmp1B3=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A5)->f1).aggr_info;_tmp1B4=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A5)->f1).targs;_LLBA:
# 912
{union Cyc_Absyn_AggrInfoU _tmp1DB=_tmp1B3;if((_tmp1DB.UnknownAggr).tag == 1){_LLF4: _LLF5:
 return t;}else{_LLF6: _LLF7:
 goto _LLF3;}_LLF3:;}{
# 916
struct Cyc_Absyn_Aggrdecl*_tmp1DC=Cyc_Absyn_get_known_aggrdecl(_tmp1B3);
if(_tmp1DC->expected_mem_kind){
# 919
if(_tmp1DC->impl == 0){
const char*_tmp9F6;void*_tmp9F5[2];const char*_tmp9F4;const char*_tmp9F3;struct Cyc_String_pa_PrintArg_struct _tmp9F2;struct Cyc_String_pa_PrintArg_struct _tmp9F1;(_tmp9F1.tag=0,((_tmp9F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 922
Cyc_Absynpp_qvar2string(_tmp1DC->name)),((_tmp9F2.tag=0,((_tmp9F2.f1=(struct _dyneither_ptr)(
# 921
_tmp1DC->kind == Cyc_Absyn_UnionA?(_tmp9F3="union",_tag_dyneither(_tmp9F3,sizeof(char),6)):((_tmp9F4="struct",_tag_dyneither(_tmp9F4,sizeof(char),7)))),((_tmp9F5[0]=& _tmp9F2,((_tmp9F5[1]=& _tmp9F1,Cyc_Tcutil_warn(0,((_tmp9F6="%s %s was never defined.",_tag_dyneither(_tmp9F6,sizeof(char),25))),_tag_dyneither(_tmp9F5,sizeof(void*),2)))))))))))));}}
# 927
if(_tmp1DC->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp1DC->name,Cyc_Absyn_unionq_typ);{
struct Cyc_List_List*_tmp1E3=_tmp1DC->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1DC->impl))->fields;
if(_tmp1E3 == 0)return Cyc_Toc_aggrdecl_type(_tmp1DC->name,Cyc_Absyn_strctq);
for(0;_tmp1E3->tl != 0;_tmp1E3=_tmp1E3->tl){;}{
void*_tmp1E4=((struct Cyc_Absyn_Aggrfield*)_tmp1E3->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp1E4))){
if(_tmp1DC->expected_mem_kind){
const char*_tmp9FA;void*_tmp9F9[1];struct Cyc_String_pa_PrintArg_struct _tmp9F8;(_tmp9F8.tag=0,((_tmp9F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp1DC->name)),((_tmp9F9[0]=& _tmp9F8,Cyc_Tcutil_warn(0,((_tmp9FA="struct %s ended up being abstract.",_tag_dyneither(_tmp9FA,sizeof(char),35))),_tag_dyneither(_tmp9F9,sizeof(void*),1)))))));}{
# 941
struct _RegionHandle _tmp1E8=_new_region("r");struct _RegionHandle*r=& _tmp1E8;_push_region(r);
{struct Cyc_List_List*_tmp1E9=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp1DC->tvs,_tmp1B4);
void*_tmp1EA=Cyc_Tcutil_rsubstitute(r,_tmp1E9,_tmp1E4);
if(Cyc_Toc_is_abstract_type(_tmp1EA)){void*_tmp1EB=Cyc_Toc_aggrdecl_type(_tmp1DC->name,Cyc_Absyn_strctq);_npop_handler(0);return _tmp1EB;}{
void*_tmp1EC=Cyc_Toc_add_struct_type(_tmp1DC->name,_tmp1DC->tvs,_tmp1B4,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1DC->impl))->fields);_npop_handler(0);return _tmp1EC;};}
# 942
;_pop_region(r);};}
# 947
return Cyc_Toc_aggrdecl_type(_tmp1DC->name,Cyc_Absyn_strctq);};};};case 13: _LLBB: _tmp1B2=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp1A5)->f1;_LLBC:
 return t;case 14: _LLBD: _tmp1B1=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1A5)->f1;_LLBE:
 Cyc_Toc_enumfields_to_c(_tmp1B1);return t;case 17: _LLBF: _tmp1AD=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1A5)->f1;_tmp1AE=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1A5)->f2;_tmp1AF=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1A5)->f3;_tmp1B0=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1A5)->f4;_LLC0:
# 951
 if(_tmp1B0 == 0  || Cyc_noexpand_r){
if(_tmp1AE != 0){
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp9FD;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp9FC;return(void*)((_tmp9FC=_cycalloc(sizeof(*_tmp9FC)),((_tmp9FC[0]=((_tmp9FD.tag=17,((_tmp9FD.f1=_tmp1AD,((_tmp9FD.f2=0,((_tmp9FD.f3=_tmp1AF,((_tmp9FD.f4=0,_tmp9FD)))))))))),_tmp9FC))));}else{
return t;}}else{
# 956
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpA00;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp9FF;return(void*)((_tmp9FF=_cycalloc(sizeof(*_tmp9FF)),((_tmp9FF[0]=((_tmpA00.tag=17,((_tmpA00.f1=_tmp1AD,((_tmpA00.f2=0,((_tmpA00.f3=_tmp1AF,((_tmpA00.f4=Cyc_Toc_typ_to_c(_tmp1B0),_tmpA00)))))))))),_tmp9FF))));}case 19: _LLC1: _LLC2:
 return Cyc_Absyn_uint_typ;case 15: _LLC3: _tmp1AC=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1A5)->f1;_LLC4:
 return Cyc_Toc_rgn_typ();case 16: _LLC5: _LLC6:
 return Cyc_Toc_dyn_rgn_typ();case 20: _LLC7: _LLC8:
# 962
 goto _LLCA;case 21: _LLC9: _LLCA:
 goto _LLCC;case 22: _LLCB: _LLCC:
 goto _LLCE;case 23: _LLCD: _LLCE:
 goto _LLD0;case 24: _LLCF: _LLD0:
 goto _LLD2;case 25: _LLD1: _LLD2:
 return Cyc_Absyn_void_star_typ();case 18: _LLD3: _tmp1AB=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp1A5)->f1;_LLD4:
# 972
 return t;case 27: _LLD5: _tmp1AA=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp1A5)->f1;_LLD6:
# 974
 return t;case 28: _LLD7: _LLD8:
 return t;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1A5)->f1)->r)){case 0: _LLD9: _tmp1A9=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1A5)->f1)->r)->f1;_LLDA:
# 977
 Cyc_Toc_aggrdecl_to_c(_tmp1A9,1);
if(_tmp1A9->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp1A9->name,Cyc_Absyn_unionq_typ);else{
return Cyc_Toc_aggrdecl_type(_tmp1A9->name,Cyc_Absyn_strctq);}case 1: _LLDB: _tmp1A8=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1A5)->f1)->r)->f1;_LLDC:
# 982
 Cyc_Toc_enumdecl_to_c(_tmp1A8);
return t;default: _LLDD: _tmp1A6=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1A5)->f1)->r)->f1;_tmp1A7=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1A5)->f2;_LLDE:
# 985
 Cyc_Toc_datatypedecl_to_c(_tmp1A6);
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp1A7)));}}_LL9E:;}
# 990
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned int l){
void*_tmp1F1=t;void*_tmp1F2;struct Cyc_Absyn_Tqual _tmp1F3;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F1)->tag == 8){_LLF9: _tmp1F2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F1)->f1).elt_type;_tmp1F3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F1)->f1).tq;_LLFA:
# 993
 return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp1F2,(void*)& Cyc_Absyn_HeapRgn_val,_tmp1F3,Cyc_Absyn_false_conref),e);}else{_LLFB: _LLFC:
 return Cyc_Toc_cast_it(t,e);}_LLF8:;}
# 1000
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
# 1017
{union Cyc_Absyn_AggrInfoU _tmp1FC=_tmp1FA;if((_tmp1FC.UnknownAggr).tag == 1){_LL123: _LL124:
 return 0;}else{_LL125: _LL126:
 goto _LL122;}_LL122:;}{
# 1021
struct Cyc_Absyn_Aggrdecl*_tmp1FD=Cyc_Absyn_get_known_aggrdecl(_tmp1FA);
if(_tmp1FD->impl == 0)
return 0;
{struct Cyc_List_List*_tmp1FE=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1FD->impl))->fields;for(0;_tmp1FE != 0;_tmp1FE=_tmp1FE->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp1FE->hd)->type))return 0;}}
return 1;};case 12: _LL112: _tmp1F9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1F5)->f2;_LL113:
# 1028
 for(0;_tmp1F9 != 0;_tmp1F9=_tmp1F9->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp1F9->hd)->type))return 0;}
return 1;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1F5)->f1).field_info).KnownDatatypefield).tag == 2){_LL114: _tmp1F7=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1F5)->f1).field_info).KnownDatatypefield).val).f1;_tmp1F8=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1F5)->f1).field_info).KnownDatatypefield).val).f2;_LL115:
# 1032
 _tmp1F6=_tmp1F8->typs;goto _LL117;}else{goto _LL120;}case 10: _LL116: _tmp1F6=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1F5)->f1;_LL117:
# 1034
 for(0;_tmp1F6 != 0;_tmp1F6=_tmp1F6->tl){
if(!Cyc_Toc_atomic_typ((*((struct _tuple11*)_tmp1F6->hd)).f2))return 0;}
return 1;case 3: _LL118: _LL119:
# 1039
 goto _LL11B;case 5: _LL11A: _LL11B:
 goto _LL11D;case 16: _LL11C: _LL11D:
 goto _LL11F;case 15: _LL11E: _LL11F:
 return 0;default: _LL120: _LL121: {
const char*_tmpA04;void*_tmpA03[1];struct Cyc_String_pa_PrintArg_struct _tmpA02;(_tmpA02.tag=0,((_tmpA02.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA03[0]=& _tmpA02,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA04="atomic_typ:  bad type %s",_tag_dyneither(_tmpA04,sizeof(char),25))),_tag_dyneither(_tmpA03,sizeof(void*),1)))))));}}_LLFD:;}
# 1047
static int Cyc_Toc_is_void_star(void*t){
void*_tmp202=Cyc_Tcutil_compress(t);void*_tmp203=_tmp202;void*_tmp204;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp203)->tag == 5){_LL128: _tmp204=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp203)->f1).elt_typ;_LL129: {
# 1050
void*_tmp205=Cyc_Tcutil_compress(_tmp204);void*_tmp206=_tmp205;if(((struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp206)->tag == 0){_LL12D: _LL12E:
 return 1;}else{_LL12F: _LL130:
 return 0;}_LL12C:;}}else{_LL12A: _LL12B:
# 1054
 return 0;}_LL127:;}
# 1058
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1062
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1067
static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){
void*_tmp207=Cyc_Tcutil_compress(t);void*_tmp208=_tmp207;struct Cyc_List_List*_tmp209;union Cyc_Absyn_AggrInfoU _tmp20A;switch(*((int*)_tmp208)){case 11: _LL132: _tmp20A=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp208)->f1).aggr_info;_LL133: {
# 1070
struct Cyc_Absyn_Aggrdecl*_tmp20B=Cyc_Absyn_get_known_aggrdecl(_tmp20A);
if(_tmp20B->impl == 0){
const char*_tmpA07;void*_tmpA06;(_tmpA06=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA07="is_poly_field: type missing fields",_tag_dyneither(_tmpA07,sizeof(char),35))),_tag_dyneither(_tmpA06,sizeof(void*),0)));}
_tmp209=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp20B->impl))->fields;goto _LL135;}case 12: _LL134: _tmp209=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp208)->f2;_LL135: {
# 1075
struct Cyc_Absyn_Aggrfield*_tmp20E=Cyc_Absyn_lookup_field(_tmp209,f);
if(_tmp20E == 0){
const char*_tmpA0B;void*_tmpA0A[1];struct Cyc_String_pa_PrintArg_struct _tmpA09;(_tmpA09.tag=0,((_tmpA09.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpA0A[0]=& _tmpA09,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA0B="is_poly_field: bad field %s",_tag_dyneither(_tmpA0B,sizeof(char),28))),_tag_dyneither(_tmpA0A,sizeof(void*),1)))))));}
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp20E->type);}default: _LL136: _LL137: {
const char*_tmpA0F;void*_tmpA0E[1];struct Cyc_String_pa_PrintArg_struct _tmpA0D;(_tmpA0D.tag=0,((_tmpA0D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA0E[0]=& _tmpA0D,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA0F="is_poly_field: bad type %s",_tag_dyneither(_tmpA0F,sizeof(char),27))),_tag_dyneither(_tmpA0E,sizeof(void*),1)))))));}}_LL131:;}
# 1086
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp215=e->r;void*_tmp216=_tmp215;struct Cyc_Absyn_Exp*_tmp217;struct _dyneither_ptr*_tmp218;struct Cyc_Absyn_Exp*_tmp219;struct _dyneither_ptr*_tmp21A;switch(*((int*)_tmp216)){case 20: _LL139: _tmp219=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp216)->f1;_tmp21A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp216)->f2;_LL13A:
# 1089
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp219->topt),_tmp21A) && !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));case 21: _LL13B: _tmp217=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp216)->f1;_tmp218=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp216)->f2;_LL13C: {
# 1092
void*_tmp21B=Cyc_Tcutil_compress((void*)_check_null(_tmp217->topt));void*_tmp21C=_tmp21B;void*_tmp21D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21C)->tag == 5){_LL140: _tmp21D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21C)->f1).elt_typ;_LL141:
# 1094
 return Cyc_Toc_is_poly_field(_tmp21D,_tmp218) && !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}else{_LL142: _LL143: {
const char*_tmpA13;void*_tmpA12[1];struct Cyc_String_pa_PrintArg_struct _tmpA11;(_tmpA11.tag=0,((_tmpA11.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp217->topt))),((_tmpA12[0]=& _tmpA11,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA13="is_poly_project: bad type %s",_tag_dyneither(_tmpA13,sizeof(char),29))),_tag_dyneither(_tmpA12,sizeof(void*),1)))))));}}_LL13F:;}default: _LL13D: _LL13E:
# 1097
 return 0;}_LL138:;}
# 1102
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpA14;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((_tmpA14=_cycalloc(sizeof(*_tmpA14)),((_tmpA14->hd=s,((_tmpA14->tl=0,_tmpA14)))))),0);}
# 1106
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpA15;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((_tmpA15=_cycalloc(sizeof(*_tmpA15)),((_tmpA15->hd=s,((_tmpA15->tl=0,_tmpA15)))))),0);}
# 1110
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1116
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*_tmpA16[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmpA16[1]=s,((_tmpA16[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA16,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
# 1120
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*_tmpA17[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__fast_region_malloc_e,((_tmpA17[1]=s,((_tmpA17[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA17,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
# 1124
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type)){
struct Cyc_Absyn_Exp*_tmpA18[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpA18[1]=n,((_tmpA18[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA18,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{
# 1128
struct Cyc_Absyn_Exp*_tmpA19[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpA19[1]=n,((_tmpA19[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA19,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}
# 1131
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*_tmpA1A[3];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,((_tmpA1A[2]=n,((_tmpA1A[1]=s,((_tmpA1A[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA1A,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
# 1136
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
struct Cyc_List_List*_tmpA1B;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((_tmpA1B=_cycalloc(sizeof(*_tmpA1B)),((_tmpA1B->hd=e,((_tmpA1B->tl=0,_tmpA1B)))))),0);}
# 1139
static struct Cyc_Absyn_Exp*Cyc_Toc_newrethrow_exp(struct Cyc_Absyn_Exp*e){
struct Cyc_List_List*_tmpA1C;return Cyc_Absyn_fncall_exp(Cyc_Toc__rethrow_e,((_tmpA1C=_cycalloc(sizeof(*_tmpA1C)),((_tmpA1C->hd=e,((_tmpA1C->tl=0,_tmpA1C)))))),0);}
# 1143
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
struct Cyc_Absyn_Exp*_tmpA1D[0];return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA1D,sizeof(struct Cyc_Absyn_Exp*),0)),0),0);}
# 1149
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1157
int is_string=0;
{void*_tmp22B=e->r;void*_tmp22C=_tmp22B;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp22C)->tag == 0)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp22C)->f1).Wstring_c).tag){case 8: _LL145: _LL146:
 is_string=1;goto _LL144;case 9: _LL147: _LL148:
 is_string=1;goto _LL144;default: goto _LL149;}else{_LL149: _LL14A:
 goto _LL144;}_LL144:;}{
# 1163
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
if(is_string){
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_conref,0);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpA23;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpA22;struct Cyc_List_List*_tmpA21;Cyc_Toc_result_decls=((_tmpA21=_cycalloc(sizeof(*_tmpA21)),((_tmpA21->hd=Cyc_Absyn_new_decl((void*)((_tmpA23=_cycalloc(sizeof(*_tmpA23)),((_tmpA23[0]=((_tmpA22.tag=0,((_tmpA22.f1=vd,_tmpA22)))),_tmpA23)))),0),((_tmpA21->tl=Cyc_Toc_result_decls,_tmpA21))))));}
xexp=Cyc_Absyn_var_exp(x,0);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{
# 1173
xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),e);
# 1175
xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
# 1177
struct Cyc_Absyn_Exp*urm_exp;
{struct _tuple19*_tmpA2A;struct _tuple19*_tmpA29;struct _tuple19*_tmpA28;struct _tuple19*_tmpA27[3];urm_exp=Cyc_Absyn_unresolvedmem_exp(0,((_tmpA27[2]=(
# 1180
(_tmpA28=_cycalloc(sizeof(*_tmpA28)),((_tmpA28->f1=0,((_tmpA28->f2=xplussz,_tmpA28)))))),((_tmpA27[1]=(
# 1179
(_tmpA29=_cycalloc(sizeof(*_tmpA29)),((_tmpA29->f1=0,((_tmpA29->f2=xexp,_tmpA29)))))),((_tmpA27[0]=(
# 1178
(_tmpA2A=_cycalloc(sizeof(*_tmpA2A)),((_tmpA2A->f1=0,((_tmpA2A->f2=xexp,_tmpA2A)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA27,sizeof(struct _tuple19*),3)))))))),0);}
# 1181
return urm_exp;};};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_StructInfo{struct Cyc_Absyn_Exp**varsizeexp;struct Cyc_Absyn_Exp*lhs_exp;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;int*in_lhs;struct Cyc_Toc_StructInfo struct_info;int*in_sizeof;struct _RegionHandle*rgn;};
# 1224 "toc.cyc"
typedef struct Cyc_Toc_Env*Cyc_Toc_env_t;
# 1226
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp234=nv;int _tmp235;_LL14C: _tmp235=_tmp234->toplevel;_LL14D:;
return _tmp235;}
# 1230
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp236=nv;int*_tmp237;_LL14F: _tmp237=_tmp236->in_lhs;_LL150:;
return*_tmp237;}
# 1235
static struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple1*x){
struct Cyc_Toc_Env*_tmp238=nv;struct Cyc_Dict_Dict _tmp239;_LL152: _tmp239=_tmp238->varmap;_LL153:;
return((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp239,x);}
# 1241
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
int*_tmpA32;struct Cyc_Absyn_Exp**_tmpA31;int*_tmpA30;struct Cyc_Toc_Env*_tmpA2F;return(_tmpA2F=_region_malloc(r,sizeof(*_tmpA2F)),((_tmpA2F->break_lab=(struct _dyneither_ptr**)0,((_tmpA2F->continue_lab=(struct _dyneither_ptr**)0,((_tmpA2F->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpA2F->varmap=(struct Cyc_Dict_Dict)
# 1245
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((_tmpA2F->toplevel=(int)1,((_tmpA2F->in_lhs=(int*)(
# 1247
(_tmpA30=_region_malloc(r,sizeof(*_tmpA30)),((_tmpA30[0]=0,_tmpA30)))),((_tmpA2F->struct_info=(struct Cyc_Toc_StructInfo)(
((_tmpA2F->struct_info).varsizeexp=((_tmpA31=_cycalloc(sizeof(*_tmpA31)),((_tmpA31[0]=0,_tmpA31)))),(((_tmpA2F->struct_info).lhs_exp=0,_tmpA2F->struct_info)))),((_tmpA2F->in_sizeof=(int*)(
(_tmpA32=_region_malloc(r,sizeof(*_tmpA32)),((_tmpA32[0]=0,_tmpA32)))),((_tmpA2F->rgn=(struct _RegionHandle*)r,_tmpA2F)))))))))))))))))));}
# 1253
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp23E=e;struct _dyneither_ptr**_tmp23F;struct _dyneither_ptr**_tmp240;struct Cyc_Toc_FallthruInfo*_tmp241;struct Cyc_Dict_Dict _tmp242;int _tmp243;int*_tmp244;struct Cyc_Toc_StructInfo _tmp245;int*_tmp246;_LL155: _tmp23F=_tmp23E->break_lab;_tmp240=_tmp23E->continue_lab;_tmp241=_tmp23E->fallthru_info;_tmp242=_tmp23E->varmap;_tmp243=_tmp23E->toplevel;_tmp244=_tmp23E->in_lhs;_tmp245=_tmp23E->struct_info;_tmp246=_tmp23E->in_sizeof;_LL156:;{
struct Cyc_Toc_Env*_tmpA33;return(_tmpA33=_region_malloc(r,sizeof(*_tmpA33)),((_tmpA33->break_lab=(struct _dyneither_ptr**)_tmp23F,((_tmpA33->continue_lab=(struct _dyneither_ptr**)_tmp240,((_tmpA33->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp241,((_tmpA33->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp242),((_tmpA33->toplevel=(int)_tmp243,((_tmpA33->in_lhs=(int*)_tmp244,((_tmpA33->struct_info=(struct Cyc_Toc_StructInfo)_tmp245,((_tmpA33->in_sizeof=(int*)_tmp246,((_tmpA33->rgn=(struct _RegionHandle*)r,_tmpA33)))))))))))))))))));};}
# 1258
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp248=e;struct _dyneither_ptr**_tmp249;struct _dyneither_ptr**_tmp24A;struct Cyc_Toc_FallthruInfo*_tmp24B;struct Cyc_Dict_Dict _tmp24C;int _tmp24D;int*_tmp24E;struct Cyc_Toc_StructInfo _tmp24F;int*_tmp250;_LL158: _tmp249=_tmp248->break_lab;_tmp24A=_tmp248->continue_lab;_tmp24B=_tmp248->fallthru_info;_tmp24C=_tmp248->varmap;_tmp24D=_tmp248->toplevel;_tmp24E=_tmp248->in_lhs;_tmp24F=_tmp248->struct_info;_tmp250=_tmp248->in_sizeof;_LL159:;{
struct Cyc_Toc_Env*_tmpA34;return(_tmpA34=_region_malloc(r,sizeof(*_tmpA34)),((_tmpA34->break_lab=(struct _dyneither_ptr**)_tmp249,((_tmpA34->continue_lab=(struct _dyneither_ptr**)_tmp24A,((_tmpA34->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp24B,((_tmpA34->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp24C),((_tmpA34->toplevel=(int)0,((_tmpA34->in_lhs=(int*)_tmp24E,((_tmpA34->struct_info=(struct Cyc_Toc_StructInfo)_tmp24F,((_tmpA34->in_sizeof=(int*)_tmp250,((_tmpA34->rgn=(struct _RegionHandle*)r,_tmpA34)))))))))))))))))));};}
# 1262
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp252=e;struct _dyneither_ptr**_tmp253;struct _dyneither_ptr**_tmp254;struct Cyc_Toc_FallthruInfo*_tmp255;struct Cyc_Dict_Dict _tmp256;int _tmp257;int*_tmp258;struct Cyc_Toc_StructInfo _tmp259;int*_tmp25A;_LL15B: _tmp253=_tmp252->break_lab;_tmp254=_tmp252->continue_lab;_tmp255=_tmp252->fallthru_info;_tmp256=_tmp252->varmap;_tmp257=_tmp252->toplevel;_tmp258=_tmp252->in_lhs;_tmp259=_tmp252->struct_info;_tmp25A=_tmp252->in_sizeof;_LL15C:;{
struct Cyc_Toc_Env*_tmpA35;return(_tmpA35=_region_malloc(r,sizeof(*_tmpA35)),((_tmpA35->break_lab=(struct _dyneither_ptr**)_tmp253,((_tmpA35->continue_lab=(struct _dyneither_ptr**)_tmp254,((_tmpA35->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp255,((_tmpA35->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp256),((_tmpA35->toplevel=(int)1,((_tmpA35->in_lhs=(int*)_tmp258,((_tmpA35->struct_info=(struct Cyc_Toc_StructInfo)_tmp259,((_tmpA35->in_sizeof=(int*)_tmp25A,((_tmpA35->rgn=(struct _RegionHandle*)r,_tmpA35)))))))))))))))))));};}
# 1266
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp25C=e;int*_tmp25D;_LL15E: _tmp25D=_tmp25C->in_lhs;_LL15F:;
*_tmp25D=b;}
# 1270
static int Cyc_Toc_set_in_sizeof(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp25E=e;int*_tmp25F;_LL161: _tmp25F=_tmp25E->in_sizeof;_LL162:;{
int _tmp260=*_tmp25F;
*_tmp25F=b;
return _tmp260;};}
# 1276
static int Cyc_Toc_in_sizeof(struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp261=e;int*_tmp262;_LL164: _tmp262=_tmp261->in_sizeof;_LL165:;
return*_tmp262;}
# 1281
static struct Cyc_Toc_Env*Cyc_Toc_set_lhs_exp(struct _RegionHandle*r,struct Cyc_Absyn_Exp*exp,struct Cyc_Toc_Env*e){
# 1283
struct Cyc_Toc_Env*_tmp263=e;struct _dyneither_ptr**_tmp264;struct _dyneither_ptr**_tmp265;struct Cyc_Toc_FallthruInfo*_tmp266;struct Cyc_Dict_Dict _tmp267;int _tmp268;int*_tmp269;struct Cyc_Absyn_Exp**_tmp26A;int*_tmp26B;_LL167: _tmp264=_tmp263->break_lab;_tmp265=_tmp263->continue_lab;_tmp266=_tmp263->fallthru_info;_tmp267=_tmp263->varmap;_tmp268=_tmp263->toplevel;_tmp269=_tmp263->in_lhs;_tmp26A=(_tmp263->struct_info).varsizeexp;_tmp26B=_tmp263->in_sizeof;_LL168:;{
struct Cyc_Toc_Env*_tmpA36;return(_tmpA36=_region_malloc(r,sizeof(*_tmpA36)),((_tmpA36->break_lab=(struct _dyneither_ptr**)_tmp264,((_tmpA36->continue_lab=(struct _dyneither_ptr**)_tmp265,((_tmpA36->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp266,((_tmpA36->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp267),((_tmpA36->toplevel=(int)_tmp268,((_tmpA36->in_lhs=(int*)_tmp269,((_tmpA36->struct_info=(struct Cyc_Toc_StructInfo)(((_tmpA36->struct_info).varsizeexp=_tmp26A,(((_tmpA36->struct_info).lhs_exp=exp,_tmpA36->struct_info)))),((_tmpA36->in_sizeof=(int*)_tmp26B,((_tmpA36->rgn=(struct _RegionHandle*)r,_tmpA36)))))))))))))))))));};}
# 1289
static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple1*x,struct Cyc_Absyn_Exp*y){
{union Cyc_Absyn_Nmspace _tmp26D=(*x).f1;union Cyc_Absyn_Nmspace _tmp26E=_tmp26D;if((_tmp26E.Rel_n).tag == 1){_LL16A: _LL16B: {
# 1292
const char*_tmpA3A;void*_tmpA39[1];struct Cyc_String_pa_PrintArg_struct _tmpA38;(_tmpA38.tag=0,((_tmpA38.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(x)),((_tmpA39[0]=& _tmpA38,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA3A="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(_tmpA3A,sizeof(char),30))),_tag_dyneither(_tmpA39,sizeof(void*),1)))))));}}else{_LL16C: _LL16D:
 goto _LL169;}_LL169:;}{
# 1295
struct Cyc_Toc_Env*_tmp272=e;struct _dyneither_ptr**_tmp273;struct _dyneither_ptr**_tmp274;struct Cyc_Toc_FallthruInfo*_tmp275;struct Cyc_Dict_Dict _tmp276;int _tmp277;int*_tmp278;struct Cyc_Toc_StructInfo _tmp279;int*_tmp27A;_LL16F: _tmp273=_tmp272->break_lab;_tmp274=_tmp272->continue_lab;_tmp275=_tmp272->fallthru_info;_tmp276=_tmp272->varmap;_tmp277=_tmp272->toplevel;_tmp278=_tmp272->in_lhs;_tmp279=_tmp272->struct_info;_tmp27A=_tmp272->in_sizeof;_LL170:;{
struct Cyc_Dict_Dict _tmp27B=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp276),x,y);
struct Cyc_Toc_Env*_tmpA3B;return(_tmpA3B=_region_malloc(r,sizeof(*_tmpA3B)),((_tmpA3B->break_lab=(struct _dyneither_ptr**)_tmp273,((_tmpA3B->continue_lab=(struct _dyneither_ptr**)_tmp274,((_tmpA3B->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp275,((_tmpA3B->varmap=(struct Cyc_Dict_Dict)_tmp27B,((_tmpA3B->toplevel=(int)_tmp277,((_tmpA3B->in_lhs=(int*)_tmp278,((_tmpA3B->struct_info=(struct Cyc_Toc_StructInfo)_tmp279,((_tmpA3B->in_sizeof=(int*)_tmp27A,((_tmpA3B->rgn=(struct _RegionHandle*)r,_tmpA3B)))))))))))))))))));};};}
# 1302
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp27D=e;struct _dyneither_ptr**_tmp27E;struct _dyneither_ptr**_tmp27F;struct Cyc_Toc_FallthruInfo*_tmp280;struct Cyc_Dict_Dict _tmp281;int _tmp282;int*_tmp283;struct Cyc_Toc_StructInfo _tmp284;int*_tmp285;_LL172: _tmp27E=_tmp27D->break_lab;_tmp27F=_tmp27D->continue_lab;_tmp280=_tmp27D->fallthru_info;_tmp281=_tmp27D->varmap;_tmp282=_tmp27D->toplevel;_tmp283=_tmp27D->in_lhs;_tmp284=_tmp27D->struct_info;_tmp285=_tmp27D->in_sizeof;_LL173:;{
struct Cyc_Toc_Env*_tmpA3C;return(_tmpA3C=_region_malloc(r,sizeof(*_tmpA3C)),((_tmpA3C->break_lab=(struct _dyneither_ptr**)0,((_tmpA3C->continue_lab=(struct _dyneither_ptr**)0,((_tmpA3C->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp280,((_tmpA3C->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp281),((_tmpA3C->toplevel=(int)_tmp282,((_tmpA3C->in_lhs=(int*)_tmp283,((_tmpA3C->struct_info=(struct Cyc_Toc_StructInfo)_tmp284,((_tmpA3C->in_sizeof=(int*)_tmp285,((_tmpA3C->rgn=(struct _RegionHandle*)r,_tmpA3C)))))))))))))))))));};}
# 1308
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*next_case_env){
# 1313
struct Cyc_List_List*fallthru_vars=0;
for(0;fallthru_binders != 0;fallthru_binders=fallthru_binders->tl){
struct Cyc_List_List*_tmpA3D;fallthru_vars=((_tmpA3D=_region_malloc(r,sizeof(*_tmpA3D)),((_tmpA3D->hd=((struct Cyc_Absyn_Vardecl*)fallthru_binders->hd)->name,((_tmpA3D->tl=fallthru_vars,_tmpA3D))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fallthru_vars);{
struct Cyc_Toc_Env*_tmp288=e;struct _dyneither_ptr**_tmp289;struct _dyneither_ptr**_tmp28A;struct Cyc_Toc_FallthruInfo*_tmp28B;struct Cyc_Dict_Dict _tmp28C;int _tmp28D;int*_tmp28E;struct Cyc_Toc_StructInfo _tmp28F;int*_tmp290;_LL175: _tmp289=_tmp288->break_lab;_tmp28A=_tmp288->continue_lab;_tmp28B=_tmp288->fallthru_info;_tmp28C=_tmp288->varmap;_tmp28D=_tmp288->toplevel;_tmp28E=_tmp288->in_lhs;_tmp28F=_tmp288->struct_info;_tmp290=_tmp288->in_sizeof;_LL176:;{
struct Cyc_Toc_Env*_tmp291=next_case_env;struct Cyc_Dict_Dict _tmp292;_LL178: _tmp292=_tmp291->varmap;_LL179:;{
struct Cyc_Toc_FallthruInfo*_tmpA3E;struct Cyc_Toc_FallthruInfo*fi=
(_tmpA3E=_region_malloc(r,sizeof(*_tmpA3E)),((_tmpA3E->label=fallthru_l,((_tmpA3E->binders=fallthru_vars,((_tmpA3E->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp292),_tmpA3E)))))));
struct _dyneither_ptr**_tmpA41;struct Cyc_Toc_Env*_tmpA40;return(_tmpA40=_region_malloc(r,sizeof(*_tmpA40)),((_tmpA40->break_lab=(struct _dyneither_ptr**)((_tmpA41=_region_malloc(r,sizeof(*_tmpA41)),((_tmpA41[0]=break_l,_tmpA41)))),((_tmpA40->continue_lab=(struct _dyneither_ptr**)_tmp28A,((_tmpA40->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi,((_tmpA40->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp28C),((_tmpA40->toplevel=(int)_tmp28D,((_tmpA40->in_lhs=(int*)_tmp28E,((_tmpA40->struct_info=(struct Cyc_Toc_StructInfo)_tmp28F,((_tmpA40->in_sizeof=(int*)_tmp290,((_tmpA40->rgn=(struct _RegionHandle*)r,_tmpA40)))))))))))))))))));};};};}
# 1325
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1328
struct Cyc_Toc_Env*_tmp296=e;struct _dyneither_ptr**_tmp297;struct _dyneither_ptr**_tmp298;struct Cyc_Toc_FallthruInfo*_tmp299;struct Cyc_Dict_Dict _tmp29A;int _tmp29B;int*_tmp29C;struct Cyc_Toc_StructInfo _tmp29D;int*_tmp29E;_LL17B: _tmp297=_tmp296->break_lab;_tmp298=_tmp296->continue_lab;_tmp299=_tmp296->fallthru_info;_tmp29A=_tmp296->varmap;_tmp29B=_tmp296->toplevel;_tmp29C=_tmp296->in_lhs;_tmp29D=_tmp296->struct_info;_tmp29E=_tmp296->in_sizeof;_LL17C:;{
struct _dyneither_ptr**_tmpA44;struct Cyc_Toc_Env*_tmpA43;return(_tmpA43=_region_malloc(r,sizeof(*_tmpA43)),((_tmpA43->break_lab=(struct _dyneither_ptr**)((_tmpA44=_region_malloc(r,sizeof(*_tmpA44)),((_tmpA44[0]=break_l,_tmpA44)))),((_tmpA43->continue_lab=(struct _dyneither_ptr**)_tmp298,((_tmpA43->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpA43->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp29A),((_tmpA43->toplevel=(int)_tmp29B,((_tmpA43->in_lhs=(int*)_tmp29C,((_tmpA43->struct_info=(struct Cyc_Toc_StructInfo)_tmp29D,((_tmpA43->in_sizeof=(int*)_tmp29E,((_tmpA43->rgn=(struct _RegionHandle*)r,_tmpA43)))))))))))))))))));};}
# 1335
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1338
struct Cyc_Toc_Env*_tmp2A1=e;struct _dyneither_ptr**_tmp2A2;struct _dyneither_ptr**_tmp2A3;struct Cyc_Toc_FallthruInfo*_tmp2A4;struct Cyc_Dict_Dict _tmp2A5;int _tmp2A6;int*_tmp2A7;struct Cyc_Toc_StructInfo _tmp2A8;int*_tmp2A9;_LL17E: _tmp2A2=_tmp2A1->break_lab;_tmp2A3=_tmp2A1->continue_lab;_tmp2A4=_tmp2A1->fallthru_info;_tmp2A5=_tmp2A1->varmap;_tmp2A6=_tmp2A1->toplevel;_tmp2A7=_tmp2A1->in_lhs;_tmp2A8=_tmp2A1->struct_info;_tmp2A9=_tmp2A1->in_sizeof;_LL17F:;{
struct Cyc_Toc_FallthruInfo*_tmpA47;struct Cyc_Toc_Env*_tmpA46;return(_tmpA46=_region_malloc(r,sizeof(*_tmpA46)),((_tmpA46->break_lab=(struct _dyneither_ptr**)0,((_tmpA46->continue_lab=(struct _dyneither_ptr**)_tmp2A3,((_tmpA46->fallthru_info=(struct Cyc_Toc_FallthruInfo*)((_tmpA47=_region_malloc(r,sizeof(*_tmpA47)),((_tmpA47->label=next_l,((_tmpA47->binders=0,((_tmpA47->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),_tmpA47)))))))),((_tmpA46->varmap=(struct Cyc_Dict_Dict)
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2A5),((_tmpA46->toplevel=(int)_tmp2A6,((_tmpA46->in_lhs=(int*)_tmp2A7,((_tmpA46->struct_info=(struct Cyc_Toc_StructInfo)_tmp2A8,((_tmpA46->in_sizeof=(int*)_tmp2A9,((_tmpA46->rgn=(struct _RegionHandle*)r,_tmpA46)))))))))))))))))));};}
# 1353 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1356
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
# 1358
void*_tmp2AC=e->annot;void*_tmp2AD=_tmp2AC;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2AD)->tag == Cyc_CfFlowInfo_UnknownZ){_LL181: _LL182:
# 1361
 return Cyc_Toc_is_nullable((void*)_check_null(e->topt));}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2AD)->tag == Cyc_CfFlowInfo_NotZero){_LL183: _LL184:
# 1364
 return 0;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2AD)->tag == Cyc_CfFlowInfo_IsZero){_LL185: _LL186:
# 1366
{const char*_tmpA4A;void*_tmpA49;(_tmpA49=0,Cyc_Tcutil_terr(e->loc,((_tmpA4A="dereference of NULL pointer",_tag_dyneither(_tmpA4A,sizeof(char),28))),_tag_dyneither(_tmpA49,sizeof(void*),0)));}
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2AD)->tag == Cyc_Absyn_EmptyAnnot){_LL187: _LL188:
# 1372
 return 1;}else{_LL189: _LL18A: {
const char*_tmpA4D;void*_tmpA4C;(_tmpA4C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA4D="need_null_check",_tag_dyneither(_tmpA4D,sizeof(char),16))),_tag_dyneither(_tmpA4C,sizeof(void*),0)));}}}}}_LL180:;}
# 1377
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp2B2=e->annot;void*_tmp2B3=_tmp2B2;struct Cyc_List_List*_tmp2B4;struct Cyc_List_List*_tmp2B5;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2B3)->tag == Cyc_CfFlowInfo_UnknownZ){_LL18C: _tmp2B5=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2B3)->f1;_LL18D:
 return _tmp2B5;}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2B3)->tag == Cyc_CfFlowInfo_NotZero){_LL18E: _tmp2B4=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2B3)->f1;_LL18F:
 return _tmp2B4;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2B3)->tag == Cyc_CfFlowInfo_IsZero){_LL190: _LL191:
# 1382
{const char*_tmpA50;void*_tmpA4F;(_tmpA4F=0,Cyc_Tcutil_terr(e->loc,((_tmpA50="dereference of NULL pointer",_tag_dyneither(_tmpA50,sizeof(char),28))),_tag_dyneither(_tmpA4F,sizeof(void*),0)));}
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2B3)->tag == Cyc_Absyn_EmptyAnnot){_LL192: _LL193:
 return 0;}else{_LL194: _LL195: {
const char*_tmpA53;void*_tmpA52;(_tmpA52=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA53="get_relns",_tag_dyneither(_tmpA53,sizeof(char),10))),_tag_dyneither(_tmpA52,sizeof(void*),0)));}}}}}_LL18B:;}static char _tmp2BE[8]="*bogus*";
# 1392
static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
# 1402 "toc.cyc"
union Cyc_Relations_RelnOp rop_i=Cyc_Relations_RConst(0);
int valid_rop_i=Cyc_Relations_exp2relnop(i,& rop_i);
# 1405
struct Cyc_Absyn_Vardecl*x;
{void*_tmp2BA=a->r;void*_tmp2BB=_tmp2BA;void*_tmp2BC;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2BB)->tag == 1){_LL197: _tmp2BC=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2BB)->f2;_LL198: {
# 1408
struct Cyc_Absyn_Vardecl*_tmp2BD=Cyc_Tcutil_nonesc_vardecl(_tmp2BC);
if(_tmp2BD == 0)goto _LL19A;
x=_tmp2BD;
goto _LL196;}}else{_LL199: _LL19A: {
# 1414
static struct _dyneither_ptr bogus_string={_tmp2BE,_tmp2BE,_tmp2BE + 8};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
# 1417
static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};
# 1423
x=& bogus_vardecl;
x->type=a_typ;}}_LL196:;}{
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
void*_tmp2C1=i->r;void*_tmp2C2=_tmp2C1;struct Cyc_Absyn_Exp*_tmp2C3;struct Cyc_Absyn_Exp*_tmp2C4;void*_tmp2C5;struct Cyc_Absyn_Exp*_tmp2C6;switch(*((int*)_tmp2C2)){case 13: _LL19C: _tmp2C5=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2C2)->f1;_tmp2C6=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2C2)->f2;_LL19D:
 i=_tmp2C6;goto inner_loop;case 2: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2C2)->f1 == Cyc_Absyn_Mod){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2C2)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2C2)->f2)->tl != 0){_LL19E: _tmp2C3=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2C2)->f2)->hd;_tmp2C4=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2C2)->f2)->tl)->hd;_LL19F: {
# 1457
union Cyc_Relations_RelnOp rop_z=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmp2C4,& rop_z)){
# 1460
struct Cyc_List_List*_tmp2C7=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlt,rop_z,relns);
# 1462
return !Cyc_Relations_consistent_relations(_tmp2C7);}
# 1464
goto _LL19B;}}else{goto _LL1A0;}}else{goto _LL1A0;}}else{goto _LL1A0;}default: _LL1A0: _LL1A1:
 goto _LL19B;}_LL19B:;}
# 1467
return 0;};}
# 1470
static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0){const char*_tmpA56;void*_tmpA55;(_tmpA55=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA56="Missing type in primop ",_tag_dyneither(_tmpA56,sizeof(char),24))),_tag_dyneither(_tmpA55,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));}
# 1474
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0){const char*_tmpA59;void*_tmpA58;(_tmpA58=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA59="Missing type in primop ",_tag_dyneither(_tmpA59,sizeof(char),24))),_tag_dyneither(_tmpA58,sizeof(void*),0)));}
return(void*)_check_null(e->topt);}
# 1478
static struct _tuple11*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
struct _tuple11*_tmpA5A;return(_tmpA5A=_cycalloc(sizeof(*_tmpA5A)),((_tmpA5A->f1=Cyc_Toc_mt_tq,((_tmpA5A->f2=Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmpA5A)))));}
# 1481
static struct _tuple19*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple19*_tmpA5B;return(_tmpA5B=_cycalloc(sizeof(*_tmpA5B)),((_tmpA5B->f1=0,((_tmpA5B->f2=e,_tmpA5B)))));};}
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
void*_tmp2CF=x->r;void*_tmp2D0=_tmp2CF;struct _tuple1*_tmp2D1;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D0)->tag == 1){_LL1A3: _tmp2D1=((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D0)->f1;_LL1A4:
# 1514
 return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp2D1,t,eo,s,0),0);}else{_LL1A5: _LL1A6: {
# 1516
const char*_tmpA5E;void*_tmpA5D;(_tmpA5D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA5E="bogus x to make_struct",_tag_dyneither(_tmpA5E,sizeof(char),23))),_tag_dyneither(_tmpA5D,sizeof(void*),0)));}}_LL1A2:;}else{
# 1520
if(eo != 0)
return Cyc_Absyn_stmt_exp(Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(x,eo,0),0),s,0),0);else{
# 1523
return Cyc_Absyn_stmt_exp(s,0);}}}
# 1527
static int Cyc_Toc_abstract_aggr(void*typ){
void*_tmp2D4=Cyc_Tcutil_compress(typ);void*_tmp2D5=_tmp2D4;struct Cyc_List_List*_tmp2D6;union Cyc_Absyn_AggrInfoU _tmp2D7;struct Cyc_List_List*_tmp2D8;switch(*((int*)_tmp2D5)){case 11: _LL1A8: _tmp2D7=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2D5)->f1).aggr_info;_tmp2D8=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2D5)->f1).targs;_LL1A9: {
# 1530
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(_tmp2D7);
if(ad->impl == 0)return 0;{
struct Cyc_List_List*aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
_tmp2D6=aggrfields;goto _LL1AB;};}case 12: _LL1AA: _tmp2D6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D5)->f2;_LL1AB:
# 1535
 if(_tmp2D6 == 0)return 0;
return 1;default: _LL1AC: _LL1AD:
# 1541
 return 0;}_LL1A7:;}
# 1545
static void*Cyc_Toc_array_elt_type(void*t){
void*_tmp2D9=Cyc_Tcutil_compress(t);void*_tmp2DA=_tmp2D9;void*_tmp2DB;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2DA)->tag == 8){_LL1AF: _tmp2DB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2DA)->f1).elt_type;_LL1B0:
 return _tmp2DB;}else{_LL1B1: _LL1B2: {
const char*_tmpA62;void*_tmpA61[1];struct Cyc_String_pa_PrintArg_struct _tmpA60;(_tmpA60.tag=0,((_tmpA60.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA61[0]=& _tmpA60,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA62="array_elt_type called on %s",_tag_dyneither(_tmpA62,sizeof(char),28))),_tag_dyneither(_tmpA61,sizeof(void*),1)))))));}}_LL1AE:;}
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
struct _tuple19*_tmp2E0=(struct _tuple19*)_tmp2DF->hd;struct _tuple19*_tmp2E1=_tmp2E0;struct Cyc_List_List*_tmp2E2;struct Cyc_Absyn_Exp*_tmp2E3;_LL1B4: _tmp2E2=_tmp2E1->f1;_tmp2E3=_tmp2E1->f2;_LL1B5:;{
# 1576
struct Cyc_Absyn_Exp*e_index;
if(_tmp2E2 == 0)
e_index=Cyc_Absyn_signed_int_exp(count --,0);else{
# 1580
if(_tmp2E2->tl != 0){const char*_tmpA65;void*_tmpA64;(_tmpA64=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpA65="multiple designators in array",_tag_dyneither(_tmpA65,sizeof(char),30))),_tag_dyneither(_tmpA64,sizeof(void*),0)));}{
void*_tmp2E6=(void*)_tmp2E2->hd;
void*_tmp2E7=_tmp2E6;struct Cyc_Absyn_Exp*_tmp2E8;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2E7)->tag == 0){_LL1B7: _tmp2E8=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2E7)->f1;_LL1B8:
# 1584
 Cyc_Toc_exp_to_c(nv,_tmp2E8);
e_index=_tmp2E8;
goto _LL1B6;}else{_LL1B9: _LL1BA: {
const char*_tmpA68;void*_tmpA67;(_tmpA67=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpA68="field name designators in array",_tag_dyneither(_tmpA68,sizeof(char),32))),_tag_dyneither(_tmpA67,sizeof(void*),0)));}}_LL1B6:;};}{
# 1590
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);
void*_tmp2EB=_tmp2E3->r;void*_tmp2EC=_tmp2EB;void*_tmp2ED;struct Cyc_List_List*_tmp2EE;struct Cyc_Absyn_Vardecl*_tmp2EF;struct Cyc_Absyn_Exp*_tmp2F0;struct Cyc_Absyn_Exp*_tmp2F1;int _tmp2F2;struct Cyc_List_List*_tmp2F3;switch(*((int*)_tmp2EC)){case 25: _LL1BC: _tmp2F3=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp2EC)->f1;_LL1BD:
# 1593
 s=Cyc_Toc_init_array(nv,orig_elt_type,lval,_tmp2F3,s);
goto _LL1BB;case 26: _LL1BE: _tmp2EF=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2EC)->f1;_tmp2F0=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2EC)->f2;_tmp2F1=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2EC)->f3;_tmp2F2=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp2EC)->f4;_LL1BF:
# 1596
 s=Cyc_Toc_init_comprehension(nv,orig_elt_type,lval,_tmp2EF,_tmp2F0,_tmp2F1,_tmp2F2,s,0);
# 1598
goto _LL1BB;case 29: _LL1C0: _tmp2ED=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2EC)->f1;_tmp2EE=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp2EC)->f2;_LL1C1:
# 1600
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp2ED,_tmp2EE,s);
goto _LL1BB;case 27: _LL1C2: _LL1C3:
# 1603
 goto _LL1BB;default: _LL1C4: _LL1C5:
# 1605
 Cyc_Toc_exp_to_c(nv,_tmp2E3);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(orig_elt_type),_tmp2E3),0),s,0);
goto _LL1BB;}_LL1BB:;};};}}
# 1611
return s;}
# 1616
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated){
# 1621
struct _tuple1*_tmp2F4=vd->name;
void*expected_elt_type;
{void*_tmp2F5=Cyc_Tcutil_compress(comprehension_type);void*_tmp2F6=_tmp2F5;void*_tmp2F7;void*_tmp2F8;switch(*((int*)_tmp2F6)){case 8: _LL1C7: _tmp2F8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F6)->f1).elt_type;_LL1C8:
# 1625
 expected_elt_type=_tmp2F8;goto _LL1C6;case 5: _LL1C9: _tmp2F7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F6)->f1).elt_typ;_LL1CA:
# 1627
 expected_elt_type=_tmp2F7;goto _LL1C6;default: _LL1CB: _LL1CC: {
const char*_tmpA6B;void*_tmpA6A;(_tmpA6A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA6B="init_comprehension passed a bad type!",_tag_dyneither(_tmpA6B,sizeof(char),38))),_tag_dyneither(_tmpA6A,sizeof(void*),0)));}}_LL1C6:;}{
# 1630
void*_tmp2FB=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));
if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);
# 1634
{void*_tmp2FC=e2->r;void*_tmp2FD=_tmp2FC;if(((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp2FD)->tag == 27){_LL1CE: _LL1CF:
 return Cyc_Absyn_skip_stmt(0);}else{_LL1D0: _LL1D1:
 goto _LL1CD;}_LL1CD:;}{
# 1638
struct _RegionHandle _tmp2FE=_new_region("r2");struct _RegionHandle*r2=& _tmp2FE;_push_region(r2);
{struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpA6E;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpA6D;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(r2,nv,_tmp2F4,Cyc_Absyn_varb_exp(_tmp2F4,(void*)((_tmpA6D=_cycalloc(sizeof(*_tmpA6D)),((_tmpA6D[0]=((_tmpA6E.tag=4,((_tmpA6E.f1=vd,_tmpA6E)))),_tmpA6D)))),0));
struct _tuple1*max=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp2F4,0),Cyc_Absyn_signed_int_exp(0,0),0);
struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp2F4,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp2F4,0),0);
# 1646
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp2F4,0),0);
struct Cyc_Absyn_Stmt*body;
# 1650
{void*_tmp2FF=e2->r;void*_tmp300=_tmp2FF;void*_tmp301;struct Cyc_List_List*_tmp302;struct Cyc_Absyn_Vardecl*_tmp303;struct Cyc_Absyn_Exp*_tmp304;struct Cyc_Absyn_Exp*_tmp305;int _tmp306;struct Cyc_List_List*_tmp307;switch(*((int*)_tmp300)){case 25: _LL1D3: _tmp307=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_LL1D4:
# 1652
 body=Cyc_Toc_init_array(nv2,expected_elt_type,lval,_tmp307,Cyc_Toc_skip_stmt_dl());
goto _LL1D2;case 26: _LL1D5: _tmp303=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_tmp304=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp300)->f2;_tmp305=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp300)->f3;_tmp306=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp300)->f4;_LL1D6:
# 1655
 body=Cyc_Toc_init_comprehension(nv2,expected_elt_type,lval,_tmp303,_tmp304,_tmp305,_tmp306,Cyc_Toc_skip_stmt_dl(),0);
goto _LL1D2;case 29: _LL1D7: _tmp301=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp300)->f1;_tmp302=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp300)->f2;_LL1D8:
# 1658
 body=Cyc_Toc_init_anon_struct(nv,lval,_tmp301,_tmp302,Cyc_Toc_skip_stmt_dl());
goto _LL1D2;default: _LL1D9: _LL1DA:
# 1661
 Cyc_Toc_exp_to_c(nv2,e2);
body=Cyc_Absyn_assign_stmt(lval,Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(expected_elt_type),e2),0);
goto _LL1D2;}_LL1D2:;}{
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
struct _tuple19*_tmp30C=(struct _tuple19*)_tmp30B->hd;struct _tuple19*_tmp30D=_tmp30C;struct Cyc_List_List*_tmp30E;struct Cyc_Absyn_Exp*_tmp30F;_LL1DC: _tmp30E=_tmp30D->f1;_tmp30F=_tmp30D->f2;_LL1DD:;
if(_tmp30E == 0){
const char*_tmpA71;void*_tmpA70;(_tmpA70=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA71="empty designator list",_tag_dyneither(_tmpA71,sizeof(char),22))),_tag_dyneither(_tmpA70,sizeof(void*),0)));}
if(_tmp30E->tl != 0){
const char*_tmpA74;void*_tmpA73;(_tmpA73=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA74="too many designators in anonymous struct",_tag_dyneither(_tmpA74,sizeof(char),41))),_tag_dyneither(_tmpA73,sizeof(void*),0)));}{
void*_tmp314=(void*)_tmp30E->hd;void*_tmp315=_tmp314;struct _dyneither_ptr*_tmp316;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp315)->tag == 1){_LL1DF: _tmp316=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp315)->f1;_LL1E0: {
# 1694
struct Cyc_Absyn_Exp*lval=Cyc_Toc_member_exp(lhs,_tmp316,0);
{void*_tmp317=_tmp30F->r;void*_tmp318=_tmp317;void*_tmp319;struct Cyc_List_List*_tmp31A;struct Cyc_Absyn_Vardecl*_tmp31B;struct Cyc_Absyn_Exp*_tmp31C;struct Cyc_Absyn_Exp*_tmp31D;int _tmp31E;struct Cyc_List_List*_tmp31F;switch(*((int*)_tmp318)){case 25: _LL1E4: _tmp31F=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp318)->f1;_LL1E5:
# 1697
 s=Cyc_Toc_init_array(nv,(void*)_check_null(_tmp30F->topt),lval,_tmp31F,s);goto _LL1E3;case 26: _LL1E6: _tmp31B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp318)->f1;_tmp31C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp318)->f2;_tmp31D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp318)->f3;_tmp31E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp318)->f4;_LL1E7:
# 1699
 s=Cyc_Toc_init_comprehension(nv,(void*)_check_null(_tmp30F->topt),lval,_tmp31B,_tmp31C,_tmp31D,_tmp31E,s,0);
goto _LL1E3;case 29: _LL1E8: _tmp319=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp318)->f1;_tmp31A=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp318)->f2;_LL1E9:
# 1702
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp319,_tmp31A,s);goto _LL1E3;case 27: _LL1EA: _LL1EB:
# 1704
 goto _LL1E3;default: _LL1EC: _LL1ED:
# 1706
 Cyc_Toc_exp_to_c(nv,_tmp30F);
# 1708
if(Cyc_Toc_is_poly_field(struct_type,_tmp316))
_tmp30F=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp30F);
# 1711
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp30F,0),0),s,0);
goto _LL1E3;}_LL1E3:;}
# 1714
goto _LL1DE;}}else{_LL1E1: _LL1E2: {
const char*_tmpA77;void*_tmpA76;(_tmpA76=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA77="array designator in struct",_tag_dyneither(_tmpA77,sizeof(char),27))),_tag_dyneither(_tmpA76,sizeof(void*),0)));}}_LL1DE:;};}}
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
void*_tmp32A=e->r;void*_tmp32B=_tmp32A;struct Cyc_Absyn_Vardecl*_tmp32C;struct Cyc_Absyn_Exp*_tmp32D;struct Cyc_Absyn_Exp*_tmp32E;int _tmp32F;struct Cyc_List_List*_tmp330;switch(*((int*)_tmp32B)){case 25: _LL1EF: _tmp330=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp32B)->f1;_LL1F0:
# 1745
 _tmp327=Cyc_Toc_init_array(nv,(void*)_check_null(e->topt),lval,_tmp330,_tmp327);
goto _LL1EE;case 26: _LL1F1: _tmp32C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp32B)->f1;_tmp32D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp32B)->f2;_tmp32E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp32B)->f3;_tmp32F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp32B)->f4;_LL1F2:
# 1748
 _tmp327=Cyc_Toc_init_comprehension(nv,(void*)_check_null(e->topt),lval,_tmp32C,_tmp32D,_tmp32E,_tmp32F,_tmp327,0);
# 1750
goto _LL1EE;case 27: _LL1F3: _LL1F4:
# 1752
 goto _LL1EE;default: _LL1F5: _LL1F6:
# 1755
 Cyc_Toc_exp_to_c(nv,e);
_tmp327=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp328(_tmp326,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp327,0);
# 1758
goto _LL1EE;}_LL1EE:;};}}{
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
struct Cyc_String_pa_PrintArg_struct _tmpA7F;void*_tmpA7E[1];const char*_tmpA7D;void*_tmpA7C;(_tmpA7C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((struct _dyneither_ptr)((_tmpA7F.tag=0,((_tmpA7F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpA7E[0]=& _tmpA7F,Cyc_aprintf(((_tmpA7D="get_member_offset %s failed",_tag_dyneither(_tmpA7D,sizeof(char),28))),_tag_dyneither(_tmpA7E,sizeof(void*),1)))))))),_tag_dyneither(_tmpA7C,sizeof(void*),0)));};}
# 1775
static int Cyc_Toc_init_variable_array(void**typ){
{void*_tmp338=Cyc_Tcutil_compress(*typ);void*_tmp339=_tmp338;void*_tmp33A;struct Cyc_Absyn_Tqual _tmp33B;struct Cyc_Absyn_Exp*_tmp33C;union Cyc_Absyn_Constraint*_tmp33D;unsigned int _tmp33E;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp339)->tag == 8){_LL1F8: _tmp33A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp339)->f1).elt_type;_tmp33B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp339)->f1).tq;_tmp33C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp339)->f1).num_elts;_tmp33D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp339)->f1).zero_term;_tmp33E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp339)->f1).zt_loc;_LL1F9:
# 1780
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp33C))){
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpA85;struct Cyc_Absyn_ArrayInfo _tmpA84;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpA83;*typ=(void*)((_tmpA83=_cycalloc(sizeof(*_tmpA83)),((_tmpA83[0]=((_tmpA85.tag=8,((_tmpA85.f1=((_tmpA84.elt_type=_tmp33A,((_tmpA84.tq=_tmp33B,((_tmpA84.num_elts=Cyc_Absyn_uint_exp(0,0),((_tmpA84.zero_term=_tmp33D,((_tmpA84.zt_loc=_tmp33E,_tmpA84)))))))))),_tmpA85)))),_tmpA83))));}
return 1;}
# 1784
goto _LL1F7;}else{_LL1FA: _LL1FB:
 goto _LL1F7;}_LL1F7:;}
# 1787
return 0;}
# 1790
static int Cyc_Toc_is_array_type(void*t){
void*_tmp342=Cyc_Tcutil_compress(t);void*_tmp343=_tmp342;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp343)->tag == 8){_LL1FD: _LL1FE:
 return 1;}else{_LL1FF: _LL200:
 return 0;}_LL1FC:;}
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
struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpA8B;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpA8A;struct Cyc_List_List*_tmpA89;_tmp345=((_tmpA89=_cycalloc(sizeof(*_tmpA89)),((_tmpA89->hd=(void*)((_tmpA8B=_cycalloc(sizeof(*_tmpA8B)),((_tmpA8B[0]=((_tmpA8A.tag=6,((_tmpA8A.f1=0,_tmpA8A)))),_tmpA8B)))),((_tmpA89->tl=_tmp345,_tmpA89))))));}{
struct Cyc_Absyn_Aggrfield*_tmpA8C;struct Cyc_Absyn_Aggrfield*_tmp34A=(_tmpA8C=_cycalloc(sizeof(*_tmpA8C)),((_tmpA8C->name=_tmp344->name,((_tmpA8C->tq=Cyc_Toc_mt_tq,((_tmpA8C->type=new_field_type,((_tmpA8C->width=_tmp344->width,((_tmpA8C->attributes=_tmp345,((_tmpA8C->requires_clause=0,_tmpA8C)))))))))))));
# 1815
struct Cyc_List_List*_tmpA8D;new_fields=((_tmpA8D=_cycalloc(sizeof(*_tmpA8D)),((_tmpA8D->hd=_tmp34A,((_tmpA8D->tl=new_fields,_tmpA8D))))));};}else{
# 1817
struct Cyc_List_List*_tmpA8E;new_fields=((_tmpA8E=_cycalloc(sizeof(*_tmpA8E)),((_tmpA8E->hd=(struct Cyc_Absyn_Aggrfield*)fields->hd,((_tmpA8E->tl=new_fields,_tmpA8E))))));}}{
# 1819
struct Cyc_List_List*_tmp34E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);
struct Cyc_Int_pa_PrintArg_struct _tmpA96;void*_tmpA95[1];const char*_tmpA94;struct _dyneither_ptr*_tmpA93;struct _dyneither_ptr*name=
(_tmpA93=_cycalloc(sizeof(*_tmpA93)),((_tmpA93[0]=(struct _dyneither_ptr)((_tmpA96.tag=1,((_tmpA96.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpA95[0]=& _tmpA96,Cyc_aprintf(((_tmpA94="_genStruct%d",_tag_dyneither(_tmpA94,sizeof(char),13))),_tag_dyneither(_tmpA95,sizeof(void*),1)))))))),_tmpA93)));
struct _tuple1*_tmpA97;struct _tuple1*qv=(_tmpA97=_cycalloc(sizeof(*_tmpA97)),((_tmpA97->f1=Cyc_Absyn_Abs_n(0,1),((_tmpA97->f2=name,_tmpA97)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpA98;struct Cyc_Absyn_AggrdeclImpl*_tmp34F=(_tmpA98=_cycalloc(sizeof(*_tmpA98)),((_tmpA98->exist_vars=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,((_tmpA98->rgn_po=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po,((_tmpA98->fields=_tmp34E,((_tmpA98->tagged=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged,_tmpA98)))))))));
# 1827
struct Cyc_Absyn_Aggrdecl*_tmpA99;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpA99=_cycalloc(sizeof(*_tmpA99)),((_tmpA99->kind=ad->kind,((_tmpA99->sc=ad->sc,((_tmpA99->name=qv,((_tmpA99->tvs=ad->tvs,((_tmpA99->impl=_tmp34F,((_tmpA99->expected_mem_kind=0,((_tmpA99->attributes=ad->attributes,_tmpA99)))))))))))))));
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
{void*_tmp359=_tmp358->r;void*_tmp35A=_tmp359;struct Cyc_Absyn_Exp**_tmp35B;if(((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp35A)->tag == 1){_LL202: _tmp35B=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp35A)->f1;_LL203:
 varexp=_tmp35B;goto _LL201;}else{_LL204: _LL205: {
const char*_tmpA9C;void*_tmpA9B;(_tmpA9B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA9C="stmt not an expression!",_tag_dyneither(_tmpA9C,sizeof(char),24))),_tag_dyneither(_tmpA9B,sizeof(void*),0)));}}_LL201:;}{
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
{void*_tmp35F=Cyc_Tcutil_compress(struct_type);void*_tmp360=_tmp35F;union Cyc_Absyn_AggrInfoU _tmp361;struct Cyc_List_List*_tmp362;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp360)->tag == 11){_LL207: _tmp361=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp360)->f1).aggr_info;_tmp362=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp360)->f1).targs;_LL208:
# 1876
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp361);
orig_aggrfields=(aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
forall_types=_tmp362;
goto _LL206;}else{_LL209: _LL20A: {
const char*_tmpA9F;void*_tmpA9E;(_tmpA9E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA9F="init_struct: bad struct type",_tag_dyneither(_tmpA9F,sizeof(char),29))),_tag_dyneither(_tmpA9E,sizeof(void*),0)));}}_LL206:;}
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
struct Cyc_Absyn_Aggrfield*_tmpAA0;struct Cyc_Absyn_Aggrfield*_tmp36C=(_tmpAA0=_cycalloc(sizeof(*_tmpAA0)),((_tmpAA0->name=_tmp36B->name,((_tmpAA0->tq=Cyc_Toc_mt_tq,((_tmpAA0->type=
# 1901
Cyc_Tcutil_rsubstitute(temp,_tmp36A,_tmp36B->type),((_tmpAA0->width=_tmp36B->width,((_tmpAA0->attributes=_tmp36B->attributes,((_tmpAA0->requires_clause=0,_tmpAA0)))))))))))));
# 1909
if(_tmp365->tl == 0){
if(Cyc_Toc_init_variable_array(& _tmp36C->type)){
struct _dyneither_ptr**_tmpAA1;index=((_tmpAA1=_cycalloc(sizeof(*_tmpAA1)),((_tmpAA1[0]=_tmp36C->name,_tmpAA1))));}}{
# 1916
struct Cyc_List_List*_tmpAA2;new_fields=((_tmpAA2=_cycalloc(sizeof(*_tmpAA2)),((_tmpAA2->hd=_tmp36C,((_tmpAA2->tl=new_fields,_tmpAA2))))));};}
# 1918
exist_types=0;
aggrfields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);{
struct Cyc_Int_pa_PrintArg_struct _tmpAAA;void*_tmpAA9[1];const char*_tmpAA8;struct _dyneither_ptr*_tmpAA7;struct _dyneither_ptr*name=
(_tmpAA7=_cycalloc(sizeof(*_tmpAA7)),((_tmpAA7[0]=(struct _dyneither_ptr)((_tmpAAA.tag=1,((_tmpAAA.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpAA9[0]=& _tmpAAA,Cyc_aprintf(((_tmpAA8="_genStruct%d",_tag_dyneither(_tmpAA8,sizeof(char),13))),_tag_dyneither(_tmpAA9,sizeof(void*),1)))))))),_tmpAA7)));
struct _tuple1*_tmpAAB;struct _tuple1*qv=(_tmpAAB=_cycalloc(sizeof(*_tmpAAB)),((_tmpAAB->f1=Cyc_Absyn_Abs_n(0,1),((_tmpAAB->f2=name,_tmpAAB)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpAAC;struct Cyc_Absyn_AggrdeclImpl*_tmp370=(_tmpAAC=_cycalloc(sizeof(*_tmpAAC)),((_tmpAAC->exist_vars=0,((_tmpAAC->rgn_po=0,((_tmpAAC->fields=aggrfields,((_tmpAAC->tagged=0,_tmpAAC)))))))));
# 1927
struct Cyc_Absyn_Aggrdecl*_tmpAAD;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpAAD=_cycalloc(sizeof(*_tmpAAD)),((_tmpAAD->kind=Cyc_Absyn_StructA,((_tmpAAD->sc=Cyc_Absyn_Public,((_tmpAAD->name=qv,((_tmpAAD->tvs=0,((_tmpAAD->expected_mem_kind=0,((_tmpAAD->impl=_tmp370,((_tmpAAD->attributes=0,_tmpAAD)))))))))))))));
# 1934
Cyc_Toc_aggrdecl_to_c(new_ad,1);
ad=new_ad;
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpAB7;struct Cyc_Absyn_Aggrdecl**_tmpAB6;struct Cyc_Absyn_AggrInfo _tmpAB5;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpAB4;struct_type=(void*)((_tmpAB4=_cycalloc(sizeof(*_tmpAB4)),((_tmpAB4[0]=((_tmpAB7.tag=11,((_tmpAB7.f1=((_tmpAB5.aggr_info=Cyc_Absyn_KnownAggr(((_tmpAB6=_cycalloc(sizeof(*_tmpAB6)),((_tmpAB6[0]=new_ad,_tmpAB6))))),((_tmpAB5.targs=0,_tmpAB5)))),_tmpAB7)))),_tmpAB4))));}
# 1938
aggr_typ=Cyc_Toc_typ_to_c(struct_type);};}}
# 1888
;_pop_region(temp);};}{
# 1942
int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct _RegionHandle _tmp37C=_new_region("r");struct _RegionHandle*r=& _tmp37C;_push_region(r);
{struct Cyc_List_List*_tmp37D=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp37D != 0;_tmp37D=_tmp37D->tl){
struct _tuple19*_tmp37E=(struct _tuple19*)_tmp37D->hd;struct _tuple19*_tmp37F=_tmp37E;struct Cyc_List_List*_tmp380;struct Cyc_Absyn_Exp*_tmp381;_LL20C: _tmp380=_tmp37F->f1;_tmp381=_tmp37F->f2;_LL20D:;
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(_tmp381->topt));
if(_tmp380 == 0){
const char*_tmpABA;void*_tmpAB9;(_tmpAB9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpABA="empty designator list",_tag_dyneither(_tmpABA,sizeof(char),22))),_tag_dyneither(_tmpAB9,sizeof(void*),0)));}
if(_tmp380->tl != 0){
# 1952
struct _tuple1*_tmp384=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp385=Cyc_Absyn_var_exp(_tmp384,0);
for(0;_tmp380 != 0;_tmp380=_tmp380->tl){
void*_tmp386=(void*)_tmp380->hd;void*_tmp387=_tmp386;struct _dyneither_ptr*_tmp388;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp387)->tag == 1){_LL20F: _tmp388=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp387)->f1;_LL210:
# 1958
 if(Cyc_Toc_is_poly_field(struct_type,_tmp388))
_tmp385=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp385);
_tmp358=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp35E(xexp,_tmp388,0),_tmp385,0),0),_tmp358,0);
# 1962
goto _LL20E;}else{_LL211: _LL212: {
const char*_tmpABD;void*_tmpABC;(_tmpABC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpABD="array designator in struct",_tag_dyneither(_tmpABD,sizeof(char),27))),_tag_dyneither(_tmpABC,sizeof(void*),0)));}}_LL20E:;}
# 1966
Cyc_Toc_exp_to_c(nv,_tmp381);
_tmp358=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp385,_tmp381,0),0),_tmp358,0);}else{
# 1969
void*_tmp38B=(void*)_tmp380->hd;void*_tmp38C=_tmp38B;struct _dyneither_ptr*_tmp38D;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp38C)->tag == 1){_LL214: _tmp38D=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp38C)->f1;_LL215: {
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
{void*_tmp38E=_tmp381->r;void*_tmp38F=_tmp38E;void*_tmp390;struct Cyc_List_List*_tmp391;struct Cyc_Absyn_Exp*_tmp392;void*_tmp393;struct Cyc_Absyn_Vardecl*_tmp394;struct Cyc_Absyn_Exp*_tmp395;struct Cyc_Absyn_Exp*_tmp396;int _tmp397;struct Cyc_List_List*_tmp398;switch(*((int*)_tmp38F)){case 25: _LL219: _tmp398=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp38F)->f1;_LL21A:
# 1983
 _tmp358=Cyc_Toc_init_array(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(aggrfields,_tmp38D)))->type,lval,_tmp398,_tmp358);
# 1985
goto _LL218;case 26: _LL21B: _tmp394=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp38F)->f1;_tmp395=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp38F)->f2;_tmp396=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp38F)->f3;_tmp397=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp38F)->f4;_LL21C:
# 1987
 _tmp358=Cyc_Toc_init_comprehension(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(do_declare?aggrfields: orig_aggrfields,_tmp38D)))->type,lval,_tmp394,_tmp395,_tmp396,_tmp397,_tmp358,0);
# 1991
e1_translated=1;
_tmp392=_tmp395;_tmp393=(void*)_check_null(_tmp396->topt);goto _LL21E;case 27: _LL21D: _tmp392=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp38F)->f1;_tmp393=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp38F)->f2;_LL21E:
# 1996
 if(index != 0  && !Cyc_strptrcmp(*index,_tmp38D)){
if(!e1_translated)Cyc_Toc_exp_to_c(nv,_tmp392);{
void*_tmp399=Cyc_Toc_typ_to_c(_tmp393);
struct _tuple6 _tmpAC1;union Cyc_Absyn_Cnst _tmpAC0;struct Cyc_Absyn_Exp*_tmpABE[2];*(nv->struct_info).varsizeexp=
Cyc_Absyn_add_exp(Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpABE[1]=Cyc_Absyn_sizeoftyp_exp(_tmp399,0),((_tmpABE[0]=_tmp392,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpABE,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),
Cyc_Absyn_const_exp((((_tmpAC0.Int_c).val=((_tmpAC1.f1=Cyc_Absyn_Unsigned,((_tmpAC1.f2=(int)sizeof(double),_tmpAC1)))),(((_tmpAC0.Int_c).tag=5,_tmpAC0)))),0),0);};}
# 2004
goto _LL218;case 29: _LL21F: _tmp390=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp38F)->f1;_tmp391=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp38F)->f2;_LL220:
# 2006
 _tmp358=Cyc_Toc_init_anon_struct(nv,lval,_tmp390,_tmp391,_tmp358);goto _LL218;default: _LL221: _LL222: {
# 2008
void*old_e_typ=(void*)_check_null(_tmp381->topt);
int was_ptr_type=Cyc_Toc_is_pointer_or_boxed_tvar(old_e_typ);
Cyc_Toc_exp_to_c(Cyc_Toc_set_lhs_exp(nv->rgn,lval,nv),_tmp381);
{void*_tmp39D=old_e_typ;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp39D)->tag == 11){_LL224: _LL225:
# 2015
 if(old_e_typ != _tmp381->topt  && !Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(old_e_typ),(void*)_check_null(_tmp381->topt))){
# 2019
ad=Cyc_Toc_update_aggr_type(ad,_tmp38D,(void*)_check_null(_tmp381->topt));
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpACB;struct Cyc_Absyn_Aggrdecl**_tmpACA;struct Cyc_Absyn_AggrInfo _tmpAC9;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpAC8;struct_type=(void*)(
(_tmpAC8=_cycalloc(sizeof(*_tmpAC8)),((_tmpAC8[0]=((_tmpACB.tag=11,((_tmpACB.f1=((_tmpAC9.aggr_info=Cyc_Absyn_KnownAggr(((_tmpACA=_cycalloc(sizeof(*_tmpACA)),((_tmpACA[0]=ad,_tmpACA))))),((_tmpAC9.targs=0,_tmpAC9)))),_tmpACB)))),_tmpAC8))));}
# 2023
aggr_typ=Cyc_Toc_typ_to_c(struct_type);}
# 2025
goto _LL223;}else{_LL226: _LL227:
 goto _LL223;}_LL223:;}{
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
goto _LL218;};}}_LL218:;}
# 2039
goto _LL213;};}}else{_LL216: _LL217: {
const char*_tmpACE;void*_tmpACD;(_tmpACD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpACE="array designator in struct",_tag_dyneither(_tmpACE,sizeof(char),27))),_tag_dyneither(_tmpACD,sizeof(void*),0)));}}_LL213:;}}}
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
struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmpAD1;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmpAD0;return Cyc_Absyn_new_exp((void*)((_tmpAD0=_cycalloc(sizeof(*_tmpAD0)),((_tmpAD0[0]=((_tmpAD1.tag=4,((_tmpAD1.f1=e1,((_tmpAD1.f2=incr,_tmpAD1)))))),_tmpAD0)))),0);}
# 2069
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 2078
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 2080
void*_tmp3A9=e1->r;void*_tmp3AA=_tmp3A9;struct Cyc_Absyn_Exp*_tmp3AB;struct _dyneither_ptr*_tmp3AC;int _tmp3AD;int _tmp3AE;void*_tmp3AF;struct Cyc_Absyn_Exp*_tmp3B0;struct Cyc_Absyn_Stmt*_tmp3B1;switch(*((int*)_tmp3AA)){case 36: _LL229: _tmp3B1=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3AA)->f1;_LL22A:
 Cyc_Toc_lvalue_assign_stmt(_tmp3B1,fs,f,f_env);goto _LL228;case 13: _LL22B: _tmp3AF=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3AA)->f1;_tmp3B0=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3AA)->f2;_LL22C:
 Cyc_Toc_lvalue_assign(_tmp3B0,fs,f,f_env);goto _LL228;case 20: _LL22D: _tmp3AB=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3AA)->f1;_tmp3AC=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3AA)->f2;_tmp3AD=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3AA)->f3;_tmp3AE=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3AA)->f4;_LL22E:
# 2085
 e1->r=_tmp3AB->r;
{struct Cyc_List_List*_tmpAD2;Cyc_Toc_lvalue_assign(e1,((_tmpAD2=_cycalloc(sizeof(*_tmpAD2)),((_tmpAD2->hd=_tmp3AC,((_tmpAD2->tl=fs,_tmpAD2)))))),f,f_env);}
goto _LL228;default: _LL22F: _LL230: {
# 2093
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 2095
for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;
goto _LL228;}}_LL228:;}
# 2101
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 2103
void*_tmp3B3=s->r;void*_tmp3B4=_tmp3B3;struct Cyc_Absyn_Stmt*_tmp3B5;struct Cyc_Absyn_Decl*_tmp3B6;struct Cyc_Absyn_Stmt*_tmp3B7;struct Cyc_Absyn_Exp*_tmp3B8;switch(*((int*)_tmp3B4)){case 1: _LL232: _tmp3B8=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3B4)->f1;_LL233:
 Cyc_Toc_lvalue_assign(_tmp3B8,fs,f,f_env);goto _LL231;case 12: _LL234: _tmp3B6=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3B4)->f1;_tmp3B7=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3B4)->f2;_LL235:
# 2106
 Cyc_Toc_lvalue_assign_stmt(_tmp3B7,fs,f,f_env);goto _LL231;case 2: _LL236: _tmp3B5=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3B4)->f2;_LL237:
 Cyc_Toc_lvalue_assign_stmt(_tmp3B5,fs,f,f_env);goto _LL231;default: _LL238: _LL239: {
const char*_tmpAD6;void*_tmpAD5[1];struct Cyc_String_pa_PrintArg_struct _tmpAD4;(_tmpAD4.tag=0,((_tmpAD4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpAD5[0]=& _tmpAD4,Cyc_Toc_toc_impos(((_tmpAD6="lvalue_assign_stmt: %s",_tag_dyneither(_tmpAD6,sizeof(char),23))),_tag_dyneither(_tmpAD5,sizeof(void*),1)))))));}}_LL231:;}
# 2112
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 2116
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp3BC=e->r;void*_tmp3BD=_tmp3BC;struct Cyc_Absyn_Stmt*_tmp3BE;struct Cyc_Absyn_Exp*_tmp3BF;void**_tmp3C0;struct Cyc_Absyn_Exp**_tmp3C1;switch(*((int*)_tmp3BD)){case 13: _LL23B: _tmp3C0=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3BD)->f1;_tmp3C1=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3BD)->f2;_LL23C:
# 2119
*_tmp3C1=Cyc_Toc_push_address_exp(*_tmp3C1);
*_tmp3C0=Cyc_Absyn_cstar_typ(*_tmp3C0,Cyc_Toc_mt_tq);
return e;case 19: _LL23D: _tmp3BF=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3BD)->f1;_LL23E:
# 2123
 return _tmp3BF;case 36: _LL23F: _tmp3BE=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3BD)->f1;_LL240:
# 2127
 Cyc_Toc_push_address_stmt(_tmp3BE);
return e;default: _LL241: _LL242:
# 2130
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{
const char*_tmpADA;void*_tmpAD9[1];struct Cyc_String_pa_PrintArg_struct _tmpAD8;(_tmpAD8.tag=0,((_tmpAD8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpAD9[0]=& _tmpAD8,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpADA="can't take & of exp %s",_tag_dyneither(_tmpADA,sizeof(char),23))),_tag_dyneither(_tmpAD9,sizeof(void*),1)))))));};}_LL23A:;}
# 2135
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp3C5=s->r;void*_tmp3C6=_tmp3C5;struct Cyc_Absyn_Exp**_tmp3C7;struct Cyc_Absyn_Stmt*_tmp3C8;struct Cyc_Absyn_Stmt*_tmp3C9;switch(*((int*)_tmp3C6)){case 2: _LL244: _tmp3C9=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3C6)->f2;_LL245:
 _tmp3C8=_tmp3C9;goto _LL247;case 12: _LL246: _tmp3C8=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3C6)->f2;_LL247:
 Cyc_Toc_push_address_stmt(_tmp3C8);goto _LL243;case 1: _LL248: _tmp3C7=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3C6)->f1;_LL249:
*_tmp3C7=Cyc_Toc_push_address_exp(*_tmp3C7);goto _LL243;default: _LL24A: _LL24B: {
# 2141
const char*_tmpADE;void*_tmpADD[1];struct Cyc_String_pa_PrintArg_struct _tmpADC;(_tmpADC.tag=0,((_tmpADC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpADD[0]=& _tmpADC,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpADE="can't take & of stmt %s",_tag_dyneither(_tmpADE,sizeof(char),24))),_tag_dyneither(_tmpADD,sizeof(void*),1)))))));}}_LL243:;}
# 2146
static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 2148
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 2150
if(x == 0)return 0;
{struct Cyc_List_List*_tmpADF;result=((_tmpADF=_region_malloc(r2,sizeof(*_tmpADF)),((_tmpADF->hd=f(x->hd,env),((_tmpADF->tl=0,_tmpADF))))));}
prev=result;
for(x=x->tl;x != 0;x=x->tl){
{struct Cyc_List_List*_tmpAE0;((struct Cyc_List_List*)_check_null(prev))->tl=((_tmpAE0=_region_malloc(r2,sizeof(*_tmpAE0)),((_tmpAE0->hd=f(x->hd,env),((_tmpAE0->tl=0,_tmpAE0))))));}
prev=prev->tl;}
# 2157
return result;}
# 2159
static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,x);}
# 2163
static struct _tuple19*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){
struct _tuple19*_tmpAE1;return(_tmpAE1=_cycalloc(sizeof(*_tmpAE1)),((_tmpAE1->f1=0,((_tmpAE1->f2=e,_tmpAE1)))));}
# 2167
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_tmp3D0=Cyc_Tcutil_compress(t);void*_tmp3D1=_tmp3D0;struct Cyc_Absyn_PtrInfo _tmp3D2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D1)->tag == 5){_LL24D: _tmp3D2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D1)->f1;_LL24E:
 return _tmp3D2;}else{_LL24F: _LL250: {
const char*_tmpAE4;void*_tmpAE3;(_tmpAE3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAE4="get_ptr_typ: not a pointer!",_tag_dyneither(_tmpAE4,sizeof(char),28))),_tag_dyneither(_tmpAE3,sizeof(void*),0)));}}_LL24C:;}
# 2177
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp3D5=Cyc_Tcutil_compress(t);void*_tmp3D6=_tmp3D5;int _tmp3D7;enum Cyc_Absyn_Sign _tmp3D8;enum Cyc_Absyn_Sign _tmp3D9;enum Cyc_Absyn_Sign _tmp3DA;enum Cyc_Absyn_Sign _tmp3DB;enum Cyc_Absyn_Sign _tmp3DC;switch(*((int*)_tmp3D6)){case 5: _LL252: _LL253:
 res=Cyc_Absyn_null_exp(0);goto _LL251;case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3D6)->f2){case Cyc_Absyn_Char_sz: _LL254: _tmp3DC=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3D6)->f1;_LL255:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp3DC,'\000'),0);goto _LL251;case Cyc_Absyn_Short_sz: _LL256: _tmp3DB=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3D6)->f1;_LL257:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp3DB,0),0);goto _LL251;case Cyc_Absyn_Int_sz: _LL25C: _tmp3DA=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3D6)->f1;_LL25D:
# 2185
 _tmp3D9=_tmp3DA;goto _LL25F;case Cyc_Absyn_Long_sz: _LL25E: _tmp3D9=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3D6)->f1;_LL25F:
# 2187
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp3D9,0),0);goto _LL251;default: _LL260: _tmp3D8=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3D6)->f1;_LL261:
# 2189
 res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp3D8,(long long)0),0);goto _LL251;}case 13: _LL258: _LL259:
# 2183
 goto _LL25B;case 14: _LL25A: _LL25B:
 _tmp3DA=Cyc_Absyn_Unsigned;goto _LL25D;case 7: switch(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3D6)->f1){case 0: _LL262: _LL263:
# 2190
{const char*_tmpAE5;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpAE5="0.0F",_tag_dyneither(_tmpAE5,sizeof(char),5))),0),0);}goto _LL251;case 1: _LL264: _LL265:
{const char*_tmpAE6;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpAE6="0.0",_tag_dyneither(_tmpAE6,sizeof(char),4))),1),0);}goto _LL251;default: _LL266: _tmp3D7=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3D6)->f1;_LL267:
{const char*_tmpAE7;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpAE7="0.0L",_tag_dyneither(_tmpAE7,sizeof(char),5))),_tmp3D7),0);}goto _LL251;}default: _LL268: _LL269: {
# 2194
const char*_tmpAEB;void*_tmpAEA[1];struct Cyc_String_pa_PrintArg_struct _tmpAE9;(_tmpAE9.tag=0,((_tmpAE9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpAEA[0]=& _tmpAE9,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAEB="found non-zero type %s in generate_zero",_tag_dyneither(_tmpAEB,sizeof(char),40))),_tag_dyneither(_tmpAEA,sizeof(void*),1)))))));}}_LL251:;}
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
struct Cyc_Core_Opt*_tmpAEC;struct Cyc_Core_Opt*_tmp3E6=(_tmpAEC=_cycalloc(sizeof(*_tmpAEC)),((_tmpAEC->v=_tmp3E5,_tmpAEC)));
struct Cyc_Absyn_Exp*xinit;
{void*_tmp3E7=e1->r;void*_tmp3E8=_tmp3E7;struct Cyc_Absyn_Exp*_tmp3E9;struct Cyc_Absyn_Exp*_tmp3EA;struct Cyc_Absyn_Exp*_tmp3EB;switch(*((int*)_tmp3E8)){case 19: _LL26B: _tmp3EB=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3E8)->f1;_LL26C:
# 2224
 if(!is_dyneither){
_tmp3EB=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp3EB,0,Cyc_Absyn_Other_coercion,0);
_tmp3EB->topt=fat_ptr_type;}
# 2228
Cyc_Toc_exp_to_c(nv,_tmp3EB);xinit=_tmp3EB;goto _LL26A;case 22: _LL26D: _tmp3E9=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3E8)->f1;_tmp3EA=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3E8)->f2;_LL26E:
# 2230
 if(!is_dyneither){
_tmp3E9=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp3E9,0,Cyc_Absyn_Other_coercion,0);
_tmp3E9->topt=fat_ptr_type;}
# 2234
Cyc_Toc_exp_to_c(nv,_tmp3E9);Cyc_Toc_exp_to_c(nv,_tmp3EA);
{struct Cyc_Absyn_Exp*_tmpAED[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpAED[2]=_tmp3EA,((_tmpAED[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpAED[0]=_tmp3E9,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAED,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL26A;default: _LL26F: _LL270: {
const char*_tmpAF0;void*_tmpAEF;(_tmpAEF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAF0="found bad lhs for zero-terminated pointer assignment",_tag_dyneither(_tmpAF0,sizeof(char),53))),_tag_dyneither(_tmpAEF,sizeof(void*),0)));}}_LL26A:;}{
# 2240
struct _tuple1*_tmp3EF=Cyc_Toc_temp_var();
struct _RegionHandle _tmp3F0=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp3F0;_push_region(rgn2);
{struct Cyc_Toc_Env*_tmp3F1=Cyc_Toc_add_varmap(rgn2,nv,_tmp3EF,Cyc_Absyn_var_exp(_tmp3EF,0));
struct Cyc_Absyn_Vardecl*_tmpAF1;struct Cyc_Absyn_Vardecl*_tmp3F2=(_tmpAF1=_cycalloc(sizeof(*_tmpAF1)),((_tmpAF1->sc=Cyc_Absyn_Public,((_tmpAF1->name=_tmp3EF,((_tmpAF1->tq=Cyc_Toc_mt_tq,((_tmpAF1->type=_tmp3E4,((_tmpAF1->initializer=xinit,((_tmpAF1->rgn=0,((_tmpAF1->attributes=0,((_tmpAF1->escapes=0,_tmpAF1)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpAF4;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpAF3;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp3F3=(_tmpAF3=_cycalloc(sizeof(*_tmpAF3)),((_tmpAF3[0]=((_tmpAF4.tag=4,((_tmpAF4.f1=_tmp3F2,_tmpAF4)))),_tmpAF3)));
struct Cyc_Absyn_Exp*_tmp3F4=Cyc_Absyn_varb_exp(_tmp3EF,(void*)_tmp3F3,0);
_tmp3F4->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp3F5=Cyc_Absyn_deref_exp(_tmp3F4,0);
_tmp3F5->topt=elt_type;
Cyc_Toc_exp_to_c(_tmp3F1,_tmp3F5);{
struct _tuple1*_tmp3F6=Cyc_Toc_temp_var();
_tmp3F1=Cyc_Toc_add_varmap(rgn2,_tmp3F1,_tmp3F6,Cyc_Absyn_var_exp(_tmp3F6,0));{
struct Cyc_Absyn_Vardecl*_tmpAF5;struct Cyc_Absyn_Vardecl*_tmp3F7=(_tmpAF5=_cycalloc(sizeof(*_tmpAF5)),((_tmpAF5->sc=Cyc_Absyn_Public,((_tmpAF5->name=_tmp3F6,((_tmpAF5->tq=Cyc_Toc_mt_tq,((_tmpAF5->type=_tmp3E3,((_tmpAF5->initializer=_tmp3F5,((_tmpAF5->rgn=0,((_tmpAF5->attributes=0,((_tmpAF5->escapes=0,_tmpAF5)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpAF8;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpAF7;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp3F8=(_tmpAF7=_cycalloc(sizeof(*_tmpAF7)),((_tmpAF7[0]=((_tmpAF8.tag=4,((_tmpAF8.f1=_tmp3F7,_tmpAF8)))),_tmpAF7)));
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp3F9=Cyc_Absyn_varb_exp(_tmp3F6,(void*)_tmp3F8,0);
_tmp3F9->topt=_tmp3F5->topt;
z_init=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp3F9,e2,0);
z_init->topt=_tmp3F9->topt;}
# 2261
Cyc_Toc_exp_to_c(_tmp3F1,z_init);{
struct _tuple1*_tmp3FA=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmpAF9;struct Cyc_Absyn_Vardecl*_tmp3FB=(_tmpAF9=_cycalloc(sizeof(*_tmpAF9)),((_tmpAF9->sc=Cyc_Absyn_Public,((_tmpAF9->name=_tmp3FA,((_tmpAF9->tq=Cyc_Toc_mt_tq,((_tmpAF9->type=_tmp3E3,((_tmpAF9->initializer=z_init,((_tmpAF9->rgn=0,((_tmpAF9->attributes=0,((_tmpAF9->escapes=0,_tmpAF9)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpAFC;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpAFB;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp3FC=(_tmpAFB=_cycalloc(sizeof(*_tmpAFB)),((_tmpAFB[0]=((_tmpAFC.tag=4,((_tmpAFC.f1=_tmp3FB,_tmpAFC)))),_tmpAFB)));
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
struct Cyc_Absyn_Exp*_tmpAFD[2];struct Cyc_List_List*_tmp403=(_tmpAFD[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpAFD[0]=
# 2280
Cyc_Absyn_varb_exp(_tmp3EF,(void*)_tmp3F3,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAFD,sizeof(struct Cyc_Absyn_Exp*),2)))));
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
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB03;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB02;struct Cyc_Absyn_Decl*_tmpB01;_tmp40A=Cyc_Absyn_decl_stmt(((_tmpB01=_cycalloc(sizeof(*_tmpB01)),((_tmpB01->r=(void*)((_tmpB03=_cycalloc(sizeof(*_tmpB03)),((_tmpB03[0]=((_tmpB02.tag=0,((_tmpB02.f1=_tmp3FB,_tmpB02)))),_tmpB03)))),((_tmpB01->loc=0,_tmpB01)))))),_tmp40A,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB09;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB08;struct Cyc_Absyn_Decl*_tmpB07;_tmp40A=Cyc_Absyn_decl_stmt(((_tmpB07=_cycalloc(sizeof(*_tmpB07)),((_tmpB07->r=(void*)((_tmpB09=_cycalloc(sizeof(*_tmpB09)),((_tmpB09[0]=((_tmpB08.tag=0,((_tmpB08.f1=_tmp3F7,_tmpB08)))),_tmpB09)))),((_tmpB07->loc=0,_tmpB07)))))),_tmp40A,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB0F;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB0E;struct Cyc_Absyn_Decl*_tmpB0D;_tmp40A=Cyc_Absyn_decl_stmt(((_tmpB0D=_cycalloc(sizeof(*_tmpB0D)),((_tmpB0D->r=(void*)((_tmpB0F=_cycalloc(sizeof(*_tmpB0F)),((_tmpB0F[0]=((_tmpB0E.tag=0,((_tmpB0E.f1=_tmp3F2,_tmpB0E)))),_tmpB0F)))),((_tmpB0D->loc=0,_tmpB0D)))))),_tmp40A,0);}
e->r=Cyc_Toc_stmt_exp_r(_tmp40A);};};};};};};};};};};};}
# 2242
;_pop_region(rgn2);};}
# 2313 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 2317
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp41F=Cyc_Tcutil_compress(aggrtype);void*_tmp420=_tmp41F;union Cyc_Absyn_AggrInfoU _tmp421;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp420)->tag == 11){_LL272: _tmp421=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp420)->f1).aggr_info;_LL273:
# 2320
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp421);goto _LL271;}else{_LL274: _LL275: {
struct Cyc_String_pa_PrintArg_struct _tmpB17;void*_tmpB16[1];const char*_tmpB15;void*_tmpB14;(_tmpB14=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmpB17.tag=0,((_tmpB17.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpB16[0]=& _tmpB17,Cyc_aprintf(((_tmpB15="expecting union but found %s in check_tagged_union",_tag_dyneither(_tmpB15,sizeof(char),51))),_tag_dyneither(_tmpB16,sizeof(void*),1)))))))),_tag_dyneither(_tmpB14,sizeof(void*),0)));}}_LL271:;}{
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
void*_tmp437=Cyc_Tcutil_compress(t);void*_tmp438=_tmp437;union Cyc_Absyn_AggrInfoU _tmp439;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp438)->tag == 11){_LL277: _tmp439=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp438)->f1).aggr_info;_LL278: {
# 2353
struct Cyc_Absyn_Aggrdecl*_tmp43A=Cyc_Absyn_get_known_aggrdecl(_tmp439);
*f_tag=Cyc_Toc_get_member_offset(_tmp43A,f);{
# 2356
const char*_tmpB1C;void*_tmpB1B[2];struct Cyc_String_pa_PrintArg_struct _tmpB1A;struct Cyc_String_pa_PrintArg_struct _tmpB19;struct _dyneither_ptr str=(struct _dyneither_ptr)((_tmpB19.tag=0,((_tmpB19.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpB1A.tag=0,((_tmpB1A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp43A->name).f2),((_tmpB1B[0]=& _tmpB1A,((_tmpB1B[1]=& _tmpB19,Cyc_aprintf(((_tmpB1C="_union_%s_%s",_tag_dyneither(_tmpB1C,sizeof(char),13))),_tag_dyneither(_tmpB1B,sizeof(void*),2))))))))))))));
{struct _dyneither_ptr*_tmpB1D;*tagged_member_type=Cyc_Absyn_strct(((_tmpB1D=_cycalloc(sizeof(*_tmpB1D)),((_tmpB1D[0]=str,_tmpB1D)))));}
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp43A->impl))->tagged;};}}else{_LL279: _LL27A:
 return 0;}_LL276:;}
# 2367
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 2370
void*_tmp440=e->r;void*_tmp441=_tmp440;struct Cyc_Absyn_Exp*_tmp442;struct _dyneither_ptr*_tmp443;int*_tmp444;struct Cyc_Absyn_Exp*_tmp445;struct _dyneither_ptr*_tmp446;int*_tmp447;struct Cyc_Absyn_Exp*_tmp448;switch(*((int*)_tmp441)){case 13: _LL27C: _tmp448=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp441)->f2;_LL27D: {
const char*_tmpB20;void*_tmpB1F;(_tmpB1F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB20="cast on lhs!",_tag_dyneither(_tmpB20,sizeof(char),13))),_tag_dyneither(_tmpB1F,sizeof(void*),0)));}case 20: _LL27E: _tmp445=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp441)->f1;_tmp446=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp441)->f2;_tmp447=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp441)->f4;_LL27F:
# 2373
 return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(_tmp445->topt),_tmp446,f_tag,tagged_member_type,clear_read,_tmp447);case 21: _LL280: _tmp442=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp441)->f1;_tmp443=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp441)->f2;_tmp444=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp441)->f4;_LL281: {
# 2376
void*_tmp44B=Cyc_Tcutil_compress((void*)_check_null(_tmp442->topt));void*_tmp44C=_tmp44B;void*_tmp44D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44C)->tag == 5){_LL285: _tmp44D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44C)->f1).elt_typ;_LL286:
# 2378
 return Cyc_Toc_is_tagged_union_project_impl(_tmp44D,_tmp443,f_tag,tagged_member_type,clear_read,_tmp444);}else{_LL287: _LL288:
# 2380
 return 0;}_LL284:;}default: _LL282: _LL283:
# 2382
 return 0;}_LL27B:;}
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
{struct _tuple28*_tmpB23;struct Cyc_List_List*_tmpB22;struct Cyc_List_List*decls=
(_tmpB22=_region_malloc(r,sizeof(*_tmpB22)),((_tmpB22->hd=((_tmpB23=_region_malloc(r,sizeof(*_tmpB23)),((_tmpB23->f1=max,((_tmpB23->f2=Cyc_Absyn_uint_typ,((_tmpB23->f3=e1,_tmpB23)))))))),((_tmpB22->tl=0,_tmpB22)))));
struct Cyc_Absyn_Exp*ai;
if(rgnopt == 0  || Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*_tmpB24[2];ai=Cyc_Toc_malloc_exp(old_elt_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpB24[1]=_tmp452,((_tmpB24[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB24,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));}else{
# 2487
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);{
struct Cyc_Absyn_Exp*_tmpB25[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpB25[1]=_tmp452,((_tmpB25[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB25,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{
# 2493
struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);
{struct _tuple28*_tmpB28;struct Cyc_List_List*_tmpB27;decls=((_tmpB27=_region_malloc(r,sizeof(*_tmpB27)),((_tmpB27->hd=((_tmpB28=_region_malloc(r,sizeof(*_tmpB28)),((_tmpB28->f1=a,((_tmpB28->f2=ptr_typ,((_tmpB28->f3=ainit,_tmpB28)))))))),((_tmpB27->tl=decls,_tmpB27))))));}
if(!Cyc_Tcutil_is_pointer_type(old_typ)){
const char*_tmpB2B;void*_tmpB2A;(_tmpB2A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB2B="new_comprehension_to_c: comprehension is not an array type",_tag_dyneither(_tmpB2B,sizeof(char),59))),_tag_dyneither(_tmpB2A,sizeof(void*),0)));}
if(Cyc_Tcutil_is_dyneither_ptr(old_typ)){
struct _tuple1*_tmp45A=Cyc_Toc_temp_var();
void*_tmp45B=Cyc_Toc_typ_to_c(old_typ);
struct Cyc_Absyn_Exp*_tmp45C=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpB2C[3];struct Cyc_Absyn_Exp*_tmp45D=
Cyc_Absyn_fncall_exp(_tmp45C,(
(_tmpB2C[2]=_tmp452,((_tmpB2C[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((_tmpB2C[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB2C,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 2505
{struct _tuple28*_tmpB2F;struct Cyc_List_List*_tmpB2E;decls=((_tmpB2E=_region_malloc(r,sizeof(*_tmpB2E)),((_tmpB2E->hd=((_tmpB2F=_region_malloc(r,sizeof(*_tmpB2F)),((_tmpB2F->f1=_tmp45A,((_tmpB2F->f2=_tmp45B,((_tmpB2F->f3=_tmp45D,_tmpB2F)))))))),((_tmpB2E->tl=decls,_tmpB2E))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp45A,0),0),0);}else{
# 2508
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}
{struct Cyc_List_List*_tmp461=decls;for(0;_tmp461 != 0;_tmp461=_tmp461->tl){
struct _tuple28 _tmp462=*((struct _tuple28*)_tmp461->hd);struct _tuple28 _tmp463=_tmp462;struct _tuple1*_tmp464;void*_tmp465;struct Cyc_Absyn_Exp*_tmp466;_LL28A: _tmp464=_tmp463.f1;_tmp465=_tmp463.f2;_tmp466=_tmp463.f3;_LL28B:;
s=Cyc_Absyn_declare_stmt(_tmp464,_tmp465,_tmp466,s,0);}}
# 2513
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2478
;_pop_region(r);};};}struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static void _tmpB7E(unsigned int*_tmpB7D,unsigned int*_tmpB7C,struct _tuple1***_tmpB7A){for(*_tmpB7D=0;*_tmpB7D < *_tmpB7C;(*_tmpB7D)++){(*_tmpB7A)[*_tmpB7D]=(struct _tuple1*)
# 2888 "toc.cyc"
Cyc_Toc_temp_var();}}
# 2517 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
void*_tmp469=e->r;
if(e->topt == 0){
const char*_tmpB33;void*_tmpB32[1];struct Cyc_String_pa_PrintArg_struct _tmpB31;(_tmpB31.tag=0,((_tmpB31.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpB32[0]=& _tmpB31,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB33="exp_to_c: no type for %s",_tag_dyneither(_tmpB33,sizeof(char),25))),_tag_dyneither(_tmpB32,sizeof(void*),1)))))));}
# 2523
if((nv->struct_info).lhs_exp != 0){
void*_tmp46D=_tmp469;if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp46D)->tag == 28){_LL28D: _LL28E:
 goto _LL28C;}else{_LL28F: _LL290:
 nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);}_LL28C:;}{
# 2531
void*old_typ=(void*)_check_null(e->topt);
void*_tmp46E=_tmp469;struct Cyc_Absyn_Stmt*_tmp46F;struct Cyc_Absyn_Exp*_tmp470;struct _dyneither_ptr*_tmp471;struct Cyc_Absyn_Exp*_tmp472;struct Cyc_Absyn_Exp*_tmp473;int _tmp474;struct Cyc_Absyn_Exp*_tmp475;void**_tmp476;struct Cyc_Absyn_Exp*_tmp477;int _tmp478;int _tmp479;struct Cyc_List_List*_tmp47A;struct Cyc_Absyn_Datatypedecl*_tmp47B;struct Cyc_Absyn_Datatypefield*_tmp47C;void*_tmp47D;struct Cyc_List_List*_tmp47E;struct _tuple1*_tmp47F;struct Cyc_List_List*_tmp480;struct Cyc_List_List*_tmp481;struct Cyc_Absyn_Aggrdecl*_tmp482;struct Cyc_Absyn_Exp*_tmp483;void*_tmp484;int _tmp485;struct Cyc_Absyn_Vardecl*_tmp486;struct Cyc_Absyn_Exp*_tmp487;struct Cyc_Absyn_Exp*_tmp488;int _tmp489;struct Cyc_List_List*_tmp48A;struct Cyc_List_List*_tmp48B;struct Cyc_Absyn_Exp*_tmp48C;struct Cyc_Absyn_Exp*_tmp48D;struct Cyc_Absyn_Exp*_tmp48E;struct _dyneither_ptr*_tmp48F;int _tmp490;int _tmp491;struct Cyc_Absyn_Exp*_tmp492;struct _dyneither_ptr*_tmp493;int _tmp494;int _tmp495;struct Cyc_Absyn_Exp*_tmp496;void*_tmp497;struct Cyc_List_List*_tmp498;void*_tmp499;struct Cyc_Absyn_Exp*_tmp49A;struct Cyc_Absyn_Exp*_tmp49B;struct Cyc_Absyn_Exp*_tmp49C;struct Cyc_Absyn_Exp*_tmp49D;void**_tmp49E;struct Cyc_Absyn_Exp*_tmp49F;int _tmp4A0;enum Cyc_Absyn_Coercion _tmp4A1;struct Cyc_Absyn_Exp*_tmp4A2;struct Cyc_List_List*_tmp4A3;struct Cyc_Absyn_Exp*_tmp4A4;struct Cyc_Absyn_Exp*_tmp4A5;int _tmp4A6;struct Cyc_Absyn_Exp*_tmp4A7;struct Cyc_List_List*_tmp4A8;int _tmp4A9;struct Cyc_List_List*_tmp4AA;struct Cyc_Absyn_VarargInfo*_tmp4AB;struct Cyc_Absyn_Exp*_tmp4AC;struct Cyc_List_List*_tmp4AD;struct Cyc_Absyn_Exp*_tmp4AE;struct Cyc_Absyn_Exp*_tmp4AF;struct Cyc_Absyn_Exp*_tmp4B0;struct Cyc_Absyn_Exp*_tmp4B1;struct Cyc_Absyn_Exp*_tmp4B2;struct Cyc_Absyn_Exp*_tmp4B3;struct Cyc_Absyn_Exp*_tmp4B4;struct Cyc_Absyn_Exp*_tmp4B5;struct Cyc_Absyn_Exp*_tmp4B6;struct Cyc_Absyn_Exp*_tmp4B7;struct Cyc_Core_Opt*_tmp4B8;struct Cyc_Absyn_Exp*_tmp4B9;struct Cyc_Absyn_Exp*_tmp4BA;enum Cyc_Absyn_Incrementor _tmp4BB;enum Cyc_Absyn_Primop _tmp4BC;struct Cyc_List_List*_tmp4BD;struct _tuple1*_tmp4BE;void*_tmp4BF;switch(*((int*)_tmp46E)){case 0: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp46E)->f1).Null_c).tag == 1){_LL292: _LL293: {
# 2538
struct Cyc_Absyn_Exp*_tmp4C0=Cyc_Absyn_uint_exp(0,0);
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp4C0,_tmp4C0))->r;else{
# 2543
struct Cyc_Absyn_Exp*_tmpB34[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((_tmpB34[2]=_tmp4C0,((_tmpB34[1]=_tmp4C0,((_tmpB34[0]=_tmp4C0,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB34,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}else{
# 2545
e->r=(void*)& Cyc_Toc_zero_exp;}
# 2547
goto _LL291;}}else{_LL294: _LL295:
 goto _LL291;}case 1: _LL296: _tmp4BE=((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp4BF=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL297:
# 2550
{struct _handler_cons _tmp4C2;_push_handler(& _tmp4C2);{int _tmp4C4=0;if(setjmp(_tmp4C2.handler))_tmp4C4=1;if(!_tmp4C4){e->r=(Cyc_Toc_lookup_varmap(nv,_tmp4BE))->r;;_pop_handler();}else{void*_tmp4C3=(void*)_exn_thrown;void*_tmp4C5=_tmp4C3;void*_tmp4C6;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp4C5)->tag == Cyc_Dict_Absent){_LL2E7: _LL2E8: {
# 2552
const char*_tmpB38;void*_tmpB37[1];struct Cyc_String_pa_PrintArg_struct _tmpB36;(_tmpB36.tag=0,((_tmpB36.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4BE)),((_tmpB37[0]=& _tmpB36,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB38="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpB38,sizeof(char),32))),_tag_dyneither(_tmpB37,sizeof(void*),1)))))));}}else{_LL2E9: _tmp4C6=_tmp4C5;_LL2EA:(int)_rethrow(_tmp4C6);}_LL2E6:;}};}
# 2554
goto _LL291;case 2: _LL298: _tmp4BC=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp4BD=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL299: {
# 2557
struct Cyc_List_List*_tmp4CA=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp4BD);
# 2559
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4BD);
{enum Cyc_Absyn_Primop _tmp4CB=_tmp4BC;switch(_tmp4CB){case Cyc_Absyn_Numelts: _LL2EC: _LL2ED: {
# 2562
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4BD))->hd;
{void*_tmp4CC=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_tmp4CD=_tmp4CC;void*_tmp4CE;union Cyc_Absyn_Constraint*_tmp4CF;union Cyc_Absyn_Constraint*_tmp4D0;union Cyc_Absyn_Constraint*_tmp4D1;struct Cyc_Absyn_Exp*_tmp4D2;switch(*((int*)_tmp4CD)){case 8: _LL301: _tmp4D2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4CD)->f1).num_elts;_LL302:
# 2566
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp4D2))){
const char*_tmpB3B;void*_tmpB3A;(_tmpB3A=0,Cyc_Tcutil_terr(e->loc,((_tmpB3B="can't calculate numelts",_tag_dyneither(_tmpB3B,sizeof(char),24))),_tag_dyneither(_tmpB3A,sizeof(void*),0)));}
e->r=_tmp4D2->r;goto _LL300;case 5: _LL303: _tmp4CE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CD)->f1).elt_typ;_tmp4CF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CD)->f1).ptr_atts).nullable;_tmp4D0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CD)->f1).ptr_atts).bounds;_tmp4D1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CD)->f1).ptr_atts).zero_term;_LL304:
# 2570
{void*_tmp4D5=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp4D0);void*_tmp4D6=_tmp4D5;struct Cyc_Absyn_Exp*_tmp4D7;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp4D6)->tag == 0){_LL308: _LL309:
# 2572
{struct Cyc_Absyn_Exp*_tmpB3C[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,(
(_tmpB3C[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp4CE),0),((_tmpB3C[0]=(struct Cyc_Absyn_Exp*)_tmp4BD->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB3C,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL307;}else{_LL30A: _tmp4D7=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4D6)->f1;_LL30B:
# 2578
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4D1)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp4BD->hd);
# 2582
struct Cyc_Absyn_Exp*_tmpB3D[2];e->r=Cyc_Toc_fncall_exp_r(function_e,((_tmpB3D[1]=_tmp4D7,((_tmpB3D[0]=(struct Cyc_Absyn_Exp*)_tmp4BD->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB3D,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4CF)){
if(!Cyc_Evexp_c_can_eval(_tmp4D7)){
const char*_tmpB40;void*_tmpB3F;(_tmpB3F=0,Cyc_Tcutil_terr(e->loc,((_tmpB40="can't calculate numelts",_tag_dyneither(_tmpB40,sizeof(char),24))),_tag_dyneither(_tmpB3F,sizeof(void*),0)));}
# 2587
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp4D7,Cyc_Absyn_uint_exp(0,0));}else{
# 2589
e->r=_tmp4D7->r;goto _LL307;}}
# 2591
goto _LL307;}_LL307:;}
# 2593
goto _LL300;default: _LL305: _LL306: {
# 2595
const char*_tmpB45;void*_tmpB44[2];struct Cyc_String_pa_PrintArg_struct _tmpB43;struct Cyc_String_pa_PrintArg_struct _tmpB42;(_tmpB42.tag=0,((_tmpB42.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpB43.tag=0,((_tmpB43.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpB44[0]=& _tmpB43,((_tmpB44[1]=& _tmpB42,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB45="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpB45,sizeof(char),41))),_tag_dyneither(_tmpB44,sizeof(void*),2)))))))))))));}}_LL300:;}
# 2598
goto _LL2EB;}case Cyc_Absyn_Plus: _LL2EE: _LL2EF:
# 2603
{void*_tmp4E0=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp4CA))->hd);void*_tmp4E1=_tmp4E0;void*_tmp4E2;union Cyc_Absyn_Constraint*_tmp4E3;union Cyc_Absyn_Constraint*_tmp4E4;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E1)->tag == 5){_LL30D: _tmp4E2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E1)->f1).elt_typ;_tmp4E3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E1)->f1).ptr_atts).bounds;_tmp4E4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E1)->f1).ptr_atts).zero_term;_LL30E:
# 2605
{void*_tmp4E5=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp4E3);void*_tmp4E6=_tmp4E5;struct Cyc_Absyn_Exp*_tmp4E7;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp4E6)->tag == 0){_LL312: _LL313: {
# 2607
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4BD))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4BD->tl))->hd;
{struct Cyc_Absyn_Exp*_tmpB46[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpB46[2]=e2,((_tmpB46[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp4E2),0),((_tmpB46[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB46,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}
goto _LL311;}}else{_LL314: _tmp4E7=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4E6)->f1;_LL315:
# 2613
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4E4)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4BD))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4BD->tl))->hd;
struct Cyc_Absyn_Exp*_tmpB47[3];e->r=(Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1),((_tmpB47[2]=e2,((_tmpB47[1]=_tmp4E7,((_tmpB47[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB47,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0))->r;}
# 2618
goto _LL311;}_LL311:;}
# 2620
goto _LL30C;}else{_LL30F: _LL310:
# 2622
 goto _LL30C;}_LL30C:;}
# 2624
goto _LL2EB;case Cyc_Absyn_Minus: _LL2F0: _LL2F1: {
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
struct Cyc_Absyn_Exp*_tmpB48[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpB48[2]=
Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0),((_tmpB48[1]=
# 2640
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpB48[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB48,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 2644
goto _LL2EB;}case Cyc_Absyn_Eq: _LL2F2: _LL2F3:
 goto _LL2F5;case Cyc_Absyn_Neq: _LL2F4: _LL2F5:
 goto _LL2F7;case Cyc_Absyn_Gt: _LL2F6: _LL2F7:
 goto _LL2F9;case Cyc_Absyn_Gte: _LL2F8: _LL2F9:
 goto _LL2FB;case Cyc_Absyn_Lt: _LL2FA: _LL2FB:
 goto _LL2FD;case Cyc_Absyn_Lte: _LL2FC: _LL2FD: {
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
goto _LL2EB;}default: _LL2FE: _LL2FF:
 goto _LL2EB;}_LL2EB:;}
# 2664
goto _LL291;}case 4: _LL29A: _tmp4BA=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp4BB=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL29B: {
# 2666
void*e2_cyc_typ=(void*)_check_null(_tmp4BA->topt);
# 2675 "toc.cyc"
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
const char*_tmpB49;struct _dyneither_ptr incr_str=(_tmpB49="increment",_tag_dyneither(_tmpB49,sizeof(char),10));
if(_tmp4BB == Cyc_Absyn_PreDec  || _tmp4BB == Cyc_Absyn_PostDec){const char*_tmpB4A;incr_str=((_tmpB4A="decrement",_tag_dyneither(_tmpB4A,sizeof(char),10)));}
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4BA,& ptr_type,& is_dyneither,& elt_type)){
{const char*_tmpB4E;void*_tmpB4D[1];struct Cyc_String_pa_PrintArg_struct _tmpB4C;(_tmpB4C.tag=0,((_tmpB4C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpB4D[0]=& _tmpB4C,Cyc_Tcutil_terr(e->loc,((_tmpB4E="in-place %s is not supported when dereferencing a zero-terminated pointer",_tag_dyneither(_tmpB4E,sizeof(char),74))),_tag_dyneither(_tmpB4D,sizeof(void*),1)))))));}{
const char*_tmpB51;void*_tmpB50;(_tmpB50=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB51="in-place inc/dec on zero-term",_tag_dyneither(_tmpB51,sizeof(char),30))),_tag_dyneither(_tmpB50,sizeof(void*),0)));};}{
# 2684
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
int f_tag=0;
if(Cyc_Toc_is_tagged_union_project(_tmp4BA,& f_tag,& tunion_member_type,1)){
struct Cyc_Absyn_Exp*_tmp4F1=Cyc_Absyn_signed_int_exp(1,0);
_tmp4F1->topt=Cyc_Absyn_sint_typ;{
enum Cyc_Absyn_Incrementor _tmp4F2=_tmp4BB;switch(_tmp4F2){case Cyc_Absyn_PreInc: _LL317: _LL318:
# 2691
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpB57;struct Cyc_Core_Opt*_tmpB56;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpB55;e->r=(void*)((_tmpB55=_cycalloc(sizeof(*_tmpB55)),((_tmpB55[0]=((_tmpB57.tag=3,((_tmpB57.f1=_tmp4BA,((_tmpB57.f2=((_tmpB56=_cycalloc_atomic(sizeof(*_tmpB56)),((_tmpB56->v=(void*)Cyc_Absyn_Plus,_tmpB56)))),((_tmpB57.f3=_tmp4F1,_tmpB57)))))))),_tmpB55))));}
Cyc_Toc_exp_to_c(nv,e);
return;case Cyc_Absyn_PreDec: _LL319: _LL31A:
# 2695
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpB5D;struct Cyc_Core_Opt*_tmpB5C;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpB5B;e->r=(void*)((_tmpB5B=_cycalloc(sizeof(*_tmpB5B)),((_tmpB5B[0]=((_tmpB5D.tag=3,((_tmpB5D.f1=_tmp4BA,((_tmpB5D.f2=((_tmpB5C=_cycalloc_atomic(sizeof(*_tmpB5C)),((_tmpB5C->v=(void*)Cyc_Absyn_Minus,_tmpB5C)))),((_tmpB5D.f3=_tmp4F1,_tmpB5D)))))))),_tmpB5B))));}
Cyc_Toc_exp_to_c(nv,e);
return;default: _LL31B: _LL31C:
# 2699
{const char*_tmpB61;void*_tmpB60[1];struct Cyc_String_pa_PrintArg_struct _tmpB5F;(_tmpB5F.tag=0,((_tmpB5F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpB60[0]=& _tmpB5F,Cyc_Tcutil_terr(e->loc,((_tmpB61="in-place post-%s is not supported on @tagged union members",_tag_dyneither(_tmpB61,sizeof(char),59))),_tag_dyneither(_tmpB60,sizeof(void*),1)))))));}{
# 2701
const char*_tmpB64;void*_tmpB63;(_tmpB63=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB64="in-place inc/dec on @tagged union",_tag_dyneither(_tmpB64,sizeof(char),34))),_tag_dyneither(_tmpB63,sizeof(void*),0)));};}_LL316:;};}
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
struct Cyc_Absyn_Exp*_tmpB65[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpB65[2]=
# 2719
Cyc_Absyn_signed_int_exp(change,0),((_tmpB65[1]=
# 2718
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpB65[0]=
# 2717
Cyc_Toc_push_address_exp(_tmp4BA),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB65,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2720
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2724
struct Cyc_Toc_functionSet*_tmp4FF=_tmp4BB != Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
# 2726
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp4FF,_tmp4BA);
struct Cyc_Absyn_Exp*_tmpB66[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpB66[1]=
Cyc_Absyn_signed_int_exp(1,0),((_tmpB66[0]=
# 2727
Cyc_Toc_push_address_exp(_tmp4BA),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB66,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
# 2729
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp4BA)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp4BA,0,Cyc_Toc_incr_lvalue,_tmp4BB);
e->r=_tmp4BA->r;}}}
# 2733
goto _LL291;};};}case 3: _LL29C: _tmp4B7=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp4B8=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_tmp4B9=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp46E)->f3;_LL29D: {
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
{enum Cyc_Absyn_Primop _tmp502=(enum Cyc_Absyn_Primop)_tmp4B8->v;enum Cyc_Absyn_Primop _tmp503=_tmp502;switch(_tmp503){case Cyc_Absyn_Plus: _LL31E: _LL31F:
 change=_tmp4B9;goto _LL31D;case Cyc_Absyn_Minus: _LL320: _LL321:
# 2789
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp4B9,0);goto _LL31D;default: _LL322: _LL323: {
const char*_tmpB69;void*_tmpB68;(_tmpB68=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB69="bad t ? pointer arithmetic",_tag_dyneither(_tmpB69,sizeof(char),27))),_tag_dyneither(_tmpB68,sizeof(void*),0)));}}_LL31D:;}
# 2792
done=1;{
# 2794
struct Cyc_Absyn_Exp*_tmp506=Cyc_Toc__dyneither_ptr_inplace_plus_e;
struct Cyc_Absyn_Exp*_tmpB6A[3];e->r=Cyc_Toc_fncall_exp_r(_tmp506,((_tmpB6A[2]=change,((_tmpB6A[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpB6A[0]=
# 2795
Cyc_Toc_push_address_exp(_tmp4B7),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB6A,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2798
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2801
enum Cyc_Absyn_Primop _tmp508=(enum Cyc_Absyn_Primop)_tmp4B8->v;enum Cyc_Absyn_Primop _tmp509=_tmp508;if(_tmp509 == Cyc_Absyn_Plus){_LL325: _LL326:
# 2803
 done=1;
{struct Cyc_Absyn_Exp*_tmpB6B[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp4B7),((_tmpB6B[1]=_tmp4B9,((_tmpB6B[0]=_tmp4B7,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB6B,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL324;}else{_LL327: _LL328: {
const char*_tmpB6E;void*_tmpB6D;(_tmpB6D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB6E="bad zero-terminated pointer arithmetic",_tag_dyneither(_tmpB6E,sizeof(char),39))),_tag_dyneither(_tmpB6D,sizeof(void*),0)));}}_LL324:;}}}
# 2810
if(!done){
# 2812
if(e1_poly)
_tmp4B9->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp4B9->r,0));
# 2818
if(!Cyc_Absyn_is_lvalue(_tmp4B7)){
{struct _tuple27 _tmpB71;struct _tuple27*_tmpB70;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple27*),struct _tuple27*f_env))Cyc_Toc_lvalue_assign)(_tmp4B7,0,Cyc_Toc_assignop_lvalue,((_tmpB70=_cycalloc(sizeof(struct _tuple27)* 1),((_tmpB70[0]=(struct _tuple27)((_tmpB71.f1=_tmp4B8,((_tmpB71.f2=_tmp4B9,_tmpB71)))),_tmpB70)))));}
e->r=_tmp4B7->r;}}
# 2823
goto _LL291;};};};}case 5: _LL29E: _tmp4B4=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp4B5=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_tmp4B6=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp46E)->f3;_LL29F:
# 2825
 Cyc_Toc_exp_to_c(nv,_tmp4B4);
Cyc_Toc_exp_to_c(nv,_tmp4B5);
Cyc_Toc_exp_to_c(nv,_tmp4B6);
goto _LL291;case 6: _LL2A0: _tmp4B2=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp4B3=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL2A1:
# 2830
 Cyc_Toc_exp_to_c(nv,_tmp4B2);
Cyc_Toc_exp_to_c(nv,_tmp4B3);
goto _LL291;case 7: _LL2A2: _tmp4B0=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp4B1=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL2A3:
# 2834
 Cyc_Toc_exp_to_c(nv,_tmp4B0);
Cyc_Toc_exp_to_c(nv,_tmp4B1);
goto _LL291;case 8: _LL2A4: _tmp4AE=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp4AF=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL2A5:
# 2838
 Cyc_Toc_exp_to_c(nv,_tmp4AE);
Cyc_Toc_exp_to_c(nv,_tmp4AF);
goto _LL291;case 9: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46E)->f3 == 0){_LL2A6: _tmp4AC=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp4AD=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL2A7:
# 2842
 Cyc_Toc_exp_to_c(nv,_tmp4AC);
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4AD);
goto _LL291;}else{_LL2A8: _tmp4A7=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp4A8=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_tmp4A9=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46E)->f3)->num_varargs;_tmp4AA=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46E)->f3)->injectors;_tmp4AB=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46E)->f3)->vai;_LL2A9: {
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
struct Cyc_List_List*_tmpB72;new_args=((_tmpB72=_cycalloc(sizeof(*_tmpB72)),((_tmpB72->hd=(struct Cyc_Absyn_Exp*)_tmp4A8->hd,((_tmpB72->tl=new_args,_tmpB72))))));};}}
# 2871
{struct Cyc_Absyn_Exp*_tmpB75[3];struct Cyc_List_List*_tmpB74;new_args=((_tmpB74=_cycalloc(sizeof(*_tmpB74)),((_tmpB74->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpB75[2]=num_varargs_exp,((_tmpB75[1]=
Cyc_Absyn_sizeoftyp_exp(cva_type,0),((_tmpB75[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB75,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpB74->tl=new_args,_tmpB74))))));}
# 2876
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);
# 2878
Cyc_Toc_exp_to_c(nv,_tmp4A7);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4A7,new_args,0),0);
# 2882
if(_tmp4AB->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp513=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp4AB->type));void*_tmp514=_tmp513;struct Cyc_Absyn_Datatypedecl*_tmp515;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp514)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp514)->f1).datatype_info).KnownDatatype).tag == 2){_LL32A: _tmp515=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp514)->f1).datatype_info).KnownDatatype).val;_LL32B:
 tud=_tmp515;goto _LL329;}else{goto _LL32C;}}else{_LL32C: _LL32D: {
const char*_tmpB78;void*_tmpB77;(_tmpB77=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB78="toc: unknown datatype in vararg with inject",_tag_dyneither(_tmpB78,sizeof(char),44))),_tag_dyneither(_tmpB77,sizeof(void*),0)));}}_LL329:;}{
# 2888
unsigned int _tmpB7D;unsigned int _tmpB7C;struct _dyneither_ptr _tmpB7B;struct _tuple1**_tmpB7A;unsigned int _tmpB79;struct _dyneither_ptr vs=(_tmpB79=(unsigned int)_tmp4A9,((_tmpB7A=(struct _tuple1**)_region_malloc(r,_check_times(sizeof(struct _tuple1*),_tmpB79)),((_tmpB7B=_tag_dyneither(_tmpB7A,sizeof(struct _tuple1*),_tmpB79),((((_tmpB7C=_tmpB79,_tmpB7E(& _tmpB7D,& _tmpB7C,& _tmpB7A))),_tmpB7B)))))));
# 2890
if(_tmp4A9 != 0){
# 2892
struct Cyc_List_List*_tmp518=0;
{int i=_tmp4A9 - 1;for(0;i >= 0;-- i){
struct Cyc_List_List*_tmpB7F;_tmp518=((_tmpB7F=_cycalloc(sizeof(*_tmpB7F)),((_tmpB7F->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0),0)),((_tmpB7F->tl=_tmp518,_tmpB7F))))));}}
# 2896
s=Cyc_Absyn_declare_stmt(argv,arr_type,Cyc_Absyn_unresolvedmem_exp(0,_tmp518,0),s,0);{
# 2899
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
# 2914
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);
# 2917
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Toc_tag_sp,0),
Cyc_Toc_datatype_tag(tud,_tmp51C),0),s,0);
# 2920
s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp51C,tud->name)),0,s,0);};}};}else{
# 2927
struct _tuple19*_tmpB80[3];struct Cyc_List_List*_tmp51E=(_tmpB80[2]=
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpB80[1]=
# 2927
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpB80[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB80,sizeof(struct _tuple19*),3)))))));
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
_npop_handler(0);goto _LL291;
# 2854 "toc.cyc"
;_pop_region(r);}}case 10: _LL2AA: _tmp4A5=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp4A6=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL2AB:
# 2963 "toc.cyc"
 Cyc_Toc_exp_to_c(nv,_tmp4A5);{
struct Cyc_Absyn_Exp*_tmp524=_tmp4A6?Cyc_Toc_newrethrow_exp(_tmp4A5): Cyc_Toc_newthrow_exp(_tmp4A5);
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),_tmp524,0))->r;
goto _LL291;};case 11: _LL2AC: _tmp4A4=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_LL2AD:
 Cyc_Toc_exp_to_c(nv,_tmp4A4);goto _LL291;case 12: _LL2AE: _tmp4A2=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp4A3=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL2AF:
# 2969
 Cyc_Toc_exp_to_c(nv,_tmp4A2);
# 2978 "toc.cyc"
for(0;_tmp4A3 != 0;_tmp4A3=_tmp4A3->tl){
enum Cyc_Absyn_KindQual _tmp525=(Cyc_Tcutil_typ_kind((void*)_tmp4A3->hd))->kind;
if(_tmp525 != Cyc_Absyn_EffKind  && _tmp525 != Cyc_Absyn_RgnKind){
{void*_tmp526=Cyc_Tcutil_compress((void*)_tmp4A3->hd);void*_tmp527=_tmp526;switch(*((int*)_tmp527)){case 2: _LL332: _LL333:
 goto _LL335;case 3: _LL334: _LL335:
 continue;default: _LL336: _LL337:
# 2985
 e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp4A2,0))->r;
goto _LL331;}_LL331:;}
# 2988
break;}}
# 2991
goto _LL291;case 13: _LL2B0: _tmp49E=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp49F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_tmp4A0=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp46E)->f3;_tmp4A1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp46E)->f4;_LL2B1: {
# 2993
void*old_t2=(void*)_check_null(_tmp49F->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp49E;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp49E=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp49F);
# 3000
{struct _tuple29 _tmpB81;struct _tuple29 _tmp528=(_tmpB81.f1=Cyc_Tcutil_compress(old_t2),((_tmpB81.f2=Cyc_Tcutil_compress(new_typ),_tmpB81)));struct _tuple29 _tmp529=_tmp528;struct Cyc_Absyn_PtrInfo _tmp52A;struct Cyc_Absyn_PtrInfo _tmp52B;struct Cyc_Absyn_PtrInfo _tmp52C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp529.f1)->tag == 5)switch(*((int*)_tmp529.f2)){case 5: _LL339: _tmp52B=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp529.f1)->f1;_tmp52C=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp529.f2)->f1;_LL33A: {
# 3002
int _tmp52D=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp52B.ptr_atts).nullable);
int _tmp52E=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp52C.ptr_atts).nullable);
void*_tmp52F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp52B.ptr_atts).bounds);
void*_tmp530=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp52C.ptr_atts).bounds);
int _tmp531=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp52B.ptr_atts).zero_term);
int _tmp532=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp52C.ptr_atts).zero_term);
{struct _tuple29 _tmpB82;struct _tuple29 _tmp533=(_tmpB82.f1=_tmp52F,((_tmpB82.f2=_tmp530,_tmpB82)));struct _tuple29 _tmp534=_tmp533;struct Cyc_Absyn_Exp*_tmp535;struct Cyc_Absyn_Exp*_tmp536;struct Cyc_Absyn_Exp*_tmp537;struct Cyc_Absyn_Exp*_tmp538;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp534.f1)->tag == 1){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp534.f2)->tag == 1){_LL340: _tmp537=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp534.f1)->f1;_tmp538=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp534.f2)->f1;_LL341:
# 3010
 if((!Cyc_Evexp_c_can_eval(_tmp537) || !Cyc_Evexp_c_can_eval(_tmp538)) && !
Cyc_Evexp_same_const_exp(_tmp537,_tmp538)){
const char*_tmpB85;void*_tmpB84;(_tmpB84=0,Cyc_Tcutil_terr(e->loc,((_tmpB85="can't validate cast due to potential size differences",_tag_dyneither(_tmpB85,sizeof(char),54))),_tag_dyneither(_tmpB84,sizeof(void*),0)));}
if(_tmp52D  && !_tmp52E){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpB88;void*_tmpB87;(_tmpB87=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB88="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpB88,sizeof(char),44))),_tag_dyneither(_tmpB87,sizeof(void*),0)));}
# 3019
if(_tmp4A1 != Cyc_Absyn_NonNull_to_Null){
const char*_tmpB8C;void*_tmpB8B[1];struct Cyc_String_pa_PrintArg_struct _tmpB8A;(_tmpB8A.tag=0,((_tmpB8A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpB8B[0]=& _tmpB8A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpB8C="null-check conversion mis-classified: %s",_tag_dyneither(_tmpB8C,sizeof(char),41))),_tag_dyneither(_tmpB8B,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp49F);
if(do_null_check){
if(!_tmp4A0){
const char*_tmpB8F;void*_tmpB8E;(_tmpB8E=0,Cyc_Tcutil_warn(e->loc,((_tmpB8F="inserted null check due to implicit cast from * to @ type",_tag_dyneither(_tmpB8F,sizeof(char),58))),_tag_dyneither(_tmpB8E,sizeof(void*),0)));}{
# 3026
struct Cyc_List_List*_tmpB90;e->r=Cyc_Toc_cast_it_r(*_tmp49E,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpB90=_cycalloc(sizeof(*_tmpB90)),((_tmpB90->hd=_tmp49F,((_tmpB90->tl=0,_tmpB90)))))),0));};}else{
# 3030
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp49F->r;}};}else{
# 3035
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp49F->r;}
# 3042
goto _LL33F;}else{_LL342: _tmp536=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp534.f1)->f1;_LL343:
# 3044
 if(!Cyc_Evexp_c_can_eval(_tmp536)){
const char*_tmpB93;void*_tmpB92;(_tmpB92=0,Cyc_Tcutil_terr(e->loc,((_tmpB93="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpB93,sizeof(char),71))),_tag_dyneither(_tmpB92,sizeof(void*),0)));}
# 3047
if(_tmp4A1 == Cyc_Absyn_NonNull_to_Null){
const char*_tmpB97;void*_tmpB96[1];struct Cyc_String_pa_PrintArg_struct _tmpB95;(_tmpB95.tag=0,((_tmpB95.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpB96[0]=& _tmpB95,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpB97="conversion mis-classified as null-check: %s",_tag_dyneither(_tmpB97,sizeof(char),44))),_tag_dyneither(_tmpB96,sizeof(void*),1)))))));}
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
{void*_tmp54B=_tmp49F->r;void*_tmp54C=_tmp54B;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp54C)->tag == 0)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp54C)->f1).Wstring_c).tag){case 8: _LL349: _LL34A:
# 3072
 arg3=_tmp536;goto _LL348;case 9: _LL34B: _LL34C:
# 3074
 arg3=_tmp536;goto _LL348;default: goto _LL34D;}else{_LL34D: _LL34E:
# 3076
{struct Cyc_Absyn_Exp*_tmpB98[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp49F),(
# 3078
(_tmpB98[1]=_tmp536,((_tmpB98[0]=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp54A),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB98,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
goto _LL348;}_LL348:;}
# 3081
if(!_tmp532  && !(_tmp52C.elt_tq).real_const)
# 3084
arg3=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{
# 3086
struct Cyc_Absyn_Exp*_tmp54E=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp52C.elt_typ),0);
struct Cyc_Absyn_Exp*_tmpB99[3];struct Cyc_Absyn_Exp*_tmp54F=Cyc_Absyn_fncall_exp(_tmp548,((_tmpB99[2]=arg3,((_tmpB99[1]=_tmp54E,((_tmpB99[0]=_tmp54A,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB99,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
struct Cyc_Absyn_Stmt*_tmp550=Cyc_Absyn_exp_stmt(_tmp54F,0);
_tmp550=Cyc_Absyn_declare_stmt(_tmp549,Cyc_Toc_typ_to_c(old_t2),_tmp49F,_tmp550,0);
e->r=Cyc_Toc_stmt_exp_r(_tmp550);};}else{
# 3093
struct Cyc_Absyn_Exp*_tmpB9A[3];e->r=Cyc_Toc_fncall_exp_r(_tmp548,(
(_tmpB9A[2]=_tmp536,((_tmpB9A[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp52C.elt_typ),0),((_tmpB9A[0]=_tmp49F,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB9A,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 3099
goto _LL33F;}}else{if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp534.f2)->tag == 1){_LL344: _tmp535=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp534.f2)->f1;_LL345:
# 3101
 if(!Cyc_Evexp_c_can_eval(_tmp535)){
const char*_tmpB9D;void*_tmpB9C;(_tmpB9C=0,Cyc_Tcutil_terr(e->loc,((_tmpB9D="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpB9D,sizeof(char),71))),_tag_dyneither(_tmpB9C,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpBA0;void*_tmpB9F;(_tmpB9F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBA0="can't coerce t? to t* or t@ at the top-level",_tag_dyneither(_tmpBA0,sizeof(char),45))),_tag_dyneither(_tmpB9F,sizeof(void*),0)));}{
# 3113 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp557=_tmp535;
if(_tmp531  && !_tmp532)
_tmp557=Cyc_Absyn_add_exp(_tmp535,Cyc_Absyn_uint_exp(1,0),0);{
# 3120
struct Cyc_Absyn_Exp*_tmp558=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpBA1[3];struct Cyc_Absyn_Exp*_tmp559=Cyc_Absyn_fncall_exp(_tmp558,(
(_tmpBA1[2]=_tmp557,((_tmpBA1[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp52B.elt_typ),0),((_tmpBA1[0]=_tmp49F,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBA1,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 3125
if(_tmp52E){
struct Cyc_List_List*_tmpBA2;_tmp559->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,(
(_tmpBA2=_cycalloc(sizeof(*_tmpBA2)),((_tmpBA2->hd=Cyc_Absyn_copy_exp(_tmp559),((_tmpBA2->tl=0,_tmpBA2)))))));}
e->r=Cyc_Toc_cast_it_r(*_tmp49E,_tmp559);
goto _LL33F;};};}else{_LL346: _LL347:
# 3133
 DynCast:
 if((_tmp531  && !_tmp532) && !(_tmp52C.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpBA5;void*_tmpBA4;(_tmpBA4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBA5="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",_tag_dyneither(_tmpBA5,sizeof(char),70))),_tag_dyneither(_tmpBA4,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp55E=Cyc_Toc__dyneither_ptr_decrease_size_e;
struct Cyc_Absyn_Exp*_tmpBA6[3];e->r=Cyc_Toc_fncall_exp_r(_tmp55E,(
(_tmpBA6[2]=
Cyc_Absyn_uint_exp(1,0),((_tmpBA6[1]=
# 3139
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp52B.elt_typ),0),((_tmpBA6[0]=_tmp49F,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBA6,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}
# 3142
goto _LL33F;}}_LL33F:;}
# 3144
goto _LL338;}case 6: _LL33B: _tmp52A=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp529.f1)->f1;_LL33C:
# 3146
{void*_tmp561=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp52A.ptr_atts).bounds);void*_tmp562=_tmp561;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp562)->tag == 0){_LL350: _LL351:
# 3148
 _tmp49F->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp49F->r,_tmp49F->loc),Cyc_Toc_curr_sp);goto _LL34F;}else{_LL352: _LL353:
 goto _LL34F;}_LL34F:;}
# 3151
goto _LL338;default: goto _LL33D;}else{_LL33D: _LL33E:
# 3153
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp49F->r;
goto _LL338;}_LL338:;}
# 3157
goto _LL291;}case 14: _LL2B2: _tmp49D=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_LL2B3:
# 3161
{void*_tmp564=_tmp49D->r;void*_tmp565=_tmp564;struct Cyc_List_List*_tmp566;struct _tuple1*_tmp567;struct Cyc_List_List*_tmp568;struct Cyc_List_List*_tmp569;switch(*((int*)_tmp565)){case 28: _LL355: _tmp567=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp565)->f1;_tmp568=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp565)->f2;_tmp569=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp565)->f3;_LL356:
# 3163
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpBAA;void*_tmpBA9[1];struct Cyc_String_pa_PrintArg_struct _tmpBA8;(_tmpBA8.tag=0,((_tmpBA8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp49D->loc)),((_tmpBA9[0]=& _tmpBA8,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBAA="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpBAA,sizeof(char),42))),_tag_dyneither(_tmpBA9,sizeof(void*),1)))))));}{
struct Cyc_Absyn_Exp*_tmp56D=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp49D->topt),_tmp568,1,0,_tmp569,_tmp567);
e->r=_tmp56D->r;
e->topt=_tmp56D->topt;
goto _LL354;};case 23: _LL357: _tmp566=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp565)->f1;_LL358:
# 3171
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpBAE;void*_tmpBAD[1];struct Cyc_String_pa_PrintArg_struct _tmpBAC;(_tmpBAC.tag=0,((_tmpBAC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp49D->loc)),((_tmpBAD[0]=& _tmpBAC,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBAE="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpBAE,sizeof(char),42))),_tag_dyneither(_tmpBAD,sizeof(void*),1)))))));}
e->r=(Cyc_Toc_init_tuple(nv,1,0,_tmp566))->r;
goto _LL354;default: _LL359: _LL35A:
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
goto _LL354;}_LL354:;}
# 3191
goto _LL291;case 15: _LL2B4: _tmp49B=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp49C=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL2B5:
# 3194
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpBB2;void*_tmpBB1[1];struct Cyc_String_pa_PrintArg_struct _tmpBB0;(_tmpBB0.tag=0,((_tmpBB0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp49C->loc)),((_tmpBB1[0]=& _tmpBB0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBB2="%s: new at top-level",_tag_dyneither(_tmpBB2,sizeof(char),21))),_tag_dyneither(_tmpBB1,sizeof(void*),1)))))));}{
void*new_e_type=(void*)_check_null(_tmp49C->topt);
{void*_tmp574=_tmp49C->r;void*_tmp575=_tmp574;struct Cyc_List_List*_tmp576;struct _tuple1*_tmp577;struct Cyc_List_List*_tmp578;struct Cyc_List_List*_tmp579;struct Cyc_Absyn_Aggrdecl*_tmp57A;struct Cyc_Absyn_Exp*_tmp57B;void*_tmp57C;int _tmp57D;struct Cyc_Absyn_Vardecl*_tmp57E;struct Cyc_Absyn_Exp*_tmp57F;struct Cyc_Absyn_Exp*_tmp580;int _tmp581;struct Cyc_List_List*_tmp582;switch(*((int*)_tmp575)){case 25: _LL35C: _tmp582=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp575)->f1;_LL35D: {
# 3202
struct _tuple1*_tmp583=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp584=Cyc_Absyn_var_exp(_tmp583,0);
struct Cyc_Absyn_Stmt*_tmp585=Cyc_Toc_init_array(nv,new_e_type,_tmp584,_tmp582,Cyc_Absyn_exp_stmt(_tmp584,0));
void*old_elt_typ;
{void*_tmp586=Cyc_Tcutil_compress(old_typ);void*_tmp587=_tmp586;void*_tmp588;struct Cyc_Absyn_Tqual _tmp589;union Cyc_Absyn_Constraint*_tmp58A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp587)->tag == 5){_LL369: _tmp588=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp587)->f1).elt_typ;_tmp589=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp587)->f1).elt_tq;_tmp58A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp587)->f1).ptr_atts).zero_term;_LL36A:
# 3208
 old_elt_typ=_tmp588;goto _LL368;}else{_LL36B: _LL36C: {
# 3210
const char*_tmpBB5;void*_tmpBB4;old_elt_typ=(
(_tmpBB4=0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBB5="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(_tmpBB5,sizeof(char),52))),_tag_dyneither(_tmpBB4,sizeof(void*),0))));}}_LL368:;}{
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
goto _LL35B;};}case 26: _LL35E: _tmp57E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp575)->f1;_tmp57F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp575)->f2;_tmp580=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp575)->f3;_tmp581=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp575)->f4;_LL35F:
# 3230
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp49B,old_typ,_tmp57F,(void*)_check_null(_tmp580->topt),_tmp581,_tmp580,_tmp57E);
goto _LL35B;case 27: _LL360: _tmp57B=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp575)->f1;_tmp57C=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp575)->f2;_tmp57D=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp575)->f3;_LL361:
# 3234
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp49B,old_typ,_tmp57B,_tmp57C,_tmp57D,0,0);
goto _LL35B;case 28: _LL362: _tmp577=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp575)->f1;_tmp578=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp575)->f2;_tmp579=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp575)->f3;_tmp57A=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp575)->f4;_LL363: {
# 3239
struct Cyc_Absyn_Exp*_tmp58F=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp49C->topt),_tmp578,1,_tmp49B,_tmp579,_tmp577);
e->r=_tmp58F->r;
e->topt=_tmp58F->topt;
goto _LL35B;}case 23: _LL364: _tmp576=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp575)->f1;_LL365:
# 3245
 e->r=(Cyc_Toc_init_tuple(nv,1,_tmp49B,_tmp576))->r;
goto _LL35B;default: _LL366: _LL367: {
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
{void*_tmp592=_tmp49C->r;void*_tmp593=_tmp592;void*_tmp594;struct Cyc_Absyn_Exp*_tmp595;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp593)->tag == 13){_LL36E: _tmp594=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp593)->f1;_tmp595=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp593)->f2;_LL36F:
# 3273
{struct _tuple29 _tmpBB6;struct _tuple29 _tmp596=(_tmpBB6.f1=Cyc_Tcutil_compress(_tmp594),((_tmpBB6.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp595->topt)),_tmpBB6)));struct _tuple29 _tmp597=_tmp596;void*_tmp598;union Cyc_Absyn_Constraint*_tmp599;union Cyc_Absyn_Constraint*_tmp59A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp597.f1)->tag == 5){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp597.f2)->tag == 5){_LL373: _tmp598=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp597.f1)->f1).elt_typ;_tmp599=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp597.f1)->f1).ptr_atts).bounds;_tmp59A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp597.f2)->f1).ptr_atts).bounds;_LL374:
# 3276
{struct _tuple29 _tmpBB7;struct _tuple29 _tmp59B=(_tmpBB7.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp599),((_tmpBB7.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp59A),_tmpBB7)));struct _tuple29 _tmp59C=_tmp59B;struct Cyc_Absyn_Exp*_tmp59D;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp59C.f1)->tag == 0){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp59C.f2)->tag == 1){_LL378: _tmp59D=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp59C.f2)->f1;_LL379:
# 3278
 Cyc_Toc_exp_to_c(nv,_tmp595);
inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);
done=1;{
struct Cyc_Absyn_Exp*_tmp59E=Cyc_Toc__init_dyneither_ptr_e;
{struct Cyc_Absyn_Exp*_tmpBB8[4];e->r=Cyc_Toc_fncall_exp_r(_tmp59E,(
(_tmpBB8[3]=_tmp59D,((_tmpBB8[2]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp598),0),((_tmpBB8[1]=_tmp595,((_tmpBB8[0]=mexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBB8,sizeof(struct Cyc_Absyn_Exp*),4)))))))))));}
# 3286
goto _LL377;};}else{goto _LL37A;}}else{_LL37A: _LL37B:
 goto _LL377;}_LL377:;}
# 3289
goto _LL372;}else{goto _LL375;}}else{_LL375: _LL376:
 goto _LL372;}_LL372:;}
# 3292
goto _LL36D;}else{_LL370: _LL371:
 goto _LL36D;}_LL36D:;}
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
goto _LL35B;};}}_LL35B:;}
# 3306
goto _LL291;};case 17: _LL2B6: _tmp49A=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_LL2B7: {
# 3309
int _tmp5A5=Cyc_Toc_set_in_sizeof(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp49A);
Cyc_Toc_set_in_sizeof(nv,_tmp5A5);
goto _LL291;}case 16: _LL2B8: _tmp499=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_LL2B9:
{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpBBB;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpBBA;e->r=(void*)((_tmpBBA=_cycalloc(sizeof(*_tmpBBA)),((_tmpBBA[0]=((_tmpBBB.tag=16,((_tmpBBB.f1=Cyc_Toc_typ_to_c(_tmp499),_tmpBBB)))),_tmpBBA))));}goto _LL291;case 18: _LL2BA: _tmp497=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp498=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL2BB: {
# 3315
void*_tmp5A8=_tmp497;
struct Cyc_List_List*_tmp5A9=_tmp498;
for(0;_tmp5A9 != 0;_tmp5A9=_tmp5A9->tl){
void*_tmp5AA=(void*)_tmp5A9->hd;void*_tmp5AB=_tmp5AA;unsigned int _tmp5AC;struct _dyneither_ptr*_tmp5AD;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp5AB)->tag == 0){_LL37D: _tmp5AD=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp5AB)->f1;_LL37E:
 goto _LL37C;}else{_LL37F: _tmp5AC=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp5AB)->f1;_LL380:
# 3321
{void*_tmp5AE=Cyc_Tcutil_compress(_tmp5A8);void*_tmp5AF=_tmp5AE;struct Cyc_Absyn_Datatypefield*_tmp5B0;struct Cyc_List_List*_tmp5B1;struct Cyc_List_List*_tmp5B2;union Cyc_Absyn_AggrInfoU _tmp5B3;switch(*((int*)_tmp5AF)){case 11: _LL382: _tmp5B3=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5AF)->f1).aggr_info;_LL383: {
# 3323
struct Cyc_Absyn_Aggrdecl*_tmp5B4=Cyc_Absyn_get_known_aggrdecl(_tmp5B3);
if(_tmp5B4->impl == 0){
const char*_tmpBBE;void*_tmpBBD;(_tmpBBD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBBE="struct fields must be known",_tag_dyneither(_tmpBBE,sizeof(char),28))),_tag_dyneither(_tmpBBD,sizeof(void*),0)));}
_tmp5B2=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp5B4->impl))->fields;goto _LL385;}case 12: _LL384: _tmp5B2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5AF)->f2;_LL385: {
# 3328
struct Cyc_Absyn_Aggrfield*_tmp5B7=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp5B2,(int)_tmp5AC);
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpBC1;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpBC0;_tmp5A9->hd=(void*)((void*)((_tmpBC0=_cycalloc(sizeof(*_tmpBC0)),((_tmpBC0[0]=((_tmpBC1.tag=0,((_tmpBC1.f1=_tmp5B7->name,_tmpBC1)))),_tmpBC0)))));}
_tmp5A8=_tmp5B7->type;
goto _LL381;}case 10: _LL386: _tmp5B1=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5AF)->f1;_LL387:
# 3333
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpBC4;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpBC3;_tmp5A9->hd=(void*)((void*)((_tmpBC3=_cycalloc(sizeof(*_tmpBC3)),((_tmpBC3[0]=((_tmpBC4.tag=0,((_tmpBC4.f1=Cyc_Absyn_fieldname((int)(_tmp5AC + 1)),_tmpBC4)))),_tmpBC3)))));}
_tmp5A8=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp5B1,(int)_tmp5AC)).f2;
goto _LL381;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp5AF)->f1).field_info).KnownDatatypefield).tag == 2){_LL388: _tmp5B0=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp5AF)->f1).field_info).KnownDatatypefield).val).f2;_LL389:
# 3337
 if(_tmp5AC == 0){
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpBC7;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpBC6;_tmp5A9->hd=(void*)((void*)((_tmpBC6=_cycalloc(sizeof(*_tmpBC6)),((_tmpBC6[0]=((_tmpBC7.tag=0,((_tmpBC7.f1=Cyc_Toc_tag_sp,_tmpBC7)))),_tmpBC6)))));}else{
# 3340
_tmp5A8=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp5B0->typs,(int)(_tmp5AC - 1))).f2;{
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpBCA;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpBC9;_tmp5A9->hd=(void*)((void*)((_tmpBC9=_cycalloc(sizeof(*_tmpBC9)),((_tmpBC9[0]=((_tmpBCA.tag=0,((_tmpBCA.f1=Cyc_Absyn_fieldname((int)_tmp5AC),_tmpBCA)))),_tmpBC9)))));};}
# 3343
goto _LL381;}else{goto _LL38A;}default: _LL38A: _LL38B:
{const char*_tmpBCD;void*_tmpBCC;(_tmpBCC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBCD="impossible type for offsetof tuple index",_tag_dyneither(_tmpBCD,sizeof(char),41))),_tag_dyneither(_tmpBCC,sizeof(void*),0)));}
goto _LL381;}_LL381:;}
# 3347
goto _LL37C;}_LL37C:;}
# 3350
{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpBD0;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpBCF;e->r=(void*)((_tmpBCF=_cycalloc(sizeof(*_tmpBCF)),((_tmpBCF[0]=((_tmpBD0.tag=18,((_tmpBD0.f1=Cyc_Toc_typ_to_c(_tmp497),((_tmpBD0.f2=_tmp498,_tmpBD0)))))),_tmpBCF))));}
goto _LL291;}case 19: _LL2BC: _tmp496=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_LL2BD: {
# 3353
int _tmp5C4=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp5C5=Cyc_Tcutil_compress((void*)_check_null(_tmp496->topt));
{void*_tmp5C6=_tmp5C5;void*_tmp5C7;struct Cyc_Absyn_Tqual _tmp5C8;void*_tmp5C9;union Cyc_Absyn_Constraint*_tmp5CA;union Cyc_Absyn_Constraint*_tmp5CB;union Cyc_Absyn_Constraint*_tmp5CC;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C6)->tag == 5){_LL38D: _tmp5C7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C6)->f1).elt_typ;_tmp5C8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C6)->f1).elt_tq;_tmp5C9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C6)->f1).ptr_atts).rgn;_tmp5CA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C6)->f1).ptr_atts).nullable;_tmp5CB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C6)->f1).ptr_atts).bounds;_tmp5CC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C6)->f1).ptr_atts).zero_term;_LL38E:
# 3358
{void*_tmp5CD=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5CB);void*_tmp5CE=_tmp5CD;struct Cyc_Absyn_Exp*_tmp5CF;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5CE)->tag == 1){_LL392: _tmp5CF=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5CE)->f1;_LL393: {
# 3360
int do_null_check=!Cyc_Toc_in_sizeof(nv) && Cyc_Toc_need_null_check(_tmp496);
Cyc_Toc_exp_to_c(nv,_tmp496);
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpBD3;void*_tmpBD2;(_tmpBD2=0,Cyc_Tcutil_warn(e->loc,((_tmpBD3="inserted null check due to dereference",_tag_dyneither(_tmpBD3,sizeof(char),39))),_tag_dyneither(_tmpBD2,sizeof(void*),0)));}{
# 3366
struct Cyc_List_List*_tmpBD4;_tmp496->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(_tmp5C5),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpBD4=_cycalloc(sizeof(*_tmpBD4)),((_tmpBD4->hd=Cyc_Absyn_copy_exp(_tmp496),((_tmpBD4->tl=0,_tmpBD4)))))),0));};}
# 3374
if(!Cyc_Toc_in_sizeof(nv) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5CC)){
struct _tuple12 _tmp5D3=Cyc_Evexp_eval_const_uint_exp(_tmp5CF);struct _tuple12 _tmp5D4=_tmp5D3;unsigned int _tmp5D5;int _tmp5D6;_LL397: _tmp5D5=_tmp5D4.f1;_tmp5D6=_tmp5D4.f2;_LL398:;
# 3380
if(!_tmp5D6  || _tmp5D5 <= 0){
const char*_tmpBD7;void*_tmpBD6;(_tmpBD6=0,Cyc_Tcutil_terr(e->loc,((_tmpBD7="cannot determine dereference is in bounds",_tag_dyneither(_tmpBD7,sizeof(char),42))),_tag_dyneither(_tmpBD6,sizeof(void*),0)));}}
# 3383
goto _LL391;}}else{_LL394: _LL395: {
# 3386
struct Cyc_Absyn_Exp*_tmp5D9=Cyc_Absyn_uint_exp(0,0);
_tmp5D9->topt=Cyc_Absyn_uint_typ;
e->r=Cyc_Toc_subscript_exp_r(_tmp496,_tmp5D9);
Cyc_Toc_exp_to_c(nv,e);
goto _LL391;}}_LL391:;}
# 3392
goto _LL38C;}else{_LL38F: _LL390: {
const char*_tmpBDA;void*_tmpBD9;(_tmpBD9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBDA="exp_to_c: Deref: non-pointer",_tag_dyneither(_tmpBDA,sizeof(char),29))),_tag_dyneither(_tmpBD9,sizeof(void*),0)));}}_LL38C:;}
# 3395
Cyc_Toc_set_lhs(nv,_tmp5C4);
goto _LL291;};}case 20: _LL2BE: _tmp492=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp493=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_tmp494=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp46E)->f3;_tmp495=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp46E)->f4;_LL2BF: {
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
goto _LL291;}case 21: _LL2C0: _tmp48E=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp48F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_tmp490=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp46E)->f3;_tmp491=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp46E)->f4;_LL2C1: {
# 3410
int _tmp5DC=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp48E->topt));
int do_null_check=Cyc_Toc_need_null_check(_tmp48E);
Cyc_Toc_exp_to_c(nv,_tmp48E);{
int is_poly=Cyc_Toc_is_poly_project(e);
struct Cyc_Absyn_PtrInfo _tmp5DD=Cyc_Toc_get_ptr_type(e1typ);struct Cyc_Absyn_PtrInfo _tmp5DE=_tmp5DD;void*_tmp5DF;struct Cyc_Absyn_Tqual _tmp5E0;void*_tmp5E1;union Cyc_Absyn_Constraint*_tmp5E2;union Cyc_Absyn_Constraint*_tmp5E3;union Cyc_Absyn_Constraint*_tmp5E4;_LL39A: _tmp5DF=_tmp5DE.elt_typ;_tmp5E0=_tmp5DE.elt_tq;_tmp5E1=(_tmp5DE.ptr_atts).rgn;_tmp5E2=(_tmp5DE.ptr_atts).nullable;_tmp5E3=(_tmp5DE.ptr_atts).bounds;_tmp5E4=(_tmp5DE.ptr_atts).zero_term;_LL39B:;
{void*_tmp5E5=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5E3);void*_tmp5E6=_tmp5E5;struct Cyc_Absyn_Exp*_tmp5E7;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5E6)->tag == 1){_LL39D: _tmp5E7=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5E6)->f1;_LL39E: {
# 3421
struct _tuple12 _tmp5E8=Cyc_Evexp_eval_const_uint_exp(_tmp5E7);struct _tuple12 _tmp5E9=_tmp5E8;unsigned int _tmp5EA;int _tmp5EB;_LL3A2: _tmp5EA=_tmp5E9.f1;_tmp5EB=_tmp5E9.f2;_LL3A3:;
if(_tmp5EB){
if(_tmp5EA < 1){
const char*_tmpBDD;void*_tmpBDC;(_tmpBDC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpBDD="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpBDD,sizeof(char),44))),_tag_dyneither(_tmpBDC,sizeof(void*),0)));}
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpBE0;void*_tmpBDF;(_tmpBDF=0,Cyc_Tcutil_warn(e->loc,((_tmpBE0="inserted null check due to dereference",_tag_dyneither(_tmpBE0,sizeof(char),39))),_tag_dyneither(_tmpBDF,sizeof(void*),0)));}{
# 3429
struct Cyc_Absyn_Exp*_tmpBE1[1];_tmp48E->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpBE1[0]=Cyc_Absyn_new_exp(_tmp48E->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBE1,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}}else{
# 3434
if(!Cyc_Evexp_c_can_eval(_tmp5E7)){
const char*_tmpBE4;void*_tmpBE3;(_tmpBE3=0,Cyc_Tcutil_terr(e->loc,((_tmpBE4="cannot determine pointer dereference in bounds",_tag_dyneither(_tmpBE4,sizeof(char),47))),_tag_dyneither(_tmpBE3,sizeof(void*),0)));}{
# 3438
struct Cyc_Absyn_Exp*_tmpBE5[4];_tmp48E->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpBE5[3]=
# 3442
Cyc_Absyn_uint_exp(0,0),((_tmpBE5[2]=
# 3441
Cyc_Absyn_sizeoftyp_exp(_tmp5DF,0),((_tmpBE5[1]=_tmp5E7,((_tmpBE5[0]=
# 3440
Cyc_Absyn_new_exp(_tmp48E->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBE5,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0));};}
# 3444
goto _LL39C;}}else{_LL39F: _LL3A0: {
# 3447
void*ta1=Cyc_Toc_typ_to_c(_tmp5DF);
{struct Cyc_Absyn_Exp*_tmpBE6[3];_tmp48E->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp5E0),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_dyneither_subscript_e,(
(_tmpBE6[2]=
# 3452
Cyc_Absyn_uint_exp(0,0),((_tmpBE6[1]=
# 3451
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpBE6[0]=
# 3450
Cyc_Absyn_new_exp(_tmp48E->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBE6,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}
# 3455
goto _LL39C;}}_LL39C:;}
# 3457
if(_tmp490  && _tmp491)
e->r=Cyc_Toc_check_tagged_union(_tmp48E,Cyc_Toc_typ_to_c(e1typ),_tmp5DF,_tmp48F,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);
# 3460
if(is_poly  && _tmp491)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
Cyc_Toc_set_lhs(nv,_tmp5DC);
goto _LL291;};};}case 22: _LL2C2: _tmp48C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp48D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL2C3: {
# 3465
int _tmp5F5=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp5F6=Cyc_Tcutil_compress((void*)_check_null(_tmp48C->topt));
# 3469
{void*_tmp5F7=_tmp5F6;void*_tmp5F8;struct Cyc_Absyn_Tqual _tmp5F9;void*_tmp5FA;union Cyc_Absyn_Constraint*_tmp5FB;union Cyc_Absyn_Constraint*_tmp5FC;union Cyc_Absyn_Constraint*_tmp5FD;struct Cyc_List_List*_tmp5FE;switch(*((int*)_tmp5F7)){case 10: _LL3A5: _tmp5FE=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5F7)->f1;_LL3A6:
# 3472
 Cyc_Toc_exp_to_c(nv,_tmp48C);
Cyc_Toc_exp_to_c(nv,_tmp48D);{
struct _tuple12 _tmp5FF=Cyc_Evexp_eval_const_uint_exp(_tmp48D);struct _tuple12 _tmp600=_tmp5FF;unsigned int _tmp601;int _tmp602;_LL3AC: _tmp601=_tmp600.f1;_tmp602=_tmp600.f2;_LL3AD:;
if(!_tmp602){
const char*_tmpBE9;void*_tmpBE8;(_tmpBE8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpBE9="unknown tuple subscript in translation to C",_tag_dyneither(_tmpBE9,sizeof(char),44))),_tag_dyneither(_tmpBE8,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp48C,Cyc_Absyn_fieldname((int)(_tmp601 + 1)));
goto _LL3A4;};case 5: _LL3A7: _tmp5F8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F7)->f1).elt_typ;_tmp5F9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F7)->f1).elt_tq;_tmp5FA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F7)->f1).ptr_atts).rgn;_tmp5FB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F7)->f1).ptr_atts).nullable;_tmp5FC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F7)->f1).ptr_atts).bounds;_tmp5FD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F7)->f1).ptr_atts).zero_term;_LL3A8: {
# 3480
struct Cyc_List_List*_tmp605=Cyc_Toc_get_relns(_tmp48C);
int _tmp606=Cyc_Toc_need_null_check(_tmp48C);
int _tmp607=Cyc_Toc_in_sizeof(nv);
# 3488
int in_bnds=_tmp607;
{void*_tmp608=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5FC);void*_tmp609=_tmp608;_LL3AF: _LL3B0:
# 3491
 in_bnds=in_bnds  || Cyc_Toc_check_bounds(_tmp5F6,_tmp605,_tmp48C,_tmp48D);
if(Cyc_Toc_warn_bounds_checks  && !in_bnds){
const char*_tmpBED;void*_tmpBEC[1];struct Cyc_String_pa_PrintArg_struct _tmpBEB;(_tmpBEB.tag=0,((_tmpBEB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpBEC[0]=& _tmpBEB,Cyc_Tcutil_warn(e->loc,((_tmpBED="bounds check necessary for %s",_tag_dyneither(_tmpBED,sizeof(char),30))),_tag_dyneither(_tmpBEC,sizeof(void*),1)))))));}_LL3AE:;}
# 3498
Cyc_Toc_exp_to_c(nv,_tmp48C);
Cyc_Toc_exp_to_c(nv,_tmp48D);
++ Cyc_Toc_total_bounds_checks;
{void*_tmp60D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5FC);void*_tmp60E=_tmp60D;struct Cyc_Absyn_Exp*_tmp60F;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp60E)->tag == 1){_LL3B2: _tmp60F=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp60E)->f1;_LL3B3: {
# 3503
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5FB) && _tmp606;
void*ta1=Cyc_Toc_typ_to_c(_tmp5F8);
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp5F9);
if(in_bnds  && !possibly_null)
++ Cyc_Toc_bounds_checks_eliminated;else{
if(in_bnds  && possibly_null){
++ Cyc_Toc_bounds_checks_eliminated;
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpBF0;void*_tmpBEF;(_tmpBEF=0,Cyc_Tcutil_warn(e->loc,((_tmpBF0="inserted null check due to dereference",_tag_dyneither(_tmpBF0,sizeof(char),39))),_tag_dyneither(_tmpBEF,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpBF1[1];_tmp48C->r=Cyc_Toc_cast_it_r(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpBF1[0]=Cyc_Absyn_copy_exp(_tmp48C),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBF1,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}else{
# 3515
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5FD)){
# 3517
if(!Cyc_Evexp_c_can_eval(_tmp60F)){
const char*_tmpBF4;void*_tmpBF3;(_tmpBF3=0,Cyc_Tcutil_terr(e->loc,((_tmpBF4="cannot determine subscript is in bounds",_tag_dyneither(_tmpBF4,sizeof(char),40))),_tag_dyneither(_tmpBF3,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp48C);
struct Cyc_Absyn_Exp*_tmpBF5[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(function_e,(
(_tmpBF5[2]=_tmp48D,((_tmpBF5[1]=_tmp60F,((_tmpBF5[0]=_tmp48C,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBF5,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp60F)){
const char*_tmpBF8;void*_tmpBF7;(_tmpBF7=0,Cyc_Tcutil_terr(e->loc,((_tmpBF8="cannot determine subscript is in bounds",_tag_dyneither(_tmpBF8,sizeof(char),40))),_tag_dyneither(_tmpBF7,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpBFB;void*_tmpBFA;(_tmpBFA=0,Cyc_Tcutil_warn(e->loc,((_tmpBFB="inserted null check due to dereference",_tag_dyneither(_tmpBFB,sizeof(char),39))),_tag_dyneither(_tmpBFA,sizeof(void*),0)));}{
# 3529
struct Cyc_Absyn_Exp*_tmpBFC[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpBFC[3]=_tmp48D,((_tmpBFC[2]=
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpBFC[1]=_tmp60F,((_tmpBFC[0]=_tmp48C,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBFC,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{
# 3535
if(!Cyc_Evexp_c_can_eval(_tmp60F)){
const char*_tmpBFF;void*_tmpBFE;(_tmpBFE=0,Cyc_Tcutil_terr(e->loc,((_tmpBFF="cannot determine subscript is in bounds",_tag_dyneither(_tmpBFF,sizeof(char),40))),_tag_dyneither(_tmpBFE,sizeof(void*),0)));}{
# 3538
struct Cyc_Absyn_Exp*_tmpC00[2];_tmp48D->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,(
(_tmpC00[1]=Cyc_Absyn_copy_exp(_tmp48D),((_tmpC00[0]=_tmp60F,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC00,sizeof(struct Cyc_Absyn_Exp*),2)))))));};}}}}
# 3541
goto _LL3B1;}}else{_LL3B4: _LL3B5: {
# 3543
void*ta1=Cyc_Toc_typ_to_c(_tmp5F8);
if(in_bnds){
# 3547
++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp5F9),
Cyc_Toc_member_exp(_tmp48C,Cyc_Toc_curr_sp,0)),_tmp48D);}else{
# 3552
struct Cyc_Absyn_Exp*_tmp61E=Cyc_Toc__check_dyneither_subscript_e;
struct Cyc_Absyn_Exp*_tmpC01[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp5F9),
Cyc_Absyn_fncall_exp(_tmp61E,(
(_tmpC01[2]=_tmp48D,((_tmpC01[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC01[0]=_tmp48C,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC01,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}
# 3558
goto _LL3B1;}}_LL3B1:;}
# 3560
goto _LL3A4;}default: _LL3A9: _LL3AA: {
const char*_tmpC04;void*_tmpC03;(_tmpC03=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC04="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(_tmpC04,sizeof(char),49))),_tag_dyneither(_tmpC03,sizeof(void*),0)));}}_LL3A4:;}
# 3563
Cyc_Toc_set_lhs(nv,_tmp5F5);
goto _LL291;};}case 23: _LL2C4: _tmp48B=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_LL2C5:
# 3566
 if(!Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp48B))->r;else{
# 3571
struct Cyc_List_List*_tmp622=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp48B);
void*_tmp623=Cyc_Toc_add_tuple_type(_tmp622);
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp48B != 0;(_tmp48B=_tmp48B->tl,i ++)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp48B->hd);{
struct _tuple19*_tmpC07;struct Cyc_List_List*_tmpC06;dles=((_tmpC06=_cycalloc(sizeof(*_tmpC06)),((_tmpC06->hd=((_tmpC07=_cycalloc(sizeof(*_tmpC07)),((_tmpC07->f1=0,((_tmpC07->f2=(struct Cyc_Absyn_Exp*)_tmp48B->hd,_tmpC07)))))),((_tmpC06->tl=dles,_tmpC06))))));};}}
# 3578
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}
# 3581
goto _LL291;case 25: _LL2C6: _tmp48A=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_LL2C7:
# 3585
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp48A);
{struct Cyc_List_List*_tmp626=_tmp48A;for(0;_tmp626 != 0;_tmp626=_tmp626->tl){
struct _tuple19*_tmp627=(struct _tuple19*)_tmp626->hd;struct _tuple19*_tmp628=_tmp627;struct Cyc_Absyn_Exp*_tmp629;_LL3B7: _tmp629=_tmp628->f2;_LL3B8:;
Cyc_Toc_exp_to_c(nv,_tmp629);}}
# 3590
goto _LL291;case 26: _LL2C8: _tmp486=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp487=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_tmp488=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp46E)->f3;_tmp489=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp46E)->f4;_LL2C9: {
# 3594
struct _tuple12 _tmp62A=Cyc_Evexp_eval_const_uint_exp(_tmp487);struct _tuple12 _tmp62B=_tmp62A;unsigned int _tmp62C;int _tmp62D;_LL3BA: _tmp62C=_tmp62B.f1;_tmp62D=_tmp62B.f2;_LL3BB:;{
void*_tmp62E=Cyc_Toc_typ_to_c((void*)_check_null(_tmp488->topt));
Cyc_Toc_exp_to_c(nv,_tmp488);{
struct Cyc_List_List*es=0;
# 3599
if(!Cyc_Toc_is_zero(_tmp488)){
if(!_tmp62D){
const char*_tmpC0A;void*_tmpC09;(_tmpC09=0,Cyc_Tcutil_terr(_tmp487->loc,((_tmpC0A="cannot determine value of constant",_tag_dyneither(_tmpC0A,sizeof(char),35))),_tag_dyneither(_tmpC09,sizeof(void*),0)));}
{unsigned int i=0;for(0;i < _tmp62C;++ i){
struct _tuple19*_tmpC0D;struct Cyc_List_List*_tmpC0C;es=((_tmpC0C=_cycalloc(sizeof(*_tmpC0C)),((_tmpC0C->hd=((_tmpC0D=_cycalloc(sizeof(*_tmpC0D)),((_tmpC0D->f1=0,((_tmpC0D->f2=_tmp488,_tmpC0D)))))),((_tmpC0C->tl=es,_tmpC0C))))));}}
# 3605
if(_tmp489){
struct Cyc_Absyn_Exp*_tmp633=Cyc_Toc_cast_it(_tmp62E,Cyc_Absyn_uint_exp(0,0));
struct _tuple19*_tmpC10;struct Cyc_List_List*_tmpC0F;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,((_tmpC0F=_cycalloc(sizeof(*_tmpC0F)),((_tmpC0F->hd=((_tmpC10=_cycalloc(sizeof(*_tmpC10)),((_tmpC10->f1=0,((_tmpC10->f2=_tmp633,_tmpC10)))))),((_tmpC0F->tl=0,_tmpC0F)))))));}}
# 3610
e->r=Cyc_Toc_unresolvedmem_exp_r(0,es);
goto _LL291;};};}case 27: _LL2CA: _tmp483=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp484=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_tmp485=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp46E)->f3;_LL2CB:
# 3615
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,0);
goto _LL291;case 28: _LL2CC: _tmp47F=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp480=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_tmp481=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp46E)->f3;_tmp482=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp46E)->f4;_LL2CD:
# 3620
 if(!Cyc_Toc_is_toplevel(nv)){
struct Cyc_Absyn_Exp*_tmp636=Cyc_Toc_init_struct(nv,old_typ,_tmp480,0,0,_tmp481,_tmp47F);
e->r=_tmp636->r;
e->topt=_tmp636->topt;}else{
# 3631
if(_tmp482 == 0){
const char*_tmpC13;void*_tmpC12;(_tmpC12=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC13="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpC13,sizeof(char),38))),_tag_dyneither(_tmpC12,sizeof(void*),0)));}{
struct Cyc_Absyn_Aggrdecl*sd2=_tmp482;
# 3635
struct _RegionHandle _tmp639=_new_region("rgn");struct _RegionHandle*rgn=& _tmp639;_push_region(rgn);
{struct Cyc_List_List*_tmp63A=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp481,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
# 3639
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp482->impl))->tagged){
# 3641
struct _tuple30*_tmp63B=(struct _tuple30*)((struct Cyc_List_List*)_check_null(_tmp63A))->hd;struct _tuple30*_tmp63C=_tmp63B;struct Cyc_Absyn_Aggrfield*_tmp63D;struct Cyc_Absyn_Exp*_tmp63E;_LL3BD: _tmp63D=_tmp63C->f1;_tmp63E=_tmp63C->f2;_LL3BE:;{
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
struct _tuple19*_tmpC18;struct _tuple19*_tmpC17;struct _tuple19*_tmpC16[2];struct Cyc_List_List*_tmp641=(_tmpC16[1]=((_tmpC17=_cycalloc(sizeof(*_tmpC17)),((_tmpC17->f1=0,((_tmpC17->f2=_tmp63E,_tmpC17)))))),((_tmpC16[0]=((_tmpC18=_cycalloc(sizeof(*_tmpC18)),((_tmpC18->f1=0,((_tmpC18->f2=field_tag_exp,_tmpC18)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC16,sizeof(struct _tuple19*),2)))));
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp641,0);
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpC1E;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpC1D;void*_tmpC1C[1];struct Cyc_List_List*ds=(_tmpC1C[0]=(void*)((_tmpC1E=_cycalloc(sizeof(*_tmpC1E)),((_tmpC1E[0]=((_tmpC1D.tag=1,((_tmpC1D.f1=_tmp63D->name,_tmpC1D)))),_tmpC1E)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC1C,sizeof(void*),1)));
struct _tuple19*_tmpC21;struct _tuple19*_tmpC20[1];struct Cyc_List_List*dles=(_tmpC20[0]=((_tmpC21=_cycalloc(sizeof(*_tmpC21)),((_tmpC21->f1=ds,((_tmpC21->f2=umem,_tmpC21)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC20,sizeof(struct _tuple19*),1)));
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{
# 3659
struct Cyc_List_List*_tmp64A=0;
struct Cyc_List_List*_tmp64B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields;
for(0;_tmp64B != 0;_tmp64B=_tmp64B->tl){
struct Cyc_List_List*_tmp64C=_tmp63A;for(0;_tmp64C != 0;_tmp64C=_tmp64C->tl){
if((*((struct _tuple30*)_tmp64C->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp64B->hd){
struct _tuple30*_tmp64D=(struct _tuple30*)_tmp64C->hd;struct _tuple30*_tmp64E=_tmp64D;struct Cyc_Absyn_Aggrfield*_tmp64F;struct Cyc_Absyn_Exp*_tmp650;_LL3C0: _tmp64F=_tmp64E->f1;_tmp650=_tmp64E->f2;_LL3C1:;{
void*_tmp651=Cyc_Toc_typ_to_c(_tmp64F->type);
void*_tmp652=Cyc_Toc_typ_to_c((void*)_check_null(_tmp650->topt));
Cyc_Toc_exp_to_c(nv,_tmp650);
# 3669
if(!Cyc_Tcutil_unify(_tmp651,_tmp652)){
# 3671
if(!Cyc_Tcutil_is_arithmetic_type(_tmp651) || !
Cyc_Tcutil_is_arithmetic_type(_tmp652))
_tmp650=Cyc_Toc_cast_it(_tmp651,Cyc_Absyn_copy_exp(_tmp650));}
{struct _tuple19*_tmpC24;struct Cyc_List_List*_tmpC23;_tmp64A=((_tmpC23=_cycalloc(sizeof(*_tmpC23)),((_tmpC23->hd=((_tmpC24=_cycalloc(sizeof(*_tmpC24)),((_tmpC24->f1=0,((_tmpC24->f2=_tmp650,_tmpC24)))))),((_tmpC23->tl=_tmp64A,_tmpC23))))));}
break;};}}}
# 3678
e->r=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp64A));}}
# 3636
;_pop_region(rgn);};}
# 3682
goto _LL291;case 29: _LL2CE: _tmp47D=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp47E=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL2CF: {
# 3684
struct Cyc_List_List*fs;
{void*_tmp655=Cyc_Tcutil_compress(_tmp47D);void*_tmp656=_tmp655;struct Cyc_List_List*_tmp657;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp656)->tag == 12){_LL3C3: _tmp657=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp656)->f2;_LL3C4:
 fs=_tmp657;goto _LL3C2;}else{_LL3C5: _LL3C6: {
const char*_tmpC28;void*_tmpC27[1];struct Cyc_String_pa_PrintArg_struct _tmpC26;(_tmpC26.tag=0,((_tmpC26.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp47D)),((_tmpC27[0]=& _tmpC26,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC28="anon struct has type %s",_tag_dyneither(_tmpC28,sizeof(char),24))),_tag_dyneither(_tmpC27,sizeof(void*),1)))))));}}_LL3C2:;}{
# 3689
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
# 3703
e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp47E);}
# 3705
_npop_handler(0);goto _LL291;
# 3689
;_pop_region(rgn);};}case 30: _LL2D0: _tmp47A=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp47B=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_tmp47C=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp46E)->f3;_LL2D1: {
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
struct _tuple19*_tmpC2B;struct Cyc_List_List*_tmpC2A;dles=((_tmpC2A=_cycalloc(sizeof(*_tmpC2A)),((_tmpC2A->hd=((_tmpC2B=_cycalloc(sizeof(*_tmpC2B)),((_tmpC2B->f1=0,((_tmpC2B->f2=cur_e,_tmpC2B)))))),((_tmpC2A->tl=dles,_tmpC2A))))));};}
# 3732
{struct _tuple19*_tmpC2E;struct Cyc_List_List*_tmpC2D;dles=((_tmpC2D=_cycalloc(sizeof(*_tmpC2D)),((_tmpC2D->hd=((_tmpC2E=_cycalloc(sizeof(*_tmpC2E)),((_tmpC2E->f1=0,((_tmpC2E->f2=tag_exp,_tmpC2E)))))),((_tmpC2D->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles),_tmpC2D))))));}
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{
# 3738
struct Cyc_List_List*_tmpC2F;struct Cyc_List_List*_tmp66B=
(_tmpC2F=_cycalloc(sizeof(*_tmpC2F)),((_tmpC2F->hd=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Toc_tag_sp,0),tag_exp,0),((_tmpC2F->tl=0,_tmpC2F)))));
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
struct Cyc_List_List*_tmpC30;_tmp66B=((_tmpC30=_cycalloc(sizeof(*_tmpC30)),((_tmpC30->hd=_tmp66D,((_tmpC30->tl=_tmp66B,_tmpC30))))));};}}{
# 3753
struct Cyc_Absyn_Stmt*_tmp66F=Cyc_Absyn_exp_stmt(_tmp664,0);
struct Cyc_List_List*_tmpC31;struct Cyc_Absyn_Stmt*_tmp670=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpC31=_cycalloc(sizeof(*_tmpC31)),((_tmpC31->hd=_tmp66F,((_tmpC31->tl=_tmp66B,_tmpC31))))))),0);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp663,datatype_ctype,0,_tmp670,0));};}
# 3757
goto _LL291;};}case 31: _LL2D2: _LL2D3:
# 3759
 goto _LL2D5;case 32: _LL2D4: _LL2D5:
 goto _LL291;case 33: _LL2D6: _tmp474=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46E)->f1).is_calloc;_tmp475=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46E)->f1).rgn;_tmp476=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46E)->f1).elt_type;_tmp477=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46E)->f1).num_elts;_tmp478=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46E)->f1).fat_result;_tmp479=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46E)->f1).inline_call;_LL2D7: {
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
struct Cyc_Absyn_Exp*_tmpC32[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpC32[2]=
Cyc_Absyn_var_exp(_tmp673,0),((_tmpC32[1]=
# 3781
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpC32[0]=Cyc_Absyn_var_exp(_tmp674,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC32,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{
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
struct Cyc_Absyn_Exp*_tmpC33[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((_tmpC33[2]=
Cyc_Absyn_var_exp(_tmp673,0),((_tmpC33[1]=
# 3794
Cyc_Absyn_uint_exp(1,0),((_tmpC33[0]=Cyc_Absyn_var_exp(_tmp674,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC33,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}{
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
goto _LL291;}case 34: _LL2D8: _tmp472=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp473=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL2D9: {
# 3832
void*e1_old_typ=(void*)_check_null(_tmp472->topt);
void*e2_old_typ=(void*)_check_null(_tmp473->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ) && !Cyc_Tcutil_is_pointer_type(e1_old_typ)){
const char*_tmpC36;void*_tmpC35;(_tmpC35=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC36="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",_tag_dyneither(_tmpC36,sizeof(char),57))),_tag_dyneither(_tmpC35,sizeof(void*),0)));}{
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
{struct Cyc_Absyn_Exp*_tmpC37[2];e->r=Cyc_Toc_fncall_exp_r(swap_fn,((_tmpC37[1]=_tmp473,((_tmpC37[0]=_tmp472,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC37,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
# 3873
goto _LL291;};};}case 37: _LL2DA: _tmp470=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_tmp471=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL2DB: {
# 3876
void*_tmp67B=Cyc_Tcutil_compress((void*)_check_null(_tmp470->topt));
Cyc_Toc_exp_to_c(nv,_tmp470);
{void*_tmp67C=_tmp67B;struct Cyc_Absyn_Aggrdecl*_tmp67D;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp67C)->tag == 11){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp67C)->f1).aggr_info).KnownAggr).tag == 2){_LL3CB: _tmp67D=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp67C)->f1).aggr_info).KnownAggr).val;_LL3CC: {
# 3880
struct Cyc_Absyn_Exp*_tmp67E=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp67D,_tmp471),0);
struct Cyc_Absyn_Exp*_tmp67F=Cyc_Toc_member_exp(_tmp470,_tmp471,0);
struct Cyc_Absyn_Exp*_tmp680=Cyc_Toc_member_exp(_tmp67F,Cyc_Toc_tag_sp,0);
e->r=(Cyc_Absyn_eq_exp(_tmp680,_tmp67E,0))->r;
goto _LL3CA;}}else{goto _LL3CD;}}else{_LL3CD: _LL3CE: {
const char*_tmpC3A;void*_tmpC39;(_tmpC39=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC3A="non-aggregate type in tagcheck",_tag_dyneither(_tmpC3A,sizeof(char),31))),_tag_dyneither(_tmpC39,sizeof(void*),0)));}}_LL3CA:;}
# 3887
goto _LL291;}case 36: _LL2DC: _tmp46F=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp46E)->f1;_LL2DD:
 Cyc_Toc_stmt_to_c(nv,_tmp46F);goto _LL291;case 35: _LL2DE: _LL2DF: {
const char*_tmpC3D;void*_tmpC3C;(_tmpC3C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC3D="UnresolvedMem",_tag_dyneither(_tmpC3D,sizeof(char),14))),_tag_dyneither(_tmpC3C,sizeof(void*),0)));}case 24: _LL2E0: _LL2E1: {
const char*_tmpC40;void*_tmpC3F;(_tmpC3F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC40="compoundlit",_tag_dyneither(_tmpC40,sizeof(char),12))),_tag_dyneither(_tmpC3F,sizeof(void*),0)));}case 38: _LL2E2: _LL2E3: {
const char*_tmpC43;void*_tmpC42;(_tmpC42=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC43="valueof(-)",_tag_dyneither(_tmpC43,sizeof(char),11))),_tag_dyneither(_tmpC42,sizeof(void*),0)));}default: _LL2E4: _LL2E5: {
const char*_tmpC46;void*_tmpC45;(_tmpC45=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC46="__asm__",_tag_dyneither(_tmpC46,sizeof(char),8))),_tag_dyneither(_tmpC45,sizeof(void*),0)));}}_LL291:;};}struct _tuple31{int f1;struct _dyneither_ptr*f2;struct _dyneither_ptr*f3;struct Cyc_Absyn_Switch_clause*f4;};
# 3920 "toc.cyc"
static struct _tuple31*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 3922
struct _tuple31*_tmpC47;return(_tmpC47=_region_malloc(r,sizeof(*_tmpC47)),((_tmpC47->f1=0,((_tmpC47->f2=Cyc_Toc_fresh_label(),((_tmpC47->f3=Cyc_Toc_fresh_label(),((_tmpC47->f4=sc,_tmpC47)))))))));}
# 3927
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(0;ps != 0;ps=((struct Cyc_List_List*)_check_null(ps))->tl){
struct Cyc_Tcpat_PathNode*_tmp68C=(struct Cyc_Tcpat_PathNode*)ps->hd;
# 3933
if((int)(((_tmp68C->orig_pat).pattern).tag == 1)){
union Cyc_Tcpat_PatOrWhere _tmpC48;void*t=(void*)_check_null(((_tmpC48=_tmp68C->orig_pat,(((_tmpC48.pattern).tag != 1?_throw_match(): 1,(_tmpC48.pattern).val))))->topt);
void*_tmp68D=Cyc_Tcutil_compress(Cyc_Toc_typ_to_c_array(t));
void*_tmp68E=_tmp68D;switch(*((int*)_tmp68E)){case 0: _LL3D0: _LL3D1:
# 3938
 goto _LL3D3;case 11: _LL3D2: _LL3D3:
 goto _LL3D5;case 12: _LL3D4: _LL3D5:
 goto _LL3CF;default: _LL3D6: _LL3D7:
 v=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c_array(t),v);goto _LL3CF;}_LL3CF:;}{
# 3944
void*_tmp690=_tmp68C->access;void*_tmp691=_tmp690;struct Cyc_Absyn_Datatypedecl*_tmp692;struct Cyc_Absyn_Datatypefield*_tmp693;unsigned int _tmp694;int _tmp695;struct _dyneither_ptr*_tmp696;unsigned int _tmp697;switch(*((int*)_tmp691)){case 0: _LL3D9: _LL3DA:
# 3949
 goto _LL3D8;case 1: _LL3DB: _LL3DC:
# 3954
 if(ps->tl != 0){
void*_tmp698=((struct Cyc_Tcpat_PathNode*)((struct Cyc_List_List*)_check_null(ps->tl))->hd)->access;void*_tmp699=_tmp698;struct Cyc_Absyn_Datatypedecl*_tmp69A;struct Cyc_Absyn_Datatypefield*_tmp69B;unsigned int _tmp69C;if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp699)->tag == 3){_LL3E4: _tmp69A=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp699)->f1;_tmp69B=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp699)->f2;_tmp69C=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp699)->f3;_LL3E5:
# 3957
 ps=ps->tl;
v=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp69B->name,_tmp69A->name)),Cyc_Toc_mt_tq),v);
v=Cyc_Absyn_aggrarrow_exp(v,Cyc_Absyn_fieldname((int)(_tmp69C + 1)),0);
continue;}else{_LL3E6: _LL3E7:
# 3962
 goto _LL3E3;}_LL3E3:;}
# 3965
v=Cyc_Absyn_deref_exp(v,0);
goto _LL3D8;case 2: _LL3DD: _tmp697=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp691)->f1;_LL3DE:
 v=Cyc_Toc_member_exp(v,Cyc_Absyn_fieldname((int)(_tmp697 + 1)),0);goto _LL3D8;case 4: _LL3DF: _tmp695=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp691)->f1;_tmp696=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp691)->f2;_LL3E0:
# 3969
 v=Cyc_Toc_member_exp(v,_tmp696,0);
if(_tmp695)
v=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0);
goto _LL3D8;default: _LL3E1: _tmp692=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp691)->f1;_tmp693=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp691)->f2;_tmp694=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp691)->f3;_LL3E2:
# 3974
 v=Cyc_Toc_member_exp(v,Cyc_Absyn_fieldname((int)(_tmp694 + 1)),0);
goto _LL3D8;}_LL3D8:;};}
# 3978
return v;}
# 3983
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
void*_tmp69D=t;struct Cyc_Absyn_Datatypedecl*_tmp69E;struct Cyc_Absyn_Datatypefield*_tmp69F;struct _dyneither_ptr*_tmp6A0;int _tmp6A1;int _tmp6A2;struct Cyc_Absyn_Datatypedecl*_tmp6A3;struct Cyc_Absyn_Datatypefield*_tmp6A4;unsigned int _tmp6A5;struct _dyneither_ptr _tmp6A6;int _tmp6A7;void*_tmp6A8;struct Cyc_Absyn_Enumfield*_tmp6A9;struct Cyc_Absyn_Enumdecl*_tmp6AA;struct Cyc_Absyn_Enumfield*_tmp6AB;struct Cyc_Absyn_Exp*_tmp6AC;switch(*((int*)_tmp69D)){case 0: _LL3E9: _tmp6AC=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp69D)->f1;_LL3EA:
# 3987
 if(_tmp6AC == 0)return v;else{return _tmp6AC;}case 1: _LL3EB: _LL3EC:
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_signed_int_exp(0,0),0);case 2: _LL3ED: _LL3EE:
 return Cyc_Absyn_neq_exp(v,Cyc_Absyn_signed_int_exp(0,0),0);case 3: _LL3EF: _tmp6AA=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp69D)->f1;_tmp6AB=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp69D)->f2;_LL3F0: {
# 3991
struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmpC4B;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpC4A;return Cyc_Absyn_eq_exp(v,Cyc_Absyn_new_exp((void*)((_tmpC4A=_cycalloc(sizeof(*_tmpC4A)),((_tmpC4A[0]=((_tmpC4B.tag=31,((_tmpC4B.f1=_tmp6AB->name,((_tmpC4B.f2=_tmp6AA,((_tmpC4B.f3=_tmp6AB,_tmpC4B)))))))),_tmpC4A)))),0),0);}case 4: _LL3F1: _tmp6A8=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp69D)->f1;_tmp6A9=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp69D)->f2;_LL3F2: {
# 3993
struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpC4E;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpC4D;return Cyc_Absyn_eq_exp(v,Cyc_Absyn_new_exp((void*)((_tmpC4D=_cycalloc(sizeof(*_tmpC4D)),((_tmpC4D[0]=((_tmpC4E.tag=32,((_tmpC4E.f1=_tmp6A9->name,((_tmpC4E.f2=_tmp6A8,((_tmpC4E.f3=_tmp6A9,_tmpC4E)))))))),_tmpC4D)))),0),0);}case 5: _LL3F3: _tmp6A6=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp69D)->f1;_tmp6A7=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp69D)->f2;_LL3F4:
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_float_exp(_tmp6A6,_tmp6A7,0),0);case 6: _LL3F5: _tmp6A5=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp69D)->f1;_LL3F6:
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_uint_exp(_tmp6A5,0),0);case 7: _LL3F7: _tmp6A2=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp69D)->f1;_tmp6A3=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp69D)->f2;_tmp6A4=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp69D)->f3;_LL3F8:
# 3999
 LOOP1: {
void*_tmp6B1=v->r;void*_tmp6B2=_tmp6B1;struct Cyc_Absyn_Exp*_tmp6B3;struct Cyc_Absyn_Exp*_tmp6B4;switch(*((int*)_tmp6B2)){case 13: _LL3FE: _tmp6B4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6B2)->f2;_LL3FF:
 v=_tmp6B4;goto LOOP1;case 19: _LL400: _tmp6B3=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6B2)->f1;_LL401:
 v=_tmp6B3;goto _LL3FD;default: _LL402: _LL403:
 goto _LL3FD;}_LL3FD:;}
# 4006
v=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp6A4->name,_tmp6A3->name)),Cyc_Toc_mt_tq),v);
return Cyc_Absyn_eq_exp(Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0),Cyc_Absyn_uint_exp((unsigned int)_tmp6A2,0),0);case 8: _LL3F9: _tmp6A0=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp69D)->f1;_tmp6A1=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp69D)->f2;_LL3FA:
# 4009
 v=Cyc_Toc_member_exp(Cyc_Toc_member_exp(v,_tmp6A0,0),Cyc_Toc_tag_sp,0);
return Cyc_Absyn_eq_exp(v,Cyc_Absyn_uint_exp((unsigned int)_tmp6A1,0),0);default: _LL3FB: _tmp69E=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp69D)->f1;_tmp69F=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp69D)->f2;_LL3FC:
# 4013
 LOOP2: {
void*_tmp6B5=v->r;void*_tmp6B6=_tmp6B5;struct Cyc_Absyn_Exp*_tmp6B7;struct Cyc_Absyn_Exp*_tmp6B8;switch(*((int*)_tmp6B6)){case 13: _LL405: _tmp6B8=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6B6)->f2;_LL406:
 v=_tmp6B8;goto LOOP2;case 19: _LL407: _tmp6B7=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6B6)->f1;_LL408:
 v=_tmp6B7;goto _LL404;default: _LL409: _LL40A:
 goto _LL404;}_LL404:;}
# 4020
v=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp69F->name,_tmp69E->name)),Cyc_Toc_mt_tq),v);
return Cyc_Absyn_eq_exp(Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0),Cyc_Absyn_var_exp(_tmp69F->name,0),0);}_LL3E8:;}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _dyneither_ptr*f1;};
# 4032
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1};struct _tuple32{int f1;void*f2;};
# 4040
static struct _tuple32 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 4042
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(0;ss != 0;(ss=ss->tl,c=c + 1)){
struct _tuple29 _tmp6BB=*((struct _tuple29*)ss->hd);struct _tuple29 _tmp6BC=_tmp6BB;void*_tmp6BD;_LL40C: _tmp6BD=_tmp6BC.f1;_LL40D:;{
void*_tmp6BE=_tmp6BD;struct Cyc_Absyn_Exp*_tmp6BF;struct _dyneither_ptr*_tmp6C0;switch(*((int*)_tmp6BE)){case 3: _LL40F: _LL410:
# 4048
 goto _LL412;case 4: _LL411: _LL412:
 goto _LL414;case 6: _LL413: _LL414:
# 4051
 continue;case 8: _LL415: _tmp6C0=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp6BE)->f1;_LL416:
# 4053
 if(k == (void*)& Cyc_Toc_OtherTest_val){
struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct _tmpC51;struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_tmpC50;k=(void*)((_tmpC50=_cycalloc(sizeof(*_tmpC50)),((_tmpC50[0]=((_tmpC51.tag=3,((_tmpC51.f1=_tmp6C0,_tmpC51)))),_tmpC50))));}
continue;case 7: _LL417: _LL418:
# 4057
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;
continue;case 0: _LL419: _tmp6BF=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp6BE)->f1;if(_tmp6BF != 0){_LL41A:
# 4061
{struct Cyc_Toc_WhereTest_Toc_TestKind_struct _tmpC54;struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_tmpC53;k=(void*)((_tmpC53=_cycalloc(sizeof(*_tmpC53)),((_tmpC53[0]=((_tmpC54.tag=2,((_tmpC54.f1=_tmp6BF,_tmpC54)))),_tmpC53))));}{
struct _tuple32 _tmpC55;return(_tmpC55.f1=0,((_tmpC55.f2=k,_tmpC55)));};}else{_LL41B: _LL41C:
 goto _LL41E;}case 1: _LL41D: _LL41E:
 goto _LL420;case 2: _LL41F: _LL420:
 goto _LL422;case 5: _LL421: _LL422:
 goto _LL424;default: _LL423: _LL424: {
struct _tuple32 _tmpC56;return(_tmpC56.f1=0,((_tmpC56.f2=k,_tmpC56)));}}_LL40E:;};}{
# 4070
struct _tuple32 _tmpC57;return(_tmpC57.f1=c,((_tmpC57.f2=k,_tmpC57)));};}
# 4075
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{void*_tmp6C8=p;int _tmp6C9;int _tmp6CA;unsigned int _tmp6CB;void*_tmp6CC;struct Cyc_Absyn_Enumfield*_tmp6CD;struct Cyc_Absyn_Enumdecl*_tmp6CE;struct Cyc_Absyn_Enumfield*_tmp6CF;switch(*((int*)_tmp6C8)){case 3: _LL426: _tmp6CE=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp6C8)->f1;_tmp6CF=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp6C8)->f2;_LL427:
# 4079
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmpC5A;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpC59;e=Cyc_Absyn_new_exp((void*)((_tmpC59=_cycalloc(sizeof(*_tmpC59)),((_tmpC59[0]=((_tmpC5A.tag=31,((_tmpC5A.f1=_tmp6CF->name,((_tmpC5A.f2=_tmp6CE,((_tmpC5A.f3=_tmp6CF,_tmpC5A)))))))),_tmpC59)))),0);}goto _LL425;case 4: _LL428: _tmp6CC=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp6C8)->f1;_tmp6CD=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp6C8)->f2;_LL429:
# 4081
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpC5D;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpC5C;e=Cyc_Absyn_new_exp((void*)((_tmpC5C=_cycalloc(sizeof(*_tmpC5C)),((_tmpC5C[0]=((_tmpC5D.tag=32,((_tmpC5D.f1=_tmp6CD->name,((_tmpC5D.f2=_tmp6CC,((_tmpC5D.f3=_tmp6CD,_tmpC5D)))))))),_tmpC5C)))),0);}goto _LL425;case 6: _LL42A: _tmp6CB=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp6C8)->f1;_LL42B:
 _tmp6CA=(int)_tmp6CB;goto _LL42D;case 7: _LL42C: _tmp6CA=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp6C8)->f1;_LL42D:
 _tmp6C9=_tmp6CA;goto _LL42F;case 8: _LL42E: _tmp6C9=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp6C8)->f2;_LL42F:
# 4085
 e=Cyc_Absyn_uint_exp((unsigned int)_tmp6C9,0);
goto _LL425;default: _LL430: _LL431: {
const char*_tmpC60;void*_tmpC5F;(_tmpC5F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC60="compile_pat_test_as_case!",_tag_dyneither(_tmpC60,sizeof(char),26))),_tag_dyneither(_tmpC5F,sizeof(void*),0)));}}_LL425:;}{
# 4089
struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct _tmpC63;struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmpC62;return Cyc_Absyn_new_pat((void*)((_tmpC62=_cycalloc(sizeof(*_tmpC62)),((_tmpC62[0]=((_tmpC63.tag=17,((_tmpC63.f1=e,_tmpC63)))),_tmpC62)))),0);};}
# 4093
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0);}struct _tuple33{struct _tuple1*f1;void*f2;};struct _tuple34{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 4102
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**decls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 4105
void*t=(void*)_check_null(p->topt);
void*_tmp6D8=p->r;void*_tmp6D9=_tmp6D8;union Cyc_Absyn_AggrInfoU _tmp6DA;struct Cyc_List_List*_tmp6DB;struct Cyc_List_List*_tmp6DC;struct Cyc_List_List*_tmp6DD;struct Cyc_Absyn_Pat*_tmp6DE;struct Cyc_Absyn_Datatypedecl*_tmp6DF;struct Cyc_Absyn_Datatypefield*_tmp6E0;struct Cyc_List_List*_tmp6E1;struct Cyc_Absyn_Vardecl*_tmp6E2;struct Cyc_Absyn_Pat*_tmp6E3;struct _tuple1*_tmp6E4;struct _tuple1*_tmp6E5;struct Cyc_Absyn_Pat*_tmp6E6;struct _tuple1*_tmp6E7;switch(*((int*)_tmp6D9)){case 2: _LL433: _tmp6E7=(((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp6D9)->f2)->name;_LL434: {
# 4108
struct Cyc_Absyn_Pat*_tmp6E8=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0);
_tmp6E8->topt=p->topt;
_tmp6E5=_tmp6E7;_tmp6E6=_tmp6E8;goto _LL436;}case 1: _LL435: _tmp6E5=(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp6D9)->f1)->name;_tmp6E6=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp6D9)->f2;_LL436: {
# 4113
struct _tuple1*v=Cyc_Toc_temp_var();
void*_tmp6E9=(void*)_check_null(_tmp6E6->topt);
{struct _tuple33*_tmpC66;struct Cyc_List_List*_tmpC65;*decls=((_tmpC65=_region_malloc(rgn,sizeof(*_tmpC65)),((_tmpC65->hd=((_tmpC66=_region_malloc(rgn,sizeof(*_tmpC66)),((_tmpC66->f1=v,((_tmpC66->f2=Cyc_Toc_typ_to_c_array(_tmp6E9),_tmpC66)))))),((_tmpC65->tl=*decls,_tmpC65))))));}
*nv=Cyc_Toc_add_varmap(rgn,*nv,_tmp6E5,Cyc_Absyn_var_exp(v,0));{
struct Cyc_Absyn_Stmt*_tmp6EC=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(v,0),Cyc_Absyn_copy_exp(path),0);
return Cyc_Toc_seq_stmt_opt(_tmp6EC,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp6E6));};}case 4: _LL437: _tmp6E4=(((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp6D9)->f2)->name;_LL438: {
# 4121
struct _tuple1*_tmp6ED=Cyc_Toc_temp_var();
void*_tmp6EE=(void*)_check_null(p->topt);
{struct _tuple33*_tmpC69;struct Cyc_List_List*_tmpC68;*decls=((_tmpC68=_region_malloc(rgn,sizeof(*_tmpC68)),((_tmpC68->hd=((_tmpC69=_region_malloc(rgn,sizeof(*_tmpC69)),((_tmpC69->f1=_tmp6ED,((_tmpC69->f2=Cyc_Toc_typ_to_c_array(_tmp6EE),_tmpC69)))))),((_tmpC68->tl=*decls,_tmpC68))))));}
*nv=Cyc_Toc_add_varmap(rgn,*nv,_tmp6E4,Cyc_Absyn_var_exp(_tmp6ED,0));
return Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp6ED,0),Cyc_Absyn_copy_exp(path),0);}case 0: _LL439: _LL43A:
 return 0;case 3: _LL43B: _tmp6E2=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp6D9)->f1;_tmp6E3=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp6D9)->f2;_LL43C: {
# 4129
struct _tuple1*_tmp6F1=Cyc_Toc_temp_var();
{struct _tuple33*_tmpC6C;struct Cyc_List_List*_tmpC6B;*decls=((_tmpC6B=_region_malloc(rgn,sizeof(*_tmpC6B)),((_tmpC6B->hd=((_tmpC6C=_region_malloc(rgn,sizeof(*_tmpC6C)),((_tmpC6C->f1=_tmp6F1,((_tmpC6C->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),_tmpC6C)))))),((_tmpC6B->tl=*decls,_tmpC6B))))));}
*nv=Cyc_Toc_add_varmap(rgn,*nv,_tmp6E2->name,Cyc_Absyn_var_exp(_tmp6F1,0));{
# 4133
struct Cyc_Absyn_Stmt*_tmp6F4=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp6F1,0),
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path))),0);
return Cyc_Toc_seq_stmt_opt(_tmp6F4,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp6E3));};}case 9: _LL43D: _LL43E:
# 4138
 goto _LL440;case 10: _LL43F: _LL440:
 goto _LL442;case 11: _LL441: _LL442:
 goto _LL444;case 12: _LL443: _LL444:
 goto _LL446;case 13: _LL445: _LL446:
 goto _LL448;case 14: _LL447: _LL448:
 return 0;case 6: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6D9)->f1)->r)->tag == 8){_LL449: _tmp6DF=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6D9)->f1)->r)->f1;_tmp6E0=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6D9)->f1)->r)->f2;_tmp6E1=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6D9)->f1)->r)->f3;_LL44A:
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
return s;};};}else{_LL453: _tmp6DE=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6D9)->f1;_LL454:
# 4214
 return Cyc_Toc_extract_pattern_vars(rgn,nv,decls,Cyc_Absyn_deref_exp(path,0),_tmp6DE);}case 8: _LL44B: _tmp6DD=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp6D9)->f3;_LL44C:
# 4167
 _tmp6DC=_tmp6DD;goto _LL44E;case 5: _LL44D: _tmp6DC=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp6D9)->f1;_LL44E: {
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
return s;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp6D9)->f1 == 0){_LL44F: _LL450: {
const char*_tmpC6F;void*_tmpC6E;(_tmpC6E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC6F="unresolved aggregate pattern!",_tag_dyneither(_tmpC6F,sizeof(char),30))),_tag_dyneither(_tmpC6E,sizeof(void*),0)));}}else{_LL451: _tmp6DA=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp6D9)->f1)->aggr_info;_tmp6DB=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp6D9)->f3;_LL452: {
# 4183
struct Cyc_Absyn_Aggrdecl*_tmp701=Cyc_Absyn_get_known_aggrdecl(_tmp6DA);
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp6DB != 0;_tmp6DB=_tmp6DB->tl){
struct _tuple34*_tmp702=(struct _tuple34*)_tmp6DB->hd;
struct Cyc_Absyn_Pat*_tmp703=(*_tmp702).f2;
if(_tmp703->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*f;
{void*_tmp704=(void*)((struct Cyc_List_List*)_check_null((*_tmp702).f1))->hd;void*_tmp705=_tmp704;struct _dyneither_ptr*_tmp706;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp705)->tag == 1){_LL45C: _tmp706=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp705)->f1;_LL45D:
 f=_tmp706;goto _LL45B;}else{_LL45E: _LL45F: {
struct Cyc_Toc_Match_error_exn_struct _tmpC72;struct Cyc_Toc_Match_error_exn_struct*_tmpC71;(int)_throw((void*)((_tmpC71=_cycalloc_atomic(sizeof(*_tmpC71)),((_tmpC71[0]=((_tmpC72.tag=Cyc_Toc_Match_error,_tmpC72)),_tmpC71)))));}}_LL45B:;}{
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
return s;}}case 15: _LL455: _LL456: {
# 4216
const char*_tmpC75;void*_tmpC74;(_tmpC74=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC75="unknownid pat",_tag_dyneither(_tmpC75,sizeof(char),14))),_tag_dyneither(_tmpC74,sizeof(void*),0)));}case 16: _LL457: _LL458: {
const char*_tmpC78;void*_tmpC77;(_tmpC77=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC78="unknowncall pat",_tag_dyneither(_tmpC78,sizeof(char),16))),_tag_dyneither(_tmpC77,sizeof(void*),0)));}default: _LL459: _LL45A: {
const char*_tmpC7B;void*_tmpC7A;(_tmpC7A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC7B="exp pat",_tag_dyneither(_tmpC7B,sizeof(char),8))),_tag_dyneither(_tmpC7A,sizeof(void*),0)));}}_LL432:;}struct _tuple35{struct Cyc_Toc_Env*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Stmt*f3;};
# 4227
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 4235
void*_tmp713=dopt;struct Cyc_List_List*_tmp714;struct Cyc_List_List*_tmp715;void*_tmp716;struct Cyc_Tcpat_Rhs*_tmp717;if(_tmp713 == 0){_LL461: _LL462:
# 4237
 return Cyc_Absyn_skip_stmt(0);}else{switch(*((int*)_tmp713)){case 0: _LL463: _LL464:
 return Cyc_Toc_throw_match_stmt();case 1: _LL465: _tmp717=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp713)->f1;_LL466:
# 4242
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple31*_tmp718=(struct _tuple31*)lscs->hd;struct _tuple31*_tmp719=_tmp718;int*_tmp71A;struct _dyneither_ptr*_tmp71B;struct _dyneither_ptr*_tmp71C;struct Cyc_Absyn_Switch_clause*_tmp71D;_LL46A: _tmp71A=(int*)& _tmp719->f1;_tmp71B=_tmp719->f2;_tmp71C=_tmp719->f3;_tmp71D=_tmp719->f4;_LL46B:;{
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
{struct _tuple35*_tmpC7E;struct Cyc_List_List*_tmpC7D;*bodies=((_tmpC7D=_region_malloc(rgn,sizeof(*_tmpC7D)),((_tmpC7D->hd=((_tmpC7E=_region_malloc(rgn,sizeof(*_tmpC7E)),((_tmpC7E->f1=nv,((_tmpC7E->f2=_tmp71C,((_tmpC7E->f3=_tmp71E,_tmpC7E)))))))),((_tmpC7D->tl=*bodies,_tmpC7D))))));}
return res;};}};}{
# 4262
const char*_tmpC81;void*_tmpC80;(_tmpC80=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC81="couldn't find rhs!",_tag_dyneither(_tmpC81,sizeof(char),19))),_tag_dyneither(_tmpC80,sizeof(void*),0)));};default: _LL467: _tmp714=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp713)->f1;_tmp715=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp713)->f2;_tmp716=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp713)->f3;_LL468: {
# 4265
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp716,lscs,v);
# 4267
struct Cyc_Absyn_Exp*_tmp723=Cyc_Toc_compile_path(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp714),Cyc_Absyn_var_exp(v,0));
struct Cyc_List_List*_tmp724=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp715);
# 4270
struct _tuple32 _tmp725=Cyc_Toc_admits_switch(_tmp724);struct _tuple32 _tmp726=_tmp725;int _tmp727;void*_tmp728;_LL46D: _tmp727=_tmp726.f1;_tmp728=_tmp726.f2;_LL46E:;
if(_tmp727 > 1){
# 4274
struct Cyc_Absyn_Switch_clause*_tmpC84;struct Cyc_List_List*_tmpC83;struct Cyc_List_List*new_lscs=
(_tmpC83=_cycalloc(sizeof(*_tmpC83)),((_tmpC83->hd=((_tmpC84=_cycalloc(sizeof(*_tmpC84)),((_tmpC84->pattern=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0),((_tmpC84->pat_vars=0,((_tmpC84->where_clause=0,((_tmpC84->body=res,((_tmpC84->loc=0,_tmpC84)))))))))))),((_tmpC83->tl=0,_tmpC83)))));
# 4277
for(0;_tmp724 != 0;_tmp724=_tmp724->tl){
struct _tuple29 _tmp729=*((struct _tuple29*)_tmp724->hd);struct _tuple29 _tmp72A=_tmp729;void*_tmp72B;void*_tmp72C;_LL470: _tmp72B=_tmp72A.f1;_tmp72C=_tmp72A.f2;_LL471:;{
# 4280
struct Cyc_Absyn_Pat*_tmp72D=Cyc_Toc_compile_pat_test_as_case(_tmp72B);
# 4282
struct Cyc_Absyn_Stmt*_tmp72E=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp72C,lscs,v);
# 4284
struct Cyc_Absyn_Switch_clause*_tmpC87;struct Cyc_List_List*_tmpC86;new_lscs=((_tmpC86=_cycalloc(sizeof(*_tmpC86)),((_tmpC86->hd=((_tmpC87=_cycalloc(sizeof(*_tmpC87)),((_tmpC87->pattern=_tmp72D,((_tmpC87->pat_vars=0,((_tmpC87->where_clause=0,((_tmpC87->body=_tmp72E,((_tmpC87->loc=0,_tmpC87)))))))))))),((_tmpC86->tl=new_lscs,_tmpC86))))));};}
# 4286
{void*_tmp731=_tmp728;struct _dyneither_ptr*_tmp732;switch(*((int*)_tmp731)){case 1: _LL473: _LL474:
# 4289
 LOOP1: {
void*_tmp733=_tmp723->r;void*_tmp734=_tmp733;struct Cyc_Absyn_Exp*_tmp735;struct Cyc_Absyn_Exp*_tmp736;switch(*((int*)_tmp734)){case 13: _LL47C: _tmp736=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp734)->f2;_LL47D:
 _tmp723=_tmp736;goto LOOP1;case 19: _LL47E: _tmp735=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp734)->f1;_LL47F:
 _tmp723=_tmp735;goto _LL47B;default: _LL480: _LL481:
 goto _LL47B;}_LL47B:;}
# 4295
_tmp723=Cyc_Absyn_deref_exp(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,Cyc_Toc_mt_tq),_tmp723),0);goto _LL472;case 3: _LL475: _tmp732=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_tmp731)->f1;_LL476:
# 4298
 _tmp723=Cyc_Toc_member_exp(Cyc_Toc_member_exp(_tmp723,_tmp732,0),Cyc_Toc_tag_sp,0);goto _LL472;case 2: _LL477: _LL478:
# 4300
 goto _LL472;default: _LL479: _LL47A:
 goto _LL472;}_LL472:;}{
# 4303
struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct _tmpC8A;struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmpC89;res=Cyc_Absyn_new_stmt((void*)((_tmpC89=_cycalloc(sizeof(*_tmpC89)),((_tmpC89[0]=((_tmpC8A.tag=10,((_tmpC8A.f1=_tmp723,((_tmpC8A.f2=new_lscs,((_tmpC8A.f3=0,_tmpC8A)))))))),_tmpC89)))),0);};}else{
# 4307
void*_tmp73B=_tmp728;struct Cyc_Absyn_Exp*_tmp73C;if(((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp73B)->tag == 2){_LL483: _tmp73C=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp73B)->f1;_LL484: {
# 4309
struct Cyc_List_List*_tmp73D=_tmp724;void*_tmp73E;struct Cyc_Tcpat_Rhs*_tmp73F;if(_tmp73D != 0){if(((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple29*)((struct Cyc_List_List*)_tmp73D)->hd)->f2)->tag == 1){if(((struct Cyc_List_List*)_tmp73D)->tl == 0){_LL488: _tmp73E=((struct _tuple29*)_tmp73D->hd)->f1;_tmp73F=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple29*)_tmp73D->hd)->f2)->f1;_LL489:
# 4315
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple31*_tmp740=(struct _tuple31*)lscs->hd;struct _tuple31*_tmp741=_tmp740;int*_tmp742;struct _dyneither_ptr*_tmp743;struct _dyneither_ptr*_tmp744;struct Cyc_Absyn_Switch_clause*_tmp745;_LL48D: _tmp742=(int*)& _tmp741->f1;_tmp743=_tmp741->f2;_tmp744=_tmp741->f3;_tmp745=_tmp741->f4;_LL48E:;{
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
{struct _tuple35*_tmpC8D;struct Cyc_List_List*_tmpC8C;*bodies=((_tmpC8C=_region_malloc(rgn,sizeof(*_tmpC8C)),((_tmpC8C->hd=((_tmpC8D=_region_malloc(rgn,sizeof(*_tmpC8D)),((_tmpC8D->f1=nv,((_tmpC8D->f2=_tmp744,((_tmpC8D->f3=_tmp746,_tmpC8D)))))))),((_tmpC8C->tl=*bodies,_tmpC8C))))));}
return r;};};}};}{
# 4339
const char*_tmpC90;void*_tmpC8F;(_tmpC8F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC90="couldn't find rhs!",_tag_dyneither(_tmpC90,sizeof(char),19))),_tag_dyneither(_tmpC8F,sizeof(void*),0)));};}else{goto _LL48A;}}else{goto _LL48A;}}else{_LL48A: _LL48B: {
const char*_tmpC93;void*_tmpC92;(_tmpC92=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC93="bad where clause in match compiler",_tag_dyneither(_tmpC93,sizeof(char),35))),_tag_dyneither(_tmpC92,sizeof(void*),0)));}}_LL487:;}}else{_LL485: _LL486:
# 4344
 for(0;_tmp724 != 0;_tmp724=_tmp724->tl){
struct _tuple29 _tmp74D=*((struct _tuple29*)_tmp724->hd);struct _tuple29 _tmp74E=_tmp74D;void*_tmp74F;void*_tmp750;_LL490: _tmp74F=_tmp74E.f1;_tmp750=_tmp74E.f2;_LL491:;{
struct Cyc_Absyn_Exp*_tmp751=Cyc_Toc_compile_pat_test(_tmp723,_tmp74F);
struct Cyc_Absyn_Stmt*_tmp752=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp750,lscs,v);
res=Cyc_Absyn_ifthenelse_stmt(_tmp751,_tmp752,res,0);};}}_LL482:;}
# 4352
return res;}}}_LL460:;}
# 4362
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 4364
for(0;bodies != 0;bodies=bodies->tl){
struct _tuple35*_tmp753=(struct _tuple35*)bodies->hd;struct _tuple35*_tmp754=_tmp753;struct Cyc_Toc_Env**_tmp755;struct Cyc_Absyn_Stmt*_tmp756;_LL493: _tmp755=(struct Cyc_Toc_Env**)& _tmp754->f1;_tmp756=_tmp754->f3;_LL494:;
if(_tmp756 == s)return _tmp755;}
# 4370
return 0;}
# 4374
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 4377
void*_tmp757=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(nv,e);{
# 4380
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp758=_new_region("rgn");struct _RegionHandle*rgn=& _tmp758;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp759=Cyc_Toc_share_env(rgn,nv);
# 4387
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple31*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 4392
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp759,& mydecls,& mybodies,dopt,lscs,v);
# 4400
{struct Cyc_List_List*_tmp75A=lscs;for(0;_tmp75A != 0;_tmp75A=_tmp75A->tl){
struct _tuple31*_tmp75B=(struct _tuple31*)_tmp75A->hd;struct _tuple31*_tmp75C=_tmp75B;struct _dyneither_ptr*_tmp75D;struct Cyc_Absyn_Switch_clause*_tmp75E;_LL496: _tmp75D=_tmp75C->f3;_tmp75E=_tmp75C->f4;_LL497:;{
struct Cyc_Absyn_Stmt*s=_tmp75E->body;
# 4404
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 4407
if(envp == 0)continue;{
struct Cyc_Toc_Env*_tmp75F=*envp;
# 4410
if(_tmp75A->tl != 0){
struct _tuple31*_tmp760=(struct _tuple31*)((struct Cyc_List_List*)_check_null(_tmp75A->tl))->hd;struct _tuple31*_tmp761=_tmp760;struct _dyneither_ptr*_tmp762;struct Cyc_Absyn_Switch_clause*_tmp763;_LL499: _tmp762=_tmp761->f3;_tmp763=_tmp761->f4;_LL49A:;{
struct Cyc_Toc_Env**_tmp764=Cyc_Toc_find_case_env(mybodies,_tmp763->body);
# 4417
if(_tmp764 == 0)
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn,_tmp75F,end_l),s);else{
# 4421
struct Cyc_List_List*vs=0;
if(_tmp763->pat_vars != 0){
vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp763->pat_vars))->v)).f1);
vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vs);}
# 4426
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(rgn,_tmp75F,end_l,_tmp762,vs,*_tmp764),s);}};}else{
# 4430
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn,_tmp75F,end_l),s);}};};}}{
# 4434
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(test_tree,Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),0),0);
# 4436
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct _tuple33*_tmp765=(struct _tuple33*)((struct Cyc_List_List*)_check_null(mydecls))->hd;struct _tuple33*_tmp766=_tmp765;struct _tuple1*_tmp767;void*_tmp768;_LL49C: _tmp767=_tmp766->f1;_tmp768=_tmp766->f2;_LL49D:;
res=Cyc_Absyn_declare_stmt(_tmp767,_tmp768,0,res,0);}
# 4441
whole_s->r=(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),e,res,0))->r;
_npop_handler(0);return;};}
# 4384
;_pop_region(rgn);};}
# 4447
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
# 4449
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 4454
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 4456
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
struct Cyc_List_List*_tmpC94;return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,(
(_tmpC94=_cycalloc(sizeof(*_tmpC94)),((_tmpC94->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0),((_tmpC94->tl=0,_tmpC94)))))),0),0);}
# 4460
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt(s->r,0));}
# 4465
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 4467
while(1){
void*_tmp76A=s->r;void*_tmp76B=_tmp76A;struct Cyc_Absyn_Exp*_tmp76C;struct Cyc_Absyn_Stmt*_tmp76D;struct Cyc_List_List*_tmp76E;void*_tmp76F;struct Cyc_Absyn_Stmt*_tmp770;struct Cyc_Absyn_Exp*_tmp771;struct _dyneither_ptr*_tmp772;struct Cyc_Absyn_Stmt*_tmp773;struct Cyc_Absyn_Decl*_tmp774;struct Cyc_Absyn_Stmt*_tmp775;struct Cyc_List_List*_tmp776;struct Cyc_Absyn_Switch_clause**_tmp777;struct Cyc_Absyn_Exp*_tmp778;struct Cyc_List_List*_tmp779;void*_tmp77A;struct Cyc_Absyn_Exp*_tmp77B;struct Cyc_Absyn_Exp*_tmp77C;struct Cyc_Absyn_Exp*_tmp77D;struct Cyc_Absyn_Stmt*_tmp77E;struct Cyc_Absyn_Stmt*_tmp77F;struct Cyc_Absyn_Stmt*_tmp780;struct Cyc_Absyn_Stmt*_tmp781;struct Cyc_Absyn_Exp*_tmp782;struct Cyc_Absyn_Stmt*_tmp783;struct Cyc_Absyn_Exp*_tmp784;struct Cyc_Absyn_Stmt*_tmp785;struct Cyc_Absyn_Stmt*_tmp786;struct Cyc_Absyn_Exp*_tmp787;struct Cyc_Absyn_Stmt*_tmp788;struct Cyc_Absyn_Stmt*_tmp789;struct Cyc_Absyn_Exp*_tmp78A;switch(*((int*)_tmp76B)){case 0: _LL49F: _LL4A0:
# 4470
 return;case 1: _LL4A1: _tmp78A=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_LL4A2:
# 4472
 Cyc_Toc_exp_to_c(nv,_tmp78A);
return;case 2: _LL4A3: _tmp788=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_tmp789=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp76B)->f2;_LL4A4:
# 4475
 Cyc_Toc_stmt_to_c(nv,_tmp788);
s=_tmp789;
continue;case 3: _LL4A5: _tmp787=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_LL4A6: {
# 4479
void*topt=0;
if(_tmp787 != 0){
topt=Cyc_Toc_typ_to_c((void*)_check_null(_tmp787->topt));
Cyc_Toc_exp_to_c(nv,_tmp787);}
# 4485
if(s->try_depth > 0){
if(topt != 0){
struct _tuple1*_tmp78B=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp78C=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp78B,0),0);
s->r=(Cyc_Absyn_declare_stmt(_tmp78B,topt,_tmp787,
Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(s->try_depth),_tmp78C,0),0))->r;}else{
# 4493
Cyc_Toc_do_npop_before(s->try_depth,s);}}
# 4495
return;}case 4: _LL4A7: _tmp784=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_tmp785=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp76B)->f2;_tmp786=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp76B)->f3;_LL4A8:
# 4497
 Cyc_Toc_exp_to_c(nv,_tmp784);
Cyc_Toc_stmt_to_c(nv,_tmp785);
s=_tmp786;
continue;case 5: _LL4A9: _tmp782=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1).f1;_tmp783=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp76B)->f2;_LL4AA:
# 4502
 Cyc_Toc_exp_to_c(nv,_tmp782);{
struct _RegionHandle _tmp78D=_new_region("temp");struct _RegionHandle*temp=& _tmp78D;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp783);
# 4506
_npop_handler(0);return;
# 4503
;_pop_region(temp);};case 6: _LL4AB: _tmp781=((struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_LL4AC: {
# 4508
struct Cyc_Toc_Env*_tmp78E=nv;struct _dyneither_ptr**_tmp78F;_LL4C2: _tmp78F=_tmp78E->break_lab;_LL4C3:;
if(_tmp78F != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp78F,0);{
# 4512
int dest_depth=_tmp781 == 0?0: _tmp781->try_depth;
Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);
return;};}case 7: _LL4AD: _tmp780=((struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_LL4AE: {
# 4516
struct Cyc_Toc_Env*_tmp790=nv;struct _dyneither_ptr**_tmp791;_LL4C5: _tmp791=_tmp790->continue_lab;_LL4C6:;
if(_tmp791 != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp791,0);
_tmp77F=_tmp780;goto _LL4B0;}case 8: _LL4AF: _tmp77F=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp76B)->f2;_LL4B0:
# 4522
 Cyc_Toc_do_npop_before(s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp77F))->try_depth,s);
return;case 9: _LL4B1: _tmp77B=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_tmp77C=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp76B)->f2).f1;_tmp77D=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp76B)->f3).f1;_tmp77E=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp76B)->f4;_LL4B2:
# 4526
 Cyc_Toc_exp_to_c(nv,_tmp77B);Cyc_Toc_exp_to_c(nv,_tmp77C);Cyc_Toc_exp_to_c(nv,_tmp77D);{
struct _RegionHandle _tmp792=_new_region("temp");struct _RegionHandle*temp=& _tmp792;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp77E);
# 4530
_npop_handler(0);return;
# 4527
;_pop_region(temp);};case 10: _LL4B3: _tmp778=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_tmp779=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp76B)->f2;_tmp77A=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp76B)->f3;_LL4B4:
# 4532
 Cyc_Toc_xlate_switch(nv,s,_tmp778,_tmp779,_tmp77A);
return;case 11: _LL4B5: _tmp776=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_tmp777=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp76B)->f2;_LL4B6: {
# 4535
struct Cyc_Toc_Env*_tmp793=nv;struct Cyc_Toc_FallthruInfo*_tmp794;_LL4C8: _tmp794=_tmp793->fallthru_info;_LL4C9:;
if(_tmp794 == 0){
const char*_tmpC97;void*_tmpC96;(_tmpC96=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC97="fallthru in unexpected place",_tag_dyneither(_tmpC97,sizeof(char),29))),_tag_dyneither(_tmpC96,sizeof(void*),0)));}{
struct Cyc_Toc_FallthruInfo _tmp797=*_tmp794;struct Cyc_Toc_FallthruInfo _tmp798=_tmp797;struct _dyneither_ptr*_tmp799;struct Cyc_List_List*_tmp79A;struct Cyc_Dict_Dict _tmp79B;_LL4CB: _tmp799=_tmp798.label;_tmp79A=_tmp798.binders;_tmp79B=_tmp798.next_case_env;_LL4CC:;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp799,0);
# 4541
Cyc_Toc_do_npop_before(s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp777)))->body)->try_depth,s2);{
struct Cyc_List_List*_tmp79C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp79A);
struct Cyc_List_List*_tmp79D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp776);
for(0;_tmp79C != 0;(_tmp79C=_tmp79C->tl,_tmp79D=_tmp79D->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp79D))->hd);
s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp79B,(struct _tuple1*)_tmp79C->hd),(struct Cyc_Absyn_Exp*)_tmp79D->hd,0),s2,0);}
# 4549
s->r=s2->r;
return;};};};}case 12: _LL4B7: _tmp774=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_tmp775=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp76B)->f2;_LL4B8:
# 4555
{void*_tmp79E=_tmp774->r;void*_tmp79F=_tmp79E;struct Cyc_Absyn_Tvar*_tmp7A0;struct Cyc_Absyn_Vardecl*_tmp7A1;int _tmp7A2;struct Cyc_Absyn_Exp*_tmp7A3;struct Cyc_Absyn_Fndecl*_tmp7A4;struct Cyc_List_List*_tmp7A5;struct Cyc_Absyn_Pat*_tmp7A6;struct Cyc_Absyn_Exp*_tmp7A7;void*_tmp7A8;struct Cyc_Absyn_Vardecl*_tmp7A9;switch(*((int*)_tmp79F)){case 0: _LL4CE: _tmp7A9=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp79F)->f1;_LL4CF: {
# 4557
struct _RegionHandle _tmp7AA=_new_region("temp");struct _RegionHandle*temp=& _tmp7AA;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpC9A;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpC99;struct Cyc_Toc_Env*_tmp7AB=Cyc_Toc_add_varmap(temp,nv,_tmp7A9->name,
Cyc_Absyn_varb_exp(_tmp7A9->name,(void*)((_tmpC99=_cycalloc(sizeof(*_tmpC99)),((_tmpC99[0]=((_tmpC9A.tag=4,((_tmpC9A.f1=_tmp7A9,_tmpC9A)))),_tmpC99)))),0));
Cyc_Toc_local_decl_to_c(_tmp7AB,_tmp7AB,_tmp7A9,_tmp775);}
# 4562
_npop_handler(0);goto _LL4CD;
# 4557
;_pop_region(temp);}case 2: _LL4D0: _tmp7A6=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp79F)->f1;_tmp7A7=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp79F)->f3;_tmp7A8=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp79F)->f4;_LL4D1:
# 4566
{void*_tmp7AE=_tmp7A6->r;void*_tmp7AF=_tmp7AE;struct Cyc_Absyn_Vardecl*_tmp7B0;if(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp7AF)->tag == 1){if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp7AF)->f2)->r)->tag == 0){_LL4DB: _tmp7B0=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp7AF)->f1;_LL4DC: {
# 4568
struct _tuple1*old_name=_tmp7B0->name;
struct _tuple1*new_name=Cyc_Toc_temp_var();
_tmp7B0->name=new_name;
_tmp7B0->initializer=_tmp7A7;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpC9D;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpC9C;_tmp774->r=(void*)((_tmpC9C=_cycalloc(sizeof(*_tmpC9C)),((_tmpC9C[0]=((_tmpC9D.tag=0,((_tmpC9D.f1=_tmp7B0,_tmpC9D)))),_tmpC9C))));}{
struct _RegionHandle _tmp7B3=_new_region("temp");struct _RegionHandle*temp=& _tmp7B3;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpCA0;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpC9F;struct Cyc_Toc_Env*_tmp7B4=
Cyc_Toc_add_varmap(temp,nv,old_name,
Cyc_Absyn_varb_exp(new_name,(void*)((_tmpC9F=_cycalloc(sizeof(*_tmpC9F)),((_tmpC9F[0]=((_tmpCA0.tag=4,((_tmpCA0.f1=_tmp7B0,_tmpCA0)))),_tmpC9F)))),0));
Cyc_Toc_local_decl_to_c(_tmp7B4,nv,_tmp7B0,_tmp775);}
# 4579
_npop_handler(0);goto _LL4DA;
# 4573
;_pop_region(temp);};}}else{goto _LL4DD;}}else{_LL4DD: _LL4DE:
# 4584
 s->r=(Cyc_Toc_letdecl_to_c(nv,_tmp7A6,_tmp7A8,(void*)_check_null(_tmp7A7->topt),_tmp7A7,_tmp775))->r;
goto _LL4DA;}_LL4DA:;}
# 4587
goto _LL4CD;case 3: _LL4D2: _tmp7A5=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp79F)->f1;_LL4D3: {
# 4600 "toc.cyc"
struct Cyc_List_List*_tmp7B7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7A5);
if(_tmp7B7 == 0){
const char*_tmpCA3;void*_tmpCA2;(_tmpCA2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCA3="empty Letv_d",_tag_dyneither(_tmpCA3,sizeof(char),13))),_tag_dyneither(_tmpCA2,sizeof(void*),0)));}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpCA6;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpCA5;_tmp774->r=(void*)((_tmpCA5=_cycalloc(sizeof(*_tmpCA5)),((_tmpCA5[0]=((_tmpCA6.tag=0,((_tmpCA6.f1=(struct Cyc_Absyn_Vardecl*)_tmp7B7->hd,_tmpCA6)))),_tmpCA5))));}
_tmp7B7=_tmp7B7->tl;
for(0;_tmp7B7 != 0;_tmp7B7=_tmp7B7->tl){
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpCA9;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpCA8;struct Cyc_Absyn_Decl*_tmp7BC=Cyc_Absyn_new_decl((void*)((_tmpCA8=_cycalloc(sizeof(*_tmpCA8)),((_tmpCA8[0]=((_tmpCA9.tag=0,((_tmpCA9.f1=(struct Cyc_Absyn_Vardecl*)_tmp7B7->hd,_tmpCA9)))),_tmpCA8)))),0);
s->r=(Cyc_Absyn_decl_stmt(_tmp7BC,Cyc_Absyn_new_stmt(s->r,0),0))->r;}
# 4609
Cyc_Toc_stmt_to_c(nv,s);
goto _LL4CD;}case 1: _LL4D4: _tmp7A4=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp79F)->f1;_LL4D5: {
# 4612
struct _tuple1*_tmp7BF=_tmp7A4->name;
struct _RegionHandle _tmp7C0=_new_region("temp");struct _RegionHandle*temp=& _tmp7C0;_push_region(temp);{
struct Cyc_Toc_Env*_tmp7C1=Cyc_Toc_add_varmap(temp,nv,_tmp7A4->name,Cyc_Absyn_var_exp(_tmp7BF,0));
Cyc_Toc_fndecl_to_c(_tmp7C1,_tmp7A4,0);
Cyc_Toc_stmt_to_c(_tmp7C1,_tmp775);}
# 4618
_npop_handler(0);goto _LL4CD;
# 4613
;_pop_region(temp);}case 4: _LL4D6: _tmp7A0=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp79F)->f1;_tmp7A1=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp79F)->f2;_tmp7A2=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp79F)->f3;_tmp7A3=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp79F)->f4;_LL4D7: {
# 4620
struct Cyc_Absyn_Stmt*_tmp7C2=_tmp775;
# 4628
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple1*rh_var=Cyc_Toc_temp_var();
struct _tuple1*x_var=_tmp7A1->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);
# 4635
struct _RegionHandle _tmp7C3=_new_region("temp");struct _RegionHandle*temp=& _tmp7C3;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(temp,nv,x_var,x_exp),_tmp7C2);
# 4638
if(Cyc_Absyn_no_regions)
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_uint_exp(0,0),_tmp7C2,0))->r;else{
if((unsigned int)_tmp7A3){
Cyc_Toc_exp_to_c(nv,_tmp7A3);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(Cyc_Absyn_aggrarrow_exp(_tmp7A3,_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"h",sizeof(char),2),0),0);
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,_tmp7C2,0))->r;};}else{
# 4646
struct Cyc_Absyn_Exp*_tmpCAC[1];struct Cyc_Absyn_Exp*_tmpCAB[1];struct Cyc_List_List*_tmpCAA;s->r=(
Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,(
(_tmpCAA=_cycalloc(sizeof(*_tmpCAA)),((_tmpCAA->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0),((_tmpCAA->tl=0,_tmpCAA)))))),0),
# 4651
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpCAB[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCAB,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),
Cyc_Absyn_seq_stmt(_tmp7C2,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpCAC[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCAC,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}}
# 4657
_npop_handler(0);return;
# 4635
;_pop_region(temp);}default: _LL4D8: _LL4D9: {
# 4658
const char*_tmpCAF;void*_tmpCAE;(_tmpCAE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCAF="bad nested declaration within function",_tag_dyneither(_tmpCAF,sizeof(char),39))),_tag_dyneither(_tmpCAE,sizeof(void*),0)));}}_LL4CD:;}
# 4660
return;case 13: _LL4B9: _tmp772=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_tmp773=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp76B)->f2;_LL4BA:
# 4662
 s=_tmp773;continue;case 14: _LL4BB: _tmp770=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_tmp771=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp76B)->f2).f1;_LL4BC: {
# 4664
struct _RegionHandle _tmp7CA=_new_region("temp");struct _RegionHandle*temp=& _tmp7CA;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp770);
Cyc_Toc_exp_to_c(nv,_tmp771);
# 4668
_npop_handler(0);return;
# 4664
;_pop_region(temp);}case 15: _LL4BD: _tmp76D=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_tmp76E=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp76B)->f2;_tmp76F=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp76B)->f3;_LL4BE: {
# 4682 "toc.cyc"
struct _tuple1*h_var=Cyc_Toc_temp_var();
struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
# 4693
e_exp->topt=e_typ;{
struct _RegionHandle _tmp7CB=_new_region("temp");struct _RegionHandle*temp=& _tmp7CB;_push_region(temp);{
struct Cyc_Toc_Env*_tmp7CC=Cyc_Toc_add_varmap(temp,nv,e_var,e_exp);
# 4697
Cyc_Toc_stmt_to_c(_tmp7CC,_tmp76D);{
struct Cyc_Absyn_Stmt*_tmp7CD=Cyc_Absyn_seq_stmt(_tmp76D,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
# 4702
struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct _tmpCB2;struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmpCB1;struct Cyc_Absyn_Stmt*_tmp7CE=Cyc_Absyn_new_stmt((void*)((_tmpCB1=_cycalloc(sizeof(*_tmpCB1)),((_tmpCB1[0]=((_tmpCB2.tag=10,((_tmpCB2.f1=e_exp,((_tmpCB2.f2=_tmp76E,((_tmpCB2.f3=_tmp76F,_tmpCB2)))))))),_tmpCB1)))),0);
# 4704
Cyc_Toc_stmt_to_c(_tmp7CC,_tmp7CE);{
# 4707
struct Cyc_List_List*_tmpCB3;struct Cyc_Absyn_Exp*_tmp7CF=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,(
(_tmpCB3=_cycalloc(sizeof(*_tmpCB3)),((_tmpCB3->hd=Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0),((_tmpCB3->tl=0,_tmpCB3)))))),0);
# 4711
struct Cyc_List_List*_tmpCB4;struct Cyc_Absyn_Stmt*_tmp7D0=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,(
(_tmpCB4=_cycalloc(sizeof(*_tmpCB4)),((_tmpCB4->hd=Cyc_Absyn_address_exp(h_exp,0),((_tmpCB4->tl=0,_tmpCB4)))))),0),0);
# 4715
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
# 4726
Cyc_Absyn_declare_stmt(e_var,e_typ,
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp7CE,0),0),0),0),0),0))->r;};};}
# 4730
_npop_handler(0);return;
# 4694
;_pop_region(temp);};}default: _LL4BF: _tmp76C=((struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp76B)->f1;_LL4C0:
# 4732
 if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{
# 4735
Cyc_Toc_exp_to_c(nv,_tmp76C);{
struct Cyc_List_List*_tmpCB5;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((_tmpCB5=_cycalloc(sizeof(*_tmpCB5)),((_tmpCB5->hd=_tmp76C,((_tmpCB5->tl=0,_tmpCB5)))))),0));};}
# 4738
return;}_LL49E:;}}
# 4747
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
struct _tuple1*_tmpCB6;struct _tuple1*_tmp7DB=(_tmpCB6=_cycalloc(sizeof(*_tmpCB6)),((_tmpCB6->f1=Cyc_Absyn_Loc_n,((_tmpCB6->f2=(*((struct _tuple9*)_tmp7DA->hd)).f1,_tmpCB6)))));
(*((struct _tuple9*)_tmp7DA->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp7DA->hd)).f3);
_tmp7D9=Cyc_Toc_add_varmap(frgn,_tmp7D9,_tmp7DB,Cyc_Absyn_var_exp(_tmp7DB,0));}}
# 4764
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}
# 4768
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp7DD=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp7DE=_tmp7DD;struct _dyneither_ptr*_tmp7DF;struct Cyc_Absyn_Tqual _tmp7E0;void*_tmp7E1;int _tmp7E2;_LL4E0: _tmp7DF=_tmp7DE.name;_tmp7E0=_tmp7DE.tq;_tmp7E1=_tmp7DE.type;_tmp7E2=_tmp7DE.inject;_LL4E1:;{
void*_tmp7E3=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp7E1,(void*)& Cyc_Absyn_HeapRgn_val,_tmp7E0,Cyc_Absyn_false_conref));
struct _tuple1*_tmpCB7;struct _tuple1*_tmp7E4=(_tmpCB7=_cycalloc(sizeof(*_tmpCB7)),((_tmpCB7->f1=Cyc_Absyn_Loc_n,((_tmpCB7->f2=(struct _dyneither_ptr*)_check_null(_tmp7DF),_tmpCB7)))));
{struct _tuple9*_tmpCBA;struct Cyc_List_List*_tmpCB9;f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpCB9=_cycalloc(sizeof(*_tmpCB9)),((_tmpCB9->hd=((_tmpCBA=_cycalloc(sizeof(*_tmpCBA)),((_tmpCBA->f1=_tmp7DF,((_tmpCBA->f2=_tmp7E0,((_tmpCBA->f3=_tmp7E3,_tmpCBA)))))))),((_tmpCB9->tl=0,_tmpCB9)))))));}
_tmp7D9=Cyc_Toc_add_varmap(frgn,_tmp7D9,_tmp7E4,Cyc_Absyn_var_exp(_tmp7E4,0));
f->cyc_varargs=0;};}
# 4777
{struct Cyc_List_List*_tmp7E8=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp7E8 != 0;_tmp7E8=_tmp7E8->tl){
((struct Cyc_Absyn_Vardecl*)_tmp7E8->hd)->type=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp7E8->hd)->type);}}
# 4780
Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp7D9),f->body);}
# 4756
;_pop_region(frgn);};}
# 4785
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
enum Cyc_Absyn_Scope _tmp7E9=s;switch(_tmp7E9){case Cyc_Absyn_Abstract: _LL4E3: _LL4E4:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL4E5: _LL4E6:
 return Cyc_Absyn_Extern;default: _LL4E7: _LL4E8:
 return s;}_LL4E2:;}struct _tuple36{struct Cyc_Absyn_Aggrdecl*f1;int f2;};struct _tuple37{struct Cyc_Toc_TocState*f1;struct _tuple36*f2;};
# 4802 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple37*env){
# 4805
struct _tuple37 _tmp7EA=*env;struct _tuple37 _tmp7EB=_tmp7EA;struct Cyc_Toc_TocState*_tmp7EC;struct Cyc_Absyn_Aggrdecl*_tmp7ED;int _tmp7EE;_LL4EA: _tmp7EC=_tmp7EB.f1;_tmp7ED=(_tmp7EB.f2)->f1;_tmp7EE=(_tmp7EB.f2)->f2;_LL4EB:;{
struct _tuple1*_tmp7EF=_tmp7ED->name;
struct Cyc_Toc_TocState _tmp7F0=*_tmp7EC;struct Cyc_Toc_TocState _tmp7F1=_tmp7F0;struct Cyc_Dict_Dict*_tmp7F2;_LL4ED: _tmp7F2=_tmp7F1.aggrs_so_far;_LL4EE:;{
int seen_defn_before;
struct _tuple18**_tmp7F3=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp7F2,_tmp7EF);
if(_tmp7F3 == 0){
seen_defn_before=0;{
struct _tuple18*v;
if(_tmp7ED->kind == Cyc_Absyn_StructA){
struct _tuple18*_tmpCBB;v=((_tmpCBB=_region_malloc(d,sizeof(*_tmpCBB)),((_tmpCBB->f1=_tmp7ED,((_tmpCBB->f2=Cyc_Absyn_strctq(_tmp7EF),_tmpCBB))))));}else{
# 4816
struct _tuple18*_tmpCBC;v=((_tmpCBC=_region_malloc(d,sizeof(*_tmpCBC)),((_tmpCBC->f1=_tmp7ED,((_tmpCBC->f2=Cyc_Absyn_unionq_typ(_tmp7EF),_tmpCBC))))));}
*_tmp7F2=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp7F2,_tmp7EF,v);};}else{
# 4819
struct _tuple18*_tmp7F6=*_tmp7F3;struct _tuple18*_tmp7F7=_tmp7F6;struct Cyc_Absyn_Aggrdecl*_tmp7F8;void*_tmp7F9;_LL4F0: _tmp7F8=_tmp7F7->f1;_tmp7F9=_tmp7F7->f2;_LL4F1:;
if(_tmp7F8->impl == 0){
{struct _tuple18*_tmpCBD;*_tmp7F2=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp7F2,_tmp7EF,((_tmpCBD=_region_malloc(d,sizeof(*_tmpCBD)),((_tmpCBD->f1=_tmp7ED,((_tmpCBD->f2=_tmp7F9,_tmpCBD)))))));}
seen_defn_before=0;}else{
# 4824
seen_defn_before=1;}}{
# 4826
struct Cyc_Absyn_Aggrdecl*_tmpCBE;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpCBE=_cycalloc(sizeof(*_tmpCBE)),((_tmpCBE->kind=_tmp7ED->kind,((_tmpCBE->sc=Cyc_Absyn_Public,((_tmpCBE->name=_tmp7ED->name,((_tmpCBE->tvs=0,((_tmpCBE->impl=0,((_tmpCBE->expected_mem_kind=0,((_tmpCBE->attributes=_tmp7ED->attributes,_tmpCBE)))))))))))))));
# 4833
if(_tmp7ED->impl != 0  && !seen_defn_before){
{struct Cyc_Absyn_AggrdeclImpl*_tmpCBF;new_ad->impl=((_tmpCBF=_cycalloc(sizeof(*_tmpCBF)),((_tmpCBF->exist_vars=0,((_tmpCBF->rgn_po=0,((_tmpCBF->fields=0,((_tmpCBF->tagged=0,_tmpCBF))))))))));}{
# 4838
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp7FC=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7ED->impl))->fields;for(0;_tmp7FC != 0;_tmp7FC=_tmp7FC->tl){
# 4842
struct Cyc_Absyn_Aggrfield*_tmp7FD=(struct Cyc_Absyn_Aggrfield*)_tmp7FC->hd;
void*_tmp7FE=_tmp7FD->type;
struct Cyc_List_List*_tmp7FF=_tmp7FD->attributes;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp7FE)) && (
_tmp7ED->kind == Cyc_Absyn_StructA  && _tmp7FC->tl == 0  || _tmp7ED->kind == Cyc_Absyn_UnionA)){
# 4856 "toc.cyc"
void*_tmp800=Cyc_Tcutil_compress(_tmp7FE);void*_tmp801=_tmp800;void*_tmp802;struct Cyc_Absyn_Tqual _tmp803;union Cyc_Absyn_Constraint*_tmp804;unsigned int _tmp805;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp801)->tag == 8){_LL4F3: _tmp802=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp801)->f1).elt_type;_tmp803=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp801)->f1).tq;_tmp804=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp801)->f1).zero_term;_tmp805=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp801)->f1).zt_loc;_LL4F4:
# 4859
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpCC5;struct Cyc_Absyn_ArrayInfo _tmpCC4;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCC3;_tmp7FE=(void*)((_tmpCC3=_cycalloc(sizeof(*_tmpCC3)),((_tmpCC3[0]=((_tmpCC5.tag=8,((_tmpCC5.f1=((_tmpCC4.elt_type=_tmp802,((_tmpCC4.tq=_tmp803,((_tmpCC4.num_elts=Cyc_Absyn_uint_exp(0,0),((_tmpCC4.zero_term=_tmp804,((_tmpCC4.zt_loc=_tmp805,_tmpCC4)))))))))),_tmpCC5)))),_tmpCC3))));}
goto _LL4F2;}else{_LL4F5: _LL4F6:
# 4862
{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpCCB;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpCCA;struct Cyc_List_List*_tmpCC9;_tmp7FF=((_tmpCC9=_cycalloc(sizeof(*_tmpCC9)),((_tmpCC9->hd=(void*)((_tmpCCB=_cycalloc(sizeof(*_tmpCCB)),((_tmpCCB[0]=((_tmpCCA.tag=6,((_tmpCCA.f1=0,_tmpCCA)))),_tmpCCB)))),((_tmpCC9->tl=_tmp7FF,_tmpCC9))))));}{
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpCD1;struct Cyc_Absyn_ArrayInfo _tmpCD0;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCCF;_tmp7FE=(void*)((_tmpCCF=_cycalloc(sizeof(*_tmpCCF)),((_tmpCCF[0]=((_tmpCD1.tag=8,((_tmpCD1.f1=((_tmpCD0.elt_type=Cyc_Absyn_void_star_typ(),((_tmpCD0.tq=
Cyc_Absyn_empty_tqual(0),((_tmpCD0.num_elts=
Cyc_Absyn_uint_exp(0,0),((_tmpCD0.zero_term=
((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0),((_tmpCD0.zt_loc=0,_tmpCD0)))))))))),_tmpCD1)))),_tmpCCF))));};}_LL4F2:;}{
# 4869
struct Cyc_Absyn_Aggrfield*_tmpCD2;struct Cyc_Absyn_Aggrfield*_tmp80F=(_tmpCD2=_cycalloc(sizeof(*_tmpCD2)),((_tmpCD2->name=_tmp7FD->name,((_tmpCD2->tq=Cyc_Toc_mt_tq,((_tmpCD2->type=
# 4871
Cyc_Toc_typ_to_c(_tmp7FE),((_tmpCD2->width=_tmp7FD->width,((_tmpCD2->attributes=_tmp7FF,((_tmpCD2->requires_clause=0,_tmpCD2)))))))))))));
# 4879
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7ED->impl))->tagged){
void*_tmp810=_tmp80F->type;
struct _dyneither_ptr*_tmp811=_tmp80F->name;
const char*_tmpCD7;void*_tmpCD6[2];struct Cyc_String_pa_PrintArg_struct _tmpCD5;struct Cyc_String_pa_PrintArg_struct _tmpCD4;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpCD4.tag=0,((_tmpCD4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp811),((_tmpCD5.tag=0,((_tmpCD5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp7ED->name).f2),((_tmpCD6[0]=& _tmpCD5,((_tmpCD6[1]=& _tmpCD4,Cyc_aprintf(((_tmpCD7="_union_%s_%s",_tag_dyneither(_tmpCD7,sizeof(char),13))),_tag_dyneither(_tmpCD6,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpCD8;struct _dyneither_ptr*str=(_tmpCD8=_cycalloc(sizeof(*_tmpCD8)),((_tmpCD8[0]=s,_tmpCD8)));
struct Cyc_Absyn_Aggrfield*_tmpCD9;struct Cyc_Absyn_Aggrfield*_tmp812=(_tmpCD9=_cycalloc(sizeof(*_tmpCD9)),((_tmpCD9->name=Cyc_Toc_val_sp,((_tmpCD9->tq=Cyc_Toc_mt_tq,((_tmpCD9->type=_tmp810,((_tmpCD9->width=0,((_tmpCD9->attributes=0,((_tmpCD9->requires_clause=0,_tmpCD9)))))))))))));
struct Cyc_Absyn_Aggrfield*_tmpCDA;struct Cyc_Absyn_Aggrfield*_tmp813=(_tmpCDA=_cycalloc(sizeof(*_tmpCDA)),((_tmpCDA->name=Cyc_Toc_tag_sp,((_tmpCDA->tq=Cyc_Toc_mt_tq,((_tmpCDA->type=Cyc_Absyn_sint_typ,((_tmpCDA->width=0,((_tmpCDA->attributes=0,((_tmpCDA->requires_clause=0,_tmpCDA)))))))))))));
struct Cyc_Absyn_Aggrfield*_tmpCDB[2];struct Cyc_List_List*_tmp814=(_tmpCDB[1]=_tmp812,((_tmpCDB[0]=_tmp813,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCDB,sizeof(struct Cyc_Absyn_Aggrfield*),2)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpCE0;struct _tuple1*_tmpCDF;struct Cyc_Absyn_Aggrdecl*_tmpCDE;struct Cyc_Absyn_Aggrdecl*_tmp815=(_tmpCDE=_cycalloc(sizeof(*_tmpCDE)),((_tmpCDE->kind=Cyc_Absyn_StructA,((_tmpCDE->sc=Cyc_Absyn_Public,((_tmpCDE->name=(
(_tmpCDF=_cycalloc(sizeof(*_tmpCDF)),((_tmpCDF->f1=Cyc_Absyn_Loc_n,((_tmpCDF->f2=str,_tmpCDF)))))),((_tmpCDE->tvs=0,((_tmpCDE->impl=(
(_tmpCE0=_cycalloc(sizeof(*_tmpCE0)),((_tmpCE0->exist_vars=0,((_tmpCE0->rgn_po=0,((_tmpCE0->fields=_tmp814,((_tmpCE0->tagged=0,_tmpCE0)))))))))),((_tmpCDE->expected_mem_kind=0,((_tmpCDE->attributes=0,_tmpCDE)))))))))))))));
# 4892
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpCE6;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpCE5;struct Cyc_List_List*_tmpCE4;Cyc_Toc_result_decls=((_tmpCE4=_cycalloc(sizeof(*_tmpCE4)),((_tmpCE4->hd=Cyc_Absyn_new_decl((void*)((_tmpCE6=_cycalloc(sizeof(*_tmpCE6)),((_tmpCE6[0]=((_tmpCE5.tag=5,((_tmpCE5.f1=_tmp815,_tmpCE5)))),_tmpCE6)))),0),((_tmpCE4->tl=Cyc_Toc_result_decls,_tmpCE4))))));}
_tmp80F->type=Cyc_Absyn_strct(str);}{
# 4895
struct Cyc_List_List*_tmpCE7;new_fields=((_tmpCE7=_cycalloc(sizeof(*_tmpCE7)),((_tmpCE7->hd=_tmp80F,((_tmpCE7->tl=new_fields,_tmpCE7))))));};};}}
# 4897
(new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);};}
# 4899
if(_tmp7EE){
struct Cyc_Absyn_Decl*_tmpCF1;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpCF0;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpCEF;struct Cyc_List_List*_tmpCEE;Cyc_Toc_result_decls=((_tmpCEE=_cycalloc(sizeof(*_tmpCEE)),((_tmpCEE->hd=((_tmpCF1=_cycalloc(sizeof(*_tmpCF1)),((_tmpCF1->r=(void*)((_tmpCEF=_cycalloc(sizeof(*_tmpCEF)),((_tmpCEF[0]=((_tmpCF0.tag=5,((_tmpCF0.f1=new_ad,_tmpCF0)))),_tmpCEF)))),((_tmpCF1->loc=0,_tmpCF1)))))),((_tmpCEE->tl=Cyc_Toc_result_decls,_tmpCEE))))));}
return 0;};};};}
# 4904
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple36 _tmpCF2;struct _tuple36 p=(_tmpCF2.f1=ad,((_tmpCF2.f2=add_to_decls,_tmpCF2)));
((int(*)(struct _tuple36*arg,int(*f)(struct _RegionHandle*,struct _tuple37*)))Cyc_Toc_use_toc_state)(& p,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple38{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 4933 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple38*env){
# 4936
struct _tuple38 _tmp82C=*env;struct _tuple38 _tmp82D=_tmp82C;struct Cyc_Set_Set**_tmp82E;struct Cyc_Absyn_Datatypedecl*_tmp82F;_LL4F8: _tmp82E=(_tmp82D.f1)->datatypes_so_far;_tmp82F=_tmp82D.f2;_LL4F9:;{
struct _tuple1*_tmp830=_tmp82F->name;
if(_tmp82F->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*_tmp82E,_tmp830))
return 0;
*_tmp82E=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*_tmp82E,_tmp830);
{struct Cyc_List_List*_tmp831=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp82F->fields))->v;for(0;_tmp831 != 0;_tmp831=_tmp831->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp831->hd;
# 4944
struct Cyc_List_List*_tmp832=0;
int i=1;
{struct Cyc_List_List*_tmp833=f->typs;for(0;_tmp833 != 0;(_tmp833=_tmp833->tl,i ++)){
struct _dyneither_ptr*_tmp834=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmpCF3;struct Cyc_Absyn_Aggrfield*_tmp835=(_tmpCF3=_cycalloc(sizeof(*_tmpCF3)),((_tmpCF3->name=_tmp834,((_tmpCF3->tq=(*((struct _tuple11*)_tmp833->hd)).f1,((_tmpCF3->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp833->hd)).f2),((_tmpCF3->width=0,((_tmpCF3->attributes=0,((_tmpCF3->requires_clause=0,_tmpCF3)))))))))))));
struct Cyc_List_List*_tmpCF4;_tmp832=((_tmpCF4=_cycalloc(sizeof(*_tmpCF4)),((_tmpCF4->hd=_tmp835,((_tmpCF4->tl=_tmp832,_tmpCF4))))));}}
# 4952
{struct Cyc_Absyn_Aggrfield*_tmpCF7;struct Cyc_List_List*_tmpCF6;_tmp832=((_tmpCF6=_cycalloc(sizeof(*_tmpCF6)),((_tmpCF6->hd=((_tmpCF7=_cycalloc(sizeof(*_tmpCF7)),((_tmpCF7->name=Cyc_Toc_tag_sp,((_tmpCF7->tq=Cyc_Toc_mt_tq,((_tmpCF7->type=Cyc_Absyn_sint_typ,((_tmpCF7->width=0,((_tmpCF7->attributes=0,((_tmpCF7->requires_clause=0,_tmpCF7)))))))))))))),((_tmpCF6->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp832),_tmpCF6))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpCFA;struct Cyc_Absyn_Aggrdecl*_tmpCF9;struct Cyc_Absyn_Aggrdecl*_tmp83A=
(_tmpCF9=_cycalloc(sizeof(*_tmpCF9)),((_tmpCF9->kind=Cyc_Absyn_StructA,((_tmpCF9->sc=Cyc_Absyn_Public,((_tmpCF9->name=
Cyc_Toc_collapse_qvars(f->name,_tmp82F->name),((_tmpCF9->tvs=0,((_tmpCF9->expected_mem_kind=0,((_tmpCF9->impl=(
# 4958
(_tmpCFA=_cycalloc(sizeof(*_tmpCFA)),((_tmpCFA->exist_vars=0,((_tmpCFA->rgn_po=0,((_tmpCFA->fields=_tmp832,((_tmpCFA->tagged=0,_tmpCFA)))))))))),((_tmpCF9->attributes=0,_tmpCF9)))))))))))))));
# 4960
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD00;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpCFF;struct Cyc_List_List*_tmpCFE;Cyc_Toc_result_decls=((_tmpCFE=_cycalloc(sizeof(*_tmpCFE)),((_tmpCFE->hd=Cyc_Absyn_new_decl((void*)((_tmpD00=_cycalloc(sizeof(*_tmpD00)),((_tmpD00[0]=((_tmpCFF.tag=5,((_tmpCFF.f1=_tmp83A,_tmpCFF)))),_tmpD00)))),0),((_tmpCFE->tl=Cyc_Toc_result_decls,_tmpCFE))))));};}}
# 4962
return 0;};}
# 4965
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple38*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 4984 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple38*env){
# 4987
struct _tuple38 _tmp840=*env;struct _tuple38 _tmp841=_tmp840;struct Cyc_Toc_TocState*_tmp842;struct Cyc_Absyn_Datatypedecl*_tmp843;_LL4FB: _tmp842=_tmp841.f1;_tmp843=_tmp841.f2;_LL4FC:;
if(_tmp843->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp844=*_tmp842;struct Cyc_Toc_TocState _tmp845=_tmp844;struct Cyc_Dict_Dict*_tmp846;_LL4FE: _tmp846=_tmp845.xdatatypes_so_far;_LL4FF:;{
struct _tuple1*_tmp847=_tmp843->name;
{struct Cyc_List_List*_tmp848=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp843->fields))->v;for(0;_tmp848 != 0;_tmp848=_tmp848->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp848->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp849=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0);
void*_tmp84A=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,_tmp849,Cyc_Absyn_false_conref,0);
# 4998
int*_tmp84B=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp846,f->name);int*_tmp84C=_tmp84B;if(_tmp84C == 0){_LL501: _LL502: {
# 5000
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0);{
# 5004
struct Cyc_Absyn_Vardecl*_tmp84D=Cyc_Absyn_new_vardecl(f->name,_tmp84A,initopt);
_tmp84D->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD06;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD05;struct Cyc_List_List*_tmpD04;Cyc_Toc_result_decls=((_tmpD04=_cycalloc(sizeof(*_tmpD04)),((_tmpD04->hd=Cyc_Absyn_new_decl((void*)((_tmpD06=_cycalloc(sizeof(*_tmpD06)),((_tmpD06[0]=((_tmpD05.tag=0,((_tmpD05.f1=_tmp84D,_tmpD05)))),_tmpD06)))),0),((_tmpD04->tl=Cyc_Toc_result_decls,_tmpD04))))));}
*_tmp846=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp846,f->name,f->sc != Cyc_Absyn_Extern);{
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp851=f->typs;for(0;_tmp851 != 0;(_tmp851=_tmp851->tl,i ++)){
struct Cyc_Int_pa_PrintArg_struct _tmpD0E;void*_tmpD0D[1];const char*_tmpD0C;struct _dyneither_ptr*_tmpD0B;struct _dyneither_ptr*_tmp852=(_tmpD0B=_cycalloc(sizeof(*_tmpD0B)),((_tmpD0B[0]=(struct _dyneither_ptr)((_tmpD0E.tag=1,((_tmpD0E.f1=(unsigned long)i,((_tmpD0D[0]=& _tmpD0E,Cyc_aprintf(((_tmpD0C="f%d",_tag_dyneither(_tmpD0C,sizeof(char),4))),_tag_dyneither(_tmpD0D,sizeof(void*),1)))))))),_tmpD0B)));
struct Cyc_Absyn_Aggrfield*_tmpD0F;struct Cyc_Absyn_Aggrfield*_tmp853=(_tmpD0F=_cycalloc(sizeof(*_tmpD0F)),((_tmpD0F->name=_tmp852,((_tmpD0F->tq=(*((struct _tuple11*)_tmp851->hd)).f1,((_tmpD0F->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp851->hd)).f2),((_tmpD0F->width=0,((_tmpD0F->attributes=0,((_tmpD0F->requires_clause=0,_tmpD0F)))))))))))));
struct Cyc_List_List*_tmpD10;fields=((_tmpD10=_cycalloc(sizeof(*_tmpD10)),((_tmpD10->hd=_tmp853,((_tmpD10->tl=fields,_tmpD10))))));}}
# 5017
{struct Cyc_Absyn_Aggrfield*_tmpD13;struct Cyc_List_List*_tmpD12;fields=((_tmpD12=_cycalloc(sizeof(*_tmpD12)),((_tmpD12->hd=((_tmpD13=_cycalloc(sizeof(*_tmpD13)),((_tmpD13->name=Cyc_Toc_tag_sp,((_tmpD13->tq=Cyc_Toc_mt_tq,((_tmpD13->type=
Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq),((_tmpD13->width=0,((_tmpD13->attributes=0,((_tmpD13->requires_clause=0,_tmpD13)))))))))))))),((_tmpD12->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmpD12))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpD16;struct Cyc_Absyn_Aggrdecl*_tmpD15;struct Cyc_Absyn_Aggrdecl*_tmp85C=
(_tmpD15=_cycalloc(sizeof(*_tmpD15)),((_tmpD15->kind=Cyc_Absyn_StructA,((_tmpD15->sc=Cyc_Absyn_Public,((_tmpD15->name=
Cyc_Toc_collapse_qvars(f->name,_tmp843->name),((_tmpD15->tvs=0,((_tmpD15->expected_mem_kind=0,((_tmpD15->impl=(
# 5024
(_tmpD16=_cycalloc(sizeof(*_tmpD16)),((_tmpD16->exist_vars=0,((_tmpD16->rgn_po=0,((_tmpD16->fields=fields,((_tmpD16->tagged=0,_tmpD16)))))))))),((_tmpD15->attributes=0,_tmpD15)))))))))))))));
# 5026
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD1C;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD1B;struct Cyc_List_List*_tmpD1A;Cyc_Toc_result_decls=((_tmpD1A=_cycalloc(sizeof(*_tmpD1A)),((_tmpD1A->hd=Cyc_Absyn_new_decl((void*)((_tmpD1C=_cycalloc(sizeof(*_tmpD1C)),((_tmpD1C[0]=((_tmpD1B.tag=5,((_tmpD1B.f1=_tmp85C,_tmpD1B)))),_tmpD1C)))),0),((_tmpD1A->tl=Cyc_Toc_result_decls,_tmpD1A))))));}
# 5028
goto _LL500;};};};}}else{if(*((int*)_tmp84C)== 0){_LL503: _LL504:
# 5030
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp862=Cyc_Absyn_string_exp(*fn,0);
struct Cyc_Absyn_Vardecl*_tmp863=Cyc_Absyn_new_vardecl(f->name,_tmp84A,_tmp862);
_tmp863->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD22;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD21;struct Cyc_List_List*_tmpD20;Cyc_Toc_result_decls=((_tmpD20=_cycalloc(sizeof(*_tmpD20)),((_tmpD20->hd=Cyc_Absyn_new_decl((void*)((_tmpD22=_cycalloc(sizeof(*_tmpD22)),((_tmpD22[0]=((_tmpD21.tag=0,((_tmpD21.f1=_tmp863,_tmpD21)))),_tmpD22)))),0),((_tmpD20->tl=Cyc_Toc_result_decls,_tmpD20))))));}
*_tmp846=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp846,f->name,1);}
# 5037
goto _LL500;}else{_LL505: _LL506:
 goto _LL500;}}_LL500:;}}
# 5041
return 0;};};}
# 5044
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple38*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 5049
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 5057
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
vd->type=Cyc_Toc_typ_to_c(old_typ);
# 5061
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(body_nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp867=init->r;void*_tmp868=_tmp867;struct Cyc_Absyn_Vardecl*_tmp869;struct Cyc_Absyn_Exp*_tmp86A;struct Cyc_Absyn_Exp*_tmp86B;int _tmp86C;if(((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp868)->tag == 26){_LL508: _tmp869=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp868)->f1;_tmp86A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp868)->f2;_tmp86B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp868)->f3;_tmp86C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp868)->f4;_LL509:
# 5068
 vd->initializer=0;
s->r=(Cyc_Toc_init_comprehension(init_nv,old_typ,Cyc_Absyn_var_exp(vd->name,0),_tmp869,_tmp86A,_tmp86B,_tmp86C,
Cyc_Absyn_new_stmt(s->r,0),0))->r;
goto _LL507;}else{_LL50A: _LL50B:
# 5073
 if(vd->sc == Cyc_Absyn_Static){
# 5077
struct _RegionHandle _tmp86D=_new_region("temp");struct _RegionHandle*temp=& _tmp86D;_push_region(temp);
{struct Cyc_Toc_Env*_tmp86E=Cyc_Toc_set_toplevel(temp,init_nv);
Cyc_Toc_exp_to_c(_tmp86E,init);}
# 5078
;_pop_region(temp);}else{
# 5082
Cyc_Toc_exp_to_c(init_nv,init);}
goto _LL507;}_LL507:;}else{
# 5087
void*_tmp86F=Cyc_Tcutil_compress(old_typ);void*_tmp870=_tmp86F;void*_tmp871;struct Cyc_Absyn_Exp*_tmp872;union Cyc_Absyn_Constraint*_tmp873;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp870)->tag == 8){_LL50D: _tmp871=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp870)->f1).elt_type;_tmp872=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp870)->f1).num_elts;_tmp873=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp870)->f1).zero_term;if(
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp873)){_LL50E:
 if(_tmp872 == 0){
const char*_tmpD25;void*_tmpD24;(_tmpD24=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD25="can't initialize zero-terminated array -- size unknown",_tag_dyneither(_tmpD25,sizeof(char),55))),_tag_dyneither(_tmpD24,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=_tmp872;
struct Cyc_Absyn_Exp*_tmp876=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),
Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);
# 5095
struct Cyc_Absyn_Exp*_tmp877=Cyc_Absyn_signed_int_exp(0,0);
s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp876,_tmp877,0),0),
Cyc_Absyn_new_stmt(s->r,0));
goto _LL50C;};}else{goto _LL50F;}}else{_LL50F: _LL510:
 goto _LL50C;}_LL50C:;}}
# 5106
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success){
void*_tmp878=d;struct Cyc_List_List*_tmp879;struct Cyc_List_List*_tmp87A;void**_tmp87B;struct Cyc_Tcpat_Rhs*_tmp87C;switch(*((int*)_tmp878)){case 0: _LL512: _LL513:
 return d;case 1: _LL514: _tmp87C=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp878)->f1;_LL515:
 _tmp87C->rhs=success;return d;default: _LL516: _tmp879=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp878)->f1;_tmp87A=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp878)->f2;_tmp87B=(void**)&((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp878)->f3;_LL517:
# 5111
*_tmp87B=Cyc_Toc_rewrite_decision(*_tmp87B,success);
for(0;_tmp87A != 0;_tmp87A=_tmp87A->tl){
struct _tuple29*_tmp87D=(struct _tuple29*)_tmp87A->hd;struct _tuple29*_tmp87E=_tmp87D;void**_tmp87F;_LL519: _tmp87F=(void**)& _tmp87E->f2;_LL51A:;
*_tmp87F=Cyc_Toc_rewrite_decision(*_tmp87F,success);}
# 5116
return d;}_LL511:;}
# 5127 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 5129
struct _RegionHandle _tmp880=_new_region("rgn");struct _RegionHandle*rgn=& _tmp880;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp881=Cyc_Toc_share_env(rgn,nv);
void*_tmp882=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(_tmp881,e);{
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 5139
struct Cyc_Absyn_Stmt*_tmp883=Cyc_Toc_skip_stmt_dl();
if(dopt != 0)
# 5143
dopt=Cyc_Toc_rewrite_decision(dopt,_tmp883);{
# 5145
struct Cyc_Absyn_Switch_clause*_tmpD26;struct Cyc_Absyn_Switch_clause*_tmp884=(_tmpD26=_cycalloc(sizeof(*_tmpD26)),((_tmpD26->pattern=p,((_tmpD26->pat_vars=0,((_tmpD26->where_clause=0,((_tmpD26->body=_tmp883,((_tmpD26->loc=0,_tmpD26)))))))))));
struct Cyc_Absyn_Switch_clause*_tmpD27[1];struct Cyc_List_List*_tmp885=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple31*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,((_tmpD27[0]=_tmp884,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(rgn,_tag_dyneither(_tmpD27,sizeof(struct Cyc_Absyn_Switch_clause*),1)))));
# 5148
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
# 5152
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp881,& mydecls,& mybodies,dopt,_tmp885,v);
# 5155
struct Cyc_Toc_Env*senv;
for(0;mybodies != 0;mybodies=((struct Cyc_List_List*)_check_null(mybodies))->tl){
struct _tuple35*_tmp886=(struct _tuple35*)((struct Cyc_List_List*)_check_null(mybodies))->hd;struct _tuple35*_tmp887=_tmp886;struct Cyc_Toc_Env*_tmp888;struct Cyc_Absyn_Stmt*_tmp889;_LL51C: _tmp888=_tmp887->f1;_tmp889=_tmp887->f3;_LL51D:;
if(_tmp889 == _tmp883){senv=_tmp888;goto FOUND_ENV;}}
# 5160
{const char*_tmpD2A;void*_tmpD29;(_tmpD29=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD2A="letdecl_to_c: couldn't find env!",_tag_dyneither(_tmpD2A,sizeof(char),33))),_tag_dyneither(_tmpD29,sizeof(void*),0)));}
FOUND_ENV:
# 5164
 Cyc_Toc_stmt_to_c(senv,s);{
# 5166
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(test_tree,s,0);
# 5168
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct _tuple33*_tmp88C=(struct _tuple33*)((struct Cyc_List_List*)_check_null(mydecls))->hd;struct _tuple33*_tmp88D=_tmp88C;struct _tuple1*_tmp88E;void*_tmp88F;_LL51F: _tmp88E=_tmp88D->f1;_tmp88F=_tmp88D->f2;_LL520:;
res=Cyc_Absyn_declare_stmt(_tmp88E,_tmp88F,0,res,0);}
# 5173
res=Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c(_tmp882),e,res,0);{
struct Cyc_Absyn_Stmt*_tmp890=res;_npop_handler(0);return _tmp890;};};};};}
# 5130
;_pop_region(rgn);}
# 5181
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp893=e->r;void*_tmp894=_tmp893;struct Cyc_Absyn_MallocInfo*_tmp895;struct Cyc_Absyn_Stmt*_tmp896;void**_tmp897;void**_tmp898;struct Cyc_List_List*_tmp899;struct Cyc_List_List*_tmp89A;void**_tmp89B;struct Cyc_List_List*_tmp89C;void**_tmp89D;struct Cyc_Absyn_Exp*_tmp89E;struct Cyc_Absyn_Exp*_tmp89F;struct Cyc_List_List*_tmp8A0;struct Cyc_Absyn_Exp*_tmp8A1;struct Cyc_Absyn_Exp*_tmp8A2;struct Cyc_Absyn_Exp*_tmp8A3;struct Cyc_Absyn_Exp*_tmp8A4;struct Cyc_Absyn_Exp*_tmp8A5;struct Cyc_Absyn_Exp*_tmp8A6;struct Cyc_Absyn_Exp*_tmp8A7;struct Cyc_Absyn_Exp*_tmp8A8;struct Cyc_Absyn_Exp*_tmp8A9;struct Cyc_Absyn_Exp*_tmp8AA;struct Cyc_Absyn_Exp*_tmp8AB;struct Cyc_Absyn_Exp*_tmp8AC;struct Cyc_Absyn_Exp*_tmp8AD;struct Cyc_Absyn_Exp*_tmp8AE;struct Cyc_Absyn_Exp*_tmp8AF;struct Cyc_List_List*_tmp8B0;struct Cyc_Absyn_Exp*_tmp8B1;struct Cyc_Absyn_Exp*_tmp8B2;struct Cyc_Absyn_Exp*_tmp8B3;struct Cyc_Absyn_Exp*_tmp8B4;struct Cyc_Absyn_Exp*_tmp8B5;struct Cyc_Absyn_Exp*_tmp8B6;struct Cyc_Absyn_Exp*_tmp8B7;struct Cyc_Absyn_Exp*_tmp8B8;switch(*((int*)_tmp894)){case 19: _LL522: _tmp8B8=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp894)->f1;_LL523:
 _tmp8B7=_tmp8B8;goto _LL525;case 20: _LL524: _tmp8B7=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp894)->f1;_LL525:
 _tmp8B6=_tmp8B7;goto _LL527;case 21: _LL526: _tmp8B6=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp894)->f1;_LL527:
 _tmp8B5=_tmp8B6;goto _LL529;case 14: _LL528: _tmp8B5=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp894)->f1;_LL529:
 _tmp8B4=_tmp8B5;goto _LL52B;case 10: _LL52A: _tmp8B4=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp894)->f1;_LL52B:
 _tmp8B3=_tmp8B4;goto _LL52D;case 11: _LL52C: _tmp8B3=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp894)->f1;_LL52D:
 _tmp8B2=_tmp8B3;goto _LL52F;case 17: _LL52E: _tmp8B2=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp894)->f1;_LL52F:
 _tmp8B1=_tmp8B2;goto _LL531;case 4: _LL530: _tmp8B1=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp894)->f1;_LL531:
 Cyc_Toc_exptypes_to_c(_tmp8B1);goto _LL521;case 2: _LL532: _tmp8B0=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp894)->f2;_LL533:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp8B0);goto _LL521;case 6: _LL534: _tmp8AE=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp894)->f1;_tmp8AF=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp894)->f2;_LL535:
 _tmp8AC=_tmp8AE;_tmp8AD=_tmp8AF;goto _LL537;case 7: _LL536: _tmp8AC=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp894)->f1;_tmp8AD=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp894)->f2;_LL537:
 _tmp8AA=_tmp8AC;_tmp8AB=_tmp8AD;goto _LL539;case 8: _LL538: _tmp8AA=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp894)->f1;_tmp8AB=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp894)->f2;_LL539:
 _tmp8A8=_tmp8AA;_tmp8A9=_tmp8AB;goto _LL53B;case 22: _LL53A: _tmp8A8=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp894)->f1;_tmp8A9=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp894)->f2;_LL53B:
 _tmp8A6=_tmp8A8;_tmp8A7=_tmp8A9;goto _LL53D;case 34: _LL53C: _tmp8A6=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp894)->f1;_tmp8A7=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp894)->f2;_LL53D:
 _tmp8A4=_tmp8A6;_tmp8A5=_tmp8A7;goto _LL53F;case 3: _LL53E: _tmp8A4=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp894)->f1;_tmp8A5=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp894)->f3;_LL53F:
 Cyc_Toc_exptypes_to_c(_tmp8A4);Cyc_Toc_exptypes_to_c(_tmp8A5);goto _LL521;case 5: _LL540: _tmp8A1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp894)->f1;_tmp8A2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp894)->f2;_tmp8A3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp894)->f3;_LL541:
# 5199
 Cyc_Toc_exptypes_to_c(_tmp8A1);Cyc_Toc_exptypes_to_c(_tmp8A2);Cyc_Toc_exptypes_to_c(_tmp8A3);goto _LL521;case 9: _LL542: _tmp89F=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp894)->f1;_tmp8A0=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp894)->f2;_LL543:
# 5201
 Cyc_Toc_exptypes_to_c(_tmp89F);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp8A0);goto _LL521;case 13: _LL544: _tmp89D=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp894)->f1;_tmp89E=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp894)->f2;_LL545:
*_tmp89D=Cyc_Toc_typ_to_c(*_tmp89D);Cyc_Toc_exptypes_to_c(_tmp89E);goto _LL521;case 24: _LL546: _tmp89B=(void**)&(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp894)->f1)->f3;_tmp89C=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp894)->f2;_LL547:
# 5204
*_tmp89B=Cyc_Toc_typ_to_c(*_tmp89B);
_tmp89A=_tmp89C;goto _LL549;case 35: _LL548: _tmp89A=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp894)->f2;_LL549:
 _tmp899=_tmp89A;goto _LL54B;case 25: _LL54A: _tmp899=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp894)->f1;_LL54B:
# 5208
 for(0;_tmp899 != 0;_tmp899=_tmp899->tl){
struct _tuple19 _tmp8B9=*((struct _tuple19*)_tmp899->hd);struct _tuple19 _tmp8BA=_tmp8B9;struct Cyc_Absyn_Exp*_tmp8BB;_LL573: _tmp8BB=_tmp8BA.f2;_LL574:;
Cyc_Toc_exptypes_to_c(_tmp8BB);}
# 5212
goto _LL521;case 18: _LL54C: _tmp898=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp894)->f1;_LL54D:
 _tmp897=_tmp898;goto _LL54F;case 16: _LL54E: _tmp897=(void**)&((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp894)->f1;_LL54F:
*_tmp897=Cyc_Toc_typ_to_c(*_tmp897);goto _LL521;case 36: _LL550: _tmp896=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp894)->f1;_LL551:
 Cyc_Toc_stmttypes_to_c(_tmp896);goto _LL521;case 33: _LL552: _tmp895=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp894)->f1;_LL553:
# 5217
 if(_tmp895->elt_type != 0){
void**_tmpD2B;_tmp895->elt_type=((_tmpD2B=_cycalloc(sizeof(*_tmpD2B)),((_tmpD2B[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp895->elt_type))),_tmpD2B))));}
Cyc_Toc_exptypes_to_c(_tmp895->num_elts);
goto _LL521;case 0: _LL554: _LL555:
 goto _LL557;case 1: _LL556: _LL557:
 goto _LL559;case 31: _LL558: _LL559:
 goto _LL55B;case 39: _LL55A: _LL55B:
 goto _LL55D;case 32: _LL55C: _LL55D:
 goto _LL521;case 29: _LL55E: _LL55F:
# 5227
 goto _LL561;case 30: _LL560: _LL561:
 goto _LL563;case 28: _LL562: _LL563:
 goto _LL565;case 26: _LL564: _LL565:
 goto _LL567;case 27: _LL566: _LL567:
 goto _LL569;case 23: _LL568: _LL569:
 goto _LL56B;case 12: _LL56A: _LL56B:
 goto _LL56D;case 15: _LL56C: _LL56D:
 goto _LL56F;case 38: _LL56E: _LL56F:
 goto _LL571;default: _LL570: _LL571:
# 5237
{const char*_tmpD2E;void*_tmpD2D;(_tmpD2D=0,Cyc_Tcutil_terr(e->loc,((_tmpD2E="Cyclone expression within C code",_tag_dyneither(_tmpD2E,sizeof(char),33))),_tag_dyneither(_tmpD2D,sizeof(void*),0)));}
goto _LL521;}_LL521:;}
# 5242
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp8BF=d->r;void*_tmp8C0=_tmp8BF;struct Cyc_Absyn_Typedefdecl*_tmp8C1;struct Cyc_Absyn_Enumdecl*_tmp8C2;struct Cyc_Absyn_Aggrdecl*_tmp8C3;struct Cyc_Absyn_Fndecl*_tmp8C4;struct Cyc_Absyn_Vardecl*_tmp8C5;switch(*((int*)_tmp8C0)){case 0: _LL576: _tmp8C5=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8C0)->f1;_LL577:
# 5245
 _tmp8C5->type=Cyc_Toc_typ_to_c(_tmp8C5->type);
if(_tmp8C5->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp8C5->initializer));
goto _LL575;case 1: _LL578: _tmp8C4=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp8C0)->f1;_LL579:
# 5249
 _tmp8C4->ret_type=Cyc_Toc_typ_to_c(_tmp8C4->ret_type);
{struct Cyc_List_List*_tmp8C6=_tmp8C4->args;for(0;_tmp8C6 != 0;_tmp8C6=_tmp8C6->tl){
(*((struct _tuple9*)_tmp8C6->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp8C6->hd)).f3);}}
# 5253
goto _LL575;case 5: _LL57A: _tmp8C3=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp8C0)->f1;_LL57B:
 Cyc_Toc_aggrdecl_to_c(_tmp8C3,1);goto _LL575;case 7: _LL57C: _tmp8C2=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp8C0)->f1;_LL57D:
# 5256
 if(_tmp8C2->fields != 0){
struct Cyc_List_List*_tmp8C7=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp8C2->fields))->v;for(0;_tmp8C7 != 0;_tmp8C7=_tmp8C7->tl){
struct Cyc_Absyn_Enumfield*_tmp8C8=(struct Cyc_Absyn_Enumfield*)_tmp8C7->hd;
if(_tmp8C8->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp8C8->tag));}}
# 5261
goto _LL575;case 8: _LL57E: _tmp8C1=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp8C0)->f1;_LL57F:
 _tmp8C1->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp8C1->defn));goto _LL575;case 2: _LL580: _LL581:
 goto _LL583;case 3: _LL582: _LL583:
 goto _LL585;case 6: _LL584: _LL585:
 goto _LL587;case 9: _LL586: _LL587:
 goto _LL589;case 10: _LL588: _LL589:
 goto _LL58B;case 11: _LL58A: _LL58B:
 goto _LL58D;case 12: _LL58C: _LL58D:
 goto _LL58F;case 4: _LL58E: _LL58F:
# 5272
{const char*_tmpD31;void*_tmpD30;(_tmpD30=0,Cyc_Tcutil_terr(d->loc,((_tmpD31="Cyclone declaration within C code",_tag_dyneither(_tmpD31,sizeof(char),34))),_tag_dyneither(_tmpD30,sizeof(void*),0)));}
goto _LL575;case 13: _LL590: _LL591:
 goto _LL593;default: _LL592: _LL593:
# 5276
 goto _LL575;}_LL575:;}
# 5280
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp8CB=s->r;void*_tmp8CC=_tmp8CB;struct Cyc_Absyn_Stmt*_tmp8CD;struct Cyc_Absyn_Stmt*_tmp8CE;struct Cyc_Absyn_Exp*_tmp8CF;struct Cyc_Absyn_Decl*_tmp8D0;struct Cyc_Absyn_Stmt*_tmp8D1;struct Cyc_Absyn_Exp*_tmp8D2;struct Cyc_List_List*_tmp8D3;void*_tmp8D4;struct Cyc_Absyn_Exp*_tmp8D5;struct Cyc_Absyn_Exp*_tmp8D6;struct Cyc_Absyn_Exp*_tmp8D7;struct Cyc_Absyn_Stmt*_tmp8D8;struct Cyc_Absyn_Exp*_tmp8D9;struct Cyc_Absyn_Stmt*_tmp8DA;struct Cyc_Absyn_Exp*_tmp8DB;struct Cyc_Absyn_Stmt*_tmp8DC;struct Cyc_Absyn_Stmt*_tmp8DD;struct Cyc_Absyn_Exp*_tmp8DE;struct Cyc_Absyn_Stmt*_tmp8DF;struct Cyc_Absyn_Stmt*_tmp8E0;struct Cyc_Absyn_Exp*_tmp8E1;switch(*((int*)_tmp8CC)){case 1: _LL595: _tmp8E1=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp8CC)->f1;_LL596:
 Cyc_Toc_exptypes_to_c(_tmp8E1);goto _LL594;case 2: _LL597: _tmp8DF=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8CC)->f1;_tmp8E0=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8CC)->f2;_LL598:
 Cyc_Toc_stmttypes_to_c(_tmp8DF);Cyc_Toc_stmttypes_to_c(_tmp8E0);goto _LL594;case 3: _LL599: _tmp8DE=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp8CC)->f1;_LL59A:
 if(_tmp8DE != 0)Cyc_Toc_exptypes_to_c(_tmp8DE);goto _LL594;case 4: _LL59B: _tmp8DB=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8CC)->f1;_tmp8DC=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8CC)->f2;_tmp8DD=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8CC)->f3;_LL59C:
# 5286
 Cyc_Toc_exptypes_to_c(_tmp8DB);Cyc_Toc_stmttypes_to_c(_tmp8DC);Cyc_Toc_stmttypes_to_c(_tmp8DD);goto _LL594;case 5: _LL59D: _tmp8D9=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp8CC)->f1).f1;_tmp8DA=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp8CC)->f2;_LL59E:
# 5288
 Cyc_Toc_exptypes_to_c(_tmp8D9);Cyc_Toc_stmttypes_to_c(_tmp8DA);goto _LL594;case 9: _LL59F: _tmp8D5=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8CC)->f1;_tmp8D6=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8CC)->f2).f1;_tmp8D7=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8CC)->f3).f1;_tmp8D8=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8CC)->f4;_LL5A0:
# 5290
 Cyc_Toc_exptypes_to_c(_tmp8D5);Cyc_Toc_exptypes_to_c(_tmp8D6);Cyc_Toc_exptypes_to_c(_tmp8D7);
Cyc_Toc_stmttypes_to_c(_tmp8D8);goto _LL594;case 10: _LL5A1: _tmp8D2=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8CC)->f1;_tmp8D3=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8CC)->f2;_tmp8D4=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8CC)->f3;_LL5A2:
# 5293
 Cyc_Toc_exptypes_to_c(_tmp8D2);
for(0;_tmp8D3 != 0;_tmp8D3=_tmp8D3->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp8D3->hd)->body);}
goto _LL594;case 12: _LL5A3: _tmp8D0=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8CC)->f1;_tmp8D1=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8CC)->f2;_LL5A4:
 Cyc_Toc_decltypes_to_c(_tmp8D0);Cyc_Toc_stmttypes_to_c(_tmp8D1);goto _LL594;case 14: _LL5A5: _tmp8CE=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp8CC)->f1;_tmp8CF=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp8CC)->f2).f1;_LL5A6:
 Cyc_Toc_stmttypes_to_c(_tmp8CE);Cyc_Toc_exptypes_to_c(_tmp8CF);goto _LL594;case 13: _LL5A7: _tmp8CD=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp8CC)->f2;_LL5A8:
 Cyc_Toc_stmttypes_to_c(_tmp8CD);goto _LL594;case 0: _LL5A9: _LL5AA:
 goto _LL5AC;case 6: _LL5AB: _LL5AC:
 goto _LL5AE;case 7: _LL5AD: _LL5AE:
 goto _LL5B0;case 8: _LL5AF: _LL5B0:
# 5303
 goto _LL594;case 11: _LL5B1: _LL5B2:
# 5306
{struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct _tmpD34;struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmpD33;s->r=(void*)((_tmpD33=_cycalloc_atomic(sizeof(*_tmpD33)),((_tmpD33[0]=((_tmpD34.tag=0,_tmpD34)),_tmpD33))));}
goto _LL594;case 15: _LL5B3: _LL5B4:
 goto _LL5B6;default: _LL5B5: _LL5B6:
# 5310
{struct Cyc_String_pa_PrintArg_struct _tmpD3C;void*_tmpD3B[1];const char*_tmpD3A;void*_tmpD39;(_tmpD39=0,Cyc_Tcutil_terr(s->loc,(struct _dyneither_ptr)((_tmpD3C.tag=0,((_tmpD3C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpD3B[0]=& _tmpD3C,Cyc_aprintf(((_tmpD3A="Cyclone statement in C code: %s",_tag_dyneither(_tmpD3A,sizeof(char),32))),_tag_dyneither(_tmpD3B,sizeof(void*),1)))))))),_tag_dyneither(_tmpD39,sizeof(void*),0)));}
goto _LL594;}_LL594:;}
# 5319
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv)){
const char*_tmpD3F;void*_tmpD3E;(_tmpD3E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD3F="decls_to_c: not at toplevel!",_tag_dyneither(_tmpD3F,sizeof(char),29))),_tag_dyneither(_tmpD3E,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp8EA=d->r;void*_tmp8EB=_tmp8EA;struct Cyc_List_List*_tmp8EC;struct Cyc_List_List*_tmp8ED;struct Cyc_List_List*_tmp8EE;struct Cyc_List_List*_tmp8EF;struct Cyc_Absyn_Typedefdecl*_tmp8F0;struct Cyc_Absyn_Enumdecl*_tmp8F1;struct Cyc_Absyn_Datatypedecl*_tmp8F2;struct Cyc_Absyn_Aggrdecl*_tmp8F3;struct Cyc_Absyn_Fndecl*_tmp8F4;struct Cyc_Absyn_Vardecl*_tmp8F5;switch(*((int*)_tmp8EB)){case 0: _LL5B8: _tmp8F5=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8EB)->f1;_LL5B9: {
# 5326
struct _tuple1*_tmp8F6=_tmp8F5->name;
# 5328
if(_tmp8F5->sc == Cyc_Absyn_ExternC){
struct _tuple1*_tmpD40;_tmp8F6=((_tmpD40=_cycalloc(sizeof(*_tmpD40)),((_tmpD40->f1=Cyc_Absyn_Rel_n(0),((_tmpD40->f2=(*_tmp8F6).f2,_tmpD40))))));}
if(_tmp8F5->initializer != 0){
if(_tmp8F5->sc == Cyc_Absyn_ExternC)_tmp8F5->sc=Cyc_Absyn_Public;
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp8F5->initializer));else{
# 5335
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp8F5->initializer));}}
# 5337
{struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmpD43;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpD42;nv=Cyc_Toc_add_varmap(r,nv,_tmp8F5->name,Cyc_Absyn_varb_exp(_tmp8F6,(void*)((_tmpD42=_cycalloc(sizeof(*_tmpD42)),((_tmpD42[0]=((_tmpD43.tag=1,((_tmpD43.f1=_tmp8F5,_tmpD43)))),_tmpD42)))),0));}
_tmp8F5->name=_tmp8F6;
_tmp8F5->sc=Cyc_Toc_scope_to_c(_tmp8F5->sc);
_tmp8F5->type=Cyc_Toc_typ_to_c(_tmp8F5->type);
{struct Cyc_List_List*_tmpD44;Cyc_Toc_result_decls=((_tmpD44=_cycalloc(sizeof(*_tmpD44)),((_tmpD44->hd=d,((_tmpD44->tl=Cyc_Toc_result_decls,_tmpD44))))));}
goto _LL5B7;}case 1: _LL5BA: _tmp8F4=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp8EB)->f1;_LL5BB: {
# 5344
struct _tuple1*_tmp8FB=_tmp8F4->name;
# 5346
if(_tmp8F4->sc == Cyc_Absyn_ExternC){
{struct _tuple1*_tmpD45;_tmp8FB=((_tmpD45=_cycalloc(sizeof(*_tmpD45)),((_tmpD45->f1=Cyc_Absyn_Abs_n(0,1),((_tmpD45->f2=(*_tmp8FB).f2,_tmpD45))))));}
_tmp8F4->sc=Cyc_Absyn_Public;}
# 5350
nv=Cyc_Toc_add_varmap(r,nv,_tmp8F4->name,Cyc_Absyn_var_exp(_tmp8FB,0));
_tmp8F4->name=_tmp8FB;
Cyc_Toc_fndecl_to_c(nv,_tmp8F4,cinclude);
{struct Cyc_List_List*_tmpD46;Cyc_Toc_result_decls=((_tmpD46=_cycalloc(sizeof(*_tmpD46)),((_tmpD46->hd=d,((_tmpD46->tl=Cyc_Toc_result_decls,_tmpD46))))));}
goto _LL5B7;}case 2: _LL5BC: _LL5BD:
 goto _LL5BF;case 3: _LL5BE: _LL5BF: {
# 5357
const char*_tmpD49;void*_tmpD48;(_tmpD48=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD49="letdecl at toplevel",_tag_dyneither(_tmpD49,sizeof(char),20))),_tag_dyneither(_tmpD48,sizeof(void*),0)));}case 4: _LL5C0: _LL5C1: {
# 5359
const char*_tmpD4C;void*_tmpD4B;(_tmpD4B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD4C="region decl at toplevel",_tag_dyneither(_tmpD4C,sizeof(char),24))),_tag_dyneither(_tmpD4B,sizeof(void*),0)));}case 5: _LL5C2: _tmp8F3=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp8EB)->f1;_LL5C3:
# 5363
 Cyc_Toc_aggrdecl_to_c(_tmp8F3,1);
goto _LL5B7;case 6: _LL5C4: _tmp8F2=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp8EB)->f1;_LL5C5:
# 5366
 if(_tmp8F2->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp8F2);else{
# 5369
Cyc_Toc_datatypedecl_to_c(_tmp8F2);}
goto _LL5B7;case 7: _LL5C6: _tmp8F1=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp8EB)->f1;_LL5C7:
# 5372
 Cyc_Toc_enumdecl_to_c(_tmp8F1);
{struct Cyc_List_List*_tmpD4D;Cyc_Toc_result_decls=((_tmpD4D=_cycalloc(sizeof(*_tmpD4D)),((_tmpD4D->hd=d,((_tmpD4D->tl=Cyc_Toc_result_decls,_tmpD4D))))));}
goto _LL5B7;case 8: _LL5C8: _tmp8F0=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp8EB)->f1;_LL5C9:
# 5376
 _tmp8F0->tvs=0;
if(_tmp8F0->defn != 0)
_tmp8F0->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp8F0->defn));else{
# 5380
enum Cyc_Absyn_KindQual _tmp903=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp8F0->kind))->v)->kind;enum Cyc_Absyn_KindQual _tmp904=_tmp903;if(_tmp904 == Cyc_Absyn_BoxKind){_LL5D7: _LL5D8:
 _tmp8F0->defn=Cyc_Absyn_void_star_typ();goto _LL5D6;}else{_LL5D9: _LL5DA:
 _tmp8F0->defn=(void*)& Cyc_Absyn_VoidType_val;goto _LL5D6;}_LL5D6:;}
# 5385
{struct Cyc_List_List*_tmpD4E;Cyc_Toc_result_decls=((_tmpD4E=_cycalloc(sizeof(*_tmpD4E)),((_tmpD4E->hd=d,((_tmpD4E->tl=Cyc_Toc_result_decls,_tmpD4E))))));}
goto _LL5B7;case 13: _LL5CA: _LL5CB:
 goto _LL5CD;case 14: _LL5CC: _LL5CD:
# 5389
 goto _LL5B7;case 9: _LL5CE: _tmp8EF=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp8EB)->f2;_LL5CF:
 _tmp8EE=_tmp8EF;goto _LL5D1;case 10: _LL5D0: _tmp8EE=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp8EB)->f2;_LL5D1:
 _tmp8ED=_tmp8EE;goto _LL5D3;case 11: _LL5D2: _tmp8ED=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp8EB)->f1;_LL5D3:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp8ED,top,cinclude);goto _LL5B7;default: _LL5D4: _tmp8EC=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp8EB)->f1;_LL5D5:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp8EC,top,1);goto _LL5B7;}_LL5B7:;};}
# 5396
return nv;}
# 5400
static void Cyc_Toc_init(){
struct Cyc_Core_NewDynamicRegion _tmp906=Cyc_Core_new_rckey();struct Cyc_Core_NewDynamicRegion _tmp907=_tmp906;struct Cyc_Core_DynamicRegion*_tmp908;_LL5DC: _tmp908=_tmp907.key;_LL5DD:;{
struct Cyc_Toc_TocState*_tmp909=((struct Cyc_Toc_TocState*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Toc_TocState*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmp908,0,Cyc_Toc_empty_toc_state);
{struct Cyc_Toc_TocStateWrap*_tmpD4F;Cyc_Toc_toc_state=((_tmpD4F=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpD4F)),((_tmpD4F->dyn=(struct Cyc_Core_DynamicRegion*)_tmp908,((_tmpD4F->state=(struct Cyc_Toc_TocState*)_tmp909,_tmpD4F))))));}
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0;
Cyc_Toc_bounds_checks_eliminated=0;{
struct _dyneither_ptr**_tmpD50;Cyc_Toc_globals=_tag_dyneither(((_tmpD50=_cycalloc(sizeof(struct _dyneither_ptr*)* 42),((_tmpD50[0]=(struct _dyneither_ptr*)& Cyc_Toc__throw_str,((_tmpD50[1]=(struct _dyneither_ptr*)& Cyc_Toc_setjmp_str,((_tmpD50[2]=(struct _dyneither_ptr*)& Cyc_Toc__push_handler_str,((_tmpD50[3]=(struct _dyneither_ptr*)& Cyc_Toc__pop_handler_str,((_tmpD50[4]=(struct _dyneither_ptr*)& Cyc_Toc__exn_thrown_str,((_tmpD50[5]=(struct _dyneither_ptr*)& Cyc_Toc__npop_handler_str,((_tmpD50[6]=(struct _dyneither_ptr*)& Cyc_Toc__check_null_str,((_tmpD50[7]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_null_str,((_tmpD50[8]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_notnull_str,((_tmpD50[9]=(struct _dyneither_ptr*)& Cyc_Toc__check_dyneither_subscript_str,((_tmpD50[10]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_str,((_tmpD50[11]=(struct _dyneither_ptr*)& Cyc_Toc__tag_dyneither_str,((_tmpD50[12]=(struct _dyneither_ptr*)& Cyc_Toc__init_dyneither_ptr_str,((_tmpD50[13]=(struct _dyneither_ptr*)& Cyc_Toc__untag_dyneither_ptr_str,((_tmpD50[14]=(struct _dyneither_ptr*)& Cyc_Toc__get_dyneither_size_str,((_tmpD50[15]=(struct _dyneither_ptr*)& Cyc_Toc__get_zero_arr_size_str,((_tmpD50[16]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_plus_str,((_tmpD50[17]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_plus_str,((_tmpD50[18]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpD50[19]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_str,((_tmpD50[20]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpD50[21]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_post_str,((_tmpD50[22]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_str,((_tmpD50[23]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_str,((_tmpD50[24]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_atomic_str,((_tmpD50[25]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_atomic_str,((_tmpD50[26]=(struct _dyneither_ptr*)& Cyc_Toc__region_malloc_str,((_tmpD50[27]=(struct _dyneither_ptr*)& Cyc_Toc__region_calloc_str,((_tmpD50[28]=(struct _dyneither_ptr*)& Cyc_Toc__check_times_str,((_tmpD50[29]=(struct _dyneither_ptr*)& Cyc_Toc__new_region_str,((_tmpD50[30]=(struct _dyneither_ptr*)& Cyc_Toc__push_region_str,((_tmpD50[31]=(struct _dyneither_ptr*)& Cyc_Toc__pop_region_str,((_tmpD50[32]=(struct _dyneither_ptr*)& Cyc_Toc__open_dynregion_str,((_tmpD50[33]=(struct _dyneither_ptr*)& Cyc_Toc__push_dynregion_str,((_tmpD50[34]=(struct _dyneither_ptr*)& Cyc_Toc__pop_dynregion_str,((_tmpD50[35]=(struct _dyneither_ptr*)& Cyc_Toc__reset_region_str,((_tmpD50[36]=(struct _dyneither_ptr*)& Cyc_Toc__throw_arraybounds_str,((_tmpD50[37]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_decrease_size_str,((_tmpD50[38]=(struct _dyneither_ptr*)& Cyc_Toc__throw_match_str,((_tmpD50[39]=(struct _dyneither_ptr*)& Cyc_Toc__swap_word_str,((_tmpD50[40]=(struct _dyneither_ptr*)& Cyc_Toc__swap_dyneither_str,((_tmpD50[41]=(struct _dyneither_ptr*)& Cyc_Toc__fast_region_malloc_str,_tmpD50)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),sizeof(struct _dyneither_ptr*),42);};};}
# 5457
static int Cyc_Toc_destroy_labels(struct _RegionHandle*d,struct Cyc_Toc_TocState*ts){
struct Cyc_Toc_TocState _tmp90C=*ts;struct Cyc_Toc_TocState _tmp90D=_tmp90C;struct Cyc_Xarray_Xarray*_tmp90E;_LL5DF: _tmp90E=_tmp90D.temp_labels;_LL5E0:;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp90E);
return 0;}
# 5465
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){
Cyc_Toc_init();{
struct _RegionHandle _tmp90F=_new_region("start");struct _RegionHandle*start=& _tmp90F;_push_region(start);
Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(start),ds,1,0);{
# 5474
struct Cyc_Toc_TocStateWrap*ts=0;
_swap_word(& ts,& Cyc_Toc_toc_state);{
struct Cyc_Toc_TocStateWrap _tmp910=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp911=_tmp910;struct Cyc_Core_DynamicRegion*_tmp912;struct Cyc_Toc_TocState*_tmp913;_LL5E2: _tmp912=_tmp911.dyn;_tmp913=_tmp911.state;_LL5E3:;
((int(*)(struct Cyc_Core_DynamicRegion*key,struct Cyc_Toc_TocState*arg,int(*body)(struct _RegionHandle*h,struct Cyc_Toc_TocState*arg)))Cyc_Core_open_region)(_tmp912,_tmp913,Cyc_Toc_destroy_labels);
Cyc_Core_free_rckey(_tmp912);
((void(*)(struct _dyneither_ptr ptr))Cyc_Core_ufree)(_tag_dyneither(ts,sizeof(struct Cyc_Toc_TocStateWrap),1));};};{
# 5481
struct Cyc_List_List*_tmp914=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0);return _tmp914;};
# 5467
;_pop_region(start);};}
