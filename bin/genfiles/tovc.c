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
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167 "core.h"
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
# 72
struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 68
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 282 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_rdelete_same(struct Cyc_Dict_Dict,void*);
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};struct Cyc_Relations_Reln;
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 106
typedef struct _tuple0*Cyc_Absyn_qvar_t;typedef struct _tuple0*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_opt_t;
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{int tag;};
# 287
typedef void*Cyc_Absyn_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 386
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 391
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 449 "absyn.h"
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 462
typedef void*Cyc_Absyn_funcparams_t;
# 465
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 529
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 555
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 562
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 580
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 936 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 944
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 946
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 982
extern struct _tuple0*Cyc_Absyn_exn_name;
# 1013
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);
# 1030
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1036
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int);
# 1040
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1046
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned int);
# 1063
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1069
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1072
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1074
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1080
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1084
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1087
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1101
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
# 1103
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1114
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1129
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1204
struct _tuple0*Cyc_Absyn_binding2qvar(void*);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 70 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;
# 54 "set.h"
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
# 69
struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 141
struct Cyc_Iter_Iter Cyc_Set_make_iter(struct _RegionHandle*rgn,struct Cyc_Set_Set*s);struct Cyc_RgnOrder_RgnPO;
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
# 42 "tcutil.h"
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 71
void*Cyc_Tcutil_compress(void*t);
# 188
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct Cyc_Hashtable_Table;
# 35 "hashtable.h"
typedef struct Cyc_Hashtable_Table*Cyc_Hashtable_table_t;
# 32 "toc.h"
typedef struct Cyc_Hashtable_Table*Cyc_Toc_table_t;
# 39
struct _tuple0*Cyc_Toc_temp_var();
extern struct _dyneither_ptr Cyc_Toc_globals;
# 27 "tovc.h"
extern int Cyc_Tovc_elim_array_initializers;
struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*decls);
# 79 "tovc.cyc"
int Cyc_Tovc_elim_array_initializers=1;struct Cyc_Tovc_BoxingEnv{struct _RegionHandle*rgn;struct Cyc_List_List*all_locals;struct Cyc_Dict_Dict varmap;struct Cyc_List_List*boundvars;struct Cyc_Set_Set**freevars;};
# 92
typedef struct Cyc_Tovc_BoxingEnv Cyc_Tovc_boxingenv_t;struct Cyc_Tovc_ToExpEnv{struct _RegionHandle*rgn;struct Cyc_List_List*all_locals;struct Cyc_Dict_Dict varmap;struct Cyc_Absyn_Stmt*encloser;struct Cyc_List_List**gen_ds;};
# 101
typedef struct Cyc_Tovc_ToExpEnv Cyc_Tovc_toexpenv_t;struct _tuple10{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 107
static struct Cyc_Absyn_Exp*Cyc_Tovc_box_free_vars_exp(struct Cyc_Tovc_BoxingEnv env,struct Cyc_Absyn_Exp*e){
while(1){
void*_tmp0=e->r;void*_tmp1=_tmp0;struct Cyc_List_List*_tmp2;struct Cyc_Absyn_Exp**_tmp3;struct Cyc_Absyn_Exp**_tmp4;struct Cyc_Absyn_Exp**_tmp5;struct Cyc_Absyn_Exp**_tmp6;struct Cyc_Absyn_Exp**_tmp7;struct Cyc_Absyn_Exp**_tmp8;struct Cyc_Absyn_Exp**_tmp9;struct Cyc_Absyn_Exp**_tmpA;struct Cyc_Absyn_Exp**_tmpB;struct Cyc_Absyn_Exp**_tmpC;struct Cyc_Absyn_Exp**_tmpD;struct Cyc_Absyn_Exp**_tmpE;struct Cyc_Absyn_Exp**_tmpF;struct Cyc_Absyn_Exp**_tmp10;struct Cyc_Absyn_Exp**_tmp11;struct Cyc_Absyn_Exp**_tmp12;struct Cyc_List_List*_tmp13;struct Cyc_Absyn_Exp**_tmp14;struct Cyc_List_List*_tmp15;struct Cyc_Absyn_Exp*_tmp16;struct _dyneither_ptr*_tmp17;int _tmp18;int _tmp19;struct Cyc_Absyn_Exp*_tmp1A;struct _dyneither_ptr*_tmp1B;int _tmp1C;int _tmp1D;struct Cyc_Absyn_Exp*_tmp1E;struct Cyc_Absyn_Exp*_tmp1F;struct Cyc_Absyn_Exp*_tmp20;struct Cyc_Absyn_Exp*_tmp21;void*_tmp22;switch(*((int*)_tmp1)){case 1: _LL1: _tmp22=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL2: {
# 111
struct _tuple0*_tmp23=Cyc_Absyn_binding2qvar(_tmp22);struct _tuple0*_tmp24=_tmp23;struct _dyneither_ptr*_tmp25;if(((((struct _tuple0*)_tmp24)->f1).Loc_n).tag == 4){_LL32: _tmp25=_tmp24->f2;_LL33:
# 115
 if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(env.varmap,_tmp25)){
e->r=(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(env.varmap,_tmp25))->r;
continue;}
# 120
{int i=0;for(0;i < _get_dyneither_size(Cyc_Toc_globals,sizeof(struct _dyneither_ptr*));++ i){
if(Cyc_strptrcmp(_tmp25,*((struct _dyneither_ptr**)_check_dyneither_subscript(Cyc_Toc_globals,sizeof(struct _dyneither_ptr*),i)))== 0)
return 0;}}
# 124
for(0;env.boundvars != 0;env.boundvars=(env.boundvars)->tl){
if(Cyc_strptrcmp(_tmp25,(*((struct Cyc_Absyn_Vardecl*)(env.boundvars)->hd)->name).f2)== 0)
return 0;}
# 128
for(0;env.all_locals != 0;env.all_locals=(env.all_locals)->tl){
struct Cyc_Absyn_Vardecl*_tmp26=(struct Cyc_Absyn_Vardecl*)(env.all_locals)->hd;
if(Cyc_strptrcmp(_tmp25,(*_tmp26->name).f2)== 0){
*env.freevars=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct Cyc_Absyn_Vardecl*elt))Cyc_Set_rinsert)(env.rgn,*env.freevars,_tmp26);{
# 133
void*_tmp27=_tmp26->type;void*_tmp28=_tmp27;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp28)->tag == 8){_LL37: _LL38:
 return 0;}else{_LL39: _LL3A:
 return Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp26->name,0),0);}_LL36:;};}}
