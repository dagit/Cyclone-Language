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
typedef struct Cyc___cycFILE Cyc_FILE;
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;
# 232 "cycboot.h"
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 150 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 152
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;
# 187
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_region_key_t;
# 193
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_uregion_key_t;
# 198
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_rcregion_key_t;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 280 "core.h"
inline static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){
# 285
if(bd >> 20  || sz >> 12)
return 0;{
unsigned char*ptrbd=dyn.curr + bd * sz;
if(((ptrbd < dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
# 292
return 0;
return dyn.curr;};}extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};
# 57 "lexing.h"
typedef struct Cyc_Lexing_lexbuf*Cyc_Lexing_Lexbuf;struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};
# 64
typedef struct Cyc_Lexing_function_lexbuf_state*Cyc_Lexing_Function_lexbuf_state;struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr lex_check;};
# 74
typedef struct Cyc_Lexing_lex_tables*Cyc_Lexing_LexTables;
# 80
struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
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
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 153
void*Cyc_List_fold_right(void*(*f)(void*,void*),struct Cyc_List_List*x,void*accum);
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 52
int Cyc_zstrcmp(struct _dyneither_ptr,struct _dyneither_ptr);
# 54
int Cyc_zstrptrcmp(struct _dyneither_ptr*,struct _dyneither_ptr*);
# 60
struct _dyneither_ptr Cyc_strcat(struct _dyneither_ptr dest,struct _dyneither_ptr src);
# 71
struct _dyneither_ptr Cyc_strcpy(struct _dyneither_ptr dest,struct _dyneither_ptr src);
# 107 "string.h"
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;
# 41
struct _dyneither_ptr Cyc_Position_string_of_segment(unsigned int);struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;
# 55
struct Cyc_Position_Error*Cyc_Position_mk_err_parse(unsigned int,struct _dyneither_ptr);extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
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
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 333
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 350
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 364
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 385
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 390
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 442 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 458
typedef void*Cyc_Absyn_funcparams_t;
# 461
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};
# 495
extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val;
extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val;
extern struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct Cyc_Absyn_Noreturn_att_val;
extern struct Cyc_Absyn_Const_att_Absyn_Attribute_struct Cyc_Absyn_Const_att_val;
extern struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct Cyc_Absyn_Packed_att_val;
# 502
extern struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct Cyc_Absyn_Shared_att_val;
extern struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Absyn_Unused_att_val;
extern struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct Cyc_Absyn_Weak_att_val;
extern struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct Cyc_Absyn_Dllimport_att_val;
extern struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct Cyc_Absyn_Dllexport_att_val;
extern struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct Cyc_Absyn_No_instrument_function_att_val;
extern struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct Cyc_Absyn_Constructor_att_val;
extern struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct Cyc_Absyn_Destructor_att_val;
extern struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct Cyc_Absyn_No_check_memory_usage_att_val;
extern struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct Cyc_Absyn_Pure_att_val;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 522
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 548
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 555
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 573
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 737 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};
# 905
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 932
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
# 936
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 940
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 950
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;
# 955
void*Cyc_Absyn_compress_kb(void*);
# 960
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 962
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_typ(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 965
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 967
extern void*Cyc_Absyn_sint_typ;
# 969
void*Cyc_Absyn_float_typ(int);
# 1017
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1021
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
# 1025
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1027
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
# 1031
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _dyneither_ptr s,unsigned int);
# 1039
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1047
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1056
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1066
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1068
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1084
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1092
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1097
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned int loc);
# 1110
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
# 1113
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned int);
# 1116
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1120
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,int,struct Cyc_Absyn_Exp*open_exp,unsigned int);
# 1125
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
# 1127
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);
# 1134
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1143
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1148
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1154
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts,struct Cyc_Absyn_Exp*requires_clause,struct Cyc_Absyn_Exp*ensures_clause);
# 1179
int Cyc_Absyn_fntype_att(void*a);
# 1199
extern int Cyc_Absyn_porting_c_code;struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
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
# 58
void*Cyc_Tcutil_copy_type(void*t);
# 71
void*Cyc_Tcutil_compress(void*t);
# 110
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 117
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 122
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 127
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
# 129
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
# 131
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
# 134
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
# 144
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 309 "tcutil.h"
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 373
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 68 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
# 74 "parse.y"
void Cyc_Lex_register_typedef(struct _tuple0*s);
void Cyc_Lex_enter_namespace(struct _dyneither_ptr*);
void Cyc_Lex_leave_namespace();
void Cyc_Lex_enter_using(struct _tuple0*);
void Cyc_Lex_leave_using();struct Cyc_Parse_FlatList{struct Cyc_Parse_FlatList*tl;void*hd[0] __attribute__((aligned )) ;};
# 94 "parse.y"
typedef struct Cyc_Parse_FlatList*Cyc_Parse_flat_list_t;
# 96
struct Cyc_Parse_FlatList*Cyc_Parse_flat_imp_rev(struct Cyc_Parse_FlatList*x){
if(x == 0)return x;else{
# 99
struct Cyc_Parse_FlatList*first=x;
struct Cyc_Parse_FlatList*second=x->tl;
x->tl=0;
while(second != 0){
struct Cyc_Parse_FlatList*temp=second->tl;
second->tl=first;
first=second;
second=temp;}
# 108
return first;}}
# 113
int Cyc_Parse_no_register=0;struct Cyc_Parse_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned int loc;};
# 126
typedef struct Cyc_Parse_Type_specifier Cyc_Parse_type_specifier_t;
# 128
enum Cyc_Parse_Storage_class{Cyc_Parse_Typedef_sc  = 0,Cyc_Parse_Extern_sc  = 1,Cyc_Parse_ExternC_sc  = 2,Cyc_Parse_Static_sc  = 3,Cyc_Parse_Auto_sc  = 4,Cyc_Parse_Register_sc  = 5,Cyc_Parse_Abstract_sc  = 6};
# 131
typedef enum Cyc_Parse_Storage_class Cyc_Parse_storage_class_t;struct Cyc_Parse_Declaration_spec{enum Cyc_Parse_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Parse_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};
# 140
typedef struct Cyc_Parse_Declaration_spec Cyc_Parse_decl_spec_t;struct Cyc_Parse_Declarator{struct _tuple0*id;struct Cyc_List_List*tms;};
# 146
typedef struct Cyc_Parse_Declarator Cyc_Parse_declarator_t;struct _tuple10{struct _tuple10*tl;struct Cyc_Parse_Declarator hd  __attribute__((aligned )) ;};
typedef struct _tuple10*Cyc_Parse_declarators_t;struct _tuple11{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple12{struct _tuple12*tl;struct _tuple11 hd  __attribute__((aligned )) ;};
typedef struct _tuple12*Cyc_Parse_declarator_list_t;
# 150
static void Cyc_Parse_decl_split(struct _RegionHandle*r,struct _tuple12*ds,struct _tuple10**decls,struct Cyc_List_List**es){
# 154
struct _tuple10*declarators=0;
struct Cyc_List_List*exprs=0;
for(0;ds != 0;ds=ds->tl){
struct _tuple11 _tmp0=ds->hd;struct Cyc_Parse_Declarator _tmp2;struct Cyc_Absyn_Exp*_tmp3;struct _tuple11 _tmp1=_tmp0;_tmp2=_tmp1.f1;_tmp3=_tmp1.f2;
{struct _tuple10*_tmp87A;declarators=((_tmp87A=_region_malloc(r,sizeof(*_tmp87A)),((_tmp87A->tl=declarators,((_tmp87A->hd=_tmp2,_tmp87A))))));}{
struct Cyc_List_List*_tmp87B;exprs=((_tmp87B=_region_malloc(r,sizeof(*_tmp87B)),((_tmp87B->hd=_tmp3,((_tmp87B->tl=exprs,_tmp87B))))));};}
# 161
*es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(exprs);
*decls=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(declarators);}struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};
# 168
typedef struct Cyc_Parse_Abstractdeclarator Cyc_Parse_abstractdeclarator_t;
# 171
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned int loc);struct _tuple13{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
# 173
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
# 178
static struct Cyc_List_List*Cyc_Parse_parse_result=0;
# 181
static void Cyc_Parse_err(struct _dyneither_ptr msg,unsigned int sg){
Cyc_Position_post_error(Cyc_Position_mk_err_parse(sg,msg));}
# 184
static void*Cyc_Parse_parse_abort(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 186
Cyc_Parse_err((struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap),sg);{
struct Cyc_Position_Exit_exn_struct _tmp87E;struct Cyc_Position_Exit_exn_struct*_tmp87D;(int)_throw((void*)((_tmp87D=_cycalloc_atomic(sizeof(*_tmp87D)),((_tmp87D[0]=((_tmp87E.tag=Cyc_Position_Exit,_tmp87E)),_tmp87D)))));};}
# 189
static void Cyc_Parse_unimp(struct _dyneither_ptr msg,unsigned int sg){
{const char*_tmp883;void*_tmp882[2];struct Cyc_String_pa_PrintArg_struct _tmp881;struct Cyc_String_pa_PrintArg_struct _tmp880;(_tmp880.tag=0,((_tmp880.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp881.tag=0,((_tmp881.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(sg)),((_tmp882[0]=& _tmp881,((_tmp882[1]=& _tmp880,Cyc_fprintf(Cyc_stderr,((_tmp883="%s: Warning: Cyclone does not yet support %s\n",_tag_dyneither(_tmp883,sizeof(char),46))),_tag_dyneither(_tmp882,sizeof(void*),2)))))))))))));}
return;}
# 195
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned int loc){
# 197
struct Cyc_Absyn_Tqual _tmpD;void*_tmpE;struct _tuple8*_tmpC=tqt;_tmpD=_tmpC->f2;_tmpE=_tmpC->f3;
if((_tmpD.print_const  || _tmpD.q_volatile) || _tmpD.q_restrict){
if(_tmpD.loc != 0)loc=_tmpD.loc;{
const char*_tmp886;void*_tmp885;(_tmp885=0,Cyc_Tcutil_warn(loc,((_tmp886="qualifier on type is ignored",_tag_dyneither(_tmp886,sizeof(char),29))),_tag_dyneither(_tmp885,sizeof(void*),0)));};}
# 202
return _tmpE;}struct _tuple14{union Cyc_Absyn_Constraint*f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;void*f4;};
# 205
static struct _tuple14 Cyc_Parse_collapse_pointer_quals(unsigned int loc,union Cyc_Absyn_Constraint*nullable,union Cyc_Absyn_Constraint*bound,void*rgn,struct Cyc_List_List*pqs){
# 211
union Cyc_Absyn_Constraint*zeroterm=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
for(0;pqs != 0;pqs=pqs->tl){
void*_tmp11=(void*)pqs->hd;void*_tmp12=_tmp11;struct Cyc_Absyn_Exp*_tmp1A;void*_tmp1C;_LL1: {struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmp13=(struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp13->tag != 4)goto _LL3;}_LL2:
# 215
 zeroterm=Cyc_Absyn_true_conref;goto _LL0;_LL3: {struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmp14=(struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp14->tag != 5)goto _LL5;}_LL4:
# 217
 zeroterm=Cyc_Absyn_false_conref;goto _LL0;_LL5: {struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct*_tmp15=(struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp15->tag != 7)goto _LL7;}_LL6:
# 219
 nullable=Cyc_Absyn_true_conref;goto _LL0;_LL7: {struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct*_tmp16=(struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp16->tag != 6)goto _LL9;}_LL8:
# 221
 nullable=Cyc_Absyn_false_conref;goto _LL0;_LL9: {struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct*_tmp17=(struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp17->tag != 3)goto _LLB;}_LLA:
# 223
 bound=Cyc_Absyn_bounds_dyneither_conref;goto _LL0;_LLB: {struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct*_tmp18=(struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp18->tag != 2)goto _LLD;}_LLC:
# 225
 bound=Cyc_Absyn_bounds_one_conref;goto _LL0;_LLD: {struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct*_tmp19=(struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp19->tag != 0)goto _LLF;else{_tmp1A=_tmp19->f1;}}_LLE:
# 227
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp889;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp888;bound=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp888=_cycalloc(sizeof(*_tmp888)),((_tmp888[0]=((_tmp889.tag=1,((_tmp889.f1=_tmp1A,_tmp889)))),_tmp888)))));}goto _LL0;_LLF: {struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmp1B=(struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp1B->tag != 1)goto _LL0;else{_tmp1C=(void*)_tmp1B->f1;}}_LL10:
# 229
 rgn=_tmp1C;goto _LL0;_LL0:;}{
# 232
struct _tuple14 _tmp88A;return(_tmp88A.f1=nullable,((_tmp88A.f2=bound,((_tmp88A.f3=zeroterm,((_tmp88A.f4=rgn,_tmp88A)))))));};}
# 238
struct _tuple0*Cyc_Parse_gensym_enum(){
# 240
static int enum_counter=0;
struct _dyneither_ptr*_tmp897;const char*_tmp896;void*_tmp895[1];struct Cyc_Int_pa_PrintArg_struct _tmp894;struct _tuple0*_tmp893;return(_tmp893=_cycalloc(sizeof(*_tmp893)),((_tmp893->f1=Cyc_Absyn_Rel_n(0),((_tmp893->f2=(
(_tmp897=_cycalloc(sizeof(*_tmp897)),((_tmp897[0]=(struct _dyneither_ptr)((_tmp894.tag=1,((_tmp894.f1=(unsigned long)enum_counter ++,((_tmp895[0]=& _tmp894,Cyc_aprintf(((_tmp896="__anonymous_enum_%d__",_tag_dyneither(_tmp896,sizeof(char),22))),_tag_dyneither(_tmp895,sizeof(void*),1)))))))),_tmp897)))),_tmp893)))));}struct _tuple15{struct _tuple0*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple16{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple17{struct _tuple15*f1;struct _tuple16*f2;};
# 245
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned int loc,struct _tuple17*field_info){
# 250
struct _tuple0*_tmp26;struct Cyc_Absyn_Tqual _tmp27;void*_tmp28;struct Cyc_List_List*_tmp29;struct Cyc_List_List*_tmp2A;struct Cyc_Absyn_Exp*_tmp2B;struct Cyc_Absyn_Exp*_tmp2C;struct _tuple17*_tmp25=field_info;_tmp26=(_tmp25->f1)->f1;_tmp27=(_tmp25->f1)->f2;_tmp28=(_tmp25->f1)->f3;_tmp29=(_tmp25->f1)->f4;_tmp2A=(_tmp25->f1)->f5;_tmp2B=(_tmp25->f2)->f1;_tmp2C=(_tmp25->f2)->f2;
if(_tmp29 != 0){
const char*_tmp898;Cyc_Parse_err(((_tmp898="bad type params in struct field",_tag_dyneither(_tmp898,sizeof(char),32))),loc);}
if(Cyc_Absyn_is_qvar_qualified(_tmp26)){
const char*_tmp899;Cyc_Parse_err(((_tmp899="struct or union field cannot be qualified with a namespace",_tag_dyneither(_tmp899,sizeof(char),59))),loc);}{
struct Cyc_Absyn_Aggrfield*_tmp89A;return(_tmp89A=_cycalloc(sizeof(*_tmp89A)),((_tmp89A->name=(*_tmp26).f2,((_tmp89A->tq=_tmp27,((_tmp89A->type=_tmp28,((_tmp89A->width=_tmp2B,((_tmp89A->attributes=_tmp2A,((_tmp89A->requires_clause=_tmp2C,_tmp89A)))))))))))));};}
# 260
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp89B;return(_tmp89B.Signed_spec=0,((_tmp89B.Unsigned_spec=0,((_tmp89B.Short_spec=0,((_tmp89B.Long_spec=0,((_tmp89B.Long_Long_spec=0,((_tmp89B.Valid_type_spec=0,((_tmp89B.Type_spec=Cyc_Absyn_sint_typ,((_tmp89B.loc=loc,_tmp89B)))))))))))))));}
# 271
static struct Cyc_Parse_Type_specifier Cyc_Parse_type_spec(void*t,unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp31=Cyc_Parse_empty_spec(loc);
_tmp31.Type_spec=t;
_tmp31.Valid_type_spec=1;
return _tmp31;}
# 277
static struct Cyc_Parse_Type_specifier Cyc_Parse_signed_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp32=Cyc_Parse_empty_spec(loc);
_tmp32.Signed_spec=1;
return _tmp32;}
# 282
static struct Cyc_Parse_Type_specifier Cyc_Parse_unsigned_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp33=Cyc_Parse_empty_spec(loc);
_tmp33.Unsigned_spec=1;
return _tmp33;}
# 287
static struct Cyc_Parse_Type_specifier Cyc_Parse_short_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp34=Cyc_Parse_empty_spec(loc);
_tmp34.Short_spec=1;
return _tmp34;}
# 292
static struct Cyc_Parse_Type_specifier Cyc_Parse_long_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp35=Cyc_Parse_empty_spec(loc);
_tmp35.Long_spec=1;
return _tmp35;}
# 299
static void*Cyc_Parse_array2ptr(void*t,int argposn){
# 301
return Cyc_Tcutil_is_array(t)?
Cyc_Tcutil_promote_array(t,argposn?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0):(void*)& Cyc_Absyn_HeapRgn_val,0): t;}struct _tuple18{struct _dyneither_ptr*f1;void*f2;};
# 314 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp36=0;
for(0;x != 0;x=x->tl){
struct _tuple8*_tmp37=(struct _tuple8*)x->hd;struct _tuple8*_tmp38=_tmp37;struct _dyneither_ptr*_tmp39;void*_tmp3B;struct _dyneither_ptr _tmp3C;void**_tmp3F;_LL12: _tmp39=_tmp38->f1;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp3A=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp38->f3;if(_tmp3A->tag != 19)goto _LL14;else{_tmp3B=(void*)_tmp3A->f1;}};if(!(_tmp39 != 0))goto _LL14;_LL13:
# 319
{void*_tmp40=_tmp3B;void**_tmp42;_LL19: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp41=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp40;if(_tmp41->tag != 1)goto _LL1B;else{_tmp42=(void**)((void**)& _tmp41->f2);}}_LL1A: {
# 323
struct Cyc_String_pa_PrintArg_struct _tmp8A3;void*_tmp8A2[1];const char*_tmp8A1;struct _dyneither_ptr*_tmp8A0;struct _dyneither_ptr*nm=(_tmp8A0=_cycalloc(sizeof(*_tmp8A0)),((_tmp8A0[0]=(struct _dyneither_ptr)((_tmp8A3.tag=0,((_tmp8A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp39),((_tmp8A2[0]=& _tmp8A3,Cyc_aprintf(((_tmp8A1="`%s",_tag_dyneither(_tmp8A1,sizeof(char),4))),_tag_dyneither(_tmp8A2,sizeof(void*),1)))))))),_tmp8A0)));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8B2;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp8B1;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp8B0;struct Cyc_Absyn_Tvar*_tmp8AF;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8AE;*_tmp42=(void*)((_tmp8AE=_cycalloc(sizeof(*_tmp8AE)),((_tmp8AE[0]=((_tmp8B2.tag=2,((_tmp8B2.f1=((_tmp8AF=_cycalloc(sizeof(*_tmp8AF)),((_tmp8AF->name=nm,((_tmp8AF->identity=- 1,((_tmp8AF->kind=(void*)((_tmp8B1=_cycalloc(sizeof(*_tmp8B1)),((_tmp8B1[0]=((_tmp8B0.tag=0,((_tmp8B0.f1=& Cyc_Tcutil_ik,_tmp8B0)))),_tmp8B1)))),_tmp8AF)))))))),_tmp8B2)))),_tmp8AE))));}
goto _LL18;}_LL1B:;_LL1C:
 goto _LL18;_LL18:;}
# 328
{struct _tuple18*_tmp8B5;struct Cyc_List_List*_tmp8B4;_tmp36=((_tmp8B4=_cycalloc(sizeof(*_tmp8B4)),((_tmp8B4->hd=((_tmp8B5=_cycalloc(sizeof(*_tmp8B5)),((_tmp8B5->f1=_tmp39,((_tmp8B5->f2=_tmp3B,_tmp8B5)))))),((_tmp8B4->tl=_tmp36,_tmp8B4))))));}goto _LL11;_LL14: if(_tmp38->f1 == 0)goto _LL16;_tmp3C=*_tmp38->f1;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp3D=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp38->f3;if(_tmp3D->tag != 15)goto _LL16;else{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)((void*)_tmp3D->f1);if(_tmp3E->tag != 1)goto _LL16;else{_tmp3F=(void**)((void**)& _tmp3E->f2);}}};_LL15: {
# 332
struct Cyc_String_pa_PrintArg_struct _tmp8BD;void*_tmp8BC[1];const char*_tmp8BB;struct _dyneither_ptr*_tmp8BA;struct _dyneither_ptr*nm=(_tmp8BA=_cycalloc(sizeof(*_tmp8BA)),((_tmp8BA[0]=(struct _dyneither_ptr)((_tmp8BD.tag=0,((_tmp8BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3C),((_tmp8BC[0]=& _tmp8BD,Cyc_aprintf(((_tmp8BB="`%s",_tag_dyneither(_tmp8BB,sizeof(char),4))),_tag_dyneither(_tmp8BC,sizeof(void*),1)))))))),_tmp8BA)));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8CC;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp8CB;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp8CA;struct Cyc_Absyn_Tvar*_tmp8C9;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8C8;*_tmp3F=(void*)((_tmp8C8=_cycalloc(sizeof(*_tmp8C8)),((_tmp8C8[0]=((_tmp8CC.tag=2,((_tmp8CC.f1=((_tmp8C9=_cycalloc(sizeof(*_tmp8C9)),((_tmp8C9->name=nm,((_tmp8C9->identity=- 1,((_tmp8C9->kind=(void*)((_tmp8CB=_cycalloc(sizeof(*_tmp8CB)),((_tmp8CB[0]=((_tmp8CA.tag=0,((_tmp8CA.f1=& Cyc_Tcutil_rk,_tmp8CA)))),_tmp8CB)))),_tmp8C9)))))))),_tmp8CC)))),_tmp8C8))));}
goto _LL11;}_LL16:;_LL17:
 goto _LL11;_LL11:;}
# 338
return _tmp36;}
# 342
static struct Cyc_List_List*Cyc_Parse_get_argrfield_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp57=0;
for(0;x != 0;x=x->tl){
void*_tmp58=((struct Cyc_Absyn_Aggrfield*)x->hd)->type;void*_tmp59=_tmp58;void*_tmp5B;_LL1E: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5A=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp59;if(_tmp5A->tag != 19)goto _LL20;else{_tmp5B=(void*)_tmp5A->f1;}}_LL1F:
# 347
{struct _tuple18*_tmp8CF;struct Cyc_List_List*_tmp8CE;_tmp57=((_tmp8CE=_cycalloc(sizeof(*_tmp8CE)),((_tmp8CE->hd=((_tmp8CF=_cycalloc(sizeof(*_tmp8CF)),((_tmp8CF->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,((_tmp8CF->f2=_tmp5B,_tmp8CF)))))),((_tmp8CE->tl=_tmp57,_tmp8CE))))));}goto _LL1D;_LL20:;_LL21:
 goto _LL1D;_LL1D:;}
# 351
return _tmp57;}
# 355
static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){
{void*_tmp5E=e->r;void*_tmp5F=_tmp5E;struct _dyneither_ptr*_tmp61;_LL23: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp60=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5F;if(_tmp60->tag != 1)goto _LL25;else{if((((_tmp60->f1)->f1).Rel_n).tag != 1)goto _LL25;if((struct Cyc_List_List*)(((_tmp60->f1)->f1).Rel_n).val != 0)goto _LL25;_tmp61=(_tmp60->f1)->f2;}}_LL24:
# 358
{struct Cyc_List_List*_tmp62=tags;for(0;_tmp62 != 0;_tmp62=_tmp62->tl){
struct _tuple18*_tmp63=(struct _tuple18*)_tmp62->hd;struct _dyneither_ptr*_tmp65;void*_tmp66;struct _tuple18*_tmp64=_tmp63;_tmp65=_tmp64->f1;_tmp66=_tmp64->f2;
if(Cyc_strptrcmp(_tmp65,_tmp61)== 0){
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp8D2;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp8D1;return Cyc_Absyn_new_exp((void*)((_tmp8D1=_cycalloc(sizeof(*_tmp8D1)),((_tmp8D1[0]=((_tmp8D2.tag=38,((_tmp8D2.f1=Cyc_Tcutil_copy_type(_tmp66),_tmp8D2)))),_tmp8D1)))),e->loc);}}}
# 363
goto _LL22;_LL25:;_LL26:
 goto _LL22;_LL22:;}
# 366
return e;}
# 371
static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){
{void*_tmp69=t;void*_tmp6B;struct Cyc_Absyn_Tqual _tmp6C;struct Cyc_Absyn_Exp*_tmp6D;union Cyc_Absyn_Constraint*_tmp6E;unsigned int _tmp6F;void*_tmp71;struct Cyc_Absyn_Tqual _tmp72;void*_tmp73;union Cyc_Absyn_Constraint*_tmp74;union Cyc_Absyn_Constraint*_tmp75;union Cyc_Absyn_Constraint*_tmp76;struct Cyc_Absyn_PtrLoc*_tmp77;_LL28: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp6A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp69;if(_tmp6A->tag != 8)goto _LL2A;else{_tmp6B=(_tmp6A->f1).elt_type;_tmp6C=(_tmp6A->f1).tq;_tmp6D=(_tmp6A->f1).num_elts;_tmp6E=(_tmp6A->f1).zero_term;_tmp6F=(_tmp6A->f1).zt_loc;}}_LL29: {
# 374
struct Cyc_Absyn_Exp*nelts2=_tmp6D;
if(_tmp6D != 0)
nelts2=Cyc_Parse_substitute_tags_exp(tags,_tmp6D);{
# 378
void*_tmp78=Cyc_Parse_substitute_tags(tags,_tmp6B);
if(_tmp6D != nelts2  || _tmp6B != _tmp78){
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp8D8;struct Cyc_Absyn_ArrayInfo _tmp8D7;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8D6;return(void*)((_tmp8D6=_cycalloc(sizeof(*_tmp8D6)),((_tmp8D6[0]=((_tmp8D8.tag=8,((_tmp8D8.f1=((_tmp8D7.elt_type=_tmp78,((_tmp8D7.tq=_tmp6C,((_tmp8D7.num_elts=nelts2,((_tmp8D7.zero_term=_tmp6E,((_tmp8D7.zt_loc=_tmp6F,_tmp8D7)))))))))),_tmp8D8)))),_tmp8D6))));}
goto _LL27;};}_LL2A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp70=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp69;if(_tmp70->tag != 5)goto _LL2C;else{_tmp71=(_tmp70->f1).elt_typ;_tmp72=(_tmp70->f1).elt_tq;_tmp73=((_tmp70->f1).ptr_atts).rgn;_tmp74=((_tmp70->f1).ptr_atts).nullable;_tmp75=((_tmp70->f1).ptr_atts).bounds;_tmp76=((_tmp70->f1).ptr_atts).zero_term;_tmp77=((_tmp70->f1).ptr_atts).ptrloc;}}_LL2B: {
# 383
void*_tmp7C=Cyc_Parse_substitute_tags(tags,_tmp71);
union Cyc_Absyn_Constraint*_tmp7D=_tmp75;
{union Cyc_Absyn_Constraint _tmp7E=*_tmp75;union Cyc_Absyn_Constraint _tmp7F=_tmp7E;struct Cyc_Absyn_Exp*_tmp81;_LL2F: if((_tmp7F.Eq_constr).tag != 1)goto _LL31;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp80=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)((void*)(_tmp7F.Eq_constr).val);if(_tmp80->tag != 1)goto _LL31;else{_tmp81=_tmp80->f1;}};_LL30:
# 387
 _tmp81=Cyc_Parse_substitute_tags_exp(tags,_tmp81);
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp8DE;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp8DD;union Cyc_Absyn_Constraint*_tmp8DC;_tmp7D=((_tmp8DC=_cycalloc(sizeof(*_tmp8DC)),(((_tmp8DC->Eq_constr).val=(void*)((_tmp8DE=_cycalloc(sizeof(*_tmp8DE)),((_tmp8DE[0]=((_tmp8DD.tag=1,((_tmp8DD.f1=_tmp81,_tmp8DD)))),_tmp8DE)))),(((_tmp8DC->Eq_constr).tag=1,_tmp8DC))))));}
goto _LL2E;_LL31:;_LL32:
 goto _LL2E;_LL2E:;}
# 392
if(_tmp7C != _tmp71  || _tmp7D != _tmp75){
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp8E4;struct Cyc_Absyn_PtrInfo _tmp8E3;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8E2;return(void*)((_tmp8E2=_cycalloc(sizeof(*_tmp8E2)),((_tmp8E2[0]=((_tmp8E4.tag=5,((_tmp8E4.f1=((_tmp8E3.elt_typ=_tmp7C,((_tmp8E3.elt_tq=_tmp72,((_tmp8E3.ptr_atts=(((_tmp8E3.ptr_atts).rgn=_tmp73,(((_tmp8E3.ptr_atts).nullable=_tmp74,(((_tmp8E3.ptr_atts).bounds=_tmp7D,(((_tmp8E3.ptr_atts).zero_term=_tmp76,(((_tmp8E3.ptr_atts).ptrloc=_tmp77,_tmp8E3.ptr_atts)))))))))),_tmp8E3)))))),_tmp8E4)))),_tmp8E2))));}
goto _LL27;}_LL2C:;_LL2D:
# 397
 goto _LL27;_LL27:;}
# 399
return t;}
# 404
static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,struct Cyc_Absyn_Aggrfield*x){
x->type=Cyc_Parse_substitute_tags(tags,x->type);}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};
# 411
static struct _tuple19*Cyc_Parse_get_tqual_typ(unsigned int loc,struct _tuple8*t){
# 413
struct _tuple19*_tmp8E5;return(_tmp8E5=_cycalloc(sizeof(*_tmp8E5)),((_tmp8E5->f1=(*t).f2,((_tmp8E5->f2=(*t).f3,_tmp8E5)))));}
# 416
static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d){
void*_tmp89=d->r;void*_tmp8A=_tmp89;struct Cyc_Absyn_Vardecl*_tmp8C;_LL34: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp8B=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8A;if(_tmp8B->tag != 0)goto _LL36;else{_tmp8C=_tmp8B->f1;}}_LL35:
 return _tmp8C;_LL36:;_LL37: {
const char*_tmp8E8;void*_tmp8E7;(_tmp8E7=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(d->loc,((_tmp8E8="bad declaration in `forarray' statement",_tag_dyneither(_tmp8E8,sizeof(char),40))),_tag_dyneither(_tmp8E7,sizeof(void*),0)));}_LL33:;}
# 423
static int Cyc_Parse_is_typeparam(void*tm){
void*_tmp8F=tm;_LL39: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp90=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp8F;if(_tmp90->tag != 4)goto _LL3B;}_LL3A:
 return 1;_LL3B:;_LL3C:
 return 0;_LL38:;}
# 432
static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k){
const char*_tmp8E9;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8E9="`H",_tag_dyneither(_tmp8E9,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_HeapRgn_val;else{
const char*_tmp8EA;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8EA="`U",_tag_dyneither(_tmp8EA,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_UniqueRgn_val;else{
const char*_tmp8EB;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8EB="`RC",_tag_dyneither(_tmp8EB,sizeof(char),4))))== 0)
return(void*)& Cyc_Absyn_RefCntRgn_val;else{
# 440
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8F5;struct _dyneither_ptr*_tmp8F4;struct Cyc_Absyn_Tvar*_tmp8F3;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8F2;return(void*)((_tmp8F2=_cycalloc(sizeof(*_tmp8F2)),((_tmp8F2[0]=((_tmp8F5.tag=2,((_tmp8F5.f1=((_tmp8F3=_cycalloc(sizeof(*_tmp8F3)),((_tmp8F3->name=((_tmp8F4=_cycalloc(sizeof(*_tmp8F4)),((_tmp8F4[0]=s,_tmp8F4)))),((_tmp8F3->identity=- 1,((_tmp8F3->kind=k,_tmp8F3)))))))),_tmp8F5)))),_tmp8F2))));}}}}
# 443
static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t){
void*k;
{void*_tmp98=Cyc_Absyn_compress_kb(t->kind);void*_tmp99=_tmp98;struct Cyc_Absyn_Kind*_tmp9C;void*_tmp9D;_LL3E: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp9A=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9A->tag != 1)goto _LL40;}_LL3F:
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp8F8;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp8F7;k=(void*)((_tmp8F7=_cycalloc(sizeof(*_tmp8F7)),((_tmp8F7[0]=((_tmp8F8.tag=1,((_tmp8F8.f1=0,_tmp8F8)))),_tmp8F7))));}goto _LL3D;_LL40: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9B->tag != 2)goto _LL42;else{_tmp9C=_tmp9B->f2;}}_LL41:
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp8FB;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8FA;k=(void*)((_tmp8FA=_cycalloc(sizeof(*_tmp8FA)),((_tmp8FA[0]=((_tmp8FB.tag=2,((_tmp8FB.f1=0,((_tmp8FB.f2=_tmp9C,_tmp8FB)))))),_tmp8FA))));}goto _LL3D;_LL42: _tmp9D=_tmp99;_LL43:
 k=_tmp9D;goto _LL3D;_LL3D:;}{
# 450
struct Cyc_Absyn_Tvar*_tmp8FC;return(_tmp8FC=_cycalloc(sizeof(*_tmp8FC)),((_tmp8FC->name=t->name,((_tmp8FC->identity=- 1,((_tmp8FC->kind=k,_tmp8FC)))))));};}
# 457
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned int loc,void*t){
void*_tmpA3=t;struct Cyc_Absyn_Tvar*_tmpA5;_LL45: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA4=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA3;if(_tmpA4->tag != 2)goto _LL47;else{_tmpA5=_tmpA4->f1;}}_LL46:
 return _tmpA5;_LL47:;_LL48: {
const char*_tmp8FF;void*_tmp8FE;(_tmp8FE=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8FF="expecting a list of type variables, not types",_tag_dyneither(_tmp8FF,sizeof(char),46))),_tag_dyneither(_tmp8FE,sizeof(void*),0)));}_LL44:;}
# 463
static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp902;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp901;return(void*)((_tmp901=_cycalloc(sizeof(*_tmp901)),((_tmp901[0]=((_tmp902.tag=2,((_tmp902.f1=pr,_tmp902)))),_tmp901))));}
# 468
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_tmpAA=Cyc_Tcutil_compress(t);void*_tmpAB=_tmpAA;void**_tmpAD;_LL4A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpAC=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpAB;if(_tmpAC->tag != 2)goto _LL4C;else{_tmpAD=(void**)&(_tmpAC->f1)->kind;}}_LL4B: {
# 471
void*_tmpAE=Cyc_Absyn_compress_kb(*_tmpAD);void*_tmpAF=_tmpAE;_LL4F: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpB0=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpAF;if(_tmpB0->tag != 1)goto _LL51;}_LL50:
# 473
 if(!leq)*_tmpAD=Cyc_Tcutil_kind_to_bound(k);else{
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp905;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp904;*_tmpAD=(void*)((_tmp904=_cycalloc(sizeof(*_tmp904)),((_tmp904[0]=((_tmp905.tag=2,((_tmp905.f1=0,((_tmp905.f2=k,_tmp905)))))),_tmp904))));}
return;_LL51:;_LL52:
 return;_LL4E:;}_LL4C:;_LL4D:
# 478
 return;_LL49:;}
# 483
static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(struct _RegionHandle*yy,struct Cyc_List_List*tms,struct Cyc_List_List*tds,unsigned int loc){
# 489
if(tds == 0)return tms;
# 494
if(tms == 0)return 0;{
# 496
void*_tmpB3=(void*)tms->hd;void*_tmpB4=_tmpB3;void*_tmpB6;_LL54: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpB5=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpB4;if(_tmpB5->tag != 3)goto _LL56;else{_tmpB6=(void*)_tmpB5->f1;}}_LL55:
# 499
 if(tms->tl == 0  || 
Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd) && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){
# 502
void*_tmpB7=_tmpB6;struct Cyc_List_List*_tmpBA;_LL59: {struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpB8=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpB7;if(_tmpB8->tag != 1)goto _LL5B;}_LL5A:
# 504
{const char*_tmp908;void*_tmp907;(_tmp907=0,Cyc_Tcutil_warn(loc,((_tmp908="function declaration with both new- and old-style parameter declarations; ignoring old-style",_tag_dyneither(_tmp908,sizeof(char),93))),_tag_dyneither(_tmp907,sizeof(void*),0)));}
# 506
return tms;_LL5B: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpB9=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpB7;if(_tmpB9->tag != 0)goto _LL58;else{_tmpBA=_tmpB9->f1;}}_LL5C:
# 508
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpBA)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds)){
const char*_tmp90B;void*_tmp90A;(_tmp90A=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp90B="wrong number of parameter declarations in old-style function declaration",_tag_dyneither(_tmp90B,sizeof(char),73))),_tag_dyneither(_tmp90A,sizeof(void*),0)));}{
# 512
struct Cyc_List_List*rev_new_params=0;
for(0;_tmpBA != 0;_tmpBA=_tmpBA->tl){
struct Cyc_List_List*_tmpBF=tds;
for(0;_tmpBF != 0;_tmpBF=_tmpBF->tl){
struct Cyc_Absyn_Decl*_tmpC0=(struct Cyc_Absyn_Decl*)_tmpBF->hd;
void*_tmpC1=_tmpC0->r;void*_tmpC2=_tmpC1;struct Cyc_Absyn_Vardecl*_tmpC4;_LL5E: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpC3=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpC2;if(_tmpC3->tag != 0)goto _LL60;else{_tmpC4=_tmpC3->f1;}}_LL5F:
# 519
 if(Cyc_zstrptrcmp((*_tmpC4->name).f2,(struct _dyneither_ptr*)_tmpBA->hd)!= 0)
continue;
if(_tmpC4->initializer != 0){
const char*_tmp90E;void*_tmp90D;(_tmp90D=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpC0->loc,((_tmp90E="initializer found in parameter declaration",_tag_dyneither(_tmp90E,sizeof(char),43))),_tag_dyneither(_tmp90D,sizeof(void*),0)));}
if(Cyc_Absyn_is_qvar_qualified(_tmpC4->name)){
const char*_tmp911;void*_tmp910;(_tmp910=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpC0->loc,((_tmp911="namespaces forbidden in parameter declarations",_tag_dyneither(_tmp911,sizeof(char),47))),_tag_dyneither(_tmp910,sizeof(void*),0)));}
{struct _tuple8*_tmp914;struct Cyc_List_List*_tmp913;rev_new_params=(
(_tmp913=_cycalloc(sizeof(*_tmp913)),((_tmp913->hd=((_tmp914=_cycalloc(sizeof(*_tmp914)),((_tmp914->f1=(*_tmpC4->name).f2,((_tmp914->f2=_tmpC4->tq,((_tmp914->f3=_tmpC4->type,_tmp914)))))))),((_tmp913->tl=rev_new_params,_tmp913))))));}
# 528
goto L;_LL60:;_LL61: {
const char*_tmp917;void*_tmp916;(_tmp916=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpC0->loc,((_tmp917="nonvariable declaration in parameter type",_tag_dyneither(_tmp917,sizeof(char),42))),_tag_dyneither(_tmp916,sizeof(void*),0)));}_LL5D:;}
# 532
L: if(_tmpBF == 0){
const char*_tmp91B;void*_tmp91A[1];struct Cyc_String_pa_PrintArg_struct _tmp919;(_tmp919.tag=0,((_tmp919.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmpBA->hd)),((_tmp91A[0]=& _tmp919,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp91B="%s is not given a type",_tag_dyneither(_tmp91B,sizeof(char),23))),_tag_dyneither(_tmp91A,sizeof(void*),1)))))));}}{
# 535
struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp92A;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp929;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp928;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp927;struct Cyc_List_List*_tmp926;return
(_tmp926=_region_malloc(yy,sizeof(*_tmp926)),((_tmp926->hd=(void*)((_tmp92A=_region_malloc(yy,sizeof(*_tmp92A)),((_tmp92A[0]=((_tmp927.tag=3,((_tmp927.f1=(void*)((_tmp929=_region_malloc(yy,sizeof(*_tmp929)),((_tmp929[0]=((_tmp928.tag=1,((_tmp928.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_new_params),((_tmp928.f2=0,((_tmp928.f3=0,((_tmp928.f4=0,((_tmp928.f5=0,((_tmp928.f6=0,((_tmp928.f7=0,_tmp928)))))))))))))))),_tmp929)))),_tmp927)))),_tmp92A)))),((_tmp926->tl=0,_tmp926)))));};};_LL58:;}
# 542
goto _LL57;_LL56:;_LL57: {
struct Cyc_List_List*_tmp92B;return(_tmp92B=_region_malloc(yy,sizeof(*_tmp92B)),((_tmp92B->hd=(void*)tms->hd,((_tmp92B->tl=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc),_tmp92B)))));}_LL53:;};}
# 550
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned int loc){
# 554
if(tds != 0){
struct Cyc_Parse_Declarator _tmp92C;d=((_tmp92C.id=d.id,((_tmp92C.tms=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc),_tmp92C))));}{
# 557
enum Cyc_Absyn_Scope sc=Cyc_Absyn_Public;
struct Cyc_Parse_Type_specifier tss=Cyc_Parse_empty_spec(loc);
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);
int is_inline=0;
struct Cyc_List_List*atts=0;
# 563
if(dso != 0){
tss=dso->type_specs;
tq=dso->tq;
is_inline=dso->is_inline;
atts=dso->attributes;
# 569
if(dso->sc != 0){
enum Cyc_Parse_Storage_class _tmpD7=*((enum Cyc_Parse_Storage_class*)_check_null(dso->sc));switch(_tmpD7){case Cyc_Parse_Extern_sc: _LL62:
 sc=Cyc_Absyn_Extern;break;case Cyc_Parse_Static_sc: _LL63:
 sc=Cyc_Absyn_Static;break;default: _LL64:
{const char*_tmp92D;Cyc_Parse_err(((_tmp92D="bad storage class on function",_tag_dyneither(_tmp92D,sizeof(char),30))),loc);}break;}}}{
# 576
void*_tmpD9=Cyc_Parse_collapse_type_specifiers(tss,loc);
struct _tuple13 _tmpDA=Cyc_Parse_apply_tms(tq,_tmpD9,atts,d.tms);struct Cyc_Absyn_Tqual _tmpDC;void*_tmpDD;struct Cyc_List_List*_tmpDE;struct Cyc_List_List*_tmpDF;struct _tuple13 _tmpDB=_tmpDA;_tmpDC=_tmpDB.f1;_tmpDD=_tmpDB.f2;_tmpDE=_tmpDB.f3;_tmpDF=_tmpDB.f4;
# 581
if(_tmpDE != 0){
# 584
const char*_tmp930;void*_tmp92F;(_tmp92F=0,Cyc_Tcutil_warn(loc,((_tmp930="bad type params, ignoring",_tag_dyneither(_tmp930,sizeof(char),26))),_tag_dyneither(_tmp92F,sizeof(void*),0)));}{
# 586
void*_tmpE2=_tmpDD;struct Cyc_List_List*_tmpE4;void*_tmpE5;struct Cyc_Absyn_Tqual _tmpE6;void*_tmpE7;struct Cyc_List_List*_tmpE8;int _tmpE9;struct Cyc_Absyn_VarargInfo*_tmpEA;struct Cyc_List_List*_tmpEB;struct Cyc_List_List*_tmpEC;struct Cyc_Absyn_Exp*_tmpED;struct Cyc_List_List*_tmpEE;struct Cyc_Absyn_Exp*_tmpEF;struct Cyc_List_List*_tmpF0;_LL67: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE3=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE2;if(_tmpE3->tag != 9)goto _LL69;else{_tmpE4=(_tmpE3->f1).tvars;_tmpE5=(_tmpE3->f1).effect;_tmpE6=(_tmpE3->f1).ret_tqual;_tmpE7=(_tmpE3->f1).ret_typ;_tmpE8=(_tmpE3->f1).args;_tmpE9=(_tmpE3->f1).c_varargs;_tmpEA=(_tmpE3->f1).cyc_varargs;_tmpEB=(_tmpE3->f1).rgn_po;_tmpEC=(_tmpE3->f1).attributes;_tmpED=(_tmpE3->f1).requires_clause;_tmpEE=(_tmpE3->f1).requires_relns;_tmpEF=(_tmpE3->f1).ensures_clause;_tmpF0=(_tmpE3->f1).ensures_relns;}}_LL68: {
# 590
struct Cyc_List_List*_tmpF1=0;
{struct Cyc_List_List*_tmpF2=_tmpE8;for(0;_tmpF2 != 0;_tmpF2=_tmpF2->tl){
struct _tuple8*_tmpF3=(struct _tuple8*)_tmpF2->hd;struct _dyneither_ptr*_tmpF5;struct Cyc_Absyn_Tqual _tmpF6;void*_tmpF7;struct _tuple8*_tmpF4=_tmpF3;_tmpF5=_tmpF4->f1;_tmpF6=_tmpF4->f2;_tmpF7=_tmpF4->f3;
if(_tmpF5 == 0){
{const char*_tmp931;Cyc_Parse_err(((_tmp931="missing argument variable in function prototype",_tag_dyneither(_tmp931,sizeof(char),48))),loc);}{
struct _tuple8*_tmp934;struct Cyc_List_List*_tmp933;_tmpF1=((_tmp933=_cycalloc(sizeof(*_tmp933)),((_tmp933->hd=((_tmp934=_cycalloc(sizeof(*_tmp934)),((_tmp934->f1=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"?",sizeof(char),2),((_tmp934->f2=_tmpF6,((_tmp934->f3=_tmpF7,_tmp934)))))))),((_tmp933->tl=_tmpF1,_tmp933))))));};}else{
# 597
struct _tuple8*_tmp937;struct Cyc_List_List*_tmp936;_tmpF1=((_tmp936=_cycalloc(sizeof(*_tmp936)),((_tmp936->hd=((_tmp937=_cycalloc(sizeof(*_tmp937)),((_tmp937->f1=_tmpF5,((_tmp937->f2=_tmpF6,((_tmp937->f3=_tmpF7,_tmp937)))))))),((_tmp936->tl=_tmpF1,_tmp936))))));}}}{
# 601
struct Cyc_Absyn_Fndecl*_tmp938;return(_tmp938=_cycalloc(sizeof(*_tmp938)),((_tmp938->sc=sc,((_tmp938->name=d.id,((_tmp938->tvs=_tmpE4,((_tmp938->is_inline=is_inline,((_tmp938->effect=_tmpE5,((_tmp938->ret_tqual=_tmpE6,((_tmp938->ret_type=_tmpE7,((_tmp938->args=
# 604
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpF1),((_tmp938->c_varargs=_tmpE9,((_tmp938->cyc_varargs=_tmpEA,((_tmp938->rgn_po=_tmpEB,((_tmp938->body=body,((_tmp938->cached_typ=0,((_tmp938->param_vardecls=0,((_tmp938->fn_vardecl=0,((_tmp938->attributes=
# 611
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpEC,_tmpDF),((_tmp938->requires_clause=_tmpED,((_tmp938->requires_relns=0,((_tmp938->ensures_clause=_tmpEF,((_tmp938->ensures_relns=0,_tmp938)))))))))))))))))))))))))))))))))))))))));};}_LL69:;_LL6A: {
# 616
const char*_tmp93B;void*_tmp93A;(_tmp93A=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp93B="declarator is not a function prototype",_tag_dyneither(_tmp93B,sizeof(char),39))),_tag_dyneither(_tmp93A,sizeof(void*),0)));}_LL66:;};};};}static char _tmp101[76]="at most one type may appear within a type specifier \n\t(missing ';' or ','?)";
# 620
static struct _dyneither_ptr Cyc_Parse_msg1={_tmp101,_tmp101,_tmp101 + 76};static char _tmp102[87]="const or volatile may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 622
static struct _dyneither_ptr Cyc_Parse_msg2={_tmp102,_tmp102,_tmp102 + 87};static char _tmp103[74]="type specifier includes more than one declaration \n\t(missing ';' or ','?)";
# 624
static struct _dyneither_ptr Cyc_Parse_msg3={_tmp103,_tmp103,_tmp103 + 74};static char _tmp104[84]="sign specifier may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 626
static struct _dyneither_ptr Cyc_Parse_msg4={_tmp104,_tmp104,_tmp104 + 84};
# 633
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned int loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){
# 636
if(s1.Signed_spec  && s2.Signed_spec){
void*_tmp93C;(_tmp93C=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp93C,sizeof(void*),0)));}
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec  && s2.Unsigned_spec){
void*_tmp93D;(_tmp93D=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp93D,sizeof(void*),0)));}
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec  && s2.Short_spec){
void*_tmp93E;(_tmp93E=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp93E,sizeof(void*),0)));}
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec  && s2.Long_Long_spec  || 
s1.Long_Long_spec  && s2.Long_spec) || 
s2.Long_Long_spec  && s1.Long_spec){
void*_tmp93F;(_tmp93F=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp93F,sizeof(void*),0)));}
s1.Long_Long_spec=
(s1.Long_Long_spec  || s2.Long_Long_spec) || s1.Long_spec  && s2.Long_spec;
s1.Long_spec=!s1.Long_Long_spec  && (s1.Long_spec  || s2.Long_spec);
if(s1.Valid_type_spec  && s2.Valid_type_spec)
Cyc_Parse_err(Cyc_Parse_msg1,loc);else{
if(s2.Valid_type_spec){
s1.Type_spec=s2.Type_spec;
s1.Valid_type_spec=1;}}
# 658
return s1;}
# 664
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned int loc){
# 667
int seen_type=ts.Valid_type_spec;
int seen_sign=ts.Signed_spec  || ts.Unsigned_spec;
int seen_size=(ts.Short_spec  || ts.Long_spec) || ts.Long_Long_spec;
void*t=seen_type?ts.Type_spec:(void*)& Cyc_Absyn_VoidType_val;
enum Cyc_Absyn_Size_of sz=Cyc_Absyn_Int_sz;
enum Cyc_Absyn_Sign sgn=Cyc_Absyn_Signed;
# 674
if(ts.Signed_spec  && ts.Unsigned_spec)
Cyc_Parse_err(Cyc_Parse_msg4,loc);
if(ts.Unsigned_spec)sgn=Cyc_Absyn_Unsigned;
if(ts.Short_spec  && (ts.Long_spec  || ts.Long_Long_spec) || 
ts.Long_spec  && ts.Long_Long_spec)
Cyc_Parse_err(Cyc_Parse_msg4,loc);
if(ts.Short_spec)sz=Cyc_Absyn_Short_sz;
if(ts.Long_spec)sz=Cyc_Absyn_Long_sz;
if(ts.Long_Long_spec)sz=Cyc_Absyn_LongLong_sz;
# 686
if(!seen_type){
if(!seen_sign  && !seen_size){
const char*_tmp942;void*_tmp941;(_tmp941=0,Cyc_Tcutil_warn(loc,((_tmp942="missing type within specifier",_tag_dyneither(_tmp942,sizeof(char),30))),_tag_dyneither(_tmp941,sizeof(void*),0)));}
t=Cyc_Absyn_int_typ(sgn,sz);}else{
# 691
if(seen_sign){
void*_tmp10B=t;enum Cyc_Absyn_Sign _tmp10D;enum Cyc_Absyn_Size_of _tmp10E;_LL6C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp10C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp10B;if(_tmp10C->tag != 6)goto _LL6E;else{_tmp10D=_tmp10C->f1;_tmp10E=_tmp10C->f2;}}_LL6D:
# 694
 if(_tmp10D != sgn)
t=Cyc_Absyn_int_typ(sgn,_tmp10E);
goto _LL6B;_LL6E:;_LL6F:
{const char*_tmp943;Cyc_Parse_err(((_tmp943="sign specification on non-integral type",_tag_dyneither(_tmp943,sizeof(char),40))),loc);}goto _LL6B;_LL6B:;}
# 699
if(seen_size){
void*_tmp110=t;enum Cyc_Absyn_Sign _tmp112;enum Cyc_Absyn_Size_of _tmp113;_LL71: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp111=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp110;if(_tmp111->tag != 6)goto _LL73;else{_tmp112=_tmp111->f1;_tmp113=_tmp111->f2;}}_LL72:
# 702
 if(_tmp113 != sz)
t=Cyc_Absyn_int_typ(_tmp112,sz);
goto _LL70;_LL73: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp114=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp110;if(_tmp114->tag != 7)goto _LL75;}_LL74:
# 706
 t=Cyc_Absyn_float_typ(2);goto _LL70;_LL75:;_LL76:
{const char*_tmp944;Cyc_Parse_err(((_tmp944="size qualifier on non-integral type",_tag_dyneither(_tmp944,sizeof(char),36))),loc);}goto _LL70;_LL70:;}}
# 710
return t;}
# 713
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple10*ds,struct Cyc_List_List*shared_atts){
# 717
if(ds == 0)return 0;{
struct Cyc_Parse_Declarator d=ds->hd;
struct _tuple0*_tmp116=d.id;
struct _tuple13 _tmp117=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);struct Cyc_Absyn_Tqual _tmp119;void*_tmp11A;struct Cyc_List_List*_tmp11B;struct Cyc_List_List*_tmp11C;struct _tuple13 _tmp118=_tmp117;_tmp119=_tmp118.f1;_tmp11A=_tmp118.f2;_tmp11B=_tmp118.f3;_tmp11C=_tmp118.f4;
# 723
if(ds->tl == 0){
struct _tuple15*_tmp947;struct Cyc_List_List*_tmp946;return(_tmp946=_region_malloc(r,sizeof(*_tmp946)),((_tmp946->hd=((_tmp947=_region_malloc(r,sizeof(*_tmp947)),((_tmp947->f1=_tmp116,((_tmp947->f2=_tmp119,((_tmp947->f3=_tmp11A,((_tmp947->f4=_tmp11B,((_tmp947->f5=_tmp11C,_tmp947)))))))))))),((_tmp946->tl=0,_tmp946)))));}else{
# 726
struct _tuple15*_tmp94A;struct Cyc_List_List*_tmp949;return(_tmp949=_region_malloc(r,sizeof(*_tmp949)),((_tmp949->hd=((_tmp94A=_region_malloc(r,sizeof(*_tmp94A)),((_tmp94A->f1=_tmp116,((_tmp94A->f2=_tmp119,((_tmp94A->f3=_tmp11A,((_tmp94A->f4=_tmp11B,((_tmp94A->f5=_tmp11C,_tmp94A)))))))))))),((_tmp949->tl=
Cyc_Parse_apply_tmss(r,tq,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts),_tmp949)))));}};}
# 730
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 733
if(tms == 0){struct _tuple13 _tmp94B;return(_tmp94B.f1=tq,((_tmp94B.f2=t,((_tmp94B.f3=0,((_tmp94B.f4=atts,_tmp94B)))))));}{
void*_tmp122=(void*)tms->hd;void*_tmp123=_tmp122;union Cyc_Absyn_Constraint*_tmp125;unsigned int _tmp126;struct Cyc_Absyn_Exp*_tmp128;union Cyc_Absyn_Constraint*_tmp129;unsigned int _tmp12A;void*_tmp12C;struct Cyc_List_List*_tmp12E;unsigned int _tmp12F;struct Cyc_Absyn_PtrAtts _tmp131;struct Cyc_Absyn_Tqual _tmp132;unsigned int _tmp134;struct Cyc_List_List*_tmp135;_LL78: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp124=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp123;if(_tmp124->tag != 0)goto _LL7A;else{_tmp125=_tmp124->f1;_tmp126=_tmp124->f2;}}_LL79:
# 736
 return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),
Cyc_Absyn_array_typ(t,tq,0,_tmp125,_tmp126),atts,tms->tl);_LL7A: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp127=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp123;if(_tmp127->tag != 1)goto _LL7C;else{_tmp128=_tmp127->f1;_tmp129=_tmp127->f2;_tmp12A=_tmp127->f3;}}_LL7B:
# 739
 return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),
Cyc_Absyn_array_typ(t,tq,_tmp128,_tmp129,_tmp12A),atts,tms->tl);_LL7C: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp12B=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp123;if(_tmp12B->tag != 3)goto _LL7E;else{_tmp12C=(void*)_tmp12B->f1;}}_LL7D: {
# 742
void*_tmp136=_tmp12C;struct Cyc_List_List*_tmp138;int _tmp139;struct Cyc_Absyn_VarargInfo*_tmp13A;void*_tmp13B;struct Cyc_List_List*_tmp13C;struct Cyc_Absyn_Exp*_tmp13D;struct Cyc_Absyn_Exp*_tmp13E;unsigned int _tmp140;_LL85: {struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp137=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp136;if(_tmp137->tag != 1)goto _LL87;else{_tmp138=_tmp137->f1;_tmp139=_tmp137->f2;_tmp13A=_tmp137->f3;_tmp13B=(void*)_tmp137->f4;_tmp13C=_tmp137->f5;_tmp13D=_tmp137->f6;_tmp13E=_tmp137->f7;}}_LL86: {
# 744
struct Cyc_List_List*typvars=0;
# 746
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){
if(Cyc_Absyn_fntype_att((void*)as->hd)){
struct Cyc_List_List*_tmp94C;fn_atts=((_tmp94C=_cycalloc(sizeof(*_tmp94C)),((_tmp94C->hd=(void*)as->hd,((_tmp94C->tl=fn_atts,_tmp94C))))));}else{
# 751
struct Cyc_List_List*_tmp94D;new_atts=((_tmp94D=_cycalloc(sizeof(*_tmp94D)),((_tmp94D->hd=(void*)as->hd,((_tmp94D->tl=new_atts,_tmp94D))))));}}}
# 754
if(tms->tl != 0){
void*_tmp143=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_tmp144=_tmp143;struct Cyc_List_List*_tmp146;_LL8A: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp145=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp144;if(_tmp145->tag != 4)goto _LL8C;else{_tmp146=_tmp145->f1;}}_LL8B:
# 757
 typvars=_tmp146;
tms=tms->tl;
goto _LL89;_LL8C:;_LL8D:
# 761
 goto _LL89;_LL89:;}
# 765
if(((((!_tmp139  && _tmp13A == 0) && _tmp138 != 0) && _tmp138->tl == 0) && (*((struct _tuple8*)_tmp138->hd)).f1 == 0) && (*((struct _tuple8*)_tmp138->hd)).f3 == (void*)& Cyc_Absyn_VoidType_val)
# 770
_tmp138=0;{
# 773
struct Cyc_List_List*_tmp147=Cyc_Parse_get_arg_tags(_tmp138);
# 775
if(_tmp147 != 0)
t=Cyc_Parse_substitute_tags(_tmp147,t);
t=Cyc_Parse_array2ptr(t,0);
# 780
{struct Cyc_List_List*_tmp148=_tmp138;for(0;_tmp148 != 0;_tmp148=_tmp148->tl){
struct _tuple8*_tmp149=(struct _tuple8*)_tmp148->hd;struct _dyneither_ptr*_tmp14B;struct Cyc_Absyn_Tqual _tmp14C;void**_tmp14D;struct _tuple8*_tmp14A=_tmp149;_tmp14B=_tmp14A->f1;_tmp14C=_tmp14A->f2;_tmp14D=(void**)& _tmp14A->f3;
if(_tmp147 != 0)
*_tmp14D=Cyc_Parse_substitute_tags(_tmp147,*_tmp14D);
*_tmp14D=Cyc_Parse_array2ptr(*_tmp14D,1);}}
# 792
return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(tq.loc),
Cyc_Absyn_function_typ(typvars,_tmp13B,tq,t,_tmp138,_tmp139,_tmp13A,_tmp13C,fn_atts,_tmp13D,_tmp13E),new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);};}_LL87: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp13F=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp136;if(_tmp13F->tag != 0)goto _LL84;else{_tmp140=_tmp13F->f2;}}_LL88: {
# 799
const char*_tmp950;void*_tmp94F;(_tmp94F=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp140,((_tmp950="function declaration without parameter types",_tag_dyneither(_tmp950,sizeof(char),45))),_tag_dyneither(_tmp94F,sizeof(void*),0)));}_LL84:;}_LL7E: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp12D=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp123;if(_tmp12D->tag != 4)goto _LL80;else{_tmp12E=_tmp12D->f1;_tmp12F=_tmp12D->f2;}}_LL7F:
# 806
 if(tms->tl == 0){
struct _tuple13 _tmp951;return(_tmp951.f1=tq,((_tmp951.f2=t,((_tmp951.f3=_tmp12E,((_tmp951.f4=atts,_tmp951)))))));}{
# 811
const char*_tmp954;void*_tmp953;(_tmp953=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp12F,((_tmp954="type parameters must appear before function arguments in declarator",_tag_dyneither(_tmp954,sizeof(char),68))),_tag_dyneither(_tmp953,sizeof(void*),0)));};_LL80: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp130=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp123;if(_tmp130->tag != 2)goto _LL82;else{_tmp131=_tmp130->f1;_tmp132=_tmp130->f2;}}_LL81: {
# 814
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp95A;struct Cyc_Absyn_PtrInfo _tmp959;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp958;return Cyc_Parse_apply_tms(_tmp132,(void*)((_tmp958=_cycalloc(sizeof(*_tmp958)),((_tmp958[0]=((_tmp95A.tag=5,((_tmp95A.f1=((_tmp959.elt_typ=t,((_tmp959.elt_tq=tq,((_tmp959.ptr_atts=_tmp131,_tmp959)))))),_tmp95A)))),_tmp958)))),atts,tms->tl);}_LL82: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp133=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp123;if(_tmp133->tag != 5)goto _LL77;else{_tmp134=_tmp133->f1;_tmp135=_tmp133->f2;}}_LL83:
# 819
 return Cyc_Parse_apply_tms(tq,t,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(atts,_tmp135),tms->tl);_LL77:;};}
# 825
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned int loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 834
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned int loc,struct _tuple15*t){
struct _tuple0*_tmp157;struct Cyc_Absyn_Tqual _tmp158;void*_tmp159;struct Cyc_List_List*_tmp15A;struct Cyc_List_List*_tmp15B;struct _tuple15*_tmp156=t;_tmp157=_tmp156->f1;_tmp158=_tmp156->f2;_tmp159=_tmp156->f3;_tmp15A=_tmp156->f4;_tmp15B=_tmp156->f5;
# 837
Cyc_Lex_register_typedef(_tmp157);{
# 839
struct Cyc_Core_Opt*kind;
void*type;
{void*_tmp15C=_tmp159;struct Cyc_Core_Opt*_tmp15E;_LL8F: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp15D=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp15C;if(_tmp15D->tag != 1)goto _LL91;else{_tmp15E=_tmp15D->f1;}}_LL90:
# 843
 type=0;
if(_tmp15E == 0)kind=& Cyc_Tcutil_bko;else{
kind=_tmp15E;}
goto _LL8E;_LL91:;_LL92:
 kind=0;type=_tmp159;goto _LL8E;_LL8E:;}{
# 849
struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct _tmp960;struct Cyc_Absyn_Typedefdecl*_tmp95F;struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp95E;return Cyc_Absyn_new_decl((void*)((_tmp95E=_cycalloc(sizeof(*_tmp95E)),((_tmp95E[0]=((_tmp960.tag=8,((_tmp960.f1=((_tmp95F=_cycalloc(sizeof(*_tmp95F)),((_tmp95F->name=_tmp157,((_tmp95F->tvs=_tmp15A,((_tmp95F->kind=kind,((_tmp95F->defn=type,((_tmp95F->atts=_tmp15B,((_tmp95F->tq=_tmp158,_tmp95F)))))))))))))),_tmp960)))),_tmp95E)))),loc);};};}
# 856
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp963;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp962;return Cyc_Absyn_new_stmt((void*)((_tmp962=_cycalloc(sizeof(*_tmp962)),((_tmp962[0]=((_tmp963.tag=12,((_tmp963.f1=d,((_tmp963.f2=s,_tmp963)))))),_tmp962)))),d->loc);}
# 862
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
# 871
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple12*ids,unsigned int tqual_loc,unsigned int loc){
# 875
struct _RegionHandle _tmp164=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmp164;_push_region(mkrgn);
{struct Cyc_Absyn_Tqual _tmp166;struct Cyc_Parse_Type_specifier _tmp167;struct Cyc_List_List*_tmp168;struct Cyc_Parse_Declaration_spec _tmp165=ds;_tmp166=_tmp165.tq;_tmp167=_tmp165.type_specs;_tmp168=_tmp165.attributes;
if(_tmp166.loc == 0)_tmp166.loc=tqual_loc;
if(ds.is_inline){
const char*_tmp964;Cyc_Parse_err(((_tmp964="inline is allowed only on function definitions",_tag_dyneither(_tmp964,sizeof(char),47))),loc);}{
# 881
enum Cyc_Absyn_Scope s=Cyc_Absyn_Public;
int istypedef=0;
if(ds.sc != 0){
enum Cyc_Parse_Storage_class _tmp16A=*ds.sc;switch(_tmp16A){case Cyc_Parse_Typedef_sc: _LL93:
 istypedef=1;break;case Cyc_Parse_Extern_sc: _LL94:
 s=Cyc_Absyn_Extern;break;case Cyc_Parse_ExternC_sc: _LL95:
 s=Cyc_Absyn_ExternC;break;case Cyc_Parse_Static_sc: _LL96:
 s=Cyc_Absyn_Static;break;case Cyc_Parse_Auto_sc: _LL97:
 s=Cyc_Absyn_Public;break;case Cyc_Parse_Register_sc: _LL98:
 if(Cyc_Parse_no_register)s=Cyc_Absyn_Public;else{s=Cyc_Absyn_Register;}break;case Cyc_Parse_Abstract_sc: _LL99:
 s=Cyc_Absyn_Abstract;break;}}{
# 897
struct _tuple10*declarators=0;
struct Cyc_List_List*exprs=0;
Cyc_Parse_decl_split(mkrgn,ids,& declarators,& exprs);{
# 901
int exps_empty=1;
{struct Cyc_List_List*es=exprs;for(0;es != 0;es=es->tl){
if((struct Cyc_Absyn_Exp*)es->hd != 0){
exps_empty=0;
break;}}}{
# 909
void*_tmp16B=Cyc_Parse_collapse_type_specifiers(_tmp167,loc);
if(declarators == 0){
# 913
void*_tmp16C=_tmp16B;struct Cyc_Absyn_Aggrdecl*_tmp16F;struct Cyc_Absyn_Enumdecl*_tmp172;struct Cyc_Absyn_Datatypedecl*_tmp175;enum Cyc_Absyn_AggrKind _tmp177;struct _tuple0*_tmp178;struct Cyc_List_List*_tmp179;struct Cyc_Absyn_Datatypedecl**_tmp17B;struct _tuple0*_tmp17D;int _tmp17E;struct Cyc_List_List*_tmp17F;struct _tuple0*_tmp181;struct Cyc_List_List*_tmp183;_LL9C: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp16D=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp16C;if(_tmp16D->tag != 26)goto _LL9E;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp16E=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp16D->f1)->r;if(_tmp16E->tag != 0)goto _LL9E;else{_tmp16F=_tmp16E->f1;}}}_LL9D:
# 915
 _tmp16F->attributes=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp16F->attributes,_tmp168);
_tmp16F->sc=s;{
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp96A;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp969;struct Cyc_List_List*_tmp968;struct Cyc_List_List*_tmp187=(_tmp968=_cycalloc(sizeof(*_tmp968)),((_tmp968->hd=Cyc_Absyn_new_decl((void*)((_tmp96A=_cycalloc(sizeof(*_tmp96A)),((_tmp96A[0]=((_tmp969.tag=5,((_tmp969.f1=_tmp16F,_tmp969)))),_tmp96A)))),loc),((_tmp968->tl=0,_tmp968)))));_npop_handler(0);return _tmp187;};_LL9E: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp170=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp16C;if(_tmp170->tag != 26)goto _LLA0;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp171=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp170->f1)->r;if(_tmp171->tag != 1)goto _LLA0;else{_tmp172=_tmp171->f1;}}}_LL9F:
# 919
 if(_tmp168 != 0){const char*_tmp96B;Cyc_Parse_err(((_tmp96B="attributes on enum not supported",_tag_dyneither(_tmp96B,sizeof(char),33))),loc);}
_tmp172->sc=s;{
struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp971;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp970;struct Cyc_List_List*_tmp96F;struct Cyc_List_List*_tmp18C=(_tmp96F=_cycalloc(sizeof(*_tmp96F)),((_tmp96F->hd=Cyc_Absyn_new_decl((void*)((_tmp971=_cycalloc(sizeof(*_tmp971)),((_tmp971[0]=((_tmp970.tag=7,((_tmp970.f1=_tmp172,_tmp970)))),_tmp971)))),loc),((_tmp96F->tl=0,_tmp96F)))));_npop_handler(0);return _tmp18C;};_LLA0: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp173=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp16C;if(_tmp173->tag != 26)goto _LLA2;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp174=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp173->f1)->r;if(_tmp174->tag != 2)goto _LLA2;else{_tmp175=_tmp174->f1;}}}_LLA1:
# 923
 if(_tmp168 != 0){const char*_tmp972;Cyc_Parse_err(((_tmp972="attributes on datatypes not supported",_tag_dyneither(_tmp972,sizeof(char),38))),loc);}
_tmp175->sc=s;{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp978;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp977;struct Cyc_List_List*_tmp976;struct Cyc_List_List*_tmp191=(_tmp976=_cycalloc(sizeof(*_tmp976)),((_tmp976->hd=Cyc_Absyn_new_decl((void*)((_tmp978=_cycalloc(sizeof(*_tmp978)),((_tmp978[0]=((_tmp977.tag=6,((_tmp977.f1=_tmp175,_tmp977)))),_tmp978)))),loc),((_tmp976->tl=0,_tmp976)))));_npop_handler(0);return _tmp191;};_LLA2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp176=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp16C;if(_tmp176->tag != 11)goto _LLA4;else{if((((_tmp176->f1).aggr_info).UnknownAggr).tag != 1)goto _LLA4;_tmp177=((struct _tuple2)(((_tmp176->f1).aggr_info).UnknownAggr).val).f1;_tmp178=((struct _tuple2)(((_tmp176->f1).aggr_info).UnknownAggr).val).f2;_tmp179=(_tmp176->f1).targs;}}_LLA3: {
# 927
struct Cyc_List_List*_tmp192=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp179);
struct Cyc_Absyn_Aggrdecl*_tmp979;struct Cyc_Absyn_Aggrdecl*_tmp193=(_tmp979=_cycalloc(sizeof(*_tmp979)),((_tmp979->kind=_tmp177,((_tmp979->sc=s,((_tmp979->name=_tmp178,((_tmp979->tvs=_tmp192,((_tmp979->impl=0,((_tmp979->attributes=0,_tmp979)))))))))))));
if(_tmp168 != 0){const char*_tmp97A;Cyc_Parse_err(((_tmp97A="bad attributes on type declaration",_tag_dyneither(_tmp97A,sizeof(char),35))),loc);}{
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp980;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp97F;struct Cyc_List_List*_tmp97E;struct Cyc_List_List*_tmp198=(_tmp97E=_cycalloc(sizeof(*_tmp97E)),((_tmp97E->hd=Cyc_Absyn_new_decl((void*)((_tmp980=_cycalloc(sizeof(*_tmp980)),((_tmp980[0]=((_tmp97F.tag=5,((_tmp97F.f1=_tmp193,_tmp97F)))),_tmp980)))),loc),((_tmp97E->tl=0,_tmp97E)))));_npop_handler(0);return _tmp198;};}_LLA4: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp17A=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp16C;if(_tmp17A->tag != 3)goto _LLA6;else{if((((_tmp17A->f1).datatype_info).KnownDatatype).tag != 2)goto _LLA6;_tmp17B=(struct Cyc_Absyn_Datatypedecl**)(((_tmp17A->f1).datatype_info).KnownDatatype).val;}}_LLA5:
# 932
 if(_tmp168 != 0){const char*_tmp981;Cyc_Parse_err(((_tmp981="bad attributes on datatype",_tag_dyneither(_tmp981,sizeof(char),27))),loc);}{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp987;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp986;struct Cyc_List_List*_tmp985;struct Cyc_List_List*_tmp19E=(_tmp985=_cycalloc(sizeof(*_tmp985)),((_tmp985->hd=Cyc_Absyn_new_decl((void*)((_tmp987=_cycalloc(sizeof(*_tmp987)),((_tmp987[0]=((_tmp986.tag=6,((_tmp986.f1=*_tmp17B,_tmp986)))),_tmp987)))),loc),((_tmp985->tl=0,_tmp985)))));_npop_handler(0);return _tmp19E;};_LLA6: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp17C=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp16C;if(_tmp17C->tag != 3)goto _LLA8;else{if((((_tmp17C->f1).datatype_info).UnknownDatatype).tag != 1)goto _LLA8;_tmp17D=((struct Cyc_Absyn_UnknownDatatypeInfo)(((_tmp17C->f1).datatype_info).UnknownDatatype).val).name;_tmp17E=((struct Cyc_Absyn_UnknownDatatypeInfo)(((_tmp17C->f1).datatype_info).UnknownDatatype).val).is_extensible;_tmp17F=(_tmp17C->f1).targs;}}_LLA7: {
# 935
struct Cyc_List_List*_tmp19F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp17F);
struct Cyc_Absyn_Decl*_tmp1A0=Cyc_Absyn_datatype_decl(s,_tmp17D,_tmp19F,0,_tmp17E,loc);
if(_tmp168 != 0){const char*_tmp988;Cyc_Parse_err(((_tmp988="bad attributes on datatype",_tag_dyneither(_tmp988,sizeof(char),27))),loc);}{
struct Cyc_List_List*_tmp989;struct Cyc_List_List*_tmp1A3=(_tmp989=_cycalloc(sizeof(*_tmp989)),((_tmp989->hd=_tmp1A0,((_tmp989->tl=0,_tmp989)))));_npop_handler(0);return _tmp1A3;};}_LLA8: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp180=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp16C;if(_tmp180->tag != 13)goto _LLAA;else{_tmp181=_tmp180->f1;}}_LLA9: {
# 940
struct Cyc_Absyn_Enumdecl*_tmp98A;struct Cyc_Absyn_Enumdecl*_tmp1A4=(_tmp98A=_cycalloc(sizeof(*_tmp98A)),((_tmp98A->sc=s,((_tmp98A->name=_tmp181,((_tmp98A->fields=0,_tmp98A)))))));
if(_tmp168 != 0){const char*_tmp98B;Cyc_Parse_err(((_tmp98B="bad attributes on enum",_tag_dyneither(_tmp98B,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp995;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp994;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp993;struct Cyc_List_List*_tmp992;struct Cyc_List_List*_tmp1AA=(_tmp992=_cycalloc(sizeof(*_tmp992)),((_tmp992->hd=((_tmp995=_cycalloc(sizeof(*_tmp995)),((_tmp995->r=(void*)((_tmp993=_cycalloc(sizeof(*_tmp993)),((_tmp993[0]=((_tmp994.tag=7,((_tmp994.f1=_tmp1A4,_tmp994)))),_tmp993)))),((_tmp995->loc=loc,_tmp995)))))),((_tmp992->tl=0,_tmp992)))));_npop_handler(0);return _tmp1AA;};}_LLAA: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp182=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp16C;if(_tmp182->tag != 14)goto _LLAC;else{_tmp183=_tmp182->f1;}}_LLAB: {
# 946
struct Cyc_Core_Opt*_tmp998;struct Cyc_Absyn_Enumdecl*_tmp997;struct Cyc_Absyn_Enumdecl*_tmp1AC=(_tmp997=_cycalloc(sizeof(*_tmp997)),((_tmp997->sc=s,((_tmp997->name=Cyc_Parse_gensym_enum(),((_tmp997->fields=((_tmp998=_cycalloc(sizeof(*_tmp998)),((_tmp998->v=_tmp183,_tmp998)))),_tmp997)))))));
if(_tmp168 != 0){const char*_tmp999;Cyc_Parse_err(((_tmp999="bad attributes on enum",_tag_dyneither(_tmp999,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp9A3;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp9A2;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp9A1;struct Cyc_List_List*_tmp9A0;struct Cyc_List_List*_tmp1B2=(_tmp9A0=_cycalloc(sizeof(*_tmp9A0)),((_tmp9A0->hd=((_tmp9A3=_cycalloc(sizeof(*_tmp9A3)),((_tmp9A3->r=(void*)((_tmp9A1=_cycalloc(sizeof(*_tmp9A1)),((_tmp9A1[0]=((_tmp9A2.tag=7,((_tmp9A2.f1=_tmp1AC,_tmp9A2)))),_tmp9A1)))),((_tmp9A3->loc=loc,_tmp9A3)))))),((_tmp9A0->tl=0,_tmp9A0)))));_npop_handler(0);return _tmp1B2;};}_LLAC:;_LLAD:
{const char*_tmp9A4;Cyc_Parse_err(((_tmp9A4="missing declarator",_tag_dyneither(_tmp9A4,sizeof(char),19))),loc);}{struct Cyc_List_List*_tmp1B6=0;_npop_handler(0);return _tmp1B6;};_LL9B:;}else{
# 953
struct Cyc_List_List*_tmp1B7=Cyc_Parse_apply_tmss(mkrgn,_tmp166,_tmp16B,declarators,_tmp168);
if(istypedef){
# 958
if(!exps_empty){
const char*_tmp9A5;Cyc_Parse_err(((_tmp9A5="initializer in typedef declaration",_tag_dyneither(_tmp9A5,sizeof(char),35))),loc);}{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(unsigned int,struct _tuple15*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp1B7);
struct Cyc_List_List*_tmp1B9=decls;_npop_handler(0);return _tmp1B9;};}else{
# 964
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp1BA=_tmp1B7;for(0;_tmp1BA != 0;(_tmp1BA=_tmp1BA->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple15*_tmp1BB=(struct _tuple15*)_tmp1BA->hd;struct _tuple0*_tmp1BD;struct Cyc_Absyn_Tqual _tmp1BE;void*_tmp1BF;struct Cyc_List_List*_tmp1C0;struct Cyc_List_List*_tmp1C1;struct _tuple15*_tmp1BC=_tmp1BB;_tmp1BD=_tmp1BC->f1;_tmp1BE=_tmp1BC->f2;_tmp1BF=_tmp1BC->f3;_tmp1C0=_tmp1BC->f4;_tmp1C1=_tmp1BC->f5;
if(_tmp1C0 != 0){
const char*_tmp9A8;void*_tmp9A7;(_tmp9A7=0,Cyc_Tcutil_warn(loc,((_tmp9A8="bad type params, ignoring",_tag_dyneither(_tmp9A8,sizeof(char),26))),_tag_dyneither(_tmp9A7,sizeof(void*),0)));}
if(exprs == 0){
const char*_tmp9AB;void*_tmp9AA;(_tmp9AA=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp9AB="unexpected NULL in parse!",_tag_dyneither(_tmp9AB,sizeof(char),26))),_tag_dyneither(_tmp9AA,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp1C6=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd;
struct Cyc_Absyn_Vardecl*_tmp1C7=Cyc_Absyn_new_vardecl(_tmp1BD,_tmp1BF,_tmp1C6);
_tmp1C7->tq=_tmp1BE;
_tmp1C7->sc=s;
_tmp1C7->attributes=_tmp1C1;{
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp9B1;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp9B0;struct Cyc_Absyn_Decl*_tmp9AF;struct Cyc_Absyn_Decl*_tmp1C8=(_tmp9AF=_cycalloc(sizeof(*_tmp9AF)),((_tmp9AF->r=(void*)((_tmp9B1=_cycalloc(sizeof(*_tmp9B1)),((_tmp9B1[0]=((_tmp9B0.tag=0,((_tmp9B0.f1=_tmp1C7,_tmp9B0)))),_tmp9B1)))),((_tmp9AF->loc=loc,_tmp9AF)))));
struct Cyc_List_List*_tmp9B2;decls=((_tmp9B2=_cycalloc(sizeof(*_tmp9B2)),((_tmp9B2->hd=_tmp1C8,((_tmp9B2->tl=decls,_tmp9B2))))));};};}}{
# 979
struct Cyc_List_List*_tmp1CD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);_npop_handler(0);return _tmp1CD;};}}};};};};}
# 876
;_pop_region(mkrgn);}
# 986
static struct Cyc_Absyn_Kind*Cyc_Parse_id_to_kind(struct _dyneither_ptr s,unsigned int loc){
if(Cyc_strlen((struct _dyneither_ptr)s)== 1  || Cyc_strlen((struct _dyneither_ptr)s)== 2){
char _tmp1CE=*((const char*)_check_dyneither_subscript(s,sizeof(char),0));switch(_tmp1CE){case 'A': _LLAE:
 return& Cyc_Tcutil_ak;case 'M': _LLAF:
 return& Cyc_Tcutil_mk;case 'B': _LLB0:
 return& Cyc_Tcutil_bk;case 'R': _LLB1:
 return& Cyc_Tcutil_rk;case 'E': _LLB2:
 return& Cyc_Tcutil_ek;case 'I': _LLB3:
 return& Cyc_Tcutil_ik;case 'U': _LLB4:
# 996
{char _tmp1CF=*((const char*)_check_dyneither_subscript(s,sizeof(char),1));switch(_tmp1CF){case 'R': _LLB6:
 return& Cyc_Tcutil_urk;case 'A': _LLB7:
 return& Cyc_Tcutil_uak;case 'M': _LLB8:
 return& Cyc_Tcutil_umk;case 'B': _LLB9:
 return& Cyc_Tcutil_ubk;default: _LLBA:
 break;}}
# 1003
break;case 'T': _LLB5:
# 1005
{char _tmp1D0=*((const char*)_check_dyneither_subscript(s,sizeof(char),1));switch(_tmp1D0){case 'R': _LLBD:
 return& Cyc_Tcutil_trk;case 'A': _LLBE:
 return& Cyc_Tcutil_tak;case 'M': _LLBF:
 return& Cyc_Tcutil_tmk;case 'B': _LLC0:
 return& Cyc_Tcutil_tbk;default: _LLC1:
 break;}}
# 1012
break;default: _LLBC:
 break;}}
# 1015
{const char*_tmp9B7;void*_tmp9B6[2];struct Cyc_String_pa_PrintArg_struct _tmp9B5;struct Cyc_Int_pa_PrintArg_struct _tmp9B4;Cyc_Parse_err((struct _dyneither_ptr)((_tmp9B4.tag=1,((_tmp9B4.f1=(unsigned long)((int)Cyc_strlen((struct _dyneither_ptr)s)),((_tmp9B5.tag=0,((_tmp9B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp9B6[0]=& _tmp9B5,((_tmp9B6[1]=& _tmp9B4,Cyc_aprintf(((_tmp9B7="bad kind: %s; strlen=%d",_tag_dyneither(_tmp9B7,sizeof(char),24))),_tag_dyneither(_tmp9B6,sizeof(void*),2)))))))))))))),loc);}
return& Cyc_Tcutil_bk;}
# 1020
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_tmp1D5=p->r;void*_tmp1D6=_tmp1D5;struct _tuple0*_tmp1D8;struct Cyc_Absyn_Vardecl*_tmp1DA;struct Cyc_Absyn_Pat*_tmp1DD;enum Cyc_Absyn_Sign _tmp1E0;int _tmp1E1;char _tmp1E3;struct _dyneither_ptr _tmp1E5;int _tmp1E6;struct _tuple0*_tmp1E8;struct Cyc_List_List*_tmp1E9;struct Cyc_Absyn_Exp*_tmp1EB;_LLC5: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp1D7=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1D6;if(_tmp1D7->tag != 15)goto _LLC7;else{_tmp1D8=_tmp1D7->f1;}}_LLC6:
 return Cyc_Absyn_unknownid_exp(_tmp1D8,p->loc);_LLC7: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp1D9=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1D6;if(_tmp1D9->tag != 3)goto _LLC9;else{_tmp1DA=_tmp1D9->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp1DB=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp1D9->f2)->r;if(_tmp1DB->tag != 0)goto _LLC9;};}}_LLC8:
# 1024
 return Cyc_Absyn_deref_exp(Cyc_Absyn_unknownid_exp(_tmp1DA->name,p->loc),p->loc);_LLC9: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp1DC=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1D6;if(_tmp1DC->tag != 6)goto _LLCB;else{_tmp1DD=_tmp1DC->f1;}}_LLCA:
 return Cyc_Absyn_address_exp(Cyc_Parse_pat2exp(_tmp1DD),p->loc);_LLCB: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp1DE=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp1D6;if(_tmp1DE->tag != 9)goto _LLCD;}_LLCC:
 return Cyc_Absyn_null_exp(p->loc);_LLCD: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp1DF=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1D6;if(_tmp1DF->tag != 10)goto _LLCF;else{_tmp1E0=_tmp1DF->f1;_tmp1E1=_tmp1DF->f2;}}_LLCE:
 return Cyc_Absyn_int_exp(_tmp1E0,_tmp1E1,p->loc);_LLCF: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp1E2=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1D6;if(_tmp1E2->tag != 11)goto _LLD1;else{_tmp1E3=_tmp1E2->f1;}}_LLD0:
 return Cyc_Absyn_char_exp(_tmp1E3,p->loc);_LLD1: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp1E4=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1D6;if(_tmp1E4->tag != 12)goto _LLD3;else{_tmp1E5=_tmp1E4->f1;_tmp1E6=_tmp1E4->f2;}}_LLD2:
 return Cyc_Absyn_float_exp(_tmp1E5,_tmp1E6,p->loc);_LLD3: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp1E7=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1D6;if(_tmp1E7->tag != 16)goto _LLD5;else{_tmp1E8=_tmp1E7->f1;_tmp1E9=_tmp1E7->f2;if(_tmp1E7->f3 != 0)goto _LLD5;}}_LLD4: {
# 1031
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(_tmp1E8,p->loc);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp1E9);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}_LLD5: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp1EA=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1D6;if(_tmp1EA->tag != 17)goto _LLD7;else{_tmp1EB=_tmp1EA->f1;}}_LLD6:
 return _tmp1EB;_LLD7:;_LLD8:
# 1036
{const char*_tmp9B8;Cyc_Parse_err(((_tmp9B8="cannot mix patterns and expressions in case",_tag_dyneither(_tmp9B8,sizeof(char),44))),p->loc);}
return Cyc_Absyn_null_exp(p->loc);_LLC4:;}struct _union_YYSTYPE_Int_tok{int tag;struct _tuple5 val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple20{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple20 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple21{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple21*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple22{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Parse_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Parse_Declarator val;};struct _tuple24{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple25{struct Cyc_Absyn_Tqual f1;struct Cyc_Parse_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple25 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple26{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple26*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};static char _tmp1EE[14]="$(sign_t,int)";
# 1092 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp1EE,_tmp1EE,_tmp1EE + 14}};
# 1094
struct _tuple5 Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1){
struct _tuple5 yyzzz;
{union Cyc_YYSTYPE _tmp1EF=yy1;struct _tuple5 _tmp1F0;_LLDA: if((_tmp1EF.Int_tok).tag != 1)goto _LLDC;_tmp1F0=(struct _tuple5)(_tmp1EF.Int_tok).val;_LLDB:
# 1098
 yyzzz=_tmp1F0;
goto _LLD9;_LLDC:;_LLDD:
(int)_throw((void*)& Cyc_yyfail_Int_tok);_LLD9:;}
# 1102
return yyzzz;}
# 1104
union Cyc_YYSTYPE Cyc_Int_tok(struct _tuple5 yy1){union Cyc_YYSTYPE _tmp9B9;return((_tmp9B9.Int_tok).val=yy1,(((_tmp9B9.Int_tok).tag=1,_tmp9B9)));}static char _tmp1F3[5]="char";
# 1093 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Char_tok={Cyc_Core_Failure,{_tmp1F3,_tmp1F3,_tmp1F3 + 5}};
# 1095
char Cyc_yyget_Char_tok(union Cyc_YYSTYPE yy1){
char yyzzz;
{union Cyc_YYSTYPE _tmp1F4=yy1;char _tmp1F5;_LLDF: if((_tmp1F4.Char_tok).tag != 2)goto _LLE1;_tmp1F5=(char)(_tmp1F4.Char_tok).val;_LLE0:
# 1099
 yyzzz=_tmp1F5;
goto _LLDE;_LLE1:;_LLE2:
(int)_throw((void*)& Cyc_yyfail_Char_tok);_LLDE:;}
# 1103
return yyzzz;}
# 1105
union Cyc_YYSTYPE Cyc_Char_tok(char yy1){union Cyc_YYSTYPE _tmp9BA;return((_tmp9BA.Char_tok).val=yy1,(((_tmp9BA.Char_tok).tag=2,_tmp9BA)));}static char _tmp1F8[13]="string_t<`H>";
# 1094 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_String_tok={Cyc_Core_Failure,{_tmp1F8,_tmp1F8,_tmp1F8 + 13}};
# 1096
struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE yy1){
struct _dyneither_ptr yyzzz;
{union Cyc_YYSTYPE _tmp1F9=yy1;struct _dyneither_ptr _tmp1FA;_LLE4: if((_tmp1F9.String_tok).tag != 3)goto _LLE6;_tmp1FA=(struct _dyneither_ptr)(_tmp1F9.String_tok).val;_LLE5:
# 1100
 yyzzz=_tmp1FA;
goto _LLE3;_LLE6:;_LLE7:
(int)_throw((void*)& Cyc_yyfail_String_tok);_LLE3:;}
# 1104
return yyzzz;}
# 1106
union Cyc_YYSTYPE Cyc_String_tok(struct _dyneither_ptr yy1){union Cyc_YYSTYPE _tmp9BB;return((_tmp9BB.String_tok).val=yy1,(((_tmp9BB.String_tok).tag=3,_tmp9BB)));}static char _tmp1FD[56]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
# 1097 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp1FD,_tmp1FD,_tmp1FD + 56}};
# 1099
struct _tuple21*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1){
struct _tuple21*yyzzz;
{union Cyc_YYSTYPE _tmp1FE=yy1;struct _tuple21*_tmp1FF;_LLE9: if((_tmp1FE.YY1).tag != 9)goto _LLEB;_tmp1FF=(struct _tuple21*)(_tmp1FE.YY1).val;_LLEA:
# 1103
 yyzzz=_tmp1FF;
goto _LLE8;_LLEB:;_LLEC:
(int)_throw((void*)& Cyc_yyfail_YY1);_LLE8:;}
# 1107
return yyzzz;}
# 1109
union Cyc_YYSTYPE Cyc_YY1(struct _tuple21*yy1){union Cyc_YYSTYPE _tmp9BC;return((_tmp9BC.YY1).val=yy1,(((_tmp9BC.YY1).tag=9,_tmp9BC)));}static char _tmp202[19]="conref_t<bounds_t>";
# 1098 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{_tmp202,_tmp202,_tmp202 + 19}};
# 1100
union Cyc_Absyn_Constraint*Cyc_yyget_YY2(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Constraint*yyzzz;
{union Cyc_YYSTYPE _tmp203=yy1;union Cyc_Absyn_Constraint*_tmp204;_LLEE: if((_tmp203.YY2).tag != 10)goto _LLF0;_tmp204=(union Cyc_Absyn_Constraint*)(_tmp203.YY2).val;_LLEF:
# 1104
 yyzzz=_tmp204;
goto _LLED;_LLF0:;_LLF1:
(int)_throw((void*)& Cyc_yyfail_YY2);_LLED:;}
# 1108
return yyzzz;}
# 1110
union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp9BD;return((_tmp9BD.YY2).val=yy1,(((_tmp9BD.YY2).tag=10,_tmp9BD)));}static char _tmp207[28]="list_t<offsetof_field_t,`H>";
# 1099 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY3={Cyc_Core_Failure,{_tmp207,_tmp207,_tmp207 + 28}};
# 1101
struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp208=yy1;struct Cyc_List_List*_tmp209;_LLF3: if((_tmp208.YY3).tag != 11)goto _LLF5;_tmp209=(struct Cyc_List_List*)(_tmp208.YY3).val;_LLF4:
# 1105
 yyzzz=_tmp209;
goto _LLF2;_LLF5:;_LLF6:
(int)_throw((void*)& Cyc_yyfail_YY3);_LLF2:;}
# 1109
return yyzzz;}
# 1111
union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9BE;return((_tmp9BE.YY3).val=yy1,(((_tmp9BE.YY3).tag=11,_tmp9BE)));}static char _tmp20C[6]="exp_t";
# 1100 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Exp_tok={Cyc_Core_Failure,{_tmp20C,_tmp20C,_tmp20C + 6}};
# 1102
struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp20D=yy1;struct Cyc_Absyn_Exp*_tmp20E;_LLF8: if((_tmp20D.Exp_tok).tag != 7)goto _LLFA;_tmp20E=(struct Cyc_Absyn_Exp*)(_tmp20D.Exp_tok).val;_LLF9:
# 1106
 yyzzz=_tmp20E;
goto _LLF7;_LLFA:;_LLFB:
(int)_throw((void*)& Cyc_yyfail_Exp_tok);_LLF7:;}
# 1110
return yyzzz;}
# 1112
union Cyc_YYSTYPE Cyc_Exp_tok(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp9BF;return((_tmp9BF.Exp_tok).val=yy1,(((_tmp9BF.Exp_tok).tag=7,_tmp9BF)));}static char _tmp211[17]="list_t<exp_t,`H>";
# 1108 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp211,_tmp211,_tmp211 + 17}};
# 1110
struct Cyc_List_List*Cyc_yyget_YY4(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp212=yy1;struct Cyc_List_List*_tmp213;_LLFD: if((_tmp212.YY4).tag != 12)goto _LLFF;_tmp213=(struct Cyc_List_List*)(_tmp212.YY4).val;_LLFE:
# 1114
 yyzzz=_tmp213;
goto _LLFC;_LLFF:;_LL100:
(int)_throw((void*)& Cyc_yyfail_YY4);_LLFC:;}
# 1118
return yyzzz;}
# 1120
union Cyc_YYSTYPE Cyc_YY4(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9C0;return((_tmp9C0.YY4).val=yy1,(((_tmp9C0.YY4).tag=12,_tmp9C0)));}static char _tmp216[47]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1109 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY5={Cyc_Core_Failure,{_tmp216,_tmp216,_tmp216 + 47}};
# 1111
struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp217=yy1;struct Cyc_List_List*_tmp218;_LL102: if((_tmp217.YY5).tag != 13)goto _LL104;_tmp218=(struct Cyc_List_List*)(_tmp217.YY5).val;_LL103:
# 1115
 yyzzz=_tmp218;
goto _LL101;_LL104:;_LL105:
(int)_throw((void*)& Cyc_yyfail_YY5);_LL101:;}
# 1119
return yyzzz;}
# 1121
union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9C1;return((_tmp9C1.YY5).val=yy1,(((_tmp9C1.YY5).tag=13,_tmp9C1)));}static char _tmp21B[9]="primop_t";
# 1110 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp21B,_tmp21B,_tmp21B + 9}};
# 1112
enum Cyc_Absyn_Primop Cyc_yyget_YY6(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Primop yyzzz;
{union Cyc_YYSTYPE _tmp21C=yy1;enum Cyc_Absyn_Primop _tmp21D;_LL107: if((_tmp21C.YY6).tag != 14)goto _LL109;_tmp21D=(enum Cyc_Absyn_Primop)(_tmp21C.YY6).val;_LL108:
# 1116
 yyzzz=_tmp21D;
goto _LL106;_LL109:;_LL10A:
(int)_throw((void*)& Cyc_yyfail_YY6);_LL106:;}
# 1120
return yyzzz;}
# 1122
union Cyc_YYSTYPE Cyc_YY6(enum Cyc_Absyn_Primop yy1){union Cyc_YYSTYPE _tmp9C2;return((_tmp9C2.YY6).val=yy1,(((_tmp9C2.YY6).tag=14,_tmp9C2)));}static char _tmp220[19]="opt_t<primop_t,`H>";
# 1111 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp220,_tmp220,_tmp220 + 19}};
# 1113
struct Cyc_Core_Opt*Cyc_yyget_YY7(union Cyc_YYSTYPE yy1){
struct Cyc_Core_Opt*yyzzz;
{union Cyc_YYSTYPE _tmp221=yy1;struct Cyc_Core_Opt*_tmp222;_LL10C: if((_tmp221.YY7).tag != 15)goto _LL10E;_tmp222=(struct Cyc_Core_Opt*)(_tmp221.YY7).val;_LL10D:
# 1117
 yyzzz=_tmp222;
goto _LL10B;_LL10E:;_LL10F:
(int)_throw((void*)& Cyc_yyfail_YY7);_LL10B:;}
# 1121
return yyzzz;}
# 1123
union Cyc_YYSTYPE Cyc_YY7(struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _tmp9C3;return((_tmp9C3.YY7).val=yy1,(((_tmp9C3.YY7).tag=15,_tmp9C3)));}static char _tmp225[7]="qvar_t";
# 1112 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp225,_tmp225,_tmp225 + 7}};
# 1114
struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE yy1){
struct _tuple0*yyzzz;
{union Cyc_YYSTYPE _tmp226=yy1;struct _tuple0*_tmp227;_LL111: if((_tmp226.QualId_tok).tag != 5)goto _LL113;_tmp227=(struct _tuple0*)(_tmp226.QualId_tok).val;_LL112:
# 1118
 yyzzz=_tmp227;
goto _LL110;_LL113:;_LL114:
(int)_throw((void*)& Cyc_yyfail_QualId_tok);_LL110:;}
# 1122
return yyzzz;}
# 1124
union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){union Cyc_YYSTYPE _tmp9C4;return((_tmp9C4.QualId_tok).val=yy1,(((_tmp9C4.QualId_tok).tag=5,_tmp9C4)));}static char _tmp22A[7]="stmt_t";
# 1115 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Stmt_tok={Cyc_Core_Failure,{_tmp22A,_tmp22A,_tmp22A + 7}};
# 1117
struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Stmt*yyzzz;
{union Cyc_YYSTYPE _tmp22B=yy1;struct Cyc_Absyn_Stmt*_tmp22C;_LL116: if((_tmp22B.Stmt_tok).tag != 8)goto _LL118;_tmp22C=(struct Cyc_Absyn_Stmt*)(_tmp22B.Stmt_tok).val;_LL117:
# 1121
 yyzzz=_tmp22C;
goto _LL115;_LL118:;_LL119:
(int)_throw((void*)& Cyc_yyfail_Stmt_tok);_LL115:;}
# 1125
return yyzzz;}
# 1127
union Cyc_YYSTYPE Cyc_Stmt_tok(struct Cyc_Absyn_Stmt*yy1){union Cyc_YYSTYPE _tmp9C5;return((_tmp9C5.Stmt_tok).val=yy1,(((_tmp9C5.Stmt_tok).tag=8,_tmp9C5)));}static char _tmp22F[27]="list_t<switch_clause_t,`H>";
# 1119 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY8={Cyc_Core_Failure,{_tmp22F,_tmp22F,_tmp22F + 27}};
# 1121
struct Cyc_List_List*Cyc_yyget_YY8(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp230=yy1;struct Cyc_List_List*_tmp231;_LL11B: if((_tmp230.YY8).tag != 16)goto _LL11D;_tmp231=(struct Cyc_List_List*)(_tmp230.YY8).val;_LL11C:
# 1125
 yyzzz=_tmp231;
goto _LL11A;_LL11D:;_LL11E:
(int)_throw((void*)& Cyc_yyfail_YY8);_LL11A:;}
# 1129
return yyzzz;}
# 1131
union Cyc_YYSTYPE Cyc_YY8(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9C6;return((_tmp9C6.YY8).val=yy1,(((_tmp9C6.YY8).tag=16,_tmp9C6)));}static char _tmp234[6]="pat_t";
# 1120 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp234,_tmp234,_tmp234 + 6}};
# 1122
struct Cyc_Absyn_Pat*Cyc_yyget_YY9(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Pat*yyzzz;
{union Cyc_YYSTYPE _tmp235=yy1;struct Cyc_Absyn_Pat*_tmp236;_LL120: if((_tmp235.YY9).tag != 17)goto _LL122;_tmp236=(struct Cyc_Absyn_Pat*)(_tmp235.YY9).val;_LL121:
# 1126
 yyzzz=_tmp236;
goto _LL11F;_LL122:;_LL123:
(int)_throw((void*)& Cyc_yyfail_YY9);_LL11F:;}
# 1130
return yyzzz;}
# 1132
union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Pat*yy1){union Cyc_YYSTYPE _tmp9C7;return((_tmp9C7.YY9).val=yy1,(((_tmp9C7.YY9).tag=17,_tmp9C7)));}static char _tmp239[28]="$(list_t<pat_t,`H>,bool)@`H";
# 1125 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp239,_tmp239,_tmp239 + 28}};
# 1127
struct _tuple22*Cyc_yyget_YY10(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp23A=yy1;struct _tuple22*_tmp23B;_LL125: if((_tmp23A.YY10).tag != 18)goto _LL127;_tmp23B=(struct _tuple22*)(_tmp23A.YY10).val;_LL126:
# 1131
 yyzzz=_tmp23B;
goto _LL124;_LL127:;_LL128:
(int)_throw((void*)& Cyc_yyfail_YY10);_LL124:;}
# 1135
return yyzzz;}
# 1137
union Cyc_YYSTYPE Cyc_YY10(struct _tuple22*yy1){union Cyc_YYSTYPE _tmp9C8;return((_tmp9C8.YY10).val=yy1,(((_tmp9C8.YY10).tag=18,_tmp9C8)));}static char _tmp23E[17]="list_t<pat_t,`H>";
# 1126 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp23E,_tmp23E,_tmp23E + 17}};
# 1128
struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp23F=yy1;struct Cyc_List_List*_tmp240;_LL12A: if((_tmp23F.YY11).tag != 19)goto _LL12C;_tmp240=(struct Cyc_List_List*)(_tmp23F.YY11).val;_LL12B:
# 1132
 yyzzz=_tmp240;
goto _LL129;_LL12C:;_LL12D:
(int)_throw((void*)& Cyc_yyfail_YY11);_LL129:;}
# 1136
return yyzzz;}
# 1138
union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9C9;return((_tmp9C9.YY11).val=yy1,(((_tmp9C9.YY11).tag=19,_tmp9C9)));}static char _tmp243[36]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1127 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp243,_tmp243,_tmp243 + 36}};
# 1129
struct _tuple23*Cyc_yyget_YY12(union Cyc_YYSTYPE yy1){
struct _tuple23*yyzzz;
{union Cyc_YYSTYPE _tmp244=yy1;struct _tuple23*_tmp245;_LL12F: if((_tmp244.YY12).tag != 20)goto _LL131;_tmp245=(struct _tuple23*)(_tmp244.YY12).val;_LL130:
# 1133
 yyzzz=_tmp245;
goto _LL12E;_LL131:;_LL132:
(int)_throw((void*)& Cyc_yyfail_YY12);_LL12E:;}
# 1137
return yyzzz;}
# 1139
union Cyc_YYSTYPE Cyc_YY12(struct _tuple23*yy1){union Cyc_YYSTYPE _tmp9CA;return((_tmp9CA.YY12).val=yy1,(((_tmp9CA.YY12).tag=20,_tmp9CA)));}static char _tmp248[47]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1128 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp248,_tmp248,_tmp248 + 47}};
# 1130
struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp249=yy1;struct Cyc_List_List*_tmp24A;_LL134: if((_tmp249.YY13).tag != 21)goto _LL136;_tmp24A=(struct Cyc_List_List*)(_tmp249.YY13).val;_LL135:
# 1134
 yyzzz=_tmp24A;
goto _LL133;_LL136:;_LL137:
(int)_throw((void*)& Cyc_yyfail_YY13);_LL133:;}
# 1138
return yyzzz;}
# 1140
union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9CB;return((_tmp9CB.YY13).val=yy1,(((_tmp9CB.YY13).tag=21,_tmp9CB)));}static char _tmp24D[58]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1129 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp24D,_tmp24D,_tmp24D + 58}};
# 1131
struct _tuple22*Cyc_yyget_YY14(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp24E=yy1;struct _tuple22*_tmp24F;_LL139: if((_tmp24E.YY14).tag != 22)goto _LL13B;_tmp24F=(struct _tuple22*)(_tmp24E.YY14).val;_LL13A:
# 1135
 yyzzz=_tmp24F;
goto _LL138;_LL13B:;_LL13C:
(int)_throw((void*)& Cyc_yyfail_YY14);_LL138:;}
# 1139
return yyzzz;}
# 1141
union Cyc_YYSTYPE Cyc_YY14(struct _tuple22*yy1){union Cyc_YYSTYPE _tmp9CC;return((_tmp9CC.YY14).val=yy1,(((_tmp9CC.YY14).tag=22,_tmp9CC)));}static char _tmp252[9]="fndecl_t";
# 1130 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp252,_tmp252,_tmp252 + 9}};
# 1132
struct Cyc_Absyn_Fndecl*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Fndecl*yyzzz;
{union Cyc_YYSTYPE _tmp253=yy1;struct Cyc_Absyn_Fndecl*_tmp254;_LL13E: if((_tmp253.YY15).tag != 23)goto _LL140;_tmp254=(struct Cyc_Absyn_Fndecl*)(_tmp253.YY15).val;_LL13F:
# 1136
 yyzzz=_tmp254;
goto _LL13D;_LL140:;_LL141:
(int)_throw((void*)& Cyc_yyfail_YY15);_LL13D:;}
# 1140
return yyzzz;}
# 1142
union Cyc_YYSTYPE Cyc_YY15(struct Cyc_Absyn_Fndecl*yy1){union Cyc_YYSTYPE _tmp9CD;return((_tmp9CD.YY15).val=yy1,(((_tmp9CD.YY15).tag=23,_tmp9CD)));}static char _tmp257[18]="list_t<decl_t,`H>";
# 1131 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp257,_tmp257,_tmp257 + 18}};
# 1133
struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp258=yy1;struct Cyc_List_List*_tmp259;_LL143: if((_tmp258.YY16).tag != 24)goto _LL145;_tmp259=(struct Cyc_List_List*)(_tmp258.YY16).val;_LL144:
# 1137
 yyzzz=_tmp259;
goto _LL142;_LL145:;_LL146:
(int)_throw((void*)& Cyc_yyfail_YY16);_LL142:;}
# 1141
return yyzzz;}
# 1143
union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9CE;return((_tmp9CE.YY16).val=yy1,(((_tmp9CE.YY16).tag=24,_tmp9CE)));}static char _tmp25C[12]="decl_spec_t";
# 1133 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY17={Cyc_Core_Failure,{_tmp25C,_tmp25C,_tmp25C + 12}};
# 1135
struct Cyc_Parse_Declaration_spec Cyc_yyget_YY17(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declaration_spec yyzzz;
{union Cyc_YYSTYPE _tmp25D=yy1;struct Cyc_Parse_Declaration_spec _tmp25E;_LL148: if((_tmp25D.YY17).tag != 25)goto _LL14A;_tmp25E=(struct Cyc_Parse_Declaration_spec)(_tmp25D.YY17).val;_LL149:
# 1139
 yyzzz=_tmp25E;
goto _LL147;_LL14A:;_LL14B:
(int)_throw((void*)& Cyc_yyfail_YY17);_LL147:;}
# 1143
return yyzzz;}
# 1145
union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Parse_Declaration_spec yy1){union Cyc_YYSTYPE _tmp9CF;return((_tmp9CF.YY17).val=yy1,(((_tmp9CF.YY17).tag=25,_tmp9CF)));}static char _tmp261[31]="$(declarator_t<`yy>,exp_opt_t)";
# 1134 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY18={Cyc_Core_Failure,{_tmp261,_tmp261,_tmp261 + 31}};
# 1136
struct _tuple11 Cyc_yyget_YY18(union Cyc_YYSTYPE yy1){
struct _tuple11 yyzzz;
{union Cyc_YYSTYPE _tmp262=yy1;struct _tuple11 _tmp263;_LL14D: if((_tmp262.YY18).tag != 26)goto _LL14F;_tmp263=(struct _tuple11)(_tmp262.YY18).val;_LL14E:
# 1140
 yyzzz=_tmp263;
goto _LL14C;_LL14F:;_LL150:
(int)_throw((void*)& Cyc_yyfail_YY18);_LL14C:;}
# 1144
return yyzzz;}
# 1146
union Cyc_YYSTYPE Cyc_YY18(struct _tuple11 yy1){union Cyc_YYSTYPE _tmp9D0;return((_tmp9D0.YY18).val=yy1,(((_tmp9D0.YY18).tag=26,_tmp9D0)));}static char _tmp266[23]="declarator_list_t<`yy>";
# 1135 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp266,_tmp266,_tmp266 + 23}};
# 1137
struct _tuple12*Cyc_yyget_YY19(union Cyc_YYSTYPE yy1){
struct _tuple12*yyzzz;
{union Cyc_YYSTYPE _tmp267=yy1;struct _tuple12*_tmp268;_LL152: if((_tmp267.YY19).tag != 27)goto _LL154;_tmp268=(struct _tuple12*)(_tmp267.YY19).val;_LL153:
# 1141
 yyzzz=_tmp268;
goto _LL151;_LL154:;_LL155:
(int)_throw((void*)& Cyc_yyfail_YY19);_LL151:;}
# 1145
return yyzzz;}
# 1147
union Cyc_YYSTYPE Cyc_YY19(struct _tuple12*yy1){union Cyc_YYSTYPE _tmp9D1;return((_tmp9D1.YY19).val=yy1,(((_tmp9D1.YY19).tag=27,_tmp9D1)));}static char _tmp26B[19]="storage_class_t@`H";
# 1136 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp26B,_tmp26B,_tmp26B + 19}};
# 1138
enum Cyc_Parse_Storage_class*Cyc_yyget_YY20(union Cyc_YYSTYPE yy1){
enum Cyc_Parse_Storage_class*yyzzz;
{union Cyc_YYSTYPE _tmp26C=yy1;enum Cyc_Parse_Storage_class*_tmp26D;_LL157: if((_tmp26C.YY20).tag != 28)goto _LL159;_tmp26D=(enum Cyc_Parse_Storage_class*)(_tmp26C.YY20).val;_LL158:
# 1142
 yyzzz=_tmp26D;
goto _LL156;_LL159:;_LL15A:
(int)_throw((void*)& Cyc_yyfail_YY20);_LL156:;}
# 1146
return yyzzz;}
# 1148
union Cyc_YYSTYPE Cyc_YY20(enum Cyc_Parse_Storage_class*yy1){union Cyc_YYSTYPE _tmp9D2;return((_tmp9D2.YY20).val=yy1,(((_tmp9D2.YY20).tag=28,_tmp9D2)));}static char _tmp270[17]="type_specifier_t";
# 1137 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp270,_tmp270,_tmp270 + 17}};
# 1139
struct Cyc_Parse_Type_specifier Cyc_yyget_YY21(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Type_specifier yyzzz;
{union Cyc_YYSTYPE _tmp271=yy1;struct Cyc_Parse_Type_specifier _tmp272;_LL15C: if((_tmp271.YY21).tag != 29)goto _LL15E;_tmp272=(struct Cyc_Parse_Type_specifier)(_tmp271.YY21).val;_LL15D:
# 1143
 yyzzz=_tmp272;
goto _LL15B;_LL15E:;_LL15F:
(int)_throw((void*)& Cyc_yyfail_YY21);_LL15B:;}
# 1147
return yyzzz;}
# 1149
union Cyc_YYSTYPE Cyc_YY21(struct Cyc_Parse_Type_specifier yy1){union Cyc_YYSTYPE _tmp9D3;return((_tmp9D3.YY21).val=yy1,(((_tmp9D3.YY21).tag=29,_tmp9D3)));}static char _tmp275[12]="aggr_kind_t";
# 1139 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp275,_tmp275,_tmp275 + 12}};
# 1141
enum Cyc_Absyn_AggrKind Cyc_yyget_YY22(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_AggrKind yyzzz;
{union Cyc_YYSTYPE _tmp276=yy1;enum Cyc_Absyn_AggrKind _tmp277;_LL161: if((_tmp276.YY22).tag != 30)goto _LL163;_tmp277=(enum Cyc_Absyn_AggrKind)(_tmp276.YY22).val;_LL162:
# 1145
 yyzzz=_tmp277;
goto _LL160;_LL163:;_LL164:
(int)_throw((void*)& Cyc_yyfail_YY22);_LL160:;}
# 1149
return yyzzz;}
# 1151
union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Absyn_AggrKind yy1){union Cyc_YYSTYPE _tmp9D4;return((_tmp9D4.YY22).val=yy1,(((_tmp9D4.YY22).tag=30,_tmp9D4)));}static char _tmp27A[8]="tqual_t";
# 1140 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp27A,_tmp27A,_tmp27A + 8}};
# 1142
struct Cyc_Absyn_Tqual Cyc_yyget_YY23(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Tqual yyzzz;
{union Cyc_YYSTYPE _tmp27B=yy1;struct Cyc_Absyn_Tqual _tmp27C;_LL166: if((_tmp27B.YY23).tag != 31)goto _LL168;_tmp27C=(struct Cyc_Absyn_Tqual)(_tmp27B.YY23).val;_LL167:
# 1146
 yyzzz=_tmp27C;
goto _LL165;_LL168:;_LL169:
(int)_throw((void*)& Cyc_yyfail_YY23);_LL165:;}
# 1150
return yyzzz;}
# 1152
union Cyc_YYSTYPE Cyc_YY23(struct Cyc_Absyn_Tqual yy1){union Cyc_YYSTYPE _tmp9D5;return((_tmp9D5.YY23).val=yy1,(((_tmp9D5.YY23).tag=31,_tmp9D5)));}static char _tmp27F[23]="list_t<aggrfield_t,`H>";
# 1141 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY24={Cyc_Core_Failure,{_tmp27F,_tmp27F,_tmp27F + 23}};
# 1143
struct Cyc_List_List*Cyc_yyget_YY24(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp280=yy1;struct Cyc_List_List*_tmp281;_LL16B: if((_tmp280.YY24).tag != 32)goto _LL16D;_tmp281=(struct Cyc_List_List*)(_tmp280.YY24).val;_LL16C:
# 1147
 yyzzz=_tmp281;
goto _LL16A;_LL16D:;_LL16E:
(int)_throw((void*)& Cyc_yyfail_YY24);_LL16A:;}
# 1151
return yyzzz;}
# 1153
union Cyc_YYSTYPE Cyc_YY24(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D6;return((_tmp9D6.YY24).val=yy1,(((_tmp9D6.YY24).tag=32,_tmp9D6)));}static char _tmp284[34]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1142 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY25={Cyc_Core_Failure,{_tmp284,_tmp284,_tmp284 + 34}};
# 1144
struct Cyc_List_List*Cyc_yyget_YY25(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp285=yy1;struct Cyc_List_List*_tmp286;_LL170: if((_tmp285.YY25).tag != 33)goto _LL172;_tmp286=(struct Cyc_List_List*)(_tmp285.YY25).val;_LL171:
# 1148
 yyzzz=_tmp286;
goto _LL16F;_LL172:;_LL173:
(int)_throw((void*)& Cyc_yyfail_YY25);_LL16F:;}
# 1152
return yyzzz;}
# 1154
union Cyc_YYSTYPE Cyc_YY25(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D7;return((_tmp9D7.YY25).val=yy1,(((_tmp9D7.YY25).tag=33,_tmp9D7)));}static char _tmp289[33]="list_t<type_modifier_t<`yy>,`yy>";
# 1143 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp289,_tmp289,_tmp289 + 33}};
# 1145
struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp28A=yy1;struct Cyc_List_List*_tmp28B;_LL175: if((_tmp28A.YY26).tag != 34)goto _LL177;_tmp28B=(struct Cyc_List_List*)(_tmp28A.YY26).val;_LL176:
# 1149
 yyzzz=_tmp28B;
goto _LL174;_LL177:;_LL178:
(int)_throw((void*)& Cyc_yyfail_YY26);_LL174:;}
# 1153
return yyzzz;}
# 1155
union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D8;return((_tmp9D8.YY26).val=yy1,(((_tmp9D8.YY26).tag=34,_tmp9D8)));}static char _tmp28E[18]="declarator_t<`yy>";
# 1144 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp28E,_tmp28E,_tmp28E + 18}};
# 1146
struct Cyc_Parse_Declarator Cyc_yyget_YY27(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declarator yyzzz;
{union Cyc_YYSTYPE _tmp28F=yy1;struct Cyc_Parse_Declarator _tmp290;_LL17A: if((_tmp28F.YY27).tag != 35)goto _LL17C;_tmp290=(struct Cyc_Parse_Declarator)(_tmp28F.YY27).val;_LL17B:
# 1150
 yyzzz=_tmp290;
goto _LL179;_LL17C:;_LL17D:
(int)_throw((void*)& Cyc_yyfail_YY27);_LL179:;}
# 1154
return yyzzz;}
# 1156
union Cyc_YYSTYPE Cyc_YY27(struct Cyc_Parse_Declarator yy1){union Cyc_YYSTYPE _tmp9D9;return((_tmp9D9.YY27).val=yy1,(((_tmp9D9.YY27).tag=35,_tmp9D9)));}static char _tmp293[45]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1145 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp293,_tmp293,_tmp293 + 45}};
# 1147
struct _tuple24*Cyc_yyget_YY28(union Cyc_YYSTYPE yy1){
struct _tuple24*yyzzz;
{union Cyc_YYSTYPE _tmp294=yy1;struct _tuple24*_tmp295;_LL17F: if((_tmp294.YY28).tag != 36)goto _LL181;_tmp295=(struct _tuple24*)(_tmp294.YY28).val;_LL180:
# 1151
 yyzzz=_tmp295;
goto _LL17E;_LL181:;_LL182:
(int)_throw((void*)& Cyc_yyfail_YY28);_LL17E:;}
# 1155
return yyzzz;}
# 1157
union Cyc_YYSTYPE Cyc_YY28(struct _tuple24*yy1){union Cyc_YYSTYPE _tmp9DA;return((_tmp9DA.YY28).val=yy1,(((_tmp9DA.YY28).tag=36,_tmp9DA)));}static char _tmp298[57]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1146 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY29={Cyc_Core_Failure,{_tmp298,_tmp298,_tmp298 + 57}};
# 1148
struct Cyc_List_List*Cyc_yyget_YY29(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp299=yy1;struct Cyc_List_List*_tmp29A;_LL184: if((_tmp299.YY29).tag != 37)goto _LL186;_tmp29A=(struct Cyc_List_List*)(_tmp299.YY29).val;_LL185:
# 1152
 yyzzz=_tmp29A;
goto _LL183;_LL186:;_LL187:
(int)_throw((void*)& Cyc_yyfail_YY29);_LL183:;}
# 1156
return yyzzz;}
# 1158
union Cyc_YYSTYPE Cyc_YY29(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9DB;return((_tmp9DB.YY29).val=yy1,(((_tmp9DB.YY29).tag=37,_tmp9DB)));}static char _tmp29D[26]="abstractdeclarator_t<`yy>";
# 1147 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY30={Cyc_Core_Failure,{_tmp29D,_tmp29D,_tmp29D + 26}};
# 1149
struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY30(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Abstractdeclarator yyzzz;
{union Cyc_YYSTYPE _tmp29E=yy1;struct Cyc_Parse_Abstractdeclarator _tmp29F;_LL189: if((_tmp29E.YY30).tag != 38)goto _LL18B;_tmp29F=(struct Cyc_Parse_Abstractdeclarator)(_tmp29E.YY30).val;_LL18A:
# 1153
 yyzzz=_tmp29F;
goto _LL188;_LL18B:;_LL18C:
(int)_throw((void*)& Cyc_yyfail_YY30);_LL188:;}
# 1157
return yyzzz;}
# 1159
union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator yy1){union Cyc_YYSTYPE _tmp9DC;return((_tmp9DC.YY30).val=yy1,(((_tmp9DC.YY30).tag=38,_tmp9DC)));}static char _tmp2A2[5]="bool";
# 1148 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY31={Cyc_Core_Failure,{_tmp2A2,_tmp2A2,_tmp2A2 + 5}};
# 1150
int Cyc_yyget_YY31(union Cyc_YYSTYPE yy1){
int yyzzz;
{union Cyc_YYSTYPE _tmp2A3=yy1;int _tmp2A4;_LL18E: if((_tmp2A3.YY31).tag != 39)goto _LL190;_tmp2A4=(int)(_tmp2A3.YY31).val;_LL18F:
# 1154
 yyzzz=_tmp2A4;
goto _LL18D;_LL190:;_LL191:
(int)_throw((void*)& Cyc_yyfail_YY31);_LL18D:;}
# 1158
return yyzzz;}
# 1160
union Cyc_YYSTYPE Cyc_YY31(int yy1){union Cyc_YYSTYPE _tmp9DD;return((_tmp9DD.YY31).val=yy1,(((_tmp9DD.YY31).tag=39,_tmp9DD)));}static char _tmp2A7[8]="scope_t";
# 1149 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp2A7,_tmp2A7,_tmp2A7 + 8}};
# 1151
enum Cyc_Absyn_Scope Cyc_yyget_YY32(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Scope yyzzz;
{union Cyc_YYSTYPE _tmp2A8=yy1;enum Cyc_Absyn_Scope _tmp2A9;_LL193: if((_tmp2A8.YY32).tag != 40)goto _LL195;_tmp2A9=(enum Cyc_Absyn_Scope)(_tmp2A8.YY32).val;_LL194:
# 1155
 yyzzz=_tmp2A9;
goto _LL192;_LL195:;_LL196:
(int)_throw((void*)& Cyc_yyfail_YY32);_LL192:;}
# 1159
return yyzzz;}
# 1161
union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope yy1){union Cyc_YYSTYPE _tmp9DE;return((_tmp9DE.YY32).val=yy1,(((_tmp9DE.YY32).tag=40,_tmp9DE)));}static char _tmp2AC[16]="datatypefield_t";
# 1150 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY33={Cyc_Core_Failure,{_tmp2AC,_tmp2AC,_tmp2AC + 16}};
# 1152
struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY33(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Datatypefield*yyzzz;
{union Cyc_YYSTYPE _tmp2AD=yy1;struct Cyc_Absyn_Datatypefield*_tmp2AE;_LL198: if((_tmp2AD.YY33).tag != 41)goto _LL19A;_tmp2AE=(struct Cyc_Absyn_Datatypefield*)(_tmp2AD.YY33).val;_LL199:
# 1156
 yyzzz=_tmp2AE;
goto _LL197;_LL19A:;_LL19B:
(int)_throw((void*)& Cyc_yyfail_YY33);_LL197:;}
# 1160
return yyzzz;}
# 1162
union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){union Cyc_YYSTYPE _tmp9DF;return((_tmp9DF.YY33).val=yy1,(((_tmp9DF.YY33).tag=41,_tmp9DF)));}static char _tmp2B1[27]="list_t<datatypefield_t,`H>";
# 1151 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp2B1,_tmp2B1,_tmp2B1 + 27}};
# 1153
struct Cyc_List_List*Cyc_yyget_YY34(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2B2=yy1;struct Cyc_List_List*_tmp2B3;_LL19D: if((_tmp2B2.YY34).tag != 42)goto _LL19F;_tmp2B3=(struct Cyc_List_List*)(_tmp2B2.YY34).val;_LL19E:
# 1157
 yyzzz=_tmp2B3;
goto _LL19C;_LL19F:;_LL1A0:
(int)_throw((void*)& Cyc_yyfail_YY34);_LL19C:;}
# 1161
return yyzzz;}
# 1163
union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E0;return((_tmp9E0.YY34).val=yy1,(((_tmp9E0.YY34).tag=42,_tmp9E0)));}static char _tmp2B6[41]="$(tqual_t,type_specifier_t,attributes_t)";
# 1152 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp2B6,_tmp2B6,_tmp2B6 + 41}};
# 1154
struct _tuple25 Cyc_yyget_YY35(union Cyc_YYSTYPE yy1){
struct _tuple25 yyzzz;
{union Cyc_YYSTYPE _tmp2B7=yy1;struct _tuple25 _tmp2B8;_LL1A2: if((_tmp2B7.YY35).tag != 43)goto _LL1A4;_tmp2B8=(struct _tuple25)(_tmp2B7.YY35).val;_LL1A3:
# 1158
 yyzzz=_tmp2B8;
goto _LL1A1;_LL1A4:;_LL1A5:
(int)_throw((void*)& Cyc_yyfail_YY35);_LL1A1:;}
# 1162
return yyzzz;}
# 1164
union Cyc_YYSTYPE Cyc_YY35(struct _tuple25 yy1){union Cyc_YYSTYPE _tmp9E1;return((_tmp9E1.YY35).val=yy1,(((_tmp9E1.YY35).tag=43,_tmp9E1)));}static char _tmp2BB[17]="list_t<var_t,`H>";
# 1153 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY36={Cyc_Core_Failure,{_tmp2BB,_tmp2BB,_tmp2BB + 17}};
# 1155
struct Cyc_List_List*Cyc_yyget_YY36(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2BC=yy1;struct Cyc_List_List*_tmp2BD;_LL1A7: if((_tmp2BC.YY36).tag != 44)goto _LL1A9;_tmp2BD=(struct Cyc_List_List*)(_tmp2BC.YY36).val;_LL1A8:
# 1159
 yyzzz=_tmp2BD;
goto _LL1A6;_LL1A9:;_LL1AA:
(int)_throw((void*)& Cyc_yyfail_YY36);_LL1A6:;}
# 1163
return yyzzz;}
# 1165
union Cyc_YYSTYPE Cyc_YY36(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E2;return((_tmp9E2.YY36).val=yy1,(((_tmp9E2.YY36).tag=44,_tmp9E2)));}static char _tmp2C0[31]="$(var_opt_t,tqual_t,type_t)@`H";
# 1154 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp2C0,_tmp2C0,_tmp2C0 + 31}};
# 1156
struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE yy1){
struct _tuple8*yyzzz;
{union Cyc_YYSTYPE _tmp2C1=yy1;struct _tuple8*_tmp2C2;_LL1AC: if((_tmp2C1.YY37).tag != 45)goto _LL1AE;_tmp2C2=(struct _tuple8*)(_tmp2C1.YY37).val;_LL1AD:
# 1160
 yyzzz=_tmp2C2;
goto _LL1AB;_LL1AE:;_LL1AF:
(int)_throw((void*)& Cyc_yyfail_YY37);_LL1AB:;}
# 1164
return yyzzz;}
# 1166
union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1){union Cyc_YYSTYPE _tmp9E3;return((_tmp9E3.YY37).val=yy1,(((_tmp9E3.YY37).tag=45,_tmp9E3)));}static char _tmp2C5[42]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1155 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp2C5,_tmp2C5,_tmp2C5 + 42}};
# 1157
struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2C6=yy1;struct Cyc_List_List*_tmp2C7;_LL1B1: if((_tmp2C6.YY38).tag != 46)goto _LL1B3;_tmp2C7=(struct Cyc_List_List*)(_tmp2C6.YY38).val;_LL1B2:
# 1161
 yyzzz=_tmp2C7;
goto _LL1B0;_LL1B3:;_LL1B4:
(int)_throw((void*)& Cyc_yyfail_YY38);_LL1B0:;}
# 1165
return yyzzz;}
# 1167
union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E4;return((_tmp9E4.YY38).val=yy1,(((_tmp9E4.YY38).tag=46,_tmp9E4)));}static char _tmp2CA[115]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1156 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp2CA,_tmp2CA,_tmp2CA + 115}};
# 1158
struct _tuple26*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1){
struct _tuple26*yyzzz;
{union Cyc_YYSTYPE _tmp2CB=yy1;struct _tuple26*_tmp2CC;_LL1B6: if((_tmp2CB.YY39).tag != 47)goto _LL1B8;_tmp2CC=(struct _tuple26*)(_tmp2CB.YY39).val;_LL1B7:
# 1162
 yyzzz=_tmp2CC;
goto _LL1B5;_LL1B8:;_LL1B9:
(int)_throw((void*)& Cyc_yyfail_YY39);_LL1B5:;}
# 1166
return yyzzz;}
# 1168
union Cyc_YYSTYPE Cyc_YY39(struct _tuple26*yy1){union Cyc_YYSTYPE _tmp9E5;return((_tmp9E5.YY39).val=yy1,(((_tmp9E5.YY39).tag=47,_tmp9E5)));}static char _tmp2CF[18]="list_t<type_t,`H>";
# 1157 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY40={Cyc_Core_Failure,{_tmp2CF,_tmp2CF,_tmp2CF + 18}};
# 1159
struct Cyc_List_List*Cyc_yyget_YY40(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2D0=yy1;struct Cyc_List_List*_tmp2D1;_LL1BB: if((_tmp2D0.YY40).tag != 48)goto _LL1BD;_tmp2D1=(struct Cyc_List_List*)(_tmp2D0.YY40).val;_LL1BC:
# 1163
 yyzzz=_tmp2D1;
goto _LL1BA;_LL1BD:;_LL1BE:
(int)_throw((void*)& Cyc_yyfail_YY40);_LL1BA:;}
# 1167
return yyzzz;}
# 1169
union Cyc_YYSTYPE Cyc_YY40(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E6;return((_tmp9E6.YY40).val=yy1,(((_tmp9E6.YY40).tag=48,_tmp9E6)));}static char _tmp2D4[24]="list_t<designator_t,`H>";
# 1159 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp2D4,_tmp2D4,_tmp2D4 + 24}};
# 1161
struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2D5=yy1;struct Cyc_List_List*_tmp2D6;_LL1C0: if((_tmp2D5.YY41).tag != 49)goto _LL1C2;_tmp2D6=(struct Cyc_List_List*)(_tmp2D5.YY41).val;_LL1C1:
# 1165
 yyzzz=_tmp2D6;
goto _LL1BF;_LL1C2:;_LL1C3:
(int)_throw((void*)& Cyc_yyfail_YY41);_LL1BF:;}
# 1169
return yyzzz;}
# 1171
union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E7;return((_tmp9E7.YY41).val=yy1,(((_tmp9E7.YY41).tag=49,_tmp9E7)));}static char _tmp2D9[13]="designator_t";
# 1160 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp2D9,_tmp2D9,_tmp2D9 + 13}};
# 1162
void*Cyc_yyget_YY42(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2DA=yy1;void*_tmp2DB;_LL1C5: if((_tmp2DA.YY42).tag != 50)goto _LL1C7;_tmp2DB=(void*)(_tmp2DA.YY42).val;_LL1C6:
# 1166
 yyzzz=_tmp2DB;
goto _LL1C4;_LL1C7:;_LL1C8:
(int)_throw((void*)& Cyc_yyfail_YY42);_LL1C4:;}
# 1170
return yyzzz;}
# 1172
union Cyc_YYSTYPE Cyc_YY42(void*yy1){union Cyc_YYSTYPE _tmp9E8;return((_tmp9E8.YY42).val=yy1,(((_tmp9E8.YY42).tag=50,_tmp9E8)));}static char _tmp2DE[7]="kind_t";
# 1161 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{_tmp2DE,_tmp2DE,_tmp2DE + 7}};
# 1163
struct Cyc_Absyn_Kind*Cyc_yyget_YY43(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Kind*yyzzz;
{union Cyc_YYSTYPE _tmp2DF=yy1;struct Cyc_Absyn_Kind*_tmp2E0;_LL1CA: if((_tmp2DF.YY43).tag != 51)goto _LL1CC;_tmp2E0=(struct Cyc_Absyn_Kind*)(_tmp2DF.YY43).val;_LL1CB:
# 1167
 yyzzz=_tmp2E0;
goto _LL1C9;_LL1CC:;_LL1CD:
(int)_throw((void*)& Cyc_yyfail_YY43);_LL1C9:;}
# 1171
return yyzzz;}
# 1173
union Cyc_YYSTYPE Cyc_YY43(struct Cyc_Absyn_Kind*yy1){union Cyc_YYSTYPE _tmp9E9;return((_tmp9E9.YY43).val=yy1,(((_tmp9E9.YY43).tag=51,_tmp9E9)));}static char _tmp2E3[7]="type_t";
# 1162 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp2E3,_tmp2E3,_tmp2E3 + 7}};
# 1164
void*Cyc_yyget_YY44(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2E4=yy1;void*_tmp2E5;_LL1CF: if((_tmp2E4.YY44).tag != 52)goto _LL1D1;_tmp2E5=(void*)(_tmp2E4.YY44).val;_LL1D0:
# 1168
 yyzzz=_tmp2E5;
goto _LL1CE;_LL1D1:;_LL1D2:
(int)_throw((void*)& Cyc_yyfail_YY44);_LL1CE:;}
# 1172
return yyzzz;}
# 1174
union Cyc_YYSTYPE Cyc_YY44(void*yy1){union Cyc_YYSTYPE _tmp9EA;return((_tmp9EA.YY44).val=yy1,(((_tmp9EA.YY44).tag=52,_tmp9EA)));}static char _tmp2E8[23]="list_t<attribute_t,`H>";
# 1163 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY45={Cyc_Core_Failure,{_tmp2E8,_tmp2E8,_tmp2E8 + 23}};
# 1165
struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2E9=yy1;struct Cyc_List_List*_tmp2EA;_LL1D4: if((_tmp2E9.YY45).tag != 53)goto _LL1D6;_tmp2EA=(struct Cyc_List_List*)(_tmp2E9.YY45).val;_LL1D5:
# 1169
 yyzzz=_tmp2EA;
goto _LL1D3;_LL1D6:;_LL1D7:
(int)_throw((void*)& Cyc_yyfail_YY45);_LL1D3:;}
# 1173
return yyzzz;}
# 1175
union Cyc_YYSTYPE Cyc_YY45(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9EB;return((_tmp9EB.YY45).val=yy1,(((_tmp9EB.YY45).tag=53,_tmp9EB)));}static char _tmp2ED[12]="attribute_t";
# 1164 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp2ED,_tmp2ED,_tmp2ED + 12}};
# 1166
void*Cyc_yyget_YY46(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2EE=yy1;void*_tmp2EF;_LL1D9: if((_tmp2EE.YY46).tag != 54)goto _LL1DB;_tmp2EF=(void*)(_tmp2EE.YY46).val;_LL1DA:
# 1170
 yyzzz=_tmp2EF;
goto _LL1D8;_LL1DB:;_LL1DC:
(int)_throw((void*)& Cyc_yyfail_YY46);_LL1D8:;}
# 1174
return yyzzz;}
# 1176
union Cyc_YYSTYPE Cyc_YY46(void*yy1){union Cyc_YYSTYPE _tmp9EC;return((_tmp9EC.YY46).val=yy1,(((_tmp9EC.YY46).tag=54,_tmp9EC)));}static char _tmp2F2[12]="enumfield_t";
# 1165 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp2F2,_tmp2F2,_tmp2F2 + 12}};
# 1167
struct Cyc_Absyn_Enumfield*Cyc_yyget_YY47(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Enumfield*yyzzz;
{union Cyc_YYSTYPE _tmp2F3=yy1;struct Cyc_Absyn_Enumfield*_tmp2F4;_LL1DE: if((_tmp2F3.YY47).tag != 55)goto _LL1E0;_tmp2F4=(struct Cyc_Absyn_Enumfield*)(_tmp2F3.YY47).val;_LL1DF:
# 1171
 yyzzz=_tmp2F4;
goto _LL1DD;_LL1E0:;_LL1E1:
(int)_throw((void*)& Cyc_yyfail_YY47);_LL1DD:;}
# 1175
return yyzzz;}
# 1177
union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){union Cyc_YYSTYPE _tmp9ED;return((_tmp9ED.YY47).val=yy1,(((_tmp9ED.YY47).tag=55,_tmp9ED)));}static char _tmp2F7[23]="list_t<enumfield_t,`H>";
# 1166 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY48={Cyc_Core_Failure,{_tmp2F7,_tmp2F7,_tmp2F7 + 23}};
# 1168
struct Cyc_List_List*Cyc_yyget_YY48(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2F8=yy1;struct Cyc_List_List*_tmp2F9;_LL1E3: if((_tmp2F8.YY48).tag != 56)goto _LL1E5;_tmp2F9=(struct Cyc_List_List*)(_tmp2F8.YY48).val;_LL1E4:
# 1172
 yyzzz=_tmp2F9;
goto _LL1E2;_LL1E5:;_LL1E6:
(int)_throw((void*)& Cyc_yyfail_YY48);_LL1E2:;}
# 1176
return yyzzz;}
# 1178
union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9EE;return((_tmp9EE.YY48).val=yy1,(((_tmp9EE.YY48).tag=56,_tmp9EE)));}static char _tmp2FC[11]="type_opt_t";
# 1167 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp2FC,_tmp2FC,_tmp2FC + 11}};
# 1169
void*Cyc_yyget_YY49(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2FD=yy1;void*_tmp2FE;_LL1E8: if((_tmp2FD.YY49).tag != 57)goto _LL1EA;_tmp2FE=(void*)(_tmp2FD.YY49).val;_LL1E9:
# 1173
 yyzzz=_tmp2FE;
goto _LL1E7;_LL1EA:;_LL1EB:
(int)_throw((void*)& Cyc_yyfail_YY49);_LL1E7:;}
# 1177
return yyzzz;}
# 1179
union Cyc_YYSTYPE Cyc_YY49(void*yy1){union Cyc_YYSTYPE _tmp9EF;return((_tmp9EF.YY49).val=yy1,(((_tmp9EF.YY49).tag=57,_tmp9EF)));}static char _tmp301[31]="list_t<$(type_t,type_t)@`H,`H>";
# 1168 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp301,_tmp301,_tmp301 + 31}};
# 1170
struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp302=yy1;struct Cyc_List_List*_tmp303;_LL1ED: if((_tmp302.YY50).tag != 58)goto _LL1EF;_tmp303=(struct Cyc_List_List*)(_tmp302.YY50).val;_LL1EE:
# 1174
 yyzzz=_tmp303;
goto _LL1EC;_LL1EF:;_LL1F0:
(int)_throw((void*)& Cyc_yyfail_YY50);_LL1EC:;}
# 1178
return yyzzz;}
# 1180
union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F0;return((_tmp9F0.YY50).val=yy1,(((_tmp9F0.YY50).tag=58,_tmp9F0)));}static char _tmp306[15]="conref_t<bool>";
# 1169 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY51={Cyc_Core_Failure,{_tmp306,_tmp306,_tmp306 + 15}};
# 1171
union Cyc_Absyn_Constraint*Cyc_yyget_YY51(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Constraint*yyzzz;
{union Cyc_YYSTYPE _tmp307=yy1;union Cyc_Absyn_Constraint*_tmp308;_LL1F2: if((_tmp307.YY51).tag != 59)goto _LL1F4;_tmp308=(union Cyc_Absyn_Constraint*)(_tmp307.YY51).val;_LL1F3:
# 1175
 yyzzz=_tmp308;
goto _LL1F1;_LL1F4:;_LL1F5:
(int)_throw((void*)& Cyc_yyfail_YY51);_LL1F1:;}
# 1179
return yyzzz;}
# 1181
union Cyc_YYSTYPE Cyc_YY51(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp9F1;return((_tmp9F1.YY51).val=yy1,(((_tmp9F1.YY51).tag=59,_tmp9F1)));}static char _tmp30B[45]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";
# 1170 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp30B,_tmp30B,_tmp30B + 45}};
# 1172
struct Cyc_List_List*Cyc_yyget_YY52(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp30C=yy1;struct Cyc_List_List*_tmp30D;_LL1F7: if((_tmp30C.YY52).tag != 60)goto _LL1F9;_tmp30D=(struct Cyc_List_List*)(_tmp30C.YY52).val;_LL1F8:
# 1176
 yyzzz=_tmp30D;
goto _LL1F6;_LL1F9:;_LL1FA:
(int)_throw((void*)& Cyc_yyfail_YY52);_LL1F6:;}
# 1180
return yyzzz;}
# 1182
union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F2;return((_tmp9F2.YY52).val=yy1,(((_tmp9F2.YY52).tag=60,_tmp9F2)));}static char _tmp310[20]="pointer_qual_t<`yy>";
# 1171 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY53={Cyc_Core_Failure,{_tmp310,_tmp310,_tmp310 + 20}};
# 1173
void*Cyc_yyget_YY53(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp311=yy1;void*_tmp312;_LL1FC: if((_tmp311.YY53).tag != 61)goto _LL1FE;_tmp312=(void*)(_tmp311.YY53).val;_LL1FD:
# 1177
 yyzzz=_tmp312;
goto _LL1FB;_LL1FE:;_LL1FF:
(int)_throw((void*)& Cyc_yyfail_YY53);_LL1FB:;}
# 1181
return yyzzz;}
# 1183
union Cyc_YYSTYPE Cyc_YY53(void*yy1){union Cyc_YYSTYPE _tmp9F3;return((_tmp9F3.YY53).val=yy1,(((_tmp9F3.YY53).tag=61,_tmp9F3)));}static char _tmp315[21]="pointer_quals_t<`yy>";
# 1172 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY54={Cyc_Core_Failure,{_tmp315,_tmp315,_tmp315 + 21}};
# 1174
struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp316=yy1;struct Cyc_List_List*_tmp317;_LL201: if((_tmp316.YY54).tag != 62)goto _LL203;_tmp317=(struct Cyc_List_List*)(_tmp316.YY54).val;_LL202:
# 1178
 yyzzz=_tmp317;
goto _LL200;_LL203:;_LL204:
(int)_throw((void*)& Cyc_yyfail_YY54);_LL200:;}
# 1182
return yyzzz;}
# 1184
union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F4;return((_tmp9F4.YY54).val=yy1,(((_tmp9F4.YY54).tag=62,_tmp9F4)));}static char _tmp31A[21]="$(bool,string_t<`H>)";
# 1173 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Asm_tok={Cyc_Core_Failure,{_tmp31A,_tmp31A,_tmp31A + 21}};
# 1175
struct _tuple20 Cyc_yyget_Asm_tok(union Cyc_YYSTYPE yy1){
struct _tuple20 yyzzz;
{union Cyc_YYSTYPE _tmp31B=yy1;struct _tuple20 _tmp31C;_LL206: if((_tmp31B.Asm_tok).tag != 6)goto _LL208;_tmp31C=(struct _tuple20)(_tmp31B.Asm_tok).val;_LL207:
# 1179
 yyzzz=_tmp31C;
goto _LL205;_LL208:;_LL209:
(int)_throw((void*)& Cyc_yyfail_Asm_tok);_LL205:;}
# 1183
return yyzzz;}
# 1185
union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple20 yy1){union Cyc_YYSTYPE _tmp9F5;return((_tmp9F5.Asm_tok).val=yy1,(((_tmp9F5.Asm_tok).tag=6,_tmp9F5)));}static char _tmp31F[10]="exp_opt_t";
# 1174 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY55={Cyc_Core_Failure,{_tmp31F,_tmp31F,_tmp31F + 10}};
# 1176
struct Cyc_Absyn_Exp*Cyc_yyget_YY55(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp320=yy1;struct Cyc_Absyn_Exp*_tmp321;_LL20B: if((_tmp320.YY55).tag != 63)goto _LL20D;_tmp321=(struct Cyc_Absyn_Exp*)(_tmp320.YY55).val;_LL20C:
# 1180
 yyzzz=_tmp321;
goto _LL20A;_LL20D:;_LL20E:
(int)_throw((void*)& Cyc_yyfail_YY55);_LL20A:;}
# 1184
return yyzzz;}
# 1186
union Cyc_YYSTYPE Cyc_YY55(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp9F6;return((_tmp9F6.YY55).val=yy1,(((_tmp9F6.YY55).tag=63,_tmp9F6)));}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1198
typedef struct Cyc_Yyltype Cyc_yyltype;
# 1202
struct Cyc_Yyltype Cyc_yynewloc(){
struct Cyc_Yyltype _tmp9F7;return(_tmp9F7.timestamp=0,((_tmp9F7.first_line=0,((_tmp9F7.first_column=0,((_tmp9F7.last_line=0,((_tmp9F7.last_column=0,_tmp9F7)))))))));}
# 1205
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1216 "parse.y"
static short Cyc_yytranslate[373]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,144,2,2,128,142,139,2,125,126,133,136,121,140,130,141,2,2,2,2,2,2,2,2,2,2,129,118,123,122,124,135,134,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,131,2,132,138,127,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,119,137,120,143,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117};static char _tmp324[2]="$";static char _tmp325[6]="error";static char _tmp326[12]="$undefined.";static char _tmp327[5]="AUTO";static char _tmp328[9]="REGISTER";static char _tmp329[7]="STATIC";static char _tmp32A[7]="EXTERN";static char _tmp32B[8]="TYPEDEF";static char _tmp32C[5]="VOID";static char _tmp32D[5]="CHAR";static char _tmp32E[6]="SHORT";static char _tmp32F[4]="INT";static char _tmp330[5]="LONG";static char _tmp331[6]="FLOAT";static char _tmp332[7]="DOUBLE";static char _tmp333[7]="SIGNED";static char _tmp334[9]="UNSIGNED";static char _tmp335[6]="CONST";static char _tmp336[9]="VOLATILE";static char _tmp337[9]="RESTRICT";static char _tmp338[7]="STRUCT";static char _tmp339[6]="UNION";static char _tmp33A[5]="CASE";static char _tmp33B[8]="DEFAULT";static char _tmp33C[7]="INLINE";static char _tmp33D[7]="SIZEOF";static char _tmp33E[9]="OFFSETOF";static char _tmp33F[3]="IF";static char _tmp340[5]="ELSE";static char _tmp341[7]="SWITCH";static char _tmp342[6]="WHILE";static char _tmp343[3]="DO";static char _tmp344[4]="FOR";static char _tmp345[5]="GOTO";static char _tmp346[9]="CONTINUE";static char _tmp347[6]="BREAK";static char _tmp348[7]="RETURN";static char _tmp349[5]="ENUM";static char _tmp34A[8]="NULL_kw";static char _tmp34B[4]="LET";static char _tmp34C[6]="THROW";static char _tmp34D[4]="TRY";static char _tmp34E[6]="CATCH";static char _tmp34F[7]="EXPORT";static char _tmp350[4]="NEW";static char _tmp351[9]="ABSTRACT";static char _tmp352[9]="FALLTHRU";static char _tmp353[6]="USING";static char _tmp354[10]="NAMESPACE";static char _tmp355[9]="DATATYPE";static char _tmp356[7]="MALLOC";static char _tmp357[8]="RMALLOC";static char _tmp358[7]="CALLOC";static char _tmp359[8]="RCALLOC";static char _tmp35A[5]="SWAP";static char _tmp35B[9]="REGION_T";static char _tmp35C[6]="TAG_T";static char _tmp35D[7]="REGION";static char _tmp35E[5]="RNEW";static char _tmp35F[8]="REGIONS";static char _tmp360[13]="RESET_REGION";static char _tmp361[7]="PORTON";static char _tmp362[8]="PORTOFF";static char _tmp363[12]="DYNREGION_T";static char _tmp364[8]="NUMELTS";static char _tmp365[8]="VALUEOF";static char _tmp366[10]="VALUEOF_T";static char _tmp367[9]="TAGCHECK";static char _tmp368[13]="NUMELTS_QUAL";static char _tmp369[10]="THIN_QUAL";static char _tmp36A[9]="FAT_QUAL";static char _tmp36B[13]="NOTNULL_QUAL";static char _tmp36C[14]="NULLABLE_QUAL";static char _tmp36D[14]="REQUIRES_QUAL";static char _tmp36E[13]="ENSURES_QUAL";static char _tmp36F[12]="REGION_QUAL";static char _tmp370[16]="NOZEROTERM_QUAL";static char _tmp371[14]="ZEROTERM_QUAL";static char _tmp372[12]="TAGGED_QUAL";static char _tmp373[16]="EXTENSIBLE_QUAL";static char _tmp374[15]="RESETABLE_QUAL";static char _tmp375[7]="PTR_OP";static char _tmp376[7]="INC_OP";static char _tmp377[7]="DEC_OP";static char _tmp378[8]="LEFT_OP";static char _tmp379[9]="RIGHT_OP";static char _tmp37A[6]="LE_OP";static char _tmp37B[6]="GE_OP";static char _tmp37C[6]="EQ_OP";static char _tmp37D[6]="NE_OP";static char _tmp37E[7]="AND_OP";static char _tmp37F[6]="OR_OP";static char _tmp380[11]="MUL_ASSIGN";static char _tmp381[11]="DIV_ASSIGN";static char _tmp382[11]="MOD_ASSIGN";static char _tmp383[11]="ADD_ASSIGN";static char _tmp384[11]="SUB_ASSIGN";static char _tmp385[12]="LEFT_ASSIGN";static char _tmp386[13]="RIGHT_ASSIGN";static char _tmp387[11]="AND_ASSIGN";static char _tmp388[11]="XOR_ASSIGN";static char _tmp389[10]="OR_ASSIGN";static char _tmp38A[9]="ELLIPSIS";static char _tmp38B[11]="LEFT_RIGHT";static char _tmp38C[12]="COLON_COLON";static char _tmp38D[11]="IDENTIFIER";static char _tmp38E[17]="INTEGER_CONSTANT";static char _tmp38F[7]="STRING";static char _tmp390[8]="WSTRING";static char _tmp391[19]="CHARACTER_CONSTANT";static char _tmp392[20]="WCHARACTER_CONSTANT";static char _tmp393[18]="FLOATING_CONSTANT";static char _tmp394[9]="TYPE_VAR";static char _tmp395[13]="TYPEDEF_NAME";static char _tmp396[16]="QUAL_IDENTIFIER";static char _tmp397[18]="QUAL_TYPEDEF_NAME";static char _tmp398[10]="ATTRIBUTE";static char _tmp399[4]="ASM";static char _tmp39A[4]="';'";static char _tmp39B[4]="'{'";static char _tmp39C[4]="'}'";static char _tmp39D[4]="','";static char _tmp39E[4]="'='";static char _tmp39F[4]="'<'";static char _tmp3A0[4]="'>'";static char _tmp3A1[4]="'('";static char _tmp3A2[4]="')'";static char _tmp3A3[4]="'_'";static char _tmp3A4[4]="'$'";static char _tmp3A5[4]="':'";static char _tmp3A6[4]="'.'";static char _tmp3A7[4]="'['";static char _tmp3A8[4]="']'";static char _tmp3A9[4]="'*'";static char _tmp3AA[4]="'@'";static char _tmp3AB[4]="'?'";static char _tmp3AC[4]="'+'";static char _tmp3AD[4]="'|'";static char _tmp3AE[4]="'^'";static char _tmp3AF[4]="'&'";static char _tmp3B0[4]="'-'";static char _tmp3B1[4]="'/'";static char _tmp3B2[4]="'%'";static char _tmp3B3[4]="'~'";static char _tmp3B4[4]="'!'";static char _tmp3B5[5]="prog";static char _tmp3B6[17]="translation_unit";static char _tmp3B7[12]="export_list";static char _tmp3B8[19]="export_list_values";static char _tmp3B9[21]="external_declaration";static char _tmp3BA[15]="optional_comma";static char _tmp3BB[20]="function_definition";static char _tmp3BC[21]="function_definition2";static char _tmp3BD[13]="using_action";static char _tmp3BE[15]="unusing_action";static char _tmp3BF[17]="namespace_action";static char _tmp3C0[19]="unnamespace_action";static char _tmp3C1[12]="declaration";static char _tmp3C2[19]="resetable_qual_opt";static char _tmp3C3[17]="declaration_list";static char _tmp3C4[23]="declaration_specifiers";static char _tmp3C5[24]="storage_class_specifier";static char _tmp3C6[15]="attributes_opt";static char _tmp3C7[11]="attributes";static char _tmp3C8[15]="attribute_list";static char _tmp3C9[10]="attribute";static char _tmp3CA[15]="type_specifier";static char _tmp3CB[25]="type_specifier_notypedef";static char _tmp3CC[5]="kind";static char _tmp3CD[15]="type_qualifier";static char _tmp3CE[15]="enum_specifier";static char _tmp3CF[11]="enum_field";static char _tmp3D0[22]="enum_declaration_list";static char _tmp3D1[26]="struct_or_union_specifier";static char _tmp3D2[16]="type_params_opt";static char _tmp3D3[16]="struct_or_union";static char _tmp3D4[24]="struct_declaration_list";static char _tmp3D5[25]="struct_declaration_list0";static char _tmp3D6[21]="init_declarator_list";static char _tmp3D7[22]="init_declarator_list0";static char _tmp3D8[16]="init_declarator";static char _tmp3D9[19]="struct_declaration";static char _tmp3DA[25]="specifier_qualifier_list";static char _tmp3DB[35]="notypedef_specifier_qualifier_list";static char _tmp3DC[23]="struct_declarator_list";static char _tmp3DD[24]="struct_declarator_list0";static char _tmp3DE[18]="struct_declarator";static char _tmp3DF[20]="requires_clause_opt";static char _tmp3E0[19]="ensures_clause_opt";static char _tmp3E1[19]="datatype_specifier";static char _tmp3E2[14]="qual_datatype";static char _tmp3E3[19]="datatypefield_list";static char _tmp3E4[20]="datatypefield_scope";static char _tmp3E5[14]="datatypefield";static char _tmp3E6[11]="declarator";static char _tmp3E7[23]="declarator_withtypedef";static char _tmp3E8[18]="direct_declarator";static char _tmp3E9[30]="direct_declarator_withtypedef";static char _tmp3EA[8]="pointer";static char _tmp3EB[12]="one_pointer";static char _tmp3EC[14]="pointer_quals";static char _tmp3ED[13]="pointer_qual";static char _tmp3EE[23]="pointer_null_and_bound";static char _tmp3EF[14]="pointer_bound";static char _tmp3F0[18]="zeroterm_qual_opt";static char _tmp3F1[8]="rgn_opt";static char _tmp3F2[11]="tqual_list";static char _tmp3F3[20]="parameter_type_list";static char _tmp3F4[9]="type_var";static char _tmp3F5[16]="optional_effect";static char _tmp3F6[19]="optional_rgn_order";static char _tmp3F7[10]="rgn_order";static char _tmp3F8[16]="optional_inject";static char _tmp3F9[11]="effect_set";static char _tmp3FA[14]="atomic_effect";static char _tmp3FB[11]="region_set";static char _tmp3FC[15]="parameter_list";static char _tmp3FD[22]="parameter_declaration";static char _tmp3FE[16]="identifier_list";static char _tmp3FF[17]="identifier_list0";static char _tmp400[12]="initializer";static char _tmp401[18]="array_initializer";static char _tmp402[17]="initializer_list";static char _tmp403[12]="designation";static char _tmp404[16]="designator_list";static char _tmp405[11]="designator";static char _tmp406[10]="type_name";static char _tmp407[14]="any_type_name";static char _tmp408[15]="type_name_list";static char _tmp409[20]="abstract_declarator";static char _tmp40A[27]="direct_abstract_declarator";static char _tmp40B[10]="statement";static char _tmp40C[18]="labeled_statement";static char _tmp40D[21]="expression_statement";static char _tmp40E[19]="compound_statement";static char _tmp40F[16]="block_item_list";static char _tmp410[20]="selection_statement";static char _tmp411[15]="switch_clauses";static char _tmp412[20]="iteration_statement";static char _tmp413[15]="jump_statement";static char _tmp414[12]="exp_pattern";static char _tmp415[20]="conditional_pattern";static char _tmp416[19]="logical_or_pattern";static char _tmp417[20]="logical_and_pattern";static char _tmp418[21]="inclusive_or_pattern";static char _tmp419[21]="exclusive_or_pattern";static char _tmp41A[12]="and_pattern";static char _tmp41B[17]="equality_pattern";static char _tmp41C[19]="relational_pattern";static char _tmp41D[14]="shift_pattern";static char _tmp41E[17]="additive_pattern";static char _tmp41F[23]="multiplicative_pattern";static char _tmp420[13]="cast_pattern";static char _tmp421[14]="unary_pattern";static char _tmp422[16]="postfix_pattern";static char _tmp423[16]="primary_pattern";static char _tmp424[8]="pattern";static char _tmp425[19]="tuple_pattern_list";static char _tmp426[20]="tuple_pattern_list0";static char _tmp427[14]="field_pattern";static char _tmp428[19]="field_pattern_list";static char _tmp429[20]="field_pattern_list0";static char _tmp42A[11]="expression";static char _tmp42B[22]="assignment_expression";static char _tmp42C[20]="assignment_operator";static char _tmp42D[23]="conditional_expression";static char _tmp42E[20]="constant_expression";static char _tmp42F[22]="logical_or_expression";static char _tmp430[23]="logical_and_expression";static char _tmp431[24]="inclusive_or_expression";static char _tmp432[24]="exclusive_or_expression";static char _tmp433[15]="and_expression";static char _tmp434[20]="equality_expression";static char _tmp435[22]="relational_expression";static char _tmp436[17]="shift_expression";static char _tmp437[20]="additive_expression";static char _tmp438[26]="multiplicative_expression";static char _tmp439[16]="cast_expression";static char _tmp43A[17]="unary_expression";static char _tmp43B[15]="unary_operator";static char _tmp43C[19]="postfix_expression";static char _tmp43D[17]="field_expression";static char _tmp43E[19]="primary_expression";static char _tmp43F[25]="argument_expression_list";static char _tmp440[26]="argument_expression_list0";static char _tmp441[9]="constant";static char _tmp442[20]="qual_opt_identifier";static char _tmp443[17]="qual_opt_typedef";static char _tmp444[18]="struct_union_name";static char _tmp445[11]="field_name";static char _tmp446[12]="right_angle";
# 1569 "parse.y"
static struct _dyneither_ptr Cyc_yytname[291]={{_tmp324,_tmp324,_tmp324 + 2},{_tmp325,_tmp325,_tmp325 + 6},{_tmp326,_tmp326,_tmp326 + 12},{_tmp327,_tmp327,_tmp327 + 5},{_tmp328,_tmp328,_tmp328 + 9},{_tmp329,_tmp329,_tmp329 + 7},{_tmp32A,_tmp32A,_tmp32A + 7},{_tmp32B,_tmp32B,_tmp32B + 8},{_tmp32C,_tmp32C,_tmp32C + 5},{_tmp32D,_tmp32D,_tmp32D + 5},{_tmp32E,_tmp32E,_tmp32E + 6},{_tmp32F,_tmp32F,_tmp32F + 4},{_tmp330,_tmp330,_tmp330 + 5},{_tmp331,_tmp331,_tmp331 + 6},{_tmp332,_tmp332,_tmp332 + 7},{_tmp333,_tmp333,_tmp333 + 7},{_tmp334,_tmp334,_tmp334 + 9},{_tmp335,_tmp335,_tmp335 + 6},{_tmp336,_tmp336,_tmp336 + 9},{_tmp337,_tmp337,_tmp337 + 9},{_tmp338,_tmp338,_tmp338 + 7},{_tmp339,_tmp339,_tmp339 + 6},{_tmp33A,_tmp33A,_tmp33A + 5},{_tmp33B,_tmp33B,_tmp33B + 8},{_tmp33C,_tmp33C,_tmp33C + 7},{_tmp33D,_tmp33D,_tmp33D + 7},{_tmp33E,_tmp33E,_tmp33E + 9},{_tmp33F,_tmp33F,_tmp33F + 3},{_tmp340,_tmp340,_tmp340 + 5},{_tmp341,_tmp341,_tmp341 + 7},{_tmp342,_tmp342,_tmp342 + 6},{_tmp343,_tmp343,_tmp343 + 3},{_tmp344,_tmp344,_tmp344 + 4},{_tmp345,_tmp345,_tmp345 + 5},{_tmp346,_tmp346,_tmp346 + 9},{_tmp347,_tmp347,_tmp347 + 6},{_tmp348,_tmp348,_tmp348 + 7},{_tmp349,_tmp349,_tmp349 + 5},{_tmp34A,_tmp34A,_tmp34A + 8},{_tmp34B,_tmp34B,_tmp34B + 4},{_tmp34C,_tmp34C,_tmp34C + 6},{_tmp34D,_tmp34D,_tmp34D + 4},{_tmp34E,_tmp34E,_tmp34E + 6},{_tmp34F,_tmp34F,_tmp34F + 7},{_tmp350,_tmp350,_tmp350 + 4},{_tmp351,_tmp351,_tmp351 + 9},{_tmp352,_tmp352,_tmp352 + 9},{_tmp353,_tmp353,_tmp353 + 6},{_tmp354,_tmp354,_tmp354 + 10},{_tmp355,_tmp355,_tmp355 + 9},{_tmp356,_tmp356,_tmp356 + 7},{_tmp357,_tmp357,_tmp357 + 8},{_tmp358,_tmp358,_tmp358 + 7},{_tmp359,_tmp359,_tmp359 + 8},{_tmp35A,_tmp35A,_tmp35A + 5},{_tmp35B,_tmp35B,_tmp35B + 9},{_tmp35C,_tmp35C,_tmp35C + 6},{_tmp35D,_tmp35D,_tmp35D + 7},{_tmp35E,_tmp35E,_tmp35E + 5},{_tmp35F,_tmp35F,_tmp35F + 8},{_tmp360,_tmp360,_tmp360 + 13},{_tmp361,_tmp361,_tmp361 + 7},{_tmp362,_tmp362,_tmp362 + 8},{_tmp363,_tmp363,_tmp363 + 12},{_tmp364,_tmp364,_tmp364 + 8},{_tmp365,_tmp365,_tmp365 + 8},{_tmp366,_tmp366,_tmp366 + 10},{_tmp367,_tmp367,_tmp367 + 9},{_tmp368,_tmp368,_tmp368 + 13},{_tmp369,_tmp369,_tmp369 + 10},{_tmp36A,_tmp36A,_tmp36A + 9},{_tmp36B,_tmp36B,_tmp36B + 13},{_tmp36C,_tmp36C,_tmp36C + 14},{_tmp36D,_tmp36D,_tmp36D + 14},{_tmp36E,_tmp36E,_tmp36E + 13},{_tmp36F,_tmp36F,_tmp36F + 12},{_tmp370,_tmp370,_tmp370 + 16},{_tmp371,_tmp371,_tmp371 + 14},{_tmp372,_tmp372,_tmp372 + 12},{_tmp373,_tmp373,_tmp373 + 16},{_tmp374,_tmp374,_tmp374 + 15},{_tmp375,_tmp375,_tmp375 + 7},{_tmp376,_tmp376,_tmp376 + 7},{_tmp377,_tmp377,_tmp377 + 7},{_tmp378,_tmp378,_tmp378 + 8},{_tmp379,_tmp379,_tmp379 + 9},{_tmp37A,_tmp37A,_tmp37A + 6},{_tmp37B,_tmp37B,_tmp37B + 6},{_tmp37C,_tmp37C,_tmp37C + 6},{_tmp37D,_tmp37D,_tmp37D + 6},{_tmp37E,_tmp37E,_tmp37E + 7},{_tmp37F,_tmp37F,_tmp37F + 6},{_tmp380,_tmp380,_tmp380 + 11},{_tmp381,_tmp381,_tmp381 + 11},{_tmp382,_tmp382,_tmp382 + 11},{_tmp383,_tmp383,_tmp383 + 11},{_tmp384,_tmp384,_tmp384 + 11},{_tmp385,_tmp385,_tmp385 + 12},{_tmp386,_tmp386,_tmp386 + 13},{_tmp387,_tmp387,_tmp387 + 11},{_tmp388,_tmp388,_tmp388 + 11},{_tmp389,_tmp389,_tmp389 + 10},{_tmp38A,_tmp38A,_tmp38A + 9},{_tmp38B,_tmp38B,_tmp38B + 11},{_tmp38C,_tmp38C,_tmp38C + 12},{_tmp38D,_tmp38D,_tmp38D + 11},{_tmp38E,_tmp38E,_tmp38E + 17},{_tmp38F,_tmp38F,_tmp38F + 7},{_tmp390,_tmp390,_tmp390 + 8},{_tmp391,_tmp391,_tmp391 + 19},{_tmp392,_tmp392,_tmp392 + 20},{_tmp393,_tmp393,_tmp393 + 18},{_tmp394,_tmp394,_tmp394 + 9},{_tmp395,_tmp395,_tmp395 + 13},{_tmp396,_tmp396,_tmp396 + 16},{_tmp397,_tmp397,_tmp397 + 18},{_tmp398,_tmp398,_tmp398 + 10},{_tmp399,_tmp399,_tmp399 + 4},{_tmp39A,_tmp39A,_tmp39A + 4},{_tmp39B,_tmp39B,_tmp39B + 4},{_tmp39C,_tmp39C,_tmp39C + 4},{_tmp39D,_tmp39D,_tmp39D + 4},{_tmp39E,_tmp39E,_tmp39E + 4},{_tmp39F,_tmp39F,_tmp39F + 4},{_tmp3A0,_tmp3A0,_tmp3A0 + 4},{_tmp3A1,_tmp3A1,_tmp3A1 + 4},{_tmp3A2,_tmp3A2,_tmp3A2 + 4},{_tmp3A3,_tmp3A3,_tmp3A3 + 4},{_tmp3A4,_tmp3A4,_tmp3A4 + 4},{_tmp3A5,_tmp3A5,_tmp3A5 + 4},{_tmp3A6,_tmp3A6,_tmp3A6 + 4},{_tmp3A7,_tmp3A7,_tmp3A7 + 4},{_tmp3A8,_tmp3A8,_tmp3A8 + 4},{_tmp3A9,_tmp3A9,_tmp3A9 + 4},{_tmp3AA,_tmp3AA,_tmp3AA + 4},{_tmp3AB,_tmp3AB,_tmp3AB + 4},{_tmp3AC,_tmp3AC,_tmp3AC + 4},{_tmp3AD,_tmp3AD,_tmp3AD + 4},{_tmp3AE,_tmp3AE,_tmp3AE + 4},{_tmp3AF,_tmp3AF,_tmp3AF + 4},{_tmp3B0,_tmp3B0,_tmp3B0 + 4},{_tmp3B1,_tmp3B1,_tmp3B1 + 4},{_tmp3B2,_tmp3B2,_tmp3B2 + 4},{_tmp3B3,_tmp3B3,_tmp3B3 + 4},{_tmp3B4,_tmp3B4,_tmp3B4 + 4},{_tmp3B5,_tmp3B5,_tmp3B5 + 5},{_tmp3B6,_tmp3B6,_tmp3B6 + 17},{_tmp3B7,_tmp3B7,_tmp3B7 + 12},{_tmp3B8,_tmp3B8,_tmp3B8 + 19},{_tmp3B9,_tmp3B9,_tmp3B9 + 21},{_tmp3BA,_tmp3BA,_tmp3BA + 15},{_tmp3BB,_tmp3BB,_tmp3BB + 20},{_tmp3BC,_tmp3BC,_tmp3BC + 21},{_tmp3BD,_tmp3BD,_tmp3BD + 13},{_tmp3BE,_tmp3BE,_tmp3BE + 15},{_tmp3BF,_tmp3BF,_tmp3BF + 17},{_tmp3C0,_tmp3C0,_tmp3C0 + 19},{_tmp3C1,_tmp3C1,_tmp3C1 + 12},{_tmp3C2,_tmp3C2,_tmp3C2 + 19},{_tmp3C3,_tmp3C3,_tmp3C3 + 17},{_tmp3C4,_tmp3C4,_tmp3C4 + 23},{_tmp3C5,_tmp3C5,_tmp3C5 + 24},{_tmp3C6,_tmp3C6,_tmp3C6 + 15},{_tmp3C7,_tmp3C7,_tmp3C7 + 11},{_tmp3C8,_tmp3C8,_tmp3C8 + 15},{_tmp3C9,_tmp3C9,_tmp3C9 + 10},{_tmp3CA,_tmp3CA,_tmp3CA + 15},{_tmp3CB,_tmp3CB,_tmp3CB + 25},{_tmp3CC,_tmp3CC,_tmp3CC + 5},{_tmp3CD,_tmp3CD,_tmp3CD + 15},{_tmp3CE,_tmp3CE,_tmp3CE + 15},{_tmp3CF,_tmp3CF,_tmp3CF + 11},{_tmp3D0,_tmp3D0,_tmp3D0 + 22},{_tmp3D1,_tmp3D1,_tmp3D1 + 26},{_tmp3D2,_tmp3D2,_tmp3D2 + 16},{_tmp3D3,_tmp3D3,_tmp3D3 + 16},{_tmp3D4,_tmp3D4,_tmp3D4 + 24},{_tmp3D5,_tmp3D5,_tmp3D5 + 25},{_tmp3D6,_tmp3D6,_tmp3D6 + 21},{_tmp3D7,_tmp3D7,_tmp3D7 + 22},{_tmp3D8,_tmp3D8,_tmp3D8 + 16},{_tmp3D9,_tmp3D9,_tmp3D9 + 19},{_tmp3DA,_tmp3DA,_tmp3DA + 25},{_tmp3DB,_tmp3DB,_tmp3DB + 35},{_tmp3DC,_tmp3DC,_tmp3DC + 23},{_tmp3DD,_tmp3DD,_tmp3DD + 24},{_tmp3DE,_tmp3DE,_tmp3DE + 18},{_tmp3DF,_tmp3DF,_tmp3DF + 20},{_tmp3E0,_tmp3E0,_tmp3E0 + 19},{_tmp3E1,_tmp3E1,_tmp3E1 + 19},{_tmp3E2,_tmp3E2,_tmp3E2 + 14},{_tmp3E3,_tmp3E3,_tmp3E3 + 19},{_tmp3E4,_tmp3E4,_tmp3E4 + 20},{_tmp3E5,_tmp3E5,_tmp3E5 + 14},{_tmp3E6,_tmp3E6,_tmp3E6 + 11},{_tmp3E7,_tmp3E7,_tmp3E7 + 23},{_tmp3E8,_tmp3E8,_tmp3E8 + 18},{_tmp3E9,_tmp3E9,_tmp3E9 + 30},{_tmp3EA,_tmp3EA,_tmp3EA + 8},{_tmp3EB,_tmp3EB,_tmp3EB + 12},{_tmp3EC,_tmp3EC,_tmp3EC + 14},{_tmp3ED,_tmp3ED,_tmp3ED + 13},{_tmp3EE,_tmp3EE,_tmp3EE + 23},{_tmp3EF,_tmp3EF,_tmp3EF + 14},{_tmp3F0,_tmp3F0,_tmp3F0 + 18},{_tmp3F1,_tmp3F1,_tmp3F1 + 8},{_tmp3F2,_tmp3F2,_tmp3F2 + 11},{_tmp3F3,_tmp3F3,_tmp3F3 + 20},{_tmp3F4,_tmp3F4,_tmp3F4 + 9},{_tmp3F5,_tmp3F5,_tmp3F5 + 16},{_tmp3F6,_tmp3F6,_tmp3F6 + 19},{_tmp3F7,_tmp3F7,_tmp3F7 + 10},{_tmp3F8,_tmp3F8,_tmp3F8 + 16},{_tmp3F9,_tmp3F9,_tmp3F9 + 11},{_tmp3FA,_tmp3FA,_tmp3FA + 14},{_tmp3FB,_tmp3FB,_tmp3FB + 11},{_tmp3FC,_tmp3FC,_tmp3FC + 15},{_tmp3FD,_tmp3FD,_tmp3FD + 22},{_tmp3FE,_tmp3FE,_tmp3FE + 16},{_tmp3FF,_tmp3FF,_tmp3FF + 17},{_tmp400,_tmp400,_tmp400 + 12},{_tmp401,_tmp401,_tmp401 + 18},{_tmp402,_tmp402,_tmp402 + 17},{_tmp403,_tmp403,_tmp403 + 12},{_tmp404,_tmp404,_tmp404 + 16},{_tmp405,_tmp405,_tmp405 + 11},{_tmp406,_tmp406,_tmp406 + 10},{_tmp407,_tmp407,_tmp407 + 14},{_tmp408,_tmp408,_tmp408 + 15},{_tmp409,_tmp409,_tmp409 + 20},{_tmp40A,_tmp40A,_tmp40A + 27},{_tmp40B,_tmp40B,_tmp40B + 10},{_tmp40C,_tmp40C,_tmp40C + 18},{_tmp40D,_tmp40D,_tmp40D + 21},{_tmp40E,_tmp40E,_tmp40E + 19},{_tmp40F,_tmp40F,_tmp40F + 16},{_tmp410,_tmp410,_tmp410 + 20},{_tmp411,_tmp411,_tmp411 + 15},{_tmp412,_tmp412,_tmp412 + 20},{_tmp413,_tmp413,_tmp413 + 15},{_tmp414,_tmp414,_tmp414 + 12},{_tmp415,_tmp415,_tmp415 + 20},{_tmp416,_tmp416,_tmp416 + 19},{_tmp417,_tmp417,_tmp417 + 20},{_tmp418,_tmp418,_tmp418 + 21},{_tmp419,_tmp419,_tmp419 + 21},{_tmp41A,_tmp41A,_tmp41A + 12},{_tmp41B,_tmp41B,_tmp41B + 17},{_tmp41C,_tmp41C,_tmp41C + 19},{_tmp41D,_tmp41D,_tmp41D + 14},{_tmp41E,_tmp41E,_tmp41E + 17},{_tmp41F,_tmp41F,_tmp41F + 23},{_tmp420,_tmp420,_tmp420 + 13},{_tmp421,_tmp421,_tmp421 + 14},{_tmp422,_tmp422,_tmp422 + 16},{_tmp423,_tmp423,_tmp423 + 16},{_tmp424,_tmp424,_tmp424 + 8},{_tmp425,_tmp425,_tmp425 + 19},{_tmp426,_tmp426,_tmp426 + 20},{_tmp427,_tmp427,_tmp427 + 14},{_tmp428,_tmp428,_tmp428 + 19},{_tmp429,_tmp429,_tmp429 + 20},{_tmp42A,_tmp42A,_tmp42A + 11},{_tmp42B,_tmp42B,_tmp42B + 22},{_tmp42C,_tmp42C,_tmp42C + 20},{_tmp42D,_tmp42D,_tmp42D + 23},{_tmp42E,_tmp42E,_tmp42E + 20},{_tmp42F,_tmp42F,_tmp42F + 22},{_tmp430,_tmp430,_tmp430 + 23},{_tmp431,_tmp431,_tmp431 + 24},{_tmp432,_tmp432,_tmp432 + 24},{_tmp433,_tmp433,_tmp433 + 15},{_tmp434,_tmp434,_tmp434 + 20},{_tmp435,_tmp435,_tmp435 + 22},{_tmp436,_tmp436,_tmp436 + 17},{_tmp437,_tmp437,_tmp437 + 20},{_tmp438,_tmp438,_tmp438 + 26},{_tmp439,_tmp439,_tmp439 + 16},{_tmp43A,_tmp43A,_tmp43A + 17},{_tmp43B,_tmp43B,_tmp43B + 15},{_tmp43C,_tmp43C,_tmp43C + 19},{_tmp43D,_tmp43D,_tmp43D + 17},{_tmp43E,_tmp43E,_tmp43E + 19},{_tmp43F,_tmp43F,_tmp43F + 25},{_tmp440,_tmp440,_tmp440 + 26},{_tmp441,_tmp441,_tmp441 + 9},{_tmp442,_tmp442,_tmp442 + 20},{_tmp443,_tmp443,_tmp443 + 17},{_tmp444,_tmp444,_tmp444 + 18},{_tmp445,_tmp445,_tmp445 + 11},{_tmp446,_tmp446,_tmp446 + 12}};
# 1624
static short Cyc_yyr1[525]={0,145,146,146,146,146,146,146,146,146,146,146,147,147,148,148,148,149,149,149,150,150,151,151,151,151,152,152,153,154,155,156,157,157,157,157,157,157,157,158,158,159,159,160,160,160,160,160,160,160,160,160,160,161,161,161,161,161,161,161,162,162,163,164,164,165,165,165,165,165,165,166,166,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,168,169,169,169,170,170,170,171,171,172,172,172,173,173,173,173,173,174,174,175,175,176,176,177,177,178,179,179,180,180,181,182,182,182,182,182,182,183,183,183,183,183,183,184,185,185,186,186,186,186,187,187,188,188,189,189,189,190,190,191,191,191,191,192,192,192,193,193,194,194,195,195,196,196,196,196,196,196,196,196,196,196,197,197,197,197,197,197,197,197,197,197,197,198,198,199,200,200,201,201,201,201,201,201,201,201,202,202,202,203,203,204,204,204,205,205,205,206,206,207,207,207,207,208,208,209,209,210,210,211,211,212,212,213,213,214,214,214,214,215,215,216,216,217,217,217,218,219,219,220,220,221,221,221,221,221,222,222,222,222,223,224,224,225,225,226,226,227,227,227,227,227,228,228,229,229,229,230,230,230,230,230,230,230,230,230,230,230,231,231,231,231,231,231,231,232,233,233,234,234,235,235,235,235,235,235,235,235,236,236,236,236,236,236,237,237,237,237,237,237,238,238,238,238,238,238,238,238,238,238,238,238,238,238,239,239,239,239,239,239,239,239,240,241,241,242,242,243,243,244,244,245,245,246,246,247,247,247,248,248,248,248,248,249,249,249,250,250,250,251,251,251,251,252,252,253,253,253,253,253,253,254,255,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,257,257,257,258,258,259,259,260,260,260,261,261,262,262,263,263,263,264,264,264,264,264,264,264,264,264,264,264,265,265,265,265,265,265,265,266,267,267,268,268,269,269,270,270,271,271,272,272,272,273,273,273,273,273,274,274,274,275,275,275,276,276,276,276,277,277,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,279,279,279,280,280,280,280,280,280,280,280,280,280,281,281,281,281,282,282,282,282,282,282,282,282,282,282,283,284,284,285,285,285,285,285,286,286,287,287,288,288,289,289,290,290};
# 1680
static short Cyc_yyr2[525]={0,1,2,3,5,3,5,6,7,3,3,0,4,3,1,2,3,1,1,1,1,0,3,4,4,5,3,4,2,1,2,1,2,3,5,3,7,4,9,0,1,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,4,4,8,1,2,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,4,4,1,4,6,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,2,2,0,3,4,0,4,0,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,6,7,4,4,2,1,1,3,4,4,5,6,7,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,6,5,7,6,4,2,1,1,1,1,1,1,5,3,1,2,2,3,1,2,3,4,1,2,1,2,5,7,7,5,8,6,0,3,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,1,1,1,3,1,1,3,6,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,4,6,9,11,4,6,6,4,1,1,1,1,1,4,3,4,3,3,2,2,6,7,1,1,3,3,1,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1736
static short Cyc_yydefact[1061]={0,19,53,54,55,56,58,73,76,77,78,79,80,81,82,83,97,98,99,115,116,49,0,0,59,0,0,153,90,94,0,0,0,0,0,0,40,515,219,517,516,518,0,0,74,0,205,205,204,1,0,17,0,0,18,0,0,43,51,45,71,47,84,85,0,86,0,39,164,0,189,192,87,168,113,57,56,50,0,101,514,0,515,510,511,512,513,113,0,379,0,0,0,0,242,0,381,382,28,30,0,0,0,0,0,0,0,154,0,0,0,0,0,0,0,202,203,2,0,0,0,0,0,32,0,121,122,124,44,52,46,48,117,519,520,113,113,39,41,39,0,21,0,221,0,177,165,190,0,196,197,200,201,0,199,198,210,192,0,72,0,57,105,0,103,0,515,390,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,499,500,479,0,0,0,0,0,482,480,481,0,407,409,423,431,433,435,437,439,441,444,449,452,455,459,0,461,483,498,497,0,391,389,35,0,0,113,0,0,0,131,127,129,261,263,0,0,9,10,0,0,113,521,522,220,96,0,0,169,75,240,0,237,0,3,0,5,0,0,0,33,0,0,39,21,0,118,119,144,112,0,151,0,0,0,0,0,0,0,0,0,0,0,0,515,292,294,0,302,296,0,300,284,285,286,0,287,288,289,0,42,21,124,20,22,268,0,227,243,0,0,223,221,0,207,0,0,0,212,60,211,193,0,0,106,102,0,0,383,0,0,404,0,0,0,0,257,400,405,0,402,0,469,0,425,459,0,426,427,0,0,0,0,0,0,0,0,0,462,463,39,0,0,0,465,466,464,0,380,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,413,414,415,416,417,418,419,420,421,422,412,0,467,0,489,490,0,0,0,502,0,113,397,398,0,395,0,244,0,0,0,0,264,0,235,132,137,133,135,128,130,221,0,270,262,271,524,523,0,89,93,0,91,95,111,66,65,0,63,170,221,239,166,270,241,178,179,0,88,206,29,0,31,0,37,0,0,123,125,246,245,21,23,108,120,0,0,0,139,140,147,0,113,113,159,0,0,0,0,0,515,0,39,0,331,332,333,0,0,335,0,0,39,0,303,297,124,301,295,293,24,0,176,228,0,0,0,234,222,229,147,0,0,0,223,175,209,208,171,207,0,0,213,61,114,0,107,430,104,100,393,394,260,0,401,256,258,388,0,0,0,0,247,252,0,0,0,0,0,0,0,0,0,0,0,0,0,501,508,0,507,408,432,0,434,436,438,440,442,443,447,448,445,446,450,451,453,454,456,457,458,411,410,488,485,0,487,0,0,0,385,0,392,34,0,386,0,265,0,138,134,136,0,223,0,207,0,272,0,221,0,283,267,0,113,0,0,0,131,0,113,0,221,0,188,167,238,4,6,0,0,25,0,143,126,144,0,0,142,223,152,161,160,0,0,155,0,0,0,310,0,0,0,0,0,0,330,334,0,0,0,0,298,291,39,26,269,221,0,231,0,0,149,224,0,147,227,215,172,194,195,213,191,0,7,0,0,259,403,406,468,0,0,248,0,253,471,0,0,0,0,475,478,0,0,0,0,0,460,504,0,0,486,484,0,0,396,399,387,266,236,147,0,273,274,207,0,0,223,207,0,92,223,0,0,0,62,64,0,180,0,0,223,0,207,0,0,0,141,0,145,117,150,162,159,159,0,0,0,0,0,0,0,0,0,0,0,0,0,310,336,0,0,299,27,223,0,232,230,0,173,0,149,223,0,214,0,8,384,494,0,493,0,249,254,0,0,0,0,428,429,488,487,506,0,509,424,503,505,0,149,147,276,282,147,0,275,207,117,0,69,67,68,181,187,147,0,186,182,207,0,36,0,0,0,158,157,304,310,0,0,0,0,0,0,338,339,341,343,345,347,349,351,354,359,362,365,369,371,377,378,0,39,307,316,0,0,0,0,0,0,0,0,0,0,337,290,217,233,0,225,174,216,221,13,0,14,470,0,0,255,472,0,0,477,476,491,0,279,149,149,147,277,0,0,149,147,183,0,146,110,0,0,0,310,0,375,0,0,372,39,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,373,311,0,318,0,0,0,326,0,0,0,0,0,0,309,0,0,223,12,15,0,496,495,0,0,0,492,278,281,149,109,0,184,149,38,163,305,306,0,0,0,0,310,312,342,0,344,346,348,350,352,353,357,358,355,356,360,361,363,364,366,367,368,0,317,319,320,0,328,327,0,322,0,0,0,148,226,218,16,0,0,0,0,280,0,185,308,374,0,370,313,0,39,321,329,323,324,0,251,250,473,0,70,0,340,310,314,325,0,376,315,474,0,0,0};
# 1845
static short Cyc_yydefgoto[146]={1058,49,690,890,50,300,51,284,52,456,53,458,54,55,139,56,57,527,226,442,443,227,60,240,228,62,162,163,63,159,64,261,262,124,125,126,263,229,424,472,473,474,643,785,65,66,648,649,650,67,475,68,447,69,70,156,157,71,115,523,315,687,607,72,608,517,678,509,513,514,418,308,248,93,94,548,464,549,330,331,332,230,301,302,609,430,287,288,289,290,291,292,766,293,294,849,850,851,852,853,854,855,856,857,858,859,860,861,862,863,864,333,409,410,334,335,336,295,197,396,198,533,199,200,201,202,203,204,205,206,207,208,209,210,211,212,795,213,564,565,214,215,74,891,241,434};
# 1863
static short Cyc_yypact[1061]={2874,- -32768,- -32768,- -32768,- -32768,36,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3451,265,3119,- -32768,84,9,- -32768,- 53,53,98,124,131,154,394,269,- -32768,- -32768,229,- -32768,- -32768,- -32768,211,727,260,262,319,319,- -32768,- -32768,2738,- -32768,695,704,- -32768,387,1025,3451,3451,3451,- -32768,3451,- -32768,- -32768,523,- -32768,84,3371,251,- 13,606,1077,- -32768,- -32768,323,347,427,- -32768,84,397,- -32768,4063,208,- -32768,- -32768,- -32768,- -32768,323,6232,- -32768,444,455,4063,453,460,467,- -32768,153,- -32768,- -32768,3599,3599,2738,2738,3599,6232,195,- -32768,215,489,744,482,215,3969,6232,- -32768,- -32768,- -32768,2738,3009,2738,3009,79,- -32768,500,501,- -32768,3290,- -32768,- -32768,- -32768,- -32768,3969,- -32768,- -32768,323,91,1615,- -32768,3371,1025,506,3599,3525,4583,- -32768,251,- -32768,508,- -32768,- -32768,- -32768,- -32768,515,- -32768,- -32768,1,1077,3599,- -32768,3009,- -32768,528,524,538,84,90,- -32768,4063,35,925,6376,551,6232,6280,553,557,573,582,592,596,607,609,6424,6424,- -32768,- -32768,- -32768,2320,621,6472,6472,6472,- -32768,- -32768,- -32768,- 48,- -32768,- -32768,- 40,602,591,620,611,748,81,791,116,140,- -32768,987,6472,136,7,- -32768,647,1275,665,- -32768,- -32768,698,6232,323,1275,691,3747,3969,4043,3969,550,- -32768,21,21,- -32768,- -32768,51,181,323,- -32768,- -32768,- -32768,- -32768,43,685,- -32768,- -32768,841,329,- -32768,706,- -32768,708,- -32768,720,235,736,- -32768,744,4680,3371,506,730,3969,- -32768,868,738,84,747,745,264,756,4098,764,750,755,778,4777,4098,151,776,782,- -32768,- -32768,793,1757,1757,1025,1757,- -32768,- -32768,- -32768,796,- -32768,- -32768,- -32768,378,- -32768,506,807,- -32768,- -32768,811,73,830,- -32768,10,823,822,436,827,816,835,6232,3599,- -32768,846,- -32768,- -32768,73,848,84,- -32768,6232,858,- -32768,860,861,- -32768,215,6232,4063,354,- -32768,- -32768,- -32768,874,867,2320,- -32768,3969,- -32768,- -32768,4195,- -32768,905,6232,6232,6232,6232,6232,6232,3969,777,2320,- -32768,- -32768,1899,872,336,6232,- -32768,- -32768,- -32768,6232,- -32768,6472,6232,6472,6472,6472,6472,6472,6472,6472,6472,6472,6472,6472,6472,6472,6472,6472,6472,6472,6232,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,6232,- -32768,215,- -32768,- -32768,4874,215,6232,- -32768,882,323,- -32768,- -32768,873,883,4063,- -32768,568,925,884,3599,- -32768,887,890,- -32768,4043,4043,4043,- -32768,- -32768,1161,4971,272,- -32768,421,- -32768,- -32768,10,- -32768,- -32768,3599,- -32768,- -32768,893,- -32768,898,902,908,- -32768,3083,- -32768,548,95,- -32768,- -32768,- -32768,3969,- -32768,- -32768,- -32768,2738,- -32768,2738,- -32768,927,909,- -32768,- -32768,- -32768,- -32768,506,- -32768,- -32768,- -32768,1078,6232,919,921,- -32768,- 33,110,323,323,904,6232,6232,915,928,6232,917,1029,2183,942,- -32768,- -32768,- -32768,587,1019,- -32768,5068,6232,2041,2457,- -32768,- -32768,3290,- -32768,- -32768,- -32768,- -32768,3599,- -32768,- -32768,3969,937,3821,- -32768,- -32768,930,994,10,943,3895,822,- -32768,- -32768,- -32768,- -32768,816,945,37,802,- -32768,- -32768,2602,- -32768,- -32768,- -32768,- -32768,3969,- -32768,- -32768,941,- -32768,- -32768,- -32768,- -32768,1195,948,954,971,- -32768,- -32768,792,4680,951,957,968,970,356,967,969,213,972,974,6328,- -32768,- -32768,975,976,- -32768,602,219,591,620,611,748,81,81,791,791,791,791,116,116,140,140,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,977,- -32768,17,3599,4486,- -32768,1317,- -32768,- -32768,984,- -32768,70,- -32768,3969,- -32768,- -32768,- -32768,985,822,986,816,964,421,3599,3673,5165,- -32768,- -32768,21,323,728,989,43,3157,990,323,3599,3525,5262,- -32768,548,- -32768,- -32768,- -32768,988,1013,- -32768,638,- -32768,- -32768,868,995,6232,- -32768,822,- -32768,- -32768,- -32768,1001,84,612,357,368,6232,892,374,4098,997,5359,5456,675,- -32768,- -32768,1005,1008,1002,384,1757,- -32768,3371,- -32768,811,1009,3599,- -32768,1012,10,1055,- -32768,1011,994,175,- -32768,- -32768,- -32768,- -32768,802,- -32768,1017,- -32768,2738,1037,- -32768,- -32768,- -32768,1032,192,1034,- -32768,4292,- -32768,- -32768,6232,1137,6232,6280,- -32768,- -32768,215,215,1032,1038,4486,- -32768,- -32768,6232,6232,- -32768,- -32768,73,800,- -32768,- -32768,- -32768,- -32768,- -32768,994,1039,- -32768,- -32768,816,73,1040,822,816,1035,- -32768,822,440,1042,1058,- -32768,- -32768,1059,- -32768,73,1060,822,1063,816,1065,6232,1072,- -32768,6232,- -32768,3969,- -32768,1070,41,904,4098,1081,1079,881,1075,1086,4098,6232,5553,676,5650,677,5747,892,- -32768,1089,1090,- -32768,- -32768,822,86,- -32768,- -32768,1084,- -32768,1102,1055,822,3969,- -32768,232,- -32768,- -32768,- -32768,224,- -32768,6232,- -32768,- -32768,4680,1092,1094,1099,- -32768,905,1095,1097,- -32768,806,- -32768,- -32768,- -32768,- -32768,4486,1055,994,- -32768,- -32768,994,1103,- -32768,816,3969,1109,- -32768,- -32768,- -32768,- -32768,- -32768,994,1104,- -32768,- -32768,816,483,- -32768,1105,1108,3969,- -32768,- -32768,1197,892,1113,6520,1110,2457,6472,1107,- -32768,- 37,- -32768,1144,1100,799,870,147,876,258,521,- -32768,- -32768,- -32768,- -32768,1151,6472,1899,- -32768,- -32768,485,4098,525,5844,4098,567,5941,6038,686,1122,- -32768,- -32768,- -32768,- -32768,6232,1123,- -32768,- -32768,1009,- -32768,1125,687,- -32768,435,279,- -32768,- -32768,3969,1218,- -32768,- -32768,- -32768,4389,- -32768,1055,1055,994,- -32768,1126,1127,1055,994,- -32768,1129,- -32768,- -32768,583,4098,1130,892,2320,- -32768,3969,1133,- -32768,1473,6472,6232,6472,6472,6472,6472,6472,6472,6472,6472,6472,6472,6472,6472,6472,6472,6472,6472,6472,6232,- -32768,- -32768,1131,- -32768,4098,4098,585,- -32768,4098,4098,593,4098,599,6135,- -32768,1134,10,822,- -32768,- -32768,195,- -32768,- -32768,2457,1135,1128,- -32768,- -32768,- -32768,1055,- -32768,1145,- -32768,1055,- -32768,- -32768,- -32768,- -32768,1132,1136,1143,6472,892,- -32768,602,283,591,620,620,748,81,81,791,791,791,791,116,116,140,140,- -32768,- -32768,- -32768,313,- -32768,- -32768,- -32768,4098,- -32768,- -32768,4098,- -32768,4098,4098,603,- -32768,- -32768,- -32768,- -32768,1146,844,1141,3969,- -32768,1142,- -32768,- -32768,1032,192,- -32768,- -32768,6232,1473,- -32768,- -32768,- -32768,- -32768,4098,- -32768,- -32768,- -32768,1152,- -32768,306,- -32768,892,- -32768,- -32768,1154,- -32768,- -32768,- -32768,1256,1269,- -32768};
# 1972
static short Cyc_yypgoto[146]={- -32768,128,- -32768,305,- -32768,- 216,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 22,- -32768,- 102,27,- -32768,- -32768,0,648,- -32768,32,- 178,1160,5,- -32768,- -32768,- 134,- -32768,271,1247,- 714,- -32768,- -32768,- -32768,1026,1020,399,442,- -32768,- -32768,644,- 209,- 51,- -32768,- -32768,220,- -32768,- -32768,560,- 242,1221,- 396,187,- -32768,1150,- -32768,- -32768,1240,- 99,- -32768,605,- 141,- 150,- 123,- 21,337,617,626,- 419,- 470,- 112,- 378,- 104,- -32768,- 247,- 158,- 556,- 259,- -32768,980,- 160,- 77,- 54,22,- 372,183,- -32768,- -32768,- 52,- 277,- -32768,- 412,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,107,1080,- -32768,765,901,- -32768,172,760,- -32768,- 139,- 218,- 155,- 362,- 358,- 335,947,- 340,- 359,- 135,- 360,- 343,- 10,409,554,960,285,- -32768,- 379,- -32768,304,299,- 35,- 29,- 228,23};
# 1994
static short Cyc_yytable[6665]={58,247,306,567,446,61,316,499,500,569,502,463,573,574,617,141,343,579,580,344,307,58,590,231,232,259,61,235,357,134,572,323,59,570,340,135,581,582,720,309,641,675,838,110,467,138,646,647,77,422,58,365,630,59,926,61,612,58,58,58,440,58,61,61,61,419,61,58,145,510,100,134,61,363,631,260,612,237,364,560,630,505,59,550,128,129,130,297,131,59,59,59,37,59,140,366,642,679,927,59,537,40,58,58,318,138,431,61,61,908,404,538,43,38,99,285,665,296,58,58,58,58,38,61,61,61,61,58,314,511,95,672,61,725,59,59,431,58,363,58,631,405,61,75,61,432,145,325,441,718,59,59,59,59,140,512,809,433,431,59,58,- 156,326,685,286,61,140,373,374,59,588,59,436,433,591,432,101,544,117,545,360,361,362,532,254,519,531,433,167,37,532,557,59,559,506,168,724,432,40,218,37,397,255,624,375,376,433,466,39,40,41,451,883,169,158,37,102,398,399,400,445,266,433,39,40,41,427,421,624,451,233,234,423,934,935,470,526,296,575,576,577,578,103,422,422,422,250,251,252,253,636,429,379,638,104,435,380,147,437,58,196,401,285,285,61,285,402,403,449,494,936,937,222,381,763,324,495,236,223,105,508,382,383,512,58,58,140,58,319,61,61,59,61,305,708,399,400,238,794,73,37,- 221,363,700,- 221,239,677,438,39,40,41,286,286,168,286,528,59,59,107,59,238,79,97,408,98,507,- 243,96,239,- 243,408,169,532,108,285,550,109,37,401,601,363,529,73,709,403,39,40,41,716,73,892,419,889,459,893,73,58,460,170,618,358,61,879,133,112,136,512,42,73,37,37,583,584,585,142,691,143,164,40,40,97,743,144,286,78,96,247,113,59,481,778,97,482,413,940,743,96,426,941,669,363,73,73,427,363,133,265,267,969,73,625,888,1037,451,19,20,428,73,73,73,73,421,421,421,755,683,423,423,423,671,616,918,1055,448,363,451,893,539,114,73,800,451,1038,419,122,623,158,629,492,670,452,476,799,550,486,453,58,363,58,73,493,61,562,61,164,659,160,97,796,97,637,787,96,732,96,667,540,363,363,138,806,807,705,761,328,329,746,58,59,363,59,733,61,414,762,363,504,58,682,363,767,58,61,532,747,363,61,984,439,358,777,729,246,989,140,97,165,815,597,59,96,555,97,748,140,358,512,96,140,59,58,264,686,59,161,61,837,42,568,719,238,967,246,679,613,450,614,804,239,780,805,713,615,895,305,800,73,518,788,731,217,824,59,450,990,478,825,799,483,216,992,219,745,996,997,592,1036,811,1002,1003,338,220,341,341,632,73,633,727,625,221,947,995,354,355,993,994,781,1004,1005,341,341,341,451,111,363,451,363,904,244,913,905,948,616,428,242,532,127,779,256,164,341,910,257,756,299,420,1052,425,37,97,629,817,448,312,96,821,39,40,41,1056,313,736,132,800,321,285,363,296,988,320,833,950,651,652,942,799,655,476,689,660,322,264,943,944,42,968,961,58,666,58,243,626,61,627,61,426,339,594,345,628,668,427,346,46,47,48,598,923,363,363,58,686,367,954,286,61,140,975,347,59,298,59,979,722,452,662,363,348,363,981,97,1013,820,97,363,96,823,349,96,1016,363,350,59,907,363,1018,831,916,368,1043,759,341,351,760,352,912,886,970,341,46,47,48,812,37,450,532,359,450,644,645,370,39,40,41,818,73,134,73,369,882,985,1051,986,470,903,963,406,887,829,450,411,46,47,48,341,450,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,773,873,876,363,363,363,998,999,1000,1001,412,959,965,796,363,966,1025,476,444,512,118,119,80,416,298,792,16,17,18,120,121,476,246,454,476,455,73,770,772,37,738,739,740,371,372,924,668,457,40,97,42,246,285,501,96,461,37,468,246,43,973,974,488,946,477,40,978,46,47,48,604,605,606,479,58,1047,43,480,865,61,489,249,377,378,46,47,48,341,484,37,83,185,186,84,85,86,487,737,40,521,522,286,97,490,246,1050,59,96,496,353,285,311,189,845,846,246,646,647,497,698,699,764,765,503,246,498,80,813,814,450,835,1029,58,901,902,1031,258,61,134,1006,1007,1008,508,450,930,931,450,870,872,1023,875,842,878,37,433,758,515,869,516,286,520,39,40,41,59,932,933,938,939,42,80,1045,363,445,524,530,894,341,81,427,37,46,47,48,1035,534,840,841,39,40,41,535,536,166,83,543,73,84,85,86,470,542,40,365,471,561,595,87,46,47,48,596,593,847,602,89,90,600,603,619,246,91,81,285,848,465,196,92,193,420,620,194,195,246,327,621,622,166,83,634,635,84,85,86,639,58,40,653,384,640,61,87,952,656,654,956,958,88,341,89,90,949,328,329,953,91,657,661,663,673,97,92,286,676,641,96,680,59,684,525,692,695,696,697,701,702,385,386,387,388,389,390,391,392,393,394,703,133,704,358,706,711,707,730,715,710,991,982,714,465,717,723,551,552,553,554,395,556,726,728,751,341,741,744,1009,752,563,754,757,768,566,774,341,775,305,776,784,37,1020,782,1011,1012,786,791,1014,1015,40,1017,1026,793,123,586,148,149,150,151,152,43,712,153,154,155,264,587,797,46,47,48,563,802,341,808,816,819,822,826,7,8,9,10,11,12,13,14,15,16,17,18,19,20,37,827,828,830,611,246,832,836,39,40,41,42,839,1039,834,22,1040,843,1041,1042,470,867,844,868,880,881,884,27,46,47,48,885,909,28,29,896,897,898,899,264,900,32,917,1053,33,915,906,911,914,919,80,928,922,925,929,246,34,35,945,960,971,962,964,976,980,977,1010,983,1030,1032,1028,921,563,1059,341,563,987,1021,1027,1033,303,1034,133,1044,1046,1048,1060,742,1024,245,38,39,341,41,42,1054,305,1057,106,469,462,753,81,426,116,44,45,146,790,427,341,46,47,48,693,789,1022,166,83,783,415,84,85,86,317,694,40,465,541,558,80,87,599,571,0,866,1049,88,0,89,90,0,328,329,0,91,0,0,0,0,0,92,341,0,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,465,80,0,0,0,0,0,0,0,0,0,81,0,0,0,0,0,0,0,0,0,735,0,407,0,0,166,83,0,0,84,85,86,0,750,40,0,0,0,0,87,0,341,0,0,0,88,0,89,90,0,0,0,81,91,0,0,0,0,563,92,0,0,0,0,721,0,0,166,83,0,0,84,85,86,0,0,40,0,0,0,0,87,0,0,0,0,0,88,0,89,90,341,0,0,0,91,0,0,0,0,0,92,0,0,465,0,0,801,0,803,0,0,0,0,0,0,0,465,0,0,810,2,3,4,76,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,764,765,21,171,172,268,0,269,270,271,272,273,274,275,276,22,80,23,173,277,0,0,174,24,278,0,0,27,175,176,177,178,0,28,29,0,179,0,279,0,0,32,180,181,33,182,0,0,0,0,0,0,0,0,0,0,34,35,36,0,183,184,0,0,0,465,0,0,0,0,0,0,0,0,0,0,0,0,0,465,0,0,0,280,83,185,186,84,85,86,38,39,40,41,42,187,281,137,- 310,0,0,0,0,188,0,44,283,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,76,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,171,172,268,0,269,270,271,272,273,274,275,276,22,80,23,173,277,0,0,174,24,278,465,0,27,175,176,177,178,0,28,29,0,179,0,279,0,0,32,180,181,33,182,0,0,0,0,0,0,0,0,0,0,34,35,36,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,280,83,185,186,84,85,86,38,39,40,41,42,187,281,137,282,0,0,0,0,188,0,44,283,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,76,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,171,172,268,0,269,270,271,272,273,274,275,276,22,80,23,173,277,0,0,174,24,278,0,0,27,175,176,177,178,0,28,29,- 39,179,0,279,0,0,32,180,181,33,182,0,0,0,0,0,0,0,0,0,0,34,35,36,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,280,83,185,186,84,85,86,38,39,40,41,42,187,281,137,0,0,0,0,0,188,0,44,283,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,76,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,171,172,268,0,269,270,271,272,273,274,275,276,22,80,23,173,277,0,0,174,24,278,0,0,27,175,176,177,178,0,28,29,0,179,0,279,0,0,32,180,181,33,182,0,0,0,0,0,0,0,0,0,0,34,35,36,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,280,83,185,186,84,85,86,38,39,40,41,42,187,281,137,0,0,0,0,0,188,0,44,283,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,76,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,171,172,268,0,269,270,271,272,273,274,275,276,22,80,23,173,277,0,0,174,24,278,0,0,27,175,176,177,178,0,28,29,0,179,0,279,0,0,32,180,181,33,182,0,0,0,0,0,0,0,0,0,0,34,35,36,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,485,83,185,186,84,85,86,38,39,40,41,42,187,281,137,0,0,0,0,0,188,0,44,283,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,76,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,171,172,0,0,0,0,0,0,0,0,0,0,22,80,23,173,0,0,0,174,24,0,0,0,27,175,176,177,178,0,28,29,0,179,0,0,0,0,32,180,181,33,182,0,0,0,0,0,0,0,0,0,0,34,35,36,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,38,39,40,41,42,187,658,0,0,0,0,0,0,188,0,44,283,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,171,172,0,0,0,0,0,0,0,0,0,0,22,80,0,173,0,0,0,174,0,0,0,0,27,175,176,177,178,0,28,29,0,179,0,0,0,0,32,180,181,33,182,0,0,0,0,0,0,0,0,0,0,34,35,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,38,39,40,41,42,187,0,356,0,0,0,0,0,188,0,44,283,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,171,172,0,0,0,0,0,0,0,0,0,0,22,80,0,173,0,0,0,174,0,0,0,0,27,175,176,177,178,0,28,29,0,179,0,0,0,0,32,180,181,33,182,0,0,0,0,0,0,0,0,0,0,34,35,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,38,39,40,41,42,187,0,0,0,0,0,0,0,188,0,44,283,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,688,0,24,0,25,26,27,0,0,0,0,0,28,29,- 39,0,0,0,30,31,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,36,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,0,0,0,0,0,0,38,39,40,41,42,0,0,0,- 11,0,0,0,0,43,0,44,45,0,0,0,0,46,47,48,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,28,29,- 39,0,0,0,30,31,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,36,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,0,0,0,0,0,0,38,39,40,41,42,0,0,0,- 11,0,0,0,0,43,0,44,45,0,0,0,0,46,47,48,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,28,29,- 39,0,0,0,30,31,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,36,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,0,0,0,0,0,0,38,39,40,41,42,0,0,0,0,0,0,0,0,43,0,44,45,0,0,0,0,46,47,48,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,28,29,- 39,0,0,0,30,31,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,36,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,37,0,0,0,0,0,22,38,39,40,41,42,0,0,0,- 11,0,0,27,0,43,0,44,45,28,29,0,0,46,47,48,0,32,0,0,33,0,0,0,0,0,0,0,80,0,0,0,34,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,303,0,0,37,0,0,0,0,0,22,38,39,40,41,42,0,305,0,0,0,0,27,0,445,81,44,45,28,29,427,0,46,47,48,0,32,0,0,33,82,83,0,0,84,85,86,0,0,40,0,34,35,0,87,0,0,0,0,0,88,0,89,90,0,0,0,0,91,0,0,0,0,0,92,0,0,0,37,0,0,0,0,0,0,38,39,40,41,42,0,0,0,0,0,0,0,0,470,0,44,45,0,0,0,0,46,47,48,2,3,4,76,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,0,0,27,0,0,0,0,0,28,29,- 39,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,36,0,0,0,2,3,4,76,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,38,39,0,41,42,0,22,137,23,0,258,0,0,0,24,44,45,0,27,0,0,0,0,0,28,29,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,36,0,0,2,3,4,76,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,38,39,0,41,42,22,0,137,0,0,0,0,0,24,0,44,45,27,0,0,0,0,0,28,29,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,38,39,0,41,42,0,0,0,0,0,0,27,0,0,0,44,45,28,29,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,303,0,0,304,0,0,0,0,0,22,38,39,0,41,42,0,305,0,0,0,0,27,0,0,0,44,45,28,29,0,0,224,0,0,0,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,38,39,0,41,42,0,0,225,0,0,0,27,0,0,0,44,45,28,29,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,303,0,0,0,0,0,0,0,0,22,38,39,0,41,42,0,305,0,0,0,0,27,0,0,0,44,45,28,29,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,38,39,0,41,42,0,0,0,417,0,0,27,0,0,0,44,45,28,29,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,38,39,0,41,42,0,0,0,674,0,0,27,0,0,0,44,45,28,29,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,681,0,0,0,0,0,0,0,0,22,38,39,0,41,42,0,0,0,0,0,0,27,0,0,0,44,45,28,29,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,38,39,0,41,42,0,0,0,0,0,0,27,0,0,0,44,45,28,29,0,80,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,171,172,268,0,269,270,271,272,273,274,275,276,0,80,0,173,277,0,0,174,0,278,0,0,0,175,176,177,178,0,81,0,38,179,0,279,42,0,0,180,181,0,182,0,0,166,83,44,45,84,85,86,0,0,40,0,0,183,184,87,0,0,0,0,0,88,0,89,90,0,0,0,0,91,0,0,0,0,0,92,485,83,185,186,84,85,86,0,0,40,0,0,187,281,137,0,0,171,172,0,188,0,0,189,546,0,0,0,190,0,80,191,173,0,192,193,174,0,194,195,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,342,547,0,171,172,0,188,0,0,189,0,328,329,0,190,0,80,191,173,0,192,193,174,0,194,195,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,342,798,0,171,172,0,188,0,0,189,0,328,329,0,190,0,80,191,173,0,192,193,174,0,194,195,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,342,972,0,171,172,0,188,0,0,189,0,328,329,0,190,0,80,191,173,0,192,193,174,0,194,195,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,342,0,0,171,172,0,188,0,0,189,0,328,329,0,190,0,80,191,173,0,192,193,174,0,194,195,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,0,0,0,171,172,0,188,0,0,189,0,0,0,310,190,0,80,191,173,0,192,193,174,0,194,195,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,342,0,0,171,172,0,188,0,0,189,0,0,0,0,190,0,80,191,173,0,192,193,174,0,194,195,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,491,0,0,0,171,172,0,188,0,0,189,0,0,0,0,190,0,80,191,173,0,192,193,174,0,194,195,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,0,0,0,171,172,0,188,589,0,189,0,0,0,0,190,0,80,191,173,0,192,193,174,0,194,195,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,0,0,0,171,172,0,188,0,0,189,0,0,0,610,190,0,80,191,173,0,192,193,174,0,194,195,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,0,0,0,171,172,0,188,664,0,189,0,0,0,0,190,0,80,191,173,0,192,193,174,0,194,195,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,0,0,0,171,172,0,188,0,0,189,0,0,0,734,190,0,80,191,173,0,192,193,174,0,194,195,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,0,0,0,171,172,0,188,0,0,189,0,0,0,749,190,0,80,191,173,0,192,193,174,0,194,195,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,769,0,0,0,171,172,0,188,0,0,189,0,0,0,0,190,0,80,191,173,0,192,193,174,0,194,195,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,771,0,0,0,171,172,0,188,0,0,189,0,0,0,0,190,0,80,191,173,0,192,193,174,0,194,195,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,0,0,0,171,172,0,188,871,0,189,0,0,0,0,190,0,80,191,173,0,192,193,174,0,194,195,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,0,0,0,171,172,0,188,874,0,189,0,0,0,0,190,0,80,191,173,0,192,193,174,0,194,195,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,877,0,0,0,171,172,0,188,0,0,189,0,0,0,0,190,0,80,191,173,0,192,193,174,0,194,195,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,0,0,0,171,172,0,188,951,0,189,0,0,0,0,190,0,80,191,173,0,192,193,174,0,194,195,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,0,0,0,171,172,0,188,955,0,189,0,0,0,0,190,0,80,191,173,0,192,193,174,0,194,195,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,0,0,0,171,172,0,188,957,0,189,0,0,0,0,190,0,80,191,173,0,192,193,174,0,194,195,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,0,0,0,171,172,0,188,1019,0,189,0,0,0,0,190,0,80,191,173,0,192,193,174,0,194,195,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,180,181,0,182,0,0,0,0,0,171,172,0,0,0,0,0,0,0,183,184,0,0,80,0,0,0,0,0,0,0,0,0,0,0,175,176,177,178,0,0,0,37,83,185,186,84,85,86,180,181,40,182,0,187,0,0,0,171,172,0,0,188,0,0,189,0,183,184,0,190,80,0,191,0,0,192,193,0,0,194,195,0,175,176,177,178,0,0,0,37,83,185,186,84,85,86,180,181,40,182,0,187,0,342,0,171,172,0,0,188,0,0,189,0,183,184,0,190,80,0,191,0,0,192,193,0,0,194,195,0,175,176,177,178,0,0,0,37,83,185,186,84,85,86,180,181,40,182,0,187,0,712,0,171,172,0,0,188,0,0,189,0,183,184,0,190,80,0,191,0,0,192,193,0,0,194,195,0,175,176,177,178,0,0,0,37,83,185,186,84,85,86,180,181,40,182,0,187,0,0,0,171,172,0,0,337,0,0,189,0,183,184,0,190,80,0,191,0,0,192,193,0,0,194,195,0,175,176,177,178,0,0,0,37,83,185,186,84,85,86,180,181,40,182,0,187,0,0,0,171,172,0,0,353,0,0,189,0,183,184,0,190,80,0,191,0,0,192,193,0,0,194,195,0,175,176,177,178,0,0,0,37,83,185,186,84,85,86,180,181,40,182,0,187,0,0,0,0,0,0,0,188,0,0,189,0,183,184,0,190,0,0,191,0,0,192,193,0,0,194,195,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,0,0,0,0,0,0,920,0,0,189,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195};
# 2664
static short Cyc_yycheck[6665]={0,113,143,365,246,0,156,284,285,367,287,258,371,372,433,67,174,377,378,174,143,21,401,100,101,127,21,104,188,64,370,165,0,368,173,64,379,380,594,143,73,511,756,43,260,67,5,6,21,227,50,91,448,21,91,50,428,57,58,59,17,61,57,58,59,225,61,67,68,59,123,106,67,121,452,127,448,106,126,356,476,297,50,342,57,58,59,139,61,57,58,59,105,61,67,135,129,516,135,67,328,114,102,103,158,127,85,102,103,823,103,329,125,112,105,137,495,139,118,119,120,121,112,118,119,120,121,127,127,119,23,509,127,603,102,103,85,137,121,139,518,134,137,107,139,124,146,112,105,132,118,119,120,121,127,305,712,136,85,127,160,120,127,126,137,160,139,86,87,137,398,139,121,136,402,124,123,337,50,339,190,191,192,322,105,308,320,136,81,105,329,351,160,353,121,105,126,124,114,92,105,211,123,445,123,124,136,259,113,114,115,246,126,123,123,105,118,81,82,83,125,130,136,113,114,115,131,227,470,264,102,103,227,86,87,125,313,259,373,374,375,376,118,421,422,423,118,119,120,121,466,229,136,471,123,232,140,70,235,259,88,125,284,285,259,287,130,131,246,118,123,124,119,133,653,168,125,105,125,125,105,141,142,433,284,285,259,287,160,284,285,259,287,118,81,82,83,105,106,0,105,126,121,550,129,113,515,126,113,114,115,284,285,105,287,315,284,285,49,287,105,22,23,216,25,302,118,23,113,121,223,123,471,104,356,594,125,105,125,416,121,318,43,130,131,113,114,115,129,50,126,511,120,118,130,56,356,122,87,436,188,356,774,64,104,66,516,116,69,105,105,381,382,383,123,535,125,78,114,114,81,623,131,356,119,81,498,125,356,125,667,92,128,221,136,637,92,125,140,501,121,102,103,131,121,106,135,136,129,110,445,789,129,448,20,21,229,118,119,120,121,421,422,423,642,524,421,422,423,506,430,843,126,246,121,470,130,330,119,140,699,476,129,603,57,445,123,447,276,501,121,264,699,712,271,126,456,121,458,160,277,456,126,458,165,487,119,168,696,170,470,680,168,614,170,497,122,121,121,501,708,709,126,126,130,131,627,487,456,121,458,614,487,222,126,121,118,497,519,121,126,501,497,642,627,121,501,919,237,337,126,610,113,925,487,216,119,726,411,487,216,349,223,627,497,353,676,223,501,497,530,132,527,501,107,530,754,116,366,593,105,106,143,962,123,246,125,705,113,672,705,561,131,800,118,814,257,121,681,613,105,121,530,264,926,266,126,814,269,125,928,118,626,932,933,403,988,716,938,939,171,121,173,174,456,286,458,608,623,122,867,931,183,184,929,930,673,940,941,190,191,192,637,43,121,640,121,816,126,126,819,126,612,426,125,754,56,669,118,320,211,830,121,644,118,226,1038,228,105,330,630,730,445,125,330,734,113,114,115,1051,125,618,119,902,120,667,121,669,925,121,749,126,480,481,133,902,484,470,530,487,122,262,141,142,116,893,884,667,496,669,110,123,667,125,669,125,125,406,125,131,497,131,125,133,134,135,118,847,121,121,690,686,90,126,667,690,669,906,125,667,140,669,911,596,121,118,121,125,121,126,411,126,733,414,121,411,737,125,414,126,121,125,690,822,121,126,747,839,137,126,118,322,125,121,125,834,787,897,329,133,134,135,719,105,445,884,125,448,477,478,139,113,114,115,731,456,791,458,138,780,920,1038,922,125,815,888,119,788,745,470,105,133,134,135,365,476,367,368,369,370,371,372,373,374,375,376,377,378,379,380,381,382,383,118,118,118,121,121,121,934,935,936,937,105,118,118,1034,121,121,969,623,126,962,118,119,38,125,257,690,17,18,19,118,119,637,426,120,640,120,530,658,659,105,105,106,107,88,89,848,656,120,114,543,116,445,867,286,543,112,105,120,452,125,904,905,105,866,119,114,910,133,134,135,421,422,423,119,867,1028,125,125,764,867,118,114,84,85,133,134,135,471,125,105,106,107,108,109,110,111,125,619,114,76,77,867,596,118,498,1037,867,596,125,125,925,144,128,25,26,509,5,6,129,120,121,22,23,120,518,125,38,120,121,623,751,975,925,120,121,979,122,925,966,942,943,944,105,637,138,139,640,768,769,963,771,761,773,105,136,649,126,767,129,925,126,113,114,115,925,88,89,84,85,116,38,120,121,125,132,120,797,561,90,131,105,133,134,135,987,120,759,760,113,114,115,124,124,105,106,121,690,109,110,111,125,120,114,91,129,126,126,119,133,134,135,121,123,125,120,127,128,126,121,119,614,133,90,1038,136,258,847,139,140,623,125,143,144,627,102,126,121,105,106,105,124,109,110,111,118,1038,114,125,54,121,1038,119,873,129,119,876,877,125,642,127,128,871,130,131,874,133,30,118,42,125,764,139,1038,136,73,764,126,1038,126,312,132,126,121,105,126,121,92,93,94,95,96,97,98,99,100,101,121,791,121,920,126,120,126,132,121,126,927,917,126,342,126,120,345,346,347,348,122,350,126,126,125,705,126,126,945,105,359,125,120,125,363,119,716,118,118,126,74,105,959,120,950,951,124,119,954,955,114,957,969,105,118,384,68,69,70,71,72,125,119,75,76,77,756,396,123,133,134,135,401,25,754,126,126,126,132,126,8,9,10,11,12,13,14,15,16,17,18,19,20,21,105,126,126,126,427,789,126,118,113,114,115,116,125,1013,132,37,1016,119,1018,1019,125,129,126,120,118,118,125,49,133,134,135,112,106,55,56,126,125,121,126,823,126,63,28,1043,66,120,126,126,126,119,38,90,125,129,137,839,78,79,90,120,25,121,120,120,118,121,118,120,106,120,125,845,495,0,848,498,126,126,126,126,102,121,966,120,126,126,0,622,966,112,112,113,866,115,116,126,118,126,34,262,257,640,90,125,47,127,128,69,686,131,884,133,134,135,102,681,962,105,106,676,223,109,110,111,157,543,114,550,331,352,38,119,414,369,- 1,764,1034,125,- 1,127,128,- 1,130,131,- 1,133,- 1,- 1,- 1,- 1,- 1,139,926,- 1,928,929,930,931,932,933,934,935,936,937,938,939,940,941,942,943,944,594,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,615,- 1,102,- 1,- 1,105,106,- 1,- 1,109,110,111,- 1,628,114,- 1,- 1,- 1,- 1,119,- 1,987,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,90,133,- 1,- 1,- 1,- 1,653,139,- 1,- 1,- 1,- 1,102,- 1,- 1,105,106,- 1,- 1,109,110,111,- 1,- 1,114,- 1,- 1,- 1,- 1,119,- 1,- 1,- 1,- 1,- 1,125,- 1,127,128,1037,- 1,- 1,- 1,133,- 1,- 1,- 1,- 1,- 1,139,- 1,- 1,699,- 1,- 1,702,- 1,704,- 1,- 1,- 1,- 1,- 1,- 1,- 1,712,- 1,- 1,715,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,50,51,52,53,- 1,55,56,- 1,58,- 1,60,- 1,- 1,63,64,65,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,- 1,82,83,- 1,- 1,- 1,800,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,814,- 1,- 1,- 1,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,- 1,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,902,- 1,49,50,51,52,53,- 1,55,56,- 1,58,- 1,60,- 1,- 1,63,64,65,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,- 1,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,50,51,52,53,- 1,55,56,57,58,- 1,60,- 1,- 1,63,64,65,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,- 1,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,- 1,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,50,51,52,53,- 1,55,56,- 1,58,- 1,60,- 1,- 1,63,64,65,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,- 1,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,- 1,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,50,51,52,53,- 1,55,56,- 1,58,- 1,60,- 1,- 1,63,64,65,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,- 1,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,- 1,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,- 1,- 1,44,45,- 1,- 1,- 1,49,50,51,52,53,- 1,55,56,- 1,58,- 1,- 1,- 1,- 1,63,64,65,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,112,113,114,115,116,117,118,- 1,- 1,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,- 1,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,50,51,52,53,- 1,55,56,- 1,58,- 1,- 1,- 1,- 1,63,64,65,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,112,113,114,115,116,117,- 1,119,- 1,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,- 1,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,50,51,52,53,- 1,55,56,- 1,58,- 1,- 1,- 1,- 1,63,64,65,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,112,113,114,115,116,117,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,- 1,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,43,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,61,62,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,- 1,- 1,- 1,120,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,134,135,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,61,62,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,- 1,- 1,- 1,120,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,134,135,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,61,62,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,134,135,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,61,62,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,- 1,- 1,- 1,- 1,- 1,37,112,113,114,115,116,- 1,- 1,- 1,120,- 1,- 1,49,- 1,125,- 1,127,128,55,56,- 1,- 1,133,134,135,- 1,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,78,79,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,102,- 1,- 1,105,- 1,- 1,- 1,- 1,- 1,37,112,113,114,115,116,- 1,118,- 1,- 1,- 1,- 1,49,- 1,125,90,127,128,55,56,131,- 1,133,134,135,- 1,63,- 1,- 1,66,105,106,- 1,- 1,109,110,111,- 1,- 1,114,- 1,78,79,- 1,119,- 1,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,- 1,- 1,- 1,- 1,- 1,139,- 1,- 1,- 1,105,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,134,135,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,112,113,- 1,115,116,- 1,37,119,39,- 1,122,- 1,- 1,- 1,45,127,128,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,- 1,115,116,37,- 1,119,- 1,- 1,- 1,- 1,- 1,45,- 1,127,128,49,- 1,- 1,- 1,- 1,- 1,55,56,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,- 1,115,116,- 1,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,127,128,55,56,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,102,- 1,- 1,105,- 1,- 1,- 1,- 1,- 1,37,112,113,- 1,115,116,- 1,118,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,127,128,55,56,- 1,- 1,59,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,- 1,115,116,- 1,- 1,119,- 1,- 1,- 1,49,- 1,- 1,- 1,127,128,55,56,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,102,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,- 1,115,116,- 1,118,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,127,128,55,56,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,- 1,115,116,- 1,- 1,- 1,120,- 1,- 1,49,- 1,- 1,- 1,127,128,55,56,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,- 1,115,116,- 1,- 1,- 1,120,- 1,- 1,49,- 1,- 1,- 1,127,128,55,56,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,102,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,- 1,115,116,- 1,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,127,128,55,56,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,- 1,115,116,- 1,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,127,128,55,56,- 1,38,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,38,- 1,40,41,- 1,- 1,44,- 1,46,- 1,- 1,- 1,50,51,52,53,- 1,90,- 1,112,58,- 1,60,116,- 1,- 1,64,65,- 1,67,- 1,- 1,105,106,127,128,109,110,111,- 1,- 1,114,- 1,- 1,82,83,119,- 1,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,- 1,- 1,- 1,- 1,- 1,139,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,118,119,- 1,- 1,25,26,- 1,125,- 1,- 1,128,32,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,119,120,- 1,25,26,- 1,125,- 1,- 1,128,- 1,130,131,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,119,120,- 1,25,26,- 1,125,- 1,- 1,128,- 1,130,131,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,119,120,- 1,25,26,- 1,125,- 1,- 1,128,- 1,130,131,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,119,- 1,- 1,25,26,- 1,125,- 1,- 1,128,- 1,130,131,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,- 1,- 1,128,- 1,- 1,- 1,132,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,119,- 1,- 1,25,26,- 1,125,- 1,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,118,- 1,- 1,- 1,25,26,- 1,125,- 1,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,126,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,- 1,- 1,128,- 1,- 1,- 1,132,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,126,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,- 1,- 1,128,- 1,- 1,- 1,132,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,- 1,- 1,128,- 1,- 1,- 1,132,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,118,- 1,- 1,- 1,25,26,- 1,125,- 1,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,118,- 1,- 1,- 1,25,26,- 1,125,- 1,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,126,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,126,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,118,- 1,- 1,- 1,25,26,- 1,125,- 1,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,126,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,126,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,126,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,126,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,51,52,53,- 1,- 1,- 1,105,106,107,108,109,110,111,64,65,114,67,- 1,117,- 1,- 1,- 1,25,26,- 1,- 1,125,- 1,- 1,128,- 1,82,83,- 1,133,38,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,- 1,50,51,52,53,- 1,- 1,- 1,105,106,107,108,109,110,111,64,65,114,67,- 1,117,- 1,119,- 1,25,26,- 1,- 1,125,- 1,- 1,128,- 1,82,83,- 1,133,38,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,- 1,50,51,52,53,- 1,- 1,- 1,105,106,107,108,109,110,111,64,65,114,67,- 1,117,- 1,119,- 1,25,26,- 1,- 1,125,- 1,- 1,128,- 1,82,83,- 1,133,38,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,- 1,50,51,52,53,- 1,- 1,- 1,105,106,107,108,109,110,111,64,65,114,67,- 1,117,- 1,- 1,- 1,25,26,- 1,- 1,125,- 1,- 1,128,- 1,82,83,- 1,133,38,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,- 1,50,51,52,53,- 1,- 1,- 1,105,106,107,108,109,110,111,64,65,114,67,- 1,117,- 1,- 1,- 1,25,26,- 1,- 1,125,- 1,- 1,128,- 1,82,83,- 1,133,38,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,- 1,50,51,52,53,- 1,- 1,- 1,105,106,107,108,109,110,111,64,65,114,67,- 1,117,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,- 1,- 1,128,- 1,82,83,- 1,133,- 1,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,- 1,- 1,128,- 1,- 1,- 1,- 1,133,- 1,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144};char Cyc_Yystack_overflow[17]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
void Cyc_yyerror(struct _dyneither_ptr,int state,int token);
# 82 "cycbison.simple"
int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*yylval_ptr,struct Cyc_Yyltype*yylloc);struct _tuple27{unsigned int f1;struct _tuple0*f2;int f3;};struct _tuple28{struct _dyneither_ptr f1;void*f2;};static char _tmp505[8]="stdcall";static char _tmp506[6]="cdecl";static char _tmp507[9]="fastcall";static char _tmp508[9]="noreturn";static char _tmp509[6]="const";static char _tmp50A[8]="aligned";static char _tmp50B[7]="packed";static char _tmp50C[7]="shared";static char _tmp50D[7]="unused";static char _tmp50E[5]="weak";static char _tmp50F[10]="dllimport";static char _tmp510[10]="dllexport";static char _tmp511[23]="no_instrument_function";static char _tmp512[12]="constructor";static char _tmp513[11]="destructor";static char _tmp514[22]="no_check_memory_usage";static char _tmp515[5]="pure";struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf);static void _tmp9FF(union Cyc_YYSTYPE*yylval,unsigned int*_tmp9FE,unsigned int*_tmp9FD,union Cyc_YYSTYPE**_tmp9FC){for(*_tmp9FE=0;*_tmp9FE < *_tmp9FD;(*_tmp9FE)++){(*_tmp9FC)[*_tmp9FE]=(union Cyc_YYSTYPE)*yylval;}}static void _tmpA04(unsigned int*_tmpA03,unsigned int*_tmpA02,struct Cyc_Yyltype**_tmpA01){for(*_tmpA03=0;*_tmpA03 < *_tmpA02;(*_tmpA03)++){(*_tmpA01)[*_tmpA03]=(struct Cyc_Yyltype)
# 164 "cycbison.simple"
Cyc_yynewloc();}}static void _tmpA0B(int*yyssp_offset,struct _dyneither_ptr*yyss,unsigned int*_tmpA0A,unsigned int*_tmpA09,short**_tmpA07){for(*_tmpA0A=0;*_tmpA0A < *_tmpA09;(*_tmpA0A)++){(*_tmpA07)[*_tmpA0A]=(short)(
# 216
*_tmpA0A <= *yyssp_offset?*((short*)_check_dyneither_subscript(*yyss,sizeof(short),(int)*_tmpA0A)): 0);}}static void _tmpA11(struct _dyneither_ptr*yyvs,int*yyssp_offset,union Cyc_YYSTYPE*yylval,unsigned int*_tmpA10,unsigned int*_tmpA0F,union Cyc_YYSTYPE**_tmpA0D){for(*_tmpA10=0;*_tmpA10 < *_tmpA0F;(*_tmpA10)++){(*_tmpA0D)[*_tmpA10]=(union Cyc_YYSTYPE)(
# 219
*_tmpA10 <= *yyssp_offset?*((union Cyc_YYSTYPE*)_check_dyneither_subscript(*yyvs,sizeof(union Cyc_YYSTYPE),(int)*_tmpA10)):*yylval);}}static void _tmpA17(int*yyssp_offset,struct _dyneither_ptr*yyls,unsigned int*_tmpA16,unsigned int*_tmpA15,struct Cyc_Yyltype**_tmpA13){for(*_tmpA16=0;*_tmpA16 < *_tmpA15;(*_tmpA16)++){(*_tmpA13)[*_tmpA16]=(struct Cyc_Yyltype)(
# 222
*_tmpA16 <= *yyssp_offset?*((struct Cyc_Yyltype*)_check_dyneither_subscript(*yyls,sizeof(struct Cyc_Yyltype),(int)*_tmpA16)):
 Cyc_yynewloc());}}static void _tmpF1D(unsigned int*_tmpF1C,unsigned int*_tmpF1B,char**_tmpF19){for(*_tmpF1C=0;*_tmpF1C < *_tmpF1B;(*_tmpF1C)++){(*_tmpF19)[*_tmpF1C]=(char)'\000';}}
# 137
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){
# 140
struct _RegionHandle _tmp448=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp448;_push_region(yyregion);
{int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 146
int yychar;
union Cyc_YYSTYPE _tmp9F8;union Cyc_YYSTYPE yylval=((_tmp9F8.YYINITIALSVAL).val=0,(((_tmp9F8.YYINITIALSVAL).tag=64,_tmp9F8)));
int yynerrs;
# 150
struct Cyc_Yyltype yylloc;
# 154
int yyssp_offset;
# 156
short*_tmp9FA;unsigned int _tmp9F9;struct _dyneither_ptr yyss=(_tmp9F9=200,((_tmp9FA=_region_calloc(yyregion,sizeof(short),_tmp9F9),_tag_dyneither(_tmp9FA,sizeof(short),_tmp9F9))));
# 158
int yyvsp_offset;
unsigned int _tmp9FE;unsigned int _tmp9FD;union Cyc_YYSTYPE*_tmp9FC;unsigned int _tmp9FB;struct _dyneither_ptr yyvs=_tag_dyneither(((_tmp9FB=(unsigned int)200,((_tmp9FC=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp9FB)),((((_tmp9FD=_tmp9FB,_tmp9FF(& yylval,& _tmp9FE,& _tmp9FD,& _tmp9FC))),_tmp9FC)))))),sizeof(union Cyc_YYSTYPE),(unsigned int)200);
# 163
int yylsp_offset;
unsigned int _tmpA03;unsigned int _tmpA02;struct Cyc_Yyltype*_tmpA01;unsigned int _tmpA00;struct _dyneither_ptr yyls=_tag_dyneither(((_tmpA00=(unsigned int)200,((_tmpA01=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmpA00)),((((_tmpA02=_tmpA00,_tmpA04(& _tmpA03,& _tmpA02,& _tmpA01))),_tmpA01)))))),sizeof(struct Cyc_Yyltype),(unsigned int)200);
# 170
int yystacksize=200;
# 172
union Cyc_YYSTYPE yyval=yylval;
# 176
int yylen;
# 183
yystate=0;
yyerrstatus=0;
yynerrs=0;
yychar=-2;
# 193
yyssp_offset=-1;
yyvsp_offset=0;
# 196
yylsp_offset=0;
# 202
yynewstate:
# 204
*((short*)_check_dyneither_subscript(yyss,sizeof(short),++ yyssp_offset))=(short)yystate;
# 206
if(yyssp_offset >= yystacksize - 1){
# 208
if(yystacksize >= 10000){
{const char*_tmpA05;Cyc_yyerror(((_tmpA05="parser stack overflow",_tag_dyneither(_tmpA05,sizeof(char),22))),yystate,yychar);}
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 212
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
unsigned int _tmpA0A;unsigned int _tmpA09;struct _dyneither_ptr _tmpA08;short*_tmpA07;unsigned int _tmpA06;struct _dyneither_ptr yyss1=(_tmpA06=(unsigned int)yystacksize,((_tmpA07=(short*)_region_malloc(yyregion,_check_times(sizeof(short),_tmpA06)),((_tmpA08=_tag_dyneither(_tmpA07,sizeof(short),_tmpA06),((((_tmpA09=_tmpA06,_tmpA0B(& yyssp_offset,& yyss,& _tmpA0A,& _tmpA09,& _tmpA07))),_tmpA08)))))));
# 217
unsigned int _tmpA10;unsigned int _tmpA0F;struct _dyneither_ptr _tmpA0E;union Cyc_YYSTYPE*_tmpA0D;unsigned int _tmpA0C;struct _dyneither_ptr yyvs1=
(_tmpA0C=(unsigned int)yystacksize,((_tmpA0D=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmpA0C)),((_tmpA0E=_tag_dyneither(_tmpA0D,sizeof(union Cyc_YYSTYPE),_tmpA0C),((((_tmpA0F=_tmpA0C,_tmpA11(& yyvs,& yyssp_offset,& yylval,& _tmpA10,& _tmpA0F,& _tmpA0D))),_tmpA0E)))))));
# 221
unsigned int _tmpA16;unsigned int _tmpA15;struct _dyneither_ptr _tmpA14;struct Cyc_Yyltype*_tmpA13;unsigned int _tmpA12;struct _dyneither_ptr yyls1=(_tmpA12=(unsigned int)yystacksize,((_tmpA13=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmpA12)),((_tmpA14=_tag_dyneither(_tmpA13,sizeof(struct Cyc_Yyltype),_tmpA12),((((_tmpA15=_tmpA12,_tmpA17(& yyssp_offset,& yyls,& _tmpA16,& _tmpA15,& _tmpA13))),_tmpA14)))))));
# 225
yyss=yyss1;
yyvs=yyvs1;
# 228
yyls=yyls1;};}
# 238
goto yybackup;
# 240
yybackup:
# 252 "cycbison.simple"
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1061,yystate)];
if(yyn == - 32768)goto yydefault;
# 259
if(yychar == - 2)
# 265
yychar=Cyc_yylex(yylex_buf,& yylval,& yylloc);
# 269
if(yychar <= 0){
# 271
yychar1=0;
yychar=0;}else{
# 280
yychar1=yychar > 0  && yychar <= 372?(int)Cyc_yytranslate[_check_known_subscript_notnull(373,yychar)]: 291;}
# 297 "cycbison.simple"
yyn +=yychar1;
if((yyn < 0  || yyn > 6664) || Cyc_yycheck[_check_known_subscript_notnull(6665,yyn)]!= yychar1)goto yydefault;
# 300
yyn=(int)Cyc_yytable[_check_known_subscript_notnull(6665,yyn)];
# 307
if(yyn < 0){
# 309
if(yyn == - 32768)goto yyerrlab;
yyn=- yyn;
goto yyreduce;}else{
# 313
if(yyn == 0)goto yyerrlab;}
# 315
if(yyn == 1060){
int _tmp456=0;_npop_handler(0);return _tmp456;}
# 326 "cycbison.simple"
if(yychar != 0)
yychar=-2;
# 329
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yylval;
# 331
*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),++ yylsp_offset))=yylloc;
# 336
if(yyerrstatus != 0)-- yyerrstatus;
# 338
yystate=yyn;
goto yynewstate;
# 342
yydefault:
# 344
 yyn=(int)Cyc_yydefact[_check_known_subscript_notnull(1061,yystate)];
if(yyn == 0)goto yyerrlab;
# 349
yyreduce:
# 351
 yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(525,yyn)];
if(yylen > 0)
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),(yyvsp_offset + 1)- yylen));
# 369 "cycbison.simple"
switch(yyn){case 1: _LL20F: {
# 372
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 374
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1181 "parse.y"
yyval=yyyyvsp[0];
Cyc_Parse_parse_result=Cyc_yyget_YY16(yyyyvsp[0]);
# 1184
break;}case 2: _LL210: {
# 1186
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1188
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1187 "parse.y"
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp457;_push_handler(& _tmp457);{int _tmp459=0;if(setjmp(_tmp457.handler))_tmp459=1;if(!_tmp459){
x=Cyc_yyget_YY16(yyyyvsp[0]);;_pop_handler();}else{void*_tmp458=(void*)_exn_thrown;void*_tmp45B=_tmp458;void*_tmp45D;_LL213: {struct Cyc_Core_Failure_exn_struct*_tmp45C=(struct Cyc_Core_Failure_exn_struct*)_tmp45B;if(_tmp45C->tag != Cyc_Core_Failure)goto _LL215;}_LL214:
# 1192
 x=0;goto _LL212;_LL215: _tmp45D=_tmp45B;_LL216:(void)_rethrow(_tmp45D);_LL212:;}};}
# 1194
{struct _handler_cons _tmp45E;_push_handler(& _tmp45E);{int _tmp460=0;if(setjmp(_tmp45E.handler))_tmp460=1;if(!_tmp460){
y=Cyc_yyget_YY16(yyyyvsp[1]);;_pop_handler();}else{void*_tmp45F=(void*)_exn_thrown;void*_tmp462=_tmp45F;void*_tmp464;_LL218: {struct Cyc_Core_Failure_exn_struct*_tmp463=(struct Cyc_Core_Failure_exn_struct*)_tmp462;if(_tmp463->tag != Cyc_Core_Failure)goto _LL21A;}_LL219:
# 1198
 y=0;goto _LL217;_LL21A: _tmp464=_tmp462;_LL21B:(void)_rethrow(_tmp464);_LL217:;}};}
# 1200
yyval=Cyc_YY16(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y));
# 1202
break;}case 3: _LL211: {
# 1204
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1206
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1205 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA21;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmpA20;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmpA1F;struct Cyc_List_List*_tmpA1E;yyval=Cyc_YY16(((_tmpA1E=_cycalloc(sizeof(*_tmpA1E)),((_tmpA1E->hd=((_tmpA21=_cycalloc(sizeof(*_tmpA21)),((_tmpA21->r=(void*)((_tmpA1F=_cycalloc(sizeof(*_tmpA1F)),((_tmpA1F[0]=((_tmpA20.tag=10,((_tmpA20.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA20.f2=Cyc_yyget_YY16(yyyyvsp[2]),_tmpA20)))))),_tmpA1F)))),((_tmpA21->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA21)))))),((_tmpA1E->tl=0,_tmpA1E)))))));}
Cyc_Lex_leave_using();
# 1208
break;}case 4: _LL21C: {
# 1210
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1212
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1209 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA2B;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmpA2A;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmpA29;struct Cyc_List_List*_tmpA28;yyval=Cyc_YY16(((_tmpA28=_cycalloc(sizeof(*_tmpA28)),((_tmpA28->hd=((_tmpA2B=_cycalloc(sizeof(*_tmpA2B)),((_tmpA2B->r=(void*)((_tmpA29=_cycalloc(sizeof(*_tmpA29)),((_tmpA29[0]=((_tmpA2A.tag=10,((_tmpA2A.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA2A.f2=Cyc_yyget_YY16(yyyyvsp[2]),_tmpA2A)))))),_tmpA29)))),((_tmpA2B->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA2B)))))),((_tmpA28->tl=Cyc_yyget_YY16(yyyyvsp[4]),_tmpA28)))))));}
break;}case 5: _LL21D: {
# 1212
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1214
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1212 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA3A;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmpA39;struct _dyneither_ptr*_tmpA38;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpA37;struct Cyc_List_List*_tmpA36;yyval=Cyc_YY16(((_tmpA36=_cycalloc(sizeof(*_tmpA36)),((_tmpA36->hd=((_tmpA3A=_cycalloc(sizeof(*_tmpA3A)),((_tmpA3A->r=(void*)((_tmpA37=_cycalloc(sizeof(*_tmpA37)),((_tmpA37[0]=((_tmpA39.tag=9,((_tmpA39.f1=((_tmpA38=_cycalloc(sizeof(*_tmpA38)),((_tmpA38[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpA38)))),((_tmpA39.f2=Cyc_yyget_YY16(yyyyvsp[2]),_tmpA39)))))),_tmpA37)))),((_tmpA3A->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA3A)))))),((_tmpA36->tl=0,_tmpA36)))))));}
Cyc_Lex_leave_namespace();
# 1215
break;}case 6: _LL21E: {
# 1217
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1219
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1217 "parse.y"
struct _dyneither_ptr nspace;
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp472;_push_handler(& _tmp472);{int _tmp474=0;if(setjmp(_tmp472.handler))_tmp474=1;if(!_tmp474){nspace=Cyc_yyget_String_tok(yyyyvsp[0]);;_pop_handler();}else{void*_tmp473=(void*)_exn_thrown;void*_tmp476=_tmp473;void*_tmp478;_LL221: {struct Cyc_Core_Failure_exn_struct*_tmp477=(struct Cyc_Core_Failure_exn_struct*)_tmp476;if(_tmp477->tag != Cyc_Core_Failure)goto _LL223;}_LL222:
{const char*_tmpA3B;nspace=((_tmpA3B="",_tag_dyneither(_tmpA3B,sizeof(char),1)));}goto _LL220;_LL223: _tmp478=_tmp476;_LL224:(void)_rethrow(_tmp478);_LL220:;}};}
# 1222
{struct _handler_cons _tmp47A;_push_handler(& _tmp47A);{int _tmp47C=0;if(setjmp(_tmp47A.handler))_tmp47C=1;if(!_tmp47C){x=Cyc_yyget_YY16(yyyyvsp[2]);;_pop_handler();}else{void*_tmp47B=(void*)_exn_thrown;void*_tmp47E=_tmp47B;void*_tmp480;_LL226: {struct Cyc_Core_Failure_exn_struct*_tmp47F=(struct Cyc_Core_Failure_exn_struct*)_tmp47E;if(_tmp47F->tag != Cyc_Core_Failure)goto _LL228;}_LL227:
 x=0;goto _LL225;_LL228: _tmp480=_tmp47E;_LL229:(void)_rethrow(_tmp480);_LL225:;}};}
# 1225
{struct _handler_cons _tmp481;_push_handler(& _tmp481);{int _tmp483=0;if(setjmp(_tmp481.handler))_tmp483=1;if(!_tmp483){y=Cyc_yyget_YY16(yyyyvsp[4]);;_pop_handler();}else{void*_tmp482=(void*)_exn_thrown;void*_tmp485=_tmp482;void*_tmp487;_LL22B: {struct Cyc_Core_Failure_exn_struct*_tmp486=(struct Cyc_Core_Failure_exn_struct*)_tmp485;if(_tmp486->tag != Cyc_Core_Failure)goto _LL22D;}_LL22C:
 y=0;goto _LL22A;_LL22D: _tmp487=_tmp485;_LL22E:(void)_rethrow(_tmp487);_LL22A:;}};}
# 1228
{struct Cyc_Absyn_Decl*_tmpA4A;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmpA49;struct _dyneither_ptr*_tmpA48;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpA47;struct Cyc_List_List*_tmpA46;yyval=Cyc_YY16(((_tmpA46=_cycalloc(sizeof(*_tmpA46)),((_tmpA46->hd=((_tmpA4A=_cycalloc(sizeof(*_tmpA4A)),((_tmpA4A->r=(void*)((_tmpA47=_cycalloc(sizeof(*_tmpA47)),((_tmpA47[0]=((_tmpA49.tag=9,((_tmpA49.f1=((_tmpA48=_cycalloc(sizeof(*_tmpA48)),((_tmpA48[0]=nspace,_tmpA48)))),((_tmpA49.f2=x,_tmpA49)))))),_tmpA47)))),((_tmpA4A->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA4A)))))),((_tmpA46->tl=y,_tmpA46)))))));}
# 1230
break;}case 7: _LL21F: {
# 1232
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1234
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1231 "parse.y"
struct _dyneither_ptr _tmp48D=Cyc_yyget_String_tok(yyyyvsp[1]);
{const char*_tmpA4B;if(Cyc_strcmp((struct _dyneither_ptr)_tmp48D,((_tmpA4B="C",_tag_dyneither(_tmpA4B,sizeof(char),2))))== 0){
struct Cyc_Absyn_Decl*_tmpA55;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct _tmpA54;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmpA53;struct Cyc_List_List*_tmpA52;yyval=Cyc_YY16(((_tmpA52=_cycalloc(sizeof(*_tmpA52)),((_tmpA52->hd=((_tmpA55=_cycalloc(sizeof(*_tmpA55)),((_tmpA55->r=(void*)((_tmpA53=_cycalloc(sizeof(*_tmpA53)),((_tmpA53[0]=((_tmpA54.tag=11,((_tmpA54.f1=Cyc_yyget_YY16(yyyyvsp[3]),_tmpA54)))),_tmpA53)))),((_tmpA55->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA55)))))),((_tmpA52->tl=Cyc_yyget_YY16(yyyyvsp[5]),_tmpA52)))))));}else{
# 1235
{const char*_tmpA56;if(Cyc_strcmp((struct _dyneither_ptr)_tmp48D,((_tmpA56="C include",_tag_dyneither(_tmpA56,sizeof(char),10))))!= 0){
const char*_tmpA57;Cyc_Parse_err(((_tmpA57="expecting \"C\" or \"C include\"",_tag_dyneither(_tmpA57,sizeof(char),29))),(unsigned int)(yyyylsp[0]).first_line);}}{
# 1238
struct Cyc_Absyn_Decl*_tmpA61;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmpA60;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA5F;struct Cyc_List_List*_tmpA5E;yyval=Cyc_YY16(((_tmpA5E=_cycalloc(sizeof(*_tmpA5E)),((_tmpA5E->hd=((_tmpA61=_cycalloc(sizeof(*_tmpA61)),((_tmpA61->r=(void*)((_tmpA5F=_cycalloc(sizeof(*_tmpA5F)),((_tmpA5F[0]=((_tmpA60.tag=12,((_tmpA60.f1=Cyc_yyget_YY16(yyyyvsp[3]),((_tmpA60.f2=0,_tmpA60)))))),_tmpA5F)))),((_tmpA61->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA61)))))),((_tmpA5E->tl=Cyc_yyget_YY16(yyyyvsp[5]),_tmpA5E)))))));};}}
# 1241
break;}case 8: _LL22F: {
# 1243
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 1245
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 1242 "parse.y"
{const char*_tmpA62;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpA62="C include",_tag_dyneither(_tmpA62,sizeof(char),10))))!= 0){
const char*_tmpA63;Cyc_Parse_err(((_tmpA63="expecting \"C include\"",_tag_dyneither(_tmpA63,sizeof(char),22))),(unsigned int)(yyyylsp[0]).first_line);}}{
struct Cyc_List_List*exs=Cyc_yyget_YY52(yyyyvsp[5]);
{struct Cyc_Absyn_Decl*_tmpA6D;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmpA6C;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA6B;struct Cyc_List_List*_tmpA6A;yyval=Cyc_YY16(((_tmpA6A=_cycalloc(sizeof(*_tmpA6A)),((_tmpA6A->hd=((_tmpA6D=_cycalloc(sizeof(*_tmpA6D)),((_tmpA6D->r=(void*)((_tmpA6B=_cycalloc(sizeof(*_tmpA6B)),((_tmpA6B[0]=((_tmpA6C.tag=12,((_tmpA6C.f1=Cyc_yyget_YY16(yyyyvsp[3]),((_tmpA6C.f2=exs,_tmpA6C)))))),_tmpA6B)))),((_tmpA6D->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA6D)))))),((_tmpA6A->tl=Cyc_yyget_YY16(yyyyvsp[6]),_tmpA6A)))))));}
# 1247
break;};}case 9: _LL230: {
# 1249
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1251
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1248 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA70;struct Cyc_List_List*_tmpA6F;yyval=Cyc_YY16(((_tmpA6F=_cycalloc(sizeof(*_tmpA6F)),((_tmpA6F->hd=((_tmpA70=_cycalloc(sizeof(*_tmpA70)),((_tmpA70->r=(void*)& Cyc_Absyn_Porton_d_val,((_tmpA70->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA70)))))),((_tmpA6F->tl=Cyc_yyget_YY16(yyyyvsp[2]),_tmpA6F)))))));}
break;}case 10: _LL231: {
# 1251
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1253
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1250 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA73;struct Cyc_List_List*_tmpA72;yyval=Cyc_YY16(((_tmpA72=_cycalloc(sizeof(*_tmpA72)),((_tmpA72->hd=((_tmpA73=_cycalloc(sizeof(*_tmpA73)),((_tmpA73->r=(void*)& Cyc_Absyn_Portoff_d_val,((_tmpA73->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA73)))))),((_tmpA72->tl=Cyc_yyget_YY16(yyyyvsp[2]),_tmpA72)))))));}
break;}case 11: _LL232:
# 1253
 yyval=Cyc_YY16(0);
break;case 12: _LL233: {
# 1256
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1258
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1255 "parse.y"
yyval=yyyyvsp[2];
break;}case 13: _LL234: {
# 1258
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1260
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1256 "parse.y"
yyval=Cyc_YY52(0);
break;}case 14: _LL235: {
# 1259
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1261
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1260 "parse.y"
{struct _tuple27*_tmpA76;struct Cyc_List_List*_tmpA75;yyval=Cyc_YY52(((_tmpA75=_cycalloc(sizeof(*_tmpA75)),((_tmpA75->hd=((_tmpA76=_cycalloc(sizeof(*_tmpA76)),((_tmpA76->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA76->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA76->f3=0,_tmpA76)))))))),((_tmpA75->tl=0,_tmpA75)))))));}
break;}case 15: _LL236: {
# 1263
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1265
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1261 "parse.y"
{struct _tuple27*_tmpA79;struct Cyc_List_List*_tmpA78;yyval=Cyc_YY52(((_tmpA78=_cycalloc(sizeof(*_tmpA78)),((_tmpA78->hd=((_tmpA79=_cycalloc(sizeof(*_tmpA79)),((_tmpA79->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA79->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA79->f3=0,_tmpA79)))))))),((_tmpA78->tl=0,_tmpA78)))))));}
break;}case 16: _LL237: {
# 1264
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1266
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1263 "parse.y"
{struct _tuple27*_tmpA7C;struct Cyc_List_List*_tmpA7B;yyval=Cyc_YY52(((_tmpA7B=_cycalloc(sizeof(*_tmpA7B)),((_tmpA7B->hd=((_tmpA7C=_cycalloc(sizeof(*_tmpA7C)),((_tmpA7C->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA7C->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA7C->f3=0,_tmpA7C)))))))),((_tmpA7B->tl=Cyc_yyget_YY52(yyyyvsp[2]),_tmpA7B)))))));}
break;}case 17: _LL238: {
# 1266
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1268
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1267 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpA82;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpA81;struct Cyc_List_List*_tmpA80;yyval=Cyc_YY16(((_tmpA80=_cycalloc(sizeof(*_tmpA80)),((_tmpA80->hd=Cyc_Absyn_new_decl((void*)((_tmpA82=_cycalloc(sizeof(*_tmpA82)),((_tmpA82[0]=((_tmpA81.tag=1,((_tmpA81.f1=Cyc_yyget_YY15(yyyyvsp[0]),_tmpA81)))),_tmpA82)))),(unsigned int)(yyyylsp[0]).first_line),((_tmpA80->tl=0,_tmpA80)))))));}
break;}case 18: _LL239: {
# 1270
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1272
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1268 "parse.y"
yyval=yyyyvsp[0];
break;}case 19: _LL23A: {
# 1271
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1273
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1269 "parse.y"
yyval=Cyc_YY16(0);
break;}case 22: _LL23B: {
# 1272
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1274
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1278 "parse.y"
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,0,Cyc_yyget_YY27(yyyyvsp[0]),0,Cyc_yyget_Stmt_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 23: _LL23C: {
# 1281
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1283
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1280 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4AC=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,& _tmp4AC,Cyc_yyget_YY27(yyyyvsp[1]),0,Cyc_yyget_Stmt_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 24: _LL23D: {
# 1284
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1286
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1293 "parse.y"
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,0,Cyc_yyget_YY27(yyyyvsp[0]),Cyc_yyget_YY16(yyyyvsp[1]),Cyc_yyget_Stmt_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 25: _LL23E: {
# 1296
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1298
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1295 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4AD=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,& _tmp4AD,Cyc_yyget_YY27(yyyyvsp[1]),Cyc_yyget_YY16(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 26: _LL23F: {
# 1299
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1301
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1303 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4AE=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,& _tmp4AE,Cyc_yyget_YY27(yyyyvsp[1]),0,Cyc_yyget_Stmt_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 27: _LL240: {
# 1307
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1309
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1306 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4AF=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,& _tmp4AF,Cyc_yyget_YY27(yyyyvsp[1]),Cyc_yyget_YY16(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 28: _LL241: {
# 1310
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1312
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1311 "parse.y"
Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyyyvsp[1]));yyval=yyyyvsp[1];
break;}case 29: _LL242: {
# 1314
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1316
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1314 "parse.y"
Cyc_Lex_leave_using();
break;}case 30: _LL243: {
# 1317
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1319
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1317 "parse.y"
{struct _dyneither_ptr*_tmpA83;Cyc_Lex_enter_namespace(((_tmpA83=_cycalloc(sizeof(*_tmpA83)),((_tmpA83[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpA83)))));}yyval=yyyyvsp[1];
break;}case 31: _LL244: {
# 1320
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1322
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1320 "parse.y"
Cyc_Lex_leave_namespace();
break;}case 32: _LL245: {
# 1323
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1325
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
int _tmp4B1=(yyyylsp[0]).first_line;
yyval=Cyc_YY16(Cyc_Parse_make_declarations(Cyc_yyget_YY17(yyyyvsp[0]),0,(unsigned int)_tmp4B1,(unsigned int)_tmp4B1));
break;}case 33: _LL246: {
# 1330
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1332
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1329 "parse.y"
int _tmp4B2=(yyyylsp[0]).first_line;
yyval=Cyc_YY16(Cyc_Parse_make_declarations(Cyc_yyget_YY17(yyyyvsp[0]),Cyc_yyget_YY19(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line,(unsigned int)_tmp4B2));
break;}case 34: _LL247: {
# 1333
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1335
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1333 "parse.y"
{struct Cyc_List_List*_tmpA84;yyval=Cyc_YY16(((_tmpA84=_cycalloc(sizeof(*_tmpA84)),((_tmpA84->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY9(yyyyvsp[1]),Cyc_yyget_Exp_tok(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line),((_tmpA84->tl=0,_tmpA84)))))));}
break;}case 35: _LL248: {
# 1336
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1338
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1335 "parse.y"
struct Cyc_List_List*_tmp4B4=0;
{struct Cyc_List_List*_tmp4B5=Cyc_yyget_YY36(yyyyvsp[1]);for(0;_tmp4B5 != 0;_tmp4B5=_tmp4B5->tl){
struct _dyneither_ptr*_tmp4B6=(struct _dyneither_ptr*)_tmp4B5->hd;
struct _tuple0*_tmpA85;struct _tuple0*qv=(_tmpA85=_cycalloc(sizeof(*_tmpA85)),((_tmpA85->f1=Cyc_Absyn_Rel_n(0),((_tmpA85->f2=_tmp4B6,_tmpA85)))));
struct Cyc_Absyn_Vardecl*_tmp4B7=Cyc_Absyn_new_vardecl(qv,Cyc_Absyn_wildtyp(0),0);
struct Cyc_List_List*_tmpA86;_tmp4B4=((_tmpA86=_cycalloc(sizeof(*_tmpA86)),((_tmpA86->hd=_tmp4B7,((_tmpA86->tl=_tmp4B4,_tmpA86))))));}}
# 1342
_tmp4B4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4B4);
{struct Cyc_List_List*_tmpA87;yyval=Cyc_YY16(((_tmpA87=_cycalloc(sizeof(*_tmpA87)),((_tmpA87->hd=Cyc_Absyn_letv_decl(_tmp4B4,(unsigned int)(yyyylsp[0]).first_line),((_tmpA87->tl=0,_tmpA87)))))));}
# 1345
break;}case 36: _LL249: {
# 1347
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 1349
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 1348 "parse.y"
struct _dyneither_ptr _tmp4BB=Cyc_yyget_String_tok(yyyyvsp[3]);
# 1350
{const char*_tmpA88;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4BB,((_tmpA88="`H",_tag_dyneither(_tmpA88,sizeof(char),3))))== 0){
const char*_tmpA8B;void*_tmpA8A;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA8A=0,Cyc_aprintf(((_tmpA8B="bad occurrence of heap region",_tag_dyneither(_tmpA8B,sizeof(char),30))),_tag_dyneither(_tmpA8A,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}
{const char*_tmpA8C;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4BB,((_tmpA8C="`U",_tag_dyneither(_tmpA8C,sizeof(char),3))))== 0){
const char*_tmpA8F;void*_tmpA8E;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA8E=0,Cyc_aprintf(((_tmpA8F="bad occurrence of unique region",_tag_dyneither(_tmpA8F,sizeof(char),32))),_tag_dyneither(_tmpA8E,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}{
struct _dyneither_ptr*_tmpA92;struct Cyc_Absyn_Tvar*_tmpA91;struct Cyc_Absyn_Tvar*tv=(_tmpA91=_cycalloc(sizeof(*_tmpA91)),((_tmpA91->name=((_tmpA92=_cycalloc(sizeof(*_tmpA92)),((_tmpA92[0]=_tmp4BB,_tmpA92)))),((_tmpA91->identity=- 1,((_tmpA91->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA91)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpA95;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA94;void*t=(void*)((_tmpA94=_cycalloc(sizeof(*_tmpA94)),((_tmpA94[0]=((_tmpA95.tag=2,((_tmpA95.f1=tv,_tmpA95)))),_tmpA94))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpA9B;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA9A;struct _dyneither_ptr*_tmpA98;struct _tuple0*_tmpA97;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpA97=_cycalloc(sizeof(*_tmpA97)),((_tmpA97->f1=Cyc_Absyn_Loc_n,((_tmpA97->f2=((_tmpA98=_cycalloc(sizeof(*_tmpA98)),((_tmpA98[0]=Cyc_yyget_String_tok(yyyyvsp[5]),_tmpA98)))),_tmpA97)))))),(void*)((_tmpA9A=_cycalloc(sizeof(*_tmpA9A)),((_tmpA9A[0]=((_tmpA9B.tag=15,((_tmpA9B.f1=t,_tmpA9B)))),_tmpA9A)))),0);
{struct Cyc_List_List*_tmpA9C;yyval=Cyc_YY16(((_tmpA9C=_cycalloc(sizeof(*_tmpA9C)),((_tmpA9C->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY31(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line),((_tmpA9C->tl=0,_tmpA9C)))))));}
# 1359
break;};}case 37: _LL24A: {
# 1361
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1363
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1361 "parse.y"
int _tmp4CB=Cyc_yyget_YY31(yyyyvsp[0]);
struct _dyneither_ptr _tmp4CC=Cyc_yyget_String_tok(yyyyvsp[2]);
{const char*_tmpA9D;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4CC,((_tmpA9D="H",_tag_dyneither(_tmpA9D,sizeof(char),2))))== 0){
const char*_tmpA9E;Cyc_Parse_err(((_tmpA9E="bad occurrence of heap region `H",_tag_dyneither(_tmpA9E,sizeof(char),33))),(unsigned int)(yyyylsp[2]).first_line);}}
{const char*_tmpA9F;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4CC,((_tmpA9F="U",_tag_dyneither(_tmpA9F,sizeof(char),2))))== 0){
const char*_tmpAA0;Cyc_Parse_err(((_tmpAA0="bad occurrence of unique region `U",_tag_dyneither(_tmpAA0,sizeof(char),35))),(unsigned int)(yyyylsp[2]).first_line);}}{
struct _dyneither_ptr*_tmpAAD;const char*_tmpAAC;void*_tmpAAB[1];struct Cyc_String_pa_PrintArg_struct _tmpAAA;struct Cyc_Absyn_Tvar*_tmpAA9;struct Cyc_Absyn_Tvar*tv=(_tmpAA9=_cycalloc(sizeof(*_tmpAA9)),((_tmpAA9->name=((_tmpAAD=_cycalloc(sizeof(*_tmpAAD)),((_tmpAAD[0]=(struct _dyneither_ptr)((_tmpAAA.tag=0,((_tmpAAA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4CC),((_tmpAAB[0]=& _tmpAAA,Cyc_aprintf(((_tmpAAC="`%s",_tag_dyneither(_tmpAAC,sizeof(char),4))),_tag_dyneither(_tmpAAB,sizeof(void*),1)))))))),_tmpAAD)))),((_tmpAA9->identity=- 1,((_tmpAA9->kind=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpAA9)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpAB0;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpAAF;void*t=(void*)((_tmpAAF=_cycalloc(sizeof(*_tmpAAF)),((_tmpAAF[0]=((_tmpAB0.tag=2,((_tmpAB0.f1=tv,_tmpAB0)))),_tmpAAF))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpAB6;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpAB5;struct _dyneither_ptr*_tmpAB3;struct _tuple0*_tmpAB2;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpAB2=_cycalloc(sizeof(*_tmpAB2)),((_tmpAB2->f1=Cyc_Absyn_Loc_n,((_tmpAB2->f2=((_tmpAB3=_cycalloc(sizeof(*_tmpAB3)),((_tmpAB3[0]=_tmp4CC,_tmpAB3)))),_tmpAB2)))))),(void*)((_tmpAB5=_cycalloc(sizeof(*_tmpAB5)),((_tmpAB5[0]=((_tmpAB6.tag=15,((_tmpAB6.f1=t,_tmpAB6)))),_tmpAB5)))),0);
# 1372
{struct Cyc_List_List*_tmpAB7;yyval=Cyc_YY16(((_tmpAB7=_cycalloc(sizeof(*_tmpAB7)),((_tmpAB7->hd=Cyc_Absyn_region_decl(tv,vd,_tmp4CB,0,(unsigned int)(yyyylsp[0]).first_line),((_tmpAB7->tl=0,_tmpAB7)))))));}
# 1374
break;};}case 38: _LL24B: {
# 1376
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 1378
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 1376 "parse.y"
struct _dyneither_ptr _tmp4DD=Cyc_yyget_String_tok(yyyyvsp[2]);
struct _dyneither_ptr _tmp4DE=Cyc_yyget_String_tok(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp4DF=Cyc_yyget_Exp_tok(yyyyvsp[6]);
{const char*_tmpAB8;if(Cyc_strcmp((struct _dyneither_ptr)_tmp4DE,((_tmpAB8="open",_tag_dyneither(_tmpAB8,sizeof(char),5))))!= 0){const char*_tmpAB9;Cyc_Parse_err(((_tmpAB9="expecting `open'",_tag_dyneither(_tmpAB9,sizeof(char),17))),(unsigned int)(yyyylsp[3]).first_line);}}{
struct _dyneither_ptr*_tmpAC6;const char*_tmpAC5;void*_tmpAC4[1];struct Cyc_String_pa_PrintArg_struct _tmpAC3;struct Cyc_Absyn_Tvar*_tmpAC2;struct Cyc_Absyn_Tvar*tv=(_tmpAC2=_cycalloc(sizeof(*_tmpAC2)),((_tmpAC2->name=((_tmpAC6=_cycalloc(sizeof(*_tmpAC6)),((_tmpAC6[0]=(struct _dyneither_ptr)((_tmpAC3.tag=0,((_tmpAC3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4DD),((_tmpAC4[0]=& _tmpAC3,Cyc_aprintf(((_tmpAC5="`%s",_tag_dyneither(_tmpAC5,sizeof(char),4))),_tag_dyneither(_tmpAC4,sizeof(void*),1)))))))),_tmpAC6)))),((_tmpAC2->identity=- 1,((_tmpAC2->kind=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpAC2)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpAC9;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpAC8;void*t=(void*)((_tmpAC8=_cycalloc(sizeof(*_tmpAC8)),((_tmpAC8[0]=((_tmpAC9.tag=2,((_tmpAC9.f1=tv,_tmpAC9)))),_tmpAC8))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpACF;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpACE;struct _dyneither_ptr*_tmpACC;struct _tuple0*_tmpACB;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpACB=_cycalloc(sizeof(*_tmpACB)),((_tmpACB->f1=Cyc_Absyn_Loc_n,((_tmpACB->f2=((_tmpACC=_cycalloc(sizeof(*_tmpACC)),((_tmpACC[0]=_tmp4DD,_tmpACC)))),_tmpACB)))))),(void*)((_tmpACE=_cycalloc(sizeof(*_tmpACE)),((_tmpACE[0]=((_tmpACF.tag=15,((_tmpACF.f1=t,_tmpACF)))),_tmpACE)))),0);
# 1385
{struct Cyc_List_List*_tmpAD0;yyval=Cyc_YY16(((_tmpAD0=_cycalloc(sizeof(*_tmpAD0)),((_tmpAD0->hd=Cyc_Absyn_region_decl(tv,vd,0,_tmp4DF,(unsigned int)(yyyylsp[0]).first_line),((_tmpAD0->tl=0,_tmpAD0)))))));}
# 1387
break;};}case 39: _LL24C:
# 1389
 yyval=Cyc_YY31(0);
break;case 40: _LL24D: {
# 1392
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1394
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1391 "parse.y"
yyval=Cyc_YY31(1);
break;}case 41: _LL24E: {
# 1394
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1396
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1396 "parse.y"
yyval=yyyyvsp[0];
break;}case 42: _LL24F: {
# 1399
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1401
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1398 "parse.y"
yyval=Cyc_YY16(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY16(yyyyvsp[0]),Cyc_yyget_YY16(yyyyvsp[1])));
break;}case 43: _LL250: {
# 1401
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1403
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_Parse_Declaration_spec _tmpAD1;yyval=Cyc_YY17(((_tmpAD1.sc=Cyc_yyget_YY20(yyyyvsp[0]),((_tmpAD1.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAD1.type_specs=
Cyc_Parse_empty_spec(0),((_tmpAD1.is_inline=0,((_tmpAD1.attributes=0,_tmpAD1)))))))))));}
break;}case 44: _LL251: {
# 1408
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1410
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1407 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4EF=Cyc_yyget_YY17(yyyyvsp[1]);
if(_tmp4EF.sc != 0){
const char*_tmpAD4;void*_tmpAD3;(_tmpAD3=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpAD4="Only one storage class is allowed in a declaration (missing ';' or ','?)",_tag_dyneither(_tmpAD4,sizeof(char),73))),_tag_dyneither(_tmpAD3,sizeof(void*),0)));}
# 1411
{struct Cyc_Parse_Declaration_spec _tmpAD5;yyval=Cyc_YY17(((_tmpAD5.sc=Cyc_yyget_YY20(yyyyvsp[0]),((_tmpAD5.tq=_tmp4EF.tq,((_tmpAD5.type_specs=_tmp4EF.type_specs,((_tmpAD5.is_inline=_tmp4EF.is_inline,((_tmpAD5.attributes=_tmp4EF.attributes,_tmpAD5)))))))))));}
# 1415
break;}case 45: _LL252: {
# 1417
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1419
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1416 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpAD6;yyval=Cyc_YY17(((_tmpAD6.sc=0,((_tmpAD6.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAD6.type_specs=
Cyc_yyget_YY21(yyyyvsp[0]),((_tmpAD6.is_inline=0,((_tmpAD6.attributes=0,_tmpAD6)))))))))));}
break;}case 46: _LL253: {
# 1420
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1422
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1419 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4F4=Cyc_yyget_YY17(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpAD7;yyval=Cyc_YY17(((_tmpAD7.sc=_tmp4F4.sc,((_tmpAD7.tq=_tmp4F4.tq,((_tmpAD7.type_specs=
Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,_tmp4F4.type_specs,Cyc_yyget_YY21(yyyyvsp[0])),((_tmpAD7.is_inline=_tmp4F4.is_inline,((_tmpAD7.attributes=_tmp4F4.attributes,_tmpAD7)))))))))));}
# 1425
break;}case 47: _LL254: {
# 1427
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1429
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1426 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpAD8;yyval=Cyc_YY17(((_tmpAD8.sc=0,((_tmpAD8.tq=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpAD8.type_specs=Cyc_Parse_empty_spec(0),((_tmpAD8.is_inline=0,((_tmpAD8.attributes=0,_tmpAD8)))))))))));}
break;}case 48: _LL255: {
# 1429
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1431
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1428 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4F7=Cyc_yyget_YY17(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpAD9;yyval=Cyc_YY17(((_tmpAD9.sc=_tmp4F7.sc,((_tmpAD9.tq=Cyc_Absyn_combine_tqual(Cyc_yyget_YY23(yyyyvsp[0]),_tmp4F7.tq),((_tmpAD9.type_specs=_tmp4F7.type_specs,((_tmpAD9.is_inline=_tmp4F7.is_inline,((_tmpAD9.attributes=_tmp4F7.attributes,_tmpAD9)))))))))));}
# 1433
break;}case 49: _LL256: {
# 1435
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1437
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1434 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpADA;yyval=Cyc_YY17(((_tmpADA.sc=0,((_tmpADA.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpADA.type_specs=
Cyc_Parse_empty_spec(0),((_tmpADA.is_inline=1,((_tmpADA.attributes=0,_tmpADA)))))))))));}
break;}case 50: _LL257: {
# 1438
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1440
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1437 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4FA=Cyc_yyget_YY17(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpADB;yyval=Cyc_YY17(((_tmpADB.sc=_tmp4FA.sc,((_tmpADB.tq=_tmp4FA.tq,((_tmpADB.type_specs=_tmp4FA.type_specs,((_tmpADB.is_inline=1,((_tmpADB.attributes=_tmp4FA.attributes,_tmpADB)))))))))));}
# 1441
break;}case 51: _LL258: {
# 1443
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1445
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1442 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpADC;yyval=Cyc_YY17(((_tmpADC.sc=0,((_tmpADC.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpADC.type_specs=
Cyc_Parse_empty_spec(0),((_tmpADC.is_inline=0,((_tmpADC.attributes=Cyc_yyget_YY45(yyyyvsp[0]),_tmpADC)))))))))));}
break;}case 52: _LL259: {
# 1446
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1448
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1445 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4FD=Cyc_yyget_YY17(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpADD;yyval=Cyc_YY17(((_tmpADD.sc=_tmp4FD.sc,((_tmpADD.tq=_tmp4FD.tq,((_tmpADD.type_specs=_tmp4FD.type_specs,((_tmpADD.is_inline=_tmp4FD.is_inline,((_tmpADD.attributes=
# 1448
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY45(yyyyvsp[0]),_tmp4FD.attributes),_tmpADD)))))))))));}
break;}case 53: _LL25A: {
# 1451
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1453
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1452 "parse.y"
static enum Cyc_Parse_Storage_class auto_sc=Cyc_Parse_Auto_sc;
yyval=Cyc_YY20(& auto_sc);
break;}case 54: _LL25B: {
# 1456
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1458
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1454 "parse.y"
static enum Cyc_Parse_Storage_class register_sc=Cyc_Parse_Register_sc;
yyval=Cyc_YY20(& register_sc);
break;}case 55: _LL25C: {
# 1458
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1460
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1456 "parse.y"
static enum Cyc_Parse_Storage_class static_sc=Cyc_Parse_Static_sc;
yyval=Cyc_YY20(& static_sc);
break;}case 56: _LL25D: {
# 1460
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1462
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1458 "parse.y"
static enum Cyc_Parse_Storage_class extern_sc=Cyc_Parse_Extern_sc;
yyval=Cyc_YY20(& extern_sc);
break;}case 57: _LL25E: {
# 1462
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1464
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1461 "parse.y"
static enum Cyc_Parse_Storage_class externC_sc=Cyc_Parse_ExternC_sc;
{const char*_tmpADE;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpADE="C",_tag_dyneither(_tmpADE,sizeof(char),2))))!= 0){
const char*_tmpADF;Cyc_Parse_err(((_tmpADF="only extern or extern \"C\" is allowed",_tag_dyneither(_tmpADF,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY20(& externC_sc);
# 1466
break;}case 58: _LL25F: {
# 1468
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1470
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1466 "parse.y"
static enum Cyc_Parse_Storage_class typedef_sc=Cyc_Parse_Typedef_sc;
yyval=Cyc_YY20(& typedef_sc);
break;}case 59: _LL260: {
# 1470
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1472
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1469 "parse.y"
static enum Cyc_Parse_Storage_class abstract_sc=Cyc_Parse_Abstract_sc;
yyval=Cyc_YY20(& abstract_sc);
break;}case 60: _LL261:
# 1473
 yyval=Cyc_YY45(0);
break;case 61: _LL262: {
# 1476
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1478
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1476 "parse.y"
yyval=yyyyvsp[0];
break;}case 62: _LL263: {
# 1479
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1481
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1481 "parse.y"
yyval=yyyyvsp[3];
break;}case 63: _LL264: {
# 1484
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1486
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1485 "parse.y"
{struct Cyc_List_List*_tmpAE0;yyval=Cyc_YY45(((_tmpAE0=_cycalloc(sizeof(*_tmpAE0)),((_tmpAE0->hd=Cyc_yyget_YY46(yyyyvsp[0]),((_tmpAE0->tl=0,_tmpAE0)))))));}
break;}case 64: _LL265: {
# 1488
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1490
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1486 "parse.y"
{struct Cyc_List_List*_tmpAE1;yyval=Cyc_YY45(((_tmpAE1=_cycalloc(sizeof(*_tmpAE1)),((_tmpAE1->hd=Cyc_yyget_YY46(yyyyvsp[0]),((_tmpAE1->tl=Cyc_yyget_YY45(yyyyvsp[2]),_tmpAE1)))))));}
break;}case 65: _LL266: {
# 1489
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1491
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1491 "parse.y"
static struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct att_aligned={6,- 1};
static struct _tuple28 att_map[17]={{{_tmp505,_tmp505,_tmp505 + 8},(void*)& Cyc_Absyn_Stdcall_att_val},{{_tmp506,_tmp506,_tmp506 + 6},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp507,_tmp507,_tmp507 + 9},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp508,_tmp508,_tmp508 + 9},(void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp509,_tmp509,_tmp509 + 6},(void*)& Cyc_Absyn_Const_att_val},{{_tmp50A,_tmp50A,_tmp50A + 8},(void*)& att_aligned},{{_tmp50B,_tmp50B,_tmp50B + 7},(void*)& Cyc_Absyn_Packed_att_val},{{_tmp50C,_tmp50C,_tmp50C + 7},(void*)& Cyc_Absyn_Shared_att_val},{{_tmp50D,_tmp50D,_tmp50D + 7},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp50E,_tmp50E,_tmp50E + 5},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp50F,_tmp50F,_tmp50F + 10},(void*)& Cyc_Absyn_Dllimport_att_val},{{_tmp510,_tmp510,_tmp510 + 10},(void*)& Cyc_Absyn_Dllexport_att_val},{{_tmp511,_tmp511,_tmp511 + 23},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{_tmp512,_tmp512,_tmp512 + 12},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp513,_tmp513,_tmp513 + 11},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp514,_tmp514,_tmp514 + 22},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp515,_tmp515,_tmp515 + 5},(void*)& Cyc_Absyn_Pure_att_val}};
# 1511
struct _dyneither_ptr _tmp503=Cyc_yyget_String_tok(yyyyvsp[0]);
# 1513
if((((_get_dyneither_size(_tmp503,sizeof(char))> 4  && ((const char*)_tmp503.curr)[0]== '_') && ((const char*)_tmp503.curr)[1]== '_') && *((const char*)_check_dyneither_subscript(_tmp503,sizeof(char),(int)(_get_dyneither_size(_tmp503,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(_tmp503,sizeof(char),(int)(_get_dyneither_size(_tmp503,sizeof(char))- 3)))== '_')
# 1515
_tmp503=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp503,2,_get_dyneither_size(_tmp503,sizeof(char))- 5);{
int i=0;
for(0;i < 17;++ i){
if(Cyc_strcmp((struct _dyneither_ptr)_tmp503,(struct _dyneither_ptr)(att_map[_check_known_subscript_notnull(17,i)]).f1)== 0){
yyval=Cyc_YY46((att_map[_check_known_subscript_notnull(17,i)]).f2);
break;}}
# 1522
if(i == 17){
{const char*_tmpAE2;Cyc_Parse_err(((_tmpAE2="unrecognized attribute",_tag_dyneither(_tmpAE2,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
yyval=Cyc_YY46((void*)& Cyc_Absyn_Cdecl_att_val);}
# 1527
break;};}case 66: _LL267: {
# 1529
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1531
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1527 "parse.y"
yyval=Cyc_YY46((void*)& Cyc_Absyn_Const_att_val);
break;}case 67: _LL268: {
# 1530
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1532
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1529 "parse.y"
struct _dyneither_ptr _tmp517=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _tuple5 _tmp518=Cyc_yyget_Int_tok(yyyyvsp[2]);int _tmp51A;struct _tuple5 _tmp519=_tmp518;_tmp51A=_tmp519.f2;{
void*a;
{const char*_tmpAE4;const char*_tmpAE3;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp517,((_tmpAE4="regparm",_tag_dyneither(_tmpAE4,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp517,((_tmpAE3="__regparm__",_tag_dyneither(_tmpAE3,sizeof(char),12))))== 0){
if(_tmp51A < 0  || _tmp51A > 3){
const char*_tmpAE5;Cyc_Parse_err(((_tmpAE5="regparm requires value between 0 and 3",_tag_dyneither(_tmpAE5,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct _tmpAE8;struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmpAE7;a=(void*)((_tmpAE7=_cycalloc_atomic(sizeof(*_tmpAE7)),((_tmpAE7[0]=((_tmpAE8.tag=0,((_tmpAE8.f1=_tmp51A,_tmpAE8)))),_tmpAE7))));};}else{
const char*_tmpAEA;const char*_tmpAE9;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp517,((_tmpAEA="aligned",_tag_dyneither(_tmpAEA,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp517,((_tmpAE9="__aligned__",_tag_dyneither(_tmpAE9,sizeof(char),12))))== 0){
if(_tmp51A < 0){const char*_tmpAEB;Cyc_Parse_err(((_tmpAEB="aligned requires positive power of two",_tag_dyneither(_tmpAEB,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
unsigned int j=(unsigned int)_tmp51A;
for(0;(j & 1)== 0;j=j >> 1){
;}
j=j >> 1;
if(j != 0){const char*_tmpAEC;Cyc_Parse_err(((_tmpAEC="aligned requires positive power of two",_tag_dyneither(_tmpAEC,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpAEF;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpAEE;a=(void*)((_tmpAEE=_cycalloc_atomic(sizeof(*_tmpAEE)),((_tmpAEE[0]=((_tmpAEF.tag=6,((_tmpAEF.f1=_tmp51A,_tmpAEF)))),_tmpAEE))));};};}else{
const char*_tmpAF1;const char*_tmpAF0;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp517,((_tmpAF1="initializes",_tag_dyneither(_tmpAF1,sizeof(char),12))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp517,((_tmpAF0="__initializes__",_tag_dyneither(_tmpAF0,sizeof(char),16))))== 0){
struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct _tmpAF4;struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmpAF3;a=(void*)((_tmpAF3=_cycalloc_atomic(sizeof(*_tmpAF3)),((_tmpAF3[0]=((_tmpAF4.tag=20,((_tmpAF4.f1=_tmp51A,_tmpAF4)))),_tmpAF3))));}else{
const char*_tmpAF6;const char*_tmpAF5;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp517,((_tmpAF6="noliveunique",_tag_dyneither(_tmpAF6,sizeof(char),13))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp517,((_tmpAF5="__noliveunique__",_tag_dyneither(_tmpAF5,sizeof(char),17))))== 0){
struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct _tmpAF9;struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmpAF8;a=(void*)((_tmpAF8=_cycalloc_atomic(sizeof(*_tmpAF8)),((_tmpAF8[0]=((_tmpAF9.tag=21,((_tmpAF9.f1=_tmp51A,_tmpAF9)))),_tmpAF8))));}else{
const char*_tmpAFB;const char*_tmpAFA;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp517,((_tmpAFB="noconsume",_tag_dyneither(_tmpAFB,sizeof(char),10))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp517,((_tmpAFA="__noconsume__",_tag_dyneither(_tmpAFA,sizeof(char),14))))== 0){
struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct _tmpAFE;struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmpAFD;a=(void*)((_tmpAFD=_cycalloc_atomic(sizeof(*_tmpAFD)),((_tmpAFD[0]=((_tmpAFE.tag=22,((_tmpAFE.f1=_tmp51A,_tmpAFE)))),_tmpAFD))));}else{
# 1551
{const char*_tmpAFF;Cyc_Parse_err(((_tmpAFF="unrecognized attribute",_tag_dyneither(_tmpAFF,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}}}}
# 1554
yyval=Cyc_YY46(a);
# 1556
break;};}case 68: _LL269: {
# 1558
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1560
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1557 "parse.y"
struct _dyneither_ptr _tmp533=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp534=Cyc_yyget_String_tok(yyyyvsp[2]);
void*a;
{const char*_tmpB01;const char*_tmpB00;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp533,((_tmpB01="section",_tag_dyneither(_tmpB01,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp533,((_tmpB00="__section__",_tag_dyneither(_tmpB00,sizeof(char),12))))== 0){
struct Cyc_Absyn_Section_att_Absyn_Attribute_struct _tmpB04;struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmpB03;a=(void*)((_tmpB03=_cycalloc(sizeof(*_tmpB03)),((_tmpB03[0]=((_tmpB04.tag=8,((_tmpB04.f1=_tmp534,_tmpB04)))),_tmpB03))));}else{
# 1563
{const char*_tmpB05;Cyc_Parse_err(((_tmpB05="unrecognized attribute",_tag_dyneither(_tmpB05,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}
# 1566
yyval=Cyc_YY46(a);
# 1568
break;}case 69: _LL26A: {
# 1570
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1572
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1569 "parse.y"
struct _dyneither_ptr _tmp53A=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp53B=Cyc_yyget_String_tok(yyyyvsp[2]);
void*a;
# 1573
{const char*_tmpB06;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp53A,((_tmpB06="__mode__",_tag_dyneither(_tmpB06,sizeof(char),9))))== 0){
struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct _tmpB09;struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmpB08;a=(void*)((_tmpB08=_cycalloc(sizeof(*_tmpB08)),((_tmpB08[0]=((_tmpB09.tag=24,((_tmpB09.f1=_tmp53B,_tmpB09)))),_tmpB08))));}else{
# 1576
{const char*_tmpB0A;Cyc_Parse_err(((_tmpB0A="unrecognized attribute",_tag_dyneither(_tmpB0A,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}
# 1579
yyval=Cyc_YY46(a);
# 1581
break;}case 70: _LL26B: {
# 1583
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 1585
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 1582 "parse.y"
struct _dyneither_ptr _tmp540=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp541=Cyc_yyget_String_tok(yyyyvsp[2]);
struct _tuple5 _tmp542=Cyc_yyget_Int_tok(yyyyvsp[4]);int _tmp544;struct _tuple5 _tmp543=_tmp542;_tmp544=_tmp543.f2;{
struct _tuple5 _tmp545=Cyc_yyget_Int_tok(yyyyvsp[6]);int _tmp547;struct _tuple5 _tmp546=_tmp545;_tmp547=_tmp546.f2;{
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;
{const char*_tmpB0C;const char*_tmpB0B;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp540,((_tmpB0C="format",_tag_dyneither(_tmpB0C,sizeof(char),7))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp540,((_tmpB0B="__format__",_tag_dyneither(_tmpB0B,sizeof(char),11))))== 0){
const char*_tmpB0D;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp541,((_tmpB0D="printf",_tag_dyneither(_tmpB0D,sizeof(char),7))))== 0){
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpB10;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpB0F;a=(void*)((_tmpB0F=_cycalloc_atomic(sizeof(*_tmpB0F)),((_tmpB0F[0]=((_tmpB10.tag=19,((_tmpB10.f1=Cyc_Absyn_Printf_ft,((_tmpB10.f2=_tmp544,((_tmpB10.f3=_tmp547,_tmpB10)))))))),_tmpB0F))));}else{
const char*_tmpB11;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp541,((_tmpB11="scanf",_tag_dyneither(_tmpB11,sizeof(char),6))))== 0){
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpB14;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpB13;a=(void*)((_tmpB13=_cycalloc_atomic(sizeof(*_tmpB13)),((_tmpB13[0]=((_tmpB14.tag=19,((_tmpB14.f1=Cyc_Absyn_Scanf_ft,((_tmpB14.f2=_tmp544,((_tmpB14.f3=_tmp547,_tmpB14)))))))),_tmpB13))));}else{
# 1593
const char*_tmpB15;Cyc_Parse_err(((_tmpB15="unrecognized format type",_tag_dyneither(_tmpB15,sizeof(char),25))),(unsigned int)(yyyylsp[2]).first_line);}}}else{
# 1595
const char*_tmpB16;Cyc_Parse_err(((_tmpB16="unrecognized attribute",_tag_dyneither(_tmpB16,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY46(a);
# 1598
break;};};}case 71: _LL26C: {
# 1600
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1602
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1607 "parse.y"
yyval=yyyyvsp[0];
break;}case 72: _LL26D: {
# 1610
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1612
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1609 "parse.y"
{struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpB19;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpB18;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB18=_cycalloc(sizeof(*_tmpB18)),((_tmpB18[0]=((_tmpB19.tag=17,((_tmpB19.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB19.f2=Cyc_yyget_YY40(yyyyvsp[1]),((_tmpB19.f3=0,((_tmpB19.f4=0,_tmpB19)))))))))),_tmpB18)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 73: _LL26E: {
# 1612
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1614
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1613 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)& Cyc_Absyn_VoidType_val,(unsigned int)(yyyylsp[0]).first_line));
break;}case 74: _LL26F: {
# 1616
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1618
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1614 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 75: _LL270: {
# 1617
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1619
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1616 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY43(yyyyvsp[2])),0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 76: _LL271: {
# 1619
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1621
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1617 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_char_typ,(unsigned int)(yyyylsp[0]).first_line));
break;}case 77: _LL272: {
# 1620
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1622
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1618 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_short_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 78: _LL273: {
# 1621
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1623
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1619 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,(unsigned int)(yyyylsp[0]).first_line));
break;}case 79: _LL274: {
# 1622
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1624
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1620 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_long_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 80: _LL275: {
# 1623
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1625
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1621 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_float_typ(0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 81: _LL276: {
# 1624
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1626
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1622 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_float_typ(1),(unsigned int)(yyyylsp[0]).first_line));
break;}case 82: _LL277: {
# 1625
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1627
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1623 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_signed_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 83: _LL278: {
# 1626
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1628
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1624 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_unsigned_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 84: _LL279: {
# 1627
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1629
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1625 "parse.y"
yyval=yyyyvsp[0];
break;}case 85: _LL27A: {
# 1628
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1630
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1626 "parse.y"
yyval=yyyyvsp[0];
break;}case 86: _LL27B: {
# 1629
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1631
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1628 "parse.y"
yyval=yyyyvsp[0];
break;}case 87: _LL27C: {
# 1631
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1633
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1630 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_yyget_YY44(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 88: _LL27D: {
# 1633
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1635
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1633 "parse.y"
{struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpB1C;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpB1B;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB1B=_cycalloc(sizeof(*_tmpB1B)),((_tmpB1B[0]=((_tmpB1C.tag=10,((_tmpB1C.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[2]).first_line,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[2]))),_tmpB1C)))),_tmpB1B)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1636
break;}case 89: _LL27E: {
# 1638
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1640
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1637 "parse.y"
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpB1F;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpB1E;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB1E=_cycalloc(sizeof(*_tmpB1E)),((_tmpB1E[0]=((_tmpB1F.tag=15,((_tmpB1F.f1=Cyc_yyget_YY44(yyyyvsp[2]),_tmpB1F)))),_tmpB1E)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 90: _LL27F: {
# 1640
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1642
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1639 "parse.y"
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpB22;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpB21;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB21=_cycalloc(sizeof(*_tmpB21)),((_tmpB21[0]=((_tmpB22.tag=15,((_tmpB22.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0),_tmpB22)))),_tmpB21)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1641
break;}case 91: _LL280: {
# 1643
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1645
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1642 "parse.y"
void*_tmp55A=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0);
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpB25;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpB24;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB24=_cycalloc(sizeof(*_tmpB24)),((_tmpB24[0]=((_tmpB25.tag=16,((_tmpB25.f1=Cyc_yyget_YY44(yyyyvsp[2]),((_tmpB25.f2=_tmp55A,_tmpB25)))))),_tmpB24)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1645
break;}case 92: _LL281: {
# 1647
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1649
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1646 "parse.y"
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpB28;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpB27;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB27=_cycalloc(sizeof(*_tmpB27)),((_tmpB27[0]=((_tmpB28.tag=16,((_tmpB28.f1=Cyc_yyget_YY44(yyyyvsp[2]),((_tmpB28.f2=Cyc_yyget_YY44(yyyyvsp[4]),_tmpB28)))))),_tmpB27)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 93: _LL282: {
# 1649
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1651
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1648 "parse.y"
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpB2B;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpB2A;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB2A=_cycalloc(sizeof(*_tmpB2A)),((_tmpB2A[0]=((_tmpB2B.tag=19,((_tmpB2B.f1=Cyc_yyget_YY44(yyyyvsp[2]),_tmpB2B)))),_tmpB2A)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 94: _LL283: {
# 1651
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1653
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1650 "parse.y"
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpB2E;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpB2D;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB2D=_cycalloc(sizeof(*_tmpB2D)),((_tmpB2D[0]=((_tmpB2E.tag=19,((_tmpB2E.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_iko,0),_tmpB2E)))),_tmpB2D)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 95: _LL284: {
# 1653
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1655
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1652 "parse.y"
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpB31;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpB30;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB30=_cycalloc(sizeof(*_tmpB30)),((_tmpB30[0]=((_tmpB31.tag=18,((_tmpB31.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),_tmpB31)))),_tmpB30)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 96: _LL285: {
# 1655
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1657
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1657 "parse.y"
yyval=Cyc_YY43(Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 97: _LL286: {
# 1660
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1662
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1661 "parse.y"
unsigned int loc=(unsigned int)(Cyc_Absyn_porting_c_code?(yyyylsp[0]).first_line:(int)0);
{struct Cyc_Absyn_Tqual _tmpB32;yyval=Cyc_YY23(((_tmpB32.print_const=1,((_tmpB32.q_volatile=0,((_tmpB32.q_restrict=0,((_tmpB32.real_const=1,((_tmpB32.loc=loc,_tmpB32)))))))))));}
break;}case 98: _LL287: {
# 1665
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1667
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1663 "parse.y"
{struct Cyc_Absyn_Tqual _tmpB33;yyval=Cyc_YY23(((_tmpB33.print_const=0,((_tmpB33.q_volatile=1,((_tmpB33.q_restrict=0,((_tmpB33.real_const=0,((_tmpB33.loc=0,_tmpB33)))))))))));}
break;}case 99: _LL288: {
# 1666
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1668
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1664 "parse.y"
{struct Cyc_Absyn_Tqual _tmpB34;yyval=Cyc_YY23(((_tmpB34.print_const=0,((_tmpB34.q_volatile=0,((_tmpB34.q_restrict=1,((_tmpB34.real_const=0,((_tmpB34.loc=0,_tmpB34)))))))))));}
break;}case 100: _LL289: {
# 1667
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1669
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpB43;struct Cyc_Absyn_Enumdecl*_tmpB42;struct Cyc_Core_Opt*_tmpB41;struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct _tmpB40;struct Cyc_Absyn_TypeDecl*_tmpB3F;struct Cyc_Absyn_TypeDecl*_tmp568=(_tmpB3F=_cycalloc(sizeof(*_tmpB3F)),((_tmpB3F->r=(void*)((_tmpB43=_cycalloc(sizeof(*_tmpB43)),((_tmpB43[0]=((_tmpB40.tag=1,((_tmpB40.f1=((_tmpB42=_cycalloc(sizeof(*_tmpB42)),((_tmpB42->sc=Cyc_Absyn_Public,((_tmpB42->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB42->fields=((_tmpB41=_cycalloc(sizeof(*_tmpB41)),((_tmpB41->v=Cyc_yyget_YY48(yyyyvsp[3]),_tmpB41)))),_tmpB42)))))))),_tmpB40)))),_tmpB43)))),((_tmpB3F->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpB3F)))));
# 1672
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB46;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB45;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB45=_cycalloc(sizeof(*_tmpB45)),((_tmpB45[0]=((_tmpB46.tag=26,((_tmpB46.f1=_tmp568,((_tmpB46.f2=0,_tmpB46)))))),_tmpB45)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1674
break;}case 101: _LL28A: {
# 1676
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1678
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1675 "parse.y"
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpB49;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpB48;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB48=_cycalloc(sizeof(*_tmpB48)),((_tmpB48[0]=((_tmpB49.tag=13,((_tmpB49.f1=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB49.f2=0,_tmpB49)))))),_tmpB48)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 102: _LL28B: {
# 1678
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1680
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1677 "parse.y"
{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpB4C;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpB4B;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB4B=_cycalloc(sizeof(*_tmpB4B)),((_tmpB4B[0]=((_tmpB4C.tag=14,((_tmpB4C.f1=Cyc_yyget_YY48(yyyyvsp[2]),_tmpB4C)))),_tmpB4B)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 103: _LL28C: {
# 1680
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1682
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_Absyn_Enumfield*_tmpB4D;yyval=Cyc_YY47(((_tmpB4D=_cycalloc(sizeof(*_tmpB4D)),((_tmpB4D->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB4D->tag=0,((_tmpB4D->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpB4D)))))))));}
break;}case 104: _LL28D: {
# 1686
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1688
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1685 "parse.y"
{struct Cyc_Absyn_Enumfield*_tmpB4E;yyval=Cyc_YY47(((_tmpB4E=_cycalloc(sizeof(*_tmpB4E)),((_tmpB4E->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB4E->tag=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpB4E->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpB4E)))))))));}
break;}case 105: _LL28E: {
# 1688
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1690
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1689 "parse.y"
{struct Cyc_List_List*_tmpB4F;yyval=Cyc_YY48(((_tmpB4F=_cycalloc(sizeof(*_tmpB4F)),((_tmpB4F->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpB4F->tl=0,_tmpB4F)))))));}
break;}case 106: _LL28F: {
# 1692
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1694
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1690 "parse.y"
{struct Cyc_List_List*_tmpB50;yyval=Cyc_YY48(((_tmpB50=_cycalloc(sizeof(*_tmpB50)),((_tmpB50->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpB50->tl=0,_tmpB50)))))));}
break;}case 107: _LL290: {
# 1693
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1695
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1691 "parse.y"
{struct Cyc_List_List*_tmpB51;yyval=Cyc_YY48(((_tmpB51=_cycalloc(sizeof(*_tmpB51)),((_tmpB51->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpB51->tl=Cyc_yyget_YY48(yyyyvsp[2]),_tmpB51)))))));}
break;}case 108: _LL291: {
# 1694
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1696
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpB54;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpB53;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB53=_cycalloc(sizeof(*_tmpB53)),((_tmpB53[0]=((_tmpB54.tag=12,((_tmpB54.f1=Cyc_yyget_YY22(yyyyvsp[0]),((_tmpB54.f2=Cyc_yyget_YY24(yyyyvsp[2]),_tmpB54)))))),_tmpB53)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 109: _LL292: {
# 1700
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 1702
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 1703 "parse.y"
struct Cyc_List_List*_tmp57B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[3]).first_line,Cyc_yyget_YY40(yyyyvsp[3]));
struct Cyc_List_List*_tmp57C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[5]).first_line,Cyc_yyget_YY40(yyyyvsp[5]));
struct Cyc_Absyn_TypeDecl*_tmp57D=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY22(yyyyvsp[1]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[2]),_tmp57B,
Cyc_Absyn_aggrdecl_impl(_tmp57C,Cyc_yyget_YY50(yyyyvsp[6]),Cyc_yyget_YY24(yyyyvsp[7]),1),0,(unsigned int)(yyyylsp[0]).first_line);
# 1708
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB57;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB56;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB56=_cycalloc(sizeof(*_tmpB56)),((_tmpB56[0]=((_tmpB57.tag=26,((_tmpB57.f1=_tmp57D,((_tmpB57.f2=0,_tmpB57)))))),_tmpB56)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1710
break;}case 110: _LL293: {
# 1712
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 1714
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 1714 "parse.y"
struct Cyc_List_List*_tmp580=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY40(yyyyvsp[2]));
struct Cyc_List_List*_tmp581=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[4]).first_line,Cyc_yyget_YY40(yyyyvsp[4]));
struct Cyc_Absyn_TypeDecl*_tmp582=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY22(yyyyvsp[0]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp580,
Cyc_Absyn_aggrdecl_impl(_tmp581,Cyc_yyget_YY50(yyyyvsp[5]),Cyc_yyget_YY24(yyyyvsp[6]),0),0,(unsigned int)(yyyylsp[0]).first_line);
# 1719
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB5A;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB59;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB59=_cycalloc(sizeof(*_tmpB59)),((_tmpB59[0]=((_tmpB5A.tag=26,((_tmpB5A.f1=_tmp582,((_tmpB5A.f2=0,_tmpB5A)))))),_tmpB59)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1721
break;}case 111: _LL294: {
# 1723
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1725
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1722 "parse.y"
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB64;struct Cyc_Core_Opt*_tmpB63;struct Cyc_Absyn_AggrInfo _tmpB62;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB61;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB61=_cycalloc(sizeof(*_tmpB61)),((_tmpB61[0]=((_tmpB64.tag=11,((_tmpB64.f1=((_tmpB62.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY22(yyyyvsp[1]),Cyc_yyget_QualId_tok(yyyyvsp[2]),((_tmpB63=_cycalloc_atomic(sizeof(*_tmpB63)),((_tmpB63->v=(void*)1,_tmpB63))))),((_tmpB62.targs=Cyc_yyget_YY40(yyyyvsp[3]),_tmpB62)))),_tmpB64)))),_tmpB61)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1725
break;}case 112: _LL295: {
# 1727
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1729
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1726 "parse.y"
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB6A;struct Cyc_Absyn_AggrInfo _tmpB69;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB68;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB68=_cycalloc(sizeof(*_tmpB68)),((_tmpB68[0]=((_tmpB6A.tag=11,((_tmpB6A.f1=((_tmpB69.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY22(yyyyvsp[0]),Cyc_yyget_QualId_tok(yyyyvsp[1]),0),((_tmpB69.targs=Cyc_yyget_YY40(yyyyvsp[2]),_tmpB69)))),_tmpB6A)))),_tmpB68)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1729
break;}case 113: _LL296:
# 1731
 yyval=Cyc_YY40(0);
break;case 114: _LL297: {
# 1734
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1736
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1735 "parse.y"
yyval=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[1])));
break;}case 115: _LL298: {
# 1738
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1740
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1739 "parse.y"
yyval=Cyc_YY22(Cyc_Absyn_StructA);
break;}case 116: _LL299: {
# 1742
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1744
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1740 "parse.y"
yyval=Cyc_YY22(Cyc_Absyn_UnionA);
break;}case 117: _LL29A:
# 1743
 yyval=Cyc_YY24(0);
break;case 118: _LL29B: {
# 1746
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1748
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1747 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp58C=Cyc_yyget_YY25(yyyyvsp[0]);for(0;_tmp58C != 0;_tmp58C=_tmp58C->tl){
decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)((struct Cyc_List_List*)_tmp58C->hd,decls);}}{
# 1751
struct Cyc_List_List*_tmp58D=Cyc_Parse_get_argrfield_tags(decls);
if(_tmp58D != 0)
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,_tmp58D,decls);
yyval=Cyc_YY24(decls);
# 1756
break;};}case 119: _LL29C: {
# 1758
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1760
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpB6B;yyval=Cyc_YY25(((_tmpB6B=_cycalloc(sizeof(*_tmpB6B)),((_tmpB6B->hd=Cyc_yyget_YY24(yyyyvsp[0]),((_tmpB6B->tl=0,_tmpB6B)))))));}
break;}case 120: _LL29D: {
# 1764
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1766
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1763 "parse.y"
{struct Cyc_List_List*_tmpB6C;yyval=Cyc_YY25(((_tmpB6C=_cycalloc(sizeof(*_tmpB6C)),((_tmpB6C->hd=Cyc_yyget_YY24(yyyyvsp[1]),((_tmpB6C->tl=Cyc_yyget_YY25(yyyyvsp[0]),_tmpB6C)))))));}
break;}case 121: _LL29E: {
# 1766
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1768
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1767 "parse.y"
yyval=Cyc_YY19(((struct _tuple12*(*)(struct _tuple12*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY19(yyyyvsp[0])));
break;}case 122: _LL29F: {
# 1770
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1772
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct _tuple12*_tmpB6D;yyval=Cyc_YY19(((_tmpB6D=_region_malloc(yyr,sizeof(*_tmpB6D)),((_tmpB6D->tl=0,((_tmpB6D->hd=Cyc_yyget_YY18(yyyyvsp[0]),_tmpB6D)))))));}
break;}case 123: _LL2A0: {
# 1776
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1778
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1775 "parse.y"
{struct _tuple12*_tmpB6E;yyval=Cyc_YY19(((_tmpB6E=_region_malloc(yyr,sizeof(*_tmpB6E)),((_tmpB6E->tl=Cyc_yyget_YY19(yyyyvsp[0]),((_tmpB6E->hd=Cyc_yyget_YY18(yyyyvsp[2]),_tmpB6E)))))));}
break;}case 124: _LL2A1: {
# 1778
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1780
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1780 "parse.y"
{struct _tuple11 _tmpB6F;yyval=Cyc_YY18(((_tmpB6F.f1=Cyc_yyget_YY27(yyyyvsp[0]),((_tmpB6F.f2=0,_tmpB6F)))));}
break;}case 125: _LL2A2: {
# 1783
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1785
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1782 "parse.y"
{struct _tuple11 _tmpB70;yyval=Cyc_YY18(((_tmpB70.f1=Cyc_yyget_YY27(yyyyvsp[0]),((_tmpB70.f2=Cyc_yyget_Exp_tok(yyyyvsp[2]),_tmpB70)))));}
break;}case 126: _LL2A3: {
# 1785
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1787
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1788 "parse.y"
struct _RegionHandle _tmp594=_new_region("temp");struct _RegionHandle*temp=& _tmp594;_push_region(temp);{
struct _tuple25 _tmp595=Cyc_yyget_YY35(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp597;struct Cyc_Parse_Type_specifier _tmp598;struct Cyc_List_List*_tmp599;struct _tuple25 _tmp596=_tmp595;_tmp597=_tmp596.f1;_tmp598=_tmp596.f2;_tmp599=_tmp596.f3;
if(_tmp597.loc == 0)_tmp597.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct _tuple10*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*_tmp59A=Cyc_yyget_YY29(yyyyvsp[1]);for(0;_tmp59A != 0;_tmp59A=_tmp59A->tl){
struct _tuple24*_tmp59B=(struct _tuple24*)_tmp59A->hd;struct Cyc_Parse_Declarator _tmp59D;struct Cyc_Absyn_Exp*_tmp59E;struct Cyc_Absyn_Exp*_tmp59F;struct _tuple24*_tmp59C=_tmp59B;_tmp59D=_tmp59C->f1;_tmp59E=_tmp59C->f2;_tmp59F=_tmp59C->f3;
{struct _tuple10*_tmpB71;decls=((_tmpB71=_region_malloc(temp,sizeof(*_tmpB71)),((_tmpB71->tl=decls,((_tmpB71->hd=_tmp59D,_tmpB71))))));}{
struct _tuple16*_tmpB74;struct Cyc_List_List*_tmpB73;widths_and_reqs=(
(_tmpB73=_region_malloc(temp,sizeof(*_tmpB73)),((_tmpB73->hd=((_tmpB74=_region_malloc(temp,sizeof(*_tmpB74)),((_tmpB74->f1=_tmp59E,((_tmpB74->f2=_tmp59F,_tmpB74)))))),((_tmpB73->tl=widths_and_reqs,_tmpB73))))));};}}
# 1799
decls=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(decls);
widths_and_reqs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(widths_and_reqs);{
void*_tmp5A3=Cyc_Parse_speclist2typ(_tmp598,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp5A4=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,
Cyc_Parse_apply_tmss(temp,_tmp597,_tmp5A3,decls,_tmp599),widths_and_reqs);
# 1805
yyval=Cyc_YY24(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(unsigned int,struct _tuple17*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,(unsigned int)(yyyylsp[0]).first_line,_tmp5A4));};};}
# 1808
_npop_handler(0);break;
# 1788
;_pop_region(temp);}case 127: _LL2A4: {
# 1810
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1812
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1816 "parse.y"
{struct _tuple25 _tmpB75;yyval=Cyc_YY35(((_tmpB75.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB75.f2=Cyc_yyget_YY21(yyyyvsp[0]),((_tmpB75.f3=0,_tmpB75)))))));}
break;}case 128: _LL2A5: {
# 1819
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1821
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1818 "parse.y"
struct _tuple25 _tmp5A6=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple25 _tmpB76;yyval=Cyc_YY35(((_tmpB76.f1=_tmp5A6.f1,((_tmpB76.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY21(yyyyvsp[0]),_tmp5A6.f2),((_tmpB76.f3=_tmp5A6.f3,_tmpB76)))))));}
break;}case 129: _LL2A6: {
# 1821
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1823
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1820 "parse.y"
{struct _tuple25 _tmpB77;yyval=Cyc_YY35(((_tmpB77.f1=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpB77.f2=Cyc_Parse_empty_spec(0),((_tmpB77.f3=0,_tmpB77)))))));}
break;}case 130: _LL2A7: {
# 1823
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1825
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1822 "parse.y"
struct _tuple25 _tmp5A9=Cyc_yyget_YY35(yyyyvsp[1]);
{struct _tuple25 _tmpB78;yyval=Cyc_YY35(((_tmpB78.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY23(yyyyvsp[0]),_tmp5A9.f1),((_tmpB78.f2=_tmp5A9.f2,((_tmpB78.f3=_tmp5A9.f3,_tmpB78)))))));}
break;}case 131: _LL2A8: {
# 1826
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1828
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1825 "parse.y"
{struct _tuple25 _tmpB79;yyval=Cyc_YY35(((_tmpB79.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB79.f2=Cyc_Parse_empty_spec(0),((_tmpB79.f3=Cyc_yyget_YY45(yyyyvsp[0]),_tmpB79)))))));}
break;}case 132: _LL2A9: {
# 1828
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1830
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1827 "parse.y"
struct _tuple25 _tmp5AC=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple25 _tmpB7A;yyval=Cyc_YY35(((_tmpB7A.f1=_tmp5AC.f1,((_tmpB7A.f2=_tmp5AC.f2,((_tmpB7A.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY45(yyyyvsp[0]),_tmp5AC.f3),_tmpB7A)))))));}
break;}case 133: _LL2AA: {
# 1830
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1832
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct _tuple25 _tmpB7B;yyval=Cyc_YY35(((_tmpB7B.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB7B.f2=Cyc_yyget_YY21(yyyyvsp[0]),((_tmpB7B.f3=0,_tmpB7B)))))));}
break;}case 134: _LL2AB: {
# 1836
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1838
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1835 "parse.y"
struct _tuple25 _tmp5AF=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple25 _tmpB7C;yyval=Cyc_YY35(((_tmpB7C.f1=_tmp5AF.f1,((_tmpB7C.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY21(yyyyvsp[0]),_tmp5AF.f2),((_tmpB7C.f3=_tmp5AF.f3,_tmpB7C)))))));}
break;}case 135: _LL2AC: {
# 1838
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1840
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1837 "parse.y"
{struct _tuple25 _tmpB7D;yyval=Cyc_YY35(((_tmpB7D.f1=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpB7D.f2=Cyc_Parse_empty_spec(0),((_tmpB7D.f3=0,_tmpB7D)))))));}
break;}case 136: _LL2AD: {
# 1840
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1842
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1839 "parse.y"
struct _tuple25 _tmp5B2=Cyc_yyget_YY35(yyyyvsp[1]);
{struct _tuple25 _tmpB7E;yyval=Cyc_YY35(((_tmpB7E.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY23(yyyyvsp[0]),_tmp5B2.f1),((_tmpB7E.f2=_tmp5B2.f2,((_tmpB7E.f3=_tmp5B2.f3,_tmpB7E)))))));}
break;}case 137: _LL2AE: {
# 1843
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1845
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1842 "parse.y"
{struct _tuple25 _tmpB7F;yyval=Cyc_YY35(((_tmpB7F.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB7F.f2=Cyc_Parse_empty_spec(0),((_tmpB7F.f3=Cyc_yyget_YY45(yyyyvsp[0]),_tmpB7F)))))));}
break;}case 138: _LL2AF: {
# 1845
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1847
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1844 "parse.y"
struct _tuple25 _tmp5B5=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple25 _tmpB80;yyval=Cyc_YY35(((_tmpB80.f1=_tmp5B5.f1,((_tmpB80.f2=_tmp5B5.f2,((_tmpB80.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY45(yyyyvsp[0]),_tmp5B5.f3),_tmpB80)))))));}
break;}case 139: _LL2B0: {
# 1847
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1849
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1848 "parse.y"
yyval=Cyc_YY29(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY29(yyyyvsp[0])));
break;}case 140: _LL2B1: {
# 1851
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1853
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpB81;yyval=Cyc_YY29(((_tmpB81=_region_malloc(yyr,sizeof(*_tmpB81)),((_tmpB81->hd=Cyc_yyget_YY28(yyyyvsp[0]),((_tmpB81->tl=0,_tmpB81)))))));}
break;}case 141: _LL2B2: {
# 1857
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1859
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1856 "parse.y"
{struct Cyc_List_List*_tmpB82;yyval=Cyc_YY29(((_tmpB82=_region_malloc(yyr,sizeof(*_tmpB82)),((_tmpB82->hd=Cyc_yyget_YY28(yyyyvsp[2]),((_tmpB82->tl=Cyc_yyget_YY29(yyyyvsp[0]),_tmpB82)))))));}
break;}case 142: _LL2B3: {
# 1859
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1861
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1861 "parse.y"
{struct _tuple24*_tmpB83;yyval=Cyc_YY28(((_tmpB83=_region_malloc(yyr,sizeof(*_tmpB83)),((_tmpB83->f1=Cyc_yyget_YY27(yyyyvsp[0]),((_tmpB83->f2=0,((_tmpB83->f3=Cyc_yyget_YY55(yyyyvsp[1]),_tmpB83)))))))));}
break;}case 143: _LL2B4: {
# 1864
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1866
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1865 "parse.y"
{struct Cyc_Parse_Declarator _tmpB89;struct _tuple0*_tmpB88;struct _tuple24*_tmpB87;yyval=Cyc_YY28(((_tmpB87=_region_malloc(yyr,sizeof(*_tmpB87)),((_tmpB87->f1=((_tmpB89.id=((_tmpB88=_cycalloc(sizeof(*_tmpB88)),((_tmpB88->f1=Cyc_Absyn_Rel_n(0),((_tmpB88->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpB88)))))),((_tmpB89.tms=0,_tmpB89)))),((_tmpB87->f2=Cyc_yyget_Exp_tok(yyyyvsp[1]),((_tmpB87->f3=0,_tmpB87)))))))));}
# 1867
break;}case 144: _LL2B5:
# 1871
{struct Cyc_Parse_Declarator _tmpB8F;struct _tuple0*_tmpB8E;struct _tuple24*_tmpB8D;yyval=Cyc_YY28(((_tmpB8D=_region_malloc(yyr,sizeof(*_tmpB8D)),((_tmpB8D->f1=((_tmpB8F.id=((_tmpB8E=_cycalloc(sizeof(*_tmpB8E)),((_tmpB8E->f1=Cyc_Absyn_Rel_n(0),((_tmpB8E->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpB8E)))))),((_tmpB8F.tms=0,_tmpB8F)))),((_tmpB8D->f2=0,((_tmpB8D->f3=0,_tmpB8D)))))))));}
# 1873
break;case 145: _LL2B6: {
# 1875
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1877
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1873 "parse.y"
{struct _tuple24*_tmpB90;yyval=Cyc_YY28(((_tmpB90=_region_malloc(yyr,sizeof(*_tmpB90)),((_tmpB90->f1=Cyc_yyget_YY27(yyyyvsp[0]),((_tmpB90->f2=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpB90->f3=0,_tmpB90)))))))));}
break;}case 146: _LL2B7: {
# 1876
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1878
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1877 "parse.y"
yyval=Cyc_YY55(Cyc_yyget_Exp_tok(yyyyvsp[2]));
break;}case 147: _LL2B8:
# 1880
 yyval=Cyc_YY55(0);
break;case 148: _LL2B9: {
# 1883
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1885
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1882 "parse.y"
yyval=Cyc_YY55(Cyc_yyget_Exp_tok(yyyyvsp[2]));
break;}case 149: _LL2BA:
# 1885
 yyval=Cyc_YY55(0);
break;case 150: _LL2BB: {
# 1888
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1890
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1889 "parse.y"
int _tmp5C3=Cyc_yyget_YY31(yyyyvsp[0]);
struct Cyc_List_List*_tmp5C4=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY40(yyyyvsp[2]));
struct Cyc_Core_Opt*_tmpB91;struct Cyc_Absyn_TypeDecl*_tmp5C5=Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp5C4,((_tmpB91=_cycalloc(sizeof(*_tmpB91)),((_tmpB91->v=Cyc_yyget_YY34(yyyyvsp[4]),_tmpB91)))),_tmp5C3,(unsigned int)(yyyylsp[0]).first_line);
# 1893
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB94;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB93;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB93=_cycalloc(sizeof(*_tmpB93)),((_tmpB93[0]=((_tmpB94.tag=26,((_tmpB94.f1=_tmp5C5,((_tmpB94.f2=0,_tmpB94)))))),_tmpB93)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1895
break;}case 151: _LL2BC: {
# 1897
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1899
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1896 "parse.y"
int _tmp5C9=Cyc_yyget_YY31(yyyyvsp[0]);
{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpB9E;struct Cyc_Absyn_UnknownDatatypeInfo _tmpB9D;struct Cyc_Absyn_DatatypeInfo _tmpB9C;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpB9B;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB9B=_cycalloc(sizeof(*_tmpB9B)),((_tmpB9B[0]=((_tmpB9E.tag=3,((_tmpB9E.f1=((_tmpB9C.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpB9D.name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB9D.is_extensible=_tmp5C9,_tmpB9D))))),((_tmpB9C.targs=Cyc_yyget_YY40(yyyyvsp[2]),_tmpB9C)))),_tmpB9E)))),_tmpB9B)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1899
break;}case 152: _LL2BD: {
# 1901
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1903
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1900 "parse.y"
int _tmp5CE=Cyc_yyget_YY31(yyyyvsp[0]);
{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpBA8;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmpBA7;struct Cyc_Absyn_DatatypeFieldInfo _tmpBA6;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpBA5;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpBA5=_cycalloc(sizeof(*_tmpBA5)),((_tmpBA5[0]=((_tmpBA8.tag=4,((_tmpBA8.f1=((_tmpBA6.field_info=
Cyc_Absyn_UnknownDatatypefield(((_tmpBA7.datatype_name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpBA7.field_name=Cyc_yyget_QualId_tok(yyyyvsp[3]),((_tmpBA7.is_extensible=_tmp5CE,_tmpBA7))))))),((_tmpBA6.targs=Cyc_yyget_YY40(yyyyvsp[4]),_tmpBA6)))),_tmpBA8)))),_tmpBA5)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1905
break;}case 153: _LL2BE: {
# 1907
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1909
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1908 "parse.y"
yyval=Cyc_YY31(0);
break;}case 154: _LL2BF: {
# 1911
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1913
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1909 "parse.y"
yyval=Cyc_YY31(1);
break;}case 155: _LL2C0: {
# 1912
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1914
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1913 "parse.y"
{struct Cyc_List_List*_tmpBA9;yyval=Cyc_YY34(((_tmpBA9=_cycalloc(sizeof(*_tmpBA9)),((_tmpBA9->hd=Cyc_yyget_YY33(yyyyvsp[0]),((_tmpBA9->tl=0,_tmpBA9)))))));}
break;}case 156: _LL2C1: {
# 1916
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1918
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1914 "parse.y"
{struct Cyc_List_List*_tmpBAA;yyval=Cyc_YY34(((_tmpBAA=_cycalloc(sizeof(*_tmpBAA)),((_tmpBAA->hd=Cyc_yyget_YY33(yyyyvsp[0]),((_tmpBAA->tl=0,_tmpBAA)))))));}
break;}case 157: _LL2C2: {
# 1917
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1919
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1915 "parse.y"
{struct Cyc_List_List*_tmpBAB;yyval=Cyc_YY34(((_tmpBAB=_cycalloc(sizeof(*_tmpBAB)),((_tmpBAB->hd=Cyc_yyget_YY33(yyyyvsp[0]),((_tmpBAB->tl=Cyc_yyget_YY34(yyyyvsp[2]),_tmpBAB)))))));}
break;}case 158: _LL2C3: {
# 1918
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1920
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1916 "parse.y"
{struct Cyc_List_List*_tmpBAC;yyval=Cyc_YY34(((_tmpBAC=_cycalloc(sizeof(*_tmpBAC)),((_tmpBAC->hd=Cyc_yyget_YY33(yyyyvsp[0]),((_tmpBAC->tl=Cyc_yyget_YY34(yyyyvsp[2]),_tmpBAC)))))));}
break;}case 159: _LL2C4:
# 1919
 yyval=Cyc_YY32(Cyc_Absyn_Public);
break;case 160: _LL2C5: {
# 1922
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1924
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1921 "parse.y"
yyval=Cyc_YY32(Cyc_Absyn_Extern);
break;}case 161: _LL2C6: {
# 1924
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1926
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1922 "parse.y"
yyval=Cyc_YY32(Cyc_Absyn_Static);
break;}case 162: _LL2C7: {
# 1925
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1927
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1926 "parse.y"
{struct Cyc_Absyn_Datatypefield*_tmpBAD;yyval=Cyc_YY33(((_tmpBAD=_cycalloc(sizeof(*_tmpBAD)),((_tmpBAD->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpBAD->typs=0,((_tmpBAD->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpBAD->sc=Cyc_yyget_YY32(yyyyvsp[0]),_tmpBAD)))))))))));}
break;}case 163: _LL2C8: {
# 1929
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1931
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1928 "parse.y"
struct Cyc_List_List*_tmp5D8=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[3]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[3])));
{struct Cyc_Absyn_Datatypefield*_tmpBAE;yyval=Cyc_YY33(((_tmpBAE=_cycalloc(sizeof(*_tmpBAE)),((_tmpBAE->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpBAE->typs=_tmp5D8,((_tmpBAE->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpBAE->sc=Cyc_yyget_YY32(yyyyvsp[0]),_tmpBAE)))))))))));}
break;}case 164: _LL2C9: {
# 1932
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1934
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1934 "parse.y"
yyval=yyyyvsp[0];
break;}case 165: _LL2CA: {
# 1937
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1939
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1936 "parse.y"
struct Cyc_Parse_Declarator _tmp5DA=Cyc_yyget_YY27(yyyyvsp[1]);
{struct Cyc_Parse_Declarator _tmpBAF;yyval=Cyc_YY27(((_tmpBAF.id=_tmp5DA.id,((_tmpBAF.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY26(yyyyvsp[0]),_tmp5DA.tms),_tmpBAF)))));}
break;}case 166: _LL2CB: {
# 1940
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1942
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=yyyyvsp[0];
break;}case 167: _LL2CC: {
# 1946
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1948
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1945 "parse.y"
struct Cyc_Parse_Declarator _tmp5DC=Cyc_yyget_YY27(yyyyvsp[1]);
{struct Cyc_Parse_Declarator _tmpBB0;yyval=Cyc_YY27(((_tmpBB0.id=_tmp5DC.id,((_tmpBB0.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY26(yyyyvsp[0]),_tmp5DC.tms),_tmpBB0)))));}
break;}case 168: _LL2CD: {
# 1949
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1951
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1951 "parse.y"
{struct Cyc_Parse_Declarator _tmpBB1;yyval=Cyc_YY27(((_tmpBB1.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpBB1.tms=0,_tmpBB1)))));}
break;}case 169: _LL2CE: {
# 1954
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1956
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1953 "parse.y"
yyval=yyyyvsp[1];
break;}case 170: _LL2CF: {
# 1956
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1958
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1957 "parse.y"
struct Cyc_Parse_Declarator _tmp5DF=Cyc_yyget_YY27(yyyyvsp[2]);
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpBB7;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpBB6;struct Cyc_List_List*_tmpBB5;_tmp5DF.tms=((_tmpBB5=_region_malloc(yyr,sizeof(*_tmpBB5)),((_tmpBB5->hd=(void*)((_tmpBB7=_region_malloc(yyr,sizeof(*_tmpBB7)),((_tmpBB7[0]=((_tmpBB6.tag=5,((_tmpBB6.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpBB6.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpBB6)))))),_tmpBB7)))),((_tmpBB5->tl=_tmp5DF.tms,_tmpBB5))))));}
yyval=yyyyvsp[2];
# 1961
break;}case 171: _LL2D0: {
# 1963
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1965
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1962 "parse.y"
{struct Cyc_List_List*_tmpBC1;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpBC0;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpBBF;struct Cyc_Parse_Declarator _tmpBBE;yyval=Cyc_YY27(((_tmpBBE.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpBBE.tms=((_tmpBC1=_region_malloc(yyr,sizeof(*_tmpBC1)),((_tmpBC1->hd=(void*)((_tmpBBF=_region_malloc(yyr,sizeof(*_tmpBBF)),((_tmpBBF[0]=((_tmpBC0.tag=0,((_tmpBC0.f1=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpBC0.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpBC0)))))),_tmpBBF)))),((_tmpBC1->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpBC1)))))),_tmpBBE)))));}
break;}case 172: _LL2D1: {
# 1965
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1967
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1964 "parse.y"
{struct Cyc_List_List*_tmpBCB;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpBCA;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpBC9;struct Cyc_Parse_Declarator _tmpBC8;yyval=Cyc_YY27(((_tmpBC8.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpBC8.tms=(
(_tmpBCB=_region_malloc(yyr,sizeof(*_tmpBCB)),((_tmpBCB->hd=(void*)((_tmpBC9=_region_malloc(yyr,sizeof(*_tmpBC9)),((_tmpBC9[0]=((_tmpBCA.tag=1,((_tmpBCA.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpBCA.f2=Cyc_yyget_YY51(yyyyvsp[4]),((_tmpBCA.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpBCA)))))))),_tmpBC9)))),((_tmpBCB->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpBCB)))))),_tmpBC8)))));}
break;}case 173: _LL2D2: {
# 1968
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1970
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1967 "parse.y"
struct _tuple26*_tmp5EB=Cyc_yyget_YY39(yyyyvsp[2]);struct Cyc_List_List*_tmp5ED;int _tmp5EE;struct Cyc_Absyn_VarargInfo*_tmp5EF;void*_tmp5F0;struct Cyc_List_List*_tmp5F1;struct _tuple26*_tmp5EC=_tmp5EB;_tmp5ED=_tmp5EC->f1;_tmp5EE=_tmp5EC->f2;_tmp5EF=_tmp5EC->f3;_tmp5F0=_tmp5EC->f4;_tmp5F1=_tmp5EC->f5;{
struct Cyc_Absyn_Exp*_tmp5F2=Cyc_yyget_YY55(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp5F3=Cyc_yyget_YY55(yyyyvsp[5]);
{struct Cyc_List_List*_tmpBE0;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBDF;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpBDE;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpBDD;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBDC;struct Cyc_Parse_Declarator _tmpBDB;yyval=Cyc_YY27(((_tmpBDB.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpBDB.tms=((_tmpBE0=_region_malloc(yyr,sizeof(*_tmpBE0)),((_tmpBE0->hd=(void*)((_tmpBDC=_region_malloc(yyr,sizeof(*_tmpBDC)),((_tmpBDC[0]=((_tmpBDF.tag=3,((_tmpBDF.f1=(void*)((_tmpBDD=_region_malloc(yyr,sizeof(*_tmpBDD)),((_tmpBDD[0]=((_tmpBDE.tag=1,((_tmpBDE.f1=_tmp5ED,((_tmpBDE.f2=_tmp5EE,((_tmpBDE.f3=_tmp5EF,((_tmpBDE.f4=_tmp5F0,((_tmpBDE.f5=_tmp5F1,((_tmpBDE.f6=_tmp5F2,((_tmpBDE.f7=_tmp5F3,_tmpBDE)))))))))))))))),_tmpBDD)))),_tmpBDF)))),_tmpBDC)))),((_tmpBE0->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpBE0)))))),_tmpBDB)))));}
# 1972
break;};}case 174: _LL2D3: {
# 1974
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 1976
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 1973 "parse.y"
{struct Cyc_List_List*_tmpBF5;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBF4;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpBF3;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpBF2;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBF1;struct Cyc_Parse_Declarator _tmpBF0;yyval=Cyc_YY27(((_tmpBF0.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpBF0.tms=(
(_tmpBF5=_region_malloc(yyr,sizeof(*_tmpBF5)),((_tmpBF5->hd=(void*)((_tmpBF1=_region_malloc(yyr,sizeof(*_tmpBF1)),((_tmpBF1[0]=((_tmpBF4.tag=3,((_tmpBF4.f1=(void*)((_tmpBF2=_region_malloc(yyr,sizeof(*_tmpBF2)),((_tmpBF2[0]=((_tmpBF3.tag=1,((_tmpBF3.f1=0,((_tmpBF3.f2=0,((_tmpBF3.f3=0,((_tmpBF3.f4=
# 1976
Cyc_yyget_YY49(yyyyvsp[2]),((_tmpBF3.f5=Cyc_yyget_YY50(yyyyvsp[3]),((_tmpBF3.f6=Cyc_yyget_YY55(yyyyvsp[5]),((_tmpBF3.f7=Cyc_yyget_YY55(yyyyvsp[6]),_tmpBF3)))))))))))))))),_tmpBF2)))),_tmpBF4)))),_tmpBF1)))),((_tmpBF5->tl=(
Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpBF5)))))),_tmpBF0)))));}
# 1979
break;}case 175: _LL2D4: {
# 1981
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1983
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1980 "parse.y"
{struct Cyc_List_List*_tmpC0A;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC09;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpC08;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpC07;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC06;struct Cyc_Parse_Declarator _tmpC05;yyval=Cyc_YY27(((_tmpC05.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpC05.tms=((_tmpC0A=_region_malloc(yyr,sizeof(*_tmpC0A)),((_tmpC0A->hd=(void*)((_tmpC06=_region_malloc(yyr,sizeof(*_tmpC06)),((_tmpC06[0]=((_tmpC09.tag=3,((_tmpC09.f1=(void*)((_tmpC07=_region_malloc(yyr,sizeof(*_tmpC07)),((_tmpC07[0]=((_tmpC08.tag=0,((_tmpC08.f1=Cyc_yyget_YY36(yyyyvsp[2]),((_tmpC08.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpC08)))))),_tmpC07)))),_tmpC09)))),_tmpC06)))),((_tmpC0A->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpC0A)))))),_tmpC05)))));}
break;}case 176: _LL2D5: {
# 1983
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1985
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1983 "parse.y"
struct Cyc_List_List*_tmp606=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));
{struct Cyc_List_List*_tmpC14;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpC13;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpC12;struct Cyc_Parse_Declarator _tmpC11;yyval=Cyc_YY27(((_tmpC11.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpC11.tms=((_tmpC14=_region_malloc(yyr,sizeof(*_tmpC14)),((_tmpC14->hd=(void*)((_tmpC12=_region_malloc(yyr,sizeof(*_tmpC12)),((_tmpC12[0]=((_tmpC13.tag=4,((_tmpC13.f1=_tmp606,((_tmpC13.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpC13.f3=0,_tmpC13)))))))),_tmpC12)))),((_tmpC14->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpC14)))))),_tmpC11)))));}
# 1986
break;}case 177: _LL2D6: {
# 1988
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1990
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1987 "parse.y"
{struct Cyc_List_List*_tmpC1E;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC1D;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC1C;struct Cyc_Parse_Declarator _tmpC1B;yyval=Cyc_YY27(((_tmpC1B.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpC1B.tms=((_tmpC1E=_region_malloc(yyr,sizeof(*_tmpC1E)),((_tmpC1E->hd=(void*)((_tmpC1C=_region_malloc(yyr,sizeof(*_tmpC1C)),((_tmpC1C[0]=((_tmpC1D.tag=5,((_tmpC1D.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpC1D.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpC1D)))))),_tmpC1C)))),((_tmpC1E->tl=(
Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpC1E)))))),_tmpC1B)))));}
# 1990
break;}case 178: _LL2D7: {
# 1992
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1994
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2001 "parse.y"
{struct Cyc_Parse_Declarator _tmpC1F;yyval=Cyc_YY27(((_tmpC1F.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpC1F.tms=0,_tmpC1F)))));}
break;}case 179: _LL2D8: {
# 2004
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2006
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2003 "parse.y"
{struct Cyc_Parse_Declarator _tmpC20;yyval=Cyc_YY27(((_tmpC20.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpC20.tms=0,_tmpC20)))));}
break;}case 180: _LL2D9: {
# 2006
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2008
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2005 "parse.y"
yyval=yyyyvsp[1];
break;}case 181: _LL2DA: {
# 2008
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2010
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2009 "parse.y"
struct Cyc_Parse_Declarator _tmp611=Cyc_yyget_YY27(yyyyvsp[2]);
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC26;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC25;struct Cyc_List_List*_tmpC24;_tmp611.tms=((_tmpC24=_region_malloc(yyr,sizeof(*_tmpC24)),((_tmpC24->hd=(void*)((_tmpC26=_region_malloc(yyr,sizeof(*_tmpC26)),((_tmpC26[0]=((_tmpC25.tag=5,((_tmpC25.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpC25.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpC25)))))),_tmpC26)))),((_tmpC24->tl=_tmp611.tms,_tmpC24))))));}
yyval=yyyyvsp[2];
# 2013
break;}case 182: _LL2DB: {
# 2015
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2017
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2014 "parse.y"
struct Cyc_Parse_Declarator _tmp615=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC30;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpC2F;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpC2E;struct Cyc_Parse_Declarator _tmpC2D;yyval=Cyc_YY27(((_tmpC2D.id=_tmp615.id,((_tmpC2D.tms=(
(_tmpC30=_region_malloc(yyr,sizeof(*_tmpC30)),((_tmpC30->hd=(void*)((_tmpC2E=_region_malloc(yyr,sizeof(*_tmpC2E)),((_tmpC2E[0]=((_tmpC2F.tag=0,((_tmpC2F.f1=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpC2F.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpC2F)))))),_tmpC2E)))),((_tmpC30->tl=_tmp615.tms,_tmpC30)))))),_tmpC2D)))));}
break;}case 183: _LL2DC: {
# 2019
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2021
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2018 "parse.y"
struct Cyc_Parse_Declarator _tmp61A=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC3A;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpC39;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpC38;struct Cyc_Parse_Declarator _tmpC37;yyval=Cyc_YY27(((_tmpC37.id=_tmp61A.id,((_tmpC37.tms=(
(_tmpC3A=_region_malloc(yyr,sizeof(*_tmpC3A)),((_tmpC3A->hd=(void*)((_tmpC38=_region_malloc(yyr,sizeof(*_tmpC38)),((_tmpC38[0]=((_tmpC39.tag=1,((_tmpC39.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpC39.f2=Cyc_yyget_YY51(yyyyvsp[4]),((_tmpC39.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpC39)))))))),_tmpC38)))),((_tmpC3A->tl=_tmp61A.tms,_tmpC3A)))))),_tmpC37)))));}
# 2022
break;}case 184: _LL2DD: {
# 2024
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2026
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2023 "parse.y"
struct _tuple26*_tmp61F=Cyc_yyget_YY39(yyyyvsp[2]);struct Cyc_List_List*_tmp621;int _tmp622;struct Cyc_Absyn_VarargInfo*_tmp623;void*_tmp624;struct Cyc_List_List*_tmp625;struct _tuple26*_tmp620=_tmp61F;_tmp621=_tmp620->f1;_tmp622=_tmp620->f2;_tmp623=_tmp620->f3;_tmp624=_tmp620->f4;_tmp625=_tmp620->f5;{
struct Cyc_Absyn_Exp*_tmp626=Cyc_yyget_YY55(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp627=Cyc_yyget_YY55(yyyyvsp[5]);
struct Cyc_Parse_Declarator _tmp628=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC4F;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC4E;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpC4D;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpC4C;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC4B;struct Cyc_Parse_Declarator _tmpC4A;yyval=Cyc_YY27(((_tmpC4A.id=_tmp628.id,((_tmpC4A.tms=((_tmpC4F=_region_malloc(yyr,sizeof(*_tmpC4F)),((_tmpC4F->hd=(void*)((_tmpC4B=_region_malloc(yyr,sizeof(*_tmpC4B)),((_tmpC4B[0]=((_tmpC4E.tag=3,((_tmpC4E.f1=(void*)((_tmpC4C=_region_malloc(yyr,sizeof(*_tmpC4C)),((_tmpC4C[0]=((_tmpC4D.tag=1,((_tmpC4D.f1=_tmp621,((_tmpC4D.f2=_tmp622,((_tmpC4D.f3=_tmp623,((_tmpC4D.f4=_tmp624,((_tmpC4D.f5=_tmp625,((_tmpC4D.f6=_tmp626,((_tmpC4D.f7=_tmp627,_tmpC4D)))))))))))))))),_tmpC4C)))),_tmpC4E)))),_tmpC4B)))),((_tmpC4F->tl=_tmp628.tms,_tmpC4F)))))),_tmpC4A)))));}
# 2029
break;};}case 185: _LL2DE: {
# 2031
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2033
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2030 "parse.y"
struct Cyc_Parse_Declarator _tmp62F=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC64;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC63;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpC62;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpC61;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC60;struct Cyc_Parse_Declarator _tmpC5F;yyval=Cyc_YY27(((_tmpC5F.id=_tmp62F.id,((_tmpC5F.tms=(
(_tmpC64=_region_malloc(yyr,sizeof(*_tmpC64)),((_tmpC64->hd=(void*)((_tmpC60=_region_malloc(yyr,sizeof(*_tmpC60)),((_tmpC60[0]=((_tmpC63.tag=3,((_tmpC63.f1=(void*)((_tmpC61=_region_malloc(yyr,sizeof(*_tmpC61)),((_tmpC61[0]=((_tmpC62.tag=1,((_tmpC62.f1=0,((_tmpC62.f2=0,((_tmpC62.f3=0,((_tmpC62.f4=
# 2034
Cyc_yyget_YY49(yyyyvsp[2]),((_tmpC62.f5=Cyc_yyget_YY50(yyyyvsp[3]),((_tmpC62.f6=Cyc_yyget_YY55(yyyyvsp[5]),((_tmpC62.f7=Cyc_yyget_YY55(yyyyvsp[6]),_tmpC62)))))))))))))))),_tmpC61)))),_tmpC63)))),_tmpC60)))),((_tmpC64->tl=_tmp62F.tms,_tmpC64)))))),_tmpC5F)))));}
# 2037
break;}case 186: _LL2DF: {
# 2039
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2041
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2038 "parse.y"
struct Cyc_Parse_Declarator _tmp636=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC79;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC78;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpC77;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpC76;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC75;struct Cyc_Parse_Declarator _tmpC74;yyval=Cyc_YY27(((_tmpC74.id=_tmp636.id,((_tmpC74.tms=((_tmpC79=_region_malloc(yyr,sizeof(*_tmpC79)),((_tmpC79->hd=(void*)((_tmpC75=_region_malloc(yyr,sizeof(*_tmpC75)),((_tmpC75[0]=((_tmpC78.tag=3,((_tmpC78.f1=(void*)((_tmpC76=_region_malloc(yyr,sizeof(*_tmpC76)),((_tmpC76[0]=((_tmpC77.tag=0,((_tmpC77.f1=Cyc_yyget_YY36(yyyyvsp[2]),((_tmpC77.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpC77)))))),_tmpC76)))),_tmpC78)))),_tmpC75)))),((_tmpC79->tl=_tmp636.tms,_tmpC79)))))),_tmpC74)))));}
break;}case 187: _LL2E0: {
# 2042
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2044
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2042 "parse.y"
struct Cyc_List_List*_tmp63D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));
struct Cyc_Parse_Declarator _tmp63E=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC83;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpC82;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpC81;struct Cyc_Parse_Declarator _tmpC80;yyval=Cyc_YY27(((_tmpC80.id=_tmp63E.id,((_tmpC80.tms=((_tmpC83=_region_malloc(yyr,sizeof(*_tmpC83)),((_tmpC83->hd=(void*)((_tmpC81=_region_malloc(yyr,sizeof(*_tmpC81)),((_tmpC81[0]=((_tmpC82.tag=4,((_tmpC82.f1=_tmp63D,((_tmpC82.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpC82.f3=0,_tmpC82)))))))),_tmpC81)))),((_tmpC83->tl=_tmp63E.tms,_tmpC83)))))),_tmpC80)))));}
# 2046
break;}case 188: _LL2E1: {
# 2048
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2050
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2047 "parse.y"
struct Cyc_Parse_Declarator _tmp643=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC8D;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC8C;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC8B;struct Cyc_Parse_Declarator _tmpC8A;yyval=Cyc_YY27(((_tmpC8A.id=_tmp643.id,((_tmpC8A.tms=((_tmpC8D=_region_malloc(yyr,sizeof(*_tmpC8D)),((_tmpC8D->hd=(void*)((_tmpC8B=_region_malloc(yyr,sizeof(*_tmpC8B)),((_tmpC8B[0]=((_tmpC8C.tag=5,((_tmpC8C.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpC8C.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpC8C)))))),_tmpC8B)))),((_tmpC8D->tl=_tmp643.tms,_tmpC8D)))))),_tmpC8A)))));}
# 2050
break;}case 189: _LL2E2: {
# 2052
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2054
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2060 "parse.y"
yyval=yyyyvsp[0];
break;}case 190: _LL2E3: {
# 2063
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2065
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2061 "parse.y"
yyval=Cyc_YY26(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY26(yyyyvsp[0]),Cyc_yyget_YY26(yyyyvsp[1])));
break;}case 191: _LL2E4: {
# 2064
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2066
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2065 "parse.y"
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY45(yyyyvsp[3])!= 0){
struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC93;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC92;struct Cyc_List_List*_tmpC91;ans=((_tmpC91=_region_malloc(yyr,sizeof(*_tmpC91)),((_tmpC91->hd=(void*)((_tmpC93=_region_malloc(yyr,sizeof(*_tmpC93)),((_tmpC93[0]=((_tmpC92.tag=5,((_tmpC92.f1=(unsigned int)(yyyylsp[3]).first_line,((_tmpC92.f2=Cyc_yyget_YY45(yyyyvsp[3]),_tmpC92)))))),_tmpC93)))),((_tmpC91->tl=ans,_tmpC91))))));}{
# 2069
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple21 _tmp64B=*Cyc_yyget_YY1(yyyyvsp[0]);unsigned int _tmp64D;union Cyc_Absyn_Constraint*_tmp64E;union Cyc_Absyn_Constraint*_tmp64F;struct _tuple21 _tmp64C=_tmp64B;_tmp64D=_tmp64C.f1;_tmp64E=_tmp64C.f2;_tmp64F=_tmp64C.f3;
if(Cyc_Absyn_porting_c_code){
struct Cyc_Absyn_PtrLoc*_tmpC94;ptrloc=((_tmpC94=_cycalloc_atomic(sizeof(*_tmpC94)),((_tmpC94->ptr_loc=_tmp64D,((_tmpC94->rgn_loc=(unsigned int)(yyyylsp[2]).first_line,((_tmpC94->zt_loc=(unsigned int)(yyyylsp[1]).first_line,_tmpC94))))))));}{
# 2074
struct _tuple14 _tmp651=Cyc_Parse_collapse_pointer_quals(_tmp64D,_tmp64E,_tmp64F,Cyc_yyget_YY44(yyyyvsp[2]),Cyc_yyget_YY54(yyyyvsp[1]));union Cyc_Absyn_Constraint*_tmp653;union Cyc_Absyn_Constraint*_tmp654;union Cyc_Absyn_Constraint*_tmp655;void*_tmp656;struct _tuple14 _tmp652=_tmp651;_tmp653=_tmp652.f1;_tmp654=_tmp652.f2;_tmp655=_tmp652.f3;_tmp656=_tmp652.f4;
{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpC9E;struct Cyc_Absyn_PtrAtts _tmpC9D;struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmpC9C;struct Cyc_List_List*_tmpC9B;ans=((_tmpC9B=_region_malloc(yyr,sizeof(*_tmpC9B)),((_tmpC9B->hd=(void*)((_tmpC9E=_region_malloc(yyr,sizeof(*_tmpC9E)),((_tmpC9E[0]=((_tmpC9C.tag=2,((_tmpC9C.f1=((_tmpC9D.rgn=_tmp656,((_tmpC9D.nullable=_tmp653,((_tmpC9D.bounds=_tmp654,((_tmpC9D.zero_term=_tmp655,((_tmpC9D.ptrloc=ptrloc,_tmpC9D)))))))))),((_tmpC9C.f2=Cyc_yyget_YY23(yyyyvsp[4]),_tmpC9C)))))),_tmpC9E)))),((_tmpC9B->tl=ans,_tmpC9B))))));}
yyval=Cyc_YY26(ans);
# 2078
break;};};}case 192: _LL2E5:
# 2080
 yyval=Cyc_YY54(0);
break;case 193: _LL2E6: {
# 2083
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2085
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2081 "parse.y"
{struct Cyc_List_List*_tmpC9F;yyval=Cyc_YY54(((_tmpC9F=_region_malloc(yyr,sizeof(*_tmpC9F)),((_tmpC9F->hd=Cyc_yyget_YY53(yyyyvsp[0]),((_tmpC9F->tl=Cyc_yyget_YY54(yyyyvsp[1]),_tmpC9F)))))));}
break;}case 194: _LL2E7: {
# 2084
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2086
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2086 "parse.y"
{struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct _tmpCA2;struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct*_tmpCA1;yyval=Cyc_YY53((void*)((_tmpCA1=_region_malloc(yyr,sizeof(*_tmpCA1)),((_tmpCA1[0]=((_tmpCA2.tag=0,((_tmpCA2.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),_tmpCA2)))),_tmpCA1)))));}
break;}case 195: _LL2E8: {
# 2089
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2091
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2088 "parse.y"
{struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct _tmpCA5;struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmpCA4;yyval=Cyc_YY53((void*)((_tmpCA4=_region_malloc(yyr,sizeof(*_tmpCA4)),((_tmpCA4[0]=((_tmpCA5.tag=1,((_tmpCA5.f1=Cyc_yyget_YY44(yyyyvsp[2]),_tmpCA5)))),_tmpCA4)))));}
break;}case 196: _LL2E9: {
# 2091
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2093
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2090 "parse.y"
{struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct _tmpCA8;struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct*_tmpCA7;yyval=Cyc_YY53((void*)((_tmpCA7=_region_malloc(yyr,sizeof(*_tmpCA7)),((_tmpCA7[0]=((_tmpCA8.tag=2,_tmpCA8)),_tmpCA7)))));}
break;}case 197: _LL2EA: {
# 2093
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2095
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2092 "parse.y"
{struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct _tmpCAB;struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct*_tmpCAA;yyval=Cyc_YY53((void*)((_tmpCAA=_region_malloc(yyr,sizeof(*_tmpCAA)),((_tmpCAA[0]=((_tmpCAB.tag=3,_tmpCAB)),_tmpCAA)))));}
break;}case 198: _LL2EB: {
# 2095
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2097
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2094 "parse.y"
{struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpCAE;struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpCAD;yyval=Cyc_YY53((void*)((_tmpCAD=_region_malloc(yyr,sizeof(*_tmpCAD)),((_tmpCAD[0]=((_tmpCAE.tag=4,_tmpCAE)),_tmpCAD)))));}
break;}case 199: _LL2EC: {
# 2097
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2099
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2096 "parse.y"
{struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpCB1;struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpCB0;yyval=Cyc_YY53((void*)((_tmpCB0=_region_malloc(yyr,sizeof(*_tmpCB0)),((_tmpCB0[0]=((_tmpCB1.tag=5,_tmpCB1)),_tmpCB0)))));}
break;}case 200: _LL2ED: {
# 2099
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2101
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2098 "parse.y"
{struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct _tmpCB4;struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct*_tmpCB3;yyval=Cyc_YY53((void*)((_tmpCB3=_region_malloc(yyr,sizeof(*_tmpCB3)),((_tmpCB3[0]=((_tmpCB4.tag=6,_tmpCB4)),_tmpCB3)))));}
break;}case 201: _LL2EE: {
# 2101
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2103
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2100 "parse.y"
{struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct _tmpCB7;struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct*_tmpCB6;yyval=Cyc_YY53((void*)((_tmpCB6=_region_malloc(yyr,sizeof(*_tmpCB6)),((_tmpCB6[0]=((_tmpCB7.tag=7,_tmpCB7)),_tmpCB6)))));}
break;}case 202: _LL2EF: {
# 2103
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2105
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2106 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpCB8;yyval=Cyc_YY1(((_tmpCB8=_cycalloc(sizeof(*_tmpCB8)),((_tmpCB8->f1=loc,((_tmpCB8->f2=Cyc_Absyn_true_conref,((_tmpCB8->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpCB8)))))))));}
# 2109
break;}case 203: _LL2F0: {
# 2111
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2113
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2110 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpCB9;yyval=Cyc_YY1(((_tmpCB9=_cycalloc(sizeof(*_tmpCB9)),((_tmpCB9->f1=loc,((_tmpCB9->f2=Cyc_Absyn_false_conref,((_tmpCB9->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpCB9)))))))));}
# 2113
break;}case 204: _LL2F1: {
# 2115
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2117
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2114 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpCBA;yyval=Cyc_YY1(((_tmpCBA=_cycalloc(sizeof(*_tmpCBA)),((_tmpCBA->f1=loc,((_tmpCBA->f2=Cyc_Absyn_true_conref,((_tmpCBA->f3=Cyc_Absyn_bounds_dyneither_conref,_tmpCBA)))))))));}
# 2117
break;}case 205: _LL2F2:
# 2119
 yyval=Cyc_YY2(Cyc_Absyn_bounds_one_conref);
break;case 206: _LL2F3: {
# 2122
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2124
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2120 "parse.y"
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpCBD;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpCBC;yyval=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmpCBC=_cycalloc(sizeof(*_tmpCBC)),((_tmpCBC[0]=((_tmpCBD.tag=1,((_tmpCBD.f1=Cyc_yyget_Exp_tok(yyyyvsp[1]),_tmpCBD)))),_tmpCBC))))));}
break;}case 207: _LL2F4:
# 2123
 yyval=Cyc_YY51(((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());
break;case 208: _LL2F5: {
# 2126
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2128
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2124 "parse.y"
yyval=Cyc_YY51(Cyc_Absyn_true_conref);
break;}case 209: _LL2F6: {
# 2127
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2129
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2125 "parse.y"
yyval=Cyc_YY51(Cyc_Absyn_false_conref);
break;}case 210: _LL2F7:
# 2128
 yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
break;case 211: _LL2F8: {
# 2131
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2133
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2131 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_trk,1);yyval=yyyyvsp[0];
break;}case 212: _LL2F9: {
# 2134
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2136
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2132 "parse.y"
yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
break;}case 213: _LL2FA:
# 2135
 yyval=Cyc_YY23(Cyc_Absyn_empty_tqual((unsigned int)(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset + 1))).first_line));
break;case 214: _LL2FB: {
# 2138
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2140
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2137 "parse.y"
yyval=Cyc_YY23(Cyc_Absyn_combine_tqual(Cyc_yyget_YY23(yyyyvsp[0]),Cyc_yyget_YY23(yyyyvsp[1])));
break;}case 215: _LL2FC: {
# 2140
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2142
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2142 "parse.y"
{struct _tuple26*_tmpCBE;yyval=Cyc_YY39(((_tmpCBE=_cycalloc(sizeof(*_tmpCBE)),((_tmpCBE->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0])),((_tmpCBE->f2=0,((_tmpCBE->f3=0,((_tmpCBE->f4=Cyc_yyget_YY49(yyyyvsp[1]),((_tmpCBE->f5=Cyc_yyget_YY50(yyyyvsp[2]),_tmpCBE)))))))))))));}
break;}case 216: _LL2FD: {
# 2145
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2147
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2144 "parse.y"
{struct _tuple26*_tmpCBF;yyval=Cyc_YY39(((_tmpCBF=_cycalloc(sizeof(*_tmpCBF)),((_tmpCBF->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0])),((_tmpCBF->f2=1,((_tmpCBF->f3=0,((_tmpCBF->f4=Cyc_yyget_YY49(yyyyvsp[3]),((_tmpCBF->f5=Cyc_yyget_YY50(yyyyvsp[4]),_tmpCBF)))))))))))));}
break;}case 217: _LL2FE: {
# 2147
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2149
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2147 "parse.y"
struct _tuple8*_tmp673=Cyc_yyget_YY37(yyyyvsp[2]);struct _dyneither_ptr*_tmp675;struct Cyc_Absyn_Tqual _tmp676;void*_tmp677;struct _tuple8*_tmp674=_tmp673;_tmp675=_tmp674->f1;_tmp676=_tmp674->f2;_tmp677=_tmp674->f3;{
struct Cyc_Absyn_VarargInfo*_tmpCC0;struct Cyc_Absyn_VarargInfo*_tmp678=(_tmpCC0=_cycalloc(sizeof(*_tmpCC0)),((_tmpCC0->name=_tmp675,((_tmpCC0->tq=_tmp676,((_tmpCC0->type=_tmp677,((_tmpCC0->inject=Cyc_yyget_YY31(yyyyvsp[1]),_tmpCC0)))))))));
{struct _tuple26*_tmpCC1;yyval=Cyc_YY39(((_tmpCC1=_cycalloc(sizeof(*_tmpCC1)),((_tmpCC1->f1=0,((_tmpCC1->f2=0,((_tmpCC1->f3=_tmp678,((_tmpCC1->f4=Cyc_yyget_YY49(yyyyvsp[3]),((_tmpCC1->f5=Cyc_yyget_YY50(yyyyvsp[4]),_tmpCC1)))))))))))));}
# 2151
break;};}case 218: _LL2FF: {
# 2153
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2155
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2153 "parse.y"
struct _tuple8*_tmp67B=Cyc_yyget_YY37(yyyyvsp[4]);struct _dyneither_ptr*_tmp67D;struct Cyc_Absyn_Tqual _tmp67E;void*_tmp67F;struct _tuple8*_tmp67C=_tmp67B;_tmp67D=_tmp67C->f1;_tmp67E=_tmp67C->f2;_tmp67F=_tmp67C->f3;{
struct Cyc_Absyn_VarargInfo*_tmpCC2;struct Cyc_Absyn_VarargInfo*_tmp680=(_tmpCC2=_cycalloc(sizeof(*_tmpCC2)),((_tmpCC2->name=_tmp67D,((_tmpCC2->tq=_tmp67E,((_tmpCC2->type=_tmp67F,((_tmpCC2->inject=Cyc_yyget_YY31(yyyyvsp[3]),_tmpCC2)))))))));
{struct _tuple26*_tmpCC3;yyval=Cyc_YY39(((_tmpCC3=_cycalloc(sizeof(*_tmpCC3)),((_tmpCC3->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0])),((_tmpCC3->f2=0,((_tmpCC3->f3=_tmp680,((_tmpCC3->f4=Cyc_yyget_YY49(yyyyvsp[5]),((_tmpCC3->f5=Cyc_yyget_YY50(yyyyvsp[6]),_tmpCC3)))))))))))));}
# 2157
break;};}case 219: _LL300: {
# 2159
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2161
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2161 "parse.y"
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpCC6;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpCC5;yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),(void*)((_tmpCC5=_cycalloc(sizeof(*_tmpCC5)),((_tmpCC5[0]=((_tmpCC6.tag=1,((_tmpCC6.f1=0,_tmpCC6)))),_tmpCC5))))));}
break;}case 220: _LL301: {
# 2164
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2166
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2162 "parse.y"
yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),Cyc_Tcutil_kind_to_bound(Cyc_yyget_YY43(yyyyvsp[2]))));
break;}case 221: _LL302:
# 2165
 yyval=Cyc_YY49(0);
break;case 222: _LL303: {
# 2168
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2170
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2166 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCC9;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCC8;yyval=Cyc_YY49((void*)((_tmpCC8=_cycalloc(sizeof(*_tmpCC8)),((_tmpCC8[0]=((_tmpCC9.tag=24,((_tmpCC9.f1=Cyc_yyget_YY40(yyyyvsp[1]),_tmpCC9)))),_tmpCC8)))));}
break;}case 223: _LL304:
# 2169
 yyval=Cyc_YY50(0);
break;case 224: _LL305: {
# 2172
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2174
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2171 "parse.y"
yyval=yyyyvsp[1];
break;}case 225: _LL306: {
# 2174
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2176
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2179 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpCCC;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCCB;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp687=(_tmpCCB=_cycalloc(sizeof(*_tmpCCB)),((_tmpCCB[0]=((_tmpCCC.tag=2,((_tmpCCC.f1=0,((_tmpCCC.f2=& Cyc_Tcutil_trk,_tmpCCC)))))),_tmpCCB)));
struct _dyneither_ptr _tmp688=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp689=Cyc_Parse_id2type(_tmp688,(void*)_tmp687);
{struct _tuple29*_tmpCD6;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCD5;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCD4;struct Cyc_List_List*_tmpCD3;yyval=Cyc_YY50(((_tmpCD3=_cycalloc(sizeof(*_tmpCD3)),((_tmpCD3->hd=((_tmpCD6=_cycalloc(sizeof(*_tmpCD6)),((_tmpCD6->f1=(void*)((_tmpCD4=_cycalloc(sizeof(*_tmpCD4)),((_tmpCD4[0]=((_tmpCD5.tag=24,((_tmpCD5.f1=Cyc_yyget_YY40(yyyyvsp[0]),_tmpCD5)))),_tmpCD4)))),((_tmpCD6->f2=_tmp689,_tmpCD6)))))),((_tmpCD3->tl=0,_tmpCD3)))))));}
# 2184
break;}case 226: _LL307: {
# 2186
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2188
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2186 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpCD9;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCD8;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp690=(_tmpCD8=_cycalloc(sizeof(*_tmpCD8)),((_tmpCD8[0]=((_tmpCD9.tag=2,((_tmpCD9.f1=0,((_tmpCD9.f2=& Cyc_Tcutil_trk,_tmpCD9)))))),_tmpCD8)));
struct _dyneither_ptr _tmp691=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp692=Cyc_Parse_id2type(_tmp691,(void*)_tmp690);
{struct _tuple29*_tmpCE3;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCE2;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCE1;struct Cyc_List_List*_tmpCE0;yyval=Cyc_YY50(((_tmpCE0=_cycalloc(sizeof(*_tmpCE0)),((_tmpCE0->hd=((_tmpCE3=_cycalloc(sizeof(*_tmpCE3)),((_tmpCE3->f1=(void*)((_tmpCE1=_cycalloc(sizeof(*_tmpCE1)),((_tmpCE1[0]=((_tmpCE2.tag=24,((_tmpCE2.f1=Cyc_yyget_YY40(yyyyvsp[0]),_tmpCE2)))),_tmpCE1)))),((_tmpCE3->f2=_tmp692,_tmpCE3)))))),((_tmpCE0->tl=Cyc_yyget_YY50(yyyyvsp[4]),_tmpCE0)))))));}
# 2191
break;}case 227: _LL308:
# 2193
 yyval=Cyc_YY31(0);
break;case 228: _LL309: {
# 2196
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2198
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2197 "parse.y"
{const char*_tmpCE4;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]),((_tmpCE4="inject",_tag_dyneither(_tmpCE4,sizeof(char),7))))!= 0){
const char*_tmpCE5;Cyc_Parse_err(((_tmpCE5="missing type in function declaration",_tag_dyneither(_tmpCE5,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY31(1);
# 2201
break;}case 229: _LL30A: {
# 2203
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2205
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2204 "parse.y"
yyval=yyyyvsp[0];
break;}case 230: _LL30B: {
# 2207
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2209
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2205 "parse.y"
yyval=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY40(yyyyvsp[0]),Cyc_yyget_YY40(yyyyvsp[2])));
break;}case 231: _LL30C: {
# 2208
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2210
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2209 "parse.y"
yyval=Cyc_YY40(0);
break;}case 232: _LL30D: {
# 2212
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2214
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2210 "parse.y"
yyval=yyyyvsp[1];
break;}case 233: _LL30E: {
# 2213
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2215
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2212 "parse.y"
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpCEB;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpCEA;struct Cyc_List_List*_tmpCE9;yyval=Cyc_YY40(((_tmpCE9=_cycalloc(sizeof(*_tmpCE9)),((_tmpCE9->hd=(void*)((_tmpCEB=_cycalloc(sizeof(*_tmpCEB)),((_tmpCEB[0]=((_tmpCEA.tag=25,((_tmpCEA.f1=Cyc_yyget_YY44(yyyyvsp[2]),_tmpCEA)))),_tmpCEB)))),((_tmpCE9->tl=0,_tmpCE9)))))));}
break;}case 234: _LL30F: {
# 2215
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2217
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2214 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_ek,0);
{struct Cyc_List_List*_tmpCEC;yyval=Cyc_YY40(((_tmpCEC=_cycalloc(sizeof(*_tmpCEC)),((_tmpCEC->hd=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpCEC->tl=0,_tmpCEC)))))));}
# 2217
break;}case 235: _LL310: {
# 2219
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2221
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2234 "parse.y"
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpCF2;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpCF1;struct Cyc_List_List*_tmpCF0;yyval=Cyc_YY40(((_tmpCF0=_cycalloc(sizeof(*_tmpCF0)),((_tmpCF0->hd=(void*)((_tmpCF2=_cycalloc(sizeof(*_tmpCF2)),((_tmpCF2[0]=((_tmpCF1.tag=23,((_tmpCF1.f1=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line),_tmpCF1)))),_tmpCF2)))),((_tmpCF0->tl=0,_tmpCF0)))))));}
break;}case 236: _LL311: {
# 2237
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2239
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2236 "parse.y"
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpCF8;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpCF7;struct Cyc_List_List*_tmpCF6;yyval=Cyc_YY40(((_tmpCF6=_cycalloc(sizeof(*_tmpCF6)),((_tmpCF6->hd=(void*)((_tmpCF8=_cycalloc(sizeof(*_tmpCF8)),((_tmpCF8[0]=((_tmpCF7.tag=23,((_tmpCF7.f1=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line),_tmpCF7)))),_tmpCF8)))),((_tmpCF6->tl=Cyc_yyget_YY40(yyyyvsp[2]),_tmpCF6)))))));}
break;}case 237: _LL312: {
# 2239
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2241
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpCF9;yyval=Cyc_YY38(((_tmpCF9=_cycalloc(sizeof(*_tmpCF9)),((_tmpCF9->hd=Cyc_yyget_YY37(yyyyvsp[0]),((_tmpCF9->tl=0,_tmpCF9)))))));}
break;}case 238: _LL313: {
# 2245
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2247
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2244 "parse.y"
{struct Cyc_List_List*_tmpCFA;yyval=Cyc_YY38(((_tmpCFA=_cycalloc(sizeof(*_tmpCFA)),((_tmpCFA->hd=Cyc_yyget_YY37(yyyyvsp[2]),((_tmpCFA->tl=Cyc_yyget_YY38(yyyyvsp[0]),_tmpCFA)))))));}
break;}case 239: _LL314: {
# 2247
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2249
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
struct _tuple25 _tmp6A7=Cyc_yyget_YY35(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp6A9;struct Cyc_Parse_Type_specifier _tmp6AA;struct Cyc_List_List*_tmp6AB;struct _tuple25 _tmp6A8=_tmp6A7;_tmp6A9=_tmp6A8.f1;_tmp6AA=_tmp6A8.f2;_tmp6AB=_tmp6A8.f3;
if(_tmp6A9.loc == 0)_tmp6A9.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct Cyc_Parse_Declarator _tmp6AC=Cyc_yyget_YY27(yyyyvsp[1]);struct _tuple0*_tmp6AE;struct Cyc_List_List*_tmp6AF;struct Cyc_Parse_Declarator _tmp6AD=_tmp6AC;_tmp6AE=_tmp6AD.id;_tmp6AF=_tmp6AD.tms;{
void*_tmp6B0=Cyc_Parse_speclist2typ(_tmp6AA,(unsigned int)(yyyylsp[0]).first_line);
struct _tuple13 _tmp6B1=Cyc_Parse_apply_tms(_tmp6A9,_tmp6B0,_tmp6AB,_tmp6AF);struct Cyc_Absyn_Tqual _tmp6B3;void*_tmp6B4;struct Cyc_List_List*_tmp6B5;struct Cyc_List_List*_tmp6B6;struct _tuple13 _tmp6B2=_tmp6B1;_tmp6B3=_tmp6B2.f1;_tmp6B4=_tmp6B2.f2;_tmp6B5=_tmp6B2.f3;_tmp6B6=_tmp6B2.f4;
if(_tmp6B5 != 0){
const char*_tmpCFB;Cyc_Parse_err(((_tmpCFB="parameter with bad type params",_tag_dyneither(_tmpCFB,sizeof(char),31))),(unsigned int)(yyyylsp[1]).first_line);}
if(Cyc_Absyn_is_qvar_qualified(_tmp6AE)){
const char*_tmpCFC;Cyc_Parse_err(((_tmpCFC="parameter cannot be qualified with a namespace",_tag_dyneither(_tmpCFC,sizeof(char),47))),(unsigned int)(yyyylsp[0]).first_line);}{
struct _dyneither_ptr*idopt=(*_tmp6AE).f2;
if(_tmp6B6 != 0){
const char*_tmpCFF;void*_tmpCFE;(_tmpCFE=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCFF="extra attributes on parameter, ignoring",_tag_dyneither(_tmpCFF,sizeof(char),40))),_tag_dyneither(_tmpCFE,sizeof(void*),0)));}
{struct _tuple8*_tmpD00;yyval=Cyc_YY37(((_tmpD00=_cycalloc(sizeof(*_tmpD00)),((_tmpD00->f1=idopt,((_tmpD00->f2=_tmp6B3,((_tmpD00->f3=_tmp6B4,_tmpD00)))))))));}
# 2264
break;};};};}case 240: _LL315: {
# 2266
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2268
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2265 "parse.y"
struct _tuple25 _tmp6BC=Cyc_yyget_YY35(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp6BE;struct Cyc_Parse_Type_specifier _tmp6BF;struct Cyc_List_List*_tmp6C0;struct _tuple25 _tmp6BD=_tmp6BC;_tmp6BE=_tmp6BD.f1;_tmp6BF=_tmp6BD.f2;_tmp6C0=_tmp6BD.f3;
if(_tmp6BE.loc == 0)_tmp6BE.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp6C1=Cyc_Parse_speclist2typ(_tmp6BF,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp6C0 != 0){
const char*_tmpD03;void*_tmpD02;(_tmpD02=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpD03="bad attributes on parameter, ignoring",_tag_dyneither(_tmpD03,sizeof(char),38))),_tag_dyneither(_tmpD02,sizeof(void*),0)));}
{struct _tuple8*_tmpD04;yyval=Cyc_YY37(((_tmpD04=_cycalloc(sizeof(*_tmpD04)),((_tmpD04->f1=0,((_tmpD04->f2=_tmp6BE,((_tmpD04->f3=_tmp6C1,_tmpD04)))))))));}
# 2272
break;};}case 241: _LL316: {
# 2274
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2276
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2273 "parse.y"
struct _tuple25 _tmp6C5=Cyc_yyget_YY35(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp6C7;struct Cyc_Parse_Type_specifier _tmp6C8;struct Cyc_List_List*_tmp6C9;struct _tuple25 _tmp6C6=_tmp6C5;_tmp6C7=_tmp6C6.f1;_tmp6C8=_tmp6C6.f2;_tmp6C9=_tmp6C6.f3;
if(_tmp6C7.loc == 0)_tmp6C7.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp6CA=Cyc_Parse_speclist2typ(_tmp6C8,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp6CB=(Cyc_yyget_YY30(yyyyvsp[1])).tms;
struct _tuple13 _tmp6CC=Cyc_Parse_apply_tms(_tmp6C7,_tmp6CA,_tmp6C9,_tmp6CB);struct Cyc_Absyn_Tqual _tmp6CE;void*_tmp6CF;struct Cyc_List_List*_tmp6D0;struct Cyc_List_List*_tmp6D1;struct _tuple13 _tmp6CD=_tmp6CC;_tmp6CE=_tmp6CD.f1;_tmp6CF=_tmp6CD.f2;_tmp6D0=_tmp6CD.f3;_tmp6D1=_tmp6CD.f4;
if(_tmp6D0 != 0){
const char*_tmpD07;void*_tmpD06;(_tmpD06=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpD07="bad type parameters on formal argument, ignoring",_tag_dyneither(_tmpD07,sizeof(char),49))),_tag_dyneither(_tmpD06,sizeof(void*),0)));}
# 2281
if(_tmp6D1 != 0){
const char*_tmpD0A;void*_tmpD09;(_tmpD09=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpD0A="bad attributes on parameter, ignoring",_tag_dyneither(_tmpD0A,sizeof(char),38))),_tag_dyneither(_tmpD09,sizeof(void*),0)));}
{struct _tuple8*_tmpD0B;yyval=Cyc_YY37(((_tmpD0B=_cycalloc(sizeof(*_tmpD0B)),((_tmpD0B->f1=0,((_tmpD0B->f2=_tmp6CE,((_tmpD0B->f3=_tmp6CF,_tmpD0B)))))))));}
# 2285
break;};}case 242: _LL317: {
# 2287
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2289
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2289 "parse.y"
yyval=Cyc_YY36(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyyyvsp[0])));
break;}case 243: _LL318: {
# 2292
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2294
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2293 "parse.y"
{struct _dyneither_ptr*_tmpD0E;struct Cyc_List_List*_tmpD0D;yyval=Cyc_YY36(((_tmpD0D=_cycalloc(sizeof(*_tmpD0D)),((_tmpD0D->hd=((_tmpD0E=_cycalloc(sizeof(*_tmpD0E)),((_tmpD0E[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpD0E)))),((_tmpD0D->tl=0,_tmpD0D)))))));}
break;}case 244: _LL319: {
# 2296
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2298
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2295 "parse.y"
{struct _dyneither_ptr*_tmpD11;struct Cyc_List_List*_tmpD10;yyval=Cyc_YY36(((_tmpD10=_cycalloc(sizeof(*_tmpD10)),((_tmpD10->hd=((_tmpD11=_cycalloc(sizeof(*_tmpD11)),((_tmpD11[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpD11)))),((_tmpD10->tl=Cyc_yyget_YY36(yyyyvsp[0]),_tmpD10)))))));}
break;}case 245: _LL31A: {
# 2298
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2300
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2299 "parse.y"
yyval=yyyyvsp[0];
break;}case 246: _LL31B: {
# 2302
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2304
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2300 "parse.y"
yyval=yyyyvsp[0];
break;}case 247: _LL31C: {
# 2303
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2305
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2305 "parse.y"
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpD14;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpD13;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpD13=_cycalloc(sizeof(*_tmpD13)),((_tmpD13[0]=((_tmpD14.tag=35,((_tmpD14.f1=0,((_tmpD14.f2=0,_tmpD14)))))),_tmpD13)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 248: _LL31D: {
# 2308
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2310
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2307 "parse.y"
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpD17;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpD16;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpD16=_cycalloc(sizeof(*_tmpD16)),((_tmpD16[0]=((_tmpD17.tag=35,((_tmpD17.f1=0,((_tmpD17.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[1])),_tmpD17)))))),_tmpD16)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 249: _LL31E: {
# 2310
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2312
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2309 "parse.y"
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpD1A;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpD19;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpD19=_cycalloc(sizeof(*_tmpD19)),((_tmpD19[0]=((_tmpD1A.tag=35,((_tmpD1A.f1=0,((_tmpD1A.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[1])),_tmpD1A)))))),_tmpD19)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 250: _LL31F: {
# 2312
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2314
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2311 "parse.y"
struct _dyneither_ptr*_tmpD1D;struct _tuple0*_tmpD1C;struct Cyc_Absyn_Vardecl*_tmp6E1=Cyc_Absyn_new_vardecl(((_tmpD1C=_cycalloc(sizeof(*_tmpD1C)),((_tmpD1C->f1=Cyc_Absyn_Loc_n,((_tmpD1C->f2=((_tmpD1D=_cycalloc(sizeof(*_tmpD1D)),((_tmpD1D[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpD1D)))),_tmpD1C)))))),Cyc_Absyn_uint_typ,
Cyc_Absyn_uint_exp(0,(unsigned int)(yyyylsp[2]).first_line));
# 2314
(_tmp6E1->tq).real_const=1;
{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpD20;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpD1F;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpD1F=_cycalloc(sizeof(*_tmpD1F)),((_tmpD1F[0]=((_tmpD20.tag=26,((_tmpD20.f1=_tmp6E1,((_tmpD20.f2=Cyc_yyget_Exp_tok(yyyyvsp[4]),((_tmpD20.f3=Cyc_yyget_Exp_tok(yyyyvsp[6]),((_tmpD20.f4=0,_tmpD20)))))))))),_tmpD1F)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2317
break;}case 251: _LL320: {
# 2319
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2321
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2318 "parse.y"
void*_tmp6E6=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpD23;struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpD22;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpD22=_cycalloc(sizeof(*_tmpD22)),((_tmpD22[0]=((_tmpD23.tag=27,((_tmpD23.f1=Cyc_yyget_Exp_tok(yyyyvsp[4]),((_tmpD23.f2=_tmp6E6,((_tmpD23.f3=0,_tmpD23)))))))),_tmpD22)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2321
break;}case 252: _LL321: {
# 2323
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2325
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct _tuple30*_tmpD26;struct Cyc_List_List*_tmpD25;yyval=Cyc_YY5(((_tmpD25=_cycalloc(sizeof(*_tmpD25)),((_tmpD25->hd=((_tmpD26=_cycalloc(sizeof(*_tmpD26)),((_tmpD26->f1=0,((_tmpD26->f2=Cyc_yyget_Exp_tok(yyyyvsp[0]),_tmpD26)))))),((_tmpD25->tl=0,_tmpD25)))))));}
break;}case 253: _LL322: {
# 2329
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2331
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2328 "parse.y"
{struct _tuple30*_tmpD29;struct Cyc_List_List*_tmpD28;yyval=Cyc_YY5(((_tmpD28=_cycalloc(sizeof(*_tmpD28)),((_tmpD28->hd=((_tmpD29=_cycalloc(sizeof(*_tmpD29)),((_tmpD29->f1=Cyc_yyget_YY41(yyyyvsp[0]),((_tmpD29->f2=Cyc_yyget_Exp_tok(yyyyvsp[1]),_tmpD29)))))),((_tmpD28->tl=0,_tmpD28)))))));}
break;}case 254: _LL323: {
# 2331
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2333
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2330 "parse.y"
{struct _tuple30*_tmpD2C;struct Cyc_List_List*_tmpD2B;yyval=Cyc_YY5(((_tmpD2B=_cycalloc(sizeof(*_tmpD2B)),((_tmpD2B->hd=((_tmpD2C=_cycalloc(sizeof(*_tmpD2C)),((_tmpD2C->f1=0,((_tmpD2C->f2=Cyc_yyget_Exp_tok(yyyyvsp[2]),_tmpD2C)))))),((_tmpD2B->tl=Cyc_yyget_YY5(yyyyvsp[0]),_tmpD2B)))))));}
break;}case 255: _LL324: {
# 2333
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2335
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2332 "parse.y"
{struct _tuple30*_tmpD2F;struct Cyc_List_List*_tmpD2E;yyval=Cyc_YY5(((_tmpD2E=_cycalloc(sizeof(*_tmpD2E)),((_tmpD2E->hd=((_tmpD2F=_cycalloc(sizeof(*_tmpD2F)),((_tmpD2F->f1=Cyc_yyget_YY41(yyyyvsp[2]),((_tmpD2F->f2=Cyc_yyget_Exp_tok(yyyyvsp[3]),_tmpD2F)))))),((_tmpD2E->tl=Cyc_yyget_YY5(yyyyvsp[0]),_tmpD2E)))))));}
break;}case 256: _LL325: {
# 2335
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2337
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2336 "parse.y"
yyval=Cyc_YY41(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])));
break;}case 257: _LL326: {
# 2339
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2341
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2341 "parse.y"
{struct Cyc_List_List*_tmpD30;yyval=Cyc_YY41(((_tmpD30=_cycalloc(sizeof(*_tmpD30)),((_tmpD30->hd=Cyc_yyget_YY42(yyyyvsp[0]),((_tmpD30->tl=0,_tmpD30)))))));}
break;}case 258: _LL327: {
# 2344
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2346
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2342 "parse.y"
{struct Cyc_List_List*_tmpD31;yyval=Cyc_YY41(((_tmpD31=_cycalloc(sizeof(*_tmpD31)),((_tmpD31->hd=Cyc_yyget_YY42(yyyyvsp[1]),((_tmpD31->tl=Cyc_yyget_YY41(yyyyvsp[0]),_tmpD31)))))));}
break;}case 259: _LL328: {
# 2345
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2347
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2346 "parse.y"
{struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpD34;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpD33;yyval=Cyc_YY42((void*)((_tmpD33=_cycalloc(sizeof(*_tmpD33)),((_tmpD33[0]=((_tmpD34.tag=0,((_tmpD34.f1=Cyc_yyget_Exp_tok(yyyyvsp[1]),_tmpD34)))),_tmpD33)))));}
break;}case 260: _LL329: {
# 2349
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2351
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2347 "parse.y"
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpD3A;struct _dyneither_ptr*_tmpD39;struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpD38;yyval=Cyc_YY42((void*)((_tmpD38=_cycalloc(sizeof(*_tmpD38)),((_tmpD38[0]=((_tmpD3A.tag=1,((_tmpD3A.f1=((_tmpD39=_cycalloc(sizeof(*_tmpD39)),((_tmpD39[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpD39)))),_tmpD3A)))),_tmpD38)))));}
break;}case 261: _LL32A: {
# 2350
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2352
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2352 "parse.y"
struct _tuple25 _tmp6F8=Cyc_yyget_YY35(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp6FA;struct Cyc_Parse_Type_specifier _tmp6FB;struct Cyc_List_List*_tmp6FC;struct _tuple25 _tmp6F9=_tmp6F8;_tmp6FA=_tmp6F9.f1;_tmp6FB=_tmp6F9.f2;_tmp6FC=_tmp6F9.f3;{
void*_tmp6FD=Cyc_Parse_speclist2typ(_tmp6FB,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp6FC != 0){
const char*_tmpD3D;void*_tmpD3C;(_tmpD3C=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpD3D="ignoring attributes in type",_tag_dyneither(_tmpD3D,sizeof(char),28))),_tag_dyneither(_tmpD3C,sizeof(void*),0)));}
{struct _tuple8*_tmpD3E;yyval=Cyc_YY37(((_tmpD3E=_cycalloc(sizeof(*_tmpD3E)),((_tmpD3E->f1=0,((_tmpD3E->f2=_tmp6FA,((_tmpD3E->f3=_tmp6FD,_tmpD3E)))))))));}
# 2358
break;};}case 262: _LL32B: {
# 2360
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2362
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2359 "parse.y"
struct _tuple25 _tmp701=Cyc_yyget_YY35(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp703;struct Cyc_Parse_Type_specifier _tmp704;struct Cyc_List_List*_tmp705;struct _tuple25 _tmp702=_tmp701;_tmp703=_tmp702.f1;_tmp704=_tmp702.f2;_tmp705=_tmp702.f3;{
void*_tmp706=Cyc_Parse_speclist2typ(_tmp704,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp707=(Cyc_yyget_YY30(yyyyvsp[1])).tms;
struct _tuple13 _tmp708=Cyc_Parse_apply_tms(_tmp703,_tmp706,_tmp705,_tmp707);
if(_tmp708.f3 != 0){
# 2365
const char*_tmpD41;void*_tmpD40;(_tmpD40=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpD41="bad type params, ignoring",_tag_dyneither(_tmpD41,sizeof(char),26))),_tag_dyneither(_tmpD40,sizeof(void*),0)));}
if(_tmp708.f4 != 0){
const char*_tmpD44;void*_tmpD43;(_tmpD43=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpD44="bad specifiers, ignoring",_tag_dyneither(_tmpD44,sizeof(char),25))),_tag_dyneither(_tmpD43,sizeof(void*),0)));}
{struct _tuple8*_tmpD45;yyval=Cyc_YY37(((_tmpD45=_cycalloc(sizeof(*_tmpD45)),((_tmpD45->f1=0,((_tmpD45->f2=_tmp708.f1,((_tmpD45->f3=_tmp708.f2,_tmpD45)))))))));}
# 2370
break;};}case 263: _LL32C: {
# 2372
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2374
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2373 "parse.y"
yyval=Cyc_YY44(Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 264: _LL32D: {
# 2376
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2378
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2374 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD48;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD47;yyval=Cyc_YY44((void*)((_tmpD47=_cycalloc(sizeof(*_tmpD47)),((_tmpD47[0]=((_tmpD48.tag=24,((_tmpD48.f1=0,_tmpD48)))),_tmpD47)))));}
break;}case 265: _LL32E: {
# 2377
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2379
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2375 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD4B;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD4A;yyval=Cyc_YY44((void*)((_tmpD4A=_cycalloc(sizeof(*_tmpD4A)),((_tmpD4A[0]=((_tmpD4B.tag=24,((_tmpD4B.f1=Cyc_yyget_YY40(yyyyvsp[1]),_tmpD4B)))),_tmpD4A)))));}
break;}case 266: _LL32F: {
# 2378
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2380
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2376 "parse.y"
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpD4E;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpD4D;yyval=Cyc_YY44((void*)((_tmpD4D=_cycalloc(sizeof(*_tmpD4D)),((_tmpD4D[0]=((_tmpD4E.tag=25,((_tmpD4E.f1=Cyc_yyget_YY44(yyyyvsp[2]),_tmpD4E)))),_tmpD4D)))));}
break;}case 267: _LL330: {
# 2379
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2381
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2377 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD54;struct Cyc_List_List*_tmpD53;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD52;yyval=Cyc_YY44((void*)((_tmpD52=_cycalloc(sizeof(*_tmpD52)),((_tmpD52[0]=((_tmpD54.tag=24,((_tmpD54.f1=((_tmpD53=_cycalloc(sizeof(*_tmpD53)),((_tmpD53->hd=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpD53->tl=Cyc_yyget_YY40(yyyyvsp[2]),_tmpD53)))))),_tmpD54)))),_tmpD52)))));}
break;}case 268: _LL331: {
# 2380
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2382
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpD55;yyval=Cyc_YY40(((_tmpD55=_cycalloc(sizeof(*_tmpD55)),((_tmpD55->hd=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpD55->tl=0,_tmpD55)))))));}
break;}case 269: _LL332: {
# 2386
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2388
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2384 "parse.y"
{struct Cyc_List_List*_tmpD56;yyval=Cyc_YY40(((_tmpD56=_cycalloc(sizeof(*_tmpD56)),((_tmpD56->hd=Cyc_yyget_YY44(yyyyvsp[2]),((_tmpD56->tl=Cyc_yyget_YY40(yyyyvsp[0]),_tmpD56)))))));}
break;}case 270: _LL333: {
# 2387
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2389
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2389 "parse.y"
{struct Cyc_Parse_Abstractdeclarator _tmpD57;yyval=Cyc_YY30(((_tmpD57.tms=Cyc_yyget_YY26(yyyyvsp[0]),_tmpD57)));}
break;}case 271: _LL334: {
# 2392
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2394
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2391 "parse.y"
yyval=yyyyvsp[0];
break;}case 272: _LL335: {
# 2394
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2396
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2393 "parse.y"
{struct Cyc_Parse_Abstractdeclarator _tmpD58;yyval=Cyc_YY30(((_tmpD58.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY26(yyyyvsp[0]),(Cyc_yyget_YY30(yyyyvsp[1])).tms),_tmpD58)));}
break;}case 273: _LL336: {
# 2396
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2398
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2398 "parse.y"
yyval=yyyyvsp[1];
break;}case 274: _LL337: {
# 2401
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2403
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2400 "parse.y"
{struct Cyc_List_List*_tmpD62;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpD61;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpD60;struct Cyc_Parse_Abstractdeclarator _tmpD5F;yyval=Cyc_YY30(((_tmpD5F.tms=((_tmpD62=_region_malloc(yyr,sizeof(*_tmpD62)),((_tmpD62->hd=(void*)((_tmpD60=_region_malloc(yyr,sizeof(*_tmpD60)),((_tmpD60[0]=((_tmpD61.tag=0,((_tmpD61.f1=Cyc_yyget_YY51(yyyyvsp[2]),((_tmpD61.f2=(unsigned int)(yyyylsp[2]).first_line,_tmpD61)))))),_tmpD60)))),((_tmpD62->tl=0,_tmpD62)))))),_tmpD5F)));}
break;}case 275: _LL338: {
# 2403
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2405
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2402 "parse.y"
{struct Cyc_List_List*_tmpD6C;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpD6B;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpD6A;struct Cyc_Parse_Abstractdeclarator _tmpD69;yyval=Cyc_YY30(((_tmpD69.tms=((_tmpD6C=_region_malloc(yyr,sizeof(*_tmpD6C)),((_tmpD6C->hd=(void*)((_tmpD6A=_region_malloc(yyr,sizeof(*_tmpD6A)),((_tmpD6A[0]=((_tmpD6B.tag=0,((_tmpD6B.f1=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpD6B.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpD6B)))))),_tmpD6A)))),((_tmpD6C->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpD6C)))))),_tmpD69)));}
break;}case 276: _LL339: {
# 2405
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2407
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2404 "parse.y"
{struct Cyc_List_List*_tmpD76;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpD75;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD74;struct Cyc_Parse_Abstractdeclarator _tmpD73;yyval=Cyc_YY30(((_tmpD73.tms=((_tmpD76=_region_malloc(yyr,sizeof(*_tmpD76)),((_tmpD76->hd=(void*)((_tmpD74=_region_malloc(yyr,sizeof(*_tmpD74)),((_tmpD74[0]=((_tmpD75.tag=1,((_tmpD75.f1=Cyc_yyget_Exp_tok(yyyyvsp[1]),((_tmpD75.f2=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpD75.f3=(unsigned int)(yyyylsp[3]).first_line,_tmpD75)))))))),_tmpD74)))),((_tmpD76->tl=0,_tmpD76)))))),_tmpD73)));}
break;}case 277: _LL33A: {
# 2407
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2409
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2406 "parse.y"
{struct Cyc_List_List*_tmpD80;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpD7F;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD7E;struct Cyc_Parse_Abstractdeclarator _tmpD7D;yyval=Cyc_YY30(((_tmpD7D.tms=((_tmpD80=_region_malloc(yyr,sizeof(*_tmpD80)),((_tmpD80->hd=(void*)((_tmpD7E=_region_malloc(yyr,sizeof(*_tmpD7E)),((_tmpD7E[0]=((_tmpD7F.tag=1,((_tmpD7F.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpD7F.f2=Cyc_yyget_YY51(yyyyvsp[4]),((_tmpD7F.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpD7F)))))))),_tmpD7E)))),((_tmpD80->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpD80)))))),_tmpD7D)));}
# 2409
break;}case 278: _LL33B: {
# 2411
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2413
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2410 "parse.y"
{struct Cyc_List_List*_tmpD95;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD94;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD93;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD92;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD91;struct Cyc_Parse_Abstractdeclarator _tmpD90;yyval=Cyc_YY30(((_tmpD90.tms=((_tmpD95=_region_malloc(yyr,sizeof(*_tmpD95)),((_tmpD95->hd=(void*)((_tmpD91=_region_malloc(yyr,sizeof(*_tmpD91)),((_tmpD91[0]=((_tmpD94.tag=3,((_tmpD94.f1=(void*)((_tmpD92=_region_malloc(yyr,sizeof(*_tmpD92)),((_tmpD92[0]=((_tmpD93.tag=1,((_tmpD93.f1=0,((_tmpD93.f2=0,((_tmpD93.f3=0,((_tmpD93.f4=Cyc_yyget_YY49(yyyyvsp[1]),((_tmpD93.f5=Cyc_yyget_YY50(yyyyvsp[2]),((_tmpD93.f6=Cyc_yyget_YY55(yyyyvsp[4]),((_tmpD93.f7=Cyc_yyget_YY55(yyyyvsp[5]),_tmpD93)))))))))))))))),_tmpD92)))),_tmpD94)))),_tmpD91)))),((_tmpD95->tl=0,_tmpD95)))))),_tmpD90)));}
# 2412
break;}case 279: _LL33C: {
# 2414
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2416
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2413 "parse.y"
struct _tuple26*_tmp731=Cyc_yyget_YY39(yyyyvsp[1]);struct Cyc_List_List*_tmp733;int _tmp734;struct Cyc_Absyn_VarargInfo*_tmp735;void*_tmp736;struct Cyc_List_List*_tmp737;struct _tuple26*_tmp732=_tmp731;_tmp733=_tmp732->f1;_tmp734=_tmp732->f2;_tmp735=_tmp732->f3;_tmp736=_tmp732->f4;_tmp737=_tmp732->f5;
{struct Cyc_List_List*_tmpDAA;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpDA9;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpDA8;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpDA7;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpDA6;struct Cyc_Parse_Abstractdeclarator _tmpDA5;yyval=Cyc_YY30(((_tmpDA5.tms=((_tmpDAA=_region_malloc(yyr,sizeof(*_tmpDAA)),((_tmpDAA->hd=(void*)((_tmpDA6=_region_malloc(yyr,sizeof(*_tmpDA6)),((_tmpDA6[0]=((_tmpDA9.tag=3,((_tmpDA9.f1=(void*)((_tmpDA7=_region_malloc(yyr,sizeof(*_tmpDA7)),((_tmpDA7[0]=((_tmpDA8.tag=1,((_tmpDA8.f1=_tmp733,((_tmpDA8.f2=_tmp734,((_tmpDA8.f3=_tmp735,((_tmpDA8.f4=_tmp736,((_tmpDA8.f5=_tmp737,((_tmpDA8.f6=Cyc_yyget_YY55(yyyyvsp[3]),((_tmpDA8.f7=Cyc_yyget_YY55(yyyyvsp[4]),_tmpDA8)))))))))))))))),_tmpDA7)))),_tmpDA9)))),_tmpDA6)))),((_tmpDAA->tl=0,_tmpDAA)))))),_tmpDA5)));}
# 2416
break;}case 280: _LL33D: {
# 2418
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2420
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2417 "parse.y"
{struct Cyc_List_List*_tmpDBF;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpDBE;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpDBD;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpDBC;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpDBB;struct Cyc_Parse_Abstractdeclarator _tmpDBA;yyval=Cyc_YY30(((_tmpDBA.tms=((_tmpDBF=_region_malloc(yyr,sizeof(*_tmpDBF)),((_tmpDBF->hd=(void*)((_tmpDBB=_region_malloc(yyr,sizeof(*_tmpDBB)),((_tmpDBB[0]=((_tmpDBE.tag=3,((_tmpDBE.f1=(void*)((_tmpDBC=_region_malloc(yyr,sizeof(*_tmpDBC)),((_tmpDBC[0]=((_tmpDBD.tag=1,((_tmpDBD.f1=0,((_tmpDBD.f2=0,((_tmpDBD.f3=0,((_tmpDBD.f4=Cyc_yyget_YY49(yyyyvsp[2]),((_tmpDBD.f5=Cyc_yyget_YY50(yyyyvsp[3]),((_tmpDBD.f6=Cyc_yyget_YY55(yyyyvsp[5]),((_tmpDBD.f7=Cyc_yyget_YY55(yyyyvsp[6]),_tmpDBD)))))))))))))))),_tmpDBC)))),_tmpDBE)))),_tmpDBB)))),((_tmpDBF->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpDBF)))))),_tmpDBA)));}
# 2420
break;}case 281: _LL33E: {
# 2422
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2424
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2421 "parse.y"
struct _tuple26*_tmp744=Cyc_yyget_YY39(yyyyvsp[2]);struct Cyc_List_List*_tmp746;int _tmp747;struct Cyc_Absyn_VarargInfo*_tmp748;void*_tmp749;struct Cyc_List_List*_tmp74A;struct _tuple26*_tmp745=_tmp744;_tmp746=_tmp745->f1;_tmp747=_tmp745->f2;_tmp748=_tmp745->f3;_tmp749=_tmp745->f4;_tmp74A=_tmp745->f5;
{struct Cyc_List_List*_tmpDD4;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpDD3;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpDD2;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpDD1;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpDD0;struct Cyc_Parse_Abstractdeclarator _tmpDCF;yyval=Cyc_YY30(((_tmpDCF.tms=((_tmpDD4=_region_malloc(yyr,sizeof(*_tmpDD4)),((_tmpDD4->hd=(void*)((_tmpDD0=_region_malloc(yyr,sizeof(*_tmpDD0)),((_tmpDD0[0]=((_tmpDD3.tag=3,((_tmpDD3.f1=(void*)((_tmpDD1=_region_malloc(yyr,sizeof(*_tmpDD1)),((_tmpDD1[0]=((_tmpDD2.tag=1,((_tmpDD2.f1=_tmp746,((_tmpDD2.f2=_tmp747,((_tmpDD2.f3=_tmp748,((_tmpDD2.f4=_tmp749,((_tmpDD2.f5=_tmp74A,((_tmpDD2.f6=
Cyc_yyget_YY55(yyyyvsp[4]),((_tmpDD2.f7=Cyc_yyget_YY55(yyyyvsp[5]),_tmpDD2)))))))))))))))),_tmpDD1)))),_tmpDD3)))),_tmpDD0)))),((_tmpDD4->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpDD4)))))),_tmpDCF)));}
# 2425
break;}case 282: _LL33F: {
# 2427
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2429
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2427 "parse.y"
struct Cyc_List_List*_tmp751=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));
{struct Cyc_List_List*_tmpDDE;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpDDD;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpDDC;struct Cyc_Parse_Abstractdeclarator _tmpDDB;yyval=Cyc_YY30(((_tmpDDB.tms=((_tmpDDE=_region_malloc(yyr,sizeof(*_tmpDDE)),((_tmpDDE->hd=(void*)((_tmpDDC=_region_malloc(yyr,sizeof(*_tmpDDC)),((_tmpDDC[0]=((_tmpDDD.tag=4,((_tmpDDD.f1=_tmp751,((_tmpDDD.f2=(unsigned int)(yyyylsp[1]).first_line,((_tmpDDD.f3=0,_tmpDDD)))))))),_tmpDDC)))),((_tmpDDE->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpDDE)))))),_tmpDDB)));}
# 2431
break;}case 283: _LL340: {
# 2433
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2435
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2432 "parse.y"
{struct Cyc_List_List*_tmpDE8;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpDE7;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpDE6;struct Cyc_Parse_Abstractdeclarator _tmpDE5;yyval=Cyc_YY30(((_tmpDE5.tms=((_tmpDE8=_region_malloc(yyr,sizeof(*_tmpDE8)),((_tmpDE8->hd=(void*)((_tmpDE6=_region_malloc(yyr,sizeof(*_tmpDE6)),((_tmpDE6[0]=((_tmpDE7.tag=5,((_tmpDE7.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpDE7.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpDE7)))))),_tmpDE6)))),((_tmpDE8->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpDE8)))))),_tmpDE5)));}
# 2434
break;}case 284: _LL341: {
# 2436
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2438
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2438 "parse.y"
yyval=yyyyvsp[0];
break;}case 285: _LL342: {
# 2441
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2443
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2439 "parse.y"
yyval=yyyyvsp[0];
break;}case 286: _LL343: {
# 2442
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2444
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2440 "parse.y"
yyval=yyyyvsp[0];
break;}case 287: _LL344: {
# 2443
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2445
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2441 "parse.y"
yyval=yyyyvsp[0];
break;}case 288: _LL345: {
# 2444
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2446
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2442 "parse.y"
yyval=yyyyvsp[0];
break;}case 289: _LL346: {
# 2445
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2447
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2443 "parse.y"
yyval=yyyyvsp[0];
break;}case 290: _LL347: {
# 2446
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2448
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2446 "parse.y"
{struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct _tmpDEB;struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmpDEA;yyval=Cyc_Stmt_tok(Cyc_Absyn_new_stmt((void*)((_tmpDEA=_cycalloc(sizeof(*_tmpDEA)),((_tmpDEA[0]=((_tmpDEB.tag=16,((_tmpDEB.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),_tmpDEB)))),_tmpDEA)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 291: _LL348: {
# 2449
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2451
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpDF1;struct _dyneither_ptr*_tmpDF0;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpDEF;yyval=Cyc_Stmt_tok(Cyc_Absyn_new_stmt((void*)((_tmpDEF=_cycalloc(sizeof(*_tmpDEF)),((_tmpDEF[0]=((_tmpDF1.tag=13,((_tmpDF1.f1=((_tmpDF0=_cycalloc(sizeof(*_tmpDF0)),((_tmpDF0[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDF0)))),((_tmpDF1.f2=Cyc_yyget_Stmt_tok(yyyyvsp[2]),_tmpDF1)))))),_tmpDEF)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 292: _LL349: {
# 2455
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2457
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2456 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 293: _LL34A: {
# 2459
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2461
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2457 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_exp_stmt(Cyc_yyget_Exp_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 294: _LL34B: {
# 2460
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2462
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2462 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 295: _LL34C: {
# 2465
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2467
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2463 "parse.y"
yyval=yyyyvsp[1];
break;}case 296: _LL34D: {
# 2466
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2468
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2468 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(Cyc_yyget_YY16(yyyyvsp[0]),Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line)));
break;}case 297: _LL34E: {
# 2471
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2473
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2469 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(Cyc_yyget_YY16(yyyyvsp[0]),Cyc_yyget_Stmt_tok(yyyyvsp[1])));
break;}case 298: _LL34F: {
# 2472
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2474
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2470 "parse.y"
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpDF7;struct _dyneither_ptr*_tmpDF6;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpDF5;yyval=Cyc_Stmt_tok(Cyc_Absyn_new_stmt((void*)((_tmpDF5=_cycalloc(sizeof(*_tmpDF5)),((_tmpDF5[0]=((_tmpDF7.tag=13,((_tmpDF7.f1=((_tmpDF6=_cycalloc(sizeof(*_tmpDF6)),((_tmpDF6[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDF6)))),((_tmpDF7.f2=Cyc_Parse_flatten_declarations(Cyc_yyget_YY16(yyyyvsp[2]),Cyc_Absyn_skip_stmt(0)),_tmpDF7)))))),_tmpDF5)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2472
break;}case 299: _LL350: {
# 2474
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2476
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2472 "parse.y"
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpDFD;struct _dyneither_ptr*_tmpDFC;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpDFB;yyval=Cyc_Stmt_tok(Cyc_Absyn_new_stmt((void*)((_tmpDFB=_cycalloc(sizeof(*_tmpDFB)),((_tmpDFB[0]=((_tmpDFD.tag=13,((_tmpDFD.f1=((_tmpDFC=_cycalloc(sizeof(*_tmpDFC)),((_tmpDFC[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDFC)))),((_tmpDFD.f2=Cyc_Parse_flatten_declarations(Cyc_yyget_YY16(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[3])),_tmpDFD)))))),_tmpDFB)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2474
break;}case 300: _LL351: {
# 2476
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2478
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2474 "parse.y"
yyval=yyyyvsp[0];
break;}case 301: _LL352: {
# 2477
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2479
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2475 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_seq_stmt(Cyc_yyget_Stmt_tok(yyyyvsp[0]),Cyc_yyget_Stmt_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 302: _LL353: {
# 2478
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2480
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2476 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpE00;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpDFF;yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpDFF=_cycalloc(sizeof(*_tmpDFF)),((_tmpDFF[0]=((_tmpE00.tag=1,((_tmpE00.f1=Cyc_yyget_YY15(yyyyvsp[0]),_tmpE00)))),_tmpDFF)))),(unsigned int)(yyyylsp[0]).first_line),
Cyc_Absyn_skip_stmt(0)));}
break;}case 303: _LL354: {
# 2480
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2482
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2479 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpE03;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpE02;yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpE02=_cycalloc(sizeof(*_tmpE02)),((_tmpE02[0]=((_tmpE03.tag=1,((_tmpE03.f1=Cyc_yyget_YY15(yyyyvsp[0]),_tmpE03)))),_tmpE02)))),(unsigned int)(yyyylsp[0]).first_line),Cyc_yyget_Stmt_tok(yyyyvsp[1])));}
break;}case 304: _LL355: {
# 2482
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2484
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2484 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[4]),Cyc_Absyn_skip_stmt(0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 305: _LL356: {
# 2487
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2489
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2486 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[4]),Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 306: _LL357: {
# 2489
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2491
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
yyval=Cyc_Stmt_tok(Cyc_Absyn_switch_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_YY8(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 307: _LL358: {
# 2495
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2497
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2495 "parse.y"
struct Cyc_Absyn_Exp*_tmp769=Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Absyn_switch_stmt(_tmp769,Cyc_yyget_YY8(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 308: _LL359: {
# 2499
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2501
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2499 "parse.y"
struct Cyc_Absyn_Exp*_tmp76A=Cyc_Absyn_tuple_exp(Cyc_yyget_YY4(yyyyvsp[3]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Absyn_switch_stmt(_tmp76A,Cyc_yyget_YY8(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
# 2502
break;}case 309: _LL35A: {
# 2504
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2506
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2505 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_trycatch_stmt(Cyc_yyget_Stmt_tok(yyyyvsp[1]),Cyc_yyget_YY8(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 310: _LL35B:
# 2508
 yyval=Cyc_YY8(0);
break;case 311: _LL35C: {
# 2511
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2513
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2521 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpE06;struct Cyc_List_List*_tmpE05;yyval=Cyc_YY8(((_tmpE05=_cycalloc(sizeof(*_tmpE05)),((_tmpE05->hd=((_tmpE06=_cycalloc(sizeof(*_tmpE06)),((_tmpE06->pattern=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line),((_tmpE06->pat_vars=0,((_tmpE06->where_clause=0,((_tmpE06->body=
Cyc_yyget_Stmt_tok(yyyyvsp[2]),((_tmpE06->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpE06)))))))))))),((_tmpE05->tl=0,_tmpE05)))))));}
# 2524
break;}case 312: _LL35D: {
# 2526
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2528
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2525 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpE09;struct Cyc_List_List*_tmpE08;yyval=Cyc_YY8(((_tmpE08=_cycalloc(sizeof(*_tmpE08)),((_tmpE08->hd=((_tmpE09=_cycalloc(sizeof(*_tmpE09)),((_tmpE09->pattern=Cyc_yyget_YY9(yyyyvsp[1]),((_tmpE09->pat_vars=0,((_tmpE09->where_clause=0,((_tmpE09->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[2]).first_line),((_tmpE09->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpE09)))))))))))),((_tmpE08->tl=
Cyc_yyget_YY8(yyyyvsp[3]),_tmpE08)))))));}
break;}case 313: _LL35E: {
# 2530
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2532
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2529 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpE0C;struct Cyc_List_List*_tmpE0B;yyval=Cyc_YY8(((_tmpE0B=_cycalloc(sizeof(*_tmpE0B)),((_tmpE0B->hd=((_tmpE0C=_cycalloc(sizeof(*_tmpE0C)),((_tmpE0C->pattern=Cyc_yyget_YY9(yyyyvsp[1]),((_tmpE0C->pat_vars=0,((_tmpE0C->where_clause=0,((_tmpE0C->body=Cyc_yyget_Stmt_tok(yyyyvsp[3]),((_tmpE0C->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpE0C)))))))))))),((_tmpE0B->tl=Cyc_yyget_YY8(yyyyvsp[4]),_tmpE0B)))))));}
break;}case 314: _LL35F: {
# 2532
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2534
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2531 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpE0F;struct Cyc_List_List*_tmpE0E;yyval=Cyc_YY8(((_tmpE0E=_cycalloc(sizeof(*_tmpE0E)),((_tmpE0E->hd=((_tmpE0F=_cycalloc(sizeof(*_tmpE0F)),((_tmpE0F->pattern=Cyc_yyget_YY9(yyyyvsp[1]),((_tmpE0F->pat_vars=0,((_tmpE0F->where_clause=Cyc_yyget_Exp_tok(yyyyvsp[3]),((_tmpE0F->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[4]).first_line),((_tmpE0F->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpE0F)))))))))))),((_tmpE0E->tl=
Cyc_yyget_YY8(yyyyvsp[5]),_tmpE0E)))))));}
break;}case 315: _LL360: {
# 2536
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2538
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2535 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpE12;struct Cyc_List_List*_tmpE11;yyval=Cyc_YY8(((_tmpE11=_cycalloc(sizeof(*_tmpE11)),((_tmpE11->hd=((_tmpE12=_cycalloc(sizeof(*_tmpE12)),((_tmpE12->pattern=Cyc_yyget_YY9(yyyyvsp[1]),((_tmpE12->pat_vars=0,((_tmpE12->where_clause=Cyc_yyget_Exp_tok(yyyyvsp[3]),((_tmpE12->body=Cyc_yyget_Stmt_tok(yyyyvsp[5]),((_tmpE12->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpE12)))))))))))),((_tmpE11->tl=Cyc_yyget_YY8(yyyyvsp[6]),_tmpE11)))))));}
break;}case 316: _LL361: {
# 2538
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2540
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2542 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_while_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 317: _LL362: {
# 2545
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2547
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2546 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_do_stmt(Cyc_yyget_Stmt_tok(yyyyvsp[1]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 318: _LL363: {
# 2549
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2551
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2550 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 319: _LL364: {
# 2554
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2556
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2553 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[4]),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 320: _LL365: {
# 2557
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2559
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2556 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 321: _LL366: {
# 2560
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2562
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2559 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[3]),Cyc_yyget_Exp_tok(yyyyvsp[5]),
Cyc_yyget_Stmt_tok(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 322: _LL367: {
# 2563
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2565
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2562 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 323: _LL368: {
# 2566
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2568
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2565 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[5]),
Cyc_yyget_Stmt_tok(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 324: _LL369: {
# 2569
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2571
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2568 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 325: _LL36A: {
# 2572
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 2574
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 2571 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),Cyc_yyget_Exp_tok(yyyyvsp[6]),
Cyc_yyget_Stmt_tok(yyyyvsp[8]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 326: _LL36B: {
# 2575
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2577
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2574 "parse.y"
struct Cyc_List_List*_tmp775=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp776=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp775,_tmp776));
# 2579
break;}case 327: _LL36C: {
# 2581
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2583
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2580 "parse.y"
struct Cyc_List_List*_tmp777=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp778=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp777,_tmp778));
# 2585
break;}case 328: _LL36D: {
# 2587
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2589
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2586 "parse.y"
struct Cyc_List_List*_tmp779=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp77A=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[4]),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp779,_tmp77A));
# 2591
break;}case 329: _LL36E: {
# 2593
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2595
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2592 "parse.y"
struct Cyc_List_List*_tmp77B=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp77C=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[3]),Cyc_yyget_Exp_tok(yyyyvsp[5]),
Cyc_yyget_Stmt_tok(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp77B,_tmp77C));
# 2597
break;}case 330: _LL36F: {
# 2599
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2601
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
{struct _dyneither_ptr*_tmpE13;yyval=Cyc_Stmt_tok(Cyc_Absyn_goto_stmt(((_tmpE13=_cycalloc(sizeof(*_tmpE13)),((_tmpE13[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpE13)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 331: _LL370: {
# 2605
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2607
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2603 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_continue_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 332: _LL371: {
# 2606
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2608
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2604 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_break_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 333: _LL372: {
# 2607
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2609
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2605 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 334: _LL373: {
# 2608
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2610
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2606 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 335: _LL374: {
# 2609
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2611
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2608 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 336: _LL375: {
# 2611
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2613
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2609 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 337: _LL376: {
# 2612
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2614
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2611 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 338: _LL377: {
# 2614
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2616
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2620 "parse.y"
yyval=yyyyvsp[0];
break;}case 339: _LL378: {
# 2623
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2625
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2623 "parse.y"
yyval=yyyyvsp[0];
break;}case 340: _LL379: {
# 2626
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2628
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2625 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 341: _LL37A: {
# 2628
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2630
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2628 "parse.y"
yyval=yyyyvsp[0];
break;}case 342: _LL37B: {
# 2631
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2633
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2630 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 343: _LL37C: {
# 2633
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2635
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2633 "parse.y"
yyval=yyyyvsp[0];
break;}case 344: _LL37D: {
# 2636
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2638
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2635 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 345: _LL37E: {
# 2638
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2640
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2638 "parse.y"
yyval=yyyyvsp[0];
break;}case 346: _LL37F: {
# 2641
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2643
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2640 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 347: _LL380: {
# 2643
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2645
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2643 "parse.y"
yyval=yyyyvsp[0];
break;}case 348: _LL381: {
# 2646
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2648
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2645 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 349: _LL382: {
# 2648
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2650
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2648 "parse.y"
yyval=yyyyvsp[0];
break;}case 350: _LL383: {
# 2651
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2653
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2650 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 351: _LL384: {
# 2653
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2655
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2653 "parse.y"
yyval=yyyyvsp[0];
break;}case 352: _LL385: {
# 2656
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2658
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2655 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 353: _LL386: {
# 2658
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2660
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2657 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 354: _LL387: {
# 2660
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2662
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2660 "parse.y"
yyval=yyyyvsp[0];
break;}case 355: _LL388: {
# 2663
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2665
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2662 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 356: _LL389: {
# 2665
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2667
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2664 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 357: _LL38A: {
# 2667
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2669
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2666 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 358: _LL38B: {
# 2669
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2671
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2668 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 359: _LL38C: {
# 2671
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2673
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2671 "parse.y"
yyval=yyyyvsp[0];
break;}case 360: _LL38D: {
# 2674
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2676
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2673 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 361: _LL38E: {
# 2676
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2678
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2675 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 362: _LL38F: {
# 2678
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2680
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2678 "parse.y"
yyval=yyyyvsp[0];
break;}case 363: _LL390: {
# 2681
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2683
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2680 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 364: _LL391: {
# 2683
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2685
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2682 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 365: _LL392: {
# 2685
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2687
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2685 "parse.y"
yyval=yyyyvsp[0];
break;}case 366: _LL393: {
# 2688
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2690
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2687 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 367: _LL394: {
# 2690
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2692
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2689 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 368: _LL395: {
# 2692
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2694
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2691 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 369: _LL396: {
# 2694
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2696
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2694 "parse.y"
yyval=yyyyvsp[0];
break;}case 370: _LL397: {
# 2697
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2699
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2696 "parse.y"
void*_tmp77E=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp(_tmp77E,Cyc_yyget_Exp_tok(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line)));
# 2699
break;}case 371: _LL398: {
# 2701
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2703
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2702 "parse.y"
yyval=yyyyvsp[0];
break;}case 372: _LL399: {
# 2705
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2707
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2705 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(yyyyvsp[1])));
break;}case 373: _LL39A: {
# 2708
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2710
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2707 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim1_exp(Cyc_yyget_YY6(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 374: _LL39B: {
# 2710
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2712
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2709 "parse.y"
void*_tmp77F=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp(_tmp77F,(unsigned int)(yyyylsp[0]).first_line)));
# 2712
break;}case 375: _LL39C: {
# 2714
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2716
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2713 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 376: _LL39D: {
# 2716
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2718
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2715 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyyyvsp[2])).f3,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4])),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 377: _LL39E: {
# 2718
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2720
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2720 "parse.y"
yyval=yyyyvsp[0];
break;}case 378: _LL39F: {
# 2723
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2725
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2729 "parse.y"
yyval=yyyyvsp[0];
break;}case 379: _LL3A0: {
# 2732
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2734
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2734 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line));
break;}case 380: _LL3A1: {
# 2737
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2739
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2736 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(yyyyvsp[1])));
break;}case 381: _LL3A2: {
# 2739
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2741
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2738 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(yyyyvsp[0]);
{void*_tmp780=e->r;void*_tmp781=_tmp780;enum Cyc_Absyn_Sign _tmp783;char _tmp784;enum Cyc_Absyn_Sign _tmp786;short _tmp787;enum Cyc_Absyn_Sign _tmp789;int _tmp78A;struct _dyneither_ptr _tmp78C;int _tmp78D;_LL3A5: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp782=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp781;if(_tmp782->tag != 0)goto _LL3A7;else{if(((_tmp782->f1).Char_c).tag != 2)goto _LL3A7;_tmp783=((struct _tuple3)((_tmp782->f1).Char_c).val).f1;_tmp784=((struct _tuple3)((_tmp782->f1).Char_c).val).f2;}}_LL3A6:
# 2742
{struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct _tmpE16;struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmpE15;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE15=_cycalloc_atomic(sizeof(*_tmpE15)),((_tmpE15[0]=((_tmpE16.tag=11,((_tmpE16.f1=_tmp784,_tmpE16)))),_tmpE15)))),e->loc));}goto _LL3A4;_LL3A7: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp785=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp781;if(_tmp785->tag != 0)goto _LL3A9;else{if(((_tmp785->f1).Short_c).tag != 4)goto _LL3A9;_tmp786=((struct _tuple4)((_tmp785->f1).Short_c).val).f1;_tmp787=((struct _tuple4)((_tmp785->f1).Short_c).val).f2;}}_LL3A8:
# 2744
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpE19;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpE18;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE18=_cycalloc_atomic(sizeof(*_tmpE18)),((_tmpE18[0]=((_tmpE19.tag=10,((_tmpE19.f1=_tmp786,((_tmpE19.f2=(int)_tmp787,_tmpE19)))))),_tmpE18)))),e->loc));}goto _LL3A4;_LL3A9: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp788=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp781;if(_tmp788->tag != 0)goto _LL3AB;else{if(((_tmp788->f1).Int_c).tag != 5)goto _LL3AB;_tmp789=((struct _tuple5)((_tmp788->f1).Int_c).val).f1;_tmp78A=((struct _tuple5)((_tmp788->f1).Int_c).val).f2;}}_LL3AA:
# 2746
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpE1C;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpE1B;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE1B=_cycalloc_atomic(sizeof(*_tmpE1B)),((_tmpE1B[0]=((_tmpE1C.tag=10,((_tmpE1C.f1=_tmp789,((_tmpE1C.f2=_tmp78A,_tmpE1C)))))),_tmpE1B)))),e->loc));}goto _LL3A4;_LL3AB: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp78B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp781;if(_tmp78B->tag != 0)goto _LL3AD;else{if(((_tmp78B->f1).Float_c).tag != 7)goto _LL3AD;_tmp78C=((struct _tuple7)((_tmp78B->f1).Float_c).val).f1;_tmp78D=((struct _tuple7)((_tmp78B->f1).Float_c).val).f2;}}_LL3AC:
# 2748
{struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct _tmpE1F;struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmpE1E;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE1E=_cycalloc(sizeof(*_tmpE1E)),((_tmpE1E[0]=((_tmpE1F.tag=12,((_tmpE1F.f1=_tmp78C,((_tmpE1F.f2=_tmp78D,_tmpE1F)))))),_tmpE1E)))),e->loc));}goto _LL3A4;_LL3AD: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp78E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp781;if(_tmp78E->tag != 0)goto _LL3AF;else{if(((_tmp78E->f1).Null_c).tag != 1)goto _LL3AF;}}_LL3AE:
# 2750
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL3A4;_LL3AF: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp78F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp781;if(_tmp78F->tag != 0)goto _LL3B1;else{if(((_tmp78F->f1).String_c).tag != 8)goto _LL3B1;}}_LL3B0:
# 2752
{const char*_tmpE20;Cyc_Parse_err(((_tmpE20="strings cannot occur within patterns",_tag_dyneither(_tmpE20,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3A4;_LL3B1: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp790=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp781;if(_tmp790->tag != 0)goto _LL3B3;else{if(((_tmp790->f1).Wstring_c).tag != 9)goto _LL3B3;}}_LL3B2:
# 2754
{const char*_tmpE21;Cyc_Parse_err(((_tmpE21="strings cannot occur within patterns",_tag_dyneither(_tmpE21,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3A4;_LL3B3: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp791=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp781;if(_tmp791->tag != 0)goto _LL3B5;else{if(((_tmp791->f1).LongLong_c).tag != 6)goto _LL3B5;}}_LL3B4:
# 2756
{const char*_tmpE22;Cyc_Parse_unimp(((_tmpE22="long long's in patterns",_tag_dyneither(_tmpE22,sizeof(char),24))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3A4;_LL3B5:;_LL3B6: {
# 2758
const char*_tmpE23;Cyc_Parse_err(((_tmpE23="bad constant in case",_tag_dyneither(_tmpE23,sizeof(char),21))),(unsigned int)(yyyylsp[0]).first_line);}_LL3A4:;}
# 2761
break;}case 382: _LL3A3: {
# 2763
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2765
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2762 "parse.y"
{struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct _tmpE26;struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmpE25;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE25=_cycalloc(sizeof(*_tmpE25)),((_tmpE25[0]=((_tmpE26.tag=15,((_tmpE26.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),_tmpE26)))),_tmpE25)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 383: _LL3B7: {
# 2765
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2767
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2764 "parse.y"
{const char*_tmpE27;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpE27="as",_tag_dyneither(_tmpE27,sizeof(char),3))))!= 0){
const char*_tmpE28;Cyc_Parse_err(((_tmpE28="expecting `as'",_tag_dyneither(_tmpE28,sizeof(char),15))),(unsigned int)(yyyylsp[1]).first_line);}}
{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpE32;struct _dyneither_ptr*_tmpE31;struct _tuple0*_tmpE30;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpE2F;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE2F=_cycalloc(sizeof(*_tmpE2F)),((_tmpE2F[0]=((_tmpE32.tag=1,((_tmpE32.f1=Cyc_Absyn_new_vardecl(((_tmpE30=_cycalloc(sizeof(*_tmpE30)),((_tmpE30->f1=Cyc_Absyn_Loc_n,((_tmpE30->f2=((_tmpE31=_cycalloc(sizeof(*_tmpE31)),((_tmpE31[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE31)))),_tmpE30)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpE32.f2=
Cyc_yyget_YY9(yyyyvsp[2]),_tmpE32)))))),_tmpE2F)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2769
break;}case 384: _LL3B8: {
# 2771
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2773
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2770 "parse.y"
{const char*_tmpE33;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]),((_tmpE33="alias",_tag_dyneither(_tmpE33,sizeof(char),6))))!= 0){
const char*_tmpE34;Cyc_Parse_err(((_tmpE34="expecting `alias'",_tag_dyneither(_tmpE34,sizeof(char),18))),(unsigned int)(yyyylsp[1]).first_line);}}{
int _tmp7A8=(yyyylsp[0]).first_line;
struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpE3C;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmpE3B;struct _dyneither_ptr*_tmpE3A;struct Cyc_Absyn_Tvar*_tmpE39;struct Cyc_Absyn_Tvar*tv=(_tmpE39=_cycalloc(sizeof(*_tmpE39)),((_tmpE39->name=((_tmpE3A=_cycalloc(sizeof(*_tmpE3A)),((_tmpE3A[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpE3A)))),((_tmpE39->identity=- 1,((_tmpE39->kind=(void*)((_tmpE3C=_cycalloc(sizeof(*_tmpE3C)),((_tmpE3C[0]=((_tmpE3B.tag=0,((_tmpE3B.f1=& Cyc_Tcutil_rk,_tmpE3B)))),_tmpE3C)))),_tmpE39)))))));
struct _dyneither_ptr*_tmpE3F;struct _tuple0*_tmpE3E;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpE3E=_cycalloc(sizeof(*_tmpE3E)),((_tmpE3E->f1=Cyc_Absyn_Loc_n,((_tmpE3E->f2=((_tmpE3F=_cycalloc(sizeof(*_tmpE3F)),((_tmpE3F[0]=Cyc_yyget_String_tok(yyyyvsp[5]),_tmpE3F)))),_tmpE3E)))))),
Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[4]),(unsigned int)(yyyylsp[4]).first_line),0);
{struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct _tmpE42;struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmpE41;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE41=_cycalloc(sizeof(*_tmpE41)),((_tmpE41[0]=((_tmpE42.tag=2,((_tmpE42.f1=tv,((_tmpE42.f2=vd,_tmpE42)))))),_tmpE41)))),(unsigned int)_tmp7A8));}
# 2778
break;};}case 385: _LL3B9: {
# 2780
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2782
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2779 "parse.y"
struct _tuple22 _tmp7B1=*Cyc_yyget_YY10(yyyyvsp[2]);struct Cyc_List_List*_tmp7B3;int _tmp7B4;struct _tuple22 _tmp7B2=_tmp7B1;_tmp7B3=_tmp7B2.f1;_tmp7B4=_tmp7B2.f2;
{struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct _tmpE45;struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmpE44;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE44=_cycalloc(sizeof(*_tmpE44)),((_tmpE44[0]=((_tmpE45.tag=5,((_tmpE45.f1=_tmp7B3,((_tmpE45.f2=_tmp7B4,_tmpE45)))))),_tmpE44)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2782
break;}case 386: _LL3BA: {
# 2784
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2786
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2783 "parse.y"
struct _tuple22 _tmp7B7=*Cyc_yyget_YY10(yyyyvsp[2]);struct Cyc_List_List*_tmp7B9;int _tmp7BA;struct _tuple22 _tmp7B8=_tmp7B7;_tmp7B9=_tmp7B8.f1;_tmp7BA=_tmp7B8.f2;
{struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct _tmpE48;struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmpE47;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE47=_cycalloc(sizeof(*_tmpE47)),((_tmpE47[0]=((_tmpE48.tag=16,((_tmpE48.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpE48.f2=_tmp7B9,((_tmpE48.f3=_tmp7BA,_tmpE48)))))))),_tmpE47)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2786
break;}case 387: _LL3BB: {
# 2788
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2790
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2787 "parse.y"
struct _tuple22 _tmp7BD=*Cyc_yyget_YY14(yyyyvsp[3]);struct Cyc_List_List*_tmp7BF;int _tmp7C0;struct _tuple22 _tmp7BE=_tmp7BD;_tmp7BF=_tmp7BE.f1;_tmp7C0=_tmp7BE.f2;{
struct Cyc_List_List*_tmp7C1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY40(yyyyvsp[2]));
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpE4E;struct Cyc_Absyn_AggrInfo*_tmpE4D;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpE4C;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE4C=_cycalloc(sizeof(*_tmpE4C)),((_tmpE4C[0]=((_tmpE4E.tag=7,((_tmpE4E.f1=((_tmpE4D=_cycalloc(sizeof(*_tmpE4D)),((_tmpE4D->aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(yyyyvsp[0]),0),((_tmpE4D->targs=0,_tmpE4D)))))),((_tmpE4E.f2=_tmp7C1,((_tmpE4E.f3=_tmp7BF,((_tmpE4E.f4=_tmp7C0,_tmpE4E)))))))))),_tmpE4C)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2792
break;};}case 388: _LL3BC: {
# 2794
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2796
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2793 "parse.y"
struct _tuple22 _tmp7C5=*Cyc_yyget_YY14(yyyyvsp[2]);struct Cyc_List_List*_tmp7C7;int _tmp7C8;struct _tuple22 _tmp7C6=_tmp7C5;_tmp7C7=_tmp7C6.f1;_tmp7C8=_tmp7C6.f2;{
struct Cyc_List_List*_tmp7C9=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,Cyc_yyget_YY40(yyyyvsp[1]));
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpE51;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpE50;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE50=_cycalloc(sizeof(*_tmpE50)),((_tmpE50[0]=((_tmpE51.tag=7,((_tmpE51.f1=0,((_tmpE51.f2=_tmp7C9,((_tmpE51.f3=_tmp7C7,((_tmpE51.f4=_tmp7C8,_tmpE51)))))))))),_tmpE50)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2797
break;};}case 389: _LL3BD: {
# 2799
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2801
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2798 "parse.y"
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE54;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpE53;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE53=_cycalloc(sizeof(*_tmpE53)),((_tmpE53[0]=((_tmpE54.tag=6,((_tmpE54.f1=Cyc_yyget_YY9(yyyyvsp[1]),_tmpE54)))),_tmpE53)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 390: _LL3BE: {
# 2801
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2803
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2800 "parse.y"
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE5E;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE5D;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpE5C;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpE5B;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE5B=_cycalloc(sizeof(*_tmpE5B)),((_tmpE5B[0]=((_tmpE5E.tag=6,((_tmpE5E.f1=Cyc_Absyn_new_pat((void*)((_tmpE5C=_cycalloc(sizeof(*_tmpE5C)),((_tmpE5C[0]=((_tmpE5D.tag=6,((_tmpE5D.f1=Cyc_yyget_YY9(yyyyvsp[1]),_tmpE5D)))),_tmpE5C)))),(unsigned int)(yyyylsp[0]).first_line),_tmpE5E)))),_tmpE5B)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 391: _LL3BF: {
# 2803
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2805
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2802 "parse.y"
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpE68;struct _dyneither_ptr*_tmpE67;struct _tuple0*_tmpE66;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpE65;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE65=_cycalloc(sizeof(*_tmpE65)),((_tmpE65[0]=((_tmpE68.tag=3,((_tmpE68.f1=Cyc_Absyn_new_vardecl(((_tmpE66=_cycalloc(sizeof(*_tmpE66)),((_tmpE66->f1=Cyc_Absyn_Loc_n,((_tmpE66->f2=((_tmpE67=_cycalloc(sizeof(*_tmpE67)),((_tmpE67[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpE67)))),_tmpE66)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpE68.f2=
# 2804
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[1]).first_line),_tmpE68)))))),_tmpE65)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2806
break;}case 392: _LL3C0: {
# 2808
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2810
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2807 "parse.y"
{const char*_tmpE69;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[2]),((_tmpE69="as",_tag_dyneither(_tmpE69,sizeof(char),3))))!= 0){
const char*_tmpE6A;Cyc_Parse_err(((_tmpE6A="expecting `as'",_tag_dyneither(_tmpE6A,sizeof(char),15))),(unsigned int)(yyyylsp[2]).first_line);}}
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpE74;struct _dyneither_ptr*_tmpE73;struct _tuple0*_tmpE72;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpE71;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE71=_cycalloc(sizeof(*_tmpE71)),((_tmpE71[0]=((_tmpE74.tag=3,((_tmpE74.f1=Cyc_Absyn_new_vardecl(((_tmpE72=_cycalloc(sizeof(*_tmpE72)),((_tmpE72->f1=Cyc_Absyn_Loc_n,((_tmpE72->f2=((_tmpE73=_cycalloc(sizeof(*_tmpE73)),((_tmpE73[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpE73)))),_tmpE72)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpE74.f2=
# 2811
Cyc_yyget_YY9(yyyyvsp[3]),_tmpE74)))))),_tmpE71)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2813
break;}case 393: _LL3C1: {
# 2815
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2817
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2814 "parse.y"
void*_tmp7DC=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[2]),Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpE85;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpE84;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE83;struct _dyneither_ptr*_tmpE82;struct _tuple0*_tmpE81;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpE80;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE80=_cycalloc(sizeof(*_tmpE80)),((_tmpE80[0]=((_tmpE85.tag=4,((_tmpE85.f1=Cyc_Parse_typ2tvar((unsigned int)(yyyylsp[2]).first_line,_tmp7DC),((_tmpE85.f2=
Cyc_Absyn_new_vardecl(((_tmpE81=_cycalloc(sizeof(*_tmpE81)),((_tmpE81->f1=Cyc_Absyn_Loc_n,((_tmpE81->f2=((_tmpE82=_cycalloc(sizeof(*_tmpE82)),((_tmpE82[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE82)))),_tmpE81)))))),(void*)(
(_tmpE83=_cycalloc(sizeof(*_tmpE83)),((_tmpE83[0]=((_tmpE84.tag=19,((_tmpE84.f1=_tmp7DC,_tmpE84)))),_tmpE83)))),0),_tmpE85)))))),_tmpE80)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2819
break;}case 394: _LL3C2: {
# 2821
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2823
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2820 "parse.y"
struct Cyc_Absyn_Tvar*_tmp7E3=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpEA1;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpEA0;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpE9F;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpE9E;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE9D;struct _dyneither_ptr*_tmpE9C;struct _tuple0*_tmpE9B;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpE9A;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE9A=_cycalloc(sizeof(*_tmpE9A)),((_tmpE9A[0]=((_tmpEA1.tag=4,((_tmpEA1.f1=_tmp7E3,((_tmpEA1.f2=
Cyc_Absyn_new_vardecl(((_tmpE9B=_cycalloc(sizeof(*_tmpE9B)),((_tmpE9B->f1=Cyc_Absyn_Loc_n,((_tmpE9B->f2=((_tmpE9C=_cycalloc(sizeof(*_tmpE9C)),((_tmpE9C[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE9C)))),_tmpE9B)))))),(void*)(
(_tmpE9D=_cycalloc(sizeof(*_tmpE9D)),((_tmpE9D[0]=((_tmpEA0.tag=19,((_tmpEA0.f1=(void*)((_tmpE9E=_cycalloc(sizeof(*_tmpE9E)),((_tmpE9E[0]=((_tmpE9F.tag=2,((_tmpE9F.f1=_tmp7E3,_tmpE9F)))),_tmpE9E)))),_tmpEA0)))),_tmpE9D)))),0),_tmpEA1)))))),_tmpE9A)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2825
break;}case 395: _LL3C3: {
# 2827
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2829
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2828 "parse.y"
{struct _tuple22*_tmpEA2;yyval=Cyc_YY10(((_tmpEA2=_cycalloc(sizeof(*_tmpEA2)),((_tmpEA2->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(yyyyvsp[0])),((_tmpEA2->f2=0,_tmpEA2)))))));}
break;}case 396: _LL3C4: {
# 2831
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2833
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2829 "parse.y"
{struct _tuple22*_tmpEA3;yyval=Cyc_YY10(((_tmpEA3=_cycalloc(sizeof(*_tmpEA3)),((_tmpEA3->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(yyyyvsp[0])),((_tmpEA3->f2=1,_tmpEA3)))))));}
break;}case 397: _LL3C5: {
# 2832
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2834
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2830 "parse.y"
{struct _tuple22*_tmpEA4;yyval=Cyc_YY10(((_tmpEA4=_cycalloc(sizeof(*_tmpEA4)),((_tmpEA4->f1=0,((_tmpEA4->f2=1,_tmpEA4)))))));}
break;}case 398: _LL3C6: {
# 2833
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2835
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2835 "parse.y"
{struct Cyc_List_List*_tmpEA5;yyval=Cyc_YY11(((_tmpEA5=_cycalloc(sizeof(*_tmpEA5)),((_tmpEA5->hd=Cyc_yyget_YY9(yyyyvsp[0]),((_tmpEA5->tl=0,_tmpEA5)))))));}
break;}case 399: _LL3C7: {
# 2838
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2840
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2837 "parse.y"
{struct Cyc_List_List*_tmpEA6;yyval=Cyc_YY11(((_tmpEA6=_cycalloc(sizeof(*_tmpEA6)),((_tmpEA6->hd=Cyc_yyget_YY9(yyyyvsp[2]),((_tmpEA6->tl=Cyc_yyget_YY11(yyyyvsp[0]),_tmpEA6)))))));}
break;}case 400: _LL3C8: {
# 2840
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2842
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2842 "parse.y"
{struct _tuple23*_tmpEA7;yyval=Cyc_YY12(((_tmpEA7=_cycalloc(sizeof(*_tmpEA7)),((_tmpEA7->f1=0,((_tmpEA7->f2=Cyc_yyget_YY9(yyyyvsp[0]),_tmpEA7)))))));}
break;}case 401: _LL3C9: {
# 2845
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2847
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2844 "parse.y"
{struct _tuple23*_tmpEA8;yyval=Cyc_YY12(((_tmpEA8=_cycalloc(sizeof(*_tmpEA8)),((_tmpEA8->f1=Cyc_yyget_YY41(yyyyvsp[0]),((_tmpEA8->f2=Cyc_yyget_YY9(yyyyvsp[1]),_tmpEA8)))))));}
break;}case 402: _LL3CA: {
# 2847
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2849
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2847 "parse.y"
{struct _tuple22*_tmpEA9;yyval=Cyc_YY14(((_tmpEA9=_cycalloc(sizeof(*_tmpEA9)),((_tmpEA9->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0])),((_tmpEA9->f2=0,_tmpEA9)))))));}
break;}case 403: _LL3CB: {
# 2850
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2852
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2848 "parse.y"
{struct _tuple22*_tmpEAA;yyval=Cyc_YY14(((_tmpEAA=_cycalloc(sizeof(*_tmpEAA)),((_tmpEAA->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0])),((_tmpEAA->f2=1,_tmpEAA)))))));}
break;}case 404: _LL3CC: {
# 2851
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2853
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2849 "parse.y"
{struct _tuple22*_tmpEAB;yyval=Cyc_YY14(((_tmpEAB=_cycalloc(sizeof(*_tmpEAB)),((_tmpEAB->f1=0,((_tmpEAB->f2=1,_tmpEAB)))))));}
break;}case 405: _LL3CD: {
# 2852
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2854
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2854 "parse.y"
{struct Cyc_List_List*_tmpEAC;yyval=Cyc_YY13(((_tmpEAC=_cycalloc(sizeof(*_tmpEAC)),((_tmpEAC->hd=Cyc_yyget_YY12(yyyyvsp[0]),((_tmpEAC->tl=0,_tmpEAC)))))));}
break;}case 406: _LL3CE: {
# 2857
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2859
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2856 "parse.y"
{struct Cyc_List_List*_tmpEAD;yyval=Cyc_YY13(((_tmpEAD=_cycalloc(sizeof(*_tmpEAD)),((_tmpEAD->hd=Cyc_yyget_YY12(yyyyvsp[2]),((_tmpEAD->tl=Cyc_yyget_YY13(yyyyvsp[0]),_tmpEAD)))))));}
break;}case 407: _LL3CF: {
# 2859
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2861
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=yyyyvsp[0];
break;}case 408: _LL3D0: {
# 2865
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2867
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2864 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_seq_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 409: _LL3D1: {
# 2867
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2869
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2869 "parse.y"
yyval=yyyyvsp[0];
break;}case 410: _LL3D2: {
# 2872
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2874
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2871 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_assignop_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_YY7(yyyyvsp[1]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 411: _LL3D3: {
# 2874
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2876
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2873 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_swap_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 412: _LL3D4: {
# 2876
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2878
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2877 "parse.y"
yyval=Cyc_YY7(0);
break;}case 413: _LL3D5: {
# 2880
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2882
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2878 "parse.y"
{struct Cyc_Core_Opt*_tmpEAE;yyval=Cyc_YY7(((_tmpEAE=_cycalloc_atomic(sizeof(*_tmpEAE)),((_tmpEAE->v=(void*)Cyc_Absyn_Times,_tmpEAE)))));}
break;}case 414: _LL3D6: {
# 2881
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2883
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2879 "parse.y"
{struct Cyc_Core_Opt*_tmpEAF;yyval=Cyc_YY7(((_tmpEAF=_cycalloc_atomic(sizeof(*_tmpEAF)),((_tmpEAF->v=(void*)Cyc_Absyn_Div,_tmpEAF)))));}
break;}case 415: _LL3D7: {
# 2882
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2884
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2880 "parse.y"
{struct Cyc_Core_Opt*_tmpEB0;yyval=Cyc_YY7(((_tmpEB0=_cycalloc_atomic(sizeof(*_tmpEB0)),((_tmpEB0->v=(void*)Cyc_Absyn_Mod,_tmpEB0)))));}
break;}case 416: _LL3D8: {
# 2883
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2885
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2881 "parse.y"
{struct Cyc_Core_Opt*_tmpEB1;yyval=Cyc_YY7(((_tmpEB1=_cycalloc_atomic(sizeof(*_tmpEB1)),((_tmpEB1->v=(void*)Cyc_Absyn_Plus,_tmpEB1)))));}
break;}case 417: _LL3D9: {
# 2884
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2886
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2882 "parse.y"
{struct Cyc_Core_Opt*_tmpEB2;yyval=Cyc_YY7(((_tmpEB2=_cycalloc_atomic(sizeof(*_tmpEB2)),((_tmpEB2->v=(void*)Cyc_Absyn_Minus,_tmpEB2)))));}
break;}case 418: _LL3DA: {
# 2885
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2887
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2883 "parse.y"
{struct Cyc_Core_Opt*_tmpEB3;yyval=Cyc_YY7(((_tmpEB3=_cycalloc_atomic(sizeof(*_tmpEB3)),((_tmpEB3->v=(void*)Cyc_Absyn_Bitlshift,_tmpEB3)))));}
break;}case 419: _LL3DB: {
# 2886
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2888
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2884 "parse.y"
{struct Cyc_Core_Opt*_tmpEB4;yyval=Cyc_YY7(((_tmpEB4=_cycalloc_atomic(sizeof(*_tmpEB4)),((_tmpEB4->v=(void*)Cyc_Absyn_Bitlrshift,_tmpEB4)))));}
break;}case 420: _LL3DC: {
# 2887
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2889
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2885 "parse.y"
{struct Cyc_Core_Opt*_tmpEB5;yyval=Cyc_YY7(((_tmpEB5=_cycalloc_atomic(sizeof(*_tmpEB5)),((_tmpEB5->v=(void*)Cyc_Absyn_Bitand,_tmpEB5)))));}
break;}case 421: _LL3DD: {
# 2888
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2890
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2886 "parse.y"
{struct Cyc_Core_Opt*_tmpEB6;yyval=Cyc_YY7(((_tmpEB6=_cycalloc_atomic(sizeof(*_tmpEB6)),((_tmpEB6->v=(void*)Cyc_Absyn_Bitxor,_tmpEB6)))));}
break;}case 422: _LL3DE: {
# 2889
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2891
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2887 "parse.y"
{struct Cyc_Core_Opt*_tmpEB7;yyval=Cyc_YY7(((_tmpEB7=_cycalloc_atomic(sizeof(*_tmpEB7)),((_tmpEB7->v=(void*)Cyc_Absyn_Bitor,_tmpEB7)))));}
break;}case 423: _LL3DF: {
# 2890
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2892
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2892 "parse.y"
yyval=yyyyvsp[0];
break;}case 424: _LL3E0: {
# 2895
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2897
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2894 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_conditional_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 425: _LL3E1: {
# 2897
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2899
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2897 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_throw_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 426: _LL3E2: {
# 2900
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2902
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2900 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 427: _LL3E3: {
# 2903
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2905
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2902 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 428: _LL3E4: {
# 2905
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2907
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2904 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_New_exp(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 429: _LL3E5: {
# 2907
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2909
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2906 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_New_exp(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 430: _LL3E6: {
# 2909
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2911
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2910 "parse.y"
yyval=yyyyvsp[0];
break;}case 431: _LL3E7: {
# 2913
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2915
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2914 "parse.y"
yyval=yyyyvsp[0];
break;}case 432: _LL3E8: {
# 2917
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2919
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2916 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_or_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 433: _LL3E9: {
# 2919
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2921
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2920 "parse.y"
yyval=yyyyvsp[0];
break;}case 434: _LL3EA: {
# 2923
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2925
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2922 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_and_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 435: _LL3EB: {
# 2925
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2927
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2926 "parse.y"
yyval=yyyyvsp[0];
break;}case 436: _LL3EC: {
# 2929
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2931
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2928 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 437: _LL3ED: {
# 2931
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2933
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2932 "parse.y"
yyval=yyyyvsp[0];
break;}case 438: _LL3EE: {
# 2935
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2937
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2934 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 439: _LL3EF: {
# 2937
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2939
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2938 "parse.y"
yyval=yyyyvsp[0];
break;}case 440: _LL3F0: {
# 2941
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2943
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2940 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 441: _LL3F1: {
# 2943
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2945
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2944 "parse.y"
yyval=yyyyvsp[0];
break;}case 442: _LL3F2: {
# 2947
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2949
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2946 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_eq_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 443: _LL3F3: {
# 2949
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2951
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2948 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_neq_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 444: _LL3F4: {
# 2951
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2953
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2952 "parse.y"
yyval=yyyyvsp[0];
break;}case 445: _LL3F5: {
# 2955
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2957
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2954 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_lt_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 446: _LL3F6: {
# 2957
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2959
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2956 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_gt_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 447: _LL3F7: {
# 2959
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2961
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2958 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_lte_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 448: _LL3F8: {
# 2961
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2963
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2960 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_gte_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 449: _LL3F9: {
# 2963
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2965
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2964 "parse.y"
yyval=yyyyvsp[0];
break;}case 450: _LL3FA: {
# 2967
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2969
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2966 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 451: _LL3FB: {
# 2969
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2971
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2968 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 452: _LL3FC: {
# 2971
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2973
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2972 "parse.y"
yyval=yyyyvsp[0];
break;}case 453: _LL3FD: {
# 2975
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2977
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2974 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 454: _LL3FE: {
# 2977
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2979
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2976 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 455: _LL3FF: {
# 2979
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2981
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2980 "parse.y"
yyval=yyyyvsp[0];
break;}case 456: _LL400: {
# 2983
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2985
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2982 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 457: _LL401: {
# 2985
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2987
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2984 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 458: _LL402: {
# 2987
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2989
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2986 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 459: _LL403: {
# 2989
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2991
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2990 "parse.y"
yyval=yyyyvsp[0];
break;}case 460: _LL404: {
# 2993
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2995
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2992 "parse.y"
void*_tmp802=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_cast_exp(_tmp802,Cyc_yyget_Exp_tok(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line));
# 2995
break;}case 461: _LL405: {
# 2997
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2999
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2998 "parse.y"
yyval=yyyyvsp[0];
break;}case 462: _LL406: {
# 3001
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3003
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2999 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_pre_inc_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 463: _LL407: {
# 3002
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3004
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3000 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_pre_dec_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 464: _LL408: {
# 3003
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3005
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3001 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_address_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 465: _LL409: {
# 3004
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3006
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3002 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_deref_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 466: _LL40A: {
# 3005
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3007
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3003 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim1_exp(Cyc_Absyn_Plus,Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 467: _LL40B: {
# 3006
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3008
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3004 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim1_exp(Cyc_yyget_YY6(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 468: _LL40C: {
# 3007
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3009
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3006 "parse.y"
void*_tmp803=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_sizeoftyp_exp(_tmp803,(unsigned int)(yyyylsp[0]).first_line));
# 3009
break;}case 469: _LL40D: {
# 3011
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3013
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3009 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 470: _LL40E: {
# 3012
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3014
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3011 "parse.y"
void*_tmp804=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_offsetof_exp(_tmp804,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4])),(unsigned int)(yyyylsp[0]).first_line));
# 3014
break;}case 471: _LL40F: {
# 3016
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3018
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3016 "parse.y"
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpEBD;struct Cyc_Absyn_MallocInfo _tmpEBC;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpEBB;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpEBB=_cycalloc(sizeof(*_tmpEBB)),((_tmpEBB[0]=((_tmpEBD.tag=33,((_tmpEBD.f1=((_tmpEBC.is_calloc=0,((_tmpEBC.rgn=0,((_tmpEBC.elt_type=0,((_tmpEBC.num_elts=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpEBC.fat_result=0,_tmpEBC)))))))))),_tmpEBD)))),_tmpEBB)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3018
break;}case 472: _LL410: {
# 3020
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3022
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3019 "parse.y"
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpEC3;struct Cyc_Absyn_MallocInfo _tmpEC2;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpEC1;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpEC1=_cycalloc(sizeof(*_tmpEC1)),((_tmpEC1[0]=((_tmpEC3.tag=33,((_tmpEC3.f1=((_tmpEC2.is_calloc=0,((_tmpEC2.rgn=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpEC2.elt_type=0,((_tmpEC2.num_elts=Cyc_yyget_Exp_tok(yyyyvsp[4]),((_tmpEC2.fat_result=0,_tmpEC2)))))))))),_tmpEC3)))),_tmpEC1)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3021
break;}case 473: _LL411: {
# 3023
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 3025
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 3022 "parse.y"
void*_tmp80B=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpECD;void**_tmpECC;struct Cyc_Absyn_MallocInfo _tmpECB;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpECA;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpECA=_cycalloc(sizeof(*_tmpECA)),((_tmpECA[0]=((_tmpECD.tag=33,((_tmpECD.f1=((_tmpECB.is_calloc=1,((_tmpECB.rgn=0,((_tmpECB.elt_type=((_tmpECC=_cycalloc(sizeof(*_tmpECC)),((_tmpECC[0]=_tmp80B,_tmpECC)))),((_tmpECB.num_elts=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpECB.fat_result=0,_tmpECB)))))))))),_tmpECD)))),_tmpECA)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3025
break;}case 474: _LL412: {
# 3027
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 10),sizeof(union Cyc_YYSTYPE),11);
# 3029
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 10),sizeof(struct Cyc_Yyltype),11);
# 3027 "parse.y"
void*_tmp810=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[8]),(unsigned int)(yyyylsp[8]).first_line);
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpED7;void**_tmpED6;struct Cyc_Absyn_MallocInfo _tmpED5;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpED4;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpED4=_cycalloc(sizeof(*_tmpED4)),((_tmpED4[0]=((_tmpED7.tag=33,((_tmpED7.f1=((_tmpED5.is_calloc=1,((_tmpED5.rgn=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpED5.elt_type=((_tmpED6=_cycalloc(sizeof(*_tmpED6)),((_tmpED6[0]=_tmp810,_tmpED6)))),((_tmpED5.num_elts=Cyc_yyget_Exp_tok(yyyyvsp[4]),((_tmpED5.fat_result=0,_tmpED5)))))))))),_tmpED7)))),_tmpED4)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3030
break;}case 475: _LL413: {
# 3032
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3034
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3031 "parse.y"
{struct Cyc_Absyn_Exp*_tmpED8[1];yyval=Cyc_Exp_tok(Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,((_tmpED8[0]=Cyc_yyget_Exp_tok(yyyyvsp[2]),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpED8,sizeof(struct Cyc_Absyn_Exp*),1)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 476: _LL414: {
# 3034
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3036
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3033 "parse.y"
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpEDE;struct _dyneither_ptr*_tmpEDD;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpEDC;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpEDC=_cycalloc(sizeof(*_tmpEDC)),((_tmpEDC[0]=((_tmpEDE.tag=37,((_tmpEDE.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpEDE.f2=((_tmpEDD=_cycalloc(sizeof(*_tmpEDD)),((_tmpEDD[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpEDD)))),_tmpEDE)))))),_tmpEDC)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 477: _LL415: {
# 3036
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3038
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3035 "parse.y"
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpEE4;struct _dyneither_ptr*_tmpEE3;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpEE2;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpEE2=_cycalloc(sizeof(*_tmpEE2)),((_tmpEE2[0]=((_tmpEE4.tag=37,((_tmpEE4.f1=Cyc_Absyn_deref_exp(Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line),((_tmpEE4.f2=((_tmpEE3=_cycalloc(sizeof(*_tmpEE3)),((_tmpEE3[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpEE3)))),_tmpEE4)))))),_tmpEE2)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 478: _LL416: {
# 3038
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3040
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3037 "parse.y"
void*_tmp81C=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_valueof_exp(_tmp81C,(unsigned int)(yyyylsp[0]).first_line));
break;}case 479: _LL417: {
# 3041
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3043
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3040 "parse.y"
struct _tuple20 _tmp81D=Cyc_yyget_Asm_tok(yyyyvsp[0]);int _tmp81F;struct _dyneither_ptr _tmp820;struct _tuple20 _tmp81E=_tmp81D;_tmp81F=_tmp81E.f1;_tmp820=_tmp81E.f2;
yyval=Cyc_Exp_tok(Cyc_Absyn_asm_exp(_tmp81F,_tmp820,(unsigned int)(yyyylsp[0]).first_line));
break;}case 480: _LL418: {
# 3044
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3046
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3045 "parse.y"
yyval=Cyc_YY6(Cyc_Absyn_Bitnot);
break;}case 481: _LL419: {
# 3048
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3050
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3046 "parse.y"
yyval=Cyc_YY6(Cyc_Absyn_Not);
break;}case 482: _LL41A: {
# 3049
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3051
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3047 "parse.y"
yyval=Cyc_YY6(Cyc_Absyn_Minus);
break;}case 483: _LL41B: {
# 3050
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3052
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3052 "parse.y"
yyval=yyyyvsp[0];
break;}case 484: _LL41C: {
# 3055
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3057
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3054 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_subscript_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 485: _LL41D: {
# 3057
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3059
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3056 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 486: _LL41E: {
# 3059
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3061
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3058 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 487: _LL41F: {
# 3061
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3063
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3060 "parse.y"
{struct _dyneither_ptr*_tmpEE5;yyval=Cyc_Exp_tok(Cyc_Absyn_aggrmember_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),((_tmpEE5=_cycalloc(sizeof(*_tmpEE5)),((_tmpEE5[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpEE5)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 488: _LL420: {
# 3063
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3065
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3062 "parse.y"
{struct _dyneither_ptr*_tmpEE6;yyval=Cyc_Exp_tok(Cyc_Absyn_aggrarrow_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),((_tmpEE6=_cycalloc(sizeof(*_tmpEE6)),((_tmpEE6[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpEE6)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 489: _LL421: {
# 3065
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3067
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3064 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_post_inc_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 490: _LL422: {
# 3067
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3069
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3066 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_post_dec_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 491: _LL423: {
# 3069
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3071
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3068 "parse.y"
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpEE9;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpEE8;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpEE8=_cycalloc(sizeof(*_tmpEE8)),((_tmpEE8[0]=((_tmpEE9.tag=24,((_tmpEE9.f1=Cyc_yyget_YY37(yyyyvsp[1]),((_tmpEE9.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[4])),_tmpEE9)))))),_tmpEE8)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 492: _LL424: {
# 3071
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 3073
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 3070 "parse.y"
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpEEC;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpEEB;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpEEB=_cycalloc(sizeof(*_tmpEEB)),((_tmpEEB[0]=((_tmpEEC.tag=24,((_tmpEEC.f1=Cyc_yyget_YY37(yyyyvsp[1]),((_tmpEEC.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[4])),_tmpEEC)))))),_tmpEEB)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 493: _LL425: {
# 3073
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3075
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3075 "parse.y"
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpEF6;struct _dyneither_ptr*_tmpEF5;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpEF4;struct Cyc_List_List*_tmpEF3;yyval=Cyc_YY3(((_tmpEF3=_cycalloc(sizeof(*_tmpEF3)),((_tmpEF3->hd=(void*)((_tmpEF6=_cycalloc(sizeof(*_tmpEF6)),((_tmpEF6[0]=((_tmpEF4.tag=0,((_tmpEF4.f1=((_tmpEF5=_cycalloc(sizeof(*_tmpEF5)),((_tmpEF5[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpEF5)))),_tmpEF4)))),_tmpEF6)))),((_tmpEF3->tl=0,_tmpEF3)))))));}
break;}case 494: _LL426: {
# 3078
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3080
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3078 "parse.y"
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpEFC;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpEFB;struct Cyc_List_List*_tmpEFA;yyval=Cyc_YY3(((_tmpEFA=_cycalloc(sizeof(*_tmpEFA)),((_tmpEFA->hd=(void*)((_tmpEFC=_cycalloc_atomic(sizeof(*_tmpEFC)),((_tmpEFC[0]=((_tmpEFB.tag=1,((_tmpEFB.f1=(unsigned int)(Cyc_yyget_Int_tok(yyyyvsp[0])).f2,_tmpEFB)))),_tmpEFC)))),((_tmpEFA->tl=0,_tmpEFA)))))));}
break;}case 495: _LL427: {
# 3081
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3083
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3080 "parse.y"
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpF06;struct _dyneither_ptr*_tmpF05;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpF04;struct Cyc_List_List*_tmpF03;yyval=Cyc_YY3(((_tmpF03=_cycalloc(sizeof(*_tmpF03)),((_tmpF03->hd=(void*)((_tmpF06=_cycalloc(sizeof(*_tmpF06)),((_tmpF06[0]=((_tmpF04.tag=0,((_tmpF04.f1=((_tmpF05=_cycalloc(sizeof(*_tmpF05)),((_tmpF05[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpF05)))),_tmpF04)))),_tmpF06)))),((_tmpF03->tl=Cyc_yyget_YY3(yyyyvsp[0]),_tmpF03)))))));}
break;}case 496: _LL428: {
# 3083
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3085
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3083 "parse.y"
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpF0C;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpF0B;struct Cyc_List_List*_tmpF0A;yyval=Cyc_YY3(((_tmpF0A=_cycalloc(sizeof(*_tmpF0A)),((_tmpF0A->hd=(void*)((_tmpF0C=_cycalloc_atomic(sizeof(*_tmpF0C)),((_tmpF0C[0]=((_tmpF0B.tag=1,((_tmpF0B.f1=(unsigned int)(Cyc_yyget_Int_tok(yyyyvsp[2])).f2,_tmpF0B)))),_tmpF0C)))),((_tmpF0A->tl=Cyc_yyget_YY3(yyyyvsp[0]),_tmpF0A)))))));}
break;}case 497: _LL429: {
# 3086
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3088
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=Cyc_Exp_tok(Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 498: _LL42A: {
# 3092
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3094
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3091 "parse.y"
yyval=yyyyvsp[0];
break;}case 499: _LL42B: {
# 3094
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3096
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3093 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 500: _LL42C: {
# 3096
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3098
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3095 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_wstring_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 501: _LL42D: {
# 3098
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3100
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3097 "parse.y"
yyval=yyyyvsp[1];
break;}case 502: _LL42E: {
# 3100
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3102
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3102 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_noinstantiate_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 503: _LL42F: {
# 3105
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3107
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3104 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_instantiate_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[3])),(unsigned int)(yyyylsp[0]).first_line));
break;}case 504: _LL430: {
# 3107
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3109
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3107 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_tuple_exp(Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 505: _LL431: {
# 3110
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3112
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3110 "parse.y"
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpF0F;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpF0E;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpF0E=_cycalloc(sizeof(*_tmpF0E)),((_tmpF0E[0]=((_tmpF0F.tag=28,((_tmpF0F.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpF0F.f2=Cyc_yyget_YY40(yyyyvsp[2]),((_tmpF0F.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[3])),((_tmpF0F.f4=0,_tmpF0F)))))))))),_tmpF0E)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 506: _LL432: {
# 3113
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3115
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3113 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_stmt_exp(Cyc_yyget_Stmt_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 507: _LL433: {
# 3116
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3118
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3117 "parse.y"
yyval=Cyc_YY4(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY4(yyyyvsp[0])));
break;}case 508: _LL434: {
# 3120
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3122
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpF10;yyval=Cyc_YY4(((_tmpF10=_cycalloc(sizeof(*_tmpF10)),((_tmpF10->hd=Cyc_yyget_Exp_tok(yyyyvsp[0]),((_tmpF10->tl=0,_tmpF10)))))));}
break;}case 509: _LL435: {
# 3126
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3128
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3125 "parse.y"
{struct Cyc_List_List*_tmpF11;yyval=Cyc_YY4(((_tmpF11=_cycalloc(sizeof(*_tmpF11)),((_tmpF11->hd=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpF11->tl=Cyc_yyget_YY4(yyyyvsp[0]),_tmpF11)))))));}
break;}case 510: _LL436: {
# 3128
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3130
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
struct _tuple5 _tmp839=Cyc_yyget_Int_tok(yyyyvsp[0]);yyval=Cyc_Exp_tok(Cyc_Absyn_int_exp(_tmp839.f1,_tmp839.f2,(unsigned int)(yyyylsp[0]).first_line));
break;}case 511: _LL437: {
# 3134
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3136
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3132 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 512: _LL438: {
# 3135
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3137
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3133 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_wchar_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 513: _LL439: {
# 3136
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3138
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3135 "parse.y"
struct _dyneither_ptr _tmp83A=Cyc_yyget_String_tok(yyyyvsp[0]);
int l=(int)Cyc_strlen((struct _dyneither_ptr)_tmp83A);
int i=1;
if(l > 0){
char c=*((const char*)_check_dyneither_subscript(_tmp83A,sizeof(char),l - 1));
if(c == 'f'  || c == 'F')i=0;else{
if(c == 'l'  || c == 'L')i=2;}}
# 3143
yyval=Cyc_Exp_tok(Cyc_Absyn_float_exp(_tmp83A,i,(unsigned int)(yyyylsp[0]).first_line));
# 3145
break;}case 514: _LL43A: {
# 3147
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3149
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3146 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_null_exp((unsigned int)(yyyylsp[0]).first_line));
break;}case 515: _LL43B: {
# 3149
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3151
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3150 "parse.y"
{struct _dyneither_ptr*_tmpF14;struct _tuple0*_tmpF13;yyval=Cyc_QualId_tok(((_tmpF13=_cycalloc(sizeof(*_tmpF13)),((_tmpF13->f1=Cyc_Absyn_Rel_n(0),((_tmpF13->f2=((_tmpF14=_cycalloc(sizeof(*_tmpF14)),((_tmpF14[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpF14)))),_tmpF13)))))));}
break;}case 516: _LL43C: {
# 3153
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3155
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3151 "parse.y"
yyval=yyyyvsp[0];
break;}case 517: _LL43D: {
# 3154
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3156
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3154 "parse.y"
{struct _dyneither_ptr*_tmpF17;struct _tuple0*_tmpF16;yyval=Cyc_QualId_tok(((_tmpF16=_cycalloc(sizeof(*_tmpF16)),((_tmpF16->f1=Cyc_Absyn_Rel_n(0),((_tmpF16->f2=((_tmpF17=_cycalloc(sizeof(*_tmpF17)),((_tmpF17[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpF17)))),_tmpF16)))))));}
break;}case 518: _LL43E: {
# 3157
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3159
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3155 "parse.y"
yyval=yyyyvsp[0];
break;}case 519: _LL43F: {
# 3158
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3160
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3160 "parse.y"
yyval=yyyyvsp[0];
break;}case 520: _LL440: {
# 3163
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3165
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3161 "parse.y"
yyval=yyyyvsp[0];
break;}case 521: _LL441: {
# 3164
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3166
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3164 "parse.y"
yyval=yyyyvsp[0];
break;}case 522: _LL442: {
# 3167
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3169
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3165 "parse.y"
yyval=yyyyvsp[0];
break;}case 523: _LL443: {
# 3168
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3170
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3170 "parse.y"
break;}case 524: _LL444: {
# 3172
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3174
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3170 "parse.y"
yylex_buf->lex_curr_pos -=1;
break;}default: _LL445:
# 3175
 break;}
# 371 "cyc_bison.simple"
yyvsp_offset -=yylen;
yyssp_offset -=yylen;
# 374
yylsp_offset -=yylen;
# 388 "cyc_bison.simple"
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yyval;
# 391
++ yylsp_offset;
if(yylen == 0){
# 394
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line=yylloc.first_line;
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_column=yylloc.first_column;
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line;
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_column=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_column;}else{
# 401
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(yylsp_offset + yylen)- 1))).last_line;
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_column=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(yylsp_offset + yylen)- 1))).last_column;}
# 411
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(525,yyn)];
# 413
yystate=Cyc_yypgoto[_check_known_subscript_notnull(146,yyn - 145)]+ *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset));
if((yystate >= 0  && yystate <= 6664) && Cyc_yycheck[_check_known_subscript_notnull(6665,yystate)]== *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset)))
yystate=(int)Cyc_yytable[_check_known_subscript_notnull(6665,yystate)];else{
# 417
yystate=(int)Cyc_yydefgoto[_check_known_subscript_notnull(146,yyn - 145)];}
# 419
goto yynewstate;
# 421
yyerrlab:
# 423
 if(yyerrstatus == 0){
# 426
++ yynerrs;
# 429
yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1061,yystate)];
# 431
if(yyn > - 32768  && yyn < 6664){
# 433
int sze=0;
struct _dyneither_ptr msg;
int x;int count;
# 437
count=0;
# 439
for(x=yyn < 0?- yyn: 0;x < 291 / sizeof(char*);++ x){
# 441
if(Cyc_yycheck[_check_known_subscript_notnull(6665,x + yyn)]== x)
(sze +=Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(291,x)])+ 15,count ++);}
{unsigned int _tmpF1C;unsigned int _tmpF1B;struct _dyneither_ptr _tmpF1A;char*_tmpF19;unsigned int _tmpF18;msg=((_tmpF18=(unsigned int)(sze + 15),((_tmpF19=(char*)_region_malloc(yyregion,_check_times(sizeof(char),_tmpF18 + 1)),((_tmpF1A=_tag_dyneither(_tmpF19,sizeof(char),_tmpF18 + 1),((((_tmpF1B=_tmpF18,((_tmpF1D(& _tmpF1C,& _tmpF1B,& _tmpF19),_tmpF19[_tmpF1B]=(char)0)))),_tmpF1A))))))));}
{const char*_tmpF1E;Cyc_strcpy(msg,((_tmpF1E="parse error",_tag_dyneither(_tmpF1E,sizeof(char),12))));}
# 446
if(count < 5){
# 448
count=0;
for(x=yyn < 0?- yyn: 0;x < 291 / sizeof(char*);++ x){
# 451
if(Cyc_yycheck[_check_known_subscript_notnull(6665,x + yyn)]== x){
# 453
{const char*_tmpF20;const char*_tmpF1F;Cyc_strcat(msg,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)(
(_tmpF20=", expecting `",_tag_dyneither(_tmpF20,sizeof(char),14))):(struct _dyneither_ptr)(
(_tmpF1F=" or `",_tag_dyneither(_tmpF1F,sizeof(char),6)))));}
Cyc_strcat(msg,(struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(291,x)]);
{const char*_tmpF21;Cyc_strcat(msg,((_tmpF21="'",_tag_dyneither(_tmpF21,sizeof(char),2))));}
++ count;}}}
# 461
Cyc_yyerror((struct _dyneither_ptr)msg,yystate,yychar);}else{
# 465
const char*_tmpF22;Cyc_yyerror(((_tmpF22="parse error",_tag_dyneither(_tmpF22,sizeof(char),12))),yystate,yychar);}}
# 467
goto yyerrlab1;
# 469
yyerrlab1:
# 471
 if(yyerrstatus == 3){
# 476
if(yychar == 0){
int _tmp848=1;_npop_handler(0);return _tmp848;}
# 485
yychar=-2;}
# 491
yyerrstatus=3;
# 493
goto yyerrhandle;
# 495
yyerrdefault:
# 505 "cyc_bison.simple"
 yyerrpop:
# 507
 if(yyssp_offset == 0){int _tmp849=1;_npop_handler(0);return _tmp849;}
-- yyvsp_offset;
yystate=(int)*((short*)_check_dyneither_subscript(yyss,sizeof(short),-- yyssp_offset));
# 511
-- yylsp_offset;
# 526 "cyc_bison.simple"
yyerrhandle:
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1061,yystate)];
if(yyn == - 32768)goto yyerrdefault;
# 530
yyn +=1;
if((yyn < 0  || yyn > 6664) || Cyc_yycheck[_check_known_subscript_notnull(6665,yyn)]!= 1)goto yyerrdefault;
# 533
yyn=(int)Cyc_yytable[_check_known_subscript_notnull(6665,yyn)];
if(yyn < 0){
# 536
if(yyn == - 32768)goto yyerrpop;
yyn=- yyn;
goto yyreduce;}else{
# 540
if(yyn == 0)goto yyerrpop;}
# 542
if(yyn == 1060){
int _tmp84A=0;_npop_handler(0);return _tmp84A;}
# 550
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yylval;
# 552
*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),++ yylsp_offset))=yylloc;
# 555
goto yynewstate;}
# 141 "cycbison.simple"
;_pop_region(yyregion);}
# 3173 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
union Cyc_YYSTYPE _tmp854=v;int _tmp855;char _tmp856;struct _dyneither_ptr _tmp857;union Cyc_Absyn_Nmspace _tmp858;struct _dyneither_ptr*_tmp859;struct Cyc_Absyn_Exp*_tmp85A;struct Cyc_Absyn_Stmt*_tmp85B;_LL448: if((_tmp854.Int_tok).tag != 1)goto _LL44A;_tmp855=((struct _tuple5)(_tmp854.Int_tok).val).f2;_LL449:
{const char*_tmpF26;void*_tmpF25[1];struct Cyc_Int_pa_PrintArg_struct _tmpF24;(_tmpF24.tag=1,((_tmpF24.f1=(unsigned long)_tmp855,((_tmpF25[0]=& _tmpF24,Cyc_fprintf(Cyc_stderr,((_tmpF26="%d",_tag_dyneither(_tmpF26,sizeof(char),3))),_tag_dyneither(_tmpF25,sizeof(void*),1)))))));}goto _LL447;_LL44A: if((_tmp854.Char_tok).tag != 2)goto _LL44C;_tmp856=(char)(_tmp854.Char_tok).val;_LL44B:
{const char*_tmpF2A;void*_tmpF29[1];struct Cyc_Int_pa_PrintArg_struct _tmpF28;(_tmpF28.tag=1,((_tmpF28.f1=(unsigned long)((int)_tmp856),((_tmpF29[0]=& _tmpF28,Cyc_fprintf(Cyc_stderr,((_tmpF2A="%c",_tag_dyneither(_tmpF2A,sizeof(char),3))),_tag_dyneither(_tmpF29,sizeof(void*),1)))))));}goto _LL447;_LL44C: if((_tmp854.String_tok).tag != 3)goto _LL44E;_tmp857=(struct _dyneither_ptr)(_tmp854.String_tok).val;_LL44D:
{const char*_tmpF2E;void*_tmpF2D[1];struct Cyc_String_pa_PrintArg_struct _tmpF2C;(_tmpF2C.tag=0,((_tmpF2C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp857),((_tmpF2D[0]=& _tmpF2C,Cyc_fprintf(Cyc_stderr,((_tmpF2E="\"%s\"",_tag_dyneither(_tmpF2E,sizeof(char),5))),_tag_dyneither(_tmpF2D,sizeof(void*),1)))))));}goto _LL447;_LL44E: if((_tmp854.QualId_tok).tag != 5)goto _LL450;_tmp858=((struct _tuple0*)(_tmp854.QualId_tok).val)->f1;_tmp859=((struct _tuple0*)(_tmp854.QualId_tok).val)->f2;_LL44F: {
# 3179
struct Cyc_List_List*_tmp865=0;
{union Cyc_Absyn_Nmspace _tmp866=_tmp858;struct Cyc_List_List*_tmp867;struct Cyc_List_List*_tmp868;struct Cyc_List_List*_tmp869;_LL457: if((_tmp866.Rel_n).tag != 1)goto _LL459;_tmp867=(struct Cyc_List_List*)(_tmp866.Rel_n).val;_LL458:
 _tmp865=_tmp867;goto _LL456;_LL459: if((_tmp866.Abs_n).tag != 2)goto _LL45B;_tmp868=(struct Cyc_List_List*)(_tmp866.Abs_n).val;_LL45A:
 _tmp865=_tmp868;goto _LL456;_LL45B: if((_tmp866.C_n).tag != 3)goto _LL45D;_tmp869=(struct Cyc_List_List*)(_tmp866.C_n).val;_LL45C:
 _tmp865=_tmp869;goto _LL456;_LL45D: if((_tmp866.Loc_n).tag != 4)goto _LL456;_LL45E:
 goto _LL456;_LL456:;}
# 3186
for(0;_tmp865 != 0;_tmp865=_tmp865->tl){
const char*_tmpF32;void*_tmpF31[1];struct Cyc_String_pa_PrintArg_struct _tmpF30;(_tmpF30.tag=0,((_tmpF30.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp865->hd)),((_tmpF31[0]=& _tmpF30,Cyc_fprintf(Cyc_stderr,((_tmpF32="%s::",_tag_dyneither(_tmpF32,sizeof(char),5))),_tag_dyneither(_tmpF31,sizeof(void*),1)))))));}
{const char*_tmpF36;void*_tmpF35[1];struct Cyc_String_pa_PrintArg_struct _tmpF34;(_tmpF34.tag=0,((_tmpF34.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp859),((_tmpF35[0]=& _tmpF34,Cyc_fprintf(Cyc_stderr,((_tmpF36="%s::",_tag_dyneither(_tmpF36,sizeof(char),5))),_tag_dyneither(_tmpF35,sizeof(void*),1)))))));}
goto _LL447;}_LL450: if((_tmp854.Exp_tok).tag != 7)goto _LL452;_tmp85A=(struct Cyc_Absyn_Exp*)(_tmp854.Exp_tok).val;_LL451:
{const char*_tmpF3A;void*_tmpF39[1];struct Cyc_String_pa_PrintArg_struct _tmpF38;(_tmpF38.tag=0,((_tmpF38.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp85A)),((_tmpF39[0]=& _tmpF38,Cyc_fprintf(Cyc_stderr,((_tmpF3A="%s",_tag_dyneither(_tmpF3A,sizeof(char),3))),_tag_dyneither(_tmpF39,sizeof(void*),1)))))));}goto _LL447;_LL452: if((_tmp854.Stmt_tok).tag != 8)goto _LL454;_tmp85B=(struct Cyc_Absyn_Stmt*)(_tmp854.Stmt_tok).val;_LL453:
{const char*_tmpF3E;void*_tmpF3D[1];struct Cyc_String_pa_PrintArg_struct _tmpF3C;(_tmpF3C.tag=0,((_tmpF3C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(_tmp85B)),((_tmpF3D[0]=& _tmpF3C,Cyc_fprintf(Cyc_stderr,((_tmpF3E="%s",_tag_dyneither(_tmpF3E,sizeof(char),3))),_tag_dyneither(_tmpF3D,sizeof(void*),1)))))));}goto _LL447;_LL454:;_LL455:
{const char*_tmpF41;void*_tmpF40;(_tmpF40=0,Cyc_fprintf(Cyc_stderr,((_tmpF41="?",_tag_dyneither(_tmpF41,sizeof(char),2))),_tag_dyneither(_tmpF40,sizeof(void*),0)));}goto _LL447;_LL447:;}
# 3197
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp878=_new_region("yyr");struct _RegionHandle*yyr=& _tmp878;_push_region(yyr);
Cyc_yyparse(yyr,Cyc_Lexing_from_file(f));{
struct Cyc_List_List*_tmp879=Cyc_Parse_parse_result;_npop_handler(0);return _tmp879;};
# 3200
;_pop_region(yyr);};}
