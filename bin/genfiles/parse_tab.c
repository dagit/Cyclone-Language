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
# 281 "core.h"
typedef void*Cyc_Core___cyclone_internal_array_t;
typedef unsigned int Cyc_Core___cyclone_internal_singleton;
# 285
inline static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){
# 290
if(bd >> 20  || sz >> 12)
return 0;{
unsigned char*ptrbd=dyn.curr + bd * sz;
if(((ptrbd < dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
# 297
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
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};
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
# 1024
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
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
# 69 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst);
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
# 74 "parse.y"
void Cyc_Lex_register_typedef(struct _tuple0*s);
void Cyc_Lex_enter_namespace(struct _dyneither_ptr*);
void Cyc_Lex_leave_namespace();
void Cyc_Lex_enter_using(struct _tuple0*);
void Cyc_Lex_leave_using();
void Cyc_Lex_enter_extern_c();
void Cyc_Lex_leave_extern_c();struct Cyc_Parse_FlatList{struct Cyc_Parse_FlatList*tl;void*hd[0] __attribute__((aligned )) ;};
# 96 "parse.y"
typedef struct Cyc_Parse_FlatList*Cyc_Parse_flat_list_t;
# 98
struct Cyc_Parse_FlatList*Cyc_Parse_flat_imp_rev(struct Cyc_Parse_FlatList*x){
if(x == 0)return x;else{
# 101
struct Cyc_Parse_FlatList*first=x;
struct Cyc_Parse_FlatList*second=x->tl;
x->tl=0;
while(second != 0){
struct Cyc_Parse_FlatList*temp=second->tl;
second->tl=first;
first=second;
second=temp;}
# 110
return first;}}
# 115
int Cyc_Parse_no_register=0;struct Cyc_Parse_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned int loc;};
# 128
typedef struct Cyc_Parse_Type_specifier Cyc_Parse_type_specifier_t;
# 130
enum Cyc_Parse_Storage_class{Cyc_Parse_Typedef_sc  = 0,Cyc_Parse_Extern_sc  = 1,Cyc_Parse_ExternC_sc  = 2,Cyc_Parse_Static_sc  = 3,Cyc_Parse_Auto_sc  = 4,Cyc_Parse_Register_sc  = 5,Cyc_Parse_Abstract_sc  = 6};
# 133
typedef enum Cyc_Parse_Storage_class Cyc_Parse_storage_class_t;struct Cyc_Parse_Declaration_spec{enum Cyc_Parse_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Parse_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};
# 142
typedef struct Cyc_Parse_Declaration_spec Cyc_Parse_decl_spec_t;struct Cyc_Parse_Declarator{struct _tuple0*id;struct Cyc_List_List*tms;};
# 148
typedef struct Cyc_Parse_Declarator Cyc_Parse_declarator_t;struct _tuple10{struct _tuple10*tl;struct Cyc_Parse_Declarator hd  __attribute__((aligned )) ;};
typedef struct _tuple10*Cyc_Parse_declarators_t;struct _tuple11{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple12{struct _tuple12*tl;struct _tuple11 hd  __attribute__((aligned )) ;};
typedef struct _tuple12*Cyc_Parse_declarator_list_t;
# 152
static void Cyc_Parse_decl_split(struct _RegionHandle*r,struct _tuple12*ds,struct _tuple10**decls,struct Cyc_List_List**es){
# 156
struct _tuple10*declarators=0;
struct Cyc_List_List*exprs=0;
for(0;ds != 0;ds=ds->tl){
struct _tuple11 _tmp0=ds->hd;struct Cyc_Parse_Declarator _tmp2;struct Cyc_Absyn_Exp*_tmp3;struct _tuple11 _tmp1=_tmp0;_tmp2=_tmp1.f1;_tmp3=_tmp1.f2;
{struct _tuple10*_tmp87F;declarators=((_tmp87F=_region_malloc(r,sizeof(*_tmp87F)),((_tmp87F->tl=declarators,((_tmp87F->hd=_tmp2,_tmp87F))))));}{
struct Cyc_List_List*_tmp880;exprs=((_tmp880=_region_malloc(r,sizeof(*_tmp880)),((_tmp880->hd=_tmp3,((_tmp880->tl=exprs,_tmp880))))));};}
# 163
*es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(exprs);
*decls=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(declarators);}struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};
# 170
typedef struct Cyc_Parse_Abstractdeclarator Cyc_Parse_abstractdeclarator_t;
# 173
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned int loc);struct _tuple13{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
# 175
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
# 180
static struct Cyc_List_List*Cyc_Parse_parse_result=0;
# 183
static void Cyc_Parse_err(struct _dyneither_ptr msg,unsigned int sg){
Cyc_Position_post_error(Cyc_Position_mk_err_parse(sg,msg));}
# 186
static void*Cyc_Parse_parse_abort(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 188
Cyc_Parse_err((struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap),sg);{
struct Cyc_Position_Exit_exn_struct _tmp883;struct Cyc_Position_Exit_exn_struct*_tmp882;(int)_throw((void*)((_tmp882=_cycalloc_atomic(sizeof(*_tmp882)),((_tmp882[0]=((_tmp883.tag=Cyc_Position_Exit,_tmp883)),_tmp882)))));};}
# 191
static void Cyc_Parse_unimp(struct _dyneither_ptr msg,unsigned int sg){
{const char*_tmp888;void*_tmp887[2];struct Cyc_String_pa_PrintArg_struct _tmp886;struct Cyc_String_pa_PrintArg_struct _tmp885;(_tmp885.tag=0,((_tmp885.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp886.tag=0,((_tmp886.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(sg)),((_tmp887[0]=& _tmp886,((_tmp887[1]=& _tmp885,Cyc_fprintf(Cyc_stderr,((_tmp888="%s: Warning: Cyclone does not yet support %s\n",_tag_dyneither(_tmp888,sizeof(char),46))),_tag_dyneither(_tmp887,sizeof(void*),2)))))))))))));}
return;}
# 197
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned int loc){
# 199
struct Cyc_Absyn_Tqual _tmpD;void*_tmpE;struct _tuple8*_tmpC=tqt;_tmpD=_tmpC->f2;_tmpE=_tmpC->f3;
if((_tmpD.print_const  || _tmpD.q_volatile) || _tmpD.q_restrict){
if(_tmpD.loc != 0)loc=_tmpD.loc;{
const char*_tmp88B;void*_tmp88A;(_tmp88A=0,Cyc_Tcutil_warn(loc,((_tmp88B="qualifier on type is ignored",_tag_dyneither(_tmp88B,sizeof(char),29))),_tag_dyneither(_tmp88A,sizeof(void*),0)));};}
# 204
return _tmpE;}struct _tuple14{union Cyc_Absyn_Constraint*f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;void*f4;};
# 207
static struct _tuple14 Cyc_Parse_collapse_pointer_quals(unsigned int loc,union Cyc_Absyn_Constraint*nullable,union Cyc_Absyn_Constraint*bound,void*rgn,struct Cyc_List_List*pqs){
# 213
union Cyc_Absyn_Constraint*zeroterm=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
for(0;pqs != 0;pqs=pqs->tl){
void*_tmp11=(void*)pqs->hd;void*_tmp12=_tmp11;struct Cyc_Absyn_Exp*_tmp1A;void*_tmp1C;_LL1: {struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmp13=(struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp13->tag != 4)goto _LL3;}_LL2:
# 217
 zeroterm=Cyc_Absyn_true_conref;goto _LL0;_LL3: {struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmp14=(struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp14->tag != 5)goto _LL5;}_LL4:
# 219
 zeroterm=Cyc_Absyn_false_conref;goto _LL0;_LL5: {struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct*_tmp15=(struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp15->tag != 7)goto _LL7;}_LL6:
# 221
 nullable=Cyc_Absyn_true_conref;goto _LL0;_LL7: {struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct*_tmp16=(struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp16->tag != 6)goto _LL9;}_LL8:
# 223
 nullable=Cyc_Absyn_false_conref;goto _LL0;_LL9: {struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct*_tmp17=(struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp17->tag != 3)goto _LLB;}_LLA:
# 225
 bound=Cyc_Absyn_bounds_dyneither_conref;goto _LL0;_LLB: {struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct*_tmp18=(struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp18->tag != 2)goto _LLD;}_LLC:
# 227
 bound=Cyc_Absyn_bounds_one_conref;goto _LL0;_LLD: {struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct*_tmp19=(struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp19->tag != 0)goto _LLF;else{_tmp1A=_tmp19->f1;}}_LLE:
# 229
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp88E;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp88D;bound=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp88D=_cycalloc(sizeof(*_tmp88D)),((_tmp88D[0]=((_tmp88E.tag=1,((_tmp88E.f1=_tmp1A,_tmp88E)))),_tmp88D)))));}goto _LL0;_LLF: {struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmp1B=(struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp1B->tag != 1)goto _LL0;else{_tmp1C=(void*)_tmp1B->f1;}}_LL10:
# 231
 rgn=_tmp1C;goto _LL0;_LL0:;}{
# 234
struct _tuple14 _tmp88F;return(_tmp88F.f1=nullable,((_tmp88F.f2=bound,((_tmp88F.f3=zeroterm,((_tmp88F.f4=rgn,_tmp88F)))))));};}
# 240
struct _tuple0*Cyc_Parse_gensym_enum(){
# 242
static int enum_counter=0;
struct _dyneither_ptr*_tmp89C;const char*_tmp89B;void*_tmp89A[1];struct Cyc_Int_pa_PrintArg_struct _tmp899;struct _tuple0*_tmp898;return(_tmp898=_cycalloc(sizeof(*_tmp898)),((_tmp898->f1=Cyc_Absyn_Rel_n(0),((_tmp898->f2=(
(_tmp89C=_cycalloc(sizeof(*_tmp89C)),((_tmp89C[0]=(struct _dyneither_ptr)((_tmp899.tag=1,((_tmp899.f1=(unsigned long)enum_counter ++,((_tmp89A[0]=& _tmp899,Cyc_aprintf(((_tmp89B="__anonymous_enum_%d__",_tag_dyneither(_tmp89B,sizeof(char),22))),_tag_dyneither(_tmp89A,sizeof(void*),1)))))))),_tmp89C)))),_tmp898)))));}struct _tuple15{struct _tuple0*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple16{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple17{struct _tuple15*f1;struct _tuple16*f2;};
# 247
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned int loc,struct _tuple17*field_info){
# 252
struct _tuple0*_tmp26;struct Cyc_Absyn_Tqual _tmp27;void*_tmp28;struct Cyc_List_List*_tmp29;struct Cyc_List_List*_tmp2A;struct Cyc_Absyn_Exp*_tmp2B;struct Cyc_Absyn_Exp*_tmp2C;struct _tuple17*_tmp25=field_info;_tmp26=(_tmp25->f1)->f1;_tmp27=(_tmp25->f1)->f2;_tmp28=(_tmp25->f1)->f3;_tmp29=(_tmp25->f1)->f4;_tmp2A=(_tmp25->f1)->f5;_tmp2B=(_tmp25->f2)->f1;_tmp2C=(_tmp25->f2)->f2;
if(_tmp29 != 0){
const char*_tmp89D;Cyc_Parse_err(((_tmp89D="bad type params in struct field",_tag_dyneither(_tmp89D,sizeof(char),32))),loc);}
if(Cyc_Absyn_is_qvar_qualified(_tmp26)){
const char*_tmp89E;Cyc_Parse_err(((_tmp89E="struct or union field cannot be qualified with a namespace",_tag_dyneither(_tmp89E,sizeof(char),59))),loc);}{
struct Cyc_Absyn_Aggrfield*_tmp89F;return(_tmp89F=_cycalloc(sizeof(*_tmp89F)),((_tmp89F->name=(*_tmp26).f2,((_tmp89F->tq=_tmp27,((_tmp89F->type=_tmp28,((_tmp89F->width=_tmp2B,((_tmp89F->attributes=_tmp2A,((_tmp89F->requires_clause=_tmp2C,_tmp89F)))))))))))));};}
# 262
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp8A0;return(_tmp8A0.Signed_spec=0,((_tmp8A0.Unsigned_spec=0,((_tmp8A0.Short_spec=0,((_tmp8A0.Long_spec=0,((_tmp8A0.Long_Long_spec=0,((_tmp8A0.Valid_type_spec=0,((_tmp8A0.Type_spec=Cyc_Absyn_sint_typ,((_tmp8A0.loc=loc,_tmp8A0)))))))))))))));}
# 273
static struct Cyc_Parse_Type_specifier Cyc_Parse_type_spec(void*t,unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp31=Cyc_Parse_empty_spec(loc);
_tmp31.Type_spec=t;
_tmp31.Valid_type_spec=1;
return _tmp31;}
# 279
static struct Cyc_Parse_Type_specifier Cyc_Parse_signed_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp32=Cyc_Parse_empty_spec(loc);
_tmp32.Signed_spec=1;
return _tmp32;}
# 284
static struct Cyc_Parse_Type_specifier Cyc_Parse_unsigned_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp33=Cyc_Parse_empty_spec(loc);
_tmp33.Unsigned_spec=1;
return _tmp33;}
# 289
static struct Cyc_Parse_Type_specifier Cyc_Parse_short_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp34=Cyc_Parse_empty_spec(loc);
_tmp34.Short_spec=1;
return _tmp34;}
# 294
static struct Cyc_Parse_Type_specifier Cyc_Parse_long_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp35=Cyc_Parse_empty_spec(loc);
_tmp35.Long_spec=1;
return _tmp35;}
# 301
static void*Cyc_Parse_array2ptr(void*t,int argposn){
# 303
return Cyc_Tcutil_is_array(t)?
Cyc_Tcutil_promote_array(t,argposn?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0):(void*)& Cyc_Absyn_HeapRgn_val,0): t;}struct _tuple18{struct _dyneither_ptr*f1;void*f2;};
# 316 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp36=0;
for(0;x != 0;x=x->tl){
struct _tuple8*_tmp37=(struct _tuple8*)x->hd;struct _tuple8*_tmp38=_tmp37;struct _dyneither_ptr*_tmp39;void*_tmp3B;struct _dyneither_ptr _tmp3C;void**_tmp3F;_LL12: _tmp39=_tmp38->f1;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp3A=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp38->f3;if(_tmp3A->tag != 19)goto _LL14;else{_tmp3B=(void*)_tmp3A->f1;}};if(!(_tmp39 != 0))goto _LL14;_LL13:
# 321
{void*_tmp40=_tmp3B;void**_tmp42;_LL19: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp41=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp40;if(_tmp41->tag != 1)goto _LL1B;else{_tmp42=(void**)((void**)& _tmp41->f2);}}_LL1A: {
# 325
struct Cyc_String_pa_PrintArg_struct _tmp8A8;void*_tmp8A7[1];const char*_tmp8A6;struct _dyneither_ptr*_tmp8A5;struct _dyneither_ptr*nm=(_tmp8A5=_cycalloc(sizeof(*_tmp8A5)),((_tmp8A5[0]=(struct _dyneither_ptr)((_tmp8A8.tag=0,((_tmp8A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp39),((_tmp8A7[0]=& _tmp8A8,Cyc_aprintf(((_tmp8A6="`%s",_tag_dyneither(_tmp8A6,sizeof(char),4))),_tag_dyneither(_tmp8A7,sizeof(void*),1)))))))),_tmp8A5)));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8B7;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp8B6;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp8B5;struct Cyc_Absyn_Tvar*_tmp8B4;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8B3;*_tmp42=(void*)((_tmp8B3=_cycalloc(sizeof(*_tmp8B3)),((_tmp8B3[0]=((_tmp8B7.tag=2,((_tmp8B7.f1=((_tmp8B4=_cycalloc(sizeof(*_tmp8B4)),((_tmp8B4->name=nm,((_tmp8B4->identity=- 1,((_tmp8B4->kind=(void*)((_tmp8B6=_cycalloc(sizeof(*_tmp8B6)),((_tmp8B6[0]=((_tmp8B5.tag=0,((_tmp8B5.f1=& Cyc_Tcutil_ik,_tmp8B5)))),_tmp8B6)))),_tmp8B4)))))))),_tmp8B7)))),_tmp8B3))));}
goto _LL18;}_LL1B:;_LL1C:
 goto _LL18;_LL18:;}
# 330
{struct _tuple18*_tmp8BA;struct Cyc_List_List*_tmp8B9;_tmp36=((_tmp8B9=_cycalloc(sizeof(*_tmp8B9)),((_tmp8B9->hd=((_tmp8BA=_cycalloc(sizeof(*_tmp8BA)),((_tmp8BA->f1=_tmp39,((_tmp8BA->f2=_tmp3B,_tmp8BA)))))),((_tmp8B9->tl=_tmp36,_tmp8B9))))));}goto _LL11;_LL14: if(_tmp38->f1 == 0)goto _LL16;_tmp3C=*_tmp38->f1;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp3D=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp38->f3;if(_tmp3D->tag != 15)goto _LL16;else{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)((void*)_tmp3D->f1);if(_tmp3E->tag != 1)goto _LL16;else{_tmp3F=(void**)((void**)& _tmp3E->f2);}}};_LL15: {
# 334
struct Cyc_String_pa_PrintArg_struct _tmp8C2;void*_tmp8C1[1];const char*_tmp8C0;struct _dyneither_ptr*_tmp8BF;struct _dyneither_ptr*nm=(_tmp8BF=_cycalloc(sizeof(*_tmp8BF)),((_tmp8BF[0]=(struct _dyneither_ptr)((_tmp8C2.tag=0,((_tmp8C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3C),((_tmp8C1[0]=& _tmp8C2,Cyc_aprintf(((_tmp8C0="`%s",_tag_dyneither(_tmp8C0,sizeof(char),4))),_tag_dyneither(_tmp8C1,sizeof(void*),1)))))))),_tmp8BF)));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8D1;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp8D0;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp8CF;struct Cyc_Absyn_Tvar*_tmp8CE;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8CD;*_tmp3F=(void*)((_tmp8CD=_cycalloc(sizeof(*_tmp8CD)),((_tmp8CD[0]=((_tmp8D1.tag=2,((_tmp8D1.f1=((_tmp8CE=_cycalloc(sizeof(*_tmp8CE)),((_tmp8CE->name=nm,((_tmp8CE->identity=- 1,((_tmp8CE->kind=(void*)((_tmp8D0=_cycalloc(sizeof(*_tmp8D0)),((_tmp8D0[0]=((_tmp8CF.tag=0,((_tmp8CF.f1=& Cyc_Tcutil_rk,_tmp8CF)))),_tmp8D0)))),_tmp8CE)))))))),_tmp8D1)))),_tmp8CD))));}
goto _LL11;}_LL16:;_LL17:
 goto _LL11;_LL11:;}
# 340
return _tmp36;}
# 344
static struct Cyc_List_List*Cyc_Parse_get_argrfield_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp57=0;
for(0;x != 0;x=x->tl){
void*_tmp58=((struct Cyc_Absyn_Aggrfield*)x->hd)->type;void*_tmp59=_tmp58;void*_tmp5B;_LL1E: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5A=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp59;if(_tmp5A->tag != 19)goto _LL20;else{_tmp5B=(void*)_tmp5A->f1;}}_LL1F:
# 349
{struct _tuple18*_tmp8D4;struct Cyc_List_List*_tmp8D3;_tmp57=((_tmp8D3=_cycalloc(sizeof(*_tmp8D3)),((_tmp8D3->hd=((_tmp8D4=_cycalloc(sizeof(*_tmp8D4)),((_tmp8D4->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,((_tmp8D4->f2=_tmp5B,_tmp8D4)))))),((_tmp8D3->tl=_tmp57,_tmp8D3))))));}goto _LL1D;_LL20:;_LL21:
 goto _LL1D;_LL1D:;}
# 353
return _tmp57;}
# 357
static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){
{void*_tmp5E=e->r;void*_tmp5F=_tmp5E;struct _dyneither_ptr*_tmp61;_LL23: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp60=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5F;if(_tmp60->tag != 1)goto _LL25;else{if((((_tmp60->f1)->f1).Rel_n).tag != 1)goto _LL25;if((struct Cyc_List_List*)(((_tmp60->f1)->f1).Rel_n).val != 0)goto _LL25;_tmp61=(_tmp60->f1)->f2;}}_LL24:
# 360
{struct Cyc_List_List*_tmp62=tags;for(0;_tmp62 != 0;_tmp62=_tmp62->tl){
struct _tuple18*_tmp63=(struct _tuple18*)_tmp62->hd;struct _dyneither_ptr*_tmp65;void*_tmp66;struct _tuple18*_tmp64=_tmp63;_tmp65=_tmp64->f1;_tmp66=_tmp64->f2;
if(Cyc_strptrcmp(_tmp65,_tmp61)== 0){
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp8D7;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp8D6;return Cyc_Absyn_new_exp((void*)((_tmp8D6=_cycalloc(sizeof(*_tmp8D6)),((_tmp8D6[0]=((_tmp8D7.tag=38,((_tmp8D7.f1=Cyc_Tcutil_copy_type(_tmp66),_tmp8D7)))),_tmp8D6)))),e->loc);}}}
# 365
goto _LL22;_LL25:;_LL26:
 goto _LL22;_LL22:;}
# 368
return e;}
# 373
static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){
{void*_tmp69=t;void*_tmp6B;struct Cyc_Absyn_Tqual _tmp6C;struct Cyc_Absyn_Exp*_tmp6D;union Cyc_Absyn_Constraint*_tmp6E;unsigned int _tmp6F;void*_tmp71;struct Cyc_Absyn_Tqual _tmp72;void*_tmp73;union Cyc_Absyn_Constraint*_tmp74;union Cyc_Absyn_Constraint*_tmp75;union Cyc_Absyn_Constraint*_tmp76;struct Cyc_Absyn_PtrLoc*_tmp77;_LL28: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp6A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp69;if(_tmp6A->tag != 8)goto _LL2A;else{_tmp6B=(_tmp6A->f1).elt_type;_tmp6C=(_tmp6A->f1).tq;_tmp6D=(_tmp6A->f1).num_elts;_tmp6E=(_tmp6A->f1).zero_term;_tmp6F=(_tmp6A->f1).zt_loc;}}_LL29: {
# 376
struct Cyc_Absyn_Exp*nelts2=_tmp6D;
if(_tmp6D != 0)
nelts2=Cyc_Parse_substitute_tags_exp(tags,_tmp6D);{
# 380
void*_tmp78=Cyc_Parse_substitute_tags(tags,_tmp6B);
if(_tmp6D != nelts2  || _tmp6B != _tmp78){
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp8DD;struct Cyc_Absyn_ArrayInfo _tmp8DC;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8DB;return(void*)((_tmp8DB=_cycalloc(sizeof(*_tmp8DB)),((_tmp8DB[0]=((_tmp8DD.tag=8,((_tmp8DD.f1=((_tmp8DC.elt_type=_tmp78,((_tmp8DC.tq=_tmp6C,((_tmp8DC.num_elts=nelts2,((_tmp8DC.zero_term=_tmp6E,((_tmp8DC.zt_loc=_tmp6F,_tmp8DC)))))))))),_tmp8DD)))),_tmp8DB))));}
goto _LL27;};}_LL2A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp70=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp69;if(_tmp70->tag != 5)goto _LL2C;else{_tmp71=(_tmp70->f1).elt_typ;_tmp72=(_tmp70->f1).elt_tq;_tmp73=((_tmp70->f1).ptr_atts).rgn;_tmp74=((_tmp70->f1).ptr_atts).nullable;_tmp75=((_tmp70->f1).ptr_atts).bounds;_tmp76=((_tmp70->f1).ptr_atts).zero_term;_tmp77=((_tmp70->f1).ptr_atts).ptrloc;}}_LL2B: {
# 385
void*_tmp7C=Cyc_Parse_substitute_tags(tags,_tmp71);
union Cyc_Absyn_Constraint*_tmp7D=_tmp75;
{union Cyc_Absyn_Constraint _tmp7E=*_tmp75;union Cyc_Absyn_Constraint _tmp7F=_tmp7E;struct Cyc_Absyn_Exp*_tmp81;_LL2F: if((_tmp7F.Eq_constr).tag != 1)goto _LL31;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp80=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)((void*)(_tmp7F.Eq_constr).val);if(_tmp80->tag != 1)goto _LL31;else{_tmp81=_tmp80->f1;}};_LL30:
# 389
 _tmp81=Cyc_Parse_substitute_tags_exp(tags,_tmp81);
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp8E3;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp8E2;union Cyc_Absyn_Constraint*_tmp8E1;_tmp7D=((_tmp8E1=_cycalloc(sizeof(*_tmp8E1)),(((_tmp8E1->Eq_constr).val=(void*)((_tmp8E3=_cycalloc(sizeof(*_tmp8E3)),((_tmp8E3[0]=((_tmp8E2.tag=1,((_tmp8E2.f1=_tmp81,_tmp8E2)))),_tmp8E3)))),(((_tmp8E1->Eq_constr).tag=1,_tmp8E1))))));}
goto _LL2E;_LL31:;_LL32:
 goto _LL2E;_LL2E:;}
# 394
if(_tmp7C != _tmp71  || _tmp7D != _tmp75){
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp8E9;struct Cyc_Absyn_PtrInfo _tmp8E8;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8E7;return(void*)((_tmp8E7=_cycalloc(sizeof(*_tmp8E7)),((_tmp8E7[0]=((_tmp8E9.tag=5,((_tmp8E9.f1=((_tmp8E8.elt_typ=_tmp7C,((_tmp8E8.elt_tq=_tmp72,((_tmp8E8.ptr_atts=(((_tmp8E8.ptr_atts).rgn=_tmp73,(((_tmp8E8.ptr_atts).nullable=_tmp74,(((_tmp8E8.ptr_atts).bounds=_tmp7D,(((_tmp8E8.ptr_atts).zero_term=_tmp76,(((_tmp8E8.ptr_atts).ptrloc=_tmp77,_tmp8E8.ptr_atts)))))))))),_tmp8E8)))))),_tmp8E9)))),_tmp8E7))));}
goto _LL27;}_LL2C:;_LL2D:
# 399
 goto _LL27;_LL27:;}
# 401
return t;}
# 406
static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,struct Cyc_Absyn_Aggrfield*x){
x->type=Cyc_Parse_substitute_tags(tags,x->type);}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};
# 413
static struct _tuple19*Cyc_Parse_get_tqual_typ(unsigned int loc,struct _tuple8*t){
# 415
struct _tuple19*_tmp8EA;return(_tmp8EA=_cycalloc(sizeof(*_tmp8EA)),((_tmp8EA->f1=(*t).f2,((_tmp8EA->f2=(*t).f3,_tmp8EA)))));}
# 418
static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d){
void*_tmp89=d->r;void*_tmp8A=_tmp89;struct Cyc_Absyn_Vardecl*_tmp8C;_LL34: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp8B=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8A;if(_tmp8B->tag != 0)goto _LL36;else{_tmp8C=_tmp8B->f1;}}_LL35:
 return _tmp8C;_LL36:;_LL37: {
const char*_tmp8ED;void*_tmp8EC;(_tmp8EC=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(d->loc,((_tmp8ED="bad declaration in `forarray' statement",_tag_dyneither(_tmp8ED,sizeof(char),40))),_tag_dyneither(_tmp8EC,sizeof(void*),0)));}_LL33:;}
# 425
static int Cyc_Parse_is_typeparam(void*tm){
void*_tmp8F=tm;_LL39: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp90=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp8F;if(_tmp90->tag != 4)goto _LL3B;}_LL3A:
 return 1;_LL3B:;_LL3C:
 return 0;_LL38:;}
# 434
static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k){
const char*_tmp8EE;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8EE="`H",_tag_dyneither(_tmp8EE,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_HeapRgn_val;else{
const char*_tmp8EF;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8EF="`U",_tag_dyneither(_tmp8EF,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_UniqueRgn_val;else{
const char*_tmp8F0;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8F0="`RC",_tag_dyneither(_tmp8F0,sizeof(char),4))))== 0)
return(void*)& Cyc_Absyn_RefCntRgn_val;else{
# 442
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8FA;struct _dyneither_ptr*_tmp8F9;struct Cyc_Absyn_Tvar*_tmp8F8;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8F7;return(void*)((_tmp8F7=_cycalloc(sizeof(*_tmp8F7)),((_tmp8F7[0]=((_tmp8FA.tag=2,((_tmp8FA.f1=((_tmp8F8=_cycalloc(sizeof(*_tmp8F8)),((_tmp8F8->name=((_tmp8F9=_cycalloc(sizeof(*_tmp8F9)),((_tmp8F9[0]=s,_tmp8F9)))),((_tmp8F8->identity=- 1,((_tmp8F8->kind=k,_tmp8F8)))))))),_tmp8FA)))),_tmp8F7))));}}}}
# 445
static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t){
void*k;
{void*_tmp98=Cyc_Absyn_compress_kb(t->kind);void*_tmp99=_tmp98;struct Cyc_Absyn_Kind*_tmp9C;void*_tmp9D;_LL3E: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp9A=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9A->tag != 1)goto _LL40;}_LL3F:
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp8FD;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp8FC;k=(void*)((_tmp8FC=_cycalloc(sizeof(*_tmp8FC)),((_tmp8FC[0]=((_tmp8FD.tag=1,((_tmp8FD.f1=0,_tmp8FD)))),_tmp8FC))));}goto _LL3D;_LL40: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9B->tag != 2)goto _LL42;else{_tmp9C=_tmp9B->f2;}}_LL41:
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp900;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8FF;k=(void*)((_tmp8FF=_cycalloc(sizeof(*_tmp8FF)),((_tmp8FF[0]=((_tmp900.tag=2,((_tmp900.f1=0,((_tmp900.f2=_tmp9C,_tmp900)))))),_tmp8FF))));}goto _LL3D;_LL42: _tmp9D=_tmp99;_LL43:
 k=_tmp9D;goto _LL3D;_LL3D:;}{
# 452
struct Cyc_Absyn_Tvar*_tmp901;return(_tmp901=_cycalloc(sizeof(*_tmp901)),((_tmp901->name=t->name,((_tmp901->identity=- 1,((_tmp901->kind=k,_tmp901)))))));};}
# 459
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned int loc,void*t){
void*_tmpA3=t;struct Cyc_Absyn_Tvar*_tmpA5;_LL45: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA4=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA3;if(_tmpA4->tag != 2)goto _LL47;else{_tmpA5=_tmpA4->f1;}}_LL46:
 return _tmpA5;_LL47:;_LL48: {
const char*_tmp904;void*_tmp903;(_tmp903=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp904="expecting a list of type variables, not types",_tag_dyneither(_tmp904,sizeof(char),46))),_tag_dyneither(_tmp903,sizeof(void*),0)));}_LL44:;}
# 465
static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp907;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp906;return(void*)((_tmp906=_cycalloc(sizeof(*_tmp906)),((_tmp906[0]=((_tmp907.tag=2,((_tmp907.f1=pr,_tmp907)))),_tmp906))));}
# 470
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_tmpAA=Cyc_Tcutil_compress(t);void*_tmpAB=_tmpAA;void**_tmpAD;_LL4A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpAC=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpAB;if(_tmpAC->tag != 2)goto _LL4C;else{_tmpAD=(void**)&(_tmpAC->f1)->kind;}}_LL4B: {
# 473
void*_tmpAE=Cyc_Absyn_compress_kb(*_tmpAD);void*_tmpAF=_tmpAE;_LL4F: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpB0=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpAF;if(_tmpB0->tag != 1)goto _LL51;}_LL50:
# 475
 if(!leq)*_tmpAD=Cyc_Tcutil_kind_to_bound(k);else{
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp90A;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp909;*_tmpAD=(void*)((_tmp909=_cycalloc(sizeof(*_tmp909)),((_tmp909[0]=((_tmp90A.tag=2,((_tmp90A.f1=0,((_tmp90A.f2=k,_tmp90A)))))),_tmp909))));}
return;_LL51:;_LL52:
 return;_LL4E:;}_LL4C:;_LL4D:
# 480
 return;_LL49:;}
# 485
static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(struct _RegionHandle*yy,struct Cyc_List_List*tms,struct Cyc_List_List*tds,unsigned int loc){
# 491
if(tds == 0)return tms;
# 496
if(tms == 0)return 0;{
# 498
void*_tmpB3=(void*)tms->hd;void*_tmpB4=_tmpB3;void*_tmpB6;_LL54: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpB5=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpB4;if(_tmpB5->tag != 3)goto _LL56;else{_tmpB6=(void*)_tmpB5->f1;}}_LL55:
# 501
 if(tms->tl == 0  || 
Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd) && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){
# 504
void*_tmpB7=_tmpB6;struct Cyc_List_List*_tmpBA;_LL59: {struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpB8=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpB7;if(_tmpB8->tag != 1)goto _LL5B;}_LL5A:
# 506
{const char*_tmp90D;void*_tmp90C;(_tmp90C=0,Cyc_Tcutil_warn(loc,((_tmp90D="function declaration with both new- and old-style parameter declarations; ignoring old-style",_tag_dyneither(_tmp90D,sizeof(char),93))),_tag_dyneither(_tmp90C,sizeof(void*),0)));}
# 508
return tms;_LL5B: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpB9=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpB7;if(_tmpB9->tag != 0)goto _LL58;else{_tmpBA=_tmpB9->f1;}}_LL5C:
# 510
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpBA)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds)){
const char*_tmp910;void*_tmp90F;(_tmp90F=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp910="wrong number of parameter declarations in old-style function declaration",_tag_dyneither(_tmp910,sizeof(char),73))),_tag_dyneither(_tmp90F,sizeof(void*),0)));}{
# 514
struct Cyc_List_List*rev_new_params=0;
for(0;_tmpBA != 0;_tmpBA=_tmpBA->tl){
struct Cyc_List_List*_tmpBF=tds;
for(0;_tmpBF != 0;_tmpBF=_tmpBF->tl){
struct Cyc_Absyn_Decl*_tmpC0=(struct Cyc_Absyn_Decl*)_tmpBF->hd;
void*_tmpC1=_tmpC0->r;void*_tmpC2=_tmpC1;struct Cyc_Absyn_Vardecl*_tmpC4;_LL5E: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpC3=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpC2;if(_tmpC3->tag != 0)goto _LL60;else{_tmpC4=_tmpC3->f1;}}_LL5F:
# 521
 if(Cyc_zstrptrcmp((*_tmpC4->name).f2,(struct _dyneither_ptr*)_tmpBA->hd)!= 0)
continue;
if(_tmpC4->initializer != 0){
const char*_tmp913;void*_tmp912;(_tmp912=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpC0->loc,((_tmp913="initializer found in parameter declaration",_tag_dyneither(_tmp913,sizeof(char),43))),_tag_dyneither(_tmp912,sizeof(void*),0)));}
if(Cyc_Absyn_is_qvar_qualified(_tmpC4->name)){
const char*_tmp916;void*_tmp915;(_tmp915=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpC0->loc,((_tmp916="namespaces forbidden in parameter declarations",_tag_dyneither(_tmp916,sizeof(char),47))),_tag_dyneither(_tmp915,sizeof(void*),0)));}
{struct _tuple8*_tmp919;struct Cyc_List_List*_tmp918;rev_new_params=(
(_tmp918=_cycalloc(sizeof(*_tmp918)),((_tmp918->hd=((_tmp919=_cycalloc(sizeof(*_tmp919)),((_tmp919->f1=(*_tmpC4->name).f2,((_tmp919->f2=_tmpC4->tq,((_tmp919->f3=_tmpC4->type,_tmp919)))))))),((_tmp918->tl=rev_new_params,_tmp918))))));}
# 530
goto L;_LL60:;_LL61: {
const char*_tmp91C;void*_tmp91B;(_tmp91B=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpC0->loc,((_tmp91C="nonvariable declaration in parameter type",_tag_dyneither(_tmp91C,sizeof(char),42))),_tag_dyneither(_tmp91B,sizeof(void*),0)));}_LL5D:;}
# 534
L: if(_tmpBF == 0){
const char*_tmp920;void*_tmp91F[1];struct Cyc_String_pa_PrintArg_struct _tmp91E;(_tmp91E.tag=0,((_tmp91E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmpBA->hd)),((_tmp91F[0]=& _tmp91E,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp920="%s is not given a type",_tag_dyneither(_tmp920,sizeof(char),23))),_tag_dyneither(_tmp91F,sizeof(void*),1)))))));}}{
# 537
struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp92F;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp92E;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp92D;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp92C;struct Cyc_List_List*_tmp92B;return
(_tmp92B=_region_malloc(yy,sizeof(*_tmp92B)),((_tmp92B->hd=(void*)((_tmp92F=_region_malloc(yy,sizeof(*_tmp92F)),((_tmp92F[0]=((_tmp92C.tag=3,((_tmp92C.f1=(void*)((_tmp92E=_region_malloc(yy,sizeof(*_tmp92E)),((_tmp92E[0]=((_tmp92D.tag=1,((_tmp92D.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_new_params),((_tmp92D.f2=0,((_tmp92D.f3=0,((_tmp92D.f4=0,((_tmp92D.f5=0,((_tmp92D.f6=0,((_tmp92D.f7=0,_tmp92D)))))))))))))))),_tmp92E)))),_tmp92C)))),_tmp92F)))),((_tmp92B->tl=0,_tmp92B)))));};};_LL58:;}
# 544
goto _LL57;_LL56:;_LL57: {
struct Cyc_List_List*_tmp930;return(_tmp930=_region_malloc(yy,sizeof(*_tmp930)),((_tmp930->hd=(void*)tms->hd,((_tmp930->tl=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc),_tmp930)))));}_LL53:;};}
# 552
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned int loc){
# 556
if(tds != 0){
struct Cyc_Parse_Declarator _tmp931;d=((_tmp931.id=d.id,((_tmp931.tms=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc),_tmp931))));}{
# 559
enum Cyc_Absyn_Scope sc=Cyc_Absyn_Public;
struct Cyc_Parse_Type_specifier tss=Cyc_Parse_empty_spec(loc);
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);
int is_inline=0;
struct Cyc_List_List*atts=0;
# 565
if(dso != 0){
tss=dso->type_specs;
tq=dso->tq;
is_inline=dso->is_inline;
atts=dso->attributes;
# 571
if(dso->sc != 0){
enum Cyc_Parse_Storage_class _tmpD7=*((enum Cyc_Parse_Storage_class*)_check_null(dso->sc));switch(_tmpD7){case Cyc_Parse_Extern_sc: _LL62:
 sc=Cyc_Absyn_Extern;break;case Cyc_Parse_Static_sc: _LL63:
 sc=Cyc_Absyn_Static;break;default: _LL64:
{const char*_tmp932;Cyc_Parse_err(((_tmp932="bad storage class on function",_tag_dyneither(_tmp932,sizeof(char),30))),loc);}break;}}}{
# 578
void*_tmpD9=Cyc_Parse_collapse_type_specifiers(tss,loc);
struct _tuple13 _tmpDA=Cyc_Parse_apply_tms(tq,_tmpD9,atts,d.tms);struct Cyc_Absyn_Tqual _tmpDC;void*_tmpDD;struct Cyc_List_List*_tmpDE;struct Cyc_List_List*_tmpDF;struct _tuple13 _tmpDB=_tmpDA;_tmpDC=_tmpDB.f1;_tmpDD=_tmpDB.f2;_tmpDE=_tmpDB.f3;_tmpDF=_tmpDB.f4;
# 583
if(_tmpDE != 0){
# 586
const char*_tmp935;void*_tmp934;(_tmp934=0,Cyc_Tcutil_warn(loc,((_tmp935="bad type params, ignoring",_tag_dyneither(_tmp935,sizeof(char),26))),_tag_dyneither(_tmp934,sizeof(void*),0)));}{
# 588
void*_tmpE2=_tmpDD;struct Cyc_List_List*_tmpE4;void*_tmpE5;struct Cyc_Absyn_Tqual _tmpE6;void*_tmpE7;struct Cyc_List_List*_tmpE8;int _tmpE9;struct Cyc_Absyn_VarargInfo*_tmpEA;struct Cyc_List_List*_tmpEB;struct Cyc_List_List*_tmpEC;struct Cyc_Absyn_Exp*_tmpED;struct Cyc_List_List*_tmpEE;struct Cyc_Absyn_Exp*_tmpEF;struct Cyc_List_List*_tmpF0;_LL67: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE3=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE2;if(_tmpE3->tag != 9)goto _LL69;else{_tmpE4=(_tmpE3->f1).tvars;_tmpE5=(_tmpE3->f1).effect;_tmpE6=(_tmpE3->f1).ret_tqual;_tmpE7=(_tmpE3->f1).ret_typ;_tmpE8=(_tmpE3->f1).args;_tmpE9=(_tmpE3->f1).c_varargs;_tmpEA=(_tmpE3->f1).cyc_varargs;_tmpEB=(_tmpE3->f1).rgn_po;_tmpEC=(_tmpE3->f1).attributes;_tmpED=(_tmpE3->f1).requires_clause;_tmpEE=(_tmpE3->f1).requires_relns;_tmpEF=(_tmpE3->f1).ensures_clause;_tmpF0=(_tmpE3->f1).ensures_relns;}}_LL68: {
# 592
struct Cyc_List_List*_tmpF1=0;
{struct Cyc_List_List*_tmpF2=_tmpE8;for(0;_tmpF2 != 0;_tmpF2=_tmpF2->tl){
struct _tuple8*_tmpF3=(struct _tuple8*)_tmpF2->hd;struct _dyneither_ptr*_tmpF5;struct Cyc_Absyn_Tqual _tmpF6;void*_tmpF7;struct _tuple8*_tmpF4=_tmpF3;_tmpF5=_tmpF4->f1;_tmpF6=_tmpF4->f2;_tmpF7=_tmpF4->f3;
if(_tmpF5 == 0){
{const char*_tmp936;Cyc_Parse_err(((_tmp936="missing argument variable in function prototype",_tag_dyneither(_tmp936,sizeof(char),48))),loc);}{
struct _tuple8*_tmp939;struct Cyc_List_List*_tmp938;_tmpF1=((_tmp938=_cycalloc(sizeof(*_tmp938)),((_tmp938->hd=((_tmp939=_cycalloc(sizeof(*_tmp939)),((_tmp939->f1=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"?",sizeof(char),2),((_tmp939->f2=_tmpF6,((_tmp939->f3=_tmpF7,_tmp939)))))))),((_tmp938->tl=_tmpF1,_tmp938))))));};}else{
# 599
struct _tuple8*_tmp93C;struct Cyc_List_List*_tmp93B;_tmpF1=((_tmp93B=_cycalloc(sizeof(*_tmp93B)),((_tmp93B->hd=((_tmp93C=_cycalloc(sizeof(*_tmp93C)),((_tmp93C->f1=_tmpF5,((_tmp93C->f2=_tmpF6,((_tmp93C->f3=_tmpF7,_tmp93C)))))))),((_tmp93B->tl=_tmpF1,_tmp93B))))));}}}{
# 603
struct Cyc_Absyn_Fndecl*_tmp93D;return(_tmp93D=_cycalloc(sizeof(*_tmp93D)),((_tmp93D->sc=sc,((_tmp93D->name=d.id,((_tmp93D->tvs=_tmpE4,((_tmp93D->is_inline=is_inline,((_tmp93D->effect=_tmpE5,((_tmp93D->ret_tqual=_tmpE6,((_tmp93D->ret_type=_tmpE7,((_tmp93D->args=
# 606
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpF1),((_tmp93D->c_varargs=_tmpE9,((_tmp93D->cyc_varargs=_tmpEA,((_tmp93D->rgn_po=_tmpEB,((_tmp93D->body=body,((_tmp93D->cached_typ=0,((_tmp93D->param_vardecls=0,((_tmp93D->fn_vardecl=0,((_tmp93D->attributes=
# 613
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpEC,_tmpDF),((_tmp93D->requires_clause=_tmpED,((_tmp93D->requires_relns=0,((_tmp93D->ensures_clause=_tmpEF,((_tmp93D->ensures_relns=0,_tmp93D)))))))))))))))))))))))))))))))))))))))));};}_LL69:;_LL6A: {
# 618
const char*_tmp940;void*_tmp93F;(_tmp93F=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp940="declarator is not a function prototype",_tag_dyneither(_tmp940,sizeof(char),39))),_tag_dyneither(_tmp93F,sizeof(void*),0)));}_LL66:;};};};}static char _tmp101[76]="at most one type may appear within a type specifier \n\t(missing ';' or ','?)";
# 622
static struct _dyneither_ptr Cyc_Parse_msg1={_tmp101,_tmp101,_tmp101 + 76};static char _tmp102[87]="const or volatile may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 624
static struct _dyneither_ptr Cyc_Parse_msg2={_tmp102,_tmp102,_tmp102 + 87};static char _tmp103[74]="type specifier includes more than one declaration \n\t(missing ';' or ','?)";
# 626
static struct _dyneither_ptr Cyc_Parse_msg3={_tmp103,_tmp103,_tmp103 + 74};static char _tmp104[84]="sign specifier may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 628
static struct _dyneither_ptr Cyc_Parse_msg4={_tmp104,_tmp104,_tmp104 + 84};
# 635
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned int loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){
# 638
if(s1.Signed_spec  && s2.Signed_spec){
void*_tmp941;(_tmp941=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp941,sizeof(void*),0)));}
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec  && s2.Unsigned_spec){
void*_tmp942;(_tmp942=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp942,sizeof(void*),0)));}
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec  && s2.Short_spec){
void*_tmp943;(_tmp943=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp943,sizeof(void*),0)));}
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec  && s2.Long_Long_spec  || 
s1.Long_Long_spec  && s2.Long_spec) || 
s2.Long_Long_spec  && s1.Long_spec){
void*_tmp944;(_tmp944=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp944,sizeof(void*),0)));}
s1.Long_Long_spec=
(s1.Long_Long_spec  || s2.Long_Long_spec) || s1.Long_spec  && s2.Long_spec;
s1.Long_spec=!s1.Long_Long_spec  && (s1.Long_spec  || s2.Long_spec);
if(s1.Valid_type_spec  && s2.Valid_type_spec)
Cyc_Parse_err(Cyc_Parse_msg1,loc);else{
if(s2.Valid_type_spec){
s1.Type_spec=s2.Type_spec;
s1.Valid_type_spec=1;}}
# 660
return s1;}
# 666
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned int loc){
# 669
int seen_type=ts.Valid_type_spec;
int seen_sign=ts.Signed_spec  || ts.Unsigned_spec;
int seen_size=(ts.Short_spec  || ts.Long_spec) || ts.Long_Long_spec;
void*t=seen_type?ts.Type_spec:(void*)& Cyc_Absyn_VoidType_val;
enum Cyc_Absyn_Size_of sz=Cyc_Absyn_Int_sz;
enum Cyc_Absyn_Sign sgn=Cyc_Absyn_Signed;
# 676
if(ts.Signed_spec  && ts.Unsigned_spec)
Cyc_Parse_err(Cyc_Parse_msg4,loc);
if(ts.Unsigned_spec)sgn=Cyc_Absyn_Unsigned;
if(ts.Short_spec  && (ts.Long_spec  || ts.Long_Long_spec) || 
ts.Long_spec  && ts.Long_Long_spec)
Cyc_Parse_err(Cyc_Parse_msg4,loc);
if(ts.Short_spec)sz=Cyc_Absyn_Short_sz;
if(ts.Long_spec)sz=Cyc_Absyn_Long_sz;
if(ts.Long_Long_spec)sz=Cyc_Absyn_LongLong_sz;
# 688
if(!seen_type){
if(!seen_sign  && !seen_size){
const char*_tmp947;void*_tmp946;(_tmp946=0,Cyc_Tcutil_warn(loc,((_tmp947="missing type within specifier",_tag_dyneither(_tmp947,sizeof(char),30))),_tag_dyneither(_tmp946,sizeof(void*),0)));}
t=Cyc_Absyn_int_typ(sgn,sz);}else{
# 693
if(seen_sign){
void*_tmp10B=t;enum Cyc_Absyn_Sign _tmp10D;enum Cyc_Absyn_Size_of _tmp10E;_LL6C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp10C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp10B;if(_tmp10C->tag != 6)goto _LL6E;else{_tmp10D=_tmp10C->f1;_tmp10E=_tmp10C->f2;}}_LL6D:
# 696
 if(_tmp10D != sgn)
t=Cyc_Absyn_int_typ(sgn,_tmp10E);
goto _LL6B;_LL6E:;_LL6F:
{const char*_tmp948;Cyc_Parse_err(((_tmp948="sign specification on non-integral type",_tag_dyneither(_tmp948,sizeof(char),40))),loc);}goto _LL6B;_LL6B:;}
# 701
if(seen_size){
void*_tmp110=t;enum Cyc_Absyn_Sign _tmp112;enum Cyc_Absyn_Size_of _tmp113;_LL71: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp111=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp110;if(_tmp111->tag != 6)goto _LL73;else{_tmp112=_tmp111->f1;_tmp113=_tmp111->f2;}}_LL72:
# 704
 if(_tmp113 != sz)
t=Cyc_Absyn_int_typ(_tmp112,sz);
goto _LL70;_LL73: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp114=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp110;if(_tmp114->tag != 7)goto _LL75;}_LL74:
# 708
 t=Cyc_Absyn_float_typ(2);goto _LL70;_LL75:;_LL76:
{const char*_tmp949;Cyc_Parse_err(((_tmp949="size qualifier on non-integral type",_tag_dyneither(_tmp949,sizeof(char),36))),loc);}goto _LL70;_LL70:;}}
# 712
return t;}
# 715
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple10*ds,struct Cyc_List_List*shared_atts){
# 719
if(ds == 0)return 0;{
struct Cyc_Parse_Declarator d=ds->hd;
struct _tuple0*_tmp116=d.id;
struct _tuple13 _tmp117=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);struct Cyc_Absyn_Tqual _tmp119;void*_tmp11A;struct Cyc_List_List*_tmp11B;struct Cyc_List_List*_tmp11C;struct _tuple13 _tmp118=_tmp117;_tmp119=_tmp118.f1;_tmp11A=_tmp118.f2;_tmp11B=_tmp118.f3;_tmp11C=_tmp118.f4;
# 725
if(ds->tl == 0){
struct _tuple15*_tmp94C;struct Cyc_List_List*_tmp94B;return(_tmp94B=_region_malloc(r,sizeof(*_tmp94B)),((_tmp94B->hd=((_tmp94C=_region_malloc(r,sizeof(*_tmp94C)),((_tmp94C->f1=_tmp116,((_tmp94C->f2=_tmp119,((_tmp94C->f3=_tmp11A,((_tmp94C->f4=_tmp11B,((_tmp94C->f5=_tmp11C,_tmp94C)))))))))))),((_tmp94B->tl=0,_tmp94B)))));}else{
# 728
struct _tuple15*_tmp94F;struct Cyc_List_List*_tmp94E;return(_tmp94E=_region_malloc(r,sizeof(*_tmp94E)),((_tmp94E->hd=((_tmp94F=_region_malloc(r,sizeof(*_tmp94F)),((_tmp94F->f1=_tmp116,((_tmp94F->f2=_tmp119,((_tmp94F->f3=_tmp11A,((_tmp94F->f4=_tmp11B,((_tmp94F->f5=_tmp11C,_tmp94F)))))))))))),((_tmp94E->tl=
Cyc_Parse_apply_tmss(r,tq,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts),_tmp94E)))));}};}
# 732
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 735
if(tms == 0){struct _tuple13 _tmp950;return(_tmp950.f1=tq,((_tmp950.f2=t,((_tmp950.f3=0,((_tmp950.f4=atts,_tmp950)))))));}{
void*_tmp122=(void*)tms->hd;void*_tmp123=_tmp122;union Cyc_Absyn_Constraint*_tmp125;unsigned int _tmp126;struct Cyc_Absyn_Exp*_tmp128;union Cyc_Absyn_Constraint*_tmp129;unsigned int _tmp12A;void*_tmp12C;struct Cyc_List_List*_tmp12E;unsigned int _tmp12F;struct Cyc_Absyn_PtrAtts _tmp131;struct Cyc_Absyn_Tqual _tmp132;unsigned int _tmp134;struct Cyc_List_List*_tmp135;_LL78: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp124=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp123;if(_tmp124->tag != 0)goto _LL7A;else{_tmp125=_tmp124->f1;_tmp126=_tmp124->f2;}}_LL79:
# 738
 return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),
Cyc_Absyn_array_typ(t,tq,0,_tmp125,_tmp126),atts,tms->tl);_LL7A: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp127=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp123;if(_tmp127->tag != 1)goto _LL7C;else{_tmp128=_tmp127->f1;_tmp129=_tmp127->f2;_tmp12A=_tmp127->f3;}}_LL7B:
# 741
 return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),
Cyc_Absyn_array_typ(t,tq,_tmp128,_tmp129,_tmp12A),atts,tms->tl);_LL7C: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp12B=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp123;if(_tmp12B->tag != 3)goto _LL7E;else{_tmp12C=(void*)_tmp12B->f1;}}_LL7D: {
# 744
void*_tmp136=_tmp12C;struct Cyc_List_List*_tmp138;int _tmp139;struct Cyc_Absyn_VarargInfo*_tmp13A;void*_tmp13B;struct Cyc_List_List*_tmp13C;struct Cyc_Absyn_Exp*_tmp13D;struct Cyc_Absyn_Exp*_tmp13E;unsigned int _tmp140;_LL85: {struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp137=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp136;if(_tmp137->tag != 1)goto _LL87;else{_tmp138=_tmp137->f1;_tmp139=_tmp137->f2;_tmp13A=_tmp137->f3;_tmp13B=(void*)_tmp137->f4;_tmp13C=_tmp137->f5;_tmp13D=_tmp137->f6;_tmp13E=_tmp137->f7;}}_LL86: {
# 746
struct Cyc_List_List*typvars=0;
# 748
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){
if(Cyc_Absyn_fntype_att((void*)as->hd)){
struct Cyc_List_List*_tmp951;fn_atts=((_tmp951=_cycalloc(sizeof(*_tmp951)),((_tmp951->hd=(void*)as->hd,((_tmp951->tl=fn_atts,_tmp951))))));}else{
# 753
struct Cyc_List_List*_tmp952;new_atts=((_tmp952=_cycalloc(sizeof(*_tmp952)),((_tmp952->hd=(void*)as->hd,((_tmp952->tl=new_atts,_tmp952))))));}}}
# 756
if(tms->tl != 0){
void*_tmp143=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_tmp144=_tmp143;struct Cyc_List_List*_tmp146;_LL8A: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp145=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp144;if(_tmp145->tag != 4)goto _LL8C;else{_tmp146=_tmp145->f1;}}_LL8B:
# 759
 typvars=_tmp146;
tms=tms->tl;
goto _LL89;_LL8C:;_LL8D:
# 763
 goto _LL89;_LL89:;}
# 767
if(((((!_tmp139  && _tmp13A == 0) && _tmp138 != 0) && _tmp138->tl == 0) && (*((struct _tuple8*)_tmp138->hd)).f1 == 0) && (*((struct _tuple8*)_tmp138->hd)).f3 == (void*)& Cyc_Absyn_VoidType_val)
# 772
_tmp138=0;{
# 775
struct Cyc_List_List*_tmp147=Cyc_Parse_get_arg_tags(_tmp138);
# 777
if(_tmp147 != 0)
t=Cyc_Parse_substitute_tags(_tmp147,t);
t=Cyc_Parse_array2ptr(t,0);
# 782
{struct Cyc_List_List*_tmp148=_tmp138;for(0;_tmp148 != 0;_tmp148=_tmp148->tl){
struct _tuple8*_tmp149=(struct _tuple8*)_tmp148->hd;struct _dyneither_ptr*_tmp14B;struct Cyc_Absyn_Tqual _tmp14C;void**_tmp14D;struct _tuple8*_tmp14A=_tmp149;_tmp14B=_tmp14A->f1;_tmp14C=_tmp14A->f2;_tmp14D=(void**)& _tmp14A->f3;
if(_tmp147 != 0)
*_tmp14D=Cyc_Parse_substitute_tags(_tmp147,*_tmp14D);
*_tmp14D=Cyc_Parse_array2ptr(*_tmp14D,1);}}
# 794
return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(tq.loc),
Cyc_Absyn_function_typ(typvars,_tmp13B,tq,t,_tmp138,_tmp139,_tmp13A,_tmp13C,fn_atts,_tmp13D,_tmp13E),new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);};}_LL87: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp13F=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp136;if(_tmp13F->tag != 0)goto _LL84;else{_tmp140=_tmp13F->f2;}}_LL88: {
# 801
const char*_tmp955;void*_tmp954;(_tmp954=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp140,((_tmp955="function declaration without parameter types",_tag_dyneither(_tmp955,sizeof(char),45))),_tag_dyneither(_tmp954,sizeof(void*),0)));}_LL84:;}_LL7E: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp12D=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp123;if(_tmp12D->tag != 4)goto _LL80;else{_tmp12E=_tmp12D->f1;_tmp12F=_tmp12D->f2;}}_LL7F:
# 808
 if(tms->tl == 0){
struct _tuple13 _tmp956;return(_tmp956.f1=tq,((_tmp956.f2=t,((_tmp956.f3=_tmp12E,((_tmp956.f4=atts,_tmp956)))))));}{
# 813
const char*_tmp959;void*_tmp958;(_tmp958=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp12F,((_tmp959="type parameters must appear before function arguments in declarator",_tag_dyneither(_tmp959,sizeof(char),68))),_tag_dyneither(_tmp958,sizeof(void*),0)));};_LL80: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp130=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp123;if(_tmp130->tag != 2)goto _LL82;else{_tmp131=_tmp130->f1;_tmp132=_tmp130->f2;}}_LL81: {
# 816
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp95F;struct Cyc_Absyn_PtrInfo _tmp95E;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp95D;return Cyc_Parse_apply_tms(_tmp132,(void*)((_tmp95D=_cycalloc(sizeof(*_tmp95D)),((_tmp95D[0]=((_tmp95F.tag=5,((_tmp95F.f1=((_tmp95E.elt_typ=t,((_tmp95E.elt_tq=tq,((_tmp95E.ptr_atts=_tmp131,_tmp95E)))))),_tmp95F)))),_tmp95D)))),atts,tms->tl);}_LL82: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp133=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp123;if(_tmp133->tag != 5)goto _LL77;else{_tmp134=_tmp133->f1;_tmp135=_tmp133->f2;}}_LL83:
# 821
 return Cyc_Parse_apply_tms(tq,t,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(atts,_tmp135),tms->tl);_LL77:;};}
# 827
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned int loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 836
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned int loc,struct _tuple15*t){
struct _tuple0*_tmp157;struct Cyc_Absyn_Tqual _tmp158;void*_tmp159;struct Cyc_List_List*_tmp15A;struct Cyc_List_List*_tmp15B;struct _tuple15*_tmp156=t;_tmp157=_tmp156->f1;_tmp158=_tmp156->f2;_tmp159=_tmp156->f3;_tmp15A=_tmp156->f4;_tmp15B=_tmp156->f5;
# 839
Cyc_Lex_register_typedef(_tmp157);{
# 841
struct Cyc_Core_Opt*kind;
void*type;
{void*_tmp15C=_tmp159;struct Cyc_Core_Opt*_tmp15E;_LL8F: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp15D=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp15C;if(_tmp15D->tag != 1)goto _LL91;else{_tmp15E=_tmp15D->f1;}}_LL90:
# 845
 type=0;
if(_tmp15E == 0)kind=& Cyc_Tcutil_bko;else{
kind=_tmp15E;}
goto _LL8E;_LL91:;_LL92:
 kind=0;type=_tmp159;goto _LL8E;_LL8E:;}{
# 851
struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct _tmp965;struct Cyc_Absyn_Typedefdecl*_tmp964;struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp963;return Cyc_Absyn_new_decl((void*)((_tmp963=_cycalloc(sizeof(*_tmp963)),((_tmp963[0]=((_tmp965.tag=8,((_tmp965.f1=((_tmp964=_cycalloc(sizeof(*_tmp964)),((_tmp964->name=_tmp157,((_tmp964->tvs=_tmp15A,((_tmp964->kind=kind,((_tmp964->defn=type,((_tmp964->atts=_tmp15B,((_tmp964->tq=_tmp158,_tmp964)))))))))))))),_tmp965)))),_tmp963)))),loc);};};}
# 858
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp968;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp967;return Cyc_Absyn_new_stmt((void*)((_tmp967=_cycalloc(sizeof(*_tmp967)),((_tmp967[0]=((_tmp968.tag=12,((_tmp968.f1=d,((_tmp968.f2=s,_tmp968)))))),_tmp967)))),d->loc);}
# 864
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
# 873
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple12*ids,unsigned int tqual_loc,unsigned int loc){
# 877
struct _RegionHandle _tmp164=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmp164;_push_region(mkrgn);
{struct Cyc_Absyn_Tqual _tmp166;struct Cyc_Parse_Type_specifier _tmp167;struct Cyc_List_List*_tmp168;struct Cyc_Parse_Declaration_spec _tmp165=ds;_tmp166=_tmp165.tq;_tmp167=_tmp165.type_specs;_tmp168=_tmp165.attributes;
if(_tmp166.loc == 0)_tmp166.loc=tqual_loc;
if(ds.is_inline){
const char*_tmp969;Cyc_Parse_err(((_tmp969="inline is allowed only on function definitions",_tag_dyneither(_tmp969,sizeof(char),47))),loc);}{
# 883
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
# 899
struct _tuple10*declarators=0;
struct Cyc_List_List*exprs=0;
Cyc_Parse_decl_split(mkrgn,ids,& declarators,& exprs);{
# 903
int exps_empty=1;
{struct Cyc_List_List*es=exprs;for(0;es != 0;es=es->tl){
if((struct Cyc_Absyn_Exp*)es->hd != 0){
exps_empty=0;
break;}}}{
# 911
void*_tmp16B=Cyc_Parse_collapse_type_specifiers(_tmp167,loc);
if(declarators == 0){
# 915
void*_tmp16C=_tmp16B;struct Cyc_Absyn_Aggrdecl*_tmp16F;struct Cyc_Absyn_Enumdecl*_tmp172;struct Cyc_Absyn_Datatypedecl*_tmp175;enum Cyc_Absyn_AggrKind _tmp177;struct _tuple0*_tmp178;struct Cyc_List_List*_tmp179;struct Cyc_Absyn_Datatypedecl**_tmp17B;struct _tuple0*_tmp17D;int _tmp17E;struct Cyc_List_List*_tmp17F;struct _tuple0*_tmp181;struct Cyc_List_List*_tmp183;_LL9C: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp16D=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp16C;if(_tmp16D->tag != 26)goto _LL9E;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp16E=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp16D->f1)->r;if(_tmp16E->tag != 0)goto _LL9E;else{_tmp16F=_tmp16E->f1;}}}_LL9D:
# 917
 _tmp16F->attributes=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp16F->attributes,_tmp168);
_tmp16F->sc=s;{
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp96F;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp96E;struct Cyc_List_List*_tmp96D;struct Cyc_List_List*_tmp187=(_tmp96D=_cycalloc(sizeof(*_tmp96D)),((_tmp96D->hd=Cyc_Absyn_new_decl((void*)((_tmp96F=_cycalloc(sizeof(*_tmp96F)),((_tmp96F[0]=((_tmp96E.tag=5,((_tmp96E.f1=_tmp16F,_tmp96E)))),_tmp96F)))),loc),((_tmp96D->tl=0,_tmp96D)))));_npop_handler(0);return _tmp187;};_LL9E: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp170=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp16C;if(_tmp170->tag != 26)goto _LLA0;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp171=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp170->f1)->r;if(_tmp171->tag != 1)goto _LLA0;else{_tmp172=_tmp171->f1;}}}_LL9F:
# 921
 if(_tmp168 != 0){const char*_tmp970;Cyc_Parse_err(((_tmp970="attributes on enum not supported",_tag_dyneither(_tmp970,sizeof(char),33))),loc);}
_tmp172->sc=s;{
struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp976;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp975;struct Cyc_List_List*_tmp974;struct Cyc_List_List*_tmp18C=(_tmp974=_cycalloc(sizeof(*_tmp974)),((_tmp974->hd=Cyc_Absyn_new_decl((void*)((_tmp976=_cycalloc(sizeof(*_tmp976)),((_tmp976[0]=((_tmp975.tag=7,((_tmp975.f1=_tmp172,_tmp975)))),_tmp976)))),loc),((_tmp974->tl=0,_tmp974)))));_npop_handler(0);return _tmp18C;};_LLA0: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp173=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp16C;if(_tmp173->tag != 26)goto _LLA2;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp174=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp173->f1)->r;if(_tmp174->tag != 2)goto _LLA2;else{_tmp175=_tmp174->f1;}}}_LLA1:
# 925
 if(_tmp168 != 0){const char*_tmp977;Cyc_Parse_err(((_tmp977="attributes on datatypes not supported",_tag_dyneither(_tmp977,sizeof(char),38))),loc);}
_tmp175->sc=s;{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp97D;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp97C;struct Cyc_List_List*_tmp97B;struct Cyc_List_List*_tmp191=(_tmp97B=_cycalloc(sizeof(*_tmp97B)),((_tmp97B->hd=Cyc_Absyn_new_decl((void*)((_tmp97D=_cycalloc(sizeof(*_tmp97D)),((_tmp97D[0]=((_tmp97C.tag=6,((_tmp97C.f1=_tmp175,_tmp97C)))),_tmp97D)))),loc),((_tmp97B->tl=0,_tmp97B)))));_npop_handler(0);return _tmp191;};_LLA2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp176=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp16C;if(_tmp176->tag != 11)goto _LLA4;else{if((((_tmp176->f1).aggr_info).UnknownAggr).tag != 1)goto _LLA4;_tmp177=((struct _tuple2)(((_tmp176->f1).aggr_info).UnknownAggr).val).f1;_tmp178=((struct _tuple2)(((_tmp176->f1).aggr_info).UnknownAggr).val).f2;_tmp179=(_tmp176->f1).targs;}}_LLA3: {
# 929
struct Cyc_List_List*_tmp192=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp179);
struct Cyc_Absyn_Aggrdecl*_tmp97E;struct Cyc_Absyn_Aggrdecl*_tmp193=(_tmp97E=_cycalloc(sizeof(*_tmp97E)),((_tmp97E->kind=_tmp177,((_tmp97E->sc=s,((_tmp97E->name=_tmp178,((_tmp97E->tvs=_tmp192,((_tmp97E->impl=0,((_tmp97E->attributes=0,_tmp97E)))))))))))));
if(_tmp168 != 0){const char*_tmp97F;Cyc_Parse_err(((_tmp97F="bad attributes on type declaration",_tag_dyneither(_tmp97F,sizeof(char),35))),loc);}{
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp985;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp984;struct Cyc_List_List*_tmp983;struct Cyc_List_List*_tmp198=(_tmp983=_cycalloc(sizeof(*_tmp983)),((_tmp983->hd=Cyc_Absyn_new_decl((void*)((_tmp985=_cycalloc(sizeof(*_tmp985)),((_tmp985[0]=((_tmp984.tag=5,((_tmp984.f1=_tmp193,_tmp984)))),_tmp985)))),loc),((_tmp983->tl=0,_tmp983)))));_npop_handler(0);return _tmp198;};}_LLA4: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp17A=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp16C;if(_tmp17A->tag != 3)goto _LLA6;else{if((((_tmp17A->f1).datatype_info).KnownDatatype).tag != 2)goto _LLA6;_tmp17B=(struct Cyc_Absyn_Datatypedecl**)(((_tmp17A->f1).datatype_info).KnownDatatype).val;}}_LLA5:
# 934
 if(_tmp168 != 0){const char*_tmp986;Cyc_Parse_err(((_tmp986="bad attributes on datatype",_tag_dyneither(_tmp986,sizeof(char),27))),loc);}{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp98C;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp98B;struct Cyc_List_List*_tmp98A;struct Cyc_List_List*_tmp19E=(_tmp98A=_cycalloc(sizeof(*_tmp98A)),((_tmp98A->hd=Cyc_Absyn_new_decl((void*)((_tmp98C=_cycalloc(sizeof(*_tmp98C)),((_tmp98C[0]=((_tmp98B.tag=6,((_tmp98B.f1=*_tmp17B,_tmp98B)))),_tmp98C)))),loc),((_tmp98A->tl=0,_tmp98A)))));_npop_handler(0);return _tmp19E;};_LLA6: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp17C=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp16C;if(_tmp17C->tag != 3)goto _LLA8;else{if((((_tmp17C->f1).datatype_info).UnknownDatatype).tag != 1)goto _LLA8;_tmp17D=((struct Cyc_Absyn_UnknownDatatypeInfo)(((_tmp17C->f1).datatype_info).UnknownDatatype).val).name;_tmp17E=((struct Cyc_Absyn_UnknownDatatypeInfo)(((_tmp17C->f1).datatype_info).UnknownDatatype).val).is_extensible;_tmp17F=(_tmp17C->f1).targs;}}_LLA7: {
# 937
struct Cyc_List_List*_tmp19F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp17F);
struct Cyc_Absyn_Decl*_tmp1A0=Cyc_Absyn_datatype_decl(s,_tmp17D,_tmp19F,0,_tmp17E,loc);
if(_tmp168 != 0){const char*_tmp98D;Cyc_Parse_err(((_tmp98D="bad attributes on datatype",_tag_dyneither(_tmp98D,sizeof(char),27))),loc);}{
struct Cyc_List_List*_tmp98E;struct Cyc_List_List*_tmp1A3=(_tmp98E=_cycalloc(sizeof(*_tmp98E)),((_tmp98E->hd=_tmp1A0,((_tmp98E->tl=0,_tmp98E)))));_npop_handler(0);return _tmp1A3;};}_LLA8: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp180=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp16C;if(_tmp180->tag != 13)goto _LLAA;else{_tmp181=_tmp180->f1;}}_LLA9: {
# 942
struct Cyc_Absyn_Enumdecl*_tmp98F;struct Cyc_Absyn_Enumdecl*_tmp1A4=(_tmp98F=_cycalloc(sizeof(*_tmp98F)),((_tmp98F->sc=s,((_tmp98F->name=_tmp181,((_tmp98F->fields=0,_tmp98F)))))));
if(_tmp168 != 0){const char*_tmp990;Cyc_Parse_err(((_tmp990="bad attributes on enum",_tag_dyneither(_tmp990,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp99A;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp999;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp998;struct Cyc_List_List*_tmp997;struct Cyc_List_List*_tmp1AA=(_tmp997=_cycalloc(sizeof(*_tmp997)),((_tmp997->hd=((_tmp99A=_cycalloc(sizeof(*_tmp99A)),((_tmp99A->r=(void*)((_tmp998=_cycalloc(sizeof(*_tmp998)),((_tmp998[0]=((_tmp999.tag=7,((_tmp999.f1=_tmp1A4,_tmp999)))),_tmp998)))),((_tmp99A->loc=loc,_tmp99A)))))),((_tmp997->tl=0,_tmp997)))));_npop_handler(0);return _tmp1AA;};}_LLAA: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp182=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp16C;if(_tmp182->tag != 14)goto _LLAC;else{_tmp183=_tmp182->f1;}}_LLAB: {
# 948
struct Cyc_Core_Opt*_tmp99D;struct Cyc_Absyn_Enumdecl*_tmp99C;struct Cyc_Absyn_Enumdecl*_tmp1AC=(_tmp99C=_cycalloc(sizeof(*_tmp99C)),((_tmp99C->sc=s,((_tmp99C->name=Cyc_Parse_gensym_enum(),((_tmp99C->fields=((_tmp99D=_cycalloc(sizeof(*_tmp99D)),((_tmp99D->v=_tmp183,_tmp99D)))),_tmp99C)))))));
if(_tmp168 != 0){const char*_tmp99E;Cyc_Parse_err(((_tmp99E="bad attributes on enum",_tag_dyneither(_tmp99E,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp9A8;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp9A7;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp9A6;struct Cyc_List_List*_tmp9A5;struct Cyc_List_List*_tmp1B2=(_tmp9A5=_cycalloc(sizeof(*_tmp9A5)),((_tmp9A5->hd=((_tmp9A8=_cycalloc(sizeof(*_tmp9A8)),((_tmp9A8->r=(void*)((_tmp9A6=_cycalloc(sizeof(*_tmp9A6)),((_tmp9A6[0]=((_tmp9A7.tag=7,((_tmp9A7.f1=_tmp1AC,_tmp9A7)))),_tmp9A6)))),((_tmp9A8->loc=loc,_tmp9A8)))))),((_tmp9A5->tl=0,_tmp9A5)))));_npop_handler(0);return _tmp1B2;};}_LLAC:;_LLAD:
{const char*_tmp9A9;Cyc_Parse_err(((_tmp9A9="missing declarator",_tag_dyneither(_tmp9A9,sizeof(char),19))),loc);}{struct Cyc_List_List*_tmp1B6=0;_npop_handler(0);return _tmp1B6;};_LL9B:;}else{
# 955
struct Cyc_List_List*_tmp1B7=Cyc_Parse_apply_tmss(mkrgn,_tmp166,_tmp16B,declarators,_tmp168);
if(istypedef){
# 960
if(!exps_empty){
const char*_tmp9AA;Cyc_Parse_err(((_tmp9AA="initializer in typedef declaration",_tag_dyneither(_tmp9AA,sizeof(char),35))),loc);}{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(unsigned int,struct _tuple15*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp1B7);
struct Cyc_List_List*_tmp1B9=decls;_npop_handler(0);return _tmp1B9;};}else{
# 966
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp1BA=_tmp1B7;for(0;_tmp1BA != 0;(_tmp1BA=_tmp1BA->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple15*_tmp1BB=(struct _tuple15*)_tmp1BA->hd;struct _tuple0*_tmp1BD;struct Cyc_Absyn_Tqual _tmp1BE;void*_tmp1BF;struct Cyc_List_List*_tmp1C0;struct Cyc_List_List*_tmp1C1;struct _tuple15*_tmp1BC=_tmp1BB;_tmp1BD=_tmp1BC->f1;_tmp1BE=_tmp1BC->f2;_tmp1BF=_tmp1BC->f3;_tmp1C0=_tmp1BC->f4;_tmp1C1=_tmp1BC->f5;
if(_tmp1C0 != 0){
const char*_tmp9AD;void*_tmp9AC;(_tmp9AC=0,Cyc_Tcutil_warn(loc,((_tmp9AD="bad type params, ignoring",_tag_dyneither(_tmp9AD,sizeof(char),26))),_tag_dyneither(_tmp9AC,sizeof(void*),0)));}
if(exprs == 0){
const char*_tmp9B0;void*_tmp9AF;(_tmp9AF=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp9B0="unexpected NULL in parse!",_tag_dyneither(_tmp9B0,sizeof(char),26))),_tag_dyneither(_tmp9AF,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp1C6=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd;
struct Cyc_Absyn_Vardecl*_tmp1C7=Cyc_Absyn_new_vardecl(_tmp1BD,_tmp1BF,_tmp1C6);
_tmp1C7->tq=_tmp1BE;
_tmp1C7->sc=s;
_tmp1C7->attributes=_tmp1C1;{
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp9B6;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp9B5;struct Cyc_Absyn_Decl*_tmp9B4;struct Cyc_Absyn_Decl*_tmp1C8=(_tmp9B4=_cycalloc(sizeof(*_tmp9B4)),((_tmp9B4->r=(void*)((_tmp9B6=_cycalloc(sizeof(*_tmp9B6)),((_tmp9B6[0]=((_tmp9B5.tag=0,((_tmp9B5.f1=_tmp1C7,_tmp9B5)))),_tmp9B6)))),((_tmp9B4->loc=loc,_tmp9B4)))));
struct Cyc_List_List*_tmp9B7;decls=((_tmp9B7=_cycalloc(sizeof(*_tmp9B7)),((_tmp9B7->hd=_tmp1C8,((_tmp9B7->tl=decls,_tmp9B7))))));};};}}{
# 981
struct Cyc_List_List*_tmp1CD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);_npop_handler(0);return _tmp1CD;};}}};};};};}
# 878
;_pop_region(mkrgn);}
# 988
static struct Cyc_Absyn_Kind*Cyc_Parse_id_to_kind(struct _dyneither_ptr s,unsigned int loc){
if(Cyc_strlen((struct _dyneither_ptr)s)== 1  || Cyc_strlen((struct _dyneither_ptr)s)== 2){
char _tmp1CE=*((const char*)_check_dyneither_subscript(s,sizeof(char),0));switch(_tmp1CE){case 'A': _LLAE:
 return& Cyc_Tcutil_ak;case 'M': _LLAF:
 return& Cyc_Tcutil_mk;case 'B': _LLB0:
 return& Cyc_Tcutil_bk;case 'R': _LLB1:
 return& Cyc_Tcutil_rk;case 'E': _LLB2:
 return& Cyc_Tcutil_ek;case 'I': _LLB3:
 return& Cyc_Tcutil_ik;case 'U': _LLB4:
# 998
{char _tmp1CF=*((const char*)_check_dyneither_subscript(s,sizeof(char),1));switch(_tmp1CF){case 'R': _LLB6:
 return& Cyc_Tcutil_urk;case 'A': _LLB7:
 return& Cyc_Tcutil_uak;case 'M': _LLB8:
 return& Cyc_Tcutil_umk;case 'B': _LLB9:
 return& Cyc_Tcutil_ubk;default: _LLBA:
 break;}}
# 1005
break;case 'T': _LLB5:
# 1007
{char _tmp1D0=*((const char*)_check_dyneither_subscript(s,sizeof(char),1));switch(_tmp1D0){case 'R': _LLBD:
 return& Cyc_Tcutil_trk;case 'A': _LLBE:
 return& Cyc_Tcutil_tak;case 'M': _LLBF:
 return& Cyc_Tcutil_tmk;case 'B': _LLC0:
 return& Cyc_Tcutil_tbk;default: _LLC1:
 break;}}
# 1014
break;default: _LLBC:
 break;}}
# 1017
{const char*_tmp9BC;void*_tmp9BB[2];struct Cyc_String_pa_PrintArg_struct _tmp9BA;struct Cyc_Int_pa_PrintArg_struct _tmp9B9;Cyc_Parse_err((struct _dyneither_ptr)((_tmp9B9.tag=1,((_tmp9B9.f1=(unsigned long)((int)Cyc_strlen((struct _dyneither_ptr)s)),((_tmp9BA.tag=0,((_tmp9BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp9BB[0]=& _tmp9BA,((_tmp9BB[1]=& _tmp9B9,Cyc_aprintf(((_tmp9BC="bad kind: %s; strlen=%d",_tag_dyneither(_tmp9BC,sizeof(char),24))),_tag_dyneither(_tmp9BB,sizeof(void*),2)))))))))))))),loc);}
return& Cyc_Tcutil_bk;}
# 1022
static int Cyc_Parse_exp2int(unsigned int loc,struct Cyc_Absyn_Exp*e){
void*_tmp1D5=e->r;void*_tmp1D6=_tmp1D5;int _tmp1D8;_LLC5: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1D7=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1D6;if(_tmp1D7->tag != 0)goto _LLC7;else{if(((_tmp1D7->f1).Int_c).tag != 5)goto _LLC7;_tmp1D8=((struct _tuple5)((_tmp1D7->f1).Int_c).val).f2;}}_LLC6:
 return _tmp1D8;_LLC7:;_LLC8:
# 1026
{const char*_tmp9BD;Cyc_Parse_err(((_tmp9BD="expecting integer constant",_tag_dyneither(_tmp9BD,sizeof(char),27))),loc);}
return 0;_LLC4:;}
# 1032
static struct _dyneither_ptr Cyc_Parse_exp2string(unsigned int loc,struct Cyc_Absyn_Exp*e){
void*_tmp1DA=e->r;void*_tmp1DB=_tmp1DA;struct _dyneither_ptr _tmp1DD;_LLCA: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1DC=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1DB;if(_tmp1DC->tag != 0)goto _LLCC;else{if(((_tmp1DC->f1).String_c).tag != 8)goto _LLCC;_tmp1DD=(struct _dyneither_ptr)((_tmp1DC->f1).String_c).val;}}_LLCB:
 return _tmp1DD;_LLCC:;_LLCD:
# 1036
{const char*_tmp9BE;Cyc_Parse_err(((_tmp9BE="expecting string constant",_tag_dyneither(_tmp9BE,sizeof(char),26))),loc);}
return _tag_dyneither(0,0,0);_LLC9:;}
# 1042
static unsigned int Cyc_Parse_cnst2uint(unsigned int loc,union Cyc_Absyn_Cnst x){
union Cyc_Absyn_Cnst _tmp1DF=x;int _tmp1E0;char _tmp1E1;long long _tmp1E2;_LLCF: if((_tmp1DF.Int_c).tag != 5)goto _LLD1;_tmp1E0=((struct _tuple5)(_tmp1DF.Int_c).val).f2;_LLD0:
 return(unsigned int)_tmp1E0;_LLD1: if((_tmp1DF.Char_c).tag != 2)goto _LLD3;_tmp1E1=((struct _tuple3)(_tmp1DF.Char_c).val).f2;_LLD2:
 return(unsigned int)_tmp1E1;_LLD3: if((_tmp1DF.LongLong_c).tag != 6)goto _LLD5;_tmp1E2=((struct _tuple6)(_tmp1DF.LongLong_c).val).f2;_LLD4: {
# 1047
unsigned long long y=(unsigned long long)_tmp1E2;
if(y > -1){
const char*_tmp9BF;Cyc_Parse_err(((_tmp9BF="integer constant too large",_tag_dyneither(_tmp9BF,sizeof(char),27))),loc);}
return(unsigned int)_tmp1E2;}_LLD5:;_LLD6:
# 1052
{const char*_tmp9C3;void*_tmp9C2[1];struct Cyc_String_pa_PrintArg_struct _tmp9C1;Cyc_Parse_err((struct _dyneither_ptr)((_tmp9C1.tag=0,((_tmp9C1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_cnst2string(x)),((_tmp9C2[0]=& _tmp9C1,Cyc_aprintf(((_tmp9C3="expected integer constant but found %s",_tag_dyneither(_tmp9C3,sizeof(char),39))),_tag_dyneither(_tmp9C2,sizeof(void*),1)))))))),loc);}
return 0;_LLCE:;}
# 1058
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_tmp1E7=p->r;void*_tmp1E8=_tmp1E7;struct _tuple0*_tmp1EA;struct Cyc_Absyn_Vardecl*_tmp1EC;struct Cyc_Absyn_Pat*_tmp1EF;enum Cyc_Absyn_Sign _tmp1F2;int _tmp1F3;char _tmp1F5;struct _dyneither_ptr _tmp1F7;int _tmp1F8;struct _tuple0*_tmp1FA;struct Cyc_List_List*_tmp1FB;struct Cyc_Absyn_Exp*_tmp1FD;_LLD8: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp1E9=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1E8;if(_tmp1E9->tag != 15)goto _LLDA;else{_tmp1EA=_tmp1E9->f1;}}_LLD9:
 return Cyc_Absyn_unknownid_exp(_tmp1EA,p->loc);_LLDA: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp1EB=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1E8;if(_tmp1EB->tag != 3)goto _LLDC;else{_tmp1EC=_tmp1EB->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp1ED=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp1EB->f2)->r;if(_tmp1ED->tag != 0)goto _LLDC;};}}_LLDB:
# 1062
 return Cyc_Absyn_deref_exp(Cyc_Absyn_unknownid_exp(_tmp1EC->name,p->loc),p->loc);_LLDC: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp1EE=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1E8;if(_tmp1EE->tag != 6)goto _LLDE;else{_tmp1EF=_tmp1EE->f1;}}_LLDD:
 return Cyc_Absyn_address_exp(Cyc_Parse_pat2exp(_tmp1EF),p->loc);_LLDE: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp1F0=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp1E8;if(_tmp1F0->tag != 9)goto _LLE0;}_LLDF:
 return Cyc_Absyn_null_exp(p->loc);_LLE0: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp1F1=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1E8;if(_tmp1F1->tag != 10)goto _LLE2;else{_tmp1F2=_tmp1F1->f1;_tmp1F3=_tmp1F1->f2;}}_LLE1:
 return Cyc_Absyn_int_exp(_tmp1F2,_tmp1F3,p->loc);_LLE2: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp1F4=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1E8;if(_tmp1F4->tag != 11)goto _LLE4;else{_tmp1F5=_tmp1F4->f1;}}_LLE3:
 return Cyc_Absyn_char_exp(_tmp1F5,p->loc);_LLE4: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp1F6=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1E8;if(_tmp1F6->tag != 12)goto _LLE6;else{_tmp1F7=_tmp1F6->f1;_tmp1F8=_tmp1F6->f2;}}_LLE5:
 return Cyc_Absyn_float_exp(_tmp1F7,_tmp1F8,p->loc);_LLE6: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp1F9=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1E8;if(_tmp1F9->tag != 16)goto _LLE8;else{_tmp1FA=_tmp1F9->f1;_tmp1FB=_tmp1F9->f2;if(_tmp1F9->f3 != 0)goto _LLE8;}}_LLE7: {
# 1069
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(_tmp1FA,p->loc);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp1FB);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}_LLE8: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp1FC=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1E8;if(_tmp1FC->tag != 17)goto _LLEA;else{_tmp1FD=_tmp1FC->f1;}}_LLE9:
 return _tmp1FD;_LLEA:;_LLEB:
# 1074
{const char*_tmp9C4;Cyc_Parse_err(((_tmp9C4="cannot mix patterns and expressions in case",_tag_dyneither(_tmp9C4,sizeof(char),44))),p->loc);}
return Cyc_Absyn_null_exp(p->loc);_LLD7:;}struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple20{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple20 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple21{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple21*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple22{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Parse_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Parse_Declarator val;};struct _tuple24{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple25{struct Cyc_Absyn_Tqual f1;struct Cyc_Parse_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple25 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple26{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple26*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};static char _tmp200[7]="cnst_t";
# 1130 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp200,_tmp200,_tmp200 + 7}};
# 1132
union Cyc_Absyn_Cnst Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Cnst yyzzz;
{union Cyc_YYSTYPE _tmp201=yy1;union Cyc_Absyn_Cnst _tmp202;_LLED: if((_tmp201.Int_tok).tag != 1)goto _LLEF;_tmp202=(union Cyc_Absyn_Cnst)(_tmp201.Int_tok).val;_LLEE:
# 1136
 yyzzz=_tmp202;
goto _LLEC;_LLEF:;_LLF0:
(int)_throw((void*)& Cyc_yyfail_Int_tok);_LLEC:;}
# 1140
return yyzzz;}
# 1142
union Cyc_YYSTYPE Cyc_Int_tok(union Cyc_Absyn_Cnst yy1){union Cyc_YYSTYPE _tmp9C5;return((_tmp9C5.Int_tok).val=yy1,(((_tmp9C5.Int_tok).tag=1,_tmp9C5)));}static char _tmp205[5]="char";
# 1131 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Char_tok={Cyc_Core_Failure,{_tmp205,_tmp205,_tmp205 + 5}};
# 1133
char Cyc_yyget_Char_tok(union Cyc_YYSTYPE yy1){
char yyzzz;
{union Cyc_YYSTYPE _tmp206=yy1;char _tmp207;_LLF2: if((_tmp206.Char_tok).tag != 2)goto _LLF4;_tmp207=(char)(_tmp206.Char_tok).val;_LLF3:
# 1137
 yyzzz=_tmp207;
goto _LLF1;_LLF4:;_LLF5:
(int)_throw((void*)& Cyc_yyfail_Char_tok);_LLF1:;}
# 1141
return yyzzz;}
# 1143
union Cyc_YYSTYPE Cyc_Char_tok(char yy1){union Cyc_YYSTYPE _tmp9C6;return((_tmp9C6.Char_tok).val=yy1,(((_tmp9C6.Char_tok).tag=2,_tmp9C6)));}static char _tmp20A[13]="string_t<`H>";
# 1132 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_String_tok={Cyc_Core_Failure,{_tmp20A,_tmp20A,_tmp20A + 13}};
# 1134
struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE yy1){
struct _dyneither_ptr yyzzz;
{union Cyc_YYSTYPE _tmp20B=yy1;struct _dyneither_ptr _tmp20C;_LLF7: if((_tmp20B.String_tok).tag != 3)goto _LLF9;_tmp20C=(struct _dyneither_ptr)(_tmp20B.String_tok).val;_LLF8:
# 1138
 yyzzz=_tmp20C;
goto _LLF6;_LLF9:;_LLFA:
(int)_throw((void*)& Cyc_yyfail_String_tok);_LLF6:;}
# 1142
return yyzzz;}
# 1144
union Cyc_YYSTYPE Cyc_String_tok(struct _dyneither_ptr yy1){union Cyc_YYSTYPE _tmp9C7;return((_tmp9C7.String_tok).val=yy1,(((_tmp9C7.String_tok).tag=3,_tmp9C7)));}static char _tmp20F[56]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
# 1135 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp20F,_tmp20F,_tmp20F + 56}};
# 1137
struct _tuple21*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1){
struct _tuple21*yyzzz;
{union Cyc_YYSTYPE _tmp210=yy1;struct _tuple21*_tmp211;_LLFC: if((_tmp210.YY1).tag != 9)goto _LLFE;_tmp211=(struct _tuple21*)(_tmp210.YY1).val;_LLFD:
# 1141
 yyzzz=_tmp211;
goto _LLFB;_LLFE:;_LLFF:
(int)_throw((void*)& Cyc_yyfail_YY1);_LLFB:;}
# 1145
return yyzzz;}
# 1147
union Cyc_YYSTYPE Cyc_YY1(struct _tuple21*yy1){union Cyc_YYSTYPE _tmp9C8;return((_tmp9C8.YY1).val=yy1,(((_tmp9C8.YY1).tag=9,_tmp9C8)));}static char _tmp214[19]="conref_t<bounds_t>";
# 1136 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{_tmp214,_tmp214,_tmp214 + 19}};
# 1138
union Cyc_Absyn_Constraint*Cyc_yyget_YY2(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Constraint*yyzzz;
{union Cyc_YYSTYPE _tmp215=yy1;union Cyc_Absyn_Constraint*_tmp216;_LL101: if((_tmp215.YY2).tag != 10)goto _LL103;_tmp216=(union Cyc_Absyn_Constraint*)(_tmp215.YY2).val;_LL102:
# 1142
 yyzzz=_tmp216;
goto _LL100;_LL103:;_LL104:
(int)_throw((void*)& Cyc_yyfail_YY2);_LL100:;}
# 1146
return yyzzz;}
# 1148
union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp9C9;return((_tmp9C9.YY2).val=yy1,(((_tmp9C9.YY2).tag=10,_tmp9C9)));}static char _tmp219[28]="list_t<offsetof_field_t,`H>";
# 1137 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY3={Cyc_Core_Failure,{_tmp219,_tmp219,_tmp219 + 28}};
# 1139
struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp21A=yy1;struct Cyc_List_List*_tmp21B;_LL106: if((_tmp21A.YY3).tag != 11)goto _LL108;_tmp21B=(struct Cyc_List_List*)(_tmp21A.YY3).val;_LL107:
# 1143
 yyzzz=_tmp21B;
goto _LL105;_LL108:;_LL109:
(int)_throw((void*)& Cyc_yyfail_YY3);_LL105:;}
# 1147
return yyzzz;}
# 1149
union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9CA;return((_tmp9CA.YY3).val=yy1,(((_tmp9CA.YY3).tag=11,_tmp9CA)));}static char _tmp21E[6]="exp_t";
# 1138 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Exp_tok={Cyc_Core_Failure,{_tmp21E,_tmp21E,_tmp21E + 6}};
# 1140
struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp21F=yy1;struct Cyc_Absyn_Exp*_tmp220;_LL10B: if((_tmp21F.Exp_tok).tag != 7)goto _LL10D;_tmp220=(struct Cyc_Absyn_Exp*)(_tmp21F.Exp_tok).val;_LL10C:
# 1144
 yyzzz=_tmp220;
goto _LL10A;_LL10D:;_LL10E:
(int)_throw((void*)& Cyc_yyfail_Exp_tok);_LL10A:;}
# 1148
return yyzzz;}
# 1150
union Cyc_YYSTYPE Cyc_Exp_tok(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp9CB;return((_tmp9CB.Exp_tok).val=yy1,(((_tmp9CB.Exp_tok).tag=7,_tmp9CB)));}static char _tmp223[17]="list_t<exp_t,`H>";
# 1146 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp223,_tmp223,_tmp223 + 17}};
# 1148
struct Cyc_List_List*Cyc_yyget_YY4(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp224=yy1;struct Cyc_List_List*_tmp225;_LL110: if((_tmp224.YY4).tag != 12)goto _LL112;_tmp225=(struct Cyc_List_List*)(_tmp224.YY4).val;_LL111:
# 1152
 yyzzz=_tmp225;
goto _LL10F;_LL112:;_LL113:
(int)_throw((void*)& Cyc_yyfail_YY4);_LL10F:;}
# 1156
return yyzzz;}
# 1158
union Cyc_YYSTYPE Cyc_YY4(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9CC;return((_tmp9CC.YY4).val=yy1,(((_tmp9CC.YY4).tag=12,_tmp9CC)));}static char _tmp228[47]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1147 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY5={Cyc_Core_Failure,{_tmp228,_tmp228,_tmp228 + 47}};
# 1149
struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp229=yy1;struct Cyc_List_List*_tmp22A;_LL115: if((_tmp229.YY5).tag != 13)goto _LL117;_tmp22A=(struct Cyc_List_List*)(_tmp229.YY5).val;_LL116:
# 1153
 yyzzz=_tmp22A;
goto _LL114;_LL117:;_LL118:
(int)_throw((void*)& Cyc_yyfail_YY5);_LL114:;}
# 1157
return yyzzz;}
# 1159
union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9CD;return((_tmp9CD.YY5).val=yy1,(((_tmp9CD.YY5).tag=13,_tmp9CD)));}static char _tmp22D[9]="primop_t";
# 1148 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp22D,_tmp22D,_tmp22D + 9}};
# 1150
enum Cyc_Absyn_Primop Cyc_yyget_YY6(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Primop yyzzz;
{union Cyc_YYSTYPE _tmp22E=yy1;enum Cyc_Absyn_Primop _tmp22F;_LL11A: if((_tmp22E.YY6).tag != 14)goto _LL11C;_tmp22F=(enum Cyc_Absyn_Primop)(_tmp22E.YY6).val;_LL11B:
# 1154
 yyzzz=_tmp22F;
goto _LL119;_LL11C:;_LL11D:
(int)_throw((void*)& Cyc_yyfail_YY6);_LL119:;}
# 1158
return yyzzz;}
# 1160
union Cyc_YYSTYPE Cyc_YY6(enum Cyc_Absyn_Primop yy1){union Cyc_YYSTYPE _tmp9CE;return((_tmp9CE.YY6).val=yy1,(((_tmp9CE.YY6).tag=14,_tmp9CE)));}static char _tmp232[19]="opt_t<primop_t,`H>";
# 1149 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp232,_tmp232,_tmp232 + 19}};
# 1151
struct Cyc_Core_Opt*Cyc_yyget_YY7(union Cyc_YYSTYPE yy1){
struct Cyc_Core_Opt*yyzzz;
{union Cyc_YYSTYPE _tmp233=yy1;struct Cyc_Core_Opt*_tmp234;_LL11F: if((_tmp233.YY7).tag != 15)goto _LL121;_tmp234=(struct Cyc_Core_Opt*)(_tmp233.YY7).val;_LL120:
# 1155
 yyzzz=_tmp234;
goto _LL11E;_LL121:;_LL122:
(int)_throw((void*)& Cyc_yyfail_YY7);_LL11E:;}
# 1159
return yyzzz;}
# 1161
union Cyc_YYSTYPE Cyc_YY7(struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _tmp9CF;return((_tmp9CF.YY7).val=yy1,(((_tmp9CF.YY7).tag=15,_tmp9CF)));}static char _tmp237[7]="qvar_t";
# 1150 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp237,_tmp237,_tmp237 + 7}};
# 1152
struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE yy1){
struct _tuple0*yyzzz;
{union Cyc_YYSTYPE _tmp238=yy1;struct _tuple0*_tmp239;_LL124: if((_tmp238.QualId_tok).tag != 5)goto _LL126;_tmp239=(struct _tuple0*)(_tmp238.QualId_tok).val;_LL125:
# 1156
 yyzzz=_tmp239;
goto _LL123;_LL126:;_LL127:
(int)_throw((void*)& Cyc_yyfail_QualId_tok);_LL123:;}
# 1160
return yyzzz;}
# 1162
union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){union Cyc_YYSTYPE _tmp9D0;return((_tmp9D0.QualId_tok).val=yy1,(((_tmp9D0.QualId_tok).tag=5,_tmp9D0)));}static char _tmp23C[7]="stmt_t";
# 1153 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Stmt_tok={Cyc_Core_Failure,{_tmp23C,_tmp23C,_tmp23C + 7}};
# 1155
struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Stmt*yyzzz;
{union Cyc_YYSTYPE _tmp23D=yy1;struct Cyc_Absyn_Stmt*_tmp23E;_LL129: if((_tmp23D.Stmt_tok).tag != 8)goto _LL12B;_tmp23E=(struct Cyc_Absyn_Stmt*)(_tmp23D.Stmt_tok).val;_LL12A:
# 1159
 yyzzz=_tmp23E;
goto _LL128;_LL12B:;_LL12C:
(int)_throw((void*)& Cyc_yyfail_Stmt_tok);_LL128:;}
# 1163
return yyzzz;}
# 1165
union Cyc_YYSTYPE Cyc_Stmt_tok(struct Cyc_Absyn_Stmt*yy1){union Cyc_YYSTYPE _tmp9D1;return((_tmp9D1.Stmt_tok).val=yy1,(((_tmp9D1.Stmt_tok).tag=8,_tmp9D1)));}static char _tmp241[27]="list_t<switch_clause_t,`H>";
# 1157 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY8={Cyc_Core_Failure,{_tmp241,_tmp241,_tmp241 + 27}};
# 1159
struct Cyc_List_List*Cyc_yyget_YY8(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp242=yy1;struct Cyc_List_List*_tmp243;_LL12E: if((_tmp242.YY8).tag != 16)goto _LL130;_tmp243=(struct Cyc_List_List*)(_tmp242.YY8).val;_LL12F:
# 1163
 yyzzz=_tmp243;
goto _LL12D;_LL130:;_LL131:
(int)_throw((void*)& Cyc_yyfail_YY8);_LL12D:;}
# 1167
return yyzzz;}
# 1169
union Cyc_YYSTYPE Cyc_YY8(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D2;return((_tmp9D2.YY8).val=yy1,(((_tmp9D2.YY8).tag=16,_tmp9D2)));}static char _tmp246[6]="pat_t";
# 1158 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp246,_tmp246,_tmp246 + 6}};
# 1160
struct Cyc_Absyn_Pat*Cyc_yyget_YY9(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Pat*yyzzz;
{union Cyc_YYSTYPE _tmp247=yy1;struct Cyc_Absyn_Pat*_tmp248;_LL133: if((_tmp247.YY9).tag != 17)goto _LL135;_tmp248=(struct Cyc_Absyn_Pat*)(_tmp247.YY9).val;_LL134:
# 1164
 yyzzz=_tmp248;
goto _LL132;_LL135:;_LL136:
(int)_throw((void*)& Cyc_yyfail_YY9);_LL132:;}
# 1168
return yyzzz;}
# 1170
union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Pat*yy1){union Cyc_YYSTYPE _tmp9D3;return((_tmp9D3.YY9).val=yy1,(((_tmp9D3.YY9).tag=17,_tmp9D3)));}static char _tmp24B[28]="$(list_t<pat_t,`H>,bool)@`H";
# 1163 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp24B,_tmp24B,_tmp24B + 28}};
# 1165
struct _tuple22*Cyc_yyget_YY10(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp24C=yy1;struct _tuple22*_tmp24D;_LL138: if((_tmp24C.YY10).tag != 18)goto _LL13A;_tmp24D=(struct _tuple22*)(_tmp24C.YY10).val;_LL139:
# 1169
 yyzzz=_tmp24D;
goto _LL137;_LL13A:;_LL13B:
(int)_throw((void*)& Cyc_yyfail_YY10);_LL137:;}
# 1173
return yyzzz;}
# 1175
union Cyc_YYSTYPE Cyc_YY10(struct _tuple22*yy1){union Cyc_YYSTYPE _tmp9D4;return((_tmp9D4.YY10).val=yy1,(((_tmp9D4.YY10).tag=18,_tmp9D4)));}static char _tmp250[17]="list_t<pat_t,`H>";
# 1164 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp250,_tmp250,_tmp250 + 17}};
# 1166
struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp251=yy1;struct Cyc_List_List*_tmp252;_LL13D: if((_tmp251.YY11).tag != 19)goto _LL13F;_tmp252=(struct Cyc_List_List*)(_tmp251.YY11).val;_LL13E:
# 1170
 yyzzz=_tmp252;
goto _LL13C;_LL13F:;_LL140:
(int)_throw((void*)& Cyc_yyfail_YY11);_LL13C:;}
# 1174
return yyzzz;}
# 1176
union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D5;return((_tmp9D5.YY11).val=yy1,(((_tmp9D5.YY11).tag=19,_tmp9D5)));}static char _tmp255[36]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1165 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp255,_tmp255,_tmp255 + 36}};
# 1167
struct _tuple23*Cyc_yyget_YY12(union Cyc_YYSTYPE yy1){
struct _tuple23*yyzzz;
{union Cyc_YYSTYPE _tmp256=yy1;struct _tuple23*_tmp257;_LL142: if((_tmp256.YY12).tag != 20)goto _LL144;_tmp257=(struct _tuple23*)(_tmp256.YY12).val;_LL143:
# 1171
 yyzzz=_tmp257;
goto _LL141;_LL144:;_LL145:
(int)_throw((void*)& Cyc_yyfail_YY12);_LL141:;}
# 1175
return yyzzz;}
# 1177
union Cyc_YYSTYPE Cyc_YY12(struct _tuple23*yy1){union Cyc_YYSTYPE _tmp9D6;return((_tmp9D6.YY12).val=yy1,(((_tmp9D6.YY12).tag=20,_tmp9D6)));}static char _tmp25A[47]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1166 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp25A,_tmp25A,_tmp25A + 47}};
# 1168
struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp25B=yy1;struct Cyc_List_List*_tmp25C;_LL147: if((_tmp25B.YY13).tag != 21)goto _LL149;_tmp25C=(struct Cyc_List_List*)(_tmp25B.YY13).val;_LL148:
# 1172
 yyzzz=_tmp25C;
goto _LL146;_LL149:;_LL14A:
(int)_throw((void*)& Cyc_yyfail_YY13);_LL146:;}
# 1176
return yyzzz;}
# 1178
union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D7;return((_tmp9D7.YY13).val=yy1,(((_tmp9D7.YY13).tag=21,_tmp9D7)));}static char _tmp25F[58]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1167 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp25F,_tmp25F,_tmp25F + 58}};
# 1169
struct _tuple22*Cyc_yyget_YY14(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp260=yy1;struct _tuple22*_tmp261;_LL14C: if((_tmp260.YY14).tag != 22)goto _LL14E;_tmp261=(struct _tuple22*)(_tmp260.YY14).val;_LL14D:
# 1173
 yyzzz=_tmp261;
goto _LL14B;_LL14E:;_LL14F:
(int)_throw((void*)& Cyc_yyfail_YY14);_LL14B:;}
# 1177
return yyzzz;}
# 1179
union Cyc_YYSTYPE Cyc_YY14(struct _tuple22*yy1){union Cyc_YYSTYPE _tmp9D8;return((_tmp9D8.YY14).val=yy1,(((_tmp9D8.YY14).tag=22,_tmp9D8)));}static char _tmp264[9]="fndecl_t";
# 1168 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp264,_tmp264,_tmp264 + 9}};
# 1170
struct Cyc_Absyn_Fndecl*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Fndecl*yyzzz;
{union Cyc_YYSTYPE _tmp265=yy1;struct Cyc_Absyn_Fndecl*_tmp266;_LL151: if((_tmp265.YY15).tag != 23)goto _LL153;_tmp266=(struct Cyc_Absyn_Fndecl*)(_tmp265.YY15).val;_LL152:
# 1174
 yyzzz=_tmp266;
goto _LL150;_LL153:;_LL154:
(int)_throw((void*)& Cyc_yyfail_YY15);_LL150:;}
# 1178
return yyzzz;}
# 1180
union Cyc_YYSTYPE Cyc_YY15(struct Cyc_Absyn_Fndecl*yy1){union Cyc_YYSTYPE _tmp9D9;return((_tmp9D9.YY15).val=yy1,(((_tmp9D9.YY15).tag=23,_tmp9D9)));}static char _tmp269[18]="list_t<decl_t,`H>";
# 1169 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp269,_tmp269,_tmp269 + 18}};
# 1171
struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp26A=yy1;struct Cyc_List_List*_tmp26B;_LL156: if((_tmp26A.YY16).tag != 24)goto _LL158;_tmp26B=(struct Cyc_List_List*)(_tmp26A.YY16).val;_LL157:
# 1175
 yyzzz=_tmp26B;
goto _LL155;_LL158:;_LL159:
(int)_throw((void*)& Cyc_yyfail_YY16);_LL155:;}
# 1179
return yyzzz;}
# 1181
union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9DA;return((_tmp9DA.YY16).val=yy1,(((_tmp9DA.YY16).tag=24,_tmp9DA)));}static char _tmp26E[12]="decl_spec_t";
# 1171 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY17={Cyc_Core_Failure,{_tmp26E,_tmp26E,_tmp26E + 12}};
# 1173
struct Cyc_Parse_Declaration_spec Cyc_yyget_YY17(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declaration_spec yyzzz;
{union Cyc_YYSTYPE _tmp26F=yy1;struct Cyc_Parse_Declaration_spec _tmp270;_LL15B: if((_tmp26F.YY17).tag != 25)goto _LL15D;_tmp270=(struct Cyc_Parse_Declaration_spec)(_tmp26F.YY17).val;_LL15C:
# 1177
 yyzzz=_tmp270;
goto _LL15A;_LL15D:;_LL15E:
(int)_throw((void*)& Cyc_yyfail_YY17);_LL15A:;}
# 1181
return yyzzz;}
# 1183
union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Parse_Declaration_spec yy1){union Cyc_YYSTYPE _tmp9DB;return((_tmp9DB.YY17).val=yy1,(((_tmp9DB.YY17).tag=25,_tmp9DB)));}static char _tmp273[31]="$(declarator_t<`yy>,exp_opt_t)";
# 1172 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY18={Cyc_Core_Failure,{_tmp273,_tmp273,_tmp273 + 31}};
# 1174
struct _tuple11 Cyc_yyget_YY18(union Cyc_YYSTYPE yy1){
struct _tuple11 yyzzz;
{union Cyc_YYSTYPE _tmp274=yy1;struct _tuple11 _tmp275;_LL160: if((_tmp274.YY18).tag != 26)goto _LL162;_tmp275=(struct _tuple11)(_tmp274.YY18).val;_LL161:
# 1178
 yyzzz=_tmp275;
goto _LL15F;_LL162:;_LL163:
(int)_throw((void*)& Cyc_yyfail_YY18);_LL15F:;}
# 1182
return yyzzz;}
# 1184
union Cyc_YYSTYPE Cyc_YY18(struct _tuple11 yy1){union Cyc_YYSTYPE _tmp9DC;return((_tmp9DC.YY18).val=yy1,(((_tmp9DC.YY18).tag=26,_tmp9DC)));}static char _tmp278[23]="declarator_list_t<`yy>";
# 1173 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp278,_tmp278,_tmp278 + 23}};
# 1175
struct _tuple12*Cyc_yyget_YY19(union Cyc_YYSTYPE yy1){
struct _tuple12*yyzzz;
{union Cyc_YYSTYPE _tmp279=yy1;struct _tuple12*_tmp27A;_LL165: if((_tmp279.YY19).tag != 27)goto _LL167;_tmp27A=(struct _tuple12*)(_tmp279.YY19).val;_LL166:
# 1179
 yyzzz=_tmp27A;
goto _LL164;_LL167:;_LL168:
(int)_throw((void*)& Cyc_yyfail_YY19);_LL164:;}
# 1183
return yyzzz;}
# 1185
union Cyc_YYSTYPE Cyc_YY19(struct _tuple12*yy1){union Cyc_YYSTYPE _tmp9DD;return((_tmp9DD.YY19).val=yy1,(((_tmp9DD.YY19).tag=27,_tmp9DD)));}static char _tmp27D[19]="storage_class_t@`H";
# 1174 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp27D,_tmp27D,_tmp27D + 19}};
# 1176
enum Cyc_Parse_Storage_class*Cyc_yyget_YY20(union Cyc_YYSTYPE yy1){
enum Cyc_Parse_Storage_class*yyzzz;
{union Cyc_YYSTYPE _tmp27E=yy1;enum Cyc_Parse_Storage_class*_tmp27F;_LL16A: if((_tmp27E.YY20).tag != 28)goto _LL16C;_tmp27F=(enum Cyc_Parse_Storage_class*)(_tmp27E.YY20).val;_LL16B:
# 1180
 yyzzz=_tmp27F;
goto _LL169;_LL16C:;_LL16D:
(int)_throw((void*)& Cyc_yyfail_YY20);_LL169:;}
# 1184
return yyzzz;}
# 1186
union Cyc_YYSTYPE Cyc_YY20(enum Cyc_Parse_Storage_class*yy1){union Cyc_YYSTYPE _tmp9DE;return((_tmp9DE.YY20).val=yy1,(((_tmp9DE.YY20).tag=28,_tmp9DE)));}static char _tmp282[17]="type_specifier_t";
# 1175 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp282,_tmp282,_tmp282 + 17}};
# 1177
struct Cyc_Parse_Type_specifier Cyc_yyget_YY21(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Type_specifier yyzzz;
{union Cyc_YYSTYPE _tmp283=yy1;struct Cyc_Parse_Type_specifier _tmp284;_LL16F: if((_tmp283.YY21).tag != 29)goto _LL171;_tmp284=(struct Cyc_Parse_Type_specifier)(_tmp283.YY21).val;_LL170:
# 1181
 yyzzz=_tmp284;
goto _LL16E;_LL171:;_LL172:
(int)_throw((void*)& Cyc_yyfail_YY21);_LL16E:;}
# 1185
return yyzzz;}
# 1187
union Cyc_YYSTYPE Cyc_YY21(struct Cyc_Parse_Type_specifier yy1){union Cyc_YYSTYPE _tmp9DF;return((_tmp9DF.YY21).val=yy1,(((_tmp9DF.YY21).tag=29,_tmp9DF)));}static char _tmp287[12]="aggr_kind_t";
# 1177 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp287,_tmp287,_tmp287 + 12}};
# 1179
enum Cyc_Absyn_AggrKind Cyc_yyget_YY22(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_AggrKind yyzzz;
{union Cyc_YYSTYPE _tmp288=yy1;enum Cyc_Absyn_AggrKind _tmp289;_LL174: if((_tmp288.YY22).tag != 30)goto _LL176;_tmp289=(enum Cyc_Absyn_AggrKind)(_tmp288.YY22).val;_LL175:
# 1183
 yyzzz=_tmp289;
goto _LL173;_LL176:;_LL177:
(int)_throw((void*)& Cyc_yyfail_YY22);_LL173:;}
# 1187
return yyzzz;}
# 1189
union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Absyn_AggrKind yy1){union Cyc_YYSTYPE _tmp9E0;return((_tmp9E0.YY22).val=yy1,(((_tmp9E0.YY22).tag=30,_tmp9E0)));}static char _tmp28C[8]="tqual_t";
# 1178 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp28C,_tmp28C,_tmp28C + 8}};
# 1180
struct Cyc_Absyn_Tqual Cyc_yyget_YY23(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Tqual yyzzz;
{union Cyc_YYSTYPE _tmp28D=yy1;struct Cyc_Absyn_Tqual _tmp28E;_LL179: if((_tmp28D.YY23).tag != 31)goto _LL17B;_tmp28E=(struct Cyc_Absyn_Tqual)(_tmp28D.YY23).val;_LL17A:
# 1184
 yyzzz=_tmp28E;
goto _LL178;_LL17B:;_LL17C:
(int)_throw((void*)& Cyc_yyfail_YY23);_LL178:;}
# 1188
return yyzzz;}
# 1190
union Cyc_YYSTYPE Cyc_YY23(struct Cyc_Absyn_Tqual yy1){union Cyc_YYSTYPE _tmp9E1;return((_tmp9E1.YY23).val=yy1,(((_tmp9E1.YY23).tag=31,_tmp9E1)));}static char _tmp291[23]="list_t<aggrfield_t,`H>";
# 1179 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY24={Cyc_Core_Failure,{_tmp291,_tmp291,_tmp291 + 23}};
# 1181
struct Cyc_List_List*Cyc_yyget_YY24(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp292=yy1;struct Cyc_List_List*_tmp293;_LL17E: if((_tmp292.YY24).tag != 32)goto _LL180;_tmp293=(struct Cyc_List_List*)(_tmp292.YY24).val;_LL17F:
# 1185
 yyzzz=_tmp293;
goto _LL17D;_LL180:;_LL181:
(int)_throw((void*)& Cyc_yyfail_YY24);_LL17D:;}
# 1189
return yyzzz;}
# 1191
union Cyc_YYSTYPE Cyc_YY24(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E2;return((_tmp9E2.YY24).val=yy1,(((_tmp9E2.YY24).tag=32,_tmp9E2)));}static char _tmp296[34]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1180 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY25={Cyc_Core_Failure,{_tmp296,_tmp296,_tmp296 + 34}};
# 1182
struct Cyc_List_List*Cyc_yyget_YY25(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp297=yy1;struct Cyc_List_List*_tmp298;_LL183: if((_tmp297.YY25).tag != 33)goto _LL185;_tmp298=(struct Cyc_List_List*)(_tmp297.YY25).val;_LL184:
# 1186
 yyzzz=_tmp298;
goto _LL182;_LL185:;_LL186:
(int)_throw((void*)& Cyc_yyfail_YY25);_LL182:;}
# 1190
return yyzzz;}
# 1192
union Cyc_YYSTYPE Cyc_YY25(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E3;return((_tmp9E3.YY25).val=yy1,(((_tmp9E3.YY25).tag=33,_tmp9E3)));}static char _tmp29B[33]="list_t<type_modifier_t<`yy>,`yy>";
# 1181 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp29B,_tmp29B,_tmp29B + 33}};
# 1183
struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp29C=yy1;struct Cyc_List_List*_tmp29D;_LL188: if((_tmp29C.YY26).tag != 34)goto _LL18A;_tmp29D=(struct Cyc_List_List*)(_tmp29C.YY26).val;_LL189:
# 1187
 yyzzz=_tmp29D;
goto _LL187;_LL18A:;_LL18B:
(int)_throw((void*)& Cyc_yyfail_YY26);_LL187:;}
# 1191
return yyzzz;}
# 1193
union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E4;return((_tmp9E4.YY26).val=yy1,(((_tmp9E4.YY26).tag=34,_tmp9E4)));}static char _tmp2A0[18]="declarator_t<`yy>";
# 1182 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp2A0,_tmp2A0,_tmp2A0 + 18}};
# 1184
struct Cyc_Parse_Declarator Cyc_yyget_YY27(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declarator yyzzz;
{union Cyc_YYSTYPE _tmp2A1=yy1;struct Cyc_Parse_Declarator _tmp2A2;_LL18D: if((_tmp2A1.YY27).tag != 35)goto _LL18F;_tmp2A2=(struct Cyc_Parse_Declarator)(_tmp2A1.YY27).val;_LL18E:
# 1188
 yyzzz=_tmp2A2;
goto _LL18C;_LL18F:;_LL190:
(int)_throw((void*)& Cyc_yyfail_YY27);_LL18C:;}
# 1192
return yyzzz;}
# 1194
union Cyc_YYSTYPE Cyc_YY27(struct Cyc_Parse_Declarator yy1){union Cyc_YYSTYPE _tmp9E5;return((_tmp9E5.YY27).val=yy1,(((_tmp9E5.YY27).tag=35,_tmp9E5)));}static char _tmp2A5[45]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1183 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp2A5,_tmp2A5,_tmp2A5 + 45}};
# 1185
struct _tuple24*Cyc_yyget_YY28(union Cyc_YYSTYPE yy1){
struct _tuple24*yyzzz;
{union Cyc_YYSTYPE _tmp2A6=yy1;struct _tuple24*_tmp2A7;_LL192: if((_tmp2A6.YY28).tag != 36)goto _LL194;_tmp2A7=(struct _tuple24*)(_tmp2A6.YY28).val;_LL193:
# 1189
 yyzzz=_tmp2A7;
goto _LL191;_LL194:;_LL195:
(int)_throw((void*)& Cyc_yyfail_YY28);_LL191:;}
# 1193
return yyzzz;}
# 1195
union Cyc_YYSTYPE Cyc_YY28(struct _tuple24*yy1){union Cyc_YYSTYPE _tmp9E6;return((_tmp9E6.YY28).val=yy1,(((_tmp9E6.YY28).tag=36,_tmp9E6)));}static char _tmp2AA[57]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1184 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY29={Cyc_Core_Failure,{_tmp2AA,_tmp2AA,_tmp2AA + 57}};
# 1186
struct Cyc_List_List*Cyc_yyget_YY29(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2AB=yy1;struct Cyc_List_List*_tmp2AC;_LL197: if((_tmp2AB.YY29).tag != 37)goto _LL199;_tmp2AC=(struct Cyc_List_List*)(_tmp2AB.YY29).val;_LL198:
# 1190
 yyzzz=_tmp2AC;
goto _LL196;_LL199:;_LL19A:
(int)_throw((void*)& Cyc_yyfail_YY29);_LL196:;}
# 1194
return yyzzz;}
# 1196
union Cyc_YYSTYPE Cyc_YY29(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E7;return((_tmp9E7.YY29).val=yy1,(((_tmp9E7.YY29).tag=37,_tmp9E7)));}static char _tmp2AF[26]="abstractdeclarator_t<`yy>";
# 1185 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY30={Cyc_Core_Failure,{_tmp2AF,_tmp2AF,_tmp2AF + 26}};
# 1187
struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY30(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Abstractdeclarator yyzzz;
{union Cyc_YYSTYPE _tmp2B0=yy1;struct Cyc_Parse_Abstractdeclarator _tmp2B1;_LL19C: if((_tmp2B0.YY30).tag != 38)goto _LL19E;_tmp2B1=(struct Cyc_Parse_Abstractdeclarator)(_tmp2B0.YY30).val;_LL19D:
# 1191
 yyzzz=_tmp2B1;
goto _LL19B;_LL19E:;_LL19F:
(int)_throw((void*)& Cyc_yyfail_YY30);_LL19B:;}
# 1195
return yyzzz;}
# 1197
union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator yy1){union Cyc_YYSTYPE _tmp9E8;return((_tmp9E8.YY30).val=yy1,(((_tmp9E8.YY30).tag=38,_tmp9E8)));}static char _tmp2B4[5]="bool";
# 1186 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY31={Cyc_Core_Failure,{_tmp2B4,_tmp2B4,_tmp2B4 + 5}};
# 1188
int Cyc_yyget_YY31(union Cyc_YYSTYPE yy1){
int yyzzz;
{union Cyc_YYSTYPE _tmp2B5=yy1;int _tmp2B6;_LL1A1: if((_tmp2B5.YY31).tag != 39)goto _LL1A3;_tmp2B6=(int)(_tmp2B5.YY31).val;_LL1A2:
# 1192
 yyzzz=_tmp2B6;
goto _LL1A0;_LL1A3:;_LL1A4:
(int)_throw((void*)& Cyc_yyfail_YY31);_LL1A0:;}
# 1196
return yyzzz;}
# 1198
union Cyc_YYSTYPE Cyc_YY31(int yy1){union Cyc_YYSTYPE _tmp9E9;return((_tmp9E9.YY31).val=yy1,(((_tmp9E9.YY31).tag=39,_tmp9E9)));}static char _tmp2B9[8]="scope_t";
# 1187 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp2B9,_tmp2B9,_tmp2B9 + 8}};
# 1189
enum Cyc_Absyn_Scope Cyc_yyget_YY32(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Scope yyzzz;
{union Cyc_YYSTYPE _tmp2BA=yy1;enum Cyc_Absyn_Scope _tmp2BB;_LL1A6: if((_tmp2BA.YY32).tag != 40)goto _LL1A8;_tmp2BB=(enum Cyc_Absyn_Scope)(_tmp2BA.YY32).val;_LL1A7:
# 1193
 yyzzz=_tmp2BB;
goto _LL1A5;_LL1A8:;_LL1A9:
(int)_throw((void*)& Cyc_yyfail_YY32);_LL1A5:;}
# 1197
return yyzzz;}
# 1199
union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope yy1){union Cyc_YYSTYPE _tmp9EA;return((_tmp9EA.YY32).val=yy1,(((_tmp9EA.YY32).tag=40,_tmp9EA)));}static char _tmp2BE[16]="datatypefield_t";
# 1188 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY33={Cyc_Core_Failure,{_tmp2BE,_tmp2BE,_tmp2BE + 16}};
# 1190
struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY33(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Datatypefield*yyzzz;
{union Cyc_YYSTYPE _tmp2BF=yy1;struct Cyc_Absyn_Datatypefield*_tmp2C0;_LL1AB: if((_tmp2BF.YY33).tag != 41)goto _LL1AD;_tmp2C0=(struct Cyc_Absyn_Datatypefield*)(_tmp2BF.YY33).val;_LL1AC:
# 1194
 yyzzz=_tmp2C0;
goto _LL1AA;_LL1AD:;_LL1AE:
(int)_throw((void*)& Cyc_yyfail_YY33);_LL1AA:;}
# 1198
return yyzzz;}
# 1200
union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){union Cyc_YYSTYPE _tmp9EB;return((_tmp9EB.YY33).val=yy1,(((_tmp9EB.YY33).tag=41,_tmp9EB)));}static char _tmp2C3[27]="list_t<datatypefield_t,`H>";
# 1189 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp2C3,_tmp2C3,_tmp2C3 + 27}};
# 1191
struct Cyc_List_List*Cyc_yyget_YY34(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2C4=yy1;struct Cyc_List_List*_tmp2C5;_LL1B0: if((_tmp2C4.YY34).tag != 42)goto _LL1B2;_tmp2C5=(struct Cyc_List_List*)(_tmp2C4.YY34).val;_LL1B1:
# 1195
 yyzzz=_tmp2C5;
goto _LL1AF;_LL1B2:;_LL1B3:
(int)_throw((void*)& Cyc_yyfail_YY34);_LL1AF:;}
# 1199
return yyzzz;}
# 1201
union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9EC;return((_tmp9EC.YY34).val=yy1,(((_tmp9EC.YY34).tag=42,_tmp9EC)));}static char _tmp2C8[41]="$(tqual_t,type_specifier_t,attributes_t)";
# 1190 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp2C8,_tmp2C8,_tmp2C8 + 41}};
# 1192
struct _tuple25 Cyc_yyget_YY35(union Cyc_YYSTYPE yy1){
struct _tuple25 yyzzz;
{union Cyc_YYSTYPE _tmp2C9=yy1;struct _tuple25 _tmp2CA;_LL1B5: if((_tmp2C9.YY35).tag != 43)goto _LL1B7;_tmp2CA=(struct _tuple25)(_tmp2C9.YY35).val;_LL1B6:
# 1196
 yyzzz=_tmp2CA;
goto _LL1B4;_LL1B7:;_LL1B8:
(int)_throw((void*)& Cyc_yyfail_YY35);_LL1B4:;}
# 1200
return yyzzz;}
# 1202
union Cyc_YYSTYPE Cyc_YY35(struct _tuple25 yy1){union Cyc_YYSTYPE _tmp9ED;return((_tmp9ED.YY35).val=yy1,(((_tmp9ED.YY35).tag=43,_tmp9ED)));}static char _tmp2CD[17]="list_t<var_t,`H>";
# 1191 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY36={Cyc_Core_Failure,{_tmp2CD,_tmp2CD,_tmp2CD + 17}};
# 1193
struct Cyc_List_List*Cyc_yyget_YY36(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2CE=yy1;struct Cyc_List_List*_tmp2CF;_LL1BA: if((_tmp2CE.YY36).tag != 44)goto _LL1BC;_tmp2CF=(struct Cyc_List_List*)(_tmp2CE.YY36).val;_LL1BB:
# 1197
 yyzzz=_tmp2CF;
goto _LL1B9;_LL1BC:;_LL1BD:
(int)_throw((void*)& Cyc_yyfail_YY36);_LL1B9:;}
# 1201
return yyzzz;}
# 1203
union Cyc_YYSTYPE Cyc_YY36(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9EE;return((_tmp9EE.YY36).val=yy1,(((_tmp9EE.YY36).tag=44,_tmp9EE)));}static char _tmp2D2[31]="$(var_opt_t,tqual_t,type_t)@`H";
# 1192 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp2D2,_tmp2D2,_tmp2D2 + 31}};
# 1194
struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE yy1){
struct _tuple8*yyzzz;
{union Cyc_YYSTYPE _tmp2D3=yy1;struct _tuple8*_tmp2D4;_LL1BF: if((_tmp2D3.YY37).tag != 45)goto _LL1C1;_tmp2D4=(struct _tuple8*)(_tmp2D3.YY37).val;_LL1C0:
# 1198
 yyzzz=_tmp2D4;
goto _LL1BE;_LL1C1:;_LL1C2:
(int)_throw((void*)& Cyc_yyfail_YY37);_LL1BE:;}
# 1202
return yyzzz;}
# 1204
union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1){union Cyc_YYSTYPE _tmp9EF;return((_tmp9EF.YY37).val=yy1,(((_tmp9EF.YY37).tag=45,_tmp9EF)));}static char _tmp2D7[42]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1193 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp2D7,_tmp2D7,_tmp2D7 + 42}};
# 1195
struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2D8=yy1;struct Cyc_List_List*_tmp2D9;_LL1C4: if((_tmp2D8.YY38).tag != 46)goto _LL1C6;_tmp2D9=(struct Cyc_List_List*)(_tmp2D8.YY38).val;_LL1C5:
# 1199
 yyzzz=_tmp2D9;
goto _LL1C3;_LL1C6:;_LL1C7:
(int)_throw((void*)& Cyc_yyfail_YY38);_LL1C3:;}
# 1203
return yyzzz;}
# 1205
union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F0;return((_tmp9F0.YY38).val=yy1,(((_tmp9F0.YY38).tag=46,_tmp9F0)));}static char _tmp2DC[115]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1194 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp2DC,_tmp2DC,_tmp2DC + 115}};
# 1196
struct _tuple26*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1){
struct _tuple26*yyzzz;
{union Cyc_YYSTYPE _tmp2DD=yy1;struct _tuple26*_tmp2DE;_LL1C9: if((_tmp2DD.YY39).tag != 47)goto _LL1CB;_tmp2DE=(struct _tuple26*)(_tmp2DD.YY39).val;_LL1CA:
# 1200
 yyzzz=_tmp2DE;
goto _LL1C8;_LL1CB:;_LL1CC:
(int)_throw((void*)& Cyc_yyfail_YY39);_LL1C8:;}
# 1204
return yyzzz;}
# 1206
union Cyc_YYSTYPE Cyc_YY39(struct _tuple26*yy1){union Cyc_YYSTYPE _tmp9F1;return((_tmp9F1.YY39).val=yy1,(((_tmp9F1.YY39).tag=47,_tmp9F1)));}static char _tmp2E1[18]="list_t<type_t,`H>";
# 1195 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY40={Cyc_Core_Failure,{_tmp2E1,_tmp2E1,_tmp2E1 + 18}};
# 1197
struct Cyc_List_List*Cyc_yyget_YY40(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2E2=yy1;struct Cyc_List_List*_tmp2E3;_LL1CE: if((_tmp2E2.YY40).tag != 48)goto _LL1D0;_tmp2E3=(struct Cyc_List_List*)(_tmp2E2.YY40).val;_LL1CF:
# 1201
 yyzzz=_tmp2E3;
goto _LL1CD;_LL1D0:;_LL1D1:
(int)_throw((void*)& Cyc_yyfail_YY40);_LL1CD:;}
# 1205
return yyzzz;}
# 1207
union Cyc_YYSTYPE Cyc_YY40(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F2;return((_tmp9F2.YY40).val=yy1,(((_tmp9F2.YY40).tag=48,_tmp9F2)));}static char _tmp2E6[24]="list_t<designator_t,`H>";
# 1197 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp2E6,_tmp2E6,_tmp2E6 + 24}};
# 1199
struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2E7=yy1;struct Cyc_List_List*_tmp2E8;_LL1D3: if((_tmp2E7.YY41).tag != 49)goto _LL1D5;_tmp2E8=(struct Cyc_List_List*)(_tmp2E7.YY41).val;_LL1D4:
# 1203
 yyzzz=_tmp2E8;
goto _LL1D2;_LL1D5:;_LL1D6:
(int)_throw((void*)& Cyc_yyfail_YY41);_LL1D2:;}
# 1207
return yyzzz;}
# 1209
union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F3;return((_tmp9F3.YY41).val=yy1,(((_tmp9F3.YY41).tag=49,_tmp9F3)));}static char _tmp2EB[13]="designator_t";
# 1198 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp2EB,_tmp2EB,_tmp2EB + 13}};
# 1200
void*Cyc_yyget_YY42(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2EC=yy1;void*_tmp2ED;_LL1D8: if((_tmp2EC.YY42).tag != 50)goto _LL1DA;_tmp2ED=(void*)(_tmp2EC.YY42).val;_LL1D9:
# 1204
 yyzzz=_tmp2ED;
goto _LL1D7;_LL1DA:;_LL1DB:
(int)_throw((void*)& Cyc_yyfail_YY42);_LL1D7:;}
# 1208
return yyzzz;}
# 1210
union Cyc_YYSTYPE Cyc_YY42(void*yy1){union Cyc_YYSTYPE _tmp9F4;return((_tmp9F4.YY42).val=yy1,(((_tmp9F4.YY42).tag=50,_tmp9F4)));}static char _tmp2F0[7]="kind_t";
# 1199 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{_tmp2F0,_tmp2F0,_tmp2F0 + 7}};
# 1201
struct Cyc_Absyn_Kind*Cyc_yyget_YY43(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Kind*yyzzz;
{union Cyc_YYSTYPE _tmp2F1=yy1;struct Cyc_Absyn_Kind*_tmp2F2;_LL1DD: if((_tmp2F1.YY43).tag != 51)goto _LL1DF;_tmp2F2=(struct Cyc_Absyn_Kind*)(_tmp2F1.YY43).val;_LL1DE:
# 1205
 yyzzz=_tmp2F2;
goto _LL1DC;_LL1DF:;_LL1E0:
(int)_throw((void*)& Cyc_yyfail_YY43);_LL1DC:;}
# 1209
return yyzzz;}
# 1211
union Cyc_YYSTYPE Cyc_YY43(struct Cyc_Absyn_Kind*yy1){union Cyc_YYSTYPE _tmp9F5;return((_tmp9F5.YY43).val=yy1,(((_tmp9F5.YY43).tag=51,_tmp9F5)));}static char _tmp2F5[7]="type_t";
# 1200 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp2F5,_tmp2F5,_tmp2F5 + 7}};
# 1202
void*Cyc_yyget_YY44(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2F6=yy1;void*_tmp2F7;_LL1E2: if((_tmp2F6.YY44).tag != 52)goto _LL1E4;_tmp2F7=(void*)(_tmp2F6.YY44).val;_LL1E3:
# 1206
 yyzzz=_tmp2F7;
goto _LL1E1;_LL1E4:;_LL1E5:
(int)_throw((void*)& Cyc_yyfail_YY44);_LL1E1:;}
# 1210
return yyzzz;}
# 1212
union Cyc_YYSTYPE Cyc_YY44(void*yy1){union Cyc_YYSTYPE _tmp9F6;return((_tmp9F6.YY44).val=yy1,(((_tmp9F6.YY44).tag=52,_tmp9F6)));}static char _tmp2FA[23]="list_t<attribute_t,`H>";
# 1201 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY45={Cyc_Core_Failure,{_tmp2FA,_tmp2FA,_tmp2FA + 23}};
# 1203
struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2FB=yy1;struct Cyc_List_List*_tmp2FC;_LL1E7: if((_tmp2FB.YY45).tag != 53)goto _LL1E9;_tmp2FC=(struct Cyc_List_List*)(_tmp2FB.YY45).val;_LL1E8:
# 1207
 yyzzz=_tmp2FC;
goto _LL1E6;_LL1E9:;_LL1EA:
(int)_throw((void*)& Cyc_yyfail_YY45);_LL1E6:;}
# 1211
return yyzzz;}
# 1213
union Cyc_YYSTYPE Cyc_YY45(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F7;return((_tmp9F7.YY45).val=yy1,(((_tmp9F7.YY45).tag=53,_tmp9F7)));}static char _tmp2FF[12]="attribute_t";
# 1202 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp2FF,_tmp2FF,_tmp2FF + 12}};
# 1204
void*Cyc_yyget_YY46(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp300=yy1;void*_tmp301;_LL1EC: if((_tmp300.YY46).tag != 54)goto _LL1EE;_tmp301=(void*)(_tmp300.YY46).val;_LL1ED:
# 1208
 yyzzz=_tmp301;
goto _LL1EB;_LL1EE:;_LL1EF:
(int)_throw((void*)& Cyc_yyfail_YY46);_LL1EB:;}
# 1212
return yyzzz;}
# 1214
union Cyc_YYSTYPE Cyc_YY46(void*yy1){union Cyc_YYSTYPE _tmp9F8;return((_tmp9F8.YY46).val=yy1,(((_tmp9F8.YY46).tag=54,_tmp9F8)));}static char _tmp304[12]="enumfield_t";
# 1203 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp304,_tmp304,_tmp304 + 12}};
# 1205
struct Cyc_Absyn_Enumfield*Cyc_yyget_YY47(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Enumfield*yyzzz;
{union Cyc_YYSTYPE _tmp305=yy1;struct Cyc_Absyn_Enumfield*_tmp306;_LL1F1: if((_tmp305.YY47).tag != 55)goto _LL1F3;_tmp306=(struct Cyc_Absyn_Enumfield*)(_tmp305.YY47).val;_LL1F2:
# 1209
 yyzzz=_tmp306;
goto _LL1F0;_LL1F3:;_LL1F4:
(int)_throw((void*)& Cyc_yyfail_YY47);_LL1F0:;}
# 1213
return yyzzz;}
# 1215
union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){union Cyc_YYSTYPE _tmp9F9;return((_tmp9F9.YY47).val=yy1,(((_tmp9F9.YY47).tag=55,_tmp9F9)));}static char _tmp309[23]="list_t<enumfield_t,`H>";
# 1204 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY48={Cyc_Core_Failure,{_tmp309,_tmp309,_tmp309 + 23}};
# 1206
struct Cyc_List_List*Cyc_yyget_YY48(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp30A=yy1;struct Cyc_List_List*_tmp30B;_LL1F6: if((_tmp30A.YY48).tag != 56)goto _LL1F8;_tmp30B=(struct Cyc_List_List*)(_tmp30A.YY48).val;_LL1F7:
# 1210
 yyzzz=_tmp30B;
goto _LL1F5;_LL1F8:;_LL1F9:
(int)_throw((void*)& Cyc_yyfail_YY48);_LL1F5:;}
# 1214
return yyzzz;}
# 1216
union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9FA;return((_tmp9FA.YY48).val=yy1,(((_tmp9FA.YY48).tag=56,_tmp9FA)));}static char _tmp30E[11]="type_opt_t";
# 1205 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp30E,_tmp30E,_tmp30E + 11}};
# 1207
void*Cyc_yyget_YY49(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp30F=yy1;void*_tmp310;_LL1FB: if((_tmp30F.YY49).tag != 57)goto _LL1FD;_tmp310=(void*)(_tmp30F.YY49).val;_LL1FC:
# 1211
 yyzzz=_tmp310;
goto _LL1FA;_LL1FD:;_LL1FE:
(int)_throw((void*)& Cyc_yyfail_YY49);_LL1FA:;}
# 1215
return yyzzz;}
# 1217
union Cyc_YYSTYPE Cyc_YY49(void*yy1){union Cyc_YYSTYPE _tmp9FB;return((_tmp9FB.YY49).val=yy1,(((_tmp9FB.YY49).tag=57,_tmp9FB)));}static char _tmp313[31]="list_t<$(type_t,type_t)@`H,`H>";
# 1206 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp313,_tmp313,_tmp313 + 31}};
# 1208
struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp314=yy1;struct Cyc_List_List*_tmp315;_LL200: if((_tmp314.YY50).tag != 58)goto _LL202;_tmp315=(struct Cyc_List_List*)(_tmp314.YY50).val;_LL201:
# 1212
 yyzzz=_tmp315;
goto _LL1FF;_LL202:;_LL203:
(int)_throw((void*)& Cyc_yyfail_YY50);_LL1FF:;}
# 1216
return yyzzz;}
# 1218
union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9FC;return((_tmp9FC.YY50).val=yy1,(((_tmp9FC.YY50).tag=58,_tmp9FC)));}static char _tmp318[15]="conref_t<bool>";
# 1207 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY51={Cyc_Core_Failure,{_tmp318,_tmp318,_tmp318 + 15}};
# 1209
union Cyc_Absyn_Constraint*Cyc_yyget_YY51(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Constraint*yyzzz;
{union Cyc_YYSTYPE _tmp319=yy1;union Cyc_Absyn_Constraint*_tmp31A;_LL205: if((_tmp319.YY51).tag != 59)goto _LL207;_tmp31A=(union Cyc_Absyn_Constraint*)(_tmp319.YY51).val;_LL206:
# 1213
 yyzzz=_tmp31A;
goto _LL204;_LL207:;_LL208:
(int)_throw((void*)& Cyc_yyfail_YY51);_LL204:;}
# 1217
return yyzzz;}
# 1219
union Cyc_YYSTYPE Cyc_YY51(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp9FD;return((_tmp9FD.YY51).val=yy1,(((_tmp9FD.YY51).tag=59,_tmp9FD)));}static char _tmp31D[45]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";
# 1208 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp31D,_tmp31D,_tmp31D + 45}};
# 1210
struct Cyc_List_List*Cyc_yyget_YY52(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp31E=yy1;struct Cyc_List_List*_tmp31F;_LL20A: if((_tmp31E.YY52).tag != 60)goto _LL20C;_tmp31F=(struct Cyc_List_List*)(_tmp31E.YY52).val;_LL20B:
# 1214
 yyzzz=_tmp31F;
goto _LL209;_LL20C:;_LL20D:
(int)_throw((void*)& Cyc_yyfail_YY52);_LL209:;}
# 1218
return yyzzz;}
# 1220
union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9FE;return((_tmp9FE.YY52).val=yy1,(((_tmp9FE.YY52).tag=60,_tmp9FE)));}static char _tmp322[20]="pointer_qual_t<`yy>";
# 1209 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY53={Cyc_Core_Failure,{_tmp322,_tmp322,_tmp322 + 20}};
# 1211
void*Cyc_yyget_YY53(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp323=yy1;void*_tmp324;_LL20F: if((_tmp323.YY53).tag != 61)goto _LL211;_tmp324=(void*)(_tmp323.YY53).val;_LL210:
# 1215
 yyzzz=_tmp324;
goto _LL20E;_LL211:;_LL212:
(int)_throw((void*)& Cyc_yyfail_YY53);_LL20E:;}
# 1219
return yyzzz;}
# 1221
union Cyc_YYSTYPE Cyc_YY53(void*yy1){union Cyc_YYSTYPE _tmp9FF;return((_tmp9FF.YY53).val=yy1,(((_tmp9FF.YY53).tag=61,_tmp9FF)));}static char _tmp327[21]="pointer_quals_t<`yy>";
# 1210 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY54={Cyc_Core_Failure,{_tmp327,_tmp327,_tmp327 + 21}};
# 1212
struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp328=yy1;struct Cyc_List_List*_tmp329;_LL214: if((_tmp328.YY54).tag != 62)goto _LL216;_tmp329=(struct Cyc_List_List*)(_tmp328.YY54).val;_LL215:
# 1216
 yyzzz=_tmp329;
goto _LL213;_LL216:;_LL217:
(int)_throw((void*)& Cyc_yyfail_YY54);_LL213:;}
# 1220
return yyzzz;}
# 1222
union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA00;return((_tmpA00.YY54).val=yy1,(((_tmpA00.YY54).tag=62,_tmpA00)));}static char _tmp32C[21]="$(bool,string_t<`H>)";
# 1211 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Asm_tok={Cyc_Core_Failure,{_tmp32C,_tmp32C,_tmp32C + 21}};
# 1213
struct _tuple20 Cyc_yyget_Asm_tok(union Cyc_YYSTYPE yy1){
struct _tuple20 yyzzz;
{union Cyc_YYSTYPE _tmp32D=yy1;struct _tuple20 _tmp32E;_LL219: if((_tmp32D.Asm_tok).tag != 6)goto _LL21B;_tmp32E=(struct _tuple20)(_tmp32D.Asm_tok).val;_LL21A:
# 1217
 yyzzz=_tmp32E;
goto _LL218;_LL21B:;_LL21C:
(int)_throw((void*)& Cyc_yyfail_Asm_tok);_LL218:;}
# 1221
return yyzzz;}
# 1223
union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple20 yy1){union Cyc_YYSTYPE _tmpA01;return((_tmpA01.Asm_tok).val=yy1,(((_tmpA01.Asm_tok).tag=6,_tmpA01)));}static char _tmp331[10]="exp_opt_t";
# 1212 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY55={Cyc_Core_Failure,{_tmp331,_tmp331,_tmp331 + 10}};
# 1214
struct Cyc_Absyn_Exp*Cyc_yyget_YY55(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp332=yy1;struct Cyc_Absyn_Exp*_tmp333;_LL21E: if((_tmp332.YY55).tag != 63)goto _LL220;_tmp333=(struct Cyc_Absyn_Exp*)(_tmp332.YY55).val;_LL21F:
# 1218
 yyzzz=_tmp333;
goto _LL21D;_LL220:;_LL221:
(int)_throw((void*)& Cyc_yyfail_YY55);_LL21D:;}
# 1222
return yyzzz;}
# 1224
union Cyc_YYSTYPE Cyc_YY55(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmpA02;return((_tmpA02.YY55).val=yy1,(((_tmpA02.YY55).tag=63,_tmpA02)));}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1236
typedef struct Cyc_Yyltype Cyc_yyltype;
# 1240
struct Cyc_Yyltype Cyc_yynewloc(){
struct Cyc_Yyltype _tmpA03;return(_tmpA03.timestamp=0,((_tmpA03.first_line=0,((_tmpA03.first_column=0,((_tmpA03.last_line=0,((_tmpA03.last_column=0,_tmpA03)))))))));}
# 1243
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1254 "parse.y"
static short Cyc_yytranslate[373]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,144,2,2,128,142,139,2,125,126,133,136,121,140,130,141,2,2,2,2,2,2,2,2,2,2,129,118,123,122,124,135,134,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,131,2,132,138,127,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,119,137,120,143,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117};static char _tmp336[2]="$";static char _tmp337[6]="error";static char _tmp338[12]="$undefined.";static char _tmp339[5]="AUTO";static char _tmp33A[9]="REGISTER";static char _tmp33B[7]="STATIC";static char _tmp33C[7]="EXTERN";static char _tmp33D[8]="TYPEDEF";static char _tmp33E[5]="VOID";static char _tmp33F[5]="CHAR";static char _tmp340[6]="SHORT";static char _tmp341[4]="INT";static char _tmp342[5]="LONG";static char _tmp343[6]="FLOAT";static char _tmp344[7]="DOUBLE";static char _tmp345[7]="SIGNED";static char _tmp346[9]="UNSIGNED";static char _tmp347[6]="CONST";static char _tmp348[9]="VOLATILE";static char _tmp349[9]="RESTRICT";static char _tmp34A[7]="STRUCT";static char _tmp34B[6]="UNION";static char _tmp34C[5]="CASE";static char _tmp34D[8]="DEFAULT";static char _tmp34E[7]="INLINE";static char _tmp34F[7]="SIZEOF";static char _tmp350[9]="OFFSETOF";static char _tmp351[3]="IF";static char _tmp352[5]="ELSE";static char _tmp353[7]="SWITCH";static char _tmp354[6]="WHILE";static char _tmp355[3]="DO";static char _tmp356[4]="FOR";static char _tmp357[5]="GOTO";static char _tmp358[9]="CONTINUE";static char _tmp359[6]="BREAK";static char _tmp35A[7]="RETURN";static char _tmp35B[5]="ENUM";static char _tmp35C[8]="NULL_kw";static char _tmp35D[4]="LET";static char _tmp35E[6]="THROW";static char _tmp35F[4]="TRY";static char _tmp360[6]="CATCH";static char _tmp361[7]="EXPORT";static char _tmp362[4]="NEW";static char _tmp363[9]="ABSTRACT";static char _tmp364[9]="FALLTHRU";static char _tmp365[6]="USING";static char _tmp366[10]="NAMESPACE";static char _tmp367[9]="DATATYPE";static char _tmp368[7]="MALLOC";static char _tmp369[8]="RMALLOC";static char _tmp36A[7]="CALLOC";static char _tmp36B[8]="RCALLOC";static char _tmp36C[5]="SWAP";static char _tmp36D[9]="REGION_T";static char _tmp36E[6]="TAG_T";static char _tmp36F[7]="REGION";static char _tmp370[5]="RNEW";static char _tmp371[8]="REGIONS";static char _tmp372[13]="RESET_REGION";static char _tmp373[7]="PORTON";static char _tmp374[8]="PORTOFF";static char _tmp375[12]="DYNREGION_T";static char _tmp376[8]="NUMELTS";static char _tmp377[8]="VALUEOF";static char _tmp378[10]="VALUEOF_T";static char _tmp379[9]="TAGCHECK";static char _tmp37A[13]="NUMELTS_QUAL";static char _tmp37B[10]="THIN_QUAL";static char _tmp37C[9]="FAT_QUAL";static char _tmp37D[13]="NOTNULL_QUAL";static char _tmp37E[14]="NULLABLE_QUAL";static char _tmp37F[14]="REQUIRES_QUAL";static char _tmp380[13]="ENSURES_QUAL";static char _tmp381[12]="REGION_QUAL";static char _tmp382[16]="NOZEROTERM_QUAL";static char _tmp383[14]="ZEROTERM_QUAL";static char _tmp384[12]="TAGGED_QUAL";static char _tmp385[16]="EXTENSIBLE_QUAL";static char _tmp386[15]="RESETABLE_QUAL";static char _tmp387[7]="PTR_OP";static char _tmp388[7]="INC_OP";static char _tmp389[7]="DEC_OP";static char _tmp38A[8]="LEFT_OP";static char _tmp38B[9]="RIGHT_OP";static char _tmp38C[6]="LE_OP";static char _tmp38D[6]="GE_OP";static char _tmp38E[6]="EQ_OP";static char _tmp38F[6]="NE_OP";static char _tmp390[7]="AND_OP";static char _tmp391[6]="OR_OP";static char _tmp392[11]="MUL_ASSIGN";static char _tmp393[11]="DIV_ASSIGN";static char _tmp394[11]="MOD_ASSIGN";static char _tmp395[11]="ADD_ASSIGN";static char _tmp396[11]="SUB_ASSIGN";static char _tmp397[12]="LEFT_ASSIGN";static char _tmp398[13]="RIGHT_ASSIGN";static char _tmp399[11]="AND_ASSIGN";static char _tmp39A[11]="XOR_ASSIGN";static char _tmp39B[10]="OR_ASSIGN";static char _tmp39C[9]="ELLIPSIS";static char _tmp39D[11]="LEFT_RIGHT";static char _tmp39E[12]="COLON_COLON";static char _tmp39F[11]="IDENTIFIER";static char _tmp3A0[17]="INTEGER_CONSTANT";static char _tmp3A1[7]="STRING";static char _tmp3A2[8]="WSTRING";static char _tmp3A3[19]="CHARACTER_CONSTANT";static char _tmp3A4[20]="WCHARACTER_CONSTANT";static char _tmp3A5[18]="FLOATING_CONSTANT";static char _tmp3A6[9]="TYPE_VAR";static char _tmp3A7[13]="TYPEDEF_NAME";static char _tmp3A8[16]="QUAL_IDENTIFIER";static char _tmp3A9[18]="QUAL_TYPEDEF_NAME";static char _tmp3AA[10]="ATTRIBUTE";static char _tmp3AB[4]="ASM";static char _tmp3AC[4]="';'";static char _tmp3AD[4]="'{'";static char _tmp3AE[4]="'}'";static char _tmp3AF[4]="','";static char _tmp3B0[4]="'='";static char _tmp3B1[4]="'<'";static char _tmp3B2[4]="'>'";static char _tmp3B3[4]="'('";static char _tmp3B4[4]="')'";static char _tmp3B5[4]="'_'";static char _tmp3B6[4]="'$'";static char _tmp3B7[4]="':'";static char _tmp3B8[4]="'.'";static char _tmp3B9[4]="'['";static char _tmp3BA[4]="']'";static char _tmp3BB[4]="'*'";static char _tmp3BC[4]="'@'";static char _tmp3BD[4]="'?'";static char _tmp3BE[4]="'+'";static char _tmp3BF[4]="'|'";static char _tmp3C0[4]="'^'";static char _tmp3C1[4]="'&'";static char _tmp3C2[4]="'-'";static char _tmp3C3[4]="'/'";static char _tmp3C4[4]="'%'";static char _tmp3C5[4]="'~'";static char _tmp3C6[4]="'!'";static char _tmp3C7[5]="prog";static char _tmp3C8[17]="translation_unit";static char _tmp3C9[16]="extern_c_action";static char _tmp3CA[13]="end_extern_c";static char _tmp3CB[12]="export_list";static char _tmp3CC[19]="export_list_values";static char _tmp3CD[21]="external_declaration";static char _tmp3CE[15]="optional_comma";static char _tmp3CF[20]="function_definition";static char _tmp3D0[21]="function_definition2";static char _tmp3D1[13]="using_action";static char _tmp3D2[15]="unusing_action";static char _tmp3D3[17]="namespace_action";static char _tmp3D4[19]="unnamespace_action";static char _tmp3D5[12]="declaration";static char _tmp3D6[19]="resetable_qual_opt";static char _tmp3D7[17]="declaration_list";static char _tmp3D8[23]="declaration_specifiers";static char _tmp3D9[24]="storage_class_specifier";static char _tmp3DA[15]="attributes_opt";static char _tmp3DB[11]="attributes";static char _tmp3DC[15]="attribute_list";static char _tmp3DD[10]="attribute";static char _tmp3DE[15]="type_specifier";static char _tmp3DF[25]="type_specifier_notypedef";static char _tmp3E0[5]="kind";static char _tmp3E1[15]="type_qualifier";static char _tmp3E2[15]="enum_specifier";static char _tmp3E3[11]="enum_field";static char _tmp3E4[22]="enum_declaration_list";static char _tmp3E5[26]="struct_or_union_specifier";static char _tmp3E6[16]="type_params_opt";static char _tmp3E7[16]="struct_or_union";static char _tmp3E8[24]="struct_declaration_list";static char _tmp3E9[25]="struct_declaration_list0";static char _tmp3EA[21]="init_declarator_list";static char _tmp3EB[22]="init_declarator_list0";static char _tmp3EC[16]="init_declarator";static char _tmp3ED[19]="struct_declaration";static char _tmp3EE[25]="specifier_qualifier_list";static char _tmp3EF[35]="notypedef_specifier_qualifier_list";static char _tmp3F0[23]="struct_declarator_list";static char _tmp3F1[24]="struct_declarator_list0";static char _tmp3F2[18]="struct_declarator";static char _tmp3F3[20]="requires_clause_opt";static char _tmp3F4[19]="ensures_clause_opt";static char _tmp3F5[19]="datatype_specifier";static char _tmp3F6[14]="qual_datatype";static char _tmp3F7[19]="datatypefield_list";static char _tmp3F8[20]="datatypefield_scope";static char _tmp3F9[14]="datatypefield";static char _tmp3FA[11]="declarator";static char _tmp3FB[23]="declarator_withtypedef";static char _tmp3FC[18]="direct_declarator";static char _tmp3FD[30]="direct_declarator_withtypedef";static char _tmp3FE[8]="pointer";static char _tmp3FF[12]="one_pointer";static char _tmp400[14]="pointer_quals";static char _tmp401[13]="pointer_qual";static char _tmp402[23]="pointer_null_and_bound";static char _tmp403[14]="pointer_bound";static char _tmp404[18]="zeroterm_qual_opt";static char _tmp405[8]="rgn_opt";static char _tmp406[11]="tqual_list";static char _tmp407[20]="parameter_type_list";static char _tmp408[9]="type_var";static char _tmp409[16]="optional_effect";static char _tmp40A[19]="optional_rgn_order";static char _tmp40B[10]="rgn_order";static char _tmp40C[16]="optional_inject";static char _tmp40D[11]="effect_set";static char _tmp40E[14]="atomic_effect";static char _tmp40F[11]="region_set";static char _tmp410[15]="parameter_list";static char _tmp411[22]="parameter_declaration";static char _tmp412[16]="identifier_list";static char _tmp413[17]="identifier_list0";static char _tmp414[12]="initializer";static char _tmp415[18]="array_initializer";static char _tmp416[17]="initializer_list";static char _tmp417[12]="designation";static char _tmp418[16]="designator_list";static char _tmp419[11]="designator";static char _tmp41A[10]="type_name";static char _tmp41B[14]="any_type_name";static char _tmp41C[15]="type_name_list";static char _tmp41D[20]="abstract_declarator";static char _tmp41E[27]="direct_abstract_declarator";static char _tmp41F[10]="statement";static char _tmp420[18]="labeled_statement";static char _tmp421[21]="expression_statement";static char _tmp422[19]="compound_statement";static char _tmp423[16]="block_item_list";static char _tmp424[20]="selection_statement";static char _tmp425[15]="switch_clauses";static char _tmp426[20]="iteration_statement";static char _tmp427[15]="jump_statement";static char _tmp428[12]="exp_pattern";static char _tmp429[20]="conditional_pattern";static char _tmp42A[19]="logical_or_pattern";static char _tmp42B[20]="logical_and_pattern";static char _tmp42C[21]="inclusive_or_pattern";static char _tmp42D[21]="exclusive_or_pattern";static char _tmp42E[12]="and_pattern";static char _tmp42F[17]="equality_pattern";static char _tmp430[19]="relational_pattern";static char _tmp431[14]="shift_pattern";static char _tmp432[17]="additive_pattern";static char _tmp433[23]="multiplicative_pattern";static char _tmp434[13]="cast_pattern";static char _tmp435[14]="unary_pattern";static char _tmp436[16]="postfix_pattern";static char _tmp437[16]="primary_pattern";static char _tmp438[8]="pattern";static char _tmp439[19]="tuple_pattern_list";static char _tmp43A[20]="tuple_pattern_list0";static char _tmp43B[14]="field_pattern";static char _tmp43C[19]="field_pattern_list";static char _tmp43D[20]="field_pattern_list0";static char _tmp43E[11]="expression";static char _tmp43F[22]="assignment_expression";static char _tmp440[20]="assignment_operator";static char _tmp441[23]="conditional_expression";static char _tmp442[20]="constant_expression";static char _tmp443[22]="logical_or_expression";static char _tmp444[23]="logical_and_expression";static char _tmp445[24]="inclusive_or_expression";static char _tmp446[24]="exclusive_or_expression";static char _tmp447[15]="and_expression";static char _tmp448[20]="equality_expression";static char _tmp449[22]="relational_expression";static char _tmp44A[17]="shift_expression";static char _tmp44B[20]="additive_expression";static char _tmp44C[26]="multiplicative_expression";static char _tmp44D[16]="cast_expression";static char _tmp44E[17]="unary_expression";static char _tmp44F[15]="unary_operator";static char _tmp450[19]="postfix_expression";static char _tmp451[17]="field_expression";static char _tmp452[19]="primary_expression";static char _tmp453[25]="argument_expression_list";static char _tmp454[26]="argument_expression_list0";static char _tmp455[9]="constant";static char _tmp456[20]="qual_opt_identifier";static char _tmp457[17]="qual_opt_typedef";static char _tmp458[18]="struct_union_name";static char _tmp459[11]="field_name";static char _tmp45A[12]="right_angle";
# 1606 "parse.y"
static struct _dyneither_ptr Cyc_yytname[293]={{_tmp336,_tmp336,_tmp336 + 2},{_tmp337,_tmp337,_tmp337 + 6},{_tmp338,_tmp338,_tmp338 + 12},{_tmp339,_tmp339,_tmp339 + 5},{_tmp33A,_tmp33A,_tmp33A + 9},{_tmp33B,_tmp33B,_tmp33B + 7},{_tmp33C,_tmp33C,_tmp33C + 7},{_tmp33D,_tmp33D,_tmp33D + 8},{_tmp33E,_tmp33E,_tmp33E + 5},{_tmp33F,_tmp33F,_tmp33F + 5},{_tmp340,_tmp340,_tmp340 + 6},{_tmp341,_tmp341,_tmp341 + 4},{_tmp342,_tmp342,_tmp342 + 5},{_tmp343,_tmp343,_tmp343 + 6},{_tmp344,_tmp344,_tmp344 + 7},{_tmp345,_tmp345,_tmp345 + 7},{_tmp346,_tmp346,_tmp346 + 9},{_tmp347,_tmp347,_tmp347 + 6},{_tmp348,_tmp348,_tmp348 + 9},{_tmp349,_tmp349,_tmp349 + 9},{_tmp34A,_tmp34A,_tmp34A + 7},{_tmp34B,_tmp34B,_tmp34B + 6},{_tmp34C,_tmp34C,_tmp34C + 5},{_tmp34D,_tmp34D,_tmp34D + 8},{_tmp34E,_tmp34E,_tmp34E + 7},{_tmp34F,_tmp34F,_tmp34F + 7},{_tmp350,_tmp350,_tmp350 + 9},{_tmp351,_tmp351,_tmp351 + 3},{_tmp352,_tmp352,_tmp352 + 5},{_tmp353,_tmp353,_tmp353 + 7},{_tmp354,_tmp354,_tmp354 + 6},{_tmp355,_tmp355,_tmp355 + 3},{_tmp356,_tmp356,_tmp356 + 4},{_tmp357,_tmp357,_tmp357 + 5},{_tmp358,_tmp358,_tmp358 + 9},{_tmp359,_tmp359,_tmp359 + 6},{_tmp35A,_tmp35A,_tmp35A + 7},{_tmp35B,_tmp35B,_tmp35B + 5},{_tmp35C,_tmp35C,_tmp35C + 8},{_tmp35D,_tmp35D,_tmp35D + 4},{_tmp35E,_tmp35E,_tmp35E + 6},{_tmp35F,_tmp35F,_tmp35F + 4},{_tmp360,_tmp360,_tmp360 + 6},{_tmp361,_tmp361,_tmp361 + 7},{_tmp362,_tmp362,_tmp362 + 4},{_tmp363,_tmp363,_tmp363 + 9},{_tmp364,_tmp364,_tmp364 + 9},{_tmp365,_tmp365,_tmp365 + 6},{_tmp366,_tmp366,_tmp366 + 10},{_tmp367,_tmp367,_tmp367 + 9},{_tmp368,_tmp368,_tmp368 + 7},{_tmp369,_tmp369,_tmp369 + 8},{_tmp36A,_tmp36A,_tmp36A + 7},{_tmp36B,_tmp36B,_tmp36B + 8},{_tmp36C,_tmp36C,_tmp36C + 5},{_tmp36D,_tmp36D,_tmp36D + 9},{_tmp36E,_tmp36E,_tmp36E + 6},{_tmp36F,_tmp36F,_tmp36F + 7},{_tmp370,_tmp370,_tmp370 + 5},{_tmp371,_tmp371,_tmp371 + 8},{_tmp372,_tmp372,_tmp372 + 13},{_tmp373,_tmp373,_tmp373 + 7},{_tmp374,_tmp374,_tmp374 + 8},{_tmp375,_tmp375,_tmp375 + 12},{_tmp376,_tmp376,_tmp376 + 8},{_tmp377,_tmp377,_tmp377 + 8},{_tmp378,_tmp378,_tmp378 + 10},{_tmp379,_tmp379,_tmp379 + 9},{_tmp37A,_tmp37A,_tmp37A + 13},{_tmp37B,_tmp37B,_tmp37B + 10},{_tmp37C,_tmp37C,_tmp37C + 9},{_tmp37D,_tmp37D,_tmp37D + 13},{_tmp37E,_tmp37E,_tmp37E + 14},{_tmp37F,_tmp37F,_tmp37F + 14},{_tmp380,_tmp380,_tmp380 + 13},{_tmp381,_tmp381,_tmp381 + 12},{_tmp382,_tmp382,_tmp382 + 16},{_tmp383,_tmp383,_tmp383 + 14},{_tmp384,_tmp384,_tmp384 + 12},{_tmp385,_tmp385,_tmp385 + 16},{_tmp386,_tmp386,_tmp386 + 15},{_tmp387,_tmp387,_tmp387 + 7},{_tmp388,_tmp388,_tmp388 + 7},{_tmp389,_tmp389,_tmp389 + 7},{_tmp38A,_tmp38A,_tmp38A + 8},{_tmp38B,_tmp38B,_tmp38B + 9},{_tmp38C,_tmp38C,_tmp38C + 6},{_tmp38D,_tmp38D,_tmp38D + 6},{_tmp38E,_tmp38E,_tmp38E + 6},{_tmp38F,_tmp38F,_tmp38F + 6},{_tmp390,_tmp390,_tmp390 + 7},{_tmp391,_tmp391,_tmp391 + 6},{_tmp392,_tmp392,_tmp392 + 11},{_tmp393,_tmp393,_tmp393 + 11},{_tmp394,_tmp394,_tmp394 + 11},{_tmp395,_tmp395,_tmp395 + 11},{_tmp396,_tmp396,_tmp396 + 11},{_tmp397,_tmp397,_tmp397 + 12},{_tmp398,_tmp398,_tmp398 + 13},{_tmp399,_tmp399,_tmp399 + 11},{_tmp39A,_tmp39A,_tmp39A + 11},{_tmp39B,_tmp39B,_tmp39B + 10},{_tmp39C,_tmp39C,_tmp39C + 9},{_tmp39D,_tmp39D,_tmp39D + 11},{_tmp39E,_tmp39E,_tmp39E + 12},{_tmp39F,_tmp39F,_tmp39F + 11},{_tmp3A0,_tmp3A0,_tmp3A0 + 17},{_tmp3A1,_tmp3A1,_tmp3A1 + 7},{_tmp3A2,_tmp3A2,_tmp3A2 + 8},{_tmp3A3,_tmp3A3,_tmp3A3 + 19},{_tmp3A4,_tmp3A4,_tmp3A4 + 20},{_tmp3A5,_tmp3A5,_tmp3A5 + 18},{_tmp3A6,_tmp3A6,_tmp3A6 + 9},{_tmp3A7,_tmp3A7,_tmp3A7 + 13},{_tmp3A8,_tmp3A8,_tmp3A8 + 16},{_tmp3A9,_tmp3A9,_tmp3A9 + 18},{_tmp3AA,_tmp3AA,_tmp3AA + 10},{_tmp3AB,_tmp3AB,_tmp3AB + 4},{_tmp3AC,_tmp3AC,_tmp3AC + 4},{_tmp3AD,_tmp3AD,_tmp3AD + 4},{_tmp3AE,_tmp3AE,_tmp3AE + 4},{_tmp3AF,_tmp3AF,_tmp3AF + 4},{_tmp3B0,_tmp3B0,_tmp3B0 + 4},{_tmp3B1,_tmp3B1,_tmp3B1 + 4},{_tmp3B2,_tmp3B2,_tmp3B2 + 4},{_tmp3B3,_tmp3B3,_tmp3B3 + 4},{_tmp3B4,_tmp3B4,_tmp3B4 + 4},{_tmp3B5,_tmp3B5,_tmp3B5 + 4},{_tmp3B6,_tmp3B6,_tmp3B6 + 4},{_tmp3B7,_tmp3B7,_tmp3B7 + 4},{_tmp3B8,_tmp3B8,_tmp3B8 + 4},{_tmp3B9,_tmp3B9,_tmp3B9 + 4},{_tmp3BA,_tmp3BA,_tmp3BA + 4},{_tmp3BB,_tmp3BB,_tmp3BB + 4},{_tmp3BC,_tmp3BC,_tmp3BC + 4},{_tmp3BD,_tmp3BD,_tmp3BD + 4},{_tmp3BE,_tmp3BE,_tmp3BE + 4},{_tmp3BF,_tmp3BF,_tmp3BF + 4},{_tmp3C0,_tmp3C0,_tmp3C0 + 4},{_tmp3C1,_tmp3C1,_tmp3C1 + 4},{_tmp3C2,_tmp3C2,_tmp3C2 + 4},{_tmp3C3,_tmp3C3,_tmp3C3 + 4},{_tmp3C4,_tmp3C4,_tmp3C4 + 4},{_tmp3C5,_tmp3C5,_tmp3C5 + 4},{_tmp3C6,_tmp3C6,_tmp3C6 + 4},{_tmp3C7,_tmp3C7,_tmp3C7 + 5},{_tmp3C8,_tmp3C8,_tmp3C8 + 17},{_tmp3C9,_tmp3C9,_tmp3C9 + 16},{_tmp3CA,_tmp3CA,_tmp3CA + 13},{_tmp3CB,_tmp3CB,_tmp3CB + 12},{_tmp3CC,_tmp3CC,_tmp3CC + 19},{_tmp3CD,_tmp3CD,_tmp3CD + 21},{_tmp3CE,_tmp3CE,_tmp3CE + 15},{_tmp3CF,_tmp3CF,_tmp3CF + 20},{_tmp3D0,_tmp3D0,_tmp3D0 + 21},{_tmp3D1,_tmp3D1,_tmp3D1 + 13},{_tmp3D2,_tmp3D2,_tmp3D2 + 15},{_tmp3D3,_tmp3D3,_tmp3D3 + 17},{_tmp3D4,_tmp3D4,_tmp3D4 + 19},{_tmp3D5,_tmp3D5,_tmp3D5 + 12},{_tmp3D6,_tmp3D6,_tmp3D6 + 19},{_tmp3D7,_tmp3D7,_tmp3D7 + 17},{_tmp3D8,_tmp3D8,_tmp3D8 + 23},{_tmp3D9,_tmp3D9,_tmp3D9 + 24},{_tmp3DA,_tmp3DA,_tmp3DA + 15},{_tmp3DB,_tmp3DB,_tmp3DB + 11},{_tmp3DC,_tmp3DC,_tmp3DC + 15},{_tmp3DD,_tmp3DD,_tmp3DD + 10},{_tmp3DE,_tmp3DE,_tmp3DE + 15},{_tmp3DF,_tmp3DF,_tmp3DF + 25},{_tmp3E0,_tmp3E0,_tmp3E0 + 5},{_tmp3E1,_tmp3E1,_tmp3E1 + 15},{_tmp3E2,_tmp3E2,_tmp3E2 + 15},{_tmp3E3,_tmp3E3,_tmp3E3 + 11},{_tmp3E4,_tmp3E4,_tmp3E4 + 22},{_tmp3E5,_tmp3E5,_tmp3E5 + 26},{_tmp3E6,_tmp3E6,_tmp3E6 + 16},{_tmp3E7,_tmp3E7,_tmp3E7 + 16},{_tmp3E8,_tmp3E8,_tmp3E8 + 24},{_tmp3E9,_tmp3E9,_tmp3E9 + 25},{_tmp3EA,_tmp3EA,_tmp3EA + 21},{_tmp3EB,_tmp3EB,_tmp3EB + 22},{_tmp3EC,_tmp3EC,_tmp3EC + 16},{_tmp3ED,_tmp3ED,_tmp3ED + 19},{_tmp3EE,_tmp3EE,_tmp3EE + 25},{_tmp3EF,_tmp3EF,_tmp3EF + 35},{_tmp3F0,_tmp3F0,_tmp3F0 + 23},{_tmp3F1,_tmp3F1,_tmp3F1 + 24},{_tmp3F2,_tmp3F2,_tmp3F2 + 18},{_tmp3F3,_tmp3F3,_tmp3F3 + 20},{_tmp3F4,_tmp3F4,_tmp3F4 + 19},{_tmp3F5,_tmp3F5,_tmp3F5 + 19},{_tmp3F6,_tmp3F6,_tmp3F6 + 14},{_tmp3F7,_tmp3F7,_tmp3F7 + 19},{_tmp3F8,_tmp3F8,_tmp3F8 + 20},{_tmp3F9,_tmp3F9,_tmp3F9 + 14},{_tmp3FA,_tmp3FA,_tmp3FA + 11},{_tmp3FB,_tmp3FB,_tmp3FB + 23},{_tmp3FC,_tmp3FC,_tmp3FC + 18},{_tmp3FD,_tmp3FD,_tmp3FD + 30},{_tmp3FE,_tmp3FE,_tmp3FE + 8},{_tmp3FF,_tmp3FF,_tmp3FF + 12},{_tmp400,_tmp400,_tmp400 + 14},{_tmp401,_tmp401,_tmp401 + 13},{_tmp402,_tmp402,_tmp402 + 23},{_tmp403,_tmp403,_tmp403 + 14},{_tmp404,_tmp404,_tmp404 + 18},{_tmp405,_tmp405,_tmp405 + 8},{_tmp406,_tmp406,_tmp406 + 11},{_tmp407,_tmp407,_tmp407 + 20},{_tmp408,_tmp408,_tmp408 + 9},{_tmp409,_tmp409,_tmp409 + 16},{_tmp40A,_tmp40A,_tmp40A + 19},{_tmp40B,_tmp40B,_tmp40B + 10},{_tmp40C,_tmp40C,_tmp40C + 16},{_tmp40D,_tmp40D,_tmp40D + 11},{_tmp40E,_tmp40E,_tmp40E + 14},{_tmp40F,_tmp40F,_tmp40F + 11},{_tmp410,_tmp410,_tmp410 + 15},{_tmp411,_tmp411,_tmp411 + 22},{_tmp412,_tmp412,_tmp412 + 16},{_tmp413,_tmp413,_tmp413 + 17},{_tmp414,_tmp414,_tmp414 + 12},{_tmp415,_tmp415,_tmp415 + 18},{_tmp416,_tmp416,_tmp416 + 17},{_tmp417,_tmp417,_tmp417 + 12},{_tmp418,_tmp418,_tmp418 + 16},{_tmp419,_tmp419,_tmp419 + 11},{_tmp41A,_tmp41A,_tmp41A + 10},{_tmp41B,_tmp41B,_tmp41B + 14},{_tmp41C,_tmp41C,_tmp41C + 15},{_tmp41D,_tmp41D,_tmp41D + 20},{_tmp41E,_tmp41E,_tmp41E + 27},{_tmp41F,_tmp41F,_tmp41F + 10},{_tmp420,_tmp420,_tmp420 + 18},{_tmp421,_tmp421,_tmp421 + 21},{_tmp422,_tmp422,_tmp422 + 19},{_tmp423,_tmp423,_tmp423 + 16},{_tmp424,_tmp424,_tmp424 + 20},{_tmp425,_tmp425,_tmp425 + 15},{_tmp426,_tmp426,_tmp426 + 20},{_tmp427,_tmp427,_tmp427 + 15},{_tmp428,_tmp428,_tmp428 + 12},{_tmp429,_tmp429,_tmp429 + 20},{_tmp42A,_tmp42A,_tmp42A + 19},{_tmp42B,_tmp42B,_tmp42B + 20},{_tmp42C,_tmp42C,_tmp42C + 21},{_tmp42D,_tmp42D,_tmp42D + 21},{_tmp42E,_tmp42E,_tmp42E + 12},{_tmp42F,_tmp42F,_tmp42F + 17},{_tmp430,_tmp430,_tmp430 + 19},{_tmp431,_tmp431,_tmp431 + 14},{_tmp432,_tmp432,_tmp432 + 17},{_tmp433,_tmp433,_tmp433 + 23},{_tmp434,_tmp434,_tmp434 + 13},{_tmp435,_tmp435,_tmp435 + 14},{_tmp436,_tmp436,_tmp436 + 16},{_tmp437,_tmp437,_tmp437 + 16},{_tmp438,_tmp438,_tmp438 + 8},{_tmp439,_tmp439,_tmp439 + 19},{_tmp43A,_tmp43A,_tmp43A + 20},{_tmp43B,_tmp43B,_tmp43B + 14},{_tmp43C,_tmp43C,_tmp43C + 19},{_tmp43D,_tmp43D,_tmp43D + 20},{_tmp43E,_tmp43E,_tmp43E + 11},{_tmp43F,_tmp43F,_tmp43F + 22},{_tmp440,_tmp440,_tmp440 + 20},{_tmp441,_tmp441,_tmp441 + 23},{_tmp442,_tmp442,_tmp442 + 20},{_tmp443,_tmp443,_tmp443 + 22},{_tmp444,_tmp444,_tmp444 + 23},{_tmp445,_tmp445,_tmp445 + 24},{_tmp446,_tmp446,_tmp446 + 24},{_tmp447,_tmp447,_tmp447 + 15},{_tmp448,_tmp448,_tmp448 + 20},{_tmp449,_tmp449,_tmp449 + 22},{_tmp44A,_tmp44A,_tmp44A + 17},{_tmp44B,_tmp44B,_tmp44B + 20},{_tmp44C,_tmp44C,_tmp44C + 26},{_tmp44D,_tmp44D,_tmp44D + 16},{_tmp44E,_tmp44E,_tmp44E + 17},{_tmp44F,_tmp44F,_tmp44F + 15},{_tmp450,_tmp450,_tmp450 + 19},{_tmp451,_tmp451,_tmp451 + 17},{_tmp452,_tmp452,_tmp452 + 19},{_tmp453,_tmp453,_tmp453 + 25},{_tmp454,_tmp454,_tmp454 + 26},{_tmp455,_tmp455,_tmp455 + 9},{_tmp456,_tmp456,_tmp456 + 20},{_tmp457,_tmp457,_tmp457 + 17},{_tmp458,_tmp458,_tmp458 + 18},{_tmp459,_tmp459,_tmp459 + 11},{_tmp45A,_tmp45A,_tmp45A + 12}};
# 1661
static short Cyc_yyr1[525]={0,145,146,146,146,146,146,146,146,146,146,146,147,148,149,149,150,150,150,151,151,151,152,152,153,153,153,153,154,154,155,156,157,158,159,159,159,159,159,159,159,160,160,161,161,162,162,162,162,162,162,162,162,162,162,163,163,163,163,163,163,163,164,164,165,166,166,167,167,167,167,168,168,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,170,171,171,171,172,172,172,173,173,174,174,174,175,175,175,175,175,176,176,177,177,178,178,179,179,180,181,181,182,182,183,184,184,184,184,184,184,185,185,185,185,185,185,186,187,187,188,188,188,188,189,189,190,190,191,191,191,192,192,193,193,193,193,194,194,194,195,195,196,196,197,197,198,198,198,198,198,198,198,198,198,198,199,199,199,199,199,199,199,199,199,199,199,200,200,201,202,202,203,203,203,203,203,203,203,203,204,204,204,205,205,206,206,206,207,207,207,208,208,209,209,209,209,210,210,211,211,212,212,213,213,214,214,215,215,216,216,216,216,217,217,218,218,219,219,219,220,221,221,222,222,223,223,223,223,223,224,224,224,224,225,226,226,227,227,228,228,229,229,229,229,229,230,230,231,231,231,232,232,232,232,232,232,232,232,232,232,232,233,233,233,233,233,233,233,234,235,235,236,236,237,237,237,237,237,237,237,237,238,238,238,238,238,238,239,239,239,239,239,239,240,240,240,240,240,240,240,240,240,240,240,240,240,240,241,241,241,241,241,241,241,241,242,243,243,244,244,245,245,246,246,247,247,248,248,249,249,249,250,250,250,250,250,251,251,251,252,252,252,253,253,253,253,254,254,255,255,255,255,255,255,256,257,258,258,258,258,258,258,258,258,258,258,258,258,258,258,258,258,259,259,259,260,260,261,261,262,262,262,263,263,264,264,265,265,265,266,266,266,266,266,266,266,266,266,266,266,267,267,267,267,267,267,267,268,269,269,270,270,271,271,272,272,273,273,274,274,274,275,275,275,275,275,276,276,276,277,277,277,278,278,278,278,279,279,280,280,280,280,280,280,280,280,280,280,280,280,280,280,280,280,280,280,280,281,281,281,282,282,282,282,282,282,282,282,282,282,283,283,283,283,284,284,284,284,284,284,284,284,284,284,285,286,286,287,287,287,287,287,288,288,289,289,290,290,291,291,292,292};
# 1717
static short Cyc_yyr2[525]={0,1,2,3,5,3,5,5,6,3,3,0,2,1,4,3,1,2,3,1,1,1,1,0,3,4,4,5,3,4,2,1,2,1,2,3,5,3,7,4,9,0,1,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,8,1,2,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,4,4,1,4,6,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,2,2,0,3,4,0,4,0,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,6,7,4,4,2,1,1,3,4,4,5,6,7,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,6,5,7,6,4,2,1,1,1,1,1,1,5,3,1,2,2,3,1,2,3,4,1,2,1,2,5,7,7,5,8,6,0,3,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,1,1,1,3,1,1,3,6,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,4,6,9,11,4,6,6,4,1,1,1,1,1,4,3,4,3,3,2,2,6,7,1,1,3,3,1,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1773
static short Cyc_yydefact[1060]={0,21,55,56,57,58,60,73,76,77,78,79,80,81,82,83,97,98,99,115,116,51,0,0,61,0,0,153,90,94,0,0,0,0,0,0,42,515,219,517,516,518,0,0,74,0,205,205,204,1,0,0,19,0,0,20,0,0,45,53,47,71,49,84,85,0,86,0,41,164,0,189,192,87,168,113,59,58,52,0,101,514,0,515,510,511,512,513,113,0,379,0,0,0,0,242,0,381,382,30,32,0,0,0,0,0,0,0,154,0,0,0,0,0,0,0,202,203,0,2,0,0,0,0,0,34,0,121,122,124,46,54,48,50,117,519,520,113,113,41,43,41,0,23,0,221,0,177,165,190,0,196,197,200,201,0,199,198,210,192,0,72,59,105,0,103,0,515,390,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,499,500,479,0,0,0,0,0,482,480,481,0,407,409,423,431,433,435,437,439,441,444,449,452,455,459,0,461,483,498,497,0,391,389,37,0,0,113,0,0,0,131,127,129,261,263,0,0,9,10,0,0,113,521,522,220,96,0,0,169,75,240,0,237,0,0,3,0,5,0,0,0,35,0,0,41,23,0,118,119,144,112,0,151,0,0,0,0,0,0,0,0,0,0,0,0,515,292,294,0,302,296,0,300,284,285,286,0,287,288,289,0,44,23,124,22,24,268,0,227,243,0,0,223,221,0,207,0,0,0,212,62,211,193,0,106,102,0,0,383,0,0,404,0,0,0,0,257,400,405,0,402,0,469,0,425,459,0,426,427,0,0,0,0,0,0,0,0,0,462,463,41,0,0,0,465,466,464,0,380,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,413,414,415,416,417,418,419,420,421,422,412,0,467,0,489,490,0,0,0,502,0,113,397,398,0,395,0,244,0,0,0,0,264,0,235,132,137,133,135,128,130,221,0,270,262,271,524,523,0,89,93,0,91,95,111,68,67,0,65,170,221,239,166,270,241,178,179,0,88,206,13,0,31,0,33,0,39,0,0,123,125,246,245,23,25,108,120,0,0,0,139,140,147,0,113,113,159,0,0,0,0,0,515,0,41,0,331,332,333,0,0,335,0,0,41,0,303,297,124,301,295,293,26,0,176,228,0,0,0,234,222,229,147,0,0,0,223,175,209,208,171,207,0,0,213,63,114,107,430,104,100,393,394,260,0,401,256,258,388,0,0,0,0,247,252,0,0,0,0,0,0,0,0,0,0,0,0,0,501,508,0,507,408,432,0,434,436,438,440,442,443,447,448,445,446,450,451,453,454,456,457,458,411,410,488,485,0,487,0,0,0,385,0,392,36,0,386,0,265,0,138,134,136,0,223,0,207,0,272,0,221,0,283,267,0,113,0,0,0,131,0,113,0,221,0,188,167,238,0,7,0,4,6,0,0,27,0,143,126,144,0,0,142,223,152,161,160,0,0,155,0,0,0,310,0,0,0,0,0,0,330,334,0,0,0,0,298,291,41,28,269,221,0,231,0,0,149,224,0,147,227,215,172,194,195,213,191,0,259,403,406,468,0,0,248,0,253,471,0,0,0,0,475,478,0,0,0,0,0,460,504,0,0,486,484,0,0,396,399,387,266,236,147,0,273,274,207,0,0,223,207,0,92,223,515,0,64,66,0,180,0,0,223,0,207,0,0,8,0,0,141,0,145,117,150,162,159,159,0,0,0,0,0,0,0,0,0,0,0,0,0,310,336,0,0,299,29,223,0,232,230,0,173,0,149,223,0,214,384,494,0,493,0,249,254,0,0,0,0,428,429,488,487,506,0,509,424,503,505,0,149,147,276,282,147,0,275,207,117,0,69,181,187,147,0,186,182,207,15,0,16,0,38,0,0,0,158,157,304,310,0,0,0,0,0,0,338,339,341,343,345,347,349,351,354,359,362,365,369,371,377,378,0,41,307,316,0,0,0,0,0,0,0,0,0,0,337,290,217,233,0,225,174,216,221,470,0,0,255,472,0,0,477,476,491,0,279,149,149,147,277,0,0,149,147,183,14,17,0,0,146,110,0,0,0,310,0,375,0,0,372,41,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,373,311,0,318,0,0,0,326,0,0,0,0,0,0,309,0,0,223,496,495,0,0,0,492,278,281,149,109,0,184,149,18,40,163,305,306,0,0,0,0,310,312,342,0,344,346,348,350,352,353,357,358,355,356,360,361,363,364,366,367,368,0,317,319,320,0,328,327,0,322,0,0,0,148,226,218,0,0,0,0,280,0,185,308,374,0,370,313,0,41,321,329,323,324,0,251,250,473,0,70,0,340,310,314,325,0,376,315,474,0,0,0};
# 1882
static short Cyc_yydefgoto[148]={1057,49,50,457,636,835,51,302,52,286,53,459,54,461,55,56,141,57,58,530,227,443,444,228,61,241,229,63,163,164,64,161,65,263,264,126,127,128,265,230,425,475,476,477,648,788,66,67,653,654,655,68,478,69,448,70,71,158,159,72,116,526,317,692,609,73,610,520,683,512,516,517,419,310,249,94,95,550,467,551,331,332,333,231,303,304,611,431,289,290,291,292,293,294,769,295,296,851,852,853,854,855,856,857,858,859,860,861,862,863,864,865,866,334,410,411,335,336,337,297,198,397,199,535,200,201,202,203,204,205,206,207,208,209,210,211,212,213,796,214,566,567,215,216,75,836,242,435};
# 1900
static short Cyc_yypact[1060]={1340,- -32768,- -32768,- -32768,- -32768,- 72,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3350,230,1352,- -32768,179,- 20,- -32768,- 22,7,27,37,104,80,455,265,- -32768,- -32768,221,- -32768,- -32768,- -32768,231,606,283,234,275,275,- -32768,- -32768,285,2773,- -32768,377,527,- -32768,375,662,3350,3350,3350,- -32768,3350,- -32768,- -32768,685,- -32768,179,3270,187,62,136,766,- -32768,- -32768,331,370,403,- -32768,179,397,- -32768,3962,157,- -32768,- -32768,- -32768,- -32768,331,6131,- -32768,398,427,3962,423,426,434,- -32768,163,- -32768,- -32768,3498,3498,2773,2773,3498,6131,372,- -32768,138,450,563,471,138,3868,6131,- -32768,- -32768,2908,- -32768,2773,2908,2773,2908,99,- -32768,487,516,- -32768,3189,- -32768,- -32768,- -32768,- -32768,3868,- -32768,- -32768,331,77,1650,- -32768,3270,662,503,3498,3424,4482,- -32768,187,- -32768,555,- -32768,- -32768,- -32768,- -32768,590,- -32768,- -32768,- 2,766,3498,- -32768,- -32768,549,589,599,179,142,- -32768,3962,141,3018,6372,603,6131,6276,609,638,644,661,664,676,687,691,6420,6420,- -32768,- -32768,- -32768,2355,704,6468,6468,6468,- -32768,- -32768,- -32768,272,- -32768,- -32768,- 24,750,657,711,721,605,86,578,90,429,- -32768,1006,6468,115,- 29,- -32768,746,6639,763,- -32768,- -32768,776,6131,331,6639,767,3646,3868,3942,3868,697,- -32768,97,97,- -32768,- -32768,108,293,331,- -32768,- -32768,- -32768,- -32768,35,749,- -32768,- -32768,233,308,- -32768,774,778,- -32768,779,- -32768,783,219,784,- -32768,563,4579,3270,503,786,3868,- -32768,874,785,179,788,787,109,798,3997,801,803,792,793,4676,3997,263,805,802,- -32768,- -32768,809,1792,1792,662,1792,- -32768,- -32768,- -32768,816,- -32768,- -32768,- -32768,297,- -32768,503,791,- -32768,- -32768,806,118,817,- -32768,34,811,812,323,818,769,814,6131,3498,- -32768,827,- -32768,- -32768,118,179,- -32768,6131,828,- -32768,823,825,- -32768,138,6131,3962,460,- -32768,- -32768,- -32768,832,833,2355,- -32768,3868,- -32768,- -32768,4094,- -32768,864,6131,6131,6131,6131,6131,6131,3868,810,2355,- -32768,- -32768,1934,830,317,6131,- -32768,- -32768,- -32768,6131,- -32768,6468,6131,6468,6468,6468,6468,6468,6468,6468,6468,6468,6468,6468,6468,6468,6468,6468,6468,6468,6131,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,6131,- -32768,138,- -32768,- -32768,4773,138,6131,- -32768,836,331,- -32768,- -32768,831,839,3962,- -32768,436,3018,835,3498,- -32768,843,844,- -32768,3942,3942,3942,- -32768,- -32768,2982,4870,211,- -32768,335,- -32768,- -32768,34,- -32768,- -32768,3498,- -32768,- -32768,845,- -32768,841,842,848,- -32768,1136,- -32768,396,238,- -32768,- -32768,- -32768,3868,- -32768,- -32768,- -32768,2637,- -32768,2773,- -32768,2773,- -32768,865,847,- -32768,- -32768,- -32768,- -32768,503,- -32768,- -32768,- -32768,1061,6131,856,854,- -32768,- 10,678,331,331,768,6131,6131,853,863,6131,855,956,2218,872,- -32768,- -32768,- -32768,509,949,- -32768,4967,6131,2076,2492,- -32768,- -32768,3189,- -32768,- -32768,- -32768,- -32768,3498,- -32768,- -32768,3868,867,3720,- -32768,- -32768,862,927,34,875,3794,812,- -32768,- -32768,- -32768,- -32768,769,878,147,834,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3868,- -32768,- -32768,879,- -32768,- -32768,- -32768,- -32768,6600,880,889,908,- -32768,- -32768,734,4579,895,901,904,905,388,902,906,167,909,907,6324,- -32768,- -32768,910,913,- -32768,750,135,657,711,721,605,86,86,578,578,578,578,90,90,429,429,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,914,- -32768,74,3498,4385,- -32768,6675,- -32768,- -32768,919,- -32768,229,- -32768,3868,- -32768,- -32768,- -32768,915,812,916,769,898,335,3498,3572,5064,- -32768,- -32768,97,331,6228,918,35,3056,920,331,3498,3424,5161,- -32768,396,- -32768,928,- -32768,2773,- -32768,- -32768,923,946,- -32768,1122,- -32768,- -32768,874,936,6131,- -32768,812,- -32768,- -32768,- -32768,944,179,534,399,413,6131,840,432,3997,947,5258,5355,614,- -32768,- -32768,950,955,948,473,1792,- -32768,3270,- -32768,806,957,3498,- -32768,958,34,1002,- -32768,960,927,281,- -32768,- -32768,- -32768,- -32768,834,- -32768,974,- -32768,- -32768,- -32768,961,203,962,- -32768,4191,- -32768,- -32768,6131,1056,6131,6276,- -32768,- -32768,138,138,961,963,4385,- -32768,- -32768,6131,6131,- -32768,- -32768,118,751,- -32768,- -32768,- -32768,- -32768,- -32768,927,964,- -32768,- -32768,769,118,965,812,769,954,- -32768,812,966,967,- -32768,- -32768,970,- -32768,118,982,812,983,769,978,208,- -32768,6131,997,- -32768,6131,- -32768,3868,- -32768,992,82,768,3997,969,994,6564,993,972,3997,6131,5452,667,5549,693,5646,840,- -32768,1005,1007,- -32768,- -32768,812,235,- -32768,- -32768,999,- -32768,1014,1002,812,3868,- -32768,- -32768,- -32768,494,- -32768,6131,- -32768,- -32768,4579,1003,1009,1010,- -32768,864,1004,1011,- -32768,759,- -32768,- -32768,- -32768,- -32768,4385,1002,927,- -32768,- -32768,927,1012,- -32768,769,3868,1026,- -32768,- -32768,- -32768,927,1013,- -32768,- -32768,769,- -32768,1015,705,496,- -32768,1017,1016,3868,- -32768,- -32768,1112,840,1022,6516,1033,2492,6468,1030,- -32768,- 21,- -32768,1070,1024,745,797,172,804,498,500,- -32768,- -32768,- -32768,- -32768,1072,6468,1934,- -32768,- -32768,497,3997,545,5743,3997,574,5840,5937,706,1043,- -32768,- -32768,- -32768,- -32768,6131,1044,- -32768,- -32768,957,- -32768,303,255,- -32768,- -32768,3868,1139,- -32768,- -32768,- -32768,4288,- -32768,1002,1002,927,- -32768,1048,1050,1002,927,- -32768,- -32768,- -32768,372,1060,- -32768,- -32768,581,3997,1059,840,2355,- -32768,3868,1055,- -32768,1508,6468,6131,6468,6468,6468,6468,6468,6468,6468,6468,6468,6468,6468,6468,6468,6468,6468,6468,6468,6131,- -32768,- -32768,1064,- -32768,3997,3997,582,- -32768,3997,3997,584,3997,597,6034,- -32768,1057,34,812,- -32768,- -32768,2492,1058,1062,- -32768,- -32768,- -32768,1002,- -32768,1082,- -32768,1002,- -32768,- -32768,- -32768,- -32768,- -32768,1073,1071,1069,6468,840,- -32768,750,292,657,711,711,605,86,86,578,578,578,578,90,90,429,429,- -32768,- -32768,- -32768,326,- -32768,- -32768,- -32768,3997,- -32768,- -32768,3997,- -32768,3997,3997,617,- -32768,- -32768,- -32768,1078,770,1074,3868,- -32768,1075,- -32768,- -32768,961,203,- -32768,- -32768,6131,1508,- -32768,- -32768,- -32768,- -32768,3997,- -32768,- -32768,- -32768,1077,- -32768,543,- -32768,840,- -32768,- -32768,1079,- -32768,- -32768,- -32768,1204,1206,- -32768};
# 2009
static short Cyc_yypgoto[148]={- -32768,536,- -32768,- -32768,- -32768,295,- -32768,- 234,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,45,- -32768,- 98,18,- -32768,- -32768,0,583,- -32768,40,- 211,1097,13,- -32768,- -32768,- 136,- -32768,587,1177,- 727,- -32768,- -32768,- -32768,953,952,668,435,- -32768,- -32768,568,133,- 684,- -32768,- -32768,139,- -32768,- -32768,113,- 192,1147,- 367,420,- -32768,1063,- -32768,- -32768,1171,- 107,- -32768,528,- 101,- 157,- 122,- 472,256,538,544,- 427,- 478,- 112,- 397,- 118,- -32768,- 222,- 169,- 563,- 168,- -32768,896,- 160,- 97,- 32,- 81,- 340,100,- -32768,- -32768,131,- 267,- -32768,- 588,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,14,1008,- -32768,684,815,- -32768,102,669,- -32768,- 159,- 273,- 165,- 352,- 350,- 366,861,- 355,- 361,- 328,- 337,- 327,- 167,376,466,881,206,- -32768,- 308,- -32768,513,360,19,30,- 320,- 68};
# 2031
static short Cyc_yytable[6815]={59,318,248,572,232,233,344,619,236,539,345,575,576,62,569,341,574,423,571,502,503,59,505,309,361,362,363,311,470,358,324,261,840,722,62,76,680,96,466,78,60,581,582,111,308,398,577,578,579,580,687,59,441,583,584,447,633,540,59,59,59,60,59,646,62,508,420,366,59,147,928,62,62,62,405,62,130,131,132,590,133,62,632,593,136,100,142,651,652,614,562,60,684,513,592,137,168,907,60,60,60,101,60,59,59,406,888,219,60,614,38,367,632,140,929,677,62,62,59,647,59,59,59,59,633,316,136,727,320,59,102,62,902,62,62,62,62,238,729,59,442,59,62,60,60,103,38,142,147,430,515,810,62,514,62,104,112,288,60,142,60,60,60,60,534,436,450,37,438,60,129,534,374,375,140,552,40,759,546,60,547,60,432,325,287,533,298,43,522,881,670,197,559,432,561,364,399,400,401,143,160,643,- 156,432,256,106,720,268,237,376,377,423,423,423,37,585,586,587,529,972,973,433,257,40,244,977,380,105,422,437,381,409,433,434,484,641,510,485,409,509,402,424,433,239,434,403,404,169,710,400,401,240,532,326,626,300,364,920,936,937,262,59,169,821,718,170,452,824,327,46,47,48,299,690,62,- 243,830,515,- 243,142,170,626,223,434,37,452,59,59,224,59,1028,359,402,40,1030,938,939,711,404,62,62,60,62,42,288,288,298,288,239,795,144,884,145,37,108,534,240,531,146,889,603,39,40,41,414,109,60,60,834,60,702,287,287,984,287,37,427,462,37,989,620,463,428,37,40,541,39,40,41,79,766,39,40,41,420,726,110,59,446,114,74,885,515,446,428,434,46,47,48,428,62,434,300,489,758,288,364,693,797,496,495,497,80,98,968,99,511,113,498,248,807,808,469,364,115,890,715,60,365,307,1035,504,287,74,118,781,674,- 221,239,966,- 221,74,676,364,364,507,240,74,364,439,688,1036,422,422,422,135,599,138,552,453,74,618,124,744,454,424,424,424,364,165,359,307,98,564,521,420,625,364,631,1051,744,42,557,98,160,1037,359,59,615,59,616,59,1055,74,74,627,617,135,452,570,62,74,62,642,62,19,20,37,74,800,74,74,74,74,839,39,40,41,534,- 12,59,149,452,1023,735,120,121,60,452,60,59,60,74,62,59,731,594,748,142,364,162,749,42,62,707,734,166,62,142,628,364,629,142,217,515,764,165,630,747,98,60,98,218,801,364,664,97,684,805,765,60,220,806,691,60,672,552,221,339,140,342,342,738,364,600,783,222,364,770,812,355,356,382,721,791,993,994,342,342,342,383,384,967,996,997,243,990,98,995,894,992,784,542,733,98,656,657,119,342,660,329,330,665,800,364,97,746,245,534,780,673,671,949,1002,1003,258,97,451,998,999,1000,1001,724,963,618,1004,1005,364,364,74,891,301,915,950,892,818,451,667,481,822,364,486,631,944,942,675,59,259,943,234,235,945,946,832,627,122,123,801,74,62,429,682,762,813,251,763,252,253,254,255,988,452,378,379,452,819,364,449,37,1054,321,952,59,892,59,171,60,40,828,800,314,165,97,926,97,62,479,62,43,925,288,98,142,372,373,364,46,47,48,342,956,948,453,364,691,364,342,981,1013,322,1016,37,60,797,60,315,906,287,364,298,40,323,42,1018,267,269,911,534,340,918,97,43,776,801,346,364,969,97,364,46,47,48,342,1042,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,673,985,347,986,773,775,37,965,348,1050,136,98,651,652,98,40,1006,1007,1008,125,867,247,37,250,875,349,43,364,350,37,39,40,41,369,46,47,48,39,40,41,351,266,473,134,782,451,515,1024,451,415,878,352,247,364,313,353,74,790,74,1034,74,427,913,961,440,914,364,428,360,46,47,48,451,150,151,152,153,154,451,368,155,156,157,97,524,525,429,81,370,342,16,17,18,700,701,837,606,607,608,371,816,767,768,844,407,449,1046,412,59,871,814,815,872,874,445,877,1049,880,900,901,413,62,932,933,934,935,288,940,941,1044,364,417,479,455,421,464,426,456,458,893,842,843,460,480,98,471,482,491,60,492,493,483,260,287,37,84,186,187,85,86,87,511,487,40,97,490,59,97,468,499,500,266,136,501,354,506,518,190,342,62,519,434,42,523,288,527,537,536,538,903,197,544,904,545,366,563,597,98,595,598,602,909,604,621,605,622,60,623,624,639,640,287,951,644,645,955,954,658,37,958,960,659,528,661,451,662,39,40,41,666,668,678,635,596,637,74,638,681,473,646,685,451,474,689,451,697,46,47,48,698,694,468,699,761,553,554,555,556,982,558,703,704,342,359,705,706,713,708,565,732,991,709,568,717,712,716,59,974,725,719,728,730,978,742,479,745,752,754,1009,62,755,1011,1012,588,288,1014,1015,97,1017,385,757,479,1020,760,479,589,649,650,777,1025,565,771,778,779,307,787,60,785,794,714,803,287,342,789,798,823,825,845,809,817,820,870,826,342,247,827,613,386,387,388,389,390,391,392,393,394,395,829,831,833,97,135,1038,247,838,1039,841,1040,1041,846,247,869,882,886,883,887,98,396,895,898,897,908,342,896,912,917,899,905,910,919,921,1052,916,7,8,9,10,11,12,13,14,15,16,17,18,19,20,924,927,930,931,947,962,970,964,37,565,975,247,565,976,753,22,39,40,41,42,980,983,247,987,1010,1021,1026,27,473,1027,1029,247,1033,28,29,1031,46,47,48,1032,1043,32,1045,1047,33,1053,1058,1056,1059,743,739,979,246,107,465,756,34,35,472,148,117,793,1022,468,319,923,792,786,342,37,543,696,601,573,416,868,560,39,40,41,305,1048,0,37,0,0,342,0,0,473,38,39,40,41,42,0,307,46,47,48,0,0,0,446,342,44,45,468,0,428,0,46,47,48,0,0,135,0,0,0,0,0,97,0,0,0,247,0,737,0,0,0,0,741,0,421,0,0,0,247,0,751,0,0,0,0,342,0,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,0,0,0,0,565,0,0,0,0,0,0,0,0,0,0,0,0,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,342,21,0,0,0,0,0,468,0,0,802,0,804,0,22,0,23,0,0,0,468,0,24,811,25,26,27,81,0,0,0,0,28,29,- 41,0,0,0,30,31,32,0,0,33,0,0,0,0,0,342,0,0,0,0,0,34,35,36,0,0,0,0,0,0,266,0,0,0,0,0,0,0,0,0,0,0,0,0,0,82,0,0,37,0,0,0,0,0,0,38,39,40,41,42,83,84,0,247,85,86,87,0,43,40,44,45,0,468,88,0,46,47,48,0,89,0,90,91,0,0,0,468,92,0,0,0,0,0,93,266,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,247,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,767,768,21,172,173,270,0,271,272,273,274,275,276,277,278,22,81,23,174,279,0,0,175,24,280,0,0,27,176,177,178,179,0,28,29,0,180,0,281,0,468,32,181,182,33,183,0,0,0,0,0,0,0,0,0,0,34,35,36,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,282,84,186,187,85,86,87,38,39,40,41,42,188,283,139,- 310,0,0,0,0,189,0,44,285,0,0,0,0,191,0,0,192,0,0,193,194,0,0,195,196,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,172,173,270,0,271,272,273,274,275,276,277,278,22,81,23,174,279,0,0,175,24,280,0,0,27,176,177,178,179,0,28,29,0,180,0,281,0,0,32,181,182,33,183,0,0,0,0,0,0,0,0,0,0,34,35,36,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,282,84,186,187,85,86,87,38,39,40,41,42,188,283,139,284,0,0,0,0,189,0,44,285,0,0,0,0,191,0,0,192,0,0,193,194,0,0,195,196,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,172,173,270,0,271,272,273,274,275,276,277,278,22,81,23,174,279,0,0,175,24,280,0,0,27,176,177,178,179,0,28,29,- 41,180,0,281,0,0,32,181,182,33,183,0,0,0,0,0,0,0,0,0,0,34,35,36,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,282,84,186,187,85,86,87,38,39,40,41,42,188,283,139,0,0,0,0,0,189,0,44,285,0,0,0,0,191,0,0,192,0,0,193,194,0,0,195,196,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,172,173,270,0,271,272,273,274,275,276,277,278,22,81,23,174,279,0,0,175,24,280,0,0,27,176,177,178,179,0,28,29,0,180,0,281,0,0,32,181,182,33,183,0,0,0,0,0,0,0,0,0,0,34,35,36,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,282,84,186,187,85,86,87,38,39,40,41,42,188,283,139,0,0,0,0,0,189,0,44,285,0,0,0,0,191,0,0,192,0,0,193,194,0,0,195,196,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,172,173,270,0,271,272,273,274,275,276,277,278,22,81,23,174,279,0,0,175,24,280,0,0,27,176,177,178,179,0,28,29,0,180,0,281,0,0,32,181,182,33,183,0,0,0,0,0,0,0,0,0,0,34,35,36,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,488,84,186,187,85,86,87,38,39,40,41,42,188,283,139,0,0,0,0,0,189,0,44,285,0,0,0,0,191,0,0,192,0,0,193,194,0,0,195,196,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,172,173,0,0,0,0,0,0,0,0,0,0,22,81,23,174,0,0,0,175,24,0,0,0,27,176,177,178,179,0,28,29,0,180,0,0,0,0,32,181,182,33,183,0,0,0,0,0,0,0,0,0,0,34,35,36,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,38,39,40,41,42,188,663,0,0,0,0,0,0,189,0,44,285,0,0,0,0,191,0,0,192,0,0,193,194,0,0,195,196,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,172,173,0,0,0,0,0,0,0,0,0,0,22,81,0,174,0,0,0,175,0,0,0,0,27,176,177,178,179,0,28,29,0,180,0,0,0,0,32,181,182,33,183,0,0,0,0,0,0,0,0,0,0,34,35,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,38,39,40,41,42,188,0,357,0,0,0,0,0,189,0,44,285,0,0,0,0,191,0,0,192,0,0,193,194,0,0,195,196,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,172,173,0,0,0,0,0,0,0,0,0,0,22,81,0,174,0,0,0,175,0,0,0,0,27,176,177,178,179,0,28,29,0,180,0,0,0,0,32,181,182,33,183,0,0,0,0,0,0,0,0,0,0,34,35,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,38,39,40,41,42,188,0,0,0,0,0,0,0,189,0,44,285,0,0,0,0,191,0,0,192,0,0,193,194,0,0,195,196,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,634,0,24,0,25,26,27,0,0,0,0,0,28,29,- 41,0,0,0,30,31,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,36,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,0,0,0,0,0,0,38,39,40,41,42,0,0,0,- 11,0,0,0,0,43,0,44,45,0,0,0,0,46,47,48,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,28,29,- 41,0,0,0,30,31,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,36,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,0,0,0,0,0,0,38,39,40,41,42,0,0,0,- 11,0,0,0,0,43,0,44,45,0,0,0,0,46,47,48,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,28,29,- 41,0,0,0,30,31,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,36,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,37,0,0,0,0,0,22,38,39,40,41,42,0,0,0,- 11,0,0,27,0,43,0,44,45,28,29,0,0,46,47,48,0,32,0,0,33,0,0,0,0,0,0,0,81,0,0,0,34,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,305,0,0,0,0,0,0,0,0,22,38,39,0,41,42,0,307,0,0,0,0,27,0,427,82,44,45,28,29,428,0,46,47,48,0,32,328,0,33,167,84,0,0,85,86,87,0,0,40,0,34,35,0,88,0,0,0,0,0,89,0,90,91,0,329,330,0,92,0,0,0,0,0,93,0,0,0,37,0,0,0,0,0,0,38,39,40,41,42,0,0,0,0,0,0,0,0,473,0,44,45,0,0,0,0,46,47,48,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,0,0,27,0,0,0,0,0,28,29,- 41,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,36,0,0,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,38,39,0,41,42,0,22,139,23,0,260,0,0,0,24,44,45,0,27,0,0,0,0,0,28,29,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,36,0,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,38,39,0,41,42,22,0,139,0,0,0,0,0,24,0,44,45,27,0,0,0,0,0,28,29,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,38,39,0,41,42,0,0,0,0,0,0,27,0,0,0,44,45,28,29,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,305,0,0,306,0,0,0,0,0,22,38,39,0,41,42,0,307,0,0,0,0,27,0,0,0,44,45,28,29,0,0,225,0,0,0,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,38,39,0,41,42,0,0,226,0,0,0,27,0,0,0,44,45,28,29,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,305,0,0,0,0,0,0,0,0,22,38,39,0,41,42,0,307,0,0,0,0,27,0,0,0,44,45,28,29,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,38,39,0,41,42,0,0,0,418,0,0,27,0,0,0,44,45,28,29,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,38,39,0,41,42,0,0,0,679,0,0,27,0,0,0,44,45,28,29,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,686,0,0,0,0,0,0,0,0,22,38,39,0,41,42,0,0,0,0,0,0,27,0,0,0,44,45,28,29,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,38,39,0,41,42,0,0,0,0,0,0,27,0,0,0,44,45,28,29,0,81,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,172,173,270,0,271,272,273,274,275,276,277,278,0,81,0,174,279,0,0,175,0,280,0,0,0,176,177,178,179,0,82,0,38,180,0,281,42,0,0,181,182,0,183,0,0,167,84,44,45,85,86,87,0,0,40,0,0,184,185,88,0,0,0,0,0,89,0,90,91,0,0,0,0,92,0,0,0,0,0,93,488,84,186,187,85,86,87,0,0,40,0,0,188,283,139,0,0,172,173,0,189,0,0,190,548,0,0,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,0,343,549,0,172,173,0,189,0,0,190,0,329,330,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,0,343,799,0,172,173,0,189,0,0,190,0,329,330,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,0,343,971,0,172,173,0,189,0,0,190,0,329,330,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,0,343,0,0,172,173,0,189,0,0,190,0,329,330,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,0,0,0,0,172,173,0,189,0,0,190,0,0,0,312,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,0,343,0,0,172,173,0,189,0,0,190,0,0,0,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,494,0,0,0,172,173,0,189,0,0,190,0,0,0,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,0,0,0,0,172,173,0,189,591,0,190,0,0,0,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,0,0,0,0,172,173,0,189,0,0,190,0,0,0,612,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,0,0,0,0,172,173,0,189,669,0,190,0,0,0,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,0,0,0,0,172,173,0,189,0,0,190,0,0,0,736,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,0,0,0,0,172,173,0,189,0,0,190,0,0,0,750,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,772,0,0,0,172,173,0,189,0,0,190,0,0,0,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,774,0,0,0,172,173,0,189,0,0,190,0,0,0,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,0,0,0,0,172,173,0,189,873,0,190,0,0,0,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,0,0,0,0,172,173,0,189,876,0,190,0,0,0,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,879,0,0,0,172,173,0,189,0,0,190,0,0,0,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,0,0,0,0,172,173,0,189,953,0,190,0,0,0,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,0,0,0,0,172,173,0,189,957,0,190,0,0,0,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,0,0,0,0,172,173,0,189,959,0,190,0,0,0,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,0,0,0,0,172,173,0,189,1019,0,190,0,0,0,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,0,0,0,0,172,173,0,189,0,0,190,0,0,0,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,172,173,0,0,0,0,0,0,0,184,185,0,0,81,0,0,0,0,0,0,0,0,0,0,0,176,177,178,179,0,0,0,740,84,186,187,85,86,87,181,182,40,183,0,188,0,0,0,172,173,0,0,189,0,0,190,0,184,185,0,191,81,0,192,0,0,193,194,0,0,195,196,0,176,177,178,179,0,0,0,37,84,186,187,85,86,87,181,182,40,183,0,188,0,343,0,172,173,0,0,189,0,0,190,0,184,185,0,191,81,0,192,0,0,193,194,0,0,195,196,0,176,177,178,179,0,0,0,37,84,186,187,85,86,87,181,182,40,183,0,188,0,714,0,172,173,0,0,189,0,0,190,0,184,185,0,191,81,0,192,0,0,193,194,0,0,195,196,0,176,177,178,179,0,0,0,37,84,186,187,85,86,87,181,182,40,183,0,188,0,0,0,172,173,0,0,338,0,0,190,0,184,185,0,191,81,0,192,0,0,193,194,0,0,195,196,0,176,177,178,179,0,0,0,37,84,186,187,85,86,87,181,182,40,183,0,188,0,0,0,172,173,0,0,354,0,0,190,0,184,185,0,191,81,0,192,0,0,193,194,0,0,195,196,0,176,177,178,179,0,0,0,37,84,186,187,85,86,87,181,182,40,183,0,188,0,0,0,847,848,0,0,189,0,0,190,0,184,185,0,191,81,0,192,0,0,193,194,0,0,195,196,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,0,0,0,0,81,0,0,922,0,0,190,0,0,0,0,191,0,0,192,0,82,193,194,0,0,195,196,0,0,0,0,0,0,0,0,167,84,0,0,85,86,87,0,81,40,0,0,0,0,88,0,0,0,0,0,849,82,90,91,0,0,0,0,92,0,0,850,0,695,93,194,167,84,195,196,85,86,87,0,81,40,0,0,0,0,88,0,0,0,0,0,89,0,90,91,82,329,330,0,92,0,0,0,0,0,93,0,408,0,0,167,84,0,0,85,86,87,0,0,40,0,0,0,0,88,0,0,0,0,0,89,82,90,91,0,0,0,0,92,0,0,0,0,723,93,0,167,84,0,0,85,86,87,0,0,40,0,0,0,0,88,0,0,0,0,0,89,0,90,91,0,0,0,0,92,0,0,0,0,0,93};
# 2716
static short Cyc_yycheck[6815]={0,158,114,369,101,102,175,434,105,329,175,372,373,0,366,174,371,228,368,286,287,21,289,145,191,192,193,145,262,189,166,129,759,596,21,107,514,23,260,21,0,378,379,43,145,212,374,375,376,377,522,51,17,380,381,247,453,330,58,59,60,21,62,73,51,299,226,91,68,69,91,58,59,60,103,62,58,59,60,399,62,68,449,403,65,105,68,5,6,429,357,51,519,59,402,65,82,824,58,59,60,123,62,103,104,134,790,93,68,449,112,135,479,68,135,512,103,104,118,129,120,121,122,123,521,127,107,605,160,129,123,118,816,120,121,122,123,107,610,139,105,141,129,103,104,118,112,129,148,230,307,714,139,119,141,118,43,139,118,141,120,121,122,123,323,233,247,105,236,129,57,330,86,87,129,343,114,649,338,139,340,141,85,169,139,321,141,125,310,777,498,89,352,85,354,121,81,82,83,68,123,474,120,85,105,125,132,130,106,123,124,422,423,424,105,382,383,384,315,903,904,124,123,114,111,909,136,123,228,121,140,217,124,136,125,469,304,128,224,121,125,228,124,105,136,130,131,105,81,82,83,113,320,112,446,142,121,845,86,87,129,261,105,735,129,123,247,739,127,133,134,135,141,126,261,118,748,434,121,261,123,473,119,136,105,266,286,287,125,289,974,189,125,114,978,123,124,130,131,286,287,261,289,116,286,287,261,289,105,106,123,783,125,105,49,474,113,317,131,791,417,113,114,115,222,104,286,287,120,289,552,286,287,921,289,105,125,118,105,927,437,122,131,105,114,331,113,114,115,119,658,113,114,115,514,126,125,357,125,125,0,126,519,125,131,136,133,134,135,131,357,136,259,273,647,357,121,537,698,279,278,118,22,23,129,25,105,104,125,501,710,711,261,121,119,792,563,357,126,118,988,288,357,43,119,672,504,126,105,106,129,51,509,121,121,118,113,57,121,126,527,129,422,423,424,65,412,67,596,121,70,431,57,625,126,422,423,424,121,79,338,118,82,126,121,605,446,121,448,1037,642,116,350,93,123,129,354,457,123,459,125,461,1050,103,104,446,131,107,449,367,457,111,459,473,461,20,21,105,118,701,120,121,122,123,757,113,114,115,647,119,490,71,473,965,616,118,119,457,479,459,500,461,142,490,504,612,404,629,490,121,107,629,116,500,126,616,119,504,500,123,121,125,504,125,681,126,166,131,629,169,490,171,105,701,121,490,23,964,707,126,500,118,707,530,504,500,714,121,172,504,174,175,620,121,118,677,122,121,126,718,184,185,133,595,686,931,932,191,192,193,141,142,892,934,935,125,928,217,933,801,930,678,122,615,224,483,484,51,212,487,130,131,490,815,121,82,628,126,757,126,500,499,869,940,941,118,93,247,936,937,938,939,598,886,614,942,943,121,121,259,126,118,126,126,130,732,266,118,268,736,121,271,632,133,136,504,636,121,140,103,104,141,142,750,625,118,119,815,288,636,230,518,118,721,118,121,120,121,122,123,927,642,84,85,645,733,121,247,105,126,121,126,672,130,674,88,636,114,746,901,125,321,169,850,171,672,266,674,125,849,672,331,674,88,89,121,133,134,135,323,126,868,121,121,691,121,330,126,126,120,126,105,672,1033,674,125,823,672,121,674,114,122,116,126,137,138,833,886,125,841,217,125,118,901,125,121,896,224,121,133,134,135,366,126,368,369,370,371,372,373,374,375,376,377,378,379,380,381,382,383,384,661,922,125,924,663,664,105,890,125,1037,752,412,5,6,415,114,944,945,946,118,767,114,105,115,118,125,125,121,125,105,113,114,115,137,133,134,135,113,114,115,125,134,125,119,674,446,964,968,449,223,118,125,145,121,146,125,457,685,459,987,461,125,118,118,238,121,121,131,125,133,134,135,473,68,69,70,71,72,479,90,75,76,77,331,76,77,427,38,138,474,17,18,19,120,121,754,422,423,424,139,728,22,23,764,119,446,1027,105,869,770,120,121,771,772,126,774,1036,776,120,121,105,869,138,139,88,89,869,84,85,120,121,125,473,120,227,112,229,120,120,798,762,763,120,119,545,120,119,105,869,118,118,125,122,869,105,106,107,108,109,110,111,105,125,114,412,125,927,415,260,125,129,264,914,125,125,120,126,128,563,927,129,136,116,126,927,132,124,120,124,817,849,120,820,121,91,126,126,598,123,121,126,829,120,119,121,125,927,126,121,105,124,927,873,118,121,876,875,125,105,878,879,119,314,129,625,30,113,114,115,118,42,125,457,407,459,636,461,136,125,73,126,642,129,126,645,126,133,134,135,121,132,343,105,654,346,347,348,349,919,351,126,121,647,922,121,121,120,126,360,132,929,126,364,121,126,126,1037,905,120,126,126,126,910,126,625,126,119,125,947,1037,105,952,953,385,1037,956,957,545,959,54,125,642,961,120,645,397,480,481,119,968,402,125,118,126,118,74,1037,120,105,119,25,1037,707,124,123,132,121,119,126,126,126,120,126,718,427,126,428,92,93,94,95,96,97,98,99,100,101,126,126,132,598,752,1013,446,118,1016,125,1018,1019,126,453,129,118,125,118,112,767,122,126,126,121,106,757,125,120,120,126,126,126,28,119,1042,126,8,9,10,11,12,13,14,15,16,17,18,19,20,21,125,129,90,137,90,120,25,121,105,498,120,501,501,121,636,37,113,114,115,116,118,120,512,126,118,126,126,49,125,125,106,521,121,55,56,120,133,134,135,126,120,63,126,126,66,126,0,126,0,624,621,914,113,34,259,645,78,79,264,70,47,691,964,552,159,847,686,681,850,105,332,545,415,370,224,767,353,113,114,115,102,1033,- 1,105,- 1,- 1,868,- 1,- 1,125,112,113,114,115,116,- 1,118,133,134,135,- 1,- 1,- 1,125,886,127,128,596,- 1,131,- 1,133,134,135,- 1,- 1,914,- 1,- 1,- 1,- 1,- 1,767,- 1,- 1,- 1,616,- 1,617,- 1,- 1,- 1,- 1,622,- 1,625,- 1,- 1,- 1,629,- 1,630,- 1,- 1,- 1,- 1,928,- 1,930,931,932,933,934,935,936,937,938,939,940,941,942,943,944,945,946,- 1,- 1,- 1,- 1,658,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,987,24,- 1,- 1,- 1,- 1,- 1,701,- 1,- 1,704,- 1,706,- 1,37,- 1,39,- 1,- 1,- 1,714,- 1,45,717,47,48,49,38,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,61,62,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,1036,- 1,- 1,- 1,- 1,- 1,78,79,80,- 1,- 1,- 1,- 1,- 1,- 1,759,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,- 1,- 1,105,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,105,106,- 1,792,109,110,111,- 1,125,114,127,128,- 1,801,119,- 1,133,134,135,- 1,125,- 1,127,128,- 1,- 1,- 1,815,133,- 1,- 1,- 1,- 1,- 1,139,824,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,841,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,50,51,52,53,- 1,55,56,- 1,58,- 1,60,- 1,901,63,64,65,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,- 1,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,50,51,52,53,- 1,55,56,- 1,58,- 1,60,- 1,- 1,63,64,65,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,- 1,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,50,51,52,53,- 1,55,56,57,58,- 1,60,- 1,- 1,63,64,65,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,- 1,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,- 1,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,50,51,52,53,- 1,55,56,- 1,58,- 1,60,- 1,- 1,63,64,65,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,- 1,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,- 1,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,50,51,52,53,- 1,55,56,- 1,58,- 1,60,- 1,- 1,63,64,65,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,- 1,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,- 1,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,- 1,- 1,44,45,- 1,- 1,- 1,49,50,51,52,53,- 1,55,56,- 1,58,- 1,- 1,- 1,- 1,63,64,65,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,112,113,114,115,116,117,118,- 1,- 1,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,- 1,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,50,51,52,53,- 1,55,56,- 1,58,- 1,- 1,- 1,- 1,63,64,65,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,112,113,114,115,116,117,- 1,119,- 1,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,- 1,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,50,51,52,53,- 1,55,56,- 1,58,- 1,- 1,- 1,- 1,63,64,65,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,112,113,114,115,116,117,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,- 1,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,43,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,61,62,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,- 1,- 1,- 1,120,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,134,135,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,61,62,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,- 1,- 1,- 1,120,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,134,135,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,61,62,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,- 1,- 1,- 1,- 1,- 1,37,112,113,114,115,116,- 1,- 1,- 1,120,- 1,- 1,49,- 1,125,- 1,127,128,55,56,- 1,- 1,133,134,135,- 1,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,78,79,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,102,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,- 1,115,116,- 1,118,- 1,- 1,- 1,- 1,49,- 1,125,90,127,128,55,56,131,- 1,133,134,135,- 1,63,102,- 1,66,105,106,- 1,- 1,109,110,111,- 1,- 1,114,- 1,78,79,- 1,119,- 1,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,130,131,- 1,133,- 1,- 1,- 1,- 1,- 1,139,- 1,- 1,- 1,105,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,134,135,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,112,113,- 1,115,116,- 1,37,119,39,- 1,122,- 1,- 1,- 1,45,127,128,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,- 1,115,116,37,- 1,119,- 1,- 1,- 1,- 1,- 1,45,- 1,127,128,49,- 1,- 1,- 1,- 1,- 1,55,56,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,- 1,115,116,- 1,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,127,128,55,56,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,102,- 1,- 1,105,- 1,- 1,- 1,- 1,- 1,37,112,113,- 1,115,116,- 1,118,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,127,128,55,56,- 1,- 1,59,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,- 1,115,116,- 1,- 1,119,- 1,- 1,- 1,49,- 1,- 1,- 1,127,128,55,56,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,102,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,- 1,115,116,- 1,118,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,127,128,55,56,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,- 1,115,116,- 1,- 1,- 1,120,- 1,- 1,49,- 1,- 1,- 1,127,128,55,56,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,- 1,115,116,- 1,- 1,- 1,120,- 1,- 1,49,- 1,- 1,- 1,127,128,55,56,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,102,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,- 1,115,116,- 1,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,127,128,55,56,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,- 1,115,116,- 1,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,127,128,55,56,- 1,38,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,38,- 1,40,41,- 1,- 1,44,- 1,46,- 1,- 1,- 1,50,51,52,53,- 1,90,- 1,112,58,- 1,60,116,- 1,- 1,64,65,- 1,67,- 1,- 1,105,106,127,128,109,110,111,- 1,- 1,114,- 1,- 1,82,83,119,- 1,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,- 1,- 1,- 1,- 1,- 1,139,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,118,119,- 1,- 1,25,26,- 1,125,- 1,- 1,128,32,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,119,120,- 1,25,26,- 1,125,- 1,- 1,128,- 1,130,131,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,119,120,- 1,25,26,- 1,125,- 1,- 1,128,- 1,130,131,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,119,120,- 1,25,26,- 1,125,- 1,- 1,128,- 1,130,131,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,119,- 1,- 1,25,26,- 1,125,- 1,- 1,128,- 1,130,131,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,- 1,- 1,128,- 1,- 1,- 1,132,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,119,- 1,- 1,25,26,- 1,125,- 1,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,118,- 1,- 1,- 1,25,26,- 1,125,- 1,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,126,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,- 1,- 1,128,- 1,- 1,- 1,132,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,126,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,- 1,- 1,128,- 1,- 1,- 1,132,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,- 1,- 1,128,- 1,- 1,- 1,132,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,118,- 1,- 1,- 1,25,26,- 1,125,- 1,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,118,- 1,- 1,- 1,25,26,- 1,125,- 1,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,126,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,126,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,118,- 1,- 1,- 1,25,26,- 1,125,- 1,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,126,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,126,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,126,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,126,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,- 1,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,51,52,53,- 1,- 1,- 1,105,106,107,108,109,110,111,64,65,114,67,- 1,117,- 1,- 1,- 1,25,26,- 1,- 1,125,- 1,- 1,128,- 1,82,83,- 1,133,38,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,- 1,50,51,52,53,- 1,- 1,- 1,105,106,107,108,109,110,111,64,65,114,67,- 1,117,- 1,119,- 1,25,26,- 1,- 1,125,- 1,- 1,128,- 1,82,83,- 1,133,38,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,- 1,50,51,52,53,- 1,- 1,- 1,105,106,107,108,109,110,111,64,65,114,67,- 1,117,- 1,119,- 1,25,26,- 1,- 1,125,- 1,- 1,128,- 1,82,83,- 1,133,38,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,- 1,50,51,52,53,- 1,- 1,- 1,105,106,107,108,109,110,111,64,65,114,67,- 1,117,- 1,- 1,- 1,25,26,- 1,- 1,125,- 1,- 1,128,- 1,82,83,- 1,133,38,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,- 1,50,51,52,53,- 1,- 1,- 1,105,106,107,108,109,110,111,64,65,114,67,- 1,117,- 1,- 1,- 1,25,26,- 1,- 1,125,- 1,- 1,128,- 1,82,83,- 1,133,38,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,- 1,50,51,52,53,- 1,- 1,- 1,105,106,107,108,109,110,111,64,65,114,67,- 1,117,- 1,- 1,- 1,25,26,- 1,- 1,125,- 1,- 1,128,- 1,82,83,- 1,133,38,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,38,- 1,- 1,125,- 1,- 1,128,- 1,- 1,- 1,- 1,133,- 1,- 1,136,- 1,90,139,140,- 1,- 1,143,144,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,- 1,- 1,109,110,111,- 1,38,114,- 1,- 1,- 1,- 1,119,- 1,- 1,- 1,- 1,- 1,125,90,127,128,- 1,- 1,- 1,- 1,133,- 1,- 1,136,- 1,102,139,140,105,106,143,144,109,110,111,- 1,38,114,- 1,- 1,- 1,- 1,119,- 1,- 1,- 1,- 1,- 1,125,- 1,127,128,90,130,131,- 1,133,- 1,- 1,- 1,- 1,- 1,139,- 1,102,- 1,- 1,105,106,- 1,- 1,109,110,111,- 1,- 1,114,- 1,- 1,- 1,- 1,119,- 1,- 1,- 1,- 1,- 1,125,90,127,128,- 1,- 1,- 1,- 1,133,- 1,- 1,- 1,- 1,102,139,- 1,105,106,- 1,- 1,109,110,111,- 1,- 1,114,- 1,- 1,- 1,- 1,119,- 1,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,- 1,- 1,- 1,- 1,- 1,139};char Cyc_Yystack_overflow[17]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
void Cyc_yyerror(struct _dyneither_ptr,int state,int token);
# 82 "cycbison.simple"
int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*yylval_ptr,struct Cyc_Yyltype*yylloc);struct _tuple27{unsigned int f1;struct _tuple0*f2;int f3;};struct _tuple28{struct _dyneither_ptr f1;void*f2;};static char _tmp519[8]="stdcall";static char _tmp51A[6]="cdecl";static char _tmp51B[9]="fastcall";static char _tmp51C[9]="noreturn";static char _tmp51D[6]="const";static char _tmp51E[8]="aligned";static char _tmp51F[7]="packed";static char _tmp520[7]="shared";static char _tmp521[7]="unused";static char _tmp522[5]="weak";static char _tmp523[10]="dllimport";static char _tmp524[10]="dllexport";static char _tmp525[23]="no_instrument_function";static char _tmp526[12]="constructor";static char _tmp527[11]="destructor";static char _tmp528[22]="no_check_memory_usage";static char _tmp529[5]="pure";struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf);static void _tmpA0B(union Cyc_YYSTYPE*yylval,unsigned int*_tmpA0A,unsigned int*_tmpA09,union Cyc_YYSTYPE**_tmpA08){for(*_tmpA0A=0;*_tmpA0A < *_tmpA09;(*_tmpA0A)++){(*_tmpA08)[*_tmpA0A]=(union Cyc_YYSTYPE)*yylval;}}static void _tmpA10(unsigned int*_tmpA0F,unsigned int*_tmpA0E,struct Cyc_Yyltype**_tmpA0D){for(*_tmpA0F=0;*_tmpA0F < *_tmpA0E;(*_tmpA0F)++){(*_tmpA0D)[*_tmpA0F]=(struct Cyc_Yyltype)
# 164 "cycbison.simple"
Cyc_yynewloc();}}static void _tmpA17(int*yyssp_offset,struct _dyneither_ptr*yyss,unsigned int*_tmpA16,unsigned int*_tmpA15,short**_tmpA13){for(*_tmpA16=0;*_tmpA16 < *_tmpA15;(*_tmpA16)++){(*_tmpA13)[*_tmpA16]=(short)(
# 216
*_tmpA16 <= *yyssp_offset?*((short*)_check_dyneither_subscript(*yyss,sizeof(short),(int)*_tmpA16)): 0);}}static void _tmpA1D(struct _dyneither_ptr*yyvs,int*yyssp_offset,union Cyc_YYSTYPE*yylval,unsigned int*_tmpA1C,unsigned int*_tmpA1B,union Cyc_YYSTYPE**_tmpA19){for(*_tmpA1C=0;*_tmpA1C < *_tmpA1B;(*_tmpA1C)++){(*_tmpA19)[*_tmpA1C]=(union Cyc_YYSTYPE)(
# 219
*_tmpA1C <= *yyssp_offset?*((union Cyc_YYSTYPE*)_check_dyneither_subscript(*yyvs,sizeof(union Cyc_YYSTYPE),(int)*_tmpA1C)):*yylval);}}static void _tmpA23(int*yyssp_offset,struct _dyneither_ptr*yyls,unsigned int*_tmpA22,unsigned int*_tmpA21,struct Cyc_Yyltype**_tmpA1F){for(*_tmpA22=0;*_tmpA22 < *_tmpA21;(*_tmpA22)++){(*_tmpA1F)[*_tmpA22]=(struct Cyc_Yyltype)(
# 222
*_tmpA22 <= *yyssp_offset?*((struct Cyc_Yyltype*)_check_dyneither_subscript(*yyls,sizeof(struct Cyc_Yyltype),(int)*_tmpA22)):
 Cyc_yynewloc());}}static void _tmpF24(unsigned int*_tmpF23,unsigned int*_tmpF22,char**_tmpF20){for(*_tmpF23=0;*_tmpF23 < *_tmpF22;(*_tmpF23)++){(*_tmpF20)[*_tmpF23]=(char)'\000';}}
# 137
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){
# 140
struct _RegionHandle _tmp45C=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp45C;_push_region(yyregion);
{int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 146
int yychar;
union Cyc_YYSTYPE _tmpA04;union Cyc_YYSTYPE yylval=((_tmpA04.YYINITIALSVAL).val=0,(((_tmpA04.YYINITIALSVAL).tag=64,_tmpA04)));
int yynerrs;
# 150
struct Cyc_Yyltype yylloc;
# 154
int yyssp_offset;
# 156
short*_tmpA06;unsigned int _tmpA05;struct _dyneither_ptr yyss=(_tmpA05=200,((_tmpA06=_region_calloc(yyregion,sizeof(short),_tmpA05),_tag_dyneither(_tmpA06,sizeof(short),_tmpA05))));
# 158
int yyvsp_offset;
unsigned int _tmpA0A;unsigned int _tmpA09;union Cyc_YYSTYPE*_tmpA08;unsigned int _tmpA07;struct _dyneither_ptr yyvs=_tag_dyneither(((_tmpA07=(unsigned int)200,((_tmpA08=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmpA07)),((((_tmpA09=_tmpA07,_tmpA0B(& yylval,& _tmpA0A,& _tmpA09,& _tmpA08))),_tmpA08)))))),sizeof(union Cyc_YYSTYPE),(unsigned int)200);
# 163
int yylsp_offset;
unsigned int _tmpA0F;unsigned int _tmpA0E;struct Cyc_Yyltype*_tmpA0D;unsigned int _tmpA0C;struct _dyneither_ptr yyls=_tag_dyneither(((_tmpA0C=(unsigned int)200,((_tmpA0D=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmpA0C)),((((_tmpA0E=_tmpA0C,_tmpA10(& _tmpA0F,& _tmpA0E,& _tmpA0D))),_tmpA0D)))))),sizeof(struct Cyc_Yyltype),(unsigned int)200);
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
{const char*_tmpA11;Cyc_yyerror(((_tmpA11="parser stack overflow",_tag_dyneither(_tmpA11,sizeof(char),22))),yystate,yychar);}
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 212
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
unsigned int _tmpA16;unsigned int _tmpA15;struct _dyneither_ptr _tmpA14;short*_tmpA13;unsigned int _tmpA12;struct _dyneither_ptr yyss1=(_tmpA12=(unsigned int)yystacksize,((_tmpA13=(short*)_region_malloc(yyregion,_check_times(sizeof(short),_tmpA12)),((_tmpA14=_tag_dyneither(_tmpA13,sizeof(short),_tmpA12),((((_tmpA15=_tmpA12,_tmpA17(& yyssp_offset,& yyss,& _tmpA16,& _tmpA15,& _tmpA13))),_tmpA14)))))));
# 217
unsigned int _tmpA1C;unsigned int _tmpA1B;struct _dyneither_ptr _tmpA1A;union Cyc_YYSTYPE*_tmpA19;unsigned int _tmpA18;struct _dyneither_ptr yyvs1=
(_tmpA18=(unsigned int)yystacksize,((_tmpA19=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmpA18)),((_tmpA1A=_tag_dyneither(_tmpA19,sizeof(union Cyc_YYSTYPE),_tmpA18),((((_tmpA1B=_tmpA18,_tmpA1D(& yyvs,& yyssp_offset,& yylval,& _tmpA1C,& _tmpA1B,& _tmpA19))),_tmpA1A)))))));
# 221
unsigned int _tmpA22;unsigned int _tmpA21;struct _dyneither_ptr _tmpA20;struct Cyc_Yyltype*_tmpA1F;unsigned int _tmpA1E;struct _dyneither_ptr yyls1=(_tmpA1E=(unsigned int)yystacksize,((_tmpA1F=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmpA1E)),((_tmpA20=_tag_dyneither(_tmpA1F,sizeof(struct Cyc_Yyltype),_tmpA1E),((((_tmpA21=_tmpA1E,_tmpA23(& yyssp_offset,& yyls,& _tmpA22,& _tmpA21,& _tmpA1F))),_tmpA20)))))));
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
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1060,yystate)];
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
yychar1=yychar > 0  && yychar <= 372?(int)Cyc_yytranslate[_check_known_subscript_notnull(373,yychar)]: 293;}
# 297 "cycbison.simple"
yyn +=yychar1;
if((yyn < 0  || yyn > 6814) || Cyc_yycheck[_check_known_subscript_notnull(6815,yyn)]!= yychar1)goto yydefault;
# 300
yyn=(int)Cyc_yytable[_check_known_subscript_notnull(6815,yyn)];
# 307
if(yyn < 0){
# 309
if(yyn == - 32768)goto yyerrlab;
yyn=- yyn;
goto yyreduce;}else{
# 313
if(yyn == 0)goto yyerrlab;}
# 315
if(yyn == 1059){
int _tmp46A=0;_npop_handler(0);return _tmp46A;}
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
 yyn=(int)Cyc_yydefact[_check_known_subscript_notnull(1060,yystate)];
if(yyn == 0)goto yyerrlab;
# 349
yyreduce:
# 351
 yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(525,yyn)];
if(yylen > 0)
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),(yyvsp_offset + 1)- yylen));
# 369 "cycbison.simple"
switch(yyn){case 1: _LL222: {
# 372
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 374
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1219 "parse.y"
yyval=yyyyvsp[0];
Cyc_Parse_parse_result=Cyc_yyget_YY16(yyyyvsp[0]);
# 1222
break;}case 2: _LL223: {
# 1224
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1226
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1225 "parse.y"
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp46B;_push_handler(& _tmp46B);{int _tmp46D=0;if(setjmp(_tmp46B.handler))_tmp46D=1;if(!_tmp46D){
x=Cyc_yyget_YY16(yyyyvsp[0]);;_pop_handler();}else{void*_tmp46C=(void*)_exn_thrown;void*_tmp46F=_tmp46C;void*_tmp471;_LL226: {struct Cyc_Core_Failure_exn_struct*_tmp470=(struct Cyc_Core_Failure_exn_struct*)_tmp46F;if(_tmp470->tag != Cyc_Core_Failure)goto _LL228;}_LL227:
# 1230
 x=0;goto _LL225;_LL228: _tmp471=_tmp46F;_LL229:(void)_rethrow(_tmp471);_LL225:;}};}
# 1232
{struct _handler_cons _tmp472;_push_handler(& _tmp472);{int _tmp474=0;if(setjmp(_tmp472.handler))_tmp474=1;if(!_tmp474){
y=Cyc_yyget_YY16(yyyyvsp[1]);;_pop_handler();}else{void*_tmp473=(void*)_exn_thrown;void*_tmp476=_tmp473;void*_tmp478;_LL22B: {struct Cyc_Core_Failure_exn_struct*_tmp477=(struct Cyc_Core_Failure_exn_struct*)_tmp476;if(_tmp477->tag != Cyc_Core_Failure)goto _LL22D;}_LL22C:
# 1236
 y=0;goto _LL22A;_LL22D: _tmp478=_tmp476;_LL22E:(void)_rethrow(_tmp478);_LL22A:;}};}
# 1238
yyval=Cyc_YY16(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y));
# 1240
break;}case 3: _LL224: {
# 1242
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1244
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1243 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA2D;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmpA2C;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmpA2B;struct Cyc_List_List*_tmpA2A;yyval=Cyc_YY16(((_tmpA2A=_cycalloc(sizeof(*_tmpA2A)),((_tmpA2A->hd=((_tmpA2D=_cycalloc(sizeof(*_tmpA2D)),((_tmpA2D->r=(void*)((_tmpA2B=_cycalloc(sizeof(*_tmpA2B)),((_tmpA2B[0]=((_tmpA2C.tag=10,((_tmpA2C.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA2C.f2=Cyc_yyget_YY16(yyyyvsp[2]),_tmpA2C)))))),_tmpA2B)))),((_tmpA2D->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA2D)))))),((_tmpA2A->tl=0,_tmpA2A)))))));}
Cyc_Lex_leave_using();
# 1246
break;}case 4: _LL22F: {
# 1248
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1250
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1247 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA37;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmpA36;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmpA35;struct Cyc_List_List*_tmpA34;yyval=Cyc_YY16(((_tmpA34=_cycalloc(sizeof(*_tmpA34)),((_tmpA34->hd=((_tmpA37=_cycalloc(sizeof(*_tmpA37)),((_tmpA37->r=(void*)((_tmpA35=_cycalloc(sizeof(*_tmpA35)),((_tmpA35[0]=((_tmpA36.tag=10,((_tmpA36.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA36.f2=Cyc_yyget_YY16(yyyyvsp[2]),_tmpA36)))))),_tmpA35)))),((_tmpA37->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA37)))))),((_tmpA34->tl=Cyc_yyget_YY16(yyyyvsp[4]),_tmpA34)))))));}
break;}case 5: _LL230: {
# 1250
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1252
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1250 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA46;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmpA45;struct _dyneither_ptr*_tmpA44;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpA43;struct Cyc_List_List*_tmpA42;yyval=Cyc_YY16(((_tmpA42=_cycalloc(sizeof(*_tmpA42)),((_tmpA42->hd=((_tmpA46=_cycalloc(sizeof(*_tmpA46)),((_tmpA46->r=(void*)((_tmpA43=_cycalloc(sizeof(*_tmpA43)),((_tmpA43[0]=((_tmpA45.tag=9,((_tmpA45.f1=((_tmpA44=_cycalloc(sizeof(*_tmpA44)),((_tmpA44[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpA44)))),((_tmpA45.f2=Cyc_yyget_YY16(yyyyvsp[2]),_tmpA45)))))),_tmpA43)))),((_tmpA46->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA46)))))),((_tmpA42->tl=0,_tmpA42)))))));}
Cyc_Lex_leave_namespace();
# 1253
break;}case 6: _LL231: {
# 1255
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1257
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1255 "parse.y"
struct _dyneither_ptr nspace;
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp486;_push_handler(& _tmp486);{int _tmp488=0;if(setjmp(_tmp486.handler))_tmp488=1;if(!_tmp488){nspace=Cyc_yyget_String_tok(yyyyvsp[0]);;_pop_handler();}else{void*_tmp487=(void*)_exn_thrown;void*_tmp48A=_tmp487;void*_tmp48C;_LL234: {struct Cyc_Core_Failure_exn_struct*_tmp48B=(struct Cyc_Core_Failure_exn_struct*)_tmp48A;if(_tmp48B->tag != Cyc_Core_Failure)goto _LL236;}_LL235:
{const char*_tmpA47;nspace=((_tmpA47="",_tag_dyneither(_tmpA47,sizeof(char),1)));}goto _LL233;_LL236: _tmp48C=_tmp48A;_LL237:(void)_rethrow(_tmp48C);_LL233:;}};}
# 1260
{struct _handler_cons _tmp48E;_push_handler(& _tmp48E);{int _tmp490=0;if(setjmp(_tmp48E.handler))_tmp490=1;if(!_tmp490){x=Cyc_yyget_YY16(yyyyvsp[2]);;_pop_handler();}else{void*_tmp48F=(void*)_exn_thrown;void*_tmp492=_tmp48F;void*_tmp494;_LL239: {struct Cyc_Core_Failure_exn_struct*_tmp493=(struct Cyc_Core_Failure_exn_struct*)_tmp492;if(_tmp493->tag != Cyc_Core_Failure)goto _LL23B;}_LL23A:
 x=0;goto _LL238;_LL23B: _tmp494=_tmp492;_LL23C:(void)_rethrow(_tmp494);_LL238:;}};}
# 1263
{struct _handler_cons _tmp495;_push_handler(& _tmp495);{int _tmp497=0;if(setjmp(_tmp495.handler))_tmp497=1;if(!_tmp497){y=Cyc_yyget_YY16(yyyyvsp[4]);;_pop_handler();}else{void*_tmp496=(void*)_exn_thrown;void*_tmp499=_tmp496;void*_tmp49B;_LL23E: {struct Cyc_Core_Failure_exn_struct*_tmp49A=(struct Cyc_Core_Failure_exn_struct*)_tmp499;if(_tmp49A->tag != Cyc_Core_Failure)goto _LL240;}_LL23F:
 y=0;goto _LL23D;_LL240: _tmp49B=_tmp499;_LL241:(void)_rethrow(_tmp49B);_LL23D:;}};}
# 1266
{struct Cyc_Absyn_Decl*_tmpA56;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmpA55;struct _dyneither_ptr*_tmpA54;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpA53;struct Cyc_List_List*_tmpA52;yyval=Cyc_YY16(((_tmpA52=_cycalloc(sizeof(*_tmpA52)),((_tmpA52->hd=((_tmpA56=_cycalloc(sizeof(*_tmpA56)),((_tmpA56->r=(void*)((_tmpA53=_cycalloc(sizeof(*_tmpA53)),((_tmpA53[0]=((_tmpA55.tag=9,((_tmpA55.f1=((_tmpA54=_cycalloc(sizeof(*_tmpA54)),((_tmpA54[0]=nspace,_tmpA54)))),((_tmpA55.f2=x,_tmpA55)))))),_tmpA53)))),((_tmpA56->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA56)))))),((_tmpA52->tl=y,_tmpA52)))))));}
# 1268
break;}case 7: _LL232: {
# 1270
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1272
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1269 "parse.y"
int _tmp4A1=Cyc_yyget_YY31(yyyyvsp[0]);
if(!_tmp4A1){
struct Cyc_Absyn_Decl*_tmpA60;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct _tmpA5F;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmpA5E;struct Cyc_List_List*_tmpA5D;yyval=Cyc_YY16(((_tmpA5D=_cycalloc(sizeof(*_tmpA5D)),((_tmpA5D->hd=((_tmpA60=_cycalloc(sizeof(*_tmpA60)),((_tmpA60->r=(void*)((_tmpA5E=_cycalloc(sizeof(*_tmpA5E)),((_tmpA5E[0]=((_tmpA5F.tag=11,((_tmpA5F.f1=Cyc_yyget_YY16(yyyyvsp[2]),_tmpA5F)))),_tmpA5E)))),((_tmpA60->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA60)))))),((_tmpA5D->tl=Cyc_yyget_YY16(yyyyvsp[4]),_tmpA5D)))))));}else{
# 1273
struct Cyc_Absyn_Decl*_tmpA6A;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmpA69;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA68;struct Cyc_List_List*_tmpA67;yyval=Cyc_YY16(((_tmpA67=_cycalloc(sizeof(*_tmpA67)),((_tmpA67->hd=((_tmpA6A=_cycalloc(sizeof(*_tmpA6A)),((_tmpA6A->r=(void*)((_tmpA68=_cycalloc(sizeof(*_tmpA68)),((_tmpA68[0]=((_tmpA69.tag=12,((_tmpA69.f1=Cyc_yyget_YY16(yyyyvsp[2]),((_tmpA69.f2=0,_tmpA69)))))),_tmpA68)))),((_tmpA6A->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA6A)))))),((_tmpA67->tl=Cyc_yyget_YY16(yyyyvsp[4]),_tmpA67)))))));}
# 1275
break;}case 8: _LL242: {
# 1277
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1279
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1276 "parse.y"
if(!Cyc_yyget_YY31(yyyyvsp[0])){
const char*_tmpA6B;Cyc_Parse_err(((_tmpA6B="expecting \"C include\"",_tag_dyneither(_tmpA6B,sizeof(char),22))),(unsigned int)(yyyylsp[0]).first_line);}{
struct Cyc_List_List*exs=Cyc_yyget_YY52(yyyyvsp[4]);
{struct Cyc_Absyn_Decl*_tmpA75;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmpA74;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA73;struct Cyc_List_List*_tmpA72;yyval=Cyc_YY16(((_tmpA72=_cycalloc(sizeof(*_tmpA72)),((_tmpA72->hd=((_tmpA75=_cycalloc(sizeof(*_tmpA75)),((_tmpA75->r=(void*)((_tmpA73=_cycalloc(sizeof(*_tmpA73)),((_tmpA73[0]=((_tmpA74.tag=12,((_tmpA74.f1=Cyc_yyget_YY16(yyyyvsp[2]),((_tmpA74.f2=exs,_tmpA74)))))),_tmpA73)))),((_tmpA75->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA75)))))),((_tmpA72->tl=Cyc_yyget_YY16(yyyyvsp[5]),_tmpA72)))))));}
# 1281
break;};}case 9: _LL243: {
# 1283
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1285
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1282 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA78;struct Cyc_List_List*_tmpA77;yyval=Cyc_YY16(((_tmpA77=_cycalloc(sizeof(*_tmpA77)),((_tmpA77->hd=((_tmpA78=_cycalloc(sizeof(*_tmpA78)),((_tmpA78->r=(void*)& Cyc_Absyn_Porton_d_val,((_tmpA78->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA78)))))),((_tmpA77->tl=Cyc_yyget_YY16(yyyyvsp[2]),_tmpA77)))))));}
break;}case 10: _LL244: {
# 1285
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1287
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1284 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA7B;struct Cyc_List_List*_tmpA7A;yyval=Cyc_YY16(((_tmpA7A=_cycalloc(sizeof(*_tmpA7A)),((_tmpA7A->hd=((_tmpA7B=_cycalloc(sizeof(*_tmpA7B)),((_tmpA7B->r=(void*)& Cyc_Absyn_Portoff_d_val,((_tmpA7B->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA7B)))))),((_tmpA7A->tl=Cyc_yyget_YY16(yyyyvsp[2]),_tmpA7A)))))));}
break;}case 11: _LL245:
# 1287
 yyval=Cyc_YY16(0);
break;case 12: _LL246: {
# 1290
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1292
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1290 "parse.y"
struct _dyneither_ptr _tmp4B3=Cyc_yyget_String_tok(yyyyvsp[1]);
Cyc_Lex_enter_extern_c();
{const char*_tmpA7C;if(Cyc_strcmp((struct _dyneither_ptr)_tmp4B3,((_tmpA7C="C",_tag_dyneither(_tmpA7C,sizeof(char),2))))== 0)
yyval=Cyc_YY31(0);else{
const char*_tmpA7D;if(Cyc_strcmp((struct _dyneither_ptr)_tmp4B3,((_tmpA7D="C include",_tag_dyneither(_tmpA7D,sizeof(char),10))))== 0)
yyval=Cyc_YY31(1);else{
# 1297
{const char*_tmpA7E;Cyc_Parse_err(((_tmpA7E="expecting \"C\" or \"C include\"",_tag_dyneither(_tmpA7E,sizeof(char),29))),(unsigned int)(yyyylsp[0]).first_line);}
yyval=Cyc_YY31(1);}}}
# 1301
break;}case 13: _LL247: {
# 1303
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1305
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1304 "parse.y"
Cyc_Lex_leave_extern_c();
break;}case 14: _LL248: {
# 1307
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1309
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1308 "parse.y"
yyval=yyyyvsp[2];
break;}case 15: _LL249: {
# 1311
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1313
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1309 "parse.y"
yyval=Cyc_YY52(0);
break;}case 16: _LL24A: {
# 1312
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1314
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1313 "parse.y"
{struct _tuple27*_tmpA81;struct Cyc_List_List*_tmpA80;yyval=Cyc_YY52(((_tmpA80=_cycalloc(sizeof(*_tmpA80)),((_tmpA80->hd=((_tmpA81=_cycalloc(sizeof(*_tmpA81)),((_tmpA81->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA81->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA81->f3=0,_tmpA81)))))))),((_tmpA80->tl=0,_tmpA80)))))));}
break;}case 17: _LL24B: {
# 1316
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1318
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1314 "parse.y"
{struct _tuple27*_tmpA84;struct Cyc_List_List*_tmpA83;yyval=Cyc_YY52(((_tmpA83=_cycalloc(sizeof(*_tmpA83)),((_tmpA83->hd=((_tmpA84=_cycalloc(sizeof(*_tmpA84)),((_tmpA84->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA84->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA84->f3=0,_tmpA84)))))))),((_tmpA83->tl=0,_tmpA83)))))));}
break;}case 18: _LL24C: {
# 1317
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1319
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1316 "parse.y"
{struct _tuple27*_tmpA87;struct Cyc_List_List*_tmpA86;yyval=Cyc_YY52(((_tmpA86=_cycalloc(sizeof(*_tmpA86)),((_tmpA86->hd=((_tmpA87=_cycalloc(sizeof(*_tmpA87)),((_tmpA87->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA87->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA87->f3=0,_tmpA87)))))))),((_tmpA86->tl=Cyc_yyget_YY52(yyyyvsp[2]),_tmpA86)))))));}
break;}case 19: _LL24D: {
# 1319
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1321
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1320 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpA8D;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpA8C;struct Cyc_List_List*_tmpA8B;yyval=Cyc_YY16(((_tmpA8B=_cycalloc(sizeof(*_tmpA8B)),((_tmpA8B->hd=Cyc_Absyn_new_decl((void*)((_tmpA8D=_cycalloc(sizeof(*_tmpA8D)),((_tmpA8D[0]=((_tmpA8C.tag=1,((_tmpA8C.f1=Cyc_yyget_YY15(yyyyvsp[0]),_tmpA8C)))),_tmpA8D)))),(unsigned int)(yyyylsp[0]).first_line),((_tmpA8B->tl=0,_tmpA8B)))))));}
break;}case 20: _LL24E: {
# 1323
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1325
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1321 "parse.y"
yyval=yyyyvsp[0];
break;}case 21: _LL24F: {
# 1324
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1326
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1322 "parse.y"
yyval=Cyc_YY16(0);
break;}case 24: _LL250: {
# 1325
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1327
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1331 "parse.y"
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,0,Cyc_yyget_YY27(yyyyvsp[0]),0,Cyc_yyget_Stmt_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 25: _LL251: {
# 1334
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1336
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1333 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4C0=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,& _tmp4C0,Cyc_yyget_YY27(yyyyvsp[1]),0,Cyc_yyget_Stmt_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 26: _LL252: {
# 1337
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1339
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1346 "parse.y"
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,0,Cyc_yyget_YY27(yyyyvsp[0]),Cyc_yyget_YY16(yyyyvsp[1]),Cyc_yyget_Stmt_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 27: _LL253: {
# 1349
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1351
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1348 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4C1=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,& _tmp4C1,Cyc_yyget_YY27(yyyyvsp[1]),Cyc_yyget_YY16(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 28: _LL254: {
# 1352
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1354
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1356 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4C2=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,& _tmp4C2,Cyc_yyget_YY27(yyyyvsp[1]),0,Cyc_yyget_Stmt_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 29: _LL255: {
# 1360
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1362
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1359 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4C3=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(Cyc_Parse_make_function(yyr,& _tmp4C3,Cyc_yyget_YY27(yyyyvsp[1]),Cyc_yyget_YY16(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 30: _LL256: {
# 1363
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1365
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1364 "parse.y"
Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyyyvsp[1]));yyval=yyyyvsp[1];
break;}case 31: _LL257: {
# 1367
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1369
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1367 "parse.y"
Cyc_Lex_leave_using();
break;}case 32: _LL258: {
# 1370
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1372
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1370 "parse.y"
{struct _dyneither_ptr*_tmpA8E;Cyc_Lex_enter_namespace(((_tmpA8E=_cycalloc(sizeof(*_tmpA8E)),((_tmpA8E[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpA8E)))));}yyval=yyyyvsp[1];
break;}case 33: _LL259: {
# 1373
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1375
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1373 "parse.y"
Cyc_Lex_leave_namespace();
break;}case 34: _LL25A: {
# 1376
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1378
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
int _tmp4C5=(yyyylsp[0]).first_line;
yyval=Cyc_YY16(Cyc_Parse_make_declarations(Cyc_yyget_YY17(yyyyvsp[0]),0,(unsigned int)_tmp4C5,(unsigned int)_tmp4C5));
break;}case 35: _LL25B: {
# 1383
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1385
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1382 "parse.y"
int _tmp4C6=(yyyylsp[0]).first_line;
yyval=Cyc_YY16(Cyc_Parse_make_declarations(Cyc_yyget_YY17(yyyyvsp[0]),Cyc_yyget_YY19(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line,(unsigned int)_tmp4C6));
break;}case 36: _LL25C: {
# 1386
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1388
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1386 "parse.y"
{struct Cyc_List_List*_tmpA8F;yyval=Cyc_YY16(((_tmpA8F=_cycalloc(sizeof(*_tmpA8F)),((_tmpA8F->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY9(yyyyvsp[1]),Cyc_yyget_Exp_tok(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line),((_tmpA8F->tl=0,_tmpA8F)))))));}
break;}case 37: _LL25D: {
# 1389
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1391
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1388 "parse.y"
struct Cyc_List_List*_tmp4C8=0;
{struct Cyc_List_List*_tmp4C9=Cyc_yyget_YY36(yyyyvsp[1]);for(0;_tmp4C9 != 0;_tmp4C9=_tmp4C9->tl){
struct _dyneither_ptr*_tmp4CA=(struct _dyneither_ptr*)_tmp4C9->hd;
struct _tuple0*_tmpA90;struct _tuple0*qv=(_tmpA90=_cycalloc(sizeof(*_tmpA90)),((_tmpA90->f1=Cyc_Absyn_Rel_n(0),((_tmpA90->f2=_tmp4CA,_tmpA90)))));
struct Cyc_Absyn_Vardecl*_tmp4CB=Cyc_Absyn_new_vardecl(qv,Cyc_Absyn_wildtyp(0),0);
struct Cyc_List_List*_tmpA91;_tmp4C8=((_tmpA91=_cycalloc(sizeof(*_tmpA91)),((_tmpA91->hd=_tmp4CB,((_tmpA91->tl=_tmp4C8,_tmpA91))))));}}
# 1395
_tmp4C8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4C8);
{struct Cyc_List_List*_tmpA92;yyval=Cyc_YY16(((_tmpA92=_cycalloc(sizeof(*_tmpA92)),((_tmpA92->hd=Cyc_Absyn_letv_decl(_tmp4C8,(unsigned int)(yyyylsp[0]).first_line),((_tmpA92->tl=0,_tmpA92)))))));}
# 1398
break;}case 38: _LL25E: {
# 1400
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 1402
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 1401 "parse.y"
struct _dyneither_ptr _tmp4CF=Cyc_yyget_String_tok(yyyyvsp[3]);
# 1403
{const char*_tmpA93;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4CF,((_tmpA93="`H",_tag_dyneither(_tmpA93,sizeof(char),3))))== 0){
const char*_tmpA96;void*_tmpA95;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA95=0,Cyc_aprintf(((_tmpA96="bad occurrence of heap region",_tag_dyneither(_tmpA96,sizeof(char),30))),_tag_dyneither(_tmpA95,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}
{const char*_tmpA97;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4CF,((_tmpA97="`U",_tag_dyneither(_tmpA97,sizeof(char),3))))== 0){
const char*_tmpA9A;void*_tmpA99;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA99=0,Cyc_aprintf(((_tmpA9A="bad occurrence of unique region",_tag_dyneither(_tmpA9A,sizeof(char),32))),_tag_dyneither(_tmpA99,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}{
struct _dyneither_ptr*_tmpA9D;struct Cyc_Absyn_Tvar*_tmpA9C;struct Cyc_Absyn_Tvar*tv=(_tmpA9C=_cycalloc(sizeof(*_tmpA9C)),((_tmpA9C->name=((_tmpA9D=_cycalloc(sizeof(*_tmpA9D)),((_tmpA9D[0]=_tmp4CF,_tmpA9D)))),((_tmpA9C->identity=- 1,((_tmpA9C->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA9C)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpAA0;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA9F;void*t=(void*)((_tmpA9F=_cycalloc(sizeof(*_tmpA9F)),((_tmpA9F[0]=((_tmpAA0.tag=2,((_tmpAA0.f1=tv,_tmpAA0)))),_tmpA9F))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpAA6;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpAA5;struct _dyneither_ptr*_tmpAA3;struct _tuple0*_tmpAA2;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpAA2=_cycalloc(sizeof(*_tmpAA2)),((_tmpAA2->f1=Cyc_Absyn_Loc_n,((_tmpAA2->f2=((_tmpAA3=_cycalloc(sizeof(*_tmpAA3)),((_tmpAA3[0]=Cyc_yyget_String_tok(yyyyvsp[5]),_tmpAA3)))),_tmpAA2)))))),(void*)((_tmpAA5=_cycalloc(sizeof(*_tmpAA5)),((_tmpAA5[0]=((_tmpAA6.tag=15,((_tmpAA6.f1=t,_tmpAA6)))),_tmpAA5)))),0);
{struct Cyc_List_List*_tmpAA7;yyval=Cyc_YY16(((_tmpAA7=_cycalloc(sizeof(*_tmpAA7)),((_tmpAA7->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY31(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line),((_tmpAA7->tl=0,_tmpAA7)))))));}
# 1412
break;};}case 39: _LL25F: {
# 1414
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1416
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1414 "parse.y"
int _tmp4DF=Cyc_yyget_YY31(yyyyvsp[0]);
struct _dyneither_ptr _tmp4E0=Cyc_yyget_String_tok(yyyyvsp[2]);
{const char*_tmpAA8;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4E0,((_tmpAA8="H",_tag_dyneither(_tmpAA8,sizeof(char),2))))== 0){
const char*_tmpAA9;Cyc_Parse_err(((_tmpAA9="bad occurrence of heap region `H",_tag_dyneither(_tmpAA9,sizeof(char),33))),(unsigned int)(yyyylsp[2]).first_line);}}
{const char*_tmpAAA;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4E0,((_tmpAAA="U",_tag_dyneither(_tmpAAA,sizeof(char),2))))== 0){
const char*_tmpAAB;Cyc_Parse_err(((_tmpAAB="bad occurrence of unique region `U",_tag_dyneither(_tmpAAB,sizeof(char),35))),(unsigned int)(yyyylsp[2]).first_line);}}{
struct _dyneither_ptr*_tmpAB8;const char*_tmpAB7;void*_tmpAB6[1];struct Cyc_String_pa_PrintArg_struct _tmpAB5;struct Cyc_Absyn_Tvar*_tmpAB4;struct Cyc_Absyn_Tvar*tv=(_tmpAB4=_cycalloc(sizeof(*_tmpAB4)),((_tmpAB4->name=((_tmpAB8=_cycalloc(sizeof(*_tmpAB8)),((_tmpAB8[0]=(struct _dyneither_ptr)((_tmpAB5.tag=0,((_tmpAB5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4E0),((_tmpAB6[0]=& _tmpAB5,Cyc_aprintf(((_tmpAB7="`%s",_tag_dyneither(_tmpAB7,sizeof(char),4))),_tag_dyneither(_tmpAB6,sizeof(void*),1)))))))),_tmpAB8)))),((_tmpAB4->identity=- 1,((_tmpAB4->kind=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpAB4)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpABB;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpABA;void*t=(void*)((_tmpABA=_cycalloc(sizeof(*_tmpABA)),((_tmpABA[0]=((_tmpABB.tag=2,((_tmpABB.f1=tv,_tmpABB)))),_tmpABA))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpAC1;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpAC0;struct _dyneither_ptr*_tmpABE;struct _tuple0*_tmpABD;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpABD=_cycalloc(sizeof(*_tmpABD)),((_tmpABD->f1=Cyc_Absyn_Loc_n,((_tmpABD->f2=((_tmpABE=_cycalloc(sizeof(*_tmpABE)),((_tmpABE[0]=_tmp4E0,_tmpABE)))),_tmpABD)))))),(void*)((_tmpAC0=_cycalloc(sizeof(*_tmpAC0)),((_tmpAC0[0]=((_tmpAC1.tag=15,((_tmpAC1.f1=t,_tmpAC1)))),_tmpAC0)))),0);
# 1425
{struct Cyc_List_List*_tmpAC2;yyval=Cyc_YY16(((_tmpAC2=_cycalloc(sizeof(*_tmpAC2)),((_tmpAC2->hd=Cyc_Absyn_region_decl(tv,vd,_tmp4DF,0,(unsigned int)(yyyylsp[0]).first_line),((_tmpAC2->tl=0,_tmpAC2)))))));}
# 1427
break;};}case 40: _LL260: {
# 1429
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 1431
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 1429 "parse.y"
struct _dyneither_ptr _tmp4F1=Cyc_yyget_String_tok(yyyyvsp[2]);
struct _dyneither_ptr _tmp4F2=Cyc_yyget_String_tok(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp4F3=Cyc_yyget_Exp_tok(yyyyvsp[6]);
{const char*_tmpAC3;if(Cyc_strcmp((struct _dyneither_ptr)_tmp4F2,((_tmpAC3="open",_tag_dyneither(_tmpAC3,sizeof(char),5))))!= 0){const char*_tmpAC4;Cyc_Parse_err(((_tmpAC4="expecting `open'",_tag_dyneither(_tmpAC4,sizeof(char),17))),(unsigned int)(yyyylsp[3]).first_line);}}{
struct _dyneither_ptr*_tmpAD1;const char*_tmpAD0;void*_tmpACF[1];struct Cyc_String_pa_PrintArg_struct _tmpACE;struct Cyc_Absyn_Tvar*_tmpACD;struct Cyc_Absyn_Tvar*tv=(_tmpACD=_cycalloc(sizeof(*_tmpACD)),((_tmpACD->name=((_tmpAD1=_cycalloc(sizeof(*_tmpAD1)),((_tmpAD1[0]=(struct _dyneither_ptr)((_tmpACE.tag=0,((_tmpACE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4F1),((_tmpACF[0]=& _tmpACE,Cyc_aprintf(((_tmpAD0="`%s",_tag_dyneither(_tmpAD0,sizeof(char),4))),_tag_dyneither(_tmpACF,sizeof(void*),1)))))))),_tmpAD1)))),((_tmpACD->identity=- 1,((_tmpACD->kind=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpACD)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpAD4;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpAD3;void*t=(void*)((_tmpAD3=_cycalloc(sizeof(*_tmpAD3)),((_tmpAD3[0]=((_tmpAD4.tag=2,((_tmpAD4.f1=tv,_tmpAD4)))),_tmpAD3))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpADA;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpAD9;struct _dyneither_ptr*_tmpAD7;struct _tuple0*_tmpAD6;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpAD6=_cycalloc(sizeof(*_tmpAD6)),((_tmpAD6->f1=Cyc_Absyn_Loc_n,((_tmpAD6->f2=((_tmpAD7=_cycalloc(sizeof(*_tmpAD7)),((_tmpAD7[0]=_tmp4F1,_tmpAD7)))),_tmpAD6)))))),(void*)((_tmpAD9=_cycalloc(sizeof(*_tmpAD9)),((_tmpAD9[0]=((_tmpADA.tag=15,((_tmpADA.f1=t,_tmpADA)))),_tmpAD9)))),0);
# 1438
{struct Cyc_List_List*_tmpADB;yyval=Cyc_YY16(((_tmpADB=_cycalloc(sizeof(*_tmpADB)),((_tmpADB->hd=Cyc_Absyn_region_decl(tv,vd,0,_tmp4F3,(unsigned int)(yyyylsp[0]).first_line),((_tmpADB->tl=0,_tmpADB)))))));}
# 1440
break;};}case 41: _LL261:
# 1442
 yyval=Cyc_YY31(0);
break;case 42: _LL262: {
# 1445
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1447
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1444 "parse.y"
yyval=Cyc_YY31(1);
break;}case 43: _LL263: {
# 1447
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1449
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1449 "parse.y"
yyval=yyyyvsp[0];
break;}case 44: _LL264: {
# 1452
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1454
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1451 "parse.y"
yyval=Cyc_YY16(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY16(yyyyvsp[0]),Cyc_yyget_YY16(yyyyvsp[1])));
break;}case 45: _LL265: {
# 1454
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1456
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_Parse_Declaration_spec _tmpADC;yyval=Cyc_YY17(((_tmpADC.sc=Cyc_yyget_YY20(yyyyvsp[0]),((_tmpADC.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpADC.type_specs=
Cyc_Parse_empty_spec(0),((_tmpADC.is_inline=0,((_tmpADC.attributes=0,_tmpADC)))))))))));}
break;}case 46: _LL266: {
# 1461
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1463
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1460 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp503=Cyc_yyget_YY17(yyyyvsp[1]);
if(_tmp503.sc != 0){
const char*_tmpADF;void*_tmpADE;(_tmpADE=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpADF="Only one storage class is allowed in a declaration (missing ';' or ','?)",_tag_dyneither(_tmpADF,sizeof(char),73))),_tag_dyneither(_tmpADE,sizeof(void*),0)));}
# 1464
{struct Cyc_Parse_Declaration_spec _tmpAE0;yyval=Cyc_YY17(((_tmpAE0.sc=Cyc_yyget_YY20(yyyyvsp[0]),((_tmpAE0.tq=_tmp503.tq,((_tmpAE0.type_specs=_tmp503.type_specs,((_tmpAE0.is_inline=_tmp503.is_inline,((_tmpAE0.attributes=_tmp503.attributes,_tmpAE0)))))))))));}
# 1468
break;}case 47: _LL267: {
# 1470
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1472
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1469 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpAE1;yyval=Cyc_YY17(((_tmpAE1.sc=0,((_tmpAE1.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAE1.type_specs=
Cyc_yyget_YY21(yyyyvsp[0]),((_tmpAE1.is_inline=0,((_tmpAE1.attributes=0,_tmpAE1)))))))))));}
break;}case 48: _LL268: {
# 1473
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1475
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1472 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp508=Cyc_yyget_YY17(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpAE2;yyval=Cyc_YY17(((_tmpAE2.sc=_tmp508.sc,((_tmpAE2.tq=_tmp508.tq,((_tmpAE2.type_specs=
Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,_tmp508.type_specs,Cyc_yyget_YY21(yyyyvsp[0])),((_tmpAE2.is_inline=_tmp508.is_inline,((_tmpAE2.attributes=_tmp508.attributes,_tmpAE2)))))))))));}
# 1478
break;}case 49: _LL269: {
# 1480
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1482
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1479 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpAE3;yyval=Cyc_YY17(((_tmpAE3.sc=0,((_tmpAE3.tq=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpAE3.type_specs=Cyc_Parse_empty_spec(0),((_tmpAE3.is_inline=0,((_tmpAE3.attributes=0,_tmpAE3)))))))))));}
break;}case 50: _LL26A: {
# 1482
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1484
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1481 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp50B=Cyc_yyget_YY17(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpAE4;yyval=Cyc_YY17(((_tmpAE4.sc=_tmp50B.sc,((_tmpAE4.tq=Cyc_Absyn_combine_tqual(Cyc_yyget_YY23(yyyyvsp[0]),_tmp50B.tq),((_tmpAE4.type_specs=_tmp50B.type_specs,((_tmpAE4.is_inline=_tmp50B.is_inline,((_tmpAE4.attributes=_tmp50B.attributes,_tmpAE4)))))))))));}
# 1486
break;}case 51: _LL26B: {
# 1488
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1490
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1487 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpAE5;yyval=Cyc_YY17(((_tmpAE5.sc=0,((_tmpAE5.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAE5.type_specs=
Cyc_Parse_empty_spec(0),((_tmpAE5.is_inline=1,((_tmpAE5.attributes=0,_tmpAE5)))))))))));}
break;}case 52: _LL26C: {
# 1491
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1493
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1490 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp50E=Cyc_yyget_YY17(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpAE6;yyval=Cyc_YY17(((_tmpAE6.sc=_tmp50E.sc,((_tmpAE6.tq=_tmp50E.tq,((_tmpAE6.type_specs=_tmp50E.type_specs,((_tmpAE6.is_inline=1,((_tmpAE6.attributes=_tmp50E.attributes,_tmpAE6)))))))))));}
# 1494
break;}case 53: _LL26D: {
# 1496
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1498
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1495 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpAE7;yyval=Cyc_YY17(((_tmpAE7.sc=0,((_tmpAE7.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAE7.type_specs=
Cyc_Parse_empty_spec(0),((_tmpAE7.is_inline=0,((_tmpAE7.attributes=Cyc_yyget_YY45(yyyyvsp[0]),_tmpAE7)))))))))));}
break;}case 54: _LL26E: {
# 1499
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1501
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1498 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp511=Cyc_yyget_YY17(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpAE8;yyval=Cyc_YY17(((_tmpAE8.sc=_tmp511.sc,((_tmpAE8.tq=_tmp511.tq,((_tmpAE8.type_specs=_tmp511.type_specs,((_tmpAE8.is_inline=_tmp511.is_inline,((_tmpAE8.attributes=
# 1501
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY45(yyyyvsp[0]),_tmp511.attributes),_tmpAE8)))))))))));}
break;}case 55: _LL26F: {
# 1504
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1506
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1505 "parse.y"
static enum Cyc_Parse_Storage_class auto_sc=Cyc_Parse_Auto_sc;
yyval=Cyc_YY20(& auto_sc);
break;}case 56: _LL270: {
# 1509
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1511
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1507 "parse.y"
static enum Cyc_Parse_Storage_class register_sc=Cyc_Parse_Register_sc;
yyval=Cyc_YY20(& register_sc);
break;}case 57: _LL271: {
# 1511
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1513
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1509 "parse.y"
static enum Cyc_Parse_Storage_class static_sc=Cyc_Parse_Static_sc;
yyval=Cyc_YY20(& static_sc);
break;}case 58: _LL272: {
# 1513
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1515
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1511 "parse.y"
static enum Cyc_Parse_Storage_class extern_sc=Cyc_Parse_Extern_sc;
yyval=Cyc_YY20(& extern_sc);
break;}case 59: _LL273: {
# 1515
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1517
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1514 "parse.y"
static enum Cyc_Parse_Storage_class externC_sc=Cyc_Parse_ExternC_sc;
{const char*_tmpAE9;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpAE9="C",_tag_dyneither(_tmpAE9,sizeof(char),2))))!= 0){
const char*_tmpAEA;Cyc_Parse_err(((_tmpAEA="only extern or extern \"C\" is allowed",_tag_dyneither(_tmpAEA,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY20(& externC_sc);
# 1519
break;}case 60: _LL274: {
# 1521
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1523
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1519 "parse.y"
static enum Cyc_Parse_Storage_class typedef_sc=Cyc_Parse_Typedef_sc;
yyval=Cyc_YY20(& typedef_sc);
break;}case 61: _LL275: {
# 1523
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1525
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1522 "parse.y"
static enum Cyc_Parse_Storage_class abstract_sc=Cyc_Parse_Abstract_sc;
yyval=Cyc_YY20(& abstract_sc);
break;}case 62: _LL276:
# 1526
 yyval=Cyc_YY45(0);
break;case 63: _LL277: {
# 1529
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1531
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1529 "parse.y"
yyval=yyyyvsp[0];
break;}case 64: _LL278: {
# 1532
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1534
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1534 "parse.y"
yyval=yyyyvsp[3];
break;}case 65: _LL279: {
# 1537
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1539
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1538 "parse.y"
{struct Cyc_List_List*_tmpAEB;yyval=Cyc_YY45(((_tmpAEB=_cycalloc(sizeof(*_tmpAEB)),((_tmpAEB->hd=Cyc_yyget_YY46(yyyyvsp[0]),((_tmpAEB->tl=0,_tmpAEB)))))));}
break;}case 66: _LL27A: {
# 1541
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1543
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1539 "parse.y"
{struct Cyc_List_List*_tmpAEC;yyval=Cyc_YY45(((_tmpAEC=_cycalloc(sizeof(*_tmpAEC)),((_tmpAEC->hd=Cyc_yyget_YY46(yyyyvsp[0]),((_tmpAEC->tl=Cyc_yyget_YY45(yyyyvsp[2]),_tmpAEC)))))));}
break;}case 67: _LL27B: {
# 1542
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1544
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1544 "parse.y"
static struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct att_aligned={6,0};
static struct _tuple28 att_map[17]={{{_tmp519,_tmp519,_tmp519 + 8},(void*)& Cyc_Absyn_Stdcall_att_val},{{_tmp51A,_tmp51A,_tmp51A + 6},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp51B,_tmp51B,_tmp51B + 9},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp51C,_tmp51C,_tmp51C + 9},(void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp51D,_tmp51D,_tmp51D + 6},(void*)& Cyc_Absyn_Const_att_val},{{_tmp51E,_tmp51E,_tmp51E + 8},(void*)& att_aligned},{{_tmp51F,_tmp51F,_tmp51F + 7},(void*)& Cyc_Absyn_Packed_att_val},{{_tmp520,_tmp520,_tmp520 + 7},(void*)& Cyc_Absyn_Shared_att_val},{{_tmp521,_tmp521,_tmp521 + 7},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp522,_tmp522,_tmp522 + 5},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp523,_tmp523,_tmp523 + 10},(void*)& Cyc_Absyn_Dllimport_att_val},{{_tmp524,_tmp524,_tmp524 + 10},(void*)& Cyc_Absyn_Dllexport_att_val},{{_tmp525,_tmp525,_tmp525 + 23},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{_tmp526,_tmp526,_tmp526 + 12},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp527,_tmp527,_tmp527 + 11},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp528,_tmp528,_tmp528 + 22},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp529,_tmp529,_tmp529 + 5},(void*)& Cyc_Absyn_Pure_att_val}};
# 1564
struct _dyneither_ptr _tmp517=Cyc_yyget_String_tok(yyyyvsp[0]);
# 1566
if((((_get_dyneither_size(_tmp517,sizeof(char))> 4  && ((const char*)_tmp517.curr)[0]== '_') && ((const char*)_tmp517.curr)[1]== '_') && *((const char*)_check_dyneither_subscript(_tmp517,sizeof(char),(int)(_get_dyneither_size(_tmp517,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(_tmp517,sizeof(char),(int)(_get_dyneither_size(_tmp517,sizeof(char))- 3)))== '_')
# 1568
_tmp517=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp517,2,_get_dyneither_size(_tmp517,sizeof(char))- 5);{
int i=0;
for(0;i < 17;++ i){
if(Cyc_strcmp((struct _dyneither_ptr)_tmp517,(struct _dyneither_ptr)(att_map[_check_known_subscript_notnull(17,i)]).f1)== 0){
yyval=Cyc_YY46((att_map[_check_known_subscript_notnull(17,i)]).f2);
break;}}
# 1575
if(i == 17){
{const char*_tmpAED;Cyc_Parse_err(((_tmpAED="unrecognized attribute",_tag_dyneither(_tmpAED,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
yyval=Cyc_YY46((void*)& Cyc_Absyn_Cdecl_att_val);}
# 1580
break;};}case 68: _LL27C: {
# 1582
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1584
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1580 "parse.y"
yyval=Cyc_YY46((void*)& Cyc_Absyn_Const_att_val);
break;}case 69: _LL27D: {
# 1583
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1585
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1582 "parse.y"
struct _dyneither_ptr _tmp52B=Cyc_yyget_String_tok(yyyyvsp[0]);
struct Cyc_Absyn_Exp*_tmp52C=Cyc_yyget_Exp_tok(yyyyvsp[2]);
void*a;
{const char*_tmpAEF;const char*_tmpAEE;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpAEF="aligned",_tag_dyneither(_tmpAEF,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpAEE="__aligned__",_tag_dyneither(_tmpAEE,sizeof(char),12))))== 0){
struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpAF2;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpAF1;a=(void*)((_tmpAF1=_cycalloc(sizeof(*_tmpAF1)),((_tmpAF1[0]=((_tmpAF2.tag=6,((_tmpAF2.f1=_tmp52C,_tmpAF2)))),_tmpAF1))));}else{
const char*_tmpAF4;const char*_tmpAF3;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpAF4="section",_tag_dyneither(_tmpAF4,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpAF3="__section__",_tag_dyneither(_tmpAF3,sizeof(char),12))))== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp52C);
struct Cyc_Absyn_Section_att_Absyn_Attribute_struct _tmpAF7;struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmpAF6;a=(void*)((_tmpAF6=_cycalloc(sizeof(*_tmpAF6)),((_tmpAF6[0]=((_tmpAF7.tag=8,((_tmpAF7.f1=str,_tmpAF7)))),_tmpAF6))));}else{
const char*_tmpAF8;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpAF8="__mode__",_tag_dyneither(_tmpAF8,sizeof(char),9))))== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp52C);
struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct _tmpAFB;struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmpAFA;a=(void*)((_tmpAFA=_cycalloc(sizeof(*_tmpAFA)),((_tmpAFA[0]=((_tmpAFB.tag=24,((_tmpAFB.f1=str,_tmpAFB)))),_tmpAFA))));}else{
# 1595
int n=Cyc_Parse_exp2int((unsigned int)(yyyylsp[2]).first_line,_tmp52C);
const char*_tmpAFD;const char*_tmpAFC;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpAFD="regparm",_tag_dyneither(_tmpAFD,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpAFC="__regparm__",_tag_dyneither(_tmpAFC,sizeof(char),12))))== 0){
if(n < 0  || n > 3){
const char*_tmpAFE;Cyc_Parse_err(((_tmpAFE="regparm requires value between 0 and 3",_tag_dyneither(_tmpAFE,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct _tmpB01;struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmpB00;a=(void*)((_tmpB00=_cycalloc_atomic(sizeof(*_tmpB00)),((_tmpB00[0]=((_tmpB01.tag=0,((_tmpB01.f1=n,_tmpB01)))),_tmpB00))));};}else{
const char*_tmpB03;const char*_tmpB02;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpB03="initializes",_tag_dyneither(_tmpB03,sizeof(char),12))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpB02="__initializes__",_tag_dyneither(_tmpB02,sizeof(char),16))))== 0){
struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct _tmpB06;struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmpB05;a=(void*)((_tmpB05=_cycalloc_atomic(sizeof(*_tmpB05)),((_tmpB05[0]=((_tmpB06.tag=20,((_tmpB06.f1=n,_tmpB06)))),_tmpB05))));}else{
const char*_tmpB08;const char*_tmpB07;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpB08="noliveunique",_tag_dyneither(_tmpB08,sizeof(char),13))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpB07="__noliveunique__",_tag_dyneither(_tmpB07,sizeof(char),17))))== 0){
struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct _tmpB0B;struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmpB0A;a=(void*)((_tmpB0A=_cycalloc_atomic(sizeof(*_tmpB0A)),((_tmpB0A[0]=((_tmpB0B.tag=21,((_tmpB0B.f1=n,_tmpB0B)))),_tmpB0A))));}else{
const char*_tmpB0D;const char*_tmpB0C;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpB0D="noconsume",_tag_dyneither(_tmpB0D,sizeof(char),10))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpB0C="__noconsume__",_tag_dyneither(_tmpB0C,sizeof(char),14))))== 0){
struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct _tmpB10;struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmpB0F;a=(void*)((_tmpB0F=_cycalloc_atomic(sizeof(*_tmpB0F)),((_tmpB0F[0]=((_tmpB10.tag=22,((_tmpB10.f1=n,_tmpB10)))),_tmpB0F))));}else{
# 1607
{const char*_tmpB11;Cyc_Parse_err(((_tmpB11="unrecognized attribute",_tag_dyneither(_tmpB11,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}}}}}}
# 1611
yyval=Cyc_YY46(a);
# 1613
break;}case 70: _LL27E: {
# 1615
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 1617
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 1614 "parse.y"
struct _dyneither_ptr _tmp54A=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp54B=Cyc_yyget_String_tok(yyyyvsp[2]);
unsigned int _tmp54C=Cyc_Parse_cnst2uint((unsigned int)(yyyylsp[4]).first_line,Cyc_yyget_Int_tok(yyyyvsp[4]));
unsigned int _tmp54D=Cyc_Parse_cnst2uint((unsigned int)(yyyylsp[6]).first_line,Cyc_yyget_Int_tok(yyyyvsp[6]));
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;
{const char*_tmpB13;const char*_tmpB12;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp54A,((_tmpB13="format",_tag_dyneither(_tmpB13,sizeof(char),7))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp54A,((_tmpB12="__format__",_tag_dyneither(_tmpB12,sizeof(char),11))))== 0){
const char*_tmpB14;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp54B,((_tmpB14="printf",_tag_dyneither(_tmpB14,sizeof(char),7))))== 0){
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpB17;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpB16;a=(void*)((_tmpB16=_cycalloc_atomic(sizeof(*_tmpB16)),((_tmpB16[0]=((_tmpB17.tag=19,((_tmpB17.f1=Cyc_Absyn_Printf_ft,((_tmpB17.f2=(int)_tmp54C,((_tmpB17.f3=(int)_tmp54D,_tmpB17)))))))),_tmpB16))));}else{
const char*_tmpB18;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp54B,((_tmpB18="scanf",_tag_dyneither(_tmpB18,sizeof(char),6))))== 0){
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpB1B;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpB1A;a=(void*)((_tmpB1A=_cycalloc_atomic(sizeof(*_tmpB1A)),((_tmpB1A[0]=((_tmpB1B.tag=19,((_tmpB1B.f1=Cyc_Absyn_Scanf_ft,((_tmpB1B.f2=(int)_tmp54C,((_tmpB1B.f3=(int)_tmp54D,_tmpB1B)))))))),_tmpB1A))));}else{
# 1625
const char*_tmpB1C;Cyc_Parse_err(((_tmpB1C="unrecognized format type",_tag_dyneither(_tmpB1C,sizeof(char),25))),(unsigned int)(yyyylsp[2]).first_line);}}}else{
# 1627
const char*_tmpB1D;Cyc_Parse_err(((_tmpB1D="unrecognized attribute",_tag_dyneither(_tmpB1D,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY46(a);
# 1630
break;}case 71: _LL27F: {
# 1632
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1634
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1639 "parse.y"
yyval=yyyyvsp[0];
break;}case 72: _LL280: {
# 1642
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1644
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1641 "parse.y"
{struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpB20;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpB1F;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB1F=_cycalloc(sizeof(*_tmpB1F)),((_tmpB1F[0]=((_tmpB20.tag=17,((_tmpB20.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB20.f2=Cyc_yyget_YY40(yyyyvsp[1]),((_tmpB20.f3=0,((_tmpB20.f4=0,_tmpB20)))))))))),_tmpB1F)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 73: _LL281: {
# 1644
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1646
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1645 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)& Cyc_Absyn_VoidType_val,(unsigned int)(yyyylsp[0]).first_line));
break;}case 74: _LL282: {
# 1648
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1650
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1646 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 75: _LL283: {
# 1649
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1651
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1648 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY43(yyyyvsp[2])),0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 76: _LL284: {
# 1651
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1653
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1649 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_char_typ,(unsigned int)(yyyylsp[0]).first_line));
break;}case 77: _LL285: {
# 1652
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1654
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1650 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_short_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 78: _LL286: {
# 1653
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1655
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1651 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,(unsigned int)(yyyylsp[0]).first_line));
break;}case 79: _LL287: {
# 1654
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1656
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1652 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_long_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 80: _LL288: {
# 1655
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1657
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1653 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_float_typ(0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 81: _LL289: {
# 1656
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1658
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1654 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_float_typ(1),(unsigned int)(yyyylsp[0]).first_line));
break;}case 82: _LL28A: {
# 1657
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1659
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1655 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_signed_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 83: _LL28B: {
# 1658
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1660
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1656 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_unsigned_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 84: _LL28C: {
# 1659
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1661
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1657 "parse.y"
yyval=yyyyvsp[0];
break;}case 85: _LL28D: {
# 1660
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1662
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1658 "parse.y"
yyval=yyyyvsp[0];
break;}case 86: _LL28E: {
# 1661
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1663
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1660 "parse.y"
yyval=yyyyvsp[0];
break;}case 87: _LL28F: {
# 1663
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1665
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1662 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_yyget_YY44(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 88: _LL290: {
# 1665
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1667
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1665 "parse.y"
{struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpB23;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpB22;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB22=_cycalloc(sizeof(*_tmpB22)),((_tmpB22[0]=((_tmpB23.tag=10,((_tmpB23.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[2]).first_line,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[2]))),_tmpB23)))),_tmpB22)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1668
break;}case 89: _LL291: {
# 1670
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1672
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1669 "parse.y"
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpB26;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpB25;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB25=_cycalloc(sizeof(*_tmpB25)),((_tmpB25[0]=((_tmpB26.tag=15,((_tmpB26.f1=Cyc_yyget_YY44(yyyyvsp[2]),_tmpB26)))),_tmpB25)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 90: _LL292: {
# 1672
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1674
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1671 "parse.y"
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpB29;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpB28;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB28=_cycalloc(sizeof(*_tmpB28)),((_tmpB28[0]=((_tmpB29.tag=15,((_tmpB29.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0),_tmpB29)))),_tmpB28)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1673
break;}case 91: _LL293: {
# 1675
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1677
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1674 "parse.y"
void*_tmp560=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0);
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpB2C;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpB2B;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB2B=_cycalloc(sizeof(*_tmpB2B)),((_tmpB2B[0]=((_tmpB2C.tag=16,((_tmpB2C.f1=Cyc_yyget_YY44(yyyyvsp[2]),((_tmpB2C.f2=_tmp560,_tmpB2C)))))),_tmpB2B)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1677
break;}case 92: _LL294: {
# 1679
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1681
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1678 "parse.y"
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpB2F;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpB2E;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB2E=_cycalloc(sizeof(*_tmpB2E)),((_tmpB2E[0]=((_tmpB2F.tag=16,((_tmpB2F.f1=Cyc_yyget_YY44(yyyyvsp[2]),((_tmpB2F.f2=Cyc_yyget_YY44(yyyyvsp[4]),_tmpB2F)))))),_tmpB2E)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 93: _LL295: {
# 1681
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1683
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1680 "parse.y"
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpB32;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpB31;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB31=_cycalloc(sizeof(*_tmpB31)),((_tmpB31[0]=((_tmpB32.tag=19,((_tmpB32.f1=Cyc_yyget_YY44(yyyyvsp[2]),_tmpB32)))),_tmpB31)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 94: _LL296: {
# 1683
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1685
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1682 "parse.y"
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpB35;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpB34;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB34=_cycalloc(sizeof(*_tmpB34)),((_tmpB34[0]=((_tmpB35.tag=19,((_tmpB35.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_iko,0),_tmpB35)))),_tmpB34)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 95: _LL297: {
# 1685
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1687
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1684 "parse.y"
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpB38;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpB37;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB37=_cycalloc(sizeof(*_tmpB37)),((_tmpB37[0]=((_tmpB38.tag=18,((_tmpB38.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),_tmpB38)))),_tmpB37)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 96: _LL298: {
# 1687
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1689
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1689 "parse.y"
yyval=Cyc_YY43(Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 97: _LL299: {
# 1692
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1694
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1693 "parse.y"
unsigned int loc=(unsigned int)(Cyc_Absyn_porting_c_code?(yyyylsp[0]).first_line:(int)0);
{struct Cyc_Absyn_Tqual _tmpB39;yyval=Cyc_YY23(((_tmpB39.print_const=1,((_tmpB39.q_volatile=0,((_tmpB39.q_restrict=0,((_tmpB39.real_const=1,((_tmpB39.loc=loc,_tmpB39)))))))))));}
break;}case 98: _LL29A: {
# 1697
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1699
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1695 "parse.y"
{struct Cyc_Absyn_Tqual _tmpB3A;yyval=Cyc_YY23(((_tmpB3A.print_const=0,((_tmpB3A.q_volatile=1,((_tmpB3A.q_restrict=0,((_tmpB3A.real_const=0,((_tmpB3A.loc=0,_tmpB3A)))))))))));}
break;}case 99: _LL29B: {
# 1698
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1700
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1696 "parse.y"
{struct Cyc_Absyn_Tqual _tmpB3B;yyval=Cyc_YY23(((_tmpB3B.print_const=0,((_tmpB3B.q_volatile=0,((_tmpB3B.q_restrict=1,((_tmpB3B.real_const=0,((_tmpB3B.loc=0,_tmpB3B)))))))))));}
break;}case 100: _LL29C: {
# 1699
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1701
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpB4A;struct Cyc_Absyn_Enumdecl*_tmpB49;struct Cyc_Core_Opt*_tmpB48;struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct _tmpB47;struct Cyc_Absyn_TypeDecl*_tmpB46;struct Cyc_Absyn_TypeDecl*_tmp56E=(_tmpB46=_cycalloc(sizeof(*_tmpB46)),((_tmpB46->r=(void*)((_tmpB4A=_cycalloc(sizeof(*_tmpB4A)),((_tmpB4A[0]=((_tmpB47.tag=1,((_tmpB47.f1=((_tmpB49=_cycalloc(sizeof(*_tmpB49)),((_tmpB49->sc=Cyc_Absyn_Public,((_tmpB49->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB49->fields=((_tmpB48=_cycalloc(sizeof(*_tmpB48)),((_tmpB48->v=Cyc_yyget_YY48(yyyyvsp[3]),_tmpB48)))),_tmpB49)))))))),_tmpB47)))),_tmpB4A)))),((_tmpB46->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpB46)))));
# 1704
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB4D;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB4C;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB4C=_cycalloc(sizeof(*_tmpB4C)),((_tmpB4C[0]=((_tmpB4D.tag=26,((_tmpB4D.f1=_tmp56E,((_tmpB4D.f2=0,_tmpB4D)))))),_tmpB4C)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1706
break;}case 101: _LL29D: {
# 1708
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1710
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1707 "parse.y"
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpB50;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpB4F;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB4F=_cycalloc(sizeof(*_tmpB4F)),((_tmpB4F[0]=((_tmpB50.tag=13,((_tmpB50.f1=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB50.f2=0,_tmpB50)))))),_tmpB4F)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 102: _LL29E: {
# 1710
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1712
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1709 "parse.y"
{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpB53;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpB52;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB52=_cycalloc(sizeof(*_tmpB52)),((_tmpB52[0]=((_tmpB53.tag=14,((_tmpB53.f1=Cyc_yyget_YY48(yyyyvsp[2]),_tmpB53)))),_tmpB52)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 103: _LL29F: {
# 1712
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1714
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_Absyn_Enumfield*_tmpB54;yyval=Cyc_YY47(((_tmpB54=_cycalloc(sizeof(*_tmpB54)),((_tmpB54->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB54->tag=0,((_tmpB54->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpB54)))))))));}
break;}case 104: _LL2A0: {
# 1718
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1720
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1717 "parse.y"
{struct Cyc_Absyn_Enumfield*_tmpB55;yyval=Cyc_YY47(((_tmpB55=_cycalloc(sizeof(*_tmpB55)),((_tmpB55->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB55->tag=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpB55->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpB55)))))))));}
break;}case 105: _LL2A1: {
# 1720
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1722
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1721 "parse.y"
{struct Cyc_List_List*_tmpB56;yyval=Cyc_YY48(((_tmpB56=_cycalloc(sizeof(*_tmpB56)),((_tmpB56->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpB56->tl=0,_tmpB56)))))));}
break;}case 106: _LL2A2: {
# 1724
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1726
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1722 "parse.y"
{struct Cyc_List_List*_tmpB57;yyval=Cyc_YY48(((_tmpB57=_cycalloc(sizeof(*_tmpB57)),((_tmpB57->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpB57->tl=0,_tmpB57)))))));}
break;}case 107: _LL2A3: {
# 1725
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1727
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1723 "parse.y"
{struct Cyc_List_List*_tmpB58;yyval=Cyc_YY48(((_tmpB58=_cycalloc(sizeof(*_tmpB58)),((_tmpB58->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpB58->tl=Cyc_yyget_YY48(yyyyvsp[2]),_tmpB58)))))));}
break;}case 108: _LL2A4: {
# 1726
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1728
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpB5B;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpB5A;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB5A=_cycalloc(sizeof(*_tmpB5A)),((_tmpB5A[0]=((_tmpB5B.tag=12,((_tmpB5B.f1=Cyc_yyget_YY22(yyyyvsp[0]),((_tmpB5B.f2=Cyc_yyget_YY24(yyyyvsp[2]),_tmpB5B)))))),_tmpB5A)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 109: _LL2A5: {
# 1732
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 1734
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 1735 "parse.y"
struct Cyc_List_List*_tmp581=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[3]).first_line,Cyc_yyget_YY40(yyyyvsp[3]));
struct Cyc_List_List*_tmp582=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[5]).first_line,Cyc_yyget_YY40(yyyyvsp[5]));
struct Cyc_Absyn_TypeDecl*_tmp583=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY22(yyyyvsp[1]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[2]),_tmp581,
Cyc_Absyn_aggrdecl_impl(_tmp582,Cyc_yyget_YY50(yyyyvsp[6]),Cyc_yyget_YY24(yyyyvsp[7]),1),0,(unsigned int)(yyyylsp[0]).first_line);
# 1740
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB5E;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB5D;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB5D=_cycalloc(sizeof(*_tmpB5D)),((_tmpB5D[0]=((_tmpB5E.tag=26,((_tmpB5E.f1=_tmp583,((_tmpB5E.f2=0,_tmpB5E)))))),_tmpB5D)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1742
break;}case 110: _LL2A6: {
# 1744
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 1746
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 1746 "parse.y"
struct Cyc_List_List*_tmp586=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY40(yyyyvsp[2]));
struct Cyc_List_List*_tmp587=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[4]).first_line,Cyc_yyget_YY40(yyyyvsp[4]));
struct Cyc_Absyn_TypeDecl*_tmp588=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY22(yyyyvsp[0]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp586,
Cyc_Absyn_aggrdecl_impl(_tmp587,Cyc_yyget_YY50(yyyyvsp[5]),Cyc_yyget_YY24(yyyyvsp[6]),0),0,(unsigned int)(yyyylsp[0]).first_line);
# 1751
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB61;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB60;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB60=_cycalloc(sizeof(*_tmpB60)),((_tmpB60[0]=((_tmpB61.tag=26,((_tmpB61.f1=_tmp588,((_tmpB61.f2=0,_tmpB61)))))),_tmpB60)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1753
break;}case 111: _LL2A7: {
# 1755
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1757
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1754 "parse.y"
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB6B;struct Cyc_Core_Opt*_tmpB6A;struct Cyc_Absyn_AggrInfo _tmpB69;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB68;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB68=_cycalloc(sizeof(*_tmpB68)),((_tmpB68[0]=((_tmpB6B.tag=11,((_tmpB6B.f1=((_tmpB69.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY22(yyyyvsp[1]),Cyc_yyget_QualId_tok(yyyyvsp[2]),((_tmpB6A=_cycalloc_atomic(sizeof(*_tmpB6A)),((_tmpB6A->v=(void*)1,_tmpB6A))))),((_tmpB69.targs=Cyc_yyget_YY40(yyyyvsp[3]),_tmpB69)))),_tmpB6B)))),_tmpB68)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1757
break;}case 112: _LL2A8: {
# 1759
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1761
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1758 "parse.y"
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB71;struct Cyc_Absyn_AggrInfo _tmpB70;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB6F;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB6F=_cycalloc(sizeof(*_tmpB6F)),((_tmpB6F[0]=((_tmpB71.tag=11,((_tmpB71.f1=((_tmpB70.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY22(yyyyvsp[0]),Cyc_yyget_QualId_tok(yyyyvsp[1]),0),((_tmpB70.targs=Cyc_yyget_YY40(yyyyvsp[2]),_tmpB70)))),_tmpB71)))),_tmpB6F)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1761
break;}case 113: _LL2A9:
# 1763
 yyval=Cyc_YY40(0);
break;case 114: _LL2AA: {
# 1766
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1768
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1767 "parse.y"
yyval=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[1])));
break;}case 115: _LL2AB: {
# 1770
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1772
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1771 "parse.y"
yyval=Cyc_YY22(Cyc_Absyn_StructA);
break;}case 116: _LL2AC: {
# 1774
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1776
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1772 "parse.y"
yyval=Cyc_YY22(Cyc_Absyn_UnionA);
break;}case 117: _LL2AD:
# 1775
 yyval=Cyc_YY24(0);
break;case 118: _LL2AE: {
# 1778
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1780
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1779 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp592=Cyc_yyget_YY25(yyyyvsp[0]);for(0;_tmp592 != 0;_tmp592=_tmp592->tl){
decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)((struct Cyc_List_List*)_tmp592->hd,decls);}}{
# 1783
struct Cyc_List_List*_tmp593=Cyc_Parse_get_argrfield_tags(decls);
if(_tmp593 != 0)
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,_tmp593,decls);
yyval=Cyc_YY24(decls);
# 1788
break;};}case 119: _LL2AF: {
# 1790
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1792
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpB72;yyval=Cyc_YY25(((_tmpB72=_cycalloc(sizeof(*_tmpB72)),((_tmpB72->hd=Cyc_yyget_YY24(yyyyvsp[0]),((_tmpB72->tl=0,_tmpB72)))))));}
break;}case 120: _LL2B0: {
# 1796
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1798
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1795 "parse.y"
{struct Cyc_List_List*_tmpB73;yyval=Cyc_YY25(((_tmpB73=_cycalloc(sizeof(*_tmpB73)),((_tmpB73->hd=Cyc_yyget_YY24(yyyyvsp[1]),((_tmpB73->tl=Cyc_yyget_YY25(yyyyvsp[0]),_tmpB73)))))));}
break;}case 121: _LL2B1: {
# 1798
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1800
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1799 "parse.y"
yyval=Cyc_YY19(((struct _tuple12*(*)(struct _tuple12*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY19(yyyyvsp[0])));
break;}case 122: _LL2B2: {
# 1802
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1804
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct _tuple12*_tmpB74;yyval=Cyc_YY19(((_tmpB74=_region_malloc(yyr,sizeof(*_tmpB74)),((_tmpB74->tl=0,((_tmpB74->hd=Cyc_yyget_YY18(yyyyvsp[0]),_tmpB74)))))));}
break;}case 123: _LL2B3: {
# 1808
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1810
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1807 "parse.y"
{struct _tuple12*_tmpB75;yyval=Cyc_YY19(((_tmpB75=_region_malloc(yyr,sizeof(*_tmpB75)),((_tmpB75->tl=Cyc_yyget_YY19(yyyyvsp[0]),((_tmpB75->hd=Cyc_yyget_YY18(yyyyvsp[2]),_tmpB75)))))));}
break;}case 124: _LL2B4: {
# 1810
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1812
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1812 "parse.y"
{struct _tuple11 _tmpB76;yyval=Cyc_YY18(((_tmpB76.f1=Cyc_yyget_YY27(yyyyvsp[0]),((_tmpB76.f2=0,_tmpB76)))));}
break;}case 125: _LL2B5: {
# 1815
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1817
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1814 "parse.y"
{struct _tuple11 _tmpB77;yyval=Cyc_YY18(((_tmpB77.f1=Cyc_yyget_YY27(yyyyvsp[0]),((_tmpB77.f2=Cyc_yyget_Exp_tok(yyyyvsp[2]),_tmpB77)))));}
break;}case 126: _LL2B6: {
# 1817
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1819
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1820 "parse.y"
struct _RegionHandle _tmp59A=_new_region("temp");struct _RegionHandle*temp=& _tmp59A;_push_region(temp);{
struct _tuple25 _tmp59B=Cyc_yyget_YY35(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp59D;struct Cyc_Parse_Type_specifier _tmp59E;struct Cyc_List_List*_tmp59F;struct _tuple25 _tmp59C=_tmp59B;_tmp59D=_tmp59C.f1;_tmp59E=_tmp59C.f2;_tmp59F=_tmp59C.f3;
if(_tmp59D.loc == 0)_tmp59D.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct _tuple10*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*_tmp5A0=Cyc_yyget_YY29(yyyyvsp[1]);for(0;_tmp5A0 != 0;_tmp5A0=_tmp5A0->tl){
struct _tuple24*_tmp5A1=(struct _tuple24*)_tmp5A0->hd;struct Cyc_Parse_Declarator _tmp5A3;struct Cyc_Absyn_Exp*_tmp5A4;struct Cyc_Absyn_Exp*_tmp5A5;struct _tuple24*_tmp5A2=_tmp5A1;_tmp5A3=_tmp5A2->f1;_tmp5A4=_tmp5A2->f2;_tmp5A5=_tmp5A2->f3;
{struct _tuple10*_tmpB78;decls=((_tmpB78=_region_malloc(temp,sizeof(*_tmpB78)),((_tmpB78->tl=decls,((_tmpB78->hd=_tmp5A3,_tmpB78))))));}{
struct _tuple16*_tmpB7B;struct Cyc_List_List*_tmpB7A;widths_and_reqs=(
(_tmpB7A=_region_malloc(temp,sizeof(*_tmpB7A)),((_tmpB7A->hd=((_tmpB7B=_region_malloc(temp,sizeof(*_tmpB7B)),((_tmpB7B->f1=_tmp5A4,((_tmpB7B->f2=_tmp5A5,_tmpB7B)))))),((_tmpB7A->tl=widths_and_reqs,_tmpB7A))))));};}}
# 1831
decls=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(decls);
widths_and_reqs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(widths_and_reqs);{
void*_tmp5A9=Cyc_Parse_speclist2typ(_tmp59E,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp5AA=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,
Cyc_Parse_apply_tmss(temp,_tmp59D,_tmp5A9,decls,_tmp59F),widths_and_reqs);
# 1837
yyval=Cyc_YY24(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(unsigned int,struct _tuple17*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,(unsigned int)(yyyylsp[0]).first_line,_tmp5AA));};};}
# 1840
_npop_handler(0);break;
# 1820
;_pop_region(temp);}case 127: _LL2B7: {
# 1842
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1844
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1848 "parse.y"
{struct _tuple25 _tmpB7C;yyval=Cyc_YY35(((_tmpB7C.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB7C.f2=Cyc_yyget_YY21(yyyyvsp[0]),((_tmpB7C.f3=0,_tmpB7C)))))));}
break;}case 128: _LL2B8: {
# 1851
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1853
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1850 "parse.y"
struct _tuple25 _tmp5AC=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple25 _tmpB7D;yyval=Cyc_YY35(((_tmpB7D.f1=_tmp5AC.f1,((_tmpB7D.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY21(yyyyvsp[0]),_tmp5AC.f2),((_tmpB7D.f3=_tmp5AC.f3,_tmpB7D)))))));}
break;}case 129: _LL2B9: {
# 1853
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1855
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1852 "parse.y"
{struct _tuple25 _tmpB7E;yyval=Cyc_YY35(((_tmpB7E.f1=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpB7E.f2=Cyc_Parse_empty_spec(0),((_tmpB7E.f3=0,_tmpB7E)))))));}
break;}case 130: _LL2BA: {
# 1855
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1857
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1854 "parse.y"
struct _tuple25 _tmp5AF=Cyc_yyget_YY35(yyyyvsp[1]);
{struct _tuple25 _tmpB7F;yyval=Cyc_YY35(((_tmpB7F.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY23(yyyyvsp[0]),_tmp5AF.f1),((_tmpB7F.f2=_tmp5AF.f2,((_tmpB7F.f3=_tmp5AF.f3,_tmpB7F)))))));}
break;}case 131: _LL2BB: {
# 1858
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1860
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1857 "parse.y"
{struct _tuple25 _tmpB80;yyval=Cyc_YY35(((_tmpB80.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB80.f2=Cyc_Parse_empty_spec(0),((_tmpB80.f3=Cyc_yyget_YY45(yyyyvsp[0]),_tmpB80)))))));}
break;}case 132: _LL2BC: {
# 1860
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1862
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1859 "parse.y"
struct _tuple25 _tmp5B2=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple25 _tmpB81;yyval=Cyc_YY35(((_tmpB81.f1=_tmp5B2.f1,((_tmpB81.f2=_tmp5B2.f2,((_tmpB81.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY45(yyyyvsp[0]),_tmp5B2.f3),_tmpB81)))))));}
break;}case 133: _LL2BD: {
# 1862
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1864
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct _tuple25 _tmpB82;yyval=Cyc_YY35(((_tmpB82.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB82.f2=Cyc_yyget_YY21(yyyyvsp[0]),((_tmpB82.f3=0,_tmpB82)))))));}
break;}case 134: _LL2BE: {
# 1868
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1870
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1867 "parse.y"
struct _tuple25 _tmp5B5=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple25 _tmpB83;yyval=Cyc_YY35(((_tmpB83.f1=_tmp5B5.f1,((_tmpB83.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY21(yyyyvsp[0]),_tmp5B5.f2),((_tmpB83.f3=_tmp5B5.f3,_tmpB83)))))));}
break;}case 135: _LL2BF: {
# 1870
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1872
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1869 "parse.y"
{struct _tuple25 _tmpB84;yyval=Cyc_YY35(((_tmpB84.f1=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpB84.f2=Cyc_Parse_empty_spec(0),((_tmpB84.f3=0,_tmpB84)))))));}
break;}case 136: _LL2C0: {
# 1872
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1874
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1871 "parse.y"
struct _tuple25 _tmp5B8=Cyc_yyget_YY35(yyyyvsp[1]);
{struct _tuple25 _tmpB85;yyval=Cyc_YY35(((_tmpB85.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY23(yyyyvsp[0]),_tmp5B8.f1),((_tmpB85.f2=_tmp5B8.f2,((_tmpB85.f3=_tmp5B8.f3,_tmpB85)))))));}
break;}case 137: _LL2C1: {
# 1875
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1877
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1874 "parse.y"
{struct _tuple25 _tmpB86;yyval=Cyc_YY35(((_tmpB86.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB86.f2=Cyc_Parse_empty_spec(0),((_tmpB86.f3=Cyc_yyget_YY45(yyyyvsp[0]),_tmpB86)))))));}
break;}case 138: _LL2C2: {
# 1877
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1879
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1876 "parse.y"
struct _tuple25 _tmp5BB=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple25 _tmpB87;yyval=Cyc_YY35(((_tmpB87.f1=_tmp5BB.f1,((_tmpB87.f2=_tmp5BB.f2,((_tmpB87.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY45(yyyyvsp[0]),_tmp5BB.f3),_tmpB87)))))));}
break;}case 139: _LL2C3: {
# 1879
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1881
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1880 "parse.y"
yyval=Cyc_YY29(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY29(yyyyvsp[0])));
break;}case 140: _LL2C4: {
# 1883
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1885
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpB88;yyval=Cyc_YY29(((_tmpB88=_region_malloc(yyr,sizeof(*_tmpB88)),((_tmpB88->hd=Cyc_yyget_YY28(yyyyvsp[0]),((_tmpB88->tl=0,_tmpB88)))))));}
break;}case 141: _LL2C5: {
# 1889
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1891
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1888 "parse.y"
{struct Cyc_List_List*_tmpB89;yyval=Cyc_YY29(((_tmpB89=_region_malloc(yyr,sizeof(*_tmpB89)),((_tmpB89->hd=Cyc_yyget_YY28(yyyyvsp[2]),((_tmpB89->tl=Cyc_yyget_YY29(yyyyvsp[0]),_tmpB89)))))));}
break;}case 142: _LL2C6: {
# 1891
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1893
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1893 "parse.y"
{struct _tuple24*_tmpB8A;yyval=Cyc_YY28(((_tmpB8A=_region_malloc(yyr,sizeof(*_tmpB8A)),((_tmpB8A->f1=Cyc_yyget_YY27(yyyyvsp[0]),((_tmpB8A->f2=0,((_tmpB8A->f3=Cyc_yyget_YY55(yyyyvsp[1]),_tmpB8A)))))))));}
break;}case 143: _LL2C7: {
# 1896
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1898
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1897 "parse.y"
{struct Cyc_Parse_Declarator _tmpB90;struct _tuple0*_tmpB8F;struct _tuple24*_tmpB8E;yyval=Cyc_YY28(((_tmpB8E=_region_malloc(yyr,sizeof(*_tmpB8E)),((_tmpB8E->f1=((_tmpB90.id=((_tmpB8F=_cycalloc(sizeof(*_tmpB8F)),((_tmpB8F->f1=Cyc_Absyn_Rel_n(0),((_tmpB8F->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpB8F)))))),((_tmpB90.tms=0,_tmpB90)))),((_tmpB8E->f2=Cyc_yyget_Exp_tok(yyyyvsp[1]),((_tmpB8E->f3=0,_tmpB8E)))))))));}
# 1899
break;}case 144: _LL2C8:
# 1903
{struct Cyc_Parse_Declarator _tmpB96;struct _tuple0*_tmpB95;struct _tuple24*_tmpB94;yyval=Cyc_YY28(((_tmpB94=_region_malloc(yyr,sizeof(*_tmpB94)),((_tmpB94->f1=((_tmpB96.id=((_tmpB95=_cycalloc(sizeof(*_tmpB95)),((_tmpB95->f1=Cyc_Absyn_Rel_n(0),((_tmpB95->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpB95)))))),((_tmpB96.tms=0,_tmpB96)))),((_tmpB94->f2=0,((_tmpB94->f3=0,_tmpB94)))))))));}
# 1905
break;case 145: _LL2C9: {
# 1907
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1909
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1905 "parse.y"
{struct _tuple24*_tmpB97;yyval=Cyc_YY28(((_tmpB97=_region_malloc(yyr,sizeof(*_tmpB97)),((_tmpB97->f1=Cyc_yyget_YY27(yyyyvsp[0]),((_tmpB97->f2=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpB97->f3=0,_tmpB97)))))))));}
break;}case 146: _LL2CA: {
# 1908
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1910
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1909 "parse.y"
yyval=Cyc_YY55(Cyc_yyget_Exp_tok(yyyyvsp[2]));
break;}case 147: _LL2CB:
# 1912
 yyval=Cyc_YY55(0);
break;case 148: _LL2CC: {
# 1915
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1917
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1914 "parse.y"
yyval=Cyc_YY55(Cyc_yyget_Exp_tok(yyyyvsp[2]));
break;}case 149: _LL2CD:
# 1917
 yyval=Cyc_YY55(0);
break;case 150: _LL2CE: {
# 1920
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1922
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1921 "parse.y"
int _tmp5C9=Cyc_yyget_YY31(yyyyvsp[0]);
struct Cyc_List_List*_tmp5CA=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY40(yyyyvsp[2]));
struct Cyc_Core_Opt*_tmpB98;struct Cyc_Absyn_TypeDecl*_tmp5CB=Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp5CA,((_tmpB98=_cycalloc(sizeof(*_tmpB98)),((_tmpB98->v=Cyc_yyget_YY34(yyyyvsp[4]),_tmpB98)))),_tmp5C9,(unsigned int)(yyyylsp[0]).first_line);
# 1925
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB9B;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB9A;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB9A=_cycalloc(sizeof(*_tmpB9A)),((_tmpB9A[0]=((_tmpB9B.tag=26,((_tmpB9B.f1=_tmp5CB,((_tmpB9B.f2=0,_tmpB9B)))))),_tmpB9A)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1927
break;}case 151: _LL2CF: {
# 1929
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1931
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1928 "parse.y"
int _tmp5CF=Cyc_yyget_YY31(yyyyvsp[0]);
{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpBA5;struct Cyc_Absyn_UnknownDatatypeInfo _tmpBA4;struct Cyc_Absyn_DatatypeInfo _tmpBA3;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpBA2;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpBA2=_cycalloc(sizeof(*_tmpBA2)),((_tmpBA2[0]=((_tmpBA5.tag=3,((_tmpBA5.f1=((_tmpBA3.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpBA4.name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpBA4.is_extensible=_tmp5CF,_tmpBA4))))),((_tmpBA3.targs=Cyc_yyget_YY40(yyyyvsp[2]),_tmpBA3)))),_tmpBA5)))),_tmpBA2)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1931
break;}case 152: _LL2D0: {
# 1933
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1935
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1932 "parse.y"
int _tmp5D4=Cyc_yyget_YY31(yyyyvsp[0]);
{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpBAF;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmpBAE;struct Cyc_Absyn_DatatypeFieldInfo _tmpBAD;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpBAC;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpBAC=_cycalloc(sizeof(*_tmpBAC)),((_tmpBAC[0]=((_tmpBAF.tag=4,((_tmpBAF.f1=((_tmpBAD.field_info=
Cyc_Absyn_UnknownDatatypefield(((_tmpBAE.datatype_name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpBAE.field_name=Cyc_yyget_QualId_tok(yyyyvsp[3]),((_tmpBAE.is_extensible=_tmp5D4,_tmpBAE))))))),((_tmpBAD.targs=Cyc_yyget_YY40(yyyyvsp[4]),_tmpBAD)))),_tmpBAF)))),_tmpBAC)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1937
break;}case 153: _LL2D1: {
# 1939
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1941
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1940 "parse.y"
yyval=Cyc_YY31(0);
break;}case 154: _LL2D2: {
# 1943
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1945
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1941 "parse.y"
yyval=Cyc_YY31(1);
break;}case 155: _LL2D3: {
# 1944
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1946
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1945 "parse.y"
{struct Cyc_List_List*_tmpBB0;yyval=Cyc_YY34(((_tmpBB0=_cycalloc(sizeof(*_tmpBB0)),((_tmpBB0->hd=Cyc_yyget_YY33(yyyyvsp[0]),((_tmpBB0->tl=0,_tmpBB0)))))));}
break;}case 156: _LL2D4: {
# 1948
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1950
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1946 "parse.y"
{struct Cyc_List_List*_tmpBB1;yyval=Cyc_YY34(((_tmpBB1=_cycalloc(sizeof(*_tmpBB1)),((_tmpBB1->hd=Cyc_yyget_YY33(yyyyvsp[0]),((_tmpBB1->tl=0,_tmpBB1)))))));}
break;}case 157: _LL2D5: {
# 1949
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1951
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1947 "parse.y"
{struct Cyc_List_List*_tmpBB2;yyval=Cyc_YY34(((_tmpBB2=_cycalloc(sizeof(*_tmpBB2)),((_tmpBB2->hd=Cyc_yyget_YY33(yyyyvsp[0]),((_tmpBB2->tl=Cyc_yyget_YY34(yyyyvsp[2]),_tmpBB2)))))));}
break;}case 158: _LL2D6: {
# 1950
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1952
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1948 "parse.y"
{struct Cyc_List_List*_tmpBB3;yyval=Cyc_YY34(((_tmpBB3=_cycalloc(sizeof(*_tmpBB3)),((_tmpBB3->hd=Cyc_yyget_YY33(yyyyvsp[0]),((_tmpBB3->tl=Cyc_yyget_YY34(yyyyvsp[2]),_tmpBB3)))))));}
break;}case 159: _LL2D7:
# 1951
 yyval=Cyc_YY32(Cyc_Absyn_Public);
break;case 160: _LL2D8: {
# 1954
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1956
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1953 "parse.y"
yyval=Cyc_YY32(Cyc_Absyn_Extern);
break;}case 161: _LL2D9: {
# 1956
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1958
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1954 "parse.y"
yyval=Cyc_YY32(Cyc_Absyn_Static);
break;}case 162: _LL2DA: {
# 1957
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1959
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1958 "parse.y"
{struct Cyc_Absyn_Datatypefield*_tmpBB4;yyval=Cyc_YY33(((_tmpBB4=_cycalloc(sizeof(*_tmpBB4)),((_tmpBB4->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpBB4->typs=0,((_tmpBB4->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpBB4->sc=Cyc_yyget_YY32(yyyyvsp[0]),_tmpBB4)))))))))));}
break;}case 163: _LL2DB: {
# 1961
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1963
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1960 "parse.y"
struct Cyc_List_List*_tmp5DE=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[3]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[3])));
{struct Cyc_Absyn_Datatypefield*_tmpBB5;yyval=Cyc_YY33(((_tmpBB5=_cycalloc(sizeof(*_tmpBB5)),((_tmpBB5->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpBB5->typs=_tmp5DE,((_tmpBB5->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpBB5->sc=Cyc_yyget_YY32(yyyyvsp[0]),_tmpBB5)))))))))));}
break;}case 164: _LL2DC: {
# 1964
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1966
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1966 "parse.y"
yyval=yyyyvsp[0];
break;}case 165: _LL2DD: {
# 1969
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1971
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1968 "parse.y"
struct Cyc_Parse_Declarator _tmp5E0=Cyc_yyget_YY27(yyyyvsp[1]);
{struct Cyc_Parse_Declarator _tmpBB6;yyval=Cyc_YY27(((_tmpBB6.id=_tmp5E0.id,((_tmpBB6.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY26(yyyyvsp[0]),_tmp5E0.tms),_tmpBB6)))));}
break;}case 166: _LL2DE: {
# 1972
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1974
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=yyyyvsp[0];
break;}case 167: _LL2DF: {
# 1978
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1980
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1977 "parse.y"
struct Cyc_Parse_Declarator _tmp5E2=Cyc_yyget_YY27(yyyyvsp[1]);
{struct Cyc_Parse_Declarator _tmpBB7;yyval=Cyc_YY27(((_tmpBB7.id=_tmp5E2.id,((_tmpBB7.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY26(yyyyvsp[0]),_tmp5E2.tms),_tmpBB7)))));}
break;}case 168: _LL2E0: {
# 1981
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1983
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1983 "parse.y"
{struct Cyc_Parse_Declarator _tmpBB8;yyval=Cyc_YY27(((_tmpBB8.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpBB8.tms=0,_tmpBB8)))));}
break;}case 169: _LL2E1: {
# 1986
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1988
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1985 "parse.y"
yyval=yyyyvsp[1];
break;}case 170: _LL2E2: {
# 1988
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1990
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1989 "parse.y"
struct Cyc_Parse_Declarator _tmp5E5=Cyc_yyget_YY27(yyyyvsp[2]);
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpBBE;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpBBD;struct Cyc_List_List*_tmpBBC;_tmp5E5.tms=((_tmpBBC=_region_malloc(yyr,sizeof(*_tmpBBC)),((_tmpBBC->hd=(void*)((_tmpBBE=_region_malloc(yyr,sizeof(*_tmpBBE)),((_tmpBBE[0]=((_tmpBBD.tag=5,((_tmpBBD.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpBBD.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpBBD)))))),_tmpBBE)))),((_tmpBBC->tl=_tmp5E5.tms,_tmpBBC))))));}
yyval=yyyyvsp[2];
# 1993
break;}case 171: _LL2E3: {
# 1995
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1997
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1994 "parse.y"
{struct Cyc_List_List*_tmpBC8;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpBC7;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpBC6;struct Cyc_Parse_Declarator _tmpBC5;yyval=Cyc_YY27(((_tmpBC5.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpBC5.tms=((_tmpBC8=_region_malloc(yyr,sizeof(*_tmpBC8)),((_tmpBC8->hd=(void*)((_tmpBC6=_region_malloc(yyr,sizeof(*_tmpBC6)),((_tmpBC6[0]=((_tmpBC7.tag=0,((_tmpBC7.f1=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpBC7.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpBC7)))))),_tmpBC6)))),((_tmpBC8->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpBC8)))))),_tmpBC5)))));}
break;}case 172: _LL2E4: {
# 1997
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1999
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1996 "parse.y"
{struct Cyc_List_List*_tmpBD2;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpBD1;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpBD0;struct Cyc_Parse_Declarator _tmpBCF;yyval=Cyc_YY27(((_tmpBCF.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpBCF.tms=(
(_tmpBD2=_region_malloc(yyr,sizeof(*_tmpBD2)),((_tmpBD2->hd=(void*)((_tmpBD0=_region_malloc(yyr,sizeof(*_tmpBD0)),((_tmpBD0[0]=((_tmpBD1.tag=1,((_tmpBD1.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpBD1.f2=Cyc_yyget_YY51(yyyyvsp[4]),((_tmpBD1.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpBD1)))))))),_tmpBD0)))),((_tmpBD2->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpBD2)))))),_tmpBCF)))));}
break;}case 173: _LL2E5: {
# 2000
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2002
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1999 "parse.y"
struct _tuple26*_tmp5F1=Cyc_yyget_YY39(yyyyvsp[2]);struct Cyc_List_List*_tmp5F3;int _tmp5F4;struct Cyc_Absyn_VarargInfo*_tmp5F5;void*_tmp5F6;struct Cyc_List_List*_tmp5F7;struct _tuple26*_tmp5F2=_tmp5F1;_tmp5F3=_tmp5F2->f1;_tmp5F4=_tmp5F2->f2;_tmp5F5=_tmp5F2->f3;_tmp5F6=_tmp5F2->f4;_tmp5F7=_tmp5F2->f5;{
struct Cyc_Absyn_Exp*_tmp5F8=Cyc_yyget_YY55(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp5F9=Cyc_yyget_YY55(yyyyvsp[5]);
{struct Cyc_List_List*_tmpBE7;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBE6;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpBE5;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpBE4;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBE3;struct Cyc_Parse_Declarator _tmpBE2;yyval=Cyc_YY27(((_tmpBE2.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpBE2.tms=((_tmpBE7=_region_malloc(yyr,sizeof(*_tmpBE7)),((_tmpBE7->hd=(void*)((_tmpBE3=_region_malloc(yyr,sizeof(*_tmpBE3)),((_tmpBE3[0]=((_tmpBE6.tag=3,((_tmpBE6.f1=(void*)((_tmpBE4=_region_malloc(yyr,sizeof(*_tmpBE4)),((_tmpBE4[0]=((_tmpBE5.tag=1,((_tmpBE5.f1=_tmp5F3,((_tmpBE5.f2=_tmp5F4,((_tmpBE5.f3=_tmp5F5,((_tmpBE5.f4=_tmp5F6,((_tmpBE5.f5=_tmp5F7,((_tmpBE5.f6=_tmp5F8,((_tmpBE5.f7=_tmp5F9,_tmpBE5)))))))))))))))),_tmpBE4)))),_tmpBE6)))),_tmpBE3)))),((_tmpBE7->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpBE7)))))),_tmpBE2)))));}
# 2004
break;};}case 174: _LL2E6: {
# 2006
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2008
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2005 "parse.y"
{struct Cyc_List_List*_tmpBFC;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBFB;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpBFA;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpBF9;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBF8;struct Cyc_Parse_Declarator _tmpBF7;yyval=Cyc_YY27(((_tmpBF7.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpBF7.tms=(
(_tmpBFC=_region_malloc(yyr,sizeof(*_tmpBFC)),((_tmpBFC->hd=(void*)((_tmpBF8=_region_malloc(yyr,sizeof(*_tmpBF8)),((_tmpBF8[0]=((_tmpBFB.tag=3,((_tmpBFB.f1=(void*)((_tmpBF9=_region_malloc(yyr,sizeof(*_tmpBF9)),((_tmpBF9[0]=((_tmpBFA.tag=1,((_tmpBFA.f1=0,((_tmpBFA.f2=0,((_tmpBFA.f3=0,((_tmpBFA.f4=
# 2008
Cyc_yyget_YY49(yyyyvsp[2]),((_tmpBFA.f5=Cyc_yyget_YY50(yyyyvsp[3]),((_tmpBFA.f6=Cyc_yyget_YY55(yyyyvsp[5]),((_tmpBFA.f7=Cyc_yyget_YY55(yyyyvsp[6]),_tmpBFA)))))))))))))))),_tmpBF9)))),_tmpBFB)))),_tmpBF8)))),((_tmpBFC->tl=(
Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpBFC)))))),_tmpBF7)))));}
# 2011
break;}case 175: _LL2E7: {
# 2013
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2015
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2012 "parse.y"
{struct Cyc_List_List*_tmpC11;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC10;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpC0F;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpC0E;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC0D;struct Cyc_Parse_Declarator _tmpC0C;yyval=Cyc_YY27(((_tmpC0C.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpC0C.tms=((_tmpC11=_region_malloc(yyr,sizeof(*_tmpC11)),((_tmpC11->hd=(void*)((_tmpC0D=_region_malloc(yyr,sizeof(*_tmpC0D)),((_tmpC0D[0]=((_tmpC10.tag=3,((_tmpC10.f1=(void*)((_tmpC0E=_region_malloc(yyr,sizeof(*_tmpC0E)),((_tmpC0E[0]=((_tmpC0F.tag=0,((_tmpC0F.f1=Cyc_yyget_YY36(yyyyvsp[2]),((_tmpC0F.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpC0F)))))),_tmpC0E)))),_tmpC10)))),_tmpC0D)))),((_tmpC11->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpC11)))))),_tmpC0C)))));}
break;}case 176: _LL2E8: {
# 2015
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2017
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2015 "parse.y"
struct Cyc_List_List*_tmp60C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));
{struct Cyc_List_List*_tmpC1B;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpC1A;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpC19;struct Cyc_Parse_Declarator _tmpC18;yyval=Cyc_YY27(((_tmpC18.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpC18.tms=((_tmpC1B=_region_malloc(yyr,sizeof(*_tmpC1B)),((_tmpC1B->hd=(void*)((_tmpC19=_region_malloc(yyr,sizeof(*_tmpC19)),((_tmpC19[0]=((_tmpC1A.tag=4,((_tmpC1A.f1=_tmp60C,((_tmpC1A.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpC1A.f3=0,_tmpC1A)))))))),_tmpC19)))),((_tmpC1B->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpC1B)))))),_tmpC18)))));}
# 2018
break;}case 177: _LL2E9: {
# 2020
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2022
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2019 "parse.y"
{struct Cyc_List_List*_tmpC25;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC24;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC23;struct Cyc_Parse_Declarator _tmpC22;yyval=Cyc_YY27(((_tmpC22.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpC22.tms=((_tmpC25=_region_malloc(yyr,sizeof(*_tmpC25)),((_tmpC25->hd=(void*)((_tmpC23=_region_malloc(yyr,sizeof(*_tmpC23)),((_tmpC23[0]=((_tmpC24.tag=5,((_tmpC24.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpC24.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpC24)))))),_tmpC23)))),((_tmpC25->tl=(
Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpC25)))))),_tmpC22)))));}
# 2022
break;}case 178: _LL2EA: {
# 2024
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2026
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2033 "parse.y"
{struct Cyc_Parse_Declarator _tmpC26;yyval=Cyc_YY27(((_tmpC26.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpC26.tms=0,_tmpC26)))));}
break;}case 179: _LL2EB: {
# 2036
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2038
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2035 "parse.y"
{struct Cyc_Parse_Declarator _tmpC27;yyval=Cyc_YY27(((_tmpC27.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpC27.tms=0,_tmpC27)))));}
break;}case 180: _LL2EC: {
# 2038
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2040
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2037 "parse.y"
yyval=yyyyvsp[1];
break;}case 181: _LL2ED: {
# 2040
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2042
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2041 "parse.y"
struct Cyc_Parse_Declarator _tmp617=Cyc_yyget_YY27(yyyyvsp[2]);
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC2D;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC2C;struct Cyc_List_List*_tmpC2B;_tmp617.tms=((_tmpC2B=_region_malloc(yyr,sizeof(*_tmpC2B)),((_tmpC2B->hd=(void*)((_tmpC2D=_region_malloc(yyr,sizeof(*_tmpC2D)),((_tmpC2D[0]=((_tmpC2C.tag=5,((_tmpC2C.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpC2C.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpC2C)))))),_tmpC2D)))),((_tmpC2B->tl=_tmp617.tms,_tmpC2B))))));}
yyval=yyyyvsp[2];
# 2045
break;}case 182: _LL2EE: {
# 2047
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2049
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2046 "parse.y"
struct Cyc_Parse_Declarator _tmp61B=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC37;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpC36;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpC35;struct Cyc_Parse_Declarator _tmpC34;yyval=Cyc_YY27(((_tmpC34.id=_tmp61B.id,((_tmpC34.tms=(
(_tmpC37=_region_malloc(yyr,sizeof(*_tmpC37)),((_tmpC37->hd=(void*)((_tmpC35=_region_malloc(yyr,sizeof(*_tmpC35)),((_tmpC35[0]=((_tmpC36.tag=0,((_tmpC36.f1=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpC36.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpC36)))))),_tmpC35)))),((_tmpC37->tl=_tmp61B.tms,_tmpC37)))))),_tmpC34)))));}
break;}case 183: _LL2EF: {
# 2051
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2053
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2050 "parse.y"
struct Cyc_Parse_Declarator _tmp620=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC41;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpC40;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpC3F;struct Cyc_Parse_Declarator _tmpC3E;yyval=Cyc_YY27(((_tmpC3E.id=_tmp620.id,((_tmpC3E.tms=(
(_tmpC41=_region_malloc(yyr,sizeof(*_tmpC41)),((_tmpC41->hd=(void*)((_tmpC3F=_region_malloc(yyr,sizeof(*_tmpC3F)),((_tmpC3F[0]=((_tmpC40.tag=1,((_tmpC40.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpC40.f2=Cyc_yyget_YY51(yyyyvsp[4]),((_tmpC40.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpC40)))))))),_tmpC3F)))),((_tmpC41->tl=_tmp620.tms,_tmpC41)))))),_tmpC3E)))));}
# 2054
break;}case 184: _LL2F0: {
# 2056
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2058
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2055 "parse.y"
struct _tuple26*_tmp625=Cyc_yyget_YY39(yyyyvsp[2]);struct Cyc_List_List*_tmp627;int _tmp628;struct Cyc_Absyn_VarargInfo*_tmp629;void*_tmp62A;struct Cyc_List_List*_tmp62B;struct _tuple26*_tmp626=_tmp625;_tmp627=_tmp626->f1;_tmp628=_tmp626->f2;_tmp629=_tmp626->f3;_tmp62A=_tmp626->f4;_tmp62B=_tmp626->f5;{
struct Cyc_Absyn_Exp*_tmp62C=Cyc_yyget_YY55(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp62D=Cyc_yyget_YY55(yyyyvsp[5]);
struct Cyc_Parse_Declarator _tmp62E=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC56;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC55;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpC54;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpC53;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC52;struct Cyc_Parse_Declarator _tmpC51;yyval=Cyc_YY27(((_tmpC51.id=_tmp62E.id,((_tmpC51.tms=((_tmpC56=_region_malloc(yyr,sizeof(*_tmpC56)),((_tmpC56->hd=(void*)((_tmpC52=_region_malloc(yyr,sizeof(*_tmpC52)),((_tmpC52[0]=((_tmpC55.tag=3,((_tmpC55.f1=(void*)((_tmpC53=_region_malloc(yyr,sizeof(*_tmpC53)),((_tmpC53[0]=((_tmpC54.tag=1,((_tmpC54.f1=_tmp627,((_tmpC54.f2=_tmp628,((_tmpC54.f3=_tmp629,((_tmpC54.f4=_tmp62A,((_tmpC54.f5=_tmp62B,((_tmpC54.f6=_tmp62C,((_tmpC54.f7=_tmp62D,_tmpC54)))))))))))))))),_tmpC53)))),_tmpC55)))),_tmpC52)))),((_tmpC56->tl=_tmp62E.tms,_tmpC56)))))),_tmpC51)))));}
# 2061
break;};}case 185: _LL2F1: {
# 2063
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2065
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2062 "parse.y"
struct Cyc_Parse_Declarator _tmp635=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC6B;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC6A;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpC69;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpC68;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC67;struct Cyc_Parse_Declarator _tmpC66;yyval=Cyc_YY27(((_tmpC66.id=_tmp635.id,((_tmpC66.tms=(
(_tmpC6B=_region_malloc(yyr,sizeof(*_tmpC6B)),((_tmpC6B->hd=(void*)((_tmpC67=_region_malloc(yyr,sizeof(*_tmpC67)),((_tmpC67[0]=((_tmpC6A.tag=3,((_tmpC6A.f1=(void*)((_tmpC68=_region_malloc(yyr,sizeof(*_tmpC68)),((_tmpC68[0]=((_tmpC69.tag=1,((_tmpC69.f1=0,((_tmpC69.f2=0,((_tmpC69.f3=0,((_tmpC69.f4=
# 2066
Cyc_yyget_YY49(yyyyvsp[2]),((_tmpC69.f5=Cyc_yyget_YY50(yyyyvsp[3]),((_tmpC69.f6=Cyc_yyget_YY55(yyyyvsp[5]),((_tmpC69.f7=Cyc_yyget_YY55(yyyyvsp[6]),_tmpC69)))))))))))))))),_tmpC68)))),_tmpC6A)))),_tmpC67)))),((_tmpC6B->tl=_tmp635.tms,_tmpC6B)))))),_tmpC66)))));}
# 2069
break;}case 186: _LL2F2: {
# 2071
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2073
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2070 "parse.y"
struct Cyc_Parse_Declarator _tmp63C=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC80;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC7F;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpC7E;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpC7D;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC7C;struct Cyc_Parse_Declarator _tmpC7B;yyval=Cyc_YY27(((_tmpC7B.id=_tmp63C.id,((_tmpC7B.tms=((_tmpC80=_region_malloc(yyr,sizeof(*_tmpC80)),((_tmpC80->hd=(void*)((_tmpC7C=_region_malloc(yyr,sizeof(*_tmpC7C)),((_tmpC7C[0]=((_tmpC7F.tag=3,((_tmpC7F.f1=(void*)((_tmpC7D=_region_malloc(yyr,sizeof(*_tmpC7D)),((_tmpC7D[0]=((_tmpC7E.tag=0,((_tmpC7E.f1=Cyc_yyget_YY36(yyyyvsp[2]),((_tmpC7E.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpC7E)))))),_tmpC7D)))),_tmpC7F)))),_tmpC7C)))),((_tmpC80->tl=_tmp63C.tms,_tmpC80)))))),_tmpC7B)))));}
break;}case 187: _LL2F3: {
# 2074
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2076
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2074 "parse.y"
struct Cyc_List_List*_tmp643=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));
struct Cyc_Parse_Declarator _tmp644=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC8A;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpC89;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpC88;struct Cyc_Parse_Declarator _tmpC87;yyval=Cyc_YY27(((_tmpC87.id=_tmp644.id,((_tmpC87.tms=((_tmpC8A=_region_malloc(yyr,sizeof(*_tmpC8A)),((_tmpC8A->hd=(void*)((_tmpC88=_region_malloc(yyr,sizeof(*_tmpC88)),((_tmpC88[0]=((_tmpC89.tag=4,((_tmpC89.f1=_tmp643,((_tmpC89.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpC89.f3=0,_tmpC89)))))))),_tmpC88)))),((_tmpC8A->tl=_tmp644.tms,_tmpC8A)))))),_tmpC87)))));}
# 2078
break;}case 188: _LL2F4: {
# 2080
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2082
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2079 "parse.y"
struct Cyc_Parse_Declarator _tmp649=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC94;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC93;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC92;struct Cyc_Parse_Declarator _tmpC91;yyval=Cyc_YY27(((_tmpC91.id=_tmp649.id,((_tmpC91.tms=((_tmpC94=_region_malloc(yyr,sizeof(*_tmpC94)),((_tmpC94->hd=(void*)((_tmpC92=_region_malloc(yyr,sizeof(*_tmpC92)),((_tmpC92[0]=((_tmpC93.tag=5,((_tmpC93.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpC93.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpC93)))))),_tmpC92)))),((_tmpC94->tl=_tmp649.tms,_tmpC94)))))),_tmpC91)))));}
# 2082
break;}case 189: _LL2F5: {
# 2084
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2086
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2092 "parse.y"
yyval=yyyyvsp[0];
break;}case 190: _LL2F6: {
# 2095
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2097
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2093 "parse.y"
yyval=Cyc_YY26(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY26(yyyyvsp[0]),Cyc_yyget_YY26(yyyyvsp[1])));
break;}case 191: _LL2F7: {
# 2096
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2098
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2097 "parse.y"
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY45(yyyyvsp[3])!= 0){
struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC9A;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC99;struct Cyc_List_List*_tmpC98;ans=((_tmpC98=_region_malloc(yyr,sizeof(*_tmpC98)),((_tmpC98->hd=(void*)((_tmpC9A=_region_malloc(yyr,sizeof(*_tmpC9A)),((_tmpC9A[0]=((_tmpC99.tag=5,((_tmpC99.f1=(unsigned int)(yyyylsp[3]).first_line,((_tmpC99.f2=Cyc_yyget_YY45(yyyyvsp[3]),_tmpC99)))))),_tmpC9A)))),((_tmpC98->tl=ans,_tmpC98))))));}{
# 2101
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple21 _tmp651=*Cyc_yyget_YY1(yyyyvsp[0]);unsigned int _tmp653;union Cyc_Absyn_Constraint*_tmp654;union Cyc_Absyn_Constraint*_tmp655;struct _tuple21 _tmp652=_tmp651;_tmp653=_tmp652.f1;_tmp654=_tmp652.f2;_tmp655=_tmp652.f3;
if(Cyc_Absyn_porting_c_code){
struct Cyc_Absyn_PtrLoc*_tmpC9B;ptrloc=((_tmpC9B=_cycalloc_atomic(sizeof(*_tmpC9B)),((_tmpC9B->ptr_loc=_tmp653,((_tmpC9B->rgn_loc=(unsigned int)(yyyylsp[2]).first_line,((_tmpC9B->zt_loc=(unsigned int)(yyyylsp[1]).first_line,_tmpC9B))))))));}{
# 2106
struct _tuple14 _tmp657=Cyc_Parse_collapse_pointer_quals(_tmp653,_tmp654,_tmp655,Cyc_yyget_YY44(yyyyvsp[2]),Cyc_yyget_YY54(yyyyvsp[1]));union Cyc_Absyn_Constraint*_tmp659;union Cyc_Absyn_Constraint*_tmp65A;union Cyc_Absyn_Constraint*_tmp65B;void*_tmp65C;struct _tuple14 _tmp658=_tmp657;_tmp659=_tmp658.f1;_tmp65A=_tmp658.f2;_tmp65B=_tmp658.f3;_tmp65C=_tmp658.f4;
{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpCA5;struct Cyc_Absyn_PtrAtts _tmpCA4;struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmpCA3;struct Cyc_List_List*_tmpCA2;ans=((_tmpCA2=_region_malloc(yyr,sizeof(*_tmpCA2)),((_tmpCA2->hd=(void*)((_tmpCA5=_region_malloc(yyr,sizeof(*_tmpCA5)),((_tmpCA5[0]=((_tmpCA3.tag=2,((_tmpCA3.f1=((_tmpCA4.rgn=_tmp65C,((_tmpCA4.nullable=_tmp659,((_tmpCA4.bounds=_tmp65A,((_tmpCA4.zero_term=_tmp65B,((_tmpCA4.ptrloc=ptrloc,_tmpCA4)))))))))),((_tmpCA3.f2=Cyc_yyget_YY23(yyyyvsp[4]),_tmpCA3)))))),_tmpCA5)))),((_tmpCA2->tl=ans,_tmpCA2))))));}
yyval=Cyc_YY26(ans);
# 2110
break;};};}case 192: _LL2F8:
# 2112
 yyval=Cyc_YY54(0);
break;case 193: _LL2F9: {
# 2115
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2117
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2113 "parse.y"
{struct Cyc_List_List*_tmpCA6;yyval=Cyc_YY54(((_tmpCA6=_region_malloc(yyr,sizeof(*_tmpCA6)),((_tmpCA6->hd=Cyc_yyget_YY53(yyyyvsp[0]),((_tmpCA6->tl=Cyc_yyget_YY54(yyyyvsp[1]),_tmpCA6)))))));}
break;}case 194: _LL2FA: {
# 2116
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2118
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2118 "parse.y"
{struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct _tmpCA9;struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct*_tmpCA8;yyval=Cyc_YY53((void*)((_tmpCA8=_region_malloc(yyr,sizeof(*_tmpCA8)),((_tmpCA8[0]=((_tmpCA9.tag=0,((_tmpCA9.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),_tmpCA9)))),_tmpCA8)))));}
break;}case 195: _LL2FB: {
# 2121
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2123
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2120 "parse.y"
{struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct _tmpCAC;struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmpCAB;yyval=Cyc_YY53((void*)((_tmpCAB=_region_malloc(yyr,sizeof(*_tmpCAB)),((_tmpCAB[0]=((_tmpCAC.tag=1,((_tmpCAC.f1=Cyc_yyget_YY44(yyyyvsp[2]),_tmpCAC)))),_tmpCAB)))));}
break;}case 196: _LL2FC: {
# 2123
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2125
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2122 "parse.y"
{struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct _tmpCAF;struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct*_tmpCAE;yyval=Cyc_YY53((void*)((_tmpCAE=_region_malloc(yyr,sizeof(*_tmpCAE)),((_tmpCAE[0]=((_tmpCAF.tag=2,_tmpCAF)),_tmpCAE)))));}
break;}case 197: _LL2FD: {
# 2125
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2127
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2124 "parse.y"
{struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct _tmpCB2;struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct*_tmpCB1;yyval=Cyc_YY53((void*)((_tmpCB1=_region_malloc(yyr,sizeof(*_tmpCB1)),((_tmpCB1[0]=((_tmpCB2.tag=3,_tmpCB2)),_tmpCB1)))));}
break;}case 198: _LL2FE: {
# 2127
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2129
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2126 "parse.y"
{struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpCB5;struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpCB4;yyval=Cyc_YY53((void*)((_tmpCB4=_region_malloc(yyr,sizeof(*_tmpCB4)),((_tmpCB4[0]=((_tmpCB5.tag=4,_tmpCB5)),_tmpCB4)))));}
break;}case 199: _LL2FF: {
# 2129
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2131
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2128 "parse.y"
{struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpCB8;struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpCB7;yyval=Cyc_YY53((void*)((_tmpCB7=_region_malloc(yyr,sizeof(*_tmpCB7)),((_tmpCB7[0]=((_tmpCB8.tag=5,_tmpCB8)),_tmpCB7)))));}
break;}case 200: _LL300: {
# 2131
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2133
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2130 "parse.y"
{struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct _tmpCBB;struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct*_tmpCBA;yyval=Cyc_YY53((void*)((_tmpCBA=_region_malloc(yyr,sizeof(*_tmpCBA)),((_tmpCBA[0]=((_tmpCBB.tag=6,_tmpCBB)),_tmpCBA)))));}
break;}case 201: _LL301: {
# 2133
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2135
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2132 "parse.y"
{struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct _tmpCBE;struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct*_tmpCBD;yyval=Cyc_YY53((void*)((_tmpCBD=_region_malloc(yyr,sizeof(*_tmpCBD)),((_tmpCBD[0]=((_tmpCBE.tag=7,_tmpCBE)),_tmpCBD)))));}
break;}case 202: _LL302: {
# 2135
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2137
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2138 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpCBF;yyval=Cyc_YY1(((_tmpCBF=_cycalloc(sizeof(*_tmpCBF)),((_tmpCBF->f1=loc,((_tmpCBF->f2=Cyc_Absyn_true_conref,((_tmpCBF->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpCBF)))))))));}
# 2141
break;}case 203: _LL303: {
# 2143
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2145
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2142 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpCC0;yyval=Cyc_YY1(((_tmpCC0=_cycalloc(sizeof(*_tmpCC0)),((_tmpCC0->f1=loc,((_tmpCC0->f2=Cyc_Absyn_false_conref,((_tmpCC0->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpCC0)))))))));}
# 2145
break;}case 204: _LL304: {
# 2147
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2149
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2146 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpCC1;yyval=Cyc_YY1(((_tmpCC1=_cycalloc(sizeof(*_tmpCC1)),((_tmpCC1->f1=loc,((_tmpCC1->f2=Cyc_Absyn_true_conref,((_tmpCC1->f3=Cyc_Absyn_bounds_dyneither_conref,_tmpCC1)))))))));}
# 2149
break;}case 205: _LL305:
# 2151
 yyval=Cyc_YY2(Cyc_Absyn_bounds_one_conref);
break;case 206: _LL306: {
# 2154
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2156
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2152 "parse.y"
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpCC4;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpCC3;yyval=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmpCC3=_cycalloc(sizeof(*_tmpCC3)),((_tmpCC3[0]=((_tmpCC4.tag=1,((_tmpCC4.f1=Cyc_yyget_Exp_tok(yyyyvsp[1]),_tmpCC4)))),_tmpCC3))))));}
break;}case 207: _LL307:
# 2155
 yyval=Cyc_YY51(((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());
break;case 208: _LL308: {
# 2158
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2160
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2156 "parse.y"
yyval=Cyc_YY51(Cyc_Absyn_true_conref);
break;}case 209: _LL309: {
# 2159
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2161
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2157 "parse.y"
yyval=Cyc_YY51(Cyc_Absyn_false_conref);
break;}case 210: _LL30A:
# 2160
 yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
break;case 211: _LL30B: {
# 2163
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2165
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2163 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_trk,1);yyval=yyyyvsp[0];
break;}case 212: _LL30C: {
# 2166
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2168
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2164 "parse.y"
yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
break;}case 213: _LL30D:
# 2167
 yyval=Cyc_YY23(Cyc_Absyn_empty_tqual((unsigned int)(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset + 1))).first_line));
break;case 214: _LL30E: {
# 2170
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2172
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2169 "parse.y"
yyval=Cyc_YY23(Cyc_Absyn_combine_tqual(Cyc_yyget_YY23(yyyyvsp[0]),Cyc_yyget_YY23(yyyyvsp[1])));
break;}case 215: _LL30F: {
# 2172
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2174
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2174 "parse.y"
{struct _tuple26*_tmpCC5;yyval=Cyc_YY39(((_tmpCC5=_cycalloc(sizeof(*_tmpCC5)),((_tmpCC5->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0])),((_tmpCC5->f2=0,((_tmpCC5->f3=0,((_tmpCC5->f4=Cyc_yyget_YY49(yyyyvsp[1]),((_tmpCC5->f5=Cyc_yyget_YY50(yyyyvsp[2]),_tmpCC5)))))))))))));}
break;}case 216: _LL310: {
# 2177
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2179
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2176 "parse.y"
{struct _tuple26*_tmpCC6;yyval=Cyc_YY39(((_tmpCC6=_cycalloc(sizeof(*_tmpCC6)),((_tmpCC6->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0])),((_tmpCC6->f2=1,((_tmpCC6->f3=0,((_tmpCC6->f4=Cyc_yyget_YY49(yyyyvsp[3]),((_tmpCC6->f5=Cyc_yyget_YY50(yyyyvsp[4]),_tmpCC6)))))))))))));}
break;}case 217: _LL311: {
# 2179
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2181
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2179 "parse.y"
struct _tuple8*_tmp679=Cyc_yyget_YY37(yyyyvsp[2]);struct _dyneither_ptr*_tmp67B;struct Cyc_Absyn_Tqual _tmp67C;void*_tmp67D;struct _tuple8*_tmp67A=_tmp679;_tmp67B=_tmp67A->f1;_tmp67C=_tmp67A->f2;_tmp67D=_tmp67A->f3;{
struct Cyc_Absyn_VarargInfo*_tmpCC7;struct Cyc_Absyn_VarargInfo*_tmp67E=(_tmpCC7=_cycalloc(sizeof(*_tmpCC7)),((_tmpCC7->name=_tmp67B,((_tmpCC7->tq=_tmp67C,((_tmpCC7->type=_tmp67D,((_tmpCC7->inject=Cyc_yyget_YY31(yyyyvsp[1]),_tmpCC7)))))))));
{struct _tuple26*_tmpCC8;yyval=Cyc_YY39(((_tmpCC8=_cycalloc(sizeof(*_tmpCC8)),((_tmpCC8->f1=0,((_tmpCC8->f2=0,((_tmpCC8->f3=_tmp67E,((_tmpCC8->f4=Cyc_yyget_YY49(yyyyvsp[3]),((_tmpCC8->f5=Cyc_yyget_YY50(yyyyvsp[4]),_tmpCC8)))))))))))));}
# 2183
break;};}case 218: _LL312: {
# 2185
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2187
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2185 "parse.y"
struct _tuple8*_tmp681=Cyc_yyget_YY37(yyyyvsp[4]);struct _dyneither_ptr*_tmp683;struct Cyc_Absyn_Tqual _tmp684;void*_tmp685;struct _tuple8*_tmp682=_tmp681;_tmp683=_tmp682->f1;_tmp684=_tmp682->f2;_tmp685=_tmp682->f3;{
struct Cyc_Absyn_VarargInfo*_tmpCC9;struct Cyc_Absyn_VarargInfo*_tmp686=(_tmpCC9=_cycalloc(sizeof(*_tmpCC9)),((_tmpCC9->name=_tmp683,((_tmpCC9->tq=_tmp684,((_tmpCC9->type=_tmp685,((_tmpCC9->inject=Cyc_yyget_YY31(yyyyvsp[3]),_tmpCC9)))))))));
{struct _tuple26*_tmpCCA;yyval=Cyc_YY39(((_tmpCCA=_cycalloc(sizeof(*_tmpCCA)),((_tmpCCA->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0])),((_tmpCCA->f2=0,((_tmpCCA->f3=_tmp686,((_tmpCCA->f4=Cyc_yyget_YY49(yyyyvsp[5]),((_tmpCCA->f5=Cyc_yyget_YY50(yyyyvsp[6]),_tmpCCA)))))))))))));}
# 2189
break;};}case 219: _LL313: {
# 2191
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2193
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2193 "parse.y"
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpCCD;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpCCC;yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),(void*)((_tmpCCC=_cycalloc(sizeof(*_tmpCCC)),((_tmpCCC[0]=((_tmpCCD.tag=1,((_tmpCCD.f1=0,_tmpCCD)))),_tmpCCC))))));}
break;}case 220: _LL314: {
# 2196
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2198
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2194 "parse.y"
yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),Cyc_Tcutil_kind_to_bound(Cyc_yyget_YY43(yyyyvsp[2]))));
break;}case 221: _LL315:
# 2197
 yyval=Cyc_YY49(0);
break;case 222: _LL316: {
# 2200
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2202
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2198 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCD0;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCCF;yyval=Cyc_YY49((void*)((_tmpCCF=_cycalloc(sizeof(*_tmpCCF)),((_tmpCCF[0]=((_tmpCD0.tag=24,((_tmpCD0.f1=Cyc_yyget_YY40(yyyyvsp[1]),_tmpCD0)))),_tmpCCF)))));}
break;}case 223: _LL317:
# 2201
 yyval=Cyc_YY50(0);
break;case 224: _LL318: {
# 2204
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2206
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2203 "parse.y"
yyval=yyyyvsp[1];
break;}case 225: _LL319: {
# 2206
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2208
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2211 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpCD3;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCD2;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp68D=(_tmpCD2=_cycalloc(sizeof(*_tmpCD2)),((_tmpCD2[0]=((_tmpCD3.tag=2,((_tmpCD3.f1=0,((_tmpCD3.f2=& Cyc_Tcutil_trk,_tmpCD3)))))),_tmpCD2)));
struct _dyneither_ptr _tmp68E=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp68F=Cyc_Parse_id2type(_tmp68E,(void*)_tmp68D);
{struct _tuple29*_tmpCDD;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCDC;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCDB;struct Cyc_List_List*_tmpCDA;yyval=Cyc_YY50(((_tmpCDA=_cycalloc(sizeof(*_tmpCDA)),((_tmpCDA->hd=((_tmpCDD=_cycalloc(sizeof(*_tmpCDD)),((_tmpCDD->f1=(void*)((_tmpCDB=_cycalloc(sizeof(*_tmpCDB)),((_tmpCDB[0]=((_tmpCDC.tag=24,((_tmpCDC.f1=Cyc_yyget_YY40(yyyyvsp[0]),_tmpCDC)))),_tmpCDB)))),((_tmpCDD->f2=_tmp68F,_tmpCDD)))))),((_tmpCDA->tl=0,_tmpCDA)))))));}
# 2216
break;}case 226: _LL31A: {
# 2218
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2220
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2218 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpCE0;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCDF;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp696=(_tmpCDF=_cycalloc(sizeof(*_tmpCDF)),((_tmpCDF[0]=((_tmpCE0.tag=2,((_tmpCE0.f1=0,((_tmpCE0.f2=& Cyc_Tcutil_trk,_tmpCE0)))))),_tmpCDF)));
struct _dyneither_ptr _tmp697=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp698=Cyc_Parse_id2type(_tmp697,(void*)_tmp696);
{struct _tuple29*_tmpCEA;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCE9;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCE8;struct Cyc_List_List*_tmpCE7;yyval=Cyc_YY50(((_tmpCE7=_cycalloc(sizeof(*_tmpCE7)),((_tmpCE7->hd=((_tmpCEA=_cycalloc(sizeof(*_tmpCEA)),((_tmpCEA->f1=(void*)((_tmpCE8=_cycalloc(sizeof(*_tmpCE8)),((_tmpCE8[0]=((_tmpCE9.tag=24,((_tmpCE9.f1=Cyc_yyget_YY40(yyyyvsp[0]),_tmpCE9)))),_tmpCE8)))),((_tmpCEA->f2=_tmp698,_tmpCEA)))))),((_tmpCE7->tl=Cyc_yyget_YY50(yyyyvsp[4]),_tmpCE7)))))));}
# 2223
break;}case 227: _LL31B:
# 2225
 yyval=Cyc_YY31(0);
break;case 228: _LL31C: {
# 2228
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2230
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2229 "parse.y"
{const char*_tmpCEB;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]),((_tmpCEB="inject",_tag_dyneither(_tmpCEB,sizeof(char),7))))!= 0){
const char*_tmpCEC;Cyc_Parse_err(((_tmpCEC="missing type in function declaration",_tag_dyneither(_tmpCEC,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY31(1);
# 2233
break;}case 229: _LL31D: {
# 2235
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2237
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2236 "parse.y"
yyval=yyyyvsp[0];
break;}case 230: _LL31E: {
# 2239
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2241
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2237 "parse.y"
yyval=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY40(yyyyvsp[0]),Cyc_yyget_YY40(yyyyvsp[2])));
break;}case 231: _LL31F: {
# 2240
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2242
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2241 "parse.y"
yyval=Cyc_YY40(0);
break;}case 232: _LL320: {
# 2244
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2246
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2242 "parse.y"
yyval=yyyyvsp[1];
break;}case 233: _LL321: {
# 2245
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2247
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2244 "parse.y"
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpCF2;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpCF1;struct Cyc_List_List*_tmpCF0;yyval=Cyc_YY40(((_tmpCF0=_cycalloc(sizeof(*_tmpCF0)),((_tmpCF0->hd=(void*)((_tmpCF2=_cycalloc(sizeof(*_tmpCF2)),((_tmpCF2[0]=((_tmpCF1.tag=25,((_tmpCF1.f1=Cyc_yyget_YY44(yyyyvsp[2]),_tmpCF1)))),_tmpCF2)))),((_tmpCF0->tl=0,_tmpCF0)))))));}
break;}case 234: _LL322: {
# 2247
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2249
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2246 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_ek,0);
{struct Cyc_List_List*_tmpCF3;yyval=Cyc_YY40(((_tmpCF3=_cycalloc(sizeof(*_tmpCF3)),((_tmpCF3->hd=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpCF3->tl=0,_tmpCF3)))))));}
# 2249
break;}case 235: _LL323: {
# 2251
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2253
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2266 "parse.y"
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpCF9;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpCF8;struct Cyc_List_List*_tmpCF7;yyval=Cyc_YY40(((_tmpCF7=_cycalloc(sizeof(*_tmpCF7)),((_tmpCF7->hd=(void*)((_tmpCF9=_cycalloc(sizeof(*_tmpCF9)),((_tmpCF9[0]=((_tmpCF8.tag=23,((_tmpCF8.f1=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line),_tmpCF8)))),_tmpCF9)))),((_tmpCF7->tl=0,_tmpCF7)))))));}
break;}case 236: _LL324: {
# 2269
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2271
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2268 "parse.y"
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpCFF;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpCFE;struct Cyc_List_List*_tmpCFD;yyval=Cyc_YY40(((_tmpCFD=_cycalloc(sizeof(*_tmpCFD)),((_tmpCFD->hd=(void*)((_tmpCFF=_cycalloc(sizeof(*_tmpCFF)),((_tmpCFF[0]=((_tmpCFE.tag=23,((_tmpCFE.f1=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line),_tmpCFE)))),_tmpCFF)))),((_tmpCFD->tl=Cyc_yyget_YY40(yyyyvsp[2]),_tmpCFD)))))));}
break;}case 237: _LL325: {
# 2271
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2273
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpD00;yyval=Cyc_YY38(((_tmpD00=_cycalloc(sizeof(*_tmpD00)),((_tmpD00->hd=Cyc_yyget_YY37(yyyyvsp[0]),((_tmpD00->tl=0,_tmpD00)))))));}
break;}case 238: _LL326: {
# 2277
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2279
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2276 "parse.y"
{struct Cyc_List_List*_tmpD01;yyval=Cyc_YY38(((_tmpD01=_cycalloc(sizeof(*_tmpD01)),((_tmpD01->hd=Cyc_yyget_YY37(yyyyvsp[2]),((_tmpD01->tl=Cyc_yyget_YY38(yyyyvsp[0]),_tmpD01)))))));}
break;}case 239: _LL327: {
# 2279
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2281
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
struct _tuple25 _tmp6AD=Cyc_yyget_YY35(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp6AF;struct Cyc_Parse_Type_specifier _tmp6B0;struct Cyc_List_List*_tmp6B1;struct _tuple25 _tmp6AE=_tmp6AD;_tmp6AF=_tmp6AE.f1;_tmp6B0=_tmp6AE.f2;_tmp6B1=_tmp6AE.f3;
if(_tmp6AF.loc == 0)_tmp6AF.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct Cyc_Parse_Declarator _tmp6B2=Cyc_yyget_YY27(yyyyvsp[1]);struct _tuple0*_tmp6B4;struct Cyc_List_List*_tmp6B5;struct Cyc_Parse_Declarator _tmp6B3=_tmp6B2;_tmp6B4=_tmp6B3.id;_tmp6B5=_tmp6B3.tms;{
void*_tmp6B6=Cyc_Parse_speclist2typ(_tmp6B0,(unsigned int)(yyyylsp[0]).first_line);
struct _tuple13 _tmp6B7=Cyc_Parse_apply_tms(_tmp6AF,_tmp6B6,_tmp6B1,_tmp6B5);struct Cyc_Absyn_Tqual _tmp6B9;void*_tmp6BA;struct Cyc_List_List*_tmp6BB;struct Cyc_List_List*_tmp6BC;struct _tuple13 _tmp6B8=_tmp6B7;_tmp6B9=_tmp6B8.f1;_tmp6BA=_tmp6B8.f2;_tmp6BB=_tmp6B8.f3;_tmp6BC=_tmp6B8.f4;
if(_tmp6BB != 0){
const char*_tmpD02;Cyc_Parse_err(((_tmpD02="parameter with bad type params",_tag_dyneither(_tmpD02,sizeof(char),31))),(unsigned int)(yyyylsp[1]).first_line);}
if(Cyc_Absyn_is_qvar_qualified(_tmp6B4)){
const char*_tmpD03;Cyc_Parse_err(((_tmpD03="parameter cannot be qualified with a namespace",_tag_dyneither(_tmpD03,sizeof(char),47))),(unsigned int)(yyyylsp[0]).first_line);}{
struct _dyneither_ptr*idopt=(*_tmp6B4).f2;
if(_tmp6BC != 0){
const char*_tmpD06;void*_tmpD05;(_tmpD05=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpD06="extra attributes on parameter, ignoring",_tag_dyneither(_tmpD06,sizeof(char),40))),_tag_dyneither(_tmpD05,sizeof(void*),0)));}
{struct _tuple8*_tmpD07;yyval=Cyc_YY37(((_tmpD07=_cycalloc(sizeof(*_tmpD07)),((_tmpD07->f1=idopt,((_tmpD07->f2=_tmp6B9,((_tmpD07->f3=_tmp6BA,_tmpD07)))))))));}
# 2296
break;};};};}case 240: _LL328: {
# 2298
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2300
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2297 "parse.y"
struct _tuple25 _tmp6C2=Cyc_yyget_YY35(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp6C4;struct Cyc_Parse_Type_specifier _tmp6C5;struct Cyc_List_List*_tmp6C6;struct _tuple25 _tmp6C3=_tmp6C2;_tmp6C4=_tmp6C3.f1;_tmp6C5=_tmp6C3.f2;_tmp6C6=_tmp6C3.f3;
if(_tmp6C4.loc == 0)_tmp6C4.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp6C7=Cyc_Parse_speclist2typ(_tmp6C5,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp6C6 != 0){
const char*_tmpD0A;void*_tmpD09;(_tmpD09=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpD0A="bad attributes on parameter, ignoring",_tag_dyneither(_tmpD0A,sizeof(char),38))),_tag_dyneither(_tmpD09,sizeof(void*),0)));}
{struct _tuple8*_tmpD0B;yyval=Cyc_YY37(((_tmpD0B=_cycalloc(sizeof(*_tmpD0B)),((_tmpD0B->f1=0,((_tmpD0B->f2=_tmp6C4,((_tmpD0B->f3=_tmp6C7,_tmpD0B)))))))));}
# 2304
break;};}case 241: _LL329: {
# 2306
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2308
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2305 "parse.y"
struct _tuple25 _tmp6CB=Cyc_yyget_YY35(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp6CD;struct Cyc_Parse_Type_specifier _tmp6CE;struct Cyc_List_List*_tmp6CF;struct _tuple25 _tmp6CC=_tmp6CB;_tmp6CD=_tmp6CC.f1;_tmp6CE=_tmp6CC.f2;_tmp6CF=_tmp6CC.f3;
if(_tmp6CD.loc == 0)_tmp6CD.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp6D0=Cyc_Parse_speclist2typ(_tmp6CE,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp6D1=(Cyc_yyget_YY30(yyyyvsp[1])).tms;
struct _tuple13 _tmp6D2=Cyc_Parse_apply_tms(_tmp6CD,_tmp6D0,_tmp6CF,_tmp6D1);struct Cyc_Absyn_Tqual _tmp6D4;void*_tmp6D5;struct Cyc_List_List*_tmp6D6;struct Cyc_List_List*_tmp6D7;struct _tuple13 _tmp6D3=_tmp6D2;_tmp6D4=_tmp6D3.f1;_tmp6D5=_tmp6D3.f2;_tmp6D6=_tmp6D3.f3;_tmp6D7=_tmp6D3.f4;
if(_tmp6D6 != 0){
const char*_tmpD0E;void*_tmpD0D;(_tmpD0D=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpD0E="bad type parameters on formal argument, ignoring",_tag_dyneither(_tmpD0E,sizeof(char),49))),_tag_dyneither(_tmpD0D,sizeof(void*),0)));}
# 2313
if(_tmp6D7 != 0){
const char*_tmpD11;void*_tmpD10;(_tmpD10=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpD11="bad attributes on parameter, ignoring",_tag_dyneither(_tmpD11,sizeof(char),38))),_tag_dyneither(_tmpD10,sizeof(void*),0)));}
{struct _tuple8*_tmpD12;yyval=Cyc_YY37(((_tmpD12=_cycalloc(sizeof(*_tmpD12)),((_tmpD12->f1=0,((_tmpD12->f2=_tmp6D4,((_tmpD12->f3=_tmp6D5,_tmpD12)))))))));}
# 2317
break;};}case 242: _LL32A: {
# 2319
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2321
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2321 "parse.y"
yyval=Cyc_YY36(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyyyvsp[0])));
break;}case 243: _LL32B: {
# 2324
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2326
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2325 "parse.y"
{struct _dyneither_ptr*_tmpD15;struct Cyc_List_List*_tmpD14;yyval=Cyc_YY36(((_tmpD14=_cycalloc(sizeof(*_tmpD14)),((_tmpD14->hd=((_tmpD15=_cycalloc(sizeof(*_tmpD15)),((_tmpD15[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpD15)))),((_tmpD14->tl=0,_tmpD14)))))));}
break;}case 244: _LL32C: {
# 2328
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2330
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2327 "parse.y"
{struct _dyneither_ptr*_tmpD18;struct Cyc_List_List*_tmpD17;yyval=Cyc_YY36(((_tmpD17=_cycalloc(sizeof(*_tmpD17)),((_tmpD17->hd=((_tmpD18=_cycalloc(sizeof(*_tmpD18)),((_tmpD18[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpD18)))),((_tmpD17->tl=Cyc_yyget_YY36(yyyyvsp[0]),_tmpD17)))))));}
break;}case 245: _LL32D: {
# 2330
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2332
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2331 "parse.y"
yyval=yyyyvsp[0];
break;}case 246: _LL32E: {
# 2334
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2336
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2332 "parse.y"
yyval=yyyyvsp[0];
break;}case 247: _LL32F: {
# 2335
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2337
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2337 "parse.y"
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpD1B;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpD1A;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpD1A=_cycalloc(sizeof(*_tmpD1A)),((_tmpD1A[0]=((_tmpD1B.tag=35,((_tmpD1B.f1=0,((_tmpD1B.f2=0,_tmpD1B)))))),_tmpD1A)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 248: _LL330: {
# 2340
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2342
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2339 "parse.y"
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpD1E;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpD1D;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpD1D=_cycalloc(sizeof(*_tmpD1D)),((_tmpD1D[0]=((_tmpD1E.tag=35,((_tmpD1E.f1=0,((_tmpD1E.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[1])),_tmpD1E)))))),_tmpD1D)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 249: _LL331: {
# 2342
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2344
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2341 "parse.y"
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpD21;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpD20;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpD20=_cycalloc(sizeof(*_tmpD20)),((_tmpD20[0]=((_tmpD21.tag=35,((_tmpD21.f1=0,((_tmpD21.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[1])),_tmpD21)))))),_tmpD20)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 250: _LL332: {
# 2344
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2346
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2343 "parse.y"
struct _dyneither_ptr*_tmpD24;struct _tuple0*_tmpD23;struct Cyc_Absyn_Vardecl*_tmp6E7=Cyc_Absyn_new_vardecl(((_tmpD23=_cycalloc(sizeof(*_tmpD23)),((_tmpD23->f1=Cyc_Absyn_Loc_n,((_tmpD23->f2=((_tmpD24=_cycalloc(sizeof(*_tmpD24)),((_tmpD24[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpD24)))),_tmpD23)))))),Cyc_Absyn_uint_typ,
Cyc_Absyn_uint_exp(0,(unsigned int)(yyyylsp[2]).first_line));
# 2346
(_tmp6E7->tq).real_const=1;
{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpD27;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpD26;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpD26=_cycalloc(sizeof(*_tmpD26)),((_tmpD26[0]=((_tmpD27.tag=26,((_tmpD27.f1=_tmp6E7,((_tmpD27.f2=Cyc_yyget_Exp_tok(yyyyvsp[4]),((_tmpD27.f3=Cyc_yyget_Exp_tok(yyyyvsp[6]),((_tmpD27.f4=0,_tmpD27)))))))))),_tmpD26)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2349
break;}case 251: _LL333: {
# 2351
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2353
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2350 "parse.y"
void*_tmp6EC=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpD2A;struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpD29;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpD29=_cycalloc(sizeof(*_tmpD29)),((_tmpD29[0]=((_tmpD2A.tag=27,((_tmpD2A.f1=Cyc_yyget_Exp_tok(yyyyvsp[4]),((_tmpD2A.f2=_tmp6EC,((_tmpD2A.f3=0,_tmpD2A)))))))),_tmpD29)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2353
break;}case 252: _LL334: {
# 2355
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2357
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct _tuple30*_tmpD2D;struct Cyc_List_List*_tmpD2C;yyval=Cyc_YY5(((_tmpD2C=_cycalloc(sizeof(*_tmpD2C)),((_tmpD2C->hd=((_tmpD2D=_cycalloc(sizeof(*_tmpD2D)),((_tmpD2D->f1=0,((_tmpD2D->f2=Cyc_yyget_Exp_tok(yyyyvsp[0]),_tmpD2D)))))),((_tmpD2C->tl=0,_tmpD2C)))))));}
break;}case 253: _LL335: {
# 2361
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2363
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2360 "parse.y"
{struct _tuple30*_tmpD30;struct Cyc_List_List*_tmpD2F;yyval=Cyc_YY5(((_tmpD2F=_cycalloc(sizeof(*_tmpD2F)),((_tmpD2F->hd=((_tmpD30=_cycalloc(sizeof(*_tmpD30)),((_tmpD30->f1=Cyc_yyget_YY41(yyyyvsp[0]),((_tmpD30->f2=Cyc_yyget_Exp_tok(yyyyvsp[1]),_tmpD30)))))),((_tmpD2F->tl=0,_tmpD2F)))))));}
break;}case 254: _LL336: {
# 2363
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2365
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2362 "parse.y"
{struct _tuple30*_tmpD33;struct Cyc_List_List*_tmpD32;yyval=Cyc_YY5(((_tmpD32=_cycalloc(sizeof(*_tmpD32)),((_tmpD32->hd=((_tmpD33=_cycalloc(sizeof(*_tmpD33)),((_tmpD33->f1=0,((_tmpD33->f2=Cyc_yyget_Exp_tok(yyyyvsp[2]),_tmpD33)))))),((_tmpD32->tl=Cyc_yyget_YY5(yyyyvsp[0]),_tmpD32)))))));}
break;}case 255: _LL337: {
# 2365
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2367
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2364 "parse.y"
{struct _tuple30*_tmpD36;struct Cyc_List_List*_tmpD35;yyval=Cyc_YY5(((_tmpD35=_cycalloc(sizeof(*_tmpD35)),((_tmpD35->hd=((_tmpD36=_cycalloc(sizeof(*_tmpD36)),((_tmpD36->f1=Cyc_yyget_YY41(yyyyvsp[2]),((_tmpD36->f2=Cyc_yyget_Exp_tok(yyyyvsp[3]),_tmpD36)))))),((_tmpD35->tl=Cyc_yyget_YY5(yyyyvsp[0]),_tmpD35)))))));}
break;}case 256: _LL338: {
# 2367
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2369
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2368 "parse.y"
yyval=Cyc_YY41(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])));
break;}case 257: _LL339: {
# 2371
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2373
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2373 "parse.y"
{struct Cyc_List_List*_tmpD37;yyval=Cyc_YY41(((_tmpD37=_cycalloc(sizeof(*_tmpD37)),((_tmpD37->hd=Cyc_yyget_YY42(yyyyvsp[0]),((_tmpD37->tl=0,_tmpD37)))))));}
break;}case 258: _LL33A: {
# 2376
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2378
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2374 "parse.y"
{struct Cyc_List_List*_tmpD38;yyval=Cyc_YY41(((_tmpD38=_cycalloc(sizeof(*_tmpD38)),((_tmpD38->hd=Cyc_yyget_YY42(yyyyvsp[1]),((_tmpD38->tl=Cyc_yyget_YY41(yyyyvsp[0]),_tmpD38)))))));}
break;}case 259: _LL33B: {
# 2377
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2379
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2378 "parse.y"
{struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpD3B;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpD3A;yyval=Cyc_YY42((void*)((_tmpD3A=_cycalloc(sizeof(*_tmpD3A)),((_tmpD3A[0]=((_tmpD3B.tag=0,((_tmpD3B.f1=Cyc_yyget_Exp_tok(yyyyvsp[1]),_tmpD3B)))),_tmpD3A)))));}
break;}case 260: _LL33C: {
# 2381
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2383
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2379 "parse.y"
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpD41;struct _dyneither_ptr*_tmpD40;struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpD3F;yyval=Cyc_YY42((void*)((_tmpD3F=_cycalloc(sizeof(*_tmpD3F)),((_tmpD3F[0]=((_tmpD41.tag=1,((_tmpD41.f1=((_tmpD40=_cycalloc(sizeof(*_tmpD40)),((_tmpD40[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpD40)))),_tmpD41)))),_tmpD3F)))));}
break;}case 261: _LL33D: {
# 2382
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2384
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2384 "parse.y"
struct _tuple25 _tmp6FE=Cyc_yyget_YY35(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp700;struct Cyc_Parse_Type_specifier _tmp701;struct Cyc_List_List*_tmp702;struct _tuple25 _tmp6FF=_tmp6FE;_tmp700=_tmp6FF.f1;_tmp701=_tmp6FF.f2;_tmp702=_tmp6FF.f3;{
void*_tmp703=Cyc_Parse_speclist2typ(_tmp701,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp702 != 0){
const char*_tmpD44;void*_tmpD43;(_tmpD43=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpD44="ignoring attributes in type",_tag_dyneither(_tmpD44,sizeof(char),28))),_tag_dyneither(_tmpD43,sizeof(void*),0)));}
{struct _tuple8*_tmpD45;yyval=Cyc_YY37(((_tmpD45=_cycalloc(sizeof(*_tmpD45)),((_tmpD45->f1=0,((_tmpD45->f2=_tmp700,((_tmpD45->f3=_tmp703,_tmpD45)))))))));}
# 2390
break;};}case 262: _LL33E: {
# 2392
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2394
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2391 "parse.y"
struct _tuple25 _tmp707=Cyc_yyget_YY35(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp709;struct Cyc_Parse_Type_specifier _tmp70A;struct Cyc_List_List*_tmp70B;struct _tuple25 _tmp708=_tmp707;_tmp709=_tmp708.f1;_tmp70A=_tmp708.f2;_tmp70B=_tmp708.f3;{
void*_tmp70C=Cyc_Parse_speclist2typ(_tmp70A,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp70D=(Cyc_yyget_YY30(yyyyvsp[1])).tms;
struct _tuple13 _tmp70E=Cyc_Parse_apply_tms(_tmp709,_tmp70C,_tmp70B,_tmp70D);
if(_tmp70E.f3 != 0){
# 2397
const char*_tmpD48;void*_tmpD47;(_tmpD47=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpD48="bad type params, ignoring",_tag_dyneither(_tmpD48,sizeof(char),26))),_tag_dyneither(_tmpD47,sizeof(void*),0)));}
if(_tmp70E.f4 != 0){
const char*_tmpD4B;void*_tmpD4A;(_tmpD4A=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpD4B="bad specifiers, ignoring",_tag_dyneither(_tmpD4B,sizeof(char),25))),_tag_dyneither(_tmpD4A,sizeof(void*),0)));}
{struct _tuple8*_tmpD4C;yyval=Cyc_YY37(((_tmpD4C=_cycalloc(sizeof(*_tmpD4C)),((_tmpD4C->f1=0,((_tmpD4C->f2=_tmp70E.f1,((_tmpD4C->f3=_tmp70E.f2,_tmpD4C)))))))));}
# 2402
break;};}case 263: _LL33F: {
# 2404
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2406
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2405 "parse.y"
yyval=Cyc_YY44(Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 264: _LL340: {
# 2408
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2410
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2406 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD4F;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD4E;yyval=Cyc_YY44((void*)((_tmpD4E=_cycalloc(sizeof(*_tmpD4E)),((_tmpD4E[0]=((_tmpD4F.tag=24,((_tmpD4F.f1=0,_tmpD4F)))),_tmpD4E)))));}
break;}case 265: _LL341: {
# 2409
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2411
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2407 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD52;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD51;yyval=Cyc_YY44((void*)((_tmpD51=_cycalloc(sizeof(*_tmpD51)),((_tmpD51[0]=((_tmpD52.tag=24,((_tmpD52.f1=Cyc_yyget_YY40(yyyyvsp[1]),_tmpD52)))),_tmpD51)))));}
break;}case 266: _LL342: {
# 2410
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2412
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2408 "parse.y"
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpD55;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpD54;yyval=Cyc_YY44((void*)((_tmpD54=_cycalloc(sizeof(*_tmpD54)),((_tmpD54[0]=((_tmpD55.tag=25,((_tmpD55.f1=Cyc_yyget_YY44(yyyyvsp[2]),_tmpD55)))),_tmpD54)))));}
break;}case 267: _LL343: {
# 2411
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2413
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2409 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD5B;struct Cyc_List_List*_tmpD5A;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD59;yyval=Cyc_YY44((void*)((_tmpD59=_cycalloc(sizeof(*_tmpD59)),((_tmpD59[0]=((_tmpD5B.tag=24,((_tmpD5B.f1=((_tmpD5A=_cycalloc(sizeof(*_tmpD5A)),((_tmpD5A->hd=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpD5A->tl=Cyc_yyget_YY40(yyyyvsp[2]),_tmpD5A)))))),_tmpD5B)))),_tmpD59)))));}
break;}case 268: _LL344: {
# 2412
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2414
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpD5C;yyval=Cyc_YY40(((_tmpD5C=_cycalloc(sizeof(*_tmpD5C)),((_tmpD5C->hd=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpD5C->tl=0,_tmpD5C)))))));}
break;}case 269: _LL345: {
# 2418
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2420
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2416 "parse.y"
{struct Cyc_List_List*_tmpD5D;yyval=Cyc_YY40(((_tmpD5D=_cycalloc(sizeof(*_tmpD5D)),((_tmpD5D->hd=Cyc_yyget_YY44(yyyyvsp[2]),((_tmpD5D->tl=Cyc_yyget_YY40(yyyyvsp[0]),_tmpD5D)))))));}
break;}case 270: _LL346: {
# 2419
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2421
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2421 "parse.y"
{struct Cyc_Parse_Abstractdeclarator _tmpD5E;yyval=Cyc_YY30(((_tmpD5E.tms=Cyc_yyget_YY26(yyyyvsp[0]),_tmpD5E)));}
break;}case 271: _LL347: {
# 2424
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2426
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2423 "parse.y"
yyval=yyyyvsp[0];
break;}case 272: _LL348: {
# 2426
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2428
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2425 "parse.y"
{struct Cyc_Parse_Abstractdeclarator _tmpD5F;yyval=Cyc_YY30(((_tmpD5F.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY26(yyyyvsp[0]),(Cyc_yyget_YY30(yyyyvsp[1])).tms),_tmpD5F)));}
break;}case 273: _LL349: {
# 2428
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2430
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2430 "parse.y"
yyval=yyyyvsp[1];
break;}case 274: _LL34A: {
# 2433
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2435
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2432 "parse.y"
{struct Cyc_List_List*_tmpD69;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpD68;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpD67;struct Cyc_Parse_Abstractdeclarator _tmpD66;yyval=Cyc_YY30(((_tmpD66.tms=((_tmpD69=_region_malloc(yyr,sizeof(*_tmpD69)),((_tmpD69->hd=(void*)((_tmpD67=_region_malloc(yyr,sizeof(*_tmpD67)),((_tmpD67[0]=((_tmpD68.tag=0,((_tmpD68.f1=Cyc_yyget_YY51(yyyyvsp[2]),((_tmpD68.f2=(unsigned int)(yyyylsp[2]).first_line,_tmpD68)))))),_tmpD67)))),((_tmpD69->tl=0,_tmpD69)))))),_tmpD66)));}
break;}case 275: _LL34B: {
# 2435
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2437
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2434 "parse.y"
{struct Cyc_List_List*_tmpD73;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpD72;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpD71;struct Cyc_Parse_Abstractdeclarator _tmpD70;yyval=Cyc_YY30(((_tmpD70.tms=((_tmpD73=_region_malloc(yyr,sizeof(*_tmpD73)),((_tmpD73->hd=(void*)((_tmpD71=_region_malloc(yyr,sizeof(*_tmpD71)),((_tmpD71[0]=((_tmpD72.tag=0,((_tmpD72.f1=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpD72.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpD72)))))),_tmpD71)))),((_tmpD73->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpD73)))))),_tmpD70)));}
break;}case 276: _LL34C: {
# 2437
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2439
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2436 "parse.y"
{struct Cyc_List_List*_tmpD7D;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpD7C;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD7B;struct Cyc_Parse_Abstractdeclarator _tmpD7A;yyval=Cyc_YY30(((_tmpD7A.tms=((_tmpD7D=_region_malloc(yyr,sizeof(*_tmpD7D)),((_tmpD7D->hd=(void*)((_tmpD7B=_region_malloc(yyr,sizeof(*_tmpD7B)),((_tmpD7B[0]=((_tmpD7C.tag=1,((_tmpD7C.f1=Cyc_yyget_Exp_tok(yyyyvsp[1]),((_tmpD7C.f2=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpD7C.f3=(unsigned int)(yyyylsp[3]).first_line,_tmpD7C)))))))),_tmpD7B)))),((_tmpD7D->tl=0,_tmpD7D)))))),_tmpD7A)));}
break;}case 277: _LL34D: {
# 2439
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2441
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2438 "parse.y"
{struct Cyc_List_List*_tmpD87;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpD86;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD85;struct Cyc_Parse_Abstractdeclarator _tmpD84;yyval=Cyc_YY30(((_tmpD84.tms=((_tmpD87=_region_malloc(yyr,sizeof(*_tmpD87)),((_tmpD87->hd=(void*)((_tmpD85=_region_malloc(yyr,sizeof(*_tmpD85)),((_tmpD85[0]=((_tmpD86.tag=1,((_tmpD86.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpD86.f2=Cyc_yyget_YY51(yyyyvsp[4]),((_tmpD86.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpD86)))))))),_tmpD85)))),((_tmpD87->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpD87)))))),_tmpD84)));}
# 2441
break;}case 278: _LL34E: {
# 2443
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2445
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2442 "parse.y"
{struct Cyc_List_List*_tmpD9C;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD9B;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD9A;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD99;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD98;struct Cyc_Parse_Abstractdeclarator _tmpD97;yyval=Cyc_YY30(((_tmpD97.tms=((_tmpD9C=_region_malloc(yyr,sizeof(*_tmpD9C)),((_tmpD9C->hd=(void*)((_tmpD98=_region_malloc(yyr,sizeof(*_tmpD98)),((_tmpD98[0]=((_tmpD9B.tag=3,((_tmpD9B.f1=(void*)((_tmpD99=_region_malloc(yyr,sizeof(*_tmpD99)),((_tmpD99[0]=((_tmpD9A.tag=1,((_tmpD9A.f1=0,((_tmpD9A.f2=0,((_tmpD9A.f3=0,((_tmpD9A.f4=Cyc_yyget_YY49(yyyyvsp[1]),((_tmpD9A.f5=Cyc_yyget_YY50(yyyyvsp[2]),((_tmpD9A.f6=Cyc_yyget_YY55(yyyyvsp[4]),((_tmpD9A.f7=Cyc_yyget_YY55(yyyyvsp[5]),_tmpD9A)))))))))))))))),_tmpD99)))),_tmpD9B)))),_tmpD98)))),((_tmpD9C->tl=0,_tmpD9C)))))),_tmpD97)));}
# 2444
break;}case 279: _LL34F: {
# 2446
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2448
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2445 "parse.y"
struct _tuple26*_tmp737=Cyc_yyget_YY39(yyyyvsp[1]);struct Cyc_List_List*_tmp739;int _tmp73A;struct Cyc_Absyn_VarargInfo*_tmp73B;void*_tmp73C;struct Cyc_List_List*_tmp73D;struct _tuple26*_tmp738=_tmp737;_tmp739=_tmp738->f1;_tmp73A=_tmp738->f2;_tmp73B=_tmp738->f3;_tmp73C=_tmp738->f4;_tmp73D=_tmp738->f5;
{struct Cyc_List_List*_tmpDB1;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpDB0;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpDAF;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpDAE;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpDAD;struct Cyc_Parse_Abstractdeclarator _tmpDAC;yyval=Cyc_YY30(((_tmpDAC.tms=((_tmpDB1=_region_malloc(yyr,sizeof(*_tmpDB1)),((_tmpDB1->hd=(void*)((_tmpDAD=_region_malloc(yyr,sizeof(*_tmpDAD)),((_tmpDAD[0]=((_tmpDB0.tag=3,((_tmpDB0.f1=(void*)((_tmpDAE=_region_malloc(yyr,sizeof(*_tmpDAE)),((_tmpDAE[0]=((_tmpDAF.tag=1,((_tmpDAF.f1=_tmp739,((_tmpDAF.f2=_tmp73A,((_tmpDAF.f3=_tmp73B,((_tmpDAF.f4=_tmp73C,((_tmpDAF.f5=_tmp73D,((_tmpDAF.f6=Cyc_yyget_YY55(yyyyvsp[3]),((_tmpDAF.f7=Cyc_yyget_YY55(yyyyvsp[4]),_tmpDAF)))))))))))))))),_tmpDAE)))),_tmpDB0)))),_tmpDAD)))),((_tmpDB1->tl=0,_tmpDB1)))))),_tmpDAC)));}
# 2448
break;}case 280: _LL350: {
# 2450
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2452
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2449 "parse.y"
{struct Cyc_List_List*_tmpDC6;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpDC5;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpDC4;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpDC3;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpDC2;struct Cyc_Parse_Abstractdeclarator _tmpDC1;yyval=Cyc_YY30(((_tmpDC1.tms=((_tmpDC6=_region_malloc(yyr,sizeof(*_tmpDC6)),((_tmpDC6->hd=(void*)((_tmpDC2=_region_malloc(yyr,sizeof(*_tmpDC2)),((_tmpDC2[0]=((_tmpDC5.tag=3,((_tmpDC5.f1=(void*)((_tmpDC3=_region_malloc(yyr,sizeof(*_tmpDC3)),((_tmpDC3[0]=((_tmpDC4.tag=1,((_tmpDC4.f1=0,((_tmpDC4.f2=0,((_tmpDC4.f3=0,((_tmpDC4.f4=Cyc_yyget_YY49(yyyyvsp[2]),((_tmpDC4.f5=Cyc_yyget_YY50(yyyyvsp[3]),((_tmpDC4.f6=Cyc_yyget_YY55(yyyyvsp[5]),((_tmpDC4.f7=Cyc_yyget_YY55(yyyyvsp[6]),_tmpDC4)))))))))))))))),_tmpDC3)))),_tmpDC5)))),_tmpDC2)))),((_tmpDC6->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpDC6)))))),_tmpDC1)));}
# 2452
break;}case 281: _LL351: {
# 2454
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2456
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2453 "parse.y"
struct _tuple26*_tmp74A=Cyc_yyget_YY39(yyyyvsp[2]);struct Cyc_List_List*_tmp74C;int _tmp74D;struct Cyc_Absyn_VarargInfo*_tmp74E;void*_tmp74F;struct Cyc_List_List*_tmp750;struct _tuple26*_tmp74B=_tmp74A;_tmp74C=_tmp74B->f1;_tmp74D=_tmp74B->f2;_tmp74E=_tmp74B->f3;_tmp74F=_tmp74B->f4;_tmp750=_tmp74B->f5;
{struct Cyc_List_List*_tmpDDB;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpDDA;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpDD9;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpDD8;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpDD7;struct Cyc_Parse_Abstractdeclarator _tmpDD6;yyval=Cyc_YY30(((_tmpDD6.tms=((_tmpDDB=_region_malloc(yyr,sizeof(*_tmpDDB)),((_tmpDDB->hd=(void*)((_tmpDD7=_region_malloc(yyr,sizeof(*_tmpDD7)),((_tmpDD7[0]=((_tmpDDA.tag=3,((_tmpDDA.f1=(void*)((_tmpDD8=_region_malloc(yyr,sizeof(*_tmpDD8)),((_tmpDD8[0]=((_tmpDD9.tag=1,((_tmpDD9.f1=_tmp74C,((_tmpDD9.f2=_tmp74D,((_tmpDD9.f3=_tmp74E,((_tmpDD9.f4=_tmp74F,((_tmpDD9.f5=_tmp750,((_tmpDD9.f6=
Cyc_yyget_YY55(yyyyvsp[4]),((_tmpDD9.f7=Cyc_yyget_YY55(yyyyvsp[5]),_tmpDD9)))))))))))))))),_tmpDD8)))),_tmpDDA)))),_tmpDD7)))),((_tmpDDB->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpDDB)))))),_tmpDD6)));}
# 2457
break;}case 282: _LL352: {
# 2459
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2461
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2459 "parse.y"
struct Cyc_List_List*_tmp757=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));
{struct Cyc_List_List*_tmpDE5;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpDE4;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpDE3;struct Cyc_Parse_Abstractdeclarator _tmpDE2;yyval=Cyc_YY30(((_tmpDE2.tms=((_tmpDE5=_region_malloc(yyr,sizeof(*_tmpDE5)),((_tmpDE5->hd=(void*)((_tmpDE3=_region_malloc(yyr,sizeof(*_tmpDE3)),((_tmpDE3[0]=((_tmpDE4.tag=4,((_tmpDE4.f1=_tmp757,((_tmpDE4.f2=(unsigned int)(yyyylsp[1]).first_line,((_tmpDE4.f3=0,_tmpDE4)))))))),_tmpDE3)))),((_tmpDE5->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpDE5)))))),_tmpDE2)));}
# 2463
break;}case 283: _LL353: {
# 2465
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2467
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2464 "parse.y"
{struct Cyc_List_List*_tmpDEF;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpDEE;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpDED;struct Cyc_Parse_Abstractdeclarator _tmpDEC;yyval=Cyc_YY30(((_tmpDEC.tms=((_tmpDEF=_region_malloc(yyr,sizeof(*_tmpDEF)),((_tmpDEF->hd=(void*)((_tmpDED=_region_malloc(yyr,sizeof(*_tmpDED)),((_tmpDED[0]=((_tmpDEE.tag=5,((_tmpDEE.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpDEE.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpDEE)))))),_tmpDED)))),((_tmpDEF->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpDEF)))))),_tmpDEC)));}
# 2466
break;}case 284: _LL354: {
# 2468
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2470
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2470 "parse.y"
yyval=yyyyvsp[0];
break;}case 285: _LL355: {
# 2473
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2475
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2471 "parse.y"
yyval=yyyyvsp[0];
break;}case 286: _LL356: {
# 2474
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2476
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2472 "parse.y"
yyval=yyyyvsp[0];
break;}case 287: _LL357: {
# 2475
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2477
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2473 "parse.y"
yyval=yyyyvsp[0];
break;}case 288: _LL358: {
# 2476
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2478
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2474 "parse.y"
yyval=yyyyvsp[0];
break;}case 289: _LL359: {
# 2477
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2479
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2475 "parse.y"
yyval=yyyyvsp[0];
break;}case 290: _LL35A: {
# 2478
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2480
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2478 "parse.y"
{struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct _tmpDF2;struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmpDF1;yyval=Cyc_Stmt_tok(Cyc_Absyn_new_stmt((void*)((_tmpDF1=_cycalloc(sizeof(*_tmpDF1)),((_tmpDF1[0]=((_tmpDF2.tag=16,((_tmpDF2.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),_tmpDF2)))),_tmpDF1)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 291: _LL35B: {
# 2481
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2483
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpDF8;struct _dyneither_ptr*_tmpDF7;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpDF6;yyval=Cyc_Stmt_tok(Cyc_Absyn_new_stmt((void*)((_tmpDF6=_cycalloc(sizeof(*_tmpDF6)),((_tmpDF6[0]=((_tmpDF8.tag=13,((_tmpDF8.f1=((_tmpDF7=_cycalloc(sizeof(*_tmpDF7)),((_tmpDF7[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDF7)))),((_tmpDF8.f2=Cyc_yyget_Stmt_tok(yyyyvsp[2]),_tmpDF8)))))),_tmpDF6)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 292: _LL35C: {
# 2487
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2489
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2488 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 293: _LL35D: {
# 2491
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2493
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2489 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_exp_stmt(Cyc_yyget_Exp_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 294: _LL35E: {
# 2492
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2494
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2494 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 295: _LL35F: {
# 2497
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2499
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2495 "parse.y"
yyval=yyyyvsp[1];
break;}case 296: _LL360: {
# 2498
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2500
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2500 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(Cyc_yyget_YY16(yyyyvsp[0]),Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line)));
break;}case 297: _LL361: {
# 2503
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2505
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2501 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(Cyc_yyget_YY16(yyyyvsp[0]),Cyc_yyget_Stmt_tok(yyyyvsp[1])));
break;}case 298: _LL362: {
# 2504
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2506
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2502 "parse.y"
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpDFE;struct _dyneither_ptr*_tmpDFD;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpDFC;yyval=Cyc_Stmt_tok(Cyc_Absyn_new_stmt((void*)((_tmpDFC=_cycalloc(sizeof(*_tmpDFC)),((_tmpDFC[0]=((_tmpDFE.tag=13,((_tmpDFE.f1=((_tmpDFD=_cycalloc(sizeof(*_tmpDFD)),((_tmpDFD[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDFD)))),((_tmpDFE.f2=Cyc_Parse_flatten_declarations(Cyc_yyget_YY16(yyyyvsp[2]),Cyc_Absyn_skip_stmt(0)),_tmpDFE)))))),_tmpDFC)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2504
break;}case 299: _LL363: {
# 2506
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2508
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2504 "parse.y"
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpE04;struct _dyneither_ptr*_tmpE03;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpE02;yyval=Cyc_Stmt_tok(Cyc_Absyn_new_stmt((void*)((_tmpE02=_cycalloc(sizeof(*_tmpE02)),((_tmpE02[0]=((_tmpE04.tag=13,((_tmpE04.f1=((_tmpE03=_cycalloc(sizeof(*_tmpE03)),((_tmpE03[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE03)))),((_tmpE04.f2=Cyc_Parse_flatten_declarations(Cyc_yyget_YY16(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[3])),_tmpE04)))))),_tmpE02)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2506
break;}case 300: _LL364: {
# 2508
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2510
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2506 "parse.y"
yyval=yyyyvsp[0];
break;}case 301: _LL365: {
# 2509
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2511
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2507 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_seq_stmt(Cyc_yyget_Stmt_tok(yyyyvsp[0]),Cyc_yyget_Stmt_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 302: _LL366: {
# 2510
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2512
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2508 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpE07;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpE06;yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpE06=_cycalloc(sizeof(*_tmpE06)),((_tmpE06[0]=((_tmpE07.tag=1,((_tmpE07.f1=Cyc_yyget_YY15(yyyyvsp[0]),_tmpE07)))),_tmpE06)))),(unsigned int)(yyyylsp[0]).first_line),
Cyc_Absyn_skip_stmt(0)));}
break;}case 303: _LL367: {
# 2512
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2514
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2511 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpE0A;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpE09;yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpE09=_cycalloc(sizeof(*_tmpE09)),((_tmpE09[0]=((_tmpE0A.tag=1,((_tmpE0A.f1=Cyc_yyget_YY15(yyyyvsp[0]),_tmpE0A)))),_tmpE09)))),(unsigned int)(yyyylsp[0]).first_line),Cyc_yyget_Stmt_tok(yyyyvsp[1])));}
break;}case 304: _LL368: {
# 2514
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2516
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2516 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[4]),Cyc_Absyn_skip_stmt(0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 305: _LL369: {
# 2519
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2521
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2518 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[4]),Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 306: _LL36A: {
# 2521
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2523
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
yyval=Cyc_Stmt_tok(Cyc_Absyn_switch_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_YY8(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 307: _LL36B: {
# 2527
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2529
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2527 "parse.y"
struct Cyc_Absyn_Exp*_tmp76F=Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Absyn_switch_stmt(_tmp76F,Cyc_yyget_YY8(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 308: _LL36C: {
# 2531
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2533
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2531 "parse.y"
struct Cyc_Absyn_Exp*_tmp770=Cyc_Absyn_tuple_exp(Cyc_yyget_YY4(yyyyvsp[3]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Absyn_switch_stmt(_tmp770,Cyc_yyget_YY8(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
# 2534
break;}case 309: _LL36D: {
# 2536
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2538
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2537 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_trycatch_stmt(Cyc_yyget_Stmt_tok(yyyyvsp[1]),Cyc_yyget_YY8(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 310: _LL36E:
# 2540
 yyval=Cyc_YY8(0);
break;case 311: _LL36F: {
# 2543
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2545
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2553 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpE0D;struct Cyc_List_List*_tmpE0C;yyval=Cyc_YY8(((_tmpE0C=_cycalloc(sizeof(*_tmpE0C)),((_tmpE0C->hd=((_tmpE0D=_cycalloc(sizeof(*_tmpE0D)),((_tmpE0D->pattern=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line),((_tmpE0D->pat_vars=0,((_tmpE0D->where_clause=0,((_tmpE0D->body=
Cyc_yyget_Stmt_tok(yyyyvsp[2]),((_tmpE0D->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpE0D)))))))))))),((_tmpE0C->tl=0,_tmpE0C)))))));}
# 2556
break;}case 312: _LL370: {
# 2558
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2560
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2557 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpE10;struct Cyc_List_List*_tmpE0F;yyval=Cyc_YY8(((_tmpE0F=_cycalloc(sizeof(*_tmpE0F)),((_tmpE0F->hd=((_tmpE10=_cycalloc(sizeof(*_tmpE10)),((_tmpE10->pattern=Cyc_yyget_YY9(yyyyvsp[1]),((_tmpE10->pat_vars=0,((_tmpE10->where_clause=0,((_tmpE10->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[2]).first_line),((_tmpE10->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpE10)))))))))))),((_tmpE0F->tl=
Cyc_yyget_YY8(yyyyvsp[3]),_tmpE0F)))))));}
break;}case 313: _LL371: {
# 2562
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2564
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2561 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpE13;struct Cyc_List_List*_tmpE12;yyval=Cyc_YY8(((_tmpE12=_cycalloc(sizeof(*_tmpE12)),((_tmpE12->hd=((_tmpE13=_cycalloc(sizeof(*_tmpE13)),((_tmpE13->pattern=Cyc_yyget_YY9(yyyyvsp[1]),((_tmpE13->pat_vars=0,((_tmpE13->where_clause=0,((_tmpE13->body=Cyc_yyget_Stmt_tok(yyyyvsp[3]),((_tmpE13->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpE13)))))))))))),((_tmpE12->tl=Cyc_yyget_YY8(yyyyvsp[4]),_tmpE12)))))));}
break;}case 314: _LL372: {
# 2564
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2566
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2563 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpE16;struct Cyc_List_List*_tmpE15;yyval=Cyc_YY8(((_tmpE15=_cycalloc(sizeof(*_tmpE15)),((_tmpE15->hd=((_tmpE16=_cycalloc(sizeof(*_tmpE16)),((_tmpE16->pattern=Cyc_yyget_YY9(yyyyvsp[1]),((_tmpE16->pat_vars=0,((_tmpE16->where_clause=Cyc_yyget_Exp_tok(yyyyvsp[3]),((_tmpE16->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[4]).first_line),((_tmpE16->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpE16)))))))))))),((_tmpE15->tl=
Cyc_yyget_YY8(yyyyvsp[5]),_tmpE15)))))));}
break;}case 315: _LL373: {
# 2568
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2570
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2567 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpE19;struct Cyc_List_List*_tmpE18;yyval=Cyc_YY8(((_tmpE18=_cycalloc(sizeof(*_tmpE18)),((_tmpE18->hd=((_tmpE19=_cycalloc(sizeof(*_tmpE19)),((_tmpE19->pattern=Cyc_yyget_YY9(yyyyvsp[1]),((_tmpE19->pat_vars=0,((_tmpE19->where_clause=Cyc_yyget_Exp_tok(yyyyvsp[3]),((_tmpE19->body=Cyc_yyget_Stmt_tok(yyyyvsp[5]),((_tmpE19->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpE19)))))))))))),((_tmpE18->tl=Cyc_yyget_YY8(yyyyvsp[6]),_tmpE18)))))));}
break;}case 316: _LL374: {
# 2570
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2572
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2574 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_while_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 317: _LL375: {
# 2577
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2579
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2578 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_do_stmt(Cyc_yyget_Stmt_tok(yyyyvsp[1]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 318: _LL376: {
# 2581
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2583
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2582 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 319: _LL377: {
# 2586
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2588
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2585 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[4]),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 320: _LL378: {
# 2589
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2591
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2588 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 321: _LL379: {
# 2592
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2594
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2591 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[3]),Cyc_yyget_Exp_tok(yyyyvsp[5]),
Cyc_yyget_Stmt_tok(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 322: _LL37A: {
# 2595
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2597
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2594 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 323: _LL37B: {
# 2598
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2600
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2597 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[5]),
Cyc_yyget_Stmt_tok(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 324: _LL37C: {
# 2601
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2603
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2600 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 325: _LL37D: {
# 2604
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 2606
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 2603 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),Cyc_yyget_Exp_tok(yyyyvsp[6]),
Cyc_yyget_Stmt_tok(yyyyvsp[8]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 326: _LL37E: {
# 2607
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2609
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2606 "parse.y"
struct Cyc_List_List*_tmp77B=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp77C=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp77B,_tmp77C));
# 2611
break;}case 327: _LL37F: {
# 2613
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2615
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2612 "parse.y"
struct Cyc_List_List*_tmp77D=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp77E=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp77D,_tmp77E));
# 2617
break;}case 328: _LL380: {
# 2619
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2621
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2618 "parse.y"
struct Cyc_List_List*_tmp77F=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp780=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[4]),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp77F,_tmp780));
# 2623
break;}case 329: _LL381: {
# 2625
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2627
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2624 "parse.y"
struct Cyc_List_List*_tmp781=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp782=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[3]),Cyc_yyget_Exp_tok(yyyyvsp[5]),
Cyc_yyget_Stmt_tok(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp781,_tmp782));
# 2629
break;}case 330: _LL382: {
# 2631
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2633
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
{struct _dyneither_ptr*_tmpE1A;yyval=Cyc_Stmt_tok(Cyc_Absyn_goto_stmt(((_tmpE1A=_cycalloc(sizeof(*_tmpE1A)),((_tmpE1A[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpE1A)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 331: _LL383: {
# 2637
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2639
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2635 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_continue_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 332: _LL384: {
# 2638
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2640
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2636 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_break_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 333: _LL385: {
# 2639
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2641
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2637 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 334: _LL386: {
# 2640
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2642
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2638 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 335: _LL387: {
# 2641
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2643
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2640 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 336: _LL388: {
# 2643
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2645
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2641 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 337: _LL389: {
# 2644
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2646
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2643 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 338: _LL38A: {
# 2646
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2648
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2652 "parse.y"
yyval=yyyyvsp[0];
break;}case 339: _LL38B: {
# 2655
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2657
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2655 "parse.y"
yyval=yyyyvsp[0];
break;}case 340: _LL38C: {
# 2658
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2660
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2657 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 341: _LL38D: {
# 2660
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2662
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2660 "parse.y"
yyval=yyyyvsp[0];
break;}case 342: _LL38E: {
# 2663
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2665
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2662 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 343: _LL38F: {
# 2665
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2667
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2665 "parse.y"
yyval=yyyyvsp[0];
break;}case 344: _LL390: {
# 2668
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2670
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2667 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 345: _LL391: {
# 2670
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2672
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2670 "parse.y"
yyval=yyyyvsp[0];
break;}case 346: _LL392: {
# 2673
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2675
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2672 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 347: _LL393: {
# 2675
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2677
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2675 "parse.y"
yyval=yyyyvsp[0];
break;}case 348: _LL394: {
# 2678
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2680
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2677 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 349: _LL395: {
# 2680
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2682
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2680 "parse.y"
yyval=yyyyvsp[0];
break;}case 350: _LL396: {
# 2683
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2685
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2682 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 351: _LL397: {
# 2685
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2687
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2685 "parse.y"
yyval=yyyyvsp[0];
break;}case 352: _LL398: {
# 2688
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2690
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2687 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 353: _LL399: {
# 2690
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2692
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2689 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 354: _LL39A: {
# 2692
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2694
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2692 "parse.y"
yyval=yyyyvsp[0];
break;}case 355: _LL39B: {
# 2695
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2697
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2694 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 356: _LL39C: {
# 2697
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2699
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2696 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 357: _LL39D: {
# 2699
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2701
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2698 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 358: _LL39E: {
# 2701
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2703
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2700 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 359: _LL39F: {
# 2703
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2705
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2703 "parse.y"
yyval=yyyyvsp[0];
break;}case 360: _LL3A0: {
# 2706
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2708
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2705 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 361: _LL3A1: {
# 2708
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2710
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2707 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 362: _LL3A2: {
# 2710
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2712
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2710 "parse.y"
yyval=yyyyvsp[0];
break;}case 363: _LL3A3: {
# 2713
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2715
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2712 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 364: _LL3A4: {
# 2715
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2717
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2714 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 365: _LL3A5: {
# 2717
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2719
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2717 "parse.y"
yyval=yyyyvsp[0];
break;}case 366: _LL3A6: {
# 2720
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2722
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2719 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 367: _LL3A7: {
# 2722
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2724
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2721 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 368: _LL3A8: {
# 2724
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2726
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2723 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 369: _LL3A9: {
# 2726
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2728
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2726 "parse.y"
yyval=yyyyvsp[0];
break;}case 370: _LL3AA: {
# 2729
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2731
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2728 "parse.y"
void*_tmp784=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp(_tmp784,Cyc_yyget_Exp_tok(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line)));
# 2731
break;}case 371: _LL3AB: {
# 2733
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2735
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2734 "parse.y"
yyval=yyyyvsp[0];
break;}case 372: _LL3AC: {
# 2737
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2739
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2737 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(yyyyvsp[1])));
break;}case 373: _LL3AD: {
# 2740
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2742
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2739 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim1_exp(Cyc_yyget_YY6(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 374: _LL3AE: {
# 2742
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2744
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2741 "parse.y"
void*_tmp785=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp(_tmp785,(unsigned int)(yyyylsp[0]).first_line)));
# 2744
break;}case 375: _LL3AF: {
# 2746
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2748
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2745 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 376: _LL3B0: {
# 2748
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2750
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2747 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyyyvsp[2])).f3,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4])),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 377: _LL3B1: {
# 2750
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2752
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2752 "parse.y"
yyval=yyyyvsp[0];
break;}case 378: _LL3B2: {
# 2755
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2757
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2761 "parse.y"
yyval=yyyyvsp[0];
break;}case 379: _LL3B3: {
# 2764
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2766
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2766 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line));
break;}case 380: _LL3B4: {
# 2769
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2771
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2768 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(yyyyvsp[1])));
break;}case 381: _LL3B5: {
# 2771
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2773
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2770 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(yyyyvsp[0]);
{void*_tmp786=e->r;void*_tmp787=_tmp786;enum Cyc_Absyn_Sign _tmp789;char _tmp78A;enum Cyc_Absyn_Sign _tmp78C;short _tmp78D;enum Cyc_Absyn_Sign _tmp78F;int _tmp790;struct _dyneither_ptr _tmp792;int _tmp793;_LL3B8: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp788=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp787;if(_tmp788->tag != 0)goto _LL3BA;else{if(((_tmp788->f1).Char_c).tag != 2)goto _LL3BA;_tmp789=((struct _tuple3)((_tmp788->f1).Char_c).val).f1;_tmp78A=((struct _tuple3)((_tmp788->f1).Char_c).val).f2;}}_LL3B9:
# 2774
{struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct _tmpE1D;struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmpE1C;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE1C=_cycalloc_atomic(sizeof(*_tmpE1C)),((_tmpE1C[0]=((_tmpE1D.tag=11,((_tmpE1D.f1=_tmp78A,_tmpE1D)))),_tmpE1C)))),e->loc));}goto _LL3B7;_LL3BA: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp78B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp787;if(_tmp78B->tag != 0)goto _LL3BC;else{if(((_tmp78B->f1).Short_c).tag != 4)goto _LL3BC;_tmp78C=((struct _tuple4)((_tmp78B->f1).Short_c).val).f1;_tmp78D=((struct _tuple4)((_tmp78B->f1).Short_c).val).f2;}}_LL3BB:
# 2776
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpE20;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpE1F;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE1F=_cycalloc_atomic(sizeof(*_tmpE1F)),((_tmpE1F[0]=((_tmpE20.tag=10,((_tmpE20.f1=_tmp78C,((_tmpE20.f2=(int)_tmp78D,_tmpE20)))))),_tmpE1F)))),e->loc));}goto _LL3B7;_LL3BC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp78E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp787;if(_tmp78E->tag != 0)goto _LL3BE;else{if(((_tmp78E->f1).Int_c).tag != 5)goto _LL3BE;_tmp78F=((struct _tuple5)((_tmp78E->f1).Int_c).val).f1;_tmp790=((struct _tuple5)((_tmp78E->f1).Int_c).val).f2;}}_LL3BD:
# 2778
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpE23;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpE22;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE22=_cycalloc_atomic(sizeof(*_tmpE22)),((_tmpE22[0]=((_tmpE23.tag=10,((_tmpE23.f1=_tmp78F,((_tmpE23.f2=_tmp790,_tmpE23)))))),_tmpE22)))),e->loc));}goto _LL3B7;_LL3BE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp791=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp787;if(_tmp791->tag != 0)goto _LL3C0;else{if(((_tmp791->f1).Float_c).tag != 7)goto _LL3C0;_tmp792=((struct _tuple7)((_tmp791->f1).Float_c).val).f1;_tmp793=((struct _tuple7)((_tmp791->f1).Float_c).val).f2;}}_LL3BF:
# 2780
{struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct _tmpE26;struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmpE25;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE25=_cycalloc(sizeof(*_tmpE25)),((_tmpE25[0]=((_tmpE26.tag=12,((_tmpE26.f1=_tmp792,((_tmpE26.f2=_tmp793,_tmpE26)))))),_tmpE25)))),e->loc));}goto _LL3B7;_LL3C0: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp794=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp787;if(_tmp794->tag != 0)goto _LL3C2;else{if(((_tmp794->f1).Null_c).tag != 1)goto _LL3C2;}}_LL3C1:
# 2782
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL3B7;_LL3C2: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp795=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp787;if(_tmp795->tag != 0)goto _LL3C4;else{if(((_tmp795->f1).String_c).tag != 8)goto _LL3C4;}}_LL3C3:
# 2784
{const char*_tmpE27;Cyc_Parse_err(((_tmpE27="strings cannot occur within patterns",_tag_dyneither(_tmpE27,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3B7;_LL3C4: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp796=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp787;if(_tmp796->tag != 0)goto _LL3C6;else{if(((_tmp796->f1).Wstring_c).tag != 9)goto _LL3C6;}}_LL3C5:
# 2786
{const char*_tmpE28;Cyc_Parse_err(((_tmpE28="strings cannot occur within patterns",_tag_dyneither(_tmpE28,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3B7;_LL3C6: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp797=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp787;if(_tmp797->tag != 0)goto _LL3C8;else{if(((_tmp797->f1).LongLong_c).tag != 6)goto _LL3C8;}}_LL3C7:
# 2788
{const char*_tmpE29;Cyc_Parse_unimp(((_tmpE29="long long's in patterns",_tag_dyneither(_tmpE29,sizeof(char),24))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3B7;_LL3C8:;_LL3C9: {
# 2790
const char*_tmpE2A;Cyc_Parse_err(((_tmpE2A="bad constant in case",_tag_dyneither(_tmpE2A,sizeof(char),21))),(unsigned int)(yyyylsp[0]).first_line);}_LL3B7:;}
# 2793
break;}case 382: _LL3B6: {
# 2795
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2797
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2794 "parse.y"
{struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct _tmpE2D;struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmpE2C;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE2C=_cycalloc(sizeof(*_tmpE2C)),((_tmpE2C[0]=((_tmpE2D.tag=15,((_tmpE2D.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),_tmpE2D)))),_tmpE2C)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 383: _LL3CA: {
# 2797
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2799
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2796 "parse.y"
{const char*_tmpE2E;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpE2E="as",_tag_dyneither(_tmpE2E,sizeof(char),3))))!= 0){
const char*_tmpE2F;Cyc_Parse_err(((_tmpE2F="expecting `as'",_tag_dyneither(_tmpE2F,sizeof(char),15))),(unsigned int)(yyyylsp[1]).first_line);}}
{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpE39;struct _dyneither_ptr*_tmpE38;struct _tuple0*_tmpE37;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpE36;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE36=_cycalloc(sizeof(*_tmpE36)),((_tmpE36[0]=((_tmpE39.tag=1,((_tmpE39.f1=Cyc_Absyn_new_vardecl(((_tmpE37=_cycalloc(sizeof(*_tmpE37)),((_tmpE37->f1=Cyc_Absyn_Loc_n,((_tmpE37->f2=((_tmpE38=_cycalloc(sizeof(*_tmpE38)),((_tmpE38[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE38)))),_tmpE37)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpE39.f2=
Cyc_yyget_YY9(yyyyvsp[2]),_tmpE39)))))),_tmpE36)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2801
break;}case 384: _LL3CB: {
# 2803
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2805
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2802 "parse.y"
{const char*_tmpE3A;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]),((_tmpE3A="alias",_tag_dyneither(_tmpE3A,sizeof(char),6))))!= 0){
const char*_tmpE3B;Cyc_Parse_err(((_tmpE3B="expecting `alias'",_tag_dyneither(_tmpE3B,sizeof(char),18))),(unsigned int)(yyyylsp[1]).first_line);}}{
int _tmp7AE=(yyyylsp[0]).first_line;
struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpE43;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmpE42;struct _dyneither_ptr*_tmpE41;struct Cyc_Absyn_Tvar*_tmpE40;struct Cyc_Absyn_Tvar*tv=(_tmpE40=_cycalloc(sizeof(*_tmpE40)),((_tmpE40->name=((_tmpE41=_cycalloc(sizeof(*_tmpE41)),((_tmpE41[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpE41)))),((_tmpE40->identity=- 1,((_tmpE40->kind=(void*)((_tmpE43=_cycalloc(sizeof(*_tmpE43)),((_tmpE43[0]=((_tmpE42.tag=0,((_tmpE42.f1=& Cyc_Tcutil_rk,_tmpE42)))),_tmpE43)))),_tmpE40)))))));
struct _dyneither_ptr*_tmpE46;struct _tuple0*_tmpE45;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpE45=_cycalloc(sizeof(*_tmpE45)),((_tmpE45->f1=Cyc_Absyn_Loc_n,((_tmpE45->f2=((_tmpE46=_cycalloc(sizeof(*_tmpE46)),((_tmpE46[0]=Cyc_yyget_String_tok(yyyyvsp[5]),_tmpE46)))),_tmpE45)))))),
Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[4]),(unsigned int)(yyyylsp[4]).first_line),0);
{struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct _tmpE49;struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmpE48;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE48=_cycalloc(sizeof(*_tmpE48)),((_tmpE48[0]=((_tmpE49.tag=2,((_tmpE49.f1=tv,((_tmpE49.f2=vd,_tmpE49)))))),_tmpE48)))),(unsigned int)_tmp7AE));}
# 2810
break;};}case 385: _LL3CC: {
# 2812
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2814
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2811 "parse.y"
struct _tuple22 _tmp7B7=*Cyc_yyget_YY10(yyyyvsp[2]);struct Cyc_List_List*_tmp7B9;int _tmp7BA;struct _tuple22 _tmp7B8=_tmp7B7;_tmp7B9=_tmp7B8.f1;_tmp7BA=_tmp7B8.f2;
{struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct _tmpE4C;struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmpE4B;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE4B=_cycalloc(sizeof(*_tmpE4B)),((_tmpE4B[0]=((_tmpE4C.tag=5,((_tmpE4C.f1=_tmp7B9,((_tmpE4C.f2=_tmp7BA,_tmpE4C)))))),_tmpE4B)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2814
break;}case 386: _LL3CD: {
# 2816
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2818
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2815 "parse.y"
struct _tuple22 _tmp7BD=*Cyc_yyget_YY10(yyyyvsp[2]);struct Cyc_List_List*_tmp7BF;int _tmp7C0;struct _tuple22 _tmp7BE=_tmp7BD;_tmp7BF=_tmp7BE.f1;_tmp7C0=_tmp7BE.f2;
{struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct _tmpE4F;struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmpE4E;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE4E=_cycalloc(sizeof(*_tmpE4E)),((_tmpE4E[0]=((_tmpE4F.tag=16,((_tmpE4F.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpE4F.f2=_tmp7BF,((_tmpE4F.f3=_tmp7C0,_tmpE4F)))))))),_tmpE4E)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2818
break;}case 387: _LL3CE: {
# 2820
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2822
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2819 "parse.y"
struct _tuple22 _tmp7C3=*Cyc_yyget_YY14(yyyyvsp[3]);struct Cyc_List_List*_tmp7C5;int _tmp7C6;struct _tuple22 _tmp7C4=_tmp7C3;_tmp7C5=_tmp7C4.f1;_tmp7C6=_tmp7C4.f2;{
struct Cyc_List_List*_tmp7C7=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY40(yyyyvsp[2]));
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpE55;struct Cyc_Absyn_AggrInfo*_tmpE54;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpE53;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE53=_cycalloc(sizeof(*_tmpE53)),((_tmpE53[0]=((_tmpE55.tag=7,((_tmpE55.f1=((_tmpE54=_cycalloc(sizeof(*_tmpE54)),((_tmpE54->aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(yyyyvsp[0]),0),((_tmpE54->targs=0,_tmpE54)))))),((_tmpE55.f2=_tmp7C7,((_tmpE55.f3=_tmp7C5,((_tmpE55.f4=_tmp7C6,_tmpE55)))))))))),_tmpE53)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2824
break;};}case 388: _LL3CF: {
# 2826
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2828
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2825 "parse.y"
struct _tuple22 _tmp7CB=*Cyc_yyget_YY14(yyyyvsp[2]);struct Cyc_List_List*_tmp7CD;int _tmp7CE;struct _tuple22 _tmp7CC=_tmp7CB;_tmp7CD=_tmp7CC.f1;_tmp7CE=_tmp7CC.f2;{
struct Cyc_List_List*_tmp7CF=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,Cyc_yyget_YY40(yyyyvsp[1]));
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpE58;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpE57;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE57=_cycalloc(sizeof(*_tmpE57)),((_tmpE57[0]=((_tmpE58.tag=7,((_tmpE58.f1=0,((_tmpE58.f2=_tmp7CF,((_tmpE58.f3=_tmp7CD,((_tmpE58.f4=_tmp7CE,_tmpE58)))))))))),_tmpE57)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2829
break;};}case 389: _LL3D0: {
# 2831
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2833
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2830 "parse.y"
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE5B;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpE5A;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE5A=_cycalloc(sizeof(*_tmpE5A)),((_tmpE5A[0]=((_tmpE5B.tag=6,((_tmpE5B.f1=Cyc_yyget_YY9(yyyyvsp[1]),_tmpE5B)))),_tmpE5A)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 390: _LL3D1: {
# 2833
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2835
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2832 "parse.y"
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE65;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE64;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpE63;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpE62;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE62=_cycalloc(sizeof(*_tmpE62)),((_tmpE62[0]=((_tmpE65.tag=6,((_tmpE65.f1=Cyc_Absyn_new_pat((void*)((_tmpE63=_cycalloc(sizeof(*_tmpE63)),((_tmpE63[0]=((_tmpE64.tag=6,((_tmpE64.f1=Cyc_yyget_YY9(yyyyvsp[1]),_tmpE64)))),_tmpE63)))),(unsigned int)(yyyylsp[0]).first_line),_tmpE65)))),_tmpE62)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 391: _LL3D2: {
# 2835
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2837
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2834 "parse.y"
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpE6F;struct _dyneither_ptr*_tmpE6E;struct _tuple0*_tmpE6D;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpE6C;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE6C=_cycalloc(sizeof(*_tmpE6C)),((_tmpE6C[0]=((_tmpE6F.tag=3,((_tmpE6F.f1=Cyc_Absyn_new_vardecl(((_tmpE6D=_cycalloc(sizeof(*_tmpE6D)),((_tmpE6D->f1=Cyc_Absyn_Loc_n,((_tmpE6D->f2=((_tmpE6E=_cycalloc(sizeof(*_tmpE6E)),((_tmpE6E[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpE6E)))),_tmpE6D)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpE6F.f2=
# 2836
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[1]).first_line),_tmpE6F)))))),_tmpE6C)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2838
break;}case 392: _LL3D3: {
# 2840
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2842
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2839 "parse.y"
{const char*_tmpE70;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[2]),((_tmpE70="as",_tag_dyneither(_tmpE70,sizeof(char),3))))!= 0){
const char*_tmpE71;Cyc_Parse_err(((_tmpE71="expecting `as'",_tag_dyneither(_tmpE71,sizeof(char),15))),(unsigned int)(yyyylsp[2]).first_line);}}
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpE7B;struct _dyneither_ptr*_tmpE7A;struct _tuple0*_tmpE79;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpE78;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE78=_cycalloc(sizeof(*_tmpE78)),((_tmpE78[0]=((_tmpE7B.tag=3,((_tmpE7B.f1=Cyc_Absyn_new_vardecl(((_tmpE79=_cycalloc(sizeof(*_tmpE79)),((_tmpE79->f1=Cyc_Absyn_Loc_n,((_tmpE79->f2=((_tmpE7A=_cycalloc(sizeof(*_tmpE7A)),((_tmpE7A[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpE7A)))),_tmpE79)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpE7B.f2=
# 2843
Cyc_yyget_YY9(yyyyvsp[3]),_tmpE7B)))))),_tmpE78)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2845
break;}case 393: _LL3D4: {
# 2847
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2849
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2846 "parse.y"
void*_tmp7E2=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[2]),Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpE8C;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpE8B;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE8A;struct _dyneither_ptr*_tmpE89;struct _tuple0*_tmpE88;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpE87;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE87=_cycalloc(sizeof(*_tmpE87)),((_tmpE87[0]=((_tmpE8C.tag=4,((_tmpE8C.f1=Cyc_Parse_typ2tvar((unsigned int)(yyyylsp[2]).first_line,_tmp7E2),((_tmpE8C.f2=
Cyc_Absyn_new_vardecl(((_tmpE88=_cycalloc(sizeof(*_tmpE88)),((_tmpE88->f1=Cyc_Absyn_Loc_n,((_tmpE88->f2=((_tmpE89=_cycalloc(sizeof(*_tmpE89)),((_tmpE89[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE89)))),_tmpE88)))))),(void*)(
(_tmpE8A=_cycalloc(sizeof(*_tmpE8A)),((_tmpE8A[0]=((_tmpE8B.tag=19,((_tmpE8B.f1=_tmp7E2,_tmpE8B)))),_tmpE8A)))),0),_tmpE8C)))))),_tmpE87)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2851
break;}case 394: _LL3D5: {
# 2853
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2855
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2852 "parse.y"
struct Cyc_Absyn_Tvar*_tmp7E9=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpEA8;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpEA7;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpEA6;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpEA5;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpEA4;struct _dyneither_ptr*_tmpEA3;struct _tuple0*_tmpEA2;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpEA1;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpEA1=_cycalloc(sizeof(*_tmpEA1)),((_tmpEA1[0]=((_tmpEA8.tag=4,((_tmpEA8.f1=_tmp7E9,((_tmpEA8.f2=
Cyc_Absyn_new_vardecl(((_tmpEA2=_cycalloc(sizeof(*_tmpEA2)),((_tmpEA2->f1=Cyc_Absyn_Loc_n,((_tmpEA2->f2=((_tmpEA3=_cycalloc(sizeof(*_tmpEA3)),((_tmpEA3[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpEA3)))),_tmpEA2)))))),(void*)(
(_tmpEA4=_cycalloc(sizeof(*_tmpEA4)),((_tmpEA4[0]=((_tmpEA7.tag=19,((_tmpEA7.f1=(void*)((_tmpEA5=_cycalloc(sizeof(*_tmpEA5)),((_tmpEA5[0]=((_tmpEA6.tag=2,((_tmpEA6.f1=_tmp7E9,_tmpEA6)))),_tmpEA5)))),_tmpEA7)))),_tmpEA4)))),0),_tmpEA8)))))),_tmpEA1)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2857
break;}case 395: _LL3D6: {
# 2859
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2861
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2860 "parse.y"
{struct _tuple22*_tmpEA9;yyval=Cyc_YY10(((_tmpEA9=_cycalloc(sizeof(*_tmpEA9)),((_tmpEA9->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(yyyyvsp[0])),((_tmpEA9->f2=0,_tmpEA9)))))));}
break;}case 396: _LL3D7: {
# 2863
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2865
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2861 "parse.y"
{struct _tuple22*_tmpEAA;yyval=Cyc_YY10(((_tmpEAA=_cycalloc(sizeof(*_tmpEAA)),((_tmpEAA->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(yyyyvsp[0])),((_tmpEAA->f2=1,_tmpEAA)))))));}
break;}case 397: _LL3D8: {
# 2864
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2866
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2862 "parse.y"
{struct _tuple22*_tmpEAB;yyval=Cyc_YY10(((_tmpEAB=_cycalloc(sizeof(*_tmpEAB)),((_tmpEAB->f1=0,((_tmpEAB->f2=1,_tmpEAB)))))));}
break;}case 398: _LL3D9: {
# 2865
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2867
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2867 "parse.y"
{struct Cyc_List_List*_tmpEAC;yyval=Cyc_YY11(((_tmpEAC=_cycalloc(sizeof(*_tmpEAC)),((_tmpEAC->hd=Cyc_yyget_YY9(yyyyvsp[0]),((_tmpEAC->tl=0,_tmpEAC)))))));}
break;}case 399: _LL3DA: {
# 2870
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2872
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2869 "parse.y"
{struct Cyc_List_List*_tmpEAD;yyval=Cyc_YY11(((_tmpEAD=_cycalloc(sizeof(*_tmpEAD)),((_tmpEAD->hd=Cyc_yyget_YY9(yyyyvsp[2]),((_tmpEAD->tl=Cyc_yyget_YY11(yyyyvsp[0]),_tmpEAD)))))));}
break;}case 400: _LL3DB: {
# 2872
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2874
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2874 "parse.y"
{struct _tuple23*_tmpEAE;yyval=Cyc_YY12(((_tmpEAE=_cycalloc(sizeof(*_tmpEAE)),((_tmpEAE->f1=0,((_tmpEAE->f2=Cyc_yyget_YY9(yyyyvsp[0]),_tmpEAE)))))));}
break;}case 401: _LL3DC: {
# 2877
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2879
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2876 "parse.y"
{struct _tuple23*_tmpEAF;yyval=Cyc_YY12(((_tmpEAF=_cycalloc(sizeof(*_tmpEAF)),((_tmpEAF->f1=Cyc_yyget_YY41(yyyyvsp[0]),((_tmpEAF->f2=Cyc_yyget_YY9(yyyyvsp[1]),_tmpEAF)))))));}
break;}case 402: _LL3DD: {
# 2879
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2881
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2879 "parse.y"
{struct _tuple22*_tmpEB0;yyval=Cyc_YY14(((_tmpEB0=_cycalloc(sizeof(*_tmpEB0)),((_tmpEB0->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0])),((_tmpEB0->f2=0,_tmpEB0)))))));}
break;}case 403: _LL3DE: {
# 2882
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2884
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2880 "parse.y"
{struct _tuple22*_tmpEB1;yyval=Cyc_YY14(((_tmpEB1=_cycalloc(sizeof(*_tmpEB1)),((_tmpEB1->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0])),((_tmpEB1->f2=1,_tmpEB1)))))));}
break;}case 404: _LL3DF: {
# 2883
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2885
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2881 "parse.y"
{struct _tuple22*_tmpEB2;yyval=Cyc_YY14(((_tmpEB2=_cycalloc(sizeof(*_tmpEB2)),((_tmpEB2->f1=0,((_tmpEB2->f2=1,_tmpEB2)))))));}
break;}case 405: _LL3E0: {
# 2884
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2886
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2886 "parse.y"
{struct Cyc_List_List*_tmpEB3;yyval=Cyc_YY13(((_tmpEB3=_cycalloc(sizeof(*_tmpEB3)),((_tmpEB3->hd=Cyc_yyget_YY12(yyyyvsp[0]),((_tmpEB3->tl=0,_tmpEB3)))))));}
break;}case 406: _LL3E1: {
# 2889
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2891
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2888 "parse.y"
{struct Cyc_List_List*_tmpEB4;yyval=Cyc_YY13(((_tmpEB4=_cycalloc(sizeof(*_tmpEB4)),((_tmpEB4->hd=Cyc_yyget_YY12(yyyyvsp[2]),((_tmpEB4->tl=Cyc_yyget_YY13(yyyyvsp[0]),_tmpEB4)))))));}
break;}case 407: _LL3E2: {
# 2891
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2893
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=yyyyvsp[0];
break;}case 408: _LL3E3: {
# 2897
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2899
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2896 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_seq_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 409: _LL3E4: {
# 2899
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2901
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2901 "parse.y"
yyval=yyyyvsp[0];
break;}case 410: _LL3E5: {
# 2904
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2906
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2903 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_assignop_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_YY7(yyyyvsp[1]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 411: _LL3E6: {
# 2906
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2908
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2905 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_swap_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 412: _LL3E7: {
# 2908
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2910
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2909 "parse.y"
yyval=Cyc_YY7(0);
break;}case 413: _LL3E8: {
# 2912
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2914
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2910 "parse.y"
{struct Cyc_Core_Opt*_tmpEB5;yyval=Cyc_YY7(((_tmpEB5=_cycalloc_atomic(sizeof(*_tmpEB5)),((_tmpEB5->v=(void*)Cyc_Absyn_Times,_tmpEB5)))));}
break;}case 414: _LL3E9: {
# 2913
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2915
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2911 "parse.y"
{struct Cyc_Core_Opt*_tmpEB6;yyval=Cyc_YY7(((_tmpEB6=_cycalloc_atomic(sizeof(*_tmpEB6)),((_tmpEB6->v=(void*)Cyc_Absyn_Div,_tmpEB6)))));}
break;}case 415: _LL3EA: {
# 2914
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2916
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2912 "parse.y"
{struct Cyc_Core_Opt*_tmpEB7;yyval=Cyc_YY7(((_tmpEB7=_cycalloc_atomic(sizeof(*_tmpEB7)),((_tmpEB7->v=(void*)Cyc_Absyn_Mod,_tmpEB7)))));}
break;}case 416: _LL3EB: {
# 2915
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2917
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2913 "parse.y"
{struct Cyc_Core_Opt*_tmpEB8;yyval=Cyc_YY7(((_tmpEB8=_cycalloc_atomic(sizeof(*_tmpEB8)),((_tmpEB8->v=(void*)Cyc_Absyn_Plus,_tmpEB8)))));}
break;}case 417: _LL3EC: {
# 2916
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2918
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2914 "parse.y"
{struct Cyc_Core_Opt*_tmpEB9;yyval=Cyc_YY7(((_tmpEB9=_cycalloc_atomic(sizeof(*_tmpEB9)),((_tmpEB9->v=(void*)Cyc_Absyn_Minus,_tmpEB9)))));}
break;}case 418: _LL3ED: {
# 2917
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2919
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2915 "parse.y"
{struct Cyc_Core_Opt*_tmpEBA;yyval=Cyc_YY7(((_tmpEBA=_cycalloc_atomic(sizeof(*_tmpEBA)),((_tmpEBA->v=(void*)Cyc_Absyn_Bitlshift,_tmpEBA)))));}
break;}case 419: _LL3EE: {
# 2918
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2920
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2916 "parse.y"
{struct Cyc_Core_Opt*_tmpEBB;yyval=Cyc_YY7(((_tmpEBB=_cycalloc_atomic(sizeof(*_tmpEBB)),((_tmpEBB->v=(void*)Cyc_Absyn_Bitlrshift,_tmpEBB)))));}
break;}case 420: _LL3EF: {
# 2919
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2921
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2917 "parse.y"
{struct Cyc_Core_Opt*_tmpEBC;yyval=Cyc_YY7(((_tmpEBC=_cycalloc_atomic(sizeof(*_tmpEBC)),((_tmpEBC->v=(void*)Cyc_Absyn_Bitand,_tmpEBC)))));}
break;}case 421: _LL3F0: {
# 2920
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2922
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2918 "parse.y"
{struct Cyc_Core_Opt*_tmpEBD;yyval=Cyc_YY7(((_tmpEBD=_cycalloc_atomic(sizeof(*_tmpEBD)),((_tmpEBD->v=(void*)Cyc_Absyn_Bitxor,_tmpEBD)))));}
break;}case 422: _LL3F1: {
# 2921
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2923
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2919 "parse.y"
{struct Cyc_Core_Opt*_tmpEBE;yyval=Cyc_YY7(((_tmpEBE=_cycalloc_atomic(sizeof(*_tmpEBE)),((_tmpEBE->v=(void*)Cyc_Absyn_Bitor,_tmpEBE)))));}
break;}case 423: _LL3F2: {
# 2922
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2924
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2924 "parse.y"
yyval=yyyyvsp[0];
break;}case 424: _LL3F3: {
# 2927
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2929
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2926 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_conditional_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 425: _LL3F4: {
# 2929
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2931
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2929 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_throw_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 426: _LL3F5: {
# 2932
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2934
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2932 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 427: _LL3F6: {
# 2935
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2937
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2934 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 428: _LL3F7: {
# 2937
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2939
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2936 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_New_exp(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 429: _LL3F8: {
# 2939
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2941
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2938 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_New_exp(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 430: _LL3F9: {
# 2941
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2943
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2942 "parse.y"
yyval=yyyyvsp[0];
break;}case 431: _LL3FA: {
# 2945
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2947
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2946 "parse.y"
yyval=yyyyvsp[0];
break;}case 432: _LL3FB: {
# 2949
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2951
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2948 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_or_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 433: _LL3FC: {
# 2951
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2953
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2952 "parse.y"
yyval=yyyyvsp[0];
break;}case 434: _LL3FD: {
# 2955
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2957
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2954 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_and_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 435: _LL3FE: {
# 2957
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2959
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2958 "parse.y"
yyval=yyyyvsp[0];
break;}case 436: _LL3FF: {
# 2961
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2963
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2960 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 437: _LL400: {
# 2963
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2965
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2964 "parse.y"
yyval=yyyyvsp[0];
break;}case 438: _LL401: {
# 2967
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2969
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2966 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 439: _LL402: {
# 2969
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2971
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2970 "parse.y"
yyval=yyyyvsp[0];
break;}case 440: _LL403: {
# 2973
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2975
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2972 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 441: _LL404: {
# 2975
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2977
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2976 "parse.y"
yyval=yyyyvsp[0];
break;}case 442: _LL405: {
# 2979
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2981
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2978 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_eq_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 443: _LL406: {
# 2981
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2983
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2980 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_neq_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 444: _LL407: {
# 2983
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2985
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2984 "parse.y"
yyval=yyyyvsp[0];
break;}case 445: _LL408: {
# 2987
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2989
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2986 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_lt_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 446: _LL409: {
# 2989
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2991
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2988 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_gt_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 447: _LL40A: {
# 2991
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2993
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2990 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_lte_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 448: _LL40B: {
# 2993
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2995
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2992 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_gte_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 449: _LL40C: {
# 2995
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2997
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2996 "parse.y"
yyval=yyyyvsp[0];
break;}case 450: _LL40D: {
# 2999
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3001
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2998 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 451: _LL40E: {
# 3001
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3003
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3000 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 452: _LL40F: {
# 3003
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3005
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3004 "parse.y"
yyval=yyyyvsp[0];
break;}case 453: _LL410: {
# 3007
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3009
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3006 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 454: _LL411: {
# 3009
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3011
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3008 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 455: _LL412: {
# 3011
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3013
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3012 "parse.y"
yyval=yyyyvsp[0];
break;}case 456: _LL413: {
# 3015
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3017
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3014 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 457: _LL414: {
# 3017
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3019
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3016 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 458: _LL415: {
# 3019
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3021
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3018 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 459: _LL416: {
# 3021
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3023
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3022 "parse.y"
yyval=yyyyvsp[0];
break;}case 460: _LL417: {
# 3025
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3027
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3024 "parse.y"
void*_tmp808=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_cast_exp(_tmp808,Cyc_yyget_Exp_tok(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line));
# 3027
break;}case 461: _LL418: {
# 3029
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3031
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3030 "parse.y"
yyval=yyyyvsp[0];
break;}case 462: _LL419: {
# 3033
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3035
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3031 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_pre_inc_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 463: _LL41A: {
# 3034
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3036
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3032 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_pre_dec_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 464: _LL41B: {
# 3035
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3037
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3033 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_address_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 465: _LL41C: {
# 3036
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3038
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3034 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_deref_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 466: _LL41D: {
# 3037
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3039
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3035 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim1_exp(Cyc_Absyn_Plus,Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 467: _LL41E: {
# 3038
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3040
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3036 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim1_exp(Cyc_yyget_YY6(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 468: _LL41F: {
# 3039
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3041
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3038 "parse.y"
void*_tmp809=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_sizeoftyp_exp(_tmp809,(unsigned int)(yyyylsp[0]).first_line));
# 3041
break;}case 469: _LL420: {
# 3043
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3045
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3041 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 470: _LL421: {
# 3044
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3046
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3043 "parse.y"
void*_tmp80A=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_offsetof_exp(_tmp80A,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4])),(unsigned int)(yyyylsp[0]).first_line));
# 3046
break;}case 471: _LL422: {
# 3048
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3050
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3048 "parse.y"
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpEC4;struct Cyc_Absyn_MallocInfo _tmpEC3;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpEC2;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpEC2=_cycalloc(sizeof(*_tmpEC2)),((_tmpEC2[0]=((_tmpEC4.tag=33,((_tmpEC4.f1=((_tmpEC3.is_calloc=0,((_tmpEC3.rgn=0,((_tmpEC3.elt_type=0,((_tmpEC3.num_elts=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpEC3.fat_result=0,_tmpEC3)))))))))),_tmpEC4)))),_tmpEC2)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3050
break;}case 472: _LL423: {
# 3052
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3054
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3051 "parse.y"
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpECA;struct Cyc_Absyn_MallocInfo _tmpEC9;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpEC8;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpEC8=_cycalloc(sizeof(*_tmpEC8)),((_tmpEC8[0]=((_tmpECA.tag=33,((_tmpECA.f1=((_tmpEC9.is_calloc=0,((_tmpEC9.rgn=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpEC9.elt_type=0,((_tmpEC9.num_elts=Cyc_yyget_Exp_tok(yyyyvsp[4]),((_tmpEC9.fat_result=0,_tmpEC9)))))))))),_tmpECA)))),_tmpEC8)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3053
break;}case 473: _LL424: {
# 3055
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 3057
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 3054 "parse.y"
void*_tmp811=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpED4;void**_tmpED3;struct Cyc_Absyn_MallocInfo _tmpED2;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpED1;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpED1=_cycalloc(sizeof(*_tmpED1)),((_tmpED1[0]=((_tmpED4.tag=33,((_tmpED4.f1=((_tmpED2.is_calloc=1,((_tmpED2.rgn=0,((_tmpED2.elt_type=((_tmpED3=_cycalloc(sizeof(*_tmpED3)),((_tmpED3[0]=_tmp811,_tmpED3)))),((_tmpED2.num_elts=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpED2.fat_result=0,_tmpED2)))))))))),_tmpED4)))),_tmpED1)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3057
break;}case 474: _LL425: {
# 3059
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 10),sizeof(union Cyc_YYSTYPE),11);
# 3061
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 10),sizeof(struct Cyc_Yyltype),11);
# 3059 "parse.y"
void*_tmp816=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[8]),(unsigned int)(yyyylsp[8]).first_line);
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpEDE;void**_tmpEDD;struct Cyc_Absyn_MallocInfo _tmpEDC;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpEDB;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpEDB=_cycalloc(sizeof(*_tmpEDB)),((_tmpEDB[0]=((_tmpEDE.tag=33,((_tmpEDE.f1=((_tmpEDC.is_calloc=1,((_tmpEDC.rgn=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpEDC.elt_type=((_tmpEDD=_cycalloc(sizeof(*_tmpEDD)),((_tmpEDD[0]=_tmp816,_tmpEDD)))),((_tmpEDC.num_elts=Cyc_yyget_Exp_tok(yyyyvsp[4]),((_tmpEDC.fat_result=0,_tmpEDC)))))))))),_tmpEDE)))),_tmpEDB)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3062
break;}case 475: _LL426: {
# 3064
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3066
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3063 "parse.y"
{struct Cyc_Absyn_Exp*_tmpEDF[1];yyval=Cyc_Exp_tok(Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,((_tmpEDF[0]=Cyc_yyget_Exp_tok(yyyyvsp[2]),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpEDF,sizeof(struct Cyc_Absyn_Exp*),1)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 476: _LL427: {
# 3066
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3068
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3065 "parse.y"
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpEE5;struct _dyneither_ptr*_tmpEE4;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpEE3;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpEE3=_cycalloc(sizeof(*_tmpEE3)),((_tmpEE3[0]=((_tmpEE5.tag=37,((_tmpEE5.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpEE5.f2=((_tmpEE4=_cycalloc(sizeof(*_tmpEE4)),((_tmpEE4[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpEE4)))),_tmpEE5)))))),_tmpEE3)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 477: _LL428: {
# 3068
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3070
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3067 "parse.y"
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpEEB;struct _dyneither_ptr*_tmpEEA;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpEE9;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpEE9=_cycalloc(sizeof(*_tmpEE9)),((_tmpEE9[0]=((_tmpEEB.tag=37,((_tmpEEB.f1=Cyc_Absyn_deref_exp(Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line),((_tmpEEB.f2=((_tmpEEA=_cycalloc(sizeof(*_tmpEEA)),((_tmpEEA[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpEEA)))),_tmpEEB)))))),_tmpEE9)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 478: _LL429: {
# 3070
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3072
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3069 "parse.y"
void*_tmp822=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_valueof_exp(_tmp822,(unsigned int)(yyyylsp[0]).first_line));
break;}case 479: _LL42A: {
# 3073
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3075
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3072 "parse.y"
struct _tuple20 _tmp823=Cyc_yyget_Asm_tok(yyyyvsp[0]);int _tmp825;struct _dyneither_ptr _tmp826;struct _tuple20 _tmp824=_tmp823;_tmp825=_tmp824.f1;_tmp826=_tmp824.f2;
yyval=Cyc_Exp_tok(Cyc_Absyn_asm_exp(_tmp825,_tmp826,(unsigned int)(yyyylsp[0]).first_line));
break;}case 480: _LL42B: {
# 3076
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3078
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3077 "parse.y"
yyval=Cyc_YY6(Cyc_Absyn_Bitnot);
break;}case 481: _LL42C: {
# 3080
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3082
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3078 "parse.y"
yyval=Cyc_YY6(Cyc_Absyn_Not);
break;}case 482: _LL42D: {
# 3081
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3083
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3079 "parse.y"
yyval=Cyc_YY6(Cyc_Absyn_Minus);
break;}case 483: _LL42E: {
# 3082
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3084
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3084 "parse.y"
yyval=yyyyvsp[0];
break;}case 484: _LL42F: {
# 3087
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3089
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3086 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_subscript_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 485: _LL430: {
# 3089
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3091
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3088 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 486: _LL431: {
# 3091
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3093
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3090 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 487: _LL432: {
# 3093
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3095
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3092 "parse.y"
{struct _dyneither_ptr*_tmpEEC;yyval=Cyc_Exp_tok(Cyc_Absyn_aggrmember_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),((_tmpEEC=_cycalloc(sizeof(*_tmpEEC)),((_tmpEEC[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpEEC)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 488: _LL433: {
# 3095
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3097
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3094 "parse.y"
{struct _dyneither_ptr*_tmpEED;yyval=Cyc_Exp_tok(Cyc_Absyn_aggrarrow_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),((_tmpEED=_cycalloc(sizeof(*_tmpEED)),((_tmpEED[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpEED)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 489: _LL434: {
# 3097
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3099
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3096 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_post_inc_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 490: _LL435: {
# 3099
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3101
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3098 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_post_dec_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 491: _LL436: {
# 3101
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3103
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3100 "parse.y"
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpEF0;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpEEF;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpEEF=_cycalloc(sizeof(*_tmpEEF)),((_tmpEEF[0]=((_tmpEF0.tag=24,((_tmpEF0.f1=Cyc_yyget_YY37(yyyyvsp[1]),((_tmpEF0.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[4])),_tmpEF0)))))),_tmpEEF)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 492: _LL437: {
# 3103
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 3105
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 3102 "parse.y"
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpEF3;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpEF2;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpEF2=_cycalloc(sizeof(*_tmpEF2)),((_tmpEF2[0]=((_tmpEF3.tag=24,((_tmpEF3.f1=Cyc_yyget_YY37(yyyyvsp[1]),((_tmpEF3.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[4])),_tmpEF3)))))),_tmpEF2)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 493: _LL438: {
# 3105
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3107
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3107 "parse.y"
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpEFD;struct _dyneither_ptr*_tmpEFC;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpEFB;struct Cyc_List_List*_tmpEFA;yyval=Cyc_YY3(((_tmpEFA=_cycalloc(sizeof(*_tmpEFA)),((_tmpEFA->hd=(void*)((_tmpEFD=_cycalloc(sizeof(*_tmpEFD)),((_tmpEFD[0]=((_tmpEFB.tag=0,((_tmpEFB.f1=((_tmpEFC=_cycalloc(sizeof(*_tmpEFC)),((_tmpEFC[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpEFC)))),_tmpEFB)))),_tmpEFD)))),((_tmpEFA->tl=0,_tmpEFA)))))));}
break;}case 494: _LL439: {
# 3110
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3112
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3110 "parse.y"
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpF03;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpF02;struct Cyc_List_List*_tmpF01;yyval=Cyc_YY3(((_tmpF01=_cycalloc(sizeof(*_tmpF01)),((_tmpF01->hd=(void*)((_tmpF03=_cycalloc_atomic(sizeof(*_tmpF03)),((_tmpF03[0]=((_tmpF02.tag=1,((_tmpF02.f1=Cyc_Parse_cnst2uint((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_Int_tok(yyyyvsp[0])),_tmpF02)))),_tmpF03)))),((_tmpF01->tl=0,_tmpF01)))))));}
break;}case 495: _LL43A: {
# 3113
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3115
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3112 "parse.y"
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpF0D;struct _dyneither_ptr*_tmpF0C;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpF0B;struct Cyc_List_List*_tmpF0A;yyval=Cyc_YY3(((_tmpF0A=_cycalloc(sizeof(*_tmpF0A)),((_tmpF0A->hd=(void*)((_tmpF0D=_cycalloc(sizeof(*_tmpF0D)),((_tmpF0D[0]=((_tmpF0B.tag=0,((_tmpF0B.f1=((_tmpF0C=_cycalloc(sizeof(*_tmpF0C)),((_tmpF0C[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpF0C)))),_tmpF0B)))),_tmpF0D)))),((_tmpF0A->tl=Cyc_yyget_YY3(yyyyvsp[0]),_tmpF0A)))))));}
break;}case 496: _LL43B: {
# 3115
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3117
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3115 "parse.y"
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpF13;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpF12;struct Cyc_List_List*_tmpF11;yyval=Cyc_YY3(((_tmpF11=_cycalloc(sizeof(*_tmpF11)),((_tmpF11->hd=(void*)((_tmpF13=_cycalloc_atomic(sizeof(*_tmpF13)),((_tmpF13[0]=((_tmpF12.tag=1,((_tmpF12.f1=Cyc_Parse_cnst2uint((unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_Int_tok(yyyyvsp[2])),_tmpF12)))),_tmpF13)))),((_tmpF11->tl=Cyc_yyget_YY3(yyyyvsp[0]),_tmpF11)))))));}
break;}case 497: _LL43C: {
# 3118
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3120
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=Cyc_Exp_tok(Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 498: _LL43D: {
# 3124
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3126
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3123 "parse.y"
yyval=yyyyvsp[0];
break;}case 499: _LL43E: {
# 3126
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3128
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3125 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 500: _LL43F: {
# 3128
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3130
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3127 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_wstring_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 501: _LL440: {
# 3130
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3132
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3129 "parse.y"
yyval=yyyyvsp[1];
break;}case 502: _LL441: {
# 3132
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3134
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3134 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_noinstantiate_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 503: _LL442: {
# 3137
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3139
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3136 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_instantiate_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[3])),(unsigned int)(yyyylsp[0]).first_line));
break;}case 504: _LL443: {
# 3139
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3141
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3139 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_tuple_exp(Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 505: _LL444: {
# 3142
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3144
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3142 "parse.y"
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpF16;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpF15;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpF15=_cycalloc(sizeof(*_tmpF15)),((_tmpF15[0]=((_tmpF16.tag=28,((_tmpF16.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpF16.f2=Cyc_yyget_YY40(yyyyvsp[2]),((_tmpF16.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[3])),((_tmpF16.f4=0,_tmpF16)))))))))),_tmpF15)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 506: _LL445: {
# 3145
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3147
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3145 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_stmt_exp(Cyc_yyget_Stmt_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 507: _LL446: {
# 3148
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3150
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3149 "parse.y"
yyval=Cyc_YY4(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY4(yyyyvsp[0])));
break;}case 508: _LL447: {
# 3152
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3154
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpF17;yyval=Cyc_YY4(((_tmpF17=_cycalloc(sizeof(*_tmpF17)),((_tmpF17->hd=Cyc_yyget_Exp_tok(yyyyvsp[0]),((_tmpF17->tl=0,_tmpF17)))))));}
break;}case 509: _LL448: {
# 3158
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3160
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3157 "parse.y"
{struct Cyc_List_List*_tmpF18;yyval=Cyc_YY4(((_tmpF18=_cycalloc(sizeof(*_tmpF18)),((_tmpF18->hd=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpF18->tl=Cyc_yyget_YY4(yyyyvsp[0]),_tmpF18)))))));}
break;}case 510: _LL449: {
# 3160
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3162
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=Cyc_Exp_tok(Cyc_Absyn_const_exp(Cyc_yyget_Int_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 511: _LL44A: {
# 3166
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3168
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3164 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 512: _LL44B: {
# 3167
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3169
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3165 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_wchar_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 513: _LL44C: {
# 3168
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3170
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3167 "parse.y"
struct _dyneither_ptr _tmp83F=Cyc_yyget_String_tok(yyyyvsp[0]);
int l=(int)Cyc_strlen((struct _dyneither_ptr)_tmp83F);
int i=1;
if(l > 0){
char c=*((const char*)_check_dyneither_subscript(_tmp83F,sizeof(char),l - 1));
if(c == 'f'  || c == 'F')i=0;else{
if(c == 'l'  || c == 'L')i=2;}}
# 3175
yyval=Cyc_Exp_tok(Cyc_Absyn_float_exp(_tmp83F,i,(unsigned int)(yyyylsp[0]).first_line));
# 3177
break;}case 514: _LL44D: {
# 3179
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3181
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3178 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_null_exp((unsigned int)(yyyylsp[0]).first_line));
break;}case 515: _LL44E: {
# 3181
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3183
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3182 "parse.y"
{struct _dyneither_ptr*_tmpF1B;struct _tuple0*_tmpF1A;yyval=Cyc_QualId_tok(((_tmpF1A=_cycalloc(sizeof(*_tmpF1A)),((_tmpF1A->f1=Cyc_Absyn_Rel_n(0),((_tmpF1A->f2=((_tmpF1B=_cycalloc(sizeof(*_tmpF1B)),((_tmpF1B[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpF1B)))),_tmpF1A)))))));}
break;}case 516: _LL44F: {
# 3185
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3187
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3183 "parse.y"
yyval=yyyyvsp[0];
break;}case 517: _LL450: {
# 3186
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3188
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3186 "parse.y"
{struct _dyneither_ptr*_tmpF1E;struct _tuple0*_tmpF1D;yyval=Cyc_QualId_tok(((_tmpF1D=_cycalloc(sizeof(*_tmpF1D)),((_tmpF1D->f1=Cyc_Absyn_Rel_n(0),((_tmpF1D->f2=((_tmpF1E=_cycalloc(sizeof(*_tmpF1E)),((_tmpF1E[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpF1E)))),_tmpF1D)))))));}
break;}case 518: _LL451: {
# 3189
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3191
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3187 "parse.y"
yyval=yyyyvsp[0];
break;}case 519: _LL452: {
# 3190
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3192
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3192 "parse.y"
yyval=yyyyvsp[0];
break;}case 520: _LL453: {
# 3195
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3197
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3193 "parse.y"
yyval=yyyyvsp[0];
break;}case 521: _LL454: {
# 3196
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3198
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3196 "parse.y"
yyval=yyyyvsp[0];
break;}case 522: _LL455: {
# 3199
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3201
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3197 "parse.y"
yyval=yyyyvsp[0];
break;}case 523: _LL456: {
# 3200
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3202
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3202 "parse.y"
break;}case 524: _LL457: {
# 3204
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3206
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3202 "parse.y"
yylex_buf->lex_curr_pos -=1;
break;}default: _LL458:
# 3207
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
yystate=Cyc_yypgoto[_check_known_subscript_notnull(148,yyn - 145)]+ *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset));
if((yystate >= 0  && yystate <= 6814) && Cyc_yycheck[_check_known_subscript_notnull(6815,yystate)]== *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset)))
yystate=(int)Cyc_yytable[_check_known_subscript_notnull(6815,yystate)];else{
# 417
yystate=(int)Cyc_yydefgoto[_check_known_subscript_notnull(148,yyn - 145)];}
# 419
goto yynewstate;
# 421
yyerrlab:
# 423
 if(yyerrstatus == 0){
# 426
++ yynerrs;
# 429
yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1060,yystate)];
# 431
if(yyn > - 32768  && yyn < 6814){
# 433
int sze=0;
struct _dyneither_ptr msg;
int x;int count;
# 437
count=0;
# 439
for(x=yyn < 0?- yyn: 0;x < 293 / sizeof(char*);++ x){
# 441
if(Cyc_yycheck[_check_known_subscript_notnull(6815,x + yyn)]== x)
(sze +=Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(293,x)])+ 15,count ++);}
{unsigned int _tmpF23;unsigned int _tmpF22;struct _dyneither_ptr _tmpF21;char*_tmpF20;unsigned int _tmpF1F;msg=((_tmpF1F=(unsigned int)(sze + 15),((_tmpF20=(char*)_region_malloc(yyregion,_check_times(sizeof(char),_tmpF1F + 1)),((_tmpF21=_tag_dyneither(_tmpF20,sizeof(char),_tmpF1F + 1),((((_tmpF22=_tmpF1F,((_tmpF24(& _tmpF23,& _tmpF22,& _tmpF20),_tmpF20[_tmpF22]=(char)0)))),_tmpF21))))))));}
{const char*_tmpF25;Cyc_strcpy(msg,((_tmpF25="parse error",_tag_dyneither(_tmpF25,sizeof(char),12))));}
# 446
if(count < 5){
# 448
count=0;
for(x=yyn < 0?- yyn: 0;x < 293 / sizeof(char*);++ x){
# 451
if(Cyc_yycheck[_check_known_subscript_notnull(6815,x + yyn)]== x){
# 453
{const char*_tmpF27;const char*_tmpF26;Cyc_strcat(msg,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)(
(_tmpF27=", expecting `",_tag_dyneither(_tmpF27,sizeof(char),14))):(struct _dyneither_ptr)(
(_tmpF26=" or `",_tag_dyneither(_tmpF26,sizeof(char),6)))));}
Cyc_strcat(msg,(struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(293,x)]);
{const char*_tmpF28;Cyc_strcat(msg,((_tmpF28="'",_tag_dyneither(_tmpF28,sizeof(char),2))));}
++ count;}}}
# 461
Cyc_yyerror((struct _dyneither_ptr)msg,yystate,yychar);}else{
# 465
const char*_tmpF29;Cyc_yyerror(((_tmpF29="parse error",_tag_dyneither(_tmpF29,sizeof(char),12))),yystate,yychar);}}
# 467
goto yyerrlab1;
# 469
yyerrlab1:
# 471
 if(yyerrstatus == 3){
# 476
if(yychar == 0){
int _tmp84D=1;_npop_handler(0);return _tmp84D;}
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
 if(yyssp_offset == 0){int _tmp84E=1;_npop_handler(0);return _tmp84E;}
-- yyvsp_offset;
yystate=(int)*((short*)_check_dyneither_subscript(yyss,sizeof(short),-- yyssp_offset));
# 511
-- yylsp_offset;
# 526 "cyc_bison.simple"
yyerrhandle:
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1060,yystate)];
if(yyn == - 32768)goto yyerrdefault;
# 530
yyn +=1;
if((yyn < 0  || yyn > 6814) || Cyc_yycheck[_check_known_subscript_notnull(6815,yyn)]!= 1)goto yyerrdefault;
# 533
yyn=(int)Cyc_yytable[_check_known_subscript_notnull(6815,yyn)];
if(yyn < 0){
# 536
if(yyn == - 32768)goto yyerrpop;
yyn=- yyn;
goto yyreduce;}else{
# 540
if(yyn == 0)goto yyerrpop;}
# 542
if(yyn == 1059){
int _tmp84F=0;_npop_handler(0);return _tmp84F;}
# 550
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yylval;
# 552
*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),++ yylsp_offset))=yylloc;
# 555
goto yynewstate;}
# 141 "cycbison.simple"
;_pop_region(yyregion);}
# 3205 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
union Cyc_YYSTYPE _tmp859=v;union Cyc_Absyn_Cnst _tmp85A;char _tmp85B;struct _dyneither_ptr _tmp85C;union Cyc_Absyn_Nmspace _tmp85D;struct _dyneither_ptr*_tmp85E;struct Cyc_Absyn_Exp*_tmp85F;struct Cyc_Absyn_Stmt*_tmp860;_LL45B: if((_tmp859.Int_tok).tag != 1)goto _LL45D;_tmp85A=(union Cyc_Absyn_Cnst)(_tmp859.Int_tok).val;_LL45C:
{const char*_tmpF2D;void*_tmpF2C[1];struct Cyc_String_pa_PrintArg_struct _tmpF2B;(_tmpF2B.tag=0,((_tmpF2B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_cnst2string(_tmp85A)),((_tmpF2C[0]=& _tmpF2B,Cyc_fprintf(Cyc_stderr,((_tmpF2D="%s",_tag_dyneither(_tmpF2D,sizeof(char),3))),_tag_dyneither(_tmpF2C,sizeof(void*),1)))))));}goto _LL45A;_LL45D: if((_tmp859.Char_tok).tag != 2)goto _LL45F;_tmp85B=(char)(_tmp859.Char_tok).val;_LL45E:
{const char*_tmpF31;void*_tmpF30[1];struct Cyc_Int_pa_PrintArg_struct _tmpF2F;(_tmpF2F.tag=1,((_tmpF2F.f1=(unsigned long)((int)_tmp85B),((_tmpF30[0]=& _tmpF2F,Cyc_fprintf(Cyc_stderr,((_tmpF31="%c",_tag_dyneither(_tmpF31,sizeof(char),3))),_tag_dyneither(_tmpF30,sizeof(void*),1)))))));}goto _LL45A;_LL45F: if((_tmp859.String_tok).tag != 3)goto _LL461;_tmp85C=(struct _dyneither_ptr)(_tmp859.String_tok).val;_LL460:
{const char*_tmpF35;void*_tmpF34[1];struct Cyc_String_pa_PrintArg_struct _tmpF33;(_tmpF33.tag=0,((_tmpF33.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp85C),((_tmpF34[0]=& _tmpF33,Cyc_fprintf(Cyc_stderr,((_tmpF35="\"%s\"",_tag_dyneither(_tmpF35,sizeof(char),5))),_tag_dyneither(_tmpF34,sizeof(void*),1)))))));}goto _LL45A;_LL461: if((_tmp859.QualId_tok).tag != 5)goto _LL463;_tmp85D=((struct _tuple0*)(_tmp859.QualId_tok).val)->f1;_tmp85E=((struct _tuple0*)(_tmp859.QualId_tok).val)->f2;_LL462: {
# 3211
struct Cyc_List_List*_tmp86A=0;
{union Cyc_Absyn_Nmspace _tmp86B=_tmp85D;struct Cyc_List_List*_tmp86C;struct Cyc_List_List*_tmp86D;struct Cyc_List_List*_tmp86E;_LL46A: if((_tmp86B.Rel_n).tag != 1)goto _LL46C;_tmp86C=(struct Cyc_List_List*)(_tmp86B.Rel_n).val;_LL46B:
 _tmp86A=_tmp86C;goto _LL469;_LL46C: if((_tmp86B.Abs_n).tag != 2)goto _LL46E;_tmp86D=(struct Cyc_List_List*)(_tmp86B.Abs_n).val;_LL46D:
 _tmp86A=_tmp86D;goto _LL469;_LL46E: if((_tmp86B.C_n).tag != 3)goto _LL470;_tmp86E=(struct Cyc_List_List*)(_tmp86B.C_n).val;_LL46F:
 _tmp86A=_tmp86E;goto _LL469;_LL470: if((_tmp86B.Loc_n).tag != 4)goto _LL469;_LL471:
 goto _LL469;_LL469:;}
# 3218
for(0;_tmp86A != 0;_tmp86A=_tmp86A->tl){
const char*_tmpF39;void*_tmpF38[1];struct Cyc_String_pa_PrintArg_struct _tmpF37;(_tmpF37.tag=0,((_tmpF37.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp86A->hd)),((_tmpF38[0]=& _tmpF37,Cyc_fprintf(Cyc_stderr,((_tmpF39="%s::",_tag_dyneither(_tmpF39,sizeof(char),5))),_tag_dyneither(_tmpF38,sizeof(void*),1)))))));}
{const char*_tmpF3D;void*_tmpF3C[1];struct Cyc_String_pa_PrintArg_struct _tmpF3B;(_tmpF3B.tag=0,((_tmpF3B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp85E),((_tmpF3C[0]=& _tmpF3B,Cyc_fprintf(Cyc_stderr,((_tmpF3D="%s::",_tag_dyneither(_tmpF3D,sizeof(char),5))),_tag_dyneither(_tmpF3C,sizeof(void*),1)))))));}
goto _LL45A;}_LL463: if((_tmp859.Exp_tok).tag != 7)goto _LL465;_tmp85F=(struct Cyc_Absyn_Exp*)(_tmp859.Exp_tok).val;_LL464:
{const char*_tmpF41;void*_tmpF40[1];struct Cyc_String_pa_PrintArg_struct _tmpF3F;(_tmpF3F.tag=0,((_tmpF3F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp85F)),((_tmpF40[0]=& _tmpF3F,Cyc_fprintf(Cyc_stderr,((_tmpF41="%s",_tag_dyneither(_tmpF41,sizeof(char),3))),_tag_dyneither(_tmpF40,sizeof(void*),1)))))));}goto _LL45A;_LL465: if((_tmp859.Stmt_tok).tag != 8)goto _LL467;_tmp860=(struct Cyc_Absyn_Stmt*)(_tmp859.Stmt_tok).val;_LL466:
{const char*_tmpF45;void*_tmpF44[1];struct Cyc_String_pa_PrintArg_struct _tmpF43;(_tmpF43.tag=0,((_tmpF43.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(_tmp860)),((_tmpF44[0]=& _tmpF43,Cyc_fprintf(Cyc_stderr,((_tmpF45="%s",_tag_dyneither(_tmpF45,sizeof(char),3))),_tag_dyneither(_tmpF44,sizeof(void*),1)))))));}goto _LL45A;_LL467:;_LL468:
{const char*_tmpF48;void*_tmpF47;(_tmpF47=0,Cyc_fprintf(Cyc_stderr,((_tmpF48="?",_tag_dyneither(_tmpF48,sizeof(char),2))),_tag_dyneither(_tmpF47,sizeof(void*),0)));}goto _LL45A;_LL45A:;}
# 3229
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp87D=_new_region("yyr");struct _RegionHandle*yyr=& _tmp87D;_push_region(yyr);
Cyc_yyparse(yyr,Cyc_Lexing_from_file(f));{
struct Cyc_List_List*_tmp87E=Cyc_Parse_parse_result;_npop_handler(0);return _tmp87E;};
# 3232
;_pop_region(yyr);};}