# 142
for(0;env.all_locals != 0;env.all_locals=((struct Cyc_List_List*)_check_null(env.all_locals))->tl){
if(Cyc_strptrcmp(_tmp25,(*((struct _tuple0*)_check_null(((struct Cyc_Absyn_Vardecl*)_check_null((struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(env.all_locals))->hd))->name))).f2)== 0)
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp29=_cycalloc(sizeof(*_tmp29));_tmp29[0]=({struct Cyc_Core_Impossible_exn_struct _tmp2A;_tmp2A.tag=Cyc_Core_Impossible;_tmp2A.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2D;_tmp2D.tag=0;_tmp2D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(_tmp25)));({void*_tmp2B[1]={& _tmp2D};Cyc_aprintf(({const char*_tmp2C="unbound variable %s found in box_free_vars_exp";_tag_dyneither(_tmp2C,sizeof(char),47);}),_tag_dyneither(_tmp2B,sizeof(void*),1));});});_tmp2A;});_tmp29;}));}
# 149
return 0;}else{_LL34: _LL35:
 return 0;}_LL31:;}case 22: _LL3: _tmp20=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp21=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL4: {
# 156
struct Cyc_Absyn_Exp*_tmp2E=Cyc_Tovc_box_free_vars_exp(env,_tmp20);
struct Cyc_Absyn_Exp*_tmp2F=Cyc_Tovc_box_free_vars_exp(env,_tmp21);
if(_tmp2E != 0){
if(_tmp2F != 0){
struct Cyc_Absyn_Exp*_tmp30=Cyc_Absyn_subscript_exp(_tmp2E,_tmp2F,e->loc);
_tmp30->topt=e->topt;
_tmp30->annot=e->annot;
return _tmp30;}else{
# 166
struct Cyc_Absyn_Exp*_tmp31=Cyc_Absyn_subscript_exp(_tmp2E,_tmp21,e->loc);
_tmp31->topt=e->topt;
_tmp31->annot=e->annot;
return _tmp31;}}else{
# 172
if(_tmp2F != 0){
struct Cyc_Absyn_Exp*_tmp32=Cyc_Absyn_subscript_exp(_tmp20,_tmp2F,e->loc);
_tmp32->topt=e->topt;
_tmp32->annot=e->annot;
return _tmp32;}}
# 178
return 0;}case 14: _LL5: _tmp1F=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL6: {
# 181
struct Cyc_Absyn_Exp*_tmp33=Cyc_Tovc_box_free_vars_exp(env,_tmp1F);
if(_tmp33 != 0){
struct Cyc_Absyn_Exp*_tmp34=Cyc_Absyn_address_exp(_tmp33,e->loc);
_tmp34->topt=e->topt;
_tmp34->annot=e->annot;
return _tmp34;}
# 188
return 0;}case 19: _LL7: _tmp1E=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL8: {
# 191
struct Cyc_Absyn_Exp*_tmp35=Cyc_Tovc_box_free_vars_exp(env,_tmp1E);
if(_tmp35 != 0){
struct Cyc_Absyn_Exp*_tmp36=Cyc_Absyn_deref_exp(_tmp35,e->loc);
_tmp36->topt=e->topt;
_tmp36->annot=e->annot;
return _tmp36;}
# 198
return 0;}case 20: _LL9: _tmp1A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp1B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_tmp1C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1)->f3;_tmp1D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1)->f4;_LLA: {
# 201
struct Cyc_Absyn_Exp*_tmp37=Cyc_Tovc_box_free_vars_exp(env,_tmp1A);
if(_tmp37 != 0){
struct Cyc_Absyn_Exp*_tmp38=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39[0]=({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmp3A;_tmp3A.tag=20;_tmp3A.f1=_tmp37;_tmp3A.f2=_tmp1B;_tmp3A.f3=_tmp1C;_tmp3A.f4=_tmp1D;_tmp3A;});_tmp39;}),e->loc);
_tmp38->topt=e->topt;
_tmp38->annot=e->annot;
return _tmp38;}
# 208
return 0;}case 21: _LLB: _tmp16=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp17=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_tmp18=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1)->f3;_tmp19=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1)->f4;_LLC: {
# 211
struct Cyc_Absyn_Exp*_tmp3B=Cyc_Tovc_box_free_vars_exp(env,_tmp16);
if(_tmp3B != 0){
struct Cyc_Absyn_Exp*_tmp3C=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp3D=_cycalloc(sizeof(*_tmp3D));_tmp3D[0]=({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmp3E;_tmp3E.tag=21;_tmp3E.f1=_tmp3B;_tmp3E.f2=_tmp17;_tmp3E.f3=_tmp18;_tmp3E.f4=_tmp19;_tmp3E;});_tmp3D;}),e->loc);
_tmp3C->topt=e->topt;
_tmp3C->annot=e->annot;
return _tmp3C;}
# 218
return 0;}case 0: _LLD: _LLE:
# 221
 return 0;case 9: _LLF: _tmp14=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp15=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL10:
# 223
{struct Cyc_Absyn_Exp*_tmp3F=Cyc_Tovc_box_free_vars_exp(env,*_tmp14);if(_tmp3F != 0)*_tmp14=_tmp3F;}
_tmp13=_tmp15;goto _LL12;case 2: _LL11: _tmp13=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL12:
# 226
 for(0;_tmp13 != 0;_tmp13=_tmp13->tl){
struct Cyc_Absyn_Exp*_tmp40=Cyc_Tovc_box_free_vars_exp(env,(struct Cyc_Absyn_Exp*)_tmp13->hd);if(_tmp40 != 0)_tmp13->hd=(void*)_tmp40;}
# 229
return 0;case 5: _LL13: _tmp10=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp11=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_tmp12=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp1)->f3;_LL14:
# 232
{struct Cyc_Absyn_Exp*_tmp41=Cyc_Tovc_box_free_vars_exp(env,*_tmp12);if(_tmp41 != 0)*_tmp12=_tmp41;}
_tmpE=_tmp10;_tmpF=_tmp11;goto _LL16;case 6: _LL15: _tmpE=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmpF=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL16:
 _tmpC=_tmpE;_tmpD=_tmpF;goto _LL18;case 7: _LL17: _tmpC=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmpD=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL18:
 _tmpA=_tmpC;_tmpB=_tmpD;goto _LL1A;case 8: _LL19: _tmpA=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmpB=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL1A:
 _tmp8=_tmpA;_tmp9=_tmpB;goto _LL1C;case 3: _LL1B: _tmp8=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp9=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp1)->f3;_LL1C:
# 238
{struct Cyc_Absyn_Exp*_tmp42=Cyc_Tovc_box_free_vars_exp(env,*_tmp9);if(_tmp42 != 0)*_tmp9=_tmp42;}
_tmp7=_tmp8;goto _LL1E;case 11: _LL1D: _tmp7=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL1E:
# 241
 _tmp6=_tmp7;goto _LL20;case 12: _LL1F: _tmp6=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL20:
 _tmp5=_tmp6;goto _LL22;case 13: _LL21: _tmp5=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL22:
 _tmp4=_tmp5;goto _LL24;case 17: _LL23: _tmp4=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL24:
 _tmp3=_tmp4;goto _LL26;case 4: _LL25: _tmp3=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL26:
# 246
{struct Cyc_Absyn_Exp*_tmp43=Cyc_Tovc_box_free_vars_exp(env,*_tmp3);if(_tmp43 != 0)*_tmp3=_tmp43;}
return 0;case 16: _LL27: _LL28:
# 249
 goto _LL2A;case 18: _LL29: _LL2A:
 goto _LL2C;case 31: _LL2B: _LL2C:
 return 0;case 35: _LL2D: _tmp2=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL2E:
