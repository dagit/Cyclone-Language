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
typedef struct Cyc___cycFILE Cyc_FILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;
# 197 "cycboot.h"
int Cyc_sscanf(struct _dyneither_ptr,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;
# 97 "core.h"
struct _dyneither_ptr Cyc_Core_new_string(unsigned int);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 150
extern struct _RegionHandle*Cyc_Core_heap_region;
# 152
extern struct _RegionHandle*Cyc_Core_unique_region;
# 159
void Cyc_Core_ufree(struct _dyneither_ptr ptr);struct Cyc_Core_DynamicRegion;
# 187
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_region_key_t;
# 193
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_uregion_key_t;
# 198
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_rcregion_key_t;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 215
struct Cyc_Core_NewDynamicRegion Cyc_Core_new_ukey();
# 223
void Cyc_Core_free_ukey(struct Cyc_Core_DynamicRegion*k);
# 233
void*Cyc_Core_open_region(struct Cyc_Core_DynamicRegion*key,void*arg,void*(*body)(struct _RegionHandle*h,void*arg));
# 280 "core.h"
inline static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){
# 285
if(bd >> 20  || sz >> 12)
return 0;{
unsigned char*ptrbd=dyn.curr + bd * sz;
if(((ptrbd < dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
# 292
return 0;
return dyn.curr;};}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};
# 57 "lexing.h"
typedef struct Cyc_Lexing_lexbuf*Cyc_Lexing_Lexbuf;struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};
# 64
typedef struct Cyc_Lexing_function_lexbuf_state*Cyc_Lexing_Function_lexbuf_state;struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr lex_check;};
# 74
typedef struct Cyc_Lexing_lex_tables*Cyc_Lexing_LexTables;
# 84
struct _dyneither_ptr Cyc_Lexing_lexeme(struct Cyc_Lexing_lexbuf*);
char Cyc_Lexing_lexeme_char(struct Cyc_Lexing_lexbuf*,int);
int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);
int Cyc_Lexing_lexeme_end(struct Cyc_Lexing_lexbuf*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;
# 51 "set.h"
struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));
# 63
struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);
# 127
void Cyc_Set_iter(void(*f)(void*),struct Cyc_Set_Set*s);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
# 54
int Cyc_zstrptrcmp(struct _dyneither_ptr*,struct _dyneither_ptr*);
# 66
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);
# 73
struct _dyneither_ptr Cyc_zstrncpy(struct _dyneither_ptr,struct _dyneither_ptr,unsigned long);
# 107 "string.h"
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);struct Cyc_Xarray_Xarray{struct _dyneither_ptr elmts;int num_elmts;};
# 38 "xarray.h"
typedef struct Cyc_Xarray_Xarray*Cyc_Xarray_xarray_t;
# 42
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);
# 51
struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate(struct _RegionHandle*,int,void*);
# 66
void Cyc_Xarray_add(struct Cyc_Xarray_Xarray*,void*);
# 69
int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;
struct Cyc_Position_Error*Cyc_Position_mk_err_lex(unsigned int,struct _dyneither_ptr);extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
# 64
void Cyc_Position_post_error(struct Cyc_Position_Error*);struct Cyc_Relations_Reln;
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 105
typedef struct _tuple0*Cyc_Absyn_qvar_t;typedef struct _tuple0*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_opt_t;
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{int tag;};
# 286
typedef void*Cyc_Absyn_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 385
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 390
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 458 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 461
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 522
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 548
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 555
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 573
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 928 "absyn.h"
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_FlatList{struct Cyc_FlatList*tl;void*hd[0] __attribute__((aligned )) ;};
# 36 "parse.h"
typedef struct Cyc_FlatList*Cyc_flat_list_t;struct Cyc_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned int loc;};
# 47
typedef struct Cyc_Type_specifier Cyc_type_specifier_t;struct Cyc_Declarator{struct _tuple0*id;struct Cyc_List_List*tms;};
# 52
typedef struct Cyc_Declarator Cyc_declarator_t;struct _tuple11{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple10{struct _tuple10*tl;struct _tuple11 hd  __attribute__((aligned )) ;};
typedef struct _tuple10*Cyc_declarator_list_t;
# 55
enum Cyc_Storage_class{Cyc_Typedef_sc  = 0,Cyc_Extern_sc  = 1,Cyc_ExternC_sc  = 2,Cyc_Static_sc  = 3,Cyc_Auto_sc  = 4,Cyc_Register_sc  = 5,Cyc_Abstract_sc  = 6};
# 59
typedef enum Cyc_Storage_class Cyc_storage_class_t;struct Cyc_Declaration_spec{enum Cyc_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};
# 67
typedef struct Cyc_Declaration_spec Cyc_decl_spec_t;struct Cyc_Abstractdeclarator{struct Cyc_List_List*tms;};
# 71
typedef struct Cyc_Abstractdeclarator Cyc_abstractdeclarator_t;struct _union_YYSTYPE_Int_tok{int tag;struct _tuple5 val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple12{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple12 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple13{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple13*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple14{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple10*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Declarator val;};struct _tuple16{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple16*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple17{struct Cyc_Absyn_Tqual f1;struct Cyc_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple17 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple18{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple18*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 78 "parse_tab.h"
typedef struct Cyc_Yyltype Cyc_yyltype;
# 83
extern struct Cyc_Yyltype Cyc_yylloc;struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct Cyc_RgnOrder_RgnPO;
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
# 42 "tcutil.h"
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 373 "tcutil.h"
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 43 "lex.cyl"
void Cyc_yyerror(struct _dyneither_ptr s,int state,int token);
# 47
int Cyc_Lex_compile_for_boot_r=0;struct Cyc_Lex_Trie;
# 50
typedef struct Cyc_Lex_Trie*Cyc_Lex_trie_t;struct _union_TrieChildren_Zero{int tag;int val;};struct _tuple19{int f1;struct Cyc_Lex_Trie*f2;};struct _union_TrieChildren_One{int tag;struct _tuple19 val;};struct _union_TrieChildren_Many{int tag;struct Cyc_Lex_Trie**val;};union Cyc_Lex_TrieChildren{struct _union_TrieChildren_Zero Zero;struct _union_TrieChildren_One One;struct _union_TrieChildren_Many Many;};
# 56
union Cyc_Lex_TrieChildren Cyc_Lex_Zero(){union Cyc_Lex_TrieChildren _tmp16A;return((_tmp16A.Zero).val=0,(((_tmp16A.Zero).tag=1,_tmp16A)));}
union Cyc_Lex_TrieChildren Cyc_Lex_One(int i,struct Cyc_Lex_Trie*t){
struct _tuple19 _tmp16D;union Cyc_Lex_TrieChildren _tmp16C;return((_tmp16C.One).val=((_tmp16D.f1=i,((_tmp16D.f2=t,_tmp16D)))),(((_tmp16C.One).tag=2,_tmp16C)));}
# 60
union Cyc_Lex_TrieChildren Cyc_Lex_Many(struct Cyc_Lex_Trie**ts){
union Cyc_Lex_TrieChildren _tmp16E;return((_tmp16E.Many).val=ts,(((_tmp16E.Many).tag=3,_tmp16E)));}struct Cyc_Lex_Trie{union Cyc_Lex_TrieChildren children;int shared_str;};
# 71
static int Cyc_Lex_num_kws=0;
static struct _dyneither_ptr Cyc_Lex_kw_nums={(void*)0,(void*)0,(void*)(0 + 0)};struct Cyc_Lex_DynTrie{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Lex_Trie*t;};
# 79
typedef struct Cyc_Lex_DynTrie*Cyc_Lex_dyn_trie_t;struct Cyc_Lex_DynTrieSymbols{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Lex_Trie*t;struct Cyc_Xarray_Xarray*symbols;};
# 86
typedef struct Cyc_Lex_DynTrieSymbols*Cyc_Lex_dyn_trie_symbols_t;
static struct Cyc_Lex_DynTrieSymbols*Cyc_Lex_ids_trie=0;
static struct Cyc_Lex_DynTrie*Cyc_Lex_typedefs_trie=0;
# 90
static int Cyc_Lex_comment_depth=0;
# 93
static struct _tuple5 Cyc_Lex_token_int={Cyc_Absyn_Signed,0};static char _tmp4[8]="*bogus*";
static struct _dyneither_ptr Cyc_Lex_bogus_string={_tmp4,_tmp4,_tmp4 + 8};
static struct _tuple0 Cyc_Lex_token_id_pair={{.Loc_n={4,0}},& Cyc_Lex_bogus_string};
# 98
static char Cyc_Lex_token_char='\000';static char _tmp5[1]="";
static struct _dyneither_ptr Cyc_Lex_token_string={_tmp5,_tmp5,_tmp5 + 1};
static struct _tuple0*Cyc_Lex_token_qvar=& Cyc_Lex_token_id_pair;static char _tmp6[1]="";
static struct _tuple12 Cyc_Lex_token_asm={0,{_tmp6,_tmp6,_tmp6 + 1}};
# 103
static int Cyc_Lex_runaway_start=0;
static int Cyc_Lex_paren_depth=0;
# 106
static void Cyc_Lex_err(struct _dyneither_ptr msg,struct Cyc_Lexing_lexbuf*lb){
unsigned int s=(unsigned int)Cyc_Lexing_lexeme_start(lb);
Cyc_Position_post_error(Cyc_Position_mk_err_lex(s,msg));}
# 110
static void Cyc_Lex_runaway_err(struct _dyneither_ptr msg,struct Cyc_Lexing_lexbuf*lb){
unsigned int s=(unsigned int)Cyc_Lex_runaway_start;
Cyc_Position_post_error(Cyc_Position_mk_err_lex(s,msg));}struct _tuple20{struct _dyneither_ptr f1;short f2;int f3;};static char _tmp7[14]="__attribute__";static char _tmp8[9]="abstract";static char _tmp9[5]="auto";static char _tmpA[6]="break";static char _tmpB[7]="calloc";static char _tmpC[5]="case";static char _tmpD[6]="catch";static char _tmpE[5]="char";static char _tmpF[6]="const";static char _tmp10[9]="continue";static char _tmp11[9]="datatype";static char _tmp12[8]="default";static char _tmp13[3]="do";static char _tmp14[7]="double";static char _tmp15[12]="dynregion_t";static char _tmp16[5]="else";static char _tmp17[5]="enum";static char _tmp18[7]="export";static char _tmp19[7]="extern";static char _tmp1A[9]="fallthru";static char _tmp1B[6]="float";static char _tmp1C[4]="for";static char _tmp1D[5]="goto";static char _tmp1E[3]="if";static char _tmp1F[7]="inline";static char _tmp20[11]="__inline__";static char _tmp21[4]="int";static char _tmp22[4]="let";static char _tmp23[5]="long";static char _tmp24[7]="malloc";static char _tmp25[10]="namespace";static char _tmp26[4]="new";static char _tmp27[5]="NULL";static char _tmp28[8]="numelts";static char _tmp29[9]="offsetof";static char _tmp2A[20]="__cyclone_port_on__";static char _tmp2B[21]="__cyclone_port_off__";static char _tmp2C[8]="rcalloc";static char _tmp2D[9]="region_t";static char _tmp2E[7]="region";static char _tmp2F[8]="regions";static char _tmp30[9]="register";static char _tmp31[13]="reset_region";static char _tmp32[9]="restrict";static char _tmp33[7]="return";static char _tmp34[8]="rmalloc";static char _tmp35[5]="rnew";static char _tmp36[6]="short";static char _tmp37[7]="signed";static char _tmp38[7]="sizeof";static char _tmp39[7]="static";static char _tmp3A[7]="struct";static char _tmp3B[7]="switch";static char _tmp3C[9]="tagcheck";static char _tmp3D[6]="tag_t";static char _tmp3E[6]="throw";static char _tmp3F[4]="try";static char _tmp40[8]="typedef";static char _tmp41[6]="union";static char _tmp42[9]="unsigned";static char _tmp43[6]="using";static char _tmp44[8]="valueof";static char _tmp45[10]="valueof_t";static char _tmp46[5]="void";static char _tmp47[9]="volatile";static char _tmp48[6]="while";
# 118
static struct _tuple20 Cyc_Lex_rw_array[66]={{{_tmp7,_tmp7,_tmp7 + 14},371,1},{{_tmp8,_tmp8,_tmp8 + 9},300,0},{{_tmp9,_tmp9,_tmp9 + 5},258,1},{{_tmpA,_tmpA,_tmpA + 6},290,1},{{_tmpB,_tmpB,_tmpB + 7},307,0},{{_tmpC,_tmpC,_tmpC + 5},277,1},{{_tmpD,_tmpD,_tmpD + 6},297,1},{{_tmpE,_tmpE,_tmpE + 5},264,1},{{_tmpF,_tmpF,_tmpF + 6},272,1},{{_tmp10,_tmp10,_tmp10 + 9},289,1},{{_tmp11,_tmp11,_tmp11 + 9},304,0},{{_tmp12,_tmp12,_tmp12 + 8},278,1},{{_tmp13,_tmp13,_tmp13 + 3},286,1},{{_tmp14,_tmp14,_tmp14 + 7},269,1},{{_tmp15,_tmp15,_tmp15 + 12},318,0},{{_tmp16,_tmp16,_tmp16 + 5},283,1},{{_tmp17,_tmp17,_tmp17 + 5},292,1},{{_tmp18,_tmp18,_tmp18 + 7},298,0},{{_tmp19,_tmp19,_tmp19 + 7},261,1},{{_tmp1A,_tmp1A,_tmp1A + 9},301,0},{{_tmp1B,_tmp1B,_tmp1B + 6},268,1},{{_tmp1C,_tmp1C,_tmp1C + 4},287,1},{{_tmp1D,_tmp1D,_tmp1D + 5},288,1},{{_tmp1E,_tmp1E,_tmp1E + 3},282,1},{{_tmp1F,_tmp1F,_tmp1F + 7},279,1},{{_tmp20,_tmp20,_tmp20 + 11},279,1},{{_tmp21,_tmp21,_tmp21 + 4},266,1},{{_tmp22,_tmp22,_tmp22 + 4},294,0},{{_tmp23,_tmp23,_tmp23 + 5},267,1},{{_tmp24,_tmp24,_tmp24 + 7},305,0},{{_tmp25,_tmp25,_tmp25 + 10},303,0},{{_tmp26,_tmp26,_tmp26 + 4},299,0},{{_tmp27,_tmp27,_tmp27 + 5},293,0},{{_tmp28,_tmp28,_tmp28 + 8},319,0},{{_tmp29,_tmp29,_tmp29 + 9},281,1},{{_tmp2A,_tmp2A,_tmp2A + 20},316,0},{{_tmp2B,_tmp2B,_tmp2B + 21},317,0},{{_tmp2C,_tmp2C,_tmp2C + 8},308,0},{{_tmp2D,_tmp2D,_tmp2D + 9},310,0},{{_tmp2E,_tmp2E,_tmp2E + 7},312,0},{{_tmp2F,_tmp2F,_tmp2F + 8},314,0},{{_tmp30,_tmp30,_tmp30 + 9},259,1},{{_tmp31,_tmp31,_tmp31 + 13},315,0},{{_tmp32,_tmp32,_tmp32 + 9},274,1},{{_tmp33,_tmp33,_tmp33 + 7},291,1},{{_tmp34,_tmp34,_tmp34 + 8},306,0},{{_tmp35,_tmp35,_tmp35 + 5},313,0},{{_tmp36,_tmp36,_tmp36 + 6},265,1},{{_tmp37,_tmp37,_tmp37 + 7},270,1},{{_tmp38,_tmp38,_tmp38 + 7},280,1},{{_tmp39,_tmp39,_tmp39 + 7},260,1},{{_tmp3A,_tmp3A,_tmp3A + 7},275,1},{{_tmp3B,_tmp3B,_tmp3B + 7},284,1},{{_tmp3C,_tmp3C,_tmp3C + 9},322,0},{{_tmp3D,_tmp3D,_tmp3D + 6},311,0},{{_tmp3E,_tmp3E,_tmp3E + 6},295,0},{{_tmp3F,_tmp3F,_tmp3F + 4},296,0},{{_tmp40,_tmp40,_tmp40 + 8},262,1},{{_tmp41,_tmp41,_tmp41 + 6},276,1},{{_tmp42,_tmp42,_tmp42 + 9},271,1},{{_tmp43,_tmp43,_tmp43 + 6},302,0},{{_tmp44,_tmp44,_tmp44 + 8},320,0},{{_tmp45,_tmp45,_tmp45 + 10},321,0},{{_tmp46,_tmp46,_tmp46 + 5},263,1},{{_tmp47,_tmp47,_tmp47 + 9},273,1},{{_tmp48,_tmp48,_tmp48 + 6},285,1}};
# 188
static int Cyc_Lex_num_keywords(int include_cyclone_keywords){
int sum=0;
{unsigned int i=0;for(0;i < 66;++ i){
if(include_cyclone_keywords  || (Cyc_Lex_rw_array[(int)i]).f3)
++ sum;}}
return sum;}
# 197
static int Cyc_Lex_trie_char(int c){
# 199
if(c >= 95)return c - 59;else{
if(c > 64)return c - 55;}
return c - 48;}static struct Cyc_Lex_Trie*Cyc_Lex_trie_lookup(struct _RegionHandle*r,struct Cyc_Lex_Trie*t,struct _dyneither_ptr buff,int offset,int len);static void _tmp174(unsigned int*_tmp173,unsigned int*_tmp172,struct Cyc_Lex_Trie***_tmp171){for(*_tmp173=0;*_tmp173 < *_tmp172;(*_tmp173)++){(*_tmp171)[*_tmp173]=(struct Cyc_Lex_Trie*)0;}}
# 204
static struct Cyc_Lex_Trie*Cyc_Lex_trie_lookup(struct _RegionHandle*r,struct Cyc_Lex_Trie*t,struct _dyneither_ptr buff,int offset,int len){
# 206
int i=offset;
int last=(offset + len)- 1;
while(i <= last){
union Cyc_Lex_TrieChildren _tmp49=((struct Cyc_Lex_Trie*)_check_null(t))->children;union Cyc_Lex_TrieChildren _tmp4A=_tmp49;struct Cyc_Lex_Trie**_tmp4B;int _tmp4C;struct Cyc_Lex_Trie*_tmp4D;_LL1: if((_tmp4A.Many).tag != 3)goto _LL3;_tmp4B=(struct Cyc_Lex_Trie**)(_tmp4A.Many).val;_LL2: {
# 212
int ch=Cyc_Lex_trie_char((int)*((const char*)_check_dyneither_subscript(buff,sizeof(char),i)));
if(_tmp4B[_check_known_subscript_notnull(64,ch)]== 0){
struct Cyc_Lex_Trie*_tmp16F;_tmp4B[_check_known_subscript_notnull(64,ch)]=((_tmp16F=_region_malloc(r,sizeof(*_tmp16F)),((_tmp16F->children=Cyc_Lex_Zero(),((_tmp16F->shared_str=0,_tmp16F))))));}
t=_tmp4B[_check_known_subscript_notnull(64,ch)];
++ i;
goto _LL0;}_LL3: if((_tmp4A.One).tag != 2)goto _LL5;_tmp4C=((struct _tuple19)(_tmp4A.One).val).f1;_tmp4D=((struct _tuple19)(_tmp4A.One).val).f2;_LL4:
# 220
 if(_tmp4C == *((const char*)_check_dyneither_subscript(buff,sizeof(char),i)))
t=_tmp4D;else{
# 223
unsigned int _tmp173;unsigned int _tmp172;struct Cyc_Lex_Trie**_tmp171;unsigned int _tmp170;struct Cyc_Lex_Trie**_tmp4F=(_tmp170=64,((_tmp171=(struct Cyc_Lex_Trie**)_region_malloc(r,_check_times(sizeof(struct Cyc_Lex_Trie*),_tmp170)),((((_tmp172=_tmp170,_tmp174(& _tmp173,& _tmp172,& _tmp171))),_tmp171)))));
_tmp4F[_check_known_subscript_notnull(64,Cyc_Lex_trie_char(_tmp4C))]=_tmp4D;{
int _tmp50=Cyc_Lex_trie_char((int)*((const char*)_check_dyneither_subscript(buff,sizeof(char),i)));
{struct Cyc_Lex_Trie*_tmp175;_tmp4F[_tmp50]=((_tmp175=_region_malloc(r,sizeof(*_tmp175)),((_tmp175->children=Cyc_Lex_Zero(),((_tmp175->shared_str=0,_tmp175))))));}
t->children=Cyc_Lex_Many(_tmp4F);
t=_tmp4F[_tmp50];};}
# 230
++ i;
goto _LL0;_LL5: if((_tmp4A.Zero).tag != 1)goto _LL0;_LL6:
# 234
 while(i <= last){
struct Cyc_Lex_Trie*_tmp176;struct Cyc_Lex_Trie*_tmp55=(_tmp176=_region_malloc(r,sizeof(*_tmp176)),((_tmp176->children=Cyc_Lex_Zero(),((_tmp176->shared_str=0,_tmp176)))));
t->children=Cyc_Lex_One((int)*((const char*)_check_dyneither_subscript(buff,sizeof(char),i ++)),_tmp55);
t=_tmp55;}
# 239
return t;_LL0:;}
# 242
return t;}struct _tuple21{struct Cyc_Lex_Trie*f1;struct Cyc_Xarray_Xarray*f2;struct _dyneither_ptr f3;int f4;int f5;};
# 245
static int Cyc_Lex_str_index_body(struct _RegionHandle*d,struct _tuple21*env){
# 248
struct _tuple21 _tmp57=*env;struct Cyc_Lex_Trie*_tmp59;struct Cyc_Xarray_Xarray*_tmp5A;struct _dyneither_ptr _tmp5B;int _tmp5C;int _tmp5D;struct _tuple21 _tmp58=_tmp57;_tmp59=_tmp58.f1;_tmp5A=_tmp58.f2;_tmp5B=_tmp58.f3;_tmp5C=_tmp58.f4;_tmp5D=_tmp58.f5;{
struct Cyc_Lex_Trie*_tmp5E=Cyc_Lex_trie_lookup(d,_tmp59,_tmp5B,_tmp5C,_tmp5D);
# 251
if(_tmp5E->shared_str == 0){
struct _dyneither_ptr _tmp5F=Cyc_Core_new_string((unsigned int)(_tmp5D + 1));
Cyc_zstrncpy(_dyneither_ptr_decrease_size(_tmp5F,sizeof(char),1),(struct _dyneither_ptr)_dyneither_ptr_plus(_tmp5B,sizeof(char),_tmp5C),(unsigned long)_tmp5D);{
struct _dyneither_ptr*_tmp177;int ans=((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp5A,((_tmp177=_cycalloc(sizeof(*_tmp177)),((_tmp177[0]=(struct _dyneither_ptr)_tmp5F,_tmp177)))));
_tmp5E->shared_str=ans;};}
# 257
return _tmp5E->shared_str;};}
# 260
static int Cyc_Lex_str_index(struct _dyneither_ptr buff,int offset,int len){
struct Cyc_Lex_DynTrieSymbols*idt=0;
_swap_word(& idt,& Cyc_Lex_ids_trie);{
struct Cyc_Lex_DynTrieSymbols _tmp61=*((struct Cyc_Lex_DynTrieSymbols*)_check_null(idt));struct Cyc_Core_DynamicRegion*_tmp63;struct Cyc_Lex_Trie*_tmp64;struct Cyc_Xarray_Xarray*_tmp65;struct Cyc_Lex_DynTrieSymbols _tmp62=_tmp61;_tmp63=_tmp62.dyn;_tmp64=_tmp62.t;_tmp65=_tmp62.symbols;{
struct _tuple21 _tmp178;struct _tuple21 _tmp66=(_tmp178.f1=_tmp64,((_tmp178.f2=_tmp65,((_tmp178.f3=buff,((_tmp178.f4=offset,((_tmp178.f5=len,_tmp178)))))))));
int _tmp67=((int(*)(struct Cyc_Core_DynamicRegion*key,struct _tuple21*arg,int(*body)(struct _RegionHandle*h,struct _tuple21*arg)))Cyc_Core_open_region)(_tmp63,& _tmp66,Cyc_Lex_str_index_body);
{struct Cyc_Lex_DynTrieSymbols _tmp179;*idt=((_tmp179.dyn=(struct Cyc_Core_DynamicRegion*)_tmp63,((_tmp179.t=(struct Cyc_Lex_Trie*)_tmp64,((_tmp179.symbols=(struct Cyc_Xarray_Xarray*)_tmp65,_tmp179))))));}
_swap_word(& idt,& Cyc_Lex_ids_trie);
return _tmp67;};};}
# 271
static int Cyc_Lex_str_index_lbuf(struct Cyc_Lexing_lexbuf*lbuf){
return Cyc_Lex_str_index((struct _dyneither_ptr)lbuf->lex_buffer,lbuf->lex_start_pos,lbuf->lex_curr_pos - lbuf->lex_start_pos);}struct _tuple22{struct Cyc_Lex_Trie*f1;struct _dyneither_ptr f2;};
# 277
static int Cyc_Lex_insert_typedef_body(struct _RegionHandle*h,struct _tuple22*arg){
# 279
struct _tuple22 _tmp6A=*arg;struct Cyc_Lex_Trie*_tmp6C;struct _dyneither_ptr _tmp6D;struct _tuple22 _tmp6B=_tmp6A;_tmp6C=_tmp6B.f1;_tmp6D=_tmp6B.f2;{
struct Cyc_Lex_Trie*_tmp6E=Cyc_Lex_trie_lookup(h,_tmp6C,_tmp6D,0,(int)(_get_dyneither_size(_tmp6D,sizeof(char))- 1));
((struct Cyc_Lex_Trie*)_check_null(_tmp6E))->shared_str=1;
return 0;};}
# 286
static void Cyc_Lex_insert_typedef(struct _dyneither_ptr*sptr){
struct _dyneither_ptr _tmp6F=*sptr;
struct Cyc_Lex_DynTrie*tdefs=0;
_swap_word(& tdefs,& Cyc_Lex_typedefs_trie);{
struct Cyc_Lex_DynTrie _tmp70=*((struct Cyc_Lex_DynTrie*)_check_null(tdefs));struct Cyc_Core_DynamicRegion*_tmp72;struct Cyc_Lex_Trie*_tmp73;struct Cyc_Lex_DynTrie _tmp71=_tmp70;_tmp72=_tmp71.dyn;_tmp73=_tmp71.t;{
struct _tuple22 _tmp17A;struct _tuple22 _tmp74=(_tmp17A.f1=_tmp73,((_tmp17A.f2=_tmp6F,_tmp17A)));
((int(*)(struct Cyc_Core_DynamicRegion*key,struct _tuple22*arg,int(*body)(struct _RegionHandle*h,struct _tuple22*arg)))Cyc_Core_open_region)(_tmp72,& _tmp74,Cyc_Lex_insert_typedef_body);
{struct Cyc_Lex_DynTrie _tmp17B;*tdefs=((_tmp17B.dyn=(struct Cyc_Core_DynamicRegion*)_tmp72,((_tmp17B.t=(struct Cyc_Lex_Trie*)_tmp73,_tmp17B))));}
_swap_word(& tdefs,& Cyc_Lex_typedefs_trie);
return;};};}struct _tuple23{struct Cyc_Lex_Trie*f1;struct Cyc_Xarray_Xarray*f2;int f3;};
# 298
static struct _dyneither_ptr*Cyc_Lex_get_symbol_body(struct _RegionHandle*dyn,struct _tuple23*env){
struct _tuple23 _tmp77=*env;struct Cyc_Lex_Trie*_tmp79;struct Cyc_Xarray_Xarray*_tmp7A;int _tmp7B;struct _tuple23 _tmp78=_tmp77;_tmp79=_tmp78.f1;_tmp7A=_tmp78.f2;_tmp7B=_tmp78.f3;
return((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp7A,_tmp7B);}
# 303
static struct _dyneither_ptr*Cyc_Lex_get_symbol(int symbol_num){
struct Cyc_Lex_DynTrieSymbols*idt=0;
_swap_word(& idt,& Cyc_Lex_ids_trie);{
struct Cyc_Lex_DynTrieSymbols _tmp7C=*((struct Cyc_Lex_DynTrieSymbols*)_check_null(idt));struct Cyc_Core_DynamicRegion*_tmp7E;struct Cyc_Lex_Trie*_tmp7F;struct Cyc_Xarray_Xarray*_tmp80;struct Cyc_Lex_DynTrieSymbols _tmp7D=_tmp7C;_tmp7E=_tmp7D.dyn;_tmp7F=_tmp7D.t;_tmp80=_tmp7D.symbols;{
struct _tuple23 _tmp17C;struct _tuple23 _tmp81=(_tmp17C.f1=_tmp7F,((_tmp17C.f2=_tmp80,((_tmp17C.f3=symbol_num,_tmp17C)))));
struct _dyneither_ptr*_tmp82=((struct _dyneither_ptr*(*)(struct Cyc_Core_DynamicRegion*key,struct _tuple23*arg,struct _dyneither_ptr*(*body)(struct _RegionHandle*h,struct _tuple23*arg)))Cyc_Core_open_region)(_tmp7E,& _tmp81,Cyc_Lex_get_symbol_body);
{struct Cyc_Lex_DynTrieSymbols _tmp17D;*idt=((_tmp17D.dyn=(struct Cyc_Core_DynamicRegion*)_tmp7E,((_tmp17D.t=(struct Cyc_Lex_Trie*)_tmp7F,((_tmp17D.symbols=(struct Cyc_Xarray_Xarray*)_tmp80,_tmp17D))))));}
_swap_word(& idt,& Cyc_Lex_ids_trie);
return _tmp82;};};}
# 322 "lex.cyl"
static int Cyc_Lex_int_of_char(char c){
if('0' <= c  && c <= '9')return c - '0';else{
if('a' <= c  && c <= 'f')return(10 + c)- 'a';else{
if('A' <= c  && c <= 'F')return(10 + c)- 'A';else{
struct Cyc_Core_Invalid_argument_exn_struct _tmp183;const char*_tmp182;struct Cyc_Core_Invalid_argument_exn_struct*_tmp181;(int)_throw((void*)((_tmp181=_cycalloc(sizeof(*_tmp181)),((_tmp181[0]=((_tmp183.tag=Cyc_Core_Invalid_argument,((_tmp183.f1=((_tmp182="string to integer conversion",_tag_dyneither(_tmp182,sizeof(char),29))),_tmp183)))),_tmp181)))));}}}}
# 332
static struct _tuple5 Cyc_Lex_intconst(struct Cyc_Lexing_lexbuf*lbuf,int start,int end,int base){
unsigned int n=0;
int end2=lbuf->lex_curr_pos - end;
struct _dyneither_ptr buff=lbuf->lex_buffer;
int i=start + lbuf->lex_start_pos;
{int i=start + lbuf->lex_start_pos;for(0;i < end2;++ i){
char c=*((char*)_check_dyneither_subscript(buff,sizeof(char),i));
switch(c){case 'u': _LL7:
# 341
 goto _LL8;case 'U': _LL8: {
struct _tuple5 _tmp184;return(_tmp184.f1=Cyc_Absyn_Unsigned,((_tmp184.f2=(int)n,_tmp184)));}case 'l': _LL9:
 break;case 'L': _LLA:
 break;default: _LLB:
 n=n * base + (unsigned int)Cyc_Lex_int_of_char(c);break;}}}{
# 348
struct _tuple5 _tmp185;return(_tmp185.f1=Cyc_Absyn_Signed,((_tmp185.f2=(int)n,_tmp185)));};}
# 353
char Cyc_Lex_string_buffer_v[11]={'x','x','x','x','x','x','x','x','x','x','\000'};
struct _dyneither_ptr Cyc_Lex_string_buffer={(void*)Cyc_Lex_string_buffer_v,(void*)Cyc_Lex_string_buffer_v,(void*)(Cyc_Lex_string_buffer_v + 11)};
int Cyc_Lex_string_pos=0;void Cyc_Lex_store_string_char(char c);static void _tmp18B(int*sz,unsigned int*_tmp18A,unsigned int*_tmp189,char**_tmp187){for(*_tmp18A=0;*_tmp18A < *_tmp189;(*_tmp18A)++){(*_tmp187)[*_tmp18A]=(char)(
# 361
*_tmp18A < *sz?*((char*)_check_dyneither_subscript(Cyc_Lex_string_buffer,sizeof(char),(int)*_tmp18A)):'\000');}}
# 356
void Cyc_Lex_store_string_char(char c){
int sz=(int)(_get_dyneither_size(Cyc_Lex_string_buffer,sizeof(char))- 1);
if(Cyc_Lex_string_pos >= sz){
int newsz=sz;
while(Cyc_Lex_string_pos >= newsz){newsz=newsz * 2;}{
unsigned int _tmp18A;unsigned int _tmp189;struct _dyneither_ptr _tmp188;char*_tmp187;unsigned int _tmp186;struct _dyneither_ptr str=(_tmp186=(unsigned int)newsz,((_tmp187=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp186 + 1)),((_tmp188=_tag_dyneither(_tmp187,sizeof(char),_tmp186 + 1),((((_tmp189=_tmp186,((_tmp18B(& sz,& _tmp18A,& _tmp189,& _tmp187),_tmp187[_tmp189]=(char)0)))),_tmp188)))))));
Cyc_Lex_string_buffer=str;};}
# 364
{char _tmp18E;char _tmp18D;struct _dyneither_ptr _tmp18C;(_tmp18C=_dyneither_ptr_plus(Cyc_Lex_string_buffer,sizeof(char),Cyc_Lex_string_pos),((_tmp18D=*((char*)_check_dyneither_subscript(_tmp18C,sizeof(char),0)),((_tmp18E=c,((_get_dyneither_size(_tmp18C,sizeof(char))== 1  && (_tmp18D == '\000'  && _tmp18E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp18C.curr)=_tmp18E)))))));}
++ Cyc_Lex_string_pos;}
# 367
void Cyc_Lex_store_string(struct _dyneither_ptr s){
int sz=(int)Cyc_strlen((struct _dyneither_ptr)s);
int i=0;for(0;i < sz;++ i){
Cyc_Lex_store_string_char(*((const char*)_check_dyneither_subscript(s,sizeof(char),i)));}}
# 372
struct _dyneither_ptr Cyc_Lex_get_stored_string(){
struct _dyneither_ptr str=Cyc_substring((struct _dyneither_ptr)Cyc_Lex_string_buffer,0,(unsigned long)Cyc_Lex_string_pos);
Cyc_Lex_string_pos=0;
return str;}struct Cyc_Lex_Ldecls{struct Cyc_Set_Set*typedefs;struct Cyc_Set_Set*namespaces;};
# 385
typedef struct Cyc_Lex_Ldecls*Cyc_Lex_ldecls_t;struct Cyc_Lex_Lvis{struct Cyc_List_List*current_namespace;struct Cyc_List_List*imported_namespaces;};
# 392
typedef struct Cyc_Lex_Lvis*Cyc_Lex_lvis_t;struct Cyc_Lex_Lstate{struct Cyc_List_List*lstack;struct Cyc_Dict_Dict decls;};
# 400
typedef struct Cyc_Lex_Lstate*Cyc_Lex_lstate_t;
# 402
static struct Cyc_Core_Opt*Cyc_Lex_lstate=0;
# 404
static void Cyc_Lex_typedef_init(){
struct Cyc_Lex_Lvis*_tmp18F;struct Cyc_Lex_Lvis*_tmp91=(_tmp18F=_cycalloc(sizeof(*_tmp18F)),((_tmp18F->current_namespace=0,((_tmp18F->imported_namespaces=0,_tmp18F)))));
# 407
struct Cyc_List_List*_tmp190;struct Cyc_List_List*_tmp92=(_tmp190=_cycalloc(sizeof(*_tmp190)),((_tmp190->hd=_tmp91,((_tmp190->tl=0,_tmp190)))));
struct Cyc_Lex_Ldecls*_tmp191;struct Cyc_Dict_Dict init_decls=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_empty)(Cyc_Absyn_varlist_cmp),0,(
# 411
(_tmp191=_cycalloc(sizeof(*_tmp191)),((_tmp191->typedefs=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_zstrptrcmp),((_tmp191->namespaces=
((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_zstrptrcmp),_tmp191)))))));
struct Cyc_Lex_Lstate*_tmp194;struct Cyc_Core_Opt*_tmp193;Cyc_Lex_lstate=((_tmp193=_cycalloc(sizeof(*_tmp193)),((_tmp193->v=((_tmp194=_cycalloc(sizeof(*_tmp194)),((_tmp194->lstack=_tmp92,((_tmp194->decls=init_decls,_tmp194)))))),_tmp193))));}
# 416
static struct Cyc_List_List*Cyc_Lex_get_absolute_namespace(struct Cyc_List_List*ns){
# 418
struct _dyneither_ptr*n=(struct _dyneither_ptr*)ns->hd;
# 420
{struct Cyc_List_List*ls=((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack;for(0;ls != 0;ls=ls->tl){
struct Cyc_Lex_Lvis*lv=(struct Cyc_Lex_Lvis*)ls->hd;
# 423
struct Cyc_List_List*_tmp195;struct Cyc_List_List*x=(_tmp195=_cycalloc(sizeof(*_tmp195)),((_tmp195->hd=lv->current_namespace,((_tmp195->tl=lv->imported_namespaces,_tmp195)))));
# 425
for(0;x != 0;x=x->tl){
struct Cyc_Lex_Ldecls*ld=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,(struct Cyc_List_List*)x->hd);
# 428
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(ld->namespaces,n))
# 430
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((struct Cyc_List_List*)x->hd,ns);}}}
# 433
{const char*_tmp19B;void*_tmp19A[1];const char*_tmp199;struct Cyc_String_pa_PrintArg_struct _tmp198;Cyc_yyerror((struct _dyneither_ptr)((_tmp198.tag=0,((_tmp198.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_str_sepstr(ns,((_tmp199="::",_tag_dyneither(_tmp199,sizeof(char),3))))),((_tmp19A[0]=& _tmp198,Cyc_aprintf(((_tmp19B="undeclared namespace %s",_tag_dyneither(_tmp19B,sizeof(char),24))),_tag_dyneither(_tmp19A,sizeof(void*),1)))))))),- 1,- 1);}
return 0;}
# 437
static struct Cyc_Lex_Trie*Cyc_Lex_empty_trie(struct _RegionHandle*d,int dummy){
struct Cyc_Lex_Trie*_tmp19C;return(_tmp19C=_region_malloc(d,sizeof(*_tmp19C)),((_tmp19C->children=Cyc_Lex_Zero(),((_tmp19C->shared_str=0,_tmp19C)))));}
# 441
static void Cyc_Lex_recompute_typedefs(){
# 443
struct Cyc_Lex_DynTrie*tdefs=0;
_swap_word(& tdefs,& Cyc_Lex_typedefs_trie);{
struct Cyc_Lex_DynTrie _tmp9E=*((struct Cyc_Lex_DynTrie*)_check_null(tdefs));struct Cyc_Core_DynamicRegion*_tmpA0;struct Cyc_Lex_Trie*_tmpA1;struct Cyc_Lex_DynTrie _tmp9F=_tmp9E;_tmpA0=_tmp9F.dyn;_tmpA1=_tmp9F.t;
Cyc_Core_free_ukey(_tmpA0);{
struct Cyc_Core_NewDynamicRegion _tmpA2=Cyc_Core_new_ukey();struct Cyc_Core_DynamicRegion*_tmpA4;struct Cyc_Core_NewDynamicRegion _tmpA3=_tmpA2;_tmpA4=_tmpA3.key;{
struct Cyc_Lex_Trie*t2=((struct Cyc_Lex_Trie*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Lex_Trie*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmpA4,0,Cyc_Lex_empty_trie);
{struct Cyc_Lex_DynTrie _tmp19D;*tdefs=((_tmp19D.dyn=(struct Cyc_Core_DynamicRegion*)_tmpA4,((_tmp19D.t=(struct Cyc_Lex_Trie*)t2,_tmp19D))));}
_swap_word(& Cyc_Lex_typedefs_trie,& tdefs);{
# 453
struct Cyc_List_List*ls=((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack;for(0;ls != 0;ls=ls->tl){
struct Cyc_Lex_Lvis*lv=(struct Cyc_Lex_Lvis*)ls->hd;
struct Cyc_List_List*_tmp19E;struct Cyc_List_List*x=(_tmp19E=_cycalloc(sizeof(*_tmp19E)),((_tmp19E->hd=lv->current_namespace,((_tmp19E->tl=lv->imported_namespaces,_tmp19E)))));
# 457
for(0;x != 0;x=x->tl){
struct Cyc_Lex_Ldecls*ld=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,(struct Cyc_List_List*)x->hd);
((void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,ld->typedefs);}}};};};};}
# 465
static int Cyc_Lex_is_typedef_in_namespace(struct Cyc_List_List*ns,struct _dyneither_ptr*v){
struct Cyc_List_List*ans=Cyc_Lex_get_absolute_namespace(ns);
struct _handler_cons _tmpA7;_push_handler(& _tmpA7);{int _tmpA9=0;if(setjmp(_tmpA7.handler))_tmpA9=1;if(!_tmpA9){
{struct Cyc_Lex_Ldecls*ld=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,ans);
int _tmpAA=((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(ld->typedefs,v);_npop_handler(0);return _tmpAA;}
# 468
;_pop_handler();}else{void*_tmpA8=(void*)_exn_thrown;void*_tmpAC=_tmpA8;void*_tmpAE;_LLE: {struct Cyc_Dict_Absent_exn_struct*_tmpAD=(struct Cyc_Dict_Absent_exn_struct*)_tmpAC;if(_tmpAD->tag != Cyc_Dict_Absent)goto _LL10;}_LLF:
# 471
 return 0;_LL10: _tmpAE=_tmpAC;_LL11:(void)_rethrow(_tmpAE);_LLD:;}};}struct _tuple24{struct Cyc_List_List*f1;struct Cyc_Lex_Trie*f2;struct _dyneither_ptr f3;};
# 475
static int Cyc_Lex_is_typedef_body(struct _RegionHandle*d,struct _tuple24*env){
# 478
struct _tuple24 _tmpAF=*env;struct Cyc_List_List*_tmpB1;struct Cyc_Lex_Trie*_tmpB2;struct _dyneither_ptr _tmpB3;struct _tuple24 _tmpB0=_tmpAF;_tmpB1=_tmpB0.f1;_tmpB2=_tmpB0.f2;_tmpB3=_tmpB0.f3;{
int len=(int)(_get_dyneither_size(_tmpB3,sizeof(char))- 1);
{int i=0;for(0;i < len;++ i){
union Cyc_Lex_TrieChildren _tmpB4=((struct Cyc_Lex_Trie*)_check_null(_tmpB2))->children;union Cyc_Lex_TrieChildren _tmpB5=_tmpB4;int _tmpB6;struct Cyc_Lex_Trie*_tmpB7;struct Cyc_Lex_Trie*_tmpB8;struct Cyc_Lex_Trie**_tmpB9;_LL13: if((_tmpB5.Zero).tag != 1)goto _LL15;_LL14:
 return 0;_LL15: if((_tmpB5.One).tag != 2)goto _LL17;_tmpB6=((struct _tuple19)(_tmpB5.One).val).f1;_tmpB7=((struct _tuple19)(_tmpB5.One).val).f2;if(!(_tmpB6 != *((const char*)_check_dyneither_subscript(_tmpB3,sizeof(char),i))))goto _LL17;_LL16:
 return 0;_LL17: if((_tmpB5.One).tag != 2)goto _LL19;_tmpB8=((struct _tuple19)(_tmpB5.One).val).f2;_LL18:
 _tmpB2=_tmpB8;goto _LL12;_LL19: if((_tmpB5.Many).tag != 3)goto _LL12;_tmpB9=(struct Cyc_Lex_Trie**)(_tmpB5.Many).val;_LL1A: {
# 486
struct Cyc_Lex_Trie*_tmpBA=_tmpB9[_check_known_subscript_notnull(64,Cyc_Lex_trie_char((int)*((const char*)_check_dyneither_subscript(_tmpB3,sizeof(char),i))))];
if(_tmpBA == 0)
return 0;
_tmpB2=_tmpBA;
goto _LL12;}_LL12:;}}
# 492
return((struct Cyc_Lex_Trie*)_check_null(_tmpB2))->shared_str;};}
# 494
static int Cyc_Lex_is_typedef(struct Cyc_List_List*ns,struct _dyneither_ptr*v){
if(ns != 0)
return Cyc_Lex_is_typedef_in_namespace(ns,v);{
# 499
struct _dyneither_ptr _tmpBB=*v;
struct Cyc_Lex_DynTrie*tdefs=0;
_swap_word(& tdefs,& Cyc_Lex_typedefs_trie);{
struct Cyc_Lex_DynTrie _tmpBC=*((struct Cyc_Lex_DynTrie*)_check_null(tdefs));struct Cyc_Core_DynamicRegion*_tmpBE;struct Cyc_Lex_Trie*_tmpBF;struct Cyc_Lex_DynTrie _tmpBD=_tmpBC;_tmpBE=_tmpBD.dyn;_tmpBF=_tmpBD.t;{
struct _tuple24 _tmp19F;struct _tuple24 _tmpC0=(_tmp19F.f1=ns,((_tmp19F.f2=_tmpBF,((_tmp19F.f3=_tmpBB,_tmp19F)))));
int _tmpC1=((int(*)(struct Cyc_Core_DynamicRegion*key,struct _tuple24*arg,int(*body)(struct _RegionHandle*h,struct _tuple24*arg)))Cyc_Core_open_region)(_tmpBE,& _tmpC0,Cyc_Lex_is_typedef_body);
{struct Cyc_Lex_DynTrie _tmp1A0;*tdefs=((_tmp1A0.dyn=(struct Cyc_Core_DynamicRegion*)_tmpBE,((_tmp1A0.t=(struct Cyc_Lex_Trie*)_tmpBF,_tmp1A0))));}
_swap_word(& tdefs,& Cyc_Lex_typedefs_trie);
return _tmpC1;};};};}
# 510
void Cyc_Lex_enter_namespace(struct _dyneither_ptr*s){
struct Cyc_List_List*ns=((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->current_namespace;
struct Cyc_List_List*_tmp1A1;struct Cyc_List_List*new_ns=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(ns,((_tmp1A1=_cycalloc(sizeof(*_tmp1A1)),((_tmp1A1->hd=s,((_tmp1A1->tl=0,_tmp1A1)))))));
# 514
{struct Cyc_Lex_Lvis*_tmp1A4;struct Cyc_List_List*_tmp1A3;((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack=(
(_tmp1A3=_cycalloc(sizeof(*_tmp1A3)),((_tmp1A3->hd=((_tmp1A4=_cycalloc(sizeof(*_tmp1A4)),((_tmp1A4->current_namespace=new_ns,((_tmp1A4->imported_namespaces=0,_tmp1A4)))))),((_tmp1A3->tl=((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack,_tmp1A3))))));}{
# 520
struct Cyc_Lex_Ldecls*ld=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,ns);
if(!((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(ld->namespaces,s)){
{struct Cyc_Lex_Ldecls*_tmp1A5;((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*v))Cyc_Dict_insert)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,ns,(
# 524
(_tmp1A5=_cycalloc(sizeof(*_tmp1A5)),((_tmp1A5->typedefs=ld->typedefs,((_tmp1A5->namespaces=
# 526
((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(ld->namespaces,s),_tmp1A5)))))));}{
# 528
struct Cyc_Lex_Ldecls*_tmp1A6;((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*v))Cyc_Dict_insert)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,new_ns,(
# 530
(_tmp1A6=_cycalloc(sizeof(*_tmp1A6)),((_tmp1A6->typedefs=
((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_zstrptrcmp),((_tmp1A6->namespaces=
# 533
((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_zstrptrcmp),_tmp1A6)))))));};}
# 539
((void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,(((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,new_ns))->typedefs);};}
# 541
void Cyc_Lex_leave_namespace(){
# 543
((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack=(struct Cyc_List_List*)_check_null((((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->tl);
Cyc_Lex_recompute_typedefs();}
# 546
void Cyc_Lex_enter_using(struct _tuple0*q){
struct Cyc_List_List*ns;
{union Cyc_Absyn_Nmspace _tmpC9=(*q).f1;union Cyc_Absyn_Nmspace _tmpCA=_tmpC9;struct Cyc_List_List*_tmpCB;struct Cyc_List_List*_tmpCC;struct Cyc_List_List*_tmpCD;_LL1C: if((_tmpCA.Loc_n).tag != 4)goto _LL1E;_LL1D:
# 550
{struct Cyc_List_List*_tmp1A7;ns=((_tmp1A7=_cycalloc(sizeof(*_tmp1A7)),((_tmp1A7->hd=(*q).f2,((_tmp1A7->tl=0,_tmp1A7))))));}
goto _LL1B;_LL1E: if((_tmpCA.Rel_n).tag != 1)goto _LL20;_tmpCB=(struct Cyc_List_List*)(_tmpCA.Rel_n).val;_LL1F:
 _tmpCC=_tmpCB;goto _LL21;_LL20: if((_tmpCA.C_n).tag != 3)goto _LL22;_tmpCC=(struct Cyc_List_List*)(_tmpCA.C_n).val;_LL21:
 _tmpCD=_tmpCC;goto _LL23;_LL22: if((_tmpCA.Abs_n).tag != 2)goto _LL1B;_tmpCD=(struct Cyc_List_List*)(_tmpCA.Abs_n).val;_LL23:
# 555
{struct Cyc_List_List*_tmp1A8;ns=(struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpCD,((_tmp1A8=_cycalloc(sizeof(*_tmp1A8)),((_tmp1A8->hd=(*q).f2,((_tmp1A8->tl=0,_tmp1A8))))))));}
goto _LL1B;_LL1B:;}{
# 559
struct Cyc_List_List*_tmpD0=Cyc_Lex_get_absolute_namespace(ns);
struct Cyc_List_List*_tmpD1=((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces;
# 562
{struct Cyc_List_List*_tmp1A9;((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces=((_tmp1A9=_cycalloc(sizeof(*_tmp1A9)),((_tmp1A9->hd=_tmpD0,((_tmp1A9->tl=_tmpD1,_tmp1A9))))));}
# 566
((void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,(((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,_tmpD0))->typedefs);};}
# 568
void Cyc_Lex_leave_using(){
struct Cyc_List_List*_tmpD3=((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces;
((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces=((struct Cyc_List_List*)_check_null(_tmpD3))->tl;
# 573
Cyc_Lex_recompute_typedefs();}
# 576
void Cyc_Lex_register_typedef(struct _tuple0*q){
# 578
struct Cyc_List_List*_tmpD4=((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->current_namespace;
struct Cyc_Dict_Dict _tmpD5=((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls;
struct Cyc_Lex_Ldecls*_tmpD6=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmpD5,_tmpD4);
struct Cyc_Lex_Ldecls*_tmp1AA;struct Cyc_Lex_Ldecls*_tmpD7=(_tmp1AA=_cycalloc(sizeof(*_tmp1AA)),((_tmp1AA->typedefs=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(_tmpD6->typedefs,(*q).f2),((_tmp1AA->namespaces=_tmpD6->namespaces,_tmp1AA)))));
# 583
((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*v))Cyc_Dict_insert)(_tmpD5,_tmpD4,_tmpD7);
# 587
Cyc_Lex_insert_typedef((*q).f2);}
# 590
static short Cyc_Lex_process_id(struct Cyc_Lexing_lexbuf*lbuf){
int symbol_num=Cyc_Lex_str_index_lbuf(lbuf);
# 594
if(symbol_num <= Cyc_Lex_num_kws){
short res=(short)*((int*)_check_dyneither_subscript(Cyc_Lex_kw_nums,sizeof(int),symbol_num - 1));
switch(res){case 332: _LL24:
# 598
{const char*_tmp1AD;void*_tmp1AC;(_tmp1AC=0,Cyc_Tcutil_warn((unsigned int)Cyc_yylloc.first_line,((_tmp1AD="deprecated ZEROTERM, use @zeroterm",_tag_dyneither(_tmp1AD,sizeof(char),35))),_tag_dyneither(_tmp1AC,sizeof(void*),0)));}
break;case 331: _LL25:
# 601
{const char*_tmp1B0;void*_tmp1AF;(_tmp1AF=0,Cyc_Tcutil_warn((unsigned int)Cyc_yylloc.first_line,((_tmp1B0="deprecated NOZEROTERM, use @nozeroterm",_tag_dyneither(_tmp1B0,sizeof(char),39))),_tag_dyneither(_tmp1AF,sizeof(void*),0)));}
break;default: _LL26:
 break;}
# 605
return(short)*((int*)_check_dyneither_subscript(Cyc_Lex_kw_nums,sizeof(int),symbol_num - 1));}{
# 608
struct _dyneither_ptr*_tmpDD=Cyc_Lex_get_symbol(symbol_num);
Cyc_Lex_token_string=*_tmpDD;
# 612
if(Cyc_Lex_is_typedef(0,_tmpDD))
return 368;
# 616
return 360;};}
# 619
static short Cyc_Lex_process_qual_id(struct Cyc_Lexing_lexbuf*lbuf){
int i=lbuf->lex_start_pos;
int end=lbuf->lex_curr_pos;
struct _dyneither_ptr s=lbuf->lex_buffer;
# 624
struct _dyneither_ptr*v=0;
struct Cyc_List_List*rev_vs=0;
# 627
while(i < end){
int start=i;
for(0;i < end  && *((char*)_check_dyneither_subscript(s,sizeof(char),i))!= ':';++ i){
;}
if(start == i){
struct Cyc_Core_Impossible_exn_struct _tmp1B6;const char*_tmp1B5;struct Cyc_Core_Impossible_exn_struct*_tmp1B4;(int)_throw((void*)((_tmp1B4=_cycalloc(sizeof(*_tmp1B4)),((_tmp1B4[0]=((_tmp1B6.tag=Cyc_Core_Impossible,((_tmp1B6.f1=((_tmp1B5="bad namespace",_tag_dyneither(_tmp1B5,sizeof(char),14))),_tmp1B6)))),_tmp1B4)))));}{
int vlen=i - start;
if(v != 0){
struct Cyc_List_List*_tmp1B7;rev_vs=((_tmp1B7=_cycalloc(sizeof(*_tmp1B7)),((_tmp1B7->hd=v,((_tmp1B7->tl=rev_vs,_tmp1B7))))));}
v=Cyc_Lex_get_symbol(Cyc_Lex_str_index((struct _dyneither_ptr)s,start,vlen));
i +=2;};}
# 639
if(v == 0){
struct Cyc_Core_Impossible_exn_struct _tmp1BD;const char*_tmp1BC;struct Cyc_Core_Impossible_exn_struct*_tmp1BB;(int)_throw((void*)((_tmp1BB=_cycalloc(sizeof(*_tmp1BB)),((_tmp1BB[0]=((_tmp1BD.tag=Cyc_Core_Impossible,((_tmp1BD.f1=((_tmp1BC="bad namespace",_tag_dyneither(_tmp1BC,sizeof(char),14))),_tmp1BD)))),_tmp1BB)))));}{
struct Cyc_List_List*vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_vs);
# 644
{const char*_tmp1BE;if(vs != 0  && Cyc_strcmp((struct _dyneither_ptr)*((struct _dyneither_ptr*)vs->hd),((_tmp1BE="Cyc",_tag_dyneither(_tmp1BE,sizeof(char),4))))== 0){
vs=vs->tl;{
struct _tuple0*_tmp1BF;Cyc_Lex_token_qvar=((_tmp1BF=_cycalloc(sizeof(*_tmp1BF)),((_tmp1BF->f1=Cyc_Absyn_Abs_n(vs,0),((_tmp1BF->f2=v,_tmp1BF))))));};}else{
# 648
const char*_tmp1C0;if(vs != 0  && Cyc_strcmp((struct _dyneither_ptr)*((struct _dyneither_ptr*)vs->hd),((_tmp1C0="C",_tag_dyneither(_tmp1C0,sizeof(char),2))))== 0){
vs=vs->tl;{
struct _tuple0*_tmp1C1;Cyc_Lex_token_qvar=((_tmp1C1=_cycalloc(sizeof(*_tmp1C1)),((_tmp1C1->f1=Cyc_Absyn_Abs_n(vs,1),((_tmp1C1->f2=v,_tmp1C1))))));};}else{
# 652
struct _tuple0*_tmp1C2;Cyc_Lex_token_qvar=((_tmp1C2=_cycalloc(sizeof(*_tmp1C2)),((_tmp1C2->f1=Cyc_Absyn_Rel_n(vs),((_tmp1C2->f2=v,_tmp1C2))))));}}}
if(Cyc_Lex_is_typedef(vs,v))
return 370;
return 369;};}struct Cyc_Lex_PosInfo{struct Cyc_Lex_PosInfo*next;unsigned int starting_line;struct _dyneither_ptr filename;struct _dyneither_ptr linenumpos;unsigned int linenumpos_offset;};
# 668
typedef struct Cyc_Lex_PosInfo*Cyc_Lex_pinfo_t;
typedef struct Cyc_Lex_PosInfo*Cyc_Lex_pinfo_opt_t;
# 671
static struct Cyc_Lex_PosInfo*Cyc_Lex_pos_info=0;
# 673
static int Cyc_Lex_linenumber=1;
# 676
static struct Cyc_Lex_PosInfo*Cyc_Lex_rnew_filepos(struct _RegionHandle*r,struct _dyneither_ptr filename,unsigned int starting_line,struct Cyc_Lex_PosInfo*next){
# 680
unsigned int*_tmp1C4;unsigned int _tmp1C3;struct _dyneither_ptr linenumpos=(_tmp1C3=10,((_tmp1C4=_region_calloc(Cyc_Core_unique_region,sizeof(unsigned int),_tmp1C3),_tag_dyneither(_tmp1C4,sizeof(unsigned int),_tmp1C3))));
*((unsigned int*)_check_dyneither_subscript(linenumpos,sizeof(unsigned int),0))=(unsigned int)Cyc_yylloc.first_line;{
struct Cyc_Lex_PosInfo*_tmp1C5;return(_tmp1C5=_region_malloc(r,sizeof(*_tmp1C5)),((_tmp1C5->next=next,((_tmp1C5->starting_line=starting_line,((_tmp1C5->filename=(struct _dyneither_ptr)filename,((_tmp1C5->linenumpos=linenumpos,((_tmp1C5->linenumpos_offset=1,_tmp1C5)))))))))));};}
# 689
static void Cyc_Lex_inc_linenumber(){
if(Cyc_Lex_pos_info == 0){struct Cyc_Core_Impossible_exn_struct _tmp1CB;const char*_tmp1CA;struct Cyc_Core_Impossible_exn_struct*_tmp1C9;(int)_throw((void*)((_tmp1C9=_cycalloc(sizeof(*_tmp1C9)),((_tmp1C9[0]=((_tmp1CB.tag=Cyc_Core_Impossible,((_tmp1CB.f1=((_tmp1CA="empty position info!",_tag_dyneither(_tmp1CA,sizeof(char),21))),_tmp1CB)))),_tmp1C9)))));}{
struct Cyc_Lex_PosInfo*p=(struct Cyc_Lex_PosInfo*)_check_null(Cyc_Lex_pos_info);
struct _dyneither_ptr linenums=_tag_dyneither(0,0,0);
_swap_dyneither(& p->linenumpos,& linenums);{
unsigned int offset=p->linenumpos_offset;
unsigned int n=_get_dyneither_size(linenums,sizeof(unsigned int));
# 697
if(offset >= n){
# 699
unsigned int*_tmp1CD;unsigned int _tmp1CC;struct _dyneither_ptr newlinenums=(_tmp1CC=n * 2,((_tmp1CD=_region_calloc(Cyc_Core_unique_region,sizeof(unsigned int),_tmp1CC),_tag_dyneither(_tmp1CD,sizeof(unsigned int),_tmp1CC))));
{unsigned int i=0;for(0;i < n;++ i){
*((unsigned int*)_check_dyneither_subscript(newlinenums,sizeof(unsigned int),(int)i))=((unsigned int*)linenums.curr)[(int)i];}}
_swap_dyneither(& linenums,& newlinenums);
# 704
((void(*)(struct _dyneither_ptr ptr))Cyc_Core_ufree)(newlinenums);}
# 707
*((unsigned int*)_check_dyneither_subscript(linenums,sizeof(unsigned int),(int)offset))=(unsigned int)Cyc_yylloc.first_line;
p->linenumpos_offset=offset + 1;
++ Cyc_Lex_linenumber;
_swap_dyneither(& p->linenumpos,& linenums);};};}
# 714
static void Cyc_Lex_process_directive(struct _dyneither_ptr line){
int i;
char buf[100];
const char*_tmp1CE;struct _dyneither_ptr filename=(_tmp1CE="",_tag_dyneither(_tmp1CE,sizeof(char),1));
{const char*_tmp1D3;void*_tmp1D2[2];struct Cyc_IntPtr_sa_ScanfArg_struct _tmp1D1;struct Cyc_CharPtr_sa_ScanfArg_struct _tmp1D0;if(((_tmp1D0.tag=7,((_tmp1D0.f1=_tag_dyneither(buf,sizeof(char),100),((_tmp1D1.tag=2,((_tmp1D1.f1=& i,((_tmp1D2[0]=& _tmp1D1,((_tmp1D2[1]=& _tmp1D0,Cyc_sscanf((struct _dyneither_ptr)line,((_tmp1D3="# %d \"%s",_tag_dyneither(_tmp1D3,sizeof(char),9))),_tag_dyneither(_tmp1D2,sizeof(void*),2))))))))))))))== 2){
if(Cyc_Lex_compile_for_boot_r){
# 722
int i=(int)(Cyc_strlen(_tag_dyneither(buf,sizeof(char),100))- 1);
int last_slash=-1;
while(i >= 0){
if(buf[_check_known_subscript_notnull(100,i)]== '/'){last_slash=i;break;}
-- i;}{
# 728
const char*_tmp1D7;void*_tmp1D6[1];struct Cyc_String_pa_PrintArg_struct _tmp1D5;filename=(struct _dyneither_ptr)((_tmp1D5.tag=0,((_tmp1D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_dyneither_ptr_plus(_dyneither_ptr_plus(_tag_dyneither(buf,sizeof(char),100),sizeof(char),last_slash),sizeof(char),1)),((_tmp1D6[0]=& _tmp1D5,Cyc_aprintf(((_tmp1D7="\"%s",_tag_dyneither(_tmp1D7,sizeof(char),4))),_tag_dyneither(_tmp1D6,sizeof(void*),1))))))));};}else{
# 730
const char*_tmp1DB;void*_tmp1DA[1];struct Cyc_String_pa_PrintArg_struct _tmp1D9;filename=(struct _dyneither_ptr)((_tmp1D9.tag=0,((_tmp1D9.f1=(struct _dyneither_ptr)_tag_dyneither(buf,sizeof(char),100),((_tmp1DA[0]=& _tmp1D9,Cyc_aprintf(((_tmp1DB="\"%s",_tag_dyneither(_tmp1DB,sizeof(char),4))),_tag_dyneither(_tmp1DA,sizeof(void*),1))))))));}
if((Cyc_Lex_linenumber == i  && Cyc_Lex_pos_info != 0) && Cyc_strcmp((struct _dyneither_ptr)((struct Cyc_Lex_PosInfo*)_check_null(Cyc_Lex_pos_info))->filename,(struct _dyneither_ptr)filename)== 0)return;
Cyc_Lex_linenumber=i;}else{
const char*_tmp1DF;void*_tmp1DE[1];struct Cyc_IntPtr_sa_ScanfArg_struct _tmp1DD;if(((_tmp1DD.tag=2,((_tmp1DD.f1=& i,((_tmp1DE[0]=& _tmp1DD,Cyc_sscanf((struct _dyneither_ptr)line,((_tmp1DF="# %d",_tag_dyneither(_tmp1DF,sizeof(char),5))),_tag_dyneither(_tmp1DE,sizeof(void*),1))))))))== 1){
if(Cyc_Lex_linenumber == i)return;
Cyc_Lex_linenumber=i;
if(Cyc_Lex_pos_info != 0)filename=((struct Cyc_Lex_PosInfo*)_check_null(Cyc_Lex_pos_info))->filename;}else{
# 738
++ Cyc_Lex_linenumber;
return;}}}
# 741
Cyc_Lex_pos_info=Cyc_Lex_rnew_filepos(Cyc_Core_heap_region,filename,(unsigned int)Cyc_Lex_linenumber,Cyc_Lex_pos_info);}struct _tuple25{struct _dyneither_ptr f1;unsigned int f2;};
# 748
struct _tuple25 Cyc_Lex_xlate_pos(unsigned int char_offset){
{struct Cyc_Lex_PosInfo*p=Cyc_Lex_pos_info;for(0;p != 0;p=p->next){
unsigned int first_char_offset=*((unsigned int*)_check_dyneither_subscript(p->linenumpos,sizeof(unsigned int),0));
if(char_offset < first_char_offset  && p->next != 0)continue;{
# 754
unsigned int base=0;
unsigned int size=p->linenumpos_offset;
while(size > 1){
int half=(int)(size / 2);
int mid=(int)(base + half);
if(char_offset > *((unsigned int*)_check_dyneither_subscript(p->linenumpos,sizeof(unsigned int),mid))){
base=base + half;
size=size - half;}else{
# 763
size=(unsigned int)half;}}{
# 766
struct _tuple25 _tmp1E0;return(_tmp1E0.f1=p->filename,((_tmp1E0.f2=p->starting_line + base,_tmp1E0)));};};}}{
# 768
struct _tuple25 _tmp1E1;return(_tmp1E1.f1=_tag_dyneither(0,0,0),((_tmp1E1.f2=0,_tmp1E1)));};}
# 773
int Cyc_Lex_token(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_scan_charconst(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_strng(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_strng_next(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_wstrng(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_wstrng_next(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_comment(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_cyc_qual(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_snarf_asm(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_snarf_asm_body(struct Cyc_Lexing_lexbuf*);
# 786
int Cyc_yylex(struct Cyc_Lexing_lexbuf*lbuf,union Cyc_YYSTYPE*yylval,struct Cyc_Yyltype*yyllocptr){
# 788
int ans=((int(*)(struct Cyc_Lexing_lexbuf*))Cyc_Lex_token)(lbuf);
Cyc_yylloc.first_line=(yyllocptr->first_line=((int(*)(struct Cyc_Lexing_lexbuf*))Cyc_Lexing_lexeme_start)(lbuf));
Cyc_yylloc.last_line=(yyllocptr->last_line=((int(*)(struct Cyc_Lexing_lexbuf*))Cyc_Lexing_lexeme_end)(lbuf));
switch(ans){case 360: _LL28:
# 793
{union Cyc_YYSTYPE _tmp1E2;*yylval=(((_tmp1E2.String_tok).val=Cyc_Lex_token_string,(((_tmp1E2.String_tok).tag=3,_tmp1E2))));}break;case 368: _LL29:
# 795
{union Cyc_YYSTYPE _tmp1E3;*yylval=(((_tmp1E3.String_tok).val=Cyc_Lex_token_string,(((_tmp1E3.String_tok).tag=3,_tmp1E3))));}break;case 369: _LL2A:
# 797
{union Cyc_YYSTYPE _tmp1E4;*yylval=(((_tmp1E4.QualId_tok).val=Cyc_Lex_token_qvar,(((_tmp1E4.QualId_tok).tag=5,_tmp1E4))));}break;case 370: _LL2B:
# 799
{union Cyc_YYSTYPE _tmp1E5;*yylval=(((_tmp1E5.QualId_tok).val=Cyc_Lex_token_qvar,(((_tmp1E5.QualId_tok).tag=5,_tmp1E5))));}break;case 367: _LL2C:
# 801
{union Cyc_YYSTYPE _tmp1E6;*yylval=(((_tmp1E6.String_tok).val=Cyc_Lex_token_string,(((_tmp1E6.String_tok).tag=3,_tmp1E6))));}break;case 361: _LL2D:
# 803
{union Cyc_YYSTYPE _tmp1E7;*yylval=(((_tmp1E7.Int_tok).val=Cyc_Lex_token_int,(((_tmp1E7.Int_tok).tag=1,_tmp1E7))));}break;case 364: _LL2E:
# 805
{union Cyc_YYSTYPE _tmp1E8;*yylval=(((_tmp1E8.Char_tok).val=Cyc_Lex_token_char,(((_tmp1E8.Char_tok).tag=2,_tmp1E8))));}break;case 365: _LL2F:
# 807
{union Cyc_YYSTYPE _tmp1E9;*yylval=(((_tmp1E9.String_tok).val=Cyc_Lex_token_string,(((_tmp1E9.String_tok).tag=3,_tmp1E9))));}break;case 366: _LL30:
# 809
{union Cyc_YYSTYPE _tmp1EA;*yylval=(((_tmp1EA.String_tok).val=Cyc_Lex_token_string,(((_tmp1EA.String_tok).tag=3,_tmp1EA))));}break;case 362: _LL31:
# 811
{union Cyc_YYSTYPE _tmp1EB;*yylval=(((_tmp1EB.String_tok).val=Cyc_Lex_token_string,(((_tmp1EB.String_tok).tag=3,_tmp1EB))));}break;case 363: _LL32:
# 813
{union Cyc_YYSTYPE _tmp1EC;*yylval=(((_tmp1EC.String_tok).val=Cyc_Lex_token_string,(((_tmp1EC.String_tok).tag=3,_tmp1EC))));}break;case 372: _LL33:
# 815
{union Cyc_YYSTYPE _tmp1ED;*yylval=(((_tmp1ED.Asm_tok).val=Cyc_Lex_token_asm,(((_tmp1ED.Asm_tok).tag=6,_tmp1ED))));}break;default: _LL34:
 break;}
# 818
return ans;}
# 825
const int Cyc_Lex_lex_base[299]={0,24,117,119,123,132,134,139,150,157,- 8,- 7,4,- 1,5,- 4,- 5,2,- 6,- 2,6,- 3,7,- 7,163,- 1,12,23,38,21,34,35,47,201,174,10,12,21,- 15,- 16,11,- 1,195,- 17,- 11,- 10,215,- 12,- 9,- 4,- 5,268,349,223,- 13,- 16,- 3,- 2,12,2,- 16,- 17,13,- 1,373,- 18,14,291,446,469,299,- 14,15,- 1,16,524,307,597,672,389,- 27,45,53,60,53,81,57,52,58,63,79,69,79,747,86,88,84,822,93,98,100,897,104,96,85,104,90,121,122,114,122,972,119,111,126,113,1047,119,121,1122,116,127,129,138,132,126,136,1197,155,155,146,154,1272,147,158,146,168,166,182,170,176,1347,169,180,181,1422,174,1497,176,178,194,186,183,194,202,193,201,1572,186,190,205,192,1647,- 62,399,- 41,17,30,- 44,361,31,270,353,32,267,319,406,330,1724,1767,268,372,251,323,- 7,1837,1926,253,2001,2059,2134,269,- 61,- 25,- 31,2209,269,2284,2359,2417,2492,2567,274,2642,2717,2792,2867,2942,3017,3092,3167,- 26,- 45,- 46,315,- 29,317,- 34,- 28,- 37,324,- 38,3242,3271,540,350,377,550,3281,3311,1805,1734,3344,3375,3413,387,397,3483,3521,389,400,392,402,442,452,- 9,- 43,551,- 23,- 42,18,341,3453,- 36,- 19,- 21,- 35,- 20,- 22,1,3561,2,350,501,355,357,370,381,390,425,427,429,430,3634,3718,- 59,- 53,- 52,- 51,- 50,- 49,- 48,- 47,- 54,- 57,- 58,1753,440,- 55,- 56,- 60,- 30,- 24,552,- 39,19,- 15,- 41,599};
const int Cyc_Lex_lex_backtrk[299]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,- 1,7,- 1,- 1,7,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,5,3,4,- 1,- 1,14,- 1,17,- 1,- 1,- 1,12,- 1,- 1,- 1,- 1,- 1,13,12,- 1,- 1,- 1,- 1,1,3,- 1,- 1,15,- 1,18,- 1,1,13,- 1,14,13,- 1,1,- 1,14,17,12,- 1,13,12,- 1,26,26,26,26,26,26,- 1,- 1,- 1,- 1,- 1,- 1,14,- 1,- 1,- 1,20,- 1,- 1,- 1,13,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,17,- 1,- 1,- 1,- 1,24,- 1,- 1,18,- 1,- 1,- 1,- 1,- 1,- 1,- 1,19,- 1,- 1,- 1,- 1,23,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,15,- 1,- 1,- 1,22,- 1,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,16,- 1,- 1,- 1,- 1,25,- 1,39,- 1,40,61,- 1,61,61,61,61,61,61,61,61,61,8,10,61,61,61,61,- 1,2,2,61,61,61,2,61,- 1,- 1,- 1,2,- 1,2,0,- 1,4,4,- 1,5,3,3,3,3,3,3,1,- 1,- 1,- 1,32,- 1,31,- 1,- 1,- 1,- 1,- 1,12,10,- 1,10,10,- 1,11,12,- 1,- 1,12,8,9,8,8,- 1,7,7,7,9,9,8,8,- 1,- 1,- 1,- 1,- 1,41,- 1,12,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,39};
const int Cyc_Lex_lex_default[299]={163,80,38,49,60,49,38,34,- 1,10,0,0,- 1,0,20,0,0,- 1,0,0,20,0,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,34,- 1,- 1,- 1,0,0,- 1,0,- 1,0,0,0,- 1,0,0,0,0,- 1,- 1,- 1,0,0,0,0,- 1,- 1,0,0,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,- 1,0,293,- 1,- 1,259,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,0,- 1,0,0,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,247,0,0,- 1,- 1,- 1,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,0,0,0,0,- 1,- 1,0,0,0,0,0,293,0,- 1,0,0,- 1};
const int Cyc_Lex_lex_trans[3975]={0,0,0,0,0,0,0,0,0,164,165,164,164,166,23,21,21,21,22,22,18,55,19,43,19,19,55,297,249,294,0,0,164,167,168,169,25,170,171,172,290,289,173,174,19,175,176,177,178,179,179,179,179,179,179,179,179,179,180,19,181,182,183,25,184,185,185,185,185,185,185,185,185,185,185,185,186,185,185,185,185,185,185,185,185,185,185,185,185,185,185,296,292,258,187,188,189,190,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,27,191,81,82,39,56,57,74,28,72,61,83,29,62,30,84,31,85,56,57,32,39,58,86,40,33,18,146,56,35,73,102,148,41,63,87,24,24,24,24,24,56,149,88,11,41,89,12,120,24,24,24,24,24,121,94,90,91,36,24,92,- 1,95,37,- 1,93,98,25,13,14,96,97,24,99,15,16,100,101,117,112,25,17,106,107,103,59,75,33,33,33,33,33,64,- 1,104,108,105,109,- 1,110,111,113,114,42,115,116,44,118,119,133,33,45,122,123,128,134,124,125,19,126,46,46,46,46,46,46,46,46,127,129,130,131,132,192,142,47,135,136,- 1,- 1,53,53,53,53,53,53,53,53,54,54,54,54,54,54,54,54,137,- 1,26,138,139,140,141,143,48,144,145,147,158,19,21,150,151,152,49,153,154,155,156,157,159,160,50,161,162,291,18,65,23,71,10,211,51,52,52,52,52,52,52,52,52,52,52,219,199,257,220,193,80,199,52,52,52,52,52,52,70,70,70,70,70,70,70,70,71,71,71,71,71,71,71,71,79,79,79,79,79,79,79,79,- 1,254,52,52,52,52,52,52,294,246,43,295,- 1,215,247,218,65,255,256,221,19,43,214,66,253,- 1,288,43,248,- 1,194,284,21,283,52,52,52,52,52,52,52,52,52,52,47,298,282,298,298,44,18,52,52,52,52,52,52,281,67,67,67,67,67,67,67,67,280,- 1,298,216,55,217,44,54,54,54,54,54,54,54,54,54,260,52,52,52,52,52,52,251,44,252,252,252,252,252,252,252,252,252,252,279,45,278,44,277,276,21,49,48,48,10,50,10,45,45,285,0,0,0,18,0,44,0,23,0,10,0,48,0,68,69,69,69,69,69,69,69,69,69,69,48,48,10,0,10,45,45,69,69,69,69,69,69,69,69,69,69,69,69,69,69,69,69,245,245,0,0,0,0,0,69,69,69,69,69,69,285,0,0,69,69,69,69,69,69,286,286,286,286,286,286,286,286,0,44,245,245,249,294,45,250,295,69,69,69,69,69,69,76,76,76,76,76,76,76,76,0,0,0,227,0,227,0,47,228,228,228,228,228,228,228,228,228,228,228,228,228,228,228,228,228,228,228,228,298,- 1,298,298,0,0,0,0,48,- 1,0,0,0,19,21,0,0,0,49,0,0,0,0,298,0,0,50,0,0,0,18,0,23,0,10,0,77,78,78,78,78,78,78,78,78,78,78,0,0,0,0,0,0,0,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,0,0,0,0,0,0,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,0,0,0,0,0,0,0,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,0,0,0,0,0,0,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,19,19,19,19,19,19,19,19,19,19,0,0,- 1,- 1,0,0,0,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,0,0,0,0,19,0,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,10,10,10,10,10,10,10,10,10,10,0,0,0,0,0,0,0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,0,0,0,0,10,0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,25,25,25,25,25,25,25,25,25,25,0,0,0,0,0,0,0,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,0,0,0,0,25,0,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,50,50,50,50,50,50,50,50,50,50,0,0,0,0,0,0,0,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,0,0,0,0,50,0,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,47,47,47,47,47,47,47,47,47,47,0,0,0,0,0,0,0,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,0,0,0,0,47,0,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,18,18,18,18,18,18,18,18,18,18,0,0,0,0,0,0,0,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,0,0,0,0,18,0,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,23,23,23,23,23,23,23,23,23,23,0,0,0,0,0,0,0,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,0,0,0,0,23,0,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,44,44,44,44,44,44,44,44,44,44,0,0,0,0,0,0,0,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,0,0,0,0,44,0,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,21,21,21,21,21,21,21,21,21,21,0,0,0,0,0,0,0,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,0,0,0,0,21,0,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,45,45,45,45,45,45,45,45,45,45,0,0,0,0,0,0,0,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,0,0,0,0,45,0,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,45,48,48,48,48,48,48,48,48,48,48,0,0,0,0,0,0,0,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,0,0,0,0,48,0,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,49,49,49,49,49,49,49,49,49,49,0,0,0,0,0,0,0,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,0,0,0,0,49,0,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,54,54,54,54,54,54,54,54,54,54,0,0,0,0,0,0,0,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,0,0,0,0,54,0,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,222,0,233,233,233,233,233,233,233,233,234,234,232,232,232,232,232,232,232,232,232,232,285,224,0,0,0,0,0,0,235,287,287,287,287,287,287,287,287,236,0,0,237,222,0,223,223,223,223,223,223,223,223,223,223,224,0,0,0,0,0,0,235,0,0,0,224,0,0,0,0,236,0,225,237,0,0,0,231,0,231,0,226,232,232,232,232,232,232,232,232,232,232,0,0,0,0,0,224,0,0,0,0,0,0,225,0,0,0,0,0,0,0,0,226,195,195,195,195,195,195,195,195,195,195,196,0,0,0,0,0,0,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,0,0,0,0,195,0,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,212,0,0,0,0,213,0,0,0,0,0,0,0,0,195,195,195,195,195,195,195,195,195,195,196,0,0,0,0,0,0,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,0,0,0,0,195,0,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,204,204,204,204,204,204,204,204,204,204,0,0,0,0,0,0,0,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,0,0,0,0,205,0,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,0,0,0,0,203,0,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,195,195,195,195,195,195,195,195,195,195,196,0,0,0,0,0,0,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,0,0,0,0,195,0,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,197,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,196,0,0,0,0,0,0,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,0,0,0,0,195,0,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,196,0,0,0,0,0,0,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,0,0,0,0,195,0,195,195,195,195,195,195,195,195,195,195,195,195,198,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,196,0,0,0,0,0,0,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,0,0,0,0,195,0,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,0,0,0,0,201,0,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,202,0,0,0,0,0,0,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,0,0,0,0,200,0,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,201,201,201,201,201,201,201,201,201,201,0,0,0,0,0,0,0,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,0,0,0,0,201,0,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,203,203,203,203,203,203,203,203,203,203,0,0,0,0,0,0,0,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,0,0,0,0,203,0,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,204,204,204,204,204,204,204,204,204,204,0,0,0,0,0,0,0,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,0,0,0,0,204,0,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,0,0,0,0,0,0,0,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,0,0,0,0,204,0,206,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,0,0,0,0,0,0,0,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,0,0,0,0,204,0,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,207,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,0,0,0,0,0,0,0,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,0,0,0,0,204,0,204,204,204,204,204,204,204,204,204,204,204,204,208,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,0,0,0,0,0,0,0,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,0,0,0,0,209,0,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,0,0,0,0,0,0,0,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,0,0,0,0,210,0,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,0,0,0,0,0,0,0,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,0,0,0,0,204,0,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,229,229,229,229,229,229,229,229,229,229,0,0,0,0,0,0,0,0,0,0,0,230,54,0,0,0,0,222,54,223,223,223,223,223,223,223,223,223,223,228,228,228,228,228,228,228,228,228,228,0,224,0,0,230,54,0,0,225,0,0,54,47,0,0,0,0,226,47,0,229,229,229,229,229,229,229,229,229,229,0,0,0,224,0,0,0,0,0,0,225,230,54,0,47,0,0,0,54,226,47,0,0,232,232,232,232,232,232,232,232,232,232,0,0,0,0,0,0,0,0,0,0,230,54,54,0,0,0,0,54,54,222,0,233,233,233,233,233,233,233,233,234,234,0,0,0,0,0,0,0,0,0,0,0,224,0,54,0,0,0,0,243,54,0,0,0,0,0,0,222,244,234,234,234,234,234,234,234,234,234,234,0,0,0,0,0,224,0,0,0,0,0,224,243,0,0,0,0,0,241,0,0,244,0,0,0,0,0,242,0,0,252,252,252,252,252,252,252,252,252,252,0,0,0,224,0,0,0,0,0,0,241,230,54,0,0,0,0,0,54,242,238,238,238,238,238,238,238,238,238,238,0,0,0,0,0,0,0,238,238,238,238,238,238,230,54,0,0,0,0,0,54,0,0,0,0,0,0,0,238,238,238,238,238,238,238,238,238,238,0,238,238,238,238,238,238,238,238,238,238,238,238,0,0,0,261,0,239,0,0,262,0,0,0,0,0,240,0,0,263,263,263,263,263,263,263,263,0,238,238,238,238,238,238,264,0,0,0,0,239,0,0,0,0,0,0,0,0,240,0,0,0,0,0,0,0,0,0,0,0,0,0,0,265,0,0,0,0,266,267,0,0,0,268,0,0,0,0,0,0,0,269,0,0,0,270,0,271,0,272,0,273,274,274,274,274,274,274,274,274,274,274,0,0,0,0,0,0,0,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,0,0,0,0,0,0,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,275,0,0,0,0,0,0,0,0,274,274,274,274,274,274,274,274,274,274,0,0,0,0,0,0,0,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,0,0,0,0,0,0,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const int Cyc_Lex_lex_check[3975]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,12,14,20,22,14,20,35,40,58,62,66,72,74,166,250,295,- 1,- 1,0,0,0,0,59,0,0,0,259,261,0,0,17,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,0,0,0,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,167,170,173,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,26,0,1,1,2,3,3,2,27,3,4,1,28,4,29,1,30,1,5,5,31,6,5,1,6,32,7,82,3,7,3,84,81,2,4,86,8,8,8,8,8,5,81,87,9,6,88,9,83,24,24,24,24,24,83,85,89,90,7,8,91,34,85,7,34,92,94,8,9,9,95,96,24,98,9,9,99,100,103,104,24,9,105,106,102,5,2,33,33,33,33,33,4,34,102,107,102,108,34,109,110,112,113,6,114,115,42,117,118,120,33,42,121,121,122,120,123,124,33,125,42,42,42,42,42,42,42,42,126,128,129,130,131,0,133,42,134,135,14,20,46,46,46,46,46,46,46,46,53,53,53,53,53,53,53,53,136,1,24,137,138,139,140,142,42,143,144,146,148,42,42,149,150,151,42,152,153,154,155,156,158,159,42,160,161,171,42,174,42,182,42,187,42,51,51,51,51,51,51,51,51,51,51,180,196,174,180,191,171,202,51,51,51,51,51,51,67,67,67,67,67,67,67,67,70,70,70,70,70,70,70,70,76,76,76,76,76,76,76,76,172,175,51,51,51,51,51,51,169,177,2,169,3,214,177,216,4,175,175,220,64,183,183,64,251,5,262,6,177,172,191,264,7,265,52,52,52,52,52,52,52,52,52,52,64,164,266,164,164,64,9,52,52,52,52,52,52,267,64,64,64,64,64,64,64,64,268,34,164,181,181,181,225,64,79,79,79,79,79,79,79,79,172,52,52,52,52,52,52,176,226,176,176,176,176,176,176,176,176,176,176,269,64,270,225,271,272,64,64,235,236,239,64,240,241,242,287,- 1,- 1,- 1,64,- 1,226,- 1,64,- 1,64,- 1,64,- 1,64,68,68,68,68,68,68,68,68,68,68,235,236,239,- 1,240,241,242,68,68,68,68,68,68,69,69,69,69,69,69,69,69,69,69,243,244,- 1,- 1,- 1,- 1,- 1,69,69,69,69,69,69,263,- 1,- 1,68,68,68,68,68,68,263,263,263,263,263,263,263,263,- 1,75,243,244,247,293,75,247,293,69,69,69,69,69,69,75,75,75,75,75,75,75,75,- 1,- 1,- 1,224,- 1,224,- 1,75,224,224,224,224,224,224,224,224,224,224,227,227,227,227,227,227,227,227,227,227,298,172,298,298,- 1,- 1,- 1,- 1,75,169,- 1,- 1,- 1,75,75,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,298,- 1,- 1,75,- 1,- 1,- 1,75,- 1,75,- 1,75,- 1,75,77,77,77,77,77,77,77,77,77,77,- 1,- 1,- 1,- 1,- 1,- 1,- 1,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,- 1,- 1,- 1,- 1,- 1,- 1,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,78,78,78,78,78,78,78,78,78,78,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,- 1,- 1,- 1,- 1,- 1,- 1,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,93,93,93,93,93,93,93,93,93,93,- 1,- 1,247,293,- 1,- 1,- 1,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,- 1,- 1,- 1,- 1,93,- 1,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,97,97,97,97,97,97,97,97,97,97,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,- 1,- 1,- 1,- 1,97,- 1,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,101,101,101,101,101,101,101,101,101,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,- 1,- 1,- 1,- 1,101,- 1,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,111,111,111,111,111,111,111,111,111,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,- 1,- 1,- 1,- 1,111,- 1,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,116,116,116,116,116,116,116,116,116,116,- 1,- 1,- 1,- 1,- 1,- 1,- 1,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,- 1,- 1,- 1,- 1,116,- 1,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,119,119,119,119,119,119,119,119,119,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,- 1,- 1,- 1,- 1,119,- 1,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,127,127,127,127,127,127,127,127,127,127,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,- 1,- 1,- 1,- 1,127,- 1,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,132,132,132,132,132,132,132,132,132,132,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,- 1,- 1,- 1,- 1,132,- 1,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,141,141,141,141,141,141,141,141,141,141,- 1,- 1,- 1,- 1,- 1,- 1,- 1,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,- 1,- 1,- 1,- 1,141,- 1,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,145,145,145,145,145,145,145,145,145,145,- 1,- 1,- 1,- 1,- 1,- 1,- 1,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,- 1,- 1,- 1,- 1,145,- 1,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,147,147,147,147,147,147,147,147,147,147,- 1,- 1,- 1,- 1,- 1,- 1,- 1,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,- 1,- 1,- 1,- 1,147,- 1,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,157,157,157,157,157,157,157,157,157,157,- 1,- 1,- 1,- 1,- 1,- 1,- 1,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,- 1,- 1,- 1,- 1,157,- 1,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,162,162,162,162,162,162,162,162,162,162,- 1,- 1,- 1,- 1,- 1,- 1,- 1,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,- 1,- 1,- 1,- 1,162,- 1,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,178,- 1,178,178,178,178,178,178,178,178,178,178,231,231,231,231,231,231,231,231,231,231,286,178,- 1,- 1,- 1,- 1,- 1,- 1,178,286,286,286,286,286,286,286,286,178,- 1,- 1,178,179,- 1,179,179,179,179,179,179,179,179,179,179,178,- 1,- 1,- 1,- 1,- 1,- 1,178,- 1,- 1,- 1,179,- 1,- 1,- 1,- 1,178,- 1,179,178,- 1,- 1,- 1,230,- 1,230,- 1,179,230,230,230,230,230,230,230,230,230,230,- 1,- 1,- 1,- 1,- 1,179,- 1,- 1,- 1,- 1,- 1,- 1,179,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,179,185,185,185,185,185,185,185,185,185,185,185,- 1,- 1,- 1,- 1,- 1,- 1,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,- 1,- 1,- 1,- 1,185,- 1,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,185,186,- 1,- 1,- 1,- 1,186,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,186,186,186,186,186,186,186,186,186,186,186,- 1,- 1,- 1,- 1,- 1,- 1,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,- 1,- 1,- 1,- 1,186,- 1,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,186,188,188,188,188,188,188,188,188,188,188,- 1,- 1,- 1,- 1,- 1,- 1,- 1,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,- 1,- 1,- 1,- 1,188,- 1,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,188,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,- 1,- 1,- 1,- 1,189,- 1,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,190,190,190,190,190,190,190,190,190,190,190,- 1,- 1,- 1,- 1,- 1,- 1,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,- 1,- 1,- 1,- 1,190,- 1,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190,195,195,195,195,195,195,195,195,195,195,195,- 1,- 1,- 1,- 1,- 1,- 1,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,- 1,- 1,- 1,- 1,195,- 1,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,195,197,197,197,197,197,197,197,197,197,197,197,- 1,- 1,- 1,- 1,- 1,- 1,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,- 1,- 1,- 1,- 1,197,- 1,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,197,198,198,198,198,198,198,198,198,198,198,198,- 1,- 1,- 1,- 1,- 1,- 1,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,- 1,- 1,- 1,- 1,198,- 1,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,- 1,- 1,- 1,- 1,199,- 1,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,199,200,200,200,200,200,200,200,200,200,200,200,- 1,- 1,- 1,- 1,- 1,- 1,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,- 1,- 1,- 1,- 1,200,- 1,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,201,201,201,201,201,201,201,201,201,201,- 1,- 1,- 1,- 1,- 1,- 1,- 1,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,- 1,- 1,- 1,- 1,201,- 1,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,203,203,203,203,203,203,203,203,203,203,- 1,- 1,- 1,- 1,- 1,- 1,- 1,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,- 1,- 1,- 1,- 1,203,- 1,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,203,204,204,204,204,204,204,204,204,204,204,- 1,- 1,- 1,- 1,- 1,- 1,- 1,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,- 1,- 1,- 1,- 1,204,- 1,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,204,205,205,205,205,205,205,205,205,205,205,- 1,- 1,- 1,- 1,- 1,- 1,- 1,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,- 1,- 1,- 1,- 1,205,- 1,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,206,206,206,206,206,206,206,206,206,206,- 1,- 1,- 1,- 1,- 1,- 1,- 1,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,- 1,- 1,- 1,- 1,206,- 1,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,206,207,207,207,207,207,207,207,207,207,207,- 1,- 1,- 1,- 1,- 1,- 1,- 1,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,- 1,- 1,- 1,- 1,207,- 1,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,208,208,208,208,208,208,208,208,208,208,- 1,- 1,- 1,- 1,- 1,- 1,- 1,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,- 1,- 1,- 1,- 1,208,- 1,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,209,209,209,209,209,209,209,209,209,209,- 1,- 1,- 1,- 1,- 1,- 1,- 1,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,- 1,- 1,- 1,- 1,209,- 1,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,210,210,210,210,210,210,210,210,210,210,- 1,- 1,- 1,- 1,- 1,- 1,- 1,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,- 1,- 1,- 1,- 1,210,- 1,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,210,222,222,222,222,222,222,222,222,222,222,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,222,222,- 1,- 1,- 1,- 1,223,222,223,223,223,223,223,223,223,223,223,223,228,228,228,228,228,228,228,228,228,228,- 1,223,- 1,- 1,222,222,- 1,- 1,223,- 1,- 1,222,228,- 1,- 1,- 1,- 1,223,228,- 1,229,229,229,229,229,229,229,229,229,229,- 1,- 1,- 1,223,- 1,- 1,- 1,- 1,- 1,- 1,223,229,229,- 1,228,- 1,- 1,- 1,229,223,228,- 1,- 1,232,232,232,232,232,232,232,232,232,232,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,229,229,232,- 1,- 1,- 1,- 1,229,232,233,- 1,233,233,233,233,233,233,233,233,233,233,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,233,- 1,232,- 1,- 1,- 1,- 1,233,232,- 1,- 1,- 1,- 1,- 1,- 1,234,233,234,234,234,234,234,234,234,234,234,234,- 1,- 1,- 1,- 1,- 1,233,- 1,- 1,- 1,- 1,- 1,234,233,- 1,- 1,- 1,- 1,- 1,234,- 1,- 1,233,- 1,- 1,- 1,- 1,- 1,234,- 1,- 1,252,252,252,252,252,252,252,252,252,252,- 1,- 1,- 1,234,- 1,- 1,- 1,- 1,- 1,- 1,234,252,252,- 1,- 1,- 1,- 1,- 1,252,234,237,237,237,237,237,237,237,237,237,237,- 1,- 1,- 1,- 1,- 1,- 1,- 1,237,237,237,237,237,237,252,252,- 1,- 1,- 1,- 1,- 1,252,- 1,- 1,- 1,- 1,- 1,- 1,- 1,238,238,238,238,238,238,238,238,238,238,- 1,237,237,237,237,237,237,238,238,238,238,238,238,- 1,- 1,- 1,260,- 1,238,- 1,- 1,260,- 1,- 1,- 1,- 1,- 1,238,- 1,- 1,260,260,260,260,260,260,260,260,- 1,238,238,238,238,238,238,260,- 1,- 1,- 1,- 1,238,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,238,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,260,- 1,- 1,- 1,- 1,260,260,- 1,- 1,- 1,260,- 1,- 1,- 1,- 1,- 1,- 1,- 1,260,- 1,- 1,- 1,260,- 1,260,- 1,260,- 1,260,273,273,273,273,273,273,273,273,273,273,- 1,- 1,- 1,- 1,- 1,- 1,- 1,273,273,273,273,273,273,273,273,273,273,273,273,273,273,273,273,273,273,273,273,273,273,273,273,273,273,- 1,- 1,- 1,- 1,- 1,- 1,273,273,273,273,273,273,273,273,273,273,273,273,273,273,273,273,273,273,273,273,273,273,273,273,273,273,274,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,274,274,274,274,274,274,274,274,274,274,- 1,- 1,- 1,- 1,- 1,- 1,- 1,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,- 1,- 1,- 1,- 1,- 1,- 1,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
int Cyc_Lex_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*lbuf){
# 832
int state;int base;int backtrk;
int c;
state=start_state;
# 836
if(state >= 0){
lbuf->lex_last_pos=(lbuf->lex_start_pos=lbuf->lex_curr_pos);
lbuf->lex_last_action=- 1;}else{
# 840
state=(- state)- 1;}
# 842
while(1){
base=Cyc_Lex_lex_base[_check_known_subscript_notnull(299,state)];
if(base < 0)return(- base)- 1;
backtrk=Cyc_Lex_lex_backtrk[_check_known_subscript_notnull(299,state)];
if(backtrk >= 0){
lbuf->lex_last_pos=lbuf->lex_curr_pos;
lbuf->lex_last_action=backtrk;}
# 850
if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){
if(!lbuf->lex_eof_reached)
return(- state)- 1;else{
# 854
c=256;}}else{
# 856
c=(int)*((char*)_check_dyneither_subscript(lbuf->lex_buffer,sizeof(char),lbuf->lex_curr_pos ++));
if(c == - 1)c=256;}
# 859
if(Cyc_Lex_lex_check[_check_known_subscript_notnull(3975,base + c)]== state)
state=Cyc_Lex_lex_trans[_check_known_subscript_notnull(3975,base + c)];else{
# 862
state=Cyc_Lex_lex_default[_check_known_subscript_notnull(299,state)];}
if(state < 0){
lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == - 1){
struct Cyc_Lexing_Error_exn_struct _tmp1F3;const char*_tmp1F2;struct Cyc_Lexing_Error_exn_struct*_tmp1F1;(int)_throw((void*)((_tmp1F1=_cycalloc(sizeof(*_tmp1F1)),((_tmp1F1[0]=((_tmp1F3.tag=Cyc_Lexing_Error,((_tmp1F3.f1=((_tmp1F2="empty token",_tag_dyneither(_tmp1F2,sizeof(char),12))),_tmp1F3)))),_tmp1F1)))));}else{
# 868
return lbuf->lex_last_action;}}else{
# 871
if(c == 256)lbuf->lex_eof_reached=0;}}}
# 875
int Cyc_Lex_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LL36:
# 828 "lex.cyl"
 return Cyc_Lex_snarf_asm(lexbuf);case 1: _LL37:
# 829 "lex.cyl"
 return Cyc_Lex_snarf_asm(lexbuf);case 2: _LL38:
# 832 "lex.cyl"
 return(int)Cyc_Lex_process_id(lexbuf);case 3: _LL39:
# 834
 return(int)Cyc_Lex_process_id(lexbuf);case 4: _LL3A:
# 838 "lex.cyl"
 return(int)Cyc_Lex_process_qual_id(lexbuf);case 5: _LL3B:
# 841 "lex.cyl"
 Cyc_Lex_token_string=*Cyc_Lex_get_symbol(Cyc_Lex_str_index_lbuf(lexbuf));
return 367;case 6: _LL3C:
# 844
 return Cyc_Lex_cyc_qual(lexbuf);case 7: _LL3D:
# 847 "lex.cyl"
 Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,2,0,16);
return 361;case 8: _LL3E:
# 850
 Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,8);
return 361;case 9: _LL3F:
# 856 "lex.cyl"
 Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,10);
return 361;case 10: _LL40:
# 859
 Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,10);
return 361;case 11: _LL41:
# 863 "lex.cyl"
 Cyc_Lex_token_string=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);
return 366;case 12: _LL42:
# 867 "lex.cyl"
 Cyc_Lex_token_string=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);
return 366;case 13: _LL43:
# 870
 return 343;case 14: _LL44:
# 871 "lex.cyl"
 return 344;case 15: _LL45:
# 872 "lex.cyl"
 return 341;case 16: _LL46:
# 873 "lex.cyl"
 return 342;case 17: _LL47:
# 874 "lex.cyl"
 return 337;case 18: _LL48:
# 875 "lex.cyl"
 return 338;case 19: _LL49:
# 876 "lex.cyl"
 return 350;case 20: _LL4A:
# 877 "lex.cyl"
 return 351;case 21: _LL4B:
# 878 "lex.cyl"
 return 347;case 22: _LL4C:
# 879 "lex.cyl"
 return 348;case 23: _LL4D:
# 880 "lex.cyl"
 return 349;case 24: _LL4E:
# 881 "lex.cyl"
 return 356;case 25: _LL4F:
# 882 "lex.cyl"
 return 355;case 26: _LL50:
# 883 "lex.cyl"
 return 354;case 27: _LL51:
# 884 "lex.cyl"
 return 352;case 28: _LL52:
# 885 "lex.cyl"
 return 353;case 29: _LL53:
# 886 "lex.cyl"
 return 345;case 30: _LL54:
# 887 "lex.cyl"
 return 346;case 31: _LL55:
# 888 "lex.cyl"
 return 339;case 32: _LL56:
# 890
 return 340;case 33: _LL57:
# 891 "lex.cyl"
 return 358;case 34: _LL58:
# 892 "lex.cyl"
 return 336;case 35: _LL59:
# 893 "lex.cyl"
 return 357;case 36: _LL5A:
# 894 "lex.cyl"
 return 359;case 37: _LL5B:
# 896
 return 309;case 38: _LL5C:
# 899 "lex.cyl"
 Cyc_Lex_process_directive(Cyc_Lexing_lexeme(lexbuf));return Cyc_Lex_token(lexbuf);case 39: _LL5D:
# 900 "lex.cyl"
 return Cyc_Lex_token(lexbuf);case 40: _LL5E:
# 901 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_token(lexbuf);case 41: _LL5F:
# 902 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_token(lexbuf);case 42: _LL60:
# 903 "lex.cyl"
 Cyc_Lex_comment_depth=1;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
Cyc_Lex_comment(lexbuf);
return Cyc_Lex_token(lexbuf);case 43: _LL61:
# 909 "lex.cyl"
 Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_strng(lexbuf)){
;}
Cyc_Lex_token_string=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();
return 362;case 44: _LL62:
# 917 "lex.cyl"
 Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_wstrng(lexbuf)){
;}
Cyc_Lex_token_string=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();
return 363;case 45: _LL63:
# 925 "lex.cyl"
 Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_scan_charconst(lexbuf)){
;}
Cyc_Lex_token_string=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();
return 365;case 46: _LL64:
# 932
 Cyc_Lex_token_char='\a';return 364;case 47: _LL65:
# 933 "lex.cyl"
 Cyc_Lex_token_char='\b';return 364;case 48: _LL66:
# 934 "lex.cyl"
 Cyc_Lex_token_char='\f';return 364;case 49: _LL67:
# 935 "lex.cyl"
 Cyc_Lex_token_char='\n';return 364;case 50: _LL68:
# 936 "lex.cyl"
 Cyc_Lex_token_char='\r';return 364;case 51: _LL69:
# 937 "lex.cyl"
 Cyc_Lex_token_char='\t';return 364;case 52: _LL6A:
# 938 "lex.cyl"
 Cyc_Lex_token_char='\v';return 364;case 53: _LL6B:
# 939 "lex.cyl"
 Cyc_Lex_token_char='\\';return 364;case 54: _LL6C:
# 940 "lex.cyl"
 Cyc_Lex_token_char='\'';return 364;case 55: _LL6D:
# 941 "lex.cyl"
 Cyc_Lex_token_char='"';return 364;case 56: _LL6E:
# 942 "lex.cyl"
 Cyc_Lex_token_char='?';return 364;case 57: _LL6F:
# 945 "lex.cyl"
 Cyc_Lex_token_char=(char)(Cyc_Lex_intconst(lexbuf,2,1,8)).f2;
return 364;case 58: _LL70:
# 950 "lex.cyl"
 Cyc_Lex_token_char=(char)(Cyc_Lex_intconst(lexbuf,3,1,16)).f2;
return 364;case 59: _LL71:
# 954
 Cyc_Lex_token_char=Cyc_Lexing_lexeme_char(lexbuf,1);
return 364;case 60: _LL72:
# 958
 return - 1;case 61: _LL73:
# 960
 return(int)Cyc_Lexing_lexeme_char(lexbuf,0);default: _LL74:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_token_rec(lexbuf,lexstate);}{
# 964
struct Cyc_Lexing_Error_exn_struct _tmp1F9;const char*_tmp1F8;struct Cyc_Lexing_Error_exn_struct*_tmp1F7;(int)_throw((void*)((_tmp1F7=_cycalloc(sizeof(*_tmp1F7)),((_tmp1F7[0]=((_tmp1F9.tag=Cyc_Lexing_Error,((_tmp1F9.f1=((_tmp1F8="some action didn't return!",_tag_dyneither(_tmp1F8,sizeof(char),27))),_tmp1F9)))),_tmp1F7)))));};}
# 966
int Cyc_Lex_token(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_token_rec(lexbuf,0);}
int Cyc_Lex_cyc_qual_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LL76:
# 967 "lex.cyl"
{const char*_tmp1FA;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp1FA="taggedZ",_tag_dyneither(_tmp1FA,sizeof(char),8))));}return(int)'@';case 1: _LL77:
# 969
{const char*_tmp1FB;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp1FB="zerotermZ",_tag_dyneither(_tmp1FB,sizeof(char),10))));}return(int)'@';case 2: _LL78:
# 971
{const char*_tmp1FC;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp1FC="nozerotermZ",_tag_dyneither(_tmp1FC,sizeof(char),12))));}return(int)'@';case 3: _LL79:
# 973
{const char*_tmp1FD;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp1FD="extensibleZ",_tag_dyneither(_tmp1FD,sizeof(char),12))));}return(int)'@';case 4: _LL7A:
# 975
{const char*_tmp1FE;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp1FE="resetableZ",_tag_dyneither(_tmp1FE,sizeof(char),11))));}return(int)'@';case 5: _LL7B:
# 977
{const char*_tmp1FF;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp1FF="regionZ",_tag_dyneither(_tmp1FF,sizeof(char),8))));}return(int)'@';case 6: _LL7C:
# 979
{const char*_tmp200;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp200="numeltsZ",_tag_dyneither(_tmp200,sizeof(char),9))));}return(int)'@';case 7: _LL7D:
# 981
{const char*_tmp201;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp201="thinZ",_tag_dyneither(_tmp201,sizeof(char),6))));}return(int)'@';case 8: _LL7E:
# 983
{const char*_tmp202;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp202="fatZ",_tag_dyneither(_tmp202,sizeof(char),5))));}return(int)'@';case 9: _LL7F:
# 985
{const char*_tmp203;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp203="notnullZ",_tag_dyneither(_tmp203,sizeof(char),9))));}return(int)'@';case 10: _LL80:
# 987
{const char*_tmp204;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp204="nullableZ",_tag_dyneither(_tmp204,sizeof(char),10))));}return(int)'@';case 11: _LL81:
# 989
{const char*_tmp205;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp205="requiresZ",_tag_dyneither(_tmp205,sizeof(char),10))));}return(int)'@';case 12: _LL82:
# 991
{const char*_tmp206;lexbuf->lex_curr_pos -=Cyc_strlen(((_tmp206="ensuresZ",_tag_dyneither(_tmp206,sizeof(char),9))));}return(int)'@';case 13: _LL83:
# 992 "lex.cyl"
 return 333;case 14: _LL84:
# 993 "lex.cyl"
 return 332;case 15: _LL85:
# 994 "lex.cyl"
 return 331;case 16: _LL86:
# 995 "lex.cyl"
 return 334;case 17: _LL87:
# 996 "lex.cyl"
 return 335;case 18: _LL88:
# 997 "lex.cyl"
 return 330;case 19: _LL89:
# 998 "lex.cyl"
 return 323;case 20: _LL8A:
# 999 "lex.cyl"
 return 324;case 21: _LL8B:
# 1000 "lex.cyl"
 return 325;case 22: _LL8C:
# 1001 "lex.cyl"
 return 326;case 23: _LL8D:
# 1002 "lex.cyl"
 return 327;case 24: _LL8E:
# 1003 "lex.cyl"
 return 328;case 25: _LL8F:
# 1004 "lex.cyl"
 return 329;case 26: _LL90:
# 1005 "lex.cyl"
 lexbuf->lex_curr_pos -=1;return(int)'@';default: _LL91:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_cyc_qual_rec(lexbuf,lexstate);}{
# 1009
struct Cyc_Lexing_Error_exn_struct _tmp20C;const char*_tmp20B;struct Cyc_Lexing_Error_exn_struct*_tmp20A;(int)_throw((void*)((_tmp20A=_cycalloc(sizeof(*_tmp20A)),((_tmp20A[0]=((_tmp20C.tag=Cyc_Lexing_Error,((_tmp20C.f1=((_tmp20B="some action didn't return!",_tag_dyneither(_tmp20B,sizeof(char),27))),_tmp20C)))),_tmp20A)))));};}
# 1011
int Cyc_Lex_cyc_qual(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_cyc_qual_rec(lexbuf,1);}
int Cyc_Lex_scan_charconst_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LL93:
# 1008 "lex.cyl"
 return 0;case 1: _LL94:
# 1010
 Cyc_Lex_store_string_char('\a');return 1;case 2: _LL95:
# 1011 "lex.cyl"
 Cyc_Lex_store_string_char('\b');return 1;case 3: _LL96:
# 1012 "lex.cyl"
 Cyc_Lex_store_string_char('\f');return 1;case 4: _LL97:
# 1013 "lex.cyl"
 Cyc_Lex_store_string_char('\n');return 1;case 5: _LL98:
# 1014 "lex.cyl"
 Cyc_Lex_store_string_char('\r');return 1;case 6: _LL99:
# 1015 "lex.cyl"
 Cyc_Lex_store_string_char('\t');return 1;case 7: _LL9A:
# 1016 "lex.cyl"
 Cyc_Lex_store_string_char('\v');return 1;case 8: _LL9B:
# 1017 "lex.cyl"
 Cyc_Lex_store_string_char('\\');return 1;case 9: _LL9C:
# 1018 "lex.cyl"
 Cyc_Lex_store_string_char('\'');return 1;case 10: _LL9D:
# 1019 "lex.cyl"
 Cyc_Lex_store_string_char('"');return 1;case 11: _LL9E:
# 1020 "lex.cyl"
 Cyc_Lex_store_string_char('?');return 1;case 12: _LL9F:
# 1023 "lex.cyl"
 Cyc_Lex_store_string((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 13: _LLA0:
# 1026 "lex.cyl"
 Cyc_Lex_store_string((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 14: _LLA1:
# 1028
 Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(lexbuf,0));return 1;case 15: _LLA2:
# 1030
 Cyc_Lex_inc_linenumber();{const char*_tmp20D;Cyc_Lex_runaway_err(((_tmp20D="wide character ends in newline",_tag_dyneither(_tmp20D,sizeof(char),31))),lexbuf);}return 0;case 16: _LLA3:
# 1031 "lex.cyl"
{const char*_tmp20E;Cyc_Lex_runaway_err(((_tmp20E="unterminated wide character",_tag_dyneither(_tmp20E,sizeof(char),28))),lexbuf);}return 0;case 17: _LLA4:
# 1032 "lex.cyl"
{const char*_tmp20F;Cyc_Lex_err(((_tmp20F="bad character following backslash in wide character",_tag_dyneither(_tmp20F,sizeof(char),52))),lexbuf);}return 1;default: _LLA5:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_scan_charconst_rec(lexbuf,lexstate);}{
# 1036
struct Cyc_Lexing_Error_exn_struct _tmp215;const char*_tmp214;struct Cyc_Lexing_Error_exn_struct*_tmp213;(int)_throw((void*)((_tmp213=_cycalloc(sizeof(*_tmp213)),((_tmp213[0]=((_tmp215.tag=Cyc_Lexing_Error,((_tmp215.f1=((_tmp214="some action didn't return!",_tag_dyneither(_tmp214,sizeof(char),27))),_tmp215)))),_tmp213)))));};}
# 1038
int Cyc_Lex_scan_charconst(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_scan_charconst_rec(lexbuf,2);}
int Cyc_Lex_strng_next_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLA7:
# 1037 "lex.cyl"
 return 1;case 1: _LLA8:
# 1038 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_strng_next(lexbuf);case 2: _LLA9:
# 1039 "lex.cyl"
 return Cyc_Lex_strng_next(lexbuf);case 3: _LLAA:
# 1041
 lexbuf->lex_curr_pos -=1;return 0;default: _LLAB:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_strng_next_rec(lexbuf,lexstate);}{
# 1045
struct Cyc_Lexing_Error_exn_struct _tmp21B;const char*_tmp21A;struct Cyc_Lexing_Error_exn_struct*_tmp219;(int)_throw((void*)((_tmp219=_cycalloc(sizeof(*_tmp219)),((_tmp219[0]=((_tmp21B.tag=Cyc_Lexing_Error,((_tmp21B.f1=((_tmp21A="some action didn't return!",_tag_dyneither(_tmp21A,sizeof(char),27))),_tmp21B)))),_tmp219)))));};}
# 1047
int Cyc_Lex_strng_next(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_strng_next_rec(lexbuf,3);}
int Cyc_Lex_strng_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLAD:
# 1045 "lex.cyl"
 return Cyc_Lex_strng_next(lexbuf);case 1: _LLAE:
# 1046 "lex.cyl"
 Cyc_Lex_inc_linenumber();return 1;case 2: _LLAF:
# 1047 "lex.cyl"
 Cyc_Lex_store_string_char('\a');return 1;case 3: _LLB0:
# 1048 "lex.cyl"
 Cyc_Lex_store_string_char('\b');return 1;case 4: _LLB1:
# 1049 "lex.cyl"
 Cyc_Lex_store_string_char('\f');return 1;case 5: _LLB2:
# 1050 "lex.cyl"
 Cyc_Lex_store_string_char('\n');return 1;case 6: _LLB3:
# 1051 "lex.cyl"
 Cyc_Lex_store_string_char('\r');return 1;case 7: _LLB4:
# 1052 "lex.cyl"
 Cyc_Lex_store_string_char('\t');return 1;case 8: _LLB5:
# 1053 "lex.cyl"
 Cyc_Lex_store_string_char('\v');return 1;case 9: _LLB6:
# 1054 "lex.cyl"
 Cyc_Lex_store_string_char('\\');return 1;case 10: _LLB7:
# 1055 "lex.cyl"
 Cyc_Lex_store_string_char('\'');return 1;case 11: _LLB8:
# 1056 "lex.cyl"
 Cyc_Lex_store_string_char('"');return 1;case 12: _LLB9:
# 1057 "lex.cyl"
 Cyc_Lex_store_string_char('?');return 1;case 13: _LLBA:
# 1060 "lex.cyl"
 Cyc_Lex_store_string_char((char)(Cyc_Lex_intconst(lexbuf,1,0,8)).f2);
return 1;case 14: _LLBB:
# 1065 "lex.cyl"
 Cyc_Lex_store_string_char((char)(Cyc_Lex_intconst(lexbuf,2,0,16)).f2);
return 1;case 15: _LLBC:
# 1070
 Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(lexbuf,0));
return 1;case 16: _LLBD:
# 1072 "lex.cyl"
 Cyc_Lex_inc_linenumber();
{const char*_tmp21C;Cyc_Lex_runaway_err(((_tmp21C="string ends in newline",_tag_dyneither(_tmp21C,sizeof(char),23))),lexbuf);}
return 0;case 17: _LLBE:
# 1076 "lex.cyl"
{const char*_tmp21D;Cyc_Lex_runaway_err(((_tmp21D="unterminated string",_tag_dyneither(_tmp21D,sizeof(char),20))),lexbuf);}
return 0;case 18: _LLBF:
# 1079 "lex.cyl"
{const char*_tmp21E;Cyc_Lex_err(((_tmp21E="bad character following backslash in string",_tag_dyneither(_tmp21E,sizeof(char),44))),lexbuf);}
return 1;default: _LLC0:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_strng_rec(lexbuf,lexstate);}{
# 1084
struct Cyc_Lexing_Error_exn_struct _tmp224;const char*_tmp223;struct Cyc_Lexing_Error_exn_struct*_tmp222;(int)_throw((void*)((_tmp222=_cycalloc(sizeof(*_tmp222)),((_tmp222[0]=((_tmp224.tag=Cyc_Lexing_Error,((_tmp224.f1=((_tmp223="some action didn't return!",_tag_dyneither(_tmp223,sizeof(char),27))),_tmp224)))),_tmp222)))));};}
# 1086
int Cyc_Lex_strng(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_strng_rec(lexbuf,4);}
int Cyc_Lex_wstrng_next_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLC2:
# 1089 "lex.cyl"
{const char*_tmp225;Cyc_Lex_store_string(((_tmp225="\" L\"",_tag_dyneither(_tmp225,sizeof(char),5))));}return 1;case 1: _LLC3:
# 1090 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_wstrng_next(lexbuf);case 2: _LLC4:
# 1091 "lex.cyl"
 return Cyc_Lex_wstrng_next(lexbuf);case 3: _LLC5:
# 1093
 lexbuf->lex_curr_pos -=1;return 0;default: _LLC6:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_wstrng_next_rec(lexbuf,lexstate);}{
# 1097
struct Cyc_Lexing_Error_exn_struct _tmp22B;const char*_tmp22A;struct Cyc_Lexing_Error_exn_struct*_tmp229;(int)_throw((void*)((_tmp229=_cycalloc(sizeof(*_tmp229)),((_tmp229[0]=((_tmp22B.tag=Cyc_Lexing_Error,((_tmp22B.f1=((_tmp22A="some action didn't return!",_tag_dyneither(_tmp22A,sizeof(char),27))),_tmp22B)))),_tmp229)))));};}
# 1099
int Cyc_Lex_wstrng_next(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_wstrng_next_rec(lexbuf,5);}
int Cyc_Lex_wstrng_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLC8:
# 1096 "lex.cyl"
 return Cyc_Lex_wstrng_next(lexbuf);case 1: _LLC9:
# 1097 "lex.cyl"
{const char*_tmp22C;Cyc_Lex_store_string(((_tmp22C="\\a",_tag_dyneither(_tmp22C,sizeof(char),3))));}return 1;case 2: _LLCA:
# 1098 "lex.cyl"
{const char*_tmp22D;Cyc_Lex_store_string(((_tmp22D="\\b",_tag_dyneither(_tmp22D,sizeof(char),3))));}return 1;case 3: _LLCB:
# 1099 "lex.cyl"
{const char*_tmp22E;Cyc_Lex_store_string(((_tmp22E="\\f",_tag_dyneither(_tmp22E,sizeof(char),3))));}return 1;case 4: _LLCC:
# 1100 "lex.cyl"
{const char*_tmp22F;Cyc_Lex_store_string(((_tmp22F="\\n",_tag_dyneither(_tmp22F,sizeof(char),3))));}return 1;case 5: _LLCD:
# 1101 "lex.cyl"
{const char*_tmp230;Cyc_Lex_store_string(((_tmp230="\\r",_tag_dyneither(_tmp230,sizeof(char),3))));}return 1;case 6: _LLCE:
# 1102 "lex.cyl"
{const char*_tmp231;Cyc_Lex_store_string(((_tmp231="\\t",_tag_dyneither(_tmp231,sizeof(char),3))));}return 1;case 7: _LLCF:
# 1103 "lex.cyl"
{const char*_tmp232;Cyc_Lex_store_string(((_tmp232="\\v",_tag_dyneither(_tmp232,sizeof(char),3))));}return 1;case 8: _LLD0:
# 1104 "lex.cyl"
{const char*_tmp233;Cyc_Lex_store_string(((_tmp233="\\\\",_tag_dyneither(_tmp233,sizeof(char),3))));}return 1;case 9: _LLD1:
# 1105 "lex.cyl"
{const char*_tmp234;Cyc_Lex_store_string(((_tmp234="\\'",_tag_dyneither(_tmp234,sizeof(char),3))));}return 1;case 10: _LLD2:
# 1106 "lex.cyl"
{const char*_tmp235;Cyc_Lex_store_string(((_tmp235="\\\"",_tag_dyneither(_tmp235,sizeof(char),3))));}return 1;case 11: _LLD3:
# 1107 "lex.cyl"
{const char*_tmp236;Cyc_Lex_store_string(((_tmp236="\\?",_tag_dyneither(_tmp236,sizeof(char),3))));}return 1;case 12: _LLD4:
# 1110 "lex.cyl"
 Cyc_Lex_store_string((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 13: _LLD5:
# 1115 "lex.cyl"
 Cyc_Lex_store_string((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 14: _LLD6:
# 1120
 Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(lexbuf,0));
return 1;case 15: _LLD7:
# 1122 "lex.cyl"
 Cyc_Lex_inc_linenumber();
{const char*_tmp237;Cyc_Lex_runaway_err(((_tmp237="string ends in newline",_tag_dyneither(_tmp237,sizeof(char),23))),lexbuf);}
return 0;case 16: _LLD8:
# 1126 "lex.cyl"
{const char*_tmp238;Cyc_Lex_runaway_err(((_tmp238="unterminated string",_tag_dyneither(_tmp238,sizeof(char),20))),lexbuf);}
return 0;case 17: _LLD9:
# 1129 "lex.cyl"
{const char*_tmp239;Cyc_Lex_err(((_tmp239="bad character following backslash in string",_tag_dyneither(_tmp239,sizeof(char),44))),lexbuf);}
return 1;default: _LLDA:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_wstrng_rec(lexbuf,lexstate);}{
# 1134
struct Cyc_Lexing_Error_exn_struct _tmp23F;const char*_tmp23E;struct Cyc_Lexing_Error_exn_struct*_tmp23D;(int)_throw((void*)((_tmp23D=_cycalloc(sizeof(*_tmp23D)),((_tmp23D[0]=((_tmp23F.tag=Cyc_Lexing_Error,((_tmp23F.f1=((_tmp23E="some action didn't return!",_tag_dyneither(_tmp23E,sizeof(char),27))),_tmp23F)))),_tmp23D)))));};}
# 1136
int Cyc_Lex_wstrng(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_wstrng_rec(lexbuf,6);}
int Cyc_Lex_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLDC:
# 1132 "lex.cyl"
 ++ Cyc_Lex_comment_depth;return Cyc_Lex_comment(lexbuf);case 1: _LLDD:
# 1133 "lex.cyl"
 -- Cyc_Lex_comment_depth;
if(Cyc_Lex_comment_depth > 0)
return Cyc_Lex_comment(lexbuf);
return 0;case 2: _LLDE:
# 1138 "lex.cyl"
{const char*_tmp240;Cyc_Lex_runaway_err(((_tmp240="unterminated comment",_tag_dyneither(_tmp240,sizeof(char),21))),lexbuf);}
return 0;case 3: _LLDF:
# 1141 "lex.cyl"
 return Cyc_Lex_comment(lexbuf);case 4: _LLE0:
# 1142 "lex.cyl"
 return Cyc_Lex_comment(lexbuf);case 5: _LLE1:
# 1143 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_comment(lexbuf);case 6: _LLE2:
# 1144 "lex.cyl"
 return Cyc_Lex_comment(lexbuf);default: _LLE3:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_comment_rec(lexbuf,lexstate);}{
# 1148
struct Cyc_Lexing_Error_exn_struct _tmp246;const char*_tmp245;struct Cyc_Lexing_Error_exn_struct*_tmp244;(int)_throw((void*)((_tmp244=_cycalloc(sizeof(*_tmp244)),((_tmp244[0]=((_tmp246.tag=Cyc_Lexing_Error,((_tmp246.f1=((_tmp245="some action didn't return!",_tag_dyneither(_tmp245,sizeof(char),27))),_tmp246)))),_tmp244)))));};}
# 1150
int Cyc_Lex_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_comment_rec(lexbuf,7);}
int Cyc_Lex_snarf_asm_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLE5:
# 1151 "lex.cyl"
 Cyc_Lex_token_asm.f1=0;
