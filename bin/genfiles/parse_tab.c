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
{struct _tuple10*_tmp883;declarators=((_tmp883=_region_malloc(r,sizeof(*_tmp883)),((_tmp883->tl=declarators,((_tmp883->hd=_tmp2,_tmp883))))));}{
struct Cyc_List_List*_tmp884;exprs=((_tmp884=_region_malloc(r,sizeof(*_tmp884)),((_tmp884->hd=_tmp3,((_tmp884->tl=exprs,_tmp884))))));};}
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
struct Cyc_Position_Exit_exn_struct _tmp887;struct Cyc_Position_Exit_exn_struct*_tmp886;(int)_throw((void*)((_tmp886=_cycalloc_atomic(sizeof(*_tmp886)),((_tmp886[0]=((_tmp887.tag=Cyc_Position_Exit,_tmp887)),_tmp886)))));};}
# 191
static void Cyc_Parse_unimp(struct _dyneither_ptr msg,unsigned int sg){
{const char*_tmp88C;void*_tmp88B[2];struct Cyc_String_pa_PrintArg_struct _tmp88A;struct Cyc_String_pa_PrintArg_struct _tmp889;(_tmp889.tag=0,((_tmp889.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp88A.tag=0,((_tmp88A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(sg)),((_tmp88B[0]=& _tmp88A,((_tmp88B[1]=& _tmp889,Cyc_fprintf(Cyc_stderr,((_tmp88C="%s: Warning: Cyclone does not yet support %s\n",_tag_dyneither(_tmp88C,sizeof(char),46))),_tag_dyneither(_tmp88B,sizeof(void*),2)))))))))))));}
return;}
# 197
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned int loc){
# 199
struct Cyc_Absyn_Tqual _tmpD;void*_tmpE;struct _tuple8*_tmpC=tqt;_tmpD=_tmpC->f2;_tmpE=_tmpC->f3;
if((_tmpD.print_const  || _tmpD.q_volatile) || _tmpD.q_restrict){
if(_tmpD.loc != 0)loc=_tmpD.loc;{
const char*_tmp88F;void*_tmp88E;(_tmp88E=0,Cyc_Tcutil_warn(loc,((_tmp88F="qualifier on type is ignored",_tag_dyneither(_tmp88F,sizeof(char),29))),_tag_dyneither(_tmp88E,sizeof(void*),0)));};}
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
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp892;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp891;bound=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp891=_cycalloc(sizeof(*_tmp891)),((_tmp891[0]=((_tmp892.tag=1,((_tmp892.f1=_tmp1A,_tmp892)))),_tmp891)))));}goto _LL0;_LLF: {struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmp1B=(struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp1B->tag != 1)goto _LL0;else{_tmp1C=(void*)_tmp1B->f1;}}_LL10:
# 231
 rgn=_tmp1C;goto _LL0;_LL0:;}{
# 234
struct _tuple14 _tmp893;return(_tmp893.f1=nullable,((_tmp893.f2=bound,((_tmp893.f3=zeroterm,((_tmp893.f4=rgn,_tmp893)))))));};}
# 240
struct _tuple0*Cyc_Parse_gensym_enum(){
# 242
static int enum_counter=0;
struct _dyneither_ptr*_tmp8A0;const char*_tmp89F;void*_tmp89E[1];struct Cyc_Int_pa_PrintArg_struct _tmp89D;struct _tuple0*_tmp89C;return(_tmp89C=_cycalloc(sizeof(*_tmp89C)),((_tmp89C->f1=Cyc_Absyn_Rel_n(0),((_tmp89C->f2=(
(_tmp8A0=_cycalloc(sizeof(*_tmp8A0)),((_tmp8A0[0]=(struct _dyneither_ptr)((_tmp89D.tag=1,((_tmp89D.f1=(unsigned long)enum_counter ++,((_tmp89E[0]=& _tmp89D,Cyc_aprintf(((_tmp89F="__anonymous_enum_%d__",_tag_dyneither(_tmp89F,sizeof(char),22))),_tag_dyneither(_tmp89E,sizeof(void*),1)))))))),_tmp8A0)))),_tmp89C)))));}struct _tuple15{struct _tuple0*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple16{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple17{struct _tuple15*f1;struct _tuple16*f2;};
# 247
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned int loc,struct _tuple17*field_info){
# 252
struct _tuple0*_tmp26;struct Cyc_Absyn_Tqual _tmp27;void*_tmp28;struct Cyc_List_List*_tmp29;struct Cyc_List_List*_tmp2A;struct Cyc_Absyn_Exp*_tmp2B;struct Cyc_Absyn_Exp*_tmp2C;struct _tuple17*_tmp25=field_info;_tmp26=(_tmp25->f1)->f1;_tmp27=(_tmp25->f1)->f2;_tmp28=(_tmp25->f1)->f3;_tmp29=(_tmp25->f1)->f4;_tmp2A=(_tmp25->f1)->f5;_tmp2B=(_tmp25->f2)->f1;_tmp2C=(_tmp25->f2)->f2;
if(_tmp29 != 0){
const char*_tmp8A1;Cyc_Parse_err(((_tmp8A1="bad type params in struct field",_tag_dyneither(_tmp8A1,sizeof(char),32))),loc);}
if(Cyc_Absyn_is_qvar_qualified(_tmp26)){
const char*_tmp8A2;Cyc_Parse_err(((_tmp8A2="struct or union field cannot be qualified with a namespace",_tag_dyneither(_tmp8A2,sizeof(char),59))),loc);}{
struct Cyc_Absyn_Aggrfield*_tmp8A3;return(_tmp8A3=_cycalloc(sizeof(*_tmp8A3)),((_tmp8A3->name=(*_tmp26).f2,((_tmp8A3->tq=_tmp27,((_tmp8A3->type=_tmp28,((_tmp8A3->width=_tmp2B,((_tmp8A3->attributes=_tmp2A,((_tmp8A3->requires_clause=_tmp2C,_tmp8A3)))))))))))));};}
# 262
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp8A4;return(_tmp8A4.Signed_spec=0,((_tmp8A4.Unsigned_spec=0,((_tmp8A4.Short_spec=0,((_tmp8A4.Long_spec=0,((_tmp8A4.Long_Long_spec=0,((_tmp8A4.Valid_type_spec=0,((_tmp8A4.Type_spec=Cyc_Absyn_sint_typ,((_tmp8A4.loc=loc,_tmp8A4)))))))))))))));}
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
struct Cyc_String_pa_PrintArg_struct _tmp8AC;void*_tmp8AB[1];const char*_tmp8AA;struct _dyneither_ptr*_tmp8A9;struct _dyneither_ptr*nm=(_tmp8A9=_cycalloc(sizeof(*_tmp8A9)),((_tmp8A9[0]=(struct _dyneither_ptr)((_tmp8AC.tag=0,((_tmp8AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp39),((_tmp8AB[0]=& _tmp8AC,Cyc_aprintf(((_tmp8AA="`%s",_tag_dyneither(_tmp8AA,sizeof(char),4))),_tag_dyneither(_tmp8AB,sizeof(void*),1)))))))),_tmp8A9)));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8BB;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp8BA;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp8B9;struct Cyc_Absyn_Tvar*_tmp8B8;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8B7;*_tmp42=(void*)((_tmp8B7=_cycalloc(sizeof(*_tmp8B7)),((_tmp8B7[0]=((_tmp8BB.tag=2,((_tmp8BB.f1=((_tmp8B8=_cycalloc(sizeof(*_tmp8B8)),((_tmp8B8->name=nm,((_tmp8B8->identity=- 1,((_tmp8B8->kind=(void*)((_tmp8BA=_cycalloc(sizeof(*_tmp8BA)),((_tmp8BA[0]=((_tmp8B9.tag=0,((_tmp8B9.f1=& Cyc_Tcutil_ik,_tmp8B9)))),_tmp8BA)))),_tmp8B8)))))))),_tmp8BB)))),_tmp8B7))));}
goto _LL18;}_LL1B:;_LL1C:
 goto _LL18;_LL18:;}
# 330
{struct _tuple18*_tmp8BE;struct Cyc_List_List*_tmp8BD;_tmp36=((_tmp8BD=_cycalloc(sizeof(*_tmp8BD)),((_tmp8BD->hd=((_tmp8BE=_cycalloc(sizeof(*_tmp8BE)),((_tmp8BE->f1=_tmp39,((_tmp8BE->f2=_tmp3B,_tmp8BE)))))),((_tmp8BD->tl=_tmp36,_tmp8BD))))));}goto _LL11;_LL14: if(_tmp38->f1 == 0)goto _LL16;_tmp3C=*_tmp38->f1;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp3D=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp38->f3;if(_tmp3D->tag != 15)goto _LL16;else{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)((void*)_tmp3D->f1);if(_tmp3E->tag != 1)goto _LL16;else{_tmp3F=(void**)((void**)& _tmp3E->f2);}}};_LL15: {
# 334
struct Cyc_String_pa_PrintArg_struct _tmp8C6;void*_tmp8C5[1];const char*_tmp8C4;struct _dyneither_ptr*_tmp8C3;struct _dyneither_ptr*nm=(_tmp8C3=_cycalloc(sizeof(*_tmp8C3)),((_tmp8C3[0]=(struct _dyneither_ptr)((_tmp8C6.tag=0,((_tmp8C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3C),((_tmp8C5[0]=& _tmp8C6,Cyc_aprintf(((_tmp8C4="`%s",_tag_dyneither(_tmp8C4,sizeof(char),4))),_tag_dyneither(_tmp8C5,sizeof(void*),1)))))))),_tmp8C3)));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8D5;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp8D4;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp8D3;struct Cyc_Absyn_Tvar*_tmp8D2;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8D1;*_tmp3F=(void*)((_tmp8D1=_cycalloc(sizeof(*_tmp8D1)),((_tmp8D1[0]=((_tmp8D5.tag=2,((_tmp8D5.f1=((_tmp8D2=_cycalloc(sizeof(*_tmp8D2)),((_tmp8D2->name=nm,((_tmp8D2->identity=- 1,((_tmp8D2->kind=(void*)((_tmp8D4=_cycalloc(sizeof(*_tmp8D4)),((_tmp8D4[0]=((_tmp8D3.tag=0,((_tmp8D3.f1=& Cyc_Tcutil_rk,_tmp8D3)))),_tmp8D4)))),_tmp8D2)))))))),_tmp8D5)))),_tmp8D1))));}
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
{struct _tuple18*_tmp8D8;struct Cyc_List_List*_tmp8D7;_tmp57=((_tmp8D7=_cycalloc(sizeof(*_tmp8D7)),((_tmp8D7->hd=((_tmp8D8=_cycalloc(sizeof(*_tmp8D8)),((_tmp8D8->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,((_tmp8D8->f2=_tmp5B,_tmp8D8)))))),((_tmp8D7->tl=_tmp57,_tmp8D7))))));}goto _LL1D;_LL20:;_LL21:
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
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp8DB;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp8DA;return Cyc_Absyn_new_exp((void*)((_tmp8DA=_cycalloc(sizeof(*_tmp8DA)),((_tmp8DA[0]=((_tmp8DB.tag=38,((_tmp8DB.f1=Cyc_Tcutil_copy_type(_tmp66),_tmp8DB)))),_tmp8DA)))),e->loc);}}}
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
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp8E1;struct Cyc_Absyn_ArrayInfo _tmp8E0;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8DF;return(void*)((_tmp8DF=_cycalloc(sizeof(*_tmp8DF)),((_tmp8DF[0]=((_tmp8E1.tag=8,((_tmp8E1.f1=((_tmp8E0.elt_type=_tmp78,((_tmp8E0.tq=_tmp6C,((_tmp8E0.num_elts=nelts2,((_tmp8E0.zero_term=_tmp6E,((_tmp8E0.zt_loc=_tmp6F,_tmp8E0)))))))))),_tmp8E1)))),_tmp8DF))));}
goto _LL27;};}_LL2A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp70=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp69;if(_tmp70->tag != 5)goto _LL2C;else{_tmp71=(_tmp70->f1).elt_typ;_tmp72=(_tmp70->f1).elt_tq;_tmp73=((_tmp70->f1).ptr_atts).rgn;_tmp74=((_tmp70->f1).ptr_atts).nullable;_tmp75=((_tmp70->f1).ptr_atts).bounds;_tmp76=((_tmp70->f1).ptr_atts).zero_term;_tmp77=((_tmp70->f1).ptr_atts).ptrloc;}}_LL2B: {
# 385
void*_tmp7C=Cyc_Parse_substitute_tags(tags,_tmp71);
union Cyc_Absyn_Constraint*_tmp7D=_tmp75;
{union Cyc_Absyn_Constraint _tmp7E=*_tmp75;union Cyc_Absyn_Constraint _tmp7F=_tmp7E;struct Cyc_Absyn_Exp*_tmp81;_LL2F: if((_tmp7F.Eq_constr).tag != 1)goto _LL31;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp80=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)((void*)(_tmp7F.Eq_constr).val);if(_tmp80->tag != 1)goto _LL31;else{_tmp81=_tmp80->f1;}};_LL30:
# 389
 _tmp81=Cyc_Parse_substitute_tags_exp(tags,_tmp81);
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp8E7;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp8E6;union Cyc_Absyn_Constraint*_tmp8E5;_tmp7D=((_tmp8E5=_cycalloc(sizeof(*_tmp8E5)),(((_tmp8E5->Eq_constr).val=(void*)((_tmp8E7=_cycalloc(sizeof(*_tmp8E7)),((_tmp8E7[0]=((_tmp8E6.tag=1,((_tmp8E6.f1=_tmp81,_tmp8E6)))),_tmp8E7)))),(((_tmp8E5->Eq_constr).tag=1,_tmp8E5))))));}
goto _LL2E;_LL31:;_LL32:
 goto _LL2E;_LL2E:;}
# 394
if(_tmp7C != _tmp71  || _tmp7D != _tmp75){
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp8ED;struct Cyc_Absyn_PtrInfo _tmp8EC;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8EB;return(void*)((_tmp8EB=_cycalloc(sizeof(*_tmp8EB)),((_tmp8EB[0]=((_tmp8ED.tag=5,((_tmp8ED.f1=((_tmp8EC.elt_typ=_tmp7C,((_tmp8EC.elt_tq=_tmp72,((_tmp8EC.ptr_atts=(((_tmp8EC.ptr_atts).rgn=_tmp73,(((_tmp8EC.ptr_atts).nullable=_tmp74,(((_tmp8EC.ptr_atts).bounds=_tmp7D,(((_tmp8EC.ptr_atts).zero_term=_tmp76,(((_tmp8EC.ptr_atts).ptrloc=_tmp77,_tmp8EC.ptr_atts)))))))))),_tmp8EC)))))),_tmp8ED)))),_tmp8EB))));}
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
struct _tuple19*_tmp8EE;return(_tmp8EE=_cycalloc(sizeof(*_tmp8EE)),((_tmp8EE->f1=(*t).f2,((_tmp8EE->f2=(*t).f3,_tmp8EE)))));}
# 418
static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d){
void*_tmp89=d->r;void*_tmp8A=_tmp89;struct Cyc_Absyn_Vardecl*_tmp8C;_LL34: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp8B=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8A;if(_tmp8B->tag != 0)goto _LL36;else{_tmp8C=_tmp8B->f1;}}_LL35:
 return _tmp8C;_LL36:;_LL37: {
const char*_tmp8F1;void*_tmp8F0;(_tmp8F0=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(d->loc,((_tmp8F1="bad declaration in `forarray' statement",_tag_dyneither(_tmp8F1,sizeof(char),40))),_tag_dyneither(_tmp8F0,sizeof(void*),0)));}_LL33:;}
# 425
static int Cyc_Parse_is_typeparam(void*tm){
void*_tmp8F=tm;_LL39: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp90=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp8F;if(_tmp90->tag != 4)goto _LL3B;}_LL3A:
 return 1;_LL3B:;_LL3C:
 return 0;_LL38:;}
# 434
static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k){
const char*_tmp8F2;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8F2="`H",_tag_dyneither(_tmp8F2,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_HeapRgn_val;else{
const char*_tmp8F3;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8F3="`U",_tag_dyneither(_tmp8F3,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_UniqueRgn_val;else{
const char*_tmp8F4;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8F4="`RC",_tag_dyneither(_tmp8F4,sizeof(char),4))))== 0)
return(void*)& Cyc_Absyn_RefCntRgn_val;else{
# 442
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8FE;struct _dyneither_ptr*_tmp8FD;struct Cyc_Absyn_Tvar*_tmp8FC;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8FB;return(void*)((_tmp8FB=_cycalloc(sizeof(*_tmp8FB)),((_tmp8FB[0]=((_tmp8FE.tag=2,((_tmp8FE.f1=((_tmp8FC=_cycalloc(sizeof(*_tmp8FC)),((_tmp8FC->name=((_tmp8FD=_cycalloc(sizeof(*_tmp8FD)),((_tmp8FD[0]=s,_tmp8FD)))),((_tmp8FC->identity=- 1,((_tmp8FC->kind=k,_tmp8FC)))))))),_tmp8FE)))),_tmp8FB))));}}}}
# 445
static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t){
void*k;
{void*_tmp98=Cyc_Absyn_compress_kb(t->kind);void*_tmp99=_tmp98;struct Cyc_Absyn_Kind*_tmp9C;void*_tmp9D;_LL3E: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp9A=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9A->tag != 1)goto _LL40;}_LL3F:
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp901;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp900;k=(void*)((_tmp900=_cycalloc(sizeof(*_tmp900)),((_tmp900[0]=((_tmp901.tag=1,((_tmp901.f1=0,_tmp901)))),_tmp900))));}goto _LL3D;_LL40: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9B->tag != 2)goto _LL42;else{_tmp9C=_tmp9B->f2;}}_LL41:
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp904;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp903;k=(void*)((_tmp903=_cycalloc(sizeof(*_tmp903)),((_tmp903[0]=((_tmp904.tag=2,((_tmp904.f1=0,((_tmp904.f2=_tmp9C,_tmp904)))))),_tmp903))));}goto _LL3D;_LL42: _tmp9D=_tmp99;_LL43:
 k=_tmp9D;goto _LL3D;_LL3D:;}{
# 452
struct Cyc_Absyn_Tvar*_tmp905;return(_tmp905=_cycalloc(sizeof(*_tmp905)),((_tmp905->name=t->name,((_tmp905->identity=- 1,((_tmp905->kind=k,_tmp905)))))));};}
# 459
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned int loc,void*t){
void*_tmpA3=t;struct Cyc_Absyn_Tvar*_tmpA5;_LL45: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA4=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA3;if(_tmpA4->tag != 2)goto _LL47;else{_tmpA5=_tmpA4->f1;}}_LL46:
 return _tmpA5;_LL47:;_LL48: {
const char*_tmp908;void*_tmp907;(_tmp907=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp908="expecting a list of type variables, not types",_tag_dyneither(_tmp908,sizeof(char),46))),_tag_dyneither(_tmp907,sizeof(void*),0)));}_LL44:;}
# 465
static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp90B;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp90A;return(void*)((_tmp90A=_cycalloc(sizeof(*_tmp90A)),((_tmp90A[0]=((_tmp90B.tag=2,((_tmp90B.f1=pr,_tmp90B)))),_tmp90A))));}
# 470
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_tmpAA=Cyc_Tcutil_compress(t);void*_tmpAB=_tmpAA;void**_tmpAD;_LL4A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpAC=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpAB;if(_tmpAC->tag != 2)goto _LL4C;else{_tmpAD=(void**)&(_tmpAC->f1)->kind;}}_LL4B: {
# 473
void*_tmpAE=Cyc_Absyn_compress_kb(*_tmpAD);void*_tmpAF=_tmpAE;_LL4F: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpB0=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpAF;if(_tmpB0->tag != 1)goto _LL51;}_LL50:
# 475
 if(!leq)*_tmpAD=Cyc_Tcutil_kind_to_bound(k);else{
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp90E;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp90D;*_tmpAD=(void*)((_tmp90D=_cycalloc(sizeof(*_tmp90D)),((_tmp90D[0]=((_tmp90E.tag=2,((_tmp90E.f1=0,((_tmp90E.f2=k,_tmp90E)))))),_tmp90D))));}
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
{const char*_tmp911;void*_tmp910;(_tmp910=0,Cyc_Tcutil_warn(loc,((_tmp911="function declaration with both new- and old-style parameter declarations; ignoring old-style",_tag_dyneither(_tmp911,sizeof(char),93))),_tag_dyneither(_tmp910,sizeof(void*),0)));}
# 508
return tms;_LL5B: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpB9=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpB7;if(_tmpB9->tag != 0)goto _LL58;else{_tmpBA=_tmpB9->f1;}}_LL5C:
# 510
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpBA)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds)){
const char*_tmp914;void*_tmp913;(_tmp913=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp914="wrong number of parameter declarations in old-style function declaration",_tag_dyneither(_tmp914,sizeof(char),73))),_tag_dyneither(_tmp913,sizeof(void*),0)));}{
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
const char*_tmp917;void*_tmp916;(_tmp916=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpC0->loc,((_tmp917="initializer found in parameter declaration",_tag_dyneither(_tmp917,sizeof(char),43))),_tag_dyneither(_tmp916,sizeof(void*),0)));}
if(Cyc_Absyn_is_qvar_qualified(_tmpC4->name)){
const char*_tmp91A;void*_tmp919;(_tmp919=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpC0->loc,((_tmp91A="namespaces forbidden in parameter declarations",_tag_dyneither(_tmp91A,sizeof(char),47))),_tag_dyneither(_tmp919,sizeof(void*),0)));}
{struct _tuple8*_tmp91D;struct Cyc_List_List*_tmp91C;rev_new_params=(
(_tmp91C=_cycalloc(sizeof(*_tmp91C)),((_tmp91C->hd=((_tmp91D=_cycalloc(sizeof(*_tmp91D)),((_tmp91D->f1=(*_tmpC4->name).f2,((_tmp91D->f2=_tmpC4->tq,((_tmp91D->f3=_tmpC4->type,_tmp91D)))))))),((_tmp91C->tl=rev_new_params,_tmp91C))))));}
# 530
goto L;_LL60:;_LL61: {
const char*_tmp920;void*_tmp91F;(_tmp91F=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpC0->loc,((_tmp920="nonvariable declaration in parameter type",_tag_dyneither(_tmp920,sizeof(char),42))),_tag_dyneither(_tmp91F,sizeof(void*),0)));}_LL5D:;}
# 534
L: if(_tmpBF == 0){
const char*_tmp924;void*_tmp923[1];struct Cyc_String_pa_PrintArg_struct _tmp922;(_tmp922.tag=0,((_tmp922.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmpBA->hd)),((_tmp923[0]=& _tmp922,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp924="%s is not given a type",_tag_dyneither(_tmp924,sizeof(char),23))),_tag_dyneither(_tmp923,sizeof(void*),1)))))));}}{
# 537
struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp933;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp932;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp931;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp930;struct Cyc_List_List*_tmp92F;return
(_tmp92F=_region_malloc(yy,sizeof(*_tmp92F)),((_tmp92F->hd=(void*)((_tmp933=_region_malloc(yy,sizeof(*_tmp933)),((_tmp933[0]=((_tmp930.tag=3,((_tmp930.f1=(void*)((_tmp932=_region_malloc(yy,sizeof(*_tmp932)),((_tmp932[0]=((_tmp931.tag=1,((_tmp931.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_new_params),((_tmp931.f2=0,((_tmp931.f3=0,((_tmp931.f4=0,((_tmp931.f5=0,((_tmp931.f6=0,((_tmp931.f7=0,_tmp931)))))))))))))))),_tmp932)))),_tmp930)))),_tmp933)))),((_tmp92F->tl=0,_tmp92F)))));};};_LL58:;}
# 544
goto _LL57;_LL56:;_LL57: {
struct Cyc_List_List*_tmp934;return(_tmp934=_region_malloc(yy,sizeof(*_tmp934)),((_tmp934->hd=(void*)tms->hd,((_tmp934->tl=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc),_tmp934)))));}_LL53:;};}
# 552
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned int loc){
# 556
if(tds != 0){
struct Cyc_Parse_Declarator _tmp935;d=((_tmp935.id=d.id,((_tmp935.tms=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc),_tmp935))));}{
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
{const char*_tmp936;Cyc_Parse_err(((_tmp936="bad storage class on function",_tag_dyneither(_tmp936,sizeof(char),30))),loc);}break;}}}{
# 578
void*_tmpD9=Cyc_Parse_collapse_type_specifiers(tss,loc);
struct _tuple13 _tmpDA=Cyc_Parse_apply_tms(tq,_tmpD9,atts,d.tms);struct Cyc_Absyn_Tqual _tmpDC;void*_tmpDD;struct Cyc_List_List*_tmpDE;struct Cyc_List_List*_tmpDF;struct _tuple13 _tmpDB=_tmpDA;_tmpDC=_tmpDB.f1;_tmpDD=_tmpDB.f2;_tmpDE=_tmpDB.f3;_tmpDF=_tmpDB.f4;
# 583
if(_tmpDE != 0){
# 586
const char*_tmp939;void*_tmp938;(_tmp938=0,Cyc_Tcutil_warn(loc,((_tmp939="bad type params, ignoring",_tag_dyneither(_tmp939,sizeof(char),26))),_tag_dyneither(_tmp938,sizeof(void*),0)));}{
# 588
void*_tmpE2=_tmpDD;struct Cyc_List_List*_tmpE4;void*_tmpE5;struct Cyc_Absyn_Tqual _tmpE6;void*_tmpE7;struct Cyc_List_List*_tmpE8;int _tmpE9;struct Cyc_Absyn_VarargInfo*_tmpEA;struct Cyc_List_List*_tmpEB;struct Cyc_List_List*_tmpEC;struct Cyc_Absyn_Exp*_tmpED;struct Cyc_List_List*_tmpEE;struct Cyc_Absyn_Exp*_tmpEF;struct Cyc_List_List*_tmpF0;_LL67: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE3=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE2;if(_tmpE3->tag != 9)goto _LL69;else{_tmpE4=(_tmpE3->f1).tvars;_tmpE5=(_tmpE3->f1).effect;_tmpE6=(_tmpE3->f1).ret_tqual;_tmpE7=(_tmpE3->f1).ret_typ;_tmpE8=(_tmpE3->f1).args;_tmpE9=(_tmpE3->f1).c_varargs;_tmpEA=(_tmpE3->f1).cyc_varargs;_tmpEB=(_tmpE3->f1).rgn_po;_tmpEC=(_tmpE3->f1).attributes;_tmpED=(_tmpE3->f1).requires_clause;_tmpEE=(_tmpE3->f1).requires_relns;_tmpEF=(_tmpE3->f1).ensures_clause;_tmpF0=(_tmpE3->f1).ensures_relns;}}_LL68: {
# 592
struct Cyc_List_List*_tmpF1=0;
{struct Cyc_List_List*_tmpF2=_tmpE8;for(0;_tmpF2 != 0;_tmpF2=_tmpF2->tl){
struct _tuple8*_tmpF3=(struct _tuple8*)_tmpF2->hd;struct _dyneither_ptr*_tmpF5;struct Cyc_Absyn_Tqual _tmpF6;void*_tmpF7;struct _tuple8*_tmpF4=_tmpF3;_tmpF5=_tmpF4->f1;_tmpF6=_tmpF4->f2;_tmpF7=_tmpF4->f3;
if(_tmpF5 == 0){
{const char*_tmp93A;Cyc_Parse_err(((_tmp93A="missing argument variable in function prototype",_tag_dyneither(_tmp93A,sizeof(char),48))),loc);}{
struct _tuple8*_tmp93D;struct Cyc_List_List*_tmp93C;_tmpF1=((_tmp93C=_cycalloc(sizeof(*_tmp93C)),((_tmp93C->hd=((_tmp93D=_cycalloc(sizeof(*_tmp93D)),((_tmp93D->f1=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"?",sizeof(char),2),((_tmp93D->f2=_tmpF6,((_tmp93D->f3=_tmpF7,_tmp93D)))))))),((_tmp93C->tl=_tmpF1,_tmp93C))))));};}else{
# 599
struct _tuple8*_tmp940;struct Cyc_List_List*_tmp93F;_tmpF1=((_tmp93F=_cycalloc(sizeof(*_tmp93F)),((_tmp93F->hd=((_tmp940=_cycalloc(sizeof(*_tmp940)),((_tmp940->f1=_tmpF5,((_tmp940->f2=_tmpF6,((_tmp940->f3=_tmpF7,_tmp940)))))))),((_tmp93F->tl=_tmpF1,_tmp93F))))));}}}{
# 603
struct Cyc_Absyn_Fndecl*_tmp941;return(_tmp941=_cycalloc(sizeof(*_tmp941)),((_tmp941->sc=sc,((_tmp941->name=d.id,((_tmp941->tvs=_tmpE4,((_tmp941->is_inline=is_inline,((_tmp941->effect=_tmpE5,((_tmp941->ret_tqual=_tmpE6,((_tmp941->ret_type=_tmpE7,((_tmp941->args=
# 606
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpF1),((_tmp941->c_varargs=_tmpE9,((_tmp941->cyc_varargs=_tmpEA,((_tmp941->rgn_po=_tmpEB,((_tmp941->body=body,((_tmp941->cached_typ=0,((_tmp941->param_vardecls=0,((_tmp941->fn_vardecl=0,((_tmp941->attributes=
# 613
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpEC,_tmpDF),((_tmp941->requires_clause=_tmpED,((_tmp941->requires_relns=0,((_tmp941->ensures_clause=_tmpEF,((_tmp941->ensures_relns=0,_tmp941)))))))))))))))))))))))))))))))))))))))));};}_LL69:;_LL6A: {
# 618
const char*_tmp944;void*_tmp943;(_tmp943=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp944="declarator is not a function prototype",_tag_dyneither(_tmp944,sizeof(char),39))),_tag_dyneither(_tmp943,sizeof(void*),0)));}_LL66:;};};};}static char _tmp101[76]="at most one type may appear within a type specifier \n\t(missing ';' or ','?)";
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
void*_tmp945;(_tmp945=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp945,sizeof(void*),0)));}
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec  && s2.Unsigned_spec){
void*_tmp946;(_tmp946=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp946,sizeof(void*),0)));}
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec  && s2.Short_spec){
void*_tmp947;(_tmp947=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp947,sizeof(void*),0)));}
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec  && s2.Long_Long_spec  || 
s1.Long_Long_spec  && s2.Long_spec) || 
s2.Long_Long_spec  && s1.Long_spec){
void*_tmp948;(_tmp948=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp948,sizeof(void*),0)));}
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
const char*_tmp94B;void*_tmp94A;(_tmp94A=0,Cyc_Tcutil_warn(loc,((_tmp94B="missing type within specifier",_tag_dyneither(_tmp94B,sizeof(char),30))),_tag_dyneither(_tmp94A,sizeof(void*),0)));}
t=Cyc_Absyn_int_typ(sgn,sz);}else{
# 693
if(seen_sign){
void*_tmp10B=t;enum Cyc_Absyn_Sign _tmp10D;enum Cyc_Absyn_Size_of _tmp10E;_LL6C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp10C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp10B;if(_tmp10C->tag != 6)goto _LL6E;else{_tmp10D=_tmp10C->f1;_tmp10E=_tmp10C->f2;}}_LL6D:
# 696
 if(_tmp10D != sgn)
t=Cyc_Absyn_int_typ(sgn,_tmp10E);
goto _LL6B;_LL6E:;_LL6F:
{const char*_tmp94C;Cyc_Parse_err(((_tmp94C="sign specification on non-integral type",_tag_dyneither(_tmp94C,sizeof(char),40))),loc);}goto _LL6B;_LL6B:;}
# 701
if(seen_size){
void*_tmp110=t;enum Cyc_Absyn_Sign _tmp112;enum Cyc_Absyn_Size_of _tmp113;_LL71: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp111=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp110;if(_tmp111->tag != 6)goto _LL73;else{_tmp112=_tmp111->f1;_tmp113=_tmp111->f2;}}_LL72:
# 704
 if(_tmp113 != sz)
t=Cyc_Absyn_int_typ(_tmp112,sz);
goto _LL70;_LL73: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp114=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp110;if(_tmp114->tag != 7)goto _LL75;}_LL74:
# 708
 t=Cyc_Absyn_float_typ(2);goto _LL70;_LL75:;_LL76:
{const char*_tmp94D;Cyc_Parse_err(((_tmp94D="size qualifier on non-integral type",_tag_dyneither(_tmp94D,sizeof(char),36))),loc);}goto _LL70;_LL70:;}}
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
struct _tuple15*_tmp950;struct Cyc_List_List*_tmp94F;return(_tmp94F=_region_malloc(r,sizeof(*_tmp94F)),((_tmp94F->hd=((_tmp950=_region_malloc(r,sizeof(*_tmp950)),((_tmp950->f1=_tmp116,((_tmp950->f2=_tmp119,((_tmp950->f3=_tmp11A,((_tmp950->f4=_tmp11B,((_tmp950->f5=_tmp11C,_tmp950)))))))))))),((_tmp94F->tl=0,_tmp94F)))));}else{
# 728
struct _tuple15*_tmp953;struct Cyc_List_List*_tmp952;return(_tmp952=_region_malloc(r,sizeof(*_tmp952)),((_tmp952->hd=((_tmp953=_region_malloc(r,sizeof(*_tmp953)),((_tmp953->f1=_tmp116,((_tmp953->f2=_tmp119,((_tmp953->f3=_tmp11A,((_tmp953->f4=_tmp11B,((_tmp953->f5=_tmp11C,_tmp953)))))))))))),((_tmp952->tl=
Cyc_Parse_apply_tmss(r,tq,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts),_tmp952)))));}};}
# 732
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 735
if(tms == 0){struct _tuple13 _tmp954;return(_tmp954.f1=tq,((_tmp954.f2=t,((_tmp954.f3=0,((_tmp954.f4=atts,_tmp954)))))));}{
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
struct Cyc_List_List*_tmp955;fn_atts=((_tmp955=_cycalloc(sizeof(*_tmp955)),((_tmp955->hd=(void*)as->hd,((_tmp955->tl=fn_atts,_tmp955))))));}else{
# 753
struct Cyc_List_List*_tmp956;new_atts=((_tmp956=_cycalloc(sizeof(*_tmp956)),((_tmp956->hd=(void*)as->hd,((_tmp956->tl=new_atts,_tmp956))))));}}}
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
const char*_tmp959;void*_tmp958;(_tmp958=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp140,((_tmp959="function declaration without parameter types",_tag_dyneither(_tmp959,sizeof(char),45))),_tag_dyneither(_tmp958,sizeof(void*),0)));}_LL84:;}_LL7E: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp12D=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp123;if(_tmp12D->tag != 4)goto _LL80;else{_tmp12E=_tmp12D->f1;_tmp12F=_tmp12D->f2;}}_LL7F:
# 808
 if(tms->tl == 0){
struct _tuple13 _tmp95A;return(_tmp95A.f1=tq,((_tmp95A.f2=t,((_tmp95A.f3=_tmp12E,((_tmp95A.f4=atts,_tmp95A)))))));}{
# 813
const char*_tmp95D;void*_tmp95C;(_tmp95C=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp12F,((_tmp95D="type parameters must appear before function arguments in declarator",_tag_dyneither(_tmp95D,sizeof(char),68))),_tag_dyneither(_tmp95C,sizeof(void*),0)));};_LL80: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp130=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp123;if(_tmp130->tag != 2)goto _LL82;else{_tmp131=_tmp130->f1;_tmp132=_tmp130->f2;}}_LL81: {
# 816
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp963;struct Cyc_Absyn_PtrInfo _tmp962;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp961;return Cyc_Parse_apply_tms(_tmp132,(void*)((_tmp961=_cycalloc(sizeof(*_tmp961)),((_tmp961[0]=((_tmp963.tag=5,((_tmp963.f1=((_tmp962.elt_typ=t,((_tmp962.elt_tq=tq,((_tmp962.ptr_atts=_tmp131,_tmp962)))))),_tmp963)))),_tmp961)))),atts,tms->tl);}_LL82: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp133=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp123;if(_tmp133->tag != 5)goto _LL77;else{_tmp134=_tmp133->f1;_tmp135=_tmp133->f2;}}_LL83:
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
struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct _tmp969;struct Cyc_Absyn_Typedefdecl*_tmp968;struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp967;return Cyc_Absyn_new_decl((void*)((_tmp967=_cycalloc(sizeof(*_tmp967)),((_tmp967[0]=((_tmp969.tag=8,((_tmp969.f1=((_tmp968=_cycalloc(sizeof(*_tmp968)),((_tmp968->name=_tmp157,((_tmp968->tvs=_tmp15A,((_tmp968->kind=kind,((_tmp968->defn=type,((_tmp968->atts=_tmp15B,((_tmp968->tq=_tmp158,_tmp968)))))))))))))),_tmp969)))),_tmp967)))),loc);};};}
# 858
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp96C;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp96B;return Cyc_Absyn_new_stmt((void*)((_tmp96B=_cycalloc(sizeof(*_tmp96B)),((_tmp96B[0]=((_tmp96C.tag=12,((_tmp96C.f1=d,((_tmp96C.f2=s,_tmp96C)))))),_tmp96B)))),d->loc);}
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
const char*_tmp96D;Cyc_Parse_err(((_tmp96D="inline is allowed only on function definitions",_tag_dyneither(_tmp96D,sizeof(char),47))),loc);}{
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
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp973;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp972;struct Cyc_List_List*_tmp971;struct Cyc_List_List*_tmp187=(_tmp971=_cycalloc(sizeof(*_tmp971)),((_tmp971->hd=Cyc_Absyn_new_decl((void*)((_tmp973=_cycalloc(sizeof(*_tmp973)),((_tmp973[0]=((_tmp972.tag=5,((_tmp972.f1=_tmp16F,_tmp972)))),_tmp973)))),loc),((_tmp971->tl=0,_tmp971)))));_npop_handler(0);return _tmp187;};_LL9E: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp170=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp16C;if(_tmp170->tag != 26)goto _LLA0;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp171=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp170->f1)->r;if(_tmp171->tag != 1)goto _LLA0;else{_tmp172=_tmp171->f1;}}}_LL9F:
# 921
 if(_tmp168 != 0){const char*_tmp974;Cyc_Parse_err(((_tmp974="attributes on enum not supported",_tag_dyneither(_tmp974,sizeof(char),33))),loc);}
_tmp172->sc=s;{
struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp97A;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp979;struct Cyc_List_List*_tmp978;struct Cyc_List_List*_tmp18C=(_tmp978=_cycalloc(sizeof(*_tmp978)),((_tmp978->hd=Cyc_Absyn_new_decl((void*)((_tmp97A=_cycalloc(sizeof(*_tmp97A)),((_tmp97A[0]=((_tmp979.tag=7,((_tmp979.f1=_tmp172,_tmp979)))),_tmp97A)))),loc),((_tmp978->tl=0,_tmp978)))));_npop_handler(0);return _tmp18C;};_LLA0: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp173=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp16C;if(_tmp173->tag != 26)goto _LLA2;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp174=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp173->f1)->r;if(_tmp174->tag != 2)goto _LLA2;else{_tmp175=_tmp174->f1;}}}_LLA1:
# 925
 if(_tmp168 != 0){const char*_tmp97B;Cyc_Parse_err(((_tmp97B="attributes on datatypes not supported",_tag_dyneither(_tmp97B,sizeof(char),38))),loc);}
_tmp175->sc=s;{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp981;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp980;struct Cyc_List_List*_tmp97F;struct Cyc_List_List*_tmp191=(_tmp97F=_cycalloc(sizeof(*_tmp97F)),((_tmp97F->hd=Cyc_Absyn_new_decl((void*)((_tmp981=_cycalloc(sizeof(*_tmp981)),((_tmp981[0]=((_tmp980.tag=6,((_tmp980.f1=_tmp175,_tmp980)))),_tmp981)))),loc),((_tmp97F->tl=0,_tmp97F)))));_npop_handler(0);return _tmp191;};_LLA2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp176=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp16C;if(_tmp176->tag != 11)goto _LLA4;else{if((((_tmp176->f1).aggr_info).UnknownAggr).tag != 1)goto _LLA4;_tmp177=((struct _tuple2)(((_tmp176->f1).aggr_info).UnknownAggr).val).f1;_tmp178=((struct _tuple2)(((_tmp176->f1).aggr_info).UnknownAggr).val).f2;_tmp179=(_tmp176->f1).targs;}}_LLA3: {
# 929
struct Cyc_List_List*_tmp192=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp179);
struct Cyc_Absyn_Aggrdecl*_tmp982;struct Cyc_Absyn_Aggrdecl*_tmp193=(_tmp982=_cycalloc(sizeof(*_tmp982)),((_tmp982->kind=_tmp177,((_tmp982->sc=s,((_tmp982->name=_tmp178,((_tmp982->tvs=_tmp192,((_tmp982->impl=0,((_tmp982->attributes=0,_tmp982)))))))))))));
if(_tmp168 != 0){const char*_tmp983;Cyc_Parse_err(((_tmp983="bad attributes on type declaration",_tag_dyneither(_tmp983,sizeof(char),35))),loc);}{
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp989;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp988;struct Cyc_List_List*_tmp987;struct Cyc_List_List*_tmp198=(_tmp987=_cycalloc(sizeof(*_tmp987)),((_tmp987->hd=Cyc_Absyn_new_decl((void*)((_tmp989=_cycalloc(sizeof(*_tmp989)),((_tmp989[0]=((_tmp988.tag=5,((_tmp988.f1=_tmp193,_tmp988)))),_tmp989)))),loc),((_tmp987->tl=0,_tmp987)))));_npop_handler(0);return _tmp198;};}_LLA4: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp17A=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp16C;if(_tmp17A->tag != 3)goto _LLA6;else{if((((_tmp17A->f1).datatype_info).KnownDatatype).tag != 2)goto _LLA6;_tmp17B=(struct Cyc_Absyn_Datatypedecl**)(((_tmp17A->f1).datatype_info).KnownDatatype).val;}}_LLA5:
# 934
 if(_tmp168 != 0){const char*_tmp98A;Cyc_Parse_err(((_tmp98A="bad attributes on datatype",_tag_dyneither(_tmp98A,sizeof(char),27))),loc);}{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp990;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp98F;struct Cyc_List_List*_tmp98E;struct Cyc_List_List*_tmp19E=(_tmp98E=_cycalloc(sizeof(*_tmp98E)),((_tmp98E->hd=Cyc_Absyn_new_decl((void*)((_tmp990=_cycalloc(sizeof(*_tmp990)),((_tmp990[0]=((_tmp98F.tag=6,((_tmp98F.f1=*_tmp17B,_tmp98F)))),_tmp990)))),loc),((_tmp98E->tl=0,_tmp98E)))));_npop_handler(0);return _tmp19E;};_LLA6: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp17C=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp16C;if(_tmp17C->tag != 3)goto _LLA8;else{if((((_tmp17C->f1).datatype_info).UnknownDatatype).tag != 1)goto _LLA8;_tmp17D=((struct Cyc_Absyn_UnknownDatatypeInfo)(((_tmp17C->f1).datatype_info).UnknownDatatype).val).name;_tmp17E=((struct Cyc_Absyn_UnknownDatatypeInfo)(((_tmp17C->f1).datatype_info).UnknownDatatype).val).is_extensible;_tmp17F=(_tmp17C->f1).targs;}}_LLA7: {
# 937
struct Cyc_List_List*_tmp19F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp17F);
struct Cyc_Absyn_Decl*_tmp1A0=Cyc_Absyn_datatype_decl(s,_tmp17D,_tmp19F,0,_tmp17E,loc);
if(_tmp168 != 0){const char*_tmp991;Cyc_Parse_err(((_tmp991="bad attributes on datatype",_tag_dyneither(_tmp991,sizeof(char),27))),loc);}{
struct Cyc_List_List*_tmp992;struct Cyc_List_List*_tmp1A3=(_tmp992=_cycalloc(sizeof(*_tmp992)),((_tmp992->hd=_tmp1A0,((_tmp992->tl=0,_tmp992)))));_npop_handler(0);return _tmp1A3;};}_LLA8: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp180=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp16C;if(_tmp180->tag != 13)goto _LLAA;else{_tmp181=_tmp180->f1;}}_LLA9: {
# 942
struct Cyc_Absyn_Enumdecl*_tmp993;struct Cyc_Absyn_Enumdecl*_tmp1A4=(_tmp993=_cycalloc(sizeof(*_tmp993)),((_tmp993->sc=s,((_tmp993->name=_tmp181,((_tmp993->fields=0,_tmp993)))))));
if(_tmp168 != 0){const char*_tmp994;Cyc_Parse_err(((_tmp994="bad attributes on enum",_tag_dyneither(_tmp994,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp99E;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp99D;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp99C;struct Cyc_List_List*_tmp99B;struct Cyc_List_List*_tmp1AA=(_tmp99B=_cycalloc(sizeof(*_tmp99B)),((_tmp99B->hd=((_tmp99E=_cycalloc(sizeof(*_tmp99E)),((_tmp99E->r=(void*)((_tmp99C=_cycalloc(sizeof(*_tmp99C)),((_tmp99C[0]=((_tmp99D.tag=7,((_tmp99D.f1=_tmp1A4,_tmp99D)))),_tmp99C)))),((_tmp99E->loc=loc,_tmp99E)))))),((_tmp99B->tl=0,_tmp99B)))));_npop_handler(0);return _tmp1AA;};}_LLAA: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp182=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp16C;if(_tmp182->tag != 14)goto _LLAC;else{_tmp183=_tmp182->f1;}}_LLAB: {
# 948
struct Cyc_Core_Opt*_tmp9A1;struct Cyc_Absyn_Enumdecl*_tmp9A0;struct Cyc_Absyn_Enumdecl*_tmp1AC=(_tmp9A0=_cycalloc(sizeof(*_tmp9A0)),((_tmp9A0->sc=s,((_tmp9A0->name=Cyc_Parse_gensym_enum(),((_tmp9A0->fields=((_tmp9A1=_cycalloc(sizeof(*_tmp9A1)),((_tmp9A1->v=_tmp183,_tmp9A1)))),_tmp9A0)))))));
if(_tmp168 != 0){const char*_tmp9A2;Cyc_Parse_err(((_tmp9A2="bad attributes on enum",_tag_dyneither(_tmp9A2,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp9AC;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp9AB;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp9AA;struct Cyc_List_List*_tmp9A9;struct Cyc_List_List*_tmp1B2=(_tmp9A9=_cycalloc(sizeof(*_tmp9A9)),((_tmp9A9->hd=((_tmp9AC=_cycalloc(sizeof(*_tmp9AC)),((_tmp9AC->r=(void*)((_tmp9AA=_cycalloc(sizeof(*_tmp9AA)),((_tmp9AA[0]=((_tmp9AB.tag=7,((_tmp9AB.f1=_tmp1AC,_tmp9AB)))),_tmp9AA)))),((_tmp9AC->loc=loc,_tmp9AC)))))),((_tmp9A9->tl=0,_tmp9A9)))));_npop_handler(0);return _tmp1B2;};}_LLAC:;_LLAD:
{const char*_tmp9AD;Cyc_Parse_err(((_tmp9AD="missing declarator",_tag_dyneither(_tmp9AD,sizeof(char),19))),loc);}{struct Cyc_List_List*_tmp1B6=0;_npop_handler(0);return _tmp1B6;};_LL9B:;}else{
# 955
struct Cyc_List_List*_tmp1B7=Cyc_Parse_apply_tmss(mkrgn,_tmp166,_tmp16B,declarators,_tmp168);
if(istypedef){
# 960
if(!exps_empty){
const char*_tmp9AE;Cyc_Parse_err(((_tmp9AE="initializer in typedef declaration",_tag_dyneither(_tmp9AE,sizeof(char),35))),loc);}{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(unsigned int,struct _tuple15*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp1B7);
struct Cyc_List_List*_tmp1B9=decls;_npop_handler(0);return _tmp1B9;};}else{
# 966
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp1BA=_tmp1B7;for(0;_tmp1BA != 0;(_tmp1BA=_tmp1BA->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple15*_tmp1BB=(struct _tuple15*)_tmp1BA->hd;struct _tuple0*_tmp1BD;struct Cyc_Absyn_Tqual _tmp1BE;void*_tmp1BF;struct Cyc_List_List*_tmp1C0;struct Cyc_List_List*_tmp1C1;struct _tuple15*_tmp1BC=_tmp1BB;_tmp1BD=_tmp1BC->f1;_tmp1BE=_tmp1BC->f2;_tmp1BF=_tmp1BC->f3;_tmp1C0=_tmp1BC->f4;_tmp1C1=_tmp1BC->f5;
if(_tmp1C0 != 0){
const char*_tmp9B1;void*_tmp9B0;(_tmp9B0=0,Cyc_Tcutil_warn(loc,((_tmp9B1="bad type params, ignoring",_tag_dyneither(_tmp9B1,sizeof(char),26))),_tag_dyneither(_tmp9B0,sizeof(void*),0)));}
if(exprs == 0){
const char*_tmp9B4;void*_tmp9B3;(_tmp9B3=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp9B4="unexpected NULL in parse!",_tag_dyneither(_tmp9B4,sizeof(char),26))),_tag_dyneither(_tmp9B3,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp1C6=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd;
struct Cyc_Absyn_Vardecl*_tmp1C7=Cyc_Absyn_new_vardecl(_tmp1BD,_tmp1BF,_tmp1C6);
_tmp1C7->tq=_tmp1BE;
_tmp1C7->sc=s;
_tmp1C7->attributes=_tmp1C1;{
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp9BA;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp9B9;struct Cyc_Absyn_Decl*_tmp9B8;struct Cyc_Absyn_Decl*_tmp1C8=(_tmp9B8=_cycalloc(sizeof(*_tmp9B8)),((_tmp9B8->r=(void*)((_tmp9BA=_cycalloc(sizeof(*_tmp9BA)),((_tmp9BA[0]=((_tmp9B9.tag=0,((_tmp9B9.f1=_tmp1C7,_tmp9B9)))),_tmp9BA)))),((_tmp9B8->loc=loc,_tmp9B8)))));
struct Cyc_List_List*_tmp9BB;decls=((_tmp9BB=_cycalloc(sizeof(*_tmp9BB)),((_tmp9BB->hd=_tmp1C8,((_tmp9BB->tl=decls,_tmp9BB))))));};};}}{
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
{const char*_tmp9C0;void*_tmp9BF[2];struct Cyc_String_pa_PrintArg_struct _tmp9BE;struct Cyc_Int_pa_PrintArg_struct _tmp9BD;Cyc_Parse_err((struct _dyneither_ptr)((_tmp9BD.tag=1,((_tmp9BD.f1=(unsigned long)((int)Cyc_strlen((struct _dyneither_ptr)s)),((_tmp9BE.tag=0,((_tmp9BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp9BF[0]=& _tmp9BE,((_tmp9BF[1]=& _tmp9BD,Cyc_aprintf(((_tmp9C0="bad kind: %s; strlen=%d",_tag_dyneither(_tmp9C0,sizeof(char),24))),_tag_dyneither(_tmp9BF,sizeof(void*),2)))))))))))))),loc);}
return& Cyc_Tcutil_bk;}
# 1022
static int Cyc_Parse_exp2int(unsigned int loc,struct Cyc_Absyn_Exp*e){
void*_tmp1D5=e->r;void*_tmp1D6=_tmp1D5;int _tmp1D8;_LLC5: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1D7=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1D6;if(_tmp1D7->tag != 0)goto _LLC7;else{if(((_tmp1D7->f1).Int_c).tag != 5)goto _LLC7;_tmp1D8=((struct _tuple5)((_tmp1D7->f1).Int_c).val).f2;}}_LLC6:
 return _tmp1D8;_LLC7:;_LLC8:
# 1026
{const char*_tmp9C1;Cyc_Parse_err(((_tmp9C1="expecting integer constant",_tag_dyneither(_tmp9C1,sizeof(char),27))),loc);}
return 0;_LLC4:;}
# 1032
static struct _dyneither_ptr Cyc_Parse_exp2string(unsigned int loc,struct Cyc_Absyn_Exp*e){
void*_tmp1DA=e->r;void*_tmp1DB=_tmp1DA;struct _dyneither_ptr _tmp1DD;_LLCA: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1DC=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1DB;if(_tmp1DC->tag != 0)goto _LLCC;else{if(((_tmp1DC->f1).String_c).tag != 8)goto _LLCC;_tmp1DD=(struct _dyneither_ptr)((_tmp1DC->f1).String_c).val;}}_LLCB:
 return _tmp1DD;_LLCC:;_LLCD:
# 1036
{const char*_tmp9C2;Cyc_Parse_err(((_tmp9C2="expecting string constant",_tag_dyneither(_tmp9C2,sizeof(char),26))),loc);}
return _tag_dyneither(0,0,0);_LLC9:;}
# 1042
static unsigned int Cyc_Parse_cnst2uint(unsigned int loc,union Cyc_Absyn_Cnst x){
union Cyc_Absyn_Cnst _tmp1DF=x;int _tmp1E0;char _tmp1E1;long long _tmp1E2;_LLCF: if((_tmp1DF.Int_c).tag != 5)goto _LLD1;_tmp1E0=((struct _tuple5)(_tmp1DF.Int_c).val).f2;_LLD0:
 return(unsigned int)_tmp1E0;_LLD1: if((_tmp1DF.Char_c).tag != 2)goto _LLD3;_tmp1E1=((struct _tuple3)(_tmp1DF.Char_c).val).f2;_LLD2:
 return(unsigned int)_tmp1E1;_LLD3: if((_tmp1DF.LongLong_c).tag != 6)goto _LLD5;_tmp1E2=((struct _tuple6)(_tmp1DF.LongLong_c).val).f2;_LLD4: {
# 1047
unsigned long long y=(unsigned long long)_tmp1E2;
if(y > -1){
const char*_tmp9C3;Cyc_Parse_err(((_tmp9C3="integer constant too large",_tag_dyneither(_tmp9C3,sizeof(char),27))),loc);}
return(unsigned int)_tmp1E2;}_LLD5:;_LLD6:
# 1052
{const char*_tmp9C7;void*_tmp9C6[1];struct Cyc_String_pa_PrintArg_struct _tmp9C5;Cyc_Parse_err((struct _dyneither_ptr)((_tmp9C5.tag=0,((_tmp9C5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_cnst2string(x)),((_tmp9C6[0]=& _tmp9C5,Cyc_aprintf(((_tmp9C7="expected integer constant but found %s",_tag_dyneither(_tmp9C7,sizeof(char),39))),_tag_dyneither(_tmp9C6,sizeof(void*),1)))))))),loc);}
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
{const char*_tmp9C8;Cyc_Parse_err(((_tmp9C8="cannot mix patterns and expressions in case",_tag_dyneither(_tmp9C8,sizeof(char),44))),p->loc);}
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
union Cyc_YYSTYPE Cyc_Int_tok(union Cyc_Absyn_Cnst yy1){union Cyc_YYSTYPE _tmp9C9;return((_tmp9C9.Int_tok).val=yy1,(((_tmp9C9.Int_tok).tag=1,_tmp9C9)));}static char _tmp205[5]="char";
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
union Cyc_YYSTYPE Cyc_Char_tok(char yy1){union Cyc_YYSTYPE _tmp9CA;return((_tmp9CA.Char_tok).val=yy1,(((_tmp9CA.Char_tok).tag=2,_tmp9CA)));}static char _tmp20A[13]="string_t<`H>";
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
union Cyc_YYSTYPE Cyc_String_tok(struct _dyneither_ptr yy1){union Cyc_YYSTYPE _tmp9CB;return((_tmp9CB.String_tok).val=yy1,(((_tmp9CB.String_tok).tag=3,_tmp9CB)));}static char _tmp20F[56]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
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
union Cyc_YYSTYPE Cyc_YY1(struct _tuple21*yy1){union Cyc_YYSTYPE _tmp9CC;return((_tmp9CC.YY1).val=yy1,(((_tmp9CC.YY1).tag=9,_tmp9CC)));}static char _tmp214[19]="conref_t<bounds_t>";
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
union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp9CD;return((_tmp9CD.YY2).val=yy1,(((_tmp9CD.YY2).tag=10,_tmp9CD)));}static char _tmp219[28]="list_t<offsetof_field_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9CE;return((_tmp9CE.YY3).val=yy1,(((_tmp9CE.YY3).tag=11,_tmp9CE)));}static char _tmp21E[6]="exp_t";
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
union Cyc_YYSTYPE Cyc_Exp_tok(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp9CF;return((_tmp9CF.Exp_tok).val=yy1,(((_tmp9CF.Exp_tok).tag=7,_tmp9CF)));}static char _tmp223[17]="list_t<exp_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY4(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D0;return((_tmp9D0.YY4).val=yy1,(((_tmp9D0.YY4).tag=12,_tmp9D0)));}static char _tmp228[47]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
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
union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D1;return((_tmp9D1.YY5).val=yy1,(((_tmp9D1.YY5).tag=13,_tmp9D1)));}static char _tmp22D[9]="primop_t";
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
union Cyc_YYSTYPE Cyc_YY6(enum Cyc_Absyn_Primop yy1){union Cyc_YYSTYPE _tmp9D2;return((_tmp9D2.YY6).val=yy1,(((_tmp9D2.YY6).tag=14,_tmp9D2)));}static char _tmp232[19]="opt_t<primop_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY7(struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _tmp9D3;return((_tmp9D3.YY7).val=yy1,(((_tmp9D3.YY7).tag=15,_tmp9D3)));}static char _tmp237[7]="qvar_t";
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
union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){union Cyc_YYSTYPE _tmp9D4;return((_tmp9D4.QualId_tok).val=yy1,(((_tmp9D4.QualId_tok).tag=5,_tmp9D4)));}static char _tmp23C[7]="stmt_t";
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
union Cyc_YYSTYPE Cyc_Stmt_tok(struct Cyc_Absyn_Stmt*yy1){union Cyc_YYSTYPE _tmp9D5;return((_tmp9D5.Stmt_tok).val=yy1,(((_tmp9D5.Stmt_tok).tag=8,_tmp9D5)));}static char _tmp241[27]="list_t<switch_clause_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY8(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D6;return((_tmp9D6.YY8).val=yy1,(((_tmp9D6.YY8).tag=16,_tmp9D6)));}static char _tmp246[6]="pat_t";
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
union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Pat*yy1){union Cyc_YYSTYPE _tmp9D7;return((_tmp9D7.YY9).val=yy1,(((_tmp9D7.YY9).tag=17,_tmp9D7)));}static char _tmp24B[28]="$(list_t<pat_t,`H>,bool)@`H";
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
union Cyc_YYSTYPE Cyc_YY10(struct _tuple22*yy1){union Cyc_YYSTYPE _tmp9D8;return((_tmp9D8.YY10).val=yy1,(((_tmp9D8.YY10).tag=18,_tmp9D8)));}static char _tmp250[17]="list_t<pat_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D9;return((_tmp9D9.YY11).val=yy1,(((_tmp9D9.YY11).tag=19,_tmp9D9)));}static char _tmp255[36]="$(list_t<designator_t,`H>,pat_t)@`H";
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
union Cyc_YYSTYPE Cyc_YY12(struct _tuple23*yy1){union Cyc_YYSTYPE _tmp9DA;return((_tmp9DA.YY12).val=yy1,(((_tmp9DA.YY12).tag=20,_tmp9DA)));}static char _tmp25A[47]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
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
union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9DB;return((_tmp9DB.YY13).val=yy1,(((_tmp9DB.YY13).tag=21,_tmp9DB)));}static char _tmp25F[58]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
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
union Cyc_YYSTYPE Cyc_YY14(struct _tuple22*yy1){union Cyc_YYSTYPE _tmp9DC;return((_tmp9DC.YY14).val=yy1,(((_tmp9DC.YY14).tag=22,_tmp9DC)));}static char _tmp264[9]="fndecl_t";
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
union Cyc_YYSTYPE Cyc_YY15(struct Cyc_Absyn_Fndecl*yy1){union Cyc_YYSTYPE _tmp9DD;return((_tmp9DD.YY15).val=yy1,(((_tmp9DD.YY15).tag=23,_tmp9DD)));}static char _tmp269[18]="list_t<decl_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9DE;return((_tmp9DE.YY16).val=yy1,(((_tmp9DE.YY16).tag=24,_tmp9DE)));}static char _tmp26E[12]="decl_spec_t";
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
union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Parse_Declaration_spec yy1){union Cyc_YYSTYPE _tmp9DF;return((_tmp9DF.YY17).val=yy1,(((_tmp9DF.YY17).tag=25,_tmp9DF)));}static char _tmp273[31]="$(declarator_t<`yy>,exp_opt_t)";
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
union Cyc_YYSTYPE Cyc_YY18(struct _tuple11 yy1){union Cyc_YYSTYPE _tmp9E0;return((_tmp9E0.YY18).val=yy1,(((_tmp9E0.YY18).tag=26,_tmp9E0)));}static char _tmp278[23]="declarator_list_t<`yy>";
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
union Cyc_YYSTYPE Cyc_YY19(struct _tuple12*yy1){union Cyc_YYSTYPE _tmp9E1;return((_tmp9E1.YY19).val=yy1,(((_tmp9E1.YY19).tag=27,_tmp9E1)));}static char _tmp27D[19]="storage_class_t@`H";
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
union Cyc_YYSTYPE Cyc_YY20(enum Cyc_Parse_Storage_class*yy1){union Cyc_YYSTYPE _tmp9E2;return((_tmp9E2.YY20).val=yy1,(((_tmp9E2.YY20).tag=28,_tmp9E2)));}static char _tmp282[17]="type_specifier_t";
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
union Cyc_YYSTYPE Cyc_YY21(struct Cyc_Parse_Type_specifier yy1){union Cyc_YYSTYPE _tmp9E3;return((_tmp9E3.YY21).val=yy1,(((_tmp9E3.YY21).tag=29,_tmp9E3)));}static char _tmp287[12]="aggr_kind_t";
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
union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Absyn_AggrKind yy1){union Cyc_YYSTYPE _tmp9E4;return((_tmp9E4.YY22).val=yy1,(((_tmp9E4.YY22).tag=30,_tmp9E4)));}static char _tmp28C[8]="tqual_t";
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
union Cyc_YYSTYPE Cyc_YY23(struct Cyc_Absyn_Tqual yy1){union Cyc_YYSTYPE _tmp9E5;return((_tmp9E5.YY23).val=yy1,(((_tmp9E5.YY23).tag=31,_tmp9E5)));}static char _tmp291[23]="list_t<aggrfield_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY24(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E6;return((_tmp9E6.YY24).val=yy1,(((_tmp9E6.YY24).tag=32,_tmp9E6)));}static char _tmp296[34]="list_t<list_t<aggrfield_t,`H>,`H>";
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
union Cyc_YYSTYPE Cyc_YY25(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E7;return((_tmp9E7.YY25).val=yy1,(((_tmp9E7.YY25).tag=33,_tmp9E7)));}static char _tmp29B[33]="list_t<type_modifier_t<`yy>,`yy>";
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
union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E8;return((_tmp9E8.YY26).val=yy1,(((_tmp9E8.YY26).tag=34,_tmp9E8)));}static char _tmp2A0[18]="declarator_t<`yy>";
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
union Cyc_YYSTYPE Cyc_YY27(struct Cyc_Parse_Declarator yy1){union Cyc_YYSTYPE _tmp9E9;return((_tmp9E9.YY27).val=yy1,(((_tmp9E9.YY27).tag=35,_tmp9E9)));}static char _tmp2A5[45]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
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
union Cyc_YYSTYPE Cyc_YY28(struct _tuple24*yy1){union Cyc_YYSTYPE _tmp9EA;return((_tmp9EA.YY28).val=yy1,(((_tmp9EA.YY28).tag=36,_tmp9EA)));}static char _tmp2AA[57]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
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
union Cyc_YYSTYPE Cyc_YY29(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9EB;return((_tmp9EB.YY29).val=yy1,(((_tmp9EB.YY29).tag=37,_tmp9EB)));}static char _tmp2AF[26]="abstractdeclarator_t<`yy>";
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
union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator yy1){union Cyc_YYSTYPE _tmp9EC;return((_tmp9EC.YY30).val=yy1,(((_tmp9EC.YY30).tag=38,_tmp9EC)));}static char _tmp2B4[5]="bool";
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
union Cyc_YYSTYPE Cyc_YY31(int yy1){union Cyc_YYSTYPE _tmp9ED;return((_tmp9ED.YY31).val=yy1,(((_tmp9ED.YY31).tag=39,_tmp9ED)));}static char _tmp2B9[8]="scope_t";
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
union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope yy1){union Cyc_YYSTYPE _tmp9EE;return((_tmp9EE.YY32).val=yy1,(((_tmp9EE.YY32).tag=40,_tmp9EE)));}static char _tmp2BE[16]="datatypefield_t";
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
union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){union Cyc_YYSTYPE _tmp9EF;return((_tmp9EF.YY33).val=yy1,(((_tmp9EF.YY33).tag=41,_tmp9EF)));}static char _tmp2C3[27]="list_t<datatypefield_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F0;return((_tmp9F0.YY34).val=yy1,(((_tmp9F0.YY34).tag=42,_tmp9F0)));}static char _tmp2C8[41]="$(tqual_t,type_specifier_t,attributes_t)";
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
union Cyc_YYSTYPE Cyc_YY35(struct _tuple25 yy1){union Cyc_YYSTYPE _tmp9F1;return((_tmp9F1.YY35).val=yy1,(((_tmp9F1.YY35).tag=43,_tmp9F1)));}static char _tmp2CD[17]="list_t<var_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY36(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F2;return((_tmp9F2.YY36).val=yy1,(((_tmp9F2.YY36).tag=44,_tmp9F2)));}static char _tmp2D2[31]="$(var_opt_t,tqual_t,type_t)@`H";
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
union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1){union Cyc_YYSTYPE _tmp9F3;return((_tmp9F3.YY37).val=yy1,(((_tmp9F3.YY37).tag=45,_tmp9F3)));}static char _tmp2D7[42]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
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
union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F4;return((_tmp9F4.YY38).val=yy1,(((_tmp9F4.YY38).tag=46,_tmp9F4)));}static char _tmp2DC[115]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
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
union Cyc_YYSTYPE Cyc_YY39(struct _tuple26*yy1){union Cyc_YYSTYPE _tmp9F5;return((_tmp9F5.YY39).val=yy1,(((_tmp9F5.YY39).tag=47,_tmp9F5)));}static char _tmp2E1[18]="list_t<type_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY40(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F6;return((_tmp9F6.YY40).val=yy1,(((_tmp9F6.YY40).tag=48,_tmp9F6)));}static char _tmp2E6[24]="list_t<designator_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F7;return((_tmp9F7.YY41).val=yy1,(((_tmp9F7.YY41).tag=49,_tmp9F7)));}static char _tmp2EB[13]="designator_t";
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
union Cyc_YYSTYPE Cyc_YY42(void*yy1){union Cyc_YYSTYPE _tmp9F8;return((_tmp9F8.YY42).val=yy1,(((_tmp9F8.YY42).tag=50,_tmp9F8)));}static char _tmp2F0[7]="kind_t";
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
union Cyc_YYSTYPE Cyc_YY43(struct Cyc_Absyn_Kind*yy1){union Cyc_YYSTYPE _tmp9F9;return((_tmp9F9.YY43).val=yy1,(((_tmp9F9.YY43).tag=51,_tmp9F9)));}static char _tmp2F5[7]="type_t";
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
union Cyc_YYSTYPE Cyc_YY44(void*yy1){union Cyc_YYSTYPE _tmp9FA;return((_tmp9FA.YY44).val=yy1,(((_tmp9FA.YY44).tag=52,_tmp9FA)));}static char _tmp2FA[23]="list_t<attribute_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY45(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9FB;return((_tmp9FB.YY45).val=yy1,(((_tmp9FB.YY45).tag=53,_tmp9FB)));}static char _tmp2FF[12]="attribute_t";
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
union Cyc_YYSTYPE Cyc_YY46(void*yy1){union Cyc_YYSTYPE _tmp9FC;return((_tmp9FC.YY46).val=yy1,(((_tmp9FC.YY46).tag=54,_tmp9FC)));}static char _tmp304[12]="enumfield_t";
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
union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){union Cyc_YYSTYPE _tmp9FD;return((_tmp9FD.YY47).val=yy1,(((_tmp9FD.YY47).tag=55,_tmp9FD)));}static char _tmp309[23]="list_t<enumfield_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9FE;return((_tmp9FE.YY48).val=yy1,(((_tmp9FE.YY48).tag=56,_tmp9FE)));}static char _tmp30E[11]="type_opt_t";
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
union Cyc_YYSTYPE Cyc_YY49(void*yy1){union Cyc_YYSTYPE _tmp9FF;return((_tmp9FF.YY49).val=yy1,(((_tmp9FF.YY49).tag=57,_tmp9FF)));}static char _tmp313[31]="list_t<$(type_t,type_t)@`H,`H>";
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
union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA00;return((_tmpA00.YY50).val=yy1,(((_tmpA00.YY50).tag=58,_tmpA00)));}static char _tmp318[15]="conref_t<bool>";
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
union Cyc_YYSTYPE Cyc_YY51(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmpA01;return((_tmpA01.YY51).val=yy1,(((_tmpA01.YY51).tag=59,_tmpA01)));}static char _tmp31D[45]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";
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
union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA02;return((_tmpA02.YY52).val=yy1,(((_tmpA02.YY52).tag=60,_tmpA02)));}static char _tmp322[20]="pointer_qual_t<`yy>";
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
union Cyc_YYSTYPE Cyc_YY53(void*yy1){union Cyc_YYSTYPE _tmpA03;return((_tmpA03.YY53).val=yy1,(((_tmpA03.YY53).tag=61,_tmpA03)));}static char _tmp327[21]="pointer_quals_t<`yy>";
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
union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA04;return((_tmpA04.YY54).val=yy1,(((_tmpA04.YY54).tag=62,_tmpA04)));}static char _tmp32C[21]="$(bool,string_t<`H>)";
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
union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple20 yy1){union Cyc_YYSTYPE _tmpA05;return((_tmpA05.Asm_tok).val=yy1,(((_tmpA05.Asm_tok).tag=6,_tmpA05)));}static char _tmp331[10]="exp_opt_t";
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
union Cyc_YYSTYPE Cyc_YY55(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmpA06;return((_tmpA06.YY55).val=yy1,(((_tmpA06.YY55).tag=63,_tmpA06)));}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1236
typedef struct Cyc_Yyltype Cyc_yyltype;
# 1240
struct Cyc_Yyltype Cyc_yynewloc(){
struct Cyc_Yyltype _tmpA07;return(_tmpA07.timestamp=0,((_tmpA07.first_line=0,((_tmpA07.first_column=0,((_tmpA07.last_line=0,((_tmpA07.last_column=0,_tmpA07)))))))));}
# 1243
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1254 "parse.y"
static short Cyc_yytranslate[373]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,144,2,2,128,142,139,2,125,126,133,136,121,140,130,141,2,2,2,2,2,2,2,2,2,2,129,118,123,122,124,135,134,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,131,2,132,138,127,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,119,137,120,143,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117};static char _tmp336[2]="$";static char _tmp337[6]="error";static char _tmp338[12]="$undefined.";static char _tmp339[5]="AUTO";static char _tmp33A[9]="REGISTER";static char _tmp33B[7]="STATIC";static char _tmp33C[7]="EXTERN";static char _tmp33D[8]="TYPEDEF";static char _tmp33E[5]="VOID";static char _tmp33F[5]="CHAR";static char _tmp340[6]="SHORT";static char _tmp341[4]="INT";static char _tmp342[5]="LONG";static char _tmp343[6]="FLOAT";static char _tmp344[7]="DOUBLE";static char _tmp345[7]="SIGNED";static char _tmp346[9]="UNSIGNED";static char _tmp347[6]="CONST";static char _tmp348[9]="VOLATILE";static char _tmp349[9]="RESTRICT";static char _tmp34A[7]="STRUCT";static char _tmp34B[6]="UNION";static char _tmp34C[5]="CASE";static char _tmp34D[8]="DEFAULT";static char _tmp34E[7]="INLINE";static char _tmp34F[7]="SIZEOF";static char _tmp350[9]="OFFSETOF";static char _tmp351[3]="IF";static char _tmp352[5]="ELSE";static char _tmp353[7]="SWITCH";static char _tmp354[6]="WHILE";static char _tmp355[3]="DO";static char _tmp356[4]="FOR";static char _tmp357[5]="GOTO";static char _tmp358[9]="CONTINUE";static char _tmp359[6]="BREAK";static char _tmp35A[7]="RETURN";static char _tmp35B[5]="ENUM";static char _tmp35C[8]="NULL_kw";static char _tmp35D[4]="LET";static char _tmp35E[6]="THROW";static char _tmp35F[4]="TRY";static char _tmp360[6]="CATCH";static char _tmp361[7]="EXPORT";static char _tmp362[4]="NEW";static char _tmp363[9]="ABSTRACT";static char _tmp364[9]="FALLTHRU";static char _tmp365[6]="USING";static char _tmp366[10]="NAMESPACE";static char _tmp367[9]="DATATYPE";static char _tmp368[7]="MALLOC";static char _tmp369[8]="RMALLOC";static char _tmp36A[7]="CALLOC";static char _tmp36B[8]="RCALLOC";static char _tmp36C[5]="SWAP";static char _tmp36D[9]="REGION_T";static char _tmp36E[6]="TAG_T";static char _tmp36F[7]="REGION";static char _tmp370[5]="RNEW";static char _tmp371[8]="REGIONS";static char _tmp372[13]="RESET_REGION";static char _tmp373[7]="PORTON";static char _tmp374[8]="PORTOFF";static char _tmp375[12]="DYNREGION_T";static char _tmp376[8]="NUMELTS";static char _tmp377[8]="VALUEOF";static char _tmp378[10]="VALUEOF_T";static char _tmp379[9]="TAGCHECK";static char _tmp37A[13]="NUMELTS_QUAL";static char _tmp37B[10]="THIN_QUAL";static char _tmp37C[9]="FAT_QUAL";static char _tmp37D[13]="NOTNULL_QUAL";static char _tmp37E[14]="NULLABLE_QUAL";static char _tmp37F[14]="REQUIRES_QUAL";static char _tmp380[13]="ENSURES_QUAL";static char _tmp381[12]="REGION_QUAL";static char _tmp382[16]="NOZEROTERM_QUAL";static char _tmp383[14]="ZEROTERM_QUAL";static char _tmp384[12]="TAGGED_QUAL";static char _tmp385[16]="EXTENSIBLE_QUAL";static char _tmp386[15]="RESETABLE_QUAL";static char _tmp387[7]="PTR_OP";static char _tmp388[7]="INC_OP";static char _tmp389[7]="DEC_OP";static char _tmp38A[8]="LEFT_OP";static char _tmp38B[9]="RIGHT_OP";static char _tmp38C[6]="LE_OP";static char _tmp38D[6]="GE_OP";static char _tmp38E[6]="EQ_OP";static char _tmp38F[6]="NE_OP";static char _tmp390[7]="AND_OP";static char _tmp391[6]="OR_OP";static char _tmp392[11]="MUL_ASSIGN";static char _tmp393[11]="DIV_ASSIGN";static char _tmp394[11]="MOD_ASSIGN";static char _tmp395[11]="ADD_ASSIGN";static char _tmp396[11]="SUB_ASSIGN";static char _tmp397[12]="LEFT_ASSIGN";static char _tmp398[13]="RIGHT_ASSIGN";static char _tmp399[11]="AND_ASSIGN";static char _tmp39A[11]="XOR_ASSIGN";static char _tmp39B[10]="OR_ASSIGN";static char _tmp39C[9]="ELLIPSIS";static char _tmp39D[11]="LEFT_RIGHT";static char _tmp39E[12]="COLON_COLON";static char _tmp39F[11]="IDENTIFIER";static char _tmp3A0[17]="INTEGER_CONSTANT";static char _tmp3A1[7]="STRING";static char _tmp3A2[8]="WSTRING";static char _tmp3A3[19]="CHARACTER_CONSTANT";static char _tmp3A4[20]="WCHARACTER_CONSTANT";static char _tmp3A5[18]="FLOATING_CONSTANT";static char _tmp3A6[9]="TYPE_VAR";static char _tmp3A7[13]="TYPEDEF_NAME";static char _tmp3A8[16]="QUAL_IDENTIFIER";static char _tmp3A9[18]="QUAL_TYPEDEF_NAME";static char _tmp3AA[10]="ATTRIBUTE";static char _tmp3AB[4]="ASM";static char _tmp3AC[4]="';'";static char _tmp3AD[4]="'{'";static char _tmp3AE[4]="'}'";static char _tmp3AF[4]="','";static char _tmp3B0[4]="'='";static char _tmp3B1[4]="'<'";static char _tmp3B2[4]="'>'";static char _tmp3B3[4]="'('";static char _tmp3B4[4]="')'";static char _tmp3B5[4]="'_'";static char _tmp3B6[4]="'$'";static char _tmp3B7[4]="':'";static char _tmp3B8[4]="'.'";static char _tmp3B9[4]="'['";static char _tmp3BA[4]="']'";static char _tmp3BB[4]="'*'";static char _tmp3BC[4]="'@'";static char _tmp3BD[4]="'?'";static char _tmp3BE[4]="'+'";static char _tmp3BF[4]="'|'";static char _tmp3C0[4]="'^'";static char _tmp3C1[4]="'&'";static char _tmp3C2[4]="'-'";static char _tmp3C3[4]="'/'";static char _tmp3C4[4]="'%'";static char _tmp3C5[4]="'~'";static char _tmp3C6[4]="'!'";static char _tmp3C7[5]="prog";static char _tmp3C8[17]="translation_unit";static char _tmp3C9[16]="extern_c_action";static char _tmp3CA[13]="end_extern_c";static char _tmp3CB[12]="export_list";static char _tmp3CC[19]="export_list_values";static char _tmp3CD[21]="external_declaration";static char _tmp3CE[15]="optional_comma";static char _tmp3CF[20]="function_definition";static char _tmp3D0[21]="function_definition2";static char _tmp3D1[13]="using_action";static char _tmp3D2[15]="unusing_action";static char _tmp3D3[17]="namespace_action";static char _tmp3D4[19]="unnamespace_action";static char _tmp3D5[12]="declaration";static char _tmp3D6[19]="resetable_qual_opt";static char _tmp3D7[17]="declaration_list";static char _tmp3D8[23]="declaration_specifiers";static char _tmp3D9[24]="storage_class_specifier";static char _tmp3DA[15]="attributes_opt";static char _tmp3DB[11]="attributes";static char _tmp3DC[15]="attribute_list";static char _tmp3DD[10]="attribute";static char _tmp3DE[15]="type_specifier";static char _tmp3DF[25]="type_specifier_notypedef";static char _tmp3E0[5]="kind";static char _tmp3E1[15]="type_qualifier";static char _tmp3E2[15]="enum_specifier";static char _tmp3E3[11]="enum_field";static char _tmp3E4[22]="enum_declaration_list";static char _tmp3E5[26]="struct_or_union_specifier";static char _tmp3E6[16]="type_params_opt";static char _tmp3E7[16]="struct_or_union";static char _tmp3E8[24]="struct_declaration_list";static char _tmp3E9[25]="struct_declaration_list0";static char _tmp3EA[21]="init_declarator_list";static char _tmp3EB[22]="init_declarator_list0";static char _tmp3EC[16]="init_declarator";static char _tmp3ED[19]="struct_declaration";static char _tmp3EE[25]="specifier_qualifier_list";static char _tmp3EF[35]="notypedef_specifier_qualifier_list";static char _tmp3F0[23]="struct_declarator_list";static char _tmp3F1[24]="struct_declarator_list0";static char _tmp3F2[18]="struct_declarator";static char _tmp3F3[20]="requires_clause_opt";static char _tmp3F4[19]="ensures_clause_opt";static char _tmp3F5[19]="datatype_specifier";static char _tmp3F6[14]="qual_datatype";static char _tmp3F7[19]="datatypefield_list";static char _tmp3F8[20]="datatypefield_scope";static char _tmp3F9[14]="datatypefield";static char _tmp3FA[11]="declarator";static char _tmp3FB[23]="declarator_withtypedef";static char _tmp3FC[18]="direct_declarator";static char _tmp3FD[30]="direct_declarator_withtypedef";static char _tmp3FE[8]="pointer";static char _tmp3FF[12]="one_pointer";static char _tmp400[14]="pointer_quals";static char _tmp401[13]="pointer_qual";static char _tmp402[23]="pointer_null_and_bound";static char _tmp403[14]="pointer_bound";static char _tmp404[18]="zeroterm_qual_opt";static char _tmp405[8]="rgn_opt";static char _tmp406[11]="tqual_list";static char _tmp407[20]="parameter_type_list";static char _tmp408[9]="type_var";static char _tmp409[16]="optional_effect";static char _tmp40A[19]="optional_rgn_order";static char _tmp40B[10]="rgn_order";static char _tmp40C[16]="optional_inject";static char _tmp40D[11]="effect_set";static char _tmp40E[14]="atomic_effect";static char _tmp40F[11]="region_set";static char _tmp410[15]="parameter_list";static char _tmp411[22]="parameter_declaration";static char _tmp412[16]="identifier_list";static char _tmp413[17]="identifier_list0";static char _tmp414[12]="initializer";static char _tmp415[18]="array_initializer";static char _tmp416[17]="initializer_list";static char _tmp417[12]="designation";static char _tmp418[16]="designator_list";static char _tmp419[11]="designator";static char _tmp41A[10]="type_name";static char _tmp41B[14]="any_type_name";static char _tmp41C[15]="type_name_list";static char _tmp41D[20]="abstract_declarator";static char _tmp41E[27]="direct_abstract_declarator";static char _tmp41F[10]="statement";static char _tmp420[18]="labeled_statement";static char _tmp421[21]="expression_statement";static char _tmp422[19]="compound_statement";static char _tmp423[16]="block_item_list";static char _tmp424[20]="selection_statement";static char _tmp425[15]="switch_clauses";static char _tmp426[20]="iteration_statement";static char _tmp427[15]="jump_statement";static char _tmp428[12]="exp_pattern";static char _tmp429[20]="conditional_pattern";static char _tmp42A[19]="logical_or_pattern";static char _tmp42B[20]="logical_and_pattern";static char _tmp42C[21]="inclusive_or_pattern";static char _tmp42D[21]="exclusive_or_pattern";static char _tmp42E[12]="and_pattern";static char _tmp42F[17]="equality_pattern";static char _tmp430[19]="relational_pattern";static char _tmp431[14]="shift_pattern";static char _tmp432[17]="additive_pattern";static char _tmp433[23]="multiplicative_pattern";static char _tmp434[13]="cast_pattern";static char _tmp435[14]="unary_pattern";static char _tmp436[16]="postfix_pattern";static char _tmp437[16]="primary_pattern";static char _tmp438[8]="pattern";static char _tmp439[19]="tuple_pattern_list";static char _tmp43A[20]="tuple_pattern_list0";static char _tmp43B[14]="field_pattern";static char _tmp43C[19]="field_pattern_list";static char _tmp43D[20]="field_pattern_list0";static char _tmp43E[11]="expression";static char _tmp43F[22]="assignment_expression";static char _tmp440[20]="assignment_operator";static char _tmp441[23]="conditional_expression";static char _tmp442[20]="constant_expression";static char _tmp443[22]="logical_or_expression";static char _tmp444[23]="logical_and_expression";static char _tmp445[24]="inclusive_or_expression";static char _tmp446[24]="exclusive_or_expression";static char _tmp447[15]="and_expression";static char _tmp448[20]="equality_expression";static char _tmp449[22]="relational_expression";static char _tmp44A[17]="shift_expression";static char _tmp44B[20]="additive_expression";static char _tmp44C[26]="multiplicative_expression";static char _tmp44D[16]="cast_expression";static char _tmp44E[17]="unary_expression";static char _tmp44F[15]="unary_operator";static char _tmp450[19]="postfix_expression";static char _tmp451[17]="field_expression";static char _tmp452[19]="primary_expression";static char _tmp453[25]="argument_expression_list";static char _tmp454[26]="argument_expression_list0";static char _tmp455[9]="constant";static char _tmp456[20]="qual_opt_identifier";static char _tmp457[17]="qual_opt_typedef";static char _tmp458[18]="struct_union_name";static char _tmp459[11]="field_name";static char _tmp45A[12]="right_angle";
# 1606 "parse.y"
static struct _dyneither_ptr Cyc_yytname[293]={{_tmp336,_tmp336,_tmp336 + 2},{_tmp337,_tmp337,_tmp337 + 6},{_tmp338,_tmp338,_tmp338 + 12},{_tmp339,_tmp339,_tmp339 + 5},{_tmp33A,_tmp33A,_tmp33A + 9},{_tmp33B,_tmp33B,_tmp33B + 7},{_tmp33C,_tmp33C,_tmp33C + 7},{_tmp33D,_tmp33D,_tmp33D + 8},{_tmp33E,_tmp33E,_tmp33E + 5},{_tmp33F,_tmp33F,_tmp33F + 5},{_tmp340,_tmp340,_tmp340 + 6},{_tmp341,_tmp341,_tmp341 + 4},{_tmp342,_tmp342,_tmp342 + 5},{_tmp343,_tmp343,_tmp343 + 6},{_tmp344,_tmp344,_tmp344 + 7},{_tmp345,_tmp345,_tmp345 + 7},{_tmp346,_tmp346,_tmp346 + 9},{_tmp347,_tmp347,_tmp347 + 6},{_tmp348,_tmp348,_tmp348 + 9},{_tmp349,_tmp349,_tmp349 + 9},{_tmp34A,_tmp34A,_tmp34A + 7},{_tmp34B,_tmp34B,_tmp34B + 6},{_tmp34C,_tmp34C,_tmp34C + 5},{_tmp34D,_tmp34D,_tmp34D + 8},{_tmp34E,_tmp34E,_tmp34E + 7},{_tmp34F,_tmp34F,_tmp34F + 7},{_tmp350,_tmp350,_tmp350 + 9},{_tmp351,_tmp351,_tmp351 + 3},{_tmp352,_tmp352,_tmp352 + 5},{_tmp353,_tmp353,_tmp353 + 7},{_tmp354,_tmp354,_tmp354 + 6},{_tmp355,_tmp355,_tmp355 + 3},{_tmp356,_tmp356,_tmp356 + 4},{_tmp357,_tmp357,_tmp357 + 5},{_tmp358,_tmp358,_tmp358 + 9},{_tmp359,_tmp359,_tmp359 + 6},{_tmp35A,_tmp35A,_tmp35A + 7},{_tmp35B,_tmp35B,_tmp35B + 5},{_tmp35C,_tmp35C,_tmp35C + 8},{_tmp35D,_tmp35D,_tmp35D + 4},{_tmp35E,_tmp35E,_tmp35E + 6},{_tmp35F,_tmp35F,_tmp35F + 4},{_tmp360,_tmp360,_tmp360 + 6},{_tmp361,_tmp361,_tmp361 + 7},{_tmp362,_tmp362,_tmp362 + 4},{_tmp363,_tmp363,_tmp363 + 9},{_tmp364,_tmp364,_tmp364 + 9},{_tmp365,_tmp365,_tmp365 + 6},{_tmp366,_tmp366,_tmp366 + 10},{_tmp367,_tmp367,_tmp367 + 9},{_tmp368,_tmp368,_tmp368 + 7},{_tmp369,_tmp369,_tmp369 + 8},{_tmp36A,_tmp36A,_tmp36A + 7},{_tmp36B,_tmp36B,_tmp36B + 8},{_tmp36C,_tmp36C,_tmp36C + 5},{_tmp36D,_tmp36D,_tmp36D + 9},{_tmp36E,_tmp36E,_tmp36E + 6},{_tmp36F,_tmp36F,_tmp36F + 7},{_tmp370,_tmp370,_tmp370 + 5},{_tmp371,_tmp371,_tmp371 + 8},{_tmp372,_tmp372,_tmp372 + 13},{_tmp373,_tmp373,_tmp373 + 7},{_tmp374,_tmp374,_tmp374 + 8},{_tmp375,_tmp375,_tmp375 + 12},{_tmp376,_tmp376,_tmp376 + 8},{_tmp377,_tmp377,_tmp377 + 8},{_tmp378,_tmp378,_tmp378 + 10},{_tmp379,_tmp379,_tmp379 + 9},{_tmp37A,_tmp37A,_tmp37A + 13},{_tmp37B,_tmp37B,_tmp37B + 10},{_tmp37C,_tmp37C,_tmp37C + 9},{_tmp37D,_tmp37D,_tmp37D + 13},{_tmp37E,_tmp37E,_tmp37E + 14},{_tmp37F,_tmp37F,_tmp37F + 14},{_tmp380,_tmp380,_tmp380 + 13},{_tmp381,_tmp381,_tmp381 + 12},{_tmp382,_tmp382,_tmp382 + 16},{_tmp383,_tmp383,_tmp383 + 14},{_tmp384,_tmp384,_tmp384 + 12},{_tmp385,_tmp385,_tmp385 + 16},{_tmp386,_tmp386,_tmp386 + 15},{_tmp387,_tmp387,_tmp387 + 7},{_tmp388,_tmp388,_tmp388 + 7},{_tmp389,_tmp389,_tmp389 + 7},{_tmp38A,_tmp38A,_tmp38A + 8},{_tmp38B,_tmp38B,_tmp38B + 9},{_tmp38C,_tmp38C,_tmp38C + 6},{_tmp38D,_tmp38D,_tmp38D + 6},{_tmp38E,_tmp38E,_tmp38E + 6},{_tmp38F,_tmp38F,_tmp38F + 6},{_tmp390,_tmp390,_tmp390 + 7},{_tmp391,_tmp391,_tmp391 + 6},{_tmp392,_tmp392,_tmp392 + 11},{_tmp393,_tmp393,_tmp393 + 11},{_tmp394,_tmp394,_tmp394 + 11},{_tmp395,_tmp395,_tmp395 + 11},{_tmp396,_tmp396,_tmp396 + 11},{_tmp397,_tmp397,_tmp397 + 12},{_tmp398,_tmp398,_tmp398 + 13},{_tmp399,_tmp399,_tmp399 + 11},{_tmp39A,_tmp39A,_tmp39A + 11},{_tmp39B,_tmp39B,_tmp39B + 10},{_tmp39C,_tmp39C,_tmp39C + 9},{_tmp39D,_tmp39D,_tmp39D + 11},{_tmp39E,_tmp39E,_tmp39E + 12},{_tmp39F,_tmp39F,_tmp39F + 11},{_tmp3A0,_tmp3A0,_tmp3A0 + 17},{_tmp3A1,_tmp3A1,_tmp3A1 + 7},{_tmp3A2,_tmp3A2,_tmp3A2 + 8},{_tmp3A3,_tmp3A3,_tmp3A3 + 19},{_tmp3A4,_tmp3A4,_tmp3A4 + 20},{_tmp3A5,_tmp3A5,_tmp3A5 + 18},{_tmp3A6,_tmp3A6,_tmp3A6 + 9},{_tmp3A7,_tmp3A7,_tmp3A7 + 13},{_tmp3A8,_tmp3A8,_tmp3A8 + 16},{_tmp3A9,_tmp3A9,_tmp3A9 + 18},{_tmp3AA,_tmp3AA,_tmp3AA + 10},{_tmp3AB,_tmp3AB,_tmp3AB + 4},{_tmp3AC,_tmp3AC,_tmp3AC + 4},{_tmp3AD,_tmp3AD,_tmp3AD + 4},{_tmp3AE,_tmp3AE,_tmp3AE + 4},{_tmp3AF,_tmp3AF,_tmp3AF + 4},{_tmp3B0,_tmp3B0,_tmp3B0 + 4},{_tmp3B1,_tmp3B1,_tmp3B1 + 4},{_tmp3B2,_tmp3B2,_tmp3B2 + 4},{_tmp3B3,_tmp3B3,_tmp3B3 + 4},{_tmp3B4,_tmp3B4,_tmp3B4 + 4},{_tmp3B5,_tmp3B5,_tmp3B5 + 4},{_tmp3B6,_tmp3B6,_tmp3B6 + 4},{_tmp3B7,_tmp3B7,_tmp3B7 + 4},{_tmp3B8,_tmp3B8,_tmp3B8 + 4},{_tmp3B9,_tmp3B9,_tmp3B9 + 4},{_tmp3BA,_tmp3BA,_tmp3BA + 4},{_tmp3BB,_tmp3BB,_tmp3BB + 4},{_tmp3BC,_tmp3BC,_tmp3BC + 4},{_tmp3BD,_tmp3BD,_tmp3BD + 4},{_tmp3BE,_tmp3BE,_tmp3BE + 4},{_tmp3BF,_tmp3BF,_tmp3BF + 4},{_tmp3C0,_tmp3C0,_tmp3C0 + 4},{_tmp3C1,_tmp3C1,_tmp3C1 + 4},{_tmp3C2,_tmp3C2,_tmp3C2 + 4},{_tmp3C3,_tmp3C3,_tmp3C3 + 4},{_tmp3C4,_tmp3C4,_tmp3C4 + 4},{_tmp3C5,_tmp3C5,_tmp3C5 + 4},{_tmp3C6,_tmp3C6,_tmp3C6 + 4},{_tmp3C7,_tmp3C7,_tmp3C7 + 5},{_tmp3C8,_tmp3C8,_tmp3C8 + 17},{_tmp3C9,_tmp3C9,_tmp3C9 + 16},{_tmp3CA,_tmp3CA,_tmp3CA + 13},{_tmp3CB,_tmp3CB,_tmp3CB + 12},{_tmp3CC,_tmp3CC,_tmp3CC + 19},{_tmp3CD,_tmp3CD,_tmp3CD + 21},{_tmp3CE,_tmp3CE,_tmp3CE + 15},{_tmp3CF,_tmp3CF,_tmp3CF + 20},{_tmp3D0,_tmp3D0,_tmp3D0 + 21},{_tmp3D1,_tmp3D1,_tmp3D1 + 13},{_tmp3D2,_tmp3D2,_tmp3D2 + 15},{_tmp3D3,_tmp3D3,_tmp3D3 + 17},{_tmp3D4,_tmp3D4,_tmp3D4 + 19},{_tmp3D5,_tmp3D5,_tmp3D5 + 12},{_tmp3D6,_tmp3D6,_tmp3D6 + 19},{_tmp3D7,_tmp3D7,_tmp3D7 + 17},{_tmp3D8,_tmp3D8,_tmp3D8 + 23},{_tmp3D9,_tmp3D9,_tmp3D9 + 24},{_tmp3DA,_tmp3DA,_tmp3DA + 15},{_tmp3DB,_tmp3DB,_tmp3DB + 11},{_tmp3DC,_tmp3DC,_tmp3DC + 15},{_tmp3DD,_tmp3DD,_tmp3DD + 10},{_tmp3DE,_tmp3DE,_tmp3DE + 15},{_tmp3DF,_tmp3DF,_tmp3DF + 25},{_tmp3E0,_tmp3E0,_tmp3E0 + 5},{_tmp3E1,_tmp3E1,_tmp3E1 + 15},{_tmp3E2,_tmp3E2,_tmp3E2 + 15},{_tmp3E3,_tmp3E3,_tmp3E3 + 11},{_tmp3E4,_tmp3E4,_tmp3E4 + 22},{_tmp3E5,_tmp3E5,_tmp3E5 + 26},{_tmp3E6,_tmp3E6,_tmp3E6 + 16},{_tmp3E7,_tmp3E7,_tmp3E7 + 16},{_tmp3E8,_tmp3E8,_tmp3E8 + 24},{_tmp3E9,_tmp3E9,_tmp3E9 + 25},{_tmp3EA,_tmp3EA,_tmp3EA + 21},{_tmp3EB,_tmp3EB,_tmp3EB + 22},{_tmp3EC,_tmp3EC,_tmp3EC + 16},{_tmp3ED,_tmp3ED,_tmp3ED + 19},{_tmp3EE,_tmp3EE,_tmp3EE + 25},{_tmp3EF,_tmp3EF,_tmp3EF + 35},{_tmp3F0,_tmp3F0,_tmp3F0 + 23},{_tmp3F1,_tmp3F1,_tmp3F1 + 24},{_tmp3F2,_tmp3F2,_tmp3F2 + 18},{_tmp3F3,_tmp3F3,_tmp3F3 + 20},{_tmp3F4,_tmp3F4,_tmp3F4 + 19},{_tmp3F5,_tmp3F5,_tmp3F5 + 19},{_tmp3F6,_tmp3F6,_tmp3F6 + 14},{_tmp3F7,_tmp3F7,_tmp3F7 + 19},{_tmp3F8,_tmp3F8,_tmp3F8 + 20},{_tmp3F9,_tmp3F9,_tmp3F9 + 14},{_tmp3FA,_tmp3FA,_tmp3FA + 11},{_tmp3FB,_tmp3FB,_tmp3FB + 23},{_tmp3FC,_tmp3FC,_tmp3FC + 18},{_tmp3FD,_tmp3FD,_tmp3FD + 30},{_tmp3FE,_tmp3FE,_tmp3FE + 8},{_tmp3FF,_tmp3FF,_tmp3FF + 12},{_tmp400,_tmp400,_tmp400 + 14},{_tmp401,_tmp401,_tmp401 + 13},{_tmp402,_tmp402,_tmp402 + 23},{_tmp403,_tmp403,_tmp403 + 14},{_tmp404,_tmp404,_tmp404 + 18},{_tmp405,_tmp405,_tmp405 + 8},{_tmp406,_tmp406,_tmp406 + 11},{_tmp407,_tmp407,_tmp407 + 20},{_tmp408,_tmp408,_tmp408 + 9},{_tmp409,_tmp409,_tmp409 + 16},{_tmp40A,_tmp40A,_tmp40A + 19},{_tmp40B,_tmp40B,_tmp40B + 10},{_tmp40C,_tmp40C,_tmp40C + 16},{_tmp40D,_tmp40D,_tmp40D + 11},{_tmp40E,_tmp40E,_tmp40E + 14},{_tmp40F,_tmp40F,_tmp40F + 11},{_tmp410,_tmp410,_tmp410 + 15},{_tmp411,_tmp411,_tmp411 + 22},{_tmp412,_tmp412,_tmp412 + 16},{_tmp413,_tmp413,_tmp413 + 17},{_tmp414,_tmp414,_tmp414 + 12},{_tmp415,_tmp415,_tmp415 + 18},{_tmp416,_tmp416,_tmp416 + 17},{_tmp417,_tmp417,_tmp417 + 12},{_tmp418,_tmp418,_tmp418 + 16},{_tmp419,_tmp419,_tmp419 + 11},{_tmp41A,_tmp41A,_tmp41A + 10},{_tmp41B,_tmp41B,_tmp41B + 14},{_tmp41C,_tmp41C,_tmp41C + 15},{_tmp41D,_tmp41D,_tmp41D + 20},{_tmp41E,_tmp41E,_tmp41E + 27},{_tmp41F,_tmp41F,_tmp41F + 10},{_tmp420,_tmp420,_tmp420 + 18},{_tmp421,_tmp421,_tmp421 + 21},{_tmp422,_tmp422,_tmp422 + 19},{_tmp423,_tmp423,_tmp423 + 16},{_tmp424,_tmp424,_tmp424 + 20},{_tmp425,_tmp425,_tmp425 + 15},{_tmp426,_tmp426,_tmp426 + 20},{_tmp427,_tmp427,_tmp427 + 15},{_tmp428,_tmp428,_tmp428 + 12},{_tmp429,_tmp429,_tmp429 + 20},{_tmp42A,_tmp42A,_tmp42A + 19},{_tmp42B,_tmp42B,_tmp42B + 20},{_tmp42C,_tmp42C,_tmp42C + 21},{_tmp42D,_tmp42D,_tmp42D + 21},{_tmp42E,_tmp42E,_tmp42E + 12},{_tmp42F,_tmp42F,_tmp42F + 17},{_tmp430,_tmp430,_tmp430 + 19},{_tmp431,_tmp431,_tmp431 + 14},{_tmp432,_tmp432,_tmp432 + 17},{_tmp433,_tmp433,_tmp433 + 23},{_tmp434,_tmp434,_tmp434 + 13},{_tmp435,_tmp435,_tmp435 + 14},{_tmp436,_tmp436,_tmp436 + 16},{_tmp437,_tmp437,_tmp437 + 16},{_tmp438,_tmp438,_tmp438 + 8},{_tmp439,_tmp439,_tmp439 + 19},{_tmp43A,_tmp43A,_tmp43A + 20},{_tmp43B,_tmp43B,_tmp43B + 14},{_tmp43C,_tmp43C,_tmp43C + 19},{_tmp43D,_tmp43D,_tmp43D + 20},{_tmp43E,_tmp43E,_tmp43E + 11},{_tmp43F,_tmp43F,_tmp43F + 22},{_tmp440,_tmp440,_tmp440 + 20},{_tmp441,_tmp441,_tmp441 + 23},{_tmp442,_tmp442,_tmp442 + 20},{_tmp443,_tmp443,_tmp443 + 22},{_tmp444,_tmp444,_tmp444 + 23},{_tmp445,_tmp445,_tmp445 + 24},{_tmp446,_tmp446,_tmp446 + 24},{_tmp447,_tmp447,_tmp447 + 15},{_tmp448,_tmp448,_tmp448 + 20},{_tmp449,_tmp449,_tmp449 + 22},{_tmp44A,_tmp44A,_tmp44A + 17},{_tmp44B,_tmp44B,_tmp44B + 20},{_tmp44C,_tmp44C,_tmp44C + 26},{_tmp44D,_tmp44D,_tmp44D + 16},{_tmp44E,_tmp44E,_tmp44E + 17},{_tmp44F,_tmp44F,_tmp44F + 15},{_tmp450,_tmp450,_tmp450 + 19},{_tmp451,_tmp451,_tmp451 + 17},{_tmp452,_tmp452,_tmp452 + 19},{_tmp453,_tmp453,_tmp453 + 25},{_tmp454,_tmp454,_tmp454 + 26},{_tmp455,_tmp455,_tmp455 + 9},{_tmp456,_tmp456,_tmp456 + 20},{_tmp457,_tmp457,_tmp457 + 17},{_tmp458,_tmp458,_tmp458 + 18},{_tmp459,_tmp459,_tmp459 + 11},{_tmp45A,_tmp45A,_tmp45A + 12}};
# 1661
static short Cyc_yyr1[526]={0,145,146,146,146,146,146,146,146,146,146,146,147,148,149,149,150,150,150,151,151,151,152,152,153,153,153,153,154,154,155,156,157,158,159,159,159,159,159,159,159,160,160,161,161,162,162,162,162,162,162,162,162,162,162,163,163,163,163,163,163,163,164,164,165,166,166,167,167,167,167,168,168,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,170,171,171,171,172,172,172,173,173,174,174,174,175,175,175,175,175,176,176,177,177,178,178,179,179,180,181,181,182,182,183,184,184,184,184,184,184,185,185,185,185,185,185,186,187,187,188,188,188,188,189,189,190,190,191,191,191,192,192,193,193,193,193,194,194,194,195,195,196,196,197,197,198,198,198,198,198,198,198,198,198,198,199,199,199,199,199,199,199,199,199,199,199,200,200,201,202,202,203,203,203,203,203,203,203,203,204,204,204,205,205,206,206,206,207,207,207,208,208,209,209,209,209,210,210,211,211,212,212,213,213,214,214,215,215,216,216,216,216,217,217,218,218,219,219,219,220,221,221,222,222,223,223,223,223,223,224,224,224,224,225,225,226,226,227,227,228,228,229,229,229,229,229,230,230,231,231,231,232,232,232,232,232,232,232,232,232,232,232,233,233,233,233,233,233,233,234,235,235,236,236,237,237,237,237,237,237,237,237,238,238,238,238,238,238,239,239,239,239,239,239,240,240,240,240,240,240,240,240,240,240,240,240,240,240,241,241,241,241,241,241,241,241,242,243,243,244,244,245,245,246,246,247,247,248,248,249,249,249,250,250,250,250,250,251,251,251,252,252,252,253,253,253,253,254,254,255,255,255,255,255,255,256,257,258,258,258,258,258,258,258,258,258,258,258,258,258,258,258,258,259,259,259,260,260,261,261,262,262,262,263,263,264,264,265,265,265,266,266,266,266,266,266,266,266,266,266,266,267,267,267,267,267,267,267,268,269,269,270,270,271,271,272,272,273,273,274,274,274,275,275,275,275,275,276,276,276,277,277,277,278,278,278,278,279,279,280,280,280,280,280,280,280,280,280,280,280,280,280,280,280,280,280,280,280,281,281,281,282,282,282,282,282,282,282,282,282,282,283,283,283,283,284,284,284,284,284,284,284,284,284,284,285,286,286,287,287,287,287,287,288,288,289,289,290,290,291,291,292,292};
# 1717
static short Cyc_yyr2[526]={0,1,2,3,5,3,5,5,6,3,3,0,2,1,4,3,1,2,3,1,1,1,1,0,3,4,4,5,3,4,2,1,2,1,2,3,5,3,7,4,9,0,1,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,8,1,2,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,4,4,1,4,6,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,2,2,0,3,4,0,4,0,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,6,7,4,4,2,1,1,3,4,4,5,6,7,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,6,5,7,6,4,2,1,1,1,1,1,1,5,3,1,2,2,3,1,2,3,4,1,2,1,2,5,7,7,5,8,6,0,3,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,1,1,1,3,1,1,3,6,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,4,6,9,11,4,6,6,4,1,1,1,1,1,4,3,4,3,3,2,2,6,7,1,1,3,3,1,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1773
static short Cyc_yydefact[1064]={0,21,55,56,57,58,60,73,76,77,78,79,80,81,82,83,97,98,99,115,116,51,0,0,61,0,0,153,90,94,0,0,0,0,0,0,42,516,219,518,517,519,0,0,74,0,205,205,204,1,0,0,19,0,0,20,0,0,45,53,47,71,49,84,85,0,86,0,41,164,0,189,192,87,168,113,59,58,52,0,101,515,0,516,511,512,513,514,113,0,380,0,0,0,0,242,0,382,383,30,32,0,0,0,0,0,0,0,154,0,0,0,0,0,0,0,202,203,0,2,0,0,0,0,0,34,0,121,122,124,46,54,48,50,117,520,521,113,113,41,43,41,0,23,0,221,0,177,165,190,0,196,197,200,201,0,199,198,210,192,0,72,59,105,0,103,0,516,391,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,500,501,480,0,0,0,0,0,483,481,482,0,408,410,424,432,434,436,438,440,442,445,450,453,456,460,0,462,484,499,498,0,392,390,37,0,0,113,0,0,0,131,127,129,262,264,0,0,9,10,0,0,113,522,523,220,96,0,0,169,75,240,0,237,0,0,3,0,5,0,0,0,35,0,0,41,23,0,118,119,144,112,0,151,0,0,0,0,0,0,0,0,0,0,0,0,516,293,295,0,303,297,0,301,285,286,287,0,288,289,290,0,44,23,124,22,24,269,0,227,243,0,0,223,221,0,207,0,0,0,212,62,211,193,0,106,102,0,0,384,0,0,405,516,0,0,0,0,258,401,406,0,403,0,0,470,0,426,460,0,427,428,0,0,0,0,0,0,0,0,0,463,464,41,0,0,0,466,467,465,0,381,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,414,415,416,417,418,419,420,421,422,423,413,0,468,0,490,491,0,0,0,503,0,113,398,399,0,396,0,244,0,0,0,0,265,0,235,132,137,133,135,128,130,221,0,271,263,272,525,524,0,89,93,0,91,95,111,68,67,0,65,170,221,239,166,271,241,178,179,0,88,206,13,0,31,0,33,0,39,0,0,123,125,246,245,23,25,108,120,0,0,0,139,140,147,0,113,113,159,0,0,0,0,0,516,0,41,0,332,333,334,0,0,336,0,0,41,0,304,298,124,302,296,294,26,0,176,228,0,0,0,234,222,229,147,0,0,0,223,175,209,208,171,207,0,0,213,63,114,107,431,104,100,394,395,261,0,402,256,259,389,0,257,0,0,0,516,247,252,0,0,0,0,0,0,0,0,0,0,0,0,0,502,509,0,508,409,433,0,435,437,439,441,443,444,448,449,446,447,451,452,454,455,457,458,459,412,411,489,486,0,488,0,0,0,386,0,393,36,0,387,0,266,0,138,134,136,0,223,0,207,0,273,0,221,0,284,268,0,113,0,0,0,131,0,113,0,221,0,188,167,238,0,7,0,4,6,0,0,27,0,143,126,144,0,0,142,223,152,161,160,0,0,155,0,0,0,311,0,0,0,0,0,0,331,335,0,0,0,0,299,292,41,28,270,221,0,231,0,0,149,224,0,147,227,215,172,194,195,213,191,0,260,404,407,469,0,0,248,0,253,472,0,0,0,0,476,479,0,0,0,0,0,461,505,0,0,487,485,0,0,397,400,388,267,236,147,0,274,275,207,0,0,223,207,0,92,223,516,0,64,66,0,180,0,0,223,0,207,0,0,8,0,0,141,0,145,117,150,162,159,159,0,0,0,0,0,0,0,0,0,0,0,0,0,311,337,0,0,300,29,223,0,232,230,0,173,0,149,223,0,214,385,495,0,494,0,249,254,0,0,0,0,429,430,489,488,507,0,510,425,504,506,0,149,147,277,283,147,0,276,207,117,0,69,181,187,147,0,186,182,207,15,0,16,0,38,0,0,0,158,157,305,311,0,0,0,0,0,0,339,340,342,344,346,348,350,352,355,360,363,366,370,372,378,379,0,41,308,317,0,0,0,0,0,0,0,0,0,0,338,291,217,233,0,225,174,216,221,471,0,0,255,473,0,0,478,477,492,0,280,149,149,147,278,0,0,149,147,183,14,17,0,0,146,110,0,0,0,311,0,376,0,0,373,41,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,374,312,0,319,0,0,0,327,0,0,0,0,0,0,310,0,0,223,497,496,0,0,0,493,279,282,149,109,0,184,149,18,40,163,306,307,0,0,0,0,311,313,343,0,345,347,349,351,353,354,358,359,356,357,361,362,364,365,367,368,369,0,318,320,321,0,329,328,0,323,0,0,0,148,226,218,0,0,0,0,281,0,185,309,375,0,371,314,0,41,322,330,324,325,0,251,250,474,0,70,0,341,311,315,326,0,377,316,475,0,0,0};
# 1883
static short Cyc_yydefgoto[148]={1061,49,50,459,640,839,51,302,52,286,53,461,54,463,55,56,141,57,58,532,227,445,446,228,61,241,229,63,163,164,64,161,65,263,264,126,127,128,265,230,427,477,478,479,652,792,66,67,657,658,659,68,480,69,450,70,71,158,159,72,116,528,317,696,613,73,614,522,687,514,518,519,421,310,249,94,95,554,469,555,332,333,334,231,303,304,615,433,289,290,291,292,293,294,773,295,296,855,856,857,858,859,860,861,862,863,864,865,866,867,868,869,870,335,412,413,336,337,338,297,198,399,199,537,200,201,202,203,204,205,206,207,208,209,210,211,212,213,800,214,570,571,215,216,75,840,339,437};
# 1901
static short Cyc_yypact[1064]={2868,- -32768,- -32768,- -32768,- -32768,- 51,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3438,157,1023,- -32768,75,- 23,- -32768,14,17,- 4,46,96,117,288,234,- -32768,- -32768,177,- -32768,- -32768,- -32768,167,915,218,226,259,259,- -32768,- -32768,311,2732,- -32768,282,352,- -32768,309,570,3438,3438,3438,- -32768,3438,- -32768,- -32768,663,- -32768,75,3358,187,126,662,833,- -32768,- -32768,295,336,391,- -32768,75,406,- -32768,4050,215,- -32768,- -32768,- -32768,- -32768,295,6219,- -32768,413,455,4050,464,459,493,- -32768,144,- -32768,- -32768,3586,3586,2732,2732,3586,6219,283,- -32768,183,499,675,508,183,3956,6219,- -32768,- -32768,3003,- -32768,2732,3003,2732,3003,71,- -32768,530,538,- -32768,1160,- -32768,- -32768,- -32768,- -32768,3956,- -32768,- -32768,295,82,1609,- -32768,3358,570,550,3586,3512,4570,- -32768,187,- -32768,552,- -32768,- -32768,- -32768,- -32768,556,- -32768,- -32768,123,833,3586,- -32768,- -32768,577,591,598,75,73,- -32768,4050,131,1210,6460,588,6219,6364,600,607,611,618,628,637,642,646,6508,6508,- -32768,- -32768,- -32768,2314,649,6556,6556,6556,- -32768,- -32768,- -32768,- 16,- -32768,- -32768,- 26,669,644,648,651,394,106,468,47,299,- -32768,1159,6556,233,- 9,- -32768,679,1464,697,- -32768,- -32768,702,6219,295,1464,704,3734,3956,4030,3956,759,- -32768,2,2,- -32768,- -32768,85,229,295,- -32768,- -32768,- -32768,- -32768,50,706,- -32768,- -32768,883,247,- -32768,715,720,- -32768,727,- -32768,752,390,737,- -32768,675,4667,3358,550,761,3956,- -32768,1384,764,75,768,772,99,773,4085,792,784,788,797,4764,4085,100,794,795,- -32768,- -32768,800,1751,1751,570,1751,- -32768,- -32768,- -32768,817,- -32768,- -32768,- -32768,379,- -32768,550,805,- -32768,- -32768,803,- 28,838,- -32768,53,819,821,393,820,522,816,6219,3586,- -32768,836,- -32768,- -32768,- 28,75,- -32768,6219,834,- -32768,831,835,- -32768,68,183,6219,4050,334,- -32768,- -32768,- -32768,837,842,829,2314,- -32768,3956,- -32768,- -32768,4182,- -32768,876,6219,6219,6219,6219,6219,6219,3956,708,2314,- -32768,- -32768,1893,843,269,6219,- -32768,- -32768,- -32768,6219,- -32768,6556,6219,6556,6556,6556,6556,6556,6556,6556,6556,6556,6556,6556,6556,6556,6556,6556,6556,6556,6219,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,6219,- -32768,183,- -32768,- -32768,4861,183,6219,- -32768,851,295,- -32768,- -32768,849,856,4050,- -32768,572,1210,852,3586,- -32768,859,860,- -32768,4030,4030,4030,- -32768,- -32768,3151,4958,194,- -32768,454,- -32768,- -32768,53,- -32768,- -32768,3586,- -32768,- -32768,861,- -32768,858,866,864,- -32768,3077,- -32768,519,635,- -32768,- -32768,- -32768,3956,- -32768,- -32768,- -32768,2596,- -32768,2732,- -32768,2732,- -32768,881,863,- -32768,- -32768,- -32768,- -32768,550,- -32768,- -32768,- -32768,1471,6219,882,873,- -32768,1,524,295,295,701,6219,6219,874,885,6219,872,972,2177,887,- -32768,- -32768,- -32768,581,968,- -32768,5055,6219,2035,2451,- -32768,- -32768,1160,- -32768,- -32768,- -32768,- -32768,3586,- -32768,- -32768,3956,886,3808,- -32768,- -32768,871,948,53,897,3882,821,- -32768,- -32768,- -32768,- -32768,522,898,120,806,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3956,- -32768,- -32768,894,- -32768,- -32768,- -32768,- -32768,1254,- -32768,906,912,929,907,- -32768,- -32768,672,4667,909,916,917,918,328,919,920,327,926,922,6412,- -32768,- -32768,927,923,- -32768,669,232,644,648,651,394,106,106,468,468,468,468,47,47,299,299,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,930,- -32768,143,3586,4473,- -32768,3187,- -32768,- -32768,937,- -32768,146,- -32768,3956,- -32768,- -32768,- -32768,932,821,934,522,911,454,3586,3660,5152,- -32768,- -32768,2,295,6316,936,50,3225,940,295,3586,3512,5249,- -32768,519,- -32768,944,- -32768,2732,- -32768,- -32768,943,967,- -32768,807,- -32768,- -32768,1384,952,6219,- -32768,821,- -32768,- -32768,- -32768,953,75,594,360,400,6219,798,407,4085,957,5346,5443,601,- -32768,- -32768,959,969,963,414,1751,- -32768,3358,- -32768,803,973,3586,- -32768,974,53,1009,- -32768,975,948,118,- -32768,- -32768,- -32768,- -32768,806,- -32768,991,- -32768,- -32768,- -32768,978,235,977,- -32768,4279,- -32768,- -32768,6219,1077,6219,6364,- -32768,- -32768,183,183,978,980,4473,- -32768,- -32768,6219,6219,- -32768,- -32768,- 28,718,- -32768,- -32768,- -32768,- -32768,- -32768,948,981,- -32768,- -32768,522,- 28,984,821,522,979,- -32768,821,993,986,- -32768,- -32768,989,- -32768,- 28,990,821,992,522,985,374,- -32768,6219,1001,- -32768,6219,- -32768,3956,- -32768,997,30,701,4085,1005,1000,965,998,1010,4085,6219,5540,612,5637,624,5734,798,- -32768,1017,1018,- -32768,- -32768,821,159,- -32768,- -32768,1013,- -32768,1027,1009,821,3956,- -32768,- -32768,- -32768,285,- -32768,6219,- -32768,- -32768,4667,1014,1019,1022,- -32768,876,1020,1026,- -32768,722,- -32768,- -32768,- -32768,- -32768,4473,1009,948,- -32768,- -32768,948,1028,- -32768,522,3956,1039,- -32768,- -32768,- -32768,948,1029,- -32768,- -32768,522,- -32768,1033,634,428,- -32768,1031,1038,3956,- -32768,- -32768,1121,798,1040,6604,1035,2451,6556,1032,- -32768,33,- -32768,1092,1046,729,781,130,790,432,541,- -32768,- -32768,- -32768,- -32768,1096,6556,1893,- -32768,- -32768,438,4085,465,5831,4085,475,5928,6025,640,1067,- -32768,- -32768,- -32768,- -32768,6219,1068,- -32768,- -32768,973,- -32768,307,250,- -32768,- -32768,3956,1163,- -32768,- -32768,- -32768,4376,- -32768,1009,1009,948,- -32768,1070,1071,1009,948,- -32768,- -32768,- -32768,283,1075,- -32768,- -32768,500,4085,1074,798,2314,- -32768,3956,1078,- -32768,1428,6556,6219,6556,6556,6556,6556,6556,6556,6556,6556,6556,6556,6556,6556,6556,6556,6556,6556,6556,6219,- -32768,- -32768,1082,- -32768,4085,4085,501,- -32768,4085,4085,520,4085,537,6122,- -32768,1081,53,821,- -32768,- -32768,2451,1084,1076,- -32768,- -32768,- -32768,1009,- -32768,1090,- -32768,1009,- -32768,- -32768,- -32768,- -32768,- -32768,1083,1086,1087,6556,798,- -32768,669,293,644,648,648,394,106,106,468,468,468,468,47,47,299,299,- -32768,- -32768,- -32768,348,- -32768,- -32768,- -32768,4085,- -32768,- -32768,4085,- -32768,4085,4085,540,- -32768,- -32768,- -32768,1098,757,1094,3956,- -32768,1099,- -32768,- -32768,978,235,- -32768,- -32768,6219,1428,- -32768,- -32768,- -32768,- -32768,4085,- -32768,- -32768,- -32768,1103,- -32768,521,- -32768,798,- -32768,- -32768,1104,- -32768,- -32768,- -32768,1222,1231,- -32768};
# 2011
static short Cyc_yypgoto[148]={- -32768,606,- -32768,- -32768,- -32768,314,- -32768,- 214,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,45,- -32768,- 99,18,- -32768,- -32768,0,608,- -32768,40,- 158,1122,13,- -32768,- -32768,- 154,- -32768,122,1200,- 709,- -32768,- -32768,- -32768,1002,999,766,402,- -32768,- -32768,595,- 441,- 705,- -32768,- -32768,129,- -32768,- -32768,128,- 210,1167,- 354,155,- -32768,1091,- -32768,- -32768,1196,- 384,- -32768,554,- 142,- 156,- 103,- 230,277,574,580,- 410,- 453,- 110,- 422,- 117,- -32768,- 243,- 161,- 569,- 238,- -32768,933,- 12,- 82,- 76,- 10,- 285,5,- -32768,- -32768,- 46,- 271,- -32768,- 743,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,152,1043,- -32768,721,845,- -32768,345,585,- -32768,- 133,- 265,- 150,- 357,- 346,- 316,902,- 344,- 365,- 332,- 331,- 330,- 185,480,506,928,248,- -32768,- 372,- -32768,462,324,88,25,- 108,19};
# 2033
static short Cyc_yytable[6749]={59,242,318,308,248,242,363,364,365,579,580,573,324,62,346,504,505,468,507,232,233,59,143,236,575,347,623,400,311,578,261,726,596,637,62,655,656,449,885,78,60,343,309,111,581,582,583,584,472,585,586,59,587,588,844,576,76,434,59,59,59,60,59,684,62,368,542,443,59,147,425,62,62,62,650,62,130,131,132,686,133,62,100,262,320,510,142,434,566,892,137,60,681,511,407,299,435,636,60,60,60,637,60,59,59,366,924,556,60,369,367,688,515,140,103,906,62,62,59,911,59,59,59,59,932,408,435,636,674,59,651,62,238,62,62,62,62,101,436,59,102,59,62,60,60,692,618,142,147,814,- 156,517,62,136,62,444,731,288,60,142,60,60,60,60,104,38,618,535,933,60,434,112,516,169,140,96,256,360,169,60,37,60,988,382,287,129,298,383,993,40,536,170,376,377,257,136,170,- 522,536,589,590,591,976,977,37,160,439,524,981,435,171,647,268,40,422,471,940,941,499,105,432,436,541,513,486,500,149,487,424,378,379,37,735,531,168,38,307,452,630,244,40,426,106,326,- 221,219,694,- 221,794,1039,316,43,438,942,943,440,436,645,327,267,269,59,37,223,366,630,425,425,425,224,300,40,730,1032,62,724,79,1034,491,142,517,109,436,108,498,889,59,59,239,59,770,820,110,594,691,436,240,597,1055,62,62,60,62,42,288,288,298,288,19,20,144,1059,145,706,401,402,403,533,146,429,169,325,113,512,74,430,60,60,549,60,550,287,287,- 243,287,454,- 243,607,170,534,239,799,563,536,565,417,80,98,240,99,366,114,822,366,454,441,826,624,404,59,442,722,556,405,406,517,124,74,455,411,836,366,62,456,894,74,411,288,115,972,907,74,719,908,733,431,762,300,37,135,366,138,913,248,74,568,39,40,41,60,120,121,451,165,287,785,98,678,714,402,403,895,239,970,366,896,506,98,160,748,240,481,1040,763,424,424,424,74,74,680,118,135,384,622,197,74,748,426,426,426,385,386,74,910,74,74,74,74,629,366,635,237,404,915,711,- 12,544,715,406,59,679,59,804,59,330,331,74,805,978,366,122,123,62,982,62,646,62,1041,738,37,556,366,374,375,543,97,768,39,40,41,165,751,59,98,838,98,843,509,162,60,366,60,59,60,422,62,59,677,464,825,142,307,465,828,523,62,753,739,536,62,142,366,834,725,142,166,769,697,366,517,752,600,60,774,361,366,631,668,217,454,784,98,60,737,97,695,60,676,98,366,809,140,380,381,919,97,750,888,688,366,218,810,898,454,954,893,603,416,946,454,42,453,947,1000,1001,994,804,619,787,620,221,805,220,74,431,621,366,795,996,816,453,956,483,999,801,488,366,422,526,527,788,960,953,451,653,654,811,812,1002,1003,1004,1005,74,1006,1007,222,1008,1009,622,997,998,455,366,497,243,967,985,1017,536,37,481,97,786,97,245,42,635,39,40,41,59,366,632,742,633,165,1020,1058,258,475,634,896,341,62,344,344,98,119,366,259,992,366,804,1022,357,358,1046,805,301,930,677,344,344,344,948,37,59,314,59,97,60,315,949,950,40,361,97,952,125,62,604,62,344,366,288,43,142,561,321,671,250,361,366,46,47,48,655,656,695,234,235,322,766,342,574,767,60,631,60,780,323,287,366,298,251,348,252,253,254,255,879,313,349,366,454,922,350,454,98,1027,37,98,882,351,817,366,81,743,39,40,41,598,917,352,728,918,823,536,965,370,448,366,353,1010,1011,1012,430,354,37,832,1054,355,453,848,362,453,39,40,41,875,37,371,134,74,481,74,372,74,971,40,373,969,704,705,97,46,47,48,409,453,43,481,414,344,481,453,1038,415,46,47,48,344,517,37,84,186,187,85,86,87,771,772,40,16,17,18,610,611,612,419,660,661,447,356,664,457,190,669,818,819,458,929,904,905,136,470,675,460,344,466,344,344,344,344,344,344,344,344,344,344,344,344,344,344,344,344,344,936,937,938,939,98,462,59,944,945,97,1048,366,97,247,473,955,482,429,959,62,484,973,493,430,288,46,47,48,846,847,485,489,530,266,150,151,152,153,154,494,1053,155,156,157,247,37,60,989,495,990,492,287,501,39,40,41,871,502,503,98,260,986,801,470,59,475,557,558,559,560,508,562,436,46,47,48,513,62,520,525,569,529,288,521,572,42,453,538,539,344,546,548,540,1028,1015,1016,547,74,1018,1019,368,1021,567,453,60,592,453,599,601,287,602,606,608,625,609,765,626,593,628,643,644,37,569,851,852,627,423,649,428,39,40,41,662,648,665,666,81,663,670,136,685,448,97,672,682,777,779,430,617,46,47,48,1050,37,650,1042,689,693,1043,698,1044,1045,40,266,42,701,702,703,707,- 522,708,709,710,43,59,717,736,721,712,713,344,46,47,48,1056,716,720,62,82,723,729,732,288,734,81,746,756,97,639,749,641,758,642,167,84,759,764,85,86,87,761,781,40,135,60,775,791,88,569,287,782,569,783,853,307,90,91,789,98,798,718,92,793,802,854,807,841,93,194,813,821,195,196,824,827,830,82,829,831,833,837,835,842,876,878,845,881,849,884,850,873,83,84,874,344,85,86,87,886,887,40,890,891,899,470,88,901,900,912,902,897,89,923,90,91,903,916,909,914,92,920,921,925,928,931,93,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,934,935,21,470,951,966,974,968,979,344,980,984,987,247,1033,22,197,23,1014,1031,344,1035,991,24,741,1025,1037,27,1030,745,1036,387,247,28,29,- 41,1047,755,1049,247,1062,32,958,1051,33,962,964,1057,1060,1063,983,97,107,246,747,148,34,35,36,344,135,117,760,1026,757,569,81,797,319,388,389,390,391,392,393,394,395,396,397,467,605,474,796,790,545,418,700,247,0,361,38,39,577,41,42,872,995,139,247,398,260,564,0,1052,0,44,45,247,470,0,81,806,0,808,1013,0,0,0,82,0,0,470,0,0,815,0,0,0,1024,0,328,0,0,329,84,1029,0,85,86,87,0,240,40,0,0,0,0,88,0,927,0,0,344,89,0,90,91,0,330,331,0,92,82,0,0,0,0,93,0,0,344,0,0,0,699,0,0,329,84,0,0,85,86,87,0,240,40,0,344,0,0,88,0,0,0,0,0,89,0,90,91,0,330,331,247,92,0,0,470,0,0,93,0,423,0,0,0,247,0,0,0,0,470,0,0,0,0,0,0,0,344,0,344,344,344,344,344,344,344,344,344,344,344,344,344,344,344,344,344,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,771,772,21,172,173,270,0,271,272,273,274,275,276,277,278,22,81,23,174,279,0,344,175,24,280,0,0,27,176,177,178,179,0,28,29,0,180,0,281,37,470,32,181,182,33,183,0,39,40,41,0,0,81,0,0,0,34,35,36,475,184,185,0,476,0,0,0,46,47,48,344,0,0,0,0,0,0,0,0,266,0,0,0,282,84,186,187,85,86,87,38,39,40,41,42,188,283,139,- 311,0,0,0,0,189,82,44,285,0,0,0,0,191,247,0,192,0,410,193,194,167,84,195,196,85,86,87,37,0,40,0,0,0,0,88,39,40,41,42,0,89,0,90,91,0,266,0,475,92,0,0,0,0,0,93,46,47,48,0,0,0,0,247,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,172,173,270,0,271,272,273,274,275,276,277,278,22,81,23,174,279,0,0,175,24,280,0,0,27,176,177,178,179,0,28,29,0,180,0,281,0,0,32,181,182,33,183,0,0,0,0,0,0,0,0,0,0,34,35,36,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,282,84,186,187,85,86,87,38,39,40,41,42,188,283,139,284,0,0,0,0,189,0,44,285,0,0,0,0,191,0,0,192,0,0,193,194,0,0,195,196,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,172,173,270,0,271,272,273,274,275,276,277,278,22,81,23,174,279,0,0,175,24,280,0,0,27,176,177,178,179,0,28,29,- 41,180,0,281,0,0,32,181,182,33,183,0,0,0,0,0,0,0,0,0,0,34,35,36,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,282,84,186,187,85,86,87,38,39,40,41,42,188,283,139,0,0,0,0,0,189,0,44,285,0,0,0,0,191,0,0,192,0,0,193,194,0,0,195,196,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,172,173,270,0,271,272,273,274,275,276,277,278,22,81,23,174,279,0,0,175,24,280,0,0,27,176,177,178,179,0,28,29,0,180,0,281,0,0,32,181,182,33,183,0,0,0,0,0,0,0,0,0,0,34,35,36,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,282,84,186,187,85,86,87,38,39,40,41,42,188,283,139,0,0,0,0,0,189,0,44,285,0,0,0,0,191,0,0,192,0,0,193,194,0,0,195,196,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,172,173,270,0,271,272,273,274,275,276,277,278,22,81,23,174,279,0,0,175,24,280,0,0,27,176,177,178,179,0,28,29,0,180,0,281,0,0,32,181,182,33,183,0,0,0,0,0,0,0,0,0,0,34,35,36,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,490,84,186,187,85,86,87,38,39,40,41,42,188,283,139,0,0,0,0,0,189,0,44,285,0,0,0,0,191,0,0,192,0,0,193,194,0,0,195,196,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,172,173,0,0,0,0,0,0,0,0,0,0,22,81,23,174,0,0,0,175,24,0,0,0,27,176,177,178,179,0,28,29,0,180,0,0,0,0,32,181,182,33,183,0,0,0,0,0,0,0,0,0,0,34,35,36,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,38,39,40,41,42,188,667,0,0,0,0,0,0,189,0,44,285,0,0,0,0,191,0,0,192,0,0,193,194,0,0,195,196,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,172,173,0,0,0,0,0,0,0,0,0,0,22,81,0,174,0,0,0,175,0,0,0,0,27,176,177,178,179,0,28,29,0,180,0,0,0,0,32,181,182,33,183,0,0,0,0,0,0,0,0,0,0,34,35,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,38,39,40,41,42,188,0,359,0,0,0,0,0,189,0,44,285,0,0,0,0,191,0,0,192,0,0,193,194,0,0,195,196,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,172,173,0,0,0,0,0,0,0,0,0,0,22,81,0,174,0,0,0,175,0,0,0,0,27,176,177,178,179,0,28,29,0,180,0,0,0,0,32,181,182,33,183,0,0,0,0,0,0,0,0,0,0,34,35,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,38,39,40,41,42,188,0,0,0,0,0,0,0,189,0,44,285,0,0,0,0,191,0,0,192,0,0,193,194,0,0,195,196,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,638,0,24,0,25,26,27,0,0,0,0,0,28,29,- 41,0,0,0,30,31,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,36,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,0,0,0,0,0,0,38,39,40,41,42,0,0,0,- 11,0,0,0,0,43,0,44,45,0,0,0,0,46,47,48,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,28,29,- 41,0,0,0,30,31,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,36,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,0,0,0,0,0,0,38,39,40,41,42,0,0,0,- 11,0,0,0,0,43,0,44,45,0,0,0,0,46,47,48,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,28,29,- 41,0,0,0,30,31,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,36,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,0,0,0,0,0,0,38,39,40,41,42,0,0,0,0,0,0,0,0,43,0,44,45,0,0,0,0,46,47,48,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,28,29,- 41,0,0,0,30,31,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,36,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,37,0,0,0,0,0,22,38,39,40,41,42,0,0,0,- 11,0,0,27,0,43,0,44,45,28,29,0,0,46,47,48,0,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,305,0,0,37,0,0,0,0,0,22,38,39,40,41,42,0,307,0,0,0,0,27,0,448,0,44,45,28,29,430,0,46,47,48,0,32,0,0,33,0,0,0,0,0,0,0,81,0,0,0,34,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,305,0,0,0,0,0,0,0,0,22,38,39,0,41,42,0,307,0,0,0,0,27,0,429,82,44,45,28,29,430,0,46,47,48,0,32,727,0,33,167,84,0,0,85,86,87,0,0,40,0,34,35,0,88,0,0,0,0,0,89,0,90,91,0,0,0,0,92,0,0,0,0,0,93,0,0,0,37,0,0,0,0,0,0,38,39,40,41,42,0,0,0,0,0,0,0,0,475,0,44,45,0,0,0,0,46,47,48,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,0,0,27,0,0,0,0,0,28,29,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,36,0,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,38,39,0,41,42,22,0,139,0,0,0,0,0,24,0,44,45,27,0,0,0,0,0,28,29,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,38,39,0,41,42,0,0,0,0,0,0,27,0,0,0,44,45,28,29,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,305,0,0,306,0,0,0,0,0,22,38,39,0,41,42,0,307,0,0,0,0,27,0,0,0,44,45,28,29,0,0,225,0,0,0,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,38,39,0,41,42,0,0,226,0,0,0,27,0,0,0,44,45,28,29,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,305,0,0,0,0,0,0,0,0,22,38,39,0,41,42,0,307,0,0,0,0,27,0,0,0,44,45,28,29,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,38,39,0,41,42,0,0,0,420,0,0,27,0,0,0,44,45,28,29,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,38,39,0,41,42,0,0,0,683,0,0,27,0,0,0,44,45,28,29,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,690,0,0,0,0,0,0,0,0,22,38,39,0,41,42,0,0,0,0,0,0,27,0,0,0,44,45,28,29,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,38,39,0,41,42,0,0,0,0,0,0,27,0,0,0,44,45,28,29,0,81,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,0,0,0,0,34,35,172,173,270,0,271,272,273,274,275,276,277,278,0,81,0,174,279,0,0,175,0,280,0,0,0,176,177,178,179,0,82,0,38,180,0,281,42,0,0,181,182,0,183,0,0,167,84,44,45,85,86,87,0,0,40,0,0,184,185,88,0,0,0,0,0,89,0,90,91,0,0,0,0,92,0,0,0,0,0,93,490,84,186,187,85,86,87,0,0,40,0,0,188,283,139,0,0,172,173,0,189,0,0,190,551,0,0,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,552,84,186,187,85,86,87,0,240,40,0,0,188,0,345,553,0,172,173,0,189,0,0,190,0,330,331,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,552,84,186,187,85,86,87,0,240,40,0,0,188,0,345,803,0,172,173,0,189,0,0,190,0,330,331,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,552,84,186,187,85,86,87,0,240,40,0,0,188,0,345,975,0,172,173,0,189,0,0,190,0,330,331,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,552,84,186,187,85,86,87,0,240,40,0,0,188,0,345,0,0,172,173,0,189,0,0,190,0,330,331,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,0,0,0,0,172,173,0,189,0,0,190,0,0,0,312,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,0,345,0,0,172,173,0,189,0,0,190,0,0,0,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,496,0,0,0,172,173,0,189,0,0,190,0,0,0,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,0,0,0,0,172,173,0,189,595,0,190,0,0,0,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,0,0,0,0,172,173,0,189,0,0,190,0,0,0,616,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,0,0,0,0,172,173,0,189,673,0,190,0,0,0,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,0,0,0,0,172,173,0,189,0,0,190,0,0,0,740,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,0,0,0,0,172,173,0,189,0,0,190,0,0,0,754,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,776,0,0,0,172,173,0,189,0,0,190,0,0,0,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,778,0,0,0,172,173,0,189,0,0,190,0,0,0,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,0,0,0,0,172,173,0,189,877,0,190,0,0,0,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,0,0,0,0,172,173,0,189,880,0,190,0,0,0,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,883,0,0,0,172,173,0,189,0,0,190,0,0,0,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,0,0,0,0,172,173,0,189,957,0,190,0,0,0,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,0,0,0,0,172,173,0,189,961,0,190,0,0,0,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,0,0,0,0,172,173,0,189,963,0,190,0,0,0,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,0,0,0,0,172,173,0,189,1023,0,190,0,0,0,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,0,0,0,0,172,173,0,189,0,0,190,0,0,0,0,191,0,81,192,174,0,193,194,175,0,195,196,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,181,182,0,183,0,0,0,0,0,172,173,0,0,0,0,0,0,0,184,185,0,0,81,0,0,0,0,0,0,0,0,0,0,0,176,177,178,179,0,0,0,744,84,186,187,85,86,87,181,182,40,183,0,188,0,0,0,172,173,0,0,189,0,0,190,0,184,185,0,191,81,0,192,0,0,193,194,0,0,195,196,0,176,177,178,179,0,0,0,37,84,186,187,85,86,87,181,182,40,183,0,188,0,345,0,172,173,0,0,189,0,0,190,0,184,185,0,191,81,0,192,0,0,193,194,0,0,195,196,0,176,177,178,179,0,0,0,37,84,186,187,85,86,87,181,182,40,183,0,188,0,718,0,172,173,0,0,189,0,0,190,0,184,185,0,191,81,0,192,0,0,193,194,0,0,195,196,0,176,177,178,179,0,0,0,37,84,186,187,85,86,87,181,182,40,183,0,188,0,0,0,172,173,0,0,340,0,0,190,0,184,185,0,191,81,0,192,0,0,193,194,0,0,195,196,0,176,177,178,179,0,0,0,37,84,186,187,85,86,87,181,182,40,183,0,188,0,0,0,172,173,0,0,356,0,0,190,0,184,185,0,191,81,0,192,0,0,193,194,0,0,195,196,0,176,177,178,179,0,0,0,37,84,186,187,85,86,87,181,182,40,183,0,188,0,0,0,0,0,0,0,189,0,0,190,0,184,185,0,191,0,0,192,0,0,193,194,0,0,195,196,0,0,0,0,0,0,0,0,37,84,186,187,85,86,87,0,0,40,0,0,188,0,0,0,0,0,0,0,926,0,0,190,0,0,0,0,191,0,0,192,0,0,193,194,0,0,195,196};
# 2711
static short Cyc_yycheck[6749]={0,109,158,145,114,113,191,192,193,374,375,368,166,0,175,286,287,260,289,101,102,21,68,105,370,175,436,212,145,373,129,600,404,455,21,5,6,247,781,21,0,174,145,43,376,377,378,379,262,380,381,51,382,383,763,371,107,85,58,59,60,21,62,516,51,91,331,17,68,69,228,58,59,60,73,62,58,59,60,520,62,68,105,129,160,299,68,85,359,794,65,51,514,121,103,141,124,451,58,59,60,523,62,103,104,121,849,345,68,135,126,521,59,68,118,820,103,104,118,828,120,121,122,123,91,134,124,481,500,129,129,118,107,120,121,122,123,123,136,139,123,141,129,103,104,529,431,129,148,718,120,307,139,65,141,105,609,139,118,141,120,121,122,123,118,112,451,321,135,129,85,43,119,105,129,23,105,189,105,139,105,141,925,136,139,57,141,140,931,114,323,123,86,87,123,107,123,129,331,384,385,386,907,908,105,123,121,310,913,124,88,476,130,114,226,261,86,87,118,123,230,136,330,105,125,125,71,128,228,123,124,105,616,315,82,112,118,247,448,111,114,228,125,112,126,93,126,129,689,992,127,125,233,123,124,236,136,471,127,137,138,261,105,119,121,475,424,425,426,125,142,114,126,978,261,132,119,982,273,261,436,104,136,49,279,126,286,287,105,289,662,732,125,401,524,136,113,405,1041,286,287,261,289,116,286,287,261,289,20,21,123,1054,125,556,81,82,83,317,131,125,105,169,104,304,0,131,286,287,340,289,342,286,287,118,289,247,121,419,123,320,105,106,354,476,356,223,22,23,113,25,121,125,736,121,266,126,740,439,125,359,238,129,600,130,131,521,57,43,121,217,754,121,359,126,796,51,224,359,119,129,821,57,567,824,614,230,651,259,105,65,121,67,833,503,70,126,113,114,115,359,118,119,247,79,359,676,82,506,81,82,83,126,105,106,121,130,288,93,123,629,113,266,129,653,424,425,426,103,104,511,119,107,133,433,89,111,646,424,425,426,141,142,118,827,120,121,122,123,448,121,450,106,125,837,126,119,122,130,131,459,506,461,705,463,130,131,142,705,909,121,118,119,459,914,461,475,463,129,620,105,718,121,88,89,332,23,126,113,114,115,166,633,492,169,120,171,761,118,107,459,121,461,502,463,516,492,506,502,118,739,492,118,122,743,121,502,633,620,651,506,502,121,752,599,506,119,126,539,121,685,633,409,492,126,189,121,448,492,125,451,126,217,502,619,82,532,506,502,224,121,711,506,84,85,126,93,632,787,968,121,105,711,805,475,126,795,414,222,136,481,116,247,140,938,939,932,819,123,681,125,121,819,118,259,429,131,121,690,934,722,266,126,268,937,702,271,121,609,76,77,682,126,873,448,482,483,714,715,940,941,942,943,288,944,945,122,946,947,618,935,936,121,121,278,125,890,126,126,761,105,475,169,678,171,126,116,636,113,114,115,640,121,123,624,125,321,126,126,118,125,131,130,172,640,174,175,332,51,121,121,931,121,905,126,184,185,126,905,118,854,665,191,192,193,133,105,676,125,678,217,640,125,141,142,114,340,224,872,118,676,118,678,212,121,676,125,678,352,121,118,115,356,121,133,134,135,5,6,695,103,104,120,118,125,369,121,676,629,678,118,122,676,121,678,118,125,120,121,122,123,118,146,125,121,646,845,125,649,414,969,105,417,118,125,725,121,38,625,113,114,115,406,118,125,602,121,737,890,118,90,125,121,125,948,949,950,131,125,105,750,1041,125,448,768,125,451,113,114,115,774,105,137,119,459,629,461,138,463,896,114,139,894,120,121,332,133,134,135,119,475,125,646,105,323,649,481,991,105,133,134,135,331,968,105,106,107,108,109,110,111,22,23,114,17,18,19,424,425,426,125,485,486,126,125,489,120,128,492,120,121,120,853,120,121,756,260,501,120,368,112,370,371,372,373,374,375,376,377,378,379,380,381,382,383,384,385,386,138,139,88,89,547,120,873,84,85,414,120,121,417,114,120,877,119,125,880,873,119,900,105,131,873,133,134,135,766,767,125,125,314,134,68,69,70,71,72,118,1040,75,76,77,145,105,873,926,118,928,125,873,125,113,114,115,771,129,125,602,122,923,1037,345,931,125,348,349,350,351,120,353,136,133,134,135,105,931,126,126,362,132,931,129,366,116,629,120,124,476,120,129,124,972,956,957,121,640,960,961,91,963,126,646,931,387,649,123,126,931,121,126,120,119,121,658,125,399,121,105,124,105,404,25,26,126,227,121,229,113,114,115,125,118,129,30,38,119,118,918,136,125,547,42,125,667,668,131,430,133,134,135,1031,105,73,1017,126,126,1020,132,1022,1023,114,264,116,126,121,105,126,129,121,121,121,125,1041,120,132,121,126,126,567,133,134,135,1046,126,126,1041,90,126,120,126,1041,126,38,126,119,602,459,126,461,125,463,105,106,105,120,109,110,111,125,119,114,756,1041,125,74,119,500,1041,118,503,126,125,118,127,128,120,771,105,119,133,124,123,136,25,758,139,140,126,126,143,144,126,132,126,90,121,126,126,132,126,118,775,776,125,778,119,780,126,129,105,106,120,651,109,110,111,118,118,114,125,112,126,556,119,121,125,106,126,802,125,28,127,128,126,120,126,126,133,126,120,119,125,129,139,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,90,137,24,600,90,120,25,121,120,711,121,118,120,429,106,37,853,39,118,125,722,120,126,45,621,126,121,49,126,626,126,54,448,55,56,57,120,634,126,455,0,63,879,126,66,882,883,126,126,0,918,771,34,113,628,70,78,79,80,761,918,47,649,968,640,662,38,695,159,92,93,94,95,96,97,98,99,100,101,259,417,264,690,685,333,224,547,503,- 1,926,112,113,372,115,116,771,933,119,514,122,122,355,- 1,1037,- 1,127,128,523,705,- 1,38,708,- 1,710,951,- 1,- 1,- 1,90,- 1,- 1,718,- 1,- 1,721,- 1,- 1,- 1,965,- 1,102,- 1,- 1,105,106,972,- 1,109,110,111,- 1,113,114,- 1,- 1,- 1,- 1,119,- 1,851,- 1,- 1,854,125,- 1,127,128,- 1,130,131,- 1,133,90,- 1,- 1,- 1,- 1,139,- 1,- 1,872,- 1,- 1,- 1,102,- 1,- 1,105,106,- 1,- 1,109,110,111,- 1,113,114,- 1,890,- 1,- 1,119,- 1,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,130,131,620,133,- 1,- 1,805,- 1,- 1,139,- 1,629,- 1,- 1,- 1,633,- 1,- 1,- 1,- 1,819,- 1,- 1,- 1,- 1,- 1,- 1,- 1,932,- 1,934,935,936,937,938,939,940,941,942,943,944,945,946,947,948,949,950,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,991,44,45,46,- 1,- 1,49,50,51,52,53,- 1,55,56,- 1,58,- 1,60,105,905,63,64,65,66,67,- 1,113,114,115,- 1,- 1,38,- 1,- 1,- 1,78,79,80,125,82,83,- 1,129,- 1,- 1,- 1,133,134,135,1040,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,763,- 1,- 1,- 1,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,125,90,127,128,- 1,- 1,- 1,- 1,133,796,- 1,136,- 1,102,139,140,105,106,143,144,109,110,111,105,- 1,114,- 1,- 1,- 1,- 1,119,113,114,115,116,- 1,125,- 1,127,128,- 1,828,- 1,125,133,- 1,- 1,- 1,- 1,- 1,139,133,134,135,- 1,- 1,- 1,- 1,845,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,50,51,52,53,- 1,55,56,- 1,58,- 1,60,- 1,- 1,63,64,65,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,- 1,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,50,51,52,53,- 1,55,56,57,58,- 1,60,- 1,- 1,63,64,65,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,- 1,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,- 1,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,50,51,52,53,- 1,55,56,- 1,58,- 1,60,- 1,- 1,63,64,65,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,- 1,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,- 1,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,50,51,52,53,- 1,55,56,- 1,58,- 1,60,- 1,- 1,63,64,65,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,- 1,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,- 1,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,- 1,- 1,44,45,- 1,- 1,- 1,49,50,51,52,53,- 1,55,56,- 1,58,- 1,- 1,- 1,- 1,63,64,65,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,112,113,114,115,116,117,118,- 1,- 1,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,- 1,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,50,51,52,53,- 1,55,56,- 1,58,- 1,- 1,- 1,- 1,63,64,65,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,112,113,114,115,116,117,- 1,119,- 1,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,- 1,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,50,51,52,53,- 1,55,56,- 1,58,- 1,- 1,- 1,- 1,63,64,65,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,112,113,114,115,116,117,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,- 1,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,43,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,61,62,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,- 1,- 1,- 1,120,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,134,135,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,61,62,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,- 1,- 1,- 1,120,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,134,135,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,61,62,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,134,135,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,55,56,57,- 1,- 1,- 1,61,62,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,- 1,- 1,- 1,- 1,- 1,37,112,113,114,115,116,- 1,- 1,- 1,120,- 1,- 1,49,- 1,125,- 1,127,128,55,56,- 1,- 1,133,134,135,- 1,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,102,- 1,- 1,105,- 1,- 1,- 1,- 1,- 1,37,112,113,114,115,116,- 1,118,- 1,- 1,- 1,- 1,49,- 1,125,- 1,127,128,55,56,131,- 1,133,134,135,- 1,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,78,79,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,102,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,- 1,115,116,- 1,118,- 1,- 1,- 1,- 1,49,- 1,125,90,127,128,55,56,131,- 1,133,134,135,- 1,63,102,- 1,66,105,106,- 1,- 1,109,110,111,- 1,- 1,114,- 1,78,79,- 1,119,- 1,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,- 1,- 1,- 1,- 1,- 1,139,- 1,- 1,- 1,105,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,134,135,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,- 1,115,116,37,- 1,119,- 1,- 1,- 1,- 1,- 1,45,- 1,127,128,49,- 1,- 1,- 1,- 1,- 1,55,56,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,- 1,115,116,- 1,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,127,128,55,56,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,102,- 1,- 1,105,- 1,- 1,- 1,- 1,- 1,37,112,113,- 1,115,116,- 1,118,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,127,128,55,56,- 1,- 1,59,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,- 1,115,116,- 1,- 1,119,- 1,- 1,- 1,49,- 1,- 1,- 1,127,128,55,56,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,102,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,- 1,115,116,- 1,118,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,127,128,55,56,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,- 1,115,116,- 1,- 1,- 1,120,- 1,- 1,49,- 1,- 1,- 1,127,128,55,56,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,- 1,115,116,- 1,- 1,- 1,120,- 1,- 1,49,- 1,- 1,- 1,127,128,55,56,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,102,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,- 1,115,116,- 1,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,127,128,55,56,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,- 1,115,116,- 1,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,127,128,55,56,- 1,38,- 1,- 1,- 1,- 1,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,38,- 1,40,41,- 1,- 1,44,- 1,46,- 1,- 1,- 1,50,51,52,53,- 1,90,- 1,112,58,- 1,60,116,- 1,- 1,64,65,- 1,67,- 1,- 1,105,106,127,128,109,110,111,- 1,- 1,114,- 1,- 1,82,83,119,- 1,- 1,- 1,- 1,- 1,125,- 1,127,128,- 1,- 1,- 1,- 1,133,- 1,- 1,- 1,- 1,- 1,139,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,118,119,- 1,- 1,25,26,- 1,125,- 1,- 1,128,32,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,113,114,- 1,- 1,117,- 1,119,120,- 1,25,26,- 1,125,- 1,- 1,128,- 1,130,131,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,113,114,- 1,- 1,117,- 1,119,120,- 1,25,26,- 1,125,- 1,- 1,128,- 1,130,131,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,113,114,- 1,- 1,117,- 1,119,120,- 1,25,26,- 1,125,- 1,- 1,128,- 1,130,131,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,113,114,- 1,- 1,117,- 1,119,- 1,- 1,25,26,- 1,125,- 1,- 1,128,- 1,130,131,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,- 1,- 1,128,- 1,- 1,- 1,132,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,119,- 1,- 1,25,26,- 1,125,- 1,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,118,- 1,- 1,- 1,25,26,- 1,125,- 1,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,126,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,- 1,- 1,128,- 1,- 1,- 1,132,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,126,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,- 1,- 1,128,- 1,- 1,- 1,132,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,- 1,- 1,128,- 1,- 1,- 1,132,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,118,- 1,- 1,- 1,25,26,- 1,125,- 1,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,118,- 1,- 1,- 1,25,26,- 1,125,- 1,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,126,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,126,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,118,- 1,- 1,- 1,25,26,- 1,125,- 1,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,126,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,126,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,126,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,126,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,25,26,- 1,125,- 1,- 1,128,- 1,- 1,- 1,- 1,133,- 1,38,136,40,- 1,139,140,44,- 1,143,144,- 1,- 1,50,51,52,53,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,- 1,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,51,52,53,- 1,- 1,- 1,105,106,107,108,109,110,111,64,65,114,67,- 1,117,- 1,- 1,- 1,25,26,- 1,- 1,125,- 1,- 1,128,- 1,82,83,- 1,133,38,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,- 1,50,51,52,53,- 1,- 1,- 1,105,106,107,108,109,110,111,64,65,114,67,- 1,117,- 1,119,- 1,25,26,- 1,- 1,125,- 1,- 1,128,- 1,82,83,- 1,133,38,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,- 1,50,51,52,53,- 1,- 1,- 1,105,106,107,108,109,110,111,64,65,114,67,- 1,117,- 1,119,- 1,25,26,- 1,- 1,125,- 1,- 1,128,- 1,82,83,- 1,133,38,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,- 1,50,51,52,53,- 1,- 1,- 1,105,106,107,108,109,110,111,64,65,114,67,- 1,117,- 1,- 1,- 1,25,26,- 1,- 1,125,- 1,- 1,128,- 1,82,83,- 1,133,38,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,- 1,50,51,52,53,- 1,- 1,- 1,105,106,107,108,109,110,111,64,65,114,67,- 1,117,- 1,- 1,- 1,25,26,- 1,- 1,125,- 1,- 1,128,- 1,82,83,- 1,133,38,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,- 1,50,51,52,53,- 1,- 1,- 1,105,106,107,108,109,110,111,64,65,114,67,- 1,117,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,- 1,- 1,128,- 1,82,83,- 1,133,- 1,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,106,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,- 1,- 1,128,- 1,- 1,- 1,- 1,133,- 1,- 1,136,- 1,- 1,139,140,- 1,- 1,143,144};char Cyc_Yystack_overflow[17]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
void Cyc_yyerror(struct _dyneither_ptr,int state,int token);
# 82 "cycbison.simple"
int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*yylval_ptr,struct Cyc_Yyltype*yylloc);struct _tuple27{unsigned int f1;struct _tuple0*f2;int f3;};struct _tuple28{struct _dyneither_ptr f1;void*f2;};static char _tmp519[8]="stdcall";static char _tmp51A[6]="cdecl";static char _tmp51B[9]="fastcall";static char _tmp51C[9]="noreturn";static char _tmp51D[6]="const";static char _tmp51E[8]="aligned";static char _tmp51F[7]="packed";static char _tmp520[7]="shared";static char _tmp521[7]="unused";static char _tmp522[5]="weak";static char _tmp523[10]="dllimport";static char _tmp524[10]="dllexport";static char _tmp525[23]="no_instrument_function";static char _tmp526[12]="constructor";static char _tmp527[11]="destructor";static char _tmp528[22]="no_check_memory_usage";static char _tmp529[5]="pure";struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf);static void _tmpA0F(union Cyc_YYSTYPE*yylval,unsigned int*_tmpA0E,unsigned int*_tmpA0D,union Cyc_YYSTYPE**_tmpA0C){for(*_tmpA0E=0;*_tmpA0E < *_tmpA0D;(*_tmpA0E)++){(*_tmpA0C)[*_tmpA0E]=(union Cyc_YYSTYPE)*yylval;}}static void _tmpA14(unsigned int*_tmpA13,unsigned int*_tmpA12,struct Cyc_Yyltype**_tmpA11){for(*_tmpA13=0;*_tmpA13 < *_tmpA12;(*_tmpA13)++){(*_tmpA11)[*_tmpA13]=(struct Cyc_Yyltype)
# 164 "cycbison.simple"
Cyc_yynewloc();}}static void _tmpA1B(int*yyssp_offset,struct _dyneither_ptr*yyss,unsigned int*_tmpA1A,unsigned int*_tmpA19,short**_tmpA17){for(*_tmpA1A=0;*_tmpA1A < *_tmpA19;(*_tmpA1A)++){(*_tmpA17)[*_tmpA1A]=(short)(
# 216
*_tmpA1A <= *yyssp_offset?*((short*)_check_dyneither_subscript(*yyss,sizeof(short),(int)*_tmpA1A)): 0);}}static void _tmpA21(struct _dyneither_ptr*yyvs,int*yyssp_offset,union Cyc_YYSTYPE*yylval,unsigned int*_tmpA20,unsigned int*_tmpA1F,union Cyc_YYSTYPE**_tmpA1D){for(*_tmpA20=0;*_tmpA20 < *_tmpA1F;(*_tmpA20)++){(*_tmpA1D)[*_tmpA20]=(union Cyc_YYSTYPE)(
# 219
*_tmpA20 <= *yyssp_offset?*((union Cyc_YYSTYPE*)_check_dyneither_subscript(*yyvs,sizeof(union Cyc_YYSTYPE),(int)*_tmpA20)):*yylval);}}static void _tmpA27(int*yyssp_offset,struct _dyneither_ptr*yyls,unsigned int*_tmpA26,unsigned int*_tmpA25,struct Cyc_Yyltype**_tmpA23){for(*_tmpA26=0;*_tmpA26 < *_tmpA25;(*_tmpA26)++){(*_tmpA23)[*_tmpA26]=(struct Cyc_Yyltype)(
# 222
*_tmpA26 <= *yyssp_offset?*((struct Cyc_Yyltype*)_check_dyneither_subscript(*yyls,sizeof(struct Cyc_Yyltype),(int)*_tmpA26)):
 Cyc_yynewloc());}}static void _tmpF32(unsigned int*_tmpF31,unsigned int*_tmpF30,char**_tmpF2E){for(*_tmpF31=0;*_tmpF31 < *_tmpF30;(*_tmpF31)++){(*_tmpF2E)[*_tmpF31]=(char)'\000';}}
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
union Cyc_YYSTYPE _tmpA08;union Cyc_YYSTYPE yylval=((_tmpA08.YYINITIALSVAL).val=0,(((_tmpA08.YYINITIALSVAL).tag=64,_tmpA08)));
int yynerrs;
# 150
struct Cyc_Yyltype yylloc;
# 154
int yyssp_offset;
# 156
short*_tmpA0A;unsigned int _tmpA09;struct _dyneither_ptr yyss=(_tmpA09=200,((_tmpA0A=_region_calloc(yyregion,sizeof(short),_tmpA09),_tag_dyneither(_tmpA0A,sizeof(short),_tmpA09))));
# 158
int yyvsp_offset;
unsigned int _tmpA0E;unsigned int _tmpA0D;union Cyc_YYSTYPE*_tmpA0C;unsigned int _tmpA0B;struct _dyneither_ptr yyvs=_tag_dyneither(((_tmpA0B=(unsigned int)200,((_tmpA0C=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmpA0B)),((((_tmpA0D=_tmpA0B,_tmpA0F(& yylval,& _tmpA0E,& _tmpA0D,& _tmpA0C))),_tmpA0C)))))),sizeof(union Cyc_YYSTYPE),(unsigned int)200);
# 163
int yylsp_offset;
unsigned int _tmpA13;unsigned int _tmpA12;struct Cyc_Yyltype*_tmpA11;unsigned int _tmpA10;struct _dyneither_ptr yyls=_tag_dyneither(((_tmpA10=(unsigned int)200,((_tmpA11=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmpA10)),((((_tmpA12=_tmpA10,_tmpA14(& _tmpA13,& _tmpA12,& _tmpA11))),_tmpA11)))))),sizeof(struct Cyc_Yyltype),(unsigned int)200);
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
{const char*_tmpA15;Cyc_yyerror(((_tmpA15="parser stack overflow",_tag_dyneither(_tmpA15,sizeof(char),22))),yystate,yychar);}
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 212
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
unsigned int _tmpA1A;unsigned int _tmpA19;struct _dyneither_ptr _tmpA18;short*_tmpA17;unsigned int _tmpA16;struct _dyneither_ptr yyss1=(_tmpA16=(unsigned int)yystacksize,((_tmpA17=(short*)_region_malloc(yyregion,_check_times(sizeof(short),_tmpA16)),((_tmpA18=_tag_dyneither(_tmpA17,sizeof(short),_tmpA16),((((_tmpA19=_tmpA16,_tmpA1B(& yyssp_offset,& yyss,& _tmpA1A,& _tmpA19,& _tmpA17))),_tmpA18)))))));
# 217
unsigned int _tmpA20;unsigned int _tmpA1F;struct _dyneither_ptr _tmpA1E;union Cyc_YYSTYPE*_tmpA1D;unsigned int _tmpA1C;struct _dyneither_ptr yyvs1=
(_tmpA1C=(unsigned int)yystacksize,((_tmpA1D=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmpA1C)),((_tmpA1E=_tag_dyneither(_tmpA1D,sizeof(union Cyc_YYSTYPE),_tmpA1C),((((_tmpA1F=_tmpA1C,_tmpA21(& yyvs,& yyssp_offset,& yylval,& _tmpA20,& _tmpA1F,& _tmpA1D))),_tmpA1E)))))));
# 221
unsigned int _tmpA26;unsigned int _tmpA25;struct _dyneither_ptr _tmpA24;struct Cyc_Yyltype*_tmpA23;unsigned int _tmpA22;struct _dyneither_ptr yyls1=(_tmpA22=(unsigned int)yystacksize,((_tmpA23=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmpA22)),((_tmpA24=_tag_dyneither(_tmpA23,sizeof(struct Cyc_Yyltype),_tmpA22),((((_tmpA25=_tmpA22,_tmpA27(& yyssp_offset,& yyls,& _tmpA26,& _tmpA25,& _tmpA23))),_tmpA24)))))));
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
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1064,yystate)];
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
if((yyn < 0  || yyn > 6748) || Cyc_yycheck[_check_known_subscript_notnull(6749,yyn)]!= yychar1)goto yydefault;
# 300
yyn=(int)Cyc_yytable[_check_known_subscript_notnull(6749,yyn)];
# 307
if(yyn < 0){
# 309
if(yyn == - 32768)goto yyerrlab;
yyn=- yyn;
goto yyreduce;}else{
# 313
if(yyn == 0)goto yyerrlab;}
# 315
if(yyn == 1063){
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
 yyn=(int)Cyc_yydefact[_check_known_subscript_notnull(1064,yystate)];
if(yyn == 0)goto yyerrlab;
# 349
yyreduce:
# 351
 yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(526,yyn)];
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
{struct Cyc_Absyn_Decl*_tmpA31;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmpA30;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmpA2F;struct Cyc_List_List*_tmpA2E;yyval=Cyc_YY16(((_tmpA2E=_cycalloc(sizeof(*_tmpA2E)),((_tmpA2E->hd=((_tmpA31=_cycalloc(sizeof(*_tmpA31)),((_tmpA31->r=(void*)((_tmpA2F=_cycalloc(sizeof(*_tmpA2F)),((_tmpA2F[0]=((_tmpA30.tag=10,((_tmpA30.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA30.f2=Cyc_yyget_YY16(yyyyvsp[2]),_tmpA30)))))),_tmpA2F)))),((_tmpA31->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA31)))))),((_tmpA2E->tl=0,_tmpA2E)))))));}
Cyc_Lex_leave_using();
# 1246
break;}case 4: _LL22F: {
# 1248
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1250
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1247 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA3B;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmpA3A;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmpA39;struct Cyc_List_List*_tmpA38;yyval=Cyc_YY16(((_tmpA38=_cycalloc(sizeof(*_tmpA38)),((_tmpA38->hd=((_tmpA3B=_cycalloc(sizeof(*_tmpA3B)),((_tmpA3B->r=(void*)((_tmpA39=_cycalloc(sizeof(*_tmpA39)),((_tmpA39[0]=((_tmpA3A.tag=10,((_tmpA3A.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA3A.f2=Cyc_yyget_YY16(yyyyvsp[2]),_tmpA3A)))))),_tmpA39)))),((_tmpA3B->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA3B)))))),((_tmpA38->tl=Cyc_yyget_YY16(yyyyvsp[4]),_tmpA38)))))));}
break;}case 5: _LL230: {
# 1250
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1252
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1250 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA4A;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmpA49;struct _dyneither_ptr*_tmpA48;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpA47;struct Cyc_List_List*_tmpA46;yyval=Cyc_YY16(((_tmpA46=_cycalloc(sizeof(*_tmpA46)),((_tmpA46->hd=((_tmpA4A=_cycalloc(sizeof(*_tmpA4A)),((_tmpA4A->r=(void*)((_tmpA47=_cycalloc(sizeof(*_tmpA47)),((_tmpA47[0]=((_tmpA49.tag=9,((_tmpA49.f1=((_tmpA48=_cycalloc(sizeof(*_tmpA48)),((_tmpA48[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpA48)))),((_tmpA49.f2=Cyc_yyget_YY16(yyyyvsp[2]),_tmpA49)))))),_tmpA47)))),((_tmpA4A->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA4A)))))),((_tmpA46->tl=0,_tmpA46)))))));}
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
{const char*_tmpA4B;nspace=((_tmpA4B="",_tag_dyneither(_tmpA4B,sizeof(char),1)));}goto _LL233;_LL236: _tmp48C=_tmp48A;_LL237:(void)_rethrow(_tmp48C);_LL233:;}};}
# 1260
{struct _handler_cons _tmp48E;_push_handler(& _tmp48E);{int _tmp490=0;if(setjmp(_tmp48E.handler))_tmp490=1;if(!_tmp490){x=Cyc_yyget_YY16(yyyyvsp[2]);;_pop_handler();}else{void*_tmp48F=(void*)_exn_thrown;void*_tmp492=_tmp48F;void*_tmp494;_LL239: {struct Cyc_Core_Failure_exn_struct*_tmp493=(struct Cyc_Core_Failure_exn_struct*)_tmp492;if(_tmp493->tag != Cyc_Core_Failure)goto _LL23B;}_LL23A:
 x=0;goto _LL238;_LL23B: _tmp494=_tmp492;_LL23C:(void)_rethrow(_tmp494);_LL238:;}};}
# 1263
{struct _handler_cons _tmp495;_push_handler(& _tmp495);{int _tmp497=0;if(setjmp(_tmp495.handler))_tmp497=1;if(!_tmp497){y=Cyc_yyget_YY16(yyyyvsp[4]);;_pop_handler();}else{void*_tmp496=(void*)_exn_thrown;void*_tmp499=_tmp496;void*_tmp49B;_LL23E: {struct Cyc_Core_Failure_exn_struct*_tmp49A=(struct Cyc_Core_Failure_exn_struct*)_tmp499;if(_tmp49A->tag != Cyc_Core_Failure)goto _LL240;}_LL23F:
 y=0;goto _LL23D;_LL240: _tmp49B=_tmp499;_LL241:(void)_rethrow(_tmp49B);_LL23D:;}};}
# 1266
{struct Cyc_Absyn_Decl*_tmpA5A;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmpA59;struct _dyneither_ptr*_tmpA58;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpA57;struct Cyc_List_List*_tmpA56;yyval=Cyc_YY16(((_tmpA56=_cycalloc(sizeof(*_tmpA56)),((_tmpA56->hd=((_tmpA5A=_cycalloc(sizeof(*_tmpA5A)),((_tmpA5A->r=(void*)((_tmpA57=_cycalloc(sizeof(*_tmpA57)),((_tmpA57[0]=((_tmpA59.tag=9,((_tmpA59.f1=((_tmpA58=_cycalloc(sizeof(*_tmpA58)),((_tmpA58[0]=nspace,_tmpA58)))),((_tmpA59.f2=x,_tmpA59)))))),_tmpA57)))),((_tmpA5A->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA5A)))))),((_tmpA56->tl=y,_tmpA56)))))));}
# 1268
break;}case 7: _LL232: {
# 1270
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1272
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1269 "parse.y"
int _tmp4A1=Cyc_yyget_YY31(yyyyvsp[0]);
if(!_tmp4A1){
struct Cyc_Absyn_Decl*_tmpA64;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct _tmpA63;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmpA62;struct Cyc_List_List*_tmpA61;yyval=Cyc_YY16(((_tmpA61=_cycalloc(sizeof(*_tmpA61)),((_tmpA61->hd=((_tmpA64=_cycalloc(sizeof(*_tmpA64)),((_tmpA64->r=(void*)((_tmpA62=_cycalloc(sizeof(*_tmpA62)),((_tmpA62[0]=((_tmpA63.tag=11,((_tmpA63.f1=Cyc_yyget_YY16(yyyyvsp[2]),_tmpA63)))),_tmpA62)))),((_tmpA64->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA64)))))),((_tmpA61->tl=Cyc_yyget_YY16(yyyyvsp[4]),_tmpA61)))))));}else{
# 1273
struct Cyc_Absyn_Decl*_tmpA6E;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmpA6D;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA6C;struct Cyc_List_List*_tmpA6B;yyval=Cyc_YY16(((_tmpA6B=_cycalloc(sizeof(*_tmpA6B)),((_tmpA6B->hd=((_tmpA6E=_cycalloc(sizeof(*_tmpA6E)),((_tmpA6E->r=(void*)((_tmpA6C=_cycalloc(sizeof(*_tmpA6C)),((_tmpA6C[0]=((_tmpA6D.tag=12,((_tmpA6D.f1=Cyc_yyget_YY16(yyyyvsp[2]),((_tmpA6D.f2=0,_tmpA6D)))))),_tmpA6C)))),((_tmpA6E->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA6E)))))),((_tmpA6B->tl=Cyc_yyget_YY16(yyyyvsp[4]),_tmpA6B)))))));}
# 1275
break;}case 8: _LL242: {
# 1277
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1279
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1276 "parse.y"
if(!Cyc_yyget_YY31(yyyyvsp[0])){
const char*_tmpA6F;Cyc_Parse_err(((_tmpA6F="expecting \"C include\"",_tag_dyneither(_tmpA6F,sizeof(char),22))),(unsigned int)(yyyylsp[0]).first_line);}{
struct Cyc_List_List*exs=Cyc_yyget_YY52(yyyyvsp[4]);
{struct Cyc_Absyn_Decl*_tmpA79;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmpA78;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA77;struct Cyc_List_List*_tmpA76;yyval=Cyc_YY16(((_tmpA76=_cycalloc(sizeof(*_tmpA76)),((_tmpA76->hd=((_tmpA79=_cycalloc(sizeof(*_tmpA79)),((_tmpA79->r=(void*)((_tmpA77=_cycalloc(sizeof(*_tmpA77)),((_tmpA77[0]=((_tmpA78.tag=12,((_tmpA78.f1=Cyc_yyget_YY16(yyyyvsp[2]),((_tmpA78.f2=exs,_tmpA78)))))),_tmpA77)))),((_tmpA79->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA79)))))),((_tmpA76->tl=Cyc_yyget_YY16(yyyyvsp[5]),_tmpA76)))))));}
# 1281
break;};}case 9: _LL243: {
# 1283
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1285
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1282 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA7C;struct Cyc_List_List*_tmpA7B;yyval=Cyc_YY16(((_tmpA7B=_cycalloc(sizeof(*_tmpA7B)),((_tmpA7B->hd=((_tmpA7C=_cycalloc(sizeof(*_tmpA7C)),((_tmpA7C->r=(void*)& Cyc_Absyn_Porton_d_val,((_tmpA7C->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA7C)))))),((_tmpA7B->tl=Cyc_yyget_YY16(yyyyvsp[2]),_tmpA7B)))))));}
break;}case 10: _LL244: {
# 1285
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1287
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1284 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA7F;struct Cyc_List_List*_tmpA7E;yyval=Cyc_YY16(((_tmpA7E=_cycalloc(sizeof(*_tmpA7E)),((_tmpA7E->hd=((_tmpA7F=_cycalloc(sizeof(*_tmpA7F)),((_tmpA7F->r=(void*)& Cyc_Absyn_Portoff_d_val,((_tmpA7F->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA7F)))))),((_tmpA7E->tl=Cyc_yyget_YY16(yyyyvsp[2]),_tmpA7E)))))));}
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
{const char*_tmpA80;if(Cyc_strcmp((struct _dyneither_ptr)_tmp4B3,((_tmpA80="C",_tag_dyneither(_tmpA80,sizeof(char),2))))== 0)
yyval=Cyc_YY31(0);else{
const char*_tmpA81;if(Cyc_strcmp((struct _dyneither_ptr)_tmp4B3,((_tmpA81="C include",_tag_dyneither(_tmpA81,sizeof(char),10))))== 0)
yyval=Cyc_YY31(1);else{
# 1297
{const char*_tmpA82;Cyc_Parse_err(((_tmpA82="expecting \"C\" or \"C include\"",_tag_dyneither(_tmpA82,sizeof(char),29))),(unsigned int)(yyyylsp[0]).first_line);}
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
{struct _tuple27*_tmpA85;struct Cyc_List_List*_tmpA84;yyval=Cyc_YY52(((_tmpA84=_cycalloc(sizeof(*_tmpA84)),((_tmpA84->hd=((_tmpA85=_cycalloc(sizeof(*_tmpA85)),((_tmpA85->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA85->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA85->f3=0,_tmpA85)))))))),((_tmpA84->tl=0,_tmpA84)))))));}
break;}case 17: _LL24B: {
# 1316
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1318
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1314 "parse.y"
{struct _tuple27*_tmpA88;struct Cyc_List_List*_tmpA87;yyval=Cyc_YY52(((_tmpA87=_cycalloc(sizeof(*_tmpA87)),((_tmpA87->hd=((_tmpA88=_cycalloc(sizeof(*_tmpA88)),((_tmpA88->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA88->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA88->f3=0,_tmpA88)))))))),((_tmpA87->tl=0,_tmpA87)))))));}
break;}case 18: _LL24C: {
# 1317
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1319
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1316 "parse.y"
{struct _tuple27*_tmpA8B;struct Cyc_List_List*_tmpA8A;yyval=Cyc_YY52(((_tmpA8A=_cycalloc(sizeof(*_tmpA8A)),((_tmpA8A->hd=((_tmpA8B=_cycalloc(sizeof(*_tmpA8B)),((_tmpA8B->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA8B->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA8B->f3=0,_tmpA8B)))))))),((_tmpA8A->tl=Cyc_yyget_YY52(yyyyvsp[2]),_tmpA8A)))))));}
break;}case 19: _LL24D: {
# 1319
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1321
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1320 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpA91;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpA90;struct Cyc_List_List*_tmpA8F;yyval=Cyc_YY16(((_tmpA8F=_cycalloc(sizeof(*_tmpA8F)),((_tmpA8F->hd=Cyc_Absyn_new_decl((void*)((_tmpA91=_cycalloc(sizeof(*_tmpA91)),((_tmpA91[0]=((_tmpA90.tag=1,((_tmpA90.f1=Cyc_yyget_YY15(yyyyvsp[0]),_tmpA90)))),_tmpA91)))),(unsigned int)(yyyylsp[0]).first_line),((_tmpA8F->tl=0,_tmpA8F)))))));}
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
{struct _dyneither_ptr*_tmpA92;Cyc_Lex_enter_namespace(((_tmpA92=_cycalloc(sizeof(*_tmpA92)),((_tmpA92[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpA92)))));}yyval=yyyyvsp[1];
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
{struct Cyc_List_List*_tmpA93;yyval=Cyc_YY16(((_tmpA93=_cycalloc(sizeof(*_tmpA93)),((_tmpA93->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY9(yyyyvsp[1]),Cyc_yyget_Exp_tok(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line),((_tmpA93->tl=0,_tmpA93)))))));}
break;}case 37: _LL25D: {
# 1389
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1391
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1388 "parse.y"
struct Cyc_List_List*_tmp4C8=0;
{struct Cyc_List_List*_tmp4C9=Cyc_yyget_YY36(yyyyvsp[1]);for(0;_tmp4C9 != 0;_tmp4C9=_tmp4C9->tl){
struct _dyneither_ptr*_tmp4CA=(struct _dyneither_ptr*)_tmp4C9->hd;
struct _tuple0*_tmpA94;struct _tuple0*qv=(_tmpA94=_cycalloc(sizeof(*_tmpA94)),((_tmpA94->f1=Cyc_Absyn_Rel_n(0),((_tmpA94->f2=_tmp4CA,_tmpA94)))));
struct Cyc_Absyn_Vardecl*_tmp4CB=Cyc_Absyn_new_vardecl(qv,Cyc_Absyn_wildtyp(0),0);
struct Cyc_List_List*_tmpA95;_tmp4C8=((_tmpA95=_cycalloc(sizeof(*_tmpA95)),((_tmpA95->hd=_tmp4CB,((_tmpA95->tl=_tmp4C8,_tmpA95))))));}}
# 1395
_tmp4C8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4C8);
{struct Cyc_List_List*_tmpA96;yyval=Cyc_YY16(((_tmpA96=_cycalloc(sizeof(*_tmpA96)),((_tmpA96->hd=Cyc_Absyn_letv_decl(_tmp4C8,(unsigned int)(yyyylsp[0]).first_line),((_tmpA96->tl=0,_tmpA96)))))));}
# 1398
break;}case 38: _LL25E: {
# 1400
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 1402
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 1401 "parse.y"
struct _dyneither_ptr _tmp4CF=Cyc_yyget_String_tok(yyyyvsp[3]);
# 1403
{const char*_tmpA97;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4CF,((_tmpA97="`H",_tag_dyneither(_tmpA97,sizeof(char),3))))== 0){
const char*_tmpA9A;void*_tmpA99;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA99=0,Cyc_aprintf(((_tmpA9A="bad occurrence of heap region",_tag_dyneither(_tmpA9A,sizeof(char),30))),_tag_dyneither(_tmpA99,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}
{const char*_tmpA9B;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4CF,((_tmpA9B="`U",_tag_dyneither(_tmpA9B,sizeof(char),3))))== 0){
const char*_tmpA9E;void*_tmpA9D;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA9D=0,Cyc_aprintf(((_tmpA9E="bad occurrence of unique region",_tag_dyneither(_tmpA9E,sizeof(char),32))),_tag_dyneither(_tmpA9D,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}{
struct _dyneither_ptr*_tmpAA1;struct Cyc_Absyn_Tvar*_tmpAA0;struct Cyc_Absyn_Tvar*tv=(_tmpAA0=_cycalloc(sizeof(*_tmpAA0)),((_tmpAA0->name=((_tmpAA1=_cycalloc(sizeof(*_tmpAA1)),((_tmpAA1[0]=_tmp4CF,_tmpAA1)))),((_tmpAA0->identity=- 1,((_tmpAA0->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpAA0)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpAA4;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpAA3;void*t=(void*)((_tmpAA3=_cycalloc(sizeof(*_tmpAA3)),((_tmpAA3[0]=((_tmpAA4.tag=2,((_tmpAA4.f1=tv,_tmpAA4)))),_tmpAA3))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpAAA;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpAA9;struct _dyneither_ptr*_tmpAA7;struct _tuple0*_tmpAA6;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpAA6=_cycalloc(sizeof(*_tmpAA6)),((_tmpAA6->f1=Cyc_Absyn_Loc_n,((_tmpAA6->f2=((_tmpAA7=_cycalloc(sizeof(*_tmpAA7)),((_tmpAA7[0]=Cyc_yyget_String_tok(yyyyvsp[5]),_tmpAA7)))),_tmpAA6)))))),(void*)((_tmpAA9=_cycalloc(sizeof(*_tmpAA9)),((_tmpAA9[0]=((_tmpAAA.tag=15,((_tmpAAA.f1=t,_tmpAAA)))),_tmpAA9)))),0);
{struct Cyc_List_List*_tmpAAB;yyval=Cyc_YY16(((_tmpAAB=_cycalloc(sizeof(*_tmpAAB)),((_tmpAAB->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY31(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line),((_tmpAAB->tl=0,_tmpAAB)))))));}
# 1412
break;};}case 39: _LL25F: {
# 1414
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1416
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1414 "parse.y"
int _tmp4DF=Cyc_yyget_YY31(yyyyvsp[0]);
struct _dyneither_ptr _tmp4E0=Cyc_yyget_String_tok(yyyyvsp[2]);
{const char*_tmpAAC;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4E0,((_tmpAAC="H",_tag_dyneither(_tmpAAC,sizeof(char),2))))== 0){
const char*_tmpAAD;Cyc_Parse_err(((_tmpAAD="bad occurrence of heap region `H",_tag_dyneither(_tmpAAD,sizeof(char),33))),(unsigned int)(yyyylsp[2]).first_line);}}
{const char*_tmpAAE;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4E0,((_tmpAAE="U",_tag_dyneither(_tmpAAE,sizeof(char),2))))== 0){
const char*_tmpAAF;Cyc_Parse_err(((_tmpAAF="bad occurrence of unique region `U",_tag_dyneither(_tmpAAF,sizeof(char),35))),(unsigned int)(yyyylsp[2]).first_line);}}{
struct _dyneither_ptr*_tmpABC;const char*_tmpABB;void*_tmpABA[1];struct Cyc_String_pa_PrintArg_struct _tmpAB9;struct Cyc_Absyn_Tvar*_tmpAB8;struct Cyc_Absyn_Tvar*tv=(_tmpAB8=_cycalloc(sizeof(*_tmpAB8)),((_tmpAB8->name=((_tmpABC=_cycalloc(sizeof(*_tmpABC)),((_tmpABC[0]=(struct _dyneither_ptr)((_tmpAB9.tag=0,((_tmpAB9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4E0),((_tmpABA[0]=& _tmpAB9,Cyc_aprintf(((_tmpABB="`%s",_tag_dyneither(_tmpABB,sizeof(char),4))),_tag_dyneither(_tmpABA,sizeof(void*),1)))))))),_tmpABC)))),((_tmpAB8->identity=- 1,((_tmpAB8->kind=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpAB8)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpABF;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpABE;void*t=(void*)((_tmpABE=_cycalloc(sizeof(*_tmpABE)),((_tmpABE[0]=((_tmpABF.tag=2,((_tmpABF.f1=tv,_tmpABF)))),_tmpABE))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpAC5;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpAC4;struct _dyneither_ptr*_tmpAC2;struct _tuple0*_tmpAC1;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpAC1=_cycalloc(sizeof(*_tmpAC1)),((_tmpAC1->f1=Cyc_Absyn_Loc_n,((_tmpAC1->f2=((_tmpAC2=_cycalloc(sizeof(*_tmpAC2)),((_tmpAC2[0]=_tmp4E0,_tmpAC2)))),_tmpAC1)))))),(void*)((_tmpAC4=_cycalloc(sizeof(*_tmpAC4)),((_tmpAC4[0]=((_tmpAC5.tag=15,((_tmpAC5.f1=t,_tmpAC5)))),_tmpAC4)))),0);
# 1425
{struct Cyc_List_List*_tmpAC6;yyval=Cyc_YY16(((_tmpAC6=_cycalloc(sizeof(*_tmpAC6)),((_tmpAC6->hd=Cyc_Absyn_region_decl(tv,vd,_tmp4DF,0,(unsigned int)(yyyylsp[0]).first_line),((_tmpAC6->tl=0,_tmpAC6)))))));}
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
{const char*_tmpAC7;if(Cyc_strcmp((struct _dyneither_ptr)_tmp4F2,((_tmpAC7="open",_tag_dyneither(_tmpAC7,sizeof(char),5))))!= 0){const char*_tmpAC8;Cyc_Parse_err(((_tmpAC8="expecting `open'",_tag_dyneither(_tmpAC8,sizeof(char),17))),(unsigned int)(yyyylsp[3]).first_line);}}{
struct _dyneither_ptr*_tmpAD5;const char*_tmpAD4;void*_tmpAD3[1];struct Cyc_String_pa_PrintArg_struct _tmpAD2;struct Cyc_Absyn_Tvar*_tmpAD1;struct Cyc_Absyn_Tvar*tv=(_tmpAD1=_cycalloc(sizeof(*_tmpAD1)),((_tmpAD1->name=((_tmpAD5=_cycalloc(sizeof(*_tmpAD5)),((_tmpAD5[0]=(struct _dyneither_ptr)((_tmpAD2.tag=0,((_tmpAD2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4F1),((_tmpAD3[0]=& _tmpAD2,Cyc_aprintf(((_tmpAD4="`%s",_tag_dyneither(_tmpAD4,sizeof(char),4))),_tag_dyneither(_tmpAD3,sizeof(void*),1)))))))),_tmpAD5)))),((_tmpAD1->identity=- 1,((_tmpAD1->kind=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpAD1)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpAD8;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpAD7;void*t=(void*)((_tmpAD7=_cycalloc(sizeof(*_tmpAD7)),((_tmpAD7[0]=((_tmpAD8.tag=2,((_tmpAD8.f1=tv,_tmpAD8)))),_tmpAD7))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpADE;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpADD;struct _dyneither_ptr*_tmpADB;struct _tuple0*_tmpADA;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpADA=_cycalloc(sizeof(*_tmpADA)),((_tmpADA->f1=Cyc_Absyn_Loc_n,((_tmpADA->f2=((_tmpADB=_cycalloc(sizeof(*_tmpADB)),((_tmpADB[0]=_tmp4F1,_tmpADB)))),_tmpADA)))))),(void*)((_tmpADD=_cycalloc(sizeof(*_tmpADD)),((_tmpADD[0]=((_tmpADE.tag=15,((_tmpADE.f1=t,_tmpADE)))),_tmpADD)))),0);
# 1438
{struct Cyc_List_List*_tmpADF;yyval=Cyc_YY16(((_tmpADF=_cycalloc(sizeof(*_tmpADF)),((_tmpADF->hd=Cyc_Absyn_region_decl(tv,vd,0,_tmp4F3,(unsigned int)(yyyylsp[0]).first_line),((_tmpADF->tl=0,_tmpADF)))))));}
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
{struct Cyc_Parse_Declaration_spec _tmpAE0;yyval=Cyc_YY17(((_tmpAE0.sc=Cyc_yyget_YY20(yyyyvsp[0]),((_tmpAE0.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAE0.type_specs=
Cyc_Parse_empty_spec(0),((_tmpAE0.is_inline=0,((_tmpAE0.attributes=0,_tmpAE0)))))))))));}
break;}case 46: _LL266: {
# 1461
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1463
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1460 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp503=Cyc_yyget_YY17(yyyyvsp[1]);
if(_tmp503.sc != 0){
const char*_tmpAE3;void*_tmpAE2;(_tmpAE2=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpAE3="Only one storage class is allowed in a declaration (missing ';' or ','?)",_tag_dyneither(_tmpAE3,sizeof(char),73))),_tag_dyneither(_tmpAE2,sizeof(void*),0)));}
# 1464
{struct Cyc_Parse_Declaration_spec _tmpAE4;yyval=Cyc_YY17(((_tmpAE4.sc=Cyc_yyget_YY20(yyyyvsp[0]),((_tmpAE4.tq=_tmp503.tq,((_tmpAE4.type_specs=_tmp503.type_specs,((_tmpAE4.is_inline=_tmp503.is_inline,((_tmpAE4.attributes=_tmp503.attributes,_tmpAE4)))))))))));}
# 1468
break;}case 47: _LL267: {
# 1470
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1472
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1469 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpAE5;yyval=Cyc_YY17(((_tmpAE5.sc=0,((_tmpAE5.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAE5.type_specs=
Cyc_yyget_YY21(yyyyvsp[0]),((_tmpAE5.is_inline=0,((_tmpAE5.attributes=0,_tmpAE5)))))))))));}
break;}case 48: _LL268: {
# 1473
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1475
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1472 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp508=Cyc_yyget_YY17(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpAE6;yyval=Cyc_YY17(((_tmpAE6.sc=_tmp508.sc,((_tmpAE6.tq=_tmp508.tq,((_tmpAE6.type_specs=
Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,_tmp508.type_specs,Cyc_yyget_YY21(yyyyvsp[0])),((_tmpAE6.is_inline=_tmp508.is_inline,((_tmpAE6.attributes=_tmp508.attributes,_tmpAE6)))))))))));}
# 1478
break;}case 49: _LL269: {
# 1480
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1482
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1479 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpAE7;yyval=Cyc_YY17(((_tmpAE7.sc=0,((_tmpAE7.tq=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpAE7.type_specs=Cyc_Parse_empty_spec(0),((_tmpAE7.is_inline=0,((_tmpAE7.attributes=0,_tmpAE7)))))))))));}
break;}case 50: _LL26A: {
# 1482
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1484
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1481 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp50B=Cyc_yyget_YY17(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpAE8;yyval=Cyc_YY17(((_tmpAE8.sc=_tmp50B.sc,((_tmpAE8.tq=Cyc_Absyn_combine_tqual(Cyc_yyget_YY23(yyyyvsp[0]),_tmp50B.tq),((_tmpAE8.type_specs=_tmp50B.type_specs,((_tmpAE8.is_inline=_tmp50B.is_inline,((_tmpAE8.attributes=_tmp50B.attributes,_tmpAE8)))))))))));}
# 1486
break;}case 51: _LL26B: {
# 1488
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1490
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1487 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpAE9;yyval=Cyc_YY17(((_tmpAE9.sc=0,((_tmpAE9.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAE9.type_specs=
Cyc_Parse_empty_spec(0),((_tmpAE9.is_inline=1,((_tmpAE9.attributes=0,_tmpAE9)))))))))));}
break;}case 52: _LL26C: {
# 1491
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1493
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1490 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp50E=Cyc_yyget_YY17(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpAEA;yyval=Cyc_YY17(((_tmpAEA.sc=_tmp50E.sc,((_tmpAEA.tq=_tmp50E.tq,((_tmpAEA.type_specs=_tmp50E.type_specs,((_tmpAEA.is_inline=1,((_tmpAEA.attributes=_tmp50E.attributes,_tmpAEA)))))))))));}
# 1494
break;}case 53: _LL26D: {
# 1496
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1498
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1495 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpAEB;yyval=Cyc_YY17(((_tmpAEB.sc=0,((_tmpAEB.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAEB.type_specs=
Cyc_Parse_empty_spec(0),((_tmpAEB.is_inline=0,((_tmpAEB.attributes=Cyc_yyget_YY45(yyyyvsp[0]),_tmpAEB)))))))))));}
break;}case 54: _LL26E: {
# 1499
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1501
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1498 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp511=Cyc_yyget_YY17(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpAEC;yyval=Cyc_YY17(((_tmpAEC.sc=_tmp511.sc,((_tmpAEC.tq=_tmp511.tq,((_tmpAEC.type_specs=_tmp511.type_specs,((_tmpAEC.is_inline=_tmp511.is_inline,((_tmpAEC.attributes=
# 1501
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY45(yyyyvsp[0]),_tmp511.attributes),_tmpAEC)))))))))));}
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
{const char*_tmpAED;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpAED="C",_tag_dyneither(_tmpAED,sizeof(char),2))))!= 0){
const char*_tmpAEE;Cyc_Parse_err(((_tmpAEE="only extern or extern \"C\" is allowed",_tag_dyneither(_tmpAEE,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
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
{struct Cyc_List_List*_tmpAEF;yyval=Cyc_YY45(((_tmpAEF=_cycalloc(sizeof(*_tmpAEF)),((_tmpAEF->hd=Cyc_yyget_YY46(yyyyvsp[0]),((_tmpAEF->tl=0,_tmpAEF)))))));}
break;}case 66: _LL27A: {
# 1541
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1543
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1539 "parse.y"
{struct Cyc_List_List*_tmpAF0;yyval=Cyc_YY45(((_tmpAF0=_cycalloc(sizeof(*_tmpAF0)),((_tmpAF0->hd=Cyc_yyget_YY46(yyyyvsp[0]),((_tmpAF0->tl=Cyc_yyget_YY45(yyyyvsp[2]),_tmpAF0)))))));}
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
{const char*_tmpAF1;Cyc_Parse_err(((_tmpAF1="unrecognized attribute",_tag_dyneither(_tmpAF1,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
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
{const char*_tmpAF3;const char*_tmpAF2;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpAF3="aligned",_tag_dyneither(_tmpAF3,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpAF2="__aligned__",_tag_dyneither(_tmpAF2,sizeof(char),12))))== 0){
struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpAF6;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpAF5;a=(void*)((_tmpAF5=_cycalloc(sizeof(*_tmpAF5)),((_tmpAF5[0]=((_tmpAF6.tag=6,((_tmpAF6.f1=_tmp52C,_tmpAF6)))),_tmpAF5))));}else{
const char*_tmpAF8;const char*_tmpAF7;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpAF8="section",_tag_dyneither(_tmpAF8,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpAF7="__section__",_tag_dyneither(_tmpAF7,sizeof(char),12))))== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp52C);
struct Cyc_Absyn_Section_att_Absyn_Attribute_struct _tmpAFB;struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmpAFA;a=(void*)((_tmpAFA=_cycalloc(sizeof(*_tmpAFA)),((_tmpAFA[0]=((_tmpAFB.tag=8,((_tmpAFB.f1=str,_tmpAFB)))),_tmpAFA))));}else{
const char*_tmpAFC;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpAFC="__mode__",_tag_dyneither(_tmpAFC,sizeof(char),9))))== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp52C);
struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct _tmpAFF;struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmpAFE;a=(void*)((_tmpAFE=_cycalloc(sizeof(*_tmpAFE)),((_tmpAFE[0]=((_tmpAFF.tag=24,((_tmpAFF.f1=str,_tmpAFF)))),_tmpAFE))));}else{
# 1595
int n=Cyc_Parse_exp2int((unsigned int)(yyyylsp[2]).first_line,_tmp52C);
const char*_tmpB01;const char*_tmpB00;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpB01="regparm",_tag_dyneither(_tmpB01,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpB00="__regparm__",_tag_dyneither(_tmpB00,sizeof(char),12))))== 0){
if(n < 0  || n > 3){
const char*_tmpB02;Cyc_Parse_err(((_tmpB02="regparm requires value between 0 and 3",_tag_dyneither(_tmpB02,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct _tmpB05;struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmpB04;a=(void*)((_tmpB04=_cycalloc_atomic(sizeof(*_tmpB04)),((_tmpB04[0]=((_tmpB05.tag=0,((_tmpB05.f1=n,_tmpB05)))),_tmpB04))));};}else{
const char*_tmpB07;const char*_tmpB06;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpB07="initializes",_tag_dyneither(_tmpB07,sizeof(char),12))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpB06="__initializes__",_tag_dyneither(_tmpB06,sizeof(char),16))))== 0){
struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct _tmpB0A;struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmpB09;a=(void*)((_tmpB09=_cycalloc_atomic(sizeof(*_tmpB09)),((_tmpB09[0]=((_tmpB0A.tag=20,((_tmpB0A.f1=n,_tmpB0A)))),_tmpB09))));}else{
const char*_tmpB0C;const char*_tmpB0B;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpB0C="noliveunique",_tag_dyneither(_tmpB0C,sizeof(char),13))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpB0B="__noliveunique__",_tag_dyneither(_tmpB0B,sizeof(char),17))))== 0){
struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct _tmpB0F;struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmpB0E;a=(void*)((_tmpB0E=_cycalloc_atomic(sizeof(*_tmpB0E)),((_tmpB0E[0]=((_tmpB0F.tag=21,((_tmpB0F.f1=n,_tmpB0F)))),_tmpB0E))));}else{
const char*_tmpB11;const char*_tmpB10;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpB11="noconsume",_tag_dyneither(_tmpB11,sizeof(char),10))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpB10="__noconsume__",_tag_dyneither(_tmpB10,sizeof(char),14))))== 0){
struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct _tmpB14;struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmpB13;a=(void*)((_tmpB13=_cycalloc_atomic(sizeof(*_tmpB13)),((_tmpB13[0]=((_tmpB14.tag=22,((_tmpB14.f1=n,_tmpB14)))),_tmpB13))));}else{
# 1607
{const char*_tmpB15;Cyc_Parse_err(((_tmpB15="unrecognized attribute",_tag_dyneither(_tmpB15,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
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
{const char*_tmpB17;const char*_tmpB16;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp54A,((_tmpB17="format",_tag_dyneither(_tmpB17,sizeof(char),7))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp54A,((_tmpB16="__format__",_tag_dyneither(_tmpB16,sizeof(char),11))))== 0){
const char*_tmpB18;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp54B,((_tmpB18="printf",_tag_dyneither(_tmpB18,sizeof(char),7))))== 0){
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpB1B;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpB1A;a=(void*)((_tmpB1A=_cycalloc_atomic(sizeof(*_tmpB1A)),((_tmpB1A[0]=((_tmpB1B.tag=19,((_tmpB1B.f1=Cyc_Absyn_Printf_ft,((_tmpB1B.f2=(int)_tmp54C,((_tmpB1B.f3=(int)_tmp54D,_tmpB1B)))))))),_tmpB1A))));}else{
const char*_tmpB1C;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp54B,((_tmpB1C="scanf",_tag_dyneither(_tmpB1C,sizeof(char),6))))== 0){
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpB1F;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpB1E;a=(void*)((_tmpB1E=_cycalloc_atomic(sizeof(*_tmpB1E)),((_tmpB1E[0]=((_tmpB1F.tag=19,((_tmpB1F.f1=Cyc_Absyn_Scanf_ft,((_tmpB1F.f2=(int)_tmp54C,((_tmpB1F.f3=(int)_tmp54D,_tmpB1F)))))))),_tmpB1E))));}else{
# 1625
const char*_tmpB20;Cyc_Parse_err(((_tmpB20="unrecognized format type",_tag_dyneither(_tmpB20,sizeof(char),25))),(unsigned int)(yyyylsp[2]).first_line);}}}else{
# 1627
const char*_tmpB21;Cyc_Parse_err(((_tmpB21="unrecognized attribute",_tag_dyneither(_tmpB21,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}}
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
{struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpB24;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpB23;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB23=_cycalloc(sizeof(*_tmpB23)),((_tmpB23[0]=((_tmpB24.tag=17,((_tmpB24.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB24.f2=Cyc_yyget_YY40(yyyyvsp[1]),((_tmpB24.f3=0,((_tmpB24.f4=0,_tmpB24)))))))))),_tmpB23)))),(unsigned int)(yyyylsp[0]).first_line));}
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
{struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpB27;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpB26;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB26=_cycalloc(sizeof(*_tmpB26)),((_tmpB26[0]=((_tmpB27.tag=10,((_tmpB27.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[2]).first_line,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[2]))),_tmpB27)))),_tmpB26)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1668
break;}case 89: _LL291: {
# 1670
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1672
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1669 "parse.y"
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpB2A;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpB29;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB29=_cycalloc(sizeof(*_tmpB29)),((_tmpB29[0]=((_tmpB2A.tag=15,((_tmpB2A.f1=Cyc_yyget_YY44(yyyyvsp[2]),_tmpB2A)))),_tmpB29)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 90: _LL292: {
# 1672
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1674
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1671 "parse.y"
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpB2D;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpB2C;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB2C=_cycalloc(sizeof(*_tmpB2C)),((_tmpB2C[0]=((_tmpB2D.tag=15,((_tmpB2D.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0),_tmpB2D)))),_tmpB2C)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1673
break;}case 91: _LL293: {
# 1675
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1677
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1674 "parse.y"
void*_tmp560=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0);
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpB30;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpB2F;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB2F=_cycalloc(sizeof(*_tmpB2F)),((_tmpB2F[0]=((_tmpB30.tag=16,((_tmpB30.f1=Cyc_yyget_YY44(yyyyvsp[2]),((_tmpB30.f2=_tmp560,_tmpB30)))))),_tmpB2F)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1677
break;}case 92: _LL294: {
# 1679
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1681
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1678 "parse.y"
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpB33;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpB32;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB32=_cycalloc(sizeof(*_tmpB32)),((_tmpB32[0]=((_tmpB33.tag=16,((_tmpB33.f1=Cyc_yyget_YY44(yyyyvsp[2]),((_tmpB33.f2=Cyc_yyget_YY44(yyyyvsp[4]),_tmpB33)))))),_tmpB32)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 93: _LL295: {
# 1681
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1683
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1680 "parse.y"
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpB36;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpB35;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB35=_cycalloc(sizeof(*_tmpB35)),((_tmpB35[0]=((_tmpB36.tag=19,((_tmpB36.f1=Cyc_yyget_YY44(yyyyvsp[2]),_tmpB36)))),_tmpB35)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 94: _LL296: {
# 1683
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1685
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1682 "parse.y"
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpB39;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpB38;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB38=_cycalloc(sizeof(*_tmpB38)),((_tmpB38[0]=((_tmpB39.tag=19,((_tmpB39.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_iko,0),_tmpB39)))),_tmpB38)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 95: _LL297: {
# 1685
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1687
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1684 "parse.y"
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpB3C;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpB3B;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB3B=_cycalloc(sizeof(*_tmpB3B)),((_tmpB3B[0]=((_tmpB3C.tag=18,((_tmpB3C.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),_tmpB3C)))),_tmpB3B)))),(unsigned int)(yyyylsp[0]).first_line));}
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
{struct Cyc_Absyn_Tqual _tmpB3D;yyval=Cyc_YY23(((_tmpB3D.print_const=1,((_tmpB3D.q_volatile=0,((_tmpB3D.q_restrict=0,((_tmpB3D.real_const=1,((_tmpB3D.loc=loc,_tmpB3D)))))))))));}
break;}case 98: _LL29A: {
# 1697
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1699
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1695 "parse.y"
{struct Cyc_Absyn_Tqual _tmpB3E;yyval=Cyc_YY23(((_tmpB3E.print_const=0,((_tmpB3E.q_volatile=1,((_tmpB3E.q_restrict=0,((_tmpB3E.real_const=0,((_tmpB3E.loc=0,_tmpB3E)))))))))));}
break;}case 99: _LL29B: {
# 1698
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1700
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1696 "parse.y"
{struct Cyc_Absyn_Tqual _tmpB3F;yyval=Cyc_YY23(((_tmpB3F.print_const=0,((_tmpB3F.q_volatile=0,((_tmpB3F.q_restrict=1,((_tmpB3F.real_const=0,((_tmpB3F.loc=0,_tmpB3F)))))))))));}
break;}case 100: _LL29C: {
# 1699
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1701
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpB4E;struct Cyc_Absyn_Enumdecl*_tmpB4D;struct Cyc_Core_Opt*_tmpB4C;struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct _tmpB4B;struct Cyc_Absyn_TypeDecl*_tmpB4A;struct Cyc_Absyn_TypeDecl*_tmp56E=(_tmpB4A=_cycalloc(sizeof(*_tmpB4A)),((_tmpB4A->r=(void*)((_tmpB4E=_cycalloc(sizeof(*_tmpB4E)),((_tmpB4E[0]=((_tmpB4B.tag=1,((_tmpB4B.f1=((_tmpB4D=_cycalloc(sizeof(*_tmpB4D)),((_tmpB4D->sc=Cyc_Absyn_Public,((_tmpB4D->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB4D->fields=((_tmpB4C=_cycalloc(sizeof(*_tmpB4C)),((_tmpB4C->v=Cyc_yyget_YY48(yyyyvsp[3]),_tmpB4C)))),_tmpB4D)))))))),_tmpB4B)))),_tmpB4E)))),((_tmpB4A->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpB4A)))));
# 1704
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB51;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB50;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB50=_cycalloc(sizeof(*_tmpB50)),((_tmpB50[0]=((_tmpB51.tag=26,((_tmpB51.f1=_tmp56E,((_tmpB51.f2=0,_tmpB51)))))),_tmpB50)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1706
break;}case 101: _LL29D: {
# 1708
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1710
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1707 "parse.y"
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpB54;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpB53;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB53=_cycalloc(sizeof(*_tmpB53)),((_tmpB53[0]=((_tmpB54.tag=13,((_tmpB54.f1=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB54.f2=0,_tmpB54)))))),_tmpB53)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 102: _LL29E: {
# 1710
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1712
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1709 "parse.y"
{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpB57;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpB56;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB56=_cycalloc(sizeof(*_tmpB56)),((_tmpB56[0]=((_tmpB57.tag=14,((_tmpB57.f1=Cyc_yyget_YY48(yyyyvsp[2]),_tmpB57)))),_tmpB56)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 103: _LL29F: {
# 1712
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1714
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_Absyn_Enumfield*_tmpB58;yyval=Cyc_YY47(((_tmpB58=_cycalloc(sizeof(*_tmpB58)),((_tmpB58->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB58->tag=0,((_tmpB58->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpB58)))))))));}
break;}case 104: _LL2A0: {
# 1718
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1720
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1717 "parse.y"
{struct Cyc_Absyn_Enumfield*_tmpB59;yyval=Cyc_YY47(((_tmpB59=_cycalloc(sizeof(*_tmpB59)),((_tmpB59->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB59->tag=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpB59->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpB59)))))))));}
break;}case 105: _LL2A1: {
# 1720
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1722
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1721 "parse.y"
{struct Cyc_List_List*_tmpB5A;yyval=Cyc_YY48(((_tmpB5A=_cycalloc(sizeof(*_tmpB5A)),((_tmpB5A->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpB5A->tl=0,_tmpB5A)))))));}
break;}case 106: _LL2A2: {
# 1724
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1726
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1722 "parse.y"
{struct Cyc_List_List*_tmpB5B;yyval=Cyc_YY48(((_tmpB5B=_cycalloc(sizeof(*_tmpB5B)),((_tmpB5B->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpB5B->tl=0,_tmpB5B)))))));}
break;}case 107: _LL2A3: {
# 1725
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1727
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1723 "parse.y"
{struct Cyc_List_List*_tmpB5C;yyval=Cyc_YY48(((_tmpB5C=_cycalloc(sizeof(*_tmpB5C)),((_tmpB5C->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpB5C->tl=Cyc_yyget_YY48(yyyyvsp[2]),_tmpB5C)))))));}
break;}case 108: _LL2A4: {
# 1726
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1728
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpB5F;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpB5E;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB5E=_cycalloc(sizeof(*_tmpB5E)),((_tmpB5E[0]=((_tmpB5F.tag=12,((_tmpB5F.f1=Cyc_yyget_YY22(yyyyvsp[0]),((_tmpB5F.f2=Cyc_yyget_YY24(yyyyvsp[2]),_tmpB5F)))))),_tmpB5E)))),(unsigned int)(yyyylsp[0]).first_line));}
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
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB62;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB61;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB61=_cycalloc(sizeof(*_tmpB61)),((_tmpB61[0]=((_tmpB62.tag=26,((_tmpB62.f1=_tmp583,((_tmpB62.f2=0,_tmpB62)))))),_tmpB61)))),(unsigned int)(yyyylsp[0]).first_line));}
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
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB65;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB64;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB64=_cycalloc(sizeof(*_tmpB64)),((_tmpB64[0]=((_tmpB65.tag=26,((_tmpB65.f1=_tmp588,((_tmpB65.f2=0,_tmpB65)))))),_tmpB64)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1753
break;}case 111: _LL2A7: {
# 1755
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1757
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1754 "parse.y"
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB6F;struct Cyc_Core_Opt*_tmpB6E;struct Cyc_Absyn_AggrInfo _tmpB6D;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB6C;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB6C=_cycalloc(sizeof(*_tmpB6C)),((_tmpB6C[0]=((_tmpB6F.tag=11,((_tmpB6F.f1=((_tmpB6D.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY22(yyyyvsp[1]),Cyc_yyget_QualId_tok(yyyyvsp[2]),((_tmpB6E=_cycalloc_atomic(sizeof(*_tmpB6E)),((_tmpB6E->v=(void*)1,_tmpB6E))))),((_tmpB6D.targs=Cyc_yyget_YY40(yyyyvsp[3]),_tmpB6D)))),_tmpB6F)))),_tmpB6C)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1757
break;}case 112: _LL2A8: {
# 1759
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1761
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1758 "parse.y"
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB75;struct Cyc_Absyn_AggrInfo _tmpB74;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB73;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB73=_cycalloc(sizeof(*_tmpB73)),((_tmpB73[0]=((_tmpB75.tag=11,((_tmpB75.f1=((_tmpB74.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY22(yyyyvsp[0]),Cyc_yyget_QualId_tok(yyyyvsp[1]),0),((_tmpB74.targs=Cyc_yyget_YY40(yyyyvsp[2]),_tmpB74)))),_tmpB75)))),_tmpB73)))),(unsigned int)(yyyylsp[0]).first_line));}
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
{struct Cyc_List_List*_tmpB76;yyval=Cyc_YY25(((_tmpB76=_cycalloc(sizeof(*_tmpB76)),((_tmpB76->hd=Cyc_yyget_YY24(yyyyvsp[0]),((_tmpB76->tl=0,_tmpB76)))))));}
break;}case 120: _LL2B0: {
# 1796
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1798
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1795 "parse.y"
{struct Cyc_List_List*_tmpB77;yyval=Cyc_YY25(((_tmpB77=_cycalloc(sizeof(*_tmpB77)),((_tmpB77->hd=Cyc_yyget_YY24(yyyyvsp[1]),((_tmpB77->tl=Cyc_yyget_YY25(yyyyvsp[0]),_tmpB77)))))));}
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
{struct _tuple12*_tmpB78;yyval=Cyc_YY19(((_tmpB78=_region_malloc(yyr,sizeof(*_tmpB78)),((_tmpB78->tl=0,((_tmpB78->hd=Cyc_yyget_YY18(yyyyvsp[0]),_tmpB78)))))));}
break;}case 123: _LL2B3: {
# 1808
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1810
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1807 "parse.y"
{struct _tuple12*_tmpB79;yyval=Cyc_YY19(((_tmpB79=_region_malloc(yyr,sizeof(*_tmpB79)),((_tmpB79->tl=Cyc_yyget_YY19(yyyyvsp[0]),((_tmpB79->hd=Cyc_yyget_YY18(yyyyvsp[2]),_tmpB79)))))));}
break;}case 124: _LL2B4: {
# 1810
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1812
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1812 "parse.y"
{struct _tuple11 _tmpB7A;yyval=Cyc_YY18(((_tmpB7A.f1=Cyc_yyget_YY27(yyyyvsp[0]),((_tmpB7A.f2=0,_tmpB7A)))));}
break;}case 125: _LL2B5: {
# 1815
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1817
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1814 "parse.y"
{struct _tuple11 _tmpB7B;yyval=Cyc_YY18(((_tmpB7B.f1=Cyc_yyget_YY27(yyyyvsp[0]),((_tmpB7B.f2=Cyc_yyget_Exp_tok(yyyyvsp[2]),_tmpB7B)))));}
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
{struct _tuple10*_tmpB7C;decls=((_tmpB7C=_region_malloc(temp,sizeof(*_tmpB7C)),((_tmpB7C->tl=decls,((_tmpB7C->hd=_tmp5A3,_tmpB7C))))));}{
struct _tuple16*_tmpB7F;struct Cyc_List_List*_tmpB7E;widths_and_reqs=(
(_tmpB7E=_region_malloc(temp,sizeof(*_tmpB7E)),((_tmpB7E->hd=((_tmpB7F=_region_malloc(temp,sizeof(*_tmpB7F)),((_tmpB7F->f1=_tmp5A4,((_tmpB7F->f2=_tmp5A5,_tmpB7F)))))),((_tmpB7E->tl=widths_and_reqs,_tmpB7E))))));};}}
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
{struct _tuple25 _tmpB80;yyval=Cyc_YY35(((_tmpB80.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB80.f2=Cyc_yyget_YY21(yyyyvsp[0]),((_tmpB80.f3=0,_tmpB80)))))));}
break;}case 128: _LL2B8: {
# 1851
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1853
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1850 "parse.y"
struct _tuple25 _tmp5AC=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple25 _tmpB81;yyval=Cyc_YY35(((_tmpB81.f1=_tmp5AC.f1,((_tmpB81.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY21(yyyyvsp[0]),_tmp5AC.f2),((_tmpB81.f3=_tmp5AC.f3,_tmpB81)))))));}
break;}case 129: _LL2B9: {
# 1853
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1855
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1852 "parse.y"
{struct _tuple25 _tmpB82;yyval=Cyc_YY35(((_tmpB82.f1=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpB82.f2=Cyc_Parse_empty_spec(0),((_tmpB82.f3=0,_tmpB82)))))));}
break;}case 130: _LL2BA: {
# 1855
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1857
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1854 "parse.y"
struct _tuple25 _tmp5AF=Cyc_yyget_YY35(yyyyvsp[1]);
{struct _tuple25 _tmpB83;yyval=Cyc_YY35(((_tmpB83.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY23(yyyyvsp[0]),_tmp5AF.f1),((_tmpB83.f2=_tmp5AF.f2,((_tmpB83.f3=_tmp5AF.f3,_tmpB83)))))));}
break;}case 131: _LL2BB: {
# 1858
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1860
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1857 "parse.y"
{struct _tuple25 _tmpB84;yyval=Cyc_YY35(((_tmpB84.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB84.f2=Cyc_Parse_empty_spec(0),((_tmpB84.f3=Cyc_yyget_YY45(yyyyvsp[0]),_tmpB84)))))));}
break;}case 132: _LL2BC: {
# 1860
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1862
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1859 "parse.y"
struct _tuple25 _tmp5B2=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple25 _tmpB85;yyval=Cyc_YY35(((_tmpB85.f1=_tmp5B2.f1,((_tmpB85.f2=_tmp5B2.f2,((_tmpB85.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY45(yyyyvsp[0]),_tmp5B2.f3),_tmpB85)))))));}
break;}case 133: _LL2BD: {
# 1862
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1864
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct _tuple25 _tmpB86;yyval=Cyc_YY35(((_tmpB86.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB86.f2=Cyc_yyget_YY21(yyyyvsp[0]),((_tmpB86.f3=0,_tmpB86)))))));}
break;}case 134: _LL2BE: {
# 1868
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1870
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1867 "parse.y"
struct _tuple25 _tmp5B5=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple25 _tmpB87;yyval=Cyc_YY35(((_tmpB87.f1=_tmp5B5.f1,((_tmpB87.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY21(yyyyvsp[0]),_tmp5B5.f2),((_tmpB87.f3=_tmp5B5.f3,_tmpB87)))))));}
break;}case 135: _LL2BF: {
# 1870
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1872
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1869 "parse.y"
{struct _tuple25 _tmpB88;yyval=Cyc_YY35(((_tmpB88.f1=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpB88.f2=Cyc_Parse_empty_spec(0),((_tmpB88.f3=0,_tmpB88)))))));}
break;}case 136: _LL2C0: {
# 1872
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1874
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1871 "parse.y"
struct _tuple25 _tmp5B8=Cyc_yyget_YY35(yyyyvsp[1]);
{struct _tuple25 _tmpB89;yyval=Cyc_YY35(((_tmpB89.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY23(yyyyvsp[0]),_tmp5B8.f1),((_tmpB89.f2=_tmp5B8.f2,((_tmpB89.f3=_tmp5B8.f3,_tmpB89)))))));}
break;}case 137: _LL2C1: {
# 1875
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1877
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1874 "parse.y"
{struct _tuple25 _tmpB8A;yyval=Cyc_YY35(((_tmpB8A.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB8A.f2=Cyc_Parse_empty_spec(0),((_tmpB8A.f3=Cyc_yyget_YY45(yyyyvsp[0]),_tmpB8A)))))));}
break;}case 138: _LL2C2: {
# 1877
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1879
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1876 "parse.y"
struct _tuple25 _tmp5BB=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple25 _tmpB8B;yyval=Cyc_YY35(((_tmpB8B.f1=_tmp5BB.f1,((_tmpB8B.f2=_tmp5BB.f2,((_tmpB8B.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY45(yyyyvsp[0]),_tmp5BB.f3),_tmpB8B)))))));}
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
{struct Cyc_List_List*_tmpB8C;yyval=Cyc_YY29(((_tmpB8C=_region_malloc(yyr,sizeof(*_tmpB8C)),((_tmpB8C->hd=Cyc_yyget_YY28(yyyyvsp[0]),((_tmpB8C->tl=0,_tmpB8C)))))));}
break;}case 141: _LL2C5: {
# 1889
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1891
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1888 "parse.y"
{struct Cyc_List_List*_tmpB8D;yyval=Cyc_YY29(((_tmpB8D=_region_malloc(yyr,sizeof(*_tmpB8D)),((_tmpB8D->hd=Cyc_yyget_YY28(yyyyvsp[2]),((_tmpB8D->tl=Cyc_yyget_YY29(yyyyvsp[0]),_tmpB8D)))))));}
break;}case 142: _LL2C6: {
# 1891
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1893
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1893 "parse.y"
{struct _tuple24*_tmpB8E;yyval=Cyc_YY28(((_tmpB8E=_region_malloc(yyr,sizeof(*_tmpB8E)),((_tmpB8E->f1=Cyc_yyget_YY27(yyyyvsp[0]),((_tmpB8E->f2=0,((_tmpB8E->f3=Cyc_yyget_YY55(yyyyvsp[1]),_tmpB8E)))))))));}
break;}case 143: _LL2C7: {
# 1896
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1898
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1897 "parse.y"
{struct Cyc_Parse_Declarator _tmpB94;struct _tuple0*_tmpB93;struct _tuple24*_tmpB92;yyval=Cyc_YY28(((_tmpB92=_region_malloc(yyr,sizeof(*_tmpB92)),((_tmpB92->f1=((_tmpB94.id=((_tmpB93=_cycalloc(sizeof(*_tmpB93)),((_tmpB93->f1=Cyc_Absyn_Rel_n(0),((_tmpB93->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpB93)))))),((_tmpB94.tms=0,_tmpB94)))),((_tmpB92->f2=Cyc_yyget_Exp_tok(yyyyvsp[1]),((_tmpB92->f3=0,_tmpB92)))))))));}
# 1899
break;}case 144: _LL2C8:
# 1903
{struct Cyc_Parse_Declarator _tmpB9A;struct _tuple0*_tmpB99;struct _tuple24*_tmpB98;yyval=Cyc_YY28(((_tmpB98=_region_malloc(yyr,sizeof(*_tmpB98)),((_tmpB98->f1=((_tmpB9A.id=((_tmpB99=_cycalloc(sizeof(*_tmpB99)),((_tmpB99->f1=Cyc_Absyn_Rel_n(0),((_tmpB99->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpB99)))))),((_tmpB9A.tms=0,_tmpB9A)))),((_tmpB98->f2=0,((_tmpB98->f3=0,_tmpB98)))))))));}
# 1905
break;case 145: _LL2C9: {
# 1907
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1909
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1905 "parse.y"
{struct _tuple24*_tmpB9B;yyval=Cyc_YY28(((_tmpB9B=_region_malloc(yyr,sizeof(*_tmpB9B)),((_tmpB9B->f1=Cyc_yyget_YY27(yyyyvsp[0]),((_tmpB9B->f2=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpB9B->f3=0,_tmpB9B)))))))));}
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
struct Cyc_Core_Opt*_tmpB9C;struct Cyc_Absyn_TypeDecl*_tmp5CB=Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp5CA,((_tmpB9C=_cycalloc(sizeof(*_tmpB9C)),((_tmpB9C->v=Cyc_yyget_YY34(yyyyvsp[4]),_tmpB9C)))),_tmp5C9,(unsigned int)(yyyylsp[0]).first_line);
# 1925
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB9F;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB9E;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpB9E=_cycalloc(sizeof(*_tmpB9E)),((_tmpB9E[0]=((_tmpB9F.tag=26,((_tmpB9F.f1=_tmp5CB,((_tmpB9F.f2=0,_tmpB9F)))))),_tmpB9E)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1927
break;}case 151: _LL2CF: {
# 1929
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1931
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1928 "parse.y"
int _tmp5CF=Cyc_yyget_YY31(yyyyvsp[0]);
{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpBA9;struct Cyc_Absyn_UnknownDatatypeInfo _tmpBA8;struct Cyc_Absyn_DatatypeInfo _tmpBA7;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpBA6;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpBA6=_cycalloc(sizeof(*_tmpBA6)),((_tmpBA6[0]=((_tmpBA9.tag=3,((_tmpBA9.f1=((_tmpBA7.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpBA8.name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpBA8.is_extensible=_tmp5CF,_tmpBA8))))),((_tmpBA7.targs=Cyc_yyget_YY40(yyyyvsp[2]),_tmpBA7)))),_tmpBA9)))),_tmpBA6)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1931
break;}case 152: _LL2D0: {
# 1933
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1935
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1932 "parse.y"
int _tmp5D4=Cyc_yyget_YY31(yyyyvsp[0]);
{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpBB3;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmpBB2;struct Cyc_Absyn_DatatypeFieldInfo _tmpBB1;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpBB0;yyval=Cyc_YY21(Cyc_Parse_type_spec((void*)((_tmpBB0=_cycalloc(sizeof(*_tmpBB0)),((_tmpBB0[0]=((_tmpBB3.tag=4,((_tmpBB3.f1=((_tmpBB1.field_info=
Cyc_Absyn_UnknownDatatypefield(((_tmpBB2.datatype_name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpBB2.field_name=Cyc_yyget_QualId_tok(yyyyvsp[3]),((_tmpBB2.is_extensible=_tmp5D4,_tmpBB2))))))),((_tmpBB1.targs=Cyc_yyget_YY40(yyyyvsp[4]),_tmpBB1)))),_tmpBB3)))),_tmpBB0)))),(unsigned int)(yyyylsp[0]).first_line));}
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
{struct Cyc_List_List*_tmpBB4;yyval=Cyc_YY34(((_tmpBB4=_cycalloc(sizeof(*_tmpBB4)),((_tmpBB4->hd=Cyc_yyget_YY33(yyyyvsp[0]),((_tmpBB4->tl=0,_tmpBB4)))))));}
break;}case 156: _LL2D4: {
# 1948
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1950
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1946 "parse.y"
{struct Cyc_List_List*_tmpBB5;yyval=Cyc_YY34(((_tmpBB5=_cycalloc(sizeof(*_tmpBB5)),((_tmpBB5->hd=Cyc_yyget_YY33(yyyyvsp[0]),((_tmpBB5->tl=0,_tmpBB5)))))));}
break;}case 157: _LL2D5: {
# 1949
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1951
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1947 "parse.y"
{struct Cyc_List_List*_tmpBB6;yyval=Cyc_YY34(((_tmpBB6=_cycalloc(sizeof(*_tmpBB6)),((_tmpBB6->hd=Cyc_yyget_YY33(yyyyvsp[0]),((_tmpBB6->tl=Cyc_yyget_YY34(yyyyvsp[2]),_tmpBB6)))))));}
break;}case 158: _LL2D6: {
# 1950
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1952
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1948 "parse.y"
{struct Cyc_List_List*_tmpBB7;yyval=Cyc_YY34(((_tmpBB7=_cycalloc(sizeof(*_tmpBB7)),((_tmpBB7->hd=Cyc_yyget_YY33(yyyyvsp[0]),((_tmpBB7->tl=Cyc_yyget_YY34(yyyyvsp[2]),_tmpBB7)))))));}
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
{struct Cyc_Absyn_Datatypefield*_tmpBB8;yyval=Cyc_YY33(((_tmpBB8=_cycalloc(sizeof(*_tmpBB8)),((_tmpBB8->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpBB8->typs=0,((_tmpBB8->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpBB8->sc=Cyc_yyget_YY32(yyyyvsp[0]),_tmpBB8)))))))))));}
break;}case 163: _LL2DB: {
# 1961
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1963
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1960 "parse.y"
struct Cyc_List_List*_tmp5DE=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[3]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[3])));
{struct Cyc_Absyn_Datatypefield*_tmpBB9;yyval=Cyc_YY33(((_tmpBB9=_cycalloc(sizeof(*_tmpBB9)),((_tmpBB9->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpBB9->typs=_tmp5DE,((_tmpBB9->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpBB9->sc=Cyc_yyget_YY32(yyyyvsp[0]),_tmpBB9)))))))))));}
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
{struct Cyc_Parse_Declarator _tmpBBA;yyval=Cyc_YY27(((_tmpBBA.id=_tmp5E0.id,((_tmpBBA.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY26(yyyyvsp[0]),_tmp5E0.tms),_tmpBBA)))));}
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
{struct Cyc_Parse_Declarator _tmpBBB;yyval=Cyc_YY27(((_tmpBBB.id=_tmp5E2.id,((_tmpBBB.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY26(yyyyvsp[0]),_tmp5E2.tms),_tmpBBB)))));}
break;}case 168: _LL2E0: {
# 1981
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1983
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1983 "parse.y"
{struct Cyc_Parse_Declarator _tmpBBC;yyval=Cyc_YY27(((_tmpBBC.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpBBC.tms=0,_tmpBBC)))));}
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
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpBC2;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpBC1;struct Cyc_List_List*_tmpBC0;_tmp5E5.tms=((_tmpBC0=_region_malloc(yyr,sizeof(*_tmpBC0)),((_tmpBC0->hd=(void*)((_tmpBC2=_region_malloc(yyr,sizeof(*_tmpBC2)),((_tmpBC2[0]=((_tmpBC1.tag=5,((_tmpBC1.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpBC1.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpBC1)))))),_tmpBC2)))),((_tmpBC0->tl=_tmp5E5.tms,_tmpBC0))))));}
yyval=yyyyvsp[2];
# 1993
break;}case 171: _LL2E3: {
# 1995
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1997
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1994 "parse.y"
{struct Cyc_List_List*_tmpBCC;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpBCB;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpBCA;struct Cyc_Parse_Declarator _tmpBC9;yyval=Cyc_YY27(((_tmpBC9.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpBC9.tms=((_tmpBCC=_region_malloc(yyr,sizeof(*_tmpBCC)),((_tmpBCC->hd=(void*)((_tmpBCA=_region_malloc(yyr,sizeof(*_tmpBCA)),((_tmpBCA[0]=((_tmpBCB.tag=0,((_tmpBCB.f1=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpBCB.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpBCB)))))),_tmpBCA)))),((_tmpBCC->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpBCC)))))),_tmpBC9)))));}
break;}case 172: _LL2E4: {
# 1997
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1999
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1996 "parse.y"
{struct Cyc_List_List*_tmpBD6;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpBD5;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpBD4;struct Cyc_Parse_Declarator _tmpBD3;yyval=Cyc_YY27(((_tmpBD3.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpBD3.tms=(
(_tmpBD6=_region_malloc(yyr,sizeof(*_tmpBD6)),((_tmpBD6->hd=(void*)((_tmpBD4=_region_malloc(yyr,sizeof(*_tmpBD4)),((_tmpBD4[0]=((_tmpBD5.tag=1,((_tmpBD5.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpBD5.f2=Cyc_yyget_YY51(yyyyvsp[4]),((_tmpBD5.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpBD5)))))))),_tmpBD4)))),((_tmpBD6->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpBD6)))))),_tmpBD3)))));}
break;}case 173: _LL2E5: {
# 2000
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2002
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1999 "parse.y"
struct _tuple26*_tmp5F1=Cyc_yyget_YY39(yyyyvsp[2]);struct Cyc_List_List*_tmp5F3;int _tmp5F4;struct Cyc_Absyn_VarargInfo*_tmp5F5;void*_tmp5F6;struct Cyc_List_List*_tmp5F7;struct _tuple26*_tmp5F2=_tmp5F1;_tmp5F3=_tmp5F2->f1;_tmp5F4=_tmp5F2->f2;_tmp5F5=_tmp5F2->f3;_tmp5F6=_tmp5F2->f4;_tmp5F7=_tmp5F2->f5;{
struct Cyc_Absyn_Exp*_tmp5F8=Cyc_yyget_YY55(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp5F9=Cyc_yyget_YY55(yyyyvsp[5]);
{struct Cyc_List_List*_tmpBEB;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBEA;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpBE9;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpBE8;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBE7;struct Cyc_Parse_Declarator _tmpBE6;yyval=Cyc_YY27(((_tmpBE6.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpBE6.tms=((_tmpBEB=_region_malloc(yyr,sizeof(*_tmpBEB)),((_tmpBEB->hd=(void*)((_tmpBE7=_region_malloc(yyr,sizeof(*_tmpBE7)),((_tmpBE7[0]=((_tmpBEA.tag=3,((_tmpBEA.f1=(void*)((_tmpBE8=_region_malloc(yyr,sizeof(*_tmpBE8)),((_tmpBE8[0]=((_tmpBE9.tag=1,((_tmpBE9.f1=_tmp5F3,((_tmpBE9.f2=_tmp5F4,((_tmpBE9.f3=_tmp5F5,((_tmpBE9.f4=_tmp5F6,((_tmpBE9.f5=_tmp5F7,((_tmpBE9.f6=_tmp5F8,((_tmpBE9.f7=_tmp5F9,_tmpBE9)))))))))))))))),_tmpBE8)))),_tmpBEA)))),_tmpBE7)))),((_tmpBEB->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpBEB)))))),_tmpBE6)))));}
# 2004
break;};}case 174: _LL2E6: {
# 2006
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2008
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2005 "parse.y"
{struct Cyc_List_List*_tmpC00;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBFF;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpBFE;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpBFD;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBFC;struct Cyc_Parse_Declarator _tmpBFB;yyval=Cyc_YY27(((_tmpBFB.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpBFB.tms=(
(_tmpC00=_region_malloc(yyr,sizeof(*_tmpC00)),((_tmpC00->hd=(void*)((_tmpBFC=_region_malloc(yyr,sizeof(*_tmpBFC)),((_tmpBFC[0]=((_tmpBFF.tag=3,((_tmpBFF.f1=(void*)((_tmpBFD=_region_malloc(yyr,sizeof(*_tmpBFD)),((_tmpBFD[0]=((_tmpBFE.tag=1,((_tmpBFE.f1=0,((_tmpBFE.f2=0,((_tmpBFE.f3=0,((_tmpBFE.f4=
# 2008
Cyc_yyget_YY49(yyyyvsp[2]),((_tmpBFE.f5=Cyc_yyget_YY50(yyyyvsp[3]),((_tmpBFE.f6=Cyc_yyget_YY55(yyyyvsp[5]),((_tmpBFE.f7=Cyc_yyget_YY55(yyyyvsp[6]),_tmpBFE)))))))))))))))),_tmpBFD)))),_tmpBFF)))),_tmpBFC)))),((_tmpC00->tl=(
Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpC00)))))),_tmpBFB)))));}
# 2011
break;}case 175: _LL2E7: {
# 2013
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2015
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2012 "parse.y"
{struct Cyc_List_List*_tmpC15;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC14;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpC13;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpC12;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC11;struct Cyc_Parse_Declarator _tmpC10;yyval=Cyc_YY27(((_tmpC10.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpC10.tms=((_tmpC15=_region_malloc(yyr,sizeof(*_tmpC15)),((_tmpC15->hd=(void*)((_tmpC11=_region_malloc(yyr,sizeof(*_tmpC11)),((_tmpC11[0]=((_tmpC14.tag=3,((_tmpC14.f1=(void*)((_tmpC12=_region_malloc(yyr,sizeof(*_tmpC12)),((_tmpC12[0]=((_tmpC13.tag=0,((_tmpC13.f1=Cyc_yyget_YY36(yyyyvsp[2]),((_tmpC13.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpC13)))))),_tmpC12)))),_tmpC14)))),_tmpC11)))),((_tmpC15->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpC15)))))),_tmpC10)))));}
break;}case 176: _LL2E8: {
# 2015
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2017
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2015 "parse.y"
struct Cyc_List_List*_tmp60C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));
{struct Cyc_List_List*_tmpC1F;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpC1E;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpC1D;struct Cyc_Parse_Declarator _tmpC1C;yyval=Cyc_YY27(((_tmpC1C.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpC1C.tms=((_tmpC1F=_region_malloc(yyr,sizeof(*_tmpC1F)),((_tmpC1F->hd=(void*)((_tmpC1D=_region_malloc(yyr,sizeof(*_tmpC1D)),((_tmpC1D[0]=((_tmpC1E.tag=4,((_tmpC1E.f1=_tmp60C,((_tmpC1E.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpC1E.f3=0,_tmpC1E)))))))),_tmpC1D)))),((_tmpC1F->tl=(Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpC1F)))))),_tmpC1C)))));}
# 2018
break;}case 177: _LL2E9: {
# 2020
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2022
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2019 "parse.y"
{struct Cyc_List_List*_tmpC29;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC28;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC27;struct Cyc_Parse_Declarator _tmpC26;yyval=Cyc_YY27(((_tmpC26.id=(Cyc_yyget_YY27(yyyyvsp[0])).id,((_tmpC26.tms=((_tmpC29=_region_malloc(yyr,sizeof(*_tmpC29)),((_tmpC29->hd=(void*)((_tmpC27=_region_malloc(yyr,sizeof(*_tmpC27)),((_tmpC27[0]=((_tmpC28.tag=5,((_tmpC28.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpC28.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpC28)))))),_tmpC27)))),((_tmpC29->tl=(
Cyc_yyget_YY27(yyyyvsp[0])).tms,_tmpC29)))))),_tmpC26)))));}
# 2022
break;}case 178: _LL2EA: {
# 2024
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2026
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2033 "parse.y"
{struct Cyc_Parse_Declarator _tmpC2A;yyval=Cyc_YY27(((_tmpC2A.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpC2A.tms=0,_tmpC2A)))));}
break;}case 179: _LL2EB: {
# 2036
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2038
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2035 "parse.y"
{struct Cyc_Parse_Declarator _tmpC2B;yyval=Cyc_YY27(((_tmpC2B.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpC2B.tms=0,_tmpC2B)))));}
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
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC31;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC30;struct Cyc_List_List*_tmpC2F;_tmp617.tms=((_tmpC2F=_region_malloc(yyr,sizeof(*_tmpC2F)),((_tmpC2F->hd=(void*)((_tmpC31=_region_malloc(yyr,sizeof(*_tmpC31)),((_tmpC31[0]=((_tmpC30.tag=5,((_tmpC30.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpC30.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpC30)))))),_tmpC31)))),((_tmpC2F->tl=_tmp617.tms,_tmpC2F))))));}
yyval=yyyyvsp[2];
# 2045
break;}case 182: _LL2EE: {
# 2047
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2049
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2046 "parse.y"
struct Cyc_Parse_Declarator _tmp61B=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC3B;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpC3A;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpC39;struct Cyc_Parse_Declarator _tmpC38;yyval=Cyc_YY27(((_tmpC38.id=_tmp61B.id,((_tmpC38.tms=(
(_tmpC3B=_region_malloc(yyr,sizeof(*_tmpC3B)),((_tmpC3B->hd=(void*)((_tmpC39=_region_malloc(yyr,sizeof(*_tmpC39)),((_tmpC39[0]=((_tmpC3A.tag=0,((_tmpC3A.f1=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpC3A.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpC3A)))))),_tmpC39)))),((_tmpC3B->tl=_tmp61B.tms,_tmpC3B)))))),_tmpC38)))));}
break;}case 183: _LL2EF: {
# 2051
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2053
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2050 "parse.y"
struct Cyc_Parse_Declarator _tmp620=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC45;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpC44;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpC43;struct Cyc_Parse_Declarator _tmpC42;yyval=Cyc_YY27(((_tmpC42.id=_tmp620.id,((_tmpC42.tms=(
(_tmpC45=_region_malloc(yyr,sizeof(*_tmpC45)),((_tmpC45->hd=(void*)((_tmpC43=_region_malloc(yyr,sizeof(*_tmpC43)),((_tmpC43[0]=((_tmpC44.tag=1,((_tmpC44.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpC44.f2=Cyc_yyget_YY51(yyyyvsp[4]),((_tmpC44.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpC44)))))))),_tmpC43)))),((_tmpC45->tl=_tmp620.tms,_tmpC45)))))),_tmpC42)))));}
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
{struct Cyc_List_List*_tmpC5A;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC59;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpC58;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpC57;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC56;struct Cyc_Parse_Declarator _tmpC55;yyval=Cyc_YY27(((_tmpC55.id=_tmp62E.id,((_tmpC55.tms=((_tmpC5A=_region_malloc(yyr,sizeof(*_tmpC5A)),((_tmpC5A->hd=(void*)((_tmpC56=_region_malloc(yyr,sizeof(*_tmpC56)),((_tmpC56[0]=((_tmpC59.tag=3,((_tmpC59.f1=(void*)((_tmpC57=_region_malloc(yyr,sizeof(*_tmpC57)),((_tmpC57[0]=((_tmpC58.tag=1,((_tmpC58.f1=_tmp627,((_tmpC58.f2=_tmp628,((_tmpC58.f3=_tmp629,((_tmpC58.f4=_tmp62A,((_tmpC58.f5=_tmp62B,((_tmpC58.f6=_tmp62C,((_tmpC58.f7=_tmp62D,_tmpC58)))))))))))))))),_tmpC57)))),_tmpC59)))),_tmpC56)))),((_tmpC5A->tl=_tmp62E.tms,_tmpC5A)))))),_tmpC55)))));}
# 2061
break;};}case 185: _LL2F1: {
# 2063
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2065
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2062 "parse.y"
struct Cyc_Parse_Declarator _tmp635=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC6F;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC6E;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpC6D;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpC6C;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC6B;struct Cyc_Parse_Declarator _tmpC6A;yyval=Cyc_YY27(((_tmpC6A.id=_tmp635.id,((_tmpC6A.tms=(
(_tmpC6F=_region_malloc(yyr,sizeof(*_tmpC6F)),((_tmpC6F->hd=(void*)((_tmpC6B=_region_malloc(yyr,sizeof(*_tmpC6B)),((_tmpC6B[0]=((_tmpC6E.tag=3,((_tmpC6E.f1=(void*)((_tmpC6C=_region_malloc(yyr,sizeof(*_tmpC6C)),((_tmpC6C[0]=((_tmpC6D.tag=1,((_tmpC6D.f1=0,((_tmpC6D.f2=0,((_tmpC6D.f3=0,((_tmpC6D.f4=
# 2066
Cyc_yyget_YY49(yyyyvsp[2]),((_tmpC6D.f5=Cyc_yyget_YY50(yyyyvsp[3]),((_tmpC6D.f6=Cyc_yyget_YY55(yyyyvsp[5]),((_tmpC6D.f7=Cyc_yyget_YY55(yyyyvsp[6]),_tmpC6D)))))))))))))))),_tmpC6C)))),_tmpC6E)))),_tmpC6B)))),((_tmpC6F->tl=_tmp635.tms,_tmpC6F)))))),_tmpC6A)))));}
# 2069
break;}case 186: _LL2F2: {
# 2071
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2073
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2070 "parse.y"
struct Cyc_Parse_Declarator _tmp63C=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC84;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC83;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpC82;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpC81;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC80;struct Cyc_Parse_Declarator _tmpC7F;yyval=Cyc_YY27(((_tmpC7F.id=_tmp63C.id,((_tmpC7F.tms=((_tmpC84=_region_malloc(yyr,sizeof(*_tmpC84)),((_tmpC84->hd=(void*)((_tmpC80=_region_malloc(yyr,sizeof(*_tmpC80)),((_tmpC80[0]=((_tmpC83.tag=3,((_tmpC83.f1=(void*)((_tmpC81=_region_malloc(yyr,sizeof(*_tmpC81)),((_tmpC81[0]=((_tmpC82.tag=0,((_tmpC82.f1=Cyc_yyget_YY36(yyyyvsp[2]),((_tmpC82.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpC82)))))),_tmpC81)))),_tmpC83)))),_tmpC80)))),((_tmpC84->tl=_tmp63C.tms,_tmpC84)))))),_tmpC7F)))));}
break;}case 187: _LL2F3: {
# 2074
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2076
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2074 "parse.y"
struct Cyc_List_List*_tmp643=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));
struct Cyc_Parse_Declarator _tmp644=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC8E;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpC8D;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpC8C;struct Cyc_Parse_Declarator _tmpC8B;yyval=Cyc_YY27(((_tmpC8B.id=_tmp644.id,((_tmpC8B.tms=((_tmpC8E=_region_malloc(yyr,sizeof(*_tmpC8E)),((_tmpC8E->hd=(void*)((_tmpC8C=_region_malloc(yyr,sizeof(*_tmpC8C)),((_tmpC8C[0]=((_tmpC8D.tag=4,((_tmpC8D.f1=_tmp643,((_tmpC8D.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpC8D.f3=0,_tmpC8D)))))))),_tmpC8C)))),((_tmpC8E->tl=_tmp644.tms,_tmpC8E)))))),_tmpC8B)))));}
# 2078
break;}case 188: _LL2F4: {
# 2080
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2082
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2079 "parse.y"
struct Cyc_Parse_Declarator _tmp649=Cyc_yyget_YY27(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC98;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC97;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC96;struct Cyc_Parse_Declarator _tmpC95;yyval=Cyc_YY27(((_tmpC95.id=_tmp649.id,((_tmpC95.tms=((_tmpC98=_region_malloc(yyr,sizeof(*_tmpC98)),((_tmpC98->hd=(void*)((_tmpC96=_region_malloc(yyr,sizeof(*_tmpC96)),((_tmpC96[0]=((_tmpC97.tag=5,((_tmpC97.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpC97.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpC97)))))),_tmpC96)))),((_tmpC98->tl=_tmp649.tms,_tmpC98)))))),_tmpC95)))));}
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
struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC9E;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC9D;struct Cyc_List_List*_tmpC9C;ans=((_tmpC9C=_region_malloc(yyr,sizeof(*_tmpC9C)),((_tmpC9C->hd=(void*)((_tmpC9E=_region_malloc(yyr,sizeof(*_tmpC9E)),((_tmpC9E[0]=((_tmpC9D.tag=5,((_tmpC9D.f1=(unsigned int)(yyyylsp[3]).first_line,((_tmpC9D.f2=Cyc_yyget_YY45(yyyyvsp[3]),_tmpC9D)))))),_tmpC9E)))),((_tmpC9C->tl=ans,_tmpC9C))))));}{
# 2101
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple21 _tmp651=*Cyc_yyget_YY1(yyyyvsp[0]);unsigned int _tmp653;union Cyc_Absyn_Constraint*_tmp654;union Cyc_Absyn_Constraint*_tmp655;struct _tuple21 _tmp652=_tmp651;_tmp653=_tmp652.f1;_tmp654=_tmp652.f2;_tmp655=_tmp652.f3;
if(Cyc_Absyn_porting_c_code){
struct Cyc_Absyn_PtrLoc*_tmpC9F;ptrloc=((_tmpC9F=_cycalloc_atomic(sizeof(*_tmpC9F)),((_tmpC9F->ptr_loc=_tmp653,((_tmpC9F->rgn_loc=(unsigned int)(yyyylsp[2]).first_line,((_tmpC9F->zt_loc=(unsigned int)(yyyylsp[1]).first_line,_tmpC9F))))))));}{
# 2106
struct _tuple14 _tmp657=Cyc_Parse_collapse_pointer_quals(_tmp653,_tmp654,_tmp655,Cyc_yyget_YY44(yyyyvsp[2]),Cyc_yyget_YY54(yyyyvsp[1]));union Cyc_Absyn_Constraint*_tmp659;union Cyc_Absyn_Constraint*_tmp65A;union Cyc_Absyn_Constraint*_tmp65B;void*_tmp65C;struct _tuple14 _tmp658=_tmp657;_tmp659=_tmp658.f1;_tmp65A=_tmp658.f2;_tmp65B=_tmp658.f3;_tmp65C=_tmp658.f4;
{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpCA9;struct Cyc_Absyn_PtrAtts _tmpCA8;struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmpCA7;struct Cyc_List_List*_tmpCA6;ans=((_tmpCA6=_region_malloc(yyr,sizeof(*_tmpCA6)),((_tmpCA6->hd=(void*)((_tmpCA9=_region_malloc(yyr,sizeof(*_tmpCA9)),((_tmpCA9[0]=((_tmpCA7.tag=2,((_tmpCA7.f1=((_tmpCA8.rgn=_tmp65C,((_tmpCA8.nullable=_tmp659,((_tmpCA8.bounds=_tmp65A,((_tmpCA8.zero_term=_tmp65B,((_tmpCA8.ptrloc=ptrloc,_tmpCA8)))))))))),((_tmpCA7.f2=Cyc_yyget_YY23(yyyyvsp[4]),_tmpCA7)))))),_tmpCA9)))),((_tmpCA6->tl=ans,_tmpCA6))))));}
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
{struct Cyc_List_List*_tmpCAA;yyval=Cyc_YY54(((_tmpCAA=_region_malloc(yyr,sizeof(*_tmpCAA)),((_tmpCAA->hd=Cyc_yyget_YY53(yyyyvsp[0]),((_tmpCAA->tl=Cyc_yyget_YY54(yyyyvsp[1]),_tmpCAA)))))));}
break;}case 194: _LL2FA: {
# 2116
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2118
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2118 "parse.y"
{struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct _tmpCAD;struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct*_tmpCAC;yyval=Cyc_YY53((void*)((_tmpCAC=_region_malloc(yyr,sizeof(*_tmpCAC)),((_tmpCAC[0]=((_tmpCAD.tag=0,((_tmpCAD.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),_tmpCAD)))),_tmpCAC)))));}
break;}case 195: _LL2FB: {
# 2121
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2123
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2120 "parse.y"
{struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct _tmpCB0;struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmpCAF;yyval=Cyc_YY53((void*)((_tmpCAF=_region_malloc(yyr,sizeof(*_tmpCAF)),((_tmpCAF[0]=((_tmpCB0.tag=1,((_tmpCB0.f1=Cyc_yyget_YY44(yyyyvsp[2]),_tmpCB0)))),_tmpCAF)))));}
break;}case 196: _LL2FC: {
# 2123
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2125
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2122 "parse.y"
{struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct _tmpCB3;struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct*_tmpCB2;yyval=Cyc_YY53((void*)((_tmpCB2=_region_malloc(yyr,sizeof(*_tmpCB2)),((_tmpCB2[0]=((_tmpCB3.tag=2,_tmpCB3)),_tmpCB2)))));}
break;}case 197: _LL2FD: {
# 2125
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2127
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2124 "parse.y"
{struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct _tmpCB6;struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct*_tmpCB5;yyval=Cyc_YY53((void*)((_tmpCB5=_region_malloc(yyr,sizeof(*_tmpCB5)),((_tmpCB5[0]=((_tmpCB6.tag=3,_tmpCB6)),_tmpCB5)))));}
break;}case 198: _LL2FE: {
# 2127
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2129
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2126 "parse.y"
{struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpCB9;struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpCB8;yyval=Cyc_YY53((void*)((_tmpCB8=_region_malloc(yyr,sizeof(*_tmpCB8)),((_tmpCB8[0]=((_tmpCB9.tag=4,_tmpCB9)),_tmpCB8)))));}
break;}case 199: _LL2FF: {
# 2129
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2131
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2128 "parse.y"
{struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpCBC;struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpCBB;yyval=Cyc_YY53((void*)((_tmpCBB=_region_malloc(yyr,sizeof(*_tmpCBB)),((_tmpCBB[0]=((_tmpCBC.tag=5,_tmpCBC)),_tmpCBB)))));}
break;}case 200: _LL300: {
# 2131
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2133
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2130 "parse.y"
{struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct _tmpCBF;struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct*_tmpCBE;yyval=Cyc_YY53((void*)((_tmpCBE=_region_malloc(yyr,sizeof(*_tmpCBE)),((_tmpCBE[0]=((_tmpCBF.tag=6,_tmpCBF)),_tmpCBE)))));}
break;}case 201: _LL301: {
# 2133
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2135
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2132 "parse.y"
{struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct _tmpCC2;struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct*_tmpCC1;yyval=Cyc_YY53((void*)((_tmpCC1=_region_malloc(yyr,sizeof(*_tmpCC1)),((_tmpCC1[0]=((_tmpCC2.tag=7,_tmpCC2)),_tmpCC1)))));}
break;}case 202: _LL302: {
# 2135
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2137
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2138 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpCC3;yyval=Cyc_YY1(((_tmpCC3=_cycalloc(sizeof(*_tmpCC3)),((_tmpCC3->f1=loc,((_tmpCC3->f2=Cyc_Absyn_true_conref,((_tmpCC3->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpCC3)))))))));}
# 2141
break;}case 203: _LL303: {
# 2143
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2145
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2142 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpCC4;yyval=Cyc_YY1(((_tmpCC4=_cycalloc(sizeof(*_tmpCC4)),((_tmpCC4->f1=loc,((_tmpCC4->f2=Cyc_Absyn_false_conref,((_tmpCC4->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpCC4)))))))));}
# 2145
break;}case 204: _LL304: {
# 2147
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2149
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2146 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpCC5;yyval=Cyc_YY1(((_tmpCC5=_cycalloc(sizeof(*_tmpCC5)),((_tmpCC5->f1=loc,((_tmpCC5->f2=Cyc_Absyn_true_conref,((_tmpCC5->f3=Cyc_Absyn_bounds_dyneither_conref,_tmpCC5)))))))));}
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
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpCC8;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpCC7;yyval=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmpCC7=_cycalloc(sizeof(*_tmpCC7)),((_tmpCC7[0]=((_tmpCC8.tag=1,((_tmpCC8.f1=Cyc_yyget_Exp_tok(yyyyvsp[1]),_tmpCC8)))),_tmpCC7))))));}
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
{struct _tuple26*_tmpCC9;yyval=Cyc_YY39(((_tmpCC9=_cycalloc(sizeof(*_tmpCC9)),((_tmpCC9->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0])),((_tmpCC9->f2=0,((_tmpCC9->f3=0,((_tmpCC9->f4=Cyc_yyget_YY49(yyyyvsp[1]),((_tmpCC9->f5=Cyc_yyget_YY50(yyyyvsp[2]),_tmpCC9)))))))))))));}
break;}case 216: _LL310: {
# 2177
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2179
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2176 "parse.y"
{struct _tuple26*_tmpCCA;yyval=Cyc_YY39(((_tmpCCA=_cycalloc(sizeof(*_tmpCCA)),((_tmpCCA->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0])),((_tmpCCA->f2=1,((_tmpCCA->f3=0,((_tmpCCA->f4=Cyc_yyget_YY49(yyyyvsp[3]),((_tmpCCA->f5=Cyc_yyget_YY50(yyyyvsp[4]),_tmpCCA)))))))))))));}
break;}case 217: _LL311: {
# 2179
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2181
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2179 "parse.y"
struct _tuple8*_tmp679=Cyc_yyget_YY37(yyyyvsp[2]);struct _dyneither_ptr*_tmp67B;struct Cyc_Absyn_Tqual _tmp67C;void*_tmp67D;struct _tuple8*_tmp67A=_tmp679;_tmp67B=_tmp67A->f1;_tmp67C=_tmp67A->f2;_tmp67D=_tmp67A->f3;{
struct Cyc_Absyn_VarargInfo*_tmpCCB;struct Cyc_Absyn_VarargInfo*_tmp67E=(_tmpCCB=_cycalloc(sizeof(*_tmpCCB)),((_tmpCCB->name=_tmp67B,((_tmpCCB->tq=_tmp67C,((_tmpCCB->type=_tmp67D,((_tmpCCB->inject=Cyc_yyget_YY31(yyyyvsp[1]),_tmpCCB)))))))));
{struct _tuple26*_tmpCCC;yyval=Cyc_YY39(((_tmpCCC=_cycalloc(sizeof(*_tmpCCC)),((_tmpCCC->f1=0,((_tmpCCC->f2=0,((_tmpCCC->f3=_tmp67E,((_tmpCCC->f4=Cyc_yyget_YY49(yyyyvsp[3]),((_tmpCCC->f5=Cyc_yyget_YY50(yyyyvsp[4]),_tmpCCC)))))))))))));}
# 2183
break;};}case 218: _LL312: {
# 2185
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2187
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2185 "parse.y"
struct _tuple8*_tmp681=Cyc_yyget_YY37(yyyyvsp[4]);struct _dyneither_ptr*_tmp683;struct Cyc_Absyn_Tqual _tmp684;void*_tmp685;struct _tuple8*_tmp682=_tmp681;_tmp683=_tmp682->f1;_tmp684=_tmp682->f2;_tmp685=_tmp682->f3;{
struct Cyc_Absyn_VarargInfo*_tmpCCD;struct Cyc_Absyn_VarargInfo*_tmp686=(_tmpCCD=_cycalloc(sizeof(*_tmpCCD)),((_tmpCCD->name=_tmp683,((_tmpCCD->tq=_tmp684,((_tmpCCD->type=_tmp685,((_tmpCCD->inject=Cyc_yyget_YY31(yyyyvsp[3]),_tmpCCD)))))))));
{struct _tuple26*_tmpCCE;yyval=Cyc_YY39(((_tmpCCE=_cycalloc(sizeof(*_tmpCCE)),((_tmpCCE->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0])),((_tmpCCE->f2=0,((_tmpCCE->f3=_tmp686,((_tmpCCE->f4=Cyc_yyget_YY49(yyyyvsp[5]),((_tmpCCE->f5=Cyc_yyget_YY50(yyyyvsp[6]),_tmpCCE)))))))))))));}
# 2189
break;};}case 219: _LL313: {
# 2191
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2193
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2193 "parse.y"
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpCD1;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpCD0;yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),(void*)((_tmpCD0=_cycalloc(sizeof(*_tmpCD0)),((_tmpCD0[0]=((_tmpCD1.tag=1,((_tmpCD1.f1=0,_tmpCD1)))),_tmpCD0))))));}
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
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCD4;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCD3;yyval=Cyc_YY49((void*)((_tmpCD3=_cycalloc(sizeof(*_tmpCD3)),((_tmpCD3[0]=((_tmpCD4.tag=24,((_tmpCD4.f1=Cyc_yyget_YY40(yyyyvsp[1]),_tmpCD4)))),_tmpCD3)))));}
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
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpCD7;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCD6;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp68D=(_tmpCD6=_cycalloc(sizeof(*_tmpCD6)),((_tmpCD6[0]=((_tmpCD7.tag=2,((_tmpCD7.f1=0,((_tmpCD7.f2=& Cyc_Tcutil_trk,_tmpCD7)))))),_tmpCD6)));
struct _dyneither_ptr _tmp68E=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp68F=Cyc_Parse_id2type(_tmp68E,(void*)_tmp68D);
{struct _tuple29*_tmpCE1;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCE0;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCDF;struct Cyc_List_List*_tmpCDE;yyval=Cyc_YY50(((_tmpCDE=_cycalloc(sizeof(*_tmpCDE)),((_tmpCDE->hd=((_tmpCE1=_cycalloc(sizeof(*_tmpCE1)),((_tmpCE1->f1=(void*)((_tmpCDF=_cycalloc(sizeof(*_tmpCDF)),((_tmpCDF[0]=((_tmpCE0.tag=24,((_tmpCE0.f1=Cyc_yyget_YY40(yyyyvsp[0]),_tmpCE0)))),_tmpCDF)))),((_tmpCE1->f2=_tmp68F,_tmpCE1)))))),((_tmpCDE->tl=0,_tmpCDE)))))));}
# 2216
break;}case 226: _LL31A: {
# 2218
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2220
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2218 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpCE4;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCE3;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp696=(_tmpCE3=_cycalloc(sizeof(*_tmpCE3)),((_tmpCE3[0]=((_tmpCE4.tag=2,((_tmpCE4.f1=0,((_tmpCE4.f2=& Cyc_Tcutil_trk,_tmpCE4)))))),_tmpCE3)));
struct _dyneither_ptr _tmp697=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp698=Cyc_Parse_id2type(_tmp697,(void*)_tmp696);
{struct _tuple29*_tmpCEE;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCED;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCEC;struct Cyc_List_List*_tmpCEB;yyval=Cyc_YY50(((_tmpCEB=_cycalloc(sizeof(*_tmpCEB)),((_tmpCEB->hd=((_tmpCEE=_cycalloc(sizeof(*_tmpCEE)),((_tmpCEE->f1=(void*)((_tmpCEC=_cycalloc(sizeof(*_tmpCEC)),((_tmpCEC[0]=((_tmpCED.tag=24,((_tmpCED.f1=Cyc_yyget_YY40(yyyyvsp[0]),_tmpCED)))),_tmpCEC)))),((_tmpCEE->f2=_tmp698,_tmpCEE)))))),((_tmpCEB->tl=Cyc_yyget_YY50(yyyyvsp[4]),_tmpCEB)))))));}
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
{const char*_tmpCEF;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]),((_tmpCEF="inject",_tag_dyneither(_tmpCEF,sizeof(char),7))))!= 0){
const char*_tmpCF0;Cyc_Parse_err(((_tmpCF0="missing type in function declaration",_tag_dyneither(_tmpCF0,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
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
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpCF6;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpCF5;struct Cyc_List_List*_tmpCF4;yyval=Cyc_YY40(((_tmpCF4=_cycalloc(sizeof(*_tmpCF4)),((_tmpCF4->hd=(void*)((_tmpCF6=_cycalloc(sizeof(*_tmpCF6)),((_tmpCF6[0]=((_tmpCF5.tag=25,((_tmpCF5.f1=Cyc_yyget_YY44(yyyyvsp[2]),_tmpCF5)))),_tmpCF6)))),((_tmpCF4->tl=0,_tmpCF4)))))));}
break;}case 234: _LL322: {
# 2247
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2249
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2246 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_ek,0);
{struct Cyc_List_List*_tmpCF7;yyval=Cyc_YY40(((_tmpCF7=_cycalloc(sizeof(*_tmpCF7)),((_tmpCF7->hd=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpCF7->tl=0,_tmpCF7)))))));}
# 2249
break;}case 235: _LL323: {
# 2251
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2253
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2266 "parse.y"
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpCFD;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpCFC;struct Cyc_List_List*_tmpCFB;yyval=Cyc_YY40(((_tmpCFB=_cycalloc(sizeof(*_tmpCFB)),((_tmpCFB->hd=(void*)((_tmpCFD=_cycalloc(sizeof(*_tmpCFD)),((_tmpCFD[0]=((_tmpCFC.tag=23,((_tmpCFC.f1=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line),_tmpCFC)))),_tmpCFD)))),((_tmpCFB->tl=0,_tmpCFB)))))));}
break;}case 236: _LL324: {
# 2269
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2271
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2268 "parse.y"
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpD03;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpD02;struct Cyc_List_List*_tmpD01;yyval=Cyc_YY40(((_tmpD01=_cycalloc(sizeof(*_tmpD01)),((_tmpD01->hd=(void*)((_tmpD03=_cycalloc(sizeof(*_tmpD03)),((_tmpD03[0]=((_tmpD02.tag=23,((_tmpD02.f1=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line),_tmpD02)))),_tmpD03)))),((_tmpD01->tl=Cyc_yyget_YY40(yyyyvsp[2]),_tmpD01)))))));}
break;}case 237: _LL325: {
# 2271
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2273
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpD04;yyval=Cyc_YY38(((_tmpD04=_cycalloc(sizeof(*_tmpD04)),((_tmpD04->hd=Cyc_yyget_YY37(yyyyvsp[0]),((_tmpD04->tl=0,_tmpD04)))))));}
break;}case 238: _LL326: {
# 2277
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2279
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2276 "parse.y"
{struct Cyc_List_List*_tmpD05;yyval=Cyc_YY38(((_tmpD05=_cycalloc(sizeof(*_tmpD05)),((_tmpD05->hd=Cyc_yyget_YY37(yyyyvsp[2]),((_tmpD05->tl=Cyc_yyget_YY38(yyyyvsp[0]),_tmpD05)))))));}
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
const char*_tmpD06;Cyc_Parse_err(((_tmpD06="parameter with bad type params",_tag_dyneither(_tmpD06,sizeof(char),31))),(unsigned int)(yyyylsp[1]).first_line);}
if(Cyc_Absyn_is_qvar_qualified(_tmp6B4)){
const char*_tmpD07;Cyc_Parse_err(((_tmpD07="parameter cannot be qualified with a namespace",_tag_dyneither(_tmpD07,sizeof(char),47))),(unsigned int)(yyyylsp[0]).first_line);}{
struct _dyneither_ptr*idopt=(*_tmp6B4).f2;
if(_tmp6BC != 0){
const char*_tmpD0A;void*_tmpD09;(_tmpD09=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpD0A="extra attributes on parameter, ignoring",_tag_dyneither(_tmpD0A,sizeof(char),40))),_tag_dyneither(_tmpD09,sizeof(void*),0)));}
{struct _tuple8*_tmpD0B;yyval=Cyc_YY37(((_tmpD0B=_cycalloc(sizeof(*_tmpD0B)),((_tmpD0B->f1=idopt,((_tmpD0B->f2=_tmp6B9,((_tmpD0B->f3=_tmp6BA,_tmpD0B)))))))));}
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
const char*_tmpD0E;void*_tmpD0D;(_tmpD0D=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpD0E="bad attributes on parameter, ignoring",_tag_dyneither(_tmpD0E,sizeof(char),38))),_tag_dyneither(_tmpD0D,sizeof(void*),0)));}
{struct _tuple8*_tmpD0F;yyval=Cyc_YY37(((_tmpD0F=_cycalloc(sizeof(*_tmpD0F)),((_tmpD0F->f1=0,((_tmpD0F->f2=_tmp6C4,((_tmpD0F->f3=_tmp6C7,_tmpD0F)))))))));}
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
const char*_tmpD12;void*_tmpD11;(_tmpD11=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpD12="bad type parameters on formal argument, ignoring",_tag_dyneither(_tmpD12,sizeof(char),49))),_tag_dyneither(_tmpD11,sizeof(void*),0)));}
# 2313
if(_tmp6D7 != 0){
const char*_tmpD15;void*_tmpD14;(_tmpD14=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpD15="bad attributes on parameter, ignoring",_tag_dyneither(_tmpD15,sizeof(char),38))),_tag_dyneither(_tmpD14,sizeof(void*),0)));}
{struct _tuple8*_tmpD16;yyval=Cyc_YY37(((_tmpD16=_cycalloc(sizeof(*_tmpD16)),((_tmpD16->f1=0,((_tmpD16->f2=_tmp6D4,((_tmpD16->f3=_tmp6D5,_tmpD16)))))))));}
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
{struct _dyneither_ptr*_tmpD19;struct Cyc_List_List*_tmpD18;yyval=Cyc_YY36(((_tmpD18=_cycalloc(sizeof(*_tmpD18)),((_tmpD18->hd=((_tmpD19=_cycalloc(sizeof(*_tmpD19)),((_tmpD19[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpD19)))),((_tmpD18->tl=0,_tmpD18)))))));}
break;}case 244: _LL32C: {
# 2328
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2330
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2327 "parse.y"
{struct _dyneither_ptr*_tmpD1C;struct Cyc_List_List*_tmpD1B;yyval=Cyc_YY36(((_tmpD1B=_cycalloc(sizeof(*_tmpD1B)),((_tmpD1B->hd=((_tmpD1C=_cycalloc(sizeof(*_tmpD1C)),((_tmpD1C[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpD1C)))),((_tmpD1B->tl=Cyc_yyget_YY36(yyyyvsp[0]),_tmpD1B)))))));}
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
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpD1F;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpD1E;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpD1E=_cycalloc(sizeof(*_tmpD1E)),((_tmpD1E[0]=((_tmpD1F.tag=35,((_tmpD1F.f1=0,((_tmpD1F.f2=0,_tmpD1F)))))),_tmpD1E)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 248: _LL330: {
# 2340
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2342
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2339 "parse.y"
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpD22;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpD21;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpD21=_cycalloc(sizeof(*_tmpD21)),((_tmpD21[0]=((_tmpD22.tag=35,((_tmpD22.f1=0,((_tmpD22.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[1])),_tmpD22)))))),_tmpD21)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 249: _LL331: {
# 2342
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2344
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2341 "parse.y"
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpD25;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpD24;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpD24=_cycalloc(sizeof(*_tmpD24)),((_tmpD24[0]=((_tmpD25.tag=35,((_tmpD25.f1=0,((_tmpD25.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[1])),_tmpD25)))))),_tmpD24)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 250: _LL332: {
# 2344
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2346
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2343 "parse.y"
struct _dyneither_ptr*_tmpD28;struct _tuple0*_tmpD27;struct Cyc_Absyn_Vardecl*_tmp6E7=Cyc_Absyn_new_vardecl(((_tmpD27=_cycalloc(sizeof(*_tmpD27)),((_tmpD27->f1=Cyc_Absyn_Loc_n,((_tmpD27->f2=((_tmpD28=_cycalloc(sizeof(*_tmpD28)),((_tmpD28[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpD28)))),_tmpD27)))))),Cyc_Absyn_uint_typ,
Cyc_Absyn_uint_exp(0,(unsigned int)(yyyylsp[2]).first_line));
# 2346
(_tmp6E7->tq).real_const=1;
{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpD2B;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpD2A;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpD2A=_cycalloc(sizeof(*_tmpD2A)),((_tmpD2A[0]=((_tmpD2B.tag=26,((_tmpD2B.f1=_tmp6E7,((_tmpD2B.f2=Cyc_yyget_Exp_tok(yyyyvsp[4]),((_tmpD2B.f3=Cyc_yyget_Exp_tok(yyyyvsp[6]),((_tmpD2B.f4=0,_tmpD2B)))))))))),_tmpD2A)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2349
break;}case 251: _LL333: {
# 2351
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2353
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2350 "parse.y"
void*_tmp6EC=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpD2E;struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpD2D;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpD2D=_cycalloc(sizeof(*_tmpD2D)),((_tmpD2D[0]=((_tmpD2E.tag=27,((_tmpD2E.f1=Cyc_yyget_Exp_tok(yyyyvsp[4]),((_tmpD2E.f2=_tmp6EC,((_tmpD2E.f3=0,_tmpD2E)))))))),_tmpD2D)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2353
break;}case 252: _LL334: {
# 2355
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2357
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct _tuple30*_tmpD31;struct Cyc_List_List*_tmpD30;yyval=Cyc_YY5(((_tmpD30=_cycalloc(sizeof(*_tmpD30)),((_tmpD30->hd=((_tmpD31=_cycalloc(sizeof(*_tmpD31)),((_tmpD31->f1=0,((_tmpD31->f2=Cyc_yyget_Exp_tok(yyyyvsp[0]),_tmpD31)))))),((_tmpD30->tl=0,_tmpD30)))))));}
break;}case 253: _LL335: {
# 2361
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2363
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2360 "parse.y"
{struct _tuple30*_tmpD34;struct Cyc_List_List*_tmpD33;yyval=Cyc_YY5(((_tmpD33=_cycalloc(sizeof(*_tmpD33)),((_tmpD33->hd=((_tmpD34=_cycalloc(sizeof(*_tmpD34)),((_tmpD34->f1=Cyc_yyget_YY41(yyyyvsp[0]),((_tmpD34->f2=Cyc_yyget_Exp_tok(yyyyvsp[1]),_tmpD34)))))),((_tmpD33->tl=0,_tmpD33)))))));}
break;}case 254: _LL336: {
# 2363
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2365
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2362 "parse.y"
{struct _tuple30*_tmpD37;struct Cyc_List_List*_tmpD36;yyval=Cyc_YY5(((_tmpD36=_cycalloc(sizeof(*_tmpD36)),((_tmpD36->hd=((_tmpD37=_cycalloc(sizeof(*_tmpD37)),((_tmpD37->f1=0,((_tmpD37->f2=Cyc_yyget_Exp_tok(yyyyvsp[2]),_tmpD37)))))),((_tmpD36->tl=Cyc_yyget_YY5(yyyyvsp[0]),_tmpD36)))))));}
break;}case 255: _LL337: {
# 2365
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2367
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2364 "parse.y"
{struct _tuple30*_tmpD3A;struct Cyc_List_List*_tmpD39;yyval=Cyc_YY5(((_tmpD39=_cycalloc(sizeof(*_tmpD39)),((_tmpD39->hd=((_tmpD3A=_cycalloc(sizeof(*_tmpD3A)),((_tmpD3A->f1=Cyc_yyget_YY41(yyyyvsp[2]),((_tmpD3A->f2=Cyc_yyget_Exp_tok(yyyyvsp[3]),_tmpD3A)))))),((_tmpD39->tl=Cyc_yyget_YY5(yyyyvsp[0]),_tmpD39)))))));}
break;}case 256: _LL338: {
# 2367
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2369
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2368 "parse.y"
yyval=Cyc_YY41(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])));
break;}case 257: _LL339: {
# 2371
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2373
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2369 "parse.y"
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpD44;struct _dyneither_ptr*_tmpD43;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpD42;struct Cyc_List_List*_tmpD41;yyval=Cyc_YY41(((_tmpD41=_cycalloc(sizeof(*_tmpD41)),((_tmpD41->hd=(void*)((_tmpD44=_cycalloc(sizeof(*_tmpD44)),((_tmpD44[0]=((_tmpD42.tag=1,((_tmpD42.f1=((_tmpD43=_cycalloc(sizeof(*_tmpD43)),((_tmpD43[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpD43)))),_tmpD42)))),_tmpD44)))),((_tmpD41->tl=0,_tmpD41)))))));}
break;}case 258: _LL33A: {
# 2372
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2374
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2374 "parse.y"
{struct Cyc_List_List*_tmpD45;yyval=Cyc_YY41(((_tmpD45=_cycalloc(sizeof(*_tmpD45)),((_tmpD45->hd=Cyc_yyget_YY42(yyyyvsp[0]),((_tmpD45->tl=0,_tmpD45)))))));}
break;}case 259: _LL33B: {
# 2377
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2379
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2375 "parse.y"
{struct Cyc_List_List*_tmpD46;yyval=Cyc_YY41(((_tmpD46=_cycalloc(sizeof(*_tmpD46)),((_tmpD46->hd=Cyc_yyget_YY42(yyyyvsp[1]),((_tmpD46->tl=Cyc_yyget_YY41(yyyyvsp[0]),_tmpD46)))))));}
break;}case 260: _LL33C: {
# 2378
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2380
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2379 "parse.y"
{struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpD49;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpD48;yyval=Cyc_YY42((void*)((_tmpD48=_cycalloc(sizeof(*_tmpD48)),((_tmpD48[0]=((_tmpD49.tag=0,((_tmpD49.f1=Cyc_yyget_Exp_tok(yyyyvsp[1]),_tmpD49)))),_tmpD48)))));}
break;}case 261: _LL33D: {
# 2382
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2384
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2380 "parse.y"
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpD4F;struct _dyneither_ptr*_tmpD4E;struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpD4D;yyval=Cyc_YY42((void*)((_tmpD4D=_cycalloc(sizeof(*_tmpD4D)),((_tmpD4D[0]=((_tmpD4F.tag=1,((_tmpD4F.f1=((_tmpD4E=_cycalloc(sizeof(*_tmpD4E)),((_tmpD4E[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpD4E)))),_tmpD4F)))),_tmpD4D)))));}
break;}case 262: _LL33E: {
# 2383
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2385
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2385 "parse.y"
struct _tuple25 _tmp702=Cyc_yyget_YY35(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp704;struct Cyc_Parse_Type_specifier _tmp705;struct Cyc_List_List*_tmp706;struct _tuple25 _tmp703=_tmp702;_tmp704=_tmp703.f1;_tmp705=_tmp703.f2;_tmp706=_tmp703.f3;{
void*_tmp707=Cyc_Parse_speclist2typ(_tmp705,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp706 != 0){
const char*_tmpD52;void*_tmpD51;(_tmpD51=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpD52="ignoring attributes in type",_tag_dyneither(_tmpD52,sizeof(char),28))),_tag_dyneither(_tmpD51,sizeof(void*),0)));}
{struct _tuple8*_tmpD53;yyval=Cyc_YY37(((_tmpD53=_cycalloc(sizeof(*_tmpD53)),((_tmpD53->f1=0,((_tmpD53->f2=_tmp704,((_tmpD53->f3=_tmp707,_tmpD53)))))))));}
# 2391
break;};}case 263: _LL33F: {
# 2393
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2395
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2392 "parse.y"
struct _tuple25 _tmp70B=Cyc_yyget_YY35(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp70D;struct Cyc_Parse_Type_specifier _tmp70E;struct Cyc_List_List*_tmp70F;struct _tuple25 _tmp70C=_tmp70B;_tmp70D=_tmp70C.f1;_tmp70E=_tmp70C.f2;_tmp70F=_tmp70C.f3;{
void*_tmp710=Cyc_Parse_speclist2typ(_tmp70E,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp711=(Cyc_yyget_YY30(yyyyvsp[1])).tms;
struct _tuple13 _tmp712=Cyc_Parse_apply_tms(_tmp70D,_tmp710,_tmp70F,_tmp711);
if(_tmp712.f3 != 0){
# 2398
const char*_tmpD56;void*_tmpD55;(_tmpD55=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpD56="bad type params, ignoring",_tag_dyneither(_tmpD56,sizeof(char),26))),_tag_dyneither(_tmpD55,sizeof(void*),0)));}
if(_tmp712.f4 != 0){
const char*_tmpD59;void*_tmpD58;(_tmpD58=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpD59="bad specifiers, ignoring",_tag_dyneither(_tmpD59,sizeof(char),25))),_tag_dyneither(_tmpD58,sizeof(void*),0)));}
{struct _tuple8*_tmpD5A;yyval=Cyc_YY37(((_tmpD5A=_cycalloc(sizeof(*_tmpD5A)),((_tmpD5A->f1=0,((_tmpD5A->f2=_tmp712.f1,((_tmpD5A->f3=_tmp712.f2,_tmpD5A)))))))));}
# 2403
break;};}case 264: _LL340: {
# 2405
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2407
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2406 "parse.y"
yyval=Cyc_YY44(Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 265: _LL341: {
# 2409
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2411
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2407 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD5D;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD5C;yyval=Cyc_YY44((void*)((_tmpD5C=_cycalloc(sizeof(*_tmpD5C)),((_tmpD5C[0]=((_tmpD5D.tag=24,((_tmpD5D.f1=0,_tmpD5D)))),_tmpD5C)))));}
break;}case 266: _LL342: {
# 2410
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2412
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2408 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD60;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD5F;yyval=Cyc_YY44((void*)((_tmpD5F=_cycalloc(sizeof(*_tmpD5F)),((_tmpD5F[0]=((_tmpD60.tag=24,((_tmpD60.f1=Cyc_yyget_YY40(yyyyvsp[1]),_tmpD60)))),_tmpD5F)))));}
break;}case 267: _LL343: {
# 2411
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2413
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2409 "parse.y"
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpD63;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpD62;yyval=Cyc_YY44((void*)((_tmpD62=_cycalloc(sizeof(*_tmpD62)),((_tmpD62[0]=((_tmpD63.tag=25,((_tmpD63.f1=Cyc_yyget_YY44(yyyyvsp[2]),_tmpD63)))),_tmpD62)))));}
break;}case 268: _LL344: {
# 2412
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2414
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2410 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD69;struct Cyc_List_List*_tmpD68;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD67;yyval=Cyc_YY44((void*)((_tmpD67=_cycalloc(sizeof(*_tmpD67)),((_tmpD67[0]=((_tmpD69.tag=24,((_tmpD69.f1=((_tmpD68=_cycalloc(sizeof(*_tmpD68)),((_tmpD68->hd=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpD68->tl=Cyc_yyget_YY40(yyyyvsp[2]),_tmpD68)))))),_tmpD69)))),_tmpD67)))));}
break;}case 269: _LL345: {
# 2413
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2415
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpD6A;yyval=Cyc_YY40(((_tmpD6A=_cycalloc(sizeof(*_tmpD6A)),((_tmpD6A->hd=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpD6A->tl=0,_tmpD6A)))))));}
break;}case 270: _LL346: {
# 2419
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2421
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2417 "parse.y"
{struct Cyc_List_List*_tmpD6B;yyval=Cyc_YY40(((_tmpD6B=_cycalloc(sizeof(*_tmpD6B)),((_tmpD6B->hd=Cyc_yyget_YY44(yyyyvsp[2]),((_tmpD6B->tl=Cyc_yyget_YY40(yyyyvsp[0]),_tmpD6B)))))));}
break;}case 271: _LL347: {
# 2420
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2422
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2422 "parse.y"
{struct Cyc_Parse_Abstractdeclarator _tmpD6C;yyval=Cyc_YY30(((_tmpD6C.tms=Cyc_yyget_YY26(yyyyvsp[0]),_tmpD6C)));}
break;}case 272: _LL348: {
# 2425
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2427
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2424 "parse.y"
yyval=yyyyvsp[0];
break;}case 273: _LL349: {
# 2427
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2429
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2426 "parse.y"
{struct Cyc_Parse_Abstractdeclarator _tmpD6D;yyval=Cyc_YY30(((_tmpD6D.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY26(yyyyvsp[0]),(Cyc_yyget_YY30(yyyyvsp[1])).tms),_tmpD6D)));}
break;}case 274: _LL34A: {
# 2429
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2431
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2431 "parse.y"
yyval=yyyyvsp[1];
break;}case 275: _LL34B: {
# 2434
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2436
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2433 "parse.y"
{struct Cyc_List_List*_tmpD77;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpD76;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpD75;struct Cyc_Parse_Abstractdeclarator _tmpD74;yyval=Cyc_YY30(((_tmpD74.tms=((_tmpD77=_region_malloc(yyr,sizeof(*_tmpD77)),((_tmpD77->hd=(void*)((_tmpD75=_region_malloc(yyr,sizeof(*_tmpD75)),((_tmpD75[0]=((_tmpD76.tag=0,((_tmpD76.f1=Cyc_yyget_YY51(yyyyvsp[2]),((_tmpD76.f2=(unsigned int)(yyyylsp[2]).first_line,_tmpD76)))))),_tmpD75)))),((_tmpD77->tl=0,_tmpD77)))))),_tmpD74)));}
break;}case 276: _LL34C: {
# 2436
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2438
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2435 "parse.y"
{struct Cyc_List_List*_tmpD81;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpD80;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpD7F;struct Cyc_Parse_Abstractdeclarator _tmpD7E;yyval=Cyc_YY30(((_tmpD7E.tms=((_tmpD81=_region_malloc(yyr,sizeof(*_tmpD81)),((_tmpD81->hd=(void*)((_tmpD7F=_region_malloc(yyr,sizeof(*_tmpD7F)),((_tmpD7F[0]=((_tmpD80.tag=0,((_tmpD80.f1=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpD80.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpD80)))))),_tmpD7F)))),((_tmpD81->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpD81)))))),_tmpD7E)));}
break;}case 277: _LL34D: {
# 2438
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2440
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2437 "parse.y"
{struct Cyc_List_List*_tmpD8B;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpD8A;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD89;struct Cyc_Parse_Abstractdeclarator _tmpD88;yyval=Cyc_YY30(((_tmpD88.tms=((_tmpD8B=_region_malloc(yyr,sizeof(*_tmpD8B)),((_tmpD8B->hd=(void*)((_tmpD89=_region_malloc(yyr,sizeof(*_tmpD89)),((_tmpD89[0]=((_tmpD8A.tag=1,((_tmpD8A.f1=Cyc_yyget_Exp_tok(yyyyvsp[1]),((_tmpD8A.f2=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpD8A.f3=(unsigned int)(yyyylsp[3]).first_line,_tmpD8A)))))))),_tmpD89)))),((_tmpD8B->tl=0,_tmpD8B)))))),_tmpD88)));}
break;}case 278: _LL34E: {
# 2440
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2442
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2439 "parse.y"
{struct Cyc_List_List*_tmpD95;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpD94;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD93;struct Cyc_Parse_Abstractdeclarator _tmpD92;yyval=Cyc_YY30(((_tmpD92.tms=((_tmpD95=_region_malloc(yyr,sizeof(*_tmpD95)),((_tmpD95->hd=(void*)((_tmpD93=_region_malloc(yyr,sizeof(*_tmpD93)),((_tmpD93[0]=((_tmpD94.tag=1,((_tmpD94.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpD94.f2=Cyc_yyget_YY51(yyyyvsp[4]),((_tmpD94.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpD94)))))))),_tmpD93)))),((_tmpD95->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpD95)))))),_tmpD92)));}
# 2442
break;}case 279: _LL34F: {
# 2444
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2446
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2443 "parse.y"
{struct Cyc_List_List*_tmpDAA;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpDA9;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpDA8;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpDA7;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpDA6;struct Cyc_Parse_Abstractdeclarator _tmpDA5;yyval=Cyc_YY30(((_tmpDA5.tms=((_tmpDAA=_region_malloc(yyr,sizeof(*_tmpDAA)),((_tmpDAA->hd=(void*)((_tmpDA6=_region_malloc(yyr,sizeof(*_tmpDA6)),((_tmpDA6[0]=((_tmpDA9.tag=3,((_tmpDA9.f1=(void*)((_tmpDA7=_region_malloc(yyr,sizeof(*_tmpDA7)),((_tmpDA7[0]=((_tmpDA8.tag=1,((_tmpDA8.f1=0,((_tmpDA8.f2=0,((_tmpDA8.f3=0,((_tmpDA8.f4=Cyc_yyget_YY49(yyyyvsp[1]),((_tmpDA8.f5=Cyc_yyget_YY50(yyyyvsp[2]),((_tmpDA8.f6=Cyc_yyget_YY55(yyyyvsp[4]),((_tmpDA8.f7=Cyc_yyget_YY55(yyyyvsp[5]),_tmpDA8)))))))))))))))),_tmpDA7)))),_tmpDA9)))),_tmpDA6)))),((_tmpDAA->tl=0,_tmpDAA)))))),_tmpDA5)));}
# 2445
break;}case 280: _LL350: {
# 2447
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2449
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2446 "parse.y"
struct _tuple26*_tmp73B=Cyc_yyget_YY39(yyyyvsp[1]);struct Cyc_List_List*_tmp73D;int _tmp73E;struct Cyc_Absyn_VarargInfo*_tmp73F;void*_tmp740;struct Cyc_List_List*_tmp741;struct _tuple26*_tmp73C=_tmp73B;_tmp73D=_tmp73C->f1;_tmp73E=_tmp73C->f2;_tmp73F=_tmp73C->f3;_tmp740=_tmp73C->f4;_tmp741=_tmp73C->f5;
{struct Cyc_List_List*_tmpDBF;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpDBE;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpDBD;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpDBC;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpDBB;struct Cyc_Parse_Abstractdeclarator _tmpDBA;yyval=Cyc_YY30(((_tmpDBA.tms=((_tmpDBF=_region_malloc(yyr,sizeof(*_tmpDBF)),((_tmpDBF->hd=(void*)((_tmpDBB=_region_malloc(yyr,sizeof(*_tmpDBB)),((_tmpDBB[0]=((_tmpDBE.tag=3,((_tmpDBE.f1=(void*)((_tmpDBC=_region_malloc(yyr,sizeof(*_tmpDBC)),((_tmpDBC[0]=((_tmpDBD.tag=1,((_tmpDBD.f1=_tmp73D,((_tmpDBD.f2=_tmp73E,((_tmpDBD.f3=_tmp73F,((_tmpDBD.f4=_tmp740,((_tmpDBD.f5=_tmp741,((_tmpDBD.f6=Cyc_yyget_YY55(yyyyvsp[3]),((_tmpDBD.f7=Cyc_yyget_YY55(yyyyvsp[4]),_tmpDBD)))))))))))))))),_tmpDBC)))),_tmpDBE)))),_tmpDBB)))),((_tmpDBF->tl=0,_tmpDBF)))))),_tmpDBA)));}
# 2449
break;}case 281: _LL351: {
# 2451
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2453
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2450 "parse.y"
{struct Cyc_List_List*_tmpDD4;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpDD3;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpDD2;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpDD1;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpDD0;struct Cyc_Parse_Abstractdeclarator _tmpDCF;yyval=Cyc_YY30(((_tmpDCF.tms=((_tmpDD4=_region_malloc(yyr,sizeof(*_tmpDD4)),((_tmpDD4->hd=(void*)((_tmpDD0=_region_malloc(yyr,sizeof(*_tmpDD0)),((_tmpDD0[0]=((_tmpDD3.tag=3,((_tmpDD3.f1=(void*)((_tmpDD1=_region_malloc(yyr,sizeof(*_tmpDD1)),((_tmpDD1[0]=((_tmpDD2.tag=1,((_tmpDD2.f1=0,((_tmpDD2.f2=0,((_tmpDD2.f3=0,((_tmpDD2.f4=Cyc_yyget_YY49(yyyyvsp[2]),((_tmpDD2.f5=Cyc_yyget_YY50(yyyyvsp[3]),((_tmpDD2.f6=Cyc_yyget_YY55(yyyyvsp[5]),((_tmpDD2.f7=Cyc_yyget_YY55(yyyyvsp[6]),_tmpDD2)))))))))))))))),_tmpDD1)))),_tmpDD3)))),_tmpDD0)))),((_tmpDD4->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpDD4)))))),_tmpDCF)));}
# 2453
break;}case 282: _LL352: {
# 2455
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2457
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2454 "parse.y"
struct _tuple26*_tmp74E=Cyc_yyget_YY39(yyyyvsp[2]);struct Cyc_List_List*_tmp750;int _tmp751;struct Cyc_Absyn_VarargInfo*_tmp752;void*_tmp753;struct Cyc_List_List*_tmp754;struct _tuple26*_tmp74F=_tmp74E;_tmp750=_tmp74F->f1;_tmp751=_tmp74F->f2;_tmp752=_tmp74F->f3;_tmp753=_tmp74F->f4;_tmp754=_tmp74F->f5;
{struct Cyc_List_List*_tmpDE9;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpDE8;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpDE7;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpDE6;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpDE5;struct Cyc_Parse_Abstractdeclarator _tmpDE4;yyval=Cyc_YY30(((_tmpDE4.tms=((_tmpDE9=_region_malloc(yyr,sizeof(*_tmpDE9)),((_tmpDE9->hd=(void*)((_tmpDE5=_region_malloc(yyr,sizeof(*_tmpDE5)),((_tmpDE5[0]=((_tmpDE8.tag=3,((_tmpDE8.f1=(void*)((_tmpDE6=_region_malloc(yyr,sizeof(*_tmpDE6)),((_tmpDE6[0]=((_tmpDE7.tag=1,((_tmpDE7.f1=_tmp750,((_tmpDE7.f2=_tmp751,((_tmpDE7.f3=_tmp752,((_tmpDE7.f4=_tmp753,((_tmpDE7.f5=_tmp754,((_tmpDE7.f6=
Cyc_yyget_YY55(yyyyvsp[4]),((_tmpDE7.f7=Cyc_yyget_YY55(yyyyvsp[5]),_tmpDE7)))))))))))))))),_tmpDE6)))),_tmpDE8)))),_tmpDE5)))),((_tmpDE9->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpDE9)))))),_tmpDE4)));}
# 2458
break;}case 283: _LL353: {
# 2460
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2462
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2460 "parse.y"
struct Cyc_List_List*_tmp75B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));
{struct Cyc_List_List*_tmpDF3;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpDF2;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpDF1;struct Cyc_Parse_Abstractdeclarator _tmpDF0;yyval=Cyc_YY30(((_tmpDF0.tms=((_tmpDF3=_region_malloc(yyr,sizeof(*_tmpDF3)),((_tmpDF3->hd=(void*)((_tmpDF1=_region_malloc(yyr,sizeof(*_tmpDF1)),((_tmpDF1[0]=((_tmpDF2.tag=4,((_tmpDF2.f1=_tmp75B,((_tmpDF2.f2=(unsigned int)(yyyylsp[1]).first_line,((_tmpDF2.f3=0,_tmpDF2)))))))),_tmpDF1)))),((_tmpDF3->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpDF3)))))),_tmpDF0)));}
# 2464
break;}case 284: _LL354: {
# 2466
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2468
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2465 "parse.y"
{struct Cyc_List_List*_tmpDFD;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpDFC;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpDFB;struct Cyc_Parse_Abstractdeclarator _tmpDFA;yyval=Cyc_YY30(((_tmpDFA.tms=((_tmpDFD=_region_malloc(yyr,sizeof(*_tmpDFD)),((_tmpDFD->hd=(void*)((_tmpDFB=_region_malloc(yyr,sizeof(*_tmpDFB)),((_tmpDFB[0]=((_tmpDFC.tag=5,((_tmpDFC.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpDFC.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpDFC)))))),_tmpDFB)))),((_tmpDFD->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpDFD)))))),_tmpDFA)));}
# 2467
break;}case 285: _LL355: {
# 2469
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2471
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
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2480
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2476 "parse.y"
yyval=yyyyvsp[0];
break;}case 291: _LL35B: {
# 2479
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2481
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2479 "parse.y"
{struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct _tmpE00;struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmpDFF;yyval=Cyc_Stmt_tok(Cyc_Absyn_new_stmt((void*)((_tmpDFF=_cycalloc(sizeof(*_tmpDFF)),((_tmpDFF[0]=((_tmpE00.tag=16,((_tmpE00.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),_tmpE00)))),_tmpDFF)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 292: _LL35C: {
# 2482
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2484
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpE06;struct _dyneither_ptr*_tmpE05;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpE04;yyval=Cyc_Stmt_tok(Cyc_Absyn_new_stmt((void*)((_tmpE04=_cycalloc(sizeof(*_tmpE04)),((_tmpE04[0]=((_tmpE06.tag=13,((_tmpE06.f1=((_tmpE05=_cycalloc(sizeof(*_tmpE05)),((_tmpE05[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE05)))),((_tmpE06.f2=Cyc_yyget_Stmt_tok(yyyyvsp[2]),_tmpE06)))))),_tmpE04)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 293: _LL35D: {
# 2488
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2490
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2489 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 294: _LL35E: {
# 2492
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2494
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2490 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_exp_stmt(Cyc_yyget_Exp_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 295: _LL35F: {
# 2493
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2495
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2495 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 296: _LL360: {
# 2498
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2500
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2496 "parse.y"
yyval=yyyyvsp[1];
break;}case 297: _LL361: {
# 2499
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2501
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2501 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(Cyc_yyget_YY16(yyyyvsp[0]),Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line)));
break;}case 298: _LL362: {
# 2504
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2506
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2502 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(Cyc_yyget_YY16(yyyyvsp[0]),Cyc_yyget_Stmt_tok(yyyyvsp[1])));
break;}case 299: _LL363: {
# 2505
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2507
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2503 "parse.y"
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpE0C;struct _dyneither_ptr*_tmpE0B;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpE0A;yyval=Cyc_Stmt_tok(Cyc_Absyn_new_stmt((void*)((_tmpE0A=_cycalloc(sizeof(*_tmpE0A)),((_tmpE0A[0]=((_tmpE0C.tag=13,((_tmpE0C.f1=((_tmpE0B=_cycalloc(sizeof(*_tmpE0B)),((_tmpE0B[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE0B)))),((_tmpE0C.f2=Cyc_Parse_flatten_declarations(Cyc_yyget_YY16(yyyyvsp[2]),Cyc_Absyn_skip_stmt(0)),_tmpE0C)))))),_tmpE0A)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2505
break;}case 300: _LL364: {
# 2507
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2509
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2505 "parse.y"
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpE12;struct _dyneither_ptr*_tmpE11;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpE10;yyval=Cyc_Stmt_tok(Cyc_Absyn_new_stmt((void*)((_tmpE10=_cycalloc(sizeof(*_tmpE10)),((_tmpE10[0]=((_tmpE12.tag=13,((_tmpE12.f1=((_tmpE11=_cycalloc(sizeof(*_tmpE11)),((_tmpE11[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE11)))),((_tmpE12.f2=Cyc_Parse_flatten_declarations(Cyc_yyget_YY16(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[3])),_tmpE12)))))),_tmpE10)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2507
break;}case 301: _LL365: {
# 2509
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2511
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2507 "parse.y"
yyval=yyyyvsp[0];
break;}case 302: _LL366: {
# 2510
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2512
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2508 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_seq_stmt(Cyc_yyget_Stmt_tok(yyyyvsp[0]),Cyc_yyget_Stmt_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 303: _LL367: {
# 2511
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2513
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2509 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpE15;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpE14;yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpE14=_cycalloc(sizeof(*_tmpE14)),((_tmpE14[0]=((_tmpE15.tag=1,((_tmpE15.f1=Cyc_yyget_YY15(yyyyvsp[0]),_tmpE15)))),_tmpE14)))),(unsigned int)(yyyylsp[0]).first_line),
Cyc_Absyn_skip_stmt(0)));}
break;}case 304: _LL368: {
# 2513
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2515
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2512 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpE18;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpE17;yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpE17=_cycalloc(sizeof(*_tmpE17)),((_tmpE17[0]=((_tmpE18.tag=1,((_tmpE18.f1=Cyc_yyget_YY15(yyyyvsp[0]),_tmpE18)))),_tmpE17)))),(unsigned int)(yyyylsp[0]).first_line),Cyc_yyget_Stmt_tok(yyyyvsp[1])));}
break;}case 305: _LL369: {
# 2515
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2517
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2517 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[4]),Cyc_Absyn_skip_stmt(0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 306: _LL36A: {
# 2520
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2522
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2519 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[4]),Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 307: _LL36B: {
# 2522
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2524
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
yyval=Cyc_Stmt_tok(Cyc_Absyn_switch_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_YY8(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 308: _LL36C: {
# 2528
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2530
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2528 "parse.y"
struct Cyc_Absyn_Exp*_tmp773=Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Absyn_switch_stmt(_tmp773,Cyc_yyget_YY8(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 309: _LL36D: {
# 2532
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2534
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2532 "parse.y"
struct Cyc_Absyn_Exp*_tmp774=Cyc_Absyn_tuple_exp(Cyc_yyget_YY4(yyyyvsp[3]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Absyn_switch_stmt(_tmp774,Cyc_yyget_YY8(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
# 2535
break;}case 310: _LL36E: {
# 2537
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2539
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2538 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_trycatch_stmt(Cyc_yyget_Stmt_tok(yyyyvsp[1]),Cyc_yyget_YY8(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 311: _LL36F:
# 2541
 yyval=Cyc_YY8(0);
break;case 312: _LL370: {
# 2544
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2546
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2554 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpE1B;struct Cyc_List_List*_tmpE1A;yyval=Cyc_YY8(((_tmpE1A=_cycalloc(sizeof(*_tmpE1A)),((_tmpE1A->hd=((_tmpE1B=_cycalloc(sizeof(*_tmpE1B)),((_tmpE1B->pattern=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line),((_tmpE1B->pat_vars=0,((_tmpE1B->where_clause=0,((_tmpE1B->body=
Cyc_yyget_Stmt_tok(yyyyvsp[2]),((_tmpE1B->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpE1B)))))))))))),((_tmpE1A->tl=0,_tmpE1A)))))));}
# 2557
break;}case 313: _LL371: {
# 2559
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2561
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2558 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpE1E;struct Cyc_List_List*_tmpE1D;yyval=Cyc_YY8(((_tmpE1D=_cycalloc(sizeof(*_tmpE1D)),((_tmpE1D->hd=((_tmpE1E=_cycalloc(sizeof(*_tmpE1E)),((_tmpE1E->pattern=Cyc_yyget_YY9(yyyyvsp[1]),((_tmpE1E->pat_vars=0,((_tmpE1E->where_clause=0,((_tmpE1E->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[2]).first_line),((_tmpE1E->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpE1E)))))))))))),((_tmpE1D->tl=
Cyc_yyget_YY8(yyyyvsp[3]),_tmpE1D)))))));}
break;}case 314: _LL372: {
# 2563
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2565
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2562 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpE21;struct Cyc_List_List*_tmpE20;yyval=Cyc_YY8(((_tmpE20=_cycalloc(sizeof(*_tmpE20)),((_tmpE20->hd=((_tmpE21=_cycalloc(sizeof(*_tmpE21)),((_tmpE21->pattern=Cyc_yyget_YY9(yyyyvsp[1]),((_tmpE21->pat_vars=0,((_tmpE21->where_clause=0,((_tmpE21->body=Cyc_yyget_Stmt_tok(yyyyvsp[3]),((_tmpE21->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpE21)))))))))))),((_tmpE20->tl=Cyc_yyget_YY8(yyyyvsp[4]),_tmpE20)))))));}
break;}case 315: _LL373: {
# 2565
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2567
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2564 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpE24;struct Cyc_List_List*_tmpE23;yyval=Cyc_YY8(((_tmpE23=_cycalloc(sizeof(*_tmpE23)),((_tmpE23->hd=((_tmpE24=_cycalloc(sizeof(*_tmpE24)),((_tmpE24->pattern=Cyc_yyget_YY9(yyyyvsp[1]),((_tmpE24->pat_vars=0,((_tmpE24->where_clause=Cyc_yyget_Exp_tok(yyyyvsp[3]),((_tmpE24->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[4]).first_line),((_tmpE24->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpE24)))))))))))),((_tmpE23->tl=
Cyc_yyget_YY8(yyyyvsp[5]),_tmpE23)))))));}
break;}case 316: _LL374: {
# 2569
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2571
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2568 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpE27;struct Cyc_List_List*_tmpE26;yyval=Cyc_YY8(((_tmpE26=_cycalloc(sizeof(*_tmpE26)),((_tmpE26->hd=((_tmpE27=_cycalloc(sizeof(*_tmpE27)),((_tmpE27->pattern=Cyc_yyget_YY9(yyyyvsp[1]),((_tmpE27->pat_vars=0,((_tmpE27->where_clause=Cyc_yyget_Exp_tok(yyyyvsp[3]),((_tmpE27->body=Cyc_yyget_Stmt_tok(yyyyvsp[5]),((_tmpE27->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpE27)))))))))))),((_tmpE26->tl=Cyc_yyget_YY8(yyyyvsp[6]),_tmpE26)))))));}
break;}case 317: _LL375: {
# 2571
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2573
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2575 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_while_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Stmt_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 318: _LL376: {
# 2578
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2580
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2579 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_do_stmt(Cyc_yyget_Stmt_tok(yyyyvsp[1]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 319: _LL377: {
# 2582
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2584
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2583 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 320: _LL378: {
# 2587
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2589
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2586 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[4]),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 321: _LL379: {
# 2590
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2592
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2589 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 322: _LL37A: {
# 2593
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2595
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2592 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[3]),Cyc_yyget_Exp_tok(yyyyvsp[5]),
Cyc_yyget_Stmt_tok(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 323: _LL37B: {
# 2596
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2598
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2595 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 324: _LL37C: {
# 2599
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2601
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2598 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[5]),
Cyc_yyget_Stmt_tok(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 325: _LL37D: {
# 2602
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2604
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2601 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 326: _LL37E: {
# 2605
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 2607
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 2604 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_for_stmt(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),Cyc_yyget_Exp_tok(yyyyvsp[6]),
Cyc_yyget_Stmt_tok(yyyyvsp[8]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 327: _LL37F: {
# 2608
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2610
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2607 "parse.y"
struct Cyc_List_List*_tmp77F=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp780=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp77F,_tmp780));
# 2612
break;}case 328: _LL380: {
# 2614
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2616
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2613 "parse.y"
struct Cyc_List_List*_tmp781=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp782=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp781,_tmp782));
# 2618
break;}case 329: _LL381: {
# 2620
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2622
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2619 "parse.y"
struct Cyc_List_List*_tmp783=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp784=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[4]),
Cyc_yyget_Stmt_tok(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp783,_tmp784));
# 2624
break;}case 330: _LL382: {
# 2626
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2628
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2625 "parse.y"
struct Cyc_List_List*_tmp785=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp786=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_Exp_tok(yyyyvsp[3]),Cyc_yyget_Exp_tok(yyyyvsp[5]),
Cyc_yyget_Stmt_tok(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp785,_tmp786));
# 2630
break;}case 331: _LL383: {
# 2632
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2634
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
{struct _dyneither_ptr*_tmpE28;yyval=Cyc_Stmt_tok(Cyc_Absyn_goto_stmt(((_tmpE28=_cycalloc(sizeof(*_tmpE28)),((_tmpE28[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpE28)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 332: _LL384: {
# 2638
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2640
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2636 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_continue_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 333: _LL385: {
# 2639
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2641
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2637 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_break_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 334: _LL386: {
# 2640
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2642
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2638 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 335: _LL387: {
# 2641
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2643
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2639 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 336: _LL388: {
# 2642
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2644
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2641 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 337: _LL389: {
# 2644
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2646
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2642 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 338: _LL38A: {
# 2645
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2647
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2644 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 339: _LL38B: {
# 2647
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2649
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2653 "parse.y"
yyval=yyyyvsp[0];
break;}case 340: _LL38C: {
# 2656
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2658
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2656 "parse.y"
yyval=yyyyvsp[0];
break;}case 341: _LL38D: {
# 2659
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2661
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2658 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 342: _LL38E: {
# 2661
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2663
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2661 "parse.y"
yyval=yyyyvsp[0];
break;}case 343: _LL38F: {
# 2664
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2666
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2663 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 344: _LL390: {
# 2666
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2668
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2666 "parse.y"
yyval=yyyyvsp[0];
break;}case 345: _LL391: {
# 2669
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2671
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2668 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 346: _LL392: {
# 2671
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2673
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2671 "parse.y"
yyval=yyyyvsp[0];
break;}case 347: _LL393: {
# 2674
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2676
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2673 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 348: _LL394: {
# 2676
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2678
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2676 "parse.y"
yyval=yyyyvsp[0];
break;}case 349: _LL395: {
# 2679
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2681
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2678 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 350: _LL396: {
# 2681
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2683
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2681 "parse.y"
yyval=yyyyvsp[0];
break;}case 351: _LL397: {
# 2684
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2686
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2683 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 352: _LL398: {
# 2686
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2688
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2686 "parse.y"
yyval=yyyyvsp[0];
break;}case 353: _LL399: {
# 2689
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2691
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2688 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 354: _LL39A: {
# 2691
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2693
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2690 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 355: _LL39B: {
# 2693
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2695
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2693 "parse.y"
yyval=yyyyvsp[0];
break;}case 356: _LL39C: {
# 2696
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2698
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2695 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 357: _LL39D: {
# 2698
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2700
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2697 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 358: _LL39E: {
# 2700
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2702
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2699 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 359: _LL39F: {
# 2702
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2704
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2701 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 360: _LL3A0: {
# 2704
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2706
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2704 "parse.y"
yyval=yyyyvsp[0];
break;}case 361: _LL3A1: {
# 2707
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2709
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2706 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 362: _LL3A2: {
# 2709
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2711
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2708 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 363: _LL3A3: {
# 2711
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2713
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2711 "parse.y"
yyval=yyyyvsp[0];
break;}case 364: _LL3A4: {
# 2714
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2716
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2713 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 365: _LL3A5: {
# 2716
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2718
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2715 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 366: _LL3A6: {
# 2718
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2720
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2718 "parse.y"
yyval=yyyyvsp[0];
break;}case 367: _LL3A7: {
# 2721
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2723
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2720 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 368: _LL3A8: {
# 2723
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2725
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2722 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 369: _LL3A9: {
# 2725
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2727
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2724 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0])),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 370: _LL3AA: {
# 2727
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2729
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2727 "parse.y"
yyval=yyyyvsp[0];
break;}case 371: _LL3AB: {
# 2730
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2732
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2729 "parse.y"
void*_tmp788=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp(_tmp788,Cyc_yyget_Exp_tok(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line)));
# 2732
break;}case 372: _LL3AC: {
# 2734
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2736
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2735 "parse.y"
yyval=yyyyvsp[0];
break;}case 373: _LL3AD: {
# 2738
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2740
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2738 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(yyyyvsp[1])));
break;}case 374: _LL3AE: {
# 2741
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2743
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2740 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_prim1_exp(Cyc_yyget_YY6(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 375: _LL3AF: {
# 2743
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2745
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2742 "parse.y"
void*_tmp789=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp(_tmp789,(unsigned int)(yyyylsp[0]).first_line)));
# 2745
break;}case 376: _LL3B0: {
# 2747
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2749
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2746 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 377: _LL3B1: {
# 2749
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2751
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2748 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyyyvsp[2])).f3,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4])),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 378: _LL3B2: {
# 2751
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2753
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2753 "parse.y"
yyval=yyyyvsp[0];
break;}case 379: _LL3B3: {
# 2756
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2758
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2762 "parse.y"
yyval=yyyyvsp[0];
break;}case 380: _LL3B4: {
# 2765
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2767
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2767 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line));
break;}case 381: _LL3B5: {
# 2770
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2772
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2769 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(yyyyvsp[1])));
break;}case 382: _LL3B6: {
# 2772
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2774
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2771 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(yyyyvsp[0]);
{void*_tmp78A=e->r;void*_tmp78B=_tmp78A;enum Cyc_Absyn_Sign _tmp78D;char _tmp78E;enum Cyc_Absyn_Sign _tmp790;short _tmp791;enum Cyc_Absyn_Sign _tmp793;int _tmp794;struct _dyneither_ptr _tmp796;int _tmp797;_LL3B9: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp78C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp78B;if(_tmp78C->tag != 0)goto _LL3BB;else{if(((_tmp78C->f1).Char_c).tag != 2)goto _LL3BB;_tmp78D=((struct _tuple3)((_tmp78C->f1).Char_c).val).f1;_tmp78E=((struct _tuple3)((_tmp78C->f1).Char_c).val).f2;}}_LL3BA:
# 2775
{struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct _tmpE2B;struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmpE2A;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE2A=_cycalloc_atomic(sizeof(*_tmpE2A)),((_tmpE2A[0]=((_tmpE2B.tag=11,((_tmpE2B.f1=_tmp78E,_tmpE2B)))),_tmpE2A)))),e->loc));}goto _LL3B8;_LL3BB: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp78F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp78B;if(_tmp78F->tag != 0)goto _LL3BD;else{if(((_tmp78F->f1).Short_c).tag != 4)goto _LL3BD;_tmp790=((struct _tuple4)((_tmp78F->f1).Short_c).val).f1;_tmp791=((struct _tuple4)((_tmp78F->f1).Short_c).val).f2;}}_LL3BC:
# 2777
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpE2E;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpE2D;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE2D=_cycalloc_atomic(sizeof(*_tmpE2D)),((_tmpE2D[0]=((_tmpE2E.tag=10,((_tmpE2E.f1=_tmp790,((_tmpE2E.f2=(int)_tmp791,_tmpE2E)))))),_tmpE2D)))),e->loc));}goto _LL3B8;_LL3BD: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp792=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp78B;if(_tmp792->tag != 0)goto _LL3BF;else{if(((_tmp792->f1).Int_c).tag != 5)goto _LL3BF;_tmp793=((struct _tuple5)((_tmp792->f1).Int_c).val).f1;_tmp794=((struct _tuple5)((_tmp792->f1).Int_c).val).f2;}}_LL3BE:
# 2779
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpE31;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpE30;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE30=_cycalloc_atomic(sizeof(*_tmpE30)),((_tmpE30[0]=((_tmpE31.tag=10,((_tmpE31.f1=_tmp793,((_tmpE31.f2=_tmp794,_tmpE31)))))),_tmpE30)))),e->loc));}goto _LL3B8;_LL3BF: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp795=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp78B;if(_tmp795->tag != 0)goto _LL3C1;else{if(((_tmp795->f1).Float_c).tag != 7)goto _LL3C1;_tmp796=((struct _tuple7)((_tmp795->f1).Float_c).val).f1;_tmp797=((struct _tuple7)((_tmp795->f1).Float_c).val).f2;}}_LL3C0:
# 2781
{struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct _tmpE34;struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmpE33;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE33=_cycalloc(sizeof(*_tmpE33)),((_tmpE33[0]=((_tmpE34.tag=12,((_tmpE34.f1=_tmp796,((_tmpE34.f2=_tmp797,_tmpE34)))))),_tmpE33)))),e->loc));}goto _LL3B8;_LL3C1: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp798=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp78B;if(_tmp798->tag != 0)goto _LL3C3;else{if(((_tmp798->f1).Null_c).tag != 1)goto _LL3C3;}}_LL3C2:
# 2783
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL3B8;_LL3C3: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp799=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp78B;if(_tmp799->tag != 0)goto _LL3C5;else{if(((_tmp799->f1).String_c).tag != 8)goto _LL3C5;}}_LL3C4:
# 2785
{const char*_tmpE35;Cyc_Parse_err(((_tmpE35="strings cannot occur within patterns",_tag_dyneither(_tmpE35,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3B8;_LL3C5: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp79A=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp78B;if(_tmp79A->tag != 0)goto _LL3C7;else{if(((_tmp79A->f1).Wstring_c).tag != 9)goto _LL3C7;}}_LL3C6:
# 2787
{const char*_tmpE36;Cyc_Parse_err(((_tmpE36="strings cannot occur within patterns",_tag_dyneither(_tmpE36,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3B8;_LL3C7: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp79B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp78B;if(_tmp79B->tag != 0)goto _LL3C9;else{if(((_tmp79B->f1).LongLong_c).tag != 6)goto _LL3C9;}}_LL3C8:
# 2789
{const char*_tmpE37;Cyc_Parse_unimp(((_tmpE37="long long's in patterns",_tag_dyneither(_tmpE37,sizeof(char),24))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3B8;_LL3C9:;_LL3CA: {
# 2791
const char*_tmpE38;Cyc_Parse_err(((_tmpE38="bad constant in case",_tag_dyneither(_tmpE38,sizeof(char),21))),(unsigned int)(yyyylsp[0]).first_line);}_LL3B8:;}
# 2794
break;}case 383: _LL3B7: {
# 2796
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2798
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2795 "parse.y"
{struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct _tmpE3B;struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmpE3A;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE3A=_cycalloc(sizeof(*_tmpE3A)),((_tmpE3A[0]=((_tmpE3B.tag=15,((_tmpE3B.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),_tmpE3B)))),_tmpE3A)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 384: _LL3CB: {
# 2798
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2800
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2797 "parse.y"
{const char*_tmpE3C;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpE3C="as",_tag_dyneither(_tmpE3C,sizeof(char),3))))!= 0){
const char*_tmpE3D;Cyc_Parse_err(((_tmpE3D="expecting `as'",_tag_dyneither(_tmpE3D,sizeof(char),15))),(unsigned int)(yyyylsp[1]).first_line);}}
{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpE47;struct _dyneither_ptr*_tmpE46;struct _tuple0*_tmpE45;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpE44;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE44=_cycalloc(sizeof(*_tmpE44)),((_tmpE44[0]=((_tmpE47.tag=1,((_tmpE47.f1=Cyc_Absyn_new_vardecl(((_tmpE45=_cycalloc(sizeof(*_tmpE45)),((_tmpE45->f1=Cyc_Absyn_Loc_n,((_tmpE45->f2=((_tmpE46=_cycalloc(sizeof(*_tmpE46)),((_tmpE46[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE46)))),_tmpE45)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpE47.f2=
Cyc_yyget_YY9(yyyyvsp[2]),_tmpE47)))))),_tmpE44)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2802
break;}case 385: _LL3CC: {
# 2804
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2806
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2803 "parse.y"
{const char*_tmpE48;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]),((_tmpE48="alias",_tag_dyneither(_tmpE48,sizeof(char),6))))!= 0){
const char*_tmpE49;Cyc_Parse_err(((_tmpE49="expecting `alias'",_tag_dyneither(_tmpE49,sizeof(char),18))),(unsigned int)(yyyylsp[1]).first_line);}}{
int _tmp7B2=(yyyylsp[0]).first_line;
struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpE51;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmpE50;struct _dyneither_ptr*_tmpE4F;struct Cyc_Absyn_Tvar*_tmpE4E;struct Cyc_Absyn_Tvar*tv=(_tmpE4E=_cycalloc(sizeof(*_tmpE4E)),((_tmpE4E->name=((_tmpE4F=_cycalloc(sizeof(*_tmpE4F)),((_tmpE4F[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpE4F)))),((_tmpE4E->identity=- 1,((_tmpE4E->kind=(void*)((_tmpE51=_cycalloc(sizeof(*_tmpE51)),((_tmpE51[0]=((_tmpE50.tag=0,((_tmpE50.f1=& Cyc_Tcutil_rk,_tmpE50)))),_tmpE51)))),_tmpE4E)))))));
struct _dyneither_ptr*_tmpE54;struct _tuple0*_tmpE53;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpE53=_cycalloc(sizeof(*_tmpE53)),((_tmpE53->f1=Cyc_Absyn_Loc_n,((_tmpE53->f2=((_tmpE54=_cycalloc(sizeof(*_tmpE54)),((_tmpE54[0]=Cyc_yyget_String_tok(yyyyvsp[5]),_tmpE54)))),_tmpE53)))))),
Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[4]),(unsigned int)(yyyylsp[4]).first_line),0);
{struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct _tmpE57;struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmpE56;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE56=_cycalloc(sizeof(*_tmpE56)),((_tmpE56[0]=((_tmpE57.tag=2,((_tmpE57.f1=tv,((_tmpE57.f2=vd,_tmpE57)))))),_tmpE56)))),(unsigned int)_tmp7B2));}
# 2811
break;};}case 386: _LL3CD: {
# 2813
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2815
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2812 "parse.y"
struct _tuple22 _tmp7BB=*Cyc_yyget_YY10(yyyyvsp[2]);struct Cyc_List_List*_tmp7BD;int _tmp7BE;struct _tuple22 _tmp7BC=_tmp7BB;_tmp7BD=_tmp7BC.f1;_tmp7BE=_tmp7BC.f2;
{struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct _tmpE5A;struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmpE59;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE59=_cycalloc(sizeof(*_tmpE59)),((_tmpE59[0]=((_tmpE5A.tag=5,((_tmpE5A.f1=_tmp7BD,((_tmpE5A.f2=_tmp7BE,_tmpE5A)))))),_tmpE59)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2815
break;}case 387: _LL3CE: {
# 2817
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2819
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2816 "parse.y"
struct _tuple22 _tmp7C1=*Cyc_yyget_YY10(yyyyvsp[2]);struct Cyc_List_List*_tmp7C3;int _tmp7C4;struct _tuple22 _tmp7C2=_tmp7C1;_tmp7C3=_tmp7C2.f1;_tmp7C4=_tmp7C2.f2;
{struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct _tmpE5D;struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmpE5C;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE5C=_cycalloc(sizeof(*_tmpE5C)),((_tmpE5C[0]=((_tmpE5D.tag=16,((_tmpE5D.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpE5D.f2=_tmp7C3,((_tmpE5D.f3=_tmp7C4,_tmpE5D)))))))),_tmpE5C)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2819
break;}case 388: _LL3CF: {
# 2821
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2823
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2820 "parse.y"
struct _tuple22 _tmp7C7=*Cyc_yyget_YY14(yyyyvsp[3]);struct Cyc_List_List*_tmp7C9;int _tmp7CA;struct _tuple22 _tmp7C8=_tmp7C7;_tmp7C9=_tmp7C8.f1;_tmp7CA=_tmp7C8.f2;{
struct Cyc_List_List*_tmp7CB=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY40(yyyyvsp[2]));
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpE63;struct Cyc_Absyn_AggrInfo*_tmpE62;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpE61;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE61=_cycalloc(sizeof(*_tmpE61)),((_tmpE61[0]=((_tmpE63.tag=7,((_tmpE63.f1=((_tmpE62=_cycalloc(sizeof(*_tmpE62)),((_tmpE62->aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(yyyyvsp[0]),0),((_tmpE62->targs=0,_tmpE62)))))),((_tmpE63.f2=_tmp7CB,((_tmpE63.f3=_tmp7C9,((_tmpE63.f4=_tmp7CA,_tmpE63)))))))))),_tmpE61)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2825
break;};}case 389: _LL3D0: {
# 2827
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2829
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2826 "parse.y"
struct _tuple22 _tmp7CF=*Cyc_yyget_YY14(yyyyvsp[2]);struct Cyc_List_List*_tmp7D1;int _tmp7D2;struct _tuple22 _tmp7D0=_tmp7CF;_tmp7D1=_tmp7D0.f1;_tmp7D2=_tmp7D0.f2;{
struct Cyc_List_List*_tmp7D3=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,Cyc_yyget_YY40(yyyyvsp[1]));
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpE66;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpE65;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE65=_cycalloc(sizeof(*_tmpE65)),((_tmpE65[0]=((_tmpE66.tag=7,((_tmpE66.f1=0,((_tmpE66.f2=_tmp7D3,((_tmpE66.f3=_tmp7D1,((_tmpE66.f4=_tmp7D2,_tmpE66)))))))))),_tmpE65)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2830
break;};}case 390: _LL3D1: {
# 2832
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2834
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2831 "parse.y"
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE69;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpE68;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE68=_cycalloc(sizeof(*_tmpE68)),((_tmpE68[0]=((_tmpE69.tag=6,((_tmpE69.f1=Cyc_yyget_YY9(yyyyvsp[1]),_tmpE69)))),_tmpE68)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 391: _LL3D2: {
# 2834
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2836
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2833 "parse.y"
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE73;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE72;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpE71;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpE70;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE70=_cycalloc(sizeof(*_tmpE70)),((_tmpE70[0]=((_tmpE73.tag=6,((_tmpE73.f1=Cyc_Absyn_new_pat((void*)((_tmpE71=_cycalloc(sizeof(*_tmpE71)),((_tmpE71[0]=((_tmpE72.tag=6,((_tmpE72.f1=Cyc_yyget_YY9(yyyyvsp[1]),_tmpE72)))),_tmpE71)))),(unsigned int)(yyyylsp[0]).first_line),_tmpE73)))),_tmpE70)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 392: _LL3D3: {
# 2836
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2838
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2835 "parse.y"
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpE7D;struct _dyneither_ptr*_tmpE7C;struct _tuple0*_tmpE7B;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpE7A;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE7A=_cycalloc(sizeof(*_tmpE7A)),((_tmpE7A[0]=((_tmpE7D.tag=3,((_tmpE7D.f1=Cyc_Absyn_new_vardecl(((_tmpE7B=_cycalloc(sizeof(*_tmpE7B)),((_tmpE7B->f1=Cyc_Absyn_Loc_n,((_tmpE7B->f2=((_tmpE7C=_cycalloc(sizeof(*_tmpE7C)),((_tmpE7C[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpE7C)))),_tmpE7B)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpE7D.f2=
# 2837
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[1]).first_line),_tmpE7D)))))),_tmpE7A)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2839
break;}case 393: _LL3D4: {
# 2841
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2843
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2840 "parse.y"
{const char*_tmpE7E;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[2]),((_tmpE7E="as",_tag_dyneither(_tmpE7E,sizeof(char),3))))!= 0){
const char*_tmpE7F;Cyc_Parse_err(((_tmpE7F="expecting `as'",_tag_dyneither(_tmpE7F,sizeof(char),15))),(unsigned int)(yyyylsp[2]).first_line);}}
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpE89;struct _dyneither_ptr*_tmpE88;struct _tuple0*_tmpE87;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpE86;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE86=_cycalloc(sizeof(*_tmpE86)),((_tmpE86[0]=((_tmpE89.tag=3,((_tmpE89.f1=Cyc_Absyn_new_vardecl(((_tmpE87=_cycalloc(sizeof(*_tmpE87)),((_tmpE87->f1=Cyc_Absyn_Loc_n,((_tmpE87->f2=((_tmpE88=_cycalloc(sizeof(*_tmpE88)),((_tmpE88[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpE88)))),_tmpE87)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpE89.f2=
# 2844
Cyc_yyget_YY9(yyyyvsp[3]),_tmpE89)))))),_tmpE86)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2846
break;}case 394: _LL3D5: {
# 2848
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2850
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2847 "parse.y"
void*_tmp7E6=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[2]),Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpE9A;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpE99;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE98;struct _dyneither_ptr*_tmpE97;struct _tuple0*_tmpE96;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpE95;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpE95=_cycalloc(sizeof(*_tmpE95)),((_tmpE95[0]=((_tmpE9A.tag=4,((_tmpE9A.f1=Cyc_Parse_typ2tvar((unsigned int)(yyyylsp[2]).first_line,_tmp7E6),((_tmpE9A.f2=
Cyc_Absyn_new_vardecl(((_tmpE96=_cycalloc(sizeof(*_tmpE96)),((_tmpE96->f1=Cyc_Absyn_Loc_n,((_tmpE96->f2=((_tmpE97=_cycalloc(sizeof(*_tmpE97)),((_tmpE97[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE97)))),_tmpE96)))))),(void*)(
(_tmpE98=_cycalloc(sizeof(*_tmpE98)),((_tmpE98[0]=((_tmpE99.tag=19,((_tmpE99.f1=_tmp7E6,_tmpE99)))),_tmpE98)))),0),_tmpE9A)))))),_tmpE95)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2852
break;}case 395: _LL3D6: {
# 2854
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2856
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2853 "parse.y"
struct Cyc_Absyn_Tvar*_tmp7ED=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpEB6;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpEB5;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpEB4;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpEB3;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpEB2;struct _dyneither_ptr*_tmpEB1;struct _tuple0*_tmpEB0;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpEAF;yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)((_tmpEAF=_cycalloc(sizeof(*_tmpEAF)),((_tmpEAF[0]=((_tmpEB6.tag=4,((_tmpEB6.f1=_tmp7ED,((_tmpEB6.f2=
Cyc_Absyn_new_vardecl(((_tmpEB0=_cycalloc(sizeof(*_tmpEB0)),((_tmpEB0->f1=Cyc_Absyn_Loc_n,((_tmpEB0->f2=((_tmpEB1=_cycalloc(sizeof(*_tmpEB1)),((_tmpEB1[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpEB1)))),_tmpEB0)))))),(void*)(
(_tmpEB2=_cycalloc(sizeof(*_tmpEB2)),((_tmpEB2[0]=((_tmpEB5.tag=19,((_tmpEB5.f1=(void*)((_tmpEB3=_cycalloc(sizeof(*_tmpEB3)),((_tmpEB3[0]=((_tmpEB4.tag=2,((_tmpEB4.f1=_tmp7ED,_tmpEB4)))),_tmpEB3)))),_tmpEB5)))),_tmpEB2)))),0),_tmpEB6)))))),_tmpEAF)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2858
break;}case 396: _LL3D7: {
# 2860
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2862
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2861 "parse.y"
{struct _tuple22*_tmpEB7;yyval=Cyc_YY10(((_tmpEB7=_cycalloc(sizeof(*_tmpEB7)),((_tmpEB7->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(yyyyvsp[0])),((_tmpEB7->f2=0,_tmpEB7)))))));}
break;}case 397: _LL3D8: {
# 2864
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2866
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2862 "parse.y"
{struct _tuple22*_tmpEB8;yyval=Cyc_YY10(((_tmpEB8=_cycalloc(sizeof(*_tmpEB8)),((_tmpEB8->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(yyyyvsp[0])),((_tmpEB8->f2=1,_tmpEB8)))))));}
break;}case 398: _LL3D9: {
# 2865
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2867
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2863 "parse.y"
{struct _tuple22*_tmpEB9;yyval=Cyc_YY10(((_tmpEB9=_cycalloc(sizeof(*_tmpEB9)),((_tmpEB9->f1=0,((_tmpEB9->f2=1,_tmpEB9)))))));}
break;}case 399: _LL3DA: {
# 2866
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2868
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2868 "parse.y"
{struct Cyc_List_List*_tmpEBA;yyval=Cyc_YY11(((_tmpEBA=_cycalloc(sizeof(*_tmpEBA)),((_tmpEBA->hd=Cyc_yyget_YY9(yyyyvsp[0]),((_tmpEBA->tl=0,_tmpEBA)))))));}
break;}case 400: _LL3DB: {
# 2871
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2873
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2870 "parse.y"
{struct Cyc_List_List*_tmpEBB;yyval=Cyc_YY11(((_tmpEBB=_cycalloc(sizeof(*_tmpEBB)),((_tmpEBB->hd=Cyc_yyget_YY9(yyyyvsp[2]),((_tmpEBB->tl=Cyc_yyget_YY11(yyyyvsp[0]),_tmpEBB)))))));}
break;}case 401: _LL3DC: {
# 2873
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2875
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2875 "parse.y"
{struct _tuple23*_tmpEBC;yyval=Cyc_YY12(((_tmpEBC=_cycalloc(sizeof(*_tmpEBC)),((_tmpEBC->f1=0,((_tmpEBC->f2=Cyc_yyget_YY9(yyyyvsp[0]),_tmpEBC)))))));}
break;}case 402: _LL3DD: {
# 2878
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2880
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2877 "parse.y"
{struct _tuple23*_tmpEBD;yyval=Cyc_YY12(((_tmpEBD=_cycalloc(sizeof(*_tmpEBD)),((_tmpEBD->f1=Cyc_yyget_YY41(yyyyvsp[0]),((_tmpEBD->f2=Cyc_yyget_YY9(yyyyvsp[1]),_tmpEBD)))))));}
break;}case 403: _LL3DE: {
# 2880
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2882
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2880 "parse.y"
{struct _tuple22*_tmpEBE;yyval=Cyc_YY14(((_tmpEBE=_cycalloc(sizeof(*_tmpEBE)),((_tmpEBE->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0])),((_tmpEBE->f2=0,_tmpEBE)))))));}
break;}case 404: _LL3DF: {
# 2883
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2885
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2881 "parse.y"
{struct _tuple22*_tmpEBF;yyval=Cyc_YY14(((_tmpEBF=_cycalloc(sizeof(*_tmpEBF)),((_tmpEBF->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0])),((_tmpEBF->f2=1,_tmpEBF)))))));}
break;}case 405: _LL3E0: {
# 2884
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2886
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2882 "parse.y"
{struct _tuple22*_tmpEC0;yyval=Cyc_YY14(((_tmpEC0=_cycalloc(sizeof(*_tmpEC0)),((_tmpEC0->f1=0,((_tmpEC0->f2=1,_tmpEC0)))))));}
break;}case 406: _LL3E1: {
# 2885
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2887
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2887 "parse.y"
{struct Cyc_List_List*_tmpEC1;yyval=Cyc_YY13(((_tmpEC1=_cycalloc(sizeof(*_tmpEC1)),((_tmpEC1->hd=Cyc_yyget_YY12(yyyyvsp[0]),((_tmpEC1->tl=0,_tmpEC1)))))));}
break;}case 407: _LL3E2: {
# 2890
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2892
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2889 "parse.y"
{struct Cyc_List_List*_tmpEC2;yyval=Cyc_YY13(((_tmpEC2=_cycalloc(sizeof(*_tmpEC2)),((_tmpEC2->hd=Cyc_yyget_YY12(yyyyvsp[2]),((_tmpEC2->tl=Cyc_yyget_YY13(yyyyvsp[0]),_tmpEC2)))))));}
break;}case 408: _LL3E3: {
# 2892
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2894
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=yyyyvsp[0];
break;}case 409: _LL3E4: {
# 2898
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2900
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2897 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_seq_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 410: _LL3E5: {
# 2900
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2902
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2902 "parse.y"
yyval=yyyyvsp[0];
break;}case 411: _LL3E6: {
# 2905
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2907
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2904 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_assignop_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_YY7(yyyyvsp[1]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 412: _LL3E7: {
# 2907
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2909
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2906 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_swap_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 413: _LL3E8: {
# 2909
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2911
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2910 "parse.y"
yyval=Cyc_YY7(0);
break;}case 414: _LL3E9: {
# 2913
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2915
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2911 "parse.y"
{struct Cyc_Core_Opt*_tmpEC3;yyval=Cyc_YY7(((_tmpEC3=_cycalloc_atomic(sizeof(*_tmpEC3)),((_tmpEC3->v=(void*)Cyc_Absyn_Times,_tmpEC3)))));}
break;}case 415: _LL3EA: {
# 2914
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2916
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2912 "parse.y"
{struct Cyc_Core_Opt*_tmpEC4;yyval=Cyc_YY7(((_tmpEC4=_cycalloc_atomic(sizeof(*_tmpEC4)),((_tmpEC4->v=(void*)Cyc_Absyn_Div,_tmpEC4)))));}
break;}case 416: _LL3EB: {
# 2915
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2917
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2913 "parse.y"
{struct Cyc_Core_Opt*_tmpEC5;yyval=Cyc_YY7(((_tmpEC5=_cycalloc_atomic(sizeof(*_tmpEC5)),((_tmpEC5->v=(void*)Cyc_Absyn_Mod,_tmpEC5)))));}
break;}case 417: _LL3EC: {
# 2916
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2918
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2914 "parse.y"
{struct Cyc_Core_Opt*_tmpEC6;yyval=Cyc_YY7(((_tmpEC6=_cycalloc_atomic(sizeof(*_tmpEC6)),((_tmpEC6->v=(void*)Cyc_Absyn_Plus,_tmpEC6)))));}
break;}case 418: _LL3ED: {
# 2917
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2919
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2915 "parse.y"
{struct Cyc_Core_Opt*_tmpEC7;yyval=Cyc_YY7(((_tmpEC7=_cycalloc_atomic(sizeof(*_tmpEC7)),((_tmpEC7->v=(void*)Cyc_Absyn_Minus,_tmpEC7)))));}
break;}case 419: _LL3EE: {
# 2918
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2920
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2916 "parse.y"
{struct Cyc_Core_Opt*_tmpEC8;yyval=Cyc_YY7(((_tmpEC8=_cycalloc_atomic(sizeof(*_tmpEC8)),((_tmpEC8->v=(void*)Cyc_Absyn_Bitlshift,_tmpEC8)))));}
break;}case 420: _LL3EF: {
# 2919
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2921
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2917 "parse.y"
{struct Cyc_Core_Opt*_tmpEC9;yyval=Cyc_YY7(((_tmpEC9=_cycalloc_atomic(sizeof(*_tmpEC9)),((_tmpEC9->v=(void*)Cyc_Absyn_Bitlrshift,_tmpEC9)))));}
break;}case 421: _LL3F0: {
# 2920
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2922
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2918 "parse.y"
{struct Cyc_Core_Opt*_tmpECA;yyval=Cyc_YY7(((_tmpECA=_cycalloc_atomic(sizeof(*_tmpECA)),((_tmpECA->v=(void*)Cyc_Absyn_Bitand,_tmpECA)))));}
break;}case 422: _LL3F1: {
# 2921
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2923
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2919 "parse.y"
{struct Cyc_Core_Opt*_tmpECB;yyval=Cyc_YY7(((_tmpECB=_cycalloc_atomic(sizeof(*_tmpECB)),((_tmpECB->v=(void*)Cyc_Absyn_Bitxor,_tmpECB)))));}
break;}case 423: _LL3F2: {
# 2922
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2924
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2920 "parse.y"
{struct Cyc_Core_Opt*_tmpECC;yyval=Cyc_YY7(((_tmpECC=_cycalloc_atomic(sizeof(*_tmpECC)),((_tmpECC->v=(void*)Cyc_Absyn_Bitor,_tmpECC)))));}
break;}case 424: _LL3F3: {
# 2923
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2925
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2925 "parse.y"
yyval=yyyyvsp[0];
break;}case 425: _LL3F4: {
# 2928
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2930
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2927 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_conditional_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 426: _LL3F5: {
# 2930
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2932
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2930 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_throw_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 427: _LL3F6: {
# 2933
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2935
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2933 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 428: _LL3F7: {
# 2936
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2938
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2935 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_New_exp(0,Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 429: _LL3F8: {
# 2938
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2940
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2937 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_New_exp(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 430: _LL3F9: {
# 2940
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2942
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2939 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_New_exp(Cyc_yyget_Exp_tok(yyyyvsp[2]),Cyc_yyget_Exp_tok(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 431: _LL3FA: {
# 2942
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2944
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2943 "parse.y"
yyval=yyyyvsp[0];
break;}case 432: _LL3FB: {
# 2946
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2948
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2947 "parse.y"
yyval=yyyyvsp[0];
break;}case 433: _LL3FC: {
# 2950
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2952
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2949 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_or_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 434: _LL3FD: {
# 2952
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2954
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2953 "parse.y"
yyval=yyyyvsp[0];
break;}case 435: _LL3FE: {
# 2956
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2958
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2955 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_and_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 436: _LL3FF: {
# 2958
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2960
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2959 "parse.y"
yyval=yyyyvsp[0];
break;}case 437: _LL400: {
# 2962
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2964
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2961 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 438: _LL401: {
# 2964
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2966
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2965 "parse.y"
yyval=yyyyvsp[0];
break;}case 439: _LL402: {
# 2968
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2970
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2967 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 440: _LL403: {
# 2970
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2972
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2971 "parse.y"
yyval=yyyyvsp[0];
break;}case 441: _LL404: {
# 2974
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2976
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2973 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 442: _LL405: {
# 2976
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2978
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2977 "parse.y"
yyval=yyyyvsp[0];
break;}case 443: _LL406: {
# 2980
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2982
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2979 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_eq_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 444: _LL407: {
# 2982
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2984
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2981 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_neq_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 445: _LL408: {
# 2984
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2986
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2985 "parse.y"
yyval=yyyyvsp[0];
break;}case 446: _LL409: {
# 2988
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2990
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2987 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_lt_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 447: _LL40A: {
# 2990
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2992
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2989 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_gt_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 448: _LL40B: {
# 2992
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2994
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2991 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_lte_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 449: _LL40C: {
# 2994
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2996
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2993 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_gte_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 450: _LL40D: {
# 2996
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2998
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2997 "parse.y"
yyval=yyyyvsp[0];
break;}case 451: _LL40E: {
# 3000
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3002
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2999 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 452: _LL40F: {
# 3002
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3004
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3001 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 453: _LL410: {
# 3004
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3006
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3005 "parse.y"
yyval=yyyyvsp[0];
break;}case 454: _LL411: {
# 3008
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3010
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3007 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 455: _LL412: {
# 3010
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3012
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3009 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 456: _LL413: {
# 3012
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3014
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3013 "parse.y"
yyval=yyyyvsp[0];
break;}case 457: _LL414: {
# 3016
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3018
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3015 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 458: _LL415: {
# 3018
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3020
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3017 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 459: _LL416: {
# 3020
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3022
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3019 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 460: _LL417: {
# 3022
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3024
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3023 "parse.y"
yyval=yyyyvsp[0];
break;}case 461: _LL418: {
# 3026
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3028
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3025 "parse.y"
void*_tmp80C=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_cast_exp(_tmp80C,Cyc_yyget_Exp_tok(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line));
# 3028
break;}case 462: _LL419: {
# 3030
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3032
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3031 "parse.y"
yyval=yyyyvsp[0];
break;}case 463: _LL41A: {
# 3034
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3036
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3032 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_pre_inc_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 464: _LL41B: {
# 3035
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3037
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3033 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_pre_dec_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 465: _LL41C: {
# 3036
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3038
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3034 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_address_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 466: _LL41D: {
# 3037
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3039
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3035 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_deref_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 467: _LL41E: {
# 3038
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3040
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3036 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim1_exp(Cyc_Absyn_Plus,Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 468: _LL41F: {
# 3039
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3041
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3037 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_prim1_exp(Cyc_yyget_YY6(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 469: _LL420: {
# 3040
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3042
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3039 "parse.y"
void*_tmp80D=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_sizeoftyp_exp(_tmp80D,(unsigned int)(yyyylsp[0]).first_line));
# 3042
break;}case 470: _LL421: {
# 3044
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3046
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3042 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_Exp_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 471: _LL422: {
# 3045
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3047
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3044 "parse.y"
void*_tmp80E=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_offsetof_exp(_tmp80E,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4])),(unsigned int)(yyyylsp[0]).first_line));
# 3047
break;}case 472: _LL423: {
# 3049
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3051
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3049 "parse.y"
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpED2;struct Cyc_Absyn_MallocInfo _tmpED1;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpED0;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpED0=_cycalloc(sizeof(*_tmpED0)),((_tmpED0[0]=((_tmpED2.tag=33,((_tmpED2.f1=((_tmpED1.is_calloc=0,((_tmpED1.rgn=0,((_tmpED1.elt_type=0,((_tmpED1.num_elts=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpED1.fat_result=0,_tmpED1)))))))))),_tmpED2)))),_tmpED0)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3051
break;}case 473: _LL424: {
# 3053
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3055
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3052 "parse.y"
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpED8;struct Cyc_Absyn_MallocInfo _tmpED7;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpED6;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpED6=_cycalloc(sizeof(*_tmpED6)),((_tmpED6[0]=((_tmpED8.tag=33,((_tmpED8.f1=((_tmpED7.is_calloc=0,((_tmpED7.rgn=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpED7.elt_type=0,((_tmpED7.num_elts=Cyc_yyget_Exp_tok(yyyyvsp[4]),((_tmpED7.fat_result=0,_tmpED7)))))))))),_tmpED8)))),_tmpED6)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3054
break;}case 474: _LL425: {
# 3056
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 3058
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 3055 "parse.y"
void*_tmp815=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpEE2;void**_tmpEE1;struct Cyc_Absyn_MallocInfo _tmpEE0;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpEDF;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpEDF=_cycalloc(sizeof(*_tmpEDF)),((_tmpEDF[0]=((_tmpEE2.tag=33,((_tmpEE2.f1=((_tmpEE0.is_calloc=1,((_tmpEE0.rgn=0,((_tmpEE0.elt_type=((_tmpEE1=_cycalloc(sizeof(*_tmpEE1)),((_tmpEE1[0]=_tmp815,_tmpEE1)))),((_tmpEE0.num_elts=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpEE0.fat_result=0,_tmpEE0)))))))))),_tmpEE2)))),_tmpEDF)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3058
break;}case 475: _LL426: {
# 3060
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 10),sizeof(union Cyc_YYSTYPE),11);
# 3062
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 10),sizeof(struct Cyc_Yyltype),11);
# 3060 "parse.y"
void*_tmp81A=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[8]),(unsigned int)(yyyylsp[8]).first_line);
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpEEC;void**_tmpEEB;struct Cyc_Absyn_MallocInfo _tmpEEA;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpEE9;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpEE9=_cycalloc(sizeof(*_tmpEE9)),((_tmpEE9[0]=((_tmpEEC.tag=33,((_tmpEEC.f1=((_tmpEEA.is_calloc=1,((_tmpEEA.rgn=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpEEA.elt_type=((_tmpEEB=_cycalloc(sizeof(*_tmpEEB)),((_tmpEEB[0]=_tmp81A,_tmpEEB)))),((_tmpEEA.num_elts=Cyc_yyget_Exp_tok(yyyyvsp[4]),((_tmpEEA.fat_result=0,_tmpEEA)))))))))),_tmpEEC)))),_tmpEE9)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3063
break;}case 476: _LL427: {
# 3065
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3067
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3064 "parse.y"
{struct Cyc_Absyn_Exp*_tmpEED[1];yyval=Cyc_Exp_tok(Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,((_tmpEED[0]=Cyc_yyget_Exp_tok(yyyyvsp[2]),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpEED,sizeof(struct Cyc_Absyn_Exp*),1)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 477: _LL428: {
# 3067
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3069
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3066 "parse.y"
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpEF3;struct _dyneither_ptr*_tmpEF2;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpEF1;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpEF1=_cycalloc(sizeof(*_tmpEF1)),((_tmpEF1[0]=((_tmpEF3.tag=37,((_tmpEF3.f1=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpEF3.f2=((_tmpEF2=_cycalloc(sizeof(*_tmpEF2)),((_tmpEF2[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpEF2)))),_tmpEF3)))))),_tmpEF1)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 478: _LL429: {
# 3069
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3071
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3068 "parse.y"
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpEF9;struct _dyneither_ptr*_tmpEF8;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpEF7;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpEF7=_cycalloc(sizeof(*_tmpEF7)),((_tmpEF7[0]=((_tmpEF9.tag=37,((_tmpEF9.f1=Cyc_Absyn_deref_exp(Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line),((_tmpEF9.f2=((_tmpEF8=_cycalloc(sizeof(*_tmpEF8)),((_tmpEF8[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpEF8)))),_tmpEF9)))))),_tmpEF7)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 479: _LL42A: {
# 3071
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3073
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3070 "parse.y"
void*_tmp826=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_Exp_tok(Cyc_Absyn_valueof_exp(_tmp826,(unsigned int)(yyyylsp[0]).first_line));
break;}case 480: _LL42B: {
# 3074
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3076
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3073 "parse.y"
struct _tuple20 _tmp827=Cyc_yyget_Asm_tok(yyyyvsp[0]);int _tmp829;struct _dyneither_ptr _tmp82A;struct _tuple20 _tmp828=_tmp827;_tmp829=_tmp828.f1;_tmp82A=_tmp828.f2;
yyval=Cyc_Exp_tok(Cyc_Absyn_asm_exp(_tmp829,_tmp82A,(unsigned int)(yyyylsp[0]).first_line));
break;}case 481: _LL42C: {
# 3077
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3079
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3078 "parse.y"
yyval=Cyc_YY6(Cyc_Absyn_Bitnot);
break;}case 482: _LL42D: {
# 3081
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3083
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3079 "parse.y"
yyval=Cyc_YY6(Cyc_Absyn_Not);
break;}case 483: _LL42E: {
# 3082
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3084
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3080 "parse.y"
yyval=Cyc_YY6(Cyc_Absyn_Minus);
break;}case 484: _LL42F: {
# 3083
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3085
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3085 "parse.y"
yyval=yyyyvsp[0];
break;}case 485: _LL430: {
# 3088
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3090
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3087 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_subscript_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_Exp_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 486: _LL431: {
# 3090
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3092
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3089 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 487: _LL432: {
# 3092
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3094
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3091 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_unknowncall_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 488: _LL433: {
# 3094
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3096
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3093 "parse.y"
{struct _dyneither_ptr*_tmpEFA;yyval=Cyc_Exp_tok(Cyc_Absyn_aggrmember_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),((_tmpEFA=_cycalloc(sizeof(*_tmpEFA)),((_tmpEFA[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpEFA)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 489: _LL434: {
# 3096
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3098
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3095 "parse.y"
{struct _dyneither_ptr*_tmpEFB;yyval=Cyc_Exp_tok(Cyc_Absyn_aggrarrow_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),((_tmpEFB=_cycalloc(sizeof(*_tmpEFB)),((_tmpEFB[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpEFB)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 490: _LL435: {
# 3098
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3100
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3097 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_post_inc_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 491: _LL436: {
# 3100
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3102
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3099 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_post_dec_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 492: _LL437: {
# 3102
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3104
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3101 "parse.y"
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpEFE;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpEFD;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpEFD=_cycalloc(sizeof(*_tmpEFD)),((_tmpEFD[0]=((_tmpEFE.tag=24,((_tmpEFE.f1=Cyc_yyget_YY37(yyyyvsp[1]),((_tmpEFE.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[4])),_tmpEFE)))))),_tmpEFD)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 493: _LL438: {
# 3104
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 3106
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 3103 "parse.y"
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpF01;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpF00;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpF00=_cycalloc(sizeof(*_tmpF00)),((_tmpF00[0]=((_tmpF01.tag=24,((_tmpF01.f1=Cyc_yyget_YY37(yyyyvsp[1]),((_tmpF01.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[4])),_tmpF01)))))),_tmpF00)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 494: _LL439: {
# 3106
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3108
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3108 "parse.y"
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpF0B;struct _dyneither_ptr*_tmpF0A;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpF09;struct Cyc_List_List*_tmpF08;yyval=Cyc_YY3(((_tmpF08=_cycalloc(sizeof(*_tmpF08)),((_tmpF08->hd=(void*)((_tmpF0B=_cycalloc(sizeof(*_tmpF0B)),((_tmpF0B[0]=((_tmpF09.tag=0,((_tmpF09.f1=((_tmpF0A=_cycalloc(sizeof(*_tmpF0A)),((_tmpF0A[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpF0A)))),_tmpF09)))),_tmpF0B)))),((_tmpF08->tl=0,_tmpF08)))))));}
break;}case 495: _LL43A: {
# 3111
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3113
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3111 "parse.y"
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpF11;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpF10;struct Cyc_List_List*_tmpF0F;yyval=Cyc_YY3(((_tmpF0F=_cycalloc(sizeof(*_tmpF0F)),((_tmpF0F->hd=(void*)((_tmpF11=_cycalloc_atomic(sizeof(*_tmpF11)),((_tmpF11[0]=((_tmpF10.tag=1,((_tmpF10.f1=Cyc_Parse_cnst2uint((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_Int_tok(yyyyvsp[0])),_tmpF10)))),_tmpF11)))),((_tmpF0F->tl=0,_tmpF0F)))))));}
break;}case 496: _LL43B: {
# 3114
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3116
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3113 "parse.y"
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpF1B;struct _dyneither_ptr*_tmpF1A;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpF19;struct Cyc_List_List*_tmpF18;yyval=Cyc_YY3(((_tmpF18=_cycalloc(sizeof(*_tmpF18)),((_tmpF18->hd=(void*)((_tmpF1B=_cycalloc(sizeof(*_tmpF1B)),((_tmpF1B[0]=((_tmpF19.tag=0,((_tmpF19.f1=((_tmpF1A=_cycalloc(sizeof(*_tmpF1A)),((_tmpF1A[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpF1A)))),_tmpF19)))),_tmpF1B)))),((_tmpF18->tl=Cyc_yyget_YY3(yyyyvsp[0]),_tmpF18)))))));}
break;}case 497: _LL43C: {
# 3116
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3118
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3116 "parse.y"
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpF21;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpF20;struct Cyc_List_List*_tmpF1F;yyval=Cyc_YY3(((_tmpF1F=_cycalloc(sizeof(*_tmpF1F)),((_tmpF1F->hd=(void*)((_tmpF21=_cycalloc_atomic(sizeof(*_tmpF21)),((_tmpF21[0]=((_tmpF20.tag=1,((_tmpF20.f1=Cyc_Parse_cnst2uint((unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_Int_tok(yyyyvsp[2])),_tmpF20)))),_tmpF21)))),((_tmpF1F->tl=Cyc_yyget_YY3(yyyyvsp[0]),_tmpF1F)))))));}
break;}case 498: _LL43D: {
# 3119
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3121
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=Cyc_Exp_tok(Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 499: _LL43E: {
# 3125
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3127
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3124 "parse.y"
yyval=yyyyvsp[0];
break;}case 500: _LL43F: {
# 3127
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3129
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3126 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 501: _LL440: {
# 3129
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3131
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3128 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_wstring_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 502: _LL441: {
# 3131
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3133
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3130 "parse.y"
yyval=yyyyvsp[1];
break;}case 503: _LL442: {
# 3133
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3135
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3135 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_noinstantiate_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 504: _LL443: {
# 3138
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3140
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3137 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_instantiate_exp(Cyc_yyget_Exp_tok(yyyyvsp[0]),((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[3])),(unsigned int)(yyyylsp[0]).first_line));
break;}case 505: _LL444: {
# 3140
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3142
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3140 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_tuple_exp(Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 506: _LL445: {
# 3143
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3145
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3143 "parse.y"
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpF24;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpF23;yyval=Cyc_Exp_tok(Cyc_Absyn_new_exp((void*)((_tmpF23=_cycalloc(sizeof(*_tmpF23)),((_tmpF23[0]=((_tmpF24.tag=28,((_tmpF24.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpF24.f2=Cyc_yyget_YY40(yyyyvsp[2]),((_tmpF24.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[3])),((_tmpF24.f4=0,_tmpF24)))))))))),_tmpF23)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 507: _LL446: {
# 3146
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3148
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3146 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_stmt_exp(Cyc_yyget_Stmt_tok(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 508: _LL447: {
# 3149
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3151
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3150 "parse.y"
yyval=Cyc_YY4(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY4(yyyyvsp[0])));
break;}case 509: _LL448: {
# 3153
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3155
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpF25;yyval=Cyc_YY4(((_tmpF25=_cycalloc(sizeof(*_tmpF25)),((_tmpF25->hd=Cyc_yyget_Exp_tok(yyyyvsp[0]),((_tmpF25->tl=0,_tmpF25)))))));}
break;}case 510: _LL449: {
# 3159
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3161
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3158 "parse.y"
{struct Cyc_List_List*_tmpF26;yyval=Cyc_YY4(((_tmpF26=_cycalloc(sizeof(*_tmpF26)),((_tmpF26->hd=Cyc_yyget_Exp_tok(yyyyvsp[2]),((_tmpF26->tl=Cyc_yyget_YY4(yyyyvsp[0]),_tmpF26)))))));}
break;}case 511: _LL44A: {
# 3161
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3163
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=Cyc_Exp_tok(Cyc_Absyn_const_exp(Cyc_yyget_Int_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 512: _LL44B: {
# 3167
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3169
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3165 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 513: _LL44C: {
# 3168
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3170
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3166 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_wchar_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 514: _LL44D: {
# 3169
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3171
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3168 "parse.y"
struct _dyneither_ptr _tmp843=Cyc_yyget_String_tok(yyyyvsp[0]);
int l=(int)Cyc_strlen((struct _dyneither_ptr)_tmp843);
int i=1;
if(l > 0){
char c=*((const char*)_check_dyneither_subscript(_tmp843,sizeof(char),l - 1));
if(c == 'f'  || c == 'F')i=0;else{
if(c == 'l'  || c == 'L')i=2;}}
# 3176
yyval=Cyc_Exp_tok(Cyc_Absyn_float_exp(_tmp843,i,(unsigned int)(yyyylsp[0]).first_line));
# 3178
break;}case 515: _LL44E: {
# 3180
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3182
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3179 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_null_exp((unsigned int)(yyyylsp[0]).first_line));
break;}case 516: _LL44F: {
# 3182
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3184
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3183 "parse.y"
{struct _dyneither_ptr*_tmpF29;struct _tuple0*_tmpF28;yyval=Cyc_QualId_tok(((_tmpF28=_cycalloc(sizeof(*_tmpF28)),((_tmpF28->f1=Cyc_Absyn_Rel_n(0),((_tmpF28->f2=((_tmpF29=_cycalloc(sizeof(*_tmpF29)),((_tmpF29[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpF29)))),_tmpF28)))))));}
break;}case 517: _LL450: {
# 3186
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3188
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3184 "parse.y"
yyval=yyyyvsp[0];
break;}case 518: _LL451: {
# 3187
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3189
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3187 "parse.y"
{struct _dyneither_ptr*_tmpF2C;struct _tuple0*_tmpF2B;yyval=Cyc_QualId_tok(((_tmpF2B=_cycalloc(sizeof(*_tmpF2B)),((_tmpF2B->f1=Cyc_Absyn_Rel_n(0),((_tmpF2B->f2=((_tmpF2C=_cycalloc(sizeof(*_tmpF2C)),((_tmpF2C[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpF2C)))),_tmpF2B)))))));}
break;}case 519: _LL452: {
# 3190
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3192
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3188 "parse.y"
yyval=yyyyvsp[0];
break;}case 520: _LL453: {
# 3191
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3193
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3193 "parse.y"
yyval=yyyyvsp[0];
break;}case 521: _LL454: {
# 3196
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3198
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3194 "parse.y"
yyval=yyyyvsp[0];
break;}case 522: _LL455: {
# 3197
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3199
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3197 "parse.y"
yyval=yyyyvsp[0];
break;}case 523: _LL456: {
# 3200
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3202
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3198 "parse.y"
yyval=yyyyvsp[0];
break;}case 524: _LL457: {
# 3201
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3203
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3203 "parse.y"
break;}case 525: _LL458: {
# 3205
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3207
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3203 "parse.y"
yylex_buf->lex_curr_pos -=1;
break;}default: _LL459:
# 3208
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
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(526,yyn)];
# 413
yystate=Cyc_yypgoto[_check_known_subscript_notnull(148,yyn - 145)]+ *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset));
if((yystate >= 0  && yystate <= 6748) && Cyc_yycheck[_check_known_subscript_notnull(6749,yystate)]== *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset)))
yystate=(int)Cyc_yytable[_check_known_subscript_notnull(6749,yystate)];else{
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
yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1064,yystate)];
# 431
if(yyn > - 32768  && yyn < 6748){
# 433
int sze=0;
struct _dyneither_ptr msg;
int x;int count;
# 437
count=0;
# 439
for(x=yyn < 0?- yyn: 0;x < 293 / sizeof(char*);++ x){
# 441
if(Cyc_yycheck[_check_known_subscript_notnull(6749,x + yyn)]== x)
(sze +=Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(293,x)])+ 15,count ++);}
{unsigned int _tmpF31;unsigned int _tmpF30;struct _dyneither_ptr _tmpF2F;char*_tmpF2E;unsigned int _tmpF2D;msg=((_tmpF2D=(unsigned int)(sze + 15),((_tmpF2E=(char*)_region_malloc(yyregion,_check_times(sizeof(char),_tmpF2D + 1)),((_tmpF2F=_tag_dyneither(_tmpF2E,sizeof(char),_tmpF2D + 1),((((_tmpF30=_tmpF2D,((_tmpF32(& _tmpF31,& _tmpF30,& _tmpF2E),_tmpF2E[_tmpF30]=(char)0)))),_tmpF2F))))))));}
{const char*_tmpF33;Cyc_strcpy(msg,((_tmpF33="parse error",_tag_dyneither(_tmpF33,sizeof(char),12))));}
# 446
if(count < 5){
# 448
count=0;
for(x=yyn < 0?- yyn: 0;x < 293 / sizeof(char*);++ x){
# 451
if(Cyc_yycheck[_check_known_subscript_notnull(6749,x + yyn)]== x){
# 453
{const char*_tmpF35;const char*_tmpF34;Cyc_strcat(msg,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)(
(_tmpF35=", expecting `",_tag_dyneither(_tmpF35,sizeof(char),14))):(struct _dyneither_ptr)(
(_tmpF34=" or `",_tag_dyneither(_tmpF34,sizeof(char),6)))));}
Cyc_strcat(msg,(struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(293,x)]);
{const char*_tmpF36;Cyc_strcat(msg,((_tmpF36="'",_tag_dyneither(_tmpF36,sizeof(char),2))));}
++ count;}}}
# 461
Cyc_yyerror((struct _dyneither_ptr)msg,yystate,yychar);}else{
# 465
const char*_tmpF37;Cyc_yyerror(((_tmpF37="parse error",_tag_dyneither(_tmpF37,sizeof(char),12))),yystate,yychar);}}
# 467
goto yyerrlab1;
# 469
yyerrlab1:
# 471
 if(yyerrstatus == 3){
# 476
if(yychar == 0){
int _tmp851=1;_npop_handler(0);return _tmp851;}
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
 if(yyssp_offset == 0){int _tmp852=1;_npop_handler(0);return _tmp852;}
-- yyvsp_offset;
yystate=(int)*((short*)_check_dyneither_subscript(yyss,sizeof(short),-- yyssp_offset));
# 511
-- yylsp_offset;
# 526 "cyc_bison.simple"
yyerrhandle:
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1064,yystate)];
if(yyn == - 32768)goto yyerrdefault;
# 530
yyn +=1;
if((yyn < 0  || yyn > 6748) || Cyc_yycheck[_check_known_subscript_notnull(6749,yyn)]!= 1)goto yyerrdefault;
# 533
yyn=(int)Cyc_yytable[_check_known_subscript_notnull(6749,yyn)];
if(yyn < 0){
# 536
if(yyn == - 32768)goto yyerrpop;
yyn=- yyn;
goto yyreduce;}else{
# 540
if(yyn == 0)goto yyerrpop;}
# 542
if(yyn == 1063){
int _tmp853=0;_npop_handler(0);return _tmp853;}
# 550
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yylval;
# 552
*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),++ yylsp_offset))=yylloc;
# 555
goto yynewstate;}
# 141 "cycbison.simple"
;_pop_region(yyregion);}
# 3206 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
union Cyc_YYSTYPE _tmp85D=v;union Cyc_Absyn_Cnst _tmp85E;char _tmp85F;struct _dyneither_ptr _tmp860;union Cyc_Absyn_Nmspace _tmp861;struct _dyneither_ptr*_tmp862;struct Cyc_Absyn_Exp*_tmp863;struct Cyc_Absyn_Stmt*_tmp864;_LL45C: if((_tmp85D.Int_tok).tag != 1)goto _LL45E;_tmp85E=(union Cyc_Absyn_Cnst)(_tmp85D.Int_tok).val;_LL45D:
{const char*_tmpF3B;void*_tmpF3A[1];struct Cyc_String_pa_PrintArg_struct _tmpF39;(_tmpF39.tag=0,((_tmpF39.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_cnst2string(_tmp85E)),((_tmpF3A[0]=& _tmpF39,Cyc_fprintf(Cyc_stderr,((_tmpF3B="%s",_tag_dyneither(_tmpF3B,sizeof(char),3))),_tag_dyneither(_tmpF3A,sizeof(void*),1)))))));}goto _LL45B;_LL45E: if((_tmp85D.Char_tok).tag != 2)goto _LL460;_tmp85F=(char)(_tmp85D.Char_tok).val;_LL45F:
{const char*_tmpF3F;void*_tmpF3E[1];struct Cyc_Int_pa_PrintArg_struct _tmpF3D;(_tmpF3D.tag=1,((_tmpF3D.f1=(unsigned long)((int)_tmp85F),((_tmpF3E[0]=& _tmpF3D,Cyc_fprintf(Cyc_stderr,((_tmpF3F="%c",_tag_dyneither(_tmpF3F,sizeof(char),3))),_tag_dyneither(_tmpF3E,sizeof(void*),1)))))));}goto _LL45B;_LL460: if((_tmp85D.String_tok).tag != 3)goto _LL462;_tmp860=(struct _dyneither_ptr)(_tmp85D.String_tok).val;_LL461:
{const char*_tmpF43;void*_tmpF42[1];struct Cyc_String_pa_PrintArg_struct _tmpF41;(_tmpF41.tag=0,((_tmpF41.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp860),((_tmpF42[0]=& _tmpF41,Cyc_fprintf(Cyc_stderr,((_tmpF43="\"%s\"",_tag_dyneither(_tmpF43,sizeof(char),5))),_tag_dyneither(_tmpF42,sizeof(void*),1)))))));}goto _LL45B;_LL462: if((_tmp85D.QualId_tok).tag != 5)goto _LL464;_tmp861=((struct _tuple0*)(_tmp85D.QualId_tok).val)->f1;_tmp862=((struct _tuple0*)(_tmp85D.QualId_tok).val)->f2;_LL463: {
# 3212
struct Cyc_List_List*_tmp86E=0;
{union Cyc_Absyn_Nmspace _tmp86F=_tmp861;struct Cyc_List_List*_tmp870;struct Cyc_List_List*_tmp871;struct Cyc_List_List*_tmp872;_LL46B: if((_tmp86F.Rel_n).tag != 1)goto _LL46D;_tmp870=(struct Cyc_List_List*)(_tmp86F.Rel_n).val;_LL46C:
 _tmp86E=_tmp870;goto _LL46A;_LL46D: if((_tmp86F.Abs_n).tag != 2)goto _LL46F;_tmp871=(struct Cyc_List_List*)(_tmp86F.Abs_n).val;_LL46E:
 _tmp86E=_tmp871;goto _LL46A;_LL46F: if((_tmp86F.C_n).tag != 3)goto _LL471;_tmp872=(struct Cyc_List_List*)(_tmp86F.C_n).val;_LL470:
 _tmp86E=_tmp872;goto _LL46A;_LL471: if((_tmp86F.Loc_n).tag != 4)goto _LL46A;_LL472:
 goto _LL46A;_LL46A:;}
# 3219
for(0;_tmp86E != 0;_tmp86E=_tmp86E->tl){
const char*_tmpF47;void*_tmpF46[1];struct Cyc_String_pa_PrintArg_struct _tmpF45;(_tmpF45.tag=0,((_tmpF45.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp86E->hd)),((_tmpF46[0]=& _tmpF45,Cyc_fprintf(Cyc_stderr,((_tmpF47="%s::",_tag_dyneither(_tmpF47,sizeof(char),5))),_tag_dyneither(_tmpF46,sizeof(void*),1)))))));}
{const char*_tmpF4B;void*_tmpF4A[1];struct Cyc_String_pa_PrintArg_struct _tmpF49;(_tmpF49.tag=0,((_tmpF49.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp862),((_tmpF4A[0]=& _tmpF49,Cyc_fprintf(Cyc_stderr,((_tmpF4B="%s::",_tag_dyneither(_tmpF4B,sizeof(char),5))),_tag_dyneither(_tmpF4A,sizeof(void*),1)))))));}
goto _LL45B;}_LL464: if((_tmp85D.Exp_tok).tag != 7)goto _LL466;_tmp863=(struct Cyc_Absyn_Exp*)(_tmp85D.Exp_tok).val;_LL465:
{const char*_tmpF4F;void*_tmpF4E[1];struct Cyc_String_pa_PrintArg_struct _tmpF4D;(_tmpF4D.tag=0,((_tmpF4D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp863)),((_tmpF4E[0]=& _tmpF4D,Cyc_fprintf(Cyc_stderr,((_tmpF4F="%s",_tag_dyneither(_tmpF4F,sizeof(char),3))),_tag_dyneither(_tmpF4E,sizeof(void*),1)))))));}goto _LL45B;_LL466: if((_tmp85D.Stmt_tok).tag != 8)goto _LL468;_tmp864=(struct Cyc_Absyn_Stmt*)(_tmp85D.Stmt_tok).val;_LL467:
{const char*_tmpF53;void*_tmpF52[1];struct Cyc_String_pa_PrintArg_struct _tmpF51;(_tmpF51.tag=0,((_tmpF51.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(_tmp864)),((_tmpF52[0]=& _tmpF51,Cyc_fprintf(Cyc_stderr,((_tmpF53="%s",_tag_dyneither(_tmpF53,sizeof(char),3))),_tag_dyneither(_tmpF52,sizeof(void*),1)))))));}goto _LL45B;_LL468:;_LL469:
{const char*_tmpF56;void*_tmpF55;(_tmpF55=0,Cyc_fprintf(Cyc_stderr,((_tmpF56="?",_tag_dyneither(_tmpF56,sizeof(char),2))),_tag_dyneither(_tmpF55,sizeof(void*),0)));}goto _LL45B;_LL45B:;}
# 3230
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp881=_new_region("yyr");struct _RegionHandle*yyr=& _tmp881;_push_region(yyr);
Cyc_yyparse(yyr,Cyc_Lexing_from_file(f));{
struct Cyc_List_List*_tmp882=Cyc_Parse_parse_result;_npop_handler(0);return _tmp882;};
# 3233
;_pop_region(yyr);};}