# 254
 for(0;_tmp2 != 0;_tmp2=_tmp2->tl){
struct Cyc_Absyn_Exp*_tmp44=Cyc_Tovc_box_free_vars_exp(env,(*((struct _tuple10*)_tmp2->hd)).f2);if(_tmp44 != 0)(*((struct _tuple10*)_tmp2->hd)).f2=_tmp44;}
# 257
return 0;default: _LL2F: _LL30:
# 259
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45[0]=({struct Cyc_Core_Impossible_exn_struct _tmp46;_tmp46.tag=Cyc_Core_Impossible;_tmp46.f1=({const char*_tmp47="bad exp form in Tocv::box_free_vars_exp";_tag_dyneither(_tmp47,sizeof(char),40);});_tmp46;});_tmp45;}));}_LL0:;}}
# 263
static void Cyc_Tovc_box_free_vars_stmt(struct Cyc_Tovc_BoxingEnv env,struct Cyc_Absyn_Stmt*s){
# 267
while(1){
void*_tmp48=s->r;void*_tmp49=_tmp48;struct Cyc_Absyn_Decl*_tmp4A;struct Cyc_Absyn_Stmt*_tmp4B;struct Cyc_Absyn_Exp**_tmp4C;struct Cyc_List_List*_tmp4D;struct Cyc_Absyn_Stmt*_tmp4E;struct Cyc_Absyn_Exp**_tmp4F;struct Cyc_Absyn_Exp**_tmp50;struct Cyc_Absyn_Exp**_tmp51;struct Cyc_Absyn_Exp**_tmp52;struct Cyc_Absyn_Stmt*_tmp53;struct Cyc_Absyn_Exp**_tmp54;struct Cyc_Absyn_Stmt*_tmp55;struct Cyc_Absyn_Exp**_tmp56;struct Cyc_Absyn_Stmt*_tmp57;struct Cyc_Absyn_Stmt*_tmp58;struct Cyc_Absyn_Stmt*_tmp59;struct Cyc_Absyn_Stmt*_tmp5A;struct Cyc_Absyn_Exp**_tmp5B;struct Cyc_Absyn_Exp**_tmp5C;struct Cyc_Absyn_Stmt*_tmp5D;switch(*((int*)_tmp49)){case 0: _LL3C: _LL3D:
 return;case 6: _LL3E: _LL3F:
# 271
({void*_tmp5E=0;Cyc_Tcutil_warn(s->loc,({const char*_tmp5F="break in eliminated statement expression";_tag_dyneither(_tmp5F,sizeof(char),41);}),_tag_dyneither(_tmp5E,sizeof(void*),0));});
return;case 7: _LL40: _LL41:
# 274
({void*_tmp60=0;Cyc_Tcutil_warn(s->loc,({const char*_tmp61="continue in eliminated statement expression";_tag_dyneither(_tmp61,sizeof(char),44);}),_tag_dyneither(_tmp60,sizeof(void*),0));});
# 277
return;case 8: _LL42: _LL43:
# 279
({void*_tmp62=0;Cyc_Tcutil_warn(s->loc,({const char*_tmp63="goto in eliminated statement expression";_tag_dyneither(_tmp63,sizeof(char),40);}),_tag_dyneither(_tmp62,sizeof(void*),0));});
return;case 13: _LL44: _tmp5D=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp49)->f2;_LL45:
# 282
 s=_tmp5D;
continue;case 3: _LL46: _tmp5C=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp49)->f1;_LL47:
# 285
({void*_tmp64=0;Cyc_Tcutil_warn(s->loc,({const char*_tmp65="return in eliminated statement expression";_tag_dyneither(_tmp65,sizeof(char),42);}),_tag_dyneither(_tmp64,sizeof(void*),0));});
# 287
if(*_tmp5C != 0){
struct Cyc_Absyn_Exp*_tmp66=Cyc_Tovc_box_free_vars_exp(env,(struct Cyc_Absyn_Exp*)_check_null(*_tmp5C));
if(_tmp66 != 0)*_tmp5C=_tmp66;}
# 291
return;case 1: _LL48: _tmp5B=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp49)->f1;_LL49:
# 293
{struct Cyc_Absyn_Exp*_tmp67=Cyc_Tovc_box_free_vars_exp(env,*_tmp5B);if(_tmp67 != 0)*_tmp5B=_tmp67;}
return;case 2: _LL4A: _tmp59=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp49)->f1;_tmp5A=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp49)->f2;_LL4B:
# 296
 Cyc_Tovc_box_free_vars_stmt(env,_tmp59);
s=_tmp5A;
continue;case 4: _LL4C: _tmp56=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp49)->f1;_tmp57=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp49)->f2;_tmp58=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp49)->f3;_LL4D:
# 300
{struct Cyc_Absyn_Exp*_tmp68=Cyc_Tovc_box_free_vars_exp(env,*_tmp56);if(_tmp68 != 0)*_tmp56=_tmp68;}
Cyc_Tovc_box_free_vars_stmt(env,_tmp57);
s=_tmp58;
continue;case 5: _LL4E: _tmp54=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp49)->f1).f1;_tmp55=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp49)->f2;_LL4F:
# 305
{struct Cyc_Absyn_Exp*_tmp69=Cyc_Tovc_box_free_vars_exp(env,*_tmp54);if(_tmp69 != 0)*_tmp54=_tmp69;}
s=_tmp55;
continue;case 9: _LL50: _tmp50=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp49)->f1;_tmp51=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp49)->f2).f1;_tmp52=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp49)->f3).f1;_tmp53=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp49)->f4;_LL51:
# 309
{struct Cyc_Absyn_Exp*_tmp6A=Cyc_Tovc_box_free_vars_exp(env,*_tmp50);if(_tmp6A != 0)*_tmp50=_tmp6A;}
{struct Cyc_Absyn_Exp*_tmp6B=Cyc_Tovc_box_free_vars_exp(env,*_tmp51);if(_tmp6B != 0)*_tmp51=_tmp6B;}
{struct Cyc_Absyn_Exp*_tmp6C=Cyc_Tovc_box_free_vars_exp(env,*_tmp52);if(_tmp6C != 0)*_tmp52=_tmp6C;}
s=_tmp53;
continue;case 14: _LL52: _tmp4E=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp49)->f1;_tmp4F=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp49)->f2).f1;_LL53:
# 315
{struct Cyc_Absyn_Exp*_tmp6D=Cyc_Tovc_box_free_vars_exp(env,*_tmp4F);if(_tmp6D != 0)*_tmp4F=_tmp6D;}
s=_tmp4E;
continue;case 10: _LL54: _tmp4C=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp49)->f1;_tmp4D=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp49)->f2;_LL55:
# 320
{struct Cyc_Absyn_Exp*_tmp6E=Cyc_Tovc_box_free_vars_exp(env,*_tmp4C);if(_tmp6E != 0)*_tmp4C=_tmp6E;}
for(0;_tmp4D != 0;_tmp4D=_tmp4D->tl){
Cyc_Tovc_box_free_vars_stmt(env,((struct Cyc_Absyn_Switch_clause*)_tmp4D->hd)->body);}
return;case 12: _LL56: _tmp4A=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp49)->f1;_tmp4B=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp49)->f2;_LL57:
# 325
{void*_tmp6F=_tmp4A->r;void*_tmp70=_tmp6F;struct Cyc_Absyn_Vardecl*_tmp71;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp70)->tag == 0){_LL5B: _tmp71=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp70)->f1;_LL5C:
# 327
 env.boundvars=({struct Cyc_List_List*_tmp72=_region_malloc(env.rgn,sizeof(*_tmp72));_tmp72->hd=_tmp71;_tmp72->tl=env.boundvars;_tmp72;});
env.varmap=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _dyneither_ptr*))Cyc_Dict_rdelete_same)(env.varmap,(*_tmp71->name).f2);
if(_tmp71->initializer != 0){
struct Cyc_Absyn_Exp*_tmp73=Cyc_Tovc_box_free_vars_exp(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp71->initializer));
if(_tmp73 != 0)_tmp71->initializer=_tmp73;}
# 333
s=_tmp4B;
continue;}else{_LL5D: _LL5E:
 goto _LL5A;}_LL5A:;}