Cyc_Lex_paren_depth=1;
Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_snarf_asm_body(lexbuf)){;}
Cyc_Lex_token_asm.f2=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();
return 372;case 1: _LLE6:
# 1161 "lex.cyl"
 Cyc_Lex_token_asm.f1=1;
Cyc_Lex_paren_depth=1;
Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_snarf_asm_body(lexbuf)){;}
Cyc_Lex_token_asm.f2=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();
return 372;default: _LLE7:
# 1169
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_snarf_asm_rec(lexbuf,lexstate);}{
# 1172
struct Cyc_Lexing_Error_exn_struct _tmp24C;const char*_tmp24B;struct Cyc_Lexing_Error_exn_struct*_tmp24A;(int)_throw((void*)((_tmp24A=_cycalloc(sizeof(*_tmp24A)),((_tmp24A[0]=((_tmp24C.tag=Cyc_Lexing_Error,((_tmp24C.f1=((_tmp24B="some action didn't return!",_tag_dyneither(_tmp24B,sizeof(char),27))),_tmp24C)))),_tmp24A)))));};}
# 1174
int Cyc_Lex_snarf_asm(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_snarf_asm_rec(lexbuf,8);}
int Cyc_Lex_snarf_asm_body_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch(lexstate){case 0: _LLE9:
# 1171 "lex.cyl"
 Cyc_Lex_store_string_char('"');
while(Cyc_Lex_strng(lexbuf)){;}
Cyc_Lex_store_string_char('"');
return 1;case 1: _LLEA:
# 1175 "lex.cyl"
 Cyc_Lex_comment_depth=1;
Cyc_Lex_comment(lexbuf);
Cyc_Lex_store_string_char(' ');
return 1;case 2: _LLEB:
# 1179 "lex.cyl"
 Cyc_Lex_inc_linenumber();
Cyc_Lex_store_string_char('\n');return 1;case 3: _LLEC:
# 1181 "lex.cyl"
 Cyc_Lex_paren_depth +=1;Cyc_Lex_store_string_char('(');return 1;case 4: _LLED:
# 1182 "lex.cyl"
 Cyc_Lex_paren_depth -=1;
if(Cyc_Lex_paren_depth == 0)return 0;else{
# 1185
Cyc_Lex_store_string_char(')');
return 1;}case 5: _LLEE:
# 1188 "lex.cyl"
{const char*_tmp24D;Cyc_Lex_runaway_err(((_tmp24D="unterminated asm",_tag_dyneither(_tmp24D,sizeof(char),17))),lexbuf);}return 0;case 6: _LLEF:
# 1189 "lex.cyl"
 Cyc_Lex_inc_linenumber();Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(lexbuf,0));return 1;case 7: _LLF0:
# 1190 "lex.cyl"
 Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(lexbuf,0));return 1;default: _LLF1:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_snarf_asm_body_rec(lexbuf,lexstate);}{
# 1194
struct Cyc_Lexing_Error_exn_struct _tmp253;const char*_tmp252;struct Cyc_Lexing_Error_exn_struct*_tmp251;(int)_throw((void*)((_tmp251=_cycalloc(sizeof(*_tmp251)),((_tmp251[0]=((_tmp253.tag=Cyc_Lexing_Error,((_tmp253.f1=((_tmp252="some action didn't return!",_tag_dyneither(_tmp252,sizeof(char),27))),_tmp253)))),_tmp251)))));};}
# 1196
int Cyc_Lex_snarf_asm_body(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_snarf_asm_body_rec(lexbuf,9);}
# 1199 "lex.cyl"
void Cyc_Lex_pos_init(){
Cyc_Lex_linenumber=1;
Cyc_Lex_pos_info=0;}
# 1204
static struct Cyc_Xarray_Xarray*Cyc_Lex_empty_xarray(struct _RegionHandle*id_rgn,int dummy){
const char*_tmp256;struct _dyneither_ptr*_tmp255;struct Cyc_Xarray_Xarray*symbols=
((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*,int,struct _dyneither_ptr*))Cyc_Xarray_rcreate)(id_rgn,101,((_tmp255=_cycalloc(sizeof(*_tmp255)),((_tmp255[0]=(struct _dyneither_ptr)((_tmp256="",_tag_dyneither(_tmp256,sizeof(char),1))),_tmp255)))));
# 1208
((void(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add)(symbols,& Cyc_Lex_bogus_string);
return symbols;}void Cyc_Lex_lex_init(int include_cyclone_keywords);static void _tmp25E(unsigned int*_tmp25D,unsigned int*_tmp25C,int**_tmp25A){for(*_tmp25D=0;*_tmp25D < *_tmp25C;(*_tmp25D)++){(*_tmp25A)[*_tmp25D]=(int)0;}}
# 1212
void Cyc_Lex_lex_init(int include_cyclone_keywords){
# 1214
if(Cyc_Lex_ids_trie != 0){
struct Cyc_Lex_DynTrieSymbols*idt=0;
_swap_word(& idt,& Cyc_Lex_ids_trie);{
struct Cyc_Lex_DynTrieSymbols _tmp155=*((struct Cyc_Lex_DynTrieSymbols*)_check_null(idt));struct Cyc_Core_DynamicRegion*_tmp157;struct Cyc_Lex_DynTrieSymbols _tmp156=_tmp155;_tmp157=_tmp156.dyn;
Cyc_Core_free_ukey(_tmp157);
((void(*)(struct _dyneither_ptr ptr))Cyc_Core_ufree)(_tag_dyneither(idt,sizeof(struct Cyc_Lex_DynTrieSymbols),1));};}
# 1221
if(Cyc_Lex_typedefs_trie != 0){
struct Cyc_Lex_DynTrie*tdefs=0;
_swap_word(& tdefs,& Cyc_Lex_typedefs_trie);{
struct Cyc_Lex_DynTrie _tmp158=*((struct Cyc_Lex_DynTrie*)_check_null(tdefs));struct Cyc_Core_DynamicRegion*_tmp15A;struct Cyc_Lex_DynTrie _tmp159=_tmp158;_tmp15A=_tmp159.dyn;
Cyc_Core_free_ukey(_tmp15A);
((void(*)(struct _dyneither_ptr ptr))Cyc_Core_ufree)(_tag_dyneither(tdefs,sizeof(struct Cyc_Lex_DynTrie),1));};}{
# 1229
struct Cyc_Core_NewDynamicRegion _tmp15B=Cyc_Core_new_ukey();struct Cyc_Core_DynamicRegion*_tmp15D;struct Cyc_Core_NewDynamicRegion _tmp15C=_tmp15B;_tmp15D=_tmp15C.key;{
struct Cyc_Lex_Trie*_tmp15E=((struct Cyc_Lex_Trie*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Lex_Trie*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmp15D,0,Cyc_Lex_empty_trie);
struct Cyc_Xarray_Xarray*_tmp15F=((struct Cyc_Xarray_Xarray*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Xarray_Xarray*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmp15D,0,Cyc_Lex_empty_xarray);
{struct Cyc_Lex_DynTrieSymbols*_tmp257;Cyc_Lex_ids_trie=((_tmp257=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp257)),((_tmp257->dyn=(struct Cyc_Core_DynamicRegion*)_tmp15D,((_tmp257->t=(struct Cyc_Lex_Trie*)_tmp15E,((_tmp257->symbols=(struct Cyc_Xarray_Xarray*)_tmp15F,_tmp257))))))));}{
# 1234
struct Cyc_Core_NewDynamicRegion _tmp161=Cyc_Core_new_ukey();struct Cyc_Core_DynamicRegion*_tmp163;struct Cyc_Core_NewDynamicRegion _tmp162=_tmp161;_tmp163=_tmp162.key;{
struct Cyc_Lex_Trie*t=((struct Cyc_Lex_Trie*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Lex_Trie*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmp163,0,Cyc_Lex_empty_trie);
{struct Cyc_Lex_DynTrie*_tmp258;Cyc_Lex_typedefs_trie=((_tmp258=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp258)),((_tmp258->dyn=(struct Cyc_Core_DynamicRegion*)_tmp163,((_tmp258->t=(struct Cyc_Lex_Trie*)t,_tmp258))))));}
Cyc_Lex_num_kws=Cyc_Lex_num_keywords(include_cyclone_keywords);
{unsigned int _tmp25D;unsigned int _tmp25C;struct _dyneither_ptr _tmp25B;int*_tmp25A;unsigned int _tmp259;Cyc_Lex_kw_nums=((_tmp259=(unsigned int)Cyc_Lex_num_kws,((_tmp25A=(int*)_cycalloc_atomic(_check_times(sizeof(int),_tmp259)),((_tmp25B=_tag_dyneither(_tmp25A,sizeof(int),_tmp259),((((_tmp25C=_tmp259,_tmp25E(& _tmp25D,& _tmp25C,& _tmp25A))),_tmp25B))))))));}{
unsigned int i=0;
unsigned int rwsze=66;
{unsigned int j=0;for(0;j < rwsze;++ j){
if(include_cyclone_keywords  || (Cyc_Lex_rw_array[(int)j]).f3){
struct _dyneither_ptr _tmp169=(Cyc_Lex_rw_array[(int)j]).f1;
Cyc_Lex_str_index(_tmp169,0,(int)Cyc_strlen((struct _dyneither_ptr)_tmp169));
*((int*)_check_dyneither_subscript(Cyc_Lex_kw_nums,sizeof(int),(int)i))=(int)(Cyc_Lex_rw_array[(int)j]).f2;
++ i;}}}
# 1249
Cyc_Lex_typedef_init();
Cyc_Lex_comment_depth=0;};};};};};}
