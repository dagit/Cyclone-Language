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
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 113
void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k);
# 122 "dict.h"
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);
# 33 "position.h"
typedef unsigned int Cyc_Position_seg_t;
# 37
struct _dyneither_ptr Cyc_Position_string_of_segment(unsigned int);struct Cyc_Position_Error;
# 42
typedef struct Cyc_Position_Error*Cyc_Position_error_t;struct Cyc_Xarray_Xarray{struct _dyneither_ptr elmts;int num_elmts;};
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 103
typedef struct _tuple1*Cyc_Absyn_qvar_t;typedef struct _tuple1*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_opt_t;
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
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 349
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 369
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 374
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 429 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
# 432
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 445
typedef void*Cyc_Absyn_funcparams_t;
# 448
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 512
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 528
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 530
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign,long long);
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr,int);
# 538
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 545
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 563
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 694 "absyn.h"
extern struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val;struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 726
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 912
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 919
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 929
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 932
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
# 939
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 942
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 957
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 959
extern void*Cyc_Absyn_sint_typ;
# 971
void*Cyc_Absyn_exn_typ();
# 982
extern void*Cyc_Absyn_bounds_one;
# 990
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 996
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);
# 998
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1001
void*Cyc_Absyn_void_star_typ();
# 1003
void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple1*name);
void*Cyc_Absyn_unionq_typ(struct _tuple1*name);
# 1009
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1013
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1015
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1021
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1026
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
# 1029
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned int);
# 1033
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1036
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1042
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1045
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1048
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1053
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1057
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1062
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
# 1067
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1072
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1078
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned int);
# 1084
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1094
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1097
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned int loc);
# 1101
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1103
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc);
# 1108
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1112
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1117
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
# 1160
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1163
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1173
struct _dyneither_ptr*Cyc_Absyn_fieldname(int);
# 1177
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1187
struct _tuple1*Cyc_Absyn_binding2qvar(void*);
# 1192
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
# 43 "tcenv.h"
typedef const struct Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;
typedef const struct Cyc_Tcenv_CList*const Cyc_Tcenv_clist_t;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};
# 54
typedef void*Cyc_Tcenv_resolved_t;struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};
# 70
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 74
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 85
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 120 "tcenv.h"
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
# 253 "tcutil.h"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 259
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 261
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 263
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 273
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;};
# 331
void*Cyc_Tcutil_snd_tqt(struct _tuple11*);
# 360
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 364
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 367
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
extern struct _dyneither_ptr Cyc_Toc_globals;
# 42
extern int Cyc_Toc_warn_bounds_checks;
extern int Cyc_Toc_warn_all_null_deref;struct Cyc_Tcpat_TcPatResult{struct _tuple0*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};
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
# 66 "toc.cyc"
extern int Cyc_noexpand_r;
# 77 "toc.cyc"
int Cyc_Toc_warn_bounds_checks=0;
int Cyc_Toc_warn_all_null_deref=0;
unsigned int Cyc_Toc_total_bounds_checks=0;
unsigned int Cyc_Toc_bounds_checks_eliminated=0;
# 83
struct Cyc_Hashtable_Table**Cyc_Toc_gpop_tables=0;
struct Cyc_Hashtable_Table**Cyc_Toc_fn_pop_table=0;
static int Cyc_Toc_get_npop(struct Cyc_Absyn_Stmt*s){
return((int(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_fn_pop_table)),s);}
# 93
static struct Cyc_List_List*Cyc_Toc_result_decls=0;struct Cyc_Toc_TocState{struct Cyc_List_List**tuple_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_List_List**abs_struct_types;struct Cyc_Set_Set**datatypes_so_far;struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*temp_labels;};struct _tuple14{struct _tuple1*f1;struct _tuple1*f2;};
# 113
int Cyc_Toc_qvar_tag_cmp(struct _tuple14*x,struct _tuple14*y){
struct _tuple14 _tmp0=*x;struct _tuple14 _tmp1=_tmp0;struct _tuple1*_tmp7;struct _tuple1*_tmp6;_LL1: _tmp7=_tmp1.f1;_tmp6=_tmp1.f2;_LL2:;{
struct _tuple14 _tmp2=*y;struct _tuple14 _tmp3=_tmp2;struct _tuple1*_tmp5;struct _tuple1*_tmp4;_LL4: _tmp5=_tmp3.f1;_tmp4=_tmp3.f2;_LL5:;{
int i=Cyc_Absyn_qvar_cmp(_tmp7,_tmp5);
if(i != 0)return i;
return Cyc_Absyn_qvar_cmp(_tmp6,_tmp4);};};}
# 122
static struct Cyc_Toc_TocState*Cyc_Toc_empty_toc_state(struct _RegionHandle*d,int unused){
return({struct Cyc_Toc_TocState*_tmp8=_region_malloc(d,sizeof(*_tmp8));_tmp8->tuple_types=({struct Cyc_List_List**_tmpE=_region_malloc(d,sizeof(*_tmpE));_tmpE[0]=0;_tmpE;});_tmp8->aggrs_so_far=({struct Cyc_Dict_Dict*_tmpD=_region_malloc(d,sizeof(*_tmpD));_tmpD[0]=
# 125
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);_tmpD;});_tmp8->abs_struct_types=({struct Cyc_List_List**_tmpC=_region_malloc(d,sizeof(*_tmpC));_tmpC[0]=0;_tmpC;});_tmp8->datatypes_so_far=({struct Cyc_Set_Set**_tmpB=_region_malloc(d,sizeof(*_tmpB));_tmpB[0]=
# 127
((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp);_tmpB;});_tmp8->xdatatypes_so_far=({struct Cyc_Dict_Dict*_tmpA=_region_malloc(d,sizeof(*_tmpA));_tmpA[0]=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);_tmpA;});_tmp8->qvar_tags=({struct Cyc_Dict_Dict*_tmp9=_region_malloc(d,sizeof(*_tmp9));_tmp9[0]=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp);_tmp9;});_tmp8->temp_labels=
((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d);_tmp8;});}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 139
typedef struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state_t;
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple15{struct Cyc_Toc_TocState*f1;void*f2;};
# 146
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple15*)){
# 149
struct Cyc_Toc_TocStateWrap*ts=0;
_swap_word(& ts,& Cyc_Toc_toc_state);{
struct Cyc_Toc_TocStateWrap _tmpF=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp10=_tmpF;struct Cyc_Core_DynamicRegion*_tmp17;struct Cyc_Toc_TocState*_tmp16;_LL7: _tmp17=_tmp10.dyn;_tmp16=_tmp10.state;_LL8:;{
struct _dyneither_ptr _tmp11=((struct _dyneither_ptr(*)(struct _dyneither_ptr ptr))Cyc_Core_alias_refptr)(_tag_dyneither(_tmp17,sizeof(struct Cyc_Core_DynamicRegion),1));
*ts=({struct Cyc_Toc_TocStateWrap _tmp12;_tmp12.dyn=(struct Cyc_Core_DynamicRegion*)_tmp17;_tmp12.state=(struct Cyc_Toc_TocState*)_tmp16;_tmp12;});
_swap_word(& ts,& Cyc_Toc_toc_state);{
struct _tuple15 _tmp13=({struct _tuple15 _tmp15;_tmp15.f1=_tmp16;_tmp15.f2=arg;_tmp15;});
void*_tmp14=((void*(*)(struct Cyc_Core_DynamicRegion*key,struct _tuple15*arg,void*(*body)(struct _RegionHandle*h,struct _tuple15*arg)))Cyc_Core_open_region)((struct Cyc_Core_DynamicRegion*)_untag_dyneither_ptr(_tmp11,sizeof(struct Cyc_Core_DynamicRegion),1),& _tmp13,f);
Cyc_Core_free_rckey((struct Cyc_Core_DynamicRegion*)_untag_dyneither_ptr(_tmp11,sizeof(struct Cyc_Core_DynamicRegion),1));
return _tmp14;};};};}struct _tuple16{struct _tuple1*f1;void*(*f2)(struct _tuple1*);};struct _tuple17{struct Cyc_Toc_TocState*f1;struct _tuple16*f2;};struct _tuple18{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
# 161
static void*Cyc_Toc_aggrdecl_type_body(struct _RegionHandle*d,struct _tuple17*env){
# 164
struct _tuple17 _tmp18=*env;struct _tuple17 _tmp19=_tmp18;struct Cyc_Toc_TocState*_tmp1F;struct _tuple1*_tmp1E;void*(*_tmp1D)(struct _tuple1*);_LLA: _tmp1F=_tmp19.f1;_tmp1E=(_tmp19.f2)->f1;_tmp1D=(_tmp19.f2)->f2;_LLB:;{
struct _tuple18**v=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp1F->aggrs_so_far,_tmp1E);
if(v == 0)
return _tmp1D(_tmp1E);else{
# 169
struct _tuple18*_tmp1A=*v;struct _tuple18*_tmp1B=_tmp1A;void*_tmp1C;_LLD: _tmp1C=_tmp1B->f2;_LLE:;
return _tmp1C;}};}
# 174
void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){
struct _tuple16 env=({struct _tuple16 _tmp20;_tmp20.f1=q;_tmp20.f2=type_maker;_tmp20;});
return((void*(*)(struct _tuple16*arg,void*(*f)(struct _RegionHandle*,struct _tuple17*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_aggrdecl_type_body);}
# 179
static int Cyc_Toc_tuple_type_counter=0;
static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;char Cyc_Toc_Toc_Unimplemented[18]="Toc_Unimplemented";struct Cyc_Toc_Toc_Unimplemented_exn_struct{char*tag;};char Cyc_Toc_Toc_Impossible[15]="Toc_Impossible";struct Cyc_Toc_Toc_Impossible_exn_struct{char*tag;};
# 186
static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 189
Cyc_vfprintf(Cyc_stderr,fmt,ap);
({void*_tmp21=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp22="\n";_tag_dyneither(_tmp22,sizeof(char),2);}),_tag_dyneither(_tmp21,sizeof(void*),0));});
Cyc_fflush(Cyc_stderr);
(int)_throw((void*)({struct Cyc_Toc_Toc_Unimplemented_exn_struct*_tmp23=_cycalloc_atomic(sizeof(*_tmp23));_tmp23[0]=({struct Cyc_Toc_Toc_Unimplemented_exn_struct _tmp24;_tmp24.tag=Cyc_Toc_Toc_Unimplemented;_tmp24;});_tmp23;}));}
# 194
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 197
Cyc_vfprintf(Cyc_stderr,fmt,ap);
({void*_tmp25=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp26="\n";_tag_dyneither(_tmp26,sizeof(char),2);}),_tag_dyneither(_tmp25,sizeof(void*),0));});
Cyc_fflush(Cyc_stderr);
(int)_throw((void*)({struct Cyc_Toc_Toc_Impossible_exn_struct*_tmp27=_cycalloc_atomic(sizeof(*_tmp27));_tmp27[0]=({struct Cyc_Toc_Toc_Impossible_exn_struct _tmp28;_tmp28.tag=Cyc_Toc_Toc_Impossible;_tmp28;});_tmp27;}));}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_exn_struct{char*tag;};static char _tmp29[5]="curr";
# 212 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc_curr_string={_tmp29,_tmp29,_tmp29 + 5};static struct _dyneither_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp2A[4]="tag";
static struct _dyneither_ptr Cyc_Toc_tag_string={_tmp2A,_tmp2A,_tmp2A + 4};static struct _dyneither_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp2B[4]="val";
static struct _dyneither_ptr Cyc_Toc_val_string={_tmp2B,_tmp2B,_tmp2B + 4};static struct _dyneither_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp2C[14]="_handler_cons";
static struct _dyneither_ptr Cyc_Toc__handler_cons_string={_tmp2C,_tmp2C,_tmp2C + 14};static struct _dyneither_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;static char _tmp2D[8]="handler";
static struct _dyneither_ptr Cyc_Toc_handler_string={_tmp2D,_tmp2D,_tmp2D + 8};static struct _dyneither_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp2E[14]="_RegionHandle";
static struct _dyneither_ptr Cyc_Toc__RegionHandle_string={_tmp2E,_tmp2E,_tmp2E + 14};static struct _dyneither_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;static char _tmp2F[17]="_DynRegionHandle";
static struct _dyneither_ptr Cyc_Toc__DynRegionHandle_string={_tmp2F,_tmp2F,_tmp2F + 17};static struct _dyneither_ptr*Cyc_Toc__DynRegionHandle_sp=& Cyc_Toc__DynRegionHandle_string;static char _tmp30[16]="_DynRegionFrame";
static struct _dyneither_ptr Cyc_Toc__DynRegionFrame_string={_tmp30,_tmp30,_tmp30 + 16};static struct _dyneither_ptr*Cyc_Toc__DynRegionFrame_sp=& Cyc_Toc__DynRegionFrame_string;
# 225
struct _dyneither_ptr Cyc_Toc_globals={(void*)0,(void*)0,(void*)(0 + 0)};static char _tmp31[7]="_throw";
# 234 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc__throw_str={_tmp31,_tmp31,_tmp31 + 7};static struct _tuple1 Cyc_Toc__throw_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_bnd={0,& Cyc_Toc__throw_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_re={1,(void*)& Cyc_Toc__throw_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)& Cyc_Toc__throw_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp34[7]="setjmp";
static struct _dyneither_ptr Cyc_Toc_setjmp_str={_tmp34,_tmp34,_tmp34 + 7};static struct _tuple1 Cyc_Toc_setjmp_pr={{.Loc_n={4,0}},& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc_setjmp_bnd={0,& Cyc_Toc_setjmp_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc_setjmp_re={1,(void*)& Cyc_Toc_setjmp_bnd};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)& Cyc_Toc_setjmp_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmp37[14]="_push_handler";
static struct _dyneither_ptr Cyc_Toc__push_handler_str={_tmp37,_tmp37,_tmp37 + 14};static struct _tuple1 Cyc_Toc__push_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_handler_bnd={0,& Cyc_Toc__push_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_handler_re={1,(void*)& Cyc_Toc__push_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)& Cyc_Toc__push_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static char _tmp3A[13]="_pop_handler";
static struct _dyneither_ptr Cyc_Toc__pop_handler_str={_tmp3A,_tmp3A,_tmp3A + 13};static struct _tuple1 Cyc_Toc__pop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_handler_bnd={0,& Cyc_Toc__pop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_handler_re={1,(void*)& Cyc_Toc__pop_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={0,(void*)& Cyc_Toc__pop_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static char _tmp3D[12]="_exn_thrown";
static struct _dyneither_ptr Cyc_Toc__exn_thrown_str={_tmp3D,_tmp3D,_tmp3D + 12};static struct _tuple1 Cyc_Toc__exn_thrown_pr={{.Loc_n={4,0}},& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__exn_thrown_bnd={0,& Cyc_Toc__exn_thrown_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__exn_thrown_re={1,(void*)& Cyc_Toc__exn_thrown_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)& Cyc_Toc__exn_thrown_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;static char _tmp40[14]="_npop_handler";
static struct _dyneither_ptr Cyc_Toc__npop_handler_str={_tmp40,_tmp40,_tmp40 + 14};static struct _tuple1 Cyc_Toc__npop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__npop_handler_bnd={0,& Cyc_Toc__npop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__npop_handler_re={1,(void*)& Cyc_Toc__npop_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)& Cyc_Toc__npop_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;static char _tmp43[12]="_check_null";
static struct _dyneither_ptr Cyc_Toc__check_null_str={_tmp43,_tmp43,_tmp43 + 12};static struct _tuple1 Cyc_Toc__check_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_null_bnd={0,& Cyc_Toc__check_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_null_re={1,(void*)& Cyc_Toc__check_null_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)& Cyc_Toc__check_null_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;static char _tmp46[28]="_check_known_subscript_null";
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_null_str={_tmp46,_tmp46,_tmp46 + 28};static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_null_bnd={0,& Cyc_Toc__check_known_subscript_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_null_re={1,(void*)& Cyc_Toc__check_known_subscript_null_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={0,(void*)& Cyc_Toc__check_known_subscript_null_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;static char _tmp49[31]="_check_known_subscript_notnull";
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_notnull_str={_tmp49,_tmp49,_tmp49 + 31};static struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_notnull_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_notnull_bnd={0,& Cyc_Toc__check_known_subscript_notnull_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_notnull_re={1,(void*)& Cyc_Toc__check_known_subscript_notnull_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)& Cyc_Toc__check_known_subscript_notnull_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp4C[27]="_check_dyneither_subscript";
static struct _dyneither_ptr Cyc_Toc__check_dyneither_subscript_str={_tmp4C,_tmp4C,_tmp4C + 27};static struct _tuple1 Cyc_Toc__check_dyneither_subscript_pr={{.Loc_n={4,0}},& Cyc_Toc__check_dyneither_subscript_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_dyneither_subscript_bnd={0,& Cyc_Toc__check_dyneither_subscript_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_dyneither_subscript_re={1,(void*)& Cyc_Toc__check_dyneither_subscript_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={0,(void*)& Cyc_Toc__check_dyneither_subscript_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;static char _tmp4F[15]="_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_str={_tmp4F,_tmp4F,_tmp4F + 15};static struct _tuple1 Cyc_Toc__dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_bnd={0,& Cyc_Toc__dyneither_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_re={1,(void*)& Cyc_Toc__dyneither_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={0,(void*)& Cyc_Toc__dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=& Cyc_Toc__dyneither_ptr_ev;static char _tmp52[15]="_tag_dyneither";
static struct _dyneither_ptr Cyc_Toc__tag_dyneither_str={_tmp52,_tmp52,_tmp52 + 15};static struct _tuple1 Cyc_Toc__tag_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__tag_dyneither_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__tag_dyneither_bnd={0,& Cyc_Toc__tag_dyneither_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__tag_dyneither_re={1,(void*)& Cyc_Toc__tag_dyneither_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={0,(void*)& Cyc_Toc__tag_dyneither_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;static char _tmp55[20]="_init_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__init_dyneither_ptr_str={_tmp55,_tmp55,_tmp55 + 20};static struct _tuple1 Cyc_Toc__init_dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__init_dyneither_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__init_dyneither_ptr_bnd={0,& Cyc_Toc__init_dyneither_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__init_dyneither_ptr_re={1,(void*)& Cyc_Toc__init_dyneither_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__init_dyneither_ptr_ev={0,(void*)& Cyc_Toc__init_dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__init_dyneither_ptr_e=& Cyc_Toc__init_dyneither_ptr_ev;static char _tmp58[21]="_untag_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__untag_dyneither_ptr_str={_tmp58,_tmp58,_tmp58 + 21};static struct _tuple1 Cyc_Toc__untag_dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__untag_dyneither_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__untag_dyneither_ptr_bnd={0,& Cyc_Toc__untag_dyneither_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__untag_dyneither_ptr_re={1,(void*)& Cyc_Toc__untag_dyneither_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={0,(void*)& Cyc_Toc__untag_dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;static char _tmp5B[20]="_get_dyneither_size";
static struct _dyneither_ptr Cyc_Toc__get_dyneither_size_str={_tmp5B,_tmp5B,_tmp5B + 20};static struct _tuple1 Cyc_Toc__get_dyneither_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_dyneither_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_dyneither_size_bnd={0,& Cyc_Toc__get_dyneither_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_dyneither_size_re={1,(void*)& Cyc_Toc__get_dyneither_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_dyneither_size_ev={0,(void*)& Cyc_Toc__get_dyneither_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=& Cyc_Toc__get_dyneither_size_ev;static char _tmp5E[19]="_get_zero_arr_size";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_str={_tmp5E,_tmp5E,_tmp5E + 19};static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_bnd={0,& Cyc_Toc__get_zero_arr_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_re={1,(void*)& Cyc_Toc__get_zero_arr_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;static char _tmp61[24]="_get_zero_arr_size_char";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_char_str={_tmp61,_tmp61,_tmp61 + 24};static struct _tuple1 Cyc_Toc__get_zero_arr_size_char_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_char_bnd={0,& Cyc_Toc__get_zero_arr_size_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_char_re={1,(void*)& Cyc_Toc__get_zero_arr_size_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;static char _tmp64[25]="_get_zero_arr_size_short";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_short_str={_tmp64,_tmp64,_tmp64 + 25};static struct _tuple1 Cyc_Toc__get_zero_arr_size_short_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_short_bnd={0,& Cyc_Toc__get_zero_arr_size_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_short_re={1,(void*)& Cyc_Toc__get_zero_arr_size_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;static char _tmp67[23]="_get_zero_arr_size_int";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_int_str={_tmp67,_tmp67,_tmp67 + 23};static struct _tuple1 Cyc_Toc__get_zero_arr_size_int_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_int_bnd={0,& Cyc_Toc__get_zero_arr_size_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_int_re={1,(void*)& Cyc_Toc__get_zero_arr_size_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_int_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=& Cyc_Toc__get_zero_arr_size_int_ev;static char _tmp6A[25]="_get_zero_arr_size_float";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_float_str={_tmp6A,_tmp6A,_tmp6A + 25};static struct _tuple1 Cyc_Toc__get_zero_arr_size_float_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_float_bnd={0,& Cyc_Toc__get_zero_arr_size_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_float_re={1,(void*)& Cyc_Toc__get_zero_arr_size_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_float_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=& Cyc_Toc__get_zero_arr_size_float_ev;static char _tmp6D[26]="_get_zero_arr_size_double";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_double_str={_tmp6D,_tmp6D,_tmp6D + 26};static struct _tuple1 Cyc_Toc__get_zero_arr_size_double_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_double_bnd={0,& Cyc_Toc__get_zero_arr_size_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_double_re={1,(void*)& Cyc_Toc__get_zero_arr_size_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;static char _tmp70[30]="_get_zero_arr_size_longdouble";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_longdouble_str={_tmp70,_tmp70,_tmp70 + 30};static struct _tuple1 Cyc_Toc__get_zero_arr_size_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_longdouble_bnd={0,& Cyc_Toc__get_zero_arr_size_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_longdouble_re={1,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=& Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _tmp73[28]="_get_zero_arr_size_voidstar";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_tmp73,_tmp73,_tmp73 + 28};static struct _tuple1 Cyc_Toc__get_zero_arr_size_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_voidstar_bnd={0,& Cyc_Toc__get_zero_arr_size_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_voidstar_re={1,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;static char _tmp76[20]="_dyneither_ptr_plus";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_plus_str={_tmp76,_tmp76,_tmp76 + 20};static struct _tuple1 Cyc_Toc__dyneither_ptr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_plus_bnd={0,& Cyc_Toc__dyneither_ptr_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_plus_re={1,(void*)& Cyc_Toc__dyneither_ptr_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=& Cyc_Toc__dyneither_ptr_plus_ev;static char _tmp79[15]="_zero_arr_plus";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_str={_tmp79,_tmp79,_tmp79 + 15};static struct _tuple1 Cyc_Toc__zero_arr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_bnd={0,& Cyc_Toc__zero_arr_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_re={1,(void*)& Cyc_Toc__zero_arr_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)& Cyc_Toc__zero_arr_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static char _tmp7C[20]="_zero_arr_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_char_str={_tmp7C,_tmp7C,_tmp7C + 20};static struct _tuple1 Cyc_Toc__zero_arr_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_char_bnd={0,& Cyc_Toc__zero_arr_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_char_re={1,(void*)& Cyc_Toc__zero_arr_plus_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_plus_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=& Cyc_Toc__zero_arr_plus_char_ev;static char _tmp7F[21]="_zero_arr_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_short_str={_tmp7F,_tmp7F,_tmp7F + 21};static struct _tuple1 Cyc_Toc__zero_arr_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_short_bnd={0,& Cyc_Toc__zero_arr_plus_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_short_re={1,(void*)& Cyc_Toc__zero_arr_plus_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_plus_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=& Cyc_Toc__zero_arr_plus_short_ev;static char _tmp82[19]="_zero_arr_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_int_str={_tmp82,_tmp82,_tmp82 + 19};static struct _tuple1 Cyc_Toc__zero_arr_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_int_bnd={0,& Cyc_Toc__zero_arr_plus_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_int_re={1,(void*)& Cyc_Toc__zero_arr_plus_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_plus_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=& Cyc_Toc__zero_arr_plus_int_ev;static char _tmp85[21]="_zero_arr_plus_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_float_str={_tmp85,_tmp85,_tmp85 + 21};static struct _tuple1 Cyc_Toc__zero_arr_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_float_bnd={0,& Cyc_Toc__zero_arr_plus_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_float_re={1,(void*)& Cyc_Toc__zero_arr_plus_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_plus_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=& Cyc_Toc__zero_arr_plus_float_ev;static char _tmp88[22]="_zero_arr_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_double_str={_tmp88,_tmp88,_tmp88 + 22};static struct _tuple1 Cyc_Toc__zero_arr_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_double_bnd={0,& Cyc_Toc__zero_arr_plus_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_double_re={1,(void*)& Cyc_Toc__zero_arr_plus_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_plus_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=& Cyc_Toc__zero_arr_plus_double_ev;static char _tmp8B[26]="_zero_arr_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_longdouble_str={_tmp8B,_tmp8B,_tmp8B + 26};static struct _tuple1 Cyc_Toc__zero_arr_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_longdouble_bnd={0,& Cyc_Toc__zero_arr_plus_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_longdouble_re={1,(void*)& Cyc_Toc__zero_arr_plus_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_plus_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;static char _tmp8E[24]="_zero_arr_plus_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_voidstar_str={_tmp8E,_tmp8E,_tmp8E + 24};static struct _tuple1 Cyc_Toc__zero_arr_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_voidstar_bnd={0,& Cyc_Toc__zero_arr_plus_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_voidstar_re={1,(void*)& Cyc_Toc__zero_arr_plus_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_plus_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;static char _tmp91[28]="_dyneither_ptr_inplace_plus";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_str={_tmp91,_tmp91,_tmp91 + 28};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_inplace_plus_bnd={0,& Cyc_Toc__dyneither_ptr_inplace_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_re={1,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;static char _tmp94[23]="_zero_arr_inplace_plus";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_str={_tmp94,_tmp94,_tmp94 + 23};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;static char _tmp97[28]="_zero_arr_inplace_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={_tmp97,_tmp97,_tmp97 + 28};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_char_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_char_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;static char _tmp9A[29]="_zero_arr_inplace_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_short_str={_tmp9A,_tmp9A,_tmp9A + 29};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_short_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_short_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=& Cyc_Toc__zero_arr_inplace_plus_short_ev;static char _tmp9D[27]="_zero_arr_inplace_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={_tmp9D,_tmp9D,_tmp9D + 27};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_int_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_int_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;static char _tmpA0[29]="_zero_arr_inplace_plus_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_float_str={_tmpA0,_tmpA0,_tmpA0 + 29};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_float_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_float_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=& Cyc_Toc__zero_arr_inplace_plus_float_ev;static char _tmpA3[30]="_zero_arr_inplace_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_double_str={_tmpA3,_tmpA3,_tmpA3 + 30};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_double_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_double_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=& Cyc_Toc__zero_arr_inplace_plus_double_ev;static char _tmpA6[34]="_zero_arr_inplace_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_tmpA6,_tmpA6,_tmpA6 + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;static char _tmpA9[32]="_zero_arr_inplace_plus_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_tmpA9,_tmpA9,_tmpA9 + 32};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;static char _tmpAC[33]="_dyneither_ptr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmpAC,_tmpAC,_tmpAC + 33};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_bnd={0,& Cyc_Toc__dyneither_ptr_inplace_plus_post_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=& Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;static char _tmpAF[28]="_zero_arr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={_tmpAF,_tmpAF,_tmpAF + 28};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;static char _tmpB2[33]="_zero_arr_inplace_plus_post_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_tmpB2,_tmpB2,_tmpB2 + 33};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_char_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_char_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=& Cyc_Toc__zero_arr_inplace_plus_post_char_ev;static char _tmpB5[34]="_zero_arr_inplace_plus_post_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_tmpB5,_tmpB5,_tmpB5 + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_short_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_short_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=& Cyc_Toc__zero_arr_inplace_plus_post_short_ev;static char _tmpB8[32]="_zero_arr_inplace_plus_post_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_tmpB8,_tmpB8,_tmpB8 + 32};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_int_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_int_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=& Cyc_Toc__zero_arr_inplace_plus_post_int_ev;static char _tmpBB[34]="_zero_arr_inplace_plus_post_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_tmpBB,_tmpBB,_tmpBB + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_float_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_float_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=& Cyc_Toc__zero_arr_inplace_plus_post_float_ev;static char _tmpBE[35]="_zero_arr_inplace_plus_post_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={_tmpBE,_tmpBE,_tmpBE + 35};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_double_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_double_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;static char _tmpC1[39]="_zero_arr_inplace_plus_post_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_tmpC1,_tmpC1,_tmpC1 + 39};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;static char _tmpC4[37]="_zero_arr_inplace_plus_post_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_tmpC4,_tmpC4,_tmpC4 + 37};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;static char _tmpC7[10]="_cycalloc";
static struct _dyneither_ptr Cyc_Toc__cycalloc_str={_tmpC7,_tmpC7,_tmpC7 + 10};static struct _tuple1 Cyc_Toc__cycalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_bnd={0,& Cyc_Toc__cycalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_re={1,(void*)& Cyc_Toc__cycalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={0,(void*)& Cyc_Toc__cycalloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char _tmpCA[11]="_cyccalloc";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_str={_tmpCA,_tmpCA,_tmpCA + 11};static struct _tuple1 Cyc_Toc__cyccalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_bnd={0,& Cyc_Toc__cyccalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_re={1,(void*)& Cyc_Toc__cyccalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)& Cyc_Toc__cyccalloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static char _tmpCD[17]="_cycalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cycalloc_atomic_str={_tmpCD,_tmpCD,_tmpCD + 17};static struct _tuple1 Cyc_Toc__cycalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_atomic_bnd={0,& Cyc_Toc__cycalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_atomic_re={1,(void*)& Cyc_Toc__cycalloc_atomic_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)& Cyc_Toc__cycalloc_atomic_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;static char _tmpD0[18]="_cyccalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_atomic_str={_tmpD0,_tmpD0,_tmpD0 + 18};static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_atomic_bnd={0,& Cyc_Toc__cyccalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_atomic_re={1,(void*)& Cyc_Toc__cyccalloc_atomic_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)& Cyc_Toc__cyccalloc_atomic_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;static char _tmpD3[15]="_region_malloc";
static struct _dyneither_ptr Cyc_Toc__region_malloc_str={_tmpD3,_tmpD3,_tmpD3 + 15};static struct _tuple1 Cyc_Toc__region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_malloc_bnd={0,& Cyc_Toc__region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_malloc_re={1,(void*)& Cyc_Toc__region_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)& Cyc_Toc__region_malloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;static char _tmpD6[15]="_region_calloc";
static struct _dyneither_ptr Cyc_Toc__region_calloc_str={_tmpD6,_tmpD6,_tmpD6 + 15};static struct _tuple1 Cyc_Toc__region_calloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_calloc_bnd={0,& Cyc_Toc__region_calloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_calloc_re={1,(void*)& Cyc_Toc__region_calloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)& Cyc_Toc__region_calloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static char _tmpD9[13]="_check_times";
static struct _dyneither_ptr Cyc_Toc__check_times_str={_tmpD9,_tmpD9,_tmpD9 + 13};static struct _tuple1 Cyc_Toc__check_times_pr={{.Loc_n={4,0}},& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_times_bnd={0,& Cyc_Toc__check_times_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_times_re={1,(void*)& Cyc_Toc__check_times_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)& Cyc_Toc__check_times_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=& Cyc_Toc__check_times_ev;static char _tmpDC[12]="_new_region";
static struct _dyneither_ptr Cyc_Toc__new_region_str={_tmpDC,_tmpDC,_tmpDC + 12};static struct _tuple1 Cyc_Toc__new_region_pr={{.Loc_n={4,0}},& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__new_region_bnd={0,& Cyc_Toc__new_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__new_region_re={1,(void*)& Cyc_Toc__new_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)& Cyc_Toc__new_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static char _tmpDF[13]="_push_region";
static struct _dyneither_ptr Cyc_Toc__push_region_str={_tmpDF,_tmpDF,_tmpDF + 13};static struct _tuple1 Cyc_Toc__push_region_pr={{.Loc_n={4,0}},& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_region_bnd={0,& Cyc_Toc__push_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_region_re={1,(void*)& Cyc_Toc__push_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)& Cyc_Toc__push_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;static char _tmpE2[12]="_pop_region";
static struct _dyneither_ptr Cyc_Toc__pop_region_str={_tmpE2,_tmpE2,_tmpE2 + 12};static struct _tuple1 Cyc_Toc__pop_region_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_region_bnd={0,& Cyc_Toc__pop_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_region_re={1,(void*)& Cyc_Toc__pop_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)& Cyc_Toc__pop_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static char _tmpE5[16]="_open_dynregion";
static struct _dyneither_ptr Cyc_Toc__open_dynregion_str={_tmpE5,_tmpE5,_tmpE5 + 16};static struct _tuple1 Cyc_Toc__open_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__open_dynregion_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__open_dynregion_bnd={0,& Cyc_Toc__open_dynregion_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__open_dynregion_re={1,(void*)& Cyc_Toc__open_dynregion_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__open_dynregion_ev={0,(void*)& Cyc_Toc__open_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__open_dynregion_e=& Cyc_Toc__open_dynregion_ev;static char _tmpE8[16]="_push_dynregion";
static struct _dyneither_ptr Cyc_Toc__push_dynregion_str={_tmpE8,_tmpE8,_tmpE8 + 16};static struct _tuple1 Cyc_Toc__push_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__push_dynregion_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_dynregion_bnd={0,& Cyc_Toc__push_dynregion_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_dynregion_re={1,(void*)& Cyc_Toc__push_dynregion_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_dynregion_ev={0,(void*)& Cyc_Toc__push_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_dynregion_e=& Cyc_Toc__push_dynregion_ev;static char _tmpEB[15]="_pop_dynregion";
static struct _dyneither_ptr Cyc_Toc__pop_dynregion_str={_tmpEB,_tmpEB,_tmpEB + 15};static struct _tuple1 Cyc_Toc__pop_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_dynregion_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_dynregion_bnd={0,& Cyc_Toc__pop_dynregion_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_dynregion_re={1,(void*)& Cyc_Toc__pop_dynregion_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_dynregion_ev={0,(void*)& Cyc_Toc__pop_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_dynregion_e=& Cyc_Toc__pop_dynregion_ev;static char _tmpEE[14]="_reset_region";
static struct _dyneither_ptr Cyc_Toc__reset_region_str={_tmpEE,_tmpEE,_tmpEE + 14};static struct _tuple1 Cyc_Toc__reset_region_pr={{.Loc_n={4,0}},& Cyc_Toc__reset_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__reset_region_bnd={0,& Cyc_Toc__reset_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__reset_region_re={1,(void*)& Cyc_Toc__reset_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__reset_region_ev={0,(void*)& Cyc_Toc__reset_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__reset_region_e=& Cyc_Toc__reset_region_ev;static char _tmpF1[19]="_throw_arraybounds";
static struct _dyneither_ptr Cyc_Toc__throw_arraybounds_str={_tmpF1,_tmpF1,_tmpF1 + 19};static struct _tuple1 Cyc_Toc__throw_arraybounds_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_arraybounds_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_arraybounds_bnd={0,& Cyc_Toc__throw_arraybounds_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_arraybounds_re={1,(void*)& Cyc_Toc__throw_arraybounds_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(void*)& Cyc_Toc__throw_arraybounds_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;static char _tmpF4[29]="_dyneither_ptr_decrease_size";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_decrease_size_str={_tmpF4,_tmpF4,_tmpF4 + 29};static struct _tuple1 Cyc_Toc__dyneither_ptr_decrease_size_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_decrease_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_decrease_size_bnd={0,& Cyc_Toc__dyneither_ptr_decrease_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_decrease_size_re={1,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_decrease_size_ev={0,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=& Cyc_Toc__dyneither_ptr_decrease_size_ev;static char _tmpF7[11]="_swap_word";
static struct _dyneither_ptr Cyc_Toc__swap_word_str={_tmpF7,_tmpF7,_tmpF7 + 11};static struct _tuple1 Cyc_Toc__swap_word_pr={{.Loc_n={4,0}},& Cyc_Toc__swap_word_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__swap_word_bnd={0,& Cyc_Toc__swap_word_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__swap_word_re={1,(void*)& Cyc_Toc__swap_word_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__swap_word_ev={0,(void*)& Cyc_Toc__swap_word_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__swap_word_e=& Cyc_Toc__swap_word_ev;static char _tmpFA[16]="_swap_dyneither";
static struct _dyneither_ptr Cyc_Toc__swap_dyneither_str={_tmpFA,_tmpFA,_tmpFA + 16};static struct _tuple1 Cyc_Toc__swap_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__swap_dyneither_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__swap_dyneither_bnd={0,& Cyc_Toc__swap_dyneither_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__swap_dyneither_re={1,(void*)& Cyc_Toc__swap_dyneither_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__swap_dyneither_ev={0,(void*)& Cyc_Toc__swap_dyneither_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__swap_dyneither_e=& Cyc_Toc__swap_dyneither_ev;static char _tmpFD[13]="_throw_match";
static struct _dyneither_ptr Cyc_Toc__throw_match_str={_tmpFD,_tmpFD,_tmpFD + 13};static struct _tuple1 Cyc_Toc__throw_match_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_match_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_match_bnd={0,& Cyc_Toc__throw_match_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_match_re={1,(void*)& Cyc_Toc__throw_match_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_match_ev={0,(void*)& Cyc_Toc__throw_match_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_match_e=& Cyc_Toc__throw_match_ev;static char _tmp100[9]="_rethrow";
static struct _dyneither_ptr Cyc_Toc__rethrow_str={_tmp100,_tmp100,_tmp100 + 9};static struct _tuple1 Cyc_Toc__rethrow_pr={{.Loc_n={4,0}},& Cyc_Toc__rethrow_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__rethrow_bnd={0,& Cyc_Toc__rethrow_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__rethrow_re={1,(void*)& Cyc_Toc__rethrow_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__rethrow_ev={0,(void*)& Cyc_Toc__rethrow_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__rethrow_e=& Cyc_Toc__rethrow_ev;static char _tmp103[20]="_fast_region_malloc";
static struct _dyneither_ptr Cyc_Toc__fast_region_malloc_str={_tmp103,_tmp103,_tmp103 + 20};static struct _tuple1 Cyc_Toc__fast_region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__fast_region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fast_region_malloc_bnd={0,& Cyc_Toc__fast_region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fast_region_malloc_re={1,(void*)& Cyc_Toc__fast_region_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fast_region_malloc_ev={0,(void*)& Cyc_Toc__fast_region_malloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fast_region_malloc_e=& Cyc_Toc__fast_region_malloc_ev;
# 309
static struct Cyc_Absyn_AggrType_Absyn_Type_struct Cyc_Toc_dyneither_ptr_typ_v={11,{{.UnknownAggr={1,{Cyc_Absyn_StructA,& Cyc_Toc__dyneither_ptr_pr,0}}},0}};
# 312
static void*Cyc_Toc_dyneither_ptr_typ=(void*)& Cyc_Toc_dyneither_ptr_typ_v;
# 314
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0};
# 316
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
return Cyc_Absyn_skip_stmt(0);}
# 322
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
void*_tmp107=e->r;void*_tmp108=_tmp107;struct Cyc_Absyn_Exp*_tmp109;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp108)->tag == 13){if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp108)->f4 == Cyc_Absyn_No_coercion){_LL10: _tmp109=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp108)->f2;_LL11:
 return Cyc_Toc_cast_it(t,_tmp109);}else{goto _LL12;}}else{_LL12: _LL13:
 return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}_LLF:;}
# 328
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp10A=_cycalloc(sizeof(*_tmp10A));_tmp10A[0]=({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp10B;_tmp10B.tag=13;_tmp10B.f1=t;_tmp10B.f2=e;_tmp10B.f3=0;_tmp10B.f4=Cyc_Absyn_No_coercion;_tmp10B;});_tmp10A;});}
# 331
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp10C=_cycalloc(sizeof(*_tmp10C));_tmp10C[0]=({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmp10D;_tmp10D.tag=19;_tmp10D.f1=e;_tmp10D;});_tmp10C;});}
# 334
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp10E=_cycalloc(sizeof(*_tmp10E));_tmp10E[0]=({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmp10F;_tmp10F.tag=22;_tmp10F.f1=e1;_tmp10F.f2=e2;_tmp10F;});_tmp10E;});}
# 337
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
return(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp110=_cycalloc(sizeof(*_tmp110));_tmp110[0]=({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmp111;_tmp111.tag=36;_tmp111.f1=s;_tmp111;});_tmp110;});}
# 340
static void*Cyc_Toc_sizeoftyp_exp_r(void*t){
return(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112[0]=({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp113;_tmp113.tag=16;_tmp113.f1=t;_tmp113;});_tmp112;});}
# 343
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
return(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp114=_cycalloc(sizeof(*_tmp114));_tmp114[0]=({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp115;_tmp115.tag=9;_tmp115.f1=e;_tmp115.f2=es;_tmp115.f3=0;_tmp115.f4=1;_tmp115;});_tmp114;});}
# 346
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp116=_cycalloc(sizeof(*_tmp116));_tmp116[0]=({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmp117;_tmp117.tag=1;_tmp117.f1=e;_tmp117;});_tmp116;});}
# 349
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
return(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp118=_cycalloc(sizeof(*_tmp118));_tmp118[0]=({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp119;_tmp119.tag=2;_tmp119.f1=s1;_tmp119.f2=s2;_tmp119;});_tmp118;});}
# 352
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
return(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A[0]=({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp11B;_tmp11B.tag=5;_tmp11B.f1=e1;_tmp11B.f2=e2;_tmp11B.f3=e3;_tmp11B;});_tmp11A;});}
# 355
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C[0]=({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmp11D;_tmp11D.tag=20;_tmp11D.f1=e;_tmp11D.f2=n;_tmp11D.f3=0;_tmp11D.f4=0;_tmp11D;});_tmp11C;});}
# 358
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp11E=_cycalloc(sizeof(*_tmp11E));_tmp11E[0]=({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmp11F;_tmp11F.tag=21;_tmp11F.f1=e;_tmp11F.f2=n;_tmp11F.f3=0;_tmp11F.f4=0;_tmp11F;});_tmp11E;});}
# 361
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 364
return(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp120=_cycalloc(sizeof(*_tmp120));_tmp120[0]=({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp121;_tmp121.tag=35;_tmp121.f1=tdopt;_tmp121.f2=ds;_tmp121;});_tmp120;});}
# 366
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v){
return(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122[0]=({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmp123;_tmp123.tag=8;_tmp123.f1=v;_tmp123;});_tmp122;});}
# 369
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 373
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,unsigned int loc){
void*_tmp125=e->r;void*_tmp126=_tmp125;struct Cyc_Absyn_Exp*_tmp127;if(((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp126)->tag == 19){_LL15: _tmp127=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp126)->f1;_LL16:
 return Cyc_Absyn_aggrarrow_exp(_tmp127,f,loc);}else{_LL17: _LL18:
 return Cyc_Absyn_aggrmember_exp(e,f,loc);}_LL14:;}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 390
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
# 399
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 408
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
# 417
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
# 428
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
struct Cyc_Absyn_Exp*function;
{void*_tmp128=Cyc_Tcutil_compress(t);void*_tmp129=_tmp128;enum Cyc_Absyn_Size_of _tmp133;switch(*((int*)_tmp129)){case 6: _LL1A: _tmp133=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp129)->f2;_LL1B:
# 432
{enum Cyc_Absyn_Size_of _tmp12A=_tmp133;switch(_tmp12A){case Cyc_Absyn_Char_sz: _LL27: _LL28:
 function=fS->fchar;goto _LL26;case Cyc_Absyn_Short_sz: _LL29: _LL2A:
 function=fS->fshort;goto _LL26;case Cyc_Absyn_Int_sz: _LL2B: _LL2C:
 function=fS->fint;goto _LL26;default: _LL2D: _LL2E:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp12B=_cycalloc(sizeof(*_tmp12B));_tmp12B[0]=({struct Cyc_Core_Impossible_exn_struct _tmp12C;_tmp12C.tag=Cyc_Core_Impossible;_tmp12C.f1=({const char*_tmp12D="impossible IntType (not char, short or int)";_tag_dyneither(_tmp12D,sizeof(char),44);});_tmp12C;});_tmp12B;}));}_LL26:;}
# 438
goto _LL19;case 7: switch(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp129)->f1){case 0: _LL1C: _LL1D:
# 440
 function=fS->ffloat;
goto _LL19;case 1: _LL1E: _LL1F:
# 443
 function=fS->fdouble;
goto _LL19;default: _LL20: _LL21:
# 446
 function=fS->flongdouble;
goto _LL19;}case 5: _LL22: _LL23:
# 449
 function=fS->fvoidstar;
goto _LL19;default: _LL24: _LL25:
# 452
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp12E=_cycalloc(sizeof(*_tmp12E));_tmp12E[0]=({struct Cyc_Core_Impossible_exn_struct _tmp12F;_tmp12F.tag=Cyc_Core_Impossible;_tmp12F.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp132;_tmp132.tag=0;_tmp132.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp130[1]={& _tmp132};Cyc_aprintf(({const char*_tmp131="impossible expression type %s (not int, float, double, or pointer)";_tag_dyneither(_tmp131,sizeof(char),67);}),_tag_dyneither(_tmp130,sizeof(void*),1));});});_tmp12F;});_tmp12E;}));}_LL19:;}
# 454
return function;}
# 456
struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
return Cyc_Toc_getFunctionType(fS,(void*)_check_null(arr->topt));}
# 459
struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmp134=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_tmp135=_tmp134;void*_tmp139;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp135)->tag == 5){_LL30: _tmp139=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp135)->f1).elt_typ;_LL31:
# 462
 return Cyc_Toc_getFunctionType(fS,_tmp139);}else{_LL32: _LL33:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp136=_cycalloc(sizeof(*_tmp136));_tmp136[0]=({struct Cyc_Core_Impossible_exn_struct _tmp137;_tmp137.tag=Cyc_Core_Impossible;_tmp137.f1=({const char*_tmp138="impossible type (not pointer)";_tag_dyneither(_tmp138,sizeof(char),30);});_tmp137;});_tmp136;}));}_LL2F:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 470
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp13A=e->r;void*_tmp13B=_tmp13A;struct Cyc_List_List*_tmp147;struct Cyc_List_List*_tmp146;struct Cyc_List_List*_tmp145;struct Cyc_List_List*_tmp144;struct Cyc_List_List*_tmp143;struct Cyc_Absyn_Exp*_tmp142;long long _tmp141;int _tmp140;short _tmp13F;struct _dyneither_ptr _tmp13E;char _tmp13D;switch(*((int*)_tmp13B)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp13B)->f1).Null_c).tag){case 2: _LL35: _tmp13D=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp13B)->f1).Char_c).val).f2;_LL36:
 return _tmp13D == '\000';case 3: _LL37: _tmp13E=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp13B)->f1).Wchar_c).val;_LL38: {
# 474
unsigned long _tmp13C=Cyc_strlen((struct _dyneither_ptr)_tmp13E);
int i=0;
if(_tmp13C >= 2  && *((const char*)_check_dyneither_subscript(_tmp13E,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp13E,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmp13E,sizeof(char),1))== 'x'  && _tmp13C >= 3) && *((const char*)_check_dyneither_subscript(_tmp13E,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp13C;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp13E,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 484
return 0;}}case 4: _LL39: _tmp13F=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp13B)->f1).Short_c).val).f2;_LL3A:
 return _tmp13F == 0;case 5: _LL3B: _tmp140=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp13B)->f1).Int_c).val).f2;_LL3C:
 return _tmp140 == 0;case 6: _LL3D: _tmp141=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp13B)->f1).LongLong_c).val).f2;_LL3E:
 return _tmp141 == 0;case 1: _LL3F: _LL40:
 return 1;default: goto _LL4D;}case 13: _LL41: _tmp142=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp13B)->f2;_LL42:
 return Cyc_Toc_is_zero(_tmp142);case 23: _LL43: _tmp143=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp13B)->f1;_LL44:
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmp143);case 25: _LL45: _tmp144=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp13B)->f1;_LL46:
 _tmp145=_tmp144;goto _LL48;case 28: _LL47: _tmp145=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp13B)->f3;_LL48:
 _tmp146=_tmp145;goto _LL4A;case 24: _LL49: _tmp146=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp13B)->f2;_LL4A:
 _tmp147=_tmp146;goto _LL4C;case 35: _LL4B: _tmp147=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp13B)->f2;_LL4C:
# 495
 for(0;_tmp147 != 0;_tmp147=_tmp147->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple19*)_tmp147->hd)).f2))return 0;}
return 1;default: _LL4D: _LL4E:
 return 0;}_LL34:;}
# 503
static int Cyc_Toc_is_nullable(void*t){
void*_tmp148=Cyc_Tcutil_compress(t);void*_tmp149=_tmp148;union Cyc_Absyn_Constraint*_tmp14C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp149)->tag == 5){_LL50: _tmp14C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp149)->f1).ptr_atts).nullable;_LL51:
# 506
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp14C);}else{_LL52: _LL53:
({void*_tmp14A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp14B="is_nullable";_tag_dyneither(_tmp14B,sizeof(char),12);}),_tag_dyneither(_tmp14A,sizeof(void*),0));});}_LL4F:;}
# 512
static struct _dyneither_ptr Cyc_Toc_collapse_qvar(struct _dyneither_ptr s,struct _tuple1*x){
unsigned int slen=Cyc_strlen((struct _dyneither_ptr)s);
unsigned int len=9 + slen;
struct _tuple1*_tmp14D=x;union Cyc_Absyn_Nmspace _tmp160;struct _dyneither_ptr _tmp15F;_LL55: _tmp160=_tmp14D->f1;_tmp15F=*_tmp14D->f2;_LL56:;{
unsigned int vlen=Cyc_strlen((struct _dyneither_ptr)_tmp15F);
len +=1 + vlen;{
struct Cyc_List_List*nms;
{union Cyc_Absyn_Nmspace _tmp14E=_tmp160;struct Cyc_List_List*_tmp151;struct Cyc_List_List*_tmp150;struct Cyc_List_List*_tmp14F;switch((_tmp14E.Abs_n).tag){case 4: _LL58: _LL59:
 nms=0;goto _LL57;case 1: _LL5A: _tmp14F=(_tmp14E.Rel_n).val;_LL5B:
 nms=_tmp14F;goto _LL57;case 2: _LL5C: _tmp150=(_tmp14E.Abs_n).val;_LL5D:
 nms=_tmp150;goto _LL57;default: _LL5E: _tmp151=(_tmp14E.C_n).val;_LL5F:
 nms=_tmp151;goto _LL57;}_LL57:;}
# 525
{struct Cyc_List_List*_tmp152=nms;for(0;_tmp152 != 0;_tmp152=_tmp152->tl){
len +=1 + Cyc_strlen((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp152->hd));}}{
struct _dyneither_ptr buf=({unsigned int _tmp15D=len;char*_tmp15E=_cyccalloc_atomic(sizeof(char),_tmp15D);_tag_dyneither(_tmp15E,sizeof(char),_tmp15D);});
struct _dyneither_ptr p=buf;
Cyc_strcpy(p,(struct _dyneither_ptr)s);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)slen);
for(0;nms != 0;nms=nms->tl){
struct _dyneither_ptr*_tmp153=(struct _dyneither_ptr*)nms->hd;struct _dyneither_ptr*_tmp154=_tmp153;struct _dyneither_ptr _tmp158;_LL61: _tmp158=*_tmp154;_LL62:;
({struct _dyneither_ptr _tmp155=p;char _tmp156=*((char*)_check_dyneither_subscript(_tmp155,sizeof(char),0));char _tmp157='_';if(_get_dyneither_size(_tmp155,sizeof(char))== 1  && (_tmp156 == '\000'  && _tmp157 != '\000'))_throw_arraybounds();*((char*)_tmp155.curr)=_tmp157;});_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp158);
_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)Cyc_strlen((struct _dyneither_ptr)_tmp158));}
# 536
({struct _dyneither_ptr _tmp159=p;char _tmp15A=*((char*)_check_dyneither_subscript(_tmp159,sizeof(char),0));char _tmp15B='_';if(_get_dyneither_size(_tmp159,sizeof(char))== 1  && (_tmp15A == '\000'  && _tmp15B != '\000'))_throw_arraybounds();*((char*)_tmp159.curr)=_tmp15B;});_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp15F);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)vlen);
Cyc_strcpy(p,({const char*_tmp15C="_struct";_tag_dyneither(_tmp15C,sizeof(char),8);}));
return(struct _dyneither_ptr)buf;};};};}struct _tuple20{struct Cyc_Toc_TocState*f1;struct _tuple14*f2;};static char _tmp176[8]="*bogus*";
# 548
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple20*env){
# 551
static struct _dyneither_ptr bogus_string={_tmp176,_tmp176,_tmp176 + 8};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
static struct _tuple14 pair={& bogus_qvar,& bogus_qvar};
# 555
struct _tuple20 _tmp161=*env;struct _tuple20 _tmp162=_tmp161;struct Cyc_Dict_Dict*_tmp175;struct _tuple14*_tmp174;_LL64: _tmp175=(_tmp162.f1)->qvar_tags;_tmp174=_tmp162.f2;_LL65:;{
struct _tuple14 _tmp163=*_tmp174;struct _tuple14 _tmp164=_tmp163;struct _tuple1*_tmp173;struct _tuple1*_tmp172;_LL67: _tmp173=_tmp164.f1;_tmp172=_tmp164.f2;_LL68:;{
struct _handler_cons _tmp165;_push_handler(& _tmp165);{int _tmp167=0;if(setjmp(_tmp165.handler))_tmp167=1;if(!_tmp167){
{struct _tuple1*_tmp168=((struct _tuple1*(*)(struct Cyc_Dict_Dict d,int(*cmp)(struct _tuple14*,struct _tuple14*),struct _tuple14*k))Cyc_Dict_lookup_other)(*_tmp175,Cyc_Toc_qvar_tag_cmp,_tmp174);_npop_handler(0);return _tmp168;};_pop_handler();}else{void*_tmp166=(void*)_exn_thrown;void*_tmp169=_tmp166;void*_tmp171;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp169)->tag == Cyc_Dict_Absent){_LL6A: _LL6B: {
# 561
struct _tuple14*_tmp16A=({struct _tuple14*_tmp170=_cycalloc(sizeof(*_tmp170));_tmp170->f1=_tmp173;_tmp170->f2=_tmp172;_tmp170;});
struct _tuple1*_tmp16B=_tmp173;union Cyc_Absyn_Nmspace _tmp16F;struct _dyneither_ptr _tmp16E;_LL6F: _tmp16F=_tmp16B->f1;_tmp16E=*_tmp16B->f2;_LL70:;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmp16E,_tmp172);
struct _tuple1*res=({struct _tuple1*_tmp16C=_cycalloc(sizeof(*_tmp16C));_tmp16C->f1=_tmp16F;_tmp16C->f2=({struct _dyneither_ptr*_tmp16D=_cycalloc(sizeof(*_tmp16D));_tmp16D[0]=newvar;_tmp16D;});_tmp16C;});
*_tmp175=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmp175,_tmp16A,res);
return res;};}}else{_LL6C: _tmp171=_tmp169;_LL6D:(int)_rethrow(_tmp171);}_LL69:;}};};};}
# 570
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple14 env=({struct _tuple14 _tmp177;_tmp177.f1=fieldname;_tmp177.f2=dtname;_tmp177;});
return((struct _tuple1*(*)(struct _tuple14*arg,struct _tuple1*(*f)(struct _RegionHandle*,struct _tuple20*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_collapse_qvars_body);}
# 575
static void*Cyc_Toc_typ_to_c(void*t);struct _tuple21{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple22{void*f1;struct Cyc_List_List*f2;};
# 579
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple21*env){
# 582
struct _tuple21 _tmp178=*env;struct _tuple21 _tmp179=_tmp178;struct Cyc_List_List**_tmp191;struct Cyc_List_List*_tmp190;_LL72: _tmp191=(_tmp179.f1)->tuple_types;_tmp190=_tmp179.f2;_LL73:;
# 584
{struct Cyc_List_List*_tmp17A=*_tmp191;for(0;_tmp17A != 0;_tmp17A=_tmp17A->tl){
struct _tuple22*_tmp17B=(struct _tuple22*)_tmp17A->hd;struct _tuple22*_tmp17C=_tmp17B;void*_tmp17F;struct Cyc_List_List*_tmp17E;_LL75: _tmp17F=_tmp17C->f1;_tmp17E=_tmp17C->f2;_LL76:;{
struct Cyc_List_List*_tmp17D=_tmp190;
for(0;_tmp17D != 0  && _tmp17E != 0;(_tmp17D=_tmp17D->tl,_tmp17E=_tmp17E->tl)){
if(!Cyc_Tcutil_unify((*((struct _tuple11*)_tmp17D->hd)).f2,(void*)_tmp17E->hd))
break;}
if(_tmp17D == 0  && _tmp17E == 0)
return _tmp17F;};}}{
# 594
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmp18C=_cycalloc(sizeof(*_tmp18C));_tmp18C[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp18F;_tmp18F.tag=1;_tmp18F.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp18D[1]={& _tmp18F};Cyc_aprintf(({const char*_tmp18E="_tuple%d";_tag_dyneither(_tmp18E,sizeof(char),9);}),_tag_dyneither(_tmp18D,sizeof(void*),1));});});_tmp18C;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,_tmp190);
struct Cyc_List_List*_tmp180=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
_tmp180=({struct Cyc_List_List*_tmp181=_cycalloc(sizeof(*_tmp181));_tmp181->hd=({struct Cyc_Absyn_Aggrfield*_tmp182=_cycalloc(sizeof(*_tmp182));_tmp182->name=Cyc_Absyn_fieldname(i);_tmp182->tq=Cyc_Toc_mt_tq;_tmp182->type=(void*)ts2->hd;_tmp182->width=0;_tmp182->attributes=0;_tmp182->requires_clause=0;_tmp182;});_tmp181->tl=_tmp180;_tmp181;});}}
_tmp180=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp180);{
struct Cyc_Absyn_Aggrdecl*_tmp183=({struct Cyc_Absyn_Aggrdecl*_tmp189=_cycalloc(sizeof(*_tmp189));_tmp189->kind=Cyc_Absyn_StructA;_tmp189->sc=Cyc_Absyn_Public;_tmp189->name=({struct _tuple1*_tmp18B=_cycalloc(sizeof(*_tmp18B));_tmp18B->f1=
Cyc_Absyn_Rel_n(0);_tmp18B->f2=xname;_tmp18B;});_tmp189->tvs=0;_tmp189->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp18A=_cycalloc(sizeof(*_tmp18A));_tmp18A->exist_vars=0;_tmp18A->rgn_po=0;_tmp18A->fields=_tmp180;_tmp18A->tagged=0;_tmp18A;});_tmp189->attributes=0;_tmp189->expected_mem_kind=0;_tmp189;});
# 609
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp184=_cycalloc(sizeof(*_tmp184));_tmp184->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp185=_cycalloc(sizeof(*_tmp185));_tmp185[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp186;_tmp186.tag=5;_tmp186.f1=_tmp183;_tmp186;});_tmp185;}),0);_tmp184->tl=Cyc_Toc_result_decls;_tmp184;});
*_tmp191=({struct Cyc_List_List*_tmp187=_region_malloc(d,sizeof(*_tmp187));_tmp187->hd=({struct _tuple22*_tmp188=_region_malloc(d,sizeof(*_tmp188));_tmp188->f1=x;_tmp188->f2=ts;_tmp188;});_tmp187->tl=*_tmp191;_tmp187;});
return x;};};}
# 614
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return((void*(*)(struct Cyc_List_List*arg,void*(*f)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state)(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple23{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple24{struct Cyc_Toc_TocState*f1;struct _tuple23*f2;};struct _tuple25{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 623
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple24*env){
# 631
struct _tuple24 _tmp192=*env;struct _tuple24 _tmp193=_tmp192;struct Cyc_List_List**_tmp1BD;struct _tuple1*_tmp1BC;struct Cyc_List_List*_tmp1BB;struct Cyc_List_List*_tmp1BA;struct Cyc_List_List*_tmp1B9;_LL78: _tmp1BD=(_tmp193.f1)->abs_struct_types;_tmp1BC=(_tmp193.f2)->f1;_tmp1BB=(_tmp193.f2)->f2;_tmp1BA=(_tmp193.f2)->f3;_tmp1B9=(_tmp193.f2)->f4;_LL79:;
# 636
{struct Cyc_List_List*_tmp194=*_tmp1BD;for(0;_tmp194 != 0;_tmp194=_tmp194->tl){
struct _tuple25*_tmp195=(struct _tuple25*)_tmp194->hd;struct _tuple25*_tmp196=_tmp195;struct _tuple1*_tmp19E;struct Cyc_List_List*_tmp19D;void*_tmp19C;_LL7B: _tmp19E=_tmp196->f1;_tmp19D=_tmp196->f2;_tmp19C=_tmp196->f3;_LL7C:;
# 639
if(Cyc_Absyn_qvar_cmp(_tmp19E,_tmp1BC)== 0  && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1BA)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp19D)){
int okay=1;
{struct Cyc_List_List*_tmp197=_tmp1BA;for(0;_tmp197 != 0;(_tmp197=_tmp197->tl,_tmp19D=_tmp19D->tl)){
void*_tmp198=(void*)_tmp197->hd;
void*_tmp199=(void*)((struct Cyc_List_List*)_check_null(_tmp19D))->hd;
# 646
{struct Cyc_Absyn_Kind*_tmp19A=Cyc_Tcutil_typ_kind(_tmp198);struct Cyc_Absyn_Kind*_tmp19B=_tmp19A;switch(((struct Cyc_Absyn_Kind*)_tmp19B)->kind){case Cyc_Absyn_EffKind: _LL7E: _LL7F:
 goto _LL81;case Cyc_Absyn_RgnKind: _LL80: _LL81:
# 651
 continue;default: _LL82: _LL83:
# 654
 if(Cyc_Tcutil_unify(_tmp198,_tmp199) || Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(_tmp198),Cyc_Toc_typ_to_c(_tmp199)))
# 656
continue;
# 659
okay=0;
goto _LL7D;}_LL7D:;}
# 662
break;}}
# 664
if(okay)
# 666
return _tmp19C;}}}{
# 674
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmp1B5=_cycalloc(sizeof(*_tmp1B5));_tmp1B5[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1B8;_tmp1B8.tag=1;_tmp1B8.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp1B6[1]={& _tmp1B8};Cyc_aprintf(({const char*_tmp1B7="_tuple%d";_tag_dyneither(_tmp1B7,sizeof(char),9);}),_tag_dyneither(_tmp1B6,sizeof(void*),1));});});_tmp1B5;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp19F=0;
# 678
*_tmp1BD=({struct Cyc_List_List*_tmp1A0=_region_malloc(d,sizeof(*_tmp1A0));_tmp1A0->hd=({struct _tuple25*_tmp1A1=_region_malloc(d,sizeof(*_tmp1A1));_tmp1A1->f1=_tmp1BC;_tmp1A1->f2=_tmp1BA;_tmp1A1->f3=x;_tmp1A1;});_tmp1A0->tl=*_tmp1BD;_tmp1A0;});{
# 681
struct _RegionHandle _tmp1A2=_new_region("r");struct _RegionHandle*r=& _tmp1A2;_push_region(r);
{struct Cyc_List_List*_tmp1A3=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp1BB,_tmp1BA);
for(0;_tmp1B9 != 0;_tmp1B9=_tmp1B9->tl){
struct Cyc_Absyn_Aggrfield*_tmp1A4=(struct Cyc_Absyn_Aggrfield*)_tmp1B9->hd;
void*t=_tmp1A4->type;
struct Cyc_List_List*atts=_tmp1A4->attributes;
# 689
if(_tmp1B9->tl == 0  && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(t)))
atts=({struct Cyc_List_List*_tmp1A5=_cycalloc(sizeof(*_tmp1A5));_tmp1A5->hd=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp1A6=_cycalloc(sizeof(*_tmp1A6));_tmp1A6[0]=({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmp1A7;_tmp1A7.tag=6;_tmp1A7.f1=0;_tmp1A7;});_tmp1A6;});_tmp1A5->tl=atts;_tmp1A5;});
# 692
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp1A3,t));
# 695
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val))
t=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1A8=_cycalloc(sizeof(*_tmp1A8));_tmp1A8[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp1A9;_tmp1A9.tag=8;_tmp1A9.f1=({struct Cyc_Absyn_ArrayInfo _tmp1AA;_tmp1AA.elt_type=Cyc_Absyn_void_star_typ();_tmp1AA.tq=Cyc_Absyn_empty_tqual(0);_tmp1AA.num_elts=
Cyc_Absyn_uint_exp(0,0);_tmp1AA.zero_term=((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0);_tmp1AA.zt_loc=0;_tmp1AA;});_tmp1A9;});_tmp1A8;});
# 699
_tmp19F=({struct Cyc_List_List*_tmp1AB=_cycalloc(sizeof(*_tmp1AB));_tmp1AB->hd=({struct Cyc_Absyn_Aggrfield*_tmp1AC=_cycalloc(sizeof(*_tmp1AC));_tmp1AC->name=_tmp1A4->name;_tmp1AC->tq=Cyc_Toc_mt_tq;_tmp1AC->type=t;_tmp1AC->width=_tmp1A4->width;_tmp1AC->attributes=atts;_tmp1AC->requires_clause=0;_tmp1AC;});_tmp1AB->tl=_tmp19F;_tmp1AB;});}
# 701
_tmp19F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp19F);{
struct Cyc_Absyn_Aggrdecl*_tmp1AD=({struct Cyc_Absyn_Aggrdecl*_tmp1B2=_cycalloc(sizeof(*_tmp1B2));_tmp1B2->kind=Cyc_Absyn_StructA;_tmp1B2->sc=Cyc_Absyn_Public;_tmp1B2->name=({struct _tuple1*_tmp1B4=_cycalloc(sizeof(*_tmp1B4));_tmp1B4->f1=
Cyc_Absyn_Rel_n(0);_tmp1B4->f2=xname;_tmp1B4;});_tmp1B2->tvs=0;_tmp1B2->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp1B3=_cycalloc(sizeof(*_tmp1B3));_tmp1B3->exist_vars=0;_tmp1B3->rgn_po=0;_tmp1B3->fields=_tmp19F;_tmp1B3->tagged=0;_tmp1B3;});_tmp1B2->attributes=0;_tmp1B2->expected_mem_kind=0;_tmp1B2;});
# 709
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp1AE=_cycalloc(sizeof(*_tmp1AE));_tmp1AE->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp1AF=_cycalloc(sizeof(*_tmp1AF));_tmp1AF[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp1B0;_tmp1B0.tag=5;_tmp1B0.f1=_tmp1AD;_tmp1B0;});_tmp1AF;}),0);_tmp1AE->tl=Cyc_Toc_result_decls;_tmp1AE;});{
void*_tmp1B1=x;_npop_handler(0);return _tmp1B1;};};}
# 682
;_pop_region(r);};};}
# 713
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 717
struct _tuple23 env=({struct _tuple23 _tmp1BE;_tmp1BE.f1=struct_name;_tmp1BE.f2=type_vars;_tmp1BE.f3=type_args;_tmp1BE.f4=fields;_tmp1BE;});
return((void*(*)(struct _tuple23*arg,void*(*f)(struct _RegionHandle*,struct _tuple24*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 724
struct _tuple1*Cyc_Toc_temp_var(){
int _tmp1BF=Cyc_Toc_temp_var_counter ++;
struct _tuple1*res=({struct _tuple1*_tmp1C0=_cycalloc(sizeof(*_tmp1C0));_tmp1C0->f1=Cyc_Absyn_Loc_n;_tmp1C0->f2=({struct _dyneither_ptr*_tmp1C1=_cycalloc(sizeof(*_tmp1C1));_tmp1C1[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1C4;_tmp1C4.tag=1;_tmp1C4.f1=(unsigned int)_tmp1BF;({void*_tmp1C2[1]={& _tmp1C4};Cyc_aprintf(({const char*_tmp1C3="_tmp%X";_tag_dyneither(_tmp1C3,sizeof(char),7);}),_tag_dyneither(_tmp1C2,sizeof(void*),1));});});_tmp1C1;});_tmp1C0;});
return res;}struct _tuple26{struct Cyc_Toc_TocState*f1;int f2;};
# 732
static struct _dyneither_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple26*env){
struct _tuple26 _tmp1C5=*env;struct _tuple26 _tmp1C6=_tmp1C5;struct Cyc_Xarray_Xarray*_tmp1CE;_LL85: _tmp1CE=(_tmp1C6.f1)->temp_labels;_LL86:;{
int _tmp1C7=Cyc_Toc_fresh_label_counter ++;
if(_tmp1C7 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp1CE))
return((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp1CE,_tmp1C7);{
struct _dyneither_ptr*res=({struct _dyneither_ptr*_tmp1CA=_cycalloc(sizeof(*_tmp1CA));_tmp1CA[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1CD;_tmp1CD.tag=1;_tmp1CD.f1=(unsigned int)_tmp1C7;({void*_tmp1CB[1]={& _tmp1CD};Cyc_aprintf(({const char*_tmp1CC="_LL%X";_tag_dyneither(_tmp1CC,sizeof(char),6);}),_tag_dyneither(_tmp1CB,sizeof(void*),1));});});_tmp1CA;});
if(((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp1CE,res)!= _tmp1C7)
({void*_tmp1C8=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1C9="fresh_label: add_ind returned bad index...";_tag_dyneither(_tmp1C9,sizeof(char),43);}),_tag_dyneither(_tmp1C8,sizeof(void*),0));});
return res;};};}
# 743
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
return((struct _dyneither_ptr*(*)(int arg,struct _dyneither_ptr*(*f)(struct _RegionHandle*,struct _tuple26*)))Cyc_Toc_use_toc_state)(0,Cyc_Toc_fresh_label_body);}
# 750
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
struct Cyc_List_List*_tmp1CF=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp1CF))->hd)->name)!= 0){
++ ans;
_tmp1CF=_tmp1CF->tl;}
# 757
return Cyc_Absyn_uint_exp((unsigned int)ans,0);}
# 763
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_result_decls);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
# 768
struct _tuple9 _tmp1D0=*a;struct _tuple9 _tmp1D1=_tmp1D0;struct _dyneither_ptr*_tmp1D5;struct Cyc_Absyn_Tqual _tmp1D4;void*_tmp1D3;_LL88: _tmp1D5=_tmp1D1.f1;_tmp1D4=_tmp1D1.f2;_tmp1D3=_tmp1D1.f3;_LL89:;
return({struct _tuple9*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));_tmp1D2->f1=_tmp1D5;_tmp1D2->f2=_tmp1D4;_tmp1D2->f3=Cyc_Toc_typ_to_c(_tmp1D3);_tmp1D2;});}
# 771
static struct _tuple11*Cyc_Toc_typ_to_c_f(struct _tuple11*x){
struct _tuple11 _tmp1D6=*x;struct _tuple11 _tmp1D7=_tmp1D6;struct Cyc_Absyn_Tqual _tmp1DA;void*_tmp1D9;_LL8B: _tmp1DA=_tmp1D7.f1;_tmp1D9=_tmp1D7.f2;_LL8C:;
return({struct _tuple11*_tmp1D8=_cycalloc(sizeof(*_tmp1D8));_tmp1D8->f1=_tmp1DA;_tmp1D8->f2=Cyc_Toc_typ_to_c(_tmp1D9);_tmp1D8;});}
# 790 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp1DB=Cyc_Tcutil_compress(t);void*_tmp1DC=_tmp1DB;void*_tmp1E2;void*_tmp1E1;struct Cyc_Absyn_Tqual _tmp1E0;struct Cyc_Absyn_Exp*_tmp1DF;union Cyc_Absyn_Constraint*_tmp1DE;unsigned int _tmp1DD;switch(*((int*)_tmp1DC)){case 8: _LL8E: _tmp1E1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1DC)->f1).elt_type;_tmp1E0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1DC)->f1).tq;_tmp1DF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1DC)->f1).num_elts;_tmp1DE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1DC)->f1).zero_term;_tmp1DD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1DC)->f1).zt_loc;_LL8F:
# 793
 return Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(_tmp1E1),_tmp1E0);case 1: _LL90: _tmp1E2=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1DC)->f2;if(_tmp1E2 != 0){_LL91:
 return Cyc_Toc_typ_to_c_array(_tmp1E2);}else{goto _LL92;}default: _LL92: _LL93:
 return Cyc_Toc_typ_to_c(t);}_LL8D:;}
# 799
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){
# 801
return({struct Cyc_Absyn_Aggrfield*_tmp1E3=_cycalloc(sizeof(*_tmp1E3));_tmp1E3->name=f->name;_tmp1E3->tq=Cyc_Toc_mt_tq;_tmp1E3->type=
# 803
Cyc_Toc_typ_to_c(f->type);_tmp1E3->width=f->width;_tmp1E3->attributes=f->attributes;_tmp1E3->requires_clause=0;_tmp1E3;});}
# 808
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 810
return;}
# 813
static void*Cyc_Toc_char_star_typ(){
static void**cs=0;
if(cs == 0)
cs=({void**_tmp1E4=_cycalloc(sizeof(*_tmp1E4));_tmp1E4[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref);_tmp1E4;});
# 818
return*cs;}
# 820
static void*Cyc_Toc_rgn_typ(){
static void**r=0;
if(r == 0)
r=({void**_tmp1E5=_cycalloc(sizeof(*_tmp1E5));_tmp1E5[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq);_tmp1E5;});
# 825
return*r;}
# 827
static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;
if(r == 0)
r=({void**_tmp1E6=_cycalloc(sizeof(*_tmp1E6));_tmp1E6[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq);_tmp1E6;});
# 832
return*r;}
# 834
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp1E7=Cyc_Tcutil_compress(t);void*_tmp1E8=_tmp1E7;struct Cyc_Absyn_Tvar*_tmp1E9;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1E8)->tag == 2){_LL95: _tmp1E9=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1E8)->f1;_LL96:
# 837
 return Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tbk);}else{_LL97: _LL98:
 return 0;}_LL94:;}
# 841
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp1EA=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp1EB=_tmp1EA;if(((struct Cyc_Absyn_Kind*)_tmp1EB)->kind == Cyc_Absyn_AnyKind){_LL9A: _LL9B:
 return 1;}else{_LL9C: _LL9D:
 return 0;}_LL99:;}
# 848
static void*Cyc_Toc_typ_to_c(void*t){
void*_tmp1EC=t;struct Cyc_Absyn_Datatypedecl*_tmp237;void**_tmp236;struct Cyc_Absyn_Enumdecl*_tmp235;struct Cyc_Absyn_Aggrdecl*_tmp234;struct Cyc_Absyn_Exp*_tmp233;struct Cyc_Absyn_Exp*_tmp232;void*_tmp231;struct _tuple1*_tmp230;struct Cyc_List_List*_tmp22F;struct Cyc_Absyn_Typedefdecl*_tmp22E;void*_tmp22D;struct Cyc_List_List*_tmp22C;struct _tuple1*_tmp22B;union Cyc_Absyn_AggrInfoU _tmp22A;struct Cyc_List_List*_tmp229;enum Cyc_Absyn_AggrKind _tmp228;struct Cyc_List_List*_tmp227;struct Cyc_List_List*_tmp226;struct Cyc_Absyn_Tqual _tmp225;void*_tmp224;struct Cyc_List_List*_tmp223;int _tmp222;struct Cyc_Absyn_VarargInfo*_tmp221;struct Cyc_List_List*_tmp220;void*_tmp21F;struct Cyc_Absyn_Tqual _tmp21E;struct Cyc_Absyn_Exp*_tmp21D;unsigned int _tmp21C;void*_tmp21B;struct Cyc_Absyn_Tqual _tmp21A;union Cyc_Absyn_Constraint*_tmp219;struct Cyc_Absyn_Datatypedecl*_tmp218;struct Cyc_Absyn_Datatypefield*_tmp217;struct Cyc_Absyn_Tvar*_tmp216;void**_tmp215;switch(*((int*)_tmp1EC)){case 0: _LL9F: _LLA0:
 return t;case 1: _LLA1: _tmp215=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1EC)->f2;_LLA2:
# 852
 if(*_tmp215 == 0){
*_tmp215=Cyc_Absyn_sint_typ;
return Cyc_Absyn_sint_typ;}
# 856
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp215));case 2: _LLA3: _tmp216=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1EC)->f1;_LLA4:
# 858
 if((Cyc_Tcutil_tvar_kind(_tmp216,& Cyc_Tcutil_bk))->kind == Cyc_Absyn_AnyKind)
# 861
return(void*)& Cyc_Absyn_VoidType_val;else{
# 863
return Cyc_Absyn_void_star_typ();}case 3: _LLA5: _LLA6:
# 865
 return(void*)& Cyc_Absyn_VoidType_val;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1EC)->f1).field_info).KnownDatatypefield).tag == 2){_LLA7: _tmp218=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1EC)->f1).field_info).KnownDatatypefield).val).f1;_tmp217=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1EC)->f1).field_info).KnownDatatypefield).val).f2;_LLA8:
# 867
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp217->name,_tmp218->name));}else{_LLA9: _LLAA:
({void*_tmp1ED=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1EE="unresolved DatatypeFieldType";_tag_dyneither(_tmp1EE,sizeof(char),29);}),_tag_dyneither(_tmp1ED,sizeof(void*),0));});}case 5: _LLAB: _tmp21B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1EC)->f1).elt_typ;_tmp21A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1EC)->f1).elt_tq;_tmp219=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1EC)->f1).ptr_atts).bounds;_LLAC:
# 872
 _tmp21B=Cyc_Toc_typ_to_c(_tmp21B);{
void*_tmp1EF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp219);void*_tmp1F0=_tmp1EF;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1F0)->tag == 0){_LLE0: _LLE1:
 return Cyc_Toc_dyneither_ptr_typ;}else{_LLE2: _LLE3:
 return Cyc_Absyn_star_typ(_tmp21B,(void*)& Cyc_Absyn_HeapRgn_val,_tmp21A,Cyc_Absyn_false_conref);}_LLDF:;};case 6: _LLAD: _LLAE:
# 877
 goto _LLB0;case 7: _LLAF: _LLB0:
 return t;case 8: _LLB1: _tmp21F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1EC)->f1).elt_type;_tmp21E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1EC)->f1).tq;_tmp21D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1EC)->f1).num_elts;_tmp21C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1EC)->f1).zt_loc;_LLB2:
# 880
 return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c(_tmp21F),_tmp21E,_tmp21D,Cyc_Absyn_false_conref,_tmp21C);case 9: _LLB3: _tmp225=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EC)->f1).ret_tqual;_tmp224=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EC)->f1).ret_typ;_tmp223=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EC)->f1).args;_tmp222=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EC)->f1).c_varargs;_tmp221=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EC)->f1).cyc_varargs;_tmp220=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EC)->f1).attributes;_LLB4: {
# 886
struct Cyc_List_List*_tmp1F1=0;
for(0;_tmp220 != 0;_tmp220=_tmp220->tl){
void*_tmp1F2=(void*)_tmp220->hd;void*_tmp1F3=_tmp1F2;switch(*((int*)_tmp1F3)){case 4: _LLE5: _LLE6:
 goto _LLE8;case 5: _LLE7: _LLE8:
 goto _LLEA;case 19: _LLE9: _LLEA:
 continue;case 22: _LLEB: _LLEC:
 continue;case 21: _LLED: _LLEE:
 continue;case 20: _LLEF: _LLF0:
 continue;default: _LLF1: _LLF2:
 _tmp1F1=({struct Cyc_List_List*_tmp1F4=_cycalloc(sizeof(*_tmp1F4));_tmp1F4->hd=(void*)_tmp220->hd;_tmp1F4->tl=_tmp1F1;_tmp1F4;});goto _LLE4;}_LLE4:;}{
# 897
struct Cyc_List_List*_tmp1F5=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp223);
if(_tmp221 != 0){
# 900
void*_tmp1F6=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp221->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple9*_tmp1F7=({struct _tuple9*_tmp1F9=_cycalloc(sizeof(*_tmp1F9));_tmp1F9->f1=_tmp221->name;_tmp1F9->f2=_tmp221->tq;_tmp1F9->f3=_tmp1F6;_tmp1F9;});
_tmp1F5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1F5,({struct Cyc_List_List*_tmp1F8=_cycalloc(sizeof(*_tmp1F8));_tmp1F8->hd=_tmp1F7;_tmp1F8->tl=0;_tmp1F8;}));}
# 904
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1FA=_cycalloc(sizeof(*_tmp1FA));_tmp1FA[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1FB;_tmp1FB.tag=9;_tmp1FB.f1=({struct Cyc_Absyn_FnInfo _tmp1FC;_tmp1FC.tvars=0;_tmp1FC.effect=0;_tmp1FC.ret_tqual=_tmp225;_tmp1FC.ret_typ=Cyc_Toc_typ_to_c(_tmp224);_tmp1FC.args=_tmp1F5;_tmp1FC.c_varargs=_tmp222;_tmp1FC.cyc_varargs=0;_tmp1FC.rgn_po=0;_tmp1FC.attributes=_tmp1F1;_tmp1FC.requires_clause=0;_tmp1FC.requires_relns=0;_tmp1FC.ensures_clause=0;_tmp1FC.ensures_relns=0;_tmp1FC;});_tmp1FB;});_tmp1FA;});};}case 10: _LLB5: _tmp226=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1EC)->f1;_LLB6:
# 909
 _tmp226=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp226);
return Cyc_Toc_add_tuple_type(_tmp226);case 12: _LLB7: _tmp228=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1EC)->f1;_tmp227=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1EC)->f2;_LLB8:
# 914
 return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1FD=_cycalloc(sizeof(*_tmp1FD));_tmp1FD[0]=({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp1FE;_tmp1FE.tag=12;_tmp1FE.f1=_tmp228;_tmp1FE.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp227);_tmp1FE;});_tmp1FD;});case 11: _LLB9: _tmp22A=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1EC)->f1).aggr_info;_tmp229=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1EC)->f1).targs;_LLBA:
# 918
{union Cyc_Absyn_AggrInfoU _tmp1FF=_tmp22A;if((_tmp1FF.UnknownAggr).tag == 1){_LLF4: _LLF5:
 return t;}else{_LLF6: _LLF7:
 goto _LLF3;}_LLF3:;}{
# 922
struct Cyc_Absyn_Aggrdecl*_tmp200=Cyc_Absyn_get_known_aggrdecl(_tmp22A);
if(_tmp200->expected_mem_kind){
# 925
if(_tmp200->impl == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp204;_tmp204.tag=0;_tmp204.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 928
Cyc_Absynpp_qvar2string(_tmp200->name));({struct Cyc_String_pa_PrintArg_struct _tmp203;_tmp203.tag=0;_tmp203.f1=(struct _dyneither_ptr)(
# 927
_tmp200->kind == Cyc_Absyn_UnionA?({const char*_tmp205="union";_tag_dyneither(_tmp205,sizeof(char),6);}):({const char*_tmp206="struct";_tag_dyneither(_tmp206,sizeof(char),7);}));({void*_tmp201[2]={& _tmp203,& _tmp204};Cyc_Tcutil_warn(0,({const char*_tmp202="%s %s was never defined.";_tag_dyneither(_tmp202,sizeof(char),25);}),_tag_dyneither(_tmp201,sizeof(void*),2));});});});}
# 933
if(_tmp200->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp200->name,Cyc_Absyn_unionq_typ);{
struct Cyc_List_List*_tmp207=_tmp200->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp200->impl))->fields;
if(_tmp207 == 0)return Cyc_Toc_aggrdecl_type(_tmp200->name,Cyc_Absyn_strctq);
for(0;_tmp207->tl != 0;_tmp207=_tmp207->tl){;}{
void*_tmp208=((struct Cyc_Absyn_Aggrfield*)_tmp207->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp208))){
if(_tmp200->expected_mem_kind)
({struct Cyc_String_pa_PrintArg_struct _tmp20B;_tmp20B.tag=0;_tmp20B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp200->name));({void*_tmp209[1]={& _tmp20B};Cyc_Tcutil_warn(0,({const char*_tmp20A="struct %s ended up being abstract.";_tag_dyneither(_tmp20A,sizeof(char),35);}),_tag_dyneither(_tmp209,sizeof(void*),1));});});{
# 947
struct _RegionHandle _tmp20C=_new_region("r");struct _RegionHandle*r=& _tmp20C;_push_region(r);
{struct Cyc_List_List*_tmp20D=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp200->tvs,_tmp229);
void*_tmp20E=Cyc_Tcutil_rsubstitute(r,_tmp20D,_tmp208);
if(Cyc_Toc_is_abstract_type(_tmp20E)){void*_tmp20F=Cyc_Toc_aggrdecl_type(_tmp200->name,Cyc_Absyn_strctq);_npop_handler(0);return _tmp20F;}{
void*_tmp210=Cyc_Toc_add_struct_type(_tmp200->name,_tmp200->tvs,_tmp229,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp200->impl))->fields);_npop_handler(0);return _tmp210;};}
# 948
;_pop_region(r);};}
# 953
return Cyc_Toc_aggrdecl_type(_tmp200->name,Cyc_Absyn_strctq);};};};case 13: _LLBB: _tmp22B=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp1EC)->f1;_LLBC:
 return t;case 14: _LLBD: _tmp22C=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1EC)->f1;_LLBE:
 Cyc_Toc_enumfields_to_c(_tmp22C);return t;case 17: _LLBF: _tmp230=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1EC)->f1;_tmp22F=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1EC)->f2;_tmp22E=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1EC)->f3;_tmp22D=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1EC)->f4;_LLC0:
# 957
 if(_tmp22D == 0  || Cyc_noexpand_r){
if(_tmp22F != 0)
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp211=_cycalloc(sizeof(*_tmp211));_tmp211[0]=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp212;_tmp212.tag=17;_tmp212.f1=_tmp230;_tmp212.f2=0;_tmp212.f3=_tmp22E;_tmp212.f4=0;_tmp212;});_tmp211;});else{
return t;}}else{
# 962
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp213=_cycalloc(sizeof(*_tmp213));_tmp213[0]=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp214;_tmp214.tag=17;_tmp214.f1=_tmp230;_tmp214.f2=0;_tmp214.f3=_tmp22E;_tmp214.f4=Cyc_Toc_typ_to_c(_tmp22D);_tmp214;});_tmp213;});}case 19: _LLC1: _LLC2:
 return Cyc_Absyn_uint_typ;case 15: _LLC3: _tmp231=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1EC)->f1;_LLC4:
 return Cyc_Toc_rgn_typ();case 16: _LLC5: _LLC6:
 return Cyc_Toc_dyn_rgn_typ();case 20: _LLC7: _LLC8:
# 968
 goto _LLCA;case 21: _LLC9: _LLCA:
 goto _LLCC;case 22: _LLCB: _LLCC:
 goto _LLCE;case 23: _LLCD: _LLCE:
 goto _LLD0;case 24: _LLCF: _LLD0:
 goto _LLD2;case 25: _LLD1: _LLD2:
 return Cyc_Absyn_void_star_typ();case 18: _LLD3: _tmp232=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp1EC)->f1;_LLD4:
# 978
 return t;case 27: _LLD5: _tmp233=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp1EC)->f1;_LLD6:
# 980
 return t;case 28: _LLD7: _LLD8:
 return t;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1EC)->f1)->r)){case 0: _LLD9: _tmp234=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1EC)->f1)->r)->f1;_LLDA:
# 983
 Cyc_Toc_aggrdecl_to_c(_tmp234,1);
if(_tmp234->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp234->name,Cyc_Absyn_unionq_typ);else{
return Cyc_Toc_aggrdecl_type(_tmp234->name,Cyc_Absyn_strctq);}case 1: _LLDB: _tmp235=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1EC)->f1)->r)->f1;_LLDC:
# 988
 Cyc_Toc_enumdecl_to_c(_tmp235);
return t;default: _LLDD: _tmp237=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1EC)->f1)->r)->f1;_tmp236=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1EC)->f2;_LLDE:
# 991
 Cyc_Toc_datatypedecl_to_c(_tmp237);
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp236)));}}_LL9E:;}
# 996
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned int l){
void*_tmp238=t;void*_tmp23A;struct Cyc_Absyn_Tqual _tmp239;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp238)->tag == 8){_LLF9: _tmp23A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp238)->f1).elt_type;_tmp239=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp238)->f1).tq;_LLFA:
# 999
 return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp23A,(void*)& Cyc_Absyn_HeapRgn_val,_tmp239,Cyc_Absyn_false_conref),e);}else{_LLFB: _LLFC:
 return Cyc_Toc_cast_it(t,e);}_LLF8:;}
# 1006
static int Cyc_Toc_atomic_typ(void*t){
void*_tmp23B=Cyc_Tcutil_compress(t);void*_tmp23C=_tmp23B;struct Cyc_List_List*_tmp248;struct Cyc_Absyn_Datatypedecl*_tmp247;struct Cyc_Absyn_Datatypefield*_tmp246;struct Cyc_List_List*_tmp245;union Cyc_Absyn_AggrInfoU _tmp244;void*_tmp243;switch(*((int*)_tmp23C)){case 0: _LLFE: _LLFF:
 return 1;case 2: _LL100: _LL101:
 return 0;case 6: _LL102: _LL103:
 goto _LL105;case 13: _LL104: _LL105:
 goto _LL107;case 14: _LL106: _LL107:
 goto _LL109;case 7: _LL108: _LL109:
 goto _LL10B;case 9: _LL10A: _LL10B:
 goto _LL10D;case 19: _LL10C: _LL10D:
 return 1;case 8: _LL10E: _tmp243=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp23C)->f1).elt_type;_LL10F:
 return Cyc_Toc_atomic_typ(_tmp243);case 11: _LL110: _tmp244=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp23C)->f1).aggr_info;_LL111:
# 1023
{union Cyc_Absyn_AggrInfoU _tmp23D=_tmp244;if((_tmp23D.UnknownAggr).tag == 1){_LL123: _LL124:
 return 0;}else{_LL125: _LL126:
 goto _LL122;}_LL122:;}{
# 1027
struct Cyc_Absyn_Aggrdecl*_tmp23E=Cyc_Absyn_get_known_aggrdecl(_tmp244);
if(_tmp23E->impl == 0)
return 0;
{struct Cyc_List_List*_tmp23F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp23E->impl))->fields;for(0;_tmp23F != 0;_tmp23F=_tmp23F->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp23F->hd)->type))return 0;}}
return 1;};case 12: _LL112: _tmp245=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp23C)->f2;_LL113:
# 1034
 for(0;_tmp245 != 0;_tmp245=_tmp245->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp245->hd)->type))return 0;}
return 1;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp23C)->f1).field_info).KnownDatatypefield).tag == 2){_LL114: _tmp247=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp23C)->f1).field_info).KnownDatatypefield).val).f1;_tmp246=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp23C)->f1).field_info).KnownDatatypefield).val).f2;_LL115:
# 1038
 _tmp248=_tmp246->typs;goto _LL117;}else{goto _LL120;}case 10: _LL116: _tmp248=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp23C)->f1;_LL117:
# 1040
 for(0;_tmp248 != 0;_tmp248=_tmp248->tl){
if(!Cyc_Toc_atomic_typ((*((struct _tuple11*)_tmp248->hd)).f2))return 0;}
return 1;case 3: _LL118: _LL119:
# 1045
 goto _LL11B;case 5: _LL11A: _LL11B:
 goto _LL11D;case 16: _LL11C: _LL11D:
 goto _LL11F;case 15: _LL11E: _LL11F:
 return 0;default: _LL120: _LL121:
({struct Cyc_String_pa_PrintArg_struct _tmp242;_tmp242.tag=0;_tmp242.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp240[1]={& _tmp242};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp241="atomic_typ:  bad type %s";_tag_dyneither(_tmp241,sizeof(char),25);}),_tag_dyneither(_tmp240,sizeof(void*),1));});});}_LLFD:;}
# 1053
static int Cyc_Toc_is_void_star(void*t){
void*_tmp249=Cyc_Tcutil_compress(t);void*_tmp24A=_tmp249;void*_tmp24D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24A)->tag == 5){_LL128: _tmp24D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24A)->f1).elt_typ;_LL129: {
# 1056
void*_tmp24B=Cyc_Tcutil_compress(_tmp24D);void*_tmp24C=_tmp24B;if(((struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp24C)->tag == 0){_LL12D: _LL12E:
 return 1;}else{_LL12F: _LL130:
 return 0;}_LL12C:;}}else{_LL12A: _LL12B:
# 1060
 return 0;}_LL127:;}
# 1064
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1068
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1073
static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){
void*_tmp24E=Cyc_Tcutil_compress(t);void*_tmp24F=_tmp24E;struct Cyc_List_List*_tmp25B;union Cyc_Absyn_AggrInfoU _tmp25A;switch(*((int*)_tmp24F)){case 11: _LL132: _tmp25A=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp24F)->f1).aggr_info;_LL133: {
# 1076
struct Cyc_Absyn_Aggrdecl*_tmp250=Cyc_Absyn_get_known_aggrdecl(_tmp25A);
if(_tmp250->impl == 0)
({void*_tmp251=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp252="is_poly_field: type missing fields";_tag_dyneither(_tmp252,sizeof(char),35);}),_tag_dyneither(_tmp251,sizeof(void*),0));});
_tmp25B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp250->impl))->fields;goto _LL135;}case 12: _LL134: _tmp25B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp24F)->f2;_LL135: {
# 1081
struct Cyc_Absyn_Aggrfield*_tmp253=Cyc_Absyn_lookup_field(_tmp25B,f);
if(_tmp253 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp256;_tmp256.tag=0;_tmp256.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp254[1]={& _tmp256};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp255="is_poly_field: bad field %s";_tag_dyneither(_tmp255,sizeof(char),28);}),_tag_dyneither(_tmp254,sizeof(void*),1));});});
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp253->type);}default: _LL136: _LL137:
({struct Cyc_String_pa_PrintArg_struct _tmp259;_tmp259.tag=0;_tmp259.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp257[1]={& _tmp259};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp258="is_poly_field: bad type %s";_tag_dyneither(_tmp258,sizeof(char),27);}),_tag_dyneither(_tmp257,sizeof(void*),1));});});}_LL131:;}
# 1092
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp25C=e->r;void*_tmp25D=_tmp25C;struct Cyc_Absyn_Exp*_tmp267;struct _dyneither_ptr*_tmp266;struct Cyc_Absyn_Exp*_tmp265;struct _dyneither_ptr*_tmp264;switch(*((int*)_tmp25D)){case 20: _LL139: _tmp265=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp25D)->f1;_tmp264=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp25D)->f2;_LL13A:
# 1095
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp265->topt),_tmp264) && !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));case 21: _LL13B: _tmp267=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp25D)->f1;_tmp266=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp25D)->f2;_LL13C: {
# 1098
void*_tmp25E=Cyc_Tcutil_compress((void*)_check_null(_tmp267->topt));void*_tmp25F=_tmp25E;void*_tmp263;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp25F)->tag == 5){_LL140: _tmp263=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp25F)->f1).elt_typ;_LL141:
# 1100
 return Cyc_Toc_is_poly_field(_tmp263,_tmp266) && !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}else{_LL142: _LL143:
({struct Cyc_String_pa_PrintArg_struct _tmp262;_tmp262.tag=0;_tmp262.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp267->topt)));({void*_tmp260[1]={& _tmp262};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp261="is_poly_project: bad type %s";_tag_dyneither(_tmp261,sizeof(char),29);}),_tag_dyneither(_tmp260,sizeof(void*),1));});});}_LL13F:;}default: _LL13D: _LL13E:
# 1103
 return 0;}_LL138:;}
# 1108
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,({struct Cyc_List_List*_tmp268=_cycalloc(sizeof(*_tmp268));_tmp268->hd=s;_tmp268->tl=0;_tmp268;}),0);}
# 1112
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,({struct Cyc_List_List*_tmp269=_cycalloc(sizeof(*_tmp269));_tmp269->hd=s;_tmp269->tl=0;_tmp269;}),0);}
# 1116
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1122
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,({struct Cyc_Absyn_Exp*_tmp26A[2];_tmp26A[1]=s;_tmp26A[0]=rgn;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp26A,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}
# 1126
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return Cyc_Absyn_fncall_exp(Cyc_Toc__fast_region_malloc_e,({struct Cyc_Absyn_Exp*_tmp26B[2];_tmp26B[1]=s;_tmp26B[0]=rgn;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp26B,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}
# 1130
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type))
return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,({struct Cyc_Absyn_Exp*_tmp26C[2];_tmp26C[1]=n;_tmp26C[0]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp26C,sizeof(struct Cyc_Absyn_Exp*),2));}),0);else{
# 1134
return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,({struct Cyc_Absyn_Exp*_tmp26D[2];_tmp26D[1]=n;_tmp26D[0]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp26D,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}}
# 1137
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,({struct Cyc_Absyn_Exp*_tmp26E[3];_tmp26E[2]=n;_tmp26E[1]=s;_tmp26E[0]=rgn;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp26E,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}
# 1142
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,({struct Cyc_List_List*_tmp26F=_cycalloc(sizeof(*_tmp26F));_tmp26F->hd=e;_tmp26F->tl=0;_tmp26F;}),0);}
# 1145
static struct Cyc_Absyn_Exp*Cyc_Toc_newrethrow_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_fncall_exp(Cyc_Toc__rethrow_e,({struct Cyc_List_List*_tmp270=_cycalloc(sizeof(*_tmp270));_tmp270->hd=e;_tmp270->tl=0;_tmp270;}),0);}
# 1149
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,({struct Cyc_Absyn_Exp*_tmp271[0];((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp271,sizeof(struct Cyc_Absyn_Exp*),0));}),0),0);}
# 1155
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1163
int is_string=0;
{void*_tmp272=e->r;void*_tmp273=_tmp272;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp273)->tag == 0)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp273)->f1).Wstring_c).tag){case 8: _LL145: _LL146:
 is_string=1;goto _LL144;case 9: _LL147: _LL148:
 is_string=1;goto _LL144;default: goto _LL149;}else{_LL149: _LL14A:
 goto _LL144;}_LL144:;}{
# 1169
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
if(is_string){
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_conref,0);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp274=_cycalloc(sizeof(*_tmp274));_tmp274->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp275=_cycalloc(sizeof(*_tmp275));_tmp275[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp276;_tmp276.tag=0;_tmp276.f1=vd;_tmp276;});_tmp275;}),0);_tmp274->tl=Cyc_Toc_result_decls;_tmp274;});
xexp=Cyc_Absyn_var_exp(x,0);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{
# 1179
xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),e);
# 1181
xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
# 1183
struct Cyc_Absyn_Exp*urm_exp;
urm_exp=Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple19*_tmp277[3];_tmp277[2]=({struct _tuple19*_tmp27A=_cycalloc(sizeof(*_tmp27A));_tmp27A->f1=0;_tmp27A->f2=xplussz;_tmp27A;});_tmp277[1]=({struct _tuple19*_tmp279=_cycalloc(sizeof(*_tmp279));_tmp279->f1=0;_tmp279->f2=xexp;_tmp279;});_tmp277[0]=({struct _tuple19*_tmp278=_cycalloc(sizeof(*_tmp278));_tmp278->f1=0;_tmp278->f2=xexp;_tmp278;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp277,sizeof(struct _tuple19*),3));}),0);
# 1187
return urm_exp;};};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;};struct Cyc_Toc_StructInfo{struct Cyc_Absyn_Exp**varsizeexp;struct Cyc_Absyn_Exp*lhs_exp;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;int toplevel;int*in_lhs;struct Cyc_Toc_StructInfo struct_info;int*in_sizeof;struct _RegionHandle*rgn;};
# 1226 "toc.cyc"
typedef struct Cyc_Toc_Env*Cyc_Toc_env_t;
# 1228
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp27B=nv;int _tmp27C;_LL14C: _tmp27C=_tmp27B->toplevel;_LL14D:;
return _tmp27C;}
# 1232
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp27D=nv;int*_tmp27E;_LL14F: _tmp27E=_tmp27D->in_lhs;_LL150:;
return*_tmp27E;}
# 1238
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
return({struct Cyc_Toc_Env*_tmp27F=_region_malloc(r,sizeof(*_tmp27F));_tmp27F->break_lab=(struct _dyneither_ptr**)0;_tmp27F->continue_lab=(struct _dyneither_ptr**)0;_tmp27F->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0;_tmp27F->toplevel=(int)1;_tmp27F->in_lhs=(int*)({int*_tmp282=_region_malloc(r,sizeof(*_tmp282));_tmp282[0]=0;_tmp282;});_tmp27F->struct_info=(struct Cyc_Toc_StructInfo)({(_tmp27F->struct_info).varsizeexp=({struct Cyc_Absyn_Exp**_tmp281=_cycalloc(sizeof(*_tmp281));_tmp281[0]=0;_tmp281;});(_tmp27F->struct_info).lhs_exp=0;_tmp27F->struct_info;});_tmp27F->in_sizeof=(int*)({int*_tmp280=_region_malloc(r,sizeof(*_tmp280));_tmp280[0]=0;_tmp280;});_tmp27F->rgn=(struct _RegionHandle*)r;_tmp27F;});}
# 1249
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp283=e;struct _dyneither_ptr**_tmp28B;struct _dyneither_ptr**_tmp28A;struct Cyc_Toc_FallthruInfo*_tmp289;int _tmp288;int*_tmp287;struct Cyc_Toc_StructInfo _tmp286;int*_tmp285;_LL152: _tmp28B=_tmp283->break_lab;_tmp28A=_tmp283->continue_lab;_tmp289=_tmp283->fallthru_info;_tmp288=_tmp283->toplevel;_tmp287=_tmp283->in_lhs;_tmp286=_tmp283->struct_info;_tmp285=_tmp283->in_sizeof;_LL153:;
return({struct Cyc_Toc_Env*_tmp284=_region_malloc(r,sizeof(*_tmp284));_tmp284->break_lab=(struct _dyneither_ptr**)_tmp28B;_tmp284->continue_lab=(struct _dyneither_ptr**)_tmp28A;_tmp284->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp289;_tmp284->toplevel=(int)_tmp288;_tmp284->in_lhs=(int*)_tmp287;_tmp284->struct_info=(struct Cyc_Toc_StructInfo)_tmp286;_tmp284->in_sizeof=(int*)_tmp285;_tmp284->rgn=(struct _RegionHandle*)r;_tmp284;});}
# 1254
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp28C=e;struct _dyneither_ptr**_tmp294;struct _dyneither_ptr**_tmp293;struct Cyc_Toc_FallthruInfo*_tmp292;int _tmp291;int*_tmp290;struct Cyc_Toc_StructInfo _tmp28F;int*_tmp28E;_LL155: _tmp294=_tmp28C->break_lab;_tmp293=_tmp28C->continue_lab;_tmp292=_tmp28C->fallthru_info;_tmp291=_tmp28C->toplevel;_tmp290=_tmp28C->in_lhs;_tmp28F=_tmp28C->struct_info;_tmp28E=_tmp28C->in_sizeof;_LL156:;
return({struct Cyc_Toc_Env*_tmp28D=_region_malloc(r,sizeof(*_tmp28D));_tmp28D->break_lab=(struct _dyneither_ptr**)_tmp294;_tmp28D->continue_lab=(struct _dyneither_ptr**)_tmp293;_tmp28D->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp292;_tmp28D->toplevel=(int)0;_tmp28D->in_lhs=(int*)_tmp290;_tmp28D->struct_info=(struct Cyc_Toc_StructInfo)_tmp28F;_tmp28D->in_sizeof=(int*)_tmp28E;_tmp28D->rgn=(struct _RegionHandle*)r;_tmp28D;});}
# 1258
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp295=e;struct _dyneither_ptr**_tmp29D;struct _dyneither_ptr**_tmp29C;struct Cyc_Toc_FallthruInfo*_tmp29B;int _tmp29A;int*_tmp299;struct Cyc_Toc_StructInfo _tmp298;int*_tmp297;_LL158: _tmp29D=_tmp295->break_lab;_tmp29C=_tmp295->continue_lab;_tmp29B=_tmp295->fallthru_info;_tmp29A=_tmp295->toplevel;_tmp299=_tmp295->in_lhs;_tmp298=_tmp295->struct_info;_tmp297=_tmp295->in_sizeof;_LL159:;
return({struct Cyc_Toc_Env*_tmp296=_region_malloc(r,sizeof(*_tmp296));_tmp296->break_lab=(struct _dyneither_ptr**)_tmp29D;_tmp296->continue_lab=(struct _dyneither_ptr**)_tmp29C;_tmp296->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp29B;_tmp296->toplevel=(int)1;_tmp296->in_lhs=(int*)_tmp299;_tmp296->struct_info=(struct Cyc_Toc_StructInfo)_tmp298;_tmp296->in_sizeof=(int*)_tmp297;_tmp296->rgn=(struct _RegionHandle*)r;_tmp296;});}
# 1262
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp29E=e;int*_tmp29F;_LL15B: _tmp29F=_tmp29E->in_lhs;_LL15C:;
*_tmp29F=b;}
# 1266
static int Cyc_Toc_set_in_sizeof(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp2A0=e;int*_tmp2A2;_LL15E: _tmp2A2=_tmp2A0->in_sizeof;_LL15F:;{
int _tmp2A1=*_tmp2A2;
*_tmp2A2=b;
return _tmp2A1;};}
# 1272
static int Cyc_Toc_in_sizeof(struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp2A3=e;int*_tmp2A4;_LL161: _tmp2A4=_tmp2A3->in_sizeof;_LL162:;
return*_tmp2A4;}
# 1277
static struct Cyc_Toc_Env*Cyc_Toc_set_lhs_exp(struct _RegionHandle*r,struct Cyc_Absyn_Exp*exp,struct Cyc_Toc_Env*e){
# 1279
struct Cyc_Toc_Env*_tmp2A5=e;struct _dyneither_ptr**_tmp2AD;struct _dyneither_ptr**_tmp2AC;struct Cyc_Toc_FallthruInfo*_tmp2AB;int _tmp2AA;int*_tmp2A9;struct Cyc_Absyn_Exp**_tmp2A8;int*_tmp2A7;_LL164: _tmp2AD=_tmp2A5->break_lab;_tmp2AC=_tmp2A5->continue_lab;_tmp2AB=_tmp2A5->fallthru_info;_tmp2AA=_tmp2A5->toplevel;_tmp2A9=_tmp2A5->in_lhs;_tmp2A8=(_tmp2A5->struct_info).varsizeexp;_tmp2A7=_tmp2A5->in_sizeof;_LL165:;
return({struct Cyc_Toc_Env*_tmp2A6=_region_malloc(r,sizeof(*_tmp2A6));_tmp2A6->break_lab=(struct _dyneither_ptr**)_tmp2AD;_tmp2A6->continue_lab=(struct _dyneither_ptr**)_tmp2AC;_tmp2A6->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2AB;_tmp2A6->toplevel=(int)_tmp2AA;_tmp2A6->in_lhs=(int*)_tmp2A9;_tmp2A6->struct_info=(struct Cyc_Toc_StructInfo)({(_tmp2A6->struct_info).varsizeexp=_tmp2A8;(_tmp2A6->struct_info).lhs_exp=exp;_tmp2A6->struct_info;});_tmp2A6->in_sizeof=(int*)_tmp2A7;_tmp2A6->rgn=(struct _RegionHandle*)r;_tmp2A6;});}
# 1285
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp2AE=e;struct _dyneither_ptr**_tmp2B6;struct _dyneither_ptr**_tmp2B5;struct Cyc_Toc_FallthruInfo*_tmp2B4;int _tmp2B3;int*_tmp2B2;struct Cyc_Toc_StructInfo _tmp2B1;int*_tmp2B0;_LL167: _tmp2B6=_tmp2AE->break_lab;_tmp2B5=_tmp2AE->continue_lab;_tmp2B4=_tmp2AE->fallthru_info;_tmp2B3=_tmp2AE->toplevel;_tmp2B2=_tmp2AE->in_lhs;_tmp2B1=_tmp2AE->struct_info;_tmp2B0=_tmp2AE->in_sizeof;_LL168:;
return({struct Cyc_Toc_Env*_tmp2AF=_region_malloc(r,sizeof(*_tmp2AF));_tmp2AF->break_lab=(struct _dyneither_ptr**)0;_tmp2AF->continue_lab=(struct _dyneither_ptr**)0;_tmp2AF->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2B4;_tmp2AF->toplevel=(int)_tmp2B3;_tmp2AF->in_lhs=(int*)_tmp2B2;_tmp2AF->struct_info=(struct Cyc_Toc_StructInfo)_tmp2B1;_tmp2AF->in_sizeof=(int*)_tmp2B0;_tmp2AF->rgn=(struct _RegionHandle*)r;_tmp2AF;});}
# 1291
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders){
# 1295
struct Cyc_Toc_Env*_tmp2B7=e;struct _dyneither_ptr**_tmp2C1;struct _dyneither_ptr**_tmp2C0;struct Cyc_Toc_FallthruInfo*_tmp2BF;int _tmp2BE;int*_tmp2BD;struct Cyc_Toc_StructInfo _tmp2BC;int*_tmp2BB;_LL16A: _tmp2C1=_tmp2B7->break_lab;_tmp2C0=_tmp2B7->continue_lab;_tmp2BF=_tmp2B7->fallthru_info;_tmp2BE=_tmp2B7->toplevel;_tmp2BD=_tmp2B7->in_lhs;_tmp2BC=_tmp2B7->struct_info;_tmp2BB=_tmp2B7->in_sizeof;_LL16B:;{
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp2BA=_region_malloc(r,sizeof(*_tmp2BA));_tmp2BA->label=fallthru_l;_tmp2BA->binders=fallthru_binders;_tmp2BA;});
# 1298
return({struct Cyc_Toc_Env*_tmp2B8=_region_malloc(r,sizeof(*_tmp2B8));_tmp2B8->break_lab=(struct _dyneither_ptr**)({struct _dyneither_ptr**_tmp2B9=_region_malloc(r,sizeof(*_tmp2B9));_tmp2B9[0]=break_l;_tmp2B9;});_tmp2B8->continue_lab=(struct _dyneither_ptr**)_tmp2C0;_tmp2B8->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi;_tmp2B8->toplevel=(int)_tmp2BE;_tmp2B8->in_lhs=(int*)_tmp2BD;_tmp2B8->struct_info=(struct Cyc_Toc_StructInfo)_tmp2BC;_tmp2B8->in_sizeof=(int*)_tmp2BB;_tmp2B8->rgn=(struct _RegionHandle*)r;_tmp2B8;});};}
# 1301
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1303
struct Cyc_Toc_Env*_tmp2C2=e;struct _dyneither_ptr**_tmp2CB;struct _dyneither_ptr**_tmp2CA;struct Cyc_Toc_FallthruInfo*_tmp2C9;int _tmp2C8;int*_tmp2C7;struct Cyc_Toc_StructInfo _tmp2C6;int*_tmp2C5;_LL16D: _tmp2CB=_tmp2C2->break_lab;_tmp2CA=_tmp2C2->continue_lab;_tmp2C9=_tmp2C2->fallthru_info;_tmp2C8=_tmp2C2->toplevel;_tmp2C7=_tmp2C2->in_lhs;_tmp2C6=_tmp2C2->struct_info;_tmp2C5=_tmp2C2->in_sizeof;_LL16E:;
return({struct Cyc_Toc_Env*_tmp2C3=_region_malloc(r,sizeof(*_tmp2C3));_tmp2C3->break_lab=(struct _dyneither_ptr**)({struct _dyneither_ptr**_tmp2C4=_region_malloc(r,sizeof(*_tmp2C4));_tmp2C4[0]=break_l;_tmp2C4;});_tmp2C3->continue_lab=(struct _dyneither_ptr**)_tmp2CA;_tmp2C3->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0;_tmp2C3->toplevel=(int)_tmp2C8;_tmp2C3->in_lhs=(int*)_tmp2C7;_tmp2C3->struct_info=(struct Cyc_Toc_StructInfo)_tmp2C6;_tmp2C3->in_sizeof=(int*)_tmp2C5;_tmp2C3->rgn=(struct _RegionHandle*)r;_tmp2C3;});}
# 1310
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1313
struct Cyc_Toc_Env*_tmp2CC=e;struct _dyneither_ptr**_tmp2D5;struct _dyneither_ptr**_tmp2D4;struct Cyc_Toc_FallthruInfo*_tmp2D3;int _tmp2D2;int*_tmp2D1;struct Cyc_Toc_StructInfo _tmp2D0;int*_tmp2CF;_LL170: _tmp2D5=_tmp2CC->break_lab;_tmp2D4=_tmp2CC->continue_lab;_tmp2D3=_tmp2CC->fallthru_info;_tmp2D2=_tmp2CC->toplevel;_tmp2D1=_tmp2CC->in_lhs;_tmp2D0=_tmp2CC->struct_info;_tmp2CF=_tmp2CC->in_sizeof;_LL171:;
return({struct Cyc_Toc_Env*_tmp2CD=_region_malloc(r,sizeof(*_tmp2CD));_tmp2CD->break_lab=(struct _dyneither_ptr**)0;_tmp2CD->continue_lab=(struct _dyneither_ptr**)_tmp2D4;_tmp2CD->fallthru_info=(struct Cyc_Toc_FallthruInfo*)({struct Cyc_Toc_FallthruInfo*_tmp2CE=_region_malloc(r,sizeof(*_tmp2CE));_tmp2CE->label=next_l;_tmp2CE->binders=0;_tmp2CE;});_tmp2CD->toplevel=(int)_tmp2D2;_tmp2CD->in_lhs=(int*)_tmp2D1;_tmp2CD->struct_info=(struct Cyc_Toc_StructInfo)_tmp2D0;_tmp2CD->in_sizeof=(int*)_tmp2CF;_tmp2CD->rgn=(struct _RegionHandle*)r;_tmp2CD;});}
# 1326 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1329
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
# 1331
void*_tmp2D6=e->annot;void*_tmp2D7=_tmp2D6;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2D7)->tag == Cyc_CfFlowInfo_UnknownZ){_LL173: _LL174:
# 1334
 return Cyc_Toc_is_nullable((void*)_check_null(e->topt));}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2D7)->tag == Cyc_CfFlowInfo_NotZero){_LL175: _LL176:
# 1337
 return 0;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2D7)->tag == Cyc_CfFlowInfo_IsZero){_LL177: _LL178:
# 1339
({void*_tmp2D8=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp2D9="dereference of NULL pointer";_tag_dyneither(_tmp2D9,sizeof(char),28);}),_tag_dyneither(_tmp2D8,sizeof(void*),0));});
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2D7)->tag == Cyc_Absyn_EmptyAnnot){_LL179: _LL17A:
# 1345
 return 1;}else{_LL17B: _LL17C:
({void*_tmp2DA=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2DB="need_null_check";_tag_dyneither(_tmp2DB,sizeof(char),16);}),_tag_dyneither(_tmp2DA,sizeof(void*),0));});}}}}_LL172:;}
# 1350
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp2DC=e->annot;void*_tmp2DD=_tmp2DC;struct Cyc_List_List*_tmp2E3;struct Cyc_List_List*_tmp2E2;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2DD)->tag == Cyc_CfFlowInfo_UnknownZ){_LL17E: _tmp2E2=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2DD)->f1;_LL17F:
 return _tmp2E2;}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2DD)->tag == Cyc_CfFlowInfo_NotZero){_LL180: _tmp2E3=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2DD)->f1;_LL181:
 return _tmp2E3;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2DD)->tag == Cyc_CfFlowInfo_IsZero){_LL182: _LL183:
# 1355
({void*_tmp2DE=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp2DF="dereference of NULL pointer";_tag_dyneither(_tmp2DF,sizeof(char),28);}),_tag_dyneither(_tmp2DE,sizeof(void*),0));});
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2DD)->tag == Cyc_Absyn_EmptyAnnot){_LL184: _LL185:
 return 0;}else{_LL186: _LL187:
({void*_tmp2E0=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2E1="get_relns";_tag_dyneither(_tmp2E1,sizeof(char),10);}),_tag_dyneither(_tmp2E0,sizeof(void*),0));});}}}}_LL17D:;}static char _tmp2E7[8]="*bogus*";
# 1365
static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
# 1375 "toc.cyc"
union Cyc_Relations_RelnOp rop_i=Cyc_Relations_RConst(0);
int valid_rop_i=Cyc_Relations_exp2relnop(i,& rop_i);
# 1378
struct Cyc_Absyn_Vardecl*x;
{void*_tmp2E4=a->r;void*_tmp2E5=_tmp2E4;void*_tmp2E8;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E5)->tag == 1){_LL189: _tmp2E8=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E5)->f1;_LL18A: {
# 1381
struct Cyc_Absyn_Vardecl*_tmp2E6=Cyc_Tcutil_nonesc_vardecl(_tmp2E8);
if(_tmp2E6 == 0)goto _LL18C;
x=_tmp2E6;
goto _LL188;}}else{_LL18B: _LL18C: {
# 1387
static struct _dyneither_ptr bogus_string={_tmp2E7,_tmp2E7,_tmp2E7 + 8};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
# 1390
static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};
# 1396
x=& bogus_vardecl;
x->type=a_typ;}}_LL188:;}{
# 1399
void*_tmp2E9=a_typ;
union Cyc_Relations_RelnOp rop_a=Cyc_Relations_RNumelts(x);
# 1404
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound(_tmp2E9);
if(bound != 0){
union Cyc_Relations_RelnOp rop_bound=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(bound,& rop_bound))
relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_bound,Cyc_Relations_Rlte,rop_a,relns);}
# 1413
if(valid_rop_i){
struct Cyc_List_List*_tmp2EA=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlte,rop_i,relns);
# 1421
if(!Cyc_Relations_consistent_relations(_tmp2EA))return 1;}
# 1424
inner_loop: {
void*_tmp2EB=i->r;void*_tmp2EC=_tmp2EB;struct Cyc_Absyn_Exp*_tmp2F1;struct Cyc_Absyn_Exp*_tmp2F0;void*_tmp2EF;struct Cyc_Absyn_Exp*_tmp2EE;switch(*((int*)_tmp2EC)){case 13: _LL18E: _tmp2EF=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2EC)->f1;_tmp2EE=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2EC)->f2;_LL18F:
 i=_tmp2EE;goto inner_loop;case 2: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2EC)->f1 == Cyc_Absyn_Mod){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2EC)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2EC)->f2)->tl != 0){_LL190: _tmp2F1=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2EC)->f2)->hd;_tmp2F0=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2EC)->f2)->tl)->hd;_LL191: {
# 1430
union Cyc_Relations_RelnOp rop_z=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmp2F0,& rop_z)){
# 1433
struct Cyc_List_List*_tmp2ED=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlt,rop_z,relns);
# 1435
return !Cyc_Relations_consistent_relations(_tmp2ED);}
# 1437
goto _LL18D;}}else{goto _LL192;}}else{goto _LL192;}}else{goto _LL192;}default: _LL192: _LL193:
 goto _LL18D;}_LL18D:;}
# 1440
return 0;};}
# 1443
static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp2F2=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2F3="Missing type in primop ";_tag_dyneither(_tmp2F3,sizeof(char),24);}),_tag_dyneither(_tmp2F2,sizeof(void*),0));});
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));}
# 1447
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp2F4=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2F5="Missing type in primop ";_tag_dyneither(_tmp2F5,sizeof(char),24);}),_tag_dyneither(_tmp2F4,sizeof(void*),0));});
return(void*)_check_null(e->topt);}
# 1451
static struct _tuple11*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
return({struct _tuple11*_tmp2F6=_cycalloc(sizeof(*_tmp2F6));_tmp2F6->f1=Cyc_Toc_mt_tq;_tmp2F6->f2=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));_tmp2F6;});}
# 1454
static struct _tuple19*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
Cyc_Toc_exp_to_c(nv,e);
return({struct _tuple19*_tmp2F7=_cycalloc(sizeof(*_tmp2F7));_tmp2F7->f1=0;_tmp2F7->f2=e;_tmp2F7;});}
# 1461
static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic,int do_declare){
# 1464
struct Cyc_Absyn_Exp*eo;
void*t;
if(pointer){
struct Cyc_Absyn_Exp*_tmp2F8=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(x,0),0);
if(*(nv->struct_info).varsizeexp != 0){
_tmp2F8=Cyc_Absyn_add_exp(_tmp2F8,(struct Cyc_Absyn_Exp*)_check_null(*(nv->struct_info).varsizeexp),0);
*(nv->struct_info).varsizeexp=0;}
# 1472
t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);
if(rgnopt == 0  || Cyc_Absyn_no_regions)
eo=is_atomic?Cyc_Toc_malloc_atomic(_tmp2F8): Cyc_Toc_malloc_ptr(_tmp2F8);else{
# 1476
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);
eo=Cyc_Toc_rmalloc_exp(r,_tmp2F8);}}else{
# 1481
t=struct_typ;
eo=0;}
# 1484
if(do_declare){
void*_tmp2F9=x->r;void*_tmp2FA=_tmp2F9;void*_tmp2FD;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2FA)->tag == 1){_LL195: _tmp2FD=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2FA)->f1;_LL196:
 return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(Cyc_Absyn_binding2qvar(_tmp2FD),t,eo,s,0),0);}else{_LL197: _LL198:
({void*_tmp2FB=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2FC="bogus x to make_struct";_tag_dyneither(_tmp2FC,sizeof(char),23);}),_tag_dyneither(_tmp2FB,sizeof(void*),0));});}_LL194:;}else{
# 1491
if(eo != 0)
return Cyc_Absyn_stmt_exp(Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(x,eo,0),0),s,0),0);else{
# 1494
return Cyc_Absyn_stmt_exp(s,0);}}}
# 1498
static int Cyc_Toc_abstract_aggr(void*typ){
void*_tmp2FE=Cyc_Tcutil_compress(typ);void*_tmp2FF=_tmp2FE;struct Cyc_List_List*_tmp302;union Cyc_Absyn_AggrInfoU _tmp301;struct Cyc_List_List*_tmp300;switch(*((int*)_tmp2FF)){case 11: _LL19A: _tmp301=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2FF)->f1).aggr_info;_tmp300=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2FF)->f1).targs;_LL19B: {
# 1501
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(_tmp301);
if(ad->impl == 0)return 0;{
struct Cyc_List_List*aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
_tmp302=aggrfields;goto _LL19D;};}case 12: _LL19C: _tmp302=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2FF)->f2;_LL19D:
# 1506
 if(_tmp302 == 0)return 0;
return 1;default: _LL19E: _LL19F:
# 1512
 return 0;}_LL199:;}
# 1516
static void*Cyc_Toc_array_elt_type(void*t){
void*_tmp303=Cyc_Tcutil_compress(t);void*_tmp304=_tmp303;void*_tmp308;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp304)->tag == 8){_LL1A1: _tmp308=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp304)->f1).elt_type;_LL1A2:
 return _tmp308;}else{_LL1A3: _LL1A4:
({struct Cyc_String_pa_PrintArg_struct _tmp307;_tmp307.tag=0;_tmp307.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp305[1]={& _tmp307};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp306="array_elt_type called on %s";_tag_dyneither(_tmp306,sizeof(char),28);}),_tag_dyneither(_tmp305,sizeof(void*),1));});});}_LL1A0:;}
# 1523
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated);
# 1529
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s);
# 1534
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,void*array_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){
# 1539
void*orig_elt_type=Cyc_Toc_array_elt_type(array_type);
int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;
{struct Cyc_List_List*_tmp309=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;_tmp309 != 0;_tmp309=_tmp309->tl){
# 1543
struct _tuple19*_tmp30A=(struct _tuple19*)_tmp309->hd;struct _tuple19*_tmp30B=_tmp30A;struct Cyc_List_List*_tmp31D;struct Cyc_Absyn_Exp*_tmp31C;_LL1A6: _tmp31D=_tmp30B->f1;_tmp31C=_tmp30B->f2;_LL1A7:;{
# 1547
struct Cyc_Absyn_Exp*e_index;
if(_tmp31D == 0)
e_index=Cyc_Absyn_signed_int_exp(count --,0);else{
# 1551
if(_tmp31D->tl != 0)({void*_tmp30C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp30D="multiple designators in array";_tag_dyneither(_tmp30D,sizeof(char),30);}),_tag_dyneither(_tmp30C,sizeof(void*),0));});{
void*_tmp30E=(void*)_tmp31D->hd;
void*_tmp30F=_tmp30E;struct Cyc_Absyn_Exp*_tmp312;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp30F)->tag == 0){_LL1A9: _tmp312=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp30F)->f1;_LL1AA:
# 1555
 Cyc_Toc_exp_to_c(nv,_tmp312);
e_index=_tmp312;
goto _LL1A8;}else{_LL1AB: _LL1AC:
({void*_tmp310=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp311="field name designators in array";_tag_dyneither(_tmp311,sizeof(char),32);}),_tag_dyneither(_tmp310,sizeof(void*),0));});}_LL1A8:;};}{
# 1561
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);
void*_tmp313=_tmp31C->r;void*_tmp314=_tmp313;void*_tmp31B;struct Cyc_List_List*_tmp31A;struct Cyc_Absyn_Vardecl*_tmp319;struct Cyc_Absyn_Exp*_tmp318;struct Cyc_Absyn_Exp*_tmp317;int _tmp316;struct Cyc_List_List*_tmp315;switch(*((int*)_tmp314)){case 25: _LL1AE: _tmp315=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp314)->f1;_LL1AF:
# 1564
 s=Cyc_Toc_init_array(nv,orig_elt_type,lval,_tmp315,s);
goto _LL1AD;case 26: _LL1B0: _tmp319=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp314)->f1;_tmp318=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp314)->f2;_tmp317=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp314)->f3;_tmp316=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp314)->f4;_LL1B1:
# 1567
 s=Cyc_Toc_init_comprehension(nv,orig_elt_type,lval,_tmp319,_tmp318,_tmp317,_tmp316,s,0);
# 1569
goto _LL1AD;case 29: _LL1B2: _tmp31B=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp314)->f1;_tmp31A=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp314)->f2;_LL1B3:
# 1571
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp31B,_tmp31A,s);
goto _LL1AD;case 27: _LL1B4: _LL1B5:
# 1574
 goto _LL1AD;default: _LL1B6: _LL1B7:
# 1576
 Cyc_Toc_exp_to_c(nv,_tmp31C);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(orig_elt_type),_tmp31C),0),s,0);
goto _LL1AD;}_LL1AD:;};};}}
# 1582
return s;}
# 1587
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated){
# 1592
struct _tuple1*_tmp31E=vd->name;
void*expected_elt_type;
{void*_tmp31F=Cyc_Tcutil_compress(comprehension_type);void*_tmp320=_tmp31F;void*_tmp324;void*_tmp323;switch(*((int*)_tmp320)){case 8: _LL1B9: _tmp323=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp320)->f1).elt_type;_LL1BA:
# 1596
 expected_elt_type=_tmp323;goto _LL1B8;case 5: _LL1BB: _tmp324=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp320)->f1).elt_typ;_LL1BC:
# 1598
 expected_elt_type=_tmp324;goto _LL1B8;default: _LL1BD: _LL1BE:
({void*_tmp321=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp322="init_comprehension passed a bad type!";_tag_dyneither(_tmp322,sizeof(char),38);}),_tag_dyneither(_tmp321,sizeof(void*),0));});}_LL1B8:;}{
# 1601
void*_tmp325=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));
if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);
# 1605
{void*_tmp326=e2->r;void*_tmp327=_tmp326;if(((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp327)->tag == 27){_LL1C0: _LL1C1:
 return Cyc_Absyn_skip_stmt(0);}else{_LL1C2: _LL1C3:
 goto _LL1BF;}_LL1BF:;}{
# 1609
struct _RegionHandle _tmp328=_new_region("r2");struct _RegionHandle*r2=& _tmp328;_push_region(r2);
{struct _tuple1*max=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp31E,0),Cyc_Absyn_signed_int_exp(0,0),0);
struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp31E,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp31E,0),0);
# 1616
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp31E,0),0);
struct Cyc_Absyn_Stmt*body;
# 1620
{void*_tmp329=e2->r;void*_tmp32A=_tmp329;void*_tmp331;struct Cyc_List_List*_tmp330;struct Cyc_Absyn_Vardecl*_tmp32F;struct Cyc_Absyn_Exp*_tmp32E;struct Cyc_Absyn_Exp*_tmp32D;int _tmp32C;struct Cyc_List_List*_tmp32B;switch(*((int*)_tmp32A)){case 25: _LL1C5: _tmp32B=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp32A)->f1;_LL1C6:
# 1622
 body=Cyc_Toc_init_array(nv,expected_elt_type,lval,_tmp32B,Cyc_Toc_skip_stmt_dl());
goto _LL1C4;case 26: _LL1C7: _tmp32F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp32A)->f1;_tmp32E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp32A)->f2;_tmp32D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp32A)->f3;_tmp32C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp32A)->f4;_LL1C8:
# 1625
 body=Cyc_Toc_init_comprehension(nv,expected_elt_type,lval,_tmp32F,_tmp32E,_tmp32D,_tmp32C,Cyc_Toc_skip_stmt_dl(),0);
goto _LL1C4;case 29: _LL1C9: _tmp331=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp32A)->f1;_tmp330=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp32A)->f2;_LL1CA:
# 1628
 body=Cyc_Toc_init_anon_struct(nv,lval,_tmp331,_tmp330,Cyc_Toc_skip_stmt_dl());
goto _LL1C4;default: _LL1CB: _LL1CC:
# 1631
 Cyc_Toc_exp_to_c(nv,e2);
body=Cyc_Absyn_assign_stmt(lval,Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(expected_elt_type),e2),0);
goto _LL1C4;}_LL1C4:;}{
# 1635
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0);
# 1637
if(zero_term){
# 1642
struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(_tmp325,Cyc_Absyn_uint_exp(0,0)),0);
s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}{
# 1647
struct Cyc_Absyn_Stmt*_tmp332=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,e1,
Cyc_Absyn_declare_stmt(_tmp31E,Cyc_Absyn_uint_typ,0,s2,0),0),s,0);_npop_handler(0);return _tmp332;};};}
# 1610
;_pop_region(r2);};};}
# 1653
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s){
# 1656
{struct Cyc_List_List*_tmp333=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles);for(0;_tmp333 != 0;_tmp333=_tmp333->tl){
struct _tuple19*_tmp334=(struct _tuple19*)_tmp333->hd;struct _tuple19*_tmp335=_tmp334;struct Cyc_List_List*_tmp349;struct Cyc_Absyn_Exp*_tmp348;_LL1CE: _tmp349=_tmp335->f1;_tmp348=_tmp335->f2;_LL1CF:;
if(_tmp349 == 0)
({void*_tmp336=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp337="empty designator list";_tag_dyneither(_tmp337,sizeof(char),22);}),_tag_dyneither(_tmp336,sizeof(void*),0));});
if(_tmp349->tl != 0)
({void*_tmp338=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp339="too many designators in anonymous struct";_tag_dyneither(_tmp339,sizeof(char),41);}),_tag_dyneither(_tmp338,sizeof(void*),0));});{
void*_tmp33A=(void*)_tmp349->hd;void*_tmp33B=_tmp33A;struct _dyneither_ptr*_tmp347;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp33B)->tag == 1){_LL1D1: _tmp347=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp33B)->f1;_LL1D2: {
# 1664
struct Cyc_Absyn_Exp*lval=Cyc_Toc_member_exp(lhs,_tmp347,0);
{void*_tmp33C=_tmp348->r;void*_tmp33D=_tmp33C;void*_tmp344;struct Cyc_List_List*_tmp343;struct Cyc_Absyn_Vardecl*_tmp342;struct Cyc_Absyn_Exp*_tmp341;struct Cyc_Absyn_Exp*_tmp340;int _tmp33F;struct Cyc_List_List*_tmp33E;switch(*((int*)_tmp33D)){case 25: _LL1D6: _tmp33E=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp33D)->f1;_LL1D7:
# 1667
 s=Cyc_Toc_init_array(nv,(void*)_check_null(_tmp348->topt),lval,_tmp33E,s);goto _LL1D5;case 26: _LL1D8: _tmp342=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp33D)->f1;_tmp341=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp33D)->f2;_tmp340=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp33D)->f3;_tmp33F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp33D)->f4;_LL1D9:
# 1669
 s=Cyc_Toc_init_comprehension(nv,(void*)_check_null(_tmp348->topt),lval,_tmp342,_tmp341,_tmp340,_tmp33F,s,0);
goto _LL1D5;case 29: _LL1DA: _tmp344=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp33D)->f1;_tmp343=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp33D)->f2;_LL1DB:
# 1672
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp344,_tmp343,s);goto _LL1D5;case 27: _LL1DC: _LL1DD:
# 1674
 goto _LL1D5;default: _LL1DE: _LL1DF:
# 1676
 Cyc_Toc_exp_to_c(nv,_tmp348);
# 1678
if(Cyc_Toc_is_poly_field(struct_type,_tmp347))
_tmp348=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp348);
# 1681
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp348,0),0),s,0);
goto _LL1D5;}_LL1D5:;}
# 1684
goto _LL1D0;}}else{_LL1D3: _LL1D4:
({void*_tmp345=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp346="array designator in struct";_tag_dyneither(_tmp346,sizeof(char),27);}),_tag_dyneither(_tmp345,sizeof(void*),0));});}_LL1D0:;};}}
# 1688
return s;}
# 1693
static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){
# 1697
struct _RegionHandle _tmp34A=_new_region("r");struct _RegionHandle*r=& _tmp34A;_push_region(r);
{struct Cyc_List_List*_tmp34B=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);
void*_tmp34C=Cyc_Toc_add_tuple_type(_tmp34B);
# 1701
struct _tuple1*_tmp34D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp34E=Cyc_Absyn_var_exp(_tmp34D,0);
struct Cyc_Absyn_Stmt*_tmp34F=Cyc_Absyn_exp_stmt(_tmp34E,0);
# 1705
struct Cyc_Absyn_Exp*(*_tmp350)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
# 1707
int is_atomic=1;
struct Cyc_List_List*_tmp351=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);
{int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp351);for(0;_tmp351 != 0;(_tmp351=_tmp351->tl,-- i)){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp351->hd;
struct Cyc_Absyn_Exp*lval=_tmp350(_tmp34E,Cyc_Absyn_fieldname(i),0);
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(e->topt));{
void*_tmp352=e->r;void*_tmp353=_tmp352;struct Cyc_Absyn_Vardecl*_tmp358;struct Cyc_Absyn_Exp*_tmp357;struct Cyc_Absyn_Exp*_tmp356;int _tmp355;struct Cyc_List_List*_tmp354;switch(*((int*)_tmp353)){case 25: _LL1E1: _tmp354=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp353)->f1;_LL1E2:
# 1715
 _tmp34F=Cyc_Toc_init_array(nv,(void*)_check_null(e->topt),lval,_tmp354,_tmp34F);
goto _LL1E0;case 26: _LL1E3: _tmp358=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp353)->f1;_tmp357=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp353)->f2;_tmp356=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp353)->f3;_tmp355=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp353)->f4;_LL1E4:
# 1718
 _tmp34F=Cyc_Toc_init_comprehension(nv,(void*)_check_null(e->topt),lval,_tmp358,_tmp357,_tmp356,_tmp355,_tmp34F,0);
# 1720
goto _LL1E0;case 27: _LL1E5: _LL1E6:
# 1722
 goto _LL1E0;default: _LL1E7: _LL1E8:
# 1725
 Cyc_Toc_exp_to_c(nv,e);
_tmp34F=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp350(_tmp34E,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp34F,0);
# 1728
goto _LL1E0;}_LL1E0:;};}}{
# 1731
struct Cyc_Absyn_Exp*_tmp359=Cyc_Toc_make_struct(nv,Cyc_Absyn_var_exp(_tmp34D,0),_tmp34C,_tmp34F,pointer,rgnopt,is_atomic,1);_npop_handler(0);return _tmp359;};}
# 1698
;_pop_region(r);}
# 1735
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp35A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp35A != 0;_tmp35A=_tmp35A->tl){
struct Cyc_Absyn_Aggrfield*_tmp35B=(struct Cyc_Absyn_Aggrfield*)_tmp35A->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp35B->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}
# 1742
({void*_tmp35C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp35F;_tmp35F.tag=0;_tmp35F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp35D[1]={& _tmp35F};Cyc_aprintf(({const char*_tmp35E="get_member_offset %s failed";_tag_dyneither(_tmp35E,sizeof(char),28);}),_tag_dyneither(_tmp35D,sizeof(void*),1));});}),_tag_dyneither(_tmp35C,sizeof(void*),0));});}
# 1745
static int Cyc_Toc_init_variable_array(void**typ){
{void*_tmp360=Cyc_Tcutil_compress(*typ);void*_tmp361=_tmp360;void*_tmp369;struct Cyc_Absyn_Tqual _tmp368;struct Cyc_Absyn_Exp*_tmp367;union Cyc_Absyn_Constraint*_tmp366;unsigned int _tmp365;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp361)->tag == 8){_LL1EA: _tmp369=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp361)->f1).elt_type;_tmp368=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp361)->f1).tq;_tmp367=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp361)->f1).num_elts;_tmp366=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp361)->f1).zero_term;_tmp365=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp361)->f1).zt_loc;_LL1EB:
# 1750
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp367))){
*typ=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp362=_cycalloc(sizeof(*_tmp362));_tmp362[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp363;_tmp363.tag=8;_tmp363.f1=({struct Cyc_Absyn_ArrayInfo _tmp364;_tmp364.elt_type=_tmp369;_tmp364.tq=_tmp368;_tmp364.num_elts=Cyc_Absyn_uint_exp(0,0);_tmp364.zero_term=_tmp366;_tmp364.zt_loc=_tmp365;_tmp364;});_tmp363;});_tmp362;});
return 1;}
# 1754
goto _LL1E9;}else{_LL1EC: _LL1ED:
 goto _LL1E9;}_LL1E9:;}
# 1757
return 0;}
# 1760
static int Cyc_Toc_is_array_type(void*t){
void*_tmp36A=Cyc_Tcutil_compress(t);void*_tmp36B=_tmp36A;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp36B)->tag == 8){_LL1EF: _LL1F0:
 return 1;}else{_LL1F1: _LL1F2:
 return 0;}_LL1EE:;}
# 1767
struct Cyc_Absyn_Aggrdecl*Cyc_Toc_update_aggr_type(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*fn,void*new_field_type){
# 1769
struct Cyc_List_List*new_fields=0;
struct Cyc_List_List*fields;
for(fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;fields != 0;fields=fields->tl){
# 1773
if(!Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,fn)){
struct Cyc_Absyn_Aggrfield*_tmp36C=(struct Cyc_Absyn_Aggrfield*)fields->hd;
struct Cyc_List_List*_tmp36D=_tmp36C->attributes;
void*_tmp36E=_tmp36C->type;
if((!Cyc_Toc_is_array_type(_tmp36E),Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp36E))))
_tmp36D=({struct Cyc_List_List*_tmp36F=_cycalloc(sizeof(*_tmp36F));_tmp36F->hd=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp370=_cycalloc(sizeof(*_tmp370));_tmp370[0]=({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmp371;_tmp371.tag=6;_tmp371.f1=0;_tmp371;});_tmp370;});_tmp36F->tl=_tmp36D;_tmp36F;});{
struct Cyc_Absyn_Aggrfield*_tmp372=({struct Cyc_Absyn_Aggrfield*_tmp374=_cycalloc(sizeof(*_tmp374));_tmp374->name=_tmp36C->name;_tmp374->tq=Cyc_Toc_mt_tq;_tmp374->type=new_field_type;_tmp374->width=_tmp36C->width;_tmp374->attributes=_tmp36D;_tmp374->requires_clause=0;_tmp374;});
# 1785
new_fields=({struct Cyc_List_List*_tmp373=_cycalloc(sizeof(*_tmp373));_tmp373->hd=_tmp372;_tmp373->tl=new_fields;_tmp373;});};}else{
# 1787
new_fields=({struct Cyc_List_List*_tmp375=_cycalloc(sizeof(*_tmp375));_tmp375->hd=(struct Cyc_Absyn_Aggrfield*)fields->hd;_tmp375->tl=new_fields;_tmp375;});}}{
# 1789
struct Cyc_List_List*_tmp376=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);
struct _dyneither_ptr*name=({struct _dyneither_ptr*_tmp37B=_cycalloc(sizeof(*_tmp37B));_tmp37B[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp37E;_tmp37E.tag=1;_tmp37E.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp37C[1]={& _tmp37E};Cyc_aprintf(({const char*_tmp37D="_genStruct%d";_tag_dyneither(_tmp37D,sizeof(char),13);}),_tag_dyneither(_tmp37C,sizeof(void*),1));});});_tmp37B;});
# 1792
struct _tuple1*qv=({struct _tuple1*_tmp37A=_cycalloc(sizeof(*_tmp37A));_tmp37A->f1=Cyc_Absyn_Abs_n(0,1);_tmp37A->f2=name;_tmp37A;});
struct Cyc_Absyn_AggrdeclImpl*_tmp377=({struct Cyc_Absyn_AggrdeclImpl*_tmp379=_cycalloc(sizeof(*_tmp379));_tmp379->exist_vars=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars;_tmp379->rgn_po=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;_tmp379->fields=_tmp376;_tmp379->tagged=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;_tmp379;});
# 1797
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp378=_cycalloc(sizeof(*_tmp378));_tmp378->kind=ad->kind;_tmp378->sc=ad->sc;_tmp378->name=qv;_tmp378->tvs=ad->tvs;_tmp378->impl=_tmp377;_tmp378->expected_mem_kind=0;_tmp378->attributes=ad->attributes;_tmp378;});
# 1804
Cyc_Toc_aggrdecl_to_c(new_ad,1);
return new_ad;};}
# 1808
static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*nv,void*struct_type,struct Cyc_List_List*exist_types,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*dles,struct _tuple1*tdn){
# 1814
int do_declare=0;
struct Cyc_Absyn_Exp*xexp;
if((nv->struct_info).lhs_exp == 0){
struct _tuple1*_tmp37F=Cyc_Toc_temp_var();
xexp=Cyc_Absyn_var_exp(_tmp37F,0);
do_declare=1;}else{
# 1822
xexp=(struct Cyc_Absyn_Exp*)_check_null((nv->struct_info).lhs_exp);
nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);}{
# 1825
struct Cyc_Absyn_Stmt*_tmp380=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(xexp),0);
# 1829
struct Cyc_Absyn_Exp**varexp;
{void*_tmp381=_tmp380->r;void*_tmp382=_tmp381;struct Cyc_Absyn_Exp**_tmp385;if(((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp382)->tag == 1){_LL1F4: _tmp385=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp382)->f1;_LL1F5:
 varexp=_tmp385;goto _LL1F3;}else{_LL1F6: _LL1F7:
({void*_tmp383=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp384="stmt not an expression!";_tag_dyneither(_tmp384,sizeof(char),24);}),_tag_dyneither(_tmp383,sizeof(void*),0));});}_LL1F3:;}{
# 1835
struct Cyc_Absyn_Exp*(*_tmp386)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
void*aggr_typ=Cyc_Toc_typ_to_c(struct_type);
void*orig_typ=aggr_typ;
# 1839
int is_atomic=1;
struct Cyc_List_List*forall_types;
struct Cyc_Absyn_Aggrdecl*ad;
struct Cyc_List_List*aggrfields;struct Cyc_List_List*orig_aggrfields;
struct _dyneither_ptr**index=0;
{void*_tmp387=Cyc_Tcutil_compress(struct_type);void*_tmp388=_tmp387;union Cyc_Absyn_AggrInfoU _tmp38C;struct Cyc_List_List*_tmp38B;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp388)->tag == 11){_LL1F9: _tmp38C=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp388)->f1).aggr_info;_tmp38B=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp388)->f1).targs;_LL1FA:
# 1846
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp38C);
orig_aggrfields=(aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
forall_types=_tmp38B;
goto _LL1F8;}else{_LL1FB: _LL1FC:
({void*_tmp389=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp38A="init_struct: bad struct type";_tag_dyneither(_tmp38A,sizeof(char),29);}),_tag_dyneither(_tmp389,sizeof(void*),0));});}_LL1F8:;}
# 1853
if(exist_types != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields != 0){
struct Cyc_List_List*_tmp38D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
for(0;_tmp38D->tl != 0;_tmp38D=_tmp38D->tl){;}{
struct Cyc_Absyn_Aggrfield*_tmp38E=(struct Cyc_Absyn_Aggrfield*)_tmp38D->hd;
struct _RegionHandle _tmp38F=_new_region("temp");struct _RegionHandle*temp=& _tmp38F;_push_region(temp);
{struct Cyc_List_List*_tmp390=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,forall_types);
# 1862
if(Cyc_Toc_is_abstract_type(Cyc_Tcutil_rsubstitute(temp,_tmp390,_tmp38E->type))){
struct Cyc_List_List*_tmp391=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,exist_types);
struct Cyc_List_List*_tmp392=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(temp,_tmp390,_tmp391);
# 1866
struct Cyc_List_List*new_fields=0;
for(_tmp38D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;_tmp38D != 0;_tmp38D=_tmp38D->tl){
struct Cyc_Absyn_Aggrfield*_tmp393=(struct Cyc_Absyn_Aggrfield*)_tmp38D->hd;
struct Cyc_Absyn_Aggrfield*_tmp394=({struct Cyc_Absyn_Aggrfield*_tmp397=_cycalloc(sizeof(*_tmp397));_tmp397->name=_tmp393->name;_tmp397->tq=Cyc_Toc_mt_tq;_tmp397->type=
# 1871
Cyc_Tcutil_rsubstitute(temp,_tmp392,_tmp393->type);_tmp397->width=_tmp393->width;_tmp397->attributes=_tmp393->attributes;_tmp397->requires_clause=0;_tmp397;});
# 1879
if(_tmp38D->tl == 0){
if(Cyc_Toc_init_variable_array(& _tmp394->type))
index=({struct _dyneither_ptr**_tmp395=_cycalloc(sizeof(*_tmp395));_tmp395[0]=_tmp394->name;_tmp395;});}
# 1886
new_fields=({struct Cyc_List_List*_tmp396=_cycalloc(sizeof(*_tmp396));_tmp396->hd=_tmp394;_tmp396->tl=new_fields;_tmp396;});}
# 1888
exist_types=0;
aggrfields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);{
struct _dyneither_ptr*name=({struct _dyneither_ptr*_tmp3A0=_cycalloc(sizeof(*_tmp3A0));_tmp3A0[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3A3;_tmp3A3.tag=1;_tmp3A3.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp3A1[1]={& _tmp3A3};Cyc_aprintf(({const char*_tmp3A2="_genStruct%d";_tag_dyneither(_tmp3A2,sizeof(char),13);}),_tag_dyneither(_tmp3A1,sizeof(void*),1));});});_tmp3A0;});
# 1892
struct _tuple1*qv=({struct _tuple1*_tmp39F=_cycalloc(sizeof(*_tmp39F));_tmp39F->f1=Cyc_Absyn_Abs_n(0,1);_tmp39F->f2=name;_tmp39F;});
struct Cyc_Absyn_AggrdeclImpl*_tmp398=({struct Cyc_Absyn_AggrdeclImpl*_tmp39E=_cycalloc(sizeof(*_tmp39E));_tmp39E->exist_vars=0;_tmp39E->rgn_po=0;_tmp39E->fields=aggrfields;_tmp39E->tagged=0;_tmp39E;});
# 1897
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp39D=_cycalloc(sizeof(*_tmp39D));_tmp39D->kind=Cyc_Absyn_StructA;_tmp39D->sc=Cyc_Absyn_Public;_tmp39D->name=qv;_tmp39D->tvs=0;_tmp39D->expected_mem_kind=0;_tmp39D->impl=_tmp398;_tmp39D->attributes=0;_tmp39D;});
# 1904
Cyc_Toc_aggrdecl_to_c(new_ad,1);
ad=new_ad;
struct_type=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp399=_cycalloc(sizeof(*_tmp399));_tmp399[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp39A;_tmp39A.tag=11;_tmp39A.f1=({struct Cyc_Absyn_AggrInfo _tmp39B;_tmp39B.aggr_info=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp39C=_cycalloc(sizeof(*_tmp39C));_tmp39C[0]=new_ad;_tmp39C;}));_tmp39B.targs=0;_tmp39B;});_tmp39A;});_tmp399;});
# 1908
aggr_typ=Cyc_Toc_typ_to_c(struct_type);};}}
# 1858
;_pop_region(temp);};}{
# 1912
int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct _RegionHandle _tmp3A4=_new_region("r");struct _RegionHandle*r=& _tmp3A4;_push_region(r);
{struct Cyc_List_List*_tmp3A5=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp3A5 != 0;_tmp3A5=_tmp3A5->tl){
struct _tuple19*_tmp3A6=(struct _tuple19*)_tmp3A5->hd;struct _tuple19*_tmp3A7=_tmp3A6;struct Cyc_List_List*_tmp3CC;struct Cyc_Absyn_Exp*_tmp3CB;_LL1FE: _tmp3CC=_tmp3A7->f1;_tmp3CB=_tmp3A7->f2;_LL1FF:;
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(_tmp3CB->topt));
if(_tmp3CC == 0)
({void*_tmp3A8=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3A9="empty designator list";_tag_dyneither(_tmp3A9,sizeof(char),22);}),_tag_dyneither(_tmp3A8,sizeof(void*),0));});
if(_tmp3CC->tl != 0){
# 1922
struct _tuple1*_tmp3AA=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3AB=Cyc_Absyn_var_exp(_tmp3AA,0);
for(0;_tmp3CC != 0;_tmp3CC=_tmp3CC->tl){
void*_tmp3AC=(void*)_tmp3CC->hd;void*_tmp3AD=_tmp3AC;struct _dyneither_ptr*_tmp3B0;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3AD)->tag == 1){_LL201: _tmp3B0=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3AD)->f1;_LL202:
# 1928
 if(Cyc_Toc_is_poly_field(struct_type,_tmp3B0))
_tmp3AB=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3AB);
_tmp380=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp386(xexp,_tmp3B0,0),_tmp3AB,0),0),_tmp380,0);
# 1932
goto _LL200;}else{_LL203: _LL204:
({void*_tmp3AE=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3AF="array designator in struct";_tag_dyneither(_tmp3AF,sizeof(char),27);}),_tag_dyneither(_tmp3AE,sizeof(void*),0));});}_LL200:;}
# 1936
Cyc_Toc_exp_to_c(nv,_tmp3CB);
_tmp380=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3AB,_tmp3CB,0),0),_tmp380,0);}else{
# 1939
void*_tmp3B1=(void*)_tmp3CC->hd;void*_tmp3B2=_tmp3B1;struct _dyneither_ptr*_tmp3CA;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3B2)->tag == 1){_LL206: _tmp3CA=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3B2)->f1;_LL207: {
# 1941
struct Cyc_Absyn_Exp*lval=_tmp386(xexp,_tmp3CA,0);
if(is_tagged_union){
int i=Cyc_Toc_get_member_offset(ad,_tmp3CA);
struct Cyc_Absyn_Exp*f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct Cyc_Absyn_Exp*lhs=Cyc_Toc_member_exp(lval,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,f_tag_exp,0);
_tmp380=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp380,0);
lval=Cyc_Toc_member_exp(lval,Cyc_Toc_val_sp,0);}{
# 1950
int e1_translated=0;
{void*_tmp3B3=_tmp3CB->r;void*_tmp3B4=_tmp3B3;void*_tmp3C7;struct Cyc_List_List*_tmp3C6;struct Cyc_Absyn_Exp*_tmp3C5;void*_tmp3C4;struct Cyc_Absyn_Vardecl*_tmp3C3;struct Cyc_Absyn_Exp*_tmp3C2;struct Cyc_Absyn_Exp*_tmp3C1;int _tmp3C0;struct Cyc_List_List*_tmp3BF;switch(*((int*)_tmp3B4)){case 25: _LL20B: _tmp3BF=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3B4)->f1;_LL20C:
# 1953
 _tmp380=Cyc_Toc_init_array(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(aggrfields,_tmp3CA)))->type,lval,_tmp3BF,_tmp380);
# 1955
goto _LL20A;case 26: _LL20D: _tmp3C3=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3B4)->f1;_tmp3C2=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3B4)->f2;_tmp3C1=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3B4)->f3;_tmp3C0=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3B4)->f4;_LL20E:
# 1957
 _tmp380=Cyc_Toc_init_comprehension(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(do_declare?aggrfields: orig_aggrfields,_tmp3CA)))->type,lval,_tmp3C3,_tmp3C2,_tmp3C1,_tmp3C0,_tmp380,0);
# 1961
e1_translated=1;
_tmp3C5=_tmp3C2;_tmp3C4=(void*)_check_null(_tmp3C1->topt);goto _LL210;case 27: _LL20F: _tmp3C5=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp3B4)->f1;_tmp3C4=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp3B4)->f2;_LL210:
# 1966
 if(index != 0  && !Cyc_strptrcmp(*index,_tmp3CA)){
if(!e1_translated)Cyc_Toc_exp_to_c(nv,_tmp3C5);{
void*_tmp3B5=Cyc_Toc_typ_to_c(_tmp3C4);
*(nv->struct_info).varsizeexp=
Cyc_Absyn_add_exp(Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp3B6[2];_tmp3B6[1]=
Cyc_Absyn_sizeoftyp_exp(_tmp3B5,0);_tmp3B6[0]=_tmp3C5;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3B6,sizeof(struct Cyc_Absyn_Exp*),2));}),0),
Cyc_Absyn_const_exp(({union Cyc_Absyn_Cnst _tmp3B7;(_tmp3B7.Int_c).val=({struct _tuple6 _tmp3B8;_tmp3B8.f1=Cyc_Absyn_Unsigned;_tmp3B8.f2=(int)sizeof(double);_tmp3B8;});(_tmp3B7.Int_c).tag=5;_tmp3B7;}),0),0);};}
# 1974
goto _LL20A;case 29: _LL211: _tmp3C7=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3B4)->f1;_tmp3C6=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3B4)->f2;_LL212:
# 1976
 _tmp380=Cyc_Toc_init_anon_struct(nv,lval,_tmp3C7,_tmp3C6,_tmp380);goto _LL20A;default: _LL213: _LL214: {
# 1978
void*old_e_typ=(void*)_check_null(_tmp3CB->topt);
int was_ptr_type=Cyc_Toc_is_pointer_or_boxed_tvar(old_e_typ);
Cyc_Toc_exp_to_c(Cyc_Toc_set_lhs_exp(nv->rgn,lval,nv),_tmp3CB);
{void*_tmp3B9=old_e_typ;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3B9)->tag == 11){_LL216: _LL217:
# 1985
 if(old_e_typ != _tmp3CB->topt  && !Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(old_e_typ),(void*)_check_null(_tmp3CB->topt))){
# 1989
ad=Cyc_Toc_update_aggr_type(ad,_tmp3CA,(void*)_check_null(_tmp3CB->topt));
struct_type=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3BA=_cycalloc(sizeof(*_tmp3BA));_tmp3BA[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp3BB;_tmp3BB.tag=11;_tmp3BB.f1=({struct Cyc_Absyn_AggrInfo _tmp3BC;_tmp3BC.aggr_info=
Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp3BD=_cycalloc(sizeof(*_tmp3BD));_tmp3BD[0]=ad;_tmp3BD;}));_tmp3BC.targs=0;_tmp3BC;});_tmp3BB;});_tmp3BA;});
# 1993
aggr_typ=Cyc_Toc_typ_to_c(struct_type);}
# 1995
goto _LL215;}else{_LL218: _LL219:
 goto _LL215;}_LL215:;}{
# 1998
struct Cyc_Absyn_Aggrfield*_tmp3BE=Cyc_Absyn_lookup_field(aggrfields,_tmp3CA);
# 2000
if(Cyc_Toc_is_poly_field(struct_type,_tmp3CA) && !was_ptr_type)
_tmp3CB=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3CB);
# 2003
if(exist_types != 0)
_tmp3CB=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3BE))->type),_tmp3CB);
# 2006
_tmp380=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp3CB,0),0),_tmp380,0);
goto _LL20A;};}}_LL20A:;}
# 2009
goto _LL205;};}}else{_LL208: _LL209:
({void*_tmp3C8=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3C9="array designator in struct";_tag_dyneither(_tmp3C9,sizeof(char),27);}),_tag_dyneither(_tmp3C8,sizeof(void*),0));});}_LL205:;}}}
# 2016
if(aggr_typ != orig_typ  && pointer){
void*cast_type=Cyc_Absyn_cstar_typ(orig_typ,Cyc_Toc_mt_tq);
*varexp=Cyc_Toc_cast_it(cast_type,*varexp);}{
# 2021
struct Cyc_Absyn_Exp*_tmp3CD=Cyc_Toc_make_struct(nv,xexp,aggr_typ,_tmp380,pointer,rgnopt,is_atomic,do_declare);
# 2023
_tmp3CD->topt=struct_type;{
struct Cyc_Absyn_Exp*_tmp3CE=_tmp3CD;_npop_handler(0);return _tmp3CE;};};
# 1914
;_pop_region(r);};};};}struct _tuple27{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 2029
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple27*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
# 2032
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0);}
# 2035
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp3CF=_cycalloc(sizeof(*_tmp3CF));_tmp3CF[0]=({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmp3D0;_tmp3D0.tag=4;_tmp3D0.f1=e1;_tmp3D0.f2=incr;_tmp3D0;});_tmp3CF;}),0);}
# 2039
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 2048
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 2050
void*_tmp3D1=e1->r;void*_tmp3D2=_tmp3D1;struct Cyc_Absyn_Exp*_tmp3DA;struct _dyneither_ptr*_tmp3D9;int _tmp3D8;int _tmp3D7;void*_tmp3D6;struct Cyc_Absyn_Exp*_tmp3D5;struct Cyc_Absyn_Stmt*_tmp3D4;switch(*((int*)_tmp3D2)){case 36: _LL21B: _tmp3D4=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3D2)->f1;_LL21C:
 Cyc_Toc_lvalue_assign_stmt(_tmp3D4,fs,f,f_env);goto _LL21A;case 13: _LL21D: _tmp3D6=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3D2)->f1;_tmp3D5=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3D2)->f2;_LL21E:
 Cyc_Toc_lvalue_assign(_tmp3D5,fs,f,f_env);goto _LL21A;case 20: _LL21F: _tmp3DA=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3D2)->f1;_tmp3D9=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3D2)->f2;_tmp3D8=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3D2)->f3;_tmp3D7=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3D2)->f4;_LL220:
# 2055
 e1->r=_tmp3DA->r;
Cyc_Toc_lvalue_assign(e1,({struct Cyc_List_List*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));_tmp3D3->hd=_tmp3D9;_tmp3D3->tl=fs;_tmp3D3;}),f,f_env);
goto _LL21A;default: _LL221: _LL222: {
# 2063
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 2065
for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;
goto _LL21A;}}_LL21A:;}
# 2071
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 2073
void*_tmp3DB=s->r;void*_tmp3DC=_tmp3DB;struct Cyc_Absyn_Stmt*_tmp3E3;struct Cyc_Absyn_Decl*_tmp3E2;struct Cyc_Absyn_Stmt*_tmp3E1;struct Cyc_Absyn_Exp*_tmp3E0;switch(*((int*)_tmp3DC)){case 1: _LL224: _tmp3E0=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3DC)->f1;_LL225:
 Cyc_Toc_lvalue_assign(_tmp3E0,fs,f,f_env);goto _LL223;case 12: _LL226: _tmp3E2=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3DC)->f1;_tmp3E1=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3DC)->f2;_LL227:
# 2076
 Cyc_Toc_lvalue_assign_stmt(_tmp3E1,fs,f,f_env);goto _LL223;case 2: _LL228: _tmp3E3=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3DC)->f2;_LL229:
 Cyc_Toc_lvalue_assign_stmt(_tmp3E3,fs,f,f_env);goto _LL223;default: _LL22A: _LL22B:
({struct Cyc_String_pa_PrintArg_struct _tmp3DF;_tmp3DF.tag=0;_tmp3DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));({void*_tmp3DD[1]={& _tmp3DF};Cyc_Toc_toc_impos(({const char*_tmp3DE="lvalue_assign_stmt: %s";_tag_dyneither(_tmp3DE,sizeof(char),23);}),_tag_dyneither(_tmp3DD,sizeof(void*),1));});});}_LL223:;}
# 2082
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 2086
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp3E4=e->r;void*_tmp3E5=_tmp3E4;struct Cyc_Absyn_Stmt*_tmp3EC;struct Cyc_Absyn_Exp*_tmp3EB;void**_tmp3EA;struct Cyc_Absyn_Exp**_tmp3E9;switch(*((int*)_tmp3E5)){case 13: _LL22D: _tmp3EA=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3E5)->f1;_tmp3E9=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3E5)->f2;_LL22E:
# 2089
*_tmp3E9=Cyc_Toc_push_address_exp(*_tmp3E9);
*_tmp3EA=Cyc_Absyn_cstar_typ(*_tmp3EA,Cyc_Toc_mt_tq);
return e;case 19: _LL22F: _tmp3EB=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3E5)->f1;_LL230:
# 2093
 return _tmp3EB;case 36: _LL231: _tmp3EC=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3E5)->f1;_LL232:
# 2097
 Cyc_Toc_push_address_stmt(_tmp3EC);
return e;default: _LL233: _LL234:
# 2100
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);
({struct Cyc_String_pa_PrintArg_struct _tmp3E8;_tmp3E8.tag=0;_tmp3E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp3E6[1]={& _tmp3E8};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3E7="can't take & of exp %s";_tag_dyneither(_tmp3E7,sizeof(char),23);}),_tag_dyneither(_tmp3E6,sizeof(void*),1));});});}_LL22C:;}
# 2105
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp3ED=s->r;void*_tmp3EE=_tmp3ED;struct Cyc_Absyn_Exp**_tmp3F4;struct Cyc_Absyn_Stmt*_tmp3F3;struct Cyc_Absyn_Stmt*_tmp3F2;switch(*((int*)_tmp3EE)){case 2: _LL236: _tmp3F2=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3EE)->f2;_LL237:
 _tmp3F3=_tmp3F2;goto _LL239;case 12: _LL238: _tmp3F3=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3EE)->f2;_LL239:
 Cyc_Toc_push_address_stmt(_tmp3F3);goto _LL235;case 1: _LL23A: _tmp3F4=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3EE)->f1;_LL23B:
*_tmp3F4=Cyc_Toc_push_address_exp(*_tmp3F4);goto _LL235;default: _LL23C: _LL23D:
# 2111
({struct Cyc_String_pa_PrintArg_struct _tmp3F1;_tmp3F1.tag=0;_tmp3F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));({void*_tmp3EF[1]={& _tmp3F1};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3F0="can't take & of stmt %s";_tag_dyneither(_tmp3F0,sizeof(char),24);}),_tag_dyneither(_tmp3EF,sizeof(void*),1));});});}_LL235:;}
# 2116
static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 2118
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 2120
if(x == 0)return 0;
result=({struct Cyc_List_List*_tmp3F5=_region_malloc(r2,sizeof(*_tmp3F5));_tmp3F5->hd=f(x->hd,env);_tmp3F5->tl=0;_tmp3F5;});
prev=result;
for(x=x->tl;x != 0;x=x->tl){
((struct Cyc_List_List*)_check_null(prev))->tl=({struct Cyc_List_List*_tmp3F6=_region_malloc(r2,sizeof(*_tmp3F6));_tmp3F6->hd=f(x->hd,env);_tmp3F6->tl=0;_tmp3F6;});
prev=prev->tl;}
# 2127
return result;}
# 2129
static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,x);}
# 2133
static struct _tuple19*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){
return({struct _tuple19*_tmp3F7=_cycalloc(sizeof(*_tmp3F7));_tmp3F7->f1=0;_tmp3F7->f2=e;_tmp3F7;});}
# 2137
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_tmp3F8=Cyc_Tcutil_compress(t);void*_tmp3F9=_tmp3F8;struct Cyc_Absyn_PtrInfo _tmp3FC;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F9)->tag == 5){_LL23F: _tmp3FC=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F9)->f1;_LL240:
 return _tmp3FC;}else{_LL241: _LL242:
({void*_tmp3FA=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3FB="get_ptr_typ: not a pointer!";_tag_dyneither(_tmp3FB,sizeof(char),28);}),_tag_dyneither(_tmp3FA,sizeof(void*),0));});}_LL23E:;}
# 2147
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp3FD=Cyc_Tcutil_compress(t);void*_tmp3FE=_tmp3FD;int _tmp40A;enum Cyc_Absyn_Sign _tmp409;enum Cyc_Absyn_Sign _tmp408;enum Cyc_Absyn_Sign _tmp407;enum Cyc_Absyn_Sign _tmp406;enum Cyc_Absyn_Sign _tmp405;switch(*((int*)_tmp3FE)){case 5: _LL244: _LL245:
 res=Cyc_Absyn_null_exp(0);goto _LL243;case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3FE)->f2){case Cyc_Absyn_Char_sz: _LL246: _tmp405=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3FE)->f1;_LL247:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp405,'\000'),0);goto _LL243;case Cyc_Absyn_Short_sz: _LL248: _tmp406=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3FE)->f1;_LL249:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp406,0),0);goto _LL243;case Cyc_Absyn_Int_sz: _LL24E: _tmp407=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3FE)->f1;_LL24F:
# 2155
 _tmp408=_tmp407;goto _LL251;case Cyc_Absyn_Long_sz: _LL250: _tmp408=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3FE)->f1;_LL251:
# 2157
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp408,0),0);goto _LL243;default: _LL252: _tmp409=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3FE)->f1;_LL253:
# 2159
 res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp409,(long long)0),0);goto _LL243;}case 13: _LL24A: _LL24B:
# 2153
 goto _LL24D;case 14: _LL24C: _LL24D:
 _tmp407=Cyc_Absyn_Unsigned;goto _LL24F;case 7: switch(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3FE)->f1){case 0: _LL254: _LL255:
# 2160
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp3FF="0.0F";_tag_dyneither(_tmp3FF,sizeof(char),5);}),0),0);goto _LL243;case 1: _LL256: _LL257:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp400="0.0";_tag_dyneither(_tmp400,sizeof(char),4);}),1),0);goto _LL243;default: _LL258: _tmp40A=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3FE)->f1;_LL259:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp401="0.0L";_tag_dyneither(_tmp401,sizeof(char),5);}),_tmp40A),0);goto _LL243;}default: _LL25A: _LL25B:
# 2164
({struct Cyc_String_pa_PrintArg_struct _tmp404;_tmp404.tag=0;_tmp404.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp402[1]={& _tmp404};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp403="found non-zero type %s in generate_zero";_tag_dyneither(_tmp403,sizeof(char),40);}),_tag_dyneither(_tmp402,sizeof(void*),1));});});}_LL243:;}
# 2166
res->topt=t;
return res;}
# 2173
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 2186 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp40B=Cyc_Toc_typ_to_c(elt_type);
void*_tmp40C=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp40D=Cyc_Absyn_cstar_typ(_tmp40B,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmp40E=({struct Cyc_Core_Opt*_tmp445=_cycalloc(sizeof(*_tmp445));_tmp445->v=_tmp40D;_tmp445;});
struct Cyc_Absyn_Exp*xinit;
{void*_tmp40F=e1->r;void*_tmp410=_tmp40F;struct Cyc_Absyn_Exp*_tmp416;struct Cyc_Absyn_Exp*_tmp415;struct Cyc_Absyn_Exp*_tmp414;switch(*((int*)_tmp410)){case 19: _LL25D: _tmp414=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp410)->f1;_LL25E:
# 2194
 if(!is_dyneither){
_tmp414=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp414,0,Cyc_Absyn_Other_coercion,0);
_tmp414->topt=fat_ptr_type;}
# 2198
Cyc_Toc_exp_to_c(nv,_tmp414);xinit=_tmp414;goto _LL25C;case 22: _LL25F: _tmp416=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp410)->f1;_tmp415=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp410)->f2;_LL260:
# 2200
 if(!is_dyneither){
_tmp416=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp416,0,Cyc_Absyn_Other_coercion,0);
_tmp416->topt=fat_ptr_type;}
# 2204
Cyc_Toc_exp_to_c(nv,_tmp416);Cyc_Toc_exp_to_c(nv,_tmp415);
xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp411[3];_tmp411[2]=_tmp415;_tmp411[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0);_tmp411[0]=_tmp416;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp411,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
goto _LL25C;default: _LL261: _LL262:
({void*_tmp412=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp413="found bad lhs for zero-terminated pointer assignment";_tag_dyneither(_tmp413,sizeof(char),53);}),_tag_dyneither(_tmp412,sizeof(void*),0));});}_LL25C:;}{
# 2210
struct _tuple1*_tmp417=Cyc_Toc_temp_var();
struct _RegionHandle _tmp418=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp418;_push_region(rgn2);
{struct Cyc_Absyn_Vardecl*_tmp419=({struct Cyc_Absyn_Vardecl*_tmp444=_cycalloc(sizeof(*_tmp444));_tmp444->sc=Cyc_Absyn_Public;_tmp444->name=_tmp417;_tmp444->tq=Cyc_Toc_mt_tq;_tmp444->type=_tmp40C;_tmp444->initializer=xinit;_tmp444->rgn=0;_tmp444->attributes=0;_tmp444->escapes=0;_tmp444;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp41A=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp442=_cycalloc(sizeof(*_tmp442));_tmp442[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp443;_tmp443.tag=4;_tmp443.f1=_tmp419;_tmp443;});_tmp442;});
struct Cyc_Absyn_Exp*_tmp41B=Cyc_Absyn_varb_exp((void*)_tmp41A,0);
_tmp41B->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp41C=Cyc_Absyn_deref_exp(_tmp41B,0);
_tmp41C->topt=elt_type;
Cyc_Toc_exp_to_c(nv,_tmp41C);{
struct _tuple1*_tmp41D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp41E=({struct Cyc_Absyn_Vardecl*_tmp441=_cycalloc(sizeof(*_tmp441));_tmp441->sc=Cyc_Absyn_Public;_tmp441->name=_tmp41D;_tmp441->tq=Cyc_Toc_mt_tq;_tmp441->type=_tmp40B;_tmp441->initializer=_tmp41C;_tmp441->rgn=0;_tmp441->attributes=0;_tmp441->escapes=0;_tmp441;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp41F=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp43F=_cycalloc(sizeof(*_tmp43F));_tmp43F[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp440;_tmp440.tag=4;_tmp440.f1=_tmp41E;_tmp440;});_tmp43F;});
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp420=Cyc_Absyn_varb_exp((void*)_tmp41F,0);
_tmp420->topt=_tmp41C->topt;
z_init=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp420,e2,0);
z_init->topt=_tmp420->topt;}
# 2229
Cyc_Toc_exp_to_c(nv,z_init);{
struct _tuple1*_tmp421=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp422=({struct Cyc_Absyn_Vardecl*_tmp43E=_cycalloc(sizeof(*_tmp43E));_tmp43E->sc=Cyc_Absyn_Public;_tmp43E->name=_tmp421;_tmp43E->tq=Cyc_Toc_mt_tq;_tmp43E->type=_tmp40B;_tmp43E->initializer=z_init;_tmp43E->rgn=0;_tmp43E->attributes=0;_tmp43E->escapes=0;_tmp43E;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp423=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp43C=_cycalloc(sizeof(*_tmp43C));_tmp43C[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp43D;_tmp43D.tag=4;_tmp43D.f1=_tmp422;_tmp43D;});_tmp43C;});
# 2235
struct Cyc_Absyn_Exp*_tmp424=Cyc_Absyn_varb_exp((void*)_tmp41F,0);_tmp424->topt=_tmp41C->topt;{
struct Cyc_Absyn_Exp*_tmp425=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp426=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp424,_tmp425,0);
_tmp426->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(nv,_tmp426);{
# 2241
struct Cyc_Absyn_Exp*_tmp427=Cyc_Absyn_varb_exp((void*)_tmp423,0);_tmp427->topt=_tmp41C->topt;{
struct Cyc_Absyn_Exp*_tmp428=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp429=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp427,_tmp428,0);
_tmp429->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(nv,_tmp429);{
# 2247
struct Cyc_List_List*_tmp42A=({struct Cyc_Absyn_Exp*_tmp43B[2];_tmp43B[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0);_tmp43B[0]=
# 2247
Cyc_Absyn_varb_exp((void*)_tmp41A,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp43B,sizeof(struct Cyc_Absyn_Exp*),2));});
# 2249
struct Cyc_Absyn_Exp*_tmp42B=Cyc_Absyn_uint_exp(1,0);
struct Cyc_Absyn_Exp*xsize;
xsize=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp42A,0),_tmp42B,0);{
# 2254
struct Cyc_Absyn_Exp*_tmp42C=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp426,_tmp429,0),0);
struct Cyc_Absyn_Stmt*_tmp42D=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0),0);
struct Cyc_Absyn_Exp*_tmp42E=Cyc_Toc_member_exp(Cyc_Absyn_varb_exp((void*)_tmp41A,0),Cyc_Toc_curr_sp,0);
_tmp42E=Cyc_Toc_cast_it(_tmp40D,_tmp42E);{
struct Cyc_Absyn_Exp*_tmp42F=Cyc_Absyn_deref_exp(_tmp42E,0);
struct Cyc_Absyn_Exp*_tmp430=Cyc_Absyn_assign_exp(_tmp42F,Cyc_Absyn_var_exp(_tmp421,0),0);
struct Cyc_Absyn_Stmt*_tmp431=Cyc_Absyn_exp_stmt(_tmp430,0);
_tmp431=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp42C,_tmp42D,Cyc_Absyn_skip_stmt(0),0),_tmp431,0);
_tmp431=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp432=_cycalloc(sizeof(*_tmp432));_tmp432->r=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp433=_cycalloc(sizeof(*_tmp433));_tmp433[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp434;_tmp434.tag=0;_tmp434.f1=_tmp422;_tmp434;});_tmp433;});_tmp432->loc=0;_tmp432;}),_tmp431,0);
_tmp431=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp435=_cycalloc(sizeof(*_tmp435));_tmp435->r=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp436=_cycalloc(sizeof(*_tmp436));_tmp436[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp437;_tmp437.tag=0;_tmp437.f1=_tmp41E;_tmp437;});_tmp436;});_tmp435->loc=0;_tmp435;}),_tmp431,0);
_tmp431=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp438=_cycalloc(sizeof(*_tmp438));_tmp438->r=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp439=_cycalloc(sizeof(*_tmp439));_tmp439[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp43A;_tmp43A.tag=0;_tmp43A.f1=_tmp419;_tmp43A;});_tmp439;});_tmp438->loc=0;_tmp438;}),_tmp431,0);
e->r=Cyc_Toc_stmt_exp_r(_tmp431);};};};};};};};};};}
# 2212
;_pop_region(rgn2);};}
# 2280 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 2284
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp446=Cyc_Tcutil_compress(aggrtype);void*_tmp447=_tmp446;union Cyc_Absyn_AggrInfoU _tmp44C;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp447)->tag == 11){_LL264: _tmp44C=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp447)->f1).aggr_info;_LL265:
# 2287
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp44C);goto _LL263;}else{_LL266: _LL267:
({void*_tmp448=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp44B;_tmp44B.tag=0;_tmp44B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype));({void*_tmp449[1]={& _tmp44B};Cyc_aprintf(({const char*_tmp44A="expecting union but found %s in check_tagged_union";_tag_dyneither(_tmp44A,sizeof(char),51);}),_tag_dyneither(_tmp449,sizeof(void*),1));});}),_tag_dyneither(_tmp448,sizeof(void*),0));});}_LL263:;}{
# 2291
struct _tuple1*_tmp44D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp44E=Cyc_Absyn_var_exp(_tmp44D,0);
struct Cyc_Absyn_Exp*_tmp44F=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp450=Cyc_Absyn_aggrarrow_exp(_tmp44E,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp451=Cyc_Absyn_neq_exp(_tmp450,_tmp44F,0);
struct Cyc_Absyn_Exp*_tmp452=Cyc_Absyn_aggrarrow_exp(_tmp44E,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp453=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp452,0),0);
struct Cyc_Absyn_Stmt*_tmp454=Cyc_Absyn_ifthenelse_stmt(_tmp451,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
void*_tmp455=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp456=Cyc_Absyn_address_exp(aggrproj(e1,f,0),0);
struct Cyc_Absyn_Stmt*_tmp457=Cyc_Absyn_declare_stmt(_tmp44D,_tmp455,_tmp456,Cyc_Absyn_seq_stmt(_tmp454,_tmp453,0),0);
return Cyc_Toc_stmt_exp_r(_tmp457);}else{
# 2305
struct Cyc_Absyn_Exp*_tmp458=Cyc_Toc_member_exp(aggrproj(_tmp44E,f,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp459=Cyc_Absyn_neq_exp(_tmp458,_tmp44F,0);
struct Cyc_Absyn_Exp*_tmp45A=Cyc_Toc_member_exp(aggrproj(_tmp44E,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp45B=Cyc_Absyn_exp_stmt(_tmp45A,0);
struct Cyc_Absyn_Stmt*_tmp45C=Cyc_Absyn_ifthenelse_stmt(_tmp459,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*_tmp45D=Cyc_Absyn_declare_stmt(_tmp44D,e1_c_type,e1,Cyc_Absyn_seq_stmt(_tmp45C,_tmp45B,0),0);
return Cyc_Toc_stmt_exp_r(_tmp45D);}};}
# 2315
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _dyneither_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 2318
void*_tmp45E=Cyc_Tcutil_compress(t);void*_tmp45F=_tmp45E;union Cyc_Absyn_AggrInfoU _tmp466;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp45F)->tag == 11){_LL269: _tmp466=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp45F)->f1).aggr_info;_LL26A: {
# 2320
struct Cyc_Absyn_Aggrdecl*_tmp460=Cyc_Absyn_get_known_aggrdecl(_tmp466);
*f_tag=Cyc_Toc_get_member_offset(_tmp460,f);{
# 2323
struct _dyneither_ptr str=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp465;_tmp465.tag=0;_tmp465.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp464;_tmp464.tag=0;_tmp464.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp460->name).f2);({void*_tmp462[2]={& _tmp464,& _tmp465};Cyc_aprintf(({const char*_tmp463="_union_%s_%s";_tag_dyneither(_tmp463,sizeof(char),13);}),_tag_dyneither(_tmp462,sizeof(void*),2));});});});
*tagged_member_type=Cyc_Absyn_strct(({struct _dyneither_ptr*_tmp461=_cycalloc(sizeof(*_tmp461));_tmp461[0]=str;_tmp461;}));
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp460->impl))->tagged;};}}else{_LL26B: _LL26C:
 return 0;}_LL268:;}
# 2334
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 2337
void*_tmp467=e->r;void*_tmp468=_tmp467;struct Cyc_Absyn_Exp*_tmp474;struct _dyneither_ptr*_tmp473;int*_tmp472;struct Cyc_Absyn_Exp*_tmp471;struct _dyneither_ptr*_tmp470;int*_tmp46F;struct Cyc_Absyn_Exp*_tmp46E;switch(*((int*)_tmp468)){case 13: _LL26E: _tmp46E=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp468)->f2;_LL26F:
({void*_tmp469=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp46A="cast on lhs!";_tag_dyneither(_tmp46A,sizeof(char),13);}),_tag_dyneither(_tmp469,sizeof(void*),0));});case 20: _LL270: _tmp471=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp468)->f1;_tmp470=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp468)->f2;_tmp46F=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp468)->f4;_LL271:
# 2340
 return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(_tmp471->topt),_tmp470,f_tag,tagged_member_type,clear_read,_tmp46F);case 21: _LL272: _tmp474=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp468)->f1;_tmp473=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp468)->f2;_tmp472=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp468)->f4;_LL273: {
# 2343
void*_tmp46B=Cyc_Tcutil_compress((void*)_check_null(_tmp474->topt));void*_tmp46C=_tmp46B;void*_tmp46D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46C)->tag == 5){_LL277: _tmp46D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46C)->f1).elt_typ;_LL278:
# 2345
 return Cyc_Toc_is_tagged_union_project_impl(_tmp46D,_tmp473,f_tag,tagged_member_type,clear_read,_tmp472);}else{_LL279: _LL27A:
# 2347
 return 0;}_LL276:;}default: _LL274: _LL275:
# 2349
 return 0;}_LL26D:;}
# 2361 "toc.cyc"
void Cyc_Toc_add_tagged_union_check_for_swap(struct Cyc_Absyn_Exp*e,int tag,void*mem_type){
struct _tuple1*_tmp475=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp475,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_val,0),0);
struct Cyc_Absyn_Exp*_tmp476=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_ifthenelse_stmt(_tmp476,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp475,Cyc_Absyn_cstar_typ(mem_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(e)),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
e->r=Cyc_Toc_stmt_exp_r(s1);}
# 2385 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 2389
struct _tuple1*_tmp477=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp477,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{
# 2399
struct Cyc_Absyn_Exp*_tmp478=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
s2=Cyc_Absyn_ifthenelse_stmt(_tmp478,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{
# 2402
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp477,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(e1),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple28{struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};
# 2408
static void Cyc_Toc_new_comprehension_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,void*new_e_type,struct Cyc_Absyn_Exp*rgnopt,void*old_typ,struct Cyc_Absyn_Exp*e1,void*t1,int zero_term,struct Cyc_Absyn_Exp*init_exp,struct Cyc_Absyn_Vardecl*vd){
# 2423 "toc.cyc"
struct _tuple1*max=Cyc_Toc_temp_var();
struct _tuple1*a=Cyc_Toc_temp_var();
void*old_elt_typ=t1;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c(nv,e1);{
struct Cyc_Absyn_Exp*_tmp479=Cyc_Absyn_var_exp(max,0);
# 2431
struct Cyc_Absyn_Stmt*s;
if(zero_term)
_tmp479=Cyc_Absyn_add_exp(_tmp479,Cyc_Absyn_uint_exp(1,0),0);
if(init_exp != 0)
s=Cyc_Toc_init_comprehension(nv,new_e_type,
Cyc_Absyn_var_exp(a,0),(struct Cyc_Absyn_Vardecl*)_check_null(vd),Cyc_Absyn_var_exp(max,0),init_exp,zero_term,
Cyc_Toc_skip_stmt_dl(),1);else{
if(zero_term)
s=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(a,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(elt_typ,Cyc_Absyn_uint_exp(0,0)),0),0);else{
# 2442
s=Cyc_Absyn_skip_stmt(0);}}{
# 2444
struct _RegionHandle _tmp47A=_new_region("r");struct _RegionHandle*r=& _tmp47A;_push_region(r);
{struct Cyc_List_List*decls=({struct Cyc_List_List*_tmp48E=_region_malloc(r,sizeof(*_tmp48E));_tmp48E->hd=({struct _tuple28*_tmp48F=_region_malloc(r,sizeof(*_tmp48F));_tmp48F->f1=max;_tmp48F->f2=Cyc_Absyn_uint_typ;_tmp48F->f3=e1;_tmp48F;});_tmp48E->tl=0;_tmp48E;});
# 2447
struct Cyc_Absyn_Exp*ai;
if(rgnopt == 0  || Cyc_Absyn_no_regions)
ai=Cyc_Toc_malloc_exp(old_elt_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp47B[2];_tmp47B[1]=_tmp479;_tmp47B[0]=
Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp47B,sizeof(struct Cyc_Absyn_Exp*),2));}),0));else{
# 2454
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);
ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp47C[2];_tmp47C[1]=_tmp479;_tmp47C[0]=
Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp47C,sizeof(struct Cyc_Absyn_Exp*),2));}),0));}{
# 2460
struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);
decls=({struct Cyc_List_List*_tmp47D=_region_malloc(r,sizeof(*_tmp47D));_tmp47D->hd=({struct _tuple28*_tmp47E=_region_malloc(r,sizeof(*_tmp47E));_tmp47E->f1=a;_tmp47E->f2=ptr_typ;_tmp47E->f3=ainit;_tmp47E;});_tmp47D->tl=decls;_tmp47D;});
if(!Cyc_Tcutil_is_pointer_type(old_typ))
({void*_tmp47F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp480="new_comprehension_to_c: comprehension is not an array type";_tag_dyneither(_tmp480,sizeof(char),59);}),_tag_dyneither(_tmp47F,sizeof(void*),0));});
if(Cyc_Tcutil_is_dyneither_ptr(old_typ)){
struct _tuple1*_tmp481=Cyc_Toc_temp_var();
void*_tmp482=Cyc_Toc_typ_to_c(old_typ);
struct Cyc_Absyn_Exp*_tmp483=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmp484=
Cyc_Absyn_fncall_exp(_tmp483,({struct Cyc_Absyn_Exp*_tmp487[3];_tmp487[2]=_tmp479;_tmp487[1]=
Cyc_Absyn_sizeoftyp_exp(elt_typ,0);_tmp487[0]=Cyc_Absyn_var_exp(a,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp487,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
# 2472
decls=({struct Cyc_List_List*_tmp485=_region_malloc(r,sizeof(*_tmp485));_tmp485->hd=({struct _tuple28*_tmp486=_region_malloc(r,sizeof(*_tmp486));_tmp486->f1=_tmp481;_tmp486->f2=_tmp482;_tmp486->f3=_tmp484;_tmp486;});_tmp485->tl=decls;_tmp485;});
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp481,0),0),0);}else{
# 2475
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}
{struct Cyc_List_List*_tmp488=decls;for(0;_tmp488 != 0;_tmp488=_tmp488->tl){
struct _tuple28 _tmp489=*((struct _tuple28*)_tmp488->hd);struct _tuple28 _tmp48A=_tmp489;struct _tuple1*_tmp48D;void*_tmp48C;struct Cyc_Absyn_Exp*_tmp48B;_LL27C: _tmp48D=_tmp48A.f1;_tmp48C=_tmp48A.f2;_tmp48B=_tmp48A.f3;_LL27D:;
s=Cyc_Absyn_declare_stmt(_tmp48D,_tmp48C,_tmp48B,s,0);}}
# 2480
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2445
;_pop_region(r);};};}struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 2484
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
void*_tmp490=e->r;
if(e->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp493;_tmp493.tag=0;_tmp493.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp491[1]={& _tmp493};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp492="exp_to_c: no type for %s";_tag_dyneither(_tmp492,sizeof(char),25);}),_tag_dyneither(_tmp491,sizeof(void*),1));});});
# 2490
if((nv->struct_info).lhs_exp != 0){
void*_tmp494=_tmp490;if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp494)->tag == 28){_LL27F: _LL280:
 goto _LL27E;}else{_LL281: _LL282:
 nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);}_LL27E:;}{
# 2498
void*old_typ=(void*)_check_null(e->topt);
void*_tmp495=_tmp490;struct Cyc_Absyn_Stmt*_tmp6A7;struct Cyc_Absyn_Exp*_tmp6A6;struct _dyneither_ptr*_tmp6A5;struct Cyc_Absyn_Exp*_tmp6A4;struct Cyc_Absyn_Exp*_tmp6A3;int _tmp6A2;struct Cyc_Absyn_Exp*_tmp6A1;void**_tmp6A0;struct Cyc_Absyn_Exp*_tmp69F;int _tmp69E;int _tmp69D;struct Cyc_List_List*_tmp69C;struct Cyc_Absyn_Datatypedecl*_tmp69B;struct Cyc_Absyn_Datatypefield*_tmp69A;void*_tmp699;struct Cyc_List_List*_tmp698;struct _tuple1*_tmp697;struct Cyc_List_List*_tmp696;struct Cyc_List_List*_tmp695;struct Cyc_Absyn_Aggrdecl*_tmp694;struct Cyc_Absyn_Exp*_tmp693;void*_tmp692;int _tmp691;struct Cyc_Absyn_Vardecl*_tmp690;struct Cyc_Absyn_Exp*_tmp68F;struct Cyc_Absyn_Exp*_tmp68E;int _tmp68D;struct Cyc_List_List*_tmp68C;struct Cyc_List_List*_tmp68B;struct Cyc_Absyn_Exp*_tmp68A;struct Cyc_Absyn_Exp*_tmp689;struct Cyc_Absyn_Exp*_tmp688;struct _dyneither_ptr*_tmp687;int _tmp686;int _tmp685;struct Cyc_Absyn_Exp*_tmp684;struct _dyneither_ptr*_tmp683;int _tmp682;int _tmp681;struct Cyc_Absyn_Exp*_tmp680;void*_tmp67F;struct Cyc_List_List*_tmp67E;void*_tmp67D;struct Cyc_Absyn_Exp*_tmp67C;struct Cyc_Absyn_Exp*_tmp67B;struct Cyc_Absyn_Exp*_tmp67A;struct Cyc_Absyn_Exp*_tmp679;void**_tmp678;struct Cyc_Absyn_Exp*_tmp677;int _tmp676;enum Cyc_Absyn_Coercion _tmp675;struct Cyc_Absyn_Exp*_tmp674;struct Cyc_List_List*_tmp673;struct Cyc_Absyn_Exp*_tmp672;struct Cyc_Absyn_Exp*_tmp671;int _tmp670;struct Cyc_Absyn_Exp*_tmp66F;struct Cyc_List_List*_tmp66E;int _tmp66D;struct Cyc_List_List*_tmp66C;struct Cyc_Absyn_VarargInfo*_tmp66B;struct Cyc_Absyn_Exp*_tmp66A;struct Cyc_List_List*_tmp669;struct Cyc_Absyn_Exp*_tmp668;struct Cyc_Absyn_Exp*_tmp667;struct Cyc_Absyn_Exp*_tmp666;struct Cyc_Absyn_Exp*_tmp665;struct Cyc_Absyn_Exp*_tmp664;struct Cyc_Absyn_Exp*_tmp663;struct Cyc_Absyn_Exp*_tmp662;struct Cyc_Absyn_Exp*_tmp661;struct Cyc_Absyn_Exp*_tmp660;struct Cyc_Absyn_Exp*_tmp65F;struct Cyc_Core_Opt*_tmp65E;struct Cyc_Absyn_Exp*_tmp65D;struct Cyc_Absyn_Exp*_tmp65C;enum Cyc_Absyn_Incrementor _tmp65B;enum Cyc_Absyn_Primop _tmp65A;struct Cyc_List_List*_tmp659;switch(*((int*)_tmp495)){case 0: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp495)->f1).Null_c).tag == 1){_LL284: _LL285: {
# 2505
struct Cyc_Absyn_Exp*_tmp496=Cyc_Absyn_uint_exp(0,0);
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp496,_tmp496))->r;else{
# 2510
e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*_tmp497[3];_tmp497[2]=_tmp496;_tmp497[1]=_tmp496;_tmp497[0]=_tmp496;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp497,sizeof(struct Cyc_Absyn_Exp*),3));}));}}else{
# 2512
e->r=(void*)& Cyc_Toc_zero_exp;}
# 2514
goto _LL283;}}else{_LL286: _LL287:
 goto _LL283;}case 1: _LL288: _LL289:
 goto _LL283;case 2: _LL28A: _tmp65A=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp495)->f1;_tmp659=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp495)->f2;_LL28B: {
# 2519
struct Cyc_List_List*_tmp498=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp659);
# 2521
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp659);
{enum Cyc_Absyn_Primop _tmp499=_tmp65A;switch(_tmp499){case Cyc_Absyn_Numelts: _LL2D9: _LL2DA: {
# 2524
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp659))->hd;
{void*_tmp49A=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_tmp49B=_tmp49A;void*_tmp4AD;union Cyc_Absyn_Constraint*_tmp4AC;union Cyc_Absyn_Constraint*_tmp4AB;union Cyc_Absyn_Constraint*_tmp4AA;struct Cyc_Absyn_Exp*_tmp4A9;switch(*((int*)_tmp49B)){case 8: _LL2EE: _tmp4A9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp49B)->f1).num_elts;_LL2EF:
# 2528
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp4A9)))
({void*_tmp49C=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp49D="can't calculate numelts";_tag_dyneither(_tmp49D,sizeof(char),24);}),_tag_dyneither(_tmp49C,sizeof(void*),0));});
e->r=_tmp4A9->r;goto _LL2ED;case 5: _LL2F0: _tmp4AD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49B)->f1).elt_typ;_tmp4AC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49B)->f1).ptr_atts).nullable;_tmp4AB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49B)->f1).ptr_atts).bounds;_tmp4AA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49B)->f1).ptr_atts).zero_term;_LL2F1:
# 2532
{void*_tmp49E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp4AB);void*_tmp49F=_tmp49E;struct Cyc_Absyn_Exp*_tmp4A4;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp49F)->tag == 0){_LL2F5: _LL2F6:
# 2534
 e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,({struct Cyc_Absyn_Exp*_tmp4A0[2];_tmp4A0[1]=
# 2536
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp4AD),0);_tmp4A0[0]=(struct Cyc_Absyn_Exp*)_tmp659->hd;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4A0,sizeof(struct Cyc_Absyn_Exp*),2));}));
goto _LL2F4;}else{_LL2F7: _tmp4A4=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp49F)->f1;_LL2F8:
# 2540
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4AA)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp659->hd);
# 2544
e->r=Cyc_Toc_fncall_exp_r(function_e,({struct Cyc_Absyn_Exp*_tmp4A1[2];_tmp4A1[1]=_tmp4A4;_tmp4A1[0]=(struct Cyc_Absyn_Exp*)_tmp659->hd;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4A1,sizeof(struct Cyc_Absyn_Exp*),2));}));}else{
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4AC)){
if(!Cyc_Evexp_c_can_eval(_tmp4A4))
({void*_tmp4A2=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp4A3="can't calculate numelts";_tag_dyneither(_tmp4A3,sizeof(char),24);}),_tag_dyneither(_tmp4A2,sizeof(void*),0));});
# 2549
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp4A4,Cyc_Absyn_uint_exp(0,0));}else{
# 2551
e->r=_tmp4A4->r;goto _LL2F4;}}
# 2553
goto _LL2F4;}_LL2F4:;}
# 2555
goto _LL2ED;default: _LL2F2: _LL2F3:
# 2557
({struct Cyc_String_pa_PrintArg_struct _tmp4A8;_tmp4A8.tag=0;_tmp4A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp4A7;_tmp4A7.tag=0;_tmp4A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));({void*_tmp4A5[2]={& _tmp4A7,& _tmp4A8};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4A6="size primop applied to non-array %s (%s)";_tag_dyneither(_tmp4A6,sizeof(char),41);}),_tag_dyneither(_tmp4A5,sizeof(void*),2));});});});}_LL2ED:;}
# 2560
goto _LL2D8;}case Cyc_Absyn_Plus: _LL2DB: _LL2DC:
# 2565
{void*_tmp4AE=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp498))->hd);void*_tmp4AF=_tmp4AE;void*_tmp4B7;union Cyc_Absyn_Constraint*_tmp4B6;union Cyc_Absyn_Constraint*_tmp4B5;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AF)->tag == 5){_LL2FA: _tmp4B7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AF)->f1).elt_typ;_tmp4B6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AF)->f1).ptr_atts).bounds;_tmp4B5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AF)->f1).ptr_atts).zero_term;_LL2FB:
# 2567
{void*_tmp4B0=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp4B6);void*_tmp4B1=_tmp4B0;struct Cyc_Absyn_Exp*_tmp4B4;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp4B1)->tag == 0){_LL2FF: _LL300: {
# 2569
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp659))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp659->tl))->hd;
e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp4B2[3];_tmp4B2[2]=e2;_tmp4B2[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp4B7),0);_tmp4B2[0]=e1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4B2,sizeof(struct Cyc_Absyn_Exp*),3));}));
goto _LL2FE;}}else{_LL301: _tmp4B4=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4B1)->f1;_LL302:
# 2575
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4B5)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp659))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp659->tl))->hd;
e->r=(Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1),({struct Cyc_Absyn_Exp*_tmp4B3[3];_tmp4B3[2]=e2;_tmp4B3[1]=_tmp4B4;_tmp4B3[0]=e1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4B3,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r;}
# 2580
goto _LL2FE;}_LL2FE:;}
# 2582
goto _LL2F9;}else{_LL2FC: _LL2FD:
# 2584
 goto _LL2F9;}_LL2F9:;}
# 2586
goto _LL2D8;case Cyc_Absyn_Minus: _LL2DD: _LL2DE: {
# 2591
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(_tmp498))->hd,& elt_typ)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp659))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp659->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(_tmp498->tl))->hd)){
e1->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);
e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp);
e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{
# 2601
e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp4B8[3];_tmp4B8[2]=
# 2603
Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0);_tmp4B8[1]=
# 2602
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp4B8[0]=e1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4B8,sizeof(struct Cyc_Absyn_Exp*),3));}));}}
# 2606
goto _LL2D8;}case Cyc_Absyn_Eq: _LL2DF: _LL2E0:
 goto _LL2E2;case Cyc_Absyn_Neq: _LL2E1: _LL2E2:
 goto _LL2E4;case Cyc_Absyn_Gt: _LL2E3: _LL2E4:
 goto _LL2E6;case Cyc_Absyn_Gte: _LL2E5: _LL2E6:
 goto _LL2E8;case Cyc_Absyn_Lt: _LL2E7: _LL2E8:
 goto _LL2EA;case Cyc_Absyn_Lte: _LL2E9: _LL2EA: {
# 2614
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp659))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp659->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp498))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp498->tl))->hd;
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& elt_typ))
e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,0));
if(Cyc_Tcutil_is_tagged_pointer_typ(t2))
e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp,0));
goto _LL2D8;}default: _LL2EB: _LL2EC:
 goto _LL2D8;}_LL2D8:;}
# 2626
goto _LL283;}case 4: _LL28C: _tmp65C=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp495)->f1;_tmp65B=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp495)->f2;_LL28D: {
# 2628
void*e2_cyc_typ=(void*)_check_null(_tmp65C->topt);
# 2637 "toc.cyc"
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
struct _dyneither_ptr incr_str=({const char*_tmp4CF="increment";_tag_dyneither(_tmp4CF,sizeof(char),10);});
if(_tmp65B == Cyc_Absyn_PreDec  || _tmp65B == Cyc_Absyn_PostDec)incr_str=({const char*_tmp4B9="decrement";_tag_dyneither(_tmp4B9,sizeof(char),10);});
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp65C,& ptr_type,& is_dyneither,& elt_type)){
({struct Cyc_String_pa_PrintArg_struct _tmp4BC;_tmp4BC.tag=0;_tmp4BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);({void*_tmp4BA[1]={& _tmp4BC};Cyc_Tcutil_terr(e->loc,({const char*_tmp4BB="in-place %s is not supported when dereferencing a zero-terminated pointer";_tag_dyneither(_tmp4BB,sizeof(char),74);}),_tag_dyneither(_tmp4BA,sizeof(void*),1));});});
({void*_tmp4BD=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4BE="in-place inc/dec on zero-term";_tag_dyneither(_tmp4BE,sizeof(char),30);}),_tag_dyneither(_tmp4BD,sizeof(void*),0));});}{
# 2646
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
int f_tag=0;
if(Cyc_Toc_is_tagged_union_project(_tmp65C,& f_tag,& tunion_member_type,1)){
struct Cyc_Absyn_Exp*_tmp4BF=Cyc_Absyn_signed_int_exp(1,0);
_tmp4BF->topt=Cyc_Absyn_sint_typ;{
enum Cyc_Absyn_Incrementor _tmp4C0=_tmp65B;switch(_tmp4C0){case Cyc_Absyn_PreInc: _LL304: _LL305:
# 2653
 e->r=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp4C1=_cycalloc(sizeof(*_tmp4C1));_tmp4C1[0]=({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmp4C2;_tmp4C2.tag=3;_tmp4C2.f1=_tmp65C;_tmp4C2.f2=({struct Cyc_Core_Opt*_tmp4C3=_cycalloc_atomic(sizeof(*_tmp4C3));_tmp4C3->v=(void*)Cyc_Absyn_Plus;_tmp4C3;});_tmp4C2.f3=_tmp4BF;_tmp4C2;});_tmp4C1;});
Cyc_Toc_exp_to_c(nv,e);
return;case Cyc_Absyn_PreDec: _LL306: _LL307:
# 2657
 e->r=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp4C4=_cycalloc(sizeof(*_tmp4C4));_tmp4C4[0]=({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmp4C5;_tmp4C5.tag=3;_tmp4C5.f1=_tmp65C;_tmp4C5.f2=({struct Cyc_Core_Opt*_tmp4C6=_cycalloc_atomic(sizeof(*_tmp4C6));_tmp4C6->v=(void*)Cyc_Absyn_Minus;_tmp4C6;});_tmp4C5.f3=_tmp4BF;_tmp4C5;});_tmp4C4;});
Cyc_Toc_exp_to_c(nv,e);
return;default: _LL308: _LL309:
# 2661
({struct Cyc_String_pa_PrintArg_struct _tmp4C9;_tmp4C9.tag=0;_tmp4C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);({void*_tmp4C7[1]={& _tmp4C9};Cyc_Tcutil_terr(e->loc,({const char*_tmp4C8="in-place post-%s is not supported on @tagged union members";_tag_dyneither(_tmp4C8,sizeof(char),59);}),_tag_dyneither(_tmp4C7,sizeof(void*),1));});});
# 2663
({void*_tmp4CA=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4CB="in-place inc/dec on @tagged union";_tag_dyneither(_tmp4CB,sizeof(char),34);}),_tag_dyneither(_tmp4CA,sizeof(void*),0));});}_LL303:;};}
# 2666
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp65C);
Cyc_Toc_set_lhs(nv,0);{
# 2671
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp65B == Cyc_Absyn_PostInc  || _tmp65B == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2677
if(_tmp65B == Cyc_Absyn_PreDec  || _tmp65B == Cyc_Absyn_PostDec)
change=-1;
e->r=Cyc_Toc_fncall_exp_r(fn_e,({struct Cyc_Absyn_Exp*_tmp4CC[3];_tmp4CC[2]=
# 2681
Cyc_Absyn_signed_int_exp(change,0);_tmp4CC[1]=
# 2680
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp4CC[0]=
# 2679
Cyc_Toc_push_address_exp(_tmp65C);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4CC,sizeof(struct Cyc_Absyn_Exp*),3));}));}else{
# 2682
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2686
struct Cyc_Toc_functionSet*_tmp4CD=_tmp65B != Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
# 2688
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp4CD,_tmp65C);
e->r=Cyc_Toc_fncall_exp_r(fn_e,({struct Cyc_Absyn_Exp*_tmp4CE[2];_tmp4CE[1]=
Cyc_Absyn_signed_int_exp(1,0);_tmp4CE[0]=
# 2689
Cyc_Toc_push_address_exp(_tmp65C);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4CE,sizeof(struct Cyc_Absyn_Exp*),2));}));}else{
# 2691
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp65C)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp65C,0,Cyc_Toc_incr_lvalue,_tmp65B);
e->r=_tmp65C->r;}}}
# 2695
goto _LL283;};};}case 3: _LL28E: _tmp65F=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp495)->f1;_tmp65E=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp495)->f2;_tmp65D=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp495)->f3;_LL28F: {
# 2714 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp65F->topt);
void*e2_old_typ=(void*)_check_null(_tmp65D->topt);
int f_tag=0;
void*tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Toc_is_tagged_union_project(_tmp65F,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp65F);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp65D);
e->r=Cyc_Toc_tagged_union_assignop(_tmp65F,e1_old_typ,_tmp65E,_tmp65D,e2_old_typ,f_tag,tagged_member_struct_type);
# 2725
return;}{
# 2727
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp65F,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp65F,_tmp65E,_tmp65D,ptr_type,is_dyneither,elt_type);
# 2733
return;}{
# 2737
int e1_poly=Cyc_Toc_is_poly_project(_tmp65F);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp65F);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp65D);{
# 2743
int done=0;
if(_tmp65E != 0){
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp4D0=(enum Cyc_Absyn_Primop)_tmp65E->v;enum Cyc_Absyn_Primop _tmp4D1=_tmp4D0;switch(_tmp4D1){case Cyc_Absyn_Plus: _LL30B: _LL30C:
 change=_tmp65D;goto _LL30A;case Cyc_Absyn_Minus: _LL30D: _LL30E:
# 2751
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp65D,0);goto _LL30A;default: _LL30F: _LL310:
({void*_tmp4D2=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4D3="bad t ? pointer arithmetic";_tag_dyneither(_tmp4D3,sizeof(char),27);}),_tag_dyneither(_tmp4D2,sizeof(void*),0));});}_LL30A:;}
# 2754
done=1;{
# 2756
struct Cyc_Absyn_Exp*_tmp4D4=Cyc_Toc__dyneither_ptr_inplace_plus_e;
e->r=Cyc_Toc_fncall_exp_r(_tmp4D4,({struct Cyc_Absyn_Exp*_tmp4D5[3];_tmp4D5[2]=change;_tmp4D5[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp4D5[0]=
# 2757
Cyc_Toc_push_address_exp(_tmp65F);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4D5,sizeof(struct Cyc_Absyn_Exp*),3));}));};}else{
# 2760
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2763
enum Cyc_Absyn_Primop _tmp4D6=(enum Cyc_Absyn_Primop)_tmp65E->v;enum Cyc_Absyn_Primop _tmp4D7=_tmp4D6;if(_tmp4D7 == Cyc_Absyn_Plus){_LL312: _LL313:
# 2765
 done=1;
e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp65F),({struct Cyc_Absyn_Exp*_tmp4D8[2];_tmp4D8[1]=_tmp65D;_tmp4D8[0]=_tmp65F;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4D8,sizeof(struct Cyc_Absyn_Exp*),2));}));
goto _LL311;}else{_LL314: _LL315:
({void*_tmp4D9=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4DA="bad zero-terminated pointer arithmetic";_tag_dyneither(_tmp4DA,sizeof(char),39);}),_tag_dyneither(_tmp4D9,sizeof(void*),0));});}_LL311:;}}}
# 2772
if(!done){
# 2774
if(e1_poly)
_tmp65D->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp65D->r,0));
# 2780
if(!Cyc_Absyn_is_lvalue(_tmp65F)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple27*),struct _tuple27*f_env))Cyc_Toc_lvalue_assign)(_tmp65F,0,Cyc_Toc_assignop_lvalue,({struct _tuple27*_tmp4DB=_cycalloc(sizeof(struct _tuple27)* 1);_tmp4DB[0]=(struct _tuple27)({struct _tuple27 _tmp4DC;_tmp4DC.f1=_tmp65E;_tmp4DC.f2=_tmp65D;_tmp4DC;});_tmp4DB;}));
e->r=_tmp65F->r;}}
# 2785
goto _LL283;};};};}case 5: _LL290: _tmp662=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp495)->f1;_tmp661=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp495)->f2;_tmp660=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp495)->f3;_LL291:
# 2787
 Cyc_Toc_exp_to_c(nv,_tmp662);
Cyc_Toc_exp_to_c(nv,_tmp661);
Cyc_Toc_exp_to_c(nv,_tmp660);
goto _LL283;case 6: _LL292: _tmp664=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp495)->f1;_tmp663=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp495)->f2;_LL293:
# 2792
 Cyc_Toc_exp_to_c(nv,_tmp664);
Cyc_Toc_exp_to_c(nv,_tmp663);
goto _LL283;case 7: _LL294: _tmp666=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp495)->f1;_tmp665=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp495)->f2;_LL295:
# 2796
 Cyc_Toc_exp_to_c(nv,_tmp666);
Cyc_Toc_exp_to_c(nv,_tmp665);
goto _LL283;case 8: _LL296: _tmp668=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp495)->f1;_tmp667=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp495)->f2;_LL297:
# 2800
 Cyc_Toc_exp_to_c(nv,_tmp668);
Cyc_Toc_exp_to_c(nv,_tmp667);
goto _LL283;case 9: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp495)->f3 == 0){_LL298: _tmp66A=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp495)->f1;_tmp669=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp495)->f2;_LL299:
# 2804
 Cyc_Toc_exp_to_c(nv,_tmp66A);
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp669);
goto _LL283;}else{_LL29A: _tmp66F=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp495)->f1;_tmp66E=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp495)->f2;_tmp66D=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp495)->f3)->num_varargs;_tmp66C=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp495)->f3)->injectors;_tmp66B=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp495)->f3)->vai;_LL29B: {
# 2816 "toc.cyc"
struct _RegionHandle _tmp4DD=_new_region("r");struct _RegionHandle*r=& _tmp4DD;_push_region(r);{
struct _tuple1*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp66D,0);
void*cva_type=Cyc_Toc_typ_to_c(_tmp66B->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_conref,0);
# 2824
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp66E);
int num_normargs=num_args - _tmp66D;
# 2828
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp66E=_tmp66E->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp66E))->hd);
new_args=({struct Cyc_List_List*_tmp4DE=_cycalloc(sizeof(*_tmp4DE));_tmp4DE->hd=(struct Cyc_Absyn_Exp*)_tmp66E->hd;_tmp4DE->tl=new_args;_tmp4DE;});}}
# 2833
new_args=({struct Cyc_List_List*_tmp4DF=_cycalloc(sizeof(*_tmp4DF));_tmp4DF->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*_tmp4E0[3];_tmp4E0[2]=num_varargs_exp;_tmp4E0[1]=
# 2835
Cyc_Absyn_sizeoftyp_exp(cva_type,0);_tmp4E0[0]=argvexp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4E0,sizeof(struct Cyc_Absyn_Exp*),3));}),0);_tmp4DF->tl=new_args;_tmp4DF;});
# 2838
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);
# 2840
Cyc_Toc_exp_to_c(nv,_tmp66F);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp66F,new_args,0),0);
# 2844
if(_tmp66B->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp4E1=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp66B->type));void*_tmp4E2=_tmp4E1;struct Cyc_Absyn_Datatypedecl*_tmp4E5;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4E2)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4E2)->f1).datatype_info).KnownDatatype).tag == 2){_LL317: _tmp4E5=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4E2)->f1).datatype_info).KnownDatatype).val;_LL318:
 tud=_tmp4E5;goto _LL316;}else{goto _LL319;}}else{_LL319: _LL31A:
({void*_tmp4E3=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4E4="toc: unknown datatype in vararg with inject";_tag_dyneither(_tmp4E4,sizeof(char),44);}),_tag_dyneither(_tmp4E3,sizeof(void*),0));});}_LL316:;}{
# 2850
struct _dyneither_ptr vs=({unsigned int _tmp4EE=(unsigned int)_tmp66D;struct _tuple1**_tmp4EF=(struct _tuple1**)_region_malloc(r,_check_times(sizeof(struct _tuple1*),_tmp4EE));struct _dyneither_ptr _tmp4F1=_tag_dyneither(_tmp4EF,sizeof(struct _tuple1*),_tmp4EE);{unsigned int _tmp4F0=_tmp4EE;unsigned int i;for(i=0;i < _tmp4F0;i ++){_tmp4EF[i]=(struct _tuple1*)Cyc_Toc_temp_var();}}_tmp4F1;});
# 2852
if(_tmp66D != 0){
# 2854
struct Cyc_List_List*_tmp4E6=0;
{int i=_tmp66D - 1;for(0;i >= 0;-- i){
_tmp4E6=({struct Cyc_List_List*_tmp4E7=_cycalloc(sizeof(*_tmp4E7));_tmp4E7->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0),0));_tmp4E7->tl=_tmp4E6;_tmp4E7;});}}
# 2858
s=Cyc_Absyn_declare_stmt(argv,arr_type,Cyc_Absyn_unresolvedmem_exp(0,_tmp4E6,0),s,0);{
# 2861
int i=0;for(0;_tmp66E != 0;(((_tmp66E=_tmp66E->tl,_tmp66C=_tmp66C->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp66E->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0);
struct Cyc_Absyn_Datatypefield*_tmp4E8=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp66C))->hd;struct Cyc_Absyn_Datatypefield*_tmp4E9=_tmp4E8;struct _tuple1*_tmp4EB;struct Cyc_List_List*_tmp4EA;_LL31C: _tmp4EB=_tmp4E9->name;_tmp4EA=_tmp4E9->typs;_LL31D:;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp4EA))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);
# 2876
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);
# 2879
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Toc_tag_sp,0),
Cyc_Toc_datatype_tag(tud,_tmp4EB),0),s,0);
# 2882
s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp4EB,tud->name)),0,s,0);};}};}else{
# 2889
struct Cyc_List_List*_tmp4EC=({struct _tuple19*_tmp4ED[3];_tmp4ED[2]=
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0));_tmp4ED[1]=
# 2889
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0));_tmp4ED[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0));((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4ED,sizeof(struct _tuple19*),3));});
# 2891
s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),Cyc_Absyn_uint_exp(0,0),s,0);}};}else{
# 2901
{int i=0;for(0;_tmp66E != 0;(_tmp66E=_tmp66E->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp66E->hd);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp66E->hd,0),s,0);}}
# 2907
s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}
# 2920 "toc.cyc"
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2922
_npop_handler(0);goto _LL283;
# 2816 "toc.cyc"
;_pop_region(r);}}case 10: _LL29C: _tmp671=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp495)->f1;_tmp670=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp495)->f2;_LL29D:
# 2925 "toc.cyc"
 Cyc_Toc_exp_to_c(nv,_tmp671);{
struct Cyc_Absyn_Exp*_tmp4F2=_tmp670?Cyc_Toc_newrethrow_exp(_tmp671): Cyc_Toc_newthrow_exp(_tmp671);
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),_tmp4F2,0))->r;
goto _LL283;};case 11: _LL29E: _tmp672=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp495)->f1;_LL29F:
 Cyc_Toc_exp_to_c(nv,_tmp672);goto _LL283;case 12: _LL2A0: _tmp674=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp495)->f1;_tmp673=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp495)->f2;_LL2A1:
# 2931
 Cyc_Toc_exp_to_c(nv,_tmp674);
# 2940 "toc.cyc"
for(0;_tmp673 != 0;_tmp673=_tmp673->tl){
enum Cyc_Absyn_KindQual _tmp4F3=(Cyc_Tcutil_typ_kind((void*)_tmp673->hd))->kind;
if(_tmp4F3 != Cyc_Absyn_EffKind  && _tmp4F3 != Cyc_Absyn_RgnKind){
{void*_tmp4F4=Cyc_Tcutil_compress((void*)_tmp673->hd);void*_tmp4F5=_tmp4F4;switch(*((int*)_tmp4F5)){case 2: _LL31F: _LL320:
 goto _LL322;case 3: _LL321: _LL322:
 continue;default: _LL323: _LL324:
# 2947
 e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp674,0))->r;
goto _LL31E;}_LL31E:;}
# 2950
break;}}
# 2953
goto _LL283;case 13: _LL2A2: _tmp678=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp495)->f1;_tmp677=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp495)->f2;_tmp676=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp495)->f3;_tmp675=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp495)->f4;_LL2A3: {
# 2955
void*old_t2=(void*)_check_null(_tmp677->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp678;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp678=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp677);
# 2962
{struct _tuple29 _tmp4F6=({struct _tuple29 _tmp531;_tmp531.f1=Cyc_Tcutil_compress(old_t2);_tmp531.f2=Cyc_Tcutil_compress(new_typ);_tmp531;});struct _tuple29 _tmp4F7=_tmp4F6;struct Cyc_Absyn_PtrInfo _tmp530;struct Cyc_Absyn_PtrInfo _tmp52F;struct Cyc_Absyn_PtrInfo _tmp52E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4F7.f1)->tag == 5)switch(*((int*)_tmp4F7.f2)){case 5: _LL326: _tmp52F=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4F7.f1)->f1;_tmp52E=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4F7.f2)->f1;_LL327: {
# 2964
int _tmp4F8=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp52F.ptr_atts).nullable);
int _tmp4F9=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp52E.ptr_atts).nullable);
void*_tmp4FA=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp52F.ptr_atts).bounds);
void*_tmp4FB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp52E.ptr_atts).bounds);
int _tmp4FC=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp52F.ptr_atts).zero_term);
int _tmp4FD=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp52E.ptr_atts).zero_term);
{struct _tuple29 _tmp4FE=({struct _tuple29 _tmp52B;_tmp52B.f1=_tmp4FA;_tmp52B.f2=_tmp4FB;_tmp52B;});struct _tuple29 _tmp4FF=_tmp4FE;struct Cyc_Absyn_Exp*_tmp52A;struct Cyc_Absyn_Exp*_tmp529;struct Cyc_Absyn_Exp*_tmp528;struct Cyc_Absyn_Exp*_tmp527;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4FF.f1)->tag == 1){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4FF.f2)->tag == 1){_LL32D: _tmp528=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4FF.f1)->f1;_tmp527=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4FF.f2)->f1;_LL32E:
# 2972
 if((!Cyc_Evexp_c_can_eval(_tmp528) || !Cyc_Evexp_c_can_eval(_tmp527)) && !
Cyc_Evexp_same_const_exp(_tmp528,_tmp527))
({void*_tmp500=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp501="can't validate cast due to potential size differences";_tag_dyneither(_tmp501,sizeof(char),54);}),_tag_dyneither(_tmp500,sizeof(void*),0));});
if(_tmp4F8  && !_tmp4F9){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp502=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp503="can't do NULL-check conversion at top-level";_tag_dyneither(_tmp503,sizeof(char),44);}),_tag_dyneither(_tmp502,sizeof(void*),0));});
# 2981
if(_tmp675 != Cyc_Absyn_NonNull_to_Null)
({struct Cyc_String_pa_PrintArg_struct _tmp506;_tmp506.tag=0;_tmp506.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp504[1]={& _tmp506};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp505="null-check conversion mis-classified: %s";_tag_dyneither(_tmp505,sizeof(char),41);}),_tag_dyneither(_tmp504,sizeof(void*),1));});});{
int do_null_check=Cyc_Toc_need_null_check(_tmp677);
if(do_null_check){
if(!_tmp676)
({void*_tmp507=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp508="inserted null check due to implicit cast from * to @ type";_tag_dyneither(_tmp508,sizeof(char),58);}),_tag_dyneither(_tmp507,sizeof(void*),0));});
# 2988
e->r=Cyc_Toc_cast_it_r(*_tmp678,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp509=_cycalloc(sizeof(*_tmp509));_tmp509->hd=_tmp677;_tmp509->tl=0;_tmp509;}),0));}else{
# 2992
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp677->r;}};}else{
# 2997
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp677->r;}
# 3004
goto _LL32C;}else{_LL32F: _tmp529=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4FF.f1)->f1;_LL330:
# 3006
 if(!Cyc_Evexp_c_can_eval(_tmp529))
({void*_tmp50A=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp50B="cannot perform coercion since numelts cannot be determined statically.";_tag_dyneither(_tmp50B,sizeof(char),71);}),_tag_dyneither(_tmp50A,sizeof(void*),0));});
# 3009
if(_tmp675 == Cyc_Absyn_NonNull_to_Null)
({struct Cyc_String_pa_PrintArg_struct _tmp50E;_tmp50E.tag=0;_tmp50E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp50C[1]={& _tmp50E};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp50D="conversion mis-classified as null-check: %s";_tag_dyneither(_tmp50D,sizeof(char),44);}),_tag_dyneither(_tmp50C,sizeof(void*),1));});});
if(Cyc_Toc_is_toplevel(nv)){
# 3013
if((_tmp4FC  && !(_tmp52E.elt_tq).real_const) && !_tmp4FD)
# 3016
_tmp529=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp529,Cyc_Absyn_uint_exp(1,0),0);
# 3018
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp529,_tmp677))->r;}else{
# 3020
struct Cyc_Absyn_Exp*_tmp50F=Cyc_Toc__tag_dyneither_e;
# 3022
if(_tmp4FC){
# 3027
struct _tuple1*_tmp510=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp511=Cyc_Absyn_var_exp(_tmp510,0);
struct Cyc_Absyn_Exp*arg3;
# 3032
{void*_tmp512=_tmp677->r;void*_tmp513=_tmp512;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp513)->tag == 0)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp513)->f1).Wstring_c).tag){case 8: _LL336: _LL337:
# 3034
 arg3=_tmp529;goto _LL335;case 9: _LL338: _LL339:
# 3036
 arg3=_tmp529;goto _LL335;default: goto _LL33A;}else{_LL33A: _LL33B:
# 3038
 arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp677),({struct Cyc_Absyn_Exp*_tmp514[2];_tmp514[1]=_tmp529;_tmp514[0]=
# 3040
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp511);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp514,sizeof(struct Cyc_Absyn_Exp*),2));}),0);
goto _LL335;}_LL335:;}
# 3043
if(!_tmp4FD  && !(_tmp52E.elt_tq).real_const)
# 3046
arg3=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{
# 3048
struct Cyc_Absyn_Exp*_tmp515=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp52E.elt_typ),0);
struct Cyc_Absyn_Exp*_tmp516=Cyc_Absyn_fncall_exp(_tmp50F,({struct Cyc_Absyn_Exp*_tmp518[3];_tmp518[2]=arg3;_tmp518[1]=_tmp515;_tmp518[0]=_tmp511;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp518,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
struct Cyc_Absyn_Stmt*_tmp517=Cyc_Absyn_exp_stmt(_tmp516,0);
_tmp517=Cyc_Absyn_declare_stmt(_tmp510,Cyc_Toc_typ_to_c(old_t2),_tmp677,_tmp517,0);
e->r=Cyc_Toc_stmt_exp_r(_tmp517);};}else{
# 3055
e->r=Cyc_Toc_fncall_exp_r(_tmp50F,({struct Cyc_Absyn_Exp*_tmp519[3];_tmp519[2]=_tmp529;_tmp519[1]=
# 3057
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp52E.elt_typ),0);_tmp519[0]=_tmp677;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp519,sizeof(struct Cyc_Absyn_Exp*),3));}));}}
# 3061
goto _LL32C;}}else{if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4FF.f2)->tag == 1){_LL331: _tmp52A=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4FF.f2)->f1;_LL332:
# 3063
 if(!Cyc_Evexp_c_can_eval(_tmp52A))
({void*_tmp51A=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp51B="cannot perform coercion since numelts cannot be determined statically.";_tag_dyneither(_tmp51B,sizeof(char),71);}),_tag_dyneither(_tmp51A,sizeof(void*),0));});
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp51C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp51D="can't coerce t? to t* or t@ at the top-level";_tag_dyneither(_tmp51D,sizeof(char),45);}),_tag_dyneither(_tmp51C,sizeof(void*),0));});{
# 3075 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp51E=_tmp52A;
if(_tmp4FC  && !_tmp4FD)
_tmp51E=Cyc_Absyn_add_exp(_tmp52A,Cyc_Absyn_uint_exp(1,0),0);{
# 3082
struct Cyc_Absyn_Exp*_tmp51F=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmp520=Cyc_Absyn_fncall_exp(_tmp51F,({struct Cyc_Absyn_Exp*_tmp522[3];_tmp522[2]=_tmp51E;_tmp522[1]=
# 3085
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp52F.elt_typ),0);_tmp522[0]=_tmp677;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp522,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
# 3087
if(_tmp4F9)
_tmp520->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp521=_cycalloc(sizeof(*_tmp521));_tmp521->hd=
Cyc_Absyn_copy_exp(_tmp520);_tmp521->tl=0;_tmp521;}));
e->r=Cyc_Toc_cast_it_r(*_tmp678,_tmp520);
goto _LL32C;};};}else{_LL333: _LL334:
# 3095
 DynCast:
 if((_tmp4FC  && !_tmp4FD) && !(_tmp52E.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp523=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp524="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";_tag_dyneither(_tmp524,sizeof(char),70);}),_tag_dyneither(_tmp523,sizeof(void*),0));});{
struct Cyc_Absyn_Exp*_tmp525=Cyc_Toc__dyneither_ptr_decrease_size_e;
e->r=Cyc_Toc_fncall_exp_r(_tmp525,({struct Cyc_Absyn_Exp*_tmp526[3];_tmp526[2]=
# 3102
Cyc_Absyn_uint_exp(1,0);_tmp526[1]=
# 3101
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp52F.elt_typ),0);_tmp526[0]=_tmp677;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp526,sizeof(struct Cyc_Absyn_Exp*),3));}));};}
# 3104
goto _LL32C;}}_LL32C:;}
# 3106
goto _LL325;}case 6: _LL328: _tmp530=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4F7.f1)->f1;_LL329:
# 3108
{void*_tmp52C=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp530.ptr_atts).bounds);void*_tmp52D=_tmp52C;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp52D)->tag == 0){_LL33D: _LL33E:
# 3110
 _tmp677->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp677->r,_tmp677->loc),Cyc_Toc_curr_sp);goto _LL33C;}else{_LL33F: _LL340:
 goto _LL33C;}_LL33C:;}
# 3113
goto _LL325;default: goto _LL32A;}else{_LL32A: _LL32B:
# 3115
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp677->r;
goto _LL325;}_LL325:;}
# 3119
goto _LL283;}case 14: _LL2A4: _tmp679=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp495)->f1;_LL2A5:
# 3123
{void*_tmp532=_tmp679->r;void*_tmp533=_tmp532;struct Cyc_List_List*_tmp53E;struct _tuple1*_tmp53D;struct Cyc_List_List*_tmp53C;struct Cyc_List_List*_tmp53B;switch(*((int*)_tmp533)){case 28: _LL342: _tmp53D=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp533)->f1;_tmp53C=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp533)->f2;_tmp53B=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp533)->f3;_LL343:
# 3125
 if(Cyc_Toc_is_toplevel(nv))
({struct Cyc_String_pa_PrintArg_struct _tmp536;_tmp536.tag=0;_tmp536.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp679->loc));({void*_tmp534[1]={& _tmp536};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp535="%s: & on non-identifiers at the top-level";_tag_dyneither(_tmp535,sizeof(char),42);}),_tag_dyneither(_tmp534,sizeof(void*),1));});});{
struct Cyc_Absyn_Exp*_tmp537=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp679->topt),_tmp53C,1,0,_tmp53B,_tmp53D);
e->r=_tmp537->r;
e->topt=_tmp537->topt;
goto _LL341;};case 23: _LL344: _tmp53E=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp533)->f1;_LL345:
# 3133
 if(Cyc_Toc_is_toplevel(nv))
({struct Cyc_String_pa_PrintArg_struct _tmp53A;_tmp53A.tag=0;_tmp53A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp679->loc));({void*_tmp538[1]={& _tmp53A};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp539="%s: & on non-identifiers at the top-level";_tag_dyneither(_tmp539,sizeof(char),42);}),_tag_dyneither(_tmp538,sizeof(void*),1));});});
e->r=(Cyc_Toc_init_tuple(nv,1,0,_tmp53E))->r;
goto _LL341;default: _LL346: _LL347:
# 3139
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp679);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp679)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp679,0,Cyc_Toc_address_lvalue,1);
# 3145
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp679);}else{
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind((void*)_check_null(_tmp679->topt))))
# 3149
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp679);}
# 3151
goto _LL341;}_LL341:;}
# 3153
goto _LL283;case 15: _LL2A6: _tmp67B=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp495)->f1;_tmp67A=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp495)->f2;_LL2A7:
# 3156
 if(Cyc_Toc_is_toplevel(nv))
({struct Cyc_String_pa_PrintArg_struct _tmp541;_tmp541.tag=0;_tmp541.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp67A->loc));({void*_tmp53F[1]={& _tmp541};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp540="%s: new at top-level";_tag_dyneither(_tmp540,sizeof(char),21);}),_tag_dyneither(_tmp53F,sizeof(void*),1));});});{
void*new_e_type=(void*)_check_null(_tmp67A->topt);
{void*_tmp542=_tmp67A->r;void*_tmp543=_tmp542;struct Cyc_List_List*_tmp572;struct _tuple1*_tmp571;struct Cyc_List_List*_tmp570;struct Cyc_List_List*_tmp56F;struct Cyc_Absyn_Aggrdecl*_tmp56E;struct Cyc_Absyn_Exp*_tmp56D;void*_tmp56C;int _tmp56B;struct Cyc_Absyn_Vardecl*_tmp56A;struct Cyc_Absyn_Exp*_tmp569;struct Cyc_Absyn_Exp*_tmp568;int _tmp567;struct Cyc_List_List*_tmp566;switch(*((int*)_tmp543)){case 25: _LL349: _tmp566=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_LL34A: {
# 3164
struct _tuple1*_tmp544=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp545=Cyc_Absyn_var_exp(_tmp544,0);
struct Cyc_Absyn_Stmt*_tmp546=Cyc_Toc_init_array(nv,new_e_type,_tmp545,_tmp566,Cyc_Absyn_exp_stmt(_tmp545,0));
void*old_elt_typ;
{void*_tmp547=Cyc_Tcutil_compress(old_typ);void*_tmp548=_tmp547;void*_tmp54D;struct Cyc_Absyn_Tqual _tmp54C;union Cyc_Absyn_Constraint*_tmp54B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp548)->tag == 5){_LL356: _tmp54D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp548)->f1).elt_typ;_tmp54C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp548)->f1).elt_tq;_tmp54B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp548)->f1).ptr_atts).zero_term;_LL357:
# 3170
 old_elt_typ=_tmp54D;goto _LL355;}else{_LL358: _LL359:
# 3172
 old_elt_typ=({void*_tmp549=0;((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp54A="exp_to_c:new array expression doesn't have ptr type";_tag_dyneither(_tmp54A,sizeof(char),52);}),_tag_dyneither(_tmp549,sizeof(void*),0));});}_LL355:;}{
# 3175
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*_tmp54E=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp54F=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),
Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp566),0),0);
struct Cyc_Absyn_Exp*e1;
if(_tmp67B == 0  || Cyc_Absyn_no_regions)
e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp54F);else{
# 3183
struct Cyc_Absyn_Exp*r=_tmp67B;
Cyc_Toc_exp_to_c(nv,r);
e1=Cyc_Toc_rmalloc_exp(r,_tmp54F);}
# 3188
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp544,_tmp54E,e1,_tmp546,0));
goto _LL348;};}case 26: _LL34B: _tmp56A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_tmp569=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp543)->f2;_tmp568=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp543)->f3;_tmp567=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp543)->f4;_LL34C:
# 3192
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp67B,old_typ,_tmp569,(void*)_check_null(_tmp568->topt),_tmp567,_tmp568,_tmp56A);
goto _LL348;case 27: _LL34D: _tmp56D=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_tmp56C=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp543)->f2;_tmp56B=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp543)->f3;_LL34E:
# 3196
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp67B,old_typ,_tmp56D,_tmp56C,_tmp56B,0,0);
goto _LL348;case 28: _LL34F: _tmp571=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_tmp570=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp543)->f2;_tmp56F=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp543)->f3;_tmp56E=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp543)->f4;_LL350: {
# 3201
struct Cyc_Absyn_Exp*_tmp550=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp67A->topt),_tmp570,1,_tmp67B,_tmp56F,_tmp571);
e->r=_tmp550->r;
e->topt=_tmp550->topt;
goto _LL348;}case 23: _LL351: _tmp572=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_LL352:
# 3207
 e->r=(Cyc_Toc_init_tuple(nv,1,_tmp67B,_tmp572))->r;
goto _LL348;default: _LL353: _LL354: {
# 3214
void*old_elt_typ=(void*)_check_null(_tmp67A->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
# 3217
struct _tuple1*_tmp551=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp552=Cyc_Absyn_var_exp(_tmp551,0);
struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp552,0),0);
struct Cyc_Absyn_Exp*inner_mexp=mexp;
if(_tmp67B == 0  || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
# 3224
struct Cyc_Absyn_Exp*r=_tmp67B;
Cyc_Toc_exp_to_c(nv,r);
mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{
# 3232
int done=0;
{void*_tmp553=_tmp67A->r;void*_tmp554=_tmp553;void*_tmp562;struct Cyc_Absyn_Exp*_tmp561;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp554)->tag == 13){_LL35B: _tmp562=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp554)->f1;_tmp561=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp554)->f2;_LL35C:
# 3235
{struct _tuple29 _tmp555=({struct _tuple29 _tmp560;_tmp560.f1=Cyc_Tcutil_compress(_tmp562);_tmp560.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp561->topt));_tmp560;});struct _tuple29 _tmp556=_tmp555;void*_tmp55F;union Cyc_Absyn_Constraint*_tmp55E;union Cyc_Absyn_Constraint*_tmp55D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp556.f1)->tag == 5){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp556.f2)->tag == 5){_LL360: _tmp55F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp556.f1)->f1).elt_typ;_tmp55E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp556.f1)->f1).ptr_atts).bounds;_tmp55D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp556.f2)->f1).ptr_atts).bounds;_LL361:
# 3238
{struct _tuple29 _tmp557=({struct _tuple29 _tmp55C;_tmp55C.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp55E);_tmp55C.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp55D);_tmp55C;});struct _tuple29 _tmp558=_tmp557;struct Cyc_Absyn_Exp*_tmp55B;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp558.f1)->tag == 0){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp558.f2)->tag == 1){_LL365: _tmp55B=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp558.f2)->f1;_LL366:
# 3240
 Cyc_Toc_exp_to_c(nv,_tmp561);
inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);
done=1;{
struct Cyc_Absyn_Exp*_tmp559=Cyc_Toc__init_dyneither_ptr_e;
e->r=Cyc_Toc_fncall_exp_r(_tmp559,({struct Cyc_Absyn_Exp*_tmp55A[4];_tmp55A[3]=_tmp55B;_tmp55A[2]=
# 3246
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp55F),0);_tmp55A[1]=_tmp561;_tmp55A[0]=mexp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp55A,sizeof(struct Cyc_Absyn_Exp*),4));}));
# 3248
goto _LL364;};}else{goto _LL367;}}else{_LL367: _LL368:
 goto _LL364;}_LL364:;}
# 3251
goto _LL35F;}else{goto _LL362;}}else{_LL362: _LL363:
 goto _LL35F;}_LL35F:;}
# 3254
goto _LL35A;}else{_LL35D: _LL35E:
 goto _LL35A;}_LL35A:;}
# 3257
if(!done){
struct Cyc_Absyn_Stmt*_tmp563=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp552),0);
struct Cyc_Absyn_Exp*_tmp564=Cyc_Absyn_signed_int_exp(0,0);
Cyc_Toc_exp_to_c(nv,_tmp67A);
_tmp563=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp552,_tmp564,0),_tmp67A,0),_tmp563,0);{
# 3263
void*_tmp565=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp551,_tmp565,mexp,_tmp563,0));};}
# 3266
goto _LL348;};}}_LL348:;}
# 3268
goto _LL283;};case 17: _LL2A8: _tmp67C=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp495)->f1;_LL2A9: {
# 3271
int _tmp573=Cyc_Toc_set_in_sizeof(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp67C);
Cyc_Toc_set_in_sizeof(nv,_tmp573);
goto _LL283;}case 16: _LL2AA: _tmp67D=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp495)->f1;_LL2AB:
 e->r=(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp574=_cycalloc(sizeof(*_tmp574));_tmp574[0]=({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp575;_tmp575.tag=16;_tmp575.f1=Cyc_Toc_typ_to_c(_tmp67D);_tmp575;});_tmp574;});goto _LL283;case 18: _LL2AC: _tmp67F=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp495)->f1;_tmp67E=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp495)->f2;_LL2AD: {
# 3277
void*_tmp576=_tmp67F;
struct Cyc_List_List*_tmp577=_tmp67E;
for(0;_tmp577 != 0;_tmp577=_tmp577->tl){
void*_tmp578=(void*)_tmp577->hd;void*_tmp579=_tmp578;unsigned int _tmp58F;struct _dyneither_ptr*_tmp58E;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp579)->tag == 0){_LL36A: _tmp58E=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp579)->f1;_LL36B:
 goto _LL369;}else{_LL36C: _tmp58F=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp579)->f1;_LL36D:
# 3283
{void*_tmp57A=Cyc_Tcutil_compress(_tmp576);void*_tmp57B=_tmp57A;struct Cyc_Absyn_Datatypefield*_tmp58D;struct Cyc_List_List*_tmp58C;struct Cyc_List_List*_tmp58B;union Cyc_Absyn_AggrInfoU _tmp58A;switch(*((int*)_tmp57B)){case 11: _LL36F: _tmp58A=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp57B)->f1).aggr_info;_LL370: {
# 3285
struct Cyc_Absyn_Aggrdecl*_tmp57C=Cyc_Absyn_get_known_aggrdecl(_tmp58A);
if(_tmp57C->impl == 0)
({void*_tmp57D=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp57E="struct fields must be known";_tag_dyneither(_tmp57E,sizeof(char),28);}),_tag_dyneither(_tmp57D,sizeof(void*),0));});
_tmp58B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp57C->impl))->fields;goto _LL372;}case 12: _LL371: _tmp58B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp57B)->f2;_LL372: {
# 3290
struct Cyc_Absyn_Aggrfield*_tmp57F=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp58B,(int)_tmp58F);
_tmp577->hd=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp580=_cycalloc(sizeof(*_tmp580));_tmp580[0]=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp581;_tmp581.tag=0;_tmp581.f1=_tmp57F->name;_tmp581;});_tmp580;}));
_tmp576=_tmp57F->type;
goto _LL36E;}case 10: _LL373: _tmp58C=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp57B)->f1;_LL374:
# 3295
 _tmp577->hd=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp582=_cycalloc(sizeof(*_tmp582));_tmp582[0]=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp583;_tmp583.tag=0;_tmp583.f1=Cyc_Absyn_fieldname((int)(_tmp58F + 1));_tmp583;});_tmp582;}));
_tmp576=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp58C,(int)_tmp58F)).f2;
goto _LL36E;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp57B)->f1).field_info).KnownDatatypefield).tag == 2){_LL375: _tmp58D=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp57B)->f1).field_info).KnownDatatypefield).val).f2;_LL376:
# 3299
 if(_tmp58F == 0)
_tmp577->hd=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp584=_cycalloc(sizeof(*_tmp584));_tmp584[0]=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp585;_tmp585.tag=0;_tmp585.f1=Cyc_Toc_tag_sp;_tmp585;});_tmp584;}));else{
# 3302
_tmp576=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp58D->typs,(int)(_tmp58F - 1))).f2;
_tmp577->hd=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp586=_cycalloc(sizeof(*_tmp586));_tmp586[0]=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp587;_tmp587.tag=0;_tmp587.f1=Cyc_Absyn_fieldname((int)_tmp58F);_tmp587;});_tmp586;}));}
# 3305
goto _LL36E;}else{goto _LL377;}default: _LL377: _LL378:
({void*_tmp588=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp589="impossible type for offsetof tuple index";_tag_dyneither(_tmp589,sizeof(char),41);}),_tag_dyneither(_tmp588,sizeof(void*),0));});
goto _LL36E;}_LL36E:;}
# 3309
goto _LL369;}_LL369:;}
# 3312
e->r=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp590=_cycalloc(sizeof(*_tmp590));_tmp590[0]=({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp591;_tmp591.tag=18;_tmp591.f1=Cyc_Toc_typ_to_c(_tmp67F);_tmp591.f2=_tmp67E;_tmp591;});_tmp590;});
goto _LL283;}case 19: _LL2AE: _tmp680=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp495)->f1;_LL2AF: {
# 3315
int _tmp592=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp593=Cyc_Tcutil_compress((void*)_check_null(_tmp680->topt));
{void*_tmp594=_tmp593;void*_tmp5A9;struct Cyc_Absyn_Tqual _tmp5A8;void*_tmp5A7;union Cyc_Absyn_Constraint*_tmp5A6;union Cyc_Absyn_Constraint*_tmp5A5;union Cyc_Absyn_Constraint*_tmp5A4;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp594)->tag == 5){_LL37A: _tmp5A9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp594)->f1).elt_typ;_tmp5A8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp594)->f1).elt_tq;_tmp5A7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp594)->f1).ptr_atts).rgn;_tmp5A6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp594)->f1).ptr_atts).nullable;_tmp5A5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp594)->f1).ptr_atts).bounds;_tmp5A4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp594)->f1).ptr_atts).zero_term;_LL37B:
# 3320
{void*_tmp595=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5A5);void*_tmp596=_tmp595;struct Cyc_Absyn_Exp*_tmp5A1;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp596)->tag == 1){_LL37F: _tmp5A1=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp596)->f1;_LL380: {
# 3322
int do_null_check=!Cyc_Toc_in_sizeof(nv) && Cyc_Toc_need_null_check(_tmp680);
Cyc_Toc_exp_to_c(nv,_tmp680);
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp597=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp598="inserted null check due to dereference";_tag_dyneither(_tmp598,sizeof(char),39);}),_tag_dyneither(_tmp597,sizeof(void*),0));});
# 3328
_tmp680->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(_tmp593),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp599=_cycalloc(sizeof(*_tmp599));_tmp599->hd=
Cyc_Absyn_copy_exp(_tmp680);_tmp599->tl=0;_tmp599;}),0));}
# 3336
if(!Cyc_Toc_in_sizeof(nv) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5A4)){
struct _tuple12 _tmp59A=Cyc_Evexp_eval_const_uint_exp(_tmp5A1);struct _tuple12 _tmp59B=_tmp59A;unsigned int _tmp59F;int _tmp59E;_LL384: _tmp59F=_tmp59B.f1;_tmp59E=_tmp59B.f2;_LL385:;
# 3342
if(!_tmp59E  || _tmp59F <= 0)
({void*_tmp59C=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp59D="cannot determine dereference is in bounds";_tag_dyneither(_tmp59D,sizeof(char),42);}),_tag_dyneither(_tmp59C,sizeof(void*),0));});}
# 3345
goto _LL37E;}}else{_LL381: _LL382: {
# 3348
struct Cyc_Absyn_Exp*_tmp5A0=Cyc_Absyn_uint_exp(0,0);
_tmp5A0->topt=Cyc_Absyn_uint_typ;
e->r=Cyc_Toc_subscript_exp_r(_tmp680,_tmp5A0);
Cyc_Toc_exp_to_c(nv,e);
goto _LL37E;}}_LL37E:;}
# 3354
goto _LL379;}else{_LL37C: _LL37D:
({void*_tmp5A2=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp5A3="exp_to_c: Deref: non-pointer";_tag_dyneither(_tmp5A3,sizeof(char),29);}),_tag_dyneither(_tmp5A2,sizeof(void*),0));});}_LL379:;}
# 3357
Cyc_Toc_set_lhs(nv,_tmp592);
goto _LL283;};}case 20: _LL2B0: _tmp684=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp495)->f1;_tmp683=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp495)->f2;_tmp682=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp495)->f3;_tmp681=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp495)->f4;_LL2B1: {
# 3360
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp684->topt);
Cyc_Toc_exp_to_c(nv,_tmp684);
if(_tmp682  && _tmp681)
e->r=Cyc_Toc_check_tagged_union(_tmp684,Cyc_Toc_typ_to_c(e1_cyc_type),e1_cyc_type,_tmp683,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);
# 3368
if(is_poly)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
goto _LL283;}case 21: _LL2B2: _tmp688=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp495)->f1;_tmp687=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp495)->f2;_tmp686=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp495)->f3;_tmp685=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp495)->f4;_LL2B3: {
# 3372
int _tmp5AA=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp688->topt));
int do_null_check=Cyc_Toc_need_null_check(_tmp688);
Cyc_Toc_exp_to_c(nv,_tmp688);{
int is_poly=Cyc_Toc_is_poly_project(e);
struct Cyc_Absyn_PtrInfo _tmp5AB=Cyc_Toc_get_ptr_type(e1typ);struct Cyc_Absyn_PtrInfo _tmp5AC=_tmp5AB;void*_tmp5C2;struct Cyc_Absyn_Tqual _tmp5C1;void*_tmp5C0;union Cyc_Absyn_Constraint*_tmp5BF;union Cyc_Absyn_Constraint*_tmp5BE;union Cyc_Absyn_Constraint*_tmp5BD;_LL387: _tmp5C2=_tmp5AC.elt_typ;_tmp5C1=_tmp5AC.elt_tq;_tmp5C0=(_tmp5AC.ptr_atts).rgn;_tmp5BF=(_tmp5AC.ptr_atts).nullable;_tmp5BE=(_tmp5AC.ptr_atts).bounds;_tmp5BD=(_tmp5AC.ptr_atts).zero_term;_LL388:;
{void*_tmp5AD=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5BE);void*_tmp5AE=_tmp5AD;struct Cyc_Absyn_Exp*_tmp5BC;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5AE)->tag == 1){_LL38A: _tmp5BC=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5AE)->f1;_LL38B: {
# 3383
struct _tuple12 _tmp5AF=Cyc_Evexp_eval_const_uint_exp(_tmp5BC);struct _tuple12 _tmp5B0=_tmp5AF;unsigned int _tmp5BA;int _tmp5B9;_LL38F: _tmp5BA=_tmp5B0.f1;_tmp5B9=_tmp5B0.f2;_LL390:;
if(_tmp5B9){
if(_tmp5BA < 1)
({void*_tmp5B1=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp5B2="exp_to_c:  AggrArrow_e on pointer of size 0";_tag_dyneither(_tmp5B2,sizeof(char),44);}),_tag_dyneither(_tmp5B1,sizeof(void*),0));});
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp5B3=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp5B4="inserted null check due to dereference";_tag_dyneither(_tmp5B4,sizeof(char),39);}),_tag_dyneither(_tmp5B3,sizeof(void*),0));});
# 3391
_tmp688->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_Absyn_Exp*_tmp5B5[1];_tmp5B5[0]=
Cyc_Absyn_new_exp(_tmp688->r,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5B5,sizeof(struct Cyc_Absyn_Exp*),1));}),0));}}else{
# 3396
if(!Cyc_Evexp_c_can_eval(_tmp5BC))
({void*_tmp5B6=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp5B7="cannot determine pointer dereference in bounds";_tag_dyneither(_tmp5B7,sizeof(char),47);}),_tag_dyneither(_tmp5B6,sizeof(void*),0));});
# 3400
_tmp688->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*_tmp5B8[4];_tmp5B8[3]=
# 3404
Cyc_Absyn_uint_exp(0,0);_tmp5B8[2]=
# 3403
Cyc_Absyn_sizeoftyp_exp(_tmp5C2,0);_tmp5B8[1]=_tmp5BC;_tmp5B8[0]=
# 3402
Cyc_Absyn_new_exp(_tmp688->r,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5B8,sizeof(struct Cyc_Absyn_Exp*),4));}),0));}
# 3406
goto _LL389;}}else{_LL38C: _LL38D: {
# 3409
void*ta1=Cyc_Toc_typ_to_c(_tmp5C2);
_tmp688->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp5C1),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_dyneither_subscript_e,({struct Cyc_Absyn_Exp*_tmp5BB[3];_tmp5BB[2]=
# 3414
Cyc_Absyn_uint_exp(0,0);_tmp5BB[1]=
# 3413
Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp5BB[0]=
# 3412
Cyc_Absyn_new_exp(_tmp688->r,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5BB,sizeof(struct Cyc_Absyn_Exp*),3));}),0));
# 3417
goto _LL389;}}_LL389:;}
# 3419
if(_tmp686  && _tmp685)
e->r=Cyc_Toc_check_tagged_union(_tmp688,Cyc_Toc_typ_to_c(e1typ),_tmp5C2,_tmp687,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);
# 3422
if(is_poly  && _tmp685)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
Cyc_Toc_set_lhs(nv,_tmp5AA);
goto _LL283;};};}case 22: _LL2B4: _tmp68A=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp495)->f1;_tmp689=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp495)->f2;_LL2B5: {
# 3427
int _tmp5C3=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp5C4=Cyc_Tcutil_compress((void*)_check_null(_tmp68A->topt));
# 3431
{void*_tmp5C5=_tmp5C4;void*_tmp5EF;struct Cyc_Absyn_Tqual _tmp5EE;void*_tmp5ED;union Cyc_Absyn_Constraint*_tmp5EC;union Cyc_Absyn_Constraint*_tmp5EB;union Cyc_Absyn_Constraint*_tmp5EA;struct Cyc_List_List*_tmp5E9;switch(*((int*)_tmp5C5)){case 10: _LL392: _tmp5E9=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5C5)->f1;_LL393:
# 3434
 Cyc_Toc_exp_to_c(nv,_tmp68A);
Cyc_Toc_exp_to_c(nv,_tmp689);{
struct _tuple12 _tmp5C6=Cyc_Evexp_eval_const_uint_exp(_tmp689);struct _tuple12 _tmp5C7=_tmp5C6;unsigned int _tmp5CB;int _tmp5CA;_LL399: _tmp5CB=_tmp5C7.f1;_tmp5CA=_tmp5C7.f2;_LL39A:;
if(!_tmp5CA)
({void*_tmp5C8=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp5C9="unknown tuple subscript in translation to C";_tag_dyneither(_tmp5C9,sizeof(char),44);}),_tag_dyneither(_tmp5C8,sizeof(void*),0));});
e->r=Cyc_Toc_aggrmember_exp_r(_tmp68A,Cyc_Absyn_fieldname((int)(_tmp5CB + 1)));
goto _LL391;};case 5: _LL394: _tmp5EF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C5)->f1).elt_typ;_tmp5EE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C5)->f1).elt_tq;_tmp5ED=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C5)->f1).ptr_atts).rgn;_tmp5EC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C5)->f1).ptr_atts).nullable;_tmp5EB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C5)->f1).ptr_atts).bounds;_tmp5EA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C5)->f1).ptr_atts).zero_term;_LL395: {
# 3442
struct Cyc_List_List*_tmp5CC=Cyc_Toc_get_relns(_tmp68A);
int _tmp5CD=Cyc_Toc_need_null_check(_tmp68A);
int _tmp5CE=Cyc_Toc_in_sizeof(nv);
# 3450
int in_bnds=_tmp5CE;
{void*_tmp5CF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5EB);void*_tmp5D0=_tmp5CF;_LL39C: _LL39D:
# 3453
 in_bnds=in_bnds  || Cyc_Toc_check_bounds(_tmp5C4,_tmp5CC,_tmp68A,_tmp689);
if(Cyc_Toc_warn_bounds_checks  && !in_bnds)
({struct Cyc_String_pa_PrintArg_struct _tmp5D3;_tmp5D3.tag=0;_tmp5D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp5D1[1]={& _tmp5D3};Cyc_Tcutil_warn(e->loc,({const char*_tmp5D2="bounds check necessary for %s";_tag_dyneither(_tmp5D2,sizeof(char),30);}),_tag_dyneither(_tmp5D1,sizeof(void*),1));});});_LL39B:;}
# 3460
Cyc_Toc_exp_to_c(nv,_tmp68A);
Cyc_Toc_exp_to_c(nv,_tmp689);
++ Cyc_Toc_total_bounds_checks;
{void*_tmp5D4=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5EB);void*_tmp5D5=_tmp5D4;struct Cyc_Absyn_Exp*_tmp5E6;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5D5)->tag == 1){_LL39F: _tmp5E6=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5D5)->f1;_LL3A0: {
# 3465
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5EC) && _tmp5CD;
void*ta1=Cyc_Toc_typ_to_c(_tmp5EF);
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp5EE);
if(in_bnds  && !possibly_null)
++ Cyc_Toc_bounds_checks_eliminated;else{
if(in_bnds  && possibly_null){
++ Cyc_Toc_bounds_checks_eliminated;
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp5D6=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp5D7="inserted null check due to dereference";_tag_dyneither(_tmp5D7,sizeof(char),39);}),_tag_dyneither(_tmp5D6,sizeof(void*),0));});
_tmp68A->r=Cyc_Toc_cast_it_r(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_Absyn_Exp*_tmp5D8[1];_tmp5D8[0]=
Cyc_Absyn_copy_exp(_tmp68A);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5D8,sizeof(struct Cyc_Absyn_Exp*),1));}),0));}else{
# 3477
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5EA)){
# 3479
if(!Cyc_Evexp_c_can_eval(_tmp5E6))
({void*_tmp5D9=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp5DA="cannot determine subscript is in bounds";_tag_dyneither(_tmp5DA,sizeof(char),40);}),_tag_dyneither(_tmp5D9,sizeof(void*),0));});{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp68A);
e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(function_e,({struct Cyc_Absyn_Exp*_tmp5DB[3];_tmp5DB[2]=_tmp689;_tmp5DB[1]=_tmp5E6;_tmp5DB[0]=_tmp68A;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5DB,sizeof(struct Cyc_Absyn_Exp*),3));}),0)));};}else{
# 3485
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp5E6))
({void*_tmp5DC=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp5DD="cannot determine subscript is in bounds";_tag_dyneither(_tmp5DD,sizeof(char),40);}),_tag_dyneither(_tmp5DC,sizeof(void*),0));});
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp5DE=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp5DF="inserted null check due to dereference";_tag_dyneither(_tmp5DF,sizeof(char),39);}),_tag_dyneither(_tmp5DE,sizeof(void*),0));});
# 3491
e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*_tmp5E0[4];_tmp5E0[3]=_tmp689;_tmp5E0[2]=
# 3494
Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp5E0[1]=_tmp5E6;_tmp5E0[0]=_tmp68A;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5E0,sizeof(struct Cyc_Absyn_Exp*),4));}),0)));}else{
# 3497
if(!Cyc_Evexp_c_can_eval(_tmp5E6))
({void*_tmp5E1=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp5E2="cannot determine subscript is in bounds";_tag_dyneither(_tmp5E2,sizeof(char),40);}),_tag_dyneither(_tmp5E1,sizeof(void*),0));});
# 3500
_tmp689->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,({struct Cyc_Absyn_Exp*_tmp5E3[2];_tmp5E3[1]=
Cyc_Absyn_copy_exp(_tmp689);_tmp5E3[0]=_tmp5E6;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5E3,sizeof(struct Cyc_Absyn_Exp*),2));}));}}}}
# 3503
goto _LL39E;}}else{_LL3A1: _LL3A2: {
# 3505
void*ta1=Cyc_Toc_typ_to_c(_tmp5EF);
if(in_bnds){
# 3509
++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp5EE),
Cyc_Toc_member_exp(_tmp68A,Cyc_Toc_curr_sp,0)),_tmp689);}else{
# 3514
struct Cyc_Absyn_Exp*_tmp5E4=Cyc_Toc__check_dyneither_subscript_e;
e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp5EE),
Cyc_Absyn_fncall_exp(_tmp5E4,({struct Cyc_Absyn_Exp*_tmp5E5[3];_tmp5E5[2]=_tmp689;_tmp5E5[1]=
Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp5E5[0]=_tmp68A;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5E5,sizeof(struct Cyc_Absyn_Exp*),3));}),0)));}
# 3520
goto _LL39E;}}_LL39E:;}
# 3522
goto _LL391;}default: _LL396: _LL397:
({void*_tmp5E7=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp5E8="exp_to_c: Subscript on non-tuple/array/tuple ptr";_tag_dyneither(_tmp5E8,sizeof(char),49);}),_tag_dyneither(_tmp5E7,sizeof(void*),0));});}_LL391:;}
# 3525
Cyc_Toc_set_lhs(nv,_tmp5C3);
goto _LL283;};}case 23: _LL2B6: _tmp68B=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp495)->f1;_LL2B7:
# 3528
 if(!Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp68B))->r;else{
# 3533
struct Cyc_List_List*_tmp5F0=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp68B);
void*_tmp5F1=Cyc_Toc_add_tuple_type(_tmp5F0);
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp68B != 0;(_tmp68B=_tmp68B->tl,i ++)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp68B->hd);
dles=({struct Cyc_List_List*_tmp5F2=_cycalloc(sizeof(*_tmp5F2));_tmp5F2->hd=({struct _tuple19*_tmp5F3=_cycalloc(sizeof(*_tmp5F3));_tmp5F3->f1=0;_tmp5F3->f2=(struct Cyc_Absyn_Exp*)_tmp68B->hd;_tmp5F3;});_tmp5F2->tl=dles;_tmp5F2;});}}
# 3540
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}
# 3543
goto _LL283;case 25: _LL2B8: _tmp68C=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp495)->f1;_LL2B9:
# 3547
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp68C);
{struct Cyc_List_List*_tmp5F4=_tmp68C;for(0;_tmp5F4 != 0;_tmp5F4=_tmp5F4->tl){
struct _tuple19*_tmp5F5=(struct _tuple19*)_tmp5F4->hd;struct _tuple19*_tmp5F6=_tmp5F5;struct Cyc_Absyn_Exp*_tmp5F7;_LL3A4: _tmp5F7=_tmp5F6->f2;_LL3A5:;
Cyc_Toc_exp_to_c(nv,_tmp5F7);}}
# 3552
goto _LL283;case 26: _LL2BA: _tmp690=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp495)->f1;_tmp68F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp495)->f2;_tmp68E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp495)->f3;_tmp68D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp495)->f4;_LL2BB: {
# 3556
struct _tuple12 _tmp5F8=Cyc_Evexp_eval_const_uint_exp(_tmp68F);struct _tuple12 _tmp5F9=_tmp5F8;unsigned int _tmp603;int _tmp602;_LL3A7: _tmp603=_tmp5F9.f1;_tmp602=_tmp5F9.f2;_LL3A8:;{
void*_tmp5FA=Cyc_Toc_typ_to_c((void*)_check_null(_tmp68E->topt));
Cyc_Toc_exp_to_c(nv,_tmp68E);{
struct Cyc_List_List*es=0;
# 3561
if(!Cyc_Toc_is_zero(_tmp68E)){
if(!_tmp602)
({void*_tmp5FB=0;Cyc_Tcutil_terr(_tmp68F->loc,({const char*_tmp5FC="cannot determine value of constant";_tag_dyneither(_tmp5FC,sizeof(char),35);}),_tag_dyneither(_tmp5FB,sizeof(void*),0));});
{unsigned int i=0;for(0;i < _tmp603;++ i){
es=({struct Cyc_List_List*_tmp5FD=_cycalloc(sizeof(*_tmp5FD));_tmp5FD->hd=({struct _tuple19*_tmp5FE=_cycalloc(sizeof(*_tmp5FE));_tmp5FE->f1=0;_tmp5FE->f2=_tmp68E;_tmp5FE;});_tmp5FD->tl=es;_tmp5FD;});}}
# 3567
if(_tmp68D){
struct Cyc_Absyn_Exp*_tmp5FF=Cyc_Toc_cast_it(_tmp5FA,Cyc_Absyn_uint_exp(0,0));
es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,({struct Cyc_List_List*_tmp600=_cycalloc(sizeof(*_tmp600));_tmp600->hd=({struct _tuple19*_tmp601=_cycalloc(sizeof(*_tmp601));_tmp601->f1=0;_tmp601->f2=_tmp5FF;_tmp601;});_tmp600->tl=0;_tmp600;}));}}
# 3572
e->r=Cyc_Toc_unresolvedmem_exp_r(0,es);
goto _LL283;};};}case 27: _LL2BC: _tmp693=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp495)->f1;_tmp692=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp495)->f2;_tmp691=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp495)->f3;_LL2BD:
# 3577
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,0);
goto _LL283;case 28: _LL2BE: _tmp697=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp495)->f1;_tmp696=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp495)->f2;_tmp695=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp495)->f3;_tmp694=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp495)->f4;_LL2BF:
# 3582
 if(!Cyc_Toc_is_toplevel(nv)){
struct Cyc_Absyn_Exp*_tmp604=Cyc_Toc_init_struct(nv,old_typ,_tmp696,0,0,_tmp695,_tmp697);
e->r=_tmp604->r;
e->topt=_tmp604->topt;}else{
# 3593
if(_tmp694 == 0)
({void*_tmp605=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp606="Aggregate_e: missing aggrdecl pointer";_tag_dyneither(_tmp606,sizeof(char),38);}),_tag_dyneither(_tmp605,sizeof(void*),0));});{
struct Cyc_Absyn_Aggrdecl*sd2=_tmp694;
# 3597
struct _RegionHandle _tmp607=_new_region("rgn");struct _RegionHandle*rgn=& _tmp607;_push_region(rgn);
{struct Cyc_List_List*_tmp608=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp695,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
# 3601
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp694->impl))->tagged){
# 3603
struct _tuple30*_tmp609=(struct _tuple30*)((struct Cyc_List_List*)_check_null(_tmp608))->hd;struct _tuple30*_tmp60A=_tmp609;struct Cyc_Absyn_Aggrfield*_tmp617;struct Cyc_Absyn_Exp*_tmp616;_LL3AA: _tmp617=_tmp60A->f1;_tmp616=_tmp60A->f2;_LL3AB:;{
void*_tmp60B=(void*)_check_null(_tmp616->topt);
void*_tmp60C=_tmp617->type;
Cyc_Toc_exp_to_c(nv,_tmp616);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp60C) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp60B))
_tmp616->r=
Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp616->r,0));{
# 3612
int i=Cyc_Toc_get_member_offset(_tmp694,_tmp617->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct Cyc_List_List*_tmp60D=({struct _tuple19*_tmp613[2];_tmp613[1]=({struct _tuple19*_tmp615=_cycalloc(sizeof(*_tmp615));_tmp615->f1=0;_tmp615->f2=_tmp616;_tmp615;});_tmp613[0]=({struct _tuple19*_tmp614=_cycalloc(sizeof(*_tmp614));_tmp614->f1=0;_tmp614->f2=field_tag_exp;_tmp614;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp613,sizeof(struct _tuple19*),2));});
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp60D,0);
struct Cyc_List_List*ds=({void*_tmp610[1];_tmp610[0]=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp611=_cycalloc(sizeof(*_tmp611));_tmp611[0]=({struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp612;_tmp612.tag=1;_tmp612.f1=_tmp617->name;_tmp612;});_tmp611;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp610,sizeof(void*),1));});
struct Cyc_List_List*dles=({struct _tuple19*_tmp60E[1];_tmp60E[0]=({struct _tuple19*_tmp60F=_cycalloc(sizeof(*_tmp60F));_tmp60F->f1=ds;_tmp60F->f2=umem;_tmp60F;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp60E,sizeof(struct _tuple19*),1));});
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{
# 3621
struct Cyc_List_List*_tmp618=0;
struct Cyc_List_List*_tmp619=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields;
for(0;_tmp619 != 0;_tmp619=_tmp619->tl){
struct Cyc_List_List*_tmp61A=_tmp608;for(0;_tmp61A != 0;_tmp61A=_tmp61A->tl){
if((*((struct _tuple30*)_tmp61A->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp619->hd){
struct _tuple30*_tmp61B=(struct _tuple30*)_tmp61A->hd;struct _tuple30*_tmp61C=_tmp61B;struct Cyc_Absyn_Aggrfield*_tmp622;struct Cyc_Absyn_Exp*_tmp621;_LL3AD: _tmp622=_tmp61C->f1;_tmp621=_tmp61C->f2;_LL3AE:;{
void*_tmp61D=Cyc_Toc_typ_to_c(_tmp622->type);
void*_tmp61E=Cyc_Toc_typ_to_c((void*)_check_null(_tmp621->topt));
Cyc_Toc_exp_to_c(nv,_tmp621);
# 3631
if(!Cyc_Tcutil_unify(_tmp61D,_tmp61E)){
# 3633
if(!Cyc_Tcutil_is_arithmetic_type(_tmp61D) || !
Cyc_Tcutil_is_arithmetic_type(_tmp61E))
_tmp621=Cyc_Toc_cast_it(_tmp61D,Cyc_Absyn_copy_exp(_tmp621));}
_tmp618=({struct Cyc_List_List*_tmp61F=_cycalloc(sizeof(*_tmp61F));_tmp61F->hd=({struct _tuple19*_tmp620=_cycalloc(sizeof(*_tmp620));_tmp620->f1=0;_tmp620->f2=_tmp621;_tmp620;});_tmp61F->tl=_tmp618;_tmp61F;});
break;};}}}
# 3640
e->r=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp618));}}
# 3598
;_pop_region(rgn);};}
# 3644
goto _LL283;case 29: _LL2C0: _tmp699=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp495)->f1;_tmp698=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp495)->f2;_LL2C1: {
# 3646
struct Cyc_List_List*fs;
{void*_tmp623=Cyc_Tcutil_compress(_tmp699);void*_tmp624=_tmp623;struct Cyc_List_List*_tmp628;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp624)->tag == 12){_LL3B0: _tmp628=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp624)->f2;_LL3B1:
 fs=_tmp628;goto _LL3AF;}else{_LL3B2: _LL3B3:
({struct Cyc_String_pa_PrintArg_struct _tmp627;_tmp627.tag=0;_tmp627.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp699));({void*_tmp625[1]={& _tmp627};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp626="anon struct has type %s";_tag_dyneither(_tmp626,sizeof(char),24);}),_tag_dyneither(_tmp625,sizeof(void*),1));});});}_LL3AF:;}{
# 3651
struct _RegionHandle _tmp629=_new_region("rgn");struct _RegionHandle*rgn=& _tmp629;_push_region(rgn);{
struct Cyc_List_List*_tmp62A=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp698,Cyc_Absyn_StructA,fs);
for(0;_tmp62A != 0;_tmp62A=_tmp62A->tl){
struct _tuple30*_tmp62B=(struct _tuple30*)_tmp62A->hd;struct _tuple30*_tmp62C=_tmp62B;struct Cyc_Absyn_Aggrfield*_tmp630;struct Cyc_Absyn_Exp*_tmp62F;_LL3B5: _tmp630=_tmp62C->f1;_tmp62F=_tmp62C->f2;_LL3B6:;{
void*_tmp62D=(void*)_check_null(_tmp62F->topt);
void*_tmp62E=_tmp630->type;
Cyc_Toc_exp_to_c(nv,_tmp62F);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp62E) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp62D))
_tmp62F->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp62F->r,0));};}
# 3665
e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp698);}
# 3667
_npop_handler(0);goto _LL283;
# 3651
;_pop_region(rgn);};}case 30: _LL2C2: _tmp69C=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp495)->f1;_tmp69B=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp495)->f2;_tmp69A=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp495)->f3;_LL2C3: {
# 3670
void*datatype_ctype;
struct Cyc_Absyn_Exp*tag_exp;
struct _tuple1*_tmp631=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp632=Cyc_Absyn_var_exp(_tmp631,0);
struct Cyc_Absyn_Exp*mem_exp;
datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp69A->name,_tmp69B->name));
tag_exp=_tmp69B->is_extensible?Cyc_Absyn_var_exp(_tmp69A->name,0):
 Cyc_Toc_datatype_tag(_tmp69B,_tmp69A->name);
mem_exp=_tmp632;{
struct Cyc_List_List*_tmp633=_tmp69A->typs;
# 3681
if(Cyc_Toc_is_toplevel(nv)){
# 3683
struct Cyc_List_List*dles=0;
for(0;_tmp69C != 0;(_tmp69C=_tmp69C->tl,_tmp633=_tmp633->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp69C->hd;
void*_tmp634=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp633))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp634))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);
dles=({struct Cyc_List_List*_tmp635=_cycalloc(sizeof(*_tmp635));_tmp635->hd=({struct _tuple19*_tmp636=_cycalloc(sizeof(*_tmp636));_tmp636->f1=0;_tmp636->f2=cur_e;_tmp636;});_tmp635->tl=dles;_tmp635;});}
# 3694
dles=({struct Cyc_List_List*_tmp637=_cycalloc(sizeof(*_tmp637));_tmp637->hd=({struct _tuple19*_tmp638=_cycalloc(sizeof(*_tmp638));_tmp638->f1=0;_tmp638->f2=tag_exp;_tmp638;});_tmp637->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp637;});
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{
# 3700
struct Cyc_List_List*_tmp639=({struct Cyc_List_List*_tmp640=_cycalloc(sizeof(*_tmp640));_tmp640->hd=
Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Toc_tag_sp,0),tag_exp,0);_tmp640->tl=0;_tmp640;});
# 3703
{int i=1;for(0;_tmp69C != 0;(((_tmp69C=_tmp69C->tl,i ++)),_tmp633=_tmp633->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp69C->hd;
void*_tmp63A=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp633))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp63A))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct Cyc_Absyn_Stmt*_tmp63B=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Absyn_fieldname(i),0),cur_e,0);
# 3713
_tmp639=({struct Cyc_List_List*_tmp63C=_cycalloc(sizeof(*_tmp63C));_tmp63C->hd=_tmp63B;_tmp63C->tl=_tmp639;_tmp63C;});};}}{
# 3715
struct Cyc_Absyn_Stmt*_tmp63D=Cyc_Absyn_exp_stmt(_tmp632,0);
struct Cyc_Absyn_Stmt*_tmp63E=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(({struct Cyc_List_List*_tmp63F=_cycalloc(sizeof(*_tmp63F));_tmp63F->hd=_tmp63D;_tmp63F->tl=_tmp639;_tmp63F;})),0);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp631,datatype_ctype,0,_tmp63E,0));};}
# 3719
goto _LL283;};}case 31: _LL2C4: _LL2C5:
# 3721
 goto _LL2C7;case 32: _LL2C6: _LL2C7:
 goto _LL283;case 33: _LL2C8: _tmp6A2=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp495)->f1).is_calloc;_tmp6A1=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp495)->f1).rgn;_tmp6A0=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp495)->f1).elt_type;_tmp69F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp495)->f1).num_elts;_tmp69E=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp495)->f1).fat_result;_tmp69D=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp495)->f1).inline_call;_LL2C9: {
# 3725
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp6A0)));
Cyc_Toc_exp_to_c(nv,_tmp69F);
# 3729
if(_tmp69E){
struct _tuple1*_tmp641=Cyc_Toc_temp_var();
struct _tuple1*_tmp642=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp6A2){
xexp=_tmp69F;
if(_tmp6A1 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp6A1;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp641,0));}else{
# 3740
pexp=Cyc_Toc_calloc_exp(*_tmp6A0,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp641,0));}
# 3742
rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*_tmp643[3];_tmp643[2]=
# 3744
Cyc_Absyn_var_exp(_tmp641,0);_tmp643[1]=
# 3743
Cyc_Absyn_sizeoftyp_exp(t_c,0);_tmp643[0]=Cyc_Absyn_var_exp(_tmp642,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp643,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}else{
# 3746
if(_tmp6A1 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp6A1;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp69D)
pexp=Cyc_Toc_rmalloc_inline_exp(rgn,Cyc_Absyn_var_exp(_tmp641,0));else{
# 3752
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp641,0));}}else{
# 3754
pexp=Cyc_Toc_malloc_exp(*_tmp6A0,Cyc_Absyn_var_exp(_tmp641,0));}
# 3756
rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*_tmp644[3];_tmp644[2]=
Cyc_Absyn_var_exp(_tmp641,0);_tmp644[1]=
# 3756
Cyc_Absyn_uint_exp(1,0);_tmp644[0]=Cyc_Absyn_var_exp(_tmp642,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp644,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}{
# 3759
struct Cyc_Absyn_Stmt*_tmp645=Cyc_Absyn_declare_stmt(_tmp641,Cyc_Absyn_uint_typ,_tmp69F,
Cyc_Absyn_declare_stmt(_tmp642,Cyc_Absyn_cstar_typ(t_c,Cyc_Toc_mt_tq),pexp,
Cyc_Absyn_exp_stmt(rexp,0),0),0);
e->r=Cyc_Toc_stmt_exp_r(_tmp645);};}else{
# 3764
if(_tmp6A2){
if(_tmp6A1 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp6A1;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp69F))->r;}else{
# 3770
e->r=(Cyc_Toc_calloc_exp(*_tmp6A0,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp69F))->r;}}else{
# 3773
if(_tmp6A1 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp6A1;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp69D)
e->r=(Cyc_Toc_rmalloc_inline_exp(rgn,_tmp69F))->r;else{
# 3779
e->r=(Cyc_Toc_rmalloc_exp(rgn,_tmp69F))->r;}}else{
# 3781
e->r=(Cyc_Toc_malloc_exp(*_tmp6A0,_tmp69F))->r;}}}
# 3785
goto _LL283;}case 34: _LL2CA: _tmp6A4=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp495)->f1;_tmp6A3=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp495)->f2;_LL2CB: {
# 3794
void*e1_old_typ=(void*)_check_null(_tmp6A4->topt);
void*e2_old_typ=(void*)_check_null(_tmp6A3->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ) && !Cyc_Tcutil_is_pointer_type(e1_old_typ))
({void*_tmp646=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp647="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";_tag_dyneither(_tmp647,sizeof(char),57);}),_tag_dyneither(_tmp646,sizeof(void*),0));});{
# 3800
struct Cyc_Absyn_Exp*swap_fn;
if(Cyc_Tcutil_is_dyneither_ptr(e1_old_typ))
swap_fn=Cyc_Toc__swap_dyneither_e;else{
# 3804
swap_fn=Cyc_Toc__swap_word_e;}{
# 3808
int f1_tag=0;
void*tagged_mem_type1=(void*)& Cyc_Absyn_VoidType_val;
int e1_tmem=Cyc_Toc_is_tagged_union_project(_tmp6A4,& f1_tag,& tagged_mem_type1,1);
int f2_tag=0;
void*tagged_mem_type2=(void*)& Cyc_Absyn_VoidType_val;
int e2_tmem=Cyc_Toc_is_tagged_union_project(_tmp6A3,& f2_tag,& tagged_mem_type2,1);
# 3815
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp6A4);
Cyc_Toc_exp_to_c(nv,_tmp6A3);
Cyc_Toc_set_lhs(nv,0);
# 3821
if(e1_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp6A4,f1_tag,tagged_mem_type1);else{
# 3825
_tmp6A4=Cyc_Toc_push_address_exp(_tmp6A4);}
# 3827
if(e2_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp6A3,f2_tag,tagged_mem_type2);else{
# 3831
_tmp6A3=Cyc_Toc_push_address_exp(_tmp6A3);}
# 3833
e->r=Cyc_Toc_fncall_exp_r(swap_fn,({struct Cyc_Absyn_Exp*_tmp648[2];_tmp648[1]=_tmp6A3;_tmp648[0]=_tmp6A4;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp648,sizeof(struct Cyc_Absyn_Exp*),2));}));
# 3835
goto _LL283;};};}case 37: _LL2CC: _tmp6A6=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp495)->f1;_tmp6A5=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp495)->f2;_LL2CD: {
# 3838
void*_tmp649=Cyc_Tcutil_compress((void*)_check_null(_tmp6A6->topt));
Cyc_Toc_exp_to_c(nv,_tmp6A6);
{void*_tmp64A=_tmp649;struct Cyc_Absyn_Aggrdecl*_tmp650;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp64A)->tag == 11){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp64A)->f1).aggr_info).KnownAggr).tag == 2){_LL3B8: _tmp650=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp64A)->f1).aggr_info).KnownAggr).val;_LL3B9: {
# 3842
struct Cyc_Absyn_Exp*_tmp64B=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp650,_tmp6A5),0);
struct Cyc_Absyn_Exp*_tmp64C=Cyc_Toc_member_exp(_tmp6A6,_tmp6A5,0);
struct Cyc_Absyn_Exp*_tmp64D=Cyc_Toc_member_exp(_tmp64C,Cyc_Toc_tag_sp,0);
e->r=(Cyc_Absyn_eq_exp(_tmp64D,_tmp64B,0))->r;
goto _LL3B7;}}else{goto _LL3BA;}}else{_LL3BA: _LL3BB:
({void*_tmp64E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp64F="non-aggregate type in tagcheck";_tag_dyneither(_tmp64F,sizeof(char),31);}),_tag_dyneither(_tmp64E,sizeof(void*),0));});}_LL3B7:;}
# 3849
goto _LL283;}case 36: _LL2CE: _tmp6A7=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp495)->f1;_LL2CF:
 Cyc_Toc_stmt_to_c(nv,_tmp6A7);goto _LL283;case 35: _LL2D0: _LL2D1:
({void*_tmp651=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp652="UnresolvedMem";_tag_dyneither(_tmp652,sizeof(char),14);}),_tag_dyneither(_tmp651,sizeof(void*),0));});case 24: _LL2D2: _LL2D3:
({void*_tmp653=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp654="compoundlit";_tag_dyneither(_tmp654,sizeof(char),12);}),_tag_dyneither(_tmp653,sizeof(void*),0));});case 38: _LL2D4: _LL2D5:
({void*_tmp655=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp656="valueof(-)";_tag_dyneither(_tmp656,sizeof(char),11);}),_tag_dyneither(_tmp655,sizeof(void*),0));});default: _LL2D6: _LL2D7:
({void*_tmp657=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp658="__asm__";_tag_dyneither(_tmp658,sizeof(char),8);}),_tag_dyneither(_tmp657,sizeof(void*),0));});}_LL283:;};}struct _tuple31{int f1;struct _dyneither_ptr*f2;struct _dyneither_ptr*f3;struct Cyc_Absyn_Switch_clause*f4;};
# 3882 "toc.cyc"
static struct _tuple31*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 3884
return({struct _tuple31*_tmp6A8=_region_malloc(r,sizeof(*_tmp6A8));_tmp6A8->f1=0;_tmp6A8->f2=Cyc_Toc_fresh_label();_tmp6A8->f3=Cyc_Toc_fresh_label();_tmp6A8->f4=sc;_tmp6A8;});}
# 3889
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(0;ps != 0;ps=((struct Cyc_List_List*)_check_null(ps))->tl){
struct Cyc_Tcpat_PathNode*_tmp6A9=(struct Cyc_Tcpat_PathNode*)ps->hd;
# 3895
if((int)(((_tmp6A9->orig_pat).pattern).tag == 1)){
void*t=(void*)_check_null(({union Cyc_Tcpat_PatOrWhere _tmp6AC=_tmp6A9->orig_pat;if((_tmp6AC.pattern).tag != 1)_throw_match();(_tmp6AC.pattern).val;})->topt);
void*_tmp6AA=Cyc_Tcutil_compress(Cyc_Toc_typ_to_c_array(t));
void*_tmp6AB=_tmp6AA;switch(*((int*)_tmp6AB)){case 0: _LL3BD: _LL3BE:
# 3900
 goto _LL3C0;case 11: _LL3BF: _LL3C0:
 goto _LL3C2;case 12: _LL3C1: _LL3C2:
 goto _LL3BC;default: _LL3C3: _LL3C4:
 v=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c_array(t),v);goto _LL3BC;}_LL3BC:;}{
# 3906
void*_tmp6AD=_tmp6A9->access;void*_tmp6AE=_tmp6AD;struct Cyc_Absyn_Datatypedecl*_tmp6B9;struct Cyc_Absyn_Datatypefield*_tmp6B8;unsigned int _tmp6B7;int _tmp6B6;struct _dyneither_ptr*_tmp6B5;unsigned int _tmp6B4;switch(*((int*)_tmp6AE)){case 0: _LL3C6: _LL3C7:
# 3911
 goto _LL3C5;case 1: _LL3C8: _LL3C9:
# 3916
 if(ps->tl != 0){
void*_tmp6AF=((struct Cyc_Tcpat_PathNode*)((struct Cyc_List_List*)_check_null(ps->tl))->hd)->access;void*_tmp6B0=_tmp6AF;struct Cyc_Absyn_Datatypedecl*_tmp6B3;struct Cyc_Absyn_Datatypefield*_tmp6B2;unsigned int _tmp6B1;if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6B0)->tag == 3){_LL3D1: _tmp6B3=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6B0)->f1;_tmp6B2=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6B0)->f2;_tmp6B1=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6B0)->f3;_LL3D2:
# 3919
 ps=ps->tl;
v=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp6B2->name,_tmp6B3->name)),Cyc_Toc_mt_tq),v);
v=Cyc_Absyn_aggrarrow_exp(v,Cyc_Absyn_fieldname((int)(_tmp6B1 + 1)),0);
continue;}else{_LL3D3: _LL3D4:
# 3924
 goto _LL3D0;}_LL3D0:;}
# 3927
v=Cyc_Absyn_deref_exp(v,0);
goto _LL3C5;case 2: _LL3CA: _tmp6B4=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp6AE)->f1;_LL3CB:
 v=Cyc_Toc_member_exp(v,Cyc_Absyn_fieldname((int)(_tmp6B4 + 1)),0);goto _LL3C5;case 4: _LL3CC: _tmp6B6=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp6AE)->f1;_tmp6B5=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp6AE)->f2;_LL3CD:
# 3931
 v=Cyc_Toc_member_exp(v,_tmp6B5,0);
if(_tmp6B6)
v=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0);
goto _LL3C5;default: _LL3CE: _tmp6B9=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6AE)->f1;_tmp6B8=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6AE)->f2;_tmp6B7=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6AE)->f3;_LL3CF:
# 3936
 v=Cyc_Toc_member_exp(v,Cyc_Absyn_fieldname((int)(_tmp6B7 + 1)),0);
goto _LL3C5;}_LL3C5:;};}
# 3940
return v;}
# 3945
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
void*_tmp6BA=t;struct Cyc_Absyn_Datatypedecl*_tmp6D5;struct Cyc_Absyn_Datatypefield*_tmp6D4;struct _dyneither_ptr*_tmp6D3;int _tmp6D2;int _tmp6D1;struct Cyc_Absyn_Datatypedecl*_tmp6D0;struct Cyc_Absyn_Datatypefield*_tmp6CF;unsigned int _tmp6CE;struct _dyneither_ptr _tmp6CD;int _tmp6CC;void*_tmp6CB;struct Cyc_Absyn_Enumfield*_tmp6CA;struct Cyc_Absyn_Enumdecl*_tmp6C9;struct Cyc_Absyn_Enumfield*_tmp6C8;struct Cyc_Absyn_Exp*_tmp6C7;switch(*((int*)_tmp6BA)){case 0: _LL3D6: _tmp6C7=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp6BA)->f1;_LL3D7:
# 3949
 if(_tmp6C7 == 0)return v;else{return _tmp6C7;}case 1: _LL3D8: _LL3D9:
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_signed_int_exp(0,0),0);case 2: _LL3DA: _LL3DB:
 return Cyc_Absyn_neq_exp(v,Cyc_Absyn_signed_int_exp(0,0),0);case 3: _LL3DC: _tmp6C9=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp6BA)->f1;_tmp6C8=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp6BA)->f2;_LL3DD:
# 3953
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp6BB=_cycalloc(sizeof(*_tmp6BB));_tmp6BB[0]=({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmp6BC;_tmp6BC.tag=31;_tmp6BC.f1=_tmp6C9;_tmp6BC.f2=_tmp6C8;_tmp6BC;});_tmp6BB;}),0),0);case 4: _LL3DE: _tmp6CB=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp6BA)->f1;_tmp6CA=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp6BA)->f2;_LL3DF:
# 3955
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp6BD=_cycalloc(sizeof(*_tmp6BD));_tmp6BD[0]=({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp6BE;_tmp6BE.tag=32;_tmp6BE.f1=_tmp6CB;_tmp6BE.f2=_tmp6CA;_tmp6BE;});_tmp6BD;}),0),0);case 5: _LL3E0: _tmp6CD=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp6BA)->f1;_tmp6CC=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp6BA)->f2;_LL3E1:
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_float_exp(_tmp6CD,_tmp6CC,0),0);case 6: _LL3E2: _tmp6CE=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp6BA)->f1;_LL3E3:
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_uint_exp(_tmp6CE,0),0);case 7: _LL3E4: _tmp6D1=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp6BA)->f1;_tmp6D0=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp6BA)->f2;_tmp6CF=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp6BA)->f3;_LL3E5:
# 3961
 LOOP1: {
void*_tmp6BF=v->r;void*_tmp6C0=_tmp6BF;struct Cyc_Absyn_Exp*_tmp6C2;struct Cyc_Absyn_Exp*_tmp6C1;switch(*((int*)_tmp6C0)){case 13: _LL3EB: _tmp6C1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6C0)->f2;_LL3EC:
 v=_tmp6C1;goto LOOP1;case 19: _LL3ED: _tmp6C2=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6C0)->f1;_LL3EE:
 v=_tmp6C2;goto _LL3EA;default: _LL3EF: _LL3F0:
 goto _LL3EA;}_LL3EA:;}
# 3968
v=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp6CF->name,_tmp6D0->name)),Cyc_Toc_mt_tq),v);
return Cyc_Absyn_eq_exp(Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0),Cyc_Absyn_uint_exp((unsigned int)_tmp6D1,0),0);case 8: _LL3E6: _tmp6D3=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp6BA)->f1;_tmp6D2=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp6BA)->f2;_LL3E7:
# 3971
 v=Cyc_Toc_member_exp(Cyc_Toc_member_exp(v,_tmp6D3,0),Cyc_Toc_tag_sp,0);
return Cyc_Absyn_eq_exp(v,Cyc_Absyn_uint_exp((unsigned int)_tmp6D2,0),0);default: _LL3E8: _tmp6D5=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp6BA)->f1;_tmp6D4=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp6BA)->f2;_LL3E9:
# 3975
 LOOP2: {
void*_tmp6C3=v->r;void*_tmp6C4=_tmp6C3;struct Cyc_Absyn_Exp*_tmp6C6;struct Cyc_Absyn_Exp*_tmp6C5;switch(*((int*)_tmp6C4)){case 13: _LL3F2: _tmp6C5=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6C4)->f2;_LL3F3:
 v=_tmp6C5;goto LOOP2;case 19: _LL3F4: _tmp6C6=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6C4)->f1;_LL3F5:
 v=_tmp6C6;goto _LL3F1;default: _LL3F6: _LL3F7:
 goto _LL3F1;}_LL3F1:;}
# 3982
v=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp6D4->name,_tmp6D5->name)),Cyc_Toc_mt_tq),v);
return Cyc_Absyn_eq_exp(Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0),Cyc_Absyn_var_exp(_tmp6D4->name,0),0);}_LL3D5:;}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _dyneither_ptr*f1;};
# 3994
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1};struct _tuple32{int f1;void*f2;};
# 4002
static struct _tuple32 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 4004
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(0;ss != 0;(ss=ss->tl,c=c + 1)){
struct _tuple29 _tmp6D8=*((struct _tuple29*)ss->hd);struct _tuple29 _tmp6D9=_tmp6D8;void*_tmp6E3;_LL3F9: _tmp6E3=_tmp6D9.f1;_LL3FA:;{
void*_tmp6DA=_tmp6E3;struct Cyc_Absyn_Exp*_tmp6E2;struct _dyneither_ptr*_tmp6E1;switch(*((int*)_tmp6DA)){case 3: _LL3FC: _LL3FD:
# 4010
 goto _LL3FF;case 4: _LL3FE: _LL3FF:
 goto _LL401;case 6: _LL400: _LL401:
# 4013
 continue;case 8: _LL402: _tmp6E1=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp6DA)->f1;_LL403:
# 4015
 if(k == (void*)& Cyc_Toc_OtherTest_val)
k=(void*)({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_tmp6DB=_cycalloc(sizeof(*_tmp6DB));_tmp6DB[0]=({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct _tmp6DC;_tmp6DC.tag=3;_tmp6DC.f1=_tmp6E1;_tmp6DC;});_tmp6DB;});
continue;case 7: _LL404: _LL405:
# 4019
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;
continue;case 0: _LL406: _tmp6E2=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp6DA)->f1;if(_tmp6E2 != 0){_LL407:
# 4023
 k=(void*)({struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_tmp6DD=_cycalloc(sizeof(*_tmp6DD));_tmp6DD[0]=({struct Cyc_Toc_WhereTest_Toc_TestKind_struct _tmp6DE;_tmp6DE.tag=2;_tmp6DE.f1=_tmp6E2;_tmp6DE;});_tmp6DD;});
return({struct _tuple32 _tmp6DF;_tmp6DF.f1=0;_tmp6DF.f2=k;_tmp6DF;});}else{_LL408: _LL409:
 goto _LL40B;}case 1: _LL40A: _LL40B:
 goto _LL40D;case 2: _LL40C: _LL40D:
 goto _LL40F;case 5: _LL40E: _LL40F:
 goto _LL411;default: _LL410: _LL411:
 return({struct _tuple32 _tmp6E0;_tmp6E0.f1=0;_tmp6E0.f2=k;_tmp6E0;});}_LL3FB:;};}
# 4032
return({struct _tuple32 _tmp6E4;_tmp6E4.f1=c;_tmp6E4.f2=k;_tmp6E4;});}
# 4037
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{void*_tmp6E5=p;int _tmp6F2;int _tmp6F1;unsigned int _tmp6F0;void*_tmp6EF;struct Cyc_Absyn_Enumfield*_tmp6EE;struct Cyc_Absyn_Enumdecl*_tmp6ED;struct Cyc_Absyn_Enumfield*_tmp6EC;switch(*((int*)_tmp6E5)){case 3: _LL413: _tmp6ED=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp6E5)->f1;_tmp6EC=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp6E5)->f2;_LL414:
# 4041
 e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp6E6=_cycalloc(sizeof(*_tmp6E6));_tmp6E6[0]=({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmp6E7;_tmp6E7.tag=31;_tmp6E7.f1=_tmp6ED;_tmp6E7.f2=_tmp6EC;_tmp6E7;});_tmp6E6;}),0);goto _LL412;case 4: _LL415: _tmp6EF=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp6E5)->f1;_tmp6EE=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp6E5)->f2;_LL416:
# 4043
 e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp6E8=_cycalloc(sizeof(*_tmp6E8));_tmp6E8[0]=({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp6E9;_tmp6E9.tag=32;_tmp6E9.f1=_tmp6EF;_tmp6E9.f2=_tmp6EE;_tmp6E9;});_tmp6E8;}),0);goto _LL412;case 6: _LL417: _tmp6F0=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp6E5)->f1;_LL418:
 _tmp6F1=(int)_tmp6F0;goto _LL41A;case 7: _LL419: _tmp6F1=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp6E5)->f1;_LL41A:
 _tmp6F2=_tmp6F1;goto _LL41C;case 8: _LL41B: _tmp6F2=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp6E5)->f2;_LL41C:
# 4047
 e=Cyc_Absyn_uint_exp((unsigned int)_tmp6F2,0);
goto _LL412;default: _LL41D: _LL41E:
({void*_tmp6EA=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp6EB="compile_pat_test_as_case!";_tag_dyneither(_tmp6EB,sizeof(char),26);}),_tag_dyneither(_tmp6EA,sizeof(void*),0));});}_LL412:;}
# 4051
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp6F3=_cycalloc(sizeof(*_tmp6F3));_tmp6F3[0]=({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct _tmp6F4;_tmp6F4.tag=17;_tmp6F4.f1=e;_tmp6F4;});_tmp6F3;}),0);}
# 4055
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0);}struct _tuple33{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 4064
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**decls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 4067
void*t=(void*)_check_null(p->topt);
void*_tmp6F5=p->r;void*_tmp6F6=_tmp6F5;union Cyc_Absyn_AggrInfoU _tmp72E;struct Cyc_List_List*_tmp72D;struct Cyc_List_List*_tmp72C;struct Cyc_List_List*_tmp72B;struct Cyc_Absyn_Pat*_tmp72A;struct Cyc_Absyn_Datatypedecl*_tmp729;struct Cyc_Absyn_Datatypefield*_tmp728;struct Cyc_List_List*_tmp727;struct Cyc_Absyn_Vardecl*_tmp726;struct Cyc_Absyn_Pat*_tmp725;struct Cyc_Absyn_Vardecl*_tmp724;struct Cyc_Absyn_Vardecl*_tmp723;struct Cyc_Absyn_Pat*_tmp722;struct Cyc_Absyn_Vardecl*_tmp721;switch(*((int*)_tmp6F6)){case 2: _LL420: _tmp721=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp6F6)->f2;_LL421: {
# 4070
struct Cyc_Absyn_Pat*_tmp6F7=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0);
_tmp6F7->topt=p->topt;
_tmp723=_tmp721;_tmp722=_tmp6F7;goto _LL423;}case 1: _LL422: _tmp723=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp6F6)->f1;_tmp722=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp6F6)->f2;_LL423:
# 4075
*decls=({struct Cyc_List_List*_tmp6F8=_region_malloc(rgn,sizeof(*_tmp6F8));_tmp6F8->hd=_tmp723;_tmp6F8->tl=*decls;_tmp6F8;});{
struct Cyc_Absyn_Stmt*_tmp6F9=Cyc_Absyn_assign_stmt(Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp6FA=_cycalloc(sizeof(*_tmp6FA));_tmp6FA[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp6FB;_tmp6FB.tag=4;_tmp6FB.f1=_tmp723;_tmp6FB;});_tmp6FA;}),0),Cyc_Absyn_copy_exp(path),0);
return Cyc_Toc_seq_stmt_opt(_tmp6F9,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp722));};case 4: _LL424: _tmp724=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp6F6)->f2;_LL425:
# 4080
*decls=({struct Cyc_List_List*_tmp6FC=_region_malloc(rgn,sizeof(*_tmp6FC));_tmp6FC->hd=_tmp724;_tmp6FC->tl=*decls;_tmp6FC;});
return Cyc_Absyn_assign_stmt(Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp6FD=_cycalloc(sizeof(*_tmp6FD));_tmp6FD[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp6FE;_tmp6FE.tag=4;_tmp6FE.f1=_tmp724;_tmp6FE;});_tmp6FD;}),0),Cyc_Absyn_copy_exp(path),0);case 0: _LL426: _LL427:
 return 0;case 3: _LL428: _tmp726=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp6F6)->f1;_tmp725=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp6F6)->f2;_LL429:
# 4085
*decls=({struct Cyc_List_List*_tmp6FF=_region_malloc(rgn,sizeof(*_tmp6FF));_tmp6FF->hd=_tmp726;_tmp6FF->tl=*decls;_tmp6FF;});
_tmp726->type=Cyc_Absyn_cstar_typ(t,Cyc_Toc_mt_tq);{
# 4088
struct Cyc_Absyn_Stmt*_tmp700=Cyc_Absyn_assign_stmt(Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp701=_cycalloc(sizeof(*_tmp701));_tmp701[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp702;_tmp702.tag=4;_tmp702.f1=_tmp726;_tmp702;});_tmp701;}),0),
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path))),0);
return Cyc_Toc_seq_stmt_opt(_tmp700,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp725));};case 9: _LL42A: _LL42B:
# 4093
 goto _LL42D;case 10: _LL42C: _LL42D:
 goto _LL42F;case 11: _LL42E: _LL42F:
 goto _LL431;case 12: _LL430: _LL431:
 goto _LL433;case 13: _LL432: _LL433:
 goto _LL435;case 14: _LL434: _LL435:
 return 0;case 6: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6F6)->f1)->r)->tag == 8){_LL436: _tmp729=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6F6)->f1)->r)->f1;_tmp728=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6F6)->f1)->r)->f2;_tmp727=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6F6)->f1)->r)->f3;_LL437:
# 4102
 if(_tmp727 == 0)return 0;{
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(_tmp728->name,_tmp729->name);
void*_tmp703=Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);
path=Cyc_Toc_cast_it(_tmp703,path);{
int cnt=1;
struct Cyc_List_List*_tmp704=_tmp728->typs;
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp727 != 0;(((_tmp727=_tmp727->tl,_tmp704=((struct Cyc_List_List*)_check_null(_tmp704))->tl)),++ cnt)){
struct Cyc_Absyn_Pat*_tmp705=(struct Cyc_Absyn_Pat*)_tmp727->hd;
if(_tmp705->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
void*_tmp706=(*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp704))->hd)).f2;
void*_tmp707=(void*)_check_null(_tmp705->topt);
void*_tmp708=Cyc_Toc_typ_to_c_array(_tmp707);
struct Cyc_Absyn_Exp*_tmp709=Cyc_Absyn_aggrarrow_exp(path,Cyc_Absyn_fieldname(cnt),0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp706)))
_tmp709=Cyc_Toc_cast_it(_tmp708,_tmp709);
s=Cyc_Toc_seq_stmt_opt(s,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp709,_tmp705));};}
# 4120
return s;};};}else{_LL440: _tmp72A=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp6F6)->f1;_LL441:
# 4169
 return Cyc_Toc_extract_pattern_vars(rgn,nv,decls,Cyc_Absyn_deref_exp(path,0),_tmp72A);}case 8: _LL438: _tmp72B=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp6F6)->f3;_LL439:
# 4122
 _tmp72C=_tmp72B;goto _LL43B;case 5: _LL43A: _tmp72C=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp6F6)->f1;_LL43B: {
# 4124
struct Cyc_Absyn_Stmt*s=0;
int cnt=1;
for(0;_tmp72C != 0;(_tmp72C=_tmp72C->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp70A=(struct Cyc_Absyn_Pat*)_tmp72C->hd;
if(_tmp70A->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp70B=(void*)_check_null(_tmp70A->topt);
struct _dyneither_ptr*_tmp70C=Cyc_Absyn_fieldname(cnt);
s=Cyc_Toc_seq_stmt_opt(s,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,Cyc_Toc_member_exp(path,_tmp70C,0),_tmp70A));};}
# 4134
return s;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp6F6)->f1 == 0){_LL43C: _LL43D:
({void*_tmp70D=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp70E="unresolved aggregate pattern!";_tag_dyneither(_tmp70E,sizeof(char),30);}),_tag_dyneither(_tmp70D,sizeof(void*),0));});}else{_LL43E: _tmp72E=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp6F6)->f1)->aggr_info;_tmp72D=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp6F6)->f3;_LL43F: {
# 4138
struct Cyc_Absyn_Aggrdecl*_tmp70F=Cyc_Absyn_get_known_aggrdecl(_tmp72E);
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp72D != 0;_tmp72D=_tmp72D->tl){
struct _tuple33*_tmp710=(struct _tuple33*)_tmp72D->hd;
struct Cyc_Absyn_Pat*_tmp711=(*_tmp710).f2;
if(_tmp711->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*f;
{void*_tmp712=(void*)((struct Cyc_List_List*)_check_null((*_tmp710).f1))->hd;void*_tmp713=_tmp712;struct _dyneither_ptr*_tmp716;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp713)->tag == 1){_LL449: _tmp716=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp713)->f1;_LL44A:
 f=_tmp716;goto _LL448;}else{_LL44B: _LL44C:
(int)_throw((void*)({struct Cyc_Toc_Match_error_exn_struct*_tmp714=_cycalloc_atomic(sizeof(*_tmp714));_tmp714[0]=({struct Cyc_Toc_Match_error_exn_struct _tmp715;_tmp715.tag=Cyc_Toc_Match_error;_tmp715;});_tmp714;}));}_LL448:;}{
# 4150
void*_tmp717=(void*)_check_null(_tmp711->topt);
void*_tmp718=Cyc_Toc_typ_to_c_array(_tmp717);
struct Cyc_Absyn_Exp*_tmp719=Cyc_Toc_member_exp(path,f,0);
# 4154
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp70F->impl))->tagged)_tmp719=Cyc_Toc_member_exp(_tmp719,Cyc_Toc_val_sp,0);{
void*_tmp71A=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp70F->impl))->fields,f)))->type;
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp71A))
_tmp719=Cyc_Toc_cast_it(_tmp718,_tmp719);else{
if(!Cyc_Toc_is_array_type(_tmp71A) && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp71A)))
# 4161
_tmp719=Cyc_Absyn_deref_exp(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(_tmp718,Cyc_Toc_mt_tq),
Cyc_Absyn_address_exp(_tmp719,0)),0);}
# 4164
s=Cyc_Toc_seq_stmt_opt(s,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp719,_tmp711));};};};}
# 4166
return s;}}case 15: _LL442: _LL443:
# 4171
({void*_tmp71B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp71C="unknownid pat";_tag_dyneither(_tmp71C,sizeof(char),14);}),_tag_dyneither(_tmp71B,sizeof(void*),0));});case 16: _LL444: _LL445:
({void*_tmp71D=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp71E="unknowncall pat";_tag_dyneither(_tmp71E,sizeof(char),16);}),_tag_dyneither(_tmp71D,sizeof(void*),0));});default: _LL446: _LL447:
({void*_tmp71F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp720="exp pat";_tag_dyneither(_tmp720,sizeof(char),8);}),_tag_dyneither(_tmp71F,sizeof(void*),0));});}_LL41F:;}struct _tuple34{struct Cyc_Toc_Env*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Stmt*f3;};
# 4182
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 4190
void*_tmp72F=dopt;struct Cyc_List_List*_tmp76E;struct Cyc_List_List*_tmp76D;void*_tmp76C;struct Cyc_Tcpat_Rhs*_tmp76B;if(_tmp72F == 0){_LL44E: _LL44F:
# 4192
 return Cyc_Absyn_skip_stmt(0);}else{switch(*((int*)_tmp72F)){case 0: _LL450: _LL451:
 return Cyc_Toc_throw_match_stmt();case 1: _LL452: _tmp76B=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp72F)->f1;_LL453:
# 4197
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple31*_tmp730=(struct _tuple31*)lscs->hd;struct _tuple31*_tmp731=_tmp730;int*_tmp738;struct _dyneither_ptr*_tmp737;struct _dyneither_ptr*_tmp736;struct Cyc_Absyn_Switch_clause*_tmp735;_LL457: _tmp738=(int*)& _tmp731->f1;_tmp737=_tmp731->f2;_tmp736=_tmp731->f3;_tmp735=_tmp731->f4;_LL458:;{
struct Cyc_Absyn_Stmt*_tmp732=_tmp735->body;
if(_tmp732 == _tmp76B->rhs){
# 4202
if(*_tmp738)
return Cyc_Absyn_goto_stmt(_tmp737,0);
*_tmp738=1;{
# 4206
struct Cyc_Absyn_Stmt*init_opt=Cyc_Toc_extract_pattern_vars(rgn,& nv,decls,Cyc_Absyn_var_exp(v,0),_tmp735->pattern);
# 4209
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_label_stmt(_tmp736,_tmp735->body,0);
if(init_opt != 0)
res=Cyc_Absyn_seq_stmt(init_opt,res,0);
res=Cyc_Absyn_label_stmt(_tmp737,res,0);
*bodies=({struct Cyc_List_List*_tmp733=_region_malloc(rgn,sizeof(*_tmp733));_tmp733->hd=({struct _tuple34*_tmp734=_region_malloc(rgn,sizeof(*_tmp734));_tmp734->f1=nv;_tmp734->f2=_tmp736;_tmp734->f3=_tmp732;_tmp734;});_tmp733->tl=*bodies;_tmp733;});
return res;};}};}
# 4217
({void*_tmp739=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp73A="couldn't find rhs!";_tag_dyneither(_tmp73A,sizeof(char),19);}),_tag_dyneither(_tmp739,sizeof(void*),0));});default: _LL454: _tmp76E=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp72F)->f1;_tmp76D=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp72F)->f2;_tmp76C=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp72F)->f3;_LL455: {
# 4220
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp76C,lscs,v);
# 4222
struct Cyc_Absyn_Exp*_tmp73B=Cyc_Toc_compile_path(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp76E),Cyc_Absyn_var_exp(v,0));
struct Cyc_List_List*_tmp73C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp76D);
# 4225
struct _tuple32 _tmp73D=Cyc_Toc_admits_switch(_tmp73C);struct _tuple32 _tmp73E=_tmp73D;int _tmp76A;void*_tmp769;_LL45A: _tmp76A=_tmp73E.f1;_tmp769=_tmp73E.f2;_LL45B:;
if(_tmp76A > 1){
# 4229
struct Cyc_List_List*new_lscs=({struct Cyc_List_List*_tmp74F=_cycalloc(sizeof(*_tmp74F));_tmp74F->hd=({struct Cyc_Absyn_Switch_clause*_tmp750=_cycalloc(sizeof(*_tmp750));_tmp750->pattern=
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0);_tmp750->pat_vars=0;_tmp750->where_clause=0;_tmp750->body=res;_tmp750->loc=0;_tmp750;});_tmp74F->tl=0;_tmp74F;});
# 4232
for(0;_tmp73C != 0;_tmp73C=_tmp73C->tl){
struct _tuple29 _tmp73F=*((struct _tuple29*)_tmp73C->hd);struct _tuple29 _tmp740=_tmp73F;void*_tmp746;void*_tmp745;_LL45D: _tmp746=_tmp740.f1;_tmp745=_tmp740.f2;_LL45E:;{
# 4235
struct Cyc_Absyn_Pat*_tmp741=Cyc_Toc_compile_pat_test_as_case(_tmp746);
# 4237
struct Cyc_Absyn_Stmt*_tmp742=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp745,lscs,v);
# 4239
new_lscs=({struct Cyc_List_List*_tmp743=_cycalloc(sizeof(*_tmp743));_tmp743->hd=({struct Cyc_Absyn_Switch_clause*_tmp744=_cycalloc(sizeof(*_tmp744));_tmp744->pattern=_tmp741;_tmp744->pat_vars=0;_tmp744->where_clause=0;_tmp744->body=_tmp742;_tmp744->loc=0;_tmp744;});_tmp743->tl=new_lscs;_tmp743;});};}
# 4241
{void*_tmp747=_tmp769;struct _dyneither_ptr*_tmp74C;switch(*((int*)_tmp747)){case 1: _LL460: _LL461:
# 4244
 LOOP1: {
void*_tmp748=_tmp73B->r;void*_tmp749=_tmp748;struct Cyc_Absyn_Exp*_tmp74B;struct Cyc_Absyn_Exp*_tmp74A;switch(*((int*)_tmp749)){case 13: _LL469: _tmp74A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp749)->f2;_LL46A:
 _tmp73B=_tmp74A;goto LOOP1;case 19: _LL46B: _tmp74B=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp749)->f1;_LL46C:
 _tmp73B=_tmp74B;goto _LL468;default: _LL46D: _LL46E:
 goto _LL468;}_LL468:;}
# 4250
_tmp73B=Cyc_Absyn_deref_exp(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,Cyc_Toc_mt_tq),_tmp73B),0);goto _LL45F;case 3: _LL462: _tmp74C=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_tmp747)->f1;_LL463:
# 4253
 _tmp73B=Cyc_Toc_member_exp(Cyc_Toc_member_exp(_tmp73B,_tmp74C,0),Cyc_Toc_tag_sp,0);goto _LL45F;case 2: _LL464: _LL465:
# 4255
 goto _LL45F;default: _LL466: _LL467:
 goto _LL45F;}_LL45F:;}
# 4258
res=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp74D=_cycalloc(sizeof(*_tmp74D));_tmp74D[0]=({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct _tmp74E;_tmp74E.tag=10;_tmp74E.f1=_tmp73B;_tmp74E.f2=new_lscs;_tmp74E.f3=0;_tmp74E;});_tmp74D;}),0);}else{
# 4262
void*_tmp751=_tmp769;struct Cyc_Absyn_Exp*_tmp768;if(((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp751)->tag == 2){_LL470: _tmp768=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp751)->f1;_LL471: {
# 4264
struct Cyc_List_List*_tmp752=_tmp73C;void*_tmp761;struct Cyc_Tcpat_Rhs*_tmp760;if(_tmp752 != 0){if(((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple29*)((struct Cyc_List_List*)_tmp752)->hd)->f2)->tag == 1){if(((struct Cyc_List_List*)_tmp752)->tl == 0){_LL475: _tmp761=((struct _tuple29*)_tmp752->hd)->f1;_tmp760=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple29*)_tmp752->hd)->f2)->f1;_LL476:
# 4270
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple31*_tmp753=(struct _tuple31*)lscs->hd;struct _tuple31*_tmp754=_tmp753;int*_tmp75B;struct _dyneither_ptr*_tmp75A;struct _dyneither_ptr*_tmp759;struct Cyc_Absyn_Switch_clause*_tmp758;_LL47A: _tmp75B=(int*)& _tmp754->f1;_tmp75A=_tmp754->f2;_tmp759=_tmp754->f3;_tmp758=_tmp754->f4;_LL47B:;{
struct Cyc_Absyn_Stmt*_tmp755=_tmp758->body;
if(_tmp755 == _tmp760->rhs){
# 4275
if(*_tmp75B)
return Cyc_Absyn_goto_stmt(_tmp75A,0);
*_tmp75B=1;{
# 4279
struct Cyc_Absyn_Stmt*init_opt=Cyc_Toc_extract_pattern_vars(rgn,& nv,decls,Cyc_Absyn_var_exp(v,0),_tmp758->pattern);
# 4282
Cyc_Toc_exp_to_c(nv,_tmp768);{
# 4285
struct Cyc_Absyn_Stmt*r=Cyc_Absyn_label_stmt(_tmp759,_tmp758->body,0);
r=Cyc_Absyn_ifthenelse_stmt(_tmp768,r,res,0);
if(init_opt != 0)
r=Cyc_Absyn_seq_stmt(init_opt,r,0);
r=Cyc_Absyn_label_stmt(_tmp75A,r,0);
*bodies=({struct Cyc_List_List*_tmp756=_region_malloc(rgn,sizeof(*_tmp756));_tmp756->hd=({struct _tuple34*_tmp757=_region_malloc(rgn,sizeof(*_tmp757));_tmp757->f1=nv;_tmp757->f2=_tmp759;_tmp757->f3=_tmp755;_tmp757;});_tmp756->tl=*bodies;_tmp756;});
return r;};};}};}
# 4294
({void*_tmp75C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp75D="couldn't find rhs!";_tag_dyneither(_tmp75D,sizeof(char),19);}),_tag_dyneither(_tmp75C,sizeof(void*),0));});}else{goto _LL477;}}else{goto _LL477;}}else{_LL477: _LL478:
({void*_tmp75E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp75F="bad where clause in match compiler";_tag_dyneither(_tmp75F,sizeof(char),35);}),_tag_dyneither(_tmp75E,sizeof(void*),0));});}_LL474:;}}else{_LL472: _LL473:
# 4299
 for(0;_tmp73C != 0;_tmp73C=_tmp73C->tl){
struct _tuple29 _tmp762=*((struct _tuple29*)_tmp73C->hd);struct _tuple29 _tmp763=_tmp762;void*_tmp767;void*_tmp766;_LL47D: _tmp767=_tmp763.f1;_tmp766=_tmp763.f2;_LL47E:;{
struct Cyc_Absyn_Exp*_tmp764=Cyc_Toc_compile_pat_test(_tmp73B,_tmp767);
struct Cyc_Absyn_Stmt*_tmp765=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp766,lscs,v);
res=Cyc_Absyn_ifthenelse_stmt(_tmp764,_tmp765,res,0);};}}_LL46F:;}
# 4307
return res;}}}_LL44D:;}
# 4317
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 4319
for(0;bodies != 0;bodies=bodies->tl){
struct _tuple34*_tmp76F=(struct _tuple34*)bodies->hd;struct _tuple34*_tmp770=_tmp76F;struct Cyc_Toc_Env**_tmp772;struct Cyc_Absyn_Stmt*_tmp771;_LL480: _tmp772=(struct Cyc_Toc_Env**)& _tmp770->f1;_tmp771=_tmp770->f3;_LL481:;
if(_tmp771 == s)return _tmp772;}
# 4325
return 0;}
# 4329
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 4332
void*_tmp773=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(nv,e);{
# 4335
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp774=_new_region("rgn");struct _RegionHandle*rgn=& _tmp774;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp775=Cyc_Toc_share_env(rgn,nv);
# 4342
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple31*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 4347
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp775,& mydecls,& mybodies,dopt,lscs,v);
# 4355
{struct Cyc_List_List*_tmp776=lscs;for(0;_tmp776 != 0;_tmp776=_tmp776->tl){
struct _tuple31*_tmp777=(struct _tuple31*)_tmp776->hd;struct _tuple31*_tmp778=_tmp777;struct _dyneither_ptr*_tmp780;struct Cyc_Absyn_Switch_clause*_tmp77F;_LL483: _tmp780=_tmp778->f3;_tmp77F=_tmp778->f4;_LL484:;{
struct Cyc_Absyn_Stmt*s=_tmp77F->body;
# 4359
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 4362
if(envp == 0)continue;{
struct Cyc_Toc_Env*_tmp779=*envp;
# 4365
if(_tmp776->tl != 0){
struct _tuple31*_tmp77A=(struct _tuple31*)((struct Cyc_List_List*)_check_null(_tmp776->tl))->hd;struct _tuple31*_tmp77B=_tmp77A;struct _dyneither_ptr*_tmp77E;struct Cyc_Absyn_Switch_clause*_tmp77D;_LL486: _tmp77E=_tmp77B->f3;_tmp77D=_tmp77B->f4;_LL487:;{
# 4369
struct Cyc_Toc_Env**_tmp77C=Cyc_Toc_find_case_env(mybodies,_tmp77D->body);
# 4374
if(_tmp77C == 0)
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn,_tmp779,end_l),s);else{
# 4378
struct Cyc_List_List*vs=0;
if(_tmp77D->pat_vars != 0){
vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp77D->pat_vars))->v)).f1);
vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vs);}
# 4383
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(rgn,_tmp779,end_l,_tmp77E,vs),s);}};}else{
# 4387
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn,_tmp779,end_l),s);}};};}}{
# 4391
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(test_tree,Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),0),0);
# 4393
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp781=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
_tmp781->name=Cyc_Toc_temp_var();
_tmp781->type=Cyc_Toc_typ_to_c_array(_tmp781->type);
res=Cyc_Absyn_decl_stmt(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp782=_cycalloc(sizeof(*_tmp782));_tmp782[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp783;_tmp783.tag=0;_tmp783.f1=_tmp781;_tmp783;});_tmp782;}),0),res,0);}
# 4400
whole_s->r=(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),e,res,0))->r;
_npop_handler(0);return;};}
# 4339
;_pop_region(rgn);};}
# 4406
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
# 4408
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 4413
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 4415
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,({struct Cyc_List_List*_tmp784=_cycalloc(sizeof(*_tmp784));_tmp784->hd=
Cyc_Absyn_uint_exp((unsigned int)(n - 1),0);_tmp784->tl=0;_tmp784;}),0),0);}
# 4419
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt(s->r,0));}
# 4424
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 4426
while(1){
void*_tmp785=s->r;void*_tmp786=_tmp785;struct Cyc_Absyn_Exp*_tmp7E4;struct Cyc_Absyn_Stmt*_tmp7E3;struct Cyc_List_List*_tmp7E2;void*_tmp7E1;struct Cyc_Absyn_Stmt*_tmp7E0;struct Cyc_Absyn_Exp*_tmp7DF;struct _dyneither_ptr*_tmp7DE;struct Cyc_Absyn_Stmt*_tmp7DD;struct Cyc_Absyn_Decl*_tmp7DC;struct Cyc_Absyn_Stmt*_tmp7DB;struct Cyc_List_List*_tmp7DA;struct Cyc_Absyn_Switch_clause**_tmp7D9;struct Cyc_Absyn_Exp*_tmp7D8;struct Cyc_List_List*_tmp7D7;void*_tmp7D6;struct Cyc_Absyn_Exp*_tmp7D5;struct Cyc_Absyn_Exp*_tmp7D4;struct Cyc_Absyn_Exp*_tmp7D3;struct Cyc_Absyn_Stmt*_tmp7D2;struct Cyc_Absyn_Exp*_tmp7D1;struct Cyc_Absyn_Stmt*_tmp7D0;struct Cyc_Absyn_Exp*_tmp7CF;struct Cyc_Absyn_Stmt*_tmp7CE;struct Cyc_Absyn_Stmt*_tmp7CD;struct Cyc_Absyn_Exp*_tmp7CC;struct Cyc_Absyn_Stmt*_tmp7CB;struct Cyc_Absyn_Stmt*_tmp7CA;struct Cyc_Absyn_Exp*_tmp7C9;switch(*((int*)_tmp786)){case 0: _LL489: _LL48A:
# 4429
 return;case 1: _LL48B: _tmp7C9=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp786)->f1;_LL48C:
# 4431
 Cyc_Toc_exp_to_c(nv,_tmp7C9);
return;case 2: _LL48D: _tmp7CB=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp786)->f1;_tmp7CA=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp786)->f2;_LL48E:
# 4434
 Cyc_Toc_stmt_to_c(nv,_tmp7CB);
s=_tmp7CA;
continue;case 3: _LL48F: _tmp7CC=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp786)->f1;_LL490: {
# 4438
void*topt=0;
if(_tmp7CC != 0){
topt=Cyc_Toc_typ_to_c((void*)_check_null(_tmp7CC->topt));
Cyc_Toc_exp_to_c(nv,_tmp7CC);}{
# 4444
int _tmp787=Cyc_Toc_get_npop(s);
if(_tmp787 > 0){
if(topt != 0){
struct _tuple1*_tmp788=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp789=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp788,0),0);
s->r=(Cyc_Absyn_declare_stmt(_tmp788,topt,_tmp7CC,
Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(_tmp787),_tmp789,0),0))->r;}else{
# 4453
Cyc_Toc_do_npop_before(_tmp787,s);}}
# 4455
return;};}case 4: _LL491: _tmp7CF=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp786)->f1;_tmp7CE=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp786)->f2;_tmp7CD=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp786)->f3;_LL492:
# 4457
 Cyc_Toc_exp_to_c(nv,_tmp7CF);
Cyc_Toc_stmt_to_c(nv,_tmp7CE);
s=_tmp7CD;
continue;case 5: _LL493: _tmp7D1=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp786)->f1).f1;_tmp7D0=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp786)->f2;_LL494:
# 4462
 Cyc_Toc_exp_to_c(nv,_tmp7D1);{
struct _RegionHandle _tmp78A=_new_region("temp");struct _RegionHandle*temp=& _tmp78A;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7D0);
# 4466
_npop_handler(0);return;
# 4463
;_pop_region(temp);};case 6: _LL495: _LL496: {
# 4468
struct Cyc_Toc_Env*_tmp78B=nv;struct _dyneither_ptr**_tmp78C;_LL4AC: _tmp78C=_tmp78B->break_lab;_LL4AD:;
if(_tmp78C != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp78C);
# 4472
Cyc_Toc_do_npop_before(Cyc_Toc_get_npop(s),s);
return;}case 7: _LL497: _LL498: {
# 4475
struct Cyc_Toc_Env*_tmp78D=nv;struct _dyneither_ptr**_tmp78E;_LL4AF: _tmp78E=_tmp78D->continue_lab;_LL4B0:;
if(_tmp78E != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp78E);
goto _LL49A;}case 8: _LL499: _LL49A:
# 4481
 Cyc_Toc_do_npop_before(Cyc_Toc_get_npop(s),s);
return;case 9: _LL49B: _tmp7D5=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp786)->f1;_tmp7D4=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp786)->f2).f1;_tmp7D3=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp786)->f3).f1;_tmp7D2=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp786)->f4;_LL49C:
# 4485
 Cyc_Toc_exp_to_c(nv,_tmp7D5);Cyc_Toc_exp_to_c(nv,_tmp7D4);Cyc_Toc_exp_to_c(nv,_tmp7D3);{
struct _RegionHandle _tmp78F=_new_region("temp");struct _RegionHandle*temp=& _tmp78F;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7D2);
# 4489
_npop_handler(0);return;
# 4486
;_pop_region(temp);};case 10: _LL49D: _tmp7D8=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp786)->f1;_tmp7D7=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp786)->f2;_tmp7D6=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp786)->f3;_LL49E:
# 4491
 Cyc_Toc_xlate_switch(nv,s,_tmp7D8,_tmp7D7,_tmp7D6);
return;case 11: _LL49F: _tmp7DA=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp786)->f1;_tmp7D9=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp786)->f2;_LL4A0: {
# 4494
struct Cyc_Toc_Env*_tmp790=nv;struct Cyc_Toc_FallthruInfo*_tmp79B;_LL4B2: _tmp79B=_tmp790->fallthru_info;_LL4B3:;
if(_tmp79B == 0)
({void*_tmp791=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp792="fallthru in unexpected place";_tag_dyneither(_tmp792,sizeof(char),29);}),_tag_dyneither(_tmp791,sizeof(void*),0));});{
struct Cyc_Toc_FallthruInfo _tmp793=*_tmp79B;struct Cyc_Toc_FallthruInfo _tmp794=_tmp793;struct _dyneither_ptr*_tmp79A;struct Cyc_List_List*_tmp799;_LL4B5: _tmp79A=_tmp794.label;_tmp799=_tmp794.binders;_LL4B6:;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp79A,0);
# 4500
Cyc_Toc_do_npop_before(Cyc_Toc_get_npop(s),s2);{
struct Cyc_List_List*_tmp795=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp799);
struct Cyc_List_List*_tmp796=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7DA);
for(0;_tmp795 != 0;(_tmp795=_tmp795->tl,_tmp796=_tmp796->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp796))->hd);
s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp797=_cycalloc(sizeof(*_tmp797));_tmp797[0]=({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct _tmp798;_tmp798.tag=5;_tmp798.f1=(struct Cyc_Absyn_Vardecl*)_tmp795->hd;_tmp798;});_tmp797;}),0),(struct Cyc_Absyn_Exp*)_tmp796->hd,0),s2,0);}
# 4508
s->r=s2->r;
return;};};};}case 12: _LL4A1: _tmp7DC=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp786)->f1;_tmp7DB=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp786)->f2;_LL4A2:
# 4514
{void*_tmp79C=_tmp7DC->r;void*_tmp79D=_tmp79C;struct Cyc_Absyn_Tvar*_tmp7BB;struct Cyc_Absyn_Vardecl*_tmp7BA;int _tmp7B9;struct Cyc_Absyn_Exp*_tmp7B8;struct Cyc_Absyn_Fndecl*_tmp7B7;struct Cyc_List_List*_tmp7B6;struct Cyc_Absyn_Pat*_tmp7B5;struct Cyc_Absyn_Exp*_tmp7B4;void*_tmp7B3;struct Cyc_Absyn_Vardecl*_tmp7B2;switch(*((int*)_tmp79D)){case 0: _LL4B8: _tmp7B2=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp79D)->f1;_LL4B9:
 Cyc_Toc_local_decl_to_c(nv,_tmp7B2,_tmp7DB);goto _LL4B7;case 2: _LL4BA: _tmp7B5=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp79D)->f1;_tmp7B4=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp79D)->f3;_tmp7B3=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp79D)->f4;_LL4BB:
# 4522
{void*_tmp79E=_tmp7B5->r;void*_tmp79F=_tmp79E;struct Cyc_Absyn_Vardecl*_tmp7A2;if(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp79F)->tag == 1){if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp79F)->f2)->r)->tag == 0){_LL4C5: _tmp7A2=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp79F)->f1;_LL4C6:
# 4524
 _tmp7A2->name=Cyc_Toc_temp_var();
_tmp7A2->initializer=_tmp7B4;
_tmp7DC->r=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp7A0=_cycalloc(sizeof(*_tmp7A0));_tmp7A0[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp7A1;_tmp7A1.tag=0;_tmp7A1.f1=_tmp7A2;_tmp7A1;});_tmp7A0;});
Cyc_Toc_local_decl_to_c(nv,_tmp7A2,_tmp7DB);
goto _LL4C4;}else{goto _LL4C7;}}else{_LL4C7: _LL4C8:
# 4533
 s->r=(Cyc_Toc_letdecl_to_c(nv,_tmp7B5,_tmp7B3,(void*)_check_null(_tmp7B4->topt),_tmp7B4,_tmp7DB))->r;
goto _LL4C4;}_LL4C4:;}
# 4536
goto _LL4B7;case 3: _LL4BC: _tmp7B6=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp79D)->f1;_LL4BD: {
# 4540
struct Cyc_List_List*_tmp7A3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7B6);
if(_tmp7A3 == 0)
({void*_tmp7A4=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp7A5="empty Letv_d";_tag_dyneither(_tmp7A5,sizeof(char),13);}),_tag_dyneither(_tmp7A4,sizeof(void*),0));});
_tmp7DC->r=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp7A6=_cycalloc(sizeof(*_tmp7A6));_tmp7A6[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp7A7;_tmp7A7.tag=0;_tmp7A7.f1=(struct Cyc_Absyn_Vardecl*)_tmp7A3->hd;_tmp7A7;});_tmp7A6;});
_tmp7A3=_tmp7A3->tl;
for(0;_tmp7A3 != 0;_tmp7A3=_tmp7A3->tl){
struct Cyc_Absyn_Decl*_tmp7A8=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp7A9=_cycalloc(sizeof(*_tmp7A9));_tmp7A9[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp7AA;_tmp7AA.tag=0;_tmp7AA.f1=(struct Cyc_Absyn_Vardecl*)_tmp7A3->hd;_tmp7AA;});_tmp7A9;}),0);
s->r=(Cyc_Absyn_decl_stmt(_tmp7A8,Cyc_Absyn_new_stmt(s->r,0),0))->r;}
# 4549
Cyc_Toc_stmt_to_c(nv,s);
goto _LL4B7;}case 1: _LL4BE: _tmp7B7=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp79D)->f1;_LL4BF:
# 4552
 Cyc_Toc_fndecl_to_c(nv,_tmp7B7,0);
Cyc_Toc_stmt_to_c(nv,_tmp7DB);
goto _LL4B7;case 4: _LL4C0: _tmp7BB=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp79D)->f1;_tmp7BA=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp79D)->f2;_tmp7B9=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp79D)->f3;_tmp7B8=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp79D)->f4;_LL4C1: {
# 4556
struct Cyc_Absyn_Stmt*_tmp7AB=_tmp7DB;
# 4564
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple1*rh_var=Cyc_Toc_temp_var();
struct _tuple1*x_var=_tmp7BA->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);
# 4571
Cyc_Toc_stmt_to_c(nv,_tmp7AB);
if(Cyc_Absyn_no_regions)
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_uint_exp(0,0),_tmp7AB,0))->r;else{
if((unsigned int)_tmp7B8){
Cyc_Toc_exp_to_c(nv,_tmp7B8);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(Cyc_Absyn_aggrarrow_exp(_tmp7B8,_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"h",sizeof(char),2),0),0);
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,_tmp7AB,0))->r;};}else{
# 4580
s->r=(
Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,({struct Cyc_List_List*_tmp7AD=_cycalloc(sizeof(*_tmp7AD));_tmp7AD->hd=
Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0);_tmp7AD->tl=0;_tmp7AD;}),0),
# 4585
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,({struct Cyc_Absyn_Exp*_tmp7AE[1];_tmp7AE[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp7AE,sizeof(struct Cyc_Absyn_Exp*),1));}),0),0),
Cyc_Absyn_seq_stmt(_tmp7AB,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,({struct Cyc_Absyn_Exp*_tmp7AF[1];_tmp7AF[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp7AF,sizeof(struct Cyc_Absyn_Exp*),1));}),0),0),0),0),0),0))->r;}}
# 4591
return;}default: _LL4C2: _LL4C3:
({void*_tmp7B0=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp7B1="bad nested declaration within function";_tag_dyneither(_tmp7B1,sizeof(char),39);}),_tag_dyneither(_tmp7B0,sizeof(void*),0));});}_LL4B7:;}
# 4594
return;case 13: _LL4A3: _tmp7DE=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp786)->f1;_tmp7DD=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp786)->f2;_LL4A4:
# 4596
 s=_tmp7DD;continue;case 14: _LL4A5: _tmp7E0=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp786)->f1;_tmp7DF=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp786)->f2).f1;_LL4A6: {
# 4598
struct _RegionHandle _tmp7BC=_new_region("temp");struct _RegionHandle*temp=& _tmp7BC;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7E0);
Cyc_Toc_exp_to_c(nv,_tmp7DF);
# 4602
_npop_handler(0);return;
# 4598
;_pop_region(temp);}case 15: _LL4A7: _tmp7E3=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp786)->f1;_tmp7E2=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp786)->f2;_tmp7E1=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp786)->f3;_LL4A8: {
# 4616 "toc.cyc"
struct _tuple1*h_var=Cyc_Toc_temp_var();
struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
# 4626
e_exp->topt=e_typ;{
struct _RegionHandle _tmp7BD=_new_region("temp");struct _RegionHandle*temp=& _tmp7BD;_push_region(temp);
# 4629
Cyc_Toc_stmt_to_c(nv,_tmp7E3);{
struct Cyc_Absyn_Stmt*_tmp7BE=Cyc_Absyn_seq_stmt(_tmp7E3,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
# 4634
struct Cyc_Absyn_Stmt*_tmp7BF=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp7C6=_cycalloc(sizeof(*_tmp7C6));_tmp7C6[0]=({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct _tmp7C7;_tmp7C7.tag=10;_tmp7C7.f1=e_exp;_tmp7C7.f2=_tmp7E2;_tmp7C7.f3=_tmp7E1;_tmp7C7;});_tmp7C6;}),0);
# 4636
Cyc_Toc_stmt_to_c(nv,_tmp7BF);{
# 4639
struct Cyc_Absyn_Exp*_tmp7C0=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,({struct Cyc_List_List*_tmp7C5=_cycalloc(sizeof(*_tmp7C5));_tmp7C5->hd=
Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0);_tmp7C5->tl=0;_tmp7C5;}),0);
# 4643
struct Cyc_Absyn_Stmt*_tmp7C1=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,({struct Cyc_List_List*_tmp7C4=_cycalloc(sizeof(*_tmp7C4));_tmp7C4->hd=
Cyc_Absyn_address_exp(h_exp,0);_tmp7C4->tl=0;_tmp7C4;}),0),0);
# 4647
struct Cyc_Absyn_Exp*_tmp7C2=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0);
struct Cyc_Absyn_Exp*_tmp7C3=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0);
s->r=(
Cyc_Absyn_declare_stmt(h_var,h_typ,0,
Cyc_Absyn_seq_stmt(_tmp7C1,
Cyc_Absyn_declare_stmt(was_thrown_var,was_thrown_typ,_tmp7C2,
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp7C0,
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp7C3,0),
Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0),_tmp7BE,
# 4658
Cyc_Absyn_declare_stmt(e_var,e_typ,
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp7BF,0),0),0),0),0),0))->r;};};
# 4662
_npop_handler(0);return;
# 4627
;_pop_region(temp);};}default: _LL4A9: _tmp7E4=((struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp786)->f1;_LL4AA:
# 4664
 if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{
# 4667
Cyc_Toc_exp_to_c(nv,_tmp7E4);
s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,({struct Cyc_List_List*_tmp7C8=_cycalloc(sizeof(*_tmp7C8));_tmp7C8->hd=_tmp7E4;_tmp7C8->tl=0;_tmp7C8;}),0));}
# 4670
return;}_LL488:;}}
# 4679
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->requires_clause=0;
f->ensures_clause=0;
f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp7E5=_new_region("frgn");struct _RegionHandle*frgn=& _tmp7E5;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp7E6=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp7E7=f->args;for(0;_tmp7E7 != 0;_tmp7E7=_tmp7E7->tl){
struct _tuple1*_tmp7E8=({struct _tuple1*_tmp7E9=_cycalloc(sizeof(*_tmp7E9));_tmp7E9->f1=Cyc_Absyn_Loc_n;_tmp7E9->f2=(*((struct _tuple9*)_tmp7E7->hd)).f1;_tmp7E9;});
(*((struct _tuple9*)_tmp7E7->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp7E7->hd)).f3);}}
# 4695
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}
# 4699
Cyc_Toc_fn_pop_table=({struct Cyc_Hashtable_Table**_tmp7EA=_cycalloc(sizeof(*_tmp7EA));_tmp7EA[0]=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_gpop_tables)),f);_tmp7EA;});
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp7EB=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp7EC=_tmp7EB;struct _dyneither_ptr*_tmp7F5;struct Cyc_Absyn_Tqual _tmp7F4;void*_tmp7F3;int _tmp7F2;_LL4CA: _tmp7F5=_tmp7EC.name;_tmp7F4=_tmp7EC.tq;_tmp7F3=_tmp7EC.type;_tmp7F2=_tmp7EC.inject;_LL4CB:;{
void*_tmp7ED=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp7F3,(void*)& Cyc_Absyn_HeapRgn_val,_tmp7F4,Cyc_Absyn_false_conref));
struct _tuple1*_tmp7EE=({struct _tuple1*_tmp7F1=_cycalloc(sizeof(*_tmp7F1));_tmp7F1->f1=Cyc_Absyn_Loc_n;_tmp7F1->f2=(struct _dyneither_ptr*)_check_null(_tmp7F5);_tmp7F1;});
f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,({struct Cyc_List_List*_tmp7EF=_cycalloc(sizeof(*_tmp7EF));_tmp7EF->hd=({struct _tuple9*_tmp7F0=_cycalloc(sizeof(*_tmp7F0));_tmp7F0->f1=_tmp7F5;_tmp7F0->f2=_tmp7F4;_tmp7F0->f3=_tmp7ED;_tmp7F0;});_tmp7EF->tl=0;_tmp7EF;}));
f->cyc_varargs=0;};}
# 4708
{struct Cyc_List_List*_tmp7F6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp7F6 != 0;_tmp7F6=_tmp7F6->tl){
((struct Cyc_Absyn_Vardecl*)_tmp7F6->hd)->type=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp7F6->hd)->type);}}
# 4711
Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp7E6),f->body);}
# 4688
;_pop_region(frgn);};}
# 4716
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
enum Cyc_Absyn_Scope _tmp7F7=s;switch(_tmp7F7){case Cyc_Absyn_Abstract: _LL4CD: _LL4CE:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL4CF: _LL4D0:
 return Cyc_Absyn_Extern;default: _LL4D1: _LL4D2:
 return s;}_LL4CC:;}struct _tuple35{struct Cyc_Absyn_Aggrdecl*f1;int f2;};struct _tuple36{struct Cyc_Toc_TocState*f1;struct _tuple35*f2;};
# 4733 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple36*env){
# 4736
struct _tuple36 _tmp7F8=*env;struct _tuple36 _tmp7F9=_tmp7F8;struct Cyc_Toc_TocState*_tmp838;struct Cyc_Absyn_Aggrdecl*_tmp837;int _tmp836;_LL4D4: _tmp838=_tmp7F9.f1;_tmp837=(_tmp7F9.f2)->f1;_tmp836=(_tmp7F9.f2)->f2;_LL4D5:;{
struct _tuple1*_tmp7FA=_tmp837->name;
struct Cyc_Toc_TocState _tmp7FB=*_tmp838;struct Cyc_Toc_TocState _tmp7FC=_tmp7FB;struct Cyc_Dict_Dict*_tmp835;_LL4D7: _tmp835=_tmp7FC.aggrs_so_far;_LL4D8:;{
int seen_defn_before;
struct _tuple18**_tmp7FD=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp835,_tmp7FA);
if(_tmp7FD == 0){
seen_defn_before=0;{
struct _tuple18*v;
if(_tmp837->kind == Cyc_Absyn_StructA)
v=({struct _tuple18*_tmp7FE=_region_malloc(d,sizeof(*_tmp7FE));_tmp7FE->f1=_tmp837;_tmp7FE->f2=Cyc_Absyn_strctq(_tmp7FA);_tmp7FE;});else{
# 4747
v=({struct _tuple18*_tmp7FF=_region_malloc(d,sizeof(*_tmp7FF));_tmp7FF->f1=_tmp837;_tmp7FF->f2=Cyc_Absyn_unionq_typ(_tmp7FA);_tmp7FF;});}
*_tmp835=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp835,_tmp7FA,v);};}else{
# 4750
struct _tuple18*_tmp800=*_tmp7FD;struct _tuple18*_tmp801=_tmp800;struct Cyc_Absyn_Aggrdecl*_tmp804;void*_tmp803;_LL4DA: _tmp804=_tmp801->f1;_tmp803=_tmp801->f2;_LL4DB:;
if(_tmp804->impl == 0){
*_tmp835=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp835,_tmp7FA,({struct _tuple18*_tmp802=_region_malloc(d,sizeof(*_tmp802));_tmp802->f1=_tmp837;_tmp802->f2=_tmp803;_tmp802;}));
seen_defn_before=0;}else{
# 4755
seen_defn_before=1;}}{
# 4757
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp834=_cycalloc(sizeof(*_tmp834));_tmp834->kind=_tmp837->kind;_tmp834->sc=Cyc_Absyn_Public;_tmp834->name=_tmp837->name;_tmp834->tvs=0;_tmp834->impl=0;_tmp834->expected_mem_kind=0;_tmp834->attributes=_tmp837->attributes;_tmp834;});
# 4764
if(_tmp837->impl != 0  && !seen_defn_before){
new_ad->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp805=_cycalloc(sizeof(*_tmp805));_tmp805->exist_vars=0;_tmp805->rgn_po=0;_tmp805->fields=0;_tmp805->tagged=0;_tmp805;});{
# 4769
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp806=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp837->impl))->fields;for(0;_tmp806 != 0;_tmp806=_tmp806->tl){
# 4773
struct Cyc_Absyn_Aggrfield*_tmp807=(struct Cyc_Absyn_Aggrfield*)_tmp806->hd;
void*_tmp808=_tmp807->type;
struct Cyc_List_List*_tmp809=_tmp807->attributes;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp808)) && (
_tmp837->kind == Cyc_Absyn_StructA  && _tmp806->tl == 0  || _tmp837->kind == Cyc_Absyn_UnionA)){
# 4787 "toc.cyc"
void*_tmp80A=Cyc_Tcutil_compress(_tmp808);void*_tmp80B=_tmp80A;void*_tmp818;struct Cyc_Absyn_Tqual _tmp817;union Cyc_Absyn_Constraint*_tmp816;unsigned int _tmp815;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp80B)->tag == 8){_LL4DD: _tmp818=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp80B)->f1).elt_type;_tmp817=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp80B)->f1).tq;_tmp816=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp80B)->f1).zero_term;_tmp815=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp80B)->f1).zt_loc;_LL4DE:
# 4790
 _tmp808=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp80C=_cycalloc(sizeof(*_tmp80C));_tmp80C[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp80D;_tmp80D.tag=8;_tmp80D.f1=({struct Cyc_Absyn_ArrayInfo _tmp80E;_tmp80E.elt_type=_tmp818;_tmp80E.tq=_tmp817;_tmp80E.num_elts=Cyc_Absyn_uint_exp(0,0);_tmp80E.zero_term=_tmp816;_tmp80E.zt_loc=_tmp815;_tmp80E;});_tmp80D;});_tmp80C;});
goto _LL4DC;}else{_LL4DF: _LL4E0:
# 4793
 _tmp809=({struct Cyc_List_List*_tmp80F=_cycalloc(sizeof(*_tmp80F));_tmp80F->hd=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp810=_cycalloc(sizeof(*_tmp810));_tmp810[0]=({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmp811;_tmp811.tag=6;_tmp811.f1=0;_tmp811;});_tmp810;});_tmp80F->tl=_tmp809;_tmp80F;});
_tmp808=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp812=_cycalloc(sizeof(*_tmp812));_tmp812[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp813;_tmp813.tag=8;_tmp813.f1=({struct Cyc_Absyn_ArrayInfo _tmp814;_tmp814.elt_type=Cyc_Absyn_void_star_typ();_tmp814.tq=
Cyc_Absyn_empty_tqual(0);_tmp814.num_elts=
Cyc_Absyn_uint_exp(0,0);_tmp814.zero_term=
((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0);_tmp814.zt_loc=0;_tmp814;});_tmp813;});_tmp812;});}_LL4DC:;}{
# 4800
struct Cyc_Absyn_Aggrfield*_tmp819=({struct Cyc_Absyn_Aggrfield*_tmp82F=_cycalloc(sizeof(*_tmp82F));_tmp82F->name=_tmp807->name;_tmp82F->tq=Cyc_Toc_mt_tq;_tmp82F->type=
# 4802
Cyc_Toc_typ_to_c(_tmp808);_tmp82F->width=_tmp807->width;_tmp82F->attributes=_tmp809;_tmp82F->requires_clause=0;_tmp82F;});
# 4810
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp837->impl))->tagged){
void*_tmp81A=_tmp819->type;
struct _dyneither_ptr*_tmp81B=_tmp819->name;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp82D;_tmp82D.tag=0;_tmp82D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp81B);({struct Cyc_String_pa_PrintArg_struct _tmp82C;_tmp82C.tag=0;_tmp82C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp837->name).f2);({void*_tmp82A[2]={& _tmp82C,& _tmp82D};Cyc_aprintf(({const char*_tmp82B="_union_%s_%s";_tag_dyneither(_tmp82B,sizeof(char),13);}),_tag_dyneither(_tmp82A,sizeof(void*),2));});});});
struct _dyneither_ptr*str=({struct _dyneither_ptr*_tmp829=_cycalloc(sizeof(*_tmp829));_tmp829[0]=s;_tmp829;});
struct Cyc_Absyn_Aggrfield*_tmp81C=({struct Cyc_Absyn_Aggrfield*_tmp828=_cycalloc(sizeof(*_tmp828));_tmp828->name=Cyc_Toc_val_sp;_tmp828->tq=Cyc_Toc_mt_tq;_tmp828->type=_tmp81A;_tmp828->width=0;_tmp828->attributes=0;_tmp828->requires_clause=0;_tmp828;});
struct Cyc_Absyn_Aggrfield*_tmp81D=({struct Cyc_Absyn_Aggrfield*_tmp827=_cycalloc(sizeof(*_tmp827));_tmp827->name=Cyc_Toc_tag_sp;_tmp827->tq=Cyc_Toc_mt_tq;_tmp827->type=Cyc_Absyn_sint_typ;_tmp827->width=0;_tmp827->attributes=0;_tmp827->requires_clause=0;_tmp827;});
struct Cyc_List_List*_tmp81E=({struct Cyc_Absyn_Aggrfield*_tmp826[2];_tmp826[1]=_tmp81C;_tmp826[0]=_tmp81D;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp826,sizeof(struct Cyc_Absyn_Aggrfield*),2));});
struct Cyc_Absyn_Aggrdecl*_tmp81F=({struct Cyc_Absyn_Aggrdecl*_tmp823=_cycalloc(sizeof(*_tmp823));_tmp823->kind=Cyc_Absyn_StructA;_tmp823->sc=Cyc_Absyn_Public;_tmp823->name=({struct _tuple1*_tmp825=_cycalloc(sizeof(*_tmp825));_tmp825->f1=Cyc_Absyn_Loc_n;_tmp825->f2=str;_tmp825;});_tmp823->tvs=0;_tmp823->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp824=_cycalloc(sizeof(*_tmp824));_tmp824->exist_vars=0;_tmp824->rgn_po=0;_tmp824->fields=_tmp81E;_tmp824->tagged=0;_tmp824;});_tmp823->expected_mem_kind=0;_tmp823->attributes=0;_tmp823;});
# 4823
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp820=_cycalloc(sizeof(*_tmp820));_tmp820->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp821=_cycalloc(sizeof(*_tmp821));_tmp821[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp822;_tmp822.tag=5;_tmp822.f1=_tmp81F;_tmp822;});_tmp821;}),0);_tmp820->tl=Cyc_Toc_result_decls;_tmp820;});
_tmp819->type=Cyc_Absyn_strct(str);}
# 4826
new_fields=({struct Cyc_List_List*_tmp82E=_cycalloc(sizeof(*_tmp82E));_tmp82E->hd=_tmp819;_tmp82E->tl=new_fields;_tmp82E;});};}}
# 4828
(new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);};}
# 4830
if(_tmp836)
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp830=_cycalloc(sizeof(*_tmp830));_tmp830->hd=({struct Cyc_Absyn_Decl*_tmp831=_cycalloc(sizeof(*_tmp831));_tmp831->r=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp832=_cycalloc(sizeof(*_tmp832));_tmp832[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp833;_tmp833.tag=5;_tmp833.f1=new_ad;_tmp833;});_tmp832;});_tmp831->loc=0;_tmp831;});_tmp830->tl=Cyc_Toc_result_decls;_tmp830;});
return 0;};};};}
# 4835
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple35 p=({struct _tuple35 _tmp839;_tmp839.f1=ad;_tmp839.f2=add_to_decls;_tmp839;});
((int(*)(struct _tuple35*arg,int(*f)(struct _RegionHandle*,struct _tuple36*)))Cyc_Toc_use_toc_state)(& p,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple37{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 4864 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple37*env){
# 4867
struct _tuple37 _tmp83A=*env;struct _tuple37 _tmp83B=_tmp83A;struct Cyc_Set_Set**_tmp84D;struct Cyc_Absyn_Datatypedecl*_tmp84C;_LL4E2: _tmp84D=(_tmp83B.f1)->datatypes_so_far;_tmp84C=_tmp83B.f2;_LL4E3:;{
struct _tuple1*_tmp83C=_tmp84C->name;
if(_tmp84C->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*_tmp84D,_tmp83C))
return 0;
*_tmp84D=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*_tmp84D,_tmp83C);
{struct Cyc_List_List*_tmp83D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp84C->fields))->v;for(0;_tmp83D != 0;_tmp83D=_tmp83D->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp83D->hd;
# 4875
struct Cyc_List_List*_tmp83E=0;
int i=1;
{struct Cyc_List_List*_tmp83F=f->typs;for(0;_tmp83F != 0;(_tmp83F=_tmp83F->tl,i ++)){
struct _dyneither_ptr*_tmp840=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmp841=({struct Cyc_Absyn_Aggrfield*_tmp843=_cycalloc(sizeof(*_tmp843));_tmp843->name=_tmp840;_tmp843->tq=(*((struct _tuple11*)_tmp83F->hd)).f1;_tmp843->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp83F->hd)).f2);_tmp843->width=0;_tmp843->attributes=0;_tmp843->requires_clause=0;_tmp843;});
_tmp83E=({struct Cyc_List_List*_tmp842=_cycalloc(sizeof(*_tmp842));_tmp842->hd=_tmp841;_tmp842->tl=_tmp83E;_tmp842;});}}
# 4883
_tmp83E=({struct Cyc_List_List*_tmp844=_cycalloc(sizeof(*_tmp844));_tmp844->hd=({struct Cyc_Absyn_Aggrfield*_tmp845=_cycalloc(sizeof(*_tmp845));_tmp845->name=Cyc_Toc_tag_sp;_tmp845->tq=Cyc_Toc_mt_tq;_tmp845->type=Cyc_Absyn_sint_typ;_tmp845->width=0;_tmp845->attributes=0;_tmp845->requires_clause=0;_tmp845;});_tmp844->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp83E);_tmp844;});{
struct Cyc_Absyn_Aggrdecl*_tmp846=({struct Cyc_Absyn_Aggrdecl*_tmp84A=_cycalloc(sizeof(*_tmp84A));_tmp84A->kind=Cyc_Absyn_StructA;_tmp84A->sc=Cyc_Absyn_Public;_tmp84A->name=
# 4887
Cyc_Toc_collapse_qvars(f->name,_tmp84C->name);_tmp84A->tvs=0;_tmp84A->expected_mem_kind=0;_tmp84A->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp84B=_cycalloc(sizeof(*_tmp84B));_tmp84B->exist_vars=0;_tmp84B->rgn_po=0;_tmp84B->fields=_tmp83E;_tmp84B->tagged=0;_tmp84B;});_tmp84A->attributes=0;_tmp84A;});
# 4891
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp847=_cycalloc(sizeof(*_tmp847));_tmp847->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp848=_cycalloc(sizeof(*_tmp848));_tmp848[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp849;_tmp849.tag=5;_tmp849.f1=_tmp846;_tmp849;});_tmp848;}),0);_tmp847->tl=Cyc_Toc_result_decls;_tmp847;});};}}
# 4893
return 0;};}
# 4896
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple37*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 4915 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple37*env){
# 4918
struct _tuple37 _tmp84E=*env;struct _tuple37 _tmp84F=_tmp84E;struct Cyc_Toc_TocState*_tmp874;struct Cyc_Absyn_Datatypedecl*_tmp873;_LL4E5: _tmp874=_tmp84F.f1;_tmp873=_tmp84F.f2;_LL4E6:;
if(_tmp873->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp850=*_tmp874;struct Cyc_Toc_TocState _tmp851=_tmp850;struct Cyc_Dict_Dict*_tmp872;_LL4E8: _tmp872=_tmp851.xdatatypes_so_far;_LL4E9:;{
struct _tuple1*_tmp852=_tmp873->name;
{struct Cyc_List_List*_tmp853=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp873->fields))->v;for(0;_tmp853 != 0;_tmp853=_tmp853->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp853->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp854=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0);
void*_tmp855=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,_tmp854,Cyc_Absyn_false_conref,0);
# 4929
int*_tmp856=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp872,f->name);int*_tmp857=_tmp856;if(_tmp857 == 0){_LL4EB: _LL4EC: {
# 4931
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0);{
# 4935
struct Cyc_Absyn_Vardecl*_tmp858=Cyc_Absyn_new_vardecl(f->name,_tmp855,initopt);
_tmp858->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp859=_cycalloc(sizeof(*_tmp859));_tmp859->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp85A=_cycalloc(sizeof(*_tmp85A));_tmp85A[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp85B;_tmp85B.tag=0;_tmp85B.f1=_tmp858;_tmp85B;});_tmp85A;}),0);_tmp859->tl=Cyc_Toc_result_decls;_tmp859;});
*_tmp872=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp872,f->name,f->sc != Cyc_Absyn_Extern);{
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp85C=f->typs;for(0;_tmp85C != 0;(_tmp85C=_tmp85C->tl,i ++)){
struct _dyneither_ptr*_tmp85D=({struct _dyneither_ptr*_tmp861=_cycalloc(sizeof(*_tmp861));_tmp861[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp864;_tmp864.tag=1;_tmp864.f1=(unsigned long)i;({void*_tmp862[1]={& _tmp864};Cyc_aprintf(({const char*_tmp863="f%d";_tag_dyneither(_tmp863,sizeof(char),4);}),_tag_dyneither(_tmp862,sizeof(void*),1));});});_tmp861;});
struct Cyc_Absyn_Aggrfield*_tmp85E=({struct Cyc_Absyn_Aggrfield*_tmp860=_cycalloc(sizeof(*_tmp860));_tmp860->name=_tmp85D;_tmp860->tq=(*((struct _tuple11*)_tmp85C->hd)).f1;_tmp860->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp85C->hd)).f2);_tmp860->width=0;_tmp860->attributes=0;_tmp860->requires_clause=0;_tmp860;});
fields=({struct Cyc_List_List*_tmp85F=_cycalloc(sizeof(*_tmp85F));_tmp85F->hd=_tmp85E;_tmp85F->tl=fields;_tmp85F;});}}
# 4948
fields=({struct Cyc_List_List*_tmp865=_cycalloc(sizeof(*_tmp865));_tmp865->hd=({struct Cyc_Absyn_Aggrfield*_tmp866=_cycalloc(sizeof(*_tmp866));_tmp866->name=Cyc_Toc_tag_sp;_tmp866->tq=Cyc_Toc_mt_tq;_tmp866->type=
Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq);_tmp866->width=0;_tmp866->attributes=0;_tmp866->requires_clause=0;_tmp866;});_tmp865->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp865;});{
struct Cyc_Absyn_Aggrdecl*_tmp867=({struct Cyc_Absyn_Aggrdecl*_tmp86B=_cycalloc(sizeof(*_tmp86B));_tmp86B->kind=Cyc_Absyn_StructA;_tmp86B->sc=Cyc_Absyn_Public;_tmp86B->name=
# 4953
Cyc_Toc_collapse_qvars(f->name,_tmp873->name);_tmp86B->tvs=0;_tmp86B->expected_mem_kind=0;_tmp86B->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp86C=_cycalloc(sizeof(*_tmp86C));_tmp86C->exist_vars=0;_tmp86C->rgn_po=0;_tmp86C->fields=fields;_tmp86C->tagged=0;_tmp86C;});_tmp86B->attributes=0;_tmp86B;});
# 4957
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp868=_cycalloc(sizeof(*_tmp868));_tmp868->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp869=_cycalloc(sizeof(*_tmp869));_tmp869[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp86A;_tmp86A.tag=5;_tmp86A.f1=_tmp867;_tmp86A;});_tmp869;}),0);_tmp868->tl=Cyc_Toc_result_decls;_tmp868;});
# 4959
goto _LL4EA;};};};}}else{if(*((int*)_tmp857)== 0){_LL4ED: _LL4EE:
# 4961
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp86D=Cyc_Absyn_string_exp(*fn,0);
struct Cyc_Absyn_Vardecl*_tmp86E=Cyc_Absyn_new_vardecl(f->name,_tmp855,_tmp86D);
_tmp86E->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp86F=_cycalloc(sizeof(*_tmp86F));_tmp86F->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp870=_cycalloc(sizeof(*_tmp870));_tmp870[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp871;_tmp871.tag=0;_tmp871.f1=_tmp86E;_tmp871;});_tmp870;}),0);_tmp86F->tl=Cyc_Toc_result_decls;_tmp86F;});
*_tmp872=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp872,f->name,1);}
# 4968
goto _LL4EA;}else{_LL4EF: _LL4F0:
 goto _LL4EA;}}_LL4EA:;}}
# 4972
return 0;};};}
# 4975
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple37*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 4979
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4985
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
vd->type=Cyc_Toc_typ_to_c(old_typ);
# 4989
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp875=init->r;void*_tmp876=_tmp875;struct Cyc_Absyn_Vardecl*_tmp87C;struct Cyc_Absyn_Exp*_tmp87B;struct Cyc_Absyn_Exp*_tmp87A;int _tmp879;if(((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp876)->tag == 26){_LL4F2: _tmp87C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp876)->f1;_tmp87B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp876)->f2;_tmp87A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp876)->f3;_tmp879=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp876)->f4;_LL4F3:
# 4996
 vd->initializer=0;
s->r=(Cyc_Toc_init_comprehension(nv,old_typ,Cyc_Absyn_var_exp(vd->name,0),_tmp87C,_tmp87B,_tmp87A,_tmp879,
Cyc_Absyn_new_stmt(s->r,0),0))->r;
goto _LL4F1;}else{_LL4F4: _LL4F5:
# 5001
 if(vd->sc == Cyc_Absyn_Static){
# 5005
struct _RegionHandle _tmp877=_new_region("temp");struct _RegionHandle*temp=& _tmp877;_push_region(temp);
{struct Cyc_Toc_Env*_tmp878=Cyc_Toc_set_toplevel(temp,nv);
Cyc_Toc_exp_to_c(_tmp878,init);}
# 5006
;_pop_region(temp);}else{
# 5010
Cyc_Toc_exp_to_c(nv,init);}
goto _LL4F1;}_LL4F1:;}else{
# 5015
void*_tmp87D=Cyc_Tcutil_compress(old_typ);void*_tmp87E=_tmp87D;void*_tmp885;struct Cyc_Absyn_Exp*_tmp884;union Cyc_Absyn_Constraint*_tmp883;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp87E)->tag == 8){_LL4F7: _tmp885=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp87E)->f1).elt_type;_tmp884=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp87E)->f1).num_elts;_tmp883=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp87E)->f1).zero_term;if(
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp883)){_LL4F8:
 if(_tmp884 == 0)
({void*_tmp87F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp880="can't initialize zero-terminated array -- size unknown";_tag_dyneither(_tmp880,sizeof(char),55);}),_tag_dyneither(_tmp87F,sizeof(void*),0));});{
struct Cyc_Absyn_Exp*num_elts=_tmp884;
struct Cyc_Absyn_Exp*_tmp881=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),
Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);
# 5023
struct Cyc_Absyn_Exp*_tmp882=Cyc_Absyn_signed_int_exp(0,0);
s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp881,_tmp882,0),0),
Cyc_Absyn_new_stmt(s->r,0));
goto _LL4F6;};}else{goto _LL4F9;}}else{_LL4F9: _LL4FA:
 goto _LL4F6;}_LL4F6:;}}
# 5034
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success){
void*_tmp886=d;struct Cyc_List_List*_tmp88D;struct Cyc_List_List*_tmp88C;void**_tmp88B;struct Cyc_Tcpat_Rhs*_tmp88A;switch(*((int*)_tmp886)){case 0: _LL4FC: _LL4FD:
 return d;case 1: _LL4FE: _tmp88A=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp886)->f1;_LL4FF:
 _tmp88A->rhs=success;return d;default: _LL500: _tmp88D=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp886)->f1;_tmp88C=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp886)->f2;_tmp88B=(void**)&((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp886)->f3;_LL501:
# 5039
*_tmp88B=Cyc_Toc_rewrite_decision(*_tmp88B,success);
for(0;_tmp88C != 0;_tmp88C=_tmp88C->tl){
struct _tuple29*_tmp887=(struct _tuple29*)_tmp88C->hd;struct _tuple29*_tmp888=_tmp887;void**_tmp889;_LL503: _tmp889=(void**)& _tmp888->f2;_LL504:;
*_tmp889=Cyc_Toc_rewrite_decision(*_tmp889,success);}
# 5044
return d;}_LL4FB:;}
# 5055 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 5057
struct _RegionHandle _tmp88E=_new_region("rgn");struct _RegionHandle*rgn=& _tmp88E;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp88F=Cyc_Toc_share_env(rgn,nv);
void*_tmp890=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(_tmp88F,e);{
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 5067
struct Cyc_Absyn_Stmt*_tmp891=Cyc_Toc_skip_stmt_dl();
if(dopt != 0)
# 5071
dopt=Cyc_Toc_rewrite_decision(dopt,_tmp891);{
# 5073
struct Cyc_Absyn_Switch_clause*_tmp892=({struct Cyc_Absyn_Switch_clause*_tmp89F=_cycalloc(sizeof(*_tmp89F));_tmp89F->pattern=p;_tmp89F->pat_vars=0;_tmp89F->where_clause=0;_tmp89F->body=_tmp891;_tmp89F->loc=0;_tmp89F;});
struct Cyc_List_List*_tmp893=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple31*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,({struct Cyc_Absyn_Switch_clause*_tmp89E[1];_tmp89E[0]=_tmp892;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(rgn,_tag_dyneither(_tmp89E,sizeof(struct Cyc_Absyn_Switch_clause*),1));}));
# 5076
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
# 5080
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp88F,& mydecls,& mybodies,dopt,_tmp893,v);
# 5083
struct Cyc_Toc_Env*senv;
for(0;mybodies != 0;mybodies=((struct Cyc_List_List*)_check_null(mybodies))->tl){
struct _tuple34*_tmp894=(struct _tuple34*)((struct Cyc_List_List*)_check_null(mybodies))->hd;struct _tuple34*_tmp895=_tmp894;struct Cyc_Toc_Env*_tmp897;struct Cyc_Absyn_Stmt*_tmp896;_LL506: _tmp897=_tmp895->f1;_tmp896=_tmp895->f3;_LL507:;
if(_tmp896 == _tmp891){senv=_tmp897;goto FOUND_ENV;}}
# 5088
({void*_tmp898=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp899="letdecl_to_c: couldn't find env!";_tag_dyneither(_tmp899,sizeof(char),33);}),_tag_dyneither(_tmp898,sizeof(void*),0));});
FOUND_ENV:
# 5092
 Cyc_Toc_stmt_to_c(senv,s);{
# 5094
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(test_tree,s,0);
# 5096
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp89A=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
_tmp89A->name=Cyc_Toc_temp_var();
_tmp89A->type=Cyc_Toc_typ_to_c_array(_tmp89A->type);
res=Cyc_Absyn_decl_stmt(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp89B=_cycalloc(sizeof(*_tmp89B));_tmp89B[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp89C;_tmp89C.tag=0;_tmp89C.f1=_tmp89A;_tmp89C;});_tmp89B;}),0),res,0);}
# 5103
res=Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c(_tmp890),e,res,0);{
struct Cyc_Absyn_Stmt*_tmp89D=res;_npop_handler(0);return _tmp89D;};};};};}
# 5058
;_pop_region(rgn);}
# 5111
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp8A0=e->r;void*_tmp8A1=_tmp8A0;struct Cyc_Absyn_MallocInfo*_tmp8CB;struct Cyc_Absyn_Stmt*_tmp8CA;void**_tmp8C9;void**_tmp8C8;struct Cyc_List_List*_tmp8C7;struct Cyc_List_List*_tmp8C6;void**_tmp8C5;struct Cyc_List_List*_tmp8C4;void**_tmp8C3;struct Cyc_Absyn_Exp*_tmp8C2;struct Cyc_Absyn_Exp*_tmp8C1;struct Cyc_List_List*_tmp8C0;struct Cyc_Absyn_Exp*_tmp8BF;struct Cyc_Absyn_Exp*_tmp8BE;struct Cyc_Absyn_Exp*_tmp8BD;struct Cyc_Absyn_Exp*_tmp8BC;struct Cyc_Absyn_Exp*_tmp8BB;struct Cyc_Absyn_Exp*_tmp8BA;struct Cyc_Absyn_Exp*_tmp8B9;struct Cyc_Absyn_Exp*_tmp8B8;struct Cyc_Absyn_Exp*_tmp8B7;struct Cyc_Absyn_Exp*_tmp8B6;struct Cyc_Absyn_Exp*_tmp8B5;struct Cyc_Absyn_Exp*_tmp8B4;struct Cyc_Absyn_Exp*_tmp8B3;struct Cyc_Absyn_Exp*_tmp8B2;struct Cyc_Absyn_Exp*_tmp8B1;struct Cyc_List_List*_tmp8B0;struct Cyc_Absyn_Exp*_tmp8AF;struct Cyc_Absyn_Exp*_tmp8AE;struct Cyc_Absyn_Exp*_tmp8AD;struct Cyc_Absyn_Exp*_tmp8AC;struct Cyc_Absyn_Exp*_tmp8AB;struct Cyc_Absyn_Exp*_tmp8AA;struct Cyc_Absyn_Exp*_tmp8A9;struct Cyc_Absyn_Exp*_tmp8A8;switch(*((int*)_tmp8A1)){case 19: _LL509: _tmp8A8=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp8A1)->f1;_LL50A:
 _tmp8A9=_tmp8A8;goto _LL50C;case 20: _LL50B: _tmp8A9=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8A1)->f1;_LL50C:
 _tmp8AA=_tmp8A9;goto _LL50E;case 21: _LL50D: _tmp8AA=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp8A1)->f1;_LL50E:
 _tmp8AB=_tmp8AA;goto _LL510;case 14: _LL50F: _tmp8AB=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp8A1)->f1;_LL510:
 _tmp8AC=_tmp8AB;goto _LL512;case 10: _LL511: _tmp8AC=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp8A1)->f1;_LL512:
 _tmp8AD=_tmp8AC;goto _LL514;case 11: _LL513: _tmp8AD=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp8A1)->f1;_LL514:
 _tmp8AE=_tmp8AD;goto _LL516;case 17: _LL515: _tmp8AE=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp8A1)->f1;_LL516:
 _tmp8AF=_tmp8AE;goto _LL518;case 4: _LL517: _tmp8AF=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp8A1)->f1;_LL518:
 Cyc_Toc_exptypes_to_c(_tmp8AF);goto _LL508;case 2: _LL519: _tmp8B0=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp8A1)->f2;_LL51A:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp8B0);goto _LL508;case 6: _LL51B: _tmp8B2=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp8A1)->f1;_tmp8B1=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp8A1)->f2;_LL51C:
 _tmp8B4=_tmp8B2;_tmp8B3=_tmp8B1;goto _LL51E;case 7: _LL51D: _tmp8B4=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp8A1)->f1;_tmp8B3=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp8A1)->f2;_LL51E:
 _tmp8B6=_tmp8B4;_tmp8B5=_tmp8B3;goto _LL520;case 8: _LL51F: _tmp8B6=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp8A1)->f1;_tmp8B5=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp8A1)->f2;_LL520:
 _tmp8B8=_tmp8B6;_tmp8B7=_tmp8B5;goto _LL522;case 22: _LL521: _tmp8B8=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8A1)->f1;_tmp8B7=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8A1)->f2;_LL522:
 _tmp8BA=_tmp8B8;_tmp8B9=_tmp8B7;goto _LL524;case 34: _LL523: _tmp8BA=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp8A1)->f1;_tmp8B9=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp8A1)->f2;_LL524:
 _tmp8BC=_tmp8BA;_tmp8BB=_tmp8B9;goto _LL526;case 3: _LL525: _tmp8BC=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp8A1)->f1;_tmp8BB=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp8A1)->f3;_LL526:
 Cyc_Toc_exptypes_to_c(_tmp8BC);Cyc_Toc_exptypes_to_c(_tmp8BB);goto _LL508;case 5: _LL527: _tmp8BF=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8A1)->f1;_tmp8BE=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8A1)->f2;_tmp8BD=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8A1)->f3;_LL528:
# 5129
 Cyc_Toc_exptypes_to_c(_tmp8BF);Cyc_Toc_exptypes_to_c(_tmp8BE);Cyc_Toc_exptypes_to_c(_tmp8BD);goto _LL508;case 9: _LL529: _tmp8C1=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp8A1)->f1;_tmp8C0=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp8A1)->f2;_LL52A:
# 5131
 Cyc_Toc_exptypes_to_c(_tmp8C1);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp8C0);goto _LL508;case 13: _LL52B: _tmp8C3=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp8A1)->f1;_tmp8C2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp8A1)->f2;_LL52C:
*_tmp8C3=Cyc_Toc_typ_to_c(*_tmp8C3);Cyc_Toc_exptypes_to_c(_tmp8C2);goto _LL508;case 24: _LL52D: _tmp8C5=(void**)&(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp8A1)->f1)->f3;_tmp8C4=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp8A1)->f2;_LL52E:
# 5134
*_tmp8C5=Cyc_Toc_typ_to_c(*_tmp8C5);
_tmp8C6=_tmp8C4;goto _LL530;case 35: _LL52F: _tmp8C6=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp8A1)->f2;_LL530:
 _tmp8C7=_tmp8C6;goto _LL532;case 25: _LL531: _tmp8C7=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp8A1)->f1;_LL532:
# 5138
 for(0;_tmp8C7 != 0;_tmp8C7=_tmp8C7->tl){
struct _tuple19 _tmp8A2=*((struct _tuple19*)_tmp8C7->hd);struct _tuple19 _tmp8A3=_tmp8A2;struct Cyc_Absyn_Exp*_tmp8A4;_LL55A: _tmp8A4=_tmp8A3.f2;_LL55B:;
Cyc_Toc_exptypes_to_c(_tmp8A4);}
# 5142
goto _LL508;case 18: _LL533: _tmp8C8=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp8A1)->f1;_LL534:
 _tmp8C9=_tmp8C8;goto _LL536;case 16: _LL535: _tmp8C9=(void**)&((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp8A1)->f1;_LL536:
*_tmp8C9=Cyc_Toc_typ_to_c(*_tmp8C9);goto _LL508;case 36: _LL537: _tmp8CA=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp8A1)->f1;_LL538:
 Cyc_Toc_stmttypes_to_c(_tmp8CA);goto _LL508;case 33: _LL539: _tmp8CB=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp8A1)->f1;_LL53A:
# 5147
 if(_tmp8CB->elt_type != 0)
_tmp8CB->elt_type=({void**_tmp8A5=_cycalloc(sizeof(*_tmp8A5));_tmp8A5[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp8CB->elt_type)));_tmp8A5;});
Cyc_Toc_exptypes_to_c(_tmp8CB->num_elts);
goto _LL508;case 0: _LL53B: _LL53C:
 goto _LL53E;case 1: _LL53D: _LL53E:
 goto _LL540;case 31: _LL53F: _LL540:
 goto _LL542;case 39: _LL541: _LL542:
 goto _LL544;case 32: _LL543: _LL544:
 goto _LL508;case 29: _LL545: _LL546:
# 5157
 goto _LL548;case 30: _LL547: _LL548:
 goto _LL54A;case 28: _LL549: _LL54A:
 goto _LL54C;case 26: _LL54B: _LL54C:
 goto _LL54E;case 27: _LL54D: _LL54E:
 goto _LL550;case 23: _LL54F: _LL550:
 goto _LL552;case 12: _LL551: _LL552:
 goto _LL554;case 15: _LL553: _LL554:
 goto _LL556;case 38: _LL555: _LL556:
 goto _LL558;default: _LL557: _LL558:
# 5167
({void*_tmp8A6=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp8A7="Cyclone expression within C code";_tag_dyneither(_tmp8A7,sizeof(char),33);}),_tag_dyneither(_tmp8A6,sizeof(void*),0));});
goto _LL508;}_LL508:;}
# 5172
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp8CC=d->r;void*_tmp8CD=_tmp8CC;struct Cyc_Absyn_Typedefdecl*_tmp8D7;struct Cyc_Absyn_Enumdecl*_tmp8D6;struct Cyc_Absyn_Aggrdecl*_tmp8D5;struct Cyc_Absyn_Fndecl*_tmp8D4;struct Cyc_Absyn_Vardecl*_tmp8D3;switch(*((int*)_tmp8CD)){case 0: _LL55D: _tmp8D3=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8CD)->f1;_LL55E:
# 5175
 _tmp8D3->type=Cyc_Toc_typ_to_c(_tmp8D3->type);
if(_tmp8D3->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp8D3->initializer));
goto _LL55C;case 1: _LL55F: _tmp8D4=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp8CD)->f1;_LL560:
# 5179
 _tmp8D4->ret_type=Cyc_Toc_typ_to_c(_tmp8D4->ret_type);
{struct Cyc_List_List*_tmp8CE=_tmp8D4->args;for(0;_tmp8CE != 0;_tmp8CE=_tmp8CE->tl){
(*((struct _tuple9*)_tmp8CE->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp8CE->hd)).f3);}}
# 5183
goto _LL55C;case 5: _LL561: _tmp8D5=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp8CD)->f1;_LL562:
 Cyc_Toc_aggrdecl_to_c(_tmp8D5,1);goto _LL55C;case 7: _LL563: _tmp8D6=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp8CD)->f1;_LL564:
# 5186
 if(_tmp8D6->fields != 0){
struct Cyc_List_List*_tmp8CF=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp8D6->fields))->v;for(0;_tmp8CF != 0;_tmp8CF=_tmp8CF->tl){
struct Cyc_Absyn_Enumfield*_tmp8D0=(struct Cyc_Absyn_Enumfield*)_tmp8CF->hd;
if(_tmp8D0->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp8D0->tag));}}
# 5191
goto _LL55C;case 8: _LL565: _tmp8D7=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp8CD)->f1;_LL566:
 _tmp8D7->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp8D7->defn));goto _LL55C;case 2: _LL567: _LL568:
 goto _LL56A;case 3: _LL569: _LL56A:
 goto _LL56C;case 6: _LL56B: _LL56C:
 goto _LL56E;case 9: _LL56D: _LL56E:
 goto _LL570;case 10: _LL56F: _LL570:
 goto _LL572;case 11: _LL571: _LL572:
 goto _LL574;case 12: _LL573: _LL574:
 goto _LL576;case 4: _LL575: _LL576:
# 5202
({void*_tmp8D1=0;Cyc_Tcutil_terr(d->loc,({const char*_tmp8D2="Cyclone declaration within C code";_tag_dyneither(_tmp8D2,sizeof(char),34);}),_tag_dyneither(_tmp8D1,sizeof(void*),0));});
goto _LL55C;case 13: _LL577: _LL578:
 goto _LL57A;default: _LL579: _LL57A:
# 5206
 goto _LL55C;}_LL55C:;}
# 5210
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp8D8=s->r;void*_tmp8D9=_tmp8D8;struct Cyc_Absyn_Stmt*_tmp8F4;struct Cyc_Absyn_Stmt*_tmp8F3;struct Cyc_Absyn_Exp*_tmp8F2;struct Cyc_Absyn_Decl*_tmp8F1;struct Cyc_Absyn_Stmt*_tmp8F0;struct Cyc_Absyn_Exp*_tmp8EF;struct Cyc_List_List*_tmp8EE;void*_tmp8ED;struct Cyc_Absyn_Exp*_tmp8EC;struct Cyc_Absyn_Exp*_tmp8EB;struct Cyc_Absyn_Exp*_tmp8EA;struct Cyc_Absyn_Stmt*_tmp8E9;struct Cyc_Absyn_Exp*_tmp8E8;struct Cyc_Absyn_Stmt*_tmp8E7;struct Cyc_Absyn_Exp*_tmp8E6;struct Cyc_Absyn_Stmt*_tmp8E5;struct Cyc_Absyn_Stmt*_tmp8E4;struct Cyc_Absyn_Exp*_tmp8E3;struct Cyc_Absyn_Stmt*_tmp8E2;struct Cyc_Absyn_Stmt*_tmp8E1;struct Cyc_Absyn_Exp*_tmp8E0;switch(*((int*)_tmp8D9)){case 1: _LL57C: _tmp8E0=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp8D9)->f1;_LL57D:
 Cyc_Toc_exptypes_to_c(_tmp8E0);goto _LL57B;case 2: _LL57E: _tmp8E2=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8D9)->f1;_tmp8E1=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8D9)->f2;_LL57F:
 Cyc_Toc_stmttypes_to_c(_tmp8E2);Cyc_Toc_stmttypes_to_c(_tmp8E1);goto _LL57B;case 3: _LL580: _tmp8E3=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp8D9)->f1;_LL581:
 if(_tmp8E3 != 0)Cyc_Toc_exptypes_to_c(_tmp8E3);goto _LL57B;case 4: _LL582: _tmp8E6=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8D9)->f1;_tmp8E5=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8D9)->f2;_tmp8E4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp8D9)->f3;_LL583:
# 5216
 Cyc_Toc_exptypes_to_c(_tmp8E6);Cyc_Toc_stmttypes_to_c(_tmp8E5);Cyc_Toc_stmttypes_to_c(_tmp8E4);goto _LL57B;case 5: _LL584: _tmp8E8=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp8D9)->f1).f1;_tmp8E7=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp8D9)->f2;_LL585:
# 5218
 Cyc_Toc_exptypes_to_c(_tmp8E8);Cyc_Toc_stmttypes_to_c(_tmp8E7);goto _LL57B;case 9: _LL586: _tmp8EC=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8D9)->f1;_tmp8EB=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8D9)->f2).f1;_tmp8EA=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8D9)->f3).f1;_tmp8E9=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp8D9)->f4;_LL587:
# 5220
 Cyc_Toc_exptypes_to_c(_tmp8EC);Cyc_Toc_exptypes_to_c(_tmp8EB);Cyc_Toc_exptypes_to_c(_tmp8EA);
Cyc_Toc_stmttypes_to_c(_tmp8E9);goto _LL57B;case 10: _LL588: _tmp8EF=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8D9)->f1;_tmp8EE=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8D9)->f2;_tmp8ED=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp8D9)->f3;_LL589:
# 5223
 Cyc_Toc_exptypes_to_c(_tmp8EF);
for(0;_tmp8EE != 0;_tmp8EE=_tmp8EE->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp8EE->hd)->body);}
goto _LL57B;case 12: _LL58A: _tmp8F1=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8D9)->f1;_tmp8F0=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8D9)->f2;_LL58B:
 Cyc_Toc_decltypes_to_c(_tmp8F1);Cyc_Toc_stmttypes_to_c(_tmp8F0);goto _LL57B;case 14: _LL58C: _tmp8F3=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp8D9)->f1;_tmp8F2=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp8D9)->f2).f1;_LL58D:
 Cyc_Toc_stmttypes_to_c(_tmp8F3);Cyc_Toc_exptypes_to_c(_tmp8F2);goto _LL57B;case 13: _LL58E: _tmp8F4=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp8D9)->f2;_LL58F:
 Cyc_Toc_stmttypes_to_c(_tmp8F4);goto _LL57B;case 0: _LL590: _LL591:
 goto _LL593;case 6: _LL592: _LL593:
 goto _LL595;case 7: _LL594: _LL595:
 goto _LL597;case 8: _LL596: _LL597:
# 5233
 goto _LL57B;case 11: _LL598: _LL599:
# 5236
 s->r=(void*)({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp8DA=_cycalloc_atomic(sizeof(*_tmp8DA));_tmp8DA[0]=({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct _tmp8DB;_tmp8DB.tag=0;_tmp8DB;});_tmp8DA;});
goto _LL57B;case 15: _LL59A: _LL59B:
 goto _LL59D;default: _LL59C: _LL59D:
# 5240
({void*_tmp8DC=0;Cyc_Tcutil_terr(s->loc,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp8DF;_tmp8DF.tag=0;_tmp8DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));({void*_tmp8DD[1]={& _tmp8DF};Cyc_aprintf(({const char*_tmp8DE="Cyclone statement in C code: %s";_tag_dyneither(_tmp8DE,sizeof(char),32);}),_tag_dyneither(_tmp8DD,sizeof(void*),1));});}),_tag_dyneither(_tmp8DC,sizeof(void*),0));});
goto _LL57B;}_LL57B:;}
# 5248
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv))
({void*_tmp8F5=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp8F6="decls_to_c: not at toplevel!";_tag_dyneither(_tmp8F6,sizeof(char),29);}),_tag_dyneither(_tmp8F5,sizeof(void*),0));});{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp8F7=d->r;void*_tmp8F8=_tmp8F7;struct Cyc_List_List*_tmp910;struct Cyc_List_List*_tmp90F;struct Cyc_List_List*_tmp90E;struct Cyc_List_List*_tmp90D;struct Cyc_Absyn_Typedefdecl*_tmp90C;struct Cyc_Absyn_Enumdecl*_tmp90B;struct Cyc_Absyn_Datatypedecl*_tmp90A;struct Cyc_Absyn_Aggrdecl*_tmp909;struct Cyc_Absyn_Fndecl*_tmp908;struct Cyc_Absyn_Vardecl*_tmp907;switch(*((int*)_tmp8F8)){case 0: _LL59F: _tmp907=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8F8)->f1;_LL5A0: {
# 5255
struct _tuple1*_tmp8F9=_tmp907->name;
# 5257
if(_tmp907->sc == Cyc_Absyn_ExternC)
_tmp8F9=({struct _tuple1*_tmp8FA=_cycalloc(sizeof(*_tmp8FA));_tmp8FA->f1=Cyc_Absyn_Abs_n(0,1);_tmp8FA->f2=(*_tmp8F9).f2;_tmp8FA;});
if(_tmp907->initializer != 0){
if(_tmp907->sc == Cyc_Absyn_ExternC)_tmp907->sc=Cyc_Absyn_Public;
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp907->initializer));else{
# 5264
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp907->initializer));}}
# 5266
_tmp907->name=_tmp8F9;
_tmp907->sc=Cyc_Toc_scope_to_c(_tmp907->sc);
_tmp907->type=Cyc_Toc_typ_to_c(_tmp907->type);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp8FB=_cycalloc(sizeof(*_tmp8FB));_tmp8FB->hd=d;_tmp8FB->tl=Cyc_Toc_result_decls;_tmp8FB;});
goto _LL59E;}case 1: _LL5A1: _tmp908=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp8F8)->f1;_LL5A2: {
# 5272
struct _tuple1*_tmp8FC=_tmp908->name;
# 5274
if(_tmp908->sc == Cyc_Absyn_ExternC){
_tmp8FC=({struct _tuple1*_tmp8FD=_cycalloc(sizeof(*_tmp8FD));_tmp8FD->f1=Cyc_Absyn_Abs_n(0,1);_tmp8FD->f2=(*_tmp8FC).f2;_tmp8FD;});
_tmp908->sc=Cyc_Absyn_Public;}
# 5278
_tmp908->name=_tmp8FC;
Cyc_Toc_fndecl_to_c(nv,_tmp908,cinclude);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp8FE=_cycalloc(sizeof(*_tmp8FE));_tmp8FE->hd=d;_tmp8FE->tl=Cyc_Toc_result_decls;_tmp8FE;});
goto _LL59E;}case 2: _LL5A3: _LL5A4:
 goto _LL5A6;case 3: _LL5A5: _LL5A6:
({void*_tmp8FF=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp900="letdecl at toplevel";_tag_dyneither(_tmp900,sizeof(char),20);}),_tag_dyneither(_tmp8FF,sizeof(void*),0));});case 4: _LL5A7: _LL5A8:
({void*_tmp901=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp902="region decl at toplevel";_tag_dyneither(_tmp902,sizeof(char),24);}),_tag_dyneither(_tmp901,sizeof(void*),0));});case 5: _LL5A9: _tmp909=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp8F8)->f1;_LL5AA:
# 5286
 Cyc_Toc_aggrdecl_to_c(_tmp909,1);
goto _LL59E;case 6: _LL5AB: _tmp90A=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp8F8)->f1;_LL5AC:
# 5289
 if(_tmp90A->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp90A);else{
# 5292
Cyc_Toc_datatypedecl_to_c(_tmp90A);}
goto _LL59E;case 7: _LL5AD: _tmp90B=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp8F8)->f1;_LL5AE:
# 5295
 Cyc_Toc_enumdecl_to_c(_tmp90B);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp903=_cycalloc(sizeof(*_tmp903));_tmp903->hd=d;_tmp903->tl=Cyc_Toc_result_decls;_tmp903;});
goto _LL59E;case 8: _LL5AF: _tmp90C=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp8F8)->f1;_LL5B0:
# 5299
 _tmp90C->tvs=0;
if(_tmp90C->defn != 0)
_tmp90C->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp90C->defn));else{
# 5303
enum Cyc_Absyn_KindQual _tmp904=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp90C->kind))->v)->kind;enum Cyc_Absyn_KindQual _tmp905=_tmp904;if(_tmp905 == Cyc_Absyn_BoxKind){_LL5BE: _LL5BF:
 _tmp90C->defn=Cyc_Absyn_void_star_typ();goto _LL5BD;}else{_LL5C0: _LL5C1:
 _tmp90C->defn=(void*)& Cyc_Absyn_VoidType_val;goto _LL5BD;}_LL5BD:;}
# 5308
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp906=_cycalloc(sizeof(*_tmp906));_tmp906->hd=d;_tmp906->tl=Cyc_Toc_result_decls;_tmp906;});
goto _LL59E;case 13: _LL5B1: _LL5B2:
 goto _LL5B4;case 14: _LL5B3: _LL5B4:
# 5312
 goto _LL59E;case 9: _LL5B5: _tmp90D=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp8F8)->f2;_LL5B6:
 _tmp90E=_tmp90D;goto _LL5B8;case 10: _LL5B7: _tmp90E=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp8F8)->f2;_LL5B8:
 _tmp90F=_tmp90E;goto _LL5BA;case 11: _LL5B9: _tmp90F=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp8F8)->f1;_LL5BA:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp90F,top,cinclude);goto _LL59E;default: _LL5BB: _tmp910=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp8F8)->f1;_LL5BC:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp910,top,1);goto _LL59E;}_LL59E:;};}
# 5319
return nv;}
# 5323
static void Cyc_Toc_init(){
struct Cyc_Core_NewDynamicRegion _tmp911=Cyc_Core_new_rckey();struct Cyc_Core_NewDynamicRegion _tmp912=_tmp911;struct Cyc_Core_DynamicRegion*_tmp916;_LL5C3: _tmp916=_tmp912.key;_LL5C4:;{
struct Cyc_Toc_TocState*_tmp913=((struct Cyc_Toc_TocState*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Toc_TocState*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmp916,0,Cyc_Toc_empty_toc_state);
Cyc_Toc_toc_state=({struct Cyc_Toc_TocStateWrap*_tmp914=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp914));_tmp914->dyn=(struct Cyc_Core_DynamicRegion*)_tmp916;_tmp914->state=(struct Cyc_Toc_TocState*)_tmp913;_tmp914;});
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0;
Cyc_Toc_bounds_checks_eliminated=0;
Cyc_Toc_globals=_tag_dyneither(({struct _dyneither_ptr**_tmp915=_cycalloc(sizeof(struct _dyneither_ptr*)* 42);_tmp915[0]=(struct _dyneither_ptr*)& Cyc_Toc__throw_str;_tmp915[1]=(struct _dyneither_ptr*)& Cyc_Toc_setjmp_str;_tmp915[2]=(struct _dyneither_ptr*)& Cyc_Toc__push_handler_str;_tmp915[3]=(struct _dyneither_ptr*)& Cyc_Toc__pop_handler_str;_tmp915[4]=(struct _dyneither_ptr*)& Cyc_Toc__exn_thrown_str;_tmp915[5]=(struct _dyneither_ptr*)& Cyc_Toc__npop_handler_str;_tmp915[6]=(struct _dyneither_ptr*)& Cyc_Toc__check_null_str;_tmp915[7]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_null_str;_tmp915[8]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_notnull_str;_tmp915[9]=(struct _dyneither_ptr*)& Cyc_Toc__check_dyneither_subscript_str;_tmp915[10]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_str;_tmp915[11]=(struct _dyneither_ptr*)& Cyc_Toc__tag_dyneither_str;_tmp915[12]=(struct _dyneither_ptr*)& Cyc_Toc__init_dyneither_ptr_str;_tmp915[13]=(struct _dyneither_ptr*)& Cyc_Toc__untag_dyneither_ptr_str;_tmp915[14]=(struct _dyneither_ptr*)& Cyc_Toc__get_dyneither_size_str;_tmp915[15]=(struct _dyneither_ptr*)& Cyc_Toc__get_zero_arr_size_str;_tmp915[16]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_plus_str;_tmp915[17]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_plus_str;_tmp915[18]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_str;_tmp915[19]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_str;_tmp915[20]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_str;_tmp915[21]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_post_str;_tmp915[22]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_str;_tmp915[23]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_str;_tmp915[24]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_atomic_str;_tmp915[25]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_atomic_str;_tmp915[26]=(struct _dyneither_ptr*)& Cyc_Toc__region_malloc_str;_tmp915[27]=(struct _dyneither_ptr*)& Cyc_Toc__region_calloc_str;_tmp915[28]=(struct _dyneither_ptr*)& Cyc_Toc__check_times_str;_tmp915[29]=(struct _dyneither_ptr*)& Cyc_Toc__new_region_str;_tmp915[30]=(struct _dyneither_ptr*)& Cyc_Toc__push_region_str;_tmp915[31]=(struct _dyneither_ptr*)& Cyc_Toc__pop_region_str;_tmp915[32]=(struct _dyneither_ptr*)& Cyc_Toc__open_dynregion_str;_tmp915[33]=(struct _dyneither_ptr*)& Cyc_Toc__push_dynregion_str;_tmp915[34]=(struct _dyneither_ptr*)& Cyc_Toc__pop_dynregion_str;_tmp915[35]=(struct _dyneither_ptr*)& Cyc_Toc__reset_region_str;_tmp915[36]=(struct _dyneither_ptr*)& Cyc_Toc__throw_arraybounds_str;_tmp915[37]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_decrease_size_str;_tmp915[38]=(struct _dyneither_ptr*)& Cyc_Toc__throw_match_str;_tmp915[39]=(struct _dyneither_ptr*)& Cyc_Toc__swap_word_str;_tmp915[40]=(struct _dyneither_ptr*)& Cyc_Toc__swap_dyneither_str;_tmp915[41]=(struct _dyneither_ptr*)& Cyc_Toc__fast_region_malloc_str;_tmp915;}),sizeof(struct _dyneither_ptr*),42);};}
# 5380
static int Cyc_Toc_destroy_labels(struct _RegionHandle*d,struct Cyc_Toc_TocState*ts){
struct Cyc_Toc_TocState _tmp917=*ts;struct Cyc_Toc_TocState _tmp918=_tmp917;struct Cyc_Xarray_Xarray*_tmp919;_LL5C6: _tmp919=_tmp918.temp_labels;_LL5C7:;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp919);
return 0;}
# 5388
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds){
# 5390
Cyc_Toc_gpop_tables=({struct Cyc_Hashtable_Table**_tmp91A=_cycalloc(sizeof(*_tmp91A));_tmp91A[0]=pop_tables;_tmp91A;});
Cyc_Toc_init();
{struct _RegionHandle _tmp91B=_new_region("start");struct _RegionHandle*start=& _tmp91B;_push_region(start);
Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(start),ds,1,0);;_pop_region(start);}{
# 5400
struct Cyc_Toc_TocStateWrap*ts=0;
_swap_word(& ts,& Cyc_Toc_toc_state);{
struct Cyc_Toc_TocStateWrap _tmp91C=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp91D=_tmp91C;struct Cyc_Core_DynamicRegion*_tmp91F;struct Cyc_Toc_TocState*_tmp91E;_LL5C9: _tmp91F=_tmp91D.dyn;_tmp91E=_tmp91D.state;_LL5CA:;
((int(*)(struct Cyc_Core_DynamicRegion*key,struct Cyc_Toc_TocState*arg,int(*body)(struct _RegionHandle*h,struct Cyc_Toc_TocState*arg)))Cyc_Core_open_region)(_tmp91F,_tmp91E,Cyc_Toc_destroy_labels);
Cyc_Core_free_rckey(_tmp91F);
((void(*)(struct _dyneither_ptr ptr))Cyc_Core_ufree)(_tag_dyneither(ts,sizeof(struct Cyc_Toc_TocStateWrap),1));
# 5407
Cyc_Toc_gpop_tables=0;
Cyc_Toc_fn_pop_table=0;
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);};};}