# 337
goto _LL59;default: _LL58: _LL59:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74[0]=({struct Cyc_Core_Impossible_exn_struct _tmp75;_tmp75.tag=Cyc_Core_Impossible;_tmp75.f1=({const char*_tmp76="bad stmt after xlation to C";_tag_dyneither(_tmp76,sizeof(char),28);});_tmp75;});_tmp74;}));}_LL3B:;}}
# 348
static int Cyc_Tovc_stmt_to_fun_cmp(struct Cyc_Absyn_Vardecl*x,struct Cyc_Absyn_Vardecl*y){
return Cyc_Absyn_qvar_cmp(x->name,y->name);}
# 352
static struct Cyc_Absyn_Exp*Cyc_Tovc_stmt_to_fun(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s,void*rettype){
# 355
struct Cyc_Set_Set*freevars=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*)))Cyc_Set_rempty)(env.rgn,Cyc_Tovc_stmt_to_fun_cmp);
struct Cyc_Tovc_BoxingEnv _tmp77=({struct Cyc_Tovc_BoxingEnv _tmp87;_tmp87.rgn=env.rgn;_tmp87.all_locals=env.all_locals;_tmp87.varmap=env.varmap;_tmp87.boundvars=0;_tmp87.freevars=& freevars;_tmp87;});
Cyc_Tovc_box_free_vars_stmt(_tmp77,s);{
struct Cyc_List_List*params=0;
struct Cyc_List_List*args=0;
# 361
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(Cyc_Core_heap_region,freevars);
# 363
struct Cyc_Absyn_Vardecl*_tmp78=({struct Cyc_Absyn_Vardecl*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86->sc=Cyc_Absyn_Public;_tmp86->name=Cyc_Absyn_exn_name;_tmp86->tq=Cyc_Absyn_empty_tqual(0);_tmp86->type=(void*)& Cyc_Absyn_VoidType_val;_tmp86->initializer=0;_tmp86->rgn=0;_tmp86->attributes=0;_tmp86->escapes=0;_tmp86;});
# 366
while(((int(*)(struct Cyc_Iter_Iter,struct Cyc_Absyn_Vardecl**))Cyc_Iter_next)(iter,& _tmp78)){
void*_tmp79=_tmp78->type;void*_tmp7A=_tmp79;void*_tmp7B;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp7A)->tag == 8){_LL60: _tmp7B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp7A)->f1).elt_type;_LL61:
# 369
 params=({struct Cyc_List_List*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->hd=({struct _tuple8*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->f1=(*_tmp78->name).f2;_tmp7D->f2=_tmp78->tq;_tmp7D->f3=
Cyc_Absyn_cstar_typ(_tmp7B,Cyc_Absyn_empty_tqual(0));_tmp7D;});_tmp7C->tl=params;_tmp7C;});
# 372
args=({struct Cyc_List_List*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->hd=Cyc_Absyn_var_exp(_tmp78->name,0);_tmp7E->tl=args;_tmp7E;});
goto _LL5F;}else{_LL62: _LL63:
# 375
 params=({struct Cyc_List_List*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->hd=({struct _tuple8*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->f1=(*_tmp78->name).f2;_tmp80->f2=_tmp78->tq;_tmp80->f3=
Cyc_Absyn_cstar_typ(_tmp78->type,Cyc_Absyn_empty_tqual(0));_tmp80;});_tmp7F->tl=params;_tmp7F;});
# 378
args=({struct Cyc_List_List*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->hd=Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(_tmp78->name,0),0);_tmp81->tl=args;_tmp81;});
goto _LL5F;}_LL5F:;}{
# 381
struct _tuple0*funname=Cyc_Toc_temp_var();
*env.gen_ds=({struct Cyc_List_List*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83[0]=({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmp84;_tmp84.tag=1;_tmp84.f1=({struct Cyc_Absyn_Fndecl*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->sc=Cyc_Absyn_Static;_tmp85->is_inline=0;_tmp85->name=funname;_tmp85->tvs=0;_tmp85->effect=0;_tmp85->ret_tqual=
Cyc_Absyn_empty_tqual(0);_tmp85->ret_type=rettype;_tmp85->args=params;_tmp85->c_varargs=0;_tmp85->cyc_varargs=0;_tmp85->rgn_po=0;_tmp85->body=s;_tmp85->cached_typ=0;_tmp85->param_vardecls=0;_tmp85->fn_vardecl=0;_tmp85->attributes=0;_tmp85->requires_clause=0;_tmp85->requires_relns=0;_tmp85->ensures_clause=0;_tmp85->ensures_relns=0;_tmp85;});_tmp84;});_tmp83;}),0);_tmp82->tl=*env.gen_ds;_tmp82;});
# 388
return Cyc_Absyn_fncall_exp(Cyc_Absyn_var_exp(funname,0),args,0);};};}
# 391
static void Cyc_Tovc_apply_varmap(struct Cyc_Dict_Dict varmap,struct Cyc_Absyn_Exp*e){
void*_tmp88=e->r;void*_tmp89=_tmp88;struct Cyc_List_List*_tmp8A;struct Cyc_Absyn_Exp*_tmp8B;struct Cyc_Absyn_Exp*_tmp8C;struct Cyc_Absyn_Exp*_tmp8D;struct Cyc_Absyn_Exp*_tmp8E;struct Cyc_Absyn_Exp*_tmp8F;struct Cyc_Absyn_Exp*_tmp90;struct Cyc_Absyn_Exp*_tmp91;struct Cyc_Absyn_Exp*_tmp92;struct Cyc_Absyn_Exp*_tmp93;struct Cyc_Absyn_Exp*_tmp94;struct Cyc_Absyn_Exp*_tmp95;struct Cyc_Absyn_Exp*_tmp96;struct Cyc_Absyn_Exp*_tmp97;struct Cyc_Absyn_Exp*_tmp98;struct Cyc_Absyn_Exp*_tmp99;struct Cyc_Absyn_Exp*_tmp9A;struct Cyc_Absyn_Exp*_tmp9B;struct Cyc_Absyn_Exp*_tmp9C;struct Cyc_Absyn_Exp*_tmp9D;struct Cyc_Absyn_Exp*_tmp9E;struct Cyc_Absyn_Exp*_tmp9F;struct Cyc_Absyn_Exp*_tmpA0;struct Cyc_List_List*_tmpA1;struct Cyc_Absyn_Exp*_tmpA2;struct Cyc_List_List*_tmpA3;void*_tmpA4;switch(*((int*)_tmp89)){case 1: _LL65: _tmpA4=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp89)->f1;_LL66: {
# 394
struct _tuple0*_tmpA5=Cyc_Absyn_binding2qvar(_tmpA4);struct _tuple0*_tmpA6=_tmpA5;struct _dyneither_ptr*_tmpA7;if(((((struct _tuple0*)_tmpA6)->f1).Loc_n).tag == 4){_LL96: _tmpA7=_tmpA6->f2;_LL97:
# 396
 if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(varmap,_tmpA7))
e->r=(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(varmap,_tmpA7))->r;
return;}else{_LL98: _LL99:
 return;}_LL95:;}case 0: _LL67: _LL68:
# 403
 return;case 9: _LL69: _tmpA2=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp89)->f1;_tmpA3=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp89)->f2;_LL6A:
# 405
 Cyc_Tovc_apply_varmap(varmap,_tmpA2);_tmpA1=_tmpA3;goto _LL6C;case 2: _LL6B: _tmpA1=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp89)->f2;_LL6C:
