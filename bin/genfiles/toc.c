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
# 73 "absyn.h"
typedef struct Cyc_Relations_Reln*Cyc_Relations_reln_t;
typedef struct Cyc_List_List*Cyc_Relations_relns_t;
# 79
typedef void*Cyc_Tcpat_decision_opt_t;
# 87
typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_t;
typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 99
typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 103
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 106
typedef struct _tuple1*Cyc_Absyn_qvar_t;typedef struct _tuple1*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_opt_t;
# 111
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
# 162
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int real_const;unsigned int loc;};
# 183
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz  = 4};
# 188
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0,Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top  = 2};
# 195
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_EffKind  = 4,Cyc_Absyn_IntKind  = 5};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 215
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None  = 2};
# 217
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0,Cyc_Absyn_UnionA  = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};
# 226
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 252
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{int tag;};
# 287
typedef void*Cyc_Absyn_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 366
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 386
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 391
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 446 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
# 449
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 462
typedef void*Cyc_Absyn_funcparams_t;
# 465
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 529
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 545
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 547
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign,long long);
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr,int);
# 555
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 562
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 580
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 711 "absyn.h"
extern struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val;struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 743
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
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
# 1043
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
# 1046
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned int);
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
# 1204
struct _tuple1*Cyc_Absyn_binding2qvar(void*);
# 1209
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
typedef void*Cyc_Tcenv_resolved_t;struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};
# 73
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 77
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 88
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 124 "tcenv.h"
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
struct _tuple14 _tmp0=*x;struct _tuple14 _tmp1=_tmp0;struct _tuple1*_tmp2;struct _tuple1*_tmp3;_LL1: _tmp2=_tmp1.f1;_tmp3=_tmp1.f2;_LL2:;{
struct _tuple14 _tmp4=*y;struct _tuple14 _tmp5=_tmp4;struct _tuple1*_tmp6;struct _tuple1*_tmp7;_LL4: _tmp6=_tmp5.f1;_tmp7=_tmp5.f2;_LL5:;{
int i=Cyc_Absyn_qvar_cmp(_tmp2,_tmp6);
if(i != 0)return i;
return Cyc_Absyn_qvar_cmp(_tmp3,_tmp7);};};}
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
struct Cyc_Toc_TocStateWrap _tmpF=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp10=_tmpF;struct Cyc_Core_DynamicRegion*_tmp11;struct Cyc_Toc_TocState*_tmp12;_LL7: _tmp11=_tmp10.dyn;_tmp12=_tmp10.state;_LL8:;{
struct _dyneither_ptr _tmp13=((struct _dyneither_ptr(*)(struct _dyneither_ptr ptr))Cyc_Core_alias_refptr)(_tag_dyneither(_tmp11,sizeof(struct Cyc_Core_DynamicRegion),1));
*ts=({struct Cyc_Toc_TocStateWrap _tmp14;_tmp14.dyn=(struct Cyc_Core_DynamicRegion*)_tmp11;_tmp14.state=(struct Cyc_Toc_TocState*)_tmp12;_tmp14;});
_swap_word(& ts,& Cyc_Toc_toc_state);{
struct _tuple15 _tmp15=({struct _tuple15 _tmp17;_tmp17.f1=_tmp12;_tmp17.f2=arg;_tmp17;});
void*_tmp16=((void*(*)(struct Cyc_Core_DynamicRegion*key,struct _tuple15*arg,void*(*body)(struct _RegionHandle*h,struct _tuple15*arg)))Cyc_Core_open_region)((struct Cyc_Core_DynamicRegion*)_untag_dyneither_ptr(_tmp13,sizeof(struct Cyc_Core_DynamicRegion),1),& _tmp15,f);
Cyc_Core_free_rckey((struct Cyc_Core_DynamicRegion*)_untag_dyneither_ptr(_tmp13,sizeof(struct Cyc_Core_DynamicRegion),1));
return _tmp16;};};};}struct _tuple16{struct _tuple1*f1;void*(*f2)(struct _tuple1*);};struct _tuple17{struct Cyc_Toc_TocState*f1;struct _tuple16*f2;};struct _tuple18{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
# 161
static void*Cyc_Toc_aggrdecl_type_body(struct _RegionHandle*d,struct _tuple17*env){
# 164
struct _tuple17 _tmp18=*env;struct _tuple17 _tmp19=_tmp18;struct Cyc_Toc_TocState*_tmp1A;struct _tuple1*_tmp1B;void*(*_tmp1C)(struct _tuple1*);_LLA: _tmp1A=_tmp19.f1;_tmp1B=(_tmp19.f2)->f1;_tmp1C=(_tmp19.f2)->f2;_LLB:;{
struct _tuple18**v=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp1A->aggrs_so_far,_tmp1B);
if(v == 0)
return _tmp1C(_tmp1B);else{
# 169
struct _tuple18*_tmp1D=*v;struct _tuple18*_tmp1E=_tmp1D;void*_tmp1F;_LLD: _tmp1F=_tmp1E->f2;_LLE:;
return _tmp1F;}};}
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
{void*_tmp128=Cyc_Tcutil_compress(t);void*_tmp129=_tmp128;enum Cyc_Absyn_Size_of _tmp12A;switch(*((int*)_tmp129)){case 6: _LL1A: _tmp12A=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp129)->f2;_LL1B:
# 432
{enum Cyc_Absyn_Size_of _tmp12B=_tmp12A;switch(_tmp12B){case Cyc_Absyn_Char_sz: _LL27: _LL28:
 function=fS->fchar;goto _LL26;case Cyc_Absyn_Short_sz: _LL29: _LL2A:
 function=fS->fshort;goto _LL26;case Cyc_Absyn_Int_sz: _LL2B: _LL2C:
 function=fS->fint;goto _LL26;default: _LL2D: _LL2E:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp12C=_cycalloc(sizeof(*_tmp12C));_tmp12C[0]=({struct Cyc_Core_Impossible_exn_struct _tmp12D;_tmp12D.tag=Cyc_Core_Impossible;_tmp12D.f1=({const char*_tmp12E="impossible IntType (not char, short or int)";_tag_dyneither(_tmp12E,sizeof(char),44);});_tmp12D;});_tmp12C;}));}_LL26:;}
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
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp12F=_cycalloc(sizeof(*_tmp12F));_tmp12F[0]=({struct Cyc_Core_Impossible_exn_struct _tmp130;_tmp130.tag=Cyc_Core_Impossible;_tmp130.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp133;_tmp133.tag=0;_tmp133.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp131[1]={& _tmp133};Cyc_aprintf(({const char*_tmp132="impossible expression type %s (not int, float, double, or pointer)";_tag_dyneither(_tmp132,sizeof(char),67);}),_tag_dyneither(_tmp131,sizeof(void*),1));});});_tmp130;});_tmp12F;}));}_LL19:;}
# 454
return function;}
# 456
struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
return Cyc_Toc_getFunctionType(fS,(void*)_check_null(arr->topt));}
# 459
struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmp134=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_tmp135=_tmp134;void*_tmp136;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp135)->tag == 5){_LL30: _tmp136=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp135)->f1).elt_typ;_LL31:
# 462
 return Cyc_Toc_getFunctionType(fS,_tmp136);}else{_LL32: _LL33:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp137=_cycalloc(sizeof(*_tmp137));_tmp137[0]=({struct Cyc_Core_Impossible_exn_struct _tmp138;_tmp138.tag=Cyc_Core_Impossible;_tmp138.f1=({const char*_tmp139="impossible type (not pointer)";_tag_dyneither(_tmp139,sizeof(char),30);});_tmp138;});_tmp137;}));}_LL2F:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 470
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp13A=e->r;void*_tmp13B=_tmp13A;struct Cyc_List_List*_tmp13C;struct Cyc_List_List*_tmp13D;struct Cyc_List_List*_tmp13E;struct Cyc_List_List*_tmp13F;struct Cyc_List_List*_tmp140;struct Cyc_Absyn_Exp*_tmp141;long long _tmp142;int _tmp143;short _tmp144;struct _dyneither_ptr _tmp145;char _tmp146;switch(*((int*)_tmp13B)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp13B)->f1).Null_c).tag){case 2: _LL35: _tmp146=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp13B)->f1).Char_c).val).f2;_LL36:
 return _tmp146 == '\000';case 3: _LL37: _tmp145=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp13B)->f1).Wchar_c).val;_LL38: {
# 474
unsigned long _tmp147=Cyc_strlen((struct _dyneither_ptr)_tmp145);
int i=0;
if(_tmp147 >= 2  && *((const char*)_check_dyneither_subscript(_tmp145,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp145,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmp145,sizeof(char),1))== 'x'  && _tmp147 >= 3) && *((const char*)_check_dyneither_subscript(_tmp145,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp147;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp145,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 484
return 0;}}case 4: _LL39: _tmp144=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp13B)->f1).Short_c).val).f2;_LL3A:
 return _tmp144 == 0;case 5: _LL3B: _tmp143=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp13B)->f1).Int_c).val).f2;_LL3C:
 return _tmp143 == 0;case 6: _LL3D: _tmp142=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp13B)->f1).LongLong_c).val).f2;_LL3E:
 return _tmp142 == 0;case 1: _LL3F: _LL40:
 return 1;default: goto _LL4D;}case 13: _LL41: _tmp141=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp13B)->f2;_LL42:
 return Cyc_Toc_is_zero(_tmp141);case 23: _LL43: _tmp140=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp13B)->f1;_LL44:
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmp140);case 25: _LL45: _tmp13F=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp13B)->f1;_LL46:
 _tmp13E=_tmp13F;goto _LL48;case 28: _LL47: _tmp13E=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp13B)->f3;_LL48:
 _tmp13D=_tmp13E;goto _LL4A;case 24: _LL49: _tmp13D=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp13B)->f2;_LL4A:
 _tmp13C=_tmp13D;goto _LL4C;case 35: _LL4B: _tmp13C=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp13B)->f2;_LL4C:
# 495
 for(0;_tmp13C != 0;_tmp13C=_tmp13C->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple19*)_tmp13C->hd)).f2))return 0;}
return 1;default: _LL4D: _LL4E:
 return 0;}_LL34:;}
# 503
static int Cyc_Toc_is_nullable(void*t){
void*_tmp148=Cyc_Tcutil_compress(t);void*_tmp149=_tmp148;union Cyc_Absyn_Constraint*_tmp14A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp149)->tag == 5){_LL50: _tmp14A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp149)->f1).ptr_atts).nullable;_LL51:
# 506
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp14A);}else{_LL52: _LL53:
({void*_tmp14B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp14C="is_nullable";_tag_dyneither(_tmp14C,sizeof(char),12);}),_tag_dyneither(_tmp14B,sizeof(void*),0));});}_LL4F:;}
# 512
static struct _dyneither_ptr Cyc_Toc_collapse_qvar(struct _dyneither_ptr s,struct _tuple1*x){
unsigned int slen=Cyc_strlen((struct _dyneither_ptr)s);
unsigned int len=9 + slen;
struct _tuple1*_tmp14D=x;union Cyc_Absyn_Nmspace _tmp14E;struct _dyneither_ptr _tmp14F;_LL55: _tmp14E=_tmp14D->f1;_tmp14F=*_tmp14D->f2;_LL56:;{
unsigned int vlen=Cyc_strlen((struct _dyneither_ptr)_tmp14F);
len +=1 + vlen;{
struct Cyc_List_List*nms;
{union Cyc_Absyn_Nmspace _tmp150=_tmp14E;struct Cyc_List_List*_tmp151;struct Cyc_List_List*_tmp152;struct Cyc_List_List*_tmp153;switch((_tmp150.Abs_n).tag){case 4: _LL58: _LL59:
 nms=0;goto _LL57;case 1: _LL5A: _tmp153=(_tmp150.Rel_n).val;_LL5B:
 nms=_tmp153;goto _LL57;case 2: _LL5C: _tmp152=(_tmp150.Abs_n).val;_LL5D:
 nms=_tmp152;goto _LL57;default: _LL5E: _tmp151=(_tmp150.C_n).val;_LL5F:
 nms=_tmp151;goto _LL57;}_LL57:;}
# 525
{struct Cyc_List_List*_tmp154=nms;for(0;_tmp154 != 0;_tmp154=_tmp154->tl){
len +=1 + Cyc_strlen((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp154->hd));}}{
struct _dyneither_ptr buf=({unsigned int _tmp15F=len;char*_tmp160=_cyccalloc_atomic(sizeof(char),_tmp15F);_tag_dyneither(_tmp160,sizeof(char),_tmp15F);});
struct _dyneither_ptr p=buf;
Cyc_strcpy(p,(struct _dyneither_ptr)s);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)slen);
for(0;nms != 0;nms=nms->tl){
struct _dyneither_ptr*_tmp155=(struct _dyneither_ptr*)nms->hd;struct _dyneither_ptr*_tmp156=_tmp155;struct _dyneither_ptr _tmp157;_LL61: _tmp157=*_tmp156;_LL62:;
({struct _dyneither_ptr _tmp158=p;char _tmp159=*((char*)_check_dyneither_subscript(_tmp158,sizeof(char),0));char _tmp15A='_';if(_get_dyneither_size(_tmp158,sizeof(char))== 1  && (_tmp159 == '\000'  && _tmp15A != '\000'))_throw_arraybounds();*((char*)_tmp158.curr)=_tmp15A;});_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp157);
_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)Cyc_strlen((struct _dyneither_ptr)_tmp157));}
# 536
({struct _dyneither_ptr _tmp15B=p;char _tmp15C=*((char*)_check_dyneither_subscript(_tmp15B,sizeof(char),0));char _tmp15D='_';if(_get_dyneither_size(_tmp15B,sizeof(char))== 1  && (_tmp15C == '\000'  && _tmp15D != '\000'))_throw_arraybounds();*((char*)_tmp15B.curr)=_tmp15D;});_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp14F);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)vlen);
Cyc_strcpy(p,({const char*_tmp15E="_struct";_tag_dyneither(_tmp15E,sizeof(char),8);}));
return(struct _dyneither_ptr)buf;};};};}struct _tuple20{struct Cyc_Toc_TocState*f1;struct _tuple14*f2;};static char _tmp176[8]="*bogus*";
# 548
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple20*env){
# 551
static struct _dyneither_ptr bogus_string={_tmp176,_tmp176,_tmp176 + 8};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
static struct _tuple14 pair={& bogus_qvar,& bogus_qvar};
# 555
struct _tuple20 _tmp161=*env;struct _tuple20 _tmp162=_tmp161;struct Cyc_Dict_Dict*_tmp163;struct _tuple14*_tmp164;_LL64: _tmp163=(_tmp162.f1)->qvar_tags;_tmp164=_tmp162.f2;_LL65:;{
struct _tuple14 _tmp165=*_tmp164;struct _tuple14 _tmp166=_tmp165;struct _tuple1*_tmp167;struct _tuple1*_tmp168;_LL67: _tmp167=_tmp166.f1;_tmp168=_tmp166.f2;_LL68:;{
struct _handler_cons _tmp169;_push_handler(& _tmp169);{int _tmp16B=0;if(setjmp(_tmp169.handler))_tmp16B=1;if(!_tmp16B){
{struct _tuple1*_tmp16C=((struct _tuple1*(*)(struct Cyc_Dict_Dict d,int(*cmp)(struct _tuple14*,struct _tuple14*),struct _tuple14*k))Cyc_Dict_lookup_other)(*_tmp163,Cyc_Toc_qvar_tag_cmp,_tmp164);_npop_handler(0);return _tmp16C;};_pop_handler();}else{void*_tmp16A=(void*)_exn_thrown;void*_tmp16D=_tmp16A;void*_tmp16E;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp16D)->tag == Cyc_Dict_Absent){_LL6A: _LL6B: {
# 561
struct _tuple14*_tmp16F=({struct _tuple14*_tmp175=_cycalloc(sizeof(*_tmp175));_tmp175->f1=_tmp167;_tmp175->f2=_tmp168;_tmp175;});
struct _tuple1*_tmp170=_tmp167;union Cyc_Absyn_Nmspace _tmp171;struct _dyneither_ptr _tmp172;_LL6F: _tmp171=_tmp170->f1;_tmp172=*_tmp170->f2;_LL70:;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmp172,_tmp168);
struct _tuple1*res=({struct _tuple1*_tmp173=_cycalloc(sizeof(*_tmp173));_tmp173->f1=_tmp171;_tmp173->f2=({struct _dyneither_ptr*_tmp174=_cycalloc(sizeof(*_tmp174));_tmp174[0]=newvar;_tmp174;});_tmp173;});
*_tmp163=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmp163,_tmp16F,res);
return res;};}}else{_LL6C: _tmp16E=_tmp16D;_LL6D:(int)_rethrow(_tmp16E);}_LL69:;}};};};}
# 570
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple14 env=({struct _tuple14 _tmp177;_tmp177.f1=fieldname;_tmp177.f2=dtname;_tmp177;});
return((struct _tuple1*(*)(struct _tuple14*arg,struct _tuple1*(*f)(struct _RegionHandle*,struct _tuple20*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_collapse_qvars_body);}
# 575
static void*Cyc_Toc_typ_to_c(void*t);struct _tuple21{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple22{void*f1;struct Cyc_List_List*f2;};
# 579
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple21*env){
# 582
struct _tuple21 _tmp178=*env;struct _tuple21 _tmp179=_tmp178;struct Cyc_List_List**_tmp17A;struct Cyc_List_List*_tmp17B;_LL72: _tmp17A=(_tmp179.f1)->tuple_types;_tmp17B=_tmp179.f2;_LL73:;
# 584
{struct Cyc_List_List*_tmp17C=*_tmp17A;for(0;_tmp17C != 0;_tmp17C=_tmp17C->tl){
struct _tuple22*_tmp17D=(struct _tuple22*)_tmp17C->hd;struct _tuple22*_tmp17E=_tmp17D;void*_tmp17F;struct Cyc_List_List*_tmp180;_LL75: _tmp17F=_tmp17E->f1;_tmp180=_tmp17E->f2;_LL76:;{
struct Cyc_List_List*_tmp181=_tmp17B;
for(0;_tmp181 != 0  && _tmp180 != 0;(_tmp181=_tmp181->tl,_tmp180=_tmp180->tl)){
if(!Cyc_Tcutil_unify((*((struct _tuple11*)_tmp181->hd)).f2,(void*)_tmp180->hd))
break;}
if(_tmp181 == 0  && _tmp180 == 0)
return _tmp17F;};}}{
# 594
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmp18E=_cycalloc(sizeof(*_tmp18E));_tmp18E[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp191;_tmp191.tag=1;_tmp191.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp18F[1]={& _tmp191};Cyc_aprintf(({const char*_tmp190="_tuple%d";_tag_dyneither(_tmp190,sizeof(char),9);}),_tag_dyneither(_tmp18F,sizeof(void*),1));});});_tmp18E;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,_tmp17B);
struct Cyc_List_List*_tmp182=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
_tmp182=({struct Cyc_List_List*_tmp183=_cycalloc(sizeof(*_tmp183));_tmp183->hd=({struct Cyc_Absyn_Aggrfield*_tmp184=_cycalloc(sizeof(*_tmp184));_tmp184->name=Cyc_Absyn_fieldname(i);_tmp184->tq=Cyc_Toc_mt_tq;_tmp184->type=(void*)ts2->hd;_tmp184->width=0;_tmp184->attributes=0;_tmp184->requires_clause=0;_tmp184;});_tmp183->tl=_tmp182;_tmp183;});}}
_tmp182=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp182);{
struct Cyc_Absyn_Aggrdecl*_tmp185=({struct Cyc_Absyn_Aggrdecl*_tmp18B=_cycalloc(sizeof(*_tmp18B));_tmp18B->kind=Cyc_Absyn_StructA;_tmp18B->sc=Cyc_Absyn_Public;_tmp18B->name=({struct _tuple1*_tmp18D=_cycalloc(sizeof(*_tmp18D));_tmp18D->f1=
Cyc_Absyn_Rel_n(0);_tmp18D->f2=xname;_tmp18D;});_tmp18B->tvs=0;_tmp18B->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp18C=_cycalloc(sizeof(*_tmp18C));_tmp18C->exist_vars=0;_tmp18C->rgn_po=0;_tmp18C->fields=_tmp182;_tmp18C->tagged=0;_tmp18C;});_tmp18B->attributes=0;_tmp18B->expected_mem_kind=0;_tmp18B;});
# 609
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp186=_cycalloc(sizeof(*_tmp186));_tmp186->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp187=_cycalloc(sizeof(*_tmp187));_tmp187[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp188;_tmp188.tag=5;_tmp188.f1=_tmp185;_tmp188;});_tmp187;}),0);_tmp186->tl=Cyc_Toc_result_decls;_tmp186;});
*_tmp17A=({struct Cyc_List_List*_tmp189=_region_malloc(d,sizeof(*_tmp189));_tmp189->hd=({struct _tuple22*_tmp18A=_region_malloc(d,sizeof(*_tmp18A));_tmp18A->f1=x;_tmp18A->f2=ts;_tmp18A;});_tmp189->tl=*_tmp17A;_tmp189;});
return x;};};}
# 614
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return((void*(*)(struct Cyc_List_List*arg,void*(*f)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state)(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple23{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple24{struct Cyc_Toc_TocState*f1;struct _tuple23*f2;};struct _tuple25{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 623
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple24*env){
# 631
struct _tuple24 _tmp192=*env;struct _tuple24 _tmp193=_tmp192;struct Cyc_List_List**_tmp194;struct _tuple1*_tmp195;struct Cyc_List_List*_tmp196;struct Cyc_List_List*_tmp197;struct Cyc_List_List*_tmp198;_LL78: _tmp194=(_tmp193.f1)->abs_struct_types;_tmp195=(_tmp193.f2)->f1;_tmp196=(_tmp193.f2)->f2;_tmp197=(_tmp193.f2)->f3;_tmp198=(_tmp193.f2)->f4;_LL79:;
# 636
{struct Cyc_List_List*_tmp199=*_tmp194;for(0;_tmp199 != 0;_tmp199=_tmp199->tl){
struct _tuple25*_tmp19A=(struct _tuple25*)_tmp199->hd;struct _tuple25*_tmp19B=_tmp19A;struct _tuple1*_tmp19C;struct Cyc_List_List*_tmp19D;void*_tmp19E;_LL7B: _tmp19C=_tmp19B->f1;_tmp19D=_tmp19B->f2;_tmp19E=_tmp19B->f3;_LL7C:;
# 639
if(Cyc_Absyn_qvar_cmp(_tmp19C,_tmp195)== 0  && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp197)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp19D)){
int okay=1;
{struct Cyc_List_List*_tmp19F=_tmp197;for(0;_tmp19F != 0;(_tmp19F=_tmp19F->tl,_tmp19D=_tmp19D->tl)){
void*_tmp1A0=(void*)_tmp19F->hd;
void*_tmp1A1=(void*)((struct Cyc_List_List*)_check_null(_tmp19D))->hd;
# 646
{struct Cyc_Absyn_Kind*_tmp1A2=Cyc_Tcutil_typ_kind(_tmp1A0);struct Cyc_Absyn_Kind*_tmp1A3=_tmp1A2;switch(((struct Cyc_Absyn_Kind*)_tmp1A3)->kind){case Cyc_Absyn_EffKind: _LL7E: _LL7F:
 goto _LL81;case Cyc_Absyn_RgnKind: _LL80: _LL81:
# 651
 continue;default: _LL82: _LL83:
# 654
 if(Cyc_Tcutil_unify(_tmp1A0,_tmp1A1) || Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(_tmp1A0),Cyc_Toc_typ_to_c(_tmp1A1)))
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
return _tmp19E;}}}{
# 674
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmp1BA=_cycalloc(sizeof(*_tmp1BA));_tmp1BA[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1BD;_tmp1BD.tag=1;_tmp1BD.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp1BB[1]={& _tmp1BD};Cyc_aprintf(({const char*_tmp1BC="_tuple%d";_tag_dyneither(_tmp1BC,sizeof(char),9);}),_tag_dyneither(_tmp1BB,sizeof(void*),1));});});_tmp1BA;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp1A4=0;
# 678
*_tmp194=({struct Cyc_List_List*_tmp1A5=_region_malloc(d,sizeof(*_tmp1A5));_tmp1A5->hd=({struct _tuple25*_tmp1A6=_region_malloc(d,sizeof(*_tmp1A6));_tmp1A6->f1=_tmp195;_tmp1A6->f2=_tmp197;_tmp1A6->f3=x;_tmp1A6;});_tmp1A5->tl=*_tmp194;_tmp1A5;});{
# 681
struct _RegionHandle _tmp1A7=_new_region("r");struct _RegionHandle*r=& _tmp1A7;_push_region(r);
{struct Cyc_List_List*_tmp1A8=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp196,_tmp197);
for(0;_tmp198 != 0;_tmp198=_tmp198->tl){
struct Cyc_Absyn_Aggrfield*_tmp1A9=(struct Cyc_Absyn_Aggrfield*)_tmp198->hd;
void*t=_tmp1A9->type;
struct Cyc_List_List*atts=_tmp1A9->attributes;
# 689
if(_tmp198->tl == 0  && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(t)))
atts=({struct Cyc_List_List*_tmp1AA=_cycalloc(sizeof(*_tmp1AA));_tmp1AA->hd=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp1AB=_cycalloc(sizeof(*_tmp1AB));_tmp1AB[0]=({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmp1AC;_tmp1AC.tag=6;_tmp1AC.f1=0;_tmp1AC;});_tmp1AB;});_tmp1AA->tl=atts;_tmp1AA;});
# 692
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp1A8,t));
# 695
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val))
t=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1AD=_cycalloc(sizeof(*_tmp1AD));_tmp1AD[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp1AE;_tmp1AE.tag=8;_tmp1AE.f1=({struct Cyc_Absyn_ArrayInfo _tmp1AF;_tmp1AF.elt_type=Cyc_Absyn_void_star_typ();_tmp1AF.tq=Cyc_Absyn_empty_tqual(0);_tmp1AF.num_elts=
Cyc_Absyn_uint_exp(0,0);_tmp1AF.zero_term=((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0);_tmp1AF.zt_loc=0;_tmp1AF;});_tmp1AE;});_tmp1AD;});
# 699
_tmp1A4=({struct Cyc_List_List*_tmp1B0=_cycalloc(sizeof(*_tmp1B0));_tmp1B0->hd=({struct Cyc_Absyn_Aggrfield*_tmp1B1=_cycalloc(sizeof(*_tmp1B1));_tmp1B1->name=_tmp1A9->name;_tmp1B1->tq=Cyc_Toc_mt_tq;_tmp1B1->type=t;_tmp1B1->width=_tmp1A9->width;_tmp1B1->attributes=atts;_tmp1B1->requires_clause=0;_tmp1B1;});_tmp1B0->tl=_tmp1A4;_tmp1B0;});}
# 701
_tmp1A4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1A4);{
struct Cyc_Absyn_Aggrdecl*_tmp1B2=({struct Cyc_Absyn_Aggrdecl*_tmp1B7=_cycalloc(sizeof(*_tmp1B7));_tmp1B7->kind=Cyc_Absyn_StructA;_tmp1B7->sc=Cyc_Absyn_Public;_tmp1B7->name=({struct _tuple1*_tmp1B9=_cycalloc(sizeof(*_tmp1B9));_tmp1B9->f1=
Cyc_Absyn_Rel_n(0);_tmp1B9->f2=xname;_tmp1B9;});_tmp1B7->tvs=0;_tmp1B7->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp1B8=_cycalloc(sizeof(*_tmp1B8));_tmp1B8->exist_vars=0;_tmp1B8->rgn_po=0;_tmp1B8->fields=_tmp1A4;_tmp1B8->tagged=0;_tmp1B8;});_tmp1B7->attributes=0;_tmp1B7->expected_mem_kind=0;_tmp1B7;});
# 709
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp1B3=_cycalloc(sizeof(*_tmp1B3));_tmp1B3->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp1B4=_cycalloc(sizeof(*_tmp1B4));_tmp1B4[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp1B5;_tmp1B5.tag=5;_tmp1B5.f1=_tmp1B2;_tmp1B5;});_tmp1B4;}),0);_tmp1B3->tl=Cyc_Toc_result_decls;_tmp1B3;});{
void*_tmp1B6=x;_npop_handler(0);return _tmp1B6;};};}
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
struct _tuple26 _tmp1C5=*env;struct _tuple26 _tmp1C6=_tmp1C5;struct Cyc_Xarray_Xarray*_tmp1C7;_LL85: _tmp1C7=(_tmp1C6.f1)->temp_labels;_LL86:;{
int _tmp1C8=Cyc_Toc_fresh_label_counter ++;
if(_tmp1C8 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp1C7))
return((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp1C7,_tmp1C8);{
struct _dyneither_ptr*res=({struct _dyneither_ptr*_tmp1CB=_cycalloc(sizeof(*_tmp1CB));_tmp1CB[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1CE;_tmp1CE.tag=1;_tmp1CE.f1=(unsigned int)_tmp1C8;({void*_tmp1CC[1]={& _tmp1CE};Cyc_aprintf(({const char*_tmp1CD="_LL%X";_tag_dyneither(_tmp1CD,sizeof(char),6);}),_tag_dyneither(_tmp1CC,sizeof(void*),1));});});_tmp1CB;});
if(((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp1C7,res)!= _tmp1C8)
({void*_tmp1C9=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1CA="fresh_label: add_ind returned bad index...";_tag_dyneither(_tmp1CA,sizeof(char),43);}),_tag_dyneither(_tmp1C9,sizeof(void*),0));});
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
struct _tuple9 _tmp1D0=*a;struct _tuple9 _tmp1D1=_tmp1D0;struct _dyneither_ptr*_tmp1D2;struct Cyc_Absyn_Tqual _tmp1D3;void*_tmp1D4;_LL88: _tmp1D2=_tmp1D1.f1;_tmp1D3=_tmp1D1.f2;_tmp1D4=_tmp1D1.f3;_LL89:;
return({struct _tuple9*_tmp1D5=_cycalloc(sizeof(*_tmp1D5));_tmp1D5->f1=_tmp1D2;_tmp1D5->f2=_tmp1D3;_tmp1D5->f3=Cyc_Toc_typ_to_c(_tmp1D4);_tmp1D5;});}
# 771
static struct _tuple11*Cyc_Toc_typ_to_c_f(struct _tuple11*x){
struct _tuple11 _tmp1D6=*x;struct _tuple11 _tmp1D7=_tmp1D6;struct Cyc_Absyn_Tqual _tmp1D8;void*_tmp1D9;_LL8B: _tmp1D8=_tmp1D7.f1;_tmp1D9=_tmp1D7.f2;_LL8C:;
return({struct _tuple11*_tmp1DA=_cycalloc(sizeof(*_tmp1DA));_tmp1DA->f1=_tmp1D8;_tmp1DA->f2=Cyc_Toc_typ_to_c(_tmp1D9);_tmp1DA;});}
# 790 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp1DB=Cyc_Tcutil_compress(t);void*_tmp1DC=_tmp1DB;void*_tmp1DD;void*_tmp1DE;struct Cyc_Absyn_Tqual _tmp1DF;struct Cyc_Absyn_Exp*_tmp1E0;union Cyc_Absyn_Constraint*_tmp1E1;unsigned int _tmp1E2;switch(*((int*)_tmp1DC)){case 8: _LL8E: _tmp1DE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1DC)->f1).elt_type;_tmp1DF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1DC)->f1).tq;_tmp1E0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1DC)->f1).num_elts;_tmp1E1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1DC)->f1).zero_term;_tmp1E2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1DC)->f1).zt_loc;_LL8F:
# 793
 return Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(_tmp1DE),_tmp1DF);case 1: _LL90: _tmp1DD=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1DC)->f2;if(_tmp1DD != 0){_LL91:
 return Cyc_Toc_typ_to_c_array(_tmp1DD);}else{goto _LL92;}default: _LL92: _LL93:
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
void*_tmp1EC=t;struct Cyc_Absyn_Datatypedecl*_tmp1ED;void**_tmp1EE;struct Cyc_Absyn_Enumdecl*_tmp1EF;struct Cyc_Absyn_Aggrdecl*_tmp1F0;struct Cyc_Absyn_Exp*_tmp1F1;struct Cyc_Absyn_Exp*_tmp1F2;void*_tmp1F3;struct _tuple1*_tmp1F4;struct Cyc_List_List*_tmp1F5;struct Cyc_Absyn_Typedefdecl*_tmp1F6;void*_tmp1F7;struct Cyc_List_List*_tmp1F8;struct _tuple1*_tmp1F9;union Cyc_Absyn_AggrInfoU _tmp1FA;struct Cyc_List_List*_tmp1FB;enum Cyc_Absyn_AggrKind _tmp1FC;struct Cyc_List_List*_tmp1FD;struct Cyc_List_List*_tmp1FE;struct Cyc_Absyn_Tqual _tmp1FF;void*_tmp200;struct Cyc_List_List*_tmp201;int _tmp202;struct Cyc_Absyn_VarargInfo*_tmp203;struct Cyc_List_List*_tmp204;void*_tmp205;struct Cyc_Absyn_Tqual _tmp206;struct Cyc_Absyn_Exp*_tmp207;unsigned int _tmp208;void*_tmp209;struct Cyc_Absyn_Tqual _tmp20A;union Cyc_Absyn_Constraint*_tmp20B;struct Cyc_Absyn_Datatypedecl*_tmp20C;struct Cyc_Absyn_Datatypefield*_tmp20D;struct Cyc_Absyn_Tvar*_tmp20E;void**_tmp20F;switch(*((int*)_tmp1EC)){case 0: _LL9F: _LLA0:
 return t;case 1: _LLA1: _tmp20F=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1EC)->f2;_LLA2:
# 852
 if(*_tmp20F == 0){
*_tmp20F=Cyc_Absyn_sint_typ;
return Cyc_Absyn_sint_typ;}
# 856
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp20F));case 2: _LLA3: _tmp20E=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1EC)->f1;_LLA4:
# 858
 if((Cyc_Tcutil_tvar_kind(_tmp20E,& Cyc_Tcutil_bk))->kind == Cyc_Absyn_AnyKind)
# 861
return(void*)& Cyc_Absyn_VoidType_val;else{
# 863
return Cyc_Absyn_void_star_typ();}case 3: _LLA5: _LLA6:
# 865
 return(void*)& Cyc_Absyn_VoidType_val;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1EC)->f1).field_info).KnownDatatypefield).tag == 2){_LLA7: _tmp20C=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1EC)->f1).field_info).KnownDatatypefield).val).f1;_tmp20D=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1EC)->f1).field_info).KnownDatatypefield).val).f2;_LLA8:
# 867
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp20D->name,_tmp20C->name));}else{_LLA9: _LLAA:
({void*_tmp210=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp211="unresolved DatatypeFieldType";_tag_dyneither(_tmp211,sizeof(char),29);}),_tag_dyneither(_tmp210,sizeof(void*),0));});}case 5: _LLAB: _tmp209=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1EC)->f1).elt_typ;_tmp20A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1EC)->f1).elt_tq;_tmp20B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1EC)->f1).ptr_atts).bounds;_LLAC:
# 872
 _tmp209=Cyc_Toc_typ_to_c(_tmp209);{
void*_tmp212=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp20B);void*_tmp213=_tmp212;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp213)->tag == 0){_LLE0: _LLE1:
 return Cyc_Toc_dyneither_ptr_typ;}else{_LLE2: _LLE3:
 return Cyc_Absyn_star_typ(_tmp209,(void*)& Cyc_Absyn_HeapRgn_val,_tmp20A,Cyc_Absyn_false_conref);}_LLDF:;};case 6: _LLAD: _LLAE:
# 877
 goto _LLB0;case 7: _LLAF: _LLB0:
 return t;case 8: _LLB1: _tmp205=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1EC)->f1).elt_type;_tmp206=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1EC)->f1).tq;_tmp207=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1EC)->f1).num_elts;_tmp208=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1EC)->f1).zt_loc;_LLB2:
# 880
 return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c(_tmp205),_tmp206,_tmp207,Cyc_Absyn_false_conref,_tmp208);case 9: _LLB3: _tmp1FF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EC)->f1).ret_tqual;_tmp200=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EC)->f1).ret_typ;_tmp201=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EC)->f1).args;_tmp202=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EC)->f1).c_varargs;_tmp203=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EC)->f1).cyc_varargs;_tmp204=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EC)->f1).attributes;_LLB4: {
# 886
struct Cyc_List_List*_tmp214=0;
for(0;_tmp204 != 0;_tmp204=_tmp204->tl){
void*_tmp215=(void*)_tmp204->hd;void*_tmp216=_tmp215;switch(*((int*)_tmp216)){case 4: _LLE5: _LLE6:
 goto _LLE8;case 5: _LLE7: _LLE8:
 goto _LLEA;case 19: _LLE9: _LLEA:
 continue;case 22: _LLEB: _LLEC:
 continue;case 21: _LLED: _LLEE:
 continue;case 20: _LLEF: _LLF0:
 continue;default: _LLF1: _LLF2:
 _tmp214=({struct Cyc_List_List*_tmp217=_cycalloc(sizeof(*_tmp217));_tmp217->hd=(void*)_tmp204->hd;_tmp217->tl=_tmp214;_tmp217;});goto _LLE4;}_LLE4:;}{
# 897
struct Cyc_List_List*_tmp218=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp201);
if(_tmp203 != 0){
# 900
void*_tmp219=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp203->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple9*_tmp21A=({struct _tuple9*_tmp21C=_cycalloc(sizeof(*_tmp21C));_tmp21C->f1=_tmp203->name;_tmp21C->f2=_tmp203->tq;_tmp21C->f3=_tmp219;_tmp21C;});
_tmp218=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp218,({struct Cyc_List_List*_tmp21B=_cycalloc(sizeof(*_tmp21B));_tmp21B->hd=_tmp21A;_tmp21B->tl=0;_tmp21B;}));}
# 904
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp21D=_cycalloc(sizeof(*_tmp21D));_tmp21D[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp21E;_tmp21E.tag=9;_tmp21E.f1=({struct Cyc_Absyn_FnInfo _tmp21F;_tmp21F.tvars=0;_tmp21F.effect=0;_tmp21F.ret_tqual=_tmp1FF;_tmp21F.ret_typ=Cyc_Toc_typ_to_c(_tmp200);_tmp21F.args=_tmp218;_tmp21F.c_varargs=_tmp202;_tmp21F.cyc_varargs=0;_tmp21F.rgn_po=0;_tmp21F.attributes=_tmp214;_tmp21F.requires_clause=0;_tmp21F.requires_relns=0;_tmp21F.ensures_clause=0;_tmp21F.ensures_relns=0;_tmp21F;});_tmp21E;});_tmp21D;});};}case 10: _LLB5: _tmp1FE=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1EC)->f1;_LLB6:
# 909
 _tmp1FE=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp1FE);
return Cyc_Toc_add_tuple_type(_tmp1FE);case 12: _LLB7: _tmp1FC=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1EC)->f1;_tmp1FD=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1EC)->f2;_LLB8:
# 914
 return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp220=_cycalloc(sizeof(*_tmp220));_tmp220[0]=({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp221;_tmp221.tag=12;_tmp221.f1=_tmp1FC;_tmp221.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp1FD);_tmp221;});_tmp220;});case 11: _LLB9: _tmp1FA=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1EC)->f1).aggr_info;_tmp1FB=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1EC)->f1).targs;_LLBA:
# 918
{union Cyc_Absyn_AggrInfoU _tmp222=_tmp1FA;if((_tmp222.UnknownAggr).tag == 1){_LLF4: _LLF5:
 return t;}else{_LLF6: _LLF7:
 goto _LLF3;}_LLF3:;}{
# 922
struct Cyc_Absyn_Aggrdecl*_tmp223=Cyc_Absyn_get_known_aggrdecl(_tmp1FA);
if(_tmp223->expected_mem_kind){
# 925
if(_tmp223->impl == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp227;_tmp227.tag=0;_tmp227.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 928
Cyc_Absynpp_qvar2string(_tmp223->name));({struct Cyc_String_pa_PrintArg_struct _tmp226;_tmp226.tag=0;_tmp226.f1=(struct _dyneither_ptr)(
# 927
_tmp223->kind == Cyc_Absyn_UnionA?({const char*_tmp228="union";_tag_dyneither(_tmp228,sizeof(char),6);}):({const char*_tmp229="struct";_tag_dyneither(_tmp229,sizeof(char),7);}));({void*_tmp224[2]={& _tmp226,& _tmp227};Cyc_Tcutil_warn(0,({const char*_tmp225="%s %s was never defined.";_tag_dyneither(_tmp225,sizeof(char),25);}),_tag_dyneither(_tmp224,sizeof(void*),2));});});});}
# 933
if(_tmp223->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp223->name,Cyc_Absyn_unionq_typ);{
struct Cyc_List_List*_tmp22A=_tmp223->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp223->impl))->fields;
if(_tmp22A == 0)return Cyc_Toc_aggrdecl_type(_tmp223->name,Cyc_Absyn_strctq);
for(0;_tmp22A->tl != 0;_tmp22A=_tmp22A->tl){;}{
void*_tmp22B=((struct Cyc_Absyn_Aggrfield*)_tmp22A->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp22B))){
if(_tmp223->expected_mem_kind)
({struct Cyc_String_pa_PrintArg_struct _tmp22E;_tmp22E.tag=0;_tmp22E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp223->name));({void*_tmp22C[1]={& _tmp22E};Cyc_Tcutil_warn(0,({const char*_tmp22D="struct %s ended up being abstract.";_tag_dyneither(_tmp22D,sizeof(char),35);}),_tag_dyneither(_tmp22C,sizeof(void*),1));});});{
# 947
struct _RegionHandle _tmp22F=_new_region("r");struct _RegionHandle*r=& _tmp22F;_push_region(r);
{struct Cyc_List_List*_tmp230=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp223->tvs,_tmp1FB);
void*_tmp231=Cyc_Tcutil_rsubstitute(r,_tmp230,_tmp22B);
if(Cyc_Toc_is_abstract_type(_tmp231)){void*_tmp232=Cyc_Toc_aggrdecl_type(_tmp223->name,Cyc_Absyn_strctq);_npop_handler(0);return _tmp232;}{
void*_tmp233=Cyc_Toc_add_struct_type(_tmp223->name,_tmp223->tvs,_tmp1FB,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp223->impl))->fields);_npop_handler(0);return _tmp233;};}
# 948
;_pop_region(r);};}
# 953
return Cyc_Toc_aggrdecl_type(_tmp223->name,Cyc_Absyn_strctq);};};};case 13: _LLBB: _tmp1F9=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp1EC)->f1;_LLBC:
 return t;case 14: _LLBD: _tmp1F8=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1EC)->f1;_LLBE:
 Cyc_Toc_enumfields_to_c(_tmp1F8);return t;case 17: _LLBF: _tmp1F4=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1EC)->f1;_tmp1F5=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1EC)->f2;_tmp1F6=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1EC)->f3;_tmp1F7=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1EC)->f4;_LLC0:
# 957
 if(_tmp1F7 == 0  || Cyc_noexpand_r){
if(_tmp1F5 != 0)
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp234=_cycalloc(sizeof(*_tmp234));_tmp234[0]=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp235;_tmp235.tag=17;_tmp235.f1=_tmp1F4;_tmp235.f2=0;_tmp235.f3=_tmp1F6;_tmp235.f4=0;_tmp235;});_tmp234;});else{
return t;}}else{
# 962
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp236=_cycalloc(sizeof(*_tmp236));_tmp236[0]=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp237;_tmp237.tag=17;_tmp237.f1=_tmp1F4;_tmp237.f2=0;_tmp237.f3=_tmp1F6;_tmp237.f4=Cyc_Toc_typ_to_c(_tmp1F7);_tmp237;});_tmp236;});}case 19: _LLC1: _LLC2:
 return Cyc_Absyn_uint_typ;case 15: _LLC3: _tmp1F3=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1EC)->f1;_LLC4:
 return Cyc_Toc_rgn_typ();case 16: _LLC5: _LLC6:
 return Cyc_Toc_dyn_rgn_typ();case 20: _LLC7: _LLC8:
# 968
 goto _LLCA;case 21: _LLC9: _LLCA:
 goto _LLCC;case 22: _LLCB: _LLCC:
 goto _LLCE;case 23: _LLCD: _LLCE:
 goto _LLD0;case 24: _LLCF: _LLD0:
 goto _LLD2;case 25: _LLD1: _LLD2:
 return Cyc_Absyn_void_star_typ();case 18: _LLD3: _tmp1F2=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp1EC)->f1;_LLD4:
# 978
 return t;case 27: _LLD5: _tmp1F1=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp1EC)->f1;_LLD6:
# 980
 return t;case 28: _LLD7: _LLD8:
 return t;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1EC)->f1)->r)){case 0: _LLD9: _tmp1F0=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1EC)->f1)->r)->f1;_LLDA:
# 983
 Cyc_Toc_aggrdecl_to_c(_tmp1F0,1);
if(_tmp1F0->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp1F0->name,Cyc_Absyn_unionq_typ);else{
return Cyc_Toc_aggrdecl_type(_tmp1F0->name,Cyc_Absyn_strctq);}case 1: _LLDB: _tmp1EF=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1EC)->f1)->r)->f1;_LLDC:
# 988
 Cyc_Toc_enumdecl_to_c(_tmp1EF);
return t;default: _LLDD: _tmp1ED=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1EC)->f1)->r)->f1;_tmp1EE=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1EC)->f2;_LLDE:
# 991
 Cyc_Toc_datatypedecl_to_c(_tmp1ED);
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp1EE)));}}_LL9E:;}
# 996
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned int l){
void*_tmp238=t;void*_tmp239;struct Cyc_Absyn_Tqual _tmp23A;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp238)->tag == 8){_LLF9: _tmp239=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp238)->f1).elt_type;_tmp23A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp238)->f1).tq;_LLFA:
# 999
 return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp239,(void*)& Cyc_Absyn_HeapRgn_val,_tmp23A,Cyc_Absyn_false_conref),e);}else{_LLFB: _LLFC:
 return Cyc_Toc_cast_it(t,e);}_LLF8:;}
# 1006
static int Cyc_Toc_atomic_typ(void*t){
void*_tmp23B=Cyc_Tcutil_compress(t);void*_tmp23C=_tmp23B;struct Cyc_List_List*_tmp23D;struct Cyc_Absyn_Datatypedecl*_tmp23E;struct Cyc_Absyn_Datatypefield*_tmp23F;struct Cyc_List_List*_tmp240;union Cyc_Absyn_AggrInfoU _tmp241;void*_tmp242;switch(*((int*)_tmp23C)){case 0: _LLFE: _LLFF:
 return 1;case 2: _LL100: _LL101:
 return 0;case 6: _LL102: _LL103:
 goto _LL105;case 13: _LL104: _LL105:
 goto _LL107;case 14: _LL106: _LL107:
 goto _LL109;case 7: _LL108: _LL109:
 goto _LL10B;case 9: _LL10A: _LL10B:
 goto _LL10D;case 19: _LL10C: _LL10D:
 return 1;case 8: _LL10E: _tmp242=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp23C)->f1).elt_type;_LL10F:
 return Cyc_Toc_atomic_typ(_tmp242);case 11: _LL110: _tmp241=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp23C)->f1).aggr_info;_LL111:
# 1023
{union Cyc_Absyn_AggrInfoU _tmp243=_tmp241;if((_tmp243.UnknownAggr).tag == 1){_LL123: _LL124:
 return 0;}else{_LL125: _LL126:
 goto _LL122;}_LL122:;}{
# 1027
struct Cyc_Absyn_Aggrdecl*_tmp244=Cyc_Absyn_get_known_aggrdecl(_tmp241);
if(_tmp244->impl == 0)
return 0;
{struct Cyc_List_List*_tmp245=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp244->impl))->fields;for(0;_tmp245 != 0;_tmp245=_tmp245->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp245->hd)->type))return 0;}}
return 1;};case 12: _LL112: _tmp240=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp23C)->f2;_LL113:
# 1034
 for(0;_tmp240 != 0;_tmp240=_tmp240->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp240->hd)->type))return 0;}
return 1;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp23C)->f1).field_info).KnownDatatypefield).tag == 2){_LL114: _tmp23E=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp23C)->f1).field_info).KnownDatatypefield).val).f1;_tmp23F=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp23C)->f1).field_info).KnownDatatypefield).val).f2;_LL115:
# 1038
 _tmp23D=_tmp23F->typs;goto _LL117;}else{goto _LL120;}case 10: _LL116: _tmp23D=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp23C)->f1;_LL117:
# 1040
 for(0;_tmp23D != 0;_tmp23D=_tmp23D->tl){
if(!Cyc_Toc_atomic_typ((*((struct _tuple11*)_tmp23D->hd)).f2))return 0;}
return 1;case 3: _LL118: _LL119:
# 1045
 goto _LL11B;case 5: _LL11A: _LL11B:
 goto _LL11D;case 16: _LL11C: _LL11D:
 goto _LL11F;case 15: _LL11E: _LL11F:
 return 0;default: _LL120: _LL121:
({struct Cyc_String_pa_PrintArg_struct _tmp248;_tmp248.tag=0;_tmp248.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp246[1]={& _tmp248};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp247="atomic_typ:  bad type %s";_tag_dyneither(_tmp247,sizeof(char),25);}),_tag_dyneither(_tmp246,sizeof(void*),1));});});}_LLFD:;}
# 1053
static int Cyc_Toc_is_void_star(void*t){
void*_tmp249=Cyc_Tcutil_compress(t);void*_tmp24A=_tmp249;void*_tmp24B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24A)->tag == 5){_LL128: _tmp24B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24A)->f1).elt_typ;_LL129: {
# 1056
void*_tmp24C=Cyc_Tcutil_compress(_tmp24B);void*_tmp24D=_tmp24C;if(((struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp24D)->tag == 0){_LL12D: _LL12E:
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
void*_tmp24E=Cyc_Tcutil_compress(t);void*_tmp24F=_tmp24E;struct Cyc_List_List*_tmp250;union Cyc_Absyn_AggrInfoU _tmp251;switch(*((int*)_tmp24F)){case 11: _LL132: _tmp251=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp24F)->f1).aggr_info;_LL133: {
# 1076
struct Cyc_Absyn_Aggrdecl*_tmp252=Cyc_Absyn_get_known_aggrdecl(_tmp251);
if(_tmp252->impl == 0)
({void*_tmp253=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp254="is_poly_field: type missing fields";_tag_dyneither(_tmp254,sizeof(char),35);}),_tag_dyneither(_tmp253,sizeof(void*),0));});
_tmp250=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp252->impl))->fields;goto _LL135;}case 12: _LL134: _tmp250=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp24F)->f2;_LL135: {
# 1081
struct Cyc_Absyn_Aggrfield*_tmp255=Cyc_Absyn_lookup_field(_tmp250,f);
if(_tmp255 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp258;_tmp258.tag=0;_tmp258.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp256[1]={& _tmp258};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp257="is_poly_field: bad field %s";_tag_dyneither(_tmp257,sizeof(char),28);}),_tag_dyneither(_tmp256,sizeof(void*),1));});});
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp255->type);}default: _LL136: _LL137:
({struct Cyc_String_pa_PrintArg_struct _tmp25B;_tmp25B.tag=0;_tmp25B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp259[1]={& _tmp25B};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp25A="is_poly_field: bad type %s";_tag_dyneither(_tmp25A,sizeof(char),27);}),_tag_dyneither(_tmp259,sizeof(void*),1));});});}_LL131:;}
# 1092
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp25C=e->r;void*_tmp25D=_tmp25C;struct Cyc_Absyn_Exp*_tmp25E;struct _dyneither_ptr*_tmp25F;struct Cyc_Absyn_Exp*_tmp260;struct _dyneither_ptr*_tmp261;switch(*((int*)_tmp25D)){case 20: _LL139: _tmp260=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp25D)->f1;_tmp261=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp25D)->f2;_LL13A:
# 1095
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp260->topt),_tmp261) && !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));case 21: _LL13B: _tmp25E=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp25D)->f1;_tmp25F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp25D)->f2;_LL13C: {
# 1098
void*_tmp262=Cyc_Tcutil_compress((void*)_check_null(_tmp25E->topt));void*_tmp263=_tmp262;void*_tmp264;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp263)->tag == 5){_LL140: _tmp264=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp263)->f1).elt_typ;_LL141:
# 1100
 return Cyc_Toc_is_poly_field(_tmp264,_tmp25F) && !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}else{_LL142: _LL143:
({struct Cyc_String_pa_PrintArg_struct _tmp267;_tmp267.tag=0;_tmp267.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp25E->topt)));({void*_tmp265[1]={& _tmp267};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp266="is_poly_project: bad type %s";_tag_dyneither(_tmp266,sizeof(char),29);}),_tag_dyneither(_tmp265,sizeof(void*),1));});});}_LL13F:;}default: _LL13D: _LL13E:
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
return urm_exp;};};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_StructInfo{struct Cyc_Absyn_Exp**varsizeexp;struct Cyc_Absyn_Exp*lhs_exp;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;int*in_lhs;struct Cyc_Toc_StructInfo struct_info;int*in_sizeof;struct _RegionHandle*rgn;};
# 1230 "toc.cyc"
typedef struct Cyc_Toc_Env*Cyc_Toc_env_t;
# 1232
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp27B=nv;int _tmp27C;_LL14C: _tmp27C=_tmp27B->toplevel;_LL14D:;
return _tmp27C;}
# 1236
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp27D=nv;int*_tmp27E;_LL14F: _tmp27E=_tmp27D->in_lhs;_LL150:;
return*_tmp27E;}
# 1241
static struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple1*x){
struct Cyc_Toc_Env*_tmp27F=nv;struct Cyc_Dict_Dict _tmp280;_LL152: _tmp280=_tmp27F->varmap;_LL153:;
return((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp280,x);}
# 1247
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
return({struct Cyc_Toc_Env*_tmp281=_region_malloc(r,sizeof(*_tmp281));_tmp281->break_lab=(struct _dyneither_ptr**)0;_tmp281->continue_lab=(struct _dyneither_ptr**)0;_tmp281->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0;_tmp281->varmap=(struct Cyc_Dict_Dict)
# 1251
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp);_tmp281->toplevel=(int)1;_tmp281->in_lhs=(int*)({int*_tmp284=_region_malloc(r,sizeof(*_tmp284));_tmp284[0]=0;_tmp284;});_tmp281->struct_info=(struct Cyc_Toc_StructInfo)({(_tmp281->struct_info).varsizeexp=({struct Cyc_Absyn_Exp**_tmp283=_cycalloc(sizeof(*_tmp283));_tmp283[0]=0;_tmp283;});(_tmp281->struct_info).lhs_exp=0;_tmp281->struct_info;});_tmp281->in_sizeof=(int*)({int*_tmp282=_region_malloc(r,sizeof(*_tmp282));_tmp282[0]=0;_tmp282;});_tmp281->rgn=(struct _RegionHandle*)r;_tmp281;});}
# 1259
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp285=e;struct _dyneither_ptr**_tmp286;struct _dyneither_ptr**_tmp287;struct Cyc_Toc_FallthruInfo*_tmp288;struct Cyc_Dict_Dict _tmp289;int _tmp28A;int*_tmp28B;struct Cyc_Toc_StructInfo _tmp28C;int*_tmp28D;_LL155: _tmp286=_tmp285->break_lab;_tmp287=_tmp285->continue_lab;_tmp288=_tmp285->fallthru_info;_tmp289=_tmp285->varmap;_tmp28A=_tmp285->toplevel;_tmp28B=_tmp285->in_lhs;_tmp28C=_tmp285->struct_info;_tmp28D=_tmp285->in_sizeof;_LL156:;
return({struct Cyc_Toc_Env*_tmp28E=_region_malloc(r,sizeof(*_tmp28E));_tmp28E->break_lab=(struct _dyneither_ptr**)_tmp286;_tmp28E->continue_lab=(struct _dyneither_ptr**)_tmp287;_tmp28E->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp288;_tmp28E->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp289);_tmp28E->toplevel=(int)_tmp28A;_tmp28E->in_lhs=(int*)_tmp28B;_tmp28E->struct_info=(struct Cyc_Toc_StructInfo)_tmp28C;_tmp28E->in_sizeof=(int*)_tmp28D;_tmp28E->rgn=(struct _RegionHandle*)r;_tmp28E;});}
# 1264
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp28F=e;struct _dyneither_ptr**_tmp290;struct _dyneither_ptr**_tmp291;struct Cyc_Toc_FallthruInfo*_tmp292;struct Cyc_Dict_Dict _tmp293;int _tmp294;int*_tmp295;struct Cyc_Toc_StructInfo _tmp296;int*_tmp297;_LL158: _tmp290=_tmp28F->break_lab;_tmp291=_tmp28F->continue_lab;_tmp292=_tmp28F->fallthru_info;_tmp293=_tmp28F->varmap;_tmp294=_tmp28F->toplevel;_tmp295=_tmp28F->in_lhs;_tmp296=_tmp28F->struct_info;_tmp297=_tmp28F->in_sizeof;_LL159:;
return({struct Cyc_Toc_Env*_tmp298=_region_malloc(r,sizeof(*_tmp298));_tmp298->break_lab=(struct _dyneither_ptr**)_tmp290;_tmp298->continue_lab=(struct _dyneither_ptr**)_tmp291;_tmp298->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp292;_tmp298->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp293);_tmp298->toplevel=(int)0;_tmp298->in_lhs=(int*)_tmp295;_tmp298->struct_info=(struct Cyc_Toc_StructInfo)_tmp296;_tmp298->in_sizeof=(int*)_tmp297;_tmp298->rgn=(struct _RegionHandle*)r;_tmp298;});}
# 1268
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp299=e;struct _dyneither_ptr**_tmp29A;struct _dyneither_ptr**_tmp29B;struct Cyc_Toc_FallthruInfo*_tmp29C;struct Cyc_Dict_Dict _tmp29D;int _tmp29E;int*_tmp29F;struct Cyc_Toc_StructInfo _tmp2A0;int*_tmp2A1;_LL15B: _tmp29A=_tmp299->break_lab;_tmp29B=_tmp299->continue_lab;_tmp29C=_tmp299->fallthru_info;_tmp29D=_tmp299->varmap;_tmp29E=_tmp299->toplevel;_tmp29F=_tmp299->in_lhs;_tmp2A0=_tmp299->struct_info;_tmp2A1=_tmp299->in_sizeof;_LL15C:;
return({struct Cyc_Toc_Env*_tmp2A2=_region_malloc(r,sizeof(*_tmp2A2));_tmp2A2->break_lab=(struct _dyneither_ptr**)_tmp29A;_tmp2A2->continue_lab=(struct _dyneither_ptr**)_tmp29B;_tmp2A2->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp29C;_tmp2A2->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp29D);_tmp2A2->toplevel=(int)1;_tmp2A2->in_lhs=(int*)_tmp29F;_tmp2A2->struct_info=(struct Cyc_Toc_StructInfo)_tmp2A0;_tmp2A2->in_sizeof=(int*)_tmp2A1;_tmp2A2->rgn=(struct _RegionHandle*)r;_tmp2A2;});}
# 1272
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp2A3=e;int*_tmp2A4;_LL15E: _tmp2A4=_tmp2A3->in_lhs;_LL15F:;
*_tmp2A4=b;}
# 1276
static int Cyc_Toc_set_in_sizeof(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp2A5=e;int*_tmp2A6;_LL161: _tmp2A6=_tmp2A5->in_sizeof;_LL162:;{
int _tmp2A7=*_tmp2A6;
*_tmp2A6=b;
return _tmp2A7;};}
# 1282
static int Cyc_Toc_in_sizeof(struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp2A8=e;int*_tmp2A9;_LL164: _tmp2A9=_tmp2A8->in_sizeof;_LL165:;
return*_tmp2A9;}
# 1287
static struct Cyc_Toc_Env*Cyc_Toc_set_lhs_exp(struct _RegionHandle*r,struct Cyc_Absyn_Exp*exp,struct Cyc_Toc_Env*e){
# 1289
struct Cyc_Toc_Env*_tmp2AA=e;struct _dyneither_ptr**_tmp2AB;struct _dyneither_ptr**_tmp2AC;struct Cyc_Toc_FallthruInfo*_tmp2AD;struct Cyc_Dict_Dict _tmp2AE;int _tmp2AF;int*_tmp2B0;struct Cyc_Absyn_Exp**_tmp2B1;int*_tmp2B2;_LL167: _tmp2AB=_tmp2AA->break_lab;_tmp2AC=_tmp2AA->continue_lab;_tmp2AD=_tmp2AA->fallthru_info;_tmp2AE=_tmp2AA->varmap;_tmp2AF=_tmp2AA->toplevel;_tmp2B0=_tmp2AA->in_lhs;_tmp2B1=(_tmp2AA->struct_info).varsizeexp;_tmp2B2=_tmp2AA->in_sizeof;_LL168:;
return({struct Cyc_Toc_Env*_tmp2B3=_region_malloc(r,sizeof(*_tmp2B3));_tmp2B3->break_lab=(struct _dyneither_ptr**)_tmp2AB;_tmp2B3->continue_lab=(struct _dyneither_ptr**)_tmp2AC;_tmp2B3->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2AD;_tmp2B3->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2AE);_tmp2B3->toplevel=(int)_tmp2AF;_tmp2B3->in_lhs=(int*)_tmp2B0;_tmp2B3->struct_info=(struct Cyc_Toc_StructInfo)({(_tmp2B3->struct_info).varsizeexp=_tmp2B1;(_tmp2B3->struct_info).lhs_exp=exp;_tmp2B3->struct_info;});_tmp2B3->in_sizeof=(int*)_tmp2B2;_tmp2B3->rgn=(struct _RegionHandle*)r;_tmp2B3;});}
# 1295
static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple1*x,struct Cyc_Absyn_Exp*y){
{union Cyc_Absyn_Nmspace _tmp2B4=(*x).f1;union Cyc_Absyn_Nmspace _tmp2B5=_tmp2B4;if((_tmp2B5.Rel_n).tag == 1){_LL16A: _LL16B:
# 1298
({struct Cyc_String_pa_PrintArg_struct _tmp2B8;_tmp2B8.tag=0;_tmp2B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(x));({void*_tmp2B6[1]={& _tmp2B8};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2B7="Toc::add_varmap on Rel_n: %s\n";_tag_dyneither(_tmp2B7,sizeof(char),30);}),_tag_dyneither(_tmp2B6,sizeof(void*),1));});});}else{_LL16C: _LL16D:
 goto _LL169;}_LL169:;}{
# 1301
struct Cyc_Toc_Env*_tmp2B9=e;struct _dyneither_ptr**_tmp2BA;struct _dyneither_ptr**_tmp2BB;struct Cyc_Toc_FallthruInfo*_tmp2BC;struct Cyc_Dict_Dict _tmp2BD;int _tmp2BE;int*_tmp2BF;struct Cyc_Toc_StructInfo _tmp2C0;int*_tmp2C1;_LL16F: _tmp2BA=_tmp2B9->break_lab;_tmp2BB=_tmp2B9->continue_lab;_tmp2BC=_tmp2B9->fallthru_info;_tmp2BD=_tmp2B9->varmap;_tmp2BE=_tmp2B9->toplevel;_tmp2BF=_tmp2B9->in_lhs;_tmp2C0=_tmp2B9->struct_info;_tmp2C1=_tmp2B9->in_sizeof;_LL170:;{
struct Cyc_Dict_Dict _tmp2C2=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2BD),x,y);
return({struct Cyc_Toc_Env*_tmp2C3=_region_malloc(r,sizeof(*_tmp2C3));_tmp2C3->break_lab=(struct _dyneither_ptr**)_tmp2BA;_tmp2C3->continue_lab=(struct _dyneither_ptr**)_tmp2BB;_tmp2C3->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2BC;_tmp2C3->varmap=(struct Cyc_Dict_Dict)_tmp2C2;_tmp2C3->toplevel=(int)_tmp2BE;_tmp2C3->in_lhs=(int*)_tmp2BF;_tmp2C3->struct_info=(struct Cyc_Toc_StructInfo)_tmp2C0;_tmp2C3->in_sizeof=(int*)_tmp2C1;_tmp2C3->rgn=(struct _RegionHandle*)r;_tmp2C3;});};};}
# 1308
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp2C4=e;struct _dyneither_ptr**_tmp2C5;struct _dyneither_ptr**_tmp2C6;struct Cyc_Toc_FallthruInfo*_tmp2C7;struct Cyc_Dict_Dict _tmp2C8;int _tmp2C9;int*_tmp2CA;struct Cyc_Toc_StructInfo _tmp2CB;int*_tmp2CC;_LL172: _tmp2C5=_tmp2C4->break_lab;_tmp2C6=_tmp2C4->continue_lab;_tmp2C7=_tmp2C4->fallthru_info;_tmp2C8=_tmp2C4->varmap;_tmp2C9=_tmp2C4->toplevel;_tmp2CA=_tmp2C4->in_lhs;_tmp2CB=_tmp2C4->struct_info;_tmp2CC=_tmp2C4->in_sizeof;_LL173:;
return({struct Cyc_Toc_Env*_tmp2CD=_region_malloc(r,sizeof(*_tmp2CD));_tmp2CD->break_lab=(struct _dyneither_ptr**)0;_tmp2CD->continue_lab=(struct _dyneither_ptr**)0;_tmp2CD->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2C7;_tmp2CD->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2C8);_tmp2CD->toplevel=(int)_tmp2C9;_tmp2CD->in_lhs=(int*)_tmp2CA;_tmp2CD->struct_info=(struct Cyc_Toc_StructInfo)_tmp2CB;_tmp2CD->in_sizeof=(int*)_tmp2CC;_tmp2CD->rgn=(struct _RegionHandle*)r;_tmp2CD;});}
# 1314
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*next_case_env){
# 1319
struct Cyc_List_List*fallthru_vars=0;
for(0;fallthru_binders != 0;fallthru_binders=fallthru_binders->tl){
fallthru_vars=({struct Cyc_List_List*_tmp2CE=_region_malloc(r,sizeof(*_tmp2CE));_tmp2CE->hd=((struct Cyc_Absyn_Vardecl*)fallthru_binders->hd)->name;_tmp2CE->tl=fallthru_vars;_tmp2CE;});}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fallthru_vars);{
struct Cyc_Toc_Env*_tmp2CF=e;struct _dyneither_ptr**_tmp2D0;struct _dyneither_ptr**_tmp2D1;struct Cyc_Toc_FallthruInfo*_tmp2D2;struct Cyc_Dict_Dict _tmp2D3;int _tmp2D4;int*_tmp2D5;struct Cyc_Toc_StructInfo _tmp2D6;int*_tmp2D7;_LL175: _tmp2D0=_tmp2CF->break_lab;_tmp2D1=_tmp2CF->continue_lab;_tmp2D2=_tmp2CF->fallthru_info;_tmp2D3=_tmp2CF->varmap;_tmp2D4=_tmp2CF->toplevel;_tmp2D5=_tmp2CF->in_lhs;_tmp2D6=_tmp2CF->struct_info;_tmp2D7=_tmp2CF->in_sizeof;_LL176:;{
struct Cyc_Toc_Env*_tmp2D8=next_case_env;struct Cyc_Dict_Dict _tmp2D9;_LL178: _tmp2D9=_tmp2D8->varmap;_LL179:;{
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp2DC=_region_malloc(r,sizeof(*_tmp2DC));_tmp2DC->label=fallthru_l;_tmp2DC->binders=fallthru_vars;_tmp2DC->next_case_env=
# 1327
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2D9);_tmp2DC;});
return({struct Cyc_Toc_Env*_tmp2DA=_region_malloc(r,sizeof(*_tmp2DA));_tmp2DA->break_lab=(struct _dyneither_ptr**)({struct _dyneither_ptr**_tmp2DB=_region_malloc(r,sizeof(*_tmp2DB));_tmp2DB[0]=break_l;_tmp2DB;});_tmp2DA->continue_lab=(struct _dyneither_ptr**)_tmp2D1;_tmp2DA->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi;_tmp2DA->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2D3);_tmp2DA->toplevel=(int)_tmp2D4;_tmp2DA->in_lhs=(int*)_tmp2D5;_tmp2DA->struct_info=(struct Cyc_Toc_StructInfo)_tmp2D6;_tmp2DA->in_sizeof=(int*)_tmp2D7;_tmp2DA->rgn=(struct _RegionHandle*)r;_tmp2DA;});};};};}
# 1331
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1334
struct Cyc_Toc_Env*_tmp2DD=e;struct _dyneither_ptr**_tmp2DE;struct _dyneither_ptr**_tmp2DF;struct Cyc_Toc_FallthruInfo*_tmp2E0;struct Cyc_Dict_Dict _tmp2E1;int _tmp2E2;int*_tmp2E3;struct Cyc_Toc_StructInfo _tmp2E4;int*_tmp2E5;_LL17B: _tmp2DE=_tmp2DD->break_lab;_tmp2DF=_tmp2DD->continue_lab;_tmp2E0=_tmp2DD->fallthru_info;_tmp2E1=_tmp2DD->varmap;_tmp2E2=_tmp2DD->toplevel;_tmp2E3=_tmp2DD->in_lhs;_tmp2E4=_tmp2DD->struct_info;_tmp2E5=_tmp2DD->in_sizeof;_LL17C:;
return({struct Cyc_Toc_Env*_tmp2E6=_region_malloc(r,sizeof(*_tmp2E6));_tmp2E6->break_lab=(struct _dyneither_ptr**)({struct _dyneither_ptr**_tmp2E7=_region_malloc(r,sizeof(*_tmp2E7));_tmp2E7[0]=break_l;_tmp2E7;});_tmp2E6->continue_lab=(struct _dyneither_ptr**)_tmp2DF;_tmp2E6->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0;_tmp2E6->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2E1);_tmp2E6->toplevel=(int)_tmp2E2;_tmp2E6->in_lhs=(int*)_tmp2E3;_tmp2E6->struct_info=(struct Cyc_Toc_StructInfo)_tmp2E4;_tmp2E6->in_sizeof=(int*)_tmp2E5;_tmp2E6->rgn=(struct _RegionHandle*)r;_tmp2E6;});}
# 1341
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1344
struct Cyc_Toc_Env*_tmp2E8=e;struct _dyneither_ptr**_tmp2E9;struct _dyneither_ptr**_tmp2EA;struct Cyc_Toc_FallthruInfo*_tmp2EB;struct Cyc_Dict_Dict _tmp2EC;int _tmp2ED;int*_tmp2EE;struct Cyc_Toc_StructInfo _tmp2EF;int*_tmp2F0;_LL17E: _tmp2E9=_tmp2E8->break_lab;_tmp2EA=_tmp2E8->continue_lab;_tmp2EB=_tmp2E8->fallthru_info;_tmp2EC=_tmp2E8->varmap;_tmp2ED=_tmp2E8->toplevel;_tmp2EE=_tmp2E8->in_lhs;_tmp2EF=_tmp2E8->struct_info;_tmp2F0=_tmp2E8->in_sizeof;_LL17F:;
return({struct Cyc_Toc_Env*_tmp2F1=_region_malloc(r,sizeof(*_tmp2F1));_tmp2F1->break_lab=(struct _dyneither_ptr**)0;_tmp2F1->continue_lab=(struct _dyneither_ptr**)_tmp2EA;_tmp2F1->fallthru_info=(struct Cyc_Toc_FallthruInfo*)({struct Cyc_Toc_FallthruInfo*_tmp2F2=_region_malloc(r,sizeof(*_tmp2F2));_tmp2F2->label=next_l;_tmp2F2->binders=0;_tmp2F2->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp);_tmp2F2;});_tmp2F1->varmap=(struct Cyc_Dict_Dict)
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2EC);_tmp2F1->toplevel=(int)_tmp2ED;_tmp2F1->in_lhs=(int*)_tmp2EE;_tmp2F1->struct_info=(struct Cyc_Toc_StructInfo)_tmp2EF;_tmp2F1->in_sizeof=(int*)_tmp2F0;_tmp2F1->rgn=(struct _RegionHandle*)r;_tmp2F1;});}
# 1359 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1362
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
# 1364
void*_tmp2F3=e->annot;void*_tmp2F4=_tmp2F3;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2F4)->tag == Cyc_CfFlowInfo_UnknownZ){_LL181: _LL182:
# 1367
 return Cyc_Toc_is_nullable((void*)_check_null(e->topt));}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2F4)->tag == Cyc_CfFlowInfo_NotZero){_LL183: _LL184:
# 1370
 return 0;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2F4)->tag == Cyc_CfFlowInfo_IsZero){_LL185: _LL186:
# 1372
({void*_tmp2F5=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp2F6="dereference of NULL pointer";_tag_dyneither(_tmp2F6,sizeof(char),28);}),_tag_dyneither(_tmp2F5,sizeof(void*),0));});
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2F4)->tag == Cyc_Absyn_EmptyAnnot){_LL187: _LL188:
# 1378
 return 1;}else{_LL189: _LL18A:
({void*_tmp2F7=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2F8="need_null_check";_tag_dyneither(_tmp2F8,sizeof(char),16);}),_tag_dyneither(_tmp2F7,sizeof(void*),0));});}}}}_LL180:;}
# 1383
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp2F9=e->annot;void*_tmp2FA=_tmp2F9;struct Cyc_List_List*_tmp2FB;struct Cyc_List_List*_tmp2FC;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2FA)->tag == Cyc_CfFlowInfo_UnknownZ){_LL18C: _tmp2FC=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2FA)->f1;_LL18D:
 return _tmp2FC;}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2FA)->tag == Cyc_CfFlowInfo_NotZero){_LL18E: _tmp2FB=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2FA)->f1;_LL18F:
 return _tmp2FB;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2FA)->tag == Cyc_CfFlowInfo_IsZero){_LL190: _LL191:
# 1388
({void*_tmp2FD=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp2FE="dereference of NULL pointer";_tag_dyneither(_tmp2FE,sizeof(char),28);}),_tag_dyneither(_tmp2FD,sizeof(void*),0));});
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2FA)->tag == Cyc_Absyn_EmptyAnnot){_LL192: _LL193:
 return 0;}else{_LL194: _LL195:
({void*_tmp2FF=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp300="get_relns";_tag_dyneither(_tmp300,sizeof(char),10);}),_tag_dyneither(_tmp2FF,sizeof(void*),0));});}}}}_LL18B:;}static char _tmp305[8]="*bogus*";
# 1398
static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
# 1408 "toc.cyc"
union Cyc_Relations_RelnOp rop_i=Cyc_Relations_RConst(0);
int valid_rop_i=Cyc_Relations_exp2relnop(i,& rop_i);
# 1411
struct Cyc_Absyn_Vardecl*x;
{void*_tmp301=a->r;void*_tmp302=_tmp301;void*_tmp303;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp302)->tag == 1){_LL197: _tmp303=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp302)->f1;_LL198: {
# 1414
struct Cyc_Absyn_Vardecl*_tmp304=Cyc_Tcutil_nonesc_vardecl(_tmp303);
if(_tmp304 == 0)goto _LL19A;
x=_tmp304;
goto _LL196;}}else{_LL199: _LL19A: {
# 1420
static struct _dyneither_ptr bogus_string={_tmp305,_tmp305,_tmp305 + 8};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
# 1423
static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};
# 1429
x=& bogus_vardecl;
x->type=a_typ;}}_LL196:;}{
# 1432
void*_tmp306=a_typ;
union Cyc_Relations_RelnOp rop_a=Cyc_Relations_RNumelts(x);
# 1437
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound(_tmp306);
if(bound != 0){
union Cyc_Relations_RelnOp rop_bound=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(bound,& rop_bound))
relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_bound,Cyc_Relations_Rlte,rop_a,relns);}
# 1446
if(valid_rop_i){
struct Cyc_List_List*_tmp307=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlte,rop_i,relns);
# 1454
if(!Cyc_Relations_consistent_relations(_tmp307))return 1;}
# 1457
inner_loop: {
void*_tmp308=i->r;void*_tmp309=_tmp308;struct Cyc_Absyn_Exp*_tmp30A;struct Cyc_Absyn_Exp*_tmp30B;void*_tmp30C;struct Cyc_Absyn_Exp*_tmp30D;switch(*((int*)_tmp309)){case 13: _LL19C: _tmp30C=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp30D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_LL19D:
 i=_tmp30D;goto inner_loop;case 2: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp309)->f1 == Cyc_Absyn_Mod){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp309)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp309)->f2)->tl != 0){_LL19E: _tmp30A=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp309)->f2)->hd;_tmp30B=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp309)->f2)->tl)->hd;_LL19F: {
# 1463
union Cyc_Relations_RelnOp rop_z=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmp30B,& rop_z)){
# 1466
struct Cyc_List_List*_tmp30E=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlt,rop_z,relns);
# 1468
return !Cyc_Relations_consistent_relations(_tmp30E);}
# 1470
goto _LL19B;}}else{goto _LL1A0;}}else{goto _LL1A0;}}else{goto _LL1A0;}default: _LL1A0: _LL1A1:
 goto _LL19B;}_LL19B:;}
# 1473
return 0;};}
# 1476
static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp30F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp310="Missing type in primop ";_tag_dyneither(_tmp310,sizeof(char),24);}),_tag_dyneither(_tmp30F,sizeof(void*),0));});
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));}
# 1480
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp311=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp312="Missing type in primop ";_tag_dyneither(_tmp312,sizeof(char),24);}),_tag_dyneither(_tmp311,sizeof(void*),0));});
return(void*)_check_null(e->topt);}
# 1484
static struct _tuple11*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
return({struct _tuple11*_tmp313=_cycalloc(sizeof(*_tmp313));_tmp313->f1=Cyc_Toc_mt_tq;_tmp313->f2=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));_tmp313;});}
# 1487
static struct _tuple19*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
Cyc_Toc_exp_to_c(nv,e);
return({struct _tuple19*_tmp314=_cycalloc(sizeof(*_tmp314));_tmp314->f1=0;_tmp314->f2=e;_tmp314;});}
# 1494
static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic,int do_declare){
# 1497
struct Cyc_Absyn_Exp*eo;
void*t;
if(pointer){
struct Cyc_Absyn_Exp*_tmp315=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(x,0),0);
if(*(nv->struct_info).varsizeexp != 0){
_tmp315=Cyc_Absyn_add_exp(_tmp315,(struct Cyc_Absyn_Exp*)_check_null(*(nv->struct_info).varsizeexp),0);
*(nv->struct_info).varsizeexp=0;}
# 1505
t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);
if(rgnopt == 0  || Cyc_Absyn_no_regions)
eo=is_atomic?Cyc_Toc_malloc_atomic(_tmp315): Cyc_Toc_malloc_ptr(_tmp315);else{
# 1509
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);
eo=Cyc_Toc_rmalloc_exp(r,_tmp315);}}else{
# 1514
t=struct_typ;
eo=0;}
# 1517
if(do_declare){
void*_tmp316=x->r;void*_tmp317=_tmp316;void*_tmp318;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp317)->tag == 1){_LL1A3: _tmp318=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp317)->f1;_LL1A4:
# 1520
 return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(Cyc_Absyn_binding2qvar(_tmp318),t,eo,s,0),0);}else{_LL1A5: _LL1A6:
# 1522
({void*_tmp319=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp31A="bogus x to make_struct";_tag_dyneither(_tmp31A,sizeof(char),23);}),_tag_dyneither(_tmp319,sizeof(void*),0));});}_LL1A2:;}else{
# 1526
if(eo != 0)
return Cyc_Absyn_stmt_exp(Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(x,eo,0),0),s,0),0);else{
# 1529
return Cyc_Absyn_stmt_exp(s,0);}}}
# 1533
static int Cyc_Toc_abstract_aggr(void*typ){
void*_tmp31B=Cyc_Tcutil_compress(typ);void*_tmp31C=_tmp31B;struct Cyc_List_List*_tmp31D;union Cyc_Absyn_AggrInfoU _tmp31E;struct Cyc_List_List*_tmp31F;switch(*((int*)_tmp31C)){case 11: _LL1A8: _tmp31E=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp31C)->f1).aggr_info;_tmp31F=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp31C)->f1).targs;_LL1A9: {
# 1536
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(_tmp31E);
if(ad->impl == 0)return 0;{
struct Cyc_List_List*aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
_tmp31D=aggrfields;goto _LL1AB;};}case 12: _LL1AA: _tmp31D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp31C)->f2;_LL1AB:
# 1541
 if(_tmp31D == 0)return 0;
return 1;default: _LL1AC: _LL1AD:
# 1547
 return 0;}_LL1A7:;}
# 1551
static void*Cyc_Toc_array_elt_type(void*t){
void*_tmp320=Cyc_Tcutil_compress(t);void*_tmp321=_tmp320;void*_tmp322;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp321)->tag == 8){_LL1AF: _tmp322=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp321)->f1).elt_type;_LL1B0:
 return _tmp322;}else{_LL1B1: _LL1B2:
({struct Cyc_String_pa_PrintArg_struct _tmp325;_tmp325.tag=0;_tmp325.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp323[1]={& _tmp325};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp324="array_elt_type called on %s";_tag_dyneither(_tmp324,sizeof(char),28);}),_tag_dyneither(_tmp323,sizeof(void*),1));});});}_LL1AE:;}
# 1558
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated);
# 1564
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s);
# 1569
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,void*array_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){
# 1574
void*orig_elt_type=Cyc_Toc_array_elt_type(array_type);
int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;
{struct Cyc_List_List*_tmp326=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;_tmp326 != 0;_tmp326=_tmp326->tl){
# 1578
struct _tuple19*_tmp327=(struct _tuple19*)_tmp326->hd;struct _tuple19*_tmp328=_tmp327;struct Cyc_List_List*_tmp329;struct Cyc_Absyn_Exp*_tmp32A;_LL1B4: _tmp329=_tmp328->f1;_tmp32A=_tmp328->f2;_LL1B5:;{
# 1582
struct Cyc_Absyn_Exp*e_index;
if(_tmp329 == 0)
e_index=Cyc_Absyn_signed_int_exp(count --,0);else{
# 1586
if(_tmp329->tl != 0)({void*_tmp32B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp32C="multiple designators in array";_tag_dyneither(_tmp32C,sizeof(char),30);}),_tag_dyneither(_tmp32B,sizeof(void*),0));});{
void*_tmp32D=(void*)_tmp329->hd;
void*_tmp32E=_tmp32D;struct Cyc_Absyn_Exp*_tmp32F;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp32E)->tag == 0){_LL1B7: _tmp32F=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp32E)->f1;_LL1B8:
# 1590
 Cyc_Toc_exp_to_c(nv,_tmp32F);
e_index=_tmp32F;
goto _LL1B6;}else{_LL1B9: _LL1BA:
({void*_tmp330=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp331="field name designators in array";_tag_dyneither(_tmp331,sizeof(char),32);}),_tag_dyneither(_tmp330,sizeof(void*),0));});}_LL1B6:;};}{
# 1596
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);
void*_tmp332=_tmp32A->r;void*_tmp333=_tmp332;void*_tmp334;struct Cyc_List_List*_tmp335;struct Cyc_Absyn_Vardecl*_tmp336;struct Cyc_Absyn_Exp*_tmp337;struct Cyc_Absyn_Exp*_tmp338;int _tmp339;struct Cyc_List_List*_tmp33A;switch(*((int*)_tmp333)){case 25: _LL1BC: _tmp33A=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp333)->f1;_LL1BD:
# 1599
 s=Cyc_Toc_init_array(nv,orig_elt_type,lval,_tmp33A,s);
goto _LL1BB;case 26: _LL1BE: _tmp336=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp333)->f1;_tmp337=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp333)->f2;_tmp338=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp333)->f3;_tmp339=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp333)->f4;_LL1BF:
# 1602
 s=Cyc_Toc_init_comprehension(nv,orig_elt_type,lval,_tmp336,_tmp337,_tmp338,_tmp339,s,0);
# 1604
goto _LL1BB;case 29: _LL1C0: _tmp334=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp333)->f1;_tmp335=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp333)->f2;_LL1C1:
# 1606
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp334,_tmp335,s);
goto _LL1BB;case 27: _LL1C2: _LL1C3:
# 1609
 goto _LL1BB;default: _LL1C4: _LL1C5:
# 1611
 Cyc_Toc_exp_to_c(nv,_tmp32A);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(orig_elt_type),_tmp32A),0),s,0);
goto _LL1BB;}_LL1BB:;};};}}
# 1617
return s;}
# 1622
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated){
# 1627
struct _tuple1*_tmp33B=vd->name;
void*expected_elt_type;
{void*_tmp33C=Cyc_Tcutil_compress(comprehension_type);void*_tmp33D=_tmp33C;void*_tmp33E;void*_tmp33F;switch(*((int*)_tmp33D)){case 8: _LL1C7: _tmp33F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp33D)->f1).elt_type;_LL1C8:
# 1631
 expected_elt_type=_tmp33F;goto _LL1C6;case 5: _LL1C9: _tmp33E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp33D)->f1).elt_typ;_LL1CA:
# 1633
 expected_elt_type=_tmp33E;goto _LL1C6;default: _LL1CB: _LL1CC:
({void*_tmp340=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp341="init_comprehension passed a bad type!";_tag_dyneither(_tmp341,sizeof(char),38);}),_tag_dyneither(_tmp340,sizeof(void*),0));});}_LL1C6:;}{
# 1636
void*_tmp342=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));
if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);
# 1640
{void*_tmp343=e2->r;void*_tmp344=_tmp343;if(((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp344)->tag == 27){_LL1CE: _LL1CF:
 return Cyc_Absyn_skip_stmt(0);}else{_LL1D0: _LL1D1:
 goto _LL1CD;}_LL1CD:;}{
# 1644
struct _RegionHandle _tmp345=_new_region("r2");struct _RegionHandle*r2=& _tmp345;_push_region(r2);
{struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(r2,nv,_tmp33B,Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp350=_cycalloc(sizeof(*_tmp350));_tmp350[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp351;_tmp351.tag=4;_tmp351.f1=vd;_tmp351;});_tmp350;}),0));
struct _tuple1*max=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp33B,0),Cyc_Absyn_signed_int_exp(0,0),0);
struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp33B,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp33B,0),0);
# 1652
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp33B,0),0);
struct Cyc_Absyn_Stmt*body;
# 1656
{void*_tmp346=e2->r;void*_tmp347=_tmp346;void*_tmp348;struct Cyc_List_List*_tmp349;struct Cyc_Absyn_Vardecl*_tmp34A;struct Cyc_Absyn_Exp*_tmp34B;struct Cyc_Absyn_Exp*_tmp34C;int _tmp34D;struct Cyc_List_List*_tmp34E;switch(*((int*)_tmp347)){case 25: _LL1D3: _tmp34E=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp347)->f1;_LL1D4:
# 1658
 body=Cyc_Toc_init_array(nv2,expected_elt_type,lval,_tmp34E,Cyc_Toc_skip_stmt_dl());
goto _LL1D2;case 26: _LL1D5: _tmp34A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp347)->f1;_tmp34B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp347)->f2;_tmp34C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp347)->f3;_tmp34D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp347)->f4;_LL1D6:
# 1661
 body=Cyc_Toc_init_comprehension(nv2,expected_elt_type,lval,_tmp34A,_tmp34B,_tmp34C,_tmp34D,Cyc_Toc_skip_stmt_dl(),0);
goto _LL1D2;case 29: _LL1D7: _tmp348=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp347)->f1;_tmp349=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp347)->f2;_LL1D8:
# 1664
 body=Cyc_Toc_init_anon_struct(nv,lval,_tmp348,_tmp349,Cyc_Toc_skip_stmt_dl());
goto _LL1D2;default: _LL1D9: _LL1DA:
# 1667
 Cyc_Toc_exp_to_c(nv2,e2);
body=Cyc_Absyn_assign_stmt(lval,Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(expected_elt_type),e2),0);
goto _LL1D2;}_LL1D2:;}{
# 1671
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0);
# 1673
if(zero_term){
# 1678
struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(_tmp342,Cyc_Absyn_uint_exp(0,0)),0);
s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}{
# 1683
struct Cyc_Absyn_Stmt*_tmp34F=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,e1,
Cyc_Absyn_declare_stmt(_tmp33B,Cyc_Absyn_uint_typ,0,s2,0),0),s,0);_npop_handler(0);return _tmp34F;};};}
# 1645
;_pop_region(r2);};};}
# 1689
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s){
# 1692
{struct Cyc_List_List*_tmp352=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles);for(0;_tmp352 != 0;_tmp352=_tmp352->tl){
struct _tuple19*_tmp353=(struct _tuple19*)_tmp352->hd;struct _tuple19*_tmp354=_tmp353;struct Cyc_List_List*_tmp355;struct Cyc_Absyn_Exp*_tmp356;_LL1DC: _tmp355=_tmp354->f1;_tmp356=_tmp354->f2;_LL1DD:;
if(_tmp355 == 0)
({void*_tmp357=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp358="empty designator list";_tag_dyneither(_tmp358,sizeof(char),22);}),_tag_dyneither(_tmp357,sizeof(void*),0));});
if(_tmp355->tl != 0)
({void*_tmp359=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp35A="too many designators in anonymous struct";_tag_dyneither(_tmp35A,sizeof(char),41);}),_tag_dyneither(_tmp359,sizeof(void*),0));});{
void*_tmp35B=(void*)_tmp355->hd;void*_tmp35C=_tmp35B;struct _dyneither_ptr*_tmp35D;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp35C)->tag == 1){_LL1DF: _tmp35D=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp35C)->f1;_LL1E0: {
# 1700
struct Cyc_Absyn_Exp*lval=Cyc_Toc_member_exp(lhs,_tmp35D,0);
{void*_tmp35E=_tmp356->r;void*_tmp35F=_tmp35E;void*_tmp360;struct Cyc_List_List*_tmp361;struct Cyc_Absyn_Vardecl*_tmp362;struct Cyc_Absyn_Exp*_tmp363;struct Cyc_Absyn_Exp*_tmp364;int _tmp365;struct Cyc_List_List*_tmp366;switch(*((int*)_tmp35F)){case 25: _LL1E4: _tmp366=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp35F)->f1;_LL1E5:
# 1703
 s=Cyc_Toc_init_array(nv,(void*)_check_null(_tmp356->topt),lval,_tmp366,s);goto _LL1E3;case 26: _LL1E6: _tmp362=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp35F)->f1;_tmp363=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp35F)->f2;_tmp364=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp35F)->f3;_tmp365=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp35F)->f4;_LL1E7:
# 1705
 s=Cyc_Toc_init_comprehension(nv,(void*)_check_null(_tmp356->topt),lval,_tmp362,_tmp363,_tmp364,_tmp365,s,0);
goto _LL1E3;case 29: _LL1E8: _tmp360=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp35F)->f1;_tmp361=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp35F)->f2;_LL1E9:
# 1708
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp360,_tmp361,s);goto _LL1E3;case 27: _LL1EA: _LL1EB:
# 1710
 goto _LL1E3;default: _LL1EC: _LL1ED:
# 1712
 Cyc_Toc_exp_to_c(nv,_tmp356);
# 1714
if(Cyc_Toc_is_poly_field(struct_type,_tmp35D))
_tmp356=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp356);
# 1717
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp356,0),0),s,0);
goto _LL1E3;}_LL1E3:;}
# 1720
goto _LL1DE;}}else{_LL1E1: _LL1E2:
({void*_tmp367=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp368="array designator in struct";_tag_dyneither(_tmp368,sizeof(char),27);}),_tag_dyneither(_tmp367,sizeof(void*),0));});}_LL1DE:;};}}
# 1724
return s;}
# 1729
static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){
# 1733
struct _RegionHandle _tmp369=_new_region("r");struct _RegionHandle*r=& _tmp369;_push_region(r);
{struct Cyc_List_List*_tmp36A=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);
void*_tmp36B=Cyc_Toc_add_tuple_type(_tmp36A);
# 1737
struct _tuple1*_tmp36C=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp36D=Cyc_Absyn_var_exp(_tmp36C,0);
struct Cyc_Absyn_Stmt*_tmp36E=Cyc_Absyn_exp_stmt(_tmp36D,0);
# 1741
struct Cyc_Absyn_Exp*(*_tmp36F)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
# 1743
int is_atomic=1;
struct Cyc_List_List*_tmp370=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);
{int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp370);for(0;_tmp370 != 0;(_tmp370=_tmp370->tl,-- i)){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp370->hd;
struct Cyc_Absyn_Exp*lval=_tmp36F(_tmp36D,Cyc_Absyn_fieldname(i),0);
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(e->topt));{
void*_tmp371=e->r;void*_tmp372=_tmp371;struct Cyc_Absyn_Vardecl*_tmp373;struct Cyc_Absyn_Exp*_tmp374;struct Cyc_Absyn_Exp*_tmp375;int _tmp376;struct Cyc_List_List*_tmp377;switch(*((int*)_tmp372)){case 25: _LL1EF: _tmp377=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp372)->f1;_LL1F0:
# 1751
 _tmp36E=Cyc_Toc_init_array(nv,(void*)_check_null(e->topt),lval,_tmp377,_tmp36E);
goto _LL1EE;case 26: _LL1F1: _tmp373=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp372)->f1;_tmp374=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp372)->f2;_tmp375=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp372)->f3;_tmp376=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp372)->f4;_LL1F2:
# 1754
 _tmp36E=Cyc_Toc_init_comprehension(nv,(void*)_check_null(e->topt),lval,_tmp373,_tmp374,_tmp375,_tmp376,_tmp36E,0);
# 1756
goto _LL1EE;case 27: _LL1F3: _LL1F4:
# 1758
 goto _LL1EE;default: _LL1F5: _LL1F6:
# 1761
 Cyc_Toc_exp_to_c(nv,e);
_tmp36E=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp36F(_tmp36D,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp36E,0);
# 1764
goto _LL1EE;}_LL1EE:;};}}{
# 1767
struct Cyc_Absyn_Exp*_tmp378=Cyc_Toc_make_struct(nv,Cyc_Absyn_var_exp(_tmp36C,0),_tmp36B,_tmp36E,pointer,rgnopt,is_atomic,1);_npop_handler(0);return _tmp378;};}
# 1734
;_pop_region(r);}
# 1771
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp379=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp379 != 0;_tmp379=_tmp379->tl){
struct Cyc_Absyn_Aggrfield*_tmp37A=(struct Cyc_Absyn_Aggrfield*)_tmp379->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp37A->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}
# 1778
({void*_tmp37B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp37E;_tmp37E.tag=0;_tmp37E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp37C[1]={& _tmp37E};Cyc_aprintf(({const char*_tmp37D="get_member_offset %s failed";_tag_dyneither(_tmp37D,sizeof(char),28);}),_tag_dyneither(_tmp37C,sizeof(void*),1));});}),_tag_dyneither(_tmp37B,sizeof(void*),0));});}
# 1781
static int Cyc_Toc_init_variable_array(void**typ){
{void*_tmp37F=Cyc_Tcutil_compress(*typ);void*_tmp380=_tmp37F;void*_tmp381;struct Cyc_Absyn_Tqual _tmp382;struct Cyc_Absyn_Exp*_tmp383;union Cyc_Absyn_Constraint*_tmp384;unsigned int _tmp385;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp380)->tag == 8){_LL1F8: _tmp381=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp380)->f1).elt_type;_tmp382=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp380)->f1).tq;_tmp383=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp380)->f1).num_elts;_tmp384=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp380)->f1).zero_term;_tmp385=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp380)->f1).zt_loc;_LL1F9:
# 1786
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp383))){
*typ=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp386=_cycalloc(sizeof(*_tmp386));_tmp386[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp387;_tmp387.tag=8;_tmp387.f1=({struct Cyc_Absyn_ArrayInfo _tmp388;_tmp388.elt_type=_tmp381;_tmp388.tq=_tmp382;_tmp388.num_elts=Cyc_Absyn_uint_exp(0,0);_tmp388.zero_term=_tmp384;_tmp388.zt_loc=_tmp385;_tmp388;});_tmp387;});_tmp386;});
return 1;}
# 1790
goto _LL1F7;}else{_LL1FA: _LL1FB:
 goto _LL1F7;}_LL1F7:;}
# 1793
return 0;}
# 1796
static int Cyc_Toc_is_array_type(void*t){
void*_tmp389=Cyc_Tcutil_compress(t);void*_tmp38A=_tmp389;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp38A)->tag == 8){_LL1FD: _LL1FE:
 return 1;}else{_LL1FF: _LL200:
 return 0;}_LL1FC:;}
# 1803
struct Cyc_Absyn_Aggrdecl*Cyc_Toc_update_aggr_type(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*fn,void*new_field_type){
# 1805
struct Cyc_List_List*new_fields=0;
struct Cyc_List_List*fields;
for(fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;fields != 0;fields=fields->tl){
# 1809
if(!Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,fn)){
struct Cyc_Absyn_Aggrfield*_tmp38B=(struct Cyc_Absyn_Aggrfield*)fields->hd;
struct Cyc_List_List*_tmp38C=_tmp38B->attributes;
void*_tmp38D=_tmp38B->type;
if((!Cyc_Toc_is_array_type(_tmp38D),Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp38D))))
_tmp38C=({struct Cyc_List_List*_tmp38E=_cycalloc(sizeof(*_tmp38E));_tmp38E->hd=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp38F=_cycalloc(sizeof(*_tmp38F));_tmp38F[0]=({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmp390;_tmp390.tag=6;_tmp390.f1=0;_tmp390;});_tmp38F;});_tmp38E->tl=_tmp38C;_tmp38E;});{
struct Cyc_Absyn_Aggrfield*_tmp391=({struct Cyc_Absyn_Aggrfield*_tmp393=_cycalloc(sizeof(*_tmp393));_tmp393->name=_tmp38B->name;_tmp393->tq=Cyc_Toc_mt_tq;_tmp393->type=new_field_type;_tmp393->width=_tmp38B->width;_tmp393->attributes=_tmp38C;_tmp393->requires_clause=0;_tmp393;});
# 1821
new_fields=({struct Cyc_List_List*_tmp392=_cycalloc(sizeof(*_tmp392));_tmp392->hd=_tmp391;_tmp392->tl=new_fields;_tmp392;});};}else{
# 1823
new_fields=({struct Cyc_List_List*_tmp394=_cycalloc(sizeof(*_tmp394));_tmp394->hd=(struct Cyc_Absyn_Aggrfield*)fields->hd;_tmp394->tl=new_fields;_tmp394;});}}{
# 1825
struct Cyc_List_List*_tmp395=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);
struct _dyneither_ptr*name=({struct _dyneither_ptr*_tmp39A=_cycalloc(sizeof(*_tmp39A));_tmp39A[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp39D;_tmp39D.tag=1;_tmp39D.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp39B[1]={& _tmp39D};Cyc_aprintf(({const char*_tmp39C="_genStruct%d";_tag_dyneither(_tmp39C,sizeof(char),13);}),_tag_dyneither(_tmp39B,sizeof(void*),1));});});_tmp39A;});
# 1828
struct _tuple1*qv=({struct _tuple1*_tmp399=_cycalloc(sizeof(*_tmp399));_tmp399->f1=Cyc_Absyn_Abs_n(0,1);_tmp399->f2=name;_tmp399;});
struct Cyc_Absyn_AggrdeclImpl*_tmp396=({struct Cyc_Absyn_AggrdeclImpl*_tmp398=_cycalloc(sizeof(*_tmp398));_tmp398->exist_vars=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars;_tmp398->rgn_po=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;_tmp398->fields=_tmp395;_tmp398->tagged=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;_tmp398;});
# 1833
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp397=_cycalloc(sizeof(*_tmp397));_tmp397->kind=ad->kind;_tmp397->sc=ad->sc;_tmp397->name=qv;_tmp397->tvs=ad->tvs;_tmp397->impl=_tmp396;_tmp397->expected_mem_kind=0;_tmp397->attributes=ad->attributes;_tmp397;});
# 1840
Cyc_Toc_aggrdecl_to_c(new_ad,1);
return new_ad;};}
# 1844
static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*nv,void*struct_type,struct Cyc_List_List*exist_types,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*dles,struct _tuple1*tdn){
# 1850
int do_declare=0;
struct Cyc_Absyn_Exp*xexp;
if((nv->struct_info).lhs_exp == 0){
struct _tuple1*_tmp39E=Cyc_Toc_temp_var();
xexp=Cyc_Absyn_var_exp(_tmp39E,0);
do_declare=1;}else{
# 1858
xexp=(struct Cyc_Absyn_Exp*)_check_null((nv->struct_info).lhs_exp);
nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);}{
# 1861
struct Cyc_Absyn_Stmt*_tmp39F=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(xexp),0);
# 1865
struct Cyc_Absyn_Exp**varexp;
{void*_tmp3A0=_tmp39F->r;void*_tmp3A1=_tmp3A0;struct Cyc_Absyn_Exp**_tmp3A2;if(((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3A1)->tag == 1){_LL202: _tmp3A2=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3A1)->f1;_LL203:
 varexp=_tmp3A2;goto _LL201;}else{_LL204: _LL205:
({void*_tmp3A3=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3A4="stmt not an expression!";_tag_dyneither(_tmp3A4,sizeof(char),24);}),_tag_dyneither(_tmp3A3,sizeof(void*),0));});}_LL201:;}{
# 1871
struct Cyc_Absyn_Exp*(*_tmp3A5)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
void*aggr_typ=Cyc_Toc_typ_to_c(struct_type);
void*orig_typ=aggr_typ;
# 1875
int is_atomic=1;
struct Cyc_List_List*forall_types;
struct Cyc_Absyn_Aggrdecl*ad;
struct Cyc_List_List*aggrfields;struct Cyc_List_List*orig_aggrfields;
struct _dyneither_ptr**index=0;
{void*_tmp3A6=Cyc_Tcutil_compress(struct_type);void*_tmp3A7=_tmp3A6;union Cyc_Absyn_AggrInfoU _tmp3A8;struct Cyc_List_List*_tmp3A9;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3A7)->tag == 11){_LL207: _tmp3A8=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3A7)->f1).aggr_info;_tmp3A9=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3A7)->f1).targs;_LL208:
# 1882
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp3A8);
orig_aggrfields=(aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
forall_types=_tmp3A9;
goto _LL206;}else{_LL209: _LL20A:
({void*_tmp3AA=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3AB="init_struct: bad struct type";_tag_dyneither(_tmp3AB,sizeof(char),29);}),_tag_dyneither(_tmp3AA,sizeof(void*),0));});}_LL206:;}
# 1889
if(exist_types != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields != 0){
struct Cyc_List_List*_tmp3AC=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
for(0;_tmp3AC->tl != 0;_tmp3AC=_tmp3AC->tl){;}{
struct Cyc_Absyn_Aggrfield*_tmp3AD=(struct Cyc_Absyn_Aggrfield*)_tmp3AC->hd;
struct _RegionHandle _tmp3AE=_new_region("temp");struct _RegionHandle*temp=& _tmp3AE;_push_region(temp);
{struct Cyc_List_List*_tmp3AF=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,forall_types);
# 1898
if(Cyc_Toc_is_abstract_type(Cyc_Tcutil_rsubstitute(temp,_tmp3AF,_tmp3AD->type))){
struct Cyc_List_List*_tmp3B0=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,exist_types);
struct Cyc_List_List*_tmp3B1=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(temp,_tmp3AF,_tmp3B0);
# 1902
struct Cyc_List_List*new_fields=0;
for(_tmp3AC=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;_tmp3AC != 0;_tmp3AC=_tmp3AC->tl){
struct Cyc_Absyn_Aggrfield*_tmp3B2=(struct Cyc_Absyn_Aggrfield*)_tmp3AC->hd;
struct Cyc_Absyn_Aggrfield*_tmp3B3=({struct Cyc_Absyn_Aggrfield*_tmp3B6=_cycalloc(sizeof(*_tmp3B6));_tmp3B6->name=_tmp3B2->name;_tmp3B6->tq=Cyc_Toc_mt_tq;_tmp3B6->type=
# 1907
Cyc_Tcutil_rsubstitute(temp,_tmp3B1,_tmp3B2->type);_tmp3B6->width=_tmp3B2->width;_tmp3B6->attributes=_tmp3B2->attributes;_tmp3B6->requires_clause=0;_tmp3B6;});
# 1915
if(_tmp3AC->tl == 0){
if(Cyc_Toc_init_variable_array(& _tmp3B3->type))
index=({struct _dyneither_ptr**_tmp3B4=_cycalloc(sizeof(*_tmp3B4));_tmp3B4[0]=_tmp3B3->name;_tmp3B4;});}
# 1922
new_fields=({struct Cyc_List_List*_tmp3B5=_cycalloc(sizeof(*_tmp3B5));_tmp3B5->hd=_tmp3B3;_tmp3B5->tl=new_fields;_tmp3B5;});}
# 1924
exist_types=0;
aggrfields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);{
struct _dyneither_ptr*name=({struct _dyneither_ptr*_tmp3BF=_cycalloc(sizeof(*_tmp3BF));_tmp3BF[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3C2;_tmp3C2.tag=1;_tmp3C2.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;({void*_tmp3C0[1]={& _tmp3C2};Cyc_aprintf(({const char*_tmp3C1="_genStruct%d";_tag_dyneither(_tmp3C1,sizeof(char),13);}),_tag_dyneither(_tmp3C0,sizeof(void*),1));});});_tmp3BF;});
# 1928
struct _tuple1*qv=({struct _tuple1*_tmp3BE=_cycalloc(sizeof(*_tmp3BE));_tmp3BE->f1=Cyc_Absyn_Abs_n(0,1);_tmp3BE->f2=name;_tmp3BE;});
struct Cyc_Absyn_AggrdeclImpl*_tmp3B7=({struct Cyc_Absyn_AggrdeclImpl*_tmp3BD=_cycalloc(sizeof(*_tmp3BD));_tmp3BD->exist_vars=0;_tmp3BD->rgn_po=0;_tmp3BD->fields=aggrfields;_tmp3BD->tagged=0;_tmp3BD;});
# 1933
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp3BC=_cycalloc(sizeof(*_tmp3BC));_tmp3BC->kind=Cyc_Absyn_StructA;_tmp3BC->sc=Cyc_Absyn_Public;_tmp3BC->name=qv;_tmp3BC->tvs=0;_tmp3BC->expected_mem_kind=0;_tmp3BC->impl=_tmp3B7;_tmp3BC->attributes=0;_tmp3BC;});
# 1940
Cyc_Toc_aggrdecl_to_c(new_ad,1);
ad=new_ad;
struct_type=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3B8=_cycalloc(sizeof(*_tmp3B8));_tmp3B8[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp3B9;_tmp3B9.tag=11;_tmp3B9.f1=({struct Cyc_Absyn_AggrInfo _tmp3BA;_tmp3BA.aggr_info=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp3BB=_cycalloc(sizeof(*_tmp3BB));_tmp3BB[0]=new_ad;_tmp3BB;}));_tmp3BA.targs=0;_tmp3BA;});_tmp3B9;});_tmp3B8;});
# 1944
aggr_typ=Cyc_Toc_typ_to_c(struct_type);};}}
# 1894
;_pop_region(temp);};}{
# 1948
int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct _RegionHandle _tmp3C3=_new_region("r");struct _RegionHandle*r=& _tmp3C3;_push_region(r);
{struct Cyc_List_List*_tmp3C4=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp3C4 != 0;_tmp3C4=_tmp3C4->tl){
struct _tuple19*_tmp3C5=(struct _tuple19*)_tmp3C4->hd;struct _tuple19*_tmp3C6=_tmp3C5;struct Cyc_List_List*_tmp3C7;struct Cyc_Absyn_Exp*_tmp3C8;_LL20C: _tmp3C7=_tmp3C6->f1;_tmp3C8=_tmp3C6->f2;_LL20D:;
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(_tmp3C8->topt));
if(_tmp3C7 == 0)
({void*_tmp3C9=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3CA="empty designator list";_tag_dyneither(_tmp3CA,sizeof(char),22);}),_tag_dyneither(_tmp3C9,sizeof(void*),0));});
if(_tmp3C7->tl != 0){
# 1958
struct _tuple1*_tmp3CB=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3CC=Cyc_Absyn_var_exp(_tmp3CB,0);
for(0;_tmp3C7 != 0;_tmp3C7=_tmp3C7->tl){
void*_tmp3CD=(void*)_tmp3C7->hd;void*_tmp3CE=_tmp3CD;struct _dyneither_ptr*_tmp3CF;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3CE)->tag == 1){_LL20F: _tmp3CF=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3CE)->f1;_LL210:
# 1964
 if(Cyc_Toc_is_poly_field(struct_type,_tmp3CF))
_tmp3CC=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3CC);
_tmp39F=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3A5(xexp,_tmp3CF,0),_tmp3CC,0),0),_tmp39F,0);
# 1968
goto _LL20E;}else{_LL211: _LL212:
({void*_tmp3D0=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3D1="array designator in struct";_tag_dyneither(_tmp3D1,sizeof(char),27);}),_tag_dyneither(_tmp3D0,sizeof(void*),0));});}_LL20E:;}
# 1972
Cyc_Toc_exp_to_c(nv,_tmp3C8);
_tmp39F=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3CC,_tmp3C8,0),0),_tmp39F,0);}else{
# 1975
void*_tmp3D2=(void*)_tmp3C7->hd;void*_tmp3D3=_tmp3D2;struct _dyneither_ptr*_tmp3D4;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3D3)->tag == 1){_LL214: _tmp3D4=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3D3)->f1;_LL215: {
# 1977
struct Cyc_Absyn_Exp*lval=_tmp3A5(xexp,_tmp3D4,0);
if(is_tagged_union){
int i=Cyc_Toc_get_member_offset(ad,_tmp3D4);
struct Cyc_Absyn_Exp*f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct Cyc_Absyn_Exp*lhs=Cyc_Toc_member_exp(lval,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,f_tag_exp,0);
_tmp39F=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp39F,0);
lval=Cyc_Toc_member_exp(lval,Cyc_Toc_val_sp,0);}{
# 1986
int e1_translated=0;
{void*_tmp3D5=_tmp3C8->r;void*_tmp3D6=_tmp3D5;void*_tmp3D7;struct Cyc_List_List*_tmp3D8;struct Cyc_Absyn_Exp*_tmp3D9;void*_tmp3DA;struct Cyc_Absyn_Vardecl*_tmp3DB;struct Cyc_Absyn_Exp*_tmp3DC;struct Cyc_Absyn_Exp*_tmp3DD;int _tmp3DE;struct Cyc_List_List*_tmp3DF;switch(*((int*)_tmp3D6)){case 25: _LL219: _tmp3DF=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3D6)->f1;_LL21A:
# 1989
 _tmp39F=Cyc_Toc_init_array(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(aggrfields,_tmp3D4)))->type,lval,_tmp3DF,_tmp39F);
# 1991
goto _LL218;case 26: _LL21B: _tmp3DB=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3D6)->f1;_tmp3DC=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3D6)->f2;_tmp3DD=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3D6)->f3;_tmp3DE=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3D6)->f4;_LL21C:
# 1993
 _tmp39F=Cyc_Toc_init_comprehension(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(do_declare?aggrfields: orig_aggrfields,_tmp3D4)))->type,lval,_tmp3DB,_tmp3DC,_tmp3DD,_tmp3DE,_tmp39F,0);
# 1997
e1_translated=1;
_tmp3D9=_tmp3DC;_tmp3DA=(void*)_check_null(_tmp3DD->topt);goto _LL21E;case 27: _LL21D: _tmp3D9=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp3D6)->f1;_tmp3DA=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp3D6)->f2;_LL21E:
# 2002
 if(index != 0  && !Cyc_strptrcmp(*index,_tmp3D4)){
if(!e1_translated)Cyc_Toc_exp_to_c(nv,_tmp3D9);{
void*_tmp3E0=Cyc_Toc_typ_to_c(_tmp3DA);
*(nv->struct_info).varsizeexp=
Cyc_Absyn_add_exp(Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp3E1[2];_tmp3E1[1]=
Cyc_Absyn_sizeoftyp_exp(_tmp3E0,0);_tmp3E1[0]=_tmp3D9;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3E1,sizeof(struct Cyc_Absyn_Exp*),2));}),0),
Cyc_Absyn_const_exp(({union Cyc_Absyn_Cnst _tmp3E2;(_tmp3E2.Int_c).val=({struct _tuple6 _tmp3E3;_tmp3E3.f1=Cyc_Absyn_Unsigned;_tmp3E3.f2=(int)sizeof(double);_tmp3E3;});(_tmp3E2.Int_c).tag=5;_tmp3E2;}),0),0);};}
# 2010
goto _LL218;case 29: _LL21F: _tmp3D7=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3D6)->f1;_tmp3D8=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3D6)->f2;_LL220:
# 2012
 _tmp39F=Cyc_Toc_init_anon_struct(nv,lval,_tmp3D7,_tmp3D8,_tmp39F);goto _LL218;default: _LL221: _LL222: {
# 2014
void*old_e_typ=(void*)_check_null(_tmp3C8->topt);
int was_ptr_type=Cyc_Toc_is_pointer_or_boxed_tvar(old_e_typ);
Cyc_Toc_exp_to_c(Cyc_Toc_set_lhs_exp(nv->rgn,lval,nv),_tmp3C8);
{void*_tmp3E4=old_e_typ;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3E4)->tag == 11){_LL224: _LL225:
# 2021
 if(old_e_typ != _tmp3C8->topt  && !Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(old_e_typ),(void*)_check_null(_tmp3C8->topt))){
# 2025
ad=Cyc_Toc_update_aggr_type(ad,_tmp3D4,(void*)_check_null(_tmp3C8->topt));
struct_type=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3E5=_cycalloc(sizeof(*_tmp3E5));_tmp3E5[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp3E6;_tmp3E6.tag=11;_tmp3E6.f1=({struct Cyc_Absyn_AggrInfo _tmp3E7;_tmp3E7.aggr_info=
Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp3E8=_cycalloc(sizeof(*_tmp3E8));_tmp3E8[0]=ad;_tmp3E8;}));_tmp3E7.targs=0;_tmp3E7;});_tmp3E6;});_tmp3E5;});
# 2029
aggr_typ=Cyc_Toc_typ_to_c(struct_type);}
# 2031
goto _LL223;}else{_LL226: _LL227:
 goto _LL223;}_LL223:;}{
# 2034
struct Cyc_Absyn_Aggrfield*_tmp3E9=Cyc_Absyn_lookup_field(aggrfields,_tmp3D4);
# 2036
if(Cyc_Toc_is_poly_field(struct_type,_tmp3D4) && !was_ptr_type)
_tmp3C8=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3C8);
# 2039
if(exist_types != 0)
_tmp3C8=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3E9))->type),_tmp3C8);
# 2042
_tmp39F=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp3C8,0),0),_tmp39F,0);
goto _LL218;};}}_LL218:;}
# 2045
goto _LL213;};}}else{_LL216: _LL217:
({void*_tmp3EA=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3EB="array designator in struct";_tag_dyneither(_tmp3EB,sizeof(char),27);}),_tag_dyneither(_tmp3EA,sizeof(void*),0));});}_LL213:;}}}
# 2052
if(aggr_typ != orig_typ  && pointer){
void*cast_type=Cyc_Absyn_cstar_typ(orig_typ,Cyc_Toc_mt_tq);
*varexp=Cyc_Toc_cast_it(cast_type,*varexp);}{
# 2057
struct Cyc_Absyn_Exp*_tmp3EC=Cyc_Toc_make_struct(nv,xexp,aggr_typ,_tmp39F,pointer,rgnopt,is_atomic,do_declare);
# 2059
_tmp3EC->topt=struct_type;{
struct Cyc_Absyn_Exp*_tmp3ED=_tmp3EC;_npop_handler(0);return _tmp3ED;};};
# 1950
;_pop_region(r);};};};}struct _tuple27{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 2065
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple27*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
# 2068
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0);}
# 2071
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp3EE=_cycalloc(sizeof(*_tmp3EE));_tmp3EE[0]=({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmp3EF;_tmp3EF.tag=4;_tmp3EF.f1=e1;_tmp3EF.f2=incr;_tmp3EF;});_tmp3EE;}),0);}
# 2075
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 2084
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 2086
void*_tmp3F0=e1->r;void*_tmp3F1=_tmp3F0;struct Cyc_Absyn_Exp*_tmp3F2;struct _dyneither_ptr*_tmp3F3;int _tmp3F4;int _tmp3F5;void*_tmp3F6;struct Cyc_Absyn_Exp*_tmp3F7;struct Cyc_Absyn_Stmt*_tmp3F8;switch(*((int*)_tmp3F1)){case 36: _LL229: _tmp3F8=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3F1)->f1;_LL22A:
 Cyc_Toc_lvalue_assign_stmt(_tmp3F8,fs,f,f_env);goto _LL228;case 13: _LL22B: _tmp3F6=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3F1)->f1;_tmp3F7=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3F1)->f2;_LL22C:
 Cyc_Toc_lvalue_assign(_tmp3F7,fs,f,f_env);goto _LL228;case 20: _LL22D: _tmp3F2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3F1)->f1;_tmp3F3=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3F1)->f2;_tmp3F4=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3F1)->f3;_tmp3F5=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3F1)->f4;_LL22E:
# 2091
 e1->r=_tmp3F2->r;
Cyc_Toc_lvalue_assign(e1,({struct Cyc_List_List*_tmp3F9=_cycalloc(sizeof(*_tmp3F9));_tmp3F9->hd=_tmp3F3;_tmp3F9->tl=fs;_tmp3F9;}),f,f_env);
goto _LL228;default: _LL22F: _LL230: {
# 2099
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 2101
for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;
goto _LL228;}}_LL228:;}
# 2107
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 2109
void*_tmp3FA=s->r;void*_tmp3FB=_tmp3FA;struct Cyc_Absyn_Stmt*_tmp3FC;struct Cyc_Absyn_Decl*_tmp3FD;struct Cyc_Absyn_Stmt*_tmp3FE;struct Cyc_Absyn_Exp*_tmp3FF;switch(*((int*)_tmp3FB)){case 1: _LL232: _tmp3FF=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3FB)->f1;_LL233:
 Cyc_Toc_lvalue_assign(_tmp3FF,fs,f,f_env);goto _LL231;case 12: _LL234: _tmp3FD=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3FB)->f1;_tmp3FE=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3FB)->f2;_LL235:
# 2112
 Cyc_Toc_lvalue_assign_stmt(_tmp3FE,fs,f,f_env);goto _LL231;case 2: _LL236: _tmp3FC=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3FB)->f2;_LL237:
 Cyc_Toc_lvalue_assign_stmt(_tmp3FC,fs,f,f_env);goto _LL231;default: _LL238: _LL239:
({struct Cyc_String_pa_PrintArg_struct _tmp402;_tmp402.tag=0;_tmp402.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));({void*_tmp400[1]={& _tmp402};Cyc_Toc_toc_impos(({const char*_tmp401="lvalue_assign_stmt: %s";_tag_dyneither(_tmp401,sizeof(char),23);}),_tag_dyneither(_tmp400,sizeof(void*),1));});});}_LL231:;}
# 2118
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 2122
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp403=e->r;void*_tmp404=_tmp403;struct Cyc_Absyn_Stmt*_tmp405;struct Cyc_Absyn_Exp*_tmp406;void**_tmp407;struct Cyc_Absyn_Exp**_tmp408;switch(*((int*)_tmp404)){case 13: _LL23B: _tmp407=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp404)->f1;_tmp408=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp404)->f2;_LL23C:
# 2125
*_tmp408=Cyc_Toc_push_address_exp(*_tmp408);
*_tmp407=Cyc_Absyn_cstar_typ(*_tmp407,Cyc_Toc_mt_tq);
return e;case 19: _LL23D: _tmp406=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp404)->f1;_LL23E:
# 2129
 return _tmp406;case 36: _LL23F: _tmp405=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp404)->f1;_LL240:
# 2133
 Cyc_Toc_push_address_stmt(_tmp405);
return e;default: _LL241: _LL242:
# 2136
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);
({struct Cyc_String_pa_PrintArg_struct _tmp40B;_tmp40B.tag=0;_tmp40B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp409[1]={& _tmp40B};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp40A="can't take & of exp %s";_tag_dyneither(_tmp40A,sizeof(char),23);}),_tag_dyneither(_tmp409,sizeof(void*),1));});});}_LL23A:;}
# 2141
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp40C=s->r;void*_tmp40D=_tmp40C;struct Cyc_Absyn_Exp**_tmp40E;struct Cyc_Absyn_Stmt*_tmp40F;struct Cyc_Absyn_Stmt*_tmp410;switch(*((int*)_tmp40D)){case 2: _LL244: _tmp410=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp40D)->f2;_LL245:
 _tmp40F=_tmp410;goto _LL247;case 12: _LL246: _tmp40F=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp40D)->f2;_LL247:
 Cyc_Toc_push_address_stmt(_tmp40F);goto _LL243;case 1: _LL248: _tmp40E=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp40D)->f1;_LL249:
*_tmp40E=Cyc_Toc_push_address_exp(*_tmp40E);goto _LL243;default: _LL24A: _LL24B:
# 2147
({struct Cyc_String_pa_PrintArg_struct _tmp413;_tmp413.tag=0;_tmp413.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));({void*_tmp411[1]={& _tmp413};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp412="can't take & of stmt %s";_tag_dyneither(_tmp412,sizeof(char),24);}),_tag_dyneither(_tmp411,sizeof(void*),1));});});}_LL243:;}
# 2152
static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 2154
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 2156
if(x == 0)return 0;
result=({struct Cyc_List_List*_tmp414=_region_malloc(r2,sizeof(*_tmp414));_tmp414->hd=f(x->hd,env);_tmp414->tl=0;_tmp414;});
prev=result;
for(x=x->tl;x != 0;x=x->tl){
((struct Cyc_List_List*)_check_null(prev))->tl=({struct Cyc_List_List*_tmp415=_region_malloc(r2,sizeof(*_tmp415));_tmp415->hd=f(x->hd,env);_tmp415->tl=0;_tmp415;});
prev=prev->tl;}
# 2163
return result;}
# 2165
static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,x);}
# 2169
static struct _tuple19*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){
return({struct _tuple19*_tmp416=_cycalloc(sizeof(*_tmp416));_tmp416->f1=0;_tmp416->f2=e;_tmp416;});}
# 2173
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_tmp417=Cyc_Tcutil_compress(t);void*_tmp418=_tmp417;struct Cyc_Absyn_PtrInfo _tmp419;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp418)->tag == 5){_LL24D: _tmp419=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp418)->f1;_LL24E:
 return _tmp419;}else{_LL24F: _LL250:
({void*_tmp41A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp41B="get_ptr_typ: not a pointer!";_tag_dyneither(_tmp41B,sizeof(char),28);}),_tag_dyneither(_tmp41A,sizeof(void*),0));});}_LL24C:;}
# 2183
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp41C=Cyc_Tcutil_compress(t);void*_tmp41D=_tmp41C;int _tmp41E;enum Cyc_Absyn_Sign _tmp41F;enum Cyc_Absyn_Sign _tmp420;enum Cyc_Absyn_Sign _tmp421;enum Cyc_Absyn_Sign _tmp422;enum Cyc_Absyn_Sign _tmp423;switch(*((int*)_tmp41D)){case 5: _LL252: _LL253:
 res=Cyc_Absyn_null_exp(0);goto _LL251;case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp41D)->f2){case Cyc_Absyn_Char_sz: _LL254: _tmp423=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp41D)->f1;_LL255:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp423,'\000'),0);goto _LL251;case Cyc_Absyn_Short_sz: _LL256: _tmp422=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp41D)->f1;_LL257:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp422,0),0);goto _LL251;case Cyc_Absyn_Int_sz: _LL25C: _tmp421=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp41D)->f1;_LL25D:
# 2191
 _tmp420=_tmp421;goto _LL25F;case Cyc_Absyn_Long_sz: _LL25E: _tmp420=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp41D)->f1;_LL25F:
# 2193
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp420,0),0);goto _LL251;default: _LL260: _tmp41F=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp41D)->f1;_LL261:
# 2195
 res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp41F,(long long)0),0);goto _LL251;}case 13: _LL258: _LL259:
# 2189
 goto _LL25B;case 14: _LL25A: _LL25B:
 _tmp421=Cyc_Absyn_Unsigned;goto _LL25D;case 7: switch(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp41D)->f1){case 0: _LL262: _LL263:
# 2196
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp424="0.0F";_tag_dyneither(_tmp424,sizeof(char),5);}),0),0);goto _LL251;case 1: _LL264: _LL265:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp425="0.0";_tag_dyneither(_tmp425,sizeof(char),4);}),1),0);goto _LL251;default: _LL266: _tmp41E=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp41D)->f1;_LL267:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp426="0.0L";_tag_dyneither(_tmp426,sizeof(char),5);}),_tmp41E),0);goto _LL251;}default: _LL268: _LL269:
# 2200
({struct Cyc_String_pa_PrintArg_struct _tmp429;_tmp429.tag=0;_tmp429.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp427[1]={& _tmp429};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp428="found non-zero type %s in generate_zero";_tag_dyneither(_tmp428,sizeof(char),40);}),_tag_dyneither(_tmp427,sizeof(void*),1));});});}_LL251:;}
# 2202
res->topt=t;
return res;}
# 2209
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 2222 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp42A=Cyc_Toc_typ_to_c(elt_type);
void*_tmp42B=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp42C=Cyc_Absyn_cstar_typ(_tmp42A,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmp42D=({struct Cyc_Core_Opt*_tmp465=_cycalloc(sizeof(*_tmp465));_tmp465->v=_tmp42C;_tmp465;});
struct Cyc_Absyn_Exp*xinit;
{void*_tmp42E=e1->r;void*_tmp42F=_tmp42E;struct Cyc_Absyn_Exp*_tmp430;struct Cyc_Absyn_Exp*_tmp431;struct Cyc_Absyn_Exp*_tmp432;switch(*((int*)_tmp42F)){case 19: _LL26B: _tmp432=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp42F)->f1;_LL26C:
# 2230
 if(!is_dyneither){
_tmp432=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp432,0,Cyc_Absyn_Other_coercion,0);
_tmp432->topt=fat_ptr_type;}
# 2234
Cyc_Toc_exp_to_c(nv,_tmp432);xinit=_tmp432;goto _LL26A;case 22: _LL26D: _tmp430=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp42F)->f1;_tmp431=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp42F)->f2;_LL26E:
# 2236
 if(!is_dyneither){
_tmp430=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp430,0,Cyc_Absyn_Other_coercion,0);
_tmp430->topt=fat_ptr_type;}
# 2240
Cyc_Toc_exp_to_c(nv,_tmp430);Cyc_Toc_exp_to_c(nv,_tmp431);
xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp433[3];_tmp433[2]=_tmp431;_tmp433[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0);_tmp433[0]=_tmp430;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp433,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
goto _LL26A;default: _LL26F: _LL270:
({void*_tmp434=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp435="found bad lhs for zero-terminated pointer assignment";_tag_dyneither(_tmp435,sizeof(char),53);}),_tag_dyneither(_tmp434,sizeof(void*),0));});}_LL26A:;}{
# 2246
struct _tuple1*_tmp436=Cyc_Toc_temp_var();
struct _RegionHandle _tmp437=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp437;_push_region(rgn2);
{struct Cyc_Toc_Env*_tmp438=Cyc_Toc_add_varmap(rgn2,nv,_tmp436,Cyc_Absyn_var_exp(_tmp436,0));
struct Cyc_Absyn_Vardecl*_tmp439=({struct Cyc_Absyn_Vardecl*_tmp464=_cycalloc(sizeof(*_tmp464));_tmp464->sc=Cyc_Absyn_Public;_tmp464->name=_tmp436;_tmp464->tq=Cyc_Toc_mt_tq;_tmp464->type=_tmp42B;_tmp464->initializer=xinit;_tmp464->rgn=0;_tmp464->attributes=0;_tmp464->escapes=0;_tmp464;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp43A=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp462=_cycalloc(sizeof(*_tmp462));_tmp462[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp463;_tmp463.tag=4;_tmp463.f1=_tmp439;_tmp463;});_tmp462;});
struct Cyc_Absyn_Exp*_tmp43B=Cyc_Absyn_varb_exp((void*)_tmp43A,0);
_tmp43B->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp43C=Cyc_Absyn_deref_exp(_tmp43B,0);
_tmp43C->topt=elt_type;
Cyc_Toc_exp_to_c(_tmp438,_tmp43C);{
struct _tuple1*_tmp43D=Cyc_Toc_temp_var();
_tmp438=Cyc_Toc_add_varmap(rgn2,_tmp438,_tmp43D,Cyc_Absyn_var_exp(_tmp43D,0));{
struct Cyc_Absyn_Vardecl*_tmp43E=({struct Cyc_Absyn_Vardecl*_tmp461=_cycalloc(sizeof(*_tmp461));_tmp461->sc=Cyc_Absyn_Public;_tmp461->name=_tmp43D;_tmp461->tq=Cyc_Toc_mt_tq;_tmp461->type=_tmp42A;_tmp461->initializer=_tmp43C;_tmp461->rgn=0;_tmp461->attributes=0;_tmp461->escapes=0;_tmp461;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp43F=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp45F=_cycalloc(sizeof(*_tmp45F));_tmp45F[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp460;_tmp460.tag=4;_tmp460.f1=_tmp43E;_tmp460;});_tmp45F;});
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp440=Cyc_Absyn_varb_exp((void*)_tmp43F,0);
_tmp440->topt=_tmp43C->topt;
z_init=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp440,e2,0);
z_init->topt=_tmp440->topt;}
# 2267
Cyc_Toc_exp_to_c(_tmp438,z_init);{
struct _tuple1*_tmp441=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp442=({struct Cyc_Absyn_Vardecl*_tmp45E=_cycalloc(sizeof(*_tmp45E));_tmp45E->sc=Cyc_Absyn_Public;_tmp45E->name=_tmp441;_tmp45E->tq=Cyc_Toc_mt_tq;_tmp45E->type=_tmp42A;_tmp45E->initializer=z_init;_tmp45E->rgn=0;_tmp45E->attributes=0;_tmp45E->escapes=0;_tmp45E;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp443=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp45C=_cycalloc(sizeof(*_tmp45C));_tmp45C[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp45D;_tmp45D.tag=4;_tmp45D.f1=_tmp442;_tmp45D;});_tmp45C;});
_tmp438=Cyc_Toc_add_varmap(rgn2,_tmp438,_tmp441,Cyc_Absyn_var_exp(_tmp441,0));{
# 2274
struct Cyc_Absyn_Exp*_tmp444=Cyc_Absyn_varb_exp((void*)_tmp43F,0);_tmp444->topt=_tmp43C->topt;{
struct Cyc_Absyn_Exp*_tmp445=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp446=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp444,_tmp445,0);
_tmp446->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp438,_tmp446);{
# 2280
struct Cyc_Absyn_Exp*_tmp447=Cyc_Absyn_varb_exp((void*)_tmp443,0);_tmp447->topt=_tmp43C->topt;{
struct Cyc_Absyn_Exp*_tmp448=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp449=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp447,_tmp448,0);
_tmp449->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp438,_tmp449);{
# 2286
struct Cyc_List_List*_tmp44A=({struct Cyc_Absyn_Exp*_tmp45B[2];_tmp45B[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0);_tmp45B[0]=
# 2286
Cyc_Absyn_varb_exp((void*)_tmp43A,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp45B,sizeof(struct Cyc_Absyn_Exp*),2));});
# 2288
struct Cyc_Absyn_Exp*_tmp44B=Cyc_Absyn_uint_exp(1,0);
struct Cyc_Absyn_Exp*xsize;
xsize=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp44A,0),_tmp44B,0);{
# 2293
struct Cyc_Absyn_Exp*_tmp44C=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp446,_tmp449,0),0);
struct Cyc_Absyn_Stmt*_tmp44D=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0),0);
struct Cyc_Absyn_Exp*_tmp44E=Cyc_Toc_member_exp(Cyc_Absyn_varb_exp((void*)_tmp43A,0),Cyc_Toc_curr_sp,0);
_tmp44E=Cyc_Toc_cast_it(_tmp42C,_tmp44E);{
struct Cyc_Absyn_Exp*_tmp44F=Cyc_Absyn_deref_exp(_tmp44E,0);
struct Cyc_Absyn_Exp*_tmp450=Cyc_Absyn_assign_exp(_tmp44F,Cyc_Absyn_var_exp(_tmp441,0),0);
struct Cyc_Absyn_Stmt*_tmp451=Cyc_Absyn_exp_stmt(_tmp450,0);
_tmp451=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp44C,_tmp44D,Cyc_Absyn_skip_stmt(0),0),_tmp451,0);
_tmp451=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp452=_cycalloc(sizeof(*_tmp452));_tmp452->r=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp453=_cycalloc(sizeof(*_tmp453));_tmp453[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp454;_tmp454.tag=0;_tmp454.f1=_tmp442;_tmp454;});_tmp453;});_tmp452->loc=0;_tmp452;}),_tmp451,0);
_tmp451=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp455=_cycalloc(sizeof(*_tmp455));_tmp455->r=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp456=_cycalloc(sizeof(*_tmp456));_tmp456[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp457;_tmp457.tag=0;_tmp457.f1=_tmp43E;_tmp457;});_tmp456;});_tmp455->loc=0;_tmp455;}),_tmp451,0);
_tmp451=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp458=_cycalloc(sizeof(*_tmp458));_tmp458->r=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp459=_cycalloc(sizeof(*_tmp459));_tmp459[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp45A;_tmp45A.tag=0;_tmp45A.f1=_tmp439;_tmp45A;});_tmp459;});_tmp458->loc=0;_tmp458;}),_tmp451,0);
e->r=Cyc_Toc_stmt_exp_r(_tmp451);};};};};};};};};};};};}
# 2248
;_pop_region(rgn2);};}
# 2319 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 2323
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp466=Cyc_Tcutil_compress(aggrtype);void*_tmp467=_tmp466;union Cyc_Absyn_AggrInfoU _tmp468;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp467)->tag == 11){_LL272: _tmp468=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp467)->f1).aggr_info;_LL273:
# 2326
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp468);goto _LL271;}else{_LL274: _LL275:
({void*_tmp469=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp46C;_tmp46C.tag=0;_tmp46C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype));({void*_tmp46A[1]={& _tmp46C};Cyc_aprintf(({const char*_tmp46B="expecting union but found %s in check_tagged_union";_tag_dyneither(_tmp46B,sizeof(char),51);}),_tag_dyneither(_tmp46A,sizeof(void*),1));});}),_tag_dyneither(_tmp469,sizeof(void*),0));});}_LL271:;}{
# 2330
struct _tuple1*_tmp46D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp46E=Cyc_Absyn_var_exp(_tmp46D,0);
struct Cyc_Absyn_Exp*_tmp46F=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp470=Cyc_Absyn_aggrarrow_exp(_tmp46E,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp471=Cyc_Absyn_neq_exp(_tmp470,_tmp46F,0);
struct Cyc_Absyn_Exp*_tmp472=Cyc_Absyn_aggrarrow_exp(_tmp46E,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp473=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp472,0),0);
struct Cyc_Absyn_Stmt*_tmp474=Cyc_Absyn_ifthenelse_stmt(_tmp471,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
void*_tmp475=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp476=Cyc_Absyn_address_exp(aggrproj(e1,f,0),0);
struct Cyc_Absyn_Stmt*_tmp477=Cyc_Absyn_declare_stmt(_tmp46D,_tmp475,_tmp476,Cyc_Absyn_seq_stmt(_tmp474,_tmp473,0),0);
return Cyc_Toc_stmt_exp_r(_tmp477);}else{
# 2344
struct Cyc_Absyn_Exp*_tmp478=Cyc_Toc_member_exp(aggrproj(_tmp46E,f,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp479=Cyc_Absyn_neq_exp(_tmp478,_tmp46F,0);
struct Cyc_Absyn_Exp*_tmp47A=Cyc_Toc_member_exp(aggrproj(_tmp46E,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp47B=Cyc_Absyn_exp_stmt(_tmp47A,0);
struct Cyc_Absyn_Stmt*_tmp47C=Cyc_Absyn_ifthenelse_stmt(_tmp479,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*_tmp47D=Cyc_Absyn_declare_stmt(_tmp46D,e1_c_type,e1,Cyc_Absyn_seq_stmt(_tmp47C,_tmp47B,0),0);
return Cyc_Toc_stmt_exp_r(_tmp47D);}};}
# 2354
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _dyneither_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 2357
void*_tmp47E=Cyc_Tcutil_compress(t);void*_tmp47F=_tmp47E;union Cyc_Absyn_AggrInfoU _tmp480;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp47F)->tag == 11){_LL277: _tmp480=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp47F)->f1).aggr_info;_LL278: {
# 2359
struct Cyc_Absyn_Aggrdecl*_tmp481=Cyc_Absyn_get_known_aggrdecl(_tmp480);
*f_tag=Cyc_Toc_get_member_offset(_tmp481,f);{
# 2362
struct _dyneither_ptr str=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp486;_tmp486.tag=0;_tmp486.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp485;_tmp485.tag=0;_tmp485.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp481->name).f2);({void*_tmp483[2]={& _tmp485,& _tmp486};Cyc_aprintf(({const char*_tmp484="_union_%s_%s";_tag_dyneither(_tmp484,sizeof(char),13);}),_tag_dyneither(_tmp483,sizeof(void*),2));});});});
*tagged_member_type=Cyc_Absyn_strct(({struct _dyneither_ptr*_tmp482=_cycalloc(sizeof(*_tmp482));_tmp482[0]=str;_tmp482;}));
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp481->impl))->tagged;};}}else{_LL279: _LL27A:
 return 0;}_LL276:;}
# 2373
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 2376
void*_tmp487=e->r;void*_tmp488=_tmp487;struct Cyc_Absyn_Exp*_tmp489;struct _dyneither_ptr*_tmp48A;int*_tmp48B;struct Cyc_Absyn_Exp*_tmp48C;struct _dyneither_ptr*_tmp48D;int*_tmp48E;struct Cyc_Absyn_Exp*_tmp48F;switch(*((int*)_tmp488)){case 13: _LL27C: _tmp48F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp488)->f2;_LL27D:
({void*_tmp490=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp491="cast on lhs!";_tag_dyneither(_tmp491,sizeof(char),13);}),_tag_dyneither(_tmp490,sizeof(void*),0));});case 20: _LL27E: _tmp48C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp488)->f1;_tmp48D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp488)->f2;_tmp48E=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp488)->f4;_LL27F:
# 2379
 return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(_tmp48C->topt),_tmp48D,f_tag,tagged_member_type,clear_read,_tmp48E);case 21: _LL280: _tmp489=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp488)->f1;_tmp48A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp488)->f2;_tmp48B=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp488)->f4;_LL281: {
# 2382
void*_tmp492=Cyc_Tcutil_compress((void*)_check_null(_tmp489->topt));void*_tmp493=_tmp492;void*_tmp494;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp493)->tag == 5){_LL285: _tmp494=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp493)->f1).elt_typ;_LL286:
# 2384
 return Cyc_Toc_is_tagged_union_project_impl(_tmp494,_tmp48A,f_tag,tagged_member_type,clear_read,_tmp48B);}else{_LL287: _LL288:
# 2386
 return 0;}_LL284:;}default: _LL282: _LL283:
# 2388
 return 0;}_LL27B:;}
# 2400 "toc.cyc"
void Cyc_Toc_add_tagged_union_check_for_swap(struct Cyc_Absyn_Exp*e,int tag,void*mem_type){
struct _tuple1*_tmp495=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp495,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_val,0),0);
struct Cyc_Absyn_Exp*_tmp496=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_ifthenelse_stmt(_tmp496,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp495,Cyc_Absyn_cstar_typ(mem_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(e)),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
e->r=Cyc_Toc_stmt_exp_r(s1);}
# 2424 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 2428
struct _tuple1*_tmp497=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp497,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{
# 2438
struct Cyc_Absyn_Exp*_tmp498=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
s2=Cyc_Absyn_ifthenelse_stmt(_tmp498,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{
# 2441
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp497,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(e1),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple28{struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};
# 2447
static void Cyc_Toc_new_comprehension_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,void*new_e_type,struct Cyc_Absyn_Exp*rgnopt,void*old_typ,struct Cyc_Absyn_Exp*e1,void*t1,int zero_term,struct Cyc_Absyn_Exp*init_exp,struct Cyc_Absyn_Vardecl*vd){
# 2462 "toc.cyc"
struct _tuple1*max=Cyc_Toc_temp_var();
struct _tuple1*a=Cyc_Toc_temp_var();
void*old_elt_typ=t1;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c(nv,e1);{
struct Cyc_Absyn_Exp*_tmp499=Cyc_Absyn_var_exp(max,0);
# 2470
struct Cyc_Absyn_Stmt*s;
if(zero_term)
_tmp499=Cyc_Absyn_add_exp(_tmp499,Cyc_Absyn_uint_exp(1,0),0);
if(init_exp != 0)
s=Cyc_Toc_init_comprehension(nv,new_e_type,
Cyc_Absyn_var_exp(a,0),(struct Cyc_Absyn_Vardecl*)_check_null(vd),Cyc_Absyn_var_exp(max,0),init_exp,zero_term,
Cyc_Toc_skip_stmt_dl(),1);else{
if(zero_term)
s=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(a,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(elt_typ,Cyc_Absyn_uint_exp(0,0)),0),0);else{
# 2481
s=Cyc_Absyn_skip_stmt(0);}}{
# 2483
struct _RegionHandle _tmp49A=_new_region("r");struct _RegionHandle*r=& _tmp49A;_push_region(r);
{struct Cyc_List_List*decls=({struct Cyc_List_List*_tmp4AE=_region_malloc(r,sizeof(*_tmp4AE));_tmp4AE->hd=({struct _tuple28*_tmp4AF=_region_malloc(r,sizeof(*_tmp4AF));_tmp4AF->f1=max;_tmp4AF->f2=Cyc_Absyn_uint_typ;_tmp4AF->f3=e1;_tmp4AF;});_tmp4AE->tl=0;_tmp4AE;});
# 2486
struct Cyc_Absyn_Exp*ai;
if(rgnopt == 0  || Cyc_Absyn_no_regions)
ai=Cyc_Toc_malloc_exp(old_elt_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp49B[2];_tmp49B[1]=_tmp499;_tmp49B[0]=
Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp49B,sizeof(struct Cyc_Absyn_Exp*),2));}),0));else{
# 2493
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);
ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp49C[2];_tmp49C[1]=_tmp499;_tmp49C[0]=
Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp49C,sizeof(struct Cyc_Absyn_Exp*),2));}),0));}{
# 2499
struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);
decls=({struct Cyc_List_List*_tmp49D=_region_malloc(r,sizeof(*_tmp49D));_tmp49D->hd=({struct _tuple28*_tmp49E=_region_malloc(r,sizeof(*_tmp49E));_tmp49E->f1=a;_tmp49E->f2=ptr_typ;_tmp49E->f3=ainit;_tmp49E;});_tmp49D->tl=decls;_tmp49D;});
if(!Cyc_Tcutil_is_pointer_type(old_typ))
({void*_tmp49F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4A0="new_comprehension_to_c: comprehension is not an array type";_tag_dyneither(_tmp4A0,sizeof(char),59);}),_tag_dyneither(_tmp49F,sizeof(void*),0));});
if(Cyc_Tcutil_is_dyneither_ptr(old_typ)){
struct _tuple1*_tmp4A1=Cyc_Toc_temp_var();
void*_tmp4A2=Cyc_Toc_typ_to_c(old_typ);
struct Cyc_Absyn_Exp*_tmp4A3=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmp4A4=
Cyc_Absyn_fncall_exp(_tmp4A3,({struct Cyc_Absyn_Exp*_tmp4A7[3];_tmp4A7[2]=_tmp499;_tmp4A7[1]=
Cyc_Absyn_sizeoftyp_exp(elt_typ,0);_tmp4A7[0]=Cyc_Absyn_var_exp(a,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4A7,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
# 2511
decls=({struct Cyc_List_List*_tmp4A5=_region_malloc(r,sizeof(*_tmp4A5));_tmp4A5->hd=({struct _tuple28*_tmp4A6=_region_malloc(r,sizeof(*_tmp4A6));_tmp4A6->f1=_tmp4A1;_tmp4A6->f2=_tmp4A2;_tmp4A6->f3=_tmp4A4;_tmp4A6;});_tmp4A5->tl=decls;_tmp4A5;});
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp4A1,0),0),0);}else{
# 2514
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}
{struct Cyc_List_List*_tmp4A8=decls;for(0;_tmp4A8 != 0;_tmp4A8=_tmp4A8->tl){
struct _tuple28 _tmp4A9=*((struct _tuple28*)_tmp4A8->hd);struct _tuple28 _tmp4AA=_tmp4A9;struct _tuple1*_tmp4AB;void*_tmp4AC;struct Cyc_Absyn_Exp*_tmp4AD;_LL28A: _tmp4AB=_tmp4AA.f1;_tmp4AC=_tmp4AA.f2;_tmp4AD=_tmp4AA.f3;_LL28B:;
s=Cyc_Absyn_declare_stmt(_tmp4AB,_tmp4AC,_tmp4AD,s,0);}}
# 2519
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2484
;_pop_region(r);};};}struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 2523
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
void*_tmp4B0=e->r;
if(e->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp4B3;_tmp4B3.tag=0;_tmp4B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp4B1[1]={& _tmp4B3};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4B2="exp_to_c: no type for %s";_tag_dyneither(_tmp4B2,sizeof(char),25);}),_tag_dyneither(_tmp4B1,sizeof(void*),1));});});
# 2529
if((nv->struct_info).lhs_exp != 0){
void*_tmp4B4=_tmp4B0;if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4B4)->tag == 28){_LL28D: _LL28E:
 goto _LL28C;}else{_LL28F: _LL290:
 nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);}_LL28C:;}{
# 2537
void*old_typ=(void*)_check_null(e->topt);
void*_tmp4B5=_tmp4B0;struct Cyc_Absyn_Stmt*_tmp4B6;struct Cyc_Absyn_Exp*_tmp4B7;struct _dyneither_ptr*_tmp4B8;struct Cyc_Absyn_Exp*_tmp4B9;struct Cyc_Absyn_Exp*_tmp4BA;int _tmp4BB;struct Cyc_Absyn_Exp*_tmp4BC;void**_tmp4BD;struct Cyc_Absyn_Exp*_tmp4BE;int _tmp4BF;int _tmp4C0;struct Cyc_List_List*_tmp4C1;struct Cyc_Absyn_Datatypedecl*_tmp4C2;struct Cyc_Absyn_Datatypefield*_tmp4C3;void*_tmp4C4;struct Cyc_List_List*_tmp4C5;struct _tuple1*_tmp4C6;struct Cyc_List_List*_tmp4C7;struct Cyc_List_List*_tmp4C8;struct Cyc_Absyn_Aggrdecl*_tmp4C9;struct Cyc_Absyn_Exp*_tmp4CA;void*_tmp4CB;int _tmp4CC;struct Cyc_Absyn_Vardecl*_tmp4CD;struct Cyc_Absyn_Exp*_tmp4CE;struct Cyc_Absyn_Exp*_tmp4CF;int _tmp4D0;struct Cyc_List_List*_tmp4D1;struct Cyc_List_List*_tmp4D2;struct Cyc_Absyn_Exp*_tmp4D3;struct Cyc_Absyn_Exp*_tmp4D4;struct Cyc_Absyn_Exp*_tmp4D5;struct _dyneither_ptr*_tmp4D6;int _tmp4D7;int _tmp4D8;struct Cyc_Absyn_Exp*_tmp4D9;struct _dyneither_ptr*_tmp4DA;int _tmp4DB;int _tmp4DC;struct Cyc_Absyn_Exp*_tmp4DD;void*_tmp4DE;struct Cyc_List_List*_tmp4DF;void*_tmp4E0;struct Cyc_Absyn_Exp*_tmp4E1;struct Cyc_Absyn_Exp*_tmp4E2;struct Cyc_Absyn_Exp*_tmp4E3;struct Cyc_Absyn_Exp*_tmp4E4;void**_tmp4E5;struct Cyc_Absyn_Exp*_tmp4E6;int _tmp4E7;enum Cyc_Absyn_Coercion _tmp4E8;struct Cyc_Absyn_Exp*_tmp4E9;struct Cyc_List_List*_tmp4EA;struct Cyc_Absyn_Exp*_tmp4EB;struct Cyc_Absyn_Exp*_tmp4EC;int _tmp4ED;struct Cyc_Absyn_Exp*_tmp4EE;struct Cyc_List_List*_tmp4EF;int _tmp4F0;struct Cyc_List_List*_tmp4F1;struct Cyc_Absyn_VarargInfo*_tmp4F2;struct Cyc_Absyn_Exp*_tmp4F3;struct Cyc_List_List*_tmp4F4;struct Cyc_Absyn_Exp*_tmp4F5;struct Cyc_Absyn_Exp*_tmp4F6;struct Cyc_Absyn_Exp*_tmp4F7;struct Cyc_Absyn_Exp*_tmp4F8;struct Cyc_Absyn_Exp*_tmp4F9;struct Cyc_Absyn_Exp*_tmp4FA;struct Cyc_Absyn_Exp*_tmp4FB;struct Cyc_Absyn_Exp*_tmp4FC;struct Cyc_Absyn_Exp*_tmp4FD;struct Cyc_Absyn_Exp*_tmp4FE;struct Cyc_Core_Opt*_tmp4FF;struct Cyc_Absyn_Exp*_tmp500;struct Cyc_Absyn_Exp*_tmp501;enum Cyc_Absyn_Incrementor _tmp502;enum Cyc_Absyn_Primop _tmp503;struct Cyc_List_List*_tmp504;void*_tmp505;switch(*((int*)_tmp4B5)){case 0: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1).Null_c).tag == 1){_LL292: _LL293: {
# 2544
struct Cyc_Absyn_Exp*_tmp506=Cyc_Absyn_uint_exp(0,0);
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp506,_tmp506))->r;else{
# 2549
e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*_tmp507[3];_tmp507[2]=_tmp506;_tmp507[1]=_tmp506;_tmp507[0]=_tmp506;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp507,sizeof(struct Cyc_Absyn_Exp*),3));}));}}else{
# 2551
e->r=(void*)& Cyc_Toc_zero_exp;}
# 2553
goto _LL291;}}else{_LL294: _LL295:
 goto _LL291;}case 1: _LL296: _tmp505=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1;_LL297: {
# 2556
struct _tuple1*_tmp508=Cyc_Absyn_binding2qvar(_tmp505);
{struct _handler_cons _tmp509;_push_handler(& _tmp509);{int _tmp50B=0;if(setjmp(_tmp509.handler))_tmp50B=1;if(!_tmp50B){e->r=(Cyc_Toc_lookup_varmap(nv,_tmp508))->r;;_pop_handler();}else{void*_tmp50A=(void*)_exn_thrown;void*_tmp50C=_tmp50A;void*_tmp50D;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp50C)->tag == Cyc_Dict_Absent){_LL2E7: _LL2E8:
# 2559
({struct Cyc_String_pa_PrintArg_struct _tmp510;_tmp510.tag=0;_tmp510.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp508));({void*_tmp50E[1]={& _tmp510};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp50F="Can't find %s in exp_to_c, Var\n";_tag_dyneither(_tmp50F,sizeof(char),32);}),_tag_dyneither(_tmp50E,sizeof(void*),1));});});}else{_LL2E9: _tmp50D=_tmp50C;_LL2EA:(int)_rethrow(_tmp50D);}_LL2E6:;}};}
# 2561
goto _LL291;}case 2: _LL298: _tmp503=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1;_tmp504=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4B5)->f2;_LL299: {
# 2564
struct Cyc_List_List*_tmp511=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp504);
# 2566
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp504);
{enum Cyc_Absyn_Primop _tmp512=_tmp503;switch(_tmp512){case Cyc_Absyn_Numelts: _LL2EC: _LL2ED: {
# 2569
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp504))->hd;
{void*_tmp513=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_tmp514=_tmp513;void*_tmp515;union Cyc_Absyn_Constraint*_tmp516;union Cyc_Absyn_Constraint*_tmp517;union Cyc_Absyn_Constraint*_tmp518;struct Cyc_Absyn_Exp*_tmp519;switch(*((int*)_tmp514)){case 8: _LL301: _tmp519=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp514)->f1).num_elts;_LL302:
# 2573
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp519)))
({void*_tmp51A=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp51B="can't calculate numelts";_tag_dyneither(_tmp51B,sizeof(char),24);}),_tag_dyneither(_tmp51A,sizeof(void*),0));});
e->r=_tmp519->r;goto _LL300;case 5: _LL303: _tmp515=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp514)->f1).elt_typ;_tmp516=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp514)->f1).ptr_atts).nullable;_tmp517=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp514)->f1).ptr_atts).bounds;_tmp518=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp514)->f1).ptr_atts).zero_term;_LL304:
# 2577
{void*_tmp51C=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp517);void*_tmp51D=_tmp51C;struct Cyc_Absyn_Exp*_tmp51E;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp51D)->tag == 0){_LL308: _LL309:
# 2579
 e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,({struct Cyc_Absyn_Exp*_tmp51F[2];_tmp51F[1]=
# 2581
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp515),0);_tmp51F[0]=(struct Cyc_Absyn_Exp*)_tmp504->hd;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp51F,sizeof(struct Cyc_Absyn_Exp*),2));}));
goto _LL307;}else{_LL30A: _tmp51E=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp51D)->f1;_LL30B:
# 2585
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp518)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp504->hd);
# 2589
e->r=Cyc_Toc_fncall_exp_r(function_e,({struct Cyc_Absyn_Exp*_tmp520[2];_tmp520[1]=_tmp51E;_tmp520[0]=(struct Cyc_Absyn_Exp*)_tmp504->hd;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp520,sizeof(struct Cyc_Absyn_Exp*),2));}));}else{
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp516)){
if(!Cyc_Evexp_c_can_eval(_tmp51E))
({void*_tmp521=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp522="can't calculate numelts";_tag_dyneither(_tmp522,sizeof(char),24);}),_tag_dyneither(_tmp521,sizeof(void*),0));});
# 2594
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp51E,Cyc_Absyn_uint_exp(0,0));}else{
# 2596
e->r=_tmp51E->r;goto _LL307;}}
# 2598
goto _LL307;}_LL307:;}
# 2600
goto _LL300;default: _LL305: _LL306:
# 2602
({struct Cyc_String_pa_PrintArg_struct _tmp526;_tmp526.tag=0;_tmp526.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp525;_tmp525.tag=0;_tmp525.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));({void*_tmp523[2]={& _tmp525,& _tmp526};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp524="size primop applied to non-array %s (%s)";_tag_dyneither(_tmp524,sizeof(char),41);}),_tag_dyneither(_tmp523,sizeof(void*),2));});});});}_LL300:;}
# 2605
goto _LL2EB;}case Cyc_Absyn_Plus: _LL2EE: _LL2EF:
# 2610
{void*_tmp527=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp511))->hd);void*_tmp528=_tmp527;void*_tmp529;union Cyc_Absyn_Constraint*_tmp52A;union Cyc_Absyn_Constraint*_tmp52B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp528)->tag == 5){_LL30D: _tmp529=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp528)->f1).elt_typ;_tmp52A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp528)->f1).ptr_atts).bounds;_tmp52B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp528)->f1).ptr_atts).zero_term;_LL30E:
# 2612
{void*_tmp52C=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp52A);void*_tmp52D=_tmp52C;struct Cyc_Absyn_Exp*_tmp52E;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp52D)->tag == 0){_LL312: _LL313: {
# 2614
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp504))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp504->tl))->hd;
e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp52F[3];_tmp52F[2]=e2;_tmp52F[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp529),0);_tmp52F[0]=e1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp52F,sizeof(struct Cyc_Absyn_Exp*),3));}));
goto _LL311;}}else{_LL314: _tmp52E=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp52D)->f1;_LL315:
# 2620
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp52B)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp504))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp504->tl))->hd;
e->r=(Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1),({struct Cyc_Absyn_Exp*_tmp530[3];_tmp530[2]=e2;_tmp530[1]=_tmp52E;_tmp530[0]=e1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp530,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r;}
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
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(_tmp511))->hd,& elt_typ)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp504))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp504->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(_tmp511->tl))->hd)){
e1->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);
e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp);
e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{
# 2646
e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp531[3];_tmp531[2]=
# 2648
Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0);_tmp531[1]=
# 2647
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp531[0]=e1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp531,sizeof(struct Cyc_Absyn_Exp*),3));}));}}
# 2651
goto _LL2EB;}case Cyc_Absyn_Eq: _LL2F2: _LL2F3:
 goto _LL2F5;case Cyc_Absyn_Neq: _LL2F4: _LL2F5:
 goto _LL2F7;case Cyc_Absyn_Gt: _LL2F6: _LL2F7:
 goto _LL2F9;case Cyc_Absyn_Gte: _LL2F8: _LL2F9:
 goto _LL2FB;case Cyc_Absyn_Lt: _LL2FA: _LL2FB:
 goto _LL2FD;case Cyc_Absyn_Lte: _LL2FC: _LL2FD: {
# 2659
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp504))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp504->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp511))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp511->tl))->hd;
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& elt_typ))
e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,0));
if(Cyc_Tcutil_is_tagged_pointer_typ(t2))
e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp,0));
goto _LL2EB;}default: _LL2FE: _LL2FF:
 goto _LL2EB;}_LL2EB:;}
# 2671
goto _LL291;}case 4: _LL29A: _tmp501=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1;_tmp502=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp4B5)->f2;_LL29B: {
# 2673
void*e2_cyc_typ=(void*)_check_null(_tmp501->topt);
# 2682 "toc.cyc"
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
struct _dyneither_ptr incr_str=({const char*_tmp548="increment";_tag_dyneither(_tmp548,sizeof(char),10);});
if(_tmp502 == Cyc_Absyn_PreDec  || _tmp502 == Cyc_Absyn_PostDec)incr_str=({const char*_tmp532="decrement";_tag_dyneither(_tmp532,sizeof(char),10);});
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp501,& ptr_type,& is_dyneither,& elt_type)){
({struct Cyc_String_pa_PrintArg_struct _tmp535;_tmp535.tag=0;_tmp535.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);({void*_tmp533[1]={& _tmp535};Cyc_Tcutil_terr(e->loc,({const char*_tmp534="in-place %s is not supported when dereferencing a zero-terminated pointer";_tag_dyneither(_tmp534,sizeof(char),74);}),_tag_dyneither(_tmp533,sizeof(void*),1));});});
({void*_tmp536=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp537="in-place inc/dec on zero-term";_tag_dyneither(_tmp537,sizeof(char),30);}),_tag_dyneither(_tmp536,sizeof(void*),0));});}{
# 2691
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
int f_tag=0;
if(Cyc_Toc_is_tagged_union_project(_tmp501,& f_tag,& tunion_member_type,1)){
struct Cyc_Absyn_Exp*_tmp538=Cyc_Absyn_signed_int_exp(1,0);
_tmp538->topt=Cyc_Absyn_sint_typ;{
enum Cyc_Absyn_Incrementor _tmp539=_tmp502;switch(_tmp539){case Cyc_Absyn_PreInc: _LL317: _LL318:
# 2698
 e->r=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp53A=_cycalloc(sizeof(*_tmp53A));_tmp53A[0]=({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmp53B;_tmp53B.tag=3;_tmp53B.f1=_tmp501;_tmp53B.f2=({struct Cyc_Core_Opt*_tmp53C=_cycalloc_atomic(sizeof(*_tmp53C));_tmp53C->v=(void*)Cyc_Absyn_Plus;_tmp53C;});_tmp53B.f3=_tmp538;_tmp53B;});_tmp53A;});
Cyc_Toc_exp_to_c(nv,e);
return;case Cyc_Absyn_PreDec: _LL319: _LL31A:
# 2702
 e->r=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp53D=_cycalloc(sizeof(*_tmp53D));_tmp53D[0]=({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmp53E;_tmp53E.tag=3;_tmp53E.f1=_tmp501;_tmp53E.f2=({struct Cyc_Core_Opt*_tmp53F=_cycalloc_atomic(sizeof(*_tmp53F));_tmp53F->v=(void*)Cyc_Absyn_Minus;_tmp53F;});_tmp53E.f3=_tmp538;_tmp53E;});_tmp53D;});
Cyc_Toc_exp_to_c(nv,e);
return;default: _LL31B: _LL31C:
# 2706
({struct Cyc_String_pa_PrintArg_struct _tmp542;_tmp542.tag=0;_tmp542.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);({void*_tmp540[1]={& _tmp542};Cyc_Tcutil_terr(e->loc,({const char*_tmp541="in-place post-%s is not supported on @tagged union members";_tag_dyneither(_tmp541,sizeof(char),59);}),_tag_dyneither(_tmp540,sizeof(void*),1));});});
# 2708
({void*_tmp543=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp544="in-place inc/dec on @tagged union";_tag_dyneither(_tmp544,sizeof(char),34);}),_tag_dyneither(_tmp543,sizeof(void*),0));});}_LL316:;};}
# 2711
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp501);
Cyc_Toc_set_lhs(nv,0);{
# 2716
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp502 == Cyc_Absyn_PostInc  || _tmp502 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2722
if(_tmp502 == Cyc_Absyn_PreDec  || _tmp502 == Cyc_Absyn_PostDec)
change=-1;
e->r=Cyc_Toc_fncall_exp_r(fn_e,({struct Cyc_Absyn_Exp*_tmp545[3];_tmp545[2]=
# 2726
Cyc_Absyn_signed_int_exp(change,0);_tmp545[1]=
# 2725
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp545[0]=
# 2724
Cyc_Toc_push_address_exp(_tmp501);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp545,sizeof(struct Cyc_Absyn_Exp*),3));}));}else{
# 2727
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2731
struct Cyc_Toc_functionSet*_tmp546=_tmp502 != Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
# 2733
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp546,_tmp501);
e->r=Cyc_Toc_fncall_exp_r(fn_e,({struct Cyc_Absyn_Exp*_tmp547[2];_tmp547[1]=
Cyc_Absyn_signed_int_exp(1,0);_tmp547[0]=
# 2734
Cyc_Toc_push_address_exp(_tmp501);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp547,sizeof(struct Cyc_Absyn_Exp*),2));}));}else{
# 2736
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp501)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp501,0,Cyc_Toc_incr_lvalue,_tmp502);
e->r=_tmp501->r;}}}
# 2740
goto _LL291;};};}case 3: _LL29C: _tmp4FE=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1;_tmp4FF=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4B5)->f2;_tmp500=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4B5)->f3;_LL29D: {
# 2759 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp4FE->topt);
void*e2_old_typ=(void*)_check_null(_tmp500->topt);
int f_tag=0;
void*tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Toc_is_tagged_union_project(_tmp4FE,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4FE);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp500);
e->r=Cyc_Toc_tagged_union_assignop(_tmp4FE,e1_old_typ,_tmp4FF,_tmp500,e2_old_typ,f_tag,tagged_member_struct_type);
# 2770
return;}{
# 2772
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4FE,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp4FE,_tmp4FF,_tmp500,ptr_type,is_dyneither,elt_type);
# 2778
return;}{
# 2782
int e1_poly=Cyc_Toc_is_poly_project(_tmp4FE);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4FE);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp500);{
# 2788
int done=0;
if(_tmp4FF != 0){
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp549=(enum Cyc_Absyn_Primop)_tmp4FF->v;enum Cyc_Absyn_Primop _tmp54A=_tmp549;switch(_tmp54A){case Cyc_Absyn_Plus: _LL31E: _LL31F:
 change=_tmp500;goto _LL31D;case Cyc_Absyn_Minus: _LL320: _LL321:
# 2796
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp500,0);goto _LL31D;default: _LL322: _LL323:
({void*_tmp54B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp54C="bad t ? pointer arithmetic";_tag_dyneither(_tmp54C,sizeof(char),27);}),_tag_dyneither(_tmp54B,sizeof(void*),0));});}_LL31D:;}
# 2799
done=1;{
# 2801
struct Cyc_Absyn_Exp*_tmp54D=Cyc_Toc__dyneither_ptr_inplace_plus_e;
e->r=Cyc_Toc_fncall_exp_r(_tmp54D,({struct Cyc_Absyn_Exp*_tmp54E[3];_tmp54E[2]=change;_tmp54E[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp54E[0]=
# 2802
Cyc_Toc_push_address_exp(_tmp4FE);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp54E,sizeof(struct Cyc_Absyn_Exp*),3));}));};}else{
# 2805
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2808
enum Cyc_Absyn_Primop _tmp54F=(enum Cyc_Absyn_Primop)_tmp4FF->v;enum Cyc_Absyn_Primop _tmp550=_tmp54F;if(_tmp550 == Cyc_Absyn_Plus){_LL325: _LL326:
# 2810
 done=1;
e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp4FE),({struct Cyc_Absyn_Exp*_tmp551[2];_tmp551[1]=_tmp500;_tmp551[0]=_tmp4FE;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp551,sizeof(struct Cyc_Absyn_Exp*),2));}));
goto _LL324;}else{_LL327: _LL328:
({void*_tmp552=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp553="bad zero-terminated pointer arithmetic";_tag_dyneither(_tmp553,sizeof(char),39);}),_tag_dyneither(_tmp552,sizeof(void*),0));});}_LL324:;}}}
# 2817
if(!done){
# 2819
if(e1_poly)
_tmp500->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp500->r,0));
# 2825
if(!Cyc_Absyn_is_lvalue(_tmp4FE)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple27*),struct _tuple27*f_env))Cyc_Toc_lvalue_assign)(_tmp4FE,0,Cyc_Toc_assignop_lvalue,({struct _tuple27*_tmp554=_cycalloc(sizeof(struct _tuple27)* 1);_tmp554[0]=(struct _tuple27)({struct _tuple27 _tmp555;_tmp555.f1=_tmp4FF;_tmp555.f2=_tmp500;_tmp555;});_tmp554;}));
e->r=_tmp4FE->r;}}
# 2830
goto _LL291;};};};}case 5: _LL29E: _tmp4FB=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1;_tmp4FC=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4B5)->f2;_tmp4FD=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4B5)->f3;_LL29F:
# 2832
 Cyc_Toc_exp_to_c(nv,_tmp4FB);
Cyc_Toc_exp_to_c(nv,_tmp4FC);
Cyc_Toc_exp_to_c(nv,_tmp4FD);
goto _LL291;case 6: _LL2A0: _tmp4F9=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1;_tmp4FA=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4B5)->f2;_LL2A1:
# 2837
 Cyc_Toc_exp_to_c(nv,_tmp4F9);
Cyc_Toc_exp_to_c(nv,_tmp4FA);
goto _LL291;case 7: _LL2A2: _tmp4F7=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1;_tmp4F8=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4B5)->f2;_LL2A3:
# 2841
 Cyc_Toc_exp_to_c(nv,_tmp4F7);
Cyc_Toc_exp_to_c(nv,_tmp4F8);
goto _LL291;case 8: _LL2A4: _tmp4F5=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1;_tmp4F6=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4B5)->f2;_LL2A5:
# 2845
 Cyc_Toc_exp_to_c(nv,_tmp4F5);
Cyc_Toc_exp_to_c(nv,_tmp4F6);
goto _LL291;case 9: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4B5)->f3 == 0){_LL2A6: _tmp4F3=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1;_tmp4F4=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4B5)->f2;_LL2A7:
# 2849
 Cyc_Toc_exp_to_c(nv,_tmp4F3);
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4F4);
goto _LL291;}else{_LL2A8: _tmp4EE=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1;_tmp4EF=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4B5)->f2;_tmp4F0=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4B5)->f3)->num_varargs;_tmp4F1=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4B5)->f3)->injectors;_tmp4F2=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4B5)->f3)->vai;_LL2A9: {
# 2861 "toc.cyc"
struct _RegionHandle _tmp556=_new_region("r");struct _RegionHandle*r=& _tmp556;_push_region(r);{
struct _tuple1*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp4F0,0);
void*cva_type=Cyc_Toc_typ_to_c(_tmp4F2->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_conref,0);
# 2869
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4EF);
int num_normargs=num_args - _tmp4F0;
# 2873
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp4EF=_tmp4EF->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4EF))->hd);
new_args=({struct Cyc_List_List*_tmp557=_cycalloc(sizeof(*_tmp557));_tmp557->hd=(struct Cyc_Absyn_Exp*)_tmp4EF->hd;_tmp557->tl=new_args;_tmp557;});}}
# 2878
new_args=({struct Cyc_List_List*_tmp558=_cycalloc(sizeof(*_tmp558));_tmp558->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*_tmp559[3];_tmp559[2]=num_varargs_exp;_tmp559[1]=
# 2880
Cyc_Absyn_sizeoftyp_exp(cva_type,0);_tmp559[0]=argvexp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp559,sizeof(struct Cyc_Absyn_Exp*),3));}),0);_tmp558->tl=new_args;_tmp558;});
# 2883
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);
# 2885
Cyc_Toc_exp_to_c(nv,_tmp4EE);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4EE,new_args,0),0);
# 2889
if(_tmp4F2->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp55A=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp4F2->type));void*_tmp55B=_tmp55A;struct Cyc_Absyn_Datatypedecl*_tmp55C;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp55B)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp55B)->f1).datatype_info).KnownDatatype).tag == 2){_LL32A: _tmp55C=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp55B)->f1).datatype_info).KnownDatatype).val;_LL32B:
 tud=_tmp55C;goto _LL329;}else{goto _LL32C;}}else{_LL32C: _LL32D:
({void*_tmp55D=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp55E="toc: unknown datatype in vararg with inject";_tag_dyneither(_tmp55E,sizeof(char),44);}),_tag_dyneither(_tmp55D,sizeof(void*),0));});}_LL329:;}{
# 2895
struct _dyneither_ptr vs=({unsigned int _tmp567=(unsigned int)_tmp4F0;struct _tuple1**_tmp568=(struct _tuple1**)_region_malloc(r,_check_times(sizeof(struct _tuple1*),_tmp567));struct _dyneither_ptr _tmp56A=_tag_dyneither(_tmp568,sizeof(struct _tuple1*),_tmp567);{unsigned int _tmp569=_tmp567;unsigned int i;for(i=0;i < _tmp569;i ++){_tmp568[i]=(struct _tuple1*)Cyc_Toc_temp_var();}}_tmp56A;});
# 2897
if(_tmp4F0 != 0){
# 2899
struct Cyc_List_List*_tmp55F=0;
{int i=_tmp4F0 - 1;for(0;i >= 0;-- i){
_tmp55F=({struct Cyc_List_List*_tmp560=_cycalloc(sizeof(*_tmp560));_tmp560->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0),0));_tmp560->tl=_tmp55F;_tmp560;});}}
# 2903
s=Cyc_Absyn_declare_stmt(argv,arr_type,Cyc_Absyn_unresolvedmem_exp(0,_tmp55F,0),s,0);{
# 2906
int i=0;for(0;_tmp4EF != 0;(((_tmp4EF=_tmp4EF->tl,_tmp4F1=_tmp4F1->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp4EF->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0);
struct Cyc_Absyn_Datatypefield*_tmp561=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp4F1))->hd;struct Cyc_Absyn_Datatypefield*_tmp562=_tmp561;struct _tuple1*_tmp563;struct Cyc_List_List*_tmp564;_LL32F: _tmp563=_tmp562->name;_tmp564=_tmp562->typs;_LL330:;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp564))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);
# 2921
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);
# 2924
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Toc_tag_sp,0),
Cyc_Toc_datatype_tag(tud,_tmp563),0),s,0);
# 2927
s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp563,tud->name)),0,s,0);};}};}else{
# 2934
struct Cyc_List_List*_tmp565=({struct _tuple19*_tmp566[3];_tmp566[2]=
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0));_tmp566[1]=
# 2934
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0));_tmp566[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0));((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp566,sizeof(struct _tuple19*),3));});
# 2936
s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),Cyc_Absyn_uint_exp(0,0),s,0);}};}else{
# 2946
{int i=0;for(0;_tmp4EF != 0;(_tmp4EF=_tmp4EF->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4EF->hd);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp4EF->hd,0),s,0);}}
# 2952
s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}
# 2965 "toc.cyc"
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2967
_npop_handler(0);goto _LL291;
# 2861 "toc.cyc"
;_pop_region(r);}}case 10: _LL2AA: _tmp4EC=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1;_tmp4ED=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp4B5)->f2;_LL2AB:
# 2970 "toc.cyc"
 Cyc_Toc_exp_to_c(nv,_tmp4EC);{
struct Cyc_Absyn_Exp*_tmp56B=_tmp4ED?Cyc_Toc_newrethrow_exp(_tmp4EC): Cyc_Toc_newthrow_exp(_tmp4EC);
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),_tmp56B,0))->r;
goto _LL291;};case 11: _LL2AC: _tmp4EB=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1;_LL2AD:
 Cyc_Toc_exp_to_c(nv,_tmp4EB);goto _LL291;case 12: _LL2AE: _tmp4E9=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1;_tmp4EA=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4B5)->f2;_LL2AF:
# 2976
 Cyc_Toc_exp_to_c(nv,_tmp4E9);
# 2985 "toc.cyc"
for(0;_tmp4EA != 0;_tmp4EA=_tmp4EA->tl){
enum Cyc_Absyn_KindQual _tmp56C=(Cyc_Tcutil_typ_kind((void*)_tmp4EA->hd))->kind;
if(_tmp56C != Cyc_Absyn_EffKind  && _tmp56C != Cyc_Absyn_RgnKind){
{void*_tmp56D=Cyc_Tcutil_compress((void*)_tmp4EA->hd);void*_tmp56E=_tmp56D;switch(*((int*)_tmp56E)){case 2: _LL332: _LL333:
 goto _LL335;case 3: _LL334: _LL335:
 continue;default: _LL336: _LL337:
# 2992
 e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp4E9,0))->r;
goto _LL331;}_LL331:;}
# 2995
break;}}
# 2998
goto _LL291;case 13: _LL2B0: _tmp4E5=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1;_tmp4E6=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4B5)->f2;_tmp4E7=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4B5)->f3;_tmp4E8=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4B5)->f4;_LL2B1: {
# 3000
void*old_t2=(void*)_check_null(_tmp4E6->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp4E5;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp4E5=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp4E6);
# 3007
{struct _tuple29 _tmp56F=({struct _tuple29 _tmp5AA;_tmp5AA.f1=Cyc_Tcutil_compress(old_t2);_tmp5AA.f2=Cyc_Tcutil_compress(new_typ);_tmp5AA;});struct _tuple29 _tmp570=_tmp56F;struct Cyc_Absyn_PtrInfo _tmp571;struct Cyc_Absyn_PtrInfo _tmp572;struct Cyc_Absyn_PtrInfo _tmp573;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp570.f1)->tag == 5)switch(*((int*)_tmp570.f2)){case 5: _LL339: _tmp572=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp570.f1)->f1;_tmp573=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp570.f2)->f1;_LL33A: {
# 3009
int _tmp574=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp572.ptr_atts).nullable);
int _tmp575=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp573.ptr_atts).nullable);
void*_tmp576=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp572.ptr_atts).bounds);
void*_tmp577=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp573.ptr_atts).bounds);
int _tmp578=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp572.ptr_atts).zero_term);
int _tmp579=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp573.ptr_atts).zero_term);
{struct _tuple29 _tmp57A=({struct _tuple29 _tmp5A7;_tmp5A7.f1=_tmp576;_tmp5A7.f2=_tmp577;_tmp5A7;});struct _tuple29 _tmp57B=_tmp57A;struct Cyc_Absyn_Exp*_tmp57C;struct Cyc_Absyn_Exp*_tmp57D;struct Cyc_Absyn_Exp*_tmp57E;struct Cyc_Absyn_Exp*_tmp57F;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp57B.f1)->tag == 1){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp57B.f2)->tag == 1){_LL340: _tmp57E=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp57B.f1)->f1;_tmp57F=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp57B.f2)->f1;_LL341:
# 3017
 if((!Cyc_Evexp_c_can_eval(_tmp57E) || !Cyc_Evexp_c_can_eval(_tmp57F)) && !
Cyc_Evexp_same_const_exp(_tmp57E,_tmp57F))
({void*_tmp580=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp581="can't validate cast due to potential size differences";_tag_dyneither(_tmp581,sizeof(char),54);}),_tag_dyneither(_tmp580,sizeof(void*),0));});
if(_tmp574  && !_tmp575){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp582=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp583="can't do NULL-check conversion at top-level";_tag_dyneither(_tmp583,sizeof(char),44);}),_tag_dyneither(_tmp582,sizeof(void*),0));});
# 3026
if(_tmp4E8 != Cyc_Absyn_NonNull_to_Null)
({struct Cyc_String_pa_PrintArg_struct _tmp586;_tmp586.tag=0;_tmp586.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp584[1]={& _tmp586};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp585="null-check conversion mis-classified: %s";_tag_dyneither(_tmp585,sizeof(char),41);}),_tag_dyneither(_tmp584,sizeof(void*),1));});});{
int do_null_check=Cyc_Toc_need_null_check(_tmp4E6);
if(do_null_check){
if(!_tmp4E7)
({void*_tmp587=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp588="inserted null check due to implicit cast from * to @ type";_tag_dyneither(_tmp588,sizeof(char),58);}),_tag_dyneither(_tmp587,sizeof(void*),0));});
# 3033
e->r=Cyc_Toc_cast_it_r(*_tmp4E5,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp589=_cycalloc(sizeof(*_tmp589));_tmp589->hd=_tmp4E6;_tmp589->tl=0;_tmp589;}),0));}else{
# 3037
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4E6->r;}};}else{
# 3042
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4E6->r;}
# 3049
goto _LL33F;}else{_LL342: _tmp57D=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp57B.f1)->f1;_LL343:
# 3051
 if(!Cyc_Evexp_c_can_eval(_tmp57D))
({void*_tmp58A=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp58B="cannot perform coercion since numelts cannot be determined statically.";_tag_dyneither(_tmp58B,sizeof(char),71);}),_tag_dyneither(_tmp58A,sizeof(void*),0));});
# 3054
if(_tmp4E8 == Cyc_Absyn_NonNull_to_Null)
({struct Cyc_String_pa_PrintArg_struct _tmp58E;_tmp58E.tag=0;_tmp58E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp58C[1]={& _tmp58E};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp58D="conversion mis-classified as null-check: %s";_tag_dyneither(_tmp58D,sizeof(char),44);}),_tag_dyneither(_tmp58C,sizeof(void*),1));});});
if(Cyc_Toc_is_toplevel(nv)){
# 3058
if((_tmp578  && !(_tmp573.elt_tq).real_const) && !_tmp579)
# 3061
_tmp57D=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp57D,Cyc_Absyn_uint_exp(1,0),0);
# 3063
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp57D,_tmp4E6))->r;}else{
# 3065
struct Cyc_Absyn_Exp*_tmp58F=Cyc_Toc__tag_dyneither_e;
# 3067
if(_tmp578){
# 3072
struct _tuple1*_tmp590=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp591=Cyc_Absyn_var_exp(_tmp590,0);
struct Cyc_Absyn_Exp*arg3;
# 3077
{void*_tmp592=_tmp4E6->r;void*_tmp593=_tmp592;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp593)->tag == 0)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp593)->f1).Wstring_c).tag){case 8: _LL349: _LL34A:
# 3079
 arg3=_tmp57D;goto _LL348;case 9: _LL34B: _LL34C:
# 3081
 arg3=_tmp57D;goto _LL348;default: goto _LL34D;}else{_LL34D: _LL34E:
# 3083
 arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp4E6),({struct Cyc_Absyn_Exp*_tmp594[2];_tmp594[1]=_tmp57D;_tmp594[0]=
# 3085
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp591);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp594,sizeof(struct Cyc_Absyn_Exp*),2));}),0);
goto _LL348;}_LL348:;}
# 3088
if(!_tmp579  && !(_tmp573.elt_tq).real_const)
# 3091
arg3=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{
# 3093
struct Cyc_Absyn_Exp*_tmp595=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp573.elt_typ),0);
struct Cyc_Absyn_Exp*_tmp596=Cyc_Absyn_fncall_exp(_tmp58F,({struct Cyc_Absyn_Exp*_tmp598[3];_tmp598[2]=arg3;_tmp598[1]=_tmp595;_tmp598[0]=_tmp591;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp598,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
struct Cyc_Absyn_Stmt*_tmp597=Cyc_Absyn_exp_stmt(_tmp596,0);
_tmp597=Cyc_Absyn_declare_stmt(_tmp590,Cyc_Toc_typ_to_c(old_t2),_tmp4E6,_tmp597,0);
e->r=Cyc_Toc_stmt_exp_r(_tmp597);};}else{
# 3100
e->r=Cyc_Toc_fncall_exp_r(_tmp58F,({struct Cyc_Absyn_Exp*_tmp599[3];_tmp599[2]=_tmp57D;_tmp599[1]=
# 3102
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp573.elt_typ),0);_tmp599[0]=_tmp4E6;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp599,sizeof(struct Cyc_Absyn_Exp*),3));}));}}
# 3106
goto _LL33F;}}else{if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp57B.f2)->tag == 1){_LL344: _tmp57C=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp57B.f2)->f1;_LL345:
# 3108
 if(!Cyc_Evexp_c_can_eval(_tmp57C))
({void*_tmp59A=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp59B="cannot perform coercion since numelts cannot be determined statically.";_tag_dyneither(_tmp59B,sizeof(char),71);}),_tag_dyneither(_tmp59A,sizeof(void*),0));});
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp59C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp59D="can't coerce t? to t* or t@ at the top-level";_tag_dyneither(_tmp59D,sizeof(char),45);}),_tag_dyneither(_tmp59C,sizeof(void*),0));});{
# 3120 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp59E=_tmp57C;
if(_tmp578  && !_tmp579)
_tmp59E=Cyc_Absyn_add_exp(_tmp57C,Cyc_Absyn_uint_exp(1,0),0);{
# 3127
struct Cyc_Absyn_Exp*_tmp59F=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmp5A0=Cyc_Absyn_fncall_exp(_tmp59F,({struct Cyc_Absyn_Exp*_tmp5A2[3];_tmp5A2[2]=_tmp59E;_tmp5A2[1]=
# 3130
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp572.elt_typ),0);_tmp5A2[0]=_tmp4E6;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5A2,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
# 3132
if(_tmp575)
_tmp5A0->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp5A1=_cycalloc(sizeof(*_tmp5A1));_tmp5A1->hd=
Cyc_Absyn_copy_exp(_tmp5A0);_tmp5A1->tl=0;_tmp5A1;}));
e->r=Cyc_Toc_cast_it_r(*_tmp4E5,_tmp5A0);
goto _LL33F;};};}else{_LL346: _LL347:
# 3140
 DynCast:
 if((_tmp578  && !_tmp579) && !(_tmp573.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp5A3=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp5A4="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";_tag_dyneither(_tmp5A4,sizeof(char),70);}),_tag_dyneither(_tmp5A3,sizeof(void*),0));});{
struct Cyc_Absyn_Exp*_tmp5A5=Cyc_Toc__dyneither_ptr_decrease_size_e;
e->r=Cyc_Toc_fncall_exp_r(_tmp5A5,({struct Cyc_Absyn_Exp*_tmp5A6[3];_tmp5A6[2]=
# 3147
Cyc_Absyn_uint_exp(1,0);_tmp5A6[1]=
# 3146
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp572.elt_typ),0);_tmp5A6[0]=_tmp4E6;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5A6,sizeof(struct Cyc_Absyn_Exp*),3));}));};}
# 3149
goto _LL33F;}}_LL33F:;}
# 3151
goto _LL338;}case 6: _LL33B: _tmp571=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp570.f1)->f1;_LL33C:
# 3153
{void*_tmp5A8=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp571.ptr_atts).bounds);void*_tmp5A9=_tmp5A8;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5A9)->tag == 0){_LL350: _LL351:
# 3155
 _tmp4E6->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp4E6->r,_tmp4E6->loc),Cyc_Toc_curr_sp);goto _LL34F;}else{_LL352: _LL353:
 goto _LL34F;}_LL34F:;}
# 3158
goto _LL338;default: goto _LL33D;}else{_LL33D: _LL33E:
# 3160
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4E6->r;
goto _LL338;}_LL338:;}
# 3164
goto _LL291;}case 14: _LL2B2: _tmp4E4=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1;_LL2B3:
# 3168
{void*_tmp5AB=_tmp4E4->r;void*_tmp5AC=_tmp5AB;struct Cyc_List_List*_tmp5AD;struct _tuple1*_tmp5AE;struct Cyc_List_List*_tmp5AF;struct Cyc_List_List*_tmp5B0;switch(*((int*)_tmp5AC)){case 28: _LL355: _tmp5AE=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5AC)->f1;_tmp5AF=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5AC)->f2;_tmp5B0=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5AC)->f3;_LL356:
# 3170
 if(Cyc_Toc_is_toplevel(nv))
({struct Cyc_String_pa_PrintArg_struct _tmp5B3;_tmp5B3.tag=0;_tmp5B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp4E4->loc));({void*_tmp5B1[1]={& _tmp5B3};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp5B2="%s: & on non-identifiers at the top-level";_tag_dyneither(_tmp5B2,sizeof(char),42);}),_tag_dyneither(_tmp5B1,sizeof(void*),1));});});{
struct Cyc_Absyn_Exp*_tmp5B4=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp4E4->topt),_tmp5AF,1,0,_tmp5B0,_tmp5AE);
e->r=_tmp5B4->r;
e->topt=_tmp5B4->topt;
goto _LL354;};case 23: _LL357: _tmp5AD=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp5AC)->f1;_LL358:
# 3178
 if(Cyc_Toc_is_toplevel(nv))
({struct Cyc_String_pa_PrintArg_struct _tmp5B7;_tmp5B7.tag=0;_tmp5B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp4E4->loc));({void*_tmp5B5[1]={& _tmp5B7};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp5B6="%s: & on non-identifiers at the top-level";_tag_dyneither(_tmp5B6,sizeof(char),42);}),_tag_dyneither(_tmp5B5,sizeof(void*),1));});});
e->r=(Cyc_Toc_init_tuple(nv,1,0,_tmp5AD))->r;
goto _LL354;default: _LL359: _LL35A:
# 3184
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4E4);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp4E4)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp4E4,0,Cyc_Toc_address_lvalue,1);
# 3190
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp4E4);}else{
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind((void*)_check_null(_tmp4E4->topt))))
# 3194
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp4E4);}
# 3196
goto _LL354;}_LL354:;}
# 3198
goto _LL291;case 15: _LL2B4: _tmp4E2=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1;_tmp4E3=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp4B5)->f2;_LL2B5:
# 3201
 if(Cyc_Toc_is_toplevel(nv))
({struct Cyc_String_pa_PrintArg_struct _tmp5BA;_tmp5BA.tag=0;_tmp5BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp4E3->loc));({void*_tmp5B8[1]={& _tmp5BA};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp5B9="%s: new at top-level";_tag_dyneither(_tmp5B9,sizeof(char),21);}),_tag_dyneither(_tmp5B8,sizeof(void*),1));});});{
void*new_e_type=(void*)_check_null(_tmp4E3->topt);
{void*_tmp5BB=_tmp4E3->r;void*_tmp5BC=_tmp5BB;struct Cyc_List_List*_tmp5BD;struct _tuple1*_tmp5BE;struct Cyc_List_List*_tmp5BF;struct Cyc_List_List*_tmp5C0;struct Cyc_Absyn_Aggrdecl*_tmp5C1;struct Cyc_Absyn_Exp*_tmp5C2;void*_tmp5C3;int _tmp5C4;struct Cyc_Absyn_Vardecl*_tmp5C5;struct Cyc_Absyn_Exp*_tmp5C6;struct Cyc_Absyn_Exp*_tmp5C7;int _tmp5C8;struct Cyc_List_List*_tmp5C9;switch(*((int*)_tmp5BC)){case 25: _LL35C: _tmp5C9=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5BC)->f1;_LL35D: {
# 3209
struct _tuple1*_tmp5CA=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp5CB=Cyc_Absyn_var_exp(_tmp5CA,0);
struct Cyc_Absyn_Stmt*_tmp5CC=Cyc_Toc_init_array(nv,new_e_type,_tmp5CB,_tmp5C9,Cyc_Absyn_exp_stmt(_tmp5CB,0));
void*old_elt_typ;
{void*_tmp5CD=Cyc_Tcutil_compress(old_typ);void*_tmp5CE=_tmp5CD;void*_tmp5CF;struct Cyc_Absyn_Tqual _tmp5D0;union Cyc_Absyn_Constraint*_tmp5D1;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5CE)->tag == 5){_LL369: _tmp5CF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5CE)->f1).elt_typ;_tmp5D0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5CE)->f1).elt_tq;_tmp5D1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5CE)->f1).ptr_atts).zero_term;_LL36A:
# 3215
 old_elt_typ=_tmp5CF;goto _LL368;}else{_LL36B: _LL36C:
# 3217
 old_elt_typ=({void*_tmp5D2=0;((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp5D3="exp_to_c:new array expression doesn't have ptr type";_tag_dyneither(_tmp5D3,sizeof(char),52);}),_tag_dyneither(_tmp5D2,sizeof(void*),0));});}_LL368:;}{
# 3220
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*_tmp5D4=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp5D5=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),
Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5C9),0),0);
struct Cyc_Absyn_Exp*e1;
if(_tmp4E2 == 0  || Cyc_Absyn_no_regions)
e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp5D5);else{
# 3228
struct Cyc_Absyn_Exp*r=_tmp4E2;
Cyc_Toc_exp_to_c(nv,r);
e1=Cyc_Toc_rmalloc_exp(r,_tmp5D5);}
# 3233
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp5CA,_tmp5D4,e1,_tmp5CC,0));
goto _LL35B;};}case 26: _LL35E: _tmp5C5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5BC)->f1;_tmp5C6=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5BC)->f2;_tmp5C7=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5BC)->f3;_tmp5C8=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5BC)->f4;_LL35F:
# 3237
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp4E2,old_typ,_tmp5C6,(void*)_check_null(_tmp5C7->topt),_tmp5C8,_tmp5C7,_tmp5C5);
goto _LL35B;case 27: _LL360: _tmp5C2=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp5BC)->f1;_tmp5C3=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp5BC)->f2;_tmp5C4=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp5BC)->f3;_LL361:
# 3241
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp4E2,old_typ,_tmp5C2,_tmp5C3,_tmp5C4,0,0);
goto _LL35B;case 28: _LL362: _tmp5BE=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5BC)->f1;_tmp5BF=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5BC)->f2;_tmp5C0=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5BC)->f3;_tmp5C1=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5BC)->f4;_LL363: {
# 3246
struct Cyc_Absyn_Exp*_tmp5D6=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp4E3->topt),_tmp5BF,1,_tmp4E2,_tmp5C0,_tmp5BE);
e->r=_tmp5D6->r;
e->topt=_tmp5D6->topt;
goto _LL35B;}case 23: _LL364: _tmp5BD=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp5BC)->f1;_LL365:
# 3252
 e->r=(Cyc_Toc_init_tuple(nv,1,_tmp4E2,_tmp5BD))->r;
goto _LL35B;default: _LL366: _LL367: {
# 3259
void*old_elt_typ=(void*)_check_null(_tmp4E3->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
# 3262
struct _tuple1*_tmp5D7=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp5D8=Cyc_Absyn_var_exp(_tmp5D7,0);
struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp5D8,0),0);
struct Cyc_Absyn_Exp*inner_mexp=mexp;
if(_tmp4E2 == 0  || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
# 3269
struct Cyc_Absyn_Exp*r=_tmp4E2;
Cyc_Toc_exp_to_c(nv,r);
mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{
# 3277
int done=0;
{void*_tmp5D9=_tmp4E3->r;void*_tmp5DA=_tmp5D9;void*_tmp5DB;struct Cyc_Absyn_Exp*_tmp5DC;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5DA)->tag == 13){_LL36E: _tmp5DB=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5DA)->f1;_tmp5DC=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5DA)->f2;_LL36F:
# 3280
{struct _tuple29 _tmp5DD=({struct _tuple29 _tmp5E8;_tmp5E8.f1=Cyc_Tcutil_compress(_tmp5DB);_tmp5E8.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp5DC->topt));_tmp5E8;});struct _tuple29 _tmp5DE=_tmp5DD;void*_tmp5DF;union Cyc_Absyn_Constraint*_tmp5E0;union Cyc_Absyn_Constraint*_tmp5E1;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5DE.f1)->tag == 5){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5DE.f2)->tag == 5){_LL373: _tmp5DF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5DE.f1)->f1).elt_typ;_tmp5E0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5DE.f1)->f1).ptr_atts).bounds;_tmp5E1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5DE.f2)->f1).ptr_atts).bounds;_LL374:
# 3283
{struct _tuple29 _tmp5E2=({struct _tuple29 _tmp5E7;_tmp5E7.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5E0);_tmp5E7.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5E1);_tmp5E7;});struct _tuple29 _tmp5E3=_tmp5E2;struct Cyc_Absyn_Exp*_tmp5E4;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5E3.f1)->tag == 0){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5E3.f2)->tag == 1){_LL378: _tmp5E4=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5E3.f2)->f1;_LL379:
# 3285
 Cyc_Toc_exp_to_c(nv,_tmp5DC);
inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);
done=1;{
struct Cyc_Absyn_Exp*_tmp5E5=Cyc_Toc__init_dyneither_ptr_e;
e->r=Cyc_Toc_fncall_exp_r(_tmp5E5,({struct Cyc_Absyn_Exp*_tmp5E6[4];_tmp5E6[3]=_tmp5E4;_tmp5E6[2]=
# 3291
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5DF),0);_tmp5E6[1]=_tmp5DC;_tmp5E6[0]=mexp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5E6,sizeof(struct Cyc_Absyn_Exp*),4));}));
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
struct Cyc_Absyn_Stmt*_tmp5E9=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp5D8),0);
struct Cyc_Absyn_Exp*_tmp5EA=Cyc_Absyn_signed_int_exp(0,0);
Cyc_Toc_exp_to_c(nv,_tmp4E3);
_tmp5E9=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp5D8,_tmp5EA,0),_tmp4E3,0),_tmp5E9,0);{
# 3308
void*_tmp5EB=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp5D7,_tmp5EB,mexp,_tmp5E9,0));};}
# 3311
goto _LL35B;};}}_LL35B:;}
# 3313
goto _LL291;};case 17: _LL2B6: _tmp4E1=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1;_LL2B7: {
# 3316
int _tmp5EC=Cyc_Toc_set_in_sizeof(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4E1);
Cyc_Toc_set_in_sizeof(nv,_tmp5EC);
goto _LL291;}case 16: _LL2B8: _tmp4E0=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1;_LL2B9:
 e->r=(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp5ED=_cycalloc(sizeof(*_tmp5ED));_tmp5ED[0]=({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp5EE;_tmp5EE.tag=16;_tmp5EE.f1=Cyc_Toc_typ_to_c(_tmp4E0);_tmp5EE;});_tmp5ED;});goto _LL291;case 18: _LL2BA: _tmp4DE=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1;_tmp4DF=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp4B5)->f2;_LL2BB: {
# 3322
void*_tmp5EF=_tmp4DE;
struct Cyc_List_List*_tmp5F0=_tmp4DF;
for(0;_tmp5F0 != 0;_tmp5F0=_tmp5F0->tl){
void*_tmp5F1=(void*)_tmp5F0->hd;void*_tmp5F2=_tmp5F1;unsigned int _tmp5F3;struct _dyneither_ptr*_tmp5F4;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp5F2)->tag == 0){_LL37D: _tmp5F4=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp5F2)->f1;_LL37E:
 goto _LL37C;}else{_LL37F: _tmp5F3=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp5F2)->f1;_LL380:
# 3328
{void*_tmp5F5=Cyc_Tcutil_compress(_tmp5EF);void*_tmp5F6=_tmp5F5;struct Cyc_Absyn_Datatypefield*_tmp5F7;struct Cyc_List_List*_tmp5F8;struct Cyc_List_List*_tmp5F9;union Cyc_Absyn_AggrInfoU _tmp5FA;switch(*((int*)_tmp5F6)){case 11: _LL382: _tmp5FA=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5F6)->f1).aggr_info;_LL383: {
# 3330
struct Cyc_Absyn_Aggrdecl*_tmp5FB=Cyc_Absyn_get_known_aggrdecl(_tmp5FA);
if(_tmp5FB->impl == 0)
({void*_tmp5FC=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp5FD="struct fields must be known";_tag_dyneither(_tmp5FD,sizeof(char),28);}),_tag_dyneither(_tmp5FC,sizeof(void*),0));});
_tmp5F9=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp5FB->impl))->fields;goto _LL385;}case 12: _LL384: _tmp5F9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5F6)->f2;_LL385: {
# 3335
struct Cyc_Absyn_Aggrfield*_tmp5FE=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp5F9,(int)_tmp5F3);
_tmp5F0->hd=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp5FF=_cycalloc(sizeof(*_tmp5FF));_tmp5FF[0]=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp600;_tmp600.tag=0;_tmp600.f1=_tmp5FE->name;_tmp600;});_tmp5FF;}));
_tmp5EF=_tmp5FE->type;
goto _LL381;}case 10: _LL386: _tmp5F8=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5F6)->f1;_LL387:
# 3340
 _tmp5F0->hd=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp601=_cycalloc(sizeof(*_tmp601));_tmp601[0]=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp602;_tmp602.tag=0;_tmp602.f1=Cyc_Absyn_fieldname((int)(_tmp5F3 + 1));_tmp602;});_tmp601;}));
_tmp5EF=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp5F8,(int)_tmp5F3)).f2;
goto _LL381;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp5F6)->f1).field_info).KnownDatatypefield).tag == 2){_LL388: _tmp5F7=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp5F6)->f1).field_info).KnownDatatypefield).val).f2;_LL389:
# 3344
 if(_tmp5F3 == 0)
_tmp5F0->hd=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp603=_cycalloc(sizeof(*_tmp603));_tmp603[0]=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp604;_tmp604.tag=0;_tmp604.f1=Cyc_Toc_tag_sp;_tmp604;});_tmp603;}));else{
# 3347
_tmp5EF=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp5F7->typs,(int)(_tmp5F3 - 1))).f2;
_tmp5F0->hd=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp605=_cycalloc(sizeof(*_tmp605));_tmp605[0]=({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmp606;_tmp606.tag=0;_tmp606.f1=Cyc_Absyn_fieldname((int)_tmp5F3);_tmp606;});_tmp605;}));}
# 3350
goto _LL381;}else{goto _LL38A;}default: _LL38A: _LL38B:
({void*_tmp607=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp608="impossible type for offsetof tuple index";_tag_dyneither(_tmp608,sizeof(char),41);}),_tag_dyneither(_tmp607,sizeof(void*),0));});
goto _LL381;}_LL381:;}
# 3354
goto _LL37C;}_LL37C:;}
# 3357
e->r=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp609=_cycalloc(sizeof(*_tmp609));_tmp609[0]=({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp60A;_tmp60A.tag=18;_tmp60A.f1=Cyc_Toc_typ_to_c(_tmp4DE);_tmp60A.f2=_tmp4DF;_tmp60A;});_tmp609;});
goto _LL291;}case 19: _LL2BC: _tmp4DD=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1;_LL2BD: {
# 3360
int _tmp60B=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp60C=Cyc_Tcutil_compress((void*)_check_null(_tmp4DD->topt));
{void*_tmp60D=_tmp60C;void*_tmp60E;struct Cyc_Absyn_Tqual _tmp60F;void*_tmp610;union Cyc_Absyn_Constraint*_tmp611;union Cyc_Absyn_Constraint*_tmp612;union Cyc_Absyn_Constraint*_tmp613;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp60D)->tag == 5){_LL38D: _tmp60E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp60D)->f1).elt_typ;_tmp60F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp60D)->f1).elt_tq;_tmp610=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp60D)->f1).ptr_atts).rgn;_tmp611=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp60D)->f1).ptr_atts).nullable;_tmp612=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp60D)->f1).ptr_atts).bounds;_tmp613=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp60D)->f1).ptr_atts).zero_term;_LL38E:
# 3365
{void*_tmp614=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp612);void*_tmp615=_tmp614;struct Cyc_Absyn_Exp*_tmp616;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp615)->tag == 1){_LL392: _tmp616=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp615)->f1;_LL393: {
# 3367
int do_null_check=!Cyc_Toc_in_sizeof(nv) && Cyc_Toc_need_null_check(_tmp4DD);
Cyc_Toc_exp_to_c(nv,_tmp4DD);
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp617=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp618="inserted null check due to dereference";_tag_dyneither(_tmp618,sizeof(char),39);}),_tag_dyneither(_tmp617,sizeof(void*),0));});
# 3373
_tmp4DD->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(_tmp60C),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp619=_cycalloc(sizeof(*_tmp619));_tmp619->hd=
Cyc_Absyn_copy_exp(_tmp4DD);_tmp619->tl=0;_tmp619;}),0));}
# 3381
if(!Cyc_Toc_in_sizeof(nv) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp613)){
struct _tuple12 _tmp61A=Cyc_Evexp_eval_const_uint_exp(_tmp616);struct _tuple12 _tmp61B=_tmp61A;unsigned int _tmp61C;int _tmp61D;_LL397: _tmp61C=_tmp61B.f1;_tmp61D=_tmp61B.f2;_LL398:;
# 3387
if(!_tmp61D  || _tmp61C <= 0)
({void*_tmp61E=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp61F="cannot determine dereference is in bounds";_tag_dyneither(_tmp61F,sizeof(char),42);}),_tag_dyneither(_tmp61E,sizeof(void*),0));});}
# 3390
goto _LL391;}}else{_LL394: _LL395: {
# 3393
struct Cyc_Absyn_Exp*_tmp620=Cyc_Absyn_uint_exp(0,0);
_tmp620->topt=Cyc_Absyn_uint_typ;
e->r=Cyc_Toc_subscript_exp_r(_tmp4DD,_tmp620);
Cyc_Toc_exp_to_c(nv,e);
goto _LL391;}}_LL391:;}
# 3399
goto _LL38C;}else{_LL38F: _LL390:
({void*_tmp621=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp622="exp_to_c: Deref: non-pointer";_tag_dyneither(_tmp622,sizeof(char),29);}),_tag_dyneither(_tmp621,sizeof(void*),0));});}_LL38C:;}
# 3402
Cyc_Toc_set_lhs(nv,_tmp60B);
goto _LL291;};}case 20: _LL2BE: _tmp4D9=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1;_tmp4DA=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4B5)->f2;_tmp4DB=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4B5)->f3;_tmp4DC=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4B5)->f4;_LL2BF: {
# 3405
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp4D9->topt);
Cyc_Toc_exp_to_c(nv,_tmp4D9);
if(_tmp4DB  && _tmp4DC)
e->r=Cyc_Toc_check_tagged_union(_tmp4D9,Cyc_Toc_typ_to_c(e1_cyc_type),e1_cyc_type,_tmp4DA,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);
# 3413
if(is_poly)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
goto _LL291;}case 21: _LL2C0: _tmp4D5=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1;_tmp4D6=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4B5)->f2;_tmp4D7=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4B5)->f3;_tmp4D8=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4B5)->f4;_LL2C1: {
# 3417
int _tmp623=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp4D5->topt));
int do_null_check=Cyc_Toc_need_null_check(_tmp4D5);
Cyc_Toc_exp_to_c(nv,_tmp4D5);{
int is_poly=Cyc_Toc_is_poly_project(e);
struct Cyc_Absyn_PtrInfo _tmp624=Cyc_Toc_get_ptr_type(e1typ);struct Cyc_Absyn_PtrInfo _tmp625=_tmp624;void*_tmp626;struct Cyc_Absyn_Tqual _tmp627;void*_tmp628;union Cyc_Absyn_Constraint*_tmp629;union Cyc_Absyn_Constraint*_tmp62A;union Cyc_Absyn_Constraint*_tmp62B;_LL39A: _tmp626=_tmp625.elt_typ;_tmp627=_tmp625.elt_tq;_tmp628=(_tmp625.ptr_atts).rgn;_tmp629=(_tmp625.ptr_atts).nullable;_tmp62A=(_tmp625.ptr_atts).bounds;_tmp62B=(_tmp625.ptr_atts).zero_term;_LL39B:;
{void*_tmp62C=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp62A);void*_tmp62D=_tmp62C;struct Cyc_Absyn_Exp*_tmp62E;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp62D)->tag == 1){_LL39D: _tmp62E=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp62D)->f1;_LL39E: {
# 3428
struct _tuple12 _tmp62F=Cyc_Evexp_eval_const_uint_exp(_tmp62E);struct _tuple12 _tmp630=_tmp62F;unsigned int _tmp631;int _tmp632;_LL3A2: _tmp631=_tmp630.f1;_tmp632=_tmp630.f2;_LL3A3:;
if(_tmp632){
if(_tmp631 < 1)
({void*_tmp633=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp634="exp_to_c:  AggrArrow_e on pointer of size 0";_tag_dyneither(_tmp634,sizeof(char),44);}),_tag_dyneither(_tmp633,sizeof(void*),0));});
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp635=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp636="inserted null check due to dereference";_tag_dyneither(_tmp636,sizeof(char),39);}),_tag_dyneither(_tmp635,sizeof(void*),0));});
# 3436
_tmp4D5->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_Absyn_Exp*_tmp637[1];_tmp637[0]=
Cyc_Absyn_new_exp(_tmp4D5->r,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp637,sizeof(struct Cyc_Absyn_Exp*),1));}),0));}}else{
# 3441
if(!Cyc_Evexp_c_can_eval(_tmp62E))
({void*_tmp638=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp639="cannot determine pointer dereference in bounds";_tag_dyneither(_tmp639,sizeof(char),47);}),_tag_dyneither(_tmp638,sizeof(void*),0));});
# 3445
_tmp4D5->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*_tmp63A[4];_tmp63A[3]=
# 3449
Cyc_Absyn_uint_exp(0,0);_tmp63A[2]=
# 3448
Cyc_Absyn_sizeoftyp_exp(_tmp626,0);_tmp63A[1]=_tmp62E;_tmp63A[0]=
# 3447
Cyc_Absyn_new_exp(_tmp4D5->r,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp63A,sizeof(struct Cyc_Absyn_Exp*),4));}),0));}
# 3451
goto _LL39C;}}else{_LL39F: _LL3A0: {
# 3454
void*ta1=Cyc_Toc_typ_to_c(_tmp626);
_tmp4D5->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp627),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_dyneither_subscript_e,({struct Cyc_Absyn_Exp*_tmp63B[3];_tmp63B[2]=
# 3459
Cyc_Absyn_uint_exp(0,0);_tmp63B[1]=
# 3458
Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp63B[0]=
# 3457
Cyc_Absyn_new_exp(_tmp4D5->r,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp63B,sizeof(struct Cyc_Absyn_Exp*),3));}),0));
# 3462
goto _LL39C;}}_LL39C:;}
# 3464
if(_tmp4D7  && _tmp4D8)
e->r=Cyc_Toc_check_tagged_union(_tmp4D5,Cyc_Toc_typ_to_c(e1typ),_tmp626,_tmp4D6,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);
# 3467
if(is_poly  && _tmp4D8)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
Cyc_Toc_set_lhs(nv,_tmp623);
goto _LL291;};};}case 22: _LL2C2: _tmp4D3=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1;_tmp4D4=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4B5)->f2;_LL2C3: {
# 3472
int _tmp63C=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp63D=Cyc_Tcutil_compress((void*)_check_null(_tmp4D3->topt));
# 3476
{void*_tmp63E=_tmp63D;void*_tmp63F;struct Cyc_Absyn_Tqual _tmp640;void*_tmp641;union Cyc_Absyn_Constraint*_tmp642;union Cyc_Absyn_Constraint*_tmp643;union Cyc_Absyn_Constraint*_tmp644;struct Cyc_List_List*_tmp645;switch(*((int*)_tmp63E)){case 10: _LL3A5: _tmp645=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp63E)->f1;_LL3A6:
# 3479
 Cyc_Toc_exp_to_c(nv,_tmp4D3);
Cyc_Toc_exp_to_c(nv,_tmp4D4);{
struct _tuple12 _tmp646=Cyc_Evexp_eval_const_uint_exp(_tmp4D4);struct _tuple12 _tmp647=_tmp646;unsigned int _tmp648;int _tmp649;_LL3AC: _tmp648=_tmp647.f1;_tmp649=_tmp647.f2;_LL3AD:;
if(!_tmp649)
({void*_tmp64A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp64B="unknown tuple subscript in translation to C";_tag_dyneither(_tmp64B,sizeof(char),44);}),_tag_dyneither(_tmp64A,sizeof(void*),0));});
e->r=Cyc_Toc_aggrmember_exp_r(_tmp4D3,Cyc_Absyn_fieldname((int)(_tmp648 + 1)));
goto _LL3A4;};case 5: _LL3A7: _tmp63F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp63E)->f1).elt_typ;_tmp640=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp63E)->f1).elt_tq;_tmp641=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp63E)->f1).ptr_atts).rgn;_tmp642=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp63E)->f1).ptr_atts).nullable;_tmp643=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp63E)->f1).ptr_atts).bounds;_tmp644=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp63E)->f1).ptr_atts).zero_term;_LL3A8: {
# 3487
struct Cyc_List_List*_tmp64C=Cyc_Toc_get_relns(_tmp4D3);
int _tmp64D=Cyc_Toc_need_null_check(_tmp4D3);
int _tmp64E=Cyc_Toc_in_sizeof(nv);
# 3495
int in_bnds=_tmp64E;
{void*_tmp64F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp643);void*_tmp650=_tmp64F;_LL3AF: _LL3B0:
# 3498
 in_bnds=in_bnds  || Cyc_Toc_check_bounds(_tmp63D,_tmp64C,_tmp4D3,_tmp4D4);
if(Cyc_Toc_warn_bounds_checks  && !in_bnds)
({struct Cyc_String_pa_PrintArg_struct _tmp653;_tmp653.tag=0;_tmp653.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp651[1]={& _tmp653};Cyc_Tcutil_warn(e->loc,({const char*_tmp652="bounds check necessary for %s";_tag_dyneither(_tmp652,sizeof(char),30);}),_tag_dyneither(_tmp651,sizeof(void*),1));});});_LL3AE:;}
# 3505
Cyc_Toc_exp_to_c(nv,_tmp4D3);
Cyc_Toc_exp_to_c(nv,_tmp4D4);
++ Cyc_Toc_total_bounds_checks;
{void*_tmp654=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp643);void*_tmp655=_tmp654;struct Cyc_Absyn_Exp*_tmp656;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp655)->tag == 1){_LL3B2: _tmp656=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp655)->f1;_LL3B3: {
# 3510
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp642) && _tmp64D;
void*ta1=Cyc_Toc_typ_to_c(_tmp63F);
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp640);
if(in_bnds  && !possibly_null)
++ Cyc_Toc_bounds_checks_eliminated;else{
if(in_bnds  && possibly_null){
++ Cyc_Toc_bounds_checks_eliminated;
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp657=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp658="inserted null check due to dereference";_tag_dyneither(_tmp658,sizeof(char),39);}),_tag_dyneither(_tmp657,sizeof(void*),0));});
_tmp4D3->r=Cyc_Toc_cast_it_r(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_Absyn_Exp*_tmp659[1];_tmp659[0]=
Cyc_Absyn_copy_exp(_tmp4D3);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp659,sizeof(struct Cyc_Absyn_Exp*),1));}),0));}else{
# 3522
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp644)){
# 3524
if(!Cyc_Evexp_c_can_eval(_tmp656))
({void*_tmp65A=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp65B="cannot determine subscript is in bounds";_tag_dyneither(_tmp65B,sizeof(char),40);}),_tag_dyneither(_tmp65A,sizeof(void*),0));});{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp4D3);
e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(function_e,({struct Cyc_Absyn_Exp*_tmp65C[3];_tmp65C[2]=_tmp4D4;_tmp65C[1]=_tmp656;_tmp65C[0]=_tmp4D3;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp65C,sizeof(struct Cyc_Absyn_Exp*),3));}),0)));};}else{
# 3530
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp656))
({void*_tmp65D=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp65E="cannot determine subscript is in bounds";_tag_dyneither(_tmp65E,sizeof(char),40);}),_tag_dyneither(_tmp65D,sizeof(void*),0));});
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp65F=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp660="inserted null check due to dereference";_tag_dyneither(_tmp660,sizeof(char),39);}),_tag_dyneither(_tmp65F,sizeof(void*),0));});
# 3536
e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*_tmp661[4];_tmp661[3]=_tmp4D4;_tmp661[2]=
# 3539
Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp661[1]=_tmp656;_tmp661[0]=_tmp4D3;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp661,sizeof(struct Cyc_Absyn_Exp*),4));}),0)));}else{
# 3542
if(!Cyc_Evexp_c_can_eval(_tmp656))
({void*_tmp662=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp663="cannot determine subscript is in bounds";_tag_dyneither(_tmp663,sizeof(char),40);}),_tag_dyneither(_tmp662,sizeof(void*),0));});
# 3545
_tmp4D4->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,({struct Cyc_Absyn_Exp*_tmp664[2];_tmp664[1]=
Cyc_Absyn_copy_exp(_tmp4D4);_tmp664[0]=_tmp656;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp664,sizeof(struct Cyc_Absyn_Exp*),2));}));}}}}
# 3548
goto _LL3B1;}}else{_LL3B4: _LL3B5: {
# 3550
void*ta1=Cyc_Toc_typ_to_c(_tmp63F);
if(in_bnds){
# 3554
++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp640),
Cyc_Toc_member_exp(_tmp4D3,Cyc_Toc_curr_sp,0)),_tmp4D4);}else{
# 3559
struct Cyc_Absyn_Exp*_tmp665=Cyc_Toc__check_dyneither_subscript_e;
e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp640),
Cyc_Absyn_fncall_exp(_tmp665,({struct Cyc_Absyn_Exp*_tmp666[3];_tmp666[2]=_tmp4D4;_tmp666[1]=
Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp666[0]=_tmp4D3;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp666,sizeof(struct Cyc_Absyn_Exp*),3));}),0)));}
# 3565
goto _LL3B1;}}_LL3B1:;}
# 3567
goto _LL3A4;}default: _LL3A9: _LL3AA:
({void*_tmp667=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp668="exp_to_c: Subscript on non-tuple/array/tuple ptr";_tag_dyneither(_tmp668,sizeof(char),49);}),_tag_dyneither(_tmp667,sizeof(void*),0));});}_LL3A4:;}
# 3570
Cyc_Toc_set_lhs(nv,_tmp63C);
goto _LL291;};}case 23: _LL2C4: _tmp4D2=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1;_LL2C5:
# 3573
 if(!Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp4D2))->r;else{
# 3578
struct Cyc_List_List*_tmp669=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp4D2);
void*_tmp66A=Cyc_Toc_add_tuple_type(_tmp669);
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp4D2 != 0;(_tmp4D2=_tmp4D2->tl,i ++)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4D2->hd);
dles=({struct Cyc_List_List*_tmp66B=_cycalloc(sizeof(*_tmp66B));_tmp66B->hd=({struct _tuple19*_tmp66C=_cycalloc(sizeof(*_tmp66C));_tmp66C->f1=0;_tmp66C->f2=(struct Cyc_Absyn_Exp*)_tmp4D2->hd;_tmp66C;});_tmp66B->tl=dles;_tmp66B;});}}
# 3585
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}
# 3588
goto _LL291;case 25: _LL2C6: _tmp4D1=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1;_LL2C7:
# 3592
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp4D1);
{struct Cyc_List_List*_tmp66D=_tmp4D1;for(0;_tmp66D != 0;_tmp66D=_tmp66D->tl){
struct _tuple19*_tmp66E=(struct _tuple19*)_tmp66D->hd;struct _tuple19*_tmp66F=_tmp66E;struct Cyc_Absyn_Exp*_tmp670;_LL3B7: _tmp670=_tmp66F->f2;_LL3B8:;
Cyc_Toc_exp_to_c(nv,_tmp670);}}
# 3597
goto _LL291;case 26: _LL2C8: _tmp4CD=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1;_tmp4CE=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4B5)->f2;_tmp4CF=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4B5)->f3;_tmp4D0=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4B5)->f4;_LL2C9: {
# 3601
struct _tuple12 _tmp671=Cyc_Evexp_eval_const_uint_exp(_tmp4CE);struct _tuple12 _tmp672=_tmp671;unsigned int _tmp673;int _tmp674;_LL3BA: _tmp673=_tmp672.f1;_tmp674=_tmp672.f2;_LL3BB:;{
void*_tmp675=Cyc_Toc_typ_to_c((void*)_check_null(_tmp4CF->topt));
Cyc_Toc_exp_to_c(nv,_tmp4CF);{
struct Cyc_List_List*es=0;
# 3606
if(!Cyc_Toc_is_zero(_tmp4CF)){
if(!_tmp674)
({void*_tmp676=0;Cyc_Tcutil_terr(_tmp4CE->loc,({const char*_tmp677="cannot determine value of constant";_tag_dyneither(_tmp677,sizeof(char),35);}),_tag_dyneither(_tmp676,sizeof(void*),0));});
{unsigned int i=0;for(0;i < _tmp673;++ i){
es=({struct Cyc_List_List*_tmp678=_cycalloc(sizeof(*_tmp678));_tmp678->hd=({struct _tuple19*_tmp679=_cycalloc(sizeof(*_tmp679));_tmp679->f1=0;_tmp679->f2=_tmp4CF;_tmp679;});_tmp678->tl=es;_tmp678;});}}
# 3612
if(_tmp4D0){
struct Cyc_Absyn_Exp*_tmp67A=Cyc_Toc_cast_it(_tmp675,Cyc_Absyn_uint_exp(0,0));
es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,({struct Cyc_List_List*_tmp67B=_cycalloc(sizeof(*_tmp67B));_tmp67B->hd=({struct _tuple19*_tmp67C=_cycalloc(sizeof(*_tmp67C));_tmp67C->f1=0;_tmp67C->f2=_tmp67A;_tmp67C;});_tmp67B->tl=0;_tmp67B;}));}}
# 3617
e->r=Cyc_Toc_unresolvedmem_exp_r(0,es);
goto _LL291;};};}case 27: _LL2CA: _tmp4CA=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1;_tmp4CB=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4B5)->f2;_tmp4CC=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4B5)->f3;_LL2CB:
# 3622
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,0);
goto _LL291;case 28: _LL2CC: _tmp4C6=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1;_tmp4C7=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4B5)->f2;_tmp4C8=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4B5)->f3;_tmp4C9=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4B5)->f4;_LL2CD:
# 3627
 if(!Cyc_Toc_is_toplevel(nv)){
struct Cyc_Absyn_Exp*_tmp67D=Cyc_Toc_init_struct(nv,old_typ,_tmp4C7,0,0,_tmp4C8,_tmp4C6);
e->r=_tmp67D->r;
e->topt=_tmp67D->topt;}else{
# 3638
if(_tmp4C9 == 0)
({void*_tmp67E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp67F="Aggregate_e: missing aggrdecl pointer";_tag_dyneither(_tmp67F,sizeof(char),38);}),_tag_dyneither(_tmp67E,sizeof(void*),0));});{
struct Cyc_Absyn_Aggrdecl*sd2=_tmp4C9;
# 3642
struct _RegionHandle _tmp680=_new_region("rgn");struct _RegionHandle*rgn=& _tmp680;_push_region(rgn);
{struct Cyc_List_List*_tmp681=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp4C8,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
# 3646
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4C9->impl))->tagged){
# 3648
struct _tuple30*_tmp682=(struct _tuple30*)((struct Cyc_List_List*)_check_null(_tmp681))->hd;struct _tuple30*_tmp683=_tmp682;struct Cyc_Absyn_Aggrfield*_tmp684;struct Cyc_Absyn_Exp*_tmp685;_LL3BD: _tmp684=_tmp683->f1;_tmp685=_tmp683->f2;_LL3BE:;{
void*_tmp686=(void*)_check_null(_tmp685->topt);
void*_tmp687=_tmp684->type;
Cyc_Toc_exp_to_c(nv,_tmp685);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp687) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp686))
_tmp685->r=
Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp685->r,0));{
# 3657
int i=Cyc_Toc_get_member_offset(_tmp4C9,_tmp684->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct Cyc_List_List*_tmp688=({struct _tuple19*_tmp68E[2];_tmp68E[1]=({struct _tuple19*_tmp690=_cycalloc(sizeof(*_tmp690));_tmp690->f1=0;_tmp690->f2=_tmp685;_tmp690;});_tmp68E[0]=({struct _tuple19*_tmp68F=_cycalloc(sizeof(*_tmp68F));_tmp68F->f1=0;_tmp68F->f2=field_tag_exp;_tmp68F;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp68E,sizeof(struct _tuple19*),2));});
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp688,0);
struct Cyc_List_List*ds=({void*_tmp68B[1];_tmp68B[0]=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp68C=_cycalloc(sizeof(*_tmp68C));_tmp68C[0]=({struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp68D;_tmp68D.tag=1;_tmp68D.f1=_tmp684->name;_tmp68D;});_tmp68C;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp68B,sizeof(void*),1));});
struct Cyc_List_List*dles=({struct _tuple19*_tmp689[1];_tmp689[0]=({struct _tuple19*_tmp68A=_cycalloc(sizeof(*_tmp68A));_tmp68A->f1=ds;_tmp68A->f2=umem;_tmp68A;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp689,sizeof(struct _tuple19*),1));});
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{
# 3666
struct Cyc_List_List*_tmp691=0;
struct Cyc_List_List*_tmp692=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields;
for(0;_tmp692 != 0;_tmp692=_tmp692->tl){
struct Cyc_List_List*_tmp693=_tmp681;for(0;_tmp693 != 0;_tmp693=_tmp693->tl){
if((*((struct _tuple30*)_tmp693->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp692->hd){
struct _tuple30*_tmp694=(struct _tuple30*)_tmp693->hd;struct _tuple30*_tmp695=_tmp694;struct Cyc_Absyn_Aggrfield*_tmp696;struct Cyc_Absyn_Exp*_tmp697;_LL3C0: _tmp696=_tmp695->f1;_tmp697=_tmp695->f2;_LL3C1:;{
void*_tmp698=Cyc_Toc_typ_to_c(_tmp696->type);
void*_tmp699=Cyc_Toc_typ_to_c((void*)_check_null(_tmp697->topt));
Cyc_Toc_exp_to_c(nv,_tmp697);
# 3676
if(!Cyc_Tcutil_unify(_tmp698,_tmp699)){
# 3678
if(!Cyc_Tcutil_is_arithmetic_type(_tmp698) || !
Cyc_Tcutil_is_arithmetic_type(_tmp699))
_tmp697=Cyc_Toc_cast_it(_tmp698,Cyc_Absyn_copy_exp(_tmp697));}
_tmp691=({struct Cyc_List_List*_tmp69A=_cycalloc(sizeof(*_tmp69A));_tmp69A->hd=({struct _tuple19*_tmp69B=_cycalloc(sizeof(*_tmp69B));_tmp69B->f1=0;_tmp69B->f2=_tmp697;_tmp69B;});_tmp69A->tl=_tmp691;_tmp69A;});
break;};}}}
# 3685
e->r=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp691));}}
# 3643
;_pop_region(rgn);};}
# 3689
goto _LL291;case 29: _LL2CE: _tmp4C4=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1;_tmp4C5=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp4B5)->f2;_LL2CF: {
# 3691
struct Cyc_List_List*fs;
{void*_tmp69C=Cyc_Tcutil_compress(_tmp4C4);void*_tmp69D=_tmp69C;struct Cyc_List_List*_tmp69E;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp69D)->tag == 12){_LL3C3: _tmp69E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp69D)->f2;_LL3C4:
 fs=_tmp69E;goto _LL3C2;}else{_LL3C5: _LL3C6:
({struct Cyc_String_pa_PrintArg_struct _tmp6A1;_tmp6A1.tag=0;_tmp6A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp4C4));({void*_tmp69F[1]={& _tmp6A1};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp6A0="anon struct has type %s";_tag_dyneither(_tmp6A0,sizeof(char),24);}),_tag_dyneither(_tmp69F,sizeof(void*),1));});});}_LL3C2:;}{
# 3696
struct _RegionHandle _tmp6A2=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6A2;_push_region(rgn);{
struct Cyc_List_List*_tmp6A3=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp4C5,Cyc_Absyn_StructA,fs);
for(0;_tmp6A3 != 0;_tmp6A3=_tmp6A3->tl){
struct _tuple30*_tmp6A4=(struct _tuple30*)_tmp6A3->hd;struct _tuple30*_tmp6A5=_tmp6A4;struct Cyc_Absyn_Aggrfield*_tmp6A6;struct Cyc_Absyn_Exp*_tmp6A7;_LL3C8: _tmp6A6=_tmp6A5->f1;_tmp6A7=_tmp6A5->f2;_LL3C9:;{
void*_tmp6A8=(void*)_check_null(_tmp6A7->topt);
void*_tmp6A9=_tmp6A6->type;
Cyc_Toc_exp_to_c(nv,_tmp6A7);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6A9) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6A8))
_tmp6A7->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp6A7->r,0));};}
# 3710
e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp4C5);}
# 3712
_npop_handler(0);goto _LL291;
# 3696
;_pop_region(rgn);};}case 30: _LL2D0: _tmp4C1=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1;_tmp4C2=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4B5)->f2;_tmp4C3=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4B5)->f3;_LL2D1: {
# 3715
void*datatype_ctype;
struct Cyc_Absyn_Exp*tag_exp;
struct _tuple1*_tmp6AA=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp6AB=Cyc_Absyn_var_exp(_tmp6AA,0);
struct Cyc_Absyn_Exp*mem_exp;
datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp4C3->name,_tmp4C2->name));
tag_exp=_tmp4C2->is_extensible?Cyc_Absyn_var_exp(_tmp4C3->name,0):
 Cyc_Toc_datatype_tag(_tmp4C2,_tmp4C3->name);
mem_exp=_tmp6AB;{
struct Cyc_List_List*_tmp6AC=_tmp4C3->typs;
# 3726
if(Cyc_Toc_is_toplevel(nv)){
# 3728
struct Cyc_List_List*dles=0;
for(0;_tmp4C1 != 0;(_tmp4C1=_tmp4C1->tl,_tmp6AC=_tmp6AC->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp4C1->hd;
void*_tmp6AD=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp6AC))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp6AD))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);
dles=({struct Cyc_List_List*_tmp6AE=_cycalloc(sizeof(*_tmp6AE));_tmp6AE->hd=({struct _tuple19*_tmp6AF=_cycalloc(sizeof(*_tmp6AF));_tmp6AF->f1=0;_tmp6AF->f2=cur_e;_tmp6AF;});_tmp6AE->tl=dles;_tmp6AE;});}
# 3739
dles=({struct Cyc_List_List*_tmp6B0=_cycalloc(sizeof(*_tmp6B0));_tmp6B0->hd=({struct _tuple19*_tmp6B1=_cycalloc(sizeof(*_tmp6B1));_tmp6B1->f1=0;_tmp6B1->f2=tag_exp;_tmp6B1;});_tmp6B0->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp6B0;});
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{
# 3745
struct Cyc_List_List*_tmp6B2=({struct Cyc_List_List*_tmp6B9=_cycalloc(sizeof(*_tmp6B9));_tmp6B9->hd=
Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Toc_tag_sp,0),tag_exp,0);_tmp6B9->tl=0;_tmp6B9;});
# 3748
{int i=1;for(0;_tmp4C1 != 0;(((_tmp4C1=_tmp4C1->tl,i ++)),_tmp6AC=_tmp6AC->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp4C1->hd;
void*_tmp6B3=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp6AC))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp6B3))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct Cyc_Absyn_Stmt*_tmp6B4=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Absyn_fieldname(i),0),cur_e,0);
# 3758
_tmp6B2=({struct Cyc_List_List*_tmp6B5=_cycalloc(sizeof(*_tmp6B5));_tmp6B5->hd=_tmp6B4;_tmp6B5->tl=_tmp6B2;_tmp6B5;});};}}{
# 3760
struct Cyc_Absyn_Stmt*_tmp6B6=Cyc_Absyn_exp_stmt(_tmp6AB,0);
struct Cyc_Absyn_Stmt*_tmp6B7=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(({struct Cyc_List_List*_tmp6B8=_cycalloc(sizeof(*_tmp6B8));_tmp6B8->hd=_tmp6B6;_tmp6B8->tl=_tmp6B2;_tmp6B8;})),0);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp6AA,datatype_ctype,0,_tmp6B7,0));};}
# 3764
goto _LL291;};}case 31: _LL2D2: _LL2D3:
# 3766
 goto _LL2D5;case 32: _LL2D4: _LL2D5:
 goto _LL291;case 33: _LL2D6: _tmp4BB=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1).is_calloc;_tmp4BC=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1).rgn;_tmp4BD=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1).elt_type;_tmp4BE=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1).num_elts;_tmp4BF=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1).fat_result;_tmp4C0=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1).inline_call;_LL2D7: {
# 3770
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp4BD)));
Cyc_Toc_exp_to_c(nv,_tmp4BE);
# 3774
if(_tmp4BF){
struct _tuple1*_tmp6BA=Cyc_Toc_temp_var();
struct _tuple1*_tmp6BB=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp4BB){
xexp=_tmp4BE;
if(_tmp4BC != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp4BC;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp6BA,0));}else{
# 3785
pexp=Cyc_Toc_calloc_exp(*_tmp4BD,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp6BA,0));}
# 3787
rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*_tmp6BC[3];_tmp6BC[2]=
# 3789
Cyc_Absyn_var_exp(_tmp6BA,0);_tmp6BC[1]=
# 3788
Cyc_Absyn_sizeoftyp_exp(t_c,0);_tmp6BC[0]=Cyc_Absyn_var_exp(_tmp6BB,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp6BC,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}else{
# 3791
if(_tmp4BC != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp4BC;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp4C0)
pexp=Cyc_Toc_rmalloc_inline_exp(rgn,Cyc_Absyn_var_exp(_tmp6BA,0));else{
# 3797
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp6BA,0));}}else{
# 3799
pexp=Cyc_Toc_malloc_exp(*_tmp4BD,Cyc_Absyn_var_exp(_tmp6BA,0));}
# 3801
rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*_tmp6BD[3];_tmp6BD[2]=
Cyc_Absyn_var_exp(_tmp6BA,0);_tmp6BD[1]=
# 3801
Cyc_Absyn_uint_exp(1,0);_tmp6BD[0]=Cyc_Absyn_var_exp(_tmp6BB,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp6BD,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}{
# 3804
struct Cyc_Absyn_Stmt*_tmp6BE=Cyc_Absyn_declare_stmt(_tmp6BA,Cyc_Absyn_uint_typ,_tmp4BE,
Cyc_Absyn_declare_stmt(_tmp6BB,Cyc_Absyn_cstar_typ(t_c,Cyc_Toc_mt_tq),pexp,
Cyc_Absyn_exp_stmt(rexp,0),0),0);
e->r=Cyc_Toc_stmt_exp_r(_tmp6BE);};}else{
# 3809
if(_tmp4BB){
if(_tmp4BC != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp4BC;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp4BE))->r;}else{
# 3815
e->r=(Cyc_Toc_calloc_exp(*_tmp4BD,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp4BE))->r;}}else{
# 3818
if(_tmp4BC != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp4BC;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp4C0)
e->r=(Cyc_Toc_rmalloc_inline_exp(rgn,_tmp4BE))->r;else{
# 3824
e->r=(Cyc_Toc_rmalloc_exp(rgn,_tmp4BE))->r;}}else{
# 3826
e->r=(Cyc_Toc_malloc_exp(*_tmp4BD,_tmp4BE))->r;}}}
# 3830
goto _LL291;}case 34: _LL2D8: _tmp4B9=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1;_tmp4BA=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp4B5)->f2;_LL2D9: {
# 3839
void*e1_old_typ=(void*)_check_null(_tmp4B9->topt);
void*e2_old_typ=(void*)_check_null(_tmp4BA->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ) && !Cyc_Tcutil_is_pointer_type(e1_old_typ))
({void*_tmp6BF=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp6C0="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";_tag_dyneither(_tmp6C0,sizeof(char),57);}),_tag_dyneither(_tmp6BF,sizeof(void*),0));});{
# 3845
struct Cyc_Absyn_Exp*swap_fn;
if(Cyc_Tcutil_is_dyneither_ptr(e1_old_typ))
swap_fn=Cyc_Toc__swap_dyneither_e;else{
# 3849
swap_fn=Cyc_Toc__swap_word_e;}{
# 3853
int f1_tag=0;
void*tagged_mem_type1=(void*)& Cyc_Absyn_VoidType_val;
int e1_tmem=Cyc_Toc_is_tagged_union_project(_tmp4B9,& f1_tag,& tagged_mem_type1,1);
int f2_tag=0;
void*tagged_mem_type2=(void*)& Cyc_Absyn_VoidType_val;
int e2_tmem=Cyc_Toc_is_tagged_union_project(_tmp4BA,& f2_tag,& tagged_mem_type2,1);
# 3860
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4B9);
Cyc_Toc_exp_to_c(nv,_tmp4BA);
Cyc_Toc_set_lhs(nv,0);
# 3866
if(e1_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp4B9,f1_tag,tagged_mem_type1);else{
# 3870
_tmp4B9=Cyc_Toc_push_address_exp(_tmp4B9);}
# 3872
if(e2_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp4BA,f2_tag,tagged_mem_type2);else{
# 3876
_tmp4BA=Cyc_Toc_push_address_exp(_tmp4BA);}
# 3878
e->r=Cyc_Toc_fncall_exp_r(swap_fn,({struct Cyc_Absyn_Exp*_tmp6C1[2];_tmp6C1[1]=_tmp4BA;_tmp6C1[0]=_tmp4B9;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp6C1,sizeof(struct Cyc_Absyn_Exp*),2));}));
# 3880
goto _LL291;};};}case 37: _LL2DA: _tmp4B7=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1;_tmp4B8=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp4B5)->f2;_LL2DB: {
# 3883
void*_tmp6C2=Cyc_Tcutil_compress((void*)_check_null(_tmp4B7->topt));
Cyc_Toc_exp_to_c(nv,_tmp4B7);
{void*_tmp6C3=_tmp6C2;struct Cyc_Absyn_Aggrdecl*_tmp6C4;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp6C3)->tag == 11){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp6C3)->f1).aggr_info).KnownAggr).tag == 2){_LL3CB: _tmp6C4=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp6C3)->f1).aggr_info).KnownAggr).val;_LL3CC: {
# 3887
struct Cyc_Absyn_Exp*_tmp6C5=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp6C4,_tmp4B8),0);
struct Cyc_Absyn_Exp*_tmp6C6=Cyc_Toc_member_exp(_tmp4B7,_tmp4B8,0);
struct Cyc_Absyn_Exp*_tmp6C7=Cyc_Toc_member_exp(_tmp6C6,Cyc_Toc_tag_sp,0);
e->r=(Cyc_Absyn_eq_exp(_tmp6C7,_tmp6C5,0))->r;
goto _LL3CA;}}else{goto _LL3CD;}}else{_LL3CD: _LL3CE:
({void*_tmp6C8=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp6C9="non-aggregate type in tagcheck";_tag_dyneither(_tmp6C9,sizeof(char),31);}),_tag_dyneither(_tmp6C8,sizeof(void*),0));});}_LL3CA:;}
# 3894
goto _LL291;}case 36: _LL2DC: _tmp4B6=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp4B5)->f1;_LL2DD:
 Cyc_Toc_stmt_to_c(nv,_tmp4B6);goto _LL291;case 35: _LL2DE: _LL2DF:
({void*_tmp6CA=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp6CB="UnresolvedMem";_tag_dyneither(_tmp6CB,sizeof(char),14);}),_tag_dyneither(_tmp6CA,sizeof(void*),0));});case 24: _LL2E0: _LL2E1:
({void*_tmp6CC=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp6CD="compoundlit";_tag_dyneither(_tmp6CD,sizeof(char),12);}),_tag_dyneither(_tmp6CC,sizeof(void*),0));});case 38: _LL2E2: _LL2E3:
({void*_tmp6CE=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp6CF="valueof(-)";_tag_dyneither(_tmp6CF,sizeof(char),11);}),_tag_dyneither(_tmp6CE,sizeof(void*),0));});default: _LL2E4: _LL2E5:
({void*_tmp6D0=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp6D1="__asm__";_tag_dyneither(_tmp6D1,sizeof(char),8);}),_tag_dyneither(_tmp6D0,sizeof(void*),0));});}_LL291:;};}struct _tuple31{int f1;struct _dyneither_ptr*f2;struct _dyneither_ptr*f3;struct Cyc_Absyn_Switch_clause*f4;};
# 3927 "toc.cyc"
static struct _tuple31*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 3929
return({struct _tuple31*_tmp6D2=_region_malloc(r,sizeof(*_tmp6D2));_tmp6D2->f1=0;_tmp6D2->f2=Cyc_Toc_fresh_label();_tmp6D2->f3=Cyc_Toc_fresh_label();_tmp6D2->f4=sc;_tmp6D2;});}
# 3934
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(0;ps != 0;ps=((struct Cyc_List_List*)_check_null(ps))->tl){
struct Cyc_Tcpat_PathNode*_tmp6D3=(struct Cyc_Tcpat_PathNode*)ps->hd;
# 3940
if((int)(((_tmp6D3->orig_pat).pattern).tag == 1)){
void*t=(void*)_check_null(({union Cyc_Tcpat_PatOrWhere _tmp6D6=_tmp6D3->orig_pat;if((_tmp6D6.pattern).tag != 1)_throw_match();(_tmp6D6.pattern).val;})->topt);
void*_tmp6D4=Cyc_Tcutil_compress(Cyc_Toc_typ_to_c_array(t));
void*_tmp6D5=_tmp6D4;switch(*((int*)_tmp6D5)){case 0: _LL3D0: _LL3D1:
# 3945
 goto _LL3D3;case 11: _LL3D2: _LL3D3:
 goto _LL3D5;case 12: _LL3D4: _LL3D5:
 goto _LL3CF;default: _LL3D6: _LL3D7:
 v=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c_array(t),v);goto _LL3CF;}_LL3CF:;}{
# 3951
void*_tmp6D7=_tmp6D3->access;void*_tmp6D8=_tmp6D7;struct Cyc_Absyn_Datatypedecl*_tmp6D9;struct Cyc_Absyn_Datatypefield*_tmp6DA;unsigned int _tmp6DB;int _tmp6DC;struct _dyneither_ptr*_tmp6DD;unsigned int _tmp6DE;switch(*((int*)_tmp6D8)){case 0: _LL3D9: _LL3DA:
# 3956
 goto _LL3D8;case 1: _LL3DB: _LL3DC:
# 3961
 if(ps->tl != 0){
void*_tmp6DF=((struct Cyc_Tcpat_PathNode*)((struct Cyc_List_List*)_check_null(ps->tl))->hd)->access;void*_tmp6E0=_tmp6DF;struct Cyc_Absyn_Datatypedecl*_tmp6E1;struct Cyc_Absyn_Datatypefield*_tmp6E2;unsigned int _tmp6E3;if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6E0)->tag == 3){_LL3E4: _tmp6E1=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6E0)->f1;_tmp6E2=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6E0)->f2;_tmp6E3=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6E0)->f3;_LL3E5:
# 3964
 ps=ps->tl;
v=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp6E2->name,_tmp6E1->name)),Cyc_Toc_mt_tq),v);
v=Cyc_Absyn_aggrarrow_exp(v,Cyc_Absyn_fieldname((int)(_tmp6E3 + 1)),0);
continue;}else{_LL3E6: _LL3E7:
# 3969
 goto _LL3E3;}_LL3E3:;}
# 3972
v=Cyc_Absyn_deref_exp(v,0);
goto _LL3D8;case 2: _LL3DD: _tmp6DE=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp6D8)->f1;_LL3DE:
 v=Cyc_Toc_member_exp(v,Cyc_Absyn_fieldname((int)(_tmp6DE + 1)),0);goto _LL3D8;case 4: _LL3DF: _tmp6DC=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp6D8)->f1;_tmp6DD=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp6D8)->f2;_LL3E0:
# 3976
 v=Cyc_Toc_member_exp(v,_tmp6DD,0);
if(_tmp6DC)
v=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0);
goto _LL3D8;default: _LL3E1: _tmp6D9=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6D8)->f1;_tmp6DA=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6D8)->f2;_tmp6DB=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp6D8)->f3;_LL3E2:
# 3981
 v=Cyc_Toc_member_exp(v,Cyc_Absyn_fieldname((int)(_tmp6DB + 1)),0);
goto _LL3D8;}_LL3D8:;};}
# 3985
return v;}
# 3990
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
void*_tmp6E4=t;struct Cyc_Absyn_Datatypedecl*_tmp6E5;struct Cyc_Absyn_Datatypefield*_tmp6E6;struct _dyneither_ptr*_tmp6E7;int _tmp6E8;int _tmp6E9;struct Cyc_Absyn_Datatypedecl*_tmp6EA;struct Cyc_Absyn_Datatypefield*_tmp6EB;unsigned int _tmp6EC;struct _dyneither_ptr _tmp6ED;int _tmp6EE;void*_tmp6EF;struct Cyc_Absyn_Enumfield*_tmp6F0;struct Cyc_Absyn_Enumdecl*_tmp6F1;struct Cyc_Absyn_Enumfield*_tmp6F2;struct Cyc_Absyn_Exp*_tmp6F3;switch(*((int*)_tmp6E4)){case 0: _LL3E9: _tmp6F3=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp6E4)->f1;_LL3EA:
# 3994
 if(_tmp6F3 == 0)return v;else{return _tmp6F3;}case 1: _LL3EB: _LL3EC:
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_signed_int_exp(0,0),0);case 2: _LL3ED: _LL3EE:
 return Cyc_Absyn_neq_exp(v,Cyc_Absyn_signed_int_exp(0,0),0);case 3: _LL3EF: _tmp6F1=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp6E4)->f1;_tmp6F2=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp6E4)->f2;_LL3F0:
# 3998
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp6F4=_cycalloc(sizeof(*_tmp6F4));_tmp6F4[0]=({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmp6F5;_tmp6F5.tag=31;_tmp6F5.f1=_tmp6F2->name;_tmp6F5.f2=_tmp6F1;_tmp6F5.f3=_tmp6F2;_tmp6F5;});_tmp6F4;}),0),0);case 4: _LL3F1: _tmp6EF=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp6E4)->f1;_tmp6F0=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp6E4)->f2;_LL3F2:
# 4000
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp6F6=_cycalloc(sizeof(*_tmp6F6));_tmp6F6[0]=({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp6F7;_tmp6F7.tag=32;_tmp6F7.f1=_tmp6F0->name;_tmp6F7.f2=_tmp6EF;_tmp6F7.f3=_tmp6F0;_tmp6F7;});_tmp6F6;}),0),0);case 5: _LL3F3: _tmp6ED=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp6E4)->f1;_tmp6EE=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp6E4)->f2;_LL3F4:
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_float_exp(_tmp6ED,_tmp6EE,0),0);case 6: _LL3F5: _tmp6EC=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp6E4)->f1;_LL3F6:
 return Cyc_Absyn_eq_exp(v,Cyc_Absyn_uint_exp(_tmp6EC,0),0);case 7: _LL3F7: _tmp6E9=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp6E4)->f1;_tmp6EA=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp6E4)->f2;_tmp6EB=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp6E4)->f3;_LL3F8:
# 4006
 LOOP1: {
void*_tmp6F8=v->r;void*_tmp6F9=_tmp6F8;struct Cyc_Absyn_Exp*_tmp6FA;struct Cyc_Absyn_Exp*_tmp6FB;switch(*((int*)_tmp6F9)){case 13: _LL3FE: _tmp6FB=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6F9)->f2;_LL3FF:
 v=_tmp6FB;goto LOOP1;case 19: _LL400: _tmp6FA=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6F9)->f1;_LL401:
 v=_tmp6FA;goto _LL3FD;default: _LL402: _LL403:
 goto _LL3FD;}_LL3FD:;}
# 4013
v=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp6EB->name,_tmp6EA->name)),Cyc_Toc_mt_tq),v);
return Cyc_Absyn_eq_exp(Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0),Cyc_Absyn_uint_exp((unsigned int)_tmp6E9,0),0);case 8: _LL3F9: _tmp6E7=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp6E4)->f1;_tmp6E8=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp6E4)->f2;_LL3FA:
# 4016
 v=Cyc_Toc_member_exp(Cyc_Toc_member_exp(v,_tmp6E7,0),Cyc_Toc_tag_sp,0);
return Cyc_Absyn_eq_exp(v,Cyc_Absyn_uint_exp((unsigned int)_tmp6E8,0),0);default: _LL3FB: _tmp6E5=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp6E4)->f1;_tmp6E6=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp6E4)->f2;_LL3FC:
# 4020
 LOOP2: {
void*_tmp6FC=v->r;void*_tmp6FD=_tmp6FC;struct Cyc_Absyn_Exp*_tmp6FE;struct Cyc_Absyn_Exp*_tmp6FF;switch(*((int*)_tmp6FD)){case 13: _LL405: _tmp6FF=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6FD)->f2;_LL406:
 v=_tmp6FF;goto LOOP2;case 19: _LL407: _tmp6FE=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6FD)->f1;_LL408:
 v=_tmp6FE;goto _LL404;default: _LL409: _LL40A:
 goto _LL404;}_LL404:;}
# 4027
v=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp6E6->name,_tmp6E5->name)),Cyc_Toc_mt_tq),v);
return Cyc_Absyn_eq_exp(Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0),Cyc_Absyn_var_exp(_tmp6E6->name,0),0);}_LL3E8:;}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _dyneither_ptr*f1;};
# 4039
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1};struct _tuple32{int f1;void*f2;};
# 4047
static struct _tuple32 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 4049
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(0;ss != 0;(ss=ss->tl,c=c + 1)){
struct _tuple29 _tmp702=*((struct _tuple29*)ss->hd);struct _tuple29 _tmp703=_tmp702;void*_tmp704;_LL40C: _tmp704=_tmp703.f1;_LL40D:;{
void*_tmp705=_tmp704;struct Cyc_Absyn_Exp*_tmp706;struct _dyneither_ptr*_tmp707;switch(*((int*)_tmp705)){case 3: _LL40F: _LL410:
# 4055
 goto _LL412;case 4: _LL411: _LL412:
 goto _LL414;case 6: _LL413: _LL414:
# 4058
 continue;case 8: _LL415: _tmp707=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp705)->f1;_LL416:
# 4060
 if(k == (void*)& Cyc_Toc_OtherTest_val)
k=(void*)({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_tmp708=_cycalloc(sizeof(*_tmp708));_tmp708[0]=({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct _tmp709;_tmp709.tag=3;_tmp709.f1=_tmp707;_tmp709;});_tmp708;});
continue;case 7: _LL417: _LL418:
# 4064
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;
continue;case 0: _LL419: _tmp706=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp705)->f1;if(_tmp706 != 0){_LL41A:
# 4068
 k=(void*)({struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_tmp70A=_cycalloc(sizeof(*_tmp70A));_tmp70A[0]=({struct Cyc_Toc_WhereTest_Toc_TestKind_struct _tmp70B;_tmp70B.tag=2;_tmp70B.f1=_tmp706;_tmp70B;});_tmp70A;});
return({struct _tuple32 _tmp70C;_tmp70C.f1=0;_tmp70C.f2=k;_tmp70C;});}else{_LL41B: _LL41C:
 goto _LL41E;}case 1: _LL41D: _LL41E:
 goto _LL420;case 2: _LL41F: _LL420:
 goto _LL422;case 5: _LL421: _LL422:
 goto _LL424;default: _LL423: _LL424:
 return({struct _tuple32 _tmp70D;_tmp70D.f1=0;_tmp70D.f2=k;_tmp70D;});}_LL40E:;};}
# 4077
return({struct _tuple32 _tmp70E;_tmp70E.f1=c;_tmp70E.f2=k;_tmp70E;});}
# 4082
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{void*_tmp70F=p;int _tmp710;int _tmp711;unsigned int _tmp712;void*_tmp713;struct Cyc_Absyn_Enumfield*_tmp714;struct Cyc_Absyn_Enumdecl*_tmp715;struct Cyc_Absyn_Enumfield*_tmp716;switch(*((int*)_tmp70F)){case 3: _LL426: _tmp715=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp70F)->f1;_tmp716=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp70F)->f2;_LL427:
# 4086
 e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp717=_cycalloc(sizeof(*_tmp717));_tmp717[0]=({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmp718;_tmp718.tag=31;_tmp718.f1=_tmp716->name;_tmp718.f2=_tmp715;_tmp718.f3=_tmp716;_tmp718;});_tmp717;}),0);goto _LL425;case 4: _LL428: _tmp713=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp70F)->f1;_tmp714=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp70F)->f2;_LL429:
# 4088
 e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp719=_cycalloc(sizeof(*_tmp719));_tmp719[0]=({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp71A;_tmp71A.tag=32;_tmp71A.f1=_tmp714->name;_tmp71A.f2=_tmp713;_tmp71A.f3=_tmp714;_tmp71A;});_tmp719;}),0);goto _LL425;case 6: _LL42A: _tmp712=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp70F)->f1;_LL42B:
 _tmp711=(int)_tmp712;goto _LL42D;case 7: _LL42C: _tmp711=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp70F)->f1;_LL42D:
 _tmp710=_tmp711;goto _LL42F;case 8: _LL42E: _tmp710=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp70F)->f2;_LL42F:
# 4092
 e=Cyc_Absyn_uint_exp((unsigned int)_tmp710,0);
goto _LL425;default: _LL430: _LL431:
({void*_tmp71B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp71C="compile_pat_test_as_case!";_tag_dyneither(_tmp71C,sizeof(char),26);}),_tag_dyneither(_tmp71B,sizeof(void*),0));});}_LL425:;}
# 4096
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp71D=_cycalloc(sizeof(*_tmp71D));_tmp71D[0]=({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct _tmp71E;_tmp71E.tag=17;_tmp71E.f1=e;_tmp71E;});_tmp71D;}),0);}
# 4100
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0);}struct _tuple33{struct _tuple1*f1;void*f2;};struct _tuple34{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 4109
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**decls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 4112
void*t=(void*)_check_null(p->topt);
void*_tmp71F=p->r;void*_tmp720=_tmp71F;union Cyc_Absyn_AggrInfoU _tmp721;struct Cyc_List_List*_tmp722;struct Cyc_List_List*_tmp723;struct Cyc_List_List*_tmp724;struct Cyc_Absyn_Pat*_tmp725;struct Cyc_Absyn_Datatypedecl*_tmp726;struct Cyc_Absyn_Datatypefield*_tmp727;struct Cyc_List_List*_tmp728;struct Cyc_Absyn_Vardecl*_tmp729;struct Cyc_Absyn_Pat*_tmp72A;struct _tuple1*_tmp72B;struct _tuple1*_tmp72C;struct Cyc_Absyn_Pat*_tmp72D;struct _tuple1*_tmp72E;switch(*((int*)_tmp720)){case 2: _LL433: _tmp72E=(((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp720)->f2)->name;_LL434: {
# 4115
struct Cyc_Absyn_Pat*_tmp72F=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0);
_tmp72F->topt=p->topt;
_tmp72C=_tmp72E;_tmp72D=_tmp72F;goto _LL436;}case 1: _LL435: _tmp72C=(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp720)->f1)->name;_tmp72D=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp720)->f2;_LL436: {
# 4120
struct _tuple1*v=Cyc_Toc_temp_var();
void*_tmp730=(void*)_check_null(_tmp72D->topt);
*decls=({struct Cyc_List_List*_tmp731=_region_malloc(rgn,sizeof(*_tmp731));_tmp731->hd=({struct _tuple33*_tmp732=_region_malloc(rgn,sizeof(*_tmp732));_tmp732->f1=v;_tmp732->f2=Cyc_Toc_typ_to_c_array(_tmp730);_tmp732;});_tmp731->tl=*decls;_tmp731;});
*nv=Cyc_Toc_add_varmap(rgn,*nv,_tmp72C,Cyc_Absyn_var_exp(v,0));{
struct Cyc_Absyn_Stmt*_tmp733=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(v,0),Cyc_Absyn_copy_exp(path),0);
return Cyc_Toc_seq_stmt_opt(_tmp733,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp72D));};}case 4: _LL437: _tmp72B=(((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp720)->f2)->name;_LL438: {
# 4128
struct _tuple1*_tmp734=Cyc_Toc_temp_var();
void*_tmp735=(void*)_check_null(p->topt);
*decls=({struct Cyc_List_List*_tmp736=_region_malloc(rgn,sizeof(*_tmp736));_tmp736->hd=({struct _tuple33*_tmp737=_region_malloc(rgn,sizeof(*_tmp737));_tmp737->f1=_tmp734;_tmp737->f2=Cyc_Toc_typ_to_c_array(_tmp735);_tmp737;});_tmp736->tl=*decls;_tmp736;});
*nv=Cyc_Toc_add_varmap(rgn,*nv,_tmp72B,Cyc_Absyn_var_exp(_tmp734,0));
return Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp734,0),Cyc_Absyn_copy_exp(path),0);}case 0: _LL439: _LL43A:
 return 0;case 3: _LL43B: _tmp729=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp720)->f1;_tmp72A=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp720)->f2;_LL43C: {
# 4136
struct _tuple1*_tmp738=Cyc_Toc_temp_var();
*decls=({struct Cyc_List_List*_tmp739=_region_malloc(rgn,sizeof(*_tmp739));_tmp739->hd=({struct _tuple33*_tmp73A=_region_malloc(rgn,sizeof(*_tmp73A));_tmp73A->f1=_tmp738;_tmp73A->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq);_tmp73A;});_tmp739->tl=*decls;_tmp739;});
*nv=Cyc_Toc_add_varmap(rgn,*nv,_tmp729->name,Cyc_Absyn_var_exp(_tmp738,0));{
# 4140
struct Cyc_Absyn_Stmt*_tmp73B=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp738,0),
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path))),0);
return Cyc_Toc_seq_stmt_opt(_tmp73B,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp72A));};}case 9: _LL43D: _LL43E:
# 4145
 goto _LL440;case 10: _LL43F: _LL440:
 goto _LL442;case 11: _LL441: _LL442:
 goto _LL444;case 12: _LL443: _LL444:
 goto _LL446;case 13: _LL445: _LL446:
 goto _LL448;case 14: _LL447: _LL448:
 return 0;case 6: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp720)->f1)->r)->tag == 8){_LL449: _tmp726=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp720)->f1)->r)->f1;_tmp727=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp720)->f1)->r)->f2;_tmp728=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp720)->f1)->r)->f3;_LL44A:
# 4154
 if(_tmp728 == 0)return 0;{
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(_tmp727->name,_tmp726->name);
void*_tmp73C=Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);
path=Cyc_Toc_cast_it(_tmp73C,path);{
int cnt=1;
struct Cyc_List_List*_tmp73D=_tmp727->typs;
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp728 != 0;(((_tmp728=_tmp728->tl,_tmp73D=((struct Cyc_List_List*)_check_null(_tmp73D))->tl)),++ cnt)){
struct Cyc_Absyn_Pat*_tmp73E=(struct Cyc_Absyn_Pat*)_tmp728->hd;
if(_tmp73E->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
void*_tmp73F=(*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp73D))->hd)).f2;
void*_tmp740=(void*)_check_null(_tmp73E->topt);
void*_tmp741=Cyc_Toc_typ_to_c_array(_tmp740);
struct Cyc_Absyn_Exp*_tmp742=Cyc_Absyn_aggrarrow_exp(path,Cyc_Absyn_fieldname(cnt),0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp73F)))
_tmp742=Cyc_Toc_cast_it(_tmp741,_tmp742);
s=Cyc_Toc_seq_stmt_opt(s,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp742,_tmp73E));};}
# 4172
return s;};};}else{_LL453: _tmp725=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp720)->f1;_LL454:
# 4221
 return Cyc_Toc_extract_pattern_vars(rgn,nv,decls,Cyc_Absyn_deref_exp(path,0),_tmp725);}case 8: _LL44B: _tmp724=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp720)->f3;_LL44C:
# 4174
 _tmp723=_tmp724;goto _LL44E;case 5: _LL44D: _tmp723=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp720)->f1;_LL44E: {
# 4176
struct Cyc_Absyn_Stmt*s=0;
int cnt=1;
for(0;_tmp723 != 0;(_tmp723=_tmp723->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp743=(struct Cyc_Absyn_Pat*)_tmp723->hd;
if(_tmp743->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp744=(void*)_check_null(_tmp743->topt);
struct _dyneither_ptr*_tmp745=Cyc_Absyn_fieldname(cnt);
s=Cyc_Toc_seq_stmt_opt(s,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,Cyc_Toc_member_exp(path,_tmp745,0),_tmp743));};}
# 4186
return s;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp720)->f1 == 0){_LL44F: _LL450:
({void*_tmp746=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp747="unresolved aggregate pattern!";_tag_dyneither(_tmp747,sizeof(char),30);}),_tag_dyneither(_tmp746,sizeof(void*),0));});}else{_LL451: _tmp721=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp720)->f1)->aggr_info;_tmp722=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp720)->f3;_LL452: {
# 4190
struct Cyc_Absyn_Aggrdecl*_tmp748=Cyc_Absyn_get_known_aggrdecl(_tmp721);
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp722 != 0;_tmp722=_tmp722->tl){
struct _tuple34*_tmp749=(struct _tuple34*)_tmp722->hd;
struct Cyc_Absyn_Pat*_tmp74A=(*_tmp749).f2;
if(_tmp74A->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*f;
{void*_tmp74B=(void*)((struct Cyc_List_List*)_check_null((*_tmp749).f1))->hd;void*_tmp74C=_tmp74B;struct _dyneither_ptr*_tmp74D;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp74C)->tag == 1){_LL45C: _tmp74D=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp74C)->f1;_LL45D:
 f=_tmp74D;goto _LL45B;}else{_LL45E: _LL45F:
(int)_throw((void*)({struct Cyc_Toc_Match_error_exn_struct*_tmp74E=_cycalloc_atomic(sizeof(*_tmp74E));_tmp74E[0]=({struct Cyc_Toc_Match_error_exn_struct _tmp74F;_tmp74F.tag=Cyc_Toc_Match_error;_tmp74F;});_tmp74E;}));}_LL45B:;}{
# 4202
void*_tmp750=(void*)_check_null(_tmp74A->topt);
void*_tmp751=Cyc_Toc_typ_to_c_array(_tmp750);
struct Cyc_Absyn_Exp*_tmp752=Cyc_Toc_member_exp(path,f,0);
# 4206
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp748->impl))->tagged)_tmp752=Cyc_Toc_member_exp(_tmp752,Cyc_Toc_val_sp,0);{
void*_tmp753=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp748->impl))->fields,f)))->type;
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp753))
_tmp752=Cyc_Toc_cast_it(_tmp751,_tmp752);else{
if(!Cyc_Toc_is_array_type(_tmp753) && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp753)))
# 4213
_tmp752=Cyc_Absyn_deref_exp(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(_tmp751,Cyc_Toc_mt_tq),
Cyc_Absyn_address_exp(_tmp752,0)),0);}
# 4216
s=Cyc_Toc_seq_stmt_opt(s,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp752,_tmp74A));};};};}
# 4218
return s;}}case 15: _LL455: _LL456:
# 4223
({void*_tmp754=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp755="unknownid pat";_tag_dyneither(_tmp755,sizeof(char),14);}),_tag_dyneither(_tmp754,sizeof(void*),0));});case 16: _LL457: _LL458:
({void*_tmp756=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp757="unknowncall pat";_tag_dyneither(_tmp757,sizeof(char),16);}),_tag_dyneither(_tmp756,sizeof(void*),0));});default: _LL459: _LL45A:
({void*_tmp758=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp759="exp pat";_tag_dyneither(_tmp759,sizeof(char),8);}),_tag_dyneither(_tmp758,sizeof(void*),0));});}_LL432:;}struct _tuple35{struct Cyc_Toc_Env*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Stmt*f3;};
# 4234
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 4242
void*_tmp75A=dopt;struct Cyc_List_List*_tmp75B;struct Cyc_List_List*_tmp75C;void*_tmp75D;struct Cyc_Tcpat_Rhs*_tmp75E;if(_tmp75A == 0){_LL461: _LL462:
# 4244
 return Cyc_Absyn_skip_stmt(0);}else{switch(*((int*)_tmp75A)){case 0: _LL463: _LL464:
 return Cyc_Toc_throw_match_stmt();case 1: _LL465: _tmp75E=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp75A)->f1;_LL466:
# 4249
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple31*_tmp75F=(struct _tuple31*)lscs->hd;struct _tuple31*_tmp760=_tmp75F;int*_tmp761;struct _dyneither_ptr*_tmp762;struct _dyneither_ptr*_tmp763;struct Cyc_Absyn_Switch_clause*_tmp764;_LL46A: _tmp761=(int*)& _tmp760->f1;_tmp762=_tmp760->f2;_tmp763=_tmp760->f3;_tmp764=_tmp760->f4;_LL46B:;{
struct Cyc_Absyn_Stmt*_tmp765=_tmp764->body;
if(_tmp765 == _tmp75E->rhs){
# 4254
if(*_tmp761)
return Cyc_Absyn_goto_stmt(_tmp762,0);
*_tmp761=1;{
# 4258
struct Cyc_Absyn_Stmt*init_opt=Cyc_Toc_extract_pattern_vars(rgn,& nv,decls,Cyc_Absyn_var_exp(v,0),_tmp764->pattern);
# 4261
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_label_stmt(_tmp763,_tmp764->body,0);
if(init_opt != 0)
res=Cyc_Absyn_seq_stmt(init_opt,res,0);
res=Cyc_Absyn_label_stmt(_tmp762,res,0);
*bodies=({struct Cyc_List_List*_tmp766=_region_malloc(rgn,sizeof(*_tmp766));_tmp766->hd=({struct _tuple35*_tmp767=_region_malloc(rgn,sizeof(*_tmp767));_tmp767->f1=nv;_tmp767->f2=_tmp763;_tmp767->f3=_tmp765;_tmp767;});_tmp766->tl=*bodies;_tmp766;});
return res;};}};}
# 4269
({void*_tmp768=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp769="couldn't find rhs!";_tag_dyneither(_tmp769,sizeof(char),19);}),_tag_dyneither(_tmp768,sizeof(void*),0));});default: _LL467: _tmp75B=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp75A)->f1;_tmp75C=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp75A)->f2;_tmp75D=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp75A)->f3;_LL468: {
# 4272
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp75D,lscs,v);
# 4274
struct Cyc_Absyn_Exp*_tmp76A=Cyc_Toc_compile_path(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp75B),Cyc_Absyn_var_exp(v,0));
struct Cyc_List_List*_tmp76B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp75C);
# 4277
struct _tuple32 _tmp76C=Cyc_Toc_admits_switch(_tmp76B);struct _tuple32 _tmp76D=_tmp76C;int _tmp76E;void*_tmp76F;_LL46D: _tmp76E=_tmp76D.f1;_tmp76F=_tmp76D.f2;_LL46E:;
if(_tmp76E > 1){
# 4281
struct Cyc_List_List*new_lscs=({struct Cyc_List_List*_tmp780=_cycalloc(sizeof(*_tmp780));_tmp780->hd=({struct Cyc_Absyn_Switch_clause*_tmp781=_cycalloc(sizeof(*_tmp781));_tmp781->pattern=
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0);_tmp781->pat_vars=0;_tmp781->where_clause=0;_tmp781->body=res;_tmp781->loc=0;_tmp781;});_tmp780->tl=0;_tmp780;});
# 4284
for(0;_tmp76B != 0;_tmp76B=_tmp76B->tl){
struct _tuple29 _tmp770=*((struct _tuple29*)_tmp76B->hd);struct _tuple29 _tmp771=_tmp770;void*_tmp772;void*_tmp773;_LL470: _tmp772=_tmp771.f1;_tmp773=_tmp771.f2;_LL471:;{
# 4287
struct Cyc_Absyn_Pat*_tmp774=Cyc_Toc_compile_pat_test_as_case(_tmp772);
# 4289
struct Cyc_Absyn_Stmt*_tmp775=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp773,lscs,v);
# 4291
new_lscs=({struct Cyc_List_List*_tmp776=_cycalloc(sizeof(*_tmp776));_tmp776->hd=({struct Cyc_Absyn_Switch_clause*_tmp777=_cycalloc(sizeof(*_tmp777));_tmp777->pattern=_tmp774;_tmp777->pat_vars=0;_tmp777->where_clause=0;_tmp777->body=_tmp775;_tmp777->loc=0;_tmp777;});_tmp776->tl=new_lscs;_tmp776;});};}
# 4293
{void*_tmp778=_tmp76F;struct _dyneither_ptr*_tmp779;switch(*((int*)_tmp778)){case 1: _LL473: _LL474:
# 4296
 LOOP1: {
void*_tmp77A=_tmp76A->r;void*_tmp77B=_tmp77A;struct Cyc_Absyn_Exp*_tmp77C;struct Cyc_Absyn_Exp*_tmp77D;switch(*((int*)_tmp77B)){case 13: _LL47C: _tmp77D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp77B)->f2;_LL47D:
 _tmp76A=_tmp77D;goto LOOP1;case 19: _LL47E: _tmp77C=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp77B)->f1;_LL47F:
 _tmp76A=_tmp77C;goto _LL47B;default: _LL480: _LL481:
 goto _LL47B;}_LL47B:;}
# 4302
_tmp76A=Cyc_Absyn_deref_exp(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,Cyc_Toc_mt_tq),_tmp76A),0);goto _LL472;case 3: _LL475: _tmp779=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_tmp778)->f1;_LL476:
# 4305
 _tmp76A=Cyc_Toc_member_exp(Cyc_Toc_member_exp(_tmp76A,_tmp779,0),Cyc_Toc_tag_sp,0);goto _LL472;case 2: _LL477: _LL478:
# 4307
 goto _LL472;default: _LL479: _LL47A:
 goto _LL472;}_LL472:;}
# 4310
res=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp77E=_cycalloc(sizeof(*_tmp77E));_tmp77E[0]=({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct _tmp77F;_tmp77F.tag=10;_tmp77F.f1=_tmp76A;_tmp77F.f2=new_lscs;_tmp77F.f3=0;_tmp77F;});_tmp77E;}),0);}else{
# 4314
void*_tmp782=_tmp76F;struct Cyc_Absyn_Exp*_tmp783;if(((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp782)->tag == 2){_LL483: _tmp783=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp782)->f1;_LL484: {
# 4316
struct Cyc_List_List*_tmp784=_tmp76B;void*_tmp785;struct Cyc_Tcpat_Rhs*_tmp786;if(_tmp784 != 0){if(((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple29*)((struct Cyc_List_List*)_tmp784)->hd)->f2)->tag == 1){if(((struct Cyc_List_List*)_tmp784)->tl == 0){_LL488: _tmp785=((struct _tuple29*)_tmp784->hd)->f1;_tmp786=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple29*)_tmp784->hd)->f2)->f1;_LL489:
# 4322
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple31*_tmp787=(struct _tuple31*)lscs->hd;struct _tuple31*_tmp788=_tmp787;int*_tmp789;struct _dyneither_ptr*_tmp78A;struct _dyneither_ptr*_tmp78B;struct Cyc_Absyn_Switch_clause*_tmp78C;_LL48D: _tmp789=(int*)& _tmp788->f1;_tmp78A=_tmp788->f2;_tmp78B=_tmp788->f3;_tmp78C=_tmp788->f4;_LL48E:;{
struct Cyc_Absyn_Stmt*_tmp78D=_tmp78C->body;
if(_tmp78D == _tmp786->rhs){
# 4327
if(*_tmp789)
return Cyc_Absyn_goto_stmt(_tmp78A,0);
*_tmp789=1;{
# 4331
struct Cyc_Absyn_Stmt*init_opt=Cyc_Toc_extract_pattern_vars(rgn,& nv,decls,Cyc_Absyn_var_exp(v,0),_tmp78C->pattern);
# 4334
Cyc_Toc_exp_to_c(nv,_tmp783);{
# 4337
struct Cyc_Absyn_Stmt*r=Cyc_Absyn_label_stmt(_tmp78B,_tmp78C->body,0);
r=Cyc_Absyn_ifthenelse_stmt(_tmp783,r,res,0);
if(init_opt != 0)
r=Cyc_Absyn_seq_stmt(init_opt,r,0);
r=Cyc_Absyn_label_stmt(_tmp78A,r,0);
*bodies=({struct Cyc_List_List*_tmp78E=_region_malloc(rgn,sizeof(*_tmp78E));_tmp78E->hd=({struct _tuple35*_tmp78F=_region_malloc(rgn,sizeof(*_tmp78F));_tmp78F->f1=nv;_tmp78F->f2=_tmp78B;_tmp78F->f3=_tmp78D;_tmp78F;});_tmp78E->tl=*bodies;_tmp78E;});
return r;};};}};}
# 4346
({void*_tmp790=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp791="couldn't find rhs!";_tag_dyneither(_tmp791,sizeof(char),19);}),_tag_dyneither(_tmp790,sizeof(void*),0));});}else{goto _LL48A;}}else{goto _LL48A;}}else{_LL48A: _LL48B:
({void*_tmp792=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp793="bad where clause in match compiler";_tag_dyneither(_tmp793,sizeof(char),35);}),_tag_dyneither(_tmp792,sizeof(void*),0));});}_LL487:;}}else{_LL485: _LL486:
# 4351
 for(0;_tmp76B != 0;_tmp76B=_tmp76B->tl){
struct _tuple29 _tmp794=*((struct _tuple29*)_tmp76B->hd);struct _tuple29 _tmp795=_tmp794;void*_tmp796;void*_tmp797;_LL490: _tmp796=_tmp795.f1;_tmp797=_tmp795.f2;_LL491:;{
struct Cyc_Absyn_Exp*_tmp798=Cyc_Toc_compile_pat_test(_tmp76A,_tmp796);
struct Cyc_Absyn_Stmt*_tmp799=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp797,lscs,v);
res=Cyc_Absyn_ifthenelse_stmt(_tmp798,_tmp799,res,0);};}}_LL482:;}
# 4359
return res;}}}_LL460:;}
# 4369
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 4371
for(0;bodies != 0;bodies=bodies->tl){
struct _tuple35*_tmp79A=(struct _tuple35*)bodies->hd;struct _tuple35*_tmp79B=_tmp79A;struct Cyc_Toc_Env**_tmp79C;struct Cyc_Absyn_Stmt*_tmp79D;_LL493: _tmp79C=(struct Cyc_Toc_Env**)& _tmp79B->f1;_tmp79D=_tmp79B->f3;_LL494:;
if(_tmp79D == s)return _tmp79C;}
# 4377
return 0;}
# 4381
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 4384
void*_tmp79E=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(nv,e);{
# 4387
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp79F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp79F;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp7A0=Cyc_Toc_share_env(rgn,nv);
# 4394
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple31*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 4399
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp7A0,& mydecls,& mybodies,dopt,lscs,v);
# 4407
{struct Cyc_List_List*_tmp7A1=lscs;for(0;_tmp7A1 != 0;_tmp7A1=_tmp7A1->tl){
struct _tuple31*_tmp7A2=(struct _tuple31*)_tmp7A1->hd;struct _tuple31*_tmp7A3=_tmp7A2;struct _dyneither_ptr*_tmp7A4;struct Cyc_Absyn_Switch_clause*_tmp7A5;_LL496: _tmp7A4=_tmp7A3->f3;_tmp7A5=_tmp7A3->f4;_LL497:;{
struct Cyc_Absyn_Stmt*s=_tmp7A5->body;
# 4411
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 4414
if(envp == 0)continue;{
struct Cyc_Toc_Env*_tmp7A6=*envp;
# 4417
if(_tmp7A1->tl != 0){
struct _tuple31*_tmp7A7=(struct _tuple31*)((struct Cyc_List_List*)_check_null(_tmp7A1->tl))->hd;struct _tuple31*_tmp7A8=_tmp7A7;struct _dyneither_ptr*_tmp7A9;struct Cyc_Absyn_Switch_clause*_tmp7AA;_LL499: _tmp7A9=_tmp7A8->f3;_tmp7AA=_tmp7A8->f4;_LL49A:;{
struct Cyc_Toc_Env**_tmp7AB=Cyc_Toc_find_case_env(mybodies,_tmp7AA->body);
# 4424
if(_tmp7AB == 0)
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn,_tmp7A6,end_l),s);else{
# 4428
struct Cyc_List_List*vs=0;
if(_tmp7AA->pat_vars != 0){
vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp7AA->pat_vars))->v)).f1);
vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vs);}
# 4433
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(rgn,_tmp7A6,end_l,_tmp7A9,vs,*_tmp7AB),s);}};}else{
# 4437
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn,_tmp7A6,end_l),s);}};};}}{
# 4441
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(test_tree,Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),0),0);
# 4443
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct _tuple33*_tmp7AC=(struct _tuple33*)((struct Cyc_List_List*)_check_null(mydecls))->hd;struct _tuple33*_tmp7AD=_tmp7AC;struct _tuple1*_tmp7AE;void*_tmp7AF;_LL49C: _tmp7AE=_tmp7AD->f1;_tmp7AF=_tmp7AD->f2;_LL49D:;
res=Cyc_Absyn_declare_stmt(_tmp7AE,_tmp7AF,0,res,0);}
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
return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,({struct Cyc_List_List*_tmp7B0=_cycalloc(sizeof(*_tmp7B0));_tmp7B0->hd=
Cyc_Absyn_uint_exp((unsigned int)(n - 1),0);_tmp7B0->tl=0;_tmp7B0;}),0),0);}
# 4467
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt(s->r,0));}
# 4472
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 4474
while(1){
void*_tmp7B1=s->r;void*_tmp7B2=_tmp7B1;struct Cyc_Absyn_Exp*_tmp7B3;struct Cyc_Absyn_Stmt*_tmp7B4;struct Cyc_List_List*_tmp7B5;void*_tmp7B6;struct Cyc_Absyn_Stmt*_tmp7B7;struct Cyc_Absyn_Exp*_tmp7B8;struct _dyneither_ptr*_tmp7B9;struct Cyc_Absyn_Stmt*_tmp7BA;struct Cyc_Absyn_Decl*_tmp7BB;struct Cyc_Absyn_Stmt*_tmp7BC;struct Cyc_List_List*_tmp7BD;struct Cyc_Absyn_Switch_clause**_tmp7BE;struct Cyc_Absyn_Exp*_tmp7BF;struct Cyc_List_List*_tmp7C0;void*_tmp7C1;struct Cyc_Absyn_Exp*_tmp7C2;struct Cyc_Absyn_Exp*_tmp7C3;struct Cyc_Absyn_Exp*_tmp7C4;struct Cyc_Absyn_Stmt*_tmp7C5;struct Cyc_Absyn_Exp*_tmp7C6;struct Cyc_Absyn_Stmt*_tmp7C7;struct Cyc_Absyn_Exp*_tmp7C8;struct Cyc_Absyn_Stmt*_tmp7C9;struct Cyc_Absyn_Stmt*_tmp7CA;struct Cyc_Absyn_Exp*_tmp7CB;struct Cyc_Absyn_Stmt*_tmp7CC;struct Cyc_Absyn_Stmt*_tmp7CD;struct Cyc_Absyn_Exp*_tmp7CE;switch(*((int*)_tmp7B2)){case 0: _LL49F: _LL4A0:
# 4477
 return;case 1: _LL4A1: _tmp7CE=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp7B2)->f1;_LL4A2:
# 4479
 Cyc_Toc_exp_to_c(nv,_tmp7CE);
return;case 2: _LL4A3: _tmp7CC=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp7B2)->f1;_tmp7CD=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp7B2)->f2;_LL4A4:
# 4482
 Cyc_Toc_stmt_to_c(nv,_tmp7CC);
s=_tmp7CD;
continue;case 3: _LL4A5: _tmp7CB=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp7B2)->f1;_LL4A6: {
# 4486
void*topt=0;
if(_tmp7CB != 0){
topt=Cyc_Toc_typ_to_c((void*)_check_null(_tmp7CB->topt));
Cyc_Toc_exp_to_c(nv,_tmp7CB);}{
# 4492
int _tmp7CF=Cyc_Toc_get_npop(s);
if(_tmp7CF > 0){
if(topt != 0){
struct _tuple1*_tmp7D0=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp7D1=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp7D0,0),0);
s->r=(Cyc_Absyn_declare_stmt(_tmp7D0,topt,_tmp7CB,
Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(_tmp7CF),_tmp7D1,0),0))->r;}else{
# 4501
Cyc_Toc_do_npop_before(_tmp7CF,s);}}
# 4503
return;};}case 4: _LL4A7: _tmp7C8=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp7B2)->f1;_tmp7C9=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp7B2)->f2;_tmp7CA=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp7B2)->f3;_LL4A8:
# 4505
 Cyc_Toc_exp_to_c(nv,_tmp7C8);
Cyc_Toc_stmt_to_c(nv,_tmp7C9);
s=_tmp7CA;
continue;case 5: _LL4A9: _tmp7C6=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp7B2)->f1).f1;_tmp7C7=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp7B2)->f2;_LL4AA:
# 4510
 Cyc_Toc_exp_to_c(nv,_tmp7C6);{
struct _RegionHandle _tmp7D2=_new_region("temp");struct _RegionHandle*temp=& _tmp7D2;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7C7);
# 4514
_npop_handler(0);return;
# 4511
;_pop_region(temp);};case 6: _LL4AB: _LL4AC: {
# 4516
struct Cyc_Toc_Env*_tmp7D3=nv;struct _dyneither_ptr**_tmp7D4;_LL4C2: _tmp7D4=_tmp7D3->break_lab;_LL4C3:;
if(_tmp7D4 != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp7D4);
# 4520
Cyc_Toc_do_npop_before(Cyc_Toc_get_npop(s),s);
return;}case 7: _LL4AD: _LL4AE: {
# 4523
struct Cyc_Toc_Env*_tmp7D5=nv;struct _dyneither_ptr**_tmp7D6;_LL4C5: _tmp7D6=_tmp7D5->continue_lab;_LL4C6:;
if(_tmp7D6 != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp7D6);
goto _LL4B0;}case 8: _LL4AF: _LL4B0:
# 4529
 Cyc_Toc_do_npop_before(Cyc_Toc_get_npop(s),s);
return;case 9: _LL4B1: _tmp7C2=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp7B2)->f1;_tmp7C3=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp7B2)->f2).f1;_tmp7C4=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp7B2)->f3).f1;_tmp7C5=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp7B2)->f4;_LL4B2:
# 4533
 Cyc_Toc_exp_to_c(nv,_tmp7C2);Cyc_Toc_exp_to_c(nv,_tmp7C3);Cyc_Toc_exp_to_c(nv,_tmp7C4);{
struct _RegionHandle _tmp7D7=_new_region("temp");struct _RegionHandle*temp=& _tmp7D7;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7C5);
# 4537
_npop_handler(0);return;
# 4534
;_pop_region(temp);};case 10: _LL4B3: _tmp7BF=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp7B2)->f1;_tmp7C0=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp7B2)->f2;_tmp7C1=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp7B2)->f3;_LL4B4:
# 4539
 Cyc_Toc_xlate_switch(nv,s,_tmp7BF,_tmp7C0,_tmp7C1);
return;case 11: _LL4B5: _tmp7BD=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp7B2)->f1;_tmp7BE=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp7B2)->f2;_LL4B6: {
# 4542
struct Cyc_Toc_Env*_tmp7D8=nv;struct Cyc_Toc_FallthruInfo*_tmp7D9;_LL4C8: _tmp7D9=_tmp7D8->fallthru_info;_LL4C9:;
if(_tmp7D9 == 0)
({void*_tmp7DA=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp7DB="fallthru in unexpected place";_tag_dyneither(_tmp7DB,sizeof(char),29);}),_tag_dyneither(_tmp7DA,sizeof(void*),0));});{
struct Cyc_Toc_FallthruInfo _tmp7DC=*_tmp7D9;struct Cyc_Toc_FallthruInfo _tmp7DD=_tmp7DC;struct _dyneither_ptr*_tmp7DE;struct Cyc_List_List*_tmp7DF;struct Cyc_Dict_Dict _tmp7E0;_LL4CB: _tmp7DE=_tmp7DD.label;_tmp7DF=_tmp7DD.binders;_tmp7E0=_tmp7DD.next_case_env;_LL4CC:;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp7DE,0);
# 4548
Cyc_Toc_do_npop_before(Cyc_Toc_get_npop(s),s2);{
struct Cyc_List_List*_tmp7E1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7DF);
struct Cyc_List_List*_tmp7E2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7BD);
for(0;_tmp7E1 != 0;(_tmp7E1=_tmp7E1->tl,_tmp7E2=_tmp7E2->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp7E2))->hd);
s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp7E0,(struct _tuple1*)_tmp7E1->hd),(struct Cyc_Absyn_Exp*)_tmp7E2->hd,0),s2,0);}
# 4556
s->r=s2->r;
return;};};};}case 12: _LL4B7: _tmp7BB=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp7B2)->f1;_tmp7BC=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp7B2)->f2;_LL4B8:
# 4562
{void*_tmp7E3=_tmp7BB->r;void*_tmp7E4=_tmp7E3;struct Cyc_Absyn_Tvar*_tmp7E5;struct Cyc_Absyn_Vardecl*_tmp7E6;int _tmp7E7;struct Cyc_Absyn_Exp*_tmp7E8;struct Cyc_Absyn_Fndecl*_tmp7E9;struct Cyc_List_List*_tmp7EA;struct Cyc_Absyn_Pat*_tmp7EB;struct Cyc_Absyn_Exp*_tmp7EC;void*_tmp7ED;struct Cyc_Absyn_Vardecl*_tmp7EE;switch(*((int*)_tmp7E4)){case 0: _LL4CE: _tmp7EE=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp7E4)->f1;_LL4CF: {
# 4564
struct _RegionHandle _tmp7EF=_new_region("temp");struct _RegionHandle*temp=& _tmp7EF;_push_region(temp);{
struct Cyc_Toc_Env*_tmp7F0=Cyc_Toc_add_varmap(temp,nv,_tmp7EE->name,
Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp7F1=_cycalloc(sizeof(*_tmp7F1));_tmp7F1[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp7F2;_tmp7F2.tag=4;_tmp7F2.f1=_tmp7EE;_tmp7F2;});_tmp7F1;}),0));
Cyc_Toc_local_decl_to_c(_tmp7F0,_tmp7F0,_tmp7EE,_tmp7BC);}
# 4569
_npop_handler(0);goto _LL4CD;
# 4564
;_pop_region(temp);}case 2: _LL4D0: _tmp7EB=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp7E4)->f1;_tmp7EC=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp7E4)->f3;_tmp7ED=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp7E4)->f4;_LL4D1:
# 4573
{void*_tmp7F3=_tmp7EB->r;void*_tmp7F4=_tmp7F3;struct Cyc_Absyn_Vardecl*_tmp7F5;if(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp7F4)->tag == 1){if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp7F4)->f2)->r)->tag == 0){_LL4DB: _tmp7F5=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp7F4)->f1;_LL4DC: {
# 4575
struct _tuple1*old_name=_tmp7F5->name;
struct _tuple1*new_name=Cyc_Toc_temp_var();
_tmp7F5->name=new_name;
_tmp7F5->initializer=_tmp7EC;
_tmp7BB->r=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp7F6=_cycalloc(sizeof(*_tmp7F6));_tmp7F6[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp7F7;_tmp7F7.tag=0;_tmp7F7.f1=_tmp7F5;_tmp7F7;});_tmp7F6;});{
struct _RegionHandle _tmp7F8=_new_region("temp");struct _RegionHandle*temp=& _tmp7F8;_push_region(temp);{
struct Cyc_Toc_Env*_tmp7F9=
Cyc_Toc_add_varmap(temp,nv,new_name,
Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp7FA=_cycalloc(sizeof(*_tmp7FA));_tmp7FA[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp7FB;_tmp7FB.tag=4;_tmp7FB.f1=_tmp7F5;_tmp7FB;});_tmp7FA;}),0));
Cyc_Toc_local_decl_to_c(_tmp7F9,nv,_tmp7F5,_tmp7BC);}
# 4586
_npop_handler(0);goto _LL4DA;
# 4580
;_pop_region(temp);};}}else{goto _LL4DD;}}else{_LL4DD: _LL4DE:
# 4591
 s->r=(Cyc_Toc_letdecl_to_c(nv,_tmp7EB,_tmp7ED,(void*)_check_null(_tmp7EC->topt),_tmp7EC,_tmp7BC))->r;
goto _LL4DA;}_LL4DA:;}
# 4594
goto _LL4CD;case 3: _LL4D2: _tmp7EA=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp7E4)->f1;_LL4D3: {
# 4607 "toc.cyc"
struct Cyc_List_List*_tmp7FC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7EA);
if(_tmp7FC == 0)
({void*_tmp7FD=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp7FE="empty Letv_d";_tag_dyneither(_tmp7FE,sizeof(char),13);}),_tag_dyneither(_tmp7FD,sizeof(void*),0));});
_tmp7BB->r=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp7FF=_cycalloc(sizeof(*_tmp7FF));_tmp7FF[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp800;_tmp800.tag=0;_tmp800.f1=(struct Cyc_Absyn_Vardecl*)_tmp7FC->hd;_tmp800;});_tmp7FF;});
_tmp7FC=_tmp7FC->tl;
for(0;_tmp7FC != 0;_tmp7FC=_tmp7FC->tl){
struct Cyc_Absyn_Decl*_tmp801=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp802=_cycalloc(sizeof(*_tmp802));_tmp802[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp803;_tmp803.tag=0;_tmp803.f1=(struct Cyc_Absyn_Vardecl*)_tmp7FC->hd;_tmp803;});_tmp802;}),0);
s->r=(Cyc_Absyn_decl_stmt(_tmp801,Cyc_Absyn_new_stmt(s->r,0),0))->r;}
# 4616
Cyc_Toc_stmt_to_c(nv,s);
goto _LL4CD;}case 1: _LL4D4: _tmp7E9=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp7E4)->f1;_LL4D5: {
# 4619
struct _tuple1*_tmp804=_tmp7E9->name;
struct _RegionHandle _tmp805=_new_region("temp");struct _RegionHandle*temp=& _tmp805;_push_region(temp);{
struct Cyc_Toc_Env*_tmp806=Cyc_Toc_add_varmap(temp,nv,_tmp7E9->name,Cyc_Absyn_var_exp(_tmp804,0));
Cyc_Toc_fndecl_to_c(_tmp806,_tmp7E9,0);
Cyc_Toc_stmt_to_c(_tmp806,_tmp7BC);}
# 4625
_npop_handler(0);goto _LL4CD;
# 4620
;_pop_region(temp);}case 4: _LL4D6: _tmp7E5=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp7E4)->f1;_tmp7E6=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp7E4)->f2;_tmp7E7=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp7E4)->f3;_tmp7E8=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp7E4)->f4;_LL4D7: {
# 4627
struct Cyc_Absyn_Stmt*_tmp807=_tmp7BC;
# 4635
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple1*rh_var=Cyc_Toc_temp_var();
struct _tuple1*x_var=_tmp7E6->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);
# 4642
struct _RegionHandle _tmp808=_new_region("temp");struct _RegionHandle*temp=& _tmp808;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(temp,nv,x_var,x_exp),_tmp807);
# 4645
if(Cyc_Absyn_no_regions)
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_uint_exp(0,0),_tmp807,0))->r;else{
if((unsigned int)_tmp7E8){
Cyc_Toc_exp_to_c(nv,_tmp7E8);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(Cyc_Absyn_aggrarrow_exp(_tmp7E8,_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"h",sizeof(char),2),0),0);
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,_tmp807,0))->r;};}else{
# 4653
s->r=(
Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,({struct Cyc_List_List*_tmp80A=_cycalloc(sizeof(*_tmp80A));_tmp80A->hd=
Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0);_tmp80A->tl=0;_tmp80A;}),0),
# 4658
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,({struct Cyc_Absyn_Exp*_tmp80B[1];_tmp80B[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp80B,sizeof(struct Cyc_Absyn_Exp*),1));}),0),0),
Cyc_Absyn_seq_stmt(_tmp807,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,({struct Cyc_Absyn_Exp*_tmp80C[1];_tmp80C[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp80C,sizeof(struct Cyc_Absyn_Exp*),1));}),0),0),0),0),0),0))->r;}}
# 4664
_npop_handler(0);return;
# 4642
;_pop_region(temp);}default: _LL4D8: _LL4D9:
# 4665
({void*_tmp80D=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp80E="bad nested declaration within function";_tag_dyneither(_tmp80E,sizeof(char),39);}),_tag_dyneither(_tmp80D,sizeof(void*),0));});}_LL4CD:;}
# 4667
return;case 13: _LL4B9: _tmp7B9=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp7B2)->f1;_tmp7BA=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp7B2)->f2;_LL4BA:
# 4669
 s=_tmp7BA;continue;case 14: _LL4BB: _tmp7B7=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp7B2)->f1;_tmp7B8=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp7B2)->f2).f1;_LL4BC: {
# 4671
struct _RegionHandle _tmp80F=_new_region("temp");struct _RegionHandle*temp=& _tmp80F;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7B7);
Cyc_Toc_exp_to_c(nv,_tmp7B8);
# 4675
_npop_handler(0);return;
# 4671
;_pop_region(temp);}case 15: _LL4BD: _tmp7B4=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp7B2)->f1;_tmp7B5=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp7B2)->f2;_tmp7B6=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp7B2)->f3;_LL4BE: {
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
struct _RegionHandle _tmp810=_new_region("temp");struct _RegionHandle*temp=& _tmp810;_push_region(temp);{
struct Cyc_Toc_Env*_tmp811=Cyc_Toc_add_varmap(temp,nv,e_var,e_exp);
# 4704
Cyc_Toc_stmt_to_c(_tmp811,_tmp7B4);{
struct Cyc_Absyn_Stmt*_tmp812=Cyc_Absyn_seq_stmt(_tmp7B4,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
# 4709
struct Cyc_Absyn_Stmt*_tmp813=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp81A=_cycalloc(sizeof(*_tmp81A));_tmp81A[0]=({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct _tmp81B;_tmp81B.tag=10;_tmp81B.f1=e_exp;_tmp81B.f2=_tmp7B5;_tmp81B.f3=_tmp7B6;_tmp81B;});_tmp81A;}),0);
# 4711
Cyc_Toc_stmt_to_c(_tmp811,_tmp813);{
# 4714
struct Cyc_Absyn_Exp*_tmp814=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,({struct Cyc_List_List*_tmp819=_cycalloc(sizeof(*_tmp819));_tmp819->hd=
Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0);_tmp819->tl=0;_tmp819;}),0);
# 4718
struct Cyc_Absyn_Stmt*_tmp815=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,({struct Cyc_List_List*_tmp818=_cycalloc(sizeof(*_tmp818));_tmp818->hd=
Cyc_Absyn_address_exp(h_exp,0);_tmp818->tl=0;_tmp818;}),0),0);
# 4722
struct Cyc_Absyn_Exp*_tmp816=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0);
struct Cyc_Absyn_Exp*_tmp817=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0);
s->r=(
Cyc_Absyn_declare_stmt(h_var,h_typ,0,
Cyc_Absyn_seq_stmt(_tmp815,
Cyc_Absyn_declare_stmt(was_thrown_var,was_thrown_typ,_tmp816,
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp814,
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp817,0),
Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0),_tmp812,
# 4733
Cyc_Absyn_declare_stmt(e_var,e_typ,
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp813,0),0),0),0),0),0))->r;};};}
# 4737
_npop_handler(0);return;
# 4701
;_pop_region(temp);};}default: _LL4BF: _tmp7B3=((struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp7B2)->f1;_LL4C0:
# 4739
 if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{
# 4742
Cyc_Toc_exp_to_c(nv,_tmp7B3);
s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,({struct Cyc_List_List*_tmp81C=_cycalloc(sizeof(*_tmp81C));_tmp81C->hd=_tmp7B3;_tmp81C->tl=0;_tmp81C;}),0));}
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
struct _RegionHandle _tmp81D=_new_region("frgn");struct _RegionHandle*frgn=& _tmp81D;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp81E=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp81F=f->args;for(0;_tmp81F != 0;_tmp81F=_tmp81F->tl){
struct _tuple1*_tmp820=({struct _tuple1*_tmp821=_cycalloc(sizeof(*_tmp821));_tmp821->f1=Cyc_Absyn_Loc_n;_tmp821->f2=(*((struct _tuple9*)_tmp81F->hd)).f1;_tmp821;});
(*((struct _tuple9*)_tmp81F->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp81F->hd)).f3);
_tmp81E=Cyc_Toc_add_varmap(frgn,_tmp81E,_tmp820,Cyc_Absyn_var_exp(_tmp820,0));}}
# 4771
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}
# 4775
Cyc_Toc_fn_pop_table=({struct Cyc_Hashtable_Table**_tmp822=_cycalloc(sizeof(*_tmp822));_tmp822[0]=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_gpop_tables)),f);_tmp822;});
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp823=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp824=_tmp823;struct _dyneither_ptr*_tmp825;struct Cyc_Absyn_Tqual _tmp826;void*_tmp827;int _tmp828;_LL4E0: _tmp825=_tmp824.name;_tmp826=_tmp824.tq;_tmp827=_tmp824.type;_tmp828=_tmp824.inject;_LL4E1:;{
void*_tmp829=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp827,(void*)& Cyc_Absyn_HeapRgn_val,_tmp826,Cyc_Absyn_false_conref));
struct _tuple1*_tmp82A=({struct _tuple1*_tmp82D=_cycalloc(sizeof(*_tmp82D));_tmp82D->f1=Cyc_Absyn_Loc_n;_tmp82D->f2=(struct _dyneither_ptr*)_check_null(_tmp825);_tmp82D;});
f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,({struct Cyc_List_List*_tmp82B=_cycalloc(sizeof(*_tmp82B));_tmp82B->hd=({struct _tuple9*_tmp82C=_cycalloc(sizeof(*_tmp82C));_tmp82C->f1=_tmp825;_tmp82C->f2=_tmp826;_tmp82C->f3=_tmp829;_tmp82C;});_tmp82B->tl=0;_tmp82B;}));
_tmp81E=Cyc_Toc_add_varmap(frgn,_tmp81E,_tmp82A,Cyc_Absyn_var_exp(_tmp82A,0));
f->cyc_varargs=0;};}
# 4785
{struct Cyc_List_List*_tmp82E=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp82E != 0;_tmp82E=_tmp82E->tl){
((struct Cyc_Absyn_Vardecl*)_tmp82E->hd)->type=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp82E->hd)->type);}}
# 4788
Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp81E),f->body);}
# 4763
;_pop_region(frgn);};}
# 4793
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
enum Cyc_Absyn_Scope _tmp82F=s;switch(_tmp82F){case Cyc_Absyn_Abstract: _LL4E3: _LL4E4:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL4E5: _LL4E6:
 return Cyc_Absyn_Extern;default: _LL4E7: _LL4E8:
 return s;}_LL4E2:;}struct _tuple36{struct Cyc_Absyn_Aggrdecl*f1;int f2;};struct _tuple37{struct Cyc_Toc_TocState*f1;struct _tuple36*f2;};
# 4810 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple37*env){
# 4813
struct _tuple37 _tmp830=*env;struct _tuple37 _tmp831=_tmp830;struct Cyc_Toc_TocState*_tmp832;struct Cyc_Absyn_Aggrdecl*_tmp833;int _tmp834;_LL4EA: _tmp832=_tmp831.f1;_tmp833=(_tmp831.f2)->f1;_tmp834=(_tmp831.f2)->f2;_LL4EB:;{
struct _tuple1*_tmp835=_tmp833->name;
struct Cyc_Toc_TocState _tmp836=*_tmp832;struct Cyc_Toc_TocState _tmp837=_tmp836;struct Cyc_Dict_Dict*_tmp838;_LL4ED: _tmp838=_tmp837.aggrs_so_far;_LL4EE:;{
int seen_defn_before;
struct _tuple18**_tmp839=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp838,_tmp835);
if(_tmp839 == 0){
seen_defn_before=0;{
struct _tuple18*v;
if(_tmp833->kind == Cyc_Absyn_StructA)
v=({struct _tuple18*_tmp83A=_region_malloc(d,sizeof(*_tmp83A));_tmp83A->f1=_tmp833;_tmp83A->f2=Cyc_Absyn_strctq(_tmp835);_tmp83A;});else{
# 4824
v=({struct _tuple18*_tmp83B=_region_malloc(d,sizeof(*_tmp83B));_tmp83B->f1=_tmp833;_tmp83B->f2=Cyc_Absyn_unionq_typ(_tmp835);_tmp83B;});}
*_tmp838=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp838,_tmp835,v);};}else{
# 4827
struct _tuple18*_tmp83C=*_tmp839;struct _tuple18*_tmp83D=_tmp83C;struct Cyc_Absyn_Aggrdecl*_tmp83E;void*_tmp83F;_LL4F0: _tmp83E=_tmp83D->f1;_tmp83F=_tmp83D->f2;_LL4F1:;
if(_tmp83E->impl == 0){
*_tmp838=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp838,_tmp835,({struct _tuple18*_tmp840=_region_malloc(d,sizeof(*_tmp840));_tmp840->f1=_tmp833;_tmp840->f2=_tmp83F;_tmp840;}));
seen_defn_before=0;}else{
# 4832
seen_defn_before=1;}}{
# 4834
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp870=_cycalloc(sizeof(*_tmp870));_tmp870->kind=_tmp833->kind;_tmp870->sc=Cyc_Absyn_Public;_tmp870->name=_tmp833->name;_tmp870->tvs=0;_tmp870->impl=0;_tmp870->expected_mem_kind=0;_tmp870->attributes=_tmp833->attributes;_tmp870;});
# 4841
if(_tmp833->impl != 0  && !seen_defn_before){
new_ad->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp841=_cycalloc(sizeof(*_tmp841));_tmp841->exist_vars=0;_tmp841->rgn_po=0;_tmp841->fields=0;_tmp841->tagged=0;_tmp841;});{
# 4846
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp842=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp833->impl))->fields;for(0;_tmp842 != 0;_tmp842=_tmp842->tl){
# 4850
struct Cyc_Absyn_Aggrfield*_tmp843=(struct Cyc_Absyn_Aggrfield*)_tmp842->hd;
void*_tmp844=_tmp843->type;
struct Cyc_List_List*_tmp845=_tmp843->attributes;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp844)) && (
_tmp833->kind == Cyc_Absyn_StructA  && _tmp842->tl == 0  || _tmp833->kind == Cyc_Absyn_UnionA)){
# 4864 "toc.cyc"
void*_tmp846=Cyc_Tcutil_compress(_tmp844);void*_tmp847=_tmp846;void*_tmp848;struct Cyc_Absyn_Tqual _tmp849;union Cyc_Absyn_Constraint*_tmp84A;unsigned int _tmp84B;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp847)->tag == 8){_LL4F3: _tmp848=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp847)->f1).elt_type;_tmp849=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp847)->f1).tq;_tmp84A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp847)->f1).zero_term;_tmp84B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp847)->f1).zt_loc;_LL4F4:
# 4867
 _tmp844=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp84C=_cycalloc(sizeof(*_tmp84C));_tmp84C[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp84D;_tmp84D.tag=8;_tmp84D.f1=({struct Cyc_Absyn_ArrayInfo _tmp84E;_tmp84E.elt_type=_tmp848;_tmp84E.tq=_tmp849;_tmp84E.num_elts=Cyc_Absyn_uint_exp(0,0);_tmp84E.zero_term=_tmp84A;_tmp84E.zt_loc=_tmp84B;_tmp84E;});_tmp84D;});_tmp84C;});
goto _LL4F2;}else{_LL4F5: _LL4F6:
# 4870
 _tmp845=({struct Cyc_List_List*_tmp84F=_cycalloc(sizeof(*_tmp84F));_tmp84F->hd=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp850=_cycalloc(sizeof(*_tmp850));_tmp850[0]=({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmp851;_tmp851.tag=6;_tmp851.f1=0;_tmp851;});_tmp850;});_tmp84F->tl=_tmp845;_tmp84F;});
_tmp844=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp852=_cycalloc(sizeof(*_tmp852));_tmp852[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp853;_tmp853.tag=8;_tmp853.f1=({struct Cyc_Absyn_ArrayInfo _tmp854;_tmp854.elt_type=Cyc_Absyn_void_star_typ();_tmp854.tq=
Cyc_Absyn_empty_tqual(0);_tmp854.num_elts=
Cyc_Absyn_uint_exp(0,0);_tmp854.zero_term=
((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0);_tmp854.zt_loc=0;_tmp854;});_tmp853;});_tmp852;});}_LL4F2:;}{
# 4877
struct Cyc_Absyn_Aggrfield*_tmp855=({struct Cyc_Absyn_Aggrfield*_tmp86B=_cycalloc(sizeof(*_tmp86B));_tmp86B->name=_tmp843->name;_tmp86B->tq=Cyc_Toc_mt_tq;_tmp86B->type=
# 4879
Cyc_Toc_typ_to_c(_tmp844);_tmp86B->width=_tmp843->width;_tmp86B->attributes=_tmp845;_tmp86B->requires_clause=0;_tmp86B;});
# 4887
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp833->impl))->tagged){
void*_tmp856=_tmp855->type;
struct _dyneither_ptr*_tmp857=_tmp855->name;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp869;_tmp869.tag=0;_tmp869.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp857);({struct Cyc_String_pa_PrintArg_struct _tmp868;_tmp868.tag=0;_tmp868.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp833->name).f2);({void*_tmp866[2]={& _tmp868,& _tmp869};Cyc_aprintf(({const char*_tmp867="_union_%s_%s";_tag_dyneither(_tmp867,sizeof(char),13);}),_tag_dyneither(_tmp866,sizeof(void*),2));});});});
struct _dyneither_ptr*str=({struct _dyneither_ptr*_tmp865=_cycalloc(sizeof(*_tmp865));_tmp865[0]=s;_tmp865;});
struct Cyc_Absyn_Aggrfield*_tmp858=({struct Cyc_Absyn_Aggrfield*_tmp864=_cycalloc(sizeof(*_tmp864));_tmp864->name=Cyc_Toc_val_sp;_tmp864->tq=Cyc_Toc_mt_tq;_tmp864->type=_tmp856;_tmp864->width=0;_tmp864->attributes=0;_tmp864->requires_clause=0;_tmp864;});
struct Cyc_Absyn_Aggrfield*_tmp859=({struct Cyc_Absyn_Aggrfield*_tmp863=_cycalloc(sizeof(*_tmp863));_tmp863->name=Cyc_Toc_tag_sp;_tmp863->tq=Cyc_Toc_mt_tq;_tmp863->type=Cyc_Absyn_sint_typ;_tmp863->width=0;_tmp863->attributes=0;_tmp863->requires_clause=0;_tmp863;});
struct Cyc_List_List*_tmp85A=({struct Cyc_Absyn_Aggrfield*_tmp862[2];_tmp862[1]=_tmp858;_tmp862[0]=_tmp859;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp862,sizeof(struct Cyc_Absyn_Aggrfield*),2));});
struct Cyc_Absyn_Aggrdecl*_tmp85B=({struct Cyc_Absyn_Aggrdecl*_tmp85F=_cycalloc(sizeof(*_tmp85F));_tmp85F->kind=Cyc_Absyn_StructA;_tmp85F->sc=Cyc_Absyn_Public;_tmp85F->name=({struct _tuple1*_tmp861=_cycalloc(sizeof(*_tmp861));_tmp861->f1=Cyc_Absyn_Loc_n;_tmp861->f2=str;_tmp861;});_tmp85F->tvs=0;_tmp85F->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp860=_cycalloc(sizeof(*_tmp860));_tmp860->exist_vars=0;_tmp860->rgn_po=0;_tmp860->fields=_tmp85A;_tmp860->tagged=0;_tmp860;});_tmp85F->expected_mem_kind=0;_tmp85F->attributes=0;_tmp85F;});
# 4900
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp85C=_cycalloc(sizeof(*_tmp85C));_tmp85C->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp85D=_cycalloc(sizeof(*_tmp85D));_tmp85D[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp85E;_tmp85E.tag=5;_tmp85E.f1=_tmp85B;_tmp85E;});_tmp85D;}),0);_tmp85C->tl=Cyc_Toc_result_decls;_tmp85C;});
_tmp855->type=Cyc_Absyn_strct(str);}
# 4903
new_fields=({struct Cyc_List_List*_tmp86A=_cycalloc(sizeof(*_tmp86A));_tmp86A->hd=_tmp855;_tmp86A->tl=new_fields;_tmp86A;});};}}
# 4905
(new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);};}
# 4907
if(_tmp834)
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp86C=_cycalloc(sizeof(*_tmp86C));_tmp86C->hd=({struct Cyc_Absyn_Decl*_tmp86D=_cycalloc(sizeof(*_tmp86D));_tmp86D->r=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp86E=_cycalloc(sizeof(*_tmp86E));_tmp86E[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp86F;_tmp86F.tag=5;_tmp86F.f1=new_ad;_tmp86F;});_tmp86E;});_tmp86D->loc=0;_tmp86D;});_tmp86C->tl=Cyc_Toc_result_decls;_tmp86C;});
return 0;};};};}
# 4912
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple36 p=({struct _tuple36 _tmp871;_tmp871.f1=ad;_tmp871.f2=add_to_decls;_tmp871;});
((int(*)(struct _tuple36*arg,int(*f)(struct _RegionHandle*,struct _tuple37*)))Cyc_Toc_use_toc_state)(& p,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple38{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 4941 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple38*env){
# 4944
struct _tuple38 _tmp872=*env;struct _tuple38 _tmp873=_tmp872;struct Cyc_Set_Set**_tmp874;struct Cyc_Absyn_Datatypedecl*_tmp875;_LL4F8: _tmp874=(_tmp873.f1)->datatypes_so_far;_tmp875=_tmp873.f2;_LL4F9:;{
struct _tuple1*_tmp876=_tmp875->name;
if(_tmp875->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*_tmp874,_tmp876))
return 0;
*_tmp874=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*_tmp874,_tmp876);
{struct Cyc_List_List*_tmp877=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp875->fields))->v;for(0;_tmp877 != 0;_tmp877=_tmp877->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp877->hd;
# 4952
struct Cyc_List_List*_tmp878=0;
int i=1;
{struct Cyc_List_List*_tmp879=f->typs;for(0;_tmp879 != 0;(_tmp879=_tmp879->tl,i ++)){
struct _dyneither_ptr*_tmp87A=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmp87B=({struct Cyc_Absyn_Aggrfield*_tmp87D=_cycalloc(sizeof(*_tmp87D));_tmp87D->name=_tmp87A;_tmp87D->tq=(*((struct _tuple11*)_tmp879->hd)).f1;_tmp87D->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp879->hd)).f2);_tmp87D->width=0;_tmp87D->attributes=0;_tmp87D->requires_clause=0;_tmp87D;});
_tmp878=({struct Cyc_List_List*_tmp87C=_cycalloc(sizeof(*_tmp87C));_tmp87C->hd=_tmp87B;_tmp87C->tl=_tmp878;_tmp87C;});}}
# 4960
_tmp878=({struct Cyc_List_List*_tmp87E=_cycalloc(sizeof(*_tmp87E));_tmp87E->hd=({struct Cyc_Absyn_Aggrfield*_tmp87F=_cycalloc(sizeof(*_tmp87F));_tmp87F->name=Cyc_Toc_tag_sp;_tmp87F->tq=Cyc_Toc_mt_tq;_tmp87F->type=Cyc_Absyn_sint_typ;_tmp87F->width=0;_tmp87F->attributes=0;_tmp87F->requires_clause=0;_tmp87F;});_tmp87E->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp878);_tmp87E;});{
struct Cyc_Absyn_Aggrdecl*_tmp880=({struct Cyc_Absyn_Aggrdecl*_tmp884=_cycalloc(sizeof(*_tmp884));_tmp884->kind=Cyc_Absyn_StructA;_tmp884->sc=Cyc_Absyn_Public;_tmp884->name=
# 4964
Cyc_Toc_collapse_qvars(f->name,_tmp875->name);_tmp884->tvs=0;_tmp884->expected_mem_kind=0;_tmp884->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp885=_cycalloc(sizeof(*_tmp885));_tmp885->exist_vars=0;_tmp885->rgn_po=0;_tmp885->fields=_tmp878;_tmp885->tagged=0;_tmp885;});_tmp884->attributes=0;_tmp884;});
# 4968
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp881=_cycalloc(sizeof(*_tmp881));_tmp881->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp882=_cycalloc(sizeof(*_tmp882));_tmp882[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp883;_tmp883.tag=5;_tmp883.f1=_tmp880;_tmp883;});_tmp882;}),0);_tmp881->tl=Cyc_Toc_result_decls;_tmp881;});};}}
# 4970
return 0;};}
# 4973
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple38*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 4992 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple38*env){
# 4995
struct _tuple38 _tmp886=*env;struct _tuple38 _tmp887=_tmp886;struct Cyc_Toc_TocState*_tmp888;struct Cyc_Absyn_Datatypedecl*_tmp889;_LL4FB: _tmp888=_tmp887.f1;_tmp889=_tmp887.f2;_LL4FC:;
if(_tmp889->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp88A=*_tmp888;struct Cyc_Toc_TocState _tmp88B=_tmp88A;struct Cyc_Dict_Dict*_tmp88C;_LL4FE: _tmp88C=_tmp88B.xdatatypes_so_far;_LL4FF:;{
struct _tuple1*_tmp88D=_tmp889->name;
{struct Cyc_List_List*_tmp88E=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp889->fields))->v;for(0;_tmp88E != 0;_tmp88E=_tmp88E->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp88E->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp88F=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0);
void*_tmp890=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,_tmp88F,Cyc_Absyn_false_conref,0);
# 5006
int*_tmp891=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp88C,f->name);int*_tmp892=_tmp891;if(_tmp892 == 0){_LL501: _LL502: {
# 5008
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0);{
# 5012
struct Cyc_Absyn_Vardecl*_tmp893=Cyc_Absyn_new_vardecl(f->name,_tmp890,initopt);
_tmp893->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp894=_cycalloc(sizeof(*_tmp894));_tmp894->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp895=_cycalloc(sizeof(*_tmp895));_tmp895[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp896;_tmp896.tag=0;_tmp896.f1=_tmp893;_tmp896;});_tmp895;}),0);_tmp894->tl=Cyc_Toc_result_decls;_tmp894;});
*_tmp88C=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp88C,f->name,f->sc != Cyc_Absyn_Extern);{
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp897=f->typs;for(0;_tmp897 != 0;(_tmp897=_tmp897->tl,i ++)){
struct _dyneither_ptr*_tmp898=({struct _dyneither_ptr*_tmp89C=_cycalloc(sizeof(*_tmp89C));_tmp89C[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp89F;_tmp89F.tag=1;_tmp89F.f1=(unsigned long)i;({void*_tmp89D[1]={& _tmp89F};Cyc_aprintf(({const char*_tmp89E="f%d";_tag_dyneither(_tmp89E,sizeof(char),4);}),_tag_dyneither(_tmp89D,sizeof(void*),1));});});_tmp89C;});
struct Cyc_Absyn_Aggrfield*_tmp899=({struct Cyc_Absyn_Aggrfield*_tmp89B=_cycalloc(sizeof(*_tmp89B));_tmp89B->name=_tmp898;_tmp89B->tq=(*((struct _tuple11*)_tmp897->hd)).f1;_tmp89B->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp897->hd)).f2);_tmp89B->width=0;_tmp89B->attributes=0;_tmp89B->requires_clause=0;_tmp89B;});
fields=({struct Cyc_List_List*_tmp89A=_cycalloc(sizeof(*_tmp89A));_tmp89A->hd=_tmp899;_tmp89A->tl=fields;_tmp89A;});}}
# 5025
fields=({struct Cyc_List_List*_tmp8A0=_cycalloc(sizeof(*_tmp8A0));_tmp8A0->hd=({struct Cyc_Absyn_Aggrfield*_tmp8A1=_cycalloc(sizeof(*_tmp8A1));_tmp8A1->name=Cyc_Toc_tag_sp;_tmp8A1->tq=Cyc_Toc_mt_tq;_tmp8A1->type=
Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq);_tmp8A1->width=0;_tmp8A1->attributes=0;_tmp8A1->requires_clause=0;_tmp8A1;});_tmp8A0->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp8A0;});{
struct Cyc_Absyn_Aggrdecl*_tmp8A2=({struct Cyc_Absyn_Aggrdecl*_tmp8A6=_cycalloc(sizeof(*_tmp8A6));_tmp8A6->kind=Cyc_Absyn_StructA;_tmp8A6->sc=Cyc_Absyn_Public;_tmp8A6->name=
# 5030
Cyc_Toc_collapse_qvars(f->name,_tmp889->name);_tmp8A6->tvs=0;_tmp8A6->expected_mem_kind=0;_tmp8A6->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp8A7=_cycalloc(sizeof(*_tmp8A7));_tmp8A7->exist_vars=0;_tmp8A7->rgn_po=0;_tmp8A7->fields=fields;_tmp8A7->tagged=0;_tmp8A7;});_tmp8A6->attributes=0;_tmp8A6;});
# 5034
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp8A3=_cycalloc(sizeof(*_tmp8A3));_tmp8A3->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp8A4=_cycalloc(sizeof(*_tmp8A4));_tmp8A4[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp8A5;_tmp8A5.tag=5;_tmp8A5.f1=_tmp8A2;_tmp8A5;});_tmp8A4;}),0);_tmp8A3->tl=Cyc_Toc_result_decls;_tmp8A3;});
# 5036
goto _LL500;};};};}}else{if(*((int*)_tmp892)== 0){_LL503: _LL504:
# 5038
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp8A8=Cyc_Absyn_string_exp(*fn,0);
struct Cyc_Absyn_Vardecl*_tmp8A9=Cyc_Absyn_new_vardecl(f->name,_tmp890,_tmp8A8);
_tmp8A9->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp8AA=_cycalloc(sizeof(*_tmp8AA));_tmp8AA->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp8AB=_cycalloc(sizeof(*_tmp8AB));_tmp8AB[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp8AC;_tmp8AC.tag=0;_tmp8AC.f1=_tmp8A9;_tmp8AC;});_tmp8AB;}),0);_tmp8AA->tl=Cyc_Toc_result_decls;_tmp8AA;});
*_tmp88C=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp88C,f->name,1);}
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
void*_tmp8AD=init->r;void*_tmp8AE=_tmp8AD;struct Cyc_Absyn_Vardecl*_tmp8AF;struct Cyc_Absyn_Exp*_tmp8B0;struct Cyc_Absyn_Exp*_tmp8B1;int _tmp8B2;if(((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp8AE)->tag == 26){_LL508: _tmp8AF=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp8AE)->f1;_tmp8B0=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp8AE)->f2;_tmp8B1=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp8AE)->f3;_tmp8B2=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp8AE)->f4;_LL509:
# 5076
 vd->initializer=0;
s->r=(Cyc_Toc_init_comprehension(init_nv,old_typ,Cyc_Absyn_var_exp(vd->name,0),_tmp8AF,_tmp8B0,_tmp8B1,_tmp8B2,
Cyc_Absyn_new_stmt(s->r,0),0))->r;
goto _LL507;}else{_LL50A: _LL50B:
# 5081
 if(vd->sc == Cyc_Absyn_Static){
# 5085
struct _RegionHandle _tmp8B3=_new_region("temp");struct _RegionHandle*temp=& _tmp8B3;_push_region(temp);
{struct Cyc_Toc_Env*_tmp8B4=Cyc_Toc_set_toplevel(temp,init_nv);
Cyc_Toc_exp_to_c(_tmp8B4,init);}
# 5086
;_pop_region(temp);}else{
# 5090
Cyc_Toc_exp_to_c(init_nv,init);}
goto _LL507;}_LL507:;}else{
# 5095
void*_tmp8B5=Cyc_Tcutil_compress(old_typ);void*_tmp8B6=_tmp8B5;void*_tmp8B7;struct Cyc_Absyn_Exp*_tmp8B8;union Cyc_Absyn_Constraint*_tmp8B9;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8B6)->tag == 8){_LL50D: _tmp8B7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8B6)->f1).elt_type;_tmp8B8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8B6)->f1).num_elts;_tmp8B9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8B6)->f1).zero_term;if(
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8B9)){_LL50E:
 if(_tmp8B8 == 0)
({void*_tmp8BA=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp8BB="can't initialize zero-terminated array -- size unknown";_tag_dyneither(_tmp8BB,sizeof(char),55);}),_tag_dyneither(_tmp8BA,sizeof(void*),0));});{
struct Cyc_Absyn_Exp*num_elts=_tmp8B8;
struct Cyc_Absyn_Exp*_tmp8BC=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),
Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);
# 5103
struct Cyc_Absyn_Exp*_tmp8BD=Cyc_Absyn_signed_int_exp(0,0);
s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp8BC,_tmp8BD,0),0),
Cyc_Absyn_new_stmt(s->r,0));
goto _LL50C;};}else{goto _LL50F;}}else{_LL50F: _LL510:
 goto _LL50C;}_LL50C:;}}
# 5114
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success){
void*_tmp8BE=d;struct Cyc_List_List*_tmp8BF;struct Cyc_List_List*_tmp8C0;void**_tmp8C1;struct Cyc_Tcpat_Rhs*_tmp8C2;switch(*((int*)_tmp8BE)){case 0: _LL512: _LL513:
 return d;case 1: _LL514: _tmp8C2=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp8BE)->f1;_LL515:
 _tmp8C2->rhs=success;return d;default: _LL516: _tmp8BF=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp8BE)->f1;_tmp8C0=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp8BE)->f2;_tmp8C1=(void**)&((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp8BE)->f3;_LL517:
# 5119
*_tmp8C1=Cyc_Toc_rewrite_decision(*_tmp8C1,success);
for(0;_tmp8C0 != 0;_tmp8C0=_tmp8C0->tl){
struct _tuple29*_tmp8C3=(struct _tuple29*)_tmp8C0->hd;struct _tuple29*_tmp8C4=_tmp8C3;void**_tmp8C5;_LL519: _tmp8C5=(void**)& _tmp8C4->f2;_LL51A:;
*_tmp8C5=Cyc_Toc_rewrite_decision(*_tmp8C5,success);}
# 5124
return d;}_LL511:;}
# 5135 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 5137
struct _RegionHandle _tmp8C6=_new_region("rgn");struct _RegionHandle*rgn=& _tmp8C6;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp8C7=Cyc_Toc_share_env(rgn,nv);
void*_tmp8C8=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(_tmp8C7,e);{
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 5147
struct Cyc_Absyn_Stmt*_tmp8C9=Cyc_Toc_skip_stmt_dl();
if(dopt != 0)
# 5151
dopt=Cyc_Toc_rewrite_decision(dopt,_tmp8C9);{
# 5153
struct Cyc_Absyn_Switch_clause*_tmp8CA=({struct Cyc_Absyn_Switch_clause*_tmp8D8=_cycalloc(sizeof(*_tmp8D8));_tmp8D8->pattern=p;_tmp8D8->pat_vars=0;_tmp8D8->where_clause=0;_tmp8D8->body=_tmp8C9;_tmp8D8->loc=0;_tmp8D8;});
struct Cyc_List_List*_tmp8CB=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple31*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,({struct Cyc_Absyn_Switch_clause*_tmp8D7[1];_tmp8D7[0]=_tmp8CA;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(rgn,_tag_dyneither(_tmp8D7,sizeof(struct Cyc_Absyn_Switch_clause*),1));}));
# 5156
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
# 5160
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp8C7,& mydecls,& mybodies,dopt,_tmp8CB,v);
# 5163
struct Cyc_Toc_Env*senv;
for(0;mybodies != 0;mybodies=((struct Cyc_List_List*)_check_null(mybodies))->tl){
struct _tuple35*_tmp8CC=(struct _tuple35*)((struct Cyc_List_List*)_check_null(mybodies))->hd;struct _tuple35*_tmp8CD=_tmp8CC;struct Cyc_Toc_Env*_tmp8CE;struct Cyc_Absyn_Stmt*_tmp8CF;_LL51C: _tmp8CE=_tmp8CD->f1;_tmp8CF=_tmp8CD->f3;_LL51D:;
if(_tmp8CF == _tmp8C9){senv=_tmp8CE;goto FOUND_ENV;}}
# 5168
({void*_tmp8D0=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp8D1="letdecl_to_c: couldn't find env!";_tag_dyneither(_tmp8D1,sizeof(char),33);}),_tag_dyneither(_tmp8D0,sizeof(void*),0));});
FOUND_ENV:
# 5172
 Cyc_Toc_stmt_to_c(senv,s);{
# 5174
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(test_tree,s,0);
# 5176
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct _tuple33*_tmp8D2=(struct _tuple33*)((struct Cyc_List_List*)_check_null(mydecls))->hd;struct _tuple33*_tmp8D3=_tmp8D2;struct _tuple1*_tmp8D4;void*_tmp8D5;_LL51F: _tmp8D4=_tmp8D3->f1;_tmp8D5=_tmp8D3->f2;_LL520:;
res=Cyc_Absyn_declare_stmt(_tmp8D4,_tmp8D5,0,res,0);}
# 5181
res=Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c(_tmp8C8),e,res,0);{
struct Cyc_Absyn_Stmt*_tmp8D6=res;_npop_handler(0);return _tmp8D6;};};};};}
# 5138
;_pop_region(rgn);}
# 5189
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp8D9=e->r;void*_tmp8DA=_tmp8D9;struct Cyc_Absyn_MallocInfo*_tmp8DB;struct Cyc_Absyn_Stmt*_tmp8DC;void**_tmp8DD;void**_tmp8DE;struct Cyc_List_List*_tmp8DF;struct Cyc_List_List*_tmp8E0;void**_tmp8E1;struct Cyc_List_List*_tmp8E2;void**_tmp8E3;struct Cyc_Absyn_Exp*_tmp8E4;struct Cyc_Absyn_Exp*_tmp8E5;struct Cyc_List_List*_tmp8E6;struct Cyc_Absyn_Exp*_tmp8E7;struct Cyc_Absyn_Exp*_tmp8E8;struct Cyc_Absyn_Exp*_tmp8E9;struct Cyc_Absyn_Exp*_tmp8EA;struct Cyc_Absyn_Exp*_tmp8EB;struct Cyc_Absyn_Exp*_tmp8EC;struct Cyc_Absyn_Exp*_tmp8ED;struct Cyc_Absyn_Exp*_tmp8EE;struct Cyc_Absyn_Exp*_tmp8EF;struct Cyc_Absyn_Exp*_tmp8F0;struct Cyc_Absyn_Exp*_tmp8F1;struct Cyc_Absyn_Exp*_tmp8F2;struct Cyc_Absyn_Exp*_tmp8F3;struct Cyc_Absyn_Exp*_tmp8F4;struct Cyc_Absyn_Exp*_tmp8F5;struct Cyc_List_List*_tmp8F6;struct Cyc_Absyn_Exp*_tmp8F7;struct Cyc_Absyn_Exp*_tmp8F8;struct Cyc_Absyn_Exp*_tmp8F9;struct Cyc_Absyn_Exp*_tmp8FA;struct Cyc_Absyn_Exp*_tmp8FB;struct Cyc_Absyn_Exp*_tmp8FC;struct Cyc_Absyn_Exp*_tmp8FD;struct Cyc_Absyn_Exp*_tmp8FE;switch(*((int*)_tmp8DA)){case 19: _LL522: _tmp8FE=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp8DA)->f1;_LL523:
 _tmp8FD=_tmp8FE;goto _LL525;case 20: _LL524: _tmp8FD=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8DA)->f1;_LL525:
 _tmp8FC=_tmp8FD;goto _LL527;case 21: _LL526: _tmp8FC=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp8DA)->f1;_LL527:
 _tmp8FB=_tmp8FC;goto _LL529;case 14: _LL528: _tmp8FB=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp8DA)->f1;_LL529:
 _tmp8FA=_tmp8FB;goto _LL52B;case 10: _LL52A: _tmp8FA=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp8DA)->f1;_LL52B:
 _tmp8F9=_tmp8FA;goto _LL52D;case 11: _LL52C: _tmp8F9=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp8DA)->f1;_LL52D:
 _tmp8F8=_tmp8F9;goto _LL52F;case 17: _LL52E: _tmp8F8=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp8DA)->f1;_LL52F:
 _tmp8F7=_tmp8F8;goto _LL531;case 4: _LL530: _tmp8F7=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp8DA)->f1;_LL531:
 Cyc_Toc_exptypes_to_c(_tmp8F7);goto _LL521;case 2: _LL532: _tmp8F6=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp8DA)->f2;_LL533:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp8F6);goto _LL521;case 6: _LL534: _tmp8F4=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp8DA)->f1;_tmp8F5=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp8DA)->f2;_LL535:
 _tmp8F2=_tmp8F4;_tmp8F3=_tmp8F5;goto _LL537;case 7: _LL536: _tmp8F2=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp8DA)->f1;_tmp8F3=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp8DA)->f2;_LL537:
 _tmp8F0=_tmp8F2;_tmp8F1=_tmp8F3;goto _LL539;case 8: _LL538: _tmp8F0=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp8DA)->f1;_tmp8F1=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp8DA)->f2;_LL539:
 _tmp8EE=_tmp8F0;_tmp8EF=_tmp8F1;goto _LL53B;case 22: _LL53A: _tmp8EE=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8DA)->f1;_tmp8EF=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8DA)->f2;_LL53B:
 _tmp8EC=_tmp8EE;_tmp8ED=_tmp8EF;goto _LL53D;case 34: _LL53C: _tmp8EC=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp8DA)->f1;_tmp8ED=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp8DA)->f2;_LL53D:
 _tmp8EA=_tmp8EC;_tmp8EB=_tmp8ED;goto _LL53F;case 3: _LL53E: _tmp8EA=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp8DA)->f1;_tmp8EB=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp8DA)->f3;_LL53F:
 Cyc_Toc_exptypes_to_c(_tmp8EA);Cyc_Toc_exptypes_to_c(_tmp8EB);goto _LL521;case 5: _LL540: _tmp8E7=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8DA)->f1;_tmp8E8=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8DA)->f2;_tmp8E9=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8DA)->f3;_LL541:
# 5207
 Cyc_Toc_exptypes_to_c(_tmp8E7);Cyc_Toc_exptypes_to_c(_tmp8E8);Cyc_Toc_exptypes_to_c(_tmp8E9);goto _LL521;case 9: _LL542: _tmp8E5=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp8DA)->f1;_tmp8E6=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp8DA)->f2;_LL543:
# 5209
 Cyc_Toc_exptypes_to_c(_tmp8E5);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp8E6);goto _LL521;case 13: _LL544: _tmp8E3=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp8DA)->f1;_tmp8E4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp8DA)->f2;_LL545:
*_tmp8E3=Cyc_Toc_typ_to_c(*_tmp8E3);Cyc_Toc_exptypes_to_c(_tmp8E4);goto _LL521;case 24: _LL546: _tmp8E1=(void**)&(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp8DA)->f1)->f3;_tmp8E2=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp8DA)->f2;_LL547:
# 5212
*_tmp8E1=Cyc_Toc_typ_to_c(*_tmp8E1);
_tmp8E0=_tmp8E2;goto _LL549;case 35: _LL548: _tmp8E0=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp8DA)->f2;_LL549:
 _tmp8DF=_tmp8E0;goto _LL54B;case 25: _LL54A: _tmp8DF=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp8DA)->f1;_LL54B:
# 5216
 for(0;_tmp8DF != 0;_tmp8DF=_tmp8DF->tl){
struct _tuple19 _tmp8FF=*((struct _tuple19*)_tmp8DF->hd);struct _tuple19 _tmp900=_tmp8FF;struct Cyc_Absyn_Exp*_tmp901;_LL573: _tmp901=_tmp900.f2;_LL574:;
Cyc_Toc_exptypes_to_c(_tmp901);}
# 5220
goto _LL521;case 18: _LL54C: _tmp8DE=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp8DA)->f1;_LL54D:
 _tmp8DD=_tmp8DE;goto _LL54F;case 16: _LL54E: _tmp8DD=(void**)&((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp8DA)->f1;_LL54F:
*_tmp8DD=Cyc_Toc_typ_to_c(*_tmp8DD);goto _LL521;case 36: _LL550: _tmp8DC=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp8DA)->f1;_LL551:
 Cyc_Toc_stmttypes_to_c(_tmp8DC);goto _LL521;case 33: _LL552: _tmp8DB=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp8DA)->f1;_LL553:
# 5225
 if(_tmp8DB->elt_type != 0)
_tmp8DB->elt_type=({void**_tmp902=_cycalloc(sizeof(*_tmp902));_tmp902[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp8DB->elt_type)));_tmp902;});
Cyc_Toc_exptypes_to_c(_tmp8DB->num_elts);
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
({void*_tmp903=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp904="Cyclone expression within C code";_tag_dyneither(_tmp904,sizeof(char),33);}),_tag_dyneither(_tmp903,sizeof(void*),0));});
goto _LL521;}_LL521:;}
# 5250
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp905=d->r;void*_tmp906=_tmp905;struct Cyc_Absyn_Typedefdecl*_tmp907;struct Cyc_Absyn_Enumdecl*_tmp908;struct Cyc_Absyn_Aggrdecl*_tmp909;struct Cyc_Absyn_Fndecl*_tmp90A;struct Cyc_Absyn_Vardecl*_tmp90B;switch(*((int*)_tmp906)){case 0: _LL576: _tmp90B=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp906)->f1;_LL577:
# 5253
 _tmp90B->type=Cyc_Toc_typ_to_c(_tmp90B->type);
if(_tmp90B->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp90B->initializer));
goto _LL575;case 1: _LL578: _tmp90A=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp906)->f1;_LL579:
# 5257
 _tmp90A->ret_type=Cyc_Toc_typ_to_c(_tmp90A->ret_type);
{struct Cyc_List_List*_tmp90C=_tmp90A->args;for(0;_tmp90C != 0;_tmp90C=_tmp90C->tl){
(*((struct _tuple9*)_tmp90C->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp90C->hd)).f3);}}
# 5261
goto _LL575;case 5: _LL57A: _tmp909=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp906)->f1;_LL57B:
 Cyc_Toc_aggrdecl_to_c(_tmp909,1);goto _LL575;case 7: _LL57C: _tmp908=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp906)->f1;_LL57D:
# 5264
 if(_tmp908->fields != 0){
struct Cyc_List_List*_tmp90D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp908->fields))->v;for(0;_tmp90D != 0;_tmp90D=_tmp90D->tl){
struct Cyc_Absyn_Enumfield*_tmp90E=(struct Cyc_Absyn_Enumfield*)_tmp90D->hd;
if(_tmp90E->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp90E->tag));}}
# 5269
goto _LL575;case 8: _LL57E: _tmp907=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp906)->f1;_LL57F:
 _tmp907->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp907->defn));goto _LL575;case 2: _LL580: _LL581:
 goto _LL583;case 3: _LL582: _LL583:
 goto _LL585;case 6: _LL584: _LL585:
 goto _LL587;case 9: _LL586: _LL587:
 goto _LL589;case 10: _LL588: _LL589:
 goto _LL58B;case 11: _LL58A: _LL58B:
 goto _LL58D;case 12: _LL58C: _LL58D:
 goto _LL58F;case 4: _LL58E: _LL58F:
# 5280
({void*_tmp90F=0;Cyc_Tcutil_terr(d->loc,({const char*_tmp910="Cyclone declaration within C code";_tag_dyneither(_tmp910,sizeof(char),34);}),_tag_dyneither(_tmp90F,sizeof(void*),0));});
goto _LL575;case 13: _LL590: _LL591:
 goto _LL593;default: _LL592: _LL593:
# 5284
 goto _LL575;}_LL575:;}
# 5288
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp911=s->r;void*_tmp912=_tmp911;struct Cyc_Absyn_Stmt*_tmp913;struct Cyc_Absyn_Stmt*_tmp914;struct Cyc_Absyn_Exp*_tmp915;struct Cyc_Absyn_Decl*_tmp916;struct Cyc_Absyn_Stmt*_tmp917;struct Cyc_Absyn_Exp*_tmp918;struct Cyc_List_List*_tmp919;void*_tmp91A;struct Cyc_Absyn_Exp*_tmp91B;struct Cyc_Absyn_Exp*_tmp91C;struct Cyc_Absyn_Exp*_tmp91D;struct Cyc_Absyn_Stmt*_tmp91E;struct Cyc_Absyn_Exp*_tmp91F;struct Cyc_Absyn_Stmt*_tmp920;struct Cyc_Absyn_Exp*_tmp921;struct Cyc_Absyn_Stmt*_tmp922;struct Cyc_Absyn_Stmt*_tmp923;struct Cyc_Absyn_Exp*_tmp924;struct Cyc_Absyn_Stmt*_tmp925;struct Cyc_Absyn_Stmt*_tmp926;struct Cyc_Absyn_Exp*_tmp927;switch(*((int*)_tmp912)){case 1: _LL595: _tmp927=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp912)->f1;_LL596:
 Cyc_Toc_exptypes_to_c(_tmp927);goto _LL594;case 2: _LL597: _tmp925=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp912)->f1;_tmp926=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp912)->f2;_LL598:
 Cyc_Toc_stmttypes_to_c(_tmp925);Cyc_Toc_stmttypes_to_c(_tmp926);goto _LL594;case 3: _LL599: _tmp924=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp912)->f1;_LL59A:
 if(_tmp924 != 0)Cyc_Toc_exptypes_to_c(_tmp924);goto _LL594;case 4: _LL59B: _tmp921=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp912)->f1;_tmp922=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp912)->f2;_tmp923=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp912)->f3;_LL59C:
# 5294
 Cyc_Toc_exptypes_to_c(_tmp921);Cyc_Toc_stmttypes_to_c(_tmp922);Cyc_Toc_stmttypes_to_c(_tmp923);goto _LL594;case 5: _LL59D: _tmp91F=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp912)->f1).f1;_tmp920=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp912)->f2;_LL59E:
# 5296
 Cyc_Toc_exptypes_to_c(_tmp91F);Cyc_Toc_stmttypes_to_c(_tmp920);goto _LL594;case 9: _LL59F: _tmp91B=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp912)->f1;_tmp91C=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp912)->f2).f1;_tmp91D=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp912)->f3).f1;_tmp91E=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp912)->f4;_LL5A0:
# 5298
 Cyc_Toc_exptypes_to_c(_tmp91B);Cyc_Toc_exptypes_to_c(_tmp91C);Cyc_Toc_exptypes_to_c(_tmp91D);
Cyc_Toc_stmttypes_to_c(_tmp91E);goto _LL594;case 10: _LL5A1: _tmp918=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp912)->f1;_tmp919=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp912)->f2;_tmp91A=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp912)->f3;_LL5A2:
# 5301
 Cyc_Toc_exptypes_to_c(_tmp918);
for(0;_tmp919 != 0;_tmp919=_tmp919->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp919->hd)->body);}
goto _LL594;case 12: _LL5A3: _tmp916=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp912)->f1;_tmp917=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp912)->f2;_LL5A4:
 Cyc_Toc_decltypes_to_c(_tmp916);Cyc_Toc_stmttypes_to_c(_tmp917);goto _LL594;case 14: _LL5A5: _tmp914=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp912)->f1;_tmp915=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp912)->f2).f1;_LL5A6:
 Cyc_Toc_stmttypes_to_c(_tmp914);Cyc_Toc_exptypes_to_c(_tmp915);goto _LL594;case 13: _LL5A7: _tmp913=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp912)->f2;_LL5A8:
 Cyc_Toc_stmttypes_to_c(_tmp913);goto _LL594;case 0: _LL5A9: _LL5AA:
 goto _LL5AC;case 6: _LL5AB: _LL5AC:
 goto _LL5AE;case 7: _LL5AD: _LL5AE:
 goto _LL5B0;case 8: _LL5AF: _LL5B0:
# 5311
 goto _LL594;case 11: _LL5B1: _LL5B2:
# 5314
 s->r=(void*)({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp928=_cycalloc_atomic(sizeof(*_tmp928));_tmp928[0]=({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct _tmp929;_tmp929.tag=0;_tmp929;});_tmp928;});
goto _LL594;case 15: _LL5B3: _LL5B4:
 goto _LL5B6;default: _LL5B5: _LL5B6:
# 5318
({void*_tmp92A=0;Cyc_Tcutil_terr(s->loc,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp92D;_tmp92D.tag=0;_tmp92D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));({void*_tmp92B[1]={& _tmp92D};Cyc_aprintf(({const char*_tmp92C="Cyclone statement in C code: %s";_tag_dyneither(_tmp92C,sizeof(char),32);}),_tag_dyneither(_tmp92B,sizeof(void*),1));});}),_tag_dyneither(_tmp92A,sizeof(void*),0));});
goto _LL594;}_LL594:;}
# 5327
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv))
({void*_tmp92E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp92F="decls_to_c: not at toplevel!";_tag_dyneither(_tmp92F,sizeof(char),29);}),_tag_dyneither(_tmp92E,sizeof(void*),0));});{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp930=d->r;void*_tmp931=_tmp930;struct Cyc_List_List*_tmp932;struct Cyc_List_List*_tmp933;struct Cyc_List_List*_tmp934;struct Cyc_List_List*_tmp935;struct Cyc_Absyn_Typedefdecl*_tmp936;struct Cyc_Absyn_Enumdecl*_tmp937;struct Cyc_Absyn_Datatypedecl*_tmp938;struct Cyc_Absyn_Aggrdecl*_tmp939;struct Cyc_Absyn_Fndecl*_tmp93A;struct Cyc_Absyn_Vardecl*_tmp93B;switch(*((int*)_tmp931)){case 0: _LL5B8: _tmp93B=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp931)->f1;_LL5B9: {
# 5334
struct _tuple1*_tmp93C=_tmp93B->name;
# 5336
if(_tmp93B->sc == Cyc_Absyn_ExternC)
_tmp93C=({struct _tuple1*_tmp93D=_cycalloc(sizeof(*_tmp93D));_tmp93D->f1=Cyc_Absyn_Abs_n(0,1);_tmp93D->f2=(*_tmp93C).f2;_tmp93D;});
if(_tmp93B->initializer != 0){
if(_tmp93B->sc == Cyc_Absyn_ExternC)_tmp93B->sc=Cyc_Absyn_Public;
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp93B->initializer));else{
# 5343
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp93B->initializer));}}
# 5346
nv=Cyc_Toc_add_varmap(r,nv,_tmp93C,Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp93E=_cycalloc(sizeof(*_tmp93E));_tmp93E[0]=({struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp93F;_tmp93F.tag=1;_tmp93F.f1=_tmp93B;_tmp93F;});_tmp93E;}),0));
_tmp93B->name=_tmp93C;
_tmp93B->sc=Cyc_Toc_scope_to_c(_tmp93B->sc);
_tmp93B->type=Cyc_Toc_typ_to_c(_tmp93B->type);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp940=_cycalloc(sizeof(*_tmp940));_tmp940->hd=d;_tmp940->tl=Cyc_Toc_result_decls;_tmp940;});
goto _LL5B7;}case 1: _LL5BA: _tmp93A=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp931)->f1;_LL5BB: {
# 5353
struct _tuple1*_tmp941=_tmp93A->name;
# 5355
if(_tmp93A->sc == Cyc_Absyn_ExternC){
_tmp941=({struct _tuple1*_tmp942=_cycalloc(sizeof(*_tmp942));_tmp942->f1=Cyc_Absyn_Abs_n(0,1);_tmp942->f2=(*_tmp941).f2;_tmp942;});
_tmp93A->sc=Cyc_Absyn_Public;}
# 5360
nv=Cyc_Toc_add_varmap(r,nv,_tmp941,Cyc_Absyn_var_exp(_tmp941,0));
_tmp93A->name=_tmp941;
Cyc_Toc_fndecl_to_c(nv,_tmp93A,cinclude);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp943=_cycalloc(sizeof(*_tmp943));_tmp943->hd=d;_tmp943->tl=Cyc_Toc_result_decls;_tmp943;});
goto _LL5B7;}case 2: _LL5BC: _LL5BD:
 goto _LL5BF;case 3: _LL5BE: _LL5BF:
({void*_tmp944=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp945="letdecl at toplevel";_tag_dyneither(_tmp945,sizeof(char),20);}),_tag_dyneither(_tmp944,sizeof(void*),0));});case 4: _LL5C0: _LL5C1:
({void*_tmp946=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp947="region decl at toplevel";_tag_dyneither(_tmp947,sizeof(char),24);}),_tag_dyneither(_tmp946,sizeof(void*),0));});case 5: _LL5C2: _tmp939=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp931)->f1;_LL5C3:
# 5369
 Cyc_Toc_aggrdecl_to_c(_tmp939,1);
goto _LL5B7;case 6: _LL5C4: _tmp938=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp931)->f1;_LL5C5:
# 5372
 if(_tmp938->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp938);else{
# 5375
Cyc_Toc_datatypedecl_to_c(_tmp938);}
goto _LL5B7;case 7: _LL5C6: _tmp937=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp931)->f1;_LL5C7:
# 5378
 Cyc_Toc_enumdecl_to_c(_tmp937);
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp948=_cycalloc(sizeof(*_tmp948));_tmp948->hd=d;_tmp948->tl=Cyc_Toc_result_decls;_tmp948;});
goto _LL5B7;case 8: _LL5C8: _tmp936=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp931)->f1;_LL5C9:
# 5382
 _tmp936->tvs=0;
if(_tmp936->defn != 0)
_tmp936->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp936->defn));else{
# 5386
enum Cyc_Absyn_KindQual _tmp949=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp936->kind))->v)->kind;enum Cyc_Absyn_KindQual _tmp94A=_tmp949;if(_tmp94A == Cyc_Absyn_BoxKind){_LL5D7: _LL5D8:
 _tmp936->defn=Cyc_Absyn_void_star_typ();goto _LL5D6;}else{_LL5D9: _LL5DA:
 _tmp936->defn=(void*)& Cyc_Absyn_VoidType_val;goto _LL5D6;}_LL5D6:;}
# 5391
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp94B=_cycalloc(sizeof(*_tmp94B));_tmp94B->hd=d;_tmp94B->tl=Cyc_Toc_result_decls;_tmp94B;});
goto _LL5B7;case 13: _LL5CA: _LL5CB:
 goto _LL5CD;case 14: _LL5CC: _LL5CD:
# 5395
 goto _LL5B7;case 9: _LL5CE: _tmp935=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp931)->f2;_LL5CF:
 _tmp934=_tmp935;goto _LL5D1;case 10: _LL5D0: _tmp934=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp931)->f2;_LL5D1:
 _tmp933=_tmp934;goto _LL5D3;case 11: _LL5D2: _tmp933=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp931)->f1;_LL5D3:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp933,top,cinclude);goto _LL5B7;default: _LL5D4: _tmp932=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp931)->f1;_LL5D5:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp932,top,1);goto _LL5B7;}_LL5B7:;};}
# 5402
return nv;}
# 5406
static void Cyc_Toc_init(){
struct Cyc_Core_NewDynamicRegion _tmp94C=Cyc_Core_new_rckey();struct Cyc_Core_NewDynamicRegion _tmp94D=_tmp94C;struct Cyc_Core_DynamicRegion*_tmp94E;_LL5DC: _tmp94E=_tmp94D.key;_LL5DD:;{
struct Cyc_Toc_TocState*_tmp94F=((struct Cyc_Toc_TocState*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Toc_TocState*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmp94E,0,Cyc_Toc_empty_toc_state);
Cyc_Toc_toc_state=({struct Cyc_Toc_TocStateWrap*_tmp950=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp950));_tmp950->dyn=(struct Cyc_Core_DynamicRegion*)_tmp94E;_tmp950->state=(struct Cyc_Toc_TocState*)_tmp94F;_tmp950;});
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0;
Cyc_Toc_bounds_checks_eliminated=0;
Cyc_Toc_globals=_tag_dyneither(({struct _dyneither_ptr**_tmp951=_cycalloc(sizeof(struct _dyneither_ptr*)* 42);_tmp951[0]=(struct _dyneither_ptr*)& Cyc_Toc__throw_str;_tmp951[1]=(struct _dyneither_ptr*)& Cyc_Toc_setjmp_str;_tmp951[2]=(struct _dyneither_ptr*)& Cyc_Toc__push_handler_str;_tmp951[3]=(struct _dyneither_ptr*)& Cyc_Toc__pop_handler_str;_tmp951[4]=(struct _dyneither_ptr*)& Cyc_Toc__exn_thrown_str;_tmp951[5]=(struct _dyneither_ptr*)& Cyc_Toc__npop_handler_str;_tmp951[6]=(struct _dyneither_ptr*)& Cyc_Toc__check_null_str;_tmp951[7]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_null_str;_tmp951[8]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_notnull_str;_tmp951[9]=(struct _dyneither_ptr*)& Cyc_Toc__check_dyneither_subscript_str;_tmp951[10]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_str;_tmp951[11]=(struct _dyneither_ptr*)& Cyc_Toc__tag_dyneither_str;_tmp951[12]=(struct _dyneither_ptr*)& Cyc_Toc__init_dyneither_ptr_str;_tmp951[13]=(struct _dyneither_ptr*)& Cyc_Toc__untag_dyneither_ptr_str;_tmp951[14]=(struct _dyneither_ptr*)& Cyc_Toc__get_dyneither_size_str;_tmp951[15]=(struct _dyneither_ptr*)& Cyc_Toc__get_zero_arr_size_str;_tmp951[16]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_plus_str;_tmp951[17]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_plus_str;_tmp951[18]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_str;_tmp951[19]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_str;_tmp951[20]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_str;_tmp951[21]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_post_str;_tmp951[22]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_str;_tmp951[23]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_str;_tmp951[24]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_atomic_str;_tmp951[25]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_atomic_str;_tmp951[26]=(struct _dyneither_ptr*)& Cyc_Toc__region_malloc_str;_tmp951[27]=(struct _dyneither_ptr*)& Cyc_Toc__region_calloc_str;_tmp951[28]=(struct _dyneither_ptr*)& Cyc_Toc__check_times_str;_tmp951[29]=(struct _dyneither_ptr*)& Cyc_Toc__new_region_str;_tmp951[30]=(struct _dyneither_ptr*)& Cyc_Toc__push_region_str;_tmp951[31]=(struct _dyneither_ptr*)& Cyc_Toc__pop_region_str;_tmp951[32]=(struct _dyneither_ptr*)& Cyc_Toc__open_dynregion_str;_tmp951[33]=(struct _dyneither_ptr*)& Cyc_Toc__push_dynregion_str;_tmp951[34]=(struct _dyneither_ptr*)& Cyc_Toc__pop_dynregion_str;_tmp951[35]=(struct _dyneither_ptr*)& Cyc_Toc__reset_region_str;_tmp951[36]=(struct _dyneither_ptr*)& Cyc_Toc__throw_arraybounds_str;_tmp951[37]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_decrease_size_str;_tmp951[38]=(struct _dyneither_ptr*)& Cyc_Toc__throw_match_str;_tmp951[39]=(struct _dyneither_ptr*)& Cyc_Toc__swap_word_str;_tmp951[40]=(struct _dyneither_ptr*)& Cyc_Toc__swap_dyneither_str;_tmp951[41]=(struct _dyneither_ptr*)& Cyc_Toc__fast_region_malloc_str;_tmp951;}),sizeof(struct _dyneither_ptr*),42);};}
# 5463
static int Cyc_Toc_destroy_labels(struct _RegionHandle*d,struct Cyc_Toc_TocState*ts){
struct Cyc_Toc_TocState _tmp952=*ts;struct Cyc_Toc_TocState _tmp953=_tmp952;struct Cyc_Xarray_Xarray*_tmp954;_LL5DF: _tmp954=_tmp953.temp_labels;_LL5E0:;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp954);
return 0;}
# 5471
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds){
# 5473
Cyc_Toc_gpop_tables=({struct Cyc_Hashtable_Table**_tmp955=_cycalloc(sizeof(*_tmp955));_tmp955[0]=pop_tables;_tmp955;});
Cyc_Toc_init();{
struct _RegionHandle _tmp956=_new_region("start");struct _RegionHandle*start=& _tmp956;_push_region(start);
Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(start),ds,1,0);{
# 5483
struct Cyc_Toc_TocStateWrap*ts=0;
_swap_word(& ts,& Cyc_Toc_toc_state);{
struct Cyc_Toc_TocStateWrap _tmp957=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp958=_tmp957;struct Cyc_Core_DynamicRegion*_tmp959;struct Cyc_Toc_TocState*_tmp95A;_LL5E2: _tmp959=_tmp958.dyn;_tmp95A=_tmp958.state;_LL5E3:;
((int(*)(struct Cyc_Core_DynamicRegion*key,struct Cyc_Toc_TocState*arg,int(*body)(struct _RegionHandle*h,struct Cyc_Toc_TocState*arg)))Cyc_Core_open_region)(_tmp959,_tmp95A,Cyc_Toc_destroy_labels);
Cyc_Core_free_rckey(_tmp959);
((void(*)(struct _dyneither_ptr ptr))Cyc_Core_ufree)(_tag_dyneither(ts,sizeof(struct Cyc_Toc_TocStateWrap),1));
# 5490
Cyc_Toc_gpop_tables=0;
Cyc_Toc_fn_pop_table=0;{
struct Cyc_List_List*_tmp95B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0);return _tmp95B;};};};
# 5475
;_pop_region(start);};}