# 407
 for(0;_tmpA1 != 0;_tmpA1=_tmpA1->tl){
Cyc_Tovc_apply_varmap(varmap,(struct Cyc_Absyn_Exp*)_tmpA1->hd);}
return;case 5: _LL6D: _tmp9E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp89)->f1;_tmp9F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp89)->f2;_tmpA0=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp89)->f3;_LL6E:
# 412
 Cyc_Tovc_apply_varmap(varmap,_tmpA0);_tmp9C=_tmp9E;_tmp9D=_tmp9F;goto _LL70;case 6: _LL6F: _tmp9C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp89)->f1;_tmp9D=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp89)->f2;_LL70:
 _tmp9A=_tmp9C;_tmp9B=_tmp9D;goto _LL72;case 7: _LL71: _tmp9A=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp89)->f1;_tmp9B=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp89)->f2;_LL72:
 _tmp98=_tmp9A;_tmp99=_tmp9B;goto _LL74;case 22: _LL73: _tmp98=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp89)->f1;_tmp99=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp89)->f2;_LL74:
 _tmp96=_tmp98;_tmp97=_tmp99;goto _LL76;case 8: _LL75: _tmp96=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp89)->f1;_tmp97=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp89)->f2;_LL76:
 _tmp94=_tmp96;_tmp95=_tmp97;goto _LL78;case 3: _LL77: _tmp94=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp89)->f1;_tmp95=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp89)->f3;_LL78:
# 418
 Cyc_Tovc_apply_varmap(varmap,_tmp95);_tmp93=_tmp94;goto _LL7A;case 11: _LL79: _tmp93=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp89)->f1;_LL7A:
# 420
 _tmp92=_tmp93;goto _LL7C;case 12: _LL7B: _tmp92=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp89)->f1;_LL7C:
 _tmp91=_tmp92;goto _LL7E;case 13: _LL7D: _tmp91=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp89)->f2;_LL7E:
 _tmp90=_tmp91;goto _LL80;case 14: _LL7F: _tmp90=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp89)->f1;_LL80:
 _tmp8F=_tmp90;goto _LL82;case 17: _LL81: _tmp8F=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp89)->f1;_LL82:
 _tmp8E=_tmp8F;goto _LL84;case 19: _LL83: _tmp8E=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp89)->f1;_LL84:
 _tmp8D=_tmp8E;goto _LL86;case 20: _LL85: _tmp8D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp89)->f1;_LL86:
 _tmp8C=_tmp8D;goto _LL88;case 21: _LL87: _tmp8C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp89)->f1;_LL88:
 _tmp8B=_tmp8C;goto _LL8A;case 4: _LL89: _tmp8B=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp89)->f1;_LL8A:
# 429
 Cyc_Tovc_apply_varmap(varmap,_tmp8B);
return;case 16: _LL8B: _LL8C:
# 432
 goto _LL8E;case 18: _LL8D: _LL8E:
 goto _LL90;case 31: _LL8F: _LL90:
 return;case 35: _LL91: _tmp8A=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp89)->f2;_LL92:
# 437
 for(0;_tmp8A != 0;_tmp8A=_tmp8A->tl){
Cyc_Tovc_apply_varmap(varmap,(*((struct _tuple10*)_tmp8A->hd)).f2);}
return;default: _LL93: _LL94:
# 441
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8[0]=({struct Cyc_Core_Impossible_exn_struct _tmpA9;_tmpA9.tag=Cyc_Core_Impossible;_tmpA9.f1=({const char*_tmpAA="bad exp form in Tocv::apply_varmap";_tag_dyneither(_tmpAA,sizeof(char),35);});_tmpA9;});_tmpA8;}));}_LL64:;}
# 445
static struct Cyc_Absyn_Exp*Cyc_Tovc_stmt_to_exp(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s){
void*_tmpAB=s->r;void*_tmpAC=_tmpAB;struct Cyc_Absyn_Decl*_tmpAD;struct Cyc_Absyn_Stmt*_tmpAE;struct Cyc_Absyn_Exp*_tmpAF;struct Cyc_Absyn_Stmt*_tmpB0;struct Cyc_Absyn_Stmt*_tmpB1;struct Cyc_Absyn_Stmt*_tmpB2;struct Cyc_Absyn_Stmt*_tmpB3;struct Cyc_Absyn_Exp*_tmpB4;switch(*((int*)_tmpAC)){case 0: _LL9B: _LL9C:
 return Cyc_Absyn_true_exp(0);case 1: _LL9D: _tmpB4=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpAC)->f1;_LL9E:
# 449
 Cyc_Tovc_apply_varmap(env.varmap,_tmpB4);
return _tmpB4;case 2: _LL9F: _tmpB2=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpAC)->f1;_tmpB3=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpAC)->f2;_LLA0: {
# 453
struct Cyc_Absyn_Exp*_tmpB5=Cyc_Tovc_stmt_to_exp(env,_tmpB2);
struct Cyc_Absyn_Exp*_tmpB6=Cyc_Tovc_stmt_to_exp(env,_tmpB3);
return Cyc_Absyn_seq_exp(_tmpB5,_tmpB6,0);}case 4: _LLA1: _tmpAF=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmpAC)->f1;_tmpB0=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmpAC)->f2;_tmpB1=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmpAC)->f3;_LLA2:
# 457
 Cyc_Tovc_apply_varmap(env.varmap,_tmpAF);{
# 459
struct Cyc_Absyn_Exp*_tmpB7=Cyc_Tovc_stmt_to_exp(env,_tmpB0);
struct Cyc_Absyn_Exp*_tmpB8=Cyc_Tovc_stmt_to_exp(env,_tmpB1);
return Cyc_Absyn_conditional_exp(_tmpAF,_tmpB7,_tmpB8,0);};case 12: _LLA3: _tmpAD=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpAC)->f1;_tmpAE=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpAC)->f2;_LLA4: {
# 463
void*_tmpB9=_tmpAD->r;void*_tmpBA=_tmpB9;struct Cyc_Absyn_Vardecl*_tmpBB;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpBA)->tag == 0){_LLA8: _tmpBB=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpBA)->f1;_LLA9: {
# 466
struct _dyneither_ptr*_tmpBC=(*_tmpBB->name).f2;
struct _tuple0*_tmpBD=Cyc_Toc_temp_var();
_tmpBB->name=_tmpBD;
env.varmap=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(env.varmap,_tmpBC,Cyc_Absyn_var_exp(_tmpBD,0));{
struct Cyc_List_List*_tmpBE=0;
if(_tmpBB->initializer != 0){
Cyc_Tovc_apply_varmap(env.varmap,(struct Cyc_Absyn_Exp*)_check_null(_tmpBB->initializer));{
void*_tmpBF=((struct Cyc_Absyn_Exp*)_check_null(_tmpBB->initializer))->r;void*_tmpC0=_tmpBF;struct Cyc_List_List*_tmpC1;if(((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpC0)->tag == 35){_LLAD: _tmpC1=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpC0)->f2;_LLAE: {
# 485 "tovc.cyc"
struct Cyc_Absyn_Exp*_tmpC2=Cyc_Absyn_var_exp(_tmpBB->name,0);
{unsigned int i=0;for(0;_tmpC1 != 0;(_tmpC1=_tmpC1->tl,++ i)){
_tmpBE=({struct Cyc_List_List*_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3->hd=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(_tmpC2,Cyc_Absyn_uint_exp(i,0),0),(*((struct _tuple10*)_tmpC1->hd)).f2,0);_tmpC3->tl=_tmpBE;_tmpC3;});}}
# 491
_tmpBB->initializer=0;
goto _LLAC;}}else{_LLAF: _LLB0:
 goto _LLAC;}_LLAC:;};}
# 496
env.all_locals=({struct Cyc_List_List*_tmpC4=_region_malloc(env.rgn,sizeof(*_tmpC4));_tmpC4->hd=_tmpBB;_tmpC4->tl=env.all_locals;_tmpC4;});
# 498
(env.encloser)->r=(Cyc_Absyn_decl_stmt(_tmpAD,Cyc_Absyn_new_stmt((env.encloser)->r,0),0))->r;{
struct Cyc_Absyn_Exp*_tmpC5=Cyc_Tovc_stmt_to_exp(env,_tmpAE);
if(_tmpBB->initializer != 0){
_tmpC5=Cyc_Absyn_seq_exp(Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmpBB->name,0),(struct Cyc_Absyn_Exp*)_check_null(_tmpBB->initializer),0),_tmpC5,0);
# 503
_tmpBB->initializer=0;}
# 505
for(0;_tmpBE != 0;_tmpBE=_tmpBE->tl){
_tmpC5=Cyc_Absyn_seq_exp((struct Cyc_Absyn_Exp*)_tmpBE->hd,_tmpC5,0);}
return _tmpC5;};};}}else{_LLAA: _LLAB:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6[0]=({struct Cyc_Core_Impossible_exn_struct _tmpC7;_tmpC7.tag=Cyc_Core_Impossible;_tmpC7.f1=({const char*_tmpC8="bad local decl in Tovc::stmt_to_exp";_tag_dyneither(_tmpC8,sizeof(char),36);});_tmpC7;});_tmpC6;}));}_LLA7:;}default: _LLA5: _LLA6:
# 511
 return Cyc_Tovc_stmt_to_fun(env,s,(void*)& Cyc_Absyn_VoidType_val);}_LL9A:;}
# 515
static void Cyc_Tovc_stmt_to_vc(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s);
# 517
static void Cyc_Tovc_exp_to_vc(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Exp*e){
struct _dyneither_ptr bad_form=({const char*_tmpFA="";_tag_dyneither(_tmpFA,sizeof(char),1);});
{void*_tmpC9=e->r;void*_tmpCA=_tmpC9;struct Cyc_Absyn_Stmt*_tmpCB;struct Cyc_List_List*_tmpCC;struct Cyc_Absyn_Exp*_tmpCD;struct Cyc_Absyn_Exp*_tmpCE;struct Cyc_Absyn_Exp*_tmpCF;struct Cyc_Absyn_Exp*_tmpD0;struct Cyc_Absyn_Exp*_tmpD1;struct Cyc_Absyn_Exp*_tmpD2;struct Cyc_Absyn_Exp*_tmpD3;struct Cyc_Absyn_Exp*_tmpD4;struct Cyc_Absyn_Exp*_tmpD5;struct Cyc_Absyn_Exp*_tmpD6;struct Cyc_Absyn_Exp*_tmpD7;struct Cyc_Absyn_Exp*_tmpD8;struct Cyc_Absyn_Exp*_tmpD9;struct Cyc_Absyn_Exp*_tmpDA;struct Cyc_Absyn_Exp*_tmpDB;struct Cyc_Absyn_Exp*_tmpDC;struct Cyc_Absyn_Exp*_tmpDD;struct Cyc_Absyn_Exp*_tmpDE;struct Cyc_Absyn_Exp*_tmpDF;struct Cyc_Absyn_Exp*_tmpE0;struct Cyc_Absyn_Exp*_tmpE1;struct Cyc_Absyn_Exp*_tmpE2;struct Cyc_List_List*_tmpE3;struct Cyc_Absyn_Exp*_tmpE4;struct Cyc_List_List*_tmpE5;switch(*((int*)_tmpCA)){case 0: _LLB2: _LLB3:
 return;case 1: _LLB4: _LLB5:
 return;case 9: _LLB6: _tmpE4=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpCA)->f1;_tmpE5=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpCA)->f2;_LLB7:
# 524
 Cyc_Tovc_exp_to_vc(env,_tmpE4);_tmpE3=_tmpE5;goto _LLB9;case 2: _LLB8: _tmpE3=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpCA)->f2;_LLB9:
# 526
 for(0;_tmpE3 != 0;_tmpE3=_tmpE3->tl){
Cyc_Tovc_exp_to_vc(env,(struct Cyc_Absyn_Exp*)_tmpE3->hd);}
return;case 5: _LLBA: _tmpE0=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpCA)->f1;_tmpE1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpCA)->f2;_tmpE2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpCA)->f3;_LLBB:
# 531
 Cyc_Tovc_exp_to_vc(env,_tmpE2);_tmpDE=_tmpE0;_tmpDF=_tmpE1;goto _LLBD;case 6: _LLBC: _tmpDE=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpCA)->f1;_tmpDF=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpCA)->f2;_LLBD:
 _tmpDC=_tmpDE;_tmpDD=_tmpDF;goto _LLBF;case 7: _LLBE: _tmpDC=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpCA)->f1;_tmpDD=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpCA)->f2;_LLBF:
 _tmpDA=_tmpDC;_tmpDB=_tmpDD;goto _LLC1;case 22: _LLC0: _tmpDA=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpCA)->f1;_tmpDB=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpCA)->f2;_LLC1:
 _tmpD8=_tmpDA;_tmpD9=_tmpDB;goto _LLC3;case 8: _LLC2: _tmpD8=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpCA)->f1;_tmpD9=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpCA)->f2;_LLC3:
 _tmpD6=_tmpD8;_tmpD7=_tmpD9;goto _LLC5;case 3: _LLC4: _tmpD6=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpCA)->f1;_tmpD7=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpCA)->f3;_LLC5:
# 537
 Cyc_Tovc_exp_to_vc(env,_tmpD7);_tmpD5=_tmpD6;goto _LLC7;case 11: _LLC6: _tmpD5=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpCA)->f1;_LLC7:
# 539
 _tmpD4=_tmpD5;goto _LLC9;case 12: _LLC8: _tmpD4=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpCA)->f1;_LLC9:
 _tmpD3=_tmpD4;goto _LLCB;case 13: _LLCA: _tmpD3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpCA)->f2;_LLCB:
 _tmpD2=_tmpD3;goto _LLCD;case 14: _LLCC: _tmpD2=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpCA)->f1;_LLCD:
 _tmpD1=_tmpD2;goto _LLCF;case 17: _LLCE: _tmpD1=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpCA)->f1;_LLCF:
 _tmpD0=_tmpD1;goto _LLD1;case 19: _LLD0: _tmpD0=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpCA)->f1;_LLD1:
 _tmpCF=_tmpD0;goto _LLD3;case 20: _LLD2: _tmpCF=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpCA)->f1;_LLD3:
 _tmpCE=_tmpCF;goto _LLD5;case 21: _LLD4: _tmpCE=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpCA)->f1;_LLD5:
 _tmpCD=_tmpCE;goto _LLD7;case 4: _LLD6: _tmpCD=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpCA)->f1;_LLD7:
# 548
 Cyc_Tovc_exp_to_vc(env,_tmpCD);
return;case 16: _LLD8: _LLD9:
# 551
 goto _LLDB;case 18: _LLDA: _LLDB:
 goto _LLDD;case 32: _LLDC: _LLDD:
 goto _LLDF;case 31: _LLDE: _LLDF:
 return;case 35: _LLE0: _tmpCC=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpCA)->f2;_LLE1:
# 560
 for(0;_tmpCC != 0;_tmpCC=_tmpCC->tl){
Cyc_Tovc_exp_to_vc(env,(*((struct _tuple10*)_tmpCC->hd)).f2);}
return;case 36: _LLE2: _tmpCB=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpCA)->f1;_LLE3:
# 568
 Cyc_Tovc_stmt_to_vc(env,_tmpCB);
# 570
e->r=(Cyc_Tovc_stmt_to_exp(env,_tmpCB))->r;
return;case 39: _LLE4: _LLE5:
# 573
 return;case 10: _LLE6: _LLE7:
# 575
 bad_form=({const char*_tmpE6="Throw_e";_tag_dyneither(_tmpE6,sizeof(char),8);});goto _LLB1;case 15: _LLE8: _LLE9:
 bad_form=({const char*_tmpE7="New_e";_tag_dyneither(_tmpE7,sizeof(char),6);});goto _LLB1;case 23: _LLEA: _LLEB:
 bad_form=({const char*_tmpE8="Tuple_e";_tag_dyneither(_tmpE8,sizeof(char),8);});goto _LLB1;case 24: _LLEC: _LLED:
 bad_form=({const char*_tmpE9="CompoundLit_e";_tag_dyneither(_tmpE9,sizeof(char),14);});goto _LLB1;case 25: _LLEE: _LLEF:
 bad_form=({const char*_tmpEA="Array_e";_tag_dyneither(_tmpEA,sizeof(char),8);});goto _LLB1;case 26: _LLF0: _LLF1:
 bad_form=({const char*_tmpEB="Comprehension_e";_tag_dyneither(_tmpEB,sizeof(char),16);});goto _LLB1;case 27: _LLF2: _LLF3:
 bad_form=({const char*_tmpEC="ComprehensionNoinit_e";_tag_dyneither(_tmpEC,sizeof(char),22);});goto _LLB1;case 28: _LLF4: _LLF5:
 bad_form=({const char*_tmpED="Aggregate_e";_tag_dyneither(_tmpED,sizeof(char),12);});goto _LLB1;case 29: _LLF6: _LLF7:
 bad_form=({const char*_tmpEE="AnonStruct_e";_tag_dyneither(_tmpEE,sizeof(char),13);});goto _LLB1;case 30: _LLF8: _LLF9:
# 585
 bad_form=({const char*_tmpEF="Datatype_e";_tag_dyneither(_tmpEF,sizeof(char),11);});goto _LLB1;case 33: _LLFA: _LLFB:
 bad_form=({const char*_tmpF0="Malloc_e";_tag_dyneither(_tmpF0,sizeof(char),9);});goto _LLB1;case 34: _LLFC: _LLFD:
 bad_form=({const char*_tmpF1="Swap_e";_tag_dyneither(_tmpF1,sizeof(char),7);});goto _LLB1;case 37: _LLFE: _LLFF:
 bad_form=({const char*_tmpF2="Tagcheck_e";_tag_dyneither(_tmpF2,sizeof(char),11);});goto _LLB1;default: _LL100: _LL101:
 bad_form=({const char*_tmpF3="Valueof_e";_tag_dyneither(_tmpF3,sizeof(char),10);});goto _LLB1;}_LLB1:;}
# 591
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4[0]=({struct Cyc_Core_Impossible_exn_struct _tmpF5;_tmpF5.tag=Cyc_Core_Impossible;_tmpF5.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpF9;_tmpF9.tag=0;_tmpF9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({struct Cyc_String_pa_PrintArg_struct _tmpF8;_tmpF8.tag=0;_tmpF8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)bad_form);({void*_tmpF6[2]={& _tmpF8,& _tmpF9};Cyc_aprintf(({const char*_tmpF7="bad exp form %s (exp=|%s|) after xlation to C";_tag_dyneither(_tmpF7,sizeof(char),46);}),_tag_dyneither(_tmpF6,sizeof(void*),2));});});});_tmpF5;});_tmpF4;}));}
# 595
static void Cyc_Tovc_stmt_to_vc(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s){
# 597
while(1){
env.encloser=s;{
void*_tmpFB=s->r;void*_tmpFC=_tmpFB;struct Cyc_Absyn_Decl*_tmpFD;struct Cyc_Absyn_Stmt*_tmpFE;struct Cyc_Absyn_Exp*_tmpFF;struct Cyc_List_List*_tmp100;struct Cyc_Absyn_Stmt*_tmp101;struct Cyc_Absyn_Exp*_tmp102;struct Cyc_Absyn_Exp*_tmp103;struct Cyc_Absyn_Exp*_tmp104;struct Cyc_Absyn_Exp*_tmp105;struct Cyc_Absyn_Stmt*_tmp106;struct Cyc_Absyn_Exp*_tmp107;struct Cyc_Absyn_Stmt*_tmp108;struct Cyc_Absyn_Exp*_tmp109;struct Cyc_Absyn_Stmt*_tmp10A;struct Cyc_Absyn_Stmt*_tmp10B;struct Cyc_Absyn_Stmt*_tmp10C;struct Cyc_Absyn_Stmt*_tmp10D;struct Cyc_Absyn_Exp*_tmp10E;struct Cyc_Absyn_Exp*_tmp10F;struct Cyc_Absyn_Stmt*_tmp110;switch(*((int*)_tmpFC)){case 0: _LL103: _LL104:
 goto _LL106;case 6: _LL105: _LL106:
 goto _LL108;case 7: _LL107: _LL108:
 goto _LL10A;case 8: _LL109: _LL10A:
 return;case 13: _LL10B: _tmp110=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmpFC)->f2;_LL10C:
# 605
 s=_tmp110;
continue;case 3: _LL10D: _tmp10F=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmpFC)->f1;_LL10E:
# 608
 if(_tmp10F == 0)
return;
_tmp10E=_tmp10F;goto _LL110;case 1: _LL10F: _tmp10E=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpFC)->f1;_LL110:
# 612
 Cyc_Tovc_exp_to_vc(env,_tmp10E);
return;case 2: _LL111: _tmp10C=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpFC)->f1;_tmp10D=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpFC)->f2;_LL112:
# 615
 Cyc_Tovc_stmt_to_vc(env,_tmp10C);
s=_tmp10D;
continue;case 4: _LL113: _tmp109=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmpFC)->f1;_tmp10A=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmpFC)->f2;_tmp10B=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmpFC)->f3;_LL114:
# 619
 Cyc_Tovc_exp_to_vc(env,_tmp109);
Cyc_Tovc_stmt_to_vc(env,_tmp10A);
s=_tmp10B;
continue;case 5: _LL115: _tmp107=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmpFC)->f1).f1;_tmp108=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmpFC)->f2;_LL116:
# 624
 Cyc_Tovc_exp_to_vc(env,_tmp107);
s=_tmp108;
continue;case 9: _LL117: _tmp103=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmpFC)->f1;_tmp104=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmpFC)->f2).f1;_tmp105=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmpFC)->f3).f1;_tmp106=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmpFC)->f4;_LL118:
# 628
 Cyc_Tovc_exp_to_vc(env,_tmp103);
Cyc_Tovc_exp_to_vc(env,_tmp104);
Cyc_Tovc_exp_to_vc(env,_tmp105);
s=_tmp106;
continue;case 14: _LL119: _tmp101=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmpFC)->f1;_tmp102=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmpFC)->f2).f1;_LL11A:
# 634
 Cyc_Tovc_exp_to_vc(env,_tmp102);
s=_tmp101;
continue;case 10: _LL11B: _tmpFF=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmpFC)->f1;_tmp100=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmpFC)->f2;_LL11C:
# 640
 Cyc_Tovc_exp_to_vc(env,_tmpFF);
for(0;_tmp100 != 0;_tmp100=_tmp100->tl){
Cyc_Tovc_stmt_to_vc(env,((struct Cyc_Absyn_Switch_clause*)_tmp100->hd)->body);}
return;case 12: _LL11D: _tmpFD=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpFC)->f1;_tmpFE=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpFC)->f2;_LL11E:
# 645
{void*_tmp111=_tmpFD->r;void*_tmp112=_tmp111;struct Cyc_Absyn_Vardecl*_tmp113;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp112)->tag == 0){_LL122: _tmp113=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp112)->f1;_LL123:
# 647
 if(Cyc_Tovc_elim_array_initializers){
# 652
void*_tmp114=Cyc_Tcutil_compress(_tmp113->type);void*_tmp115=_tmp114;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp115)->tag == 8){_LL127: _LL128:
# 654
 if(_tmp113->initializer != 0){
void*_tmp116=((struct Cyc_Absyn_Exp*)_check_null(_tmp113->initializer))->r;void*_tmp117=_tmp116;struct Cyc_List_List*_tmp118;if(((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp117)->tag == 35){_LL12C: _tmp118=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp117)->f2;_LL12D:
# 657
 _tmp113->initializer=0;{
# 659
struct Cyc_Absyn_Exp*arr=Cyc_Absyn_var_exp(_tmp113->name,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_new_stmt(_tmpFE->r,0);
{unsigned int i=0;for(0;_tmp118 != 0;(_tmp118=_tmp118->tl,++ i)){
s3=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(arr,
Cyc_Absyn_uint_exp(i,0),0),(*((struct _tuple10*)_tmp118->hd)).f2,0),0),s3,0);}}
# 667
_tmpFE->r=s3->r;
goto _LL12B;};}else{_LL12E: _LL12F: {
# 670
struct _dyneither_ptr _tmp119=({struct Cyc_String_pa_PrintArg_struct _tmp11E;_tmp11E.tag=0;_tmp11E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_stmt2string(s));({void*_tmp11C[1]={& _tmp11E};Cyc_aprintf(({const char*_tmp11D="|%s|: array type bad initializer";_tag_dyneither(_tmp11D,sizeof(char),33);}),_tag_dyneither(_tmp11C,sizeof(void*),1));});});
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A[0]=({struct Cyc_Core_Impossible_exn_struct _tmp11B;_tmp11B.tag=Cyc_Core_Impossible;_tmp11B.f1=(struct _dyneither_ptr)_tmp119;_tmp11B;});_tmp11A;}));}}_LL12B:;}
# 674
goto _LL126;}else{_LL129: _LL12A:
 goto _LL126;}_LL126:;}
# 678
if(_tmp113->initializer != 0)
Cyc_Tovc_exp_to_vc(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp113->initializer));
env.all_locals=({struct Cyc_List_List*_tmp11F=_region_malloc(env.rgn,sizeof(*_tmp11F));_tmp11F->hd=_tmp113;_tmp11F->tl=env.all_locals;_tmp11F;});
s=_tmpFE;
continue;}else{_LL124: _LL125:
 goto _LL121;}_LL121:;}
# 685
goto _LL120;default: _LL11F: _LL120:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp120=_cycalloc(sizeof(*_tmp120));_tmp120[0]=({struct Cyc_Core_Impossible_exn_struct _tmp121;_tmp121.tag=Cyc_Core_Impossible;_tmp121.f1=({const char*_tmp122="bad stmt after xlation to C";_tag_dyneither(_tmp122,sizeof(char),28);});_tmp121;});_tmp120;}));}_LL102:;};}}
# 691
struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*old_ds){
struct Cyc_List_List*_tmp123=0;
for(0;old_ds != 0;old_ds=old_ds->tl){
struct Cyc_Absyn_Decl*_tmp124=(struct Cyc_Absyn_Decl*)old_ds->hd;
void*_tmp125=_tmp124->r;void*_tmp126=_tmp125;struct Cyc_List_List*_tmp127;struct Cyc_List_List*_tmp128;struct Cyc_List_List*_tmp129;struct Cyc_List_List*_tmp12A;struct Cyc_Absyn_Fndecl*_tmp12B;switch(*((int*)_tmp126)){case 1: _LL131: _tmp12B=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp126)->f1;_LL132:
# 700
 _tmp12B->cached_typ=0;
# 707
if(_tmp12B->param_vardecls != 0){
struct _RegionHandle _tmp12C=_new_region("rgn");struct _RegionHandle*rgn=& _tmp12C;_push_region(rgn);
{struct Cyc_List_List**_tmp12D=({struct Cyc_List_List**_tmp136=_region_malloc(rgn,sizeof(*_tmp136));_tmp136[0]=0;_tmp136;});
if(_tmp12B->param_vardecls == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp130;_tmp130.tag=0;_tmp130.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp12B->name).f2);({void*_tmp12E[1]={& _tmp130};Cyc_fprintf(Cyc_stderr,({const char*_tmp12F="No param vardecls for function %s\n";_tag_dyneither(_tmp12F,sizeof(char),35);}),_tag_dyneither(_tmp12E,sizeof(void*),1));});});
Cyc_fflush(Cyc_stderr);}
# 714
Cyc_Tovc_stmt_to_vc(({struct Cyc_Tovc_ToExpEnv _tmp131;_tmp131.rgn=rgn;_tmp131.all_locals=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(rgn,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp12B->param_vardecls))->v);_tmp131.varmap=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(rgn,Cyc_strptrcmp);_tmp131.encloser=_tmp12B->body;_tmp131.gen_ds=_tmp12D;_tmp131;}),_tmp12B->body);
# 720
if(*_tmp12D != 0){
struct Cyc_Absyn_Vardecl*vd=({struct Cyc_Absyn_Vardecl*_tmp135=_cycalloc(sizeof(*_tmp135));_tmp135->sc=_tmp12B->sc;_tmp135->name=_tmp12B->name;_tmp135->tq=Cyc_Absyn_const_tqual(0);_tmp135->type=
Cyc_Tcutil_fndecl2typ(_tmp12B);_tmp135->initializer=0;_tmp135->rgn=0;_tmp135->attributes=_tmp12B->attributes;_tmp135->escapes=0;_tmp135;});
_tmp123=({struct Cyc_List_List*_tmp132=_cycalloc(sizeof(*_tmp132));_tmp132->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp133=_cycalloc(sizeof(*_tmp133));_tmp133[0]=({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp134;_tmp134.tag=0;_tmp134.f1=vd;_tmp134;});_tmp133;}),0);_tmp132->tl=_tmp123;_tmp132;});
# 725
_tmp123=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(*_tmp12D,_tmp123);}}
# 709
;_pop_region(rgn);}
# 729
goto _LL134;case 0: _LL133: _LL134:
 goto _LL136;case 2: _LL135: _LL136:
 goto _LL138;case 3: _LL137: _LL138:
 goto _LL13A;case 4: _LL139: _LL13A:
 goto _LL13C;case 5: _LL13B: _LL13C:
 goto _LL13E;case 6: _LL13D: _LL13E:
 goto _LL140;case 7: _LL13F: _LL140:
 goto _LL142;case 8: _LL141: _LL142:
# 738
 _tmp123=({struct Cyc_List_List*_tmp137=_cycalloc(sizeof(*_tmp137));_tmp137->hd=_tmp124;_tmp137->tl=_tmp123;_tmp137;});
goto _LL130;case 9: _LL143: _tmp12A=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp126)->f2;_LL144:
 _tmp129=_tmp12A;goto _LL146;case 10: _LL145: _tmp129=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp126)->f2;_LL146:
 _tmp128=_tmp129;goto _LL148;case 11: _LL147: _tmp128=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp126)->f1;_LL148:
 _tmp127=_tmp128;goto _LL14A;case 12: _LL149: _tmp127=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp126)->f1;_LL14A:
 goto _LL14C;case 13: _LL14B: _LL14C:
 goto _LL14E;default: _LL14D: _LL14E:
# 746
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp138=_cycalloc(sizeof(*_tmp138));_tmp138[0]=({struct Cyc_Core_Impossible_exn_struct _tmp139;_tmp139.tag=Cyc_Core_Impossible;_tmp139.f1=({const char*_tmp13A="nested translation unit after translation to C";_tag_dyneither(_tmp13A,sizeof(char),47);});_tmp139;});_tmp138;}));}_LL130:;}
# 749
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp123);}
